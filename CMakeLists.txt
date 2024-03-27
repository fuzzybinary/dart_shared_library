cmake_minimum_required(VERSION 3.21)

project(DartSharedLibrary VERSION 0.1)

option(BUILD_SMAPLES "Build the Sampels" ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(DART_DLL_DIR "${PROJECT_SOURCE_DIR}/src")
set(DART_DIR "${PROJECT_SOURCE_DIR}/dart-sdk/sdk")

add_subdirectory(src)
if(BUILD_SAMPLES)
    add_subdirectory(examples)
endif()