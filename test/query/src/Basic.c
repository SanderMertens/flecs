#include <query.h>
#include <stdio.h>

static ecs_query_cache_kind_t cache_kind = EcsQueryCacheDefault;

void Basic_setup(void) {
    const char *cache_param = test_param("cache_kind");
    if (cache_param) {
        if (!strcmp(cache_param, "default")) {
            // already set to default
        } else if (!strcmp(cache_param, "auto")) {
            cache_kind = EcsQueryCacheAuto;
        } else {
            printf("unexpected value for cache_param '%s'\n", cache_param);
        }
    }
}

void Basic_0_query(void) {
    ecs_world_t *world = ecs_mini();

    ecs_query_t *q = ecs_query(world, {
        .expr = "0",
        .cache_kind = cache_kind
    });
    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Basic_1_fact_w_tag(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);

    ecs_entity_t ent = ecs_entity(world, { .name = "ent" });

    ecs_query_t *r = ecs_query(world, {
        .expr = "RelA(ent)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add(world, ent, RelA);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(ent, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void Basic_1_fact_w_component(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t ent = ecs_entity(world, { .name = "ent" });

    ecs_query_t *r = ecs_query(world, {
        .expr = "Position(ent)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_set(world, ent, Position, {10, 20});

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(ent, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        const Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 20);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void Basic_1_fact_w_tag_pair(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, TgtA);

    ecs_entity_t ent = ecs_entity(world, { .name = "ent" });

    ecs_query_t *r = ecs_query(world, {
        .expr = "RelA(ent, TgtA)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_pair(world, ent, RelA, TgtA);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 0));
        test_uint(ent, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void Basic_1_fact_w_component_pair(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, TgtA);

    ecs_entity_t ent = ecs_entity(world, { .name = "ent" });

    ecs_query_t *r = ecs_query(world, {
        .expr = "Position(ent, TgtA)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_set_pair(world, ent, Position, TgtA, {10, 20});

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(ecs_id(Position), TgtA), ecs_field_id(&it, 0));
        test_uint(ent, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void Basic_2_facts_same_src_w_tag(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);

    ecs_entity_t ent = ecs_entity(world, { .name = "ent" });

    ecs_query_t *r = ecs_query(world, {
        .expr = "RelA(ent), RelB(ent)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add(world, ent, RelA);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add(world, ent, RelB);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(ent, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(RelB, ecs_field_id(&it, 1));
        test_uint(ent, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void Basic_2_facts_same_src_w_component(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t ent = ecs_entity(world, { .name = "ent" });

    ecs_query_t *r = ecs_query(world, {
        .expr = "Position(ent), Velocity(ent)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_set(world, ent, Position, {10, 20});

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_set(world, ent, Velocity, {1, 2});

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(ent, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(ecs_id(Velocity), ecs_field_id(&it, 1));
        test_uint(ent, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 1));
        const Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 20);
        const Velocity *v = ecs_field(&it, Velocity, 1);
        test_assert(v != NULL);
        test_int(v->x, 1);
        test_int(v->y, 2);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void Basic_2_facts_same_src_w_tag_pair(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_entity_t ent = ecs_entity(world, { .name = "ent" });

    ecs_query_t *r = ecs_query(world, {
        .expr = "RelA(ent, TgtA), RelA(ent, TgtB)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_pair(world, ent, RelA, TgtA);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_pair(world, ent, RelA, TgtB);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 0));
        test_uint(ent, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(ecs_pair(RelA, TgtB), ecs_field_id(&it, 1));
        test_uint(ent, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void Basic_2_facts_same_src_w_component_pair(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_entity_t ent = ecs_entity(world, { .name = "ent" });

    ecs_query_t *r = ecs_query(world, {
        .expr = "Position(ent, TgtA), Position(ent, TgtB)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_set_pair(world, ent, Position, TgtA, {10, 20});

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_set_pair(world, ent, Position, TgtB, {30, 40});

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(ecs_id(Position), TgtA), ecs_field_id(&it, 0));
        test_uint(ent, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(ecs_pair(ecs_id(Position), TgtB), ecs_field_id(&it, 1));
        test_uint(ent, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void Basic_2_facts_other_src_w_tag(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);

    ecs_entity_t ent1 = ecs_entity(world, { .name = "ent1" });
    ecs_entity_t ent2 = ecs_entity(world, { .name = "ent2" });

    ecs_query_t *r = ecs_query(world, {
        .expr = "RelA(ent1), RelB(ent2)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add(world, ent1, RelA);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add(world, ent2, RelB);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(ent1, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(RelB, ecs_field_id(&it, 1));
        test_uint(ent2, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void Basic_2_facts_other_src_w_component(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t ent1 = ecs_entity(world, { .name = "ent1" });
    ecs_entity_t ent2 = ecs_entity(world, { .name = "ent2" });

    ecs_query_t *r = ecs_query(world, {
        .expr = "Position(ent1), Velocity(ent2)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_set(world, ent1, Position, {10, 20});

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_set(world, ent2, Velocity, {1, 2});

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(ent1, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(ecs_id(Velocity), ecs_field_id(&it, 1));
        test_uint(ent2, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 1));
        const Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 20);
        const Velocity *v = ecs_field(&it, Velocity, 1);
        test_assert(v != NULL);
        test_int(v->x, 1);
        test_int(v->y, 2);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void Basic_2_facts_other_src_w_tag_pair(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_entity_t ent1 = ecs_entity(world, { .name = "ent1" });
    ecs_entity_t ent2 = ecs_entity(world, { .name = "ent2" });

    ecs_query_t *r = ecs_query(world, {
        .expr = "RelA(ent1, TgtA), RelA(ent2, TgtB)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_pair(world, ent1, RelA, TgtA);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_pair(world, ent2, RelA, TgtB);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 0));
        test_uint(ent1, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(ecs_pair(RelA, TgtB), ecs_field_id(&it, 1));
        test_uint(ent2, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void Basic_2_facts_other_src_w_component_pair(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_entity_t ent1 = ecs_entity(world, { .name = "ent1" });
    ecs_entity_t ent2 = ecs_entity(world, { .name = "ent2" });

    ecs_query_t *r = ecs_query(world, {
        .expr = "Position(ent1, TgtA), Position(ent2, TgtB)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_set_pair(world, ent1, Position, TgtA, {10, 20});

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_set_pair(world, ent2, Position, TgtB, {30, 40});

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(ecs_id(Position), TgtA), ecs_field_id(&it, 0));
        test_uint(ent1, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(ecs_pair(ecs_id(Position), TgtB), ecs_field_id(&it, 1));
        test_uint(ent2, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void Basic_1_fact_w_any(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);

    ecs_entity_t ent = ecs_entity(world, { .name = "ent" });

    ecs_query_t *r = ecs_query(world, {
        .expr = "_(ent)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add(world, ent, RelA);
    ecs_add(world, ent, RelB);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(EcsWildcard, ecs_field_id(&it, 0));
        test_uint(ent, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void Basic_1_fact_w_pair_any_tgt(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_entity_t ent = ecs_entity(world, { .name = "ent" });

    ecs_query_t *r = ecs_query(world, {
        .expr = "RelA(ent, _)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_pair(world, ent, RelA, TgtA);
    ecs_add_pair(world, ent, RelA, TgtB);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(RelA, EcsWildcard), ecs_field_id(&it, 0));
        test_uint(ent, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void Basic_1_fact_w_pair_any_rel(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, TgtA);

    ecs_entity_t ent = ecs_entity(world, { .name = "ent" });

    ecs_query_t *r = ecs_query(world, {
        .expr = "_(ent, TgtA)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_pair(world, ent, RelA, TgtA);
    ecs_add_pair(world, ent, RelB, TgtA);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(EcsWildcard, TgtA), ecs_field_id(&it, 0));
        test_uint(ent, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void Basic_1_fact_w_pair_any_rel_tgt(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, TgtA);

    ecs_entity_t ent = ecs_entity(world, { .name = "ent" });
    ecs_add_pair(world, ent, RelA, TgtA);
    ecs_add_pair(world, ent, RelB, TgtA);

    ecs_query_t *r = ecs_query(world, {
        .expr = "_(ent, _)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(EcsWildcard, EcsWildcard), ecs_field_id(&it, 0));
        test_uint(ent, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void Basic_2_facts_same_src_w_any(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);

    ecs_entity_t ent = ecs_entity(world, { .name = "ent" });

    ecs_query_t *r = ecs_query(world, {
        .expr = "_(ent), _(ent)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add(world, ent, RelA);
    ecs_add(world, ent, RelB);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(EcsWildcard, ecs_field_id(&it, 0));
        test_uint(ent, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(EcsWildcard, ecs_field_id(&it, 1));
        test_uint(ent, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void Basic_2_facts_same_src_w_pair_any_tgt(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_entity_t ent = ecs_entity(world, { .name = "ent" });

    ecs_query_t *r = ecs_query(world, {
        .expr = "RelA(ent, _), RelA(ent, _)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_pair(world, ent, RelA, TgtA);
    ecs_add_pair(world, ent, RelA, TgtB);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(RelA, EcsWildcard), ecs_field_id(&it, 0));
        test_uint(ent, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(ecs_pair(RelA, EcsWildcard), ecs_field_id(&it, 1));
        test_uint(ent, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void Basic_2_facts_same_src_w_pair_any_rel(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, TgtA);

    ecs_entity_t ent = ecs_entity(world, { .name = "ent" });

    ecs_query_t *r = ecs_query(world, {
        .expr = "_(ent, TgtA), _(ent, TgtA)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_pair(world, ent, RelA, TgtA);
    ecs_add_pair(world, ent, RelB, TgtA);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(EcsWildcard, TgtA), ecs_field_id(&it, 0));
        test_uint(ent, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void Basic_2_facts_same_src_w_pair_any_rel_tgt(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, TgtA);

    ecs_entity_t ent = ecs_entity(world, { .name = "ent" });
    ecs_add_pair(world, ent, RelA, TgtA);
    ecs_add_pair(world, ent, RelB, TgtA);

    ecs_query_t *r = ecs_query(world, {
        .expr = "_(ent, _), _(ent, _)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(EcsWildcard, EcsWildcard), ecs_field_id(&it, 0));
        test_uint(ent, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(ecs_pair(EcsWildcard, EcsWildcard), ecs_field_id(&it, 1));
        test_uint(ent, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void Basic_2_facts_other_src_w_any(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);

    ecs_entity_t ent1 = ecs_entity(world, { .name = "ent1" });
    ecs_entity_t ent2 = ecs_entity(world, { .name = "ent2" });

    ecs_query_t *r = ecs_query(world, {
        .expr = "_(ent1), _(ent2)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add(world, ent1, RelA);
    ecs_add(world, ent1, RelB);
    ecs_add(world, ent2, RelA);
    ecs_add(world, ent2, RelB);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(EcsWildcard, ecs_field_id(&it, 0));
        test_uint(ent1, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(EcsWildcard, ecs_field_id(&it, 1));
        test_uint(ent2, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void Basic_2_facts_other_src_w_pair_any_tgt(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_entity_t ent1 = ecs_entity(world, { .name = "ent1" });
    ecs_entity_t ent2 = ecs_entity(world, { .name = "ent2" });

    ecs_query_t *r = ecs_query(world, {
        .expr = "RelA(ent1, _), RelA(ent2, _)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_pair(world, ent1, RelA, TgtA);
    ecs_add_pair(world, ent1, RelA, TgtB);
    ecs_add_pair(world, ent2, RelA, TgtA);
    ecs_add_pair(world, ent2, RelA, TgtB);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(RelA, EcsWildcard), ecs_field_id(&it, 0));
        test_uint(ent1, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(ecs_pair(RelA, EcsWildcard), ecs_field_id(&it, 1));
        test_uint(ent2, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void Basic_2_facts_other_src_w_pair_any_rel(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_entity_t ent1 = ecs_entity(world, { .name = "ent1" });
    ecs_entity_t ent2 = ecs_entity(world, { .name = "ent2" });

    ecs_query_t *r = ecs_query(world, {
        .expr = "RelA(ent1, _), RelA(ent2, _)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_pair(world, ent1, RelA, TgtA);
    ecs_add_pair(world, ent1, RelA, TgtB);
    ecs_add_pair(world, ent2, RelA, TgtA);
    ecs_add_pair(world, ent2, RelA, TgtB);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(RelA, EcsWildcard), ecs_field_id(&it, 0));
        test_uint(ent1, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(ecs_pair(RelA, EcsWildcard), ecs_field_id(&it, 1));
        test_uint(ent2, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void Basic_2_facts_other_src_w_pair_any_rel_tgt(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, TgtA);

    ecs_entity_t ent1 = ecs_entity(world, { .name = "ent1" });
    ecs_add_pair(world, ent1, RelA, TgtA);
    ecs_add_pair(world, ent1, RelB, TgtA);
    ecs_entity_t ent2 = ecs_entity(world, { .name = "ent2" });
    ecs_add_pair(world, ent2, RelA, TgtA);
    ecs_add_pair(world, ent2, RelB, TgtA);

    ecs_query_t *r = ecs_query(world, {
        .expr = "_(ent1, _), _(ent2, _)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(EcsWildcard, EcsWildcard), ecs_field_id(&it, 0));
        test_uint(ent1, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(ecs_pair(EcsWildcard, EcsWildcard), ecs_field_id(&it, 1));
        test_uint(ent2, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void Basic_1_this_src_w_tag(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);

    ecs_entity_t e1 = ecs_new(world);
    ecs_entity_t e2 = ecs_new(world);

    ecs_query_t *r = ecs_query(world, {
        .expr = "RelA($this)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add(world, e1, RelA);
    ecs_add(world, e2, RelA);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_uint(2, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(e1, it.entities[0]);
        test_uint(e2, it.entities[1]);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void Basic_1_this_src_w_component(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e1 = ecs_new(world);
    ecs_entity_t e2 = ecs_new(world);

    ecs_query_t *r = ecs_query(world, {
        .expr = "Position($this)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_set(world, e1, Position, {10, 20});
    ecs_set(world, e2, Position, {30, 40});

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_uint(2, it.count);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(e1, it.entities[0]);
        test_uint(e2, it.entities[1]);
        const Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p[0].x, 10); test_int(p[0].y, 20);
        test_int(p[1].x, 30); test_int(p[1].y, 40);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void Basic_1_this_src_w_tag_pair(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, TgtA);

    ecs_entity_t e1 = ecs_new(world);
    ecs_entity_t e2 = ecs_new(world);

    ecs_query_t *r = ecs_query(world, {
        .expr = "RelA($this, TgtA)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_pair(world, e1, RelA, TgtA);
    ecs_add_pair(world, e2, RelA, TgtA);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_uint(2, it.count);
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 0));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(e1, it.entities[0]);
        test_uint(e2, it.entities[1]);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void Basic_1_this_src_w_component_pair(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, TgtA);

    ecs_entity_t e1 = ecs_new(world);
    ecs_entity_t e2 = ecs_new(world);

    ecs_query_t *r = ecs_query(world, {
        .expr = "Position($this, TgtA)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_set_pair(world, e1, Position, TgtA, {10, 20});
    ecs_set_pair(world, e2, Position, TgtA, {30, 40});

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_uint(2, it.count);
        test_uint(ecs_pair(ecs_id(Position), TgtA), ecs_field_id(&it, 0));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(e1, it.entities[0]);
        test_uint(e2, it.entities[1]);
        const Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p[0].x, 10); test_int(p[0].y, 20);
        test_int(p[1].x, 30); test_int(p[1].y, 40);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void Basic_1_this_src_w_tag_2_tables(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, FragmentA);
    ECS_TAG(world, FragmentB);

    ecs_entity_t e1 = ecs_new(world);
    ecs_entity_t e2 = ecs_new(world);

    ecs_add(world, e1, FragmentA);
    ecs_add(world, e2, FragmentB);

    ecs_query_t *r = ecs_query(world, {
        .expr = "RelA($this)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add(world, e1, RelA);
    ecs_add(world, e2, RelA);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(e2, it.entities[0]);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void Basic_1_this_src_w_component_2_tables(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, FragmentA);
    ECS_TAG(world, FragmentB);

    ecs_entity_t e1 = ecs_new(world);
    ecs_entity_t e2 = ecs_new(world);

    ecs_add(world, e1, FragmentA);
    ecs_add(world, e2, FragmentB);

    ecs_query_t *r = ecs_query(world, {
        .expr = "Position($this)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_set(world, e1, Position, {10, 20});
    ecs_set(world, e2, Position, {30, 40});

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(e1, it.entities[0]);
        {
            const Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p[0].x, 10); test_int(p[0].y, 20);
        }

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(e2, it.entities[0]);
        {
            const Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p[0].x, 30); test_int(p[0].y, 40);
        }

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void Basic_1_this_src_w_tag_pair_2_tables(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, FragmentA);
    ECS_TAG(world, FragmentB);

    ecs_entity_t e1 = ecs_new(world);
    ecs_entity_t e2 = ecs_new(world);

    ecs_add(world, e1, FragmentA);
    ecs_add(world, e2, FragmentB);

    ecs_query_t *r = ecs_query(world, {
        .expr = "RelA($this, TgtA)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_pair(world, e1, RelA, TgtA);
    ecs_add_pair(world, e2, RelA, TgtA);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 0));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 0));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(e2, it.entities[0]);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void Basic_1_this_src_w_component_pair_2_tables(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, FragmentA);
    ECS_TAG(world, FragmentB);

    ecs_entity_t e1 = ecs_new(world);
    ecs_entity_t e2 = ecs_new(world);

    ecs_add(world, e1, FragmentA);
    ecs_add(world, e2, FragmentB);

    ecs_query_t *r = ecs_query(world, {
        .expr = "Position($this, TgtA)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_set_pair(world, e1, Position, TgtA, {10, 20});
    ecs_set_pair(world, e2, Position, TgtA, {30, 40});

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(ecs_id(Position), TgtA), ecs_field_id(&it, 0));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(e1, it.entities[0]);
        {
            const Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p[0].x, 10); test_int(p[0].y, 20);
        }

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(ecs_id(Position), TgtA), ecs_field_id(&it, 0));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(e2, it.entities[0]);
        {
            const Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p[0].x, 30); test_int(p[0].y, 40);
        }

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void Basic_2_this_src_w_tag(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);

    ecs_entity_t e1 = ecs_new(world);
    ecs_entity_t e2 = ecs_new(world);

    ecs_query_t *r = ecs_query(world, {
        .expr = "RelA($this), RelB($this)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add(world, e1, RelA);
    ecs_add(world, e2, RelA);
    ecs_add(world, e1, RelB);
    ecs_add(world, e2, RelB);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_uint(2, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(RelB, ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(e1, it.entities[0]);
        test_uint(e2, it.entities[1]);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void Basic_2_this_src_w_component(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t e1 = ecs_new(world);
    ecs_entity_t e2 = ecs_new(world);

    ecs_query_t *r = ecs_query(world, {
        .expr = "Position($this), Velocity($this)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_set(world, e1, Position, {10, 20});
    ecs_set(world, e2, Position, {30, 40});

    ecs_set(world, e1, Velocity, {1, 2});
    ecs_set(world, e2, Velocity, {3, 4});

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_uint(2, it.count);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(ecs_id(Velocity), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(e1, it.entities[0]);
        test_uint(e2, it.entities[1]);
        const Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p[0].x, 10); test_int(p[0].y, 20);
        test_int(p[1].x, 30); test_int(p[1].y, 40);
        const Velocity *v = ecs_field(&it, Velocity, 1);
        test_assert(v != NULL);
        test_int(v[0].x, 1); test_int(v[0].y, 2);
        test_int(v[1].x, 3); test_int(v[1].y, 4);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void Basic_2_this_src_ent_src_w_tag(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);

    ecs_entity_t e1 = ecs_new(world);
    ecs_entity_t e2 = ecs_new(world);
    ecs_entity_t e3 = ecs_entity(world, { .name = "e3" });

    ecs_query_t *r = ecs_query(world, {
        .expr = "RelA($this), RelB(e3)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add(world, e1, RelA);
    ecs_add(world, e2, RelA);
    ecs_add(world, e3, RelB);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_uint(2, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(RelB, ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(e3, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(e1, it.entities[0]);
        test_uint(e2, it.entities[1]);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void Basic_2_this_src_ent_src_w_component(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t e1 = ecs_new(world);
    ecs_entity_t e2 = ecs_new(world);
    ecs_entity_t e3 = ecs_entity(world, { .name = "e3" });

    ecs_query_t *r = ecs_query(world, {
        .expr = "Position($this), Velocity(e3)",
        .cache_kind = cache_kind,
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_set(world, e1, Position, {10, 20});
    ecs_set(world, e2, Position, {30, 40});
    ecs_set(world, e3, Velocity, {1, 2});

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_uint(2, it.count);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(ecs_id(Velocity), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(e3, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(e1, it.entities[0]);
        test_uint(e2, it.entities[1]);
        const Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p[0].x, 10); test_int(p[0].y, 20);
        test_int(p[1].x, 30); test_int(p[1].y, 40);
        const Velocity *v = ecs_field(&it, Velocity, 1);
        test_assert(v != NULL);
        test_int(v[0].x, 1); test_int(v[0].y, 2);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void Basic_2_ent_src_this_src_w_tag(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);

    ecs_entity_t e1 = ecs_new(world);
    ecs_entity_t e2 = ecs_new(world);
    ecs_entity_t e3 = ecs_entity(world, { .name = "e3" });

    ecs_query_t *r = ecs_query(world, {
        .expr = "RelB(e3), RelA($this)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add(world, e1, RelA);
    ecs_add(world, e2, RelA);
    ecs_add(world, e3, RelB);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_uint(2, it.count);
        test_uint(RelB, ecs_field_id(&it, 0));
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(e3, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(e1, it.entities[0]);
        test_uint(e2, it.entities[1]);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void Basic_2_ent_src_this_src_w_component(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t e1 = ecs_new(world);
    ecs_entity_t e2 = ecs_new(world);
    ecs_entity_t e3 = ecs_entity(world, { .name = "e3" });

    ecs_query_t *r = ecs_query(world, {
        .expr = "Velocity(e3), Position($this)",
        .cache_kind = cache_kind,
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_set(world, e1, Position, {10, 20});
    ecs_set(world, e2, Position, {30, 40});
    ecs_set(world, e3, Velocity, {1, 2});

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_uint(2, it.count);
        test_uint(ecs_id(Velocity), ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_uint(e3, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(e1, it.entities[0]);
        test_uint(e2, it.entities[1]);
        const Velocity *v = ecs_field(&it, Velocity, 0);
        test_assert(v != NULL);
        test_int(v[0].x, 1); test_int(v[0].y, 2);
        const Position *p = ecs_field(&it, Position, 1);
        test_assert(p != NULL);
        test_int(p[0].x, 10); test_int(p[0].y, 20);
        test_int(p[1].x, 30); test_int(p[1].y, 40);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void Basic_recycled_tag(void) {
    ecs_world_t *world = ecs_mini();

    ecs_delete(world, ecs_new(world));
    ECS_TAG(world, RelA);
    test_assert((uint32_t)RelA != RelA);

    ecs_entity_t ent = ecs_entity(world, { .name = "ent" });

    ecs_query_t *r = ecs_query(world, {
        .expr = "RelA(ent)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add(world, ent, RelA);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(ent, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void Basic_recycled_src(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);

    ecs_delete(world, ecs_new(world));
    ecs_entity_t ent = ecs_entity(world, { .name = "ent" });
    test_assert((uint32_t)ent != ent);

    ecs_query_t *r = ecs_query(world, {
        .expr = "RelA(ent)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add(world, ent, RelA);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(ent, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void Basic_recycled_pair_rel(void) {
    ecs_world_t *world = ecs_mini();

    ecs_delete(world, ecs_new(world));
    ECS_TAG(world, RelA);
    test_assert((uint32_t)RelA != RelA);
    ECS_TAG(world, TgtA);

    ecs_entity_t ent = ecs_entity(world, { .name = "ent" });

    ecs_query_t *r = ecs_query(world, {
        .expr = "RelA(ent, TgtA)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_pair(world, ent, RelA, TgtA);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 0));
        test_uint(ent, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void Basic_recycled_pair_tgt(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ecs_delete(world, ecs_new(world));
    ECS_TAG(world, TgtA);
    test_assert((uint32_t)TgtA != TgtA);

    ecs_entity_t ent = ecs_entity(world, { .name = "ent" });

    ecs_query_t *r = ecs_query(world, {
        .expr = "RelA(ent, TgtA)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_pair(world, ent, RelA, TgtA);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 0));
        test_uint(ent, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void Basic_this_src_w_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, RelC);

    ecs_entity_t e1 = ecs_new(world);
    ecs_entity_t e2 = ecs_new(world);
    ecs_entity_t e3 = ecs_new(world);

    ecs_query_t *r = ecs_query(world, {
        .expr = "RelA($this), *($this)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add(world, e1, RelA);
    ecs_add(world, e1, RelB);
    
    ecs_add(world, e2, RelA);
    ecs_add(world, e2, RelB);
    ecs_add(world, e2, RelC);

    ecs_add(world, e3, RelA);
    ecs_add(world, e3, RelB);
    ecs_add(world, e3, RelC);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(RelB, ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(2, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(e2, it.entities[0]);
        test_uint(e3, it.entities[1]);

        test_bool(true, ecs_query_next(&it));
        test_uint(2, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(RelB, ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(e2, it.entities[0]);
        test_uint(e3, it.entities[1]);

        test_bool(true, ecs_query_next(&it));
        test_uint(2, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(RelC, ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(e2, it.entities[0]);
        test_uint(e3, it.entities[1]);

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void Basic_this_src_w_pair_rel_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, RelC);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_entity_t e1 = ecs_new(world);
    ecs_entity_t e2 = ecs_new(world);
    ecs_entity_t e3 = ecs_new(world);

    ecs_query_t *r = ecs_query(world, {
        .expr = "RelA($this, TgtA), *($this, TgtA)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_pair(world, e1, RelA, TgtA);
    ecs_add_pair(world, e1, RelB, TgtA);
    ecs_add_pair(world, e1, RelB, TgtB);
    
    ecs_add_pair(world, e2, RelA, TgtA);
    ecs_add_pair(world, e2, RelB, TgtA);
    ecs_add_pair(world, e2, RelC, TgtA);
    ecs_add_pair(world, e2, RelC, TgtB);

    ecs_add_pair(world, e3, RelA, TgtA);
    ecs_add_pair(world, e3, RelB, TgtA);
    ecs_add_pair(world, e3, RelC, TgtA);
    ecs_add_pair(world, e3, RelC, TgtB);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelB, TgtA), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(2, it.count);
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(e2, it.entities[0]);
        test_uint(e3, it.entities[1]);

        test_bool(true, ecs_query_next(&it));
        test_uint(2, it.count);
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelB, TgtA), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(e2, it.entities[0]);
        test_uint(e3, it.entities[1]);

        test_bool(true, ecs_query_next(&it));
        test_uint(2, it.count);
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelC, TgtA), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(e2, it.entities[0]);
        test_uint(e3, it.entities[1]);

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void Basic_this_src_w_pair_tgt_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);
    ECS_TAG(world, TgtC);

    ecs_entity_t e1 = ecs_new(world);
    ecs_entity_t e2 = ecs_new(world);
    ecs_entity_t e3 = ecs_new(world);

    ecs_query_t *r = ecs_query(world, {
        .expr = "RelA($this, TgtA), RelA($this, *)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_pair(world, e1, RelA, TgtA);
    ecs_add_pair(world, e1, RelA, TgtB);
    ecs_add_pair(world, e1, RelB, TgtA);
    
    ecs_add_pair(world, e2, RelA, TgtA);
    ecs_add_pair(world, e2, RelA, TgtB);
    ecs_add_pair(world, e2, RelA, TgtC);
    ecs_add_pair(world, e2, RelB, TgtA);

    ecs_add_pair(world, e3, RelA, TgtA);
    ecs_add_pair(world, e3, RelA, TgtB);
    ecs_add_pair(world, e3, RelA, TgtC);
    ecs_add_pair(world, e3, RelB, TgtA);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelA, TgtB), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(2, it.count);
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(e2, it.entities[0]);
        test_uint(e3, it.entities[1]);

        test_bool(true, ecs_query_next(&it));
        test_uint(2, it.count);
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelA, TgtB), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(e2, it.entities[0]);
        test_uint(e3, it.entities[1]);

        test_bool(true, ecs_query_next(&it));
        test_uint(2, it.count);
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelA, TgtC), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(e2, it.entities[0]);
        test_uint(e3, it.entities[1]);

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void Basic_this_src_w_pair_rel_tgt_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_entity_t e1 = ecs_new(world);
    ecs_entity_t e2 = ecs_new(world);
    ecs_entity_t e3 = ecs_new(world);

    ecs_query_t *r = ecs_query(world, {
        .expr = "RelA($this, TgtA), *($this, *)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_pair(world, e1, RelA, TgtA);
    ecs_add_pair(world, e1, RelB, TgtA);
    
    ecs_add_pair(world, e2, RelA, TgtA);
    ecs_add_pair(world, e2, RelA, TgtB);
    ecs_add_pair(world, e2, RelB, TgtA);

    ecs_add_pair(world, e3, RelA, TgtA);
    ecs_add_pair(world, e3, RelA, TgtB);
    ecs_add_pair(world, e3, RelB, TgtA);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelB, TgtA), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(2, it.count);
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(e2, it.entities[0]);
        test_uint(e3, it.entities[1]);

        test_bool(true, ecs_query_next(&it));
        test_uint(2, it.count);
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelA, TgtB), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(e2, it.entities[0]);
        test_uint(e3, it.entities[1]);

        test_bool(true, ecs_query_next(&it));
        test_uint(2, it.count);
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelB, TgtA), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(e2, it.entities[0]);
        test_uint(e3, it.entities[1]);

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void Basic_this_src_w_any(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_TAG(world, Tgt);

    ecs_query_t *r = ecs_query(world, {
        .expr = "_",
        .cache_kind = cache_kind
    });

    ecs_entity_t e1 = ecs_new_w_pair(world, Rel, Tgt);

    ecs_iter_t it = ecs_query_iter(world, r);
    ecs_entity_t prev = 0;
    int32_t count = 0, e1_matched = 0;
    while (ecs_query_next(&it)) {
        test_assert(it.count > 0);
        test_assert(!prev || prev != it.entities[0]);
        prev = it.entities[0];
        if (it.entities[0] == e1) {
            e1_matched ++;
        }
        count ++;
    }

    test_assert(count > 0);
    test_int(e1_matched, 1);

    ecs_query_fini(r);

    ecs_fini(world);
}

void Basic_not_any(void) {
    ecs_world_t *world = ecs_mini();

    ecs_query_t *r = ecs_query(world, {
        .expr = "!_",
        .cache_kind = cache_kind
    });

    ecs_iter_t it = ecs_query_iter(world, r);
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(r);

    ecs_fini(world);
}

void Basic_this_src_w_any_written(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, RelC);

    ecs_entity_t e1 = ecs_new(world);
    ecs_entity_t e2 = ecs_new(world);
    ecs_entity_t e3 = ecs_new(world);

    ecs_query_t *r = ecs_query(world, {
        .expr = "RelA($this), _($this)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add(world, e1, RelA);
    ecs_add(world, e1, RelB);
    
    ecs_add(world, e2, RelA);
    ecs_add(world, e2, RelB);
    ecs_add(world, e2, RelC);

    ecs_add(world, e3, RelA);
    ecs_add(world, e3, RelB);
    ecs_add(world, e3, RelC);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(EcsWildcard, ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(2, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(EcsWildcard, ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(e2, it.entities[0]);
        test_uint(e3, it.entities[1]);

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void Basic_this_src_w_pair_rel_any(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, RelC);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_entity_t e1 = ecs_new(world);
    ecs_entity_t e2 = ecs_new(world);
    ecs_entity_t e3 = ecs_new(world);

    ecs_query_t *r = ecs_query(world, {
        .expr = "RelA($this, TgtA), _($this, TgtA)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_pair(world, e1, RelA, TgtA);
    ecs_add_pair(world, e1, RelB, TgtA);
    ecs_add_pair(world, e1, RelB, TgtB);
    
    ecs_add_pair(world, e2, RelA, TgtA);
    ecs_add_pair(world, e2, RelB, TgtA);
    ecs_add_pair(world, e2, RelC, TgtA);
    ecs_add_pair(world, e2, RelC, TgtB);

    ecs_add_pair(world, e3, RelA, TgtA);
    ecs_add_pair(world, e3, RelB, TgtA);
    ecs_add_pair(world, e3, RelC, TgtA);
    ecs_add_pair(world, e3, RelC, TgtB);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 0));
        test_uint(ecs_pair(EcsWildcard, TgtA), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(2, it.count);
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 0));
        test_uint(ecs_pair(EcsWildcard, TgtA), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(e2, it.entities[0]);
        test_uint(e3, it.entities[1]);

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void Basic_this_src_w_pair_tgt_any(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);
    ECS_TAG(world, TgtC);

    ecs_entity_t e1 = ecs_new(world);
    ecs_entity_t e2 = ecs_new(world);
    ecs_entity_t e3 = ecs_new(world);

    ecs_query_t *r = ecs_query(world, {
        .expr = "RelA($this, TgtA), RelA($this, _)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_pair(world, e1, RelA, TgtA);
    ecs_add_pair(world, e1, RelA, TgtB);
    ecs_add_pair(world, e1, RelB, TgtA);
    
    ecs_add_pair(world, e2, RelA, TgtA);
    ecs_add_pair(world, e2, RelA, TgtB);
    ecs_add_pair(world, e2, RelA, TgtC);
    ecs_add_pair(world, e2, RelB, TgtA);

    ecs_add_pair(world, e3, RelA, TgtA);
    ecs_add_pair(world, e3, RelA, TgtB);
    ecs_add_pair(world, e3, RelA, TgtC);
    ecs_add_pair(world, e3, RelB, TgtA);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelA, EcsWildcard), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(2, it.count);
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelA, EcsWildcard), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(e2, it.entities[0]);
        test_uint(e3, it.entities[1]);

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void Basic_this_src_w_pair_rel_tgt_any(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_entity_t e1 = ecs_new(world);
    ecs_entity_t e2 = ecs_new(world);
    ecs_entity_t e3 = ecs_new(world);

    ecs_query_t *r = ecs_query(world, {
        .expr = "RelA($this, TgtA), _($this, _)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_pair(world, e1, RelA, TgtA);
    ecs_add_pair(world, e1, RelB, TgtA);
    
    ecs_add_pair(world, e2, RelA, TgtA);
    ecs_add_pair(world, e2, RelA, TgtB);
    ecs_add_pair(world, e2, RelB, TgtA);

    ecs_add_pair(world, e3, RelA, TgtA);
    ecs_add_pair(world, e3, RelA, TgtB);
    ecs_add_pair(world, e3, RelB, TgtA);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 0));
        test_uint(ecs_pair(EcsWildcard, EcsWildcard), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(2, it.count);
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 0));
        test_uint(ecs_pair(EcsWildcard, EcsWildcard), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(e2, it.entities[0]);
        test_uint(e3, it.entities[1]);

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void Basic_ent_src_w_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, RelC);

    ecs_entity_t e1 = ecs_entity(world, { .name = "ent" });

    ecs_query_t *r = ecs_query(world, {
        .expr = "*(ent)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add(world, e1, RelA);
    ecs_add(world, e1, RelB);
    ecs_add(world, e1, RelC);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(e1, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(RelB, ecs_field_id(&it, 0));
        test_uint(e1, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(RelC, ecs_field_id(&it, 0));
        test_uint(e1, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void Basic_ent_src_w_pair_rel_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, RelC);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_entity_t e1 = ecs_entity(world, { .name = "ent" });

    ecs_query_t *r = ecs_query(world, {
        .expr = "*(ent, TgtA)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_pair(world, e1, RelA, TgtA);
    ecs_add_pair(world, e1, RelB, TgtA);
    ecs_add_pair(world, e1, RelC, TgtA);
    ecs_add_pair(world, e1, RelA, TgtB);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 0));
        test_uint(e1, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(RelB, TgtA), ecs_field_id(&it, 0));
        test_uint(e1, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(RelC, TgtA), ecs_field_id(&it, 0));
        test_uint(e1, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void Basic_ent_src_w_pair_tgt_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);
    ECS_TAG(world, TgtC);

    ecs_entity_t e1 = ecs_entity(world, { .name = "ent" });

    ecs_query_t *r = ecs_query(world, {
        .expr = "RelA(ent, *)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_pair(world, e1, RelA, TgtA);
    ecs_add_pair(world, e1, RelA, TgtB);
    ecs_add_pair(world, e1, RelA, TgtC);
    ecs_add_pair(world, e1, RelB, TgtA);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 0));
        test_uint(e1, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(RelA, TgtB), ecs_field_id(&it, 0));
        test_uint(e1, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(RelA, TgtC), ecs_field_id(&it, 0));
        test_uint(e1, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void Basic_ent_src_w_pair_rel_tgt_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, RelC);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_entity_t e1 = ecs_entity(world, { .name = "ent" });
    ecs_add_pair(world, e1, RelA, TgtA);
    ecs_add_pair(world, e1, RelA, TgtB);
    ecs_add_pair(world, e1, RelB, TgtA);

    ecs_query_t *r = ecs_query(world, {
        .expr = "*(ent, *)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(ecs_id(EcsIdentifier), EcsName), ecs_field_id(&it, 0));
        test_uint(e1, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 0));
        test_uint(e1, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(RelA, TgtB), ecs_field_id(&it, 0));
        test_uint(e1, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(RelB, TgtA), ecs_field_id(&it, 0));
        test_uint(e1, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void Basic_1_wildcard_src(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, Tag);

    ecs_query_t *r = ecs_query(world, {
        .expr = "RelA(*)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_entity_t e1 = ecs_new_w(world, RelA);
    ecs_add_id(world, e1, Tag);
    ecs_new_w(world, RelA);
    ecs_new_w(world, RelA);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void Basic_1_wildcard_src_w_pair(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, Tag);
    ECS_TAG(world, TgtA);

    ecs_query_t *r = ecs_query(world, {
        .expr = "RelA(*, TgtA)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_entity_t e1 = ecs_new_w_pair(world, RelA, TgtA);
    ecs_add(world, e1, Tag);
    ecs_new_w_pair(world, RelA, TgtA);
    ecs_new_w_pair(world, RelA, TgtA);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void Basic_2_wildcard_src(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, Tag);

    ecs_query_t *r = ecs_query(world, {
        .expr = "RelA(*), RelB(*)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_entity_t e1 = ecs_new_w(world, RelA);
    ecs_add_id(world, e1, Tag);
    ecs_entity_t e2 = ecs_new_w(world, RelA);
    ecs_entity_t e3 = ecs_new_w(world, RelA);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add(world, e1, RelB);
    ecs_add(world, e2, RelB);
    ecs_add(world, e3, RelB);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(RelB, ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(RelB, ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(RelB, ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(RelB, ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void Basic_2_wildcard_src_w_pair(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, Tag);
    ECS_TAG(world, TgtA);

    ecs_query_t *r = ecs_query(world, {
        .expr = "RelA(*, TgtA), RelB(*, TgtA)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_entity_t e1 = ecs_new_w_pair(world, RelA, TgtA);
    ecs_add(world, e1, Tag);
    ecs_entity_t e2 = ecs_new_w_pair(world, RelA, TgtA);
    ecs_entity_t e3 = ecs_new_w_pair(world, RelA, TgtA);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_pair(world, e1, RelB, TgtA);
    ecs_add_pair(world, e2, RelB, TgtA);
    ecs_add_pair(world, e3, RelB, TgtA);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelB, TgtA), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelB, TgtA), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
    
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelB, TgtA), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelB, TgtA), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void Basic_rule_w_iter_next(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);

    ecs_entity_t ent = ecs_entity(world, { .name = "ent" });

    ecs_query_t *r = ecs_query(world, {
        .expr = "RelA(ent)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add(world, ent, RelA);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(ent, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void Basic_1_wildcard_src_w_pair_tgt_var(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);
    ECS_TAG(world, TgtC);

    ecs_query_t *r = ecs_query(world, {
        .expr = "Rel(*, $x)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    int x_var = ecs_query_find_var(r, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_new_w_pair(world, Rel, TgtA);
    ecs_new_w_pair(world, Rel, TgtB);
    ecs_new_w_pair(world, Rel, TgtC);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(Rel, TgtA), ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(TgtA, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(Rel, TgtB), ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(TgtB, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(Rel, TgtC), ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(TgtC, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void Basic_1_wildcard_src_w_pair_rel_var(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, RelC);
    ECS_TAG(world, Tgt);

    ecs_query_t *r = ecs_query(world, {
        .expr = "$x(*, Tgt)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    int x_var = ecs_query_find_var(r, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_new_w_pair(world, RelA, Tgt);
    ecs_new_w_pair(world, RelB, Tgt);
    ecs_new_w_pair(world, RelC, Tgt);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(RelA, Tgt), ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(RelA, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(RelB, Tgt), ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(RelB, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(RelC, Tgt), ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(RelC, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void Basic_1_wildcard_src_w_pair_tgt_this(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);
    ECS_TAG(world, TgtC);

    ecs_query_t *r = ecs_query(world, {
        .expr = "Rel(*, $this)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    int this_var = ecs_query_find_var(r, "this");
    test_assert(this_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_new_w_pair(world, Rel, TgtA);
    ecs_new_w_pair(world, Rel, TgtB);
    ecs_new_w_pair(world, Rel, TgtC);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(Rel, TgtA), ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(TgtA, ecs_iter_get_var(&it, this_var));
        test_uint(TgtA, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(Rel, TgtB), ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(TgtB, ecs_iter_get_var(&it, this_var));
        test_uint(TgtB, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(Rel, TgtC), ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(TgtC, ecs_iter_get_var(&it, this_var));
        test_uint(TgtC, it.entities[0]);

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void Basic_1_wildcard_src_w_pair_rel_this(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, RelC);
    ECS_TAG(world, Tgt);

    ecs_query_t *r = ecs_query(world, {
        .expr = "$this(*, Tgt)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    int this_var = ecs_query_find_var(r, "this");
    test_assert(this_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_new_w_pair(world, RelA, Tgt);
    ecs_new_w_pair(world, RelB, Tgt);
    ecs_new_w_pair(world, RelC, Tgt);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(RelA, Tgt), ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(RelA, ecs_iter_get_var(&it, this_var));
        test_uint(RelA, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(RelB, Tgt), ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(RelB, ecs_iter_get_var(&it, this_var));
        test_uint(RelB, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(RelC, Tgt), ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(RelC, ecs_iter_get_var(&it, this_var));
        test_uint(RelC, it.entities[0]);

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void Basic_1_any_src(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, Tag);

    ecs_query_t *r = ecs_query(world, {
        .expr = "RelA(_)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_entity_t e1 = ecs_new_w(world, RelA);
    ecs_add_id(world, e1, Tag);
    ecs_new_w(world, RelA);
    ecs_new_w(world, RelA);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(EcsWildcard, ecs_field_src(&it, 0));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void Basic_1_any_src_component(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Tag);

    ecs_query_t *r = ecs_query(world, {
        .expr = "Position(_)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_entity_t e1 = ecs_new_w(world, Position);
    ecs_add_id(world, e1, Tag);
    ecs_new_w(world, Position);
    ecs_new_w(world, Position);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(EcsWildcard, ecs_field_src(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_assert(NULL == ecs_field(&it, Position, 0));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void Basic_1_any_src_component_w_this_component(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TAG(world, Tag);

    ecs_query_t *r = ecs_query(world, {
        .expr = "Position(_), Velocity",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    ecs_entity_t e = ecs_insert(world, ecs_value(Velocity, {10, 20}));

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_entity_t e1 = ecs_new_w(world, Position);
    ecs_add_id(world, e1, Tag);
    ecs_new_w(world, Position);
    ecs_new_w(world, Position);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(e, it.entities[0]);
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(EcsWildcard, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(ecs_id(Velocity), ecs_field_id(&it, 1));
        test_assert(NULL == ecs_field(&it, Position, 0));
        {
            Velocity *v = ecs_field(&it, Velocity, 1);
            test_assert(v != NULL);
            test_int(v->x, 10);
            test_int(v->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void Basic_1_any_src_w_pair(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, Tag);
    ECS_TAG(world, TgtA);

    ecs_query_t *r = ecs_query(world, {
        .expr = "RelA(_, TgtA)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_entity_t e1 = ecs_new_w_pair(world, RelA, TgtA);
    ecs_add(world, e1, Tag);
    ecs_new_w_pair(world, RelA, TgtA);
    ecs_new_w_pair(world, RelA, TgtA);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 0));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void Basic_2_any_src(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, Tag);

    ecs_query_t *r = ecs_query(world, {
        .expr = "RelA(_), RelB(_)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_entity_t e1 = ecs_new_w(world, RelA);
    ecs_add_id(world, e1, Tag);
    ecs_entity_t e2 = ecs_new_w(world, RelA);
    ecs_entity_t e3 = ecs_new_w(world, RelA);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add(world, e1, RelB);
    ecs_add(world, e2, RelB);
    ecs_add(world, e3, RelB);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void Basic_2_any_src_w_pair(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, Tag);
    ECS_TAG(world, TgtA);

    ecs_query_t *r = ecs_query(world, {
        .expr = "RelA(_, TgtA), RelB(_, TgtA)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_entity_t e1 = ecs_new_w_pair(world, RelA, TgtA);
    ecs_add(world, e1, Tag);
    ecs_entity_t e2 = ecs_new_w_pair(world, RelA, TgtA);
    ecs_entity_t e3 = ecs_new_w_pair(world, RelA, TgtA);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_pair(world, e1, RelB, TgtA);
    ecs_add_pair(world, e2, RelB, TgtA);
    ecs_add_pair(world, e3, RelB, TgtA);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 0));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void Basic_1_any_src_w_pair_tgt_var(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);
    ECS_TAG(world, TgtC);

    ecs_query_t *r = ecs_query(world, {
        .expr = "Rel(_, $x)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    int x_var = ecs_query_find_var(r, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_new_w_pair(world, Rel, TgtA);
    ecs_new_w_pair(world, Rel, TgtB);
    ecs_new_w_pair(world, Rel, TgtC);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(Rel, TgtC), ecs_field_id(&it, 0));
        test_uint(TgtC, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(Rel, TgtB), ecs_field_id(&it, 0));
        test_uint(TgtB, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(Rel, TgtA), ecs_field_id(&it, 0));
        test_uint(TgtA, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void Basic_1_any_src_w_pair_rel_var(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, RelC);
    ECS_TAG(world, Tgt);

    ecs_query_t *r = ecs_query(world, {
        .expr = "$x(_, Tgt)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    int x_var = ecs_query_find_var(r, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_new_w_pair(world, RelA, Tgt);
    ecs_new_w_pair(world, RelB, Tgt);
    ecs_new_w_pair(world, RelC, Tgt);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(RelC, Tgt), ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(RelC, ecs_iter_get_var(&it, x_var));
        test_uint(EcsWildcard, ecs_field_src(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(RelB, Tgt), ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(RelB, ecs_iter_get_var(&it, x_var));
        test_uint(EcsWildcard, ecs_field_src(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(RelA, Tgt), ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(RelA, ecs_iter_get_var(&it, x_var));
        test_uint(EcsWildcard, ecs_field_src(&it, 0));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void Basic_1_any_src_w_pair_tgt_this(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);
    ECS_TAG(world, TgtC);

    ecs_query_t *r = ecs_query(world, {
        .expr = "Rel(_, $this)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    int this_var = ecs_query_find_var(r, "this");
    test_assert(this_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_new_w_pair(world, Rel, TgtA);
    ecs_new_w_pair(world, Rel, TgtB);
    ecs_new_w_pair(world, Rel, TgtC);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(Rel, TgtC), ecs_field_id(&it, 0));
        test_uint(TgtC, ecs_iter_get_var(&it, this_var));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(EcsWildcard, ecs_field_src(&it, 0));
        test_uint(TgtC, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(Rel, TgtB), ecs_field_id(&it, 0));
        test_uint(TgtB, ecs_iter_get_var(&it, this_var));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(EcsWildcard, ecs_field_src(&it, 0));
        test_uint(TgtB, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(Rel, TgtA), ecs_field_id(&it, 0));
        test_uint(TgtA, ecs_iter_get_var(&it, this_var));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(EcsWildcard, ecs_field_src(&it, 0));
        test_uint(TgtA, it.entities[0]);

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void Basic_1_any_src_w_pair_rel_this(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, RelC);
    ECS_TAG(world, Tgt);

    ecs_query_t *r = ecs_query(world, {
        .expr = "$this(_, Tgt)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    int this_var = ecs_query_find_var(r, "this");
    test_assert(this_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_new_w_pair(world, RelA, Tgt);
    ecs_new_w_pair(world, RelB, Tgt);
    ecs_new_w_pair(world, RelC, Tgt);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(RelC, Tgt), ecs_field_id(&it, 0));
        test_uint(RelC, ecs_iter_get_var(&it, this_var));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(EcsWildcard, ecs_field_src(&it, 0));
        test_uint(RelC, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(RelB, Tgt), ecs_field_id(&it, 0));
        test_uint(RelB, ecs_iter_get_var(&it, this_var));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(EcsWildcard, ecs_field_src(&it, 0));
        test_uint(RelB, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(RelA, Tgt), ecs_field_id(&it, 0));
        test_uint(RelA, ecs_iter_get_var(&it, this_var));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(EcsWildcard, ecs_field_src(&it, 0));
        test_uint(RelA, it.entities[0]);

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void Basic_1_any_src_any_tgt_w_rel_fixed(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_query_t *r = ecs_query(world, {
        .expr = "RelA(_, _)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_new_w_pair(world, TgtA, RelA);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_new_w_pair(world, RelA, TgtA);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(RelA, EcsWildcard), ecs_field_id(&it, 0));
        test_uint(EcsWildcard, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_new_w_pair(world, RelA, TgtB);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(RelA, EcsWildcard), ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void Basic_1_any_src_any_rel_w_tgt_fixed(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, TgtA);

    ecs_query_t *r = ecs_query(world, {
        .expr = "_(_, TgtA)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_new_w_pair(world, TgtA, RelA);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_new_w_pair(world, RelA, TgtA);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(EcsWildcard, TgtA), ecs_field_id(&it, 0));
        test_uint(EcsWildcard, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_new_w_pair(world, RelB, TgtA);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(EcsWildcard, TgtA), ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void Basic_1_any_src_w_childof_pair_any_tgt(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_query_t *r = ecs_query(world, {
        .expr = "ChildOf(_, _)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(EcsChildOf, EcsWildcard), ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void Basic_1_any_src_empty(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);

    ecs_query_t *r = ecs_query(world, {
        .expr = "RelA(_)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_entity_t e1 = ecs_new_w(world, RelA);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(EcsWildcard, ecs_field_src(&it, 0));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_delete(world, e1);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void Basic_1_any_src_pair_tgt_wildcard_empty(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);
    ECS_TAG(world, TgtC);

    ecs_query_t *r = ecs_query(world, {
        .expr = "RelA(_, *)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_entity_t e1 = ecs_new_w_pair(world, RelA, TgtA);
    ecs_entity_t e2 = ecs_new_w_pair(world, RelA, TgtB);
    ecs_entity_t e3 = ecs_new_w_pair(world, RelA, TgtC);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(RelA, TgtC), ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(EcsWildcard, ecs_field_src(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(RelA, TgtB), ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(EcsWildcard, ecs_field_src(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(EcsWildcard, ecs_field_src(&it, 0));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_delete(world, e2);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(RelA, TgtC), ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(EcsWildcard, ecs_field_src(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(EcsWildcard, ecs_field_src(&it, 0));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_delete(world, e1);
    ecs_delete(world, e3);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void Basic_1_any_src_pair_rel_wildcard_empty(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, RelC);
    ECS_TAG(world, TgtA);

    ecs_query_t *r = ecs_query(world, {
        .expr = "*(_, TgtA)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_entity_t e1 = ecs_new_w_pair(world, RelA, TgtA);
    ecs_entity_t e2 = ecs_new_w_pair(world, RelB, TgtA);
    ecs_entity_t e3 = ecs_new_w_pair(world, RelC, TgtA);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(RelC, TgtA), ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(EcsWildcard, ecs_field_src(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(RelB, TgtA), ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(EcsWildcard, ecs_field_src(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(EcsWildcard, ecs_field_src(&it, 0));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_delete(world, e2);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(RelC, TgtA), ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(EcsWildcard, ecs_field_src(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(EcsWildcard, ecs_field_src(&it, 0));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_delete(world, e1);
    ecs_delete(world, e3);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void Basic_1_any_src_any_tgt_w_rel_fixed_empty(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_query_t *r = ecs_query(world, {
        .expr = "RelA(_, _)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_new_w_pair(world, TgtA, RelA);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_entity_t e1 = ecs_new_w_pair(world, RelA, TgtA);
    ecs_entity_t e2 = ecs_new_w_pair(world, RelA, TgtB);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(RelA, EcsWildcard), ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_delete(world, e1);
    ecs_delete(world, e2);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void Basic_1_any_src_any_rel_w_tgt_fixed_empty(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, TgtA);

    ecs_query_t *r = ecs_query(world, {
        .expr = "_(_, TgtA)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_new_w_pair(world, TgtA, RelA);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_entity_t e1 = ecs_new_w_pair(world, RelA, TgtA);
    ecs_entity_t e2 = ecs_new_w_pair(world, RelB, TgtA);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(EcsWildcard, TgtA), ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_delete(world, e1);
    ecs_delete(world, e2);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void Basic_1_any_src_pair_w_tag_query_empty(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_query_t *r = ecs_query(world, {
        .expr = "RelA(_)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_new_w_pair(world, TgtA, RelA);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_entity_t e1 = ecs_new_w_pair(world, RelA, TgtA);
    ecs_entity_t e2 = ecs_new_w_pair(world, RelA, TgtB);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_delete(world, e1);
    ecs_delete(world, e2);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void Basic_1_any_src_tag_w_pair_query_empty(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);

    ecs_query_t *r = ecs_query(world, {
        .expr = "RelA(_, _)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_entity_t e1 = ecs_new_w(world, RelA);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_delete(world, e1);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void Basic_empty_rule(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);

    ecs_query_t *r = ecs_query(world, {
        .expr = "",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void Basic_invalid_rule(void) {
    ecs_log_set_level(-4);

    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);

    ecs_query_t *r = ecs_query(world, {
        .expr = "Foo",
        .cache_kind = cache_kind
    });

    test_assert(r == NULL);

    ecs_fini(world);
}

void Basic_instanced_table_src(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_query_t *r = ecs_query(world, {
        .expr = "Position($this)",
        .cache_kind = cache_kind,
    });

    test_assert(r != NULL);

    ecs_entity_t e1 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(Position, {20, 30}));
    ecs_entity_t e3 = ecs_insert(world, ecs_value(Position, {30, 40}));

    ecs_iter_t it = ecs_query_iter(world, r);
    {
        test_bool(true, ecs_query_next(&it));
        test_int(3, it.count);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(e1, it.entities[0]);
        test_uint(e2, it.entities[1]);
        test_uint(e3, it.entities[2]);

        const Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p[0].x, 10); test_int(p[0].y, 20);
        test_int(p[1].x, 20); test_int(p[1].y, 30);
        test_int(p[2].x, 30); test_int(p[2].y, 40);
    }

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(r);

    ecs_fini(world);
}

void Basic_instanced_entity_src(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e1 = ecs_entity(world, { .name = "e1" });

    ecs_query_t *r = ecs_query(world, {
        .expr = "Position(e1)",
        .cache_kind = cache_kind,
    });

    test_assert(r != NULL);

    ecs_set(world, e1, Position, {10, 20});

    ecs_iter_t it = ecs_query_iter(world, r);
    {
        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(e1, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));

        const Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 20);
    }

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(r);

    ecs_fini(world);
}

void Basic_instanced_mixed_src(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t e0 = ecs_entity(world, { .name = "e0" });

    ecs_query_t *r = ecs_query(world, {
        .expr = "Position($this), Velocity(e0)",
        .cache_kind = cache_kind,
    });

    test_assert(r != NULL);

    ecs_entity_t e1 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(Position, {20, 30}));
    ecs_entity_t e3 = ecs_insert(world, ecs_value(Position, {30, 40}));

    ecs_set(world, e0, Velocity, {1, 2});

    ecs_iter_t it = ecs_query_iter(world, r);
    {
        test_bool(true, ecs_query_next(&it));
        test_int(3, it.count);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(e1, it.entities[0]);
        test_uint(e2, it.entities[1]);
        test_uint(e3, it.entities[2]);

        const Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p[0].x, 10); test_int(p[0].y, 20);
        test_int(p[1].x, 20); test_int(p[1].y, 30);
        test_int(p[2].x, 30); test_int(p[2].y, 40);

        const Velocity *v = ecs_field(&it, Velocity, 1);
        test_int(v->x, 1);
        test_int(v->y, 2);
    }

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(r);

    ecs_fini(world);
}

void Basic_instanced_inherited(void) {
    ecs_world_t *world = ecs_mini();
    
    ECS_TAG(world, Tag);

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t base = ecs_insert(world, ecs_value(Velocity, {1, 2}));

    ecs_entity_t e1 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(Position, {20, 30}));
    ecs_entity_t e3 = ecs_insert(world, ecs_value(Position, {30, 40}));

    ecs_entity_t e4 = ecs_insert(world, ecs_value(Position, {40, 50}));
    ecs_entity_t e5 = ecs_insert(world, ecs_value(Position, {50, 60}));
    ecs_add(world, e4, Tag);
    ecs_add(world, e5, Tag);

    ecs_add_pair(world, e1, EcsIsA, base);
    ecs_add_pair(world, e2, EcsIsA, base);
    ecs_add_pair(world, e3, EcsIsA, base);
    ecs_add_pair(world, e4, EcsIsA, base);
    ecs_add_pair(world, e5, EcsIsA, base);

    ecs_entity_t e6 = ecs_insert(world, ecs_value(Position, {60, 70}));
    ecs_entity_t e7 = ecs_insert(world, ecs_value(Position, {70, 80}));
    ecs_set(world, e6, Velocity, {2, 3});
    ecs_set(world, e7, Velocity, {4, 5});

    ecs_query_t *q = ecs_query(world, {
        .expr = "Position, Velocity",
        .cache_kind = cache_kind,
    });
    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);

    test_assert(ecs_query_next(&it));
    {
        Position *p = ecs_field(&it, Position, 0);
        Velocity *v = ecs_field(&it, Velocity, 1);
        test_int(it.count, 3);
        test_int(it.entities[0], e1);
        test_int(it.entities[1], e2);
        test_int(it.entities[2], e3);
        test_int(p[0].x, 10);
        test_int(p[0].y, 20);
        test_int(p[1].x, 20);
        test_int(p[1].y, 30);
        test_int(p[2].x, 30);
        test_int(p[2].y, 40);
        test_int(v[0].x, 1);
        test_int(v[0].y, 2);
    }

    test_assert(ecs_query_next(&it));
    {
        Position *p = ecs_field(&it, Position, 0);
        Velocity *v = ecs_field(&it, Velocity, 1);
        test_int(it.count, 2);
        test_int(it.entities[0], e4);
        test_int(it.entities[1], e5);
        test_int(p[0].x, 40);
        test_int(p[0].y, 50);
        test_int(p[1].x, 50);
        test_int(p[1].y, 60);
        test_int(v->x, 1);
        test_int(v->y, 2);
    }

    test_assert(ecs_query_next(&it));
    {
        Position *p = ecs_field(&it, Position, 0);
        Velocity *v = ecs_field(&it, Velocity, 1);
        test_int(it.count, 2);
        test_int(it.entities[0], e6);
        test_int(p[0].x, 60);
        test_int(p[0].y, 70);
        test_int(v[0].x, 2);
        test_int(v[0].y, 3);

        test_int(it.entities[1], e7);
        test_int(p[1].x, 70);
        test_int(p[1].y, 80);
        test_int(v[1].x, 4);
        test_int(v[1].y, 5);
    }

    test_assert(!ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Basic_in_term(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t ent = ecs_entity(world, { .name = "ent" });

    ecs_query_t *r = ecs_query(world, {
        .expr = "[in] Position(ent)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_set(world, ent, Position, {10, 20});

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(ent, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_readonly(&it, 0));
        const Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 20);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void Basic_out_term(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t ent = ecs_entity(world, { .name = "ent" });

    ecs_query_t *r = ecs_query(world, {
        .expr = "[out] Position(ent)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_set(world, ent, Position, {10, 20});

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(ent, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_field_is_readonly(&it, 0));
        const Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 20);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void Basic_inout_term(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t ent = ecs_entity(world, { .name = "ent" });

    ecs_query_t *r = ecs_query(world, {
        .expr = "[inout] Position(ent)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_set(world, ent, Position, {10, 20});

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(ent, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_field_is_readonly(&it, 0));
        const Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 20);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void Basic_nodata_term(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t ent = ecs_entity(world, { .name = "ent" });

    ecs_query_t *r = ecs_query(world, {
        .expr = "[none] Position(ent)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_set(world, ent, Position, {10, 20});

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(ent, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_field_is_readonly(&it, 0));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void Basic_find_this_lowercase(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);

    ecs_query_t *r = ecs_query(world, {
        .expr = "Rel($this)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    int this_var = ecs_query_find_var(r, "this");
    test_assert(this_var != -1);
    test_assert(this_var == 0);

    ecs_query_fini(r);

    ecs_fini(world);
}

void Basic_find_this_uppercase(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);

    ecs_query_t *r = ecs_query(world, {
        .expr = "Rel($this)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    int this_var = ecs_query_find_var(r, "this");
    test_assert(this_var != -1);
    test_assert(this_var == 0);

    ecs_query_fini(r);

    ecs_fini(world);
}

void Basic_find_this_tgt_lowercase(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);

    ecs_query_t *r = ecs_query(world, {
        .expr = "Rel(*, $this)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    int this_var = ecs_query_find_var(r, "this");
    test_assert(this_var != -1);
    test_assert(this_var != 0);

    ecs_query_fini(r);

    ecs_fini(world);
}

void Basic_find_this_tgt_uppercase(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);

    ecs_query_t *r = ecs_query(world, {
        .expr = "Rel(*, $this)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    int this_var = ecs_query_find_var(r, "this");
    test_assert(this_var != -1);
    test_assert(this_var != 0);

    ecs_query_fini(r);

    ecs_fini(world);
}

void Basic_get_filter(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);

    ecs_query_t *r = ecs_query(world, {
        .expr = "Rel",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    const ecs_query_t *f = r;
    test_assert(f != NULL);
    test_int(f->term_count, 1);
    test_uint(f->terms[0].id, Rel);

    ecs_query_fini(r);

    ecs_fini(world);
}

void Basic_iter_empty_source(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_query_t *r = ecs_query(world, {
        .expr = "Foo()",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_id(Foo), ecs_field_id(&it, 0));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void Basic_iter_empty_source_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_query_t *r = ecs_query(world, {
        .expr = "*()",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(EcsWildcard, ecs_field_id(&it, 0));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void Basic_iter_empty_source_2_terms(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_query_t *r = ecs_query(world, {
        .expr = "Foo(), Bar()",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_id(Foo), ecs_field_id(&it, 0));
        test_uint(ecs_id(Bar), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void Basic_iter_empty_source_pair(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Tgt);

    ecs_query_t *r = ecs_query(world, {
        .expr = "Foo(#0, Tgt)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(Foo, Tgt), ecs_field_id(&it, 0));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void Basic_iter_empty_source_pair_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Tgt);

    ecs_query_t *r = ecs_query(world, {
        .expr = "*(#0, Tgt)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(EcsWildcard, Tgt), ecs_field_id(&it, 0));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void Basic_iter_empty_source_2_terms_pair(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);
    ECS_TAG(world, Tgt);

    ecs_query_t *r = ecs_query(world, {
        .expr = "Foo(#0, Tgt), Bar(#0, Tgt)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(Foo, Tgt), ecs_field_id(&it, 0));
        test_uint(ecs_pair(Bar, Tgt), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void Basic_iter_empty_source_2_terms_mixed(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);
    ECS_TAG(world, Tgt);

    ecs_query_t *r = ecs_query(world, {
        .expr = "Foo, Bar()",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    ecs_entity_t e = ecs_new(world);
    ecs_add(world, e, Foo);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(e, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(Bar, ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void Basic_iter_empty_source_2_terms_mixed_pair(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);
    ECS_TAG(world, Tgt);

    ecs_query_t *r = ecs_query(world, {
        .expr = "(Foo, Tgt), Bar(#0, Tgt)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    ecs_entity_t e = ecs_new(world);
    ecs_add_pair(world, e, Foo, Tgt);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(e, it.entities[0]);
        test_uint(ecs_pair(Foo, Tgt), ecs_field_id(&it, 0));
        test_uint(ecs_pair(Bar, Tgt), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void Basic_iter_empty_source_2_terms_mixed_pair_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);
    ECS_TAG(world, Tgt);

    ecs_query_t *r = ecs_query(world, {
        .expr = "(Foo, Tgt), *(#0, Tgt)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    ecs_entity_t e = ecs_new(world);
    ecs_add_pair(world, e, Foo, Tgt);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(e, it.entities[0]);
        test_uint(ecs_pair(Foo, Tgt), ecs_field_id(&it, 0));
        test_uint(ecs_pair(EcsWildcard, Tgt), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void Basic_this_var_w_empty_entity(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);

    ecs_entity_t t = ecs_new(world);
    ecs_entity_t e = ecs_new_w_pair(world, Rel, t);

    ecs_query_t *r = ecs_query(world, { 
        .expr = "Rel($x, $this)",
        .cache_kind = cache_kind
    });
    test_assert(r != NULL);

    int x_var = ecs_query_find_var(r, "x");
    test_assert(x_var != -1);

    ecs_iter_t it = ecs_query_iter(world, r);
    test_bool(true, ecs_query_next(&it));
    test_uint(1, it.count);
    test_uint(t, it.entities[0]);
    test_uint(ecs_pair(Rel, t), ecs_field_id(&it, 0));
    test_uint(e, ecs_field_src(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_uint(e, ecs_iter_get_var(&it, x_var));
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(r);

    ecs_fini(world);
}

void Basic_match_disabled(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);

    ecs_entity_t e_1 = ecs_new_w(world, TagA);
    ecs_entity_t e_2 = ecs_new_w(world, TagA);
    ecs_add_id(world, e_2, EcsDisabled);
    ecs_entity_t e_3 = ecs_new_w(world, TagA);
    ecs_add_id(world, e_3, EcsPrefab);

    ecs_query_t *r_1 = ecs_query(world, {
        .expr = "TagA",
        .cache_kind = cache_kind
    });

    ecs_query_t *r_2 = ecs_query(world, {
        .expr = "TagA, Disabled",
        .cache_kind = cache_kind
    });

    test_assert(r_1 != NULL);
    test_assert(r_2 != NULL);

    test_bool(r_1->flags & EcsQueryMatchDisabled, false);
    test_bool(r_2->flags & EcsQueryMatchDisabled, true);

    ecs_iter_t it = ecs_query_iter(world, r_1);
    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e_1);
    test_int(ecs_field_id(&it, 0), TagA);
    test_bool(true, ecs_field_is_set(&it, 0));
    test_assert(!ecs_query_next(&it));

    it = ecs_query_iter(world, r_2);

    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e_2);
    test_int(ecs_field_id(&it, 0), TagA);
    test_bool(true, ecs_field_is_set(&it, 0));

    test_assert(!ecs_query_next(&it));

    ecs_query_fini(r_1);
    ecs_query_fini(r_2);

    ecs_fini(world);
}

void Basic_match_prefab(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);

    ecs_entity_t e_1 = ecs_new_w(world, TagA);
    ecs_entity_t e_2 = ecs_new_w(world, TagA);
    ecs_add_id(world, e_2, EcsDisabled);
    ecs_entity_t e_3 = ecs_new_w(world, TagA);
    ecs_add_id(world, e_3, EcsPrefab);

    ecs_query_t *r_1 = ecs_query(world, {
        .expr = "TagA",
        .cache_kind = cache_kind
    });

    ecs_query_t *r_2 = ecs_query(world, {
        .expr = "TagA, Prefab",
        .cache_kind = cache_kind
    });

    test_assert(r_1 != NULL);
    test_assert(r_2 != NULL);

    test_bool(r_1->flags & EcsQueryMatchPrefab, false);
    test_bool(r_2->flags & EcsQueryMatchPrefab, true);

    ecs_iter_t it = ecs_query_iter(world, r_1);
    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e_1);
    test_int(ecs_field_id(&it, 0), TagA);
    test_bool(true, ecs_field_is_set(&it, 0));
    test_assert(!ecs_query_next(&it));

    it = ecs_query_iter(world, r_2);

    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e_3);
    test_int(ecs_field_id(&it, 0), TagA);
    test_bool(true, ecs_field_is_set(&it, 0));

    test_assert(!ecs_query_next(&it));

    ecs_query_fini(r_1);
    ecs_query_fini(r_2);

    ecs_fini(world);
}

void Basic_match_disabled_prefab(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);

    ecs_entity_t e_1 = ecs_new_w(world, TagA);
    ecs_entity_t e_2 = ecs_new_w(world, TagA);
    ecs_add_id(world, e_2, EcsDisabled);
    ecs_entity_t e_3 = ecs_new_w(world, TagA);
    ecs_add_id(world, e_3, EcsPrefab);
    ecs_entity_t e_4 = ecs_new_w(world, TagA);
    ecs_add_id(world, e_4, EcsPrefab);
    ecs_add_id(world, e_4, EcsDisabled);

    ecs_query_t *r_1 = ecs_query(world, {
        .expr = "TagA",
        .cache_kind = cache_kind
    });

    ecs_query_t *r_2 = ecs_query(world, {
        .expr = "TagA, Disabled",
        .cache_kind = cache_kind
    });

    ecs_query_t *r_3 = ecs_query(world, {
        .expr = "TagA, Prefab",
        .cache_kind = cache_kind
    });

    ecs_query_t *r_4 = ecs_query(world, {
        .expr = "TagA, Prefab, Disabled",
        .cache_kind = cache_kind
    });

    test_assert(r_1 != NULL);
    test_assert(r_2 != NULL);
    test_assert(r_3 != NULL);
    test_assert(r_4 != NULL);

    test_bool(r_1->flags & EcsQueryMatchDisabled, false);
    test_bool(r_2->flags & EcsQueryMatchDisabled, true);
    test_bool(r_3->flags & EcsQueryMatchDisabled, false);
    test_bool(r_4->flags & EcsQueryMatchDisabled, true);

    test_bool(r_1->flags & EcsQueryMatchPrefab, false);
    test_bool(r_2->flags & EcsQueryMatchPrefab, false);
    test_bool(r_3->flags & EcsQueryMatchPrefab, true);
    test_bool(r_4->flags & EcsQueryMatchPrefab, true);

    ecs_iter_t it = ecs_query_iter(world, r_1);
    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e_1);
    test_int(ecs_field_id(&it, 0), TagA);
    test_assert(!ecs_query_next(&it));

    {
        it = ecs_query_iter(world, r_2);

        test_assert(ecs_query_next(&it));
        test_int(it.count, 1);
        test_int(it.entities[0], e_2);
        test_int(ecs_field_id(&it, 0), TagA);
        test_bool(true, ecs_field_is_set(&it, 0));
        test_assert(!ecs_query_next(&it));
    }

    {
        it = ecs_query_iter(world, r_3);

        test_assert(ecs_query_next(&it));
        test_int(it.count, 1);
        test_int(it.entities[0], e_3);
        test_int(ecs_field_id(&it, 0), TagA);
        test_bool(true, ecs_field_is_set(&it, 0));
        test_assert(!ecs_query_next(&it));
    }

    {
        it = ecs_query_iter(world, r_4);

        test_assert(ecs_query_next(&it));
        test_int(it.count, 1);
        test_int(it.entities[0], e_4);
        test_int(ecs_field_id(&it, 0), TagA);
        test_bool(true, ecs_field_is_set(&it, 0));

        test_assert(!ecs_query_next(&it));
    }

    ecs_query_fini(r_1);
    ecs_query_fini(r_2);
    ecs_query_fini(r_3);
    ecs_query_fini(r_4);

    ecs_fini(world);
}

void Basic_match_disabled_this_tgt(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);

    ecs_entity_t e_1 = ecs_new_w(world, TagA);
    ecs_entity_t e_2 = ecs_new_w(world, TagA);
    ecs_add_id(world, e_2, EcsDisabled);
    ecs_entity_t e_3 = ecs_new_w(world, TagA);
    ecs_add_id(world, e_3, EcsPrefab);

    ecs_new_w_pair(world, EcsChildOf, e_1);
    ecs_new_w_pair(world, EcsChildOf, e_2);
    ecs_new_w_pair(world, EcsChildOf, e_3);

    ecs_query_t *r_1 = ecs_query(world, {
        .expr = "TagA, ChildOf($child, $this)",
        .cache_kind = cache_kind
    });

    ecs_query_t *r_2 = ecs_query(world, {
        .expr = "TagA, ChildOf($child, $this), Disabled",
        .cache_kind = cache_kind
    });

    test_assert(r_1 != NULL);
    test_assert(r_2 != NULL);

    test_bool(r_1->flags & EcsQueryMatchDisabled, false);
    test_bool(r_2->flags & EcsQueryMatchDisabled, true);

    ecs_iter_t it = ecs_query_iter(world, r_1);
    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e_1);
    test_int(ecs_field_id(&it, 0), TagA);
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 1));
    test_assert(!ecs_query_next(&it));

    it = ecs_query_iter(world, r_2);

    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e_2);
    test_int(ecs_field_id(&it, 0), TagA);
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 1));

    test_assert(!ecs_query_next(&it));

    ecs_query_fini(r_1);
    ecs_query_fini(r_2);

    ecs_fini(world);
}

void Basic_match_prefab_this_tgt(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);

    ecs_entity_t e_1 = ecs_new_w(world, TagA);
    ecs_entity_t e_2 = ecs_new_w(world, TagA);
    ecs_add_id(world, e_2, EcsDisabled);
    ecs_entity_t e_3 = ecs_new_w(world, TagA);
    ecs_add_id(world, e_3, EcsPrefab);

    ecs_new_w_pair(world, EcsChildOf, e_1);
    ecs_new_w_pair(world, EcsChildOf, e_2);
    ecs_new_w_pair(world, EcsChildOf, e_3);

    ecs_query_t *r_1 = ecs_query(world, {
        .expr = "TagA, ChildOf($child, $this)",
        .cache_kind = cache_kind
    });

    ecs_query_t *r_2 = ecs_query(world, {
        .expr = "TagA, ChildOf($child, $this), Prefab",
        .cache_kind = cache_kind
    });

    test_assert(r_1 != NULL);
    test_assert(r_2 != NULL);

    test_bool(r_1->flags & EcsQueryMatchPrefab, false);
    test_bool(r_2->flags & EcsQueryMatchPrefab, true);

    ecs_iter_t it = ecs_query_iter(world, r_1);
    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e_1);
    test_int(ecs_field_id(&it, 0), TagA);
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 1));
    test_assert(!ecs_query_next(&it));

    it = ecs_query_iter(world, r_2);

    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e_3);
    test_int(ecs_field_id(&it, 0), TagA);
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 1));
    test_assert(!ecs_query_next(&it));

    ecs_query_fini(r_1);
    ecs_query_fini(r_2);

    ecs_fini(world);
}

void Basic_match_disabled_prefab_this_tgt(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);

    ecs_entity_t e_1 = ecs_new_w(world, TagA);
    ecs_entity_t e_2 = ecs_new_w(world, TagA);
    ecs_add_id(world, e_2, EcsDisabled);
    ecs_entity_t e_3 = ecs_new_w(world, TagA);
    ecs_add_id(world, e_3, EcsPrefab);
    ecs_entity_t e_4 = ecs_new_w(world, TagA);
    ecs_add_id(world, e_4, EcsPrefab);
    ecs_add_id(world, e_4, EcsDisabled);

    ecs_new_w_pair(world, EcsChildOf, e_1);
    ecs_new_w_pair(world, EcsChildOf, e_2);
    ecs_new_w_pair(world, EcsChildOf, e_3);
    ecs_new_w_pair(world, EcsChildOf, e_4);

    ecs_query_t *r_1 = ecs_query(world, {
        .expr = "TagA, ChildOf($child, $this)",
        .cache_kind = cache_kind
    });

    ecs_query_t *r_2 = ecs_query(world, {
        .expr = "TagA, ChildOf($child, $this), Disabled",
        .cache_kind = cache_kind
    });

    ecs_query_t *r_3 = ecs_query(world, {
        .expr = "TagA, ChildOf($child, $this), Prefab",
        .cache_kind = cache_kind
    });

    ecs_query_t *r_4 = ecs_query(world, {
        .expr = "TagA, ChildOf($child, $this), Prefab, Disabled",
        .cache_kind = cache_kind
    });

    test_assert(r_1 != NULL);
    test_assert(r_2 != NULL);
    test_assert(r_3 != NULL);
    test_assert(r_4 != NULL);

    test_bool(r_1->flags & EcsQueryMatchDisabled, false);
    test_bool(r_2->flags & EcsQueryMatchDisabled, true);
    test_bool(r_3->flags & EcsQueryMatchDisabled, false);
    test_bool(r_4->flags & EcsQueryMatchDisabled, true);

    test_bool(r_1->flags & EcsQueryMatchPrefab, false);
    test_bool(r_2->flags & EcsQueryMatchPrefab, false);
    test_bool(r_3->flags & EcsQueryMatchPrefab, true);
    test_bool(r_4->flags & EcsQueryMatchPrefab, true);

    ecs_iter_t it = ecs_query_iter(world, r_1);
    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e_1);
    test_int(ecs_field_id(&it, 0), TagA);
    test_assert(!ecs_query_next(&it));

    {
        it = ecs_query_iter(world, r_2);

        test_assert(ecs_query_next(&it));
        test_int(it.count, 1);
        test_int(it.entities[0], e_2);
        test_int(ecs_field_id(&it, 0), TagA);
        test_bool(true, ecs_field_is_set(&it, 0));
        test_assert(!ecs_query_next(&it));
    }

    {
        it = ecs_query_iter(world, r_3);

        test_assert(ecs_query_next(&it));
        test_int(it.count, 1);
        test_int(it.entities[0], e_3);
        test_int(ecs_field_id(&it, 0), TagA);
        test_bool(true, ecs_field_is_set(&it, 0));
        test_assert(!ecs_query_next(&it));
    }

    {
        it = ecs_query_iter(world, r_4);

        test_assert(ecs_query_next(&it));
        test_int(it.count, 1);
        test_int(it.entities[0], e_4);
        test_int(ecs_field_id(&it, 0), TagA);
        test_bool(true, ecs_field_is_set(&it, 0));

        test_assert(!ecs_query_next(&it));
    }

    ecs_query_fini(r_1);
    ecs_query_fini(r_2);
    ecs_query_fini(r_3);
    ecs_query_fini(r_4);

    ecs_fini(world);
}

void Basic_match_self_disabled(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);

    ecs_entity_t e_1 = ecs_new_w(world, TagA);
    ecs_entity_t e_2 = ecs_new_w(world, TagA);
    ecs_add_id(world, e_2, EcsDisabled);
    ecs_entity_t e_3 = ecs_new_w(world, TagA);
    ecs_add_id(world, e_3, EcsPrefab);

    ecs_query_t *r_1 = ecs_query(world, {
        .expr = "TagA(self)",
        .cache_kind = cache_kind
    });

    ecs_query_t *r_2 = ecs_query(world, {
        .expr = "TagA(self), Disabled(self)",
        .cache_kind = cache_kind
    });

    test_assert(r_1 != NULL);
    test_assert(r_2 != NULL);

    test_bool(r_1->flags & EcsQueryMatchDisabled, false);
    test_bool(r_2->flags & EcsQueryMatchDisabled, true);

    ecs_iter_t it = ecs_query_iter(world, r_1);
    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e_1);
    test_int(ecs_field_id(&it, 0), TagA);
    test_bool(true, ecs_field_is_set(&it, 0));
    test_assert(!ecs_query_next(&it));

    it = ecs_query_iter(world, r_2);

    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e_2);
    test_int(ecs_field_id(&it, 0), TagA);
    test_bool(true, ecs_field_is_set(&it, 0));

    test_assert(!ecs_query_next(&it));

    ecs_query_fini(r_1);
    ecs_query_fini(r_2);

    ecs_fini(world);
}

void Basic_match_self_prefab(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);

    ecs_entity_t e_1 = ecs_new_w(world, TagA);
    ecs_entity_t e_2 = ecs_new_w(world, TagA);
    ecs_add_id(world, e_2, EcsDisabled);
    ecs_entity_t e_3 = ecs_new_w(world, TagA);
    ecs_add_id(world, e_3, EcsPrefab);

    ecs_query_t *r_1 = ecs_query(world, {
        .expr = "TagA(self)",
        .cache_kind = cache_kind
    });

    ecs_query_t *r_2 = ecs_query(world, {
        .expr = "TagA(self), Prefab(self)",
        .cache_kind = cache_kind
    });

    test_assert(r_1 != NULL);
    test_assert(r_2 != NULL);

    test_bool(r_1->flags & EcsQueryMatchPrefab, false);
    test_bool(r_2->flags & EcsQueryMatchPrefab, true);

    ecs_iter_t it = ecs_query_iter(world, r_1);
    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e_1);
    test_int(ecs_field_id(&it, 0), TagA);
    test_bool(true, ecs_field_is_set(&it, 0));
    test_assert(!ecs_query_next(&it));

    it = ecs_query_iter(world, r_2);

    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e_3);
    test_int(ecs_field_id(&it, 0), TagA);
    test_bool(true, ecs_field_is_set(&it, 0));

    test_assert(!ecs_query_next(&it));

    ecs_query_fini(r_1);
    ecs_query_fini(r_2);

    ecs_fini(world);
}

void Basic_match_self_disabled_prefab(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);

    ecs_entity_t e_1 = ecs_new_w(world, TagA);
    ecs_entity_t e_2 = ecs_new_w(world, TagA);
    ecs_add_id(world, e_2, EcsDisabled);
    ecs_entity_t e_3 = ecs_new_w(world, TagA);
    ecs_add_id(world, e_3, EcsPrefab);
    ecs_entity_t e_4 = ecs_new_w(world, TagA);
    ecs_add_id(world, e_4, EcsPrefab);
    ecs_add_id(world, e_4, EcsDisabled);

    ecs_query_t *r_1 = ecs_query(world, {
        .expr = "TagA(self)",
        .cache_kind = cache_kind
    });

    ecs_query_t *r_2 = ecs_query(world, {
        .expr = "TagA(self), Disabled(self)",
        .cache_kind = cache_kind
    });

    ecs_query_t *r_3 = ecs_query(world, {
        .expr = "TagA(self), Prefab(self)",
        .cache_kind = cache_kind
    });

    ecs_query_t *r_4 = ecs_query(world, {
        .expr = "TagA(self), Prefab(self), Disabled(self)",
        .cache_kind = cache_kind
    });

    test_assert(r_1 != NULL);
    test_assert(r_2 != NULL);
    test_assert(r_3 != NULL);
    test_assert(r_4 != NULL);

    test_bool(r_1->flags & EcsQueryMatchDisabled, false);
    test_bool(r_2->flags & EcsQueryMatchDisabled, true);
    test_bool(r_3->flags & EcsQueryMatchDisabled, false);
    test_bool(r_4->flags & EcsQueryMatchDisabled, true);

    test_bool(r_1->flags & EcsQueryMatchPrefab, false);
    test_bool(r_2->flags & EcsQueryMatchPrefab, false);
    test_bool(r_3->flags & EcsQueryMatchPrefab, true);
    test_bool(r_4->flags & EcsQueryMatchPrefab, true);

    ecs_iter_t it = ecs_query_iter(world, r_1);
    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e_1);
    test_int(ecs_field_id(&it, 0), TagA);
    test_assert(!ecs_query_next(&it));

    {
        it = ecs_query_iter(world, r_2);
        test_assert(ecs_query_next(&it));
        test_int(it.count, 1);
        test_int(it.entities[0], e_2);
        test_int(ecs_field_id(&it, 0), TagA);
        test_bool(true, ecs_field_is_set(&it, 0));
        test_assert(!ecs_query_next(&it));
    }

    {
        it = ecs_query_iter(world, r_3);
        test_assert(ecs_query_next(&it));
        test_int(it.count, 1);
        test_int(it.entities[0], e_3);
        test_int(ecs_field_id(&it, 0), TagA);
        test_bool(true, ecs_field_is_set(&it, 0));
        test_assert(!ecs_query_next(&it));
    }

    {
        it = ecs_query_iter(world, r_4);
        test_assert(ecs_query_next(&it));
        test_int(it.count, 1);
        test_int(it.entities[0], e_4);
        test_int(ecs_field_id(&it, 0), TagA);
        test_bool(true, ecs_field_is_set(&it, 0));

        test_assert(!ecs_query_next(&it));
    }

    ecs_query_fini(r_1);
    ecs_query_fini(r_2);
    ecs_query_fini(r_3);
    ecs_query_fini(r_4);

    ecs_fini(world);
}

void Basic_match_optional_disabled(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);

    ecs_entity_t e_1 = ecs_new_w(world, TagA);
    ecs_entity_t e_2 = ecs_new_w(world, TagA);
    ecs_add_id(world, e_2, EcsDisabled);
    ecs_entity_t e_3 = ecs_new_w(world, TagA);
    ecs_add_id(world, e_3, EcsPrefab);

    ecs_query_t *r_1 = ecs_query(world, {
        .expr = "TagA",
        .cache_kind = cache_kind
    });

    ecs_query_t *r_2 = ecs_query(world, {
        .expr = "TagA, ?Disabled",
        .cache_kind = cache_kind
    });

    test_assert(r_1 != NULL);
    test_assert(r_2 != NULL);

    test_bool(r_1->flags & EcsQueryMatchDisabled, false);
    test_bool(r_2->flags & EcsQueryMatchDisabled, true);

    ecs_iter_t it = ecs_query_iter(world, r_1);
    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e_1);
    test_int(ecs_field_id(&it, 0), TagA);
    test_bool(true, ecs_field_is_set(&it, 0));
    test_assert(!ecs_query_next(&it));

    it = ecs_query_iter(world, r_2);

    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e_1);
    test_int(ecs_field_id(&it, 0), TagA);
    test_bool(true, ecs_field_is_set(&it, 0));

    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e_2);
    test_int(ecs_field_id(&it, 0), TagA);
    test_bool(true, ecs_field_is_set(&it, 0));

    test_assert(!ecs_query_next(&it));

    ecs_query_fini(r_1);
    ecs_query_fini(r_2);

    ecs_fini(world);
}

void Basic_match_optional_prefab(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);

    ecs_entity_t e_1 = ecs_new_w(world, TagA);
    ecs_entity_t e_2 = ecs_new_w(world, TagA);
    ecs_add_id(world, e_2, EcsDisabled);
    ecs_entity_t e_3 = ecs_new_w(world, TagA);
    ecs_add_id(world, e_3, EcsPrefab);

    ecs_query_t *r_1 = ecs_query(world, {
        .expr = "TagA",
        .cache_kind = cache_kind
    });

    ecs_query_t *r_2 = ecs_query(world, {
        .expr = "TagA, ?Prefab",
        .cache_kind = cache_kind
    });

    test_assert(r_1 != NULL);
    test_assert(r_2 != NULL);

    test_bool(r_1->flags & EcsQueryMatchPrefab, false);
    test_bool(r_2->flags & EcsQueryMatchPrefab, true);

    ecs_iter_t it = ecs_query_iter(world, r_1);
    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e_1);
    test_int(ecs_field_id(&it, 0), TagA);
    test_bool(true, ecs_field_is_set(&it, 0));
    test_assert(!ecs_query_next(&it));

    it = ecs_query_iter(world, r_2);

    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e_1);
    test_int(ecs_field_id(&it, 0), TagA);
    test_bool(true, ecs_field_is_set(&it, 0));

    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e_3);
    test_int(ecs_field_id(&it, 0), TagA);
    test_bool(true, ecs_field_is_set(&it, 0));

    test_assert(!ecs_query_next(&it));

    ecs_query_fini(r_1);
    ecs_query_fini(r_2);

    ecs_fini(world);
}

void Basic_match_optional_disabled_prefab(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);

    ecs_entity_t e_1 = ecs_new_w(world, TagA);
    ecs_entity_t e_2 = ecs_new_w(world, TagA);
    ecs_add_id(world, e_2, EcsDisabled);
    ecs_entity_t e_3 = ecs_new_w(world, TagA);
    ecs_add_id(world, e_3, EcsPrefab);
    ecs_entity_t e_4 = ecs_new_w(world, TagA);
    ecs_add_id(world, e_4, EcsPrefab);
    ecs_add_id(world, e_4, EcsDisabled);

    ecs_query_t *r_1 = ecs_query(world, {
        .expr = "TagA",
        .cache_kind = cache_kind
    });

    ecs_query_t *r_2 = ecs_query(world, {
        .expr = "TagA, ?Disabled",
        .cache_kind = cache_kind
    });

    ecs_query_t *r_3 = ecs_query(world, {
        .expr = "TagA, ?Prefab",
        .cache_kind = cache_kind
    });

    ecs_query_t *r_4 = ecs_query(world, {
        .expr = "TagA, ?Prefab, ?Disabled",
        .cache_kind = cache_kind
    });

    test_assert(r_1 != NULL);
    test_assert(r_2 != NULL);
    test_assert(r_3 != NULL);
    test_assert(r_4 != NULL);

    test_bool(r_1->flags & EcsQueryMatchDisabled, false);
    test_bool(r_2->flags & EcsQueryMatchDisabled, true);
    test_bool(r_3->flags & EcsQueryMatchDisabled, false);
    test_bool(r_4->flags & EcsQueryMatchDisabled, true);

    test_bool(r_1->flags & EcsQueryMatchPrefab, false);
    test_bool(r_2->flags & EcsQueryMatchPrefab, false);
    test_bool(r_3->flags & EcsQueryMatchPrefab, true);
    test_bool(r_4->flags & EcsQueryMatchPrefab, true);

    ecs_iter_t it = ecs_query_iter(world, r_1);
    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e_1);
    test_int(ecs_field_id(&it, 0), TagA);
    test_assert(!ecs_query_next(&it));

    {
        it = ecs_query_iter(world, r_2);
        test_assert(ecs_query_next(&it));
        test_int(it.count, 1);
        test_int(it.entities[0], e_1);
        test_int(ecs_field_id(&it, 0), TagA);
        test_bool(true, ecs_field_is_set(&it, 0));

        test_assert(ecs_query_next(&it));
        test_int(it.count, 1);
        test_int(it.entities[0], e_2);
        test_int(ecs_field_id(&it, 0), TagA);
        test_bool(true, ecs_field_is_set(&it, 0));
        test_assert(!ecs_query_next(&it));
    }

    {
        it = ecs_query_iter(world, r_3);
        test_assert(ecs_query_next(&it));
        test_int(it.count, 1);
        test_int(it.entities[0], e_1);
        test_int(ecs_field_id(&it, 0), TagA);
        test_bool(true, ecs_field_is_set(&it, 0));

        test_assert(ecs_query_next(&it));
        test_int(it.count, 1);
        test_int(it.entities[0], e_3);
        test_int(ecs_field_id(&it, 0), TagA);
        test_bool(true, ecs_field_is_set(&it, 0));
        test_assert(!ecs_query_next(&it));
    }

    {
        it = ecs_query_iter(world, r_4);
        test_assert(ecs_query_next(&it));
        test_int(it.count, 1);
        test_int(it.entities[0], e_1);
        test_int(ecs_field_id(&it, 0), TagA);
        test_bool(true, ecs_field_is_set(&it, 0));

        test_assert(ecs_query_next(&it));
        test_int(it.count, 1);
        test_int(it.entities[0], e_2);
        test_int(ecs_field_id(&it, 0), TagA);
        test_bool(true, ecs_field_is_set(&it, 0));

        test_assert(ecs_query_next(&it));
        test_int(it.count, 1);
        test_int(it.entities[0], e_3);
        test_int(ecs_field_id(&it, 0), TagA);
        test_bool(true, ecs_field_is_set(&it, 0));

        test_assert(ecs_query_next(&it));
        test_int(it.count, 1);
        test_int(it.entities[0], e_4);
        test_int(ecs_field_id(&it, 0), TagA);
        test_bool(true, ecs_field_is_set(&it, 0));

        test_assert(!ecs_query_next(&it));
    }

    ecs_query_fini(r_1);
    ecs_query_fini(r_2);
    ecs_query_fini(r_3);
    ecs_query_fini(r_4);

    ecs_fini(world);
}

void Basic_match_optional_disabled_this_tgt(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);

    ecs_entity_t e_1 = ecs_new_w(world, TagA);
    ecs_entity_t e_2 = ecs_new_w(world, TagA);
    ecs_add_id(world, e_2, EcsDisabled);
    ecs_entity_t e_3 = ecs_new_w(world, TagA);
    ecs_add_id(world, e_3, EcsPrefab);

    ecs_new_w_pair(world, EcsChildOf, e_1);
    ecs_new_w_pair(world, EcsChildOf, e_2);
    ecs_new_w_pair(world, EcsChildOf, e_3);

    ecs_query_t *r_1 = ecs_query(world, {
        .expr = "TagA, ChildOf($child, $this)",
        .cache_kind = cache_kind
    });

    ecs_query_t *r_2 = ecs_query(world, {
        .expr = "TagA, ChildOf($child, $this), ?Disabled",
        .cache_kind = cache_kind
    });

    test_assert(r_1 != NULL);
    test_assert(r_2 != NULL);

    test_bool(r_1->flags & EcsQueryMatchDisabled, false);
    test_bool(r_2->flags & EcsQueryMatchDisabled, true);

    ecs_iter_t it = ecs_query_iter(world, r_1);
    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e_1);
    test_int(ecs_field_id(&it, 0), TagA);
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 1));
    test_assert(!ecs_query_next(&it));

    it = ecs_query_iter(world, r_2);

    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e_1);
    test_int(ecs_field_id(&it, 0), TagA);
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 1));

    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e_2);
    test_int(ecs_field_id(&it, 0), TagA);
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 1));

    test_assert(!ecs_query_next(&it));

    ecs_query_fini(r_1);
    ecs_query_fini(r_2);

    ecs_fini(world);
}

void Basic_match_optional_prefab_this_tgt(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);

    ecs_entity_t e_1 = ecs_new_w(world, TagA);
    ecs_entity_t e_2 = ecs_new_w(world, TagA);
    ecs_add_id(world, e_2, EcsDisabled);
    ecs_entity_t e_3 = ecs_new_w(world, TagA);
    ecs_add_id(world, e_3, EcsPrefab);

    ecs_new_w_pair(world, EcsChildOf, e_1);
    ecs_new_w_pair(world, EcsChildOf, e_2);
    ecs_new_w_pair(world, EcsChildOf, e_3);

    ecs_query_t *r_1 = ecs_query(world, {
        .expr = "TagA, ChildOf($child, $this)",
        .cache_kind = cache_kind
    });

    ecs_query_t *r_2 = ecs_query(world, {
        .expr = "TagA, ChildOf($child, $this), ?Prefab",
        .cache_kind = cache_kind
    });

    test_assert(r_1 != NULL);
    test_assert(r_2 != NULL);

    test_bool(r_1->flags & EcsQueryMatchPrefab, false);
    test_bool(r_2->flags & EcsQueryMatchPrefab, true);

    ecs_iter_t it = ecs_query_iter(world, r_1);
    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e_1);
    test_int(ecs_field_id(&it, 0), TagA);
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 1));
    test_assert(!ecs_query_next(&it));

    it = ecs_query_iter(world, r_2);

    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e_1);
    test_int(ecs_field_id(&it, 0), TagA);
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 1));

    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e_3);
    test_int(ecs_field_id(&it, 0), TagA);
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 1));
    test_assert(!ecs_query_next(&it));

    ecs_query_fini(r_1);
    ecs_query_fini(r_2);

    ecs_fini(world);
}

void Basic_match_optional_disabled_prefab_this_tgt(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);

    ecs_entity_t e_1 = ecs_new_w(world, TagA);
    ecs_entity_t e_2 = ecs_new_w(world, TagA);
    ecs_add_id(world, e_2, EcsDisabled);
    ecs_entity_t e_3 = ecs_new_w(world, TagA);
    ecs_add_id(world, e_3, EcsPrefab);
    ecs_entity_t e_4 = ecs_new_w(world, TagA);
    ecs_add_id(world, e_4, EcsPrefab);
    ecs_add_id(world, e_4, EcsDisabled);

    ecs_new_w_pair(world, EcsChildOf, e_1);
    ecs_new_w_pair(world, EcsChildOf, e_2);
    ecs_new_w_pair(world, EcsChildOf, e_3);
    ecs_new_w_pair(world, EcsChildOf, e_4);

    ecs_query_t *r_1 = ecs_query(world, {
        .expr = "TagA, ChildOf($child, $this)",
        .cache_kind = cache_kind
    });

    ecs_query_t *r_2 = ecs_query(world, {
        .expr = "TagA, ChildOf($child, $this), ?Disabled",
        .cache_kind = cache_kind
    });

    ecs_query_t *r_3 = ecs_query(world, {
        .expr = "TagA, ChildOf($child, $this), ?Prefab",
        .cache_kind = cache_kind
    });

    ecs_query_t *r_4 = ecs_query(world, {
        .expr = "TagA, ChildOf($child, $this), ?Prefab, ?Disabled",
        .cache_kind = cache_kind
    });

    test_assert(r_1 != NULL);
    test_assert(r_2 != NULL);
    test_assert(r_3 != NULL);
    test_assert(r_4 != NULL);

    test_bool(r_1->flags & EcsQueryMatchDisabled, false);
    test_bool(r_2->flags & EcsQueryMatchDisabled, true);
    test_bool(r_3->flags & EcsQueryMatchDisabled, false);
    test_bool(r_4->flags & EcsQueryMatchDisabled, true);

    test_bool(r_1->flags & EcsQueryMatchPrefab, false);
    test_bool(r_2->flags & EcsQueryMatchPrefab, false);
    test_bool(r_3->flags & EcsQueryMatchPrefab, true);
    test_bool(r_4->flags & EcsQueryMatchPrefab, true);

    ecs_iter_t it = ecs_query_iter(world, r_1);
    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e_1);
    test_int(ecs_field_id(&it, 0), TagA);
    test_assert(!ecs_query_next(&it));

    {
        it = ecs_query_iter(world, r_2);
        test_assert(ecs_query_next(&it));
        test_int(it.count, 1);
        test_int(it.entities[0], e_1);
        test_int(ecs_field_id(&it, 0), TagA);
        test_bool(true, ecs_field_is_set(&it, 0));

        test_assert(ecs_query_next(&it));
        test_int(it.count, 1);
        test_int(it.entities[0], e_2);
        test_int(ecs_field_id(&it, 0), TagA);
        test_bool(true, ecs_field_is_set(&it, 0));
        test_assert(!ecs_query_next(&it));
    }

    {
        it = ecs_query_iter(world, r_3);
        test_assert(ecs_query_next(&it));
        test_int(it.count, 1);
        test_int(it.entities[0], e_1);
        test_int(ecs_field_id(&it, 0), TagA);
        test_bool(true, ecs_field_is_set(&it, 0));

        test_assert(ecs_query_next(&it));
        test_int(it.count, 1);
        test_int(it.entities[0], e_3);
        test_int(ecs_field_id(&it, 0), TagA);
        test_bool(true, ecs_field_is_set(&it, 0));
        test_assert(!ecs_query_next(&it));
    }

    {
        it = ecs_query_iter(world, r_4);
        test_assert(ecs_query_next(&it));
        test_int(it.count, 1);
        test_int(it.entities[0], e_1);
        test_int(ecs_field_id(&it, 0), TagA);
        test_bool(true, ecs_field_is_set(&it, 0));

        test_assert(ecs_query_next(&it));
        test_int(it.count, 1);
        test_int(it.entities[0], e_2);
        test_int(ecs_field_id(&it, 0), TagA);
        test_bool(true, ecs_field_is_set(&it, 0));

        test_assert(ecs_query_next(&it));
        test_int(it.count, 1);
        test_int(it.entities[0], e_3);
        test_int(ecs_field_id(&it, 0), TagA);
        test_bool(true, ecs_field_is_set(&it, 0));

        test_assert(ecs_query_next(&it));
        test_int(it.count, 1);
        test_int(it.entities[0], e_4);
        test_int(ecs_field_id(&it, 0), TagA);
        test_bool(true, ecs_field_is_set(&it, 0));

        test_assert(!ecs_query_next(&it));
    }

    ecs_query_fini(r_1);
    ecs_query_fini(r_2);
    ecs_query_fini(r_3);
    ecs_query_fini(r_4);

    ecs_fini(world);
}

void Basic_match_optional_self_disabled(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);

    ecs_entity_t e_1 = ecs_new_w(world, TagA);
    ecs_entity_t e_2 = ecs_new_w(world, TagA);
    ecs_add_id(world, e_2, EcsDisabled);
    ecs_entity_t e_3 = ecs_new_w(world, TagA);
    ecs_add_id(world, e_3, EcsPrefab);

    ecs_query_t *r_1 = ecs_query(world, {
        .expr = "TagA(self)",
        .cache_kind = cache_kind
    });

    ecs_query_t *r_2 = ecs_query(world, {
        .expr = "TagA(self), ?Disabled(self)",
        .cache_kind = cache_kind
    });

    test_assert(r_1 != NULL);
    test_assert(r_2 != NULL);

    test_bool(r_1->flags & EcsQueryMatchDisabled, false);
    test_bool(r_2->flags & EcsQueryMatchDisabled, true);

    ecs_iter_t it = ecs_query_iter(world, r_1);
    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e_1);
    test_int(ecs_field_id(&it, 0), TagA);
    test_bool(true, ecs_field_is_set(&it, 0));
    test_assert(!ecs_query_next(&it));

    it = ecs_query_iter(world, r_2);

    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e_1);
    test_int(ecs_field_id(&it, 0), TagA);
    test_bool(true, ecs_field_is_set(&it, 0));

    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e_2);
    test_int(ecs_field_id(&it, 0), TagA);
    test_bool(true, ecs_field_is_set(&it, 0));

    test_assert(!ecs_query_next(&it));

    ecs_query_fini(r_1);
    ecs_query_fini(r_2);

    ecs_fini(world);
}

void Basic_match_optional_self_prefab(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);

    ecs_entity_t e_1 = ecs_new_w(world, TagA);
    ecs_entity_t e_2 = ecs_new_w(world, TagA);
    ecs_add_id(world, e_2, EcsDisabled);
    ecs_entity_t e_3 = ecs_new_w(world, TagA);
    ecs_add_id(world, e_3, EcsPrefab);

    ecs_query_t *r_1 = ecs_query(world, {
        .expr = "TagA(self)",
        .cache_kind = cache_kind
    });

    ecs_query_t *r_2 = ecs_query(world, {
        .expr = "TagA(self), ?Prefab(self)",
        .cache_kind = cache_kind
    });

    test_assert(r_1 != NULL);
    test_assert(r_2 != NULL);

    test_bool(r_1->flags & EcsQueryMatchPrefab, false);
    test_bool(r_2->flags & EcsQueryMatchPrefab, true);

    ecs_iter_t it = ecs_query_iter(world, r_1);
    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e_1);
    test_int(ecs_field_id(&it, 0), TagA);
    test_bool(true, ecs_field_is_set(&it, 0));
    test_assert(!ecs_query_next(&it));

    it = ecs_query_iter(world, r_2);

    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e_1);
    test_int(ecs_field_id(&it, 0), TagA);
    test_bool(true, ecs_field_is_set(&it, 0));

    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e_3);
    test_int(ecs_field_id(&it, 0), TagA);
    test_bool(true, ecs_field_is_set(&it, 0));

    test_assert(!ecs_query_next(&it));

    ecs_query_fini(r_1);
    ecs_query_fini(r_2);

    ecs_fini(world);
}

void Basic_match_optional_self_disabled_prefab(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);

    ecs_entity_t e_1 = ecs_new_w(world, TagA);
    ecs_entity_t e_2 = ecs_new_w(world, TagA);
    ecs_add_id(world, e_2, EcsDisabled);
    ecs_entity_t e_3 = ecs_new_w(world, TagA);
    ecs_add_id(world, e_3, EcsPrefab);
    ecs_entity_t e_4 = ecs_new_w(world, TagA);
    ecs_add_id(world, e_4, EcsPrefab);
    ecs_add_id(world, e_4, EcsDisabled);

    ecs_query_t *r_1 = ecs_query(world, {
        .expr = "TagA(self)",
        .cache_kind = cache_kind
    });

    ecs_query_t *r_2 = ecs_query(world, {
        .expr = "TagA(self), ?Disabled(self)",
        .cache_kind = cache_kind
    });

    ecs_query_t *r_3 = ecs_query(world, {
        .expr = "TagA(self), ?Prefab(self)",
        .cache_kind = cache_kind
    });

    ecs_query_t *r_4 = ecs_query(world, {
        .expr = "TagA(self), ?Prefab(self), ?Disabled(self)",
        .cache_kind = cache_kind
    });

    test_assert(r_1 != NULL);
    test_assert(r_2 != NULL);
    test_assert(r_3 != NULL);
    test_assert(r_4 != NULL);

    test_bool(r_1->flags & EcsQueryMatchDisabled, false);
    test_bool(r_2->flags & EcsQueryMatchDisabled, true);
    test_bool(r_3->flags & EcsQueryMatchDisabled, false);
    test_bool(r_4->flags & EcsQueryMatchDisabled, true);

    test_bool(r_1->flags & EcsQueryMatchPrefab, false);
    test_bool(r_2->flags & EcsQueryMatchPrefab, false);
    test_bool(r_3->flags & EcsQueryMatchPrefab, true);
    test_bool(r_4->flags & EcsQueryMatchPrefab, true);

    ecs_iter_t it = ecs_query_iter(world, r_1);
    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e_1);
    test_int(ecs_field_id(&it, 0), TagA);
    test_assert(!ecs_query_next(&it));

    {
        it = ecs_query_iter(world, r_2);
        test_assert(ecs_query_next(&it));
        test_int(it.count, 1);
        test_int(it.entities[0], e_1);
        test_int(ecs_field_id(&it, 0), TagA);
        test_bool(true, ecs_field_is_set(&it, 0));

        test_assert(ecs_query_next(&it));
        test_int(it.count, 1);
        test_int(it.entities[0], e_2);
        test_int(ecs_field_id(&it, 0), TagA);
        test_bool(true, ecs_field_is_set(&it, 0));
        test_assert(!ecs_query_next(&it));
    }

    {
        it = ecs_query_iter(world, r_3);
        test_assert(ecs_query_next(&it));
        test_int(it.count, 1);
        test_int(it.entities[0], e_1);
        test_int(ecs_field_id(&it, 0), TagA);
        test_bool(true, ecs_field_is_set(&it, 0));

        test_assert(ecs_query_next(&it));
        test_int(it.count, 1);
        test_int(it.entities[0], e_3);
        test_int(ecs_field_id(&it, 0), TagA);
        test_bool(true, ecs_field_is_set(&it, 0));
        test_assert(!ecs_query_next(&it));
    }

    {
        it = ecs_query_iter(world, r_4);
        test_assert(ecs_query_next(&it));
        test_int(it.count, 1);
        test_int(it.entities[0], e_1);
        test_int(ecs_field_id(&it, 0), TagA);
        test_bool(true, ecs_field_is_set(&it, 0));

        test_assert(ecs_query_next(&it));
        test_int(it.count, 1);
        test_int(it.entities[0], e_2);
        test_int(ecs_field_id(&it, 0), TagA);
        test_bool(true, ecs_field_is_set(&it, 0));

        test_assert(ecs_query_next(&it));
        test_int(it.count, 1);
        test_int(it.entities[0], e_3);
        test_int(ecs_field_id(&it, 0), TagA);
        test_bool(true, ecs_field_is_set(&it, 0));

        test_assert(ecs_query_next(&it));
        test_int(it.count, 1);
        test_int(it.entities[0], e_4);
        test_int(ecs_field_id(&it, 0), TagA);
        test_bool(true, ecs_field_is_set(&it, 0));

        test_assert(!ecs_query_next(&it));
    }

    ecs_query_fini(r_1);
    ecs_query_fini(r_2);
    ecs_query_fini(r_3);
    ecs_query_fini(r_4);

    ecs_fini(world);
}

void Basic_inout_none_first_term(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_query_t *r = ecs_query(world, {
        .terms = {
            { .id = ecs_id(Position), .inout = EcsInOutNone },
            { .id = ecs_id(Velocity) }
        },
        .cache_kind = cache_kind
    });

    ecs_entity_t e1 = ecs_new(world);
    ecs_set(world, e1, Position, {10, 20});
    ecs_set(world, e1, Velocity, {1, 2});

    ecs_iter_t it = ecs_query_iter(world, r);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(ecs_id(Velocity), ecs_field_id(&it, 1));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 1));
    {
        Velocity *v = ecs_field(&it, Velocity, 1);
        test_assert(v != NULL);
        test_int(v->x, 1);
        test_int(v->y, 2);
    }

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(r);

    ecs_fini(world);
}

void Basic_inout_none_first_term_self_up(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_query_t *r = ecs_query(world, {
        .terms = {
            { .id = ecs_id(Position), .inout = EcsInOutNone, .src.id = EcsSelf|EcsUp },
            { .id = ecs_id(Velocity), .src.id = EcsSelf|EcsUp }
        },
        .cache_kind = cache_kind
    });

    ecs_entity_t e1 = ecs_new(world);
    ecs_set(world, e1, Position, {10, 20});
    ecs_set(world, e1, Velocity, {1, 2});

    ecs_iter_t it = ecs_query_iter(world, r);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(ecs_id(Velocity), ecs_field_id(&it, 1));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 1));
    {
        Velocity *v = ecs_field(&it, Velocity, 1);
        test_assert(v != NULL);
        test_int(v->x, 1);
        test_int(v->y, 2);
    }

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(r);

    ecs_fini(world);
}

void Basic_inout_none_second_term(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_query_t *r = ecs_query(world, {
        .terms = {
            { .id = ecs_id(Position) },
            { .id = ecs_id(Velocity), .inout = EcsInOutNone }
        },
        .cache_kind = cache_kind
    });

    ecs_entity_t e1 = ecs_new(world);
    ecs_set(world, e1, Position, {10, 20});
    ecs_set(world, e1, Velocity, {1, 2});

    ecs_iter_t it = ecs_query_iter(world, r);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(ecs_id(Velocity), ecs_field_id(&it, 1));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 20);
    }

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(r);

    ecs_fini(world);
}

void Basic_inout_none_second_term_self_up(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_query_t *r = ecs_query(world, {
        .terms = {
            { .id = ecs_id(Position), .src.id = EcsSelf|EcsUp },
            { .id = ecs_id(Velocity), .inout = EcsInOutNone, .src.id = EcsSelf|EcsUp }
        },
        .cache_kind = cache_kind
    });

    ecs_entity_t e1 = ecs_new(world);
    ecs_set(world, e1, Position, {10, 20});
    ecs_set(world, e1, Velocity, {1, 2});

    ecs_iter_t it = ecs_query_iter(world, r);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(ecs_id(Velocity), ecs_field_id(&it, 1));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 20);
    }

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(r);

    ecs_fini(world);
}

void Basic_inout_none_singleton(void) {
    ecs_world_t *ecs = ecs_mini();

    ECS_COMPONENT(ecs, Position);
    ECS_COMPONENT(ecs, Velocity);
    
    ecs_singleton_set(ecs, Position, {10, 20});

    ecs_entity_t e = ecs_insert(ecs, ecs_value(Velocity, {20, 30}));

    ecs_query_t *q = ecs_query(ecs, {
        .terms = {
            { .id = ecs_id(Position), .src.id = EcsVariable },
            { .id = ecs_id(Velocity), .inout = EcsInOutNone },
        },
        .cache_kind = cache_kind
    });
    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(ecs, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(ecs_id(Velocity), ecs_field_id(&it, 1));
    Position *p = ecs_field(&it, Position, 0);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(ecs);
}

void Basic_inout_none_singleton_w_or(void) {
    ecs_world_t *ecs = ecs_mini();

    ECS_COMPONENT(ecs, Position);
    ECS_COMPONENT(ecs, Velocity);
    ECS_COMPONENT(ecs, Mass);

    ecs_singleton_set(ecs, Position, {10, 20});

    ecs_entity_t e = ecs_insert(ecs, ecs_value(Velocity, {20, 30}));

    ecs_query_t *q = ecs_query(ecs, {
        .terms = {
            { .id = ecs_id(Position), .src.id = EcsVariable },
            { .id = ecs_id(Velocity), .inout = EcsInOutNone, .oper = EcsOr },
            { .id = ecs_id(Mass), .inout = EcsInOutNone },
        },
        .cache_kind = cache_kind
    });
    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(ecs, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(ecs_id(Velocity), ecs_field_id(&it, 1));
    Position *p = ecs_field(&it, Position, 0);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(ecs);
}

void Basic_inout_none_component_w_or(void) {
    ecs_world_t *ecs = ecs_mini();

    ECS_COMPONENT(ecs, Position);
    ECS_COMPONENT(ecs, Velocity);
    ECS_COMPONENT(ecs, Mass);

    ecs_entity_t e = ecs_insert(ecs, ecs_value(Velocity, {20, 30}));
    ecs_set(ecs, e, Position, {10, 20});

    ecs_query_t *q = ecs_query(ecs, {
        .terms = {
            { .id = ecs_id(Position) },
            { .id = ecs_id(Velocity), .inout = EcsInOutNone, .oper = EcsOr },
            { .id = ecs_id(Mass), .inout = EcsInOutNone },
        },
        .cache_kind = cache_kind
    });
    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(ecs, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(ecs_id(Velocity), ecs_field_id(&it, 1));
    Position *p = ecs_field(&it, Position, 0);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(ecs);
}

void Basic_no_data_rule(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_query_t *r = ecs_query(world, {
        .terms = {
            { .id = ecs_id(Position), .inout = EcsInOutNone },
            { .id = ecs_id(Velocity), .inout = EcsInOutNone }
        },
        .cache_kind = cache_kind
    });

    ecs_entity_t e1 = ecs_new(world);
    ecs_set(world, e1, Position, {10, 20});
    ecs_set(world, e1, Velocity, {1, 2});

    ecs_iter_t it = ecs_query_iter(world, r);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(ecs_id(Velocity), ecs_field_id(&it, 1));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 1));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(r);

    ecs_fini(world);
}

void Basic_frame_offset(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, TagB);
    ECS_TAG(world, TagC);

    ecs_query_t *f = ecs_query(world, {
        .terms = {
            { .id = ecs_id(Position), }
        },
        .cache_kind = cache_kind
    });

    ecs_entity_t e1 = ecs_new_w(world, Position);
    ecs_entity_t e2 = ecs_new_w(world, Position);
    ecs_entity_t e3 = ecs_new_w(world, Position);
    ecs_entity_t e4 = ecs_new_w(world, Position);
    ecs_entity_t e5 = ecs_new_w(world, Position);

    ecs_add(world, e3, TagB);
    ecs_add(world, e4, TagB);
    ecs_add(world, e5, TagC);

    ecs_iter_t it = ecs_query_iter(world, f);

    test_bool(ecs_query_next(&it), true);
    test_int(it.count, 2);
    test_int(it.frame_offset, 0);
    test_assert(it.entities != NULL);
    test_assert(it.entities[0] == e1);
    test_assert(it.entities[1] == e2);
    test_assert(it.ids[0] == ecs_id(Position));
    test_bool(true, ecs_field_is_set(&it, 0));

    test_bool(ecs_query_next(&it), true);
    test_int(it.count, 2);
    test_int(it.frame_offset, 2);
    test_assert(it.entities != NULL);
    test_assert(it.entities[0] == e3);
    test_assert(it.entities[1] == e4);
    test_assert(it.ids[0] == ecs_id(Position));
    test_bool(true, ecs_field_is_set(&it, 0));

    test_bool(ecs_query_next(&it), true);
    test_int(it.count, 1);
    test_int(it.frame_offset, 4);
    test_assert(it.entities != NULL);
    test_assert(it.entities[0] == e5);
    test_assert(it.ids[0] == ecs_id(Position));
    test_bool(true, ecs_field_is_set(&it, 0));

    test_bool(ecs_query_next(&it), false);

    ecs_query_fini(f);

    ecs_fini(world);
}

void Basic_frame_offset_no_data(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, TagC);

    ecs_query_t *f = ecs_query(world, {
        .terms = {
            { .id = TagA, }
        },
        .cache_kind = cache_kind
    });

    ecs_entity_t e1 = ecs_new_w(world, TagA);
    ecs_entity_t e2 = ecs_new_w(world, TagA);
    ecs_entity_t e3 = ecs_new_w(world, TagA);
    ecs_entity_t e4 = ecs_new_w(world, TagA);
    ecs_entity_t e5 = ecs_new_w(world, TagA);

    ecs_add(world, e3, TagB);
    ecs_add(world, e4, TagB);
    ecs_add(world, e5, TagC);

    ecs_iter_t it = ecs_query_iter(world, f);

    test_bool(ecs_query_next(&it), true);
    test_int(it.count, 2);
    test_int(it.frame_offset, 0);
    test_assert(it.entities != NULL);
    test_assert(it.entities[0] == e1);
    test_assert(it.entities[1] == e2);
    test_assert(it.ids[0] == TagA);
    test_bool(true, ecs_field_is_set(&it, 0));

    test_bool(ecs_query_next(&it), true);
    test_int(it.count, 2);
    test_int(it.frame_offset, 2);
    test_assert(it.entities != NULL);
    test_assert(it.entities[0] == e3);
    test_assert(it.entities[1] == e4);
    test_assert(it.ids[0] == TagA);
    test_bool(true, ecs_field_is_set(&it, 0));

    test_bool(ecs_query_next(&it), true);
    test_int(it.count, 1);
    test_int(it.frame_offset, 4);
    test_assert(it.entities != NULL);
    test_assert(it.entities[0] == e5);
    test_assert(it.ids[0] == TagA);
    test_bool(true, ecs_field_is_set(&it, 0));

    test_bool(ecs_query_next(&it), false);

    ecs_query_fini(f);

    ecs_fini(world);
}

void Basic_match_empty_tables(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    
    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, TagC);

    ecs_entity_t e1 = ecs_new_w(world, Position);
    ecs_entity_t e2 = ecs_new_w(world, Position);
    ecs_entity_t e3 = ecs_new_w(world, Position);
    ecs_entity_t e4 = ecs_new_w(world, Position);

    ecs_add(world, e2, TagA);
    ecs_add(world, e3, TagB);
    ecs_add(world, e4, TagC);

    ecs_table_t *t1 = ecs_get_table(world, e1);
    ecs_table_t *t2 = ecs_get_table(world, e2);
    ecs_table_t *t3 = ecs_get_table(world, e3);
    ecs_table_t *t4 = ecs_get_table(world, e4);

    ecs_delete(world, e1);
    ecs_delete(world, e2);

    ecs_query_t *f = ecs_query(world, {
        .terms = {{ ecs_id(Position) }},
        .flags = EcsQueryMatchEmptyTables,
        .cache_kind = cache_kind
    });

    ecs_iter_t it = ecs_query_iter(world, f);

    test_bool( ecs_query_next(&it), true);
    test_assert(it.table == t1);
    test_int(it.count, 0);
    test_uint(ecs_field_id(&it, 0), ecs_id(Position));
    test_uint(ecs_field_size(&it, 0), sizeof(Position));
    test_bool(true, ecs_field_is_set(&it, 0));

    test_bool( ecs_query_next(&it), true);
    test_assert(it.table == t2);
    test_int(it.count, 0);
    test_uint(ecs_field_id(&it, 0), ecs_id(Position));
    test_uint(ecs_field_size(&it, 0), sizeof(Position));
    test_bool(true, ecs_field_is_set(&it, 0));

    test_bool( ecs_query_next(&it), true);
    test_assert(it.table == t3);
    test_int(it.count, 1);
    test_int(it.entities[0], e3);
    test_uint(ecs_field_id(&it, 0), ecs_id(Position));
    test_uint(ecs_field_size(&it, 0), sizeof(Position));
    test_bool(true, ecs_field_is_set(&it, 0));

    test_bool( ecs_query_next(&it), true);
    test_assert(it.table == t4);
    test_int(it.count, 1);
    test_int(it.entities[0], e4);
    test_uint(ecs_field_id(&it, 0), ecs_id(Position));
    test_uint(ecs_field_size(&it, 0), sizeof(Position));
    test_bool(true, ecs_field_is_set(&it, 0));

    test_bool( ecs_query_next(&it), false);

    ecs_query_fini(f);

    ecs_fini(world);
}

void Basic_match_empty_tables_no_data(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    
    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, TagC);

    ecs_entity_t e1 = ecs_new_w(world, Foo);
    ecs_entity_t e2 = ecs_new_w(world, Foo);
    ecs_entity_t e3 = ecs_new_w(world, Foo);
    ecs_entity_t e4 = ecs_new_w(world, Foo);

    ecs_add(world, e2, TagA);
    ecs_add(world, e3, TagB);
    ecs_add(world, e4, TagC);

    ecs_table_t *t1 = ecs_get_table(world, e1);
    ecs_table_t *t2 = ecs_get_table(world, e2);
    ecs_table_t *t3 = ecs_get_table(world, e3);
    ecs_table_t *t4 = ecs_get_table(world, e4);

    ecs_delete(world, e1);
    ecs_delete(world, e2);

    ecs_query_t *f = ecs_query(world, {
        .terms = {{ Foo }},
        .flags = EcsQueryMatchEmptyTables,
        .cache_kind = cache_kind
    });

    ecs_iter_t it = ecs_query_iter(world, f);
    
    test_bool( ecs_query_next(&it), true);
    test_assert(it.table == t1);
    test_int(it.count, 0);
    test_uint(ecs_field_id(&it, 0), Foo);
    test_bool(true, ecs_field_is_set(&it, 0));

    test_bool( ecs_query_next(&it), true);
    test_assert(it.table == t2);
    test_int(it.count, 0);
    test_uint(ecs_field_id(&it, 0), Foo);
    test_bool(true, ecs_field_is_set(&it, 0));

    test_bool( ecs_query_next(&it), true);
    test_assert(it.table == t3);
    test_int(it.count, 1);
    test_int(it.entities[0], e3);
    test_uint(ecs_field_id(&it, 0), Foo);
    test_bool(true, ecs_field_is_set(&it, 0));

    test_bool( ecs_query_next(&it), true);
    test_assert(it.table == t4);
    test_int(it.count, 1);
    test_int(it.entities[0], e4);
    test_uint(ecs_field_id(&it, 0), Foo);
    test_bool(true, ecs_field_is_set(&it, 0));

    test_bool( ecs_query_next(&it), false);

    ecs_query_fini(f);

    ecs_fini(world);
}

void Basic_match_empty_tables_w_not(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    
    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, TagC);

    ecs_entity_t e1 = ecs_new_w(world, Foo);
    ecs_entity_t e2 = ecs_new_w(world, Foo);
    ecs_entity_t e3 = ecs_new_w(world, Foo);
    ecs_entity_t e4 = ecs_new_w(world, Foo);

    ecs_add(world, e2, TagA);
    ecs_add(world, e3, TagB);
    ecs_add(world, e4, TagC);

    ecs_table_t *t1 = ecs_get_table(world, e1);
    ecs_get_table(world, e2);
    ecs_table_t *t3 = ecs_get_table(world, e3);
    ecs_table_t *t4 = ecs_get_table(world, e4);

    ecs_delete(world, e1);
    ecs_delete(world, e2);

    ecs_query_t *f = ecs_query(world, {
        .terms = {{ Foo }, { TagA, .oper = EcsNot }},
        .flags = EcsQueryMatchEmptyTables,
        .cache_kind = cache_kind
    });

    ecs_iter_t it = ecs_query_iter(world, f);
    
    test_bool( ecs_query_next(&it), true);
    test_assert(it.table == t1);
    test_int(it.count, 0);
    test_uint(ecs_field_id(&it, 0), Foo);
    test_bool(true, ecs_field_is_set(&it, 0));

    test_bool( ecs_query_next(&it), true);
    test_assert(it.table == t3);
    test_int(it.count, 1);
    test_int(it.entities[0], e3);
    test_uint(ecs_field_id(&it, 0), Foo);
    test_bool(true, ecs_field_is_set(&it, 0));

    test_bool( ecs_query_next(&it), true);
    test_assert(it.table == t4);
    test_int(it.count, 1);
    test_int(it.entities[0], e4);
    test_uint(ecs_field_id(&it, 0), Foo);
    test_bool(true, ecs_field_is_set(&it, 0));

    test_bool( ecs_query_next(&it), false);

    ecs_query_fini(f);

    ecs_fini(world);
}

void Basic_match_empty_tables_w_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);
    ECS_TAG(world, TgtC);
    ECS_TAG(world, TgtD);

    ecs_entity_t e1 = ecs_new_w_pair(world, Rel, TgtA);
    ecs_entity_t e2 = ecs_new_w_pair(world, Rel, TgtB);
    ecs_entity_t e3 = ecs_new_w_pair(world, Rel, TgtC);
    ecs_entity_t e4 = ecs_new_w_pair(world, Rel, TgtD);

    ecs_table_t *t1 = ecs_get_table(world, e1);
    ecs_table_t *t2 = ecs_get_table(world, e2);
    ecs_table_t *t3 = ecs_get_table(world, e3);
    ecs_table_t *t4 = ecs_get_table(world, e4);

    ecs_delete(world, e1);
    ecs_delete(world, e2);

    ecs_query_t *f = ecs_query(world, {
        .terms = {{ ecs_pair(Rel, EcsWildcard) }},
        .flags = EcsQueryMatchEmptyTables,
        .cache_kind = cache_kind
    });

    ecs_iter_t it = ecs_query_iter(world, f);

    test_bool( ecs_query_next(&it), true);
    test_assert(it.table == t1);
    test_int(it.count, 0);
    test_uint(ecs_field_id(&it, 0), ecs_pair(Rel, TgtA));
    test_bool(true, ecs_field_is_set(&it, 0));

    test_bool( ecs_query_next(&it), true);
    test_assert(it.table == t2);
    test_int(it.count, 0);
    test_uint(ecs_field_id(&it, 0), ecs_pair(Rel, TgtB));
    test_bool(true, ecs_field_is_set(&it, 0));

    test_bool( ecs_query_next(&it), true);
    test_assert(it.table == t3);
    test_int(it.count, 1);
    test_int(it.entities[0], e3);
    test_uint(ecs_field_id(&it, 0), ecs_pair(Rel, TgtC));
    test_bool(true, ecs_field_is_set(&it, 0));

    test_bool( ecs_query_next(&it), true);
    test_assert(it.table == t4);
    test_int(it.count, 1);
    test_int(it.entities[0], e4);
    test_uint(ecs_field_id(&it, 0), ecs_pair(Rel, TgtD));
    test_bool(true, ecs_field_is_set(&it, 0));

    test_bool( ecs_query_next(&it), false);

    ecs_query_fini(f);

    ecs_fini(world);
}

void Basic_match_empty_tables_w_no_empty_tables(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_query_t *f = ecs_query(world, {
        .terms = {
            { Foo }
        },
        .flags = EcsQueryMatchEmptyTables,
        .cache_kind = cache_kind
    });

    ecs_entity_t e1 = ecs_new_w(world, Foo);
    ecs_entity_t e2 = ecs_new_w(world, Foo);
    ecs_add(world, e2, Bar);

    ecs_iter_t it = ecs_query_iter(world, f);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e1, it.entities[0]);
    test_bool(true, ecs_field_is_set(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e2, it.entities[0]);
    test_bool(true, ecs_field_is_set(&it, 0));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(f);

    ecs_fini(world);
}

void Basic_match_empty_tables_trivial(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    
    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, TagC);

    ecs_entity_t e1 = ecs_new_w(world, Position);
    ecs_entity_t e2 = ecs_new_w(world, Position);
    ecs_entity_t e3 = ecs_new_w(world, Position);
    ecs_entity_t e4 = ecs_new_w(world, Position);

    ecs_add(world, e2, TagA);
    ecs_add(world, e3, TagB);
    ecs_add(world, e4, TagC);

    ecs_table_t *t1 = ecs_get_table(world, e1);
    ecs_table_t *t2 = ecs_get_table(world, e2);
    ecs_table_t *t3 = ecs_get_table(world, e3);
    ecs_table_t *t4 = ecs_get_table(world, e4);

    ecs_delete(world, e1);
    ecs_delete(world, e2);

    ecs_query_t *f = ecs_query(world, {
        .terms = {{ ecs_id(Position), .src.id = EcsSelf }},
        .flags = EcsQueryMatchEmptyTables,
        .cache_kind = cache_kind
    });

    ecs_iter_t it = ecs_query_iter(world, f);
    test_bool( ecs_query_next(&it), true);
    test_assert(it.table == t1);
    test_int(it.count, 0);
    test_uint(ecs_field_id(&it, 0), ecs_id(Position));
    test_uint(ecs_field_size(&it, 0), sizeof(Position));
    test_bool(true, ecs_field_is_set(&it, 0));

    test_bool( ecs_query_next(&it), true);
    test_assert(it.table == t2);
    test_int(it.count, 0);
    test_uint(ecs_field_id(&it, 0), ecs_id(Position));
    test_uint(ecs_field_size(&it, 0), sizeof(Position));
    test_bool(true, ecs_field_is_set(&it, 0));

    test_bool( ecs_query_next(&it), true);
    test_assert(it.table == t3);
    test_int(it.count, 1);
    test_int(it.entities[0], e3);
    test_uint(ecs_field_id(&it, 0), ecs_id(Position));
    test_uint(ecs_field_size(&it, 0), sizeof(Position));
    test_bool(true, ecs_field_is_set(&it, 0));

    test_bool( ecs_query_next(&it), true);
    test_assert(it.table == t4);
    test_int(it.count, 1);
    test_int(it.entities[0], e4);
    test_uint(ecs_field_id(&it, 0), ecs_id(Position));
    test_uint(ecs_field_size(&it, 0), sizeof(Position));
    test_bool(true, ecs_field_is_set(&it, 0));

    test_bool( ecs_query_next(&it), false);

    ecs_query_fini(f);

    ecs_fini(world);
}

void Basic_oneof_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Parent);
    ECS_ENTITY(world, Rel, (OneOf, Parent));
    ECS_ENTITY(world, ObjA, (ChildOf, Parent));
    ECS_ENTITY(world, ObjB, (ChildOf, Parent));

    ecs_entity_t e1 = ecs_new_w_pair(world, Rel, ObjA);
    test_assert( ecs_has_pair(world, e1, Rel, ObjA));
    ecs_entity_t e2 = ecs_new_w_pair(world, Rel, ObjB);
    test_assert( ecs_has_pair(world, e2, Rel, ObjB));

    ecs_query_t *f = ecs_query(world, {
        .terms = {{
            .id = ecs_pair(Rel, EcsWildcard)
        }},
        .cache_kind = cache_kind
    });

    ecs_iter_t it = ecs_query_iter(world, f);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(ecs_pair(Rel, ObjA), ecs_field_id(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e2, it.entities[0]);
    test_uint(ecs_pair(Rel, ObjB), ecs_field_id(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 0));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(f);

    ecs_fini(world);
}

void Basic_oneof_any(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Parent);
    ECS_ENTITY(world, Rel, (OneOf, Parent));
    ECS_ENTITY(world, ObjA, (ChildOf, Parent));
    ECS_ENTITY(world, ObjB, (ChildOf, Parent));

    ecs_entity_t e1 = ecs_new_w_pair(world, Rel, ObjA);
    test_assert( ecs_has_pair(world, e1, Rel, ObjA));
    ecs_entity_t e2 = ecs_new_w_pair(world, Rel, ObjB);
    test_assert( ecs_has_pair(world, e2, Rel, ObjB));

    ecs_query_t *f = ecs_query(world, {
        .terms = {{
            .id = ecs_pair(Rel, EcsAny)
        }},
        .cache_kind = cache_kind
    });

    ecs_iter_t it = ecs_query_iter(world, f);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(ecs_pair(Rel, EcsWildcard), ecs_field_id(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e2, it.entities[0]);
    test_uint(ecs_pair(Rel, EcsWildcard), ecs_field_id(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 0));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(f);

    ecs_fini(world);
}

void Basic_instanced_w_singleton(void) {
    ecs_world_t *world = ecs_mini();
    
    ECS_TAG(world, Tag);

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_singleton_set(world, Velocity, {1, 2});

    ecs_entity_t e1 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(Position, {20, 30}));
    ecs_entity_t e3 = ecs_insert(world, ecs_value(Position, {30, 40}));

    ecs_entity_t e4 = ecs_insert(world, ecs_value(Position, {40, 50}));
    ecs_entity_t e5 = ecs_insert(world, ecs_value(Position, {50, 60}));

    ecs_add(world, e4, Tag);
    ecs_add(world, e5, Tag);

    ecs_query_t *f = ecs_query(world, {
        .expr = "Position, Velocity($)",
        .cache_kind = cache_kind,
    });

    ecs_iter_t it = ecs_query_iter(world, f);
    test_assert(ecs_query_next(&it));
    {
        Position *p = ecs_field(&it, Position, 0);
        Velocity *v = ecs_field(&it, Velocity, 1);
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_assert(p != NULL);
        test_assert(v != NULL);
        test_int(it.count, 3);
        test_int(it.entities[0], e1);
        test_int(it.entities[1], e2);
        test_int(it.entities[2], e3);
        test_int(v->x, 1);
        test_int(v->y, 2);
        test_int(p[0].x, 10);
        test_int(p[0].y, 20);
        test_int(p[1].x, 20);
        test_int(p[1].y, 30);
        test_int(p[2].x, 30);
        test_int(p[2].y, 40);
    }

    test_assert(ecs_query_next(&it));
    {
        Position *p = ecs_field(&it, Position, 0);
        Velocity *v = ecs_field(&it, Velocity, 1);
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_assert(p != NULL);
        test_assert(v != NULL);
        test_int(it.count, 2);
        test_int(it.entities[0], e4);
        test_int(it.entities[1], e5);
        test_int(p[0].x, 40);
        test_int(p[0].y, 50);
        test_int(p[1].x, 50);
        test_int(p[1].y, 60);
        test_int(v->x, 1);
        test_int(v->y, 2);
    }

    test_assert(!ecs_query_next(&it));

    ecs_query_fini(f);

    ecs_fini(world);
}

void Basic_instanced_w_base(void) {
    ecs_world_t *world = ecs_mini();
    
    ECS_TAG(world, Tag);

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    ecs_add_pair(world, ecs_id(Velocity), EcsOnInstantiate, EcsInherit);

    ecs_entity_t base_1 = ecs_insert(world, ecs_value(Velocity, {1, 2}));
    ecs_entity_t base_2 = ecs_insert(world, ecs_value(Position, {80, 90}));

    ecs_entity_t e1 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(Position, {20, 30}));
    ecs_entity_t e3 = ecs_insert(world, ecs_value(Position, {30, 40}));

    ecs_entity_t e4 = ecs_insert(world, ecs_value(Position, {40, 50}));
    ecs_entity_t e5 = ecs_insert(world, ecs_value(Position, {50, 60}));
    ecs_add(world, e4, Tag);
    ecs_add(world, e5, Tag);

    ecs_add_pair(world, e1, EcsIsA, base_1);
    ecs_add_pair(world, e2, EcsIsA, base_1);
    ecs_add_pair(world, e3, EcsIsA, base_1);
    ecs_add_pair(world, e4, EcsIsA, base_1);
    ecs_add_pair(world, e5, EcsIsA, base_1);

    ecs_entity_t e6 = ecs_insert(world, ecs_value(Position, {60, 70}));
    ecs_entity_t e7 = ecs_insert(world, ecs_value(Position, {70, 80}));
    ecs_set(world, e6, Velocity, {2, 3});
    ecs_set(world, e7, Velocity, {4, 5});

    ecs_entity_t e8 = ecs_insert(world, ecs_value(Velocity, {6, 7}));
    ecs_entity_t e9 = ecs_insert(world, ecs_value(Velocity, {8, 9}));
    ecs_add_pair(world, e8, EcsIsA, base_2);
    ecs_add_pair(world, e9, EcsIsA, base_2);

    ecs_query_t *f = ecs_query(world, {
        .expr = "Position, Velocity",
        .cache_kind = cache_kind,
    });

    ecs_iter_t it = ecs_query_iter(world, f);

    test_assert(ecs_query_next(&it));
    {
        test_bool(ecs_field_is_self(&it, 0), false);
        test_bool(ecs_field_is_self(&it, 1), true);
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));

        Position *p = ecs_field(&it, Position, 0);
        Velocity *v = ecs_field(&it, Velocity, 1);
        test_assert(p != NULL);
        test_assert(v != NULL);
        test_int(it.count, 2);
        test_int(it.entities[0], e8);
        test_int(it.entities[1], e9);
        test_int(p->x, 80);
        test_int(p->y, 90);
        test_int(v[0].x, 6);
        test_int(v[0].y, 7);
        test_int(v[1].x, 8);
        test_int(v[1].y, 9);
    }

    test_assert(ecs_query_next(&it));
    {
        test_bool(ecs_field_is_self(&it, 0), true);
        test_bool(ecs_field_is_self(&it, 1), false);
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));

        Position *p = ecs_field(&it, Position, 0);
        Velocity *v = ecs_field(&it, Velocity, 1);
        test_assert(p != NULL);
        test_assert(v != NULL);
        test_int(it.count, 3);
        test_int(it.entities[0], e1);
        test_int(it.entities[1], e2);
        test_int(it.entities[2], e3);
        test_int(p[0].x, 10);
        test_int(p[0].y, 20);
        test_int(p[1].x, 20);
        test_int(p[1].y, 30);
        test_int(p[2].x, 30);
        test_int(p[2].y, 40);
        test_int(v->x, 1);
        test_int(v->y, 2);
    }

    test_assert(ecs_query_next(&it));
    {
        test_bool(ecs_field_is_self(&it, 0), true);
        test_bool(ecs_field_is_self(&it, 1), false);
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));

        Position *p = ecs_field(&it, Position, 0);
        Velocity *v = ecs_field(&it, Velocity, 1);
        test_assert(p != NULL);
        test_assert(v != NULL);
        test_int(it.count, 2);
        test_int(it.entities[0], e4);
        test_int(it.entities[1], e5);
        test_int(p[0].x, 40);
        test_int(p[0].y, 50);
        test_int(p[1].x, 50);
        test_int(p[1].y, 60);
        test_int(v->x, 1);
        test_int(v->y, 2);
    }

    test_assert(ecs_query_next(&it));
    {
        test_bool(ecs_field_is_self(&it, 0), true);
        test_bool(ecs_field_is_self(&it, 1), true);
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));

        Position *p = ecs_field(&it, Position, 0);
        Velocity *v = ecs_field(&it, Velocity, 1);
        test_assert(p != NULL);
        test_assert(v != NULL);
        test_int(it.count, 2);
        test_int(it.entities[0], e6);
        test_int(p[0].x, 60);
        test_int(p[0].y, 70);
        test_int(v[0].x, 2);
        test_int(v[0].y, 3);

        test_int(it.entities[1], e7);
        test_int(p[1].x, 70);
        test_int(p[1].y, 80);
        test_int(v[1].x, 4);
        test_int(v[1].y, 5);
    }

    test_assert(!ecs_query_next(&it));

    ecs_query_fini(f);

    ecs_fini(world);
}

void Basic_unknown_before_known(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_query_t *r = ecs_query(world, {
        .expr = "Foo, ChildOf($gc, $c), ChildOf($c, $this)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    int c_var = ecs_query_find_var(r, "c");
    test_assert(c_var != -1);
    int gc_var = ecs_query_find_var(r, "gc");
    test_assert(gc_var != -1);

    ecs_entity_t p = ecs_new_w(world, Foo);
    ecs_entity_t c = ecs_new_w_pair(world, EcsChildOf, p);
    /* ecs_entity_t c2 = */ ecs_new_w_pair(world, EcsChildOf, p);
    ecs_entity_t gc = ecs_new_w_pair(world, EcsChildOf, c);

    ecs_iter_t it = ecs_query_iter(world, r);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(p, it.entities[0]);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_pair(EcsChildOf, c), ecs_field_id(&it, 1));
    test_uint(ecs_pair(EcsChildOf, p), ecs_field_id(&it, 2));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 1));
    test_bool(true, ecs_field_is_set(&it, 2));
    test_uint(c, ecs_iter_get_var(&it, c_var));
    test_uint(gc, ecs_iter_get_var(&it, gc_var));
    test_assert(!ecs_query_next(&it));

    ecs_query_fini(r);
    
    ecs_fini(world);
}

void Basic_unknown_before_known_after_or(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_query_t *r = ecs_query(world, {
        .expr = "Foo, ChildOf($gc, $c), Foo($gc) || Bar($gc), ChildOf($c, $this)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    int c_var = ecs_query_find_var(r, "c");
    test_assert(c_var != -1);
    int gc_var = ecs_query_find_var(r, "gc");
    test_assert(gc_var != -1);

    ecs_entity_t p = ecs_new_w(world, Foo);
    ecs_entity_t c = ecs_new_w_pair(world, EcsChildOf, p);
    /* ecs_entity_t c2 = */ ecs_new_w_pair(world, EcsChildOf, p);
    ecs_entity_t gc1 = ecs_new_w_pair(world, EcsChildOf, c);
    ecs_add(world, gc1, Foo);
    ecs_entity_t gc2 = ecs_new_w_pair(world, EcsChildOf, c);
    ecs_add(world, gc2, Bar);
    /* ecs_entity_t gc3 = */ ecs_new_w_pair(world, EcsChildOf, c);

    ecs_iter_t it = ecs_query_iter(world, r);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(p, it.entities[0]);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_pair(EcsChildOf, c), ecs_field_id(&it, 1));
    test_uint(ecs_pair(EcsChildOf, p), ecs_field_id(&it, 3));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 1));
    test_bool(true, ecs_field_is_set(&it, 3));
    test_uint(c, ecs_iter_get_var(&it, c_var));
    test_uint(gc1, ecs_iter_get_var(&it, gc_var));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(p, it.entities[0]);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_pair(EcsChildOf, c), ecs_field_id(&it, 1));
    test_uint(ecs_pair(EcsChildOf, p), ecs_field_id(&it, 3));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 1));
    test_bool(true, ecs_field_is_set(&it, 3));
    test_uint(c, ecs_iter_get_var(&it, c_var));
    test_uint(gc2, ecs_iter_get_var(&it, gc_var));
    test_assert(!ecs_query_next(&it));
    
    ecs_query_fini(r);
    
    ecs_fini(world);
}

void Basic_unknown_before_known_after_not(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_query_t *r = ecs_query(world, {
        .expr = "Foo, ChildOf($gc, $c), !Foo($gc), ChildOf($c, $this)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    int c_var = ecs_query_find_var(r, "c");
    test_assert(c_var != -1);
    int gc_var = ecs_query_find_var(r, "gc");
    test_assert(gc_var != -1);

    ecs_entity_t p = ecs_new_w(world, Foo);
    ecs_entity_t c = ecs_new_w_pair(world, EcsChildOf, p);
    /* ecs_entity_t c2 = */ ecs_new_w_pair(world, EcsChildOf, p);
    ecs_entity_t gc1 = ecs_new_w_pair(world, EcsChildOf, c);
    ecs_entity_t gc2 = ecs_new_w_pair(world, EcsChildOf, c);
    ecs_add(world, gc2, Foo);

    ecs_iter_t it = ecs_query_iter(world, r);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(p, it.entities[0]);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_pair(EcsChildOf, c), ecs_field_id(&it, 1));
    test_uint(Foo, ecs_field_id(&it, 2));
    test_uint(ecs_pair(EcsChildOf, p), ecs_field_id(&it, 3));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 1));
    test_bool(false, ecs_field_is_set(&it, 2));
    test_bool(true, ecs_field_is_set(&it, 3));
    test_uint(c, ecs_iter_get_var(&it, c_var));
    test_uint(gc1, ecs_iter_get_var(&it, gc_var));
    test_assert(!ecs_query_next(&it));

    ecs_query_fini(r);
    
    ecs_fini(world);
}

void Basic_unknown_before_known_after_optional(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_query_t *r = ecs_query(world, {
        .expr = "Foo, ChildOf($gc, $c), ?Foo($gc), ChildOf($c, $this)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    int c_var = ecs_query_find_var(r, "c");
    test_assert(c_var != -1);
    int gc_var = ecs_query_find_var(r, "gc");
    test_assert(gc_var != -1);

    ecs_entity_t p = ecs_new_w(world, Foo);
    ecs_entity_t c = ecs_new_w_pair(world, EcsChildOf, p);
    /* ecs_entity_t c2 = */ ecs_new_w_pair(world, EcsChildOf, p);
    ecs_entity_t gc1 = ecs_new_w_pair(world, EcsChildOf, c);
    ecs_entity_t gc2 = ecs_new_w_pair(world, EcsChildOf, c);
    ecs_add(world, gc2, Foo);

    ecs_iter_t it = ecs_query_iter(world, r);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(p, it.entities[0]);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_pair(EcsChildOf, c), ecs_field_id(&it, 1));
    test_uint(Foo, ecs_field_id(&it, 2));
    test_uint(ecs_pair(EcsChildOf, p), ecs_field_id(&it, 3));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 1));
    test_bool(false, ecs_field_is_set(&it, 2));
    test_bool(true, ecs_field_is_set(&it, 3));
    test_uint(c, ecs_iter_get_var(&it, c_var));
    test_uint(gc1, ecs_iter_get_var(&it, gc_var));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(p, it.entities[0]);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_pair(EcsChildOf, c), ecs_field_id(&it, 1));
    test_uint(Foo, ecs_field_id(&it, 2));
    test_uint(ecs_pair(EcsChildOf, p), ecs_field_id(&it, 3));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 1));
    test_bool(true, ecs_field_is_set(&it, 2));
    test_bool(true, ecs_field_is_set(&it, 3));
    test_uint(c, ecs_iter_get_var(&it, c_var));
    test_uint(gc2, ecs_iter_get_var(&it, gc_var));
    test_assert(!ecs_query_next(&it));

    ecs_query_fini(r);
    
    ecs_fini(world);
}

void Basic_unknown_before_known_after_scope(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_query_t *r = ecs_query(world, {
        .expr = "Foo, ChildOf($gc, $c), !{Foo($gc) || Bar($gc)}, ChildOf($c, $this)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    int c_var = ecs_query_find_var(r, "c");
    test_assert(c_var != -1);
    int gc_var = ecs_query_find_var(r, "gc");
    test_assert(gc_var != -1);

    ecs_entity_t p = ecs_new_w(world, Foo);
    ecs_entity_t c = ecs_new_w_pair(world, EcsChildOf, p);
    /* ecs_entity_t c2 = */ ecs_new_w_pair(world, EcsChildOf, p);
    ecs_entity_t gc1 = ecs_new_w_pair(world, EcsChildOf, c);
    ecs_entity_t gc2 = ecs_new_w_pair(world, EcsChildOf, c);
    ecs_add(world, gc2, Foo);
    ecs_entity_t gc3 = ecs_new_w_pair(world, EcsChildOf, c);
    ecs_add(world, gc3, Bar);

    ecs_iter_t it = ecs_query_iter(world, r);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(p, it.entities[0]);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(c, ecs_iter_get_var(&it, c_var));
    test_uint(gc1, ecs_iter_get_var(&it, gc_var));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 1));
    test_assert(!ecs_query_next(&it));

    ecs_query_fini(r);
    
    ecs_fini(world);
}

void Basic_2_trivial_mixed_2_tables(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Mass);
    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);
    ECS_TAG(world, Tag);

    ecs_entity_t ent = ecs_entity(world, { .name = "ent" });

    ecs_query_t *r = ecs_query(world, {
        .expr = "Foo(self), Bar(self), Mass(ent|self)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    ecs_entity_t e1 = ecs_new(world);
    ecs_add(world, e1, Foo);
    ecs_add(world, e1, Bar);

    ecs_entity_t e2 = ecs_new(world);
    ecs_add(world, e2, Foo);
    ecs_add(world, e2, Bar);

    ecs_entity_t e3 = ecs_new(world);
    ecs_add(world, e3, Foo);
    ecs_add(world, e3, Bar);
    ecs_add(world, e3, Tag);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_set(world, ent, Mass, {100});

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_uint(2, it.count);
        test_uint(e1, it.entities[0]);
        test_uint(e2, it.entities[1]);

        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(Bar, ecs_field_id(&it, 1));
        test_uint(ecs_id(Mass), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(ent, ecs_field_src(&it, 2));

        {
            Mass *m = ecs_field(&it, Mass, 2);
            test_assert(m != NULL);
            test_int(*m, 100);
        }

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(e3, it.entities[0]);

        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(Bar, ecs_field_id(&it, 1));
        test_uint(ecs_id(Mass), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(ent, ecs_field_src(&it, 2));

        {
            Mass *m = ecs_field(&it, Mass, 2);
            test_assert(m != NULL);
            test_int(*m, 100);
        }

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void Basic_2_trivial_mixed_2_tables_component(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_TAG(world, Tag);

    ecs_entity_t ent = ecs_entity(world, { .name = "ent" });

    ecs_query_t *r = ecs_query(world, {
        .expr = "Position(self), Velocity(self), Mass(ent|self)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    ecs_entity_t e1 = ecs_new(world);
    ecs_set(world, e1, Position, {10, 20});
    ecs_set(world, e1, Velocity, {1, 2});

    ecs_entity_t e2 = ecs_new(world);
    ecs_set(world, e2, Position, {20, 30});
    ecs_set(world, e2, Velocity, {2, 3});

    ecs_entity_t e3 = ecs_new(world);
    ecs_set(world, e3, Position, {30, 40});
    ecs_set(world, e3, Velocity, {3, 4});
    ecs_add(world, e3, Tag);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_set(world, ent, Mass, {100});

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_uint(2, it.count);
        test_uint(e1, it.entities[0]);
        test_uint(e2, it.entities[1]);

        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(ecs_id(Velocity), ecs_field_id(&it, 1));
        test_uint(ecs_id(Mass), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(ent, ecs_field_src(&it, 2));

        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p[0].x, 10); test_int(p[0].y, 20);
            test_int(p[1].x, 20); test_int(p[1].y, 30);

            Velocity *v = ecs_field(&it, Velocity, 1);
            test_assert(v != NULL);
            test_int(v[0].x, 1); test_int(v[0].y, 2);
            test_int(v[1].x, 2); test_int(v[1].y, 3);

            Mass *m = ecs_field(&it, Mass, 2);
            test_assert(m != NULL);
            test_int(*m, 100);
        }

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(e3, it.entities[0]);

        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(ecs_id(Velocity), ecs_field_id(&it, 1));
        test_uint(ecs_id(Mass), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(ent, ecs_field_src(&it, 2));

        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p[0].x, 30); test_int(p[0].y, 40);

            Velocity *v = ecs_field(&it, Velocity, 1);
            test_assert(v != NULL);
            test_int(v[0].x, 3); test_int(v[0].y, 4);

            Mass *m = ecs_field(&it, Mass, 2);
            test_assert(m != NULL);
            test_int(*m, 100);
        }

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void Basic_2_trivial_mixed_2_tables_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Mass);
    ECS_TAG(world, Foo);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);
    ECS_TAG(world, Tag);

    ecs_entity_t ent = ecs_entity(world, { .name = "ent" });

    ecs_query_t *r = ecs_query(world, {
        .expr = "Foo(self), ChildOf(self, *), Mass(ent|self)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    ecs_entity_t e1 = ecs_new(world);
    ecs_add(world, e1, Foo);
    ecs_add_pair(world, e1, EcsChildOf, TgtA);

    ecs_entity_t e2 = ecs_new(world);
    ecs_add(world, e2, Foo);
    ecs_add_pair(world, e2, EcsChildOf, TgtA);

    ecs_entity_t e3 = ecs_new(world);
    ecs_add(world, e3, Foo);
    ecs_add_pair(world, e3, EcsChildOf, TgtA);
    ecs_add(world, e3, Tag);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_set(world, ent, Mass, {100});

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_uint(2, it.count);
        test_uint(e1, it.entities[0]);
        test_uint(e2, it.entities[1]);

        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_pair(EcsChildOf, TgtA), ecs_field_id(&it, 1));
        test_uint(ecs_id(Mass), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(ent, ecs_field_src(&it, 2));

        {
            Mass *m = ecs_field(&it, Mass, 2);
            test_assert(m != NULL);
            test_int(*m, 100);
        }

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(e3, it.entities[0]);

        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_pair(EcsChildOf, TgtA), ecs_field_id(&it, 1));
        test_uint(ecs_id(Mass), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(ent, ecs_field_src(&it, 2));

        {
            Mass *m = ecs_field(&it, Mass, 2);
            test_assert(m != NULL);
            test_int(*m, 100);
        }

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void Basic_2_trivial_1_unused_id(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_entity_t e = ecs_entity(world, { .name = "e" });
    ecs_add(world, e, Foo);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Foo, Bar",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Basic_2_trivial_one_regular(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, TagC);

    ecs_entity_t e1 = ecs_new_w(world, TagA);
    ecs_add(world, e1, TagB);
    ecs_add(world, e1, TagC);

    ecs_entity_t e2 = ecs_new_w(world, TagA);
    ecs_add(world, e2, TagB);
    ecs_add_pair(world, e2, EcsChildOf, e1);

    ecs_query_t *q = ecs_query(world, {
        .expr = "TagA(self), TagB(self), TagC(self|up)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(e1, it.entities[0]);
        test_uint(TagA, ecs_field_id(&it, 0));
        test_uint(TagB, ecs_field_id(&it, 1));
        test_uint(TagC, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(e2, it.entities[0]);
        test_uint(TagA, ecs_field_id(&it, 0));
        test_uint(TagB, ecs_field_id(&it, 1));
        test_uint(TagC, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(e1, ecs_field_src(&it, 2));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Basic_1_trivial_one_regular_one_trivial(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, TagC);

    ecs_entity_t e1 = ecs_new_w(world, TagA);
    ecs_add(world, e1, TagB);
    ecs_add(world, e1, TagC);

    ecs_entity_t e2 = ecs_new_w(world, TagA);
    ecs_add_pair(world, e2, EcsChildOf, e1);
    ecs_add(world, e2, TagC);

    ecs_query_t *q = ecs_query(world, {
        .expr = "TagA(self), TagB(self|up), TagC(self)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(e1, it.entities[0]);
        test_uint(TagA, ecs_field_id(&it, 0));
        test_uint(TagB, ecs_field_id(&it, 1));
        test_uint(TagC, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(e2, it.entities[0]);
        test_uint(TagA, ecs_field_id(&it, 0));
        test_uint(TagB, ecs_field_id(&it, 1));
        test_uint(TagC, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Basic_one_regular_2_trivial(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, TagC);

    ecs_entity_t e1 = ecs_new_w(world, TagA);
    ecs_add(world, e1, TagB);
    ecs_add(world, e1, TagC);

    ecs_entity_t e2 = ecs_new_w(world, TagB);
    ecs_add(world, e2, TagC);
    ecs_add_pair(world, e2, EcsChildOf, e1);

    ecs_query_t *q = ecs_query(world, {
        .expr = "TagA(self|up), TagB(self), TagC(self)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(e1, it.entities[0]);
        test_uint(TagA, ecs_field_id(&it, 0));
        test_uint(TagB, ecs_field_id(&it, 1));
        test_uint(TagC, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(e2, it.entities[0]);
        test_uint(TagA, ecs_field_id(&it, 0));
        test_uint(TagB, ecs_field_id(&it, 1));
        test_uint(TagC, ecs_field_id(&it, 2));
        test_uint(e1, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Basic_2_trivial_w_prefab(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagC);

    ecs_entity_t e1 = ecs_new_w_id(world, EcsPrefab);
    ecs_add(world, e1, TagA);

    ecs_entity_t e2 = ecs_new_w_id(world, EcsPrefab);
    ecs_add(world, e2, TagA);
    ecs_add(world, e2, TagC);

    ecs_query_t *q = ecs_query(world, {
        .expr = "TagA(self), Prefab",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(e1, it.entities[0]);
        test_uint(TagA, ecs_field_id(&it, 0));
        test_uint(EcsPrefab, ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(e2, it.entities[0]);
        test_uint(TagA, ecs_field_id(&it, 0));
        test_uint(EcsPrefab, ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Basic_3_trivial_w_prefab(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, TagC);

    ecs_entity_t e1 = ecs_new_w_id(world, EcsPrefab);
    ecs_add(world, e1, TagA);
    ecs_add(world, e1, TagB);

    ecs_entity_t e2 = ecs_new_w_id(world, EcsPrefab);
    ecs_add(world, e2, TagA);
    ecs_add(world, e2, TagB);
    ecs_add(world, e2, TagC);

    ecs_entity_t e3 = ecs_new(world);
    ecs_add(world, e3, TagA);
    ecs_add(world, e3, TagB);

    ecs_query_t *q = ecs_query(world, {
        .expr = "TagA(self), TagB(self), Prefab",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(e1, it.entities[0]);
        test_uint(TagA, ecs_field_id(&it, 0));
        test_uint(TagB, ecs_field_id(&it, 1));
        test_uint(EcsPrefab, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(e2, it.entities[0]);
        test_uint(TagA, ecs_field_id(&it, 0));
        test_uint(TagB, ecs_field_id(&it, 1));
        test_uint(EcsPrefab, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Basic_2_trivial_w_disabled(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagC);

    ecs_entity_t e1 = ecs_new_w_id(world, EcsDisabled);
    ecs_add(world, e1, TagA);

    ecs_entity_t e2 = ecs_new_w_id(world, EcsDisabled);
    ecs_add(world, e2, TagA);
    ecs_add(world, e2, TagC);

    ecs_query_t *q = ecs_query(world, {
        .expr = "TagA(self), Disabled",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(e1, it.entities[0]);
        test_uint(TagA, ecs_field_id(&it, 0));
        test_uint(EcsDisabled, ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(e2, it.entities[0]);
        test_uint(TagA, ecs_field_id(&it, 0));
        test_uint(EcsDisabled, ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Basic_3_trivial_w_disabled(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, TagC);

    ecs_entity_t e1 = ecs_new_w_id(world, EcsDisabled);
    ecs_add(world, e1, TagA);
    ecs_add(world, e1, TagB);

    ecs_entity_t e2 = ecs_new_w_id(world, EcsDisabled);
    ecs_add(world, e2, TagA);
    ecs_add(world, e2, TagB);
    ecs_add(world, e2, TagC);

    ecs_entity_t e3 = ecs_new(world);
    ecs_add(world, e3, TagA);
    ecs_add(world, e3, TagB);

    ecs_query_t *q = ecs_query(world, {
        .expr = "TagA(self), TagB(self), Disabled",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(e1, it.entities[0]);
        test_uint(TagA, ecs_field_id(&it, 0));
        test_uint(TagB, ecs_field_id(&it, 1));
        test_uint(EcsDisabled, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(e2, it.entities[0]);
        test_uint(TagA, ecs_field_id(&it, 0));
        test_uint(TagB, ecs_field_id(&it, 1));
        test_uint(EcsDisabled, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Basic_2_this_w_fixed_src(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_entity_t e = ecs_entity(world, { .name = "e" });
    ecs_add(world, e, Bar);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Foo, Bar(e)",
        .cache_kind = cache_kind
    });

    ecs_entity_t e1 = ecs_new_w(world, Foo);
    ecs_entity_t e2 = ecs_new_w(world, Foo);

    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(2, it.count);
        test_uint(e1, it.entities[0]);
        test_uint(e2, it.entities[1]);

        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(Bar, ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(e, ecs_field_src(&it, 1));

        test_bool(false, ecs_query_next(&it)); 
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Basic_2_fixed_src_w_this(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_entity_t e = ecs_entity(world, { .name = "e" });
    ecs_add(world, e, Bar);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Bar(e), Foo",
        .cache_kind = cache_kind
    });

    ecs_entity_t e1 = ecs_new_w(world, Foo);
    ecs_entity_t e2 = ecs_new_w(world, Foo);

    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(2, it.count);
        test_uint(e1, it.entities[0]);
        test_uint(e2, it.entities[1]);

        test_uint(Bar, ecs_field_id(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 1));
        test_uint(e, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));

        test_bool(false, ecs_query_next(&it)); 
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Basic_2_this_w_fixed_src_no_match_fixed(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_entity(world, { .name = "e" });

    ecs_query_t *q = ecs_query(world, {
        .expr = "Foo, Bar(e)",
        .cache_kind = cache_kind
    });

    ecs_new_w(world, Foo);
    ecs_new_w(world, Foo);

    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Basic_2_fixed_src_w_this_no_match_fixed(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_entity(world, { .name = "e" });

    ecs_query_t *q = ecs_query(world, {
        .expr = "Bar(e), Foo",
        .cache_kind = cache_kind
    });

    ecs_new_w(world, Foo);
    ecs_new_w(world, Foo);

    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Basic_2_this_w_fixed_src_no_match_this(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_entity_t e = ecs_entity(world, { .name = "e" });
    ecs_add(world, e, Bar);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Foo, Bar(e)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Basic_2_fixed_src_w_this_no_match_this(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_entity_t e = ecs_entity(world, { .name = "e" });
    ecs_add(world, e, Bar);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Bar(e), Foo",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Basic_query_count_results(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_query_t *q = ecs_query(world, {
        .expr = "TagA",
        .cache_kind = cache_kind
    });
    test_assert(q != NULL);

    test_int(0, ecs_query_count(q).results);

    ecs_new_w(world, TagA);

    test_int(1, ecs_query_count(q).results);

    ecs_entity_t e = ecs_new_w(world, TagA);

    test_int(1, ecs_query_count(q).results);

    ecs_add(world, e, TagB);

    test_int(2, ecs_query_count(q).results);

    ecs_delete_with(world, TagB);

    test_int(1, ecs_query_count(q).results);

    ecs_delete_with(world, TagA);

    test_int(0, ecs_query_count(q).results);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Basic_query_count_entities(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_query_t *q = ecs_query(world, {
        .expr = "TagA",
        .cache_kind = cache_kind
    });
    test_assert(q != NULL);

    test_int(0, ecs_query_count(q).entities);

    ecs_entity_t e1 = ecs_new_w(world, TagA);

    test_int(1, ecs_query_count(q).entities);

    ecs_entity_t e2 = ecs_new_w(world, TagA);

    test_int(2, ecs_query_count(q).entities);

    ecs_add(world, e2, TagB);

    test_int(2, ecs_query_count(q).entities);

    ecs_delete(world, e1);

    test_int(1, ecs_query_count(q).entities);

    ecs_delete(world, e2);

    test_int(0, ecs_query_count(q).entities);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Basic_query_is_true(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_query_t *q = ecs_query(world, {
        .expr = "TagA",
        .cache_kind = cache_kind
    });
    test_assert(q != NULL);

    test_bool(false, ecs_query_is_true(q));

    ecs_entity_t e = ecs_new_w(world, TagA);

    test_bool(true, ecs_query_is_true(q));

    ecs_delete(world, e);

    test_bool(false, ecs_query_is_true(q));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Basic_implicit_cleanup_1_term(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_new_w(world, TagA);

    ecs_query_t *q = ecs_query(world, {
        .entity = ecs_new(world),
        .expr = "TagA",
        .cache_kind = cache_kind
    });
    test_assert(q != NULL);

    ecs_fini(world);
}

void Basic_implicit_cleanup_2_terms(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    
    ecs_add(world, ecs_new_w(world, TagA), TagB);

    ecs_query_t *q = ecs_query(world, {
        .entity = ecs_new(world),
        .expr = "TagA, TagB",
        .cache_kind = cache_kind
    });
    test_assert(q != NULL);

    ecs_fini(world);
}

void Basic_implicit_cleanup_1_term_w_up(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_new_w_pair(world, EcsChildOf, ecs_new_w(world, TagA));

    ecs_query_t *q = ecs_query(world, {
        .entity = ecs_new(world),
        .expr = "TagA(up)",
        .cache_kind = cache_kind
    });
    test_assert(q != NULL);

    ecs_fini(world);
}

void Basic_implicit_cleanup_2_terms_w_up(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_add(world, 
        ecs_new_w_pair(world, EcsChildOf, 
            ecs_new_w(world, TagB)), TagA);

    ecs_query_t *q = ecs_query(world, {
        .entity = ecs_new(world),
        .expr = "TagA, TagB(up)",
        .cache_kind = cache_kind
    });
    test_assert(q != NULL);

    ecs_fini(world);
}

void Basic_implicit_cleanup_2_queries(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TAG(world, Tag);

    ecs_new_w_pair(world, EcsIsA, ecs_new_w(world, Position));

    ecs_query_t *q_1 = ecs_query(world, {
        .expr = "Position",
        .cache_kind = cache_kind
    });
    test_assert(q_1 != NULL);

    ecs_query_t *q_2 = ecs_query(world, {
        .expr = "Position",
        .cache_kind = cache_kind
    });
    test_assert(q_2 != NULL);

    ecs_query_fini(q_1);
    ecs_query_fini(q_2);

    ecs_fini(world);
}

void Basic_implicit_cleanup_2_queries_1_cleanup(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TAG(world, Tag);
    
    ecs_new_w_pair(world, EcsIsA, ecs_new_w(world, Position));

    ecs_query_t *q_1 = ecs_query(world, {
        .expr = "Position",
        .cache_kind = cache_kind
    });
    test_assert(q_1 != NULL);

    ecs_query_t *q_2 = ecs_query(world, {
        .expr = "Position",
        .cache_kind = cache_kind
    });
    test_assert(q_2 != NULL);

    ecs_query_fini(q_1);
    ecs_query_fini(q_2);

    ecs_fini(world);
}

void Basic_iter_valid(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    
    ecs_new_w(world, Position);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Position",
        .cache_kind = cache_kind
    });
    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(it.flags & EcsIterIsValid, false);

    test_bool(ecs_query_next(&it), true);
    test_bool(it.flags & EcsIterIsValid, true);

    test_bool(ecs_query_next(&it), false);
    test_bool(it.flags & EcsIterIsValid, false);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Basic_iter_frame_offset(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, TagC);

    ecs_query_t *q = ecs_query(world, {
        .expr = "TagA",
        .cache_kind = cache_kind
    });

    ecs_entity_t e1 = ecs_new_w(world, TagA);
    ecs_entity_t e2 = ecs_new_w(world, TagA);
    ecs_entity_t e3 = ecs_new_w(world, TagA);
    ecs_entity_t e4 = ecs_new_w(world, TagA);
    ecs_entity_t e5 = ecs_new_w(world, TagA);

    ecs_add(world, e3, TagB);
    ecs_add(world, e4, TagB);
    ecs_add(world, e5, TagC);

    ecs_iter_t it = ecs_query_iter(world, q);

    test_bool(ecs_query_next(&it), true);
    test_int(it.count, 2);
    test_int(it.frame_offset, 0);
    test_assert(it.entities != NULL);
    test_assert(it.entities[0] == e1);
    test_assert(it.entities[1] == e2);
    test_assert(it.ids[0] == TagA);

    test_bool(ecs_query_next(&it), true);
    test_int(it.count, 2);
    test_int(it.frame_offset, 2);
    test_assert(it.entities != NULL);
    test_assert(it.entities[0] == e3);
    test_assert(it.entities[1] == e4);
    test_assert(it.ids[0] == TagA);

    test_bool(ecs_query_next(&it), true);
    test_int(it.count, 1);
    test_int(it.frame_offset, 4);
    test_assert(it.entities != NULL);
    test_assert(it.entities[0] == e5);
    test_assert(it.ids[0] == TagA);

    test_bool(ecs_query_next(&it), false);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Basic_iter_nested_1(void) {
    ecs_world_t* ecs = ecs_mini();

    ECS_TAG(ecs, Tag);
    ECS_TAG(ecs, Singleton);

    ecs_query_t *qa = ecs_query(ecs, {
        .terms = {
            {Singleton, .src.id = Singleton}, {Tag}
        },
        .cache_kind = EcsQueryCacheAuto
    });

    ecs_query_t *qb = ecs_query(ecs, {
        .terms = {{Tag}},
        .cache_kind = cache_kind
    });

    ecs_singleton_add(ecs, Singleton);
    ecs_entity_t e = ecs_new_w(ecs, Tag);

    ecs_iter_t qit = ecs_query_iter(ecs, qa);
    test_bool(true, ecs_query_next(&qit));
    test_int(1, qit.count);
    test_uint(e, qit.entities[0]);

    ecs_id_t id = qit.ids[0];
    ecs_iter_t rit = ecs_query_iter(ecs, qb);
    test_bool(true, ecs_query_next(&rit));
    test_int(1, rit.count);
    test_uint(e, rit.entities[0]);
    test_bool(false, ecs_query_next(&rit));
    test_assert(id == qit.ids[0]);
    test_bool(false, ecs_query_next(&qit));

    ecs_query_fini(qa);
    ecs_query_fini(qb);

    ecs_fini(ecs);
}

void Basic_iter_nested_2(void) {
    ecs_world_t* ecs = ecs_mini();

    ECS_TAG(ecs, TagA);
    ECS_TAG(ecs, TagB);
    ECS_TAG(ecs, Singleton);

    ecs_query_t *qa = ecs_query(ecs, {
        .terms = {
            {TagA}, {Singleton, .src.id = Singleton}
        },
        .cache_kind = cache_kind
    });

    ecs_query_t *qb = ecs_query(ecs, {
        .terms = {
            {TagB}, {Singleton, .src.id = Singleton}
        },
        .cache_kind = cache_kind
    });

    ecs_singleton_add(ecs, Singleton);

    ecs_entity_t ea = ecs_new_w(ecs, TagA);
    ecs_entity_t eb = ecs_new_w(ecs, TagB);

    ecs_iter_t qita = ecs_query_iter(ecs, qa);
    ecs_iter_t qitb = ecs_query_iter(ecs, qb);

    test_bool(ecs_query_next(&qita), true);
    test_int(1, qita.count);
    test_uint(ea, qita.entities[0]);

    test_bool(ecs_query_next(&qitb), true);
    test_int(1, qitb.count);
    test_uint(eb, qitb.entities[0]);

    test_uint(TagA, qita.ids[0]);
    test_uint(TagB, qitb.ids[0]);

    test_bool(ecs_query_next(&qita), false);
    test_bool(ecs_query_next(&qitb), false);

    ecs_query_fini(qa);
    ecs_query_fini(qb);

    ecs_fini(ecs);
}

void Basic_iter_interleaved(void) {
    ecs_world_t* ecs = ecs_mini();

    ECS_TAG(ecs, TagA);
    ECS_TAG(ecs, TagB);
    ECS_TAG(ecs, TagC);
    ECS_TAG(ecs, Singleton);

    ecs_query_t *qa = ecs_query(ecs, {
        .terms = {
            {TagA}, {Singleton, .src.id = Singleton}
        },
        .cache_kind = cache_kind
    });

    ecs_query_t *qb = ecs_query(ecs, {
        .terms = {
            {TagB}, {Singleton, .src.id = Singleton}
        },
        .cache_kind = cache_kind
    });

    ecs_query_t *qc = ecs_query(ecs, {
        .terms = {
            {TagC}, {Singleton, .src.id = Singleton}
        },
        .cache_kind = cache_kind
    });

    ecs_singleton_add(ecs, Singleton);

    ecs_entity_t ea = ecs_new_w(ecs, TagA);
    ecs_entity_t eb = ecs_new_w(ecs, TagB);
    ecs_entity_t ec = ecs_new_w(ecs, TagC);

    ecs_iter_t qita = ecs_query_iter(ecs, qa);
    ecs_iter_t qitb = ecs_query_iter(ecs, qb);

    test_bool(ecs_query_next(&qita), true);
    test_int(1, qita.count);
    test_uint(ea, qita.entities[0]);

    test_bool(ecs_query_next(&qitb), true);
    test_int(1, qitb.count);
    test_uint(eb, qitb.entities[0]);

    test_uint(TagA, qita.ids[0]);
    test_uint(TagB, qitb.ids[0]);

    test_bool(ecs_query_next(&qita), false);

    ecs_iter_t qitc = ecs_query_iter(ecs, qc);
    test_bool(ecs_query_next(&qitc), true);
    test_int(1, qitc.count);
    test_uint(ec, qitc.entities[0]);

    test_uint(TagB, qitb.ids[0]);
    test_uint(TagC, qitc.ids[0]);

    test_bool(ecs_query_next(&qitb), false);
    test_bool(ecs_query_next(&qitc), false);

    ecs_query_fini(qa);
    ecs_query_fini(qb);
    ecs_query_fini(qc);

    ecs_fini(ecs);
}

void Basic_set_get_context(void) {
    ecs_world_t* ecs = ecs_mini();

    ECS_COMPONENT(ecs, Position);

    int ctx = 0;

    ecs_query_t *q = ecs_query(ecs, {
        .terms = {
            { .id = ecs_id(Position) }
        },
        .ctx = &ctx,
        .cache_kind = cache_kind
    });
    test_assert(q != NULL);

    test_assert(q->ctx == &ctx);
    test_assert(q->binding_ctx == NULL);

    ecs_query_fini(q);

    ecs_fini(ecs);
}

void Basic_set_get_binding_context(void) {
    ecs_world_t* ecs = ecs_mini();

    ECS_COMPONENT(ecs, Position);

    int ctx = 0;

    ecs_query_t *q = ecs_query(ecs, {
        .terms = {
            { .id = ecs_id(Position) }
        },
        .binding_ctx = &ctx,
        .cache_kind = cache_kind
    });
    test_assert(q != NULL);

    test_assert(q->binding_ctx == &ctx);
    test_assert(q->ctx == NULL);

    ecs_query_fini(q);

    ecs_fini(ecs);
}

static int ctx_free_invoked = 0;

static void ctx_free(void *ptr) {
    *(int*)ptr = 10;
    ctx_free_invoked ++;
}

void Basic_set_get_context_w_free(void) {
    ecs_world_t* ecs = ecs_mini();

    ECS_COMPONENT(ecs, Position);

    int ctx = 0;

    ecs_query_t *q = ecs_query(ecs, {
        .terms = {
            { .id = ecs_id(Position) }
        },
        .ctx = &ctx,
        .ctx_free = ctx_free,
        .cache_kind = cache_kind
    });
    test_assert(q != NULL);

    test_assert(q->ctx == &ctx);
    test_assert(q->binding_ctx == NULL);
    test_int(ctx, 0);

    test_int(ctx_free_invoked, 0);
    ecs_query_fini(q);
    test_int(ctx_free_invoked, 1);

    test_int(ctx, 10);

    ecs_fini(ecs);
}

void Basic_set_get_binding_context_w_free(void) {
    ecs_world_t* ecs = ecs_mini();

    ECS_COMPONENT(ecs, Position);

    int ctx = 0;

    ecs_query_t *q = ecs_query(ecs, {
        .terms = {
            { .id = ecs_id(Position) }
        },
        .binding_ctx = &ctx,
        .binding_ctx_free = ctx_free,
        .cache_kind = cache_kind
    });
    test_assert(q != NULL);

    test_assert(q->binding_ctx == &ctx);
    test_assert(q->ctx == NULL);
    test_int(ctx, 0);

    test_int(ctx_free_invoked, 0);
    ecs_query_fini(q);
    test_int(ctx_free_invoked, 1);

    test_int(ctx, 10);

    ecs_fini(ecs);
}

void Basic_create_query_w_existing_entity(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    if (cache_kind == EcsQueryCacheDefault) {
        /* Make sure we also test uncached, as the default will create a cached
         * query if an entity id is provided. */
        cache_kind = EcsQueryCacheNone;
    }

    ecs_entity_t e = ecs_entity(world, { .name = "q" });

    ecs_query_t *q_1 = ecs_query(world, {
        .entity = e,
        .terms = {
            { .id = Foo },
        },
        .cache_kind = cache_kind
    });

    test_assert(q_1 != NULL);

    ecs_query_t *q_2 = ecs_query(world, {
        .entity = e,
        .terms = {
            { .id = Foo },
        },
        .cache_kind = cache_kind
    });

    test_assert(q_2 != NULL);
    
    test_str(ecs_get_name(world, e), "q");

    ecs_query_fini(q_2);

    ecs_fini(world);
}

void Basic_create_multi_component_query_w_existing_entity(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    if (cache_kind == EcsQueryCacheDefault) {
        /* Make sure we also test uncached, as the default will create a cached
         * query if an entity id is provided. */
        cache_kind = EcsQueryCacheNone;
    }

    ecs_entity_t e = ecs_entity(world, { .name = "q" });

    ecs_query_t *q_1 = ecs_query(world, {
        .entity = e,
        .terms = {
            { .id = Foo },
            { .id = Bar }
        },
        .cache_kind = cache_kind
    });

    test_assert(q_1 != NULL);

    ecs_query_t *q_2 = ecs_query(world, {
        .entity = e,
        .terms = {
            { .id = Foo },
            { .id = Bar },
        },
        .cache_kind = cache_kind
    });

    test_assert(q_2 != NULL);
    
    test_str(ecs_get_name(world, e), "q");

    ecs_query_fini(q_2);

    ecs_fini(world);
}

void Basic_create_query_w_existing_entity_different_term_count(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    if (cache_kind == EcsQueryCacheDefault) {
        /* Make sure we also test uncached, as the default will create a cached
         * query if an entity id is provided. */
        cache_kind = EcsQueryCacheNone;
    }

    ecs_entity_t e = ecs_entity(world, { .name = "q" });

    ecs_query_t *q_1 = ecs_query(world, {
        .entity = e,
        .terms = {
            { .id = Foo },
            { .id = Bar }
        },
        .cache_kind = cache_kind
    });

    test_assert(q_1 != NULL);

    ecs_query_t *q_2 = ecs_query(world, {
        .entity = e,
        .terms = {
            { .id = Foo },
        },
        .cache_kind = cache_kind
    });

    test_assert(q_2 != NULL);
    
    test_str(ecs_get_name(world, e), "q");

    ecs_query_fini(q_2);

    ecs_fini(world);
}

void Basic_delete_query_by_entity(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_entity_t e = ecs_entity(world, { .name = "q" });
    ecs_query_t *q = ecs_query(world, {
        .entity = e,
        .expr = "Foo",
        .cache_kind = cache_kind
    });
    test_assert(q != NULL);

    ecs_delete(world, e);

    ecs_fini(world);
}

void Basic_eval_count(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ Foo }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    test_int(q->eval_count, 0);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(false, ecs_query_next(&it));

    test_int(q->eval_count, 1);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Basic_no_results_after_delete_tree(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t parent = ecs_new_w(world, Position);
    test_assert(parent != 0);

    ecs_entity_t child = ecs_new_w_pair(world, EcsChildOf, parent);
    test_assert(ecs_get_type(world, child) != NULL);
    ecs_add(world, child, Position);

    ecs_entity_t grand_child = ecs_new_w_pair(world, EcsChildOf, child);
    test_assert(ecs_get_type(world, grand_child) != NULL);
    ecs_add(world, grand_child, Position);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Position",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    test_int(3, ecs_query_count(q).results);

    ecs_delete(world, parent);
    
    test_bool(ecs_is_alive(world, parent), false);
    test_bool(ecs_is_alive(world, child), false);
    test_bool(ecs_is_alive(world, grand_child), false);

    test_int(0, ecs_query_count(q).results);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Basic_no_results_after_delete_tree_deferred(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t parent = ecs_new_w(world, Position);
    test_assert(parent != 0);

    ecs_entity_t child = ecs_new_w_pair(world, EcsChildOf, parent);
    test_assert(ecs_get_type(world, child) != NULL);
    ecs_add(world, child, Position);

    ecs_entity_t grand_child = ecs_new_w_pair(world, EcsChildOf, child);
    test_assert(ecs_get_type(world, grand_child) != NULL);
    ecs_add(world, grand_child, Position);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Position",
        .cache_kind = cache_kind
    });

    test_int(3, ecs_query_count(q).results);

    ecs_defer_begin(world);
    ecs_delete(world, parent);
    ecs_defer_end(world);
    
    test_bool(ecs_is_alive(world, parent), false);
    test_bool(ecs_is_alive(world, child), false);
    test_bool(ecs_is_alive(world, grand_child), false);

    test_int(0, ecs_query_count(q).results);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Basic_add_on_self_ref(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);

    ecs_entity_t qe = ecs_new(world);

    ecs_query_t *q = ecs_query(world, {
        .entity = qe,
        .terms = {{ TagA, .src.id = qe }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    test_assert(ecs_has(world, qe, TagA));
    test_bool(true, ecs_query_is_true(q));

    ecs_fini(world);
}

void Basic_add_on_self_ref_by_name(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);

    ecs_entity_t qe = ecs_entity(world, { .name = "q" });

    ecs_query_t *q = ecs_query(world, {
        .entity = qe,
        .expr = "TagA(q)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    test_assert(ecs_has(world, qe, TagA));
    test_bool(true, ecs_query_is_true(q));

    ecs_fini(world);
}

void Basic_delete_id_after_delete_query(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t i = ecs_new(world);
    ecs_make_alive(world, i);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ .id = i }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    test_int(ecs_query_count(q).entities, 0);
    ecs_entity_t e = ecs_new_w_id(world, i);
    test_int(ecs_query_count(q).entities, 1);
    ecs_delete(world, e);
    test_int(ecs_query_count(q).entities, 0);

    ecs_query_fini(q);

    ecs_delete(world, i);

    ecs_fini(world);
}

void Basic_pair_sweep_tag(void) {
    ecs_world_t *world = ecs_mini();

    for (ecs_entity_t i = 1023; i < 4097; i ++) {
        ecs_entity_t i = 1024;
        test_assert(!ecs_is_alive(world, i));
        ecs_make_alive(world, i);

        ecs_query_t *q = ecs_query(world, {
            .terms = {{ .id = i }},
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        test_int(ecs_query_count(q).entities, 0);
        ecs_entity_t e = ecs_new_w_id(world, i);
        test_int(ecs_query_count(q).entities, 1);
        ecs_delete(world, e);
        test_int(ecs_query_count(q).entities, 0);

        ecs_query_fini(q);

        ecs_delete(world, i);
    }

    ecs_fini(world);
}

void Basic_pair_sweep_wildcard_first(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);

    for (ecs_entity_t i = 1023; i < 4097; i ++) {
        test_assert(!ecs_is_alive(world, i));
        ecs_make_alive(world, i);

        ecs_query_t *q = ecs_query(world, {
            .terms = {{ .id = ecs_pair(EcsWildcard, i) }},
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        test_int(ecs_query_count(q).entities, 0);
        ecs_entity_t e = ecs_new_w_pair(world, Rel, i);
        test_int(ecs_query_count(q).entities, 1);
        ecs_delete(world, e);
        test_int(ecs_query_count(q).entities, 0);

        ecs_query_fini(q);

        ecs_delete(world, i);
    }

    ecs_fini(world);
}

void Basic_pair_sweep_wildcard_second(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tgt);

    for (ecs_entity_t i = 1023; i < 4097; i ++) {
        test_assert(!ecs_is_alive(world, i));
        ecs_make_alive(world, i);

        ecs_query_t *q = ecs_query(world, {
            .terms = {{ .id = ecs_pair(i, EcsWildcard) }}
        });

        test_assert(q != NULL);

        test_int(ecs_query_count(q).entities, 0);
        ecs_entity_t e = ecs_new_w_pair(world, i, Tgt);
        test_int(ecs_query_count(q).entities, 1);
        ecs_delete(world, e);
        test_int(ecs_query_count(q).entities, 0);

        ecs_query_fini(q);

        ecs_delete(world, i);
    }

    ecs_fini(world);
}

void Basic_create_w_entity_deferred(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_entity_t qe = ecs_new(world);

    ecs_defer_begin(world);
    ecs_query_t *q = ecs_query(world, {
        .entity = qe,
        .terms = {{ Foo }}
    });
    test_assert(q != NULL);
    ecs_defer_end(world);

    test_assert(ecs_has_pair(world, qe, ecs_id(EcsPoly), EcsQuery));

    const EcsPoly *p = ecs_get_pair(world, qe, EcsPoly, EcsQuery);
    test_assert(p != NULL);
    test_assert(p->poly == q);

    ecs_entity_t e = ecs_new_w(world, Foo);
    
    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e, it.entities[0]);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_bool(false, ecs_query_next(&it));

    ecs_fini(world);
}

void Basic_32_terms(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag0);
    ECS_TAG(world, Tag1);
    ECS_TAG(world, Tag2);
    ECS_TAG(world, Tag3);
    ECS_TAG(world, Tag4);
    ECS_TAG(world, Tag5);
    ECS_TAG(world, Tag6);
    ECS_TAG(world, Tag7);
    ECS_TAG(world, Tag8);
    ECS_TAG(world, Tag9);
    ECS_TAG(world, Tag10);
    ECS_TAG(world, Tag11);
    ECS_TAG(world, Tag12);
    ECS_TAG(world, Tag13);
    ECS_TAG(world, Tag14);
    ECS_TAG(world, Tag15);
    ECS_TAG(world, Tag16);
    ECS_TAG(world, Tag17);
    ECS_TAG(world, Tag18);
    ECS_TAG(world, Tag19);
    ECS_TAG(world, Tag20);
    ECS_TAG(world, Tag21);
    ECS_TAG(world, Tag22);
    ECS_TAG(world, Tag23);
    ECS_TAG(world, Tag24);
    ECS_TAG(world, Tag25);
    ECS_TAG(world, Tag26);
    ECS_TAG(world, Tag27);
    ECS_TAG(world, Tag28);
    ECS_TAG(world, Tag29);
    ECS_TAG(world, Tag30);
    ECS_TAG(world, Tag31);
    ECS_TAG(world, Tag32);

    ecs_entity_t e = ecs_new(world);
    ecs_add(world, e, Tag0);
    ecs_add(world, e, Tag1);
    ecs_add(world, e, Tag2);
    ecs_add(world, e, Tag3);
    ecs_add(world, e, Tag4);
    ecs_add(world, e, Tag5);
    ecs_add(world, e, Tag6);
    ecs_add(world, e, Tag7);
    ecs_add(world, e, Tag8);
    ecs_add(world, e, Tag9);
    ecs_add(world, e, Tag10);
    ecs_add(world, e, Tag11);
    ecs_add(world, e, Tag12);
    ecs_add(world, e, Tag13);
    ecs_add(world, e, Tag14);
    ecs_add(world, e, Tag15);
    ecs_add(world, e, Tag16);
    ecs_add(world, e, Tag17);
    ecs_add(world, e, Tag18);
    ecs_add(world, e, Tag19);
    ecs_add(world, e, Tag20);
    ecs_add(world, e, Tag21);
    ecs_add(world, e, Tag22);
    ecs_add(world, e, Tag23);
    ecs_add(world, e, Tag24);
    ecs_add(world, e, Tag25);
    ecs_add(world, e, Tag26);
    ecs_add(world, e, Tag27);
    ecs_add(world, e, Tag28);
    ecs_add(world, e, Tag29);
    ecs_add(world, e, Tag30);
    ecs_add(world, e, Tag31);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Tag0, Tag1, Tag2, Tag3, Tag4, Tag5, Tag6, Tag7, Tag8, Tag9, Tag10, Tag11, Tag12, Tag13, Tag14, Tag15, Tag16, Tag17, Tag18, Tag19, Tag20, Tag21, Tag22, Tag23, Tag24, Tag25, Tag26, Tag27, Tag28, Tag29, Tag30, Tag31"
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_iter_next(&it));
    test_int(1, it.count);
    test_int(32, it.field_count);
    test_uint(e, it.entities[0]);
    test_bool(false, ecs_iter_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Basic_33_terms_expr(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag0);
    ECS_TAG(world, Tag1);
    ECS_TAG(world, Tag2);
    ECS_TAG(world, Tag3);
    ECS_TAG(world, Tag4);
    ECS_TAG(world, Tag5);
    ECS_TAG(world, Tag6);
    ECS_TAG(world, Tag7);
    ECS_TAG(world, Tag8);
    ECS_TAG(world, Tag9);
    ECS_TAG(world, Tag10);
    ECS_TAG(world, Tag11);
    ECS_TAG(world, Tag12);
    ECS_TAG(world, Tag13);
    ECS_TAG(world, Tag14);
    ECS_TAG(world, Tag15);
    ECS_TAG(world, Tag16);
    ECS_TAG(world, Tag17);
    ECS_TAG(world, Tag18);
    ECS_TAG(world, Tag19);
    ECS_TAG(world, Tag20);
    ECS_TAG(world, Tag21);
    ECS_TAG(world, Tag22);
    ECS_TAG(world, Tag23);
    ECS_TAG(world, Tag24);
    ECS_TAG(world, Tag25);
    ECS_TAG(world, Tag26);
    ECS_TAG(world, Tag27);
    ECS_TAG(world, Tag28);
    ECS_TAG(world, Tag29);
    ECS_TAG(world, Tag30);
    ECS_TAG(world, Tag31);
    ECS_TAG(world, Tag32);

    ecs_log_set_level(-4);
    ecs_query_t *q = ecs_query(world, {
        .expr = "Tag0, Tag1, Tag2, Tag3, Tag4, Tag5, Tag6, Tag7, Tag8, Tag9, Tag10, Tag11, Tag12, Tag13, Tag14, Tag15, Tag16, Tag17, Tag18, Tag19, Tag20, Tag21, Tag22, Tag23, Tag24, Tag25, Tag26, Tag27, Tag28, Tag29, Tag30, Tag31, Tag32"
    });

    test_assert(q == NULL);

    ecs_fini(world);
}

void Basic_stage_query(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new_w(world, Position);

    ecs_world_t *stage = ecs_get_stage(world, 0);

    ecs_query_t *q = ecs_query(stage, {
        .expr = "Position",
        .cache_kind = cache_kind
    });
    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_assert(it.world == stage);
    test_assert(it.real_world == world);
    test_int(it.count, 1);
    test_uint(it.entities[0], e);
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Basic_world_query_w_stage_iter(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new_w(world, Position);

    ecs_world_t *stage = ecs_get_stage(world, 0);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Position",
        .cache_kind = cache_kind
    });
    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(stage, q);
    test_bool(true, ecs_query_next(&it));
    test_assert(it.world == stage);
    test_assert(it.real_world == world);
    test_int(it.count, 1);
    test_uint(it.entities[0], e);
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Basic_stage_query_w_nth_stage(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new_w(world, Position);

    ecs_set_stage_count(world, 2);
    ecs_world_t *stage = ecs_get_stage(world, 1);

    ecs_query_t *q = ecs_query(stage, {
        .expr = "Position",
        .cache_kind = cache_kind
    });
    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_assert(it.world == stage);
    test_assert(it.real_world == world);
    test_int(it.count, 1);
    test_uint(it.entities[0], e);
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Basic_world_query_w_nth_stage_iter(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new_w(world, Position);

    ecs_set_stage_count(world, 2);
    ecs_world_t *stage = ecs_get_stage(world, 1);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Position",
        .cache_kind = cache_kind
    });
    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(stage, q);
    test_bool(true, ecs_query_next(&it));
    test_assert(it.world == stage);
    test_assert(it.real_world == world);
    test_int(it.count, 1);
    test_uint(it.entities[0], e);
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Basic_match_empty(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_entity_t e1 = ecs_new_w(world, Foo);
    ecs_table_t *e1_table = ecs_get_table(world, e1);

    ecs_entity_t e2 = ecs_new_w(world, Foo);
    ecs_add(world, e2, Bar);
    ecs_table_t *e2_table = ecs_get_table(world, e2);
    ecs_delete(world, e2); // creates empty [Foo, Bar]

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ Foo }},
        .flags = EcsQueryMatchEmptyTables,
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(it.count, 0);
        test_assert(it.table == e2_table);

        test_bool(true, ecs_query_next(&it));
        test_int(it.count, 1);
        test_assert(it.table == e1_table);
        test_uint(it.entities[0], e1);

        test_bool(false, ecs_query_next(&it));
    }

    ecs_delete(world, e1); // creates empty [Foo]

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(it.count, 0);
        test_assert(it.table == e2_table);

        test_bool(true, ecs_query_next(&it));
        test_assert(it.table == e1_table);
        test_int(it.count, 0);

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Basic_match_new_empty(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_entity_t e1 = ecs_new_w(world, Foo);
    ecs_table_t *e1_table = ecs_get_table(world, e1);

    ecs_entity_t e2 = ecs_new_w(world, Foo);
    ecs_add(world, e2, Bar);
    ecs_table_t *e2_table = ecs_get_table(world, e2);
    ecs_delete(world, e2); // creates empty [Foo, Bar]

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ Foo }},
        .flags = EcsQueryMatchEmptyTables,
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(it.count, 0);
        test_assert(it.table == e2_table);

        test_bool(true, ecs_query_next(&it));
        test_int(it.count, 1);
        test_assert(it.table == e1_table);
        test_uint(it.entities[0], e1);

        test_bool(false, ecs_query_next(&it));
    }

    ecs_delete(world, e1); // creates empty [Foo]

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(it.count, 0);
        test_assert(it.table == e2_table);

        test_bool(true, ecs_query_next(&it));
        test_int(it.count, 0);
        test_assert(it.table == e1_table);

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Basic_match_empty_w_component(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Bar);

    ecs_entity_t e1 = ecs_new_w(world, Position);
    ecs_table_t *e1_table = ecs_get_table(world, e1);

    ecs_entity_t e2 = ecs_new_w(world, Position);
    ecs_add(world, e2, Bar);
    ecs_table_t *e2_table = ecs_get_table(world, e2);
    ecs_delete(world, e2); // creates empty [Position, Bar]

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ ecs_id(Position) }},
        .flags = EcsQueryMatchEmptyTables,
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(it.count, 0);
        test_assert(it.table == e2_table);

        test_bool(true, ecs_query_next(&it));
        test_int(it.count, 1);
        test_assert(it.table == e1_table);
        test_uint(it.entities[0], e1);
        test_assert(ecs_field(&it, Position, 0) != NULL);

        test_bool(false, ecs_query_next(&it));
    }

    ecs_delete(world, e1); // creates empty [Position]

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(it.count, 0);
        test_assert(it.table == e2_table);

        test_bool(true, ecs_query_next(&it));
        test_assert(it.table == e1_table);
        test_int(it.count, 0);

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Basic_match_new_empty_w_component(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Bar);

    ecs_entity_t e1 = ecs_new_w(world, Position);
    ecs_table_t *e1_table = ecs_get_table(world, e1);

    ecs_entity_t e2 = ecs_new_w(world, Position);
    ecs_add(world, e2, Bar);
    ecs_table_t *e2_table = ecs_get_table(world, e2);
    ecs_delete(world, e2); // creates empty [Position, Bar]

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ ecs_id(Position) }},
        .flags = EcsQueryMatchEmptyTables,
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(it.count, 0);
        test_assert(it.table == e2_table);

        test_bool(true, ecs_query_next(&it));
        test_int(it.count, 1);
        test_assert(it.table == e1_table);
        test_uint(it.entities[0], e1);
        test_assert(ecs_field(&it, Position, 0) != NULL);

        test_bool(false, ecs_query_next(&it));
    }

    ecs_delete(world, e1); // creates empty [Position]

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(it.count, 0);
        test_assert(it.table == e2_table);

        test_bool(true, ecs_query_next(&it));
        test_int(it.count, 0);
        test_assert(it.table == e1_table);

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Basic_match_empty_w_ref(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Bar);

    ecs_entity_t p = ecs_new_w(world, Position);

    ecs_entity_t e1 = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_table_t *e1_table = ecs_get_table(world, e1);

    ecs_entity_t e2 = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_add(world, e2, Bar);
    ecs_table_t *e2_table = ecs_get_table(world, e2);
    ecs_delete(world, e2); // creates empty [Bar, (ChildOf, p)]

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ ecs_id(Position), .src.id = EcsUp, .trav = EcsChildOf }},
        .flags = EcsQueryMatchEmptyTables,
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(it.count, 0);
        test_assert(it.table == e2_table);

        test_bool(true, ecs_query_next(&it));
        test_int(it.count, 1);
        test_assert(it.table == e1_table);
        test_uint(it.entities[0], e1);
        test_assert(ecs_field(&it, Position, 0) != NULL);

        test_bool(false, ecs_query_next(&it));
    }

    ecs_delete(world, e1); // creates empty [(ChildOf, p)]

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(it.count, 0);
        test_assert(it.table == e2_table);

        test_bool(true, ecs_query_next(&it));
        test_assert(it.table == e1_table);
        test_int(it.count, 0);

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Basic_match_new_empty_w_ref(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Bar);

    ecs_entity_t p = ecs_new_w(world, Position);

    ecs_entity_t e1 = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_table_t *e1_table = ecs_get_table(world, e1);

    ecs_entity_t e2 = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_add(world, e2, Bar);
    ecs_table_t *e2_table = ecs_get_table(world, e2);
    ecs_delete(world, e2); // creates empty [Bar, (ChildOf, p)]

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ ecs_id(Position), .src.id = EcsUp, .trav = EcsChildOf }},
        .flags = EcsQueryMatchEmptyTables,
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(it.count, 0);
        test_assert(it.table == e2_table);

        test_bool(true, ecs_query_next(&it));
        test_int(it.count, 1);
        test_assert(it.table == e1_table);
        test_uint(it.entities[0], e1);
        test_assert(ecs_field(&it, Position, 0) != NULL);

        test_bool(false, ecs_query_next(&it));
    }

    ecs_delete(world, e1); // creates empty [(ChildOf, p)]

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_assert(it.table == e2_table);
        test_int(it.count, 0);

        test_bool(true, ecs_query_next(&it));
        test_int(it.count, 0);
        test_assert(it.table == e1_table);

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

static
int compare_position(
    ecs_entity_t e1,
    const void *ptr1,
    ecs_entity_t e2,
    const void *ptr2)
{
    const Position *p1 = ptr1;
    const Position *p2 = ptr2;
    return (p1->x > p2->x) - (p1->x < p2->x);
}

void Basic_match_empty_w_order_by(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Tag);

    ecs_entity_t e1 = ecs_insert(world, ecs_value(Position, {3, 0}));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(Position, {1, 0}));
    ecs_entity_t e3 = ecs_insert(world, ecs_value(Position, {5, 0}));
    ecs_entity_t e4 = ecs_insert(world, ecs_value(Position, {2, 0}));
    ecs_entity_t e5 = ecs_insert(world, ecs_value(Position, {4, 0}));

    ecs_add(world, e5, Tag);
    ecs_remove(world, e5, Tag);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "Position",
        .order_by = ecs_id(Position),
        .order_by_callback = compare_position,
        .flags = EcsQueryMatchEmptyTables,
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);

        test_assert(ecs_query_next(&it));
        test_int(it.count, 5);

        test_assert(it.entities[0] == e2);
        test_assert(it.entities[1] == e4);
        test_assert(it.entities[2] == e1);
        test_assert(it.entities[3] == e5);
        test_assert(it.entities[4] == e3);

        test_assert(!ecs_query_next(&it));
    }

    ecs_add(world, e5, Tag);

    {
        ecs_iter_t it = ecs_query_iter(world, q);

        test_assert(ecs_query_next(&it));
        test_int(it.count, 3);
        test_assert(it.entities[0] == e2);
        test_assert(it.entities[1] == e4);
        test_assert(it.entities[2] == e1);

        test_assert(ecs_query_next(&it));
        test_int(it.count, 1);
        test_assert(it.entities[0] == e5);

        test_assert(ecs_query_next(&it));
        test_int(it.count, 1);
        test_assert(it.entities[0] == e3);

        test_assert(!ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Basic_match_new_empty_w_order_by(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Tag);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "Position",
        .order_by = ecs_id(Position),
        .order_by_callback = compare_position,
        .flags = EcsQueryMatchEmptyTables,
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_entity_t e1 = ecs_insert(world, ecs_value(Position, {3, 0}));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(Position, {1, 0}));
    ecs_entity_t e3 = ecs_insert(world, ecs_value(Position, {5, 0}));
    ecs_entity_t e4 = ecs_insert(world, ecs_value(Position, {2, 0}));
    ecs_entity_t e5 = ecs_insert(world, ecs_value(Position, {4, 0}));

    ecs_add(world, e5, Tag);
    ecs_remove(world, e5, Tag);

    ecs_iter_t it = ecs_query_iter(world, q);

    test_assert(ecs_query_next(&it));
    test_int(it.count, 5);

    test_assert(it.entities[0] == e2);
    test_assert(it.entities[1] == e4);
    test_assert(it.entities[2] == e1);
    test_assert(it.entities[3] == e5);
    test_assert(it.entities[4] == e3);

    test_assert(!ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Basic_match_empty_w_bitset(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Tag);

    ecs_add_id(world, ecs_id(Position), EcsCanToggle);

    ecs_query_t *q = ecs_query(world, { 
        .terms = {{ ecs_id(Position) }},
        .flags = EcsQueryMatchEmptyTables,
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_entity_t e1 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(Position, {20, 30}));
    ecs_entity_t e3 = ecs_insert(world, ecs_value(Position, {30, 40}));

    ecs_enable_component(world, e1, Position, true);
    ecs_enable_component(world, e2, Position, false);
    ecs_enable_component(world, e3, Position, true);

    ecs_add(world, e3, Tag);
    ecs_remove(world, e3, Tag);

    ecs_iter_t it = ecs_query_iter(world, q);
    {
        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e1, it.entities[0]);
        Position *p = ecs_field(&it, Position, 0);
        test_int(p->x, 10); test_int(p->y, 20);
    }
    {
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e3, it.entities[0]);
        Position *p = ecs_field(&it, Position, 0);
        test_int(p->x, 30); test_int(p->y, 40);
    }
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Basic_default_query_flags(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Foo);

    ecs_set_default_query_flags(world, EcsQueryMatchEmptyTables);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ecs_id(Position)}},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);
    test_assert(q->flags & EcsQueryMatchEmptyTables);

    ecs_entity_t e1 = ecs_new_w(world, Position);
    ecs_entity_t e2 = ecs_new_w(world, Position);

    ecs_add(world, e2, Foo);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e2, it.entities[0]);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_remove(world, e2, Foo);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        if (cache_kind == EcsQueryCacheAuto) {
            test_bool(true, ecs_query_next(&it));
            test_int(2, it.count);
            test_uint(e1, it.entities[0]);
            test_uint(e2, it.entities[1]);

            test_bool(true, ecs_query_next(&it));
            test_int(0, it.count);
        } else {
            test_bool(true, ecs_query_next(&it));
            test_int(0, it.count);

            test_bool(true, ecs_query_next(&it));
            test_int(2, it.count);
            test_uint(e1, it.entities[0]);
            test_uint(e2, it.entities[1]);
        }

        test_bool(false, ecs_query_next(&it));
    }

    ecs_add(world, e2, Foo);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e2, it.entities[0]);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}
