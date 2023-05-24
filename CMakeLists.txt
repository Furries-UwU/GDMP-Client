cmake_minimum_required(VERSION 3.3.0)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(meow VERSION 1.0.0)

## uh oh~
#include(FetchContent)
#
#if (WIN32)
#    if(CMAKE_CROSSCOMPILING)
#        set(Rust_CARGO_TARGET i686-pc-windows-gnu)
#        set(Rust_TOOLCHAIN nightly-x86_64-unknown-linux-gnu)
#    endif()
#endif()
#
#FetchContent_Declare(
#        Corrosion
#        GIT_REPOSITORY https://github.com/corrosion-rs/corrosion.git
#        GIT_TAG bd196a3 # Optionally specify a commit hash, version tag or branch here
#)
#FetchContent_MakeAvailable(Corrosion)

#include(cmake/corrosion_cxx.cmake)

# Use GLOB_RECURSE instead of GLOB
# to recursively add all source files 
# under src/ 
file(GLOB SOURCES
	src/*.cpp
)

#corrosion_import_crate(MANIFEST_PATH thingy/Cargo.toml)
#corrosion_add_cxxbridge(bridge CRATE thingy MANIFEST_PATH thingy FILES lib.rs)

# Set up the mod binary
add_library(${PROJECT_NAME} SHARED ${SOURCES} src/global.cpp)
target_include_directories(${PROJECT_NAME} PUBLIC include)
#target_link_libraries(${PROJECT_NAME} bridge)

if (NOT DEFINED ENV{GEODE_SDK})
    message(FATAL_ERROR "Unable to find Geode SDK! Please define GEODE_SDK environment variable to point to Geode")
else()
    message(STATUS "Found Geode: $ENV{GEODE_SDK}")
endif()

add_subdirectory($ENV{GEODE_SDK} $ENV{GEODE_SDK}/build)

# Set up dependencies, resources, link Geode
setup_geode_mod(${PROJECT_NAME})