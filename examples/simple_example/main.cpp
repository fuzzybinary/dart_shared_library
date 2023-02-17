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

void SimplePrint(Dart_NativeArguments arguments) {
  Dart_Handle string = HandleError(Dart_GetNativeArgument(arguments, 0));
  if (Dart_IsString(string)) {
    const char* cstring;
    Dart_StringToCString(string, &cstring);
    std::cout << "Hello from C++. Dart says:\n";
    std::cout << cstring;
  }
}

Dart_NativeFunction ResolveNativeFunction(Dart_Handle name,
                                          int /* argc */,
                                          bool* /* auto_setup_scope */) {
  if (!Dart_IsString(name)) {
    return nullptr;
  }

  Dart_NativeFunction result = nullptr;

  const char* cname;
  HandleError(Dart_StringToCString(name, &cname));

  if (strcmp("SimplePrint", cname) == 0) {
    result = SimplePrint;
  }

  return result;
}

int main() {
  // Initialize Dart
  if (!DartDll_Initialize()) {
    return -1;
  }

  // Load your main isolate file, also providing the path to a package config if one exists.
  // The build makes sure these are coppied to the output directory for running the example
  Dart_Isolate isolate = DartDll_LoadScript("hello_world.dart", nullptr);
  if (isolate == nullptr) {
    return -1;
  }

  // With the library loaded, you can now use the dart_api.h functions
  // This includes setting the native function resolver:
  Dart_EnterIsolate(isolate);
  Dart_EnterScope();

  Dart_Handle library = Dart_RootLibrary();
  Dart_SetNativeResolver(library, ResolveNativeFunction, nullptr);

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
