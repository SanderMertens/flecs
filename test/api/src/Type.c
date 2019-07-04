#include <api.h>

void Type_type_of_1_tostr() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_TYPE(world, Type, Position);

    char *str = ecs_type_str(world, Type);
    
    test_str(str, "Position");

    free(str);

    ecs_fini(world);
}

void Type_type_of_2_tostr() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_TYPE(world, Type, Position, Velocity);

    char *str = ecs_type_str(world, Type);
    
    test_str(str, "Position,Velocity");

    free(str);

    ecs_fini(world);
}

void Type_type_of_2_tostr_no_id() {
    ecs_world_t *world = ecs_init();

    ecs_type_t t_1 = ecs_type_from_entity(world, 100);
    ecs_type_t t_2 = ecs_type_from_entity(world, 200);
    ecs_type_t t = 0;
    t = ecs_type_merge(world, t, t_1, 0);
    t = ecs_type_merge(world, t, t_2, 0);
    
    char *str = _ecs_type_str(world, t);
    
    test_str(str, "100,200");

    free(str);

    ecs_fini(world);
}

void Type_type_redefine() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t type = ecs_new_type(world, "Type", "Position");

    test_assert(type == ecs_new_type(world, "Type", "Position"));

    ecs_fini(world);
}

void Type_type_from_array() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_TYPE(world, Type, Position, Velocity);

    ecs_entity_t entities[2] = {ecs_entity(Position), ecs_entity(Velocity)};
    ecs_type_t t = ecs_type_find(world, entities, 2);
    test_assert(t == ecs_type(Type));
    test_assert( ecs_type_has_entity(world, t, ecs_entity(Position)));
    test_assert( ecs_type_has_entity(world, t, ecs_entity(Velocity)));

    ecs_fini(world);
}

void Type_type_from_array_unsorted() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_TYPE(world, Type, Position, Velocity);

    ecs_entity_t entities[2] = {ecs_entity(Velocity), ecs_entity(Position)};
    ecs_type_t t = ecs_type_find(world, entities, 2);
    test_assert(t == ecs_type(Type));
    test_assert( ecs_type_has_entity(world, t, ecs_entity(Position)));
    test_assert( ecs_type_has_entity(world, t, ecs_entity(Velocity)));

    ecs_fini(world);
}

void Type_type_has() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_TYPE(world, Type, Position, Velocity);

    test_assert( ecs_type_has_entity(world, ecs_type(Type), ecs_entity(Position)));
    test_assert( ecs_type_has_entity(world, ecs_type(Type), ecs_entity(Velocity)));

    ecs_fini(world);
}

void Type_type_has_not() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_TYPE(world, Type, Position);

    test_assert( ecs_type_has_entity(world, ecs_type(Type), ecs_entity(Position)));
    test_assert( !ecs_type_has_entity(world, ecs_type(Type), ecs_entity(Velocity)));

    ecs_fini(world);
}

void Type_zero_type_has_not() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    test_assert( !ecs_type_has_entity(world, 0, ecs_entity(Position)));

    ecs_fini(world);
}

void Type_type_has_prefab() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_TYPE(world, Type, Position, Velocity);

    ecs_entity_t entities[2] = {ecs_entity(Position), ecs_entity(Velocity) | EcsInstanceOf};
    ecs_type_t t = ecs_type_find(world, entities, 2);
    test_assert(t != ecs_type(Type));
    test_assert( ecs_type_has_entity(world, t, ecs_entity(Position)));
    test_assert( ecs_type_has_entity(world, t, ecs_entity(Velocity)));

    ecs_fini(world);
}

void Type_type_has_container() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_TYPE(world, Type, Position, Velocity);

    ecs_entity_t entities[2] = {ecs_entity(Position), ecs_entity(Velocity) | EcsChildOf};
    ecs_type_t t = ecs_type_find(world, entities, 2);
    test_assert(t != ecs_type(Type));
    test_assert( ecs_type_has_entity(world, t, ecs_entity(Position)));
    test_assert( ecs_type_has_entity(world, t, ecs_entity(Velocity)));

    ecs_fini(world);
}

void Type_type_has_prefab_container() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_TYPE(world, Type, Position, Velocity);

    ecs_entity_t entities[2] = {ecs_entity(Position), ecs_entity(Velocity) | EcsInstanceOf | EcsChildOf};
    ecs_type_t t = ecs_type_find(world, entities, 2);
    test_assert(t != ecs_type(Type));
    test_assert( ecs_type_has_entity(world, t, ecs_entity(Position)));
    test_assert( ecs_type_has_entity(world, t, ecs_entity(Velocity)));

    ecs_fini(world);
}

void Type_type_merge() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);

    ECS_TYPE(world, Type1, Position, Velocity);
    ECS_TYPE(world, Type2, Mass);

    ecs_type_t t = ecs_type_merge(world, ecs_type(Type1), ecs_type(Type2), 0);
    test_assert(t != NULL);
    test_int( ecs_vector_count(t), 3);

    ecs_entity_t *entities = ecs_vector_first(t);
    test_int(entities[0], ecs_entity(Position));
    test_int(entities[1], ecs_entity(Velocity));
    test_int(entities[2], ecs_entity(Mass));

    ecs_fini(world);
}

void Type_type_merge_overlap() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);

    ECS_TYPE(world, Type1, Position, Velocity);
    ECS_TYPE(world, Type2, Velocity, Mass);

    ecs_type_t t = ecs_type_merge(world, ecs_type(Type1), ecs_type(Type2), 0);
    test_assert(t != NULL);
    test_int( ecs_vector_count(t), 3);

    ecs_entity_t *entities = ecs_vector_first(t);
    test_int(entities[0], ecs_entity(Position));
    test_int(entities[1], ecs_entity(Velocity));
    test_int(entities[2], ecs_entity(Mass));

    ecs_fini(world);
}

void Type_type_merge_overlap_one() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);

    ECS_TYPE(world, Type1, Position, Velocity);
    ECS_TYPE(world, Type2, Position);

    ecs_type_t t = ecs_type_merge(world, ecs_type(Type1), ecs_type(Type2), 0);
    test_assert(t != NULL);
    test_int( ecs_vector_count(t), 2);

    ecs_entity_t *entities = ecs_vector_first(t);
    test_int(entities[0], ecs_entity(Position));
    test_int(entities[1], ecs_entity(Velocity));

    ecs_fini(world);
}

void Type_type_merge_overlap_w_flag() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);

    ECS_TYPE(world, Type1, Position, Velocity);

    ecs_entity_t type2_entities[2];
    type2_entities[0] = ecs_entity(Mass);
    type2_entities[1] = ecs_entity(Velocity) | EcsInstanceOf;
    ecs_type_t type2 = ecs_type_find(world, type2_entities, 2);

    ecs_type_t t = ecs_type_merge(world, ecs_type(Type1), type2, 0);
    test_assert(t != NULL);
    test_int( ecs_vector_count(t), 3);

    ecs_entity_t *entities = ecs_vector_first(t);
    test_int(entities[0], ecs_entity(Position));
    test_int(entities[1], ecs_entity(Mass));
    test_int(entities[2], ecs_entity(Velocity) | EcsInstanceOf);

    ecs_fini(world);
}

void Type_type_merge_overlap_w_flags_from_both() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);

    ecs_entity_t type1_entities[2];
    type1_entities[0] = ecs_entity(Position);
    type1_entities[1] = ecs_entity(Velocity) | EcsChildOf;
    ecs_type_t ecs_type(Type1) = ecs_type_find(world, type1_entities, 2);

    ecs_entity_t type2_entities[2];
    type2_entities[0] = ecs_entity(Mass);
    type2_entities[1] = ecs_entity(Velocity) | EcsInstanceOf;
    ecs_type_t type2 = ecs_type_find(world, type2_entities, 2);

    ecs_type_t t = ecs_type_merge(world, ecs_type(Type1), type2, 0);
    test_assert(t != NULL);
    test_int( ecs_vector_count(t), 3);

    ecs_entity_t *entities = ecs_vector_first(t);
    test_int(entities[0], ecs_entity(Position));
    test_int(entities[1], ecs_entity(Mass));
    test_int(entities[2], ecs_entity(Velocity) | EcsChildOf | EcsInstanceOf);

    ecs_fini(world);
}


void Type_type_add() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TYPE(world, Type, Position);

    ecs_type_t new_type = ecs_type_add(world, ecs_type(Type), ecs_entity(Velocity));
    test_assert(new_type != NULL);
    test_assert(new_type != ecs_type(Type));

    ecs_entity_t *type_array = ecs_vector_first(new_type);
    test_assert(type_array != NULL);
    test_int(ecs_vector_count(new_type), 2);
    test_int(type_array[0], ecs_entity(Position));
    test_int(type_array[1], ecs_entity(Velocity));

    ecs_fini(world);
}

void Type_type_add_empty() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_type_t new_type = ecs_type_add(world, NULL, ecs_entity(Velocity));
    test_assert(new_type != NULL);

    ecs_entity_t *type_array = ecs_vector_first(new_type);
    test_assert(type_array != NULL);
    test_int(ecs_vector_count(new_type), 1);
    test_int(type_array[0], ecs_entity(Velocity));

    ecs_fini(world);
}

void Type_type_add_entity_again() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TYPE(world, Type, Position, Velocity);

    ecs_type_t new_type = ecs_type_add(world, ecs_type(Type), ecs_entity(Velocity));
    test_assert(new_type != NULL);
    test_assert(new_type == ecs_type(Type));

    ecs_entity_t *type_array = ecs_vector_first(new_type);
    test_assert(type_array != NULL);
    test_int(ecs_vector_count(new_type), 2);
    test_int(type_array[0], ecs_entity(Position));
    test_int(type_array[1], ecs_entity(Velocity));

    ecs_fini(world);
}

void Type_type_add_out_of_order() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TYPE(world, Type, Velocity);

    ecs_type_t new_type = ecs_type_add(world, ecs_type(Type), ecs_entity(Position));
    test_assert(new_type != NULL);
    test_assert(new_type != ecs_type(Type));

    ecs_entity_t *type_array = ecs_vector_first(new_type);
    test_assert(type_array != NULL);
    test_int(ecs_vector_count(new_type), 2);
    test_int(type_array[0], ecs_entity(Position));
    test_int(type_array[1], ecs_entity(Velocity));

    ecs_fini(world);
}

void Type_type_add_existing() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TYPE(world, Type1, Position, Velocity);
    ECS_TYPE(world, Type2, Position);

    ecs_type_t new_type = ecs_type_add(world, ecs_type(Type2), ecs_entity(Velocity));
    test_assert(new_type != NULL);
    test_assert(new_type != ecs_type(Type2));
    test_assert(new_type == ecs_type(Type1));

    ecs_entity_t *type_array = ecs_vector_first(new_type);
    test_assert(type_array != NULL);
    test_int(ecs_vector_count(new_type), 2);
    test_int(type_array[0], ecs_entity(Position));
    test_int(type_array[1], ecs_entity(Velocity));

    ecs_fini(world);
}

void Type_type_add_empty_existing() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Velocity);
    ECS_TYPE(world, Type1, Velocity);

    ecs_type_t new_type = ecs_type_add(world,  NULL, ecs_entity(Velocity));
    test_assert(new_type != NULL);
    test_assert(new_type == ecs_type(Type1));

    ecs_entity_t *type_array = ecs_vector_first(new_type);
    test_assert(type_array != NULL);
    test_int(ecs_vector_count(new_type), 1);
    test_int(type_array[0], ecs_entity(Velocity));

    ecs_fini(world);
}

void Type_type_add_out_of_order_existing() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TYPE(world, Type1, Velocity);
    ECS_TYPE(world, Type2, Position, Velocity);

    ecs_type_t new_type = ecs_type_add(world, ecs_type(Type1), ecs_entity(Position));
    test_assert(new_type != NULL);
    test_assert(new_type != ecs_type(Type1));
    test_assert(new_type == ecs_type(Type2));

    ecs_entity_t *type_array = ecs_vector_first(new_type);
    test_assert(type_array != NULL);
    test_int(ecs_vector_count(new_type), 2);
    test_int(type_array[0], ecs_entity(Position));
    test_int(type_array[1], ecs_entity(Velocity));

    ecs_fini(world);
}

void Type_type_of_2_add() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Mass);
    ECS_TYPE(world, Type, Position, Mass);

    ecs_type_t new_type = ecs_type_add(world, ecs_type(Type), ecs_entity(Velocity));
    test_assert(new_type != NULL);
    test_assert(new_type != ecs_type(Type));

    ecs_entity_t *type_array = ecs_vector_first(new_type);
    test_assert(type_array != NULL);
    test_int(ecs_vector_count(new_type), 3);
    test_int(type_array[0], ecs_entity(Velocity));
    test_int(type_array[1], ecs_entity(Position));
    test_int(type_array[2], ecs_entity(Mass));

    ecs_fini(world);
}

void Type_type_of_3_add_entity_again() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Mass);
    ECS_TYPE(world, Type, Velocity, Position, Mass);

    ecs_type_t new_type = ecs_type_add(world, ecs_type(Type), ecs_entity(Velocity));
    test_assert(new_type != NULL);
    test_assert(new_type == ecs_type(Type));

    ecs_entity_t *type_array = ecs_vector_first(new_type);
    test_assert(type_array != NULL);
    test_int(ecs_vector_count(new_type), 3);
    test_int(type_array[0], ecs_entity(Velocity));
    test_int(type_array[1], ecs_entity(Position));
    test_int(type_array[2], ecs_entity(Mass));

    ecs_fini(world);
}

void Type_invalid_container_type_expression() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Position);

    test_expect_abort();

    ECS_TYPE(world, Type, CONTAINER.Position, Velocity);

    ecs_fini(world);
}

void Type_invalid_entity_type_expression() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Position);

    ECS_ENTITY(world, Entity, Position);

    test_expect_abort();

    ECS_TYPE(world, Type, Entity.Position, Velocity);

    ecs_fini(world);
}

void Type_invalid_singleton_type_expression() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Position);

    test_expect_abort();

    ECS_TYPE(world, Type, $.Position, Velocity);

    ecs_fini(world);
}

void Type_invalid_system_type_expression() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Position);

    test_expect_abort();

    ECS_TYPE(world, Type, SYSTEM.Position, Velocity);

    ecs_fini(world);
}
