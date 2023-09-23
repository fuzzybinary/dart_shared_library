import 'dart:io';

import 'package:path/path.dart' as path;

import 'build_helpers.dart';

String? depotToolsPath;

Future<bool> checkForDepotTools() async {
  final logger = BuildToolsLogger.shared;
  logger.i('Checking for depot_tools');
  final result = await Process.run('gclient', ['--version'], runInShell: true);
  if (result.exitCode != 0) {
    logger.w('Failed checking for depot_tools.');
    logger.d('Output of `gclient --version`:');
    logger.d(result.stdout);
  } else {
    logger.i('✅ Found depot_tools');
  }

  return result.exitCode == 0;
}

Future<bool> getDepotTools() async {
  final logger = BuildToolsLogger.shared;

  logger.i('Cloning depot_tools...');
  final cloneResult = await Process.run(
    'git',
    [
      'clone',
      'https://chromium.googlesource.com/chromium/tools/depot_tools.git'
    ],
    runInShell: true,
  );
  if (cloneResult.exitCode != 0) {
    logger.e('Failed cloning depot_tools repot.');
    logger.d('Output of clone:');
    logger.d(cloneResult.stdout);
    return false;
  }
  depotToolsPath = path.join(Directory.current.absolute.path, 'depot_tools');

  logger.i('Running `gclient` to prep depot_tools.');
  final gclientResult =
      await runAppendingDepotToolsPath('gclient', ['--version']);
  logger.d(gclientResult.stdout);
  if (gclientResult.exitCode != 0) {
    logger.f('Still could not run `gclient` after clone.');
    return false;
  }

  return true;
}

String _depotToolsAppendedPath() {
  final logger = BuildToolsLogger.shared;
  var pathEnv = Platform.environment['PATH'];

  if (depotToolsPath != null) {
    if (Platform.isWindows) {
      pathEnv = '$depotToolsPath;$pathEnv';
    } else {
      pathEnv = '$depotToolsPath:$pathEnv';
    }
    logger.d('Path is now: $pathEnv');
  }

  return pathEnv!;
}

Future<ProcessResult> runAppendingDepotToolsPath(
    String command, List<String> arguments) {
  final pathEnv = _depotToolsAppendedPath();

  return Process.run(
    command,
    arguments,
    environment: {
      'PATH': pathEnv,
    },
    runInShell: true,
  );
}

Future<Process> startAppendingDepotToolsPath(
    String command, List<String> arguments) {
  final pathEnv = _depotToolsAppendedPath();
  return Process.start(
    command,
    arguments,
    environment: {'PATH': pathEnv},
    runInShell: true,
  );
}
