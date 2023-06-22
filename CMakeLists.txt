cmake_minimum_required(VERSION 3.21)

project(Tutorial VERSION 0.1)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(DART_DLL_DIR "${PROJECT_SOURCE_DIR}/src")
set(DART_DIR "${PROJECT_SOURCE_DIR}/dart-sdk/sdk")

add_subdirectory(src)
add_subdirectory(examples)