#include <include/api.h>

void Has_zero() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t e = ecs_new(world, 0);
    test_assert(e != 0);

    test_assert(ecs_has(world, e, 0));
    
    ecs_fini(world);
}

void Has_zero_from_nonzero() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new(world, Position);
    test_assert(e != 0);

    test_assert(ecs_has(world, e, 0));
    
    ecs_fini(world);
}

void Has_1_of_0() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new(world, 0);
    test_assert(e != 0);

    test_assert(!ecs_has(world, e, Position));
    
    ecs_fini(world);
}

void Has_2_of_0() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TYPE(world, Type, Position, Velocity);

    ecs_entity_t e = ecs_new(world, 0);
    test_assert(e != 0);

    test_assert(!ecs_has(world, e, Type));
    
    ecs_fini(world);
}


void Has_1_of_1() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new(world, Position);
    test_assert(e != 0);

    test_assert(ecs_has(world, e, Position));
    
    ecs_fini(world);
}

void Has_1_of_2() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TYPE(world, Type, Position, Velocity);

    ecs_entity_t e = ecs_new(world, Type);
    test_assert(e != 0);

    test_assert(ecs_has(world, e, Position));
    test_assert(ecs_has(world, e, Velocity));
    
    ecs_fini(world);
}

void Has_2_of_2() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TYPE(world, Type, Position, Velocity);

    ecs_entity_t e = ecs_new(world, Type);
    test_assert(e != 0);

    test_assert(ecs_has(world, e, Type));
    
    ecs_fini(world);
}

void Has_3_of_2() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_TYPE(world, Type_1, Position, Velocity);
    ECS_TYPE(world, Type_2, Position, Velocity, Mass);

    ecs_entity_t e = ecs_new(world, Type_1);
    test_assert(e != 0);

    test_assert(!ecs_has(world, e, Type_2));
    
    ecs_fini(world);
}

void Has_2_of_1() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TYPE(world, Type, Position, Velocity);

    ecs_entity_t e = ecs_new(world, Position);
    test_assert(e != 0);

    test_assert(!ecs_has(world, e, Type));
    
    ecs_fini(world);
}

void Has_1_of_empty() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new(world, 0);
    test_assert(e != 0);

    test_assert(!ecs_has(world, e, Position));
    
    ecs_fini(world);
}

void Has_any_of_1_of_1() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new(world, Position);
    test_assert(e != 0);

    test_assert(ecs_has_any(world, e, Position));
    
    ecs_fini(world);
}

void Has_any_of_1_of_2() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TYPE(world, Type, Position, Velocity);

    ecs_entity_t e = ecs_new(world, Type);
    test_assert(e != 0);

    test_assert(ecs_has_any(world, e, Position));
    
    ecs_fini(world);
}

void Has_any_of_2_of_3() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_TYPE(world, Type_1, Position, Velocity, Mass);
    ECS_TYPE(world, Type_2, Position, Velocity);

    ecs_entity_t e = ecs_new(world, Type_1);
    test_assert(e != 0);

    test_assert(ecs_has_any(world, e, Type_2));
    
    ecs_fini(world);
}

void Has_any_of_3_of_2() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_TYPE(world, Type_1, Position, Velocity, Mass);
    ECS_TYPE(world, Type_2, Position, Velocity);

    ecs_entity_t e = ecs_new(world, Type_2);
    test_assert(e != 0);

    test_assert(ecs_has_any(world, e, Type_1));
    
    ecs_fini(world);
}

void Has_any_of_2_of_1() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TYPE(world, Type_1, Position, Velocity);
    ECS_TYPE(world, Type_2, Position);

    ecs_entity_t e = ecs_new(world, Type_2);
    test_assert(e != 0);

    test_assert(ecs_has_any(world, e, Type_1));
    
    ecs_fini(world);
}

void Has_any_of_1_of_0() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new(world, 0);
    test_assert(e != 0);

    test_assert(!ecs_has_any(world, e, Position));
    
    ecs_fini(world);
}

void Has_any_2_of_2_disjunct() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_COMPONENT(world, Rotation);
    ECS_TYPE(world, Type_1, Position, Velocity);
    ECS_TYPE(world, Type_2, Mass, Rotation);

    ecs_entity_t e = ecs_new(world, Type_1);
    test_assert(e != 0);

    test_assert(!ecs_has_any(world, e, Type_2));
    
    ecs_fini(world);
}

void TestHas(ecs_rows_t *rows) {
    ecs_entity_t *entities = ecs_column(rows, ecs_entity_t, 0);
    ecs_type_t ecs_to_type(Position) = ecs_column_type(rows, 1);

    int i;
    for (i = 0; i < rows->count; i ++) {
        test_assert( ecs_has(rows->world, entities[i], Position));
    }
}

void Has_has_in_progress() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_SYSTEM(world, TestHas, EcsOnFrame, Position);

    ecs_entity_t e = ecs_new(world, Position);
    test_assert(e != 0);
    test_assert( ecs_has(world, e, Position));

    ecs_progress(world, 1);
    
    ecs_fini(world);
}
