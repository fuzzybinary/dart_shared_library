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
	GIT_TAG        a2341c72f02e019e157cfc3997cd81d98c825004
)
FetchContent_MakeAvailable(cute)

add_executable(realtime_example
	main.cpp
	drawable.cpp
)

target_include_directories(realtime_example PRIVATE
	"."
    "${DART_DLL_DIR}"
    "${DART_DIR}/runtime/include"
)

if(LINUX)
	# -export-dynamic is required on Linux if your symbols are in your executable,
	# otherwise dlsym (and therefore Dart FFI) can't find them.
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -export-dynamic")
endif()

if (WIN32)
	add_custom_command(TARGET realtime_example POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:realtime_example> $<TARGET_FILE_DIR:realtime_example>
		COMMAND_EXPAND_LISTS
	)
endif()

add_custom_command(TARGET realtime_example POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/dart/ $<TARGET_FILE_DIR:realtime_example>/dart
  COMMAND_EXPAND_LISTS
)

target_link_libraries(realtime_example PUBLIC dart_dll cute)

if (MSVC)
	set_property(TARGET realtime_example PROPERTY VS_DEBUGGER_WORKING_DIRECTORY $<TARGET_FILE_DIR:realtime_example>)	
endif()
