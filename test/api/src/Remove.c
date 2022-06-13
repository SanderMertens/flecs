#include <api.h>

void Remove_zero() {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ecs_entity_t e = ecs_new(world, 0);
    test_assert(e != 0);

    test_expect_abort();
    ecs_remove(world, e, 0);
}

void Remove_1_of_1() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new(world, Position);
    test_assert(e != 0);

    ecs_remove(world, e, Position);
    test_assert(!ecs_has(world, e, Position));
    
    ecs_fini(world);
}

void Remove_1_of_1_again() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new(world, Position);
    test_assert(e != 0);

    ecs_remove(world, e, Position);
    test_assert(!ecs_has(world, e, Position));

    ecs_remove(world, e, Position);
    test_assert(!ecs_has(world, e, Position));
    
    ecs_fini(world);
}

void Remove_1_of_2() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_ENTITY(world, e, Position, Velocity);
    test_assert(e != 0);

    ecs_remove(world, e, Position);
    test_assert(!ecs_has(world, e, Position));
    test_assert(ecs_has(world, e, Velocity));
    
    ecs_fini(world);
}

void Remove_2_of_2() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_ENTITY(world, e, Position, Velocity);
    test_assert(e != 0);

    ecs_remove(world, e, Position);
    test_assert(!ecs_has(world, e, Position));
    test_assert(ecs_has(world, e, Velocity));

    ecs_remove(world, e, Velocity);
    test_assert(!ecs_has(world, e, Position));
    test_assert(!ecs_has(world, e, Velocity));
    
    ecs_fini(world);
}

void Remove_2_of_3() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);

    ECS_ENTITY(world, e, Position, Velocity, Mass);
    test_assert(e != 0);

    ecs_remove(world, e, Position);
    test_assert(!ecs_has(world, e, Position));
    test_assert(ecs_has(world, e, Velocity));
    test_assert(ecs_has(world, e, Mass));

    ecs_remove(world, e, Velocity);
    test_assert(!ecs_has(world, e, Position));
    test_assert(!ecs_has(world, e, Velocity));
    test_assert(ecs_has(world, e, Mass));
    
    ecs_fini(world);
}

void Remove_2_again() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_ENTITY(world, e, Position, Velocity);
    test_assert(e != 0);

    ecs_remove(world, e, Position);
    ecs_remove(world, e, Velocity);
    test_assert(!ecs_has(world, e, Position));
    test_assert(!ecs_has(world, e, Velocity));

    ecs_remove(world, e, Position);
    ecs_remove(world, e, Velocity);
    test_assert(!ecs_has(world, e, Position));
    test_assert(!ecs_has(world, e, Velocity));
    
    ecs_fini(world);
}

void Remove_2_overlap() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);

    ECS_ENTITY(world, e, Position, Velocity, Mass);
    test_assert(e != 0);

    ecs_remove(world, e, Position);
    ecs_remove(world, e, Velocity);
    test_assert(!ecs_has(world, e, Position));
    test_assert(!ecs_has(world, e, Velocity));
    test_assert(ecs_has(world, e, Mass));
    
    ecs_fini(world);
}

void Remove_1_from_empty() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new(world, 0);
    test_assert(e != 0);

    ecs_remove(world, e, Position);
    test_assert(!ecs_has(world, e, Position));
    
    ecs_fini(world);
}

void Remove_not_added() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t e = ecs_new(world, Position);
    test_assert(e != 0);

    ecs_remove(world, e, Velocity);
    test_assert(ecs_has(world, e, Position));
    test_assert(!ecs_has(world, e, Velocity));

    ecs_fini(world);
}
