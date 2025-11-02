#!/bin/bash
# Script to compile flecs with fil-C for memory safety checking
# 
# Prerequisites:
# - Linux x86_64 system (fil-C currently only supports this platform)
# - fil-C installed (see instructions below)
#
# Note: This script is designed to run on Linux x86_64.
# If you're on macOS, you'll need to use Docker, a VM, or a remote Linux machine.

set -e  # Exit on error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo -e "${GREEN}=== Flecs fil-C Compilation Script ===${NC}"

# Check if we're on Linux x86_64
if [[ "$(uname -s)" != "Linux" ]] || [[ "$(uname -m)" != "x86_64" ]]; then
    echo -e "${RED}Error: fil-C currently only supports Linux x86_64${NC}"
    echo -e "${YELLOW}Current platform: $(uname -s) $(uname -m)${NC}"
    echo ""
    echo "To use this script, you need to:"
    echo "  1. Use Docker/Podman to run a Linux x86_64 container, or"
    echo "  2. Use a Linux x86_64 VM, or"
    echo "  3. Use a remote Linux x86_64 machine"
    echo ""
    echo "See README_FILC.md for Docker instructions"
    exit 1
fi

# Configuration
FILC_VERSION="0.674"
FILC_DIR="${FILC_DIR:-$HOME/filc-${FILC_VERSION}-linux-x86_64}"
FILC_URL="https://github.com/pizlonator/fil-c/releases/download/v${FILC_VERSION}/filc-${FILC_VERSION}-linux-x86_64.tar.xz"
BUILD_DIR="build_filc"

# Function to check if fil-C is installed
check_filc_installed() {
    if [[ -f "${FILC_DIR}/build/bin/clang" ]]; then
        echo -e "${GREEN}✓ fil-C found at ${FILC_DIR}${NC}"
        return 0
    else
        return 1
    fi
}

# Function to install fil-C
install_filc() {
    echo -e "${YELLOW}fil-C not found. Installing...${NC}"
    
    # Create temporary directory
    TEMP_DIR=$(mktemp -d)
    cd "$TEMP_DIR"
    
    echo "Downloading fil-C ${FILC_VERSION}..."
    wget "${FILC_URL}" || curl -LO "${FILC_URL}"
    
    echo "Extracting fil-C..."
    tar -xf "filc-${FILC_VERSION}-linux-x86_64.tar.xz"
    
    # Move to home directory or specified location
    INSTALL_DIR=$(dirname "${FILC_DIR}")
    mkdir -p "${INSTALL_DIR}"
    mv "filc-${FILC_VERSION}-linux-x86_64" "${FILC_DIR}"
    
    # Run setup
    cd "${FILC_DIR}"
    echo "Running fil-C setup..."
    ./setup.sh
    
    # Cleanup
    cd -
    rm -rf "$TEMP_DIR"
    
    echo -e "${GREEN}✓ fil-C installed successfully${NC}"
}

# Check and install fil-C if needed
if ! check_filc_installed; then
    install_filc
fi

# Get the flecs root directory (assuming script is in flecs root)
FLECS_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "${FLECS_ROOT}"

echo ""
echo -e "${GREEN}=== Compiling flecs with fil-C ===${NC}"
echo "flecs root: ${FLECS_ROOT}"
echo "fil-C path: ${FILC_DIR}"
echo "Build directory: ${BUILD_DIR}"
echo ""

# Create build directory
mkdir -p "${BUILD_DIR}"
cd "${BUILD_DIR}"

# Configure with CMake using fil-C
echo "Configuring with CMake..."
cmake .. \
    -DCMAKE_C_COMPILER="${FILC_DIR}/build/bin/clang" \
    -DCMAKE_CXX_COMPILER="${FILC_DIR}/build/bin/clang++" \
    -DCMAKE_BUILD_TYPE=Debug \
    -DFLECS_STATIC=ON \
    -DFLECS_SHARED=ON \
    -DFLECS_TESTS=OFF

echo ""
echo "Building flecs..."
cmake --build . -j$(nproc)

echo ""
echo -e "${GREEN}=== Build complete! ===${NC}"
echo ""
echo "Build artifacts are in: ${FLECS_ROOT}/${BUILD_DIR}"
echo ""
echo "To run a simple test:"
echo "  1. Create a test program (see test_filc.c)"
echo "  2. Compile it with: ${FILC_DIR}/build/bin/clang -I${FLECS_ROOT}/include -L${FLECS_ROOT}/${BUILD_DIR} -lflecs_static test_filc.c -o test_filc"
echo "  3. Run: ./test_filc"
echo ""
echo "Any memory safety violations will be caught and reported by fil-C!"
