# Dart Dynamic Import Library

This is an attempt / POC to build the Dart VM into a dynamic library, importable in any platform Dart supports.

## Eventual support

The hope is that the the dynamic library will eventually support the following targets:

* A "Fully Featured" .dll / .so that supports booting Dart in different configurations:

  * Boot (or not) the service and kernel isolates
  * Support Dart Source Compilation and / or Kernel Isolates
  * JIT from source or .dil
* An AOT Only .dll / .so

Additionally we may have a static target that uses the same interface as the dynamic library, so the VM can be embedded on platforms that don't support dynamic linking.

I also hope to support all platforms that Dart currently supports, plus a few extra.

## Using

Lastest builds of the libraries are now available for certain targets as artifacts from [Github Actions](https://github.com/fuzzybinary/dart_shared_libray/actions) as well as all the headers necessary.

Github Actions currently builds a Windows x64 `.dll`, A Linux x64 `.so`, and a macOS x64 `.dylib`. You can build a M-series dylib for macOS, but Github Actions does not currently support it.

## Building

### Prerequisets

You need:

* git
* Dart 3+
* C++ build tools for your platform (Visual Studio, XCode, gcc, etc)
* For Windows
  * 2019 16.61 with 10.0.20348.0 SDK don't forget install Debugger Tools
  * 2022 17 with ? SDK don't forget install Debugger Tools
  * 2017 15 with ? SDK don't forget install Debugger Tools
  * see dart-sdk\sdk\build\vs_toolchain.py
* CMake

Optionally, I recommend installing [`depot_tools`](https://www.chromium.org/developers/how-tos/depottools/) and making sure it is on your path before running setup scripts. Without depot_tools, the scripts will download them anyway, but having them already set up will save you some time with subsequent builds.

### Patching and Building Dart

> NOTE: If you are building on Windows, I recommend running `.\setup_env.ps1` before executing any other scripts.  
> This will set up some environment variables that will be needed to build Dart properly.

The first step is to build a statically linkable verison of Dart. This requires that we download Dart, patch some of the Dart build files, and then run the actual build. Thankfully there is a Dart script to do this.
build_dart commandline

* -v -> Verbose Log
* -t -> Build Type all, release, debug

```bash
cd ./scripts/build_helpers
dart pub get
cd ../..
dart ./scripts/build_helpers/bin/build_dart.dart
```

This script does the following:

* Pulls down `depot_tools` if needed.
* Clones a fresh copy of the Dart sdk git repo using `fetch` if needed.
* Uses `gsync` to syncs the repo the the version of dart specificed in `.dart_version`.
* Applies `dart_sdk.patch` to the repo to create the statically linkable `libdart` library
* Builds `libdart`

### CMake

Once Dart is built, you can use CMake to generate build files and / or build the libraries and examples

```bash
cmake -B ./.build .
cmake --build .\.build\ --config release
```
