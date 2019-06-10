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
    t = _ecs_type_merge(world, t, t_1, 0);
    t = _ecs_type_merge(world, t, t_2, 0);
    
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
    ecs_type_t t = ecs_type_from_array(world, entities, 2);
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
    ecs_type_t t = ecs_type_from_array(world, entities, 2);
    test_assert(t == ecs_type(Type));
    test_assert( ecs_type_has_entity(world, t, ecs_entity(Position)));
    test_assert( ecs_type_has_entity(world, t, ecs_entity(Velocity)));

    ecs_fini(world);
}


void Type_vector_from_type() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_TYPE(world, Type, Position, Velocity);

    ecs_vector_t *vector = ecs_type_get_vector(world, ecs_type(Type));
    test_assert(vector != NULL);
    test_int(ecs_vector_count(vector), 2);

    ecs_entity_t *entities = ecs_vector_first(vector);
    test_int(entities[0], ecs_entity(Position));
    test_int(entities[1], ecs_entity(Velocity));

    ecs_fini(world);
}

void Type_type_has_prefab() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_TYPE(world, Type, Position, Velocity);

    ecs_entity_t entities[2] = {ecs_entity(Position), ecs_entity(Velocity) | ECS_ADD_PREFAB};
    ecs_type_t t = ecs_type_from_array(world, entities, 2);
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

    ecs_entity_t entities[2] = {ecs_entity(Position), ecs_entity(Velocity) | ECS_ADD_PARENT};
    ecs_type_t t = ecs_type_from_array(world, entities, 2);
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

    ecs_entity_t entities[2] = {ecs_entity(Position), ecs_entity(Velocity) | ECS_ADD_PREFAB | ECS_ADD_PARENT};
    ecs_type_t t = ecs_type_from_array(world, entities, 2);
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

    ecs_type_t t = ecs_type_merge(world, Type1, Type2, 0);

    ecs_vector_t *v = ecs_type_get_vector(world, t);
    test_assert(v != NULL);
    test_int( ecs_vector_count(v), 3);

    ecs_entity_t *entities = ecs_vector_first(v);
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

    ecs_type_t t = ecs_type_merge(world, Type1, Type2, 0);

    ecs_vector_t *v = ecs_type_get_vector(world, t);
    test_assert(v != NULL);
    test_int( ecs_vector_count(v), 3);

    ecs_entity_t *entities = ecs_vector_first(v);
    test_int(entities[0], ecs_entity(Position));
    test_int(entities[1], ecs_entity(Velocity));
    test_int(entities[2], ecs_entity(Mass));

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
    type2_entities[1] = ecs_entity(Velocity) | ECS_ADD_PREFAB;
    ecs_type_t ecs_type(Type2) = ecs_type_from_array(world, type2_entities, 2);

    ecs_type_t t = ecs_type_merge(world, Type1, Type2, 0);

    ecs_vector_t *v = ecs_type_get_vector(world, t);
    test_assert(v != NULL);
    test_int( ecs_vector_count(v), 3);

    ecs_entity_t *entities = ecs_vector_first(v);
    test_int(entities[0], ecs_entity(Position));
    test_int(entities[1], ecs_entity(Velocity) | ECS_ADD_PREFAB);
    test_int(entities[2], ecs_entity(Mass));

    ecs_fini(world);
}

void Type_type_merge_overlap_w_flags_from_both() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);

    ecs_entity_t type1_entities[2];
    type1_entities[0] = ecs_entity(Position);
    type1_entities[1] = ecs_entity(Velocity) | ECS_ADD_PARENT;
    ecs_type_t ecs_type(Type1) = ecs_type_from_array(world, type1_entities, 2);

    ecs_entity_t type2_entities[2];
    type2_entities[0] = ecs_entity(Mass);
    type2_entities[1] = ecs_entity(Velocity) | ECS_ADD_PREFAB;
    ecs_type_t ecs_type(Type2) = ecs_type_from_array(world, type2_entities, 2);

    ecs_type_t t = ecs_type_merge(world, Type1, Type2, 0);

    ecs_vector_t *v = ecs_type_get_vector(world, t);
    test_assert(v != NULL);
    test_int( ecs_vector_count(v), 3);

    ecs_entity_t *entities = ecs_vector_first(v);
    test_int(entities[0], ecs_entity(Position));
    test_int(entities[1], ecs_entity(Velocity) | ECS_ADD_PARENT | ECS_ADD_PREFAB);
    test_int(entities[2], ecs_entity(Mass));

    ecs_fini(world);
}
