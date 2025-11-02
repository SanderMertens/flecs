# Quick Start: Compiling Flecs with fil-C

This guide helps you quickly get started with memory safety checking using fil-C.

## For macOS Users (Quick Start)

Since you're on macOS and fil-C only supports Linux x86_64, the easiest way is to use Docker:

### Prerequisites
- Install [Docker Desktop](https://www.docker.com/products/docker-desktop/) for Mac

### One-Command Build and Test

```bash
# Build Docker image, compile flecs with fil-C, and run tests
./docker_filc_build.sh
```

That's it! The script will:
1. ✅ Build a Docker image with fil-C pre-installed
2. ✅ Compile flecs with memory safety checks
3. ✅ Run test program to verify everything works
4. ✅ Report any memory safety issues found

### Interactive Mode

Want to explore and experiment?

```bash
# Start an interactive shell in the container
./docker_filc_build.sh shell

# Inside the container, you can:
# - Modify and rebuild: cmake --build build_filc
# - Run tests: ./test_filc
# - Add your own test files
```

## Understanding the Output

### Success Output
```
Testing flecs compiled with fil-C for memory safety...

✓ Components registered
✓ Entity created with components
✓ Components retrieved: Position(10.0, 20.0), Velocity(1.0, 2.0)

Creating 1000 entities to test memory management...
✓ 1000 entities created successfully
✓ Query found 1001 entities with Position and Velocity

✓ All tests passed! No memory safety violations detected by fil-C.
```

### Memory Safety Violation Example
If fil-C detects an issue, you'll see:
```
Fil-C panic: out of bounds access at 0x7fff12345000
  Expected bounds: [0x7fff12340000, 0x7fff12344fff]
  Actual access: 0x7fff12345000
  
  This is a buffer overflow!
  
  Stack trace:
    #0 my_function at src/file.c:42
    #1 main at test.c:100
```

## Writing Your Own Tests

Create a new test file (e.g., `my_test.c`):

```c
#include <stdio.h>
#include "flecs.h"

int main() {
    ecs_world_t *world = ecs_init();
    
    // Your test code here
    
    ecs_fini(world);
    return 0;
}
```

Compile and run in Docker:
```bash
./docker_filc_build.sh shell

# Inside container:
/home/builder/filc-0.674-linux-x86_64/build/bin/clang \
    -I./include -L./build_filc -lflecs_static \
    my_test.c -o my_test -lpthread
./my_test
```

## Common Memory Issues fil-C Catches

1. **Buffer Overflows**
   ```c
   char buffer[10];
   buffer[10] = 'x';  // ❌ fil-C will catch this!
   ```

2. **Use-After-Free**
   ```c
   void *ptr = malloc(100);
   free(ptr);
   ptr[0] = 'x';  // ❌ fil-C will catch this!
   ```

3. **Double Free**
   ```c
   void *ptr = malloc(100);
   free(ptr);
   free(ptr);  // ❌ fil-C will catch this!
   ```

4. **Invalid Pointer Access**
   ```c
   int *ptr = NULL;
   *ptr = 42;  // ❌ fil-C will catch this!
   ```

## Performance Notes

- **Compilation**: Slower than regular builds (instrumentation overhead)
- **Runtime**: Slower than native (safety checks on every operation)
- **Memory**: Higher usage (garbage collection metadata)

This is expected and acceptable for testing and validation!

## Troubleshooting

### Docker not found
```bash
# Install Docker Desktop for macOS
# Download from: https://www.docker.com/products/docker-desktop/
```

### Permission denied
```bash
chmod +x docker_filc_build.sh
```

### Build fails
```bash
# Clean and rebuild
rm -rf build_filc
./docker_filc_build.sh
```

## Next Steps

- Read [README_FILC.md](./README_FILC.md) for detailed documentation
- Check [fil-C documentation](https://fil-c.org/documentation.html)
- Join [fil-C Discord](https://discord.gg/dPyNUaeajg) for support

## CI/CD Integration

Want to run fil-C checks automatically? See [.github/workflows/filc-check.yml](.github/workflows/filc-check.yml) for a GitHub Actions example.

## Questions?

- 💬 Open an issue in the flecs repository
- 💬 Ask on the [flecs Discord](https://discord.gg/BEzP5Rgrrp)
- 💬 Ask on the [fil-C Discord](https://discord.gg/dPyNUaeajg)
