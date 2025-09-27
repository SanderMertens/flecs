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

    ecs_entity_t p = ecs_new_w_id(world, EcsOrderedChildren);
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

    ecs_entity_t p = ecs_new_w_id(world, EcsOrderedChildren);
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

    ecs_entity_t p_1 = ecs_new_w_id(world, EcsOrderedChildren);
    ecs_entity_t p_2 = ecs_new_w_id(world, EcsOrderedChildren);
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

    ecs_entity_t p_1 = ecs_new_w_id(world, EcsOrderedChildren);
    ecs_entity_t p_2 = ecs_new_w_id(world, EcsOrderedChildren);
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

    ecs_entity_t p_1 = ecs_new_w_id(world, EcsOrderedChildren);
    ecs_entity_t p_2 = ecs_new_w_id(world, EcsOrderedChildren);
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

    ecs_entity_t p_1 = ecs_new_w_id(world, EcsOrderedChildren);
    ecs_entity_t p_2 = ecs_new_w_id(world, EcsOrderedChildren);
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

    ecs_entity_t p_1 = ecs_new_w_id(world, EcsOrderedChildren);
    ecs_entity_t p_2 = ecs_new_w_id(world, EcsOrderedChildren);
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

    ecs_entity_t p_1 = ecs_new_w_id(world, EcsOrderedChildren);
    ecs_entity_t p_2 = ecs_new_w_id(world, EcsOrderedChildren);
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

    ecs_entity_t p_1 = ecs_new_w_id(world, EcsOrderedChildren);
    ecs_entity_t p_2 = ecs_new_w_id(world, EcsOrderedChildren);
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

    ecs_entity_t p_1 = ecs_new_w_id(world, EcsOrderedChildren);
    ecs_entity_t p_2 = ecs_new_w_id(world, EcsOrderedChildren);
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

    ecs_entity_t p_1 = ecs_new_w_id(world, EcsOrderedChildren);
    ecs_entity_t p_2 = ecs_new_w_id(world, EcsOrderedChildren);
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

    ecs_entity_t p = ecs_new_w_id(world, EcsOrderedChildren);
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
    test_int(1, it.count);
    test_uint(c3, it.entities[0]);
    test_uint(ecs_childof(0), ecs_field_id(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(EcsFlecs, it.entities[0]);
    test_uint(ecs_childof(0), ecs_field_id(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(p, it.entities[0]);
    test_uint(ecs_childof(0), ecs_field_id(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 0));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_src_not_childof_any(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t p = ecs_new_w_id(world, EcsOrderedChildren);
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
    test_int(1, it.count);
    test_uint(c3, it.entities[0]);
    test_uint(ecs_childof(0), ecs_field_id(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(EcsFlecs, it.entities[0]);
    test_uint(ecs_childof(0), ecs_field_id(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(p, it.entities[0]);
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

    ecs_entity_t p1 = ecs_new_w_id(world, EcsOrderedChildren);
    ecs_entity_t p2 = ecs_new_w_id(world, EcsOrderedChildren);
    ecs_entity_t p3 = ecs_new(world);

    ecs_entity_t c1 = ecs_insert(world, ecs_value(EcsParent, {p1}));
    ecs_entity_t c2 = ecs_insert(world, ecs_value(EcsParent, {p1}));
    ecs_entity_t c3 = ecs_insert(world, ecs_value(EcsParent, {p1}));
    ecs_entity_t c4 = ecs_insert(world, ecs_value(EcsParent, {p1}));
    ecs_entity_t c5 = ecs_new_w_pair(world, EcsChildOf, p1);

    ecs_entity_t c6 = ecs_new_w_pair(world, EcsChildOf, p2);
    ecs_entity_t c7 = ecs_insert(world, ecs_value(EcsParent, {p2}));

    ecs_entity_t c8 = ecs_new_w_pair(world, EcsChildOf, p3);
    ecs_entity_t c9 = ecs_new_w_pair(world, EcsChildOf, p3);
    ecs_entity_t c10 = ecs_new_w_pair(world, EcsChildOf, p3);
    ecs_entity_t c11 = ecs_new_w_pair(world, EcsChildOf, p3);

    ecs_add(world, c2, Foo);
    ecs_add(world, c3, Foo);
    ecs_add(world, c4, Bar);

    ecs_add(world, c9, Foo);
    ecs_add(world, c10, Foo);
    ecs_add(world, c11, Bar);

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {
                { ecs_childof(p1) }
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
        test_uint(ecs_childof(p1), ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {
                { ecs_childof(p2) }
            },
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(2, it.count);
        test_uint(c6, it.entities[0]);
        test_uint(c7, it.entities[1]);
        test_uint(ecs_childof(p2), ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {
                { ecs_childof(p3) }
            },
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c8, it.entities[0]);
        test_uint(ecs_childof(p3), ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_int(2, it.count);
        test_uint(c9, it.entities[0]);
        test_uint(c10, it.entities[1]);
        test_uint(ecs_childof(p3), ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c11, it.entities[0]);
        test_uint(ecs_childof(p3), ecs_field_id(&it, 0));
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

    ecs_entity_t p1 = ecs_new_w_id(world, EcsOrderedChildren);
    ecs_entity_t p2 = ecs_new_w_id(world, EcsOrderedChildren);
    ecs_entity_t p3 = ecs_new(world);

    /* ecs_entity_t c1 = */ ecs_insert(world, ecs_value(EcsParent, {p1}));
    ecs_entity_t c2 = ecs_insert(world, ecs_value(EcsParent, {p1}));
    ecs_entity_t c3 = ecs_insert(world, ecs_value(EcsParent, {p1}));
    ecs_entity_t c4 = ecs_insert(world, ecs_value(EcsParent, {p1}));
    /* ecs_entity_t c5 = */ ecs_new_w_pair(world, EcsChildOf, p1);

    /* ecs_entity_t c6 = */ ecs_new_w_pair(world, EcsChildOf, p2);
    /* ecs_entity_t c7 = */ ecs_insert(world, ecs_value(EcsParent, {p2}));

    /* ecs_entity_t c8 = */ ecs_new_w_pair(world, EcsChildOf, p3);
    ecs_entity_t c9 = ecs_new_w_pair(world, EcsChildOf, p3);
    ecs_entity_t c10 = ecs_new_w_pair(world, EcsChildOf, p3);
    ecs_entity_t c11 = ecs_new_w_pair(world, EcsChildOf, p3);

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
                { ecs_childof(p1) },
                { Foo }
            },
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c2, it.entities[0]);
        test_uint(ecs_childof(p1), ecs_field_id(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c3, it.entities[0]);
        test_uint(ecs_childof(p1), ecs_field_id(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));

        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {
                { ecs_childof(p2) },
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
                { ecs_childof(p3) },
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
        test_uint(ecs_childof(p3), ecs_field_id(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c11, it.entities[0]);
        test_uint(ecs_childof(p3), ecs_field_id(&it, 0));
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

    ecs_entity_t p1 = ecs_new_w_id(world, EcsOrderedChildren);
    ecs_entity_t p2 = ecs_new_w_id(world, EcsOrderedChildren);
    ecs_entity_t p3 = ecs_new(world);

    /* ecs_entity_t c1 = */ ecs_insert(world, ecs_value(EcsParent, {p1}));
    ecs_entity_t c2 = ecs_insert(world, ecs_value(EcsParent, {p1}));
    ecs_entity_t c3 = ecs_new_w_pair(world, EcsChildOf, p1);
    ecs_entity_t c4 = ecs_insert(world, ecs_value(EcsParent, {p1}));
    /* ecs_entity_t c5 = */ ecs_new_w_pair(world, EcsChildOf, p1);

    /* ecs_entity_t c6 = */ ecs_new_w_pair(world, EcsChildOf, p2);
    ecs_entity_t c7 = ecs_insert(world, ecs_value(EcsParent, {p2}));

    /* ecs_entity_t c8 = */ ecs_new_w_pair(world, EcsChildOf, p3);
    ecs_entity_t c9 = ecs_new_w_pair(world, EcsChildOf, p3);
    ecs_entity_t c10 = ecs_new_w_pair(world, EcsChildOf, p3);
    ecs_entity_t c11 = ecs_new_w_pair(world, EcsChildOf, p3);

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
                { ecs_childof(p1) },
                { Foo }
            },
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c2, it.entities[0]);
        test_uint(ecs_childof(p1), ecs_field_id(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c3, it.entities[0]);
        test_uint(ecs_childof(p1), ecs_field_id(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));

        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {
                { ecs_childof(p2) },
                { Foo }
            },
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c7, it.entities[0]);
        test_uint(ecs_childof(p2), ecs_field_id(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {
                { ecs_childof(p3) },
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
        test_uint(ecs_childof(p3), ecs_field_id(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c11, it.entities[0]);
        test_uint(ecs_childof(p3), ecs_field_id(&it, 0));
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

    ecs_entity_t p1 = ecs_new_w_id(world, EcsOrderedChildren);
    ecs_entity_t p2 = ecs_new_w_id(world, EcsOrderedChildren);
    ecs_entity_t p3 = ecs_new(world);

    /* ecs_entity_t c1 = */ ecs_insert(world, ecs_value(EcsParent, {p1}));
    ecs_entity_t c2 = ecs_insert(world, ecs_value(EcsParent, {p1}));
    ecs_entity_t c3 = ecs_new_w_pair(world, EcsChildOf, p1);
    ecs_entity_t c4 = ecs_insert(world, ecs_value(EcsParent, {p1}));
    /* ecs_entity_t c5 = */ ecs_new_w_pair(world, EcsChildOf, p1);

    /* ecs_entity_t c6 = */ ecs_new_w_pair(world, EcsChildOf, p2);
    /* ecs_entity_t c7 = */ ecs_insert(world, ecs_value(EcsParent, {p2}));

    /* ecs_entity_t c8 = */ ecs_new_w_pair(world, EcsChildOf, p3);
    ecs_entity_t c9 = ecs_new_w_pair(world, EcsChildOf, p3);
    ecs_entity_t c10 = ecs_new_w_pair(world, EcsChildOf, p3);
    ecs_entity_t c11 = ecs_new_w_pair(world, EcsChildOf, p3);

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
                { ecs_childof(p1) },
                { Foo }
            },
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c2, it.entities[0]);
        test_uint(ecs_childof(p1), ecs_field_id(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c3, it.entities[0]);
        test_uint(ecs_childof(p1), ecs_field_id(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));

        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {
                { ecs_childof(p2) },
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
                { ecs_childof(p3) },
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
        test_uint(ecs_childof(p3), ecs_field_id(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c11, it.entities[0]);
        test_uint(ecs_childof(p3), ecs_field_id(&it, 0));
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

    ecs_entity_t p1 = ecs_new_w_id(world, EcsOrderedChildren);
    ecs_entity_t p2 = ecs_new_w_id(world, EcsOrderedChildren);
    ecs_entity_t p3 = ecs_new(world);

    /* ecs_entity_t c1 = */ ecs_insert(world, ecs_value(EcsParent, {p1}));
    ecs_entity_t c2 = ecs_insert(world, ecs_value(EcsParent, {p1}));
    ecs_entity_t c3 = ecs_insert(world, ecs_value(EcsParent, {p1}));
    ecs_entity_t c4 = ecs_insert(world, ecs_value(EcsParent, {p1}));
    /* ecs_entity_t c5 = */ ecs_new_w_pair(world, EcsChildOf, p1);

    /* ecs_entity_t c6 = */ ecs_new_w_pair(world, EcsChildOf, p2);
    /* ecs_entity_t c7 = */ ecs_insert(world, ecs_value(EcsParent, {p2}));

    /* ecs_entity_t c8 = */ ecs_new_w_pair(world, EcsChildOf, p3);
    ecs_entity_t c9 = ecs_new_w_pair(world, EcsChildOf, p3);
    ecs_entity_t c10 = ecs_new_w_pair(world, EcsChildOf, p3);
    ecs_entity_t c11 = ecs_new_w_pair(world, EcsChildOf, p3);

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
                { ecs_childof(p1) },
                { ecs_id(Position) }
            },
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c2, it.entities[0]);
        test_uint(ecs_childof(p1), ecs_field_id(&it, 0));
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
        test_uint(ecs_childof(p1), ecs_field_id(&it, 0));
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
                { ecs_childof(p2) },
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
                { ecs_childof(p3) },
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
        test_uint(ecs_childof(p3), ecs_field_id(&it, 0));
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
        test_uint(ecs_childof(p3), ecs_field_id(&it, 0));
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

    ecs_entity_t p1 = ecs_new_w_id(world, EcsOrderedChildren);
    ecs_entity_t p2 = ecs_new_w_id(world, EcsOrderedChildren);
    ecs_entity_t p3 = ecs_new(world);

    /* ecs_entity_t c1 = */ ecs_insert(world, ecs_value(EcsParent, {p1}));
    ecs_entity_t c2 = ecs_insert(world, ecs_value(EcsParent, {p1}));
    ecs_entity_t c3 = ecs_new_w_pair(world, EcsChildOf, p1);
    ecs_entity_t c4 = ecs_insert(world, ecs_value(EcsParent, {p1}));
    /* ecs_entity_t c5 = */ ecs_new_w_pair(world, EcsChildOf, p1);

    /* ecs_entity_t c6 = */ ecs_new_w_pair(world, EcsChildOf, p2);
    ecs_entity_t c7 = ecs_insert(world, ecs_value(EcsParent, {p2}));

    /* ecs_entity_t c8 = */ ecs_new_w_pair(world, EcsChildOf, p3);
    ecs_entity_t c9 = ecs_new_w_pair(world, EcsChildOf, p3);
    ecs_entity_t c10 = ecs_new_w_pair(world, EcsChildOf, p3);
    ecs_entity_t c11 = ecs_new_w_pair(world, EcsChildOf, p3);

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
                { ecs_childof(p1) },
                { ecs_id(Position) }
            },
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c2, it.entities[0]);
        test_uint(ecs_childof(p1), ecs_field_id(&it, 0));
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
        test_uint(ecs_childof(p1), ecs_field_id(&it, 0));
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
                { ecs_childof(p2) },
                { ecs_id(Position) }
            },
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c7, it.entities[0]);
        test_uint(ecs_childof(p2), ecs_field_id(&it, 0));
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
                { ecs_childof(p3) },
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
        test_uint(ecs_childof(p3), ecs_field_id(&it, 0));
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
        test_uint(ecs_childof(p3), ecs_field_id(&it, 0));
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

    ecs_entity_t p1 = ecs_new_w_id(world, EcsOrderedChildren);
    ecs_entity_t p2 = ecs_new_w_id(world, EcsOrderedChildren);
    ecs_entity_t p3 = ecs_new(world);

    /* ecs_entity_t c1 = */ ecs_insert(world, ecs_value(EcsParent, {p1}));
    ecs_entity_t c2 = ecs_insert(world, ecs_value(EcsParent, {p1}));
    ecs_entity_t c3 = ecs_new_w_pair(world, EcsChildOf, p1);
    ecs_entity_t c4 = ecs_insert(world, ecs_value(EcsParent, {p1}));
    /* ecs_entity_t c5 = */ ecs_new_w_pair(world, EcsChildOf, p1);

    /* ecs_entity_t c6 = */ ecs_new_w_pair(world, EcsChildOf, p2);
    /* ecs_entity_t c7 = */ ecs_insert(world, ecs_value(EcsParent, {p2}));

    /* ecs_entity_t c8 = */ ecs_new_w_pair(world, EcsChildOf, p3);
    ecs_entity_t c9 = ecs_new_w_pair(world, EcsChildOf, p3);
    ecs_entity_t c10 = ecs_new_w_pair(world, EcsChildOf, p3);
    ecs_entity_t c11 = ecs_new_w_pair(world, EcsChildOf, p3);

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
                { ecs_childof(p1) },
                { ecs_id(Position) }
            },
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c2, it.entities[0]);
        test_uint(ecs_childof(p1), ecs_field_id(&it, 0));
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
        test_uint(ecs_childof(p1), ecs_field_id(&it, 0));
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
                { ecs_childof(p2) },
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
                { ecs_childof(p3) },
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
        test_uint(ecs_childof(p3), ecs_field_id(&it, 0));
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
        test_uint(ecs_childof(p3), ecs_field_id(&it, 0));
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

    ecs_entity_t p1 = ecs_new_w_id(world, EcsOrderedChildren);
    ecs_entity_t p2 = ecs_new_w_id(world, EcsOrderedChildren);
    ecs_entity_t p3 = ecs_new(world);

    /* ecs_entity_t c1 = */ ecs_insert(world, ecs_value(EcsParent, {p1}));
    ecs_entity_t c2 = ecs_insert(world, ecs_value(EcsParent, {p1}));
    ecs_entity_t c3 = ecs_insert(world, ecs_value(EcsParent, {p1}));
    ecs_entity_t c4 = ecs_insert(world, ecs_value(EcsParent, {p1}));
    /* ecs_entity_t c5 = */ ecs_new_w_pair(world, EcsChildOf, p1);

    /* ecs_entity_t c6 = */ ecs_new_w_pair(world, EcsChildOf, p2);
    /* ecs_entity_t c7 = */ ecs_insert(world, ecs_value(EcsParent, {p2}));

    /* ecs_entity_t c8 = */ ecs_new_w_pair(world, EcsChildOf, p3);
    ecs_entity_t c9 = ecs_new_w_pair(world, EcsChildOf, p3);
    ecs_entity_t c10 = ecs_new_w_pair(world, EcsChildOf, p3);
    ecs_entity_t c11 = ecs_new_w_pair(world, EcsChildOf, p3);

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
                { ecs_childof(p1) }
            },
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c2, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_childof(p1), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c3, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_childof(p1), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));

        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {
                { Foo },
                { ecs_childof(p2) }
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
                { ecs_childof(p3) }
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
        test_uint(ecs_childof(p3), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c11, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_childof(p3), ecs_field_id(&it, 1));
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

    ecs_entity_t p1 = ecs_new_w_id(world, EcsOrderedChildren);
    ecs_entity_t p2 = ecs_new_w_id(world, EcsOrderedChildren);
    ecs_entity_t p3 = ecs_new(world);

    /* ecs_entity_t c1 = */ ecs_insert(world, ecs_value(EcsParent, {p1}));
    ecs_entity_t c2 = ecs_insert(world, ecs_value(EcsParent, {p1}));
    ecs_entity_t c3 = ecs_new_w_pair(world, EcsChildOf, p1);
    ecs_entity_t c4 = ecs_insert(world, ecs_value(EcsParent, {p1}));
    /* ecs_entity_t c5 = */ ecs_new_w_pair(world, EcsChildOf, p1);

    /* ecs_entity_t c6 = */ ecs_new_w_pair(world, EcsChildOf, p2);
    ecs_entity_t c7 = ecs_insert(world, ecs_value(EcsParent, {p2}));

    /* ecs_entity_t c8 = */ ecs_new_w_pair(world, EcsChildOf, p3);
    ecs_entity_t c9 = ecs_new_w_pair(world, EcsChildOf, p3);
    ecs_entity_t c10 = ecs_new_w_pair(world, EcsChildOf, p3);
    ecs_entity_t c11 = ecs_new_w_pair(world, EcsChildOf, p3);

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
                { ecs_childof(p1) }
            },
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c2, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_childof(p1), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c3, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_childof(p1), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));

        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {
                { Foo },
                { ecs_childof(p2) }
            },
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c7, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_childof(p2), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {
                { Foo },
                { ecs_childof(p3) }
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
        test_uint(ecs_childof(p3), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c11, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_childof(p3), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));

        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    ecs_fini(world);
}

void NonFragmentingChildOf_this_src_childof_tag_w_parent_single_child_table(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_entity_t p1 = ecs_new_w_id(world, EcsOrderedChildren);
    ecs_entity_t p2 = ecs_new_w_id(world, EcsOrderedChildren);
    ecs_entity_t p3 = ecs_new(world);

    /* ecs_entity_t c1 = */ ecs_insert(world, ecs_value(EcsParent, {p1}));
    ecs_entity_t c2 = ecs_insert(world, ecs_value(EcsParent, {p1}));
    ecs_entity_t c3 = ecs_new_w_pair(world, EcsChildOf, p1);
    ecs_entity_t c4 = ecs_insert(world, ecs_value(EcsParent, {p1}));
    /* ecs_entity_t c5 = */ ecs_new_w_pair(world, EcsChildOf, p1);

    /* ecs_entity_t c6 = */ ecs_new_w_pair(world, EcsChildOf, p2);
    /* ecs_entity_t c7 = */ ecs_insert(world, ecs_value(EcsParent, {p2}));

    /* ecs_entity_t c8 = */ ecs_new_w_pair(world, EcsChildOf, p3);
    ecs_entity_t c9 = ecs_new_w_pair(world, EcsChildOf, p3);
    ecs_entity_t c10 = ecs_new_w_pair(world, EcsChildOf, p3);
    ecs_entity_t c11 = ecs_new_w_pair(world, EcsChildOf, p3);

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
                { ecs_childof(p1) }
            },
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c2, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_childof(p1), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c3, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_childof(p1), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));

        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {
                { Foo },
                { ecs_childof(p2) }
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
                { ecs_childof(p3) }
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
        test_uint(ecs_childof(p3), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c11, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_childof(p3), ecs_field_id(&it, 1));
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

    ecs_entity_t p1 = ecs_new_w_id(world, EcsOrderedChildren);
    ecs_entity_t p2 = ecs_new_w_id(world, EcsOrderedChildren);
    ecs_entity_t p3 = ecs_new(world);

    /* ecs_entity_t c1 = */ ecs_insert(world, ecs_value(EcsParent, {p1}));
    ecs_entity_t c2 = ecs_insert(world, ecs_value(EcsParent, {p1}));
    ecs_entity_t c3 = ecs_insert(world, ecs_value(EcsParent, {p1}));
    ecs_entity_t c4 = ecs_insert(world, ecs_value(EcsParent, {p1}));
    /* ecs_entity_t c5 = */ ecs_new_w_pair(world, EcsChildOf, p1);

    /* ecs_entity_t c6 = */ ecs_new_w_pair(world, EcsChildOf, p2);
    /* ecs_entity_t c7 = */ ecs_insert(world, ecs_value(EcsParent, {p2}));

    /* ecs_entity_t c8 = */ ecs_new_w_pair(world, EcsChildOf, p3);
    ecs_entity_t c9 = ecs_new_w_pair(world, EcsChildOf, p3);
    ecs_entity_t c10 = ecs_new_w_pair(world, EcsChildOf, p3);
    ecs_entity_t c11 = ecs_new_w_pair(world, EcsChildOf, p3);

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
                { ecs_childof(p1) }
            },
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c2, it.entities[0]);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(ecs_childof(p1), ecs_field_id(&it, 1));
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
        test_uint(ecs_childof(p1), ecs_field_id(&it, 1));
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
                { ecs_childof(p2) }
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
                { ecs_childof(p3) }
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
        test_uint(ecs_childof(p3), ecs_field_id(&it, 1));
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
        test_uint(ecs_childof(p3), ecs_field_id(&it, 1));
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

    ecs_entity_t p1 = ecs_new_w_id(world, EcsOrderedChildren);
    ecs_entity_t p2 = ecs_new_w_id(world, EcsOrderedChildren);
    ecs_entity_t p3 = ecs_new(world);

    /* ecs_entity_t c1 = */ ecs_insert(world, ecs_value(EcsParent, {p1}));
    ecs_entity_t c2 = ecs_insert(world, ecs_value(EcsParent, {p1}));
    ecs_entity_t c3 = ecs_new_w_pair(world, EcsChildOf, p1);
    ecs_entity_t c4 = ecs_insert(world, ecs_value(EcsParent, {p1}));
    /* ecs_entity_t c5 = */ ecs_new_w_pair(world, EcsChildOf, p1);

    /* ecs_entity_t c6 = */ ecs_new_w_pair(world, EcsChildOf, p2);
    ecs_entity_t c7 = ecs_insert(world, ecs_value(EcsParent, {p2}));

    /* ecs_entity_t c8 = */ ecs_new_w_pair(world, EcsChildOf, p3);
    ecs_entity_t c9 = ecs_new_w_pair(world, EcsChildOf, p3);
    ecs_entity_t c10 = ecs_new_w_pair(world, EcsChildOf, p3);
    ecs_entity_t c11 = ecs_new_w_pair(world, EcsChildOf, p3);

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
                { ecs_childof(p1) }
            },
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c2, it.entities[0]);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(ecs_childof(p1), ecs_field_id(&it, 1));
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
        test_uint(ecs_childof(p1), ecs_field_id(&it, 1));
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
                { ecs_childof(p2) },
                { ecs_id(Position) }
            },
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c7, it.entities[0]);
        test_uint(ecs_childof(p2), ecs_field_id(&it, 0));
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
                { ecs_childof(p3) },
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
        test_uint(ecs_childof(p3), ecs_field_id(&it, 0));
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
        test_uint(ecs_childof(p3), ecs_field_id(&it, 0));
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

    ecs_entity_t p1 = ecs_new_w_id(world, EcsOrderedChildren);
    ecs_entity_t p2 = ecs_new_w_id(world, EcsOrderedChildren);
    ecs_entity_t p3 = ecs_new(world);

    /* ecs_entity_t c1 = */ ecs_insert(world, ecs_value(EcsParent, {p1}));
    ecs_entity_t c2 = ecs_insert(world, ecs_value(EcsParent, {p1}));
    ecs_entity_t c3 = ecs_new_w_pair(world, EcsChildOf, p1);
    ecs_entity_t c4 = ecs_insert(world, ecs_value(EcsParent, {p1}));
    /* ecs_entity_t c5 = */ ecs_new_w_pair(world, EcsChildOf, p1);

    /* ecs_entity_t c6 = */ ecs_new_w_pair(world, EcsChildOf, p2);
    /* ecs_entity_t c7 = */ ecs_insert(world, ecs_value(EcsParent, {p2}));

    /* ecs_entity_t c8 = */ ecs_new_w_pair(world, EcsChildOf, p3);
    ecs_entity_t c9 = ecs_new_w_pair(world, EcsChildOf, p3);
    ecs_entity_t c10 = ecs_new_w_pair(world, EcsChildOf, p3);
    ecs_entity_t c11 = ecs_new_w_pair(world, EcsChildOf, p3);

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
                { ecs_childof(p1) }
            },
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c2, it.entities[0]);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(ecs_childof(p1), ecs_field_id(&it, 1));
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
        test_uint(ecs_childof(p1), ecs_field_id(&it, 1));
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
                { ecs_childof(p2) }
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
                { ecs_childof(p3) }
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
        test_uint(ecs_childof(p3), ecs_field_id(&it, 1));
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
        test_uint(ecs_childof(p3), ecs_field_id(&it, 1));
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

    ecs_entity_t p1 = ecs_new_w_id(world, EcsOrderedChildren);
    ecs_entity_t p2 = ecs_new_w_id(world, EcsOrderedChildren);
    ecs_entity_t p3 = ecs_new(world);

    ecs_entity_t c1 = ecs_insert(world, ecs_value(EcsParent, {p1}));
    ecs_entity_t c2 = ecs_insert(world, ecs_value(EcsParent, {p1}));
    ecs_entity_t c3 = ecs_insert(world, ecs_value(EcsParent, {p1}));
    ecs_entity_t c4 = ecs_insert(world, ecs_value(EcsParent, {p1}));
    ecs_entity_t c5 = ecs_new_w_pair(world, EcsChildOf, p1);

    ecs_entity_t c6 = ecs_new_w_pair(world, EcsChildOf, p2);
    ecs_entity_t c7 = ecs_insert(world, ecs_value(EcsParent, {p2}));

    ecs_entity_t c8 = ecs_new_w_pair(world, EcsChildOf, p3);
    ecs_entity_t c9 = ecs_new_w_pair(world, EcsChildOf, p3);
    ecs_entity_t c10 = ecs_new_w_pair(world, EcsChildOf, p3);
    ecs_entity_t c11 = ecs_new_w_pair(world, EcsChildOf, p3);

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
        test_uint(ecs_childof(p3), ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_int(2, it.count);
        test_uint(c9, it.entities[0]);
        test_uint(c10, it.entities[1]);
        test_uint(ecs_childof(p3), ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c11, it.entities[0]);
        test_uint(ecs_childof(p3), ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_int(2, it.count);
        test_uint(c6, it.entities[0]);
        test_uint(c7, it.entities[1]);
        test_uint(ecs_childof(p2), ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_int(5, it.count);
        test_uint(c1, it.entities[0]);
        test_uint(c2, it.entities[1]);
        test_uint(c3, it.entities[2]);
        test_uint(c4, it.entities[3]);
        test_uint(c5, it.entities[4]);
        test_uint(ecs_childof(p1), ecs_field_id(&it, 0));
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

    ecs_entity_t p1 = ecs_new_w_id(world, EcsOrderedChildren);
    ecs_entity_t p2 = ecs_new_w_id(world, EcsOrderedChildren);
    ecs_entity_t p3 = ecs_new(world);

    /* ecs_entity_t c1 = */ ecs_insert(world, ecs_value(EcsParent, {p1}));
    ecs_entity_t c2 = ecs_insert(world, ecs_value(EcsParent, {p1}));
    ecs_entity_t c3 = ecs_insert(world, ecs_value(EcsParent, {p1}));
    ecs_entity_t c4 = ecs_insert(world, ecs_value(EcsParent, {p1}));
    /* ecs_entity_t c5 = */ ecs_new_w_pair(world, EcsChildOf, p1);

    /* ecs_entity_t c6 = */ ecs_new_w_pair(world, EcsChildOf, p2);
    /* ecs_entity_t c7 = */ ecs_insert(world, ecs_value(EcsParent, {p2}));

    /* ecs_entity_t c8 = */ ecs_new_w_pair(world, EcsChildOf, p3);
    ecs_entity_t c9 = ecs_new_w_pair(world, EcsChildOf, p3);
    ecs_entity_t c10 = ecs_new_w_pair(world, EcsChildOf, p3);
    ecs_entity_t c11 = ecs_new_w_pair(world, EcsChildOf, p3);

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
        test_bool(true, ecs_query_next(&it));
        test_int(2, it.count);
        test_uint(c9, it.entities[0]);
        test_uint(c10, it.entities[1]);
        test_uint(ecs_childof(p3), ecs_field_id(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c11, it.entities[0]);
        test_uint(ecs_childof(p3), ecs_field_id(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c2, it.entities[0]);
        test_uint(ecs_childof(p1), ecs_field_id(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c3, it.entities[0]);
        test_uint(ecs_childof(p1), ecs_field_id(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c4, it.entities[0]);
        test_uint(ecs_childof(p1), ecs_field_id(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));

        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    ecs_fini(world);
}

void NonFragmentingChildOf_this_src_childof_wildcard_w_component(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Bar);

    ecs_entity_t p1 = ecs_new_w_id(world, EcsOrderedChildren);
    ecs_entity_t p2 = ecs_new_w_id(world, EcsOrderedChildren);
    ecs_entity_t p3 = ecs_new(world);

    /* ecs_entity_t c1 = */ ecs_insert(world, ecs_value(EcsParent, {p1}));
    ecs_entity_t c2 = ecs_insert(world, ecs_value(EcsParent, {p1}));
    ecs_entity_t c3 = ecs_insert(world, ecs_value(EcsParent, {p1}));
    ecs_entity_t c4 = ecs_insert(world, ecs_value(EcsParent, {p1}));
    /* ecs_entity_t c5 = */ ecs_new_w_pair(world, EcsChildOf, p1);

    /* ecs_entity_t c6 = */ ecs_new_w_pair(world, EcsChildOf, p2);
    /* ecs_entity_t c7 = */ ecs_insert(world, ecs_value(EcsParent, {p2}));

    /* ecs_entity_t c8 = */ ecs_new_w_pair(world, EcsChildOf, p3);
    ecs_entity_t c9 = ecs_new_w_pair(world, EcsChildOf, p3);
    ecs_entity_t c10 = ecs_new_w_pair(world, EcsChildOf, p3);
    ecs_entity_t c11 = ecs_new_w_pair(world, EcsChildOf, p3);

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
        test_bool(true, ecs_query_next(&it));
        test_int(2, it.count);
        test_uint(c9, it.entities[0]);
        test_uint(c10, it.entities[1]);
        test_uint(ecs_childof(p3), ecs_field_id(&it, 0));
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
        test_uint(ecs_childof(p3), ecs_field_id(&it, 0));
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
        test_uint(ecs_childof(p1), ecs_field_id(&it, 0));
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
        test_uint(ecs_childof(p1), ecs_field_id(&it, 0));
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
        test_uint(ecs_childof(p1), ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        {
            Position *p = ecs_field(&it, Position, 1);
            test_int(p[0].x, 3); test_int(p[0].y, 4);
        }

        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    ecs_fini(world);
}

void NonFragmentingChildOf_this_src_childof_any(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_entity_t p1 = ecs_new_w_id(world, EcsOrderedChildren);
    ecs_entity_t p2 = ecs_new_w_id(world, EcsOrderedChildren);
    ecs_entity_t p3 = ecs_new(world);

    ecs_entity_t c1 = ecs_insert(world, ecs_value(EcsParent, {p1}));
    ecs_entity_t c2 = ecs_insert(world, ecs_value(EcsParent, {p1}));
    ecs_entity_t c3 = ecs_insert(world, ecs_value(EcsParent, {p1}));
    ecs_entity_t c4 = ecs_insert(world, ecs_value(EcsParent, {p1}));
    ecs_entity_t c5 = ecs_new_w_pair(world, EcsChildOf, p1);

    ecs_entity_t c6 = ecs_new_w_pair(world, EcsChildOf, p2);
    ecs_entity_t c7 = ecs_insert(world, ecs_value(EcsParent, {p2}));

    ecs_entity_t c8 = ecs_new_w_pair(world, EcsChildOf, p3);
    ecs_entity_t c9 = ecs_new_w_pair(world, EcsChildOf, p3);
    ecs_entity_t c10 = ecs_new_w_pair(world, EcsChildOf, p3);
    ecs_entity_t c11 = ecs_new_w_pair(world, EcsChildOf, p3);

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
        test_int(1, it.count);
        test_uint(c8, it.entities[0]);
        test_uint(ecs_childof(EcsWildcard), ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_int(2, it.count);
        test_uint(c9, it.entities[0]);
        test_uint(c10, it.entities[1]);
        test_uint(ecs_childof(EcsWildcard), ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c11, it.entities[0]);
        test_uint(ecs_childof(EcsWildcard), ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_int(2, it.count);
        test_uint(c6, it.entities[0]);
        test_uint(c7, it.entities[1]);
        test_uint(ecs_childof(EcsWildcard), ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_int(5, it.count);
        test_uint(c1, it.entities[0]);
        test_uint(c2, it.entities[1]);
        test_uint(c3, it.entities[2]);
        test_uint(c4, it.entities[3]);
        test_uint(c5, it.entities[4]);
        test_uint(ecs_childof(EcsWildcard), ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));

        /* Skip builtin entities */
        while (ecs_query_next(&it)) {
            test_assert(!ecs_table_has_id(world, it.table, EcsNotQueryable));
        }

        ecs_query_fini(q);
    }

    ecs_fini(world);
}

void NonFragmentingChildOf_this_src_childof_any_w_tag(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_entity_t p1 = ecs_new_w_id(world, EcsOrderedChildren);
    ecs_entity_t p2 = ecs_new_w_id(world, EcsOrderedChildren);
    ecs_entity_t p3 = ecs_new(world);

    /* ecs_entity_t c1 = */ ecs_insert(world, ecs_value(EcsParent, {p1}));
    ecs_entity_t c2 = ecs_insert(world, ecs_value(EcsParent, {p1}));
    ecs_entity_t c3 = ecs_insert(world, ecs_value(EcsParent, {p1}));
    ecs_entity_t c4 = ecs_insert(world, ecs_value(EcsParent, {p1}));
    /* ecs_entity_t c5 = */ ecs_new_w_pair(world, EcsChildOf, p1);

    /* ecs_entity_t c6 = */ ecs_new_w_pair(world, EcsChildOf, p2);
    /* ecs_entity_t c7 = */ ecs_insert(world, ecs_value(EcsParent, {p2}));

    /* ecs_entity_t c8 = */ ecs_new_w_pair(world, EcsChildOf, p3);
    ecs_entity_t c9 = ecs_new_w_pair(world, EcsChildOf, p3);
    ecs_entity_t c10 = ecs_new_w_pair(world, EcsChildOf, p3);
    ecs_entity_t c11 = ecs_new_w_pair(world, EcsChildOf, p3);

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
        test_uint(c9, it.entities[0]);
        test_uint(c10, it.entities[1]);
        test_uint(ecs_childof(EcsWildcard), ecs_field_id(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c11, it.entities[0]);
        test_uint(ecs_childof(EcsWildcard), ecs_field_id(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c2, it.entities[0]);
        test_uint(ecs_childof(EcsWildcard), ecs_field_id(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c3, it.entities[0]);
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

        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    ecs_fini(world);
}

void NonFragmentingChildOf_this_src_childof_any_w_component(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Bar);

    ecs_entity_t p1 = ecs_new_w_id(world, EcsOrderedChildren);
    ecs_entity_t p2 = ecs_new_w_id(world, EcsOrderedChildren);
    ecs_entity_t p3 = ecs_new(world);

    /* ecs_entity_t c1 = */ ecs_insert(world, ecs_value(EcsParent, {p1}));
    ecs_entity_t c2 = ecs_insert(world, ecs_value(EcsParent, {p1}));
    ecs_entity_t c3 = ecs_insert(world, ecs_value(EcsParent, {p1}));
    ecs_entity_t c4 = ecs_insert(world, ecs_value(EcsParent, {p1}));
    /* ecs_entity_t c5 = */ ecs_new_w_pair(world, EcsChildOf, p1);

    /* ecs_entity_t c6 = */ ecs_new_w_pair(world, EcsChildOf, p2);
    /* ecs_entity_t c7 = */ ecs_insert(world, ecs_value(EcsParent, {p2}));

    /* ecs_entity_t c8 = */ ecs_new_w_pair(world, EcsChildOf, p3);
    ecs_entity_t c9 = ecs_new_w_pair(world, EcsChildOf, p3);
    ecs_entity_t c10 = ecs_new_w_pair(world, EcsChildOf, p3);
    ecs_entity_t c11 = ecs_new_w_pair(world, EcsChildOf, p3);

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

        test_bool(true, ecs_query_next(&it));
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

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c2, it.entities[0]);
        test_uint(ecs_childof(EcsWildcard), ecs_field_id(&it, 0));
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
        test_uint(ecs_childof(EcsWildcard), ecs_field_id(&it, 0));
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
        test_uint(ecs_childof(EcsWildcard), ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        {
            Position *p = ecs_field(&it, Position, 1);
            test_int(p[0].x, 3); test_int(p[0].y, 4);
        }

        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    ecs_fini(world);
}

void NonFragmentingChildOf_this_src_childof_var(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_entity_t p1 = ecs_new_w_id(world, EcsOrderedChildren);
    ecs_entity_t p2 = ecs_new_w_id(world, EcsOrderedChildren);
    ecs_entity_t p3 = ecs_new(world);

    ecs_entity_t c1 = ecs_insert(world, ecs_value(EcsParent, {p1}));
    ecs_entity_t c2 = ecs_insert(world, ecs_value(EcsParent, {p1}));
    ecs_entity_t c3 = ecs_insert(world, ecs_value(EcsParent, {p1}));
    ecs_entity_t c4 = ecs_insert(world, ecs_value(EcsParent, {p1}));
    ecs_entity_t c5 = ecs_new_w_pair(world, EcsChildOf, p1);

    ecs_entity_t c6 = ecs_new_w_pair(world, EcsChildOf, p2);
    ecs_entity_t c7 = ecs_insert(world, ecs_value(EcsParent, {p2}));

    ecs_entity_t c8 = ecs_new_w_pair(world, EcsChildOf, p3);
    ecs_entity_t c9 = ecs_new_w_pair(world, EcsChildOf, p3);
    ecs_entity_t c10 = ecs_new_w_pair(world, EcsChildOf, p3);
    ecs_entity_t c11 = ecs_new_w_pair(world, EcsChildOf, p3);

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
        test_uint(ecs_childof(p3), ecs_field_id(&it, 0));
        test_uint(p3, ecs_iter_get_var(&it, x_var));
        test_bool(true, ecs_field_is_set(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_int(2, it.count);
        test_uint(c9, it.entities[0]);
        test_uint(c10, it.entities[1]);
        test_uint(ecs_childof(p3), ecs_field_id(&it, 0));
        test_uint(p3, ecs_iter_get_var(&it, x_var));
        test_bool(true, ecs_field_is_set(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c11, it.entities[0]);
        test_uint(ecs_childof(p3), ecs_field_id(&it, 0));
        test_uint(p3, ecs_iter_get_var(&it, x_var));
        test_bool(true, ecs_field_is_set(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_int(2, it.count);
        test_uint(c6, it.entities[0]);
        test_uint(c7, it.entities[1]);
        test_uint(ecs_childof(p2), ecs_field_id(&it, 0));
        test_uint(p2, ecs_iter_get_var(&it, x_var));
        test_bool(true, ecs_field_is_set(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_int(5, it.count);
        test_uint(c1, it.entities[0]);
        test_uint(c2, it.entities[1]);
        test_uint(c3, it.entities[2]);
        test_uint(c4, it.entities[3]);
        test_uint(c5, it.entities[4]);
        test_uint(ecs_childof(p1), ecs_field_id(&it, 0));
        test_uint(p1, ecs_iter_get_var(&it, x_var));
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

    ecs_entity_t p1 = ecs_new_w_id(world, EcsOrderedChildren);
    ecs_entity_t p2 = ecs_new_w_id(world, EcsOrderedChildren);
    ecs_entity_t p3 = ecs_new(world);

    ecs_entity_t c1 = ecs_insert(world, ecs_value(EcsParent, {p1}));
    ecs_entity_t c2 = ecs_insert(world, ecs_value(EcsParent, {p1}));
    ecs_entity_t c3 = ecs_insert(world, ecs_value(EcsParent, {p1}));
    ecs_entity_t c4 = ecs_insert(world, ecs_value(EcsParent, {p1}));
    ecs_entity_t c5 = ecs_new_w_pair(world, EcsChildOf, p1);

    ecs_entity_t c6 = ecs_new_w_pair(world, EcsChildOf, p2);
    ecs_entity_t c7 = ecs_insert(world, ecs_value(EcsParent, {p2}));

    ecs_entity_t c8 = ecs_new_w_pair(world, EcsChildOf, p3);
    ecs_entity_t c9 = ecs_new_w_pair(world, EcsChildOf, p3);
    ecs_entity_t c10 = ecs_new_w_pair(world, EcsChildOf, p3);
    ecs_entity_t c11 = ecs_new_w_pair(world, EcsChildOf, p3);

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
        ecs_iter_set_var(&it, x_var, p1);
        test_bool(true, ecs_query_next(&it));
        test_int(5, it.count);
        test_uint(c1, it.entities[0]);
        test_uint(c2, it.entities[1]);
        test_uint(c3, it.entities[2]);
        test_uint(c4, it.entities[3]);
        test_uint(c5, it.entities[4]);
        test_uint(ecs_childof(p1), ecs_field_id(&it, 0));
        test_uint(p1, ecs_iter_get_var(&it, x_var));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_query_next(&it));
    }

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, x_var, p2);
        test_bool(true, ecs_query_next(&it));
        test_int(2, it.count);
        test_uint(c6, it.entities[0]);
        test_uint(c7, it.entities[1]);
        test_uint(ecs_childof(p2), ecs_field_id(&it, 0));
        test_uint(p2, ecs_iter_get_var(&it, x_var));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_query_next(&it));
    }

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, x_var, p3);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c8, it.entities[0]);
        test_uint(ecs_childof(p3), ecs_field_id(&it, 0));
        test_uint(p3, ecs_iter_get_var(&it, x_var));
        test_bool(true, ecs_field_is_set(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_int(2, it.count);
        test_uint(c9, it.entities[0]);
        test_uint(c10, it.entities[1]);
        test_uint(ecs_childof(p3), ecs_field_id(&it, 0));
        test_uint(p3, ecs_iter_get_var(&it, x_var));
        test_bool(true, ecs_field_is_set(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c11, it.entities[0]);
        test_uint(ecs_childof(p3), ecs_field_id(&it, 0));
        test_uint(p3, ecs_iter_get_var(&it, x_var));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_src_1_table_1_child(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_entity_t p = ecs_new_w_id(world, EcsOrderedChildren);
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

void NonFragmentingChildOf_this_src_1_table_2_children(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_entity_t p = ecs_new_w_id(world, EcsOrderedChildren);
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

    ecs_entity_t p1 = ecs_new_w_id(world, EcsOrderedChildren);
    ecs_entity_t p2 = ecs_new_w_id(world, EcsOrderedChildren);
    ecs_entity_t e1 = ecs_insert(world, ecs_value(EcsParent, {p1}));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(EcsParent, {p2}));
    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {{ ecs_childof(p1) }},
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e1, it.entities[0]);
        test_uint(ecs_childof(p1), ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {{ ecs_childof(p2) }},
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e2, it.entities[0]);
        test_uint(ecs_childof(p2), ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    ecs_fini(world);
}

void NonFragmentingChildOf_this_src_1_table_n_children_mixed_parents(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_entity_t p1 = ecs_new_w_id(world, EcsOrderedChildren);
    ecs_entity_t p2 = ecs_new_w_id(world, EcsOrderedChildren);
    ecs_entity_t e1 = ecs_insert(world, ecs_value(EcsParent, {p1}));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(EcsParent, {p2}));
    ecs_entity_t e3 = ecs_insert(world, ecs_value(EcsParent, {p1}));
    ecs_entity_t e4 = ecs_insert(world, ecs_value(EcsParent, {p2}));
    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);
    ecs_add(world, e3, Foo);
    ecs_add(world, e4, Foo);

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {{ ecs_childof(p1) }},
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(2, it.count);
        test_uint(e1, it.entities[0]);
        test_uint(e3, it.entities[1]);
        test_uint(ecs_childof(p1), ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {{ ecs_childof(p2) }},
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(2, it.count);
        test_uint(e2, it.entities[0]);
        test_uint(e4, it.entities[1]);
        test_uint(ecs_childof(p2), ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    ecs_fini(world);
}

void NonFragmentingChildOf_this_src_1_table_1_child_w_tag(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_entity_t p = ecs_new_w_id(world, EcsOrderedChildren);
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

    ecs_entity_t p = ecs_new_w_id(world, EcsOrderedChildren);
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

    ecs_entity_t p1 = ecs_new_w_id(world, EcsOrderedChildren);
    ecs_entity_t p2 = ecs_new_w_id(world, EcsOrderedChildren);
    ecs_entity_t e1 = ecs_insert(world, ecs_value(EcsParent, {p1}));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(EcsParent, {p2}));
    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {{ ecs_childof(p1) }, { Foo }},
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e1, it.entities[0]);
        test_uint(ecs_childof(p1), ecs_field_id(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {{ ecs_childof(p2) }, { Foo }},
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e2, it.entities[0]);
        test_uint(ecs_childof(p2), ecs_field_id(&it, 0));
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

    ecs_entity_t p1 = ecs_new_w_id(world, EcsOrderedChildren);
    ecs_entity_t p2 = ecs_new_w_id(world, EcsOrderedChildren);
    ecs_entity_t e1 = ecs_insert(world, ecs_value(EcsParent, {p1}));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(EcsParent, {p2}));
    ecs_entity_t e3 = ecs_insert(world, ecs_value(EcsParent, {p1}));
    ecs_entity_t e4 = ecs_insert(world, ecs_value(EcsParent, {p2}));
    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);
    ecs_add(world, e3, Foo);
    ecs_add(world, e4, Foo);

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {{ ecs_childof(p1) }, { Foo }},
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e1, it.entities[0]);
        test_uint(ecs_childof(p1), ecs_field_id(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e3, it.entities[0]);
        test_uint(ecs_childof(p1), ecs_field_id(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {{ ecs_childof(p2) }, { Foo }},
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e2, it.entities[0]);
        test_uint(ecs_childof(p2), ecs_field_id(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e4, it.entities[0]);
        test_uint(ecs_childof(p2), ecs_field_id(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));

        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    ecs_fini(world);
}

void NonFragmentingChildOf_this_src_written_1_table_1_child(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_entity_t p = ecs_new_w_id(world, EcsOrderedChildren);
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

    ecs_entity_t p = ecs_new_w_id(world, EcsOrderedChildren);
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

    ecs_entity_t p1 = ecs_new_w_id(world, EcsOrderedChildren);
    ecs_entity_t p2 = ecs_new_w_id(world, EcsOrderedChildren);
    ecs_entity_t e1 = ecs_insert(world, ecs_value(EcsParent, {p1}));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(EcsParent, {p2}));
    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {{ Foo }, { ecs_childof(p1) }},
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e1, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_childof(p1), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {{ ecs_childof(p2) }, { Foo }},
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e2, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_childof(p2), ecs_field_id(&it, 1));
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

    ecs_entity_t p1 = ecs_new_w_id(world, EcsOrderedChildren);
    ecs_entity_t p2 = ecs_new_w_id(world, EcsOrderedChildren);
    ecs_entity_t e1 = ecs_insert(world, ecs_value(EcsParent, {p1}));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(EcsParent, {p2}));
    ecs_entity_t e3 = ecs_insert(world, ecs_value(EcsParent, {p1}));
    ecs_entity_t e4 = ecs_insert(world, ecs_value(EcsParent, {p2}));
    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);
    ecs_add(world, e3, Foo);
    ecs_add(world, e4, Foo);

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {{ Foo }, { ecs_childof(p1) }},
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e1, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_childof(p1), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e3, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_childof(p1), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {{ Foo }, { ecs_childof(p2) }},
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e2, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_childof(p2), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e4, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_childof(p2), ecs_field_id(&it, 1));
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

    ecs_entity_t p = ecs_new_w_id(world, EcsOrderedChildren);
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
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_src_1_table_2_children_w_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_entity_t p = ecs_new_w_id(world, EcsOrderedChildren);
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
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_src_1_table_2_children_mixed_parents_w_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_entity_t p1 = ecs_new_w_id(world, EcsOrderedChildren);
    ecs_entity_t p2 = ecs_new_w_id(world, EcsOrderedChildren);
    ecs_entity_t e1 = ecs_insert(world, ecs_value(EcsParent, {p1}));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(EcsParent, {p2}));
    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {{ ecs_childof(EcsWildcard) }},
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e1, it.entities[0]);
        test_uint(ecs_childof(p1), ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e2, it.entities[0]);
        test_uint(ecs_childof(p2), ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));

        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    ecs_fini(world);
}

void NonFragmentingChildOf_this_src_1_table_n_children_mixed_parents_w_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_entity_t p1 = ecs_new_w_id(world, EcsOrderedChildren);
    ecs_entity_t p2 = ecs_new_w_id(world, EcsOrderedChildren);
    ecs_entity_t e1 = ecs_insert(world, ecs_value(EcsParent, {p1}));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(EcsParent, {p2}));
    ecs_entity_t e3 = ecs_insert(world, ecs_value(EcsParent, {p1}));
    ecs_entity_t e4 = ecs_insert(world, ecs_value(EcsParent, {p2}));
    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);
    ecs_add(world, e3, Foo);
    ecs_add(world, e4, Foo);

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {{ ecs_childof(EcsWildcard) }},
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(2, it.count);
        test_uint(e1, it.entities[0]);
        test_uint(e3, it.entities[1]);
        test_uint(ecs_childof(p1), ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_int(2, it.count);
        test_uint(e2, it.entities[0]);
        test_uint(e4, it.entities[1]);
        test_uint(ecs_childof(p2), ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    ecs_fini(world);
}

void NonFragmentingChildOf_this_src_1_table_1_child_w_tag_w_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_entity_t p = ecs_new_w_id(world, EcsOrderedChildren);
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

    ecs_entity_t p = ecs_new_w_id(world, EcsOrderedChildren);
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

    ecs_entity_t p1 = ecs_new_w_id(world, EcsOrderedChildren);
    ecs_entity_t p2 = ecs_new_w_id(world, EcsOrderedChildren);
    ecs_entity_t e1 = ecs_insert(world, ecs_value(EcsParent, {p1}));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(EcsParent, {p2}));
    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);

    {
        ecs_query_t *q = ecs_query(world, {
            .terms = {{ ecs_childof(EcsWildcard) }, { Foo }},
            .cache_kind = cache_kind
        });

        test_assert(q != NULL);

        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e1, it.entities[0]);
        test_uint(ecs_childof(p1), ecs_field_id(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e2, it.entities[0]);
        test_uint(ecs_childof(p2), ecs_field_id(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));

        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }
    ecs_fini(world);
}

void NonFragmentingChildOf_this_src_1_table_n_children_mixed_parents_w_tag_w_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_entity_t p1 = ecs_new_w_id(world, EcsOrderedChildren);
    ecs_entity_t p2 = ecs_new_w_id(world, EcsOrderedChildren);
    ecs_entity_t e1 = ecs_insert(world, ecs_value(EcsParent, {p1}));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(EcsParent, {p2}));
    ecs_entity_t e3 = ecs_insert(world, ecs_value(EcsParent, {p1}));
    ecs_entity_t e4 = ecs_insert(world, ecs_value(EcsParent, {p2}));
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
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e1, it.entities[0]);
        test_uint(ecs_childof(p1), ecs_field_id(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e2, it.entities[0]);
        test_uint(ecs_childof(p2), ecs_field_id(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e3, it.entities[0]);
        test_uint(ecs_childof(p1), ecs_field_id(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e4, it.entities[0]);
        test_uint(ecs_childof(p2), ecs_field_id(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));

        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    ecs_fini(world);
}

void NonFragmentingChildOf_this_src_written_1_table_1_child_w_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_entity_t p = ecs_new_w_id(world, EcsOrderedChildren);
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

    ecs_entity_t p = ecs_new_w_id(world, EcsOrderedChildren);
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

    ecs_entity_t p1 = ecs_new_w_id(world, EcsOrderedChildren);
    ecs_entity_t p2 = ecs_new_w_id(world, EcsOrderedChildren);
    ecs_entity_t e1 = ecs_insert(world, ecs_value(EcsParent, {p1}));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(EcsParent, {p2}));
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
        test_uint(ecs_childof(p1), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e2, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_childof(p2), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_query_next(&it));

        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    ecs_fini(world);
}

void NonFragmentingChildOf_this_src_written_1_table_n_children_mixed_parents_w_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_entity_t p1 = ecs_new_w_id(world, EcsOrderedChildren);
    ecs_entity_t p2 = ecs_new_w_id(world, EcsOrderedChildren);
    ecs_entity_t e1 = ecs_insert(world, ecs_value(EcsParent, {p1}));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(EcsParent, {p2}));
    ecs_entity_t e3 = ecs_insert(world, ecs_value(EcsParent, {p1}));
    ecs_entity_t e4 = ecs_insert(world, ecs_value(EcsParent, {p2}));
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
        test_uint(ecs_childof(p1), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e2, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_childof(p2), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e3, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_childof(p1), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e4, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_childof(p2), ecs_field_id(&it, 1));
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

    ecs_entity_t p = ecs_new_w_id(world, EcsOrderedChildren);
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
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_src_1_table_2_children_w_any(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_entity_t p = ecs_new_w_id(world, EcsOrderedChildren);
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
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NonFragmentingChildOf_this_src_1_table_2_children_mixed_parents_w_any(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_entity_t p1 = ecs_new_w_id(world, EcsOrderedChildren);
    ecs_entity_t p2 = ecs_new_w_id(world, EcsOrderedChildren);
    ecs_entity_t e1 = ecs_insert(world, ecs_value(EcsParent, {p1}));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(EcsParent, {p2}));
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

        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    ecs_fini(world);
}

void NonFragmentingChildOf_this_src_1_table_n_children_mixed_parents_w_any(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_entity_t p1 = ecs_new_w_id(world, EcsOrderedChildren);
    ecs_entity_t p2 = ecs_new_w_id(world, EcsOrderedChildren);
    ecs_entity_t e1 = ecs_insert(world, ecs_value(EcsParent, {p1}));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(EcsParent, {p2}));
    ecs_entity_t e3 = ecs_insert(world, ecs_value(EcsParent, {p1}));
    ecs_entity_t e4 = ecs_insert(world, ecs_value(EcsParent, {p2}));
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

        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    ecs_fini(world);
}

void NonFragmentingChildOf_this_src_1_table_1_child_w_tag_w_any(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_entity_t p = ecs_new_w_id(world, EcsOrderedChildren);
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

    ecs_entity_t p = ecs_new_w_id(world, EcsOrderedChildren);
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

    ecs_entity_t p1 = ecs_new_w_id(world, EcsOrderedChildren);
    ecs_entity_t p2 = ecs_new_w_id(world, EcsOrderedChildren);
    ecs_entity_t e1 = ecs_insert(world, ecs_value(EcsParent, {p1}));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(EcsParent, {p2}));
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

    ecs_entity_t p1 = ecs_new_w_id(world, EcsOrderedChildren);
    ecs_entity_t p2 = ecs_new_w_id(world, EcsOrderedChildren);
    ecs_entity_t e1 = ecs_insert(world, ecs_value(EcsParent, {p1}));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(EcsParent, {p2}));
    ecs_entity_t e3 = ecs_insert(world, ecs_value(EcsParent, {p1}));
    ecs_entity_t e4 = ecs_insert(world, ecs_value(EcsParent, {p2}));
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

void NonFragmentingChildOf_this_src_written_1_table_1_child_w_any(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_entity_t p = ecs_new_w_id(world, EcsOrderedChildren);
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

    ecs_entity_t p = ecs_new_w_id(world, EcsOrderedChildren);
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

    ecs_entity_t p1 = ecs_new_w_id(world, EcsOrderedChildren);
    ecs_entity_t p2 = ecs_new_w_id(world, EcsOrderedChildren);
    ecs_entity_t e1 = ecs_insert(world, ecs_value(EcsParent, {p1}));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(EcsParent, {p2}));
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

    ecs_entity_t p1 = ecs_new_w_id(world, EcsOrderedChildren);
    ecs_entity_t p2 = ecs_new_w_id(world, EcsOrderedChildren);
    ecs_entity_t e1 = ecs_insert(world, ecs_value(EcsParent, {p1}));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(EcsParent, {p2}));
    ecs_entity_t e3 = ecs_insert(world, ecs_value(EcsParent, {p1}));
    ecs_entity_t e4 = ecs_insert(world, ecs_value(EcsParent, {p2}));
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
        test_uint(ecs_childof(p1), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));

        test_bool(false, ecs_query_next(&it));

        ecs_query_fini(q);
    }

    ecs_fini(world);
}
