#include <api.h>

void Count_count_empty() {
    ecs_world_t *world = ecs_init();

    test_int(ecs_count(world, 0), 0);

    ecs_fini(world);
}

void Count_count_w_entity_0() {
    ecs_world_t *world = ecs_init();

    test_int(ecs_count_id(world, 0), 0);

    ecs_fini(world);
}

void Count_count_1_component() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_ENTITY(world, e1, Position);
    ECS_ENTITY(world, e2, Position);

    test_int(ecs_count(world, Position), 2);

    ecs_fini(world);
}

void Count_count_2_components() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TYPE(world, Type, Position, Velocity);

    ECS_ENTITY(world, e1, AND | Type);
    ECS_ENTITY(world, e2, AND | Type);
    ECS_ENTITY(world, e3, Position);
    ECS_ENTITY(world, e4, Velocity);

    test_int(ecs_count(world, Type), 2);

    ecs_fini(world);
}

void Count_count_3_components() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_TYPE(world, Type, Position, Velocity, Mass);

    ECS_ENTITY(world, e1, AND | Type);
    ECS_ENTITY(world, e2, AND | Type);
    ECS_ENTITY(world, e3, Position);
    ECS_ENTITY(world, e4, Velocity);
    ECS_ENTITY(world, e5, Position, Velocity);
    ECS_ENTITY(world, e6, Position, Mass);
    ECS_ENTITY(world, e7, Velocity, Mass);

    test_int(ecs_count(world, Type), 2);

    ecs_fini(world);
}

void Count_count_2_types_2_comps() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_TYPE(world, Type, Position, Velocity);

    ECS_ENTITY(world, e1, AND | Type);
    ECS_ENTITY(world, e2, AND | Type);
    ECS_ENTITY(world, e3, Position);
    ECS_ENTITY(world, e4, Velocity);
    ECS_ENTITY(world, e5, Position, Velocity, Mass);
    ECS_ENTITY(world, e6, Position, Mass);
    ECS_ENTITY(world, e7, Velocity, Mass);

    test_int(ecs_count(world, Type), 3);

    ecs_fini(world);
}
