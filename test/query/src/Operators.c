#include <query.h>

static ecs_query_cache_kind_t cache_kind = EcsQueryCacheDefault;

void Operators_setup(void) {
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

void Operators_2_and_not(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);

    ecs_query_t *q = ecs_query(world, {
        .expr = "RelA($this), !RelB($this)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_entity_t e1 = ecs_new_w(world, RelA);
    ecs_entity_t e2 = ecs_new_w(world, RelA);
    ecs_add(world, e1, RelB);
    ecs_add(world, e2, RelB);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_remove(world, e1, RelB);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(RelB, ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_field_is_set(&it, 1));
        test_uint(e1, it.entities[0]);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Operators_2_and_not_component(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_COMPONENT(world, Position);

    ecs_query_t *q = ecs_query(world, {
        .expr = "RelA($this), !Position($this)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_entity_t e1 = ecs_new_w(world, RelA);
    ecs_entity_t e2 = ecs_new_w(world, RelA);
    ecs_add(world, e1, Position);
    ecs_add(world, e2, Position);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_remove(world, e1, Position);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_field_is_set(&it, 1));
        test_uint(e1, it.entities[0]);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Operators_2_and_out_not(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);

    ecs_query_t *q = ecs_query(world, {
        .expr = "RelA($this), [out] !RelB($this)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_entity_t e1 = ecs_new_w(world, RelA);
    ecs_entity_t e2 = ecs_new_w(world, RelA);
    ecs_add(world, e1, RelB);
    ecs_add(world, e2, RelB);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_remove(world, e1, RelB);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(RelB, ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_field_is_set(&it, 1));
        test_uint(e1, it.entities[0]);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Operators_2_and_out_not_component(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_COMPONENT(world, Position);

    ecs_query_t *q = ecs_query(world, {
        .expr = "RelA($this), [out] !Position($this)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_entity_t e1 = ecs_new_w(world, RelA);
    ecs_entity_t e2 = ecs_new_w(world, RelA);
    ecs_add(world, e1, Position);
    ecs_add(world, e2, Position);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_remove(world, e1, Position);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_field_is_set(&it, 1));
        test_uint(e1, it.entities[0]);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Operators_3_and_not_not(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, RelC);

    ecs_query_t *q = ecs_query(world, {
        .expr = "RelA($this), !RelB($this), !RelC($this)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_entity_t e1 = ecs_new_w(world, RelA);
    ecs_entity_t e2 = ecs_new_w(world, RelA);
    ecs_add(world, e1, RelB);
    ecs_add(world, e2, RelB);
    ecs_add(world, e1, RelC);
    ecs_add(world, e2, RelC);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_remove(world, e1, RelB);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_remove(world, e2, RelC);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_remove(world, e1, RelC);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(RelB, ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_field_is_set(&it, 2));
        test_uint(e1, it.entities[0]);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Operators_2_and_not_pair_rel_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, TgtA);

    ecs_query_t *q = ecs_query(world, {
        .expr = "RelA($this), !*($this, TgtA)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_entity_t e1 = ecs_new_w(world, RelA);
    ecs_entity_t e2 = ecs_new_w(world, RelA);
    ecs_add_pair(world, e1, RelA, TgtA);
    ecs_add_pair(world, e2, RelA, TgtA);
    ecs_add_pair(world, e1, RelB, TgtA);
    ecs_add_pair(world, e2, RelB, TgtA);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_remove_pair(world, e1, RelB, TgtA);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_remove_pair(world, e1, RelA, TgtA);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(ecs_pair(EcsWildcard, TgtA), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_field_is_set(&it, 1));
        test_uint(e1, it.entities[0]);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Operators_2_and_not_pair_tgt_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_query_t *q = ecs_query(world, {
        .expr = "RelA($this), !RelA($this, *)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_entity_t e1 = ecs_new_w(world, RelA);
    ecs_entity_t e2 = ecs_new_w(world, RelA);
    ecs_add_pair(world, e1, RelA, TgtA);
    ecs_add_pair(world, e2, RelA, TgtA);
    ecs_add_pair(world, e1, RelA, TgtB);
    ecs_add_pair(world, e2, RelA, TgtB);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_remove_pair(world, e1, RelA, TgtA);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_remove_pair(world, e1, RelA, TgtB);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelA, EcsWildcard), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_field_is_set(&it, 1));
        test_uint(e1, it.entities[0]);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Operators_2_and_not_pair_rel_tgt_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_query_t *q = ecs_query(world, {
        .expr = "RelA($this), !*($this, *)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_entity_t e1 = ecs_new_w(world, RelA);
    ecs_entity_t e2 = ecs_new_w(world, RelA);
    ecs_add_pair(world, e1, RelA, TgtA);
    ecs_add_pair(world, e2, RelA, TgtA);
    ecs_add_pair(world, e1, RelA, TgtB);
    ecs_add_pair(world, e2, RelA, TgtB);
    ecs_add_pair(world, e1, RelB, TgtA);
    ecs_add_pair(world, e2, RelB, TgtA);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_remove_pair(world, e1, RelA, TgtA);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_remove_pair(world, e1, RelA, TgtB);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_remove_pair(world, e1, RelB, TgtA);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(ecs_pair(EcsWildcard, EcsWildcard), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_field_is_set(&it, 1));
        test_uint(e1, it.entities[0]);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Operators_2_and_not_pair_rel_var(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, TgtA);

    ecs_query_t *q = ecs_query(world, {
        .expr = "RelA($this), !$x($this, TgtA)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int32_t x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_entity_t e1 = ecs_new_w(world, RelA);
    ecs_entity_t e2 = ecs_new_w(world, RelA);
    ecs_add_pair(world, e1, RelA, TgtA);
    ecs_add_pair(world, e2, RelA, TgtA);
    ecs_add_pair(world, e1, RelB, TgtA);
    ecs_add_pair(world, e2, RelB, TgtA);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_remove_pair(world, e1, RelB, TgtA);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_remove_pair(world, e1, RelA, TgtA);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(ecs_pair(EcsWildcard, TgtA), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_field_is_set(&it, 1));
        test_uint(EcsWildcard, ecs_iter_get_var(&it, x_var));
        test_uint(e1, it.entities[0]);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Operators_2_and_not_pair_tgt_var(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_query_t *q = ecs_query(world, {
        .expr = "RelA($this), !RelA($this, $x)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int32_t x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_entity_t e1 = ecs_new_w(world, RelA);
    ecs_entity_t e2 = ecs_new_w(world, RelA);
    ecs_add_pair(world, e1, RelA, TgtA);
    ecs_add_pair(world, e2, RelA, TgtA);
    ecs_add_pair(world, e1, RelA, TgtB);
    ecs_add_pair(world, e2, RelA, TgtB);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_remove_pair(world, e1, RelA, TgtA);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_remove_pair(world, e1, RelA, TgtB);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelA, EcsWildcard), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_field_is_set(&it, 1));
        test_uint(EcsWildcard, ecs_iter_get_var(&it, x_var));
        test_uint(e1, it.entities[0]);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Operators_2_and_not_pair_rel_tgt_var(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, TgtA);

    ecs_query_t *q = ecs_query(world, {
        .expr = "RelA($this), !$x($this, $y)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int32_t x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    int32_t y_var = ecs_query_find_var(q, "x");
    test_assert(y_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_entity_t e1 = ecs_new_w(world, RelA);
    ecs_entity_t e2 = ecs_new_w(world, RelA);
    ecs_add_pair(world, e1, RelA, TgtA);
    ecs_add_pair(world, e2, RelA, TgtA);
    ecs_add_pair(world, e1, RelB, TgtA);
    ecs_add_pair(world, e2, RelB, TgtA);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_remove_pair(world, e1, RelB, TgtA);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_remove_pair(world, e1, RelA, TgtA);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(ecs_pair(EcsWildcard, EcsWildcard), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_field_is_set(&it, 1));
        test_uint(EcsWildcard, ecs_iter_get_var(&it, x_var));
        test_uint(EcsWildcard, ecs_iter_get_var(&it, y_var));
        test_uint(e1, it.entities[0]);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Operators_2_and_not_pair_rel_tgt_same_var(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);

    ecs_query_t *q = ecs_query(world, {
        .expr = "RelA($this), !$x($this, $x)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int32_t x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_entity_t e1 = ecs_new_w(world, RelA);
    ecs_entity_t e2 = ecs_new_w(world, RelA);
    ecs_add_pair(world, e1, RelA, RelA);
    ecs_add_pair(world, e2, RelA, RelA);
    ecs_add_pair(world, e1, RelB, RelB);
    ecs_add_pair(world, e2, RelB, RelB);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_remove_pair(world, e1, RelA, RelA);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_remove_pair(world, e1, RelB, RelB);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(ecs_pair(EcsWildcard, EcsWildcard), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_field_is_set(&it, 1));
        test_uint(EcsWildcard, ecs_iter_get_var(&it, x_var));
        test_uint(e1, it.entities[0]);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Operators_2_and_not_pair_rel_var_written(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);
    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, TgtA);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Tag($this, $x), !$x($this, TgtA)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int32_t x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_entity_t e1 = ecs_new_w_pair(world, Tag, RelA);
    ecs_entity_t e2 = ecs_new_w_pair(world, Tag, RelB);
    ecs_add_pair(world, e1, RelA, TgtA);
    ecs_add_pair(world, e1, RelB, TgtA);
    ecs_add_pair(world, e2, RelA, TgtA);
    ecs_add_pair(world, e2, RelB, TgtA);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_remove_pair(world, e1, RelB, TgtA);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_remove_pair(world, e1, RelA, TgtA);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(Tag, RelA), ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_field_is_set(&it, 1));
        test_uint(RelA, ecs_iter_get_var(&it, x_var));
        test_uint(e1, it.entities[0]);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_remove_pair(world, e2, RelB, TgtA);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(Tag, RelA), ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_field_is_set(&it, 1));
        test_uint(RelA, ecs_iter_get_var(&it, x_var));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(Tag, RelB), ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelB, TgtA), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_field_is_set(&it, 1));
        test_uint(RelB, ecs_iter_get_var(&it, x_var));
        test_uint(e2, it.entities[0]);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Operators_2_and_not_pair_tgt_var_written(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);
    ECS_TAG(world, RelA);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Tag($this, $x), !RelA($this, $x)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int32_t x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_entity_t e1 = ecs_new_w_pair(world, Tag, TgtA);
    ecs_entity_t e2 = ecs_new_w_pair(world, Tag, TgtB);
    ecs_add_pair(world, e1, RelA, TgtA);
    ecs_add_pair(world, e1, RelA, TgtB);
    ecs_add_pair(world, e2, RelA, TgtA);
    ecs_add_pair(world, e2, RelA, TgtB);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_remove_pair(world, e1, RelA, TgtA);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(Tag, TgtA), ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_field_is_set(&it, 1));
        test_uint(TgtA, ecs_iter_get_var(&it, x_var));
        test_uint(e1, it.entities[0]);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_remove_pair(world, e2, RelA, TgtB);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(Tag, TgtA), ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_field_is_set(&it, 1));
        test_uint(TgtA, ecs_iter_get_var(&it, x_var));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(Tag, TgtB), ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelA, TgtB), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_field_is_set(&it, 1));
        test_uint(TgtB, ecs_iter_get_var(&it, x_var));
        test_uint(e2, it.entities[0]);

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Operators_2_and_not_pair_rel_tgt_var_written(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);
    ECS_TAG(world, RelA);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Tag($this, $x), !$x($this, $x)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int32_t x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_entity_t e1 = ecs_new_w_pair(world, Tag, TgtA);
    ecs_entity_t e2 = ecs_new_w_pair(world, Tag, TgtB);
    ecs_add_pair(world, e1, TgtA, TgtA);
    ecs_add_pair(world, e1, TgtB, TgtB);
    ecs_add_pair(world, e2, TgtA, TgtA);
    ecs_add_pair(world, e2, TgtB, TgtB);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_remove_pair(world, e1, TgtA, TgtA);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(Tag, TgtA), ecs_field_id(&it, 0));
        test_uint(ecs_pair(TgtA, TgtA), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_field_is_set(&it, 1));
        test_uint(TgtA, ecs_iter_get_var(&it, x_var));
        test_uint(e1, it.entities[0]);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_remove_pair(world, e2, TgtB, TgtB);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(Tag, TgtA), ecs_field_id(&it, 0));
        test_uint(ecs_pair(TgtA, TgtA), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_field_is_set(&it, 1));
        test_uint(TgtA, ecs_iter_get_var(&it, x_var));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(Tag, TgtB), ecs_field_id(&it, 0));
        test_uint(ecs_pair(TgtB, TgtB), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_field_is_set(&it, 1));
        test_uint(TgtB, ecs_iter_get_var(&it, x_var));
        test_uint(e2, it.entities[0]);

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Operators_2_and_not_pair_rel_tgt_same_var_written(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);
    ECS_TAG(world, RelA);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Tag($this, $x), !$x($this, $this)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int32_t x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_entity_t e1 = ecs_new_w_pair(world, Tag, TgtA);
    ecs_entity_t e2 = ecs_new_w_pair(world, Tag, TgtB);
    ecs_add_pair(world, e1, TgtA, e1);
    ecs_add_pair(world, e1, TgtB, e1);
    ecs_add_pair(world, e2, TgtA, e2);
    ecs_add_pair(world, e2, TgtB, e2);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_remove_pair(world, e1, TgtA, e1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(Tag, TgtA), ecs_field_id(&it, 0));
        test_uint(ecs_pair(TgtA, e1), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_field_is_set(&it, 1));
        test_uint(TgtA, ecs_iter_get_var(&it, x_var));
        test_uint(e1, it.entities[0]);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_remove_pair(world, e2, TgtB, e2);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(Tag, TgtA), ecs_field_id(&it, 0));
        test_uint(ecs_pair(TgtA, e1), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_field_is_set(&it, 1));
        test_uint(TgtA, ecs_iter_get_var(&it, x_var));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(Tag, TgtB), ecs_field_id(&it, 0));
        test_uint(ecs_pair(TgtB, e2), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_field_is_set(&it, 1));
        test_uint(TgtB, ecs_iter_get_var(&it, x_var));
        test_uint(e2, it.entities[0]);

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}


void Operators_2_and_not_pair_rel_src_tgt_same_var_written(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);

    ecs_query_t *q = ecs_query(world, {
        .expr = "RelA($x), !$x($x, $x)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int32_t x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_entity_t e1 = ecs_new_w(world, RelA);
    ecs_entity_t e2 = ecs_new_w(world, RelA);
    ecs_add_pair(world, e1, e1, e1);
    ecs_add_pair(world, e2, e2, e2);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_remove_pair(world, e1, e1, e1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(ecs_pair(e1, e1), ecs_field_id(&it, 1));
        test_uint(e1, ecs_field_src(&it, 0));
        test_uint(e1, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_field_is_set(&it, 1));
        test_uint(e1, ecs_iter_get_var(&it, x_var));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_remove_pair(world, e2, e2, e2);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(ecs_pair(e1, e1), ecs_field_id(&it, 1));
        test_uint(e1, ecs_field_src(&it, 0));
        test_uint(e1, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_field_is_set(&it, 1));
        test_uint(e1, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(ecs_pair(e2, e2), ecs_field_id(&it, 1));
        test_uint(e2, ecs_field_src(&it, 0));
        test_uint(e2, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_field_is_set(&it, 1));
        test_uint(e2, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Operators_2_and_not_pair_any_rel(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, TgtA);

    ecs_query_t *q = ecs_query(world, {
        .expr = "RelA($this), !_($this, TgtA)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_entity_t e1 = ecs_new_w(world, RelA);
    ecs_entity_t e2 = ecs_new_w(world, RelA);
    ecs_entity_t e3 = ecs_new_w(world, RelA);

    ecs_add_pair(world, e2, RelA, TgtA);
    ecs_add_pair(world, e3, RelA, TgtA);
    ecs_add_pair(world, e3, RelB, TgtA);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(ecs_pair(EcsWildcard, TgtA), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_field_is_set(&it, 1));
        test_uint(e1, it.entities[0]);

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Operators_2_and_not_pair_any_tgt(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_query_t *q = ecs_query(world, {
        .expr = "RelA($this), !RelA($this, _)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_entity_t e1 = ecs_new_w(world, RelA);
    ecs_entity_t e2 = ecs_new_w(world, RelA);
    ecs_entity_t e3 = ecs_new_w(world, RelA);

    ecs_add_pair(world, e2, RelA, TgtA);
    ecs_add_pair(world, e3, RelA, TgtA);
    ecs_add_pair(world, e3, RelA, TgtB);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelA, EcsWildcard), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_field_is_set(&it, 1));
        test_uint(e1, it.entities[0]);

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Operators_2_and_not_pair_any_src(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);
    ECS_TAG(world, RelA);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);
    ECS_TAG(world, TgtC);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Tag($this, $x), !RelA(_, $x)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_entity_t e1 = ecs_new_w_pair(world, Tag, TgtA);
    ecs_new_w_pair(world, Tag, TgtB);
    ecs_new_w_pair(world, Tag, TgtC);

    ecs_entity_t e4 = ecs_new_w_pair(world, RelA, TgtB);
    ecs_add_pair(world, e4, RelA, TgtC);
    ecs_new_w_pair(world, RelA, TgtC);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(Tag, TgtA), ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_field_is_set(&it, 1));
        test_uint(e1, it.entities[0]);

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Operators_1_not_any_src_fixed_first(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_query_t *q = ecs_query(world, {
        .expr = "!RelA(_)"
    });

    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(false, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_new_w(world, RelA);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_new_w(world, RelA);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Operators_1_not_any_src_any_tgt_fixed_first(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_query_t *q = ecs_query(world, {
        .expr = "!RelA(_, _)"
    });

    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(RelA, EcsWildcard), ecs_field_id(&it, 0));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(false, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_new_w_pair(world, TgtA, RelA);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(RelA, EcsWildcard), ecs_field_id(&it, 0));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(false, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_new_w_pair(world, RelA, TgtA);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_new_w_pair(world, RelA, TgtB);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Operators_1_not_any_src_any_first_fixed_tgt(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, TgtA);

    ecs_query_t *q = ecs_query(world, {
        .expr = "!_(_, TgtA)"
    });

    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(EcsWildcard, TgtA), ecs_field_id(&it, 0));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(false, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_new_w_pair(world, TgtA, RelA);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(EcsWildcard, TgtA), ecs_field_id(&it, 0));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(false, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_new_w_pair(world, RelA, TgtA);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_new_w_pair(world, RelB, TgtA);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Operators_1_not_any_src_any_childof_pair_any_tgt(void) {
    ecs_world_t *world = ecs_mini();

    ecs_query_t *q = ecs_query(world, {
        .expr = "!ChildOf(_, _)"
    });

    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Operators_1_not_any_src_any_isa_pair_any_tgt(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TgtA);

    ecs_query_t *q = ecs_query(world, {
        .expr = "!IsA(_, _)"
    });

    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(EcsIsA, EcsWildcard), ecs_field_id(&it, 0));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(false, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_new_w_pair(world, EcsIsA, TgtA);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Operators_2_and_optional(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);

    ecs_query_t *q = ecs_query(world, {
        .expr = "RelA($this), ?RelB($this)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_entity_t e1 = ecs_new_w(world, RelA);
    ecs_entity_t e2 = ecs_new_w(world, RelA);
    ecs_add(world, e2, RelB);

    {
        ecs_iter_t it = ecs_query_iter(world, q);   
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(RelB, ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_field_is_set(&it, 1));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(RelB, ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(e2, it.entities[0]);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Operators_3_and_optional_optional(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, RelC);

    ecs_query_t *q = ecs_query(world, {
        .expr = "RelA($this), ?RelB($this), ?RelC($this)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_entity_t e1 = ecs_new_w(world, RelA);
    ecs_entity_t e2 = ecs_new_w(world, RelA);
    ecs_entity_t e3 = ecs_new_w(world, RelA);
    ecs_add(world, e2, RelB);
    ecs_add(world, e3, RelC);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(RelB, ecs_field_id(&it, 1));
        test_uint(RelC, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_field_is_set(&it, 2));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(RelB, ecs_field_id(&it, 1));
        test_uint(RelC, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_field_is_set(&it, 2));
        test_uint(e2, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(RelB, ecs_field_id(&it, 1));
        test_uint(RelC, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(e3, it.entities[0]);

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Operators_2_and_optional_pair_rel_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, TgtA);

    ecs_query_t *q = ecs_query(world, {
        .expr = "RelA($this), ?*($this, TgtA)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_entity_t e1 = ecs_new_w(world, RelA);
    ecs_entity_t e2 = ecs_new_w(world, RelA);
    ecs_entity_t e3 = ecs_new_w(world, RelA);
    ecs_add_pair(world, e2, RelA, TgtA);
    ecs_add_pair(world, e3, RelA, TgtA);
    ecs_add_pair(world, e3, RelB, TgtA);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(ecs_pair(EcsWildcard, TgtA), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_field_is_set(&it, 1));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(e2, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(e3, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelB, TgtA), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(e3, it.entities[0]);

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Operators_2_and_optional_pair_tgt_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_query_t *q = ecs_query(world, {
        .expr = "RelA($this), ?RelA($this, *)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_entity_t e1 = ecs_new_w(world, RelA);
    ecs_entity_t e2 = ecs_new_w(world, RelA);
    ecs_entity_t e3 = ecs_new_w(world, RelA);
    ecs_add_pair(world, e2, RelA, TgtA);
    ecs_add_pair(world, e3, RelA, TgtA);
    ecs_add_pair(world, e3, RelA, TgtB);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelA, EcsWildcard), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_field_is_set(&it, 1));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(e2, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(e3, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelA, TgtB), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(e3, it.entities[0]);

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Operators_2_and_optional_pair_rel_var(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, TgtA);

    ecs_query_t *q = ecs_query(world, {
        .expr = "RelA($this), ?$x($this, TgtA)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_entity_t e1 = ecs_new_w(world, RelA);
    ecs_entity_t e2 = ecs_new_w(world, RelA);
    ecs_entity_t e3 = ecs_new_w(world, RelA);
    ecs_add_pair(world, e2, RelA, TgtA);
    ecs_add_pair(world, e3, RelA, TgtA);
    ecs_add_pair(world, e3, RelB, TgtA);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(ecs_pair(EcsWildcard, TgtA), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_field_is_set(&it, 1));
        test_uint(EcsWildcard, ecs_iter_get_var(&it, x_var));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(RelA, ecs_iter_get_var(&it, x_var));
        test_uint(e2, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(RelA, ecs_iter_get_var(&it, x_var));
        test_uint(e3, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelB, TgtA), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(RelB, ecs_iter_get_var(&it, x_var));
        test_uint(e3, it.entities[0]);

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Operators_2_and_optional_pair_tgt_var(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_query_t *q = ecs_query(world, {
        .expr = "RelA($this), ?RelA($this, $x)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_entity_t e1 = ecs_new_w(world, RelA);
    ecs_entity_t e2 = ecs_new_w(world, RelA);
    ecs_entity_t e3 = ecs_new_w(world, RelA);
    ecs_add_pair(world, e2, RelA, TgtA);
    ecs_add_pair(world, e3, RelA, TgtA);
    ecs_add_pair(world, e3, RelA, TgtB);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelA, EcsWildcard), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_field_is_set(&it, 1));
        test_uint(EcsWildcard, ecs_iter_get_var(&it, x_var));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(TgtA, ecs_iter_get_var(&it, x_var));
        test_uint(e2, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(TgtA, ecs_iter_get_var(&it, x_var));
        test_uint(e3, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelA, TgtB), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(TgtB, ecs_iter_get_var(&it, x_var));
        test_uint(e3, it.entities[0]);

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Operators_2_and_optional_pair_rel_tgt_var(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_query_t *q = ecs_query(world, {
        .expr = "RelA($this), ?$x($this, $y)",
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

    ecs_entity_t e1 = ecs_new_w(world, RelA);
    ecs_entity_t e2 = ecs_new_w(world, RelA);
    ecs_entity_t e3 = ecs_new_w(world, RelA);
    ecs_add_pair(world, e2, RelA, TgtA);
    ecs_add_pair(world, e3, RelA, TgtA);
    ecs_add_pair(world, e3, RelA, TgtB);
    ecs_add_pair(world, e3, RelB, TgtA);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(ecs_pair(EcsWildcard, EcsWildcard), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_field_is_set(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(EcsWildcard, ecs_iter_get_var(&it, x_var));
        test_uint(EcsWildcard, ecs_iter_get_var(&it, y_var));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(RelA, ecs_iter_get_var(&it, x_var));
        test_uint(TgtA, ecs_iter_get_var(&it, y_var));
        test_uint(e2, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(RelA, ecs_iter_get_var(&it, x_var));
        test_uint(TgtA, ecs_iter_get_var(&it, y_var));
        test_uint(e3, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelA, TgtB), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(RelA, ecs_iter_get_var(&it, x_var));
        test_uint(TgtB, ecs_iter_get_var(&it, y_var));
        test_uint(e3, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelB, TgtA), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
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

void Operators_2_and_optional_pair_rel_tgt_same_var(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_query_t *q = ecs_query(world, {
        .expr = "RelA($this), ?$x($this, $x)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_entity_t e1 = ecs_new_w(world, RelA);
    ecs_entity_t e2 = ecs_new_w(world, RelA);
    ecs_entity_t e3 = ecs_new_w(world, RelA);

    ecs_add_pair(world, e1, RelA, TgtA);
    ecs_add_pair(world, e2, RelA, TgtA);
    ecs_add_pair(world, e3, RelA, TgtA);

    ecs_add_pair(world, e2, TgtA, TgtA);
    ecs_add_pair(world, e3, TgtA, TgtA);
    ecs_add_pair(world, e3, RelA, RelA);

    {
        ecs_iter_t it = ecs_query_iter(world, q);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(e1, it.entities[0]);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(ecs_pair(EcsWildcard, EcsWildcard), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_field_is_set(&it, 1));
        test_uint(EcsWildcard, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(e2, it.entities[0]);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(ecs_pair(TgtA, TgtA), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(TgtA, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(e3, it.entities[0]);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelA, RelA), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(RelA, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(e3, it.entities[0]);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(ecs_pair(TgtA, TgtA), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(TgtA, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Operators_2_and_optional_pair_rel_var_written(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);
    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, TgtA);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Tag($this, $x), ?$x($this, TgtA)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_entity_t e1 = ecs_new_w_pair(world, Tag, RelA);
    ecs_entity_t e2 = ecs_new_w_pair(world, Tag, RelA);
    ecs_entity_t e3 = ecs_new_w_pair(world, Tag, RelA);
    ecs_add_pair(world, e3, Tag, RelB);

    ecs_add_pair(world, e2, RelA, TgtA);
    ecs_add_pair(world, e2, RelB, TgtA);

    ecs_add_pair(world, e3, RelA, TgtA);
    ecs_add_pair(world, e3, RelB, TgtA);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(Tag, RelA), ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_field_is_set(&it, 1));
        test_uint(RelA, ecs_iter_get_var(&it, x_var));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(Tag, RelA), ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(RelA, ecs_iter_get_var(&it, x_var));
        test_uint(e2, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(Tag, RelA), ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(RelA, ecs_iter_get_var(&it, x_var));
        test_uint(e3, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(Tag, RelB), ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelB, TgtA), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(RelB, ecs_iter_get_var(&it, x_var));
        test_uint(e3, it.entities[0]);

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Operators_2_and_optional_pair_tgt_var_written(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);
    ECS_TAG(world, RelA);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Tag($this, $x), ?RelA($this, $x)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_entity_t e1 = ecs_new_w_pair(world, Tag, TgtA);
    ecs_entity_t e2 = ecs_new_w_pair(world, Tag, TgtA);
    ecs_entity_t e3 = ecs_new_w_pair(world, Tag, TgtA);
    ecs_add_pair(world, e3, Tag, TgtB);

    ecs_add_pair(world, e2, RelA, TgtA);
    ecs_add_pair(world, e2, RelA, TgtB);

    ecs_add_pair(world, e3, RelA, TgtA);
    ecs_add_pair(world, e3, RelA, TgtB);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(Tag, TgtA), ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_field_is_set(&it, 1));
        test_uint(TgtA, ecs_iter_get_var(&it, x_var));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(Tag, TgtA), ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(TgtA, ecs_iter_get_var(&it, x_var));
        test_uint(e2, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(Tag, TgtA), ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(TgtA, ecs_iter_get_var(&it, x_var));
        test_uint(e3, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(Tag, TgtB), ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelA, TgtB), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(TgtB, ecs_iter_get_var(&it, x_var));
        test_uint(e3, it.entities[0]);

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Operators_2_and_optional_pair_rel_tgt_var_written(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);
    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Tag($this, $x), ?$x($this, $y)",
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

    ecs_entity_t e1 = ecs_new_w_pair(world, Tag, RelA);
    ecs_entity_t e2 = ecs_new_w_pair(world, Tag, RelA);
    ecs_entity_t e3 = ecs_new_w_pair(world, Tag, RelA);
    ecs_add_pair(world, e3, Tag, RelB);

    ecs_add_pair(world, e2, RelA, TgtA);
    ecs_add_pair(world, e2, RelB, TgtA);

    ecs_add_pair(world, e3, RelA, TgtA);
    ecs_add_pair(world, e3, RelB, TgtA);
    ecs_add_pair(world, e3, RelA, TgtB);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(Tag, RelA), ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelA, EcsWildcard), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_field_is_set(&it, 1));
        test_uint(RelA, ecs_iter_get_var(&it, x_var));
        test_uint(EcsWildcard, ecs_iter_get_var(&it, y_var));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(Tag, RelA), ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(RelA, ecs_iter_get_var(&it, x_var));
        test_uint(TgtA, ecs_iter_get_var(&it, y_var));
        test_uint(e2, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(Tag, RelA), ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(RelA, ecs_iter_get_var(&it, x_var));
        test_uint(TgtA, ecs_iter_get_var(&it, y_var));
        test_uint(e3, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(Tag, RelA), ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelA, TgtB), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(RelA, ecs_iter_get_var(&it, x_var));
        test_uint(TgtB, ecs_iter_get_var(&it, y_var));
        test_uint(e3, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(Tag, RelB), ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelB, TgtA), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(RelB, ecs_iter_get_var(&it, x_var));
        test_uint(TgtA, ecs_iter_get_var(&it, y_var));
        test_uint(e3, it.entities[0]);

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Operators_2_and_optional_pair_rel_tgt_same_var_written(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);
    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Tag($this, $x), ?$x($this, $x)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_entity_t e1 = ecs_new_w_pair(world, Tag, TgtA);
    ecs_entity_t e2 = ecs_new_w_pair(world, Tag, TgtA);
    ecs_entity_t e3 = ecs_new_w_pair(world, Tag, TgtA);
    ecs_add_pair(world, e3, Tag, RelA);

    ecs_add_pair(world, e1, RelA, TgtA);
    ecs_add_pair(world, e2, RelA, TgtA);
    ecs_add_pair(world, e3, RelA, TgtA);

    ecs_add_pair(world, e2, TgtA, TgtA);
    ecs_add_pair(world, e3, TgtA, TgtA);
    ecs_add_pair(world, e3, RelA, RelA);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(Tag, TgtA), ecs_field_id(&it, 0));
        test_uint(ecs_pair(TgtA, TgtA), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_field_is_set(&it, 1));
        test_uint(TgtA, ecs_iter_get_var(&it, x_var));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(Tag, TgtA), ecs_field_id(&it, 0));
        test_uint(ecs_pair(TgtA, TgtA), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(TgtA, ecs_iter_get_var(&it, x_var));
        test_uint(e2, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(Tag, RelA), ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelA, RelA), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(RelA, ecs_iter_get_var(&it, x_var));
        test_uint(e3, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(Tag, TgtA), ecs_field_id(&it, 0));
        test_uint(ecs_pair(TgtA, TgtA), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(TgtA, ecs_iter_get_var(&it, x_var));
        test_uint(e3, it.entities[0]);

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Operators_2_and_optional_pair_rel_src_tgt_same_var_written(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);

    ecs_query_t *q = ecs_query(world, {
        .expr = "RelA($x), ?$x($x, $x)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int32_t x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_entity_t e1 = ecs_new_w(world, RelA);
    ecs_entity_t e2 = ecs_new_w(world, RelA);
    ecs_entity_t e3 = ecs_new_w(world, RelA);
    ecs_add_pair(world, e2, e2, e2);
    ecs_add_pair(world, e3, e3, e3);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(ecs_pair(e1, e1), ecs_field_id(&it, 1));
        test_uint(e1, ecs_field_src(&it, 0));
        test_uint(e1, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_field_is_set(&it, 1));
        test_uint(e1, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(ecs_pair(e2, e2), ecs_field_id(&it, 1));
        test_uint(e2, ecs_field_src(&it, 0));
        test_uint(e2, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(e2, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(ecs_pair(e3, e3), ecs_field_id(&it, 1));
        test_uint(e3, ecs_field_src(&it, 0));
        test_uint(e3, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(e3, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Operators_3_and_optional_optional_pair_w_var(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, RelC);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_query_t *q = ecs_query(world, {
        .expr = "RelA($this), ?RelB($this, $x), ?RelC($this, $y)",
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

    ecs_entity_t e1 = ecs_new_w(world, RelA);
    ecs_entity_t e2 = ecs_new_w(world, RelA);
    ecs_entity_t e3 = ecs_new_w(world, RelA);
    ecs_add_pair(world, e2, RelB, TgtA);
    ecs_add_pair(world, e3, RelC, TgtB);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelB, EcsWildcard), ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelC, EcsWildcard), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_field_is_set(&it, 2));
        test_uint(EcsWildcard, ecs_iter_get_var(&it, x_var));
        test_uint(EcsWildcard, ecs_iter_get_var(&it, y_var));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelB, TgtA), ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelC, EcsWildcard), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_field_is_set(&it, 2));
        test_uint(TgtA, ecs_iter_get_var(&it, x_var));
        test_uint(EcsWildcard, ecs_iter_get_var(&it, y_var));
        test_uint(e2, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelB, EcsWildcard), ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelC, TgtB), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(EcsWildcard, ecs_iter_get_var(&it, x_var));
        test_uint(TgtB, ecs_iter_get_var(&it, y_var));
        test_uint(e3, it.entities[0]);

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Operators_2_and_optional_pair_any_rel(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, TgtA);

    ecs_query_t *q = ecs_query(world, {
        .expr = "RelA($this), ?_($this, TgtA)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_entity_t e1 = ecs_new_w(world, RelA);
    ecs_entity_t e2 = ecs_new_w(world, RelA);
    ecs_entity_t e3 = ecs_new_w(world, RelA);

    ecs_add_pair(world, e2, RelA, TgtA);
    ecs_add_pair(world, e3, RelA, TgtA);
    ecs_add_pair(world, e3, RelB, TgtA);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(ecs_pair(EcsWildcard, TgtA), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_field_is_set(&it, 1));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(ecs_pair(EcsWildcard, TgtA), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(e2, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(ecs_pair(EcsWildcard, TgtA), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(e3, it.entities[0]);

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Operators_2_and_optional_pair_any_tgt(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_query_t *q = ecs_query(world, {
        .expr = "RelA($this), ?RelA($this, _)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_entity_t e1 = ecs_new_w(world, RelA);
    ecs_entity_t e2 = ecs_new_w(world, RelA);
    ecs_entity_t e3 = ecs_new_w(world, RelA);

    ecs_add_pair(world, e2, RelA, TgtA);
    ecs_add_pair(world, e3, RelA, TgtA);
    ecs_add_pair(world, e3, RelA, TgtB);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelA, EcsWildcard), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_field_is_set(&it, 1));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelA, EcsWildcard), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(e2, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelA, EcsWildcard), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(e3, it.entities[0]);

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Operators_2_and_optional_pair_any_src(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);
    ECS_TAG(world, RelA);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);
    ECS_TAG(world, TgtC);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Tag($this, $x), ?RelA(_, $x)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int32_t x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_entity_t e1 = ecs_new_w_pair(world, Tag, TgtA);
    ecs_entity_t e2 = ecs_new_w_pair(world, Tag, TgtB);
    ecs_entity_t e3 = ecs_new_w_pair(world, Tag, TgtC);

    ecs_entity_t e4 = ecs_new_w_pair(world, RelA, TgtB);
    ecs_add_pair(world, e4, RelA, TgtC);
    ecs_new_w_pair(world, RelA, TgtC);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(Tag, TgtA), ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_field_is_set(&it, 1));
        test_uint(TgtA, ecs_iter_get_var(&it, x_var));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(Tag, TgtB), ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelA, TgtB), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(EcsWildcard, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(TgtB, ecs_iter_get_var(&it, x_var));
        test_uint(e2, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(Tag, TgtC), ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelA, TgtC), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(EcsWildcard, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(TgtC, ecs_iter_get_var(&it, x_var));
        test_uint(e3, it.entities[0]);

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Operators_3_and_optional_dependent_and_pair_rel(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_query_t *q = ecs_query(world, {
        .expr = "RelA($this), ?$x($this, TgtA), $x($this, TgtB)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != 0);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_entity_t e1 = ecs_new_w(world, RelA);
    ecs_entity_t e2 = ecs_new_w(world, RelA);
    ecs_entity_t e3 = ecs_new_w(world, RelA);
    ecs_entity_t e4 = ecs_new_w(world, RelA);

    ecs_add_pair(world, e1, RelA, TgtA);

    ecs_add_pair(world, e2, RelA, TgtB);

    ecs_add_pair(world, e3, RelA, TgtA);
    ecs_add_pair(world, e3, RelA, TgtB);
    ecs_add_pair(world, e3, RelB, TgtB);

    ecs_add_pair(world, e4, RelA, TgtA);
    ecs_add_pair(world, e4, RelA, TgtB);
    ecs_add_pair(world, e4, RelB, TgtA);
    ecs_add_pair(world, e4, RelB, TgtB);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(ecs_pair(EcsWildcard, TgtA), ecs_field_id(&it, 1));
        test_uint(ecs_pair(EcsWildcard, TgtB), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_field_is_set(&it, 2));
        test_uint(EcsWildcard, ecs_iter_get_var(&it, x_var));
        test_uint(e2, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelA, TgtB), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(RelA, ecs_iter_get_var(&it, x_var));
        test_uint(e3, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelA, TgtB), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(RelA, ecs_iter_get_var(&it, x_var));
        test_uint(e4, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelB, TgtA), ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelB, TgtB), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(RelB, ecs_iter_get_var(&it, x_var));
        test_uint(e4, it.entities[0]);

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Operators_3_and_optional_dependent_and_pair_tgt(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_query_t *q = ecs_query(world, {
        .expr = "RelA($this), ?RelA($this, $x), RelB($this, $x)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != 0);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_entity_t e1 = ecs_new_w(world, RelA);
    ecs_entity_t e2 = ecs_new_w(world, RelA);
    ecs_entity_t e3 = ecs_new_w(world, RelA);
    ecs_entity_t e4 = ecs_new_w(world, RelA);

    ecs_add_pair(world, e1, RelA, TgtA);

    ecs_add_pair(world, e2, RelB, TgtA);

    ecs_add_pair(world, e3, RelA, TgtA);
    ecs_add_pair(world, e3, RelB, TgtA);
    ecs_add_pair(world, e3, RelB, TgtB);

    ecs_add_pair(world, e4, RelA, TgtA);
    ecs_add_pair(world, e4, RelA, TgtB);
    ecs_add_pair(world, e4, RelB, TgtA);
    ecs_add_pair(world, e4, RelB, TgtB);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelA, EcsWildcard), ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelB, EcsWildcard), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_field_is_set(&it, 2));
        test_uint(EcsWildcard, ecs_iter_get_var(&it, x_var));
        test_uint(e2, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelB, TgtA), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(TgtA, ecs_iter_get_var(&it, x_var));
        test_uint(e3, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelB, TgtA), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(TgtA, ecs_iter_get_var(&it, x_var));
        test_uint(e4, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelA, TgtB), ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelB, TgtB), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(TgtB, ecs_iter_get_var(&it, x_var));
        test_uint(e4, it.entities[0]);

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Operators_3_and_optional_dependent_and_pair_rel_tgt(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_query_t *q = ecs_query(world, {
        .expr = "RelA($this), ?$x($this, $y), $y($this, $x)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != 0);

    int y_var = ecs_query_find_var(q, "y");
    test_assert(y_var != 0);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_entity_t e1 = ecs_new_w(world, RelA);
    ecs_entity_t e2 = ecs_new_w(world, RelA);
    ecs_entity_t e3 = ecs_new_w(world, RelA);

    ecs_add_pair(world, e2, RelA, TgtA);

    ecs_add_pair(world, e3, RelA, TgtA);
    ecs_add_pair(world, e3, TgtA, RelA);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(ecs_pair(EcsWildcard, EcsWildcard), ecs_field_id(&it, 1));
        test_uint(ecs_pair(EcsWildcard, EcsWildcard), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_field_is_set(&it, 2));
        test_uint(EcsWildcard, ecs_iter_get_var(&it, x_var));
        test_uint(EcsWildcard, ecs_iter_get_var(&it, y_var));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 1));
        test_uint(ecs_pair(TgtA, RelA), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(RelA, ecs_iter_get_var(&it, x_var));
        test_uint(TgtA, ecs_iter_get_var(&it, y_var));
        test_uint(e3, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(ecs_pair(TgtA, RelA), ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(TgtA, ecs_iter_get_var(&it, x_var));
        test_uint(RelA, ecs_iter_get_var(&it, y_var));
        test_uint(e3, it.entities[0]);

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Operators_3_and_optional_dependent_and_pair_rel_tgt_same_var(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_query_t *q = ecs_query(world, {
        .expr = "RelA($this), ?RelA($this, $x), $x($this, $x)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != 0);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_entity_t e1 = ecs_new_w(world, RelA);
    ecs_entity_t e2 = ecs_new_w(world, RelA);
    ecs_entity_t e3 = ecs_new_w(world, RelA);

    ecs_add_pair(world, e2, RelA, TgtA);

    ecs_add_pair(world, e3, RelA, TgtA);
    ecs_add_pair(world, e3, TgtA, TgtA);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelA, EcsWildcard), ecs_field_id(&it, 1));
        test_uint(ecs_pair(EcsWildcard, EcsWildcard), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_field_is_set(&it, 2));
        test_uint(EcsWildcard, ecs_iter_get_var(&it, x_var));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 1));
        test_uint(ecs_pair(TgtA, TgtA), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(TgtA, ecs_iter_get_var(&it, x_var));
        test_uint(e3, it.entities[0]);

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Operators_3_and_optional_dependent_and_pair_rel_tgt_same_other_var(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);
    ECS_TAG(world, TgtC);

    ecs_query_t *q = ecs_query(world, {
        .expr = "RelA($this), ?RelA($this, $x), $y($x, $y)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != 0);

    int y_var = ecs_query_find_var(q, "y");
    test_assert(y_var != 0);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_entity_t e1 = ecs_new_w(world, RelA);
    ecs_entity_t e2 = ecs_new_w(world, RelA);
    ecs_entity_t e3 = ecs_new_w(world, RelA);

    ecs_entity_t t1 = ecs_new(world);
    ecs_entity_t t2 = ecs_new_w_pair(world, TgtA, TgtA);
    ecs_entity_t t3 = ecs_new_w_pair(world, TgtB, TgtB);
    ecs_add_pair(world, t3, TgtC, TgtC);

    ecs_add_pair(world, e2, RelA, t1);
    ecs_add_pair(world, e3, RelA, t2);
    ecs_add_pair(world, e3, RelA, t3);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelA, EcsWildcard), ecs_field_id(&it, 1));
        test_uint(ecs_pair(EcsWildcard, EcsWildcard), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(EcsWildcard, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_field_is_set(&it, 2));
        test_uint(EcsWildcard, ecs_iter_get_var(&it, x_var));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelA, t2), ecs_field_id(&it, 1));
        test_uint(ecs_pair(TgtA, TgtA), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(t2, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(t2, ecs_iter_get_var(&it, x_var));
        test_uint(e3, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelA, t3), ecs_field_id(&it, 1));
        test_uint(ecs_pair(TgtB, TgtB), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(t3, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(t3, ecs_iter_get_var(&it, x_var));
        test_uint(e3, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelA, t3), ecs_field_id(&it, 1));
        test_uint(ecs_pair(TgtC, TgtC), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(t3, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(t3, ecs_iter_get_var(&it, x_var));
        test_uint(e3, it.entities[0]);

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Operators_3_and_optional_dependent_and_pair_src(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);
    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_query_t *q = ecs_query(world, {
        .expr = "RelA($this), ?RelA($this, $x), Tag($x)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != 0);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_entity_t e1 = ecs_new_w(world, RelA);
    ecs_entity_t e2 = ecs_new_w(world, RelA);
    ecs_entity_t e3 = ecs_new_w(world, RelA);

    ecs_entity_t t1 = ecs_new(world);
    ecs_entity_t t2 = ecs_new_w(world, Tag);
    ecs_entity_t t3 = ecs_new_w(world, Tag);

    ecs_add_pair(world, e2, RelA, t1);
    ecs_add_pair(world, e3, RelA, t2);
    ecs_add_pair(world, e3, RelA, t3);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelA, EcsWildcard), ecs_field_id(&it, 1));
        test_uint(Tag, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(EcsWildcard, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_field_is_set(&it, 2));
        test_uint(EcsWildcard, ecs_iter_get_var(&it, x_var));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelA, t2), ecs_field_id(&it, 1));
        test_uint(Tag, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(t2, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(t2, ecs_iter_get_var(&it, x_var));
        test_uint(e3, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelA, t3), ecs_field_id(&it, 1));
        test_uint(Tag, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(t3, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(t3, ecs_iter_get_var(&it, x_var));
        test_uint(e3, it.entities[0]);

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Operators_3_and_optional_dependent_optional_pair_rel(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_query_t *q = ecs_query(world, {
        .expr = "RelA($this), ?$x($this, TgtA), ?$x($this, TgtB)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != 0);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_entity_t e1 = ecs_new_w(world, RelA);
    ecs_entity_t e2 = ecs_new_w(world, RelA);
    ecs_entity_t e3 = ecs_new_w(world, RelA);
    ecs_entity_t e4 = ecs_new_w(world, RelA);

    ecs_add_pair(world, e1, RelA, TgtA);

    ecs_add_pair(world, e2, RelA, TgtB);

    ecs_add_pair(world, e3, RelA, TgtA);
    ecs_add_pair(world, e3, RelA, TgtB);
    ecs_add_pair(world, e3, RelB, TgtB);

    ecs_add_pair(world, e4, RelA, TgtA);
    ecs_add_pair(world, e4, RelA, TgtB);
    ecs_add_pair(world, e4, RelB, TgtA);
    ecs_add_pair(world, e4, RelB, TgtB);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(e1, it.entities[0]);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelA, TgtB), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_field_is_set(&it, 2));
        test_uint(RelA, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(e2, it.entities[0]);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(ecs_pair(EcsWildcard, TgtA), ecs_field_id(&it, 1));
        test_uint(ecs_pair(EcsWildcard, TgtB), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_field_is_set(&it, 2));
        test_uint(EcsWildcard, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(e3, it.entities[0]);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelA, TgtB), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(RelA, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(e4, it.entities[0]);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelA, TgtB), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(RelA, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(e4, it.entities[0]);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelB, TgtA), ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelB, TgtB), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(RelB, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Operators_3_and_optional_dependent_optional_pair_tgt(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_query_t *q = ecs_query(world, {
        .expr = "RelA($this), ?RelA($this, $x), ?RelB($this, $x)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != 0);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_entity_t e1 = ecs_new_w(world, RelA);
    ecs_entity_t e2 = ecs_new_w(world, RelA);
    ecs_entity_t e3 = ecs_new_w(world, RelA);
    ecs_entity_t e4 = ecs_new_w(world, RelA);

    ecs_add_pair(world, e1, RelA, TgtA);

    ecs_add_pair(world, e2, RelB, TgtA);

    ecs_add_pair(world, e3, RelA, TgtA);
    ecs_add_pair(world, e3, RelB, TgtA);
    ecs_add_pair(world, e3, RelB, TgtB);

    ecs_add_pair(world, e4, RelA, TgtA);
    ecs_add_pair(world, e4, RelA, TgtB);
    ecs_add_pair(world, e4, RelB, TgtA);
    ecs_add_pair(world, e4, RelB, TgtB);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelB, TgtA), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_field_is_set(&it, 2));
        test_uint(TgtA, ecs_iter_get_var(&it, x_var));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelA, EcsWildcard), ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelB, EcsWildcard), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_field_is_set(&it, 2));
        test_uint(EcsWildcard, ecs_iter_get_var(&it, x_var));
        test_uint(e2, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelB, TgtA), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(TgtA, ecs_iter_get_var(&it, x_var));
        test_uint(e3, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelB, TgtA), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(TgtA, ecs_iter_get_var(&it, x_var));
        test_uint(e4, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelA, TgtB), ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelB, TgtB), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(TgtB, ecs_iter_get_var(&it, x_var));
        test_uint(e4, it.entities[0]);

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Operators_3_and_optional_dependent_optional_pair_src(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);
    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_query_t *q = ecs_query(world, {
        .expr = "RelA($this), ?RelA($this, $x), ?Tag($x)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != 0);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_entity_t e1 = ecs_new_w(world, RelA);
    ecs_entity_t e2 = ecs_new_w(world, RelA);
    ecs_entity_t e3 = ecs_new_w(world, RelA);

    ecs_entity_t t1 = ecs_new(world);
    ecs_entity_t t2 = ecs_new_w(world, Tag);
    ecs_entity_t t3 = ecs_new_w(world, Tag);

    ecs_add_pair(world, e2, RelA, t1);
    ecs_add_pair(world, e3, RelA, t2);
    ecs_add_pair(world, e3, RelA, t3);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelA, EcsWildcard), ecs_field_id(&it, 1));
        test_uint(Tag, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(EcsWildcard, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_field_is_set(&it, 2));
        test_uint(EcsWildcard, ecs_iter_get_var(&it, x_var));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelA, t1), ecs_field_id(&it, 1));
        test_uint(Tag, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(t1, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_field_is_set(&it, 2));
        test_uint(t1, ecs_iter_get_var(&it, x_var));
        test_uint(e2, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelA, t2), ecs_field_id(&it, 1));
        test_uint(Tag, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(t2, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(t2, ecs_iter_get_var(&it, x_var));
        test_uint(e3, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelA, t3), ecs_field_id(&it, 1));
        test_uint(Tag, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(t3, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(t3, ecs_iter_get_var(&it, x_var));
        test_uint(e3, it.entities[0]);

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Operators_3_and_optional_dependent_not_pair_rel(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_query_t *q = ecs_query(world, {
        .expr = "RelA($this), ?$x($this, TgtA), !$x($this, TgtB)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != 0);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_entity_t e1 = ecs_new_w(world, RelA);
    ecs_entity_t e2 = ecs_new_w(world, RelA);
    ecs_entity_t e3 = ecs_new_w(world, RelA);

    ecs_add_pair(world, e1, RelA, TgtA);
    ecs_add_pair(world, e1, RelA, TgtB);

    ecs_add_pair(world, e2, RelA, TgtA);
    ecs_add_pair(world, e2, RelA, TgtB);

    ecs_add_pair(world, e3, RelA, TgtA);
    ecs_add_pair(world, e3, RelA, TgtB);
    ecs_add_pair(world, e3, RelB, TgtA);
    ecs_add_pair(world, e3, RelB, TgtB);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }
    
    ecs_remove_pair(world, e3, RelB, TgtA);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_remove_pair(world, e3, RelB, TgtB);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_remove_pair(world, e1, RelA, TgtB);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelA, TgtB), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_field_is_set(&it, 2));
        test_uint(RelA, ecs_iter_get_var(&it, x_var));
        test_uint(e1, it.entities[0]);

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Operators_3_and_optional_dependent_not_pair_tgt(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_query_t *q = ecs_query(world, {
        .expr = "RelA($this), ?RelA($this, $x), !RelB($this, $x)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != 0);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_entity_t e1 = ecs_new_w(world, RelA);
    ecs_entity_t e2 = ecs_new_w(world, RelA);
    ecs_entity_t e3 = ecs_new_w(world, RelA);

    ecs_add_pair(world, e1, RelA, TgtA);
    ecs_add_pair(world, e1, RelB, TgtA);

    ecs_add_pair(world, e2, RelA, TgtA);
    ecs_add_pair(world, e2, RelB, TgtA);

    ecs_add_pair(world, e3, RelA, TgtA);
    ecs_add_pair(world, e3, RelB, TgtA);
    ecs_add_pair(world, e3, RelA, TgtB);
    ecs_add_pair(world, e3, RelB, TgtB);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }
    
    ecs_remove_pair(world, e3, RelA, TgtB);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_remove_pair(world, e3, RelB, TgtB);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_remove_pair(world, e1, RelB, TgtA);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelB, TgtA), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_field_is_set(&it, 2));
        test_uint(TgtA, ecs_iter_get_var(&it, x_var));
        test_uint(e1, it.entities[0]);

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Operators_3_and_optional_dependent_not_pair_src(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);
    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_query_t *q = ecs_query(world, {
        .expr = "RelA($this), ?RelA($this, $x), !Tag($x)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != 0);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_entity_t e1 = ecs_new_w(world, RelA);
    ecs_entity_t e2 = ecs_new_w(world, RelA);
    ecs_entity_t e3 = ecs_new_w(world, RelA);

    ecs_entity_t t1 = ecs_new_w(world, Tag);
    ecs_entity_t t2 = ecs_new_w(world, Tag);
    ecs_entity_t t3 = ecs_new_w(world, Tag);

    ecs_add_pair(world, e2, RelA, t1);
    ecs_add_pair(world, e3, RelA, t2);
    ecs_add_pair(world, e3, RelA, t3);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(e1, it.entities[0]);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelA, EcsWildcard), ecs_field_id(&it, 1));
        test_uint(Tag, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(EcsWildcard, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_field_is_set(&it, 2));
        test_uint(EcsWildcard, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_remove(world, t1, Tag);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(e1, it.entities[0]);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelA, EcsWildcard), ecs_field_id(&it, 1));
        test_uint(Tag, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(EcsWildcard, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_field_is_set(&it, 2));
        test_uint(EcsWildcard, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(e2, it.entities[0]);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelA, t1), ecs_field_id(&it, 1));
        test_uint(Tag, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(t1, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_field_is_set(&it, 2));
        test_uint(t1, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_remove_pair(world, e3, RelA, t2);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(e1, it.entities[0]);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelA, EcsWildcard), ecs_field_id(&it, 1));
        test_uint(Tag, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(EcsWildcard, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_field_is_set(&it, 2));
        test_uint(EcsWildcard, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(e2, it.entities[0]);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelA, t1), ecs_field_id(&it, 1));
        test_uint(Tag, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(t1, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_field_is_set(&it, 2));
        test_uint(t1, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_remove_pair(world, e3, RelA, t3);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(2, it.count);
        test_uint(e1, it.entities[0]);
        test_uint(e3, it.entities[1]);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelA, EcsWildcard), ecs_field_id(&it, 1));
        test_uint(Tag, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(EcsWildcard, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_field_is_set(&it, 2));
        test_uint(EcsWildcard, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(e2, it.entities[0]);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelA, t1), ecs_field_id(&it, 1));
        test_uint(Tag, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(t1, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_field_is_set(&it, 2));
        test_uint(t1, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Operators_1_optional_any_src_fixed_first(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_query_t *q = ecs_query(world, {
        .expr = "?RelA(_)"
    });

    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(false, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_new_w(world, RelA);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(EcsWildcard, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_new_w(world, RelA);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(EcsWildcard, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Operators_1_optional_any_src_any_tgt_fixed_first(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_query_t *q = ecs_query(world, {
        .expr = "?RelA(_, _)"
    });

    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(RelA, EcsWildcard), ecs_field_id(&it, 0));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(false, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_new_w_pair(world, TgtA, RelA);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(RelA, EcsWildcard), ecs_field_id(&it, 0));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(false, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_new_w_pair(world, RelA, TgtA);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(RelA, EcsWildcard), ecs_field_id(&it, 0));
        test_uint(EcsWildcard, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_new_w_pair(world, RelA, TgtB);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(RelA, EcsWildcard), ecs_field_id(&it, 0));
        test_uint(EcsWildcard, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Operators_1_optional_any_src_any_first_fixed_tgt(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, TgtA);

    ecs_query_t *q = ecs_query(world, {
        .expr = "?_(_, TgtA)"
    });

    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(EcsWildcard, TgtA), ecs_field_id(&it, 0));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(false, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_new_w_pair(world, TgtA, RelA);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(EcsWildcard, TgtA), ecs_field_id(&it, 0));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(false, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_new_w_pair(world, RelA, TgtA);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(EcsWildcard, TgtA), ecs_field_id(&it, 0));
        test_uint(EcsWildcard, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_new_w_pair(world, RelB, TgtA);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(EcsWildcard, TgtA), ecs_field_id(&it, 0));
        test_uint(EcsWildcard, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Operators_1_optional_any_src_any_childof_pair_any_tgt(void) {
    ecs_world_t *world = ecs_mini();

    ecs_query_t *q = ecs_query(world, {
        .expr = "?ChildOf(_, _)"
    });

    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(EcsChildOf, EcsWildcard), ecs_field_id(&it, 0));
        test_uint(EcsWildcard, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Operators_1_optional_any_src_any_isa_pair_any_tgt(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TgtA);

    ecs_query_t *q = ecs_query(world, {
        .expr = "?IsA(_, _)"
    });

    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(EcsIsA, EcsWildcard), ecs_field_id(&it, 0));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(false, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_new_w_pair(world, EcsIsA, TgtA);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(EcsIsA, EcsWildcard), ecs_field_id(&it, 0));
        test_uint(EcsWildcard, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Operators_2_or(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, RelC);

    ECS_TAG(world, Tag);

    ecs_query_t *q = ecs_query(world, {
        .expr = "RelA($this) || RelB($this)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_entity_t e1 = ecs_new_w(world, RelA);
    ecs_entity_t e2 = ecs_new_w(world, RelA);
    ecs_add(world, e2, RelB);
    ecs_entity_t e3 = ecs_new_w(world, RelB);
    ecs_entity_t e4 = ecs_new_w(world, RelB);
    ecs_add(world, e4, Tag);
    ecs_new_w(world, RelC);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(e1, it.entities[0]);
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_uint(e2, it.entities[0]);
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_uint(e3, it.entities[0]);
        test_uint(1, it.count);
        test_uint(RelB, ecs_field_id(&it, 0));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_uint(e4, it.entities[0]);
        test_uint(1, it.count);
        test_uint(RelB, ecs_field_id(&it, 0));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Operators_3_or(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, RelC);
    ECS_TAG(world, RelD);

    ecs_query_t *q = ecs_query(world, {
        .expr = "RelA($this) || RelB($this) || RelC($this)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_entity_t e1 = ecs_new_w(world, RelA);
    ecs_entity_t e2 = ecs_new_w(world, RelB);
    ecs_entity_t e3 = ecs_new_w(world, RelC);
    ecs_new_w(world, RelD);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(RelB, ecs_field_id(&it, 0));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(e2, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(RelC, ecs_field_id(&it, 0));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(e3, it.entities[0]);

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Operators_2_or_w_and(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, TagC);
    ECS_TAG(world, Foo);

    ecs_query_t *q = ecs_query(world, {
        .expr = "TagA || TagB, Foo",
        .cache_kind = cache_kind
    });
    test_assert(q != NULL);

    ecs_entity_t e1 = ecs_new_w(world, TagA);
    ecs_entity_t e2 = ecs_new_w(world, TagA);
    ecs_add(world, e2, TagB);
    ecs_entity_t e3 = ecs_new_w(world, TagB);

    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);
    ecs_add(world, e3, Foo);

    ecs_new_w(world, TagB);
    ecs_new_w(world, TagC);
    ecs_new_w(world, Foo);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(TagA, ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));
    test_int(0, it.trs[0]->index);
    test_int(1, it.trs[1]->index);

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e2, it.entities[0]);
    test_uint(TagA, ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));
    test_int(0, it.trs[0]->index);
    test_int(2, it.trs[1]->index);

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e3, it.entities[0]);
    test_uint(TagB, ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));
    test_int(0, it.trs[0]->index);
    test_int(1, it.trs[1]->index);

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Operators_and_w_2_or_w_and(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, TagC);
    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Foo, TagA || TagB, Bar",
        .cache_kind = cache_kind
    });
    test_assert(q != NULL);
    test_int(q->term_count, 4);
    test_int(q->field_count, 3);

    ecs_entity_t e1 = ecs_new_w(world, TagA);
    ecs_entity_t e2 = ecs_new_w(world, TagA);
    ecs_add(world, e2, TagB);
    ecs_entity_t e3 = ecs_new_w(world, TagB);

    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);
    ecs_add(world, e3, Foo);

    ecs_add(world, e1, Bar);
    ecs_add(world, e2, Bar);
    ecs_add(world, e3, Bar);

    ecs_new_w(world, TagB);
    ecs_new_w(world, TagC);
    ecs_new_w(world, Foo);
    ecs_new_w(world, Bar);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(TagA, ecs_field_id(&it, 1));
    test_uint(Bar, ecs_field_id(&it, 2));
    test_int(1, it.trs[0]->index);
    test_int(0, it.trs[1]->index);
    test_int(2, it.trs[2]->index);

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e2, it.entities[0]);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(TagA, ecs_field_id(&it, 1));
    test_uint(Bar, ecs_field_id(&it, 2));
    test_int(2, it.trs[0]->index);
    test_int(0, it.trs[1]->index);
    test_int(3, it.trs[2]->index);

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e3, it.entities[0]);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(TagB, ecs_field_id(&it, 1));
    test_uint(Bar, ecs_field_id(&it, 2));
    test_int(1, it.trs[0]->index);
    test_int(0, it.trs[1]->index);
    test_int(2, it.trs[2]->index);

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Operators_and_w_2_or_w_and_components(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, TagC);
    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Mass);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Position, TagA || TagB, Mass",
        .cache_kind = cache_kind
    });
    test_assert(q != NULL);
    test_int(q->term_count, 4);
    test_int(q->field_count, 3);

    ecs_entity_t e1 = ecs_new_w(world, TagA);
    ecs_entity_t e2 = ecs_new_w(world, TagA);
    ecs_add(world, e2, TagB);
    ecs_entity_t e3 = ecs_new_w(world, TagB);

    ecs_set(world, e1, Position, {11, 21});
    ecs_set(world, e2, Position, {12, 22});
    ecs_set(world, e3, Position, {13, 23});

    ecs_set(world, e1, Mass, {1});
    ecs_set(world, e2, Mass, {2});
    ecs_set(world, e3, Mass, {3});

    ecs_new_w(world, TagB);
    ecs_new_w(world, TagC);
    ecs_new_w(world, Position);
    ecs_new_w(world, Mass);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(TagA, ecs_field_id(&it, 1));
    test_uint(ecs_id(Mass), ecs_field_id(&it, 2));
    test_int(0, it.trs[0]->index);
    test_int(2, it.trs[1]->index);
    test_int(1, it.trs[2]->index);
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 11); test_int(p->y, 21);
    }
    {
        Mass *v = ecs_field(&it, Mass, 2);
        test_assert(v != NULL);
        test_int(v[0], 1);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e2, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(TagA, ecs_field_id(&it, 1));
    test_uint(ecs_id(Mass), ecs_field_id(&it, 2));
    test_int(0, it.trs[0]->index);
    test_int(2, it.trs[1]->index);
    test_int(1, it.trs[2]->index);
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 12); test_int(p->y, 22);
    }
    {
        Mass *v = ecs_field(&it, Mass, 2);
        test_assert(v != NULL);
        test_int(v[0], 2);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e3, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(TagB, ecs_field_id(&it, 1));
    test_uint(ecs_id(Mass), ecs_field_id(&it, 2));
    test_int(0, it.trs[0]->index);
    test_int(2, it.trs[1]->index);
    test_int(1, it.trs[2]->index);
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 13); test_int(p->y, 23);
    }
    {
        Mass *v = ecs_field(&it, Mass, 2);
        test_assert(v != NULL);
        test_int(v[0], 3);
    }

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Operators_and_w_2_or_w_and_set_this(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, TagC);
    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Foo, TagA || TagB, Bar",
        .cache_kind = cache_kind
    });
    test_assert(q != NULL);
    test_int(q->term_count, 4);
    test_int(q->field_count, 3);

    ecs_entity_t e1 = ecs_new_w(world, TagA);
    ecs_entity_t e2 = ecs_new_w(world, TagA);
    ecs_add(world, e2, TagB);
    ecs_entity_t e3 = ecs_new_w(world, TagB);

    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);
    ecs_add(world, e3, Foo);

    ecs_add(world, e1, Bar);
    ecs_add(world, e2, Bar);
    ecs_add(world, e3, Bar);

    ecs_new_w(world, TagB);
    ecs_new_w(world, TagC);
    ecs_new_w(world, Foo);
    ecs_new_w(world, Bar);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e1);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e1, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(TagA, ecs_field_id(&it, 1));
        test_uint(Bar, ecs_field_id(&it, 2));
        test_int(1, it.trs[0]->index);
        test_int(0, it.trs[1]->index);
        test_int(2, it.trs[2]->index);
        test_bool(false, ecs_query_next(&it));
    }

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e2);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e2, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(TagA, ecs_field_id(&it, 1));
        test_uint(Bar, ecs_field_id(&it, 2));
        test_int(2, it.trs[0]->index);
        test_int(0, it.trs[1]->index);
        test_int(3, it.trs[2]->index);
        test_bool(false, ecs_query_next(&it));
    }

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e3);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e3, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(TagB, ecs_field_id(&it, 1));
        test_uint(Bar, ecs_field_id(&it, 2));
        test_int(1, it.trs[0]->index);
        test_int(0, it.trs[1]->index);
        test_int(2, it.trs[2]->index);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Operators_and_w_2_or_w_and_components_set_this(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, TagC);
    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Mass);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Position, TagA || TagB, Mass",
        .cache_kind = cache_kind
    });
    test_assert(q != NULL);
    test_int(q->term_count, 4);
    test_int(q->field_count, 3);

    ecs_entity_t e1 = ecs_new_w(world, TagA);
    ecs_entity_t e2 = ecs_new_w(world, TagA);
    ecs_add(world, e2, TagB);
    ecs_entity_t e3 = ecs_new_w(world, TagB);

    ecs_set(world, e1, Position, {11, 21});
    ecs_set(world, e2, Position, {12, 22});
    ecs_set(world, e3, Position, {13, 23});

    ecs_set(world, e1, Mass, {1});
    ecs_set(world, e2, Mass, {2});
    ecs_set(world, e3, Mass, {3});

    ecs_new_w(world, TagB);
    ecs_new_w(world, TagC);
    ecs_new_w(world, Position);
    ecs_new_w(world, Mass);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e1);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e1, it.entities[0]);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(TagA, ecs_field_id(&it, 1));
        test_uint(ecs_id(Mass), ecs_field_id(&it, 2));
        test_int(0, it.trs[0]->index);
        test_int(2, it.trs[1]->index);
        test_int(1, it.trs[2]->index);
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 11); test_int(p->y, 21);
        }
        {
            Mass *v = ecs_field(&it, Mass, 2);
            test_assert(v != NULL);
            test_int(v[0], 1);
        }
        test_bool(false, ecs_query_next(&it));
    }

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e2);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e2, it.entities[0]);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(TagA, ecs_field_id(&it, 1));
        test_uint(ecs_id(Mass), ecs_field_id(&it, 2));
        test_int(0, it.trs[0]->index);
        test_int(2, it.trs[1]->index);
        test_int(1, it.trs[2]->index);
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 12); test_int(p->y, 22);
        }
        {
            Mass *v = ecs_field(&it, Mass, 2);
            test_assert(v != NULL);
            test_int(v[0], 2);
        }
        test_bool(false, ecs_query_next(&it));
    }

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e3);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e3, it.entities[0]);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(TagB, ecs_field_id(&it, 1));
        test_uint(ecs_id(Mass), ecs_field_id(&it, 2));
        test_int(0, it.trs[0]->index);
        test_int(2, it.trs[1]->index);
        test_int(1, it.trs[2]->index);
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 13); test_int(p->y, 23);
        }
        {
            Mass *v = ecs_field(&it, Mass, 2);
            test_assert(v != NULL);
            test_int(v[0], 3);
        }
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Operators_3_or_w_and(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, TagC);
    ECS_TAG(world, Foo);

    ecs_query_t *q = ecs_query(world, {
        .expr =  "TagA || TagB || TagC, Foo",
        .cache_kind = cache_kind
    });
    test_assert(q != NULL);

    ecs_entity_t e1 = ecs_new_w(world, TagA);
    ecs_entity_t e2 = ecs_new_w(world, TagA);
    ecs_add(world, e2, TagB);
    ecs_entity_t e3 = ecs_new_w(world, TagB);
    ecs_add(world, e3, TagC);
    ecs_entity_t e4 = ecs_new_w(world, TagC);

    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);
    ecs_add(world, e3, Foo);
    ecs_add(world, e4, Foo);

    ecs_new_w(world, TagB);
    ecs_new_w(world, TagC);
    ecs_new_w(world, Foo);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(TagA, ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e2, it.entities[0]);
    test_uint(TagA, ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e3, it.entities[0]);
    test_uint(TagB, ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e4, it.entities[0]);
    test_uint(TagC, ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Operators_2_or_written(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);
    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, RelC);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Tag($this), RelA($this) || RelB($this)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_entity_t e1 = ecs_new_w(world, Tag);
    ecs_entity_t e2 = ecs_new_w(world, Tag);
    ecs_entity_t e3 = ecs_new_w(world, Tag);
    ecs_entity_t e4 = ecs_new_w(world, Tag);
    ecs_add(world, e1, RelA);
    ecs_add(world, e2, RelA);
    ecs_add(world, e2, RelB);
    ecs_add(world, e3, RelB);
    ecs_add(world, e4, RelC);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(Tag, ecs_field_id(&it, 0));
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(Tag, ecs_field_id(&it, 0));
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(e2, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(Tag, ecs_field_id(&it, 0));
        test_uint(RelB, ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(e3, it.entities[0]);

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Operators_3_or_written(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);
    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, RelC);
    ECS_TAG(world, RelD);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Tag($this), RelA($this) || RelB($this) || RelC($this)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_entity_t e1 = ecs_new_w(world, Tag);
    ecs_entity_t e2 = ecs_new_w(world, Tag);
    ecs_entity_t e3 = ecs_new_w(world, Tag);
    ecs_entity_t e4 = ecs_new_w(world, Tag);
    ecs_entity_t e5 = ecs_new_w(world, Tag);
    ecs_add(world, e1, RelA);
    ecs_add(world, e2, RelB);
    ecs_add(world, e3, RelB);
    ecs_add(world, e3, RelC);
    ecs_add(world, e4, RelC);
    ecs_add(world, e5, RelD);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(Tag, ecs_field_id(&it, 0));
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(Tag, ecs_field_id(&it, 0));
        test_uint(RelB, ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(e2, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(Tag, ecs_field_id(&it, 0));
        test_uint(RelB, ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(e3, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(Tag, ecs_field_id(&it, 0));
        test_uint(RelC, ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(e4, it.entities[0]);

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Operators_2_or_written_w_rel_var(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);
    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, RelC);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);
    ECS_TAG(world, TgtC);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Tag($this, $x), $x($this, TgtA) || $x($this, TgtB)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_entity_t e1 = ecs_new_w_pair(world, Tag, RelA);
    ecs_entity_t e2 = ecs_new_w_pair(world, Tag, RelB);
    ecs_entity_t e3 = ecs_new_w_pair(world, Tag, RelC);
    ecs_entity_t e4 = ecs_new_w_pair(world, Tag, RelA);
    ecs_add_pair(world, e1, RelA, TgtA);
    ecs_add_pair(world, e2, RelA, TgtA);
    ecs_add_pair(world, e2, RelB, TgtB);
    ecs_add_pair(world, e3, RelC, TgtA);
    ecs_add_pair(world, e3, RelC, TgtB);
    ecs_add_pair(world, e4, RelB, TgtA);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(Tag, RelA), ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(RelA, ecs_iter_get_var(&it, x_var));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(Tag, RelB), ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelB, TgtB), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(RelB, ecs_iter_get_var(&it, x_var));
        test_uint(e2, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(Tag, RelC), ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelC, TgtA), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(RelC, ecs_iter_get_var(&it, x_var));
        test_uint(e3, it.entities[0]);

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Operators_3_or_written_w_rel_var(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);
    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, RelC);
    ECS_TAG(world, RelD);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);
    ECS_TAG(world, TgtC);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Tag($this, $x), $x($this, TgtA) || $x($this, TgtB) || $x($this, TgtC)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_entity_t e1 = ecs_new_w_pair(world, Tag, RelA);
    ecs_entity_t e2 = ecs_new_w_pair(world, Tag, RelB);
    ecs_entity_t e3 = ecs_new_w_pair(world, Tag, RelC);
    ecs_entity_t e4 = ecs_new_w_pair(world, Tag, RelD);
    ecs_entity_t e5 = ecs_new_w_pair(world, Tag, RelA);
    ecs_add_pair(world, e1, RelA, TgtA);
    ecs_add_pair(world, e2, RelA, TgtA);
    ecs_add_pair(world, e2, RelB, TgtB);
    ecs_add_pair(world, e3, RelC, TgtA);
    ecs_add_pair(world, e3, RelC, TgtB);
    ecs_add_pair(world, e4, RelD, TgtC);
    ecs_add_pair(world, e5, RelD, TgtA);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(Tag, RelA), ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(RelA, ecs_iter_get_var(&it, x_var));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(Tag, RelB), ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelB, TgtB), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(RelB, ecs_iter_get_var(&it, x_var));
        test_uint(e2, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(Tag, RelC), ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelC, TgtA), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(RelC, ecs_iter_get_var(&it, x_var));
        test_uint(e3, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(Tag, RelD), ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelD, TgtC), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(RelD, ecs_iter_get_var(&it, x_var));
        test_uint(e4, it.entities[0]);

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Operators_2_or_written_w_tgt_var(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);
    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, RelC);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);
    ECS_TAG(world, TgtC);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Tag($this, $x), RelA($this, $x) || RelB($this, $x)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_entity_t e1 = ecs_new_w_pair(world, Tag, TgtA);
    ecs_entity_t e2 = ecs_new_w_pair(world, Tag, TgtB);
    ecs_entity_t e3 = ecs_new_w_pair(world, Tag, TgtC);
    ecs_entity_t e4 = ecs_new_w_pair(world, Tag, TgtA);
    ecs_add_pair(world, e1, RelA, TgtA);
    ecs_add_pair(world, e2, RelA, TgtA);
    ecs_add_pair(world, e2, RelB, TgtB);
    ecs_add_pair(world, e3, RelA, TgtC);
    ecs_add_pair(world, e3, RelB, TgtC);
    ecs_add_pair(world, e4, RelA, TgtB);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(Tag, TgtA), ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(TgtA, ecs_iter_get_var(&it, x_var));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(Tag, TgtB), ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelB, TgtB), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(TgtB, ecs_iter_get_var(&it, x_var));
        test_uint(e2, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(Tag, TgtC), ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelA, TgtC), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(TgtC, ecs_iter_get_var(&it, x_var));
        test_uint(e3, it.entities[0]);

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Operators_2_or_written_w_rel_tgt_var(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);
    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, RelC);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);
    ECS_TAG(world, TgtC);
    ECS_TAG(world, TgtD);

    ecs_query_t *q = ecs_query(world, {
        .expr = "RelA($this, $x), RelB($this, $y), $x($this, $y) || $y($this, $x)",
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

    ecs_entity_t e1 = ecs_new_w_pair(world, RelA, TgtA);
    ecs_entity_t e2 = ecs_new_w_pair(world, RelA, TgtB);
    ecs_entity_t e3 = ecs_new_w_pair(world, RelA, TgtC);
    ecs_entity_t e4 = ecs_new_w_pair(world, RelA, TgtD);

    ecs_add_pair(world, e1, RelB, TgtD);
    ecs_add_pair(world, e2, RelB, TgtC);
    ecs_add_pair(world, e3, RelB, TgtB);
    ecs_add_pair(world, e4, RelB, TgtA);

    ecs_add_pair(world, e2, TgtB, TgtC);
    ecs_add_pair(world, e3, TgtB, TgtC);
    ecs_add_pair(world, e4, TgtD, TgtA);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(RelA, TgtB), ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelB, TgtC), ecs_field_id(&it, 1));
        test_uint(ecs_pair(TgtB, TgtC), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(TgtB, ecs_iter_get_var(&it, x_var));
        test_uint(TgtC, ecs_iter_get_var(&it, y_var));
        test_uint(e2, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(RelA, TgtC), ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelB, TgtB), ecs_field_id(&it, 1));
        test_uint(ecs_pair(TgtB, TgtC), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(TgtC, ecs_iter_get_var(&it, x_var));
        test_uint(TgtB, ecs_iter_get_var(&it, y_var));
        test_uint(e3, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(RelA, TgtD), ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelB, TgtA), ecs_field_id(&it, 1));
        test_uint(ecs_pair(TgtD, TgtA), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(TgtD, ecs_iter_get_var(&it, x_var));
        test_uint(TgtA, ecs_iter_get_var(&it, y_var));
        test_uint(e4, it.entities[0]);

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Operators_2_or_written_w_rel_tgt_same_var(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);
    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, RelC);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);
    ECS_TAG(world, TgtC);
    ECS_TAG(world, TgtD);

    ecs_query_t *q = ecs_query(world, {
        .expr = "RelA($this, $x), $x($this, $x) || RelB($this, $x)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_entity_t e1 = ecs_new_w_pair(world, RelA, TgtA);
    ecs_entity_t e2 = ecs_new_w_pair(world, RelA, TgtB);
    ecs_entity_t e3 = ecs_new_w_pair(world, RelA, TgtC);
    ecs_new_w_pair(world, RelA, TgtC);

    ecs_add_pair(world, e1, TgtA, TgtA);
    ecs_add_pair(world, e2, RelB, TgtB);
    ecs_add_pair(world, e3, TgtC, TgtC);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 0));
        test_uint(ecs_pair(TgtA, TgtA), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(TgtA, ecs_iter_get_var(&it, x_var));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(RelA, TgtB), ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelB, TgtB), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(TgtB, ecs_iter_get_var(&it, x_var));
        test_uint(e2, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(RelA, TgtC), ecs_field_id(&it, 0));
        test_uint(ecs_pair(TgtC, TgtC), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(TgtC, ecs_iter_get_var(&it, x_var));
        test_uint(e3, it.entities[0]);

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Operators_3_or_written_w_tgt_var(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);
    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, RelC);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);
    ECS_TAG(world, TgtC);
    ECS_TAG(world, TgtD);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Tag($this, $x), RelA($this, $x) || RelB($this, $x) || RelC($this, $x)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_entity_t e1 = ecs_new_w_pair(world, Tag, TgtA);
    ecs_entity_t e2 = ecs_new_w_pair(world, Tag, TgtB);
    ecs_entity_t e3 = ecs_new_w_pair(world, Tag, TgtC);
    ecs_entity_t e4 = ecs_new_w_pair(world, Tag, TgtD);
    ecs_entity_t e5 = ecs_new_w_pair(world, Tag, TgtA);
    ecs_add_pair(world, e1, RelA, TgtA);
    ecs_add_pair(world, e2, RelA, TgtA);
    ecs_add_pair(world, e2, RelB, TgtB);
    ecs_add_pair(world, e3, RelA, TgtC);
    ecs_add_pair(world, e3, RelB, TgtC);
    ecs_add_pair(world, e4, RelC, TgtD);
    ecs_add_pair(world, e5, RelA, TgtD);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(Tag, TgtA), ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(TgtA, ecs_iter_get_var(&it, x_var));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(Tag, TgtB), ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelB, TgtB), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(TgtB, ecs_iter_get_var(&it, x_var));
        test_uint(e2, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(Tag, TgtC), ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelA, TgtC), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(TgtC, ecs_iter_get_var(&it, x_var));
        test_uint(e3, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(Tag, TgtD), ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelC, TgtD), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(TgtD, ecs_iter_get_var(&it, x_var));
        test_uint(e4, it.entities[0]);

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Operators_2_or_chains_written(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);
    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, RelC);
    ECS_TAG(world, RelD);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Tag($this), RelA($this) || RelB($this), RelC($this) || RelD($this)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_entity_t e1 = ecs_new_w(world, Tag);
    ecs_entity_t e2 = ecs_new_w(world, Tag);
    ecs_entity_t e3 = ecs_new_w(world, Tag);
    ecs_entity_t e4 = ecs_new_w(world, Tag);
    ecs_entity_t e5 = ecs_new_w(world, Tag);
    ecs_entity_t e6 = ecs_new_w(world, Tag);

    ecs_add(world, e1, RelA);
    ecs_add(world, e2, RelC);

    ecs_add(world, e3, RelA);
    ecs_add(world, e3, RelC);

    ecs_add(world, e4, RelA);
    ecs_add(world, e4, RelD);

    ecs_add(world, e5, RelB);
    ecs_add(world, e5, RelC);

    ecs_add(world, e6, RelB);
    ecs_add(world, e6, RelD);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(Tag, ecs_field_id(&it, 0));
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(RelC, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(e3, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(Tag, ecs_field_id(&it, 0));
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(RelD, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(e4, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(Tag, ecs_field_id(&it, 0));
        test_uint(RelB, ecs_field_id(&it, 1));
        test_uint(RelC, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(e5, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(Tag, ecs_field_id(&it, 0));
        test_uint(RelB, ecs_field_id(&it, 1));
        test_uint(RelD, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(e6, it.entities[0]);

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Operators_2_or_chains(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);
    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, RelC);
    ECS_TAG(world, RelD);

    ecs_entity_t e1 = ecs_new(world);
    ecs_entity_t e2 = ecs_new(world);
    ecs_entity_t e3 = ecs_new(world);
    ecs_entity_t e4 = ecs_new(world);
    ecs_entity_t e5 = ecs_new(world);
    ecs_entity_t e6 = ecs_new(world);

    ecs_add(world, e1, RelA);
    ecs_add(world, e2, RelC);

    ecs_add(world, e3, RelA);
    ecs_add(world, e3, RelC);

    ecs_add(world, e4, RelA);
    ecs_add(world, e4, RelD);

    ecs_add(world, e5, RelB);
    ecs_add(world, e5, RelC);

    ecs_add(world, e6, RelB);
    ecs_add(world, e6, RelD);

    ecs_query_t *q = ecs_query(world, {
        .expr = "RelA($this) || RelB($this), RelC($this) || RelD($this)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(e3, it.entities[0]);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(RelC, ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(e4, it.entities[0]);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(RelD, ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(e5, it.entities[0]);
        test_uint(RelB, ecs_field_id(&it, 0));
        test_uint(RelC, ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(e6, it.entities[0]);
        test_uint(RelB, ecs_field_id(&it, 0));
        test_uint(RelD, ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Operators_2_or_dependent(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, RelC);

    ecs_query_t *q = ecs_query(world, {
        .expr = "RelB($this) || RelA($this, $tgt), RelC($tgt)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    int tgt_var = ecs_query_find_var(q, "tgt");
    test_assert(tgt_var != -1);

    ecs_entity_t tgt = ecs_new_w(world, RelC);

    ecs_entity_t e1 = ecs_new(world);
    ecs_entity_t e2 = ecs_new(world);
    ecs_entity_t e3 = ecs_new(world);

    ecs_add(world, e1, RelB);
    ecs_add_pair(world, e2, RelA, tgt);
    ecs_add(world, e3, RelB);
    ecs_add(world, e3, Foo);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(RelB, ecs_field_id(&it, 0));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_field_is_set(&it, 1));
        test_uint(e1, it.entities[0]);
        test_uint(EcsWildcard, ecs_iter_get_var(&it, tgt_var));

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(RelB, ecs_field_id(&it, 0));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_field_is_set(&it, 1));
        test_uint(e3, it.entities[0]);
        test_uint(EcsWildcard, ecs_iter_get_var(&it, tgt_var));

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(RelA, tgt), ecs_field_id(&it, 0));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(e2, it.entities[0]);
        test_uint(tgt, ecs_iter_get_var(&it, tgt_var));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Operators_2_or_dependent_reverse(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, RelC);

    ecs_query_t *q = ecs_query(world, {
        .expr = "RelA($this, $tgt) || RelB($this), RelC($tgt)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    int tgt_var = ecs_query_find_var(q, "tgt");
    test_assert(tgt_var != -1);

    ecs_entity_t tgt = ecs_new_w(world, RelC);

    ecs_entity_t e1 = ecs_new(world);
    ecs_entity_t e2 = ecs_new(world);
    ecs_entity_t e3 = ecs_new(world);

    ecs_add(world, e1, RelB);
    ecs_add_pair(world, e2, RelA, tgt);
    ecs_add(world, e3, RelB);
    ecs_add(world, e3, Foo);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(RelA, tgt), ecs_field_id(&it, 0));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(e2, it.entities[0]);
        test_uint(tgt, ecs_iter_get_var(&it, tgt_var));

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(RelB, ecs_field_id(&it, 0));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_field_is_set(&it, 1));
        test_uint(e1, it.entities[0]);
        test_uint(EcsWildcard, ecs_iter_get_var(&it, tgt_var));

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(RelB, ecs_field_id(&it, 0));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_field_is_set(&it, 1));
        test_uint(e3, it.entities[0]);
        test_uint(EcsWildcard, ecs_iter_get_var(&it, tgt_var));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Operators_2_or_dependent_2_vars(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, RelC);
    ECS_TAG(world, RelD);

    ecs_query_t *q = ecs_query(world, {
        .expr = "RelA($this, $x) || RelB($this, $y), RelC($x), RelD($y)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);
    int y_var = ecs_query_find_var(q, "y");
    test_assert(y_var != -1);

    ecs_entity_t tgt_a = ecs_new_w(world, RelC);
    ecs_entity_t tgt_b = ecs_new_w(world, RelD);

    ecs_entity_t e1 = ecs_new(world);
    ecs_entity_t e2 = ecs_new(world);
    ecs_entity_t e3 = ecs_new(world);
    ecs_entity_t e4 = ecs_new(world);

    ecs_add_pair(world, e1, RelA, tgt_a);
    ecs_add_pair(world, e2, RelB, tgt_b);

    ecs_add_pair(world, e3, RelA, tgt_b);
    ecs_add_pair(world, e4, RelB, tgt_a);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(RelA, tgt_a), ecs_field_id(&it, 0));
        test_uint(RelC, ecs_field_id(&it, 1));
        test_uint(RelD, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_field_is_set(&it, 2));
        test_uint(e1, it.entities[0]);
        test_uint(tgt_a, ecs_iter_get_var(&it, x_var));
        test_uint(EcsWildcard, ecs_iter_get_var(&it, y_var));

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(RelB, tgt_b), ecs_field_id(&it, 0));
        test_uint(RelC, ecs_field_id(&it, 1));
        test_uint(RelD, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(e2, it.entities[0]);
        test_uint(EcsWildcard, ecs_iter_get_var(&it, x_var));
        test_uint(tgt_b, ecs_iter_get_var(&it, y_var));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Operators_2_or_written_dependent(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);
    ECS_TAG(world, Foo);
    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, RelC);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Tag, RelA($this, $tgt) || RelB($this), RelC($tgt)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    int tgt_var = ecs_query_find_var(q, "tgt");
    test_assert(tgt_var != -1);

    ecs_entity_t tgt = ecs_new_w(world, RelC);

    ecs_entity_t e1 = ecs_new_w(world, Tag);
    ecs_entity_t e2 = ecs_new_w(world, Tag);
    ecs_entity_t e3 = ecs_new_w(world, Tag);

    ecs_add(world, e1, RelB);
    ecs_add_pair(world, e2, RelA, tgt);
    ecs_add(world, e3, RelB);
    ecs_add(world, e3, Foo);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(Tag, ecs_field_id(&it, 0));
        test_uint(RelB, ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_field_is_set(&it, 2));
        test_uint(e1, it.entities[0]);
        test_uint(EcsWildcard, ecs_iter_get_var(&it, tgt_var));

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(Tag, ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelA, tgt), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(e2, it.entities[0]);
        test_uint(tgt, ecs_iter_get_var(&it, tgt_var));

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(Tag, ecs_field_id(&it, 0));
        test_uint(RelB, ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_field_is_set(&it, 2));
        test_uint(e3, it.entities[0]);
        test_uint(EcsWildcard, ecs_iter_get_var(&it, tgt_var));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Operators_2_or_written_dependent_2_vars(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);
    ECS_TAG(world, Foo);
    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, RelC);
    ECS_TAG(world, RelD);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Tag, RelA($this, $x) || RelB($this, $y), RelC($x), RelD($y)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);
    int y_var = ecs_query_find_var(q, "y");
    test_assert(y_var != -1);

    ecs_entity_t tgt_a = ecs_new_w(world, RelC);
    ecs_entity_t tgt_b = ecs_new_w(world, RelD);

    ecs_entity_t e1 = ecs_new_w(world, Tag);
    ecs_entity_t e2 = ecs_new_w(world, Tag);
    ecs_entity_t e3 = ecs_new_w(world, Tag);
    ecs_entity_t e4 = ecs_new_w(world, Tag);

    ecs_add_pair(world, e1, RelA, tgt_a);
    ecs_add_pair(world, e2, RelB, tgt_b);

    ecs_add_pair(world, e3, RelA, tgt_b);
    ecs_add_pair(world, e4, RelB, tgt_a);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(Tag, ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelA, tgt_a), ecs_field_id(&it, 1));
        test_uint(RelC, ecs_field_id(&it, 2));
        test_uint(RelD, ecs_field_id(&it, 3));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_bool(false, ecs_field_is_set(&it, 3));
        test_uint(e1, it.entities[0]);
        test_uint(tgt_a, ecs_iter_get_var(&it, x_var));
        test_uint(EcsWildcard, ecs_iter_get_var(&it, y_var));

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(Tag, ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelB, tgt_b), ecs_field_id(&it, 1));
        test_uint(RelC, ecs_field_id(&it, 2));
        test_uint(RelD, ecs_field_id(&it, 3));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_field_is_set(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 3));
        test_uint(e2, it.entities[0]);
        test_uint(EcsWildcard, ecs_iter_get_var(&it, x_var));
        test_uint(tgt_b, ecs_iter_get_var(&it, y_var));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Operators_2_or_w_dependent(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);
    ECS_TAG(world, Foo);
    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, RelC);
    ECS_TAG(world, RelD);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Tag, ?RelA($this, $x), RelB($x) || RelC($x)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    ecs_entity_t tgt_a = ecs_new_w(world, RelB);
    ecs_entity_t tgt_b = ecs_new_w(world, RelC);
    ecs_entity_t tgt_c = ecs_new_w(world, RelD);

    ecs_entity_t e1 = ecs_new_w(world, Tag);
    ecs_entity_t e2 = ecs_new_w(world, Tag);
    ecs_entity_t e3 = ecs_new_w(world, Tag);
    ecs_entity_t e4 = ecs_new_w(world, Tag);

    ecs_add_pair(world, e2, RelA, tgt_a);
    ecs_add_pair(world, e3, RelA, tgt_b);
    ecs_add_pair(world, e4, RelA, tgt_c);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(Tag, ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelA, EcsWildcard), ecs_field_id(&it, 1));
        // test_uint(RelC, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_field_is_set(&it, 2));
        test_uint(e1, it.entities[0]);
        test_uint(EcsWildcard, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(Tag, ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelA, tgt_a), ecs_field_id(&it, 1));
        test_uint(RelB, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(e2, it.entities[0]);
        test_uint(tgt_a, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(Tag, ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelA, tgt_b), ecs_field_id(&it, 1));
        test_uint(RelC, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(e3, it.entities[0]);
        test_uint(tgt_b, ecs_iter_get_var(&it, x_var));


        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Operators_2_or_w_both(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, RelC);

    ecs_query_t *q = ecs_query(world, {
        .expr = "RelA || RelB",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_entity_t e1 = ecs_new_w(world, RelA);
    ecs_entity_t e2 = ecs_new_w(world, RelA);
    ecs_add(world, e2, RelB);
    ecs_entity_t e3 = ecs_new_w(world, RelB);
    ecs_new_w(world, RelC);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(RelA, ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e2, it.entities[0]);
    test_uint(RelA, ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e3, it.entities[0]);
    test_uint(RelB, ecs_field_id(&it, 0));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Operators_3_or_w_both(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, RelC);
    ECS_TAG(world, RelD);

    ecs_query_t *q = ecs_query(world, {
        .expr = "RelA || RelB || RelC",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_entity_t e1 = ecs_new_w(world, RelA);
    ecs_entity_t e2 = ecs_new_w(world, RelA);
    ecs_add(world, e2, RelB);
    ecs_entity_t e3 = ecs_new_w(world, RelA);
    ecs_add(world, e3, RelD);

    ecs_entity_t e4 = ecs_new_w(world, RelB);
    ecs_entity_t e5 = ecs_new_w(world, RelB);
    ecs_add(world, e5, RelC);
    ecs_entity_t e6 = ecs_new_w(world, RelB);
    ecs_add(world, e6, RelD);

    ecs_entity_t e7 = ecs_new_w(world, RelC);
    ecs_entity_t e8 = ecs_new_w(world, RelC);
    ecs_add(world, e8, RelD);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(RelA, ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e2, it.entities[0]);
    test_uint(RelA, ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e3, it.entities[0]);
    test_uint(RelA, ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e4, it.entities[0]);
    test_uint(RelB, ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e5, it.entities[0]);
    test_uint(RelB, ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e6, it.entities[0]);
    test_uint(RelB, ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e7, it.entities[0]);
    test_uint(RelC, ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e8, it.entities[0]);
    test_uint(RelC, ecs_field_id(&it, 0));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Operators_2_or_w_not(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, RelC);

    ecs_query_t *q = ecs_query(world, {
        .expr = "RelA || RelB, !RelC",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_entity_t e1 = ecs_new_w(world, RelA);
    ecs_entity_t e2 = ecs_new_w(world, RelA);
    ecs_add(world, e2, RelB);
    ecs_add(world, e2, RelC);
    ecs_entity_t e3 = ecs_new_w(world, RelB);
    ecs_new_w(world, RelC);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(RelA, ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e3, it.entities[0]);
    test_uint(RelB, ecs_field_id(&it, 0));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Operators_2_or_w_not_component(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_COMPONENT(world, Position);

    ecs_query_t *q = ecs_query(world, {
        .expr = "RelA || RelB, !Position",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_entity_t e1 = ecs_new_w(world, RelA);
    ecs_entity_t e2 = ecs_new_w(world, RelA);
    ecs_add(world, e2, RelB);
    ecs_add(world, e2, Position);
    ecs_entity_t e3 = ecs_new_w(world, RelB);
    ecs_new_w(world, Position);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(RelA, ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e3, it.entities[0]);
    test_uint(RelB, ecs_field_id(&it, 0));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Operators_2_or_w_not_out_component(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_COMPONENT(world, Position);

    ecs_query_t *q = ecs_query(world, {
        .expr = "RelA || RelB, [out] !Position",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_entity_t e1 = ecs_new_w(world, RelA);
    ecs_entity_t e2 = ecs_new_w(world, RelA);
    ecs_add(world, e2, RelB);
    ecs_add(world, e2, Position);
    ecs_entity_t e3 = ecs_new_w(world, RelB);
    ecs_new_w(world, Position);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(RelA, ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e3, it.entities[0]);
    test_uint(RelB, ecs_field_id(&it, 0));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Operators_2_or_w_not_out_all_components(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_COMPONENT(world, Position);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Velocity || Mass, [out] !Position",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_entity_t e1 = ecs_new_w(world, Velocity);
    ecs_entity_t e2 = ecs_new_w(world, Velocity);
    ecs_add(world, e2, Mass);
    ecs_add(world, e2, Position);
    ecs_entity_t e3 = ecs_new_w(world, Mass);
    ecs_new_w(world, Position);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(ecs_id(Velocity), ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e3, it.entities[0]);
    test_uint(ecs_id(Mass), ecs_field_id(&it, 0));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Operators_2_not_first(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_entity_t e1 = ecs_new_w(world, TagA);
    ecs_entity_t e2 = ecs_new_w(world, TagA);
    ecs_entity_t e3 = ecs_new_w(world, TagA);

    ecs_add(world, e1, TagB);
    ecs_add(world, e2, TagB);
    ecs_add(world, e3, TagB);

    ecs_query_t *q = ecs_query(world, {
        .expr = "!TagB($this), TagA($this)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_remove(world, e1, TagB);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(TagB, ecs_field_id(&it, 0));
        test_uint(TagA, ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(false, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(e1, it.entities[0]);

        test_bool(false, ecs_query_next(&it));
    }

    ecs_remove(world, e2, TagB);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(2, it.count);
        test_uint(TagB, ecs_field_id(&it, 0));
        test_uint(TagA, ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(false, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(e1, it.entities[0]);
        test_uint(e2, it.entities[1]);

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Operators_2_optional_first(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_entity_t e1 = ecs_new_w(world, TagA);
    ecs_entity_t e2 = ecs_new_w(world, TagA);
    ecs_entity_t e3 = ecs_new_w(world, TagA);

    ecs_add(world, e1, TagB);
    ecs_add(world, e2, TagB);
    ecs_add(world, e3, TagB);

    ecs_query_t *q = ecs_query(world, {
        .expr = "?TagB($this), TagA($this)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(3, it.count);
        test_uint(TagB, ecs_field_id(&it, 0));
        test_uint(TagA, ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(e1, it.entities[0]);
        test_uint(e2, it.entities[1]);
        test_uint(e3, it.entities[2]);

        test_bool(false, ecs_query_next(&it));
    }

    ecs_remove(world, e3, TagB);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(2, it.count);
        test_uint(TagB, ecs_field_id(&it, 0));
        test_uint(TagA, ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(e1, it.entities[0]);
        test_uint(e2, it.entities[1]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(TagB, ecs_field_id(&it, 0));
        test_uint(TagA, ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(false, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(e3, it.entities[0]);

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Operators_only_not(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_entity_t e1 = ecs_new_w(world, TagA);
    ecs_entity_t e2 = ecs_new_w(world, TagA);
    ecs_add(world, e2, TagB);

    ecs_query_t *q = ecs_query(world, {
        .expr = "!TagA($this)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    int32_t count = 0;
    while (ecs_query_next(&it)) {
        for (int32_t i = 0; i < it.count; i ++) {
            test_assert(it.entities[i] != e1);
            test_assert(it.entities[i] != e2);
            count ++;
        }
    }

    test_assert(count != 0);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Operators_only_not_component(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, TagB);

    ecs_entity_t e1 = ecs_new_w(world, Position);
    ecs_entity_t e2 = ecs_new_w(world, Position);
    ecs_add(world, e2, TagB);

    ecs_query_t *q = ecs_query(world, {
        .expr = "!Position($this)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    int32_t count = 0;
    while (ecs_query_next(&it)) {
        for (int32_t i = 0; i < it.count; i ++) {
            test_assert(it.entities[i] != e1);
            test_assert(it.entities[i] != e2);
            count ++;
        }
    }

    test_assert(count != 0);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Operators_only_not_out_component(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, TagB);

    ecs_entity_t e1 = ecs_new_w(world, Position);
    ecs_entity_t e2 = ecs_new_w(world, Position);
    ecs_add(world, e2, TagB);

    ecs_query_t *q = ecs_query(world, {
        .expr = "[out] !Position($this)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    int32_t count = 0;
    while (ecs_query_next(&it)) {
        for (int32_t i = 0; i < it.count; i ++) {
            test_assert(it.entities[i] != e1);
            test_assert(it.entities[i] != e2);
            count ++;
        }
    }

    test_assert(count != 0);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Operators_only_optional(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_entity_t e1 = ecs_new_w(world, TagA);
    ecs_entity_t e2 = ecs_new_w(world, TagA);
    ecs_add(world, e2, TagB);

    ecs_query_t *q = ecs_query(world, {
        .expr = "?TagA($this)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    int32_t count = 0;
    bool e1_found = false;
    bool e2_found = false;
    while (ecs_query_next(&it)) {
        for (int32_t i = 0; i < it.count; i ++) {
            e1_found |= it.entities[i] == e1;
            e2_found |= it.entities[i] == e2;
            count ++;
        }
    }

    test_assert(count > 2);
    test_bool(e1_found, true);
    test_bool(e2_found, true);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Operators_only_optional_component(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, TagB);

    ecs_entity_t e1 = ecs_new_w(world, Position);
    ecs_entity_t e2 = ecs_new_w(world, Position);
    ecs_add(world, e2, TagB);

    ecs_query_t *q = ecs_query(world, {
        .expr = "?Position($this)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    int32_t count = 0;
    bool e1_found = false;
    bool e2_found = false;
    while (ecs_query_next(&it)) {
        for (int32_t i = 0; i < it.count; i ++) {
            e1_found |= it.entities[i] == e1;
            e2_found |= it.entities[i] == e2;
            count ++;
        }
    }

    test_assert(count > 2);
    test_bool(e1_found, true);
    test_bool(e2_found, true);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Operators_not_after_fixed_src(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, TagC);

    ecs_entity_t ent = ecs_entity(world, { .name = "ent" });
    ecs_entity_t e1 = ecs_new_w(world, TagA);
    ecs_entity_t e2 = ecs_new_w(world, TagA);
    ecs_add(world, e2, TagB);

    ecs_query_t *q = ecs_query(world, {
        .expr = "TagC(ent), !TagA($this)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add(world, ent, TagC);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        int32_t count = 0;
        while (ecs_query_next(&it)) {
            for (int32_t i = 0; i < it.count; i ++) {
                test_assert(it.entities[i] != e1);
                test_assert(it.entities[i] != e2);
                count ++;
            }
        }
        test_assert(count != 0);
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Operators_optional_after_fixed_src(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, TagC);

    ecs_entity_t ent = ecs_entity(world, { .name = "ent" });
    ecs_entity_t e1 = ecs_new_w(world, TagA);
    ecs_entity_t e2 = ecs_new_w(world, TagA);
    ecs_add(world, e2, TagB);

    ecs_query_t *q = ecs_query(world, {
        .expr = "TagC(ent), ?TagA($this)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add(world, ent, TagC);

    ecs_iter_t it = ecs_query_iter(world, q);
    int32_t count = 0;
    bool e1_found = false;
    bool e2_found = false;
    while (ecs_query_next(&it)) {
        for (int32_t i = 0; i < it.count; i ++) {
            e1_found |= it.entities[i] == e1;
            e2_found |= it.entities[i] == e2;
            count ++;
        }
    }

    test_assert(count > 2);
    test_bool(e1_found, true);
    test_bool(e2_found, true);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Operators_root_entities_empty(void) {
    ecs_world_t *world = ecs_mini();

    ecs_query_t *q = ecs_query(world, {
        .expr = "!ChildOf($this, _)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(EcsChildOf, 0), ecs_field_id(&it, 0));
        test_uint(EcsFlecs, it.entities[0]);

        if (ecs_query_next(&it)) {
            test_uint(1, it.count);
            test_uint(q->entity, it.entities[0]);
            test_bool(false, ecs_query_next(&it));
        }
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Operators_root_entities(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e1 = ecs_entity(world, { .name = "e1" });
    ecs_entity(world, { .name = "e1.e2" });

    ecs_query_t *q = ecs_query(world, {
        .expr = "!ChildOf($this, _)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(EcsChildOf, 0), ecs_field_id(&it, 0));
        test_uint(EcsFlecs, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(EcsChildOf, 0), ecs_field_id(&it, 0));
        test_uint(e1, it.entities[0]);

        if (ecs_query_next(&it)) {
            test_uint(1, it.count);
            test_uint(q->entity, it.entities[0]);
            test_bool(false, ecs_query_next(&it));
        }
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Operators_root_entities_w_children(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    ecs_query_t *q = ecs_query(world, {
        .expr = "!ChildOf($this, _), ChildOf(_, $this)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int this_var = ecs_query_find_var(q, "this");
    test_assert(this_var != -1);
    test_assert(this_var != 0);

    ecs_new_w(world, Tag);
    ecs_entity_t e2 = ecs_new_w(world, Tag);
    ecs_entity_t e3 = ecs_new_w(world, Tag);
    ecs_add_pair(world, e3, EcsChildOf, e2);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(EcsChildOf, 0), ecs_field_id(&it, 0));
        test_uint(ecs_pair(EcsChildOf, EcsFlecs), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(EcsFlecs, ecs_iter_get_var(&it, this_var));
        test_uint(EcsFlecs, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(EcsChildOf, 0), ecs_field_id(&it, 0));
        test_uint(ecs_pair(EcsChildOf, e2), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(e2, ecs_iter_get_var(&it, this_var));
        test_uint(e2, it.entities[0]);

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Operators_root_entities_w_optional_children(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    ecs_entity_t e1 = ecs_new_w(world, Tag);
    ecs_entity_t e2 = ecs_new_w(world, Tag);
    ecs_entity_t e3 = ecs_new_w(world, Tag);
    ecs_entity_t e4 = ecs_new_w(world, Tag);
    ecs_add_pair(world, e3, EcsChildOf, e2);

    ecs_query_t *q = ecs_query(world, {
        .expr = "!ChildOf($this, _), ?ChildOf(_, $this)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int this_var = ecs_query_find_var(q, "this");
    test_assert(this_var != -1);
    test_assert(this_var != 0);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(EcsChildOf, 0), ecs_field_id(&it, 0));
        test_uint(ecs_pair(EcsChildOf, EcsFlecs), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(EcsFlecs, ecs_iter_get_var(&it, this_var));
        test_uint(EcsFlecs, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(EcsChildOf, 0), ecs_field_id(&it, 0));
        test_uint(ecs_pair(EcsChildOf, Tag), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_field_is_set(&it, 1));
        test_uint(Tag, ecs_iter_get_var(&it, this_var));
        test_uint(Tag, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(EcsChildOf, 0), ecs_field_id(&it, 0));
        test_uint(ecs_pair(EcsChildOf, e1), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_field_is_set(&it, 1));
        test_uint(e1, ecs_iter_get_var(&it, this_var));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(EcsChildOf, 0), ecs_field_id(&it, 0));
        test_uint(ecs_pair(EcsChildOf, e2), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(e2, ecs_iter_get_var(&it, this_var));
        test_uint(e2, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(EcsChildOf, 0), ecs_field_id(&it, 0));
        test_uint(ecs_pair(EcsChildOf, e4), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_field_is_set(&it, 1));
        test_uint(e4, ecs_iter_get_var(&it, this_var));
        test_uint(e4, it.entities[0]);

        if (ecs_query_next(&it)) {
            test_uint(1, it.count);
            test_uint(q->entity, it.entities[0]);
            test_bool(false, ecs_query_next(&it));
        }
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Operators_core_entities_w_optional_children(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    ecs_query_t *q = ecs_query(world, {
        .expr = "ChildOf($this, flecs.core), ?ChildOf(_, $this)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int this_var = ecs_query_find_var(q, "this");
    test_assert(this_var != -1);
    test_assert(this_var != 0);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        while (ecs_query_next(&it)) {
            test_assert(ecs_field_is_set(&it, 0));
            for (int i = 0; i < it.count; i ++) {
                test_assert(ecs_has_pair(
                    world, it.entities[i], EcsChildOf, EcsFlecsCore));
                if (ecs_field_is_set(&it, 1)) {
                    test_assert(ecs_count_id(
                        world, ecs_pair(EcsChildOf, it.entities[i])) != 0);
                } else {
                    test_assert(ecs_count_id(
                        world, ecs_pair(EcsChildOf, it.entities[i])) == 0);
                }
            }
        }
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Operators_root_entities_w_not_children(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    ecs_entity_t e1 = ecs_new_w(world, Tag);
    ecs_entity_t e2 = ecs_new_w(world, Tag);
    ecs_entity_t e3 = ecs_new_w(world, Tag);
    ecs_entity_t e4 = ecs_new_w(world, Tag);
    ecs_add_pair(world, e3, EcsChildOf, e2);

    ecs_query_t *q = ecs_query(world, {
        .expr = "!ChildOf($this, _), !ChildOf(_, $this)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int this_var = ecs_query_find_var(q, "this");
    test_assert(this_var != -1);
    test_assert(this_var != 0);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(EcsChildOf, 0), ecs_field_id(&it, 0));
        test_uint(ecs_pair(EcsChildOf, Tag), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_field_is_set(&it, 1));
        test_uint(Tag, ecs_iter_get_var(&it, this_var));
        test_uint(Tag, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(EcsChildOf, 0), ecs_field_id(&it, 0));
        test_uint(ecs_pair(EcsChildOf, e1), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_field_is_set(&it, 1));
        test_uint(e1, ecs_iter_get_var(&it, this_var));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(EcsChildOf, 0), ecs_field_id(&it, 0));
        test_uint(ecs_pair(EcsChildOf, e4), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_field_is_set(&it, 1));
        test_uint(e4, ecs_iter_get_var(&it, this_var));
        test_uint(e4, it.entities[0]);

        if (ecs_query_next(&it)) {
            test_uint(1, it.count);
            test_uint(q->entity, it.entities[0]);
            test_bool(false, ecs_query_next(&it));
        }
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Operators_core_entities_w_not_children(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    ecs_query_t *q = ecs_query(world, {
        .expr = "ChildOf($this, flecs.core), !ChildOf(_, $this)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int this_var = ecs_query_find_var(q, "this");
    test_assert(this_var != -1);
    test_assert(this_var != 0);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        while (ecs_query_next(&it)) {
            test_assert(ecs_field_is_set(&it, 0));
            test_assert(!ecs_field_is_set(&it, 1));
            for (int i = 0; i < it.count; i ++) {
                test_assert(ecs_has_pair(
                    world, it.entities[i], EcsChildOf, EcsFlecsCore));
                test_assert(ecs_count_id(
                    world, ecs_pair(EcsChildOf, it.entities[i])) == 0);
            }
        }
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Operators_1_ent_src_not(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);

    ecs_entity_t e1 = ecs_entity(world, { .name = "e1" });
    ecs_add(world, e1, RelA);

    {
        ecs_query_t *q = ecs_query(world, {
            .expr = "!RelA(e1)",
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        {
            ecs_iter_t it = ecs_query_iter(world, q);
            test_bool(false, ecs_query_next(&it));
        }

        ecs_query_fini(q);
    }

    {
        ecs_query_t *q = ecs_query(world, {
            .expr = "!RelB(e1)",
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        {
            ecs_iter_t it = ecs_query_iter(world, q);
            test_bool(true, ecs_query_next(&it));
            test_uint(0, it.count);
            test_uint(RelB, ecs_field_id(&it, 0));
            test_uint(e1, ecs_field_src(&it, 0));
            test_bool(false, ecs_field_is_set(&it, 0));
            test_bool(false, ecs_query_next(&it));
        }

        ecs_query_fini(q);
    }

    ecs_fini(world);
}

void Operators_1_ent_src_not_pair(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, Tgt);

    ecs_entity_t e1 = ecs_entity(world, { .name = "e1" });
    ecs_add_pair(world, e1, RelA, Tgt);

    {
        ecs_query_t *q = ecs_query(world, {
            .expr = "!RelA(e1, Tgt)",
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        {
            ecs_iter_t it = ecs_query_iter(world, q);
            test_bool(false, ecs_query_next(&it));
        }

        ecs_query_fini(q);
    }

    {
        ecs_query_t *q = ecs_query(world, {
            .expr = "!RelB(e1, Tgt)",
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        {
            ecs_iter_t it = ecs_query_iter(world, q);
            test_bool(true, ecs_query_next(&it));
            test_uint(0, it.count);
            test_uint(ecs_pair(RelB, Tgt), ecs_field_id(&it, 0));
            test_uint(e1, ecs_field_src(&it, 0));
            test_bool(false, ecs_field_is_set(&it, 0));
            test_bool(false, ecs_query_next(&it));
        }

        ecs_query_fini(q);
    }

    ecs_fini(world);
}

void Operators_1_ent_src_not_pair_rel_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_entity_t e1 = ecs_entity(world, { .name = "e1" });
    ecs_add_pair(world, e1, RelA, TgtA);
    ecs_add_pair(world, e1, RelB, TgtA);

    {
        ecs_query_t *q = ecs_query(world, {
            .expr = "!*(e1, TgtA)",
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        {
            ecs_iter_t it = ecs_query_iter(world, q);
            test_bool(false, ecs_query_next(&it));
        }

        ecs_query_fini(q);
    }

    {
        ecs_query_t *q = ecs_query(world, {
            .expr = "!*(e1, TgtB)",
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        {
            ecs_iter_t it = ecs_query_iter(world, q);
            test_bool(true, ecs_query_next(&it));
            test_uint(0, it.count);
            test_uint(ecs_pair(EcsWildcard, TgtB), ecs_field_id(&it, 0));
            test_uint(e1, ecs_field_src(&it, 0));
            test_bool(false, ecs_field_is_set(&it, 0));
            test_bool(false, ecs_query_next(&it));
        }

        ecs_query_fini(q);
    }

    ecs_fini(world);
}

void Operators_1_ent_src_not_pair_tgt_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_entity_t e1 = ecs_entity(world, { .name = "e1" });
    ecs_add_pair(world, e1, RelA, TgtA);
    ecs_add_pair(world, e1, RelA, TgtB);

    {
        ecs_query_t *q = ecs_query(world, {
            .expr = "!RelA(e1, *)",
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        {
            ecs_iter_t it = ecs_query_iter(world, q);
            test_bool(false, ecs_query_next(&it));
        }

        ecs_query_fini(q);
    }

    {
        ecs_query_t *q = ecs_query(world, {
            .expr = "!RelB(e1, *)",
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        {
            ecs_iter_t it = ecs_query_iter(world, q);
            test_bool(true, ecs_query_next(&it));
            test_uint(0, it.count);
            test_uint(ecs_pair(RelB, EcsWildcard), ecs_field_id(&it, 0));
            test_uint(e1, ecs_field_src(&it, 0));
            test_bool(false, ecs_field_is_set(&it, 0));
            test_bool(false, ecs_query_next(&it));
        }

        ecs_query_fini(q);
    }

    ecs_fini(world);
}

void Operators_1_ent_src_not_pair_rel_tgt_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_entity_t e1 = ecs_entity(world, { .name = "e1" });
    ecs_add_pair(world, e1, RelA, TgtA);
    ecs_add_pair(world, e1, RelA, TgtB);

    {
        ecs_query_t *q = ecs_query(world, {
            .expr = "!*(e1, *)",
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        {
            ecs_iter_t it = ecs_query_iter(world, q);
            test_bool(false, ecs_query_next(&it));
        }

        ecs_query_fini(q);
    }

    ecs_fini(world);
}

void Operators_1_ent_src_not_pair_rel_any(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_entity_t e1 = ecs_entity(world, { .name = "e1" });
    ecs_add_pair(world, e1, RelA, TgtA);
    ecs_add_pair(world, e1, RelB, TgtA);

    {
        ecs_query_t *q = ecs_query(world, {
            .expr = "!_(e1, TgtA)",
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        {
            ecs_iter_t it = ecs_query_iter(world, q);
            test_bool(false, ecs_query_next(&it));
        }

        ecs_query_fini(q);
    }

    {
        ecs_query_t *q = ecs_query(world, {
            .expr = "!_(e1, TgtB)",
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        {
            ecs_iter_t it = ecs_query_iter(world, q);
            test_bool(true, ecs_query_next(&it));
            test_uint(0, it.count);
            test_uint(ecs_pair(EcsWildcard, TgtB), ecs_field_id(&it, 0));
            test_uint(e1, ecs_field_src(&it, 0));
            test_bool(false, ecs_field_is_set(&it, 0));
            test_bool(false, ecs_query_next(&it));
        }

        ecs_query_fini(q);
    }

    ecs_fini(world);
}

void Operators_1_ent_src_not_pair_tgt_any(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_entity_t e1 = ecs_entity(world, { .name = "e1" });
    ecs_add_pair(world, e1, RelA, TgtA);
    ecs_add_pair(world, e1, RelA, TgtB);

    {
        ecs_query_t *q = ecs_query(world, {
            .expr = "!RelA(e1, _)",
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        {
            ecs_iter_t it = ecs_query_iter(world, q);
            test_bool(false, ecs_query_next(&it));
        }

        ecs_query_fini(q);
    }

    {
        ecs_query_t *q = ecs_query(world, {
            .expr = "!RelB(e1, _)",
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        {
            ecs_iter_t it = ecs_query_iter(world, q);
            test_bool(true, ecs_query_next(&it));
            test_uint(0, it.count);
            test_uint(ecs_pair(RelB, EcsWildcard), ecs_field_id(&it, 0));
            test_uint(e1, ecs_field_src(&it, 0));
            test_bool(false, ecs_field_is_set(&it, 0));
            test_bool(false, ecs_query_next(&it));
        }

        ecs_query_fini(q);
    }

    ecs_fini(world);
}

void Operators_1_ent_src_not_pair_rel_tgt_any(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_entity_t e1 = ecs_entity(world, { .name = "e1" });
    ecs_add_pair(world, e1, RelA, TgtA);
    ecs_add_pair(world, e1, RelA, TgtB);

    {
        ecs_query_t *q = ecs_query(world, {
            .expr = "!_(e1, _)",
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        {
            ecs_iter_t it = ecs_query_iter(world, q);
            test_bool(false, ecs_query_next(&it));
        }

        ecs_query_fini(q);
    }

    ecs_fini(world);
}

void Operators_1_ent_src_not_pair_rel_var(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_entity_t e1 = ecs_entity(world, { .name = "e1" });
    ecs_add_pair(world, e1, RelA, TgtA);
    ecs_add_pair(world, e1, RelB, TgtA);

    {
        ecs_query_t *q = ecs_query(world, {
            .expr = "!$x(e1, TgtA)",
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        int x_var = ecs_query_find_var(q, "x");
        test_assert(x_var != -1);

        {
            ecs_iter_t it = ecs_query_iter(world, q);
            test_bool(false, ecs_query_next(&it));
        }

        ecs_query_fini(q);
    }

    {
        ecs_query_t *q = ecs_query(world, {
            .expr = "!$x(e1, TgtB)",
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        int x_var = ecs_query_find_var(q, "x");
        test_assert(x_var != -1);

        {
            ecs_iter_t it = ecs_query_iter(world, q);
            test_bool(true, ecs_query_next(&it));
            test_uint(0, it.count);
            test_uint(ecs_pair(EcsWildcard, TgtB), ecs_field_id(&it, 0));
            test_uint(e1, ecs_field_src(&it, 0));
            test_bool(false, ecs_field_is_set(&it, 0));
            test_uint(EcsWildcard, ecs_iter_get_var(&it, x_var));
            test_bool(false, ecs_query_next(&it));
        }

        ecs_query_fini(q);
    }

    ecs_fini(world);
}

void Operators_1_ent_src_not_pair_tgt_var(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_entity_t e1 = ecs_entity(world, { .name = "e1" });
    ecs_add_pair(world, e1, RelA, TgtA);
    ecs_add_pair(world, e1, RelA, TgtB);

    {
        ecs_query_t *q = ecs_query(world, {
            .expr = "!RelA(e1, $x)",
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        int x_var = ecs_query_find_var(q, "x");
        test_assert(x_var != -1);

        {
            ecs_iter_t it = ecs_query_iter(world, q);
            test_bool(false, ecs_query_next(&it));
        }

        ecs_query_fini(q);
    }

    {
        ecs_query_t *q = ecs_query(world, {
            .expr = "!RelB(e1, $x)",
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        int x_var = ecs_query_find_var(q, "x");
        test_assert(x_var != -1);

        {
            ecs_iter_t it = ecs_query_iter(world, q);
            test_bool(true, ecs_query_next(&it));
            test_uint(0, it.count);
            test_uint(ecs_pair(RelB, EcsWildcard), ecs_field_id(&it, 0));
            test_uint(e1, ecs_field_src(&it, 0));
            test_bool(false, ecs_field_is_set(&it, 0));
            test_uint(EcsWildcard, ecs_iter_get_var(&it, x_var));
            test_bool(false, ecs_query_next(&it));
        }

        ecs_query_fini(q);
    }

    ecs_fini(world);
}

void Operators_1_ent_src_not_pair_rel_tgt_var(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_entity_t e1 = ecs_entity(world, { .name = "e1" });
    ecs_add_pair(world, e1, RelA, TgtA);
    ecs_add_pair(world, e1, RelA, TgtB);

    {
        ecs_query_t *q = ecs_query(world, {
            .expr = "!$x(e1, $y)",
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

        ecs_query_fini(q);
    }

    ecs_fini(world);
}

void Operators_1_ent_src_not_pair_rel_tgt_same_var(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_entity_t e1 = ecs_entity(world, { .name = "e1" });
    ecs_add_pair(world, e1, RelA, TgtA);
    ecs_add_pair(world, e1, RelA, TgtB);

    {
        ecs_query_t *q = ecs_query(world, {
            .expr = "!$x(e1, $x)",
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        int x_var = ecs_query_find_var(q, "x");
        test_assert(x_var != -1);

        {
            ecs_iter_t it = ecs_query_iter(world, q);
            test_bool(true, ecs_query_next(&it));
            test_uint(0, it.count);
            test_uint(ecs_pair(EcsWildcard, EcsWildcard), ecs_field_id(&it, 0));
            test_uint(e1, ecs_field_src(&it, 0));
            test_bool(false, ecs_field_is_set(&it, 0));
            test_uint(EcsWildcard, ecs_iter_get_var(&it, x_var));
            test_bool(false, ecs_query_next(&it));
        }

        ecs_query_fini(q);
    }

    ecs_fini(world);
}

void Operators_1_this_src_not_pair_rel_var(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_query_t *q = ecs_query(world, {
        .expr = "!$x($this, TgtA)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    ecs_entity_t e1 = ecs_new(world);
    ecs_add_pair(world, e1, RelA, TgtA);
    ecs_add_pair(world, e1, RelA, TgtB);

    ecs_entity_t e2 = ecs_new(world);
    ecs_add_pair(world, e2, RelA, TgtB);

    bool e2_found = false;

    ecs_iter_t it = ecs_query_iter(world, q);
    while (ecs_query_next(&it)) {
        test_assert(it.count != 0);
        for (int i = 0; i < it.count; i ++) {
            test_assert(it.entities[i] != e1);
            if (it.entities[i] == e2) {
                e2_found = true;
            }
        }

        test_uint(ecs_pair(EcsWildcard, TgtA), ecs_field_id(&it, 0));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(false, ecs_field_is_set(&it, 0));
        test_uint(EcsWildcard, ecs_iter_get_var(&it, x_var));
    }

    test_assert(e2_found);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Operators_1_this_src_not_pair_tgt_var(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_query_t *q = ecs_query(world, {
        .expr = "!RelA($this, $x)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    ecs_entity_t e1 = ecs_new(world);
    ecs_add_pair(world, e1, RelA, TgtA);
    ecs_add_pair(world, e1, RelA, TgtB);

    ecs_entity_t e2 = ecs_new(world);
    ecs_add_pair(world, e2, RelB, TgtA);

    bool e2_found = false;

    ecs_iter_t it = ecs_query_iter(world, q);
    while (ecs_query_next(&it)) {
        test_assert(it.count != 0);
        for (int i = 0; i < it.count; i ++) {
            test_assert(it.entities[i] != e1);
            if (it.entities[i] == e2) {
                e2_found = true;
            }
        }

        test_uint(ecs_pair(RelA, EcsWildcard), ecs_field_id(&it, 0));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(false, ecs_field_is_set(&it, 0));
        test_uint(EcsWildcard, ecs_iter_get_var(&it, x_var));
    }

    test_assert(e2_found);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Operators_1_this_src_not_pair_rel_tgt_var(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_query_t *q = ecs_query(world, {
        .expr = "!$x($this, $y)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);
    int y_var = ecs_query_find_var(q, "y");
    test_assert(y_var != -1);

    ecs_entity_t e1 = ecs_new(world);
    ecs_add_pair(world, e1, RelA, TgtA);
    ecs_add_pair(world, e1, RelA, TgtB);

    ecs_entity_t e2 = ecs_new(world);
    ecs_add(world, e2, TgtA);

    bool e2_found = false;

    ecs_iter_t it = ecs_query_iter(world, q);
    while (ecs_query_next(&it)) {
        test_assert(it.count != 0);
        for (int i = 0; i < it.count; i ++) {
            test_assert(it.entities[i] != e1);
            if (it.entities[i] == e2) {
                e2_found = true;
            }
        }

        test_uint(ecs_pair(EcsWildcard, EcsWildcard), ecs_field_id(&it, 0));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(false, ecs_field_is_set(&it, 0));
        test_uint(EcsWildcard, ecs_iter_get_var(&it, x_var));
        test_uint(EcsWildcard, ecs_iter_get_var(&it, y_var));
    }

    test_assert(e2_found);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Operators_1_this_src_not_pair_rel_tgt_same_var(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_entity_t e1 = ecs_new(world);
    ecs_add_pair(world, e1, RelA, TgtA);
    ecs_add_pair(world, e1, RelA, TgtB);

    ecs_entity_t e2 = ecs_new(world);
    ecs_add_pair(world, e2, RelA, TgtA);
    ecs_add_pair(world, e2, RelA, RelA);

    ecs_entity_t e3 = ecs_new(world);
    ecs_add(world, e3, TgtA);

    ecs_query_t *q = ecs_query(world, {
        .expr = "!$x($this, $x)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    bool e1_found = false;

    ecs_iter_t it = ecs_query_iter(world, q);
    while (ecs_query_next(&it)) {
        test_assert(it.count != 0);
        for (int i = 0; i < it.count; i ++) {
            test_assert(it.entities[i] != e2);
            if (it.entities[i] == e1) {
                e1_found = true;
            }
        }

        test_uint(ecs_pair(EcsWildcard, EcsWildcard), ecs_field_id(&it, 0));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(false, ecs_field_is_set(&it, 0));
        test_uint(EcsWildcard, ecs_iter_get_var(&it, x_var));
    }

    test_assert(e1_found);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Operators_1_ent_src_not_pair_rel_var_written(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);
    ECS_ENTITY(world, RelA, Tag);
    ECS_ENTITY(world, RelB, Tag);
    ECS_ENTITY(world, RelC, Tag);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_entity_t e1 = ecs_entity(world, { .name = "e1" });
    ecs_add_pair(world, e1, RelA, TgtA);
    ecs_add_pair(world, e1, RelB, TgtA);
    ecs_add_pair(world, e1, RelB, TgtB);
    ecs_add_pair(world, e1, RelC, TgtB);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Tag($x), !$x(e1, TgtA)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(Tag, ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelC, TgtA), ecs_field_id(&it, 1));
        test_uint(RelC, ecs_field_src(&it, 0));
        test_uint(e1, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_field_is_set(&it, 1));
        test_uint(RelC, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Operators_1_ent_src_not_pair_tgt_var_written(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);
    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_ENTITY(world, TgtA, Tag);
    ECS_ENTITY(world, TgtB, Tag);
    ECS_ENTITY(world, TgtC, Tag);

    ecs_entity_t e1 = ecs_entity(world, { .name = "e1" });
    ecs_add_pair(world, e1, RelA, TgtA);
    ecs_add_pair(world, e1, RelA, TgtB);
    ecs_add_pair(world, e1, RelB, TgtB);
    ecs_add_pair(world, e1, RelB, TgtC);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Tag($x), !RelA(e1, $x)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(Tag, ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelA, TgtC), ecs_field_id(&it, 1));
        test_uint(TgtC, ecs_field_src(&it, 0));
        test_uint(e1, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_field_is_set(&it, 1));
        test_uint(TgtC, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Operators_1_ent_src_not_pair_rel_tgt_var_written(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);
    ECS_ENTITY(world, RelA, (Tag, TgtA));
    ECS_ENTITY(world, RelB, (Tag, TgtA));
    ECS_ENTITY(world, RelC, (Tag, TgtB));

    ecs_entity_t e1 = ecs_entity(world, { .name = "e1" });
    ecs_add_pair(world, e1, RelA, TgtA);
    ecs_add_pair(world, e1, RelB, TgtA);
    ecs_add_pair(world, e1, RelC, TgtA);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Tag($x, $y), !$x(e1, $y)",
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
        test_uint(0, it.count);
        test_uint(ecs_pair(Tag, TgtB), ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelC, TgtB), ecs_field_id(&it, 1));
        test_uint(RelC, ecs_field_src(&it, 0));
        test_uint(e1, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_field_is_set(&it, 1));
        test_uint(RelC, ecs_iter_get_var(&it, x_var));
        test_uint(TgtB, ecs_iter_get_var(&it, y_var));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Operators_1_ent_src_not_pair_rel_tgt_same_var_written(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);
    ECS_TAG(world, RelA);
    ECS_ENTITY(world, RelB, Tag);
    ECS_ENTITY(world, TgtA, Tag);
    ECS_ENTITY(world, TgtB, Tag);

    ecs_entity_t e1 = ecs_entity(world, { .name = "e1" });
    ecs_add_pair(world, e1, RelA, TgtA);
    ecs_add_pair(world, e1, TgtA, TgtA);
    ecs_add_pair(world, e1, RelB, TgtA);
    ecs_add_pair(world, e1, RelB, TgtB);
    ecs_add_pair(world, e1, TgtB, TgtB);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Tag($x), !$x(e1, $x)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(Tag, ecs_field_id(&it, 0));
        test_uint(ecs_pair(RelB, RelB), ecs_field_id(&it, 1));
        test_uint(RelB, ecs_field_src(&it, 0));
        test_uint(e1, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_field_is_set(&it, 1));
        test_uint(RelB, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Operators_and_from_fixed_src(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_PREFAB(world, Type, TagA, TagB);

    ecs_entity_t e = ecs_entity(world, { .name = "e" });

    ecs_query_t *q = ecs_query(world, {
        .expr = "and | Type(e)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add(world, e, TagA);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add(world, e, TagB);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(e, ecs_field_src(&it, 0));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Operators_not_from_fixed_src(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_PREFAB(world, Type, TagA, TagB);

    ecs_entity_t e = ecs_entity(world, { .name = "e" });

    ecs_query_t *q = ecs_query(world, {
        .expr = "not | Type(e)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(e, ecs_field_src(&it, 0));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add(world, e, TagA);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add(world, e, TagB);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_remove(world, e, TagA);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_remove(world, e, TagB);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(e, ecs_field_src(&it, 0));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Operators_or_from_fixed_src(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_PREFAB(world, Type, TagA, TagB);

    ecs_entity_t e = ecs_entity(world, { .name = "e" });

    ecs_query_t *q = ecs_query(world, {
        .expr = "or | Type(e)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add(world, e, TagA);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(e, ecs_field_src(&it, 0));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add(world, e, TagB);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(e, ecs_field_src(&it, 0));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_remove(world, e, TagA);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(e, ecs_field_src(&it, 0));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_remove(world, e, TagB);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Operators_and_from_this(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, TagC);
    ECS_PREFAB(world, Type, TagA, TagB);

    ecs_entity_t e1 = ecs_entity(world, { .name = "e1" });
    ecs_entity_t e2 = ecs_entity(world, { .name = "e2" });
    ecs_add(world, e2, TagC);

    ecs_query_t *q = ecs_query(world, {
        .expr = "and | Type",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add(world, e1, TagA);
    ecs_add(world, e2, TagA);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add(world, e1, TagB);
    ecs_add(world, e2, TagB);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e1, it.entities[0]);
        test_uint(0, ecs_field_src(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e2, it.entities[0]);
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_remove(world, e1, TagA);
    ecs_remove(world, e2, TagA);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Operators_not_from_this(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, TagC);
    ECS_PREFAB(world, Type, TagA, TagB);

    ecs_entity_t e1 = ecs_entity(world, { .name = "e1" });
    ecs_entity_t e2 = ecs_entity(world, { .name = "e2" });
    ecs_add(world, e2, TagC);

    ecs_query_t *q = ecs_query(world, {
        .expr = "not | Type",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        bool e1_found = false;
        bool e2_found = false;
        while (ecs_query_next(&it)) {
            for (int i = 0; i < it.count; i ++) {
                e1_found |= it.entities[i] == e1;
                e2_found |= it.entities[i] == e2;
            }
            test_uint(0, ecs_field_src(&it, 0));
        }
        test_assert(e1_found == true);
        test_assert(e2_found == true);
    }

    ecs_add(world, e1, TagA);
    ecs_add(world, e2, TagA);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        bool e1_found = false;
        bool e2_found = false;
        while (ecs_query_next(&it)) {
            for (int i = 0; i < it.count; i ++) {
                e1_found |= it.entities[i] == e1;
                e2_found |= it.entities[i] == e2;
            }
            test_uint(0, ecs_field_src(&it, 0));
        }
        test_assert(e1_found == false);
        test_assert(e2_found == false);
    }


    ecs_add(world, e1, TagB);
    ecs_add(world, e2, TagB);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        bool e1_found = false;
        bool e2_found = false;
        while (ecs_query_next(&it)) {
            for (int i = 0; i < it.count; i ++) {
                e1_found |= it.entities[i] == e1;
                e2_found |= it.entities[i] == e2;
            }
            test_uint(0, ecs_field_src(&it, 0));
        }
        test_assert(e1_found == false);
        test_assert(e2_found == false);
    }

    ecs_remove(world, e1, TagA);
    ecs_remove(world, e2, TagA);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        bool e1_found = false;
        bool e2_found = false;
        while (ecs_query_next(&it)) {
            for (int i = 0; i < it.count; i ++) {
                e1_found |= it.entities[i] == e1;
                e2_found |= it.entities[i] == e2;
            }
            test_uint(0, ecs_field_src(&it, 0));
        }
        test_assert(e1_found == false);
        test_assert(e2_found == false);
    }

    ecs_remove(world, e1, TagB);
    ecs_remove(world, e2, TagB);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        bool e1_found = false;
        bool e2_found = false;
        while (ecs_query_next(&it)) {
            for (int i = 0; i < it.count; i ++) {
                e1_found |= it.entities[i] == e1;
                e2_found |= it.entities[i] == e2;
            }
            test_uint(0, ecs_field_src(&it, 0));
        }
        test_assert(e1_found == true);
        test_assert(e2_found == true);
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Operators_or_from_this(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, TagC);
    ECS_PREFAB(world, Type, TagA, TagB);

    ecs_entity_t e1 = ecs_entity(world, { .name = "e1" });
    ecs_entity_t e2 = ecs_entity(world, { .name = "e2" });
    ecs_add(world, e2, TagC);

    ecs_query_t *q = ecs_query(world, {
        .expr = "or | Type",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add(world, e1, TagA);
    ecs_add(world, e2, TagA);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e1, it.entities[0]);
        test_uint(0, ecs_field_src(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e2, it.entities[0]);
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add(world, e1, TagB);
    ecs_add(world, e2, TagB);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e1, it.entities[0]);
        test_uint(0, ecs_field_src(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e2, it.entities[0]);
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_remove(world, e2, TagA);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e1, it.entities[0]);
        test_uint(0, ecs_field_src(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e2, it.entities[0]);
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_remove(world, e1, TagA);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e2, it.entities[0]);
        test_uint(0, ecs_field_src(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e1, it.entities[0]);
        test_uint(0, ecs_field_src(&it, 0));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_remove(world, e1, TagB);
    ecs_remove(world, e2, TagB);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Operators_and_from_this_written(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, TagC);
    ECS_TAG(world, TagD);
    ECS_PREFAB(world, Type, TagA, TagB);

    ecs_entity_t e1 = ecs_entity(world, { .name = "e1" });
    ecs_add(world, e1, TagD);
    ecs_entity_t e2 = ecs_entity(world, { .name = "e2" });
    ecs_add(world, e2, TagC);
    ecs_add(world, e2, TagD);

    ecs_query_t *q = ecs_query(world, {
        .expr = "TagD, and | Type",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add(world, e1, TagA);
    ecs_add(world, e2, TagA);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add(world, e1, TagB);
    ecs_add(world, e2, TagB);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e1, it.entities[0]);
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e2, it.entities[0]);
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_remove(world, e1, TagA);
    ecs_remove(world, e2, TagA);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Operators_not_from_this_written(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, TagC);
    ECS_TAG(world, TagD);
    ECS_PREFAB(world, Type, TagA, TagB);

    ecs_entity_t e1 = ecs_entity(world, { .name = "e1" });
    ecs_add(world, e1, TagD);
    ecs_entity_t e2 = ecs_entity(world, { .name = "e2" });
    ecs_add(world, e2, TagC);
    ecs_add(world, e2, TagD);

    ecs_query_t *q = ecs_query(world, {
        .expr = "TagD, not | Type",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        bool e1_found = false;
        bool e2_found = false;
        while (ecs_query_next(&it)) {
            for (int i = 0; i < it.count; i ++) {
                e1_found |= it.entities[i] == e1;
                e2_found |= it.entities[i] == e2;
            }
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 0));
            test_bool(true, ecs_field_is_set(&it, 0));
            test_bool(false, ecs_field_is_set(&it, 1));
        }
        test_assert(e1_found == true);
        test_assert(e2_found == true);
    }

    ecs_add(world, e1, TagA);
    ecs_add(world, e2, TagA);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        bool e1_found = false;
        bool e2_found = false;
        while (ecs_query_next(&it)) {
            for (int i = 0; i < it.count; i ++) {
                e1_found |= it.entities[i] == e1;
                e2_found |= it.entities[i] == e2;
            }
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 0));
            test_bool(true, ecs_field_is_set(&it, 0));
            test_bool(false, ecs_field_is_set(&it, 1));
        }
        test_assert(e1_found == false);
        test_assert(e2_found == false);
    }


    ecs_add(world, e1, TagB);
    ecs_add(world, e2, TagB);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        bool e1_found = false;
        bool e2_found = false;
        while (ecs_query_next(&it)) {
            for (int i = 0; i < it.count; i ++) {
                e1_found |= it.entities[i] == e1;
                e2_found |= it.entities[i] == e2;
            }
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 0));
            test_bool(true, ecs_field_is_set(&it, 0));
            test_bool(false, ecs_field_is_set(&it, 1));
        }
        test_assert(e1_found == false);
        test_assert(e2_found == false);
    }

    ecs_remove(world, e1, TagA);
    ecs_remove(world, e2, TagA);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        bool e1_found = false;
        bool e2_found = false;
        while (ecs_query_next(&it)) {
            for (int i = 0; i < it.count; i ++) {
                e1_found |= it.entities[i] == e1;
                e2_found |= it.entities[i] == e2;
            }
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 0));
            test_bool(true, ecs_field_is_set(&it, 0));
            test_bool(false, ecs_field_is_set(&it, 1));
        }
        test_assert(e1_found == false);
        test_assert(e2_found == false);
    }

    ecs_remove(world, e1, TagB);
    ecs_remove(world, e2, TagB);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        bool e1_found = false;
        bool e2_found = false;
        while (ecs_query_next(&it)) {
            for (int i = 0; i < it.count; i ++) {
                e1_found |= it.entities[i] == e1;
                e2_found |= it.entities[i] == e2;
            }
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 0));
            test_bool(true, ecs_field_is_set(&it, 0));
            test_bool(false, ecs_field_is_set(&it, 1));
        }
        test_assert(e1_found == true);
        test_assert(e2_found == true);
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Operators_or_from_this_written(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, TagC);
    ECS_TAG(world, TagD);
    ECS_PREFAB(world, Type, TagA, TagB);

    ecs_entity_t e1 = ecs_entity(world, { .name = "e1" });
    ecs_add(world, e1, TagD);
    ecs_entity_t e2 = ecs_entity(world, { .name = "e2" });
    ecs_add(world, e2, TagC);
    ecs_add(world, e2, TagD);

    ecs_query_t *q = ecs_query(world, {
        .expr = "TagD, or | Type",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add(world, e1, TagA);
    ecs_add(world, e2, TagA);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e1, it.entities[0]);
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e2, it.entities[0]);
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add(world, e1, TagB);
    ecs_add(world, e2, TagB);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e1, it.entities[0]);
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e2, it.entities[0]);
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_remove(world, e2, TagA);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e1, it.entities[0]);
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e2, it.entities[0]);
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_remove(world, e1, TagA);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e2, it.entities[0]);
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e1, it.entities[0]);
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_remove(world, e1, TagB);
    ecs_remove(world, e2, TagB);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Operators_and_from_empty(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity(world, { .name = "Type" });

    ecs_query_t *q = ecs_query(world, {
        .expr = "and | Type",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Operators_not_from_empty(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity(world, { .name = "Type" });

    ecs_query_t *q = ecs_query(world, {
        .expr = "not | Type",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Operators_or_from_empty(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity(world, { .name = "Type" });

    ecs_query_t *q = ecs_query(world, {
        .expr = "or | Type",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Operators_and_from_empty_w_tag(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);

    ecs_entity(world, { .name = "Type" });

    ecs_entity_t e = ecs_new_w(world, TagA);

    ecs_query_t *q = ecs_query(world, {
        .expr = "and | Type, TagA",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    test_uint(TagA, ecs_field_id(&it, 1));
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Operators_not_from_empty_w_tag(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);

    ecs_entity(world, { .name = "Type" });

    ecs_entity_t e = ecs_new_w(world, TagA);

    ecs_query_t *q = ecs_query(world, {
        .expr = "not | Type, TagA",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    test_uint(TagA, ecs_field_id(&it, 1));
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Operators_or_from_empty_w_tag(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);

    ecs_entity(world, { .name = "Type" });

    ecs_entity_t e = ecs_new_w(world, TagA);

    ecs_query_t *q = ecs_query(world, {
        .expr = "or | Type, TagA",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    test_uint(TagA, ecs_field_id(&it, 1));
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Operators_or_w_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);
    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { TagA, .oper = EcsOr },
            { TagB },
            { ecs_pair(Rel, EcsWildcard) }
        }
    });

    test_assert(q != NULL);

    ecs_entity_t e = ecs_new(world);
    ecs_add(world, e, TagA);
    ecs_add(world, e, TagB);
    ecs_add_pair(world, e, Rel, TgtA);
    ecs_add_pair(world, e, Rel, TgtB);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e, it.entities[0]);
    test_uint(TagA, ecs_field_id(&it, 0));
    test_uint(ecs_pair(Rel, TgtA), ecs_field_id(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e, it.entities[0]);
    test_uint(TagA, ecs_field_id(&it, 0));
    test_uint(ecs_pair(Rel, TgtB), ecs_field_id(&it, 1));
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Operators_or_w_component_and_tag(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, TagA);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { ecs_id(Position), .oper = EcsOr },
            { TagA }
        }
    });

    test_assert(q != NULL);

    ecs_entity_t e = ecs_new(world);
    ecs_set(world, e, Position, {10, 20});
    ecs_add(world, e, TagA);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Operators_or_w_tag_and_component(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, TagA);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { TagA, .oper = EcsOr },
            { ecs_id(Position) }
        }
    });

    test_assert(q != NULL);

    ecs_entity_t e = ecs_new(world);
    ecs_set(world, e, Position, {10, 20});
    ecs_add(world, e, TagA);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e, it.entities[0]);
    test_uint(TagA, ecs_field_id(&it, 0));
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}
