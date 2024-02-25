cmake_minimum_required(VERSION 3.21)

project(simple_example)

add_executable(simple_example main.cpp)

target_include_directories(simple_example PRIVATE
    "${DART_DIR}/runtime/include"
)

add_custom_command(TARGET simple_example POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy -t $<TARGET_FILE_DIR:simple_example> $<TARGET_RUNTIME_DLLS:simple_example>)
add_custom_command(TARGET simple_example POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy -t $<TARGET_FILE_DIR:simple_example> ${PROJECT_SOURCE_DIR}/hello_world.dart)

target_link_libraries(simple_example PUBLIC dart_dll)

if (MSVC)
	set_property(TARGET simple_example PROPERTY VS_DEBUGGER_WORKING_DIRECTORY $<TARGET_FILE_DIR:simple_example>)	
endif()

set(EXECUTABLE_OUTPUT_PATH "${EXECUTABLE_OUTPUT_PATH}/simple_example")