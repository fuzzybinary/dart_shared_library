cmake_minimum_required(VERSION 3.21)

project(realtime_example)
set(CMAKE_CXX_STANDARD 17)

set(CUTE_FRAMEWORK_STATIC ON)
set(CF_FRAMEWORK_BUILD_TESTS OFF)
# Samples Build on Windows Falied
set(CF_FRAMEWORK_BUILD_SAMPLES OFF)

include(FetchContent)
FetchContent_Declare(
	cute
	GIT_REPOSITORY https://github.com/RandyGaul/cute_framework
)
FetchContent_MakeAvailable(cute)

add_executable(realtime_example
	main.cpp
	drawable.cpp
	exportfunc.cpp
)

target_include_directories(realtime_example PRIVATE
	"."
    "${DART_DLL_DIR}"
    "${DART_DIR}/runtime/include"
)

add_custom_target(ALWAYS_DO_POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy -t $<TARGET_FILE_DIR:realtime_example> $<TARGET_RUNTIME_DLLS:simple_example_ffi>
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/dart $<TARGET_FILE_DIR:realtime_example>/dart
  COMMAND_EXPAND_LISTS
)
add_dependencies(realtime_example ALWAYS_DO_POST_BUILD)

target_link_libraries(realtime_example PUBLIC dart_dll cute)

if (MSVC)
	set_property(TARGET realtime_example PROPERTY VS_DEBUGGER_WORKING_DIRECTORY $<TARGET_FILE_DIR:realtime_example>)	
endif()

set(EXECUTABLE_OUTPUT_PATH "${EXECUTABLE_OUTPUT_PATH}/realtime_example")