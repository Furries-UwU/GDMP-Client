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

if(CMAKE_CROSSCOMPILING)
    message(STATUS "cross compiling - using prebuilt protobuf libs")
    set(Protobuf_LIBRARIES ${CMAKE_ROOT}/protobuf_win32/src/protobuf/build)
endif()

find_package(Protobuf REQUIRED)

add_compile_definitions(WIN32_LEAN_AND_MEAN)
add_subdirectory(external/enet)
# Set up the mod binary
add_library(${PROJECT_NAME} SHARED ${SOURCES} src/global.cpp proto/packet.proto)

file(GLOB PROTOBUF_DEFINITION_FILES "proto/*.proto")
set(PROTOBUF_INPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/proto")
set(PROTOBUF_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
foreach(file ${PROTOBUF_DEFINITION_FILES})
    message(STATUS "bruhh")
    set(PROTOBUF_ARGUMENTS "protoc --proto_path=\"${PROTOBUF_INPUT_DIRECTORY}\" --cpp_out=\"${PROTOBUF_OUTPUT_DIRECTORY}\" \"${file}\"")
    message(STATUS ${PROTOBUF_ARGUMENTS})
    execute_process(
            COMMAND protoc --proto_path=${PROTOBUF_INPUT_DIRECTORY} --cpp_out=${PROTOBUF_OUTPUT_DIRECTORY} "${file}"

            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            RESULT_VARIABLE PROTOBUF_RESULT
            OUTPUT_VARIABLE PROTOBUF_OUTPUT_VARIABLE)
    message(STATUS "RESULT" ${PROTOBUF_RESULT})
    message(STATUS "OUTPUT" ${PROTOBUF_OUTPUT_VARIABLE})
endforeach()

file(GLOB PROTOBUF_MODELS_INCLUDES "proto_out/*.pb.cc" "proto_out/*.hpp")

target_include_directories(${PROJECT_NAME} PUBLIC include ${CMAKE_CURRENT_BINARY_DIR} external/enet/include) #${Protobuf_INCLUDE_DIRS}

if (NOT DEFINED ENV{GEODE_SDK})
    message(FATAL_ERROR "Unable to find Geode SDK! Please define GEODE_SDK environment variable to point to Geode")
else()
    message(STATUS "Found Geode: $ENV{GEODE_SDK}")
endif()

add_subdirectory($ENV{GEODE_SDK} $ENV{GEODE_SDK}/build)

# Set up dependencies, resources, link Geode
setup_geode_mod(${PROJECT_NAME})
target_link_libraries(${PROJECT_NAME} enet)