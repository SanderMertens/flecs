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

    ecs_fini(world);
}

void DontFragment_1_fixed_sparse_wildcard(void) {
    // Implement testcase
}

void DontFragment_1_this_sparse_wildcard(void) {
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

    printf("%s\n", ecs_query_plan(q));

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

void DontFragment_1_var_sparse_wildcard(void) {
    // Implement testcase
}

void DontFragment_1_fixed_sparse_written_wildcard(void) {
    // Implement testcase
}

void DontFragment_1_this_sparse_written_wildcard(void) {
    // Implement testcase
}

void DontFragment_1_var_sparse_written_wildcard(void) {
    // Implement testcase
}
