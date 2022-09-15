#pragma once

#ifdef DART_DLL_EXPORTING
    #define DART_DLL_EXPORT __declspec(dllexport)
#else
    #define DART_DLL_EXPORT __declspec(dllimport)
#endif

DART_DLL_EXPORT void DartDll_Initialize();