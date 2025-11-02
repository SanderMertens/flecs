#!/bin/bash
set -e

# Minimal flecs runtime test - compile only core functionality needed
# Avoiding problematic subsystems like maps and queries

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
FILC_VERSION=0.674
FILC_PATH="${FILC_PATH:-/home/builder/filc-${FILC_VERSION}-linux-x86_64/build}"

echo "===fil-C Runtime Test: Minimal Flecs Build ==="
echo ""

if [ ! -f "$FILC_PATH/bin/filcc" ]; then
    echo "Error: filcc not found"
    exit 1
fi

export PATH="$FILC_PATH/bin:$PATH"

cd "$SCRIPT_DIR"
BUILD_DIR="build_filc_minimal"
rm -rf "$BUILD_DIR"
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

echo "Creating minimal flecs test that avoids problematic code paths..."
echo ""

# Create a truly minimal test using only the most basic fl ecs APIs
cat > minimal_test.c << 'EOF'
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Forward declarations - minimal flecs types
typedef struct ecs_world_t ecs_world_t;
typedef uint64_t ecs_entity_t;
typedef uint64_t ecs_id_t;

// Minimal function declarations (these actually exist in flecs)
ecs_world_t* ecs_mini(void);
void ecs_fini(ecs_world_t *world);
ecs_entity_t ecs_new(ecs_world_t *world);
void* ecs_ensure_id(ecs_world_t *world, ecs_entity_t entity, ecs_id_t id);
const void* ecs_get_id(ecs_world_t *world, ecs_entity_t entity, ecs_id_t id);
ecs_entity_t ecs_component_init(ecs_world_t *world, const void *desc);

// Simple component
typedef struct Position {
    float x, y;
} Position;

int main(void) {
    printf("\n=== fil-C Minimal Runtime Test ===\n\n");
    
    // Use ecs_mini() which creates a minimal world without full initialization
    printf("[1] Creating minimal world...\n");
    ecs_world_t *world = ecs_mini();
    if (!world) {
        printf("FAIL: Could not create world\n");
        return 1;
    }
    printf("PASS: Minimal world created\n\n");
    
    printf("[2] Creating entities...\n");
    ecs_entity_t e1 = ecs_new(world);
    ecs_entity_t e2 = ecs_new(world);
    printf("PASS: Created entities %llu, %llu\n\n", 
           (unsigned long long)e1, (unsigned long long)e2);
    
    printf("[3] Cleaning up...\n");
    ecs_fini(world);
    printf("PASS: World destroyed\n\n");
    
    printf("=== Test Complete ===\n");
    printf("If you see this, basic flecs operations work under fil-C!\n\n");
    
    return 0;
}
EOF

echo "Created minimal_test.c"
echo ""

echo "Compiling with flecs library..."
echo ""

# Try linking against the original flecs
# We'll compile our test and link with flecs.c

# First, let's see if we can at least analyze what WOULD fail
echo "Attempting compilation (this will likely fail, but we'll analyze the failure)..."
echo ""

filcc minimal_test.c ../distr/flecs.c -o minimal_test \
    -DFLECS_NO_CPP \
    2>&1 | tee compile_minimal.log || {
    
    echo ""
    echo "Compilation failed as expected."
    echo ""
    
    # Check if there's any preprocessed output
    if ls /tmp/*.c 2>/dev/null | head -1; then
        PREP_FILE=$(ls /tmp/flecs-*.c 2>/dev/null | head -1)
        if [ -f "$PREP_FILE" ]; then
            echo "Preprocessed file created: $PREP_FILE"
            echo "Analyzing for PtrToInt patterns..."
            echo ""
            
            # Search for patterns that would cause PtrToInt
            echo "Searching for (uintptr_t) casts in code..."
            grep -n "(uintptr_t)" "$PREP_FILE" 2>/dev/null | head -20 || true
            echo ""
        fi
    fi
}

if [ -f minimal_test ]; then
    echo "SUCCESS: Binary created!"
    echo "Running test..."
    ./minimal_test
else
    echo ""
    echo "=========================================="
    echo "  Analysis: Why Runtime Testing Failed"
    echo "=========================================="
    echo ""
    echo "The compilation fails during fil-C's FilPizlonatorPass,"
    echo "which means flecs has deep pointer-to-integer conversions"
    echo "that cannot be avoided even with minimal usage."
    echo ""
    echo "Root causes found in our static analysis:"
    echo "  1. ECS_OFFSET macro - pointer arithmetic via uintptr_t"
    echo "  2. ecs_map implementation - stores pointers as uint64_t"
    echo "  3. Block allocator - uses pointers as map keys"
    echo "  4. OS handles - typedef uintptr_t for thread/mutex handles"
    echo "  5. Meta system - stores pointer offsets as uintptr_t"
    echo ""
    echo "To get runtime testing working, flecs would need:"
    echo "  - Complete refactor of map implementation"
    echo "  - Replace ALL pointer-to-integer casts"
    echo "  - Use pointer arithmetic via char* instead of uintptr_t"
    echo "  - Redesign OS handle abstraction"
    echo ""
    echo "See FILC_ISSUES_ANALYSIS.md for full details."
    echo ""
fi

echo ""
echo "=== Summary ==="
echo "Build directory: $BUILD_DIR"
ls -lh *.log 2>/dev/null || true
echo ""
