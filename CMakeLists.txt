cmake_minimum_required(VERSION 3.3.0)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(meow VERSION 1.0.0)

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

#add_subdirectory(external/evpp)

get_cmake_property(_variableNames VARIABLES)
list (SORT _variableNames)
foreach (_variableName ${_variableNames})
    message(STATUS "${_variableName}=${${_variableName}}")
endforeach()

add_compile_definitions(meow PUBLIC WIN32_LEAN_AND_MEAN)

#####
include(cmake/CPM.cmake)
find_package(Threads REQUIRED)

CPMAddPackage("gh:chriskohlhoff/asio#asio-1-28-0@1.28.0")

# ASIO doesn't use CMake, we have to configure it manually. Extra notes for using on Windows:
#
# 1) If _WIN32_WINNT is not set, ASIO assumes _WIN32_WINNT=0x0501, i.e. Windows XP target, which is
# definitely not the platform which most users target.
#
# 2) WIN32_LEAN_AND_MEAN is defined to make Winsock2 work.
if(asio_ADDED)
    add_library(asio INTERFACE)

    target_include_directories(asio SYSTEM INTERFACE ${asio_SOURCE_DIR}/asio/include)

    target_compile_definitions(asio INTERFACE ASIO_STANDALONE ASIO_NO_DEPRECATED)

    target_link_libraries(asio INTERFACE Threads::Threads)

    if(WIN32)
        target_compile_definitions(asio INTERFACE WIN32_LEAN_AND_MEAN )
    endif()
endif()

#####

# Set up the mod binary
add_library(${PROJECT_NAME} SHARED ${SOURCES} src/global.cpp)
target_include_directories(${PROJECT_NAME} PUBLIC include)
target_link_libraries(${PROJECT_NAME} asio)

if (NOT DEFINED ENV{GEODE_SDK})
    message(FATAL_ERROR "Unable to find Geode SDK! Please define GEODE_SDK environment variable to point to Geode")
else()
    message(STATUS "Found Geode: $ENV{GEODE_SDK}")
endif()

add_subdirectory($ENV{GEODE_SDK} $ENV{GEODE_SDK}/build)

# Set up dependencies, resources, link Geode
setup_geode_mod(${PROJECT_NAME})