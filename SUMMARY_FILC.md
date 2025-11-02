# Flecs + fil-C Integration Summary

This document summarizes the fil-C integration setup for memory safety checking in the flecs project.

## What Was Set Up

A complete environment for compiling and testing flecs with [fil-C](https://fil-c.org/), a memory-safe implementation of C/C++ that catches all memory safety violations.

## Files Created

### 1. Quick Start Scripts

- **`docker_filc_build.sh`** ⭐ **START HERE**
  - One-command solution for macOS users
  - Automatically builds Docker image, compiles flecs, and runs tests
  - Usage: `./docker_filc_build.sh`
  - Interactive mode: `./docker_filc_build.sh shell`

- **`compile_with_filc.sh`**
  - For direct use on Linux x86_64 systems
  - Auto-downloads and installs fil-C if needed
  - Configures and builds flecs with memory safety checks

### 2. Docker Setup

- **`Dockerfile.filc`**
  - Ubuntu 22.04 x86_64 image
  - Pre-installs fil-C 0.674
  - Includes all necessary build tools
  - Ready to compile flecs

### 3. Test Program

- **`test_filc.c`**
  - Comprehensive test program
  - Tests entity creation, components, and queries
  - Creates 1000 entities to stress-test memory management
  - Validates that fil-C can catch memory issues

### 4. Documentation

- **`QUICKSTART_FILC.md`** 📖 **Quick reference**
  - Concise guide for getting started
  - Common use cases and examples
  - Troubleshooting tips

- **`README_FILC.md`** 📚 **Detailed guide**
  - Complete documentation
  - Platform requirements and workarounds
  - Manual compilation instructions
  - Advanced configuration options

### 5. CI/CD Integration

- **`.github/workflows/filc-check.yml`**
  - GitHub Actions workflow
  - Automatically tests memory safety on push/PR
  - Can be enabled for continuous validation

## Quick Start (macOS)

```bash
# One command to build and test everything:
./docker_filc_build.sh
```

That's it! The script will:
1. Build Docker image with fil-C
2. Compile flecs with memory safety checks
3. Run test program
4. Report any memory safety issues

## Quick Start (Linux x86_64)

```bash
# One command to install fil-C, build, and test:
./compile_with_filc.sh
```

## What fil-C Catches

✅ **Buffer overflows** - Accessing beyond array bounds  
✅ **Use-after-free** - Using freed memory  
✅ **Double free** - Freeing memory twice  
✅ **NULL pointer dereferences** - Accessing NULL pointers  
✅ **Invalid pointer arithmetic** - Incorrect pointer math  
✅ **Memory leaks** - Via garbage collection  

## Expected Output

### Success
```
Testing flecs compiled with fil-C for memory safety...

✓ Components registered
✓ Entity created with components
✓ Components retrieved: Position(10.0, 20.0), Velocity(1.0, 2.0)
✓ 1000 entities created successfully
✓ Query found 1001 entities with Position and Velocity
✓ All tests passed! No memory safety violations detected by fil-C.
```

### Memory Safety Violation Detected
```
Fil-C panic: out of bounds access at 0x7fff12345000
  Expected bounds: [0x7fff12340000, 0x7fff12344fff]
  Actual access: 0x7fff12345000
  Stack trace:
    #0 problematic_function at src/file.c:42
    #1 main at test.c:100
```

## Architecture

```
┌─────────────────────────────────────────┐
│ macOS (your system)                     │
│                                         │
│  ┌─────────────────────────────────┐  │
│  │ Docker Container (Linux x86_64) │  │
│  │                                 │  │
│  │  ┌──────────────────────────┐  │  │
│  │  │ fil-C Compiler           │  │  │
│  │  │ (Memory Safety Checks)   │  │  │
│  │  └──────────────────────────┘  │  │
│  │            ↓                    │  │
│  │  ┌──────────────────────────┐  │  │
│  │  │ Flecs Library            │  │  │
│  │  │ (Compiled with fil-C)    │  │  │
│  │  └──────────────────────────┘  │  │
│  │            ↓                    │  │
│  │  ┌──────────────────────────┐  │  │
│  │  │ Test Program             │  │  │
│  │  │ (test_filc.c)            │  │  │
│  │  └──────────────────────────┘  │  │
│  └─────────────────────────────────┘  │
│                                         │
│  Volume Mount: flecs/ ←→ /workspace   │
└─────────────────────────────────────────┘
```

## Performance Characteristics

| Aspect | Impact | Notes |
|--------|--------|-------|
| **Compilation Time** | 2-3x slower | Due to instrumentation |
| **Binary Size** | Larger | Includes safety metadata |
| **Runtime Speed** | 2-10x slower | Runtime safety checks |
| **Memory Usage** | Higher | GC metadata overhead |

⚠️ **This is expected and acceptable for testing/validation purposes!**

## Use Cases

### Development
- ✅ Find memory bugs during development
- ✅ Validate fixes for memory issues
- ✅ Test new features for memory safety

### Testing
- ✅ Add to CI/CD pipeline
- ✅ Nightly builds with memory checks
- ✅ Pre-release validation

### Debugging
- ✅ Reproduce hard-to-find memory bugs
- ✅ Get detailed stack traces
- ✅ Understand memory access patterns

## Workflow Examples

### 1. Quick Validation
```bash
# Build and test in one go
./docker_filc_build.sh
```

### 2. Interactive Development
```bash
# Start interactive shell
./docker_filc_build.sh shell

# Inside container:
# Edit code, rebuild, test iteratively
cmake --build build_filc
./test_filc
```

### 3. Custom Tests
```bash
./docker_filc_build.sh shell

# Inside container:
# Write your test
cat > my_test.c << 'EOF'
#include "flecs.h"
int main() {
    ecs_world_t *world = ecs_init();
    // Your test code
    ecs_fini(world);
    return 0;
}
EOF

# Compile and run
/home/builder/filc-0.674-linux-x86_64/build/bin/clang \
    -I./include -L./build_filc -lflecs_static \
    my_test.c -o my_test -lpthread
./my_test
```

### 4. CI Integration
```bash
# Enable the GitHub Actions workflow
git add .github/workflows/filc-check.yml
git commit -m "Add fil-C memory safety checks to CI"
git push
```

## Directory Structure

```
flecs/
├── docker_filc_build.sh       # Main entry point (macOS)
├── compile_with_filc.sh       # Linux native script
├── Dockerfile.filc            # Docker image definition
├── test_filc.c                # Test program
├── QUICKSTART_FILC.md         # Quick reference
├── README_FILC.md             # Detailed docs
├── SUMMARY_FILC.md            # This file
├── .github/workflows/
│   └── filc-check.yml         # CI workflow
└── build_filc/                # Generated build directory
    ├── libflecs_static.a      # Compiled library
    └── ...
```

## Resources

- 🌐 [fil-C Official Website](https://fil-c.org/)
- 📚 [fil-C Documentation](https://fil-c.org/documentation.html)
- 💬 [fil-C Discord](https://discord.gg/dPyNUaeajg)
- 📦 [fil-C GitHub Releases](https://github.com/pizlonator/fil-c/releases)
- 🎮 [Flecs Discord](https://discord.gg/BEzP5Rgrrp)

## Contributing

Found a memory safety issue? Please report it!

1. Run fil-C to reproduce the issue
2. Capture the fil-C error output
3. Create an issue with:
   - Error message and stack trace
   - Minimal reproduction case
   - Environment details

## License Notes

- **fil-C Compiler**: Apache 2.0
- **fil-C Runtime**: BSD 2-clause
- **Flecs**: MIT (unchanged)

## Next Steps

1. ✅ **Run the quick start**: `./docker_filc_build.sh`
2. 📖 **Read the docs**: Check `QUICKSTART_FILC.md`
3. 🧪 **Write tests**: Create custom test programs
4. 🔄 **Enable CI**: Use the GitHub Actions workflow
5. 🐛 **Find bugs**: Use fil-C to validate memory safety

---

**Questions or Issues?**
- Check the documentation: `README_FILC.md`
- Ask on Discord: [fil-C](https://discord.gg/dPyNUaeajg) or [Flecs](https://discord.gg/BEzP5Rgrrp)
- Open an issue on GitHub

**Happy memory-safe coding! 🛡️**
