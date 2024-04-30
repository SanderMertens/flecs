#include <core.h>

void Get_component_setup(void) {
    ecs_log_set_level(-3);
}

void Get_component_get_empty(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e = ecs_new(world);
    test_assert(e != 0);

    test_assert(ecs_get_type(world, e) == NULL);
    
    ecs_fini(world);
}

void Get_component_get_1_from_1(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new_w(world, Position);
    test_assert(e != 0);

    test_assert(ecs_get_type(world, e)->array[0] == ecs_id(Position));
    
    ecs_fini(world);
}

void Get_component_get_1_from_2(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_ENTITY(world, e, Position, Velocity);
    test_assert(e != 0);

    test_assert(ecs_get_type(world, e)->array[0] == ecs_id(Position));
    
    ecs_fini(world);
}

void Get_component_get_2_from_2(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_ENTITY(world, e, Position, Velocity);
    test_assert(e != 0);

    test_assert(ecs_get_type(world, e)->array[1] == ecs_id(Velocity));
    
    ecs_fini(world);
}

void Get_component_get_2_from_3(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);

    ECS_ENTITY(world, e, Position, Velocity, Mass);
    test_assert(e != 0);

    test_assert(ecs_get_type(world, e)->array[1] == ecs_id(Velocity));
    
    ecs_fini(world);
}

static
void Test_main_stage(ecs_iter_t *it) {
    ecs_id_t ecs_id(Position) = ecs_field_id(it, 0);

    for (int i = 0; i < it->count; i ++) {
        ecs_entity_t e = it->entities[i];
        test_assert(ecs_get_type(it->world, e)->array[0] == ecs_id(Position));
    }
}

void Get_component_get_1_from_2_in_progress_from_main_stage(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new_w(world, Position);
    test_assert(e != 0);

    ECS_SYSTEM(world, Test_main_stage, EcsOnUpdate, Position);

    ecs_progress(world, 1);
    
    ecs_fini(world);
}

static
void Add_in_progress(ecs_iter_t *it) {
    ecs_id_t ecs_id(Velocity) = 0;

    if (it->field_count >= 2) {
        ecs_id(Velocity) = ecs_field_id(it, 1);
    }

    for (int i = 0; i < it->count; i ++) {
        ecs_entity_t e = it->entities[i];
        ecs_add(it->world, e, Velocity);
        test_assert(!ecs_has(it->world, e, Velocity));
    }
}

void Get_component_get_1_from_2_add_in_progress(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t e = ecs_new_w(world, Position);
    test_assert(e != 0);

    ECS_SYSTEM(world, Add_in_progress, EcsOnUpdate, Position, Velocity());

    ecs_progress(world, 1);
    test_assert( ecs_has(world, e, Velocity));
    test_assert(ecs_get_type(world, e)->array[1] == ecs_id(Velocity));

    ecs_fini(world);
}

static
void Add_in_progress_test_main(ecs_iter_t *it) {
    ecs_id_t ecs_id(Position) = ecs_field_id(it, 0);
    ecs_id_t ecs_id(Velocity) = ecs_field_id(it, 1);

    for (int i = 0; i < it->count; i ++) {
        ecs_entity_t e = it->entities[i];
        test_assert(ecs_get_type(it->world, e)->array[0] == ecs_id(Position));
        ecs_add(it->world, e, Velocity);
    }
}

void Get_component_get_both_from_2_add_in_progress(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t e = ecs_new_w(world, Position);
    test_assert(e != 0);

    ECS_SYSTEM(world, Add_in_progress_test_main, EcsOnUpdate, Position, Velocity());

    ecs_progress(world, 1);
    
    test_assert( ecs_has(world, e, Velocity));
    test_assert(ecs_get_type(world, e)->array[0] == ecs_id(Position));
    test_assert(ecs_get_type(world, e)->array[1] == ecs_id(Velocity));

    ecs_fini(world);
}

static
void Add_remove_in_progress_test_main(ecs_iter_t *it) {
    ecs_id_t ecs_id(Position) = ecs_field_id(it, 0);
    ecs_id_t ecs_id(Velocity) = ecs_field_id(it, 1);

    for (int i = 0; i < it->count; i ++) {
        ecs_entity_t e = it->entities[i];
        test_assert(ecs_get_type(it->world, e)->array[0] == ecs_id(Position));        
        ecs_add(it->world, e, Velocity);
        ecs_remove(it->world, e, Position);
    }
}

void Get_component_get_both_from_2_add_remove_in_progress(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t e = ecs_new_w(world, Position);
    test_assert(e != 0);

    ECS_SYSTEM(world, Add_remove_in_progress_test_main, EcsOnUpdate, Position, Velocity());

    ecs_progress(world, 1);
    
    test_assert( !ecs_has(world, e, Position));
    test_assert( ecs_has(world, e, Velocity));

    test_assert(ecs_get_type(world, e)->array[0] == ecs_id(Velocity));
    test_assert(ecs_get_type(world, e)->count == 1);

    ecs_fini(world);
}

void Get_component_get_childof_component(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    const EcsComponent *ptr = ecs_get(world, ecs_id(Position), EcsComponent);
    test_assert(ptr != NULL);

    test_expect_abort();
    ecs_get(world, ecs_pair(EcsChildOf, ecs_id(Position)), EcsComponent);
}

void Get_component_ensure_equal_get(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_ENTITY(world, e, Position);
    test_assert(e != 0);

    test_assert(ecs_ensure(world, e, Position) == ecs_get(world, e,
                Position));

    ecs_fini(world);
}

void Get_component_get_tag(void) {
    install_test_abort();
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    ecs_entity_t e = ecs_new_w(world, Tag);
    test_assert(e != 0);

    test_expect_abort();
    ecs_get_id(world, e, Tag);

    ecs_fini(world);
}

void Get_component_get_pair_tag(void) {
    install_test_abort();
    ecs_world_t *world = ecs_mini();

    ecs_entity_t parent = ecs_new(world);
    ecs_entity_t e = ecs_new_w_pair(world, EcsChildOf, parent);
    test_assert(e != 0);

    test_expect_abort();
    ecs_get_id(world, e, ecs_childof(parent));

    ecs_fini(world);
}

void Get_component_get_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Tgt);

    ecs_entity_t e = ecs_insert(world, ecs_value_pair(Position, Tgt, {10, 20}));
    test_assert(e != 0);
    test_assert(ecs_has_pair(world, e, ecs_id(Position), Tgt));

    {
        const Position *p = ecs_get_pair(world, e, Position, Tgt);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 20);
    }

    {
        const Position *p = ecs_get_pair(world, e, Position, EcsWildcard);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 20);
    }

    ecs_fini(world);
}
