# Compiling Flecs with fil-C

This guide explains how to compile flecs with [fil-C](https://fil-c.org/) for memory safety checking.

## What is fil-C?

fil-C is a memory-safe implementation of C and C++ that catches all memory safety errors (buffer overflows, use-after-free, etc.) as runtime panics. It uses:
- Concurrent garbage collection
- Invisible capabilities (InvisiCaps)
- Runtime checks for all possibly-unsafe operations

## Prerequisites

⚠️ **Important**: fil-C currently only supports **Linux x86_64**. 

Since you're on macOS (Darwin arm64), you'll need to use one of these options:

### Option 1: Docker (Recommended)

Create a Docker container to run Linux x86_64:

```bash
# Create a Dockerfile
cat > Dockerfile.filc << 'EOF'
FROM --platform=linux/amd64 ubuntu:22.04

# Install dependencies
RUN apt-get update && apt-get install -y \
    wget \
    cmake \
    build-essential \
    git \
    xz-utils \
    && rm -rf /var/lib/apt/lists/*

# Set working directory
WORKDIR /workspace

# The flecs source will be mounted here
EOF

# Build the Docker image
docker build -f Dockerfile.filc -t flecs-filc .

# Run the container with flecs source mounted
docker run --platform linux/amd64 -it -v $(pwd):/workspace flecs-filc bash

# Inside the container, run:
./compile_with_filc.sh
```

### Option 2: QEMU/UTM Virtual Machine

1. Install [UTM](https://mac.getutm.app/) (free VM app for macOS)
2. Create an Ubuntu 22.04 x86_64 VM
3. Clone flecs repository inside the VM
4. Run the compilation script

### Option 3: Remote Linux Machine

Use a remote Linux x86_64 machine (cloud instance, server, etc.):

```bash
# Copy flecs to remote machine
scp -r . user@remote-linux-machine:/path/to/flecs

# SSH into the machine
ssh user@remote-linux-machine

# Navigate and run
cd /path/to/flecs
./compile_with_filc.sh
```

## Using the Compilation Script

Once you're on a Linux x86_64 system:

```bash
# Make the script executable
chmod +x compile_with_filc.sh

# Run the script (it will download and install fil-C if needed)
./compile_with_filc.sh

# The script will:
# 1. Check if fil-C is installed (and install it if not)
# 2. Configure flecs with CMake using fil-C compiler
# 3. Build flecs with memory safety checks
```

## Testing with fil-C

After building, test the compiled library:

```bash
# Compile the test program
~/filc-0.674-linux-x86_64/build/bin/clang \
    -I./include \
    -L./build_filc \
    -lflecs_static \
    test_filc.c \
    -o test_filc \
    -lpthread

# Run the test
./test_filc
```

The test program will:
- Create entities and components
- Test memory management with 1000 entities
- Query entities
- Any memory safety violations will trigger fil-C panics

## Manual Compilation

If you prefer manual control:

```bash
# Download fil-C (if not already installed)
wget https://github.com/pizlonator/fil-c/releases/download/v0.674/filc-0.674-linux-x86_64.tar.xz
tar -xf filc-0.674-linux-x86_64.tar.xz
cd filc-0.674-linux-x86_64
./setup.sh
cd ..

# Configure flecs with fil-C
mkdir build_filc
cd build_filc
cmake .. \
    -DCMAKE_C_COMPILER="$HOME/filc-0.674-linux-x86_64/build/bin/clang" \
    -DCMAKE_BUILD_TYPE=Debug \
    -DFLECS_STATIC=ON

# Build
cmake --build . -j$(nproc)
```

## Checking for Memory Issues

fil-C will automatically detect and report:
- Buffer overflows
- Use-after-free
- Double free
- Invalid pointer dereferences
- Memory leaks (via garbage collection)

When a memory safety violation is detected, fil-C will:
1. Print a detailed error message with stack trace
2. Panic and terminate the program

Example output for a buffer overflow:
```
Fil-C panic: out of bounds access at 0x7fff12345000
  Expected bounds: [0x7fff12340000, 0x7fff12344fff]
  Actual access: 0x7fff12345000
  Stack trace:
    ...
```

## Environment Variables

You can customize fil-C behavior:

```bash
# Set fil-C installation directory
export FILC_DIR="$HOME/my-filc-installation"

# Run the compilation script
./compile_with_filc.sh
```

## Troubleshooting

### Platform Mismatch Error

If you see:
```
Error: fil-C currently only supports Linux x86_64
Current platform: Darwin arm64
```

You need to use Docker, VM, or remote Linux machine (see prerequisites above).

### Build Errors

If compilation fails:
1. Check that fil-C is properly installed: `ls ~/filc-0.674-linux-x86_64/build/bin/clang`
2. Ensure all dependencies are installed: `cmake`, `build-essential`
3. Try with a fresh build directory: `rm -rf build_filc && ./compile_with_filc.sh`

### Runtime Errors

If tests crash unexpectedly:
- This might indicate a real memory safety issue in the code!
- Check the fil-C error message for details
- Review the stack trace to identify the problematic code

## Additional Resources

- [fil-C Official Documentation](https://fil-c.org/documentation.html)
- [fil-C GitHub Repository](https://github.com/pizlonator/fil-c)
- [fil-C Discord Community](https://discord.gg/dPyNUaeajg)
- [Programs That Work With fil-C](https://fil-c.org/programs_that_work.html)

## Notes

- fil-C uses musl as the libc in the pizfix distribution
- Compilation is slower than regular clang due to instrumentation
- Runtime performance is slower due to safety checks
- This is expected - the goal is to find memory safety issues during development
