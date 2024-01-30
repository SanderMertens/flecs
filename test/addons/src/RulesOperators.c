#include <addons.h>

void RulesOperators_2_and_not(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "RelA($this), !RelB($this)"
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_entity_t e1 = ecs_new(world, RelA);
    ecs_entity_t e2 = ecs_new(world, RelA);
    ecs_add(world, e1, RelB);
    ecs_add(world, e2, RelB);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_remove(world, e1, RelB);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(RelB, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_field_is_set(&it, 2));
        test_uint(e1, it.entities[0]);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesOperators_3_and_not_not(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, RelC);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "RelA($this), !RelB($this), !RelC($this)"
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_entity_t e1 = ecs_new(world, RelA);
    ecs_entity_t e2 = ecs_new(world, RelA);
    ecs_add(world, e1, RelB);
    ecs_add(world, e2, RelB);
    ecs_add(world, e1, RelC);
    ecs_add(world, e2, RelC);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_remove(world, e1, RelB);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_remove(world, e2, RelC);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_remove(world, e1, RelC);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(RelB, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_field_is_set(&it, 2));
        test_bool(false, ecs_field_is_set(&it, 3));
        test_uint(e1, it.entities[0]);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesOperators_2_and_not_pair_rel_wildcard(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, TgtA);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "RelA($this), !*($this, TgtA)"
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_entity_t e1 = ecs_new(world, RelA);
    ecs_entity_t e2 = ecs_new(world, RelA);
    ecs_add_pair(world, e1, RelA, TgtA);
    ecs_add_pair(world, e2, RelA, TgtA);
    ecs_add_pair(world, e1, RelB, TgtA);
    ecs_add_pair(world, e2, RelB, TgtA);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_remove_pair(world, e1, RelB, TgtA);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_remove_pair(world, e1, RelA, TgtA);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(ecs_pair(EcsWildcard, TgtA), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_field_is_set(&it, 2));
        test_uint(e1, it.entities[0]);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesOperators_2_and_not_pair_tgt_wildcard(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "RelA($this), !RelA($this, *)"
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_entity_t e1 = ecs_new(world, RelA);
    ecs_entity_t e2 = ecs_new(world, RelA);
    ecs_add_pair(world, e1, RelA, TgtA);
    ecs_add_pair(world, e2, RelA, TgtA);
    ecs_add_pair(world, e1, RelA, TgtB);
    ecs_add_pair(world, e2, RelA, TgtB);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_remove_pair(world, e1, RelA, TgtA);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_remove_pair(world, e1, RelA, TgtB);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelA, EcsWildcard), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_field_is_set(&it, 2));
        test_uint(e1, it.entities[0]);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesOperators_2_and_not_pair_rel_tgt_wildcard(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "RelA($this), !*($this, *)"
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_entity_t e1 = ecs_new(world, RelA);
    ecs_entity_t e2 = ecs_new(world, RelA);
    ecs_add_pair(world, e1, RelA, TgtA);
    ecs_add_pair(world, e2, RelA, TgtA);
    ecs_add_pair(world, e1, RelA, TgtB);
    ecs_add_pair(world, e2, RelA, TgtB);
    ecs_add_pair(world, e1, RelB, TgtA);
    ecs_add_pair(world, e2, RelB, TgtA);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_remove_pair(world, e1, RelA, TgtA);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_remove_pair(world, e1, RelA, TgtB);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_remove_pair(world, e1, RelB, TgtA);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(ecs_pair(EcsWildcard, EcsWildcard), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_field_is_set(&it, 2));
        test_uint(e1, it.entities[0]);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesOperators_2_and_not_pair_rel_var(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, TgtA);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "RelA($this), !$x($this, TgtA)"
    });

    test_assert(r != NULL);

    int32_t x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_entity_t e1 = ecs_new(world, RelA);
    ecs_entity_t e2 = ecs_new(world, RelA);
    ecs_add_pair(world, e1, RelA, TgtA);
    ecs_add_pair(world, e2, RelA, TgtA);
    ecs_add_pair(world, e1, RelB, TgtA);
    ecs_add_pair(world, e2, RelB, TgtA);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_remove_pair(world, e1, RelB, TgtA);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_remove_pair(world, e1, RelA, TgtA);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(ecs_pair(EcsWildcard, TgtA), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_field_is_set(&it, 2));
        test_uint(EcsWildcard, ecs_iter_get_var(&it, x_var));
        test_uint(e1, it.entities[0]);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesOperators_2_and_not_pair_tgt_var(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "RelA($this), !RelA($this, $x)"
    });

    test_assert(r != NULL);

    int32_t x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_entity_t e1 = ecs_new(world, RelA);
    ecs_entity_t e2 = ecs_new(world, RelA);
    ecs_add_pair(world, e1, RelA, TgtA);
    ecs_add_pair(world, e2, RelA, TgtA);
    ecs_add_pair(world, e1, RelA, TgtB);
    ecs_add_pair(world, e2, RelA, TgtB);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_remove_pair(world, e1, RelA, TgtA);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_remove_pair(world, e1, RelA, TgtB);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelA, EcsWildcard), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_field_is_set(&it, 2));
        test_uint(EcsWildcard, ecs_iter_get_var(&it, x_var));
        test_uint(e1, it.entities[0]);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesOperators_2_and_not_pair_rel_tgt_var(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, TgtA);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "RelA($this), !$x($this, $y)"
    });

    test_assert(r != NULL);

    int32_t x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != -1);

    int32_t y_var = ecs_rule_find_var(r, "x");
    test_assert(y_var != -1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_entity_t e1 = ecs_new(world, RelA);
    ecs_entity_t e2 = ecs_new(world, RelA);
    ecs_add_pair(world, e1, RelA, TgtA);
    ecs_add_pair(world, e2, RelA, TgtA);
    ecs_add_pair(world, e1, RelB, TgtA);
    ecs_add_pair(world, e2, RelB, TgtA);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_remove_pair(world, e1, RelB, TgtA);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_remove_pair(world, e1, RelA, TgtA);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(ecs_pair(EcsWildcard, EcsWildcard), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_field_is_set(&it, 2));
        test_uint(EcsWildcard, ecs_iter_get_var(&it, x_var));
        test_uint(EcsWildcard, ecs_iter_get_var(&it, y_var));
        test_uint(e1, it.entities[0]);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesOperators_2_and_not_pair_rel_tgt_same_var(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "RelA($this), !$x($this, $x)"
    });

    test_assert(r != NULL);

    int32_t x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_entity_t e1 = ecs_new(world, RelA);
    ecs_entity_t e2 = ecs_new(world, RelA);
    ecs_add_pair(world, e1, RelA, RelA);
    ecs_add_pair(world, e2, RelA, RelA);
    ecs_add_pair(world, e1, RelB, RelB);
    ecs_add_pair(world, e2, RelB, RelB);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_remove_pair(world, e1, RelA, RelA);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_remove_pair(world, e1, RelB, RelB);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(ecs_pair(EcsWildcard, EcsWildcard), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_field_is_set(&it, 2));
        test_uint(EcsWildcard, ecs_iter_get_var(&it, x_var));
        test_uint(e1, it.entities[0]);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesOperators_2_and_not_pair_rel_var_written(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);
    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, TgtA);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "Tag($this, $x), !$x($this, TgtA)"
    });

    test_assert(r != NULL);

    int32_t x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_entity_t e1 = ecs_new_w_pair(world, Tag, RelA);
    ecs_entity_t e2 = ecs_new_w_pair(world, Tag, RelB);
    ecs_add_pair(world, e1, RelA, TgtA);
    ecs_add_pair(world, e1, RelB, TgtA);
    ecs_add_pair(world, e2, RelA, TgtA);
    ecs_add_pair(world, e2, RelB, TgtA);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_remove_pair(world, e1, RelB, TgtA);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_remove_pair(world, e1, RelA, TgtA);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(Tag, RelA), ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_field_is_set(&it, 2));
        test_uint(RelA, ecs_iter_get_var(&it, x_var));
        test_uint(e1, it.entities[0]);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_remove_pair(world, e2, RelB, TgtA);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(Tag, RelA), ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_field_is_set(&it, 2));
        test_uint(RelA, ecs_iter_get_var(&it, x_var));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(Tag, RelB), ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelB, TgtA), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_field_is_set(&it, 2));
        test_uint(RelB, ecs_iter_get_var(&it, x_var));
        test_uint(e2, it.entities[0]);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesOperators_2_and_not_pair_tgt_var_written(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);
    ECS_TAG(world, RelA);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "Tag($this, $x), !RelA($this, $x)"
    });

    test_assert(r != NULL);

    int32_t x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_entity_t e1 = ecs_new_w_pair(world, Tag, TgtA);
    ecs_entity_t e2 = ecs_new_w_pair(world, Tag, TgtB);
    ecs_add_pair(world, e1, RelA, TgtA);
    ecs_add_pair(world, e1, RelA, TgtB);
    ecs_add_pair(world, e2, RelA, TgtA);
    ecs_add_pair(world, e2, RelA, TgtB);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_remove_pair(world, e1, RelA, TgtA);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(Tag, TgtA), ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_field_is_set(&it, 2));
        test_uint(TgtA, ecs_iter_get_var(&it, x_var));
        test_uint(e1, it.entities[0]);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_remove_pair(world, e2, RelA, TgtB);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(Tag, TgtA), ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_field_is_set(&it, 2));
        test_uint(TgtA, ecs_iter_get_var(&it, x_var));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(Tag, TgtB), ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelA, TgtB), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_field_is_set(&it, 2));
        test_uint(TgtB, ecs_iter_get_var(&it, x_var));
        test_uint(e2, it.entities[0]);

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesOperators_2_and_not_pair_rel_tgt_var_written(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);
    ECS_TAG(world, RelA);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "Tag($this, $x), !$x($this, $x)"
    });

    test_assert(r != NULL);

    int32_t x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_entity_t e1 = ecs_new_w_pair(world, Tag, TgtA);
    ecs_entity_t e2 = ecs_new_w_pair(world, Tag, TgtB);
    ecs_add_pair(world, e1, TgtA, TgtA);
    ecs_add_pair(world, e1, TgtB, TgtB);
    ecs_add_pair(world, e2, TgtA, TgtA);
    ecs_add_pair(world, e2, TgtB, TgtB);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_remove_pair(world, e1, TgtA, TgtA);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(Tag, TgtA), ecs_field_id(&it, 1));
        test_uint(ecs_pair(TgtA, TgtA), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_field_is_set(&it, 2));
        test_uint(TgtA, ecs_iter_get_var(&it, x_var));
        test_uint(e1, it.entities[0]);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_remove_pair(world, e2, TgtB, TgtB);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(Tag, TgtA), ecs_field_id(&it, 1));
        test_uint(ecs_pair(TgtA, TgtA), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_field_is_set(&it, 2));
        test_uint(TgtA, ecs_iter_get_var(&it, x_var));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(Tag, TgtB), ecs_field_id(&it, 1));
        test_uint(ecs_pair(TgtB, TgtB), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_field_is_set(&it, 2));
        test_uint(TgtB, ecs_iter_get_var(&it, x_var));
        test_uint(e2, it.entities[0]);

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesOperators_2_and_not_pair_rel_tgt_same_var_written(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);
    ECS_TAG(world, RelA);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "Tag($this, $x), !$x($this, $this)"
    });

    test_assert(r != NULL);

    int32_t x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_entity_t e1 = ecs_new_w_pair(world, Tag, TgtA);
    ecs_entity_t e2 = ecs_new_w_pair(world, Tag, TgtB);
    ecs_add_pair(world, e1, TgtA, e1);
    ecs_add_pair(world, e1, TgtB, e1);
    ecs_add_pair(world, e2, TgtA, e2);
    ecs_add_pair(world, e2, TgtB, e2);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_remove_pair(world, e1, TgtA, e1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(Tag, TgtA), ecs_field_id(&it, 1));
        test_uint(ecs_pair(TgtA, e1), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_field_is_set(&it, 2));
        test_uint(TgtA, ecs_iter_get_var(&it, x_var));
        test_uint(e1, it.entities[0]);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_remove_pair(world, e2, TgtB, e2);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(Tag, TgtA), ecs_field_id(&it, 1));
        test_uint(ecs_pair(TgtA, e1), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_field_is_set(&it, 2));
        test_uint(TgtA, ecs_iter_get_var(&it, x_var));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(Tag, TgtB), ecs_field_id(&it, 1));
        test_uint(ecs_pair(TgtB, e2), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_field_is_set(&it, 2));
        test_uint(TgtB, ecs_iter_get_var(&it, x_var));
        test_uint(e2, it.entities[0]);

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}


void RulesOperators_2_and_not_pair_rel_src_tgt_same_var_written(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "RelA($x), !$x($x, $x)"
    });

    test_assert(r != NULL);

    int32_t x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_entity_t e1 = ecs_new(world, RelA);
    ecs_entity_t e2 = ecs_new(world, RelA);
    ecs_add_pair(world, e1, e1, e1);
    ecs_add_pair(world, e2, e2, e2);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_remove_pair(world, e1, e1, e1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(ecs_pair(e1, e1), ecs_field_id(&it, 2));
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(e1, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_field_is_set(&it, 2));
        test_uint(e1, ecs_iter_get_var(&it, x_var));
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_remove_pair(world, e2, e2, e2);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(ecs_pair(e1, e1), ecs_field_id(&it, 2));
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(e1, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_field_is_set(&it, 2));
        test_uint(e1, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(ecs_pair(e2, e2), ecs_field_id(&it, 2));
        test_uint(e2, ecs_field_src(&it, 1));
        test_uint(e2, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_field_is_set(&it, 2));
        test_uint(e2, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesOperators_2_and_not_pair_any_rel(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, TgtA);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "RelA($this), !_($this, TgtA)"
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_entity_t e1 = ecs_new(world, RelA);
    ecs_entity_t e2 = ecs_new(world, RelA);
    ecs_entity_t e3 = ecs_new(world, RelA);

    ecs_add_pair(world, e2, RelA, TgtA);
    ecs_add_pair(world, e3, RelA, TgtA);
    ecs_add_pair(world, e3, RelB, TgtA);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(ecs_pair(EcsWildcard, TgtA), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_field_is_set(&it, 2));
        test_uint(e1, it.entities[0]);

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesOperators_2_and_not_pair_any_tgt(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "RelA($this), !RelA($this, _)"
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_entity_t e1 = ecs_new(world, RelA);
    ecs_entity_t e2 = ecs_new(world, RelA);
    ecs_entity_t e3 = ecs_new(world, RelA);

    ecs_add_pair(world, e2, RelA, TgtA);
    ecs_add_pair(world, e3, RelA, TgtA);
    ecs_add_pair(world, e3, RelA, TgtB);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelA, EcsWildcard), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_field_is_set(&it, 2));
        test_uint(e1, it.entities[0]);

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesOperators_2_and_not_pair_any_src(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);
    ECS_TAG(world, RelA);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);
    ECS_TAG(world, TgtC);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "Tag($this, $x), !RelA(_, $x)"
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_entity_t e1 = ecs_new_w_pair(world, Tag, TgtA);
    ecs_new_w_pair(world, Tag, TgtB);
    ecs_new_w_pair(world, Tag, TgtC);

    ecs_entity_t e4 = ecs_new_w_pair(world, RelA, TgtB);
    ecs_add_pair(world, e4, RelA, TgtC);
    ecs_new_w_pair(world, RelA, TgtC);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(Tag, TgtA), ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_field_is_set(&it, 2));
        test_uint(e1, it.entities[0]);

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesOperators_1_not_any_src_fixed_first(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "!RelA(_)"
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(false, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_new(world, RelA);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_new(world, RelA);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesOperators_1_not_any_src_any_tgt_fixed_first(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "!RelA(_, _)"
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(RelA, EcsWildcard), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(false, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_new_w_pair(world, TgtA, RelA);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(RelA, EcsWildcard), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(false, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_new_w_pair(world, RelA, TgtA);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_new_w_pair(world, RelA, TgtB);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesOperators_1_not_any_src_any_first_fixed_tgt(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, TgtA);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "!_(_, TgtA)"
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(EcsWildcard, TgtA), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(false, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_new_w_pair(world, TgtA, RelA);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(EcsWildcard, TgtA), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(false, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_new_w_pair(world, RelA, TgtA);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_new_w_pair(world, RelB, TgtA);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesOperators_1_not_any_src_any_childof_pair_any_tgt(void) {
    ecs_world_t *world = ecs_init();

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "!ChildOf(_, _)"
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesOperators_1_not_any_src_any_isa_pair_any_tgt(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TgtA);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "!IsA(_, _)"
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(EcsIsA, EcsWildcard), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(false, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_new_w_pair(world, EcsIsA, TgtA);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesOperators_2_and_optional(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "RelA($this), ?RelB($this)"
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_entity_t e1 = ecs_new(world, RelA);
    ecs_entity_t e2 = ecs_new(world, RelA);
    ecs_add(world, e2, RelB);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);   
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(RelB, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_field_is_set(&it, 2));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(RelB, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(e2, it.entities[0]);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesOperators_3_and_optional_optional(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, RelC);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "RelA($this), ?RelB($this), ?RelC($this)"
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_entity_t e1 = ecs_new(world, RelA);
    ecs_entity_t e2 = ecs_new(world, RelA);
    ecs_entity_t e3 = ecs_new(world, RelA);
    ecs_add(world, e2, RelB);
    ecs_add(world, e3, RelC);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(RelB, ecs_field_id(&it, 2));
        test_uint(RelC, ecs_field_id(&it, 3));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(0, ecs_field_src(&it, 3));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_field_is_set(&it, 2));
        test_bool(false, ecs_field_is_set(&it, 3));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(RelB, ecs_field_id(&it, 2));
        test_uint(RelC, ecs_field_id(&it, 3));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(0, ecs_field_src(&it, 3));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_bool(false, ecs_field_is_set(&it, 3));
        test_uint(e2, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(RelB, ecs_field_id(&it, 2));
        test_uint(RelC, ecs_field_id(&it, 3));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(0, ecs_field_src(&it, 3));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_field_is_set(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 3));
        test_uint(e3, it.entities[0]);

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesOperators_2_and_optional_pair_rel_wildcard(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, TgtA);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "RelA($this), ?*($this, TgtA)"
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_entity_t e1 = ecs_new(world, RelA);
    ecs_entity_t e2 = ecs_new(world, RelA);
    ecs_entity_t e3 = ecs_new(world, RelA);
    ecs_add_pair(world, e2, RelA, TgtA);
    ecs_add_pair(world, e3, RelA, TgtA);
    ecs_add_pair(world, e3, RelB, TgtA);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(ecs_pair(EcsWildcard, TgtA), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_field_is_set(&it, 2));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(e2, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(e3, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelB, TgtA), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(e3, it.entities[0]);

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesOperators_2_and_optional_pair_tgt_wildcard(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "RelA($this), ?RelA($this, *)"
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_entity_t e1 = ecs_new(world, RelA);
    ecs_entity_t e2 = ecs_new(world, RelA);
    ecs_entity_t e3 = ecs_new(world, RelA);
    ecs_add_pair(world, e2, RelA, TgtA);
    ecs_add_pair(world, e3, RelA, TgtA);
    ecs_add_pair(world, e3, RelA, TgtB);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelA, EcsWildcard), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_field_is_set(&it, 2));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(e2, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(e3, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelA, TgtB), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(e3, it.entities[0]);

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesOperators_2_and_optional_pair_rel_var(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, TgtA);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "RelA($this), ?$x($this, TgtA)"
    });

    test_assert(r != NULL);

    int x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_entity_t e1 = ecs_new(world, RelA);
    ecs_entity_t e2 = ecs_new(world, RelA);
    ecs_entity_t e3 = ecs_new(world, RelA);
    ecs_add_pair(world, e2, RelA, TgtA);
    ecs_add_pair(world, e3, RelA, TgtA);
    ecs_add_pair(world, e3, RelB, TgtA);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(ecs_pair(EcsWildcard, TgtA), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_field_is_set(&it, 2));
        test_uint(EcsWildcard, ecs_iter_get_var(&it, x_var));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(RelA, ecs_iter_get_var(&it, x_var));
        test_uint(e2, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(RelA, ecs_iter_get_var(&it, x_var));
        test_uint(e3, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelB, TgtA), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(RelB, ecs_iter_get_var(&it, x_var));
        test_uint(e3, it.entities[0]);

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesOperators_2_and_optional_pair_tgt_var(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "RelA($this), ?RelA($this, $x)"
    });

    test_assert(r != NULL);

    int x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_entity_t e1 = ecs_new(world, RelA);
    ecs_entity_t e2 = ecs_new(world, RelA);
    ecs_entity_t e3 = ecs_new(world, RelA);
    ecs_add_pair(world, e2, RelA, TgtA);
    ecs_add_pair(world, e3, RelA, TgtA);
    ecs_add_pair(world, e3, RelA, TgtB);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelA, EcsWildcard), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_field_is_set(&it, 2));
        test_uint(EcsWildcard, ecs_iter_get_var(&it, x_var));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(TgtA, ecs_iter_get_var(&it, x_var));
        test_uint(e2, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(TgtA, ecs_iter_get_var(&it, x_var));
        test_uint(e3, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelA, TgtB), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(TgtB, ecs_iter_get_var(&it, x_var));
        test_uint(e3, it.entities[0]);

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesOperators_2_and_optional_pair_rel_tgt_var(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "RelA($this), ?$x($this, $y)"
    });

    test_assert(r != NULL);

    int x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != -1);

    int y_var = ecs_rule_find_var(r, "y");
    test_assert(y_var != -1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_entity_t e1 = ecs_new(world, RelA);
    ecs_entity_t e2 = ecs_new(world, RelA);
    ecs_entity_t e3 = ecs_new(world, RelA);
    ecs_add_pair(world, e2, RelA, TgtA);
    ecs_add_pair(world, e3, RelA, TgtA);
    ecs_add_pair(world, e3, RelA, TgtB);
    ecs_add_pair(world, e3, RelB, TgtA);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(ecs_pair(EcsWildcard, EcsWildcard), ecs_field_id(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_field_is_set(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(EcsWildcard, ecs_iter_get_var(&it, x_var));
        test_uint(EcsWildcard, ecs_iter_get_var(&it, y_var));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(RelA, ecs_iter_get_var(&it, x_var));
        test_uint(TgtA, ecs_iter_get_var(&it, y_var));
        test_uint(e2, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(RelA, ecs_iter_get_var(&it, x_var));
        test_uint(TgtA, ecs_iter_get_var(&it, y_var));
        test_uint(e3, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelA, TgtB), ecs_field_id(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(RelA, ecs_iter_get_var(&it, x_var));
        test_uint(TgtB, ecs_iter_get_var(&it, y_var));
        test_uint(e3, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelB, TgtA), ecs_field_id(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(RelB, ecs_iter_get_var(&it, x_var));
        test_uint(TgtA, ecs_iter_get_var(&it, y_var));
        test_uint(e3, it.entities[0]);

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesOperators_2_and_optional_pair_rel_tgt_same_var(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "RelA($this), ?$x($this, $x)"
    });

    test_assert(r != NULL);

    int x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_entity_t e1 = ecs_new(world, RelA);
    ecs_entity_t e2 = ecs_new(world, RelA);
    ecs_entity_t e3 = ecs_new(world, RelA);

    ecs_add_pair(world, e1, RelA, TgtA);
    ecs_add_pair(world, e2, RelA, TgtA);
    ecs_add_pair(world, e3, RelA, TgtA);

    ecs_add_pair(world, e2, TgtA, TgtA);
    ecs_add_pair(world, e3, TgtA, TgtA);
    ecs_add_pair(world, e3, RelA, RelA);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(e1, it.entities[0]);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(ecs_pair(EcsWildcard, EcsWildcard), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_field_is_set(&it, 2));
        test_uint(EcsWildcard, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(e2, it.entities[0]);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(ecs_pair(TgtA, TgtA), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(TgtA, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(e3, it.entities[0]);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelA, RelA), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(RelA, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(e3, it.entities[0]);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(ecs_pair(TgtA, TgtA), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(TgtA, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesOperators_2_and_optional_pair_rel_var_written(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);
    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, TgtA);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "Tag($this, $x), ?$x($this, TgtA)"
    });

    test_assert(r != NULL);

    int x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
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
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(Tag, RelA), ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_field_is_set(&it, 2));
        test_uint(RelA, ecs_iter_get_var(&it, x_var));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(Tag, RelA), ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(RelA, ecs_iter_get_var(&it, x_var));
        test_uint(e2, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(Tag, RelA), ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(RelA, ecs_iter_get_var(&it, x_var));
        test_uint(e3, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(Tag, RelB), ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelB, TgtA), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(RelB, ecs_iter_get_var(&it, x_var));
        test_uint(e3, it.entities[0]);

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesOperators_2_and_optional_pair_tgt_var_written(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);
    ECS_TAG(world, RelA);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "Tag($this, $x), ?RelA($this, $x)"
    });

    test_assert(r != NULL);

    int x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
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
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(Tag, TgtA), ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_field_is_set(&it, 2));
        test_uint(TgtA, ecs_iter_get_var(&it, x_var));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(Tag, TgtA), ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(TgtA, ecs_iter_get_var(&it, x_var));
        test_uint(e2, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(Tag, TgtA), ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(TgtA, ecs_iter_get_var(&it, x_var));
        test_uint(e3, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(Tag, TgtB), ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelA, TgtB), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(TgtB, ecs_iter_get_var(&it, x_var));
        test_uint(e3, it.entities[0]);

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesOperators_2_and_optional_pair_rel_tgt_var_written(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);
    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "Tag($this, $x), ?$x($this, $y)"
    });

    test_assert(r != NULL);

    int x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != -1);

    int y_var = ecs_rule_find_var(r, "y");
    test_assert(y_var != -1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
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
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(Tag, RelA), ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelA, EcsWildcard), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_field_is_set(&it, 2));
        test_uint(RelA, ecs_iter_get_var(&it, x_var));
        test_uint(EcsWildcard, ecs_iter_get_var(&it, y_var));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(Tag, RelA), ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(RelA, ecs_iter_get_var(&it, x_var));
        test_uint(TgtA, ecs_iter_get_var(&it, y_var));
        test_uint(e2, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(Tag, RelA), ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(RelA, ecs_iter_get_var(&it, x_var));
        test_uint(TgtA, ecs_iter_get_var(&it, y_var));
        test_uint(e3, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(Tag, RelA), ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelA, TgtB), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(RelA, ecs_iter_get_var(&it, x_var));
        test_uint(TgtB, ecs_iter_get_var(&it, y_var));
        test_uint(e3, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(Tag, RelB), ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelB, TgtA), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(RelB, ecs_iter_get_var(&it, x_var));
        test_uint(TgtA, ecs_iter_get_var(&it, y_var));
        test_uint(e3, it.entities[0]);

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesOperators_2_and_optional_pair_rel_tgt_same_var_written(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);
    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "Tag($this, $x), ?$x($this, $x)"
    });

    test_assert(r != NULL);

    int x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
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
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(Tag, TgtA), ecs_field_id(&it, 1));
        test_uint(ecs_pair(TgtA, TgtA), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_field_is_set(&it, 2));
        test_uint(TgtA, ecs_iter_get_var(&it, x_var));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(Tag, TgtA), ecs_field_id(&it, 1));
        test_uint(ecs_pair(TgtA, TgtA), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(TgtA, ecs_iter_get_var(&it, x_var));
        test_uint(e2, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(Tag, RelA), ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelA, RelA), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(RelA, ecs_iter_get_var(&it, x_var));
        test_uint(e3, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(Tag, TgtA), ecs_field_id(&it, 1));
        test_uint(ecs_pair(TgtA, TgtA), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(TgtA, ecs_iter_get_var(&it, x_var));
        test_uint(e3, it.entities[0]);

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesOperators_2_and_optional_pair_rel_src_tgt_same_var_written(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "RelA($x), ?$x($x, $x)"
    });

    test_assert(r != NULL);

    int32_t x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_entity_t e1 = ecs_new(world, RelA);
    ecs_entity_t e2 = ecs_new(world, RelA);
    ecs_entity_t e3 = ecs_new(world, RelA);
    ecs_add_pair(world, e2, e2, e2);
    ecs_add_pair(world, e3, e3, e3);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(ecs_pair(e1, e1), ecs_field_id(&it, 2));
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(e1, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_field_is_set(&it, 2));
        test_uint(e1, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(ecs_pair(e2, e2), ecs_field_id(&it, 2));
        test_uint(e2, ecs_field_src(&it, 1));
        test_uint(e2, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(e2, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(ecs_pair(e3, e3), ecs_field_id(&it, 2));
        test_uint(e3, ecs_field_src(&it, 1));
        test_uint(e3, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(e3, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesOperators_3_and_optional_optional_pair_w_var(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, RelC);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "RelA($this), ?RelB($this, $x), ?RelC($this, $y)"
    });

    test_assert(r != NULL);

    int32_t x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != -1);

    int32_t y_var = ecs_rule_find_var(r, "y");
    test_assert(y_var != -1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_entity_t e1 = ecs_new(world, RelA);
    ecs_entity_t e2 = ecs_new(world, RelA);
    ecs_entity_t e3 = ecs_new(world, RelA);
    ecs_add_pair(world, e2, RelB, TgtA);
    ecs_add_pair(world, e3, RelC, TgtB);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelB, EcsWildcard), ecs_field_id(&it, 2));
        test_uint(ecs_pair(RelC, EcsWildcard), ecs_field_id(&it, 3));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(0, ecs_field_src(&it, 3));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_field_is_set(&it, 2));
        test_bool(false, ecs_field_is_set(&it, 3));
        test_uint(EcsWildcard, ecs_iter_get_var(&it, x_var));
        test_uint(EcsWildcard, ecs_iter_get_var(&it, y_var));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelB, TgtA), ecs_field_id(&it, 2));
        test_uint(ecs_pair(RelC, EcsWildcard), ecs_field_id(&it, 3));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(0, ecs_field_src(&it, 3));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_bool(false, ecs_field_is_set(&it, 3));
        test_uint(TgtA, ecs_iter_get_var(&it, x_var));
        test_uint(EcsWildcard, ecs_iter_get_var(&it, y_var));
        test_uint(e2, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelB, EcsWildcard), ecs_field_id(&it, 2));
        test_uint(ecs_pair(RelC, TgtB), ecs_field_id(&it, 3));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(0, ecs_field_src(&it, 3));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_field_is_set(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 3));
        test_uint(EcsWildcard, ecs_iter_get_var(&it, x_var));
        test_uint(TgtB, ecs_iter_get_var(&it, y_var));
        test_uint(e3, it.entities[0]);

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesOperators_2_and_optional_pair_any_rel(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, TgtA);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "RelA($this), ?_($this, TgtA)"
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_entity_t e1 = ecs_new(world, RelA);
    ecs_entity_t e2 = ecs_new(world, RelA);
    ecs_entity_t e3 = ecs_new(world, RelA);

    ecs_add_pair(world, e2, RelA, TgtA);
    ecs_add_pair(world, e3, RelA, TgtA);
    ecs_add_pair(world, e3, RelB, TgtA);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(ecs_pair(EcsWildcard, TgtA), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_field_is_set(&it, 2));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(ecs_pair(EcsWildcard, TgtA), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(e2, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(ecs_pair(EcsWildcard, TgtA), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(e3, it.entities[0]);

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesOperators_2_and_optional_pair_any_tgt(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "RelA($this), ?RelA($this, _)"
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_entity_t e1 = ecs_new(world, RelA);
    ecs_entity_t e2 = ecs_new(world, RelA);
    ecs_entity_t e3 = ecs_new(world, RelA);

    ecs_add_pair(world, e2, RelA, TgtA);
    ecs_add_pair(world, e3, RelA, TgtA);
    ecs_add_pair(world, e3, RelA, TgtB);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelA, EcsWildcard), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_field_is_set(&it, 2));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelA, EcsWildcard), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(e2, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelA, EcsWildcard), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(e3, it.entities[0]);

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesOperators_2_and_optional_pair_any_src(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);
    ECS_TAG(world, RelA);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);
    ECS_TAG(world, TgtC);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "Tag($this, $x), ?RelA(_, $x)"
    });

    test_assert(r != NULL);

    int32_t x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_entity_t e1 = ecs_new_w_pair(world, Tag, TgtA);
    ecs_entity_t e2 = ecs_new_w_pair(world, Tag, TgtB);
    ecs_entity_t e3 = ecs_new_w_pair(world, Tag, TgtC);

    ecs_entity_t e4 = ecs_new_w_pair(world, RelA, TgtB);
    ecs_add_pair(world, e4, RelA, TgtC);
    ecs_new_w_pair(world, RelA, TgtC);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(Tag, TgtA), ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_field_is_set(&it, 2));
        test_uint(TgtA, ecs_iter_get_var(&it, x_var));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(Tag, TgtB), ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelA, TgtB), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(EcsWildcard, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(TgtB, ecs_iter_get_var(&it, x_var));
        test_uint(e2, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(Tag, TgtC), ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelA, TgtC), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(EcsWildcard, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(TgtC, ecs_iter_get_var(&it, x_var));
        test_uint(e3, it.entities[0]);

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesOperators_3_and_optional_dependent_and_pair_rel(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "RelA($this), ?$x($this, TgtA), $x($this, TgtB)"
    });

    test_assert(r != NULL);

    int x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != 0);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_entity_t e1 = ecs_new(world, RelA);
    ecs_entity_t e2 = ecs_new(world, RelA);
    ecs_entity_t e3 = ecs_new(world, RelA);
    ecs_entity_t e4 = ecs_new(world, RelA);

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
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(ecs_pair(EcsWildcard, TgtA), ecs_field_id(&it, 2));
        test_uint(ecs_pair(EcsWildcard, TgtB), ecs_field_id(&it, 3));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(0, ecs_field_src(&it, 3));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_field_is_set(&it, 2));
        test_bool(false, ecs_field_is_set(&it, 3));
        test_uint(EcsWildcard, ecs_iter_get_var(&it, x_var));
        test_uint(e2, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 2));
        test_uint(ecs_pair(RelA, TgtB), ecs_field_id(&it, 3));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(0, ecs_field_src(&it, 3));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 3));
        test_uint(RelA, ecs_iter_get_var(&it, x_var));
        test_uint(e3, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 2));
        test_uint(ecs_pair(RelA, TgtB), ecs_field_id(&it, 3));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(0, ecs_field_src(&it, 3));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 3));
        test_uint(RelA, ecs_iter_get_var(&it, x_var));
        test_uint(e4, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelB, TgtA), ecs_field_id(&it, 2));
        test_uint(ecs_pair(RelB, TgtB), ecs_field_id(&it, 3));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(0, ecs_field_src(&it, 3));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 3));
        test_uint(RelB, ecs_iter_get_var(&it, x_var));
        test_uint(e4, it.entities[0]);

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesOperators_3_and_optional_dependent_and_pair_tgt(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "RelA($this), ?RelA($this, $x), RelB($this, $x)"
    });

    test_assert(r != NULL);

    int x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != 0);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_entity_t e1 = ecs_new(world, RelA);
    ecs_entity_t e2 = ecs_new(world, RelA);
    ecs_entity_t e3 = ecs_new(world, RelA);
    ecs_entity_t e4 = ecs_new(world, RelA);

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
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelA, EcsWildcard), ecs_field_id(&it, 2));
        test_uint(ecs_pair(RelB, EcsWildcard), ecs_field_id(&it, 3));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(0, ecs_field_src(&it, 3));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_field_is_set(&it, 2));
        test_bool(false, ecs_field_is_set(&it, 3));
        test_uint(EcsWildcard, ecs_iter_get_var(&it, x_var));
        test_uint(e2, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 2));
        test_uint(ecs_pair(RelB, TgtA), ecs_field_id(&it, 3));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(0, ecs_field_src(&it, 3));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 3));
        test_uint(TgtA, ecs_iter_get_var(&it, x_var));
        test_uint(e3, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 2));
        test_uint(ecs_pair(RelB, TgtA), ecs_field_id(&it, 3));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(0, ecs_field_src(&it, 3));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 3));
        test_uint(TgtA, ecs_iter_get_var(&it, x_var));
        test_uint(e4, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelA, TgtB), ecs_field_id(&it, 2));
        test_uint(ecs_pair(RelB, TgtB), ecs_field_id(&it, 3));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(0, ecs_field_src(&it, 3));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 3));
        test_uint(TgtB, ecs_iter_get_var(&it, x_var));
        test_uint(e4, it.entities[0]);

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesOperators_3_and_optional_dependent_and_pair_rel_tgt(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "RelA($this), ?$x($this, $y), $y($this, $x)"
    });

    test_assert(r != NULL);

    int x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != 0);

    int y_var = ecs_rule_find_var(r, "y");
    test_assert(y_var != 0);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_entity_t e1 = ecs_new(world, RelA);
    ecs_entity_t e2 = ecs_new(world, RelA);
    ecs_entity_t e3 = ecs_new(world, RelA);

    ecs_add_pair(world, e2, RelA, TgtA);

    ecs_add_pair(world, e3, RelA, TgtA);
    ecs_add_pair(world, e3, TgtA, RelA);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(ecs_pair(EcsWildcard, EcsWildcard), ecs_field_id(&it, 2));
        test_uint(ecs_pair(EcsWildcard, EcsWildcard), ecs_field_id(&it, 3));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(0, ecs_field_src(&it, 3));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_field_is_set(&it, 2));
        test_bool(false, ecs_field_is_set(&it, 3));
        test_uint(EcsWildcard, ecs_iter_get_var(&it, x_var));
        test_uint(EcsWildcard, ecs_iter_get_var(&it, y_var));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 2));
        test_uint(ecs_pair(TgtA, RelA), ecs_field_id(&it, 3));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(0, ecs_field_src(&it, 3));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 3));
        test_uint(RelA, ecs_iter_get_var(&it, x_var));
        test_uint(TgtA, ecs_iter_get_var(&it, y_var));
        test_uint(e3, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(ecs_pair(TgtA, RelA), ecs_field_id(&it, 2));
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 3));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(0, ecs_field_src(&it, 3));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 3));
        test_uint(TgtA, ecs_iter_get_var(&it, x_var));
        test_uint(RelA, ecs_iter_get_var(&it, y_var));
        test_uint(e3, it.entities[0]);

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesOperators_3_and_optional_dependent_and_pair_rel_tgt_same_var(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "RelA($this), ?RelA($this, $x), $x($this, $x)"
    });

    test_assert(r != NULL);

    int x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != 0);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_entity_t e1 = ecs_new(world, RelA);
    ecs_entity_t e2 = ecs_new(world, RelA);
    ecs_entity_t e3 = ecs_new(world, RelA);

    ecs_add_pair(world, e2, RelA, TgtA);

    ecs_add_pair(world, e3, RelA, TgtA);
    ecs_add_pair(world, e3, TgtA, TgtA);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelA, EcsWildcard), ecs_field_id(&it, 2));
        test_uint(ecs_pair(EcsWildcard, EcsWildcard), ecs_field_id(&it, 3));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(0, ecs_field_src(&it, 3));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_field_is_set(&it, 2));
        test_bool(false, ecs_field_is_set(&it, 3));
        test_uint(EcsWildcard, ecs_iter_get_var(&it, x_var));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 2));
        test_uint(ecs_pair(TgtA, TgtA), ecs_field_id(&it, 3));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(0, ecs_field_src(&it, 3));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 3));
        test_uint(TgtA, ecs_iter_get_var(&it, x_var));
        test_uint(e3, it.entities[0]);

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesOperators_3_and_optional_dependent_and_pair_rel_tgt_same_other_var(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);
    ECS_TAG(world, TgtC);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "RelA($this), ?RelA($this, $x), $y($x, $y)"
    });

    test_assert(r != NULL);

    int x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != 0);

    int y_var = ecs_rule_find_var(r, "y");
    test_assert(y_var != 0);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_entity_t e1 = ecs_new(world, RelA);
    ecs_entity_t e2 = ecs_new(world, RelA);
    ecs_entity_t e3 = ecs_new(world, RelA);

    ecs_entity_t t1 = ecs_new_id(world);
    ecs_entity_t t2 = ecs_new_w_pair(world, TgtA, TgtA);
    ecs_entity_t t3 = ecs_new_w_pair(world, TgtB, TgtB);
    ecs_add_pair(world, t3, TgtC, TgtC);

    ecs_add_pair(world, e2, RelA, t1);
    ecs_add_pair(world, e3, RelA, t2);
    ecs_add_pair(world, e3, RelA, t3);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelA, EcsWildcard), ecs_field_id(&it, 2));
        test_uint(ecs_pair(EcsWildcard, EcsWildcard), ecs_field_id(&it, 3));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(EcsWildcard, ecs_field_src(&it, 3));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_field_is_set(&it, 2));
        test_bool(false, ecs_field_is_set(&it, 3));
        test_uint(EcsWildcard, ecs_iter_get_var(&it, x_var));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelA, t2), ecs_field_id(&it, 2));
        test_uint(ecs_pair(TgtA, TgtA), ecs_field_id(&it, 3));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(t2, ecs_field_src(&it, 3));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 3));
        test_uint(t2, ecs_iter_get_var(&it, x_var));
        test_uint(e3, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelA, t3), ecs_field_id(&it, 2));
        test_uint(ecs_pair(TgtB, TgtB), ecs_field_id(&it, 3));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(t3, ecs_field_src(&it, 3));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 3));
        test_uint(t3, ecs_iter_get_var(&it, x_var));
        test_uint(e3, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelA, t3), ecs_field_id(&it, 2));
        test_uint(ecs_pair(TgtC, TgtC), ecs_field_id(&it, 3));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(t3, ecs_field_src(&it, 3));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 3));
        test_uint(t3, ecs_iter_get_var(&it, x_var));
        test_uint(e3, it.entities[0]);

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesOperators_3_and_optional_dependent_and_pair_src(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);
    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "RelA($this), ?RelA($this, $x), Tag($x)"
    });

    test_assert(r != NULL);

    int x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != 0);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_entity_t e1 = ecs_new(world, RelA);
    ecs_entity_t e2 = ecs_new(world, RelA);
    ecs_entity_t e3 = ecs_new(world, RelA);

    ecs_entity_t t1 = ecs_new_id(world);
    ecs_entity_t t2 = ecs_new(world, Tag);
    ecs_entity_t t3 = ecs_new(world, Tag);

    ecs_add_pair(world, e2, RelA, t1);
    ecs_add_pair(world, e3, RelA, t2);
    ecs_add_pair(world, e3, RelA, t3);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelA, EcsWildcard), ecs_field_id(&it, 2));
        test_uint(Tag, ecs_field_id(&it, 3));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(EcsWildcard, ecs_field_src(&it, 3));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_field_is_set(&it, 2));
        test_bool(false, ecs_field_is_set(&it, 3));
        test_uint(EcsWildcard, ecs_iter_get_var(&it, x_var));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelA, t2), ecs_field_id(&it, 2));
        test_uint(Tag, ecs_field_id(&it, 3));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(t2, ecs_field_src(&it, 3));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 3));
        test_uint(t2, ecs_iter_get_var(&it, x_var));
        test_uint(e3, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelA, t3), ecs_field_id(&it, 2));
        test_uint(Tag, ecs_field_id(&it, 3));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(t3, ecs_field_src(&it, 3));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 3));
        test_uint(t3, ecs_iter_get_var(&it, x_var));
        test_uint(e3, it.entities[0]);

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesOperators_3_and_optional_dependent_optional_pair_rel(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "RelA($this), ?$x($this, TgtA), ?$x($this, TgtB)"
    });

    test_assert(r != NULL);

    int x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != 0);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_entity_t e1 = ecs_new(world, RelA);
    ecs_entity_t e2 = ecs_new(world, RelA);
    ecs_entity_t e3 = ecs_new(world, RelA);
    ecs_entity_t e4 = ecs_new(world, RelA);

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
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(e1, it.entities[0]);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 2));
        test_uint(ecs_pair(RelA, TgtB), ecs_field_id(&it, 3));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(0, ecs_field_src(&it, 3));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_bool(false, ecs_field_is_set(&it, 3));
        test_uint(RelA, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(e2, it.entities[0]);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(ecs_pair(EcsWildcard, TgtA), ecs_field_id(&it, 2));
        test_uint(ecs_pair(EcsWildcard, TgtB), ecs_field_id(&it, 3));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(0, ecs_field_src(&it, 3));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_field_is_set(&it, 2));
        test_bool(false, ecs_field_is_set(&it, 3));
        test_uint(EcsWildcard, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(e3, it.entities[0]);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 2));
        test_uint(ecs_pair(RelA, TgtB), ecs_field_id(&it, 3));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(0, ecs_field_src(&it, 3));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 3));
        test_uint(RelA, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(e4, it.entities[0]);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 2));
        test_uint(ecs_pair(RelA, TgtB), ecs_field_id(&it, 3));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(0, ecs_field_src(&it, 3));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 3));
        test_uint(RelA, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(e4, it.entities[0]);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelB, TgtA), ecs_field_id(&it, 2));
        test_uint(ecs_pair(RelB, TgtB), ecs_field_id(&it, 3));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(0, ecs_field_src(&it, 3));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 3));
        test_uint(RelB, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesOperators_3_and_optional_dependent_optional_pair_tgt(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "RelA($this), ?RelA($this, $x), ?RelB($this, $x)"
    });

    test_assert(r != NULL);

    int x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != 0);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_entity_t e1 = ecs_new(world, RelA);
    ecs_entity_t e2 = ecs_new(world, RelA);
    ecs_entity_t e3 = ecs_new(world, RelA);
    ecs_entity_t e4 = ecs_new(world, RelA);

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
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 2));
        test_uint(ecs_pair(RelB, TgtA), ecs_field_id(&it, 3));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(0, ecs_field_src(&it, 3));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_bool(false, ecs_field_is_set(&it, 3));
        test_uint(TgtA, ecs_iter_get_var(&it, x_var));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelA, EcsWildcard), ecs_field_id(&it, 2));
        test_uint(ecs_pair(RelB, EcsWildcard), ecs_field_id(&it, 3));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(0, ecs_field_src(&it, 3));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_field_is_set(&it, 2));
        test_bool(false, ecs_field_is_set(&it, 3));
        test_uint(EcsWildcard, ecs_iter_get_var(&it, x_var));
        test_uint(e2, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 2));
        test_uint(ecs_pair(RelB, TgtA), ecs_field_id(&it, 3));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(0, ecs_field_src(&it, 3));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 3));
        test_uint(TgtA, ecs_iter_get_var(&it, x_var));
        test_uint(e3, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 2));
        test_uint(ecs_pair(RelB, TgtA), ecs_field_id(&it, 3));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(0, ecs_field_src(&it, 3));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 3));
        test_uint(TgtA, ecs_iter_get_var(&it, x_var));
        test_uint(e4, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelA, TgtB), ecs_field_id(&it, 2));
        test_uint(ecs_pair(RelB, TgtB), ecs_field_id(&it, 3));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(0, ecs_field_src(&it, 3));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 3));
        test_uint(TgtB, ecs_iter_get_var(&it, x_var));
        test_uint(e4, it.entities[0]);

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesOperators_3_and_optional_dependent_optional_pair_src(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);
    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "RelA($this), ?RelA($this, $x), ?Tag($x)"
    });

    test_assert(r != NULL);

    int x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != 0);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_entity_t e1 = ecs_new(world, RelA);
    ecs_entity_t e2 = ecs_new(world, RelA);
    ecs_entity_t e3 = ecs_new(world, RelA);

    ecs_entity_t t1 = ecs_new_id(world);
    ecs_entity_t t2 = ecs_new(world, Tag);
    ecs_entity_t t3 = ecs_new(world, Tag);

    ecs_add_pair(world, e2, RelA, t1);
    ecs_add_pair(world, e3, RelA, t2);
    ecs_add_pair(world, e3, RelA, t3);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelA, EcsWildcard), ecs_field_id(&it, 2));
        test_uint(Tag, ecs_field_id(&it, 3));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(EcsWildcard, ecs_field_src(&it, 3));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_field_is_set(&it, 2));
        test_bool(false, ecs_field_is_set(&it, 3));
        test_uint(EcsWildcard, ecs_iter_get_var(&it, x_var));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelA, t1), ecs_field_id(&it, 2));
        test_uint(Tag, ecs_field_id(&it, 3));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(t1, ecs_field_src(&it, 3));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_bool(false, ecs_field_is_set(&it, 3));
        test_uint(t1, ecs_iter_get_var(&it, x_var));
        test_uint(e2, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelA, t2), ecs_field_id(&it, 2));
        test_uint(Tag, ecs_field_id(&it, 3));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(t2, ecs_field_src(&it, 3));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 3));
        test_uint(t2, ecs_iter_get_var(&it, x_var));
        test_uint(e3, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelA, t3), ecs_field_id(&it, 2));
        test_uint(Tag, ecs_field_id(&it, 3));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(t3, ecs_field_src(&it, 3));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 3));
        test_uint(t3, ecs_iter_get_var(&it, x_var));
        test_uint(e3, it.entities[0]);

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesOperators_3_and_optional_dependent_not_pair_rel(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "RelA($this), ?$x($this, TgtA), !$x($this, TgtB)"
    });

    test_assert(r != NULL);

    int x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != 0);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_entity_t e1 = ecs_new(world, RelA);
    ecs_entity_t e2 = ecs_new(world, RelA);
    ecs_entity_t e3 = ecs_new(world, RelA);

    ecs_add_pair(world, e1, RelA, TgtA);
    ecs_add_pair(world, e1, RelA, TgtB);

    ecs_add_pair(world, e2, RelA, TgtA);
    ecs_add_pair(world, e2, RelA, TgtB);

    ecs_add_pair(world, e3, RelA, TgtA);
    ecs_add_pair(world, e3, RelA, TgtB);
    ecs_add_pair(world, e3, RelB, TgtA);
    ecs_add_pair(world, e3, RelB, TgtB);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }
    
    ecs_remove_pair(world, e3, RelB, TgtA);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_remove_pair(world, e3, RelB, TgtB);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_remove_pair(world, e1, RelA, TgtB);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 2));
        test_uint(ecs_pair(RelA, TgtB), ecs_field_id(&it, 3));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(0, ecs_field_src(&it, 3));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_bool(false, ecs_field_is_set(&it, 3));
        test_uint(RelA, ecs_iter_get_var(&it, x_var));
        test_uint(e1, it.entities[0]);

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesOperators_3_and_optional_dependent_not_pair_tgt(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "RelA($this), ?RelA($this, $x), !RelB($this, $x)"
    });

    test_assert(r != NULL);

    int x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != 0);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_entity_t e1 = ecs_new(world, RelA);
    ecs_entity_t e2 = ecs_new(world, RelA);
    ecs_entity_t e3 = ecs_new(world, RelA);

    ecs_add_pair(world, e1, RelA, TgtA);
    ecs_add_pair(world, e1, RelB, TgtA);

    ecs_add_pair(world, e2, RelA, TgtA);
    ecs_add_pair(world, e2, RelB, TgtA);

    ecs_add_pair(world, e3, RelA, TgtA);
    ecs_add_pair(world, e3, RelB, TgtA);
    ecs_add_pair(world, e3, RelA, TgtB);
    ecs_add_pair(world, e3, RelB, TgtB);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }
    
    ecs_remove_pair(world, e3, RelA, TgtB);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_remove_pair(world, e3, RelB, TgtB);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_remove_pair(world, e1, RelB, TgtA);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 2));
        test_uint(ecs_pair(RelB, TgtA), ecs_field_id(&it, 3));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(0, ecs_field_src(&it, 3));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_bool(false, ecs_field_is_set(&it, 3));
        test_uint(TgtA, ecs_iter_get_var(&it, x_var));
        test_uint(e1, it.entities[0]);

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesOperators_3_and_optional_dependent_not_pair_src(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);
    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "RelA($this), ?RelA($this, $x), !Tag($x)"
    });

    test_assert(r != NULL);

    int x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != 0);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_entity_t e1 = ecs_new(world, RelA);
    ecs_entity_t e2 = ecs_new(world, RelA);
    ecs_entity_t e3 = ecs_new(world, RelA);

    ecs_entity_t t1 = ecs_new(world, Tag);
    ecs_entity_t t2 = ecs_new(world, Tag);
    ecs_entity_t t3 = ecs_new(world, Tag);

    ecs_add_pair(world, e2, RelA, t1);
    ecs_add_pair(world, e3, RelA, t2);
    ecs_add_pair(world, e3, RelA, t3);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(e1, it.entities[0]);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelA, EcsWildcard), ecs_field_id(&it, 2));
        test_uint(Tag, ecs_field_id(&it, 3));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(EcsWildcard, ecs_field_src(&it, 3));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_field_is_set(&it, 2));
        test_bool(false, ecs_field_is_set(&it, 3));
        test_uint(EcsWildcard, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_remove(world, t1, Tag);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(e1, it.entities[0]);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelA, EcsWildcard), ecs_field_id(&it, 2));
        test_uint(Tag, ecs_field_id(&it, 3));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(EcsWildcard, ecs_field_src(&it, 3));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_field_is_set(&it, 2));
        test_bool(false, ecs_field_is_set(&it, 3));
        test_uint(EcsWildcard, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(e2, it.entities[0]);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelA, t1), ecs_field_id(&it, 2));
        test_uint(Tag, ecs_field_id(&it, 3));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(t1, ecs_field_src(&it, 3));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_bool(false, ecs_field_is_set(&it, 3));
        test_uint(t1, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_remove_pair(world, e3, RelA, t2);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(e1, it.entities[0]);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelA, EcsWildcard), ecs_field_id(&it, 2));
        test_uint(Tag, ecs_field_id(&it, 3));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(EcsWildcard, ecs_field_src(&it, 3));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_field_is_set(&it, 2));
        test_bool(false, ecs_field_is_set(&it, 3));
        test_uint(EcsWildcard, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(e2, it.entities[0]);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelA, t1), ecs_field_id(&it, 2));
        test_uint(Tag, ecs_field_id(&it, 3));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(t1, ecs_field_src(&it, 3));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_bool(false, ecs_field_is_set(&it, 3));
        test_uint(t1, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_remove_pair(world, e3, RelA, t3);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(2, it.count);
        test_uint(e1, it.entities[0]);
        test_uint(e3, it.entities[1]);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelA, EcsWildcard), ecs_field_id(&it, 2));
        test_uint(Tag, ecs_field_id(&it, 3));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(EcsWildcard, ecs_field_src(&it, 3));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_field_is_set(&it, 2));
        test_bool(false, ecs_field_is_set(&it, 3));
        test_uint(EcsWildcard, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(e2, it.entities[0]);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelA, t1), ecs_field_id(&it, 2));
        test_uint(Tag, ecs_field_id(&it, 3));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(t1, ecs_field_src(&it, 3));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_bool(false, ecs_field_is_set(&it, 3));
        test_uint(t1, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesOperators_1_optional_any_src_fixed_first(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "?RelA(_)"
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(false, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_new(world, RelA);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(EcsWildcard, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_new(world, RelA);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(EcsWildcard, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesOperators_1_optional_any_src_any_tgt_fixed_first(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "?RelA(_, _)"
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(RelA, EcsWildcard), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(false, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_new_w_pair(world, TgtA, RelA);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(RelA, EcsWildcard), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(false, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_new_w_pair(world, RelA, TgtA);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(RelA, EcsWildcard), ecs_field_id(&it, 1));
        test_uint(EcsWildcard, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_new_w_pair(world, RelA, TgtB);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(RelA, EcsWildcard), ecs_field_id(&it, 1));
        test_uint(EcsWildcard, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesOperators_1_optional_any_src_any_first_fixed_tgt(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, TgtA);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "?_(_, TgtA)"
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(EcsWildcard, TgtA), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(false, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_new_w_pair(world, TgtA, RelA);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(EcsWildcard, TgtA), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(false, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_new_w_pair(world, RelA, TgtA);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(EcsWildcard, TgtA), ecs_field_id(&it, 1));
        test_uint(EcsWildcard, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_new_w_pair(world, RelB, TgtA);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(EcsWildcard, TgtA), ecs_field_id(&it, 1));
        test_uint(EcsWildcard, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesOperators_1_optional_any_src_any_childof_pair_any_tgt(void) {
    ecs_world_t *world = ecs_init();

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "?ChildOf(_, _)"
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(EcsChildOf, EcsWildcard), ecs_field_id(&it, 1));
        test_uint(EcsWildcard, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesOperators_1_optional_any_src_any_isa_pair_any_tgt(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TgtA);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "?IsA(_, _)"
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(EcsIsA, EcsWildcard), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(false, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_new_w_pair(world, EcsIsA, TgtA);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(EcsIsA, EcsWildcard), ecs_field_id(&it, 1));
        test_uint(EcsWildcard, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesOperators_2_or(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, RelC);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "RelA($this) || RelB($this)"
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_entity_t e1 = ecs_new(world, RelA);
    ecs_entity_t e2 = ecs_new(world, RelB);
    ecs_new(world, RelC);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(RelB, ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(e2, it.entities[0]);

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesOperators_3_or(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, RelC);
    ECS_TAG(world, RelD);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "RelA($this) || RelB($this) || RelC($this)"
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_entity_t e1 = ecs_new(world, RelA);
    ecs_entity_t e2 = ecs_new(world, RelB);
    ecs_entity_t e3 = ecs_new(world, RelC);
    ecs_new(world, RelD);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(RelB, ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(e2, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(RelC, ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(e3, it.entities[0]);

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesOperators_2_or_written(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);
    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, RelC);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "Tag($this), RelA($this) || RelB($this)"
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_entity_t e1 = ecs_new(world, Tag);
    ecs_entity_t e2 = ecs_new(world, Tag);
    ecs_entity_t e3 = ecs_new(world, Tag);
    ecs_entity_t e4 = ecs_new(world, Tag);
    ecs_add(world, e1, RelA);
    ecs_add(world, e2, RelA);
    ecs_add(world, e2, RelB);
    ecs_add(world, e3, RelB);
    ecs_add(world, e4, RelC);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(Tag, ecs_field_id(&it, 1));
        test_uint(RelA, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(Tag, ecs_field_id(&it, 1));
        test_uint(RelA, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(e2, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(Tag, ecs_field_id(&it, 1));
        test_uint(RelB, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(e3, it.entities[0]);

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesOperators_3_or_written(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);
    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, RelC);
    ECS_TAG(world, RelD);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "Tag($this), RelA($this) || RelB($this) || RelC($this)"
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_entity_t e1 = ecs_new(world, Tag);
    ecs_entity_t e2 = ecs_new(world, Tag);
    ecs_entity_t e3 = ecs_new(world, Tag);
    ecs_entity_t e4 = ecs_new(world, Tag);
    ecs_entity_t e5 = ecs_new(world, Tag);
    ecs_add(world, e1, RelA);
    ecs_add(world, e2, RelB);
    ecs_add(world, e3, RelB);
    ecs_add(world, e3, RelC);
    ecs_add(world, e4, RelC);
    ecs_add(world, e5, RelD);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(Tag, ecs_field_id(&it, 1));
        test_uint(RelA, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(Tag, ecs_field_id(&it, 1));
        test_uint(RelB, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(e2, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(Tag, ecs_field_id(&it, 1));
        test_uint(RelB, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(e3, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(Tag, ecs_field_id(&it, 1));
        test_uint(RelC, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(e4, it.entities[0]);

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesOperators_2_or_written_w_rel_var(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);
    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, RelC);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);
    ECS_TAG(world, TgtC);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "Tag($this, $x), $x($this, TgtA) || $x($this, TgtB)"
    });

    test_assert(r != NULL);

    int x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
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
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(Tag, RelA), ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(RelA, ecs_iter_get_var(&it, x_var));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(Tag, RelB), ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelB, TgtB), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(RelB, ecs_iter_get_var(&it, x_var));
        test_uint(e2, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(Tag, RelC), ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelC, TgtA), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(RelC, ecs_iter_get_var(&it, x_var));
        test_uint(e3, it.entities[0]);

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesOperators_3_or_written_w_rel_var(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);
    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, RelC);
    ECS_TAG(world, RelD);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);
    ECS_TAG(world, TgtC);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "Tag($this, $x), $x($this, TgtA) || $x($this, TgtB) || $x($this, TgtC)"
    });

    test_assert(r != NULL);

    int x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
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
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(Tag, RelA), ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(RelA, ecs_iter_get_var(&it, x_var));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(Tag, RelB), ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelB, TgtB), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(RelB, ecs_iter_get_var(&it, x_var));
        test_uint(e2, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(Tag, RelC), ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelC, TgtA), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(RelC, ecs_iter_get_var(&it, x_var));
        test_uint(e3, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(Tag, RelD), ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelD, TgtC), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(RelD, ecs_iter_get_var(&it, x_var));
        test_uint(e4, it.entities[0]);

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesOperators_2_or_written_w_tgt_var(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);
    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, RelC);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);
    ECS_TAG(world, TgtC);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "Tag($this, $x), RelA($this, $x) || RelB($this, $x)"
    });

    test_assert(r != NULL);

    int x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
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
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(Tag, TgtA), ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(TgtA, ecs_iter_get_var(&it, x_var));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(Tag, TgtB), ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelB, TgtB), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(TgtB, ecs_iter_get_var(&it, x_var));
        test_uint(e2, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(Tag, TgtC), ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelA, TgtC), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(TgtC, ecs_iter_get_var(&it, x_var));
        test_uint(e3, it.entities[0]);

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesOperators_2_or_written_w_rel_tgt_var(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);
    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, RelC);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);
    ECS_TAG(world, TgtC);
    ECS_TAG(world, TgtD);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "RelA($this, $x), RelB($this, $y), $x($this, $y) || $y($this, $x)"
    });

    test_assert(r != NULL);

    int x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != -1);

    int y_var = ecs_rule_find_var(r, "y");
    test_assert(y_var != -1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
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
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(RelA, TgtB), ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelB, TgtC), ecs_field_id(&it, 2));
        test_uint(ecs_pair(TgtB, TgtC), ecs_field_id(&it, 3));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(TgtB, ecs_iter_get_var(&it, x_var));
        test_uint(TgtC, ecs_iter_get_var(&it, y_var));
        test_uint(e2, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(RelA, TgtC), ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelB, TgtB), ecs_field_id(&it, 2));
        test_uint(ecs_pair(TgtB, TgtC), ecs_field_id(&it, 3));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(TgtC, ecs_iter_get_var(&it, x_var));
        test_uint(TgtB, ecs_iter_get_var(&it, y_var));
        test_uint(e3, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(RelA, TgtD), ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelB, TgtA), ecs_field_id(&it, 2));
        test_uint(ecs_pair(TgtD, TgtA), ecs_field_id(&it, 3));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(TgtD, ecs_iter_get_var(&it, x_var));
        test_uint(TgtA, ecs_iter_get_var(&it, y_var));
        test_uint(e4, it.entities[0]);

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesOperators_2_or_written_w_rel_tgt_same_var(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);
    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, RelC);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);
    ECS_TAG(world, TgtC);
    ECS_TAG(world, TgtD);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "RelA($this, $x), $x($this, $x) || RelB($this, $x)"
    });

    test_assert(r != NULL);

    int x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_entity_t e1 = ecs_new_w_pair(world, RelA, TgtA);
    ecs_entity_t e2 = ecs_new_w_pair(world, RelA, TgtB);
    ecs_entity_t e3 = ecs_new_w_pair(world, RelA, TgtC);
    ecs_new_w_pair(world, RelA, TgtC);

    ecs_add_pair(world, e1, TgtA, TgtA);
    ecs_add_pair(world, e2, RelB, TgtB);
    ecs_add_pair(world, e3, TgtC, TgtC);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 1));
        test_uint(ecs_pair(TgtA, TgtA), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(TgtA, ecs_iter_get_var(&it, x_var));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(RelA, TgtB), ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelB, TgtB), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(TgtB, ecs_iter_get_var(&it, x_var));
        test_uint(e2, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(RelA, TgtC), ecs_field_id(&it, 1));
        test_uint(ecs_pair(TgtC, TgtC), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(TgtC, ecs_iter_get_var(&it, x_var));
        test_uint(e3, it.entities[0]);

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesOperators_3_or_written_w_tgt_var(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);
    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, RelC);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);
    ECS_TAG(world, TgtC);
    ECS_TAG(world, TgtD);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "Tag($this, $x), RelA($this, $x) || RelB($this, $x) || RelC($this, $x)"
    });

    test_assert(r != NULL);

    int x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
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
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(Tag, TgtA), ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(TgtA, ecs_iter_get_var(&it, x_var));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(Tag, TgtB), ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelB, TgtB), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(TgtB, ecs_iter_get_var(&it, x_var));
        test_uint(e2, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(Tag, TgtC), ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelA, TgtC), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(TgtC, ecs_iter_get_var(&it, x_var));
        test_uint(e3, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(Tag, TgtD), ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelC, TgtD), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(TgtD, ecs_iter_get_var(&it, x_var));
        test_uint(e4, it.entities[0]);

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesOperators_2_or_chains_written(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);
    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, RelC);
    ECS_TAG(world, RelD);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "Tag($this), RelA($this) || RelB($this), RelC($this) || RelD($this)"
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_entity_t e1 = ecs_new(world, Tag);
    ecs_entity_t e2 = ecs_new(world, Tag);
    ecs_entity_t e3 = ecs_new(world, Tag);
    ecs_entity_t e4 = ecs_new(world, Tag);
    ecs_entity_t e5 = ecs_new(world, Tag);
    ecs_entity_t e6 = ecs_new(world, Tag);

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
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(Tag, ecs_field_id(&it, 1));
        test_uint(RelA, ecs_field_id(&it, 2));
        test_uint(RelC, ecs_field_id(&it, 3));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(e3, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(Tag, ecs_field_id(&it, 1));
        test_uint(RelA, ecs_field_id(&it, 2));
        test_uint(RelD, ecs_field_id(&it, 3));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(e4, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(Tag, ecs_field_id(&it, 1));
        test_uint(RelB, ecs_field_id(&it, 2));
        test_uint(RelC, ecs_field_id(&it, 3));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(e5, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(Tag, ecs_field_id(&it, 1));
        test_uint(RelB, ecs_field_id(&it, 2));
        test_uint(RelD, ecs_field_id(&it, 3));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(e6, it.entities[0]);

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesOperators_2_or_chains(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);
    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, RelC);
    ECS_TAG(world, RelD);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "RelA($this) || RelB($this), RelC($this) || RelD($this)"
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_entity_t e1 = ecs_new_id(world);
    ecs_entity_t e2 = ecs_new_id(world);
    ecs_entity_t e3 = ecs_new_id(world);
    ecs_entity_t e4 = ecs_new_id(world);
    ecs_entity_t e5 = ecs_new_id(world);
    ecs_entity_t e6 = ecs_new_id(world);

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
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(e3, it.entities[0]);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(RelC, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 1));

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(e4, it.entities[0]);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(RelD, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 1));

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(e5, it.entities[0]);
        test_uint(RelB, ecs_field_id(&it, 1));
        test_uint(RelC, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 1));

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(e6, it.entities[0]);
        test_uint(RelB, ecs_field_id(&it, 1));
        test_uint(RelD, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 1));

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesOperators_2_or_dependent(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Foo);
    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, RelC);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "RelB($this) || RelA($this, $tgt), RelC($tgt)"
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    int tgt_var = ecs_rule_find_var(r, "tgt");
    test_assert(tgt_var != -1);

    ecs_entity_t tgt = ecs_new(world, RelC);

    ecs_entity_t e1 = ecs_new_id(world);
    ecs_entity_t e2 = ecs_new_id(world);
    ecs_entity_t e3 = ecs_new_id(world);

    ecs_add(world, e1, RelB);
    ecs_add_pair(world, e2, RelA, tgt);
    ecs_add(world, e3, RelB);
    ecs_add(world, e3, Foo);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(RelB, ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_field_is_set(&it, 2));
        test_uint(e1, it.entities[0]);
        test_uint(EcsWildcard, ecs_iter_get_var(&it, tgt_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(RelB, ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_field_is_set(&it, 2));
        test_uint(e3, it.entities[0]);
        test_uint(EcsWildcard, ecs_iter_get_var(&it, tgt_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(RelA, tgt), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(e2, it.entities[0]);
        test_uint(tgt, ecs_iter_get_var(&it, tgt_var));

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesOperators_2_or_dependent_reverse(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Foo);
    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, RelC);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "RelA($this, $tgt) || RelB($this), RelC($tgt)"
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    int tgt_var = ecs_rule_find_var(r, "tgt");
    test_assert(tgt_var != -1);

    ecs_entity_t tgt = ecs_new(world, RelC);

    ecs_entity_t e1 = ecs_new_id(world);
    ecs_entity_t e2 = ecs_new_id(world);
    ecs_entity_t e3 = ecs_new_id(world);

    ecs_add(world, e1, RelB);
    ecs_add_pair(world, e2, RelA, tgt);
    ecs_add(world, e3, RelB);
    ecs_add(world, e3, Foo);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(RelA, tgt), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(e2, it.entities[0]);
        test_uint(tgt, ecs_iter_get_var(&it, tgt_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(RelB, ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_field_is_set(&it, 2));
        test_uint(e1, it.entities[0]);
        test_uint(EcsWildcard, ecs_iter_get_var(&it, tgt_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(RelB, ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_field_is_set(&it, 2));
        test_uint(e3, it.entities[0]);
        test_uint(EcsWildcard, ecs_iter_get_var(&it, tgt_var));

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesOperators_2_or_dependent_2_vars(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Foo);
    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, RelC);
    ECS_TAG(world, RelD);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "RelA($this, $x) || RelB($this, $y), RelC($x), RelD($y)"
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    int x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != -1);
    int y_var = ecs_rule_find_var(r, "y");
    test_assert(y_var != -1);

    ecs_entity_t tgt_a = ecs_new(world, RelC);
    ecs_entity_t tgt_b = ecs_new(world, RelD);

    ecs_entity_t e1 = ecs_new_id(world);
    ecs_entity_t e2 = ecs_new_id(world);
    ecs_entity_t e3 = ecs_new_id(world);
    ecs_entity_t e4 = ecs_new_id(world);

    ecs_add_pair(world, e1, RelA, tgt_a);
    ecs_add_pair(world, e2, RelB, tgt_b);

    ecs_add_pair(world, e3, RelA, tgt_b);
    ecs_add_pair(world, e4, RelB, tgt_a);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(RelA, tgt_a), ecs_field_id(&it, 1));
        test_uint(RelC, ecs_field_id(&it, 2));
        test_uint(RelD, ecs_field_id(&it, 3));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_bool(false, ecs_field_is_set(&it, 3));
        test_uint(e1, it.entities[0]);
        test_uint(tgt_a, ecs_iter_get_var(&it, x_var));
        test_uint(EcsWildcard, ecs_iter_get_var(&it, y_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(RelB, tgt_b), ecs_field_id(&it, 1));
        test_uint(RelC, ecs_field_id(&it, 2));
        test_uint(RelD, ecs_field_id(&it, 3));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_field_is_set(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 3));
        test_uint(e2, it.entities[0]);
        test_uint(EcsWildcard, ecs_iter_get_var(&it, x_var));
        test_uint(tgt_b, ecs_iter_get_var(&it, y_var));

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesOperators_2_or_written_dependent(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);
    ECS_TAG(world, Foo);
    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, RelC);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "Tag, RelA($this, $tgt) || RelB($this), RelC($tgt)"
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    int tgt_var = ecs_rule_find_var(r, "tgt");
    test_assert(tgt_var != -1);

    ecs_entity_t tgt = ecs_new(world, RelC);

    ecs_entity_t e1 = ecs_new(world, Tag);
    ecs_entity_t e2 = ecs_new(world, Tag);
    ecs_entity_t e3 = ecs_new(world, Tag);

    ecs_add(world, e1, RelB);
    ecs_add_pair(world, e2, RelA, tgt);
    ecs_add(world, e3, RelB);
    ecs_add(world, e3, Foo);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(Tag, ecs_field_id(&it, 1));
        test_uint(RelB, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_bool(false, ecs_field_is_set(&it, 3));
        test_uint(e1, it.entities[0]);
        test_uint(EcsWildcard, ecs_iter_get_var(&it, tgt_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(Tag, ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelA, tgt), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 3));
        test_uint(e2, it.entities[0]);
        test_uint(tgt, ecs_iter_get_var(&it, tgt_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(Tag, ecs_field_id(&it, 1));
        test_uint(RelB, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_bool(false, ecs_field_is_set(&it, 3));
        test_uint(e3, it.entities[0]);
        test_uint(EcsWildcard, ecs_iter_get_var(&it, tgt_var));

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesOperators_2_or_written_dependent_2_vars(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);
    ECS_TAG(world, Foo);
    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, RelC);
    ECS_TAG(world, RelD);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "Tag, RelA($this, $x) || RelB($this, $y), RelC($x), RelD($y)"
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    int x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != -1);
    int y_var = ecs_rule_find_var(r, "y");
    test_assert(y_var != -1);

    ecs_entity_t tgt_a = ecs_new(world, RelC);
    ecs_entity_t tgt_b = ecs_new(world, RelD);

    ecs_entity_t e1 = ecs_new(world, Tag);
    ecs_entity_t e2 = ecs_new(world, Tag);
    ecs_entity_t e3 = ecs_new(world, Tag);
    ecs_entity_t e4 = ecs_new(world, Tag);

    ecs_add_pair(world, e1, RelA, tgt_a);
    ecs_add_pair(world, e2, RelB, tgt_b);

    ecs_add_pair(world, e3, RelA, tgt_b);
    ecs_add_pair(world, e4, RelB, tgt_a);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(Tag, ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelA, tgt_a), ecs_field_id(&it, 2));
        test_uint(RelC, ecs_field_id(&it, 3));
        test_uint(RelD, ecs_field_id(&it, 4));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 3));
        test_bool(false, ecs_field_is_set(&it, 4));
        test_uint(e1, it.entities[0]);
        test_uint(tgt_a, ecs_iter_get_var(&it, x_var));
        test_uint(EcsWildcard, ecs_iter_get_var(&it, y_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(Tag, ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelB, tgt_b), ecs_field_id(&it, 2));
        test_uint(RelC, ecs_field_id(&it, 3));
        test_uint(RelD, ecs_field_id(&it, 4));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_bool(false, ecs_field_is_set(&it, 3));
        test_bool(true, ecs_field_is_set(&it, 4));
        test_uint(e2, it.entities[0]);
        test_uint(EcsWildcard, ecs_iter_get_var(&it, x_var));
        test_uint(tgt_b, ecs_iter_get_var(&it, y_var));

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesOperators_2_or_w_dependent(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);
    ECS_TAG(world, Foo);
    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, RelC);
    ECS_TAG(world, RelD);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "Tag, ?RelA($this, $x), RelB($x) || RelC($x)"
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    int x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != -1);

    ecs_entity_t tgt_a = ecs_new(world, RelB);
    ecs_entity_t tgt_b = ecs_new(world, RelC);
    ecs_entity_t tgt_c = ecs_new(world, RelD);

    ecs_entity_t e1 = ecs_new(world, Tag);
    ecs_entity_t e2 = ecs_new(world, Tag);
    ecs_entity_t e3 = ecs_new(world, Tag);
    ecs_entity_t e4 = ecs_new(world, Tag);

    ecs_add_pair(world, e2, RelA, tgt_a);
    ecs_add_pair(world, e3, RelA, tgt_b);
    ecs_add_pair(world, e4, RelA, tgt_c);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(Tag, ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelA, EcsWildcard), ecs_field_id(&it, 2));
        // test_uint(RelC, ecs_field_id(&it, 3));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_field_is_set(&it, 2));
        test_bool(false, ecs_field_is_set(&it, 3));
        test_uint(e1, it.entities[0]);
        test_uint(EcsWildcard, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(Tag, ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelA, tgt_a), ecs_field_id(&it, 2));
        test_uint(RelB, ecs_field_id(&it, 3));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 3));
        test_uint(e2, it.entities[0]);
        test_uint(tgt_a, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(Tag, ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelA, tgt_b), ecs_field_id(&it, 2));
        test_uint(RelC, ecs_field_id(&it, 3));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 3));
        test_uint(e3, it.entities[0]);
        test_uint(tgt_b, ecs_iter_get_var(&it, x_var));


        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesOperators_2_or_w_both(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, RelC);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "RelA || RelB"
    });

    test_assert(r != NULL);

    ecs_entity_t e1 = ecs_new(world, RelA);
    ecs_entity_t e2 = ecs_new(world, RelA);
    ecs_add(world, e2, RelB);
    ecs_entity_t e3 = ecs_new(world, RelB);
    ecs_new(world, RelC);

    ecs_iter_t it = ecs_rule_iter(world, r);
    test_bool(true, ecs_rule_next(&it));
    test_int(1, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(RelA, ecs_field_id(&it, 1));

    test_bool(true, ecs_rule_next(&it));
    test_int(1, it.count);
    test_uint(e2, it.entities[0]);
    test_uint(RelA, ecs_field_id(&it, 1));

    test_bool(true, ecs_rule_next(&it));
    test_int(1, it.count);
    test_uint(e3, it.entities[0]);
    test_uint(RelB, ecs_field_id(&it, 1));

    test_bool(false, ecs_rule_next(&it));

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesOperators_3_or_w_both(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, RelC);
    ECS_TAG(world, RelD);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "RelA || RelB || RelC"
    });

    test_assert(r != NULL);

    ecs_entity_t e1 = ecs_new(world, RelA);
    ecs_entity_t e2 = ecs_new(world, RelA);
    ecs_add(world, e2, RelB);
    ecs_entity_t e3 = ecs_new(world, RelA);
    ecs_add(world, e3, RelD);

    ecs_entity_t e4 = ecs_new(world, RelB);
    ecs_entity_t e5 = ecs_new(world, RelB);
    ecs_add(world, e5, RelC);
    ecs_entity_t e6 = ecs_new(world, RelB);
    ecs_add(world, e6, RelD);

    ecs_entity_t e7 = ecs_new(world, RelC);
    ecs_entity_t e8 = ecs_new(world, RelC);
    ecs_add(world, e8, RelD);

    ecs_iter_t it = ecs_rule_iter(world, r);
    test_bool(true, ecs_rule_next(&it));
    test_int(1, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(RelA, ecs_field_id(&it, 1));

    test_bool(true, ecs_rule_next(&it));
    test_int(1, it.count);
    test_uint(e2, it.entities[0]);
    test_uint(RelA, ecs_field_id(&it, 1));

    test_bool(true, ecs_rule_next(&it));
    test_int(1, it.count);
    test_uint(e3, it.entities[0]);
    test_uint(RelA, ecs_field_id(&it, 1));

    test_bool(true, ecs_rule_next(&it));
    test_int(1, it.count);
    test_uint(e4, it.entities[0]);
    test_uint(RelB, ecs_field_id(&it, 1));

    test_bool(true, ecs_rule_next(&it));
    test_int(1, it.count);
    test_uint(e5, it.entities[0]);
    test_uint(RelB, ecs_field_id(&it, 1));

    test_bool(true, ecs_rule_next(&it));
    test_int(1, it.count);
    test_uint(e6, it.entities[0]);
    test_uint(RelB, ecs_field_id(&it, 1));

    test_bool(true, ecs_rule_next(&it));
    test_int(1, it.count);
    test_uint(e7, it.entities[0]);
    test_uint(RelC, ecs_field_id(&it, 1));

    test_bool(true, ecs_rule_next(&it));
    test_int(1, it.count);
    test_uint(e8, it.entities[0]);
    test_uint(RelC, ecs_field_id(&it, 1));

    test_bool(false, ecs_rule_next(&it));

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesOperators_2_not_first(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_entity_t e1 = ecs_new(world, TagA);
    ecs_entity_t e2 = ecs_new(world, TagA);
    ecs_entity_t e3 = ecs_new(world, TagA);

    ecs_add(world, e1, TagB);
    ecs_add(world, e2, TagB);
    ecs_add(world, e3, TagB);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "!TagB($this), TagA($this)"
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_remove(world, e1, TagB);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(TagB, ecs_field_id(&it, 1));
        test_uint(TagA, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(false, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(e1, it.entities[0]);

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_remove(world, e2, TagB);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(2, it.count);
        test_uint(TagB, ecs_field_id(&it, 1));
        test_uint(TagA, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(false, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(e1, it.entities[0]);
        test_uint(e2, it.entities[1]);

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesOperators_2_optional_first(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_entity_t e1 = ecs_new(world, TagA);
    ecs_entity_t e2 = ecs_new(world, TagA);
    ecs_entity_t e3 = ecs_new(world, TagA);

    ecs_add(world, e1, TagB);
    ecs_add(world, e2, TagB);
    ecs_add(world, e3, TagB);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "?TagB($this), TagA($this)"
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(3, it.count);
        test_uint(TagB, ecs_field_id(&it, 1));
        test_uint(TagA, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(e1, it.entities[0]);
        test_uint(e2, it.entities[1]);
        test_uint(e3, it.entities[2]);

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_remove(world, e3, TagB);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(2, it.count);
        test_uint(TagB, ecs_field_id(&it, 1));
        test_uint(TagA, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(e1, it.entities[0]);
        test_uint(e2, it.entities[1]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(TagB, ecs_field_id(&it, 1));
        test_uint(TagA, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(false, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(e3, it.entities[0]);

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesOperators_root_entities_empty(void) {
    ecs_world_t *world = ecs_mini();

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "!ChildOf($this, _)"
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(EcsChildOf, 0), ecs_field_id(&it, 1));
        test_uint(EcsFlecs, it.entities[0]);

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesOperators_root_entities(void) {
    ecs_world_t *world = ecs_mini();

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "!ChildOf($this, _)"
    });

    test_assert(r != NULL);

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_new_entity(world, "e1.e2");

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(EcsChildOf, 0), ecs_field_id(&it, 1));
        test_uint(EcsFlecs, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(EcsChildOf, 0), ecs_field_id(&it, 1));
        test_uint(e1, it.entities[0]);

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesOperators_root_entities_w_children(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "!ChildOf($this, _), ChildOf(_, $this)"
    });

    test_assert(r != NULL);

    int this_var = ecs_rule_find_var(r, "This");
    test_assert(this_var != -1);
    test_assert(this_var != 0);

    ecs_new(world, Tag);
    ecs_entity_t e2 = ecs_new(world, Tag);
    ecs_entity_t e3 = ecs_new(world, Tag);
    ecs_add_pair(world, e3, EcsChildOf, e2);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(EcsChildOf, 0), ecs_field_id(&it, 1));
        test_uint(ecs_pair(EcsChildOf, EcsFlecs), ecs_field_id(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(EcsFlecs, ecs_iter_get_var(&it, this_var));
        test_uint(EcsFlecs, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(EcsChildOf, 0), ecs_field_id(&it, 1));
        test_uint(ecs_pair(EcsChildOf, e2), ecs_field_id(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(e2, ecs_iter_get_var(&it, this_var));
        test_uint(e2, it.entities[0]);

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesOperators_root_entities_w_optional_children(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "!ChildOf($this, _), ?ChildOf(_, $this)"
    });

    test_assert(r != NULL);

    int this_var = ecs_rule_find_var(r, "This");
    test_assert(this_var != -1);
    test_assert(this_var != 0);

    ecs_entity_t e1 = ecs_new(world, Tag);
    ecs_entity_t e2 = ecs_new(world, Tag);
    ecs_entity_t e3 = ecs_new(world, Tag);
    ecs_entity_t e4 = ecs_new(world, Tag);
    ecs_add_pair(world, e3, EcsChildOf, e2);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(EcsChildOf, 0), ecs_field_id(&it, 1));
        test_uint(ecs_pair(EcsChildOf, EcsFlecs), ecs_field_id(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(EcsFlecs, ecs_iter_get_var(&it, this_var));
        test_uint(EcsFlecs, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(EcsChildOf, 0), ecs_field_id(&it, 1));
        test_uint(ecs_pair(EcsChildOf, Tag), ecs_field_id(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_field_is_set(&it, 2));
        test_uint(Tag, ecs_iter_get_var(&it, this_var));
        test_uint(Tag, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(EcsChildOf, 0), ecs_field_id(&it, 1));
        test_uint(ecs_pair(EcsChildOf, e1), ecs_field_id(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_field_is_set(&it, 2));
        test_uint(e1, ecs_iter_get_var(&it, this_var));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(EcsChildOf, 0), ecs_field_id(&it, 1));
        test_uint(ecs_pair(EcsChildOf, e2), ecs_field_id(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(e2, ecs_iter_get_var(&it, this_var));
        test_uint(e2, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(EcsChildOf, 0), ecs_field_id(&it, 1));
        test_uint(ecs_pair(EcsChildOf, e4), ecs_field_id(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_field_is_set(&it, 2));
        test_uint(e4, ecs_iter_get_var(&it, this_var));
        test_uint(e4, it.entities[0]);

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesOperators_core_entities_w_optional_children(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "ChildOf($this, flecs.core), ?ChildOf(_, $this)"
    });

    test_assert(r != NULL);

    int this_var = ecs_rule_find_var(r, "This");
    test_assert(this_var != -1);
    test_assert(this_var != 0);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        while (ecs_rule_next(&it)) {
            test_assert(ecs_field_is_set(&it, 1));
            for (int i = 0; i < it.count; i ++) {
                test_assert(ecs_has_pair(
                    world, it.entities[i], EcsChildOf, EcsFlecsCore));
                if (ecs_field_is_set(&it, 2)) {
                    test_assert(ecs_count_id(
                        world, ecs_pair(EcsChildOf, it.entities[i])) != 0);
                } else {
                    test_assert(ecs_count_id(
                        world, ecs_pair(EcsChildOf, it.entities[i])) == 0);
                }
            }
        }
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesOperators_root_entities_w_not_children(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "!ChildOf($this, _), !ChildOf(_, $this)"
    });

    test_assert(r != NULL);

    int this_var = ecs_rule_find_var(r, "This");
    test_assert(this_var != -1);
    test_assert(this_var != 0);

    ecs_entity_t e1 = ecs_new(world, Tag);
    ecs_entity_t e2 = ecs_new(world, Tag);
    ecs_entity_t e3 = ecs_new(world, Tag);
    ecs_entity_t e4 = ecs_new(world, Tag);
    ecs_add_pair(world, e3, EcsChildOf, e2);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(EcsChildOf, 0), ecs_field_id(&it, 1));
        test_uint(ecs_pair(EcsChildOf, Tag), ecs_field_id(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_field_is_set(&it, 2));
        test_uint(Tag, ecs_iter_get_var(&it, this_var));
        test_uint(Tag, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(EcsChildOf, 0), ecs_field_id(&it, 1));
        test_uint(ecs_pair(EcsChildOf, e1), ecs_field_id(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_field_is_set(&it, 2));
        test_uint(e1, ecs_iter_get_var(&it, this_var));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(EcsChildOf, 0), ecs_field_id(&it, 1));
        test_uint(ecs_pair(EcsChildOf, e4), ecs_field_id(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_field_is_set(&it, 2));
        test_uint(e4, ecs_iter_get_var(&it, this_var));
        test_uint(e4, it.entities[0]);

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesOperators_core_entities_w_not_children(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "ChildOf($this, flecs.core), !ChildOf(_, $this)"
    });

    test_assert(r != NULL);

    int this_var = ecs_rule_find_var(r, "This");
    test_assert(this_var != -1);
    test_assert(this_var != 0);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        while (ecs_rule_next(&it)) {
            test_assert(ecs_field_is_set(&it, 1));
            test_assert(!ecs_field_is_set(&it, 2));
            for (int i = 0; i < it.count; i ++) {
                test_assert(ecs_has_pair(
                    world, it.entities[i], EcsChildOf, EcsFlecsCore));
                test_assert(ecs_count_id(
                    world, ecs_pair(EcsChildOf, it.entities[i])) == 0);
            }
        }
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesOperators_1_ent_src_not(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_add(world, e1, RelA);

    {
        ecs_rule_t *r = ecs_rule(world, {
            .expr = "!RelA(e1)"
        });

        test_assert(r != NULL);

        {
            ecs_iter_t it = ecs_rule_iter(world, r);
            test_bool(false, ecs_rule_next(&it));
        }

        ecs_rule_fini(r);
    }

    {
        ecs_rule_t *r = ecs_rule(world, {
            .expr = "!RelB(e1)"
        });

        test_assert(r != NULL);

        {
            ecs_iter_t it = ecs_rule_iter(world, r);
            test_bool(true, ecs_rule_next(&it));
            test_uint(0, it.count);
            test_uint(RelB, ecs_field_id(&it, 1));
            test_uint(e1, ecs_field_src(&it, 1));
            test_bool(false, ecs_field_is_set(&it, 1));
            test_bool(false, ecs_rule_next(&it));
        }

        ecs_rule_fini(r);
    }

    ecs_fini(world);
}

void RulesOperators_1_ent_src_not_pair(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, Tgt);

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_add_pair(world, e1, RelA, Tgt);

    {
        ecs_rule_t *r = ecs_rule(world, {
            .expr = "!RelA(e1, Tgt)"
        });

        test_assert(r != NULL);

        {
            ecs_iter_t it = ecs_rule_iter(world, r);
            test_bool(false, ecs_rule_next(&it));
        }

        ecs_rule_fini(r);
    }

    {
        ecs_rule_t *r = ecs_rule(world, {
            .expr = "!RelB(e1, Tgt)"
        });

        test_assert(r != NULL);

        {
            ecs_iter_t it = ecs_rule_iter(world, r);
            test_bool(true, ecs_rule_next(&it));
            test_uint(0, it.count);
            test_uint(ecs_pair(RelB, Tgt), ecs_field_id(&it, 1));
            test_uint(e1, ecs_field_src(&it, 1));
            test_bool(false, ecs_field_is_set(&it, 1));
            test_bool(false, ecs_rule_next(&it));
        }

        ecs_rule_fini(r);
    }

    ecs_fini(world);
}

void RulesOperators_1_ent_src_not_pair_rel_wildcard(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_add_pair(world, e1, RelA, TgtA);
    ecs_add_pair(world, e1, RelB, TgtA);

    {
        ecs_rule_t *r = ecs_rule(world, {
            .expr = "!*(e1, TgtA)"
        });

        test_assert(r != NULL);

        {
            ecs_iter_t it = ecs_rule_iter(world, r);
            test_bool(false, ecs_rule_next(&it));
        }

        ecs_rule_fini(r);
    }

    {
        ecs_rule_t *r = ecs_rule(world, {
            .expr = "!*(e1, TgtB)"
        });

        test_assert(r != NULL);

        {
            ecs_iter_t it = ecs_rule_iter(world, r);
            test_bool(true, ecs_rule_next(&it));
            test_uint(0, it.count);
            test_uint(ecs_pair(EcsWildcard, TgtB), ecs_field_id(&it, 1));
            test_uint(e1, ecs_field_src(&it, 1));
            test_bool(false, ecs_field_is_set(&it, 1));
            test_bool(false, ecs_rule_next(&it));
        }

        ecs_rule_fini(r);
    }

    ecs_fini(world);
}

void RulesOperators_1_ent_src_not_pair_tgt_wildcard(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_add_pair(world, e1, RelA, TgtA);
    ecs_add_pair(world, e1, RelA, TgtB);

    {
        ecs_rule_t *r = ecs_rule(world, {
            .expr = "!RelA(e1, *)"
        });

        test_assert(r != NULL);

        {
            ecs_iter_t it = ecs_rule_iter(world, r);
            test_bool(false, ecs_rule_next(&it));
        }

        ecs_rule_fini(r);
    }

    {
        ecs_rule_t *r = ecs_rule(world, {
            .expr = "!RelB(e1, *)"
        });

        test_assert(r != NULL);

        {
            ecs_iter_t it = ecs_rule_iter(world, r);
            test_bool(true, ecs_rule_next(&it));
            test_uint(0, it.count);
            test_uint(ecs_pair(RelB, EcsWildcard), ecs_field_id(&it, 1));
            test_uint(e1, ecs_field_src(&it, 1));
            test_bool(false, ecs_field_is_set(&it, 1));
            test_bool(false, ecs_rule_next(&it));
        }

        ecs_rule_fini(r);
    }

    ecs_fini(world);
}

void RulesOperators_1_ent_src_not_pair_rel_tgt_wildcard(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_add_pair(world, e1, RelA, TgtA);
    ecs_add_pair(world, e1, RelA, TgtB);

    {
        ecs_rule_t *r = ecs_rule(world, {
            .expr = "!*(e1, *)"
        });

        test_assert(r != NULL);

        {
            ecs_iter_t it = ecs_rule_iter(world, r);
            test_bool(false, ecs_rule_next(&it));
        }

        ecs_rule_fini(r);
    }

    ecs_fini(world);
}

void RulesOperators_1_ent_src_not_pair_rel_any(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_add_pair(world, e1, RelA, TgtA);
    ecs_add_pair(world, e1, RelB, TgtA);

    {
        ecs_rule_t *r = ecs_rule(world, {
            .expr = "!_(e1, TgtA)"
        });

        test_assert(r != NULL);

        {
            ecs_iter_t it = ecs_rule_iter(world, r);
            test_bool(false, ecs_rule_next(&it));
        }

        ecs_rule_fini(r);
    }

    {
        ecs_rule_t *r = ecs_rule(world, {
            .expr = "!_(e1, TgtB)"
        });

        test_assert(r != NULL);

        {
            ecs_iter_t it = ecs_rule_iter(world, r);
            test_bool(true, ecs_rule_next(&it));
            test_uint(0, it.count);
            test_uint(ecs_pair(EcsWildcard, TgtB), ecs_field_id(&it, 1));
            test_uint(e1, ecs_field_src(&it, 1));
            test_bool(false, ecs_field_is_set(&it, 1));
            test_bool(false, ecs_rule_next(&it));
        }

        ecs_rule_fini(r);
    }

    ecs_fini(world);
}

void RulesOperators_1_ent_src_not_pair_tgt_any(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_add_pair(world, e1, RelA, TgtA);
    ecs_add_pair(world, e1, RelA, TgtB);

    {
        ecs_rule_t *r = ecs_rule(world, {
            .expr = "!RelA(e1, _)"
        });

        test_assert(r != NULL);

        {
            ecs_iter_t it = ecs_rule_iter(world, r);
            test_bool(false, ecs_rule_next(&it));
        }

        ecs_rule_fini(r);
    }

    {
        ecs_rule_t *r = ecs_rule(world, {
            .expr = "!RelB(e1, _)"
        });

        test_assert(r != NULL);

        {
            ecs_iter_t it = ecs_rule_iter(world, r);
            test_bool(true, ecs_rule_next(&it));
            test_uint(0, it.count);
            test_uint(ecs_pair(RelB, EcsWildcard), ecs_field_id(&it, 1));
            test_uint(e1, ecs_field_src(&it, 1));
            test_bool(false, ecs_field_is_set(&it, 1));
            test_bool(false, ecs_rule_next(&it));
        }

        ecs_rule_fini(r);
    }

    ecs_fini(world);
}

void RulesOperators_1_ent_src_not_pair_rel_tgt_any(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_add_pair(world, e1, RelA, TgtA);
    ecs_add_pair(world, e1, RelA, TgtB);

    {
        ecs_rule_t *r = ecs_rule(world, {
            .expr = "!_(e1, _)"
        });

        test_assert(r != NULL);

        {
            ecs_iter_t it = ecs_rule_iter(world, r);
            test_bool(false, ecs_rule_next(&it));
        }

        ecs_rule_fini(r);
    }

    ecs_fini(world);
}

void RulesOperators_1_ent_src_not_pair_rel_var(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_add_pair(world, e1, RelA, TgtA);
    ecs_add_pair(world, e1, RelB, TgtA);

    {
        ecs_rule_t *r = ecs_rule(world, {
            .expr = "!$x(e1, TgtA)"
        });

        test_assert(r != NULL);

        int x_var = ecs_rule_find_var(r, "x");
        test_assert(x_var != -1);

        {
            ecs_iter_t it = ecs_rule_iter(world, r);
            test_bool(false, ecs_rule_next(&it));
        }

        ecs_rule_fini(r);
    }

    {
        ecs_rule_t *r = ecs_rule(world, {
            .expr = "!$x(e1, TgtB)"
        });

        test_assert(r != NULL);

        int x_var = ecs_rule_find_var(r, "x");
        test_assert(x_var != -1);

        {
            ecs_iter_t it = ecs_rule_iter(world, r);
            test_bool(true, ecs_rule_next(&it));
            test_uint(0, it.count);
            test_uint(ecs_pair(EcsWildcard, TgtB), ecs_field_id(&it, 1));
            test_uint(e1, ecs_field_src(&it, 1));
            test_bool(false, ecs_field_is_set(&it, 1));
            test_uint(EcsWildcard, ecs_iter_get_var(&it, x_var));
            test_bool(false, ecs_rule_next(&it));
        }

        ecs_rule_fini(r);
    }

    ecs_fini(world);
}

void RulesOperators_1_ent_src_not_pair_tgt_var(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_add_pair(world, e1, RelA, TgtA);
    ecs_add_pair(world, e1, RelA, TgtB);

    {
        ecs_rule_t *r = ecs_rule(world, {
            .expr = "!RelA(e1, $x)"
        });

        test_assert(r != NULL);

        int x_var = ecs_rule_find_var(r, "x");
        test_assert(x_var != -1);

        {
            ecs_iter_t it = ecs_rule_iter(world, r);
            test_bool(false, ecs_rule_next(&it));
        }

        ecs_rule_fini(r);
    }

    {
        ecs_rule_t *r = ecs_rule(world, {
            .expr = "!RelB(e1, $x)"
        });

        test_assert(r != NULL);

        int x_var = ecs_rule_find_var(r, "x");
        test_assert(x_var != -1);

        {
            ecs_iter_t it = ecs_rule_iter(world, r);
            test_bool(true, ecs_rule_next(&it));
            test_uint(0, it.count);
            test_uint(ecs_pair(RelB, EcsWildcard), ecs_field_id(&it, 1));
            test_uint(e1, ecs_field_src(&it, 1));
            test_bool(false, ecs_field_is_set(&it, 1));
            test_uint(EcsWildcard, ecs_iter_get_var(&it, x_var));
            test_bool(false, ecs_rule_next(&it));
        }

        ecs_rule_fini(r);
    }

    ecs_fini(world);
}

void RulesOperators_1_ent_src_not_pair_rel_tgt_var(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_add_pair(world, e1, RelA, TgtA);
    ecs_add_pair(world, e1, RelA, TgtB);

    {
        ecs_rule_t *r = ecs_rule(world, {
            .expr = "!$x(e1, $y)"
        });

        test_assert(r != NULL);

        int x_var = ecs_rule_find_var(r, "x");
        test_assert(x_var != -1);
        int y_var = ecs_rule_find_var(r, "y");
        test_assert(y_var != -1);

        {
            ecs_iter_t it = ecs_rule_iter(world, r);
            test_bool(false, ecs_rule_next(&it));
        }

        ecs_rule_fini(r);
    }

    ecs_fini(world);
}

void RulesOperators_1_ent_src_not_pair_rel_tgt_same_var(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_add_pair(world, e1, RelA, TgtA);
    ecs_add_pair(world, e1, RelA, TgtB);

    {
        ecs_rule_t *r = ecs_rule(world, {
            .expr = "!$x(e1, $x)"
        });

        test_assert(r != NULL);

        int x_var = ecs_rule_find_var(r, "x");
        test_assert(x_var != -1);

        {
            ecs_iter_t it = ecs_rule_iter(world, r);
            test_bool(true, ecs_rule_next(&it));
            test_uint(0, it.count);
            test_uint(ecs_pair(EcsWildcard, EcsWildcard), ecs_field_id(&it, 1));
            test_uint(e1, ecs_field_src(&it, 1));
            test_bool(false, ecs_field_is_set(&it, 1));
            test_uint(EcsWildcard, ecs_iter_get_var(&it, x_var));
            test_bool(false, ecs_rule_next(&it));
        }

        ecs_rule_fini(r);
    }

    ecs_fini(world);
}

void RulesOperators_1_this_src_not_pair_rel_var(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "!$x($this, TgtA)"
    });

    test_assert(r != NULL);

    int x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != -1);

    ecs_entity_t e1 = ecs_new_id(world);
    ecs_add_pair(world, e1, RelA, TgtA);
    ecs_add_pair(world, e1, RelA, TgtB);

    ecs_entity_t e2 = ecs_new_id(world);
    ecs_add_pair(world, e2, RelA, TgtB);

    bool e2_found = false;

    ecs_iter_t it = ecs_rule_iter(world, r);
    while (ecs_rule_next(&it)) {
        test_assert(it.count != 0);
        for (int i = 0; i < it.count; i ++) {
            test_assert(it.entities[i] != e1);
            if (it.entities[i] == e2) {
                e2_found = true;
            }
        }

        test_uint(ecs_pair(EcsWildcard, TgtA), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(false, ecs_field_is_set(&it, 1));
        test_uint(EcsWildcard, ecs_iter_get_var(&it, x_var));
    }

    test_assert(e2_found);

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesOperators_1_this_src_not_pair_tgt_var(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "!RelA($this, $x)"
    });

    test_assert(r != NULL);

    int x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != -1);

    ecs_entity_t e1 = ecs_new_id(world);
    ecs_add_pair(world, e1, RelA, TgtA);
    ecs_add_pair(world, e1, RelA, TgtB);

    ecs_entity_t e2 = ecs_new_id(world);
    ecs_add_pair(world, e2, RelB, TgtA);

    bool e2_found = false;

    ecs_iter_t it = ecs_rule_iter(world, r);
    while (ecs_rule_next(&it)) {
        test_assert(it.count != 0);
        for (int i = 0; i < it.count; i ++) {
            test_assert(it.entities[i] != e1);
            if (it.entities[i] == e2) {
                e2_found = true;
            }
        }

        test_uint(ecs_pair(RelA, EcsWildcard), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(false, ecs_field_is_set(&it, 1));
        test_uint(EcsWildcard, ecs_iter_get_var(&it, x_var));
    }

    test_assert(e2_found);

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesOperators_1_this_src_not_pair_rel_tgt_var(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "!$x($this, $y)"
    });

    test_assert(r != NULL);

    int x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != -1);
    int y_var = ecs_rule_find_var(r, "y");
    test_assert(y_var != -1);

    ecs_entity_t e1 = ecs_new_id(world);
    ecs_add_pair(world, e1, RelA, TgtA);
    ecs_add_pair(world, e1, RelA, TgtB);

    ecs_entity_t e2 = ecs_new_id(world);
    ecs_add(world, e2, TgtA);

    bool e2_found = false;

    ecs_iter_t it = ecs_rule_iter(world, r);
    while (ecs_rule_next(&it)) {
        test_assert(it.count != 0);
        for (int i = 0; i < it.count; i ++) {
            test_assert(it.entities[i] != e1);
            if (it.entities[i] == e2) {
                e2_found = true;
            }
        }

        test_uint(ecs_pair(EcsWildcard, EcsWildcard), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(false, ecs_field_is_set(&it, 1));
        test_uint(EcsWildcard, ecs_iter_get_var(&it, x_var));
        test_uint(EcsWildcard, ecs_iter_get_var(&it, y_var));
    }

    test_assert(e2_found);

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesOperators_1_this_src_not_pair_rel_tgt_same_var(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_entity_t e1 = ecs_new_id(world);
    ecs_add_pair(world, e1, RelA, TgtA);
    ecs_add_pair(world, e1, RelA, TgtB);

    ecs_entity_t e2 = ecs_new_id(world);
    ecs_add_pair(world, e2, RelA, TgtA);
    ecs_add_pair(world, e2, RelA, RelA);

    ecs_entity_t e3 = ecs_new_id(world);
    ecs_add(world, e3, TgtA);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "!$x($this, $x)"
    });

    test_assert(r != NULL);

    int x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != -1);

    bool e1_found = false;

    ecs_iter_t it = ecs_rule_iter(world, r);
    while (ecs_rule_next(&it)) {
        test_assert(it.count != 0);
        for (int i = 0; i < it.count; i ++) {
            test_assert(it.entities[i] != e2);
            if (it.entities[i] == e1) {
                e1_found = true;
            }
        }

        test_uint(ecs_pair(EcsWildcard, EcsWildcard), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(false, ecs_field_is_set(&it, 1));
        test_uint(EcsWildcard, ecs_iter_get_var(&it, x_var));
    }

    test_assert(e1_found);

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesOperators_1_ent_src_not_pair_rel_var_written(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);
    ECS_ENTITY(world, RelA, Tag);
    ECS_ENTITY(world, RelB, Tag);
    ECS_ENTITY(world, RelC, Tag);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_add_pair(world, e1, RelA, TgtA);
    ecs_add_pair(world, e1, RelB, TgtA);
    ecs_add_pair(world, e1, RelB, TgtB);
    ecs_add_pair(world, e1, RelC, TgtB);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "Tag($x), !$x(e1, TgtA)"
    });

    test_assert(r != NULL);

    int x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(Tag, ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelC, TgtA), ecs_field_id(&it, 2));
        test_uint(RelC, ecs_field_src(&it, 1));
        test_uint(e1, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_field_is_set(&it, 2));
        test_uint(RelC, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesOperators_1_ent_src_not_pair_tgt_var_written(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);
    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_ENTITY(world, TgtA, Tag);
    ECS_ENTITY(world, TgtB, Tag);
    ECS_ENTITY(world, TgtC, Tag);

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_add_pair(world, e1, RelA, TgtA);
    ecs_add_pair(world, e1, RelA, TgtB);
    ecs_add_pair(world, e1, RelB, TgtB);
    ecs_add_pair(world, e1, RelB, TgtC);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "Tag($x), !RelA(e1, $x)"
    });

    test_assert(r != NULL);

    int x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(Tag, ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelA, TgtC), ecs_field_id(&it, 2));
        test_uint(TgtC, ecs_field_src(&it, 1));
        test_uint(e1, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_field_is_set(&it, 2));
        test_uint(TgtC, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesOperators_1_ent_src_not_pair_rel_tgt_var_written(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);
    ECS_ENTITY(world, RelA, (Tag, TgtA));
    ECS_ENTITY(world, RelB, (Tag, TgtA));
    ECS_ENTITY(world, RelC, (Tag, TgtB));

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_add_pair(world, e1, RelA, TgtA);
    ecs_add_pair(world, e1, RelB, TgtA);
    ecs_add_pair(world, e1, RelC, TgtA);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "Tag($x, $y), !$x(e1, $y)"
    });

    test_assert(r != NULL);

    int x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != -1);
    int y_var = ecs_rule_find_var(r, "y");
    test_assert(y_var != -1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(Tag, TgtB), ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelC, TgtB), ecs_field_id(&it, 2));
        test_uint(RelC, ecs_field_src(&it, 1));
        test_uint(e1, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_field_is_set(&it, 2));
        test_uint(RelC, ecs_iter_get_var(&it, x_var));
        test_uint(TgtB, ecs_iter_get_var(&it, y_var));

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesOperators_1_ent_src_not_pair_rel_tgt_same_var_written(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);
    ECS_TAG(world, RelA);
    ECS_ENTITY(world, RelB, Tag);
    ECS_ENTITY(world, TgtA, Tag);
    ECS_ENTITY(world, TgtB, Tag);

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_add_pair(world, e1, RelA, TgtA);
    ecs_add_pair(world, e1, TgtA, TgtA);
    ecs_add_pair(world, e1, RelB, TgtA);
    ecs_add_pair(world, e1, RelB, TgtB);
    ecs_add_pair(world, e1, TgtB, TgtB);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "Tag($x), !$x(e1, $x)"
    });

    test_assert(r != NULL);

    int x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(Tag, ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelB, RelB), ecs_field_id(&it, 2));
        test_uint(RelB, ecs_field_src(&it, 1));
        test_uint(e1, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_field_is_set(&it, 2));
        test_uint(RelB, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}
