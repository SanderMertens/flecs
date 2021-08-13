#include <api.h>

void Add_zero() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t e = ecs_new(world, 0);
    test_assert(e != 0);

    ecs_add(world, e, 0);
    test_assert(!ecs_get_type(world, e));
    
    ecs_fini(world);
}

void Add_component() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new(world, 0);
    test_assert(e != 0);

    ecs_add(world, e, Position);
    test_assert(ecs_has(world, e, Position));
    
    ecs_fini(world);
}

void Add_component_again() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new(world, 0);
    test_assert(e != 0);

    ecs_add(world, e, Position);
    test_assert(ecs_has(world, e, Position));

    ecs_add(world, e, Position);
    test_assert(ecs_has(world, e, Position));
    
    ecs_fini(world);
}

void Add_2_components() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t e = ecs_new(world, 0);
    test_assert(e != 0);

    ecs_add(world, e, Position);
    test_assert(ecs_has(world, e, Position));
    test_assert(!ecs_has(world, e, Velocity));

    ecs_add(world, e, Velocity);
    test_assert(ecs_has(world, e, Position));
    test_assert(ecs_has(world, e, Velocity));
    
    ecs_fini(world);
}

void Add_2_components_again() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t e = ecs_new(world, 0);
    test_assert(e != 0);

    ecs_add(world, e, Position);
    ecs_add(world, e, Velocity);
    test_assert(ecs_has(world, e, Position));
    test_assert(ecs_has(world, e, Velocity));

    ecs_add(world, e, Position);
    ecs_add(world, e, Velocity);
    test_assert(ecs_has(world, e, Position));
    test_assert(ecs_has(world, e, Velocity));
    
    ecs_fini(world);
}

void Add_2_components_overlap() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);

    ecs_entity_t e = ecs_new(world, 0);
    test_assert(e != 0);

    ecs_add(world, e, Position);
    ecs_add(world, e, Velocity);
    test_assert(ecs_has(world, e, Position));
    test_assert(ecs_has(world, e, Velocity));
    test_assert(!ecs_has(world, e, Mass));

    ecs_add(world, e, Velocity);
    ecs_add(world, e, Mass);
    test_assert(ecs_has(world, e, Position));
    test_assert(ecs_has(world, e, Velocity));
    test_assert(ecs_has(world, e, Mass));
    
    ecs_fini(world);
}

void Add_type() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_TYPE(world, Type, Position);

    ecs_entity_t e = ecs_new(world, 0);
    test_assert(e != 0);

    ecs_add(world, e, Type);
    test_assert(ecs_has(world, e, Position));

    ecs_fini(world);
}

void Add_type_of_2() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TYPE(world, Type, Position, Velocity);

    ecs_entity_t e = ecs_new(world, 0);
    test_assert(e != 0);

    ecs_add(world, e, Type);
    test_assert(ecs_has(world, e, Position));
    test_assert(ecs_has(world, e, Velocity));

    ecs_fini(world);
}

void Add_type_w_type() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_TYPE(world, Type_1, Position);
    ECS_TYPE(world, Type_2, AND | Type_1);

    ecs_entity_t e = ecs_new(world, 0);
    test_assert(e != 0);

    ecs_add(world, e, Type_2);
    test_assert(ecs_has(world, e, Position));

    ecs_fini(world);
}

void Add_type_w_2_types() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TYPE(world, Type_1, Position);
    ECS_TYPE(world, Type_2, Velocity);
    ECS_TYPE(world, Type_3, AND | Type_1, AND | Type_2);

    ecs_entity_t e = ecs_new(world, 0);
    test_assert(e != 0);

    ecs_add(world, e, Type_3);
    test_assert(ecs_has(world, e, Position));
    test_assert(ecs_has(world, e, Velocity));

    ecs_fini(world);
}

void Add_type_mixed() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TYPE(world, Type_1, Position);
    ECS_TYPE(world, Type_2, AND | Type_1, Velocity);

    ecs_entity_t e = ecs_new(world, 0);
    test_assert(e != 0);

    ecs_add(world, e, Type_2);
    test_assert(ecs_has(world, e, Position));
    test_assert(ecs_has(world, e, Velocity));

    ecs_fini(world);
}

void Add_type_again() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_TYPE(world, Type_1, Position);

    ecs_entity_t e = ecs_new(world, 0);
    test_assert(e != 0);

    ecs_add(world, e, Type_1);
    test_assert(ecs_has(world, e, Position));

    ecs_add(world, e, Type_1);
    test_assert(ecs_has(world, e, Position));

    ecs_fini(world);
}

void Add_type_overlap() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_TYPE(world, Type_1, Position, Velocity);
    ECS_TYPE(world, Type_2, Velocity, Mass);

    ecs_entity_t e = ecs_new(world, 0);
    test_assert(e != 0);

    ecs_add(world, e, Type_1);
    test_assert(ecs_has(world, e, Position));
    test_assert(ecs_has(world, e, Velocity));
    test_assert(!ecs_has(world, e, Mass));

    ecs_add(world, e, Type_2);
    test_assert(ecs_has(world, e, Position));
    test_assert(ecs_has(world, e, Velocity));
    test_assert(ecs_has(world, e, Mass));

    ecs_fini(world);
}

void Add_type_again_component() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_TYPE(world, Type, Position);

    ecs_entity_t e = ecs_new(world, 0);
    test_assert(e != 0);

    ecs_add(world, e, Position);
    test_assert(ecs_has(world, e, Position));

    ecs_add(world, e, Type);
    test_assert(ecs_has(world, e, Position));

    ecs_fini(world);
}

void Add_type_overlap_component() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TYPE(world, Type, Position, Velocity);

    ecs_entity_t e = ecs_new(world, 0);
    test_assert(e != 0);

    ecs_add(world, e, Position);
    test_assert(ecs_has(world, e, Position));

    ecs_add(world, e, Type);
    test_assert(ecs_has(world, e, Position));
    test_assert(ecs_has(world, e, Velocity));

    ecs_fini(world);
}

void Add_component_to_nonempty() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t e = ecs_new(world, Position);
    test_assert(e != 0);
    test_assert(ecs_has(world, e, Position));

    ecs_add(world, e, Velocity);
    test_assert(ecs_has(world, e, Position));
    test_assert(ecs_has(world, e, Velocity));

    ecs_fini(world);
}

void Add_component_to_nonempty_again() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new(world, Position);
    test_assert(e != 0);
    test_assert(ecs_has(world, e, Position));

    ecs_add(world, e, Position);
    test_assert(ecs_has(world, e, Position));

    ecs_fini(world);
}

void Add_component_to_nonempty_overlap() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TYPE(world, Type, Position, Velocity);

    ecs_entity_t e = ecs_new(world, Type);
    test_assert(e != 0);
    test_assert(ecs_has(world, e, Position));
    test_assert(ecs_has(world, e, Velocity));

    ecs_add(world, e, Position);

    test_assert(ecs_has(world, e, Position));
    test_assert(ecs_has(world, e, Velocity));

    ecs_fini(world);
}

void Add_type_to_nonempty() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TYPE(world, Type, Position, Velocity);

    ecs_entity_t e = ecs_new(world, Position);
    test_assert(e != 0);
    test_assert(ecs_has(world, e, Position));

    ecs_add(world, e, Type);
    test_assert(ecs_has(world, e, Position));
    test_assert(ecs_has(world, e, Velocity));

    ecs_fini(world);
}

void Add_type_to_nonempty_again() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TYPE(world, Type, Position, Velocity);

    ecs_entity_t e = ecs_new(world, Type);
    test_assert(e != 0);
    test_assert(ecs_has(world, e, Position));
    test_assert(ecs_has(world, e, Velocity));

    ecs_add(world, e, Type);
    test_assert(ecs_has(world, e, Position));
    test_assert(ecs_has(world, e, Velocity));

    ecs_fini(world);
}

void Add_type_to_nonempty_overlap() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_TYPE(world, Type_1, Position, Velocity);
    ECS_TYPE(world, Type_2, Velocity, Mass);

    ecs_entity_t e = ecs_new(world, Type_1);
    test_assert(e != 0);
    test_assert(ecs_has(world, e, Position));
    test_assert(ecs_has(world, e, Velocity));
    test_assert(!ecs_has(world, e, Mass));

    ecs_add(world, e, Type_2);
    test_assert(ecs_has(world, e, Position));
    test_assert(ecs_has(world, e, Velocity));
    test_assert(ecs_has(world, e, Mass));

    ecs_fini(world);
}

void Add_tag() {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Tag, 0);

    ecs_entity_t e = ecs_new(world, 0);
    test_assert(e != 0);

    ecs_add_id(world, e, Tag);
    test_assert(ecs_has_entity(world, e, Tag));

    ecs_fini(world);
}

void Add_type_w_tag() {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Tag, 0);
    ECS_TYPE(world, Type, Tag);

    ecs_entity_t e = ecs_new(world, 0);
    test_assert(e != 0);

    ecs_add(world, e, Type);
    test_assert(ecs_has(world, e, Type));
    test_assert(ecs_has_entity(world, e, Tag));

    ecs_fini(world);
}

void Add_type_w_2_tags() {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Tag_1, 0);
    ECS_ENTITY(world, Tag_2, 0);
    ECS_TYPE(world, Type, Tag_1, Tag_2);

    ecs_entity_t e = ecs_new(world, 0);
    test_assert(e != 0);

    ecs_add(world, e, Type);
    test_assert(ecs_has(world, e, Type));
    test_assert(ecs_has_entity(world, e, Tag_1));
    test_assert(ecs_has_entity(world, e, Tag_2));

    ecs_fini(world);
}

void Add_type_w_tag_mixed() {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Tag_1, 0);
    ECS_ENTITY(world, Tag_2, 0);
    ECS_COMPONENT(world, Position);
    ECS_TYPE(world, Type, Tag_1, Tag_2, Position);

    ecs_entity_t e = ecs_new(world, 0);
    test_assert(e != 0);

    ecs_add(world, e, Type);
    test_assert(ecs_has(world, e, Type));
    test_assert(ecs_has_entity(world, e, Tag_1));
    test_assert(ecs_has_entity(world, e, Tag_2));
    test_assert(ecs_has(world, e, Position));

    ecs_fini(world);
}

void Add_add_remove() {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Tag_1, 0);
    ECS_ENTITY(world, Tag_2, 0);
    ECS_ENTITY(world, Tag_3, 0);
    ECS_TYPE(world, Type, Tag_1, Tag_2, Tag_3);

    ecs_entity_t e = ecs_new(world, 0);
    test_assert(e != 0);

    ecs_type_t ecs_type(Tag_1) = ecs_type_from_id(world, Tag_1);
    ecs_add_remove(world, e, Tag_1, Type);
    test_assert(ecs_has_entity(world, e, Tag_1));
    test_assert(!ecs_has_entity(world, e, Tag_2));
    test_assert(!ecs_has_entity(world, e, Tag_3));

    ecs_type_t ecs_type(Tag_2) = ecs_type_from_id(world, Tag_2);
    ecs_add_remove(world, e, Tag_2, Type);
    test_assert(!ecs_has_entity(world, e, Tag_1));
    test_assert(ecs_has_entity(world, e, Tag_2));
    test_assert(!ecs_has_entity(world, e, Tag_3));

    ecs_fini(world);
}

void Add_add_remove_same() {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Tag_1, 0);

    ecs_entity_t e = ecs_new(world, 0);
    test_assert(e != 0);

    ecs_type_t ecs_type(Tag_1) = ecs_type_from_id(world, Tag_1);
    ecs_add_remove(world, e, Tag_1, Tag_1);
    test_assert(ecs_has_entity(world, e, Tag_1));

    ecs_fini(world);
}

void Add_add_remove_entity() {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Tag_1, 0);
    ECS_ENTITY(world, Tag_2, 0);

    ecs_entity_t e = ecs_new_w_id(world, Tag_2);
    test_assert(e != 0);
    test_assert(ecs_has_entity(world, e, Tag_2));

    ecs_add_remove_entity(world, e, Tag_1, Tag_2);
    test_assert(ecs_has_entity(world, e, Tag_1));
    test_assert(!ecs_has_entity(world, e, Tag_2));

    ecs_add_remove_entity(world, e, Tag_2, Tag_1);
    test_assert(!ecs_has_entity(world, e, Tag_1));
    test_assert(ecs_has_entity(world, e, Tag_2));

    ecs_fini(world);
}

void Add_add_remove_entity_same() {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Tag_1, 0);

    ecs_entity_t e = ecs_new(world, 0);
    test_assert(e != 0);

    ecs_add_remove_entity(world, e, Tag_1, Tag_1);
    test_assert(ecs_has_entity(world, e, Tag_1));

    ecs_fini(world);
}

void Add_add_2_remove() {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Tag_1, 0);
    ECS_ENTITY(world, Tag_2, 0);
    ECS_ENTITY(world, Tag_3, 0);
    ECS_TYPE(world, Type1, Tag_1, Tag_2, Tag_3);
    ECS_TYPE(world, Type2, Tag_1, Tag_2);
    ECS_TYPE(world, Type3, Tag_2, Tag_3);

    ecs_entity_t e = ecs_new(world, 0);
    test_assert(e != 0);

    ecs_add_remove(world, e, Type2, Type1);
    test_assert(ecs_has_entity(world, e, Tag_1));
    test_assert(ecs_has_entity(world, e, Tag_2));
    test_assert(!ecs_has_entity(world, e, Tag_3));

    ecs_add_remove(world, e, Type3, Type1);
    test_assert(!ecs_has_entity(world, e, Tag_1));
    test_assert(ecs_has_entity(world, e, Tag_2));
    test_assert(ecs_has_entity(world, e, Tag_3));

    ecs_fini(world);
}

void Add_add_entity() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t e = ecs_new(world, 0);
    test_assert(e != 0);

    ecs_entity_t f = ecs_new(world, 0);
    test_assert(f != 0);

    ecs_add_id(world, e, f);
    test_assert(ecs_has_entity(world, e, f));
    
    ecs_fini(world);
}

void Add_remove_entity() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t e = ecs_new(world, 0);
    test_assert(e != 0);

    ecs_entity_t f = ecs_new(world, 0);
    test_assert(f != 0);

    ecs_add_id(world, e, f);
    test_assert(ecs_has_entity(world, e, f));

    ecs_remove_id(world, e, f);
    test_assert(!ecs_has_entity(world, e, f));
    
    ecs_fini(world);
}

void Add_add_0_entity() {
    install_test_abort();

    ecs_world_t *world = ecs_init();

    ecs_entity_t e = ecs_new(world, 0);
    test_assert(e != 0);

    test_expect_abort();

    ecs_add_id(world, e, 0);
}

void Add_remove_0_entity() {
    install_test_abort();
    
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new(world, Position);
    test_assert(e != 0);

    test_expect_abort();

    ecs_remove_id(world, e, 0);
}

void Add_add_w_xor() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TYPE(world, Type, Position, Velocity);

    ecs_entity_t e = ecs_new(world, Position);
    test_assert(e != 0);
    test_assert( ecs_has(world, e, Position));

    ecs_add_id(world, e, ECS_XOR | Type);
    test_assert( ecs_has_entity(world, e, ECS_XOR | Type));
    test_assert( ecs_has(world, e, Position));

    ecs_add(world, e, Velocity);
    test_assert( !ecs_has(world, e, Position));
    test_assert( ecs_has(world, e, Velocity));
    
    ecs_fini(world);
}

void Add_add_same_w_xor() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TYPE(world, Type, Position, Velocity);

    ecs_entity_t e = ecs_new(world, Position);
    test_assert(e != 0);
    test_assert( ecs_has(world, e, Position));

    ecs_add_id(world, e, ECS_XOR | Type);
    test_assert( ecs_has_entity(world, e, ECS_XOR | Type));
    test_assert( ecs_has(world, e, Position));

    ecs_add(world, e, Position);
    test_assert( ecs_has(world, e, Position));
    
    ecs_fini(world);
}

void Add_add_after_remove_xor() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TYPE(world, Type, Position, Velocity);

    ecs_entity_t e = ecs_new(world, Position);
    test_assert(e != 0);
    test_assert( ecs_has(world, e, Position));

    ecs_add_id(world, e, ECS_XOR | Type);
    test_assert( ecs_has_entity(world, e, ECS_XOR | Type));
    test_assert( ecs_has(world, e, Position));

    ecs_remove_id(world, e, ECS_XOR | Type);
    test_assert( !ecs_has_entity(world, e, ECS_XOR | Type));

    ecs_add(world, e, Velocity);
    test_assert( ecs_has(world, e, Position));
    test_assert( ecs_has(world, e, Velocity));
    
    ecs_fini(world);
}

