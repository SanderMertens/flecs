#include <api.h>

void Type_setup() {
    ecs_tracing_enable(-3);
}

void Type_type_of_1_tostr() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_TYPE(world, Type, Position);

    char *str = ecs_type_str(world, ecs_type(Type));
    
    test_str(str, "Position");

    free(str);

    ecs_fini(world);
}

void Type_type_of_2_tostr() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_TYPE(world, Type, Position, Velocity);

    char *str = ecs_type_str(world, ecs_type(Type));
    
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
    
    char *str = ecs_type_str(world, t);
    
    test_str(str, "100,200");

    free(str);

    ecs_fini(world);
}

void Type_type_redefine() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t type = ecs_new_type(world, 0, "Type", "Position");

    test_assert(type == ecs_new_type(world, 0, "Type", "Position"));

    ecs_fini(world);
}

void Type_type_from_array() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_TYPE(world, Type, Position, Velocity);

    ecs_entity_t entities[2] = {ecs_typeid(Position), ecs_typeid(Velocity)};
    ecs_type_t t = ecs_type_find(world, entities, 2);
    test_assert(t == ecs_type(Type));
    test_assert( ecs_type_has_entity(world, t, ecs_typeid(Position)));
    test_assert( ecs_type_has_entity(world, t, ecs_typeid(Velocity)));

    ecs_fini(world);
}

void Type_type_from_array_unsorted() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_TYPE(world, Type, Position, Velocity);

    ecs_entity_t entities[2] = {ecs_typeid(Velocity), ecs_typeid(Position)};
    ecs_type_t t = ecs_type_find(world, entities, 2);
    test_assert(t == ecs_type(Type));
    test_assert( ecs_type_has_entity(world, t, ecs_typeid(Position)));
    test_assert( ecs_type_has_entity(world, t, ecs_typeid(Velocity)));

    ecs_fini(world);
}

void Type_type_has() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_TYPE(world, Type, Position, Velocity);

    test_assert( ecs_type_has_entity(world, ecs_type(Type), ecs_typeid(Position)));
    test_assert( ecs_type_has_entity(world, ecs_type(Type), ecs_typeid(Velocity)));

    ecs_fini(world);
}

void Type_type_has_not() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_TYPE(world, Type, Position);

    test_assert( ecs_type_has_entity(world, ecs_type(Type), ecs_typeid(Position)));
    test_assert( !ecs_type_has_entity(world, ecs_type(Type), ecs_typeid(Velocity)));

    ecs_fini(world);
}

void Type_zero_type_has_not() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    test_assert( !ecs_type_has_entity(world, 0, ecs_typeid(Position)));

    ecs_fini(world);
}

void Type_type_has_prefab() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_TYPE(world, Type, Position, Velocity);

    ecs_entity_t entities[2] = {ecs_typeid(Position), ecs_typeid(Velocity) | ECS_INSTANCEOF};
    ecs_type_t t = ecs_type_find(world, entities, 2);
    test_assert(t != ecs_type(Type));
    test_assert( ecs_type_has_entity(world, t, ecs_typeid(Position)));
    test_assert( ecs_type_has_entity(world, t, ECS_INSTANCEOF | ecs_typeid(Velocity)));

    ecs_fini(world);
}

void Type_type_has_container() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_TYPE(world, Type, Position, Velocity);

    ecs_entity_t entities[2] = {ecs_typeid(Position), ecs_typeid(Velocity) | ECS_CHILDOF};
    ecs_type_t t = ecs_type_find(world, entities, 2);
    test_assert(t != ecs_type(Type));
    test_assert( ecs_type_has_entity(world, t, ecs_typeid(Position)));
    test_assert( ecs_type_has_entity(world, t, ECS_CHILDOF | ecs_typeid(Velocity)));

    ecs_fini(world);
}

void Type_type_has_prefab_container() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_TYPE(world, Type, Position, Velocity);

    ecs_entity_t entities[2] = {ecs_typeid(Position), ecs_typeid(Velocity) | ECS_INSTANCEOF | ECS_CHILDOF};
    ecs_type_t t = ecs_type_find(world, entities, 2);
    test_assert(t != ecs_type(Type));
    test_assert( ecs_type_has_entity(world, t, ecs_typeid(Position)));
    test_assert( ecs_type_has_entity(world, t, ECS_INSTANCEOF | ECS_CHILDOF | ecs_typeid(Velocity)));

    ecs_fini(world);
}

void Type_type_has_trait() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Trait);

    ECS_TYPE(world, Type, TRAIT | Trait > Position);

    ecs_entity_t entities[2] = {ecs_trait(ecs_typeid(Position), Trait)};
    ecs_type_t t = ecs_type_find(world, entities, 1);
    test_assert(t == ecs_type(Type));
    test_assert( ecs_type_has_entity(world, t, ECS_TRAIT | Trait));

    ecs_fini(world);
}

void Type_type_has_trait_exact() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Trait);

    ECS_TYPE(world, Type, TRAIT | Trait > Position);

    ecs_entity_t entities[2] = {ecs_trait(ecs_typeid(Position), Trait)};
    ecs_type_t t = ecs_type_find(world, entities, 1);
    test_assert(t == ecs_type(Type));
    test_assert( ecs_type_has_entity(world, t, ecs_trait(ecs_typeid(Position), Trait)));

    ecs_fini(world);
}

void Type_type_has_trait_wildcard() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Trait);

    ECS_TYPE(world, TypeMatch, TRAIT | Trait > Position, Position);
    ECS_TYPE(world, TypeNoMatch, TRAIT | Trait > Position);

    test_assert( ecs_type_has_entity(world, ecs_type(TypeMatch), 
        ecs_trait(EcsWildcard, Trait)));

    test_assert( !ecs_type_has_entity(world, ecs_type(TypeNoMatch), 
        ecs_trait(EcsWildcard, Trait)));

    ecs_fini(world);
}

void Type_type_has_trait_wildcard_multiple() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TAG(world, Trait);

    ECS_TYPE(world, TypeMatch, TRAIT | Trait > Position, Position, TRAIT | Trait > Velocity, Velocity);
    ECS_TYPE(world, TypeNoMatch, TRAIT | Trait > Position, Position, TRAIT | Trait > Velocity);

    test_assert( ecs_type_has_entity(world, ecs_type(TypeMatch),
        ecs_trait(EcsWildcard, Trait)));

    test_assert( !ecs_type_has_entity(world, ecs_type(TypeNoMatch), 
        ecs_trait(EcsWildcard, Trait)));

    ecs_fini(world);
}

void Type_type_has_trait_wildcard_no_trait() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TAG(world, Trait);

    ECS_TYPE(world, TypeNoMatch, Position);

    test_assert( !ecs_type_has_entity(world, ecs_type(TypeNoMatch), 
        ecs_trait(EcsWildcard, Trait)));

    ecs_fini(world);
}

void Type_type_owns_trait() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Trait);

    ECS_TYPE(world, Type, TRAIT | Trait > Position);

    ecs_entity_t entities[2] = {ecs_trait(ecs_typeid(Position), Trait)};
    ecs_type_t t = ecs_type_find(world, entities, 1);
    test_assert(t == ecs_type(Type));
    test_assert( ecs_type_owns_entity(world, t, ECS_TRAIT | Trait, true));

    ecs_fini(world);
}

void Type_type_owns_trait_exact() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Trait);

    ECS_TYPE(world, Type, TRAIT | Trait > Position);

    ecs_entity_t entities[2] = {ecs_trait(ecs_typeid(Position), Trait)};
    ecs_type_t t = ecs_type_find(world, entities, 1);
    test_assert(t == ecs_type(Type));
    test_assert( ecs_type_owns_entity(world, t, ecs_trait(ecs_typeid(Position), Trait), true));

    ecs_fini(world);
}

void Type_type_owns_trait_wildcard() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Trait);

    ECS_TYPE(world, TypeMatch, TRAIT | Trait > Position, Position);
    ECS_TYPE(world, TypeNoMatch, TRAIT | Trait > Position);

    test_assert( ecs_type_owns_entity(world, ecs_type(TypeMatch), 
        ecs_trait(EcsWildcard, Trait), true));

    test_assert( !ecs_type_owns_entity(world, ecs_type(TypeNoMatch), 
        ecs_trait(EcsWildcard, Trait), true));

    ecs_fini(world);
}

void Type_type_owns_trait_wildcard_multiple() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TAG(world, Trait);

    ECS_TYPE(world, TypeMatch, TRAIT | Trait > Position, Position, TRAIT | Trait > Velocity, Velocity);
    ECS_TYPE(world, TypeNoMatch, TRAIT | Trait > Position, Position, TRAIT | Trait > Velocity);

    test_assert( ecs_type_owns_entity(world, ecs_type(TypeMatch), 
        ecs_trait(EcsWildcard, Trait), true));

    test_assert( !ecs_type_owns_entity(world, ecs_type(TypeNoMatch), 
        ecs_trait(EcsWildcard, Trait), true));

    ecs_fini(world);
}

void Type_type_owns_trait_wildcard_no_trait() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TAG(world, Trait);

    ECS_TYPE(world, TypeNoMatch, Position);

    test_assert( !ecs_type_owns_entity(world, ecs_type(TypeNoMatch), 
        ecs_trait(EcsWildcard, Trait), true));

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

    ecs_entity_t *entities = ecs_vector_first(t, ecs_entity_t);
    test_int(entities[0], ecs_typeid(Position));
    test_int(entities[1], ecs_typeid(Velocity));
    test_int(entities[2], ecs_typeid(Mass));

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

    ecs_entity_t *entities = ecs_vector_first(t, ecs_entity_t);
    test_int(entities[0], ecs_typeid(Position));
    test_int(entities[1], ecs_typeid(Velocity));
    test_int(entities[2], ecs_typeid(Mass));

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

    ecs_entity_t *entities = ecs_vector_first(t, ecs_entity_t);
    test_int(entities[0], ecs_typeid(Position));
    test_int(entities[1], ecs_typeid(Velocity));

    ecs_fini(world);
}

void Type_type_merge_overlap_w_flag() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);

    ECS_TYPE(world, Type1, Position, Velocity);

    ecs_entity_t type2_entities[2];
    type2_entities[0] = ecs_typeid(Mass);
    type2_entities[1] = ecs_typeid(Velocity) | ECS_INSTANCEOF;
    ecs_type_t type2 = ecs_type_find(world, type2_entities, 2);

    ecs_type_t t = ecs_type_merge(world, ecs_type(Type1), type2, 0);
    test_assert(t != NULL);
    test_int( ecs_vector_count(t), 4);

    ecs_entity_t *entities = ecs_vector_first(t, ecs_entity_t);
    test_int(entities[0], ecs_typeid(Position));
    test_int(entities[1], ecs_typeid(Velocity));
    test_int(entities[2], ecs_typeid(Mass));
    test_int(entities[3], ecs_typeid(Velocity) | ECS_INSTANCEOF);

    ecs_fini(world);
}

void Type_type_merge_overlap_w_flags_from_both() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);

    ecs_entity_t type1_entities[2];
    type1_entities[0] = ecs_typeid(Position);
    type1_entities[1] = ecs_typeid(Velocity) | ECS_CHILDOF;
    ecs_type_t ecs_type(Type1) = ecs_type_find(world, type1_entities, 2);

    ecs_entity_t type2_entities[2];
    type2_entities[0] = ecs_typeid(Mass);
    type2_entities[1] = ecs_typeid(Velocity) | ECS_INSTANCEOF;
    ecs_type_t type2 = ecs_type_find(world, type2_entities, 2);

    ecs_type_t t = ecs_type_merge(world, ecs_type(Type1), type2, 0);
    test_assert(t != NULL);
    test_int( ecs_vector_count(t), 4);

    ecs_entity_t *entities = ecs_vector_first(t, ecs_entity_t);
    test_int(entities[0], ecs_typeid(Position));
    test_int(entities[1], ecs_typeid(Mass));
    test_int(entities[2], ecs_typeid(Velocity) | ECS_CHILDOF);
    test_int(entities[3], ecs_typeid(Velocity) | ECS_INSTANCEOF);

    ecs_fini(world);
}


void Type_type_add() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TYPE(world, Type, Position);

    ecs_type_t new_type = ecs_type_add(world, ecs_type(Type), ecs_typeid(Velocity));
    test_assert(new_type != NULL);
    test_assert(new_type != ecs_type(Type));

    ecs_entity_t *type_array = ecs_vector_first(new_type, ecs_entity_t);
    test_assert(type_array != NULL);
    test_int(ecs_vector_count(new_type), 2);
    test_int(type_array[0], ecs_typeid(Position));
    test_int(type_array[1], ecs_typeid(Velocity));

    ecs_fini(world);
}

void Type_type_add_empty() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_type_t new_type = ecs_type_add(world, NULL, ecs_typeid(Velocity));
    test_assert(new_type != NULL);

    ecs_entity_t *type_array = ecs_vector_first(new_type, ecs_entity_t);
    test_assert(type_array != NULL);
    test_int(ecs_vector_count(new_type), 1);
    test_int(type_array[0], ecs_typeid(Velocity));

    ecs_fini(world);
}

void Type_type_add_entity_again() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TYPE(world, Type, Position, Velocity);

    ecs_type_t new_type = ecs_type_add(world, ecs_type(Type), ecs_typeid(Velocity));
    test_assert(new_type != NULL);
    test_assert(new_type == ecs_type(Type));

    ecs_entity_t *type_array = ecs_vector_first(new_type, ecs_entity_t);
    test_assert(type_array != NULL);
    test_int(ecs_vector_count(new_type), 2);
    test_int(type_array[0], ecs_typeid(Position));
    test_int(type_array[1], ecs_typeid(Velocity));

    ecs_fini(world);
}

void Type_type_add_out_of_order() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TYPE(world, Type, Velocity);

    ecs_type_t new_type = ecs_type_add(world, ecs_type(Type), ecs_typeid(Position));
    test_assert(new_type != NULL);
    test_assert(new_type != ecs_type(Type));

    ecs_entity_t *type_array = ecs_vector_first(new_type, ecs_entity_t);
    test_assert(type_array != NULL);
    test_int(ecs_vector_count(new_type), 2);
    test_int(type_array[0], ecs_typeid(Position));
    test_int(type_array[1], ecs_typeid(Velocity));

    ecs_fini(world);
}

void Type_type_add_existing() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TYPE(world, Type1, Position, Velocity);
    ECS_TYPE(world, Type2, Position);

    ecs_type_t new_type = ecs_type_add(world, ecs_type(Type2), ecs_typeid(Velocity));
    test_assert(new_type != NULL);
    test_assert(new_type != ecs_type(Type2));
    test_assert(new_type == ecs_type(Type1));

    ecs_entity_t *type_array = ecs_vector_first(new_type, ecs_entity_t);
    test_assert(type_array != NULL);
    test_int(ecs_vector_count(new_type), 2);
    test_int(type_array[0], ecs_typeid(Position));
    test_int(type_array[1], ecs_typeid(Velocity));

    ecs_fini(world);
}

void Type_type_add_empty_existing() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Velocity);
    ECS_TYPE(world, Type1, Velocity);

    ecs_type_t new_type = ecs_type_add(world,  NULL, ecs_typeid(Velocity));
    test_assert(new_type != NULL);
    test_assert(new_type == ecs_type(Type1));

    ecs_entity_t *type_array = ecs_vector_first(new_type, ecs_entity_t);
    test_assert(type_array != NULL);
    test_int(ecs_vector_count(new_type), 1);
    test_int(type_array[0], ecs_typeid(Velocity));

    ecs_fini(world);
}

void Type_type_add_out_of_order_existing() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TYPE(world, Type1, Velocity);
    ECS_TYPE(world, Type2, Position, Velocity);

    ecs_type_t new_type = ecs_type_add(world, ecs_type(Type1), ecs_typeid(Position));
    test_assert(new_type != NULL);
    test_assert(new_type != ecs_type(Type1));
    test_assert(new_type == ecs_type(Type2));

    ecs_entity_t *type_array = ecs_vector_first(new_type, ecs_entity_t);
    test_assert(type_array != NULL);
    test_int(ecs_vector_count(new_type), 2);
    test_int(type_array[0], ecs_typeid(Position));
    test_int(type_array[1], ecs_typeid(Velocity));

    ecs_fini(world);
}

void Type_type_of_2_add() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Mass);
    ECS_TYPE(world, Type, Position, Mass);

    ecs_type_t new_type = ecs_type_add(world, ecs_type(Type), ecs_typeid(Velocity));
    test_assert(new_type != NULL);
    test_assert(new_type != ecs_type(Type));

    ecs_entity_t *type_array = ecs_vector_first(new_type, ecs_entity_t);
    test_assert(type_array != NULL);
    test_int(ecs_vector_count(new_type), 3);
    test_int(type_array[0], ecs_typeid(Velocity));
    test_int(type_array[1], ecs_typeid(Position));
    test_int(type_array[2], ecs_typeid(Mass));

    ecs_fini(world);
}

void Type_type_of_3_add_entity_again() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Mass);
    ECS_TYPE(world, Type, Velocity, Position, Mass);

    ecs_type_t new_type = ecs_type_add(world, ecs_type(Type), ecs_typeid(Velocity));
    test_assert(new_type != NULL);
    test_assert(new_type == ecs_type(Type));

    ecs_entity_t *type_array = ecs_vector_first(new_type, ecs_entity_t);
    test_assert(type_array != NULL);
    test_int(ecs_vector_count(new_type), 3);
    test_int(type_array[0], ecs_typeid(Velocity));
    test_int(type_array[1], ecs_typeid(Position));
    test_int(type_array[2], ecs_typeid(Mass));

    ecs_fini(world);
}

void Type_invalid_container_type_expression() {
    install_test_abort();

    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Position);

    test_expect_abort();

    ECS_TYPE(world, Type, PARENT:Position, Velocity);

    ecs_fini(world);
}

void Type_invalid_entity_type_expression() {
    install_test_abort();

    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Position);

    ECS_ENTITY(world, Entity, Position);

    test_expect_abort();

    ECS_TYPE(world, Type, Entity:Position, Velocity);

    ecs_fini(world);
}

void Type_invalid_singleton_type_expression() {
    install_test_abort();

    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Position);

    test_expect_abort();

    ECS_TYPE(world, Type, $:Position, Velocity);

    ecs_fini(world);
}

void Type_invalid_system_type_expression() {
    install_test_abort();

    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Position);

    test_expect_abort();

    ECS_TYPE(world, Type, SYSTEM:Position, Velocity);

    ecs_fini(world);
}

void Type_type_from_empty_entity() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t e = ecs_new(world, 0);
    test_assert(e != 0);

    ecs_type_t t = ecs_type_from_entity(world, e);
    test_assert(t != NULL);

    test_int(ecs_vector_count(t), 1);

    ecs_entity_t *array = ecs_vector_first(t, ecs_entity_t);
    test_int(array[0], e);

    ecs_fini(world);
}

void Type_get_type() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new(world, Position);

    ecs_type_t t = ecs_get_type(world, e);
    test_assert(t != NULL);
    test_int(ecs_vector_count(t), 1);
    
    ecs_entity_t *type_array = ecs_vector_first(t, ecs_entity_t);
    test_assert(type_array != NULL);
    test_int(type_array[0], ecs_typeid(Position));

    ecs_fini(world);
}

void Type_get_type_from_empty() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t e = ecs_new(world, 0);

    ecs_type_t t = ecs_get_type(world, e);
    test_assert(t == NULL);

    ecs_fini(world);
}

void Type_get_type_from_0() {
    ecs_world_t *world = ecs_init();

    ecs_type_t t = ecs_get_type(world, 0);
    test_assert(t == NULL);

    ecs_fini(world);
}

void Type_entity_from_type() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_TYPE(world, MyType, Position);

    ecs_entity_t e = ecs_type_to_entity(world, ecs_type(MyType));
    test_assert(e != 0);
    test_assert(e == ecs_typeid(Position));

    ecs_fini(world);
}

void Type_entity_from_empty_type() {
    ecs_world_t *world = ecs_init();

    ECS_TYPE(world, MyType, 0);

    ecs_entity_t e = ecs_type_to_entity(world, ecs_type(MyType));
    test_assert(e == 0);

    ecs_fini(world);
}

void Type_entity_from_type_w_2_elements() {
    install_test_abort();

    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TYPE(world, MyType, Position, Velocity);
    
    test_expect_abort();

    ecs_type_to_entity(world, ecs_type(MyType));

    ecs_fini(world);
}

void Type_type_from_entity() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_ENTITY(world, Entity, Position);

    ecs_type_t t = ecs_type_from_entity(world, Entity);
    test_assert(t != NULL);
    test_int(ecs_vector_count(t), 1);

    ecs_entity_t *type_array = ecs_vector_first(t, ecs_entity_t);
    test_int(type_array[0], Entity);

    ecs_fini(world);
}

void Type_type_from_empty() {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Entity, 0);

    ecs_type_t t = ecs_type_from_entity(world, Entity);
    test_assert(t != NULL);
    test_int(ecs_vector_count(t), 1);

    ecs_entity_t *type_array = ecs_vector_first(t, ecs_entity_t);
    test_int(type_array[0], Entity);

    ecs_fini(world);
}

void Type_type_from_0() {
    ecs_world_t *world = ecs_init();

    ecs_type_t t = ecs_type_from_entity(world, 0);
    test_assert(t == NULL);

    ecs_fini(world);
}

void Type_type_remove() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TYPE(world, Type, Position, Velocity);

    ecs_type_t new_type = ecs_type_remove(world, ecs_type(Type), ecs_typeid(Velocity));
    test_assert(new_type != NULL);
    test_assert(new_type != ecs_type(Type));

    ecs_entity_t *type_array = ecs_vector_first(new_type, ecs_entity_t);
    test_assert(type_array != NULL);
    test_int(ecs_vector_count(new_type), 1);
    test_int(type_array[0], ecs_typeid(Position));

    ecs_fini(world);
}

void Type_type_remove_empty() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_TYPE(world, Type, 0);

    ecs_type_t new_type = ecs_type_remove(world, ecs_type(Type), ecs_typeid(Position));
    test_assert(new_type == NULL);

    ecs_fini(world);
}

void Type_type_remove_non_existing() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TYPE(world, Type, Position);

    ecs_type_t new_type = ecs_type_remove(world, ecs_type(Type), ecs_typeid(Velocity));
    test_assert(new_type != NULL);
    test_assert(new_type == ecs_type(Type));

    ecs_fini(world);
}

void Type_type_to_expr_1_comp() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TYPE(world, Type, Position);

    char *expr = ecs_type_str(world, ecs_type(Type));
    test_str(expr, "Position");
    ecs_os_free(expr);

    ecs_fini(world);
}

void Type_type_to_expr_2_comp() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TYPE(world, Type, Position, Velocity);

    char *expr = ecs_type_str(world, ecs_type(Type));
    test_str(expr, "Position,Velocity");
    ecs_os_free(expr);

    ecs_fini(world);
}

void Type_type_to_expr_instanceof() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TYPE(world, Type, INSTANCEOF | Position);

    char *expr = ecs_type_str(world, ecs_type(Type));
    test_str(expr, "INSTANCEOF|Position");
    ecs_os_free(expr);

    ecs_fini(world);
}

void Type_type_to_expr_childof() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TYPE(world, Type, CHILDOF | Position);

    char *expr = ecs_type_str(world, ecs_type(Type));
    test_str(expr, "CHILDOF|Position");
    ecs_os_free(expr);

    ecs_fini(world);
}

void Type_type_to_expr_trait() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TYPE(world, Type, TRAIT | Position);

    char *expr = ecs_type_str(world, ecs_type(Type));
    test_str(expr, "TRAIT|Position");
    ecs_os_free(expr);

    ecs_fini(world);
}

void Type_type_to_expr_trait_w_comp() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TYPE(world, Type, TRAIT | Position > Velocity);

    char *expr = ecs_type_str(world, ecs_type(Type));
    test_str(expr, "TRAIT|Position>Velocity");
    ecs_os_free(expr);

    ecs_fini(world);
}

void Type_type_to_expr_scope() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_ENTITY(world, scope, 0);
    ecs_set_scope(world, scope);
    ECS_COMPONENT(world, Velocity);
    ECS_TYPE(world, Type, Position, Velocity);
    ecs_set_scope(world, 0);

    char *expr = ecs_type_str(world, ecs_type(Type));
    test_str(expr, "Position,scope.Velocity");
    ecs_os_free(expr);

    ecs_fini(world);
}

void Type_type_from_expr() {
    ecs_world_t *world = ecs_init();
    
    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_type_t type = ecs_type_from_str(world, "Position, Velocity");
    test_int(ecs_vector_count(type), 2);
    test_assert(ecs_type_has_entity(world, type, ecs_typeid(Position)));
    test_assert(ecs_type_has_entity(world, type, ecs_typeid(Velocity)));

    ecs_fini(world);
}

void Type_type_from_expr_scope() {
    ecs_world_t *world = ecs_init();
    
    ECS_COMPONENT(world, Position);

    ECS_ENTITY(world, scope, 0);
    ecs_set_scope(world, scope);
    ECS_COMPONENT(world, Velocity);
    ecs_set_scope(world, 0);

    ecs_type_t type = ecs_type_from_str(world, "Position, scope.Velocity");
    test_int(ecs_vector_count(type), 2);
    test_assert(ecs_type_has_entity(world, type, ecs_typeid(Position)));
    test_assert(ecs_type_has_entity(world, type, ecs_typeid(Velocity)));

    ecs_fini(world);
}

void Type_type_from_expr_digit() {
    ecs_world_t *world = ecs_init();

    ecs_type_t type = ecs_type_from_str(world, "10, 20");
    test_int(ecs_vector_count(type), 2);
    test_assert(ecs_type_has_entity(world, type, 10));
    test_assert(ecs_type_has_entity(world, type, 20));

    ecs_fini(world);
}

void Type_type_from_expr_instanceof() {
    ecs_world_t *world = ecs_init();
    
    ECS_ENTITY(world, Base, 0);

    ecs_type_t type = ecs_type_from_str(world, "INSTANCEOF | Base");
    test_int(ecs_vector_count(type), 1);
    test_assert(ecs_type_has_entity(world, type, ECS_INSTANCEOF | Base));

    ecs_fini(world);
}

void Type_type_from_expr_childof() {
    ecs_world_t *world = ecs_init();
    
    ECS_ENTITY(world, Parent, 0);

    ecs_type_t type = ecs_type_from_str(world, "CHILDOF | Parent");
    test_int(ecs_vector_count(type), 1);
    test_assert(ecs_type_has_entity(world, type, ECS_CHILDOF | Parent));

    ecs_fini(world);
}

void Type_type_from_expr_trait() {
    ecs_world_t *world = ecs_init();
    
    ECS_TAG(world, Trait);

    ecs_type_t type = ecs_type_from_str(world, "TRAIT | Trait");
    test_int(ecs_vector_count(type), 1);
    test_assert(ecs_type_has_entity(world, type, ECS_TRAIT | Trait));

    ecs_fini(world);
}

void Type_type_from_expr_trait_w_comp() {
    ecs_world_t *world = ecs_init();
    
    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Trait);

    ecs_type_t type = ecs_type_from_str(world, "TRAIT | Trait > Position");
    test_int(ecs_vector_count(type), 1);
    test_assert(ecs_type_has_entity(world, type, ecs_trait(ecs_typeid(Position), Trait)));

    ecs_fini(world);
}

void Type_entity_str() {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, e, 0);

    char buffer[256];
    size_t result = ecs_entity_str(world, e, buffer, 256);
    test_str(buffer, "e");
    test_int(strlen(buffer), result);

    ecs_fini(world);
}

void Type_entity_path_str() {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, parent, 0);
    ECS_ENTITY(world, e, CHILDOF | parent);

    char buffer[256];
    size_t result = ecs_entity_str(world, e, buffer, 256);
    test_str(buffer, "parent.e");
    test_int(strlen(buffer), result);

    ecs_fini(world);
}

void Type_entity_instanceof_str() {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Foo, 0);

    ecs_entity_t e = ECS_INSTANCEOF | Foo;

    char buffer[256];
    size_t result = ecs_entity_str(world, e, buffer, 256);
    test_str(buffer, "INSTANCEOF|Foo");
    test_int(strlen(buffer), result);

    ecs_fini(world);
}

void Type_entity_childof_str() {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Foo, 0);

    ecs_entity_t e = ECS_CHILDOF | Foo;

    char buffer[256];
    size_t result = ecs_entity_str(world, e, buffer, 256);
    test_str(buffer, "CHILDOF|Foo");
    test_int(strlen(buffer), result);

    ecs_fini(world);
}

void Type_entity_trait_str() {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Foo, 0);
    ECS_ENTITY(world, Bar, 0);

    ecs_entity_t e = ecs_trait(Foo, Bar);

    char buffer[256];
    size_t result = ecs_entity_str(world, e, buffer, 256);
    test_str(buffer, "TRAIT|Bar>Foo");
    test_int(strlen(buffer), result);

    ecs_fini(world);
}

void Type_entity_switch_str() {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Foo, 0);

    ecs_entity_t e = ECS_SWITCH | Foo;

    char buffer[256];
    size_t result = ecs_entity_str(world, e, buffer, 256);
    test_str(buffer, "SWITCH|Foo");
    test_int(strlen(buffer), result);

    ecs_fini(world);
}

void Type_entity_case_str() {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Foo, 0);

    ecs_entity_t e = ECS_CASE | Foo;

    char buffer[256];
    size_t result = ecs_entity_str(world, e, buffer, 256);
    test_str(buffer, "CASE|Foo");
    test_int(strlen(buffer), result);

    ecs_fini(world);
}

void Type_entity_and_str() {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Foo, 0);

    ecs_entity_t e = ECS_AND | Foo;

    char buffer[256];
    size_t result = ecs_entity_str(world, e, buffer, 256);
    test_str(buffer, "AND|Foo");
    test_int(strlen(buffer), result);

    ecs_fini(world);
}

void Type_entity_or_str() {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Foo, 0);

    ecs_entity_t e = ECS_OR | Foo;

    char buffer[256];
    size_t result = ecs_entity_str(world, e, buffer, 256);
    test_str(buffer, "OR|Foo");
    test_int(strlen(buffer), result);

    ecs_fini(world);
}

void Type_entity_xor_str() {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Foo, 0);

    ecs_entity_t e = ECS_XOR | Foo;

    char buffer[256];
    size_t result = ecs_entity_str(world, e, buffer, 256);
    test_str(buffer, "XOR|Foo");
    test_int(strlen(buffer), result);

    ecs_fini(world);
}

void Type_entity_not_str() {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Foo, 0);

    ecs_entity_t e = ECS_NOT | Foo;

    char buffer[256];
    size_t result = ecs_entity_str(world, e, buffer, 256);
    test_str(buffer, "NOT|Foo");
    test_int(strlen(buffer), result);

    ecs_fini(world);
}

void Type_entity_str_small_buffer() {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Foo, 0);

    ecs_entity_t e = ECS_INSTANCEOF | Foo;

    char buffer[10];
    size_t result = ecs_entity_str(world, e, buffer, 10);
    test_str(buffer, "INSTANCEO");
    test_int(strlen(buffer), 9);
    test_int(strlen("INSTANCEOF|Foo"), result);

    ecs_fini(world);
}

void Type_role_childof_str() {
    ecs_entity_t e = ECS_CHILDOF;
    test_str(ecs_role_str(e), "CHILDOF");
}

void Type_role_instanceof_str() {
    ecs_entity_t e = ECS_INSTANCEOF;
    test_str(ecs_role_str(e), "INSTANCEOF");
}

void Type_role_trait_str() {
    ecs_entity_t e = ECS_TRAIT;
    test_str(ecs_role_str(e), "TRAIT");
}

void Type_role_switch_str() {
    ecs_entity_t e = ECS_SWITCH;
    test_str(ecs_role_str(e), "SWITCH");
}

void Type_role_case_str() {
    ecs_entity_t e = ECS_CASE;
    test_str(ecs_role_str(e), "CASE");
}

void Type_role_and_str() {
    ecs_entity_t e = ECS_AND;
    test_str(ecs_role_str(e), "AND");
}

void Type_role_or_str() {
    ecs_entity_t e = ECS_OR;
    test_str(ecs_role_str(e), "OR");
}

void Type_role_xor_str() {
    ecs_entity_t e = ECS_XOR;
    test_str(ecs_role_str(e), "XOR");
}

void Type_role_not_str() {
    ecs_entity_t e = ECS_NOT;
    test_str(ecs_role_str(e), "NOT");
}

void Type_role_owned_str() {
    ecs_entity_t e = ECS_OWNED;
    test_str(ecs_role_str(e), "OWNED");
}

void Type_role_disabled_str() {
    ecs_entity_t e = ECS_DISABLED;
    test_str(ecs_role_str(e), "DISABLED");
}
