#include <query.h>

static ecs_query_cache_kind_t cache_kind = EcsQueryCacheDefault;

void NonFragmentingChildOf_setup(void) {
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

void NonFragmentingChildOf_0_src_childof_parent(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t p = ecs_new(world);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { ecs_childof(p), .src.id = EcsIsEntity }
        },
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);
    
    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(ecs_childof(p), ecs_field_id(&it, 0));
    test_bool(false, ecs_field_is_set(&it, 0));
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_0_src_childof_0(void) {
    ecs_world_t *world = ecs_mini();

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { ecs_childof(0), .src.id = EcsIsEntity }
        },
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);
    
    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(ecs_childof(0), ecs_field_id(&it, 0));
    test_bool(false, ecs_field_is_set(&it, 0));
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_fixed_src_childof_0(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t p = ecs_new(world);
    ecs_entity_t c1 = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_entity_t c2 = ecs_new_w_pair(world, EcsChildOf, p);

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {
                { ecs_childof(0), .src.id = p }
            },
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(ecs_childof(EcsWildcard), ecs_field_id(&it, 0));
        test_uint(p, ecs_field_src(&it, 0));
        test_bool(false, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_query_next(&it));
        ecs_query_fini(q);
    }

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {
                { ecs_childof(0), .src.id = c1 }
            },
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
        ecs_query_fini(q);
    }

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {
                { ecs_childof(0), .src.id = c2 }
            },
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
        ecs_query_fini(q);
    }

    ecs_fini(world);
}

void NonFragmentingChildOf_fixed_src_not_childof_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t p = ecs_new(world);
    ecs_entity_t c1 = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_entity_t c2 = ecs_new_w_pair(world, EcsChildOf, p);

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {
                { ecs_childof(EcsWildcard), .src.id = p, .oper = EcsNot }
            },
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(ecs_childof(EcsWildcard), ecs_field_id(&it, 0));
        test_uint(p, ecs_field_src(&it, 0));
        test_bool(false, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_query_next(&it));
        ecs_query_fini(q);
    }

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {
                { ecs_childof(EcsWildcard), .src.id = c1, .oper = EcsNot }
            },
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
        ecs_query_fini(q);
    }

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {
                { ecs_childof(EcsWildcard), .src.id = c2, .oper = EcsNot }
            },
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
        ecs_query_fini(q);
    }

    ecs_fini(world);
}

void NonFragmentingChildOf_fixed_src_childof_parent(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t p_1 = ecs_new(world);
    ecs_entity_t p_2 = ecs_new(world);
    ecs_entity_t c_1 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t c_2 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t c_3 = ecs_insert(world, ecs_value(EcsParent, {p_2}));
    ecs_entity_t c_4 = ecs_new_w_pair(world, EcsChildOf, p_2);
    ecs_entity_t c_5 = ecs_new(world);

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {
                { ecs_childof(p_2), .src.id = c_5 }
            },
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {
                { ecs_childof(p_2), .src.id = c_1 }
            },
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {
                { ecs_childof(p_1), .src.id = c_1 }
            },
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(c_1, ecs_field_src(&it, 0));
        test_uint(ecs_pair(EcsChildOf, p_1), ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {
                { ecs_childof(p_1), .src.id = c_2 }
            },
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(c_2, ecs_field_src(&it, 0));
        test_uint(ecs_pair(EcsChildOf, p_1), ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {
                { ecs_childof(p_2), .src.id = c_3 }
            },
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(c_3, ecs_field_src(&it, 0));
        test_uint(ecs_pair(EcsChildOf, p_2), ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {
                { ecs_childof(p_2), .src.id = c_4 }
            },
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(c_4, ecs_field_src(&it, 0));
        test_uint(ecs_pair(EcsChildOf, p_2), ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    ecs_fini(world);
}

void NonFragmentingChildOf_fixed_src_childof_parent_w_tag(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_entity_t p_1 = ecs_new(world);
    ecs_entity_t p_2 = ecs_new(world);
    ecs_entity_t c_1 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t c_2 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t c_3 = ecs_insert(world, ecs_value(EcsParent, {p_2}));
    ecs_entity_t c_4 = ecs_new_w_pair(world, EcsChildOf, p_2);
    ecs_entity_t c_5 = ecs_insert(world, ecs_value(EcsParent, {p_1}));

    ecs_add(world, c_1, Foo);
    ecs_add(world, c_2, Foo);
    ecs_add(world, c_3, Foo);
    ecs_add(world, c_4, Foo);

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {
                { ecs_childof(p_2), .src.id = c_1 },
                { Foo, .src.id = c_1 }
            },
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {
                { ecs_childof(p_1), .src.id = c_5 },
                { Foo, .src.id = c_5 }
            },
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {
                { ecs_childof(p_1), .src.id = c_1 },
                { Foo, .src.id = c_1 }
            },
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(c_1, ecs_field_src(&it, 0));
        test_uint(ecs_pair(EcsChildOf, p_1), ecs_field_id(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {
                { ecs_childof(p_1), .src.id = c_2 },
                { Foo, .src.id = c_2 }
            },
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(c_2, ecs_field_src(&it, 0));
        test_uint(ecs_pair(EcsChildOf, p_1), ecs_field_id(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {
                { ecs_childof(p_2), .src.id = c_3 },
                { Foo, .src.id = c_3 }
            },
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(c_3, ecs_field_src(&it, 0));
        test_uint(ecs_pair(EcsChildOf, p_2), ecs_field_id(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {
                { ecs_childof(p_2), .src.id = c_4 },
                { Foo, .src.id = c_4 }
            },
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(c_4, ecs_field_src(&it, 0));
        test_uint(ecs_pair(EcsChildOf, p_2), ecs_field_id(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    ecs_fini(world);
}

void NonFragmentingChildOf_fixed_src_childof_parent_w_component(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t p_1 = ecs_new(world);
    ecs_entity_t p_2 = ecs_new(world);
    ecs_entity_t c_1 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t c_2 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t c_3 = ecs_insert(world, ecs_value(EcsParent, {p_2}));
    ecs_entity_t c_4 = ecs_new_w_pair(world, EcsChildOf, p_2);
    ecs_entity_t c_5 = ecs_insert(world, ecs_value(EcsParent, {p_1}));

    ecs_set(world, c_1, Position, {1, 2});
    ecs_set(world, c_2, Position, {2, 3});
    ecs_set(world, c_3, Position, {3, 4});
    ecs_set(world, c_4, Position, {4, 5});

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {
                { ecs_childof(p_2), .src.id = c_1 },
                { ecs_id(Position), .src.id = c_1 }
            },
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {
                { ecs_childof(p_1), .src.id = c_5 },
                { ecs_id(Position), .src.id = c_5 }
            },
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {
                { ecs_childof(p_1), .src.id = c_1 },
                { ecs_id(Position), .src.id = c_1 }
            },
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(c_1, ecs_field_src(&it, 0));
        test_uint(ecs_pair(EcsChildOf, p_1), ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        {
            Position *p = ecs_field(&it, Position, 1);
            test_assert(p != NULL);
            test_int(p->x, 1);
            test_int(p->y, 2);
        }
        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {
                { ecs_childof(p_1), .src.id = c_2 },
                { ecs_id(Position), .src.id = c_2 }
            },
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(c_2, ecs_field_src(&it, 0));
        test_uint(ecs_pair(EcsChildOf, p_1), ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        {
            Position *p = ecs_field(&it, Position, 1);
            test_assert(p != NULL);
            test_int(p->x, 2);
            test_int(p->y, 3);
        }
        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {
                { ecs_childof(p_2), .src.id = c_3 },
                { ecs_id(Position), .src.id = c_3 }
            },
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(c_3, ecs_field_src(&it, 0));
        test_uint(ecs_pair(EcsChildOf, p_2), ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        {
            Position *p = ecs_field(&it, Position, 1);
            test_assert(p != NULL);
            test_int(p->x, 3);
            test_int(p->y, 4);
        }
        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {
                { ecs_childof(p_2), .src.id = c_4 },
                { ecs_id(Position), .src.id = c_4 }
            },
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(c_4, ecs_field_src(&it, 0));
        test_uint(ecs_pair(EcsChildOf, p_2), ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        {
            Position *p = ecs_field(&it, Position, 1);
            test_assert(p != NULL);
            test_int(p->x, 4);
            test_int(p->y, 5);
        }
        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    ecs_fini(world);
}

void NonFragmentingChildOf_fixed_src_childof_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t p_1 = ecs_new(world);
    ecs_entity_t p_2 = ecs_new(world);
    ecs_entity_t c_1 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t c_2 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t c_3 = ecs_insert(world, ecs_value(EcsParent, {p_2}));
    ecs_entity_t c_4 = ecs_new_w_pair(world, EcsChildOf, p_2);
    ecs_entity_t c_5 = ecs_new(world);

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {
                { ecs_childof(EcsWildcard), .src.id = c_5 }
            },
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {
                { ecs_childof(EcsWildcard), .src.id = c_1 }
            },
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(c_1, ecs_field_src(&it, 0));
        test_uint(ecs_pair(EcsChildOf, p_1), ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {
                { ecs_childof(EcsWildcard), .src.id = c_2 }
            },
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(c_2, ecs_field_src(&it, 0));
        test_uint(ecs_pair(EcsChildOf, p_1), ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {
                { ecs_childof(EcsWildcard), .src.id = c_3 }
            },
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(c_3, ecs_field_src(&it, 0));
        test_uint(ecs_pair(EcsChildOf, p_2), ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {
                { ecs_childof(EcsWildcard), .src.id = c_4 }
            },
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(c_4, ecs_field_src(&it, 0));
        test_uint(ecs_pair(EcsChildOf, p_2), ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    ecs_fini(world);
}

void NonFragmentingChildOf_fixed_src_childof_wildcard_w_tag(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_entity_t p_1 = ecs_new(world);
    ecs_entity_t p_2 = ecs_new(world);
    ecs_entity_t c_1 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t c_2 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t c_3 = ecs_insert(world, ecs_value(EcsParent, {p_2}));
    ecs_entity_t c_4 = ecs_new_w_pair(world, EcsChildOf, p_2);
    ecs_entity_t c_5 = ecs_insert(world, ecs_value(EcsParent, {p_1}));

    ecs_add(world, c_1, Foo);
    ecs_add(world, c_2, Foo);
    ecs_add(world, c_3, Foo);
    ecs_add(world, c_4, Foo);

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {
                { ecs_childof(EcsWildcard), .src.id = c_5 },
                { Foo, .src.id = c_5 }
            },
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {
                { ecs_childof(EcsWildcard), .src.id = c_1 },
                { Foo, .src.id = c_1 }
            },
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(c_1, ecs_field_src(&it, 0));
        test_uint(ecs_pair(EcsChildOf, p_1), ecs_field_id(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {
                { ecs_childof(EcsWildcard), .src.id = c_2 },
                { Foo, .src.id = c_2 }
            },
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(c_2, ecs_field_src(&it, 0));
        test_uint(ecs_pair(EcsChildOf, p_1), ecs_field_id(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {
                { ecs_childof(EcsWildcard), .src.id = c_3 },
                { Foo, .src.id = c_3 }
            },
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(c_3, ecs_field_src(&it, 0));
        test_uint(ecs_pair(EcsChildOf, p_2), ecs_field_id(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {
                { ecs_childof(EcsWildcard), .src.id = c_4 },
                { Foo, .src.id = c_4 }
            },
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(c_4, ecs_field_src(&it, 0));
        test_uint(ecs_pair(EcsChildOf, p_2), ecs_field_id(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    ecs_fini(world);
}

void NonFragmentingChildOf_fixed_src_childof_wildcard_w_component(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t p_1 = ecs_new(world);
    ecs_entity_t p_2 = ecs_new(world);
    ecs_entity_t c_1 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t c_2 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t c_3 = ecs_insert(world, ecs_value(EcsParent, {p_2}));
    ecs_entity_t c_4 = ecs_new_w_pair(world, EcsChildOf, p_2);
    ecs_entity_t c_5 = ecs_insert(world, ecs_value(EcsParent, {p_1}));

    ecs_set(world, c_1, Position, {1, 2});
    ecs_set(world, c_2, Position, {2, 3});
    ecs_set(world, c_3, Position, {3, 4});
    ecs_set(world, c_4, Position, {4, 5});

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {
                { ecs_childof(EcsWildcard), .src.id = c_5 },
                { ecs_id(Position), .src.id = c_5 }
            },
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {
                { ecs_childof(EcsWildcard), .src.id = c_1 },
                { ecs_id(Position), .src.id = c_1 }
            },
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(c_1, ecs_field_src(&it, 0));
        test_uint(ecs_pair(EcsChildOf, p_1), ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        {
            Position *p = ecs_field(&it, Position, 1);
            test_assert(p != NULL);
            test_int(p->x, 1);
            test_int(p->y, 2);
        }
        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {
                { ecs_childof(EcsWildcard), .src.id = c_2 },
                { ecs_id(Position), .src.id = c_2 }
            },
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(c_2, ecs_field_src(&it, 0));
        test_uint(ecs_pair(EcsChildOf, p_1), ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        {
            Position *p = ecs_field(&it, Position, 1);
            test_assert(p != NULL);
            test_int(p->x, 2);
            test_int(p->y, 3);
        }
        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {
                { ecs_childof(EcsWildcard), .src.id = c_3 },
                { ecs_id(Position), .src.id = c_3 }
            },
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(c_3, ecs_field_src(&it, 0));
        test_uint(ecs_pair(EcsChildOf, p_2), ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        {
            Position *p = ecs_field(&it, Position, 1);
            test_assert(p != NULL);
            test_int(p->x, 3);
            test_int(p->y, 4);
        }
        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {
                { ecs_childof(EcsWildcard), .src.id = c_4 },
                { ecs_id(Position), .src.id = c_4 }
            },
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(c_4, ecs_field_src(&it, 0));
        test_uint(ecs_pair(EcsChildOf, p_2), ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        {
            Position *p = ecs_field(&it, Position, 1);
            test_assert(p != NULL);
            test_int(p->x, 4);
            test_int(p->y, 5);
        }
        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    ecs_fini(world);
}

void NonFragmentingChildOf_fixed_src_childof_any(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t p_1 = ecs_new(world);
    ecs_entity_t p_2 = ecs_new(world);
    ecs_entity_t c_1 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t c_2 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t c_3 = ecs_insert(world, ecs_value(EcsParent, {p_2}));
    ecs_entity_t c_4 = ecs_new_w_pair(world, EcsChildOf, p_2);
    ecs_entity_t c_5 = ecs_new(world);

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {
                { ecs_childof(EcsAny), .src.id = c_5 }
            },
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {
                { ecs_childof(EcsAny), .src.id = c_1 }
            },
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(c_1, ecs_field_src(&it, 0));
        test_uint(ecs_pair(EcsChildOf, EcsWildcard), ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {
                { ecs_childof(EcsAny), .src.id = c_2 }
            },
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(c_2, ecs_field_src(&it, 0));
        test_uint(ecs_pair(EcsChildOf, EcsWildcard), ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {
                { ecs_childof(EcsAny), .src.id = c_3 }
            },
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(c_3, ecs_field_src(&it, 0));
        test_uint(ecs_pair(EcsChildOf, EcsWildcard), ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {
                { ecs_childof(EcsAny), .src.id = c_4 }
            },
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(c_4, ecs_field_src(&it, 0));
        test_uint(ecs_pair(EcsChildOf, EcsWildcard), ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    ecs_fini(world);
}

void NonFragmentingChildOf_fixed_src_childof_var(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t p_1 = ecs_new(world);
    ecs_entity_t p_2 = ecs_new(world);
    ecs_entity_t c_1 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t c_2 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t c_3 = ecs_insert(world, ecs_value(EcsParent, {p_2}));
    ecs_entity_t c_4 = ecs_new_w_pair(world, EcsChildOf, p_2);
    ecs_entity_t c_5 = ecs_new(world);

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {
                { .first.id = EcsChildOf, .second.name = "$x", .src.id = c_5 },
            },
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        int x_var = ecs_query_find_var(q, "x");
        test_assert(x_var != -1);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {
                { .first.id = EcsChildOf, .second.name = "$x", .src.id = c_1 },
            },
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        int x_var = ecs_query_find_var(q, "x");
        test_assert(x_var != -1);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(c_1, ecs_field_src(&it, 0));
        test_uint(ecs_pair(EcsChildOf, p_1), ecs_field_id(&it, 0));
        test_uint(p_1, ecs_iter_get_var(&it, x_var));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {
                { .first.id = EcsChildOf, .second.name = "$x", .src.id = c_2 },
            },
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        int x_var = ecs_query_find_var(q, "x");
        test_assert(x_var != -1);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(c_2, ecs_field_src(&it, 0));
        test_uint(ecs_pair(EcsChildOf, p_1), ecs_field_id(&it, 0));
        test_uint(p_1, ecs_iter_get_var(&it, x_var));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {
                { .first.id = EcsChildOf, .second.name = "$x", .src.id = c_3 },
            },
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        int x_var = ecs_query_find_var(q, "x");
        test_assert(x_var != -1);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(c_3, ecs_field_src(&it, 0));
        test_uint(ecs_pair(EcsChildOf, p_2), ecs_field_id(&it, 0));
        test_uint(p_2, ecs_iter_get_var(&it, x_var));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {
                { .first.id = EcsChildOf, .second.name = "$x", .src.id = c_4 },
            },
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        int x_var = ecs_query_find_var(q, "x");
        test_assert(x_var != -1);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(c_4, ecs_field_src(&it, 0));
        test_uint(ecs_pair(EcsChildOf, p_2), ecs_field_id(&it, 0));
        test_uint(p_2, ecs_iter_get_var(&it, x_var));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    ecs_fini(world);
}

void NonFragmentingChildOf_fixed_src_childof_set_var(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t p_1 = ecs_new(world);
    ecs_entity_t p_2 = ecs_new(world);
    ecs_entity_t c_1 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t c_2 = ecs_insert(world, ecs_value(EcsParent, {p_2}));

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {
                { .first.id = EcsChildOf, .second.name = "$x", .src.id = c_1 },
            },
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        int x_var = ecs_query_find_var(q, "x");
        test_assert(x_var != -1);

        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, x_var, p_2);
        test_bool(false, ecs_query_next(&it));
        ecs_query_fini(q);
    }

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {
                { .first.id = EcsChildOf, .second.name = "$x", .src.id = c_1 },
            },
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        int x_var = ecs_query_find_var(q, "x");
        test_assert(x_var != -1);

        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, x_var, p_1);
        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(c_1, ecs_field_src(&it, 0));
        test_uint(ecs_pair(EcsChildOf, p_1), ecs_field_id(&it, 0));
        test_uint(p_1, ecs_iter_get_var(&it, x_var));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {
                { .first.id = EcsChildOf, .second.name = "$x", .src.id = c_2 },
            },
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        int x_var = ecs_query_find_var(q, "x");
        test_assert(x_var != -1);

        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, x_var, p_1);
        test_bool(false, ecs_query_next(&it));
        ecs_query_fini(q);
    }

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {
                { .first.id = EcsChildOf, .second.name = "$x", .src.id = c_2 },
            },
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        int x_var = ecs_query_find_var(q, "x");
        test_assert(x_var != -1);

        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, x_var, p_2);
        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(c_2, ecs_field_src(&it, 0));
        test_uint(ecs_pair(EcsChildOf, p_2), ecs_field_id(&it, 0));
        test_uint(p_2, ecs_iter_get_var(&it, x_var));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    ecs_fini(world);
}

void NonFragmentingChildOf_this_src_childof_parent_only_childof(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_entity_t parent = ecs_new(world);

    ecs_query_t *q = ecs_query(world, {
       .terms = {{ ecs_childof(parent) }, { Foo }},
       .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_entity_t child = ecs_new_w(world, Foo);
    ecs_add_pair(world, child, EcsChildOf, parent);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(child, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(ecs_childof(parent), ecs_field_id(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    test_uint(Foo, ecs_field_id(&it, 1));
    test_bool(true, ecs_field_is_set(&it, 1));
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_src_written_childof_parent_only_childof(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_entity_t parent = ecs_new(world);

    ecs_query_t *q = ecs_query(world, {
       .terms = {{ Foo }, { ecs_childof(parent) }},
       .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_entity_t child = ecs_new_w(world, Foo);
    ecs_add_pair(world, child, EcsChildOf, parent);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(child, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    test_uint(ecs_childof(parent), ecs_field_id(&it, 1));
    test_bool(true, ecs_field_is_set(&it, 1));
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_any_src_childof_parent(void) {
    ecs_world_t *world = ecs_mini();
    
    ecs_entity_t parent = ecs_entity(world, { .name = "parent" });
    ecs_add_id(world, parent, EcsOrderedChildren);

    ecs_query_t *q = ecs_query(world, {
        .expr = "ChildOf(_, parent)"
    });

    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_entity_t c = ecs_insert(world, ecs_value(EcsParent, {parent}));

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(EcsWildcard, ecs_field_src(&it, 0));
        test_uint(ecs_childof(parent), ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_delete(world, c);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    c = ecs_new_w_pair(world, EcsChildOf, parent);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(EcsWildcard, ecs_field_src(&it, 0));
        test_uint(ecs_childof(parent), ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_any_src_childof_var(void) {
    ecs_world_t *world = ecs_mini();
    
    ecs_entity_t parent = ecs_new(world);
    ecs_set_name(world, parent, "parent");

    ecs_query_t *q = ecs_query(world, {
        .expr = "ChildOf(_, $x)"
    });

    test_assert(q != NULL);
    
    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    ecs_entity_t c = ecs_insert(world, ecs_value(EcsParent, {parent}));

    bool p_found = false;
    {
        ecs_iter_t it = ecs_query_iter(world, q);
        while (ecs_query_next(&it)) {
            ecs_entity_t p = ecs_iter_get_var(&it, x_var);
            test_assert(p != 0);

            test_int(0, it.count);
            test_uint(EcsWildcard, ecs_field_src(&it, 0));
            test_uint(ecs_childof(p), ecs_field_id(&it, 0));
            test_bool(true, ecs_field_is_set(&it, 0));

            if (p == parent) {
                p_found = true;
            }
        }
    }

    test_bool(true, p_found);

    ecs_delete(world, c);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        while (ecs_query_next(&it)) {
            ecs_entity_t p = ecs_iter_get_var(&it, x_var);
            test_assert(p != 0);

            test_int(0, it.count);
            test_uint(EcsWildcard, ecs_field_src(&it, 0));
            test_uint(ecs_childof(p), ecs_field_id(&it, 0));
            test_bool(true, ecs_field_is_set(&it, 0));

            test_assert (p != parent);
        }
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_any_src_childof_var_written(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    
    ecs_entity_t parent = ecs_entity(world, { .name = "parent" });
    ecs_add_id(world, parent, EcsOrderedChildren);
    ecs_add(world, parent, Foo);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Foo, ChildOf(_, $this)"
    });

    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_entity_t c = ecs_insert(world, ecs_value(EcsParent, {parent}));

    {
        ecs_iter_t it = ecs_query_iter(world, q);

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(parent, it.entities[0]);
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(EcsWildcard, ecs_field_src(&it, 1));
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_childof(parent), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_delete(world, c);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    c = ecs_new_w_pair(world, EcsChildOf, parent);

    {
        ecs_iter_t it = ecs_query_iter(world, q);

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(parent, it.entities[0]);
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(EcsWildcard, ecs_field_src(&it, 1));
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_childof(parent), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_any_src_childof_var_set(void) {
    ecs_world_t *world = ecs_mini();
    
    ecs_entity_t parent = ecs_entity(world, { .name = "parent" });
    ecs_add_id(world, parent, EcsOrderedChildren);

    ecs_query_t *q = ecs_query(world, {
        .expr = "ChildOf(_, $x)"
    });

    test_assert(q != NULL);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, x_var, parent);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_entity_t c = ecs_insert(world, ecs_value(EcsParent, {parent}));

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, x_var, parent);

        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(EcsWildcard, ecs_field_src(&it, 0));
        test_uint(ecs_childof(parent), ecs_field_id(&it, 0));
        test_uint(parent, ecs_iter_get_var(&it, x_var));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_delete(world, c);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, x_var, parent);
        test_bool(false, ecs_query_next(&it));
    }

    c = ecs_new_w_pair(world, EcsChildOf, parent);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, x_var, parent);

        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(EcsWildcard, ecs_field_src(&it, 0));
        test_uint(ecs_childof(parent), ecs_field_id(&it, 0));
        test_uint(parent, ecs_iter_get_var(&it, x_var));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_any_src_childof_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    
    ecs_entity_t parent = ecs_entity(world, { .name = "parent" });
    ecs_add_id(world, parent, EcsOrderedChildren);
    ecs_add(world, parent, Foo);

    ecs_query_t *q = ecs_query(world, {
        .expr = "ChildOf(_, *)"
    });

    test_assert(q != NULL);

    ecs_entity_t internals = ecs_lookup(world, "flecs.core.internals");
    test_assert(internals != 0);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(EcsWildcard, ecs_field_src(&it, 0));
        test_uint(ecs_childof(EcsFlecs), ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(EcsWildcard, ecs_field_src(&it, 0));
        test_uint(ecs_childof(internals), ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(EcsWildcard, ecs_field_src(&it, 0));
        test_uint(ecs_childof(EcsFlecsCore), ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_entity_t c = ecs_insert(world, ecs_value(EcsParent, {parent}));

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(EcsWildcard, ecs_field_src(&it, 0));
        test_uint(ecs_childof(parent), ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(EcsWildcard, ecs_field_src(&it, 0));
        test_uint(ecs_childof(EcsFlecs), ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(EcsWildcard, ecs_field_src(&it, 0));
        test_uint(ecs_childof(internals), ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(EcsWildcard, ecs_field_src(&it, 0));
        test_uint(ecs_childof(EcsFlecsCore), ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_delete(world, c);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(EcsWildcard, ecs_field_src(&it, 0));
        test_uint(ecs_childof(EcsFlecs), ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(EcsWildcard, ecs_field_src(&it, 0));
        test_uint(ecs_childof(internals), ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(EcsWildcard, ecs_field_src(&it, 0));
        test_uint(ecs_childof(EcsFlecsCore), ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_src_childof_0(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e1 = ecs_new(world);
    ecs_add_id(world, e1, EcsOrderedChildren);
    ecs_insert(world, ecs_value(EcsParent, {e1}));

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { ecs_childof(0) }
        },
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(EcsFlecs, it.entities[0]);
    test_uint(ecs_childof(0), ecs_field_id(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(ecs_childof(0), ecs_field_id(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 0));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_src_not_childof_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t p = ecs_new(world);
    /* ecs_entity_t c1 = */ ecs_insert(world, ecs_value(EcsParent, {p}));
    /* ecs_entity_t c2 = */ ecs_new_w_pair(world, EcsChildOf, p);
    ecs_entity_t c3 = ecs_new(world);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { ecs_childof(EcsWildcard), .oper = EcsNot }
        },
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(2, it.count);
    test_uint(p, it.entities[0]);
    test_uint(c3, it.entities[1]);
    test_uint(ecs_childof(0), ecs_field_id(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(EcsFlecs, it.entities[0]);
    test_uint(ecs_childof(0), ecs_field_id(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 0));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_src_not_childof_any(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t p = ecs_new(world);
    /* ecs_entity_t c1 = */ ecs_insert(world, ecs_value(EcsParent, {p}));
    /* ecs_entity_t c2 = */ ecs_new_w_pair(world, EcsChildOf, p);
    ecs_entity_t c3 = ecs_new(world);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { ecs_childof(EcsAny), .oper = EcsNot }
        },
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(2, it.count);
    test_uint(p, it.entities[0]);
    test_uint(c3, it.entities[1]);
    test_uint(ecs_childof(0), ecs_field_id(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(EcsFlecs, it.entities[0]);
    test_uint(ecs_childof(0), ecs_field_id(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 0));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_src_childof_parent(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_entity_t p_1 = ecs_new(world);
    ecs_entity_t p_2 = ecs_new(world);
    ecs_entity_t p_3 = ecs_new(world);

    ecs_entity_t c1 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t c2 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t c3 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t c4 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t c5 = ecs_new_w_pair(world, EcsChildOf, p_1);

    ecs_entity_t c6 = ecs_new_w_pair(world, EcsChildOf, p_2);
    ecs_entity_t c7 = ecs_insert(world, ecs_value(EcsParent, {p_2}));

    ecs_entity_t c8 = ecs_new_w_pair(world, EcsChildOf, p_3);
    ecs_entity_t c9 = ecs_new_w_pair(world, EcsChildOf, p_3);
    ecs_entity_t c10 = ecs_new_w_pair(world, EcsChildOf, p_3);
    ecs_entity_t c11 = ecs_new_w_pair(world, EcsChildOf, p_3);

    ecs_add(world, c2, Foo);
    ecs_add(world, c3, Foo);
    ecs_add(world, c4, Bar);

    ecs_add(world, c9, Foo);
    ecs_add(world, c10, Foo);
    ecs_add(world, c11, Bar);

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {
                { ecs_childof(p_1) }
            },
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(5, it.count);
        test_uint(c1, it.entities[0]);
        test_uint(c2, it.entities[1]);
        test_uint(c3, it.entities[2]);
        test_uint(c4, it.entities[3]);
        test_uint(c5, it.entities[4]);
        test_uint(ecs_childof(p_1), ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {
                { ecs_childof(p_2) }
            },
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(2, it.count);
        test_uint(c6, it.entities[0]);
        test_uint(c7, it.entities[1]);
        test_uint(ecs_childof(p_2), ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {
                { ecs_childof(p_3) }
            },
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c8, it.entities[0]);
        test_uint(ecs_childof(p_3), ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_int(2, it.count);
        test_uint(c9, it.entities[0]);
        test_uint(c10, it.entities[1]);
        test_uint(ecs_childof(p_3), ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c11, it.entities[0]);
        test_uint(ecs_childof(p_3), ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));

        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    ecs_fini(world);
}

void NonFragmentingChildOf_this_src_childof_parent_w_tag(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_entity_t p_1 = ecs_new(world);
    ecs_entity_t p_2 = ecs_new(world);
    ecs_entity_t p_3 = ecs_new(world);

    /* ecs_entity_t c1 = */ ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t c2 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t c3 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t c4 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    /* ecs_entity_t c5 = */ ecs_new_w_pair(world, EcsChildOf, p_1);

    /* ecs_entity_t c6 = */ ecs_new_w_pair(world, EcsChildOf, p_2);
    /* ecs_entity_t c7 = */ ecs_insert(world, ecs_value(EcsParent, {p_2}));

    /* ecs_entity_t c8 = */ ecs_new_w_pair(world, EcsChildOf, p_3);
    ecs_entity_t c9 = ecs_new_w_pair(world, EcsChildOf, p_3);
    ecs_entity_t c10 = ecs_new_w_pair(world, EcsChildOf, p_3);
    ecs_entity_t c11 = ecs_new_w_pair(world, EcsChildOf, p_3);

    ecs_add(world, c2, Foo);
    ecs_add(world, c3, Foo);
    ecs_add(world, c4, Bar);

    ecs_add(world, c9, Foo);
    ecs_add(world, c10, Foo);
    ecs_add(world, c11, Foo);
    ecs_add(world, c11, Bar);

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {
                { ecs_childof(p_1) },
                { Foo }
            },
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c2, it.entities[0]);
        test_uint(ecs_childof(p_1), ecs_field_id(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c3, it.entities[0]);
        test_uint(ecs_childof(p_1), ecs_field_id(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));

        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {
                { ecs_childof(p_2) },
                { Foo }
            },
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {
                { ecs_childof(p_3) },
                { Foo }
            },
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(2, it.count);
        test_uint(c9, it.entities[0]);
        test_uint(c10, it.entities[1]);
        test_uint(ecs_childof(p_3), ecs_field_id(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c11, it.entities[0]);
        test_uint(ecs_childof(p_3), ecs_field_id(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));

        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    ecs_fini(world);
}

void NonFragmentingChildOf_this_src_childof_parent_w_tag_single_child(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_entity_t p_1 = ecs_new(world);
    ecs_entity_t p_2 = ecs_new(world);
    ecs_entity_t p_3 = ecs_new(world);

    /* ecs_entity_t c1 = */ ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t c2 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t c3 = ecs_new_w_pair(world, EcsChildOf, p_1);
    ecs_entity_t c4 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    /* ecs_entity_t c5 = */ ecs_new_w_pair(world, EcsChildOf, p_1);

    /* ecs_entity_t c6 = */ ecs_new_w_pair(world, EcsChildOf, p_2);
    ecs_entity_t c7 = ecs_insert(world, ecs_value(EcsParent, {p_2}));

    /* ecs_entity_t c8 = */ ecs_new_w_pair(world, EcsChildOf, p_3);
    ecs_entity_t c9 = ecs_new_w_pair(world, EcsChildOf, p_3);
    ecs_entity_t c10 = ecs_new_w_pair(world, EcsChildOf, p_3);
    ecs_entity_t c11 = ecs_new_w_pair(world, EcsChildOf, p_3);

    ecs_add(world, c2, Foo);
    ecs_add(world, c3, Foo);
    ecs_add(world, c4, Bar);

    ecs_add(world, c7, Foo);

    ecs_add(world, c9, Foo);
    ecs_add(world, c10, Foo);
    ecs_add(world, c11, Foo);
    ecs_add(world, c11, Bar);

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {
                { ecs_childof(p_1) },
                { Foo }
            },
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c2, it.entities[0]);
        test_uint(ecs_childof(p_1), ecs_field_id(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c3, it.entities[0]);
        test_uint(ecs_childof(p_1), ecs_field_id(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));

        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {
                { ecs_childof(p_2) },
                { Foo }
            },
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c7, it.entities[0]);
        test_uint(ecs_childof(p_2), ecs_field_id(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {
                { ecs_childof(p_3) },
                { Foo }
            },
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(2, it.count);
        test_uint(c9, it.entities[0]);
        test_uint(c10, it.entities[1]);
        test_uint(ecs_childof(p_3), ecs_field_id(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c11, it.entities[0]);
        test_uint(ecs_childof(p_3), ecs_field_id(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));

        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    ecs_fini(world);
}

void NonFragmentingChildOf_this_src_childof_parent_w_tag_single_child_table(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_entity_t p_1 = ecs_new(world);
    ecs_entity_t p_2 = ecs_new(world);
    ecs_entity_t p_3 = ecs_new(world);

    /* ecs_entity_t c1 = */ ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t c2 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t c3 = ecs_new_w_pair(world, EcsChildOf, p_1);
    ecs_entity_t c4 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    /* ecs_entity_t c5 = */ ecs_new_w_pair(world, EcsChildOf, p_1);

    /* ecs_entity_t c6 = */ ecs_new_w_pair(world, EcsChildOf, p_2);
    /* ecs_entity_t c7 = */ ecs_insert(world, ecs_value(EcsParent, {p_2}));

    /* ecs_entity_t c8 = */ ecs_new_w_pair(world, EcsChildOf, p_3);
    ecs_entity_t c9 = ecs_new_w_pair(world, EcsChildOf, p_3);
    ecs_entity_t c10 = ecs_new_w_pair(world, EcsChildOf, p_3);
    ecs_entity_t c11 = ecs_new_w_pair(world, EcsChildOf, p_3);

    ecs_add(world, c2, Foo);
    ecs_add(world, c3, Foo);
    ecs_add(world, c4, Bar);

    ecs_add(world, c9, Foo);
    ecs_add(world, c10, Foo);
    ecs_add(world, c11, Foo);
    ecs_add(world, c11, Bar);

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {
                { ecs_childof(p_1) },
                { Foo }
            },
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c2, it.entities[0]);
        test_uint(ecs_childof(p_1), ecs_field_id(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c3, it.entities[0]);
        test_uint(ecs_childof(p_1), ecs_field_id(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));

        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {
                { ecs_childof(p_2) },
                { Foo }
            },
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {
                { ecs_childof(p_3) },
                { Foo }
            },
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(2, it.count);
        test_uint(c9, it.entities[0]);
        test_uint(c10, it.entities[1]);
        test_uint(ecs_childof(p_3), ecs_field_id(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c11, it.entities[0]);
        test_uint(ecs_childof(p_3), ecs_field_id(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));

        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    ecs_fini(world);
}

void NonFragmentingChildOf_this_src_childof_parent_w_component(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Bar);

    ecs_entity_t p_1 = ecs_new(world);
    ecs_entity_t p_2 = ecs_new(world);
    ecs_entity_t p_3 = ecs_new(world);

    /* ecs_entity_t c1 = */ ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t c2 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t c3 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t c4 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    /* ecs_entity_t c5 = */ ecs_new_w_pair(world, EcsChildOf, p_1);

    /* ecs_entity_t c6 = */ ecs_new_w_pair(world, EcsChildOf, p_2);
    /* ecs_entity_t c7 = */ ecs_insert(world, ecs_value(EcsParent, {p_2}));

    /* ecs_entity_t c8 = */ ecs_new_w_pair(world, EcsChildOf, p_3);
    ecs_entity_t c9 = ecs_new_w_pair(world, EcsChildOf, p_3);
    ecs_entity_t c10 = ecs_new_w_pair(world, EcsChildOf, p_3);
    ecs_entity_t c11 = ecs_new_w_pair(world, EcsChildOf, p_3);

    ecs_set(world, c2, Position, {1, 2});
    ecs_set(world, c3, Position, {2, 3});
    ecs_add(world, c4, Bar);

    ecs_set(world, c9, Position, {3, 4});
    ecs_set(world, c10, Position, {4, 5});
    ecs_set(world, c11, Position, {5, 6});
    ecs_add(world, c11, Bar);

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {
                { ecs_childof(p_1) },
                { ecs_id(Position) }
            },
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c2, it.entities[0]);
        test_uint(ecs_childof(p_1), ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        {
            Position *p = ecs_field(&it, Position, 1);
            test_assert(p != NULL);
            test_int(p->x, 1);
            test_int(p->y, 2);
        }

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c3, it.entities[0]);
        test_uint(ecs_childof(p_1), ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        {
            Position *p = ecs_field(&it, Position, 1);
            test_assert(p != NULL);
            test_int(p->x, 2);
            test_int(p->y, 3);
        }

        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {
                { ecs_childof(p_2) },
                { ecs_id(Position) }
            },
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {
                { ecs_childof(p_3) },
                { ecs_id(Position) }
            },
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(2, it.count);
        test_uint(c9, it.entities[0]);
        test_uint(c10, it.entities[1]);
        test_uint(ecs_childof(p_3), ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        {
            Position *p = ecs_field(&it, Position, 1);
            test_assert(p != NULL);
            test_int(p[0].x, 3); test_int(p[0].y, 4);
            test_int(p[1].x, 4); test_int(p[1].y, 5);
        }

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c11, it.entities[0]);
        test_uint(ecs_childof(p_3), ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        {
            Position *p = ecs_field(&it, Position, 1);
            test_assert(p != NULL);
            test_int(p->x, 5);
            test_int(p->y, 6);
        }

        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    ecs_fini(world);
}

void NonFragmentingChildOf_this_src_childof_parent_w_component_single_child(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Bar);

    ecs_entity_t p_1 = ecs_new(world);
    ecs_entity_t p_2 = ecs_new(world);
    ecs_entity_t p_3 = ecs_new(world);

    /* ecs_entity_t c1 = */ ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t c2 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t c3 = ecs_new_w_pair(world, EcsChildOf, p_1);
    ecs_entity_t c4 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    /* ecs_entity_t c5 = */ ecs_new_w_pair(world, EcsChildOf, p_1);

    /* ecs_entity_t c6 = */ ecs_new_w_pair(world, EcsChildOf, p_2);
    ecs_entity_t c7 = ecs_insert(world, ecs_value(EcsParent, {p_2}));

    /* ecs_entity_t c8 = */ ecs_new_w_pair(world, EcsChildOf, p_3);
    ecs_entity_t c9 = ecs_new_w_pair(world, EcsChildOf, p_3);
    ecs_entity_t c10 = ecs_new_w_pair(world, EcsChildOf, p_3);
    ecs_entity_t c11 = ecs_new_w_pair(world, EcsChildOf, p_3);

    ecs_set(world, c2, Position, {1, 2});
    ecs_set(world, c3, Position, {2, 3});
    ecs_add(world, c4, Bar);

    ecs_set(world, c7, Position, {2, 3});

    ecs_set(world, c9, Position, {3, 4});
    ecs_set(world, c10, Position, {4, 5});
    ecs_set(world, c11, Position, {5, 6});
    ecs_add(world, c11, Bar);

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {
                { ecs_childof(p_1) },
                { ecs_id(Position) }
            },
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c2, it.entities[0]);
        test_uint(ecs_childof(p_1), ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        {
            Position *p = ecs_field(&it, Position, 1);
            test_assert(p != NULL);
            test_int(p->x, 1);
            test_int(p->y, 2);
        }

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c3, it.entities[0]);
        test_uint(ecs_childof(p_1), ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        {
            Position *p = ecs_field(&it, Position, 1);
            test_assert(p != NULL);
            test_int(p->x, 2);
            test_int(p->y, 3);
        }

        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {
                { ecs_childof(p_2) },
                { ecs_id(Position) }
            },
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c7, it.entities[0]);
        test_uint(ecs_childof(p_2), ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        {
            Position *p = ecs_field(&it, Position, 1);
            test_assert(p != NULL);
            test_int(p[0].x, 2); test_int(p[0].y, 3);
        }

        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {
                { ecs_childof(p_3) },
                { ecs_id(Position) }
            },
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(2, it.count);
        test_uint(c9, it.entities[0]);
        test_uint(c10, it.entities[1]);
        test_uint(ecs_childof(p_3), ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        {
            Position *p = ecs_field(&it, Position, 1);
            test_assert(p != NULL);
            test_int(p[0].x, 3); test_int(p[0].y, 4);
            test_int(p[1].x, 4); test_int(p[1].y, 5);
        }

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c11, it.entities[0]);
        test_uint(ecs_childof(p_3), ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        {
            Position *p = ecs_field(&it, Position, 1);
            test_assert(p != NULL);
            test_int(p->x, 5);
            test_int(p->y, 6);
        }

        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    ecs_fini(world);
}

void NonFragmentingChildOf_this_src_childof_parent_w_component_single_child_table(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Bar);

    ecs_entity_t p_1 = ecs_new(world);
    ecs_entity_t p_2 = ecs_new(world);
    ecs_entity_t p_3 = ecs_new(world);

    /* ecs_entity_t c1 = */ ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t c2 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t c3 = ecs_new_w_pair(world, EcsChildOf, p_1);
    ecs_entity_t c4 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    /* ecs_entity_t c5 = */ ecs_new_w_pair(world, EcsChildOf, p_1);

    /* ecs_entity_t c6 = */ ecs_new_w_pair(world, EcsChildOf, p_2);
    /* ecs_entity_t c7 = */ ecs_insert(world, ecs_value(EcsParent, {p_2}));

    /* ecs_entity_t c8 = */ ecs_new_w_pair(world, EcsChildOf, p_3);
    ecs_entity_t c9 = ecs_new_w_pair(world, EcsChildOf, p_3);
    ecs_entity_t c10 = ecs_new_w_pair(world, EcsChildOf, p_3);
    ecs_entity_t c11 = ecs_new_w_pair(world, EcsChildOf, p_3);

    ecs_set(world, c2, Position, {1, 2});
    ecs_set(world, c3, Position, {2, 3});
    ecs_add(world, c4, Bar);

    ecs_set(world, c9, Position, {3, 4});
    ecs_set(world, c10, Position, {4, 5});
    ecs_set(world, c11, Position, {5, 6});
    ecs_add(world, c11, Bar);

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {
                { ecs_childof(p_1) },
                { ecs_id(Position) }
            },
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c2, it.entities[0]);
        test_uint(ecs_childof(p_1), ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        {
            Position *p = ecs_field(&it, Position, 1);
            test_assert(p != NULL);
            test_int(p->x, 1);
            test_int(p->y, 2);
        }

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c3, it.entities[0]);
        test_uint(ecs_childof(p_1), ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        {
            Position *p = ecs_field(&it, Position, 1);
            test_assert(p != NULL);
            test_int(p->x, 2);
            test_int(p->y, 3);
        }

        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {
                { ecs_childof(p_2) },
                { ecs_id(Position) }
            },
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {
                { ecs_childof(p_3) },
                { ecs_id(Position) }
            },
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(2, it.count);
        test_uint(c9, it.entities[0]);
        test_uint(c10, it.entities[1]);
        test_uint(ecs_childof(p_3), ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        {
            Position *p = ecs_field(&it, Position, 1);
            test_assert(p != NULL);
            test_int(p[0].x, 3); test_int(p[0].y, 4);
            test_int(p[1].x, 4); test_int(p[1].y, 5);
        }

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c11, it.entities[0]);
        test_uint(ecs_childof(p_3), ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        {
            Position *p = ecs_field(&it, Position, 1);
            test_assert(p != NULL);
            test_int(p->x, 5);
            test_int(p->y, 6);
        }

        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    ecs_fini(world);
}

void NonFragmentingChildOf_this_src_childof_tag_w_parent(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_entity_t p_1 = ecs_new(world);
    ecs_entity_t p_2 = ecs_new(world);
    ecs_entity_t p_3 = ecs_new(world);

    /* ecs_entity_t c1 = */ ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t c2 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t c3 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t c4 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    /* ecs_entity_t c5 = */ ecs_new_w_pair(world, EcsChildOf, p_1);

    /* ecs_entity_t c6 = */ ecs_new_w_pair(world, EcsChildOf, p_2);
    /* ecs_entity_t c7 = */ ecs_insert(world, ecs_value(EcsParent, {p_2}));

    /* ecs_entity_t c8 = */ ecs_new_w_pair(world, EcsChildOf, p_3);
    ecs_entity_t c9 = ecs_new_w_pair(world, EcsChildOf, p_3);
    ecs_entity_t c10 = ecs_new_w_pair(world, EcsChildOf, p_3);
    ecs_entity_t c11 = ecs_new_w_pair(world, EcsChildOf, p_3);

    ecs_add(world, c2, Foo);
    ecs_add(world, c3, Foo);
    ecs_add(world, c4, Bar);

    ecs_add(world, c9, Foo);
    ecs_add(world, c10, Foo);
    ecs_add(world, c11, Foo);
    ecs_add(world, c11, Bar);

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {
                { Foo },
                { ecs_childof(p_1) }
            },
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c2, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_childof(p_1), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c3, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_childof(p_1), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));

        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {
                { Foo },
                { ecs_childof(p_2) }
            },
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {
                { Foo },
                { ecs_childof(p_3) }
            },
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(2, it.count);
        test_uint(c9, it.entities[0]);
        test_uint(c10, it.entities[1]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_childof(p_3), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c11, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_childof(p_3), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));

        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    ecs_fini(world);
}

void NonFragmentingChildOf_this_src_childof_tag_w_parent_single_child(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_entity_t p_1 = ecs_new(world);
    ecs_entity_t p_2 = ecs_new(world);
    ecs_entity_t p_3 = ecs_new(world);

    /* ecs_entity_t c1 = */ ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t c2 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t c3 = ecs_new_w_pair(world, EcsChildOf, p_1);
    ecs_entity_t c4 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    /* ecs_entity_t c5 = */ ecs_new_w_pair(world, EcsChildOf, p_1);

    /* ecs_entity_t c6 = */ ecs_new_w_pair(world, EcsChildOf, p_2);
    ecs_entity_t c7 = ecs_insert(world, ecs_value(EcsParent, {p_2}));

    /* ecs_entity_t c8 = */ ecs_new_w_pair(world, EcsChildOf, p_3);
    ecs_entity_t c9 = ecs_new_w_pair(world, EcsChildOf, p_3);
    ecs_entity_t c10 = ecs_new_w_pair(world, EcsChildOf, p_3);
    ecs_entity_t c11 = ecs_new_w_pair(world, EcsChildOf, p_3);

    ecs_add(world, c2, Foo);
    ecs_add(world, c3, Foo);
    ecs_add(world, c4, Bar);

    ecs_add(world, c7, Foo);

    ecs_add(world, c9, Foo);
    ecs_add(world, c10, Foo);
    ecs_add(world, c11, Foo);
    ecs_add(world, c11, Bar);

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {
                { Foo },
                { ecs_childof(p_1) }
            },
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c2, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_childof(p_1), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c3, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_childof(p_1), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));

        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {
                { Foo },
                { ecs_childof(p_2) }
            },
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c7, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_childof(p_2), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {
                { Foo },
                { ecs_childof(p_3) }
            },
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(2, it.count);
        test_uint(c9, it.entities[0]);
        test_uint(c10, it.entities[1]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_childof(p_3), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c11, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_childof(p_3), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));

        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    ecs_fini(world);
}

void NonFragmentingChildOf_this_src_childof_tag_w_parent_single_child_at_offset(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_entity_t p_0 = ecs_new(world);
    ecs_entity_t p_1 = ecs_new(world);
    ecs_entity_t p_2 = ecs_new(world);
    ecs_entity_t p_3 = ecs_new(world);

    ecs_entity_t c1 = ecs_insert(world, ecs_value(EcsParent, {p_0}));
    ecs_entity_t c2 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t c3 = ecs_new_w_pair(world, EcsChildOf, p_1);
    ecs_entity_t c4 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    /* ecs_entity_t c5 = */ ecs_new_w_pair(world, EcsChildOf, p_1);

    /* ecs_entity_t c6 = */ ecs_new_w_pair(world, EcsChildOf, p_2);
    ecs_entity_t c7 = ecs_insert(world, ecs_value(EcsParent, {p_2}));

    /* ecs_entity_t c8 = */ ecs_new_w_pair(world, EcsChildOf, p_3);
    ecs_entity_t c9 = ecs_new_w_pair(world, EcsChildOf, p_3);
    ecs_entity_t c10 = ecs_new_w_pair(world, EcsChildOf, p_3);
    ecs_entity_t c11 = ecs_new_w_pair(world, EcsChildOf, p_3);

    ecs_add(world, c1, Foo);
    ecs_add(world, c2, Foo);
    ecs_add(world, c3, Foo);
    ecs_add(world, c4, Bar);

    ecs_add(world, c7, Foo);

    ecs_add(world, c9, Foo);
    ecs_add(world, c10, Foo);
    ecs_add(world, c11, Foo);
    ecs_add(world, c11, Bar);

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {
                { Foo },
                { ecs_childof(p_1) }
            },
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c2, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_childof(p_1), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c3, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_childof(p_1), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));

        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {
                { Foo },
                { ecs_childof(p_2) }
            },
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c7, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_childof(p_2), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {
                { Foo },
                { ecs_childof(p_3) }
            },
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(2, it.count);
        test_uint(c9, it.entities[0]);
        test_uint(c10, it.entities[1]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_childof(p_3), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c11, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_childof(p_3), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));

        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    ecs_fini(world);
}

void NonFragmentingChildOf_this_src_childof_tag_w_parent_single_child_after_range(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t p_1 = ecs_entity(world, { .name = "p_1" });
    ecs_entity_t p_2 = ecs_entity(world, { .name = "p_2" });
    ecs_entity_t p_3 = ecs_entity(world, { .name = "p_3" });

    ecs_entity_t e_1 = ecs_entity(world, { .name = "e_1" });
    ecs_entity_t e_2 = ecs_entity(world, { .name = "e_2" });
    ecs_entity_t e_3 = ecs_entity(world, { .name = "e_3" });

    ecs_set(world, e_1, EcsParent, {p_1});
    ecs_set(world, e_2, EcsParent, {p_2});
    ecs_set(world, e_3, EcsParent, {p_3});

    {
        ecs_query_t *q = ecs_query(world, {
            .expr = "$this == p_1.e_1, (ChildOf, p_1)",
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e_1, it.entities[0]);
        test_uint(ecs_childof(p_1), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    {
        ecs_query_t *q = ecs_query(world, {
            .expr = "$this == p_2.e_2, (ChildOf, p_2)",
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e_2, it.entities[0]);
        test_uint(ecs_childof(p_2), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    {
        ecs_query_t *q = ecs_query(world, {
            .expr = "$this == p_3.e_3, (ChildOf, p_3)",
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e_3, it.entities[0]);
        test_uint(ecs_childof(p_3), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    ecs_fini(world);
}

void NonFragmentingChildOf_this_src_childof_tag_w_parent_n_children_after_toggle_range(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Foo, CanToggle);

    ecs_entity_t p_1 = ecs_new(world);
    ecs_entity_t p_2 = ecs_new(world);
    ecs_entity_t p_3 = ecs_new(world);

    ecs_entity_t e_1 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t e_2 = ecs_insert(world, ecs_value(EcsParent, {p_2}));
    ecs_entity_t e_3 = ecs_insert(world, ecs_value(EcsParent, {p_3}));
    
    ecs_add(world, e_1, Foo);
    ecs_add(world, e_2, Foo);
    ecs_add(world, e_3, Foo);

    ecs_enable_id(world, e_1, Foo, false);
    ecs_enable_id(world, e_2, Foo, true);
    ecs_enable_id(world, e_3, Foo, true);

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {{ Foo }, { ecs_childof(p_1) }},
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {{ Foo }, { ecs_childof(p_2) }},
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e_2, it.entities[0]);
        test_uint(ecs_childof(p_2), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {{ Foo }, { ecs_childof(p_3) }},
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e_3, it.entities[0]);
        test_uint(ecs_childof(p_3), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    ecs_fini(world);
}

void NonFragmentingChildOf_this_src_childof_tag_w_parent_1_child_after_set_range(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_entity_t p_1 = ecs_new(world);
    ecs_entity_t p_2 = ecs_new(world);
    ecs_entity_t p_3 = ecs_new(world);

    ecs_entity_t e_1 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t e_2 = ecs_insert(world, ecs_value(EcsParent, {p_2}));
    ecs_entity_t e_3 = ecs_insert(world, ecs_value(EcsParent, {p_3}));
    
    ecs_add(world, e_1, Foo);
    ecs_add(world, e_2, Foo);
    ecs_add(world, e_3, Foo);

    ecs_table_t *table = ecs_get_table(world, e_1);
    test_assert(table == ecs_get_table(world, e_2));
    test_assert(table == ecs_get_table(world, e_3));

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {{ Foo }, { ecs_childof(p_1) }},
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        {
            ecs_iter_t it = ecs_query_iter(world, q);
            ecs_iter_set_var_as_range(&it, 0, &(ecs_table_range_t){ .table = table, .offset = 0, .count = 2 });
            test_bool(true, ecs_query_next(&it));
            test_int(1, it.count);
            test_uint(e_1, it.entities[0]);
            test_uint(ecs_childof(p_1), ecs_field_id(&it, 1));
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 1));
            test_bool(false, ecs_query_next(&it));
        }

        {
            ecs_iter_t it = ecs_query_iter(world, q);
            ecs_iter_set_var_as_range(&it, 0, &(ecs_table_range_t){ .table = table, .offset = 1, .count = 2 });
            test_bool(false, ecs_query_next(&it));
        }

        ecs_query_fini(q);
    }

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {{ Foo }, { ecs_childof(p_2) }},
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        {
            ecs_iter_t it = ecs_query_iter(world, q);
            ecs_iter_set_var_as_range(&it, 0, &(ecs_table_range_t){ .table = table, .offset = 0, .count = 2 });
            test_bool(true, ecs_query_next(&it));
            test_int(1, it.count);
            test_uint(e_2, it.entities[0]);
            test_uint(ecs_childof(p_2), ecs_field_id(&it, 1));
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 1));
            test_bool(false, ecs_query_next(&it));
        }

        {
            ecs_iter_t it = ecs_query_iter(world, q);
            ecs_iter_set_var_as_range(&it, 0, &(ecs_table_range_t){ .table = table, .offset = 1, .count = 2 });
            test_bool(true, ecs_query_next(&it));
            test_int(1, it.count);
            test_uint(e_2, it.entities[0]);
            test_uint(ecs_childof(p_2), ecs_field_id(&it, 1));
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 1));
            test_bool(false, ecs_query_next(&it));
        }

        ecs_query_fini(q);
    }

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {{ Foo }, { ecs_childof(p_3) }},
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        {
            ecs_iter_t it = ecs_query_iter(world, q);
            ecs_iter_set_var_as_range(&it, 0, &(ecs_table_range_t){ .table = table, .offset = 0, .count = 2 });
            test_bool(false, ecs_query_next(&it));
        }

        {
            ecs_iter_t it = ecs_query_iter(world, q);
            ecs_iter_set_var_as_range(&it, 0, &(ecs_table_range_t){ .table = table, .offset = 1, .count = 2 });
            test_bool(true, ecs_query_next(&it));
            test_int(1, it.count);
            test_uint(e_3, it.entities[0]);
            test_uint(ecs_childof(p_3), ecs_field_id(&it, 1));
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 1));
            test_bool(false, ecs_query_next(&it));
        }

        ecs_query_fini(q);
    }

    ecs_fini(world);
}

void NonFragmentingChildOf_this_src_childof_tag_w_parent_n_children_after_set_range(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_entity_t p_1 = ecs_new(world);
    ecs_entity_t p_2 = ecs_new(world);
    ecs_entity_t p_3 = ecs_new(world);

    ecs_entity_t e_1_1 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t e_1_2 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t e_2_1 = ecs_insert(world, ecs_value(EcsParent, {p_2}));
    ecs_entity_t e_2_2 = ecs_insert(world, ecs_value(EcsParent, {p_2}));
    ecs_entity_t e_3_1 = ecs_insert(world, ecs_value(EcsParent, {p_3}));
    ecs_entity_t e_3_2 = ecs_insert(world, ecs_value(EcsParent, {p_3}));
    
    ecs_add(world, e_1_1, Foo);
    ecs_add(world, e_2_1, Foo);
    ecs_add(world, e_3_1, Foo);
    ecs_add(world, e_1_2, Foo);
    ecs_add(world, e_2_2, Foo);
    ecs_add(world, e_3_2, Foo);

    ecs_table_t *table = ecs_get_table(world, e_1_1);
    test_assert(table == ecs_get_table(world, e_2_1));
    test_assert(table == ecs_get_table(world, e_3_1));
    test_assert(table == ecs_get_table(world, e_1_2));
    test_assert(table == ecs_get_table(world, e_2_2));
    test_assert(table == ecs_get_table(world, e_3_2));

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {{ Foo }, { ecs_childof(p_1) }},
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        {
            ecs_iter_t it = ecs_query_iter(world, q);
            ecs_iter_set_var_as_range(&it, 0, &(ecs_table_range_t){ .table = table, .offset = 0, .count = 2 });
            test_bool(true, ecs_query_next(&it));
            test_int(1, it.count);
            test_uint(e_1_1, it.entities[0]);
            test_uint(ecs_childof(p_1), ecs_field_id(&it, 1));
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 1));
            test_bool(false, ecs_query_next(&it));
        }

        {
            ecs_iter_t it = ecs_query_iter(world, q);
            ecs_iter_set_var_as_range(&it, 0, &(ecs_table_range_t){ .table = table, .offset = 1, .count = 2 });
            test_bool(false, ecs_query_next(&it));
        }

        {
            ecs_iter_t it = ecs_query_iter(world, q);
            ecs_iter_set_var_as_range(&it, 0, &(ecs_table_range_t){ .table = table, .offset = 2, .count = 2 });
            test_bool(true, ecs_query_next(&it));
            test_int(1, it.count);
            test_uint(e_1_2, it.entities[0]);
            test_uint(ecs_childof(p_1), ecs_field_id(&it, 1));
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 1));
            test_bool(false, ecs_query_next(&it));
        }

        {
            ecs_iter_t it = ecs_query_iter(world, q);
            ecs_iter_set_var_as_range(&it, 0, &(ecs_table_range_t){ .table = table, .offset = 3, .count = 2 });
            test_bool(true, ecs_query_next(&it));
            test_int(1, it.count);
            test_uint(e_1_2, it.entities[0]);
            test_uint(ecs_childof(p_1), ecs_field_id(&it, 1));
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 1));
            test_bool(false, ecs_query_next(&it));
        }

        {
            ecs_iter_t it = ecs_query_iter(world, q);
            ecs_iter_set_var_as_range(&it, 0, &(ecs_table_range_t){ .table = table, .offset = 4, .count = 2 });
            test_bool(false, ecs_query_next(&it));
        }

        ecs_query_fini(q);
    }

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {{ Foo }, { ecs_childof(p_2) }},
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        {
            ecs_iter_t it = ecs_query_iter(world, q);
            ecs_iter_set_var_as_range(&it, 0, &(ecs_table_range_t){ .table = table, .offset = 0, .count = 2 });
            test_bool(true, ecs_query_next(&it));
            test_int(1, it.count);
            test_uint(e_2_1, it.entities[0]);
            test_uint(ecs_childof(p_2), ecs_field_id(&it, 1));
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 1));
            test_bool(false, ecs_query_next(&it));
        }

        {
            ecs_iter_t it = ecs_query_iter(world, q);
            ecs_iter_set_var_as_range(&it, 0, &(ecs_table_range_t){ .table = table, .offset = 1, .count = 2 });
            test_bool(true, ecs_query_next(&it));
            test_int(1, it.count);
            test_uint(e_2_1, it.entities[0]);
            test_uint(ecs_childof(p_2), ecs_field_id(&it, 1));
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 1));
            test_bool(false, ecs_query_next(&it));
        }

        {
            ecs_iter_t it = ecs_query_iter(world, q);
            ecs_iter_set_var_as_range(&it, 0, &(ecs_table_range_t){ .table = table, .offset = 2, .count = 2 });
            test_bool(false, ecs_query_next(&it));
        }

        {
            ecs_iter_t it = ecs_query_iter(world, q);
            ecs_iter_set_var_as_range(&it, 0, &(ecs_table_range_t){ .table = table, .offset = 3, .count = 2 });
            test_bool(true, ecs_query_next(&it));
            test_int(1, it.count);
            test_uint(e_2_2, it.entities[0]);
            test_uint(ecs_childof(p_2), ecs_field_id(&it, 1));
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 1));
            test_bool(false, ecs_query_next(&it));
        }

        {
            ecs_iter_t it = ecs_query_iter(world, q);
            ecs_iter_set_var_as_range(&it, 0, &(ecs_table_range_t){ .table = table, .offset = 4, .count = 2 });
            test_bool(true, ecs_query_next(&it));
            test_int(1, it.count);
            test_uint(e_2_2, it.entities[0]);
            test_uint(ecs_childof(p_2), ecs_field_id(&it, 1));
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 1));
            test_bool(false, ecs_query_next(&it));
        }

        ecs_query_fini(q);
    }

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {{ Foo }, { ecs_childof(p_3) }},
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        {
            ecs_iter_t it = ecs_query_iter(world, q);
            ecs_iter_set_var_as_range(&it, 0, &(ecs_table_range_t){ .table = table, .offset = 0, .count = 2 });
            test_bool(false, ecs_query_next(&it));
        }

        {
            ecs_iter_t it = ecs_query_iter(world, q);
            ecs_iter_set_var_as_range(&it, 0, &(ecs_table_range_t){ .table = table, .offset = 1, .count = 2 });
            test_bool(true, ecs_query_next(&it));
            test_int(1, it.count);
            test_uint(e_3_1, it.entities[0]);
            test_uint(ecs_childof(p_3), ecs_field_id(&it, 1));
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 1));
            test_bool(false, ecs_query_next(&it));
        }

        {
            ecs_iter_t it = ecs_query_iter(world, q);
            ecs_iter_set_var_as_range(&it, 0, &(ecs_table_range_t){ .table = table, .offset = 2, .count = 2 });
            test_bool(true, ecs_query_next(&it));
            test_int(1, it.count);
            test_uint(e_3_1, it.entities[0]);
            test_uint(ecs_childof(p_3), ecs_field_id(&it, 1));
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 1));
            test_bool(false, ecs_query_next(&it));
        }

        {
            ecs_iter_t it = ecs_query_iter(world, q);
            ecs_iter_set_var_as_range(&it, 0, &(ecs_table_range_t){ .table = table, .offset = 3, .count = 2 });
            test_bool(false, ecs_query_next(&it));
        }

        {
            ecs_iter_t it = ecs_query_iter(world, q);
            ecs_iter_set_var_as_range(&it, 0, &(ecs_table_range_t){ .table = table, .offset = 4, .count = 2 });
            test_bool(true, ecs_query_next(&it));
            test_int(1, it.count);
            test_uint(e_3_2, it.entities[0]);
            test_uint(ecs_childof(p_3), ecs_field_id(&it, 1));
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 1));
            test_bool(false, ecs_query_next(&it));
        }

        ecs_query_fini(q);
    }

    ecs_fini(world);
}

void NonFragmentingChildOf_this_src_childof_tag_w_parent_n_children_after_set_range_2(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_entity_t p_1 = ecs_new(world);
    ecs_entity_t p_2 = ecs_new(world);
    ecs_entity_t p_3 = ecs_new(world);

    ecs_entity_t e_1_1 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t e_1_2 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t e_2_1 = ecs_insert(world, ecs_value(EcsParent, {p_2}));
    ecs_entity_t e_2_2 = ecs_insert(world, ecs_value(EcsParent, {p_2}));
    ecs_entity_t e_3_1 = ecs_insert(world, ecs_value(EcsParent, {p_3}));
    ecs_entity_t e_3_2 = ecs_insert(world, ecs_value(EcsParent, {p_3}));
    
    ecs_add(world, e_1_1, Foo);
    ecs_add(world, e_2_1, Foo);
    ecs_add(world, e_3_1, Foo);
    ecs_add(world, e_1_2, Foo);
    ecs_add(world, e_2_2, Foo);
    ecs_add(world, e_3_2, Foo);

    ecs_table_t *table = ecs_get_table(world, e_1_1);
    test_assert(table == ecs_get_table(world, e_2_1));
    test_assert(table == ecs_get_table(world, e_3_1));
    test_assert(table == ecs_get_table(world, e_1_2));
    test_assert(table == ecs_get_table(world, e_2_2));
    test_assert(table == ecs_get_table(world, e_3_2));

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {{ Foo }, { ecs_childof(p_1) }},
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        {
            ecs_iter_t it = ecs_query_iter(world, q);
            ecs_iter_set_var_as_range(&it, 0, &(ecs_table_range_t){ .table = table, .offset = 0, .count = 3 });
            test_bool(true, ecs_query_next(&it));
            test_int(1, it.count);
            test_uint(e_1_1, it.entities[0]);
            test_uint(ecs_childof(p_1), ecs_field_id(&it, 1));
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 1));
            test_bool(false, ecs_query_next(&it));
        }

        {
            ecs_iter_t it = ecs_query_iter(world, q);
            ecs_iter_set_var_as_range(&it, 0, &(ecs_table_range_t){ .table = table, .offset = 1, .count = 3 });
            test_bool(true, ecs_query_next(&it));
            test_int(1, it.count);
            test_uint(e_1_2, it.entities[0]);
            test_uint(ecs_childof(p_1), ecs_field_id(&it, 1));
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 1));
            test_bool(false, ecs_query_next(&it));
        }

        {
            ecs_iter_t it = ecs_query_iter(world, q);
            ecs_iter_set_var_as_range(&it, 0, &(ecs_table_range_t){ .table = table, .offset = 2, .count = 3 });
            test_bool(true, ecs_query_next(&it));
            test_int(1, it.count);
            test_uint(e_1_2, it.entities[0]);
            test_uint(ecs_childof(p_1), ecs_field_id(&it, 1));
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 1));
            test_bool(false, ecs_query_next(&it));
        }

        {
            ecs_iter_t it = ecs_query_iter(world, q);
            ecs_iter_set_var_as_range(&it, 0, &(ecs_table_range_t){ .table = table, .offset = 3, .count = 3 });
            test_bool(true, ecs_query_next(&it));
            test_int(1, it.count);
            test_uint(e_1_2, it.entities[0]);
            test_uint(ecs_childof(p_1), ecs_field_id(&it, 1));
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 1));
            test_bool(false, ecs_query_next(&it));
        }

        ecs_query_fini(q);
    }

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {{ Foo }, { ecs_childof(p_2) }},
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        {
            ecs_iter_t it = ecs_query_iter(world, q);
            ecs_iter_set_var_as_range(&it, 0, &(ecs_table_range_t){ .table = table, .offset = 0, .count = 3 });
            test_bool(true, ecs_query_next(&it));
            test_int(1, it.count);
            test_uint(e_2_1, it.entities[0]);
            test_uint(ecs_childof(p_2), ecs_field_id(&it, 1));
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 1));
            test_bool(false, ecs_query_next(&it));
        }

        {
            ecs_iter_t it = ecs_query_iter(world, q);
            ecs_iter_set_var_as_range(&it, 0, &(ecs_table_range_t){ .table = table, .offset = 1, .count = 3 });
            test_bool(true, ecs_query_next(&it));
            test_int(1, it.count);
            test_uint(e_2_1, it.entities[0]);
            test_uint(ecs_childof(p_2), ecs_field_id(&it, 1));
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 1));
            test_bool(false, ecs_query_next(&it));
        }

        {
            ecs_iter_t it = ecs_query_iter(world, q);
            ecs_iter_set_var_as_range(&it, 0, &(ecs_table_range_t){ .table = table, .offset = 2, .count = 3 });
            test_bool(true, ecs_query_next(&it));
            test_int(1, it.count);
            test_uint(e_2_2, it.entities[0]);
            test_uint(ecs_childof(p_2), ecs_field_id(&it, 1));
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 1));
            test_bool(false, ecs_query_next(&it));
        }

        {
            ecs_iter_t it = ecs_query_iter(world, q);
            ecs_iter_set_var_as_range(&it, 0, &(ecs_table_range_t){ .table = table, .offset = 3, .count = 3 });
            test_bool(true, ecs_query_next(&it));
            test_int(1, it.count);
            test_uint(e_2_2, it.entities[0]);
            test_uint(ecs_childof(p_2), ecs_field_id(&it, 1));
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 1));
            test_bool(false, ecs_query_next(&it));
        }

        ecs_query_fini(q);
    }

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {{ Foo }, { ecs_childof(p_3) }},
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        {
            ecs_iter_t it = ecs_query_iter(world, q);
            ecs_iter_set_var_as_range(&it, 0, &(ecs_table_range_t){ .table = table, .offset = 0, .count = 3 });
            test_bool(true, ecs_query_next(&it));
            test_int(1, it.count);
            test_uint(e_3_1, it.entities[0]);
            test_uint(ecs_childof(p_3), ecs_field_id(&it, 1));
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 1));
            test_bool(false, ecs_query_next(&it));
        }

        {
            ecs_iter_t it = ecs_query_iter(world, q);
            ecs_iter_set_var_as_range(&it, 0, &(ecs_table_range_t){ .table = table, .offset = 1, .count = 3 });
            test_bool(true, ecs_query_next(&it));
            test_int(1, it.count);
            test_uint(e_3_1, it.entities[0]);
            test_uint(ecs_childof(p_3), ecs_field_id(&it, 1));
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 1));
            test_bool(false, ecs_query_next(&it));
        }

        {
            ecs_iter_t it = ecs_query_iter(world, q);
            ecs_iter_set_var_as_range(&it, 0, &(ecs_table_range_t){ .table = table, .offset = 2, .count = 3 });
            test_bool(true, ecs_query_next(&it));
            test_int(1, it.count);
            test_uint(e_3_1, it.entities[0]);
            test_uint(ecs_childof(p_3), ecs_field_id(&it, 1));
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 1));
            test_bool(false, ecs_query_next(&it));
        }

        {
            ecs_iter_t it = ecs_query_iter(world, q);
            ecs_iter_set_var_as_range(&it, 0, &(ecs_table_range_t){ .table = table, .offset = 3, .count = 3 });
            test_bool(true, ecs_query_next(&it));
            test_int(1, it.count);
            test_uint(e_3_2, it.entities[0]);
            test_uint(ecs_childof(p_3), ecs_field_id(&it, 1));
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 1));
            test_bool(false, ecs_query_next(&it));
        }

        ecs_query_fini(q);
    }

    ecs_fini(world);
}

void NonFragmentingChildOf_this_src_childof_tag_w_parent_n_children_after_set_range_3(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_entity_t p_1 = ecs_new(world);
    ecs_entity_t p_2 = ecs_new(world);
    ecs_entity_t p_3 = ecs_new(world);

    ecs_entity_t e_1_1 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t e_1_2 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t e_2_1 = ecs_insert(world, ecs_value(EcsParent, {p_2}));
    ecs_entity_t e_2_2 = ecs_insert(world, ecs_value(EcsParent, {p_2}));
    ecs_entity_t e_3_1 = ecs_insert(world, ecs_value(EcsParent, {p_3}));
    ecs_entity_t e_3_2 = ecs_insert(world, ecs_value(EcsParent, {p_3}));
    
    ecs_add(world, e_1_1, Foo);
    ecs_add(world, e_2_1, Foo);
    ecs_add(world, e_3_1, Foo);
    ecs_add(world, e_1_2, Foo);
    ecs_add(world, e_2_2, Foo);
    ecs_add(world, e_3_2, Foo);

    ecs_table_t *table = ecs_get_table(world, e_1_1);
    test_assert(table == ecs_get_table(world, e_2_1));
    test_assert(table == ecs_get_table(world, e_3_1));
    test_assert(table == ecs_get_table(world, e_1_2));
    test_assert(table == ecs_get_table(world, e_2_2));
    test_assert(table == ecs_get_table(world, e_3_2));

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {{ Foo }, { ecs_childof(p_1) }},
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        {
            ecs_iter_t it = ecs_query_iter(world, q);
            ecs_iter_set_var_as_range(&it, 0, &(ecs_table_range_t){ .table = table, .offset = 0, .count = 4 });
            test_bool(true, ecs_query_next(&it));
            test_int(1, it.count);
            test_uint(e_1_1, it.entities[0]);
            test_uint(ecs_childof(p_1), ecs_field_id(&it, 1));
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 1));

            test_bool(true, ecs_query_next(&it));
            test_int(1, it.count);
            test_uint(e_1_2, it.entities[0]);
            test_uint(ecs_childof(p_1), ecs_field_id(&it, 1));
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 1));

            test_bool(false, ecs_query_next(&it));
        }

        {
            ecs_iter_t it = ecs_query_iter(world, q);
            ecs_iter_set_var_as_range(&it, 0, &(ecs_table_range_t){ .table = table, .offset = 1, .count = 4 });
            test_bool(true, ecs_query_next(&it));
            test_int(1, it.count);
            test_uint(e_1_2, it.entities[0]);
            test_uint(ecs_childof(p_1), ecs_field_id(&it, 1));
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 1));
            test_bool(false, ecs_query_next(&it));
        }

        {
            ecs_iter_t it = ecs_query_iter(world, q);
            ecs_iter_set_var_as_range(&it, 0, &(ecs_table_range_t){ .table = table, .offset = 2, .count = 4 });
            test_bool(true, ecs_query_next(&it));
            test_int(1, it.count);
            test_uint(e_1_2, it.entities[0]);
            test_uint(ecs_childof(p_1), ecs_field_id(&it, 1));
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 1));
            test_bool(false, ecs_query_next(&it));
        }

        ecs_query_fini(q);
    }

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {{ Foo }, { ecs_childof(p_2) }},
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        {
            ecs_iter_t it = ecs_query_iter(world, q);
            ecs_iter_set_var_as_range(&it, 0, &(ecs_table_range_t){ .table = table, .offset = 0, .count = 4 });
            test_bool(true, ecs_query_next(&it));
            test_int(1, it.count);
            test_uint(e_2_1, it.entities[0]);
            test_uint(ecs_childof(p_2), ecs_field_id(&it, 1));
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 1));
            test_bool(false, ecs_query_next(&it));
        }

        {
            ecs_iter_t it = ecs_query_iter(world, q);
            ecs_iter_set_var_as_range(&it, 0, &(ecs_table_range_t){ .table = table, .offset = 1, .count = 4 });
            test_bool(true, ecs_query_next(&it));
            test_int(1, it.count);
            test_uint(e_2_1, it.entities[0]);
            test_uint(ecs_childof(p_2), ecs_field_id(&it, 1));
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 1));

            test_bool(true, ecs_query_next(&it));
            test_int(1, it.count);
            test_uint(e_2_2, it.entities[0]);
            test_uint(ecs_childof(p_2), ecs_field_id(&it, 1));
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 1));

            test_bool(false, ecs_query_next(&it));
        }

        {
            ecs_iter_t it = ecs_query_iter(world, q);
            ecs_iter_set_var_as_range(&it, 0, &(ecs_table_range_t){ .table = table, .offset = 2, .count = 4 });
            test_bool(true, ecs_query_next(&it));
            test_int(1, it.count);
            test_uint(e_2_2, it.entities[0]);
            test_uint(ecs_childof(p_2), ecs_field_id(&it, 1));
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 1));
            test_bool(false, ecs_query_next(&it));
        }

        ecs_query_fini(q);
    }

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {{ Foo }, { ecs_childof(p_3) }},
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        {
            ecs_iter_t it = ecs_query_iter(world, q);
            ecs_iter_set_var_as_range(&it, 0, &(ecs_table_range_t){ .table = table, .offset = 0, .count = 4 });
            test_bool(true, ecs_query_next(&it));
            test_int(1, it.count);
            test_uint(e_3_1, it.entities[0]);
            test_uint(ecs_childof(p_3), ecs_field_id(&it, 1));
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 1));
            test_bool(false, ecs_query_next(&it));
        }

        {
            ecs_iter_t it = ecs_query_iter(world, q);
            ecs_iter_set_var_as_range(&it, 0, &(ecs_table_range_t){ .table = table, .offset = 1, .count = 4 });
            test_bool(true, ecs_query_next(&it));
            test_int(1, it.count);
            test_uint(e_3_1, it.entities[0]);
            test_uint(ecs_childof(p_3), ecs_field_id(&it, 1));
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 1));
            test_bool(false, ecs_query_next(&it));
        }

        {
            ecs_iter_t it = ecs_query_iter(world, q);
            ecs_iter_set_var_as_range(&it, 0, &(ecs_table_range_t){ .table = table, .offset = 2, .count = 4 });
            test_bool(true, ecs_query_next(&it));
            test_int(1, it.count);
            test_uint(e_3_1, it.entities[0]);
            test_uint(ecs_childof(p_3), ecs_field_id(&it, 1));
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 1));

            test_bool(true, ecs_query_next(&it));
            test_int(1, it.count);
            test_uint(e_3_2, it.entities[0]);
            test_uint(ecs_childof(p_3), ecs_field_id(&it, 1));
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 1));

            test_bool(false, ecs_query_next(&it));
        }

        ecs_query_fini(q);
    }

    ecs_fini(world);
}

void NonFragmentingChildOf_this_src_childof_tag_w_wildcard_mixed(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_entity_t p_1 = ecs_new(world);
    ecs_entity_t p_2 = ecs_new(world);
    ecs_entity_t p_3 = ecs_new(world);

    /* ecs_entity_t c1 = */ ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t c2 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t c3 = ecs_new_w_pair(world, EcsChildOf, p_1);
    ecs_entity_t c4 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    /* ecs_entity_t c5 = */ ecs_new_w_pair(world, EcsChildOf, p_1);

    /* ecs_entity_t c6 = */ ecs_new_w_pair(world, EcsChildOf, p_2);
    ecs_entity_t c7 = ecs_insert(world, ecs_value(EcsParent, {p_2}));

    /* ecs_entity_t c8 = */ ecs_new_w_pair(world, EcsChildOf, p_3);
    ecs_entity_t c9 = ecs_new_w_pair(world, EcsChildOf, p_3);
    ecs_entity_t c10 = ecs_new_w_pair(world, EcsChildOf, p_3);
    ecs_entity_t c11 = ecs_new_w_pair(world, EcsChildOf, p_3);

    ecs_add(world, c2, Foo);
    ecs_add(world, c3, Foo);
    ecs_add(world, c4, Bar);

    ecs_add(world, c7, Foo);

    ecs_add(world, c9, Foo);
    ecs_add(world, c10, Foo);
    ecs_add(world, c11, Foo);
    ecs_add(world, c11, Bar);

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {
                { Foo },
                { ecs_childof(EcsWildcard) }
            },
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c2, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_childof(p_1), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c7, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_childof(p_2), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c3, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_childof(p_1), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(2, it.count);
        test_uint(c9, it.entities[0]);
        test_uint(c10, it.entities[1]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_childof(p_3), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c11, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_childof(p_3), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));

        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    ecs_fini(world);
}

void NonFragmentingChildOf_this_src_childof_tag_w_wildcard_single_child(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_entity_t p_1 = ecs_new(world);
    ecs_entity_t p_2 = ecs_new(world);
    ecs_entity_t p_3 = ecs_new(world);

    ecs_entity_t c1 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t c2 = ecs_insert(world, ecs_value(EcsParent, {p_2}));
    ecs_entity_t c3 = ecs_insert(world, ecs_value(EcsParent, {p_3}));

    ecs_add(world, c1, Foo);
    ecs_add(world, c2, Foo);
    ecs_add(world, c3, Foo);

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {
                { Foo },
                { ecs_childof(EcsWildcard) }
            },
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c1, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_childof(p_1), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c2, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_childof(p_2), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c3, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_childof(p_3), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));

        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    ecs_fini(world);
}

void NonFragmentingChildOf_this_src_childof_tag_w_wildcard_single_child_at_offset(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_entity_t p_1 = ecs_new(world);
    ecs_entity_t p_2 = ecs_new(world);
    ecs_entity_t p_3 = ecs_new(world);

    ecs_entity_t c1_1 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t c1_2 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t c2_1 = ecs_insert(world, ecs_value(EcsParent, {p_2}));
    ecs_entity_t c2_2 = ecs_insert(world, ecs_value(EcsParent, {p_2}));
    ecs_entity_t c3_1 = ecs_insert(world, ecs_value(EcsParent, {p_3}));
    ecs_entity_t c3_2 = ecs_insert(world, ecs_value(EcsParent, {p_3}));

    ecs_add(world, c1_1, Foo);
    ecs_add(world, c2_1, Foo);
    ecs_add(world, c3_1, Foo);
    ecs_add(world, c1_2, Foo);
    ecs_add(world, c2_2, Foo);
    ecs_add(world, c3_2, Foo);

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {
                { Foo },
                { ecs_childof(EcsWildcard) }
            },
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c1_1, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_childof(p_1), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c2_1, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_childof(p_2), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c3_1, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_childof(p_3), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c1_2, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_childof(p_1), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c2_2, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_childof(p_2), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c3_2, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_childof(p_3), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));

        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    ecs_fini(world);
}

void NonFragmentingChildOf_this_src_childof_tag_w_wildcard_1_child_after_set_range(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_entity_t p_1 = ecs_new(world);
    ecs_entity_t p_2 = ecs_new(world);
    ecs_entity_t p_3 = ecs_new(world);

    ecs_entity_t e_1 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t e_2 = ecs_insert(world, ecs_value(EcsParent, {p_2}));
    ecs_entity_t e_3 = ecs_insert(world, ecs_value(EcsParent, {p_3}));
    
    ecs_add(world, e_1, Foo);
    ecs_add(world, e_2, Foo);
    ecs_add(world, e_3, Foo);

    ecs_table_t *table = ecs_get_table(world, e_1);
    test_assert(table == ecs_get_table(world, e_2));
    test_assert(table == ecs_get_table(world, e_3));

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {{ Foo }, { ecs_childof(EcsWildcard) }},
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        {
            ecs_iter_t it = ecs_query_iter(world, q);
            ecs_iter_set_var_as_range(&it, 0, &(ecs_table_range_t){ .table = table, .offset = 0, .count = 2 });
            test_bool(true, ecs_query_next(&it));
            test_int(1, it.count);
            test_uint(e_1, it.entities[0]);
            test_uint(ecs_childof(p_1), ecs_field_id(&it, 1));
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 1));

            test_bool(true, ecs_query_next(&it));
            test_int(1, it.count);
            test_uint(e_2, it.entities[0]);
            test_uint(ecs_childof(p_2), ecs_field_id(&it, 1));
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 1));

            test_bool(false, ecs_query_next(&it));
        }

        {
            ecs_iter_t it = ecs_query_iter(world, q);
            ecs_iter_set_var_as_range(&it, 0, &(ecs_table_range_t){ .table = table, .offset = 1, .count = 2 });
            test_bool(true, ecs_query_next(&it));
            test_int(1, it.count);
            test_uint(e_2, it.entities[0]);
            test_uint(ecs_childof(p_2), ecs_field_id(&it, 1));
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 1));

            test_bool(true, ecs_query_next(&it));
            test_int(1, it.count);
            test_uint(e_3, it.entities[0]);
            test_uint(ecs_childof(p_3), ecs_field_id(&it, 1));
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 1));

            test_bool(false, ecs_query_next(&it));
        }

        ecs_query_fini(q);
    }

    ecs_fini(world);
}

void NonFragmentingChildOf_this_src_childof_tag_w_wildcard_n_children_after_set_range(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_entity_t p_1 = ecs_new(world);
    ecs_entity_t p_2 = ecs_new(world);
    ecs_entity_t p_3 = ecs_new(world);

    ecs_entity_t e_1_1 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t e_1_2 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t e_2_1 = ecs_insert(world, ecs_value(EcsParent, {p_2}));
    ecs_entity_t e_2_2 = ecs_insert(world, ecs_value(EcsParent, {p_2}));
    ecs_entity_t e_3_1 = ecs_insert(world, ecs_value(EcsParent, {p_3}));
    ecs_entity_t e_3_2 = ecs_insert(world, ecs_value(EcsParent, {p_3}));
    
    ecs_add(world, e_1_1, Foo);
    ecs_add(world, e_2_1, Foo);
    ecs_add(world, e_3_1, Foo);
    ecs_add(world, e_1_2, Foo);
    ecs_add(world, e_2_2, Foo);
    ecs_add(world, e_3_2, Foo);

    ecs_table_t *table = ecs_get_table(world, e_1_1);
    test_assert(table == ecs_get_table(world, e_2_1));
    test_assert(table == ecs_get_table(world, e_3_1));
    test_assert(table == ecs_get_table(world, e_1_2));
    test_assert(table == ecs_get_table(world, e_2_2));
    test_assert(table == ecs_get_table(world, e_3_2));

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {{ Foo }, { ecs_childof(EcsWildcard) }},
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        {
            ecs_iter_t it = ecs_query_iter(world, q);
            ecs_iter_set_var_as_range(&it, 0, &(ecs_table_range_t){ .table = table, .offset = 0, .count = 2 });
            test_bool(true, ecs_query_next(&it));
            test_int(1, it.count);
            test_uint(e_1_1, it.entities[0]);
            test_uint(ecs_childof(p_1), ecs_field_id(&it, 1));
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 1));

            test_bool(true, ecs_query_next(&it));
            test_int(1, it.count);
            test_uint(e_2_1, it.entities[0]);
            test_uint(ecs_childof(p_2), ecs_field_id(&it, 1));
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 1));

            test_bool(false, ecs_query_next(&it));
        }

        {
            ecs_iter_t it = ecs_query_iter(world, q);
            ecs_iter_set_var_as_range(&it, 0, &(ecs_table_range_t){ .table = table, .offset = 1, .count = 2 });
            test_bool(true, ecs_query_next(&it));
            test_int(1, it.count);
            test_uint(e_2_1, it.entities[0]);
            test_uint(ecs_childof(p_2), ecs_field_id(&it, 1));
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 1));

            test_bool(true, ecs_query_next(&it));
            test_int(1, it.count);
            test_uint(e_3_1, it.entities[0]);
            test_uint(ecs_childof(p_3), ecs_field_id(&it, 1));
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 1));

            test_bool(false, ecs_query_next(&it));
        }

        {
            ecs_iter_t it = ecs_query_iter(world, q);
            ecs_iter_set_var_as_range(&it, 0, &(ecs_table_range_t){ .table = table, .offset = 2, .count = 2 });
            test_bool(true, ecs_query_next(&it));
            test_int(1, it.count);
            test_uint(e_3_1, it.entities[0]);
            test_uint(ecs_childof(p_3), ecs_field_id(&it, 1));
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 1));

            test_bool(true, ecs_query_next(&it));
            test_int(1, it.count);
            test_uint(e_1_2, it.entities[0]);
            test_uint(ecs_childof(p_1), ecs_field_id(&it, 1));
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 1));

            test_bool(false, ecs_query_next(&it));
        }

        {
            ecs_iter_t it = ecs_query_iter(world, q);
            ecs_iter_set_var_as_range(&it, 0, &(ecs_table_range_t){ .table = table, .offset = 3, .count = 2 });
            test_bool(true, ecs_query_next(&it));
            test_int(1, it.count);
            test_uint(e_1_2, it.entities[0]);
            test_uint(ecs_childof(p_1), ecs_field_id(&it, 1));
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 1));

            test_bool(true, ecs_query_next(&it));
            test_int(1, it.count);
            test_uint(e_2_2, it.entities[0]);
            test_uint(ecs_childof(p_2), ecs_field_id(&it, 1));
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 1));

            test_bool(false, ecs_query_next(&it));
        }

        {
            ecs_iter_t it = ecs_query_iter(world, q);
            ecs_iter_set_var_as_range(&it, 0, &(ecs_table_range_t){ .table = table, .offset = 4, .count = 2 });
            test_bool(true, ecs_query_next(&it));
            test_int(1, it.count);
            test_uint(e_2_2, it.entities[0]);
            test_uint(ecs_childof(p_2), ecs_field_id(&it, 1));
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 1));

            test_bool(true, ecs_query_next(&it));
            test_int(1, it.count);
            test_uint(e_3_2, it.entities[0]);
            test_uint(ecs_childof(p_3), ecs_field_id(&it, 1));
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 1));

            test_bool(false, ecs_query_next(&it));
        }

        ecs_query_fini(q);
    }

    ecs_fini(world);
}

void NonFragmentingChildOf_this_src_childof_tag_w_wildcard_n_children_after_set_range_2(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_entity_t p_1 = ecs_new(world);
    ecs_entity_t p_2 = ecs_new(world);
    ecs_entity_t p_3 = ecs_new(world);

    ecs_entity_t e_1_1 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t e_1_2 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t e_2_1 = ecs_insert(world, ecs_value(EcsParent, {p_2}));
    ecs_entity_t e_2_2 = ecs_insert(world, ecs_value(EcsParent, {p_2}));
    ecs_entity_t e_3_1 = ecs_insert(world, ecs_value(EcsParent, {p_3}));
    ecs_entity_t e_3_2 = ecs_insert(world, ecs_value(EcsParent, {p_3}));
    
    ecs_add(world, e_1_1, Foo);
    ecs_add(world, e_2_1, Foo);
    ecs_add(world, e_3_1, Foo);
    ecs_add(world, e_1_2, Foo);
    ecs_add(world, e_2_2, Foo);
    ecs_add(world, e_3_2, Foo);

    ecs_table_t *table = ecs_get_table(world, e_1_1);
    test_assert(table == ecs_get_table(world, e_2_1));
    test_assert(table == ecs_get_table(world, e_3_1));
    test_assert(table == ecs_get_table(world, e_1_2));
    test_assert(table == ecs_get_table(world, e_2_2));
    test_assert(table == ecs_get_table(world, e_3_2));

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {{ Foo }, { ecs_childof(EcsWildcard) }},
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        {
            ecs_iter_t it = ecs_query_iter(world, q);
            ecs_iter_set_var_as_range(&it, 0, &(ecs_table_range_t){ .table = table, .offset = 0, .count = 3 });
            test_bool(true, ecs_query_next(&it));
            test_int(1, it.count);
            test_uint(e_1_1, it.entities[0]);
            test_uint(ecs_childof(p_1), ecs_field_id(&it, 1));
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 1));

            test_bool(true, ecs_query_next(&it));
            test_int(1, it.count);
            test_uint(e_2_1, it.entities[0]);
            test_uint(ecs_childof(p_2), ecs_field_id(&it, 1));
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 1));

            test_bool(true, ecs_query_next(&it));
            test_int(1, it.count);
            test_uint(e_3_1, it.entities[0]);
            test_uint(ecs_childof(p_3), ecs_field_id(&it, 1));
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 1));

            test_bool(false, ecs_query_next(&it));
        }

        {
            ecs_iter_t it = ecs_query_iter(world, q);
            ecs_iter_set_var_as_range(&it, 0, &(ecs_table_range_t){ .table = table, .offset = 1, .count = 3 });
            test_bool(true, ecs_query_next(&it));
            test_int(1, it.count);
            test_uint(e_2_1, it.entities[0]);
            test_uint(ecs_childof(p_2), ecs_field_id(&it, 1));
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 1));

            test_bool(true, ecs_query_next(&it));
            test_int(1, it.count);
            test_uint(e_3_1, it.entities[0]);
            test_uint(ecs_childof(p_3), ecs_field_id(&it, 1));
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 1));

            test_bool(true, ecs_query_next(&it));
            test_int(1, it.count);
            test_uint(e_1_2, it.entities[0]);
            test_uint(ecs_childof(p_1), ecs_field_id(&it, 1));
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 1));

            test_bool(false, ecs_query_next(&it));
        }

        {
            ecs_iter_t it = ecs_query_iter(world, q);
            ecs_iter_set_var_as_range(&it, 0, &(ecs_table_range_t){ .table = table, .offset = 2, .count = 3 });
            test_bool(true, ecs_query_next(&it));
            test_int(1, it.count);
            test_uint(e_3_1, it.entities[0]);
            test_uint(ecs_childof(p_3), ecs_field_id(&it, 1));
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 1));

            test_bool(true, ecs_query_next(&it));
            test_int(1, it.count);
            test_uint(e_1_2, it.entities[0]);
            test_uint(ecs_childof(p_1), ecs_field_id(&it, 1));
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 1));

            test_bool(true, ecs_query_next(&it));
            test_int(1, it.count);
            test_uint(e_2_2, it.entities[0]);
            test_uint(ecs_childof(p_2), ecs_field_id(&it, 1));
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 1));

            test_bool(false, ecs_query_next(&it));
        }

        {
            ecs_iter_t it = ecs_query_iter(world, q);
            ecs_iter_set_var_as_range(&it, 0, &(ecs_table_range_t){ .table = table, .offset = 3, .count = 3 });
            test_bool(true, ecs_query_next(&it));
            test_int(1, it.count);
            test_uint(e_1_2, it.entities[0]);
            test_uint(ecs_childof(p_1), ecs_field_id(&it, 1));
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 1));

            test_bool(true, ecs_query_next(&it));
            test_int(1, it.count);
            test_uint(e_2_2, it.entities[0]);
            test_uint(ecs_childof(p_2), ecs_field_id(&it, 1));
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 1));

            test_bool(true, ecs_query_next(&it));
            test_int(1, it.count);
            test_uint(e_3_2, it.entities[0]);
            test_uint(ecs_childof(p_3), ecs_field_id(&it, 1));
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 1));

            test_bool(false, ecs_query_next(&it));
        }

        ecs_query_fini(q);
    }

    ecs_fini(world);
}

void NonFragmentingChildOf_this_src_childof_tag_w_wildcard_n_children_after_set_range_3(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_entity_t p_1 = ecs_new(world);
    ecs_entity_t p_2 = ecs_new(world);
    ecs_entity_t p_3 = ecs_new(world);

    ecs_entity_t e_1_1 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t e_1_2 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t e_2_1 = ecs_insert(world, ecs_value(EcsParent, {p_2}));
    ecs_entity_t e_2_2 = ecs_insert(world, ecs_value(EcsParent, {p_2}));
    ecs_entity_t e_3_1 = ecs_insert(world, ecs_value(EcsParent, {p_3}));
    ecs_entity_t e_3_2 = ecs_insert(world, ecs_value(EcsParent, {p_3}));
    
    ecs_add(world, e_1_1, Foo);
    ecs_add(world, e_2_1, Foo);
    ecs_add(world, e_3_1, Foo);
    ecs_add(world, e_1_2, Foo);
    ecs_add(world, e_2_2, Foo);
    ecs_add(world, e_3_2, Foo);

    ecs_table_t *table = ecs_get_table(world, e_1_1);
    test_assert(table == ecs_get_table(world, e_2_1));
    test_assert(table == ecs_get_table(world, e_3_1));
    test_assert(table == ecs_get_table(world, e_1_2));
    test_assert(table == ecs_get_table(world, e_2_2));
    test_assert(table == ecs_get_table(world, e_3_2));

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {{ Foo }, { ecs_childof(EcsWildcard) }},
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        {
            ecs_iter_t it = ecs_query_iter(world, q);
            ecs_iter_set_var_as_range(&it, 0, &(ecs_table_range_t){ .table = table, .offset = 0, .count = 4 });
            test_bool(true, ecs_query_next(&it));
            test_int(1, it.count);
            test_uint(e_1_1, it.entities[0]);
            test_uint(ecs_childof(p_1), ecs_field_id(&it, 1));
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 1));

            test_bool(true, ecs_query_next(&it));
            test_int(1, it.count);
            test_uint(e_2_1, it.entities[0]);
            test_uint(ecs_childof(p_2), ecs_field_id(&it, 1));
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 1));

            test_bool(true, ecs_query_next(&it));
            test_int(1, it.count);
            test_uint(e_3_1, it.entities[0]);
            test_uint(ecs_childof(p_3), ecs_field_id(&it, 1));
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 1));

            test_bool(true, ecs_query_next(&it));
            test_int(1, it.count);
            test_uint(e_1_2, it.entities[0]);
            test_uint(ecs_childof(p_1), ecs_field_id(&it, 1));
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 1));

            test_bool(false, ecs_query_next(&it));
        }

        {
            ecs_iter_t it = ecs_query_iter(world, q);
            ecs_iter_set_var_as_range(&it, 0, &(ecs_table_range_t){ .table = table, .offset = 1, .count = 4 });
            test_bool(true, ecs_query_next(&it));
            test_int(1, it.count);
            test_uint(e_2_1, it.entities[0]);
            test_uint(ecs_childof(p_2), ecs_field_id(&it, 1));
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 1));

            test_bool(true, ecs_query_next(&it));
            test_int(1, it.count);
            test_uint(e_3_1, it.entities[0]);
            test_uint(ecs_childof(p_3), ecs_field_id(&it, 1));
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 1));

            test_bool(true, ecs_query_next(&it));
            test_int(1, it.count);
            test_uint(e_1_2, it.entities[0]);
            test_uint(ecs_childof(p_1), ecs_field_id(&it, 1));
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 1));

            test_bool(true, ecs_query_next(&it));
            test_int(1, it.count);
            test_uint(e_2_2, it.entities[0]);
            test_uint(ecs_childof(p_2), ecs_field_id(&it, 1));
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 1));

            test_bool(false, ecs_query_next(&it));
        }

        {
            ecs_iter_t it = ecs_query_iter(world, q);
            ecs_iter_set_var_as_range(&it, 0, &(ecs_table_range_t){ .table = table, .offset = 2, .count = 4 });
            test_bool(true, ecs_query_next(&it));
            test_int(1, it.count);
            test_uint(e_3_1, it.entities[0]);
            test_uint(ecs_childof(p_3), ecs_field_id(&it, 1));
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 1));

            test_bool(true, ecs_query_next(&it));
            test_int(1, it.count);
            test_uint(e_1_2, it.entities[0]);
            test_uint(ecs_childof(p_1), ecs_field_id(&it, 1));
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 1));

            test_bool(true, ecs_query_next(&it));
            test_int(1, it.count);
            test_uint(e_2_2, it.entities[0]);
            test_uint(ecs_childof(p_2), ecs_field_id(&it, 1));
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 1));

            test_bool(true, ecs_query_next(&it));
            test_int(1, it.count);
            test_uint(e_3_2, it.entities[0]);
            test_uint(ecs_childof(p_3), ecs_field_id(&it, 1));
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 1));

            test_bool(false, ecs_query_next(&it));
        }

        ecs_query_fini(q);
    }

    ecs_fini(world);
}

void NonFragmentingChildOf_this_src_childof_tag_w_any_mixed(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_entity_t p_1 = ecs_new(world);
    ecs_entity_t p_2 = ecs_new(world);
    ecs_entity_t p_3 = ecs_new(world);

    /* ecs_entity_t c1 = */ ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t c2 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t c3 = ecs_new_w_pair(world, EcsChildOf, p_1);
    ecs_entity_t c4 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    /* ecs_entity_t c5 = */ ecs_new_w_pair(world, EcsChildOf, p_1);

    /* ecs_entity_t c6 = */ ecs_new_w_pair(world, EcsChildOf, p_2);
    ecs_entity_t c7 = ecs_insert(world, ecs_value(EcsParent, {p_2}));

    /* ecs_entity_t c8 = */ ecs_new_w_pair(world, EcsChildOf, p_3);
    ecs_entity_t c9 = ecs_new_w_pair(world, EcsChildOf, p_3);
    ecs_entity_t c10 = ecs_new_w_pair(world, EcsChildOf, p_3);
    ecs_entity_t c11 = ecs_new_w_pair(world, EcsChildOf, p_3);

    ecs_add(world, c2, Foo);
    ecs_add(world, c3, Foo);
    ecs_add(world, c4, Bar);

    ecs_add(world, c7, Foo);

    ecs_add(world, c9, Foo);
    ecs_add(world, c10, Foo);
    ecs_add(world, c11, Foo);
    ecs_add(world, c11, Bar);

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {
                { Foo },
                { ecs_childof(EcsAny) }
            },
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(2, it.count);
        test_uint(c2, it.entities[0]);
        test_uint(c7, it.entities[1]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_childof(EcsWildcard), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c3, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_childof(EcsWildcard), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(2, it.count);
        test_uint(c9, it.entities[0]);
        test_uint(c10, it.entities[1]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_childof(EcsWildcard), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c11, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_childof(EcsWildcard), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));

        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    ecs_fini(world);
}

void NonFragmentingChildOf_this_src_childof_tag_w_any_single_child(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_entity_t p_1 = ecs_new(world);
    ecs_entity_t p_2 = ecs_new(world);
    ecs_entity_t p_3 = ecs_new(world);

    ecs_entity_t c1 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t c2 = ecs_insert(world, ecs_value(EcsParent, {p_2}));
    ecs_entity_t c3 = ecs_insert(world, ecs_value(EcsParent, {p_3}));

    ecs_add(world, c1, Foo);
    ecs_add(world, c2, Foo);
    ecs_add(world, c3, Foo);

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {
                { Foo },
                { ecs_childof(EcsAny) }
            },
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(3, it.count);
        test_uint(c1, it.entities[0]);
        test_uint(c2, it.entities[1]);
        test_uint(c3, it.entities[2]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_childof(EcsWildcard), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));

        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    ecs_fini(world);
}

void NonFragmentingChildOf_this_src_childof_tag_w_any_single_child_at_offset(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_entity_t p_1 = ecs_new(world);
    ecs_entity_t p_2 = ecs_new(world);
    ecs_entity_t p_3 = ecs_new(world);

    ecs_entity_t c1_1 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t c1_2 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t c2_1 = ecs_insert(world, ecs_value(EcsParent, {p_2}));
    ecs_entity_t c2_2 = ecs_insert(world, ecs_value(EcsParent, {p_2}));
    ecs_entity_t c3_1 = ecs_insert(world, ecs_value(EcsParent, {p_3}));
    ecs_entity_t c3_2 = ecs_insert(world, ecs_value(EcsParent, {p_3}));

    ecs_add(world, c1_1, Foo);
    ecs_add(world, c2_1, Foo);
    ecs_add(world, c3_1, Foo);
    ecs_add(world, c1_2, Foo);
    ecs_add(world, c2_2, Foo);
    ecs_add(world, c3_2, Foo);

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {
                { Foo },
                { ecs_childof(EcsAny) }
            },
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(6, it.count);
        test_uint(c1_1, it.entities[0]);
        test_uint(c2_1, it.entities[1]);
        test_uint(c3_1, it.entities[2]);
        test_uint(c1_2, it.entities[3]);
        test_uint(c2_2, it.entities[4]);
        test_uint(c3_2, it.entities[5]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_childof(EcsWildcard), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));

        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    ecs_fini(world);
}

void NonFragmentingChildOf_this_src_childof_tag_w_any_1_child_after_set_range(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_entity_t p_1 = ecs_new(world);
    ecs_entity_t p_2 = ecs_new(world);
    ecs_entity_t p_3 = ecs_new(world);

    ecs_entity_t e_1 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t e_2 = ecs_insert(world, ecs_value(EcsParent, {p_2}));
    ecs_entity_t e_3 = ecs_insert(world, ecs_value(EcsParent, {p_3}));
    
    ecs_add(world, e_1, Foo);
    ecs_add(world, e_2, Foo);
    ecs_add(world, e_3, Foo);

    ecs_table_t *table = ecs_get_table(world, e_1);
    test_assert(table == ecs_get_table(world, e_2));
    test_assert(table == ecs_get_table(world, e_3));

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {{ Foo }, { ecs_childof(EcsAny) }},
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        {
            ecs_iter_t it = ecs_query_iter(world, q);
            ecs_iter_set_var_as_range(&it, 0, &(ecs_table_range_t){ .table = table, .offset = 0, .count = 2 });
            test_bool(true, ecs_query_next(&it));
            test_int(2, it.count);
            test_uint(e_1, it.entities[0]);
            test_uint(e_2, it.entities[1]);
            test_uint(ecs_childof(EcsWildcard), ecs_field_id(&it, 1));
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 1));

            test_bool(false, ecs_query_next(&it));
        }

        {
            ecs_iter_t it = ecs_query_iter(world, q);
            ecs_iter_set_var_as_range(&it, 0, &(ecs_table_range_t){ .table = table, .offset = 1, .count = 2 });
            test_bool(true, ecs_query_next(&it));
            test_int(2, it.count);
            test_uint(e_2, it.entities[0]);
            test_uint(e_3, it.entities[1]);
            test_uint(ecs_childof(EcsWildcard), ecs_field_id(&it, 1));
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 1));

            test_bool(false, ecs_query_next(&it));
        }

        ecs_query_fini(q);
    }

    ecs_fini(world);
}

void NonFragmentingChildOf_this_src_childof_tag_w_any_n_children_after_set_range(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_entity_t p_1 = ecs_new(world);
    ecs_entity_t p_2 = ecs_new(world);
    ecs_entity_t p_3 = ecs_new(world);

    ecs_entity_t e_1_1 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t e_1_2 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t e_2_1 = ecs_insert(world, ecs_value(EcsParent, {p_2}));
    ecs_entity_t e_2_2 = ecs_insert(world, ecs_value(EcsParent, {p_2}));
    ecs_entity_t e_3_1 = ecs_insert(world, ecs_value(EcsParent, {p_3}));
    ecs_entity_t e_3_2 = ecs_insert(world, ecs_value(EcsParent, {p_3}));
    
    ecs_add(world, e_1_1, Foo);
    ecs_add(world, e_2_1, Foo);
    ecs_add(world, e_3_1, Foo);
    ecs_add(world, e_1_2, Foo);
    ecs_add(world, e_2_2, Foo);
    ecs_add(world, e_3_2, Foo);

    ecs_table_t *table = ecs_get_table(world, e_1_1);
    test_assert(table == ecs_get_table(world, e_2_1));
    test_assert(table == ecs_get_table(world, e_3_1));
    test_assert(table == ecs_get_table(world, e_1_2));
    test_assert(table == ecs_get_table(world, e_2_2));
    test_assert(table == ecs_get_table(world, e_3_2));

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {{ Foo }, { ecs_childof(EcsAny) }},
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        {
            ecs_iter_t it = ecs_query_iter(world, q);
            ecs_iter_set_var_as_range(&it, 0, &(ecs_table_range_t){ .table = table, .offset = 0, .count = 2 });
            test_bool(true, ecs_query_next(&it));
            test_int(2, it.count);
            test_uint(e_1_1, it.entities[0]);
            test_uint(e_2_1, it.entities[1]);
            test_uint(ecs_childof(EcsWildcard), ecs_field_id(&it, 1));
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 1));

            test_bool(false, ecs_query_next(&it));
        }

        {
            ecs_iter_t it = ecs_query_iter(world, q);
            ecs_iter_set_var_as_range(&it, 0, &(ecs_table_range_t){ .table = table, .offset = 1, .count = 2 });
            test_bool(true, ecs_query_next(&it));
            test_int(2, it.count);
            test_uint(e_2_1, it.entities[0]);
            test_uint(e_3_1, it.entities[1]);
            test_uint(ecs_childof(EcsWildcard), ecs_field_id(&it, 1));
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 1));

            test_bool(false, ecs_query_next(&it));
        }

        {
            ecs_iter_t it = ecs_query_iter(world, q);
            ecs_iter_set_var_as_range(&it, 0, &(ecs_table_range_t){ .table = table, .offset = 2, .count = 2 });
            test_bool(true, ecs_query_next(&it));
            test_int(2, it.count);
            test_uint(e_3_1, it.entities[0]);
            test_uint(e_1_2, it.entities[1]);
            test_uint(ecs_childof(EcsWildcard), ecs_field_id(&it, 1));
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 1));

            test_bool(false, ecs_query_next(&it));
        }

        {
            ecs_iter_t it = ecs_query_iter(world, q);
            ecs_iter_set_var_as_range(&it, 0, &(ecs_table_range_t){ .table = table, .offset = 3, .count = 2 });
            test_bool(true, ecs_query_next(&it));
            test_int(2, it.count);
            test_uint(e_1_2, it.entities[0]);
            test_uint(e_2_2, it.entities[1]);
            test_uint(ecs_childof(EcsWildcard), ecs_field_id(&it, 1));
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 1));

            test_bool(false, ecs_query_next(&it));
        }

        {
            ecs_iter_t it = ecs_query_iter(world, q);
            ecs_iter_set_var_as_range(&it, 0, &(ecs_table_range_t){ .table = table, .offset = 4, .count = 2 });
            test_bool(true, ecs_query_next(&it));
            test_int(2, it.count);
            test_uint(e_2_2, it.entities[0]);
            test_uint(e_3_2, it.entities[1]);
            test_uint(ecs_childof(EcsWildcard), ecs_field_id(&it, 1));
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 1));

            test_bool(false, ecs_query_next(&it));
        }

        ecs_query_fini(q);
    }

    ecs_fini(world);
}

void NonFragmentingChildOf_this_src_childof_tag_w_any_n_children_after_set_range_2(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_entity_t p_1 = ecs_new(world);
    ecs_entity_t p_2 = ecs_new(world);
    ecs_entity_t p_3 = ecs_new(world);

    ecs_entity_t e_1_1 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t e_1_2 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t e_2_1 = ecs_insert(world, ecs_value(EcsParent, {p_2}));
    ecs_entity_t e_2_2 = ecs_insert(world, ecs_value(EcsParent, {p_2}));
    ecs_entity_t e_3_1 = ecs_insert(world, ecs_value(EcsParent, {p_3}));
    ecs_entity_t e_3_2 = ecs_insert(world, ecs_value(EcsParent, {p_3}));
    
    ecs_add(world, e_1_1, Foo);
    ecs_add(world, e_2_1, Foo);
    ecs_add(world, e_3_1, Foo);
    ecs_add(world, e_1_2, Foo);
    ecs_add(world, e_2_2, Foo);
    ecs_add(world, e_3_2, Foo);

    ecs_table_t *table = ecs_get_table(world, e_1_1);
    test_assert(table == ecs_get_table(world, e_2_1));
    test_assert(table == ecs_get_table(world, e_3_1));
    test_assert(table == ecs_get_table(world, e_1_2));
    test_assert(table == ecs_get_table(world, e_2_2));
    test_assert(table == ecs_get_table(world, e_3_2));

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {{ Foo }, { ecs_childof(EcsAny) }},
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        {
            ecs_iter_t it = ecs_query_iter(world, q);
            ecs_iter_set_var_as_range(&it, 0, &(ecs_table_range_t){ .table = table, .offset = 0, .count = 3 });
            test_bool(true, ecs_query_next(&it));
            test_int(3, it.count);
            test_uint(e_1_1, it.entities[0]);
            test_uint(e_2_1, it.entities[1]);
            test_uint(e_3_1, it.entities[2]);
            test_uint(ecs_childof(EcsWildcard), ecs_field_id(&it, 1));
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 1));

            test_bool(false, ecs_query_next(&it));
        }

        {
            ecs_iter_t it = ecs_query_iter(world, q);
            ecs_iter_set_var_as_range(&it, 0, &(ecs_table_range_t){ .table = table, .offset = 1, .count = 3 });
            test_bool(true, ecs_query_next(&it));
            test_int(3, it.count);
            test_uint(e_2_1, it.entities[0]);
            test_uint(e_3_1, it.entities[1]);
            test_uint(e_1_2, it.entities[2]);
            test_uint(ecs_childof(EcsWildcard), ecs_field_id(&it, 1));
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 1));

            test_bool(false, ecs_query_next(&it));
        }
    
        {
            ecs_iter_t it = ecs_query_iter(world, q);
            ecs_iter_set_var_as_range(&it, 0, &(ecs_table_range_t){ .table = table, .offset = 2, .count = 3 });
            test_bool(true, ecs_query_next(&it));
            test_int(3, it.count);
            test_uint(e_3_1, it.entities[0]);
            test_uint(e_1_2, it.entities[1]);
            test_uint(e_2_2, it.entities[2]);
            test_uint(ecs_childof(EcsWildcard), ecs_field_id(&it, 1));
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 1));

            test_bool(false, ecs_query_next(&it));
        }

        {
            ecs_iter_t it = ecs_query_iter(world, q);
            ecs_iter_set_var_as_range(&it, 0, &(ecs_table_range_t){ .table = table, .offset = 3, .count = 3 });
            test_bool(true, ecs_query_next(&it));
            test_int(3, it.count);
            test_uint(e_1_2, it.entities[0]);
            test_uint(e_2_2, it.entities[1]);
            test_uint(e_3_2, it.entities[2]);
            test_uint(ecs_childof(EcsWildcard), ecs_field_id(&it, 1));
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 1));

            test_bool(false, ecs_query_next(&it));
        }

        ecs_query_fini(q);
    }

    ecs_fini(world);
}

void NonFragmentingChildOf_this_src_childof_tag_w_any_n_children_after_set_range_3(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_entity_t p_1 = ecs_new(world);
    ecs_entity_t p_2 = ecs_new(world);
    ecs_entity_t p_3 = ecs_new(world);

    ecs_entity_t e_1_1 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t e_1_2 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t e_2_1 = ecs_insert(world, ecs_value(EcsParent, {p_2}));
    ecs_entity_t e_2_2 = ecs_insert(world, ecs_value(EcsParent, {p_2}));
    ecs_entity_t e_3_1 = ecs_insert(world, ecs_value(EcsParent, {p_3}));
    ecs_entity_t e_3_2 = ecs_insert(world, ecs_value(EcsParent, {p_3}));
    
    ecs_add(world, e_1_1, Foo);
    ecs_add(world, e_2_1, Foo);
    ecs_add(world, e_3_1, Foo);
    ecs_add(world, e_1_2, Foo);
    ecs_add(world, e_2_2, Foo);
    ecs_add(world, e_3_2, Foo);

    ecs_table_t *table = ecs_get_table(world, e_1_1);
    test_assert(table == ecs_get_table(world, e_2_1));
    test_assert(table == ecs_get_table(world, e_3_1));
    test_assert(table == ecs_get_table(world, e_1_2));
    test_assert(table == ecs_get_table(world, e_2_2));
    test_assert(table == ecs_get_table(world, e_3_2));

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {{ Foo }, { ecs_childof(EcsAny) }},
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        {
            ecs_iter_t it = ecs_query_iter(world, q);
            ecs_iter_set_var_as_range(&it, 0, &(ecs_table_range_t){ .table = table, .offset = 0, .count = 4 });
            test_bool(true, ecs_query_next(&it));
            test_int(4, it.count);
            test_uint(e_1_1, it.entities[0]);
            test_uint(e_2_1, it.entities[1]);
            test_uint(e_3_1, it.entities[2]);
            test_uint(e_1_2, it.entities[3]);
            test_uint(ecs_childof(EcsWildcard), ecs_field_id(&it, 1));
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 1));

            test_bool(false, ecs_query_next(&it));
        }

        {
            ecs_iter_t it = ecs_query_iter(world, q);
            ecs_iter_set_var_as_range(&it, 0, &(ecs_table_range_t){ .table = table, .offset = 1, .count = 4 });
            test_bool(true, ecs_query_next(&it));
            test_int(4, it.count);
            test_uint(e_2_1, it.entities[0]);
            test_uint(e_3_1, it.entities[1]);
            test_uint(e_1_2, it.entities[2]);
            test_uint(e_2_2, it.entities[3]);
            test_uint(ecs_childof(EcsWildcard), ecs_field_id(&it, 1));
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 1));

            test_bool(false, ecs_query_next(&it));
        }

        {
            ecs_iter_t it = ecs_query_iter(world, q);
            ecs_iter_set_var_as_range(&it, 0, &(ecs_table_range_t){ .table = table, .offset = 2, .count = 4 });
            test_bool(true, ecs_query_next(&it));
            test_int(4, it.count);
            test_uint(e_3_1, it.entities[0]);
            test_uint(e_1_2, it.entities[1]);
            test_uint(e_2_2, it.entities[2]);
            test_uint(e_3_2, it.entities[3]);
            test_uint(ecs_childof(EcsWildcard), ecs_field_id(&it, 1));
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 1));

            test_bool(false, ecs_query_next(&it));
        }

        ecs_query_fini(q);
    }

    ecs_fini(world);
}

void NonFragmentingChildOf_this_src_childof_tag_w_parent_single_child_table(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_entity_t p_1 = ecs_new(world);
    ecs_entity_t p_2 = ecs_new(world);
    ecs_entity_t p_3 = ecs_new(world);

    /* ecs_entity_t c1 = */ ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t c2 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t c3 = ecs_new_w_pair(world, EcsChildOf, p_1);
    ecs_entity_t c4 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    /* ecs_entity_t c5 = */ ecs_new_w_pair(world, EcsChildOf, p_1);

    /* ecs_entity_t c6 = */ ecs_new_w_pair(world, EcsChildOf, p_2);
    /* ecs_entity_t c7 = */ ecs_insert(world, ecs_value(EcsParent, {p_2}));

    /* ecs_entity_t c8 = */ ecs_new_w_pair(world, EcsChildOf, p_3);
    ecs_entity_t c9 = ecs_new_w_pair(world, EcsChildOf, p_3);
    ecs_entity_t c10 = ecs_new_w_pair(world, EcsChildOf, p_3);
    ecs_entity_t c11 = ecs_new_w_pair(world, EcsChildOf, p_3);

    ecs_add(world, c2, Foo);
    ecs_add(world, c3, Foo);
    ecs_add(world, c4, Bar);

    ecs_add(world, c9, Foo);
    ecs_add(world, c10, Foo);
    ecs_add(world, c11, Foo);
    ecs_add(world, c11, Bar);

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {
                { Foo },
                { ecs_childof(p_1) }
            },
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c2, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_childof(p_1), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c3, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_childof(p_1), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));

        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {
                { Foo },
                { ecs_childof(p_2) }
            },
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {
                { Foo },
                { ecs_childof(p_3) }
            },
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(2, it.count);
        test_uint(c9, it.entities[0]);
        test_uint(c10, it.entities[1]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_childof(p_3), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c11, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_childof(p_3), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));

        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    ecs_fini(world);
}

void NonFragmentingChildOf_this_src_childof_component_w_parent(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Bar);

    ecs_entity_t p_1 = ecs_new(world);
    ecs_entity_t p_2 = ecs_new(world);
    ecs_entity_t p_3 = ecs_new(world);

    /* ecs_entity_t c1 = */ ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t c2 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t c3 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t c4 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    /* ecs_entity_t c5 = */ ecs_new_w_pair(world, EcsChildOf, p_1);

    /* ecs_entity_t c6 = */ ecs_new_w_pair(world, EcsChildOf, p_2);
    /* ecs_entity_t c7 = */ ecs_insert(world, ecs_value(EcsParent, {p_2}));

    /* ecs_entity_t c8 = */ ecs_new_w_pair(world, EcsChildOf, p_3);
    ecs_entity_t c9 = ecs_new_w_pair(world, EcsChildOf, p_3);
    ecs_entity_t c10 = ecs_new_w_pair(world, EcsChildOf, p_3);
    ecs_entity_t c11 = ecs_new_w_pair(world, EcsChildOf, p_3);

    ecs_set(world, c2, Position, {1, 2});
    ecs_set(world, c3, Position, {2, 3});
    ecs_add(world, c4, Bar);

    ecs_set(world, c9, Position, {3, 4});
    ecs_set(world, c10, Position, {4, 5});
    ecs_set(world, c11, Position, {5, 6});
    ecs_add(world, c11, Bar);

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {
                { ecs_id(Position) },
                { ecs_childof(p_1) }
            },
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c2, it.entities[0]);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(ecs_childof(p_1), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 1);
            test_int(p->y, 2);
        }

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c3, it.entities[0]);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(ecs_childof(p_1), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 2);
            test_int(p->y, 3);
        }

        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {
                { ecs_id(Position) },
                { ecs_childof(p_2) }
            },
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {
                { ecs_id(Position) },
                { ecs_childof(p_3) }
            },
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(2, it.count);
        test_uint(c9, it.entities[0]);
        test_uint(c10, it.entities[1]);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(ecs_childof(p_3), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p[0].x, 3); test_int(p[0].y, 4);
            test_int(p[1].x, 4); test_int(p[1].y, 5);
        }

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c11, it.entities[0]);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(ecs_childof(p_3), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 5);
            test_int(p->y, 6);
        }

        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    ecs_fini(world);
}

void NonFragmentingChildOf_this_src_childof_component_w_parent_single_child(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Bar);

    ecs_entity_t p_1 = ecs_new(world);
    ecs_entity_t p_2 = ecs_new(world);
    ecs_entity_t p_3 = ecs_new(world);

    /* ecs_entity_t c1 = */ ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t c2 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t c3 = ecs_new_w_pair(world, EcsChildOf, p_1);
    ecs_entity_t c4 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    /* ecs_entity_t c5 = */ ecs_new_w_pair(world, EcsChildOf, p_1);

    /* ecs_entity_t c6 = */ ecs_new_w_pair(world, EcsChildOf, p_2);
    ecs_entity_t c7 = ecs_insert(world, ecs_value(EcsParent, {p_2}));

    /* ecs_entity_t c8 = */ ecs_new_w_pair(world, EcsChildOf, p_3);
    ecs_entity_t c9 = ecs_new_w_pair(world, EcsChildOf, p_3);
    ecs_entity_t c10 = ecs_new_w_pair(world, EcsChildOf, p_3);
    ecs_entity_t c11 = ecs_new_w_pair(world, EcsChildOf, p_3);

    ecs_set(world, c2, Position, {1, 2});
    ecs_set(world, c3, Position, {2, 3});
    ecs_add(world, c4, Bar);

    ecs_set(world, c7, Position, {2, 3});

    ecs_set(world, c9, Position, {3, 4});
    ecs_set(world, c10, Position, {4, 5});
    ecs_set(world, c11, Position, {5, 6});
    ecs_add(world, c11, Bar);

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {
                { ecs_id(Position) },
                { ecs_childof(p_1) }
            },
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c2, it.entities[0]);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(ecs_childof(p_1), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 1);
            test_int(p->y, 2);
        }

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c3, it.entities[0]);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(ecs_childof(p_1), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 2);
            test_int(p->y, 3);
        }

        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {
                { ecs_childof(p_2) },
                { ecs_id(Position) }
            },
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c7, it.entities[0]);
        test_uint(ecs_childof(p_2), ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        {
            Position *p = ecs_field(&it, Position, 1);
            test_assert(p != NULL);
            test_int(p[0].x, 2); test_int(p[0].y, 3);
        }

        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {
                { ecs_childof(p_3) },
                { ecs_id(Position) }
            },
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(2, it.count);
        test_uint(c9, it.entities[0]);
        test_uint(c10, it.entities[1]);
        test_uint(ecs_childof(p_3), ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        {
            Position *p = ecs_field(&it, Position, 1);
            test_assert(p != NULL);
            test_int(p[0].x, 3); test_int(p[0].y, 4);
            test_int(p[1].x, 4); test_int(p[1].y, 5);
        }

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c11, it.entities[0]);
        test_uint(ecs_childof(p_3), ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        {
            Position *p = ecs_field(&it, Position, 1);
            test_assert(p != NULL);
            test_int(p->x, 5);
            test_int(p->y, 6);
        }

        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    ecs_fini(world);
}

void NonFragmentingChildOf_this_src_childof_component_w_parent_single_child_table(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Bar);

    ecs_entity_t p_1 = ecs_new(world);
    ecs_entity_t p_2 = ecs_new(world);
    ecs_entity_t p_3 = ecs_new(world);

    /* ecs_entity_t c1 = */ ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t c2 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t c3 = ecs_new_w_pair(world, EcsChildOf, p_1);
    ecs_entity_t c4 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    /* ecs_entity_t c5 = */ ecs_new_w_pair(world, EcsChildOf, p_1);

    /* ecs_entity_t c6 = */ ecs_new_w_pair(world, EcsChildOf, p_2);
    /* ecs_entity_t c7 = */ ecs_insert(world, ecs_value(EcsParent, {p_2}));

    /* ecs_entity_t c8 = */ ecs_new_w_pair(world, EcsChildOf, p_3);
    ecs_entity_t c9 = ecs_new_w_pair(world, EcsChildOf, p_3);
    ecs_entity_t c10 = ecs_new_w_pair(world, EcsChildOf, p_3);
    ecs_entity_t c11 = ecs_new_w_pair(world, EcsChildOf, p_3);

    ecs_set(world, c2, Position, {1, 2});
    ecs_set(world, c3, Position, {2, 3});
    ecs_add(world, c4, Bar);

    ecs_set(world, c9, Position, {3, 4});
    ecs_set(world, c10, Position, {4, 5});
    ecs_set(world, c11, Position, {5, 6});
    ecs_add(world, c11, Bar);

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {
                { ecs_id(Position) },
                { ecs_childof(p_1) }
            },
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c2, it.entities[0]);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(ecs_childof(p_1), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 1);
            test_int(p->y, 2);
        }

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c3, it.entities[0]);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(ecs_childof(p_1), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 2);
            test_int(p->y, 3);
        }

        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {
                { ecs_id(Position) },
                { ecs_childof(p_2) }
            },
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {
                { ecs_id(Position) },
                { ecs_childof(p_3) }
            },
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(2, it.count);
        test_uint(c9, it.entities[0]);
        test_uint(c10, it.entities[1]);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(ecs_childof(p_3), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p[0].x, 3); test_int(p[0].y, 4);
            test_int(p[1].x, 4); test_int(p[1].y, 5);
        }

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c11, it.entities[0]);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(ecs_childof(p_3), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 5);
            test_int(p->y, 6);
        }

        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    ecs_fini(world);
}

void NonFragmentingChildOf_this_src_childof_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_entity_t p_1 = ecs_new(world);
    ecs_entity_t p_2 = ecs_new(world);
    ecs_entity_t p_3 = ecs_new(world);

    ecs_entity_t c1 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t c2 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t c3 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t c4 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t c5 = ecs_new_w_pair(world, EcsChildOf, p_1);

    ecs_entity_t c6 = ecs_new_w_pair(world, EcsChildOf, p_2);
    ecs_entity_t c7 = ecs_insert(world, ecs_value(EcsParent, {p_2}));

    ecs_entity_t c8 = ecs_new_w_pair(world, EcsChildOf, p_3);
    ecs_entity_t c9 = ecs_new_w_pair(world, EcsChildOf, p_3);
    ecs_entity_t c10 = ecs_new_w_pair(world, EcsChildOf, p_3);
    ecs_entity_t c11 = ecs_new_w_pair(world, EcsChildOf, p_3);

    ecs_add(world, c2, Foo);
    ecs_add(world, c3, Foo);
    ecs_add(world, c4, Bar);

    ecs_add(world, c9, Foo);
    ecs_add(world, c10, Foo);
    ecs_add(world, c11, Bar);

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {
                { ecs_childof(EcsWildcard) }
            },
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c8, it.entities[0]);
        test_uint(ecs_childof(p_3), ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_int(2, it.count);
        test_uint(c9, it.entities[0]);
        test_uint(c10, it.entities[1]);
        test_uint(ecs_childof(p_3), ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c11, it.entities[0]);
        test_uint(ecs_childof(p_3), ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_int(2, it.count);
        test_uint(c6, it.entities[0]);
        test_uint(c7, it.entities[1]);
        test_uint(ecs_childof(p_2), ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_int(5, it.count);
        test_uint(c1, it.entities[0]);
        test_uint(c2, it.entities[1]);
        test_uint(c3, it.entities[2]);
        test_uint(c4, it.entities[3]);
        test_uint(c5, it.entities[4]);
        test_uint(ecs_childof(p_1), ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));

        /* Skip builtin entities */
        while (ecs_query_next(&it)) {
            test_assert(!ecs_table_has_id(world, it.table, EcsNotQueryable));
        }

        ecs_query_fini(q);
    }

    ecs_fini(world);
}

void NonFragmentingChildOf_this_src_childof_wildcard_w_tag(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_entity_t p_1 = ecs_new(world);
    ecs_entity_t p_2 = ecs_new(world);
    ecs_entity_t p_3 = ecs_new(world);

    /* ecs_entity_t c1 = */ ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t c2 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t c3 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t c4 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    /* ecs_entity_t c5 = */ ecs_new_w_pair(world, EcsChildOf, p_1);

    /* ecs_entity_t c6 = */ ecs_new_w_pair(world, EcsChildOf, p_2);
    /* ecs_entity_t c7 = */ ecs_insert(world, ecs_value(EcsParent, {p_2}));

    /* ecs_entity_t c8 = */ ecs_new_w_pair(world, EcsChildOf, p_3);
    ecs_entity_t c9 = ecs_new_w_pair(world, EcsChildOf, p_3);
    ecs_entity_t c10 = ecs_new_w_pair(world, EcsChildOf, p_3);
    ecs_entity_t c11 = ecs_new_w_pair(world, EcsChildOf, p_3);

    ecs_add(world, c2, Foo);
    ecs_add(world, c3, Foo);
    ecs_add(world, c4, Foo);
    ecs_add(world, c4, Bar);

    ecs_add(world, c9, Foo);
    ecs_add(world, c10, Foo);
    ecs_add(world, c11, Foo);
    ecs_add(world, c11, Bar);

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {
                { ecs_childof(EcsWildcard) },
                { Foo }
            },
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);

        if (cache_kind == EcsQueryCacheAuto) {
            test_bool(true, ecs_query_next(&it));
            test_int(1, it.count);
            test_uint(c2, it.entities[0]);
            test_uint(ecs_childof(p_1), ecs_field_id(&it, 0));
            test_uint(Foo, ecs_field_id(&it, 1));
            test_bool(true, ecs_field_is_set(&it, 0));
            test_bool(true, ecs_field_is_set(&it, 1));
            
            test_bool(true, ecs_query_next(&it));
            test_int(1, it.count);
            test_uint(c3, it.entities[0]);
            test_uint(ecs_childof(p_1), ecs_field_id(&it, 0));
            test_uint(Foo, ecs_field_id(&it, 1));
            test_bool(true, ecs_field_is_set(&it, 0));
            test_bool(true, ecs_field_is_set(&it, 1));

            test_bool(true, ecs_query_next(&it));
            test_int(1, it.count);
            test_uint(c4, it.entities[0]);
            test_uint(ecs_childof(p_1), ecs_field_id(&it, 0));
            test_uint(Foo, ecs_field_id(&it, 1));
            test_bool(true, ecs_field_is_set(&it, 0));
            test_bool(true, ecs_field_is_set(&it, 1));

            test_bool(true, ecs_query_next(&it));
            test_int(2, it.count);
            test_uint(c9, it.entities[0]);
            test_uint(c10, it.entities[1]);
            test_uint(ecs_childof(p_3), ecs_field_id(&it, 0));
            test_uint(Foo, ecs_field_id(&it, 1));
            test_bool(true, ecs_field_is_set(&it, 0));
            test_bool(true, ecs_field_is_set(&it, 1));

            test_bool(true, ecs_query_next(&it));
            test_int(1, it.count);
            test_uint(c11, it.entities[0]);
            test_uint(ecs_childof(p_3), ecs_field_id(&it, 0));
            test_uint(Foo, ecs_field_id(&it, 1));
            test_bool(true, ecs_field_is_set(&it, 0));
            test_bool(true, ecs_field_is_set(&it, 1));
        } else {
            test_bool(true, ecs_query_next(&it));
            test_int(2, it.count);
            test_uint(c9, it.entities[0]);
            test_uint(c10, it.entities[1]);
            test_uint(ecs_childof(p_3), ecs_field_id(&it, 0));
            test_uint(Foo, ecs_field_id(&it, 1));
            test_bool(true, ecs_field_is_set(&it, 0));
            test_bool(true, ecs_field_is_set(&it, 1));

            test_bool(true, ecs_query_next(&it));
            test_int(1, it.count);
            test_uint(c11, it.entities[0]);
            test_uint(ecs_childof(p_3), ecs_field_id(&it, 0));
            test_uint(Foo, ecs_field_id(&it, 1));
            test_bool(true, ecs_field_is_set(&it, 0));
            test_bool(true, ecs_field_is_set(&it, 1));

            test_bool(true, ecs_query_next(&it));
            test_int(1, it.count);
            test_uint(c2, it.entities[0]);
            test_uint(ecs_childof(p_1), ecs_field_id(&it, 0));
            test_uint(Foo, ecs_field_id(&it, 1));
            test_bool(true, ecs_field_is_set(&it, 0));
            test_bool(true, ecs_field_is_set(&it, 1));

            test_bool(true, ecs_query_next(&it));
            test_int(1, it.count);
            test_uint(c3, it.entities[0]);
            test_uint(ecs_childof(p_1), ecs_field_id(&it, 0));
            test_uint(Foo, ecs_field_id(&it, 1));
            test_bool(true, ecs_field_is_set(&it, 0));
            test_bool(true, ecs_field_is_set(&it, 1));

            test_bool(true, ecs_query_next(&it));
            test_int(1, it.count);
            test_uint(c4, it.entities[0]);
            test_uint(ecs_childof(p_1), ecs_field_id(&it, 0));
            test_uint(Foo, ecs_field_id(&it, 1));
            test_bool(true, ecs_field_is_set(&it, 0));
            test_bool(true, ecs_field_is_set(&it, 1));
        }

        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    ecs_fini(world);
}

void NonFragmentingChildOf_this_src_childof_wildcard_w_component(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Bar);

    ecs_entity_t p_1 = ecs_new(world);
    ecs_entity_t p_2 = ecs_new(world);
    ecs_entity_t p_3 = ecs_new(world);

    /* ecs_entity_t c1 = */ ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t c2 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t c3 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t c4 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    /* ecs_entity_t c5 = */ ecs_new_w_pair(world, EcsChildOf, p_1);

    /* ecs_entity_t c6 = */ ecs_new_w_pair(world, EcsChildOf, p_2);
    /* ecs_entity_t c7 = */ ecs_insert(world, ecs_value(EcsParent, {p_2}));

    /* ecs_entity_t c8 = */ ecs_new_w_pair(world, EcsChildOf, p_3);
    ecs_entity_t c9 = ecs_new_w_pair(world, EcsChildOf, p_3);
    ecs_entity_t c10 = ecs_new_w_pair(world, EcsChildOf, p_3);
    ecs_entity_t c11 = ecs_new_w_pair(world, EcsChildOf, p_3);

    ecs_set(world, c2, Position, {1, 2});
    ecs_set(world, c3, Position, {2, 3});
    ecs_set(world, c4, Position, {3, 4});
    ecs_add(world, c4, Bar);

    ecs_set(world, c9, Position, {5, 6});
    ecs_set(world, c10, Position, {6, 7});
    ecs_set(world, c11, Position, {7, 8});
    ecs_add(world, c11, Bar);

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {
                { ecs_childof(EcsWildcard) },
                { ecs_id(Position) }
            },
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);

        if (cache_kind == EcsQueryCacheAuto) {
            test_bool(true, ecs_query_next(&it));
            test_int(1, it.count);
            test_uint(c2, it.entities[0]);
            test_uint(ecs_childof(p_1), ecs_field_id(&it, 0));
            test_uint(ecs_id(Position), ecs_field_id(&it, 1));
            test_bool(true, ecs_field_is_set(&it, 0));
            test_bool(true, ecs_field_is_set(&it, 1));
            {
                Position *p = ecs_field(&it, Position, 1);
                test_int(p[0].x, 1); test_int(p[0].y, 2);
            }

            test_bool(true, ecs_query_next(&it));
            test_int(1, it.count);
            test_uint(c3, it.entities[0]);
            test_uint(ecs_childof(p_1), ecs_field_id(&it, 0));
            test_uint(ecs_id(Position), ecs_field_id(&it, 1));
            test_bool(true, ecs_field_is_set(&it, 0));
            test_bool(true, ecs_field_is_set(&it, 1));
            {
                Position *p = ecs_field(&it, Position, 1);
                test_int(p[0].x, 2); test_int(p[0].y, 3);
            }

            test_bool(true, ecs_query_next(&it));
            test_int(1, it.count);
            test_uint(c4, it.entities[0]);
            test_uint(ecs_childof(p_1), ecs_field_id(&it, 0));
            test_uint(ecs_id(Position), ecs_field_id(&it, 1));
            test_bool(true, ecs_field_is_set(&it, 0));
            test_bool(true, ecs_field_is_set(&it, 1));
            {
                Position *p = ecs_field(&it, Position, 1);
                test_int(p[0].x, 3); test_int(p[0].y, 4);
            }

            test_bool(true, ecs_query_next(&it));
            test_int(2, it.count);
            test_uint(c9, it.entities[0]);
            test_uint(c10, it.entities[1]);
            test_uint(ecs_childof(p_3), ecs_field_id(&it, 0));
            test_uint(ecs_id(Position), ecs_field_id(&it, 1));
            test_bool(true, ecs_field_is_set(&it, 0));
            test_bool(true, ecs_field_is_set(&it, 1));
            {
                Position *p = ecs_field(&it, Position, 1);
                test_int(p[0].x, 5); test_int(p[0].y, 6);
                test_int(p[1].x, 6); test_int(p[1].y, 7);
            }

            test_bool(true, ecs_query_next(&it));
            test_int(1, it.count);
            test_uint(c11, it.entities[0]);
            test_uint(ecs_childof(p_3), ecs_field_id(&it, 0));
            test_uint(ecs_id(Position), ecs_field_id(&it, 1));
            test_bool(true, ecs_field_is_set(&it, 0));
            test_bool(true, ecs_field_is_set(&it, 1));
            {
                Position *p = ecs_field(&it, Position, 1);
                test_int(p[0].x, 7); test_int(p[0].y, 8);
            }
        } else {
            test_bool(true, ecs_query_next(&it));
            test_int(2, it.count);
            test_uint(c9, it.entities[0]);
            test_uint(c10, it.entities[1]);
            test_uint(ecs_childof(p_3), ecs_field_id(&it, 0));
            test_uint(ecs_id(Position), ecs_field_id(&it, 1));
            test_bool(true, ecs_field_is_set(&it, 0));
            test_bool(true, ecs_field_is_set(&it, 1));
            {
                Position *p = ecs_field(&it, Position, 1);
                test_int(p[0].x, 5); test_int(p[0].y, 6);
                test_int(p[1].x, 6); test_int(p[1].y, 7);
            }

            test_bool(true, ecs_query_next(&it));
            test_int(1, it.count);
            test_uint(c11, it.entities[0]);
            test_uint(ecs_childof(p_3), ecs_field_id(&it, 0));
            test_uint(ecs_id(Position), ecs_field_id(&it, 1));
            test_bool(true, ecs_field_is_set(&it, 0));
            test_bool(true, ecs_field_is_set(&it, 1));
            {
                Position *p = ecs_field(&it, Position, 1);
                test_int(p[0].x, 7); test_int(p[0].y, 8);
            }

            test_bool(true, ecs_query_next(&it));
            test_int(1, it.count);
            test_uint(c2, it.entities[0]);
            test_uint(ecs_childof(p_1), ecs_field_id(&it, 0));
            test_uint(ecs_id(Position), ecs_field_id(&it, 1));
            test_bool(true, ecs_field_is_set(&it, 0));
            test_bool(true, ecs_field_is_set(&it, 1));
            {
                Position *p = ecs_field(&it, Position, 1);
                test_int(p[0].x, 1); test_int(p[0].y, 2);
            }

            test_bool(true, ecs_query_next(&it));
            test_int(1, it.count);
            test_uint(c3, it.entities[0]);
            test_uint(ecs_childof(p_1), ecs_field_id(&it, 0));
            test_uint(ecs_id(Position), ecs_field_id(&it, 1));
            test_bool(true, ecs_field_is_set(&it, 0));
            test_bool(true, ecs_field_is_set(&it, 1));
            {
                Position *p = ecs_field(&it, Position, 1);
                test_int(p[0].x, 2); test_int(p[0].y, 3);
            }

            test_bool(true, ecs_query_next(&it));
            test_int(1, it.count);
            test_uint(c4, it.entities[0]);
            test_uint(ecs_childof(p_1), ecs_field_id(&it, 0));
            test_uint(ecs_id(Position), ecs_field_id(&it, 1));
            test_bool(true, ecs_field_is_set(&it, 0));
            test_bool(true, ecs_field_is_set(&it, 1));
            {
                Position *p = ecs_field(&it, Position, 1);
                test_int(p[0].x, 3); test_int(p[0].y, 4);
            }
        }

        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    ecs_fini(world);
}

static
bool query_skip_builtin(ecs_world_t *world, ecs_iter_t *it) {
    while (ecs_query_next(it)) {
        if (!ecs_table_has_flags(it->table, EcsTableHasBuiltins)) {
            if (!ecs_table_has_id(world, it->table, ecs_childof(EcsFlecs))) {
                return true;
            }
        }
        test_assert(!ecs_table_has_id(world, it->table, EcsNotQueryable));
        test_uint(ecs_childof(EcsWildcard), ecs_field_id(it, 0));
    }
    return false;
}

void NonFragmentingChildOf_this_src_childof_any(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_entity_t p_1 = ecs_new(world);
    ecs_entity_t p_2 = ecs_new(world);
    ecs_entity_t p_3 = ecs_new(world);

    ecs_entity_t c1 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t c2 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t c3 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t c4 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t c5 = ecs_new_w_pair(world, EcsChildOf, p_1);

    ecs_entity_t c6 = ecs_new_w_pair(world, EcsChildOf, p_2);
    ecs_entity_t c7 = ecs_insert(world, ecs_value(EcsParent, {p_2}));

    ecs_entity_t c8 = ecs_new_w_pair(world, EcsChildOf, p_3);
    ecs_entity_t c9 = ecs_new_w_pair(world, EcsChildOf, p_3);
    ecs_entity_t c10 = ecs_new_w_pair(world, EcsChildOf, p_3);
    ecs_entity_t c11 = ecs_new_w_pair(world, EcsChildOf, p_3);

    ecs_add(world, c2, Foo);
    ecs_add(world, c3, Foo);
    ecs_add(world, c4, Bar);

    ecs_add(world, c9, Foo);
    ecs_add(world, c10, Foo);
    ecs_add(world, c11, Bar);

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {
                { ecs_childof(EcsAny) }
            },
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(2, it.count);
        test_uint(c1, it.entities[0]);
        test_uint(c7, it.entities[1]);
        test_uint(ecs_childof(EcsWildcard), ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_int(2, it.count);
        test_uint(c2, it.entities[0]);
        test_uint(c3, it.entities[1]);
        test_uint(ecs_childof(EcsWildcard), ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c4, it.entities[0]);
        test_uint(ecs_childof(EcsWildcard), ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));

        test_bool(true, query_skip_builtin(world, &it));

        test_int(1, it.count);
        test_uint(c5, it.entities[0]);
        test_uint(ecs_childof(EcsWildcard), ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));

        test_bool(true, query_skip_builtin(world, &it));

        test_int(1, it.count);
        test_uint(c6, it.entities[0]);
        test_uint(ecs_childof(EcsWildcard), ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));

        test_bool(true, query_skip_builtin(world, &it));

        test_int(1, it.count);
        test_uint(c8, it.entities[0]);
        test_uint(ecs_childof(EcsWildcard), ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));

        test_bool(true, query_skip_builtin(world, &it));

        test_int(2, it.count);
        test_uint(c9, it.entities[0]);
        test_uint(c10, it.entities[1]);
        test_uint(ecs_childof(EcsWildcard), ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));

        test_bool(true, query_skip_builtin(world, &it));

        test_int(1, it.count);
        test_uint(c11, it.entities[0]);
        test_uint(ecs_childof(EcsWildcard), ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));

        test_bool(false, query_skip_builtin(world, &it));

        ecs_query_fini(q);
    }

    ecs_fini(world);
}

void NonFragmentingChildOf_this_src_childof_any_w_tag(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_entity_t p_1 = ecs_new(world);
    ecs_entity_t p_2 = ecs_new(world);
    ecs_entity_t p_3 = ecs_new(world);

    /* ecs_entity_t c1 = */ ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t c2 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t c3 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t c4 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    /* ecs_entity_t c5 = */ ecs_new_w_pair(world, EcsChildOf, p_1);

    /* ecs_entity_t c6 = */ ecs_new_w_pair(world, EcsChildOf, p_2);
    /* ecs_entity_t c7 = */ ecs_insert(world, ecs_value(EcsParent, {p_2}));

    /* ecs_entity_t c8 = */ ecs_new_w_pair(world, EcsChildOf, p_3);
    ecs_entity_t c9 = ecs_new_w_pair(world, EcsChildOf, p_3);
    ecs_entity_t c10 = ecs_new_w_pair(world, EcsChildOf, p_3);
    ecs_entity_t c11 = ecs_new_w_pair(world, EcsChildOf, p_3);

    ecs_add(world, c2, Foo);
    ecs_add(world, c3, Foo);
    ecs_add(world, c4, Foo);
    ecs_add(world, c4, Bar);

    ecs_add(world, c9, Foo);
    ecs_add(world, c10, Foo);
    ecs_add(world, c11, Foo);
    ecs_add(world, c11, Bar);

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {
                { ecs_childof(EcsAny) },
                { Foo }
            },
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(2, it.count);
        test_uint(c2, it.entities[0]);
        test_uint(c3, it.entities[1]);
        test_uint(ecs_childof(EcsWildcard), ecs_field_id(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c4, it.entities[0]);
        test_uint(ecs_childof(EcsWildcard), ecs_field_id(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));

        test_bool(true, query_skip_builtin(world, &it));

        test_int(2, it.count);
        test_uint(c9, it.entities[0]);
        test_uint(c10, it.entities[1]);
        test_uint(ecs_childof(EcsWildcard), ecs_field_id(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));

        test_bool(true, query_skip_builtin(world, &it));

        test_int(1, it.count);
        test_uint(c11, it.entities[0]);
        test_uint(ecs_childof(EcsWildcard), ecs_field_id(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));

        test_bool(false, query_skip_builtin(world, &it));

        ecs_query_fini(q);
    }

    ecs_fini(world);
}

void NonFragmentingChildOf_this_src_childof_any_w_component(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Bar);

    ecs_entity_t p_1 = ecs_new(world);
    ecs_entity_t p_2 = ecs_new(world);
    ecs_entity_t p_3 = ecs_new(world);

    /* ecs_entity_t c1 = */ ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t c2 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t c3 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t c4 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    /* ecs_entity_t c5 = */ ecs_new_w_pair(world, EcsChildOf, p_1);

    /* ecs_entity_t c6 = */ ecs_new_w_pair(world, EcsChildOf, p_2);
    /* ecs_entity_t c7 = */ ecs_insert(world, ecs_value(EcsParent, {p_2}));

    /* ecs_entity_t c8 = */ ecs_new_w_pair(world, EcsChildOf, p_3);
    ecs_entity_t c9 = ecs_new_w_pair(world, EcsChildOf, p_3);
    ecs_entity_t c10 = ecs_new_w_pair(world, EcsChildOf, p_3);
    ecs_entity_t c11 = ecs_new_w_pair(world, EcsChildOf, p_3);

    ecs_set(world, c2, Position, {1, 2});
    ecs_set(world, c3, Position, {2, 3});
    ecs_set(world, c4, Position, {3, 4});
    ecs_add(world, c4, Bar);

    ecs_set(world, c9, Position, {5, 6});
    ecs_set(world, c10, Position, {6, 7});
    ecs_set(world, c11, Position, {7, 8});
    ecs_add(world, c11, Bar);

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {
                { ecs_childof(EcsAny) },
                { ecs_id(Position) }
            },
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(2, it.count);
        test_uint(c2, it.entities[0]);
        test_uint(c3, it.entities[1]);
        test_uint(ecs_childof(EcsWildcard), ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        {
            Position *p = ecs_field(&it, Position, 1);
            test_int(p[0].x, 1); test_int(p[0].y, 2);
            test_int(p[1].x, 2); test_int(p[1].y, 3);
        }

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c4, it.entities[0]);
        test_uint(ecs_childof(EcsWildcard), ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        {
            Position *p = ecs_field(&it, Position, 1);
            test_int(p[0].x, 3); test_int(p[0].y, 4);
        }

        test_bool(true, query_skip_builtin(world, &it));

        test_int(2, it.count);
        test_uint(c9, it.entities[0]);
        test_uint(c10, it.entities[1]);
        test_uint(ecs_childof(EcsWildcard), ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        {
            Position *p = ecs_field(&it, Position, 1);
            test_int(p[0].x, 5); test_int(p[0].y, 6);
            test_int(p[1].x, 6); test_int(p[1].y, 7);
        }

        test_bool(true, query_skip_builtin(world, &it));

        test_int(1, it.count);
        test_uint(c11, it.entities[0]);
        test_uint(ecs_childof(EcsWildcard), ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        {
            Position *p = ecs_field(&it, Position, 1);
            test_int(p[0].x, 7); test_int(p[0].y, 8);
        }

        test_bool(false, query_skip_builtin(world, &it));

        ecs_query_fini(q);
    }

    ecs_fini(world);
}

void NonFragmentingChildOf_this_src_childof_var(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_entity_t p_1 = ecs_new(world);
    ecs_entity_t p_2 = ecs_new(world);
    ecs_entity_t p_3 = ecs_new(world);

    ecs_entity_t c1 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t c2 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t c3 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t c4 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t c5 = ecs_new_w_pair(world, EcsChildOf, p_1);

    ecs_entity_t c6 = ecs_new_w_pair(world, EcsChildOf, p_2);
    ecs_entity_t c7 = ecs_insert(world, ecs_value(EcsParent, {p_2}));

    ecs_entity_t c8 = ecs_new_w_pair(world, EcsChildOf, p_3);
    ecs_entity_t c9 = ecs_new_w_pair(world, EcsChildOf, p_3);
    ecs_entity_t c10 = ecs_new_w_pair(world, EcsChildOf, p_3);
    ecs_entity_t c11 = ecs_new_w_pair(world, EcsChildOf, p_3);

    ecs_add(world, c2, Foo);
    ecs_add(world, c3, Foo);
    ecs_add(world, c4, Bar);

    ecs_add(world, c9, Foo);
    ecs_add(world, c10, Foo);
    ecs_add(world, c11, Bar);

    {
        ecs_query_t *q = ecs_query(world, {
            .expr = "(ChildOf, $x)",
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        int x_var = ecs_query_find_var(q, "x");
        test_assert(x_var != -1);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c8, it.entities[0]);
        test_uint(ecs_childof(p_3), ecs_field_id(&it, 0));
        test_uint(p_3, ecs_iter_get_var(&it, x_var));
        test_bool(true, ecs_field_is_set(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_int(2, it.count);
        test_uint(c9, it.entities[0]);
        test_uint(c10, it.entities[1]);
        test_uint(ecs_childof(p_3), ecs_field_id(&it, 0));
        test_uint(p_3, ecs_iter_get_var(&it, x_var));
        test_bool(true, ecs_field_is_set(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c11, it.entities[0]);
        test_uint(ecs_childof(p_3), ecs_field_id(&it, 0));
        test_uint(p_3, ecs_iter_get_var(&it, x_var));
        test_bool(true, ecs_field_is_set(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_int(2, it.count);
        test_uint(c6, it.entities[0]);
        test_uint(c7, it.entities[1]);
        test_uint(ecs_childof(p_2), ecs_field_id(&it, 0));
        test_uint(p_2, ecs_iter_get_var(&it, x_var));
        test_bool(true, ecs_field_is_set(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_int(5, it.count);
        test_uint(c1, it.entities[0]);
        test_uint(c2, it.entities[1]);
        test_uint(c3, it.entities[2]);
        test_uint(c4, it.entities[3]);
        test_uint(c5, it.entities[4]);
        test_uint(ecs_childof(p_1), ecs_field_id(&it, 0));
        test_uint(p_1, ecs_iter_get_var(&it, x_var));
        test_bool(true, ecs_field_is_set(&it, 0));

        /* Skip builtin entities */
        while (ecs_query_next(&it)) {
            test_assert(!ecs_table_has_id(world, it.table, EcsNotQueryable));
            test_assert(ecs_childof(ecs_iter_get_var(&it, x_var)) ==
                ecs_field_id(&it, 0));
        }

        ecs_query_fini(q);
    }

    ecs_fini(world);
}

void NonFragmentingChildOf_this_src_childof_set_var(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_entity_t p_1 = ecs_new(world);
    ecs_entity_t p_2 = ecs_new(world);
    ecs_entity_t p_3 = ecs_new(world);

    ecs_entity_t c1 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t c2 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t c3 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t c4 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t c5 = ecs_new_w_pair(world, EcsChildOf, p_1);

    ecs_entity_t c6 = ecs_new_w_pair(world, EcsChildOf, p_2);
    ecs_entity_t c7 = ecs_insert(world, ecs_value(EcsParent, {p_2}));

    ecs_entity_t c8 = ecs_new_w_pair(world, EcsChildOf, p_3);
    ecs_entity_t c9 = ecs_new_w_pair(world, EcsChildOf, p_3);
    ecs_entity_t c10 = ecs_new_w_pair(world, EcsChildOf, p_3);
    ecs_entity_t c11 = ecs_new_w_pair(world, EcsChildOf, p_3);

    ecs_add(world, c2, Foo);
    ecs_add(world, c3, Foo);
    ecs_add(world, c4, Bar);

    ecs_add(world, c9, Foo);
    ecs_add(world, c10, Foo);
    ecs_add(world, c11, Bar);

    ecs_query_t *q = ecs_query(world, {
        .expr = "(ChildOf, $x)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, x_var, p_1);
        test_bool(true, ecs_query_next(&it));
        test_int(5, it.count);
        test_uint(c1, it.entities[0]);
        test_uint(c2, it.entities[1]);
        test_uint(c3, it.entities[2]);
        test_uint(c4, it.entities[3]);
        test_uint(c5, it.entities[4]);
        test_uint(ecs_childof(p_1), ecs_field_id(&it, 0));
        test_uint(p_1, ecs_iter_get_var(&it, x_var));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_query_next(&it));
    }

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, x_var, p_2);
        test_bool(true, ecs_query_next(&it));
        test_int(2, it.count);
        test_uint(c6, it.entities[0]);
        test_uint(c7, it.entities[1]);
        test_uint(ecs_childof(p_2), ecs_field_id(&it, 0));
        test_uint(p_2, ecs_iter_get_var(&it, x_var));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_query_next(&it));
    }

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, x_var, p_3);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c8, it.entities[0]);
        test_uint(ecs_childof(p_3), ecs_field_id(&it, 0));
        test_uint(p_3, ecs_iter_get_var(&it, x_var));
        test_bool(true, ecs_field_is_set(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_int(2, it.count);
        test_uint(c9, it.entities[0]);
        test_uint(c10, it.entities[1]);
        test_uint(ecs_childof(p_3), ecs_field_id(&it, 0));
        test_uint(p_3, ecs_iter_get_var(&it, x_var));
        test_bool(true, ecs_field_is_set(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c11, it.entities[0]);
        test_uint(ecs_childof(p_3), ecs_field_id(&it, 0));
        test_uint(p_3, ecs_iter_get_var(&it, x_var));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_src_1_table_1_child(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_entity_t p = ecs_new(world);
    ecs_entity_t e = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_add(world, e, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ ecs_childof(p) }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e, it.entities[0]);
    test_uint(ecs_childof(p), ecs_field_id(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_src_1_table_1_child_w_expr(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_entity_t p = ecs_new(world);
    ecs_set_name(world, p, "p");

    ecs_entity_t e = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_add(world, e, Foo);

    ecs_query_t *q = ecs_query(world, {
        .expr = "(ChildOf, p)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e, it.entities[0]);
    test_uint(ecs_childof(p), ecs_field_id(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_src_1_table_2_children(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_entity_t p = ecs_new(world);
    ecs_entity_t e1 = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ ecs_childof(p) }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(2, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(e2, it.entities[1]);
    test_uint(ecs_childof(p), ecs_field_id(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_src_1_table_2_children_mixed_parents(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_entity_t p_1 = ecs_new(world);
    ecs_entity_t p_2 = ecs_new(world);
    ecs_entity_t e1 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(EcsParent, {p_2}));
    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {{ ecs_childof(p_1) }},
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e1, it.entities[0]);
        test_uint(ecs_childof(p_1), ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {{ ecs_childof(p_2) }},
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e2, it.entities[0]);
        test_uint(ecs_childof(p_2), ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    ecs_fini(world);
}

void NonFragmentingChildOf_this_src_1_table_n_children_mixed_parents(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_entity_t p_1 = ecs_new(world);
    ecs_entity_t p_2 = ecs_new(world);
    ecs_entity_t e1 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(EcsParent, {p_2}));
    ecs_entity_t e3 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t e4 = ecs_insert(world, ecs_value(EcsParent, {p_2}));
    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);
    ecs_add(world, e3, Foo);
    ecs_add(world, e4, Foo);

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {{ ecs_childof(p_1) }},
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(2, it.count);
        test_uint(e1, it.entities[0]);
        test_uint(e3, it.entities[1]);
        test_uint(ecs_childof(p_1), ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {{ ecs_childof(p_2) }},
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(2, it.count);
        test_uint(e2, it.entities[0]);
        test_uint(e4, it.entities[1]);
        test_uint(ecs_childof(p_2), ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    ecs_fini(world);
}

void NonFragmentingChildOf_this_src_1_table_1_child_w_tag(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_entity_t p = ecs_new(world);
    ecs_entity_t e = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_add(world, e, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ ecs_childof(p) }, { Foo }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e, it.entities[0]);
    test_uint(ecs_childof(p), ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_src_1_table_2_children_w_tag(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_entity_t p = ecs_new(world);
    ecs_entity_t e1 = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ ecs_childof(p) }, { Foo }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(ecs_childof(p), ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e2, it.entities[0]);
    test_uint(ecs_childof(p), ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 1));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_src_1_table_2_children_mixed_parents_w_tag(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_entity_t p_1 = ecs_new(world);
    ecs_entity_t p_2 = ecs_new(world);
    ecs_entity_t e1 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(EcsParent, {p_2}));
    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {{ ecs_childof(p_1) }, { Foo }},
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e1, it.entities[0]);
        test_uint(ecs_childof(p_1), ecs_field_id(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {{ ecs_childof(p_2) }, { Foo }},
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e2, it.entities[0]);
        test_uint(ecs_childof(p_2), ecs_field_id(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    ecs_fini(world);
}

void NonFragmentingChildOf_this_src_1_table_n_children_mixed_parents_w_tag(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_entity_t p_1 = ecs_new(world);
    ecs_entity_t p_2 = ecs_new(world);
    ecs_entity_t e1 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(EcsParent, {p_2}));
    ecs_entity_t e3 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t e4 = ecs_insert(world, ecs_value(EcsParent, {p_2}));
    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);
    ecs_add(world, e3, Foo);
    ecs_add(world, e4, Foo);

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {{ ecs_childof(p_1) }, { Foo }},
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e1, it.entities[0]);
        test_uint(ecs_childof(p_1), ecs_field_id(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e3, it.entities[0]);
        test_uint(ecs_childof(p_1), ecs_field_id(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {{ ecs_childof(p_2) }, { Foo }},
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e2, it.entities[0]);
        test_uint(ecs_childof(p_2), ecs_field_id(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e4, it.entities[0]);
        test_uint(ecs_childof(p_2), ecs_field_id(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));

        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    ecs_fini(world);
}

void NonFragmentingChildOf_this_src_1_table_no_children(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_entity_t p = ecs_new(world);
    ecs_new_w(world, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ Foo }, { ecs_childof(p) }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_new_w(world, Foo);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_src_written_1_table_1_child(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_entity_t p = ecs_new(world);
    ecs_entity_t e = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_add(world, e, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ Foo }, { ecs_childof(p) }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e, it.entities[0]);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_childof(p), ecs_field_id(&it, 1));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 1));
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_src_written_1_table_2_children(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_entity_t p = ecs_new(world);
    ecs_entity_t e1 = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ Foo }, { ecs_childof(p) }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_childof(p), ecs_field_id(&it, 1));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e2, it.entities[0]);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_childof(p), ecs_field_id(&it, 1));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 1));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_src_written_1_table_2_children_mixed_parents(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_entity_t p_1 = ecs_new(world);
    ecs_entity_t p_2 = ecs_new(world);
    ecs_entity_t e1 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(EcsParent, {p_2}));
    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {{ Foo }, { ecs_childof(p_1) }},
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e1, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_childof(p_1), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {{ Foo }, { ecs_childof(p_2) }},
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e2, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_childof(p_2), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    ecs_fini(world);
}

void NonFragmentingChildOf_this_src_written_1_table_n_children_mixed_parents(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_entity_t p_1 = ecs_new(world);
    ecs_entity_t p_2 = ecs_new(world);
    ecs_entity_t e1 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(EcsParent, {p_2}));
    ecs_entity_t e3 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t e4 = ecs_insert(world, ecs_value(EcsParent, {p_2}));
    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);
    ecs_add(world, e3, Foo);
    ecs_add(world, e4, Foo);

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {{ Foo }, { ecs_childof(p_1) }},
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e1, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_childof(p_1), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e3, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_childof(p_1), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {{ Foo }, { ecs_childof(p_2) }},
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e2, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_childof(p_2), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e4, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_childof(p_2), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));

        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    ecs_fini(world);
}

void NonFragmentingChildOf_this_src_1_table_1_child_w_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_entity_t p = ecs_new(world);
    ecs_entity_t e = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_add(world, e, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ ecs_childof(EcsWildcard) }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e, it.entities[0]);
    test_uint(ecs_childof(p), ecs_field_id(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 0));

    /* Skip builtin entities */
    while (ecs_query_next(&it)) {
        test_assert(!ecs_table_has_id(world, it.table, EcsNotQueryable));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_src_1_table_2_children_w_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_entity_t p = ecs_new(world);
    ecs_entity_t e1 = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ ecs_childof(EcsWildcard) }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(2, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(e2, it.entities[1]);
    test_uint(ecs_childof(p), ecs_field_id(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 0));

    /* Skip builtin entities */
    while (ecs_query_next(&it)) {
        test_assert(!ecs_table_has_id(world, it.table, EcsNotQueryable));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_src_1_table_2_children_mixed_parents_w_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_entity_t p_1 = ecs_new(world);
    ecs_entity_t p_2 = ecs_new(world);
    ecs_entity_t e1 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(EcsParent, {p_2}));
    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ ecs_childof(EcsWildcard) }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e2, it.entities[0]);
    test_uint(ecs_childof(p_2), ecs_field_id(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(ecs_childof(p_1), ecs_field_id(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 0));

    /* Skip builtin entities */
    while (ecs_query_next(&it)) {
        test_assert(!ecs_table_has_id(world, it.table, EcsNotQueryable));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_src_1_table_n_children_mixed_parents_w_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_entity_t p_1 = ecs_new(world);
    ecs_entity_t p_2 = ecs_new(world);
    ecs_entity_t e1 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(EcsParent, {p_2}));
    ecs_entity_t e3 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t e4 = ecs_insert(world, ecs_value(EcsParent, {p_2}));
    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);
    ecs_add(world, e3, Foo);
    ecs_add(world, e4, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ ecs_childof(EcsWildcard) }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(2, it.count);
    test_uint(e2, it.entities[0]);
    test_uint(e4, it.entities[1]);
    test_uint(ecs_childof(p_2), ecs_field_id(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(2, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(e3, it.entities[1]);
    test_uint(ecs_childof(p_1), ecs_field_id(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 0));

    /* Skip builtin entities */
    while (ecs_query_next(&it)) {
        test_assert(!ecs_table_has_id(world, it.table, EcsNotQueryable));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_src_1_table_1_child_w_tag_w_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_entity_t p = ecs_new(world);
    ecs_entity_t e = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_add(world, e, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ ecs_childof(EcsWildcard) }, { Foo }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e, it.entities[0]);
    test_uint(ecs_childof(p), ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_src_1_table_2_children_w_tag_w_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_entity_t p = ecs_new(world);
    ecs_entity_t e1 = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ ecs_childof(EcsWildcard) }, { Foo }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(ecs_childof(p), ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e2, it.entities[0]);
    test_uint(ecs_childof(p), ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 1));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_src_1_table_2_children_mixed_parents_w_tag_w_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_entity_t p_1 = ecs_new(world);
    ecs_entity_t p_2 = ecs_new(world);
    ecs_entity_t e1 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(EcsParent, {p_2}));
    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {{ ecs_childof(EcsWildcard) }, { Foo }},
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);

        if (cache_kind == EcsQueryCacheAuto) {
            test_bool(true, ecs_query_next(&it));
            test_int(1, it.count);
            test_uint(e1, it.entities[0]);
            test_uint(ecs_childof(p_1), ecs_field_id(&it, 0));
            test_uint(Foo, ecs_field_id(&it, 1));
            test_bool(true, ecs_field_is_set(&it, 0));
            test_bool(true, ecs_field_is_set(&it, 1));

            test_bool(true, ecs_query_next(&it));
            test_int(1, it.count);
            test_uint(e2, it.entities[0]);
            test_uint(ecs_childof(p_2), ecs_field_id(&it, 0));
            test_uint(Foo, ecs_field_id(&it, 1));
            test_bool(true, ecs_field_is_set(&it, 0));
            test_bool(true, ecs_field_is_set(&it, 1));
        } else {
            test_bool(true, ecs_query_next(&it));
            test_int(1, it.count);
            test_uint(e2, it.entities[0]);
            test_uint(ecs_childof(p_2), ecs_field_id(&it, 0));
            test_uint(Foo, ecs_field_id(&it, 1));
            test_bool(true, ecs_field_is_set(&it, 0));
            test_bool(true, ecs_field_is_set(&it, 1));

            test_bool(true, ecs_query_next(&it));
            test_int(1, it.count);
            test_uint(e1, it.entities[0]);
            test_uint(ecs_childof(p_1), ecs_field_id(&it, 0));
            test_uint(Foo, ecs_field_id(&it, 1));
            test_bool(true, ecs_field_is_set(&it, 0));
            test_bool(true, ecs_field_is_set(&it, 1));
        }

        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }
    ecs_fini(world);
}

void NonFragmentingChildOf_this_src_1_table_n_children_mixed_parents_w_tag_w_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_entity_t p_1 = ecs_new(world);
    ecs_entity_t p_2 = ecs_new(world);
    ecs_entity_t e1 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(EcsParent, {p_2}));
    ecs_entity_t e3 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t e4 = ecs_insert(world, ecs_value(EcsParent, {p_2}));
    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);
    ecs_add(world, e3, Foo);
    ecs_add(world, e4, Foo);

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {{ ecs_childof(EcsWildcard) }, { Foo }},
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        if (cache_kind == EcsQueryCacheAuto) {
            test_bool(true, ecs_query_next(&it));
            test_int(1, it.count);
            test_uint(e1, it.entities[0]);
            test_uint(ecs_childof(p_1), ecs_field_id(&it, 0));
            test_uint(Foo, ecs_field_id(&it, 1));
            test_bool(true, ecs_field_is_set(&it, 0));
            test_bool(true, ecs_field_is_set(&it, 1));

            test_bool(true, ecs_query_next(&it));
            test_int(1, it.count);
            test_uint(e2, it.entities[0]);
            test_uint(ecs_childof(p_2), ecs_field_id(&it, 0));
            test_uint(Foo, ecs_field_id(&it, 1));
            test_bool(true, ecs_field_is_set(&it, 0));
            test_bool(true, ecs_field_is_set(&it, 1));

            test_bool(true, ecs_query_next(&it));
            test_int(1, it.count);
            test_uint(e3, it.entities[0]);
            test_uint(ecs_childof(p_1), ecs_field_id(&it, 0));
            test_uint(Foo, ecs_field_id(&it, 1));
            test_bool(true, ecs_field_is_set(&it, 0));
            test_bool(true, ecs_field_is_set(&it, 1));

            test_bool(true, ecs_query_next(&it));
            test_int(1, it.count);
            test_uint(e4, it.entities[0]);
            test_uint(ecs_childof(p_2), ecs_field_id(&it, 0));
            test_uint(Foo, ecs_field_id(&it, 1));
            test_bool(true, ecs_field_is_set(&it, 0));
            test_bool(true, ecs_field_is_set(&it, 1));
        } else {
            test_bool(true, ecs_query_next(&it));
            test_int(1, it.count);
            test_uint(e2, it.entities[0]);
            test_uint(ecs_childof(p_2), ecs_field_id(&it, 0));
            test_uint(Foo, ecs_field_id(&it, 1));
            test_bool(true, ecs_field_is_set(&it, 0));
            test_bool(true, ecs_field_is_set(&it, 1));

            test_bool(true, ecs_query_next(&it));
            test_int(1, it.count);
            test_uint(e4, it.entities[0]);
            test_uint(ecs_childof(p_2), ecs_field_id(&it, 0));
            test_uint(Foo, ecs_field_id(&it, 1));
            test_bool(true, ecs_field_is_set(&it, 0));
            test_bool(true, ecs_field_is_set(&it, 1));

            test_bool(true, ecs_query_next(&it));
            test_int(1, it.count);
            test_uint(e1, it.entities[0]);
            test_uint(ecs_childof(p_1), ecs_field_id(&it, 0));
            test_uint(Foo, ecs_field_id(&it, 1));
            test_bool(true, ecs_field_is_set(&it, 0));
            test_bool(true, ecs_field_is_set(&it, 1));

            test_bool(true, ecs_query_next(&it));
            test_int(1, it.count);
            test_uint(e3, it.entities[0]);
            test_uint(ecs_childof(p_1), ecs_field_id(&it, 0));
            test_uint(Foo, ecs_field_id(&it, 1));
            test_bool(true, ecs_field_is_set(&it, 0));
            test_bool(true, ecs_field_is_set(&it, 1));
        }

        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    ecs_fini(world);
}

void NonFragmentingChildOf_this_src_written_1_table_1_child_w_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_entity_t p = ecs_new(world);
    ecs_entity_t e = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_add(world, e, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ Foo }, { ecs_childof(EcsWildcard) }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e, it.entities[0]);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_childof(p), ecs_field_id(&it, 1));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 1));
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_src_written_1_table_2_children_w_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_entity_t p = ecs_new(world);
    ecs_entity_t e1 = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ Foo }, { ecs_childof(EcsWildcard) }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_childof(p), ecs_field_id(&it, 1));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e2, it.entities[0]);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_childof(p), ecs_field_id(&it, 1));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 1));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_src_written_1_table_2_children_mixed_parents_w_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_entity_t p_1 = ecs_new(world);
    ecs_entity_t p_2 = ecs_new(world);
    ecs_entity_t e1 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(EcsParent, {p_2}));
    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {{ Foo }, { ecs_childof(EcsWildcard) }},
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e1, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_childof(p_1), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e2, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_childof(p_2), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));

        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    ecs_fini(world);
}

void NonFragmentingChildOf_this_src_written_1_table_n_children_mixed_parents_w_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_entity_t p_1 = ecs_new(world);
    ecs_entity_t p_2 = ecs_new(world);
    ecs_entity_t e1 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(EcsParent, {p_2}));
    ecs_entity_t e3 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t e4 = ecs_insert(world, ecs_value(EcsParent, {p_2}));
    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);
    ecs_add(world, e3, Foo);
    ecs_add(world, e4, Foo);

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {{ Foo }, { ecs_childof(EcsWildcard) }},
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e1, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_childof(p_1), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e2, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_childof(p_2), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e3, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_childof(p_1), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e4, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_childof(p_2), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));

        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    ecs_fini(world);
}

void NonFragmentingChildOf_this_src_1_table_1_child_w_any(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_entity_t p = ecs_new(world);
    ecs_entity_t e = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_add(world, e, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ ecs_childof(EcsAny) }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e, it.entities[0]);
    test_uint(ecs_childof(EcsWildcard), ecs_field_id(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 0));

    /* Skip builtin entities */
    while (ecs_query_next(&it)) {
        test_assert(!ecs_table_has_id(world, it.table, EcsNotQueryable));
        test_uint(ecs_childof(EcsWildcard), ecs_field_id(&it, 0));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_src_1_table_2_children_w_any(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_entity_t p = ecs_new(world);
    ecs_entity_t e1 = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ ecs_childof(EcsAny) }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(2, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(e2, it.entities[1]);
    test_uint(ecs_childof(EcsWildcard), ecs_field_id(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 0));

    /* Skip builtin entities */
    while (ecs_query_next(&it)) {
        test_assert(!ecs_table_has_id(world, it.table, EcsNotQueryable));
        test_uint(ecs_childof(EcsWildcard), ecs_field_id(&it, 0));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_src_1_table_2_children_mixed_parents_w_any(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_entity_t p_1 = ecs_new(world);
    ecs_entity_t p_2 = ecs_new(world);
    ecs_entity_t e1 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(EcsParent, {p_2}));
    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {{ ecs_childof(EcsAny) }},
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(2, it.count);
        test_uint(e1, it.entities[0]);
        test_uint(e2, it.entities[1]);
        test_uint(ecs_childof(EcsWildcard), ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));

        /* Skip builtin entities */
        while (ecs_query_next(&it)) {
            test_assert(!ecs_table_has_id(world, it.table, EcsNotQueryable));
            test_uint(ecs_childof(EcsWildcard), ecs_field_id(&it, 0));
        }

        ecs_query_fini(q);
    }

    ecs_fini(world);
}

void NonFragmentingChildOf_this_src_1_table_n_children_mixed_parents_w_any(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_entity_t p_1 = ecs_new(world);
    ecs_entity_t p_2 = ecs_new(world);
    ecs_entity_t e1 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(EcsParent, {p_2}));
    ecs_entity_t e3 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t e4 = ecs_insert(world, ecs_value(EcsParent, {p_2}));
    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);
    ecs_add(world, e3, Foo);
    ecs_add(world, e4, Foo);

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {{ ecs_childof(EcsAny) }},
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(4, it.count);
        test_uint(e1, it.entities[0]);
        test_uint(e2, it.entities[1]);
        test_uint(e3, it.entities[2]);
        test_uint(e4, it.entities[3]);
        test_uint(ecs_childof(EcsWildcard), ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));

        /* Skip builtin entities */
        while (ecs_query_next(&it)) {
            test_assert(!ecs_table_has_id(world, it.table, EcsNotQueryable));
            test_uint(ecs_childof(EcsWildcard), ecs_field_id(&it, 0));
        }

        ecs_query_fini(q);
    }

    ecs_fini(world);
}

void NonFragmentingChildOf_this_src_1_table_1_child_w_tag_w_any(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_entity_t p = ecs_new(world);
    ecs_entity_t e = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_add(world, e, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ ecs_childof(EcsAny) }, { Foo }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e, it.entities[0]);
    test_uint(ecs_childof(EcsWildcard), ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_src_1_table_2_children_w_tag_w_any(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_entity_t p = ecs_new(world);
    ecs_entity_t e1 = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ ecs_childof(EcsAny) }, { Foo }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(2, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(e2, it.entities[1]);
    test_uint(ecs_childof(EcsWildcard), ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 1));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_src_1_table_2_children_mixed_parents_w_tag_w_any(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_entity_t p_1 = ecs_new(world);
    ecs_entity_t p_2 = ecs_new(world);
    ecs_entity_t e1 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(EcsParent, {p_2}));
    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {{ ecs_childof(EcsAny) }, { Foo }},
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(2, it.count);
        test_uint(e1, it.entities[0]);
        test_uint(e2, it.entities[1]);
        test_uint(ecs_childof(EcsWildcard), ecs_field_id(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));

        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }
    ecs_fini(world);
}

void NonFragmentingChildOf_this_src_1_table_n_children_mixed_parents_w_tag_w_any(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_entity_t p_1 = ecs_new(world);
    ecs_entity_t p_2 = ecs_new(world);
    ecs_entity_t e1 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(EcsParent, {p_2}));
    ecs_entity_t e3 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t e4 = ecs_insert(world, ecs_value(EcsParent, {p_2}));
    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);
    ecs_add(world, e3, Foo);
    ecs_add(world, e4, Foo);

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {{ ecs_childof(EcsAny) }, { Foo }},
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(4, it.count);
        test_uint(e1, it.entities[0]);
        test_uint(e2, it.entities[1]);
        test_uint(e3, it.entities[2]);
        test_uint(e4, it.entities[3]);
        test_uint(ecs_childof(EcsWildcard), ecs_field_id(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));

        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    ecs_fini(world);
}

void NonFragmentingChildOf_this_src_2_tables_2_children(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_entity_t p1 = ecs_new(world);
    ecs_entity_t p2 = ecs_new(world);
    ecs_entity_t c1 = ecs_insert(world, ecs_value(EcsParent, {p1}));
    ecs_entity_t c2 = ecs_insert(world, ecs_value(EcsParent, {p1}));
    ecs_entity_t c3 = ecs_insert(world, ecs_value(EcsParent, {p2}));
    ecs_entity_t c4 = ecs_insert(world, ecs_value(EcsParent, {p2}));

    ecs_add(world, c1, Foo);
    ecs_add(world, c2, Bar);
    ecs_add(world, c3, Foo);
    ecs_add(world, c4, Bar);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { ecs_childof(p1) }
        },
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(2, it.count);
    test_uint(c1, it.entities[0]);
    test_uint(c2, it.entities[1]);
    test_uint(ecs_childof(p1), ecs_field_id(&it, 0));
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_src_2_tables_n_children_mixed_parents(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_entity_t p1 = ecs_new(world);
    ecs_entity_t p2 = ecs_new(world);
    ecs_entity_t c1 = ecs_insert(world, ecs_value(EcsParent, {p1}));
    ecs_entity_t c2 = ecs_insert(world, ecs_value(EcsParent, {p1}));
    ecs_entity_t c3 = ecs_new_w_pair(world, EcsChildOf, p1);
    ecs_entity_t c4 = ecs_new_w_pair(world, EcsChildOf, p1);

    ecs_entity_t c5 = ecs_insert(world, ecs_value(EcsParent, {p2}));
    ecs_entity_t c6 = ecs_new_w_pair(world, EcsChildOf, p2);

    ecs_add(world, c1, Foo);
    ecs_add(world, c2, Bar);
    ecs_add(world, c3, Foo);
    ecs_add(world, c4, Bar);
    ecs_add(world, c5, Foo);
    ecs_add(world, c6, Bar);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { ecs_childof(p1) }
        },
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(4, it.count);
    test_uint(c1, it.entities[0]);
    test_uint(c2, it.entities[1]);
    test_uint(c3, it.entities[2]);
    test_uint(c4, it.entities[3]);
    test_uint(ecs_childof(p1), ecs_field_id(&it, 0));
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_src_written_1_table_1_child_w_any(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_entity_t p = ecs_new(world);
    ecs_entity_t e = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_add(world, e, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ Foo }, { ecs_childof(EcsAny) }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e, it.entities[0]);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_childof(EcsWildcard), ecs_field_id(&it, 1));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 1));
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_src_written_1_table_2_children_w_any(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_entity_t p = ecs_new(world);
    ecs_entity_t e1 = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ Foo }, { ecs_childof(EcsAny) }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(2, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(e2, it.entities[1]);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_childof(EcsWildcard), ecs_field_id(&it, 1));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 1));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_src_written_1_table_2_children_mixed_parents_w_any(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_entity_t p_1 = ecs_new(world);
    ecs_entity_t p_2 = ecs_new(world);
    ecs_entity_t e1 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(EcsParent, {p_2}));
    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {{ Foo }, { ecs_childof(EcsAny) }},
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(2, it.count);
        test_uint(e1, it.entities[0]);
        test_uint(e2, it.entities[1]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_childof(EcsWildcard), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));

        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    ecs_fini(world);
}

void NonFragmentingChildOf_this_src_written_1_table_n_children_mixed_parents_w_any(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_entity_t p_1 = ecs_new(world);
    ecs_entity_t p_2 = ecs_new(world);
    ecs_entity_t e1 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(EcsParent, {p_2}));
    ecs_entity_t e3 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t e4 = ecs_insert(world, ecs_value(EcsParent, {p_2}));
    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);
    ecs_add(world, e3, Foo);
    ecs_add(world, e4, Foo);

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {{ Foo }, { ecs_childof(EcsAny) }},
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(4, it.count);
        test_uint(e1, it.entities[0]);
        test_uint(e2, it.entities[1]);
        test_uint(e3, it.entities[2]);
        test_uint(e4, it.entities[3]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_childof(EcsWildcard), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));

        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    ecs_fini(world);
}

void NonFragmentingChildOf_this_src_2_tables_childof_w_any(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_entity_t p = ecs_new(world);
    ecs_entity_t e1 = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_entity_t e2 = ecs_new_w_pair(world, EcsChildOf, p);
    
    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);
    ecs_add(world, e2, Bar);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ ecs_childof(EcsAny) }, { Foo }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(ecs_childof(EcsWildcard), ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e2, it.entities[0]);
    test_uint(ecs_childof(EcsWildcard), ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 1));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_src_2_tables_parent_w_any(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_entity_t p = ecs_new(world);
    ecs_entity_t e1 = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(EcsParent, {p}));
    
    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);
    ecs_add(world, e2, Bar);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ ecs_childof(EcsAny) }, { Foo }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(ecs_childof(EcsWildcard), ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e2, it.entities[0]);
    test_uint(ecs_childof(EcsWildcard), ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 1));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_src_2_tables_mixed_w_any(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_entity_t p = ecs_new(world);
    ecs_entity_t e1 = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_entity_t e3 = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_entity_t e4 = ecs_new_w_pair(world, EcsChildOf, p);

    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);
    ecs_add(world, e3, Foo);
    ecs_add(world, e4, Foo);
    ecs_add(world, e2, Bar);
    ecs_add(world, e4, Bar);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ ecs_childof(EcsAny) }, { Foo }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(ecs_childof(EcsWildcard), ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(cache_kind == EcsQueryCacheAuto ? e3 : e2, it.entities[0]);
    test_uint(ecs_childof(EcsWildcard), ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(cache_kind == EcsQueryCacheAuto ? e2 : e3, it.entities[0]);
    test_uint(ecs_childof(EcsWildcard), ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e4, it.entities[0]);
    test_uint(ecs_childof(EcsWildcard), ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 1));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_src_written_2_tables_childof_w_any(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_entity_t p = ecs_new(world);
    ecs_entity_t e1 = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_entity_t e2 = ecs_new_w_pair(world, EcsChildOf, p);
    
    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);
    ecs_add(world, e2, Bar);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ Foo }, { ecs_childof(EcsAny) }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_childof(EcsWildcard), ecs_field_id(&it, 1));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e2, it.entities[0]);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_childof(EcsWildcard), ecs_field_id(&it, 1));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 1));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_src_written_2_tables_parent_w_any(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_entity_t p = ecs_new(world);
    ecs_entity_t e1 = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(EcsParent, {p}));
    
    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);
    ecs_add(world, e2, Bar);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ Foo }, { ecs_childof(EcsAny) }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_childof(EcsWildcard), ecs_field_id(&it, 1));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e2, it.entities[0]);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_childof(EcsWildcard), ecs_field_id(&it, 1));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 1));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_src_written_2_tables_mixed_w_any(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_entity_t p = ecs_new(world);
    ecs_entity_t e1 = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_entity_t e3 = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_entity_t e4 = ecs_new_w_pair(world, EcsChildOf, p);

    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);
    ecs_add(world, e3, Foo);
    ecs_add(world, e4, Foo);
    ecs_add(world, e2, Bar);
    ecs_add(world, e4, Bar);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ Foo }, { ecs_childof(EcsAny) }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_childof(EcsWildcard), ecs_field_id(&it, 1));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e3, it.entities[0]);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_childof(EcsWildcard), ecs_field_id(&it, 1));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e2, it.entities[0]);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_childof(EcsWildcard), ecs_field_id(&it, 1));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e4, it.entities[0]);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_childof(EcsWildcard), ecs_field_id(&it, 1));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 1));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_up_childof(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_entity_t p = ecs_new(world);
    ecs_add(world, p, Bar);
    ecs_entity_t c_1 = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_entity_t c_2 = ecs_insert(world, ecs_value(EcsParent, {p}));

    ecs_add(world, c_1, Foo);
    ecs_add(world, c_2, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { .id = Bar, .src.id = EcsUp }
        },
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], c_1);
    test_uint(p, ecs_field_src(&it, 0));
    test_uint(Bar, ecs_field_id(&it, 0));
    
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], c_2);
    test_uint(p, ecs_field_src(&it, 0));
    test_uint(Bar, ecs_field_id(&it, 0));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_up_childof_w_component(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_COMPONENT(world, Position);

    ecs_entity_t p = ecs_new(world);
    ecs_set(world, p, Position, {10, 20});
    ecs_entity_t c_1 = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_entity_t c_2 = ecs_insert(world, ecs_value(EcsParent, {p}));

    ecs_add(world, c_1, Foo);
    ecs_add(world, c_2, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { .id = ecs_id(Position), .src.id = EcsUp }
        },
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], c_1);
    test_uint(p, ecs_field_src(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_int(10, p->x);
        test_int(20, p->y);
    }
    
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], c_2);
    test_uint(p, ecs_field_src(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_int(10, p->x);
        test_int(20, p->y);
    }

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_up_childof_w_component_inherited(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_COMPONENT(world, Position);

    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);

    ecs_entity_t b = ecs_new(world);
    ecs_set(world, b, Position, {10, 20});

    ecs_entity_t p = ecs_new_w_pair(world, EcsIsA, b);
    ecs_entity_t c_1 = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_entity_t c_2 = ecs_insert(world, ecs_value(EcsParent, {p}));

    ecs_add(world, c_1, Foo);
    ecs_add(world, c_2, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { .id = ecs_id(Position), .src.id = EcsUp }
        },
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], c_1);
    test_uint(b, ecs_field_src(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_int(10, p->x);
        test_int(20, p->y);
    }
    
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], c_2);
    test_uint(b, ecs_field_src(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_int(10, p->x);
        test_int(20, p->y);
    }

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_up_childof_2_lvl(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_entity_t p = ecs_new(world);
    ecs_entity_t c_1 = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_entity_t c_2 = ecs_insert(world, ecs_value(EcsParent, {p}));

    ecs_add(world, c_1, Foo);
    ecs_add(world, c_2, Foo);

    ecs_entity_t parent = ecs_new(world);
    ecs_add(world, parent, Bar);
    ecs_add_pair(world, p, EcsChildOf, parent);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { .id = Bar, .src.id = EcsUp }
        },
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], p);
    test_uint(parent, ecs_field_src(&it, 0));
    test_uint(Bar, ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], c_1);
    test_uint(parent, ecs_field_src(&it, 0));
    test_uint(Bar, ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], c_2);
    test_uint(parent, ecs_field_src(&it, 0));
    test_uint(Bar, ecs_field_id(&it, 0));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_up_childof_2_lvl_w_component(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_COMPONENT(world, Position);

    ecs_entity_t p = ecs_new(world);
    ecs_entity_t c_1 = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_entity_t c_2 = ecs_insert(world, ecs_value(EcsParent, {p}));

    ecs_add(world, c_1, Foo);
    ecs_add(world, c_2, Foo);

    ecs_entity_t parent = ecs_new(world);
    ecs_set(world, parent, Position, {10, 20});
    ecs_add_pair(world, p, EcsChildOf, parent);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { .id = ecs_id(Position), .src.id = EcsUp }
        },
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], p);
    test_uint(parent, ecs_field_src(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_int(10, p->x);
        test_int(20, p->y);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], c_1);
    test_uint(parent, ecs_field_src(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_int(10, p->x);
        test_int(20, p->y);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], c_2);
    test_uint(parent, ecs_field_src(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_int(10, p->x);
        test_int(20, p->y);
    }


    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_up_childof_2_lvl_w_component_inherited(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_COMPONENT(world, Position);

    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);

    ecs_entity_t p = ecs_new(world);
    ecs_entity_t c_1 = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_entity_t c_2 = ecs_insert(world, ecs_value(EcsParent, {p}));

    ecs_add(world, c_1, Foo);
    ecs_add(world, c_2, Foo);

    ecs_entity_t base = ecs_new(world);
    ecs_set(world, base, Position, {10, 20});
    ecs_entity_t parent = ecs_new_w_pair(world, EcsIsA, base);
    ecs_add_pair(world, p, EcsChildOf, parent);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { .id = ecs_id(Position), .src.id = EcsUp }
        },
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);    
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], p);
    test_uint(base, ecs_field_src(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_int(10, p->x);
        test_int(20, p->y);
    }

    test_bool(true, ecs_query_next(&it));    
    test_int(1, it.count);
    test_uint(it.entities[0], c_1);
    test_uint(base, ecs_field_src(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_int(10, p->x);
        test_int(20, p->y);
    }

    test_bool(true, ecs_query_next(&it));    
    test_int(1, it.count);
    test_uint(it.entities[0], c_2);
    test_uint(base, ecs_field_src(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_int(10, p->x);
        test_int(20, p->y);
    }

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_up_childof_2_lvl_w_owned(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_entity_t p = ecs_new(world);
    ecs_entity_t c_1 = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_entity_t c_2 = ecs_insert(world, ecs_value(EcsParent, {p}));

    ecs_add(world, c_1, Foo);
    ecs_add(world, c_2, Foo);
    ecs_add(world, c_2, Bar);

    ecs_entity_t parent = ecs_new(world);
    ecs_add(world, parent, Bar);
    ecs_add_pair(world, p, EcsChildOf, parent);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { .id = Bar, .src.id = EcsUp }
        },
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], p);
    test_uint(parent, ecs_field_src(&it, 0));
    test_uint(Bar, ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], c_1);
    test_uint(parent, ecs_field_src(&it, 0));
    test_uint(Bar, ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], c_2);
    test_uint(parent, ecs_field_src(&it, 0));
    test_uint(Bar, ecs_field_id(&it, 0));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_self_up_childof(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_entity_t p = ecs_new(world);
    ecs_add(world, p, Bar);
    ecs_entity_t c_1 = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_entity_t c_2 = ecs_insert(world, ecs_value(EcsParent, {p}));

    ecs_add(world, c_1, Foo);
    ecs_add(world, c_2, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { .id = Bar, .src.id = EcsSelf|EcsUp }
        },
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], p);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(Bar, ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], c_1);
    test_uint(p, ecs_field_src(&it, 0));
    test_uint(Bar, ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], c_2);
    test_uint(p, ecs_field_src(&it, 0));
    test_uint(Bar, ecs_field_id(&it, 0));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_self_up_childof_w_component(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_COMPONENT(world, Position);

    ecs_entity_t p = ecs_new(world);
    ecs_set(world, p, Position, {10, 20});
    ecs_entity_t c_1 = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_entity_t c_2 = ecs_insert(world, ecs_value(EcsParent, {p}));

    ecs_add(world, c_1, Foo);
    ecs_add(world, c_2, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { .id = ecs_id(Position), .src.id = EcsSelf|EcsUp }
        },
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], p);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_int(10, p->x);
        test_int(20, p->y);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], c_1);
    test_uint(p, ecs_field_src(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_int(10, p[0].x); test_int(20, p[0].y);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], c_2);
    test_uint(p, ecs_field_src(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_int(10, p[0].x); test_int(20, p[0].y);
    }

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_self_up_childof_w_component_inherited(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_COMPONENT(world, Position);

    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);

    ecs_entity_t b = ecs_new(world);
    ecs_set(world, b, Position, {10, 20});
    ecs_entity_t p = ecs_new(world);
    ecs_add_pair(world, p, EcsIsA, b);
    ecs_entity_t c_1 = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_entity_t c_2 = ecs_insert(world, ecs_value(EcsParent, {p}));

    ecs_add(world, c_1, Foo);
    ecs_add(world, c_2, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { .id = ecs_id(Position), .src.id = EcsSelf|EcsUp }
        },
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], b);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_int(10, p->x);
        test_int(20, p->y);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], c_1);
    test_uint(b, ecs_field_src(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_int(10, p->x);
        test_int(20, p->y);
    }
    
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], c_2);
    test_uint(b, ecs_field_src(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_int(10, p->x);
        test_int(20, p->y);
    }

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_self_up_childof_2_lvl(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_entity_t p = ecs_new(world);
    ecs_entity_t c_1 = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_entity_t c_2 = ecs_insert(world, ecs_value(EcsParent, {p}));

    ecs_add(world, c_1, Foo);
    ecs_add(world, c_2, Foo);

    ecs_entity_t parent = ecs_new(world);
    ecs_add(world, parent, Bar);
    ecs_add_pair(world, p, EcsChildOf, parent);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { .id = Bar, .src.id = EcsSelf|EcsUp }
        },
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], parent);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(Bar, ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], p);
    test_uint(parent, ecs_field_src(&it, 0));
    test_uint(Bar, ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], c_1);
    test_uint(parent, ecs_field_src(&it, 0));
    test_uint(Bar, ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], c_2);
    test_uint(parent, ecs_field_src(&it, 0));
    test_uint(Bar, ecs_field_id(&it, 0));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_self_up_childof_2_lvl_w_component(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_COMPONENT(world, Position);

    ecs_entity_t p = ecs_new(world);
    ecs_entity_t c_1 = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_entity_t c_2 = ecs_insert(world, ecs_value(EcsParent, {p}));

    ecs_add(world, c_1, Foo);
    ecs_add(world, c_2, Foo);

    ecs_entity_t parent = ecs_new(world);
    ecs_set(world, parent, Position, {10, 20});
    ecs_add_pair(world, p, EcsChildOf, parent);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { .id = ecs_id(Position), .src.id = EcsSelf|EcsUp }
        },
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], parent);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_int(10, p->x);
        test_int(20, p->y);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], p);
    test_uint(parent, ecs_field_src(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_int(10, p->x);
        test_int(20, p->y);
    }
    
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], c_1);
    test_uint(parent, ecs_field_src(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_int(10, p->x);
        test_int(20, p->y);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], c_2);
    test_uint(parent, ecs_field_src(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_int(10, p->x);
        test_int(20, p->y);
    }

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_self_up_childof_2_lvl_w_component_inherited(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_COMPONENT(world, Position);

    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);

    ecs_entity_t p = ecs_new(world);
    ecs_entity_t c_1 = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_entity_t c_2 = ecs_insert(world, ecs_value(EcsParent, {p}));

    ecs_add(world, c_1, Foo);
    ecs_add(world, c_2, Foo);

    ecs_entity_t base = ecs_new(world);
    ecs_set(world, base, Position, {10, 20});
    ecs_entity_t parent = ecs_new_w_pair(world, EcsIsA, base);
    ecs_add_pair(world, p, EcsChildOf, parent);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { .id = ecs_id(Position), .src.id = EcsSelf|EcsUp }
        },
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], base);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_int(10, p->x);
        test_int(20, p->y);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], p);
    test_uint(base, ecs_field_src(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_int(10, p->x);
        test_int(20, p->y);
    }

    test_bool(true, ecs_query_next(&it));    
    test_int(1, it.count);
    test_uint(it.entities[0], c_1);
    test_uint(base, ecs_field_src(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_int(10, p->x);
        test_int(20, p->y);
    }

    test_bool(true, ecs_query_next(&it));    
    test_int(1, it.count);
    test_uint(it.entities[0], c_2);
    test_uint(base, ecs_field_src(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_int(10, p->x);
        test_int(20, p->y);
    }

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_self_up_childof_2_lvl_w_owned(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_entity_t p = ecs_new(world);
    ecs_entity_t c_1 = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_entity_t c_2 = ecs_insert(world, ecs_value(EcsParent, {p}));

    ecs_add(world, c_1, Foo);
    ecs_add(world, c_2, Foo);
    ecs_add(world, c_2, Bar);

    ecs_entity_t parent = ecs_new(world);
    ecs_add(world, parent, Bar);
    ecs_add_pair(world, p, EcsChildOf, parent);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { .id = Bar, .src.id = EcsSelf|EcsUp }
        },
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], parent);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(Bar, ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], p);
    test_uint(parent, ecs_field_src(&it, 0));
    test_uint(Bar, ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], c_1);
    test_uint(parent, ecs_field_src(&it, 0));
    test_uint(Bar, ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], c_2);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(Bar, ecs_field_id(&it, 0));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_up_childof_2_lvl_children(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Bar);

    ecs_entity_t p = ecs_new(world);
    ecs_add(world, p, Bar);
    ecs_entity_t c_1 = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_entity_t c_2 = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_add_id(world, c_1, EcsOrderedChildren);
    ecs_entity_t gc_1 = ecs_insert(world, ecs_value(EcsParent, {c_1}));

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { .id = Bar, .src.id = EcsUp }
        },
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], cache_kind == EcsQueryCacheAuto ? c_2 : c_1);
    test_uint(p, ecs_field_src(&it, 0));
    test_uint(Bar, ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], cache_kind == EcsQueryCacheAuto ? c_1 : c_2);
    test_uint(p, ecs_field_src(&it, 0));
    test_uint(Bar, ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], gc_1);
    test_uint(p, ecs_field_src(&it, 0));
    test_uint(Bar, ecs_field_id(&it, 0));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_up_childof_2_lvl_children_w_component(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t p = ecs_new(world);
    ecs_set(world, p, Position, {10, 20});
    ecs_entity_t c_1 = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_entity_t c_2 = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_add_id(world, c_1, EcsOrderedChildren);
    ecs_entity_t gc_1 = ecs_insert(world, ecs_value(EcsParent, {c_1}));

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { .id = ecs_id(Position), .src.id = EcsUp }
        },
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], cache_kind == EcsQueryCacheAuto ? c_2 : c_1);
    test_uint(p, ecs_field_src(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_int(10, p->x);
        test_int(20, p->y);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], cache_kind == EcsQueryCacheAuto ? c_1 : c_2);
    test_uint(p, ecs_field_src(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_int(10, p->x);
        test_int(20, p->y);
    }
    
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], gc_1);
    test_uint(p, ecs_field_src(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_int(10, p->x);
        test_int(20, p->y);
    }

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_up_childof_2_lvl_children_on_instantiate_inherit(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Bar, (OnInstantiate, Inherit));

    ecs_entity_t b = ecs_new(world);
    ecs_add(world, b, Bar);
    ecs_entity_t p = ecs_new(world);
    ecs_add_pair(world, p, EcsIsA, b);
    ecs_entity_t c_1 = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_entity_t c_2 = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_add_id(world, c_1, EcsOrderedChildren);
    ecs_entity_t gc_1 = ecs_insert(world, ecs_value(EcsParent, {c_1}));

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { .id = Bar, .src.id = EcsUp }
        },
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);    
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], cache_kind == EcsQueryCacheAuto ? c_2 : c_1);
    test_uint(b, ecs_field_src(&it, 0));
    test_uint(Bar, ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], cache_kind == EcsQueryCacheAuto ? c_1 : c_2);
    test_uint(b, ecs_field_src(&it, 0));
    test_uint(Bar, ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], gc_1);
    test_uint(b, ecs_field_src(&it, 0));
    test_uint(Bar, ecs_field_id(&it, 0));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_up_childof_2_lvl_children_w_component_on_instantiate_inherit(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);

    ecs_entity_t b = ecs_new(world);
    ecs_set(world, b, Position, {10, 20});
    ecs_entity_t p = ecs_new(world);
    ecs_add_pair(world, p, EcsIsA, b);

    ecs_entity_t c_1 = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_entity_t c_2 = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_add_id(world, c_1, EcsOrderedChildren);
    ecs_entity_t gc_1 = ecs_insert(world, ecs_value(EcsParent, {c_1}));

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { .id = ecs_id(Position), .src.id = EcsUp }
        },
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);    
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], cache_kind == EcsQueryCacheAuto ? c_2 : c_1);
    test_uint(b, ecs_field_src(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_int(10, p->x);
        test_int(20, p->y);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], cache_kind == EcsQueryCacheAuto ? c_1 : c_2);
    test_uint(b, ecs_field_src(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_int(10, p->x);
        test_int(20, p->y);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], gc_1);
    test_uint(b, ecs_field_src(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_int(10, p->x);
        test_int(20, p->y);
    }

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_up_childof_2_lvl_children_on_instantiate_dont_inherit(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Bar, (OnInstantiate, DontInherit));

    ecs_entity_t b = ecs_new(world);
    ecs_add(world, b, Bar);
    ecs_entity_t p = ecs_new(world);
    ecs_add_pair(world, p, EcsIsA, b);
    ecs_entity_t c_1 = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_add_id(world, c_1, EcsOrderedChildren);
    ecs_insert(world, ecs_value(EcsParent, {c_1}));

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { .id = Bar, .src.id = EcsUp }
        },
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_self_up_childof_2_lvl_children(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Bar);

    ecs_entity_t p = ecs_new(world);
    ecs_add(world, p, Bar);
    ecs_entity_t c_1 = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_entity_t c_2 = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_add_id(world, c_1, EcsOrderedChildren);
    ecs_entity_t gc_1 = ecs_insert(world, ecs_value(EcsParent, {c_1}));

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { .id = Bar, .src.id = EcsSelf|EcsUp }
        },
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], p);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(Bar, ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], cache_kind == EcsQueryCacheAuto ? c_2 : c_1);
    test_uint(p, ecs_field_src(&it, 0));
    test_uint(Bar, ecs_field_id(&it, 0));
    
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], cache_kind == EcsQueryCacheAuto ? c_1 : c_2);
    test_uint(p, ecs_field_src(&it, 0));
    test_uint(Bar, ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], gc_1);
    test_uint(p, ecs_field_src(&it, 0));
    test_uint(Bar, ecs_field_id(&it, 0));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_self_up_childof_2_lvl_children_w_component(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t p = ecs_new(world);
    ecs_set(world, p, Position, {10, 20});
    ecs_entity_t c_1 = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_entity_t c_2 = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_add_id(world, c_1, EcsOrderedChildren);
    ecs_entity_t gc_1 = ecs_insert(world, ecs_value(EcsParent, {c_1}));

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { .id = ecs_id(Position), .src.id = EcsSelf|EcsUp }
        },
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], p);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_int(10, p->x);
        test_int(20, p->y);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], cache_kind == EcsQueryCacheAuto ? c_2 : c_1);
    test_uint(p, ecs_field_src(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_int(10, p[0].x); test_int(20, p[0].y);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], cache_kind == EcsQueryCacheAuto ? c_1 : c_2);
    test_uint(p, ecs_field_src(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_int(10, p[0].x); test_int(20, p[0].y);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], gc_1);
    test_uint(p, ecs_field_src(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_int(10, p->x);
        test_int(20, p->y);
    }

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_self_up_childof_2_lvl_children_on_instantiate_inherit(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Bar, (OnInstantiate, Inherit));

    ecs_entity_t b = ecs_new(world);
    ecs_add(world, b, Bar);
    ecs_entity_t p = ecs_new(world);
    ecs_add_pair(world, p, EcsIsA, b);
    ecs_entity_t c_1 = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_entity_t c_2 = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_add_id(world, c_1, EcsOrderedChildren);
    ecs_entity_t gc_1 = ecs_insert(world, ecs_value(EcsParent, {c_1}));

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { .id = Bar, .src.id = EcsSelf|EcsUp }
        },
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], b);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(Bar, ecs_field_id(&it, 0));
    
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], cache_kind == EcsQueryCacheAuto ? c_2 : c_1);
    test_uint(b, ecs_field_src(&it, 0));
    test_uint(Bar, ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], cache_kind == EcsQueryCacheAuto ? c_1 : c_2);
    test_uint(b, ecs_field_src(&it, 0));
    test_uint(Bar, ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], gc_1);
    test_uint(b, ecs_field_src(&it, 0));
    test_uint(Bar, ecs_field_id(&it, 0));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_self_up_childof_2_lvl_children_on_instantiate_dont_inherit(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Bar, (OnInstantiate, DontInherit));

    ecs_entity_t b = ecs_new(world);
    ecs_add(world, b, Bar);
    ecs_entity_t p = ecs_new(world);
    ecs_add_pair(world, p, EcsIsA, b);
    ecs_entity_t c_1 = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_add_id(world, c_1, EcsOrderedChildren);
    ecs_insert(world, ecs_value(EcsParent, {c_1}));

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { .id = Bar, .src.id = EcsSelf|EcsUp }
        },
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], b);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(Bar, ecs_field_id(&it, 0));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_up_childof_2_lvl_children_from_child(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Bar);

    ecs_entity_t p = ecs_new(world);
    ecs_entity_t c_1 = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_add(world, c_1, Bar);
    ecs_add_id(world, c_1, EcsOrderedChildren);
    ecs_entity_t gc_1 = ecs_insert(world, ecs_value(EcsParent, {c_1}));

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { .id = Bar, .src.id = EcsUp }
        },
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);    
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], gc_1);
    test_uint(c_1, ecs_field_src(&it, 0));
    test_uint(Bar, ecs_field_id(&it, 0));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_up_childof_2_lvl_children_w_component_from_child(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t p = ecs_new(world);
    ecs_entity_t c_1 = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_set(world, c_1, Position, {10, 20});
    ecs_add_id(world, c_1, EcsOrderedChildren);
    ecs_entity_t gc_1 = ecs_insert(world, ecs_value(EcsParent, {c_1}));

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { .id = ecs_id(Position), .src.id = EcsUp }
        },
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);    
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], gc_1);
    test_uint(c_1, ecs_field_src(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_int(10, p->x);
        test_int(20, p->y);
    }

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_up_childof_2_lvl_children_from_child_on_instantiate_inherit(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Bar);

    ecs_add_pair(world, Bar, EcsOnInstantiate, EcsInherit);

    ecs_entity_t p = ecs_new(world);
    ecs_entity_t b = ecs_new(world);
    ecs_add(world, b, Bar);
    ecs_entity_t c_1 = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_add_pair(world, c_1, EcsIsA, b);
    ecs_add_id(world, c_1, EcsOrderedChildren);
    ecs_entity_t gc_1 = ecs_insert(world, ecs_value(EcsParent, {c_1}));

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { .id = Bar, .src.id = EcsUp }
        },
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);    
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], gc_1);
    test_uint(b, ecs_field_src(&it, 0));
    test_uint(Bar, ecs_field_id(&it, 0));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_up_childof_2_lvl_children_from_child_on_instantiate_dont_inherit(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Bar);

    ecs_add_pair(world, Bar, EcsOnInstantiate, EcsDontInherit);

    ecs_entity_t p = ecs_new(world);
    ecs_entity_t b = ecs_new(world);
    ecs_add(world, b, Bar);
    ecs_entity_t c_1 = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_add_pair(world, c_1, EcsIsA, b);
    ecs_add_id(world, c_1, EcsOrderedChildren);
    ecs_insert(world, ecs_value(EcsParent, {c_1}));

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { .id = Bar, .src.id = EcsUp }
        },
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);    
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_self_up_childof_2_lvl_children_from_child(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Bar);

    ecs_entity_t p = ecs_new(world);
    ecs_entity_t c_1 = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_add(world, c_1, Bar);
    ecs_add_id(world, c_1, EcsOrderedChildren);
    ecs_entity_t gc_1 = ecs_insert(world, ecs_value(EcsParent, {c_1}));

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { .id = Bar, .src.id = EcsSelf|EcsUp }
        },
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], c_1);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(Bar, ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], gc_1);
    test_uint(c_1, ecs_field_src(&it, 0));
    test_uint(Bar, ecs_field_id(&it, 0));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_self_up_childof_2_lvl_children_w_component_from_child(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t p = ecs_new(world);
    ecs_entity_t c_1 = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_set(world, c_1, Position, {10, 20});
    ecs_add_id(world, c_1, EcsOrderedChildren);
    ecs_entity_t gc_1 = ecs_insert(world, ecs_value(EcsParent, {c_1}));

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { .id = ecs_id(Position), .src.id = EcsSelf|EcsUp }
        },
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], c_1);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_int(10, p->x);
        test_int(20, p->y);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], gc_1);
    test_uint(c_1, ecs_field_src(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_int(10, p->x);
        test_int(20, p->y);
    }

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_self_up_childof_2_lvl_children_from_child_on_instantiate_inherit(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Bar);

    ecs_add_pair(world, Bar, EcsOnInstantiate, EcsInherit);

    ecs_entity_t p = ecs_new(world);
    ecs_entity_t b = ecs_new(world);
    ecs_add(world, b, Bar);
    ecs_entity_t c_1 = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_add_pair(world, c_1, EcsIsA, b);
    ecs_add_id(world, c_1, EcsOrderedChildren);
    ecs_entity_t gc_1 = ecs_insert(world, ecs_value(EcsParent, {c_1}));

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { .id = Bar, .src.id = EcsSelf|EcsUp }
        },
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], b);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(Bar, ecs_field_id(&it, 0));
    
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], gc_1);
    test_uint(b, ecs_field_src(&it, 0));
    test_uint(Bar, ecs_field_id(&it, 0));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_self_up_childof_2_lvl_children_from_child_on_instantiate_dont_inherit(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Bar);

    ecs_add_pair(world, Bar, EcsOnInstantiate, EcsDontInherit);

    ecs_entity_t p = ecs_new(world);
    ecs_entity_t b = ecs_new(world);
    ecs_add(world, b, Bar);
    ecs_entity_t c_1 = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_add_pair(world, c_1, EcsIsA, b);
    ecs_add_id(world, c_1, EcsOrderedChildren);
    ecs_insert(world, ecs_value(EcsParent, {c_1}));

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { .id = Bar, .src.id = EcsSelf|EcsUp }
        },
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], b);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(Bar, ecs_field_id(&it, 0));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_written_up_childof(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_entity_t p = ecs_new(world);
    ecs_add(world, p, Bar);
    ecs_entity_t c_1 = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_entity_t c_2 = ecs_insert(world, ecs_value(EcsParent, {p}));

    ecs_add(world, c_1, Foo);
    ecs_add(world, c_2, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { Foo },
            { .id = Bar, .src.id = EcsUp }
        },
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], c_1);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(p, ecs_field_src(&it, 1));
    test_uint(Bar, ecs_field_id(&it, 1));
    
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], c_2);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(p, ecs_field_src(&it, 1));
    test_uint(Bar, ecs_field_id(&it, 1));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_up_childof_1_table_3_parents(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_entity_t p_1 = ecs_new_w(world, Bar);
    ecs_entity_t p_2 = ecs_new_w(world, Bar);
    ecs_entity_t p_3 = ecs_new_w(world, Bar);
    ecs_entity_t c_1 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t c_2 = ecs_insert(world, ecs_value(EcsParent, {p_2}));
    ecs_entity_t c_3 = ecs_insert(world, ecs_value(EcsParent, {p_3}));

    ecs_add(world, c_1, Foo);
    ecs_add(world, c_2, Foo);
    ecs_add(world, c_3, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { .id = Bar, .src.id = EcsUp }
        },
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], c_1);
    test_uint(p_1, ecs_field_src(&it, 0));
    test_uint(Bar, ecs_field_id(&it, 0));
    
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], c_2);
    test_uint(p_2, ecs_field_src(&it, 0));
    test_uint(Bar, ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], c_3);
    test_uint(p_3, ecs_field_src(&it, 0));
    test_uint(Bar, ecs_field_id(&it, 0));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_up_childof_2_tables_3_parents(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_entity_t p_1 = ecs_new_w(world, Bar);
    ecs_entity_t p_2 = ecs_new_w(world, Bar);
    ecs_entity_t p_3 = ecs_new_w(world, Bar);
    ecs_entity_t c_1 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t c_2 = ecs_insert(world, ecs_value(EcsParent, {p_2}));
    ecs_entity_t c_3 = ecs_insert(world, ecs_value(EcsParent, {p_3}));

    ecs_add(world, c_2, Foo);
    ecs_add(world, c_3, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { .id = Bar, .src.id = EcsUp }
        },
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], c_1);
    test_uint(p_1, ecs_field_src(&it, 0));
    test_uint(Bar, ecs_field_id(&it, 0));
    
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], c_2);
    test_uint(p_2, ecs_field_src(&it, 0));
    test_uint(Bar, ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], c_3);
    test_uint(p_3, ecs_field_src(&it, 0));
    test_uint(Bar, ecs_field_id(&it, 0));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_written_single_child_w_recycled_id(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_entity_t p_1 = ecs_new(world);
    ecs_entity_t p_2 = ecs_new(world);

    ecs_entity_t e_old = ecs_new(world);
    ecs_delete(world, e_old);
    ecs_entity_t c_1 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    test_assert((uint32_t)c_1 == e_old);
    ecs_entity_t c_2 = ecs_insert(world, ecs_value(EcsParent, {p_2}));

    ecs_add(world, c_1, Foo);
    ecs_add(world, c_2, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { Foo },
            { ecs_childof(p_1) }
        },
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], c_1);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_childof(p_1), ecs_field_id(&it, 1));
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_written_single_child_after_two_children(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_entity_t p_1 = ecs_new(world);
    ecs_entity_t c_1 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t c_2 = ecs_insert(world, ecs_value(EcsParent, {p_1}));

    ecs_add(world, c_1, Foo);
    ecs_add(world, c_2, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { Foo },
            { ecs_childof(p_1) }
        },
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(it.entities[0], c_1);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_childof(p_1), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(it.entities[0], c_2);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_childof(p_1), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_delete(world, c_2);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(it.entities[0], c_1);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_childof(p_1), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_written_single_child_after_two_children_2(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_entity_t p_1 = ecs_new(world);
    ecs_entity_t c_1 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t c_2 = ecs_insert(world, ecs_value(EcsParent, {p_1}));

    ecs_add(world, c_1, Foo);
    ecs_add(world, c_2, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { Foo },
            { ecs_childof(p_1) }
        },
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(it.entities[0], c_1);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_childof(p_1), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(it.entities[0], c_2);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_childof(p_1), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_delete(world, c_1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(it.entities[0], c_2);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_childof(p_1), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_written_single_child_after_two_children_3(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_entity_t p_1 = ecs_new(world);
    ecs_entity_t p_2 = ecs_new(world);
    ecs_entity_t c_1 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t c_2 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t c_3 = ecs_insert(world, ecs_value(EcsParent, {p_2}));
    ecs_entity_t c_4 = ecs_insert(world, ecs_value(EcsParent, {p_2}));

    ecs_add(world, c_1, Foo);
    ecs_add(world, c_2, Foo);
    ecs_add(world, c_3, Foo);
    ecs_add(world, c_4, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { Foo },
            { ecs_childof(p_1) }
        },
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(it.entities[0], c_1);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_childof(p_1), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(it.entities[0], c_2);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_childof(p_1), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_delete(world, c_2);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(it.entities[0], c_1);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_childof(p_1), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_written_single_child_after_two_children_4(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_entity_t p_1 = ecs_new(world);
    ecs_entity_t p_2 = ecs_new(world);
    ecs_entity_t c_1 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t c_2 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t c_3 = ecs_insert(world, ecs_value(EcsParent, {p_2}));
    ecs_entity_t c_4 = ecs_insert(world, ecs_value(EcsParent, {p_2}));

    ecs_add(world, c_1, Foo);
    ecs_add(world, c_2, Foo);
    ecs_add(world, c_3, Foo);
    ecs_add(world, c_4, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { Foo },
            { ecs_childof(p_1) }
        },
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(it.entities[0], c_1);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_childof(p_1), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(it.entities[0], c_2);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_childof(p_1), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_delete(world, c_1);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(it.entities[0], c_2);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_childof(p_1), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_self_up_childof_1_table_3_parents(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_entity_t p_1 = ecs_new_w(world, Bar);
    ecs_entity_t p_2 = ecs_new_w(world, Bar);
    ecs_entity_t p_3 = ecs_new_w(world, Bar);
    ecs_entity_t c_1 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t c_2 = ecs_insert(world, ecs_value(EcsParent, {p_2}));
    ecs_entity_t c_3 = ecs_insert(world, ecs_value(EcsParent, {p_3}));

    ecs_add(world, c_1, Foo);
    ecs_add(world, c_2, Foo);
    ecs_add(world, c_3, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { .id = Bar, .src.id = EcsSelf|EcsUp }
        },
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(3, it.count);
    test_uint(it.entities[0], p_1);
    test_uint(it.entities[1], p_2);
    test_uint(it.entities[2], p_3);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(Bar, ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], c_1);
    test_uint(p_1, ecs_field_src(&it, 0));
    test_uint(Bar, ecs_field_id(&it, 0));
    
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], c_2);
    test_uint(p_2, ecs_field_src(&it, 0));
    test_uint(Bar, ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], c_3);
    test_uint(p_3, ecs_field_src(&it, 0));
    test_uint(Bar, ecs_field_id(&it, 0));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_self_up_childof_2_tables_3_parents(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_entity_t p_1 = ecs_new_w(world, Bar);
    ecs_entity_t p_2 = ecs_new_w(world, Bar);
    ecs_entity_t p_3 = ecs_new_w(world, Bar);
    ecs_entity_t c_1 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t c_2 = ecs_insert(world, ecs_value(EcsParent, {p_2}));
    ecs_entity_t c_3 = ecs_insert(world, ecs_value(EcsParent, {p_3}));

    ecs_add(world, c_2, Foo);
    ecs_add(world, c_3, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { .id = Bar, .src.id = EcsSelf|EcsUp }
        },
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(3, it.count);
    test_uint(it.entities[0], p_1);
    test_uint(it.entities[1], p_2);
    test_uint(it.entities[2], p_3);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(Bar, ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], c_1);
    test_uint(p_1, ecs_field_src(&it, 0));
    test_uint(Bar, ecs_field_id(&it, 0));
    
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], c_2);
    test_uint(p_2, ecs_field_src(&it, 0));
    test_uint(Bar, ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], c_3);
    test_uint(p_3, ecs_field_src(&it, 0));
    test_uint(Bar, ecs_field_id(&it, 0));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_written_up_childof_w_component(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_COMPONENT(world, Position);

    ecs_entity_t p = ecs_new(world);
    ecs_set(world, p, Position, {10, 20});
    ecs_entity_t c_1 = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_entity_t c_2 = ecs_insert(world, ecs_value(EcsParent, {p}));

    ecs_add(world, c_1, Foo);
    ecs_add(world, c_2, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { Foo },
            { .id = ecs_id(Position), .src.id = EcsUp }
        },
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], c_1);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(p, ecs_field_src(&it, 1));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_int(10, p->x);
        test_int(20, p->y);
    }
    
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], c_2);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(p, ecs_field_src(&it, 1));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_int(10, p->x);
        test_int(20, p->y);
    }

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_written_up_childof_w_component_inherited(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_COMPONENT(world, Position);

    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);

    ecs_entity_t b = ecs_new(world);
    ecs_set(world, b, Position, {10, 20});

    ecs_entity_t p = ecs_new_w_pair(world, EcsIsA, b);
    ecs_entity_t c_1 = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_entity_t c_2 = ecs_insert(world, ecs_value(EcsParent, {p}));

    ecs_add(world, p, Foo);
    ecs_add(world, c_1, Foo);
    ecs_add(world, c_2, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { Foo },
            { .id = ecs_id(Position), .src.id = EcsUp }
        },
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], c_1);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(b, ecs_field_src(&it, 1));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_int(10, p->x);
        test_int(20, p->y);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], c_2);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(b, ecs_field_src(&it, 1));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_int(10, p->x);
        test_int(20, p->y);
    }

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_written_up_childof_2_lvl(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_entity_t p = ecs_new(world);
    ecs_entity_t c_1 = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_entity_t c_2 = ecs_insert(world, ecs_value(EcsParent, {p}));

    ecs_add(world, p, Foo);
    ecs_add(world, c_1, Foo);
    ecs_add(world, c_2, Foo);

    ecs_entity_t parent = ecs_new(world);
    ecs_add(world, parent, Bar);
    ecs_add_pair(world, p, EcsChildOf, parent);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { Foo },
            { .id = Bar, .src.id = EcsUp }
        },
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], p);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(parent, ecs_field_src(&it, 1));
    test_uint(Bar, ecs_field_id(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], c_1);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(parent, ecs_field_src(&it, 1));
    test_uint(Bar, ecs_field_id(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], c_2);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(parent, ecs_field_src(&it, 1));
    test_uint(Bar, ecs_field_id(&it, 1));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_written_up_childof_2_lvl_w_component(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_COMPONENT(world, Position);

    ecs_entity_t p = ecs_new(world);
    ecs_entity_t c_1 = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_entity_t c_2 = ecs_insert(world, ecs_value(EcsParent, {p}));

    ecs_add(world, p, Foo);
    ecs_add(world, c_1, Foo);
    ecs_add(world, c_2, Foo);

    ecs_entity_t parent = ecs_new(world);
    ecs_set(world, parent, Position, {10, 20});
    ecs_add_pair(world, p, EcsChildOf, parent);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { Foo },
            { .id = ecs_id(Position), .src.id = EcsUp }
        },
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], p);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(parent, ecs_field_src(&it, 1));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_int(10, p->x);
        test_int(20, p->y);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], c_1);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(parent, ecs_field_src(&it, 1));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_int(10, p->x);
        test_int(20, p->y);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], c_2);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(parent, ecs_field_src(&it, 1));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_int(10, p->x);
        test_int(20, p->y);
    }

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_written_up_childof_2_lvl_w_component_inherited(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_COMPONENT(world, Position);

    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);

    ecs_entity_t p = ecs_new(world);
    ecs_entity_t c_1 = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_entity_t c_2 = ecs_insert(world, ecs_value(EcsParent, {p}));

    ecs_add(world, p, Foo);
    ecs_add(world, c_1, Foo);
    ecs_add(world, c_2, Foo);

    ecs_entity_t base = ecs_new(world);
    ecs_set(world, base, Position, {10, 20});
    ecs_entity_t parent = ecs_new_w_pair(world, EcsIsA, base);
    ecs_add_pair(world, p, EcsChildOf, parent);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { Foo },
            { .id = ecs_id(Position), .src.id = EcsUp }
        },
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);    
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], p);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));    
    test_uint(base, ecs_field_src(&it, 1));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_int(10, p->x);
        test_int(20, p->y);
    }

    test_bool(true, ecs_query_next(&it));    
    test_int(1, it.count);
    test_uint(it.entities[0], c_1);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(base, ecs_field_src(&it, 1));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_int(10, p->x);
        test_int(20, p->y);
    }

    test_bool(true, ecs_query_next(&it));    
    test_int(1, it.count);
    test_uint(it.entities[0], c_2);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(base, ecs_field_src(&it, 1));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_int(10, p->x);
        test_int(20, p->y);
    }

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_written_up_childof_2_lvl_w_owned(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_entity_t p = ecs_new(world);
    ecs_entity_t c_1 = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_entity_t c_2 = ecs_insert(world, ecs_value(EcsParent, {p}));

    ecs_add(world, p, Foo);
    ecs_add(world, c_1, Foo);
    ecs_add(world, c_2, Foo);
    ecs_add(world, c_2, Bar);

    ecs_entity_t parent = ecs_new(world);
    ecs_add(world, parent, Bar);
    ecs_add_pair(world, p, EcsChildOf, parent);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { Foo },
            { .id = Bar, .src.id = EcsUp }
        },
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], p);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(parent, ecs_field_src(&it, 1));
    test_uint(Bar, ecs_field_id(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], c_1);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(parent, ecs_field_src(&it, 1));
    test_uint(Bar, ecs_field_id(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], c_2);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(parent, ecs_field_src(&it, 1));
    test_uint(Bar, ecs_field_id(&it, 1));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_written_self_up_childof(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_entity_t p = ecs_new(world);
    ecs_add(world, p, Bar);
    ecs_entity_t c_1 = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_entity_t c_2 = ecs_insert(world, ecs_value(EcsParent, {p}));

    ecs_add(world, p, Foo);
    ecs_add(world, c_1, Foo);
    ecs_add(world, c_2, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { Foo },
            { .id = Bar, .src.id = EcsSelf|EcsUp }
        },
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], p);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    test_uint(Bar, ecs_field_id(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], c_1);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(p, ecs_field_src(&it, 1));
    test_uint(Bar, ecs_field_id(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], c_2);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(p, ecs_field_src(&it, 1));
    test_uint(Bar, ecs_field_id(&it, 1));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_written_self_up_childof_w_component(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_COMPONENT(world, Position);

    ecs_entity_t p = ecs_new(world);
    ecs_set(world, p, Position, {10, 20});
    ecs_entity_t c_1 = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_entity_t c_2 = ecs_insert(world, ecs_value(EcsParent, {p}));

    ecs_add(world, p, Foo);
    ecs_add(world, c_1, Foo);
    ecs_add(world, c_2, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { Foo },
            { .id = ecs_id(Position), .src.id = EcsSelf|EcsUp }
        },
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], p);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_int(10, p->x);
        test_int(20, p->y);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], c_1);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(p, ecs_field_src(&it, 1));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_int(10, p[0].x); test_int(20, p[0].y);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], c_2);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(p, ecs_field_src(&it, 1));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_int(10, p[0].x); test_int(20, p[0].y);
    }

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_written_self_up_childof_w_component_inherited(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_COMPONENT(world, Position);

    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);

    ecs_entity_t b = ecs_new(world);
    ecs_set(world, b, Position, {10, 20});
    ecs_entity_t p = ecs_new(world);
    ecs_add_pair(world, p, EcsIsA, b);
    ecs_entity_t c_1 = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_entity_t c_2 = ecs_insert(world, ecs_value(EcsParent, {p}));

    ecs_add(world, b, Foo);
    ecs_add(world, p, Foo);
    ecs_add(world, c_1, Foo);
    ecs_add(world, c_2, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { Foo },
            { .id = ecs_id(Position), .src.id = EcsSelf|EcsUp }
        },
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], b);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0)); 
    test_uint(0, ecs_field_src(&it, 1));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_int(10, p->x);
        test_int(20, p->y);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], c_1);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0)); 
    test_uint(b, ecs_field_src(&it, 1));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_int(10, p->x);
        test_int(20, p->y);
    }
    
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], c_2);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0)); 
    test_uint(b, ecs_field_src(&it, 1));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_int(10, p->x);
        test_int(20, p->y);
    }

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_written_self_up_childof_2_lvl(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_entity_t p = ecs_new(world);
    ecs_entity_t c_1 = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_entity_t c_2 = ecs_insert(world, ecs_value(EcsParent, {p}));

    ecs_add(world, p, Foo);
    ecs_add(world, c_1, Foo);
    ecs_add(world, c_2, Foo);

    ecs_entity_t parent = ecs_new(world);
    ecs_add(world, parent, Foo);
    ecs_add(world, parent, Bar);
    ecs_add_pair(world, p, EcsChildOf, parent);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { Foo },
            { .id = Bar, .src.id = EcsSelf|EcsUp }
        },
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], parent);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    test_uint(Bar, ecs_field_id(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], p);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(parent, ecs_field_src(&it, 1));
    test_uint(Bar, ecs_field_id(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], c_1);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(parent, ecs_field_src(&it, 1));
    test_uint(Bar, ecs_field_id(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], c_2);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(parent, ecs_field_src(&it, 1));
    test_uint(Bar, ecs_field_id(&it, 1));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_written_self_up_childof_2_lvl_w_component(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_COMPONENT(world, Position);

    ecs_entity_t p = ecs_new(world);
    ecs_entity_t c_1 = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_entity_t c_2 = ecs_insert(world, ecs_value(EcsParent, {p}));

    ecs_add(world, p, Foo);
    ecs_add(world, c_1, Foo);
    ecs_add(world, c_2, Foo);

    ecs_entity_t parent = ecs_new(world);
    ecs_add(world, parent, Foo);
    ecs_set(world, parent, Position, {10, 20});
    ecs_add_pair(world, p, EcsChildOf, parent);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { Foo },
            { .id = ecs_id(Position), .src.id = EcsSelf|EcsUp }
        },
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], parent);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_int(10, p->x);
        test_int(20, p->y);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], p);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(parent, ecs_field_src(&it, 1));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_int(10, p->x);
        test_int(20, p->y);
    }
    
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], c_1);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(parent, ecs_field_src(&it, 1));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_int(10, p->x);
        test_int(20, p->y);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], c_2);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(parent, ecs_field_src(&it, 1));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_int(10, p->x);
        test_int(20, p->y);
    }

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_written_self_up_childof_2_lvl_w_component_inherited(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_COMPONENT(world, Position);

    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);

    ecs_entity_t p = ecs_new(world);
    ecs_entity_t c_1 = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_entity_t c_2 = ecs_insert(world, ecs_value(EcsParent, {p}));

    ecs_add(world, p, Foo);
    ecs_add(world, c_1, Foo);
    ecs_add(world, c_2, Foo);

    ecs_entity_t base = ecs_new(world);
    ecs_add(world, base, Foo);
    ecs_set(world, base, Position, {10, 20});
    ecs_entity_t parent = ecs_new_w_pair(world, EcsIsA, base);
    ecs_add(world, parent, Foo);
    ecs_add_pair(world, p, EcsChildOf, parent);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { Foo },
            { .id = ecs_id(Position), .src.id = EcsSelf|EcsUp }
        },
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], base);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_int(10, p->x);
        test_int(20, p->y);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], p);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(base, ecs_field_src(&it, 1));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_int(10, p->x);
        test_int(20, p->y);
    }

    test_bool(true, ecs_query_next(&it));    
    test_int(1, it.count);
    test_uint(it.entities[0], c_1);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(base, ecs_field_src(&it, 1));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_int(10, p->x);
        test_int(20, p->y);
    }

    test_bool(true, ecs_query_next(&it));    
    test_int(1, it.count);
    test_uint(it.entities[0], c_2);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(base, ecs_field_src(&it, 1));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_int(10, p->x);
        test_int(20, p->y);
    }

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_written_self_up_childof_2_lvl_w_owned(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_entity_t p = ecs_new(world);
    ecs_entity_t c_1 = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_entity_t c_2 = ecs_insert(world, ecs_value(EcsParent, {p}));

    ecs_add(world, p, Foo);
    ecs_add(world, c_1, Foo);
    ecs_add(world, c_2, Foo);
    ecs_add(world, c_2, Bar);

    ecs_entity_t parent = ecs_new(world);
    ecs_add(world, parent, Foo);
    ecs_add(world, parent, Bar);
    ecs_add_pair(world, p, EcsChildOf, parent);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { Foo },
            { .id = Bar, .src.id = EcsSelf|EcsUp }
        },
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], parent);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    test_uint(Bar, ecs_field_id(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], p);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(parent, ecs_field_src(&it, 1));
    test_uint(Bar, ecs_field_id(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], c_1);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(parent, ecs_field_src(&it, 1));
    test_uint(Bar, ecs_field_id(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], c_2);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    test_uint(Bar, ecs_field_id(&it, 1));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_written_up_childof_2_lvl_children(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_entity_t p = ecs_new(world);
    ecs_add(world, p, Bar);
    ecs_entity_t c_1 = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_entity_t c_2 = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_add_id(world, c_1, EcsOrderedChildren);
    ecs_entity_t gc_1 = ecs_insert(world, ecs_value(EcsParent, {c_1}));

    ecs_add(world, c_1, Foo);
    ecs_add(world, c_2, Foo);
    ecs_add(world, gc_1, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { Foo },
            { .id = Bar, .src.id = EcsUp }
        },
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], c_1);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(p, ecs_field_src(&it, 1));
    test_uint(Bar, ecs_field_id(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], c_2);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(p, ecs_field_src(&it, 1));
    test_uint(Bar, ecs_field_id(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], gc_1);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(p, ecs_field_src(&it, 1));
    test_uint(Bar, ecs_field_id(&it, 1));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_written_up_childof_2_lvl_children_w_component(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Foo);

    ecs_entity_t p = ecs_new(world);
    ecs_set(world, p, Position, {10, 20});
    ecs_entity_t c_1 = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_entity_t c_2 = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_add_id(world, c_1, EcsOrderedChildren);
    ecs_entity_t gc_1 = ecs_insert(world, ecs_value(EcsParent, {c_1}));

    ecs_add(world, c_1, Foo);
    ecs_add(world, c_2, Foo);
    ecs_add(world, gc_1, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { Foo },
            { .id = ecs_id(Position), .src.id = EcsUp }
        },
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], c_1);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(p, ecs_field_src(&it, 1));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_int(10, p->x);
        test_int(20, p->y);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], c_2);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(p, ecs_field_src(&it, 1));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_int(10, p->x);
        test_int(20, p->y);
    }
    
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], gc_1);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(p, ecs_field_src(&it, 1));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_int(10, p->x);
        test_int(20, p->y);
    }

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_written_up_childof_2_lvl_children_on_instantiate_inherit(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Bar, (OnInstantiate, Inherit));
    ECS_TAG(world, Foo);

    ecs_entity_t b = ecs_new(world);
    ecs_add(world, b, Bar);
    ecs_entity_t p = ecs_new(world);
    ecs_add_pair(world, p, EcsIsA, b);
    ecs_entity_t c_1 = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_entity_t c_2 = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_add_id(world, c_1, EcsOrderedChildren);
    ecs_entity_t gc_1 = ecs_insert(world, ecs_value(EcsParent, {c_1}));

    ecs_add(world, c_1, Foo);
    ecs_add(world, c_2, Foo);
    ecs_add(world, gc_1, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { Foo },
            { .id = Bar, .src.id = EcsUp }
        },
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);    
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], c_1);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(b, ecs_field_src(&it, 1));
    test_uint(Bar, ecs_field_id(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], c_2);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(b, ecs_field_src(&it, 1));
    test_uint(Bar, ecs_field_id(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], gc_1);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(b, ecs_field_src(&it, 1));
    test_uint(Bar, ecs_field_id(&it, 1));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_written_up_childof_2_lvl_children_w_component_on_instantiate_inherit(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Foo);

    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);

    ecs_entity_t b = ecs_new(world);
    ecs_set(world, b, Position, {10, 20});
    ecs_entity_t p = ecs_new(world);
    ecs_add_pair(world, p, EcsIsA, b);

    ecs_entity_t c_1 = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_entity_t c_2 = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_add_id(world, c_1, EcsOrderedChildren);
    ecs_entity_t gc_1 = ecs_insert(world, ecs_value(EcsParent, {c_1}));

    ecs_add(world, c_1, Foo);
    ecs_add(world, c_2, Foo);
    ecs_add(world, gc_1, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { Foo },
            { .id = ecs_id(Position), .src.id = EcsUp }
        },
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);    
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], c_1);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(b, ecs_field_src(&it, 1));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_int(10, p->x);
        test_int(20, p->y);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], c_2);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(b, ecs_field_src(&it, 1));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_int(10, p->x);
        test_int(20, p->y);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], gc_1);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(b, ecs_field_src(&it, 1));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_int(10, p->x);
        test_int(20, p->y);
    }

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_written_up_childof_2_lvl_children_on_instantiate_dont_inherit(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Bar, (OnInstantiate, DontInherit));
    ECS_TAG(world, Foo);

    ecs_entity_t b = ecs_new(world);
    ecs_add(world, b, Bar);
    ecs_entity_t p = ecs_new(world);
    ecs_add_pair(world, p, EcsIsA, b);
    ecs_entity_t c_1 = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_entity_t c_2 = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_add_id(world, c_1, EcsOrderedChildren);
    ecs_entity_t gc_1 = ecs_insert(world, ecs_value(EcsParent, {c_1}));

    ecs_add(world, c_1, Foo);
    ecs_add(world, c_2, Foo);
    ecs_add(world, gc_1, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { Foo },
            { .id = Bar, .src.id = EcsUp }
        },
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_written_self_up_childof_2_lvl_children(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_entity_t p = ecs_new(world);
    ecs_add(world, p, Bar);
    ecs_entity_t c_1 = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_entity_t c_2 = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_add_id(world, c_1, EcsOrderedChildren);
    ecs_entity_t gc_1 = ecs_insert(world, ecs_value(EcsParent, {c_1}));

    ecs_add(world, p, Foo);
    ecs_add(world, c_1, Foo);
    ecs_add(world, c_2, Foo);
    ecs_add(world, gc_1, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { Foo },
            { .id = Bar, .src.id = EcsSelf|EcsUp }
        },
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], p);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    test_uint(Bar, ecs_field_id(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], c_1);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(p, ecs_field_src(&it, 1));
    test_uint(Bar, ecs_field_id(&it, 1));
    
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], c_2);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(p, ecs_field_src(&it, 1));
    test_uint(Bar, ecs_field_id(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], gc_1);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(p, ecs_field_src(&it, 1));
    test_uint(Bar, ecs_field_id(&it, 1));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_written_self_up_childof_2_lvl_children_w_component(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Foo);

    ecs_entity_t p = ecs_new(world);
    ecs_set(world, p, Position, {10, 20});
    ecs_entity_t c_1 = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_entity_t c_2 = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_add_id(world, c_1, EcsOrderedChildren);
    ecs_entity_t gc_1 = ecs_insert(world, ecs_value(EcsParent, {c_1}));

    ecs_add(world, p, Foo);
    ecs_add(world, c_1, Foo);
    ecs_add(world, c_2, Foo);
    ecs_add(world, gc_1, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { Foo },
            { .id = ecs_id(Position), .src.id = EcsSelf|EcsUp }
        },
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], p);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_int(10, p->x);
        test_int(20, p->y);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], c_1);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(p, ecs_field_src(&it, 1));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_int(10, p[0].x); test_int(20, p[0].y);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], c_2);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(p, ecs_field_src(&it, 1));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_int(10, p[0].x); test_int(20, p[0].y);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], gc_1);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(p, ecs_field_src(&it, 1));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_int(10, p->x);
        test_int(20, p->y);
    }

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_written_self_up_childof_2_lvl_children_on_instantiate_inherit(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_ENTITY(world, Bar, (OnInstantiate, Inherit));

    ecs_entity_t b = ecs_new(world);
    ecs_add(world, b, Bar);
    ecs_entity_t p = ecs_new(world);
    ecs_add_pair(world, p, EcsIsA, b);
    ecs_entity_t c_1 = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_entity_t c_2 = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_add_id(world, c_1, EcsOrderedChildren);
    ecs_entity_t gc_1 = ecs_insert(world, ecs_value(EcsParent, {c_1}));

    ecs_add(world, b, Foo);
    ecs_add(world, p, Foo);
    ecs_add(world, c_1, Foo);
    ecs_add(world, c_2, Foo);
    ecs_add(world, gc_1, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { Foo },
            { .id = Bar, .src.id = EcsSelf|EcsUp }
        },
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], b);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    test_uint(Bar, ecs_field_id(&it, 1));
    
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], c_1);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(b, ecs_field_src(&it, 1));
    test_uint(Bar, ecs_field_id(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], c_2);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(b, ecs_field_src(&it, 1));
    test_uint(Bar, ecs_field_id(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], gc_1);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(b, ecs_field_src(&it, 1));
    test_uint(Bar, ecs_field_id(&it, 1));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_written_self_up_childof_2_lvl_children_on_instantiate_dont_inherit(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_ENTITY(world, Bar, (OnInstantiate, DontInherit));

    ecs_entity_t b = ecs_new(world);
    ecs_add(world, b, Bar);
    ecs_entity_t p = ecs_new(world);
    ecs_add_pair(world, p, EcsIsA, b);
    ecs_entity_t c_1 = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_add_id(world, c_1, EcsOrderedChildren);
    ecs_entity_t gc_1 = ecs_insert(world, ecs_value(EcsParent, {c_1}));

    ecs_add(world, b, Foo);
    ecs_add(world, c_1, Foo);
    ecs_add(world, gc_1, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { Foo },
            { .id = Bar, .src.id = EcsSelf|EcsUp }
        },
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], b);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    test_uint(Bar, ecs_field_id(&it, 1));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_written_up_childof_2_lvl_children_from_child(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_entity_t p = ecs_new(world);
    ecs_entity_t c_1 = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_add(world, c_1, Bar);
    ecs_add_id(world, c_1, EcsOrderedChildren);
    ecs_entity_t gc_1 = ecs_insert(world, ecs_value(EcsParent, {c_1}));

    ecs_add(world, c_1, Foo);
    ecs_add(world, gc_1, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { Foo },
            { .id = Bar, .src.id = EcsUp }
        },
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);    
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], gc_1);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(c_1, ecs_field_src(&it, 1));
    test_uint(Bar, ecs_field_id(&it, 1));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_written_up_childof_2_lvl_children_w_component_from_child(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Foo);

    ecs_entity_t p = ecs_new(world);
    ecs_entity_t c_1 = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_set(world, c_1, Position, {10, 20});
    ecs_add_id(world, c_1, EcsOrderedChildren);
    ecs_entity_t gc_1 = ecs_insert(world, ecs_value(EcsParent, {c_1}));

    ecs_add(world, c_1, Foo);
    ecs_add(world, gc_1, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { Foo },
            { .id = ecs_id(Position), .src.id = EcsUp }
        },
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);    
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], gc_1);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(c_1, ecs_field_src(&it, 1));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_int(10, p->x);
        test_int(20, p->y);
    }

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_written_up_childof_2_lvl_children_from_child_on_instantiate_inherit(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_add_pair(world, Bar, EcsOnInstantiate, EcsInherit);

    ecs_entity_t p = ecs_new(world);
    ecs_entity_t b = ecs_new(world);
    ecs_add(world, b, Bar);
    ecs_entity_t c_1 = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_add_pair(world, c_1, EcsIsA, b);
    ecs_add_id(world, c_1, EcsOrderedChildren);
    ecs_entity_t gc_1 = ecs_insert(world, ecs_value(EcsParent, {c_1}));

    ecs_add(world, c_1, Foo);
    ecs_add(world, gc_1, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { Foo },
            { .id = Bar, .src.id = EcsUp }
        },
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);    
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], gc_1);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(b, ecs_field_src(&it, 1));
    test_uint(Bar, ecs_field_id(&it, 1));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_written_up_childof_2_lvl_children_from_child_on_instantiate_dont_inherit(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_add_pair(world, Bar, EcsOnInstantiate, EcsDontInherit);

    ecs_entity_t p = ecs_new(world);
    ecs_entity_t b = ecs_new(world);
    ecs_add(world, b, Bar);
    ecs_entity_t c_1 = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_add_pair(world, c_1, EcsIsA, b);
    ecs_add_id(world, c_1, EcsOrderedChildren);
    ecs_entity_t gc_1 = ecs_insert(world, ecs_value(EcsParent, {c_1}));

    ecs_add(world, c_1, Foo);
    ecs_add(world, gc_1, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { Foo },
            { .id = Bar, .src.id = EcsUp }
        },
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);    
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_written_self_up_childof_2_lvl_children_from_child(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_entity_t p = ecs_new(world);
    ecs_entity_t c_1 = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_add(world, c_1, Bar);
    ecs_add_id(world, c_1, EcsOrderedChildren);
    ecs_entity_t gc_1 = ecs_insert(world, ecs_value(EcsParent, {c_1}));

    ecs_add(world, c_1, Foo);
    ecs_add(world, gc_1, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { Foo },
            { .id = Bar, .src.id = EcsSelf|EcsUp }
        },
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], c_1);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    test_uint(Bar, ecs_field_id(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], gc_1);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(c_1, ecs_field_src(&it, 1));
    test_uint(Bar, ecs_field_id(&it, 1));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_written_self_up_childof_2_lvl_children_w_component_from_child(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Foo);

    ecs_entity_t p = ecs_new(world);
    ecs_entity_t c_1 = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_set(world, c_1, Position, {10, 20});
    ecs_add_id(world, c_1, EcsOrderedChildren);
    ecs_entity_t gc_1 = ecs_insert(world, ecs_value(EcsParent, {c_1}));

    ecs_add(world, c_1, Foo);
    ecs_add(world, gc_1, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { Foo },
            { .id = ecs_id(Position), .src.id = EcsSelf|EcsUp }
        },
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], c_1);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_int(10, p->x);
        test_int(20, p->y);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], gc_1);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(c_1, ecs_field_src(&it, 1));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_int(10, p->x);
        test_int(20, p->y);
    }

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_written_self_up_childof_2_lvl_children_from_child_on_instantiate_inherit(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_add_pair(world, Bar, EcsOnInstantiate, EcsInherit);

    ecs_entity_t p = ecs_new(world);
    ecs_entity_t b = ecs_new(world);
    ecs_add(world, b, Bar);
    ecs_entity_t c_1 = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_add_pair(world, c_1, EcsIsA, b);
    ecs_add_id(world, c_1, EcsOrderedChildren);
    ecs_entity_t gc_1 = ecs_insert(world, ecs_value(EcsParent, {c_1}));

    ecs_add(world, b, Foo);
    ecs_add(world, p, Foo);
    ecs_add(world, c_1, Foo);
    ecs_add(world, gc_1, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { Foo },
            { .id = Bar, .src.id = EcsSelf|EcsUp }
        },
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], b);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    test_uint(Bar, ecs_field_id(&it, 1));
    
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], gc_1);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(b, ecs_field_src(&it, 1));
    test_uint(Bar, ecs_field_id(&it, 1));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_written_self_up_childof_2_lvl_children_from_child_on_instantiate_dont_inherit(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_add_pair(world, Bar, EcsOnInstantiate, EcsDontInherit);

    ecs_entity_t p = ecs_new(world);
    ecs_entity_t b = ecs_new(world);
    ecs_add(world, b, Bar);
    ecs_entity_t c_1 = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_add_pair(world, c_1, EcsIsA, b);
    ecs_add_id(world, c_1, EcsOrderedChildren);
    ecs_entity_t gc_1 = ecs_insert(world, ecs_value(EcsParent, {c_1}));

    ecs_add(world, b, Foo);
    ecs_add(world, c_1, Foo);
    ecs_add(world, gc_1, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { Foo },
            { .id = Bar, .src.id = EcsSelf|EcsUp }
        },
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], b);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    test_uint(Bar, ecs_field_id(&it, 1));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_written_up_childof_1_table_3_parents(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_entity_t p_1 = ecs_new_w(world, Bar);
    ecs_entity_t p_2 = ecs_new_w(world, Bar);
    ecs_entity_t p_3 = ecs_new_w(world, Bar);
    ecs_entity_t c_1 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t c_2 = ecs_insert(world, ecs_value(EcsParent, {p_2}));
    ecs_entity_t c_3 = ecs_insert(world, ecs_value(EcsParent, {p_3}));

    ecs_add(world, c_1, Foo);
    ecs_add(world, c_2, Foo);
    ecs_add(world, c_3, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { Foo },
            { .id = Bar, .src.id = EcsUp }
        },
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], c_1);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(p_1, ecs_field_src(&it, 1));
    test_uint(Bar, ecs_field_id(&it, 1));
    
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], c_2);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(p_2, ecs_field_src(&it, 1));
    test_uint(Bar, ecs_field_id(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], c_3);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(p_3, ecs_field_src(&it, 1));
    test_uint(Bar, ecs_field_id(&it, 1));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_written_up_childof_2_tables_3_parents(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);
    ECS_TAG(world, Hello);

    ecs_entity_t p_1 = ecs_new_w(world, Bar);
    ecs_entity_t p_2 = ecs_new_w(world, Bar);
    ecs_entity_t p_3 = ecs_new_w(world, Bar);
    ecs_entity_t c_1 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t c_2 = ecs_insert(world, ecs_value(EcsParent, {p_2}));
    ecs_entity_t c_3 = ecs_insert(world, ecs_value(EcsParent, {p_3}));

    ecs_add(world, c_1, Hello);

    ecs_add(world, c_1, Foo);
    ecs_add(world, c_2, Foo);
    ecs_add(world, c_3, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { Foo },
            { .id = Bar, .src.id = EcsUp }
        },
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], c_1);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(p_1, ecs_field_src(&it, 1));
    test_uint(Bar, ecs_field_id(&it, 1));
    
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], c_2);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(p_2, ecs_field_src(&it, 1));
    test_uint(Bar, ecs_field_id(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], c_3);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(p_3, ecs_field_src(&it, 1));
    test_uint(Bar, ecs_field_id(&it, 1));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_written_self_up_childof_1_table_3_parents(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_entity_t p_1 = ecs_new_w(world, Bar);
    ecs_entity_t p_2 = ecs_new_w(world, Bar);
    ecs_entity_t p_3 = ecs_new_w(world, Bar);
    ecs_entity_t c_1 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t c_2 = ecs_insert(world, ecs_value(EcsParent, {p_2}));
    ecs_entity_t c_3 = ecs_insert(world, ecs_value(EcsParent, {p_3}));

    ecs_add(world, p_1, Foo);
    ecs_add(world, p_2, Foo);
    ecs_add(world, p_3, Foo);

    ecs_add(world, c_1, Foo);
    ecs_add(world, c_2, Foo);
    ecs_add(world, c_3, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { Foo },
            { .id = Bar, .src.id = EcsSelf|EcsUp }
        },
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(3, it.count);
    test_uint(it.entities[0], p_1);
    test_uint(it.entities[1], p_2);
    test_uint(it.entities[2], p_3);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    test_uint(Bar, ecs_field_id(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], c_1);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(p_1, ecs_field_src(&it, 1));
    test_uint(Bar, ecs_field_id(&it, 1));
    
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], c_2);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(p_2, ecs_field_src(&it, 1));
    test_uint(Bar, ecs_field_id(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], c_3);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(p_3, ecs_field_src(&it, 1));
    test_uint(Bar, ecs_field_id(&it, 1));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_written_self_up_childof_2_tables_3_parents(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);
    ECS_TAG(world, Hello);

    ecs_entity_t p_1 = ecs_new_w(world, Bar);
    ecs_entity_t p_2 = ecs_new_w(world, Bar);
    ecs_entity_t p_3 = ecs_new_w(world, Bar);
    ecs_entity_t c_1 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t c_2 = ecs_insert(world, ecs_value(EcsParent, {p_2}));
    ecs_entity_t c_3 = ecs_insert(world, ecs_value(EcsParent, {p_3}));

    ecs_add(world, c_1, Hello);

    ecs_add(world, p_1, Foo);
    ecs_add(world, p_2, Foo);
    ecs_add(world, p_3, Foo);

    ecs_add(world, c_1, Foo);
    ecs_add(world, c_2, Foo);
    ecs_add(world, c_3, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { Foo },
            { .id = Bar, .src.id = EcsSelf|EcsUp }
        },
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(3, it.count);
    test_uint(it.entities[0], p_1);
    test_uint(it.entities[1], p_2);
    test_uint(it.entities[2], p_3);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    test_uint(Bar, ecs_field_id(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], c_1);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(p_1, ecs_field_src(&it, 1));
    test_uint(Bar, ecs_field_id(&it, 1));
    
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], c_2);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(p_2, ecs_field_src(&it, 1));
    test_uint(Bar, ecs_field_id(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], c_3);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(p_3, ecs_field_src(&it, 1));
    test_uint(Bar, ecs_field_id(&it, 1));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_up_w_3_levels_ccc(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e0 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e1 = ecs_new_w_pair(world, EcsChildOf, e0);
    ecs_entity_t e2 = ecs_new_w_pair(world, EcsChildOf, e1);
    ecs_entity_t e3 = ecs_new_w_pair(world, EcsChildOf, e2);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ ecs_id(Position), .src.id = EcsUp }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_uint(e1, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(e0, ecs_field_src(&it, 0));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_uint(e2, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(e0, ecs_field_src(&it, 0));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_uint(e3, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(e0, ecs_field_src(&it, 0));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_up_w_3_levels_ccp(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e0 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e1 = ecs_new_w_pair(world, EcsChildOf, e0);
    ecs_entity_t e2 = ecs_new_w_pair(world, EcsChildOf, e1);
    ecs_entity_t e3 = ecs_insert(world, ecs_value(EcsParent, {e2}));

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ ecs_id(Position), .src.id = EcsUp }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_uint(e1, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(e0, ecs_field_src(&it, 0));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_uint(e2, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(e0, ecs_field_src(&it, 0));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_uint(e3, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(e0, ecs_field_src(&it, 0));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_up_w_3_levels_cpc(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e0 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e1 = ecs_new_w_pair(world, EcsChildOf, e0);
    ecs_entity_t e2 = ecs_insert(world, ecs_value(EcsParent, {e1}));
    ecs_entity_t e3 = ecs_new_w_pair(world, EcsChildOf, e2);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ ecs_id(Position), .src.id = EcsUp }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_uint(e1, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(e0, ecs_field_src(&it, 0));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_uint(cache_kind == EcsQueryCacheAuto ? e3 : e2, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(e0, ecs_field_src(&it, 0));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_uint(cache_kind == EcsQueryCacheAuto ? e2 : e3, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(e0, ecs_field_src(&it, 0));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_up_w_3_levels_cpp(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e0 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e1 = ecs_new_w_pair(world, EcsChildOf, e0);
    ecs_entity_t e2 = ecs_insert(world, ecs_value(EcsParent, {e1}));
    ecs_entity_t e3 = ecs_insert(world, ecs_value(EcsParent, {e2}));

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ ecs_id(Position), .src.id = EcsUp }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_uint(e1, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(e0, ecs_field_src(&it, 0));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_uint(e2, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(e0, ecs_field_src(&it, 0));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_uint(e3, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(e0, ecs_field_src(&it, 0));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_up_w_3_levels_pcc(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e0 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e1 = ecs_insert(world, ecs_value(EcsParent, {e0}));
    ecs_entity_t e2 = ecs_new_w_pair(world, EcsChildOf, e1);
    ecs_entity_t e3 = ecs_new_w_pair(world, EcsChildOf, e2);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ ecs_id(Position), .src.id = EcsUp }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_uint(cache_kind == EcsQueryCacheAuto ? e2 : e1, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(e0, ecs_field_src(&it, 0));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_uint(cache_kind == EcsQueryCacheAuto ? e3 : e2, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(e0, ecs_field_src(&it, 0));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_uint(cache_kind == EcsQueryCacheAuto ? e1 : e3, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(e0, ecs_field_src(&it, 0));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_up_w_3_levels_pcp(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e0 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e1 = ecs_insert(world, ecs_value(EcsParent, {e0}));
    ecs_entity_t e2 = ecs_new_w_pair(world, EcsChildOf, e1);
    ecs_entity_t e3 = ecs_insert(world, ecs_value(EcsParent, {e2}));

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ ecs_id(Position), .src.id = EcsUp }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_uint(cache_kind == EcsQueryCacheAuto ? e2 : e1, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(e0, ecs_field_src(&it, 0));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_uint(cache_kind == EcsQueryCacheAuto ? e1 : e2, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(e0, ecs_field_src(&it, 0));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_uint(e3, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(e0, ecs_field_src(&it, 0));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_up_w_3_levels_ppc(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e0 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e1 = ecs_insert(world, ecs_value(EcsParent, {e0}));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(EcsParent, {e1}));
    ecs_entity_t e3 = ecs_new_w_pair(world, EcsChildOf, e2);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ ecs_id(Position), .src.id = EcsUp }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_uint(cache_kind == EcsQueryCacheAuto ? e3 : e1, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(e0, ecs_field_src(&it, 0));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_uint(cache_kind == EcsQueryCacheAuto ? e1 : e2, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(e0, ecs_field_src(&it, 0));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_uint(cache_kind == EcsQueryCacheAuto ? e2 : e3, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(e0, ecs_field_src(&it, 0));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_up_w_3_levels_ppp(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e0 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e1 = ecs_insert(world, ecs_value(EcsParent, {e0}));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(EcsParent, {e1}));
    ecs_entity_t e3 = ecs_insert(world, ecs_value(EcsParent, {e2}));

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ ecs_id(Position), .src.id = EcsUp }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_uint(e1, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(e0, ecs_field_src(&it, 0));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_uint(e2, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(e0, ecs_field_src(&it, 0));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_uint(e3, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(e0, ecs_field_src(&it, 0));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_tag_w_up_w_3_levels_ccc(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Foo);

    ecs_entity_t e0 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e1 = ecs_new_w_pair(world, EcsChildOf, e0);
    ecs_entity_t e2 = ecs_new_w_pair(world, EcsChildOf, e1);
    ecs_entity_t e3 = ecs_new_w_pair(world, EcsChildOf, e2);

    ecs_add(world, e0, Foo);
    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);
    ecs_add(world, e3, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ Foo }, { ecs_id(Position), .src.id = EcsUp }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_uint(e1, it.entities[0]);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(e0, ecs_field_src(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_uint(e2, it.entities[0]);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(e0, ecs_field_src(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_uint(e3, it.entities[0]);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(e0, ecs_field_src(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_tag_w_up_w_3_levels_ccp(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Foo);

    ecs_entity_t e0 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e1 = ecs_new_w_pair(world, EcsChildOf, e0);
    ecs_entity_t e2 = ecs_new_w_pair(world, EcsChildOf, e1);
    ecs_entity_t e3 = ecs_insert(world, ecs_value(EcsParent, {e2}));

    ecs_add(world, e0, Foo);
    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);
    ecs_add(world, e3, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ Foo }, { ecs_id(Position), .src.id = EcsUp }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_uint(e1, it.entities[0]);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(e0, ecs_field_src(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_uint(e2, it.entities[0]);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(e0, ecs_field_src(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_uint(e3, it.entities[0]);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(e0, ecs_field_src(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_tag_w_up_w_3_levels_cpc(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Foo);

    ecs_entity_t e0 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e1 = ecs_new_w_pair(world, EcsChildOf, e0);
    ecs_entity_t e2 = ecs_insert(world, ecs_value(EcsParent, {e1}));
    ecs_entity_t e3 = ecs_new_w_pair(world, EcsChildOf, e2);

    ecs_add(world, e0, Foo);
    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);
    ecs_add(world, e3, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ Foo }, { ecs_id(Position), .src.id = EcsUp }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_uint(e1, it.entities[0]);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(e0, ecs_field_src(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_uint(e2, it.entities[0]);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(e0, ecs_field_src(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_uint(e3, it.entities[0]);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(e0, ecs_field_src(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_tag_w_up_w_3_levels_cpp(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Foo);

    ecs_entity_t e0 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e1 = ecs_new_w_pair(world, EcsChildOf, e0);
    ecs_entity_t e2 = ecs_insert(world, ecs_value(EcsParent, {e1}));
    ecs_entity_t e3 = ecs_insert(world, ecs_value(EcsParent, {e2}));

    ecs_add(world, e0, Foo);
    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);
    ecs_add(world, e3, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ Foo }, { ecs_id(Position), .src.id = EcsUp }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_uint(e1, it.entities[0]);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(e0, ecs_field_src(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_uint(e2, it.entities[0]);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(e0, ecs_field_src(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_uint(e3, it.entities[0]);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(e0, ecs_field_src(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_tag_w_up_w_3_levels_pcc(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Foo);

    ecs_entity_t e0 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e1 = ecs_insert(world, ecs_value(EcsParent, {e0}));
    ecs_entity_t e2 = ecs_new_w_pair(world, EcsChildOf, e1);
    ecs_entity_t e3 = ecs_new_w_pair(world, EcsChildOf, e2);

    ecs_add(world, e0, Foo);
    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);
    ecs_add(world, e3, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ Foo }, { ecs_id(Position), .src.id = EcsUp }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_uint(e1, it.entities[0]);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(e0, ecs_field_src(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_uint(e2, it.entities[0]);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(e0, ecs_field_src(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_uint(e3, it.entities[0]);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(e0, ecs_field_src(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_tag_w_up_w_3_levels_pcp(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Foo);

    ecs_entity_t e0 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e1 = ecs_insert(world, ecs_value(EcsParent, {e0}));
    ecs_entity_t e2 = ecs_new_w_pair(world, EcsChildOf, e1);
    ecs_entity_t e3 = ecs_insert(world, ecs_value(EcsParent, {e2}));

    ecs_add(world, e0, Foo);
    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);
    ecs_add(world, e3, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ Foo }, { ecs_id(Position), .src.id = EcsUp }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_uint(e1, it.entities[0]);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(e0, ecs_field_src(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_uint(e2, it.entities[0]);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(e0, ecs_field_src(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_uint(e3, it.entities[0]);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(e0, ecs_field_src(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_tag_w_up_w_3_levels_ppc(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Foo);

    ecs_entity_t e0 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e1 = ecs_insert(world, ecs_value(EcsParent, {e0}));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(EcsParent, {e1}));
    ecs_entity_t e3 = ecs_new_w_pair(world, EcsChildOf, e2);

    ecs_add(world, e0, Foo);
    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);
    ecs_add(world, e3, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ Foo }, { ecs_id(Position), .src.id = EcsUp }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_uint(e1, it.entities[0]);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(e0, ecs_field_src(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_uint(e2, it.entities[0]);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(e0, ecs_field_src(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_uint(e3, it.entities[0]);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(e0, ecs_field_src(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_tag_w_up_w_3_levels_ppp(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Foo);

    ecs_entity_t e0 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e1 = ecs_insert(world, ecs_value(EcsParent, {e0}));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(EcsParent, {e1}));
    ecs_entity_t e3 = ecs_insert(world, ecs_value(EcsParent, {e2}));

    ecs_add(world, e0, Foo);
    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);
    ecs_add(world, e3, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ Foo }, { ecs_id(Position), .src.id = EcsUp }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_uint(e1, it.entities[0]);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(e0, ecs_field_src(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_uint(e2, it.entities[0]);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(e0, ecs_field_src(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_uint(e3, it.entities[0]);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(e0, ecs_field_src(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_up_w_tag_w_3_levels_ccc(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Foo);

    ecs_entity_t e0 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e1 = ecs_new_w_pair(world, EcsChildOf, e0);
    ecs_entity_t e2 = ecs_new_w_pair(world, EcsChildOf, e1);
    ecs_entity_t e3 = ecs_new_w_pair(world, EcsChildOf, e2);

    ecs_add(world, e0, Foo);
    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);
    ecs_add(world, e3, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ ecs_id(Position), .src.id = EcsUp }, { Foo }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_uint(e1, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));
    test_uint(e0, ecs_field_src(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_uint(e2, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));
    test_uint(e0, ecs_field_src(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_uint(e3, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));
    test_uint(e0, ecs_field_src(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_up_w_tag_w_3_levels_ccp(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Foo);

    ecs_entity_t e0 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e1 = ecs_new_w_pair(world, EcsChildOf, e0);
    ecs_entity_t e2 = ecs_new_w_pair(world, EcsChildOf, e1);
    ecs_entity_t e3 = ecs_insert(world, ecs_value(EcsParent, {e2}));

    ecs_add(world, e0, Foo);
    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);
    ecs_add(world, e3, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ ecs_id(Position), .src.id = EcsUp }, { Foo }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_uint(e1, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));
    test_uint(e0, ecs_field_src(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_uint(e2, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));
    test_uint(e0, ecs_field_src(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_uint(e3, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));
    test_uint(e0, ecs_field_src(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_up_w_tag_w_3_levels_cpc(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Foo);

    ecs_entity_t e0 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e1 = ecs_new_w_pair(world, EcsChildOf, e0);
    ecs_entity_t e2 = ecs_insert(world, ecs_value(EcsParent, {e1}));
    ecs_entity_t e3 = ecs_new_w_pair(world, EcsChildOf, e2);

    ecs_add(world, e0, Foo);
    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);
    ecs_add(world, e3, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ ecs_id(Position), .src.id = EcsUp }, { Foo }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_uint(e1, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));
    test_uint(e0, ecs_field_src(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_uint(cache_kind == EcsQueryCacheAuto ? e3 : e2, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));
    test_uint(e0, ecs_field_src(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_uint(cache_kind == EcsQueryCacheAuto ? e2 : e3, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));
    test_uint(e0, ecs_field_src(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_up_w_tag_w_3_levels_cpp(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Foo);

    ecs_entity_t e0 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e1 = ecs_new_w_pair(world, EcsChildOf, e0);
    ecs_entity_t e2 = ecs_insert(world, ecs_value(EcsParent, {e1}));
    ecs_entity_t e3 = ecs_insert(world, ecs_value(EcsParent, {e2}));

    ecs_add(world, e0, Foo);
    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);
    ecs_add(world, e3, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ ecs_id(Position), .src.id = EcsUp }, { Foo }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_uint(e1, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));
    test_uint(e0, ecs_field_src(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_uint(e2, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));
    test_uint(e0, ecs_field_src(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_uint(e3, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));
    test_uint(e0, ecs_field_src(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_up_w_tag_w_3_levels_pcc(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Foo);

    ecs_entity_t e0 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e1 = ecs_insert(world, ecs_value(EcsParent, {e0}));
    ecs_entity_t e2 = ecs_new_w_pair(world, EcsChildOf, e1);
    ecs_entity_t e3 = ecs_new_w_pair(world, EcsChildOf, e2);

    ecs_add(world, e0, Foo);
    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);
    ecs_add(world, e3, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ ecs_id(Position), .src.id = EcsUp }, { Foo }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_uint(cache_kind == EcsQueryCacheAuto ? e2 : e1, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));
    test_uint(e0, ecs_field_src(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_uint(cache_kind == EcsQueryCacheAuto ? e3 : e2, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));
    test_uint(e0, ecs_field_src(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_uint(cache_kind == EcsQueryCacheAuto ? e1 : e3, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));
    test_uint(e0, ecs_field_src(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_up_w_tag_w_3_levels_pcp(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Foo);

    ecs_entity_t e0 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e1 = ecs_insert(world, ecs_value(EcsParent, {e0}));
    ecs_entity_t e2 = ecs_new_w_pair(world, EcsChildOf, e1);
    ecs_entity_t e3 = ecs_insert(world, ecs_value(EcsParent, {e2}));

    ecs_add(world, e0, Foo);
    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);
    ecs_add(world, e3, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ ecs_id(Position), .src.id = EcsUp }, { Foo }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_uint(cache_kind == EcsQueryCacheAuto ? e2 : e1, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));
    test_uint(e0, ecs_field_src(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_uint(cache_kind == EcsQueryCacheAuto ? e1 : e2, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));
    test_uint(e0, ecs_field_src(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_uint(e3, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));
    test_uint(e0, ecs_field_src(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_up_w_tag_w_3_levels_ppc(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Foo);

    ecs_entity_t e0 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e1 = ecs_insert(world, ecs_value(EcsParent, {e0}));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(EcsParent, {e1}));
    ecs_entity_t e3 = ecs_new_w_pair(world, EcsChildOf, e2);

    ecs_add(world, e0, Foo);
    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);
    ecs_add(world, e3, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ ecs_id(Position), .src.id = EcsUp }, { Foo }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_uint(cache_kind == EcsQueryCacheAuto ? e3 : e1, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));
    test_uint(e0, ecs_field_src(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_uint(cache_kind == EcsQueryCacheAuto ? e1 : e2, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));
    test_uint(e0, ecs_field_src(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_uint(cache_kind == EcsQueryCacheAuto ? e2 : e3, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));
    test_uint(e0, ecs_field_src(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_up_w_tag_w_3_levels_ppp(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Foo);

    ecs_entity_t e0 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e1 = ecs_insert(world, ecs_value(EcsParent, {e0}));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(EcsParent, {e1}));
    ecs_entity_t e3 = ecs_insert(world, ecs_value(EcsParent, {e2}));

    ecs_add(world, e0, Foo);
    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);
    ecs_add(world, e3, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ ecs_id(Position), .src.id = EcsUp }, { Foo }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_uint(e1, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));
    test_uint(e0, ecs_field_src(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_uint(e2, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));
    test_uint(e0, ecs_field_src(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_uint(e3, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));
    test_uint(e0, ecs_field_src(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_self_up_w_3_levels_ccc(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e0 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e1 = ecs_new_w_pair(world, EcsChildOf, e0);
    ecs_entity_t e2 = ecs_new_w_pair(world, EcsChildOf, e1);
    ecs_entity_t e3 = ecs_new_w_pair(world, EcsChildOf, e2);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ ecs_id(Position), .src.id = EcsSelf | EcsUp }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_uint(e0, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(0, ecs_field_src(&it, 0));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_uint(e1, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(e0, ecs_field_src(&it, 0));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_uint(e2, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(e0, ecs_field_src(&it, 0));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_uint(e3, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(e0, ecs_field_src(&it, 0));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_self_up_w_3_levels_ccp(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e0 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e1 = ecs_new_w_pair(world, EcsChildOf, e0);
    ecs_entity_t e2 = ecs_new_w_pair(world, EcsChildOf, e1);
    ecs_entity_t e3 = ecs_insert(world, ecs_value(EcsParent, {e2}));

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ ecs_id(Position), .src.id = EcsSelf | EcsUp }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_uint(e0, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(0, ecs_field_src(&it, 0));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_uint(e1, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(e0, ecs_field_src(&it, 0));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_uint(e2, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(e0, ecs_field_src(&it, 0));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_uint(e3, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(e0, ecs_field_src(&it, 0));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_self_up_w_3_levels_cpc(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e0 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e1 = ecs_new_w_pair(world, EcsChildOf, e0);
    ecs_entity_t e2 = ecs_insert(world, ecs_value(EcsParent, {e1}));
    ecs_entity_t e3 = ecs_new_w_pair(world, EcsChildOf, e2);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ ecs_id(Position), .src.id = EcsSelf | EcsUp }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_uint(e0, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(0, ecs_field_src(&it, 0));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_uint(e1, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(e0, ecs_field_src(&it, 0));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_uint(cache_kind == EcsQueryCacheAuto ? e3 : e2, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(e0, ecs_field_src(&it, 0));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_uint(cache_kind == EcsQueryCacheAuto ? e2 : e3, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(e0, ecs_field_src(&it, 0));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_self_up_w_3_levels_cpp(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e0 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e1 = ecs_new_w_pair(world, EcsChildOf, e0);
    ecs_entity_t e2 = ecs_insert(world, ecs_value(EcsParent, {e1}));
    ecs_entity_t e3 = ecs_insert(world, ecs_value(EcsParent, {e2}));

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ ecs_id(Position), .src.id = EcsSelf | EcsUp }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_uint(e0, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(0, ecs_field_src(&it, 0));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_uint(e1, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(e0, ecs_field_src(&it, 0));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_uint(e2, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(e0, ecs_field_src(&it, 0));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_uint(e3, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(e0, ecs_field_src(&it, 0));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_self_up_w_3_levels_pcc(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e0 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e1 = ecs_insert(world, ecs_value(EcsParent, {e0}));
    ecs_entity_t e2 = ecs_new_w_pair(world, EcsChildOf, e1);
    ecs_entity_t e3 = ecs_new_w_pair(world, EcsChildOf, e2);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ ecs_id(Position), .src.id = EcsSelf | EcsUp }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_uint(e0, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(0, ecs_field_src(&it, 0));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_uint(cache_kind == EcsQueryCacheAuto ? e2 : e1, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(e0, ecs_field_src(&it, 0));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_uint(cache_kind == EcsQueryCacheAuto ? e3 : e2, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(e0, ecs_field_src(&it, 0));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_uint(cache_kind == EcsQueryCacheAuto ? e1 : e3, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(e0, ecs_field_src(&it, 0));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_self_up_w_3_levels_pcp(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e0 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e1 = ecs_insert(world, ecs_value(EcsParent, {e0}));
    ecs_entity_t e2 = ecs_new_w_pair(world, EcsChildOf, e1);
    ecs_entity_t e3 = ecs_insert(world, ecs_value(EcsParent, {e2}));

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ ecs_id(Position), .src.id = EcsSelf | EcsUp }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_uint(e0, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(0, ecs_field_src(&it, 0));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_uint(cache_kind == EcsQueryCacheAuto ? e2 : e1, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(e0, ecs_field_src(&it, 0));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_uint(cache_kind == EcsQueryCacheAuto ? e1 : e2, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(e0, ecs_field_src(&it, 0));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_uint(e3, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(e0, ecs_field_src(&it, 0));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_self_up_w_3_levels_ppc(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e0 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e1 = ecs_insert(world, ecs_value(EcsParent, {e0}));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(EcsParent, {e1}));
    ecs_entity_t e3 = ecs_new_w_pair(world, EcsChildOf, e2);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ ecs_id(Position), .src.id = EcsSelf | EcsUp }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_uint(e0, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(0, ecs_field_src(&it, 0));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_uint(cache_kind == EcsQueryCacheAuto ? e3 : e1, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(e0, ecs_field_src(&it, 0));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_uint(cache_kind == EcsQueryCacheAuto ? e1 : e2, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(e0, ecs_field_src(&it, 0));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_uint(cache_kind == EcsQueryCacheAuto ? e2 : e3, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(e0, ecs_field_src(&it, 0));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_self_up_w_3_levels_ppp(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e0 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e1 = ecs_insert(world, ecs_value(EcsParent, {e0}));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(EcsParent, {e1}));
    ecs_entity_t e3 = ecs_insert(world, ecs_value(EcsParent, {e2}));

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ ecs_id(Position), .src.id = EcsSelf | EcsUp }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_uint(e0, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(0, ecs_field_src(&it, 0));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_uint(e1, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(e0, ecs_field_src(&it, 0));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_uint(e2, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(e0, ecs_field_src(&it, 0));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_uint(e3, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(e0, ecs_field_src(&it, 0));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_tag_w_self_up_w_3_levels_ccc(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Foo);

    ecs_entity_t e0 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e1 = ecs_new_w_pair(world, EcsChildOf, e0);
    ecs_entity_t e2 = ecs_new_w_pair(world, EcsChildOf, e1);
    ecs_entity_t e3 = ecs_new_w_pair(world, EcsChildOf, e2);

    ecs_add(world, e0, Foo);
    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);
    ecs_add(world, e3, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ Foo }, { ecs_id(Position), .src.id = EcsSelf | EcsUp }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_uint(e0, it.entities[0]);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_uint(e1, it.entities[0]);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(e0, ecs_field_src(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_uint(e2, it.entities[0]);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(e0, ecs_field_src(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_uint(e3, it.entities[0]);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(e0, ecs_field_src(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_tag_w_self_up_w_3_levels_ccp(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Foo);

    ecs_entity_t e0 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e1 = ecs_new_w_pair(world, EcsChildOf, e0);
    ecs_entity_t e2 = ecs_new_w_pair(world, EcsChildOf, e1);
    ecs_entity_t e3 = ecs_insert(world, ecs_value(EcsParent, {e2}));

    ecs_add(world, e0, Foo);
    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);
    ecs_add(world, e3, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ Foo }, { ecs_id(Position), .src.id = EcsSelf | EcsUp }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_uint(e0, it.entities[0]);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_uint(e1, it.entities[0]);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(e0, ecs_field_src(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_uint(e2, it.entities[0]);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(e0, ecs_field_src(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_uint(e3, it.entities[0]);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(e0, ecs_field_src(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_tag_w_self_up_w_3_levels_cpc(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Foo);

    ecs_entity_t e0 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e1 = ecs_new_w_pair(world, EcsChildOf, e0);
    ecs_entity_t e2 = ecs_insert(world, ecs_value(EcsParent, {e1}));
    ecs_entity_t e3 = ecs_new_w_pair(world, EcsChildOf, e2);

    ecs_add(world, e0, Foo);
    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);
    ecs_add(world, e3, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ Foo }, { ecs_id(Position), .src.id = EcsSelf | EcsUp }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_uint(e0, it.entities[0]);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_uint(e1, it.entities[0]);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(e0, ecs_field_src(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_uint(e2, it.entities[0]);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(e0, ecs_field_src(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_uint(e3, it.entities[0]);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(e0, ecs_field_src(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_tag_w_self_up_w_3_levels_cpp(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Foo);

    ecs_entity_t e0 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e1 = ecs_new_w_pair(world, EcsChildOf, e0);
    ecs_entity_t e2 = ecs_insert(world, ecs_value(EcsParent, {e1}));
    ecs_entity_t e3 = ecs_insert(world, ecs_value(EcsParent, {e2}));

    ecs_add(world, e0, Foo);
    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);
    ecs_add(world, e3, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ Foo }, { ecs_id(Position), .src.id = EcsSelf | EcsUp }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_uint(e0, it.entities[0]);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_uint(e1, it.entities[0]);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(e0, ecs_field_src(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_uint(e2, it.entities[0]);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(e0, ecs_field_src(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_uint(e3, it.entities[0]);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(e0, ecs_field_src(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_tag_w_self_up_w_3_levels_pcc(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Foo);

    ecs_entity_t e0 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e1 = ecs_insert(world, ecs_value(EcsParent, {e0}));
    ecs_entity_t e2 = ecs_new_w_pair(world, EcsChildOf, e1);
    ecs_entity_t e3 = ecs_new_w_pair(world, EcsChildOf, e2);

    ecs_add(world, e0, Foo);
    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);
    ecs_add(world, e3, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ Foo }, { ecs_id(Position), .src.id = EcsSelf | EcsUp }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_uint(e0, it.entities[0]);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_uint(e1, it.entities[0]);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(e0, ecs_field_src(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_uint(e2, it.entities[0]);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(e0, ecs_field_src(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_uint(e3, it.entities[0]);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(e0, ecs_field_src(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_tag_w_self_up_w_3_levels_pcp(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Foo);

    ecs_entity_t e0 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e1 = ecs_insert(world, ecs_value(EcsParent, {e0}));
    ecs_entity_t e2 = ecs_new_w_pair(world, EcsChildOf, e1);
    ecs_entity_t e3 = ecs_insert(world, ecs_value(EcsParent, {e2}));

    ecs_add(world, e0, Foo);
    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);
    ecs_add(world, e3, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ Foo }, { ecs_id(Position), .src.id = EcsSelf | EcsUp }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_uint(e0, it.entities[0]);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_uint(e1, it.entities[0]);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(e0, ecs_field_src(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_uint(e2, it.entities[0]);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(e0, ecs_field_src(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_uint(e3, it.entities[0]);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(e0, ecs_field_src(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_tag_w_self_up_w_3_levels_ppc(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Foo);

    ecs_entity_t e0 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e1 = ecs_insert(world, ecs_value(EcsParent, {e0}));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(EcsParent, {e1}));
    ecs_entity_t e3 = ecs_new_w_pair(world, EcsChildOf, e2);

    ecs_add(world, e0, Foo);
    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);
    ecs_add(world, e3, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ Foo }, { ecs_id(Position), .src.id = EcsSelf | EcsUp }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_uint(e0, it.entities[0]);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_uint(e1, it.entities[0]);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(e0, ecs_field_src(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_uint(e2, it.entities[0]);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(e0, ecs_field_src(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_uint(e3, it.entities[0]);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(e0, ecs_field_src(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_tag_w_self_up_w_3_levels_ppp(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Foo);

    ecs_entity_t e0 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e1 = ecs_insert(world, ecs_value(EcsParent, {e0}));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(EcsParent, {e1}));
    ecs_entity_t e3 = ecs_insert(world, ecs_value(EcsParent, {e2}));

    ecs_add(world, e0, Foo);
    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);
    ecs_add(world, e3, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ Foo }, { ecs_id(Position), .src.id = EcsSelf | EcsUp }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_uint(e0, it.entities[0]);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_uint(e1, it.entities[0]);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(e0, ecs_field_src(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_uint(e2, it.entities[0]);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(e0, ecs_field_src(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_uint(e3, it.entities[0]);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(e0, ecs_field_src(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_self_up_w_tag_w_3_levels_ccc(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Foo);

    ecs_entity_t e0 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e1 = ecs_new_w_pair(world, EcsChildOf, e0);
    ecs_entity_t e2 = ecs_new_w_pair(world, EcsChildOf, e1);
    ecs_entity_t e3 = ecs_new_w_pair(world, EcsChildOf, e2);

    ecs_add(world, e0, Foo);
    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);
    ecs_add(world, e3, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ ecs_id(Position), .src.id = EcsSelf | EcsUp }, { Foo }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_uint(e0, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_uint(e1, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));
    test_uint(e0, ecs_field_src(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_uint(e2, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));
    test_uint(e0, ecs_field_src(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_uint(e3, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));
    test_uint(e0, ecs_field_src(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_self_up_w_tag_w_3_levels_ccp(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Foo);

    ecs_entity_t e0 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e1 = ecs_new_w_pair(world, EcsChildOf, e0);
    ecs_entity_t e2 = ecs_new_w_pair(world, EcsChildOf, e1);
    ecs_entity_t e3 = ecs_insert(world, ecs_value(EcsParent, {e2}));

    ecs_add(world, e0, Foo);
    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);
    ecs_add(world, e3, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ ecs_id(Position), .src.id = EcsSelf | EcsUp }, { Foo }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_uint(e0, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_uint(e1, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));
    test_uint(e0, ecs_field_src(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_uint(e2, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));
    test_uint(e0, ecs_field_src(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_uint(e3, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));
    test_uint(e0, ecs_field_src(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_self_up_w_tag_w_3_levels_cpc(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Foo);

    ecs_entity_t e0 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e1 = ecs_new_w_pair(world, EcsChildOf, e0);
    ecs_entity_t e2 = ecs_insert(world, ecs_value(EcsParent, {e1}));
    ecs_entity_t e3 = ecs_new_w_pair(world, EcsChildOf, e2);

    ecs_add(world, e0, Foo);
    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);
    ecs_add(world, e3, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ ecs_id(Position), .src.id = EcsSelf | EcsUp }, { Foo }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_uint(e0, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_uint(e1, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));
    test_uint(e0, ecs_field_src(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_uint(cache_kind == EcsQueryCacheAuto ? e3 : e2, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));
    test_uint(e0, ecs_field_src(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_uint(cache_kind == EcsQueryCacheAuto ? e2 : e3, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));
    test_uint(e0, ecs_field_src(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_self_up_w_tag_w_3_levels_cpp(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Foo);

    ecs_entity_t e0 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e1 = ecs_new_w_pair(world, EcsChildOf, e0);
    ecs_entity_t e2 = ecs_insert(world, ecs_value(EcsParent, {e1}));
    ecs_entity_t e3 = ecs_insert(world, ecs_value(EcsParent, {e2}));

    ecs_add(world, e0, Foo);
    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);
    ecs_add(world, e3, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ ecs_id(Position), .src.id = EcsSelf | EcsUp }, { Foo }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_uint(e0, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_uint(e1, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));
    test_uint(e0, ecs_field_src(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_uint(e2, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));
    test_uint(e0, ecs_field_src(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_uint(e3, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));
    test_uint(e0, ecs_field_src(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_self_up_w_tag_w_3_levels_pcc(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Foo);

    ecs_entity_t e0 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e1 = ecs_insert(world, ecs_value(EcsParent, {e0}));
    ecs_entity_t e2 = ecs_new_w_pair(world, EcsChildOf, e1);
    ecs_entity_t e3 = ecs_new_w_pair(world, EcsChildOf, e2);

    ecs_add(world, e0, Foo);
    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);
    ecs_add(world, e3, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ ecs_id(Position), .src.id = EcsSelf | EcsUp }, { Foo }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_uint(e0, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_uint(cache_kind == EcsQueryCacheAuto ? e2 : e1, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));
    test_uint(e0, ecs_field_src(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_uint(cache_kind == EcsQueryCacheAuto ? e3 : e2, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));
    test_uint(e0, ecs_field_src(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_uint(cache_kind == EcsQueryCacheAuto ? e1 : e3, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));
    test_uint(e0, ecs_field_src(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_self_up_w_tag_w_3_levels_pcp(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Foo);

    ecs_entity_t e0 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e1 = ecs_insert(world, ecs_value(EcsParent, {e0}));
    ecs_entity_t e2 = ecs_new_w_pair(world, EcsChildOf, e1);
    ecs_entity_t e3 = ecs_insert(world, ecs_value(EcsParent, {e2}));

    ecs_add(world, e0, Foo);
    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);
    ecs_add(world, e3, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ ecs_id(Position), .src.id = EcsSelf | EcsUp }, { Foo }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_uint(e0, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_uint(cache_kind == EcsQueryCacheAuto ? e2 : e1, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));
    test_uint(e0, ecs_field_src(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_uint(cache_kind == EcsQueryCacheAuto ? e1 : e2, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));
    test_uint(e0, ecs_field_src(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_uint(e3, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));
    test_uint(e0, ecs_field_src(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_self_up_w_tag_w_3_levels_ppc(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Foo);

    ecs_entity_t e0 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e1 = ecs_insert(world, ecs_value(EcsParent, {e0}));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(EcsParent, {e1}));
    ecs_entity_t e3 = ecs_new_w_pair(world, EcsChildOf, e2);

    ecs_add(world, e0, Foo);
    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);
    ecs_add(world, e3, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ ecs_id(Position), .src.id = EcsSelf | EcsUp }, { Foo }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_uint(e0, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_uint(cache_kind == EcsQueryCacheAuto ? e3 : e1, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));
    test_uint(e0, ecs_field_src(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_uint(cache_kind == EcsQueryCacheAuto ? e1 : e2, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));
    test_uint(e0, ecs_field_src(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_uint(cache_kind == EcsQueryCacheAuto ? e2 : e3, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));
    test_uint(e0, ecs_field_src(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_self_up_w_tag_w_3_levels_ppp(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Foo);

    ecs_entity_t e0 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e1 = ecs_insert(world, ecs_value(EcsParent, {e0}));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(EcsParent, {e1}));
    ecs_entity_t e3 = ecs_insert(world, ecs_value(EcsParent, {e2}));

    ecs_add(world, e0, Foo);
    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);
    ecs_add(world, e3, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ ecs_id(Position), .src.id = EcsSelf | EcsUp }, { Foo }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_uint(e0, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_uint(e1, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));
    test_uint(e0, ecs_field_src(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_uint(e2, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));
    test_uint(e0, ecs_field_src(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_uint(e3, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));
    test_uint(e0, ecs_field_src(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_set_childof(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t parent_a = ecs_new(world);
    ecs_entity_t parent_b = ecs_new(world);
    ecs_entity_t child_1 = ecs_new_w_pair(world, EcsChildOf, parent_a);
    ecs_entity_t child_2 = ecs_insert(world, ecs_value(EcsParent, {parent_a}));
    ecs_entity_t child_3 = ecs_new_w_pair(world, EcsChildOf, parent_b);
    ecs_entity_t child_4 = ecs_insert(world, ecs_value(EcsParent, {parent_b}));

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ ecs_childof(parent_a) }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, parent_a);
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, parent_b);
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, child_1);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(child_1, it.entities[0]);
        test_uint(ecs_pair(EcsChildOf, parent_a), ecs_field_id(&it, 0));
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, child_2);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(child_2, it.entities[0]);
        test_uint(ecs_pair(EcsChildOf, parent_a), ecs_field_id(&it, 0));
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, child_3);
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, child_4);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_set_childof_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t parent_a = ecs_new(world);
    ecs_entity_t parent_b = ecs_new(world);
    ecs_entity_t child_1 = ecs_new_w_pair(world, EcsChildOf, parent_a);
    ecs_entity_t child_2 = ecs_insert(world, ecs_value(EcsParent, {parent_a}));
    ecs_entity_t child_3 = ecs_new_w_pair(world, EcsChildOf, parent_b);
    ecs_entity_t child_4 = ecs_insert(world, ecs_value(EcsParent, {parent_b}));

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ ecs_childof(EcsWildcard) }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, parent_a);
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, parent_b);
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, child_1);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(child_1, it.entities[0]);
        test_uint(ecs_pair(EcsChildOf, parent_a), ecs_field_id(&it, 0));
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, child_2);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(child_2, it.entities[0]);
        test_uint(ecs_pair(EcsChildOf, parent_a), ecs_field_id(&it, 0));
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, child_3);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(child_3, it.entities[0]);
        test_uint(ecs_pair(EcsChildOf, parent_b), ecs_field_id(&it, 0));
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, child_4);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(child_4, it.entities[0]);
        test_uint(ecs_pair(EcsChildOf, parent_b), ecs_field_id(&it, 0));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_set_childof_any(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t parent_a = ecs_new(world);
    ecs_entity_t parent_b = ecs_new(world);
    ecs_entity_t child_1 = ecs_new_w_pair(world, EcsChildOf, parent_a);
    ecs_entity_t child_2 = ecs_insert(world, ecs_value(EcsParent, {parent_a}));
    ecs_entity_t child_3 = ecs_new_w_pair(world, EcsChildOf, parent_b);
    ecs_entity_t child_4 = ecs_insert(world, ecs_value(EcsParent, {parent_b}));

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ ecs_childof(EcsAny) }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, parent_a);
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, parent_b);
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, child_1);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(child_1, it.entities[0]);
        test_uint(ecs_pair(EcsChildOf, EcsWildcard), ecs_field_id(&it, 0));
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, child_2);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(child_2, it.entities[0]);
        test_uint(ecs_pair(EcsChildOf, EcsWildcard), ecs_field_id(&it, 0));
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, child_3);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(child_3, it.entities[0]);
        test_uint(ecs_pair(EcsChildOf, EcsWildcard), ecs_field_id(&it, 0));
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, child_4);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(child_4, it.entities[0]);
        test_uint(ecs_pair(EcsChildOf, EcsWildcard), ecs_field_id(&it, 0));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_set_childof_w_tag(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_entity_t parent_a = ecs_new(world);
    ecs_entity_t parent_b = ecs_new(world);
    ecs_entity_t child_1 = ecs_new_w_pair(world, EcsChildOf, parent_a);
    ecs_entity_t child_2 = ecs_insert(world, ecs_value(EcsParent, {parent_a}));
    ecs_entity_t child_3 = ecs_new_w_pair(world, EcsChildOf, parent_b);
    ecs_entity_t child_4 = ecs_insert(world, ecs_value(EcsParent, {parent_b}));

    ecs_add(world, child_1, Foo);
    ecs_add(world, child_2, Foo);
    ecs_add(world, child_3, Foo);
    ecs_add(world, child_4, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ ecs_childof(parent_a) }, { Foo }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, parent_a);
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, parent_b);
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, child_1);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(child_1, it.entities[0]);
        test_uint(ecs_pair(EcsChildOf, parent_a), ecs_field_id(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 1));
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, child_2);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(child_2, it.entities[0]);
        test_uint(ecs_pair(EcsChildOf, parent_a), ecs_field_id(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 1));
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, child_3);
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, child_4);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_set_childof_wildcard_w_tag(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_entity_t parent_a = ecs_new(world);
    ecs_entity_t parent_b = ecs_new(world);
    ecs_entity_t child_1 = ecs_new_w_pair(world, EcsChildOf, parent_a);
    ecs_entity_t child_2 = ecs_insert(world, ecs_value(EcsParent, {parent_a}));
    ecs_entity_t child_3 = ecs_new_w_pair(world, EcsChildOf, parent_b);
    ecs_entity_t child_4 = ecs_insert(world, ecs_value(EcsParent, {parent_b}));

    ecs_add(world, child_1, Foo);
    ecs_add(world, child_2, Foo);
    ecs_add(world, child_3, Foo);
    ecs_add(world, child_4, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ ecs_childof(EcsWildcard) }, { Foo }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, parent_a);
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, parent_b);
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, child_1);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(child_1, it.entities[0]);
        test_uint(ecs_pair(EcsChildOf, parent_a), ecs_field_id(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 1));
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, child_2);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(child_2, it.entities[0]);
        test_uint(ecs_pair(EcsChildOf, parent_a), ecs_field_id(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 1));
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, child_3);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(child_3, it.entities[0]);
        test_uint(ecs_pair(EcsChildOf, parent_b), ecs_field_id(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 1));
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, child_4);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(child_4, it.entities[0]);
        test_uint(ecs_pair(EcsChildOf, parent_b), ecs_field_id(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 1));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_set_childof_any_w_tag(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_entity_t parent_a = ecs_new(world);
    ecs_entity_t parent_b = ecs_new(world);
    ecs_entity_t child_1 = ecs_new_w_pair(world, EcsChildOf, parent_a);
    ecs_entity_t child_2 = ecs_insert(world, ecs_value(EcsParent, {parent_a}));
    ecs_entity_t child_3 = ecs_new_w_pair(world, EcsChildOf, parent_b);
    ecs_entity_t child_4 = ecs_insert(world, ecs_value(EcsParent, {parent_b}));

    ecs_add(world, child_1, Foo);
    ecs_add(world, child_2, Foo);
    ecs_add(world, child_3, Foo);
    ecs_add(world, child_4, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ ecs_childof(EcsAny) }, { Foo }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, parent_a);
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, parent_b);
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, child_1);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(child_1, it.entities[0]);
        test_uint(ecs_pair(EcsChildOf, EcsWildcard), ecs_field_id(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 1));
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, child_2);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(child_2, it.entities[0]);
        test_uint(ecs_pair(EcsChildOf, EcsWildcard), ecs_field_id(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 1));
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, child_3);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(child_3, it.entities[0]);
        test_uint(ecs_pair(EcsChildOf, EcsWildcard), ecs_field_id(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 1));
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, child_4);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(child_4, it.entities[0]);
        test_uint(ecs_pair(EcsChildOf, EcsWildcard), ecs_field_id(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 1));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_set_tag_w_childof(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_entity_t parent_a = ecs_new(world);
    ecs_entity_t parent_b = ecs_new(world);
    ecs_entity_t child_1 = ecs_new_w_pair(world, EcsChildOf, parent_a);
    ecs_entity_t child_2 = ecs_insert(world, ecs_value(EcsParent, {parent_a}));
    ecs_entity_t child_3 = ecs_new_w_pair(world, EcsChildOf, parent_b);
    ecs_entity_t child_4 = ecs_insert(world, ecs_value(EcsParent, {parent_b}));

    ecs_add(world, child_1, Foo);
    ecs_add(world, child_2, Foo);
    ecs_add(world, child_3, Foo);
    ecs_add(world, child_4, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ Foo }, { ecs_childof(parent_a) }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, parent_a);
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, parent_b);
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, child_1);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(child_1, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_pair(EcsChildOf, parent_a), ecs_field_id(&it, 1));
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, child_2);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(child_2, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_pair(EcsChildOf, parent_a), ecs_field_id(&it, 1));
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, child_3);
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, child_4);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_set_tag_w_childof_wildcard(void) {
    ecs_world_t *world = ecs_mini();
    
    ECS_TAG(world, Foo);

    ecs_entity_t parent_a = ecs_new(world);
    ecs_entity_t parent_b = ecs_new(world);
    ecs_entity_t child_1 = ecs_new_w_pair(world, EcsChildOf, parent_a);
    ecs_entity_t child_2 = ecs_insert(world, ecs_value(EcsParent, {parent_a}));
    ecs_entity_t child_3 = ecs_new_w_pair(world, EcsChildOf, parent_b);
    ecs_entity_t child_4 = ecs_insert(world, ecs_value(EcsParent, {parent_b}));

    ecs_add(world, child_1, Foo);
    ecs_add(world, child_2, Foo);
    ecs_add(world, child_3, Foo);
    ecs_add(world, child_4, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ Foo }, { ecs_childof(EcsWildcard) }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, parent_a);
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, parent_b);
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, child_1);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(child_1, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_pair(EcsChildOf, parent_a), ecs_field_id(&it, 1));
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, child_2);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(child_2, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_pair(EcsChildOf, parent_a), ecs_field_id(&it, 1));
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, child_3);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(child_3, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_pair(EcsChildOf, parent_b), ecs_field_id(&it, 1));
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, child_4);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(child_4, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_pair(EcsChildOf, parent_b), ecs_field_id(&it, 1));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_set_tag_w_childof_any(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_entity_t parent_a = ecs_new(world);
    ecs_entity_t parent_b = ecs_new(world);
    ecs_entity_t child_1 = ecs_new_w_pair(world, EcsChildOf, parent_a);
    ecs_entity_t child_2 = ecs_insert(world, ecs_value(EcsParent, {parent_a}));
    ecs_entity_t child_3 = ecs_new_w_pair(world, EcsChildOf, parent_b);
    ecs_entity_t child_4 = ecs_insert(world, ecs_value(EcsParent, {parent_b}));

    ecs_add(world, child_1, Foo);
    ecs_add(world, child_2, Foo);
    ecs_add(world, child_3, Foo);
    ecs_add(world, child_4, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ Foo }, { ecs_childof(EcsAny) }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, parent_a);
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, parent_b);
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, child_1);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(child_1, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_pair(EcsChildOf, EcsWildcard), ecs_field_id(&it, 1));
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, child_2);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(child_2, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_pair(EcsChildOf, EcsWildcard), ecs_field_id(&it, 1));
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, child_3);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(child_3, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_pair(EcsChildOf, EcsWildcard), ecs_field_id(&it, 1));
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, child_4);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(child_4, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_pair(EcsChildOf, EcsWildcard), ecs_field_id(&it, 1));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_set_up_w_3_levels_ccc(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e0 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e1 = ecs_new_w_pair(world, EcsChildOf, e0);
    ecs_entity_t e2 = ecs_new_w_pair(world, EcsChildOf, e1);
    ecs_entity_t e3 = ecs_new_w_pair(world, EcsChildOf, e2);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ ecs_id(Position), .src.id = EcsUp }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    test_uint(e0, ecs_get_target_for(world, e1, EcsChildOf, Position));
    test_uint(e0, ecs_get_target_for(world, e2, EcsChildOf, Position));
    test_uint(e0, ecs_get_target_for(world, e3, EcsChildOf, Position));

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e0);
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e1);
        test_bool(true, ecs_query_next(&it));
        test_uint(e1, it.entities[0]);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(e0, ecs_field_src(&it, 0));
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e2);
        test_bool(true, ecs_query_next(&it));
        test_uint(e2, it.entities[0]);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(e0, ecs_field_src(&it, 0));
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e3);
        test_bool(true, ecs_query_next(&it));
        test_uint(e3, it.entities[0]);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(e0, ecs_field_src(&it, 0));
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_set_up_w_3_levels_ccp(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e0 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e1 = ecs_new_w_pair(world, EcsChildOf, e0);
    ecs_entity_t e2 = ecs_new_w_pair(world, EcsChildOf, e1);
    ecs_entity_t e3 = ecs_insert(world, ecs_value(EcsParent, {e2}));

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ ecs_id(Position), .src.id = EcsUp }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    test_uint(e0, ecs_get_target_for(world, e1, EcsChildOf, Position));
    test_uint(e0, ecs_get_target_for(world, e2, EcsChildOf, Position));
    test_uint(e0, ecs_get_target_for(world, e3, EcsChildOf, Position));

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e0);
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e1);
        test_bool(true, ecs_query_next(&it));
        test_uint(e1, it.entities[0]);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(e0, ecs_field_src(&it, 0));
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e2);
        test_bool(true, ecs_query_next(&it));
        test_uint(e2, it.entities[0]);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(e0, ecs_field_src(&it, 0));
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e3);
        test_bool(true, ecs_query_next(&it));
        test_uint(e3, it.entities[0]);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(e0, ecs_field_src(&it, 0));
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_set_up_w_3_levels_cpc(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e0 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e1 = ecs_new_w_pair(world, EcsChildOf, e0);
    ecs_entity_t e2 = ecs_insert(world, ecs_value(EcsParent, {e1}));
    ecs_entity_t e3 = ecs_new_w_pair(world, EcsChildOf, e2);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ ecs_id(Position), .src.id = EcsUp }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    test_uint(e0, ecs_get_target_for(world, e1, EcsChildOf, Position));
    test_uint(e0, ecs_get_target_for(world, e2, EcsChildOf, Position));
    test_uint(e0, ecs_get_target_for(world, e3, EcsChildOf, Position));

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e0);
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e1);
        test_bool(true, ecs_query_next(&it));
        test_uint(e1, it.entities[0]);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(e0, ecs_field_src(&it, 0));
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e2);
        test_bool(true, ecs_query_next(&it));
        test_uint(e2, it.entities[0]);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(e0, ecs_field_src(&it, 0));
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e3);
        test_bool(true, ecs_query_next(&it));
        test_uint(e3, it.entities[0]);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(e0, ecs_field_src(&it, 0));
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_set_up_w_3_levels_cpp(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e0 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e1 = ecs_new_w_pair(world, EcsChildOf, e0);
    ecs_entity_t e2 = ecs_insert(world, ecs_value(EcsParent, {e1}));
    ecs_entity_t e3 = ecs_insert(world, ecs_value(EcsParent, {e2}));

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ ecs_id(Position), .src.id = EcsUp }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    test_uint(e0, ecs_get_target_for(world, e1, EcsChildOf, Position));
    test_uint(e0, ecs_get_target_for(world, e2, EcsChildOf, Position));
    test_uint(e0, ecs_get_target_for(world, e3, EcsChildOf, Position));

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e0);
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e1);
        test_bool(true, ecs_query_next(&it));
        test_uint(e1, it.entities[0]);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(e0, ecs_field_src(&it, 0));
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e2);
        test_bool(true, ecs_query_next(&it));
        test_uint(e2, it.entities[0]);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(e0, ecs_field_src(&it, 0));
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e3);
        test_bool(true, ecs_query_next(&it));
        test_uint(e3, it.entities[0]);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(e0, ecs_field_src(&it, 0));
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_set_up_w_3_levels_pcc(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e0 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e1 = ecs_insert(world, ecs_value(EcsParent, {e0}));
    ecs_entity_t e2 = ecs_new_w_pair(world, EcsChildOf, e1);
    ecs_entity_t e3 = ecs_new_w_pair(world, EcsChildOf, e2);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ ecs_id(Position), .src.id = EcsUp }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    test_uint(e0, ecs_get_target_for(world, e1, EcsChildOf, Position));
    test_uint(e0, ecs_get_target_for(world, e2, EcsChildOf, Position));
    test_uint(e0, ecs_get_target_for(world, e3, EcsChildOf, Position));

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e0);
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e1);
        test_bool(true, ecs_query_next(&it));
        test_uint(e1, it.entities[0]);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(e0, ecs_field_src(&it, 0));
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e2);
        test_bool(true, ecs_query_next(&it));
        test_uint(e2, it.entities[0]);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(e0, ecs_field_src(&it, 0));
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e3);
        test_bool(true, ecs_query_next(&it));
        test_uint(e3, it.entities[0]);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(e0, ecs_field_src(&it, 0));
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_set_up_w_3_levels_pcp(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e0 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e1 = ecs_insert(world, ecs_value(EcsParent, {e0}));
    ecs_entity_t e2 = ecs_new_w_pair(world, EcsChildOf, e1);
    ecs_entity_t e3 = ecs_insert(world, ecs_value(EcsParent, {e2}));

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ ecs_id(Position), .src.id = EcsUp }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    test_uint(e0, ecs_get_target_for(world, e1, EcsChildOf, Position));
    test_uint(e0, ecs_get_target_for(world, e2, EcsChildOf, Position));
    test_uint(e0, ecs_get_target_for(world, e3, EcsChildOf, Position));

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e0);
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e1);
        test_bool(true, ecs_query_next(&it));
        test_uint(e1, it.entities[0]);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(e0, ecs_field_src(&it, 0));
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e2);
        test_bool(true, ecs_query_next(&it));
        test_uint(e2, it.entities[0]);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(e0, ecs_field_src(&it, 0));
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e3);
        test_bool(true, ecs_query_next(&it));
        test_uint(e3, it.entities[0]);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(e0, ecs_field_src(&it, 0));
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_set_up_w_3_levels_ppc(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e0 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e1 = ecs_insert(world, ecs_value(EcsParent, {e0}));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(EcsParent, {e1}));
    ecs_entity_t e3 = ecs_new_w_pair(world, EcsChildOf, e2);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ ecs_id(Position), .src.id = EcsUp }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    test_uint(e0, ecs_get_target_for(world, e1, EcsChildOf, Position));
    test_uint(e0, ecs_get_target_for(world, e2, EcsChildOf, Position));
    test_uint(e0, ecs_get_target_for(world, e3, EcsChildOf, Position));

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e0);
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e1);
        test_bool(true, ecs_query_next(&it));
        test_uint(e1, it.entities[0]);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(e0, ecs_field_src(&it, 0));
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e2);
        test_bool(true, ecs_query_next(&it));
        test_uint(e2, it.entities[0]);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(e0, ecs_field_src(&it, 0));
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e3);
        test_bool(true, ecs_query_next(&it));
        test_uint(e3, it.entities[0]);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(e0, ecs_field_src(&it, 0));
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_set_up_w_3_levels_ppp(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e0 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e1 = ecs_insert(world, ecs_value(EcsParent, {e0}));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(EcsParent, {e1}));
    ecs_entity_t e3 = ecs_insert(world, ecs_value(EcsParent, {e2}));

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ ecs_id(Position), .src.id = EcsUp }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    test_uint(e0, ecs_get_target_for(world, e1, EcsChildOf, Position));
    test_uint(e0, ecs_get_target_for(world, e2, EcsChildOf, Position));
    test_uint(e0, ecs_get_target_for(world, e3, EcsChildOf, Position));

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e0);
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e1);
        test_bool(true, ecs_query_next(&it));
        test_uint(e1, it.entities[0]);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(e0, ecs_field_src(&it, 0));
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e2);
        test_bool(true, ecs_query_next(&it));
        test_uint(e2, it.entities[0]);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(e0, ecs_field_src(&it, 0));
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e3);
        test_bool(true, ecs_query_next(&it));
        test_uint(e3, it.entities[0]);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(e0, ecs_field_src(&it, 0));
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_set_tag_w_up_w_3_levels_ccc(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Foo);

    ecs_entity_t e0 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e1 = ecs_new_w_pair(world, EcsChildOf, e0);
    ecs_entity_t e2 = ecs_new_w_pair(world, EcsChildOf, e1);
    ecs_entity_t e3 = ecs_new_w_pair(world, EcsChildOf, e2);

    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);
    ecs_add(world, e3, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ Foo }, { ecs_id(Position), .src.id = EcsUp }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    test_uint(e0, ecs_get_target_for(world, e1, EcsChildOf, Position));
    test_uint(e0, ecs_get_target_for(world, e2, EcsChildOf, Position));
    test_uint(e0, ecs_get_target_for(world, e3, EcsChildOf, Position));

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e0);
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e1);
        test_bool(true, ecs_query_next(&it));
        test_uint(e1, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(e0, ecs_field_src(&it, 1));
        {
            Position *p = ecs_field(&it, Position, 1);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e2);
        test_bool(true, ecs_query_next(&it));
        test_uint(e2, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(e0, ecs_field_src(&it, 1));
        {
            Position *p = ecs_field(&it, Position, 1);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e3);
        test_bool(true, ecs_query_next(&it));
        test_uint(e3, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(e0, ecs_field_src(&it, 1));
        {
            Position *p = ecs_field(&it, Position, 1);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_set_tag_w_up_w_3_levels_ccp(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Foo);

    ecs_entity_t e0 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e1 = ecs_new_w_pair(world, EcsChildOf, e0);
    ecs_entity_t e2 = ecs_new_w_pair(world, EcsChildOf, e1);
    ecs_entity_t e3 = ecs_insert(world, ecs_value(EcsParent, {e2}));

    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);
    ecs_add(world, e3, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ Foo }, { ecs_id(Position), .src.id = EcsUp }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    test_uint(e0, ecs_get_target_for(world, e1, EcsChildOf, Position));
    test_uint(e0, ecs_get_target_for(world, e2, EcsChildOf, Position));
    test_uint(e0, ecs_get_target_for(world, e3, EcsChildOf, Position));

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e0);
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e1);
        test_bool(true, ecs_query_next(&it));
        test_uint(e1, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(e0, ecs_field_src(&it, 1));
        {
            Position *p = ecs_field(&it, Position, 1);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e2);
        test_bool(true, ecs_query_next(&it));
        test_uint(e2, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(e0, ecs_field_src(&it, 1));
        {
            Position *p = ecs_field(&it, Position, 1);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e3);
        test_bool(true, ecs_query_next(&it));
        test_uint(e3, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(e0, ecs_field_src(&it, 1));
        {
            Position *p = ecs_field(&it, Position, 1);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_set_tag_w_up_w_3_levels_cpc(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Foo);

    ecs_entity_t e0 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e1 = ecs_new_w_pair(world, EcsChildOf, e0);
    ecs_entity_t e2 = ecs_insert(world, ecs_value(EcsParent, {e1}));
    ecs_entity_t e3 = ecs_new_w_pair(world, EcsChildOf, e2);

    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);
    ecs_add(world, e3, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ Foo }, { ecs_id(Position), .src.id = EcsUp }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    test_uint(e0, ecs_get_target_for(world, e1, EcsChildOf, Position));
    test_uint(e0, ecs_get_target_for(world, e2, EcsChildOf, Position));
    test_uint(e0, ecs_get_target_for(world, e3, EcsChildOf, Position));

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e0);
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e1);
        test_bool(true, ecs_query_next(&it));
        test_uint(e1, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(e0, ecs_field_src(&it, 1));
        {
            Position *p = ecs_field(&it, Position, 1);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e2);
        test_bool(true, ecs_query_next(&it));
        test_uint(e2, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(e0, ecs_field_src(&it, 1));
        {
            Position *p = ecs_field(&it, Position, 1);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e3);
        test_bool(true, ecs_query_next(&it));
        test_uint(e3, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(e0, ecs_field_src(&it, 1));
        {
            Position *p = ecs_field(&it, Position, 1);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_set_tag_w_up_w_3_levels_cpp(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Foo);

    ecs_entity_t e0 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e1 = ecs_new_w_pair(world, EcsChildOf, e0);
    ecs_entity_t e2 = ecs_insert(world, ecs_value(EcsParent, {e1}));
    ecs_entity_t e3 = ecs_insert(world, ecs_value(EcsParent, {e2}));

    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);
    ecs_add(world, e3, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ Foo }, { ecs_id(Position), .src.id = EcsUp }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    test_uint(e0, ecs_get_target_for(world, e1, EcsChildOf, Position));
    test_uint(e0, ecs_get_target_for(world, e2, EcsChildOf, Position));
    test_uint(e0, ecs_get_target_for(world, e3, EcsChildOf, Position));

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e0);
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e1);
        test_bool(true, ecs_query_next(&it));
        test_uint(e1, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(e0, ecs_field_src(&it, 1));
        {
            Position *p = ecs_field(&it, Position, 1);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e2);
        test_bool(true, ecs_query_next(&it));
        test_uint(e2, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(e0, ecs_field_src(&it, 1));
        {
            Position *p = ecs_field(&it, Position, 1);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e3);
        test_bool(true, ecs_query_next(&it));
        test_uint(e3, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(e0, ecs_field_src(&it, 1));
        {
            Position *p = ecs_field(&it, Position, 1);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_set_tag_w_up_w_3_levels_pcc(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Foo);

    ecs_entity_t e0 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e1 = ecs_insert(world, ecs_value(EcsParent, {e0}));
    ecs_entity_t e2 = ecs_new_w_pair(world, EcsChildOf, e1);
    ecs_entity_t e3 = ecs_new_w_pair(world, EcsChildOf, e2);

    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);
    ecs_add(world, e3, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ Foo }, { ecs_id(Position), .src.id = EcsUp }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    test_uint(e0, ecs_get_target_for(world, e1, EcsChildOf, Position));
    test_uint(e0, ecs_get_target_for(world, e2, EcsChildOf, Position));
    test_uint(e0, ecs_get_target_for(world, e3, EcsChildOf, Position));

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e0);
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e1);
        test_bool(true, ecs_query_next(&it));
        test_uint(e1, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(e0, ecs_field_src(&it, 1));
        {
            Position *p = ecs_field(&it, Position, 1);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e2);
        test_bool(true, ecs_query_next(&it));
        test_uint(e2, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(e0, ecs_field_src(&it, 1));
        {
            Position *p = ecs_field(&it, Position, 1);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e3);
        test_bool(true, ecs_query_next(&it));
        test_uint(e3, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(e0, ecs_field_src(&it, 1));
        {
            Position *p = ecs_field(&it, Position, 1);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_set_tag_w_up_w_3_levels_pcp(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Foo);

    ecs_entity_t e0 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e1 = ecs_insert(world, ecs_value(EcsParent, {e0}));
    ecs_entity_t e2 = ecs_new_w_pair(world, EcsChildOf, e1);
    ecs_entity_t e3 = ecs_insert(world, ecs_value(EcsParent, {e2}));

    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);
    ecs_add(world, e3, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ Foo }, { ecs_id(Position), .src.id = EcsUp }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    test_uint(e0, ecs_get_target_for(world, e1, EcsChildOf, Position));
    test_uint(e0, ecs_get_target_for(world, e2, EcsChildOf, Position));
    test_uint(e0, ecs_get_target_for(world, e3, EcsChildOf, Position));

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e0);
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e1);
        test_bool(true, ecs_query_next(&it));
        test_uint(e1, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(e0, ecs_field_src(&it, 1));
        {
            Position *p = ecs_field(&it, Position, 1);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e2);
        test_bool(true, ecs_query_next(&it));
        test_uint(e2, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(e0, ecs_field_src(&it, 1));
        {
            Position *p = ecs_field(&it, Position, 1);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e3);
        test_bool(true, ecs_query_next(&it));
        test_uint(e3, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(e0, ecs_field_src(&it, 1));
        {
            Position *p = ecs_field(&it, Position, 1);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_set_tag_w_up_w_3_levels_ppc(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Foo);

    ecs_entity_t e0 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e1 = ecs_insert(world, ecs_value(EcsParent, {e0}));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(EcsParent, {e1}));
    ecs_entity_t e3 = ecs_new_w_pair(world, EcsChildOf, e2);

    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);
    ecs_add(world, e3, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ Foo }, { ecs_id(Position), .src.id = EcsUp }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    test_uint(e0, ecs_get_target_for(world, e1, EcsChildOf, Position));
    test_uint(e0, ecs_get_target_for(world, e2, EcsChildOf, Position));
    test_uint(e0, ecs_get_target_for(world, e3, EcsChildOf, Position));

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e0);
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e1);
        test_bool(true, ecs_query_next(&it));
        test_uint(e1, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(e0, ecs_field_src(&it, 1));
        {
            Position *p = ecs_field(&it, Position, 1);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e2);
        test_bool(true, ecs_query_next(&it));
        test_uint(e2, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(e0, ecs_field_src(&it, 1));
        {
            Position *p = ecs_field(&it, Position, 1);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e3);
        test_bool(true, ecs_query_next(&it));
        test_uint(e3, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(e0, ecs_field_src(&it, 1));
        {
            Position *p = ecs_field(&it, Position, 1);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_set_tag_w_up_w_3_levels_ppp(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Foo);

    ecs_entity_t e0 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e1 = ecs_insert(world, ecs_value(EcsParent, {e0}));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(EcsParent, {e1}));
    ecs_entity_t e3 = ecs_insert(world, ecs_value(EcsParent, {e2}));

    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);
    ecs_add(world, e3, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ Foo }, { ecs_id(Position), .src.id = EcsUp }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    test_uint(e0, ecs_get_target_for(world, e1, EcsChildOf, Position));
    test_uint(e0, ecs_get_target_for(world, e2, EcsChildOf, Position));
    test_uint(e0, ecs_get_target_for(world, e3, EcsChildOf, Position));

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e0);
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e1);
        test_bool(true, ecs_query_next(&it));
        test_uint(e1, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(e0, ecs_field_src(&it, 1));
        {
            Position *p = ecs_field(&it, Position, 1);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e2);
        test_bool(true, ecs_query_next(&it));
        test_uint(e2, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(e0, ecs_field_src(&it, 1));
        {
            Position *p = ecs_field(&it, Position, 1);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e3);
        test_bool(true, ecs_query_next(&it));
        test_uint(e3, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(e0, ecs_field_src(&it, 1));
        {
            Position *p = ecs_field(&it, Position, 1);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_set_up_w_tag_w_3_levels_ccc(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Foo);

    ecs_entity_t e0 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e1 = ecs_new_w_pair(world, EcsChildOf, e0);
    ecs_entity_t e2 = ecs_new_w_pair(world, EcsChildOf, e1);
    ecs_entity_t e3 = ecs_new_w_pair(world, EcsChildOf, e2);

    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);
    ecs_add(world, e3, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ ecs_id(Position), .src.id = EcsUp }, { Foo }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    test_uint(e0, ecs_get_target_for(world, e1, EcsChildOf, Position));
    test_uint(e0, ecs_get_target_for(world, e2, EcsChildOf, Position));
    test_uint(e0, ecs_get_target_for(world, e3, EcsChildOf, Position));

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e0);
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e1);
        test_bool(true, ecs_query_next(&it));
        test_uint(e1, it.entities[0]);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 1));
        test_uint(e0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e2);
        test_bool(true, ecs_query_next(&it));
        test_uint(e2, it.entities[0]);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 1));
        test_uint(e0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e3);
        test_bool(true, ecs_query_next(&it));
        test_uint(e3, it.entities[0]);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 1));
        test_uint(e0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_set_up_w_tag_w_3_levels_ccp(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Foo);

    ecs_entity_t e0 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e1 = ecs_new_w_pair(world, EcsChildOf, e0);
    ecs_entity_t e2 = ecs_new_w_pair(world, EcsChildOf, e1);
    ecs_entity_t e3 = ecs_insert(world, ecs_value(EcsParent, {e2}));

    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);
    ecs_add(world, e3, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ ecs_id(Position), .src.id = EcsUp }, { Foo }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    test_uint(e0, ecs_get_target_for(world, e1, EcsChildOf, Position));
    test_uint(e0, ecs_get_target_for(world, e2, EcsChildOf, Position));
    test_uint(e0, ecs_get_target_for(world, e3, EcsChildOf, Position));

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e0);
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e1);
        test_bool(true, ecs_query_next(&it));
        test_uint(e1, it.entities[0]);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 1));
        test_uint(e0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e2);
        test_bool(true, ecs_query_next(&it));
        test_uint(e2, it.entities[0]);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 1));
        test_uint(e0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e3);
        test_bool(true, ecs_query_next(&it));
        test_uint(e3, it.entities[0]);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 1));
        test_uint(e0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_set_up_w_tag_w_3_levels_cpc(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Foo);

    ecs_entity_t e0 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e1 = ecs_new_w_pair(world, EcsChildOf, e0);
    ecs_entity_t e2 = ecs_insert(world, ecs_value(EcsParent, {e1}));
    ecs_entity_t e3 = ecs_new_w_pair(world, EcsChildOf, e2);

    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);
    ecs_add(world, e3, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ ecs_id(Position), .src.id = EcsUp }, { Foo }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    test_uint(e0, ecs_get_target_for(world, e1, EcsChildOf, Position));
    test_uint(e0, ecs_get_target_for(world, e2, EcsChildOf, Position));
    test_uint(e0, ecs_get_target_for(world, e3, EcsChildOf, Position));

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e0);
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e1);
        test_bool(true, ecs_query_next(&it));
        test_uint(e1, it.entities[0]);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 1));
        test_uint(e0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e2);
        test_bool(true, ecs_query_next(&it));
        test_uint(e2, it.entities[0]);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 1));
        test_uint(e0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e3);
        test_bool(true, ecs_query_next(&it));
        test_uint(e3, it.entities[0]);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 1));
        test_uint(e0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_set_up_w_tag_w_3_levels_cpp(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Foo);

    ecs_entity_t e0 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e1 = ecs_new_w_pair(world, EcsChildOf, e0);
    ecs_entity_t e2 = ecs_insert(world, ecs_value(EcsParent, {e1}));
    ecs_entity_t e3 = ecs_insert(world, ecs_value(EcsParent, {e2}));

    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);
    ecs_add(world, e3, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ ecs_id(Position), .src.id = EcsUp }, { Foo }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    test_uint(e0, ecs_get_target_for(world, e1, EcsChildOf, Position));
    test_uint(e0, ecs_get_target_for(world, e2, EcsChildOf, Position));
    test_uint(e0, ecs_get_target_for(world, e3, EcsChildOf, Position));

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e0);
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e1);
        test_bool(true, ecs_query_next(&it));
        test_uint(e1, it.entities[0]);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 1));
        test_uint(e0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e2);
        test_bool(true, ecs_query_next(&it));
        test_uint(e2, it.entities[0]);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 1));
        test_uint(e0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e3);
        test_bool(true, ecs_query_next(&it));
        test_uint(e3, it.entities[0]);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 1));
        test_uint(e0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_set_up_w_tag_w_3_levels_pcc(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Foo);

    ecs_entity_t e0 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e1 = ecs_insert(world, ecs_value(EcsParent, {e0}));
    ecs_entity_t e2 = ecs_new_w_pair(world, EcsChildOf, e1);
    ecs_entity_t e3 = ecs_new_w_pair(world, EcsChildOf, e2);

    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);
    ecs_add(world, e3, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ ecs_id(Position), .src.id = EcsUp }, { Foo }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    test_uint(e0, ecs_get_target_for(world, e1, EcsChildOf, Position));
    test_uint(e0, ecs_get_target_for(world, e2, EcsChildOf, Position));
    test_uint(e0, ecs_get_target_for(world, e3, EcsChildOf, Position));

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e0);
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e1);
        test_bool(true, ecs_query_next(&it));
        test_uint(e1, it.entities[0]);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 1));
        test_uint(e0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e2);
        test_bool(true, ecs_query_next(&it));
        test_uint(e2, it.entities[0]);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 1));
        test_uint(e0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e3);
        test_bool(true, ecs_query_next(&it));
        test_uint(e3, it.entities[0]);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 1));
        test_uint(e0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_set_up_w_tag_w_3_levels_pcp(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Foo);

    ecs_entity_t e0 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e1 = ecs_insert(world, ecs_value(EcsParent, {e0}));
    ecs_entity_t e2 = ecs_new_w_pair(world, EcsChildOf, e1);
    ecs_entity_t e3 = ecs_insert(world, ecs_value(EcsParent, {e2}));

    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);
    ecs_add(world, e3, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ ecs_id(Position), .src.id = EcsUp }, { Foo }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    test_uint(e0, ecs_get_target_for(world, e1, EcsChildOf, Position));
    test_uint(e0, ecs_get_target_for(world, e2, EcsChildOf, Position));
    test_uint(e0, ecs_get_target_for(world, e3, EcsChildOf, Position));

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e0);
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e1);
        test_bool(true, ecs_query_next(&it));
        test_uint(e1, it.entities[0]);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 1));
        test_uint(e0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e2);
        test_bool(true, ecs_query_next(&it));
        test_uint(e2, it.entities[0]);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 1));
        test_uint(e0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e3);
        test_bool(true, ecs_query_next(&it));
        test_uint(e3, it.entities[0]);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 1));
        test_uint(e0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_set_up_w_tag_w_3_levels_ppc(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Foo);

    ecs_entity_t e0 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e1 = ecs_insert(world, ecs_value(EcsParent, {e0}));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(EcsParent, {e1}));
    ecs_entity_t e3 = ecs_new_w_pair(world, EcsChildOf, e2);

    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);
    ecs_add(world, e3, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ ecs_id(Position), .src.id = EcsUp }, { Foo }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    test_uint(e0, ecs_get_target_for(world, e1, EcsChildOf, Position));
    test_uint(e0, ecs_get_target_for(world, e2, EcsChildOf, Position));
    test_uint(e0, ecs_get_target_for(world, e3, EcsChildOf, Position));

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e0);
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e1);
        test_bool(true, ecs_query_next(&it));
        test_uint(e1, it.entities[0]);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 1));
        test_uint(e0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e2);
        test_bool(true, ecs_query_next(&it));
        test_uint(e2, it.entities[0]);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 1));
        test_uint(e0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e3);
        test_bool(true, ecs_query_next(&it));
        test_uint(e3, it.entities[0]);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 1));
        test_uint(e0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_set_up_w_tag_w_3_levels_ppp(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Foo);

    ecs_entity_t e0 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e1 = ecs_insert(world, ecs_value(EcsParent, {e0}));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(EcsParent, {e1}));
    ecs_entity_t e3 = ecs_insert(world, ecs_value(EcsParent, {e2}));

    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);
    ecs_add(world, e3, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ ecs_id(Position), .src.id = EcsUp }, { Foo }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    test_uint(e0, ecs_get_target_for(world, e1, EcsChildOf, Position));
    test_uint(e0, ecs_get_target_for(world, e2, EcsChildOf, Position));
    test_uint(e0, ecs_get_target_for(world, e3, EcsChildOf, Position));

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e0);
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e1);
        test_bool(true, ecs_query_next(&it));
        test_uint(e1, it.entities[0]);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 1));
        test_uint(e0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e2);
        test_bool(true, ecs_query_next(&it));
        test_uint(e2, it.entities[0]);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 1));
        test_uint(e0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e3);
        test_bool(true, ecs_query_next(&it));
        test_uint(e3, it.entities[0]);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 1));
        test_uint(e0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_set_self_up_w_3_levels_ccc(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e0 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e1 = ecs_new_w_pair(world, EcsChildOf, e0);
    ecs_entity_t e2 = ecs_new_w_pair(world, EcsChildOf, e1);
    ecs_entity_t e3 = ecs_new_w_pair(world, EcsChildOf, e2);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ ecs_id(Position), .src.id = EcsSelf | EcsUp }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    test_uint(e0, ecs_get_target_for(world, e1, EcsChildOf, Position));
    test_uint(e0, ecs_get_target_for(world, e2, EcsChildOf, Position));
    test_uint(e0, ecs_get_target_for(world, e3, EcsChildOf, Position));

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e0);
        test_bool(true, ecs_query_next(&it));
        test_uint(e0, it.entities[0]);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(0, ecs_field_src(&it, 0));
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e1);
        test_bool(true, ecs_query_next(&it));
        test_uint(e1, it.entities[0]);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(e0, ecs_field_src(&it, 0));
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e2);
        test_bool(true, ecs_query_next(&it));
        test_uint(e2, it.entities[0]);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(e0, ecs_field_src(&it, 0));
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e3);
        test_bool(true, ecs_query_next(&it));
        test_uint(e3, it.entities[0]);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(e0, ecs_field_src(&it, 0));
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_set_self_up_w_3_levels_ccp(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e0 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e1 = ecs_new_w_pair(world, EcsChildOf, e0);
    ecs_entity_t e2 = ecs_new_w_pair(world, EcsChildOf, e1);
    ecs_entity_t e3 = ecs_insert(world, ecs_value(EcsParent, {e2}));

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ ecs_id(Position), .src.id = EcsSelf | EcsUp }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    test_uint(e0, ecs_get_target_for(world, e1, EcsChildOf, Position));
    test_uint(e0, ecs_get_target_for(world, e2, EcsChildOf, Position));
    test_uint(e0, ecs_get_target_for(world, e3, EcsChildOf, Position));

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e0);
        test_bool(true, ecs_query_next(&it));
        test_uint(e0, it.entities[0]);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(0, ecs_field_src(&it, 0));
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e1);
        test_bool(true, ecs_query_next(&it));
        test_uint(e1, it.entities[0]);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(e0, ecs_field_src(&it, 0));
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e2);
        test_bool(true, ecs_query_next(&it));
        test_uint(e2, it.entities[0]);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(e0, ecs_field_src(&it, 0));
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e3);
        test_bool(true, ecs_query_next(&it));
        test_uint(e3, it.entities[0]);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(e0, ecs_field_src(&it, 0));
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_set_self_up_w_3_levels_cpc(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e0 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e1 = ecs_new_w_pair(world, EcsChildOf, e0);
    ecs_entity_t e2 = ecs_insert(world, ecs_value(EcsParent, {e1}));
    ecs_entity_t e3 = ecs_new_w_pair(world, EcsChildOf, e2);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ ecs_id(Position), .src.id = EcsSelf | EcsUp }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    test_uint(e0, ecs_get_target_for(world, e1, EcsChildOf, Position));
    test_uint(e0, ecs_get_target_for(world, e2, EcsChildOf, Position));
    test_uint(e0, ecs_get_target_for(world, e3, EcsChildOf, Position));

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e0);
        test_bool(true, ecs_query_next(&it));
        test_uint(e0, it.entities[0]);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(0, ecs_field_src(&it, 0));
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e1);
        test_bool(true, ecs_query_next(&it));
        test_uint(e1, it.entities[0]);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(e0, ecs_field_src(&it, 0));
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e2);
        test_bool(true, ecs_query_next(&it));
        test_uint(e2, it.entities[0]);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(e0, ecs_field_src(&it, 0));
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e3);
        test_bool(true, ecs_query_next(&it));
        test_uint(e3, it.entities[0]);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(e0, ecs_field_src(&it, 0));
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_set_self_up_w_3_levels_cpp(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e0 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e1 = ecs_new_w_pair(world, EcsChildOf, e0);
    ecs_entity_t e2 = ecs_insert(world, ecs_value(EcsParent, {e1}));
    ecs_entity_t e3 = ecs_insert(world, ecs_value(EcsParent, {e2}));

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ ecs_id(Position), .src.id = EcsSelf | EcsUp }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    test_uint(e0, ecs_get_target_for(world, e1, EcsChildOf, Position));
    test_uint(e0, ecs_get_target_for(world, e2, EcsChildOf, Position));
    test_uint(e0, ecs_get_target_for(world, e3, EcsChildOf, Position));

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e0);
        test_bool(true, ecs_query_next(&it));
        test_uint(e0, it.entities[0]);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(0, ecs_field_src(&it, 0));
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e1);
        test_bool(true, ecs_query_next(&it));
        test_uint(e1, it.entities[0]);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(e0, ecs_field_src(&it, 0));
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e2);
        test_bool(true, ecs_query_next(&it));
        test_uint(e2, it.entities[0]);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(e0, ecs_field_src(&it, 0));
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e3);
        test_bool(true, ecs_query_next(&it));
        test_uint(e3, it.entities[0]);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(e0, ecs_field_src(&it, 0));
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_set_self_up_w_3_levels_pcc(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e0 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e1 = ecs_insert(world, ecs_value(EcsParent, {e0}));
    ecs_entity_t e2 = ecs_new_w_pair(world, EcsChildOf, e1);
    ecs_entity_t e3 = ecs_new_w_pair(world, EcsChildOf, e2);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ ecs_id(Position), .src.id = EcsSelf | EcsUp }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    test_uint(e0, ecs_get_target_for(world, e1, EcsChildOf, Position));
    test_uint(e0, ecs_get_target_for(world, e2, EcsChildOf, Position));
    test_uint(e0, ecs_get_target_for(world, e3, EcsChildOf, Position));

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e0);
        test_bool(true, ecs_query_next(&it));
        test_uint(e0, it.entities[0]);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(0, ecs_field_src(&it, 0));
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e1);
        test_bool(true, ecs_query_next(&it));
        test_uint(e1, it.entities[0]);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(e0, ecs_field_src(&it, 0));
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e2);
        test_bool(true, ecs_query_next(&it));
        test_uint(e2, it.entities[0]);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(e0, ecs_field_src(&it, 0));
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e3);
        test_bool(true, ecs_query_next(&it));
        test_uint(e3, it.entities[0]);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(e0, ecs_field_src(&it, 0));
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_set_self_up_w_3_levels_pcp(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e0 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e1 = ecs_insert(world, ecs_value(EcsParent, {e0}));
    ecs_entity_t e2 = ecs_new_w_pair(world, EcsChildOf, e1);
    ecs_entity_t e3 = ecs_insert(world, ecs_value(EcsParent, {e2}));

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ ecs_id(Position), .src.id = EcsSelf | EcsUp }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    test_uint(e0, ecs_get_target_for(world, e1, EcsChildOf, Position));
    test_uint(e0, ecs_get_target_for(world, e2, EcsChildOf, Position));
    test_uint(e0, ecs_get_target_for(world, e3, EcsChildOf, Position));

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e0);
        test_bool(true, ecs_query_next(&it));
        test_uint(e0, it.entities[0]);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(0, ecs_field_src(&it, 0));
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e1);
        test_bool(true, ecs_query_next(&it));
        test_uint(e1, it.entities[0]);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(e0, ecs_field_src(&it, 0));
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e2);
        test_bool(true, ecs_query_next(&it));
        test_uint(e2, it.entities[0]);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(e0, ecs_field_src(&it, 0));
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e3);
        test_bool(true, ecs_query_next(&it));
        test_uint(e3, it.entities[0]);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(e0, ecs_field_src(&it, 0));
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_set_self_up_w_3_levels_ppc(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e0 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e1 = ecs_insert(world, ecs_value(EcsParent, {e0}));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(EcsParent, {e1}));
    ecs_entity_t e3 = ecs_new_w_pair(world, EcsChildOf, e2);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ ecs_id(Position), .src.id = EcsSelf | EcsUp }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    test_uint(e0, ecs_get_target_for(world, e1, EcsChildOf, Position));
    test_uint(e0, ecs_get_target_for(world, e2, EcsChildOf, Position));
    test_uint(e0, ecs_get_target_for(world, e3, EcsChildOf, Position));

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e0);
        test_bool(true, ecs_query_next(&it));
        test_uint(e0, it.entities[0]);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(0, ecs_field_src(&it, 0));
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e1);
        test_bool(true, ecs_query_next(&it));
        test_uint(e1, it.entities[0]);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(e0, ecs_field_src(&it, 0));
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e2);
        test_bool(true, ecs_query_next(&it));
        test_uint(e2, it.entities[0]);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(e0, ecs_field_src(&it, 0));
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e3);
        test_bool(true, ecs_query_next(&it));
        test_uint(e3, it.entities[0]);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(e0, ecs_field_src(&it, 0));
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_set_self_up_w_3_levels_ppp(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e0 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e1 = ecs_insert(world, ecs_value(EcsParent, {e0}));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(EcsParent, {e1}));
    ecs_entity_t e3 = ecs_insert(world, ecs_value(EcsParent, {e2}));

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ ecs_id(Position), .src.id = EcsSelf | EcsUp }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    test_uint(e0, ecs_get_target_for(world, e1, EcsChildOf, Position));
    test_uint(e0, ecs_get_target_for(world, e2, EcsChildOf, Position));
    test_uint(e0, ecs_get_target_for(world, e3, EcsChildOf, Position));

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e0);
        test_bool(true, ecs_query_next(&it));
        test_uint(e0, it.entities[0]);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(0, ecs_field_src(&it, 0));
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e1);
        test_bool(true, ecs_query_next(&it));
        test_uint(e1, it.entities[0]);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(e0, ecs_field_src(&it, 0));
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e2);
        test_bool(true, ecs_query_next(&it));
        test_uint(e2, it.entities[0]);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(e0, ecs_field_src(&it, 0));
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e3);
        test_bool(true, ecs_query_next(&it));
        test_uint(e3, it.entities[0]);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(e0, ecs_field_src(&it, 0));
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_set_tag_w_self_up_w_3_levels_ccc(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Foo);

    ecs_entity_t e0 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e1 = ecs_new_w_pair(world, EcsChildOf, e0);
    ecs_entity_t e2 = ecs_new_w_pair(world, EcsChildOf, e1);
    ecs_entity_t e3 = ecs_new_w_pair(world, EcsChildOf, e2);

    ecs_add(world, e0, Foo);
    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);
    ecs_add(world, e3, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ Foo }, { ecs_id(Position), .src.id = EcsSelf | EcsUp }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    test_uint(e0, ecs_get_target_for(world, e1, EcsChildOf, Position));
    test_uint(e0, ecs_get_target_for(world, e2, EcsChildOf, Position));
    test_uint(e0, ecs_get_target_for(world, e3, EcsChildOf, Position));

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e0);
        test_bool(true, ecs_query_next(&it));
        test_uint(e0, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        {
            Position *p = ecs_field(&it, Position, 1);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e1);
        test_bool(true, ecs_query_next(&it));
        test_uint(e1, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(e0, ecs_field_src(&it, 1));
        {
            Position *p = ecs_field(&it, Position, 1);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e2);
        test_bool(true, ecs_query_next(&it));
        test_uint(e2, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(e0, ecs_field_src(&it, 1));
        {
            Position *p = ecs_field(&it, Position, 1);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e3);
        test_bool(true, ecs_query_next(&it));
        test_uint(e3, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(e0, ecs_field_src(&it, 1));
        {
            Position *p = ecs_field(&it, Position, 1);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_set_tag_w_self_up_w_3_levels_ccp(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Foo);

    ecs_entity_t e0 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e1 = ecs_new_w_pair(world, EcsChildOf, e0);
    ecs_entity_t e2 = ecs_new_w_pair(world, EcsChildOf, e1);
    ecs_entity_t e3 = ecs_insert(world, ecs_value(EcsParent, {e2}));

    ecs_add(world, e0, Foo);
    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);
    ecs_add(world, e3, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ Foo }, { ecs_id(Position), .src.id = EcsSelf | EcsUp }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    test_uint(e0, ecs_get_target_for(world, e1, EcsChildOf, Position));
    test_uint(e0, ecs_get_target_for(world, e2, EcsChildOf, Position));
    test_uint(e0, ecs_get_target_for(world, e3, EcsChildOf, Position));

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e0);
        test_bool(true, ecs_query_next(&it));
        test_uint(e0, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        {
            Position *p = ecs_field(&it, Position, 1);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e1);
        test_bool(true, ecs_query_next(&it));
        test_uint(e1, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(e0, ecs_field_src(&it, 1));
        {
            Position *p = ecs_field(&it, Position, 1);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e2);
        test_bool(true, ecs_query_next(&it));
        test_uint(e2, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(e0, ecs_field_src(&it, 1));
        {
            Position *p = ecs_field(&it, Position, 1);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e3);
        test_bool(true, ecs_query_next(&it));
        test_uint(e3, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(e0, ecs_field_src(&it, 1));
        {
            Position *p = ecs_field(&it, Position, 1);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_set_tag_w_self_up_w_3_levels_cpc(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Foo);

    ecs_entity_t e0 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e1 = ecs_new_w_pair(world, EcsChildOf, e0);
    ecs_entity_t e2 = ecs_insert(world, ecs_value(EcsParent, {e1}));
    ecs_entity_t e3 = ecs_new_w_pair(world, EcsChildOf, e2);

    ecs_add(world, e0, Foo);
    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);
    ecs_add(world, e3, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ Foo }, { ecs_id(Position), .src.id = EcsSelf | EcsUp }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    test_uint(e0, ecs_get_target_for(world, e1, EcsChildOf, Position));
    test_uint(e0, ecs_get_target_for(world, e2, EcsChildOf, Position));
    test_uint(e0, ecs_get_target_for(world, e3, EcsChildOf, Position));

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e0);
        test_bool(true, ecs_query_next(&it));
        test_uint(e0, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        {
            Position *p = ecs_field(&it, Position, 1);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e1);
        test_bool(true, ecs_query_next(&it));
        test_uint(e1, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(e0, ecs_field_src(&it, 1));
        {
            Position *p = ecs_field(&it, Position, 1);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e2);
        test_bool(true, ecs_query_next(&it));
        test_uint(e2, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(e0, ecs_field_src(&it, 1));
        {
            Position *p = ecs_field(&it, Position, 1);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e3);
        test_bool(true, ecs_query_next(&it));
        test_uint(e3, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(e0, ecs_field_src(&it, 1));
        {
            Position *p = ecs_field(&it, Position, 1);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_set_tag_w_self_up_w_3_levels_cpp(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Foo);

    ecs_entity_t e0 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e1 = ecs_new_w_pair(world, EcsChildOf, e0);
    ecs_entity_t e2 = ecs_insert(world, ecs_value(EcsParent, {e1}));
    ecs_entity_t e3 = ecs_insert(world, ecs_value(EcsParent, {e2}));

    ecs_add(world, e0, Foo);
    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);
    ecs_add(world, e3, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ Foo }, { ecs_id(Position), .src.id = EcsSelf | EcsUp }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    test_uint(e0, ecs_get_target_for(world, e1, EcsChildOf, Position));
    test_uint(e0, ecs_get_target_for(world, e2, EcsChildOf, Position));
    test_uint(e0, ecs_get_target_for(world, e3, EcsChildOf, Position));

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e0);
        test_bool(true, ecs_query_next(&it));
        test_uint(e0, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        {
            Position *p = ecs_field(&it, Position, 1);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e1);
        test_bool(true, ecs_query_next(&it));
        test_uint(e1, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(e0, ecs_field_src(&it, 1));
        {
            Position *p = ecs_field(&it, Position, 1);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e2);
        test_bool(true, ecs_query_next(&it));
        test_uint(e2, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(e0, ecs_field_src(&it, 1));
        {
            Position *p = ecs_field(&it, Position, 1);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e3);
        test_bool(true, ecs_query_next(&it));
        test_uint(e3, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(e0, ecs_field_src(&it, 1));
        {
            Position *p = ecs_field(&it, Position, 1);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_set_tag_w_self_up_w_3_levels_pcc(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Foo);

    ecs_entity_t e0 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e1 = ecs_insert(world, ecs_value(EcsParent, {e0}));
    ecs_entity_t e2 = ecs_new_w_pair(world, EcsChildOf, e1);
    ecs_entity_t e3 = ecs_new_w_pair(world, EcsChildOf, e2);

    ecs_add(world, e0, Foo);
    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);
    ecs_add(world, e3, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ Foo }, { ecs_id(Position), .src.id = EcsSelf | EcsUp }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    test_uint(e0, ecs_get_target_for(world, e1, EcsChildOf, Position));
    test_uint(e0, ecs_get_target_for(world, e2, EcsChildOf, Position));
    test_uint(e0, ecs_get_target_for(world, e3, EcsChildOf, Position));

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e0);
        test_bool(true, ecs_query_next(&it));
        test_uint(e0, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        {
            Position *p = ecs_field(&it, Position, 1);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e1);
        test_bool(true, ecs_query_next(&it));
        test_uint(e1, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(e0, ecs_field_src(&it, 1));
        {
            Position *p = ecs_field(&it, Position, 1);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e2);
        test_bool(true, ecs_query_next(&it));
        test_uint(e2, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(e0, ecs_field_src(&it, 1));
        {
            Position *p = ecs_field(&it, Position, 1);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e3);
        test_bool(true, ecs_query_next(&it));
        test_uint(e3, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(e0, ecs_field_src(&it, 1));
        {
            Position *p = ecs_field(&it, Position, 1);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_set_tag_w_self_up_w_3_levels_pcp(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Foo);

    ecs_entity_t e0 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e1 = ecs_insert(world, ecs_value(EcsParent, {e0}));
    ecs_entity_t e2 = ecs_new_w_pair(world, EcsChildOf, e1);
    ecs_entity_t e3 = ecs_insert(world, ecs_value(EcsParent, {e2}));

    ecs_add(world, e0, Foo);
    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);
    ecs_add(world, e3, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ Foo }, { ecs_id(Position), .src.id = EcsSelf | EcsUp }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    test_uint(e0, ecs_get_target_for(world, e1, EcsChildOf, Position));
    test_uint(e0, ecs_get_target_for(world, e2, EcsChildOf, Position));
    test_uint(e0, ecs_get_target_for(world, e3, EcsChildOf, Position));

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e0);
        test_bool(true, ecs_query_next(&it));
        test_uint(e0, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        {
            Position *p = ecs_field(&it, Position, 1);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e1);
        test_bool(true, ecs_query_next(&it));
        test_uint(e1, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(e0, ecs_field_src(&it, 1));
        {
            Position *p = ecs_field(&it, Position, 1);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e2);
        test_bool(true, ecs_query_next(&it));
        test_uint(e2, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(e0, ecs_field_src(&it, 1));
        {
            Position *p = ecs_field(&it, Position, 1);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e3);
        test_bool(true, ecs_query_next(&it));
        test_uint(e3, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(e0, ecs_field_src(&it, 1));
        {
            Position *p = ecs_field(&it, Position, 1);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_set_tag_w_self_up_w_3_levels_ppc(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Foo);

    ecs_entity_t e0 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e1 = ecs_insert(world, ecs_value(EcsParent, {e0}));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(EcsParent, {e1}));
    ecs_entity_t e3 = ecs_new_w_pair(world, EcsChildOf, e2);

    ecs_add(world, e0, Foo);
    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);
    ecs_add(world, e3, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ Foo }, { ecs_id(Position), .src.id = EcsSelf | EcsUp }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    test_uint(e0, ecs_get_target_for(world, e1, EcsChildOf, Position));
    test_uint(e0, ecs_get_target_for(world, e2, EcsChildOf, Position));
    test_uint(e0, ecs_get_target_for(world, e3, EcsChildOf, Position));

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e0);
        test_bool(true, ecs_query_next(&it));
        test_uint(e0, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        {
            Position *p = ecs_field(&it, Position, 1);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e1);
        test_bool(true, ecs_query_next(&it));
        test_uint(e1, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(e0, ecs_field_src(&it, 1));
        {
            Position *p = ecs_field(&it, Position, 1);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e2);
        test_bool(true, ecs_query_next(&it));
        test_uint(e2, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(e0, ecs_field_src(&it, 1));
        {
            Position *p = ecs_field(&it, Position, 1);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e3);
        test_bool(true, ecs_query_next(&it));
        test_uint(e3, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(e0, ecs_field_src(&it, 1));
        {
            Position *p = ecs_field(&it, Position, 1);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_set_tag_w_self_up_w_3_levels_ppp(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Foo);

    ecs_entity_t e0 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e1 = ecs_insert(world, ecs_value(EcsParent, {e0}));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(EcsParent, {e1}));
    ecs_entity_t e3 = ecs_insert(world, ecs_value(EcsParent, {e2}));

    ecs_add(world, e0, Foo);
    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);
    ecs_add(world, e3, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ Foo }, { ecs_id(Position), .src.id = EcsSelf | EcsUp }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    test_uint(e0, ecs_get_target_for(world, e1, EcsChildOf, Position));
    test_uint(e0, ecs_get_target_for(world, e2, EcsChildOf, Position));
    test_uint(e0, ecs_get_target_for(world, e3, EcsChildOf, Position));

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e0);
        test_bool(true, ecs_query_next(&it));
        test_uint(e0, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        {
            Position *p = ecs_field(&it, Position, 1);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e1);
        test_bool(true, ecs_query_next(&it));
        test_uint(e1, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(e0, ecs_field_src(&it, 1));
        {
            Position *p = ecs_field(&it, Position, 1);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e2);
        test_bool(true, ecs_query_next(&it));
        test_uint(e2, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(e0, ecs_field_src(&it, 1));
        {
            Position *p = ecs_field(&it, Position, 1);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e3);
        test_bool(true, ecs_query_next(&it));
        test_uint(e3, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(e0, ecs_field_src(&it, 1));
        {
            Position *p = ecs_field(&it, Position, 1);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_set_self_up_w_tag_w_3_levels_ccc(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Foo);

    ecs_entity_t e0 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e1 = ecs_new_w_pair(world, EcsChildOf, e0);
    ecs_entity_t e2 = ecs_new_w_pair(world, EcsChildOf, e1);
    ecs_entity_t e3 = ecs_new_w_pair(world, EcsChildOf, e2);

    ecs_add(world, e0, Foo);
    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);
    ecs_add(world, e3, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ ecs_id(Position), .src.id = EcsSelf | EcsUp }, { Foo }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    test_uint(e0, ecs_get_target_for(world, e1, EcsChildOf, Position));
    test_uint(e0, ecs_get_target_for(world, e2, EcsChildOf, Position));
    test_uint(e0, ecs_get_target_for(world, e3, EcsChildOf, Position));

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e0);
        test_bool(true, ecs_query_next(&it));
        test_uint(e0, it.entities[0]);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e1);
        test_bool(true, ecs_query_next(&it));
        test_uint(e1, it.entities[0]);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 1));
        test_uint(e0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e2);
        test_bool(true, ecs_query_next(&it));
        test_uint(e2, it.entities[0]);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 1));
        test_uint(e0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e3);
        test_bool(true, ecs_query_next(&it));
        test_uint(e3, it.entities[0]);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 1));
        test_uint(e0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_set_self_up_w_tag_w_3_levels_ccp(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Foo);

    ecs_entity_t e0 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e1 = ecs_new_w_pair(world, EcsChildOf, e0);
    ecs_entity_t e2 = ecs_new_w_pair(world, EcsChildOf, e1);
    ecs_entity_t e3 = ecs_insert(world, ecs_value(EcsParent, {e2}));

    ecs_add(world, e0, Foo);
    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);
    ecs_add(world, e3, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ ecs_id(Position), .src.id = EcsSelf | EcsUp }, { Foo }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    test_uint(e0, ecs_get_target_for(world, e1, EcsChildOf, Position));
    test_uint(e0, ecs_get_target_for(world, e2, EcsChildOf, Position));
    test_uint(e0, ecs_get_target_for(world, e3, EcsChildOf, Position));

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e0);
        test_bool(true, ecs_query_next(&it));
        test_uint(e0, it.entities[0]);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e1);
        test_bool(true, ecs_query_next(&it));
        test_uint(e1, it.entities[0]);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 1));
        test_uint(e0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e2);
        test_bool(true, ecs_query_next(&it));
        test_uint(e2, it.entities[0]);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 1));
        test_uint(e0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e3);
        test_bool(true, ecs_query_next(&it));
        test_uint(e3, it.entities[0]);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 1));
        test_uint(e0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_set_self_up_w_tag_w_3_levels_cpc(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Foo);

    ecs_entity_t e0 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e1 = ecs_new_w_pair(world, EcsChildOf, e0);
    ecs_entity_t e2 = ecs_insert(world, ecs_value(EcsParent, {e1}));
    ecs_entity_t e3 = ecs_new_w_pair(world, EcsChildOf, e2);

    ecs_add(world, e0, Foo);
    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);
    ecs_add(world, e3, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ ecs_id(Position), .src.id = EcsSelf | EcsUp }, { Foo }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    test_uint(e0, ecs_get_target_for(world, e1, EcsChildOf, Position));
    test_uint(e0, ecs_get_target_for(world, e2, EcsChildOf, Position));
    test_uint(e0, ecs_get_target_for(world, e3, EcsChildOf, Position));

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e0);
        test_bool(true, ecs_query_next(&it));
        test_uint(e0, it.entities[0]);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e1);
        test_bool(true, ecs_query_next(&it));
        test_uint(e1, it.entities[0]);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 1));
        test_uint(e0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e2);
        test_bool(true, ecs_query_next(&it));
        test_uint(e2, it.entities[0]);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 1));
        test_uint(e0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e3);
        test_bool(true, ecs_query_next(&it));
        test_uint(e3, it.entities[0]);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 1));
        test_uint(e0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_set_self_up_w_tag_w_3_levels_cpp(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Foo);

    ecs_entity_t e0 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e1 = ecs_new_w_pair(world, EcsChildOf, e0);
    ecs_entity_t e2 = ecs_insert(world, ecs_value(EcsParent, {e1}));
    ecs_entity_t e3 = ecs_insert(world, ecs_value(EcsParent, {e2}));

    ecs_add(world, e0, Foo);
    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);
    ecs_add(world, e3, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ ecs_id(Position), .src.id = EcsSelf | EcsUp }, { Foo }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    test_uint(e0, ecs_get_target_for(world, e1, EcsChildOf, Position));
    test_uint(e0, ecs_get_target_for(world, e2, EcsChildOf, Position));
    test_uint(e0, ecs_get_target_for(world, e3, EcsChildOf, Position));

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e0);
        test_bool(true, ecs_query_next(&it));
        test_uint(e0, it.entities[0]);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e1);
        test_bool(true, ecs_query_next(&it));
        test_uint(e1, it.entities[0]);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 1));
        test_uint(e0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e2);
        test_bool(true, ecs_query_next(&it));
        test_uint(e2, it.entities[0]);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 1));
        test_uint(e0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e3);
        test_bool(true, ecs_query_next(&it));
        test_uint(e3, it.entities[0]);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 1));
        test_uint(e0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_set_self_up_w_tag_w_3_levels_pcc(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Foo);

    ecs_entity_t e0 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e1 = ecs_insert(world, ecs_value(EcsParent, {e0}));
    ecs_entity_t e2 = ecs_new_w_pair(world, EcsChildOf, e1);
    ecs_entity_t e3 = ecs_new_w_pair(world, EcsChildOf, e2);

    ecs_add(world, e0, Foo);
    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);
    ecs_add(world, e3, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ ecs_id(Position), .src.id = EcsSelf | EcsUp }, { Foo }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    test_uint(e0, ecs_get_target_for(world, e1, EcsChildOf, Position));
    test_uint(e0, ecs_get_target_for(world, e2, EcsChildOf, Position));
    test_uint(e0, ecs_get_target_for(world, e3, EcsChildOf, Position));

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e0);
        test_bool(true, ecs_query_next(&it));
        test_uint(e0, it.entities[0]);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e1);
        test_bool(true, ecs_query_next(&it));
        test_uint(e1, it.entities[0]);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 1));
        test_uint(e0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e2);
        test_bool(true, ecs_query_next(&it));
        test_uint(e2, it.entities[0]);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 1));
        test_uint(e0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e3);
        test_bool(true, ecs_query_next(&it));
        test_uint(e3, it.entities[0]);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 1));
        test_uint(e0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_set_self_up_w_tag_w_3_levels_pcp(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Foo);

    ecs_entity_t e0 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e1 = ecs_insert(world, ecs_value(EcsParent, {e0}));
    ecs_entity_t e2 = ecs_new_w_pair(world, EcsChildOf, e1);
    ecs_entity_t e3 = ecs_insert(world, ecs_value(EcsParent, {e2}));

    ecs_add(world, e0, Foo);
    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);
    ecs_add(world, e3, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ ecs_id(Position), .src.id = EcsSelf | EcsUp }, { Foo }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    test_uint(e0, ecs_get_target_for(world, e1, EcsChildOf, Position));
    test_uint(e0, ecs_get_target_for(world, e2, EcsChildOf, Position));
    test_uint(e0, ecs_get_target_for(world, e3, EcsChildOf, Position));

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e0);
        test_bool(true, ecs_query_next(&it));
        test_uint(e0, it.entities[0]);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e1);
        test_bool(true, ecs_query_next(&it));
        test_uint(e1, it.entities[0]);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 1));
        test_uint(e0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e2);
        test_bool(true, ecs_query_next(&it));
        test_uint(e2, it.entities[0]);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 1));
        test_uint(e0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e3);
        test_bool(true, ecs_query_next(&it));
        test_uint(e3, it.entities[0]);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 1));
        test_uint(e0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_set_self_up_w_tag_w_3_levels_ppc(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Foo);

    ecs_entity_t e0 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e1 = ecs_insert(world, ecs_value(EcsParent, {e0}));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(EcsParent, {e1}));
    ecs_entity_t e3 = ecs_new_w_pair(world, EcsChildOf, e2);

    ecs_add(world, e0, Foo);
    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);
    ecs_add(world, e3, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ ecs_id(Position), .src.id = EcsSelf | EcsUp }, { Foo }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    test_uint(e0, ecs_get_target_for(world, e1, EcsChildOf, Position));
    test_uint(e0, ecs_get_target_for(world, e2, EcsChildOf, Position));
    test_uint(e0, ecs_get_target_for(world, e3, EcsChildOf, Position));

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e0);
        test_bool(true, ecs_query_next(&it));
        test_uint(e0, it.entities[0]);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e1);
        test_bool(true, ecs_query_next(&it));
        test_uint(e1, it.entities[0]);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 1));
        test_uint(e0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e2);
        test_bool(true, ecs_query_next(&it));
        test_uint(e2, it.entities[0]);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 1));
        test_uint(e0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e3);
        test_bool(true, ecs_query_next(&it));
        test_uint(e3, it.entities[0]);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 1));
        test_uint(e0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_set_self_up_w_tag_w_3_levels_ppp(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Foo);

    ecs_entity_t e0 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e1 = ecs_insert(world, ecs_value(EcsParent, {e0}));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(EcsParent, {e1}));
    ecs_entity_t e3 = ecs_insert(world, ecs_value(EcsParent, {e2}));

    ecs_add(world, e0, Foo);
    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);
    ecs_add(world, e3, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ ecs_id(Position), .src.id = EcsSelf | EcsUp }, { Foo }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    test_uint(e0, ecs_get_target_for(world, e1, EcsChildOf, Position));
    test_uint(e0, ecs_get_target_for(world, e2, EcsChildOf, Position));
    test_uint(e0, ecs_get_target_for(world, e3, EcsChildOf, Position));

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e0);
        test_bool(true, ecs_query_next(&it));
        test_uint(e0, it.entities[0]);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e1);
        test_bool(true, ecs_query_next(&it));
        test_uint(e1, it.entities[0]);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 1));
        test_uint(e0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e2);
        test_bool(true, ecs_query_next(&it));
        test_uint(e2, it.entities[0]);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 1));
        test_uint(e0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    } {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e3);
        test_bool(true, ecs_query_next(&it));
        test_uint(e3, it.entities[0]);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 1));
        test_uint(e0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 10); test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_2_up(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TAG(world, Foo);

    ecs_entity_t parent = ecs_new(world);
    ecs_set(world, parent, Position, {10, 20});
    ecs_set(world, parent, Velocity, {1, 2});

    ecs_entity_t c1 = ecs_insert(world, ecs_value(EcsParent, {parent}));
    ecs_entity_t c2 = ecs_insert(world, ecs_value(EcsParent, {parent}));
    ecs_entity_t c3 = ecs_insert(world, ecs_value(EcsParent, {parent}));
    ecs_add(world, c3, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { ecs_id(Position), .src.id = EcsUp },
            { ecs_id(Velocity), .src.id = EcsUp }
        },
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(c1, it.entities[0]);
    test_uint(parent, ecs_field_src(&it, 0));
    test_uint(parent, ecs_field_src(&it, 1));
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(ecs_id(Velocity), ecs_field_id(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_int(p->x, 10); test_int(p->y, 20);
        Velocity *v = ecs_field(&it, Velocity, 1);
        test_int(v->x, 1); test_int(v->y, 2);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(c2, it.entities[0]);
    test_uint(parent, ecs_field_src(&it, 0));
    test_uint(parent, ecs_field_src(&it, 1));
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(ecs_id(Velocity), ecs_field_id(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_int(p->x, 10); test_int(p->y, 20);
        Velocity *v = ecs_field(&it, Velocity, 1);
        test_int(v->x, 1); test_int(v->y, 2);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(c3, it.entities[0]);
    test_uint(parent, ecs_field_src(&it, 0));
    test_uint(parent, ecs_field_src(&it, 1));
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(ecs_id(Velocity), ecs_field_id(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_int(p->x, 10); test_int(p->y, 20);
        Velocity *v = ecs_field(&it, Velocity, 1);
        test_int(v->x, 1); test_int(v->y, 2);
    }

    test_bool(false, ecs_query_next(&it));
    
    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_2_self_up(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TAG(world, Foo);

    ecs_entity_t parent = ecs_new(world);
    ecs_set(world, parent, Position, {10, 20});
    ecs_set(world, parent, Velocity, {1, 2});

    ecs_entity_t c1 = ecs_insert(world, ecs_value(EcsParent, {parent}));
    ecs_entity_t c2 = ecs_insert(world, ecs_value(EcsParent, {parent}));
    ecs_entity_t c3 = ecs_insert(world, ecs_value(EcsParent, {parent}));
    ecs_add(world, c3, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { ecs_id(Position), .src.id = EcsSelf | EcsUp },
            { ecs_id(Velocity), .src.id = EcsSelf | EcsUp }
        },
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(parent, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(ecs_id(Velocity), ecs_field_id(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_int(p->x, 10); test_int(p->y, 20);
        Velocity *v = ecs_field(&it, Velocity, 1);
        test_int(v->x, 1); test_int(v->y, 2);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(c1, it.entities[0]);
    test_uint(parent, ecs_field_src(&it, 0));
    test_uint(parent, ecs_field_src(&it, 1));
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(ecs_id(Velocity), ecs_field_id(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_int(p->x, 10); test_int(p->y, 20);
        Velocity *v = ecs_field(&it, Velocity, 1);
        test_int(v->x, 1); test_int(v->y, 2);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(c2, it.entities[0]);
    test_uint(parent, ecs_field_src(&it, 0));
    test_uint(parent, ecs_field_src(&it, 1));
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(ecs_id(Velocity), ecs_field_id(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_int(p->x, 10); test_int(p->y, 20);
        Velocity *v = ecs_field(&it, Velocity, 1);
        test_int(v->x, 1); test_int(v->y, 2);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(c3, it.entities[0]);
    test_uint(parent, ecs_field_src(&it, 0));
    test_uint(parent, ecs_field_src(&it, 1));
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(ecs_id(Velocity), ecs_field_id(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_int(p->x, 10); test_int(p->y, 20);
        Velocity *v = ecs_field(&it, Velocity, 1);
        test_int(v->x, 1); test_int(v->y, 2);
    }

    test_bool(false, ecs_query_next(&it));
    
    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_written_2_up(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_entity_t parent = ecs_new(world);
    ecs_set(world, parent, Position, {10, 20});
    ecs_set(world, parent, Velocity, {1, 2});

    ecs_entity_t c1 = ecs_insert(world, ecs_value(EcsParent, {parent}));
    ecs_entity_t c2 = ecs_insert(world, ecs_value(EcsParent, {parent}));
    ecs_entity_t c3 = ecs_insert(world, ecs_value(EcsParent, {parent}));

    ecs_add(world, parent, Bar);
    ecs_add(world, c1, Bar);
    ecs_add(world, c2, Bar);
    ecs_add(world, c3, Bar);

    ecs_add(world, c3, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { Bar },
            { ecs_id(Position), .src.id = EcsUp },
            { ecs_id(Velocity), .src.id = EcsUp }
        },
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(c1, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(parent, ecs_field_src(&it, 1));
    test_uint(parent, ecs_field_src(&it, 2));
    test_uint(Bar, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(ecs_id(Velocity), ecs_field_id(&it, 2));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_int(p->x, 10); test_int(p->y, 20);
        Velocity *v = ecs_field(&it, Velocity, 2);
        test_int(v->x, 1); test_int(v->y, 2);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(c2, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(parent, ecs_field_src(&it, 1));
    test_uint(parent, ecs_field_src(&it, 2));
    test_uint(Bar, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(ecs_id(Velocity), ecs_field_id(&it, 2));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_int(p->x, 10); test_int(p->y, 20);
        Velocity *v = ecs_field(&it, Velocity, 2);
        test_int(v->x, 1); test_int(v->y, 2);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(c3, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(parent, ecs_field_src(&it, 1));
    test_uint(parent, ecs_field_src(&it, 2));
    test_uint(Bar, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(ecs_id(Velocity), ecs_field_id(&it, 2));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_int(p->x, 10); test_int(p->y, 20);
        Velocity *v = ecs_field(&it, Velocity, 2);
        test_int(v->x, 1); test_int(v->y, 2);
    }

    test_bool(false, ecs_query_next(&it));
    
    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_written_2_self_up(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_entity_t parent = ecs_new(world);
    ecs_set(world, parent, Position, {10, 20});
    ecs_set(world, parent, Velocity, {1, 2});

    ecs_entity_t c1 = ecs_insert(world, ecs_value(EcsParent, {parent}));
    ecs_entity_t c2 = ecs_insert(world, ecs_value(EcsParent, {parent}));
    ecs_entity_t c3 = ecs_insert(world, ecs_value(EcsParent, {parent}));

    ecs_add(world, parent, Bar);
    ecs_add(world, c1, Bar);
    ecs_add(world, c2, Bar);
    ecs_add(world, c3, Bar);

    ecs_add(world, c3, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { Bar },
            { ecs_id(Position), .src.id = EcsSelf | EcsUp },
            { ecs_id(Velocity), .src.id = EcsSelf | EcsUp }
        },
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(parent, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    test_uint(0, ecs_field_src(&it, 2));
    test_uint(Bar, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(ecs_id(Velocity), ecs_field_id(&it, 2));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_int(p->x, 10); test_int(p->y, 20);
        Velocity *v = ecs_field(&it, Velocity, 2);
        test_int(v->x, 1); test_int(v->y, 2);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(c1, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(parent, ecs_field_src(&it, 1));
    test_uint(parent, ecs_field_src(&it, 2));
    test_uint(Bar, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(ecs_id(Velocity), ecs_field_id(&it, 2));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_int(p->x, 10); test_int(p->y, 20);
        Velocity *v = ecs_field(&it, Velocity, 2);
        test_int(v->x, 1); test_int(v->y, 2);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(c2, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(parent, ecs_field_src(&it, 1));
    test_uint(parent, ecs_field_src(&it, 2));
    test_uint(Bar, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(ecs_id(Velocity), ecs_field_id(&it, 2));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_int(p->x, 10); test_int(p->y, 20);
        Velocity *v = ecs_field(&it, Velocity, 2);
        test_int(v->x, 1); test_int(v->y, 2);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(c3, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(parent, ecs_field_src(&it, 1));
    test_uint(parent, ecs_field_src(&it, 2));
    test_uint(Bar, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(ecs_id(Velocity), ecs_field_id(&it, 2));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_int(p->x, 10); test_int(p->y, 20);
        Velocity *v = ecs_field(&it, Velocity, 2);
        test_int(v->x, 1); test_int(v->y, 2);
    }

    test_bool(false, ecs_query_next(&it));
    
    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_2_up_different_parents(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TAG(world, Foo);

    ecs_entity_t root = ecs_new(world);
    ecs_set(world, root, Velocity, {1, 2});

    ecs_entity_t parent = ecs_new_w_pair(world, EcsChildOf, root);
    ecs_set(world, parent, Position, {10, 20});

    ecs_entity_t c1 = ecs_insert(world, ecs_value(EcsParent, {parent}));
    ecs_entity_t c2 = ecs_insert(world, ecs_value(EcsParent, {parent}));
    ecs_entity_t c3 = ecs_insert(world, ecs_value(EcsParent, {parent}));
    ecs_add(world, c3, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { ecs_id(Position), .src.id = EcsUp },
            { ecs_id(Velocity), .src.id = EcsUp }
        },
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(c1, it.entities[0]);
    test_uint(parent, ecs_field_src(&it, 0));
    test_uint(root, ecs_field_src(&it, 1));
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(ecs_id(Velocity), ecs_field_id(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_int(p->x, 10); test_int(p->y, 20);
        Velocity *v = ecs_field(&it, Velocity, 1);
        test_int(v->x, 1); test_int(v->y, 2);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(c2, it.entities[0]);
    test_uint(parent, ecs_field_src(&it, 0));
    test_uint(root, ecs_field_src(&it, 1));
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(ecs_id(Velocity), ecs_field_id(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_int(p->x, 10); test_int(p->y, 20);
        Velocity *v = ecs_field(&it, Velocity, 1);
        test_int(v->x, 1); test_int(v->y, 2);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(c3, it.entities[0]);
    test_uint(parent, ecs_field_src(&it, 0));
    test_uint(root, ecs_field_src(&it, 1));
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(ecs_id(Velocity), ecs_field_id(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_int(p->x, 10); test_int(p->y, 20);
        Velocity *v = ecs_field(&it, Velocity, 1);
        test_int(v->x, 1); test_int(v->y, 2);
    }

    test_bool(false, ecs_query_next(&it));
    
    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_2_self_up_different_parents(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TAG(world, Foo);

    ecs_entity_t root = ecs_new(world);
    ecs_set(world, root, Velocity, {1, 2});

    ecs_entity_t parent = ecs_new_w_pair(world, EcsChildOf, root);
    ecs_set(world, parent, Position, {10, 20});

    ecs_entity_t c1 = ecs_insert(world, ecs_value(EcsParent, {parent}));
    ecs_entity_t c2 = ecs_insert(world, ecs_value(EcsParent, {parent}));
    ecs_entity_t c3 = ecs_insert(world, ecs_value(EcsParent, {parent}));
    ecs_add(world, c3, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { ecs_id(Position), .src.id = EcsSelf | EcsUp },
            { ecs_id(Velocity), .src.id = EcsSelf | EcsUp }
        },
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(parent, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(root, ecs_field_src(&it, 1));
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(ecs_id(Velocity), ecs_field_id(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_int(p->x, 10); test_int(p->y, 20);
        Velocity *v = ecs_field(&it, Velocity, 1);
        test_int(v->x, 1); test_int(v->y, 2);
    }
    
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(c1, it.entities[0]);
    test_uint(parent, ecs_field_src(&it, 0));
    test_uint(root, ecs_field_src(&it, 1));
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(ecs_id(Velocity), ecs_field_id(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_int(p->x, 10); test_int(p->y, 20);
        Velocity *v = ecs_field(&it, Velocity, 1);
        test_int(v->x, 1); test_int(v->y, 2);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(c2, it.entities[0]);
    test_uint(parent, ecs_field_src(&it, 0));
    test_uint(root, ecs_field_src(&it, 1));
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(ecs_id(Velocity), ecs_field_id(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_int(p->x, 10); test_int(p->y, 20);
        Velocity *v = ecs_field(&it, Velocity, 1);
        test_int(v->x, 1); test_int(v->y, 2);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(c3, it.entities[0]);
    test_uint(parent, ecs_field_src(&it, 0));
    test_uint(root, ecs_field_src(&it, 1));
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(ecs_id(Velocity), ecs_field_id(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_int(p->x, 10); test_int(p->y, 20);
        Velocity *v = ecs_field(&it, Velocity, 1);
        test_int(v->x, 1); test_int(v->y, 2);
    }

    test_bool(false, ecs_query_next(&it));
    
    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_written_2_up_different_parents(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_entity_t root = ecs_new(world);
    ecs_set(world, root, Velocity, {1, 2});

    ecs_entity_t parent = ecs_new_w_pair(world, EcsChildOf, root);
    ecs_set(world, parent, Position, {10, 20});

    ecs_entity_t c1 = ecs_insert(world, ecs_value(EcsParent, {parent}));
    ecs_entity_t c2 = ecs_insert(world, ecs_value(EcsParent, {parent}));
    ecs_entity_t c3 = ecs_insert(world, ecs_value(EcsParent, {parent}));

    ecs_add(world, root, Bar);
    ecs_add(world, parent, Bar);
    ecs_add(world, c1, Bar);
    ecs_add(world, c2, Bar);
    ecs_add(world, c3, Bar);

    ecs_add(world, c3, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { Bar },
            { ecs_id(Position), .src.id = EcsUp },
            { ecs_id(Velocity), .src.id = EcsUp }
        },
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(c1, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(parent, ecs_field_src(&it, 1));
    test_uint(root, ecs_field_src(&it, 2));
    test_uint(Bar, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(ecs_id(Velocity), ecs_field_id(&it, 2));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_int(p->x, 10); test_int(p->y, 20);
        Velocity *v = ecs_field(&it, Velocity, 2);
        test_int(v->x, 1); test_int(v->y, 2);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(c2, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(parent, ecs_field_src(&it, 1));
    test_uint(root, ecs_field_src(&it, 2));
    test_uint(Bar, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(ecs_id(Velocity), ecs_field_id(&it, 2));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_int(p->x, 10); test_int(p->y, 20);
        Velocity *v = ecs_field(&it, Velocity, 2);
        test_int(v->x, 1); test_int(v->y, 2);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(c3, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(parent, ecs_field_src(&it, 1));
    test_uint(root, ecs_field_src(&it, 2));
    test_uint(Bar, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(ecs_id(Velocity), ecs_field_id(&it, 2));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_int(p->x, 10); test_int(p->y, 20);
        Velocity *v = ecs_field(&it, Velocity, 2);
        test_int(v->x, 1); test_int(v->y, 2);
    }

    test_bool(false, ecs_query_next(&it));
    
    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_written_2_self_up_different_parents(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_entity_t root = ecs_new(world);
    ecs_set(world, root, Velocity, {1, 2});

    ecs_entity_t parent = ecs_new_w_pair(world, EcsChildOf, root);
    ecs_set(world, parent, Position, {10, 20});

    ecs_entity_t c1 = ecs_insert(world, ecs_value(EcsParent, {parent}));
    ecs_entity_t c2 = ecs_insert(world, ecs_value(EcsParent, {parent}));
    ecs_entity_t c3 = ecs_insert(world, ecs_value(EcsParent, {parent}));

    ecs_add(world, root, Bar);
    ecs_add(world, parent, Bar);
    ecs_add(world, c1, Bar);
    ecs_add(world, c2, Bar);
    ecs_add(world, c3, Bar);

    ecs_add(world, c3, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { Bar },
            { ecs_id(Position), .src.id = EcsSelf | EcsUp },
            { ecs_id(Velocity), .src.id = EcsSelf | EcsUp }
        },
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(parent, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    test_uint(root, ecs_field_src(&it, 2));
    test_uint(Bar, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(ecs_id(Velocity), ecs_field_id(&it, 2));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_int(p->x, 10); test_int(p->y, 20);
        Velocity *v = ecs_field(&it, Velocity, 2);
        test_int(v->x, 1); test_int(v->y, 2);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(c1, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(parent, ecs_field_src(&it, 1));
    test_uint(root, ecs_field_src(&it, 2));
    test_uint(Bar, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(ecs_id(Velocity), ecs_field_id(&it, 2));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_int(p->x, 10); test_int(p->y, 20);
        Velocity *v = ecs_field(&it, Velocity, 2);
        test_int(v->x, 1); test_int(v->y, 2);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(c2, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(parent, ecs_field_src(&it, 1));
    test_uint(root, ecs_field_src(&it, 2));
    test_uint(Bar, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(ecs_id(Velocity), ecs_field_id(&it, 2));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_int(p->x, 10); test_int(p->y, 20);
        Velocity *v = ecs_field(&it, Velocity, 2);
        test_int(v->x, 1); test_int(v->y, 2);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(c3, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(parent, ecs_field_src(&it, 1));
    test_uint(root, ecs_field_src(&it, 2));
    test_uint(Bar, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(ecs_id(Velocity), ecs_field_id(&it, 2));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_int(p->x, 10); test_int(p->y, 20);
        Velocity *v = ecs_field(&it, Velocity, 2);
        test_int(v->x, 1); test_int(v->y, 2);
    }

    test_bool(false, ecs_query_next(&it));
    
    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_up_childof_pair(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t root_a = ecs_new(world);
    ecs_entity_t root_b = ecs_new(world);

    ecs_entity_t parent_a = ecs_new_w_pair(world, EcsChildOf, root_a);
    ecs_entity_t parent_b = ecs_insert(world, ecs_value(EcsParent, {root_a}));

    ecs_entity_t parent_c = ecs_new_w_pair(world, EcsChildOf, root_b);
    ecs_entity_t parent_d = ecs_insert(world, ecs_value(EcsParent, {root_b}));

    ecs_entity_t c1 = ecs_insert(world, ecs_value(EcsParent, {parent_a}));
    ecs_entity_t c2 = ecs_new_w_pair(world, EcsChildOf, parent_a);
    ecs_entity_t c3 = ecs_insert(world, ecs_value(EcsParent, {parent_b}));
    ecs_entity_t c4 = ecs_new_w_pair(world, EcsChildOf, parent_b);
    ecs_entity_t c5 = ecs_insert(world, ecs_value(EcsParent, {parent_c}));
    ecs_entity_t c6 = ecs_new_w_pair(world, EcsChildOf, parent_c);
    ecs_entity_t c7 = ecs_insert(world, ecs_value(EcsParent, {parent_d}));
    ecs_entity_t c8 = ecs_new_w_pair(world, EcsChildOf, parent_d);

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {{ ecs_childof(root_a), .src.id = EcsUp }},
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c1, it.entities[0]);
        test_uint(parent_a, ecs_field_src(&it, 0));
        test_uint(ecs_childof(root_a), ecs_field_id(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c2, it.entities[0]);
        test_uint(parent_a, ecs_field_src(&it, 0));
        test_uint(ecs_childof(root_a), ecs_field_id(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c3, it.entities[0]);
        test_uint(parent_b, ecs_field_src(&it, 0));
        test_uint(ecs_childof(root_a), ecs_field_id(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c4, it.entities[0]);
        test_uint(parent_b, ecs_field_src(&it, 0));
        test_uint(ecs_childof(root_a), ecs_field_id(&it, 0));

        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {{ ecs_childof(root_b), .src.id = EcsUp }},
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c5, it.entities[0]);
        test_uint(parent_c, ecs_field_src(&it, 0));
        test_uint(ecs_childof(root_b), ecs_field_id(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c6, it.entities[0]);
        test_uint(parent_c, ecs_field_src(&it, 0));
        test_uint(ecs_childof(root_b), ecs_field_id(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c7, it.entities[0]);
        test_uint(parent_d, ecs_field_src(&it, 0));
        test_uint(ecs_childof(root_b), ecs_field_id(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c8, it.entities[0]);
        test_uint(parent_d, ecs_field_src(&it, 0));
        test_uint(ecs_childof(root_b), ecs_field_id(&it, 0));

        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {{ ecs_childof(parent_a), .src.id = EcsUp }},
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {{ ecs_childof(parent_b), .src.id = EcsUp }},
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {{ ecs_childof(parent_c), .src.id = EcsUp }},
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {{ ecs_childof(parent_d), .src.id = EcsUp }},
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    ecs_fini(world);
}

void NonFragmentingChildOf_this_self_up_childof_pair(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t root_a = ecs_new(world);
    ecs_entity_t root_b = ecs_new(world);

    ecs_entity_t parent_a = ecs_new_w_pair(world, EcsChildOf, root_a);
    ecs_entity_t parent_b = ecs_insert(world, ecs_value(EcsParent, {root_a}));

    ecs_entity_t parent_c = ecs_new_w_pair(world, EcsChildOf, root_b);
    ecs_entity_t parent_d = ecs_insert(world, ecs_value(EcsParent, {root_b}));

    ecs_entity_t c1 = ecs_insert(world, ecs_value(EcsParent, {parent_a}));
    ecs_entity_t c2 = ecs_new_w_pair(world, EcsChildOf, parent_a);
    ecs_entity_t c3 = ecs_insert(world, ecs_value(EcsParent, {parent_b}));
    ecs_entity_t c4 = ecs_new_w_pair(world, EcsChildOf, parent_b);
    ecs_entity_t c5 = ecs_insert(world, ecs_value(EcsParent, {parent_c}));
    ecs_entity_t c6 = ecs_new_w_pair(world, EcsChildOf, parent_c);
    ecs_entity_t c7 = ecs_insert(world, ecs_value(EcsParent, {parent_d}));
    ecs_entity_t c8 = ecs_new_w_pair(world, EcsChildOf, parent_d);

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {{ ecs_childof(root_a), .src.id = EcsSelf | EcsUp }},
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(parent_a, it.entities[0]);
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(ecs_childof(root_a), ecs_field_id(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c1, it.entities[0]);
        test_uint(parent_a, ecs_field_src(&it, 0));
        test_uint(ecs_childof(root_a), ecs_field_id(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c2, it.entities[0]);
        test_uint(parent_a, ecs_field_src(&it, 0));
        test_uint(ecs_childof(root_a), ecs_field_id(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(parent_b, it.entities[0]);
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(ecs_childof(root_a), ecs_field_id(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c3, it.entities[0]);
        test_uint(parent_b, ecs_field_src(&it, 0));
        test_uint(ecs_childof(root_a), ecs_field_id(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c4, it.entities[0]);
        test_uint(parent_b, ecs_field_src(&it, 0));
        test_uint(ecs_childof(root_a), ecs_field_id(&it, 0));

        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {{ ecs_childof(root_b), .src.id = EcsSelf | EcsUp }},
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(parent_c, it.entities[0]);
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(ecs_childof(root_b), ecs_field_id(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c5, it.entities[0]);
        test_uint(parent_c, ecs_field_src(&it, 0));
        test_uint(ecs_childof(root_b), ecs_field_id(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c6, it.entities[0]);
        test_uint(parent_c, ecs_field_src(&it, 0));
        test_uint(ecs_childof(root_b), ecs_field_id(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(parent_d, it.entities[0]);
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(ecs_childof(root_b), ecs_field_id(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c7, it.entities[0]);
        test_uint(parent_d, ecs_field_src(&it, 0));
        test_uint(ecs_childof(root_b), ecs_field_id(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c8, it.entities[0]);
        test_uint(parent_d, ecs_field_src(&it, 0));
        test_uint(ecs_childof(root_b), ecs_field_id(&it, 0));

        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {{ ecs_childof(parent_a), .src.id = EcsSelf | EcsUp }},
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c1, it.entities[0]);
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(ecs_childof(parent_a), ecs_field_id(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c2, it.entities[0]);
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(ecs_childof(parent_a), ecs_field_id(&it, 0));

        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {{ ecs_childof(parent_b), .src.id = EcsSelf | EcsUp }},
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c3, it.entities[0]);
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(ecs_childof(parent_b), ecs_field_id(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c4, it.entities[0]);
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(ecs_childof(parent_b), ecs_field_id(&it, 0));

        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {{ ecs_childof(parent_c), .src.id = EcsSelf | EcsUp }},
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c5, it.entities[0]);
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(ecs_childof(parent_c), ecs_field_id(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c6, it.entities[0]);
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(ecs_childof(parent_c), ecs_field_id(&it, 0));

        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {{ ecs_childof(parent_d), .src.id = EcsSelf | EcsUp }},
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c7, it.entities[0]);
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(ecs_childof(parent_d), ecs_field_id(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c8, it.entities[0]);
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(ecs_childof(parent_d), ecs_field_id(&it, 0));

        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    ecs_fini(world);
}

void NonFragmentingChildOf_this_written_up_childof_pair(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_entity_t root_a = ecs_new(world);
    ecs_entity_t root_b = ecs_new(world);

    ecs_entity_t parent_a = ecs_new_w_pair(world, EcsChildOf, root_a);
    ecs_entity_t parent_b = ecs_insert(world, ecs_value(EcsParent, {root_a}));

    ecs_entity_t parent_c = ecs_new_w_pair(world, EcsChildOf, root_b);
    ecs_entity_t parent_d = ecs_insert(world, ecs_value(EcsParent, {root_b}));

    ecs_entity_t c1 = ecs_insert(world, ecs_value(EcsParent, {parent_a}));
    ecs_entity_t c2 = ecs_new_w_pair(world, EcsChildOf, parent_a);
    ecs_entity_t c3 = ecs_insert(world, ecs_value(EcsParent, {parent_b}));
    ecs_entity_t c4 = ecs_new_w_pair(world, EcsChildOf, parent_b);
    ecs_entity_t c5 = ecs_insert(world, ecs_value(EcsParent, {parent_c}));
    ecs_entity_t c6 = ecs_new_w_pair(world, EcsChildOf, parent_c);
    ecs_entity_t c7 = ecs_insert(world, ecs_value(EcsParent, {parent_d}));
    ecs_entity_t c8 = ecs_new_w_pair(world, EcsChildOf, parent_d);

    ecs_add(world, root_a, Foo);
    ecs_add(world, root_b, Foo);

    ecs_add(world, parent_a, Foo);
    ecs_add(world, parent_b, Foo);

    ecs_add(world, c1, Foo);
    ecs_add(world, c2, Foo);
    ecs_add(world, c3, Foo);
    ecs_add(world, c4, Foo);
    ecs_add(world, c5, Foo);
    ecs_add(world, c6, Foo);
    ecs_add(world, c7, Foo);
    ecs_add(world, c8, Foo);

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {{ Foo }, { ecs_childof(root_a), .src.id = EcsUp }},
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c1, it.entities[0]);
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(parent_a, ecs_field_src(&it, 1));
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_childof(root_a), ecs_field_id(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c3, it.entities[0]);
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(parent_b, ecs_field_src(&it, 1));
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_childof(root_a), ecs_field_id(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c2, it.entities[0]);
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(parent_a, ecs_field_src(&it, 1));
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_childof(root_a), ecs_field_id(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c4, it.entities[0]);
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(parent_b, ecs_field_src(&it, 1));
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_childof(root_a), ecs_field_id(&it, 1));

        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {{ Foo }, { ecs_childof(root_b), .src.id = EcsUp }},
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c5, it.entities[0]);
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(parent_c, ecs_field_src(&it, 1));
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_childof(root_b), ecs_field_id(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c7, it.entities[0]);
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(parent_d, ecs_field_src(&it, 1));
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_childof(root_b), ecs_field_id(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c6, it.entities[0]);
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(parent_c, ecs_field_src(&it, 1));
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_childof(root_b), ecs_field_id(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c8, it.entities[0]);
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(parent_d, ecs_field_src(&it, 1));
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_childof(root_b), ecs_field_id(&it, 1));

        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {{ ecs_childof(parent_a), .src.id = EcsUp }},
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {{ ecs_childof(parent_b), .src.id = EcsUp }},
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {{ ecs_childof(parent_c), .src.id = EcsUp }},
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {{ ecs_childof(parent_d), .src.id = EcsUp }},
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    ecs_fini(world);
}

void NonFragmentingChildOf_this_written_self_up_childof_pair(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_entity_t root_a = ecs_new(world);
    ecs_entity_t root_b = ecs_new(world);

    ecs_entity_t parent_a = ecs_new_w_pair(world, EcsChildOf, root_a);
    ecs_entity_t parent_b = ecs_insert(world, ecs_value(EcsParent, {root_a}));

    ecs_entity_t parent_c = ecs_new_w_pair(world, EcsChildOf, root_b);
    ecs_entity_t parent_d = ecs_insert(world, ecs_value(EcsParent, {root_b}));

    ecs_entity_t c1 = ecs_insert(world, ecs_value(EcsParent, {parent_a}));
    ecs_entity_t c2 = ecs_new_w_pair(world, EcsChildOf, parent_a);
    ecs_entity_t c3 = ecs_insert(world, ecs_value(EcsParent, {parent_b}));
    ecs_entity_t c4 = ecs_new_w_pair(world, EcsChildOf, parent_b);
    ecs_entity_t c5 = ecs_insert(world, ecs_value(EcsParent, {parent_c}));
    ecs_entity_t c6 = ecs_new_w_pair(world, EcsChildOf, parent_c);
    ecs_entity_t c7 = ecs_insert(world, ecs_value(EcsParent, {parent_d}));
    ecs_entity_t c8 = ecs_new_w_pair(world, EcsChildOf, parent_d);

    ecs_add(world, root_a, Foo);
    ecs_add(world, root_b, Foo);

    ecs_add(world, parent_a, Foo);
    ecs_add(world, parent_b, Foo);

    ecs_add(world, c1, Foo);
    ecs_add(world, c2, Foo);
    ecs_add(world, c3, Foo);
    ecs_add(world, c4, Foo);
    ecs_add(world, c5, Foo);
    ecs_add(world, c6, Foo);
    ecs_add(world, c7, Foo);
    ecs_add(world, c8, Foo);

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {{ Foo }, { ecs_childof(root_a), .src.id = EcsSelf | EcsUp }},
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(parent_a, it.entities[0]);
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_childof(root_a), ecs_field_id(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(parent_b, it.entities[0]);
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_childof(root_a), ecs_field_id(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c1, it.entities[0]);
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(parent_a, ecs_field_src(&it, 1));
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_childof(root_a), ecs_field_id(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c3, it.entities[0]);
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(parent_b, ecs_field_src(&it, 1));
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_childof(root_a), ecs_field_id(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c2, it.entities[0]);
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(parent_a, ecs_field_src(&it, 1));
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_childof(root_a), ecs_field_id(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c4, it.entities[0]);
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(parent_b, ecs_field_src(&it, 1));
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_childof(root_a), ecs_field_id(&it, 1));

        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {{ Foo }, { ecs_childof(root_b), .src.id = EcsUp }},
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c5, it.entities[0]);
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(parent_c, ecs_field_src(&it, 1));
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_childof(root_b), ecs_field_id(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c7, it.entities[0]);
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(parent_d, ecs_field_src(&it, 1));
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_childof(root_b), ecs_field_id(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c6, it.entities[0]);
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(parent_c, ecs_field_src(&it, 1));
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_childof(root_b), ecs_field_id(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c8, it.entities[0]);
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(parent_d, ecs_field_src(&it, 1));
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_childof(root_b), ecs_field_id(&it, 1));

        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {{ ecs_childof(parent_a), .src.id = EcsUp }},
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {{ ecs_childof(parent_b), .src.id = EcsUp }},
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {{ ecs_childof(parent_c), .src.id = EcsUp }},
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {{ ecs_childof(parent_d), .src.id = EcsUp }},
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    ecs_fini(world);
}

void NonFragmentingChildOf_this_childof_w_disabled(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t p1 = ecs_new(world);
    ecs_entity_t p2 = ecs_new(world);
    ecs_entity_t c1 = ecs_insert(world, ecs_value(EcsParent, {p1}));
    ecs_entity_t c2 = ecs_insert(world, ecs_value(EcsParent, {p1}));
    ecs_entity_t c3 = ecs_insert(world, ecs_value(EcsParent, {p1}));
    ecs_insert(world, ecs_value(EcsParent, {p2}));

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ ecs_childof(p1) }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    {
        ecs_enable(world, c1, false);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c2, it.entities[0]);
        test_uint(ecs_childof(p1), ecs_field_id(&it, 0));
        test_uint(0, ecs_field_src(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c3, it.entities[0]);
        test_uint(ecs_childof(p1), ecs_field_id(&it, 0));
        test_uint(0, ecs_field_src(&it, 0));

        test_bool(false, ecs_query_next(&it));
    }

    {
        ecs_enable(world, c1, true);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(3, it.count);
        test_uint(c1, it.entities[0]);
        test_uint(c2, it.entities[1]);
        test_uint(c3, it.entities[2]);
        test_uint(ecs_childof(p1), ecs_field_id(&it, 0));
        test_uint(0, ecs_field_src(&it, 0));

        test_bool(false, ecs_query_next(&it));
    }

    {
        ecs_enable(world, c2, false);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c1, it.entities[0]);
        test_uint(ecs_childof(p1), ecs_field_id(&it, 0));
        test_uint(0, ecs_field_src(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c3, it.entities[0]);
        test_uint(ecs_childof(p1), ecs_field_id(&it, 0));
        test_uint(0, ecs_field_src(&it, 0));

        test_bool(false, ecs_query_next(&it));
    }

    {
        ecs_enable(world, c1, false);
        ecs_enable(world, c3, false);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_childof_w_disabled_match_disabled(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t p1 = ecs_new(world);
    ecs_entity_t p2 = ecs_new(world);
    ecs_entity_t c1 = ecs_insert(world, ecs_value(EcsParent, {p1}));
    ecs_entity_t c2 = ecs_insert(world, ecs_value(EcsParent, {p1}));
    ecs_entity_t c3 = ecs_insert(world, ecs_value(EcsParent, {p1}));
    ecs_insert(world, ecs_value(EcsParent, {p2}));

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ ecs_childof(p1) }},
        .flags = EcsQueryMatchDisabled,
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(3, it.count);
        test_uint(c1, it.entities[0]);
        test_uint(c2, it.entities[1]);
        test_uint(c3, it.entities[2]);
        test_uint(ecs_childof(p1), ecs_field_id(&it, 0));
        test_uint(0, ecs_field_src(&it, 0));

        test_bool(false, ecs_query_next(&it));
    }

    {
        ecs_enable(world, c2, false);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(3, it.count);
        test_uint(c1, it.entities[0]);
        test_uint(c2, it.entities[1]);
        test_uint(c3, it.entities[2]);
        test_uint(ecs_childof(p1), ecs_field_id(&it, 0));
        test_uint(0, ecs_field_src(&it, 0));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_set_childof_w_disabled(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t p1 = ecs_new(world);
    ecs_entity_t p2 = ecs_new(world);
    ecs_entity_t c1 = ecs_insert(world, ecs_value(EcsParent, {p1}));
    ecs_entity_t c2 = ecs_insert(world, ecs_value(EcsParent, {p1}));
    ecs_entity_t c3 = ecs_insert(world, ecs_value(EcsParent, {p1}));
    ecs_insert(world, ecs_value(EcsParent, {p2}));

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ ecs_childof(p1) }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    {
        ecs_enable(world, c1, false);

        {
            ecs_iter_t it = ecs_query_iter(world, q);
            ecs_iter_set_var(&it, 0, c1);
            test_bool(false, ecs_query_next(&it));
        }

        {
            ecs_iter_t it = ecs_query_iter(world, q);
            ecs_iter_set_var(&it, 0, c2);
            test_bool(true, ecs_query_next(&it));
            test_int(1, it.count);
            test_uint(c2, it.entities[0]);
            test_uint(ecs_childof(p1), ecs_field_id(&it, 0));
            test_uint(0, ecs_field_src(&it, 0));
            test_bool(false, ecs_query_next(&it));
        }
        
        {
            ecs_iter_t it = ecs_query_iter(world, q);
            ecs_iter_set_var(&it, 0, c3);
            test_bool(true, ecs_query_next(&it));
            test_int(1, it.count);
            test_uint(c3, it.entities[0]);
            test_uint(ecs_childof(p1), ecs_field_id(&it, 0));
            test_uint(0, ecs_field_src(&it, 0));
            test_bool(false, ecs_query_next(&it));
        }
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_set_childof_w_disabled_match_disabled(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t p1 = ecs_new(world);
    ecs_entity_t p2 = ecs_new(world);
    ecs_entity_t c1 = ecs_insert(world, ecs_value(EcsParent, {p1}));
    ecs_entity_t c2 = ecs_insert(world, ecs_value(EcsParent, {p1}));
    ecs_entity_t c3 = ecs_insert(world, ecs_value(EcsParent, {p1}));
    ecs_insert(world, ecs_value(EcsParent, {p2}));

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ ecs_childof(p1) }},
        .flags = EcsQueryMatchDisabled,
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    {
        ecs_enable(world, c1, false);

        {
            ecs_iter_t it = ecs_query_iter(world, q);
            ecs_iter_set_var(&it, 0, c1);
            test_bool(true, ecs_query_next(&it));
            test_int(1, it.count);
            test_uint(c1, it.entities[0]);
            test_uint(ecs_childof(p1), ecs_field_id(&it, 0));
            test_uint(0, ecs_field_src(&it, 0));
            test_bool(false, ecs_query_next(&it));
        }

        {
            ecs_iter_t it = ecs_query_iter(world, q);
            ecs_iter_set_var(&it, 0, c2);
            test_bool(true, ecs_query_next(&it));
            test_int(1, it.count);
            test_uint(c2, it.entities[0]);
            test_uint(ecs_childof(p1), ecs_field_id(&it, 0));
            test_uint(0, ecs_field_src(&it, 0));
            test_bool(false, ecs_query_next(&it));
        }

        {
            ecs_iter_t it = ecs_query_iter(world, q);
            ecs_iter_set_var(&it, 0, c3);
            test_bool(true, ecs_query_next(&it));
            test_int(1, it.count);
            test_uint(c3, it.entities[0]);
            test_uint(ecs_childof(p1), ecs_field_id(&it, 0));
            test_uint(0, ecs_field_src(&it, 0));
            test_bool(false, ecs_query_next(&it));
        }
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_childof_w_prefab(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t p1 = ecs_new(world);
    ecs_entity_t p2 = ecs_new(world);
    ecs_entity_t c1 = ecs_insert(world, ecs_value(EcsParent, {p1}));
    ecs_entity_t c2 = ecs_insert(world, ecs_value(EcsParent, {p1}));
    ecs_entity_t c3 = ecs_insert(world, ecs_value(EcsParent, {p1}));
    ecs_insert(world, ecs_value(EcsParent, {p2}));

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ ecs_childof(p1) }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    {
        ecs_add_id(world, c1, EcsPrefab);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c2, it.entities[0]);
        test_uint(ecs_childof(p1), ecs_field_id(&it, 0));
        test_uint(0, ecs_field_src(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c3, it.entities[0]);
        test_uint(ecs_childof(p1), ecs_field_id(&it, 0));
        test_uint(0, ecs_field_src(&it, 0));

        test_bool(false, ecs_query_next(&it));
    }

    {
        ecs_remove_id(world, c1, EcsPrefab);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(3, it.count);
        test_uint(c1, it.entities[0]);
        test_uint(c2, it.entities[1]);
        test_uint(c3, it.entities[2]);
        test_uint(ecs_childof(p1), ecs_field_id(&it, 0));
        test_uint(0, ecs_field_src(&it, 0));

        test_bool(false, ecs_query_next(&it));
    }

    {
        ecs_add_id(world, c2, EcsPrefab);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c1, it.entities[0]);
        test_uint(ecs_childof(p1), ecs_field_id(&it, 0));
        test_uint(0, ecs_field_src(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c3, it.entities[0]);
        test_uint(ecs_childof(p1), ecs_field_id(&it, 0));
        test_uint(0, ecs_field_src(&it, 0));

        test_bool(false, ecs_query_next(&it));
    }

    {
        ecs_add_id(world, c1, EcsPrefab);
        ecs_add_id(world, c3, EcsPrefab);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_childof_w_prefab_match_prefab(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t p1 = ecs_new(world);
    ecs_entity_t p2 = ecs_new(world);
    ecs_entity_t c1 = ecs_insert(world, ecs_value(EcsParent, {p1}));
    ecs_entity_t c2 = ecs_insert(world, ecs_value(EcsParent, {p1}));
    ecs_entity_t c3 = ecs_insert(world, ecs_value(EcsParent, {p1}));
    ecs_insert(world, ecs_value(EcsParent, {p2}));

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ ecs_childof(p1) }},
        .flags = EcsQueryMatchPrefab,
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(3, it.count);
        test_uint(c1, it.entities[0]);
        test_uint(c2, it.entities[1]);
        test_uint(c3, it.entities[2]);
        test_uint(ecs_childof(p1), ecs_field_id(&it, 0));
        test_uint(0, ecs_field_src(&it, 0));

        test_bool(false, ecs_query_next(&it));
    }

    {
        ecs_add_id(world, c2, EcsPrefab);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(3, it.count);
        test_uint(c1, it.entities[0]);
        test_uint(c2, it.entities[1]);
        test_uint(c3, it.entities[2]);
        test_uint(ecs_childof(p1), ecs_field_id(&it, 0));
        test_uint(0, ecs_field_src(&it, 0));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_set_childof_w_prefab(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t p1 = ecs_new(world);
    ecs_entity_t p2 = ecs_new(world);
    ecs_entity_t c1 = ecs_insert(world, ecs_value(EcsParent, {p1}));
    ecs_entity_t c2 = ecs_insert(world, ecs_value(EcsParent, {p1}));
    ecs_entity_t c3 = ecs_insert(world, ecs_value(EcsParent, {p1}));
    ecs_insert(world, ecs_value(EcsParent, {p2}));

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ ecs_childof(p1) }},
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    {
        ecs_add_id(world, c1, EcsPrefab);

        {
            ecs_iter_t it = ecs_query_iter(world, q);
            ecs_iter_set_var(&it, 0, c1);
            test_bool(false, ecs_query_next(&it));
        }

        {
            ecs_iter_t it = ecs_query_iter(world, q);
            ecs_iter_set_var(&it, 0, c2);
            test_bool(true, ecs_query_next(&it));
            test_int(1, it.count);
            test_uint(c2, it.entities[0]);
            test_uint(ecs_childof(p1), ecs_field_id(&it, 0));
            test_uint(0, ecs_field_src(&it, 0));
            test_bool(false, ecs_query_next(&it));
        }
        
        {
            ecs_iter_t it = ecs_query_iter(world, q);
            ecs_iter_set_var(&it, 0, c3);
            test_bool(true, ecs_query_next(&it));
            test_int(1, it.count);
            test_uint(c3, it.entities[0]);
            test_uint(ecs_childof(p1), ecs_field_id(&it, 0));
            test_uint(0, ecs_field_src(&it, 0));
            test_bool(false, ecs_query_next(&it));
        }
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_set_childof_w_prefab_match_prefab(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t p1 = ecs_new(world);
    ecs_entity_t p2 = ecs_new(world);
    ecs_entity_t c1 = ecs_insert(world, ecs_value(EcsParent, {p1}));
    ecs_entity_t c2 = ecs_insert(world, ecs_value(EcsParent, {p1}));
    ecs_entity_t c3 = ecs_insert(world, ecs_value(EcsParent, {p1}));
    ecs_insert(world, ecs_value(EcsParent, {p2}));

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ ecs_childof(p1) }},
        .flags = EcsQueryMatchPrefab,
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    {
        ecs_add_id(world, c1, EcsPrefab);

        {
            ecs_iter_t it = ecs_query_iter(world, q);
            ecs_iter_set_var(&it, 0, c1);
            test_bool(true, ecs_query_next(&it));
            test_int(1, it.count);
            test_uint(c1, it.entities[0]);
            test_uint(ecs_childof(p1), ecs_field_id(&it, 0));
            test_uint(0, ecs_field_src(&it, 0));
            test_bool(false, ecs_query_next(&it));
        }

        {
            ecs_iter_t it = ecs_query_iter(world, q);
            ecs_iter_set_var(&it, 0, c2);
            test_bool(true, ecs_query_next(&it));
            test_int(1, it.count);
            test_uint(c2, it.entities[0]);
            test_uint(ecs_childof(p1), ecs_field_id(&it, 0));
            test_uint(0, ecs_field_src(&it, 0));
            test_bool(false, ecs_query_next(&it));
        }

        {
            ecs_iter_t it = ecs_query_iter(world, q);
            ecs_iter_set_var(&it, 0, c3);
            test_bool(true, ecs_query_next(&it));
            test_int(1, it.count);
            test_uint(c3, it.entities[0]);
            test_uint(ecs_childof(p1), ecs_field_id(&it, 0));
            test_uint(0, ecs_field_src(&it, 0));
            test_bool(false, ecs_query_next(&it));
        }
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_up_optional(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_COMPONENT(world, Position);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { Foo },
            { ecs_id(Position), .src.id = EcsUp, .oper = EcsOptional }
        },
        .cache_kind = cache_kind
    });
    
    test_assert(q != NULL);

    ecs_entity_t p1 = ecs_new(world);
    ecs_set(world, p1, Position, {10, 20});
    ecs_entity_t p2 = ecs_new(world);

    ecs_entity_t c1 = ecs_new(world);
    ecs_entity_t c2 = ecs_new_w_parent(world, p1, NULL);
    ecs_entity_t c3 = ecs_new_w_parent(world, p2, NULL);

    ecs_add(world, c1, Foo);
    ecs_add(world, c2, Foo);
    ecs_add(world, c3, Foo);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(c1, it.entities[0]);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(0, ecs_field_src(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(false, ecs_field_is_set(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(c2, it.entities[0]);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(p1, ecs_field_src(&it, 1));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(c3, it.entities[0]);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(0, ecs_field_src(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(false, ecs_field_is_set(&it, 1));

    test_bool(false, ecs_query_next(&it)); 

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_up_optional_2(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_COMPONENT(world, Position);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { Foo },
            { ecs_id(Position), .src.id = EcsUp, .oper = EcsOptional }
        },
        .cache_kind = cache_kind
    });
    
    test_assert(q != NULL);

    ecs_entity_t p1 = ecs_new(world);
    ecs_set(world, p1, Position, {10, 20});
    ecs_entity_t p2 = ecs_new(world);
    ecs_set(world, p2, Position, {30, 40});
    ecs_entity_t p3 = ecs_new(world);

    ecs_entity_t c1 = ecs_new(world);
    ecs_entity_t c2 = ecs_new_w_parent(world, p1, NULL);
    ecs_entity_t c3 = ecs_new_w_parent(world, p2, NULL);
    ecs_entity_t c4 = ecs_new_w_parent(world, p3, NULL);

    ecs_add(world, c1, Foo);
    ecs_add(world, c2, Foo);
    ecs_add(world, c3, Foo);
    ecs_add(world, c4, Foo);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(c1, it.entities[0]);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(0, ecs_field_src(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(false, ecs_field_is_set(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(c2, it.entities[0]);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(p1, ecs_field_src(&it, 1));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(c3, it.entities[0]);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(p2, ecs_field_src(&it, 1));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_assert(p != NULL);
        test_int(p->x, 30);
        test_int(p->y, 40);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(c4, it.entities[0]);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(0, ecs_field_src(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(false, ecs_field_is_set(&it, 1));

    test_bool(false, ecs_query_next(&it)); 

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_up_optional_3(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);
    ECS_COMPONENT(world, Position);

    ecs_entity_t p1 = ecs_new(world);
    ecs_set(world, p1, Position, {10, 20});
    ecs_entity_t p2 = ecs_new(world);
    ecs_set(world, p2, Position, {30, 40});
    ecs_entity_t p3 = ecs_new(world);

    ecs_entity_t c1 = ecs_new(world);
    ecs_entity_t c2 = ecs_new_w_parent(world, p1, NULL);
    ecs_entity_t c3 = ecs_new_w_parent(world, p2, NULL);
    ecs_entity_t c4 = ecs_new_w_parent(world, p3, NULL);
    ecs_entity_t c5 = ecs_new(world);

    ecs_add(world, c1, Foo);
    ecs_add(world, c2, Foo);
    ecs_add(world, c3, Foo);
    ecs_add(world, c4, Foo);
    ecs_add(world, c5, Foo);
    ecs_add(world, c5, Bar);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { Foo },
            { ecs_id(Position), .src.id = EcsUp, .oper = EcsOptional }
        },
        .cache_kind = cache_kind
    });
    
    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(c1, it.entities[0]);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(0, ecs_field_src(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(false, ecs_field_is_set(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(c2, it.entities[0]);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(p1, ecs_field_src(&it, 1));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(c3, it.entities[0]);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(p2, ecs_field_src(&it, 1));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_assert(p != NULL);
        test_int(p->x, 30);
        test_int(p->y, 40);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(c4, it.entities[0]);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(0, ecs_field_src(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(false, ecs_field_is_set(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(c5, it.entities[0]);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(0, ecs_field_src(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(false, ecs_field_is_set(&it, 1));

    test_bool(false, ecs_query_next(&it)); 

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_self_up_optional(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_COMPONENT(world, Position);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { Foo },
            { ecs_id(Position), .src.id = EcsSelf | EcsUp, .oper = EcsOptional }
        },
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_entity_t p1 = ecs_new(world);
    ecs_set(world, p1, Position, {10, 20});
    ecs_entity_t p2 = ecs_new(world);

    ecs_entity_t c1 = ecs_new(world);
    ecs_entity_t c2 = ecs_new_w_parent(world, p1, NULL);
    ecs_entity_t c3 = ecs_new_w_parent(world, p2, NULL);

    ecs_add(world, c1, Foo);
    ecs_add(world, c2, Foo);
    ecs_add(world, c3, Foo);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(c1, it.entities[0]);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(0, ecs_field_src(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(false, ecs_field_is_set(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(c2, it.entities[0]);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(p1, ecs_field_src(&it, 1));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(c3, it.entities[0]);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(0, ecs_field_src(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(false, ecs_field_is_set(&it, 1));

    test_bool(false, ecs_query_next(&it)); 

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_self_up_optional_2(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_COMPONENT(world, Position);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { Foo },
            { ecs_id(Position), .src.id = EcsSelf | EcsUp, .oper = EcsOptional }
        },
        .cache_kind = cache_kind
    });
    
    test_assert(q != NULL);

    ecs_entity_t p1 = ecs_new(world);
    ecs_set(world, p1, Position, {10, 20});
    ecs_entity_t p2 = ecs_new(world);
    ecs_set(world, p2, Position, {30, 40});
    ecs_entity_t p3 = ecs_new(world);

    ecs_entity_t c1 = ecs_new(world);
    ecs_entity_t c2 = ecs_new_w_parent(world, p1, NULL);
    ecs_entity_t c3 = ecs_new_w_parent(world, p2, NULL);
    ecs_entity_t c4 = ecs_new_w_parent(world, p3, NULL);

    ecs_add(world, c1, Foo);
    ecs_add(world, c2, Foo);
    ecs_add(world, c3, Foo);
    ecs_add(world, c4, Foo);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(c1, it.entities[0]);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(0, ecs_field_src(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(false, ecs_field_is_set(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(c2, it.entities[0]);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(p1, ecs_field_src(&it, 1));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(c3, it.entities[0]);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(p2, ecs_field_src(&it, 1));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_assert(p != NULL);
        test_int(p->x, 30);
        test_int(p->y, 40);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(c4, it.entities[0]);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(0, ecs_field_src(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(false, ecs_field_is_set(&it, 1));

    test_bool(false, ecs_query_next(&it)); 

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_self_up_optional_3(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_COMPONENT(world, Position);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { Foo },
            { ecs_id(Position), .src.id = EcsSelf | EcsUp, .oper = EcsOptional }
        },
        .cache_kind = cache_kind
    });
    
    test_assert(q != NULL);

    ecs_entity_t p1 = ecs_new(world);
    ecs_set(world, p1, Position, {10, 20});
    ecs_entity_t p2 = ecs_new(world);
    ecs_set(world, p2, Position, {30, 40});
    ecs_entity_t p3 = ecs_new(world);

    ecs_entity_t c1 = ecs_new(world);
    ecs_entity_t c2 = ecs_new_w_parent(world, p1, NULL);
    ecs_entity_t c3 = ecs_new_w_parent(world, p2, NULL);
    ecs_entity_t c4 = ecs_new_w_parent(world, p2, NULL);
    ecs_entity_t c5 = ecs_new_w_parent(world, p3, NULL);

    ecs_add(world, c1, Foo);
    ecs_add(world, c2, Foo);
    ecs_add(world, c3, Foo);
    ecs_add(world, c4, Foo);
    ecs_add(world, c5, Foo);

    ecs_set(world, c4, Position, {50, 60});

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(c1, it.entities[0]);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(0, ecs_field_src(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(false, ecs_field_is_set(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(c2, it.entities[0]);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(p1, ecs_field_src(&it, 1));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(c3, it.entities[0]);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(p2, ecs_field_src(&it, 1));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_assert(p != NULL);
        test_int(p->x, 30);
        test_int(p->y, 40);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(c5, it.entities[0]);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(0, ecs_field_src(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(false, ecs_field_is_set(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(c4, it.entities[0]);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(0, ecs_field_src(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_assert(p != NULL);
        test_int(p->x, 50);
        test_int(p->y, 60);
    }

    test_bool(false, ecs_query_next(&it)); 

    ecs_query_fini(q);

    ecs_fini(world);
}
