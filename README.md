# Dart Dynamic Import Library

This is an attempt / POC to build the Dart VM into a dynamic library, importable in any platform Dart supports.

This uses the same build system any style as the Dart SDK, partially so that it can be built from a parallel checkout of the Dart SDK source, and (potentially, maybe one day) be brought into the Dart SDK directly.

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

`TODO:`

## Contributing

`TODO:`
