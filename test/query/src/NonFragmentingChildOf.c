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

}

void NonFragmentingChildOf_this_src_not_childof_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t p = ecs_new_w_id(world, EcsOrderedChildren);
    ecs_entity_t c1 = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_entity_t c2 = ecs_new_w_pair(world, EcsChildOf, p);
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
    ecs_entity_t c1 = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_entity_t c2 = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_entity_t c3 = ecs_new(world);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { ecs_childof(EcsAny), .oper = EcsNot }
        },
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    printf("%s\n", ecs_query_plan(q));

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
    // Implement testcase
}

void NonFragmentingChildOf_this_src_childof_parent_w_tag(void) {
    // Implement testcase
}

void NonFragmentingChildOf_this_src_childof_parent_w_component(void) {
    // Implement testcase
}

void NonFragmentingChildOf_this_src_childof_wildcard(void) {
    // Implement testcase
}

void NonFragmentingChildOf_this_src_childof_wildcard_w_tag(void) {
    // Implement testcase
}

void NonFragmentingChildOf_this_src_childof_wildcard_w_component(void) {
    // Implement testcase
}

void NonFragmentingChildOf_this_src_childof_any(void) {
    // Implement testcase
}

void NonFragmentingChildOf_this_src_childof_var(void) {
    // Implement testcase
}

void NonFragmentingChildOf_this_src_childof_set_var(void) {
    // Implement testcase
}
