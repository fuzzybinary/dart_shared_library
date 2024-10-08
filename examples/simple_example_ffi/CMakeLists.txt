cmake_minimum_required(VERSION 3.21)

project(simple_example_ffi)

add_executable(simple_example_ffi main.cpp)

target_include_directories(simple_example_ffi PRIVATE
    "${DART_DLL_DIR}"
    "${DART_DIR}/runtime/include"
)

if(WIN32)
  add_custom_command(TARGET simple_example_ffi POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:simple_example_ffi> $<TARGET_FILE_DIR:simple_example_ffi>
    COMMAND_EXPAND_LISTS
  )
endif()

add_custom_command(TARGET simple_example_ffi POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/hello_world_ffi.dart $<TARGET_FILE_DIR:simple_example_ffi>
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/.dart_tool $<TARGET_FILE_DIR:simple_example_ffi>/.dart_tool
  COMMAND_EXPAND_LISTS
)

target_link_libraries(simple_example_ffi PUBLIC dart_dll)

if (MSVC)
	set_property(TARGET simple_example_ffi PROPERTY VS_DEBUGGER_WORKING_DIRECTORY $<TARGET_FILE_DIR:simple_example_ffi>)	
endif()
