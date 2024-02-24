#pragma once
#include <unordered_map>
#include "drawable.h"

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

WORM_EXPORT unsigned int create_entity(int x, int y, int width, int height);
WORM_EXPORT void destroy_entity(unsigned int entity_id);
WORM_EXPORT Drawable* get_drawable(unsigned int entity_id);
WORM_EXPORT bool get_key_just_pressed(int key_code);

#ifdef __cplusplus
}
#endif