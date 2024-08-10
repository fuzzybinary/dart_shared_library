import 'dart:ffi';

import 'ffi_calls.dart';

WorkFfiCalls ffi = new WorkFfiCalls();

class Wall {
  late int entity;

  Wall(int x, int y, int width, int height) {
    entity = ffi.createEntity(x, y, width, height);
    final drawable = ffi.getDrawable(entity);
    drawable.ref.color.r = 1.0;
    drawable.ref.color.g = 0.0;
    drawable.ref.color.b = 0.0;
    drawable.ref.color.a = 1.0;
  }
}

List<Wall> walls = [];

// Dot!
final int speed = 80;
int dotEntity = 0;
double dotX = 0;
double dotY = 0;
bool movingLeft = true;

void main() {
  print('main');
  walls.add(
    Wall(-320, -240, 5, 480),
  );
  walls.add(
    Wall(315, -240, 5, 480),
  );
  walls.add(
    Wall(-320, -240, 640, 5),
  );
  walls.add(
    Wall(-320, 235, 640, 5),
  );

  dotEntity = ffi.createEntity(0, 0, 10, 10);
  final drawable = ffi.getDrawable(dotEntity);
  drawable.ref.color.r = 0.0;
  drawable.ref.color.g = 1.0;
  drawable.ref.color.b = 0.0;
}

void frame(double dt) {
  if (movingLeft) {
    dotX -= (speed * dt);
    if (dotX < -200) {
      dotX = -200;
      movingLeft = false;
    }
  } else {
    dotX += (speed * dt);
    if (dotX > 200) {
      dotX = 200;
      movingLeft = true;
    }
  }

  final dotDrawable = ffi.getDrawable(dotEntity);
  dotDrawable.ref.x = dotX.floor();
  dotDrawable.ref.y = dotY.floor();
}
