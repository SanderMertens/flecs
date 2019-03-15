#include <include/api.h>

void Has_zero() {
    EcsWorld *world = ecs_init();

    EcsEntity e = ecs_new(world, 0);
    test_assert(e != 0);

    test_assert(ecs_has(world, e, 0));
    
    ecs_fini(world);
}

void Has_zero_from_nonzero() {
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);

    EcsEntity e = ecs_new(world, Position);
    test_assert(e != 0);

    test_assert(ecs_has(world, e, 0));
    
    ecs_fini(world);
}

void Has_1_of_0() {
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);

    EcsEntity e = ecs_new(world, 0);
    test_assert(e != 0);

    test_assert(!ecs_has(world, e, Position));
    
    ecs_fini(world);
}

void Has_2_of_0() {
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TYPE(world, Type, Position, Velocity);

    EcsEntity e = ecs_new(world, 0);
    test_assert(e != 0);

    test_assert(!ecs_has(world, e, Type));
    
    ecs_fini(world);
}


void Has_1_of_1() {
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);

    EcsEntity e = ecs_new(world, Position);
    test_assert(e != 0);

    test_assert(ecs_has(world, e, Position));
    
    ecs_fini(world);
}

void Has_1_of_2() {
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TYPE(world, Type, Position, Velocity);

    EcsEntity e = ecs_new(world, Type);
    test_assert(e != 0);

    test_assert(ecs_has(world, e, Position));
    test_assert(ecs_has(world, e, Velocity));
    
    ecs_fini(world);
}

void Has_2_of_2() {
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TYPE(world, Type, Position, Velocity);

    EcsEntity e = ecs_new(world, Type);
    test_assert(e != 0);

    test_assert(ecs_has(world, e, Type));
    
    ecs_fini(world);
}

void Has_3_of_2() {
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_TYPE(world, Type_1, Position, Velocity);
    ECS_TYPE(world, Type_2, Position, Velocity, Mass);

    EcsEntity e = ecs_new(world, Type_1);
    test_assert(e != 0);

    test_assert(!ecs_has(world, e, Type_2));
    
    ecs_fini(world);
}

void Has_2_of_1() {
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TYPE(world, Type, Position, Velocity);

    EcsEntity e = ecs_new(world, Position);
    test_assert(e != 0);

    test_assert(!ecs_has(world, e, Type));
    
    ecs_fini(world);
}

void Has_1_of_empty() {
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);

    EcsEntity e = ecs_new(world, 0);
    test_assert(e != 0);

    test_assert(!ecs_has(world, e, Position));
    
    ecs_fini(world);
}

void Has_any_of_1_of_1() {
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);

    EcsEntity e = ecs_new(world, Position);
    test_assert(e != 0);

    test_assert(ecs_has_any(world, e, Position));
    
    ecs_fini(world);
}

void Has_any_of_1_of_2() {
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TYPE(world, Type, Position, Velocity);

    EcsEntity e = ecs_new(world, Type);
    test_assert(e != 0);

    test_assert(ecs_has_any(world, e, Position));
    
    ecs_fini(world);
}

void Has_any_of_2_of_3() {
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_TYPE(world, Type_1, Position, Velocity, Mass);
    ECS_TYPE(world, Type_2, Position, Velocity);

    EcsEntity e = ecs_new(world, Type_1);
    test_assert(e != 0);

    test_assert(ecs_has_any(world, e, Type_2));
    
    ecs_fini(world);
}

void Has_any_of_3_of_2() {
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_TYPE(world, Type_1, Position, Velocity, Mass);
    ECS_TYPE(world, Type_2, Position, Velocity);

    EcsEntity e = ecs_new(world, Type_2);
    test_assert(e != 0);

    test_assert(ecs_has_any(world, e, Type_1));
    
    ecs_fini(world);
}

void Has_any_of_2_of_1() {
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TYPE(world, Type_1, Position, Velocity);
    ECS_TYPE(world, Type_2, Position);

    EcsEntity e = ecs_new(world, Type_2);
    test_assert(e != 0);

    test_assert(ecs_has_any(world, e, Type_1));
    
    ecs_fini(world);
}

void Has_any_of_1_of_0() {
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);

    EcsEntity e = ecs_new(world, 0);
    test_assert(e != 0);

    test_assert(!ecs_has_any(world, e, Position));
    
    ecs_fini(world);
}

void Has_any_2_of_2_disjunct() {
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_COMPONENT(world, Rotation);
    ECS_TYPE(world, Type_1, Position, Velocity);
    ECS_TYPE(world, Type_2, Mass, Rotation);

    EcsEntity e = ecs_new(world, Type_1);
    test_assert(e != 0);

    test_assert(!ecs_has_any(world, e, Type_2));
    
    ecs_fini(world);
}

void TestHas(EcsRows *rows) {
    EcsEntity *entities = ecs_column(rows, EcsEntity, 0);
    EcsType TPosition = ecs_column_type(rows, 1);

    int i;
    for (i = rows->begin; i < rows->end; i ++) {
        test_assert( ecs_has(rows->world, entities[i], Position));
    }
}

void Has_has_in_progress() {
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_SYSTEM(world, TestHas, EcsOnFrame, Position);

    EcsEntity e = ecs_new(world, Position);
    test_assert(e != 0);
    test_assert( ecs_has(world, e, Position));

    ecs_progress(world, 1);
    
    ecs_fini(world);
}
