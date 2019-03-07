#include <include/api.h>

typedef struct Position {
    float x;
    float y;
} Position;

typedef struct Velocity {
    float x;
    float y;
} Velocity;

typedef float Mass;

void Remove_zero() {
    EcsWorld *world = ecs_init();

    EcsEntity e = ecs_new(world, 0);
    test_assert(e != 0);

    ecs_remove(world, e, 0);
    test_assert(ecs_empty(world, e));
    
    ecs_fini(world);
}

void Remove_zero_from_nonzero() {
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);

    EcsEntity e = ecs_new(world, Position);
    test_assert(e != 0);

    ecs_remove(world, e, 0);
    test_assert(ecs_has(world, e, Position));
    
    ecs_fini(world);
}

void Remove_1_of_1() {
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);

    EcsEntity e = ecs_new(world, Position);
    test_assert(e != 0);

    ecs_remove(world, e, Position);
    test_assert(!ecs_has(world, e, Position));
    
    ecs_fini(world);
}

void Remove_1_of_1_again() {
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);

    EcsEntity e = ecs_new(world, Position);
    test_assert(e != 0);

    ecs_remove(world, e, Position);
    test_assert(!ecs_has(world, e, Position));

    ecs_remove(world, e, Position);
    test_assert(!ecs_has(world, e, Position));
    
    ecs_fini(world);
}

void Remove_1_of_2() {
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TYPE(world, Type, Position, Velocity);

    EcsEntity e = ecs_new(world, Type);
    test_assert(e != 0);

    ecs_remove(world, e, Position);
    test_assert(!ecs_has(world, e, Position));
    test_assert(ecs_has(world, e, Velocity));
    
    ecs_fini(world);
}

void Remove_2_of_2() {
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TYPE(world, Type, Position, Velocity);

    EcsEntity e = ecs_new(world, Type);
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
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_TYPE(world, Type, Position, Velocity, Mass);

    EcsEntity e = ecs_new(world, Type);
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
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TYPE(world, Type, Position, Velocity);

    EcsEntity e = ecs_new(world, Type);
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
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_TYPE(world, Type, Position, Velocity, Mass);

    EcsEntity e = ecs_new(world, Type);
    test_assert(e != 0);

    ecs_remove(world, e, Position);
    ecs_remove(world, e, Velocity);
    test_assert(!ecs_has(world, e, Position));
    test_assert(!ecs_has(world, e, Velocity));
    test_assert(ecs_has(world, e, Mass));
    
    ecs_fini(world);
}

void Remove_type_of_1_of_2() {
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TYPE(world, Type_1, Position, Velocity);
    ECS_TYPE(world, Type_2, Position);

    EcsEntity e = ecs_new(world, Type_1);
    test_assert(e != 0);

    ecs_remove(world, e, Type_2);
    test_assert(!ecs_has(world, e, Position));
    test_assert(ecs_has(world, e, Velocity));
    
    ecs_fini(world);
}

void Remove_type_of_2_of_2() {
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TYPE(world, Type, Position, Velocity);

    EcsEntity e = ecs_new(world, Type);
    test_assert(e != 0);

    ecs_remove(world, e, Type);
    test_assert(!ecs_has(world, e, Position));
    test_assert(!ecs_has(world, e, Velocity));
    
    ecs_fini(world);
}

void Remove_type_of_2_of_3() {
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_TYPE(world, Type_1, Position, Velocity, Mass);
    ECS_TYPE(world, Type_2, Position, Velocity);

    EcsEntity e = ecs_new(world, Type_1);
    test_assert(e != 0);

    ecs_remove(world, e, Type_2);
    test_assert(!ecs_has(world, e, Position));
    test_assert(!ecs_has(world, e, Velocity));
    test_assert(ecs_has(world, e, Mass));
    
    ecs_fini(world);
}

void Remove_1_from_empty() {
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);

    EcsEntity e = ecs_new(world, 0);
    test_assert(e != 0);

    ecs_remove(world, e, Position);
    test_assert(!ecs_has(world, e, Position));
    
    ecs_fini(world);
}

void Remove_type_from_empty() {
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_TYPE(world, Type, Position);

    EcsEntity e = ecs_new(world, 0);
    test_assert(e != 0);

    ecs_remove(world, e, Type);
    test_assert(!ecs_has(world, e, Position));
    
    ecs_fini(world);
}
