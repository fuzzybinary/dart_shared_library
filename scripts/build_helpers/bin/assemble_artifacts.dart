// Script to assemble artifacts into a single place
import 'dart:io';

import 'package:build_helpers/build_helpers.dart';
import 'package:glob/glob.dart';
import 'package:glob/list_local_fs.dart';
import 'package:logger/logger.dart';
import 'package:path/path.dart' as path;

void main() {
  if (!checkRightDirectory()) {
    // Not run from root. Exit.
    exit(-1);
  }
  // Always verbose
  BuildToolsLogger.initLogger(logLevel: Level.debug);

  final dest = Directory('artifacts');
  dest.createSync();
  _copyIncludeFiles(dest);
  _copyLibs(dest);
}

void _copyIncludeFiles(Directory dest) {
  final logger = BuildToolsLogger.shared;

  final includePath = Directory('dart-sdk/sdk/runtime/include');
  if (!includePath.existsSync()) {
    logger.f("Couldn't find Dart SDK include dir.");
    exit(-1);
  }

  const dartIncludeFiles = ['dart_api.h', 'dart_tools_api.h'];
  Directory(path.join(dest.path, 'include')).createSync(recursive: true);
  for (var dartIncludeFile in dartIncludeFiles) {
    final file = File(path.join(includePath.path, dartIncludeFile));
    final destPath = path.join(dest.path, 'include', dartIncludeFile);
    logger.i('  ${file.path} => $destPath');
    file.copySync(destPath);
  }

  final dartDllHeader = File('src/include/dart_dll.h');
  final destPath = path.join(path.join(dest.path, 'include', 'dart_dll.h'));
  dartDllHeader.copySync(destPath);
  logger.i('  ${dartDllHeader.path} => $destPath');
}

void _copyLibs(Directory dest) {
  final logger = BuildToolsLogger.shared;

  final builtLibPath = Directory(path.join('build', 'src'));
  if (!builtLibPath.existsSync()) {
    logger.f('Could not find built artifact path');
  }

  final binDestPath = Directory(path.join(dest.path, 'bin'));
  binDestPath.createSync(recursive: true);

  var copyGlob = Glob('*.so');
  if (Platform.isWindows) {
    copyGlob = Glob('{Release/*.*,Debug/*.*}', caseSensitive: false);
  } else if (Platform.isMacOS) {
    copyGlob = Glob('*.dylib');
  }
  final files = copyGlob.listSync(root: builtLibPath.path);
  for (var file in files) {
    final destPath = path.join(binDestPath.path, file.basename);
    logger.i('  ${file.path} => $destPath');
    (file as File).copySync(destPath);
  }
}
