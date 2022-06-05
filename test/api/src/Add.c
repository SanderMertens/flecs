#include <api.h>

void Add_zero() {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ecs_entity_t e = ecs_new(world, 0);
    test_assert(e != 0);

    test_expect_abort();
    ecs_add(world, e, 0);
}

void Add_component() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new(world, 0);
    test_assert(e != 0);

    ecs_add(world, e, Position);
    test_assert(ecs_has(world, e, Position));
    
    ecs_fini(world);
}

void Add_component_again() {
    ecs_world_t *world = ecs_mini();

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
    ecs_world_t *world = ecs_mini();

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
    ecs_world_t *world = ecs_mini();

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
    ecs_world_t *world = ecs_mini();

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

void Add_component_to_nonempty() {
    ecs_world_t *world = ecs_mini();

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
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new(world, Position);
    test_assert(e != 0);
    test_assert(ecs_has(world, e, Position));

    ecs_add(world, e, Position);
    test_assert(ecs_has(world, e, Position));

    ecs_fini(world);
}

void Add_component_to_nonempty_overlap() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_ENTITY(world, e, Position, Velocity);
    test_assert(e != 0);
    test_assert(ecs_has(world, e, Position));
    test_assert(ecs_has(world, e, Velocity));

    ecs_add(world, e, Position);

    test_assert(ecs_has(world, e, Position));
    test_assert(ecs_has(world, e, Velocity));

    ecs_fini(world);
}

void Add_tag() {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Tag, 0);

    ecs_entity_t e = ecs_new(world, 0);
    test_assert(e != 0);

    ecs_add_id(world, e, Tag);
    test_assert(ecs_has_id(world, e, Tag));

    ecs_fini(world);
}

void Add_add_entity() {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e = ecs_new(world, 0);
    test_assert(e != 0);

    ecs_entity_t f = ecs_new(world, 0);
    test_assert(f != 0);

    ecs_add_id(world, e, f);
    test_assert(ecs_has_id(world, e, f));
    
    ecs_fini(world);
}

void Add_remove_entity() {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e = ecs_new(world, 0);
    test_assert(e != 0);

    ecs_entity_t f = ecs_new(world, 0);
    test_assert(f != 0);

    ecs_add_id(world, e, f);
    test_assert(ecs_has_id(world, e, f));

    ecs_remove_id(world, e, f);
    test_assert(!ecs_has_id(world, e, f));
    
    ecs_fini(world);
}

void Add_add_0_entity() {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ecs_entity_t e = ecs_new(world, 0);
    test_assert(e != 0);

    test_expect_abort();

    ecs_add_id(world, e, 0);
}

void Add_remove_0_entity() {
    install_test_abort();
    
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new(world, Position);
    test_assert(e != 0);

    test_expect_abort();

    ecs_remove_id(world, e, 0);
}

void Add_invalid_add_wildcard() {
    install_test_abort();
    
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e = ecs_new_id(world);
    test_assert(e != 0);

    test_expect_abort();

    ecs_add_id(world, e, EcsWildcard);
}

void Add_invalid_add_pair_w_wildcard() {
    install_test_abort();
    
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e = ecs_new_id(world);
    test_assert(e != 0);

    test_expect_abort();

    ecs_add_id(world, e, ecs_pair(EcsWildcard, EcsWildcard));
}

void Add_invalid_add_pair_w_wildcard_rel() {
    install_test_abort();

    ecs_world_t *world = ecs_mini();
    
    ECS_TAG(world, Tag);

    ecs_entity_t e = ecs_new_id(world);
    test_assert(e != 0);

    test_expect_abort();

    ecs_add_id(world, e, ecs_pair(EcsWildcard, Tag));
}

void Add_invalid_add_pair_w_wildcard_obj() {
    install_test_abort();

    ecs_world_t *world = ecs_mini();
    
    ECS_TAG(world, Tag);

    ecs_entity_t e = ecs_new_id(world);
    test_assert(e != 0);

    test_expect_abort();

    ecs_add_id(world, e, ecs_pair(Tag, EcsWildcard));
}

void Add_invalid_add_any() {
    install_test_abort();

    ecs_world_t *world = ecs_mini();
    
    ECS_TAG(world, Tag);

    ecs_entity_t e = ecs_new_id(world);
    test_assert(e != 0);

    test_expect_abort();

    ecs_add_id(world, e, EcsAny);
}

void Add_invalid_add_pair_w_any() {
    install_test_abort();

    ecs_world_t *world = ecs_mini();
    
    ECS_TAG(world, Tag);

    ecs_entity_t e = ecs_new_id(world);
    test_assert(e != 0);

    test_expect_abort();

    ecs_add_id(world, e, ecs_pair(EcsAny, EcsAny));
}

void Add_invalid_add_pair_w_any_rel() {
    install_test_abort();

    ecs_world_t *world = ecs_mini();
    
    ECS_TAG(world, Tag);

    ecs_entity_t e = ecs_new_id(world);
    test_assert(e != 0);

    test_expect_abort();

    ecs_add_id(world, e, ecs_pair(EcsAny, Tag));
}

void Add_invalid_add_pair_w_any_obj() {
    install_test_abort();

    ecs_world_t *world = ecs_mini();
    
    ECS_TAG(world, Tag);

    ecs_entity_t e = ecs_new_id(world);
    test_assert(e != 0);

    test_expect_abort();

    ecs_add_id(world, e, ecs_pair(Tag, EcsAny));
}

void Add_invalid_pair_w_0() {
    install_test_abort();

    ecs_world_t *world = ecs_mini();
    
    ECS_TAG(world, Tag);

    ecs_entity_t e = ecs_new_id(world);
    test_assert(e != 0);

    test_expect_abort();

    ecs_add_id(world, e, ecs_pair(0, 0));
}

void Add_invalid_pair_w_0_rel() {
    install_test_abort();

    ecs_world_t *world = ecs_mini();
    
    ECS_TAG(world, Tag);

    ecs_entity_t e = ecs_new_id(world);
    test_assert(e != 0);

    test_expect_abort();

    ecs_add_id(world, e, ecs_pair(0, Tag));
}

void Add_invalid_pair_w_0_obj() {
    install_test_abort();

    ecs_world_t *world = ecs_mini();
    
    ECS_TAG(world, Tag);

    ecs_entity_t e = ecs_new_id(world);
    test_assert(e != 0);

    test_expect_abort();

    ecs_add_id(world, e, ecs_pair(Tag, 0));
}
