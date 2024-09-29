cmake_minimum_required(VERSION 3.21)

project(isolate_spawn)

add_executable(isolate_spawn main.cpp)

target_include_directories(isolate_spawn PRIVATE
    "${DART_DLL_DIR}"
    "${DART_DIR}/runtime/include"
)

if(WIN32)
  add_custom_command(TARGET isolate_spawn POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:isolate_spawn> $<TARGET_FILE_DIR:isolate_spawn>
    COMMAND_EXPAND_LISTS
  )
endif()

add_custom_command(TARGET isolate_spawn POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/hello_world.dart $<TARGET_FILE_DIR:isolate_spawn>
  COMMAND_EXPAND_LISTS
)

target_link_libraries(isolate_spawn PUBLIC dart_dll)

if (MSVC)
	set_property(TARGET isolate_spawn PROPERTY VS_DEBUGGER_WORKING_DIRECTORY $<TARGET_FILE_DIR:isolate_spawn>)	
endif()