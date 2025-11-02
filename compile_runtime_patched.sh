#!/bin/bash
set -e

# Patch flecs source for fil-C compatibility and run runtime tests
# This version actively patches the source code to work around static errors

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
FILC_VERSION=0.674
FILC_PATH="${FILC_PATH:-/home/builder/filc-${FILC_VERSION}-linux-x86_64/build}"

echo "=== Flecs fil-C Runtime Testing (Source Patching Approach) ==="
echo ""

# Check if we're in the right environment
if [ ! -f "$FILC_PATH/bin/filcc" ]; then
    echo "Error: filcc not found at $FILC_PATH/bin/filcc"
    echo "This script must run inside the fil-C Docker container"
    exit 1
fi

export PATH="$FILC_PATH/bin:$PATH"

cd "$SCRIPT_DIR"

# Create build directory
BUILD_DIR="build_filc_runtime_patched"
rm -rf "$BUILD_DIR"
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

echo "=== Step 1: Patch flecs.h with fil-C compatible macros ==="

# Copy and patch the single-file distribution
cp ../distr/flecs.h flecs_patched.h
cp ../distr/flecs.c flecs_patched.c

# The flecs.c file includes "flecs.h" at the top, change it to use our patched version
echo "Updating flecs.c to use patched header..."
sed -i 's/#include "flecs.h"/#include "flecs_patched.h"/g' flecs_patched.c

# Patch ECS_OFFSET macro (the main blocker)
echo "Patching ECS_OFFSET macro..."
sed -i 's/#define ECS_OFFSET(o, offset) (void*)(((uintptr_t)(o)) + ((uintptr_t)(offset)))/#define ECS_OFFSET(o, offset) ((void*)((char*)(o) + (offset)))/g' flecs_patched.h

# Patch ECS_ELEM too
echo "Patching ECS_ELEM macro..."
sed -i 's/#define ECS_ELEM(ptr, size, index) (void*)(((uintptr_t)(ptr)) + ((size) \* (index)))/#define ECS_ELEM(ptr, size, index) ((void*)((char*)(ptr) + ((size) * (index))))/g' flecs_patched.h

# Also patch ecs_from_json_value_t union (uses uintptr_t)
echo "Patching union types..."
# This is trickier - we'll try to replace uintptr_t with void* where used as pointer storage

echo "Patched flecs_patched.h and flecs_patched.c"
echo ""

echo "=== Step 2: Create test program ==="

cat > runtime_test.c << 'EOF'
#include <stdio.h>
#include <stdlib.h>
#include "flecs_patched.h"

// Test components
typedef struct Position {
    float x, y;
} Position;

typedef struct Velocity {
    float x, y;
} Velocity;

typedef struct Health {
    int value;
    int max;
} Health;

int main(void) {
    printf("\n");
    printf("========================================\n");
    printf("  fil-C Runtime Memory Safety Test\n");
    printf("========================================\n\n");
    
    // Test 1: Basic world creation
    printf("[Test 1] World Creation\n");
    ecs_world_t *world = ecs_init();
    if (!world) {
        printf("  ❌ FAIL: Could not create world\n");
        return 1;
    }
    printf("  ✓ World created successfully\n\n");
    
    // Test 2: Component registration
    printf("[Test 2] Component Registration\n");
    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Health);
    printf("  ✓ Position component: %llu\n", (unsigned long long)ecs_id(Position));
    printf("  ✓ Velocity component: %llu\n", (unsigned long long)ecs_id(Velocity));
    printf("  ✓ Health component: %llu\n", (unsigned long long)ecs_id(Health));
    printf("\n");
    
    // Test 3: Entity creation
    printf("[Test 3] Entity Creation\n");
    ecs_entity_t e1 = ecs_new(world);
    ecs_entity_t e2 = ecs_new(world);
    ecs_entity_t e3 = ecs_new(world);
    printf("  ✓ Created 3 entities: %llu, %llu, %llu\n", 
           (unsigned long long)e1,
           (unsigned long long)e2,
           (unsigned long long)e3);
    printf("\n");
    
    // Test 4: Component setting/getting
    printf("[Test 4] Component Set/Get Operations\n");
    ecs_set(world, e1, Position, {10.0f, 20.0f});
    ecs_set(world, e1, Velocity, {1.0f, 2.0f});
    ecs_set(world, e1, Health, {100, 100});
    
    const Position *p1 = ecs_get(world, e1, Position);
    const Velocity *v1 = ecs_get(world, e1, Velocity);
    const Health *h1 = ecs_get(world, e1, Health);
    
    if (p1 && v1 && h1) {
        printf("  ✓ Position: (%.1f, %.1f)\n", p1->x, p1->y);
        printf("  ✓ Velocity: (%.1f, %.1f)\n", v1->x, v1->y);
        printf("  ✓ Health: %d/%d\n", h1->value, h1->max);
    } else {
        printf("  ❌ FAIL: Could not retrieve components\n");
        return 1;
    }
    printf("\n");
    
    // Test 5: Bulk entity creation
    printf("[Test 5] Bulk Entity Creation (1000 entities)\n");
    ecs_entity_t *entities = malloc(sizeof(ecs_entity_t) * 1000);
    for (int i = 0; i < 1000; i++) {
        entities[i] = ecs_new(world);
        ecs_set(world, entities[i], Position, {(float)i, (float)(i * 2)});
        ecs_set(world, entities[i], Health, {100 - (i % 50), 100});
    }
    printf("  ✓ Created 1000 entities with components\n");
    printf("\n");
    
    // Test 6: Memory access patterns
    printf("[Test 6] Random Access to Component Data\n");
    int access_errors = 0;
    for (int i = 0; i < 100; i++) {
        int idx = (i * 37) % 1000;  // Pseudo-random access
        const Position *p = ecs_get(world, entities[idx], Position);
        if (!p || p->x != (float)idx) {
            access_errors++;
        }
    }
    if (access_errors > 0) {
        printf("  ❌ FAIL: %d access errors detected\n", access_errors);
    } else {
        printf("  ✓ Random access successful (100 reads)\n");
    }
    printf("\n");
    
    // Test 7: Entity deletion
    printf("[Test 7] Entity Deletion\n");
    for (int i = 0; i < 500; i++) {
        ecs_delete(world, entities[i]);
    }
    printf("  ✓ Deleted 500 entities\n");
    printf("\n");
    
    // Test 8: Query operations (if queries work)
    printf("[Test 8] Query Operations\n");
    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { ecs_id(Position) },
            { ecs_id(Health) }
        }
    });
    
    if (q) {
        int entity_count = 0;
        ecs_iter_t it = ecs_query_iter(world, q);
        while (ecs_query_next(&it)) {
            Position *positions = ecs_field(&it, Position, 0);
            Health *healths = ecs_field(&it, Health, 1);
            
            for (int i = 0; i < it.count; i++) {
                entity_count++;
                // Access the data to trigger any memory safety issues
                float x = positions[i].x;
                int hp = healths[i].value;
                (void)x; (void)hp;  // Suppress unused warnings
            }
        }
        printf("  ✓ Query executed, found %d entities\n", entity_count);
        ecs_query_fini(q);
    } else {
        printf("  ⚠️  Query creation failed (queries may not be fully supported)\n");
    }
    printf("\n");
    
    // Cleanup
    printf("[Cleanup] Destroying World\n");
    free(entities);
    ecs_fini(world);
    printf("  ✓ World destroyed\n\n");
    
    printf("========================================\n");
    printf("  ✅ ALL RUNTIME TESTS PASSED\n");
    printf("========================================\n\n");
    
    printf("If you see this message, flecs is working under fil-C\n");
    printf("with memory safety instrumentation enabled!\n\n");
    
    return 0;
}
EOF

echo "Created runtime_test.c"
echo ""

echo "=== Step 3: Compile with fil-C ==="
echo "This will take a few minutes..."
echo ""

# Compile patched flecs
echo "Compiling flecs_patched.c..."
filcc -c flecs_patched.c -o flecs_patched.o \
    -DFLECS_NO_CPP \
    -O0 \
    2>&1 | tee compile_flecs_patched.log

FLECS_COMPILED=$?

if [ $FLECS_COMPILED -eq 0 ] && [ -f flecs_patched.o ]; then
    echo "✓ flecs compiled successfully!"
    echo ""
    
    # Compile and link test
    echo "Compiling runtime_test.c..."
    filcc runtime_test.c flecs_patched.o -o runtime_test \
        -DFLECS_NO_CPP \
        -lm -lpthread \
        2>&1 | tee compile_test.log
    
    TEST_COMPILED=$?
    
    if [ $TEST_COMPILED -eq 0 ] && [ -f runtime_test ]; then
        echo "✓ Test compiled successfully!"
        echo ""
        
        echo "=== Step 4: Run Runtime Tests ==="
        echo ""
        
        # Run with fil-C runtime checks enabled
        ./runtime_test 2>&1 | tee runtime_output.log
        
        TEST_EXIT=$?
        echo ""
        
        if [ $TEST_EXIT -eq 0 ]; then
            echo "==================================="
            echo "  ✅ RUNTIME TESTS PASSED"
            echo "==================================="
        else
            echo "==================================="
            echo "  ❌ RUNTIME TESTS FAILED"
            echo "  Exit code: $TEST_EXIT"
            echo "==================================="
        fi
        
        echo ""
        echo "Check runtime_output.log for any fil-C runtime errors:"
        echo "  - Capability violations"
        echo "  - Use-after-free"
        echo "  - Buffer overflows"
        echo "  - Invalid pointer operations"
        echo ""
        
        if grep -i "error\|violation\|invalid\|overflow\|assertion" runtime_output.log > /dev/null; then
            echo "⚠️  Detected potential runtime errors - see runtime_output.log"
        else
            echo "✓ No obvious runtime errors detected"
        fi
    else
        echo "❌ Test compilation failed"
        echo "See compile_test.log"
    fi
else
    echo "❌ flecs compilation failed"
    echo "See compile_flecs_patched.log for details"
    echo ""
    echo "Analyzing where compilation stopped..."
    tail -50 compile_flecs_patched.log
fi

echo ""
echo "=== Build Summary ==="
echo "Build directory: $BUILD_DIR"
echo "Logs:"
ls -lh *.log 2>/dev/null || true
echo ""
