import 'dart:async';
import 'dart:convert';
import 'dart:io';

import 'package:logger/logger.dart';
import 'package:path/path.dart' as path;

export 'depot_tools.dart';

class BuildToolsLogger {
  static Logger? _shared;
  static Logger get shared {
    _shared ??= initLogger();
    return _shared!;
  }

  static Logger initLogger({Level logLevel = Level.info}) {
    _shared = Logger(
      filter: ProductionFilter(),
      level: logLevel,
      printer: SimplePrinter(),
    );
    return shared;
  }
}

bool checkRightDirectory() {
  final logger = BuildToolsLogger.shared;

  final currentDir = Directory.current;
  // Check if the .dart_version and the patch file are there.
  if (!File(path.join(currentDir.path, '.dart_version')).existsSync()) {
    logger.f(
        'Could not find `.dart_version`. Make sure you\'re running from the root of the `dart_dll` repo.');
    return false;
  }

  if (!File(path.join(currentDir.path, 'dart_sdk.patch')).existsSync()) {
    logger.f(
        'Could not find `dart_sdk.pathch`. Make sure you\'re running from the root of the `dart_dll` repo.');
    return false;
  }

  return true;
}

// Waits for the process to finish, outputting output to the BuildToolsLogger
Future<int> waitForProcessFinish(Process process) async {
  final logger = BuildToolsLogger.shared;
  final stdoutCompleter = Completer();
  final stderrCompleter = Completer();

  process.stdout.transform(utf8.decoder).transform(const LineSplitter()).listen(
    (l) {
      logger.i(l);
    },
  ).onDone(() {
    stdoutCompleter.complete();
  });

  process.stderr.transform(utf8.decoder).transform(const LineSplitter()).listen(
    (l) {
      logger.i(l);
    },
  ).onDone(() {
    stderrCompleter.complete();
  });

  var exitCode = await process.exitCode;

  await (Future.wait([stdoutCompleter.future, stderrCompleter.future]));

  return exitCode;
}

Future<T> inDir<T>(String directory, Future<T> Function() callback) async {
  final oldDir = Directory.current;
  Directory.current = Directory(directory);
  final result = await callback();
  Directory.current = oldDir;
  return result;
}
