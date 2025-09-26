#include <query.h>

static ecs_query_cache_kind_t cache_kind = EcsQueryCacheDefault;

void DontFragment_setup(void) {
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

void DontFragment_1_fixed_sparse(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_add_id(world, ecs_id(Position), EcsDontFragment);

    ecs_entity_t ent = ecs_entity(world, { .name = "ent" });
    ecs_set(world, ent, Position, {10, 20});

    ecs_query_t *q = ecs_query(world, {
        .expr = "Position(ent)",
        .cache_kind = cache_kind
    });
    test_assert(q != NULL);

    test_bool(true, !!(q->row_fields & (1llu << 0)));

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(ent, ecs_field_src(&it, 0));
    {
        Position *p = ecs_field_at(&it, Position, 0, 0);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void DontFragment_1_fixed_sparse_none(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_add_id(world, ecs_id(Position), EcsDontFragment);

    ecs_entity_t ent = ecs_entity(world, { .name = "ent" });
    ecs_set(world, ent, Position, {10, 20});

    ecs_query_t *q = ecs_query(world, {
        .expr = "[none] Position(ent)",
        .cache_kind = cache_kind
    });
    test_assert(q != NULL);

    test_bool(false, !!(q->row_fields & (1llu << 0)));

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(ent, ecs_field_src(&it, 0));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void DontFragment_1_fixed_sparse_self(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_add_id(world, ecs_id(Position), EcsDontFragment);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);

    ecs_entity_t base = ecs_new(world);
    ecs_add_id(world, base, EcsPrefab);
    ecs_set(world, base, Position, {10, 20});

    ecs_entity_t ent = ecs_entity(world, { .name = "ent" });
    ecs_add_pair(world, ent, EcsIsA, base);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Position(ent|self)",
        .cache_kind = cache_kind
    });
    test_assert(q != NULL);

    test_bool(true, !!(q->row_fields & (1llu << 0)));

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_set(world, ent, Position, {10, 20});

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(ent, ecs_field_src(&it, 0));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void DontFragment_1_fixed_sparse_self_up(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_add_id(world, ecs_id(Position), EcsDontFragment);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);

    ecs_entity_t base = ecs_new(world);
    ecs_add_id(world, base, EcsPrefab);
    ecs_set(world, base, Position, {10, 20});

    ecs_entity_t ent = ecs_entity(world, { .name = "ent" });
    ecs_add_pair(world, ent, EcsIsA, base);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Position(ent|self|up IsA)",
        .cache_kind = cache_kind
    });
    test_assert(q != NULL);

    test_bool(true, !!(q->row_fields & (1llu << 0)));

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(base, ecs_field_src(&it, 0));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_set(world, ent, Position, {10, 20});

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(ent, ecs_field_src(&it, 0));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void DontFragment_1_fixed_sparse_up(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_add_id(world, ecs_id(Position), EcsDontFragment);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);

    ecs_entity_t ent = ecs_entity(world, { .name = "ent" });

    ecs_query_t *q = ecs_query(world, {
        .expr = "Position(ent|up IsA)",
        .cache_kind = cache_kind
    });
    test_assert(q != NULL);

    test_bool(true, !!(q->row_fields & (1llu << 0)));

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_set(world, ent, Position, {10, 20});

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_entity_t base = ecs_new(world);
    ecs_add_id(world, base, EcsPrefab);
    ecs_add_pair(world, ent, EcsIsA, base);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_set(world, base, Position, {10, 20});

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(base, ecs_field_src(&it, 0));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void DontFragment_1_fixed_sparse_any(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Movement, DontFragment);
    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);
    ECS_TAG(world, Sitting);

    ecs_entity_t e1 = ecs_entity(world, { .name = "ent" });
    ecs_add_pair(world, e1, Movement, Walking);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Movement(ent, _)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(e1, ecs_field_src(&it, 0));
    test_uint(ecs_pair(Movement, EcsWildcard), ecs_field_id(&it, 0));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void DontFragment_1_fixed_sparse_exclusive_any(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Movement, DontFragment, Exclusive);
    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);
    ECS_TAG(world, Sitting);

    ecs_entity_t e1 = ecs_entity(world, { .name = "ent" });
    ecs_add_pair(world, e1, Movement, Walking);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Movement(ent, _)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(e1, ecs_field_src(&it, 0));
    test_uint(ecs_pair(Movement, EcsWildcard), ecs_field_id(&it, 0));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void DontFragment_1_fixed_sparse_tgt_var(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Movement, DontFragment);
    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);
    ECS_TAG(world, Sitting);

    ecs_entity_t e1 = ecs_entity(world, { .name = "ent" });
    ecs_add_pair(world, e1, Movement, Walking);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Movement(ent, $x)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(e1, ecs_field_src(&it, 0));
        test_uint(ecs_pair(Movement, Walking), ecs_field_id(&it, 0));
        test_uint(Walking, ecs_iter_get_var(&it, x_var));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_pair(world, e1, Movement, Running);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(e1, ecs_field_src(&it, 0));
        test_uint(ecs_pair(Movement, Running), ecs_field_id(&it, 0));
        test_uint(Running, ecs_iter_get_var(&it, x_var));
        
        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(e1, ecs_field_src(&it, 0));
        test_uint(ecs_pair(Movement, Walking), ecs_field_id(&it, 0));
        test_uint(Walking, ecs_iter_get_var(&it, x_var));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void DontFragment_1_fixed_sparse_exclusive_tgt_var(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Movement, DontFragment, Exclusive);
    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);
    ECS_TAG(world, Sitting);

    ecs_entity_t e1 = ecs_entity(world, { .name = "ent" });
    ecs_add_pair(world, e1, Movement, Walking);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Movement(ent, $x)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(e1, ecs_field_src(&it, 0));
        test_uint(ecs_pair(Movement, Walking), ecs_field_id(&it, 0));
        test_uint(Walking, ecs_iter_get_var(&it, x_var));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_pair(world, e1, Movement, Running);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(e1, ecs_field_src(&it, 0));
        test_uint(ecs_pair(Movement, Running), ecs_field_id(&it, 0));
        test_uint(Running, ecs_iter_get_var(&it, x_var));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void DontFragment_1_fixed_sparse_tgt_var_written(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Movement, DontFragment);
    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);
    ECS_TAG(world, Sitting);

    ecs_entity_t e1 = ecs_entity(world, { .name = "ent" });
    ecs_add_pair(world, e1, Movement, Walking);

    ecs_query_t *q = ecs_query(world, {
        .expr = "$x == Walking, Movement(ent, $x)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(ecs_pair(Movement, Walking), ecs_field_id(&it, 1));
        test_uint(Walking, ecs_iter_get_var(&it, x_var));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_pair(world, e1, Movement, Running);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(ecs_pair(Movement, Walking), ecs_field_id(&it, 1));
        test_uint(Walking, ecs_iter_get_var(&it, x_var));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void DontFragment_1_fixed_sparse_exclusive_tgt_var_written(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Movement, DontFragment, Exclusive);
    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);
    ECS_TAG(world, Sitting);

    ecs_entity_t e1 = ecs_entity(world, { .name = "ent" });
    ecs_add_pair(world, e1, Movement, Walking);

    ecs_query_t *q = ecs_query(world, {
        .expr = "$x == Walking, Movement(ent, $x)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(ecs_pair(Movement, Walking), ecs_field_id(&it, 1));
        test_uint(Walking, ecs_iter_get_var(&it, x_var));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_pair(world, e1, Movement, Running);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void DontFragment_1_this_sparse(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_add_id(world, ecs_id(Position), EcsDontFragment);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Position",
        .cache_kind = cache_kind
    });
    test_assert(q != NULL);

    test_bool(true, !!(q->row_fields & (1llu << 0)));

    ecs_entity_t e1 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(Position, {30, 40}));
    ecs_entity_t e3 = ecs_insert(world, ecs_value(Position, {50, 60}));

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e3, it.entities[0]);
    {
        Position *p = ecs_field_at(&it, Position, 0, 0);
        test_assert(p != NULL);
        test_int(p->x, 50); test_int(p->y, 60);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e2, it.entities[0]);
    {
        Position *p = ecs_field_at(&it, Position, 0, 0);
        test_assert(p != NULL);
        test_int(p->x, 30); test_int(p->y, 40);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e1, it.entities[0]);
    {
        Position *p = ecs_field_at(&it, Position, 0, 0);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void DontFragment_1_this_sparse_simple(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_add_id(world, ecs_id(Position), EcsDontFragment);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ ecs_id(Position) }},
        .cache_kind = cache_kind
    });
    test_assert(q != NULL);

    test_bool(true, !!(q->row_fields & (1llu << 0)));

    ecs_entity_t e1 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(Position, {30, 40}));
    ecs_entity_t e3 = ecs_insert(world, ecs_value(Position, {50, 60}));

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e3, it.entities[0]);
    {
        Position *p = ecs_field_at(&it, Position, 0, 0);
        test_assert(p != NULL);
        test_int(p->x, 50); test_int(p->y, 60);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e2, it.entities[0]);
    {
        Position *p = ecs_field_at(&it, Position, 0, 0);
        test_assert(p != NULL);
        test_int(p->x, 30); test_int(p->y, 40);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e1, it.entities[0]);
    {
        Position *p = ecs_field_at(&it, Position, 0, 0);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void DontFragment_1_this_sparse_none(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_add_id(world, ecs_id(Position), EcsDontFragment);

    ecs_query_t *q = ecs_query(world, {
        .expr = "[none] Position",
        .cache_kind = cache_kind
    });
    test_assert(q != NULL);

    test_bool(false, !!(q->row_fields & (1llu << 0)));

    ecs_entity_t e1 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(Position, {30, 40}));
    ecs_entity_t e3 = ecs_insert(world, ecs_value(Position, {50, 60}));

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e3, it.entities[0]);

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e2, it.entities[0]);

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e1, it.entities[0]);

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void DontFragment_1_this_sparse_written(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Foo);

    ecs_add_id(world, ecs_id(Position), EcsDontFragment);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Foo, Position",
        .cache_kind = cache_kind
    });
    test_assert(q != NULL);
    
    test_bool(false, !!(q->row_fields & (1llu << 0)));
    test_bool(true, !!(q->row_fields & (1llu << 1)));

    ecs_entity_t e1 = ecs_insert(world, {Foo}, ecs_value(Position, {10, 20}));
    ecs_entity_t e2 = ecs_insert(world, {Foo}, ecs_value(Position, {30, 40}));
    ecs_entity_t e3 = ecs_insert(world, {Foo}, ecs_value(Position, {50, 60}));

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e1, it.entities[0]);
    {
        Position *p = ecs_field_at(&it, Position, 1, 0);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e2, it.entities[0]);
    {
        Position *p = ecs_field_at(&it, Position, 1, 0);
        test_assert(p != NULL);
        test_int(p->x, 30); test_int(p->y, 40);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e3, it.entities[0]);
    {
        Position *p = ecs_field_at(&it, Position, 1, 0);
        test_assert(p != NULL);
        test_int(p->x, 50); test_int(p->y, 60);
    }

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void DontFragment_1_this_sparse_written_partial(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Foo);

    ecs_add_id(world, ecs_id(Position), EcsDontFragment);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Foo, Position",
        .cache_kind = cache_kind
    });
    test_assert(q != NULL);
    
    test_bool(false, !!(q->row_fields & (1llu << 0)));
    test_bool(true, !!(q->row_fields & (1llu << 1)));

    ecs_entity_t e1 = ecs_insert(world, {Foo}, ecs_value(Position, {10, 20}));
    ecs_entity_t e2 = ecs_insert(world, {Foo}, ecs_value(Position, {30, 40}));
    ecs_insert(world, {Foo});

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e1, it.entities[0]);
    {
        Position *p = ecs_field_at(&it, Position, 1, 0);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e2, it.entities[0]);
    {
        Position *p = ecs_field_at(&it, Position, 1, 0);
        test_assert(p != NULL);
        test_int(p->x, 30); test_int(p->y, 40);
    }

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void DontFragment_1_this_sparse_written_none(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Foo);

    ecs_add_id(world, ecs_id(Position), EcsDontFragment);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Foo, [none] Position",
        .cache_kind = cache_kind
    });
    test_assert(q != NULL);

    test_bool(false, !!(q->row_fields & (1llu << 0)));
    test_bool(false, !!(q->row_fields & (1llu << 1)));

    ecs_entity_t e1 = ecs_insert(world, {Foo}, ecs_value(Position, {10, 20}));
    ecs_entity_t e2 = ecs_insert(world, {Foo}, ecs_value(Position, {30, 40}));
    ecs_entity_t e3 = ecs_insert(world, {Foo}, ecs_value(Position, {50, 60}));

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e1, it.entities[0]);

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e2, it.entities[0]);

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e3, it.entities[0]);

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void DontFragment_1_this_sparse_any(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Movement, DontFragment);
    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);
    ECS_TAG(world, Sitting);

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_entity_t e1 = ecs_new(world);
    ecs_entity_t e2 = ecs_new(world);
    ecs_entity_t e3 = ecs_new(world);
    ecs_entity_t e4 = ecs_new(world);
    ecs_entity_t e5 = ecs_new(world);
    ecs_entity_t e6 = ecs_new(world);

    ecs_add_pair(world, e1, Movement, Walking);
    ecs_add_pair(world, e2, Movement, Walking);
    ecs_add_pair(world, e3, Movement, Running);
    ecs_add_pair(world, e4, Movement, Running);
    ecs_add_pair(world, e5, Movement, Running);
    ecs_add_pair(world, e6, Movement, Sitting);

    ecs_add(world, e3, Foo);
    ecs_add(world, e4, Foo);
    ecs_add(world, e5, Bar);
    ecs_add(world, e6, Bar);

    ecs_query_t *q = ecs_query(world, {
        .expr = "(Movement, _)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e6, it.entities[0]);
    test_uint(ecs_pair(Movement, EcsWildcard), ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e5, it.entities[0]);
    test_uint(ecs_pair(Movement, EcsWildcard), ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e4, it.entities[0]);
    test_uint(ecs_pair(Movement, EcsWildcard), ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e3, it.entities[0]);
    test_uint(ecs_pair(Movement, EcsWildcard), ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e2, it.entities[0]);
    test_uint(ecs_pair(Movement, EcsWildcard), ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(ecs_pair(Movement, EcsWildcard), ecs_field_id(&it, 0));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void DontFragment_1_this_sparse_exclusive_any(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Movement, DontFragment, Exclusive);
    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);
    ECS_TAG(world, Sitting);

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_entity_t e1 = ecs_new(world);
    ecs_entity_t e2 = ecs_new(world);
    ecs_entity_t e3 = ecs_new(world);
    ecs_entity_t e4 = ecs_new(world);
    ecs_entity_t e5 = ecs_new(world);
    ecs_entity_t e6 = ecs_new(world);

    ecs_add_pair(world, e1, Movement, Walking);
    ecs_add_pair(world, e2, Movement, Walking);
    ecs_add_pair(world, e3, Movement, Running);
    ecs_add_pair(world, e4, Movement, Running);
    ecs_add_pair(world, e5, Movement, Running);
    ecs_add_pair(world, e6, Movement, Sitting);

    ecs_add(world, e3, Foo);
    ecs_add(world, e4, Foo);
    ecs_add(world, e5, Bar);
    ecs_add(world, e6, Bar);

    ecs_query_t *q = ecs_query(world, {
        .expr = "(Movement, _)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e6, it.entities[0]);
    test_uint(ecs_pair(Movement, EcsWildcard), ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e5, it.entities[0]);
    test_uint(ecs_pair(Movement, EcsWildcard), ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e4, it.entities[0]);
    test_uint(ecs_pair(Movement, EcsWildcard), ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e3, it.entities[0]);
    test_uint(ecs_pair(Movement, EcsWildcard), ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e2, it.entities[0]);
    test_uint(ecs_pair(Movement, EcsWildcard), ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(ecs_pair(Movement, EcsWildcard), ecs_field_id(&it, 0));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void DontFragment_1_this_sparse_tgt_var(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Movement, DontFragment);
    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);
    ECS_TAG(world, Sitting);

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_entity_t e1 = ecs_new(world);
    ecs_entity_t e2 = ecs_new(world);
    ecs_entity_t e3 = ecs_new(world);
    ecs_entity_t e4 = ecs_new(world);
    ecs_entity_t e5 = ecs_new(world);
    ecs_entity_t e6 = ecs_new(world);

    ecs_add_pair(world, e1, Movement, Walking);
    ecs_add_pair(world, e2, Movement, Walking);
    ecs_add_pair(world, e3, Movement, Running);
    ecs_add_pair(world, e4, Movement, Running);
    ecs_add_pair(world, e5, Movement, Running);
    ecs_add_pair(world, e6, Movement, Sitting);

    ecs_add(world, e3, Foo);
    ecs_add(world, e4, Foo);
    ecs_add(world, e5, Bar);
    ecs_add(world, e6, Bar);

    ecs_query_t *q = ecs_query(world, {
        .expr = "(Movement, $x)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e6, it.entities[0]);
    test_uint(ecs_pair(Movement, Sitting), ecs_field_id(&it, 0));
    test_uint(Sitting, ecs_iter_get_var(&it, x_var));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e5, it.entities[0]);
    test_uint(ecs_pair(Movement, Running), ecs_field_id(&it, 0));
    test_uint(Running, ecs_iter_get_var(&it, x_var));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e4, it.entities[0]);
    test_uint(ecs_pair(Movement, Running), ecs_field_id(&it, 0));
    test_uint(Running, ecs_iter_get_var(&it, x_var));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e3, it.entities[0]);
    test_uint(ecs_pair(Movement, Running), ecs_field_id(&it, 0));
    test_uint(Running, ecs_iter_get_var(&it, x_var));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e2, it.entities[0]);
    test_uint(ecs_pair(Movement, Walking), ecs_field_id(&it, 0));
    test_uint(Walking, ecs_iter_get_var(&it, x_var));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(ecs_pair(Movement, Walking), ecs_field_id(&it, 0));
    test_uint(Walking, ecs_iter_get_var(&it, x_var));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void DontFragment_1_this_sparse_exclusive_tgt_var(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Movement, DontFragment, Exclusive);
    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);
    ECS_TAG(world, Sitting);

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_entity_t e1 = ecs_new(world);
    ecs_entity_t e2 = ecs_new(world);
    ecs_entity_t e3 = ecs_new(world);
    ecs_entity_t e4 = ecs_new(world);
    ecs_entity_t e5 = ecs_new(world);
    ecs_entity_t e6 = ecs_new(world);

    ecs_add_pair(world, e1, Movement, Walking);
    ecs_add_pair(world, e2, Movement, Walking);
    ecs_add_pair(world, e3, Movement, Running);
    ecs_add_pair(world, e4, Movement, Running);
    ecs_add_pair(world, e5, Movement, Running);
    ecs_add_pair(world, e6, Movement, Sitting);

    ecs_add(world, e3, Foo);
    ecs_add(world, e4, Foo);
    ecs_add(world, e5, Bar);
    ecs_add(world, e6, Bar);

    ecs_query_t *q = ecs_query(world, {
        .expr = "(Movement, $x)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e6, it.entities[0]);
    test_uint(ecs_pair(Movement, Sitting), ecs_field_id(&it, 0));
    test_uint(Sitting, ecs_iter_get_var(&it, x_var));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e5, it.entities[0]);
    test_uint(ecs_pair(Movement, Running), ecs_field_id(&it, 0));
    test_uint(Running, ecs_iter_get_var(&it, x_var));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e4, it.entities[0]);
    test_uint(ecs_pair(Movement, Running), ecs_field_id(&it, 0));
    test_uint(Running, ecs_iter_get_var(&it, x_var));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e3, it.entities[0]);
    test_uint(ecs_pair(Movement, Running), ecs_field_id(&it, 0));
    test_uint(Running, ecs_iter_get_var(&it, x_var));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e2, it.entities[0]);
    test_uint(ecs_pair(Movement, Walking), ecs_field_id(&it, 0));
    test_uint(Walking, ecs_iter_get_var(&it, x_var));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(ecs_pair(Movement, Walking), ecs_field_id(&it, 0));
    test_uint(Walking, ecs_iter_get_var(&it, x_var));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void DontFragment_1_this_sparse_tgt_var_written(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Movement, DontFragment);
    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);
    ECS_TAG(world, Sitting);

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_entity_t e1 = ecs_new(world);
    ecs_entity_t e2 = ecs_new(world);
    ecs_entity_t e3 = ecs_new(world);
    ecs_entity_t e4 = ecs_new(world);
    ecs_entity_t e5 = ecs_new(world);
    ecs_entity_t e6 = ecs_new(world);

    ecs_add_pair(world, e1, Movement, Walking);
    ecs_add_pair(world, e2, Movement, Walking);
    ecs_add_pair(world, e3, Movement, Running);
    ecs_add_pair(world, e4, Movement, Running);
    ecs_add_pair(world, e5, Movement, Running);
    ecs_add_pair(world, e6, Movement, Sitting);

    ecs_add(world, e3, Foo);
    ecs_add(world, e4, Foo);
    ecs_add(world, e5, Bar);
    ecs_add(world, e6, Bar);

    ecs_query_t *q = ecs_query(world, {
        .expr = "$x == Running, (Movement, $x)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e5, it.entities[0]);
    test_uint(ecs_pair(Movement, Running), ecs_field_id(&it, 1));
    test_uint(Running, ecs_iter_get_var(&it, x_var));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e4, it.entities[0]);
    test_uint(ecs_pair(Movement, Running), ecs_field_id(&it, 1));
    test_uint(Running, ecs_iter_get_var(&it, x_var));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e3, it.entities[0]);
    test_uint(ecs_pair(Movement, Running), ecs_field_id(&it, 1));
    test_uint(Running, ecs_iter_get_var(&it, x_var));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void DontFragment_1_this_sparse_exclusive_tgt_var_written(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Movement, DontFragment, Exclusive);
    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);
    ECS_TAG(world, Sitting);

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_entity_t e1 = ecs_new(world);
    ecs_entity_t e2 = ecs_new(world);
    ecs_entity_t e3 = ecs_new(world);
    ecs_entity_t e4 = ecs_new(world);
    ecs_entity_t e5 = ecs_new(world);
    ecs_entity_t e6 = ecs_new(world);

    ecs_add_pair(world, e1, Movement, Walking);
    ecs_add_pair(world, e2, Movement, Walking);
    ecs_add_pair(world, e3, Movement, Running);
    ecs_add_pair(world, e4, Movement, Running);
    ecs_add_pair(world, e5, Movement, Running);
    ecs_add_pair(world, e6, Movement, Sitting);

    ecs_add(world, e3, Foo);
    ecs_add(world, e4, Foo);
    ecs_add(world, e5, Bar);
    ecs_add(world, e6, Bar);

    ecs_query_t *q = ecs_query(world, {
        .expr = "$x == Running, (Movement, $x)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e5, it.entities[0]);
    test_uint(ecs_pair(Movement, Running), ecs_field_id(&it, 1));
    test_uint(Running, ecs_iter_get_var(&it, x_var));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e4, it.entities[0]);
    test_uint(ecs_pair(Movement, Running), ecs_field_id(&it, 1));
    test_uint(Running, ecs_iter_get_var(&it, x_var));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e3, it.entities[0]);
    test_uint(ecs_pair(Movement, Running), ecs_field_id(&it, 1));
    test_uint(Running, ecs_iter_get_var(&it, x_var));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void DontFragment_1_var_sparse(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_add_id(world, ecs_id(Position), EcsDontFragment);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Position($x)",
        .cache_kind = cache_kind
    });
    test_assert(q != NULL);

    test_bool(true, !!(q->row_fields & (1llu << 0)));

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    ecs_entity_t e1 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(Position, {30, 40}));
    ecs_entity_t e3 = ecs_insert(world, ecs_value(Position, {50, 60}));

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(e3, ecs_iter_get_var(&it, x_var));
    {
        Position *p = ecs_field_at(&it, Position, 0, 0);
        test_assert(p != NULL);
        test_int(p->x, 50); test_int(p->y, 60);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(e2, ecs_iter_get_var(&it, x_var));
    {
        Position *p = ecs_field_at(&it, Position, 0, 0);
        test_assert(p != NULL);
        test_int(p->x, 30); test_int(p->y, 40);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(e1, ecs_iter_get_var(&it, x_var));
    {
        Position *p = ecs_field_at(&it, Position, 0, 0);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void DontFragment_1_var_sparse_none(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_add_id(world, ecs_id(Position), EcsDontFragment);

    ecs_query_t *q = ecs_query(world, {
        .expr = "[none] Position($x)",
        .cache_kind = cache_kind
    });
    test_assert(q != NULL);

    test_bool(false, !!(q->row_fields & (1llu << 0)));

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    ecs_entity_t e1 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(Position, {30, 40}));
    ecs_entity_t e3 = ecs_insert(world, ecs_value(Position, {50, 60}));

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(e3, ecs_iter_get_var(&it, x_var));

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(e2, ecs_iter_get_var(&it, x_var));

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(e1, ecs_iter_get_var(&it, x_var));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void DontFragment_1_var_sparse_written(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Foo);

    ecs_add_id(world, ecs_id(Position), EcsDontFragment);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Foo($x), Position($x)",
        .cache_kind = cache_kind
    });
    test_assert(q != NULL);

    test_bool(false, !!(q->row_fields & (1llu << 0)));
    test_bool(true, !!(q->row_fields & (1llu << 1)));

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    ecs_entity_t e1 = ecs_insert(world, {Foo}, ecs_value(Position, {10, 20}));
    ecs_entity_t e2 = ecs_insert(world, {Foo}, ecs_value(Position, {30, 40}));
    ecs_entity_t e3 = ecs_insert(world, {Foo}, ecs_value(Position, {50, 60}));

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(e1, ecs_iter_get_var(&it, x_var));
    {
        Position *p = ecs_field_at(&it, Position, 1, 0);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(e2, ecs_iter_get_var(&it, x_var));
    {
        Position *p = ecs_field_at(&it, Position, 1, 0);
        test_assert(p != NULL);
        test_int(p->x, 30); test_int(p->y, 40);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(e3, ecs_iter_get_var(&it, x_var));
    {
        Position *p = ecs_field_at(&it, Position, 1, 0);
        test_assert(p != NULL);
        test_int(p->x, 50); test_int(p->y, 60);
    }

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void DontFragment_1_var_sparse_written_none(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Foo);

    ecs_add_id(world, ecs_id(Position), EcsDontFragment);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Foo($x), [none] Position($x)",
        .cache_kind = cache_kind
    });
    test_assert(q != NULL);

    test_bool(false, !!(q->row_fields & (1llu << 0)));
    test_bool(false, !!(q->row_fields & (1llu << 1)));

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    ecs_entity_t e1 = ecs_insert(world, {Foo}, ecs_value(Position, {10, 20}));
    ecs_entity_t e2 = ecs_insert(world, {Foo}, ecs_value(Position, {30, 40}));
    ecs_entity_t e3 = ecs_insert(world, {Foo}, ecs_value(Position, {50, 60}));

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(e1, ecs_iter_get_var(&it, x_var));

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(e2, ecs_iter_get_var(&it, x_var));

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(e3, ecs_iter_get_var(&it, x_var));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void DontFragment_1_var_sparse_any(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Movement, DontFragment);
    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);
    ECS_TAG(world, Sitting);

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_entity_t e1 = ecs_new(world);
    ecs_entity_t e2 = ecs_new(world);
    ecs_entity_t e3 = ecs_new(world);
    ecs_entity_t e4 = ecs_new(world);
    ecs_entity_t e5 = ecs_new(world);
    ecs_entity_t e6 = ecs_new(world);

    ecs_add_pair(world, e1, Movement, Walking);
    ecs_add_pair(world, e2, Movement, Walking);
    ecs_add_pair(world, e3, Movement, Running);
    ecs_add_pair(world, e4, Movement, Running);
    ecs_add_pair(world, e5, Movement, Running);
    ecs_add_pair(world, e6, Movement, Sitting);

    ecs_add(world, e3, Foo);
    ecs_add(world, e4, Foo);
    ecs_add(world, e5, Bar);
    ecs_add(world, e6, Bar);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Movement($x, _)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(e6, ecs_field_src(&it, 0));
    test_uint(e6, ecs_iter_get_var(&it, x_var));
    test_uint(ecs_pair(Movement, EcsWildcard), ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(e5, ecs_field_src(&it, 0));
    test_uint(e5, ecs_iter_get_var(&it, x_var));
    test_uint(ecs_pair(Movement, EcsWildcard), ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(e4, ecs_field_src(&it, 0));
    test_uint(e4, ecs_iter_get_var(&it, x_var));
    test_uint(ecs_pair(Movement, EcsWildcard), ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(e3, ecs_field_src(&it, 0));
    test_uint(e3, ecs_iter_get_var(&it, x_var));
    test_uint(ecs_pair(Movement, EcsWildcard), ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(e2, ecs_field_src(&it, 0));
    test_uint(e2, ecs_iter_get_var(&it, x_var));
    test_uint(ecs_pair(Movement, EcsWildcard), ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(e1, ecs_field_src(&it, 0));
    test_uint(e1, ecs_iter_get_var(&it, x_var));
    test_uint(ecs_pair(Movement, EcsWildcard), ecs_field_id(&it, 0));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void DontFragment_1_var_sparse_exclusive_any(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Movement, DontFragment, Exclusive);
    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);
    ECS_TAG(world, Sitting);

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_entity_t e1 = ecs_new(world);
    ecs_entity_t e2 = ecs_new(world);
    ecs_entity_t e3 = ecs_new(world);
    ecs_entity_t e4 = ecs_new(world);
    ecs_entity_t e5 = ecs_new(world);
    ecs_entity_t e6 = ecs_new(world);

    ecs_add_pair(world, e1, Movement, Walking);
    ecs_add_pair(world, e2, Movement, Walking);
    ecs_add_pair(world, e3, Movement, Running);
    ecs_add_pair(world, e4, Movement, Running);
    ecs_add_pair(world, e5, Movement, Running);
    ecs_add_pair(world, e6, Movement, Sitting);

    ecs_add(world, e3, Foo);
    ecs_add(world, e4, Foo);
    ecs_add(world, e5, Bar);
    ecs_add(world, e6, Bar);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Movement($x, _)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(e6, ecs_field_src(&it, 0));
    test_uint(e6, ecs_iter_get_var(&it, x_var));
    test_uint(ecs_pair(Movement, EcsWildcard), ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(e5, ecs_field_src(&it, 0));
    test_uint(e5, ecs_iter_get_var(&it, x_var));
    test_uint(ecs_pair(Movement, EcsWildcard), ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(e4, ecs_field_src(&it, 0));
    test_uint(e4, ecs_iter_get_var(&it, x_var));
    test_uint(ecs_pair(Movement, EcsWildcard), ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(e3, ecs_field_src(&it, 0));
    test_uint(e3, ecs_iter_get_var(&it, x_var));
    test_uint(ecs_pair(Movement, EcsWildcard), ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(e2, ecs_field_src(&it, 0));
    test_uint(e2, ecs_iter_get_var(&it, x_var));
    test_uint(ecs_pair(Movement, EcsWildcard), ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(e1, ecs_field_src(&it, 0));
    test_uint(e1, ecs_iter_get_var(&it, x_var));
    test_uint(ecs_pair(Movement, EcsWildcard), ecs_field_id(&it, 0));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void DontFragment_2_sparse_simple(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_add_id(world, ecs_id(Position), EcsDontFragment);
    ecs_add_id(world, ecs_id(Velocity), EcsDontFragment);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ .id = ecs_id(Position) }, { .id = ecs_id(Velocity) }},
        .cache_kind = cache_kind
    });
    test_assert(q != NULL);

    test_bool(true, !!(q->row_fields & (1llu << 0)));
    test_bool(true, !!(q->row_fields & (1llu << 1)));

    ecs_entity_t e1 = ecs_insert(world, 
        ecs_value(Position, {10, 20}),
        ecs_value(Velocity, {1,  2}));
    ecs_entity_t e2 = ecs_insert(world, 
        ecs_value(Position, {30, 40}),
        ecs_value(Velocity, {3,  4}));
    ecs_entity_t e3 = ecs_insert(world, 
        ecs_value(Position, {50, 60}),
        ecs_value(Velocity, {5,  6}));

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e3, it.entities[0]);
    {
        Position *p = ecs_field_at(&it, Position, 0, 0);
        test_assert(p != NULL);
        test_int(p->x, 50); test_int(p->y, 60);
    }
    {
        Velocity *v = ecs_field_at(&it, Velocity, 1, 0);
        test_assert(v != NULL);
        test_int(v->x, 5); test_int(v->y, 6);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e2, it.entities[0]);
    {
        Position *p = ecs_field_at(&it, Position, 0, 0);
        test_assert(p != NULL);
        test_int(p->x, 30); test_int(p->y, 40);
    }
    {
        Velocity *v = ecs_field_at(&it, Velocity, 1, 0);
        test_assert(v != NULL);
        test_int(v->x, 3); test_int(v->y, 4);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e1, it.entities[0]);
    {
        Position *p = ecs_field_at(&it, Position, 0, 0);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }
    {
        Velocity *v = ecs_field_at(&it, Velocity, 1, 0);
        test_assert(v != NULL);
        test_int(v->x, 1); test_int(v->y, 2);
    }

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void DontFragment_2_sparse(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_add_id(world, ecs_id(Position), EcsDontFragment);
    ecs_add_id(world, ecs_id(Velocity), EcsDontFragment);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Position, Velocity",
        .cache_kind = cache_kind
    });
    test_assert(q != NULL);

    test_bool(true, !!(q->row_fields & (1llu << 0)));
    test_bool(true, !!(q->row_fields & (1llu << 1)));

    ecs_entity_t e1 = ecs_insert(world, 
        ecs_value(Position, {10, 20}),
        ecs_value(Velocity, {1,  2}));
    ecs_entity_t e2 = ecs_insert(world, 
        ecs_value(Position, {30, 40}),
        ecs_value(Velocity, {3,  4}));
    ecs_entity_t e3 = ecs_insert(world, 
        ecs_value(Position, {50, 60}),
        ecs_value(Velocity, {5,  6}));

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e3, it.entities[0]);
    {
        Position *p = ecs_field_at(&it, Position, 0, 0);
        test_assert(p != NULL);
        test_int(p->x, 50); test_int(p->y, 60);
    }
    {
        Velocity *v = ecs_field_at(&it, Velocity, 1, 0);
        test_assert(v != NULL);
        test_int(v->x, 5); test_int(v->y, 6);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e2, it.entities[0]);
    {
        Position *p = ecs_field_at(&it, Position, 0, 0);
        test_assert(p != NULL);
        test_int(p->x, 30); test_int(p->y, 40);
    }
    {
        Velocity *v = ecs_field_at(&it, Velocity, 1, 0);
        test_assert(v != NULL);
        test_int(v->x, 3); test_int(v->y, 4);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e1, it.entities[0]);
    {
        Position *p = ecs_field_at(&it, Position, 0, 0);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }
    {
        Velocity *v = ecs_field_at(&it, Velocity, 1, 0);
        test_assert(v != NULL);
        test_int(v->x, 1); test_int(v->y, 2);
    }

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void DontFragment_2_sparse_and_regular(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_add_id(world, ecs_id(Position), EcsDontFragment);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Position, Velocity",
        .cache_kind = cache_kind
    });
    test_assert(q != NULL);

    test_bool(true, !!(q->row_fields & (1llu << 0)));
    test_bool(false, !!(q->row_fields & (1llu << 1)));

    ecs_entity_t e1 = ecs_insert(world, 
        ecs_value(Position, {10, 20}),
        ecs_value(Velocity, {1,  2}));
    ecs_entity_t e2 = ecs_insert(world, 
        ecs_value(Position, {30, 40}),
        ecs_value(Velocity, {3,  4}));
    ecs_entity_t e3 = ecs_insert(world, 
        ecs_value(Position, {50, 60}),
        ecs_value(Velocity, {5,  6}));

    

    ecs_iter_t it = ecs_query_iter(world, q);

    if (cache_kind == EcsQueryCacheDefault) {
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e3, it.entities[0]);
        {
            Position *p = ecs_field_at(&it, Position, 0, 0);
            test_assert(p != NULL);
            test_int(p->x, 50); test_int(p->y, 60);
        }
        {
            Velocity *v = ecs_field(&it, Velocity, 1);
            test_assert(v != NULL);
            test_int(v->x, 5); test_int(v->y, 6);
        }

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e2, it.entities[0]);
        {
            Position *p = ecs_field_at(&it, Position, 0, 0);
            test_assert(p != NULL);
            test_int(p->x, 30); test_int(p->y, 40);
        }
        {
            Velocity *v = ecs_field(&it, Velocity, 1);
            test_assert(v != NULL);
            test_int(v->x, 3); test_int(v->y, 4);
        }

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e1, it.entities[0]);
        {
            Position *p = ecs_field_at(&it, Position, 0, 0);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        {
            Velocity *v = ecs_field(&it, Velocity, 1);
            test_assert(v != NULL);
            test_int(v->x, 1); test_int(v->y, 2);
        }
    } else {
        // Different ordering, same results

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e1, it.entities[0]);
        {
            Position *p = ecs_field_at(&it, Position, 0, 0);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        {
            Velocity *v = ecs_field(&it, Velocity, 1);
            test_assert(v != NULL);
            test_int(v->x, 1); test_int(v->y, 2);
        }

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e2, it.entities[0]);
        {
            Position *p = ecs_field_at(&it, Position, 0, 0);
            test_assert(p != NULL);
            test_int(p->x, 30); test_int(p->y, 40);
        }
        {
            Velocity *v = ecs_field(&it, Velocity, 1);
            test_assert(v != NULL);
            test_int(v->x, 3); test_int(v->y, 4);
        }

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e3, it.entities[0]);
        {
            Position *p = ecs_field_at(&it, Position, 0, 0);
            test_assert(p != NULL);
            test_int(p->x, 50); test_int(p->y, 60);
        }
        {
            Velocity *v = ecs_field(&it, Velocity, 1);
            test_assert(v != NULL);
            test_int(v->x, 5); test_int(v->y, 6);
        }
    }

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void DontFragment_2_regular_and_sparse(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_add_id(world, ecs_id(Velocity), EcsDontFragment);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Position, Velocity",
        .cache_kind = cache_kind
    });
    test_assert(q != NULL);

    test_bool(false, !!(q->row_fields & (1llu << 0)));
    test_bool(true, !!(q->row_fields & (1llu << 1)));

    ecs_entity_t e1 = ecs_insert(world, 
        ecs_value(Position, {10, 20}),
        ecs_value(Velocity, {1,  2}));
    ecs_entity_t e2 = ecs_insert(world, 
        ecs_value(Position, {30, 40}),
        ecs_value(Velocity, {3,  4}));
    ecs_entity_t e3 = ecs_insert(world, 
        ecs_value(Position, {50, 60}),
        ecs_value(Velocity, {5,  6}));

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e1, it.entities[0]);
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p[0].x, 10); test_int(p[0].y, 20);
    }
    {
        Velocity *v = ecs_field_at(&it, Velocity, 1, 0);
        test_assert(v != NULL);
        test_int(v->x, 1); test_int(v->y, 2);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e2, it.entities[0]);
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p[0].x, 30); test_int(p[0].y, 40);
    }
    {
        Velocity *v = ecs_field_at(&it, Velocity, 1, 0);
        test_assert(v != NULL);
        test_int(v->x, 3); test_int(v->y, 4);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e3, it.entities[0]);
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p[0].x, 50); test_int(p[0].y, 60);
    }
    {
        Velocity *v = ecs_field_at(&it, Velocity, 1, 0);
        test_assert(v != NULL);
        test_int(v->x, 5); test_int(v->y, 6);
    }

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void DontFragment_1_sparse_self(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);

    ecs_add_id(world, ecs_id(Position), EcsDontFragment);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Position(self)",
        .cache_kind = cache_kind
    });
    test_assert(q != NULL);

    test_bool(true, !!(q->row_fields & (1llu << 0)));

    ecs_entity_t base = ecs_insert(world, {EcsPrefab}, ecs_value(Position, {1, 2}));
    ecs_entity_t e1 = ecs_insert(world, {ecs_isa(base)}, ecs_value(Position, {10, 20}));
    ecs_entity_t e2 = ecs_insert(world, {ecs_isa(base)}, ecs_value(Position, {30, 40}));
    /* ecs_entity_t e3 = */ ecs_insert(world, {ecs_isa(base)});
    /* ecs_entity_t e4 = */ ecs_insert(world, {ecs_isa(base)});

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e2, it.entities[0]);
    {
        Position *p = ecs_field_at(&it, Position, 0, 0);
        test_assert(p != NULL);
        test_int(p->x, 30); test_int(p->y, 40);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e1, it.entities[0]);
    {
        Position *p = ecs_field_at(&it, Position, 0, 0);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void DontFragment_1_sparse_up(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Foo);

    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    ecs_add_id(world, ecs_id(Position), EcsDontFragment);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Position(up IsA)",
        .cache_kind = cache_kind
    });
    test_assert(q != NULL);

    test_bool(true, !!(q->row_fields & (1llu << 0)));

    ecs_entity_t base = ecs_insert(world, {EcsPrefab}, ecs_value(Position, {1, 2}));
    ecs_entity_t e1 = ecs_insert(world, {ecs_isa(base)}, ecs_value(Position, {10, 20}));
    ecs_entity_t e2 = ecs_insert(world, {ecs_isa(base)}, ecs_value(Position, {30, 40}));
    ecs_entity_t e3 = ecs_insert(world, {ecs_isa(base)});
    ecs_entity_t e4 = ecs_insert(world, {ecs_isa(base)});
    ecs_entity_t e5 = ecs_insert(world, {ecs_isa(base)});
    ecs_add(world, e5, Foo);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(4, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(e2, it.entities[1]);
    test_uint(e3, it.entities[2]);
    test_uint(e4, it.entities[3]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(base, ecs_field_src(&it, 0));
    {
        Position *p = ecs_field_at(&it, Position, 0, 0);
        test_assert(p != NULL);
        test_int(p->x, 1); test_int(p->y, 2);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e5, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(base, ecs_field_src(&it, 0));
    {
        Position *p = ecs_field_at(&it, Position, 0, 0);
        test_assert(p != NULL);
        test_int(p->x, 1); test_int(p->y, 2);
    }


    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void DontFragment_1_sparse_self_up(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    ecs_add_id(world, ecs_id(Position), EcsDontFragment);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Position(self|up IsA)",
        .cache_kind = cache_kind
    });
    test_assert(q != NULL);

    test_bool(true, !!(q->row_fields & (1llu << 0)));

    ecs_entity_t base = ecs_insert(world, {EcsPrefab}, ecs_value(Position, {1, 2}));
    ecs_entity_t e1 = ecs_insert(world, {ecs_isa(base)}, ecs_value(Position, {10, 20}));
    ecs_entity_t e2 = ecs_insert(world, {ecs_isa(base)}, ecs_value(Position, {30, 40}));
    ecs_entity_t e3 = ecs_insert(world, {ecs_isa(base)});
    ecs_entity_t e4 = ecs_insert(world, {ecs_isa(base)});

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e2, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(0, ecs_field_src(&it, 0));
    {
        Position *p = ecs_field_at(&it, Position, 0, 0);
        test_assert(p != NULL);
        test_int(p->x, 30); test_int(p->y, 40);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(0, ecs_field_src(&it, 0));
    {
        Position *p = ecs_field_at(&it, Position, 0, 0);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e3, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(base, ecs_field_src(&it, 0));
    {
        Position *p = ecs_field_at(&it, Position, 0, 0);
        test_assert(p != NULL);
        test_int(p->x, 1); test_int(p->y, 2);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e4, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(base, ecs_field_src(&it, 0));
    {
        Position *p = ecs_field_at(&it, Position, 0, 0);
        test_assert(p != NULL);
        test_int(p->x, 1); test_int(p->y, 2);
    }

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void DontFragment_1_sparse_written_self(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Foo);

    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    ecs_add_id(world, ecs_id(Position), EcsDontFragment);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Foo, Position(self)",
        .cache_kind = cache_kind
    });
    test_assert(q != NULL);

    test_bool(false, !!(q->row_fields & (1llu << 0)));
    test_bool(true, !!(q->row_fields & (1llu << 1)));

    ecs_entity_t base = ecs_insert(world, {EcsPrefab}, ecs_value(Position, {1, 2}));
    ecs_entity_t e1 = ecs_insert(world, {ecs_isa(base)}, {Foo}, ecs_value(Position, {10, 20}));
    ecs_entity_t e2 = ecs_insert(world, {ecs_isa(base)}, {Foo}, ecs_value(Position, {30, 40}));
    /* ecs_entity_t e3 = */ ecs_insert(world, {Foo}, {ecs_isa(base)});
    /* ecs_entity_t e4 = */ ecs_insert(world, {Foo}, {ecs_isa(base)});

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e1, it.entities[0]);
    {
        Position *p = ecs_field_at(&it, Position, 1, 0);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_uint(e2, it.entities[0]);
    {
        Position *p = ecs_field_at(&it, Position, 1, 0);
        test_assert(p != NULL);
        test_int(p->x, 30); test_int(p->y, 40);
    }

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void DontFragment_1_sparse_written_up(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Foo);

    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    ecs_add_id(world, ecs_id(Position), EcsDontFragment);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Foo, Position(up IsA)",
        .cache_kind = cache_kind
    });
    test_assert(q != NULL);

    test_bool(false, !!(q->row_fields & (1llu << 0)));
    test_bool(true, !!(q->row_fields & (1llu << 1)));

    ecs_entity_t base = ecs_insert(world, {EcsPrefab}, ecs_value(Position, {1, 2}));
    ecs_entity_t e1 = ecs_insert(world, {ecs_isa(base)}, {Foo}, ecs_value(Position, {10, 20}));
    ecs_entity_t e2 = ecs_insert(world, {ecs_isa(base)}, {Foo}, ecs_value(Position, {30, 40}));
    ecs_entity_t e3 = ecs_insert(world, {ecs_isa(base)}, {Foo});
    ecs_entity_t e4 = ecs_insert(world, {ecs_isa(base)}, {Foo});
    /* ecs_entity_t e5 = */ ecs_insert(world, {Foo});

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(4, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(e2, it.entities[1]);
    test_uint(e3, it.entities[2]);
    test_uint(e4, it.entities[3]);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(base, ecs_field_src(&it, 1));
    {
        Position *p = ecs_field_at(&it, Position, 1, 0);
        test_assert(p != NULL);
        test_int(p->x, 1); test_int(p->y, 2);
    }

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void DontFragment_1_sparse_written_self_up(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Foo);

    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    ecs_add_id(world, ecs_id(Position), EcsDontFragment);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Foo, Position(self|up IsA)",
        .cache_kind = cache_kind
    });
    test_assert(q != NULL);

    test_bool(false, !!(q->row_fields & (1llu << 0)));
    test_bool(true, !!(q->row_fields & (1llu << 1)));

    ecs_entity_t base = ecs_insert(world, {EcsPrefab}, ecs_value(Position, {1, 2}));
    ecs_entity_t e1 = ecs_insert(world, {ecs_isa(base)}, {Foo}, ecs_value(Position, {10, 20}));
    ecs_entity_t e2 = ecs_insert(world, {ecs_isa(base)}, {Foo}, ecs_value(Position, {30, 40}));
    ecs_entity_t e3 = ecs_insert(world, {ecs_isa(base)}, {Foo});
    ecs_entity_t e4 = ecs_insert(world, {ecs_isa(base)}, {Foo});
    /* ecs_entity_t e5 = */ ecs_insert(world, {Foo});

    ecs_iter_t it = ecs_query_iter(world, q);

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e1, it.entities[0]);
    {
        Position *p = ecs_field_at(&it, Position, 1, 0);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_uint(e2, it.entities[0]);
    {
        Position *p = ecs_field_at(&it, Position, 1, 0);
        test_assert(p != NULL);
        test_int(p->x, 30); test_int(p->y, 40);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e3, it.entities[0]);
    {
        Position *p = ecs_field_at(&it, Position, 1, 0);
        test_assert(p != NULL);
        test_int(p->x, 1); test_int(p->y, 2);
    }

    test_bool(true, ecs_query_next(&it));
    test_uint(e4, it.entities[0]);
    {
        Position *p = ecs_field_at(&it, Position, 1, 0);
        test_assert(p != NULL);
        test_int(p->x, 1); test_int(p->y, 2);
    }

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void DontFragment_1_fixed_sparse_not(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_add_id(world, ecs_id(Position), EcsDontFragment);

    ecs_entity_t e = ecs_entity(world, { .name = "e" });

    ecs_query_t *q = ecs_query(world, {
        .expr = "!Position(e)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(e, ecs_field_src(&it, 0));
        test_bool(false, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_set(world, e, Position, {10, 20});

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void DontFragment_1_this_sparse_not(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_add_id(world, ecs_id(Position), EcsDontFragment);

    ecs_query_t *q = ecs_query(world, {
        .expr = "!Position",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    test_bool(false, !!(q->row_fields & (1llu << 0)));

    ecs_entity_t e1 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(Position, {70, 80}));

    int32_t iterated = 0;

    ecs_iter_t it = ecs_query_iter(world, q);
    while (ecs_query_next(&it)) {
        test_bool(false, ecs_field_is_set(&it, 0));

        for (int i = 0; i < it.count; i ++) {
            test_assert(it.entities[i] != e1);
            test_assert(it.entities[i] != e2);
            iterated ++;
        }
    }

    test_assert(iterated != 0);

    ecs_query_fini(q);

    ecs_fini(world);
}

void DontFragment_1_var_sparse_not(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_add_id(world, ecs_id(Position), EcsDontFragment);

    ecs_query_t *q = ecs_query(world, {
        .expr = "!Position($x)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    test_bool(false, !!(q->row_fields & (1llu << 0)));

    ecs_entity_t e1 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(Position, {70, 80}));

    int32_t iterated = 0;

    ecs_iter_t it = ecs_query_iter(world, q);
    while (ecs_query_next(&it)) {
        test_int(it.count, 0);
        test_bool(false, ecs_field_is_set(&it, 0));
        test_assert(ecs_iter_get_var(&it, x_var) != e1);
        test_assert(ecs_iter_get_var(&it, x_var) != e2);
        iterated ++;
    }

    test_assert(iterated != 0);

    ecs_query_fini(q);

    ecs_fini(world);
}

void DontFragment_1_this_sparse_written_not(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_COMPONENT(world, Position);

    ecs_add_id(world, ecs_id(Position), EcsDontFragment);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Foo, !Position",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    test_bool(false, !!(q->row_fields & (1llu << 0)));
    test_bool(false, !!(q->row_fields & (1llu << 1)));

    /* ecs_entity_t e1 = */ ecs_insert(world, {Foo}, ecs_value(Position, {10, 20}));
    ecs_entity_t e2 = ecs_insert(world, {Foo});
    ecs_entity_t e3 = ecs_insert(world, {Foo});
    /* ecs_entity_t e4 = */ ecs_insert(world, {Foo}, ecs_value(Position, {70, 80}));

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e2, it.entities[0]);
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(false, ecs_field_is_set(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e3, it.entities[0]);
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(false, ecs_field_is_set(&it, 1));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void DontFragment_1_var_sparse_written_not(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_COMPONENT(world, Position);

    ecs_add_id(world, ecs_id(Position), EcsDontFragment);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Foo($x), !Position($x)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    test_bool(false, !!(q->row_fields & (1llu << 0)));
    test_bool(false, !!(q->row_fields & (1llu << 1)));

    /* ecs_entity_t e1 = */ ecs_insert(world, {Foo}, ecs_value(Position, {10, 20}));
    ecs_entity_t e2 = ecs_insert(world, {Foo});
    ecs_entity_t e3 = ecs_insert(world, {Foo});
    /* ecs_entity_t e4 = */ ecs_insert(world, {Foo}, ecs_value(Position, {70, 80}));

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(e2, ecs_iter_get_var(&it, x_var));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(false, ecs_field_is_set(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(e3, ecs_iter_get_var(&it, x_var));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(false, ecs_field_is_set(&it, 1));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void DontFragment_1_fixed_sparse_pair_not(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Movement, DontFragment);
    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);
    ECS_TAG(world, Sitting);

    ecs_entity_t e1 = ecs_entity(world, { .name = "ent" });

    ecs_query_t *q = ecs_query(world, {
        .expr = "!Movement(ent, Walking)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(e1, ecs_field_src(&it, 0));
        test_uint(ecs_pair(Movement, Walking), ecs_field_id(&it, 0));
        test_bool(false, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_pair(world, e1, Movement, Walking);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void DontFragment_1_fixed_sparse_pair_exclusive_not(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Movement, DontFragment, Exclusive);
    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);
    ECS_TAG(world, Sitting);

    ecs_entity_t e1 = ecs_entity(world, { .name = "ent" });

    ecs_query_t *q = ecs_query(world, {
        .expr = "!Movement(ent, Walking)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(e1, ecs_field_src(&it, 0));
        test_uint(ecs_pair(Movement, Walking), ecs_field_id(&it, 0));
        test_bool(false, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_pair(world, e1, Movement, Walking);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void DontFragment_1_fixed_sparse_pair_wildcard_not(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Movement, DontFragment);
    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);
    ECS_TAG(world, Sitting);

    ecs_entity_t e1 = ecs_entity(world, { .name = "ent" });

    ecs_query_t *q = ecs_query(world, {
        .expr = "!Movement(ent, *)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(e1, ecs_field_src(&it, 0));
        test_uint(ecs_pair(Movement, EcsWildcard), ecs_field_id(&it, 0));
        test_bool(false, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_pair(world, e1, Movement, Walking);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void DontFragment_1_fixed_sparse_pair_exclusive_wildcard_not(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Movement, DontFragment, Exclusive);
    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);
    ECS_TAG(world, Sitting);

    ecs_entity_t e1 = ecs_entity(world, { .name = "ent" });

    ecs_query_t *q = ecs_query(world, {
        .expr = "!Movement(ent, *)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(e1, ecs_field_src(&it, 0));
        test_uint(ecs_pair(Movement, EcsWildcard), ecs_field_id(&it, 0));
        test_bool(false, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_pair(world, e1, Movement, Walking);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void DontFragment_1_fixed_sparse_pair_any_not(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Movement, DontFragment, Exclusive);
    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);
    ECS_TAG(world, Sitting);

    ecs_entity_t e1 = ecs_entity(world, { .name = "ent" });

    ecs_query_t *q = ecs_query(world, {
        .expr = "!Movement(ent, _)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(e1, ecs_field_src(&it, 0));
        test_uint(ecs_pair(Movement, EcsWildcard), ecs_field_id(&it, 0));
        test_bool(false, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_pair(world, e1, Movement, Walking);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void DontFragment_1_fixed_sparse_pair_exclusive_any_not(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Movement, DontFragment, Exclusive);
    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);
    ECS_TAG(world, Sitting);

    ecs_entity_t e1 = ecs_entity(world, { .name = "ent" });

    ecs_query_t *q = ecs_query(world, {
        .expr = "!Movement(ent, _)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(e1, ecs_field_src(&it, 0));
        test_uint(ecs_pair(Movement, EcsWildcard), ecs_field_id(&it, 0));
        test_bool(false, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_pair(world, e1, Movement, Walking);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void DontFragment_1_this_sparse_pair_not(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Movement, DontFragment);
    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);
    ECS_TAG(world, Sitting);

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);
    ECS_TAG(world, TagA);

    ecs_entity_t e1 = ecs_new(world);
    ecs_entity_t e2 = ecs_new(world);
    ecs_entity_t e3 = ecs_new(world);
    ecs_entity_t e4 = ecs_new(world);
    ecs_entity_t e5 = ecs_new(world);
    ecs_entity_t e6 = ecs_new(world);

    ecs_add_pair(world, e1, Movement, Walking);
    ecs_add_pair(world, e2, Movement, Walking);
    ecs_add_pair(world, e3, Movement, Running);
    ecs_add_pair(world, e4, Movement, Running);
    ecs_add_pair(world, e5, Movement, Running);
    ecs_add_pair(world, e6, Movement, Sitting);

    ecs_add(world, e1, TagA);
    ecs_add(world, e2, TagA);
    ecs_add(world, e3, TagA);
    ecs_add(world, e4, TagA);
    ecs_add(world, e5, TagA);
    ecs_add(world, e6, TagA);

    ecs_entity_t e7 = ecs_new_w(world, TagA);

    ecs_add(world, e3, Foo);
    ecs_add(world, e4, Foo);
    ecs_add(world, e5, Bar);
    ecs_add(world, e6, Bar);

    ecs_query_t *q = ecs_query(world, {
        .expr = "TagA, !(Movement, Walking)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e7, it.entities[0]);
    test_uint(TagA, ecs_field_id(&it, 0));
    test_uint(ecs_pair(Movement, Walking), ecs_field_id(&it, 1));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(false, ecs_field_is_set(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e3, it.entities[0]);
    test_uint(TagA, ecs_field_id(&it, 0));
    test_uint(ecs_pair(Movement, Walking), ecs_field_id(&it, 1));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(false, ecs_field_is_set(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e4, it.entities[0]);
    test_uint(TagA, ecs_field_id(&it, 0));
    test_uint(ecs_pair(Movement, Walking), ecs_field_id(&it, 1));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(false, ecs_field_is_set(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e5, it.entities[0]);
    test_uint(TagA, ecs_field_id(&it, 0));
    test_uint(ecs_pair(Movement, Walking), ecs_field_id(&it, 1));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(false, ecs_field_is_set(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e6, it.entities[0]);
    test_uint(TagA, ecs_field_id(&it, 0));
    test_uint(ecs_pair(Movement, Walking), ecs_field_id(&it, 1));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(false, ecs_field_is_set(&it, 1));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void DontFragment_1_this_sparse_pair_wildcard_not(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Movement, DontFragment);
    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);
    ECS_TAG(world, Sitting);

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);
    ECS_TAG(world, TagA);

    ecs_entity_t e1 = ecs_new(world);
    ecs_entity_t e2 = ecs_new(world);
    ecs_entity_t e3 = ecs_new(world);
    ecs_entity_t e4 = ecs_new(world);
    ecs_entity_t e5 = ecs_new(world);
    ecs_entity_t e6 = ecs_new(world);

    ecs_add_pair(world, e1, Movement, Walking);
    ecs_add_pair(world, e2, Movement, Walking);
    ecs_add_pair(world, e3, Movement, Running);
    ecs_add_pair(world, e4, Movement, Running);
    ecs_add_pair(world, e5, Movement, Running);
    ecs_add_pair(world, e6, Movement, Sitting);

    ecs_add(world, e1, TagA);
    ecs_add(world, e2, TagA);
    ecs_add(world, e3, TagA);
    ecs_add(world, e4, TagA);
    ecs_add(world, e5, TagA);
    ecs_add(world, e6, TagA);

    ecs_entity_t e7 = ecs_new_w(world, TagA);

    ecs_add(world, e3, Foo);
    ecs_add(world, e4, Foo);
    ecs_add(world, e5, Bar);
    ecs_add(world, e6, Bar);

    ecs_query_t *q = ecs_query(world, {
        .expr = "TagA, !(Movement, *)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e7, it.entities[0]);
    test_uint(TagA, ecs_field_id(&it, 0));
    test_uint(ecs_pair(Movement, EcsWildcard), ecs_field_id(&it, 1));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(false, ecs_field_is_set(&it, 1));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void DontFragment_1_this_sparse_pair_exclusive_not(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Movement, DontFragment, Exclusive);
    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);
    ECS_TAG(world, Sitting);

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);
    ECS_TAG(world, TagA);

    ecs_entity_t e1 = ecs_new(world);
    ecs_entity_t e2 = ecs_new(world);
    ecs_entity_t e3 = ecs_new(world);
    ecs_entity_t e4 = ecs_new(world);
    ecs_entity_t e5 = ecs_new(world);
    ecs_entity_t e6 = ecs_new(world);

    ecs_add_pair(world, e1, Movement, Walking);
    ecs_add_pair(world, e2, Movement, Walking);
    ecs_add_pair(world, e3, Movement, Running);
    ecs_add_pair(world, e4, Movement, Running);
    ecs_add_pair(world, e5, Movement, Running);
    ecs_add_pair(world, e6, Movement, Sitting);

    ecs_add(world, e1, TagA);
    ecs_add(world, e2, TagA);
    ecs_add(world, e3, TagA);
    ecs_add(world, e4, TagA);
    ecs_add(world, e5, TagA);
    ecs_add(world, e6, TagA);

    ecs_entity_t e7 = ecs_new_w(world, TagA);

    ecs_add(world, e3, Foo);
    ecs_add(world, e4, Foo);
    ecs_add(world, e5, Bar);
    ecs_add(world, e6, Bar);

    ecs_query_t *q = ecs_query(world, {
        .expr = "TagA, !(Movement, Walking)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e7, it.entities[0]);
    test_uint(TagA, ecs_field_id(&it, 0));
    test_uint(ecs_pair(Movement, Walking), ecs_field_id(&it, 1));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(false, ecs_field_is_set(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e3, it.entities[0]);
    test_uint(TagA, ecs_field_id(&it, 0));
    test_uint(ecs_pair(Movement, Walking), ecs_field_id(&it, 1));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(false, ecs_field_is_set(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e4, it.entities[0]);
    test_uint(TagA, ecs_field_id(&it, 0));
    test_uint(ecs_pair(Movement, Walking), ecs_field_id(&it, 1));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(false, ecs_field_is_set(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e5, it.entities[0]);
    test_uint(TagA, ecs_field_id(&it, 0));
    test_uint(ecs_pair(Movement, Walking), ecs_field_id(&it, 1));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(false, ecs_field_is_set(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e6, it.entities[0]);
    test_uint(TagA, ecs_field_id(&it, 0));
    test_uint(ecs_pair(Movement, Walking), ecs_field_id(&it, 1));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(false, ecs_field_is_set(&it, 1));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void DontFragment_1_this_sparse_pair_exclusive_wildcard_not(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Movement, DontFragment, Exclusive);
    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);
    ECS_TAG(world, Sitting);

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);
    ECS_TAG(world, TagA);

    ecs_entity_t e1 = ecs_new(world);
    ecs_entity_t e2 = ecs_new(world);
    ecs_entity_t e3 = ecs_new(world);
    ecs_entity_t e4 = ecs_new(world);
    ecs_entity_t e5 = ecs_new(world);
    ecs_entity_t e6 = ecs_new(world);

    ecs_add_pair(world, e1, Movement, Walking);
    ecs_add_pair(world, e2, Movement, Walking);
    ecs_add_pair(world, e3, Movement, Running);
    ecs_add_pair(world, e4, Movement, Running);
    ecs_add_pair(world, e5, Movement, Running);
    ecs_add_pair(world, e6, Movement, Sitting);

    ecs_add(world, e1, TagA);
    ecs_add(world, e2, TagA);
    ecs_add(world, e3, TagA);
    ecs_add(world, e4, TagA);
    ecs_add(world, e5, TagA);
    ecs_add(world, e6, TagA);

    ecs_entity_t e7 = ecs_new_w(world, TagA);

    ecs_add(world, e3, Foo);
    ecs_add(world, e4, Foo);
    ecs_add(world, e5, Bar);
    ecs_add(world, e6, Bar);

    ecs_query_t *q = ecs_query(world, {
        .expr = "TagA, !(Movement, *)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e7, it.entities[0]);
    test_uint(TagA, ecs_field_id(&it, 0));
    test_uint(ecs_pair(Movement, EcsWildcard), ecs_field_id(&it, 1));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(false, ecs_field_is_set(&it, 1));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void DontFragment_1_this_sparse_pair_any_not(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Movement, DontFragment);
    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);
    ECS_TAG(world, Sitting);

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);
    ECS_TAG(world, TagA);

    ecs_entity_t e1 = ecs_new(world);
    ecs_entity_t e2 = ecs_new(world);
    ecs_entity_t e3 = ecs_new(world);
    ecs_entity_t e4 = ecs_new(world);
    ecs_entity_t e5 = ecs_new(world);
    ecs_entity_t e6 = ecs_new(world);

    ecs_add_pair(world, e1, Movement, Walking);
    ecs_add_pair(world, e2, Movement, Walking);
    ecs_add_pair(world, e3, Movement, Running);
    ecs_add_pair(world, e4, Movement, Running);
    ecs_add_pair(world, e5, Movement, Running);
    ecs_add_pair(world, e6, Movement, Sitting);

    ecs_add(world, e1, TagA);
    ecs_add(world, e2, TagA);
    ecs_add(world, e3, TagA);
    ecs_add(world, e4, TagA);
    ecs_add(world, e5, TagA);
    ecs_add(world, e6, TagA);

    ecs_entity_t e7 = ecs_new_w(world, TagA);

    ecs_add(world, e3, Foo);
    ecs_add(world, e4, Foo);
    ecs_add(world, e5, Bar);
    ecs_add(world, e6, Bar);

    ecs_query_t *q = ecs_query(world, {
        .expr = "TagA, !(Movement, _)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e7, it.entities[0]);
    test_uint(TagA, ecs_field_id(&it, 0));
    test_uint(ecs_pair(Movement, EcsWildcard), ecs_field_id(&it, 1));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(false, ecs_field_is_set(&it, 1));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void DontFragment_1_this_sparse_pair_exclusive_any_not(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Movement, DontFragment, Exclusive);
    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);
    ECS_TAG(world, Sitting);

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);
    ECS_TAG(world, TagA);

    ecs_entity_t e1 = ecs_new(world);
    ecs_entity_t e2 = ecs_new(world);
    ecs_entity_t e3 = ecs_new(world);
    ecs_entity_t e4 = ecs_new(world);
    ecs_entity_t e5 = ecs_new(world);
    ecs_entity_t e6 = ecs_new(world);

    ecs_add_pair(world, e1, Movement, Walking);
    ecs_add_pair(world, e2, Movement, Walking);
    ecs_add_pair(world, e3, Movement, Running);
    ecs_add_pair(world, e4, Movement, Running);
    ecs_add_pair(world, e5, Movement, Running);
    ecs_add_pair(world, e6, Movement, Sitting);

    ecs_add(world, e1, TagA);
    ecs_add(world, e2, TagA);
    ecs_add(world, e3, TagA);
    ecs_add(world, e4, TagA);
    ecs_add(world, e5, TagA);
    ecs_add(world, e6, TagA);

    ecs_entity_t e7 = ecs_new_w(world, TagA);

    ecs_add(world, e3, Foo);
    ecs_add(world, e4, Foo);
    ecs_add(world, e5, Bar);
    ecs_add(world, e6, Bar);

    ecs_query_t *q = ecs_query(world, {
        .expr = "TagA, !(Movement, *)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e7, it.entities[0]);
    test_uint(TagA, ecs_field_id(&it, 0));
    test_uint(ecs_pair(Movement, EcsWildcard), ecs_field_id(&it, 1));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(false, ecs_field_is_set(&it, 1));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void DontFragment_1_var_sparse_pair_not(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Movement, DontFragment);
    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);
    ECS_TAG(world, Sitting);

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);
    ECS_TAG(world, TagA);

    ecs_entity_t e1 = ecs_new(world);
    ecs_entity_t e2 = ecs_new(world);
    ecs_entity_t e3 = ecs_new(world);
    ecs_entity_t e4 = ecs_new(world);
    ecs_entity_t e5 = ecs_new(world);
    ecs_entity_t e6 = ecs_new(world);

    ecs_add_pair(world, e1, Movement, Walking);
    ecs_add_pair(world, e2, Movement, Walking);
    ecs_add_pair(world, e3, Movement, Running);
    ecs_add_pair(world, e4, Movement, Running);
    ecs_add_pair(world, e5, Movement, Running);
    ecs_add_pair(world, e6, Movement, Sitting);

    ecs_add(world, e1, TagA);
    ecs_add(world, e2, TagA);
    ecs_add(world, e3, TagA);
    ecs_add(world, e4, TagA);
    ecs_add(world, e5, TagA);
    ecs_add(world, e6, TagA);

    ecs_entity_t e7 = ecs_new_w(world, TagA);

    ecs_add(world, e3, Foo);
    ecs_add(world, e4, Foo);
    ecs_add(world, e5, Bar);
    ecs_add(world, e6, Bar);

    ecs_query_t *q = ecs_query(world, {
        .expr = "TagA($x), !Movement($x, Walking)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(e7, ecs_field_src(&it, 0));
    test_uint(e7, ecs_iter_get_var(&it, x_var));
    test_uint(TagA, ecs_field_id(&it, 0));
    test_uint(ecs_pair(Movement, Walking), ecs_field_id(&it, 1));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(false, ecs_field_is_set(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(e3, ecs_field_src(&it, 0));
    test_uint(e3, ecs_iter_get_var(&it, x_var));
    test_uint(TagA, ecs_field_id(&it, 0));
    test_uint(ecs_pair(Movement, Walking), ecs_field_id(&it, 1));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(false, ecs_field_is_set(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(e4, ecs_field_src(&it, 0));
    test_uint(e4, ecs_iter_get_var(&it, x_var));
    test_uint(TagA, ecs_field_id(&it, 0));
    test_uint(ecs_pair(Movement, Walking), ecs_field_id(&it, 1));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(false, ecs_field_is_set(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(e5, ecs_field_src(&it, 0));
    test_uint(e5, ecs_iter_get_var(&it, x_var));
    test_uint(TagA, ecs_field_id(&it, 0));
    test_uint(ecs_pair(Movement, Walking), ecs_field_id(&it, 1));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(false, ecs_field_is_set(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(e6, ecs_field_src(&it, 0));
    test_uint(e6, ecs_iter_get_var(&it, x_var));
    test_uint(TagA, ecs_field_id(&it, 0));
    test_uint(ecs_pair(Movement, Walking), ecs_field_id(&it, 1));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(false, ecs_field_is_set(&it, 1));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void DontFragment_1_var_sparse_pair_exclusive_not(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Movement, DontFragment);
    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);
    ECS_TAG(world, Sitting);

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);
    ECS_TAG(world, TagA);

    ecs_entity_t e1 = ecs_new(world);
    ecs_entity_t e2 = ecs_new(world);
    ecs_entity_t e3 = ecs_new(world);
    ecs_entity_t e4 = ecs_new(world);
    ecs_entity_t e5 = ecs_new(world);
    ecs_entity_t e6 = ecs_new(world);

    ecs_add_pair(world, e1, Movement, Walking);
    ecs_add_pair(world, e2, Movement, Walking);
    ecs_add_pair(world, e3, Movement, Running);
    ecs_add_pair(world, e4, Movement, Running);
    ecs_add_pair(world, e5, Movement, Running);
    ecs_add_pair(world, e6, Movement, Sitting);

    ecs_add(world, e1, TagA);
    ecs_add(world, e2, TagA);
    ecs_add(world, e3, TagA);
    ecs_add(world, e4, TagA);
    ecs_add(world, e5, TagA);
    ecs_add(world, e6, TagA);

    ecs_entity_t e7 = ecs_new_w(world, TagA);

    ecs_add(world, e3, Foo);
    ecs_add(world, e4, Foo);
    ecs_add(world, e5, Bar);
    ecs_add(world, e6, Bar);

    ecs_query_t *q = ecs_query(world, {
        .expr = "TagA($x), !Movement($x, Walking)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(e7, ecs_field_src(&it, 0));
    test_uint(e7, ecs_iter_get_var(&it, x_var));
    test_uint(TagA, ecs_field_id(&it, 0));
    test_uint(ecs_pair(Movement, Walking), ecs_field_id(&it, 1));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(false, ecs_field_is_set(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(e3, ecs_field_src(&it, 0));
    test_uint(e3, ecs_iter_get_var(&it, x_var));
    test_uint(TagA, ecs_field_id(&it, 0));
    test_uint(ecs_pair(Movement, Walking), ecs_field_id(&it, 1));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(false, ecs_field_is_set(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(e4, ecs_field_src(&it, 0));
    test_uint(e4, ecs_iter_get_var(&it, x_var));
    test_uint(TagA, ecs_field_id(&it, 0));
    test_uint(ecs_pair(Movement, Walking), ecs_field_id(&it, 1));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(false, ecs_field_is_set(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(e5, ecs_field_src(&it, 0));
    test_uint(e5, ecs_iter_get_var(&it, x_var));
    test_uint(TagA, ecs_field_id(&it, 0));
    test_uint(ecs_pair(Movement, Walking), ecs_field_id(&it, 1));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(false, ecs_field_is_set(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(e6, ecs_field_src(&it, 0));
    test_uint(e6, ecs_iter_get_var(&it, x_var));
    test_uint(TagA, ecs_field_id(&it, 0));
    test_uint(ecs_pair(Movement, Walking), ecs_field_id(&it, 1));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(false, ecs_field_is_set(&it, 1));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void DontFragment_1_var_sparse_pair_wildcard_not(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Movement, DontFragment);
    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);
    ECS_TAG(world, Sitting);

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);
    ECS_TAG(world, TagA);

    ecs_entity_t e1 = ecs_new(world);
    ecs_entity_t e2 = ecs_new(world);
    ecs_entity_t e3 = ecs_new(world);
    ecs_entity_t e4 = ecs_new(world);
    ecs_entity_t e5 = ecs_new(world);
    ecs_entity_t e6 = ecs_new(world);

    ecs_add_pair(world, e1, Movement, Walking);
    ecs_add_pair(world, e2, Movement, Walking);
    ecs_add_pair(world, e3, Movement, Running);
    ecs_add_pair(world, e4, Movement, Running);
    ecs_add_pair(world, e5, Movement, Running);
    ecs_add_pair(world, e6, Movement, Sitting);

    ecs_add(world, e1, TagA);
    ecs_add(world, e2, TagA);
    ecs_add(world, e3, TagA);
    ecs_add(world, e4, TagA);
    ecs_add(world, e5, TagA);
    ecs_add(world, e6, TagA);

    ecs_entity_t e7 = ecs_new_w(world, TagA);

    ecs_add(world, e3, Foo);
    ecs_add(world, e4, Foo);
    ecs_add(world, e5, Bar);
    ecs_add(world, e6, Bar);

    ecs_query_t *q = ecs_query(world, {
        .expr = "TagA($x), !Movement($x, *)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(e7, ecs_field_src(&it, 0));
    test_uint(e7, ecs_iter_get_var(&it, x_var));
    test_uint(TagA, ecs_field_id(&it, 0));
    test_uint(ecs_pair(Movement, EcsWildcard), ecs_field_id(&it, 1));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(false, ecs_field_is_set(&it, 1));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void DontFragment_1_var_sparse_pair_exclusive_wildcard_not(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Movement, DontFragment, Exclusive);
    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);
    ECS_TAG(world, Sitting);

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);
    ECS_TAG(world, TagA);

    ecs_entity_t e1 = ecs_new(world);
    ecs_entity_t e2 = ecs_new(world);
    ecs_entity_t e3 = ecs_new(world);
    ecs_entity_t e4 = ecs_new(world);
    ecs_entity_t e5 = ecs_new(world);
    ecs_entity_t e6 = ecs_new(world);

    ecs_add_pair(world, e1, Movement, Walking);
    ecs_add_pair(world, e2, Movement, Walking);
    ecs_add_pair(world, e3, Movement, Running);
    ecs_add_pair(world, e4, Movement, Running);
    ecs_add_pair(world, e5, Movement, Running);
    ecs_add_pair(world, e6, Movement, Sitting);

    ecs_add(world, e1, TagA);
    ecs_add(world, e2, TagA);
    ecs_add(world, e3, TagA);
    ecs_add(world, e4, TagA);
    ecs_add(world, e5, TagA);
    ecs_add(world, e6, TagA);

    ecs_entity_t e7 = ecs_new_w(world, TagA);

    ecs_add(world, e3, Foo);
    ecs_add(world, e4, Foo);
    ecs_add(world, e5, Bar);
    ecs_add(world, e6, Bar);

    ecs_query_t *q = ecs_query(world, {
        .expr = "TagA($x), !Movement($x, *)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(e7, ecs_field_src(&it, 0));
    test_uint(e7, ecs_iter_get_var(&it, x_var));
    test_uint(TagA, ecs_field_id(&it, 0));
    test_uint(ecs_pair(Movement, EcsWildcard), ecs_field_id(&it, 1));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(false, ecs_field_is_set(&it, 1));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void DontFragment_1_var_sparse_pair_any_not(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Movement, DontFragment);
    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);
    ECS_TAG(world, Sitting);

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);
    ECS_TAG(world, TagA);

    ecs_entity_t e1 = ecs_new(world);
    ecs_entity_t e2 = ecs_new(world);
    ecs_entity_t e3 = ecs_new(world);
    ecs_entity_t e4 = ecs_new(world);
    ecs_entity_t e5 = ecs_new(world);
    ecs_entity_t e6 = ecs_new(world);

    ecs_add_pair(world, e1, Movement, Walking);
    ecs_add_pair(world, e2, Movement, Walking);
    ecs_add_pair(world, e3, Movement, Running);
    ecs_add_pair(world, e4, Movement, Running);
    ecs_add_pair(world, e5, Movement, Running);
    ecs_add_pair(world, e6, Movement, Sitting);

    ecs_add(world, e1, TagA);
    ecs_add(world, e2, TagA);
    ecs_add(world, e3, TagA);
    ecs_add(world, e4, TagA);
    ecs_add(world, e5, TagA);
    ecs_add(world, e6, TagA);

    ecs_entity_t e7 = ecs_new_w(world, TagA);

    ecs_add(world, e3, Foo);
    ecs_add(world, e4, Foo);
    ecs_add(world, e5, Bar);
    ecs_add(world, e6, Bar);

    ecs_query_t *q = ecs_query(world, {
        .expr = "TagA($x), !Movement($x, _)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(e7, ecs_field_src(&it, 0));
    test_uint(e7, ecs_iter_get_var(&it, x_var));
    test_uint(TagA, ecs_field_id(&it, 0));
    test_uint(ecs_pair(Movement, EcsWildcard), ecs_field_id(&it, 1));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(false, ecs_field_is_set(&it, 1));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void DontFragment_1_var_sparse_pair_exclusive_any_not(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Movement, DontFragment, Exclusive);
    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);
    ECS_TAG(world, Sitting);

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);
    ECS_TAG(world, TagA);

    ecs_entity_t e1 = ecs_new(world);
    ecs_entity_t e2 = ecs_new(world);
    ecs_entity_t e3 = ecs_new(world);
    ecs_entity_t e4 = ecs_new(world);
    ecs_entity_t e5 = ecs_new(world);
    ecs_entity_t e6 = ecs_new(world);

    ecs_add_pair(world, e1, Movement, Walking);
    ecs_add_pair(world, e2, Movement, Walking);
    ecs_add_pair(world, e3, Movement, Running);
    ecs_add_pair(world, e4, Movement, Running);
    ecs_add_pair(world, e5, Movement, Running);
    ecs_add_pair(world, e6, Movement, Sitting);

    ecs_add(world, e1, TagA);
    ecs_add(world, e2, TagA);
    ecs_add(world, e3, TagA);
    ecs_add(world, e4, TagA);
    ecs_add(world, e5, TagA);
    ecs_add(world, e6, TagA);

    ecs_entity_t e7 = ecs_new_w(world, TagA);

    ecs_add(world, e3, Foo);
    ecs_add(world, e4, Foo);
    ecs_add(world, e5, Bar);
    ecs_add(world, e6, Bar);

    ecs_query_t *q = ecs_query(world, {
        .expr = "TagA($x), !Movement($x, _)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(e7, ecs_field_src(&it, 0));
    test_uint(e7, ecs_iter_get_var(&it, x_var));
    test_uint(TagA, ecs_field_id(&it, 0));
    test_uint(ecs_pair(Movement, EcsWildcard), ecs_field_id(&it, 1));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(false, ecs_field_is_set(&it, 1));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void DontFragment_1_sparse_component_unused(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    
    ecs_add_id(world, ecs_id(Position), EcsDontFragment);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ ecs_id(Position) }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void DontFragment_1_sparse_tag_unused(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t tag = ecs_new(world);
    
    ecs_add_id(world, tag, EcsDontFragment);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ tag }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void DontFragment_1_sparse_pair_wildcard_unused(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t tag = ecs_new(world);
    
    ecs_add_id(world, tag, EcsDontFragment);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ ecs_pair(tag, EcsWildcard) }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void DontFragment_1_sparse_pair_unused(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t tag = ecs_new(world);
    ecs_entity_t tgt = ecs_new(world);
    
    ecs_add_id(world, tag, EcsDontFragment);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ ecs_pair(tag, tgt) }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void DontFragment_2_this_written_sparse_component_unused(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_COMPONENT(world, Position);
    
    ecs_add_id(world, ecs_id(Position), EcsDontFragment);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ Foo }, { ecs_id(Position) }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_new_w(world, Foo);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void DontFragment_2_this_written_sparse_tag_unused(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);
    
    ecs_add_id(world, Bar, EcsDontFragment);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ Foo }, { Bar }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_new_w(world, Foo);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void DontFragment_2_this_written_sparse_pair_wildcard_unused(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ecs_entity_t tag = ecs_new(world);
    
    ecs_add_id(world, tag, EcsDontFragment);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ Foo }, { ecs_pair(tag, EcsWildcard) }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_new_w(world, Foo);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void DontFragment_2_this_written_sparse_pair_unused(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ecs_entity_t tag = ecs_new(world);
    ecs_entity_t tgt = ecs_new(world);
    
    ecs_add_id(world, tag, EcsDontFragment);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ Foo }, { ecs_pair(tag, tgt) }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_new_w(world, Foo);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void DontFragment_this_written_sparse_any(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Movement, DontFragment);
    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);
    ECS_TAG(world, Sitting);

    ECS_TAG(world, TagA);
    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_entity_t e1 = ecs_new(world);
    ecs_entity_t e2 = ecs_new(world);
    ecs_entity_t e3 = ecs_new(world);
    ecs_entity_t e4 = ecs_new(world);
    ecs_entity_t e5 = ecs_new(world);
    ecs_entity_t e6 = ecs_new(world);

    ecs_add_pair(world, e1, Movement, Walking);
    ecs_add_pair(world, e2, Movement, Walking);
    ecs_add_pair(world, e3, Movement, Running);
    ecs_add_pair(world, e4, Movement, Running);
    ecs_add_pair(world, e5, Movement, Running);
    ecs_add_pair(world, e6, Movement, Sitting);

    ecs_add(world, e1, TagA);
    ecs_add(world, e2, TagA);
    ecs_add(world, e3, TagA);
    ecs_add(world, e4, TagA);
    ecs_add(world, e5, TagA);
    ecs_add(world, e6, TagA);

    /* ecs_entity_t e7 = */ ecs_new_w(world, TagA);

    ecs_add(world, e3, Foo);
    ecs_add(world, e4, Foo);
    ecs_add(world, e5, Bar);
    ecs_add(world, e6, Bar);

    ecs_query_t *q = ecs_query(world, {
        .expr = "TagA, (Movement, _)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(TagA, ecs_field_id(&it, 0));
    test_uint(ecs_pair(Movement, EcsWildcard), ecs_field_id(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e2, it.entities[0]);
    test_uint(TagA, ecs_field_id(&it, 0));
    test_uint(ecs_pair(Movement, EcsWildcard), ecs_field_id(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e3, it.entities[0]);
    test_uint(TagA, ecs_field_id(&it, 0));
    test_uint(ecs_pair(Movement, EcsWildcard), ecs_field_id(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e4, it.entities[0]);
    test_uint(TagA, ecs_field_id(&it, 0));
    test_uint(ecs_pair(Movement, EcsWildcard), ecs_field_id(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e6, it.entities[0]);
    test_uint(TagA, ecs_field_id(&it, 0));
    test_uint(ecs_pair(Movement, EcsWildcard), ecs_field_id(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e5, it.entities[0]);
    test_uint(TagA, ecs_field_id(&it, 0));
    test_uint(ecs_pair(Movement, EcsWildcard), ecs_field_id(&it, 1));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void DontFragment_this_written_sparse_exclusive_any(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Movement, DontFragment, Exclusive);
    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);
    ECS_TAG(world, Sitting);

    ECS_TAG(world, TagA);
    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_entity_t e1 = ecs_new(world);
    ecs_entity_t e2 = ecs_new(world);
    ecs_entity_t e3 = ecs_new(world);
    ecs_entity_t e4 = ecs_new(world);
    ecs_entity_t e5 = ecs_new(world);
    ecs_entity_t e6 = ecs_new(world);

    ecs_add_pair(world, e1, Movement, Walking);
    ecs_add_pair(world, e2, Movement, Walking);
    ecs_add_pair(world, e3, Movement, Running);
    ecs_add_pair(world, e4, Movement, Running);
    ecs_add_pair(world, e5, Movement, Running);
    ecs_add_pair(world, e6, Movement, Sitting);

    ecs_add(world, e1, TagA);
    ecs_add(world, e2, TagA);
    ecs_add(world, e3, TagA);
    ecs_add(world, e4, TagA);
    ecs_add(world, e5, TagA);
    ecs_add(world, e6, TagA);

    /* ecs_entity_t e7 = */ ecs_new_w(world, TagA);

    ecs_add(world, e3, Foo);
    ecs_add(world, e4, Foo);
    ecs_add(world, e5, Bar);
    ecs_add(world, e6, Bar);

    ecs_query_t *q = ecs_query(world, {
        .expr = "TagA, (Movement, _)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(TagA, ecs_field_id(&it, 0));
    test_uint(ecs_pair(Movement, EcsWildcard), ecs_field_id(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e2, it.entities[0]);
    test_uint(TagA, ecs_field_id(&it, 0));
    test_uint(ecs_pair(Movement, EcsWildcard), ecs_field_id(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e3, it.entities[0]);
    test_uint(TagA, ecs_field_id(&it, 0));
    test_uint(ecs_pair(Movement, EcsWildcard), ecs_field_id(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e4, it.entities[0]);
    test_uint(TagA, ecs_field_id(&it, 0));
    test_uint(ecs_pair(Movement, EcsWildcard), ecs_field_id(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e5, it.entities[0]);
    test_uint(TagA, ecs_field_id(&it, 0));
    test_uint(ecs_pair(Movement, EcsWildcard), ecs_field_id(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e6, it.entities[0]);
    test_uint(TagA, ecs_field_id(&it, 0));
    test_uint(ecs_pair(Movement, EcsWildcard), ecs_field_id(&it, 1));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void DontFragment_sparse_0_src_only_term(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_add_id(world, ecs_id(Position), EcsDontFragment);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Position(#0)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    test_bool(false, !!(q->row_fields & (1llu << 0)));

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_bool(false, ecs_field_is_set(&it, 0));
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void DontFragment_sparse_0_src(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Foo);

    ecs_add_id(world, ecs_id(Position), EcsDontFragment);

    ecs_entity_t e = ecs_new_w(world, Foo);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Position(#0), Foo",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    test_bool(false, !!(q->row_fields & (1llu << 0)));

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));
    test_bool(false, ecs_field_is_set(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 1));
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void DontFragment_sparse_pair_first(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Tgt);

    ecs_add_id(world, ecs_id(Position), EcsDontFragment);

    ecs_entity_t e1 = ecs_new(world);
    ecs_set_pair(world, e1, Position, Tgt, {10, 20});

    ecs_query_t *q = ecs_query(world, {
        .expr = "(Position, Tgt)",
        .cache_kind = cache_kind
    });

    ecs_iter_t it = ecs_query_iter(world, q);

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(ecs_pair_t(Position, Tgt), ecs_field_id(&it, 0));
    {
        Position *p = ecs_field_at(&it, Position, 0, 0);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 20);
    }

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void DontFragment_sparse_pair_second(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_COMPONENT(world, Position);

    ecs_add_id(world, ecs_id(Position), EcsDontFragment);

    ecs_entity_t e1 = ecs_new(world);
    ecs_set_pair_second(world, e1, Rel, Position, {10, 20});

    ecs_query_t *q = ecs_query(world, {
        .expr = "(Rel, Position)",
        .cache_kind = cache_kind
    });

    ecs_iter_t it = ecs_query_iter(world, q);

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(ecs_pair(Rel, ecs_id(Position)), ecs_field_id(&it, 0));
    {
        Position *p = ecs_field_at(&it, Position, 0, 0);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 20);
    }

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void DontFragment_sparse_pair_first_after_query(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Tgt);

    ecs_add_id(world, ecs_id(Position), EcsDontFragment);

    ecs_query_t *q = ecs_query(world, {
        .expr = "(Position, Tgt)",
        .cache_kind = cache_kind
    });

    ecs_entity_t e1 = ecs_new(world);
    ecs_set_pair(world, e1, Position, Tgt, {10, 20});

    ecs_iter_t it = ecs_query_iter(world, q);

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(ecs_pair_t(Position, Tgt), ecs_field_id(&it, 0));
    {
        Position *p = ecs_field_at(&it, Position, 0, 0);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 20);
    }

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void DontFragment_sparse_pair_second_after_query(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_COMPONENT(world, Position);

    ecs_add_id(world, ecs_id(Position), EcsDontFragment);

    ecs_query_t *q = ecs_query(world, {
        .expr = "(Rel, Position)",
        .cache_kind = cache_kind
    });

    ecs_entity_t e1 = ecs_new(world);
    ecs_set_pair_second(world, e1, Rel, Position, {10, 20});

    ecs_iter_t it = ecs_query_iter(world, q);

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(ecs_pair(Rel, ecs_id(Position)), ecs_field_id(&it, 0));
    {
        Position *p = ecs_field_at(&it, Position, 0, 0);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 20);
    }

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void DontFragment_ignore_prefab_disabled(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ecs_add_id(world, ecs_id(Position), EcsDontFragment);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Position",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_entity_t e1 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(Position, {30, 40}));
    ecs_entity_t e3 = ecs_insert(world, ecs_value(Position, {50, 60}));

    ecs_add_id(world, e2, EcsDisabled);
    ecs_add_id(world, e3, EcsPrefab);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e1, it.entities[0]);
    {
        Position *p = ecs_field_at(&it, Position, 0, 0);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void DontFragment_match_prefab_ignore_disabled(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ecs_add_id(world, ecs_id(Position), EcsDontFragment);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Position",
        .cache_kind = cache_kind,
        .flags = EcsQueryMatchPrefab
    });

    test_assert(q != NULL);

    ecs_entity_t e1 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(Position, {30, 40}));
    ecs_entity_t e3 = ecs_insert(world, ecs_value(Position, {50, 60}));

    ecs_add_id(world, e2, EcsDisabled);
    ecs_add_id(world, e3, EcsPrefab);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e3, it.entities[0]);
    {
        Position *p = ecs_field_at(&it, Position, 0, 0);
        test_assert(p != NULL);
        test_int(p->x, 50); test_int(p->y, 60);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e1, it.entities[0]);
    {
        Position *p = ecs_field_at(&it, Position, 0, 0);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void DontFragment_ignore_prefab_match_disabled(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ecs_add_id(world, ecs_id(Position), EcsDontFragment);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Position",
        .cache_kind = cache_kind,
        .flags = EcsQueryMatchDisabled
    });

    test_assert(q != NULL);

    ecs_entity_t e1 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(Position, {30, 40}));
    ecs_entity_t e3 = ecs_insert(world, ecs_value(Position, {50, 60}));

    ecs_add_id(world, e2, EcsDisabled);
    ecs_add_id(world, e3, EcsPrefab);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e2, it.entities[0]);
    {
        Position *p = ecs_field_at(&it, Position, 0, 0);
        test_assert(p != NULL);
        test_int(p->x, 30); test_int(p->y, 40);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e1, it.entities[0]);
    {
        Position *p = ecs_field_at(&it, Position, 0, 0);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void DontFragment_match_prefab_disabled(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ecs_add_id(world, ecs_id(Position), EcsDontFragment);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Position",
        .cache_kind = cache_kind,
        .flags = EcsQueryMatchDisabled|EcsQueryMatchPrefab
    });

    test_assert(q != NULL);

    ecs_entity_t e1 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(Position, {30, 40}));
    ecs_entity_t e3 = ecs_insert(world, ecs_value(Position, {50, 60}));

    ecs_add_id(world, e2, EcsDisabled);
    ecs_add_id(world, e3, EcsPrefab);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e3, it.entities[0]);
    {
        Position *p = ecs_field_at(&it, Position, 0, 0);
        test_assert(p != NULL);
        test_int(p->x, 50); test_int(p->y, 60);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e2, it.entities[0]);
    {
        Position *p = ecs_field_at(&it, Position, 0, 0);
        test_assert(p != NULL);
        test_int(p->x, 30); test_int(p->y, 40);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e1, it.entities[0]);
    {
        Position *p = ecs_field_at(&it, Position, 0, 0);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void DontFragment_1_fixed_sparse_second_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);
    ECS_TAG(world, TgtC);

    ecs_add_id(world, Rel, EcsDontFragment);

    ecs_entity_t ent = ecs_entity(world, { .name = "ent" });
    ecs_add_pair(world, ent, Rel, TgtA);
    ecs_add_pair(world, ent, Rel, TgtB);

    ecs_add_pair(world, ent, Rel, TgtC);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Rel(ent, *)",
        .cache_kind = cache_kind
    });
    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(ecs_pair(Rel, TgtC), ecs_field_id(&it, 0));
    test_uint(ent, ecs_field_src(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(ecs_pair(Rel, TgtB), ecs_field_id(&it, 0));
    test_uint(ent, ecs_field_src(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(ecs_pair(Rel, TgtA), ecs_field_id(&it, 0));
    test_uint(ent, ecs_field_src(&it, 0));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void DontFragment_1_this_sparse_second_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);
    ECS_TAG(world, TgtC);

    ecs_add_id(world, Rel, EcsDontFragment);

    ecs_query_t *q = ecs_query(world, {
        .expr = "(Rel, *)",
        .cache_kind = cache_kind
    });
    test_assert(q != NULL);

    ecs_entity_t e1 = ecs_new(world);
    ecs_entity_t e2 = ecs_new(world);
    ecs_entity_t e3 = ecs_new(world);

    ecs_add_pair(world, e1, Rel, TgtA);
    ecs_add_pair(world, e2, Rel, TgtA);
    ecs_add_pair(world, e2, Rel, TgtB);
    ecs_add_pair(world, e3, Rel, TgtB);
    ecs_add_pair(world, e3, Rel, TgtC);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(ecs_pair(Rel, TgtC), ecs_field_id(&it, 0));
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(e3, it.entities[0]);

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(ecs_pair(Rel, TgtB), ecs_field_id(&it, 0));
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(e3, it.entities[0]);

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(ecs_pair(Rel, TgtB), ecs_field_id(&it, 0));
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(e2, it.entities[0]);

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(ecs_pair(Rel, TgtA), ecs_field_id(&it, 0));
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(e2, it.entities[0]);

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(ecs_pair(Rel, TgtA), ecs_field_id(&it, 0));
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(e1, it.entities[0]);

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void DontFragment_1_var_sparse_second_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);
    ECS_TAG(world, TgtC);

    ecs_add_id(world, Rel, EcsDontFragment);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Rel($x, *)",
        .cache_kind = cache_kind
    });
    test_assert(q != NULL);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    ecs_entity_t e1 = ecs_new(world);
    ecs_entity_t e2 = ecs_new(world);
    ecs_entity_t e3 = ecs_new(world);

    ecs_add_pair(world, e1, Rel, TgtA);
    ecs_add_pair(world, e2, Rel, TgtA);
    ecs_add_pair(world, e2, Rel, TgtB);
    ecs_add_pair(world, e3, Rel, TgtB);
    ecs_add_pair(world, e3, Rel, TgtC);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(ecs_pair(Rel, TgtC), ecs_field_id(&it, 0));
    test_uint(e3, ecs_field_src(&it, 0));
    test_uint(e3, ecs_iter_get_var(&it, x_var));

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(ecs_pair(Rel, TgtB), ecs_field_id(&it, 0));
    test_uint(e3, ecs_field_src(&it, 0));
    test_uint(e3, ecs_iter_get_var(&it, x_var));

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(ecs_pair(Rel, TgtB), ecs_field_id(&it, 0));
    test_uint(e2, ecs_field_src(&it, 0));
    test_uint(e2, ecs_iter_get_var(&it, x_var));

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(ecs_pair(Rel, TgtA), ecs_field_id(&it, 0));
    test_uint(e2, ecs_field_src(&it, 0));
    test_uint(e2, ecs_iter_get_var(&it, x_var));

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(ecs_pair(Rel, TgtA), ecs_field_id(&it, 0));
    test_uint(e1, ecs_field_src(&it, 0));
    test_uint(e1, ecs_iter_get_var(&it, x_var));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void DontFragment_1_this_sparse_written_second_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Rel);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);
    ECS_TAG(world, TgtC);

    ecs_add_id(world, Rel, EcsDontFragment);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Foo, (Rel, *)",
        .cache_kind = cache_kind
    });
    test_assert(q != NULL);

    ecs_entity_t e1 = ecs_new_w(world, Foo);
    ecs_entity_t e2 = ecs_new_w(world, Foo);
    ecs_entity_t e3 = ecs_new_w(world, Foo);
    /* ecs_entity_t e4 = */ ecs_new_w(world, Foo);

    ecs_add_pair(world, e1, Rel, TgtA);
    ecs_add_pair(world, e2, Rel, TgtA);
    ecs_add_pair(world, e2, Rel, TgtB);
    ecs_add_pair(world, e3, Rel, TgtB);
    ecs_add_pair(world, e3, Rel, TgtC);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_pair(Rel, TgtC), ecs_field_id(&it, 1));
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    test_uint(e3, it.entities[0]);

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_pair(Rel, TgtB), ecs_field_id(&it, 1));
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    test_uint(e2, it.entities[0]);

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_pair(Rel, TgtB), ecs_field_id(&it, 1));
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    test_uint(e3, it.entities[0]);

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_pair(Rel, TgtA), ecs_field_id(&it, 1));
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    test_uint(e1, it.entities[0]);

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_pair(Rel, TgtA), ecs_field_id(&it, 1));
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    test_uint(e2, it.entities[0]);

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void DontFragment_1_var_sparse_written_second_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Rel);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);
    ECS_TAG(world, TgtC);

    ecs_add_id(world, Rel, EcsDontFragment);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Foo($x), Rel($x, *)",
        .cache_kind = cache_kind
    });
    test_assert(q != NULL);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    ecs_entity_t e1 = ecs_new_w(world, Foo);
    ecs_entity_t e2 = ecs_new_w(world, Foo);
    ecs_entity_t e3 = ecs_new_w(world, Foo);
    /* ecs_entity_t e4 = */ ecs_new_w(world, Foo);

    ecs_add_pair(world, e1, Rel, TgtA);
    ecs_add_pair(world, e2, Rel, TgtA);
    ecs_add_pair(world, e2, Rel, TgtB);
    ecs_add_pair(world, e3, Rel, TgtB);
    ecs_add_pair(world, e3, Rel, TgtC);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_pair(Rel, TgtC), ecs_field_id(&it, 1));
    test_uint(e3, ecs_field_src(&it, 0));
    test_uint(e3, ecs_field_src(&it, 1));
    test_uint(e3, ecs_iter_get_var(&it, x_var));

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_pair(Rel, TgtB), ecs_field_id(&it, 1));
    test_uint(e2, ecs_field_src(&it, 0));
    test_uint(e2, ecs_field_src(&it, 1));
    test_uint(e2, ecs_iter_get_var(&it, x_var));

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_pair(Rel, TgtB), ecs_field_id(&it, 1));
    test_uint(e3, ecs_field_src(&it, 0));
    test_uint(e3, ecs_field_src(&it, 1));
    test_uint(e3, ecs_iter_get_var(&it, x_var));

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_pair(Rel, TgtA), ecs_field_id(&it, 1));
    test_uint(e1, ecs_field_src(&it, 0));
    test_uint(e1, ecs_field_src(&it, 1));
    test_uint(e1, ecs_iter_get_var(&it, x_var));

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_pair(Rel, TgtA), ecs_field_id(&it, 1));
    test_uint(e2, ecs_field_src(&it, 0));
    test_uint(e2, ecs_field_src(&it, 1));
    test_uint(e2, ecs_iter_get_var(&it, x_var));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void DontFragment_1_fixed_sparse_first_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, RelC);
    ECS_TAG(world, Tgt);

    ecs_add_id(world, RelA, EcsDontFragment);
    ecs_add_id(world, RelB, EcsDontFragment);
    ecs_add_id(world, RelC, EcsDontFragment);

    ecs_entity_t ent = ecs_entity(world, { .name = "ent" });
    ecs_add_pair(world, ent, RelA, Tgt);
    ecs_add_pair(world, ent, RelB, Tgt);
    ecs_add_pair(world, ent, RelC, Tgt);

    ecs_query_t *q = ecs_query(world, {
        .expr = "*(ent, Tgt)",
        .cache_kind = cache_kind
    });
    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(ecs_pair(RelC, Tgt), ecs_field_id(&it, 0));
    test_uint(ent, ecs_field_src(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(ecs_pair(RelB, Tgt), ecs_field_id(&it, 0));
    test_uint(ent, ecs_field_src(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(ecs_pair(RelA, Tgt), ecs_field_id(&it, 0));
    test_uint(ent, ecs_field_src(&it, 0));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void DontFragment_1_fixed_sparse_first_wildcard_mixed(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, RelC);
    ECS_TAG(world, Tgt);

    ecs_add_id(world, RelA, EcsDontFragment);
    // ecs_add_id(world, RelB, EcsDontFragment);
    ecs_add_id(world, RelC, EcsDontFragment);

    ecs_entity_t ent = ecs_entity(world, { .name = "ent" });
    ecs_add_pair(world, ent, RelA, Tgt);
    ecs_add_pair(world, ent, RelB, Tgt);
    ecs_add_pair(world, ent, RelC, Tgt);

    ecs_query_t *q = ecs_query(world, {
        .expr = "*(ent, Tgt)",
        .cache_kind = cache_kind
    });
    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(ecs_pair(RelB, Tgt), ecs_field_id(&it, 0));
    test_uint(ent, ecs_field_src(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(ecs_pair(RelC, Tgt), ecs_field_id(&it, 0));
    test_uint(ent, ecs_field_src(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(ecs_pair(RelA, Tgt), ecs_field_id(&it, 0));
    test_uint(ent, ecs_field_src(&it, 0));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void DontFragment_1_this_sparse_first_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, RelC);
    ECS_TAG(world, Tgt);

    ecs_add_id(world, RelA, EcsDontFragment);
    ecs_add_id(world, RelB, EcsDontFragment);
    ecs_add_id(world, RelC, EcsDontFragment);

    ecs_query_t *q = ecs_query(world, {
        .expr = "(*, Tgt)",
        .cache_kind = cache_kind
    });
    test_assert(q != NULL);

    ecs_entity_t e1 = ecs_new_w(world, Foo);
    ecs_entity_t e2 = ecs_new_w(world, Foo);
    ecs_entity_t e3 = ecs_new_w(world, Foo);
    /* ecs_entity_t e4 = */ ecs_new_w(world, Foo);

    ecs_add_pair(world, e1, RelA, Tgt);
    ecs_add_pair(world, e2, RelA, Tgt);
    ecs_add_pair(world, e2, RelB, Tgt);
    ecs_add_pair(world, e3, RelB, Tgt);
    ecs_add_pair(world, e3, RelC, Tgt);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(ecs_pair(RelC, Tgt), ecs_field_id(&it, 0));
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(e3, it.entities[0]);

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(ecs_pair(RelB, Tgt), ecs_field_id(&it, 0));
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(e3, it.entities[0]);

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(ecs_pair(RelB, Tgt), ecs_field_id(&it, 0));
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(e2, it.entities[0]);

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(ecs_pair(RelA, Tgt), ecs_field_id(&it, 0));
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(e2, it.entities[0]);

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(ecs_pair(RelA, Tgt), ecs_field_id(&it, 0));
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(e1, it.entities[0]);

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void DontFragment_1_var_sparse_first_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, RelC);
    ECS_TAG(world, Tgt);

    ecs_add_id(world, RelA, EcsDontFragment);
    ecs_add_id(world, RelB, EcsDontFragment);
    ecs_add_id(world, RelC, EcsDontFragment);

    ecs_query_t *q = ecs_query(world, {
        .expr = "*($x, Tgt)",
        .cache_kind = cache_kind
    });
    test_assert(q != NULL);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    ecs_entity_t e1 = ecs_new_w(world, Foo);
    ecs_entity_t e2 = ecs_new_w(world, Foo);
    ecs_entity_t e3 = ecs_new_w(world, Foo);
    /* ecs_entity_t e4 = */ ecs_new_w(world, Foo);

    ecs_add_pair(world, e1, RelA, Tgt);
    ecs_add_pair(world, e2, RelA, Tgt);
    ecs_add_pair(world, e2, RelB, Tgt);
    ecs_add_pair(world, e3, RelB, Tgt);
    ecs_add_pair(world, e3, RelC, Tgt);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(ecs_pair(RelC, Tgt), ecs_field_id(&it, 0));
    test_uint(e3, ecs_field_src(&it, 0));
    test_uint(e3, ecs_iter_get_var(&it, x_var));

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(ecs_pair(RelB, Tgt), ecs_field_id(&it, 0));
    test_uint(e3, ecs_field_src(&it, 0));
    test_uint(e3, ecs_iter_get_var(&it, x_var));

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(ecs_pair(RelB, Tgt), ecs_field_id(&it, 0));
    test_uint(e2, ecs_field_src(&it, 0));
    test_uint(e2, ecs_iter_get_var(&it, x_var));

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(ecs_pair(RelA, Tgt), ecs_field_id(&it, 0));
    test_uint(e2, ecs_field_src(&it, 0));
    test_uint(e2, ecs_iter_get_var(&it, x_var));

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(ecs_pair(RelA, Tgt), ecs_field_id(&it, 0));
    test_uint(e1, ecs_field_src(&it, 0));
    test_uint(e1, ecs_iter_get_var(&it, x_var));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void DontFragment_1_this_sparse_written_first_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, RelC);
    ECS_TAG(world, Tgt);

    ecs_add_id(world, RelA, EcsDontFragment);
    ecs_add_id(world, RelB, EcsDontFragment);
    ecs_add_id(world, RelC, EcsDontFragment);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Foo, (*, Tgt)",
        .cache_kind = cache_kind
    });
    test_assert(q != NULL);

    ecs_entity_t e1 = ecs_new_w(world, Foo);
    ecs_entity_t e2 = ecs_new_w(world, Foo);
    ecs_entity_t e3 = ecs_new_w(world, Foo);
    /* ecs_entity_t e4 = */ ecs_new_w(world, Foo);

    ecs_add_pair(world, e1, RelA, Tgt);
    ecs_add_pair(world, e2, RelA, Tgt);
    ecs_add_pair(world, e2, RelB, Tgt);
    ecs_add_pair(world, e3, RelB, Tgt);
    ecs_add_pair(world, e3, RelC, Tgt);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_pair(RelC, Tgt), ecs_field_id(&it, 1));
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    test_uint(e3, it.entities[0]);

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_pair(RelB, Tgt), ecs_field_id(&it, 1));
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    test_uint(e2, it.entities[0]);

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_pair(RelB, Tgt), ecs_field_id(&it, 1));
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    test_uint(e3, it.entities[0]);

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_pair(RelA, Tgt), ecs_field_id(&it, 1));
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    test_uint(e1, it.entities[0]);

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_pair(RelA, Tgt), ecs_field_id(&it, 1));
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    test_uint(e2, it.entities[0]);

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void DontFragment_1_this_sparse_first_wildcard_mixed(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, RelC);
    ECS_TAG(world, Tgt);

    ecs_add_id(world, RelA, EcsDontFragment);
    // ecs_add_id(world, RelB, EcsDontFragment);
    ecs_add_id(world, RelC, EcsDontFragment);

    ecs_query_t *q = ecs_query(world, {
        .expr = "(*, Tgt)",
        .cache_kind = cache_kind
    });
    test_assert(q != NULL);

    ecs_entity_t e1 = ecs_new_w(world, Foo);
    ecs_entity_t e2 = ecs_new_w(world, Foo);
    ecs_entity_t e3 = ecs_new_w(world, Foo);
    /* ecs_entity_t e4 = */ ecs_new_w(world, Foo);

    ecs_add_pair(world, e1, RelA, Tgt);
    ecs_add_pair(world, e2, RelA, Tgt);
    ecs_add_pair(world, e2, RelB, Tgt);
    ecs_add_pair(world, e3, RelB, Tgt);
    ecs_add_pair(world, e3, RelC, Tgt);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(2, it.count);
    test_uint(ecs_pair(RelB, Tgt), ecs_field_id(&it, 0));
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(e2, it.entities[0]);
    test_uint(e3, it.entities[1]);

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(ecs_pair(RelC, Tgt), ecs_field_id(&it, 0));
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(e3, it.entities[0]);

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(ecs_pair(RelA, Tgt), ecs_field_id(&it, 0));
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(e2, it.entities[0]);

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(ecs_pair(RelA, Tgt), ecs_field_id(&it, 0));
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(e1, it.entities[0]);

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void DontFragment_1_this_sparse_written_first_wildcard_mixed(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, RelC);
    ECS_TAG(world, Tgt);

    ecs_add_id(world, RelA, EcsDontFragment);
    // ecs_add_id(world, RelB, EcsDontFragment);
    ecs_add_id(world, RelC, EcsDontFragment);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Foo, (*, Tgt)",
        .cache_kind = cache_kind
    });
    test_assert(q != NULL);

    ecs_entity_t e1 = ecs_new_w(world, Foo);
    ecs_entity_t e2 = ecs_new_w(world, Foo);
    ecs_entity_t e3 = ecs_new_w(world, Foo);
    /* ecs_entity_t e4 = */ ecs_new_w(world, Foo);

    ecs_add_pair(world, e1, RelA, Tgt);
    ecs_add_pair(world, e2, RelA, Tgt);
    ecs_add_pair(world, e2, RelB, Tgt);
    ecs_add_pair(world, e3, RelB, Tgt);
    ecs_add_pair(world, e3, RelC, Tgt);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_pair(RelA, Tgt), ecs_field_id(&it, 1));
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    test_uint(e1, it.entities[0]);

    test_bool(true, ecs_query_next(&it));
    test_int(2, it.count);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_pair(RelB, Tgt), ecs_field_id(&it, 1));
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    test_uint(e2, it.entities[0]);
    test_uint(e3, it.entities[1]);

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_pair(RelC, Tgt), ecs_field_id(&it, 1));
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    test_uint(e3, it.entities[0]);

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_pair(RelA, Tgt), ecs_field_id(&it, 1));
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    test_uint(e2, it.entities[0]);

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void DontFragment_1_this_sparse_first_wildcard_simple(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, RelC);
    ECS_TAG(world, Tgt);

    ecs_add_id(world, RelA, EcsDontFragment);
    ecs_add_id(world, RelB, EcsDontFragment);
    ecs_add_id(world, RelC, EcsDontFragment);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ .id = Foo}, { .id = ecs_pair(EcsWildcard, Tgt) }},
        .cache_kind = cache_kind
    });
    test_assert(q != NULL);

    ecs_entity_t e1 = ecs_new_w(world, Foo);
    ecs_entity_t e2 = ecs_new_w(world, Foo);
    ecs_entity_t e3 = ecs_new_w(world, Foo);
    /* ecs_entity_t e4 = */ ecs_new_w(world, Foo);

    ecs_add_pair(world, e1, RelA, Tgt);
    ecs_add_pair(world, e2, RelA, Tgt);
    ecs_add_pair(world, e2, RelB, Tgt);
    ecs_add_pair(world, e3, RelB, Tgt);
    ecs_add_pair(world, e3, RelC, Tgt);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_pair(RelC, Tgt), ecs_field_id(&it, 1));
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    test_uint(e3, it.entities[0]);

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_pair(RelB, Tgt), ecs_field_id(&it, 1));
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    test_uint(e2, it.entities[0]);

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_pair(RelB, Tgt), ecs_field_id(&it, 1));
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    test_uint(e3, it.entities[0]);

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_pair(RelA, Tgt), ecs_field_id(&it, 1));
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    test_uint(e1, it.entities[0]);

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_pair(RelA, Tgt), ecs_field_id(&it, 1));
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    test_uint(e2, it.entities[0]);

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void DontFragment_1_var_sparse_first_wildcard_mixed(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, RelC);
    ECS_TAG(world, Tgt);

    ecs_add_id(world, RelA, EcsDontFragment);
    // ecs_add_id(world, RelB, EcsDontFragment);
    ecs_add_id(world, RelC, EcsDontFragment);

    ecs_query_t *q = ecs_query(world, {
        .expr = "*($x, Tgt)",
        .cache_kind = cache_kind
    });
    test_assert(q != NULL);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    ecs_entity_t e1 = ecs_new_w(world, Foo);
    ecs_entity_t e2 = ecs_new_w(world, Foo);
    ecs_entity_t e3 = ecs_new_w(world, Foo);
    /* ecs_entity_t e4 = */ ecs_new_w(world, Foo);

    ecs_add_pair(world, e1, RelA, Tgt);
    ecs_add_pair(world, e2, RelA, Tgt);
    ecs_add_pair(world, e2, RelB, Tgt);
    ecs_add_pair(world, e3, RelB, Tgt);
    ecs_add_pair(world, e3, RelC, Tgt);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(ecs_pair(RelB, Tgt), ecs_field_id(&it, 0));
    test_uint(e2, ecs_field_src(&it, 0));
    test_uint(e2, ecs_iter_get_var(&it, x_var));

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(ecs_pair(RelB, Tgt), ecs_field_id(&it, 0));
    test_uint(e3, ecs_field_src(&it, 0));
    test_uint(e3, ecs_iter_get_var(&it, x_var));

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(ecs_pair(RelC, Tgt), ecs_field_id(&it, 0));
    test_uint(e3, ecs_field_src(&it, 0));
    test_uint(e3, ecs_iter_get_var(&it, x_var));

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(ecs_pair(RelA, Tgt), ecs_field_id(&it, 0));
    test_uint(e2, ecs_field_src(&it, 0));
    test_uint(e2, ecs_iter_get_var(&it, x_var));

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(ecs_pair(RelA, Tgt), ecs_field_id(&it, 0));
    test_uint(e1, ecs_field_src(&it, 0));
    test_uint(e1, ecs_iter_get_var(&it, x_var));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void DontFragment_1_var_sparse_written_first_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, RelC);
    ECS_TAG(world, Tgt);

    ecs_add_id(world, RelA, EcsDontFragment);
    ecs_add_id(world, RelB, EcsDontFragment);
    ecs_add_id(world, RelC, EcsDontFragment);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Foo($x), *($x, Tgt)",
        .cache_kind = cache_kind
    });
    test_assert(q != NULL);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    ecs_entity_t e1 = ecs_new_w(world, Foo);
    ecs_entity_t e2 = ecs_new_w(world, Foo);
    ecs_entity_t e3 = ecs_new_w(world, Foo);
    /* ecs_entity_t e4 = */ ecs_new_w(world, Foo);

    ecs_add_pair(world, e1, RelA, Tgt);
    ecs_add_pair(world, e2, RelA, Tgt);
    ecs_add_pair(world, e2, RelB, Tgt);
    ecs_add_pair(world, e3, RelB, Tgt);
    ecs_add_pair(world, e3, RelC, Tgt);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_pair(RelC, Tgt), ecs_field_id(&it, 1));
    test_uint(e3, ecs_field_src(&it, 0));
    test_uint(e3, ecs_field_src(&it, 1));
    test_uint(e3, ecs_iter_get_var(&it, x_var));

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_pair(RelB, Tgt), ecs_field_id(&it, 1));
    test_uint(e2, ecs_field_src(&it, 0));
    test_uint(e2, ecs_field_src(&it, 1));
    test_uint(e2, ecs_iter_get_var(&it, x_var));

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_pair(RelB, Tgt), ecs_field_id(&it, 1));
    test_uint(e3, ecs_field_src(&it, 0));
    test_uint(e3, ecs_field_src(&it, 1));
    test_uint(e3, ecs_iter_get_var(&it, x_var));

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_pair(RelA, Tgt), ecs_field_id(&it, 1));
    test_uint(e1, ecs_field_src(&it, 0));
    test_uint(e1, ecs_field_src(&it, 1));
    test_uint(e1, ecs_iter_get_var(&it, x_var));

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_pair(RelA, Tgt), ecs_field_id(&it, 1));
    test_uint(e2, ecs_field_src(&it, 0));
    test_uint(e2, ecs_field_src(&it, 1));
    test_uint(e2, ecs_iter_get_var(&it, x_var));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void DontFragment_1_fixed_exclusive(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);
    ECS_TAG(world, TgtC);

    ecs_add_id(world, Rel, EcsDontFragment);
    ecs_add_id(world, Rel, EcsExclusive);

    ecs_entity_t ent = ecs_entity(world, { .name = "ent" });
    ecs_add_pair(world, ent, Rel, TgtA);

    ecs_new_w_pair(world, Rel, TgtA);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Rel(ent, TgtA)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(ecs_pair(Rel, TgtA), ecs_field_id(&it, 0));
    test_uint(ent, ecs_field_src(&it, 0));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void DontFragment_1_fixed_exclusive_second_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);
    ECS_TAG(world, TgtC);

    ecs_add_id(world, Rel, EcsDontFragment);
    ecs_add_id(world, Rel, EcsExclusive);

    ecs_entity_t ent = ecs_entity(world, { .name = "ent" });
    ecs_add_pair(world, ent, Rel, TgtA);

    ecs_new_w_pair(world, Rel, TgtA);
    ecs_new_w_pair(world, Rel, TgtB);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Rel(ent, *)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(ecs_pair(Rel, TgtA), ecs_field_id(&it, 0));
    test_uint(ent, ecs_field_src(&it, 0));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void DontFragment_1_this_exclusive(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);
    ECS_TAG(world, TgtC);

    ecs_add_id(world, Rel, EcsDontFragment);
    ecs_add_id(world, Rel, EcsExclusive);

    ecs_entity_t e1 = ecs_new_w_pair(world, Rel, TgtA);
    ecs_entity_t e2 = ecs_new_w_pair(world, Rel, TgtA);
    /* ecs_entity_t e3 = */ ecs_new_w_pair(world, Rel, TgtB);
    /* ecs_entity_t e4 = */ ecs_new_w_pair(world, Rel, TgtC);

    ecs_query_t *q = ecs_query(world, {
        .expr = "(Rel, TgtA)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(ecs_pair(Rel, TgtA), ecs_field_id(&it, 0));
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(e2, it.entities[0]);

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(ecs_pair(Rel, TgtA), ecs_field_id(&it, 0));
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(e1, it.entities[0]);

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void DontFragment_1_this_exclusive_not_in_use(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Movement);
    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);

    ecs_add_id(world, Movement, EcsDontFragment);
    ecs_add_id(world, Movement, EcsExclusive);

    ecs_query_t *q = ecs_query(world, {
        .expr = "(Movement, Walking)",
        .cache_kind = cache_kind
    });
    test_assert(q != NULL);

    ecs_entity_t e1 = ecs_new_w_pair(world, Movement, Walking);
    ecs_entity_t e2 = ecs_new_w_pair(world, Movement, Walking);
    ecs_entity_t e3 = ecs_new_w_pair(world, Movement, Walking);

    test_assert(ecs_has_pair(world, e1, Movement, Walking));
    test_assert(ecs_has_pair(world, e2, Movement, Walking));
    test_assert(ecs_has_pair(world, e3, Movement, Walking));

    ecs_iter_t it = ecs_query_iter(world, q);

    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e3);

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

void DontFragment_1_this_exclusive_second_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);
    ECS_TAG(world, TgtC);

    ecs_add_id(world, Rel, EcsDontFragment);
    ecs_add_id(world, Rel, EcsExclusive);

    ecs_entity_t e1 = ecs_new_w_pair(world, Rel, TgtA);
    ecs_entity_t e2 = ecs_new_w_pair(world, Rel, TgtA);
    ecs_entity_t e3 = ecs_new_w_pair(world, Rel, TgtB);
    ecs_entity_t e4 = ecs_new_w_pair(world, Rel, TgtC);

    ecs_query_t *q = ecs_query(world, {
        .expr = "(Rel, *)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(ecs_pair(Rel, TgtC), ecs_field_id(&it, 0));
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(e4, it.entities[0]);

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(ecs_pair(Rel, TgtB), ecs_field_id(&it, 0));
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(e3, it.entities[0]);

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(ecs_pair(Rel, TgtA), ecs_field_id(&it, 0));
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(e2, it.entities[0]);

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(ecs_pair(Rel, TgtA), ecs_field_id(&it, 0));
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(e1, it.entities[0]);

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void DontFragment_1_var_exclusive(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);
    ECS_TAG(world, TgtC);

    ecs_add_id(world, Rel, EcsDontFragment);
    ecs_add_id(world, Rel, EcsExclusive);

    ecs_entity_t e1 = ecs_new_w_pair(world, Rel, TgtA);
    ecs_entity_t e2 = ecs_new_w_pair(world, Rel, TgtA);
    /* ecs_entity_t e3 = */ ecs_new_w_pair(world, Rel, TgtB);
    /* ecs_entity_t e4 = */ ecs_new_w_pair(world, Rel, TgtC);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Rel($x, TgtA)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(ecs_pair(Rel, TgtA), ecs_field_id(&it, 0));
    test_uint(e2, ecs_field_src(&it, 0));
    test_uint(e2, ecs_iter_get_var(&it, x_var));

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(ecs_pair(Rel, TgtA), ecs_field_id(&it, 0));
    test_uint(e1, ecs_field_src(&it, 0));
    test_uint(e1, ecs_iter_get_var(&it, x_var));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void DontFragment_1_var_exclusive_second_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);
    ECS_TAG(world, TgtC);

    ecs_add_id(world, Rel, EcsDontFragment);
    ecs_add_id(world, Rel, EcsExclusive);

    ecs_entity_t e1 = ecs_new_w_pair(world, Rel, TgtA);
    ecs_entity_t e2 = ecs_new_w_pair(world, Rel, TgtA);
    ecs_entity_t e3 = ecs_new_w_pair(world, Rel, TgtB);
    ecs_entity_t e4 = ecs_new_w_pair(world, Rel, TgtC);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Rel($x, *)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(ecs_pair(Rel, TgtC), ecs_field_id(&it, 0));
    test_uint(e4, ecs_field_src(&it, 0));
    test_uint(e4, ecs_iter_get_var(&it, x_var));

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(ecs_pair(Rel, TgtB), ecs_field_id(&it, 0));
    test_uint(e3, ecs_field_src(&it, 0));
    test_uint(e3, ecs_iter_get_var(&it, x_var));

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(ecs_pair(Rel, TgtA), ecs_field_id(&it, 0));
    test_uint(e2, ecs_field_src(&it, 0));
    test_uint(e2, ecs_iter_get_var(&it, x_var));

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(ecs_pair(Rel, TgtA), ecs_field_id(&it, 0));
    test_uint(e1, ecs_field_src(&it, 0));
    test_uint(e1, ecs_iter_get_var(&it, x_var));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void DontFragment_1_this_written_exclusive(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Rel);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);
    ECS_TAG(world, TgtC);

    ecs_add_id(world, Rel, EcsDontFragment);
    ecs_add_id(world, Rel, EcsExclusive);

    ecs_entity_t e1 = ecs_new_w_pair(world, Rel, TgtA);
    ecs_entity_t e2 = ecs_new_w_pair(world, Rel, TgtA);
    ecs_entity_t e3 = ecs_new_w_pair(world, Rel, TgtB);
    ecs_entity_t e4 = ecs_new_w_pair(world, Rel, TgtC);

    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);
    ecs_add(world, e3, Foo);
    ecs_add(world, e4, Foo);

    ecs_new_w(world, Foo);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Foo, (Rel, TgtA)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_pair(Rel, TgtA), ecs_field_id(&it, 1));
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    test_uint(e1, it.entities[0]);

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_pair(Rel, TgtA), ecs_field_id(&it, 1));
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    test_uint(e2, it.entities[0]);

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void DontFragment_1_this_written_exclusive_second_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Rel);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);
    ECS_TAG(world, TgtC);

    ecs_add_id(world, Rel, EcsDontFragment);
    ecs_add_id(world, Rel, EcsExclusive);

    ecs_entity_t e1 = ecs_new_w_pair(world, Rel, TgtA);
    ecs_entity_t e2 = ecs_new_w_pair(world, Rel, TgtA);
    ecs_entity_t e3 = ecs_new_w_pair(world, Rel, TgtB);
    ecs_entity_t e4 = ecs_new_w_pair(world, Rel, TgtC);

    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);
    ecs_add(world, e3, Foo);
    ecs_add(world, e4, Foo);

    ecs_new_w(world, Foo);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Foo, (Rel, *)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_pair(Rel, TgtA), ecs_field_id(&it, 1));
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    test_uint(e1, it.entities[0]);

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_pair(Rel, TgtA), ecs_field_id(&it, 1));
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    test_uint(e2, it.entities[0]);

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_pair(Rel, TgtB), ecs_field_id(&it, 1));
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    test_uint(e3, it.entities[0]);

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_pair(Rel, TgtC), ecs_field_id(&it, 1));
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    test_uint(e4, it.entities[0]);

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void DontFragment_1_var_written_exclusive(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Rel);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);
    ECS_TAG(world, TgtC);

    ecs_add_id(world, Rel, EcsDontFragment);
    ecs_add_id(world, Rel, EcsExclusive);

    ecs_entity_t e1 = ecs_new_w_pair(world, Rel, TgtA);
    ecs_entity_t e2 = ecs_new_w_pair(world, Rel, TgtA);
    ecs_entity_t e3 = ecs_new_w_pair(world, Rel, TgtB);
    ecs_entity_t e4 = ecs_new_w_pair(world, Rel, TgtC);

    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);
    ecs_add(world, e3, Foo);
    ecs_add(world, e4, Foo);

    ecs_new_w(world, Foo);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Foo($x), Rel($x, TgtA)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_pair(Rel, TgtA), ecs_field_id(&it, 1));
    test_uint(e1, ecs_field_src(&it, 0));
    test_uint(e1, ecs_field_src(&it, 1));
    test_uint(e1, ecs_iter_get_var(&it, x_var));

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_pair(Rel, TgtA), ecs_field_id(&it, 1));
    test_uint(e2, ecs_field_src(&it, 0));
    test_uint(e2, ecs_field_src(&it, 1));
    test_uint(e2, ecs_iter_get_var(&it, x_var));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void DontFragment_1_var_written_exclusive_second_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Rel);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);
    ECS_TAG(world, TgtC);

    ecs_add_id(world, Rel, EcsDontFragment);
    ecs_add_id(world, Rel, EcsExclusive);

    ecs_entity_t e1 = ecs_new_w_pair(world, Rel, TgtA);
    ecs_entity_t e2 = ecs_new_w_pair(world, Rel, TgtA);
    ecs_entity_t e3 = ecs_new_w_pair(world, Rel, TgtB);
    ecs_entity_t e4 = ecs_new_w_pair(world, Rel, TgtC);

    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);
    ecs_add(world, e3, Foo);
    ecs_add(world, e4, Foo);

    ecs_new_w(world, Foo);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Foo($x), Rel($x, *)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_pair(Rel, TgtA), ecs_field_id(&it, 1));
    test_uint(e1, ecs_field_src(&it, 0));
    test_uint(e1, ecs_field_src(&it, 1));
    test_uint(e1, ecs_iter_get_var(&it, x_var));

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_pair(Rel, TgtA), ecs_field_id(&it, 1));
    test_uint(e2, ecs_field_src(&it, 0));
    test_uint(e2, ecs_field_src(&it, 1));
    test_uint(e2, ecs_iter_get_var(&it, x_var));

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_pair(Rel, TgtB), ecs_field_id(&it, 1));
    test_uint(e3, ecs_field_src(&it, 0));
    test_uint(e3, ecs_field_src(&it, 1));
    test_uint(e3, ecs_iter_get_var(&it, x_var));

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_pair(Rel, TgtC), ecs_field_id(&it, 1));
    test_uint(e4, ecs_field_src(&it, 0));
    test_uint(e4, ecs_field_src(&it, 1));
    test_uint(e4, ecs_iter_get_var(&it, x_var));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void DontFragment_this_wildcard_wildcard_pair(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelX);
    ECS_TAG(world, RelY);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_add_id(world, RelX, EcsDontFragment);
    ecs_add_id(world, RelY, EcsDontFragment);

    ecs_entity_t e1 = ecs_new_w_pair(world, RelX, TgtA);
    ecs_add_pair(world, e1, RelY, TgtB);
    ecs_entity_t e2 = ecs_new_w_pair(world, RelY, TgtB);
    ecs_entity_t e3 = ecs_new_w_pair(world, RelY, TgtA);
    ecs_entity_t e4 = ecs_new_w(world, RelX);

    ecs_query_t *q = ecs_query(world, {
        .expr = "(*, *)",
        .cache_kind = cache_kind
    });

    ecs_iter_t it = ecs_query_iter(world, q);
    int32_t e1_found = false, e2_found = false, e3_found = false;
    while (ecs_query_next(&it)) {
        for (int i = 0; i < it.count; i ++) {
            if (it.entities[i] == e1) {
                test_int(1, it.count);
                test_uint(0, ecs_field_src(&it, 0));
                e1_found ++;
            }

            if (it.entities[i] == e2) {
                test_assert(!e2_found);
                test_int(1, it.count);
                test_uint(0, ecs_field_src(&it, 0));
                test_uint(ecs_pair(RelY, TgtB), ecs_field_id(&it, 0));
                e2_found = true;
            }

            if (it.entities[i] == e3) {
                test_assert(!e3_found);
                test_int(1, it.count);
                test_uint(0, ecs_field_src(&it, 0));
                test_uint(ecs_pair(RelY, TgtA), ecs_field_id(&it, 0));
                e3_found = true;
            }

            test_assert(it.entities[i] != e4);
        }
    }

    test_int(e1_found, 2);
    test_bool(e2_found, true);
    test_bool(e3_found, true);

    ecs_query_fini(q);

    ecs_fini(world);
}

void DontFragment_this_rel_var_w_component(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelX);
    ECS_TAG(world, RelY);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);
    ECS_TAG(world, Foo);

    ecs_add_id(world, RelX, EcsDontFragment);
    ecs_add_id(world, RelY, EcsDontFragment);
    
    ecs_add(world, RelX, Foo);
    ecs_add(world, TgtA, Foo);
    ecs_add(world, TgtB, Foo);

    ecs_entity_t e1 = ecs_new_w_pair(world, RelX, TgtA);
    /* ecs_entity_t e2 = */ ecs_new_w_pair(world, RelX, TgtB);
    /* ecs_entity_t e3 = */ ecs_new_w_pair(world, RelY, TgtB);
    /* ecs_entity_t e4 = */ ecs_new_w_pair(world, RelY, TgtA);

    ecs_query_t *q = ecs_query(world, {
        .expr = "$r($this, TgtA), Foo($r)",
        .cache_kind = cache_kind
    });

    int r_var = ecs_query_find_var(q, "r");
    test_assert(r_var != -1);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(RelX, ecs_field_src(&it, 1));
    test_uint(ecs_pair(RelX, TgtA), ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));
    test_uint(RelX, ecs_iter_get_var(&it, r_var));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void DontFragment_this_tgt_var_w_component(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelX);
    ECS_TAG(world, RelY);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);
    ECS_TAG(world, Foo);

    ecs_add_id(world, RelX, EcsDontFragment);
    ecs_add_id(world, RelY, EcsDontFragment);
    
    ecs_add(world, RelX, Foo);
    ecs_add(world, RelY, Foo);
    ecs_add(world, TgtA, Foo);

    ecs_entity_t e1 = ecs_new_w_pair(world, RelX, TgtA);
    /* ecs_entity_t e2 = */ ecs_new_w_pair(world, RelY, TgtA);
    /* ecs_entity_t e3 = */ ecs_new_w_pair(world, RelX, TgtB);
    /* ecs_entity_t e4 = */ ecs_new_w_pair(world, RelY, TgtB);

    ecs_query_t *q = ecs_query(world, {
        .expr = "RelX($this, $t), Foo($t)",
        .cache_kind = cache_kind
    });

    int t_var = ecs_query_find_var(q, "t");
    test_assert(t_var != -1);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(TgtA, ecs_field_src(&it, 1));
    test_uint(ecs_pair(RelX, TgtA), ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));
    test_uint(TgtA, ecs_iter_get_var(&it, t_var));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void DontFragment_this_wc_rel_var_w_component(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelX);
    ECS_TAG(world, RelY);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);
    ECS_TAG(world, Foo);

    ecs_add_id(world, RelX, EcsDontFragment);
    ecs_add_id(world, RelY, EcsDontFragment);
    
    ecs_add(world, RelX, Foo);
    ecs_add(world, TgtA, Foo);
    ecs_add(world, TgtB, Foo);

    ecs_entity_t e1 = ecs_new_w_pair(world, RelX, TgtA);
    ecs_entity_t e2 = ecs_new_w_pair(world, RelX, TgtB);
    /* ecs_entity_t e3 = */ ecs_new_w_pair(world, RelY, TgtB);
    /* ecs_entity_t e4 = */ ecs_new_w_pair(world, RelY, TgtA);

    ecs_query_t *q = ecs_query(world, {
        .expr = "$r($this, *), Foo($r)",
        .cache_kind = cache_kind
    });

    int r_var = ecs_query_find_var(q, "r");
    test_assert(r_var != -1);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e2, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(RelX, ecs_field_src(&it, 1));
    test_uint(ecs_pair(RelX, TgtB), ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));
    test_uint(RelX, ecs_iter_get_var(&it, r_var));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(RelX, ecs_field_src(&it, 1));
    test_uint(ecs_pair(RelX, TgtA), ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));
    test_uint(RelX, ecs_iter_get_var(&it, r_var));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void DontFragment_this_wc_tgt_var_w_component(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelX);
    ECS_TAG(world, RelY);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);
    ECS_TAG(world, Foo);

    ecs_add_id(world, RelX, EcsDontFragment);
    ecs_add_id(world, RelY, EcsDontFragment);
    
    ecs_add(world, RelX, Foo);
    ecs_add(world, RelY, Foo);
    ecs_add(world, TgtA, Foo);

    ecs_entity_t e1 = ecs_new_w_pair(world, RelX, TgtA);
    ecs_entity_t e2 = ecs_new_w_pair(world, RelY, TgtA);
    /* ecs_entity_t e3 = */ ecs_new_w_pair(world, RelX, TgtB);
    /* ecs_entity_t e4 = */ ecs_new_w_pair(world, RelY, TgtB);

    ecs_query_t *q = ecs_query(world, {
        .expr = "*($this, $t), Foo($t)",
        .cache_kind = cache_kind
    });

    int t_var = ecs_query_find_var(q, "t");
    test_assert(t_var != -1);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e2, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(TgtA, ecs_field_src(&it, 1));
    test_uint(ecs_pair(RelY, TgtA), ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));
    test_uint(TgtA, ecs_iter_get_var(&it, t_var));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(TgtA, ecs_field_src(&it, 1));
    test_uint(ecs_pair(RelX, TgtA), ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));
    test_uint(TgtA, ecs_iter_get_var(&it, t_var));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void DontFragment_this_written_wildcard_wildcard_pair(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelX);
    ECS_TAG(world, RelY);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);
    ECS_TAG(world, Tag);

    ecs_add_id(world, RelX, EcsDontFragment);
    ecs_add_id(world, RelY, EcsDontFragment);

    ecs_entity_t e1 = ecs_new_w_pair(world, RelX, TgtA);
    ecs_add_pair(world, e1, RelY, TgtA);
    ecs_entity_t e2 = ecs_new_w_pair(world, RelY, TgtB);
    ecs_entity_t e3 = ecs_new_w_pair(world, RelY, TgtA);
    ecs_entity_t e4 = ecs_new_w(world, RelX);

    ecs_add(world, e1, Tag);
    ecs_add(world, e2, Tag);
    ecs_add(world, e3, Tag);
    ecs_add(world, e4, Tag);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Tag, (*, *)",
        .cache_kind = cache_kind
    });

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e2, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_pair(RelY, TgtB), ecs_field_id(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_pair(RelY, TgtA), ecs_field_id(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e3, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_pair(RelY, TgtA), ecs_field_id(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_pair(RelX, TgtA), ecs_field_id(&it, 1));

    test_bool(false, ecs_query_next(&it)); 

    ecs_query_fini(q);

    ecs_fini(world);
}

void DontFragment_this_written_component_wildcard_wildcard_pair(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelX);
    ECS_TAG(world, RelY);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);
    
    ECS_COMPONENT(world, Position);

    ecs_add_id(world, RelX, EcsDontFragment);
    ecs_add_id(world, RelY, EcsDontFragment);

    ecs_entity_t e1 = ecs_new_w_pair(world, RelX, TgtA);
    ecs_add_pair(world, e1, RelY, TgtA);
    ecs_entity_t e2 = ecs_new_w_pair(world, RelY, TgtB);
    ecs_entity_t e3 = ecs_new_w_pair(world, RelY, TgtA);
    ecs_entity_t e4 = ecs_new_w(world, RelX);

    ecs_set(world, e1, Position, {10, 20});
    ecs_set(world, e2, Position, {20, 30});
    ecs_set(world, e3, Position, {30, 40});
    ecs_set(world, e4, Position, {40, 50});

    ecs_query_t *q = ecs_query(world, {
        .expr = "Position, (*, *)",
        .cache_kind = cache_kind
    });

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e2, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(ecs_pair(RelY, TgtB), ecs_field_id(&it, 1));
    {
        const Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 20);
        test_int(p->y, 30);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(ecs_pair(RelY, TgtA), ecs_field_id(&it, 1));
    {
        const Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e3, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(ecs_pair(RelY, TgtA), ecs_field_id(&it, 1));
    {
        const Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 30);
        test_int(p->y, 40);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(ecs_pair(RelX, TgtA), ecs_field_id(&it, 1));
    {
        const Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 20);
    }

    test_bool(false, ecs_query_next(&it)); 

    ecs_query_fini(q);

    ecs_fini(world);
}

void DontFragment_this_written_rel_var_w_component(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelX);
    ECS_TAG(world, RelY);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);
    ECS_TAG(world, Tag);
    ECS_TAG(world, Foo);

    ecs_add_id(world, RelX, EcsDontFragment);
    ecs_add_id(world, RelY, EcsDontFragment);

    ecs_add(world, RelX, Foo);

    ecs_entity_t e1 = ecs_new_w_pair(world, RelX, TgtA);
    ecs_add_pair(world, e1, RelY, TgtA);
    ecs_entity_t e2 = ecs_new_w_pair(world, RelX, TgtB);
    ecs_entity_t e3 = ecs_new_w_pair(world, RelY, TgtA);
    ecs_entity_t e4 = ecs_new_w(world, RelX);

    ecs_add(world, e1, Tag);
    ecs_add(world, e2, Tag);
    ecs_add(world, e3, Tag);
    ecs_add(world, e4, Tag);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Tag, ($r, TgtA), Foo($r)",
        .cache_kind = cache_kind
    });

    int r_var = ecs_query_find_var(q, "r");
    test_assert(r_var != -1);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_pair(RelX, TgtA), ecs_field_id(&it, 1));
    test_uint(RelX, ecs_iter_get_var(&it, r_var));

    test_bool(false, ecs_query_next(&it)); 

    ecs_query_fini(q);

    ecs_fini(world);
}

void DontFragment_this_written_tgt_var_w_component(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelX);
    ECS_TAG(world, RelY);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);
    ECS_TAG(world, Tag);
    ECS_TAG(world, Foo);

    ecs_add_id(world, RelX, EcsDontFragment);
    ecs_add_id(world, RelY, EcsDontFragment);

    ecs_entity_t e1 = ecs_new_w_pair(world, RelX, TgtA);
    ecs_add_pair(world, e1, RelY, TgtA);
    ecs_entity_t e2 = ecs_new_w_pair(world, RelY, TgtB);
    ecs_entity_t e3 = ecs_new_w_pair(world, RelY, TgtA);
    ecs_entity_t e4 = ecs_new_w(world, RelX);

    ecs_add(world, e1, Tag);
    ecs_add(world, e2, Tag);
    ecs_add(world, e3, Tag);
    ecs_add(world, e4, Tag);

    ecs_add(world, TgtA, Foo);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Tag, (RelY, $t), Foo($t)",
        .cache_kind = cache_kind
    });

    int t_var = ecs_query_find_var(q, "t");
    test_assert(t_var != -1);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_pair(RelY, TgtA), ecs_field_id(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e3, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_pair(RelY, TgtA), ecs_field_id(&it, 1));

    test_bool(false, ecs_query_next(&it)); 

    ecs_query_fini(q);

    ecs_fini(world);
}

void DontFragment_this_written_wc_rel_var_w_component(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelX);
    ECS_TAG(world, RelY);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);
    ECS_TAG(world, Tag);
    ECS_TAG(world, Foo);

    ecs_add_id(world, RelX, EcsDontFragment);
    ecs_add_id(world, RelY, EcsDontFragment);

    ecs_add(world, RelX, Foo);

    ecs_entity_t e1 = ecs_new_w_pair(world, RelX, TgtA);
    ecs_add_pair(world, e1, RelY, TgtA);
    ecs_entity_t e2 = ecs_new_w_pair(world, RelY, TgtB);
    ecs_entity_t e3 = ecs_new_w_pair(world, RelY, TgtA);
    ecs_entity_t e4 = ecs_new_w(world, RelX);

    ecs_add(world, e1, Tag);
    ecs_add(world, e2, Tag);
    ecs_add(world, e3, Tag);
    ecs_add(world, e4, Tag);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Tag, ($r, *), Foo($r)",
        .cache_kind = cache_kind
    });

    int r_var = ecs_query_find_var(q, "r");
    test_assert(r_var != -1);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_pair(RelX, TgtA), ecs_field_id(&it, 1));
    test_uint(RelX, ecs_iter_get_var(&it, r_var));

    test_bool(false, ecs_query_next(&it)); 

    ecs_query_fini(q);

    ecs_fini(world);
}

void DontFragment_this_written_wc_tgt_var_w_component(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelX);
    ECS_TAG(world, RelY);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);
    ECS_TAG(world, Tag);
    ECS_TAG(world, Foo);

    ecs_add_id(world, RelX, EcsDontFragment);
    ecs_add_id(world, RelY, EcsDontFragment);

    ecs_entity_t e1 = ecs_new_w_pair(world, RelX, TgtA);
    ecs_add_pair(world, e1, RelY, TgtA);
    ecs_entity_t e2 = ecs_new_w_pair(world, RelY, TgtB);
    ecs_entity_t e3 = ecs_new_w_pair(world, RelY, TgtA);
    ecs_entity_t e4 = ecs_new_w(world, RelX);

    ecs_add(world, e1, Tag);
    ecs_add(world, e2, Tag);
    ecs_add(world, e3, Tag);
    ecs_add(world, e4, Tag);

    ecs_add(world, TgtA, Foo);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Tag, (*, $t), Foo($t)",
        .cache_kind = cache_kind
    });

    int t_var = ecs_query_find_var(q, "t");
    test_assert(t_var != -1);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_pair(RelY, TgtA), ecs_field_id(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e3, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_pair(RelY, TgtA), ecs_field_id(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_pair(RelX, TgtA), ecs_field_id(&it, 1));

    test_bool(false, ecs_query_next(&it)); 

    ecs_query_fini(q);

    ecs_fini(world);
}

void DontFragment_add_to_self_while_iterate(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);
    ECS_TAG(world, Hello);

    ecs_add_id(world, Hello, EcsDontFragment);

    ecs_entity_t e1 = ecs_new_w(world, Foo);
    ecs_entity_t e2 = ecs_new_w(world, Foo);
    ecs_entity_t e3 = ecs_new_w(world, Foo);
    ecs_add(world, e3, Bar);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ Foo }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(2, it.count);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(0, ecs_field_src(&it, 0));
    ecs_add(world, e1, Hello);
    ecs_add(world, e2, Hello);
    test_assert(ecs_has(world, e1, Hello));
    test_assert(ecs_has(world, e2, Hello));
    test_uint(e1, it.entities[0]); // ensure entities are still in table
    test_uint(e2, it.entities[1]);

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(0, ecs_field_src(&it, 0));
    ecs_add(world, e3, Hello);
    test_assert(ecs_has(world, e3, Hello));
    test_uint(e3, it.entities[0]);

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void DontFragment_this_sparse_recycled(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Foo, DontFragment);

    ecs_entity_t e = ecs_new(world);
    ecs_add(world, e, Foo);
    test_assert(ecs_has(world, e, Foo));
    ecs_delete(world, e);
    test_assert(!ecs_is_alive(world, e));

    e = ecs_new(world);
    test_assert(!ecs_has(world, e, Foo));
    ecs_add(world, e, Foo);
    test_assert(ecs_has(world, e, Foo));

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ Foo }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e, it.entities[0]);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void DontFragment_this_sparse_pair_recycled(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Rel, DontFragment);

    ecs_entity_t e = ecs_new(world);
    ecs_add_pair(world, e, Rel, e);
    test_assert(ecs_has_pair(world, e, Rel, e));
    ecs_delete(world, e);
    test_assert(!ecs_is_alive(world, e));

    e = ecs_new(world);
    test_assert(!ecs_has_pair(world, e, Rel, e));
    ecs_add_pair(world, e, Rel, e);
    test_assert(ecs_has_pair(world, e, Rel, e));

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ ecs_pair(Rel, e) }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e, it.entities[0]);
    test_uint(ecs_pair(Rel, e), ecs_field_id(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void DontFragment_this_sparse_wildcard_pair_recycled(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Rel, DontFragment);

    ecs_entity_t e = ecs_new(world);
    ecs_add_pair(world, e, Rel, e);
    test_assert(ecs_has_pair(world, e, Rel, e));
    ecs_delete(world, e);
    test_assert(!ecs_is_alive(world, e));

    e = ecs_new(world);
    test_assert(!ecs_has_pair(world, e, Rel, e));
    ecs_add_pair(world, e, Rel, e);
    test_assert(ecs_has_pair(world, e, Rel, e));

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ ecs_pair(Rel, EcsWildcard) }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e, it.entities[0]);
    test_uint(ecs_pair(Rel, e), ecs_field_id(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void DontFragment_this_sparse_any_pair_recycled(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Rel, DontFragment);

    ecs_entity_t e = ecs_new(world);
    ecs_add_pair(world, e, Rel, e);
    test_assert(ecs_has_pair(world, e, Rel, e));
    ecs_delete(world, e);
    test_assert(!ecs_is_alive(world, e));

    e = ecs_new(world);
    test_assert(!ecs_has_pair(world, e, Rel, e));
    ecs_add_pair(world, e, Rel, e);
    test_assert(ecs_has_pair(world, e, Rel, e));

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ ecs_pair(Rel, EcsAny) }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e, it.entities[0]);
    test_uint(ecs_pair(Rel, EcsWildcard), ecs_field_id(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void DontFragment_this_written_sparse_recycled(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Foo, DontFragment);
    ECS_TAG(world, Bar);

    ecs_entity_t e = ecs_new_w(world, Bar);
    ecs_add(world, e, Foo);
    test_assert(ecs_has(world, e, Foo));
    ecs_delete(world, e);
    test_assert(!ecs_is_alive(world, e));

    e = ecs_new_w(world, Bar);
    test_assert(!ecs_has(world, e, Foo));
    ecs_add(world, e, Foo);
    test_assert(ecs_has(world, e, Foo));

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ Bar }, { Foo }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e, it.entities[0]);
    test_uint(Bar, ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 1));
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void DontFragment_this_written_sparse_pair_recycled(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Rel, DontFragment);
    ECS_TAG(world, Bar);

    ecs_entity_t e = ecs_new_w(world, Bar);
    ecs_add_pair(world, e, Rel, e);
    test_assert(ecs_has_pair(world, e, Rel, e));
    ecs_delete(world, e);
    test_assert(!ecs_is_alive(world, e));

    e = ecs_new_w(world, Bar);
    test_assert(!ecs_has_pair(world, e, Rel, e));
    ecs_add_pair(world, e, Rel, e);
    test_assert(ecs_has_pair(world, e, Rel, e));

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ Bar }, { ecs_pair(Rel, e) }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e, it.entities[0]);
    test_uint(Bar, ecs_field_id(&it, 0));
    test_uint(ecs_pair(Rel, e), ecs_field_id(&it, 1));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 1));
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void DontFragment_this_written_sparse_wildcard_pair_recycled(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Rel, DontFragment);
    ECS_TAG(world, Bar);

    ecs_entity_t e = ecs_new_w(world, Bar);
    ecs_add_pair(world, e, Rel, e);
    test_assert(ecs_has_pair(world, e, Rel, e));
    ecs_delete(world, e);
    test_assert(!ecs_is_alive(world, e));

    e = ecs_new_w(world, Bar);
    test_assert(!ecs_has_pair(world, e, Rel, e));
    ecs_add_pair(world, e, Rel, e);
    test_assert(ecs_has_pair(world, e, Rel, e));

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ Bar }, { ecs_pair(Rel, EcsWildcard) }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e, it.entities[0]);
    test_uint(Bar, ecs_field_id(&it, 0));
    test_uint(ecs_pair(Rel, e), ecs_field_id(&it, 1));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 1));
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void DontFragment_this_written_sparse_any_pair_recycled(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Rel, DontFragment);
    ECS_TAG(world, Bar);

    ecs_entity_t e = ecs_new_w(world, Bar);
    ecs_add_pair(world, e, Rel, e);
    test_assert(ecs_has_pair(world, e, Rel, e));
    ecs_delete(world, e);
    test_assert(!ecs_is_alive(world, e));

    e = ecs_new_w(world, Bar);
    test_assert(!ecs_has_pair(world, e, Rel, e));
    ecs_add_pair(world, e, Rel, e);
    test_assert(ecs_has_pair(world, e, Rel, e));

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ Bar }, { ecs_pair(Rel, EcsAny) }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e, it.entities[0]);
    test_uint(Bar, ecs_field_id(&it, 0));
    test_uint(ecs_pair(Rel, EcsWildcard), ecs_field_id(&it, 1));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 1));
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}
