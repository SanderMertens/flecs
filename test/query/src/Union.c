#include <query.h>

static ecs_query_cache_kind_t cache_kind = EcsQueryCacheDefault;

void Union_setup(void) {
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

void Union_1_fixed_union_any(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Movement, Union);
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

void Union_1_fixed_union_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Movement, Union);
    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);
    ECS_TAG(world, Sitting);

    ecs_entity_t e1 = ecs_entity(world, { .name = "ent" });
    ecs_add_pair(world, e1, Movement, Walking);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Movement(ent, *)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(e1, ecs_field_src(&it, 0));
    test_uint(ecs_pair(Movement, Walking), ecs_field_id(&it, 0));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Union_1_fixed_union_tgt(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Movement, Union);
    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);
    ECS_TAG(world, Sitting);

    ecs_entity_t e1 = ecs_entity(world, { .name = "ent" });
    ecs_add_pair(world, e1, Movement, Walking);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Movement(ent, Running)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_pair(world, e1, Movement, Running);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(e1, ecs_field_src(&it, 0));
        test_uint(ecs_pair(Movement, Running), ecs_field_id(&it, 0));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Union_1_fixed_union_tgt_var(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Movement, Union);
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

void Union_1_fixed_union_tgt_var_written(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Movement, Union);
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

void Union_1_this_union_any(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Movement, Union);
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
    test_int(2, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(e2, it.entities[1]);
    test_uint(ecs_pair(Movement, EcsWildcard), ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(2, it.count);
    test_uint(e3, it.entities[0]);
    test_uint(e4, it.entities[1]);
    test_uint(ecs_pair(Movement, EcsWildcard), ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(2, it.count);
    test_uint(e5, it.entities[0]);
    test_uint(e6, it.entities[1]);
    test_uint(ecs_pair(Movement, EcsWildcard), ecs_field_id(&it, 0));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Union_1_this_union_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Movement, Union);
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
        .expr = "(Movement, *)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e2, it.entities[0]);
    test_uint(ecs_pair(Movement, Walking), ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(ecs_pair(Movement, Walking), ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e6, it.entities[0]);
    test_uint(ecs_pair(Movement, Sitting), ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e5, it.entities[0]);
    test_uint(ecs_pair(Movement, Running), ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e4, it.entities[0]);
    test_uint(ecs_pair(Movement, Running), ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e3, it.entities[0]);
    test_uint(ecs_pair(Movement, Running), ecs_field_id(&it, 0));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Union_1_this_union_tgt(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Movement, Union);
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

    {
        ecs_query_t *q = ecs_query(world, {
            .expr = "(Movement, Walking)",
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e2, it.entities[0]);
        test_uint(ecs_pair(Movement, Walking), ecs_field_id(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e1, it.entities[0]);
        test_uint(ecs_pair(Movement, Walking), ecs_field_id(&it, 0));

        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    {
        ecs_query_t *q = ecs_query(world, {
            .expr = "(Movement, Running)",
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e5, it.entities[0]);
        test_uint(ecs_pair(Movement, Running), ecs_field_id(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e4, it.entities[0]);
        test_uint(ecs_pair(Movement, Running), ecs_field_id(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e3, it.entities[0]);
        test_uint(ecs_pair(Movement, Running), ecs_field_id(&it, 0));

        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    {
        ecs_query_t *q = ecs_query(world, {
            .expr = "(Movement, Sitting)",
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e6, it.entities[0]);
        test_uint(ecs_pair(Movement, Sitting), ecs_field_id(&it, 0));
        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    ecs_fini(world);
}

void Union_1_this_union_tgt_var(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Movement, Union);
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
    test_uint(e2, it.entities[0]);
    test_uint(ecs_pair(Movement, Walking), ecs_field_id(&it, 0));
    test_uint(Walking, ecs_iter_get_var(&it, x_var));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(ecs_pair(Movement, Walking), ecs_field_id(&it, 0));
    test_uint(Walking, ecs_iter_get_var(&it, x_var));

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

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Union_1_this_union_tgt_var_written(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Movement, Union);
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

void Union_1_var_union_any(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Movement, Union);
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
        .expr = "Movement($y, _)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int y_var = ecs_query_find_var(q, "y");
    test_assert(y_var != -1);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(ecs_pair(Movement, EcsWildcard), ecs_field_id(&it, 0));
    test_uint(e1, ecs_field_src(&it, 0));
    test_uint(e1, ecs_iter_get_var(&it, y_var));

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(ecs_pair(Movement, EcsWildcard), ecs_field_id(&it, 0));
    test_uint(e2, ecs_field_src(&it, 0));
    test_uint(e2, ecs_iter_get_var(&it, y_var));

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(ecs_pair(Movement, EcsWildcard), ecs_field_id(&it, 0));
    test_uint(e3, ecs_field_src(&it, 0));
    test_uint(e3, ecs_iter_get_var(&it, y_var));

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(ecs_pair(Movement, EcsWildcard), ecs_field_id(&it, 0));
    test_uint(e4, ecs_field_src(&it, 0));
    test_uint(e4, ecs_iter_get_var(&it, y_var));

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(ecs_pair(Movement, EcsWildcard), ecs_field_id(&it, 0));
    test_uint(e5, ecs_field_src(&it, 0));
    test_uint(e5, ecs_iter_get_var(&it, y_var));

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(ecs_pair(Movement, EcsWildcard), ecs_field_id(&it, 0));
    test_uint(e6, ecs_field_src(&it, 0));
    test_uint(e6, ecs_iter_get_var(&it, y_var));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Union_1_var_union_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Movement, Union);
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
        .expr = "Movement($y, *)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int y_var = ecs_query_find_var(q, "y");
    test_assert(y_var != -1);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(ecs_pair(Movement, Walking), ecs_field_id(&it, 0));
    test_uint(e2, ecs_field_src(&it, 0));
    test_uint(e2, ecs_iter_get_var(&it, y_var));

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(ecs_pair(Movement, Walking), ecs_field_id(&it, 0));
    test_uint(e1, ecs_field_src(&it, 0));
    test_uint(e1, ecs_iter_get_var(&it, y_var));

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(ecs_pair(Movement, Sitting), ecs_field_id(&it, 0));
    test_uint(e6, ecs_field_src(&it, 0));
    test_uint(e6, ecs_iter_get_var(&it, y_var));

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(ecs_pair(Movement, Running), ecs_field_id(&it, 0));
    test_uint(e5, ecs_field_src(&it, 0));
    test_uint(e5, ecs_iter_get_var(&it, y_var));

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(ecs_pair(Movement, Running), ecs_field_id(&it, 0));
    test_uint(e4, ecs_field_src(&it, 0));
    test_uint(e4, ecs_iter_get_var(&it, y_var));

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(ecs_pair(Movement, Running), ecs_field_id(&it, 0));
    test_uint(e3, ecs_field_src(&it, 0));
    test_uint(e3, ecs_iter_get_var(&it, y_var));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Union_1_var_union_tgt(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Movement, Union);
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

    {
        ecs_query_t *q = ecs_query(world, {
            .expr = "Movement($y, Walking)",
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        int y_var = ecs_query_find_var(q, "y");
        test_assert(y_var != -1);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(ecs_pair(Movement, Walking), ecs_field_id(&it, 0));
        test_uint(e2, ecs_field_src(&it, 0));
        test_uint(e2, ecs_iter_get_var(&it, y_var));

        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(ecs_pair(Movement, Walking), ecs_field_id(&it, 0));
        test_uint(e1, ecs_field_src(&it, 0));
        test_uint(e1, ecs_iter_get_var(&it, y_var));

        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    {
        ecs_query_t *q = ecs_query(world, {
            .expr = "Movement($y, Running)",
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        int y_var = ecs_query_find_var(q, "y");
        test_assert(y_var != -1);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(ecs_pair(Movement, Running), ecs_field_id(&it, 0));
        test_uint(e5, ecs_field_src(&it, 0));
        test_uint(e5, ecs_iter_get_var(&it, y_var));

        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(ecs_pair(Movement, Running), ecs_field_id(&it, 0));
        test_uint(e4, ecs_field_src(&it, 0));
        test_uint(e4, ecs_iter_get_var(&it, y_var));

        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(ecs_pair(Movement, Running), ecs_field_id(&it, 0));
        test_uint(e3, ecs_field_src(&it, 0));
        test_uint(e3, ecs_iter_get_var(&it, y_var));

        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    {
        ecs_query_t *q = ecs_query(world, {
            .expr = "Movement($y, Sitting)",
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        int y_var = ecs_query_find_var(q, "y");
        test_assert(y_var != -1);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(ecs_pair(Movement, Sitting), ecs_field_id(&it, 0));
        test_uint(e6, ecs_field_src(&it, 0));
        test_uint(e6, ecs_iter_get_var(&it, y_var));
        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    ecs_fini(world);
}

void Union_1_var_union_tgt_var(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Movement, Union);
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
        .expr = "Movement($y, $x)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    int y_var = ecs_query_find_var(q, "y");
    test_assert(y_var != -1);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(ecs_pair(Movement, Walking), ecs_field_id(&it, 0));
    test_uint(e2, ecs_field_src(&it, 0));
    test_uint(Walking, ecs_iter_get_var(&it, x_var));
    test_uint(e2, ecs_iter_get_var(&it, y_var));

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(ecs_pair(Movement, Walking), ecs_field_id(&it, 0));
    test_uint(e1, ecs_field_src(&it, 0));
    test_uint(Walking, ecs_iter_get_var(&it, x_var));
    test_uint(e1, ecs_iter_get_var(&it, y_var));

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(ecs_pair(Movement, Sitting), ecs_field_id(&it, 0));
    test_uint(e6, ecs_field_src(&it, 0));
    test_uint(Sitting, ecs_iter_get_var(&it, x_var));
    test_uint(e6, ecs_iter_get_var(&it, y_var));

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(ecs_pair(Movement, Running), ecs_field_id(&it, 0));
    test_uint(e5, ecs_field_src(&it, 0));
    test_uint(Running, ecs_iter_get_var(&it, x_var));
    test_uint(e5, ecs_iter_get_var(&it, y_var));

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(ecs_pair(Movement, Running), ecs_field_id(&it, 0));
    test_uint(e4, ecs_field_src(&it, 0));
    test_uint(Running, ecs_iter_get_var(&it, x_var));
    test_uint(e4, ecs_iter_get_var(&it, y_var));

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(ecs_pair(Movement, Running), ecs_field_id(&it, 0));
    test_uint(e3, ecs_field_src(&it, 0));
    test_uint(Running, ecs_iter_get_var(&it, x_var));
    test_uint(e3, ecs_iter_get_var(&it, y_var));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Union_1_var_union_tgt_var_written(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Movement, Union);
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
        .expr = "$x == Running, Movement($y, $x)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    int y_var = ecs_query_find_var(q, "y");
    test_assert(y_var != -1);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(ecs_pair(Movement, Running), ecs_field_id(&it, 1));
    test_uint(e5, ecs_field_src(&it, 1));
    test_uint(Running, ecs_iter_get_var(&it, x_var));
    test_uint(e5, ecs_iter_get_var(&it, y_var));

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(ecs_pair(Movement, Running), ecs_field_id(&it, 1));
    test_uint(e4, ecs_field_src(&it, 1));
    test_uint(Running, ecs_iter_get_var(&it, x_var));
    test_uint(e4, ecs_iter_get_var(&it, y_var));

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(ecs_pair(Movement, Running), ecs_field_id(&it, 1));
    test_uint(e3, ecs_field_src(&it, 1));
    test_uint(Running, ecs_iter_get_var(&it, x_var));
    test_uint(e3, ecs_iter_get_var(&it, y_var));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Union_1_this_written_union_any(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Movement, Union);
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
    test_int(2, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(e2, it.entities[1]);
    test_uint(TagA, ecs_field_id(&it, 0));
    test_uint(ecs_pair(Movement, EcsWildcard), ecs_field_id(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(2, it.count);
    test_uint(e3, it.entities[0]);
    test_uint(e4, it.entities[1]);
    test_uint(TagA, ecs_field_id(&it, 0));
    test_uint(ecs_pair(Movement, EcsWildcard), ecs_field_id(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(2, it.count);
    test_uint(e5, it.entities[0]);
    test_uint(e6, it.entities[1]);
    test_uint(TagA, ecs_field_id(&it, 0));
    test_uint(ecs_pair(Movement, EcsWildcard), ecs_field_id(&it, 1));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Union_1_this_written_union_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Movement, Union);
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

    /* ecs_entity_t e7 = */ ecs_new_w(world, TagA);

    ecs_add(world, e3, Foo);
    ecs_add(world, e4, Foo);
    ecs_add(world, e5, Bar);
    ecs_add(world, e6, Bar);

    ecs_query_t *q = ecs_query(world, {
        .expr = "TagA, (Movement, *)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(TagA, ecs_field_id(&it, 0));
    test_uint(ecs_pair(Movement, Walking), ecs_field_id(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e2, it.entities[0]);
    test_uint(TagA, ecs_field_id(&it, 0));
    test_uint(ecs_pair(Movement, Walking), ecs_field_id(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e3, it.entities[0]);
    test_uint(TagA, ecs_field_id(&it, 0));
    test_uint(ecs_pair(Movement, Running), ecs_field_id(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e4, it.entities[0]);
    test_uint(TagA, ecs_field_id(&it, 0));
    test_uint(ecs_pair(Movement, Running), ecs_field_id(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e5, it.entities[0]);
    test_uint(TagA, ecs_field_id(&it, 0));
    test_uint(ecs_pair(Movement, Running), ecs_field_id(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e6, it.entities[0]);
    test_uint(TagA, ecs_field_id(&it, 0));
    test_uint(ecs_pair(Movement, Sitting), ecs_field_id(&it, 1));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Union_1_this_written_union_tgt(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Movement, Union);
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

    /* ecs_entity_t e7 = */ ecs_new_w(world, TagA);

    ecs_add(world, e3, Foo);
    ecs_add(world, e4, Foo);
    ecs_add(world, e5, Bar);
    ecs_add(world, e6, Bar);

    {
        ecs_query_t *q = ecs_query(world, {
            .expr = "TagA, (Movement, Walking)",
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e1, it.entities[0]);
        test_uint(TagA, ecs_field_id(&it, 0));
        test_uint(ecs_pair(Movement, Walking), ecs_field_id(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e2, it.entities[0]);
        test_uint(TagA, ecs_field_id(&it, 0));
        test_uint(ecs_pair(Movement, Walking), ecs_field_id(&it, 1));

        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    {
        ecs_query_t *q = ecs_query(world, {
            .expr = "TagA, (Movement, Running)",
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e3, it.entities[0]);
        test_uint(TagA, ecs_field_id(&it, 0));
        test_uint(ecs_pair(Movement, Running), ecs_field_id(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e4, it.entities[0]);
        test_uint(TagA, ecs_field_id(&it, 0));
        test_uint(ecs_pair(Movement, Running), ecs_field_id(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e5, it.entities[0]);
        test_uint(TagA, ecs_field_id(&it, 0));
        test_uint(ecs_pair(Movement, Running), ecs_field_id(&it, 1));

        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    {
        ecs_query_t *q = ecs_query(world, {
            .expr = "TagA, (Movement, Sitting)",
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e6, it.entities[0]);
        test_uint(TagA, ecs_field_id(&it, 0));
        test_uint(ecs_pair(Movement, Sitting), ecs_field_id(&it, 1));
        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    ecs_fini(world);
}

void Union_1_this_written_union_tgt_var(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Movement, Union);
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

    /* ecs_entity_t e7 = */ ecs_new_w(world, TagA);

    ecs_add(world, e3, Foo);
    ecs_add(world, e4, Foo);
    ecs_add(world, e5, Bar);
    ecs_add(world, e6, Bar);

    ecs_query_t *q = ecs_query(world, {
        .expr = "TagA, (Movement, $x)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(TagA, ecs_field_id(&it, 0));
    test_uint(ecs_pair(Movement, Walking), ecs_field_id(&it, 1));
    test_uint(Walking, ecs_iter_get_var(&it, x_var));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e2, it.entities[0]);
    test_uint(TagA, ecs_field_id(&it, 0));
    test_uint(ecs_pair(Movement, Walking), ecs_field_id(&it, 1));
    test_uint(Walking, ecs_iter_get_var(&it, x_var));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e3, it.entities[0]);
    test_uint(TagA, ecs_field_id(&it, 0));
    test_uint(ecs_pair(Movement, Running), ecs_field_id(&it, 1));
    test_uint(Running, ecs_iter_get_var(&it, x_var));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e4, it.entities[0]);
    test_uint(TagA, ecs_field_id(&it, 0));
    test_uint(ecs_pair(Movement, Running), ecs_field_id(&it, 1));
    test_uint(Running, ecs_iter_get_var(&it, x_var));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e5, it.entities[0]);
    test_uint(TagA, ecs_field_id(&it, 0));
    test_uint(ecs_pair(Movement, Running), ecs_field_id(&it, 1));
    test_uint(Running, ecs_iter_get_var(&it, x_var));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e6, it.entities[0]);
    test_uint(TagA, ecs_field_id(&it, 0));
    test_uint(ecs_pair(Movement, Sitting), ecs_field_id(&it, 1));
    test_uint(Sitting, ecs_iter_get_var(&it, x_var));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Union_1_this_written_union_tgt_var_written(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Movement, Union);
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

    /* ecs_entity_t e7 = */ ecs_new_w(world, TagA);

    ecs_add(world, e3, Foo);
    ecs_add(world, e4, Foo);
    ecs_add(world, e5, Bar);
    ecs_add(world, e6, Bar);

    ecs_query_t *q = ecs_query(world, {
        .expr = "$x == Running, TagA, (Movement, $x)",
        // Order changes for cached query
        // .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e5, it.entities[0]);
    test_uint(TagA, ecs_field_id(&it, 1));
    test_uint(ecs_pair(Movement, Running), ecs_field_id(&it, 2));
    test_uint(Running, ecs_iter_get_var(&it, x_var));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e4, it.entities[0]);
    test_uint(TagA, ecs_field_id(&it, 1));
    test_uint(ecs_pair(Movement, Running), ecs_field_id(&it, 2));
    test_uint(Running, ecs_iter_get_var(&it, x_var));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e3, it.entities[0]);
    test_uint(TagA, ecs_field_id(&it, 1));
    test_uint(ecs_pair(Movement, Running), ecs_field_id(&it, 2));
    test_uint(Running, ecs_iter_get_var(&it, x_var));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Union_1_var_written_union_any(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Movement, Union);
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

    /* ecs_entity_t e7 = */ ecs_new_w(world, TagA);

    ecs_add(world, e3, Foo);
    ecs_add(world, e4, Foo);
    ecs_add(world, e5, Bar);
    ecs_add(world, e6, Bar);

    ecs_query_t *q = ecs_query(world, {
        .expr = "TagA($y), Movement($y, _)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int y_var = ecs_query_find_var(q, "y");
    test_assert(y_var != -1);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(TagA, ecs_field_id(&it, 0));
    test_uint(ecs_pair(Movement, EcsWildcard), ecs_field_id(&it, 1));
    test_uint(e1, ecs_field_src(&it, 0));
    test_uint(e1, ecs_field_src(&it, 1));
    test_uint(e1, ecs_iter_get_var(&it, y_var));

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(TagA, ecs_field_id(&it, 0));
    test_uint(ecs_pair(Movement, EcsWildcard), ecs_field_id(&it, 1));
    test_uint(e2, ecs_field_src(&it, 0));
    test_uint(e2, ecs_field_src(&it, 1));
    test_uint(e2, ecs_iter_get_var(&it, y_var));

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(TagA, ecs_field_id(&it, 0));
    test_uint(ecs_pair(Movement, EcsWildcard), ecs_field_id(&it, 1));
    test_uint(e3, ecs_field_src(&it, 0));
    test_uint(e3, ecs_field_src(&it, 1));
    test_uint(e3, ecs_iter_get_var(&it, y_var));

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(TagA, ecs_field_id(&it, 0));
    test_uint(ecs_pair(Movement, EcsWildcard), ecs_field_id(&it, 1));
    test_uint(e4, ecs_field_src(&it, 0));
    test_uint(e4, ecs_field_src(&it, 1));
    test_uint(e4, ecs_iter_get_var(&it, y_var));

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(TagA, ecs_field_id(&it, 0));
    test_uint(ecs_pair(Movement, EcsWildcard), ecs_field_id(&it, 1));
    test_uint(e5, ecs_field_src(&it, 0));
    test_uint(e5, ecs_field_src(&it, 1));
    test_uint(e5, ecs_iter_get_var(&it, y_var));

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(TagA, ecs_field_id(&it, 0));
    test_uint(ecs_pair(Movement, EcsWildcard), ecs_field_id(&it, 1));
    test_uint(e6, ecs_field_src(&it, 0));
    test_uint(e6, ecs_field_src(&it, 1));
    test_uint(e6, ecs_iter_get_var(&it, y_var));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Union_1_var_written_union_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Movement, Union);
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

    /* ecs_entity_t e7 = */ ecs_new_w(world, TagA);

    ecs_add(world, e3, Foo);
    ecs_add(world, e4, Foo);
    ecs_add(world, e5, Bar);
    ecs_add(world, e6, Bar);

    ecs_query_t *q = ecs_query(world, {
        .expr = "TagA($y), Movement($y, *)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int y_var = ecs_query_find_var(q, "y");
    test_assert(y_var != -1);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(TagA, ecs_field_id(&it, 0));
    test_uint(ecs_pair(Movement, Walking), ecs_field_id(&it, 1));
    test_uint(e1, ecs_field_src(&it, 0));
    test_uint(e1, ecs_field_src(&it, 1));
    test_uint(e1, ecs_iter_get_var(&it, y_var));

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(TagA, ecs_field_id(&it, 0));
    test_uint(ecs_pair(Movement, Walking), ecs_field_id(&it, 1));
    test_uint(e2, ecs_field_src(&it, 0));
    test_uint(e2, ecs_field_src(&it, 1));
    test_uint(e2, ecs_iter_get_var(&it, y_var));

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(TagA, ecs_field_id(&it, 0));
    test_uint(ecs_pair(Movement, Running), ecs_field_id(&it, 1));
    test_uint(e3, ecs_field_src(&it, 0));
    test_uint(e3, ecs_field_src(&it, 1));
    test_uint(e3, ecs_iter_get_var(&it, y_var));

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(TagA, ecs_field_id(&it, 0));
    test_uint(ecs_pair(Movement, Running), ecs_field_id(&it, 1));
    test_uint(e4, ecs_field_src(&it, 0));
    test_uint(e4, ecs_field_src(&it, 1));
    test_uint(e4, ecs_iter_get_var(&it, y_var));

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(TagA, ecs_field_id(&it, 0));
    test_uint(ecs_pair(Movement, Running), ecs_field_id(&it, 1));
    test_uint(e5, ecs_field_src(&it, 0));
    test_uint(e5, ecs_field_src(&it, 1));
    test_uint(e5, ecs_iter_get_var(&it, y_var));

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(TagA, ecs_field_id(&it, 0));
    test_uint(ecs_pair(Movement, Sitting), ecs_field_id(&it, 1));
    test_uint(e6, ecs_field_src(&it, 0));
    test_uint(e6, ecs_field_src(&it, 1));
    test_uint(e6, ecs_iter_get_var(&it, y_var));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Union_1_var_written_union_tgt(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Movement, Union);
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

    /* ecs_entity_t e7 = */ ecs_new_w(world, TagA);

    ecs_add(world, e3, Foo);
    ecs_add(world, e4, Foo);
    ecs_add(world, e5, Bar);
    ecs_add(world, e6, Bar);

    {
        ecs_query_t *q = ecs_query(world, {
            .expr = "TagA($y), Movement($y, Walking)",
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        int y_var = ecs_query_find_var(q, "y");
        test_assert(y_var != -1);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(TagA, ecs_field_id(&it, 0));
        test_uint(ecs_pair(Movement, Walking), ecs_field_id(&it, 1));
        test_uint(e1, ecs_field_src(&it, 0));
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(e1, ecs_iter_get_var(&it, y_var));

        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(TagA, ecs_field_id(&it, 0));
        test_uint(ecs_pair(Movement, Walking), ecs_field_id(&it, 1));
        test_uint(e2, ecs_field_src(&it, 0));
        test_uint(e2, ecs_field_src(&it, 1));
        test_uint(e2, ecs_iter_get_var(&it, y_var));

        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    {
        ecs_query_t *q = ecs_query(world, {
            .expr = "TagA($y), Movement($y, Running)",
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        int y_var = ecs_query_find_var(q, "y");
        test_assert(y_var != -1);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(TagA, ecs_field_id(&it, 0));
        test_uint(ecs_pair(Movement, Running), ecs_field_id(&it, 1));
        test_uint(e3, ecs_field_src(&it, 0));
        test_uint(e3, ecs_field_src(&it, 1));
        test_uint(e3, ecs_iter_get_var(&it, y_var));

        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(TagA, ecs_field_id(&it, 0));
        test_uint(ecs_pair(Movement, Running), ecs_field_id(&it, 1));
        test_uint(e4, ecs_field_src(&it, 0));
        test_uint(e4, ecs_field_src(&it, 1));
        test_uint(e4, ecs_iter_get_var(&it, y_var));

        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(TagA, ecs_field_id(&it, 0));
        test_uint(ecs_pair(Movement, Running), ecs_field_id(&it, 1));
        test_uint(e5, ecs_field_src(&it, 0));
        test_uint(e5, ecs_field_src(&it, 1));
        test_uint(e5, ecs_iter_get_var(&it, y_var));

        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    {
        ecs_query_t *q = ecs_query(world, {
            .expr = "TagA($y), Movement($y, Sitting)",
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        int y_var = ecs_query_find_var(q, "y");
        test_assert(y_var != -1);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(TagA, ecs_field_id(&it, 0));
        test_uint(ecs_pair(Movement, Sitting), ecs_field_id(&it, 1));
        test_uint(e6, ecs_field_src(&it, 0));
        test_uint(e6, ecs_field_src(&it, 1));
        test_uint(e6, ecs_iter_get_var(&it, y_var));
        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    ecs_fini(world);
}

void Union_1_var_written_union_tgt_var(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Movement, Union);
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

    /* ecs_entity_t e7 = */ ecs_new_w(world, TagA);

    ecs_add(world, e3, Foo);
    ecs_add(world, e4, Foo);
    ecs_add(world, e5, Bar);
    ecs_add(world, e6, Bar);

    ecs_query_t *q = ecs_query(world, {
        .expr = "TagA($y), Movement($y, $x)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    int y_var = ecs_query_find_var(q, "y");
    test_assert(y_var != -1);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(TagA, ecs_field_id(&it, 0));
    test_uint(ecs_pair(Movement, Walking), ecs_field_id(&it, 1));
    test_uint(Walking, ecs_iter_get_var(&it, x_var));
    test_uint(e1, ecs_field_src(&it, 0));
    test_uint(e1, ecs_field_src(&it, 1));
    test_uint(e1, ecs_iter_get_var(&it, y_var));

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(TagA, ecs_field_id(&it, 0));
    test_uint(ecs_pair(Movement, Walking), ecs_field_id(&it, 1));
    test_uint(Walking, ecs_iter_get_var(&it, x_var));
    test_uint(e2, ecs_field_src(&it, 0));
    test_uint(e2, ecs_field_src(&it, 1));
    test_uint(e2, ecs_iter_get_var(&it, y_var));

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(TagA, ecs_field_id(&it, 0));
    test_uint(ecs_pair(Movement, Running), ecs_field_id(&it, 1));
    test_uint(Running, ecs_iter_get_var(&it, x_var));
    test_uint(e3, ecs_field_src(&it, 0));
    test_uint(e3, ecs_field_src(&it, 1));
    test_uint(e3, ecs_iter_get_var(&it, y_var));

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(TagA, ecs_field_id(&it, 0));
    test_uint(ecs_pair(Movement, Running), ecs_field_id(&it, 1));
    test_uint(Running, ecs_iter_get_var(&it, x_var));
    test_uint(e4, ecs_field_src(&it, 0));
    test_uint(e4, ecs_field_src(&it, 1));
    test_uint(e4, ecs_iter_get_var(&it, y_var));

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(TagA, ecs_field_id(&it, 0));
    test_uint(ecs_pair(Movement, Running), ecs_field_id(&it, 1));
    test_uint(Running, ecs_iter_get_var(&it, x_var));
    test_uint(e5, ecs_field_src(&it, 0));
    test_uint(e5, ecs_field_src(&it, 1));
    test_uint(e5, ecs_iter_get_var(&it, y_var));

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(TagA, ecs_field_id(&it, 0));
    test_uint(ecs_pair(Movement, Sitting), ecs_field_id(&it, 1));
    test_uint(Sitting, ecs_iter_get_var(&it, x_var));
    test_uint(e6, ecs_field_src(&it, 0));
    test_uint(e6, ecs_field_src(&it, 1));
    test_uint(e6, ecs_iter_get_var(&it, y_var));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Union_1_var_written_union_tgt_var_written(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Movement, Union);
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

    /* ecs_entity_t e7 = */ ecs_new_w(world, TagA);

    ecs_add(world, e3, Foo);
    ecs_add(world, e4, Foo);
    ecs_add(world, e5, Bar);
    ecs_add(world, e6, Bar);

    ecs_query_t *q = ecs_query(world, {
        .expr = "$x == Running, TagA($y), Movement($y, $x)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    int y_var = ecs_query_find_var(q, "y");
    test_assert(y_var != -1);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(TagA, ecs_field_id(&it, 1));
    test_uint(ecs_pair(Movement, Running), ecs_field_id(&it, 2));
    test_uint(Running, ecs_iter_get_var(&it, x_var));
    test_uint(e5, ecs_field_src(&it, 1));
    test_uint(e5, ecs_field_src(&it, 2));
    test_uint(e5, ecs_iter_get_var(&it, y_var));

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(TagA, ecs_field_id(&it, 1));
    test_uint(ecs_pair(Movement, Running), ecs_field_id(&it, 2));
    test_uint(Running, ecs_iter_get_var(&it, x_var));
    test_uint(e4, ecs_field_src(&it, 1));
    test_uint(e4, ecs_field_src(&it, 2));
    test_uint(e4, ecs_iter_get_var(&it, y_var));

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(TagA, ecs_field_id(&it, 1));
    test_uint(ecs_pair(Movement, Running), ecs_field_id(&it, 2));
    test_uint(Running, ecs_iter_get_var(&it, x_var));
    test_uint(e3, ecs_field_src(&it, 1));
    test_uint(e3, ecs_field_src(&it, 2));
    test_uint(e3, ecs_iter_get_var(&it, y_var));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Union_not_fixed_union_any(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Movement, Union);
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

void Union_not_fixed_union_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Movement, Union);
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

    ecs_remove_pair(world, e1, Movement, EcsWildcard);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(e1, ecs_field_src(&it, 0));
        test_uint(ecs_pair(Movement, EcsWildcard), ecs_field_id(&it, 0));
        test_bool(false, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Union_not_fixed_union_tgt(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Movement, Union);
    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);
    ECS_TAG(world, Sitting);

    ecs_entity_t e1 = ecs_entity(world, { .name = "ent" });

    ecs_query_t *q = ecs_query(world, {
        .expr = "!Movement(ent, Running)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(e1, ecs_field_src(&it, 0));
        test_uint(ecs_pair(Movement, Running), ecs_field_id(&it, 0));
        test_bool(false, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_pair(world, e1, Movement, Walking);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(e1, ecs_field_src(&it, 0));
        test_uint(ecs_pair(Movement, Running), ecs_field_id(&it, 0));
        test_bool(false, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_pair(world, e1, Movement, Running);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_remove_pair(world, e1, Movement, EcsWildcard);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(e1, ecs_field_src(&it, 0));
        test_uint(ecs_pair(Movement, Running), ecs_field_id(&it, 0));
        test_bool(false, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Union_not_fixed_union_var(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Movement, Union);
    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);
    ECS_TAG(world, Sitting);

    ecs_entity_t e1 = ecs_entity(world, { .name = "ent" });

    ecs_query_t *q = ecs_query(world, {
        .expr = "!Movement(ent, $x)",
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
        test_uint(ecs_pair(Movement, EcsWildcard), ecs_field_id(&it, 0));
        test_uint(EcsWildcard, ecs_iter_get_var(&it, x_var));
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

void Union_not_fixed_union_var_written(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Movement, Union);
    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);
    ECS_TAG(world, Sitting);

    ecs_entity_t e1 = ecs_entity(world, { .name = "ent" });

    ecs_query_t *q = ecs_query(world, {
        .expr = "$x == Running, !Movement(ent, $x)",
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
        test_uint(ecs_pair(Movement, Running), ecs_field_id(&it, 1));
        test_uint(Running, ecs_iter_get_var(&it, x_var));
        test_bool(false, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_pair(world, e1, Movement, Walking);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(ecs_pair(Movement, Running), ecs_field_id(&it, 1));
        test_uint(Running, ecs_iter_get_var(&it, x_var));
        test_bool(false, ecs_field_is_set(&it, 1));
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

void Union_not_this_written_union_any(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Movement, Union);
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

void Union_not_this_written_union_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Movement, Union);
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

void Union_not_this_written_union_tgt(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Movement, Union);
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

    {
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

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e3, it.entities[0]);
        test_uint(TagA, ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_field_is_set(&it, 1));
        test_uint(ecs_pair(Movement, Walking), ecs_field_id(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e4, it.entities[0]);
        test_uint(TagA, ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_field_is_set(&it, 1));
        test_uint(ecs_pair(Movement, Walking), ecs_field_id(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e5, it.entities[0]);
        test_uint(TagA, ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_field_is_set(&it, 1));
        test_uint(ecs_pair(Movement, Walking), ecs_field_id(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e6, it.entities[0]);
        test_uint(TagA, ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_field_is_set(&it, 1));
        test_uint(ecs_pair(Movement, Walking), ecs_field_id(&it, 1));

        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    {
        ecs_query_t *q = ecs_query(world, {
            .expr = "TagA, !(Movement, Running)",
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e1, it.entities[0]);
        test_uint(TagA, ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_field_is_set(&it, 1));
        test_uint(ecs_pair(Movement, Running), ecs_field_id(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e2, it.entities[0]);
        test_uint(TagA, ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_field_is_set(&it, 1));
        test_uint(ecs_pair(Movement, Running), ecs_field_id(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e7, it.entities[0]);
        test_uint(TagA, ecs_field_id(&it, 0));
        test_uint(ecs_pair(Movement, Running), ecs_field_id(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e6, it.entities[0]);
        test_uint(TagA, ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_field_is_set(&it, 1));
        test_uint(ecs_pair(Movement, Running), ecs_field_id(&it, 1));

        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    {
        ecs_query_t *q = ecs_query(world, {
            .expr = "TagA, !(Movement, Sitting)",
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e1, it.entities[0]);
        test_uint(TagA, ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_field_is_set(&it, 1));
        test_uint(ecs_pair(Movement, Sitting), ecs_field_id(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e2, it.entities[0]);
        test_uint(TagA, ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_field_is_set(&it, 1));
        test_uint(ecs_pair(Movement, Sitting), ecs_field_id(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e7, it.entities[0]);
        test_uint(TagA, ecs_field_id(&it, 0));
        test_uint(ecs_pair(Movement, Sitting), ecs_field_id(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e3, it.entities[0]);
        test_uint(TagA, ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_field_is_set(&it, 1));
        test_uint(ecs_pair(Movement, Sitting), ecs_field_id(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e4, it.entities[0]);
        test_uint(TagA, ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_field_is_set(&it, 1));
        test_uint(ecs_pair(Movement, Sitting), ecs_field_id(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e5, it.entities[0]);
        test_uint(TagA, ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_field_is_set(&it, 1));
        test_uint(ecs_pair(Movement, Sitting), ecs_field_id(&it, 1));

        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    ecs_fini(world);
}

void Union_not_this_written_union_var(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Movement, Union);
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
    ecs_entity_t e8 = ecs_new_w(world, TagA);

    ecs_add(world, e3, Foo);
    ecs_add(world, e4, Foo);
    ecs_add(world, e5, Bar);
    ecs_add(world, e6, Bar);

    ecs_query_t *q = ecs_query(world, {
        .expr = "TagA, !(Movement, $x)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int x_var = ecs_query_find_var(q, "x");

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(2, it.count);
    test_uint(e7, it.entities[0]);
    test_uint(e8, it.entities[1]);
    test_uint(TagA, ecs_field_id(&it, 0));
    test_uint(ecs_pair(Movement, EcsWildcard), ecs_field_id(&it, 1));
    test_uint(EcsWildcard, ecs_iter_get_var(&it, x_var));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(false, ecs_field_is_set(&it, 1));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Union_not_this_written_union_var_written(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Movement, Union);
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
        .expr = "$x == Running, TagA, !(Movement, $x)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int x_var = ecs_query_find_var(q, "x");

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(TagA, ecs_field_id(&it, 1));
    test_uint(ecs_pair(Movement, Running), ecs_field_id(&it, 2));
    test_uint(Running, ecs_iter_get_var(&it, x_var));
    test_bool(true, ecs_field_is_set(&it, 1));
    test_bool(false, ecs_field_is_set(&it, 2));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e2, it.entities[0]);
    test_uint(TagA, ecs_field_id(&it, 1));
    test_uint(ecs_pair(Movement, Running), ecs_field_id(&it, 2));
    test_uint(Running, ecs_iter_get_var(&it, x_var));
    test_bool(true, ecs_field_is_set(&it, 1));
    test_bool(false, ecs_field_is_set(&it, 2));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e7, it.entities[0]);
    test_uint(TagA, ecs_field_id(&it, 1));
    test_uint(ecs_pair(Movement, Running), ecs_field_id(&it, 2));
    test_uint(Running, ecs_iter_get_var(&it, x_var));
    test_bool(true, ecs_field_is_set(&it, 1));
    test_bool(false, ecs_field_is_set(&it, 2));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e6, it.entities[0]);
    test_uint(TagA, ecs_field_id(&it, 1));
    test_uint(ecs_pair(Movement, Running), ecs_field_id(&it, 2));
    test_uint(Running, ecs_iter_get_var(&it, x_var));
    test_bool(true, ecs_field_is_set(&it, 1));
    test_bool(false, ecs_field_is_set(&it, 2));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}
