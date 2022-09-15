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

None. I literally just started.

## Using

`TODO:`

## Building

* Fetch the Dart SDK into this directory. Follow the instructions from [the dart repo](https://github.com/dart-lang/sdk/wiki/Building#source). Put it in `dart-sdk`. The code and scripts assume that this is where the dart-sdk lives.
* Build the Dart libraries
  * There is a Dart script for doing this that will do some checks for you as well:
  ```
  dart tools/build/build.dart
  ```

## Contributing

`TODO:`

# What I Did

This section is as much for my benefit as for yours.

* Modify the BUILD.gn files to add a `libdart` target. This is done by applying the patch in `./dart_sdk.patch`
* Make sure all correct environment variables are set
  * On Windows, these are set with the `setup_env.ps1` script. Specifically, you need to set `GYP_MSVS_OVERRIDE_PATH`, `GYP_MSVS_VERSION`, and `DEPOT_TOOLS_WIN_TOOLCHAIN=0`
* Builds libdart with:
  ```bash
  python ./tools/build.py --no-goma -m release libdart
  ```
* Revert the changes made to build files to leave a clean copy of the dart-sdk (this makes updating easier)
* Build the dynamic library using the Bazel script
  * This puts the output in `out/lib/platform` and `out/include`. The various examples look for these directories, and these can be copied for use in other projects.
