import 'dart:async';
import 'dart:io';

import 'package:args/args.dart';
import 'package:build_helpers/build_helpers.dart';
import 'package:logger/logger.dart';
import 'package:path/path.dart' as path;

void main(List<String> args) async {
  final parser = ArgParser();
  parser.addFlag('verbose', abbr: 'v');

  final argResults = parser.parse(args);
  Level logLevel = Level.info;
  if (argResults['verbose'] == true) {
    logLevel = Level.debug;
  }

  if (!checkRightDirectory()) {
    // Not run from root. Exit.
    exit(-1);
  }

  if (Platform.isWindows) {
    final depotToolsEnv = Platform.environment['DEPOT_TOOLS_WIN_TOOLCHAIN'];
    if (depotToolsEnv == null) {
      BuildToolsLogger.shared.e(
          'DEPOT_TOOLS_WIN_TOOOLCHAIN not set! Run ./setup_env.ps1 before running this script!');
      exit(-1);
    }
  }

  BuildToolsLogger.initLogger(logLevel: logLevel);
  if (!await checkForDepotTools()) {
    if (!await getDepotTools()) {
      // Fatal. Can't do this without depot_tools
      exit(-1);
    }
  }

  try {
    if (!await _fetchOrUpdateDartSdk()) {
      exit(-1);
    }

    if (!await _patchDartSdk()) {
      exit(-1);
    }

    if (!await _buildDart()) {
      exit(-1);
    }
  } catch (e) {
    BuildToolsLogger.shared.f('Caught an exception building the Dart SDK:');
    BuildToolsLogger.shared.f(e);
    exit(-1);
  }
}

Future<bool> _fetchOrUpdateDartSdk() async {
  final logger = BuildToolsLogger.shared;
  final dartVersion = await _fetchRequestedDartVersion();

  if (!Directory('dart-sdk').existsSync()) {
    logger.i('dart-sdk does not exist. Doing full fetch');

    Directory('dart-sdk').create();

    final fetchResult = await inDir('dart-sdk', () async {
      final fetchProcess =
          await startAppendingDepotToolsPath('fetch', ['--no-history', 'dart']);
      var fetchResult = await waitForProcessFinish(fetchProcess);
      return fetchResult;
    });
    if (fetchResult != 0) return false;
  }

  final finalResult = await inDir(path.join('dart-sdk', 'sdk'), () async {
    logger.i('Checking out tag $dartVersion');
    final fetchProcess = await Process.start(
      'git',
      ['fetch', 'origin', 'refs/tags/$dartVersion:refs/tags/$dartVersion'],
      runInShell: true,
    );
    var fetchResult = await waitForProcessFinish(fetchProcess);
    if (fetchResult != 0) return fetchResult;

    final checkoutProcess = await Process.start(
      'git',
      ['checkout', '-f', 'tags/$dartVersion'],
      runInShell: true,
    );
    var checkoutResult = await waitForProcessFinish(checkoutProcess);
    if (checkoutResult != 0) return checkoutResult;

    logger.i('Performing `gclient sync -D --no-history');
    final syncProcess = await startAppendingDepotToolsPath(
      'gclient',
      ['sync', '-D', '--no-history'],
    );
    var syncResult = await waitForProcessFinish(syncProcess);
    if (syncResult != 0) return syncResult;

    return 0;
  });

  return finalResult == 0;
}

Future<String> _fetchRequestedDartVersion() async {
  final lines = await File('.dart_version').readAsLines();
  for (var line in lines) {
    if (line.startsWith('#')) {
      continue;
    }

    return line;
  }
  throw Exception('Only found comments in the `.dart_version` file!');
}

Future<bool> _patchDartSdk() async {
  final logger = BuildToolsLogger.shared;
  final result = await inDir('dart-sdk/sdk', () async {
    logger.i("Patching the Dart SDK to create libdart");
    var result = await Process.run('git', ['apply', '../../dart_sdk.patch'],
        runInShell: true);
    logger.d(result.stdout);
    return result.exitCode;
  });
  if (result != 0) {
    logger.f('Failed to apply patch.');
  }

  return result == 0;
}

Future<bool> _buildDart() async {
  final logger = BuildToolsLogger.shared;
  final result = await inDir('dart-sdk/sdk', () async {
    logger.i("Building libdart");
    var script = './tools/build.py';
    var args = ['--no-goma', '-m', 'release', 'libdart'];
    var command = script;
    if (Platform.isWindows) {
      command = 'python';
      args.insert(0, script);
    }
    final buildProcess = await Process.start(
      command,
      args,
      runInShell: true,
    );
    var buildResult = await waitForProcessFinish(buildProcess);
    return buildResult;
  });
  if (result != 0) {
    logger.f('Failed to build dart.');
  }
  return result == 0;
}
