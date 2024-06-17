#include <core.h>

static
void SetCase(ecs_iter_t *it) {
    ecs_world_t *world = it->world;
    ecs_entity_t id = ecs_field_id(it, 1);

    int i;
    for (i = 0; i < it->count; i ++) {
        ecs_add_id(world, it->entities[i], id);
    }
}

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

void Union_remove_w_union_tgt(void) {
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

    ecs_remove_pair(world, e, Movement, EcsUnion);
    test_assert(!ecs_has_pair(world, e, Movement, Walking));
    test_assert(!ecs_has_pair(world, e, Movement, Running));

    ecs_fini(world);
}

void Union_get_non_union_tgt_from_table_w_union(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Likes);
    ECS_ENTITY(world, Movement, Union);
    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);

    ecs_entity_t e = ecs_new(world);
    test_assert(!ecs_has_pair(world, e, Movement, Walking));
    test_assert(!ecs_has_pair(world, e, Movement, Running));

    ecs_add_pair(world, e, Movement, Walking);
    test_assert(ecs_has_pair(world, e, Movement, Walking));
    test_assert(!ecs_has_pair(world, e, Movement, Running));

    ecs_entity_t movement = ecs_get_target(world, e, Movement, 0);
    test_assert(movement != 0);
    test_assert(movement == Walking);

    ecs_entity_t likes = ecs_get_target(world, e, Likes, 0);
    test_assert(likes == 0);

    ecs_fini(world);
}

void Union_has_non_union_from_table_w_union(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Likes);
    ECS_ENTITY(world, Movement, Union);
    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);

    ecs_entity_t e = ecs_new(world);
    test_assert(!ecs_has_pair(world, e, Movement, Walking));
    test_assert(!ecs_has_pair(world, e, Movement, Running));

    ecs_add_pair(world, e, Movement, Walking);
    test_assert(ecs_has_pair(world, e, Movement, Walking));
    test_assert(!ecs_has_pair(world, e, Movement, Running));

    test_assert(ecs_has_pair(world, e, Movement, Walking));
    test_assert(!ecs_has_pair(world, e, Likes, Walking));

    ecs_fini(world);
}

void Union_get_case_no_switch(void) {
    ecs_world_t *world = ecs_mini();
    
    ECS_COMPONENT(world, Position);
    ECS_ENTITY(world, Rel, Union);

    ecs_entity_t e = ecs_new_w(world, Position);
    test_assert(e != 0);

    test_uint(ecs_get_target(world, e, Rel, 0), 0);

    ecs_fini(world);
}

void Union_get_case_set(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Movement, Union);
    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);
    ECS_TAG(world, Jumping);

    ecs_entity_t e = ecs_new_w_pair(world, Movement, Walking);
    test_assert(e != 0);

    test_assert( ecs_has_pair(world, e, Movement, Walking));
    test_assert( !ecs_has_pair(world, e, Movement, Running));
    test_assert( !ecs_has_pair(world, e, Movement, Jumping));
    test_uint(ecs_get_target(world, e, Movement, 0), Walking);

    ecs_fini(world);
}

void Union_get_case_change(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Movement, Union);
    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);
    ECS_TAG(world, Jumping);

    ecs_entity_t e = ecs_new_w_pair(world, Movement, Walking);
    test_assert(e != 0);

    test_assert( ecs_has_pair(world, e, Movement, Walking));
    test_assert( !ecs_has_pair(world, e, Movement, Running));
    test_assert( !ecs_has_pair(world, e, Movement, Jumping));
    test_uint(ecs_get_target(world, e, Movement, 0), Walking);

    ecs_add_pair(world, e, Movement, Running);

    test_assert( !ecs_has_pair(world, e, Movement, Walking));
    test_assert( ecs_has_pair(world, e, Movement, Running));
    test_assert( !ecs_has_pair(world, e, Movement, Jumping));
    test_uint(ecs_get_target(world, e, Movement, 0), Running);

    ecs_add_pair(world, e, Movement, Jumping);

    test_assert( !ecs_has_pair(world, e, Movement, Walking));
    test_assert( !ecs_has_pair(world, e, Movement, Running));
    test_assert( ecs_has_pair(world, e, Movement, Jumping));
    test_uint(ecs_get_target(world, e, Movement, 0), Jumping);

    ecs_fini(world);
}

void Union_remove_case(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Movement, Union);
    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);
    ECS_TAG(world, Jumping);

    ecs_entity_t e = ecs_new_w_pair(world, Movement, Walking);
    test_assert(e != 0);

    test_assert( ecs_has_pair(world, e, Movement, Walking));
    test_assert( !ecs_has_pair(world, e, Movement, Running));
    test_assert( !ecs_has_pair(world, e, Movement, Jumping));
    test_uint(ecs_get_target(world, e, Movement, 0), Walking);

    ecs_remove_pair(world, e, Movement, Walking);

    test_assert( !ecs_has_pair(world, e, Movement, Walking));
    test_assert( !ecs_has_pair(world, e, Movement, Running));
    test_assert( !ecs_has_pair(world, e, Movement, Jumping));
    test_uint(ecs_get_target(world, e, Movement, 0), 0);

    test_assert(ecs_get_type(world, e) == NULL);

    ecs_fini(world);
}

void Union_remove_last(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Movement, Union);
    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);
    ECS_TAG(world, Tag);

    ecs_entity_t e1 = ecs_new_w_pair(world, Movement, Walking);
    test_assert(e1 != 0);
    ecs_add(world, e1, Tag);

    ecs_entity_t e2 = ecs_new_w_pair(world, Movement, Walking);
    test_assert(e2 != 0);
    ecs_add(world, e2, Tag);

    ecs_remove(world, e2, Tag);

    test_assert(!ecs_has(world, e2, Tag));
    test_assert(ecs_has_pair(world, e2, Movement, Walking));

    test_assert(ecs_has(world, e1, Tag));
    test_assert(ecs_has_pair(world, e1, Movement, Walking));

    ecs_fini(world);
}

void Union_delete_first(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Movement, Union);
    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);

    ecs_query_t *q = ecs_query(world, {
        .expr = "(Movement, Walking)"
    });
    test_assert(q != NULL);

    ecs_entity_t e1 = ecs_new_w_pair(world, Movement, Walking);
    test_assert(e1 != 0);

    ecs_entity_t e2 = ecs_new_w_pair(world, Movement, Walking);
    test_assert(e2 != 0);

    ecs_entity_t e3 = ecs_new_w_pair(world, Movement, Walking);
    test_assert(e3 != 0);

    ecs_delete(world, e1);

    test_assert(ecs_has_pair(world, e2, Movement, Walking));
    test_assert(ecs_has_pair(world, e3, Movement, Walking));

    ecs_iter_t it = ecs_query_iter(world, q);

    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e3);

    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e2);

    test_assert(!ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Union_delete_last(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Movement, Union);
    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);

    ecs_query_t *q = ecs_query(world, {
        .expr = "(Movement, Walking)"
    });
    test_assert(q != NULL);

    ecs_entity_t e1 = ecs_new_w_pair(world, Movement, Walking);
    test_assert(e1 != 0);

    ecs_entity_t e2 = ecs_new_w_pair(world, Movement, Walking);
    test_assert(e2 != 0);

    ecs_entity_t e3 = ecs_new_w_pair(world, Movement, Walking);
    test_assert(e3 != 0);

    ecs_delete(world, e3);

    test_assert(ecs_has_pair(world, e1, Movement, Walking));
    test_assert(ecs_has_pair(world, e2, Movement, Walking));

    ecs_iter_t it = ecs_query_iter(world, q);

    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e2);

    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e1);

    test_assert(!ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Union_delete_first_last(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Movement, Union);
    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);

    ecs_query_t *q = ecs_query(world, {
        .expr = "(Movement, Walking)"
    });
    test_assert(q != NULL);

    ecs_entity_t e1 = ecs_new_w_pair(world, Movement, Walking);
    test_assert(e1 != 0);

    ecs_entity_t e2 = ecs_new_w_pair(world, Movement, Walking);
    test_assert(e2 != 0);

    ecs_entity_t e3 = ecs_new_w_pair(world, Movement, Walking);
    test_assert(e3 != 0);

    ecs_delete(world, e1);
    ecs_delete(world, e3);

    test_assert(ecs_has_pair(world, e2, Movement, Walking));

    ecs_iter_t it = ecs_query_iter(world, q);

    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e2);

    test_assert(!ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Union_3_entities_same_case(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Movement, Union);
    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);
    ECS_TAG(world, Jumping);

    ECS_ENTITY(world, e1, (Movement, Running));
    ECS_ENTITY(world, e2, (Movement, Running));
    ECS_ENTITY(world, e3, (Movement, Running));

    test_assert( ecs_has_pair(world, e1, Movement, Running));
    test_assert( ecs_has_pair(world, e2, Movement, Running));
    test_assert( ecs_has_pair(world, e3, Movement, Running));

    test_int(ecs_get_target(world, e1, Movement, 0), Running);
    test_int(ecs_get_target(world, e2, Movement, 0), Running);
    test_int(ecs_get_target(world, e3, Movement, 0), Running);

    ecs_fini(world);
}

void Union_2_entities_1_change_case(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Movement, Union);
    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);
    ECS_TAG(world, Jumping);

    ECS_ENTITY(world, e1, (Movement, Running));
    ECS_ENTITY(world, e2, (Movement, Running));

    ecs_add_pair(world, e2, Movement, Jumping);
    test_assert( ecs_has_pair(world, e1, Movement, Running));
    test_assert( ecs_has_pair(world, e2, Movement, Jumping));

    test_int(ecs_get_target(world, e1, Movement, 0), Running);
    test_int(ecs_get_target(world, e2, Movement, 0), Jumping);

    ecs_fini(world);
}

void Union_3_entities_change_case(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Movement, Union);
    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);
    ECS_TAG(world, Jumping);

    ECS_ENTITY(world, e1, (Movement, Running));
    ECS_ENTITY(world, e2, (Movement, Running));
    ECS_ENTITY(world, e3, (Movement, Running));

    ecs_add_pair(world, e1, Movement, Walking);
    test_assert( ecs_has_pair(world, e1, Movement, Walking));
    test_assert( ecs_has_pair(world, e2, Movement, Running));
    test_assert( ecs_has_pair(world, e3, Movement, Running));

    test_int(ecs_get_target(world, e1, Movement, 0), Walking);
    test_int(ecs_get_target(world, e2, Movement, 0), Running);
    test_int(ecs_get_target(world, e3, Movement, 0), Running);

    ecs_add_pair(world, e2, Movement, Jumping);
    test_assert( ecs_has_pair(world, e1, Movement, Walking));
    test_assert( ecs_has_pair(world, e2, Movement, Jumping));
    test_assert( ecs_has_pair(world, e3, Movement, Running));

    test_int(ecs_get_target(world, e1, Movement, 0), Walking);
    test_int(ecs_get_target(world, e2, Movement, 0), Jumping);
    test_int(ecs_get_target(world, e3, Movement, 0), Running); 

    ecs_add_pair(world, e3, Movement, Walking);
    test_assert( ecs_has_pair(world, e1, Movement, Walking));
    test_assert( ecs_has_pair(world, e2, Movement, Jumping));
    test_assert( ecs_has_pair(world, e3, Movement, Walking));

    test_int(ecs_get_target(world, e1, Movement, 0), Walking);
    test_int(ecs_get_target(world, e2, Movement, 0), Jumping);
    test_int(ecs_get_target(world, e3, Movement, 0), Walking);

    ecs_fini(world);
}

void Union_add_case_in_stage(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_ENTITY(world, Movement, Union);
    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);
    ECS_TAG(world, Jumping);

    ECS_SYSTEM(world, SetCase, EcsOnUpdate, Position, Movement(#0, Walking));

    ECS_ENTITY(world, e1, Position);
    ECS_ENTITY(world, e2, Position);
    ECS_ENTITY(world, e3, Position);

    ecs_progress(world, 0);

    test_assert(ecs_has_pair(world, e1, Movement, Walking));
    test_assert(ecs_has_pair(world, e2, Movement, Walking));
    test_assert(ecs_has_pair(world, e3, Movement, Walking));

    ecs_fini(world);
}

void Union_change_case_in_stage(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_ENTITY(world, Movement, Union);
    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);
    ECS_TAG(world, Jumping);

    ECS_SYSTEM(world, SetCase, EcsOnUpdate, Position, Movement(#0, Walking));

    ECS_ENTITY(world, e1, Position, (Movement, Running));
    ECS_ENTITY(world, e2, Position, (Movement, Running));
    ECS_ENTITY(world, e3, Position, (Movement, Running));

    ecs_progress(world, 0);

    test_assert(!ecs_has_pair(world, e1, Movement, Running));
    test_assert(!ecs_has_pair(world, e2, Movement, Running));
    test_assert(!ecs_has_pair(world, e3, Movement, Running));

    test_assert(ecs_has_pair(world, e1, Movement, Walking));
    test_assert(ecs_has_pair(world, e2, Movement, Walking));
    test_assert(ecs_has_pair(world, e3, Movement, Walking));

    ecs_fini(world);
}

void Union_change_one_case_in_stage(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_ENTITY(world, Movement, Union);
    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);
    ECS_TAG(world, Jumping);

    ECS_SYSTEM(world, SetCase, EcsOnUpdate, Position, Movement(#0, Jumping), (Movement, Walking));

    ECS_ENTITY(world, e0, Position, (Movement, Jumping));
    ECS_ENTITY(world, e1, Position, (Movement, Walking));
    ECS_ENTITY(world, e2, Position, (Movement, Running));
    ECS_ENTITY(world, e3, Position, (Movement, Walking));
    ECS_ENTITY(world, e4, Position, (Movement, Running));
    ECS_ENTITY(world, e5, Position, (Movement, Jumping));

    ecs_progress(world, 0);

    test_assert(ecs_has_pair(world, e0, Movement, Jumping));
    test_assert(ecs_has_pair(world, e1, Movement, Jumping));
    test_assert(ecs_has_pair(world, e2, Movement, Running));
    test_assert(ecs_has_pair(world, e3, Movement, Jumping));
    test_assert(ecs_has_pair(world, e4, Movement, Running));
    test_assert(ecs_has_pair(world, e5, Movement, Jumping));

    ecs_fini(world);
}

static
void RemoveSwitch(ecs_iter_t *it) {
    ecs_world_t *world = it->world;
    ecs_entity_t id = ecs_field_id(it, 0);

    int i;
    for (i = 0; i < it->count; i ++) {
        ecs_remove_id(world, it->entities[i], id);
    }
}

void Union_remove_switch_in_stage(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_ENTITY(world, Movement, Union);
    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);
    ECS_TAG(world, Jumping);

    ECS_SYSTEM(world, RemoveSwitch, EcsOnUpdate, (Movement, *));

    ECS_ENTITY(world, e1, Position, (Movement, Walking));
    ECS_ENTITY(world, e2, Position, (Movement, Walking));
    ECS_ENTITY(world, e3, Position, (Movement, Walking));

    ecs_progress(world, 0);

    test_assert(!ecs_has_pair(world, e1, Movement, Walking));
    test_assert(!ecs_has_pair(world, e2, Movement, Walking));
    test_assert(!ecs_has_pair(world, e3, Movement, Walking));

    test_assert(!ecs_has_pair(world, e1, Movement, EcsWildcard));
    test_assert(!ecs_has_pair(world, e2, Movement, EcsWildcard));
    test_assert(!ecs_has_pair(world, e3, Movement, EcsWildcard));

    ecs_fini(world);
}

void Union_switch_no_match_for_case(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_ENTITY(world, Movement, Union);
    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);
    ECS_TAG(world, Jumping);

    ecs_new_w_pair(world, Movement, Walking);

    ecs_query_t *q = ecs_query(world, {
        .expr = "(Movement, Running)"
    });
    ecs_iter_t it = ecs_query_iter(world, q);

    int count = 0;
    while (ecs_query_next(&it)) {
        count ++;
    }

    test_assert(count == 0);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Union_empty_entity_has_case(void) {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Movement, Union);
    ECS_TAG(world, Walking);

    ecs_entity_t e = ecs_new(world);
    test_assert(!ecs_has_pair(world, e, Movement, Walking));

    ecs_fini(world);
}

void Union_zero_entity_has_case(void) {
    install_test_abort();

    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Movement, Union);
    ECS_TAG(world, Walking);

    test_expect_abort();

    test_assert(!ecs_has_pair(world, 0, Movement, Walking));
}

void Union_add_to_entity_w_switch(void) {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Movement, Union);
    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);
    ECS_TAG(world, Jumping);
    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new_w_pair(world, Movement, Walking);
    test_assert(e != 0);

    test_assert( ecs_has_pair(world, e, Movement, Walking));
    test_int(ecs_get_target(world, e, Movement, 0), Walking);

    ecs_add(world, e, Position);
    test_assert( ecs_has(world, e, Position));
    test_assert( ecs_has_pair(world, e, Movement, Walking));
    test_int(ecs_get_target(world, e, Movement, 0), Walking);

    ecs_fini(world);
}

void Union_add_pair_to_entity_w_switch(void) {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Movement, Union);
    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);
    ECS_TAG(world, Jumping);
    
    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Pair);

    ecs_entity_t e = ecs_new_w_pair(world, Movement, Walking);
    test_assert(e != 0);
    test_assert( ecs_has_pair(world, e, Movement, Walking));
    test_int(ecs_get_target(world, e, Movement, 0), Walking);

    ecs_entity_t pair_id = ecs_pair(ecs_id(Position), Pair);
    ecs_add_id(world, e, pair_id);
    test_assert(ecs_has_id(world, e, pair_id));
    test_assert( ecs_has_pair(world, e, Movement, Walking));
    test_int(ecs_get_target(world, e, Movement, 0), Walking);

    ecs_fini(world);
}

void Union_recycled_tags(void) {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Movement, Union);

    ecs_entity_t e1 = ecs_new(world);
    ecs_entity_t e2 = ecs_new(world);
    ecs_entity_t e3 = ecs_new(world);

    ecs_delete(world, e1);
    ecs_delete(world, e2);
    ecs_delete(world, e3);

    ECS_TAG(world, Standing);
    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);

    test_assert(Standing > UINT32_MAX);
    test_assert(Walking > UINT32_MAX);
    test_assert(Running > UINT32_MAX);

    ecs_entity_t e = ecs_new_w_pair(world, Movement, Standing);
    test_assert(ecs_has_pair(world, e, Movement, Standing));
    test_assert(!ecs_has_pair(world, e, Movement, Walking));
    test_assert(!ecs_has_pair(world, e, Movement, Running));

    ecs_add_pair(world, e, Movement, Walking);
    test_assert(!ecs_has_pair(world, e, Movement, Standing));
    test_assert(ecs_has_pair(world, e, Movement, Walking));
    test_assert(!ecs_has_pair(world, e, Movement, Running));

    ecs_fini(world);
}

void Union_same_table_after_change(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Movement, Union);
    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);
    ECS_TAG(world, Jumping);

    ecs_entity_t e = ecs_new_w_pair(world, Movement, Walking);
    test_assert(e != 0);

    ecs_table_t *table = ecs_get_table(world, e);
    test_assert(table != NULL);

    ecs_add_pair(world, e, Movement, Running);
    test_assert(ecs_get_table(world, e) == table);

    ecs_add_pair(world, e, Movement, Jumping);
    test_assert(ecs_get_table(world, e) == table);
    
    ecs_remove_pair(world, e, Movement, EcsWildcard);
    test_assert(ecs_get_table(world, e) != table);
    test_assert(ecs_get_table(world, e) == NULL);

    ecs_fini(world);
}

void Union_add_2(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Movement, Union);
    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);
    ECS_TAG(world, Jumping);

    ECS_ENTITY(world, Direction, Union);
    ECS_TAG(world, Up);
    ECS_TAG(world, Down);

    ecs_entity_t e = ecs_new(world);
    ecs_add_pair(world, e, Movement, Walking);
    ecs_add_pair(world, e, Direction, Up);

    test_assert( ecs_has_pair(world, e, Movement, Walking));
    test_assert( ecs_has_pair(world, e, Direction, Up));

    test_assert(ecs_get_target(world, e, Movement, 0) == Walking);
    test_assert(ecs_get_target(world, e, Direction, 0) == Up);

    ecs_fini(world);
}

void Union_add_2_reverse(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Movement, Union);
    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);
    ECS_TAG(world, Jumping);

    ECS_ENTITY(world, Direction, Union);
    ECS_TAG(world, Up);
    ECS_TAG(world, Down);

    ecs_entity_t e = ecs_new(world);
    ecs_add_pair(world, e, Direction, Up);
    ecs_add_pair(world, e, Movement, Walking);

    test_assert( ecs_has_pair(world, e, Movement, Walking));
    test_assert( ecs_has_pair(world, e, Direction, Up));

    test_assert(ecs_get_target(world, e, Movement, 0) == Walking);
    test_assert(ecs_get_target(world, e, Direction, 0) == Up);

    ecs_fini(world);
}

void Union_add_switch_to_prefab_instance(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Movement, Union, (OnInstantiate, Inherit));
    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);
    ECS_TAG(world, Jumping);

    ecs_entity_t base = ecs_new(world);
    ecs_entity_t inst = ecs_new_w_pair(world, EcsIsA, base);
    ecs_add_pair(world, inst, Movement, Walking);

    test_assert(ecs_has_pair(world, inst, Movement, Walking));
    test_assert(ecs_get_target(world, inst, Movement, 0) == Walking);

    ecs_fini(world);
}

void Union_get_case_w_generation(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Rel, Union);

    ecs_entity_t tgt = ecs_new(world);
    ecs_delete(world, tgt);
    tgt = ecs_new(world);
    test_assert(tgt != (uint32_t)tgt);

    ecs_entity_t e = ecs_new(world);
    ecs_add_pair(world, e, Rel, tgt);

    ecs_entity_t t = ecs_get_target(world, e, Rel, 0);
    test_assert(t == tgt);

    ecs_fini(world);
}

void Union_get_case_w_generation_not_alive(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Rel, Union);

    ecs_entity_t tgt = ecs_new(world);
    ecs_delete(world, tgt);
    tgt = ecs_new(world);
    test_assert(tgt != (uint32_t)tgt);

    ecs_entity_t e = ecs_new(world);
    ecs_add_pair(world, e, Rel, tgt);

    ecs_entity_t t = ecs_get_target(world, e, Rel, 0);
    test_assert(t == tgt);

    ecs_delete(world, tgt);

    t = ecs_get_target(world, e, Rel, 0);
    test_assert(t == tgt);

    ecs_fini(world);
}

void Union_defer_add_union_relationship(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Rel, Union);
    ECS_TAG(world, Tgt);

    ecs_entity_t e = ecs_new(world);
    ecs_defer_begin(world);
    ecs_add_pair(world, e, Rel, Tgt);
    test_assert(!ecs_has_pair(world, e, Rel, Tgt));
    ecs_defer_end(world);
    test_assert(ecs_has_pair(world, e, Rel, Tgt));

    ecs_fini(world);
}

void Union_defer_add_existing_union_relationship(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Rel, Union);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_entity_t e = ecs_new(world);
    ecs_add_pair(world, e, Rel, TgtA);
    test_assert(ecs_has_pair(world, e, Rel, TgtA));

    ecs_defer_begin(world);
    ecs_add_pair(world, e, Rel, TgtB);
    test_assert(ecs_has_pair(world, e, Rel, TgtA));
    test_assert(!ecs_has_pair(world, e, Rel, TgtB));
    ecs_defer_end(world);
    test_assert(!ecs_has_pair(world, e, Rel, TgtA));
    test_assert(ecs_has_pair(world, e, Rel, TgtB));

    ecs_fini(world);
}

void Union_defer_add_union_relationship_2_ops(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Rel, Union);
    ECS_TAG(world, Tgt);
    ECS_TAG(world, Tag);

    ecs_entity_t e = ecs_new(world);
    ecs_defer_begin(world);
    ecs_add_pair(world, e, Rel, Tgt);
    ecs_add(world, e, Tag);
    test_assert(!ecs_has_pair(world, e, Rel, Tgt));
    test_assert(!ecs_has(world, e, Tag));
    ecs_defer_end(world);
    test_assert(ecs_has_pair(world, e, Rel, Tgt));
    test_assert(ecs_has(world, e, Tag));

    ecs_fini(world);
}

void Union_defer_add_existing_union_relationship_2_ops(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Rel, Union);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);
    ECS_TAG(world, Tag);

    ecs_entity_t e = ecs_new(world);
    ecs_add_pair(world, e, Rel, TgtA);
    test_assert(ecs_has_pair(world, e, Rel, TgtA));

    ecs_defer_begin(world);
    ecs_add_pair(world, e, Rel, TgtB);
    ecs_add(world, e, Tag);
    test_assert(ecs_has_pair(world, e, Rel, TgtA));
    test_assert(!ecs_has_pair(world, e, Rel, TgtB));
    test_assert(!ecs_has(world, e, Tag));
    ecs_defer_end(world);
    test_assert(!ecs_has_pair(world, e, Rel, TgtA));
    test_assert(ecs_has_pair(world, e, Rel, TgtB));
    test_assert(ecs_has(world, e, Tag));

    ecs_fini(world);
}
