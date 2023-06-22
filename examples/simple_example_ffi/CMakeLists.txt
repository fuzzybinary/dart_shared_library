cmake_minimum_required(VERSION 3.21)

project(simple_example_ffi)

add_executable(simple_example_ffi main.cpp)

target_include_directories(simple_example_ffi PRIVATE
    "${DART_DLL_DIR}"
    "${DART_DIR}/runtime/include"
)

add_custom_command(TARGET simple_example_ffi POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy -t $<TARGET_FILE_DIR:simple_example_ffi> $<TARGET_RUNTIME_DLLS:simple_example_ffi>
  COMMAND_EXPAND_LISTS
)

target_link_libraries(simple_example_ffi PUBLIC dart_dll)