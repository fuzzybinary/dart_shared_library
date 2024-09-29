import 'dart:isolate';

@pragma('vm:external-name', 'SimplePrint')
external void simplePrint(String s);

Future<String> _isolateWorker() async {
  simplePrint("Isolate work 1...");
  await Future.delayed(Duration(seconds: 10));
  simplePrint("Isolate work 2...");
  return "done";
}

void main() async {
  simplePrint("Hello From Dart!\n");
  final isolateValue = await Isolate.run(_isolateWorker);
  simplePrint("Isolate complete, returned $isolateValue");
}
