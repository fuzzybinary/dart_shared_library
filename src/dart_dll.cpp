#include <iostream>

#include "dart_dll.h"
#include "isolate_setup.h"

#include <include/dart_api.h>
#include <include/dart_embedder_api.h>

#include <bin/dfe.h>
#include <bin/isolate_data.h>
#include <bin/gzip.h>
#include <bin/dartutils.h>

using namespace dart::bin;

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
                                  package_config, flags, callback_data, error);
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
  Dart_Handle result;
  //Dart_Handle result = SetupCoreLibraries(isolate, isolate_data,
  //                                        /*group_start=*/false,
  //                                        /*resolved_packages_config=*/nullptr);
  if (Dart_IsError(result)) {
    *error = _strdup(Dart_GetError(result));
    Dart_ExitScope();
    return false;
  }

  // Make the isolate runnable so that it is ready to handle messages.
  Dart_ExitScope();
  Dart_ExitIsolate();
  *error = Dart_IsolateMakeRunnable(isolate);
  Dart_EnterIsolate(isolate);
  return *error == nullptr;
}

static void OnIsolateShutdown(void* isolate_group_data, void* isolate_data) {
  Dart_EnterScope();
  Dart_Handle sticky_error = Dart_GetStickyError();
  if (!Dart_IsNull(sticky_error) && !Dart_IsFatalError(sticky_error)) {
    std::cerr << "Error shutting down isolate: " << Dart_GetError(sticky_error) << std::endl;
  }
  Dart_ExitScope();
}

static void DeleteIsolateData(void* isolate_group_data, void* callback_data) {
  auto isolate_data = reinterpret_cast<IsolateData*>(callback_data);
  delete isolate_data;
}

static void DeleteIsolateGroupData(void* callback_data) {
  auto isolate_group_data = reinterpret_cast<IsolateGroupData*>(callback_data);
  delete isolate_group_data;
}

bool DartDll_Initialize() {
  std::cout << "Initializig Dart ---- \n";

  Dart_SetVMFlags(0, nullptr);

  char* error = nullptr;
  if (!dart::embedder::InitOnce(&error)) {
    std::cerr << "Dart initialization failed: " << error << std::endl;
    return false;
  }

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

  std::cout << "Dart initialized, error was: " << (initError != nullptr ? initError : "null") << std::endl;

  return true;
}

Dart_Isolate DartDll_LoadScript(const char* script_uri, const char* package_config) {
  Dart_IsolateFlags isolate_flags;
  Dart_IsolateFlagsInitialize(&isolate_flags);

  char* error = nullptr;
  Dart_Isolate isolate = CreateIsolate(true, script_uri, "main", package_config,
                                       &isolate_flags, nullptr, &error);
  if (isolate == nullptr) {
    return false;
  }

  return isolate;
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