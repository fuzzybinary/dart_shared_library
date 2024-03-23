cmake_minimum_required(VERSION 3.21)

project(dart_dll VERSION 0.1.2)

set(DARTSDK_ROOTDIR "${PROJECT_SOURCE_DIR}/../dart-sdk" CACHE FILEPATH "Directory that dart-sdk is cloned too")
set(DART_DIR "${DARTSDK_ROOTDIR}/sdk")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS )

add_library(dart_dll SHARED
    dart_dll.cpp
    isolate_setup.cpp
)

target_include_directories(dart_dll PRIVATE
    "${DART_DIR}/runtime"
)

if(WIN32)
    set(LIB_PREFIX "lib")
endif()

cmake_path(ABSOLUTE_PATH DART_DIR NORMALIZE OUTPUT_VARIABLE DART_DIR)

MESSAGE(STATUS "Dart SDK ${DART_DIR}")

if(NOT EXISTS "${DART_DIR}/runtime/include/dart_api.h")
MESSAGE(FATAL_ERROR "Missing Dart SDK or not found")
endif()

find_library(LIB_DART_DEBUG
    NAMES "${LIB_PREFIX}dart"
    HINTS "${DART_DIR}/out/DebugX64/obj/runtime/bin" "${DART_DIR}/xcodebuild/ReleaseX64/obj/runtime/bin"
)

find_library(LIB_DART_RELEASE
    NAMES "${LIB_PREFIX}dart"
    HINTS "${DART_DIR}/out/ReleaseX64/obj/runtime/bin" "${DART_DIR}/xcodebuild/ReleaseX64/obj/runtime/bin"
)

target_compile_definitions(dart_dll PRIVATE
    __STDC_LIMIT_MACROS
    __STDC_FORMAT_MACROS
    __STDC_CONSTANT_MACROS
)

if(WIN32)
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
    set_property(TARGET dart_dll PROPERTY
             MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
elseif(LINUX)
    set(THREADS_PREFER_PTHREAD_FLAG ON)
    set(CMAKE_C_COMPILER "${DART_DIR}/buildtools/linux-x64/clang/bin/clang")
    set(CMAKE_CXX_COMPILER "${DART_DIR}/buildtools/linux-x64/clang/bin/clang++")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -nostdlib++ ${DART_DIR}/buildtools/linux-x64/clang/lib/x86_64-unknown-linux-gnu/libc++.a")
    #set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -stdlib=libc++ -lc++abi")

    find_package(Threads REQUIRED)
    target_link_libraries(dart_dll 
        Threads::Threads
        ${CMAKE_DL_LIBS}
    )
elseif(APPLE)
    set(CMAKE_C_COMPILER "${DART_DIR}/buildtools/mac-x64/clang/bin/clang")
    set(CMAKE_CXX_COMPILER "${DART_DIR}/buildtools/mac-x64/clang/bin/clang++")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -nostdlib++ ${DART_DIR}/buildtools/mac-x64/clang/lib/libc++.a -framework Cocoa -framework QuartzCore -framework Security")
endif()

if(LIB_DART_DEBUG) 
    target_link_libraries(dart_dll debug ${LIB_DART_DEBUG})
else()
    target_link_libraries(dart_dll debug ${LIB_DART_RELEASE})
endif()

target_link_libraries(dart_dll optimized ${LIB_DART_RELEASE})