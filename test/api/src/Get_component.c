#include <api.h>

void Get_component_get_empty() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t e = ecs_new(world, 0);
    test_assert(e != 0);

    test_assert(ecs_type_get_entity(world, ecs_get_type(world, e), 0) == 0);
    
    ecs_fini(world);
}

void Get_component_get_1_from_1() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new(world, Position);
    test_assert(e != 0);

    test_assert(ecs_type_get_entity(world, ecs_get_type(world, e), 0) == ecs_entity(Position));
    
    ecs_fini(world);
}

void Get_component_get_1_from_2() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TYPE(world, Type, Position, Velocity);

    ecs_entity_t e = ecs_new(world, Type);
    test_assert(e != 0);

    test_assert(ecs_type_get_entity(world, ecs_get_type(world, e), 0) == ecs_entity(Position));
    
    ecs_fini(world);
}

void Get_component_get_2_from_2() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TYPE(world, Type, Position, Velocity);

    ecs_entity_t e = ecs_new(world, Type);
    test_assert(e != 0);

    test_assert(ecs_type_get_entity(world, ecs_get_type(world, e), 1) == ecs_entity(Velocity));
    
    ecs_fini(world);
}

void Get_component_get_2_from_3() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_TYPE(world, Type, Position, Velocity, Mass);

    ecs_entity_t e = ecs_new(world, Type);
    test_assert(e != 0);

    test_assert(ecs_type_get_entity(world, ecs_get_type(world, e), 1) == ecs_entity(Velocity));
    
    ecs_fini(world);
}

static
void Test_main_stage(ecs_rows_t *rows) {
    ECS_COLUMN_COMPONENT(rows, Position, 1);

    for (int i = 0; i < rows->count; i ++) {
        ecs_entity_t e = rows->entities[i];
        test_assert(ecs_type_get_entity(rows->world, ecs_get_type(rows->world, e), 0) == ecs_entity(Position));
    }
}

void Get_component_get_1_from_2_in_progress_from_main_stage() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new(world, Position);
    test_assert(e != 0);

    ECS_SYSTEM(world, Test_main_stage, EcsOnUpdate, Position);

    ecs_progress(world, 1);
    
    ecs_fini(world);
}

static
void Add_in_progress(ecs_rows_t *rows) {
    ECS_COLUMN_COMPONENT(rows, Position, 1);

    ecs_entity_t ecs_entity(Velocity) = 0;
    ecs_type_t ecs_type(Velocity) = NULL;

    if (rows->column_count >= 2) {
        ecs_entity(Velocity) = ecs_column_entity(rows, 2);
        ecs_type(Velocity) = ecs_column_type(rows, 2);
    }

    for (int i = 0; i < rows->count; i ++) {
        ecs_entity_t e = rows->entities[i];
        
        ecs_add(rows->world, e, Velocity);
        test_assert( ecs_has(rows->world, e, Velocity));
        test_assert(ecs_type_get_entity(rows->world, ecs_get_type(rows->world, e), 1) == ecs_entity(Velocity));
    }
}

void Get_component_get_1_from_2_add_in_progress() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new(world, Position);
    test_assert(e != 0);

    ECS_SYSTEM(world, Add_in_progress, EcsOnUpdate, Position);

    ecs_progress(world, 1);
    
    ecs_fini(world);
}

static
void Add_in_progress_test_main(ecs_rows_t *rows) {
    ECS_COLUMN_COMPONENT(rows, Position, 1);
    ECS_COLUMN_COMPONENT(rows, Velocity, 2);

    for (int i = 0; i < rows->count; i ++) {
        ecs_entity_t e = rows->entities[i];
        test_assert(ecs_type_get_entity(rows->world, ecs_get_type(rows->world, e), 0) == ecs_entity(Position));
        
        ecs_add(rows->world, e, Velocity);
        test_assert( ecs_has(rows->world, e, Velocity));

        test_assert(ecs_type_get_entity(rows->world, ecs_get_type(rows->world, e), 0) == ecs_entity(Position));
        test_assert(ecs_type_get_entity(rows->world, ecs_get_type(rows->world, e), 1) == ecs_entity(Velocity));
    }
}

void Get_component_get_both_from_2_add_in_progress() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t e = ecs_new(world, Position);
    test_assert(e != 0);

    ECS_SYSTEM(world, Add_in_progress_test_main, EcsOnUpdate, Position, .Velocity);

    ecs_progress(world, 1);
    
    ecs_fini(world);
}

static
void Add_remove_in_progress_test_main(ecs_rows_t *rows) {
    ECS_COLUMN_COMPONENT(rows, Position, 1);
    ECS_COLUMN_COMPONENT(rows, Velocity, 2);

    for (int i = 0; i < rows->count; i ++) {
        ecs_entity_t e = rows->entities[i];
        test_assert(ecs_type_get_entity(rows->world, ecs_get_type(rows->world, e), 0) == ecs_entity(Position));
        
        ecs_add(rows->world, e, Velocity);
        test_assert( ecs_has(rows->world, e, Position));
        test_assert( ecs_has(rows->world, e, Velocity));

        test_assert(ecs_type_get_entity(rows->world, ecs_get_type(rows->world, e), 0) == ecs_entity(Position));
        test_assert(ecs_type_get_entity(rows->world, ecs_get_type(rows->world, e), 1) == ecs_entity(Velocity));

        ecs_remove(rows->world, e, Position);
        test_assert( !ecs_has(rows->world, e, Position));
        test_assert( ecs_has(rows->world, e, Velocity));

        test_assert(ecs_type_get_entity(rows->world, ecs_get_type(rows->world, e), 0) == ecs_entity(Velocity));
        test_assert(ecs_type_get_entity(rows->world, ecs_get_type(rows->world, e), 1) == 0);
    }
}

void Get_component_get_both_from_2_add_remove_in_progress() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t e = ecs_new(world, Position);
    test_assert(e != 0);

    ECS_SYSTEM(world, Add_remove_in_progress_test_main, EcsOnUpdate, Position, .Velocity);

    ecs_progress(world, 1);
    
    ecs_fini(world);
}
