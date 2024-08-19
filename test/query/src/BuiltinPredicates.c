#include <query.h>

static ecs_query_cache_kind_t cache_kind = EcsQueryCacheDefault;

void BuiltinPredicates_setup(void) {
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

void BuiltinPredicates_this_eq_id(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t ent = ecs_entity(world, { .name = "ent" });

    ecs_query_t *q = ecs_query(world, {
        .expr = "$this == ent",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(false, ecs_field_is_set(&it, 0));
        test_uint(ent, it.entities[0]);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void BuiltinPredicates_this_eq_name(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t ent = ecs_entity(world, { .name = "ent" });

    ecs_query_t *q = ecs_query(world, {
        .expr = "$this == \"ent\"",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(false, ecs_field_is_set(&it, 0));
        test_uint(ent, it.entities[0]);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void BuiltinPredicates_this_eq_var(void) {
    ecs_world_t *world = ecs_mini();

    ecs_log_set_level(-4);
    ecs_query_t *q = ecs_query(world, {
        .expr = "$this == $x",
        .cache_kind = cache_kind
    });

    test_assert(q == NULL);

    ecs_fini(world);
}

void BuiltinPredicates_this_eq_id_written(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);

    ecs_entity_t ent_1 = ecs_entity(world, { .name = "ent_1" });
    ecs_add(world, ent_1, RelA);
    ecs_entity_t ent_2 = ecs_entity(world, { .name = "ent_2" });
    ecs_add(world, ent_2, RelA);
    ecs_entity_t ent_3 = ecs_entity(world, { .name = "ent_3" });
    ecs_add(world, ent_3, RelA);

    ecs_query_t *q = ecs_query(world, {
        .expr = "RelA($this), $this == ent_2",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(true, ecs_field_is_set(&it, 0));
        test_uint(false, ecs_field_is_set(&it, 1));
        test_uint(ent_2, it.entities[0]);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void BuiltinPredicates_this_eq_id_written_no_match(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);

    ecs_entity_t ent_1 = ecs_entity(world, { .name = "ent_1" });
    ecs_add(world, ent_1, RelA);
    ecs_entity_t ent_2 = ecs_entity(world, { .name = "ent_2" });
    ecs_add(world, ent_2, RelA);
    ecs_entity_t ent_3 = ecs_entity(world, { .name = "ent_3" });
    ecs_add(world, ent_3, RelA);
    ecs_entity_t ent_4 = ecs_entity(world, { .name = "ent_4" });
    ecs_add(world, ent_4, RelB);

    ecs_query_t *q = ecs_query(world, {
        .expr = "RelB($this), $this == ent_2",
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

void BuiltinPredicates_this_eq_name_written(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);

    ecs_entity_t ent_1 = ecs_entity(world, { .name = "ent_1" });
    ecs_add(world, ent_1, RelA);
    ecs_entity_t ent_2 = ecs_entity(world, { .name = "ent_2" });
    ecs_add(world, ent_2, RelA);
    ecs_entity_t ent_3 = ecs_entity(world, { .name = "ent_3" });
    ecs_add(world, ent_3, RelA);

    ecs_query_t *q = ecs_query(world, {
        .expr = "RelA($this), $this == \"ent_2\"",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(true, ecs_field_is_set(&it, 0));
        test_uint(false, ecs_field_is_set(&it, 1));
        test_uint(ent_2, it.entities[0]);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void BuiltinPredicates_this_eq_name_written_no_match(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);

    ecs_entity_t ent_1 = ecs_entity(world, { .name = "ent_1" });
    ecs_add(world, ent_1, RelA);
    ecs_entity_t ent_2 = ecs_entity(world, { .name = "ent_2" });
    ecs_add(world, ent_2, RelA);
    ecs_entity_t ent_3 = ecs_entity(world, { .name = "ent_3" });
    ecs_add(world, ent_3, RelA);
    ecs_entity_t ent_4 = ecs_entity(world, { .name = "ent_4" });
    ecs_add(world, ent_4, RelB);

    ecs_query_t *q = ecs_query(world, {
        .expr = "RelB($this), $this == \"ent_2\"",
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

void BuiltinPredicates_this_eq_var_written(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);

    ecs_entity_t ent_2 = ecs_entity(world, { .name = "ent_2" });
    ecs_entity_t ent_1 = ecs_entity(world, { .name = "ent_1" });
    ecs_add_pair(world, ent_1, RelA, ent_2);
    ecs_add_pair(world, ent_2, RelA, ent_2);
    ecs_entity_t ent_3 = ecs_entity(world, { .name = "ent_3" });
    ecs_add_pair(world, ent_3, RelA, ent_2);

    ecs_query_t *q = ecs_query(world, {
        .expr = "RelA($this, $x), $this == $x",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);
    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(RelA, ent_2), ecs_field_id(&it, 0));
        test_uint(true, ecs_field_is_set(&it, 0));
        test_uint(false, ecs_field_is_set(&it, 1));
        test_uint(ent_2, ecs_iter_get_var(&it, x_var));
        test_uint(ent_2, it.entities[0]);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void BuiltinPredicates_var_eq_id(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t ent = ecs_entity(world, { .name = "ent" });

    ecs_query_t *q = ecs_query(world, {
        .expr = "$x == ent",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);
    
    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(false, ecs_field_is_set(&it, 0));
        test_uint(ent, ecs_iter_get_var(&it, x_var));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void BuiltinPredicates_var_eq_name(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t ent = ecs_entity(world, { .name = "ent" });

    ecs_query_t *q = ecs_query(world, {
        .expr = "$x == \"ent\"",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);
    
    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(false, ecs_field_is_set(&it, 0));
        test_uint(ent, ecs_iter_get_var(&it, x_var));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void BuiltinPredicates_var_eq_var(void) {
    ecs_world_t *world = ecs_mini();

    ecs_log_set_level(-4);
    ecs_query_t *q = ecs_query(world, {
        .expr = "$x == $y",
        .cache_kind = cache_kind
    });

    test_assert(q == NULL);

    ecs_fini(world);
}

void BuiltinPredicates_var_eq_this(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_entity_t e1 = ecs_new_w(world, Foo);
    ecs_add(world, e1, Bar);
    /* ecs_entity_t e2 = */ ecs_new_w(world, Foo);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Foo($this), $x == $this, Bar($x)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(e1, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(true, ecs_field_is_set(&it, 0));
        test_uint(e1, ecs_iter_get_var(&it, x_var));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void BuiltinPredicates_var_eq_id_written(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);

    ecs_entity_t ent_1 = ecs_entity(world, { .name = "ent_1" });
    ecs_add(world, ent_1, RelA);
    ecs_entity_t ent_2 = ecs_entity(world, { .name = "ent_2" });
    ecs_add(world, ent_2, RelA);
    ecs_entity_t ent_3 = ecs_entity(world, { .name = "ent_3" });
    ecs_add(world, ent_3, RelA);

    ecs_query_t *q = ecs_query(world, {
        .expr = "RelA($x), $x == ent_2",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);
    
    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(true, ecs_field_is_set(&it, 0));
        test_uint(false, ecs_field_is_set(&it, 1));
        test_uint(ent_2, ecs_iter_get_var(&it, x_var));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void BuiltinPredicates_var_eq_id_written_no_match(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);

    ecs_entity_t ent_1 = ecs_entity(world, { .name = "ent_1" });
    ecs_add(world, ent_1, RelA);
    ecs_entity_t ent_2 = ecs_entity(world, { .name = "ent_2" });
    ecs_add(world, ent_2, RelA);
    ecs_entity_t ent_3 = ecs_entity(world, { .name = "ent_3" });
    ecs_add(world, ent_3, RelA);
    ecs_entity_t ent_4 = ecs_entity(world, { .name = "ent_4" });
    ecs_add(world, ent_4, RelB);

    ecs_query_t *q = ecs_query(world, {
        .expr = "RelB($x), $x == ent_2",
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

    ecs_fini(world);
}

void BuiltinPredicates_var_eq_name_written(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);

    ecs_entity_t ent_1 = ecs_entity(world, { .name = "ent_1" });
    ecs_add(world, ent_1, RelA);
    ecs_entity_t ent_2 = ecs_entity(world, { .name = "ent_2" });
    ecs_add(world, ent_2, RelA);
    ecs_entity_t ent_3 = ecs_entity(world, { .name = "ent_3" });
    ecs_add(world, ent_3, RelA);

    ecs_query_t *q = ecs_query(world, {
        .expr = "RelA($x), $x == \"ent_2\"",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);
    
    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(true, ecs_field_is_set(&it, 0));
        test_uint(false, ecs_field_is_set(&it, 1));
        test_uint(ent_2, ecs_iter_get_var(&it, x_var));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void BuiltinPredicates_var_eq_name_written_no_match(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);

    ecs_entity_t ent_1 = ecs_entity(world, { .name = "ent_1" });
    ecs_add(world, ent_1, RelA);
    ecs_entity_t ent_2 = ecs_entity(world, { .name = "ent_2" });
    ecs_add(world, ent_2, RelA);
    ecs_entity_t ent_3 = ecs_entity(world, { .name = "ent_3" });
    ecs_add(world, ent_3, RelA);
    ecs_entity_t ent_4 = ecs_entity(world, { .name = "ent_4" });
    ecs_add(world, ent_4, RelB);

    ecs_query_t *q = ecs_query(world, {
        .expr = "RelB($x), $x == \"ent_2\"",
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

    ecs_fini(world);
}

void BuiltinPredicates_var_eq_var_written(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);

    ecs_entity_t ent_2 = ecs_entity(world, { .name = "ent_2" });
    ecs_entity_t ent_1 = ecs_entity(world, { .name = "ent_1" });
    ecs_add_pair(world, ent_1, RelA, ent_2);
    ecs_add_pair(world, ent_2, RelA, ent_2);
    ecs_entity_t ent_3 = ecs_entity(world, { .name = "ent_3" });
    ecs_add_pair(world, ent_3, RelA, ent_2);

    ecs_query_t *q = ecs_query(world, {
        .expr = "RelA($x, $y), $x == $y",
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
        test_uint(ecs_pair(RelA, ent_2), ecs_field_id(&it, 0));
        test_uint(true, ecs_field_is_set(&it, 0));
        test_uint(false, ecs_field_is_set(&it, 1));
        test_uint(ent_2, ecs_iter_get_var(&it, x_var));
        test_uint(ent_2, ecs_iter_get_var(&it, y_var));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void BuiltinPredicates_this_neq_id(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    ecs_entity_t ent_1 = ecs_entity(world, { .name = "ent_1" });
    ecs_entity_t ent_2 = ecs_entity(world, { .name = "ent_2" });
    ecs_entity_t ent_3 = ecs_entity(world, { .name = "ent_3" });
    ecs_entity_t ent_4 = ecs_entity(world, { .name = "ent_4" });
    ecs_add(world, ent_4, Tag);

    ecs_query_t *r = ecs_query(world, {
        .expr = "$this != ent_2",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    bool ent_1_found = false;
    bool ent_3_found = false;
    bool ent_4_found = false;

    ecs_iter_t it = ecs_query_iter(world, r);
    while (ecs_query_next(&it)) {
        for (int i = 0; i < it.count; i ++) {
            test_assert(it.entities[i] != ent_2);
            if (it.entities[i] == ent_1) {
                ent_1_found = true;
            }
            if (it.entities[i] == ent_3) {
                ent_3_found = true;
            }
            if (it.entities[i] == ent_4) {
                ent_4_found = true;
            }
        }
    }

    test_bool(ent_1_found, true);
    test_bool(ent_3_found, true);
    test_bool(ent_4_found, true);

    ecs_query_fini(r);

    ecs_fini(world);
}

void BuiltinPredicates_this_neq_name(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    ecs_entity_t ent_1 = ecs_entity(world, { .name = "ent_1" });
    ecs_entity_t ent_2 = ecs_entity(world, { .name = "ent_2" });
    ecs_entity_t ent_3 = ecs_entity(world, { .name = "ent_3" });
    ecs_entity_t ent_4 = ecs_entity(world, { .name = "ent_4" });
    ecs_add(world, ent_4, Tag);

    ecs_query_t *q = ecs_query(world, {
        .expr = "$this != \"ent_2\"",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    bool ent_1_found = false;
    bool ent_3_found = false;
    bool ent_4_found = false;

    ecs_iter_t it = ecs_query_iter(world, q);
    while (ecs_query_next(&it)) {
        for (int i = 0; i < it.count; i ++) {
            test_assert(it.entities[i] != ent_2);
            if (it.entities[i] == ent_1) {
                ent_1_found = true;
            }
            if (it.entities[i] == ent_3) {
                ent_3_found = true;
            }
            if (it.entities[i] == ent_4) {
                ent_4_found = true;
            }
        }
    }

    test_bool(ent_1_found, true);
    test_bool(ent_3_found, true);
    test_bool(ent_4_found, true);

    ecs_query_fini(q);

    ecs_fini(world);
}

void BuiltinPredicates_this_neq_var(void) {
    ecs_world_t *world = ecs_mini();

    ecs_log_set_level(-4);
    ecs_query_t *q = ecs_query(world, {
        .expr = "$this != $x",
        .cache_kind = cache_kind
    });

    test_assert(q == NULL);

    ecs_fini(world);
}

void BuiltinPredicates_this_neq_id_written(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);

    ecs_entity_t ent_1 = ecs_entity(world, { .name = "ent_1" });
    ecs_add(world, ent_1, RelA);
    ecs_entity_t ent_2 = ecs_entity(world, { .name = "ent_2" });
    ecs_add(world, ent_2, RelA);
    ecs_entity_t ent_3 = ecs_entity(world, { .name = "ent_3" });
    ecs_add(world, ent_3, RelA);

    ecs_query_t *q = ecs_query(world, {
        .expr = "RelA($this), $this != ent_2",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(true, ecs_field_is_set(&it, 0));
        test_uint(false, ecs_field_is_set(&it, 1));
        test_uint(ent_1, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(true, ecs_field_is_set(&it, 0));
        test_uint(false, ecs_field_is_set(&it, 1));
        test_uint(ent_3, it.entities[0]);

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void BuiltinPredicates_this_neq_id_written_no_match(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);

    ecs_entity_t ent_1 = ecs_entity(world, { .name = "ent_1" });
    ecs_add(world, ent_1, RelA);
    ecs_entity_t ent_2 = ecs_entity(world, { .name = "ent_2" });
    ecs_add(world, ent_2, RelA);
    ecs_entity_t ent_3 = ecs_entity(world, { .name = "ent_3" });
    ecs_add(world, ent_3, RelA);
    ecs_entity_t ent_4 = ecs_entity(world, { .name = "ent_4" });
    ecs_add(world, ent_4, RelB);

    ecs_query_t *q = ecs_query(world, {
        .expr = "RelB($this), $this != ent_2",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(RelB, ecs_field_id(&it, 0));
        test_uint(true, ecs_field_is_set(&it, 0));
        test_uint(false, ecs_field_is_set(&it, 1));
        test_uint(ent_4, it.entities[0]);

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void BuiltinPredicates_this_neq_name_written(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);

    ecs_entity_t ent_1 = ecs_entity(world, { .name = "ent_1" });
    ecs_add(world, ent_1, RelA);
    ecs_entity_t ent_2 = ecs_entity(world, { .name = "ent_2" });
    ecs_add(world, ent_2, RelA);
    ecs_entity_t ent_3 = ecs_entity(world, { .name = "ent_3" });
    ecs_add(world, ent_3, RelA);

    ecs_query_t *q = ecs_query(world, {
        .expr = "RelA($this), $this != \"ent_2\"",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(true, ecs_field_is_set(&it, 0));
        test_uint(false, ecs_field_is_set(&it, 1));
        test_uint(ent_1, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(true, ecs_field_is_set(&it, 0));
        test_uint(false, ecs_field_is_set(&it, 1));
        test_uint(ent_3, it.entities[0]);

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void BuiltinPredicates_this_neq_name_written_no_match(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);

    ecs_entity_t ent_1 = ecs_entity(world, { .name = "ent_1" });
    ecs_add(world, ent_1, RelA);
    ecs_entity_t ent_2 = ecs_entity(world, { .name = "ent_2" });
    ecs_add(world, ent_2, RelA);
    ecs_entity_t ent_3 = ecs_entity(world, { .name = "ent_3" });
    ecs_add(world, ent_3, RelA);
    ecs_entity_t ent_4 = ecs_entity(world, { .name = "ent_4" });
    ecs_add(world, ent_4, RelB);

    ecs_query_t *q = ecs_query(world, {
        .expr = "RelB($this), $this != \"ent_2\"",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(RelB, ecs_field_id(&it, 0));
        test_uint(true, ecs_field_is_set(&it, 0));
        test_uint(false, ecs_field_is_set(&it, 1));
        test_uint(ent_4, it.entities[0]);

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void BuiltinPredicates_this_neq_var_written(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);

    ecs_entity_t ent_2 = ecs_entity(world, { .name = "ent_2" });
    ecs_entity_t ent_1 = ecs_entity(world, { .name = "ent_1" });
    ecs_add_pair(world, ent_1, RelA, ent_2);
    ecs_add_pair(world, ent_2, RelA, ent_2);
    ecs_entity_t ent_3 = ecs_entity(world, { .name = "ent_3" });
    ecs_add_pair(world, ent_3, RelA, ent_2);

    ecs_query_t *q = ecs_query(world, {
        .expr = "RelA($this, $x), $this != $x",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);
    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(RelA, ent_2), ecs_field_id(&it, 0));
        test_uint(true, ecs_field_is_set(&it, 0));
        test_uint(false, ecs_field_is_set(&it, 1));
        test_uint(ent_2, ecs_iter_get_var(&it, x_var));
        test_uint(ent_1, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(RelA, ent_2), ecs_field_id(&it, 0));
        test_uint(true, ecs_field_is_set(&it, 0));
        test_uint(false, ecs_field_is_set(&it, 1));
        test_uint(ent_2, ecs_iter_get_var(&it, x_var));
        test_uint(ent_3, it.entities[0]);

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void BuiltinPredicates_var_neq_id(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    ecs_entity_t ent_1 = ecs_entity(world, { .name = "ent_1" });
    ecs_entity_t ent_2 = ecs_entity(world, { .name = "ent_2" });
    ecs_entity_t ent_3 = ecs_entity(world, { .name = "ent_3" });
    ecs_entity_t ent_4 = ecs_entity(world, { .name = "ent_4" });
    ecs_add(world, ent_4, Tag);

    ecs_query_t *q = ecs_query(world, {
        .expr = "$x != ent_2",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    bool ent_1_found = false;
    bool ent_3_found = false;
    bool ent_4_found = false;

    ecs_iter_t it = ecs_query_iter(world, q);
    while (ecs_query_next(&it)) {
        test_uint(it.count, 0);
        ecs_entity_t e = ecs_iter_get_var(&it, x_var);
        test_assert(e != ent_2);
        if (e == ent_1) {
            ent_1_found = true;
        }
        if (e == ent_3) {
            ent_3_found = true;
        }
        if (e == ent_4) {
            ent_4_found = true;
        }
    }

    test_bool(ent_1_found, true);
    test_bool(ent_3_found, true);
    test_bool(ent_4_found, true);

    ecs_query_fini(q);

    ecs_fini(world);
}

void BuiltinPredicates_var_neq_name(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    ecs_entity_t ent_1 = ecs_entity(world, { .name = "ent_1" });
    ecs_entity_t ent_2 = ecs_entity(world, { .name = "ent_2" });
    ecs_entity_t ent_3 = ecs_entity(world, { .name = "ent_3" });
    ecs_entity_t ent_4 = ecs_entity(world, { .name = "ent_4" });
    ecs_add(world, ent_4, Tag);

    ecs_query_t *q = ecs_query(world, {
        .expr = "$x != \"ent_2\"",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    bool ent_1_found = false;
    bool ent_3_found = false;
    bool ent_4_found = false;

    ecs_iter_t it = ecs_query_iter(world, q);
    while (ecs_query_next(&it)) {
        test_uint(it.count, 0);
        ecs_entity_t e = ecs_iter_get_var(&it, x_var);
        test_assert(e != ent_2);
        if (e == ent_1) {
            ent_1_found = true;
        }
        if (e == ent_3) {
            ent_3_found = true;
        }
        if (e == ent_4) {
            ent_4_found = true;
        }
    }

    test_bool(ent_1_found, true);
    test_bool(ent_3_found, true);
    test_bool(ent_4_found, true);

    ecs_query_fini(q);

    ecs_fini(world);
}

void BuiltinPredicates_var_neq_var(void) {
    ecs_world_t *world = ecs_mini();

    ecs_log_set_level(-4);
    ecs_query_t *q = ecs_query(world, {
        .expr = "$x != $y",
        .cache_kind = cache_kind
    });

    test_assert(q == NULL);

    ecs_fini(world);
}

void BuiltinPredicates_var_neq_this(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_TAG(world, Foo);
    ECS_TAG(world, Tgt);

    ecs_entity_t e1 = ecs_new_w_pair(world, Rel, Tgt);
    ecs_entity_t e2 = ecs_new(world);
    ecs_add_pair(world, e2, Rel, e2);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Rel($this, $x), $x != $this",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(e1, it.entities[0]);
        test_uint(ecs_pair(Rel, Tgt), ecs_field_id(&it, 0));
        test_uint(true, ecs_field_is_set(&it, 0));
        test_uint(Tgt, ecs_iter_get_var(&it, x_var));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void BuiltinPredicates_var_neq_id_written(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);

    ecs_entity_t ent_1 = ecs_entity(world, { .name = "ent_1" });
    ecs_add(world, ent_1, RelA);
    ecs_entity_t ent_2 = ecs_entity(world, { .name = "ent_2" });
    ecs_add(world, ent_2, RelA);
    ecs_entity_t ent_3 = ecs_entity(world, { .name = "ent_3" });
    ecs_add(world, ent_3, RelA);

    ecs_query_t *q = ecs_query(world, {
        .expr = "RelA($x), $x != ent_2",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);
    
    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(true, ecs_field_is_set(&it, 0));
        test_uint(false, ecs_field_is_set(&it, 1));
        test_uint(ent_1, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(true, ecs_field_is_set(&it, 0));
        test_uint(false, ecs_field_is_set(&it, 1));
        test_uint(ent_3, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void BuiltinPredicates_var_neq_id_written_no_match(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);

    ecs_entity_t ent_1 = ecs_entity(world, { .name = "ent_1" });
    ecs_add(world, ent_1, RelA);
    ecs_entity_t ent_2 = ecs_entity(world, { .name = "ent_2" });
    ecs_add(world, ent_2, RelA);
    ecs_entity_t ent_3 = ecs_entity(world, { .name = "ent_3" });
    ecs_add(world, ent_3, RelA);
    ecs_entity_t ent_4 = ecs_entity(world, { .name = "ent_4" });
    ecs_add(world, ent_4, RelB);

    ecs_query_t *q = ecs_query(world, {
        .expr = "RelB($x), $x != ent_2",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);
    
    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(RelB, ecs_field_id(&it, 0));
        test_uint(true, ecs_field_is_set(&it, 0));
        test_uint(false, ecs_field_is_set(&it, 1));
        test_uint(ent_4, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void BuiltinPredicates_var_neq_name_written(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);

    ecs_entity_t ent_1 = ecs_entity(world, { .name = "ent_1" });
    ecs_add(world, ent_1, RelA);
    ecs_entity_t ent_2 = ecs_entity(world, { .name = "ent_2" });
    ecs_add(world, ent_2, RelA);
    ecs_entity_t ent_3 = ecs_entity(world, { .name = "ent_3" });
    ecs_add(world, ent_3, RelA);

    ecs_query_t *q = ecs_query(world, {
        .expr = "RelA($x), $x != \"ent_2\"",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);
    
    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(true, ecs_field_is_set(&it, 0));
        test_uint(false, ecs_field_is_set(&it, 1));
        test_uint(ent_1, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(true, ecs_field_is_set(&it, 0));
        test_uint(false, ecs_field_is_set(&it, 1));
        test_uint(ent_3, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void BuiltinPredicates_var_neq_name_written_no_match(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);

    ecs_entity_t ent_1 = ecs_entity(world, { .name = "ent_1" });
    ecs_add(world, ent_1, RelA);
    ecs_entity_t ent_2 = ecs_entity(world, { .name = "ent_2" });
    ecs_add(world, ent_2, RelA);
    ecs_entity_t ent_3 = ecs_entity(world, { .name = "ent_3" });
    ecs_add(world, ent_3, RelA);
    ecs_entity_t ent_4 = ecs_entity(world, { .name = "ent_4" });
    ecs_add(world, ent_4, RelB);

    ecs_query_t *q = ecs_query(world, {
        .expr = "RelB($x), $x != \"ent_2\"",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);
    
    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(RelB, ecs_field_id(&it, 0));
        test_uint(true, ecs_field_is_set(&it, 0));
        test_uint(false, ecs_field_is_set(&it, 1));
        test_uint(ent_4, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void BuiltinPredicates_var_neq_var_written(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);

    ecs_entity_t ent_2 = ecs_entity(world, { .name = "ent_2" });
    ecs_entity_t ent_1 = ecs_entity(world, { .name = "ent_1" });
    ecs_add_pair(world, ent_1, RelA, ent_2);
    ecs_add_pair(world, ent_2, RelA, ent_2);
    ecs_entity_t ent_3 = ecs_entity(world, { .name = "ent_3" });
    ecs_add_pair(world, ent_3, RelA, ent_2);

    ecs_query_t *q = ecs_query(world, {
        .expr = "RelA($x, $y), $x != $y",
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
        test_uint(ecs_pair(RelA, ent_2), ecs_field_id(&it, 0));
        test_uint(true, ecs_field_is_set(&it, 0));
        test_uint(false, ecs_field_is_set(&it, 1));
        test_uint(ent_1, ecs_iter_get_var(&it, x_var));
        test_uint(ent_2, ecs_iter_get_var(&it, y_var));

        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(RelA, ent_2), ecs_field_id(&it, 0));
        test_uint(true, ecs_field_is_set(&it, 0));
        test_uint(false, ecs_field_is_set(&it, 1));
        test_uint(ent_3, ecs_iter_get_var(&it, x_var));
        test_uint(ent_2, ecs_iter_get_var(&it, y_var));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void BuiltinPredicates_this_2_neq_id(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    ecs_entity_t ent_1 = ecs_entity(world, { .name = "ent_1" });
    ecs_entity_t ent_2 = ecs_entity(world, { .name = "ent_2" });
    ecs_entity_t ent_3 = ecs_entity(world, { .name = "ent_3" });
    ecs_entity_t ent_4 = ecs_entity(world, { .name = "ent_4" });
    ecs_entity_t ent_5 = ecs_entity(world, { .name = "ent_5" });
    ecs_add(world, ent_5, Tag);

    ecs_query_t *q = ecs_query(world, {
        .expr = "$this != ent_2, $this != ent_3",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    bool ent_1_found = false;
    bool ent_4_found = false;
    bool ent_5_found = false;

    ecs_iter_t it = ecs_query_iter(world, q);
    while (ecs_query_next(&it)) {
        for (int i = 0; i < it.count; i ++) {
            test_assert(it.entities[i] != ent_2);
            test_assert(it.entities[i] != ent_3);
            if (it.entities[i] == ent_1) {
                ent_1_found = true;
            }
            if (it.entities[i] == ent_4) {
                ent_4_found = true;
            }
            if (it.entities[i] == ent_5) {
                ent_5_found = true;
            }
        }
    }

    test_bool(ent_1_found, true);
    test_bool(ent_4_found, true);
    test_bool(ent_5_found, true);

    ecs_query_fini(q);

    ecs_fini(world);
}

void BuiltinPredicates_this_2_neq_name(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    ecs_entity_t ent_1 = ecs_entity(world, { .name = "ent_1" });
    ecs_entity_t ent_2 = ecs_entity(world, { .name = "ent_2" });
    ecs_entity_t ent_3 = ecs_entity(world, { .name = "ent_3" });
    ecs_entity_t ent_4 = ecs_entity(world, { .name = "ent_4" });
    ecs_entity_t ent_5 = ecs_entity(world, { .name = "ent_5" });
    ecs_add(world, ent_5, Tag);

    ecs_query_t *q = ecs_query(world, {
        .expr = "$this != \"ent_2\", $this != \"ent_3\"",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    bool ent_1_found = false;
    bool ent_4_found = false;
    bool ent_5_found = false;

    ecs_iter_t it = ecs_query_iter(world, q);
    while (ecs_query_next(&it)) {
        for (int i = 0; i < it.count; i ++) {
            test_assert(it.entities[i] != ent_2);
            test_assert(it.entities[i] != ent_3);
            if (it.entities[i] == ent_1) {
                ent_1_found = true;
            }
            if (it.entities[i] == ent_4) {
                ent_4_found = true;
            }
            if (it.entities[i] == ent_5) {
                ent_5_found = true;
            }
        }
    }

    test_bool(ent_1_found, true);
    test_bool(ent_4_found, true);
    test_bool(ent_5_found, true);

    ecs_query_fini(q);

    ecs_fini(world);
}

void BuiltinPredicates_var_2_neq_id(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    ecs_entity_t ent_1 = ecs_entity(world, { .name = "ent_1" });
    ecs_entity_t ent_2 = ecs_entity(world, { .name = "ent_2" });
    ecs_entity_t ent_3 = ecs_entity(world, { .name = "ent_3" });
    ecs_entity_t ent_4 = ecs_entity(world, { .name = "ent_4" });
    ecs_entity_t ent_5 = ecs_entity(world, { .name = "ent_5" });
    ecs_add(world, ent_5, Tag);

    ecs_query_t *q = ecs_query(world, {
        .expr = "$x != ent_2, $x != ent_3",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    bool ent_1_found = false;
    bool ent_4_found = false;
    bool ent_5_found = false;

    ecs_iter_t it = ecs_query_iter(world, q);
    while (ecs_query_next(&it)) {
        test_uint(it.count, 0);
        ecs_entity_t e = ecs_iter_get_var(&it, x_var);
        test_assert(e != ent_2);
        test_assert(e != ent_3);
        if (e == ent_1) {
            ent_1_found = true;
        }
        if (e == ent_4) {
            ent_4_found = true;
        }
        if (e == ent_5) {
            ent_5_found = true;
        }
    }

    test_bool(ent_1_found, true);
    test_bool(ent_4_found, true);
    test_bool(ent_5_found, true);

    ecs_query_fini(q);

    ecs_fini(world);
}

void BuiltinPredicates_var_2_neq_name(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    ecs_entity_t ent_1 = ecs_entity(world, { .name = "ent_1" });
    ecs_entity_t ent_2 = ecs_entity(world, { .name = "ent_2" });
    ecs_entity_t ent_3 = ecs_entity(world, { .name = "ent_3" });
    ecs_entity_t ent_4 = ecs_entity(world, { .name = "ent_4" });
    ecs_entity_t ent_5 = ecs_entity(world, { .name = "ent_5" });
    ecs_add(world, ent_5, Tag);

    ecs_query_t *q = ecs_query(world, {
        .expr = "$x != \"ent_2\", $x != \"ent_3\"",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    bool ent_1_found = false;
    bool ent_4_found = false;
    bool ent_5_found = false;

    ecs_iter_t it = ecs_query_iter(world, q);
    while (ecs_query_next(&it)) {
        test_uint(it.count, 0);
        ecs_entity_t e = ecs_iter_get_var(&it, x_var);
        test_assert(e != ent_2);
        test_assert(e != ent_3);
        if (e == ent_1) {
            ent_1_found = true;
        }
        if (e == ent_4) {
            ent_4_found = true;
        }
        if (e == ent_5) {
            ent_5_found = true;
        }
    }

    test_bool(ent_1_found, true);
    test_bool(ent_4_found, true);
    test_bool(ent_5_found, true);

    ecs_query_fini(q);

    ecs_fini(world);
}

void BuiltinPredicates_this_2_neq_id_written(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);

    ecs_entity_t ent_1 = ecs_entity(world, { .name = "ent_1" });
    ecs_add(world, ent_1, RelA);
    ecs_entity_t ent_2 = ecs_entity(world, { .name = "ent_2" });
    ecs_add(world, ent_2, RelA);
    ecs_entity_t ent_3 = ecs_entity(world, { .name = "ent_3" });
    ecs_add(world, ent_3, RelA);
    ecs_entity_t ent_4 = ecs_entity(world, { .name = "ent_4" });
    ecs_add(world, ent_4, RelA);
    ecs_entity_t ent_5 = ecs_entity(world, { .name = "ent_5" });
    ecs_add(world, ent_5, RelA);

    ecs_query_t *q = ecs_query(world, {
        .expr = "RelA($this), $this != ent_2, $this != ent_3",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(true, ecs_field_is_set(&it, 0));
        test_uint(false, ecs_field_is_set(&it, 1));
        test_uint(ent_1, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(2, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(true, ecs_field_is_set(&it, 0));
        test_uint(false, ecs_field_is_set(&it, 1));
        test_uint(ent_4, it.entities[0]);
        test_uint(ent_5, it.entities[1]);

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void BuiltinPredicates_this_2_neq_name_written(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);

    ecs_entity_t ent_1 = ecs_entity(world, { .name = "ent_1" });
    ecs_add(world, ent_1, RelA);
    ecs_entity_t ent_2 = ecs_entity(world, { .name = "ent_2" });
    ecs_add(world, ent_2, RelA);
    ecs_entity_t ent_3 = ecs_entity(world, { .name = "ent_3" });
    ecs_add(world, ent_3, RelA);
    ecs_entity_t ent_4 = ecs_entity(world, { .name = "ent_4" });
    ecs_add(world, ent_4, RelA);
    ecs_entity_t ent_5 = ecs_entity(world, { .name = "ent_5" });
    ecs_add(world, ent_5, RelA);

    ecs_query_t *q = ecs_query(world, {
        .expr = "RelA($this), $this != \"ent_2\", $this != \"ent_3\"",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(true, ecs_field_is_set(&it, 0));
        test_uint(false, ecs_field_is_set(&it, 1));
        test_uint(ent_1, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(2, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(true, ecs_field_is_set(&it, 0));
        test_uint(false, ecs_field_is_set(&it, 1));
        test_uint(ent_4, it.entities[0]);
        test_uint(ent_5, it.entities[1]);

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void BuiltinPredicates_var_2_neq_id_written(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);

    ecs_entity_t ent_1 = ecs_entity(world, { .name = "ent_1" });
    ecs_add(world, ent_1, RelA);
    ecs_entity_t ent_2 = ecs_entity(world, { .name = "ent_2" });
    ecs_add(world, ent_2, RelA);
    ecs_entity_t ent_3 = ecs_entity(world, { .name = "ent_3" });
    ecs_add(world, ent_3, RelA);
    ecs_entity_t ent_4 = ecs_entity(world, { .name = "ent_4" });
    ecs_add(world, ent_4, RelA);
    ecs_entity_t ent_5 = ecs_entity(world, { .name = "ent_5" });
    ecs_add(world, ent_5, RelA);

    ecs_query_t *q = ecs_query(world, {
        .expr = "RelA($x), $x != ent_2, $x != ent_3",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(true, ecs_field_is_set(&it, 0));
        test_uint(false, ecs_field_is_set(&it, 1));
        test_uint(ent_1, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(true, ecs_field_is_set(&it, 0));
        test_uint(false, ecs_field_is_set(&it, 1));
        test_uint(ent_4, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(true, ecs_field_is_set(&it, 0));
        test_uint(false, ecs_field_is_set(&it, 1));
        test_uint(ent_5, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void BuiltinPredicates_var_2_neq_name_written(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);

    ecs_entity_t ent_1 = ecs_entity(world, { .name = "ent_1" });
    ecs_add(world, ent_1, RelA);
    ecs_entity_t ent_2 = ecs_entity(world, { .name = "ent_2" });
    ecs_add(world, ent_2, RelA);
    ecs_entity_t ent_3 = ecs_entity(world, { .name = "ent_3" });
    ecs_add(world, ent_3, RelA);
    ecs_entity_t ent_4 = ecs_entity(world, { .name = "ent_4" });
    ecs_add(world, ent_4, RelA);
    ecs_entity_t ent_5 = ecs_entity(world, { .name = "ent_5" });
    ecs_add(world, ent_5, RelA);

    ecs_query_t *q = ecs_query(world, {
        .expr = "RelA($x), $x != \"ent_2\", $x != \"ent_3\"",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(true, ecs_field_is_set(&it, 0));
        test_uint(false, ecs_field_is_set(&it, 1));
        test_uint(ent_1, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(true, ecs_field_is_set(&it, 0));
        test_uint(false, ecs_field_is_set(&it, 1));
        test_uint(ent_4, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(true, ecs_field_is_set(&it, 0));
        test_uint(false, ecs_field_is_set(&it, 1));
        test_uint(ent_5, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void BuiltinPredicates_this_2_or_id(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);

    /* ecs_entity_t ent_1 = */ ecs_entity(world, { .name = "ent_1" });
    ecs_entity_t ent_2 = ecs_entity(world, { .name = "ent_2" });
    ecs_entity_t ent_3 = ecs_entity(world, { .name = "ent_3" });
    /* ecs_entity_t ent_4 = */ ecs_entity(world, { .name = "ent_4" });
    /* ecs_entity_t ent_5 = */ ecs_entity(world, { .name = "ent_5" });

    ecs_query_t *q = ecs_query(world, {
        .expr = "$this == ent_2 || $this == ent_3",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(false, ecs_field_is_set(&it, 0));
        test_uint(ent_2, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(false, ecs_field_is_set(&it, 0));
        test_uint(ent_3, it.entities[0]);

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void BuiltinPredicates_this_3_or_id(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);

    /* ecs_entity_t ent_1 = */ ecs_entity(world, { .name = "ent_1" });
    ecs_entity_t ent_2 = ecs_entity(world, { .name = "ent_2" });
    ecs_entity_t ent_3 = ecs_entity(world, { .name = "ent_3" });
    ecs_entity_t ent_4 = ecs_entity(world, { .name = "ent_4" });
    /* ecs_entity_t ent_5 = */ ecs_entity(world, { .name = "ent_5" });

    ecs_query_t *q = ecs_query(world, {
        .expr = "$this == ent_2 || $this == ent_3 || $this == ent_4",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(false, ecs_field_is_set(&it, 0));
        test_uint(ent_2, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(false, ecs_field_is_set(&it, 0));
        test_uint(ent_3, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(false, ecs_field_is_set(&it, 0));
        test_uint(ent_4, it.entities[0]);

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void BuiltinPredicates_this_2_or_name(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);

    /* ecs_entity_t ent_1 = */ ecs_entity(world, { .name = "ent_1" });
    ecs_entity_t ent_2 = ecs_entity(world, { .name = "ent_2" });
    ecs_entity_t ent_3 = ecs_entity(world, { .name = "ent_3" });
    /* ecs_entity_t ent_4 = */ ecs_entity(world, { .name = "ent_4" });
    /* ecs_entity_t ent_5 = */ ecs_entity(world, { .name = "ent_5" });

    ecs_query_t *q = ecs_query(world, {
        .expr = "$this == \"ent_2\" || $this == \"ent_3\"",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(false, ecs_field_is_set(&it, 0));
        test_uint(ent_2, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(false, ecs_field_is_set(&it, 0));
        test_uint(ent_3, it.entities[0]);

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void BuiltinPredicates_this_3_or_name(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);

    /* ecs_entity_t ent_1 = */ ecs_entity(world, { .name = "ent_1" });
    ecs_entity_t ent_2 = ecs_entity(world, { .name = "ent_2" });
    ecs_entity_t ent_3 = ecs_entity(world, { .name = "ent_3" });
    ecs_entity_t ent_4 = ecs_entity(world, { .name = "ent_4" });
    /* ecs_entity_t ent_5 = */ ecs_entity(world, { .name = "ent_5" });

    ecs_query_t *q = ecs_query(world, {
        .expr = "$this == \"ent_2\" || $this == \"ent_3\" || $this == \"ent_4\"",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(false, ecs_field_is_set(&it, 0));
        test_uint(ent_2, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(false, ecs_field_is_set(&it, 0));
        test_uint(ent_3, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(false, ecs_field_is_set(&it, 0));
        test_uint(ent_4, it.entities[0]);

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void BuiltinPredicates_this_2_or_match(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);

    /* ecs_entity_t ent_1 = */ ecs_entity(world, { .name = "ent_1" });
    ecs_entity_t ent_2 = ecs_entity(world, { .name = "ent_2" });
    ecs_entity_t ent_2_5 = ecs_entity(world, { .name = "ent_25" });
    ecs_entity_t ent_3 = ecs_entity(world, { .name = "ent_3" });
    ecs_entity_t ent_3_5 = ecs_entity(world, { .name = "ent_35" });
    /* ecs_entity_t ent_4 = */ ecs_entity(world, { .name = "ent_4" });
    /* ecs_entity_t ent_5 = */ ecs_entity(world, { .name = "ent_5" });

    ecs_query_t *q = ecs_query(world, {
        .expr = "$this ~= \"ent_2\" || $this ~= \"ent_3\"",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(false, ecs_field_is_set(&it, 0));
        test_uint(ent_2, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(false, ecs_field_is_set(&it, 0));
        test_uint(ent_2_5, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(false, ecs_field_is_set(&it, 0));
        test_uint(ent_3, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(false, ecs_field_is_set(&it, 0));
        test_uint(ent_3_5, it.entities[0]);

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void BuiltinPredicates_this_3_or_match(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);

    /* ecs_entity_t ent_1 = */ ecs_entity(world, { .name = "ent_1" });
    ecs_entity_t ent_2 = ecs_entity(world, { .name = "ent_2" });
    ecs_entity_t ent_2_5 = ecs_entity(world, { .name = "ent_25" });
    ecs_entity_t ent_3 = ecs_entity(world, { .name = "ent_3" });
    ecs_entity_t ent_3_5 = ecs_entity(world, { .name = "ent_35" });
    ecs_entity_t ent_4 = ecs_entity(world, { .name = "ent_4" });
    ecs_entity_t ent_4_5 = ecs_entity(world, { .name = "ent_45" });
    /* ecs_entity_t ent_5 = */ ecs_entity(world, { .name = "ent_5" });

    ecs_query_t *q = ecs_query(world, {
        .expr = "$this ~= \"ent_2\" || $this ~= \"ent_3\" || $this ~= \"ent_4\"",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(false, ecs_field_is_set(&it, 0));
        test_uint(ent_2, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(false, ecs_field_is_set(&it, 0));
        test_uint(ent_2_5, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(false, ecs_field_is_set(&it, 0));
        test_uint(ent_3, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(false, ecs_field_is_set(&it, 0));
        test_uint(ent_3_5, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(false, ecs_field_is_set(&it, 0));
        test_uint(ent_4, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(false, ecs_field_is_set(&it, 0));
        test_uint(ent_4_5, it.entities[0]);

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void BuiltinPredicates_var_2_or_id(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);

    /* ecs_entity_t ent_1 = */ ecs_entity(world, { .name = "ent_1" });
    ecs_entity_t ent_2 = ecs_entity(world, { .name = "ent_2" });
    ecs_entity_t ent_3 = ecs_entity(world, { .name = "ent_3" });
    /* ecs_entity_t ent_4 = */ ecs_entity(world, { .name = "ent_4" });
    /* ecs_entity_t ent_5 = */ ecs_entity(world, { .name = "ent_5" });

    ecs_query_t *q = ecs_query(world, {
        .expr = "$x == ent_2 || $x == ent_3",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);
    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(false, ecs_field_is_set(&it, 0));
        test_uint(ent_2, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(false, ecs_field_is_set(&it, 0));
        test_uint(ent_3, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void BuiltinPredicates_var_2_or_name(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);

    /* ecs_entity_t ent_1 = */ ecs_entity(world, { .name = "ent_1" });
    ecs_entity_t ent_2 = ecs_entity(world, { .name = "ent_2" });
    ecs_entity_t ent_3 = ecs_entity(world, { .name = "ent_3" });
    /* ecs_entity_t ent_4 = */ ecs_entity(world, { .name = "ent_4" });
    /* ecs_entity_t ent_5 = */ ecs_entity(world, { .name = "ent_5" });

    ecs_query_t *q = ecs_query(world, {
        .expr = "$x == \"ent_2\" || $x == \"ent_3\"",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);
    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(false, ecs_field_is_set(&it, 0));
        test_uint(ent_2, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(false, ecs_field_is_set(&it, 0));
        test_uint(ent_3, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void BuiltinPredicates_this_2_or_id_written(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);

    ecs_entity_t ent_1 = ecs_entity(world, { .name = "ent_1" });
    ecs_add(world, ent_1, RelA);
    ecs_entity_t ent_2 = ecs_entity(world, { .name = "ent_2" });
    ecs_add(world, ent_2, RelA);
    ecs_entity_t ent_3 = ecs_entity(world, { .name = "ent_3" });
    ecs_add(world, ent_3, RelA);
    ecs_entity_t ent_4 = ecs_entity(world, { .name = "ent_4" });
    ecs_add(world, ent_4, RelA);
    ecs_entity_t ent_5 = ecs_entity(world, { .name = "ent_5" });
    ecs_add(world, ent_5, RelA);

    ecs_query_t *q = ecs_query(world, {
        .expr = "RelA($this), $this == ent_2 || $this == ent_3",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(true, ecs_field_is_set(&it, 0));
        test_uint(false, ecs_field_is_set(&it, 1));
        test_uint(ent_2, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(true, ecs_field_is_set(&it, 0));
        test_uint(false, ecs_field_is_set(&it, 1));
        test_uint(ent_3, it.entities[0]);

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void BuiltinPredicates_this_3_or_id_written(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);

    ecs_entity_t ent_1 = ecs_entity(world, { .name = "ent_1" });
    ecs_add(world, ent_1, RelA);
    ecs_entity_t ent_2 = ecs_entity(world, { .name = "ent_2" });
    ecs_add(world, ent_2, RelA);
    ecs_entity_t ent_3 = ecs_entity(world, { .name = "ent_3" });
    ecs_add(world, ent_3, RelA);
    ecs_entity_t ent_4 = ecs_entity(world, { .name = "ent_4" });
    ecs_add(world, ent_4, RelA);
    ecs_entity_t ent_5 = ecs_entity(world, { .name = "ent_5" });
    ecs_add(world, ent_5, RelA);

    ecs_query_t *q = ecs_query(world, {
        .expr = "RelA($this), $this == ent_2 || $this == ent_3 || $this == ent_4",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(true, ecs_field_is_set(&it, 0));
        test_uint(false, ecs_field_is_set(&it, 1));
        test_uint(ent_2, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(true, ecs_field_is_set(&it, 0));
        test_uint(false, ecs_field_is_set(&it, 1));
        test_uint(ent_3, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(true, ecs_field_is_set(&it, 0));
        test_uint(false, ecs_field_is_set(&it, 1));
        test_uint(ent_4, it.entities[0]);

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void BuiltinPredicates_this_2_or_name_written(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);

    ecs_entity_t ent_1 = ecs_entity(world, { .name = "ent_1" });
    ecs_add(world, ent_1, RelA);
    ecs_entity_t ent_2 = ecs_entity(world, { .name = "ent_2" });
    ecs_add(world, ent_2, RelA);
    ecs_entity_t ent_3 = ecs_entity(world, { .name = "ent_3" });
    ecs_add(world, ent_3, RelA);
    ecs_entity_t ent_4 = ecs_entity(world, { .name = "ent_4" });
    ecs_add(world, ent_4, RelA);
    ecs_entity_t ent_5 = ecs_entity(world, { .name = "ent_5" });
    ecs_add(world, ent_5, RelA);

    ecs_query_t *q = ecs_query(world, {
        .expr = "RelA($this), $this == \"ent_2\" || $this == \"ent_3\"",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(true, ecs_field_is_set(&it, 0));
        test_uint(false, ecs_field_is_set(&it, 1));
        test_uint(ent_2, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(true, ecs_field_is_set(&it, 0));
        test_uint(false, ecs_field_is_set(&it, 1));
        test_uint(ent_3, it.entities[0]);

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void BuiltinPredicates_var_2_or_id_written(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);

    ecs_entity_t ent_1 = ecs_entity(world, { .name = "ent_1" });
    ecs_add(world, ent_1, RelA);
    ecs_entity_t ent_2 = ecs_entity(world, { .name = "ent_2" });
    ecs_add(world, ent_2, RelA);
    ecs_entity_t ent_3 = ecs_entity(world, { .name = "ent_3" });
    ecs_add(world, ent_3, RelA);
    ecs_entity_t ent_4 = ecs_entity(world, { .name = "ent_4" });
    ecs_add(world, ent_4, RelA);
    ecs_entity_t ent_5 = ecs_entity(world, { .name = "ent_5" });
    ecs_add(world, ent_5, RelA);

    ecs_query_t *q = ecs_query(world, {
        .expr = "RelA($x), $x == ent_2 || $x == ent_3",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(true, ecs_field_is_set(&it, 0));
        test_uint(false, ecs_field_is_set(&it, 1));
        test_uint(ent_2, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(true, ecs_field_is_set(&it, 0));
        test_uint(false, ecs_field_is_set(&it, 1));
        test_uint(ent_3, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void BuiltinPredicates_var_2_or_name_written(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);

    ecs_entity_t ent_1 = ecs_entity(world, { .name = "ent_1" });
    ecs_add(world, ent_1, RelA);
    ecs_entity_t ent_2 = ecs_entity(world, { .name = "ent_2" });
    ecs_add(world, ent_2, RelA);
    ecs_entity_t ent_3 = ecs_entity(world, { .name = "ent_3" });
    ecs_add(world, ent_3, RelA);
    ecs_entity_t ent_4 = ecs_entity(world, { .name = "ent_4" });
    ecs_add(world, ent_4, RelA);
    ecs_entity_t ent_5 = ecs_entity(world, { .name = "ent_5" });
    ecs_add(world, ent_5, RelA);

    ecs_query_t *q = ecs_query(world, {
        .expr = "RelA($x), $x == \"ent_2\" || $x == \"ent_3\"",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(true, ecs_field_is_set(&it, 0));
        test_uint(false, ecs_field_is_set(&it, 1));
        test_uint(ent_2, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(RelA, ecs_field_id(&it, 0));
        test_uint(true, ecs_field_is_set(&it, 0));
        test_uint(false, ecs_field_is_set(&it, 1));
        test_uint(ent_3, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void BuiltinPredicates_this_match_eq(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    ecs_entity_t ent_1 = ecs_entity(world, { .name = "ent_1" });
    /* ecs_entity_t ent_2 = */ ecs_entity(world, { .name = "ent2" });
    ecs_entity_t ent_3 = ecs_entity(world, { .name = "nt_3" });
    ecs_entity_t ent_4 = ecs_entity(world, { .name = "ent_4" });
    ecs_entity_t ent_5 = ecs_entity(world, { .name = "ent5" });
    ecs_add(world, ent_5, Tag);
    ecs_entity_t ent_6 = ecs_entity(world, { .name = "ent_6" });
    ecs_add(world, ent_6, Tag);

    ecs_query_t *q = ecs_query(world, {
        .expr = "$this ~= \"nt_\"",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(false, ecs_field_is_set(&it, 0));
        test_uint(ent_1, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(2, it.count);
        test_uint(false, ecs_field_is_set(&it, 0));
        test_uint(ent_3, it.entities[0]);
        test_uint(ent_4, it.entities[1]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(false, ecs_field_is_set(&it, 0));
        test_uint(ent_6, it.entities[0]);

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void BuiltinPredicates_var_match_eq(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    ecs_entity_t ent_1 = ecs_entity(world, { .name = "ent_1" });
    /* ecs_entity_t ent_2 = */ ecs_entity(world, { .name = "ent2" });
    ecs_entity_t ent_3 = ecs_entity(world, { .name = "nt_3" });
    ecs_entity_t ent_4 = ecs_entity(world, { .name = "ent_4" });
    ecs_entity_t ent_5 = ecs_entity(world, { .name = "ent5" });
    ecs_add(world, ent_5, Tag);
    ecs_entity_t ent_6 = ecs_entity(world, { .name = "ent_6" });
    ecs_add(world, ent_6, Tag);

    ecs_query_t *q = ecs_query(world, {
        .expr = "$x ~= \"nt_\"",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(false, ecs_field_is_set(&it, 0));
        test_uint(ent_1, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(false, ecs_field_is_set(&it, 0));
        test_uint(ent_3, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(false, ecs_field_is_set(&it, 0));
        test_uint(ent_4, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(false, ecs_field_is_set(&it, 0));
        test_uint(ent_6, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void BuiltinPredicates_this_match_eq_written(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);
    ECS_TAG(world, RelA);

    ecs_entity_t ent_1 = ecs_entity(world, { .name = "ent_1" });
    ecs_add(world, ent_1, RelA);
    ecs_entity_t ent_2 = ecs_entity(world, { .name = "ent2" });
    ecs_add(world, ent_2, RelA);
    ecs_entity_t ent_3 = ecs_entity(world, { .name = "nt_3" });
    ecs_add(world, ent_3, RelA);
    ecs_entity_t ent_4 = ecs_entity(world, { .name = "ent_4" });
    ecs_add(world, ent_4, RelA);
    ecs_entity_t ent_5 = ecs_entity(world, { .name = "ent5" });
    ecs_add(world, ent_5, RelA);
    ecs_entity_t ent_6 = ecs_entity(world, { .name = "ent_6" });
    ecs_add(world, ent_6, RelA);
    ecs_add(world, ent_6, Tag);

    ecs_query_t *q = ecs_query(world, {
        .expr = "RelA($this), $this ~= \"nt_\"",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(true, ecs_field_is_set(&it, 0));
        test_uint(false, ecs_field_is_set(&it, 1));
        test_uint(ent_1, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(2, it.count);
        test_uint(true, ecs_field_is_set(&it, 0));
        test_uint(false, ecs_field_is_set(&it, 1));
        test_uint(ent_3, it.entities[0]);
        test_uint(ent_4, it.entities[1]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(true, ecs_field_is_set(&it, 0));
        test_uint(false, ecs_field_is_set(&it, 1));
        test_uint(ent_6, it.entities[0]);

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void BuiltinPredicates_this_match_eq_written_self(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);
    ECS_TAG(world, RelA);

    ecs_entity_t ent_1 = ecs_entity(world, { .name = "ent_1" });
    ecs_add(world, ent_1, RelA);
    ecs_entity_t ent_2 = ecs_entity(world, { .name = "ent2" });
    ecs_add(world, ent_2, RelA);
    ecs_entity_t ent_3 = ecs_entity(world, { .name = "nt_3" });
    ecs_add(world, ent_3, RelA);
    ecs_entity_t ent_4 = ecs_entity(world, { .name = "ent_4" });
    ecs_add(world, ent_4, RelA);
    ecs_entity_t ent_5 = ecs_entity(world, { .name = "ent5" });
    ecs_add(world, ent_5, RelA);
    ecs_entity_t ent_6 = ecs_entity(world, { .name = "ent_6" });
    ecs_add(world, ent_6, RelA);
    ecs_add(world, ent_6, Tag);

    ecs_query_t *r = ecs_query(world, {
        .expr = "RelA($this|self), $this ~= \"nt_\"",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(true, ecs_field_is_set(&it, 0));
        test_uint(false, ecs_field_is_set(&it, 1));
        test_uint(ent_1, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(2, it.count);
        test_uint(true, ecs_field_is_set(&it, 0));
        test_uint(false, ecs_field_is_set(&it, 1));
        test_uint(ent_3, it.entities[0]);
        test_uint(ent_4, it.entities[1]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(true, ecs_field_is_set(&it, 0));
        test_uint(false, ecs_field_is_set(&it, 1));
        test_uint(ent_6, it.entities[0]);

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void BuiltinPredicates_var_match_eq_written(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);
    ECS_TAG(world, RelA);

    ecs_entity_t ent_1 = ecs_entity(world, { .name = "ent_1" });
    ecs_add(world, ent_1, RelA);
    ecs_entity_t ent_2 = ecs_entity(world, { .name = "ent2" });
    ecs_add(world, ent_2, RelA);
    ecs_entity_t ent_3 = ecs_entity(world, { .name = "nt_3" });
    ecs_add(world, ent_3, RelA);
    ecs_entity_t ent_4 = ecs_entity(world, { .name = "ent_4" });
    ecs_add(world, ent_4, RelA);
    ecs_entity_t ent_5 = ecs_entity(world, { .name = "ent5" });
    ecs_add(world, ent_5, RelA);
    ecs_entity_t ent_6 = ecs_entity(world, { .name = "ent_6" });
    ecs_add(world, ent_6, RelA);
    ecs_add(world, ent_6, Tag);

    ecs_query_t *q = ecs_query(world, {
        .expr = "RelA($x), $x ~= \"nt_\"",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(true, ecs_field_is_set(&it, 0));
        test_uint(false, ecs_field_is_set(&it, 1));
        test_uint(ent_1, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(true, ecs_field_is_set(&it, 0));
        test_uint(false, ecs_field_is_set(&it, 1));
        test_uint(ent_3, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(true, ecs_field_is_set(&it, 0));
        test_uint(false, ecs_field_is_set(&it, 1));
        test_uint(ent_4, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(true, ecs_field_is_set(&it, 0));
        test_uint(false, ecs_field_is_set(&it, 1));
        test_uint(ent_6, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void BuiltinPredicates_this_match_neq(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    ecs_entity_t ent_1 = ecs_entity(world, { .name = "ent_1" });
    ecs_entity_t ent_2 = ecs_entity(world, { .name = "ent2" });
    ecs_entity_t ent_3 = ecs_entity(world, { .name = "nt_3" });
    ecs_entity_t ent_4 = ecs_entity(world, { .name = "ent_4" });
    ecs_entity_t ent_5 = ecs_entity(world, { .name = "ent5" });
    ecs_add(world, ent_5, Tag);
    ecs_entity_t ent_6 = ecs_entity(world, { .name = "ent_6" });
    ecs_add(world, ent_6, Tag);

    ecs_query_t *q = ecs_query(world, {
        .expr = "$this ~= \"!nt_\"",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    bool ent_2_found = false;
    bool ent_5_found = false;

    ecs_iter_t it = ecs_query_iter(world, q);
    while (ecs_query_next(&it)) {
        for (int i = 0; i < it.count; i ++) {
            test_assert(it.entities[i] != ent_1);
            test_assert(it.entities[i] != ent_3);
            test_assert(it.entities[i] != ent_4);
            test_assert(it.entities[i] != ent_6);
            if (it.entities[i] == ent_2) {
                ent_2_found = true;
            }
            if (it.entities[i] == ent_5) {
                ent_5_found = true;
            }
        }
    }

    test_bool(ent_2_found, true);
    test_bool(ent_5_found, true);

    ecs_query_fini(q);

    ecs_fini(world);
}

void BuiltinPredicates_var_match_neq(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    ecs_entity_t ent_1 = ecs_entity(world, { .name = "ent_1" });
    ecs_entity_t ent_2 = ecs_entity(world, { .name = "ent2" });
    ecs_entity_t ent_3 = ecs_entity(world, { .name = "nt_3" });
    ecs_entity_t ent_4 = ecs_entity(world, { .name = "ent_4" });
    ecs_entity_t ent_5 = ecs_entity(world, { .name = "ent5" });
    ecs_add(world, ent_5, Tag);
    ecs_entity_t ent_6 = ecs_entity(world, { .name = "ent_6" });
    ecs_add(world, ent_6, Tag);

    ecs_query_t *q = ecs_query(world, {
        .expr = "$x ~= \"!nt_\"",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    bool ent_2_found = false;
    bool ent_5_found = false;

    ecs_iter_t it = ecs_query_iter(world, q);
    while (ecs_query_next(&it)) {
        test_uint(it.count, 0);
        ecs_entity_t e = ecs_iter_get_var(&it, x_var);
        test_assert(e != ent_1);
        test_assert(e != ent_3);
        test_assert(e != ent_4);
        test_assert(e != ent_6);
        if (e == ent_2) {
            ent_2_found = true;
        }
        if (e == ent_5) {
            ent_5_found = true;
        }
    }

    test_bool(ent_2_found, true);
    test_bool(ent_5_found, true);

    ecs_query_fini(q);

    ecs_fini(world);
}

void BuiltinPredicates_this_match_neq_written(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);
    ECS_TAG(world, RelA);

    ecs_entity_t ent_1 = ecs_entity(world, { .name = "ent_1" });
    ecs_add(world, ent_1, RelA);
    ecs_entity_t ent_2 = ecs_entity(world, { .name = "ent2" });
    ecs_add(world, ent_2, RelA);
    ecs_entity_t ent_3 = ecs_entity(world, { .name = "nt_3" });
    ecs_add(world, ent_3, RelA);
    ecs_entity_t ent_4 = ecs_entity(world, { .name = "ent_4" });
    ecs_add(world, ent_4, RelA);
    ecs_entity_t ent_5 = ecs_entity(world, { .name = "ent5" });
    ecs_add(world, ent_5, RelA);
    ecs_entity_t ent_6 = ecs_entity(world, { .name = "ent_6" });
    ecs_add(world, ent_6, RelA);
    ecs_add(world, ent_6, Tag);

    ecs_query_t *q = ecs_query(world, {
        .expr = "RelA($this), $this ~= \"!nt_\"",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    bool ent_2_found = false;
    bool ent_5_found = false;

    ecs_iter_t it = ecs_query_iter(world, q);
    while (ecs_query_next(&it)) {
        for (int i = 0; i < it.count; i ++) {
            test_assert(it.entities[i] != ent_1);
            test_assert(it.entities[i] != ent_3);
            test_assert(it.entities[i] != ent_4);
            test_assert(it.entities[i] != ent_6);
            if (it.entities[i] == ent_2) {
                ent_2_found = true;
            }
            if (it.entities[i] == ent_5) {
                ent_5_found = true;
            }
        }
    }

    test_bool(ent_2_found, true);
    test_bool(ent_5_found, true);

    ecs_query_fini(q);

    ecs_fini(world);
}

void BuiltinPredicates_var_match_neq_written(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);
    ECS_TAG(world, RelA);

    ecs_entity_t ent_1 = ecs_entity(world, { .name = "ent_1" });
    ecs_add(world, ent_1, RelA);
    ecs_entity_t ent_2 = ecs_entity(world, { .name = "ent2" });
    ecs_add(world, ent_2, RelA);
    ecs_entity_t ent_3 = ecs_entity(world, { .name = "nt_3" });
    ecs_add(world, ent_3, RelA);
    ecs_entity_t ent_4 = ecs_entity(world, { .name = "ent_4" });
    ecs_add(world, ent_4, RelA);
    ecs_entity_t ent_5 = ecs_entity(world, { .name = "ent5" });
    ecs_add(world, ent_5, RelA);
    ecs_entity_t ent_6 = ecs_entity(world, { .name = "ent_6" });
    ecs_add(world, ent_6, RelA);
    ecs_add(world, ent_6, Tag);

    ecs_query_t *q = ecs_query(world, {
        .expr = "$x ~= \"!nt_\"",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    bool ent_2_found = false;
    bool ent_5_found = false;

    ecs_iter_t it = ecs_query_iter(world, q);
    while (ecs_query_next(&it)) {
        test_uint(it.count, 0);
        ecs_entity_t e = ecs_iter_get_var(&it, x_var);
        test_assert(e != ent_1);
        test_assert(e != ent_3);
        test_assert(e != ent_4);
        test_assert(e != ent_6);
        if (e == ent_2) {
            ent_2_found = true;
        }
        if (e == ent_5) {
            ent_5_found = true;
        }
    }

    test_bool(ent_2_found, true);
    test_bool(ent_5_found, true);

    ecs_query_fini(q);

    ecs_fini(world);
}

void BuiltinPredicates_this_match_2_neq(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    ecs_entity_t ent_1 = ecs_entity(world, { .name = "ent_1" });
    ecs_entity_t ent_2 = ecs_entity(world, { .name = "ent2" });
    ecs_entity_t ent_3 = ecs_entity(world, { .name = "nt_3" });
    ecs_entity_t ent_4 = ecs_entity(world, { .name = "ent_4" });
    ecs_entity_t ent_5 = ecs_entity(world, { .name = "ent5" });
    ecs_add(world, ent_5, Tag);
    ecs_entity_t ent_6 = ecs_entity(world, { .name = "ent_6" });
    ecs_add(world, ent_6, Tag);

    ecs_query_t *q = ecs_query(world, {
        .expr = "$this ~= \"!nt_\", $this ~= \"!_3\"",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    bool ent_2_found = false;
    bool ent_5_found = false;

    ecs_iter_t it = ecs_query_iter(world, q);
    while (ecs_query_next(&it)) {
        for (int i = 0; i < it.count; i ++) {
            test_assert(it.entities[i] != ent_1);
            test_assert(it.entities[i] != ent_3);
            test_assert(it.entities[i] != ent_4);
            test_assert(it.entities[i] != ent_6);
            if (it.entities[i] == ent_2) {
                ent_2_found = true;
            }
            if (it.entities[i] == ent_5) {
                ent_5_found = true;
            }
        }
    }

    test_bool(ent_2_found, true);
    test_bool(ent_5_found, true);

    ecs_query_fini(q);

    ecs_fini(world);
}

void BuiltinPredicates_var_match_2_neq(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    ecs_entity_t ent_1 = ecs_entity(world, { .name = "ent_1" });
    ecs_entity_t ent_2 = ecs_entity(world, { .name = "ent2" });
    ecs_entity_t ent_3 = ecs_entity(world, { .name = "nt_3" });
    ecs_entity_t ent_4 = ecs_entity(world, { .name = "ent_4" });
    ecs_entity_t ent_5 = ecs_entity(world, { .name = "ent5" });
    ecs_add(world, ent_5, Tag);
    ecs_entity_t ent_6 = ecs_entity(world, { .name = "ent_6" });
    ecs_add(world, ent_6, Tag);

    ecs_query_t *q = ecs_query(world, {
        .expr = "$x ~= \"!nt_\", $x ~= \"!_3\"",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    bool ent_2_found = false;
    bool ent_5_found = false;

    ecs_iter_t it = ecs_query_iter(world, q);
    while (ecs_query_next(&it)) {
        ecs_entity_t e = ecs_iter_get_var(&it, x_var);
        test_assert(e != ent_1);
        test_assert(e != ent_3);
        test_assert(e != ent_4);
        test_assert(e != ent_6);
        if (e == ent_2) {
            ent_2_found = true;
        }
        if (e == ent_5) {
            ent_5_found = true;
        }
    }

    test_bool(ent_2_found, true);
    test_bool(ent_5_found, true);

    ecs_query_fini(q);

    ecs_fini(world);
}

void BuiltinPredicates_this_match_2_neq_written(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);
    ECS_TAG(world, RelA);

    ecs_entity_t ent_1 = ecs_entity(world, { .name = "ent_1" });
    ecs_add(world, ent_1, RelA);
    ecs_entity_t ent_2 = ecs_entity(world, { .name = "ent2" });
    ecs_add(world, ent_2, RelA);
    ecs_entity_t ent_3 = ecs_entity(world, { .name = "nt_3" });
    ecs_add(world, ent_3, RelA);
    ecs_entity_t ent_4 = ecs_entity(world, { .name = "ent_4" });
    ecs_add(world, ent_4, RelA);
    ecs_entity_t ent_5 = ecs_entity(world, { .name = "ent5" });
    ecs_add(world, ent_5, RelA);
    ecs_entity_t ent_6 = ecs_entity(world, { .name = "ent_6" });
    ecs_add(world, ent_6, RelA);
    ecs_add(world, ent_6, Tag);

    ecs_query_t *q = ecs_query(world, {
        .expr = "RelA($this), $this ~= \"!nt_\", $this ~= \"!_3\"",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    bool ent_2_found = false;
    bool ent_5_found = false;

    ecs_iter_t it = ecs_query_iter(world, q);
    while (ecs_query_next(&it)) {
        for (int i = 0; i < it.count; i ++) {
            test_assert(it.entities[i] != ent_1);
            test_assert(it.entities[i] != ent_3);
            test_assert(it.entities[i] != ent_4);
            test_assert(it.entities[i] != ent_6);
            if (it.entities[i] == ent_2) {
                ent_2_found = true;
            }
            if (it.entities[i] == ent_5) {
                ent_5_found = true;
            }
        }
    }

    test_bool(ent_2_found, true);
    test_bool(ent_5_found, true);

    ecs_query_fini(q);

    ecs_fini(world);
}

void BuiltinPredicates_var_match_2_neq_written(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);
    ECS_TAG(world, RelA);

    ecs_entity_t ent_1 = ecs_entity(world, { .name = "ent_1" });
    ecs_add(world, ent_1, RelA);
    ecs_entity_t ent_2 = ecs_entity(world, { .name = "ent2" });
    ecs_add(world, ent_2, RelA);
    ecs_entity_t ent_3 = ecs_entity(world, { .name = "nt_3" });
    ecs_add(world, ent_3, RelA);
    ecs_entity_t ent_4 = ecs_entity(world, { .name = "ent_4" });
    ecs_add(world, ent_4, RelA);
    ecs_entity_t ent_5 = ecs_entity(world, { .name = "ent5" });
    ecs_add(world, ent_5, RelA);
    ecs_entity_t ent_6 = ecs_entity(world, { .name = "ent_6" });
    ecs_add(world, ent_6, RelA);
    ecs_add(world, ent_6, Tag);

    ecs_query_t *q = ecs_query(world, {
        .expr = "RelA($x), $x ~= \"!nt_\", $x ~= \"!_3\"",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    bool ent_2_found = false;
    bool ent_5_found = false;

    ecs_iter_t it = ecs_query_iter(world, q);
    while (ecs_query_next(&it)) {
        ecs_entity_t e = ecs_iter_get_var(&it, x_var);
        test_assert(e != ent_1);
        test_assert(e != ent_3);
        test_assert(e != ent_4);
        test_assert(e != ent_6);
        if (e == ent_2) {
            ent_2_found = true;
        }
        if (e == ent_5) {
            ent_5_found = true;
        }
    }

    test_bool(ent_2_found, true);
    test_bool(ent_5_found, true);

    ecs_query_fini(q);

    ecs_fini(world);
}

void BuiltinPredicates_this_match_2_or(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    ecs_entity_t ent_1 = ecs_entity(world, { .name = "ent_1" });
    /* ecs_entity_t ent_2 = */ ecs_entity(world, { .name = "ent2" });
    ecs_entity_t ent_3 = ecs_entity(world, { .name = "nt_3" });
    /* ecs_entity_t ent_4 = */ ecs_entity(world, { .name = "ent_4" });
    ecs_entity_t ent_5 = ecs_entity(world, { .name = "ent5" });
    ecs_add(world, ent_5, Tag);
    ecs_entity_t ent_6 = ecs_entity(world, { .name = "ent_6" });
    ecs_add(world, ent_6, Tag);

    ecs_query_t *q = ecs_query(world, {
        .expr = "$this ~= \"1\" || $this ~= \"_3\"",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(false, ecs_field_is_set(&it, 0));
        test_uint(ent_1, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(false, ecs_field_is_set(&it, 0));
        test_uint(ent_3, it.entities[0]);

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void BuiltinPredicates_this_match_2_or_written(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);
    ECS_TAG(world, RelA);

    ecs_entity_t ent_1 = ecs_entity(world, { .name = "ent_1" });
    ecs_add(world, ent_1, RelA);
    ecs_entity_t ent_2 = ecs_entity(world, { .name = "ent2" });
    ecs_add(world, ent_2, RelA);
    ecs_entity_t ent_3 = ecs_entity(world, { .name = "nt_3" });
    ecs_add(world, ent_3, RelA);
    ecs_entity_t ent_4 = ecs_entity(world, { .name = "ent_4" });
    ecs_add(world, ent_4, RelA);
    ecs_entity_t ent_5 = ecs_entity(world, { .name = "ent5" });
    ecs_add(world, ent_5, RelA);
    ecs_entity_t ent_6 = ecs_entity(world, { .name = "ent_6" });
    ecs_add(world, ent_6, RelA);
    ecs_add(world, ent_6, Tag);

    ecs_query_t *q = ecs_query(world, {
        .expr = "RelA($this), $this ~= \"1\" || $this ~= \"_3\"",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(true, ecs_field_is_set(&it, 0));
        test_uint(false, ecs_field_is_set(&it, 1));
        test_uint(ent_1, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(true, ecs_field_is_set(&it, 0));
        test_uint(false, ecs_field_is_set(&it, 1));
        test_uint(ent_3, it.entities[0]);

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void BuiltinPredicates_this_match_3_or(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    ecs_entity_t ent_1 = ecs_entity(world, { .name = "ent_1" });
    /* ecs_entity_t ent_2 = */ ecs_entity(world, { .name = "ent2" });
    ecs_entity_t ent_3 = ecs_entity(world, { .name = "nt_3" });
    /* ecs_entity_t ent_4 = */ ecs_entity(world, { .name = "ent_4" });
    ecs_entity_t ent_5 = ecs_entity(world, { .name = "ent5" });
    ecs_add(world, ent_5, Tag);
    ecs_entity_t ent_6 = ecs_entity(world, { .name = "ent_6" });
    ecs_add(world, ent_6, Tag);

    ecs_query_t *q = ecs_query(world, {
        .expr = "$this ~= \"1\" || $this ~= \"_3\" || $this ~= \"nt_6\"",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(false, ecs_field_is_set(&it, 0));
        test_uint(ent_1, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(false, ecs_field_is_set(&it, 0));
        test_uint(ent_3, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(false, ecs_field_is_set(&it, 0));
        test_uint(ent_6, it.entities[0]);

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void BuiltinPredicates_this_match_3_or_written(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);
    ECS_TAG(world, RelA);

    ecs_entity_t ent_1 = ecs_entity(world, { .name = "ent_1" });
    ecs_add(world, ent_1, RelA);
    ecs_entity_t ent_2 = ecs_entity(world, { .name = "ent2" });
    ecs_add(world, ent_2, RelA);
    ecs_entity_t ent_3 = ecs_entity(world, { .name = "nt_3" });
    ecs_add(world, ent_3, RelA);
    ecs_entity_t ent_4 = ecs_entity(world, { .name = "ent_4" });
    ecs_add(world, ent_4, RelA);
    ecs_entity_t ent_5 = ecs_entity(world, { .name = "ent5" });
    ecs_add(world, ent_5, RelA);
    ecs_entity_t ent_6 = ecs_entity(world, { .name = "ent_6" });
    ecs_add(world, ent_6, RelA);
    ecs_add(world, ent_6, Tag);

    ecs_query_t *q = ecs_query(world, {
        .expr = "RelA($this), $this ~= \"1\" || $this ~= \"_3\" || $this ~= \"nt_6\"",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(true, ecs_field_is_set(&it, 0));
        test_uint(false, ecs_field_is_set(&it, 1));
        test_uint(ent_1, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(true, ecs_field_is_set(&it, 0));
        test_uint(false, ecs_field_is_set(&it, 1));
        test_uint(ent_3, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(true, ecs_field_is_set(&it, 0));
        test_uint(false, ecs_field_is_set(&it, 1));
        test_uint(ent_6, it.entities[0]);

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void BuiltinPredicates_unresolved_by_name(void) {
    ecs_world_t *world = ecs_mini();

    ecs_query_t *q = ecs_query(world, {
        .expr = "$this == ent",
        .cache_kind = cache_kind,
        .flags = EcsQueryAllowUnresolvedByName
    });

    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_entity_t ent = ecs_entity(world, { .name = "ent" });

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(false, ecs_field_is_set(&it, 0));
        test_uint(ent, it.entities[0]);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void BuiltinPredicates_var_eq_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);

    ecs_entity_t tgt = ecs_new(world);
    ecs_entity_t e = ecs_new_w_pair(world, Rel, tgt);

    ecs_query_t *q = ecs_query(world, {
        .expr = "$x == *, (Rel, $x)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(e, it.entities[0]);
        test_uint(tgt, ecs_iter_get_var(&it, x_var));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void BuiltinPredicates_var_eq_any(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);

    ecs_entity_t tgt = ecs_new(world);
    ecs_entity_t e = ecs_new_w_pair(world, Rel, tgt);

    ecs_query_t *q = ecs_query(world, {
        .expr = "$x == _, (Rel, $x)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(e, it.entities[0]);
        test_uint(tgt, ecs_iter_get_var(&it, x_var));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void BuiltinPredicates_var_eq_wildcard_after_write(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);

    ecs_entity_t tgt = ecs_new(world);
    ecs_entity_t e = ecs_new_w_pair(world, Rel, tgt);

    ecs_query_t *q = ecs_query(world, {
        .expr = "(Rel, $x), $x == *",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(e, it.entities[0]);
        test_uint(tgt, ecs_iter_get_var(&it, x_var));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void BuiltinPredicates_var_eq_any_after_write(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);

    ecs_entity_t tgt = ecs_new(world);
    ecs_entity_t e = ecs_new_w_pair(world, Rel, tgt);

    ecs_query_t *q = ecs_query(world, {
        .expr = "(Rel, $x), $x == _",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(e, it.entities[0]);
        test_uint(tgt, ecs_iter_get_var(&it, x_var));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void BuiltinPredicates_var_eq_after_var_0_src(void) {
    ecs_world_t *world = ecs_mini();

    ecs_query_t *q = ecs_query(world, {
        .expr = "$x(), $x == flecs",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(EcsFlecs, ecs_iter_get_var(&it, x_var));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void BuiltinPredicates_2_or_w_eq_lookup_var(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);
    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);
    ECS_TAG(world, Hello);

    ecs_entity_t e1 = ecs_new_w(world, Foo);
    ecs_entity_t e2 = ecs_new_w(world, Foo);
    ecs_entity_t e3 = ecs_new_w(world, Foo);
    ecs_new_w(world, Foo);

    ecs_entity_t child1 = ecs_entity(world, { .name = "hello" });
    ecs_add_pair(world, child1, EcsChildOf, e1);
    ecs_add(world, child1, Bar);

    ecs_entity_t child2 = ecs_entity(world, { .name = "world" });
    ecs_add_pair(world, child2, EcsChildOf, e2);
    ecs_add(world, child2, Bar);

    ecs_entity_t child3_1 = ecs_entity(world, { .name = "hello" });
    ecs_add_pair(world, child3_1, EcsChildOf, e3);
    ecs_add(world, child3_1, Bar);

    ecs_entity_t child3_2 = ecs_entity(world, { .name = "world" });
    ecs_add_pair(world, child3_2, EcsChildOf, e3);
    ecs_add(world, child3_2, Bar);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Foo($this), $x == $this.hello || $x == $this.world, Bar($x)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    int hello_var = ecs_query_find_var(q, "this.hello");
    test_assert(hello_var != -1);

    int world_var = ecs_query_find_var(q, "this.world");
    test_assert(world_var != -1);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(child1, ecs_iter_get_var(&it, x_var));
    test_uint(child1, ecs_iter_get_var(&it, hello_var));
    test_uint(EcsWildcard, ecs_iter_get_var(&it, world_var));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e2, it.entities[0]);
    test_uint(child2, ecs_iter_get_var(&it, x_var));
    test_uint(EcsWildcard, ecs_iter_get_var(&it, hello_var));
    test_uint(child2, ecs_iter_get_var(&it, world_var));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e3, it.entities[0]);
    test_uint(child3_1, ecs_iter_get_var(&it, x_var));
    test_uint(child3_1, ecs_iter_get_var(&it, hello_var));
    test_uint(EcsWildcard, ecs_iter_get_var(&it, world_var));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void BuiltinPredicates_3_or_w_eq_lookup_var(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);
    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);
    ECS_TAG(world, Hello);

    ecs_entity_t e1 = ecs_new_w(world, Foo);
    ecs_entity_t e2 = ecs_new_w(world, Foo);
    ecs_entity_t e3 = ecs_new_w(world, Foo);
    ecs_entity_t e4 = ecs_new_w(world, Foo);
    ecs_new_w(world, Foo);

    ecs_entity_t child1 = ecs_entity(world, { .name = "hello" });
    ecs_add_pair(world, child1, EcsChildOf, e1);
    ecs_add(world, child1, Bar);

    ecs_entity_t child2 = ecs_entity(world, { .name = "world" });
    ecs_add_pair(world, child2, EcsChildOf, e2);
    ecs_add(world, child2, Bar);

    ecs_entity_t child3_1 = ecs_entity(world, { .name = "hello" });
    ecs_add_pair(world, child3_1, EcsChildOf, e3);
    ecs_add(world, child3_1, Bar);

    ecs_entity_t child3_2 = ecs_entity(world, { .name = "world" });
    ecs_add_pair(world, child3_2, EcsChildOf, e3);
    ecs_add(world, child3_2, Bar);

    ecs_entity_t child4 = ecs_entity(world, { .name = "zoo" });
    ecs_add_pair(world, child4, EcsChildOf, e4);
    ecs_add(world, child4, Bar);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Foo($this), $x == $this.hello || $x == $this.world || $x == $this.zoo, Bar($x)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    int hello_var = ecs_query_find_var(q, "this.hello");
    test_assert(hello_var != -1);

    int world_var = ecs_query_find_var(q, "this.world");
    test_assert(world_var != -1);

    int zoo_var = ecs_query_find_var(q, "this.zoo");
    test_assert(zoo_var != -1);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(child1, ecs_iter_get_var(&it, x_var));
    test_uint(child1, ecs_iter_get_var(&it, hello_var));
    test_uint(EcsWildcard, ecs_iter_get_var(&it, world_var));
    test_uint(EcsWildcard, ecs_iter_get_var(&it, zoo_var));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e2, it.entities[0]);
    test_uint(child2, ecs_iter_get_var(&it, x_var));
    test_uint(EcsWildcard, ecs_iter_get_var(&it, hello_var));
    test_uint(child2, ecs_iter_get_var(&it, world_var));
    test_uint(EcsWildcard, ecs_iter_get_var(&it, zoo_var));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e3, it.entities[0]);
    test_uint(child3_1, ecs_iter_get_var(&it, x_var));
    test_uint(child3_1, ecs_iter_get_var(&it, hello_var));
    test_uint(EcsWildcard, ecs_iter_get_var(&it, world_var));
    test_uint(EcsWildcard, ecs_iter_get_var(&it, zoo_var));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e4, it.entities[0]);
    test_uint(child4, ecs_iter_get_var(&it, x_var));
    test_uint(EcsWildcard, ecs_iter_get_var(&it, hello_var));
    test_uint(EcsWildcard, ecs_iter_get_var(&it, world_var));
    test_uint(child4, ecs_iter_get_var(&it, zoo_var));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void BuiltinPredicates_2_or_w_eq_this(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Rel);
    ECS_TAG(world, Bar);

    ecs_entity_t e1 = ecs_new(world);
    ecs_add_pair(world, e1, Rel, e1);

    ecs_entity_t e2 = ecs_new(world);
    ecs_add_pair(world, e2, Rel, e1);

    ecs_entity_t e3 = ecs_new(world);
    ecs_add_pair(world, e3, Rel, Rel);

    ecs_query_t *r = ecs_query(world, {
        .expr = "Rel($this, $x), $x == $this || $x == Rel",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);
    
    int x_var = ecs_query_find_var(r, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(e1, it.entities[0]);
        test_uint(ecs_pair(Rel, e1), ecs_field_id(&it, 0));
        test_uint(true, ecs_field_is_set(&it, 0));
        test_uint(e1, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(e3, it.entities[0]);
        test_uint(ecs_pair(Rel, Rel), ecs_field_id(&it, 0));
        test_uint(true, ecs_field_is_set(&it, 0));
        test_uint(Rel, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void BuiltinPredicates_eq_variable(void) {
    ecs_world_t *world = ecs_mini();

    ecs_log_set_level(-4);
    ecs_query_t *q = ecs_query(world, {
        .expr = "$this == $",
        .cache_kind = cache_kind
    });

    test_assert(q == NULL);

    ecs_fini(world);
}

void BuiltinPredicates_eq_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ecs_query_t *q = ecs_query(world, {
        .expr = "$this == *",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void BuiltinPredicates_eq_any(void) {
    ecs_world_t *world = ecs_mini();

    ecs_query_t *q = ecs_query(world, {
        .expr = "$this == _",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void BuiltinPredicates_neq_variable(void) {
    ecs_world_t *world = ecs_mini();

    ecs_log_set_level(-4);
    ecs_query_t *q = ecs_query(world, {
        .expr = "$this != $",
        .cache_kind = cache_kind
    });

    test_assert(q == NULL);

    ecs_fini(world);
}

void BuiltinPredicates_neq_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ecs_query_t *q = ecs_query(world, {
        .expr = "$this != *",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_query_fini(q);

    ecs_fini(world);
}

void BuiltinPredicates_neq_any(void) {
    ecs_world_t *world = ecs_mini();

    ecs_query_t *q = ecs_query(world, {
        .expr = "$this != _",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_query_fini(q);

    ecs_fini(world);
}

void BuiltinPredicates_match_variable(void) {
    ecs_world_t *world = ecs_mini();

    ecs_query_t *q = ecs_query(world, {
        .expr = "$this ~= \"$\"",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_uint(1, it.count);
    test_uint(EcsVariable, it.entities[0]);
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void BuiltinPredicates_match_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ecs_query_t *q = ecs_query(world, {
        .expr = "$this ~= \"*\"",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_uint(1, it.count);
    test_uint(EcsWildcard, it.entities[0]);
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void BuiltinPredicates_match_any(void) {
    ecs_world_t *world = ecs_mini();

    ecs_query_t *q = ecs_query(world, {
        .expr = "$this ~= \"_\"",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_uint(1, it.count);
    test_uint(EcsAny, it.entities[0]);
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}
