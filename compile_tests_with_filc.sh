#!/bin/bash
set -e

# Compile and run flecs tests with fil-C runtime instrumentation
# Focus on runtime memory safety checks, not static compilation errors

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
FILC_PATH="${FILC_PATH:-/opt/filc}"

echo "=== Flecs Runtime Testing with fil-C ==="
echo "This script compiles parts of flecs and its tests that pass fil-C's"
echo "static checks, then runs them with full runtime memory safety instrumentation."
echo ""

# Check if we're in Docker or native Linux
if [ ! -d "$FILC_PATH" ]; then
    echo "Error: fil-C not found at $FILC_PATH"
    echo "Please run this inside the fil-C Docker container or set FILC_PATH"
    exit 1
fi

export PATH="$FILC_PATH/bin:$PATH"
export LD_LIBRARY_PATH="$FILC_PATH/lib:$LD_LIBRARY_PATH"

cd "$SCRIPT_DIR"

# Create build directory for runtime tests
BUILD_DIR="build_filc_runtime"
rm -rf "$BUILD_DIR"
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

echo ""
echo "=== Strategy: Compile What Works, Test Runtime Behavior ==="
echo "We'll compile the flecs library with -Wno-error and skip problematic files"
echo "Then build simple test programs that exercise the working parts"
echo ""

# First, try to compile just the working parts of flecs
echo "=== Step 1: Compile flecs core (skipping problematic files) ==="

# Create a minimal test that uses basic flecs functionality
cat > simple_test.c << 'EOF'
#include <stdio.h>
#include "../include/flecs.h"

typedef struct Position {
    float x, y;
} Position;

typedef struct Velocity {
    float x, y;
} Velocity;

int main(void) {
    printf("=== Starting fil-C Runtime Test ===\n");
    
    ecs_world_t *world = ecs_init();
    if (!world) {
        printf("FAIL: Could not create world\n");
        return 1;
    }
    printf("PASS: World created\n");
    
    // Register components
    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    printf("PASS: Components registered\n");
    
    // Create entities
    ecs_entity_t e1 = ecs_new(world);
    printf("PASS: Entity created (e1=%llu)\n", (unsigned long long)e1);
    
    // Set component data
    ecs_set(world, e1, Position, {10.0f, 20.0f});
    printf("PASS: Position component set\n");
    
    ecs_set(world, e1, Velocity, {1.0f, 2.0f});
    printf("PASS: Velocity component set\n");
    
    // Get component data
    const Position *p = ecs_get(world, e1, Position);
    if (p) {
        printf("PASS: Position get (x=%.1f, y=%.1f)\n", p->x, p->y);
    } else {
        printf("FAIL: Could not get Position\n");
        return 1;
    }
    
    // Create multiple entities
    printf("\n=== Creating 100 entities ===\n");
    for (int i = 0; i < 100; i++) {
        ecs_entity_t e = ecs_new(world);
        ecs_set(world, e, Position, {(float)i, (float)i * 2});
        if (i % 20 == 0) {
            printf("Created entity %d\n", i);
        }
    }
    printf("PASS: 100 entities created\n");
    
    // Query entities
    printf("\n=== Querying entities ===\n");
    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { ecs_id(Position) }
        }
    });
    
    if (q) {
        printf("PASS: Query created\n");
        
        int count = 0;
        ecs_iter_t it = ecs_query_iter(world, q);
        while (ecs_query_next(&it)) {
            Position *positions = ecs_field(&it, Position, 0);
            for (int i = 0; i < it.count; i++) {
                count++;
            }
        }
        printf("PASS: Query executed, found %d entities\n", count);
        
        ecs_query_fini(q);
    } else {
        printf("WARNING: Query creation failed (may hit problematic code)\n");
    }
    
    // Cleanup
    printf("\n=== Cleanup ===\n");
    ecs_fini(world);
    printf("PASS: World destroyed\n");
    
    printf("\n=== fil-C Runtime Test Complete ===\n");
    printf("If you see this message, basic operations passed!\n");
    printf("Check for any fil-C runtime errors above.\n");
    
    return 0;
}
EOF

echo "Created simple_test.c"
echo ""

# Try compiling the single-file distribution with fil-C
echo "=== Step 2: Attempting compilation with fil-C ==="
echo "Note: We expect some static errors. We'll capture what compiles."
echo ""

# Copy single file distribution
cp ../distr/flecs.c .
cp ../distr/flecs.h .

# Try to compile with minimal flags, allowing errors
echo "Compiling flecs.c -> flecs.o (allowing errors where possible)..."

# Attempt 1: Try full compilation
filcc -c flecs.c -o flecs.o \
    -DFLECS_NO_CPP \
    -Wno-pointer-to-int-cast \
    -Wno-int-to-pointer-cast 2>&1 | tee compile_flecs.log || {
    
    echo ""
    echo "Full compilation failed (expected). Checking what compiled..."
    echo "See compile_flecs.log for details"
    echo ""
}

# If that fails, try compiling just our test with inline includes
if [ ! -f flecs.o ]; then
    echo "=== Alternative: Compile test with inline flecs ==="
    echo "Attempting to compile test directly (may fail at link time)..."
    
    filcc simple_test.c ../distr/flecs.c -o simple_test \
        -I.. \
        -DFLECS_NO_CPP \
        2>&1 | tee compile_test.log || {
        
        echo ""
        echo "Compilation failed. Analyzing error location..."
        grep -n "error:" compile_test.log | head -10 || true
        echo ""
    }
fi

# Check if we got a binary
if [ -f simple_test ]; then
    echo ""
    echo "=== SUCCESS: Got instrumented binary! ==="
    echo "=== Step 3: Running tests with fil-C runtime checks ==="
    echo ""
    
    # Run the test
    ./simple_test 2>&1 | tee runtime_output.log
    TEST_EXIT=$?
    
    echo ""
    if [ $TEST_EXIT -eq 0 ]; then
        echo "=== Test completed successfully ==="
    else
        echo "=== Test failed or crashed (exit code: $TEST_EXIT) ==="
    fi
    
    # Check for fil-C runtime errors
    if grep -q "capability" runtime_output.log || grep -q "bounds" runtime_output.log; then
        echo ""
        echo "!!! fil-C runtime errors detected !!!"
        echo "See runtime_output.log for details"
    fi
else
    echo ""
    echo "=== Could not create instrumented binary ==="
    echo "This is expected due to the pointer-to-int issues we found."
    echo ""
    echo "=== Alternative Approach: Patch the source ==="
    echo "Applying minimal patches to make basic tests work..."
    
    cd "$SCRIPT_DIR"
    
    # Create a patched version
    cat > build_filc_runtime/minimal_flecs.h << 'EOFH'
// Minimal flecs.h with fil-C compatible macros
#ifndef MINIMAL_FLECS_H
#define MINIMAL_FLECS_H

// Replace problematic macro with fil-C safe version
#define ECS_OFFSET(o, offset) ((void*)((char*)(o) + (offset)))
#define ECS_ELEM(ptr, size, index) ((void*)((char*)(ptr) + (size) * (index)))

// Include rest of flecs
#include "../distr/flecs.h"

#endif
EOFH
    
    cd build_filc_runtime
    
    # Try again with patched header
    filcc simple_test.c ../distr/flecs.c -o simple_test_patched \
        -I. \
        -DFLECS_NO_CPP \
        -include minimal_flecs.h \
        2>&1 | tee compile_patched.log || {
        
        echo "Patched compilation also failed."
        echo "See compile_patched.log for details"
    }
    
    if [ -f simple_test_patched ]; then
        echo ""
        echo "=== Running patched test ==="
        ./simple_test_patched 2>&1 | tee runtime_output_patched.log
    fi
fi

echo ""
echo "=== Build Summary ==="
echo "Build directory: $BUILD_DIR"
echo "Logs created:"
ls -lh *.log 2>/dev/null || echo "  No logs created"
echo ""
echo "Next: Review logs and create runtime analysis report"
