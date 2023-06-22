cmake_minimum_required(VERSION 3.15)

project(dart_dll VERSION 0.1)

set(DART_DIR "${PROJECT_SOURCE_DIR}/../dart-sdk/sdk")

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_library(dart_dll SHARED
    dart_dll.cpp
    isolate_setup.cpp
)

target_include_directories(dart_dll PUBLIC
    "${DART_DIR}/runtime"
)

find_library(LIB_DART
    NAMES libdart
    HINTS "${DART_DIR}/out/ReleaseX64/obj/runtime/bin"
)

target_compile_definitions(dart_dll PRIVATE
    __STDC_LIMIT_MACROS
    __STDC_FORMAT_MACROS
    __STDC_CONSTANT_MACROS
)

if(WIN32)
    set_property(TARGET dart_dll PROPERTY
        MSVC_RUNTIME_LIBRARY "MultiThreaded"
    )

    target_compile_definitions(dart_dll PRIVATE
        _HAS_EXCEPTIONS=0
        _SCL_SECURE=0
        _SECURE_SCL=0
        _SCL_SECURE_NO_WARNINGS
        _CRT_SECURE_NO_WARNINGS
        _CRT_SECURE_NO_DEPRECATE
        DART_DLL_EXPORTING
        _WIN
    )

    target_link_libraries(dart_dll
        dbghelp
        bcrypt
        rpcrt4
        ws2_32
        Iphlpapi
        Psapi
        shlwapi
    )
endif()

target_link_libraries(dart_dll
    ${LIB_DART}
)