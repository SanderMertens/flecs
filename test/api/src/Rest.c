#include <api.h>

void Rest_teardown() {
    ecs_world_t *world = ecs_init();

    ecs_singleton_set(world, EcsRest, {0});

    ecs_fini(world);

    test_assert(true); // Ensure teardown was successful
}
