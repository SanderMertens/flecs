#include <core.h>

void Has_zero(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ecs_entity_t e = ecs_new(world);
    test_assert(e != 0);

    test_expect_abort();
    ecs_has_id(world, e, 0);
}

void Has_1_of_0(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new(world);
    test_assert(e != 0);

    test_assert(!ecs_has(world, e, Position));
    
    ecs_fini(world);
}

void Has_1_of_1(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new_w(world, Position);
    test_assert(e != 0);

    test_assert(ecs_has(world, e, Position));
    
    ecs_fini(world);
}

void Has_1_of_2(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_ENTITY(world, e, Position, Velocity);
    test_assert(e != 0);

    test_assert(ecs_has(world, e, Position));
    test_assert(ecs_has(world, e, Velocity));
    
    ecs_fini(world);
}

void Has_1_of_empty(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new(world);
    test_assert(e != 0);

    test_assert(!ecs_has(world, e, Position));
    
    ecs_fini(world);
}

void TestHas(ecs_iter_t *it) {
    ecs_id_t ecs_id(Position) = ecs_field_id(it, 0);

    int i;
    for (i = 0; i < it->count; i ++) {
        test_assert( ecs_has(it->world, it->entities[i], Position));
    }
}

void Has_has_in_progress(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_SYSTEM(world, TestHas, EcsOnUpdate, Position);

    ecs_entity_t e = ecs_new_w(world, Position);
    test_assert(e != 0);
    test_assert( ecs_has(world, e, Position));

    ecs_progress(world, 1);
    
    ecs_fini(world);
}

void Has_has_of_zero(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    test_expect_abort();

    ecs_has_id(world, 0, 0);
}

void Has_owns(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_add_pair(world, ecs_id(Velocity), EcsOnInstantiate, EcsInherit);

    ecs_entity_t e = ecs_new_w(world, Position);    
    test_assert(e != 0);
    test_assert( ecs_has(world, e, Position));
    test_assert( ecs_owns(world, e, Position));

    ecs_entity_t base = ecs_new_w(world, Velocity);
    test_assert( ecs_has(world, base, Velocity));
    test_assert( ecs_owns(world, base, Velocity));

    ecs_add_pair(world, e, EcsIsA, base);
    test_assert( ecs_has(world, e, Velocity));
    test_assert( !ecs_owns(world, e, Velocity));
 
    ecs_fini(world);
}

void Has_owns_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag_1);
    ECS_TAG(world, Tag_2);

    ecs_entity_t e1 = ecs_new(world);
    test_assert(e1 != 0);

    ecs_entity_t e2 = ecs_new(world);
    test_assert(e2 != 0);    

    ecs_add_id(world, e1, Tag_1);

    test_bool(ecs_owns_id(world, e1, Tag_1), true);
    test_bool(ecs_owns_id(world, e1, Tag_2), false);
    test_bool(ecs_owns_id(world, e1, EcsWildcard), true);
 
    test_bool(ecs_owns_id(world, e2, Tag_1), false);
    test_bool(ecs_owns_id(world, e2, Tag_2), false);
    test_bool(ecs_owns_id(world, e2, EcsWildcard), false);

    ecs_fini(world);
}

void Has_owns_wildcard_pair(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_TAG(world, Obj_1);
    ECS_TAG(world, Obj_2);

    ecs_entity_t e = ecs_new(world);
    test_assert(e != 0);

    ecs_add_pair(world, e, Rel, Obj_1);

    test_bool(ecs_owns_pair(world, e, Rel, Obj_1), true);
    test_bool(ecs_owns_pair(world, e, Rel, Obj_2), false);
    test_bool(ecs_owns_pair(world, e, Rel, EcsWildcard), true);
    test_bool(ecs_owns_pair(world, e, EcsWildcard, Obj_1), true);
    test_bool(ecs_owns_pair(world, e, EcsWildcard, Obj_2), false);
    test_bool(ecs_owns_pair(world, e, EcsWildcard, EcsWildcard), true);
 
    ecs_fini(world);
}

void Has_has_entity(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e = ecs_new(world);
    test_assert(e != 0);

    ecs_entity_t base = ecs_new(world);
    test_assert(base != 0);

    ecs_add_pair(world, e, EcsIsA, base);
    test_assert( !ecs_has_id(world, e, base));
    test_assert( ecs_has_pair(world, e, EcsIsA, base));
 
    ecs_fini(world);
}

void Has_has_entity_0(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ecs_entity_t base = ecs_new(world);
    test_assert(base != 0);

    test_expect_abort();

    ecs_has_id(world, 0, base);
}

void Has_has_entity_0_component(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ecs_entity_t e = ecs_new(world);
    test_assert(e != 0);

    test_expect_abort();
    ecs_has_id(world, e, 0);
}

void Has_has_entity_owned(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e = ecs_new(world);
    test_assert(e != 0);
    
    ecs_entity_t f = ecs_new(world);
    test_assert(f != 0);

    ecs_entity_t g = ecs_new(world);
    test_assert(g != 0);

    ecs_entity_t base = ecs_new(world);
    test_assert(base != 0);

    ecs_add_pair(world, f, EcsOnInstantiate, EcsInherit);
    ecs_add_pair(world, g, EcsOnInstantiate, EcsInherit);

    ecs_add_id(world, e, f);
    ecs_add_id(world, base, g);
    ecs_add_pair(world, e, EcsIsA, base);

    test_assert( ecs_has_id(world, e, f));
    test_assert( ecs_has_id(world, e, g));
    test_assert( !ecs_has_id(world, e, base));
    test_assert( ecs_has_pair(world, e, EcsIsA, base));
    test_assert( ecs_owns_id(world, e, f));
    test_assert( !ecs_owns_id(world, e, g));
    test_assert( !ecs_owns_id(world, e, base));

    test_assert( ecs_owns_pair(world, e, EcsIsA, base) == true);
 
    ecs_fini(world);
}

void Has_has_entity_owned_0(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ecs_entity_t e = ecs_new(world);
    test_assert(e != 0);

    test_expect_abort();
    bool dummy = ecs_owns_id(world, 0, e);
    test_assert(!dummy); // should not reach this code
}

void Has_has_entity_owned_0_component(void) {
    install_test_abort();
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e = ecs_new(world);
    test_assert(e != 0);

    test_expect_abort();
    bool dummy = ecs_owns_id(world, e, 0);
    test_assert(!dummy); // should not reach this code
}

void Has_has_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag_1);
    ECS_TAG(world, Tag_2);

    ecs_entity_t e1 = ecs_new(world);
    test_assert(e1 != 0);

    ecs_entity_t e2 = ecs_new(world);
    test_assert(e2 != 0);    

    ecs_add_id(world, e1, Tag_1);

    test_bool(ecs_has_id(world, e1, Tag_1), true);
    test_bool(ecs_has_id(world, e1, Tag_2), false);
    test_bool(ecs_has_id(world, e1, EcsWildcard), true);
 
    test_bool(ecs_has_id(world, e2, Tag_1), false);
    test_bool(ecs_has_id(world, e2, Tag_2), false);
    test_bool(ecs_has_id(world, e2, EcsWildcard), false);

    ecs_fini(world);
}

void Has_has_wildcard_pair(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_TAG(world, Obj_1);
    ECS_TAG(world, Obj_2);

    ecs_entity_t e = ecs_new(world);
    test_assert(e != 0);

    ecs_add_pair(world, e, Rel, Obj_1);

    test_bool(ecs_has_pair(world, e, Rel, Obj_1), true);
    test_bool(ecs_has_pair(world, e, Rel, Obj_2), false);
    test_bool(ecs_has_pair(world, e, Rel, EcsWildcard), true);
    test_bool(ecs_has_pair(world, e, EcsWildcard, Obj_1), true);
    test_bool(ecs_has_pair(world, e, EcsWildcard, Obj_2), false);
    test_bool(ecs_has_pair(world, e, EcsWildcard, EcsWildcard), true);
 
    ecs_fini(world);
}
