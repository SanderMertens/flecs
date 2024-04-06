#include <core.h>

void Count_count_empty(void) {
    ecs_world_t *world = ecs_mini();

    test_int(ecs_count_id(world, 0), 0);

    ecs_fini(world);
}

void Count_count_w_entity_0(void) {
    ecs_world_t *world = ecs_mini();

    test_int(ecs_count_id(world, 0), 0);

    ecs_fini(world);
}

void Count_count_1_component(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ECS_ENTITY(world, e1, Position);
    ECS_ENTITY(world, e2, Position);

    test_int(ecs_count(world, Position), 2);

    ecs_fini(world);
}

void Count_count_disabled(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e1 = ecs_new_w(world, Position);
    ecs_new_w(world, Position);

    test_int(ecs_count(world, Position), 2);

    ecs_enable(world, e1, false);

    test_int(ecs_count(world, Position), 2);

    ecs_fini(world);
}

void Count_count_prefab(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e1 = ecs_new_w(world, Position);
    ecs_new_w(world, Position);

    test_int(ecs_count(world, Position), 2);

    ecs_add_id(world, e1, EcsPrefab);

    test_int(ecs_count(world, Position), 2);

    ecs_fini(world);
}
