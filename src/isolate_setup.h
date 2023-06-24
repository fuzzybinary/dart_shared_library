#pragma once

#include <include/dart_api.h>

Dart_Isolate CreateKernelIsolate(const char* script_uri,
                                 const char* main,
                                 const char* package_root,
                                 const char* package_config,
                                 Dart_IsolateFlags* flags,
                                 void* callback_data,
                                 char** error);

Dart_Isolate CreateVmServiceIsolate(const char* script_uri,
                                    const char* main,
                                    const char* package_root,
                                    const char* package_config,
                                    Dart_IsolateFlags* flags,
                                    void* callback_data,
                                    int service_port,
                                    char** error);

Dart_Isolate CreateIsolate(bool is_main_isolate,
                           const char* script_uri,
                           const char* name,
                           const char* packages_config,
                           Dart_IsolateFlags* flags,
                           void* callback_data,
                           char** error);