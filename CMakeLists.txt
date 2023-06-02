cmake_minimum_required(VERSION 3.3.0)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(meow VERSION 1.0.0)

# Use GLOB_RECURSE instead of GLOB
# to recursively add all source files 
# under src/ 
file(GLOB SOURCES
	src/*.cpp
)

# i have no idea what i'm doing but this seems to work lmao
if(CMAKE_CROSSCOMPILING)
    message(STATUS "cross compiling - using bundled protobuf things")
    # gl getting these, took me a few hours ;3
    # protobuf version: 3.21.12
    set(Protobuf_LIBRARIES ${PROJECT_SOURCE_DIR}/protobuf_win32/build/libs)
    set(Protobuf_LITE_LIBRARY_RELEASE ${PROJECT_SOURCE_DIR}/protobuf_win32/build/libs/libprotobuf-lite.lib)
    set(Protobuf_LIBRARY  ${PROJECT_SOURCE_DIR}/protobuf_win32/build/libs/libprotobuf.lib)
    set(Protobuf_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/protobuf_win32/src/protobuf/src)

endif()

find_package(Protobuf REQUIRED)

add_compile_definitions(WIN32_LEAN_AND_MEAN)
add_subdirectory(external/enet)
# Set up the mod binary
add_library(${PROJECT_NAME} SHARED ${SOURCES} src/global.cpp proto/packet.proto)
protobuf_generate(TARGET ${PROJECT_NAME} LANGUAGE cpp)

target_include_directories(${PROJECT_NAME} PUBLIC include ${CMAKE_CURRENT_BINARY_DIR} external/enet/include ${Protobuf_INCLUDE_DIRS} ${CMAKE_CURRENT_BINARY_DIR}/proto) #${Protobuf_INCLUDE_DIRS}

if (NOT DEFINED ENV{GEODE_SDK})
    message(FATAL_ERROR "Unable to find Geode SDK! Please define GEODE_SDK environment variable to point to Geode")
else()
    message(STATUS "Found Geode: $ENV{GEODE_SDK}")
endif()

add_subdirectory($ENV{GEODE_SDK} $ENV{GEODE_SDK}/build)

# Set up dependencies, resources, link Geode
setup_geode_mod(${PROJECT_NAME})
message(STATUS "PROTOBUF LIB: ${PROTOBUF_LIBRARY}")
target_link_libraries(${PROJECT_NAME} enet ${PROTOBUF_LIBRARY})
