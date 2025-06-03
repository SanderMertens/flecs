#include <core.h>

void Remove_zero(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ecs_entity_t e = ecs_new(world);
    test_assert(e != 0);

    test_expect_abort();
    ecs_remove_id(world, e, 0);
}

void Remove_1_of_1(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new_w(world, Position);
    test_assert(e != 0);

    ecs_remove(world, e, Position);
    test_assert(!ecs_has(world, e, Position));
    
    ecs_fini(world);
}

void Remove_1_of_1_again(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new_w(world, Position);
    test_assert(e != 0);

    ecs_remove(world, e, Position);
    test_assert(!ecs_has(world, e, Position));

    ecs_remove(world, e, Position);
    test_assert(!ecs_has(world, e, Position));
    
    ecs_fini(world);
}

void Remove_1_of_2(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_ENTITY(world, e, Position, Velocity);
    test_assert(e != 0);

    ecs_remove(world, e, Position);
    test_assert(!ecs_has(world, e, Position));
    test_assert(ecs_has(world, e, Velocity));
    
    ecs_fini(world);
}

void Remove_2_of_2(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_ENTITY(world, e, Position, Velocity);
    test_assert(e != 0);

    ecs_remove(world, e, Position);
    test_assert(!ecs_has(world, e, Position));
    test_assert(ecs_has(world, e, Velocity));

    ecs_remove(world, e, Velocity);
    test_assert(!ecs_has(world, e, Position));
    test_assert(!ecs_has(world, e, Velocity));
    
    ecs_fini(world);
}

void Remove_2_of_3(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);

    ECS_ENTITY(world, e, Position, Velocity, Mass);
    test_assert(e != 0);

    ecs_remove(world, e, Position);
    test_assert(!ecs_has(world, e, Position));
    test_assert(ecs_has(world, e, Velocity));
    test_assert(ecs_has(world, e, Mass));

    ecs_remove(world, e, Velocity);
    test_assert(!ecs_has(world, e, Position));
    test_assert(!ecs_has(world, e, Velocity));
    test_assert(ecs_has(world, e, Mass));
    
    ecs_fini(world);
}

void Remove_2_again(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_ENTITY(world, e, Position, Velocity);
    test_assert(e != 0);

    ecs_remove(world, e, Position);
    ecs_remove(world, e, Velocity);
    test_assert(!ecs_has(world, e, Position));
    test_assert(!ecs_has(world, e, Velocity));

    ecs_remove(world, e, Position);
    ecs_remove(world, e, Velocity);
    test_assert(!ecs_has(world, e, Position));
    test_assert(!ecs_has(world, e, Velocity));
    
    ecs_fini(world);
}

void Remove_2_overlap(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);

    ECS_ENTITY(world, e, Position, Velocity, Mass);
    test_assert(e != 0);

    ecs_remove(world, e, Position);
    ecs_remove(world, e, Velocity);
    test_assert(!ecs_has(world, e, Position));
    test_assert(!ecs_has(world, e, Velocity));
    test_assert(ecs_has(world, e, Mass));
    
    ecs_fini(world);
}

void Remove_1_from_empty(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new(world);
    test_assert(e != 0);

    ecs_remove(world, e, Position);
    test_assert(!ecs_has(world, e, Position));
    
    ecs_fini(world);
}

void Remove_not_added(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t e = ecs_new_w(world, Position);
    test_assert(e != 0);

    ecs_remove(world, e, Velocity);
    test_assert(ecs_has(world, e, Position));
    test_assert(!ecs_has(world, e, Velocity));

    ecs_fini(world);
}

void Remove_remove_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t e = ecs_new_w(world, Position);
    test_assert(e != 0);

    ecs_remove_id(world, e, EcsWildcard);
    test_assert(!ecs_has(world, e, Position));
    test_assert(!ecs_has(world, e, Velocity));

    ecs_fini(world);
}

void Remove_remove_some_wildcard_pair(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, RelX);
    ECS_TAG(world, RelY);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_entity_t e = ecs_new_w(world, Position);
    test_assert(e != 0);
    ecs_add_pair(world, e, RelX, TgtA);
    ecs_add_pair(world, e, RelX, TgtB);
    ecs_add_pair(world, e, RelY, TgtA);

    ecs_remove_pair(world, e, RelX, EcsWildcard);
    test_assert(ecs_has(world, e, Position));
    test_assert(!ecs_has_pair(world, e, RelX, TgtA));
    test_assert(!ecs_has_pair(world, e, RelX, TgtB));
    test_assert(ecs_has_pair(world, e, RelY, TgtA));

    ecs_fini(world);
}

void Remove_remove_all_tgt_wildcard_pair(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelX);
    ECS_TAG(world, RelY);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_entity_t e = ecs_new(world);
    test_assert(e != 0);
    ecs_add_pair(world, e, RelX, TgtA);
    ecs_add_pair(world, e, RelX, TgtB);

    ecs_remove_pair(world, e, RelX, EcsWildcard);
    test_assert(!ecs_has_pair(world, e, RelX, TgtA));
    test_assert(!ecs_has_pair(world, e, RelX, TgtB));

    ecs_fini(world);
}

void Remove_remove_all_rel_wildcard_pair(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelX);
    ECS_TAG(world, RelY);
    ECS_TAG(world, Tgt);

    ecs_entity_t e = ecs_new(world);
    test_assert(e != 0);
    ecs_add_pair(world, e, RelX, Tgt);
    ecs_add_pair(world, e, RelY, Tgt);

    ecs_remove_pair(world, e, EcsWildcard, Tgt);
    test_assert(!ecs_has_pair(world, e, RelX, Tgt));
    test_assert(!ecs_has_pair(world, e, RelX, Tgt));

    ecs_fini(world);
}
