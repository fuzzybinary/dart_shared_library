# Dart Dynamic Import Library

This is an attempt / POC to build the Dart VM into a dynamic library, importable in any platform Dart supports.

## Eventual support

The hope is that the the dynamic library will eventually support the following targets:
* A "Fully Featured" .dll / .so that supports booting Dart in different configurations:
  * Boot (or not) the service and kernel isolates
  * Support Dart Source Compilation and / or Kernel Isolates
  * Support "AOT only" mode? (aka, just the runtime)
* A JIT Only .dll / .so
* A AOT Only .dll / .so (aka, just the runtime)

Additionally we may have a static target that uses the same interface as the dynamic library, so the VM can be embedded on platforms that don't support dynamic linking.

I also hope to support all platforms that Dart currently supports, plus a few extra.

## Current Progress

A very, very simple Dart script works.

## Using

`TODO:`

## Building

### Prerequisets
You need
* git
* All the things to get Dart source - https://github.com/dart-lang/sdk/wiki/Building#source
* C++ build tools for your platform (Visual Studio, XCode, gcc, etc)

## Contributing

`TODO:`

# What I Did

This section is as much for my benefit as for yours. Eventually, I hope to make a script that will do most of this.

* Modify the BUILD.gn files to add a `libdart` target. This is done by applying the patch in `./dart_sdk.patch`
* Make sure all correct environment variables are set
  * On Windows, these are set with the `setup_env.ps1` script. Specifically, you need to set `GYP_MSVS_OVERRIDE_PATH`, `GYP_MSVS_VERSION`, and `DEPOT_TOOLS_WIN_TOOLCHAIN=0`
* Builds libdart with:
  ```bash
  python ./tools/build.py --no-goma -m release libdart
  ```
* Revert the changes made to build files to leave a clean copy of the dart-sdk (this makes updating easier)
* Generate build scripts with genie (located in tools/[platform]) e.g.
  ```bash
  tools/win/genie.exe vs2022
  ```
* Note only the Visual Studio target will work right now, as it's the only one I've tested.
* Open the generated build files (in ./.build/projects/[arch_platform]) and build the project.
* Build.
* Run one of the examples!
