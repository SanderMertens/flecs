#include <api.h>

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

void Has_any_of_0_0() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t e = ecs_new(world, 0);
    test_assert(e != 0);

    test_assert(ecs_has_any(world, e, 0));
    
    ecs_fini(world);
}

void Has_any_of_0_1() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new(world, Position);
    test_assert(e != 0);
    test_assert( ecs_has(world, e, Position));

    test_assert(ecs_has_any(world, e, 0));
    
    ecs_fini(world);
}

void Has_any_of_0_zero_entity() {
    ecs_world_t *world = ecs_init();

    test_assert(!ecs_has_any(world, 0, 0));
    
    ecs_fini(world);
}

void TestHas(ecs_rows_t *rows) {
    ecs_type_t ecs_type(Position) = ecs_column_type(rows, 1);

    int i;
    for (i = 0; i < rows->count; i ++) {
        test_assert( ecs_has(rows->world, rows->entities[i], Position));
    }
}

void Has_has_in_progress() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_SYSTEM(world, TestHas, EcsOnUpdate, Position);

    ecs_entity_t e = ecs_new(world, Position);
    test_assert(e != 0);
    test_assert( ecs_has(world, e, Position));

    ecs_progress(world, 1);
    
    ecs_fini(world);
}

void Has_has_of_zero() {
    ecs_world_t *world = ecs_init();

    test_assert(!ecs_has(world, 0, 0));
    
    ecs_fini(world);
}

void Has_has_owned() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t e = ecs_new(world, Position);
    test_assert(e != 0);
    test_assert( ecs_has(world, e, Position));
    test_assert( ecs_has_owned(world, e, Position));

    ecs_entity_t base = ecs_new(world, Velocity);
    test_assert( ecs_has(world, base, Velocity));
    test_assert( ecs_has_owned(world, base, Velocity));

    ecs_inherit(world, e, base);
    test_assert( ecs_has(world, e, Velocity));
    test_assert( !ecs_has_owned(world, e, Velocity));
 
    ecs_fini(world);
}

void Has_has_owned_any() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_TYPE(world, Type, Position, Velocity);

    ecs_entity_t e = ecs_new(world, Position);
    test_assert(e != 0);
    test_assert( ecs_has(world, e, Position));
    test_assert( ecs_has_owned(world, e, Position));
    test_assert( ecs_has_any(world, e, Position));
    test_assert( ecs_has_any_owned(world, e, Position));

    ecs_entity_t base = ecs_new(world, Velocity);
    test_assert( ecs_has(world, base, Velocity));
    test_assert( ecs_has_owned(world, base, Velocity));
    test_assert( ecs_has_any(world, base, Velocity));
    test_assert( ecs_has_any_owned(world, base, Velocity));

    ecs_inherit(world, e, base);
    test_assert( ecs_has(world, e, Velocity));
    test_assert( ecs_has_any(world, e, Velocity));
    test_assert( !ecs_has_any_owned(world, e, Velocity));
    test_assert( ecs_has_any(world, e, Type));
    test_assert( ecs_has_any_owned(world, e, Type));
 
    ecs_fini(world);
}

void Has_has_entity() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t e = ecs_new(world, 0);
    test_assert(e != 0);

    ecs_entity_t base = ecs_new(world, 0);
    test_assert(base != 0);

    ecs_inherit(world, e, base);
    test_assert( ecs_has_entity(world, e, base));
 
    ecs_fini(world);
}

void Has_has_entity_0() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t base = ecs_new(world, 0);
    test_assert(base != 0);

    test_assert( !ecs_has_entity(world, 0, base));
 
    ecs_fini(world);
}

void Has_has_entity_0_component() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t e = ecs_new(world, 0);
    test_assert(e != 0);

    test_assert( ecs_has_entity(world, e, 0));
 
    ecs_fini(world);
}

void Has_has_entity_owned() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t e = ecs_new(world, 0);
    test_assert(e != 0);
    
    ecs_entity_t f = ecs_new(world, 0);
    test_assert(f != 0);

    ecs_entity_t g = ecs_new(world, 0);
    test_assert(g != 0);

    ecs_entity_t base = ecs_new(world, 0);
    test_assert(base != 0);

    ecs_add_entity(world, e, f);
    ecs_add_entity(world, base, g);
    ecs_inherit(world, e, base);

    test_assert( ecs_has_entity(world, e, f));
    test_assert( ecs_has_entity(world, e, g));
    test_assert( ecs_has_entity(world, e, base));
    test_assert( ecs_has_entity_owned(world, e, f));
    test_assert( !ecs_has_entity_owned(world, e, g));
    test_assert( ecs_has_entity_owned(world, e, base));
 
    ecs_fini(world);
}

void Has_has_entity_owned_0() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t e = ecs_new(world, 0);
    test_assert(e != 0);

    test_assert( ecs_has_entity_owned(world, 0, e) == false);

    ecs_fini(world);
}

void Has_has_entity_owned_0_component() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t e = ecs_new(world, 0);
    test_assert(e != 0);

    test_assert( ecs_has_entity_owned(world, e, 0) == true);

    ecs_fini(world);
}
