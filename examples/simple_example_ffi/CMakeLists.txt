cmake_minimum_required(VERSION 3.21)

project(simple_example_ffi)

add_executable(simple_example_ffi main.cpp)

target_include_directories(simple_example_ffi PRIVATE
    "${DART_DLL_DIR}"
    "${DART_DIR}/runtime/include"
)

add_custom_command(TARGET simple_example_ffi POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy -t $<TARGET_FILE_DIR:simple_example_ffi> $<TARGET_RUNTIME_DLLS:simple_example_ffi>
  COMMAND ${CMAKE_COMMAND} -E copy -t $<TARGET_FILE_DIR:simple_example_ffi> ${PROJECT_SOURCE_DIR}/hello_world_ffi.dart
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/.dart_tool $<TARGET_FILE_DIR:simple_example_ffi>/.dart_tool
  COMMAND_EXPAND_LISTS
)

target_link_libraries(simple_example_ffi PUBLIC dart_dll)

if (MSVC)
	set_property(TARGET simple_example_ffi PROPERTY VS_DEBUGGER_WORKING_DIRECTORY $<TARGET_FILE_DIR:simple_example_ffi>)	
endif()

if(${CMAKE_SYSTEM_NAME} STREQUAL "Windows" )
    SET (project_BIN ${PROJECT_NAME})
else()
    SET (project_BIN ${PROJECT_NAME}.bin)
endif()

set(EXECUTABLE_OUTPUT_PATH "${EXECUTABLE_OUTPUT_PATH}/simple_example_ffi")