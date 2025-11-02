#!/bin/bash
# Quick script to build and run flecs with fil-C using Docker
# This works on macOS with Docker Desktop or Podman

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

echo -e "${BLUE}=== Flecs fil-C Docker Build ===${NC}"

# Check if Docker is available
if command -v docker &> /dev/null; then
    CONTAINER_CMD="docker"
    echo -e "${GREEN}✓ Using Docker${NC}"
elif command -v podman &> /dev/null; then
    CONTAINER_CMD="podman"
    echo -e "${GREEN}✓ Using Podman${NC}"
else
    echo -e "${RED}Error: Neither Docker nor Podman is installed${NC}"
    echo ""
    echo "Please install Docker Desktop from: https://www.docker.com/products/docker-desktop/"
    echo "Or install Podman from: https://podman.io/getting-started/installation"
    exit 1
fi

# Get the script directory (flecs root)
FLECS_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
IMAGE_NAME="flecs-filc"

echo "Flecs root: ${FLECS_ROOT}"
echo ""

# Build the Docker image
echo -e "${YELLOW}Building Docker image (this may take a few minutes on first run)...${NC}"
$CONTAINER_CMD build --platform linux/amd64 -t ${IMAGE_NAME} -f Dockerfile.filc .

echo ""
echo -e "${GREEN}✓ Docker image built successfully${NC}"
echo ""

# Check if user wants interactive shell or just build
if [[ "$1" == "shell" ]] || [[ "$1" == "-i" ]] || [[ "$1" == "--interactive" ]]; then
    echo -e "${BLUE}Starting interactive shell in container...${NC}"
    echo -e "${YELLOW}Run './compile_with_filc.sh' inside the container to build flecs${NC}"
    echo ""
    $CONTAINER_CMD run --platform linux/amd64 --rm -it \
        -v "${FLECS_ROOT}:/workspace" \
        ${IMAGE_NAME} /bin/bash
else
    echo -e "${YELLOW}Building flecs with fil-C...${NC}"
    echo ""
    $CONTAINER_CMD run --platform linux/amd64 --rm \
        -v "${FLECS_ROOT}:/workspace" \
        ${IMAGE_NAME} /bin/bash -c "
            echo '=== Building flecs with fil-C ===' && \
            mkdir -p build_filc && \
            cd build_filc && \
            cmake .. \
                -DCMAKE_C_COMPILER=/home/builder/filc-0.674-linux-x86_64/build/bin/clang \
                -DCMAKE_CXX_COMPILER=/home/builder/filc-0.674-linux-x86_64/build/bin/clang++ \
                -DCMAKE_BUILD_TYPE=Debug \
                -DFLECS_STATIC=ON \
                -DFLECS_SHARED=ON \
                -DFLECS_TESTS=OFF && \
            cmake --build . -j\$(nproc) && \
            echo '' && \
            echo '=== Build complete! ===' && \
            echo '' && \
            echo 'Build artifacts are in: build_filc/' && \
            echo '' && \
            echo '=== Running test program ===' && \
            cd .. && \
            /home/builder/filc-0.674-linux-x86_64/build/bin/clang \
                -I./include \
                -L./build_filc \
                -lflecs_static \
                test_filc.c \
                -o test_filc \
                -lpthread && \
            ./test_filc
        "
    
    echo ""
    echo -e "${GREEN}=== All done! ===${NC}"
    echo ""
    echo "The flecs library has been compiled with fil-C and tested."
    echo "Build artifacts are in: build_filc/"
    echo ""
    echo "To run an interactive shell in the container:"
    echo "  ./docker_filc_build.sh shell"
    echo ""
    echo "Inside the container, you can:"
    echo "  - Modify code and rebuild"
    echo "  - Run additional tests"
    echo "  - Debug memory safety issues"
fi
