#include <cute.h>
using namespace Cute;

#include <dart_dll.h>

#include <dart_tools_api.h>
#include <unordered_map>
#include <vector>
#include <iostream>
#include "drawable.h"

static Dart_Isolate _dart_isolate = nullptr;
static unsigned int _dart_pending_messages = 0;

void dart_message_notify_callback(Dart_Isolate isolate) {
  _dart_pending_messages++;
}

Dart_PersistentHandle _root_library;
bool init_dart() {
  DartDllConfig config;
  DartDll_Initialize(config);

  //if package_config.json not exits run pub get
  _dart_isolate = DartDll_LoadScript("dart/main.dart",
                                     "dart/.dart_tool/package_config.json");
  if (_dart_isolate == nullptr) {
    return false;
  }

  Dart_EnterIsolate(_dart_isolate);
  Dart_SetMessageNotifyCallback(dart_message_notify_callback);

  Dart_EnterScope();
  Dart_Handle root_library = Dart_RootLibrary();
  _root_library = Dart_NewPersistentHandle(root_library);
  Dart_Handle init_function_name = Dart_NewStringFromCString("main");
  Dart_Handle result =
      Dart_Invoke(root_library, init_function_name, 0, nullptr);
  if (Dart_IsError(result)) {
    Dart_ExitScope();
    return false;
  }

  Dart_ExitScope();

  return true;
}

void dart_frame(float delta_time) {
  Dart_EnterScope();

  Dart_Handle frame_function_name = Dart_NewStringFromCString("frame");
  Dart_Handle args[1] = {
      Dart_NewDouble(delta_time),
  };
  Dart_Handle root_library = Dart_HandleFromPersistent(_root_library);
  Dart_Handle result =
      Dart_Invoke(root_library, frame_function_name, 1, args);

  Dart_ExitScope();
}

void dart_frame_maintanance() {
  // Drain the Dart microtask queue. This allows Futures and async methods
  // to complete and execute any pending code.
  DartDll_DrainMicrotaskQueue();

  // Handle any pending messages. This includes handling Finalizers.
  Dart_EnterScope();

  while (_dart_pending_messages > 0) {
    
    auto handle = Dart_HandleMessage();
    if(Dart_IsError(handle)) {
      std::cout << Dart_GetError(handle) << std::endl;
    }
    _dart_pending_messages--;
  }

  uint64_t currentTime = Dart_TimelineGetMicros();
  // Notify Dart we're likely to be idle for the next few ms,
  // this allows the garbage collector to fire if needed.
  Dart_NotifyIdle(currentTime + 3000);

  Dart_ExitScope();
}

std::unordered_map<unsigned int, Drawable*> entity_drawable_map;
unsigned int next_entity_id = 1;
void render_drawables() {
  for (const auto& pair : entity_drawable_map) {
    draw_push_color(pair.second->color);
    draw_quad_fill(make_aabb(V2(pair.second->x, pair.second->y),
                             V2(pair.second->x + pair.second->width,
                                pair.second->y + pair.second->height)));
    draw_pop_color();
  }
}

// 
// Dart accessible funcitons
// These need to be exposed as "C" functions and be exported
//
#if defined(_WIN32)
#define WORM_EXPORT __declspec(dllexport)
#elif defined(__GNUC__)
#define WORM_EXPORT __attribute__((visibility("default")))
#else
#define WORM_EXPORT
#endif

#ifdef __cplusplus
extern "C" {
#endif

WORM_EXPORT unsigned int create_entity(int x, int y, int width, int height) {
  Drawable* d = new Drawable{x, y, width, height, color_blue() };
  unsigned int entity_id = next_entity_id;
  entity_drawable_map[entity_id] = d;

  next_entity_id++;

  return entity_id;
}

WORM_EXPORT void destroy_entity(unsigned int entity_id) {
  const auto& itr = entity_drawable_map.find(entity_id);
  if (itr != entity_drawable_map.end()) {
    delete itr->second;
    entity_drawable_map.erase(itr);
  }
}

WORM_EXPORT Drawable* get_drawable(unsigned int entity_id) {
  const auto& itr = entity_drawable_map.find(entity_id);
  if (itr != entity_drawable_map.end()) {
    return itr->second;
  }
  return nullptr;
}

WORM_EXPORT bool get_key_just_pressed(int key_code) {
  return cf_key_just_pressed((CF_KeyButton)key_code);
}

#ifdef __cplusplus
}
#endif

int main(int argc, char* argv[]) {
  // Create a window with a resolution of 640 x 480.
  int options =
      APP_OPTIONS_DEFAULT_GFX_CONTEXT | APP_OPTIONS_WINDOW_POS_CENTERED;
  Result result =
      make_app("Fancy Window Title", 0, 0, 640, 480, options, argv[0]);
  if (is_error(result)) return -1;

  if (!init_dart()) return -1;

  while (app_is_running()) {
    app_update();

    dart_frame(DELTA_TIME);
    dart_frame_maintanance();
    
    render_drawables();

    app_draw_onto_screen();
  }

  destroy_app();

  return 0;
}