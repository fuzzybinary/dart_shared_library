#include "exportfunc.h"

std::unordered_map<unsigned int, Drawable*> entity_drawable_map;
unsigned int next_entity_id = 1;

#ifdef __cplusplus
extern "C" {
#endif

WORM_EXPORT unsigned int create_entity(int x, int y, int width, int height) {
  Drawable* d = new Drawable{x, y, width, height, color_blue()};
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