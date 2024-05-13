#include <query.h>

static ecs_query_cache_kind_t cache_kind = EcsQueryCacheDefault;
static ecs_entity_t on_instantiate = 0;

void Scopes_setup(void) {
    const char *cache_param = test_param("cache_kind");
    if (cache_param) {
        if (!strcmp(cache_param, "default")) {
            // already set to default
        } else if (!strcmp(cache_param, "auto")) {
            cache_kind = EcsQueryCacheAuto;
        } else {
            printf("unexpected value for cache_kind '%s'\n", cache_param);
        }
    }

    const char *on_instantiate_param = test_param("on_instantiate");
    if (on_instantiate_param) {
        if (!strcmp(on_instantiate_param, "override")) {
            on_instantiate = EcsOverride;
        } else if (!strcmp(on_instantiate_param, "inherit")) {
            on_instantiate = EcsInherit;
        } else {
            printf("unexpected value for on_instantiate '%s'\n", cache_param);
        }
    }
}

void Scopes_term_w_not_scope_1_term(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Root);
    ecs_add_pair(world, Root, EcsOnInstantiate, on_instantiate);
    ECS_TAG(world, TagA);
    ecs_add_pair(world, TagA, EcsOnInstantiate, on_instantiate);

    ecs_entity_t parent_1 = ecs_new_w(world, Root);
    ecs_add(world, parent_1, TagA);

    ecs_entity_t parent_2 = ecs_new_w(world, Root);

    ecs_query_t *r = ecs_query(world, {
        .expr = "Root, !{ TagA }",
        .cache_kind = cache_kind
    });
    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(Root, ecs_field_id(&it, 0));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(parent_2, it.entities[0]);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void Scopes_term_w_not_scope_2_terms(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Root);
    ecs_add_pair(world, Root, EcsOnInstantiate, on_instantiate);
    ECS_TAG(world, TagA);
    ecs_add_pair(world, TagA, EcsOnInstantiate, on_instantiate);
    ECS_TAG(world, TagB);
    ecs_add_pair(world, TagB, EcsOnInstantiate, on_instantiate);

    ecs_entity_t parent_1 = ecs_new_w(world, Root);
    ecs_add(world, parent_1, TagA);

    ecs_entity_t parent_2 = ecs_new_w(world, Root);
    ecs_add(world, parent_2, TagB);

    ecs_entity_t parent_3 = ecs_new_w(world, Root);
    ecs_add(world, parent_3, TagA);
    ecs_add(world, parent_3, TagB);

    ecs_query_t *r = ecs_query(world, {
        .expr = "Root, !{ TagA, TagB }",
        .cache_kind = cache_kind
    });
    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(Root, ecs_field_id(&it, 0));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(parent_1, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(Root, ecs_field_id(&it, 0));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(parent_2, it.entities[0]);

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void Scopes_term_w_not_scope_1_term_w_not(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Root);
    ecs_add_pair(world, Root, EcsOnInstantiate, on_instantiate);
    ECS_TAG(world, TagA);
    ecs_add_pair(world, TagA, EcsOnInstantiate, on_instantiate);

    ecs_entity_t parent_1 = ecs_new_w(world, Root);
    ecs_add(world, parent_1, TagA);

    ecs_new_w(world, Root);

    ecs_query_t *r = ecs_query(world, {
        .expr = "Root, !{ !TagA }",
        .cache_kind = cache_kind
    });
    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(Root, ecs_field_id(&it, 0));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(parent_1, it.entities[0]);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void Scopes_term_w_not_scope_2_terms_w_not(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Root);
    ecs_add_pair(world, Root, EcsOnInstantiate, on_instantiate);
    ECS_TAG(world, TagA);
    ecs_add_pair(world, TagA, EcsOnInstantiate, on_instantiate);
    ECS_TAG(world, TagB);
    ecs_add_pair(world, TagB, EcsOnInstantiate, on_instantiate);

    ecs_entity_t parent_1 = ecs_new_w(world, Root);
    ecs_add(world, parent_1, TagA);

    ecs_entity_t parent_2 = ecs_new_w(world, Root);
    ecs_add(world, parent_2, TagB);

    ecs_entity_t parent_3 = ecs_new_w(world, Root);
    ecs_add(world, parent_3, TagA);
    ecs_add(world, parent_3, TagB);

    ecs_query_t *r = ecs_query(world, {
        .expr = "Root, !{ TagA, !TagB }",
        .cache_kind = cache_kind
    });
    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(Root, ecs_field_id(&it, 0));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(parent_2, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(Root, ecs_field_id(&it, 0));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(parent_3, it.entities[0]);

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void Scopes_term_w_not_scope_1_term_w_var(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Root);
    ecs_add_pair(world, Root, EcsOnInstantiate, on_instantiate);

    ecs_entity_t parent_1 = ecs_new_w(world, Root);
    ecs_new_w_pair(world, EcsChildOf, parent_1);
    ecs_new_w_pair(world, EcsChildOf, parent_1);

    ecs_entity_t parent_2 = ecs_new_w(world, Root);
    ecs_new_w_pair(world, EcsChildOf, parent_2);

    ecs_entity_t parent_3 = ecs_new_w(world, Root);

    ecs_query_t *r = ecs_query(world, {
        .expr = "Root, !{ ChildOf($child, $this) }",
        .cache_kind = cache_kind
    });
    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(Root, ecs_field_id(&it, 0));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(parent_3, it.entities[0]);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void Scopes_term_w_not_scope_2_terms_w_var(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Root);
    ecs_add_pair(world, Root, EcsOnInstantiate, on_instantiate);
    ECS_COMPONENT(world, Position);

    ecs_entity_t parent_0 = ecs_new_w(world, Root);
    {
        ecs_entity_t child_1 = ecs_new_w_pair(world, EcsChildOf, parent_0);
        ecs_set(world, child_1, Position, {10, 20});
        ecs_entity_t child_2 = ecs_new_w_pair(world, EcsChildOf, parent_0);
        ecs_set(world, child_2, Position, {10, 20});
    }

    ecs_entity_t parent_1 = ecs_new_w(world, Root);
    {
        ecs_entity_t child_1 = ecs_new_w_pair(world, EcsChildOf, parent_1);
        ecs_set(world, child_1, Position, {10, 20});
        ecs_new_w_pair(world, EcsChildOf, parent_1);
    }

    ecs_entity_t parent_2 = ecs_new_w(world, Root);
    {
        ecs_new_w_pair(world, EcsChildOf, parent_2);
        ecs_new_w_pair(world, EcsChildOf, parent_2);
    }

    ecs_query_t *r = ecs_query(world, {
        .expr = "Root, !{ ChildOf($child, $this), Position($child) }",
        .cache_kind = cache_kind
    });
    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(Root, ecs_field_id(&it, 0));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(parent_2, it.entities[0]);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void Scopes_term_w_not_scope_1_term_w_not_w_var(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Root);
    ecs_add_pair(world, Root, EcsOnInstantiate, on_instantiate);

    ecs_entity_t parent_0 = ecs_new_w(world, Root);
    {
        ecs_new_w_pair(world, EcsChildOf, parent_0);
        ecs_new_w_pair(world, EcsChildOf, parent_0);
    }

    ecs_new_w(world, Root);

    ecs_query_t *r = ecs_query(world, {
        .expr = "Root, !{ !ChildOf($child, $this) }",
        .cache_kind = cache_kind
    });
    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void Scopes_term_w_not_scope_2_terms_w_not_w_var(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Root);
    ecs_add_pair(world, Root, EcsOnInstantiate, on_instantiate);
    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Foo);
    ecs_add_pair(world, Foo, EcsOnInstantiate, on_instantiate);

    ecs_entity_t parent_0 = ecs_new_w(world, Root);
    {
        ecs_entity_t child_1 = ecs_new_w_pair(world, EcsChildOf, parent_0);
        ecs_set(world, child_1, Position, {10, 20});
        ecs_entity_t child_2 = ecs_new_w_pair(world, EcsChildOf, parent_0);
        ecs_set(world, child_2, Position, {10, 20});
        ecs_entity_t child_3 = ecs_new_w_pair(world, EcsChildOf, parent_0);
        ecs_set(world, child_3, Position, {10, 20});
        ecs_add(world, child_3, Foo);
    }

    ecs_entity_t parent_1 = ecs_new_w(world, Root);
    {
        ecs_entity_t child_1 = ecs_new_w_pair(world, EcsChildOf, parent_1);
        ecs_set(world, child_1, Position, {10, 20});
        ecs_new_w_pair(world, EcsChildOf, parent_1);
    }

    ecs_entity_t parent_2 = ecs_new_w(world, Root);
    {
        ecs_entity_t child_1 = ecs_new_w_pair(world, EcsChildOf, parent_2);
        ecs_add(world, child_1, Foo);
        ecs_new_w_pair(world, EcsChildOf, parent_2);
    }

    ecs_query_t *r = ecs_query(world, {
        .expr = "Root, !{ ChildOf($child, $this), !Position($child) }",
        .cache_kind = cache_kind
    });
    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(parent_0, it.entities[0]);
        test_uint(Root, ecs_field_id(&it, 0));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void Scopes_term_w_not_scope_2_terms_w_or(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Root);
    ecs_add_pair(world, Root, EcsOnInstantiate, on_instantiate);
    ECS_TAG(world, TagA);
    ecs_add_pair(world, TagA, EcsOnInstantiate, on_instantiate);
    ECS_TAG(world, TagB);
    ecs_add_pair(world, TagB, EcsOnInstantiate, on_instantiate);

    ecs_entity_t e1 = ecs_new_w(world, Root);
    ecs_add(world, e1, TagA);
    ecs_entity_t e2 = ecs_new_w(world, Root);
    ecs_add(world, e2, TagB);
    ecs_entity_t e3 = ecs_new_w(world, Root);
    ecs_add(world, e3, TagA);
    ecs_add(world, e3, TagB);
    ecs_entity_t e4 = ecs_new_w(world, Root);

    ecs_query_t *r = ecs_query(world, {
        .expr = "Root, !{ TagA || TagB }",
        .cache_kind = cache_kind
    });
    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(Root, ecs_field_id(&it, 0));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(e4, it.entities[0]);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void Scopes_term_w_not_scope_3_terms_w_or(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Root);
    ecs_add_pair(world, Root, EcsOnInstantiate, on_instantiate);
    ECS_TAG(world, TagA);
    ecs_add_pair(world, TagA, EcsOnInstantiate, on_instantiate);
    ECS_TAG(world, TagB);
    ecs_add_pair(world, TagB, EcsOnInstantiate, on_instantiate);
    ECS_TAG(world, TagC);
    ecs_add_pair(world, TagC, EcsOnInstantiate, on_instantiate);

    ecs_entity_t e1 = ecs_new_w(world, Root);
    ecs_add(world, e1, TagA);
    ecs_entity_t e2 = ecs_new_w(world, Root);
    ecs_add(world, e2, TagB);
    ecs_entity_t e3 = ecs_new_w(world, Root);
    ecs_add(world, e3, TagA);
    ecs_add(world, e3, TagB);
    ecs_entity_t e4 = ecs_new_w(world, Root);
    ecs_add(world, e4, TagC);
    ecs_entity_t e5 = ecs_new_w(world, Root);

    ecs_query_t *r = ecs_query(world, {
        .expr = "Root, !{ TagA || TagB || TagC }",
        .cache_kind = cache_kind
    });
    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(Root, ecs_field_id(&it, 0));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(e5, it.entities[0]);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void Scopes_term_w_not_scope_2_terms_w_before_after(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);
    ecs_add_pair(world, TagA, EcsOnInstantiate, on_instantiate);
    ECS_TAG(world, TagB);
    ecs_add_pair(world, TagB, EcsOnInstantiate, on_instantiate);

    ecs_query_t *q = ecs_query(world, {
        .expr = "TagA, !{ChildOf($child, $this), TagB($child)}, TagB"
    });

    test_assert(q != NULL);

    ecs_entity_t e1 = ecs_entity(world, { .name = "e1" });
    ecs_entity_t e2 = ecs_entity(world, { .name = "e2" });
    ecs_entity_t e3 = ecs_entity(world, { .name = "e3" });

    ecs_add(world, e1, TagA);
    ecs_add(world, e2, TagA);
    ecs_add(world, e3, TagA);

    ecs_add(world, e1, TagB);
    ecs_add(world, e2, TagB);
    ecs_add(world, e3, TagB);

    ecs_entity_t child_e1_1 = ecs_entity(world, { .name = "child_1" });
    ecs_add_pair(world, child_e1_1, EcsChildOf, e1);

    ecs_entity_t child_e2_1 = ecs_entity(world, { .name = "child_1" });
    ecs_add_pair(world, child_e2_1, EcsChildOf, e2);

    ecs_entity_t child_e2_2 = ecs_entity(world, { .name = "child_2" });
    ecs_add_pair(world, child_e2_2, EcsChildOf, e2);
    ecs_add(world, child_e2_2, TagB);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e1, it.entities[0]);

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e3, it.entities[0]);
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}
