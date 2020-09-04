#include <api.h>

ECS_COMPONENT_DECLARE(Position);
ECS_COMPONENT_DECLARE(Velocity);

ecs_entity_t create_entity(ecs_world_t *world) {
    return ecs_new(world, Position);
}

void GlobalComponentIds_declare() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT_DEFINE(world, Position);

    ecs_entity_t e = create_entity(world);
    test_assert(e != 0);
    test_assert(ecs_has(world, e, Position));

    ecs_fini(world);
}

void GlobalComponentIds_declare_2_world() {    
    ecs_world_t *world_1 = ecs_init();

    ECS_COMPONENT_DEFINE(world_1, Position);
    ECS_COMPONENT_DEFINE(world_1, Velocity);

    ecs_entity_t e_pos = ecs_entity(Position);
    ecs_entity_t e_vel = ecs_entity(Velocity);

    ecs_world_t *world_2 = ecs_init();

    /* Declare in reverse order, ensure that ids are still the same */
    ECS_COMPONENT_DEFINE(world_2, Velocity);
    ECS_COMPONENT_DEFINE(world_2, Position);

    test_assert(e_pos = ecs_entity(Position));
    test_assert(e_vel = ecs_entity(Velocity));

    ecs_fini(world_1);
    ecs_fini(world_2);
}
