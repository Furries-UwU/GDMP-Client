cmake_minimum_required(VERSION 3.14 FATAL_ERROR)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(GDMP VERSION 1.0.0)

option(USE_PREBUILT_LIBS "Only for rooot lmao" OFF)

include(cmake/CPM.cmake)

if (WIN32 OR MINGW)
    add_compile_definitions(WIN32_LEAN_AND_MEAN)
endif ()

if (USE_PREBUILT_LIBS)
    set(Protobuf_LIBRARIES ${CMAKE_CURRENT_SOURCE_DIR}/prebuilt_protobuf/win32)
    set(Protobuf_LITE_LIBRARY_RELEASE ${CMAKE_CURRENT_SOURCE_DIR}/prebuilt_protobuf/win32/libprotobuf-lite.lib)
    set(Protobuf_LIBRARY ${CMAKE_CURRENT_SOURCE_DIR}/prebuilt_protobuf/win32/libprotobuf.lib)
    set(Protobuf_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/prebuilt_protobuf/src/src)
elseif (WIN32 OR MINGW)
    set(USE_PROTOBUF_CPM ON)
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(CMAKE_OSX_ARCHITECTURES x86_64)
    set(USE_PROTOBUF_CPM ON)
endif ()

if (USE_PROTOBUF_CPM)
    CPMAddPackage(
            NAME protobuf
            VERSION 23.1
            GITHUB_REPOSITORY protocolbuffers/protobuf
            OPTIONS "BUILD_SHARED_LIBS OFF" "protobuf_BUILD_TESTS OFF" "protobuf_BUILD_LIBPROTOC ON")

    include(${protobuf_SOURCE_DIR}/cmake/protobuf-generate.cmake)

    set(PROTOBUF_LIBRARY libprotobuf)
    set(Protobuf_INCLUDE_DIRS ${protobuf_SOURCE_DIR}/src ${protobuf_SOURCE_DIR}/third_party/abseil-cpp)
endif ()

if (NOT protobuf_ADDED)
    find_package(Protobuf REQUIRED)
endif ()

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS src/*.cpp src/*.c)

add_subdirectory(external/enet)
add_library(${PROJECT_NAME} SHARED ${SOURCES} proto/packet.proto)
protobuf_generate(TARGET ${PROJECT_NAME} LANGUAGE cpp)

target_include_directories(${PROJECT_NAME} PUBLIC include ${CMAKE_CURRENT_BINARY_DIR} external/enet/include ${Protobuf_INCLUDE_DIRS} ${CMAKE_CURRENT_BINARY_DIR}/proto)

if (NOT DEFINED ENV{GEODE_SDK})
    message(FATAL_ERROR "Unable to find Geode SDK! Please define GEODE_SDK environment variable to point to Geode")
else ()
    message(STATUS "Found Geode: $ENV{GEODE_SDK}")
endif ()

add_subdirectory($ENV{GEODE_SDK} $ENV{GEODE_SDK}/build)

# Set up dependencies, resources, link Geode
setup_geode_mod(${PROJECT_NAME})
target_link_libraries(${PROJECT_NAME} enet ${PROTOBUF_LIBRARY})
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)