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

void Union_query_switch(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Movement, Union);
    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);
    ECS_TAG(world, Jumping);

    ecs_query_t *q = ecs_query(world, {
        .expr = "(Movement, *)"
    });
    test_assert(q != NULL);

    ECS_ENTITY(world, e1, (Movement, Running));
    ECS_ENTITY(world, e2, (Movement, Walking));
    ECS_ENTITY(world, e3, (Movement, Running));

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], e2);
    test_uint(ecs_pair(Movement, Walking), ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], e3);
    test_uint(ecs_pair(Movement, Running), ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], e1);
    test_uint(ecs_pair(Movement, Running), ecs_field_id(&it, 0));
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Union_query_1_case_1_type(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Movement, Union);
    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);
    ECS_TAG(world, Jumping);

    ecs_query_t *q = ecs_query(world, {
        .expr = "(Movement, Running)"
    });
    test_assert(q != NULL);

    ECS_ENTITY(world, e1, (Movement, Running));
    ECS_ENTITY(world, e2, (Movement, Walking));
    ECS_ENTITY(world, e3, (Movement, Running));

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], e3);
    test_uint(it.ids[0], ecs_pair(Movement, Running));

    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], e1);
    test_uint(it.ids[0], ecs_pair(Movement, Running));
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Union_query_1_case_2_types(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ECS_ENTITY(world, Movement, Union);
    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);
    ECS_TAG(world, Jumping);

    ecs_query_t *q = ecs_query(world, {
        .expr = "(Movement, Running)"
    });
    test_assert(q != NULL);

    ECS_ENTITY(world, e1, (Movement, Running));
    ECS_ENTITY(world, e2, (Movement, Walking));
    ECS_ENTITY(world, e3, (Movement, Running));
    ECS_ENTITY(world, e4, (Movement, Walking), Position);
    ECS_ENTITY(world, e5, (Movement, Running), Position);
    ECS_ENTITY(world, e6, (Movement, Walking), Position);
    ECS_ENTITY(world, e7, (Movement, Running), Position);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], e7);
    test_uint(ecs_pair(Movement, Running), ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], e5);
    test_uint(ecs_pair(Movement, Running), ecs_field_id(&it, 0));    

    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], e3);
    test_uint(ecs_pair(Movement, Running), ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], e1);
    test_uint(ecs_pair(Movement, Running), ecs_field_id(&it, 0));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Union_query_2_cases_1_type(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Movement, Union);
    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);
    ECS_TAG(world, Jumping);

    ECS_ENTITY(world, Direction, Union);
    ECS_TAG(world, Front);
    ECS_TAG(world, Back);
    ECS_TAG(world, Left);
    ECS_TAG(world, Right);

    ecs_query_t *q = ecs_query(world, {
        .expr = "(Movement, Running), (Direction, Front)"
    });
    test_assert(q != NULL);

    ECS_ENTITY(world, e1, (Movement, Running), (Direction, Front));
    ECS_ENTITY(world, e2, (Movement, Walking), (Direction, Front));
    ECS_ENTITY(world, e3, (Movement, Running), (Direction, Back));
    ECS_ENTITY(world, e4, (Movement, Running), (Direction, Front));
    ECS_ENTITY(world, e5, (Movement, Walking), (Direction, Front));

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], e4);
    test_uint(ecs_pair(Movement, Running), ecs_field_id(&it, 0));
    test_uint(ecs_pair(Direction, Front), ecs_field_id(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], e1);
    test_uint(ecs_pair(Movement, Running), ecs_field_id(&it, 0));
    test_uint(ecs_pair(Direction, Front), ecs_field_id(&it, 1));
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Union_query_2_cases_2_types(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ECS_ENTITY(world, Movement, Union);
    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);
    ECS_TAG(world, Jumping);

    ECS_ENTITY(world, Direction, Union);
    ECS_TAG(world, Front);
    ECS_TAG(world, Back);
    ECS_TAG(world, Left);
    ECS_TAG(world, Right);

    ecs_query_t *q = ecs_query(world, {
        .expr = "(Movement, Running), (Direction, Front)"
    });
    test_assert(q != NULL);

    ECS_ENTITY(world, e1, (Movement, Running), (Direction, Front));
    ECS_ENTITY(world, e2, (Movement, Walking), (Direction, Front));
    ECS_ENTITY(world, e3, (Movement, Running), (Direction, Back));
    ECS_ENTITY(world, e4, (Movement, Running), (Direction, Front));
    ECS_ENTITY(world, e5, (Movement, Walking), (Direction, Front));
    ECS_ENTITY(world, e6, Position, (Movement, Walking), (Direction, Front));
    ECS_ENTITY(world, e7, Position, (Movement, Running), (Direction, Front));
    ECS_ENTITY(world, e8, Position, (Movement, Walking), (Direction, Front));

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], e7);
    test_uint(ecs_pair(Movement, Running), ecs_field_id(&it, 0));
    test_uint(ecs_pair(Direction, Front), ecs_field_id(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], e4);
    test_uint(ecs_pair(Movement, Running), ecs_field_id(&it, 0));
    test_uint(ecs_pair(Direction, Front), ecs_field_id(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], e1);
    test_uint(ecs_pair(Movement, Running), ecs_field_id(&it, 0));
    test_uint(ecs_pair(Direction, Front), ecs_field_id(&it, 1));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Union_query_after_remove(void) {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Movement, Union);
    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);
    ECS_TAG(world, Jumping);

    ECS_ENTITY(world, e1, (Movement, Walking));
    ECS_ENTITY(world, e2, (Movement, Walking));
    ECS_ENTITY(world, e3, (Movement, Running));
    ECS_ENTITY(world, e4, (Movement, Running));
    ECS_ENTITY(world, e5, (Movement, Running));
    ECS_ENTITY(world, e6, (Movement, Jumping));
    ECS_ENTITY(world, e7, (Movement, Jumping));

    ecs_query_t *q_walking = ecs_query(world, {
        .expr = "(Movement, Walking)"
    });
    ecs_query_t *q_running = ecs_query(world, {
        .expr = "(Movement, Running)"
    });
    ecs_query_t *q_jumping = ecs_query(world, {
        .expr = "(Movement, Jumping)"
    });

    /* Verify all queries are correctly matched */
    ecs_iter_t it = ecs_query_iter(world, q_walking);
    test_assert(ecs_query_next(&it));
    test_int(it.count, 1); test_int(it.entities[0], e2);
    test_assert(ecs_query_next(&it));
    test_int(it.count, 1); test_int(it.entities[0], e1);
    test_assert(!ecs_query_next(&it));

    it = ecs_query_iter(world, q_running);
    test_assert(ecs_query_next(&it));
    test_int(it.count, 1); test_int(it.entities[0], e5);
    test_assert(ecs_query_next(&it));
    test_int(it.count, 1); test_int(it.entities[0], e4);
    test_assert(ecs_query_next(&it));
    test_int(it.count, 1); test_int(it.entities[0], e3);    
    test_assert(!ecs_query_next(&it));

    it = ecs_query_iter(world, q_jumping);
    test_assert(ecs_query_next(&it));
    test_int(it.count, 1); test_int(it.entities[0], e7);
    test_assert(ecs_query_next(&it));
    test_int(it.count, 1); test_int(it.entities[0], e6);
    test_assert(!ecs_query_next(&it));

    ecs_remove_pair(world, e4, Movement, Running);
    test_assert(!ecs_has_pair(world, e4, Movement, Running));
    ecs_entity_t c = ecs_get_target(world, e4, Movement, 0);
    test_int(c, 0);

    /* Verify queries are still correctly matched, now excluding e4 */
    it = ecs_query_iter(world, q_walking);
    test_assert(ecs_query_next(&it));
    test_int(it.count, 1); test_int(it.entities[0], e2);
    test_assert(ecs_query_next(&it));
    test_int(it.count, 1); test_int(it.entities[0], e1);
    test_assert(!ecs_query_next(&it));

    it = ecs_query_iter(world, q_running);
    test_assert(ecs_query_next(&it));
    test_int(it.count, 1); test_int(it.entities[0], e5);
    test_assert(ecs_query_next(&it));
    test_int(it.count, 1); test_int(it.entities[0], e3);    
    test_assert(!ecs_query_next(&it));

    it = ecs_query_iter(world, q_jumping);
    test_assert(ecs_query_next(&it));
    test_int(it.count, 1); test_int(it.entities[0], e7);
    test_assert(ecs_query_next(&it));
    test_int(it.count, 1); test_int(it.entities[0], e6);
    test_assert(!ecs_query_next(&it));    

    ecs_add_pair(world, e4, Movement, Running);
    test_assert(ecs_has_pair(world, e4, Movement, Running));
    c = ecs_get_target(world, e4, Movement, 0);
    test_int(c, Running);

    /* Verify e4 is now matched again */
    it = ecs_query_iter(world, q_walking);
    test_assert(ecs_query_next(&it));
    test_int(it.count, 1); test_int(it.entities[0], e2);
    test_assert(ecs_query_next(&it));
    test_int(it.count, 1); test_int(it.entities[0], e1);
    test_assert(!ecs_query_next(&it));

    it = ecs_query_iter(world, q_running);
    test_assert(ecs_query_next(&it));
    test_int(it.count, 1); test_int(it.entities[0], e4);      
    test_assert(ecs_query_next(&it));
    test_int(it.count, 1); test_int(it.entities[0], e5);
    test_assert(ecs_query_next(&it));
    test_int(it.count, 1); test_int(it.entities[0], e3);      
    test_assert(!ecs_query_next(&it));

    it = ecs_query_iter(world, q_jumping);
    test_assert(ecs_query_next(&it));
    test_int(it.count, 1); test_int(it.entities[0], e7);
    test_assert(ecs_query_next(&it));
    test_int(it.count, 1); test_int(it.entities[0], e6);
    test_assert(!ecs_query_next(&it));

    ecs_query_fini(q_walking);
    ecs_query_fini(q_jumping);
    ecs_query_fini(q_running);

    ecs_fini(world);
}

static
int compare_position(ecs_entity_t e1, const void *ptr1, ecs_entity_t e2, const void *ptr2) {
    const Position *p1 = ptr1;
    const Position *p2 = ptr2;
    return (p1->x > p2->x) - (p1->x < p2->x);
}

void Union_sort(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_ENTITY(world, Movement, Union);
    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);
    ECS_TAG(world, Jumping);
    ECS_TAG(world, Sitting);

    ecs_entity_t e1 = ecs_insert(world, ecs_value(Position, {3, 2}));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(Position, {2, 2}));
    ecs_entity_t e3 = ecs_insert(world, ecs_value(Position, {1, 2}));
    ecs_entity_t e4 = ecs_insert(world, ecs_value(Position, {0, 2}));

    ecs_add_pair(world, e1, Movement, Walking);
    ecs_add_pair(world, e2, Movement, Running);
    ecs_add_pair(world, e3, Movement, Jumping);
    ecs_add_pair(world, e4, Movement, Sitting);
    
    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .terms = {{ ecs_id(Position) }},
        .order_by = ecs_id(Position), 
        .order_by_callback = compare_position
    });

    ecs_iter_t it = ecs_query_iter(world, q);
    test_assert(ecs_query_next(&it));
    test_int(it.count, 4);
    test_assert(it.entities[0] == e4);
    test_assert(it.entities[1] == e3);
    test_assert(it.entities[2] == e2);
    test_assert(it.entities[3] == e1);
    test_assert(!ecs_query_next(&it));

    /* Entities will have shuffled around, ensure cases got shuffled too */
    test_uint(ecs_get_target(world, e1, Movement, 0), Walking);
    test_uint(ecs_get_target(world, e2, Movement, 0), Running);
    test_uint(ecs_get_target(world, e3, Movement, 0), Jumping);
    test_uint(ecs_get_target(world, e4, Movement, 0), Sitting);

    ecs_fini(world);
}

void Union_query_recycled_tags(void) {
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

    ecs_entity_t e = ecs_new(world);
    ecs_add_pair(world, e, Movement, Standing);

    ecs_query_t *q = ecs_query(world, {
        .expr = "(Movement, Standing)"
    });

    ecs_iter_t it = ecs_query_iter(world, q);
    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e);
    test_uint(ecs_pair(Movement, Standing), ecs_field_id(&it, 0));

    test_assert(!ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Union_query_single_case(void) {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Movement, Union);
    ECS_TAG(world, Walking);

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

    test_assert(ecs_has_pair(world, e1, Movement, Walking));
    test_assert(ecs_has_pair(world, e2, Movement, Walking));
    test_assert(ecs_has_pair(world, e3, Movement, Walking));

    ecs_delete(world, e1);

    test_assert(ecs_has_pair(world, e2, Movement, Walking));
    test_assert(ecs_has_pair(world, e3, Movement, Walking));

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

void Union_match_switch_on_base_instance(void) {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Movement, Union);
    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);
    ECS_TAG(world, Jumping);

    ecs_query_t *q = ecs_query(world, {
        .expr = "(Movement, Walking)"
    });
    test_assert(q != NULL);

    ecs_entity_t base = ecs_new(world);
    test_assert(base != 0);

    ecs_entity_t e1 = ecs_new_w_pair(world, Movement, Walking);
    test_assert(e1 != 0);
    ecs_add_pair(world, e1, EcsIsA, base);

    test_uint(ecs_get_target(world, e1, Movement, 0), Walking);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(ecs_query_next(&it), true);
    test_int(it.count, 1);
    test_int(it.entities[0], e1);

    test_bool(ecs_query_next(&it), false);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Union_switch_w_bitset_query(void) {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Movement, Union);
    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);
    ECS_TAG(world, Jumping);

    ECS_COMPONENT(world, Position);
    ecs_add_id(world, ecs_id(Position), EcsCanToggle);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Position, (Movement, Walking)"
    });
    test_assert(q != NULL);

    ecs_entity_t e1 = ecs_new_w_pair(world, Movement, Walking);
    ecs_set(world, e1, Position, {10, 20});
    ecs_enable_component(world, e1, Position, true);

    ecs_entity_t e2 = ecs_new_w_pair(world, Movement, Walking);
    ecs_set(world, e2, Position, {11, 22});
    ecs_enable_component(world, e2, Position, false);

    ecs_entity_t e3 = ecs_new_w_pair(world, Movement, Walking);
    ecs_set(world, e3, Position, {13, 23});

    test_bool(ecs_is_enabled(world, e1, Position), true);
    test_bool(ecs_is_enabled(world, e2, Position), false);

    Position *p;

    ecs_iter_t it = ecs_query_iter(world, q);
    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e3);
    p = ecs_field(&it, Position, 0);
    test_assert(p != NULL);
    test_int(p->x, 13);
    test_int(p->y, 23);
    test_uint(ecs_pair(Movement, Walking), ecs_field_id(&it, 1));

    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e1);
    p = ecs_field(&it, Position, 0);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);
    test_uint(ecs_pair(Movement, Walking), ecs_field_id(&it, 1));

    test_assert(!ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Union_switch_w_bitset_query_inv(void) {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Movement, Union);
    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);
    ECS_TAG(world, Jumping);

    ECS_COMPONENT(world, Position);
    ecs_add_id(world, ecs_id(Position), EcsCanToggle);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Position, (Movement, Walking)"
    });
    test_assert(q != NULL);

    ecs_entity_t e1 = ecs_new_w_pair(world, Movement, Walking);
    ecs_set(world, e1, Position, {10, 20});
    ecs_enable_component(world, e1, Position, true);

    ecs_entity_t e2 = ecs_new_w_pair(world, Movement, Running);
    ecs_set(world, e2, Position, {11, 22});
    ecs_enable_component(world, e2, Position, true);

    ecs_entity_t e3 = ecs_new_w_pair(world, Movement, Walking);
    ecs_set(world, e3, Position, {13, 23});

    Position *p;

    ecs_iter_t it = ecs_query_iter(world, q);
    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e3);
    p = ecs_field(&it, Position, 0);
    test_assert(p != NULL);
    test_int(p->x, 13);
    test_int(p->y, 23);
    test_uint(ecs_pair(Movement, Walking), ecs_field_id(&it, 1));

    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e1);
    p = ecs_field(&it, Position, 0);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);
    test_uint(ecs_pair(Movement, Walking), ecs_field_id(&it, 1));

    test_assert(!ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Union_switch_w_bitset_query_2_elems(void) {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Movement, Union);
    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);
    ECS_TAG(world, Jumping);

    ECS_COMPONENT(world, Position);
    ecs_add_id(world, ecs_id(Position), EcsCanToggle);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Position, (Movement, Walking)"
    });
    test_assert(q != NULL);

    ecs_entity_t e1 = ecs_new_w_pair(world, Movement, Walking);
    ecs_set(world, e1, Position, {10, 20});
    ecs_enable_component(world, e1, Position, true);

    ecs_entity_t e2 = ecs_new_w_pair(world, Movement, Walking);
    ecs_set(world, e2, Position, {11, 22});
    ecs_enable_component(world, e2, Position, true);

    ecs_entity_t e0 = ecs_new_w_pair(world, Movement, Walking);
    ecs_set(world, e0, Position, {11, 22});
    ecs_enable_component(world, e0, Position, false);

    ecs_entity_t e3 = ecs_new_w_pair(world, Movement, Walking);
    ecs_set(world, e3, Position, {13, 23});

    Position *p;

    ecs_iter_t it = ecs_query_iter(world, q);

    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e3);
    p = ecs_field(&it, Position, 0);
    test_assert(p != NULL);
    test_int(p->x, 13);
    test_int(p->y, 23);
    test_uint(ecs_pair(Movement, Walking), ecs_field_id(&it, 1));

    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e1);
    p = ecs_field(&it, Position, 0);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);
    test_uint(ecs_pair(Movement, Walking), ecs_field_id(&it, 1));

    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e2);
    p = ecs_field(&it, Position, 0);
    test_assert(p != NULL);
    test_int(p->x, 11);
    test_int(p->y, 22);
    test_uint(ecs_pair(Movement, Walking), ecs_field_id(&it, 1));

    test_assert(!ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Union_switch_w_bitset_query_2_elems_skip(void) {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Movement, Union);
    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);
    ECS_TAG(world, Jumping);

    ECS_COMPONENT(world, Position);
    ecs_add_id(world, ecs_id(Position), EcsCanToggle);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Position, (Movement, Walking)"
    });
    test_assert(q != NULL);

    ecs_entity_t e1 = ecs_new_w_pair(world, Movement, Walking);
    ecs_set(world, e1, Position, {10, 20});
    ecs_enable_component(world, e1, Position, true);

    ecs_entity_t e0 = ecs_new_w_pair(world, Movement, Walking);
    ecs_set(world, e0, Position, {11, 22});
    ecs_enable_component(world, e0, Position, false);

    ecs_entity_t e2 = ecs_new_w_pair(world, Movement, Walking);
    ecs_set(world, e2, Position, {11, 22});
    ecs_enable_component(world, e2, Position, true);

    ecs_entity_t e3 = ecs_new_w_pair(world, Movement, Walking);
    ecs_set(world, e3, Position, {13, 23});

    Position *p;

    ecs_iter_t it = ecs_query_iter(world, q);

    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e3);
    p = ecs_field(&it, Position, 0);
    test_assert(p != NULL);
    test_int(p->x, 13);
    test_int(p->y, 23);
    test_uint(ecs_pair(Movement, Walking), ecs_field_id(&it, 1));

    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e1);
    p = ecs_field(&it, Position, 0);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);
    test_uint(ecs_pair(Movement, Walking), ecs_field_id(&it, 1));

    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e2);
    p = ecs_field(&it, Position, 0);
    test_assert(p != NULL);
    test_int(p->x, 11);
    test_int(p->y, 22);
    test_uint(ecs_pair(Movement, Walking), ecs_field_id(&it, 1));

    test_assert(!ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Union_switch_w_bitset_query_elems_interleaved(void) {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Movement, Union);
    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);
    ECS_TAG(world, Jumping);

    ECS_COMPONENT(world, Position);
    ecs_add_id(world, ecs_id(Position), EcsCanToggle);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Position, (Movement, Walking)"
    });
    test_assert(q != NULL);

    ecs_entity_t e1 = ecs_new_w_pair(world, Movement, Walking);
    ecs_set(world, e1, Position, {10, 20});
    ecs_enable_component(world, e1, Position, false);

    ecs_entity_t e2 = ecs_new_w_pair(world, Movement, Running);
    ecs_set(world, e2, Position, {11, 22});
    ecs_enable_component(world, e2, Position, true);

    ecs_entity_t e3 = ecs_new_w_pair(world, Movement, Walking);
    ecs_set(world, e3, Position, {13, 23});
    ecs_enable_component(world, e3, Position, false);

    ecs_entity_t e4 = ecs_new_w_pair(world, Movement, Running);
    ecs_set(world, e4, Position, {13, 23});
    ecs_enable_component(world, e4, Position, true);

    ecs_entity_t e5 = ecs_new_w_pair(world, Movement, Walking);
    ecs_set(world, e5, Position, {13, 23});
    ecs_enable_component(world, e5, Position, true);

    ecs_entity_t e6 = ecs_new_w_pair(world, Movement, Running);
    ecs_set(world, e6, Position, {13, 23});
    ecs_enable_component(world, e6, Position, true);

    ecs_entity_t e7 = ecs_new_w_pair(world, Movement, Walking);
    ecs_set(world, e7, Position, {13, 23});
    ecs_enable_component(world, e7, Position, false);

    ecs_entity_t e8 = ecs_new_w_pair(world, Movement, Walking);
    ecs_set(world, e8, Position, {13, 23});
    ecs_enable_component(world, e8, Position, true);

    Position *p;

    ecs_iter_t it = ecs_query_iter(world, q);

    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e5);
    p = ecs_field(&it, Position, 0);
    test_assert(p != NULL);
    test_int(p->x, 13);
    test_int(p->y, 23);
    test_uint(ecs_pair(Movement, Walking), ecs_field_id(&it, 1));

    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e8);
    p = ecs_field(&it, Position, 0);
    test_assert(p != NULL);
    test_int(p->x, 13);
    test_int(p->y, 23);
    test_uint(ecs_pair(Movement, Walking), ecs_field_id(&it, 1));

    test_assert(!ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Union_switch_w_bitset_query_elems_interleaved_2_types(void) {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Movement, Union);
    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);
    ECS_TAG(world, Jumping);

    ECS_COMPONENT(world, Position);
    ecs_add_id(world, ecs_id(Position), EcsCanToggle);

    ECS_TAG(world, Tag);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Position, (Movement, Walking)"
    });
    test_assert(q != NULL);

    ecs_entity_t e1 = ecs_new_w_pair(world, Movement, Walking);
    ecs_set(world, e1, Position, {10, 20});
    ecs_enable_component(world, e1, Position, false);

    ecs_entity_t e2 = ecs_new_w_pair(world, Movement, Running);
    ecs_set(world, e2, Position, {11, 22});
    ecs_enable_component(world, e2, Position, true);

    ecs_entity_t e3 = ecs_new_w_pair(world, Movement, Walking);
    ecs_set(world, e3, Position, {13, 23});
    ecs_enable_component(world, e3, Position, false);

    ecs_entity_t e4 = ecs_new_w_pair(world, Movement, Running);
    ecs_set(world, e4, Position, {13, 23});
    ecs_enable_component(world, e4, Position, true);

    ecs_entity_t e5 = ecs_new_w_pair(world, Movement, Walking);
    ecs_set(world, e5, Position, {13, 23});
    ecs_enable_component(world, e5, Position, true);

    ecs_entity_t e6 = ecs_new_w_pair(world, Movement, Running);
    ecs_set(world, e6, Position, {13, 23});
    ecs_enable_component(world, e6, Position, true);

    ecs_entity_t e7 = ecs_new_w_pair(world, Movement, Walking);
    ecs_set(world, e7, Position, {13, 23});
    ecs_enable_component(world, e7, Position, false);

    ecs_entity_t e8 = ecs_new_w_pair(world, Movement, Walking);
    ecs_set(world, e8, Position, {13, 23});
    ecs_enable_component(world, e8, Position, true);


    ecs_entity_t e1_2 = ecs_new_w_pair(world, Movement, Walking);
    ecs_set(world, e1_2, Position, {10, 20});
    ecs_enable_component(world, e1_2, Position, false);
    ecs_add_id(world, e1_2, Tag);

    ecs_entity_t e2_2 = ecs_new_w_pair(world, Movement, Running);
    ecs_set(world, e2_2, Position, {11, 22});
    ecs_enable_component(world, e2_2, Position, true);
    ecs_add_id(world, e2_2, Tag);

    ecs_entity_t e3_2 = ecs_new_w_pair(world, Movement, Walking);
    ecs_set(world, e3_2, Position, {13, 23});
    ecs_enable_component(world, e3_2, Position, false);
    ecs_add_id(world, e3_2, Tag);

    ecs_entity_t e4_2 = ecs_new_w_pair(world, Movement, Running);
    ecs_set(world, e4_2, Position, {13, 23});
    ecs_enable_component(world, e4_2, Position, true);
    ecs_add_id(world, e4_2, Tag);

    ecs_entity_t e5_2 = ecs_new_w_pair(world, Movement, Walking);
    ecs_set(world, e5_2, Position, {13, 23});
    ecs_enable_component(world, e5_2, Position, true);
    ecs_add_id(world, e5_2, Tag);

    ecs_entity_t e6_2 = ecs_new_w_pair(world, Movement, Running);
    ecs_set(world, e6_2, Position, {13, 23});
    ecs_enable_component(world, e6_2, Position, true);
    ecs_add_id(world, e6_2, Tag);

    ecs_entity_t e7_2 = ecs_new_w_pair(world, Movement, Walking);
    ecs_set(world, e7_2, Position, {13, 23});
    ecs_enable_component(world, e7_2, Position, false);
    ecs_add_id(world, e7_2, Tag);

    ecs_entity_t e8_2 = ecs_new_w_pair(world, Movement, Walking);
    ecs_set(world, e8_2, Position, {13, 23});
    ecs_enable_component(world, e8_2, Position, true);
    ecs_add_id(world, e8_2, Tag);

    Position *p;

    ecs_iter_t it = ecs_query_iter(world, q);

    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e5);
    p = ecs_field(&it, Position, 0);
    test_assert(p != NULL);
    test_int(p->x, 13);
    test_int(p->y, 23);
    test_uint(ecs_pair(Movement, Walking), ecs_field_id(&it, 1));

    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e8);
    p = ecs_field(&it, Position, 0);
    test_assert(p != NULL);
    test_int(p->x, 13);
    test_int(p->y, 23);
    test_uint(ecs_pair(Movement, Walking), ecs_field_id(&it, 1));

    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e5_2);
    p = ecs_field(&it, Position, 0);
    test_assert(p != NULL);
    test_int(p->x, 13);
    test_int(p->y, 23);
    test_uint(ecs_pair(Movement, Walking), ecs_field_id(&it, 1));

    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e8_2);
    p = ecs_field(&it, Position, 0);
    test_assert(p != NULL);
    test_int(p->x, 13);
    test_int(p->y, 23);
    test_uint(ecs_pair(Movement, Walking), ecs_field_id(&it, 1));

    test_assert(!ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Union_has_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Movement, Union);
    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);
    ECS_TAG(world, Jumping);

    ecs_entity_t e = ecs_new(world);
    test_assert( !ecs_has_pair(world, e, Movement, EcsWildcard));

    ecs_add_pair(world, e, Movement, Walking);
    test_assert( ecs_has_pair(world, e, Movement, EcsWildcard));

    test_assert( ecs_has_pair(world, e, Movement, Walking));
    test_assert( !ecs_has_pair(world, e, Movement, Running));
    test_assert( !ecs_has_pair(world, e, Movement, Jumping));
    test_assert( ecs_has_pair(world, e, Movement, EcsWildcard));

    ecs_fini(world);
}

void Union_remove_wildcard(void) {
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

    ecs_remove_pair(world, e, Movement, EcsWildcard);

    test_assert( !ecs_has_pair(world, e, Movement, Walking));
    test_assert( !ecs_has_pair(world, e, Movement, Running));
    test_assert( !ecs_has_pair(world, e, Movement, Jumping));
    test_uint(ecs_get_target(world, e, Movement, 0), 0);
    test_assert( !ecs_has_pair(world, e, Movement, EcsWildcard));

    ecs_fini(world);
}

void Union_component_relation(void) {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Movement, Union);
    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);
    ECS_TAG(world, Jumping);

    ECS_ENTITY(world, Direction, Union);
    ECS_TAG(world, Front);
    ECS_TAG(world, Back);
    ECS_TAG(world, Left);
    ECS_TAG(world, Right);
    
    ecs_set(world, Movement, EcsComponent, { .size = 1, .alignment = 1 });
    ecs_set(world, Direction, EcsComponent, { .size = 1, .alignment = 1 });

    ecs_entity_t e1 = ecs_new(world);
    ecs_add_pair(world, e1, Movement, Walking);
    ecs_add_pair(world, e1, Direction, Front);

    ecs_entity_t e2 = ecs_new(world);
    ecs_add_pair(world, e2, Movement, Running);
    ecs_add_pair(world, e2, Direction, Left);

    ecs_query_t *q = ecs_query(world, {
        .expr = "(Movement, Walking), (Direction, *)"
    });
    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);

    test_bool(ecs_query_next(&it), true);
    test_int(it.count, 1);
    test_uint(it.entities[0], e1);
    test_uint(it.ids[0], ecs_pair(Movement, Walking));
    test_uint(it.ids[1], ecs_pair(Direction, Front));

    test_bool(ecs_query_next(&it), false);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Union_switch_term_filter(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Sw, Union);
    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_entity_t e = ecs_new_w_pair(world, Sw, TagA);
    ecs_table_t *table = ecs_get_table(world, e);
    
    ecs_query_t *q = ecs_query(world, {
        .terms = {{ .id = ecs_pair(Sw, EcsWildcard), .inout = EcsInOutNone }}
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    
    test_bool(ecs_query_next(&it), true);
    test_int(it.count, 1);
    test_int(it.entities[0], e);
    test_uint(ecs_pair(Sw, TagA), ecs_field_id(&it, 0));
    test_assert(it.table == table);

    test_bool(ecs_query_next(&it), false);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Union_2_terms_switch_term_filter(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Sw, Union);
    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new_w_pair(world, Sw, TagA);
    ecs_add(world, e, Position);
    ecs_table_t *table = ecs_get_table(world, e);
    
    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { .id = ecs_id(Position) },
            { .id = ecs_pair(Sw, EcsWildcard), .inout = EcsInOutNone }
        }
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    
    test_bool(ecs_query_next(&it), true);
    test_int(it.count, 1);
    test_int(it.entities[0], e);
    test_int(it.sizes[0], ECS_SIZEOF(Position));
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(ecs_pair(Sw, TagA), ecs_field_id(&it, 1));
    test_assert(it.table == table);

    test_bool(ecs_query_next(&it), false);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Union_match_switch_w_switch(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Sw, Union);
    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_entity_t e = ecs_new_w_pair(world, Sw, TagA);
    ecs_table_t *table = ecs_get_table(world, e);
    
    ecs_query_t *q = ecs_query(world, {
        .terms = {{ ecs_pair(Sw, EcsWildcard) }}
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    
    test_bool(ecs_query_next(&it), true);
    test_int(it.count, 1);
    test_int(it.entities[0], e);
    test_uint(ecs_pair(Sw, TagA), ecs_field_id(&it, 0));
    test_assert(it.table == table);

    test_bool(ecs_query_next(&it), false);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Union_match_switch_w_case(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Sw, Union);
    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_entity_t e = ecs_new_w_pair(world, Sw, TagA);
    ecs_table_t *table = ecs_get_table(world, e);
    
    ecs_query_t *q = ecs_query(world, {
        .terms = {{ ecs_pair(Sw, TagA) }}
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);

    test_bool(ecs_query_next(&it), true);
    test_int(it.count, 1);
    test_int(it.entities[0], e);
    test_assert(it.table == table);
    test_uint(ecs_pair(Sw, TagA), ecs_field_id(&it, 0));

    test_bool(ecs_query_next(&it), false);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Union_match_switch_w_case_2_terms(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, SwX, Union);
    ECS_ENTITY(world, SwY, Union);
    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, TagC);
    ECS_TAG(world, TagD);

    ecs_entity_t e = ecs_new_w_pair(world, SwX, TagA);
    ecs_add_pair(world, e, SwY, TagC);
    ecs_table_t *table = ecs_get_table(world, e);

    /* Not matched */
    ecs_new_w_pair(world, SwX, TagA);
    ecs_new_w_pair(world, SwY, TagC);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { ecs_pair(SwX, TagA) },
            { ecs_pair(SwY, TagC) }
        }
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    
    test_bool(ecs_query_next(&it), true);
    test_int(it.count, 1);
    test_int(it.entities[0], e);
    test_assert(it.table == table);
    test_uint(it.ids[0], ecs_pair(SwX, TagA));
    test_uint(it.ids[1], ecs_pair(SwY, TagC));

    test_bool(ecs_query_next(&it), false);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Union_up(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, SwX, Union);
    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, TagC);

    ecs_entity_t b1 = ecs_new_w_pair(world, SwX, TagA);
    ecs_entity_t b2 = ecs_new_w_pair(world, SwX, TagB);
    ecs_entity_t e1 = ecs_new_w_pair(world, EcsChildOf, b1);
    ecs_entity_t e2 = ecs_new_w_pair(world, EcsChildOf, b1);
    ecs_entity_t e3 = ecs_new_w_pair(world, EcsChildOf, b2);
    ecs_entity_t e4 = ecs_new_w_pair(world, EcsChildOf, b2);
    ecs_add_pair(world, e4, SwX, TagC);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { ecs_pair(SwX, EcsWildcard), .src.id = EcsUp }
        },
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    
    test_bool(ecs_query_next(&it), true);
    test_int(it.count, 2);
    test_int(it.entities[0], e1);
    test_int(it.entities[1], e2);
    test_uint(it.ids[0], ecs_pair(SwX, TagA));

    test_bool(ecs_query_next(&it), true);
    test_int(it.count, 1);
    test_int(it.entities[0], e3);
    test_uint(it.ids[0], ecs_pair(SwX, TagB));

    test_bool(ecs_query_next(&it), true);
    test_int(it.count, 1);
    test_int(it.entities[0], e4);
    test_uint(it.ids[0], ecs_pair(SwX, TagB));

    test_bool(ecs_query_next(&it), false);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Union_self_up(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, SwX, Union);
    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, TagC);

    ecs_entity_t b1 = ecs_new_w_pair(world, SwX, TagA);
    ecs_entity_t b2 = ecs_new_w_pair(world, SwX, TagB);
    ecs_entity_t e1 = ecs_new_w_pair(world, EcsChildOf, b1);
    ecs_entity_t e2 = ecs_new_w_pair(world, EcsChildOf, b1);
    ecs_entity_t e3 = ecs_new_w_pair(world, EcsChildOf, b2);
    ecs_entity_t e4 = ecs_new_w_pair(world, EcsChildOf, b2);
    ecs_add_pair(world, e4, SwX, TagC);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { ecs_pair(SwX, EcsWildcard), .src.id = EcsSelf|EcsUp }
        },
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);

    test_bool(ecs_query_next(&it), true);
    test_int(it.count, 1);
    test_int(it.entities[0], b1);
    test_uint(it.ids[0], ecs_pair(SwX, TagA));

    test_bool(ecs_query_next(&it), true);
    test_int(it.count, 2);
    test_int(it.entities[0], e1);
    test_int(it.entities[1], e2);
    test_uint(it.ids[0], ecs_pair(SwX, TagA));

    test_bool(ecs_query_next(&it), true);
    test_int(it.count, 1);
    test_int(it.entities[0], e4);
    test_uint(it.ids[0], ecs_pair(SwX, TagC));

    test_bool(ecs_query_next(&it), true);
    test_int(it.count, 1);
    test_int(it.entities[0], b2);
    test_uint(it.ids[0], ecs_pair(SwX, TagB));

    test_bool(ecs_query_next(&it), true);
    test_int(it.count, 1);
    test_int(it.entities[0], e3);
    test_uint(it.ids[0], ecs_pair(SwX, TagB));

    test_bool(ecs_query_next(&it), false);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Union_up_written(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, SwX, Union);
    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, TagC);
    ECS_TAG(world, Foo);

    ecs_entity_t b1 = ecs_new_w_pair(world, SwX, TagA);
    ecs_entity_t b2 = ecs_new_w_pair(world, SwX, TagB);
    ecs_entity_t e1 = ecs_new_w_pair(world, EcsChildOf, b1);
    ecs_entity_t e2 = ecs_new_w_pair(world, EcsChildOf, b1);
    ecs_entity_t e3 = ecs_new_w_pair(world, EcsChildOf, b2);
    ecs_entity_t e4 = ecs_new_w_pair(world, EcsChildOf, b2);
    ecs_add_pair(world, e4, SwX, TagC);

    ecs_add(world, b1, Foo);
    ecs_add(world, b2, Foo);
    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);
    ecs_add(world, e3, Foo);
    ecs_add(world, e4, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { Foo },
            { ecs_pair(SwX, EcsWildcard), .src.id = EcsUp }
        },
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);    
    test_bool(ecs_query_next(&it), true);
    test_int(it.count, 2);
    test_int(it.entities[0], e1);
    test_int(it.entities[1], e2);
    test_uint(it.ids[0], Foo);
    test_uint(it.ids[1], ecs_pair(SwX, TagA));

    test_bool(ecs_query_next(&it), true);
    test_int(it.count, 1);
    test_int(it.entities[0], e3);
    test_uint(it.ids[0], Foo);
    test_uint(it.ids[1], ecs_pair(SwX, TagB));

    test_bool(ecs_query_next(&it), true);
    test_int(it.count, 1);
    test_int(it.entities[0], e4);
    test_uint(it.ids[0], Foo);
    test_uint(it.ids[1], ecs_pair(SwX, TagB));

    test_bool(ecs_query_next(&it), false);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Union_self_up_written(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, SwX, Union);
    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, TagC);
    ECS_TAG(world, Foo);

    ecs_entity_t b1 = ecs_new_w_pair(world, SwX, TagA);
    ecs_entity_t b2 = ecs_new_w_pair(world, SwX, TagB);
    ecs_entity_t e1 = ecs_new_w_pair(world, EcsChildOf, b1);
    ecs_entity_t e2 = ecs_new_w_pair(world, EcsChildOf, b1);
    ecs_entity_t e3 = ecs_new_w_pair(world, EcsChildOf, b2);
    ecs_entity_t e4 = ecs_new_w_pair(world, EcsChildOf, b2);
    ecs_add_pair(world, e4, SwX, TagC);

    ecs_add(world, b1, Foo);
    ecs_add(world, b2, Foo);
    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);
    ecs_add(world, e3, Foo);
    ecs_add(world, e4, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { Foo },
            { ecs_pair(SwX, EcsWildcard), .src.id = EcsSelf|EcsUp }
        },
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);

    test_bool(ecs_query_next(&it), true);
    test_int(it.count, 1);
    test_int(it.entities[0], b1);
    test_uint(it.ids[0], Foo);
    test_uint(it.ids[1], ecs_pair(SwX, TagA));

    test_bool(ecs_query_next(&it), true);
    test_int(it.count, 1);
    test_int(it.entities[0], b2);
    test_uint(it.ids[0], Foo);
    test_uint(it.ids[1], ecs_pair(SwX, TagB));

    test_bool(ecs_query_next(&it), true);
    test_int(it.count, 2);
    test_int(it.entities[0], e1);
    test_int(it.entities[1], e2);
    test_uint(it.ids[0], Foo);
    test_uint(it.ids[1], ecs_pair(SwX, TagA));

    test_bool(ecs_query_next(&it), true);
    test_int(it.count, 1);
    test_int(it.entities[0], e3);
    test_uint(it.ids[0], Foo);
    test_uint(it.ids[1], ecs_pair(SwX, TagB));

    test_bool(ecs_query_next(&it), true);
    test_int(it.count, 1);
    test_int(it.entities[0], e4);
    test_uint(it.ids[0], Foo);
    test_uint(it.ids[1], ecs_pair(SwX, TagC));

    test_bool(ecs_query_next(&it), false);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Union_existing_union_table(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Movement, Union);
    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);

    ecs_entity_t e1 = ecs_new_w_pair(world, Movement, Walking);

    ecs_query_t *q = ecs_query(world, {
        .expr = "(Movement, *)",
        .cache_kind = EcsQueryCacheAuto
    });
    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], e1);

    test_uint(ecs_field_id(&it, 0), ecs_pair(Movement, Walking));
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Union_new_union_table(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Movement, Union);
    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);

    ecs_query_t *q = ecs_query(world, {
        .expr = "(Movement, *)",
        .cache_kind = EcsQueryCacheAuto
    });
    test_assert(q != NULL);

    ecs_entity_t e1 = ecs_new_w_pair(world, Movement, Walking);
    ecs_entity_t e2 = ecs_new_w_pair(world, Movement, Running);
    ecs_entity_t e3 = ecs_new_w_pair(world, Movement, Walking);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], e3);
    test_uint(ecs_field_id(&it, 0), ecs_pair(Movement, Walking));

    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], e1);
    test_uint(ecs_field_id(&it, 0), ecs_pair(Movement, Walking));

    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], e2);
    test_uint(ecs_field_id(&it, 0), ecs_pair(Movement, Running));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Union_existing_union_table_w_tgt(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Movement, Union);
    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);

    ecs_entity_t e1 = ecs_new_w_pair(world, Movement, Walking);
    ecs_new_w_pair(world, Movement, Running);
    ecs_entity_t e3 = ecs_new_w_pair(world, Movement, Walking);

    ecs_query_t *q = ecs_query(world, {
        .expr = "(Movement, Walking)",
        .cache_kind = cache_kind
    });
    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], e3);
    test_uint(ecs_field_id(&it, 0), ecs_pair(Movement, Walking));

    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], e1);
    test_uint(ecs_field_id(&it, 0), ecs_pair(Movement, Walking));
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Union_new_union_table_w_tgt(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Movement, Union);
    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);

    ecs_query_t *q = ecs_query(world, {
        .expr = "(Movement, Walking)",
        .cache_kind = cache_kind
    });
    test_assert(q != NULL);

    ecs_entity_t e1 = ecs_new_w_pair(world, Movement, Walking);
    ecs_new_w_pair(world, Movement, Running);
    ecs_entity_t e3 = ecs_new_w_pair(world, Movement, Walking);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], e3);
    test_uint(ecs_field_id(&it, 0), ecs_pair(Movement, Walking));

    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], e1);
    test_uint(ecs_field_id(&it, 0), ecs_pair(Movement, Walking));
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Union_tgt_w_generation(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Rel, Union);

    ecs_entity_t tgt_1 = ecs_new(world);
    ecs_delete(world, tgt_1);
    tgt_1 = ecs_new(world);
    test_assert(tgt_1 != (uint32_t)tgt_1);

    ecs_entity_t tgt_2 = ecs_new(world);
    ecs_delete(world, tgt_2);
    tgt_2 = ecs_new(world);
    test_assert(tgt_2 != (uint32_t)tgt_2);

    ecs_query_t *q = ecs_query(world, {
        .expr = "(Rel, *)",
        .cache_kind = cache_kind
    });
    test_assert(q != NULL);

    ecs_entity_t e1 = ecs_new_w_pair(world, Rel, tgt_1);
    ecs_entity_t e2 = ecs_new_w_pair(world, Rel, tgt_2);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], e2);
    test_uint(ecs_field_id(&it, 0), ecs_pair(Rel, tgt_2));

    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], e1);
    test_uint(ecs_field_id(&it, 0), ecs_pair(Rel, tgt_1));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Union_tgt_w_not_alive(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Rel, Union);

    ecs_entity_t tgt_1 = ecs_new(world);
    ecs_delete(world, tgt_1);
    tgt_1 = ecs_new(world);
    test_assert(tgt_1 != (uint32_t)tgt_1);

    ecs_entity_t tgt_2 = ecs_new(world);
    ecs_delete(world, tgt_2);
    tgt_2 = ecs_new(world);
    test_assert(tgt_2 != (uint32_t)tgt_2);

    ecs_query_t *q = ecs_query(world, {
        .expr = "(Rel, *)",
        .cache_kind = cache_kind
    });
    test_assert(q != NULL);

    ecs_entity_t e1 = ecs_new_w_pair(world, Rel, tgt_1);
    ecs_entity_t e2 = ecs_new_w_pair(world, Rel, tgt_2);

    ecs_delete(world, tgt_1);
    ecs_delete(world, tgt_2);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], e2);
    test_uint(ecs_field_id(&it, 0), ecs_pair(Rel, tgt_2));

    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], e1);
    test_uint(ecs_field_id(&it, 0), ecs_pair(Rel, tgt_1));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Union_for_switch_filter_term(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Movement, Union);
    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);

    ecs_query_t *q = ecs_query(world, {
        .expr = "[none] (Movement, *)",
        .cache_kind = cache_kind
    });
    test_assert(q != NULL);

    ecs_entity_t e1 = ecs_new_w_pair(world, Movement, Walking);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], e1);
    test_uint(ecs_field_id(&it, 0), ecs_pair(Movement, Walking));
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Union_union_from_nothing(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);
    ECS_ENTITY(world, Movement, Union);
    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { Tag },
            { ecs_pair(Movement, EcsWildcard), .src.id = EcsIsEntity }
        },
        .cache_kind = cache_kind
    });
    test_assert(q != NULL);

    ecs_entity_t e1 = ecs_new_w_id(world, Tag);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], e1);
    test_uint(ecs_field_id(&it, 0), Tag);
    test_uint(ecs_field_id(&it, 1), ecs_pair(Movement, EcsWildcard));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(false, ecs_field_is_set(&it, 1));
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Union_union_tgt_from_nothing(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);
    ECS_ENTITY(world, Movement, Union);
    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            {Tag},
            {ecs_pair(Movement, Walking), .src.id = EcsIsEntity }
        },
        .cache_kind = cache_kind
    });
    test_assert(q != NULL);

    ecs_entity_t e1 = ecs_new_w_id(world, Tag);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], e1);
    test_uint(ecs_field_id(&it, 0), Tag);
    test_uint(ecs_field_id(&it, 1), ecs_pair(Movement, Walking));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(false, ecs_field_is_set(&it, 1));
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Union_tgt_inherited(void) {
    test_quarantine("Aug 1st 2022");

    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Movement, Union);
    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);

    ecs_entity_t base_1 = ecs_new_w_pair(world, Movement, Walking);
    ecs_entity_t inst_1 = ecs_new_w_pair(world, EcsIsA, base_1);

    ecs_entity_t base_2 = ecs_new_w_pair(world, Movement, Running);
    ecs_new_w_pair(world, EcsIsA, base_2);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { ecs_pair(Movement, Walking) }
        },
        .cache_kind = cache_kind
    });
    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], base_1);
    test_uint(ecs_field_id(&it, 0), ecs_pair(Movement, Walking));

    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], inst_1);
    test_uint(ecs_field_id(&it, 0), ecs_pair(Movement, Walking));
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}
