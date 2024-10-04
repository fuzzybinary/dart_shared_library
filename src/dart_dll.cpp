#include <iostream>

#include "dart_dll.h"
#include "isolate_setup.h"

#include <include/dart_api.h>
#include <include/dart_embedder_api.h>

#include <bin/dartutils.h>
#include <bin/dfe.h>
#include <bin/file.h>
#include <bin/gzip.h>
#include <bin/loader.h>
#include <bin/isolate_data.h>
#include <platform/utils.h>

using namespace dart::bin;
using namespace dart;

static DartDllConfig _dart_dll_config;

extern "C" {
extern const uint8_t kDartVmSnapshotData[];
extern const uint8_t kDartVmSnapshotInstructions[];
}

namespace dart {
namespace bin {
extern unsigned int observatory_assets_archive_len;
extern const uint8_t* observatory_assets_archive;
}  // namespace bin
}  // namespace dart

static bool FileModifiedCallback(const char* url, int64_t since) {
  auto path = File::UriToPath(url);
  if (path == nullptr) {
    // If it isn't a file on local disk, we don't know if it has been
    // modified.
    return true;
  }
  int64_t data[File::kStatSize];
  File::Stat(nullptr, path.get(), data);
  if (data[File::kType] == File::kDoesNotExist) {
    return true;
  }
  return data[File::kModifiedTime] > since;
}

Dart_Handle GetVMServiceAssetsArchiveCallback() {
  uint8_t* decompressed = NULL;
  intptr_t decompressed_len = 0;
  Decompress(observatory_assets_archive, observatory_assets_archive_len,
             &decompressed, &decompressed_len);
  Dart_Handle tar_file =
      DartUtils::MakeUint8Array(decompressed, decompressed_len);
  // Free decompressed memory as it has been copied into a Dart array.
  free(decompressed);
  return tar_file;
}

static Dart_Isolate CreateIsolateGroupAndSetup(const char* script_uri,
                                               const char* main,
                                               const char* package_root,
                                               const char* package_config,
                                               Dart_IsolateFlags* flags,
                                               void* callback_data,
                                               char** error) {
  Dart_Isolate isolate = nullptr;

  if (0 == strcmp(script_uri, DART_KERNEL_ISOLATE_NAME)) {
    return CreateKernelIsolate(script_uri, main, package_root, package_config,
                               flags, callback_data, error);
  } else if (0 == strcmp(script_uri, DART_VM_SERVICE_ISOLATE_NAME)) {
    return CreateVmServiceIsolate(script_uri, main, package_root,
                                  package_config, flags, callback_data,
                                  _dart_dll_config.service_port, error);
  } else {
    return CreateIsolate(false, script_uri, main, package_config, flags,
                         callback_data, error);
  }

  return isolate;
}

bool OnIsolateInitialize(void** child_callback_data, char** error) {
  Dart_Isolate isolate = Dart_CurrentIsolate();
  assert(isolate != nullptr);

  auto isolate_group_data =
      reinterpret_cast<IsolateGroupData*>(Dart_CurrentIsolateGroupData());

  auto isolate_data = new IsolateData(isolate_group_data);
  *child_callback_data = isolate_data;

  Dart_EnterScope();
    
  const auto script_uri = isolate_group_data->script_url;
  // TODO
  /*const bool isolate_run_app_snapshot =
      isolate_group_data->RunFromAppSnapshot();*/
  Dart_Handle result = SetupCoreLibraries(isolate, isolate_data,
                                          /*group_start=*/false,
                                          /*resolved_packages_config=*/nullptr);
  if (Dart_IsError(result)) goto failed;

  result = DartUtils::ResolveScript(Dart_NewStringFromCString(script_uri));
  if (Dart_IsError(result)) goto failed;

  if (isolate_group_data->kernel_buffer() != nullptr) {
    // Various core-library parts will send requests to the Loader to resolve
    // relative URIs and perform other related tasks. We need Loader to be
    // initialized for this to work because loading from Kernel binary
    // bypasses normal source code loading paths that initialize it.
    const char* resolved_script_uri = nullptr;
    result = Dart_StringToCString(result, &resolved_script_uri);
    if (Dart_IsError(result)) goto failed;
    result = Loader::InitForSnapshot(resolved_script_uri, isolate_data);
    if (Dart_IsError(result)) goto failed;
  }

  Dart_ExitScope();

  return true;

failed:
  *error = Utils::StrDup(Dart_GetError(result));
  Dart_ExitScope();
  return false;
}

static void OnIsolateShutdown(void*, void*) {
  Dart_EnterScope();
  Dart_Handle sticky_error = Dart_GetStickyError();
  if (!Dart_IsNull(sticky_error) && !Dart_IsFatalError(sticky_error)) {
    std::cerr << "Error shutting down isolate: " << Dart_GetError(sticky_error)
              << std::endl;
  }
  Dart_ExitScope();
}

extern "C" {

bool DartDll_Initialize(const DartDllConfig& config) {
  std::cout << "Initializig Dart ---- \n";

  Dart_SetVMFlags(0, nullptr);

  char* error = nullptr;
  if (!dart::embedder::InitOnce(&error)) {
    std::cerr << "Dart initialization failed: " << error << std::endl;
    return false;
  }

  // copy the configuration
  memcpy(&_dart_dll_config, &config, sizeof(DartDllConfig));

  dfe.Init();
  dfe.set_use_dfe();
  dfe.set_use_incremental_compiler(true);

  Dart_InitializeParams params = {};
  params.version = DART_INITIALIZE_PARAMS_CURRENT_VERSION;
  params.vm_snapshot_data = kDartVmSnapshotData;
  params.vm_snapshot_instructions = kDartVmSnapshotInstructions;
  params.create_group = CreateIsolateGroupAndSetup;
  params.initialize_isolate = OnIsolateInitialize;
  params.shutdown_isolate = OnIsolateShutdown;
  params.cleanup_isolate = DeleteIsolateData;
  params.cleanup_group = DeleteIsolateGroupData;
  params.entropy_source = DartUtils::EntropySource;
  params.get_service_assets = GetVMServiceAssetsArchiveCallback;
  params.start_kernel_isolate =
      dfe.UseDartFrontend() && dfe.CanUseDartFrontend();

  char* initError = Dart_Initialize(&params);

  std::cout << "Dart initialized, error was: "
            << (initError != nullptr ? initError : "null") << std::endl;

  Dart_SetFileModifiedCallback(&FileModifiedCallback);

  return true;
}

Dart_Isolate DartDll_LoadScript(const char* script_uri,
                                const char* package_config,
                                void* isolate_data) {
  Dart_IsolateFlags isolate_flags;
  Dart_IsolateFlagsInitialize(&isolate_flags);

  char* error = nullptr;
  Dart_Isolate isolate = CreateIsolate(true, script_uri, "main", package_config,
                                       &isolate_flags, isolate_data, &error);

  return isolate;
}

void* DartDll_GetUserIsolateData(void* isolate_group_data) {
  return GetUserIsolateData(isolate_group_data);
}

Dart_Handle DartDll_RunMain(Dart_Handle library) {
  Dart_Handle mainClosure =
      Dart_GetField(library, Dart_NewStringFromCString("main"));
  if (!Dart_IsClosure(mainClosure)) {
    std::cerr << "Unable to find 'main' in root library hello_world.dart";
    return mainClosure;
  }

  // Call _startIsolate in the isolate library to enable dispatching the
  // initial startup message.
  const intptr_t kNumIsolateArgs = 2;
  Dart_Handle isolateArgs[2] = {mainClosure, Dart_Null()};
  Dart_Handle isolateLib =
      Dart_LookupLibrary(Dart_NewStringFromCString("dart:isolate"));
  Dart_Handle result =
      Dart_Invoke(isolateLib, Dart_NewStringFromCString("_startMainIsolate"),
                  kNumIsolateArgs, isolateArgs);
  if (Dart_IsError(result)) {
    std::cout << "Dart initialized, error was: " << Dart_GetError(result)
              << "\n"
              << std::endl;
    return result;
  }

  // Keep handling messages until the last active receive port is closed.
  result = Dart_RunLoop();

  return result;
}

Dart_Handle DartDll_DrainMicrotaskQueue() {
  Dart_EnterScope();

  // TODO: Cache looking up the dart:isolate library
  Dart_Handle libraryName = Dart_NewStringFromCString("dart:isolate");
  Dart_Handle isolateLib = Dart_LookupLibrary(libraryName);
  if (Dart_IsError(isolateLib)) {
    std::cerr << "Error looking up 'dart:isolate' library: "
              << Dart_GetError(isolateLib);
    Dart_ExitScope();
    return isolateLib;
  }

  Dart_Handle invokeName =
      Dart_NewStringFromCString("_runPendingImmediateCallback");
  Dart_Handle result = Dart_Invoke(isolateLib, invokeName, 0, nullptr);
  if (Dart_IsError(result)) {
    std::cerr << "Error draining microtask queue: " << Dart_GetError(result);
    return result;
  }
  result = Dart_HandleMessage();
  if (Dart_IsError(result)) {
    std::cerr << "Error draining microtask queue: %s" << Dart_GetError(result);
    return result;
  }

  Dart_ExitScope();

  return result;
}

bool DartDll_Shutdown() {
  char* error = Dart_Cleanup();
  if (error != nullptr) {
    std::cerr << "Error cleaning up Dart: " << error;
    return false;
  }

  dart::embedder::Cleanup();

  return true;
}
}
