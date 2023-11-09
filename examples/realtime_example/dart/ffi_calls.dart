import 'dart:ffi';

import 'drawable.dart';

class WorkFfiCalls {
  final DynamicLibrary processLib = DynamicLibrary.process();

  late final createEntity = processLib
      .lookup<NativeFunction<Uint32 Function(Int32, Int32, Int32, Int32)>>(
          'create_entity')
      .asFunction<int Function(int, int, int, int)>(isLeaf: true);
  late final destroyEntity = processLib
      .lookup<NativeFunction<Void Function(Uint32)>>('destroy_entity')
      .asFunction<void Function(int)>(isLeaf: true);
  late final getDrawable = processLib
      .lookup<NativeFunction<Pointer<Drawable> Function(Uint32)>>(
          'get_drawable')
      .asFunction<Pointer<Drawable> Function(int)>(isLeaf: true);

  late final getKeyJustPressed = processLib
      .lookup<NativeFunction<Bool Function(Uint32)>>('get_key_just_pressed')
      .asFunction<bool Function(int)>(isLeaf: true);
}

const int CF_KEY_RIGHT = 145;
const int CF_KEY_LEFT = 146;
const int CF_KEY_DOWN = 147;
const int CF_KEY_UP = 148;
