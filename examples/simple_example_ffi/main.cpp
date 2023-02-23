#include <string.h>
#include <iostream>

#include "dart_dll.h"

#include <dart_api.h>

Dart_Handle HandleError(Dart_Handle handle) {
  if (Dart_IsError(handle)) {
    Dart_PropagateError(handle);
  }
  return handle;
}

void SimplePrint(const char* string) {
  std::cout << "Hello from C++. Dart says:\n";
  std::cout << string;
}

void* ResolveNativeFunction(const char* name, uintptr_t args_n) {
  void* native_function = nullptr;
  if (strcmp("SimplePrint", name) == 0) {
    native_function = SimplePrint;
  }

  return native_function;
}

int main() {
  // Initialize Dart
  if (!DartDll_Initialize()) {
    return -1;
  }

  // Load your main isolate file, also providing the path to a package config if one exists.
  // The build makes sure these are coppied to the output directory for running the example
  Dart_Isolate isolate = DartDll_LoadScript("hello_world.dart", ".dart_tool/package_config.json");
  if (isolate == nullptr) {
    return -1;
  }

  // With the library loaded, you can now use the dart_api.h functions
  // This includes setting the native function resolver:
  Dart_EnterIsolate(isolate);
  Dart_EnterScope();

  Dart_Handle library = Dart_RootLibrary();
  Dart_SetFfiNativeResolver(library, ResolveNativeFunction);

  // And run "main"
  Dart_Handle result = DartDll_RunMain(library);
  if (Dart_IsError(result)) {
    std::cerr << "Failed to invoke main: " << Dart_GetError(result);
  }

  Dart_ExitScope();
  Dart_ShutdownIsolate();

  // Don't forget to shutdown
  DartDll_Shutdown();

  return 0;
}
