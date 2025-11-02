#!/bin/bash
set -e

# Docker wrapper for runtime testing flecs with fil-C
# Focuses on dynamic testing rather than static compilation

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

echo "=== Flecs fil-C Runtime Testing (Docker) ==="
echo ""

# Detect Docker or Podman
if command -v docker &> /dev/null; then
    DOCKER_CMD="docker"
    echo "Using: Docker"
elif command -v podman &> /dev/null; then
    DOCKER_CMD="podman"
    echo "Using: Podman"
else
    echo "Error: Neither docker nor podman found"
    echo "Please install Docker Desktop from https://www.docker.com/products/docker-desktop"
    exit 1
fi

# Build the Docker image if needed
IMAGE_NAME="flecs-filc:latest"
echo ""
echo "Checking for Docker image..."

if ! $DOCKER_CMD images | grep -q flecs-filc; then
    echo "Building Docker image (this may take a few minutes)..."
    $DOCKER_CMD build -f Dockerfile.filc -t $IMAGE_NAME .
else
    echo "Docker image exists: $IMAGE_NAME"
fi

# Run the runtime test script
echo ""
echo "=== Running runtime tests in container ==="
echo ""

# Set FILC_PATH to match Dockerfile location
FILC_VERSION=0.674
DOCKER_FILC_PATH="/home/builder/filc-${FILC_VERSION}-linux-x86_64/build"

$DOCKER_CMD run --rm \
    --platform linux/amd64 \
    -v "$SCRIPT_DIR:/workspace" \
    -w /workspace \
    -e FILC_PATH="$DOCKER_FILC_PATH" \
    $IMAGE_NAME \
    bash -c "chmod +x /workspace/compile_tests_with_filc.sh && /workspace/compile_tests_with_filc.sh"

DOCKER_EXIT=$?

echo ""
if [ $DOCKER_EXIT -eq 0 ]; then
    echo "=== Runtime testing completed ==="
else
    echo "=== Runtime testing failed (exit code: $DOCKER_EXIT) ==="
fi

echo ""
echo "Results are in: build_filc_runtime/"
echo ""
echo "To analyze the results, review:"
echo "  - build_filc_runtime/runtime_output.log (runtime behavior)"
echo "  - build_filc_runtime/compile_*.log (compilation attempts)"
echo ""
echo "Next step: Create runtime analysis report with:"
echo "  'Analyze the runtime test results and create FILC_RUNTIME_ANALYSIS.md'"

exit $DOCKER_EXIT
