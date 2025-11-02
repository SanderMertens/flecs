/*
 * Simple test program to verify flecs compilation with fil-C
 * This program creates entities and components to test basic functionality
 */

#include <stdio.h>
#include "flecs.h"

typedef struct {
    float x, y;
} Position;

typedef struct {
    float x, y;
} Velocity;

int main(int argc, char *argv[]) {
    printf("Testing flecs compiled with fil-C for memory safety...\n\n");

    // Initialize flecs world
    ecs_world_t *world = ecs_init();
    if (!world) {
        fprintf(stderr, "Failed to initialize flecs world\n");
        return 1;
    }

    // Register components
    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    printf("✓ Components registered\n");

    // Create an entity with components
    ecs_entity_t e = ecs_new(world);
    ecs_set(world, e, Position, {10.0f, 20.0f});
    ecs_set(world, e, Velocity, {1.0f, 2.0f});

    printf("✓ Entity created with components\n");

    // Get components
    const Position *p = ecs_get(world, e, Position);
    const Velocity *v = ecs_get(world, e, Velocity);

    if (p && v) {
        printf("✓ Components retrieved: Position(%.1f, %.1f), Velocity(%.1f, %.1f)\n",
               p->x, p->y, v->x, v->y);
    } else {
        fprintf(stderr, "Failed to retrieve components\n");
        ecs_fini(world);
        return 1;
    }

    // Create multiple entities to test memory management
    printf("\nCreating 1000 entities to test memory management...\n");
    for (int i = 0; i < 1000; i++) {
        ecs_entity_t test_e = ecs_new(world);
        ecs_set(world, test_e, Position, {(float)i, (float)i * 2});
        ecs_set(world, test_e, Velocity, {1.0f, 1.0f});
    }
    printf("✓ 1000 entities created successfully\n");

    // Query entities
    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { .id = ecs_id(Position) },
            { .id = ecs_id(Velocity) }
        }
    });

    int count = 0;
    ecs_iter_t it = ecs_query_iter(world, q);
    while (ecs_query_next(&it)) {
        count += it.count;
    }
    printf("✓ Query found %d entities with Position and Velocity\n", count);

    ecs_query_fini(q);

    // Cleanup
    ecs_fini(world);

    printf("\n✓ All tests passed! No memory safety violations detected by fil-C.\n");
    return 0;
}
