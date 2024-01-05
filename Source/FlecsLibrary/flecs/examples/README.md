# Examples
This folder contains code examples for the C and C++ APIs.

## Build with cmake
To build the examples with cmake, run the following commands from the root of the repository:

```
cd examples/c
mkdir cmake_build
cd cmake_build
cmake ..
cmake --build .
```

To build the C++ examples, run the same commands from examples/cpp:

```
cd examples/cpp
mkdir cmake_build
cd cmake_build
cmake ..
cmake --build .
```

This will produce two binaries for each example: one that links with a shared Flecs library, and one that is linked with static Flecs library. The binaries linked with the static library have the postfix _static.

To run a single example, execute it from the `cmake_build` directory:

```
./entities_basics_static
```

## Build with bake
To build the examples with bake, run bake once in the root of the repository, so it knows where to find Flecs:

```
bake
```

To run a single example, use the following command:

```
bake run examples/c/entities/basics
```

To run a single example with optimizations enabled, use the following command:

```
bake run examples/c/entities/basics --cfg release
```

To build all examples, run the following command:

```
bake examples
```

See the [bake repository](https://github.com/SanderMertens/bake) for instructions on how to install bake.
