#pragma once

#include <dart_api.h>
#include <cute.h>
using namespace Cute;

class Drawable {
 public:
  int x;
  int y;
  int width;
  int height;

  Color color;
};