#include <core.h>

void Union_add(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Movement, Union);
    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);

    ecs_entity_t e = ecs_new(world);
    test_assert(!ecs_has_pair(world, e, Movement, Walking));
    test_assert(!ecs_has_pair(world, e, Movement, Running));

    ecs_add_pair(world, e, Movement, Walking);
    test_assert(ecs_has_pair(world, e, Movement, Walking));
    test_assert(!ecs_has_pair(world, e, Movement, Running));

    ecs_fini(world);
}

void Union_add_twice(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Movement, Union);
    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);

    ecs_entity_t e = ecs_new(world);
    test_assert(!ecs_has_pair(world, e, Movement, Walking));
    test_assert(!ecs_has_pair(world, e, Movement, Running));

    ecs_add_pair(world, e, Movement, Walking);
    test_assert(ecs_has_pair(world, e, Movement, Walking));
    test_assert(!ecs_has_pair(world, e, Movement, Running));

    ecs_add_pair(world, e, Movement, Walking);
    test_assert(ecs_has_pair(world, e, Movement, Walking));
    test_assert(!ecs_has_pair(world, e, Movement, Running));

    ecs_fini(world);
}

void Union_add_replace(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Movement, Union);
    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);

    ecs_entity_t e = ecs_new(world);
    test_assert(!ecs_has_pair(world, e, Movement, Walking));
    test_assert(!ecs_has_pair(world, e, Movement, Running));

    ecs_add_pair(world, e, Movement, Walking);
    test_assert(ecs_has_pair(world, e, Movement, Walking));
    test_assert(!ecs_has_pair(world, e, Movement, Running));

    ecs_add_pair(world, e, Movement, Running);
    test_assert(!ecs_has_pair(world, e, Movement, Walking));
    test_assert(ecs_has_pair(world, e, Movement, Running));

    ecs_fini(world);
}

void Union_add_remove(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Movement, Union);
    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);

    ecs_entity_t e = ecs_new(world);
    test_assert(!ecs_has_pair(world, e, Movement, Walking));
    test_assert(!ecs_has_pair(world, e, Movement, Running));

    ecs_add_pair(world, e, Movement, Walking);
    test_assert(ecs_has_pair(world, e, Movement, Walking));
    test_assert(!ecs_has_pair(world, e, Movement, Running));

    ecs_remove_pair(world, e, Movement, EcsWildcard);
    test_assert(!ecs_has_pair(world, e, Movement, Walking));
    test_assert(!ecs_has_pair(world, e, Movement, Running));

    ecs_fini(world);
}

void Union_add_remove_recycled(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Movement, Union);

    ecs_entity_t ent = ecs_new(world);
    ecs_delete(world, ent);
    ecs_entity_t recycled = ecs_new(world);
    test_assert(ent != recycled);
    test_assert(ent == (uint32_t)recycled);

    ecs_entity_t e = ecs_new(world);
    test_assert(!ecs_has_pair(world, e, Movement, recycled));

    ecs_add_pair(world, e, Movement, recycled);
    test_assert(ecs_has_pair(world, e, Movement, recycled));

    ecs_remove_pair(world, e, Movement, EcsWildcard);
    test_assert(!ecs_has_pair(world, e, Movement, recycled));

    ecs_fini(world);
}

void Union_add_remove_add(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Movement, Union);
    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);

    ecs_entity_t e = ecs_new(world);
    test_assert(!ecs_has_pair(world, e, Movement, Walking));
    test_assert(!ecs_has_pair(world, e, Movement, Running));

    ecs_add_pair(world, e, Movement, Walking);
    test_assert(ecs_has_pair(world, e, Movement, Walking));
    test_assert(!ecs_has_pair(world, e, Movement, Running));

    ecs_remove_pair(world, e, Movement, EcsWildcard);
    test_assert(!ecs_has_pair(world, e, Movement, Walking));
    test_assert(!ecs_has_pair(world, e, Movement, Running));

    ecs_add_pair(world, e, Movement, Walking);
    test_assert(ecs_has_pair(world, e, Movement, Walking));
    test_assert(!ecs_has_pair(world, e, Movement, Running));

    ecs_fini(world);
}

void Union_get_target_none(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Movement, Union);
    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);

    ecs_entity_t e = ecs_new(world);
    test_uint(0, ecs_get_target(world, e, Movement, 0));

    ecs_fini(world);
}

void Union_get_target(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Movement, Union);
    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);

    ecs_entity_t e = ecs_new(world);
    test_uint(0, ecs_get_target(world, e, Movement, 0));

    ecs_add_pair(world, e, Movement, Walking);
    test_uint(Walking, ecs_get_target(world, e, Movement, 0));

    ecs_fini(world);
}

void Union_get_recycled_target(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Movement, Union);

    ecs_entity_t ent = ecs_new(world);
    ecs_delete(world, ent);
    ecs_entity_t recycled = ecs_new(world);
    test_assert(ent != recycled);
    test_assert(ent == (uint32_t)recycled);

    ecs_entity_t e = ecs_new(world);
    test_uint(0, ecs_get_target(world, e, Movement, 0));

    ecs_add_pair(world, e, Movement, recycled);
    test_uint(recycled, ecs_get_target(world, e, Movement, 0));

    ecs_remove_pair(world, e, Movement, EcsWildcard);
    test_uint(0, ecs_get_target(world, e, Movement, 0));

    ecs_fini(world);
}

void Union_get_target_after_replace(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Movement, Union);
    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);

    ecs_entity_t e = ecs_new(world);
    test_uint(0, ecs_get_target(world, e, Movement, 0));

    ecs_add_pair(world, e, Movement, Walking);
    test_uint(Walking, ecs_get_target(world, e, Movement, 0));

    ecs_add_pair(world, e, Movement, Running);
    test_uint(Running, ecs_get_target(world, e, Movement, 0));

    ecs_fini(world);
}

void Union_get_target_after_remove(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Movement, Union);
    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);

    ecs_entity_t e = ecs_new(world);
    test_uint(0, ecs_get_target(world, e, Movement, 0));

    ecs_add_pair(world, e, Movement, Walking);
    test_uint(Walking, ecs_get_target(world, e, Movement, 0));

    ecs_remove_pair(world, e, Movement, EcsWildcard);
    test_uint(0, ecs_get_target(world, e, Movement, 0));

    ecs_fini(world);
}

void Union_has_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Movement, Union);
    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);

    ecs_entity_t e = ecs_new(world);
    test_assert(!ecs_has_pair(world, e, Movement, Walking));
    test_assert(!ecs_has_pair(world, e, Movement, Running));

    ecs_add_pair(world, e, Movement, Walking);
    test_assert(ecs_has_pair(world, e, Movement, EcsWildcard));

    ecs_remove_pair(world, e, Movement, EcsWildcard);
    test_assert(!ecs_has_pair(world, e, Movement, EcsWildcard));

    ecs_fini(world);
}

void Union_has_any(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Movement, Union);
    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);

    ecs_entity_t e = ecs_new(world);
    test_assert(!ecs_has_pair(world, e, Movement, Walking));
    test_assert(!ecs_has_pair(world, e, Movement, Running));

    ecs_add_pair(world, e, Movement, Walking);
    test_assert(ecs_has_pair(world, e, Movement, EcsAny));

    ecs_remove_pair(world, e, Movement, EcsWildcard);
    test_assert(!ecs_has_pair(world, e, Movement, EcsAny));

    ecs_fini(world);
}

void Union_add_remove_2_tgts(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Movement, Union);
    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);

    ecs_entity_t e1 = ecs_new(world);
    test_assert(!ecs_has_pair(world, e1, Movement, Walking));
    test_assert(!ecs_has_pair(world, e1, Movement, Running));

    ecs_entity_t e2 = ecs_new(world);
    test_assert(!ecs_has_pair(world, e2, Movement, Walking));
    test_assert(!ecs_has_pair(world, e2, Movement, Running));

    ecs_add_pair(world, e1, Movement, Walking);
    test_assert(ecs_has_pair(world, e1, Movement, Walking));
    test_assert(!ecs_has_pair(world, e1, Movement, Running));
    test_assert(!ecs_has_pair(world, e2, Movement, Walking));
    test_assert(!ecs_has_pair(world, e2, Movement, Running));

    ecs_add_pair(world, e2, Movement, Running);
    test_assert(ecs_has_pair(world, e1, Movement, Walking));
    test_assert(!ecs_has_pair(world, e1, Movement, Running));
    test_assert(!ecs_has_pair(world, e2, Movement, Walking));
    test_assert(ecs_has_pair(world, e2, Movement, Running));

    ecs_remove_pair(world, e1, Movement, EcsWildcard);
    test_assert(!ecs_has_pair(world, e1, Movement, Walking));
    test_assert(!ecs_has_pair(world, e1, Movement, Running));
    test_assert(!ecs_has_pair(world, e2, Movement, Walking));
    test_assert(ecs_has_pair(world, e2, Movement, Running));

    ecs_remove_pair(world, e2, Movement, EcsWildcard);
    test_assert(!ecs_has_pair(world, e1, Movement, Walking));
    test_assert(!ecs_has_pair(world, e1, Movement, Running));
    test_assert(!ecs_has_pair(world, e2, Movement, Walking));
    test_assert(!ecs_has_pair(world, e2, Movement, Running));

    ecs_fini(world);
}

void Union_add_remove_2_tgts_join(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Movement, Union);
    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);

    ecs_entity_t e1 = ecs_new(world);
    test_assert(!ecs_has_pair(world, e1, Movement, Walking));
    test_assert(!ecs_has_pair(world, e1, Movement, Running));

    ecs_entity_t e2 = ecs_new(world);
    test_assert(!ecs_has_pair(world, e2, Movement, Walking));
    test_assert(!ecs_has_pair(world, e2, Movement, Running));

    ecs_add_pair(world, e1, Movement, Walking);
    test_assert(ecs_has_pair(world, e1, Movement, Walking));
    test_assert(!ecs_has_pair(world, e1, Movement, Running));
    test_assert(!ecs_has_pair(world, e2, Movement, Walking));
    test_assert(!ecs_has_pair(world, e2, Movement, Running));

    ecs_add_pair(world, e2, Movement, Running);
    test_assert(ecs_has_pair(world, e1, Movement, Walking));
    test_assert(!ecs_has_pair(world, e1, Movement, Running));
    test_assert(!ecs_has_pair(world, e2, Movement, Walking));
    test_assert(ecs_has_pair(world, e2, Movement, Running));

    ecs_add_pair(world, e2, Movement, Walking);
    test_assert(ecs_has_pair(world, e1, Movement, Walking));
    test_assert(!ecs_has_pair(world, e1, Movement, Running));
    test_assert(ecs_has_pair(world, e2, Movement, Walking));
    test_assert(!ecs_has_pair(world, e2, Movement, Running));

    ecs_remove_pair(world, e1, Movement, EcsWildcard);
    test_assert(!ecs_has_pair(world, e1, Movement, Walking));
    test_assert(!ecs_has_pair(world, e1, Movement, Running));
    test_assert(ecs_has_pair(world, e2, Movement, Walking));
    test_assert(!ecs_has_pair(world, e2, Movement, Running));

    ecs_remove_pair(world, e2, Movement, EcsWildcard);
    test_assert(!ecs_has_pair(world, e1, Movement, Walking));
    test_assert(!ecs_has_pair(world, e1, Movement, Running));
    test_assert(!ecs_has_pair(world, e2, Movement, Walking));
    test_assert(!ecs_has_pair(world, e2, Movement, Running));

    ecs_fini(world);
}

void Union_add_remove_3_tgts(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Movement, Union);
    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);
    ECS_TAG(world, Sitting);

    ecs_entity_t e1 = ecs_new(world);
    test_assert(!ecs_has_pair(world, e1, Movement, Walking));
    test_assert(!ecs_has_pair(world, e1, Movement, Running));
    test_assert(!ecs_has_pair(world, e1, Movement, Sitting));

    ecs_entity_t e2 = ecs_new(world);
    test_assert(!ecs_has_pair(world, e2, Movement, Walking));
    test_assert(!ecs_has_pair(world, e2, Movement, Running));
    test_assert(!ecs_has_pair(world, e2, Movement, Sitting));

    ecs_entity_t e3 = ecs_new(world);
    test_assert(!ecs_has_pair(world, e3, Movement, Walking));
    test_assert(!ecs_has_pair(world, e3, Movement, Running));
    test_assert(!ecs_has_pair(world, e3, Movement, Sitting));

    ecs_add_pair(world, e1, Movement, Walking);
    test_assert(ecs_has_pair(world, e1, Movement, Walking));
    test_assert(!ecs_has_pair(world, e1, Movement, Running));
    test_assert(!ecs_has_pair(world, e1, Movement, Sitting));
    test_assert(!ecs_has_pair(world, e2, Movement, Walking));
    test_assert(!ecs_has_pair(world, e2, Movement, Running));
    test_assert(!ecs_has_pair(world, e2, Movement, Sitting));
    test_assert(!ecs_has_pair(world, e3, Movement, Walking));
    test_assert(!ecs_has_pair(world, e3, Movement, Running));
    test_assert(!ecs_has_pair(world, e3, Movement, Sitting));

    ecs_add_pair(world, e2, Movement, Running);
    test_assert(ecs_has_pair(world, e1, Movement, Walking));
    test_assert(!ecs_has_pair(world, e1, Movement, Running));
    test_assert(!ecs_has_pair(world, e2, Movement, Walking));
    test_assert(ecs_has_pair(world, e2, Movement, Running));
    test_assert(!ecs_has_pair(world, e3, Movement, Walking));
    test_assert(!ecs_has_pair(world, e3, Movement, Running));
    test_assert(!ecs_has_pair(world, e3, Movement, Sitting));

    ecs_add_pair(world, e3, Movement, Sitting);
    test_assert(ecs_has_pair(world, e1, Movement, Walking));
    test_assert(!ecs_has_pair(world, e1, Movement, Running));
    test_assert(!ecs_has_pair(world, e2, Movement, Walking));
    test_assert(ecs_has_pair(world, e2, Movement, Running));
    test_assert(!ecs_has_pair(world, e3, Movement, Walking));
    test_assert(!ecs_has_pair(world, e3, Movement, Running));
    test_assert(ecs_has_pair(world, e3, Movement, Sitting));

    ecs_remove_pair(world, e1, Movement, EcsWildcard);
    test_assert(!ecs_has_pair(world, e1, Movement, Walking));
    test_assert(!ecs_has_pair(world, e1, Movement, Running));
    test_assert(!ecs_has_pair(world, e2, Movement, Walking));
    test_assert(ecs_has_pair(world, e2, Movement, Running));
    test_assert(!ecs_has_pair(world, e3, Movement, Walking));
    test_assert(!ecs_has_pair(world, e3, Movement, Running));
    test_assert(ecs_has_pair(world, e3, Movement, Sitting));

    ecs_remove_pair(world, e2, Movement, EcsWildcard);
    test_assert(!ecs_has_pair(world, e1, Movement, Walking));
    test_assert(!ecs_has_pair(world, e1, Movement, Running));
    test_assert(!ecs_has_pair(world, e2, Movement, Walking));
    test_assert(!ecs_has_pair(world, e2, Movement, Running));
    test_assert(!ecs_has_pair(world, e3, Movement, Walking));
    test_assert(!ecs_has_pair(world, e3, Movement, Running));
    test_assert(ecs_has_pair(world, e3, Movement, Sitting));

    ecs_remove_pair(world, e3, Movement, EcsWildcard);
    test_assert(!ecs_has_pair(world, e1, Movement, Walking));
    test_assert(!ecs_has_pair(world, e1, Movement, Running));
    test_assert(!ecs_has_pair(world, e2, Movement, Walking));
    test_assert(!ecs_has_pair(world, e2, Movement, Running));
    test_assert(!ecs_has_pair(world, e3, Movement, Walking));
    test_assert(!ecs_has_pair(world, e3, Movement, Running));
    test_assert(!ecs_has_pair(world, e3, Movement, Sitting));

    ecs_fini(world);
}

void Union_add_remove_3_tgts_join(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Movement, Union);
    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);
    ECS_TAG(world, Sitting);

    ecs_entity_t e1 = ecs_new(world);
    test_assert(!ecs_has_pair(world, e1, Movement, Walking));
    test_assert(!ecs_has_pair(world, e1, Movement, Running));
    test_assert(!ecs_has_pair(world, e1, Movement, Sitting));

    ecs_entity_t e2 = ecs_new(world);
    test_assert(!ecs_has_pair(world, e2, Movement, Walking));
    test_assert(!ecs_has_pair(world, e2, Movement, Running));
    test_assert(!ecs_has_pair(world, e2, Movement, Sitting));

    ecs_entity_t e3 = ecs_new(world);
    test_assert(!ecs_has_pair(world, e3, Movement, Walking));
    test_assert(!ecs_has_pair(world, e3, Movement, Running));
    test_assert(!ecs_has_pair(world, e3, Movement, Sitting));

    ecs_add_pair(world, e1, Movement, Walking);
    test_assert(ecs_has_pair(world, e1, Movement, Walking));
    test_assert(!ecs_has_pair(world, e1, Movement, Running));
    test_assert(!ecs_has_pair(world, e1, Movement, Sitting));
    test_assert(!ecs_has_pair(world, e2, Movement, Walking));
    test_assert(!ecs_has_pair(world, e2, Movement, Running));
    test_assert(!ecs_has_pair(world, e2, Movement, Sitting));
    test_assert(!ecs_has_pair(world, e3, Movement, Walking));
    test_assert(!ecs_has_pair(world, e3, Movement, Running));
    test_assert(!ecs_has_pair(world, e3, Movement, Sitting));

    ecs_add_pair(world, e2, Movement, Running);
    test_assert(ecs_has_pair(world, e1, Movement, Walking));
    test_assert(!ecs_has_pair(world, e1, Movement, Running));
    test_assert(!ecs_has_pair(world, e1, Movement, Sitting));
    test_assert(!ecs_has_pair(world, e2, Movement, Walking));
    test_assert(ecs_has_pair(world, e2, Movement, Running));
    test_assert(!ecs_has_pair(world, e2, Movement, Sitting));
    test_assert(!ecs_has_pair(world, e3, Movement, Walking));
    test_assert(!ecs_has_pair(world, e3, Movement, Running));
    test_assert(!ecs_has_pair(world, e3, Movement, Sitting));

    ecs_add_pair(world, e3, Movement, Sitting);
    test_assert(ecs_has_pair(world, e1, Movement, Walking));
    test_assert(!ecs_has_pair(world, e1, Movement, Running));
    test_assert(!ecs_has_pair(world, e1, Movement, Sitting));
    test_assert(!ecs_has_pair(world, e2, Movement, Walking));
    test_assert(ecs_has_pair(world, e2, Movement, Running));
    test_assert(!ecs_has_pair(world, e2, Movement, Sitting));
    test_assert(!ecs_has_pair(world, e3, Movement, Walking));
    test_assert(!ecs_has_pair(world, e3, Movement, Running));
    test_assert(ecs_has_pair(world, e3, Movement, Sitting));

    ecs_add_pair(world, e1, Movement, Sitting);
    test_assert(!ecs_has_pair(world, e1, Movement, Walking));
    test_assert(!ecs_has_pair(world, e1, Movement, Running));
    test_assert(ecs_has_pair(world, e1, Movement, Sitting));
    test_assert(!ecs_has_pair(world, e2, Movement, Walking));
    test_assert(ecs_has_pair(world, e2, Movement, Running));
    test_assert(!ecs_has_pair(world, e2, Movement, Sitting));
    test_assert(!ecs_has_pair(world, e3, Movement, Walking));
    test_assert(!ecs_has_pair(world, e3, Movement, Running));
    test_assert(ecs_has_pair(world, e3, Movement, Sitting));

    ecs_add_pair(world, e2, Movement, Sitting);
    test_assert(!ecs_has_pair(world, e1, Movement, Walking));
    test_assert(!ecs_has_pair(world, e1, Movement, Running));
    test_assert(ecs_has_pair(world, e1, Movement, Sitting));
    test_assert(!ecs_has_pair(world, e2, Movement, Walking));
    test_assert(!ecs_has_pair(world, e2, Movement, Running));
    test_assert(ecs_has_pair(world, e2, Movement, Sitting));
    test_assert(!ecs_has_pair(world, e3, Movement, Walking));
    test_assert(!ecs_has_pair(world, e3, Movement, Running));
    test_assert(ecs_has_pair(world, e3, Movement, Sitting));

    ecs_remove_pair(world, e1, Movement, EcsWildcard);
    test_assert(!ecs_has_pair(world, e1, Movement, Walking));
    test_assert(!ecs_has_pair(world, e1, Movement, Running));
    test_assert(!ecs_has_pair(world, e1, Movement, Sitting));
    test_assert(!ecs_has_pair(world, e2, Movement, Walking));
    test_assert(!ecs_has_pair(world, e2, Movement, Running));
    test_assert(ecs_has_pair(world, e2, Movement, Sitting));
    test_assert(!ecs_has_pair(world, e3, Movement, Walking));
    test_assert(!ecs_has_pair(world, e3, Movement, Running));
    test_assert(ecs_has_pair(world, e3, Movement, Sitting));

    ecs_remove_pair(world, e2, Movement, EcsWildcard);
    test_assert(!ecs_has_pair(world, e1, Movement, Walking));
    test_assert(!ecs_has_pair(world, e1, Movement, Running));
    test_assert(!ecs_has_pair(world, e1, Movement, Sitting));
    test_assert(!ecs_has_pair(world, e2, Movement, Walking));
    test_assert(!ecs_has_pair(world, e2, Movement, Running));
    test_assert(!ecs_has_pair(world, e2, Movement, Sitting));
    test_assert(!ecs_has_pair(world, e3, Movement, Walking));
    test_assert(!ecs_has_pair(world, e3, Movement, Running));
    test_assert(ecs_has_pair(world, e3, Movement, Sitting));

    ecs_remove_pair(world, e3, Movement, EcsWildcard);
    test_assert(!ecs_has_pair(world, e1, Movement, Walking));
    test_assert(!ecs_has_pair(world, e1, Movement, Running));
    test_assert(!ecs_has_pair(world, e1, Movement, Sitting));
    test_assert(!ecs_has_pair(world, e2, Movement, Walking));
    test_assert(!ecs_has_pair(world, e2, Movement, Running));
    test_assert(!ecs_has_pair(world, e2, Movement, Sitting));
    test_assert(!ecs_has_pair(world, e3, Movement, Walking));
    test_assert(!ecs_has_pair(world, e3, Movement, Running));
    test_assert(!ecs_has_pair(world, e3, Movement, Sitting));

    ecs_fini(world);
}
