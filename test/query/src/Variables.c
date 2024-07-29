#include <query.h>

static ecs_query_cache_kind_t cache_kind = EcsQueryCacheDefault;

void Variables_setup(void) {
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

void Variables_1_ent_src_w_var(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);

    ecs_entity_t ent = ecs_entity(world, { .name = "ent" });

    ecs_query_t *q = ecs_query(world, {
        .expr = "$x(ent)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int32_t x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add(world, ent, RelA);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(ent, ecs_field_src(&it, 0));
        test_uint(RelA, ecs_iter_get_var(&it, x_var));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add(world, ent, RelB);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(ent, ecs_field_src(&it, 0));
        test_uint(RelA, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(RelB, ecs_field_id(&it, 0));
        test_uint(ent, ecs_field_src(&it, 0));
        test_uint(RelB, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_1_ent_src_w_pair_rel_var(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, TgtA);

    ecs_entity_t ent = ecs_entity(world, { .name = "ent" });

    ecs_query_t *q = ecs_query(world, {
        .expr = "$x(ent, TgtA)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int32_t x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_pair(world, ent, RelA, TgtA);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 0));
        test_uint(ent, ecs_field_src(&it, 0));
        test_uint(RelA, ecs_iter_get_var(&it, x_var));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_pair(world, ent, RelB, TgtA);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 0));
        test_uint(ent, ecs_field_src(&it, 0));
        test_uint(RelA, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(RelB, TgtA), ecs_field_id(&it, 0));
        test_uint(ent, ecs_field_src(&it, 0));
        test_uint(RelB, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_1_ent_src_w_pair_tgt_var(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_entity_t ent = ecs_entity(world, { .name = "ent" });

    ecs_query_t *q = ecs_query(world, {
        .expr = "RelA(ent, $x)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int32_t x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_pair(world, ent, RelA, TgtA);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 0));
        test_uint(ent, ecs_field_src(&it, 0));
        test_uint(TgtA, ecs_iter_get_var(&it, x_var));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_pair(world, ent, RelA, TgtB);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 0));
        test_uint(ent, ecs_field_src(&it, 0));
        test_uint(TgtA, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(RelA, TgtB), ecs_field_id(&it, 0));
        test_uint(ent, ecs_field_src(&it, 0));
        test_uint(TgtB, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_1_ent_src_w_pair_rel_tgt_var(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_entity_t ent = ecs_entity(world, { .name = "ent" });

    ecs_query_t *q = ecs_query(world, {
        .expr = "$x(ent, $y)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int32_t x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);
    int32_t y_var = ecs_query_find_var(q, "y");
    test_assert(y_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(ecs_id(EcsIdentifier), EcsName), ecs_field_id(&it, 0));
        test_uint(ent, ecs_field_src(&it, 0));
        test_uint(ecs_id(EcsIdentifier), ecs_iter_get_var(&it, x_var));
        test_uint(EcsName, ecs_iter_get_var(&it, y_var));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_pair(world, ent, RelA, TgtA);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(ecs_id(EcsIdentifier), EcsName), ecs_field_id(&it, 0));
        test_uint(ent, ecs_field_src(&it, 0));
        test_uint(ecs_id(EcsIdentifier), ecs_iter_get_var(&it, x_var));
        test_uint(EcsName, ecs_iter_get_var(&it, y_var));

        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 0));
        test_uint(ent, ecs_field_src(&it, 0));
        test_uint(RelA, ecs_iter_get_var(&it, x_var));
        test_uint(TgtA, ecs_iter_get_var(&it, y_var));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_pair(world, ent, RelB, TgtA);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(ecs_id(EcsIdentifier), EcsName), ecs_field_id(&it, 0));
        test_uint(ent, ecs_field_src(&it, 0));
        test_uint(ecs_id(EcsIdentifier), ecs_iter_get_var(&it, x_var));
        test_uint(EcsName, ecs_iter_get_var(&it, y_var));

        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 0));
        test_uint(ent, ecs_field_src(&it, 0));
        test_uint(RelA, ecs_iter_get_var(&it, x_var));
        test_uint(TgtA, ecs_iter_get_var(&it, y_var));

        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(RelB, TgtA), ecs_field_id(&it, 0));
        test_uint(ent, ecs_field_src(&it, 0));
        test_uint(RelB, ecs_iter_get_var(&it, x_var));
        test_uint(TgtA, ecs_iter_get_var(&it, y_var));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_pair(world, ent, RelA, TgtB);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(ecs_id(EcsIdentifier), EcsName), ecs_field_id(&it, 0));
        test_uint(ent, ecs_field_src(&it, 0));
        test_uint(ecs_id(EcsIdentifier), ecs_iter_get_var(&it, x_var));
        test_uint(EcsName, ecs_iter_get_var(&it, y_var));

        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 0));
        test_uint(ent, ecs_field_src(&it, 0));
        test_uint(RelA, ecs_iter_get_var(&it, x_var));
        test_uint(TgtA, ecs_iter_get_var(&it, y_var));

        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(RelA, TgtB), ecs_field_id(&it, 0));
        test_uint(ent, ecs_field_src(&it, 0));
        test_uint(RelA, ecs_iter_get_var(&it, x_var));
        test_uint(TgtB, ecs_iter_get_var(&it, y_var));

        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(RelB, TgtA), ecs_field_id(&it, 0));
        test_uint(ent, ecs_field_src(&it, 0));
        test_uint(RelB, ecs_iter_get_var(&it, x_var));
        test_uint(TgtA, ecs_iter_get_var(&it, y_var));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_1_ent_src_w_pair_rel_tgt_same_var(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_entity_t ent = ecs_entity(world, { .name = "ent" });

    ecs_query_t *q = ecs_query(world, {
        .expr = "$x(ent, $x)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int32_t x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_pair(world, ent, RelA, TgtA);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_pair(world, ent, TgtA, TgtA);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(TgtA, TgtA), ecs_field_id(&it, 0));
        test_uint(ent, ecs_field_src(&it, 0));
        test_uint(TgtA, ecs_iter_get_var(&it, x_var));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_pair(world, ent, TgtB, TgtB);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(TgtA, TgtA), ecs_field_id(&it, 0));
        test_uint(ent, ecs_field_src(&it, 0));
        test_uint(TgtA, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(TgtB, TgtB), ecs_field_id(&it, 0));
        test_uint(ent, ecs_field_src(&it, 0));
        test_uint(TgtB, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_1_ent_src_w_pair_rel_tgt_same_var_after_write(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_entity_t ent = ecs_entity(world, { .name = "ent" });

    ecs_query_t *q = ecs_query(world, {
        .expr = "$x(ent), $x(ent, $x)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int32_t x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_pair(world, ent, RelA, TgtA);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_pair(world, ent, TgtA, TgtA);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add(world, ent, TgtA);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(TgtA, ecs_field_id(&it, 0));
        test_uint(ecs_pair(TgtA, TgtA), ecs_field_id(&it, 1));
        test_uint(ent, ecs_field_src(&it, 0));
        test_uint(ent, ecs_field_src(&it, 1));
        test_uint(TgtA, ecs_iter_get_var(&it, x_var));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_pair(world, ent, TgtB, TgtB);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(TgtA, ecs_field_id(&it, 0));
        test_uint(ecs_pair(TgtA, TgtA), ecs_field_id(&it, 1));
        test_uint(ent, ecs_field_src(&it, 0));
        test_uint(ent, ecs_field_src(&it, 1));
        test_uint(TgtA, ecs_iter_get_var(&it, x_var));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add(world, ent, TgtB);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(TgtA, ecs_field_id(&it, 0));
        test_uint(ecs_pair(TgtA, TgtA), ecs_field_id(&it, 1));
        test_uint(ent, ecs_field_src(&it, 0));
        test_uint(ent, ecs_field_src(&it, 1));
        test_uint(TgtA, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(TgtB, ecs_field_id(&it, 0));
        test_uint(ecs_pair(TgtB, TgtB), ecs_field_id(&it, 1));
        test_uint(ent, ecs_field_src(&it, 0));
        test_uint(ent, ecs_field_src(&it, 1));
        test_uint(TgtB, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_1_this_src_w_var(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);

    ecs_entity_t e1 = ecs_new_w(world, RelA);
    ecs_entity_t e2 = ecs_new_w(world, RelA);
    ecs_entity_t e3 = ecs_new_w(world, RelA);

    ecs_query_t *q = ecs_query(world, {
        .expr = "RelA($this), $x($this)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int32_t x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(3, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(RelA, ecs_iter_get_var(&it, x_var));
        test_uint(e1, it.entities[0]);
        test_uint(e2, it.entities[1]);
        test_uint(e3, it.entities[2]);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add(world, e2, RelB);
    ecs_add(world, e3, RelB);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(RelA, ecs_iter_get_var(&it, x_var));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(2, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(RelA, ecs_iter_get_var(&it, x_var));
        test_uint(e2, it.entities[0]);
        test_uint(e3, it.entities[1]);

        test_bool(true, ecs_query_next(&it));
        test_uint(2, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(RelB, ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(RelB, ecs_iter_get_var(&it, x_var));
        test_uint(e2, it.entities[0]);
        test_uint(e3, it.entities[1]);

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_1_this_src_w_pair_rel_var(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_entity_t e1 = ecs_new_w_pair(world, RelA, TgtB);
    ecs_entity_t e2 = ecs_new_w_pair(world, RelA, TgtB);
    ecs_entity_t e3 = ecs_new_w_pair(world, RelA, TgtB);

    ecs_query_t *q = ecs_query(world, {
        .expr = "$x($this, TgtA)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int32_t x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_pair(world, e1, RelA, TgtA);
    ecs_add_pair(world, e2, RelA, TgtA);
    ecs_add_pair(world, e3, RelA, TgtA);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(3, it.count);
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 0));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(RelA, ecs_iter_get_var(&it, x_var));
        test_uint(e1, it.entities[0]);
        test_uint(e2, it.entities[1]);
        test_uint(e3, it.entities[2]);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_pair(world, e2, RelB, TgtA);
    ecs_add_pair(world, e3, RelB, TgtA);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 0));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(RelA, ecs_iter_get_var(&it, x_var));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(2, it.count);
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 0));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(RelA, ecs_iter_get_var(&it, x_var));
        test_uint(e2, it.entities[0]);
        test_uint(e3, it.entities[1]);

        test_bool(true, ecs_query_next(&it));
        test_uint(2, it.count);
        test_uint(ecs_pair(RelB, TgtA), ecs_field_id(&it, 0));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(RelB, ecs_iter_get_var(&it, x_var));
        test_uint(e2, it.entities[0]);
        test_uint(e3, it.entities[1]);

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_1_this_src_w_pair_tgt_var(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_entity_t e1 = ecs_new_w_pair(world, RelB, TgtA);
    ecs_entity_t e2 = ecs_new_w_pair(world, RelB, TgtA);
    ecs_entity_t e3 = ecs_new_w_pair(world, RelB, TgtA);

    ecs_query_t *q = ecs_query(world, {
        .expr = "RelA($this, $x)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int32_t x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_pair(world, e1, RelA, TgtA);
    ecs_add_pair(world, e2, RelA, TgtA);
    ecs_add_pair(world, e3, RelA, TgtA);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(3, it.count);
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 0));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(TgtA, ecs_iter_get_var(&it, x_var));
        test_uint(e1, it.entities[0]);
        test_uint(e2, it.entities[1]);
        test_uint(e3, it.entities[2]);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_pair(world, e2, RelA, TgtB);
    ecs_add_pair(world, e3, RelA, TgtB);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 0));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(TgtA, ecs_iter_get_var(&it, x_var));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(2, it.count);
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 0));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(TgtA, ecs_iter_get_var(&it, x_var));
        test_uint(e2, it.entities[0]);
        test_uint(e3, it.entities[1]);

        test_bool(true, ecs_query_next(&it));
        test_uint(2, it.count);
        test_uint(ecs_pair(RelA, TgtB), ecs_field_id(&it, 0));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(TgtB, ecs_iter_get_var(&it, x_var));
        test_uint(e2, it.entities[0]);
        test_uint(e3, it.entities[1]);

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_1_this_src_w_pair_rel_tgt_var(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);
    ECS_TAG(world, Tag);

    ecs_entity_t e1 = ecs_new_w(world, Tag);
    ecs_entity_t e2 = ecs_new_w(world, Tag);
    ecs_entity_t e3 = ecs_new_w(world, Tag);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Tag($this), $x($this, $y)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int32_t x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    int32_t y_var = ecs_query_find_var(q, "y");
    test_assert(y_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_pair(world, e1, RelA, TgtA);
    ecs_add_pair(world, e2, RelA, TgtA);
    ecs_add_pair(world, e3, RelA, TgtA);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(3, it.count);
        test_uint(Tag, ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(RelA, ecs_iter_get_var(&it, x_var));
        test_uint(TgtA, ecs_iter_get_var(&it, y_var));
        test_uint(e1, it.entities[0]);
        test_uint(e2, it.entities[1]);
        test_uint(e3, it.entities[2]);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_pair(world, e2, RelA, TgtB);
    ecs_add_pair(world, e3, RelA, TgtB);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(Tag, ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(RelA, ecs_iter_get_var(&it, x_var));
        test_uint(TgtA, ecs_iter_get_var(&it, y_var));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(2, it.count);
        test_uint(Tag, ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(RelA, ecs_iter_get_var(&it, x_var));
        test_uint(TgtA, ecs_iter_get_var(&it, y_var));
        test_uint(e2, it.entities[0]);
        test_uint(e3, it.entities[1]);

        test_bool(true, ecs_query_next(&it));
        test_uint(2, it.count);
        test_uint(Tag, ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelA, TgtB), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(RelA, ecs_iter_get_var(&it, x_var));
        test_uint(TgtB, ecs_iter_get_var(&it, y_var));
        test_uint(e2, it.entities[0]);
        test_uint(e3, it.entities[1]);

        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_pair(world, e3, RelB, TgtA);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(Tag, ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(RelA, ecs_iter_get_var(&it, x_var));
        test_uint(TgtA, ecs_iter_get_var(&it, y_var));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(Tag, ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(RelA, ecs_iter_get_var(&it, x_var));
        test_uint(TgtA, ecs_iter_get_var(&it, y_var));
        test_uint(e2, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(Tag, ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelA, TgtB), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(RelA, ecs_iter_get_var(&it, x_var));
        test_uint(TgtB, ecs_iter_get_var(&it, y_var));
        test_uint(e2, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(Tag, ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(RelA, ecs_iter_get_var(&it, x_var));
        test_uint(TgtA, ecs_iter_get_var(&it, y_var));
        test_uint(e3, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(Tag, ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelA, TgtB), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(RelA, ecs_iter_get_var(&it, x_var));
        test_uint(TgtB, ecs_iter_get_var(&it, y_var));
        test_uint(e3, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(Tag, ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelB, TgtA), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(RelB, ecs_iter_get_var(&it, x_var));
        test_uint(TgtA, ecs_iter_get_var(&it, y_var));
        test_uint(e3, it.entities[0]);

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_1_this_src_w_pair_rel_tgt_same_var(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_entity_t e1 = ecs_new_w_pair(world, RelB, TgtA);
    ecs_entity_t e2 = ecs_new_w_pair(world, RelB, TgtA);
    ecs_entity_t e3 = ecs_new_w_pair(world, RelB, TgtA);

    ecs_query_t *q = ecs_query(world, {
        .expr = "$x($this, $x)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int32_t x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_pair(world, e1, RelA, TgtA);
    ecs_add_pair(world, e2, RelA, TgtA);
    ecs_add_pair(world, e3, RelA, TgtA);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_pair(world, e1, TgtA, TgtA);
    ecs_add_pair(world, e2, TgtA, TgtA);
    ecs_add_pair(world, e3, TgtA, TgtA);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(3, it.count);
        test_uint(ecs_pair(TgtA, TgtA), ecs_field_id(&it, 0));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(TgtA, ecs_iter_get_var(&it, x_var));
        test_uint(e1, it.entities[0]);
        test_uint(e2, it.entities[1]);
        test_uint(e3, it.entities[2]);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_pair(world, e2, TgtB, TgtB);
    ecs_add_pair(world, e3, TgtB, TgtB);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(TgtA, TgtA), ecs_field_id(&it, 0));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(TgtA, ecs_iter_get_var(&it, x_var));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(2, it.count);
        test_uint(ecs_pair(TgtA, TgtA), ecs_field_id(&it, 0));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(TgtA, ecs_iter_get_var(&it, x_var));
        test_uint(e2, it.entities[0]);
        test_uint(e3, it.entities[1]);

        test_bool(true, ecs_query_next(&it));
        test_uint(2, it.count);
        test_uint(ecs_pair(TgtB, TgtB), ecs_field_id(&it, 0));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(TgtB, ecs_iter_get_var(&it, x_var));
        test_uint(e2, it.entities[0]);
        test_uint(e3, it.entities[1]);

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_1_this_src_w_pair_rel_tgt_same_var_after_write(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_entity_t e1 = ecs_new_w_pair(world, RelB, TgtA);
    ecs_entity_t e2 = ecs_new_w_pair(world, RelB, TgtA);
    ecs_entity_t e3 = ecs_new_w_pair(world, RelB, TgtA);

    ecs_query_t *q = ecs_query(world, {
        .expr = "$x($this), $x($this, $x)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int32_t x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_pair(world, e1, RelA, TgtA);
    ecs_add_pair(world, e2, RelA, TgtA);
    ecs_add_pair(world, e3, RelA, TgtA);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_pair(world, e1, TgtA, TgtA);
    ecs_add_pair(world, e2, TgtA, TgtA);
    ecs_add_pair(world, e3, TgtA, TgtA);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add(world, e1, TgtA);
    ecs_add(world, e2, TgtA);
    ecs_add(world, e3, TgtA);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(3, it.count);
        test_uint(TgtA, ecs_field_id(&it, 0));
        test_uint(ecs_pair(TgtA, TgtA), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(TgtA, ecs_iter_get_var(&it, x_var));
        test_uint(e1, it.entities[0]);
        test_uint(e2, it.entities[1]);
        test_uint(e3, it.entities[2]);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_pair(world, e2, TgtB, TgtB);
    ecs_add_pair(world, e3, TgtB, TgtB);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(TgtA, ecs_field_id(&it, 0));
        test_uint(ecs_pair(TgtA, TgtA), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(TgtA, ecs_iter_get_var(&it, x_var));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(2, it.count);
        test_uint(TgtA, ecs_field_id(&it, 0));
        test_uint(ecs_pair(TgtA, TgtA), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(TgtA, ecs_iter_get_var(&it, x_var));
        test_uint(e2, it.entities[0]);
        test_uint(e3, it.entities[1]);

        test_bool(false, ecs_query_next(&it));
    }

    ecs_add(world, e2, TgtB);
    ecs_add(world, e3, TgtB);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(TgtA, ecs_field_id(&it, 0));
        test_uint(ecs_pair(TgtA, TgtA), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(TgtA, ecs_iter_get_var(&it, x_var));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(2, it.count);
        test_uint(TgtA, ecs_field_id(&it, 0));
        test_uint(ecs_pair(TgtA, TgtA), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(TgtA, ecs_iter_get_var(&it, x_var));
        test_uint(e2, it.entities[0]);
        test_uint(e3, it.entities[1]);

        test_bool(true, ecs_query_next(&it));
        test_uint(2, it.count);
        test_uint(TgtB, ecs_field_id(&it, 0));
        test_uint(ecs_pair(TgtB, TgtB), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(TgtB, ecs_iter_get_var(&it, x_var));
        test_uint(e2, it.entities[0]);
        test_uint(e3, it.entities[1]);

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_1_src_id_same_var(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);

    ecs_entity_t e1 = ecs_new_w(world, TagA);
    ecs_entity_t e2 = ecs_new_w(world, TagA);
    ecs_entity_t e3 = ecs_new_w(world, TagA);
    ecs_add_id(world, e3, e1);

    ecs_query_t *q = ecs_query(world, {
        .expr = "$x($x), TagA($x)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int32_t x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_id(world, e1, e1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(e1, ecs_field_id(&it, 0));
        test_uint(TagA, ecs_field_id(&it, 1));
        test_uint(e1, ecs_field_src(&it, 0));
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(e1, ecs_iter_get_var(&it, x_var));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_id(world, e2, e2);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(e1, ecs_field_id(&it, 0));
        test_uint(TagA, ecs_field_id(&it, 1));
        test_uint(e1, ecs_field_src(&it, 0));
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(e1, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(e2, ecs_field_id(&it, 0));
        test_uint(TagA, ecs_field_id(&it, 1));
        test_uint(e2, ecs_field_src(&it, 0));
        test_uint(e2, ecs_field_src(&it, 1));
        test_uint(e2, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_1_src_pair_first_same_var(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TgtA);

    ecs_entity_t e1 = ecs_new_w(world, TagA);
    ecs_entity_t e2 = ecs_new_w(world, TagA);
    ecs_entity_t e3 = ecs_new_w(world, TagA);
    ecs_add_pair(world, e3, e1, TgtA);

    ecs_query_t *q = ecs_query(world, {
        .expr = "$x($x, TgtA), TagA($x)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int32_t x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_id(world, e1, e1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_pair(world, e1, e1, TgtA);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(e1, TgtA), ecs_field_id(&it, 0));
        test_uint(TagA, ecs_field_id(&it, 1));
        test_uint(e1, ecs_field_src(&it, 0));
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(e1, ecs_iter_get_var(&it, x_var));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_pair(world, e2, e2, TgtA);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(e1, TgtA), ecs_field_id(&it, 0));
        test_uint(TagA, ecs_field_id(&it, 1));
        test_uint(e1, ecs_field_src(&it, 0));
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(e1, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(e2, TgtA), ecs_field_id(&it, 0));
        test_uint(TagA, ecs_field_id(&it, 1));
        test_uint(e2, ecs_field_src(&it, 0));
        test_uint(e2, ecs_field_src(&it, 1));
        test_uint(e2, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_1_src_pair_second_same_var(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, RelA);

    ecs_entity_t e1 = ecs_new_w(world, TagA);
    ecs_entity_t e2 = ecs_new_w(world, TagA);
    ecs_entity_t e3 = ecs_new_w(world, TagA);
    ecs_add_pair(world, e3, RelA, e1);

    ecs_query_t *q = ecs_query(world, {
        .expr = "RelA($x, $x), TagA($x)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int32_t x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_id(world, e1, e1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_pair(world, e1, RelA, e1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(RelA, e1), ecs_field_id(&it, 0));
        test_uint(TagA, ecs_field_id(&it, 1));
        test_uint(e1, ecs_field_src(&it, 0));
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(e1, ecs_iter_get_var(&it, x_var));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_pair(world, e2, RelA, e2);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(RelA, e1), ecs_field_id(&it, 0));
        test_uint(TagA, ecs_field_id(&it, 1));
        test_uint(e1, ecs_field_src(&it, 0));
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(e1, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(RelA, e2), ecs_field_id(&it, 0));
        test_uint(TagA, ecs_field_id(&it, 1));
        test_uint(e2, ecs_field_src(&it, 0));
        test_uint(e2, ecs_field_src(&it, 1));
        test_uint(e2, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_1_src_pair_first_and_second_same_var(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, RelA);

    ecs_entity_t e1 = ecs_new_w(world, TagA);
    ecs_entity_t e2 = ecs_new_w(world, TagA);
    ecs_entity_t e3 = ecs_new_w(world, TagA);
    ecs_add_pair(world, e3, e1, e1);
    ecs_entity_t e4 = ecs_new_w(world, TagA);
    ecs_add_pair(world, e4, e4, e1);
    ecs_entity_t e5 = ecs_new_w(world, TagA);
    ecs_add_pair(world, e5, e1, e5);

    ecs_query_t *q = ecs_query(world, {
        .expr = "$x($x, $x), TagA($x)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int32_t x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_id(world, e1, e1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_pair(world, e1, RelA, e1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_pair(world, e1, e1, e1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(e1, e1), ecs_field_id(&it, 0));
        test_uint(TagA, ecs_field_id(&it, 1));
        test_uint(e1, ecs_field_src(&it, 0));
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(e1, ecs_iter_get_var(&it, x_var));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_pair(world, e2, e2, e2);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(e1, e1), ecs_field_id(&it, 0));
        test_uint(TagA, ecs_field_id(&it, 1));
        test_uint(e1, ecs_field_src(&it, 0));
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(e1, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(e2, e2), ecs_field_id(&it, 0));
        test_uint(TagA, ecs_field_id(&it, 1));
        test_uint(e2, ecs_field_src(&it, 0));
        test_uint(e2, ecs_field_src(&it, 1));
        test_uint(e2, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_1_src_id_same_var_after_write(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);

    ecs_entity_t e1 = ecs_new_w(world, TagA);
    ecs_entity_t e2 = ecs_new_w(world, TagA);
    ecs_entity_t e3 = ecs_new_w(world, TagA);
    ecs_add_id(world, e3, e1);

    ecs_query_t *q = ecs_query(world, {
        .expr = "TagA($x), $x($x)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int32_t x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_id(world, e1, e1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(TagA, ecs_field_id(&it, 0));
        test_uint(e1, ecs_field_id(&it, 1));
        test_uint(e1, ecs_field_src(&it, 0));
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(e1, ecs_iter_get_var(&it, x_var));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_id(world, e2, e2);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(TagA, ecs_field_id(&it, 0));
        test_uint(e1, ecs_field_id(&it, 1));
        test_uint(e1, ecs_field_src(&it, 0));
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(e1, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(TagA, ecs_field_id(&it, 0));
        test_uint(e2, ecs_field_id(&it, 1));
        test_uint(e2, ecs_field_src(&it, 0));
        test_uint(e2, ecs_field_src(&it, 1));
        test_uint(e2, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_1_src_pair_first_same_var_after_write(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, TagA);
    ECS_TAG(world, TgtA);

    ecs_entity_t e1 = ecs_new_w(world, TagA);
    ecs_entity_t e2 = ecs_new_w(world, TagA);
    ecs_entity_t e3 = ecs_new_w(world, TagA);
    ecs_add_pair(world, e3, e1, TgtA);

    ecs_query_t *q = ecs_query(world, {
        .expr = "TagA($x), $x($x, TgtA)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int32_t x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_id(world, e1, e1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_id(world, RelA, TgtA);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_pair(world, e1, e1, TgtA);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(TagA, ecs_field_id(&it, 0));
        test_uint(ecs_pair(e1, TgtA), ecs_field_id(&it, 1));
        test_uint(e1, ecs_field_src(&it, 0));
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(e1, ecs_iter_get_var(&it, x_var));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_pair(world, e2, e2, TgtA);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(TagA, ecs_field_id(&it, 0));
        test_uint(ecs_pair(e1, TgtA), ecs_field_id(&it, 1));
        test_uint(e1, ecs_field_src(&it, 0));
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(e1, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(TagA, ecs_field_id(&it, 0));
        test_uint(ecs_pair(e2, TgtA), ecs_field_id(&it, 1));
        test_uint(e2, ecs_field_src(&it, 0));
        test_uint(e2, ecs_field_src(&it, 1));
        test_uint(e2, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_1_src_pair_second_same_var_after_write(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, TagA);
    ECS_TAG(world, TgtA);

    ecs_entity_t e1 = ecs_new_w(world, TagA);
    ecs_entity_t e2 = ecs_new_w(world, TagA);
    ecs_entity_t e3 = ecs_new_w(world, TagA);
    ecs_add_pair(world, e3, RelA, e1);

    ecs_query_t *q = ecs_query(world, {
        .expr = "TagA($x), RelA($x, $x)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int32_t x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_id(world, e1, e1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_id(world, RelA, TgtA);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_pair(world, e1, RelA, e1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(TagA, ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelA, e1), ecs_field_id(&it, 1));
        test_uint(e1, ecs_field_src(&it, 0));
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(e1, ecs_iter_get_var(&it, x_var));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_pair(world, e2, RelA, e2);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(TagA, ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelA, e1), ecs_field_id(&it, 1));
        test_uint(e1, ecs_field_src(&it, 0));
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(e1, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(TagA, ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelA, e2), ecs_field_id(&it, 1));
        test_uint(e2, ecs_field_src(&it, 0));
        test_uint(e2, ecs_field_src(&it, 1));
        test_uint(e2, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_1_src_pair_first_and_second_same_var_after_write(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, TagA);
    ECS_TAG(world, TgtA);

    ecs_entity_t e1 = ecs_new_w(world, TagA);
    ecs_entity_t e2 = ecs_new_w(world, TagA);
    ecs_entity_t e3 = ecs_new_w(world, TagA);
    ecs_add_pair(world, e3, e1, e1);
    ecs_entity_t e4 = ecs_new_w(world, TagA);
    ecs_add_pair(world, e4, e4, e1);
    ecs_entity_t e5 = ecs_new_w(world, TagA);
    ecs_add_pair(world, e5, e1, e5);

    ecs_query_t *q = ecs_query(world, {
        .expr = "TagA($x), $x($x, $x)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int32_t x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_id(world, e1, e1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_id(world, RelA, TgtA);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_pair(world, e1, RelA, e1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_pair(world, e1, e1, e1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(TagA, ecs_field_id(&it, 0));
        test_uint(ecs_pair(e1, e1), ecs_field_id(&it, 1));
        test_uint(e1, ecs_field_src(&it, 0));
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(e1, ecs_iter_get_var(&it, x_var));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_pair(world, e2, e2, e2);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(TagA, ecs_field_id(&it, 0));
        test_uint(ecs_pair(e1, e1), ecs_field_id(&it, 1));
        test_uint(e1, ecs_field_src(&it, 0));
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(e1, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(TagA, ecs_field_id(&it, 0));
        test_uint(ecs_pair(e2, e2), ecs_field_id(&it, 1));
        test_uint(e2, ecs_field_src(&it, 0));
        test_uint(e2, ecs_field_src(&it, 1));
        test_uint(e2, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_1_src_pair_first_same_var_this(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);

    ecs_entity_t e1 = ecs_new_w(world, TagA);
    ecs_entity_t e2 = ecs_new_w(world, TagA);
    ecs_entity_t e3 = ecs_new_w(world, TagA);
    ecs_add_id(world, e3, e1);

    ecs_query_t *q = ecs_query(world, {
        .expr = "$this($this), TagA($this)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int32_t this_var = ecs_query_find_var(q, "this");
    test_assert(this_var != -1);
    test_assert(this_var != 0);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_id(world, e1, e1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(e1, ecs_field_id(&it, 0));
        test_uint(TagA, ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(e1, ecs_iter_get_var(&it, this_var));
        test_uint(e1, it.entities[0]);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_id(world, e2, e2);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(e1, ecs_field_id(&it, 0));
        test_uint(TagA, ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(e1, ecs_iter_get_var(&it, this_var));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(e2, ecs_field_id(&it, 0));
        test_uint(TagA, ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(e2, ecs_iter_get_var(&it, this_var));
        test_uint(e2, it.entities[0]);

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_1_src_pair_second_same_var_this(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TgtA);

    ecs_entity_t e1 = ecs_new_w(world, TagA);
    ecs_entity_t e2 = ecs_new_w(world, TagA);
    ecs_entity_t e3 = ecs_new_w(world, TagA);
    ecs_add_pair(world, e3, e1, TgtA);

    ecs_query_t *q = ecs_query(world, {
        .expr = "$this($this, TgtA), TagA($this)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int32_t this_var = ecs_query_find_var(q, "this");
    test_assert(this_var != -1);
    test_assert(this_var != 0);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_id(world, e1, e1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_pair(world, e1, e1, TgtA);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(e1, TgtA), ecs_field_id(&it, 0));
        test_uint(TagA, ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(e1, ecs_iter_get_var(&it, this_var));
        test_uint(e1, it.entities[0]);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_pair(world, e2, e2, TgtA);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(e1, TgtA), ecs_field_id(&it, 0));
        test_uint(TagA, ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(e1, ecs_iter_get_var(&it, this_var));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(e2, TgtA), ecs_field_id(&it, 0));
        test_uint(TagA, ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(e2, ecs_iter_get_var(&it, this_var));
        test_uint(e2, it.entities[0]);

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);

}

void Variables_1_src_pair_first_and_second_same_var_this(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, RelA);

    ecs_entity_t e1 = ecs_new_w(world, TagA);
    ecs_entity_t e2 = ecs_new_w(world, TagA);
    ecs_entity_t e3 = ecs_new_w(world, TagA);
    ecs_add_pair(world, e3, RelA, e1);

    ecs_query_t *q = ecs_query(world, {
        .expr = "RelA($this, $this), TagA($this)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int32_t this_var = ecs_query_find_var(q, "this");
    test_assert(this_var != -1);
    test_assert(this_var != 0);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_id(world, e1, e1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_pair(world, e1, RelA, e1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(RelA, e1), ecs_field_id(&it, 0));
        test_uint(TagA, ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(e1, ecs_iter_get_var(&it, this_var));
        test_uint(e1, it.entities[0]);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_pair(world, e2, RelA, e2);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(RelA, e1), ecs_field_id(&it, 0));
        test_uint(TagA, ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(e1, ecs_iter_get_var(&it, this_var));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(RelA, e2), ecs_field_id(&it, 0));
        test_uint(TagA, ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(e2, ecs_iter_get_var(&it, this_var));
        test_uint(e2, it.entities[0]);

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_1_src_id_same_var_this_after_write(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);

    ecs_entity_t e1 = ecs_new_w(world, TagA);
    ecs_entity_t e2 = ecs_new_w(world, TagA);
    ecs_entity_t e3 = ecs_new_w(world, TagA);
    ecs_add_id(world, e3, e1);

    ecs_query_t *q = ecs_query(world, {
        .expr = "TagA($this), $this($this)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int32_t this_var = ecs_query_find_var(q, "this");
    test_assert(this_var != -1);
    test_assert(this_var != 0);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_id(world, e1, e1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(TagA, ecs_field_id(&it, 0));
        test_uint(e1, ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(e1, ecs_iter_get_var(&it, this_var));
        test_uint(e1, it.entities[0]);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_id(world, e2, e2);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(TagA, ecs_field_id(&it, 0));
        test_uint(e1, ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(e1, ecs_iter_get_var(&it, this_var));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(TagA, ecs_field_id(&it, 0));
        test_uint(e2, ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(e2, ecs_iter_get_var(&it, this_var));
        test_uint(e2, it.entities[0]);

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_1_src_pair_first_same_var_this_after_write(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, TagA);
    ECS_TAG(world, TgtA);

    ecs_entity_t e1 = ecs_new_w(world, TagA);
    ecs_entity_t e2 = ecs_new_w(world, TagA);
    ecs_entity_t e3 = ecs_new_w(world, TagA);
    ecs_add_pair(world, e3, e1, TgtA);

    ecs_query_t *q = ecs_query(world, {
        .expr = "TagA($this), $this($this, TgtA)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int32_t this_var = ecs_query_find_var(q, "this");
    test_assert(this_var != -1);
    test_assert(this_var != 0);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_id(world, e1, e1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_id(world, RelA, TgtA);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_pair(world, e1, e1, TgtA);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(TagA, ecs_field_id(&it, 0));
        test_uint(ecs_pair(e1, TgtA), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(e1, ecs_iter_get_var(&it, this_var));
        test_uint(e1, it.entities[0]);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_pair(world, e2, e2, TgtA);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(TagA, ecs_field_id(&it, 0));
        test_uint(ecs_pair(e1, TgtA), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(e1, ecs_iter_get_var(&it, this_var));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(TagA, ecs_field_id(&it, 0));
        test_uint(ecs_pair(e2, TgtA), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(e2, ecs_iter_get_var(&it, this_var));
        test_uint(e2, it.entities[0]);

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_1_src_pair_second_same_var_this_after_write(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, TagA);
    ECS_TAG(world, TgtA);

    ecs_entity_t e1 = ecs_new_w(world, TagA);
    ecs_entity_t e2 = ecs_new_w(world, TagA);
    ecs_entity_t e3 = ecs_new_w(world, TagA);
    ecs_add_pair(world, e3, RelA, e1);

    ecs_query_t *q = ecs_query(world, {
        .expr = "TagA($this), RelA($this, $this)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int32_t this_var = ecs_query_find_var(q, "this");
    test_assert(this_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_id(world, e1, e1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_id(world, RelA, TgtA);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_pair(world, e1, RelA, e1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(TagA, ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelA, e1), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(e1, ecs_iter_get_var(&it, this_var));
        test_uint(e1, it.entities[0]);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_pair(world, e2, RelA, e2);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(TagA, ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelA, e1), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(e1, ecs_iter_get_var(&it, this_var));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(TagA, ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelA, e2), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(e2, ecs_iter_get_var(&it, this_var));
        test_uint(e2, it.entities[0]);

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_1_src_pair_first_and_second_same_var_this_after_write(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, TagA);
    ECS_TAG(world, TgtA);

    ecs_entity_t e1 = ecs_new_w(world, TagA);
    ecs_entity_t e2 = ecs_new_w(world, TagA);
    ecs_entity_t e3 = ecs_new_w(world, TagA);
    ecs_add_pair(world, e3, e1, e1);
    ecs_entity_t e4 = ecs_new_w(world, TagA);
    ecs_add_pair(world, e4, e4, e1);
    ecs_entity_t e5 = ecs_new_w(world, TagA);
    ecs_add_pair(world, e5, e1, e5);

    ecs_query_t *q = ecs_query(world, {
        .expr = "TagA($this), $this($this, $this)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int32_t this_var = ecs_query_find_var(q, "this");
    test_assert(this_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_id(world, e1, e1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_id(world, RelA, TgtA);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_pair(world, e1, RelA, e1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_pair(world, e1, e1, e1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(TagA, ecs_field_id(&it, 0));
        test_uint(ecs_pair(e1, e1), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(e1, ecs_iter_get_var(&it, this_var));
        test_uint(e1, it.entities[0]);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_pair(world, e2, e2, e2);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(TagA, ecs_field_id(&it, 0));
        test_uint(ecs_pair(e1, e1), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(e1, ecs_iter_get_var(&it, this_var));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(TagA, ecs_field_id(&it, 0));
        test_uint(ecs_pair(e2, e2), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(e2, ecs_iter_get_var(&it, this_var));
        test_uint(e2, it.entities[0]);

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_1_ent_src_w_this_var(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);

    ecs_entity_t ent = ecs_entity(world, { .name = "ent" });

    ecs_query_t *q = ecs_query(world, {
        .expr = "$this(ent)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int this_var = ecs_query_find_var(q, "this");
    test_assert(this_var != -1);
    test_assert(this_var != 0);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add(world, ent, RelA);
    ecs_add(world, ent, RelB);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(ent, ecs_field_src(&it, 0));
        test_uint(RelA, ecs_iter_get_var(&it, this_var));
        test_uint(RelA, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(RelB, ecs_field_id(&it, 0));
        test_uint(ent, ecs_field_src(&it, 0));
        test_uint(RelB, ecs_iter_get_var(&it, this_var));
        test_uint(RelB, it.entities[0]);

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_1_ent_src_w_pair_this_rel(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, TgtA);

    ecs_entity_t ent = ecs_entity(world, { .name = "ent" });

    ecs_query_t *q = ecs_query(world, {
        .expr = "$this(ent, TgtA)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int this_var = ecs_query_find_var(q, "this");
    test_assert(this_var != -1);
    test_assert(this_var != 0);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add(world, ent, TgtA);
    ecs_add(world, ent, TgtA);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_pair(world, ent, RelA, TgtA);
    ecs_add_pair(world, ent, RelB, TgtA);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 0));
        test_uint(ent, ecs_field_src(&it, 0));
        test_uint(RelA, ecs_iter_get_var(&it, this_var));
        test_uint(RelA, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(RelB, TgtA), ecs_field_id(&it, 0));
        test_uint(ent, ecs_field_src(&it, 0));
        test_uint(RelB, ecs_iter_get_var(&it, this_var));
        test_uint(RelB, it.entities[0]);

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_1_ent_src_w_pair_this_tgt(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_entity_t ent = ecs_entity(world, { .name = "ent" });

    ecs_query_t *q = ecs_query(world, {
        .expr = "RelA(ent, $this)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int this_var = ecs_query_find_var(q, "this");
    test_assert(this_var != -1);
    test_assert(this_var != 0);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add(world, ent, TgtA);
    ecs_add(world, ent, TgtB);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_pair(world, ent, RelA, TgtA);
    ecs_add_pair(world, ent, RelA, TgtB);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 0));
        test_uint(ent, ecs_field_src(&it, 0));
        test_uint(TgtA, ecs_iter_get_var(&it, this_var));
        test_uint(TgtA, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(RelA, TgtB), ecs_field_id(&it, 0));
        test_uint(ent, ecs_field_src(&it, 0));
        test_uint(TgtB, ecs_iter_get_var(&it, this_var));
        test_uint(TgtB, it.entities[0]);

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_1_ent_src_w_pair_this_rel_tgt(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_entity_t ent = ecs_entity(world, { .name = "ent" });

    ecs_query_t *q = ecs_query(world, {
        .expr = "$this(ent, $this)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int this_var = ecs_query_find_var(q, "this");
    test_assert(this_var != -1);
    test_assert(this_var != 0);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add(world, ent, TgtA);
    ecs_add(world, ent, TgtB);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_pair(world, ent, RelA, TgtA);
    ecs_add_pair(world, ent, RelA, TgtB);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_pair(world, ent, TgtA, TgtA);
    ecs_add_pair(world, ent, TgtB, TgtB);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(TgtA, TgtA), ecs_field_id(&it, 0));
        test_uint(ent, ecs_field_src(&it, 0));
        test_uint(TgtA, ecs_iter_get_var(&it, this_var));
        test_uint(TgtA, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(TgtB, TgtB), ecs_field_id(&it, 0));
        test_uint(ent, ecs_field_src(&it, 0));
        test_uint(TgtB, ecs_iter_get_var(&it, this_var));
        test_uint(TgtB, it.entities[0]);

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_1_this_src_w_this(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_entity_t ent = ecs_new_w(world, TagA);

    ecs_query_t *q = ecs_query(world, {
        .expr = "$this($this), TagA($this)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int this_var = ecs_query_find_var(q, "this");
    test_assert(this_var != -1);
    test_assert(this_var != 0);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add(world, ent, TagB);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }
    
    ecs_add_id(world, ent, ent);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(ent, ecs_field_id(&it, 0));
        test_uint(TagA, ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(ent, ecs_iter_get_var(&it, this_var));
        test_uint(ent, it.entities[0]);

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_1_this_src_w_pair_this_rel_tgt(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_entity_t ent = ecs_new_w(world, TagA);

    ecs_query_t *q = ecs_query(world, {
        .expr = "$this($this, $this), TagA($this)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int this_var = ecs_query_find_var(q, "this");
    test_assert(this_var != -1);
    test_assert(this_var != 0);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add(world, ent, TagB);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }
    
    ecs_add_id(world, ent, ent);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_pair(world, ent, ent, ent);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(ent, ent), ecs_field_id(&it, 0));
        test_uint(TagA, ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(ent, ecs_iter_get_var(&it, this_var));
        test_uint(ent, it.entities[0]);

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_1_this_src_w_this_after_write(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_entity_t ent = ecs_new_w(world, TagA);

    ecs_query_t *q = ecs_query(world, {
        .expr = "TagA($this), $this($this)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int this_var = ecs_query_find_var(q, "this");
    test_assert(this_var != -1);
    test_assert(this_var != 0);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add(world, ent, TagB);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }
    
    ecs_add_id(world, ent, ent);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(TagA, ecs_field_id(&it, 0));
        test_uint(ent, ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(ent, ecs_iter_get_var(&it, this_var));
        test_uint(ent, it.entities[0]);

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_1_this_src_w_pair_this_rel_tgt_after_write(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_entity_t ent = ecs_new_w(world, TagA);

    ecs_query_t *q = ecs_query(world, {
        .expr = "TagA($this), $this($this, $this)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int this_var = ecs_query_find_var(q, "this");
    test_assert(this_var != -1);
    test_assert(this_var != 0);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add(world, ent, TagB);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }
    
    ecs_add_id(world, ent, ent);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_pair(world, ent, ent, ent);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(TagA, ecs_field_id(&it, 0));
        test_uint(ecs_pair(ent, ent), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(ent, ecs_iter_get_var(&it, this_var));
        test_uint(ent, it.entities[0]);

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_2_constrain_src_from_src(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);

    ecs_entity_t e1 = ecs_new_w(world, RelA);
    ecs_entity_t e2 = ecs_new_w(world, RelA);

    ecs_query_t *q = ecs_query(world, {
        .expr = "RelA($x), RelB($x)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add(world, e1, RelB);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(RelB, ecs_field_id(&it, 1));
        test_uint(e1, ecs_field_src(&it, 0));
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(e1, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_add(world, e2, RelB);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(RelB, ecs_field_id(&it, 1));
        test_uint(e1, ecs_field_src(&it, 0));
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(e1, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(RelB, ecs_field_id(&it, 1));
        test_uint(e2, ecs_field_src(&it, 0));
        test_uint(e2, ecs_field_src(&it, 1));
        test_uint(e2, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_2_constrain_rel_from_src_w_ent(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);

    ecs_entity_t e1 = ecs_new_w(world, RelA);
    ecs_entity_t e2 = ecs_entity(world, { .name = "e2" });
    ecs_entity_t e3 = ecs_entity(world, { .name = "e3" });

    ecs_query_t *q = ecs_query(world, {
        .expr = "RelA($x), $x(e2)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_id(world, e3, e1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_id(world, e2, e1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(e1, ecs_field_id(&it, 1));
        test_uint(e1, ecs_field_src(&it, 0));
        test_uint(e2, ecs_field_src(&it, 1));
        test_uint(e1, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_2_constrain_rel_from_src_w_var(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);

    ecs_entity_t e1 = ecs_new_w(world, RelA);
    ecs_entity_t e2 = ecs_new(world);
    ecs_entity_t e3 = ecs_new(world);

    ecs_query_t *q = ecs_query(world, {
        .expr = "RelA($x), $x($y)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);
    int y_var = ecs_query_find_var(q, "y");
    test_assert(y_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_id(world, e2, e1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(e1, ecs_field_id(&it, 1));
        test_uint(e1, ecs_field_src(&it, 0));
        test_uint(e2, ecs_field_src(&it, 1));
        test_uint(e1, ecs_iter_get_var(&it, x_var));
        test_uint(e2, ecs_iter_get_var(&it, y_var));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_id(world, e3, e1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(e1, ecs_field_id(&it, 1));
        test_uint(e1, ecs_field_src(&it, 0));
        test_uint(e2, ecs_field_src(&it, 1));
        test_uint(e1, ecs_iter_get_var(&it, x_var));
        test_uint(e2, ecs_iter_get_var(&it, y_var));

        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(e1, ecs_field_id(&it, 1));
        test_uint(e1, ecs_field_src(&it, 0));
        test_uint(e3, ecs_field_src(&it, 1));
        test_uint(e1, ecs_iter_get_var(&it, x_var));
        test_uint(e3, ecs_iter_get_var(&it, y_var));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_2_constrain_rel_from_src_w_this(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);

    ecs_entity_t e1 = ecs_new_w(world, RelA);
    ecs_entity_t e2 = ecs_new(world);
    ecs_entity_t e3 = ecs_new(world);

    ecs_query_t *q = ecs_query(world, {
        .expr = "RelA($x), $x($this)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_id(world, e2, e1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(e1, ecs_field_id(&it, 1));
        test_uint(e1, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(e1, ecs_iter_get_var(&it, x_var));
        test_uint(e2, it.entities[0]);

        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_id(world, e3, e1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(2, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(e1, ecs_field_id(&it, 1));
        test_uint(e1, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(e1, ecs_iter_get_var(&it, x_var));
        test_uint(e2, it.entities[0]);
        test_uint(e3, it.entities[1]);

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_2_constrain_pair_rel_from_src_w_ent(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, TgtA);

    ecs_entity_t e1 = ecs_new_w(world, RelA);
    ecs_entity_t e2 = ecs_entity(world, { .name = "e2" });
    ecs_entity_t e3 = ecs_entity(world, { .name = "e3" });

    ecs_query_t *q = ecs_query(world, {
        .expr = "RelA($x), $x(e2, TgtA)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_id(world, e3, e1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_id(world, e2, e1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_pair(world, e3, e1, TgtA);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_pair(world, e2, e1, TgtA);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(ecs_pair(e1, TgtA), ecs_field_id(&it, 1));
        test_uint(e1, ecs_field_src(&it, 0));
        test_uint(e2, ecs_field_src(&it, 1));
        test_uint(e1, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_2_constrain_pair_rel_from_src_w_var(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, TgtA);

    ecs_entity_t e1 = ecs_new_w(world, RelA);
    ecs_entity_t e2 = ecs_new(world);
    ecs_entity_t e3 = ecs_new(world);

    ecs_query_t *q = ecs_query(world, {
        .expr = "RelA($x), $x($y, TgtA)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);
    int y_var = ecs_query_find_var(q, "y");
    test_assert(y_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_id(world, e3, e1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_id(world, e2, e1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_pair(world, e2, e1, TgtA);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(ecs_pair(e1, TgtA), ecs_field_id(&it, 1));
        test_uint(e1, ecs_field_src(&it, 0));
        test_uint(e2, ecs_field_src(&it, 1));
        test_uint(e1, ecs_iter_get_var(&it, x_var));
        test_uint(e2, ecs_iter_get_var(&it, y_var));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_pair(world, e3, e1, TgtA);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(ecs_pair(e1, TgtA), ecs_field_id(&it, 1));
        test_uint(e1, ecs_field_src(&it, 0));
        test_uint(e2, ecs_field_src(&it, 1));
        test_uint(e1, ecs_iter_get_var(&it, x_var));
        test_uint(e2, ecs_iter_get_var(&it, y_var));

        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(ecs_pair(e1, TgtA), ecs_field_id(&it, 1));
        test_uint(e1, ecs_field_src(&it, 0));
        test_uint(e3, ecs_field_src(&it, 1));
        test_uint(e1, ecs_iter_get_var(&it, x_var));
        test_uint(e3, ecs_iter_get_var(&it, y_var));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_2_constrain_pair_rel_from_src_w_this(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, TgtA);

    ecs_entity_t e1 = ecs_new_w(world, RelA);
    ecs_entity_t e2 = ecs_new(world);
    ecs_entity_t e3 = ecs_new(world);

    ecs_query_t *q = ecs_query(world, {
        .expr = "RelA($x), $x($this, TgtA)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_id(world, e3, e1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_id(world, e2, e1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_pair(world, e2, e1, TgtA);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(ecs_pair(e1, TgtA), ecs_field_id(&it, 1));
        test_uint(e1, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(e1, ecs_iter_get_var(&it, x_var));
        test_uint(e2, it.entities[0]);

        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_pair(world, e3, e1, TgtA);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(2, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(ecs_pair(e1, TgtA), ecs_field_id(&it, 1));
        test_uint(e1, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(e1, ecs_iter_get_var(&it, x_var));
        test_uint(e2, it.entities[0]);
        test_uint(e3, it.entities[1]);

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_2_constrain_pair_tgt_from_src_w_ent(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, TgtA);

    ecs_entity_t e1 = ecs_new_w(world, RelA);
    ecs_entity_t e2 = ecs_entity(world, { .name = "e2" });
    ecs_entity_t e3 = ecs_entity(world, { .name = "e3" });

    ecs_query_t *q = ecs_query(world, {
        .expr = "RelA($x), RelA(e2, $x)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_id(world, e3, e1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_id(world, e2, e1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_pair(world, e3, RelA, e1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_pair(world, e2, RelA, e1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelA, e1), ecs_field_id(&it, 1));
        test_uint(e1, ecs_field_src(&it, 0));
        test_uint(e2, ecs_field_src(&it, 1));
        test_uint(e1, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_2_constrain_pair_tgt_from_src_w_var(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, TgtA);

    ecs_entity_t e1 = ecs_new_w(world, RelA);
    ecs_entity_t e2 = ecs_new(world);
    ecs_entity_t e3 = ecs_new(world);

    ecs_query_t *q = ecs_query(world, {
        .expr = "RelA($x), RelA($y, $x)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);
    int y_var = ecs_query_find_var(q, "y");
    test_assert(y_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_id(world, e3, e1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_id(world, e2, e1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_pair(world, e2, RelA, e1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelA, e1), ecs_field_id(&it, 1));
        test_uint(e1, ecs_field_src(&it, 0));
        test_uint(e2, ecs_field_src(&it, 1));
        test_uint(e1, ecs_iter_get_var(&it, x_var));
        test_uint(e2, ecs_iter_get_var(&it, y_var));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_pair(world, e3, RelA, e1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelA, e1), ecs_field_id(&it, 1));
        test_uint(e1, ecs_field_src(&it, 0));
        test_uint(e2, ecs_field_src(&it, 1));
        test_uint(e1, ecs_iter_get_var(&it, x_var));
        test_uint(e2, ecs_iter_get_var(&it, y_var));

        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelA, e1), ecs_field_id(&it, 1));
        test_uint(e1, ecs_field_src(&it, 0));
        test_uint(e3, ecs_field_src(&it, 1));
        test_uint(e1, ecs_iter_get_var(&it, x_var));
        test_uint(e3, ecs_iter_get_var(&it, y_var));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_2_constrain_pair_tgt_from_src_w_this(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, TgtA);

    ecs_entity_t e1 = ecs_new_w(world, RelA);
    ecs_entity_t e2 = ecs_new(world);
    ecs_entity_t e3 = ecs_new(world);

    ecs_query_t *q = ecs_query(world, {
        .expr = "RelA($x), RelA($this, $x)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_id(world, e3, e1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_id(world, e2, e1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_pair(world, e2, RelA, e1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelA, e1), ecs_field_id(&it, 1));
        test_uint(e1, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(e1, ecs_iter_get_var(&it, x_var));
        test_uint(e2, it.entities[0]);

        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_pair(world, e3, RelA, e1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(2, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelA, e1), ecs_field_id(&it, 1));
        test_uint(e1, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(e1, ecs_iter_get_var(&it, x_var));
        test_uint(e2, it.entities[0]);
        test_uint(e3, it.entities[1]);

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_2_constrain_pair_rel_tgt_from_src_w_ent(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, TgtA);

    ecs_entity_t e1 = ecs_new_w(world, RelA);
    ecs_entity_t e2 = ecs_entity(world, { .name = "e2" });
    ecs_entity_t e3 = ecs_entity(world, { .name = "e3" });

    ecs_query_t *q = ecs_query(world, {
        .expr = "RelA($x), $x(e2, $x)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_id(world, e3, e1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_id(world, e2, e1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_pair(world, e3, RelA, e1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_pair(world, e2, RelA, e1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_pair(world, e2, e1, e1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(ecs_pair(e1, e1), ecs_field_id(&it, 1));
        test_uint(e1, ecs_field_src(&it, 0));
        test_uint(e2, ecs_field_src(&it, 1));
        test_uint(e1, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_2_constrain_pair_rel_tgt_from_src_w_var(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, TgtA);

    ecs_entity_t e1 = ecs_new_w(world, RelA);
    ecs_entity_t e2 = ecs_new(world);
    ecs_entity_t e3 = ecs_new(world);

    ecs_query_t *q = ecs_query(world, {
        .expr = "RelA($x), $x($y, $x)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);
    int y_var = ecs_query_find_var(q, "y");
    test_assert(y_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_id(world, e3, e1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_id(world, e2, e1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_pair(world, e2, RelA, e1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_pair(world, e2, e1, e1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(ecs_pair(e1, e1), ecs_field_id(&it, 1));
        test_uint(e1, ecs_field_src(&it, 0));
        test_uint(e2, ecs_field_src(&it, 1));
        test_uint(e1, ecs_iter_get_var(&it, x_var));
        test_uint(e2, ecs_iter_get_var(&it, y_var));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_pair(world, e3, e1, e1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(ecs_pair(e1, e1), ecs_field_id(&it, 1));
        test_uint(e1, ecs_field_src(&it, 0));
        test_uint(e2, ecs_field_src(&it, 1));
        test_uint(e1, ecs_iter_get_var(&it, x_var));
        test_uint(e2, ecs_iter_get_var(&it, y_var));

        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(ecs_pair(e1, e1), ecs_field_id(&it, 1));
        test_uint(e1, ecs_field_src(&it, 0));
        test_uint(e3, ecs_field_src(&it, 1));
        test_uint(e1, ecs_iter_get_var(&it, x_var));
        test_uint(e3, ecs_iter_get_var(&it, y_var));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_2_constrain_pair_rel_tgt_from_src_w_this(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, TgtA);

    ecs_entity_t e1 = ecs_new_w(world, RelA);
    ecs_entity_t e2 = ecs_new(world);
    ecs_entity_t e3 = ecs_new(world);

    ecs_query_t *q = ecs_query(world, {
        .expr = "RelA($x), $x($this, $x)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_id(world, e3, e1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_id(world, e2, e1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_pair(world, e2, e1, e1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(ecs_pair(e1, e1), ecs_field_id(&it, 1));
        test_uint(e1, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(e1, ecs_iter_get_var(&it, x_var));
        test_uint(e2, it.entities[0]);

        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_pair(world, e3, e1, e1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(2, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(ecs_pair(e1, e1), ecs_field_id(&it, 1));
        test_uint(e1, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(e1, ecs_iter_get_var(&it, x_var));
        test_uint(e2, it.entities[0]);
        test_uint(e3, it.entities[1]);

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_1_ent_src_set_rel_var(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, RelC);

    ecs_entity_t ent = ecs_entity(world, { .name = "ent" });
    ecs_add(world, ent, RelA);
    ecs_add(world, ent, RelB);

    ecs_query_t *q = ecs_query(world, {
        .expr = "$x(ent)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, x_var, RelA);

        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(ent, ecs_field_src(&it, 0));
        test_uint(RelA, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_1_ent_src_set_pair_rel_var(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, RelC);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_entity_t ent = ecs_entity(world, { .name = "ent" });
    ecs_add_pair(world, ent, RelA, TgtA);
    ecs_add_pair(world, ent, RelB, TgtA);
    ecs_add_pair(world, ent, RelC, TgtB);

    ecs_query_t *q = ecs_query(world, {
        .expr = "$x(ent, TgtA)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, x_var, RelA);

        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 0));
        test_uint(ent, ecs_field_src(&it, 0));
        test_uint(RelA, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_query_next(&it));
    }

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, x_var, RelB);

        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(RelB, TgtA), ecs_field_id(&it, 0));
        test_uint(ent, ecs_field_src(&it, 0));
        test_uint(RelB, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_query_next(&it));
    }

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, x_var, RelC);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_1_ent_src_set_pair_tgt_var(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);
    ECS_TAG(world, TgtC);

    ecs_entity_t ent = ecs_entity(world, { .name = "ent" });
    ecs_add_pair(world, ent, RelA, TgtA);
    ecs_add_pair(world, ent, RelA, TgtB);
    ecs_add_pair(world, ent, RelB, TgtC);

    ecs_query_t *q = ecs_query(world, {
        .expr = "RelA(ent, $x)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, x_var, TgtA);

        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 0));
        test_uint(ent, ecs_field_src(&it, 0));
        test_uint(TgtA, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_query_next(&it));
    }

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, x_var, TgtB);

        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(RelA, TgtB), ecs_field_id(&it, 0));
        test_uint(ent, ecs_field_src(&it, 0));
        test_uint(TgtB, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_query_next(&it));
    }

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, x_var, TgtC);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_1_set_src_var(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, RelC);

    ecs_entity_t e1 = ecs_new_w(world, RelA);
    ecs_entity_t e2 = ecs_new_w(world, RelB);
    ecs_entity_t e3 = ecs_new_w(world, RelC);

    ecs_query_t *q = ecs_query(world, {
        .expr = "RelA($x)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, x_var, e1);

        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(e1, ecs_field_src(&it, 0));
        test_uint(e1, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_query_next(&it));
    }

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, x_var, e2);
        test_bool(false, ecs_query_next(&it));
    }

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, x_var, e3);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_1_set_src_var_w_pair(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, RelC);
    ECS_TAG(world, TgtA);

    ecs_entity_t e1 = ecs_new_w_pair(world, RelA, TgtA);
    ecs_entity_t e2 = ecs_new_w_pair(world, RelB, TgtA);
    ecs_entity_t e3 = ecs_new_w_pair(world, RelC, TgtA);

    ecs_query_t *q = ecs_query(world, {
        .expr = "RelA($x, TgtA)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, x_var, e1);

        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 0));
        test_uint(e1, ecs_field_src(&it, 0));
        test_uint(e1, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_query_next(&it));
    }

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, x_var, e2);
        test_bool(false, ecs_query_next(&it));
    }

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, x_var, e3);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_1_set_src_var_w_pair_set_rel(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, RelC);
    ECS_TAG(world, TgtA);

    ecs_entity_t e1 = ecs_new(world);
    ecs_entity_t e2 = ecs_new(world);
    ecs_entity_t e3 = ecs_new(world);

    ecs_add_pair(world, e1, e1, TgtA);
    ecs_add_pair(world, e2, e2, TgtA);
    ecs_add_pair(world, e3, RelA, TgtA);

    ecs_query_t *q = ecs_query(world, {
        .expr = "$x($x, TgtA)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, x_var, e1);

        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(e1, TgtA), ecs_field_id(&it, 0));
        test_uint(e1, ecs_field_src(&it, 0));
        test_uint(e1, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_query_next(&it));
    }

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, x_var, e2);

        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(e2, TgtA), ecs_field_id(&it, 0));
        test_uint(e2, ecs_field_src(&it, 0));
        test_uint(e2, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_query_next(&it));
    }

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, x_var, e3);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_1_set_src_var_w_pair_set_tgt(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, TgtA);

    ecs_entity_t e1 = ecs_new(world);
    ecs_entity_t e2 = ecs_new(world);
    ecs_entity_t e3 = ecs_new(world);

    ecs_add_pair(world, e1, RelA, e1);
    ecs_add_pair(world, e2, RelA, e2);
    ecs_add_pair(world, e3, RelA, TgtA);

    ecs_query_t *q = ecs_query(world, {
        .expr = "RelA($x, $x)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, x_var, e1);

        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(RelA, e1), ecs_field_id(&it, 0));
        test_uint(e1, ecs_field_src(&it, 0));
        test_uint(e1, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_query_next(&it));
    }

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, x_var, e2);

        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(RelA, e2), ecs_field_id(&it, 0));
        test_uint(e2, ecs_field_src(&it, 0));
        test_uint(e2, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_query_next(&it));
    }

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, x_var, e3);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_1_set_src_var_w_pair_set_rel_tgt(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, TgtA);

    ecs_entity_t e1 = ecs_new(world);
    ecs_entity_t e2 = ecs_new(world);
    ecs_entity_t e3 = ecs_new(world);

    ecs_add_pair(world, e1, e1, e1);
    ecs_add_pair(world, e2, e2, e2);
    ecs_add_pair(world, e3, RelA, e3);

    ecs_query_t *q = ecs_query(world, {
        .expr = "$x($x, $x)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, x_var, e1);

        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(e1, e1), ecs_field_id(&it, 0));
        test_uint(e1, ecs_field_src(&it, 0));
        test_uint(e1, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_query_next(&it));
    }

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, x_var, e2);

        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(e2, e2), ecs_field_id(&it, 0));
        test_uint(e2, ecs_field_src(&it, 0));
        test_uint(e2, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_query_next(&it));
    }

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, x_var, e3);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_1_set_src_this(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, RelC);

    ecs_entity_t e1 = ecs_new_w(world, RelA);
    ecs_entity_t e2 = ecs_new_w(world, RelB);
    ecs_entity_t e3 = ecs_new_w(world, RelC);

    ecs_query_t *q = ecs_query(world, {
        .expr = "RelA($this)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int this_var = ecs_query_find_var(q, "this");
    test_assert(this_var == 0);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, this_var, e1);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(e1, ecs_iter_get_var(&it, this_var));
        test_uint(e1, it.entities[0]);

        test_bool(false, ecs_query_next(&it));
    }

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, this_var, e2);
        test_bool(false, ecs_query_next(&it));
    }

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, this_var, e3);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_1_set_src_this_w_pair(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, RelC);
    ECS_TAG(world, TgtA);

    ecs_entity_t e1 = ecs_new_w_pair(world, RelA, TgtA);
    ecs_entity_t e2 = ecs_new_w_pair(world, RelB, TgtA);
    ecs_entity_t e3 = ecs_new_w_pair(world, RelC, TgtA);

    ecs_query_t *q = ecs_query(world, {
        .expr = "RelA($this, TgtA)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int this_var = ecs_query_find_var(q, "this");
    test_assert(this_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, this_var, e1);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 0));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(e1, ecs_iter_get_var(&it, this_var));
        test_uint(e1, it.entities[0]);

        test_bool(false, ecs_query_next(&it));
    }

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, this_var, e2);
        test_bool(false, ecs_query_next(&it));
    }

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, this_var, e3);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_1_set_src_this_w_pair_set_rel(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, RelC);
    ECS_TAG(world, TgtA);

    ecs_entity_t e1 = ecs_new(world);
    ecs_entity_t e2 = ecs_new(world);
    ecs_entity_t e3 = ecs_new(world);

    ecs_add_pair(world, e1, e1, TgtA);
    ecs_add_pair(world, e2, e2, TgtA);
    ecs_add_pair(world, e3, RelA, TgtA);

    ecs_query_t *q = ecs_query(world, {
        .expr = "$this($this, TgtA)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int this_var = ecs_query_find_var(q, "this");
    test_assert(this_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, this_var, e1);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(e1, TgtA), ecs_field_id(&it, 0));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(e1, ecs_iter_get_var(&it, this_var));
        test_uint(e1, it.entities[0]);

        test_bool(false, ecs_query_next(&it));
    }

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, this_var, e2);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(e2, TgtA), ecs_field_id(&it, 0));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(e2, ecs_iter_get_var(&it, this_var));
        test_uint(e2, it.entities[0]);

        test_bool(false, ecs_query_next(&it));
    }

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, this_var, e3);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_1_set_src_this_w_pair_set_tgt(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, TgtA);

    ecs_entity_t e1 = ecs_new(world);
    ecs_entity_t e2 = ecs_new(world);
    ecs_entity_t e3 = ecs_new(world);

    ecs_add_pair(world, e1, RelA, e1);
    ecs_add_pair(world, e2, RelA, e2);
    ecs_add_pair(world, e3, RelA, TgtA);

    ecs_query_t *q = ecs_query(world, {
        .expr = "RelA($this, $this)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int this_var = ecs_query_find_var(q, "this");
    test_assert(this_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, this_var, e1);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(RelA, e1), ecs_field_id(&it, 0));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(e1, ecs_iter_get_var(&it, this_var));
        test_uint(e1, it.entities[0]);

        test_bool(false, ecs_query_next(&it));
    }

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, this_var, e2);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(RelA, e2), ecs_field_id(&it, 0));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(e2, ecs_iter_get_var(&it, this_var));
        test_uint(e2, it.entities[0]);

        test_bool(false, ecs_query_next(&it));
    }

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, this_var, e3);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_1_set_src_this_w_pair_set_rel_tgt(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, TgtA);

    ecs_entity_t e1 = ecs_new(world);
    ecs_entity_t e2 = ecs_new(world);
    ecs_entity_t e3 = ecs_new(world);

    ecs_add_pair(world, e1, e1, e1);
    ecs_add_pair(world, e2, e2, e2);
    ecs_add_pair(world, e3, RelA, e3);

    ecs_query_t *q = ecs_query(world, {
        .expr = "$this($this, $this)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int this_var = ecs_query_find_var(q, "this");
    test_assert(this_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, this_var, e1);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(e1, e1), ecs_field_id(&it, 0));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(e1, ecs_iter_get_var(&it, this_var));
        test_uint(e1, it.entities[0]);

        test_bool(false, ecs_query_next(&it));
    }

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, this_var, e2);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(e2, e2), ecs_field_id(&it, 0));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(e2, ecs_iter_get_var(&it, this_var));
        test_uint(e2, it.entities[0]);

        test_bool(false, ecs_query_next(&it));
    }

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, this_var, e3);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_1_set_src_this_to_empty_table(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_entity_t e1 = ecs_new_w(world, TagA);
    ecs_add(world, e1, TagB);
    ecs_table_t *t1 = ecs_get_table(world, e1);
    ecs_remove(world, e1, TagB);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ TagA }}
    });

    int this_var_id = ecs_query_find_var(q, "this");
    test_assert(this_var_id != -1);

    ecs_iter_t it = ecs_query_iter(world, q);
    ecs_iter_set_var_as_table(&it, this_var_id, t1);

    test_assert(ecs_query_next(&it));
    test_int(it.count, 0);
    test_assert(it.table == t1);
    test_uint(ecs_field_id(&it, 0), TagA);
    test_uint(ecs_field_size(&it, 0), 0);
    ecs_table_t* this_var = ecs_iter_get_var_as_table(&it, this_var_id);
    test_assert(this_var != NULL);
    test_assert(this_var == t1);

    test_assert(!ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_1_set_src_this_to_empty_table_w_component(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, TagA);

    ecs_entity_t e1 = ecs_new_w(world, Position);
    ecs_add(world, e1, TagA);
    ecs_table_t *t1 = ecs_get_table(world, e1);
    ecs_remove(world, e1, TagA);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ ecs_id(Position) }}
    });

    int this_var_id = ecs_query_find_var(q, "this");
    test_assert(this_var_id != -1);

    ecs_iter_t it = ecs_query_iter(world, q);
    ecs_iter_set_var_as_table(&it, this_var_id, t1);

    test_assert(ecs_query_next(&it));
    test_int(it.count, 0);
    test_assert(it.table == t1);
    test_uint(ecs_field_id(&it, 0), ecs_id(Position));
    test_uint(ecs_field_size(&it, 0), sizeof(Position));
    ecs_table_t* this_var = ecs_iter_get_var_as_table(&it, this_var_id);
    test_assert(this_var != NULL);
    test_assert(this_var == t1);

    test_assert(!ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_1_set_src_this_to_empty_table_w_component_self(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, TagA);

    ecs_entity_t e1 = ecs_new_w(world, Position);
    ecs_add(world, e1, TagA);
    ecs_table_t *t1 = ecs_get_table(world, e1);
    ecs_remove(world, e1, TagA);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ ecs_id(Position), .src.id = EcsSelf }},
        .cache_kind = cache_kind
    });

    int this_var_id = ecs_query_find_var(q, "this");
    test_assert(this_var_id != -1);

    ecs_iter_t it = ecs_query_iter(world, q);
    ecs_iter_set_var_as_table(&it, this_var_id, t1);

    test_assert(ecs_query_next(&it));
    test_int(it.count, 0);
    test_assert(it.table == t1);
    test_uint(ecs_field_id(&it, 0), ecs_id(Position));
    test_uint(ecs_field_size(&it, 0), sizeof(Position));
    ecs_table_t* this_var = ecs_iter_get_var_as_table(&it, this_var_id);
    test_assert(this_var != NULL);
    test_assert(this_var == t1);

    test_assert(!ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_1_set_src_this_to_entity_in_table(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_query_t *q = ecs_query(world, {
        .terms[0].id = ecs_id(Position),
        .cache_kind = cache_kind
    });

    ecs_entity_t e1 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(Position, {20, 30}));

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e1);
        test_bool(true, ecs_query_next(&it));
        test_int(it.count, 1);
        test_uint(it.entities[0], e1);
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 20);
        test_bool(false, ecs_query_next(&it));
    }

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e2);
        test_bool(true, ecs_query_next(&it));
        test_int(it.count, 1);
        test_uint(it.entities[0], e2);
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 20);
        test_int(p->y, 30);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_1_set_src_this_to_entity_in_table_self(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_query_t *q = ecs_query(world, {
        .terms[0] = { .id = ecs_id(Position), .src.id = EcsSelf },
        .cache_kind = cache_kind
    });

    ecs_entity_t e1 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(Position, {20, 30}));

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e1);
        test_bool(true, ecs_query_next(&it));
        test_int(it.count, 1);
        test_uint(it.entities[0], e1);
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 20);
        test_bool(false, ecs_query_next(&it));
    }

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e2);
        test_bool(true, ecs_query_next(&it));
        test_int(it.count, 1);
        test_uint(it.entities[0], e2);
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 20);
        test_int(p->y, 30);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_2_set_src_this(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_add_pair(world, Foo, EcsOnInstantiate, EcsInherit);
    ecs_add_pair(world, Bar, EcsOnInstantiate, EcsInherit);

    ecs_query_t *q = ecs_query(world, {
        .terms[0] = { .id = Foo },
        .terms[1] = { .id = Bar },
        .cache_kind = cache_kind
    });

    ecs_entity_t prefab = ecs_new_w(world, Bar);

    ecs_entity_t e1 = ecs_new_w(world, Foo);
    ecs_add(world, e1, Bar);

    ecs_entity_t e2 = ecs_new_w(world, Foo);
    ecs_add(world, e2, Bar);

    ecs_entity_t e3 = ecs_new_w(world, Foo);
    ecs_add_pair(world, e3, EcsIsA, prefab);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e1);
        test_bool(true, ecs_query_next(&it));
        test_int(it.count, 1);
        test_uint(it.entities[0], e1);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(Bar, ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(false, ecs_query_next(&it));
    }

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e2);
        test_bool(true, ecs_query_next(&it));
        test_int(it.count, 1);
        test_uint(it.entities[0], e2);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(Bar, ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(false, ecs_query_next(&it));
    }

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e3);
        test_bool(true, ecs_query_next(&it));
        test_int(it.count, 1);
        test_uint(it.entities[0], e3);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(Bar, ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(prefab, ecs_field_src(&it, 1));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_2_set_src_this_self(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_add_pair(world, Foo, EcsOnInstantiate, EcsInherit);
    ecs_add_pair(world, Bar, EcsOnInstantiate, EcsInherit);

    ecs_query_t *q = ecs_query(world, {
        .terms[0] = { .id = Foo, .src.id = EcsSelf },
        .terms[1] = { .id = Bar, .src.id = EcsSelf },
        .cache_kind = cache_kind
    });

    ecs_entity_t prefab = ecs_new_w(world, Bar);

    ecs_entity_t e1 = ecs_new_w(world, Foo);
    ecs_add(world, e1, Bar);

    ecs_entity_t e2 = ecs_new_w(world, Foo);
    ecs_add(world, e2, Bar);

    ecs_entity_t e3 = ecs_new_w(world, Foo);
    ecs_add_pair(world, e3, EcsIsA, prefab);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e1);
        test_bool(true, ecs_query_next(&it));
        test_int(it.count, 1);
        test_uint(it.entities[0], e1);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(Bar, ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(false, ecs_query_next(&it));
    }

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e2);
        test_bool(true, ecs_query_next(&it));
        test_int(it.count, 1);
        test_uint(it.entities[0], e2);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(Bar, ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(false, ecs_query_next(&it));
    }

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e3);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_2_set_src_this_component(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    ecs_add_pair(world, ecs_id(Velocity), EcsOnInstantiate, EcsInherit);

    ecs_query_t *q = ecs_query(world, {
        .terms[0] = { .id = ecs_id(Position) },
        .terms[1] = { .id = ecs_id(Velocity) },
        .cache_kind = cache_kind
    });

    ecs_entity_t prefab = ecs_insert(world, ecs_value(Velocity, {3, 4}));

    ecs_entity_t e1 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_set(world, e1, Velocity, {1, 2});

    ecs_entity_t e2 = ecs_insert(world, ecs_value(Position, {20, 30}));
    ecs_set(world, e2, Velocity, {2, 3});

    ecs_entity_t e3 = ecs_insert(world, ecs_value(Position, {30, 40}));
    ecs_add_pair(world, e3, EcsIsA, prefab);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e1);
        test_bool(true, ecs_query_next(&it));
        test_int(it.count, 1);
        test_uint(it.entities[0], e1);
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 20);
        Velocity *v = ecs_field(&it, Velocity, 1);
        test_assert(v != NULL);
        test_int(v->x, 1);
        test_int(v->y, 2);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(ecs_id(Velocity), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(false, ecs_query_next(&it));
    }

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e2);
        test_bool(true, ecs_query_next(&it));
        test_int(it.count, 1);
        test_uint(it.entities[0], e2);
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 20);
        test_int(p->y, 30);
        Velocity *v = ecs_field(&it, Velocity, 1);
        test_assert(v != NULL);
        test_int(v->x, 2);
        test_int(v->y, 3);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(ecs_id(Velocity), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(false, ecs_query_next(&it));
    }

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e3);
        test_bool(true, ecs_query_next(&it));
        test_int(it.count, 1);
        test_uint(it.entities[0], e3);
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 30);
        test_int(p->y, 40);
        Velocity *v = ecs_field(&it, Velocity, 1);
        test_assert(v != NULL);
        test_int(v->x, 3);
        test_int(v->y, 4);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(ecs_id(Velocity), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(prefab, ecs_field_src(&it, 1));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_2_set_src_this_self_component(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    ecs_add_pair(world, ecs_id(Velocity), EcsOnInstantiate, EcsInherit);

    ecs_query_t *q = ecs_query(world, {
        .terms[0] = { .id = ecs_id(Position), .src.id = EcsSelf },
        .terms[1] = { .id = ecs_id(Velocity), .src.id = EcsSelf },
        .cache_kind = cache_kind
    });

    ecs_entity_t prefab = ecs_insert(world, ecs_value(Velocity, {4, 5}));

    ecs_entity_t e1 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_set(world, e1, Velocity, {1, 2});

    ecs_entity_t e2 = ecs_insert(world, ecs_value(Position, {20, 30}));
    ecs_set(world, e2, Velocity, {2, 3});

    ecs_entity_t e3 = ecs_insert(world, ecs_value(Position, {40, 50}));
    ecs_add_pair(world, e3, EcsIsA, prefab);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e1);
        test_bool(true, ecs_query_next(&it));
        test_int(it.count, 1);
        test_uint(it.entities[0], e1);
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 20);
        Velocity *v = ecs_field(&it, Velocity, 1);
        test_assert(v != NULL);
        test_int(v->x, 1);
        test_int(v->y, 2);
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(false, ecs_query_next(&it));
    }

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e2);
        test_bool(true, ecs_query_next(&it));
        test_int(it.count, 1);
        test_uint(it.entities[0], e2);
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 20);
        test_int(p->y, 30);
        Velocity *v = ecs_field(&it, Velocity, 1);
        test_assert(v != NULL);
        test_int(v->x, 2);
        test_int(v->y, 3);
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(false, ecs_query_next(&it));
    }

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e3);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_2_set_src_this_w_up(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);
    ECS_TAG(world, Hello);

    ecs_add_pair(world, Foo, EcsOnInstantiate, EcsInherit);
    ecs_add_pair(world, Bar, EcsOnInstantiate, EcsInherit);
    ecs_add_pair(world, Hello, EcsOnInstantiate, EcsInherit);

    ecs_query_t *q = ecs_query(world, {
        .terms[0] = { .id = Foo },
        .terms[1] = { .id = Bar },
        .terms[2] = { .id = Hello, .src.id = EcsUp, .trav = EcsChildOf },
        .cache_kind = cache_kind
    });

    ecs_entity_t prefab = ecs_new_w(world, Bar);
    ecs_entity_t parent = ecs_new_w(world, Hello);

    ecs_entity_t e1 = ecs_new_w(world, Foo);
    ecs_add(world, e1, Bar);
    ecs_add_pair(world, e1, EcsChildOf, parent);

    ecs_entity_t e2 = ecs_new_w(world, Foo);
    ecs_add(world, e2, Bar);
    ecs_add_pair(world, e2, EcsChildOf, parent);

    ecs_entity_t e3 = ecs_new_w(world, Foo);
    ecs_add_pair(world, e3, EcsIsA, prefab);
    ecs_add_pair(world, e3, EcsChildOf, parent);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e1);
        test_bool(true, ecs_query_next(&it));
        test_int(it.count, 1);
        test_uint(it.entities[0], e1);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(Bar, ecs_field_id(&it, 1));
        test_uint(Hello, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(parent, ecs_field_src(&it, 2));
        test_bool(false, ecs_query_next(&it));
    }

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e2);
        test_bool(true, ecs_query_next(&it));
        test_int(it.count, 1);
        test_uint(it.entities[0], e2);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(Bar, ecs_field_id(&it, 1));
        test_uint(Hello, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(parent, ecs_field_src(&it, 2));
        test_bool(false, ecs_query_next(&it));
    }

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e3);
        test_bool(true, ecs_query_next(&it));
        test_int(it.count, 1);
        test_uint(it.entities[0], e3);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(Bar, ecs_field_id(&it, 1));
        test_uint(Hello, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(prefab, ecs_field_src(&it, 1));
        test_uint(parent, ecs_field_src(&it, 2));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_2_set_src_this_self_w_up(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);
    ECS_TAG(world, Hello);

    ecs_add_pair(world, Foo, EcsOnInstantiate, EcsInherit);
    ecs_add_pair(world, Bar, EcsOnInstantiate, EcsInherit);
    ecs_add_pair(world, Hello, EcsOnInstantiate, EcsInherit);

    ecs_query_t *q = ecs_query(world, {
        .terms[0] = { .id = Foo, .src.id = EcsSelf },
        .terms[1] = { .id = Bar, .src.id = EcsSelf },
        .terms[2] = { .id = Hello, .src.id = EcsUp, .trav = EcsChildOf },
        .cache_kind = cache_kind
    });

    ecs_entity_t prefab = ecs_new_w(world, Bar);
    ecs_entity_t parent = ecs_new_w(world, Hello);

    ecs_entity_t e1 = ecs_new_w(world, Foo);
    ecs_add(world, e1, Bar);
    ecs_add_pair(world, e1, EcsChildOf, parent);

    ecs_entity_t e2 = ecs_new_w(world, Foo);
    ecs_add(world, e2, Bar);
    ecs_add_pair(world, e2, EcsChildOf, parent);

    ecs_entity_t e3 = ecs_new_w(world, Foo);
    ecs_add_pair(world, e3, EcsIsA, prefab);
    ecs_add_pair(world, e3, EcsChildOf, parent);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e1);
        test_bool(true, ecs_query_next(&it));
        test_int(it.count, 1);
        test_uint(it.entities[0], e1);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(Bar, ecs_field_id(&it, 1));
        test_uint(Hello, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(parent, ecs_field_src(&it, 2));
        test_bool(false, ecs_query_next(&it));
    }

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e2);
        test_bool(true, ecs_query_next(&it));
        test_int(it.count, 1);
        test_uint(it.entities[0], e2);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(Bar, ecs_field_id(&it, 1));
        test_uint(Hello, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(parent, ecs_field_src(&it, 2));
        test_bool(false, ecs_query_next(&it));
    }

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e3);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_2_set_src_this_component_w_up(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);

    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    ecs_add_pair(world, ecs_id(Velocity), EcsOnInstantiate, EcsInherit);
    ecs_add_pair(world, ecs_id(Mass), EcsOnInstantiate, EcsInherit);

    ecs_query_t *q = ecs_query(world, {
        .terms[0] = { .id = ecs_id(Position) },
        .terms[1] = { .id = ecs_id(Velocity) },
        .terms[2] = { .id = ecs_id(Mass), .src.id = EcsUp, .trav = EcsChildOf },
        .cache_kind = cache_kind
    });

    ecs_entity_t prefab = ecs_insert(world, ecs_value(Velocity, {3, 4}));
    ecs_entity_t parent = ecs_insert(world, ecs_value(Mass, {60}));

    ecs_entity_t e1 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_set(world, e1, Velocity, {1, 2});
    ecs_add_pair(world, e1, EcsChildOf, parent);

    ecs_entity_t e2 = ecs_insert(world, ecs_value(Position, {20, 30}));
    ecs_set(world, e2, Velocity, {2, 3});
    ecs_add_pair(world, e2, EcsChildOf, parent);

    ecs_entity_t e3 = ecs_insert(world, ecs_value(Position, {30, 40}));
    ecs_add_pair(world, e3, EcsIsA, prefab);
    ecs_add_pair(world, e3, EcsChildOf, parent);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e1);
        test_bool(true, ecs_query_next(&it));
        test_int(it.count, 1);
        test_uint(it.entities[0], e1);
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 20);
        Velocity *v = ecs_field(&it, Velocity, 1);
        test_assert(v != NULL);
        test_int(v->x, 1);
        test_int(v->y, 2);
        Mass *m = ecs_field(&it, Mass, 2);
        test_assert(m != NULL);
        test_int(m[0], 60);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(ecs_id(Velocity), ecs_field_id(&it, 1));
        test_uint(ecs_id(Mass), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(parent, ecs_field_src(&it, 2));
        test_bool(false, ecs_query_next(&it));
    }

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e2);
        test_bool(true, ecs_query_next(&it));
        test_int(it.count, 1);
        test_uint(it.entities[0], e2);
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 20);
        test_int(p->y, 30);
        Velocity *v = ecs_field(&it, Velocity, 1);
        test_assert(v != NULL);
        test_int(v->x, 2);
        test_int(v->y, 3);
        Mass *m = ecs_field(&it, Mass, 2);
        test_assert(m != NULL);
        test_int(m[0], 60);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(ecs_id(Velocity), ecs_field_id(&it, 1));
        test_uint(ecs_id(Mass), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(parent, ecs_field_src(&it, 2));
        test_bool(false, ecs_query_next(&it));
    }

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e3);
        test_bool(true, ecs_query_next(&it));
        test_int(it.count, 1);
        test_uint(it.entities[0], e3);
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 30);
        test_int(p->y, 40);
        Velocity *v = ecs_field(&it, Velocity, 1);
        test_assert(v != NULL);
        test_int(v->x, 3);
        test_int(v->y, 4);
        Mass *m = ecs_field(&it, Mass, 2);
        test_assert(m != NULL);
        test_int(m[0], 60);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(ecs_id(Velocity), ecs_field_id(&it, 1));
        test_uint(ecs_id(Mass), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(prefab, ecs_field_src(&it, 1));
        test_uint(parent, ecs_field_src(&it, 2));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_2_set_src_this_self_component_w_up(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);

    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    ecs_add_pair(world, ecs_id(Velocity), EcsOnInstantiate, EcsInherit);
    ecs_add_pair(world, ecs_id(Mass), EcsOnInstantiate, EcsInherit);

    ecs_query_t *q = ecs_query(world, {
        .terms[0] = { .id = ecs_id(Position), .src.id = EcsSelf },
        .terms[1] = { .id = ecs_id(Velocity), .src.id = EcsSelf },
        .terms[2] = { .id = ecs_id(Mass), .src.id = EcsUp, .trav = EcsChildOf },
        .cache_kind = cache_kind
    });

    ecs_entity_t prefab = ecs_insert(world, ecs_value(Velocity, {3, 4}));
    ecs_entity_t parent = ecs_insert(world, ecs_value(Mass, {60}));

    ecs_entity_t e1 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_set(world, e1, Velocity, {1, 2});
    ecs_add_pair(world, e1, EcsChildOf, parent);

    ecs_entity_t e2 = ecs_insert(world, ecs_value(Position, {20, 30}));
    ecs_set(world, e2, Velocity, {2, 3});
    ecs_add_pair(world, e2, EcsChildOf, parent);

    ecs_entity_t e3 = ecs_insert(world, ecs_value(Position, {30, 40}));
    ecs_add_pair(world, e3, EcsIsA, prefab);
    ecs_add_pair(world, e3, EcsChildOf, parent);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e1);
        test_bool(true, ecs_query_next(&it));
        test_int(it.count, 1);
        test_uint(it.entities[0], e1);
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 20);
        Velocity *v = ecs_field(&it, Velocity, 1);
        test_assert(v != NULL);
        test_int(v->x, 1);
        test_int(v->y, 2);
        Mass *m = ecs_field(&it, Mass, 2);
        test_assert(m != NULL);
        test_int(m[0], 60);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(ecs_id(Velocity), ecs_field_id(&it, 1));
        test_uint(ecs_id(Mass), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(parent, ecs_field_src(&it, 2));
        test_bool(false, ecs_query_next(&it));
    }

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e2);
        test_bool(true, ecs_query_next(&it));
        test_int(it.count, 1);
        test_uint(it.entities[0], e2);
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 20);
        test_int(p->y, 30);
        Velocity *v = ecs_field(&it, Velocity, 1);
        test_assert(v != NULL);
        test_int(v->x, 2);
        test_int(v->y, 3);
        Mass *m = ecs_field(&it, Mass, 2);
        test_assert(m != NULL);
        test_int(m[0], 60);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(ecs_id(Velocity), ecs_field_id(&it, 1));
        test_uint(ecs_id(Mass), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(parent, ecs_field_src(&it, 2));
        test_bool(false, ecs_query_next(&it));
    }

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e3);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_2_set_src_this_w_exclusive_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);
    ECS_TAG(world, Hello);

    ecs_add_pair(world, Foo, EcsOnInstantiate, EcsInherit);
    ecs_add_pair(world, Bar, EcsOnInstantiate, EcsInherit);
    ecs_add_pair(world, Hello, EcsOnInstantiate, EcsInherit);

    ecs_query_t *q = ecs_query(world, {
        .terms[0] = { .id = Foo },
        .terms[1] = { .id = Bar },
        .terms[2] = { .id = ecs_pair(EcsChildOf, EcsWildcard) },
        .cache_kind = cache_kind
    });

    ecs_entity_t prefab = ecs_new_w(world, Bar);
    ecs_entity_t parent = ecs_new_w(world, Hello);

    ecs_entity_t e1 = ecs_new_w(world, Foo);
    ecs_add(world, e1, Bar);
    ecs_add_pair(world, e1, EcsChildOf, parent);

    ecs_entity_t e2 = ecs_new_w(world, Foo);
    ecs_add(world, e2, Bar);
    ecs_add_pair(world, e2, EcsChildOf, parent);

    ecs_entity_t e3 = ecs_new_w(world, Foo);
    ecs_add_pair(world, e3, EcsIsA, prefab);
    ecs_add_pair(world, e3, EcsChildOf, parent);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e1);
        test_bool(true, ecs_query_next(&it));
        test_int(it.count, 1);
        test_uint(it.entities[0], e1);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(Bar, ecs_field_id(&it, 1));
        test_uint(ecs_pair(EcsChildOf, parent), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(false, ecs_query_next(&it));
    }

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e2);
        test_bool(true, ecs_query_next(&it));
        test_int(it.count, 1);
        test_uint(it.entities[0], e2);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(Bar, ecs_field_id(&it, 1));
        test_uint(ecs_pair(EcsChildOf, parent), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(false, ecs_query_next(&it));
    }

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e3);
        test_bool(true, ecs_query_next(&it));
        test_int(it.count, 1);
        test_uint(it.entities[0], e3);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(Bar, ecs_field_id(&it, 1));
        test_uint(ecs_pair(EcsChildOf, parent), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(prefab, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_2_set_src_this_self_w_exclusive_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);
    ECS_TAG(world, Hello);

    ecs_query_t *q = ecs_query(world, {
        .terms[0] = { .id = Foo, .src.id = EcsSelf },
        .terms[1] = { .id = ecs_pair(EcsChildOf, EcsWildcard) },
        .cache_kind = cache_kind
    });

    ecs_entity_t parent_1 = ecs_new_w(world, Hello);
    ecs_entity_t parent_2 = ecs_new_w(world, Hello);

    ecs_entity_t e1 = ecs_new_w(world, Foo);
    ecs_add_pair(world, e1, EcsChildOf, parent_1);

    ecs_entity_t e2 = ecs_new_w(world, Foo);
    ecs_add_pair(world, e2, EcsChildOf, parent_2);

    ecs_entity_t e3 = ecs_new_w(world, Foo);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e1);
        test_bool(true, ecs_query_next(&it));
        test_int(it.count, 1);
        test_uint(it.entities[0], e1);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_pair(EcsChildOf, parent_1), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(false, ecs_query_next(&it));
    }

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e2);
        test_bool(true, ecs_query_next(&it));
        test_int(it.count, 1);
        test_uint(it.entities[0], e2);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_pair(EcsChildOf, parent_2), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(false, ecs_query_next(&it));
    }

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e3);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_2_set_src_this_self_w_exclusive_wildcard_w_up(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);
    ECS_TAG(world, Hello);

    ecs_add_pair(world, Foo, EcsOnInstantiate, EcsInherit);
    ecs_add_pair(world, Bar, EcsOnInstantiate, EcsInherit);
    ecs_add_pair(world, Hello, EcsOnInstantiate, EcsInherit);

    ecs_query_t *q = ecs_query(world, {
        .terms[0] = { .id = Foo, .src.id = EcsSelf },
        .terms[1] = { .id = ecs_pair(EcsChildOf, EcsWildcard), .src.id = EcsSelf },
        .terms[2] = { .id = Bar, .src.id = EcsUp, .trav = EcsIsA },
        .cache_kind = cache_kind
    });

    ecs_entity_t prefab = ecs_new_w(world, Bar);
    ecs_entity_t parent = ecs_new_w(world, Hello);

    ecs_entity_t e1 = ecs_new_w(world, Foo);
    ecs_add_pair(world, e1, EcsIsA, prefab);
    ecs_add_pair(world, e1, EcsChildOf, parent);

    ecs_entity_t e2 = ecs_new_w(world, Foo);
    ecs_add_pair(world, e2, EcsIsA, prefab);
    ecs_add_pair(world, e2, EcsChildOf, parent);

    ecs_entity_t e3 = ecs_new_w(world, Foo);
    ecs_add(world, e3, Bar);
    ecs_add_pair(world, e3, EcsChildOf, parent);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e1);
        test_bool(true, ecs_query_next(&it));
        test_int(it.count, 1);
        test_uint(it.entities[0], e1);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_pair(EcsChildOf, parent), ecs_field_id(&it, 1));
        test_uint(Bar, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(prefab, ecs_field_src(&it, 2));
        test_bool(false, ecs_query_next(&it));
    }

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e2);
        test_bool(true, ecs_query_next(&it));
        test_int(it.count, 1);
        test_uint(it.entities[0], e2);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_pair(EcsChildOf, parent), ecs_field_id(&it, 1));
        test_uint(Bar, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(prefab, ecs_field_src(&it, 2));
        test_bool(false, ecs_query_next(&it));
    }

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e3);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_1_set_src_this_is_true(void) {
    ecs_world_t* ecs = ecs_mini();

    ECS_COMPONENT(ecs, Position);
    ECS_COMPONENT(ecs, Velocity);

    ecs_query_t *q = ecs_query(ecs, {
        .terms = {
            { .id = ecs_id(Position) }
        },
        .cache_kind = cache_kind
    });
    test_assert(q != NULL);

    /* ecs_entity_t e1 = */ ecs_insert(ecs, ecs_value(Position, {10, 20}));
    ecs_entity_t e2 = ecs_insert(ecs, ecs_value(Position, {20, 30}));
    /* ecs_entity_t e3 = */ ecs_insert(ecs, ecs_value(Position, {30, 40}));
    ecs_entity_t e4 = ecs_insert(ecs, ecs_value(Velocity, {20, 30}));

    ecs_iter_t it = ecs_query_iter(ecs, q);
    ecs_iter_set_var(&it, 0, e2);
    test_bool(true, ecs_iter_is_true(&it));

    it = ecs_query_iter(ecs, q);
    ecs_iter_set_var(&it, 0, e4);
    test_bool(false, ecs_iter_is_true(&it));

    ecs_query_fini(q);

    ecs_fini(ecs);
}

void Variables_2_set_src_this_w_wildcard(void) {
    ecs_world_t* ecs = ecs_mini();

    ECS_COMPONENT(ecs, Position);
    ECS_TAG(ecs, Likes);
    ECS_TAG(ecs, Apples);
    ECS_TAG(ecs, Pears);

    ecs_query_t *q = ecs_query(ecs, {
        .terms = {
            { .id = ecs_id(Position) },
            { .id = ecs_pair(Likes, EcsWildcard) }
        },
        .cache_kind = cache_kind
    });
    test_assert(q != NULL);

    ecs_entity_t e1 = ecs_insert(ecs, ecs_value(Position, {10, 20}));
    ecs_entity_t e2 = ecs_insert(ecs, ecs_value(Position, {20, 30}));
    ecs_entity_t e3 = ecs_insert(ecs, ecs_value(Position, {30, 40}));

    ecs_add_pair(ecs, e1, Likes, Apples);
    ecs_add_pair(ecs, e2, Likes, Apples);
    ecs_add_pair(ecs, e3, Likes, Apples);

    ecs_add_pair(ecs, e1, Likes, Pears);
    ecs_add_pair(ecs, e2, Likes, Pears);
    ecs_add_pair(ecs, e3, Likes, Pears);

    ecs_iter_t it = ecs_query_iter(ecs, q);
    ecs_iter_set_var(&it, 0, e2);

    {
        test_bool(true, ecs_query_next(&it));
        test_int(it.count, 1);
        test_uint(it.entities[0], e2);
        Position *p = ecs_field(&it, Position, 0);
        test_int(p[0].x, 20);
        test_int(p[0].y, 30);
        test_uint(ecs_pair(Likes, Apples), ecs_field_id(&it, 1));
    }
    {
        test_bool(true, ecs_query_next(&it));
        test_int(it.count, 1);
        test_uint(it.entities[0], e2);
        Position *p = ecs_field(&it, Position, 0);
        test_int(p[0].x, 20);
        test_int(p[0].y, 30);
        test_uint(ecs_pair(Likes, Pears), ecs_field_id(&it, 1));
    }

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(ecs);
}

void Variables_3_set_src_this_w_uncacheable_tag_tag_tag(void) {
    ecs_world_t* ecs = ecs_mini();

    ECS_TAG(ecs, TagA);
    ECS_TAG(ecs, TagB);

    ecs_entity_t p1 = ecs_new(ecs);
    ecs_entity_t p2 = ecs_new(ecs);
    ecs_entity_t e1 = ecs_new_w(ecs, TagA);
    ecs_entity_t e2 = ecs_new_w(ecs, TagA);
    ecs_entity_t e3 = ecs_new_w(ecs, TagA);
    ecs_add(ecs, e1, TagB);
    ecs_add(ecs, e2, TagB);
    ecs_add(ecs, e3, TagB);
    ecs_add_pair(ecs, e1, EcsChildOf, p1);
    ecs_add_pair(ecs, e2, EcsChildOf, p1);
    ecs_add_pair(ecs, e3, EcsChildOf, p2);

    ecs_query_t *q = ecs_query(ecs, {
        .terms = {
            { .first.id = EcsChildOf, .second.name = "$parent" },
            { TagA },
            { TagB }
        },
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int32_t parent_var = ecs_query_find_var(q, "parent");
    test_assert(parent_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(ecs, q);
        ecs_iter_set_var(&it, 0, e1);
        test_bool(true, ecs_query_next(&it));
        test_int(it.count, 1);
        test_uint(it.entities[0], e1);
        test_uint(ecs_childof(p1), ecs_field_id(&it, 0));
        test_uint(TagA, ecs_field_id(&it, 1));
        test_uint(TagB, ecs_field_id(&it, 2));
        test_uint(p1, ecs_iter_get_var(&it, parent_var));
        test_bool(false, ecs_query_next(&it));
    }

    {
        ecs_iter_t it = ecs_query_iter(ecs, q);
        ecs_iter_set_var(&it, 0, e2);
        test_bool(true, ecs_query_next(&it));
        test_int(it.count, 1);
        test_uint(it.entities[0], e2);
        test_uint(ecs_childof(p1), ecs_field_id(&it, 0));
        test_uint(TagA, ecs_field_id(&it, 1));
        test_uint(TagB, ecs_field_id(&it, 2));
        test_uint(p1, ecs_iter_get_var(&it, parent_var));
        test_bool(false, ecs_query_next(&it));
    }

    {
        ecs_iter_t it = ecs_query_iter(ecs, q);
        ecs_iter_set_var(&it, 0, e3);
        test_bool(true, ecs_query_next(&it));
        test_int(it.count, 1);
        test_uint(it.entities[0], e3);
        test_uint(ecs_childof(p2), ecs_field_id(&it, 0));
        test_uint(TagA, ecs_field_id(&it, 1));
        test_uint(TagB, ecs_field_id(&it, 2));
        test_uint(p2, ecs_iter_get_var(&it, parent_var));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(ecs);
}

void Variables_3_set_src_this_w_uncacheable_tag_component_tag(void) {
    ecs_world_t* ecs = ecs_mini();

    ECS_COMPONENT(ecs, Position);
    ECS_TAG(ecs, TagB);

    ecs_entity_t p1 = ecs_new(ecs);
    ecs_entity_t p2 = ecs_new(ecs);
    ecs_entity_t e1 = ecs_insert(ecs, ecs_value(Position, {11, 21}));
    ecs_entity_t e2 = ecs_insert(ecs, ecs_value(Position, {12, 22}));
    ecs_entity_t e3 = ecs_insert(ecs, ecs_value(Position, {13, 23}));
    ecs_add(ecs, e1, TagB);
    ecs_add(ecs, e2, TagB);
    ecs_add(ecs, e3, TagB);
    ecs_add_pair(ecs, e1, EcsChildOf, p1);
    ecs_add_pair(ecs, e2, EcsChildOf, p1);
    ecs_add_pair(ecs, e3, EcsChildOf, p2);

    ecs_query_t *q = ecs_query(ecs, {
        .terms = {
            { .first.id = EcsChildOf, .second.name = "$parent" },
            { ecs_id(Position) },
            { TagB }
        },
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int32_t parent_var = ecs_query_find_var(q, "parent");
    test_assert(parent_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(ecs, q);
        ecs_iter_set_var(&it, 0, e1);
        test_bool(true, ecs_query_next(&it));
        test_int(it.count, 1);
        test_uint(it.entities[0], e1);
        test_uint(ecs_childof(p1), ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_uint(TagB, ecs_field_id(&it, 2));
        test_uint(p1, ecs_iter_get_var(&it, parent_var));
        {
            Position *p = ecs_field(&it, Position, 1);
            test_assert(p != NULL);
            test_int(p->x, 11);
            test_int(p->y, 21);
        }
        test_bool(false, ecs_query_next(&it));
    }

    {
        ecs_iter_t it = ecs_query_iter(ecs, q);
        ecs_iter_set_var(&it, 0, e2);
        test_bool(true, ecs_query_next(&it));
        test_int(it.count, 1);
        test_uint(it.entities[0], e2);
        test_uint(ecs_childof(p1), ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_uint(TagB, ecs_field_id(&it, 2));
        test_uint(p1, ecs_iter_get_var(&it, parent_var));
        {
            Position *p = ecs_field(&it, Position, 1);
            test_assert(p != NULL);
            test_int(p->x, 12);
            test_int(p->y, 22);
        }
        test_bool(false, ecs_query_next(&it));
    }

    {
        ecs_iter_t it = ecs_query_iter(ecs, q);
        ecs_iter_set_var(&it, 0, e3);
        test_bool(true, ecs_query_next(&it));
        test_int(it.count, 1);
        test_uint(it.entities[0], e3);
        test_uint(ecs_childof(p2), ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_uint(TagB, ecs_field_id(&it, 2));
        test_uint(p2, ecs_iter_get_var(&it, parent_var));
        {
            Position *p = ecs_field(&it, Position, 1);
            test_assert(p != NULL);
            test_int(p->x, 13);
            test_int(p->y, 23);
        }
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(ecs);
}

void Variables_3_set_src_this_w_uncacheable_tag_component_component(void) {
    ecs_world_t* ecs = ecs_mini();

    ECS_COMPONENT(ecs, Position);
    ECS_COMPONENT(ecs, Velocity);

    ecs_entity_t p1 = ecs_new(ecs);
    ecs_entity_t p2 = ecs_new(ecs);
    ecs_entity_t e1 = ecs_insert(ecs, ecs_value(Position, {11, 21}));
    ecs_entity_t e2 = ecs_insert(ecs, ecs_value(Position, {12, 22}));
    ecs_entity_t e3 = ecs_insert(ecs, ecs_value(Position, {13, 23}));
    ecs_set(ecs, e1, Velocity, {1, 1});
    ecs_set(ecs, e2, Velocity, {2, 2});
    ecs_set(ecs, e3, Velocity, {3, 3});
    ecs_add_pair(ecs, e1, EcsChildOf, p1);
    ecs_add_pair(ecs, e2, EcsChildOf, p1);
    ecs_add_pair(ecs, e3, EcsChildOf, p2);

    ecs_query_t *q = ecs_query(ecs, {
        .terms = {
            { .first.id = EcsChildOf, .second.name = "$parent" },
            { ecs_id(Position) },
            { ecs_id(Velocity) }
        },
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int32_t parent_var = ecs_query_find_var(q, "parent");
    test_assert(parent_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(ecs, q);
        ecs_iter_set_var(&it, 0, e1);
        test_bool(true, ecs_query_next(&it));
        test_int(it.count, 1);
        test_uint(it.entities[0], e1);
        test_uint(ecs_childof(p1), ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_uint(ecs_id(Velocity), ecs_field_id(&it, 2));
        test_uint(p1, ecs_iter_get_var(&it, parent_var));
        {
            Position *p = ecs_field(&it, Position, 1);
            test_assert(p != NULL);
            test_int(p->x, 11);
            test_int(p->y, 21);
        }
        {
            Velocity *v = ecs_field(&it, Velocity, 2);
            test_assert(v != NULL);
            test_int(v->x, 1);
            test_int(v->y, 1);
        }
        test_bool(false, ecs_query_next(&it));
    }

    {
        ecs_iter_t it = ecs_query_iter(ecs, q);
        ecs_iter_set_var(&it, 0, e2);
        test_bool(true, ecs_query_next(&it));
        test_int(it.count, 1);
        test_uint(it.entities[0], e2);
        test_uint(ecs_childof(p1), ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_uint(ecs_id(Velocity), ecs_field_id(&it, 2));
        test_uint(p1, ecs_iter_get_var(&it, parent_var));
        {
            Position *p = ecs_field(&it, Position, 1);
            test_assert(p != NULL);
            test_int(p->x, 12);
            test_int(p->y, 22);
        }
        {
            Velocity *v = ecs_field(&it, Velocity, 2);
            test_assert(v != NULL);
            test_int(v->x, 2);
            test_int(v->y, 2);
        }
        test_bool(false, ecs_query_next(&it));
    }

    {
        ecs_iter_t it = ecs_query_iter(ecs, q);
        ecs_iter_set_var(&it, 0, e3);
        test_bool(true, ecs_query_next(&it));
        test_int(it.count, 1);
        test_uint(it.entities[0], e3);
        test_uint(ecs_childof(p2), ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_uint(ecs_id(Velocity), ecs_field_id(&it, 2));
        test_uint(p2, ecs_iter_get_var(&it, parent_var));
        {
            Position *p = ecs_field(&it, Position, 1);
            test_assert(p != NULL);
            test_int(p->x, 13);
            test_int(p->y, 23);
        }
        {
            Velocity *v = ecs_field(&it, Velocity, 2);
            test_assert(v != NULL);
            test_int(v->x, 3);
            test_int(v->y, 3);
        }
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(ecs);
}

void Variables_3_set_src_this_w_tag_uncacheable_tag_tag(void) {
    ecs_world_t* ecs = ecs_mini();

    ECS_TAG(ecs, TagA);
    ECS_TAG(ecs, TagB);

    ecs_entity_t p1 = ecs_new(ecs);
    ecs_entity_t p2 = ecs_new(ecs);
    ecs_entity_t e1 = ecs_new_w(ecs, TagA);
    ecs_entity_t e2 = ecs_new_w(ecs, TagA);
    ecs_entity_t e3 = ecs_new_w(ecs, TagA);
    ecs_add(ecs, e1, TagB);
    ecs_add(ecs, e2, TagB);
    ecs_add(ecs, e3, TagB);
    ecs_add_pair(ecs, e1, EcsChildOf, p1);
    ecs_add_pair(ecs, e2, EcsChildOf, p1);
    ecs_add_pair(ecs, e3, EcsChildOf, p2);

    ecs_query_t *q = ecs_query(ecs, {
        .terms = {
            { TagA },
            { .first.id = EcsChildOf, .second.name = "$parent" },
            { TagB }
        },
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int32_t parent_var = ecs_query_find_var(q, "parent");
    test_assert(parent_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(ecs, q);
        ecs_iter_set_var(&it, 0, e1);
        test_bool(true, ecs_query_next(&it));
        test_int(it.count, 1);
        test_uint(it.entities[0], e1);
        test_uint(TagA, ecs_field_id(&it, 0));
        test_uint(ecs_childof(p1), ecs_field_id(&it, 1));
        test_uint(TagB, ecs_field_id(&it, 2));
        test_uint(p1, ecs_iter_get_var(&it, parent_var));
        test_bool(false, ecs_query_next(&it));
    }

    {
        ecs_iter_t it = ecs_query_iter(ecs, q);
        ecs_iter_set_var(&it, 0, e2);
        test_bool(true, ecs_query_next(&it));
        test_int(it.count, 1);
        test_uint(it.entities[0], e2);
        test_uint(TagA, ecs_field_id(&it, 0));
        test_uint(ecs_childof(p1), ecs_field_id(&it, 1));
        test_uint(TagB, ecs_field_id(&it, 2));
        test_uint(p1, ecs_iter_get_var(&it, parent_var));
        test_bool(false, ecs_query_next(&it));
    }

    {
        ecs_iter_t it = ecs_query_iter(ecs, q);
        ecs_iter_set_var(&it, 0, e3);
        test_bool(true, ecs_query_next(&it));
        test_int(it.count, 1);
        test_uint(it.entities[0], e3);
        test_uint(TagA, ecs_field_id(&it, 0));
        test_uint(ecs_childof(p2), ecs_field_id(&it, 1));
        test_uint(TagB, ecs_field_id(&it, 2));
        test_uint(p2, ecs_iter_get_var(&it, parent_var));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(ecs);
}

void Variables_3_set_src_this_w_component_uncacheable_tag_tag(void) {
    ecs_world_t* ecs = ecs_mini();

    ECS_COMPONENT(ecs, Position);
    ECS_TAG(ecs, TagB);

    ecs_entity_t p1 = ecs_new(ecs);
    ecs_entity_t p2 = ecs_new(ecs);
    ecs_entity_t e1 = ecs_insert(ecs, ecs_value(Position, {11, 21}));
    ecs_entity_t e2 = ecs_insert(ecs, ecs_value(Position, {12, 22}));
    ecs_entity_t e3 = ecs_insert(ecs, ecs_value(Position, {13, 23}));
    ecs_add(ecs, e1, TagB);
    ecs_add(ecs, e2, TagB);
    ecs_add(ecs, e3, TagB);
    ecs_add_pair(ecs, e1, EcsChildOf, p1);
    ecs_add_pair(ecs, e2, EcsChildOf, p1);
    ecs_add_pair(ecs, e3, EcsChildOf, p2);

    ecs_query_t *q = ecs_query(ecs, {
        .terms = {
            { ecs_id(Position) },
            { .first.id = EcsChildOf, .second.name = "$parent" },
            { TagB }
        },
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int32_t parent_var = ecs_query_find_var(q, "parent");
    test_assert(parent_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(ecs, q);
        ecs_iter_set_var(&it, 0, e1);
        test_bool(true, ecs_query_next(&it));
        test_int(it.count, 1);
        test_uint(it.entities[0], e1);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(ecs_childof(p1), ecs_field_id(&it, 1));
        test_uint(TagB, ecs_field_id(&it, 2));
        test_uint(p1, ecs_iter_get_var(&it, parent_var));
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 11);
            test_int(p->y, 21);
        }
        test_bool(false, ecs_query_next(&it));
    }

    {
        ecs_iter_t it = ecs_query_iter(ecs, q);
        ecs_iter_set_var(&it, 0, e2);
        test_bool(true, ecs_query_next(&it));
        test_int(it.count, 1);
        test_uint(it.entities[0], e2);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(ecs_childof(p1), ecs_field_id(&it, 1));
        test_uint(TagB, ecs_field_id(&it, 2));
        test_uint(p1, ecs_iter_get_var(&it, parent_var));
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 12);
            test_int(p->y, 22);
        }
        test_bool(false, ecs_query_next(&it));
    }

    {
        ecs_iter_t it = ecs_query_iter(ecs, q);
        ecs_iter_set_var(&it, 0, e3);
        test_bool(true, ecs_query_next(&it));
        test_int(it.count, 1);
        test_uint(it.entities[0], e3);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(ecs_childof(p2), ecs_field_id(&it, 1));
        test_uint(TagB, ecs_field_id(&it, 2));
        test_uint(p2, ecs_iter_get_var(&it, parent_var));
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 13);
            test_int(p->y, 23);
        }
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(ecs);
}

void Variables_3_set_src_this_w_component_uncacheable_tag_component(void) {
    ecs_world_t* ecs = ecs_mini();

    ECS_COMPONENT(ecs, Position);
    ECS_COMPONENT(ecs, Velocity);

    ecs_entity_t p1 = ecs_new(ecs);
    ecs_entity_t p2 = ecs_new(ecs);
    ecs_entity_t e1 = ecs_insert(ecs, ecs_value(Position, {11, 21}));
    ecs_entity_t e2 = ecs_insert(ecs, ecs_value(Position, {12, 22}));
    ecs_entity_t e3 = ecs_insert(ecs, ecs_value(Position, {13, 23}));
    ecs_set(ecs, e1, Velocity, {1, 1});
    ecs_set(ecs, e2, Velocity, {2, 2});
    ecs_set(ecs, e3, Velocity, {3, 3});
    ecs_add_pair(ecs, e1, EcsChildOf, p1);
    ecs_add_pair(ecs, e2, EcsChildOf, p1);
    ecs_add_pair(ecs, e3, EcsChildOf, p2);

    ecs_query_t *q = ecs_query(ecs, {
        .terms = {
            { ecs_id(Position) },
            { .first.id = EcsChildOf, .second.name = "$parent" },
            { ecs_id(Velocity) }
        },
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int32_t parent_var = ecs_query_find_var(q, "parent");
    test_assert(parent_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(ecs, q);
        ecs_iter_set_var(&it, 0, e1);
        test_bool(true, ecs_query_next(&it));
        test_int(it.count, 1);
        test_uint(it.entities[0], e1);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(ecs_childof(p1), ecs_field_id(&it, 1));
        test_uint(ecs_id(Velocity), ecs_field_id(&it, 2));
        test_uint(p1, ecs_iter_get_var(&it, parent_var));
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 11);
            test_int(p->y, 21);
        }
        {
            Velocity *v = ecs_field(&it, Velocity, 2);
            test_assert(v != NULL);
            test_int(v->x, 1);
            test_int(v->y, 1);
        }
        test_bool(false, ecs_query_next(&it));
    }

    {
        ecs_iter_t it = ecs_query_iter(ecs, q);
        ecs_iter_set_var(&it, 0, e2);
        test_bool(true, ecs_query_next(&it));
        test_int(it.count, 1);
        test_uint(it.entities[0], e2);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(ecs_childof(p1), ecs_field_id(&it, 1));
        test_uint(ecs_id(Velocity), ecs_field_id(&it, 2));
        test_uint(p1, ecs_iter_get_var(&it, parent_var));
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 12);
            test_int(p->y, 22);
        }
        {
            Velocity *v = ecs_field(&it, Velocity, 2);
            test_assert(v != NULL);
            test_int(v->x, 2);
            test_int(v->y, 2);
        }
        test_bool(false, ecs_query_next(&it));
    }

    {
        ecs_iter_t it = ecs_query_iter(ecs, q);
        ecs_iter_set_var(&it, 0, e3);
        test_bool(true, ecs_query_next(&it));
        test_int(it.count, 1);
        test_uint(it.entities[0], e3);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(ecs_childof(p2), ecs_field_id(&it, 1));
        test_uint(ecs_id(Velocity), ecs_field_id(&it, 2));
        test_uint(p2, ecs_iter_get_var(&it, parent_var));
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 13);
            test_int(p->y, 23);
        }
        {
            Velocity *v = ecs_field(&it, Velocity, 2);
            test_assert(v != NULL);
            test_int(v->x, 3);
            test_int(v->y, 3);
        }
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(ecs);
}

void Variables_3_set_src_this_w_tag_tag_uncacheable_tag(void) {
    ecs_world_t* ecs = ecs_mini();

    ECS_TAG(ecs, TagA);
    ECS_TAG(ecs, TagB);

    ecs_entity_t p1 = ecs_new(ecs);
    ecs_entity_t p2 = ecs_new(ecs);
    ecs_entity_t e1 = ecs_new_w(ecs, TagA);
    ecs_entity_t e2 = ecs_new_w(ecs, TagA);
    ecs_entity_t e3 = ecs_new_w(ecs, TagA);
    ecs_add(ecs, e1, TagB);
    ecs_add(ecs, e2, TagB);
    ecs_add(ecs, e3, TagB);
    ecs_add_pair(ecs, e1, EcsChildOf, p1);
    ecs_add_pair(ecs, e2, EcsChildOf, p1);
    ecs_add_pair(ecs, e3, EcsChildOf, p2);

    ecs_query_t *q = ecs_query(ecs, {
        .terms = {
            { TagA },
            { TagB },
            { .first.id = EcsChildOf, .second.name = "$parent" }
        },
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int32_t parent_var = ecs_query_find_var(q, "parent");
    test_assert(parent_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(ecs, q);
        ecs_iter_set_var(&it, 0, e1);
        test_bool(true, ecs_query_next(&it));
        test_int(it.count, 1);
        test_uint(it.entities[0], e1);
        test_uint(TagA, ecs_field_id(&it, 0));
        test_uint(TagB, ecs_field_id(&it, 1));
        test_uint(ecs_childof(p1), ecs_field_id(&it, 2));
        test_uint(p1, ecs_iter_get_var(&it, parent_var));
        test_bool(false, ecs_query_next(&it));
    }

    {
        ecs_iter_t it = ecs_query_iter(ecs, q);
        ecs_iter_set_var(&it, 0, e2);
        test_bool(true, ecs_query_next(&it));
        test_int(it.count, 1);
        test_uint(it.entities[0], e2);
        test_uint(TagA, ecs_field_id(&it, 0));
        test_uint(TagB, ecs_field_id(&it, 1));
        test_uint(ecs_childof(p1), ecs_field_id(&it, 2));
        test_uint(p1, ecs_iter_get_var(&it, parent_var));
        test_bool(false, ecs_query_next(&it));
    }

    {
        ecs_iter_t it = ecs_query_iter(ecs, q);
        ecs_iter_set_var(&it, 0, e3);
        test_bool(true, ecs_query_next(&it));
        test_int(it.count, 1);
        test_uint(it.entities[0], e3);
        test_uint(TagA, ecs_field_id(&it, 0));
        test_uint(TagB, ecs_field_id(&it, 1));
        test_uint(ecs_childof(p2), ecs_field_id(&it, 2));
        test_uint(p2, ecs_iter_get_var(&it, parent_var));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(ecs);
}

void Variables_3_set_src_this_w_component_tag_uncacheable_tag(void) {
    ecs_world_t* ecs = ecs_mini();

    ECS_COMPONENT(ecs, Position);
    ECS_TAG(ecs, TagB);

    ecs_entity_t p1 = ecs_new(ecs);
    ecs_entity_t p2 = ecs_new(ecs);
    ecs_entity_t e1 = ecs_insert(ecs, ecs_value(Position, {11, 21}));
    ecs_entity_t e2 = ecs_insert(ecs, ecs_value(Position, {12, 22}));
    ecs_entity_t e3 = ecs_insert(ecs, ecs_value(Position, {13, 23}));
    ecs_add(ecs, e1, TagB);
    ecs_add(ecs, e2, TagB);
    ecs_add(ecs, e3, TagB);
    ecs_add_pair(ecs, e1, EcsChildOf, p1);
    ecs_add_pair(ecs, e2, EcsChildOf, p1);
    ecs_add_pair(ecs, e3, EcsChildOf, p2);

    ecs_query_t *q = ecs_query(ecs, {
        .terms = {
            { ecs_id(Position) },
            { TagB },
            { .first.id = EcsChildOf, .second.name = "$parent" }
        },
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int32_t parent_var = ecs_query_find_var(q, "parent");
    test_assert(parent_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(ecs, q);
        ecs_iter_set_var(&it, 0, e1);
        test_bool(true, ecs_query_next(&it));
        test_int(it.count, 1);
        test_uint(it.entities[0], e1);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(TagB, ecs_field_id(&it, 1));
        test_uint(ecs_childof(p1), ecs_field_id(&it, 2));
        test_uint(p1, ecs_iter_get_var(&it, parent_var));
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 11);
            test_int(p->y, 21);
        }
        test_bool(false, ecs_query_next(&it));
    }

    {
        ecs_iter_t it = ecs_query_iter(ecs, q);
        ecs_iter_set_var(&it, 0, e2);
        test_bool(true, ecs_query_next(&it));
        test_int(it.count, 1);
        test_uint(it.entities[0], e2);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(TagB, ecs_field_id(&it, 1));
        test_uint(ecs_childof(p1), ecs_field_id(&it, 2));
        test_uint(p1, ecs_iter_get_var(&it, parent_var));
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 12);
            test_int(p->y, 22);
        }
        test_bool(false, ecs_query_next(&it));
    }

    {
        ecs_iter_t it = ecs_query_iter(ecs, q);
        ecs_iter_set_var(&it, 0, e3);
        test_bool(true, ecs_query_next(&it));
        test_int(it.count, 1);
        test_uint(it.entities[0], e3);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(TagB, ecs_field_id(&it, 1));
        test_uint(ecs_childof(p2), ecs_field_id(&it, 2));
        test_uint(p2, ecs_iter_get_var(&it, parent_var));
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 13);
            test_int(p->y, 23);
        }
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(ecs);
}

void Variables_3_set_src_this_w_component_component_uncacheable_tag(void) {
    ecs_world_t* ecs = ecs_mini();

    ECS_COMPONENT(ecs, Position);
    ECS_COMPONENT(ecs, Velocity);

    ecs_entity_t p1 = ecs_new(ecs);
    ecs_entity_t p2 = ecs_new(ecs);
    ecs_entity_t e1 = ecs_insert(ecs, ecs_value(Position, {11, 21}));
    ecs_entity_t e2 = ecs_insert(ecs, ecs_value(Position, {12, 22}));
    ecs_entity_t e3 = ecs_insert(ecs, ecs_value(Position, {13, 23}));
    ecs_set(ecs, e1, Velocity, {1, 1});
    ecs_set(ecs, e2, Velocity, {2, 2});
    ecs_set(ecs, e3, Velocity, {3, 3});
    ecs_add_pair(ecs, e1, EcsChildOf, p1);
    ecs_add_pair(ecs, e2, EcsChildOf, p1);
    ecs_add_pair(ecs, e3, EcsChildOf, p2);

    ecs_query_t *q = ecs_query(ecs, {
        .terms = {
            { ecs_id(Position) },
            { ecs_id(Velocity) },
            { .first.id = EcsChildOf, .second.name = "$parent" }
        },
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int32_t parent_var = ecs_query_find_var(q, "parent");
    test_assert(parent_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(ecs, q);
        ecs_iter_set_var(&it, 0, e1);
        test_bool(true, ecs_query_next(&it));
        test_int(it.count, 1);
        test_uint(it.entities[0], e1);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(ecs_id(Velocity), ecs_field_id(&it, 1));
        test_uint(ecs_childof(p1), ecs_field_id(&it, 2));
        test_uint(p1, ecs_iter_get_var(&it, parent_var));
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 11);
            test_int(p->y, 21);
        }
        {
            Velocity *v = ecs_field(&it, Velocity, 1);
            test_assert(v != NULL);
            test_int(v->x, 1);
            test_int(v->y, 1);
        }
        test_bool(false, ecs_query_next(&it));
    }

    {
        ecs_iter_t it = ecs_query_iter(ecs, q);
        ecs_iter_set_var(&it, 0, e2);
        test_bool(true, ecs_query_next(&it));
        test_int(it.count, 1);
        test_uint(it.entities[0], e2);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(ecs_id(Velocity), ecs_field_id(&it, 1));
        test_uint(ecs_childof(p1), ecs_field_id(&it, 2));
        test_uint(p1, ecs_iter_get_var(&it, parent_var));
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 12);
            test_int(p->y, 22);
        }
        {
            Velocity *v = ecs_field(&it, Velocity, 1);
            test_assert(v != NULL);
            test_int(v->x, 2);
            test_int(v->y, 2);
        }
        test_bool(false, ecs_query_next(&it));
    }

    {
        ecs_iter_t it = ecs_query_iter(ecs, q);
        ecs_iter_set_var(&it, 0, e3);
        test_bool(true, ecs_query_next(&it));
        test_int(it.count, 1);
        test_uint(it.entities[0], e3);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(ecs_id(Velocity), ecs_field_id(&it, 1));
        test_uint(ecs_childof(p2), ecs_field_id(&it, 2));
        test_uint(p2, ecs_iter_get_var(&it, parent_var));
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 13);
            test_int(p->y, 23);
        }
        {
            Velocity *v = ecs_field(&it, Velocity, 1);
            test_assert(v != NULL);
            test_int(v->x, 3);
            test_int(v->y, 3);
        }
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(ecs);
}

void Variables_3_set_src_this_w_uncacheable_component_tag_tag(void) {
    ecs_world_t* ecs = ecs_mini();

    ECS_TAG(ecs, TagA);
    ECS_TAG(ecs, TagB);
    ECS_COMPONENT(ecs, Mass);

    ecs_entity_t p1 = ecs_new(ecs);
    ecs_entity_t p2 = ecs_new(ecs);
    ecs_entity_t e1 = ecs_new_w(ecs, TagA);
    ecs_entity_t e2 = ecs_new_w(ecs, TagA);
    ecs_entity_t e3 = ecs_new_w(ecs, TagA);
    ecs_add(ecs, e1, TagB);
    ecs_add(ecs, e2, TagB);
    ecs_add(ecs, e3, TagB);
    ecs_set_pair(ecs, e1, Mass, p1, {100});
    ecs_set_pair(ecs, e2, Mass, p1, {200});
    ecs_set_pair(ecs, e3, Mass, p2, {300});

    ecs_query_t *q = ecs_query(ecs, {
        .terms = {
            { .first.id = ecs_id(Mass), .second.name = "$parent" },
            { TagA },
            { TagB }
        },
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int32_t parent_var = ecs_query_find_var(q, "parent");
    test_assert(parent_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(ecs, q);
        ecs_iter_set_var(&it, 0, e1);
        test_bool(true, ecs_query_next(&it));
        test_int(it.count, 1);
        test_uint(it.entities[0], e1);
        test_uint(ecs_pair_t(Mass, p1), ecs_field_id(&it, 0));
        test_uint(TagA, ecs_field_id(&it, 1));
        test_uint(TagB, ecs_field_id(&it, 2));
        test_uint(p1, ecs_iter_get_var(&it, parent_var));
        {
            Mass *m = ecs_field(&it, Mass, 0);
            test_assert(m != NULL);
            test_int(*m, 100);
        }
        test_bool(false, ecs_query_next(&it));
    }

    {
        ecs_iter_t it = ecs_query_iter(ecs, q);
        ecs_iter_set_var(&it, 0, e2);
        test_bool(true, ecs_query_next(&it));
        test_int(it.count, 1);
        test_uint(it.entities[0], e2);
        test_uint(ecs_pair_t(Mass, p1), ecs_field_id(&it, 0));
        test_uint(TagA, ecs_field_id(&it, 1));
        test_uint(TagB, ecs_field_id(&it, 2));
        test_uint(p1, ecs_iter_get_var(&it, parent_var));
        {
            Mass *m = ecs_field(&it, Mass, 0);
            test_assert(m != NULL);
            test_int(*m, 200);
        }
        test_bool(false, ecs_query_next(&it));
    }

    {
        ecs_iter_t it = ecs_query_iter(ecs, q);
        ecs_iter_set_var(&it, 0, e3);
        test_bool(true, ecs_query_next(&it));
        test_int(it.count, 1);
        test_uint(it.entities[0], e3);
        test_uint(ecs_pair_t(Mass, p2), ecs_field_id(&it, 0));
        test_uint(TagA, ecs_field_id(&it, 1));
        test_uint(TagB, ecs_field_id(&it, 2));
        test_uint(p2, ecs_iter_get_var(&it, parent_var));
        {
            Mass *m = ecs_field(&it, Mass, 0);
            test_assert(m != NULL);
            test_int(*m, 300);
        }
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(ecs);
}

void Variables_3_set_src_this_w_uncacheable_component_component_tag(void) {
    ecs_world_t* ecs = ecs_mini();

    ECS_COMPONENT(ecs, Mass);
    ECS_COMPONENT(ecs, Position);
    ECS_TAG(ecs, TagB);

    ecs_entity_t p1 = ecs_new(ecs);
    ecs_entity_t p2 = ecs_new(ecs);
    ecs_entity_t e1 = ecs_insert(ecs, ecs_value(Position, {11, 21}));
    ecs_entity_t e2 = ecs_insert(ecs, ecs_value(Position, {12, 22}));
    ecs_entity_t e3 = ecs_insert(ecs, ecs_value(Position, {13, 23}));
    ecs_add(ecs, e1, TagB);
    ecs_add(ecs, e2, TagB);
    ecs_add(ecs, e3, TagB);
    ecs_set_pair(ecs, e1, Mass, p1, {100});
    ecs_set_pair(ecs, e2, Mass, p1, {200});
    ecs_set_pair(ecs, e3, Mass, p2, {300});

    ecs_query_t *q = ecs_query(ecs, {
        .terms = {
            { .first.id = ecs_id(Mass), .second.name = "$parent" },
            { ecs_id(Position) },
            { TagB }
        },
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int32_t parent_var = ecs_query_find_var(q, "parent");
    test_assert(parent_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(ecs, q);
        ecs_iter_set_var(&it, 0, e1);
        test_bool(true, ecs_query_next(&it));
        test_int(it.count, 1);
        test_uint(it.entities[0], e1);
        test_uint(ecs_pair_t(Mass, p1), ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_uint(TagB, ecs_field_id(&it, 2));
        test_uint(p1, ecs_iter_get_var(&it, parent_var));
        {
            Mass *m = ecs_field(&it, Mass, 0);
            test_assert(m != NULL);
            test_int(*m, 100);
        }
        {
            Position *p = ecs_field(&it, Position, 1);
            test_assert(p != NULL);
            test_int(p->x, 11);
            test_int(p->y, 21);
        }
        test_bool(false, ecs_query_next(&it));
    }

    {
        ecs_iter_t it = ecs_query_iter(ecs, q);
        ecs_iter_set_var(&it, 0, e2);
        test_bool(true, ecs_query_next(&it));
        test_int(it.count, 1);
        test_uint(it.entities[0], e2);
        test_uint(ecs_pair_t(Mass, p1), ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_uint(TagB, ecs_field_id(&it, 2));
        test_uint(p1, ecs_iter_get_var(&it, parent_var));
        {
            Mass *m = ecs_field(&it, Mass, 0);
            test_assert(m != NULL);
            test_int(*m, 200);
        }
        {
            Position *p = ecs_field(&it, Position, 1);
            test_assert(p != NULL);
            test_int(p->x, 12);
            test_int(p->y, 22);
        }
        test_bool(false, ecs_query_next(&it));
    }

    {
        ecs_iter_t it = ecs_query_iter(ecs, q);
        ecs_iter_set_var(&it, 0, e3);
        test_bool(true, ecs_query_next(&it));
        test_int(it.count, 1);
        test_uint(it.entities[0], e3);
        test_uint(ecs_pair_t(Mass, p2), ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_uint(TagB, ecs_field_id(&it, 2));
        test_uint(p2, ecs_iter_get_var(&it, parent_var));
        {
            Mass *m = ecs_field(&it, Mass, 0);
            test_assert(m != NULL);
            test_int(*m, 300);
        }
        {
            Position *p = ecs_field(&it, Position, 1);
            test_assert(p != NULL);
            test_int(p->x, 13);
            test_int(p->y, 23);
        }
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(ecs);
}

void Variables_3_set_src_this_w_uncacheable_component_component_component(void) {
    ecs_world_t* ecs = ecs_mini();

    ECS_COMPONENT(ecs, Mass);
    ECS_COMPONENT(ecs, Position);
    ECS_COMPONENT(ecs, Velocity);

    ecs_entity_t p1 = ecs_new(ecs);
    ecs_entity_t p2 = ecs_new(ecs);
    ecs_entity_t e1 = ecs_insert(ecs, ecs_value(Position, {11, 21}));
    ecs_entity_t e2 = ecs_insert(ecs, ecs_value(Position, {12, 22}));
    ecs_entity_t e3 = ecs_insert(ecs, ecs_value(Position, {13, 23}));
    ecs_set(ecs, e1, Velocity, {1, 1});
    ecs_set(ecs, e2, Velocity, {2, 2});
    ecs_set(ecs, e3, Velocity, {3, 3});
    ecs_set_pair(ecs, e1, Mass, p1, {100});
    ecs_set_pair(ecs, e2, Mass, p1, {200});
    ecs_set_pair(ecs, e3, Mass, p2, {300});

    ecs_query_t *q = ecs_query(ecs, {
        .terms = {
            { .first.id = ecs_id(Mass), .second.name = "$parent" },
            { ecs_id(Position) },
            { ecs_id(Velocity) }
        },
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int32_t parent_var = ecs_query_find_var(q, "parent");
    test_assert(parent_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(ecs, q);
        ecs_iter_set_var(&it, 0, e1);
        test_bool(true, ecs_query_next(&it));
        test_int(it.count, 1);
        test_uint(it.entities[0], e1);
        test_uint(ecs_pair_t(Mass, p1), ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_uint(ecs_id(Velocity), ecs_field_id(&it, 2));
        test_uint(p1, ecs_iter_get_var(&it, parent_var));
        {
            Mass *m = ecs_field(&it, Mass, 0);
            test_assert(m != NULL);
            test_int(*m, 100);
        }
        {
            Position *p = ecs_field(&it, Position, 1);
            test_assert(p != NULL);
            test_int(p->x, 11);
            test_int(p->y, 21);
        }
        {
            Velocity *v = ecs_field(&it, Velocity, 2);
            test_assert(v != NULL);
            test_int(v->x, 1);
            test_int(v->y, 1);
        }
        test_bool(false, ecs_query_next(&it));
    }

    {
        ecs_iter_t it = ecs_query_iter(ecs, q);
        ecs_iter_set_var(&it, 0, e2);
        test_bool(true, ecs_query_next(&it));
        test_int(it.count, 1);
        test_uint(it.entities[0], e2);
        test_uint(ecs_pair_t(Mass, p1), ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_uint(ecs_id(Velocity), ecs_field_id(&it, 2));
        test_uint(p1, ecs_iter_get_var(&it, parent_var));
        {
            Mass *m = ecs_field(&it, Mass, 0);
            test_assert(m != NULL);
            test_int(*m, 200);
        }
        {
            Position *p = ecs_field(&it, Position, 1);
            test_assert(p != NULL);
            test_int(p->x, 12);
            test_int(p->y, 22);
        }
        {
            Velocity *v = ecs_field(&it, Velocity, 2);
            test_assert(v != NULL);
            test_int(v->x, 2);
            test_int(v->y, 2);
        }
        test_bool(false, ecs_query_next(&it));
    }

    {
        ecs_iter_t it = ecs_query_iter(ecs, q);
        ecs_iter_set_var(&it, 0, e3);
        test_bool(true, ecs_query_next(&it));
        test_int(it.count, 1);
        test_uint(it.entities[0], e3);
        test_uint(ecs_pair_t(Mass, p2), ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_uint(ecs_id(Velocity), ecs_field_id(&it, 2));
        test_uint(p2, ecs_iter_get_var(&it, parent_var));
        {
            Mass *m = ecs_field(&it, Mass, 0);
            test_assert(m != NULL);
            test_int(*m, 300);
        }
        {
            Position *p = ecs_field(&it, Position, 1);
            test_assert(p != NULL);
            test_int(p->x, 13);
            test_int(p->y, 23);
        }
        {
            Velocity *v = ecs_field(&it, Velocity, 2);
            test_assert(v != NULL);
            test_int(v->x, 3);
            test_int(v->y, 3);
        }
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(ecs);
}

void Variables_3_set_src_this_w_tag_uncacheable_component_tag(void) {
    ecs_world_t* ecs = ecs_mini();

    ECS_TAG(ecs, TagA);
    ECS_TAG(ecs, TagB);
    ECS_COMPONENT(ecs, Mass);

    ecs_entity_t p1 = ecs_new(ecs);
    ecs_entity_t p2 = ecs_new(ecs);
    ecs_entity_t e1 = ecs_new_w(ecs, TagA);
    ecs_entity_t e2 = ecs_new_w(ecs, TagA);
    ecs_entity_t e3 = ecs_new_w(ecs, TagA);
    ecs_add(ecs, e1, TagB);
    ecs_add(ecs, e2, TagB);
    ecs_add(ecs, e3, TagB);
    ecs_set_pair(ecs, e1, Mass, p1, {100});
    ecs_set_pair(ecs, e2, Mass, p1, {200});
    ecs_set_pair(ecs, e3, Mass, p2, {300});

    ecs_query_t *q = ecs_query(ecs, {
        .terms = {
            { TagA },
            { .first.id = ecs_id(Mass), .second.name = "$parent" },
            { TagB }
        },
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int32_t parent_var = ecs_query_find_var(q, "parent");
    test_assert(parent_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(ecs, q);
        ecs_iter_set_var(&it, 0, e1);
        test_bool(true, ecs_query_next(&it));
        test_int(it.count, 1);
        test_uint(it.entities[0], e1);
        test_uint(TagA, ecs_field_id(&it, 0));
        test_uint(ecs_pair_t(Mass, p1), ecs_field_id(&it, 1));
        test_uint(TagB, ecs_field_id(&it, 2));
        test_uint(p1, ecs_iter_get_var(&it, parent_var));
        {
            Mass *m = ecs_field(&it, Mass, 1);
            test_assert(m != NULL);
            test_int(*m, 100);
        }
        test_bool(false, ecs_query_next(&it));
    }

    {
        ecs_iter_t it = ecs_query_iter(ecs, q);
        ecs_iter_set_var(&it, 0, e2);
        test_bool(true, ecs_query_next(&it));
        test_int(it.count, 1);
        test_uint(it.entities[0], e2);
        test_uint(TagA, ecs_field_id(&it, 0));
        test_uint(ecs_pair_t(Mass, p1), ecs_field_id(&it, 1));
        test_uint(TagB, ecs_field_id(&it, 2));
        test_uint(p1, ecs_iter_get_var(&it, parent_var));
        {
            Mass *m = ecs_field(&it, Mass, 1);
            test_assert(m != NULL);
            test_int(*m, 200);
        }
        test_bool(false, ecs_query_next(&it));
    }

    {
        ecs_iter_t it = ecs_query_iter(ecs, q);
        ecs_iter_set_var(&it, 0, e3);
        test_bool(true, ecs_query_next(&it));
        test_int(it.count, 1);
        test_uint(it.entities[0], e3);
        test_uint(TagA, ecs_field_id(&it, 0));
        test_uint(ecs_pair_t(Mass, p2), ecs_field_id(&it, 1));
        test_uint(TagB, ecs_field_id(&it, 2));
        test_uint(p2, ecs_iter_get_var(&it, parent_var));
        {
            Mass *m = ecs_field(&it, Mass, 1);
            test_assert(m != NULL);
            test_int(*m, 300);
        }
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(ecs);
}

void Variables_3_set_src_this_w_component_uncacheable_component_tag(void) {
    ecs_world_t* ecs = ecs_mini();

    ECS_COMPONENT(ecs, Mass);
    ECS_COMPONENT(ecs, Position);
    ECS_TAG(ecs, TagB);

    ecs_entity_t p1 = ecs_new(ecs);
    ecs_entity_t p2 = ecs_new(ecs);
    ecs_entity_t e1 = ecs_insert(ecs, ecs_value(Position, {11, 21}));
    ecs_entity_t e2 = ecs_insert(ecs, ecs_value(Position, {12, 22}));
    ecs_entity_t e3 = ecs_insert(ecs, ecs_value(Position, {13, 23}));
    ecs_add(ecs, e1, TagB);
    ecs_add(ecs, e2, TagB);
    ecs_add(ecs, e3, TagB);
    ecs_set_pair(ecs, e1, Mass, p1, {100});
    ecs_set_pair(ecs, e2, Mass, p1, {200});
    ecs_set_pair(ecs, e3, Mass, p2, {300});

    ecs_query_t *q = ecs_query(ecs, {
        .terms = {
            { ecs_id(Position) },
            { .first.id = ecs_id(Mass), .second.name = "$parent" },
            { TagB }
        },
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int32_t parent_var = ecs_query_find_var(q, "parent");
    test_assert(parent_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(ecs, q);
        ecs_iter_set_var(&it, 0, e1);
        test_bool(true, ecs_query_next(&it));
        test_int(it.count, 1);
        test_uint(it.entities[0], e1);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(ecs_pair_t(Mass, p1), ecs_field_id(&it, 1));
        test_uint(TagB, ecs_field_id(&it, 2));
        test_uint(p1, ecs_iter_get_var(&it, parent_var));
        {
            Mass *m = ecs_field(&it, Mass, 1);
            test_assert(m != NULL);
            test_int(*m, 100);
        }
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 11);
            test_int(p->y, 21);
        }
        test_bool(false, ecs_query_next(&it));
    }

    {
        ecs_iter_t it = ecs_query_iter(ecs, q);
        ecs_iter_set_var(&it, 0, e2);
        test_bool(true, ecs_query_next(&it));
        test_int(it.count, 1);
        test_uint(it.entities[0], e2);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(ecs_pair_t(Mass, p1), ecs_field_id(&it, 1));
        test_uint(TagB, ecs_field_id(&it, 2));
        test_uint(p1, ecs_iter_get_var(&it, parent_var));
        {
            Mass *m = ecs_field(&it, Mass, 1);
            test_assert(m != NULL);
            test_int(*m, 200);
        }
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 12);
            test_int(p->y, 22);
        }
        test_bool(false, ecs_query_next(&it));
    }

    {
        ecs_iter_t it = ecs_query_iter(ecs, q);
        ecs_iter_set_var(&it, 0, e3);
        test_bool(true, ecs_query_next(&it));
        test_int(it.count, 1);
        test_uint(it.entities[0], e3);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(ecs_pair_t(Mass, p2), ecs_field_id(&it, 1));
        test_uint(TagB, ecs_field_id(&it, 2));
        test_uint(p2, ecs_iter_get_var(&it, parent_var));
        {
            Mass *m = ecs_field(&it, Mass, 1);
            test_assert(m != NULL);
            test_int(*m, 300);
        }
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 13);
            test_int(p->y, 23);
        }
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(ecs);
}

void Variables_3_set_src_this_w_component_uncacheable_component_component(void) {
    ecs_world_t* ecs = ecs_mini();

    ECS_COMPONENT(ecs, Mass);
    ECS_COMPONENT(ecs, Position);
    ECS_COMPONENT(ecs, Velocity);

    ecs_entity_t p1 = ecs_new(ecs);
    ecs_entity_t p2 = ecs_new(ecs);
    ecs_entity_t e1 = ecs_insert(ecs, ecs_value(Position, {11, 21}));
    ecs_entity_t e2 = ecs_insert(ecs, ecs_value(Position, {12, 22}));
    ecs_entity_t e3 = ecs_insert(ecs, ecs_value(Position, {13, 23}));
    ecs_set(ecs, e1, Velocity, {1, 1});
    ecs_set(ecs, e2, Velocity, {2, 2});
    ecs_set(ecs, e3, Velocity, {3, 3});
    ecs_set_pair(ecs, e1, Mass, p1, {100});
    ecs_set_pair(ecs, e2, Mass, p1, {200});
    ecs_set_pair(ecs, e3, Mass, p2, {300});

    ecs_query_t *q = ecs_query(ecs, {
        .terms = {
            { ecs_id(Position) },
            { .first.id = ecs_id(Mass), .second.name = "$parent" },
            { ecs_id(Velocity) }
        },
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int32_t parent_var = ecs_query_find_var(q, "parent");
    test_assert(parent_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(ecs, q);
        ecs_iter_set_var(&it, 0, e1);
        test_bool(true, ecs_query_next(&it));
        test_int(it.count, 1);
        test_uint(it.entities[0], e1);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(ecs_pair_t(Mass, p1), ecs_field_id(&it, 1));
        test_uint(ecs_id(Velocity), ecs_field_id(&it, 2));
        test_uint(p1, ecs_iter_get_var(&it, parent_var));
        {
            Mass *m = ecs_field(&it, Mass, 1);
            test_assert(m != NULL);
            test_int(*m, 100);
        }
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 11);
            test_int(p->y, 21);
        }
        {
            Velocity *v = ecs_field(&it, Velocity, 2);
            test_assert(v != NULL);
            test_int(v->x, 1);
            test_int(v->y, 1);
        }
        test_bool(false, ecs_query_next(&it));
    }

    {
        ecs_iter_t it = ecs_query_iter(ecs, q);
        ecs_iter_set_var(&it, 0, e2);
        test_bool(true, ecs_query_next(&it));
        test_int(it.count, 1);
        test_uint(it.entities[0], e2);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(ecs_pair_t(Mass, p1), ecs_field_id(&it, 1));
        test_uint(ecs_id(Velocity), ecs_field_id(&it, 2));
        test_uint(p1, ecs_iter_get_var(&it, parent_var));
        {
            Mass *m = ecs_field(&it, Mass, 1);
            test_assert(m != NULL);
            test_int(*m, 200);
        }
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 12);
            test_int(p->y, 22);
        }
        {
            Velocity *v = ecs_field(&it, Velocity, 2);
            test_assert(v != NULL);
            test_int(v->x, 2);
            test_int(v->y, 2);
        }
        test_bool(false, ecs_query_next(&it));
    }

    {
        ecs_iter_t it = ecs_query_iter(ecs, q);
        ecs_iter_set_var(&it, 0, e3);
        test_bool(true, ecs_query_next(&it));
        test_int(it.count, 1);
        test_uint(it.entities[0], e3);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(ecs_pair_t(Mass, p2), ecs_field_id(&it, 1));
        test_uint(ecs_id(Velocity), ecs_field_id(&it, 2));
        test_uint(p2, ecs_iter_get_var(&it, parent_var));
        {
            Mass *m = ecs_field(&it, Mass, 1);
            test_assert(m != NULL);
            test_int(*m, 300);
        }
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 13);
            test_int(p->y, 23);
        }
        {
            Velocity *v = ecs_field(&it, Velocity, 2);
            test_assert(v != NULL);
            test_int(v->x, 3);
            test_int(v->y, 3);
        }
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(ecs);
}

void Variables_3_set_src_this_w_tag_tag_uncacheable_component(void) {
    ecs_world_t* ecs = ecs_mini();

    ECS_TAG(ecs, TagA);
    ECS_TAG(ecs, TagB);
    ECS_COMPONENT(ecs, Mass);

    ecs_entity_t p1 = ecs_new(ecs);
    ecs_entity_t p2 = ecs_new(ecs);
    ecs_entity_t e1 = ecs_new_w(ecs, TagA);
    ecs_entity_t e2 = ecs_new_w(ecs, TagA);
    ecs_entity_t e3 = ecs_new_w(ecs, TagA);
    ecs_add(ecs, e1, TagB);
    ecs_add(ecs, e2, TagB);
    ecs_add(ecs, e3, TagB);
    ecs_set_pair(ecs, e1, Mass, p1, {100});
    ecs_set_pair(ecs, e2, Mass, p1, {200});
    ecs_set_pair(ecs, e3, Mass, p2, {300});

    ecs_query_t *q = ecs_query(ecs, {
        .terms = {
            { TagA },
            { TagB },
            { .first.id = ecs_id(Mass), .second.name = "$parent" }
        },
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int32_t parent_var = ecs_query_find_var(q, "parent");
    test_assert(parent_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(ecs, q);
        ecs_iter_set_var(&it, 0, e1);
        test_bool(true, ecs_query_next(&it));
        test_int(it.count, 1);
        test_uint(it.entities[0], e1);
        test_uint(TagA, ecs_field_id(&it, 0));
        test_uint(TagB, ecs_field_id(&it, 1));
        test_uint(ecs_pair_t(Mass, p1), ecs_field_id(&it, 2));
        test_uint(p1, ecs_iter_get_var(&it, parent_var));
        {
            Mass *m = ecs_field(&it, Mass, 2);
            test_assert(m != NULL);
            test_int(*m, 100);
        }
        test_bool(false, ecs_query_next(&it));
    }

    {
        ecs_iter_t it = ecs_query_iter(ecs, q);
        ecs_iter_set_var(&it, 0, e2);
        test_bool(true, ecs_query_next(&it));
        test_int(it.count, 1);
        test_uint(it.entities[0], e2);
        test_uint(TagA, ecs_field_id(&it, 0));
        test_uint(TagB, ecs_field_id(&it, 1));
        test_uint(ecs_pair_t(Mass, p1), ecs_field_id(&it, 2));
        test_uint(p1, ecs_iter_get_var(&it, parent_var));
        {
            Mass *m = ecs_field(&it, Mass, 2);
            test_assert(m != NULL);
            test_int(*m, 200);
        }
        test_bool(false, ecs_query_next(&it));
    }

    {
        ecs_iter_t it = ecs_query_iter(ecs, q);
        ecs_iter_set_var(&it, 0, e3);
        test_bool(true, ecs_query_next(&it));
        test_int(it.count, 1);
        test_uint(it.entities[0], e3);
        test_uint(TagA, ecs_field_id(&it, 0));
        test_uint(TagB, ecs_field_id(&it, 1));
        test_uint(ecs_pair_t(Mass, p2), ecs_field_id(&it, 2));
        test_uint(p2, ecs_iter_get_var(&it, parent_var));
        {
            Mass *m = ecs_field(&it, Mass, 2);
            test_assert(m != NULL);
            test_int(*m, 300);
        }
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(ecs);
}

void Variables_3_set_src_this_w_component_tag_uncacheable_component(void) {
    ecs_world_t* ecs = ecs_mini();

    ECS_COMPONENT(ecs, Mass);
    ECS_COMPONENT(ecs, Position);
    ECS_TAG(ecs, TagB);

    ecs_entity_t p1 = ecs_new(ecs);
    ecs_entity_t p2 = ecs_new(ecs);
    ecs_entity_t e1 = ecs_insert(ecs, ecs_value(Position, {11, 21}));
    ecs_entity_t e2 = ecs_insert(ecs, ecs_value(Position, {12, 22}));
    ecs_entity_t e3 = ecs_insert(ecs, ecs_value(Position, {13, 23}));
    ecs_add(ecs, e1, TagB);
    ecs_add(ecs, e2, TagB);
    ecs_add(ecs, e3, TagB);
    ecs_set_pair(ecs, e1, Mass, p1, {100});
    ecs_set_pair(ecs, e2, Mass, p1, {200});
    ecs_set_pair(ecs, e3, Mass, p2, {300});

    ecs_query_t *q = ecs_query(ecs, {
        .terms = {
            { ecs_id(Position) },
            { TagB },
            { .first.id = ecs_id(Mass), .second.name = "$parent" }
        },
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int32_t parent_var = ecs_query_find_var(q, "parent");
    test_assert(parent_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(ecs, q);
        ecs_iter_set_var(&it, 0, e1);
        test_bool(true, ecs_query_next(&it));
        test_int(it.count, 1);
        test_uint(it.entities[0], e1);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(TagB, ecs_field_id(&it, 1));
        test_uint(ecs_pair_t(Mass, p1), ecs_field_id(&it, 2));
        test_uint(p1, ecs_iter_get_var(&it, parent_var));
        {
            Mass *m = ecs_field(&it, Mass, 2);
            test_assert(m != NULL);
            test_int(*m, 100);
        }
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 11);
            test_int(p->y, 21);
        }
        test_bool(false, ecs_query_next(&it));
    }

    {
        ecs_iter_t it = ecs_query_iter(ecs, q);
        ecs_iter_set_var(&it, 0, e2);
        test_bool(true, ecs_query_next(&it));
        test_int(it.count, 1);
        test_uint(it.entities[0], e2);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(TagB, ecs_field_id(&it, 1));
        test_uint(ecs_pair_t(Mass, p1), ecs_field_id(&it, 2));
        test_uint(p1, ecs_iter_get_var(&it, parent_var));
        {
            Mass *m = ecs_field(&it, Mass, 2);
            test_assert(m != NULL);
            test_int(*m, 200);
        }
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 12);
            test_int(p->y, 22);
        }
        test_bool(false, ecs_query_next(&it));
    }

    {
        ecs_iter_t it = ecs_query_iter(ecs, q);
        ecs_iter_set_var(&it, 0, e3);
        test_bool(true, ecs_query_next(&it));
        test_int(it.count, 1);
        test_uint(it.entities[0], e3);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(TagB, ecs_field_id(&it, 1));
        test_uint(ecs_pair_t(Mass, p2), ecs_field_id(&it, 2));
        test_uint(p2, ecs_iter_get_var(&it, parent_var));
        {
            Mass *m = ecs_field(&it, Mass, 2);
            test_assert(m != NULL);
            test_int(*m, 300);
        }
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 13);
            test_int(p->y, 23);
        }
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(ecs);
}

void Variables_3_set_src_this_w_component_component_uncacheable_component(void) {
    ecs_world_t* ecs = ecs_mini();

    ECS_COMPONENT(ecs, Mass);
    ECS_COMPONENT(ecs, Position);
    ECS_COMPONENT(ecs, Velocity);

    ecs_entity_t p1 = ecs_new(ecs);
    ecs_entity_t p2 = ecs_new(ecs);
    ecs_entity_t e1 = ecs_insert(ecs, ecs_value(Position, {11, 21}));
    ecs_entity_t e2 = ecs_insert(ecs, ecs_value(Position, {12, 22}));
    ecs_entity_t e3 = ecs_insert(ecs, ecs_value(Position, {13, 23}));
    ecs_set(ecs, e1, Velocity, {1, 1});
    ecs_set(ecs, e2, Velocity, {2, 2});
    ecs_set(ecs, e3, Velocity, {3, 3});
    ecs_set_pair(ecs, e1, Mass, p1, {100});
    ecs_set_pair(ecs, e2, Mass, p1, {200});
    ecs_set_pair(ecs, e3, Mass, p2, {300});

    ecs_query_t *q = ecs_query(ecs, {
        .terms = {
            { ecs_id(Position) },
            { ecs_id(Velocity) },
            { .first.id = ecs_id(Mass), .second.name = "$parent" }
        },
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int32_t parent_var = ecs_query_find_var(q, "parent");
    test_assert(parent_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(ecs, q);
        ecs_iter_set_var(&it, 0, e1);
        test_bool(true, ecs_query_next(&it));
        test_int(it.count, 1);
        test_uint(it.entities[0], e1);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(ecs_id(Velocity), ecs_field_id(&it, 1));
        test_uint(ecs_pair_t(Mass, p1), ecs_field_id(&it, 2));
        test_uint(p1, ecs_iter_get_var(&it, parent_var));
        {
            Mass *m = ecs_field(&it, Mass, 2);
            test_assert(m != NULL);
            test_int(*m, 100);
        }
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 11);
            test_int(p->y, 21);
        }
        {
            Velocity *v = ecs_field(&it, Velocity, 1);
            test_assert(v != NULL);
            test_int(v->x, 1);
            test_int(v->y, 1);
        }
        test_bool(false, ecs_query_next(&it));
    }

    {
        ecs_iter_t it = ecs_query_iter(ecs, q);
        ecs_iter_set_var(&it, 0, e2);
        test_bool(true, ecs_query_next(&it));
        test_int(it.count, 1);
        test_uint(it.entities[0], e2);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(ecs_id(Velocity), ecs_field_id(&it, 1));
        test_uint(ecs_pair_t(Mass, p1), ecs_field_id(&it, 2));
        test_uint(p1, ecs_iter_get_var(&it, parent_var));
        {
            Mass *m = ecs_field(&it, Mass, 2);
            test_assert(m != NULL);
            test_int(*m, 200);
        }
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 12);
            test_int(p->y, 22);
        }
        {
            Velocity *v = ecs_field(&it, Velocity, 1);
            test_assert(v != NULL);
            test_int(v->x, 2);
            test_int(v->y, 2);
        }
        test_bool(false, ecs_query_next(&it));
    }

    {
        ecs_iter_t it = ecs_query_iter(ecs, q);
        ecs_iter_set_var(&it, 0, e3);
        test_bool(true, ecs_query_next(&it));
        test_int(it.count, 1);
        test_uint(it.entities[0], e3);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(ecs_id(Velocity), ecs_field_id(&it, 1));
        test_uint(ecs_pair_t(Mass, p2), ecs_field_id(&it, 2));
        test_uint(p2, ecs_iter_get_var(&it, parent_var));
        {
            Mass *m = ecs_field(&it, Mass, 2);
            test_assert(m != NULL);
            test_int(*m, 300);
        }
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 13);
            test_int(p->y, 23);
        }
        {
            Velocity *v = ecs_field(&it, Velocity, 1);
            test_assert(v != NULL);
            test_int(v->x, 3);
            test_int(v->y, 3);
        }
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(ecs);
}

void Variables_1_src_this_var_as_entity(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, TagC);

    ecs_entity_t e1 = ecs_new_w(world, TagA);
    ecs_entity_t e2 = ecs_new_w(world, TagA);
    ecs_entity_t e3 = ecs_new_w(world, TagA);
    ecs_entity_t e4 = ecs_new_w(world, TagA);
    ecs_add(world, e2, TagB);
    ecs_add(world, e3, TagC);
    ecs_add(world, e4, TagC);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ TagA }},
        .cache_kind = cache_kind
    });

    int this_var_id = ecs_query_find_var(q, "this");
    test_assert(this_var_id != -1);

    ecs_iter_t it = ecs_query_iter(world, q);

    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], e1);
    ecs_entity_t this_var = ecs_iter_get_var(&it, this_var_id);
    test_assert(this_var != 0);
    test_assert(this_var == e1);

    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], e2);
    this_var = ecs_iter_get_var(&it, this_var_id);
    test_assert(this_var != 0);
    test_assert(this_var == e2);

    test_assert(ecs_query_next(&it));
    test_int(it.count, 2);
    test_uint(it.entities[0], e3);
    test_uint(it.entities[1], e4);
    this_var = ecs_iter_get_var(&it, this_var_id);
    test_assert(this_var == 0); /* more than one entity matches */

    test_assert(!ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_1_src_this_var_as_table(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, TagC);

    ecs_entity_t e1 = ecs_new_w(world, TagA);
    ecs_entity_t e2 = ecs_new_w(world, TagA);
    ecs_entity_t e3 = ecs_new_w(world, TagA);
    ecs_entity_t e4 = ecs_new_w(world, TagA);
    ecs_add(world, e2, TagB);
    ecs_add(world, e3, TagC);
    ecs_add(world, e4, TagC);

    ecs_table_t *t1 = ecs_get_table(world, e1);
    test_assert(t1 != NULL);

    ecs_table_t *t2 = ecs_get_table(world, e2);
    test_assert(t2 != NULL);
    test_assert(t2 != t1);

    ecs_table_t *t3 = ecs_get_table(world, e3);
    test_assert(t3 != NULL);
    test_assert(t3 != t1);
    test_assert(t3 != t2);
    test_assert(t3 == ecs_get_table(world, e4));

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ TagA }},
        .cache_kind = cache_kind
    });

    int this_var_id = ecs_query_find_var(q, "this");
    test_assert(this_var_id != -1);

    ecs_iter_t it = ecs_query_iter(world, q);

    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], e1);
    ecs_table_t *this_var = ecs_iter_get_var_as_table(&it, this_var_id);
    test_assert(this_var != NULL);
    test_assert(this_var == t1);

    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], e2);
    this_var = ecs_iter_get_var_as_table(&it, this_var_id);
    test_assert(this_var != NULL);
    test_assert(this_var == t2);

    test_assert(ecs_query_next(&it));
    test_int(it.count, 2);
    test_uint(it.entities[0], e3);
    test_uint(it.entities[1], e4);
    this_var = ecs_iter_get_var_as_table(&it, this_var_id);
    test_assert(this_var != NULL);
    test_assert(this_var == t3);

    test_assert(!ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_1_src_this_var_as_table_range(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, TagC);

    ecs_entity_t e1 = ecs_new_w(world, TagA);
    ecs_entity_t e2 = ecs_new_w(world, TagA);
    ecs_entity_t e3 = ecs_new_w(world, TagA);
    ecs_entity_t e4 = ecs_new_w(world, TagA);
    ecs_add(world, e2, TagB);
    ecs_add(world, e3, TagC);
    ecs_add(world, e4, TagC);

    ecs_table_t *t1 = ecs_get_table(world, e1);
    test_assert(t1 != NULL);

    ecs_table_t *t2 = ecs_get_table(world, e2);
    test_assert(t2 != NULL);
    test_assert(t2 != t1);

    ecs_table_t *t3 = ecs_get_table(world, e3);
    test_assert(t3 != NULL);
    test_assert(t3 != t1);
    test_assert(t3 != t2);
    test_assert(t3 == ecs_get_table(world, e4));

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ TagA }},
        .cache_kind = cache_kind
    });

    int this_var_id = ecs_query_find_var(q, "this");
    test_assert(this_var_id != -1);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], e1);
    ecs_table_range_t this_var = ecs_iter_get_var_as_range(&it, this_var_id);
    test_assert(this_var.table != NULL);
    test_assert(this_var.table == t1);
    test_assert(this_var.offset == 0);
    test_assert(this_var.count == 1);

    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], e2);
    this_var = ecs_iter_get_var_as_range(&it, this_var_id);
    test_assert(this_var.table != NULL);
    test_assert(this_var.table == t2);
    test_assert(this_var.offset == 0);
    test_assert(this_var.count == 1);

    test_assert(ecs_query_next(&it));
    test_int(it.count, 2);
    test_uint(it.entities[0], e3);
    test_uint(it.entities[1], e4);
    this_var = ecs_iter_get_var_as_range(&it, this_var_id);
    test_assert(this_var.table != NULL);
    test_assert(this_var.table == t3);
    test_assert(this_var.offset == 0);
    test_assert(this_var.count == 2);

    test_assert(!ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_1_set_src_this_w_any(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_entity_t e = ecs_new(world);
    ecs_add(world, e, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ EcsAny }}
    });

    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e, it.entities[0]);
        test_uint(EcsWildcard, ecs_field_id(&it, 0));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_1_set_src_this_w_any_fixed(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_entity_t e = ecs_new(world);
    ecs_add(world, e, Foo);

    ecs_entity_t f = ecs_new(world);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { EcsAny },
            { Bar, .src.id = f }
        }
    });

    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add(world, f, Bar);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e, it.entities[0]);
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(f, ecs_field_src(&it, 1));
        test_uint(EcsWildcard, ecs_field_id(&it, 0));
        test_uint(Bar, ecs_field_id(&it, 1));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_1_set_src_this_w_fixed_any(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_entity_t e = ecs_new(world);
    ecs_add(world, e, Foo);

    ecs_entity_t f = ecs_new(world);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { Bar, .src.id = f },
            { EcsAny }
        }
    });

    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add(world, f, Bar);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e, it.entities[0]);
        test_uint(f, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(Bar, ecs_field_id(&it, 0));
        test_uint(EcsWildcard, ecs_field_id(&it, 1));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_2_join_by_rel_var(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, RelC);

    ecs_entity_t e1 = ecs_new_w(world, RelA);
    ecs_add(world, e1, RelB);
    ecs_add_pair(world, e1, TagA, TagB);

    ecs_entity_t e2 = ecs_new_w(world, RelA);
    ecs_entity_t e3 = ecs_new_w(world, RelB);
    ecs_new_w(world, RelC);

    ecs_query_t *q = ecs_query(world, {
        .expr = "$x($this), $x($y), TagA($this, TagB)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    int y_var = ecs_query_find_var(q, "y");
    test_assert(y_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(ecs_pair(TagA, TagB), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(e2, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(RelA, ecs_iter_get_var(&it, x_var));
        test_uint(e2, ecs_iter_get_var(&it, y_var));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(ecs_pair(TagA, TagB), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(RelA, ecs_iter_get_var(&it, x_var));
        test_uint(e1, ecs_iter_get_var(&it, y_var));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(RelB, ecs_field_id(&it, 0));
        test_uint(RelB, ecs_field_id(&it, 1));
        test_uint(ecs_pair(TagA, TagB), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(RelB, ecs_iter_get_var(&it, x_var));
        test_uint(e1, ecs_iter_get_var(&it, y_var));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(RelB, ecs_field_id(&it, 0));
        test_uint(RelB, ecs_field_id(&it, 1));
        test_uint(ecs_pair(TagA, TagB), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(e3, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(RelB, ecs_iter_get_var(&it, x_var));
        test_uint(e3, ecs_iter_get_var(&it, y_var));
        test_uint(e1, it.entities[0]);

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_2_join_by_pair_rel_var(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, RelC);
    ECS_TAG(world, TgtA);

    ecs_entity_t e1 = ecs_new_w_pair(world, RelA, TgtA);
    ecs_add_pair(world, e1, RelB, TgtA);
    ecs_add(world, e1, TagA);

    ecs_entity_t e2 = ecs_new_w_pair(world, RelA, TgtA);
    ecs_entity_t e3 = ecs_new_w_pair(world, RelB, TgtA);
    ecs_new_w_pair(world, RelC, TgtA);

    ecs_query_t *q = ecs_query(world, {
        .expr = "$x($this, TgtA), $x($y, TgtA), TagA",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    int y_var = ecs_query_find_var(q, "y");
    test_assert(y_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 1));
        test_uint(TagA, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(e2, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(RelA, ecs_iter_get_var(&it, x_var));
        test_uint(e2, ecs_iter_get_var(&it, y_var));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 1));
        test_uint(TagA, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(RelA, ecs_iter_get_var(&it, x_var));
        test_uint(e1, ecs_iter_get_var(&it, y_var));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(RelB, TgtA), ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelB, TgtA), ecs_field_id(&it, 1));
        test_uint(TagA, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(RelB, ecs_iter_get_var(&it, x_var));
        test_uint(e1, ecs_iter_get_var(&it, y_var));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(RelB, TgtA), ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelB, TgtA), ecs_field_id(&it, 1));
        test_uint(TagA, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(e3, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(RelB, ecs_iter_get_var(&it, x_var));
        test_uint(e3, ecs_iter_get_var(&it, y_var));
        test_uint(e1, it.entities[0]);

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_2_join_by_pair_tgt_var(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, RelA);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);
    ECS_TAG(world, TgtC);

    ecs_entity_t e1 = ecs_new_w_pair(world, RelA, TgtA);
    ecs_add_pair(world, e1, RelA, TgtB);
    ecs_add(world, e1, TagA);

    ecs_entity_t e2 = ecs_new_w_pair(world, RelA, TgtA);
    ecs_entity_t e3 = ecs_new_w_pair(world, RelA, TgtB);
    ecs_new_w_pair(world, RelA, TgtC);

    ecs_query_t *q = ecs_query(world, {
        .expr = "RelA($this, $x), RelA($y, $x), TagA",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    int y_var = ecs_query_find_var(q, "y");
    test_assert(y_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 0));
        test_uint(TagA, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(e2, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(TgtA, ecs_iter_get_var(&it, x_var));
        test_uint(e2, ecs_iter_get_var(&it, y_var));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 1));
        test_uint(TagA, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(TgtA, ecs_iter_get_var(&it, x_var));
        test_uint(e1, ecs_iter_get_var(&it, y_var));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(RelA, TgtB), ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelA, TgtB), ecs_field_id(&it, 1));
        test_uint(TagA, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(TgtB, ecs_iter_get_var(&it, x_var));
        test_uint(e1, ecs_iter_get_var(&it, y_var));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(RelA, TgtB), ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelA, TgtB), ecs_field_id(&it, 1));
        test_uint(TagA, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(e3, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(TgtB, ecs_iter_get_var(&it, x_var));
        test_uint(e3, ecs_iter_get_var(&it, y_var));
        test_uint(e1, it.entities[0]);

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_2_cycle_w_var(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);

    ecs_entity_t e1 = ecs_new_w(world, TagA);
    ecs_entity_t e2 = ecs_new_w(world, TagA);

    ecs_query_t *q = ecs_query(world, {
        .expr = "$x($y), $y($x), TagA($x)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    int y_var = ecs_query_find_var(q, "y");
    test_assert(y_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_id(world, e1, e2);
    ecs_add_id(world, e2, e1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);

        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(e2, ecs_field_id(&it, 0));
        test_uint(e1, ecs_field_id(&it, 1));
        test_uint(TagA, ecs_field_id(&it, 2));
        test_uint(e1, ecs_field_src(&it, 0));
        test_uint(e2, ecs_field_src(&it, 1));
        test_uint(e2, ecs_field_src(&it, 2));
        test_uint(e2, ecs_iter_get_var(&it, x_var));
        test_uint(e1, ecs_iter_get_var(&it, y_var));

        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(e1, ecs_field_id(&it, 0));
        test_uint(e2, ecs_field_id(&it, 1));
        test_uint(TagA, ecs_field_id(&it, 2));
        test_uint(e2, ecs_field_src(&it, 0));
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(e1, ecs_field_src(&it, 2));
        test_uint(e1, ecs_iter_get_var(&it, x_var));
        test_uint(e2, ecs_iter_get_var(&it, y_var));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_2_cycle_w_this_var(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);

    ecs_entity_t e1 = ecs_new_w(world, TagA);
    ecs_entity_t e2 = ecs_new_w(world, TagA);

    ecs_query_t *q = ecs_query(world, {
        .expr = "$this($y), $y($this), TagA($this)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int this_var = ecs_query_find_var(q, "this");
    test_assert(this_var != -1);

    int y_var = ecs_query_find_var(q, "y");
    test_assert(y_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    if (cache_kind == EcsQueryCacheDefault) {
        ecs_add_id(world, e1, e2);
        ecs_add_id(world, e2, e1);
    } else {
        /* Cached query reverses the result */
        ecs_add_id(world, e2, e1);
        ecs_add_id(world, e1, e2);
    }

    {
        ecs_iter_t it = ecs_query_iter(world, q);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(e2, ecs_field_id(&it, 0));
        test_uint(e1, ecs_field_id(&it, 1));
        test_uint(TagA, ecs_field_id(&it, 2));
        test_uint(e1, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(e2, ecs_iter_get_var(&it, this_var));
        test_uint(e1, ecs_iter_get_var(&it, y_var));
        test_uint(e2, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(e1, ecs_field_id(&it, 0));
        test_uint(e2, ecs_field_id(&it, 1));
        test_uint(TagA, ecs_field_id(&it, 2));
        test_uint(e2, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(e1, ecs_iter_get_var(&it, this_var));
        test_uint(e2, ecs_iter_get_var(&it, y_var));
        test_uint(e1, it.entities[0]);

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_2_cycle_w_var_this(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);

    ecs_entity_t e1 = ecs_new_w(world, TagA);
    ecs_entity_t e2 = ecs_new_w(world, TagA);

    ecs_query_t *q = ecs_query(world, {
        .expr = "$x($this), $this($x), TagA($this)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int this_var = ecs_query_find_var(q, "this");
    test_assert(this_var != -1);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_id(world, e1, e2);
    ecs_add_id(world, e2, e1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(e2, ecs_field_id(&it, 0));
        test_uint(e1, ecs_field_id(&it, 1));
        test_uint(TagA, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(e2, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(e2, ecs_iter_get_var(&it, x_var));
        test_uint(e1, ecs_iter_get_var(&it, this_var));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(e1, ecs_field_id(&it, 0));
        test_uint(e2, ecs_field_id(&it, 1));
        test_uint(TagA, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(e1, ecs_iter_get_var(&it, x_var));
        test_uint(e2, ecs_iter_get_var(&it, this_var));
        test_uint(e2, it.entities[0]);

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_2_cycle_pair_w_var(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);

    ecs_entity_t e1 = ecs_new_w(world, TagA);
    ecs_entity_t e2 = ecs_new_w(world, TagA);
    ecs_entity_t e3 = ecs_new_w(world, TagA);

    ecs_query_t *q = ecs_query(world, {
        .expr = "$x($y, $z), $x($z, $y), TagA($y)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    int y_var = ecs_query_find_var(q, "y");
    test_assert(y_var != -1);

    int z_var = ecs_query_find_var(q, "z");
    test_assert(z_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_pair(world, e2, e1, e3);
    ecs_add_pair(world, e3, e1, e2);

    {
        ecs_iter_t it = ecs_query_iter(world, q);

        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(e1, e3), ecs_field_id(&it, 0));
        test_uint(ecs_pair(e1, e2), ecs_field_id(&it, 1));
        test_uint(TagA, ecs_field_id(&it, 2));
        test_uint(e2, ecs_field_src(&it, 0));
        test_uint(e3, ecs_field_src(&it, 1));
        test_uint(e2, ecs_field_src(&it, 2));
        test_uint(e1, ecs_iter_get_var(&it, x_var));
        test_uint(e2, ecs_iter_get_var(&it, y_var));
        test_uint(e3, ecs_iter_get_var(&it, z_var));

        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(e1, e2), ecs_field_id(&it, 0));
        test_uint(ecs_pair(e1, e3), ecs_field_id(&it, 1));
        test_uint(TagA, ecs_field_id(&it, 2));
        test_uint(e3, ecs_field_src(&it, 0));
        test_uint(e2, ecs_field_src(&it, 1));
        test_uint(e3, ecs_field_src(&it, 2));
        test_uint(e1, ecs_iter_get_var(&it, x_var));
        test_uint(e3, ecs_iter_get_var(&it, y_var));
        test_uint(e2, ecs_iter_get_var(&it, z_var));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_2_cycle_pair_w_this_var_var(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);

    ecs_entity_t e1 = ecs_new_w(world, TagA);
    ecs_entity_t e2 = ecs_new_w(world, TagA);
    ecs_entity_t e3 = ecs_new_w(world, TagA);

    ecs_query_t *q = ecs_query(world, {
        .expr = "$this($y, $z), $this($z, $y), TagA($y)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int this_var = ecs_query_find_var(q, "this");
    test_assert(this_var != -1);

    int y_var = ecs_query_find_var(q, "y");
    test_assert(y_var != -1);

    int z_var = ecs_query_find_var(q, "z");
    test_assert(z_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_pair(world, e2, e1, e3);
    ecs_add_pair(world, e3, e1, e2);

    {
        ecs_iter_t it = ecs_query_iter(world, q);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(e1, e3), ecs_field_id(&it, 0));
        test_uint(ecs_pair(e1, e2), ecs_field_id(&it, 1));
        test_uint(TagA, ecs_field_id(&it, 2));
        test_uint(e2, ecs_field_src(&it, 0));
        test_uint(e3, ecs_field_src(&it, 1));
        test_uint(e2, ecs_field_src(&it, 2));
        test_uint(e1, ecs_iter_get_var(&it, this_var));
        test_uint(e2, ecs_iter_get_var(&it, y_var));
        test_uint(e3, ecs_iter_get_var(&it, z_var));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(e1, e2), ecs_field_id(&it, 0));
        test_uint(ecs_pair(e1, e3), ecs_field_id(&it, 1));
        test_uint(TagA, ecs_field_id(&it, 2));
        test_uint(e3, ecs_field_src(&it, 0));
        test_uint(e2, ecs_field_src(&it, 1));
        test_uint(e3, ecs_field_src(&it, 2));
        test_uint(e1, ecs_iter_get_var(&it, this_var));
        test_uint(e3, ecs_iter_get_var(&it, y_var));
        test_uint(e2, ecs_iter_get_var(&it, z_var));
        test_uint(e1, it.entities[0]);

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_2_cycle_pair_w_var_this_var(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);

    ecs_entity_t e1 = ecs_new_w(world, TagA);
    ecs_entity_t e2 = ecs_new_w(world, TagA);
    ecs_entity_t e3 = ecs_new_w(world, TagA);

    ecs_query_t *q = ecs_query(world, {
        .expr = "$x($this, $z), $x($z, $this), TagA($this)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    int this_var = ecs_query_find_var(q, "this");
    test_assert(this_var != -1);

    int z_var = ecs_query_find_var(q, "z");
    test_assert(z_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_pair(world, e2, e1, e3);
    ecs_add_pair(world, e3, e1, e2);

    {
        ecs_iter_t it = ecs_query_iter(world, q);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(e1, e3), ecs_field_id(&it, 0));
        test_uint(ecs_pair(e1, e2), ecs_field_id(&it, 1));
        test_uint(TagA, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(e3, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(e1, ecs_iter_get_var(&it, x_var));
        test_uint(e2, ecs_iter_get_var(&it, this_var));
        test_uint(e3, ecs_iter_get_var(&it, z_var));
        test_uint(e2, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(e1, e2), ecs_field_id(&it, 0));
        test_uint(ecs_pair(e1, e3), ecs_field_id(&it, 1));
        test_uint(TagA, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(e2, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(e1, ecs_iter_get_var(&it, x_var));
        test_uint(e3, ecs_iter_get_var(&it, this_var));
        test_uint(e2, ecs_iter_get_var(&it, z_var));
        test_uint(e3, it.entities[0]);

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_2_cycle_pair_w_var_var_this(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);

    ecs_entity_t e1 = ecs_new_w(world, TagA);
    ecs_entity_t e2 = ecs_new_w(world, TagA);
    ecs_entity_t e3 = ecs_new_w(world, TagA);

    ecs_query_t *q = ecs_query(world, {
        .expr = "$x($y, $this), $x($this, $y), TagA($y)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    int y_var = ecs_query_find_var(q, "y");
    test_assert(y_var != -1);

    int this_var = ecs_query_find_var(q, "this");
    test_assert(this_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_pair(world, e2, e1, e3);
    ecs_add_pair(world, e3, e1, e2);

    {
        ecs_iter_t it = ecs_query_iter(world, q);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(e1, e3), ecs_field_id(&it, 0));
        test_uint(ecs_pair(e1, e2), ecs_field_id(&it, 1));
        test_uint(TagA, ecs_field_id(&it, 2));
        test_uint(e2, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(e2, ecs_field_src(&it, 2));
        test_uint(e1, ecs_iter_get_var(&it, x_var));
        test_uint(e2, ecs_iter_get_var(&it, y_var));
        test_uint(e3, ecs_iter_get_var(&it, this_var));
        test_uint(e3, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(e1, e2), ecs_field_id(&it, 0));
        test_uint(ecs_pair(e1, e3), ecs_field_id(&it, 1));
        test_uint(TagA, ecs_field_id(&it, 2));
        test_uint(e3, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(e3, ecs_field_src(&it, 2));
        test_uint(e1, ecs_iter_get_var(&it, x_var));
        test_uint(e3, ecs_iter_get_var(&it, y_var));
        test_uint(e2, ecs_iter_get_var(&it, this_var));
        test_uint(e2, it.entities[0]);

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_2_cycle_pair_ent_var_var(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Likes);

    ecs_entity_t e1 = ecs_entity(world, { .name = "e1" });
    ecs_entity_t e2 = ecs_entity(world, { .name = "e2" });
    ecs_entity_t e3 = ecs_entity(world, { .name = "e3" });

    ecs_query_t *q = ecs_query(world, {
        .expr = "Likes($x, $y), Likes($y, $x)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    int y_var = ecs_query_find_var(q, "y");
    test_assert(y_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_pair(world, e1, Likes, e2);
    ecs_add_pair(world, e1, Likes, e1);
    ecs_add_pair(world, e2, Likes, e1);
    ecs_add_pair(world, e2, Likes, e3);
    ecs_add_pair(world, e3, Likes, e1);
    ecs_add_pair(world, e3, Likes, e2);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(Likes, e1), ecs_field_id(&it, 0));
        test_uint(ecs_pair(Likes, e1), ecs_field_id(&it, 1));
        test_uint(e1, ecs_field_src(&it, 0));
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(e1, ecs_iter_get_var(&it, x_var));
        test_uint(e1, ecs_iter_get_var(&it, y_var));

        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(Likes, e2), ecs_field_id(&it, 0));
        test_uint(ecs_pair(Likes, e1), ecs_field_id(&it, 1));
        test_uint(e1, ecs_field_src(&it, 0));
        test_uint(e2, ecs_field_src(&it, 1));
        test_uint(e1, ecs_iter_get_var(&it, x_var));
        test_uint(e2, ecs_iter_get_var(&it, y_var));

        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(Likes, e2), ecs_field_id(&it, 0));
        test_uint(ecs_pair(Likes, e3), ecs_field_id(&it, 1));
        test_uint(e3, ecs_field_src(&it, 0));
        test_uint(e2, ecs_field_src(&it, 1));
        test_uint(e3, ecs_iter_get_var(&it, x_var));
        test_uint(e2, ecs_iter_get_var(&it, y_var));

        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(Likes, e1), ecs_field_id(&it, 0));
        test_uint(ecs_pair(Likes, e2), ecs_field_id(&it, 1));
        test_uint(e2, ecs_field_src(&it, 0));
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(e2, ecs_iter_get_var(&it, x_var));
        test_uint(e1, ecs_iter_get_var(&it, y_var));

        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(Likes, e3), ecs_field_id(&it, 0));
        test_uint(ecs_pair(Likes, e2), ecs_field_id(&it, 1));
        test_uint(e2, ecs_field_src(&it, 0));
        test_uint(e3, ecs_field_src(&it, 1));
        test_uint(e2, ecs_iter_get_var(&it, x_var));
        test_uint(e3, ecs_iter_get_var(&it, y_var));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_2_cycle_pair_ent_this_var(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Likes);

    ecs_entity_t e1 = ecs_entity(world, { .name = "e1" });
    ecs_entity_t e2 = ecs_entity(world, { .name = "e2" });
    ecs_entity_t e3 = ecs_entity(world, { .name = "e3" });

    ecs_query_t *q = ecs_query(world, {
        .expr = "Likes($this, $y), Likes($y, $this)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int this_var = ecs_query_find_var(q, "this");
    test_assert(this_var != -1);
    test_assert(this_var != 0);

    int y_var = ecs_query_find_var(q, "y");
    test_assert(y_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_pair(world, e1, Likes, e2);
    ecs_add_pair(world, e1, Likes, e1);
    ecs_add_pair(world, e2, Likes, e1);
    ecs_add_pair(world, e2, Likes, e3);
    ecs_add_pair(world, e3, Likes, e1);
    ecs_add_pair(world, e3, Likes, e2);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(Likes, e1), ecs_field_id(&it, 0));
        test_uint(ecs_pair(Likes, e1), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(e1, ecs_iter_get_var(&it, this_var));
        test_uint(e1, ecs_iter_get_var(&it, y_var));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(Likes, e2), ecs_field_id(&it, 0));
        test_uint(ecs_pair(Likes, e1), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(e2, ecs_field_src(&it, 1));
        test_uint(e1, ecs_iter_get_var(&it, this_var));
        test_uint(e2, ecs_iter_get_var(&it, y_var));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(Likes, e2), ecs_field_id(&it, 0));
        test_uint(ecs_pair(Likes, e3), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(e2, ecs_field_src(&it, 1));
        test_uint(e3, ecs_iter_get_var(&it, this_var));
        test_uint(e2, ecs_iter_get_var(&it, y_var));
        test_uint(e3, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(Likes, e1), ecs_field_id(&it, 0));
        test_uint(ecs_pair(Likes, e2), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(e2, ecs_iter_get_var(&it, this_var));
        test_uint(e1, ecs_iter_get_var(&it, y_var));
        test_uint(e2, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(Likes, e3), ecs_field_id(&it, 0));
        test_uint(ecs_pair(Likes, e2), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(e3, ecs_field_src(&it, 1));
        test_uint(e2, ecs_iter_get_var(&it, this_var));
        test_uint(e3, ecs_iter_get_var(&it, y_var));
        test_uint(e2, it.entities[0]);

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_2_cycle_pair_ent_var_this(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Likes);

    ecs_entity_t e1 = ecs_entity(world, { .name = "e1" });
    ecs_entity_t e2 = ecs_entity(world, { .name = "e2" });
    ecs_entity_t e3 = ecs_entity(world, { .name = "e3" });

    ecs_query_t *q = ecs_query(world, {
        .expr = "Likes($x, $this), Likes($this, $x)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    int this_var = ecs_query_find_var(q, "this");
    test_assert(this_var != -1);
    test_assert(this_var != 0);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_pair(world, e1, Likes, e2);
    ecs_add_pair(world, e1, Likes, e1);
    ecs_add_pair(world, e2, Likes, e1);
    ecs_add_pair(world, e2, Likes, e3);
    ecs_add_pair(world, e3, Likes, e1);
    ecs_add_pair(world, e3, Likes, e2);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(Likes, e1), ecs_field_id(&it, 0));
        test_uint(ecs_pair(Likes, e1), ecs_field_id(&it, 1));
        test_uint(e1, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(e1, ecs_iter_get_var(&it, x_var));
        test_uint(e1, ecs_iter_get_var(&it, this_var));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(Likes, e2), ecs_field_id(&it, 0));
        test_uint(ecs_pair(Likes, e1), ecs_field_id(&it, 1));
        test_uint(e1, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(e1, ecs_iter_get_var(&it, x_var));
        test_uint(e2, ecs_iter_get_var(&it, this_var));
        test_uint(e2, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(Likes, e2), ecs_field_id(&it, 0));
        test_uint(ecs_pair(Likes, e3), ecs_field_id(&it, 1));
        test_uint(e3, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(e3, ecs_iter_get_var(&it, x_var));
        test_uint(e2, ecs_iter_get_var(&it, this_var));
        test_uint(e2, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(Likes, e1), ecs_field_id(&it, 0));
        test_uint(ecs_pair(Likes, e2), ecs_field_id(&it, 1));
        test_uint(e2, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(e2, ecs_iter_get_var(&it, x_var));
        test_uint(e1, ecs_iter_get_var(&it, this_var));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(Likes, e3), ecs_field_id(&it, 0));
        test_uint(ecs_pair(Likes, e2), ecs_field_id(&it, 1));
        test_uint(e2, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(e2, ecs_iter_get_var(&it, x_var));
        test_uint(e3, ecs_iter_get_var(&it, this_var));
        test_uint(e3, it.entities[0]);

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_parse_0_var(void) {
    ecs_world_t *world = ecs_mini();

    ecs_query_t *q = ecs_query(world, {
        .expr = "$x($this, $y)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);
    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);
    int y_var = ecs_query_find_var(q, "y");
    test_assert(y_var != -1);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_str("", ecs_query_args_parse(q, &it, ""));
    test_uint(it.variables[x_var].entity, EcsWildcard);
    test_uint(it.variables[y_var].entity, EcsWildcard);
    ecs_iter_fini(&it);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_parse_1_var(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e1 = ecs_entity(world, { .name = "e1" });

    ecs_query_t *q = ecs_query(world, {
        .expr = "$x($this, $y)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);
    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);
    int y_var = ecs_query_find_var(q, "y");
    test_assert(y_var != -1);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_str("", ecs_query_args_parse(q, &it, "x:e1"));
    test_uint(it.variables[x_var].entity, e1);
    test_uint(it.variables[y_var].entity, EcsWildcard);
    ecs_iter_fini(&it);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_parse_2_vars(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e1 = ecs_entity(world, { .name = "e1" });
    ecs_entity_t e2 = ecs_entity(world, { .name = "e2" });

    ecs_query_t *q = ecs_query(world, {
        .expr = "$x($this, $y)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);
    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);
    int y_var = ecs_query_find_var(q, "y");
    test_assert(y_var != -1);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_str("", ecs_query_args_parse(q, &it, "x:e1,y:e2"));
    test_uint(it.variables[x_var].entity, e1);
    test_uint(it.variables[y_var].entity, e2);
    ecs_iter_fini(&it);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_parse_0_var_paren(void) {
    ecs_world_t *world = ecs_mini();

    ecs_query_t *q = ecs_query(world, {
        .expr = "$x($this, $y)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);
    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);
    int y_var = ecs_query_find_var(q, "y");
    test_assert(y_var != -1);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_str("", ecs_query_args_parse(q, &it, "()"));
    test_uint(it.variables[x_var].entity, EcsWildcard);
    test_uint(it.variables[y_var].entity, EcsWildcard);
    ecs_iter_fini(&it);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_parse_1_var_paren(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e1 = ecs_entity(world, { .name = "e1" });

    ecs_query_t *q = ecs_query(world, {
        .expr = "$x($this, $y)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);
    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);
    int y_var = ecs_query_find_var(q, "y");
    test_assert(y_var != -1);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_str("", ecs_query_args_parse(q, &it, "(x:e1)"));
    test_uint(it.variables[x_var].entity, e1);
    test_uint(it.variables[y_var].entity, EcsWildcard);
    ecs_iter_fini(&it);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_parse_2_vars_paren(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e1 = ecs_entity(world, { .name = "e1" });
    ecs_entity_t e2 = ecs_entity(world, { .name = "e2" });

    ecs_query_t *q = ecs_query(world, {
        .expr = "$x($this, $y)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);
    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);
    int y_var = ecs_query_find_var(q, "y");
    test_assert(y_var != -1);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_str("", ecs_query_args_parse(q, &it, "(x:e1,y:e2)"));
    test_uint(it.variables[x_var].entity, e1);
    test_uint(it.variables[y_var].entity, e2);
    ecs_iter_fini(&it);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_parse_1_vars_w_path(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e1 = ecs_entity(world, { .name = "parent.e1" });

    ecs_query_t *q = ecs_query(world, {
        .expr = "$x($this, $y)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);
    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);
    int y_var = ecs_query_find_var(q, "y");
    test_assert(y_var != -1);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_str("", ecs_query_args_parse(q, &it, "x:parent.e1"));
    test_uint(it.variables[x_var].entity, e1);
    test_uint(it.variables[y_var].entity, EcsWildcard);
    ecs_iter_fini(&it);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_parse_missing_close_paren(void) {
    ecs_world_t *world = ecs_mini();

    ecs_query_t *q = ecs_query(world, {
        .expr = "$x($this, $y)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);
    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);
    int y_var = ecs_query_find_var(q, "y");
    test_assert(y_var != -1);

    ecs_iter_t it = ecs_query_iter(world, q);
    ecs_log_set_level(-4);
    test_str(NULL, ecs_query_args_parse(q, &it, "(x:e1"));
    ecs_iter_fini(&it);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_parse_missing_open_paren(void) {
    ecs_world_t *world = ecs_mini();

    ecs_query_t *q = ecs_query(world, {
        .expr = "$x($this, $y)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);
    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);
    int y_var = ecs_query_find_var(q, "y");
    test_assert(y_var != -1);

    ecs_iter_t it = ecs_query_iter(world, q);
    ecs_log_set_level(-4);
    test_str(NULL, ecs_query_args_parse(q, &it, "x:e1)"));
    ecs_iter_fini(&it);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_parse_missing_value(void) {
    ecs_world_t *world = ecs_mini();

    ecs_query_t *q = ecs_query(world, {
        .expr = "$x($this, $y)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);
    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);
    int y_var = ecs_query_find_var(q, "y");
    test_assert(y_var != -1);

    ecs_iter_t it = ecs_query_iter(world, q);
    ecs_log_set_level(-4);
    test_str(NULL, ecs_query_args_parse(q, &it, "x:"));
    ecs_iter_fini(&it);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_parse_0_var_w_spaces(void) {
    ecs_world_t *world = ecs_mini();

    ecs_query_t *q = ecs_query(world, {
        .expr = "$x($this, $y)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);
    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);
    int y_var = ecs_query_find_var(q, "y");
    test_assert(y_var != -1);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_str("", ecs_query_args_parse(q, &it, "  "));
    test_uint(it.variables[x_var].entity, EcsWildcard);
    test_uint(it.variables[y_var].entity, EcsWildcard);
    ecs_iter_fini(&it);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_parse_1_var_w_spaces(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e1 = ecs_entity(world, { .name = "e1" });

    ecs_query_t *q = ecs_query(world, {
        .expr = "$x($this, $y)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);
    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);
    int y_var = ecs_query_find_var(q, "y");
    test_assert(y_var != -1);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_str("", ecs_query_args_parse(q, &it, "  x  :  e1  "));
    test_uint(it.variables[x_var].entity, e1);
    test_uint(it.variables[y_var].entity, EcsWildcard);
    ecs_iter_fini(&it);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_parse_2_vars_w_spaces(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e1 = ecs_entity(world, { .name = "e1" });
    ecs_entity_t e2 = ecs_entity(world, { .name = "e2" });

    ecs_query_t *q = ecs_query(world, {
        .expr = "$x($this, $y)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);
    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);
    int y_var = ecs_query_find_var(q, "y");
    test_assert(y_var != -1);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_str("", ecs_query_args_parse(q, &it, "  x  :  e1  ,  y  :  e2  "));
    test_uint(it.variables[x_var].entity, e1);
    test_uint(it.variables[y_var].entity, e2);
    ecs_iter_fini(&it);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_parse_0_var_paren_w_spaces(void) {
    ecs_world_t *world = ecs_mini();

    ecs_query_t *q = ecs_query(world, {
        .expr = "$x($this, $y)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);
    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);
    int y_var = ecs_query_find_var(q, "y");
    test_assert(y_var != -1);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_str("  ", ecs_query_args_parse(q, &it, "  (  )  "));
    test_uint(it.variables[x_var].entity, EcsWildcard);
    test_uint(it.variables[y_var].entity, EcsWildcard);
    ecs_iter_fini(&it);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_parse_1_var_paren_w_spaces(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e1 = ecs_entity(world, { .name = "e1" });

    ecs_query_t *q = ecs_query(world, {
        .expr = "$x($this, $y)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);
    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);
    int y_var = ecs_query_find_var(q, "y");
    test_assert(y_var != -1);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_str("  ", ecs_query_args_parse(q, &it, "  (  x  :  e1  )  "));
    test_uint(it.variables[x_var].entity, e1);
    test_uint(it.variables[y_var].entity, EcsWildcard);
    ecs_iter_fini(&it);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_parse_2_vars_paren_w_spaces(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e1 = ecs_entity(world, { .name = "e1" });
    ecs_entity_t e2 = ecs_entity(world, { .name = "e2" });

    ecs_query_t *q = ecs_query(world, {
        .expr = "$x($this, $y)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);
    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);
    int y_var = ecs_query_find_var(q, "y");
    test_assert(y_var != -1);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_str("  ", ecs_query_args_parse(q, &it, "  (  x  :  e1  ,  y  :  e2  )  "));
    test_uint(it.variables[x_var].entity, e1);
    test_uint(it.variables[y_var].entity, e2);
    ecs_iter_fini(&it);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_var_count(void) {
    ecs_world_t *world = ecs_mini();

    ecs_query_t *q = ecs_query(world, {
        .expr = "$x($this, $y), $x($this), $y($this)",
        .cache_kind = cache_kind
    });
    test_assert(q != NULL);

    test_int(q->var_count, 3);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_var_name(void) {
    ecs_world_t *world = ecs_mini();

    ecs_query_t *q = ecs_query(world, {
        .expr = "$x($this, $y), $x($this), $y($this)",
        .cache_kind = cache_kind
    });
    test_assert(q != NULL);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);
    test_str(ecs_query_var_name(q, x_var), "x");

    int y_var = ecs_query_find_var(q, "y");
    test_assert(y_var != -1);
    test_str(ecs_query_var_name(q, y_var), "y");

    int this_var = ecs_query_find_var(q, "this");
    test_assert(this_var != -1);
    test_str(ecs_query_var_name(q, this_var), "this");

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_var_is_entity(void) {
    ecs_world_t *world = ecs_mini();

    ecs_query_t *q = ecs_query(world, {
        .expr = "$x($this, $y), $x($this), $y($this)",
        .cache_kind = cache_kind
    });
    test_assert(q != NULL);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);
    test_bool(ecs_query_var_is_entity(q, x_var), true);

    int y_var = ecs_query_find_var(q, "y");
    test_assert(y_var != -1);
    test_bool(ecs_query_var_is_entity(q, y_var), true);

    int this_var = ecs_query_find_var(q, "this");
    test_assert(this_var != -1);
    test_bool(ecs_query_var_is_entity(q, this_var), false);
    
    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_no_this_anonymous_src(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);

    ecs_entity_t e = ecs_new_w_id(world, TagA);
    test_assert(e != 0);

    ecs_query_t *q = ecs_query(world, {
        .expr = "TagA($_x)",
        .cache_kind = cache_kind
    });
    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(ecs_query_next(&it), true);
    test_uint(TagA, ecs_field_id(&it, 0));
    test_bool(ecs_query_next(&it), false);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_no_this_anonymous_src_w_pair(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);

    ecs_entity_t parent = ecs_new(world);
    ecs_entity_t e = ecs_new_w_id(world, TagA);
    test_assert(e != 0);
    ecs_add_pair(world, e, EcsChildOf, parent);

    ecs_query_t *q = ecs_query(world, {
        .expr = "TagA($_x), ChildOf($_x, $Parent)",
        .cache_kind = cache_kind
    });
    test_assert(q != NULL);

    int parent_var = ecs_query_find_var(q, "Parent");
    test_assert(parent_var != -1);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(ecs_query_next(&it), true);
    test_uint(0, it.count);
    test_uint(TagA, ecs_field_id(&it, 0));
    test_uint(ecs_pair(EcsChildOf, parent), ecs_field_id(&it, 1));
    test_uint(parent, ecs_iter_get_var(&it, parent_var));
    test_bool(ecs_query_next(&it), false);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_no_this_anonymous_component_src(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new_w(world, Position);
    test_assert(e != 0);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Position($x)",
        .cache_kind = cache_kind
    });
    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(ecs_query_next(&it), true);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_bool(ecs_query_next(&it), false);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_no_this_anonymous_component_src_w_pair(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t parent = ecs_new(world);
    ecs_entity_t e = ecs_new_w(world, Position);
    test_assert(e != 0);
    ecs_add_pair(world, e, EcsChildOf, parent);

    ecs_log_set_level(-4);
    ecs_query_t *q = ecs_query(world, {
        .expr = "Position($_x), ChildOf($_x, $Parent)",
        .cache_kind = cache_kind
    });
    test_assert(q == NULL);

    ecs_fini(world);
}

void Variables_lookup_from_table_this(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_entity_t p1 = ecs_new_w(world, TagA);
    ecs_new_w(world, TagA);
    ecs_entity_t p3 = ecs_new_w(world, TagA);
    ecs_entity_t p4 = ecs_new_w(world, TagA);

    ecs_entity_t child_1 = ecs_entity(world, { .name = "child" });
    ecs_add_pair(world, child_1, EcsChildOf, p1);
    ecs_entity_t child_3 = ecs_entity(world, { .name = "child" });
    ecs_add_pair(world, child_3, EcsChildOf, p3);
    ecs_entity_t child_4 = ecs_entity(world, { .name = "child" });
    ecs_add_pair(world, child_4, EcsChildOf, p4);
    ecs_add(world, child_1, TagB);
    ecs_add(world, child_4, TagB);

    ecs_query_t *q = ecs_query(world, {
        .expr = "TagA, TagB($this.child)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int32_t child_var = ecs_query_find_var(q, "this.child");
    test_assert(child_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(p1, it.entities[0]);
        test_uint(TagA, ecs_field_id(&it, 0));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(TagB, ecs_field_id(&it, 1));
        test_uint(child_1, ecs_field_src(&it, 1));
        test_uint(child_1, ecs_iter_get_var(&it, child_var));

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(p4, it.entities[0]);
        test_uint(TagA, ecs_field_id(&it, 0));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(TagB, ecs_field_id(&it, 1));
        test_uint(child_4, ecs_field_src(&it, 1));
        test_uint(child_4, ecs_iter_get_var(&it, child_var));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_lookup_from_entity_this(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_entity_t t1 = ecs_new(world);
    ecs_entity_t t2 = ecs_new(world);
    ecs_entity_t t3 = ecs_new(world);
    ecs_entity_t t4 = ecs_new(world);

    ecs_entity_t p1 = ecs_new_w_pair(world, Rel, t1);
    ecs_new_w_pair(world, Rel, t2);
    ecs_new_w_pair(world, Rel, t3);
    ecs_entity_t p4 = ecs_new_w_pair(world, Rel, t4);

    ecs_entity_t child_1 = ecs_entity(world, { .name = "child" });
    ecs_add_pair(world, child_1, EcsChildOf, t1);
    ecs_entity_t child_3 = ecs_entity(world, { .name = "child" });
    ecs_add_pair(world, child_3, EcsChildOf, t3);
    ecs_entity_t child_4 = ecs_entity(world, { .name = "child" });
    ecs_add_pair(world, child_4, EcsChildOf, t4);
    ecs_add(world, child_1, TagB);
    ecs_add(world, child_4, TagB);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Rel($x, $this), TagB($this.child)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int32_t x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);
    int32_t child_var = ecs_query_find_var(q, "this.child");
    test_assert(child_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(t1, it.entities[0]);
        test_uint(ecs_pair(Rel, t1), ecs_field_id(&it, 0));
        test_uint(p1, ecs_field_src(&it, 0));
        test_uint(TagB, ecs_field_id(&it, 1));
        test_uint(child_1, ecs_field_src(&it, 1));
        test_uint(p1, ecs_iter_get_var(&it, x_var));
        test_uint(child_1, ecs_iter_get_var(&it, child_var));

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(t4, it.entities[0]);
        test_uint(ecs_pair(Rel, t4), ecs_field_id(&it, 0));
        test_uint(p4, ecs_field_src(&it, 0));
        test_uint(TagB, ecs_field_id(&it, 1));
        test_uint(child_4, ecs_field_src(&it, 1));
        test_uint(p4, ecs_iter_get_var(&it, x_var));
        test_uint(child_4, ecs_iter_get_var(&it, child_var));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_lookup_from_table(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_entity_t p1 = ecs_new_w(world, TagA);
    ecs_new_w(world, TagA);
    ecs_entity_t p3 = ecs_new_w(world, TagA);
    ecs_entity_t p4 = ecs_new_w(world, TagA);

    ecs_entity_t child_1 = ecs_entity(world, { .name = "child" });
    ecs_add_pair(world, child_1, EcsChildOf, p1);
    ecs_entity_t child_3 = ecs_entity(world, { .name = "child" });
    ecs_add_pair(world, child_3, EcsChildOf, p3);
    ecs_entity_t child_4 = ecs_entity(world, { .name = "child" });
    ecs_add_pair(world, child_4, EcsChildOf, p4);
    ecs_add(world, child_1, TagB);
    ecs_add(world, child_4, TagB);

    ecs_query_t *q = ecs_query(world, {
        .expr = "TagA($x), TagB($x.child)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int32_t x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);
    int32_t child_var = ecs_query_find_var(q, "x.child");
    test_assert(child_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(TagA, ecs_field_id(&it, 0));
        test_uint(p1, ecs_field_src(&it, 0));
        test_uint(TagB, ecs_field_id(&it, 1));
        test_uint(child_1, ecs_field_src(&it, 1));
        test_uint(p1, ecs_iter_get_var(&it, x_var));
        test_uint(child_1, ecs_iter_get_var(&it, child_var));

        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(TagA, ecs_field_id(&it, 0));
        test_uint(p4, ecs_field_src(&it, 0));
        test_uint(TagB, ecs_field_id(&it, 1));
        test_uint(p4, ecs_iter_get_var(&it, x_var));
        test_uint(child_4, ecs_iter_get_var(&it, child_var));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_lookup_from_entity(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_entity_t t1 = ecs_new(world);
    ecs_entity_t t2 = ecs_new(world);
    ecs_entity_t t3 = ecs_new(world);
    ecs_entity_t t4 = ecs_new(world);

    ecs_entity_t p1 = ecs_new_w_pair(world, Rel, t1);
    ecs_new_w_pair(world, Rel, t2);
    ecs_new_w_pair(world, Rel, t3);
    ecs_entity_t p4 = ecs_new_w_pair(world, Rel, t4);

    ecs_entity_t child_1 = ecs_entity(world, { .name = "child" });
    ecs_add_pair(world, child_1, EcsChildOf, t1);
    ecs_entity_t child_3 = ecs_entity(world, { .name = "child" });
    ecs_add_pair(world, child_3, EcsChildOf, t3);
    ecs_entity_t child_4 = ecs_entity(world, { .name = "child" });
    ecs_add_pair(world, child_4, EcsChildOf, t4);
    ecs_add(world, child_1, TagB);
    ecs_add(world, child_4, TagB);

    ecs_query_t *q = ecs_query(world, {
        .expr = "(Rel, $x), TagB($x.child)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int32_t x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);
    int32_t child_var = ecs_query_find_var(q, "x.child");
    test_assert(child_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(p1, it.entities[0]);
        test_uint(ecs_pair(Rel, t1), ecs_field_id(&it, 0));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(TagB, ecs_field_id(&it, 1));
        test_uint(child_1, ecs_field_src(&it, 1));
        test_uint(t1, ecs_iter_get_var(&it, x_var));
        test_uint(child_1, ecs_iter_get_var(&it, child_var));

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(p4, it.entities[0]);
        test_uint(ecs_pair(Rel, t4), ecs_field_id(&it, 0));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(TagB, ecs_field_id(&it, 1));
        test_uint(child_4, ecs_field_src(&it, 1));
        test_uint(t4, ecs_iter_get_var(&it, x_var));
        test_uint(child_4, ecs_iter_get_var(&it, child_var));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_lookup_from_not_written(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);

    ecs_log_set_level(-4);

    ecs_query_t *q = ecs_query(world, {
        .expr = "TagA($this.foo)",
        .cache_kind = cache_kind
    });

    test_assert(q == NULL);

    ecs_fini(world);
}

void Variables_lookup_from_table_this_component(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_COMPONENT(world, Position);

    ecs_entity_t p1 = ecs_new_w(world, TagA);
    ecs_new_w(world, TagA);
    ecs_entity_t p3 = ecs_new_w(world, TagA);
    ecs_entity_t p4 = ecs_new_w(world, TagA);

    ecs_entity_t child_1 = ecs_entity(world, { .name = "child" });
    ecs_add_pair(world, child_1, EcsChildOf, p1);
    ecs_entity_t child_3 = ecs_entity(world, { .name = "child" });
    ecs_add_pair(world, child_3, EcsChildOf, p3);
    ecs_entity_t child_4 = ecs_entity(world, { .name = "child" });
    ecs_add_pair(world, child_4, EcsChildOf, p4);
    ecs_set(world, child_1, Position, {10, 20});
    ecs_set(world, child_4, Position, {11, 22});

    ecs_query_t *q = ecs_query(world, {
        .expr = "TagA, Position($this.child)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int32_t child_var = ecs_query_find_var(q, "this.child");
    test_assert(child_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(p1, it.entities[0]);
        test_uint(TagA, ecs_field_id(&it, 0));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_uint(child_1, ecs_field_src(&it, 1));
        test_uint(child_1, ecs_iter_get_var(&it, child_var));
        {
            Position *ptr = ecs_field(&it, Position, 1);
            test_assert(ptr != NULL);
            test_int(ptr->x, 10);
            test_int(ptr->y, 20);
        }

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(p4, it.entities[0]);
        test_uint(TagA, ecs_field_id(&it, 0));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_uint(child_4, ecs_field_src(&it, 1));
        test_uint(child_4, ecs_iter_get_var(&it, child_var));
        {
            Position *ptr = ecs_field(&it, Position, 1);
            test_assert(ptr != NULL);
            test_int(ptr->x, 11);
            test_int(ptr->y, 22);
        }

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_lookup_from_entity_this_component(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_TAG(world, TagA);
    ECS_COMPONENT(world, Position);

    ecs_entity_t t1 = ecs_new(world);
    ecs_entity_t t2 = ecs_new(world);
    ecs_entity_t t3 = ecs_new(world);
    ecs_entity_t t4 = ecs_new(world);

    ecs_entity_t p1 = ecs_new_w_pair(world, Rel, t1);
    ecs_new_w_pair(world, Rel, t2);
    ecs_new_w_pair(world, Rel, t3);
    ecs_entity_t p4 = ecs_new_w_pair(world, Rel, t4);

    ecs_entity_t child_1 = ecs_entity(world, { .name = "child" });
    ecs_add_pair(world, child_1, EcsChildOf, t1);
    ecs_entity_t child_3 = ecs_entity(world, { .name = "child" });
    ecs_add_pair(world, child_3, EcsChildOf, t3);
    ecs_entity_t child_4 = ecs_entity(world, { .name = "child" });
    ecs_add_pair(world, child_4, EcsChildOf, t4);
    ecs_set(world, child_1, Position, {10, 20});
    ecs_set(world, child_4, Position, {11, 22});

    ecs_query_t *q = ecs_query(world, {
        .expr = "Rel($x, $this), Position($this.child)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int32_t x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);
    int32_t child_var = ecs_query_find_var(q, "this.child");
    test_assert(child_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(t1, it.entities[0]);
        test_uint(ecs_pair(Rel, t1), ecs_field_id(&it, 0));
        test_uint(p1, ecs_field_src(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_uint(child_1, ecs_field_src(&it, 1));
        test_uint(p1, ecs_iter_get_var(&it, x_var));
        test_uint(child_1, ecs_iter_get_var(&it, child_var));
        {
            Position *ptr = ecs_field(&it, Position, 1);
            test_assert(ptr != NULL);
            test_int(ptr->x, 10);
            test_int(ptr->y, 20);
        }

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(t4, it.entities[0]);
        test_uint(ecs_pair(Rel, t4), ecs_field_id(&it, 0));
        test_uint(p4, ecs_field_src(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_uint(child_4, ecs_field_src(&it, 1));
        test_uint(p4, ecs_iter_get_var(&it, x_var));
        test_uint(child_4, ecs_iter_get_var(&it, child_var));
        {
            Position *ptr = ecs_field(&it, Position, 1);
            test_assert(ptr != NULL);
            test_int(ptr->x, 11);
            test_int(ptr->y, 22);
        }

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_lookup_from_table_component(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_COMPONENT(world, Position);

    ecs_entity_t p1 = ecs_new_w(world, TagA);
    ecs_new_w(world, TagA);
    ecs_entity_t p3 = ecs_new_w(world, TagA);
    ecs_entity_t p4 = ecs_new_w(world, TagA);

    ecs_entity_t child_1 = ecs_entity(world, { .name = "child" });
    ecs_add_pair(world, child_1, EcsChildOf, p1);
    ecs_entity_t child_3 = ecs_entity(world, { .name = "child" });
    ecs_add_pair(world, child_3, EcsChildOf, p3);
    ecs_entity_t child_4 = ecs_entity(world, { .name = "child" });
    ecs_add_pair(world, child_4, EcsChildOf, p4);
    ecs_set(world, child_1, Position, {10, 20});
    ecs_set(world, child_4, Position, {11, 22});

    ecs_query_t *q = ecs_query(world, {
        .expr = "TagA($x), Position($x.child)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int32_t x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);
    int32_t child_var = ecs_query_find_var(q, "x.child");
    test_assert(child_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(TagA, ecs_field_id(&it, 0));
        test_uint(p1, ecs_field_src(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_uint(child_1, ecs_field_src(&it, 1));
        test_uint(p1, ecs_iter_get_var(&it, x_var));
        test_uint(child_1, ecs_iter_get_var(&it, child_var));
        {
            Position *ptr = ecs_field(&it, Position, 1);
            test_assert(ptr != NULL);
            test_int(ptr->x, 10);
            test_int(ptr->y, 20);
        }

        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(TagA, ecs_field_id(&it, 0));
        test_uint(p4, ecs_field_src(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_uint(p4, ecs_iter_get_var(&it, x_var));
        test_uint(child_4, ecs_iter_get_var(&it, child_var));
        {
            Position *ptr = ecs_field(&it, Position, 1);
            test_assert(ptr != NULL);
            test_int(ptr->x, 11);
            test_int(ptr->y, 22);
        }

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_lookup_from_entity_component(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_TAG(world, TagA);
    ECS_COMPONENT(world, Position);

    ecs_entity_t t1 = ecs_new(world);
    ecs_entity_t t2 = ecs_new(world);
    ecs_entity_t t3 = ecs_new(world);
    ecs_entity_t t4 = ecs_new(world);

    ecs_entity_t p1 = ecs_new_w_pair(world, Rel, t1);
    ecs_new_w_pair(world, Rel, t2);
    ecs_new_w_pair(world, Rel, t3);
    ecs_entity_t p4 = ecs_new_w_pair(world, Rel, t4);

    ecs_entity_t child_1 = ecs_entity(world, { .name = "child" });
    ecs_add_pair(world, child_1, EcsChildOf, t1);
    ecs_entity_t child_3 = ecs_entity(world, { .name = "child" });
    ecs_add_pair(world, child_3, EcsChildOf, t3);
    ecs_entity_t child_4 = ecs_entity(world, { .name = "child" });
    ecs_add_pair(world, child_4, EcsChildOf, t4);
    ecs_set(world, child_1, Position, {10, 20});
    ecs_set(world, child_4, Position, {11, 22});

    ecs_query_t *q = ecs_query(world, {
        .expr = "(Rel, $x), Position($x.child)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int32_t x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);
    int32_t child_var = ecs_query_find_var(q, "x.child");
    test_assert(child_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(p1, it.entities[0]);
        test_uint(ecs_pair(Rel, t1), ecs_field_id(&it, 0));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_uint(child_1, ecs_field_src(&it, 1));
        test_uint(t1, ecs_iter_get_var(&it, x_var));
        test_uint(child_1, ecs_iter_get_var(&it, child_var));
        {
            Position *ptr = ecs_field(&it, Position, 1);
            test_assert(ptr != NULL);
            test_int(ptr->x, 10);
            test_int(ptr->y, 20);
        }

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(p4, it.entities[0]);
        test_uint(ecs_pair(Rel, t4), ecs_field_id(&it, 0));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_uint(child_4, ecs_field_src(&it, 1));
        test_uint(t4, ecs_iter_get_var(&it, x_var));
        test_uint(child_4, ecs_iter_get_var(&it, child_var));
        {
            Position *ptr = ecs_field(&it, Position, 1);
            test_assert(ptr != NULL);
            test_int(ptr->x, 11);
            test_int(ptr->y, 22);
        }

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_lookup_from_table_two_children(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, TagC);

    ecs_entity_t p1 = ecs_entity(world, { .name = "p1" }); ecs_add(world, p1, TagA);
    ecs_new_w(world, TagA);
    ecs_entity_t p3 = ecs_entity(world, { .name = "p3" }); ecs_add(world, p3, TagA);
    ecs_entity_t p4 = ecs_entity(world, { .name = "p4" }); ecs_add(world, p4, TagA);

    ecs_entity_t child_a_1 = ecs_entity(world, { .name = "child_a" });
    ecs_add_pair(world, child_a_1, EcsChildOf, p1);
    ecs_entity_t child_a_3 = ecs_entity(world, { .name = "child_a" });
    ecs_add_pair(world, child_a_3, EcsChildOf, p3);
    ecs_entity_t child_a_4 = ecs_entity(world, { .name = "child_a" });
    ecs_add_pair(world, child_a_4, EcsChildOf, p4);
    ecs_add(world, child_a_1, TagB);
    ecs_add(world, child_a_4, TagB);

    ecs_entity_t child_b_1 = ecs_entity(world, { .name = "child_b" });
    ecs_add_pair(world, child_b_1, EcsChildOf, p1);
    ecs_entity_t child_b_3 = ecs_entity(world, { .name = "child_b" });
    ecs_add_pair(world, child_b_3, EcsChildOf, p3);
    ecs_entity_t child_b_4 = ecs_entity(world, { .name = "child_b" });
    ecs_add_pair(world, child_b_4, EcsChildOf, p4);
    ecs_add(world, child_b_1, TagC);
    ecs_add(world, child_b_3, TagC);

    ecs_query_t *q = ecs_query(world, {
        .expr = "TagA, TagB($this.child_a), TagC($this.child_b)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int32_t child_a_var = ecs_query_find_var(q, "this.child_a");
    test_assert(child_a_var != -1);
    int32_t child_b_var = ecs_query_find_var(q, "this.child_b");
    test_assert(child_b_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(p1, it.entities[0]);
        test_uint(TagA, ecs_field_id(&it, 0));
        test_uint(TagB, ecs_field_id(&it, 1));
        test_uint(TagC, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(child_a_1, ecs_field_src(&it, 1));
        test_uint(child_b_1, ecs_field_src(&it, 2));
        test_uint(child_a_1, ecs_iter_get_var(&it, child_a_var));
        test_uint(child_b_1, ecs_iter_get_var(&it, child_b_var));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_lookup_from_entity_two_children(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_TAG(world, TagB);
    ECS_TAG(world, TagC);

    ecs_entity_t t1 = ecs_new(world);
    ecs_entity_t t2 = ecs_new(world);
    ecs_entity_t t3 = ecs_new(world);
    ecs_entity_t t4 = ecs_new(world);

    ecs_entity_t p1 = ecs_new_w_pair(world, Rel, t1);
    ecs_new_w_pair(world, Rel, t2);
    ecs_new_w_pair(world, Rel, t3);
    ecs_new_w_pair(world, Rel, t4);

    ecs_entity_t child_a_1 = ecs_entity(world, { .name = "child_a" });
    ecs_add_pair(world, child_a_1, EcsChildOf, t1);
    ecs_entity_t child_a_3 = ecs_entity(world, { .name = "child_a" });
    ecs_add_pair(world, child_a_3, EcsChildOf, t3);
    ecs_entity_t child_a_4 = ecs_entity(world, { .name = "child_a" });
    ecs_add_pair(world, child_a_4, EcsChildOf, t4);
    ecs_add(world, child_a_1, TagB);
    ecs_add(world, child_a_4, TagB);

    ecs_entity_t child_b_1 = ecs_entity(world, { .name = "child_b" });
    ecs_add_pair(world, child_b_1, EcsChildOf, t1);
    ecs_entity_t child_b_3 = ecs_entity(world, { .name = "child_b" });
    ecs_add_pair(world, child_b_3, EcsChildOf, t3);
    ecs_entity_t child_b_4 = ecs_entity(world, { .name = "child_b" });
    ecs_add_pair(world, child_b_4, EcsChildOf, t4);
    ecs_add(world, child_b_1, TagC);
    ecs_add(world, child_b_3, TagC);

    ecs_query_t *q = ecs_query(world, {
        .expr = "(Rel, $x), TagB($x.child_a), TagC($x.child_b)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int32_t x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);
    int32_t child_a_var = ecs_query_find_var(q, "x.child_a");
    test_assert(child_a_var != -1);
    int32_t child_b_var = ecs_query_find_var(q, "x.child_b");
    test_assert(child_b_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(p1, it.entities[0]);
        test_uint(ecs_pair(Rel, t1), ecs_field_id(&it, 0));
        test_uint(TagB, ecs_field_id(&it, 1));
        test_uint(TagC, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(child_a_1, ecs_field_src(&it, 1));
        test_uint(child_b_1, ecs_field_src(&it, 2));
        test_uint(t1, ecs_iter_get_var(&it, x_var));
        test_uint(child_a_1, ecs_iter_get_var(&it, child_a_var));
        test_uint(child_b_1, ecs_iter_get_var(&it, child_b_var));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_lookup_from_table_same_child_twice(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, TagC);

    ecs_entity_t p1 = ecs_entity(world, { .name = "p1" }); ecs_add(world, p1, TagA);
    ecs_new_w(world, TagA);
    ecs_entity_t p3 = ecs_entity(world, { .name = "p3" }); ecs_add(world, p3, TagA);
    ecs_entity_t p4 = ecs_entity(world, { .name = "p4" }); ecs_add(world, p4, TagA);

    ecs_entity_t child_a_1 = ecs_entity(world, { .name = "child_a" });
    ecs_add_pair(world, child_a_1, EcsChildOf, p1);
    ecs_entity_t child_a_3 = ecs_entity(world, { .name = "child_a" });
    ecs_add_pair(world, child_a_3, EcsChildOf, p3);
    ecs_entity_t child_a_4 = ecs_entity(world, { .name = "child_a" });
    ecs_add_pair(world, child_a_4, EcsChildOf, p4);
    ecs_add(world, child_a_1, TagB);
    ecs_add(world, child_a_4, TagB);
    ecs_add(world, child_a_1, TagC);

    ecs_query_t *q = ecs_query(world, {
        .expr = "TagA, TagB($this.child_a), TagC($this.child_a)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int32_t child_a_var = ecs_query_find_var(q, "this.child_a");
    test_assert(child_a_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(p1, it.entities[0]);
        test_uint(TagA, ecs_field_id(&it, 0));
        test_uint(TagB, ecs_field_id(&it, 1));
        test_uint(TagC, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(child_a_1, ecs_field_src(&it, 1));
        test_uint(child_a_1, ecs_field_src(&it, 2));
        test_uint(child_a_1, ecs_iter_get_var(&it, child_a_var));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_lookup_from_entity_same_child_twice(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_TAG(world, TagB);
    ECS_TAG(world, TagC);

    ecs_entity_t t1 = ecs_new(world);
    ecs_entity_t t2 = ecs_new(world);
    ecs_entity_t t3 = ecs_new(world);
    ecs_entity_t t4 = ecs_new(world);

    ecs_entity_t p1 = ecs_new_w_pair(world, Rel, t1);
    ecs_new_w_pair(world, Rel, t2);
    ecs_new_w_pair(world, Rel, t3);
    ecs_new_w_pair(world, Rel, t4);

    ecs_entity_t child_a_1 = ecs_entity(world, { .name = "child_a" });
    ecs_add_pair(world, child_a_1, EcsChildOf, t1);
    ecs_entity_t child_a_3 = ecs_entity(world, { .name = "child_a" });
    ecs_add_pair(world, child_a_3, EcsChildOf, t3);
    ecs_entity_t child_a_4 = ecs_entity(world, { .name = "child_a" });
    ecs_add_pair(world, child_a_4, EcsChildOf, t4);
    ecs_add(world, child_a_1, TagB);
    ecs_add(world, child_a_4, TagB);
    ecs_add(world, child_a_1, TagC);

    ecs_query_t *q = ecs_query(world, {
        .expr = "(Rel, $x), TagB($x.child_a), TagC($x.child_a)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int32_t x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);
    int32_t child_a_var = ecs_query_find_var(q, "x.child_a");
    test_assert(child_a_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(p1, it.entities[0]);
        test_uint(ecs_pair(Rel, t1), ecs_field_id(&it, 0));
        test_uint(TagB, ecs_field_id(&it, 1));
        test_uint(TagC, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(child_a_1, ecs_field_src(&it, 1));
        test_uint(child_a_1, ecs_field_src(&it, 2));
        test_uint(t1, ecs_iter_get_var(&it, x_var));
        test_uint(child_a_1, ecs_iter_get_var(&it, child_a_var));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_lookup_from_table_not(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_entity_t p1 = ecs_new_w(world, TagA); ecs_set_name(world, p1, "p1");
    ecs_entity_t p2 = ecs_new_w(world, TagA); ecs_set_name(world, p2, "p2");
    ecs_entity_t p3 = ecs_new_w(world, TagA); ecs_set_name(world, p3, "p3");
    ecs_entity_t p4 = ecs_new_w(world, TagA); ecs_set_name(world, p4, "p4");

    ecs_entity_t child_1 = ecs_entity(world, { .name = "child" });
    ecs_add_pair(world, child_1, EcsChildOf, p1);
    ecs_entity_t child_3 = ecs_entity(world, { .name = "child" });
    ecs_add_pair(world, child_3, EcsChildOf, p3);
    ecs_entity_t child_4 = ecs_entity(world, { .name = "child" });
    ecs_add_pair(world, child_4, EcsChildOf, p4);
    ecs_add(world, child_1, TagB);
    ecs_add(world, child_4, TagB);

    ecs_query_t *q = ecs_query(world, {
        .expr = "TagA($x), !TagB($x.child)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int32_t x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);
    int32_t child_var = ecs_query_find_var(q, "x.child");
    test_assert(child_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(TagA, ecs_field_id(&it, 0));
        test_uint(TagB, ecs_field_id(&it, 1));
        test_uint(p2, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_field_is_set(&it, 1));
        test_uint(p2, ecs_iter_get_var(&it, x_var));
        test_uint(EcsWildcard, ecs_iter_get_var(&it, child_var));

        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(TagA, ecs_field_id(&it, 0));
        test_uint(TagB, ecs_field_id(&it, 1));
        test_uint(p3, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_field_is_set(&it, 1));
        test_uint(p3, ecs_iter_get_var(&it, x_var));
        test_uint(child_3, ecs_iter_get_var(&it, child_var));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_lookup_from_entity_not(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_entity_t t1 = ecs_new(world);
    ecs_entity_t t2 = ecs_new(world);
    ecs_entity_t t3 = ecs_new(world);
    ecs_entity_t t4 = ecs_new(world);

    ecs_new_w_pair(world, Rel, t1);
    ecs_entity_t p2 = ecs_new_w_pair(world, Rel, t2);
    ecs_entity_t p3 = ecs_new_w_pair(world, Rel, t3);
    ecs_new_w_pair(world, Rel, t4);

    ecs_entity_t child_1 = ecs_entity(world, { .name = "child" });
    ecs_add_pair(world, child_1, EcsChildOf, t1);
    ecs_entity_t child_3 = ecs_entity(world, { .name = "child" });
    ecs_add_pair(world, child_3, EcsChildOf, t3);
    ecs_entity_t child_4 = ecs_entity(world, { .name = "child" });
    ecs_add_pair(world, child_4, EcsChildOf, t4);
    ecs_add(world, child_1, TagB);
    ecs_add(world, child_4, TagB);

    ecs_query_t *q = ecs_query(world, {
        .expr = "(Rel, $x), !TagB($x.child)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int32_t x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);
    int32_t child_var = ecs_query_find_var(q, "x.child");
    test_assert(child_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(p2, it.entities[0]);
        test_uint(ecs_pair(Rel, t2), ecs_field_id(&it, 0));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(TagB, ecs_field_id(&it, 1));
        test_bool(false, ecs_field_is_set(&it, 1));
        test_uint(t2, ecs_iter_get_var(&it, x_var));
        test_uint(EcsWildcard, ecs_iter_get_var(&it, child_var));

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(p3, it.entities[0]);
        test_uint(ecs_pair(Rel, t3), ecs_field_id(&it, 0));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(TagB, ecs_field_id(&it, 1));
        test_bool(false, ecs_field_is_set(&it, 1));
        test_uint(t3, ecs_iter_get_var(&it, x_var));
        test_uint(child_3, ecs_iter_get_var(&it, child_var));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_lookup_from_table_w_any_component(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, TagC);

    ecs_entity_t p1 = ecs_new_w(world, TagA);
    ecs_new_w(world, TagA);
    ecs_entity_t p3 = ecs_new_w(world, TagA);
    ecs_entity_t p4 = ecs_new_w(world, TagA);

    ecs_entity_t child_1 = ecs_entity(world, { .name = "child" });
    ecs_add_pair(world, child_1, EcsChildOf, p1);
    ecs_entity_t child_3 = ecs_entity(world, { .name = "child" });
    ecs_add_pair(world, child_3, EcsChildOf, p3);
    ecs_entity_t child_4 = ecs_entity(world, { .name = "child" });
    ecs_add_pair(world, child_4, EcsChildOf, p4);
    ecs_add(world, child_1, TagB);
    ecs_add(world, child_4, TagB);
    ecs_add(world, child_4, TagC);

    ecs_query_t *q = ecs_query(world, {
        .expr = "TagA($x), _($x.child)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int32_t x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);
    int32_t child_var = ecs_query_find_var(q, "x.child");
    test_assert(child_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(TagA, ecs_field_id(&it, 0));
        test_uint(p1, ecs_field_src(&it, 0));
        test_uint(EcsWildcard, ecs_field_id(&it, 1));
        test_uint(child_1, ecs_field_src(&it, 1));
        test_uint(p1, ecs_iter_get_var(&it, x_var));
        test_uint(child_1, ecs_iter_get_var(&it, child_var));

        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(TagA, ecs_field_id(&it, 0));
        test_uint(p4, ecs_field_src(&it, 0));
        test_uint(EcsWildcard, ecs_field_id(&it, 1));
        test_uint(p4, ecs_iter_get_var(&it, x_var));
        test_uint(child_4, ecs_iter_get_var(&it, child_var));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_lookup_from_entity_w_any_component(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, TagC);

    ecs_entity_t t1 = ecs_new(world);
    ecs_entity_t t2 = ecs_new(world);
    ecs_entity_t t3 = ecs_new(world);
    ecs_entity_t t4 = ecs_new(world);

    ecs_entity_t p1 = ecs_new_w_pair(world, Rel, t1);
    ecs_new_w_pair(world, Rel, t2);
    ecs_new_w_pair(world, Rel, t3);
    ecs_entity_t p4 = ecs_new_w_pair(world, Rel, t4);

    ecs_entity_t child_1 = ecs_entity(world, { .name = "child" });
    ecs_add_pair(world, child_1, EcsChildOf, t1);
    ecs_entity_t child_3 = ecs_entity(world, { .name = "child" });
    ecs_add_pair(world, child_3, EcsChildOf, t3);
    ecs_entity_t child_4 = ecs_entity(world, { .name = "child" });
    ecs_add_pair(world, child_4, EcsChildOf, t4);
    ecs_add(world, child_1, TagB);
    ecs_add(world, child_4, TagB);
    ecs_add(world, child_4, TagC);

    ecs_query_t *q = ecs_query(world, {
        .expr = "(Rel, $x), _($x.child)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int32_t x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);
    int32_t child_var = ecs_query_find_var(q, "x.child");
    test_assert(child_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(p1, it.entities[0]);
        test_uint(ecs_pair(Rel, t1), ecs_field_id(&it, 0));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(EcsWildcard, ecs_field_id(&it, 1));
        test_uint(child_1, ecs_field_src(&it, 1));
        test_uint(t1, ecs_iter_get_var(&it, x_var));
        test_uint(child_1, ecs_iter_get_var(&it, child_var));

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(p4, it.entities[0]);
        test_uint(ecs_pair(Rel, t4), ecs_field_id(&it, 0));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(EcsWildcard, ecs_field_id(&it, 1));
        test_uint(child_4, ecs_field_src(&it, 1));
        test_uint(t4, ecs_iter_get_var(&it, x_var));
        test_uint(child_4, ecs_iter_get_var(&it, child_var));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_lookup_as_tag(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_entity_t parent_a = ecs_new_w(world, Foo);
    ecs_entity_t parent_b = ecs_new_w(world, Foo);
    ecs_entity_t parent_c = ecs_new_w(world, Foo);
    ecs_new_w(world, Foo);

    ecs_entity_t tag_a = ecs_entity(world, { .name = "Tag" });
    ecs_add_pair(world, tag_a, EcsChildOf, parent_a);

    ecs_entity_t tag_b = ecs_entity(world, { .name = "Tag" });
    ecs_add_pair(world, tag_b, EcsChildOf, parent_b);

    ecs_entity_t tag_c = ecs_entity(world, { .name = "Tag" });
    ecs_add_pair(world, tag_c, EcsChildOf, parent_c);

    ecs_entity_t a = ecs_new_w_id(world, tag_a);
    ecs_entity_t c = ecs_new_w_id(world, tag_c);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Foo, $this.Tag($x)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);
    
    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);
    int tag_var = ecs_query_find_var(q, "this.Tag");
    test_assert(tag_var != -1);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(parent_a, it.entities[0]);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(tag_a, ecs_field_id(&it, 1));
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(a, ecs_field_src(&it, 1));
    test_uint(a, ecs_iter_get_var(&it, x_var));
    test_uint(tag_a, ecs_iter_get_var(&it, tag_var));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(parent_c, it.entities[0]);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(tag_c, ecs_field_id(&it, 1));
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(c, ecs_field_src(&it, 1));
    test_uint(c, ecs_iter_get_var(&it, x_var));
    test_uint(tag_c, ecs_iter_get_var(&it, tag_var));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_lookup_as_relationship(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Tgt);

    ecs_entity_t parent_a = ecs_new_w(world, Foo);
    ecs_entity_t parent_b = ecs_new_w(world, Foo);
    ecs_entity_t parent_c = ecs_new_w(world, Foo);
    ecs_new_w(world, Foo);

    ecs_entity_t tag_a = ecs_entity(world, { .name = "Tag" });
    ecs_add_pair(world, tag_a, EcsChildOf, parent_a);

    ecs_entity_t tag_b = ecs_entity(world, { .name = "Tag" });
    ecs_add_pair(world, tag_b, EcsChildOf, parent_b);

    ecs_entity_t tag_c = ecs_entity(world, { .name = "Tag" });
    ecs_add_pair(world, tag_c, EcsChildOf, parent_c);

    ecs_entity_t a = ecs_new_w_pair(world, tag_a, Tgt);
    ecs_entity_t c = ecs_new_w_pair(world, tag_c, Tgt);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Foo, $this.Tag($x, Tgt)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);
    
    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);
    int tag_var = ecs_query_find_var(q, "this.Tag");
    test_assert(tag_var != -1);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(parent_a, it.entities[0]);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_pair(tag_a, Tgt), ecs_field_id(&it, 1));
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(a, ecs_field_src(&it, 1));
    test_uint(a, ecs_iter_get_var(&it, x_var));
    test_uint(tag_a, ecs_iter_get_var(&it, tag_var));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(parent_c, it.entities[0]);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_pair(tag_c, Tgt), ecs_field_id(&it, 1));
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(c, ecs_field_src(&it, 1));
    test_uint(c, ecs_iter_get_var(&it, x_var));
    test_uint(tag_c, ecs_iter_get_var(&it, tag_var));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_lookup_as_target(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_TAG(world, Foo);

    ecs_entity_t parent_a = ecs_new_w(world, Foo);
    ecs_entity_t parent_b = ecs_new_w(world, Foo);
    ecs_entity_t parent_c = ecs_new_w(world, Foo);
    ecs_new_w(world, Foo);

    ecs_entity_t tag_a = ecs_entity(world, { .name = "Tag" });
    ecs_add_pair(world, tag_a, EcsChildOf, parent_a);

    ecs_entity_t tag_b = ecs_entity(world, { .name = "Tag" });
    ecs_add_pair(world, tag_b, EcsChildOf, parent_b);

    ecs_entity_t tag_c = ecs_entity(world, { .name = "Tag" });
    ecs_add_pair(world, tag_c, EcsChildOf, parent_c);

    ecs_entity_t a = ecs_new_w_pair(world, Rel, tag_a);
    ecs_entity_t c = ecs_new_w_pair(world, Rel, tag_c);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Foo, Rel($x, $this.Tag)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);
    int tag_var = ecs_query_find_var(q, "this.Tag");
    test_assert(tag_var != -1);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(parent_a, it.entities[0]);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_pair(Rel, tag_a), ecs_field_id(&it, 1));
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(a, ecs_field_src(&it, 1));
    test_uint(a, ecs_iter_get_var(&it, x_var));
    test_uint(tag_a, ecs_iter_get_var(&it, tag_var));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(parent_c, it.entities[0]);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_pair(Rel, tag_c), ecs_field_id(&it, 1));
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(c, ecs_field_src(&it, 1));
    test_uint(c, ecs_iter_get_var(&it, x_var));
    test_uint(tag_c, ecs_iter_get_var(&it, tag_var));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_lookup_assign_var(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_TAG(world, Foo);

    ecs_entity_t parent_a = ecs_new_w(world, Foo);
    ecs_entity_t parent_b = ecs_new_w(world, Foo);
    ecs_new_w(world, Foo);

    ecs_entity_t tag_a = ecs_entity(world, { .name = "Tag" });
    ecs_add_pair(world, tag_a, EcsChildOf, parent_a);

    ecs_entity_t tag_b = ecs_entity(world, { .name = "Tag" });
    ecs_add_pair(world, tag_b, EcsChildOf, parent_b);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Foo, $x == $this.Tag",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);
    int tag_var = ecs_query_find_var(q, "this.Tag");
    test_assert(tag_var != -1);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(parent_a, it.entities[0]);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(tag_a, ecs_iter_get_var(&it, x_var));
    test_uint(tag_a, ecs_iter_get_var(&it, tag_var));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(parent_b, it.entities[0]);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(tag_b, ecs_iter_get_var(&it, x_var));
    test_uint(tag_b, ecs_iter_get_var(&it, tag_var));
    
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_lookup_eq_var(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_TAG(world, Foo);

    ecs_entity_t parent_a = ecs_new_w(world, Foo);
    ecs_entity_t parent_b = ecs_new_w(world, Foo);
    ecs_new_w(world, Foo);

    ecs_entity_t tag_ax = ecs_entity(world, { .name = "TagX" });
    ecs_add_pair(world, tag_ax, EcsChildOf, parent_a);
    ecs_entity_t tag_ay = ecs_entity(world, { .name = "TagY" });
    ecs_add_pair(world, tag_ay, EcsChildOf, parent_a);
    ecs_entity_t tag_az = ecs_entity(world, { .name = "TagZ" });
    ecs_add_pair(world, tag_az, EcsChildOf, parent_a);

    ecs_entity_t tag_bx = ecs_entity(world, { .name = "TagX" });
    ecs_add_pair(world, tag_bx, EcsChildOf, parent_b);
    ecs_entity_t tag_by = ecs_entity(world, { .name = "TagY" });
    ecs_add_pair(world, tag_by, EcsChildOf, parent_b);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Foo, ChildOf($x, $this), $x == $this.TagY",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);
    int tag_var = ecs_query_find_var(q, "this.TagY");
    test_assert(tag_var != -1);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(parent_a, it.entities[0]);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_childof(parent_a), ecs_field_id(&it, 1));
    test_uint(tag_ay, ecs_iter_get_var(&it, x_var));
    test_uint(tag_ay, ecs_iter_get_var(&it, tag_var));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(parent_b, it.entities[0]);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_childof(parent_b), ecs_field_id(&it, 1));
    test_uint(tag_by, ecs_iter_get_var(&it, x_var));
    test_uint(tag_by, ecs_iter_get_var(&it, tag_var));
    
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_lookup_neq_var(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_TAG(world, Foo);

    ecs_entity_t parent_a = ecs_new_w(world, Foo);
    ecs_entity_t parent_b = ecs_new_w(world, Foo);
    ecs_new_w(world, Foo);

    ecs_entity_t tag_ax = ecs_entity(world, { .name = "TagX" });
    ecs_add_pair(world, tag_ax, EcsChildOf, parent_a);
    ecs_entity_t tag_ay = ecs_entity(world, { .name = "TagY" });
    ecs_add_pair(world, tag_ay, EcsChildOf, parent_a);
    ecs_entity_t tag_az = ecs_entity(world, { .name = "TagZ" });
    ecs_add_pair(world, tag_az, EcsChildOf, parent_a);

    ecs_entity_t tag_bx = ecs_entity(world, { .name = "TagX" });
    ecs_add_pair(world, tag_bx, EcsChildOf, parent_b);
    ecs_entity_t tag_by = ecs_entity(world, { .name = "TagY" });
    ecs_add_pair(world, tag_by, EcsChildOf, parent_b);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Foo, ChildOf($x, $this), $x != $this.TagY",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);
    int tag_var = ecs_query_find_var(q, "this.TagY");
    test_assert(tag_var != -1);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(parent_a, it.entities[0]);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_childof(parent_a), ecs_field_id(&it, 1));
    test_uint(tag_ax, ecs_iter_get_var(&it, x_var));
    test_uint(tag_ay, ecs_iter_get_var(&it, tag_var));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(parent_a, it.entities[0]);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_childof(parent_a), ecs_field_id(&it, 1));
    test_uint(tag_az, ecs_iter_get_var(&it, x_var));
    test_uint(tag_ay, ecs_iter_get_var(&it, tag_var));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(parent_b, it.entities[0]);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_childof(parent_b), ecs_field_id(&it, 1));
    test_uint(tag_bx, ecs_iter_get_var(&it, x_var));
    test_uint(tag_by, ecs_iter_get_var(&it, tag_var));
    
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_lookup_unresolved_dependent(void) {
    ecs_world_t *world = ecs_init();

    ecs_log_set_level(-4);
    ecs_query_t *r = ecs_query(world, {
        .expr = "(ChildOf, $_tgt), ChildOf(flecs, $tgt.child)"
    });

    test_assert(r == NULL);

    ecs_fini(world);
}

void Variables_check_vars_this(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Foo",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);
    test_int(1, q->var_count);
    test_str("this", ecs_query_var_name(q, 0));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_check_vars_var(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Foo($x)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);
    test_int(2, q->var_count);
    test_str("this", ecs_query_var_name(q, 0));
    test_str("x", ecs_query_var_name(q, 1));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_check_vars_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_query_t *q = ecs_query(world, {
        .expr = "*",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);
    test_int(1, q->var_count);
    test_str("this", ecs_query_var_name(q, 0));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_check_vars_any(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_query_t *q = ecs_query(world, {
        .expr = "_",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);
    test_int(1, q->var_count);
    test_str("this", ecs_query_var_name(q, 0));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_check_vars_var_as_tgt(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Foo($this), ChildOf($this, $x)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);
    test_int(2, q->var_count);
    test_str("this", ecs_query_var_name(q, 0));
    test_str("x", ecs_query_var_name(q, 1));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_check_vars_this_as_tgt(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Foo($this), ChildOf($x, $this)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);
    test_int(3, q->var_count);
    test_str("this", ecs_query_var_name(q, 0)); // table
    test_str("x", ecs_query_var_name(q, 1));
    test_str("this", ecs_query_var_name(q, 2)); // entity

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_check_vars_this_w_lookup_var(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Foo($this), ChildOf($x, $this.var)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);
    test_int(4, q->var_count);
    test_str("this", ecs_query_var_name(q, 0));
    test_str("x", ecs_query_var_name(q, 1));
    test_str("this.var", ecs_query_var_name(q, 2));
    test_str("this", ecs_query_var_name(q, 3));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_check_vars_var_w_lookup_var(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Foo($y), ChildOf($x, $y.var)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);
    test_int(4, q->var_count);
    test_str("this", ecs_query_var_name(q, 0));
    test_str("y", ecs_query_var_name(q, 1));
    test_str("x", ecs_query_var_name(q, 2));
    test_str("y.var", ecs_query_var_name(q, 3));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_check_vars_anonymous_var_as_tgt(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Foo($this), ChildOf($this, $_x)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);
    test_int(2, q->var_count);
    test_str("this", ecs_query_var_name(q, 0));
    test_str("_x", ecs_query_var_name(q, 1));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_check_vars_wildcard_as_tgt(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Foo($this), ChildOf($this, *)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);
    test_int(1, q->var_count);
    test_str("this", ecs_query_var_name(q, 0));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_check_vars_any_as_tgt(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Foo($this), ChildOf($this, _)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);
    test_int(1, q->var_count);
    test_str("this", ecs_query_var_name(q, 0));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_1_trivial_1_var(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Foo(self), ChildOf($this, $x)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    ecs_entity_t p = ecs_new(world);
    ecs_entity_t e = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_add(world, e, Foo);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e, it.entities[0]);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_pair(EcsChildOf, p), ecs_field_id(&it, 1));
    test_uint(p, ecs_iter_get_var(&it, x_var));
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_2_trivial_1_var(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Foo(self), Bar(self), ChildOf($this, $x)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    ecs_entity_t p = ecs_new(world);
    ecs_entity_t e = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_add(world, e, Foo);
    ecs_add(world, e, Bar);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e, it.entities[0]);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(Bar, ecs_field_id(&it, 1));
    test_uint(ecs_pair(EcsChildOf, p), ecs_field_id(&it, 2));
    test_uint(p, ecs_iter_get_var(&it, x_var));
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_1_trivial_1_var_component(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Position(self), ChildOf($this, $x)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    ecs_entity_t p = ecs_new(world);
    ecs_entity_t e = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_set(world, e, Position, {10, 20});

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(ecs_pair(EcsChildOf, p), ecs_field_id(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 20);
    }
    test_uint(p, ecs_iter_get_var(&it, x_var));
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_2_trivial_1_var_component(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Position(self), Velocity(self), ChildOf($this, $x)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    ecs_entity_t p = ecs_new(world);
    ecs_entity_t e = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_set(world, e, Position, {10, 20});
    ecs_set(world, e, Velocity, {1, 2});

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(ecs_id(Velocity), ecs_field_id(&it, 1));
    test_uint(ecs_pair(EcsChildOf, p), ecs_field_id(&it, 2));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 20);
    }
    test_assert(ecs_field(&it, Velocity, 1) != NULL);
    {
        Velocity *v = ecs_field(&it, Velocity, 1);
        test_assert(v != NULL);
        test_int(v->x, 1);
        test_int(v->y, 2);
    }
    test_uint(p, ecs_iter_get_var(&it, x_var));
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_1_trivial_1_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Foo(self), ChildOf($this, *)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_entity_t p = ecs_new(world);
    ecs_entity_t e = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_add(world, e, Foo);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e, it.entities[0]);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_pair(EcsChildOf, p), ecs_field_id(&it, 1));
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_2_trivial_1_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Foo(self), Bar(self), ChildOf($this, *)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_entity_t p = ecs_new(world);
    ecs_entity_t e = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_add(world, e, Foo);
    ecs_add(world, e, Bar);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e, it.entities[0]);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(Bar, ecs_field_id(&it, 1));
    test_uint(ecs_pair(EcsChildOf, p), ecs_field_id(&it, 2));
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_1_trivial_1_wildcard_component(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Position(self), ChildOf($this, *)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_entity_t p = ecs_new(world);
    ecs_entity_t e = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_set(world, e, Position, {10, 20});

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(ecs_pair(EcsChildOf, p), ecs_field_id(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 20);
    }
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_2_trivial_1_wildcard_component(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Position(self), Velocity(self), ChildOf($this, *)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_entity_t p = ecs_new(world);
    ecs_entity_t e = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_set(world, e, Position, {10, 20});
    ecs_set(world, e, Velocity, {1, 2});

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(ecs_id(Velocity), ecs_field_id(&it, 1));
    test_uint(ecs_pair(EcsChildOf, p), ecs_field_id(&it, 2));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 20);
    }
    test_assert(ecs_field(&it, Velocity, 1) != NULL);
    {
        Velocity *v = ecs_field(&it, Velocity, 1);
        test_assert(v != NULL);
        test_int(v->x, 1);
        test_int(v->y, 2);
    }
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_1_trivial_1_any(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Foo(self), ChildOf($this, _)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_entity_t p = ecs_new(world);
    ecs_entity_t e = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_add(world, e, Foo);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e, it.entities[0]);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_pair(EcsChildOf, EcsWildcard), ecs_field_id(&it, 1));
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_2_trivial_1_any(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Foo(self), Bar(self), ChildOf($this, _)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_entity_t p = ecs_new(world);
    ecs_entity_t e = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_add(world, e, Foo);
    ecs_add(world, e, Bar);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e, it.entities[0]);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(Bar, ecs_field_id(&it, 1));
    test_uint(ecs_pair(EcsChildOf, EcsWildcard), ecs_field_id(&it, 2));
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_1_trivial_1_any_component(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Position(self), ChildOf($this, _)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_entity_t p = ecs_new(world);
    ecs_entity_t e = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_set(world, e, Position, {10, 20});

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(ecs_pair(EcsChildOf, EcsWildcard), ecs_field_id(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 20);
    }
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_2_trivial_1_any_component(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Position(self), Velocity(self), ChildOf($this, _)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_entity_t p = ecs_new(world);
    ecs_entity_t e = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_set(world, e, Position, {10, 20});
    ecs_set(world, e, Velocity, {1, 2});

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(ecs_id(Velocity), ecs_field_id(&it, 1));
    test_uint(ecs_pair(EcsChildOf, EcsWildcard), ecs_field_id(&it, 2));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 20);
    }
    test_assert(ecs_field(&it, Velocity, 1) != NULL);
    {
        Velocity *v = ecs_field(&it, Velocity, 1);
        test_assert(v != NULL);
        test_int(v->x, 1);
        test_int(v->y, 2);
    }
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Variables_first_invalid_var_name_and_id(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    ecs_log_set_level(-4);
    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { .first.id = Tag, .first.name = "$this" }
        },
        .cache_kind = cache_kind
    });

    test_assert(q == NULL);

    ecs_fini(world);
}

void Variables_src_invalid_var_name_and_id(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    ecs_log_set_level(-4);
    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { .first.id = Tag, .src.id = Tag, .src.name = "$this" }
        },
        .cache_kind = cache_kind
    });

    test_assert(q == NULL);

    ecs_fini(world);
}

void Variables_second_invalid_var_name_and_id(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    ecs_log_set_level(-4);
    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { .first.id = Tag, .second.id = Tag, .second.name = "$this" }
        },
        .cache_kind = cache_kind
    });

    test_assert(q == NULL);

    ecs_fini(world);
}

