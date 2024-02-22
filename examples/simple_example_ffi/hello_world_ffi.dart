import 'dart:ffi';
import 'package:ffi/ffi.dart';

@FfiNative<Void Function(Pointer<Utf8>)>("SimplePrint", isLeaf: true)
external void simplePrint(Pointer<Utf8> string);

void main() {
  var string = "Hello From Dart!\n";
  var c_string = string.toNativeUtf8();

  simplePrint(c_string);

  malloc.free(c_string);
}
