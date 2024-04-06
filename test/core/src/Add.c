#include <core.h>
#include <stdlib.h>

void Add_zero(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ecs_entity_t e = ecs_new(world);
    test_assert(e != 0);

    test_expect_abort();
    ecs_add_id(world, e, 0);
}

void Add_component(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new(world);
    test_assert(e != 0);

    ecs_add(world, e, Position);
    test_assert(ecs_has(world, e, Position));
    
    ecs_fini(world);
}

void Add_component_again(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new(world);
    test_assert(e != 0);

    ecs_add(world, e, Position);
    test_assert(ecs_has(world, e, Position));

    ecs_add(world, e, Position);
    test_assert(ecs_has(world, e, Position));
    
    ecs_fini(world);
}

void Add_2_components(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t e = ecs_new(world);
    test_assert(e != 0);

    ecs_add(world, e, Position);
    test_assert(ecs_has(world, e, Position));
    test_assert(!ecs_has(world, e, Velocity));

    ecs_add(world, e, Velocity);
    test_assert(ecs_has(world, e, Position));
    test_assert(ecs_has(world, e, Velocity));
    
    ecs_fini(world);
}

void Add_2_components_again(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t e = ecs_new(world);
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

void Add_2_components_overlap(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);

    ecs_entity_t e = ecs_new(world);
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

void Add_component_to_nonempty(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t e = ecs_new_w(world, Position);
    test_assert(e != 0);
    test_assert(ecs_has(world, e, Position));

    ecs_add(world, e, Velocity);
    test_assert(ecs_has(world, e, Position));
    test_assert(ecs_has(world, e, Velocity));

    ecs_fini(world);
}

void Add_component_to_nonempty_again(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new_w(world, Position);
    test_assert(e != 0);
    test_assert(ecs_has(world, e, Position));

    ecs_add(world, e, Position);
    test_assert(ecs_has(world, e, Position));

    ecs_fini(world);
}

void Add_component_to_nonempty_overlap(void) {
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

void Add_tag(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Tag, 0);

    ecs_entity_t e = ecs_new(world);
    test_assert(e != 0);

    ecs_add_id(world, e, Tag);
    test_assert(ecs_has_id(world, e, Tag));

    ecs_fini(world);
}

void Add_add_entity(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e = ecs_new(world);
    test_assert(e != 0);

    ecs_entity_t f = ecs_new(world);
    test_assert(f != 0);

    ecs_add_id(world, e, f);
    test_assert(ecs_has_id(world, e, f));
    
    ecs_fini(world);
}

void Add_remove_entity(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e = ecs_new(world);
    test_assert(e != 0);

    ecs_entity_t f = ecs_new(world);
    test_assert(f != 0);

    ecs_add_id(world, e, f);
    test_assert(ecs_has_id(world, e, f));

    ecs_remove_id(world, e, f);
    test_assert(!ecs_has_id(world, e, f));
    
    ecs_fini(world);
}

void Add_add_0_entity(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ecs_entity_t e = ecs_new(world);
    test_assert(e != 0);

    test_expect_abort();

    ecs_add_id(world, e, 0);
}

void Add_remove_0_entity(void) {
    install_test_abort();
    
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new_w(world, Position);
    test_assert(e != 0);

    test_expect_abort();

    ecs_remove_id(world, e, 0);
}

void Add_invalid_add_wildcard(void) {
    install_test_abort();
    
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e = ecs_new(world);
    test_assert(e != 0);

    test_expect_abort();

    ecs_add_id(world, e, EcsWildcard);
}

void Add_invalid_add_pair_w_wildcard(void) {
    install_test_abort();
    
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e = ecs_new(world);
    test_assert(e != 0);

    test_expect_abort();

    ecs_add_id(world, e, ecs_pair(EcsWildcard, EcsWildcard));
}

void Add_invalid_add_pair_w_wildcard_rel(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();
    
    ECS_TAG(world, Tag);

    ecs_entity_t e = ecs_new(world);
    test_assert(e != 0);

    test_expect_abort();

    ecs_add_id(world, e, ecs_pair(EcsWildcard, Tag));
}

void Add_invalid_add_pair_w_wildcard_obj(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();
    
    ECS_TAG(world, Tag);

    ecs_entity_t e = ecs_new(world);
    test_assert(e != 0);

    test_expect_abort();

    ecs_add_id(world, e, ecs_pair(Tag, EcsWildcard));
}

void Add_invalid_add_any(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();
    
    ECS_TAG(world, Tag);

    ecs_entity_t e = ecs_new(world);
    test_assert(e != 0);

    test_expect_abort();

    ecs_add_id(world, e, EcsAny);
}

void Add_invalid_add_pair_w_any(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();
    
    ECS_TAG(world, Tag);

    ecs_entity_t e = ecs_new(world);
    test_assert(e != 0);

    test_expect_abort();

    ecs_add_id(world, e, ecs_pair(EcsAny, EcsAny));
}

void Add_invalid_add_pair_w_any_rel(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();
    
    ECS_TAG(world, Tag);

    ecs_entity_t e = ecs_new(world);
    test_assert(e != 0);

    test_expect_abort();

    ecs_add_id(world, e, ecs_pair(EcsAny, Tag));
}

void Add_invalid_add_pair_w_any_obj(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();
    
    ECS_TAG(world, Tag);

    ecs_entity_t e = ecs_new(world);
    test_assert(e != 0);

    test_expect_abort();

    ecs_add_id(world, e, ecs_pair(Tag, EcsAny));
}

void Add_invalid_pair_w_0(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();
    
    ECS_TAG(world, Tag);

    ecs_entity_t e = ecs_new(world);
    test_assert(e != 0);

    test_expect_abort();

    ecs_add_id(world, e, ecs_pair(0, 0));
}

void Add_invalid_pair_w_0_rel(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();
    
    ECS_TAG(world, Tag);

    ecs_entity_t e = ecs_new(world);
    test_assert(e != 0);

    test_expect_abort();

    ecs_add_id(world, e, ecs_pair(0, Tag));
}

void Add_invalid_pair_w_0_obj(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();
    
    ECS_TAG(world, Tag);

    ecs_entity_t e = ecs_new(world);
    test_assert(e != 0);

    test_expect_abort();

    ecs_add_id(world, e, ecs_pair(Tag, 0));
}

void Add_add_random_id(void) {
    ecs_world_t *world = ecs_mini();

    for (int i = 0; i < 10; i ++) {
        ecs_entity_t id = rand() % (100 * 1000) + 1000;
        ecs_make_alive(world, id);
        test_assert(ecs_is_alive(world, id));
        ecs_entity_t e = ecs_new_w_id(world, id);
        test_assert(ecs_has_id(world, e, id));
    }

    ecs_fini(world);
}
