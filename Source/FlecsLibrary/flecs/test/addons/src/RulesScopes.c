#include <addons.h>

void RulesScopes_term_w_not_scope_1_term(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Root);
    ECS_TAG(world, TagA);

    ecs_entity_t parent_1 = ecs_new(world, Root);
    ecs_add(world, parent_1, TagA);

    ecs_entity_t parent_2 = ecs_new(world, Root);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "Root, !{ TagA }"
    });
    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(Root, ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(parent_2, it.entities[0]);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesScopes_term_w_not_scope_2_terms(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Root);
    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_entity_t parent_1 = ecs_new(world, Root);
    ecs_add(world, parent_1, TagA);

    ecs_entity_t parent_2 = ecs_new(world, Root);
    ecs_add(world, parent_2, TagB);

    ecs_entity_t parent_3 = ecs_new(world, Root);
    ecs_add(world, parent_3, TagA);
    ecs_add(world, parent_3, TagB);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "Root, !{ TagA, TagB }"
    });
    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(Root, ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(parent_1, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(Root, ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(parent_2, it.entities[0]);

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesScopes_term_w_not_scope_1_term_w_not(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Root);
    ECS_TAG(world, TagA);

    ecs_entity_t parent_1 = ecs_new(world, Root);
    ecs_add(world, parent_1, TagA);

    ecs_new(world, Root);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "Root, !{ !TagA }"
    });
    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(Root, ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(parent_1, it.entities[0]);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesScopes_term_w_not_scope_2_terms_w_not(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Root);
    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_entity_t parent_1 = ecs_new(world, Root);
    ecs_add(world, parent_1, TagA);

    ecs_entity_t parent_2 = ecs_new(world, Root);
    ecs_add(world, parent_2, TagB);

    ecs_entity_t parent_3 = ecs_new(world, Root);
    ecs_add(world, parent_3, TagA);
    ecs_add(world, parent_3, TagB);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "Root, !{ TagA, !TagB }"
    });
    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(Root, ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(parent_2, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(Root, ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(parent_3, it.entities[0]);

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesScopes_term_w_not_scope_1_term_w_var(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Root);

    ecs_entity_t parent_1 = ecs_new(world, Root);
    ecs_new_w_pair(world, EcsChildOf, parent_1);
    ecs_new_w_pair(world, EcsChildOf, parent_1);

    ecs_entity_t parent_2 = ecs_new(world, Root);
    ecs_new_w_pair(world, EcsChildOf, parent_2);

    ecs_entity_t parent_3 = ecs_new(world, Root);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "Root, !{ ChildOf($child, $this) }"
    });
    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(Root, ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(parent_3, it.entities[0]);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesScopes_term_w_not_scope_2_terms_w_var(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Root);
    ECS_COMPONENT(world, Position);

    ecs_entity_t parent_0 = ecs_new(world, Root);
    {
        ecs_entity_t child_1 = ecs_new_w_pair(world, EcsChildOf, parent_0);
        ecs_set(world, child_1, Position, {10, 20});
        ecs_entity_t child_2 = ecs_new_w_pair(world, EcsChildOf, parent_0);
        ecs_set(world, child_2, Position, {10, 20});
    }

    ecs_entity_t parent_1 = ecs_new(world, Root);
    {
        ecs_entity_t child_1 = ecs_new_w_pair(world, EcsChildOf, parent_1);
        ecs_set(world, child_1, Position, {10, 20});
        ecs_new_w_pair(world, EcsChildOf, parent_1);
    }

    ecs_entity_t parent_2 = ecs_new(world, Root);
    {
        ecs_new_w_pair(world, EcsChildOf, parent_2);
        ecs_new_w_pair(world, EcsChildOf, parent_2);
    }

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "Root, !{ ChildOf($child, $this), Position($child) }"
    });
    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(Root, ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(parent_2, it.entities[0]);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesScopes_term_w_not_scope_1_term_w_not_w_var(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Root);

    ecs_entity_t parent_0 = ecs_new(world, Root);
    {
        ecs_new_w_pair(world, EcsChildOf, parent_0);
        ecs_new_w_pair(world, EcsChildOf, parent_0);
    }

    ecs_new(world, Root);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "Root, !{ !ChildOf($child, $this) }"
    });
    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(Root, ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(parent_0, it.entities[0]);

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesScopes_term_w_not_scope_2_terms_w_not_w_var(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Root);
    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Foo);

    ecs_entity_t parent_0 = ecs_new(world, Root);
    {
        ecs_entity_t child_1 = ecs_new_w_pair(world, EcsChildOf, parent_0);
        ecs_set(world, child_1, Position, {10, 20});
        ecs_entity_t child_2 = ecs_new_w_pair(world, EcsChildOf, parent_0);
        ecs_set(world, child_2, Position, {10, 20});
        ecs_entity_t child_3 = ecs_new_w_pair(world, EcsChildOf, parent_0);
        ecs_set(world, child_3, Position, {10, 20});
        ecs_add(world, child_3, Foo);
    }

    ecs_entity_t parent_1 = ecs_new(world, Root);
    {
        ecs_entity_t child_1 = ecs_new_w_pair(world, EcsChildOf, parent_1);
        ecs_set(world, child_1, Position, {10, 20});
        ecs_new_w_pair(world, EcsChildOf, parent_1);
    }

    ecs_entity_t parent_2 = ecs_new(world, Root);
    {
        ecs_entity_t child_1 = ecs_new_w_pair(world, EcsChildOf, parent_2);
        ecs_add(world, child_1, Foo);
        ecs_new_w_pair(world, EcsChildOf, parent_2);
    }

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "Root, !{ ChildOf($child, $this), !Position($child) }"
    });
    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(parent_0, it.entities[0]);
        test_uint(Root, ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesScopes_term_w_not_scope_2_terms_w_or(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Root);
    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_entity_t e1 = ecs_new(world, Root);
    ecs_add(world, e1, TagA);
    ecs_entity_t e2 = ecs_new(world, Root);
    ecs_add(world, e2, TagB);
    ecs_entity_t e3 = ecs_new(world, Root);
    ecs_add(world, e3, TagA);
    ecs_add(world, e3, TagB);
    ecs_entity_t e4 = ecs_new(world, Root);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "Root, !{ TagA || TagB }"
    });
    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(Root, ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(e4, it.entities[0]);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesScopes_term_w_not_scope_3_terms_w_or(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Root);
    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, TagC);

    ecs_entity_t e1 = ecs_new(world, Root);
    ecs_add(world, e1, TagA);
    ecs_entity_t e2 = ecs_new(world, Root);
    ecs_add(world, e2, TagB);
    ecs_entity_t e3 = ecs_new(world, Root);
    ecs_add(world, e3, TagA);
    ecs_add(world, e3, TagB);
    ecs_entity_t e4 = ecs_new(world, Root);
    ecs_add(world, e4, TagC);
    ecs_entity_t e5 = ecs_new(world, Root);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "Root, !{ TagA || TagB || TagC }"
    });
    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(Root, ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(e5, it.entities[0]);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}
