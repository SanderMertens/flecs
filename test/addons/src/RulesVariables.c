#include <addons.h>

void RulesVariables_1_ent_src_w_var() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);

    ecs_entity_t ent = ecs_new_entity(world, "ent");

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "$x(ent)"
    });

    test_assert(r != NULL);

    int32_t x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add(world, ent, RelA);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(ent, ecs_field_src(&it, 1));
        test_uint(RelA, ecs_iter_get_var(&it, x_var));
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add(world, ent, RelB);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(ent, ecs_field_src(&it, 1));
        test_uint(RelA, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(RelB, ecs_field_id(&it, 1));
        test_uint(ent, ecs_field_src(&it, 1));
        test_uint(RelB, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesVariables_1_ent_src_w_pair_rel_var() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, TgtA);

    ecs_entity_t ent = ecs_new_entity(world, "ent");

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "$x(ent, TgtA)"
    });

    test_assert(r != NULL);

    int32_t x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add_pair(world, ent, RelA, TgtA);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 1));
        test_uint(ent, ecs_field_src(&it, 1));
        test_uint(RelA, ecs_iter_get_var(&it, x_var));
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add_pair(world, ent, RelB, TgtA);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 1));
        test_uint(ent, ecs_field_src(&it, 1));
        test_uint(RelA, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(RelB, TgtA), ecs_field_id(&it, 1));
        test_uint(ent, ecs_field_src(&it, 1));
        test_uint(RelB, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesVariables_1_ent_src_w_pair_tgt_var() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_entity_t ent = ecs_new_entity(world, "ent");

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "RelA(ent, $x)"
    });

    test_assert(r != NULL);

    int32_t x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add_pair(world, ent, RelA, TgtA);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 1));
        test_uint(ent, ecs_field_src(&it, 1));
        test_uint(TgtA, ecs_iter_get_var(&it, x_var));
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add_pair(world, ent, RelA, TgtB);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 1));
        test_uint(ent, ecs_field_src(&it, 1));
        test_uint(TgtA, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(RelA, TgtB), ecs_field_id(&it, 1));
        test_uint(ent, ecs_field_src(&it, 1));
        test_uint(TgtB, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesVariables_1_ent_src_w_pair_rel_tgt_var() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_entity_t ent = ecs_new_entity(world, "ent");

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "$x(ent, $y)"
    });

    test_assert(r != NULL);

    int32_t x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != -1);
    int32_t y_var = ecs_rule_find_var(r, "y");
    test_assert(y_var != -1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(ecs_id(EcsIdentifier), EcsName), ecs_field_id(&it, 1));
        test_uint(ent, ecs_field_src(&it, 1));
        test_uint(ecs_id(EcsIdentifier), ecs_iter_get_var(&it, x_var));
        test_uint(EcsName, ecs_iter_get_var(&it, y_var));
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add_pair(world, ent, RelA, TgtA);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(ecs_id(EcsIdentifier), EcsName), ecs_field_id(&it, 1));
        test_uint(ent, ecs_field_src(&it, 1));
        test_uint(ecs_id(EcsIdentifier), ecs_iter_get_var(&it, x_var));
        test_uint(EcsName, ecs_iter_get_var(&it, y_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 1));
        test_uint(ent, ecs_field_src(&it, 1));
        test_uint(RelA, ecs_iter_get_var(&it, x_var));
        test_uint(TgtA, ecs_iter_get_var(&it, y_var));
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add_pair(world, ent, RelB, TgtA);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(ecs_id(EcsIdentifier), EcsName), ecs_field_id(&it, 1));
        test_uint(ent, ecs_field_src(&it, 1));
        test_uint(ecs_id(EcsIdentifier), ecs_iter_get_var(&it, x_var));
        test_uint(EcsName, ecs_iter_get_var(&it, y_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 1));
        test_uint(ent, ecs_field_src(&it, 1));
        test_uint(RelA, ecs_iter_get_var(&it, x_var));
        test_uint(TgtA, ecs_iter_get_var(&it, y_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(RelB, TgtA), ecs_field_id(&it, 1));
        test_uint(ent, ecs_field_src(&it, 1));
        test_uint(RelB, ecs_iter_get_var(&it, x_var));
        test_uint(TgtA, ecs_iter_get_var(&it, y_var));

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add_pair(world, ent, RelA, TgtB);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(ecs_id(EcsIdentifier), EcsName), ecs_field_id(&it, 1));
        test_uint(ent, ecs_field_src(&it, 1));
        test_uint(ecs_id(EcsIdentifier), ecs_iter_get_var(&it, x_var));
        test_uint(EcsName, ecs_iter_get_var(&it, y_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 1));
        test_uint(ent, ecs_field_src(&it, 1));
        test_uint(RelA, ecs_iter_get_var(&it, x_var));
        test_uint(TgtA, ecs_iter_get_var(&it, y_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(RelA, TgtB), ecs_field_id(&it, 1));
        test_uint(ent, ecs_field_src(&it, 1));
        test_uint(RelA, ecs_iter_get_var(&it, x_var));
        test_uint(TgtB, ecs_iter_get_var(&it, y_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(RelB, TgtA), ecs_field_id(&it, 1));
        test_uint(ent, ecs_field_src(&it, 1));
        test_uint(RelB, ecs_iter_get_var(&it, x_var));
        test_uint(TgtA, ecs_iter_get_var(&it, y_var));

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesVariables_1_ent_src_w_pair_rel_tgt_same_var() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_entity_t ent = ecs_new_entity(world, "ent");

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "$x(ent, $x)"
    });

    test_assert(r != NULL);

    int32_t x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add_pair(world, ent, RelA, TgtA);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add_pair(world, ent, TgtA, TgtA);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(TgtA, TgtA), ecs_field_id(&it, 1));
        test_uint(ent, ecs_field_src(&it, 1));
        test_uint(TgtA, ecs_iter_get_var(&it, x_var));
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add_pair(world, ent, TgtB, TgtB);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(TgtA, TgtA), ecs_field_id(&it, 1));
        test_uint(ent, ecs_field_src(&it, 1));
        test_uint(TgtA, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(TgtB, TgtB), ecs_field_id(&it, 1));
        test_uint(ent, ecs_field_src(&it, 1));
        test_uint(TgtB, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesVariables_1_ent_src_w_pair_rel_tgt_same_var_after_write() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_entity_t ent = ecs_new_entity(world, "ent");

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "$x(ent), $x(ent, $x)"
    });

    test_assert(r != NULL);

    int32_t x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add_pair(world, ent, RelA, TgtA);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add_pair(world, ent, TgtA, TgtA);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add(world, ent, TgtA);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(TgtA, ecs_field_id(&it, 1));
        test_uint(ecs_pair(TgtA, TgtA), ecs_field_id(&it, 2));
        test_uint(ent, ecs_field_src(&it, 1));
        test_uint(ent, ecs_field_src(&it, 2));
        test_uint(TgtA, ecs_iter_get_var(&it, x_var));
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add_pair(world, ent, TgtB, TgtB);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(TgtA, ecs_field_id(&it, 1));
        test_uint(ecs_pair(TgtA, TgtA), ecs_field_id(&it, 2));
        test_uint(ent, ecs_field_src(&it, 1));
        test_uint(ent, ecs_field_src(&it, 2));
        test_uint(TgtA, ecs_iter_get_var(&it, x_var));
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add(world, ent, TgtB);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(TgtA, ecs_field_id(&it, 1));
        test_uint(ecs_pair(TgtA, TgtA), ecs_field_id(&it, 2));
        test_uint(ent, ecs_field_src(&it, 1));
        test_uint(ent, ecs_field_src(&it, 2));
        test_uint(TgtA, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(TgtB, ecs_field_id(&it, 1));
        test_uint(ecs_pair(TgtB, TgtB), ecs_field_id(&it, 2));
        test_uint(ent, ecs_field_src(&it, 1));
        test_uint(ent, ecs_field_src(&it, 2));
        test_uint(TgtB, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesVariables_1_this_src_w_var() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);

    ecs_entity_t e1 = ecs_new(world, RelA);
    ecs_entity_t e2 = ecs_new(world, RelA);
    ecs_entity_t e3 = ecs_new(world, RelA);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "RelA($this), $x($this)"
    });

    test_assert(r != NULL);

    int32_t x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(3, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(RelA, ecs_iter_get_var(&it, x_var));
        test_uint(e1, it.entities[0]);
        test_uint(e2, it.entities[1]);
        test_uint(e3, it.entities[2]);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add(world, e2, RelB);
    ecs_add(world, e3, RelB);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(RelA, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(RelA, ecs_iter_get_var(&it, x_var));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(2, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(RelA, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(RelA, ecs_iter_get_var(&it, x_var));
        test_uint(e2, it.entities[0]);
        test_uint(e3, it.entities[1]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(2, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(RelB, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(RelB, ecs_iter_get_var(&it, x_var));
        test_uint(e2, it.entities[0]);
        test_uint(e3, it.entities[1]);

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesVariables_1_this_src_w_pair_rel_var() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_entity_t e1 = ecs_new_w_pair(world, RelA, TgtB);
    ecs_entity_t e2 = ecs_new_w_pair(world, RelA, TgtB);
    ecs_entity_t e3 = ecs_new_w_pair(world, RelA, TgtB);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "$x($this, TgtA)"
    });

    test_assert(r != NULL);

    int32_t x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add_pair(world, e1, RelA, TgtA);
    ecs_add_pair(world, e2, RelA, TgtA);
    ecs_add_pair(world, e3, RelA, TgtA);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(3, it.count);
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(RelA, ecs_iter_get_var(&it, x_var));
        test_uint(e1, it.entities[0]);
        test_uint(e2, it.entities[1]);
        test_uint(e3, it.entities[2]);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add_pair(world, e2, RelB, TgtA);
    ecs_add_pair(world, e3, RelB, TgtA);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(RelA, ecs_iter_get_var(&it, x_var));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(2, it.count);
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(RelA, ecs_iter_get_var(&it, x_var));
        test_uint(e2, it.entities[0]);
        test_uint(e3, it.entities[1]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(2, it.count);
        test_uint(ecs_pair(RelB, TgtA), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(RelB, ecs_iter_get_var(&it, x_var));
        test_uint(e2, it.entities[0]);
        test_uint(e3, it.entities[1]);

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesVariables_1_this_src_w_pair_tgt_var() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_entity_t e1 = ecs_new_w_pair(world, RelB, TgtA);
    ecs_entity_t e2 = ecs_new_w_pair(world, RelB, TgtA);
    ecs_entity_t e3 = ecs_new_w_pair(world, RelB, TgtA);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "RelA($this, $x)"
    });

    test_assert(r != NULL);

    int32_t x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add_pair(world, e1, RelA, TgtA);
    ecs_add_pair(world, e2, RelA, TgtA);
    ecs_add_pair(world, e3, RelA, TgtA);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(3, it.count);
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(TgtA, ecs_iter_get_var(&it, x_var));
        test_uint(e1, it.entities[0]);
        test_uint(e2, it.entities[1]);
        test_uint(e3, it.entities[2]);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add_pair(world, e2, RelA, TgtB);
    ecs_add_pair(world, e3, RelA, TgtB);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(TgtA, ecs_iter_get_var(&it, x_var));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(2, it.count);
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(TgtA, ecs_iter_get_var(&it, x_var));
        test_uint(e2, it.entities[0]);
        test_uint(e3, it.entities[1]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(2, it.count);
        test_uint(ecs_pair(RelA, TgtB), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(TgtB, ecs_iter_get_var(&it, x_var));
        test_uint(e2, it.entities[0]);
        test_uint(e3, it.entities[1]);

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesVariables_1_this_src_w_pair_rel_tgt_var() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);
    ECS_TAG(world, Tag);

    ecs_entity_t e1 = ecs_new(world, Tag);
    ecs_entity_t e2 = ecs_new(world, Tag);
    ecs_entity_t e3 = ecs_new(world, Tag);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "Tag($this), $x($this, $y)"
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

    ecs_add_pair(world, e1, RelA, TgtA);
    ecs_add_pair(world, e2, RelA, TgtA);
    ecs_add_pair(world, e3, RelA, TgtA);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(3, it.count);
        test_uint(Tag, ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(RelA, ecs_iter_get_var(&it, x_var));
        test_uint(TgtA, ecs_iter_get_var(&it, y_var));
        test_uint(e1, it.entities[0]);
        test_uint(e2, it.entities[1]);
        test_uint(e3, it.entities[2]);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add_pair(world, e2, RelA, TgtB);
    ecs_add_pair(world, e3, RelA, TgtB);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(Tag, ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(RelA, ecs_iter_get_var(&it, x_var));
        test_uint(TgtA, ecs_iter_get_var(&it, y_var));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(2, it.count);
        test_uint(Tag, ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(RelA, ecs_iter_get_var(&it, x_var));
        test_uint(TgtA, ecs_iter_get_var(&it, y_var));
        test_uint(e2, it.entities[0]);
        test_uint(e3, it.entities[1]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(2, it.count);
        test_uint(Tag, ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelA, TgtB), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(RelA, ecs_iter_get_var(&it, x_var));
        test_uint(TgtB, ecs_iter_get_var(&it, y_var));
        test_uint(e2, it.entities[0]);
        test_uint(e3, it.entities[1]);

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add_pair(world, e3, RelB, TgtA);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(Tag, ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(RelA, ecs_iter_get_var(&it, x_var));
        test_uint(TgtA, ecs_iter_get_var(&it, y_var));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(Tag, ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(RelA, ecs_iter_get_var(&it, x_var));
        test_uint(TgtA, ecs_iter_get_var(&it, y_var));
        test_uint(e2, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(Tag, ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelA, TgtB), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(RelA, ecs_iter_get_var(&it, x_var));
        test_uint(TgtB, ecs_iter_get_var(&it, y_var));
        test_uint(e2, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(Tag, ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(RelA, ecs_iter_get_var(&it, x_var));
        test_uint(TgtA, ecs_iter_get_var(&it, y_var));
        test_uint(e3, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(Tag, ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelA, TgtB), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(RelA, ecs_iter_get_var(&it, x_var));
        test_uint(TgtB, ecs_iter_get_var(&it, y_var));
        test_uint(e3, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(Tag, ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelB, TgtA), ecs_field_id(&it, 2));
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

void RulesVariables_1_this_src_w_pair_rel_tgt_same_var() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_entity_t e1 = ecs_new_w_pair(world, RelB, TgtA);
    ecs_entity_t e2 = ecs_new_w_pair(world, RelB, TgtA);
    ecs_entity_t e3 = ecs_new_w_pair(world, RelB, TgtA);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "$x($this, $x)"
    });

    test_assert(r != NULL);

    int32_t x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add_pair(world, e1, RelA, TgtA);
    ecs_add_pair(world, e2, RelA, TgtA);
    ecs_add_pair(world, e3, RelA, TgtA);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add_pair(world, e1, TgtA, TgtA);
    ecs_add_pair(world, e2, TgtA, TgtA);
    ecs_add_pair(world, e3, TgtA, TgtA);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(3, it.count);
        test_uint(ecs_pair(TgtA, TgtA), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(TgtA, ecs_iter_get_var(&it, x_var));
        test_uint(e1, it.entities[0]);
        test_uint(e2, it.entities[1]);
        test_uint(e3, it.entities[2]);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add_pair(world, e2, TgtB, TgtB);
    ecs_add_pair(world, e3, TgtB, TgtB);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(TgtA, TgtA), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(TgtA, ecs_iter_get_var(&it, x_var));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(2, it.count);
        test_uint(ecs_pair(TgtA, TgtA), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(TgtA, ecs_iter_get_var(&it, x_var));
        test_uint(e2, it.entities[0]);
        test_uint(e3, it.entities[1]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(2, it.count);
        test_uint(ecs_pair(TgtB, TgtB), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(TgtB, ecs_iter_get_var(&it, x_var));
        test_uint(e2, it.entities[0]);
        test_uint(e3, it.entities[1]);

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesVariables_1_this_src_w_pair_rel_tgt_same_var_after_write() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_entity_t e1 = ecs_new_w_pair(world, RelB, TgtA);
    ecs_entity_t e2 = ecs_new_w_pair(world, RelB, TgtA);
    ecs_entity_t e3 = ecs_new_w_pair(world, RelB, TgtA);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "$x($this), $x($this, $x)"
    });

    test_assert(r != NULL);

    int32_t x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add_pair(world, e1, RelA, TgtA);
    ecs_add_pair(world, e2, RelA, TgtA);
    ecs_add_pair(world, e3, RelA, TgtA);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add_pair(world, e1, TgtA, TgtA);
    ecs_add_pair(world, e2, TgtA, TgtA);
    ecs_add_pair(world, e3, TgtA, TgtA);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add(world, e1, TgtA);
    ecs_add(world, e2, TgtA);
    ecs_add(world, e3, TgtA);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(3, it.count);
        test_uint(TgtA, ecs_field_id(&it, 1));
        test_uint(ecs_pair(TgtA, TgtA), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(TgtA, ecs_iter_get_var(&it, x_var));
        test_uint(e1, it.entities[0]);
        test_uint(e2, it.entities[1]);
        test_uint(e3, it.entities[2]);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add_pair(world, e2, TgtB, TgtB);
    ecs_add_pair(world, e3, TgtB, TgtB);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(TgtA, ecs_field_id(&it, 1));
        test_uint(ecs_pair(TgtA, TgtA), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(TgtA, ecs_iter_get_var(&it, x_var));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(2, it.count);
        test_uint(TgtA, ecs_field_id(&it, 1));
        test_uint(ecs_pair(TgtA, TgtA), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(TgtA, ecs_iter_get_var(&it, x_var));
        test_uint(e2, it.entities[0]);
        test_uint(e3, it.entities[1]);

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add(world, e2, TgtB);
    ecs_add(world, e3, TgtB);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(TgtA, ecs_field_id(&it, 1));
        test_uint(ecs_pair(TgtA, TgtA), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(TgtA, ecs_iter_get_var(&it, x_var));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(2, it.count);
        test_uint(TgtA, ecs_field_id(&it, 1));
        test_uint(ecs_pair(TgtA, TgtA), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(TgtA, ecs_iter_get_var(&it, x_var));
        test_uint(e2, it.entities[0]);
        test_uint(e3, it.entities[1]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(2, it.count);
        test_uint(TgtB, ecs_field_id(&it, 1));
        test_uint(ecs_pair(TgtB, TgtB), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(TgtB, ecs_iter_get_var(&it, x_var));
        test_uint(e2, it.entities[0]);
        test_uint(e3, it.entities[1]);

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesVariables_1_src_id_same_var() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);

    ecs_entity_t e1 = ecs_new(world, TagA);
    ecs_entity_t e2 = ecs_new(world, TagA);
    ecs_entity_t e3 = ecs_new(world, TagA);
    ecs_add_id(world, e3, e1);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "$x($x), TagA($x)"
    });

    test_assert(r != NULL);

    int32_t x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add_id(world, e1, e1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(e1, ecs_field_id(&it, 1));
        test_uint(TagA, ecs_field_id(&it, 2));
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(e1, ecs_field_src(&it, 2));
        test_uint(e1, ecs_iter_get_var(&it, x_var));
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add_id(world, e2, e2);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(e1, ecs_field_id(&it, 1));
        test_uint(TagA, ecs_field_id(&it, 2));
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(e1, ecs_field_src(&it, 2));
        test_uint(e1, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(e2, ecs_field_id(&it, 1));
        test_uint(TagA, ecs_field_id(&it, 2));
        test_uint(e2, ecs_field_src(&it, 1));
        test_uint(e2, ecs_field_src(&it, 2));
        test_uint(e2, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesVariables_1_src_pair_first_same_var() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TgtA);

    ecs_entity_t e1 = ecs_new(world, TagA);
    ecs_entity_t e2 = ecs_new(world, TagA);
    ecs_entity_t e3 = ecs_new(world, TagA);
    ecs_add_pair(world, e3, e1, TgtA);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "$x($x, TgtA), TagA($x)"
    });

    test_assert(r != NULL);

    int32_t x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add_id(world, e1, e1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add_pair(world, e1, e1, TgtA);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(e1, TgtA), ecs_field_id(&it, 1));
        test_uint(TagA, ecs_field_id(&it, 2));
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(e1, ecs_field_src(&it, 2));
        test_uint(e1, ecs_iter_get_var(&it, x_var));
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add_pair(world, e2, e2, TgtA);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(e1, TgtA), ecs_field_id(&it, 1));
        test_uint(TagA, ecs_field_id(&it, 2));
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(e1, ecs_field_src(&it, 2));
        test_uint(e1, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(e2, TgtA), ecs_field_id(&it, 1));
        test_uint(TagA, ecs_field_id(&it, 2));
        test_uint(e2, ecs_field_src(&it, 1));
        test_uint(e2, ecs_field_src(&it, 2));
        test_uint(e2, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesVariables_1_src_pair_second_same_var() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);
    ECS_TAG(world, RelA);

    ecs_entity_t e1 = ecs_new(world, TagA);
    ecs_entity_t e2 = ecs_new(world, TagA);
    ecs_entity_t e3 = ecs_new(world, TagA);
    ecs_add_pair(world, e3, RelA, e1);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "RelA($x, $x), TagA($x)"
    });

    test_assert(r != NULL);

    int32_t x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add_id(world, e1, e1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add_pair(world, e1, RelA, e1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(RelA, e1), ecs_field_id(&it, 1));
        test_uint(TagA, ecs_field_id(&it, 2));
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(e1, ecs_field_src(&it, 2));
        test_uint(e1, ecs_iter_get_var(&it, x_var));
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add_pair(world, e2, RelA, e2);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(RelA, e1), ecs_field_id(&it, 1));
        test_uint(TagA, ecs_field_id(&it, 2));
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(e1, ecs_field_src(&it, 2));
        test_uint(e1, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(RelA, e2), ecs_field_id(&it, 1));
        test_uint(TagA, ecs_field_id(&it, 2));
        test_uint(e2, ecs_field_src(&it, 1));
        test_uint(e2, ecs_field_src(&it, 2));
        test_uint(e2, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesVariables_1_src_pair_first_and_second_same_var() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);
    ECS_TAG(world, RelA);

    ecs_entity_t e1 = ecs_new(world, TagA);
    ecs_entity_t e2 = ecs_new(world, TagA);
    ecs_entity_t e3 = ecs_new(world, TagA);
    ecs_add_pair(world, e3, e1, e1);
    ecs_entity_t e4 = ecs_new(world, TagA);
    ecs_add_pair(world, e4, e4, e1);
    ecs_entity_t e5 = ecs_new(world, TagA);
    ecs_add_pair(world, e5, e1, e5);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "$x($x, $x), TagA($x)"
    });

    test_assert(r != NULL);

    int32_t x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add_id(world, e1, e1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add_pair(world, e1, RelA, e1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add_pair(world, e1, e1, e1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(e1, e1), ecs_field_id(&it, 1));
        test_uint(TagA, ecs_field_id(&it, 2));
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(e1, ecs_field_src(&it, 2));
        test_uint(e1, ecs_iter_get_var(&it, x_var));
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add_pair(world, e2, e2, e2);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(e1, e1), ecs_field_id(&it, 1));
        test_uint(TagA, ecs_field_id(&it, 2));
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(e1, ecs_field_src(&it, 2));
        test_uint(e1, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(e2, e2), ecs_field_id(&it, 1));
        test_uint(TagA, ecs_field_id(&it, 2));
        test_uint(e2, ecs_field_src(&it, 1));
        test_uint(e2, ecs_field_src(&it, 2));
        test_uint(e2, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesVariables_1_src_id_same_var_after_write() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);

    ecs_entity_t e1 = ecs_new(world, TagA);
    ecs_entity_t e2 = ecs_new(world, TagA);
    ecs_entity_t e3 = ecs_new(world, TagA);
    ecs_add_id(world, e3, e1);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "TagA($x), $x($x)"
    });

    test_assert(r != NULL);

    int32_t x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add_id(world, e1, e1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(TagA, ecs_field_id(&it, 1));
        test_uint(e1, ecs_field_id(&it, 2));
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(e1, ecs_field_src(&it, 2));
        test_uint(e1, ecs_iter_get_var(&it, x_var));
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add_id(world, e2, e2);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(TagA, ecs_field_id(&it, 1));
        test_uint(e1, ecs_field_id(&it, 2));
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(e1, ecs_field_src(&it, 2));
        test_uint(e1, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(TagA, ecs_field_id(&it, 1));
        test_uint(e2, ecs_field_id(&it, 2));
        test_uint(e2, ecs_field_src(&it, 1));
        test_uint(e2, ecs_field_src(&it, 2));
        test_uint(e2, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesVariables_1_src_pair_first_same_var_after_write() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);
    ECS_TAG(world, TagA);
    ECS_TAG(world, TgtA);

    ecs_entity_t e1 = ecs_new(world, TagA);
    ecs_entity_t e2 = ecs_new(world, TagA);
    ecs_entity_t e3 = ecs_new(world, TagA);
    ecs_add_pair(world, e3, e1, TgtA);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "TagA($x), $x($x, TgtA)"
    });

    test_assert(r != NULL);

    int32_t x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add_id(world, e1, e1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add_id(world, RelA, TgtA);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add_pair(world, e1, e1, TgtA);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(TagA, ecs_field_id(&it, 1));
        test_uint(ecs_pair(e1, TgtA), ecs_field_id(&it, 2));
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(e1, ecs_field_src(&it, 2));
        test_uint(e1, ecs_iter_get_var(&it, x_var));
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add_pair(world, e2, e2, TgtA);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(TagA, ecs_field_id(&it, 1));
        test_uint(ecs_pair(e1, TgtA), ecs_field_id(&it, 2));
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(e1, ecs_field_src(&it, 2));
        test_uint(e1, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(TagA, ecs_field_id(&it, 1));
        test_uint(ecs_pair(e2, TgtA), ecs_field_id(&it, 2));
        test_uint(e2, ecs_field_src(&it, 1));
        test_uint(e2, ecs_field_src(&it, 2));
        test_uint(e2, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesVariables_1_src_pair_second_same_var_after_write() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);
    ECS_TAG(world, TagA);
    ECS_TAG(world, TgtA);

    ecs_entity_t e1 = ecs_new(world, TagA);
    ecs_entity_t e2 = ecs_new(world, TagA);
    ecs_entity_t e3 = ecs_new(world, TagA);
    ecs_add_pair(world, e3, RelA, e1);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "TagA($x), RelA($x, $x)"
    });

    test_assert(r != NULL);

    int32_t x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add_id(world, e1, e1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add_id(world, RelA, TgtA);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add_pair(world, e1, RelA, e1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(TagA, ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelA, e1), ecs_field_id(&it, 2));
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(e1, ecs_field_src(&it, 2));
        test_uint(e1, ecs_iter_get_var(&it, x_var));
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add_pair(world, e2, RelA, e2);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(TagA, ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelA, e1), ecs_field_id(&it, 2));
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(e1, ecs_field_src(&it, 2));
        test_uint(e1, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(TagA, ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelA, e2), ecs_field_id(&it, 2));
        test_uint(e2, ecs_field_src(&it, 1));
        test_uint(e2, ecs_field_src(&it, 2));
        test_uint(e2, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesVariables_1_src_pair_first_and_second_same_var_after_write() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);
    ECS_TAG(world, TagA);
    ECS_TAG(world, TgtA);

    ecs_entity_t e1 = ecs_new(world, TagA);
    ecs_entity_t e2 = ecs_new(world, TagA);
    ecs_entity_t e3 = ecs_new(world, TagA);
    ecs_add_pair(world, e3, e1, e1);
    ecs_entity_t e4 = ecs_new(world, TagA);
    ecs_add_pair(world, e4, e4, e1);
    ecs_entity_t e5 = ecs_new(world, TagA);
    ecs_add_pair(world, e5, e1, e5);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "TagA($x), $x($x, $x)"
    });

    test_assert(r != NULL);

    int32_t x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add_id(world, e1, e1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add_id(world, RelA, TgtA);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add_pair(world, e1, RelA, e1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add_pair(world, e1, e1, e1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(TagA, ecs_field_id(&it, 1));
        test_uint(ecs_pair(e1, e1), ecs_field_id(&it, 2));
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(e1, ecs_field_src(&it, 2));
        test_uint(e1, ecs_iter_get_var(&it, x_var));
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add_pair(world, e2, e2, e2);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(TagA, ecs_field_id(&it, 1));
        test_uint(ecs_pair(e1, e1), ecs_field_id(&it, 2));
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(e1, ecs_field_src(&it, 2));
        test_uint(e1, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(TagA, ecs_field_id(&it, 1));
        test_uint(ecs_pair(e2, e2), ecs_field_id(&it, 2));
        test_uint(e2, ecs_field_src(&it, 1));
        test_uint(e2, ecs_field_src(&it, 2));
        test_uint(e2, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesVariables_1_src_pair_first_same_var_this() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);

    ecs_entity_t e1 = ecs_new(world, TagA);
    ecs_entity_t e2 = ecs_new(world, TagA);
    ecs_entity_t e3 = ecs_new(world, TagA);
    ecs_add_id(world, e3, e1);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "$this($this), TagA($this)"
    });

    test_assert(r != NULL);

    int32_t this_var = ecs_rule_find_var(r, "This");
    test_assert(this_var != -1);
    test_assert(this_var != 0);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add_id(world, e1, e1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(e1, ecs_field_id(&it, 1));
        test_uint(TagA, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(e1, ecs_iter_get_var(&it, this_var));
        test_uint(e1, it.entities[0]);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add_id(world, e2, e2);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(e1, ecs_field_id(&it, 1));
        test_uint(TagA, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(e1, ecs_iter_get_var(&it, this_var));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(e2, ecs_field_id(&it, 1));
        test_uint(TagA, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(e2, ecs_iter_get_var(&it, this_var));
        test_uint(e2, it.entities[0]);

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesVariables_1_src_pair_second_same_var_this() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TgtA);

    ecs_entity_t e1 = ecs_new(world, TagA);
    ecs_entity_t e2 = ecs_new(world, TagA);
    ecs_entity_t e3 = ecs_new(world, TagA);
    ecs_add_pair(world, e3, e1, TgtA);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "$this($this, TgtA), TagA($this)"
    });

    test_assert(r != NULL);

    int32_t this_var = ecs_rule_find_var(r, "This");
    test_assert(this_var != -1);
    test_assert(this_var != 0);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add_id(world, e1, e1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add_pair(world, e1, e1, TgtA);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(e1, TgtA), ecs_field_id(&it, 1));
        test_uint(TagA, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(e1, ecs_iter_get_var(&it, this_var));
        test_uint(e1, it.entities[0]);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add_pair(world, e2, e2, TgtA);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(e1, TgtA), ecs_field_id(&it, 1));
        test_uint(TagA, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(e1, ecs_iter_get_var(&it, this_var));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(e2, TgtA), ecs_field_id(&it, 1));
        test_uint(TagA, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(e2, ecs_iter_get_var(&it, this_var));
        test_uint(e2, it.entities[0]);

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);

}

void RulesVariables_1_src_pair_first_and_second_same_var_this() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);
    ECS_TAG(world, RelA);

    ecs_entity_t e1 = ecs_new(world, TagA);
    ecs_entity_t e2 = ecs_new(world, TagA);
    ecs_entity_t e3 = ecs_new(world, TagA);
    ecs_add_pair(world, e3, RelA, e1);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "RelA($this, $this), TagA($this)"
    });

    test_assert(r != NULL);

    int32_t this_var = ecs_rule_find_var(r, "This");
    test_assert(this_var != -1);
    test_assert(this_var != 0);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add_id(world, e1, e1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add_pair(world, e1, RelA, e1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(RelA, e1), ecs_field_id(&it, 1));
        test_uint(TagA, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(e1, ecs_iter_get_var(&it, this_var));
        test_uint(e1, it.entities[0]);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add_pair(world, e2, RelA, e2);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(RelA, e1), ecs_field_id(&it, 1));
        test_uint(TagA, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(e1, ecs_iter_get_var(&it, this_var));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(RelA, e2), ecs_field_id(&it, 1));
        test_uint(TagA, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(e2, ecs_iter_get_var(&it, this_var));
        test_uint(e2, it.entities[0]);

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesVariables_1_src_id_same_var_this_after_write() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);

    ecs_entity_t e1 = ecs_new(world, TagA);
    ecs_entity_t e2 = ecs_new(world, TagA);
    ecs_entity_t e3 = ecs_new(world, TagA);
    ecs_add_id(world, e3, e1);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "TagA($this), $this($this)"
    });

    test_assert(r != NULL);

    int32_t this_var = ecs_rule_find_var(r, "This");
    test_assert(this_var != -1);
    test_assert(this_var != 0);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add_id(world, e1, e1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(TagA, ecs_field_id(&it, 1));
        test_uint(e1, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(e1, ecs_iter_get_var(&it, this_var));
        test_uint(e1, it.entities[0]);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add_id(world, e2, e2);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(TagA, ecs_field_id(&it, 1));
        test_uint(e1, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(e1, ecs_iter_get_var(&it, this_var));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(TagA, ecs_field_id(&it, 1));
        test_uint(e2, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(e2, ecs_iter_get_var(&it, this_var));
        test_uint(e2, it.entities[0]);

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesVariables_1_src_pair_first_same_var_this_after_write() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);
    ECS_TAG(world, TagA);
    ECS_TAG(world, TgtA);

    ecs_entity_t e1 = ecs_new(world, TagA);
    ecs_entity_t e2 = ecs_new(world, TagA);
    ecs_entity_t e3 = ecs_new(world, TagA);
    ecs_add_pair(world, e3, e1, TgtA);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "TagA($this), $this($this, TgtA)"
    });

    test_assert(r != NULL);

    int32_t this_var = ecs_rule_find_var(r, "This");
    test_assert(this_var != -1);
    test_assert(this_var != 0);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add_id(world, e1, e1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add_id(world, RelA, TgtA);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add_pair(world, e1, e1, TgtA);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(TagA, ecs_field_id(&it, 1));
        test_uint(ecs_pair(e1, TgtA), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(e1, ecs_iter_get_var(&it, this_var));
        test_uint(e1, it.entities[0]);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add_pair(world, e2, e2, TgtA);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(TagA, ecs_field_id(&it, 1));
        test_uint(ecs_pair(e1, TgtA), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(e1, ecs_iter_get_var(&it, this_var));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(TagA, ecs_field_id(&it, 1));
        test_uint(ecs_pair(e2, TgtA), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(e2, ecs_iter_get_var(&it, this_var));
        test_uint(e2, it.entities[0]);

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesVariables_1_src_pair_second_same_var_this_after_write() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);
    ECS_TAG(world, TagA);
    ECS_TAG(world, TgtA);

    ecs_entity_t e1 = ecs_new(world, TagA);
    ecs_entity_t e2 = ecs_new(world, TagA);
    ecs_entity_t e3 = ecs_new(world, TagA);
    ecs_add_pair(world, e3, RelA, e1);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "TagA($this), RelA($this, $this)"
    });

    test_assert(r != NULL);

    int32_t this_var = ecs_rule_find_var(r, "This");
    test_assert(this_var != -1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add_id(world, e1, e1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add_id(world, RelA, TgtA);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add_pair(world, e1, RelA, e1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(TagA, ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelA, e1), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(e1, ecs_iter_get_var(&it, this_var));
        test_uint(e1, it.entities[0]);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add_pair(world, e2, RelA, e2);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(TagA, ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelA, e1), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(e1, ecs_iter_get_var(&it, this_var));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(TagA, ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelA, e2), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(e2, ecs_iter_get_var(&it, this_var));
        test_uint(e2, it.entities[0]);

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesVariables_1_src_pair_first_and_second_same_var_this_after_write() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);
    ECS_TAG(world, TagA);
    ECS_TAG(world, TgtA);

    ecs_entity_t e1 = ecs_new(world, TagA);
    ecs_entity_t e2 = ecs_new(world, TagA);
    ecs_entity_t e3 = ecs_new(world, TagA);
    ecs_add_pair(world, e3, e1, e1);
    ecs_entity_t e4 = ecs_new(world, TagA);
    ecs_add_pair(world, e4, e4, e1);
    ecs_entity_t e5 = ecs_new(world, TagA);
    ecs_add_pair(world, e5, e1, e5);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "TagA($this), $this($this, $this)"
    });

    test_assert(r != NULL);

    int32_t this_var = ecs_rule_find_var(r, "This");
    test_assert(this_var != -1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add_id(world, e1, e1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add_id(world, RelA, TgtA);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add_pair(world, e1, RelA, e1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add_pair(world, e1, e1, e1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(TagA, ecs_field_id(&it, 1));
        test_uint(ecs_pair(e1, e1), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(e1, ecs_iter_get_var(&it, this_var));
        test_uint(e1, it.entities[0]);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add_pair(world, e2, e2, e2);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(TagA, ecs_field_id(&it, 1));
        test_uint(ecs_pair(e1, e1), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(e1, ecs_iter_get_var(&it, this_var));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(TagA, ecs_field_id(&it, 1));
        test_uint(ecs_pair(e2, e2), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(e2, ecs_iter_get_var(&it, this_var));
        test_uint(e2, it.entities[0]);

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesVariables_1_ent_src_w_this_var() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);

    ecs_entity_t ent = ecs_new_entity(world, "ent");

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "$this(ent)"
    });

    test_assert(r != NULL);

    int this_var = ecs_rule_find_var(r, "This");
    test_assert(this_var != -1);
    test_assert(this_var != 0);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add(world, ent, RelA);
    ecs_add(world, ent, RelB);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(ent, ecs_field_src(&it, 1));
        test_uint(RelA, ecs_iter_get_var(&it, this_var));
        test_uint(RelA, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(RelB, ecs_field_id(&it, 1));
        test_uint(ent, ecs_field_src(&it, 1));
        test_uint(RelB, ecs_iter_get_var(&it, this_var));
        test_uint(RelB, it.entities[0]);

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesVariables_1_ent_src_w_pair_this_rel() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, TgtA);

    ecs_entity_t ent = ecs_new_entity(world, "ent");

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "$this(ent, TgtA)"
    });

    test_assert(r != NULL);

    int this_var = ecs_rule_find_var(r, "This");
    test_assert(this_var != -1);
    test_assert(this_var != 0);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add(world, ent, TgtA);
    ecs_add(world, ent, TgtA);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add_pair(world, ent, RelA, TgtA);
    ecs_add_pair(world, ent, RelB, TgtA);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 1));
        test_uint(ent, ecs_field_src(&it, 1));
        test_uint(RelA, ecs_iter_get_var(&it, this_var));
        test_uint(RelA, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(RelB, TgtA), ecs_field_id(&it, 1));
        test_uint(ent, ecs_field_src(&it, 1));
        test_uint(RelB, ecs_iter_get_var(&it, this_var));
        test_uint(RelB, it.entities[0]);

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesVariables_1_ent_src_w_pair_this_tgt() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_entity_t ent = ecs_new_entity(world, "ent");

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "RelA(ent, $this)"
    });

    test_assert(r != NULL);

    int this_var = ecs_rule_find_var(r, "This");
    test_assert(this_var != -1);
    test_assert(this_var != 0);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add(world, ent, TgtA);
    ecs_add(world, ent, TgtB);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add_pair(world, ent, RelA, TgtA);
    ecs_add_pair(world, ent, RelA, TgtB);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 1));
        test_uint(ent, ecs_field_src(&it, 1));
        test_uint(TgtA, ecs_iter_get_var(&it, this_var));
        test_uint(TgtA, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(RelA, TgtB), ecs_field_id(&it, 1));
        test_uint(ent, ecs_field_src(&it, 1));
        test_uint(TgtB, ecs_iter_get_var(&it, this_var));
        test_uint(TgtB, it.entities[0]);

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesVariables_1_ent_src_w_pair_this_rel_tgt() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_entity_t ent = ecs_new_entity(world, "ent");

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "$this(ent, $this)"
    });

    test_assert(r != NULL);

    int this_var = ecs_rule_find_var(r, "This");
    test_assert(this_var != -1);
    test_assert(this_var != 0);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add(world, ent, TgtA);
    ecs_add(world, ent, TgtB);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add_pair(world, ent, RelA, TgtA);
    ecs_add_pair(world, ent, RelA, TgtB);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add_pair(world, ent, TgtA, TgtA);
    ecs_add_pair(world, ent, TgtB, TgtB);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(TgtA, TgtA), ecs_field_id(&it, 1));
        test_uint(ent, ecs_field_src(&it, 1));
        test_uint(TgtA, ecs_iter_get_var(&it, this_var));
        test_uint(TgtA, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(TgtB, TgtB), ecs_field_id(&it, 1));
        test_uint(ent, ecs_field_src(&it, 1));
        test_uint(TgtB, ecs_iter_get_var(&it, this_var));
        test_uint(TgtB, it.entities[0]);

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesVariables_1_this_src_w_this() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_entity_t ent = ecs_new(world, TagA);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "$this($this), TagA($this)"
    });

    test_assert(r != NULL);

    int this_var = ecs_rule_find_var(r, "This");
    test_assert(this_var != -1);
    test_assert(this_var != 0);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add(world, ent, TagB);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }
    
    ecs_add_id(world, ent, ent);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ent, ecs_field_id(&it, 1));
        test_uint(TagA, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(ent, ecs_iter_get_var(&it, this_var));
        test_uint(ent, it.entities[0]);

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesVariables_1_this_src_w_pair_this_rel_tgt() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_entity_t ent = ecs_new(world, TagA);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "$this($this, $this), TagA($this)"
    });

    test_assert(r != NULL);

    int this_var = ecs_rule_find_var(r, "This");
    test_assert(this_var != -1);
    test_assert(this_var != 0);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add(world, ent, TagB);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }
    
    ecs_add_id(world, ent, ent);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add_pair(world, ent, ent, ent);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(ent, ent), ecs_field_id(&it, 1));
        test_uint(TagA, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(ent, ecs_iter_get_var(&it, this_var));
        test_uint(ent, it.entities[0]);

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesVariables_1_this_src_w_this_after_write() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_entity_t ent = ecs_new(world, TagA);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "TagA($this), $this($this)"
    });

    test_assert(r != NULL);

    int this_var = ecs_rule_find_var(r, "This");
    test_assert(this_var != -1);
    test_assert(this_var != 0);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add(world, ent, TagB);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }
    
    ecs_add_id(world, ent, ent);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(TagA, ecs_field_id(&it, 1));
        test_uint(ent, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(ent, ecs_iter_get_var(&it, this_var));
        test_uint(ent, it.entities[0]);

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesVariables_1_this_src_w_pair_this_rel_tgt_after_write() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_entity_t ent = ecs_new(world, TagA);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "TagA($this), $this($this, $this)"
    });

    test_assert(r != NULL);

    int this_var = ecs_rule_find_var(r, "This");
    test_assert(this_var != -1);
    test_assert(this_var != 0);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add(world, ent, TagB);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }
    
    ecs_add_id(world, ent, ent);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add_pair(world, ent, ent, ent);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(TagA, ecs_field_id(&it, 1));
        test_uint(ecs_pair(ent, ent), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(ent, ecs_iter_get_var(&it, this_var));
        test_uint(ent, it.entities[0]);

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesVariables_2_constrain_src_from_src() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);

    ecs_entity_t e1 = ecs_new(world, RelA);
    ecs_entity_t e2 = ecs_new(world, RelA);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "RelA($x), RelB($x)"
    });

    test_assert(r != NULL);

    int x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add(world, e1, RelB);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(RelB, ecs_field_id(&it, 2));
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(e1, ecs_field_src(&it, 2));
        test_uint(e1, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add(world, e2, RelB);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(RelB, ecs_field_id(&it, 2));
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(e1, ecs_field_src(&it, 2));
        test_uint(e1, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(RelB, ecs_field_id(&it, 2));
        test_uint(e2, ecs_field_src(&it, 1));
        test_uint(e2, ecs_field_src(&it, 2));
        test_uint(e2, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesVariables_2_constrain_rel_from_src_w_ent() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);

    ecs_entity_t e1 = ecs_new(world, RelA);
    ecs_entity_t e2 = ecs_new_entity(world, "e2");
    ecs_entity_t e3 = ecs_new_entity(world, "e3");

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "RelA($x), $x(e2)"
    });

    test_assert(r != NULL);

    int x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add_id(world, e3, e1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add_id(world, e2, e1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(e1, ecs_field_id(&it, 2));
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(e2, ecs_field_src(&it, 2));
        test_uint(e1, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesVariables_2_constrain_rel_from_src_w_var() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);

    ecs_entity_t e1 = ecs_new(world, RelA);
    ecs_entity_t e2 = ecs_new_id(world);
    ecs_entity_t e3 = ecs_new_id(world);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "RelA($x), $x($y)"
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

    ecs_add_id(world, e2, e1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(e1, ecs_field_id(&it, 2));
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(e2, ecs_field_src(&it, 2));
        test_uint(e1, ecs_iter_get_var(&it, x_var));
        test_uint(e2, ecs_iter_get_var(&it, y_var));

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add_id(world, e3, e1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(e1, ecs_field_id(&it, 2));
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(e2, ecs_field_src(&it, 2));
        test_uint(e1, ecs_iter_get_var(&it, x_var));
        test_uint(e2, ecs_iter_get_var(&it, y_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(e1, ecs_field_id(&it, 2));
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(e3, ecs_field_src(&it, 2));
        test_uint(e1, ecs_iter_get_var(&it, x_var));
        test_uint(e3, ecs_iter_get_var(&it, y_var));

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesVariables_2_constrain_rel_from_src_w_this() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);

    ecs_entity_t e1 = ecs_new(world, RelA);
    ecs_entity_t e2 = ecs_new_id(world);
    ecs_entity_t e3 = ecs_new_id(world);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "RelA($x), $x($this)"
    });

    test_assert(r != NULL);

    int x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add_id(world, e2, e1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(e1, ecs_field_id(&it, 2));
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(e1, ecs_iter_get_var(&it, x_var));
        test_uint(e2, it.entities[0]);

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add_id(world, e3, e1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(2, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(e1, ecs_field_id(&it, 2));
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(e1, ecs_iter_get_var(&it, x_var));
        test_uint(e2, it.entities[0]);
        test_uint(e3, it.entities[1]);

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesVariables_2_constrain_pair_rel_from_src_w_ent() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);
    ECS_TAG(world, TgtA);

    ecs_entity_t e1 = ecs_new(world, RelA);
    ecs_entity_t e2 = ecs_new_entity(world, "e2");
    ecs_entity_t e3 = ecs_new_entity(world, "e3");

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "RelA($x), $x(e2, TgtA)"
    });

    test_assert(r != NULL);

    int x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add_id(world, e3, e1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add_id(world, e2, e1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add_pair(world, e3, e1, TgtA);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add_pair(world, e2, e1, TgtA);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(ecs_pair(e1, TgtA), ecs_field_id(&it, 2));
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(e2, ecs_field_src(&it, 2));
        test_uint(e1, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesVariables_2_constrain_pair_rel_from_src_w_var() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);
    ECS_TAG(world, TgtA);

    ecs_entity_t e1 = ecs_new(world, RelA);
    ecs_entity_t e2 = ecs_new_id(world);
    ecs_entity_t e3 = ecs_new_id(world);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "RelA($x), $x($y, TgtA)"
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

    ecs_add_id(world, e3, e1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add_id(world, e2, e1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add_pair(world, e2, e1, TgtA);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(ecs_pair(e1, TgtA), ecs_field_id(&it, 2));
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(e2, ecs_field_src(&it, 2));
        test_uint(e1, ecs_iter_get_var(&it, x_var));
        test_uint(e2, ecs_iter_get_var(&it, y_var));

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add_pair(world, e3, e1, TgtA);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(ecs_pair(e1, TgtA), ecs_field_id(&it, 2));
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(e2, ecs_field_src(&it, 2));
        test_uint(e1, ecs_iter_get_var(&it, x_var));
        test_uint(e2, ecs_iter_get_var(&it, y_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(ecs_pair(e1, TgtA), ecs_field_id(&it, 2));
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(e3, ecs_field_src(&it, 2));
        test_uint(e1, ecs_iter_get_var(&it, x_var));
        test_uint(e3, ecs_iter_get_var(&it, y_var));

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesVariables_2_constrain_pair_rel_from_src_w_this() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);
    ECS_TAG(world, TgtA);

    ecs_entity_t e1 = ecs_new(world, RelA);
    ecs_entity_t e2 = ecs_new_id(world);
    ecs_entity_t e3 = ecs_new_id(world);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "RelA($x), $x($this, TgtA)"
    });

    test_assert(r != NULL);

    int x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add_id(world, e3, e1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add_id(world, e2, e1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add_pair(world, e2, e1, TgtA);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(ecs_pair(e1, TgtA), ecs_field_id(&it, 2));
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(e1, ecs_iter_get_var(&it, x_var));
        test_uint(e2, it.entities[0]);

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add_pair(world, e3, e1, TgtA);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(2, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(ecs_pair(e1, TgtA), ecs_field_id(&it, 2));
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(e1, ecs_iter_get_var(&it, x_var));
        test_uint(e2, it.entities[0]);
        test_uint(e3, it.entities[1]);

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesVariables_2_constrain_pair_tgt_from_src_w_ent() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);
    ECS_TAG(world, TgtA);

    ecs_entity_t e1 = ecs_new(world, RelA);
    ecs_entity_t e2 = ecs_new_entity(world, "e2");
    ecs_entity_t e3 = ecs_new_entity(world, "e3");

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "RelA($x), RelA(e2, $x)"
    });

    test_assert(r != NULL);

    int x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add_id(world, e3, e1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add_id(world, e2, e1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add_pair(world, e3, RelA, e1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add_pair(world, e2, RelA, e1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelA, e1), ecs_field_id(&it, 2));
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(e2, ecs_field_src(&it, 2));
        test_uint(e1, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesVariables_2_constrain_pair_tgt_from_src_w_var() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);
    ECS_TAG(world, TgtA);

    ecs_entity_t e1 = ecs_new(world, RelA);
    ecs_entity_t e2 = ecs_new_id(world);
    ecs_entity_t e3 = ecs_new_id(world);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "RelA($x), RelA($y, $x)"
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

    ecs_add_id(world, e3, e1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add_id(world, e2, e1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add_pair(world, e2, RelA, e1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelA, e1), ecs_field_id(&it, 2));
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(e2, ecs_field_src(&it, 2));
        test_uint(e1, ecs_iter_get_var(&it, x_var));
        test_uint(e2, ecs_iter_get_var(&it, y_var));

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add_pair(world, e3, RelA, e1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelA, e1), ecs_field_id(&it, 2));
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(e2, ecs_field_src(&it, 2));
        test_uint(e1, ecs_iter_get_var(&it, x_var));
        test_uint(e2, ecs_iter_get_var(&it, y_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelA, e1), ecs_field_id(&it, 2));
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(e3, ecs_field_src(&it, 2));
        test_uint(e1, ecs_iter_get_var(&it, x_var));
        test_uint(e3, ecs_iter_get_var(&it, y_var));

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesVariables_2_constrain_pair_tgt_from_src_w_this() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);
    ECS_TAG(world, TgtA);

    ecs_entity_t e1 = ecs_new(world, RelA);
    ecs_entity_t e2 = ecs_new_id(world);
    ecs_entity_t e3 = ecs_new_id(world);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "RelA($x), RelA($this, $x)"
    });

    test_assert(r != NULL);

    int x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add_id(world, e3, e1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add_id(world, e2, e1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add_pair(world, e2, RelA, e1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelA, e1), ecs_field_id(&it, 2));
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(e1, ecs_iter_get_var(&it, x_var));
        test_uint(e2, it.entities[0]);

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add_pair(world, e3, RelA, e1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(2, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelA, e1), ecs_field_id(&it, 2));
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(e1, ecs_iter_get_var(&it, x_var));
        test_uint(e2, it.entities[0]);
        test_uint(e3, it.entities[1]);

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesVariables_2_constrain_pair_rel_tgt_from_src_w_ent() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);
    ECS_TAG(world, TgtA);

    ecs_entity_t e1 = ecs_new(world, RelA);
    ecs_entity_t e2 = ecs_new_entity(world, "e2");
    ecs_entity_t e3 = ecs_new_entity(world, "e3");

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "RelA($x), $x(e2, $x)"
    });

    test_assert(r != NULL);

    int x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add_id(world, e3, e1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add_id(world, e2, e1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add_pair(world, e3, RelA, e1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add_pair(world, e2, RelA, e1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add_pair(world, e2, e1, e1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(ecs_pair(e1, e1), ecs_field_id(&it, 2));
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(e2, ecs_field_src(&it, 2));
        test_uint(e1, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesVariables_2_constrain_pair_rel_tgt_from_src_w_var() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);
    ECS_TAG(world, TgtA);

    ecs_entity_t e1 = ecs_new(world, RelA);
    ecs_entity_t e2 = ecs_new_id(world);
    ecs_entity_t e3 = ecs_new_id(world);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "RelA($x), $x($y, $x)"
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

    ecs_add_id(world, e3, e1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add_id(world, e2, e1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add_pair(world, e2, RelA, e1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add_pair(world, e2, e1, e1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(ecs_pair(e1, e1), ecs_field_id(&it, 2));
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(e2, ecs_field_src(&it, 2));
        test_uint(e1, ecs_iter_get_var(&it, x_var));
        test_uint(e2, ecs_iter_get_var(&it, y_var));

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add_pair(world, e3, e1, e1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(ecs_pair(e1, e1), ecs_field_id(&it, 2));
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(e2, ecs_field_src(&it, 2));
        test_uint(e1, ecs_iter_get_var(&it, x_var));
        test_uint(e2, ecs_iter_get_var(&it, y_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(ecs_pair(e1, e1), ecs_field_id(&it, 2));
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(e3, ecs_field_src(&it, 2));
        test_uint(e1, ecs_iter_get_var(&it, x_var));
        test_uint(e3, ecs_iter_get_var(&it, y_var));

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesVariables_2_constrain_pair_rel_tgt_from_src_w_this() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);
    ECS_TAG(world, TgtA);

    ecs_entity_t e1 = ecs_new(world, RelA);
    ecs_entity_t e2 = ecs_new_id(world);
    ecs_entity_t e3 = ecs_new_id(world);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "RelA($x), $x($this, $x)"
    });

    test_assert(r != NULL);

    int x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add_id(world, e3, e1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add_id(world, e2, e1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add_pair(world, e2, e1, e1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(ecs_pair(e1, e1), ecs_field_id(&it, 2));
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(e1, ecs_iter_get_var(&it, x_var));
        test_uint(e2, it.entities[0]);

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add_pair(world, e3, e1, e1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(2, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(ecs_pair(e1, e1), ecs_field_id(&it, 2));
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(e1, ecs_iter_get_var(&it, x_var));
        test_uint(e2, it.entities[0]);
        test_uint(e3, it.entities[1]);

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesVariables_1_ent_src_set_rel_var() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, RelC);

    ecs_entity_t ent = ecs_new_entity(world, "ent");
    ecs_add(world, ent, RelA);
    ecs_add(world, ent, RelB);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "$x(ent)"
    });

    test_assert(r != NULL);

    int x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        ecs_iter_set_var(&it, x_var, RelA);

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(ent, ecs_field_src(&it, 1));
        test_uint(RelA, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesVariables_1_ent_src_set_pair_rel_var() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, RelC);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_entity_t ent = ecs_new_entity(world, "ent");
    ecs_add_pair(world, ent, RelA, TgtA);
    ecs_add_pair(world, ent, RelB, TgtA);
    ecs_add_pair(world, ent, RelC, TgtB);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "$x(ent, TgtA)"
    });

    test_assert(r != NULL);

    int x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        ecs_iter_set_var(&it, x_var, RelA);

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 1));
        test_uint(ent, ecs_field_src(&it, 1));
        test_uint(RelA, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_rule_next(&it));
    }

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        ecs_iter_set_var(&it, x_var, RelB);

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(RelB, TgtA), ecs_field_id(&it, 1));
        test_uint(ent, ecs_field_src(&it, 1));
        test_uint(RelB, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_rule_next(&it));
    }

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        ecs_iter_set_var(&it, x_var, RelC);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesVariables_1_ent_src_set_pair_tgt_var() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);
    ECS_TAG(world, TgtC);

    ecs_entity_t ent = ecs_new_entity(world, "ent");
    ecs_add_pair(world, ent, RelA, TgtA);
    ecs_add_pair(world, ent, RelA, TgtB);
    ecs_add_pair(world, ent, RelB, TgtC);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "RelA(ent, $x)"
    });

    test_assert(r != NULL);

    int x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        ecs_iter_set_var(&it, x_var, TgtA);

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 1));
        test_uint(ent, ecs_field_src(&it, 1));
        test_uint(TgtA, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_rule_next(&it));
    }

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        ecs_iter_set_var(&it, x_var, TgtB);

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(RelA, TgtB), ecs_field_id(&it, 1));
        test_uint(ent, ecs_field_src(&it, 1));
        test_uint(TgtB, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_rule_next(&it));
    }

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        ecs_iter_set_var(&it, x_var, TgtC);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesVariables_1_set_src_var() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, RelC);

    ecs_entity_t e1 = ecs_new(world, RelA);
    ecs_entity_t e2 = ecs_new(world, RelB);
    ecs_entity_t e3 = ecs_new(world, RelC);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "RelA($x)"
    });

    test_assert(r != NULL);

    int x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        ecs_iter_set_var(&it, x_var, e1);

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(e1, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_rule_next(&it));
    }

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        ecs_iter_set_var(&it, x_var, e2);
        test_bool(false, ecs_rule_next(&it));
    }

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        ecs_iter_set_var(&it, x_var, e3);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesVariables_1_set_src_var_w_pair() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, RelC);
    ECS_TAG(world, TgtA);

    ecs_entity_t e1 = ecs_new_w_pair(world, RelA, TgtA);
    ecs_entity_t e2 = ecs_new_w_pair(world, RelB, TgtA);
    ecs_entity_t e3 = ecs_new_w_pair(world, RelC, TgtA);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "RelA($x, TgtA)"
    });

    test_assert(r != NULL);

    int x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        ecs_iter_set_var(&it, x_var, e1);

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 1));
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(e1, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_rule_next(&it));
    }

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        ecs_iter_set_var(&it, x_var, e2);
        test_bool(false, ecs_rule_next(&it));
    }

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        ecs_iter_set_var(&it, x_var, e3);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesVariables_1_set_src_var_w_pair_set_rel() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, RelC);
    ECS_TAG(world, TgtA);

    ecs_entity_t e1 = ecs_new_id(world);
    ecs_entity_t e2 = ecs_new_id(world);
    ecs_entity_t e3 = ecs_new_id(world);

    ecs_add_pair(world, e1, e1, TgtA);
    ecs_add_pair(world, e2, e2, TgtA);
    ecs_add_pair(world, e3, RelA, TgtA);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "$x($x, TgtA)"
    });

    test_assert(r != NULL);

    int x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        ecs_iter_set_var(&it, x_var, e1);

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(e1, TgtA), ecs_field_id(&it, 1));
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(e1, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_rule_next(&it));
    }

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        ecs_iter_set_var(&it, x_var, e2);

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(e2, TgtA), ecs_field_id(&it, 1));
        test_uint(e2, ecs_field_src(&it, 1));
        test_uint(e2, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_rule_next(&it));
    }

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        ecs_iter_set_var(&it, x_var, e3);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesVariables_1_set_src_var_w_pair_set_tgt() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);
    ECS_TAG(world, TgtA);

    ecs_entity_t e1 = ecs_new_id(world);
    ecs_entity_t e2 = ecs_new_id(world);
    ecs_entity_t e3 = ecs_new_id(world);

    ecs_add_pair(world, e1, RelA, e1);
    ecs_add_pair(world, e2, RelA, e2);
    ecs_add_pair(world, e3, RelA, TgtA);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "RelA($x, $x)"
    });

    test_assert(r != NULL);

    int x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        ecs_iter_set_var(&it, x_var, e1);

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(RelA, e1), ecs_field_id(&it, 1));
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(e1, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_rule_next(&it));
    }

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        ecs_iter_set_var(&it, x_var, e2);

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(RelA, e2), ecs_field_id(&it, 1));
        test_uint(e2, ecs_field_src(&it, 1));
        test_uint(e2, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_rule_next(&it));
    }

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        ecs_iter_set_var(&it, x_var, e3);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesVariables_1_set_src_var_w_pair_set_rel_tgt() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);
    ECS_TAG(world, TgtA);

    ecs_entity_t e1 = ecs_new_id(world);
    ecs_entity_t e2 = ecs_new_id(world);
    ecs_entity_t e3 = ecs_new_id(world);

    ecs_add_pair(world, e1, e1, e1);
    ecs_add_pair(world, e2, e2, e2);
    ecs_add_pair(world, e3, RelA, e3);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "$x($x, $x)"
    });

    test_assert(r != NULL);

    int x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        ecs_iter_set_var(&it, x_var, e1);

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(e1, e1), ecs_field_id(&it, 1));
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(e1, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_rule_next(&it));
    }

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        ecs_iter_set_var(&it, x_var, e2);

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(e2, e2), ecs_field_id(&it, 1));
        test_uint(e2, ecs_field_src(&it, 1));
        test_uint(e2, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_rule_next(&it));
    }

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        ecs_iter_set_var(&it, x_var, e3);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesVariables_1_set_src_this() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, RelC);

    ecs_entity_t e1 = ecs_new(world, RelA);
    ecs_entity_t e2 = ecs_new(world, RelB);
    ecs_entity_t e3 = ecs_new(world, RelC);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "RelA($this)"
    });

    test_assert(r != NULL);

    int this_var = ecs_rule_find_var(r, "This");
    test_assert(this_var == 0);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        ecs_iter_set_var(&it, this_var, e1);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(e1, ecs_iter_get_var(&it, this_var));
        test_uint(e1, it.entities[0]);

        test_bool(false, ecs_rule_next(&it));
    }

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        ecs_iter_set_var(&it, this_var, e2);
        test_bool(false, ecs_rule_next(&it));
    }

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        ecs_iter_set_var(&it, this_var, e3);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesVariables_1_set_src_this_w_pair() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, RelC);
    ECS_TAG(world, TgtA);

    ecs_entity_t e1 = ecs_new_w_pair(world, RelA, TgtA);
    ecs_entity_t e2 = ecs_new_w_pair(world, RelB, TgtA);
    ecs_entity_t e3 = ecs_new_w_pair(world, RelC, TgtA);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "RelA($this, TgtA)"
    });

    test_assert(r != NULL);

    int this_var = ecs_rule_find_var(r, "This");
    test_assert(this_var != -1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        ecs_iter_set_var(&it, this_var, e1);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(e1, ecs_iter_get_var(&it, this_var));
        test_uint(e1, it.entities[0]);

        test_bool(false, ecs_rule_next(&it));
    }

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        ecs_iter_set_var(&it, this_var, e2);
        test_bool(false, ecs_rule_next(&it));
    }

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        ecs_iter_set_var(&it, this_var, e3);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesVariables_1_set_src_this_w_pair_set_rel() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, RelC);
    ECS_TAG(world, TgtA);

    ecs_entity_t e1 = ecs_new_id(world);
    ecs_entity_t e2 = ecs_new_id(world);
    ecs_entity_t e3 = ecs_new_id(world);

    ecs_add_pair(world, e1, e1, TgtA);
    ecs_add_pair(world, e2, e2, TgtA);
    ecs_add_pair(world, e3, RelA, TgtA);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "$this($this, TgtA)"
    });

    test_assert(r != NULL);

    int this_var = ecs_rule_find_var(r, "This");
    test_assert(this_var != -1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        ecs_iter_set_var(&it, this_var, e1);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(e1, TgtA), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(e1, ecs_iter_get_var(&it, this_var));
        test_uint(e1, it.entities[0]);

        test_bool(false, ecs_rule_next(&it));
    }

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        ecs_iter_set_var(&it, this_var, e2);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(e2, TgtA), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(e2, ecs_iter_get_var(&it, this_var));
        test_uint(e2, it.entities[0]);

        test_bool(false, ecs_rule_next(&it));
    }

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        ecs_iter_set_var(&it, this_var, e3);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesVariables_1_set_src_this_w_pair_set_tgt() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);
    ECS_TAG(world, TgtA);

    ecs_entity_t e1 = ecs_new_id(world);
    ecs_entity_t e2 = ecs_new_id(world);
    ecs_entity_t e3 = ecs_new_id(world);

    ecs_add_pair(world, e1, RelA, e1);
    ecs_add_pair(world, e2, RelA, e2);
    ecs_add_pair(world, e3, RelA, TgtA);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "RelA($this, $this)"
    });

    test_assert(r != NULL);

    int this_var = ecs_rule_find_var(r, "This");
    test_assert(this_var != -1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        ecs_iter_set_var(&it, this_var, e1);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(RelA, e1), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(e1, ecs_iter_get_var(&it, this_var));
        test_uint(e1, it.entities[0]);

        test_bool(false, ecs_rule_next(&it));
    }

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        ecs_iter_set_var(&it, this_var, e2);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(RelA, e2), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(e2, ecs_iter_get_var(&it, this_var));
        test_uint(e2, it.entities[0]);

        test_bool(false, ecs_rule_next(&it));
    }

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        ecs_iter_set_var(&it, this_var, e3);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesVariables_1_set_src_this_w_pair_set_rel_tgt() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);
    ECS_TAG(world, TgtA);

    ecs_entity_t e1 = ecs_new_id(world);
    ecs_entity_t e2 = ecs_new_id(world);
    ecs_entity_t e3 = ecs_new_id(world);

    ecs_add_pair(world, e1, e1, e1);
    ecs_add_pair(world, e2, e2, e2);
    ecs_add_pair(world, e3, RelA, e3);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "$this($this, $this)"
    });

    test_assert(r != NULL);

    int this_var = ecs_rule_find_var(r, "This");
    test_assert(this_var != -1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        ecs_iter_set_var(&it, this_var, e1);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(e1, e1), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(e1, ecs_iter_get_var(&it, this_var));
        test_uint(e1, it.entities[0]);

        test_bool(false, ecs_rule_next(&it));
    }

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        ecs_iter_set_var(&it, this_var, e2);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(e2, e2), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(e2, ecs_iter_get_var(&it, this_var));
        test_uint(e2, it.entities[0]);

        test_bool(false, ecs_rule_next(&it));
    }

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        ecs_iter_set_var(&it, this_var, e3);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesVariables_2_join_by_rel_var() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, RelC);

    ecs_entity_t e1 = ecs_new(world, RelA);
    ecs_add(world, e1, RelB);
    ecs_add_pair(world, e1, TagA, TagB);

    ecs_entity_t e2 = ecs_new(world, RelA);
    ecs_entity_t e3 = ecs_new(world, RelB);
    ecs_new(world, RelC);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "$x($this), $x($y), TagA($this, TagB)"
    });

    test_assert(r != NULL);

    int x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != -1);

    int y_var = ecs_rule_find_var(r, "y");
    test_assert(y_var != -1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(RelA, ecs_field_id(&it, 2));
        test_uint(ecs_pair(TagA, TagB), ecs_field_id(&it, 3));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(e2, ecs_field_src(&it, 2));
        test_uint(0, ecs_field_src(&it, 3));
        test_uint(RelA, ecs_iter_get_var(&it, x_var));
        test_uint(e2, ecs_iter_get_var(&it, y_var));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(RelA, ecs_field_id(&it, 2));
        test_uint(ecs_pair(TagA, TagB), ecs_field_id(&it, 3));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(e1, ecs_field_src(&it, 2));
        test_uint(0, ecs_field_src(&it, 3));
        test_uint(RelA, ecs_iter_get_var(&it, x_var));
        test_uint(e1, ecs_iter_get_var(&it, y_var));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(RelB, ecs_field_id(&it, 1));
        test_uint(RelB, ecs_field_id(&it, 2));
        test_uint(ecs_pair(TagA, TagB), ecs_field_id(&it, 3));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(e1, ecs_field_src(&it, 2));
        test_uint(0, ecs_field_src(&it, 3));
        test_uint(RelB, ecs_iter_get_var(&it, x_var));
        test_uint(e1, ecs_iter_get_var(&it, y_var));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(RelB, ecs_field_id(&it, 1));
        test_uint(RelB, ecs_field_id(&it, 2));
        test_uint(ecs_pair(TagA, TagB), ecs_field_id(&it, 3));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(e3, ecs_field_src(&it, 2));
        test_uint(0, ecs_field_src(&it, 3));
        test_uint(RelB, ecs_iter_get_var(&it, x_var));
        test_uint(e3, ecs_iter_get_var(&it, y_var));
        test_uint(e1, it.entities[0]);

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesVariables_2_join_by_pair_rel_var() {
    ecs_world_t *world = ecs_init();

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

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "$x($this, TgtA), $x($y, TgtA), TagA"
    });

    test_assert(r != NULL);

    int x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != -1);

    int y_var = ecs_rule_find_var(r, "y");
    test_assert(y_var != -1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 2));
        test_uint(TagA, ecs_field_id(&it, 3));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(e2, ecs_field_src(&it, 2));
        test_uint(0, ecs_field_src(&it, 3));
        test_uint(RelA, ecs_iter_get_var(&it, x_var));
        test_uint(e2, ecs_iter_get_var(&it, y_var));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 2));
        test_uint(TagA, ecs_field_id(&it, 3));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(e1, ecs_field_src(&it, 2));
        test_uint(0, ecs_field_src(&it, 3));
        test_uint(RelA, ecs_iter_get_var(&it, x_var));
        test_uint(e1, ecs_iter_get_var(&it, y_var));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(RelB, TgtA), ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelB, TgtA), ecs_field_id(&it, 2));
        test_uint(TagA, ecs_field_id(&it, 3));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(e1, ecs_field_src(&it, 2));
        test_uint(0, ecs_field_src(&it, 3));
        test_uint(RelB, ecs_iter_get_var(&it, x_var));
        test_uint(e1, ecs_iter_get_var(&it, y_var));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(RelB, TgtA), ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelB, TgtA), ecs_field_id(&it, 2));
        test_uint(TagA, ecs_field_id(&it, 3));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(e3, ecs_field_src(&it, 2));
        test_uint(0, ecs_field_src(&it, 3));
        test_uint(RelB, ecs_iter_get_var(&it, x_var));
        test_uint(e3, ecs_iter_get_var(&it, y_var));
        test_uint(e1, it.entities[0]);

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesVariables_2_join_by_pair_tgt_var() {
    ecs_world_t *world = ecs_init();

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

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "RelA($this, $x), RelA($y, $x), TagA"
    });

    test_assert(r != NULL);

    int x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != -1);

    int y_var = ecs_rule_find_var(r, "y");
    test_assert(y_var != -1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 1));
        test_uint(TagA, ecs_field_id(&it, 3));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(e2, ecs_field_src(&it, 2));
        test_uint(0, ecs_field_src(&it, 3));
        test_uint(TgtA, ecs_iter_get_var(&it, x_var));
        test_uint(e2, ecs_iter_get_var(&it, y_var));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelA, TgtA), ecs_field_id(&it, 2));
        test_uint(TagA, ecs_field_id(&it, 3));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(e1, ecs_field_src(&it, 2));
        test_uint(0, ecs_field_src(&it, 3));
        test_uint(TgtA, ecs_iter_get_var(&it, x_var));
        test_uint(e1, ecs_iter_get_var(&it, y_var));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(RelA, TgtB), ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelA, TgtB), ecs_field_id(&it, 2));
        test_uint(TagA, ecs_field_id(&it, 3));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(e1, ecs_field_src(&it, 2));
        test_uint(0, ecs_field_src(&it, 3));
        test_uint(TgtB, ecs_iter_get_var(&it, x_var));
        test_uint(e1, ecs_iter_get_var(&it, y_var));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(RelA, TgtB), ecs_field_id(&it, 1));
        test_uint(ecs_pair(RelA, TgtB), ecs_field_id(&it, 2));
        test_uint(TagA, ecs_field_id(&it, 3));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(e3, ecs_field_src(&it, 2));
        test_uint(0, ecs_field_src(&it, 3));
        test_uint(TgtB, ecs_iter_get_var(&it, x_var));
        test_uint(e3, ecs_iter_get_var(&it, y_var));
        test_uint(e1, it.entities[0]);

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesVariables_2_cycle_w_var() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);

    ecs_entity_t e1 = ecs_new(world, TagA);
    ecs_entity_t e2 = ecs_new(world, TagA);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "$x($y), $y($x), TagA($x)"
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

    ecs_add_id(world, e1, e2);
    ecs_add_id(world, e2, e1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(e2, ecs_field_id(&it, 1));
        test_uint(e1, ecs_field_id(&it, 2));
        test_uint(TagA, ecs_field_id(&it, 3));
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(e2, ecs_field_src(&it, 2));
        test_uint(e2, ecs_field_src(&it, 3));
        test_uint(e2, ecs_iter_get_var(&it, x_var));
        test_uint(e1, ecs_iter_get_var(&it, y_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(e1, ecs_field_id(&it, 1));
        test_uint(e2, ecs_field_id(&it, 2));
        test_uint(TagA, ecs_field_id(&it, 3));
        test_uint(e2, ecs_field_src(&it, 1));
        test_uint(e1, ecs_field_src(&it, 2));
        test_uint(e1, ecs_field_src(&it, 3));
        test_uint(e1, ecs_iter_get_var(&it, x_var));
        test_uint(e2, ecs_iter_get_var(&it, y_var));

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesVariables_2_cycle_w_this_var() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);

    ecs_entity_t e1 = ecs_new(world, TagA);
    ecs_entity_t e2 = ecs_new(world, TagA);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "$this($y), $y($this), TagA($this)"
    });

    test_assert(r != NULL);

    int this_var = ecs_rule_find_var(r, "This");
    test_assert(this_var != -1);

    int y_var = ecs_rule_find_var(r, "y");
    test_assert(y_var != -1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add_id(world, e1, e2);
    ecs_add_id(world, e2, e1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(e2, ecs_field_id(&it, 1));
        test_uint(e1, ecs_field_id(&it, 2));
        test_uint(TagA, ecs_field_id(&it, 3));
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(0, ecs_field_src(&it, 3));
        test_uint(e2, ecs_iter_get_var(&it, this_var));
        test_uint(e1, ecs_iter_get_var(&it, y_var));
        test_uint(e2, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(e1, ecs_field_id(&it, 1));
        test_uint(e2, ecs_field_id(&it, 2));
        test_uint(TagA, ecs_field_id(&it, 3));
        test_uint(e2, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(0, ecs_field_src(&it, 3));
        test_uint(e1, ecs_iter_get_var(&it, this_var));
        test_uint(e2, ecs_iter_get_var(&it, y_var));
        test_uint(e1, it.entities[0]);

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesVariables_2_cycle_w_var_this() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);

    ecs_entity_t e1 = ecs_new(world, TagA);
    ecs_entity_t e2 = ecs_new(world, TagA);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "$x($this), $this($x), TagA($this)"
    });

    test_assert(r != NULL);

    int this_var = ecs_rule_find_var(r, "This");
    test_assert(this_var != -1);

    int x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add_id(world, e1, e2);
    ecs_add_id(world, e2, e1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(e2, ecs_field_id(&it, 1));
        test_uint(e1, ecs_field_id(&it, 2));
        test_uint(TagA, ecs_field_id(&it, 3));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(e2, ecs_field_src(&it, 2));
        test_uint(0, ecs_field_src(&it, 3));
        test_uint(e2, ecs_iter_get_var(&it, x_var));
        test_uint(e1, ecs_iter_get_var(&it, this_var));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(e1, ecs_field_id(&it, 1));
        test_uint(e2, ecs_field_id(&it, 2));
        test_uint(TagA, ecs_field_id(&it, 3));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(e1, ecs_field_src(&it, 2));
        test_uint(0, ecs_field_src(&it, 3));
        test_uint(e1, ecs_iter_get_var(&it, x_var));
        test_uint(e2, ecs_iter_get_var(&it, this_var));
        test_uint(e2, it.entities[0]);

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesVariables_2_cycle_pair_w_var() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);

    ecs_entity_t e1 = ecs_new(world, TagA);
    ecs_entity_t e2 = ecs_new(world, TagA);
    ecs_entity_t e3 = ecs_new(world, TagA);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "$x($y, $z), $x($z, $y), TagA($y)"
    });

    test_assert(r != NULL);

    int x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != -1);

    int y_var = ecs_rule_find_var(r, "y");
    test_assert(y_var != -1);

    int z_var = ecs_rule_find_var(r, "z");
    test_assert(z_var != -1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add_pair(world, e2, e1, e3);
    ecs_add_pair(world, e3, e1, e2);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(e1, e3), ecs_field_id(&it, 1));
        test_uint(ecs_pair(e1, e2), ecs_field_id(&it, 2));
        test_uint(TagA, ecs_field_id(&it, 3));
        test_uint(e2, ecs_field_src(&it, 1));
        test_uint(e3, ecs_field_src(&it, 2));
        test_uint(e2, ecs_field_src(&it, 3));
        test_uint(e1, ecs_iter_get_var(&it, x_var));
        test_uint(e2, ecs_iter_get_var(&it, y_var));
        test_uint(e3, ecs_iter_get_var(&it, z_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(e1, e2), ecs_field_id(&it, 1));
        test_uint(ecs_pair(e1, e3), ecs_field_id(&it, 2));
        test_uint(TagA, ecs_field_id(&it, 3));
        test_uint(e3, ecs_field_src(&it, 1));
        test_uint(e2, ecs_field_src(&it, 2));
        test_uint(e3, ecs_field_src(&it, 3));
        test_uint(e1, ecs_iter_get_var(&it, x_var));
        test_uint(e3, ecs_iter_get_var(&it, y_var));
        test_uint(e2, ecs_iter_get_var(&it, z_var));

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesVariables_2_cycle_pair_w_this_var_var() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);

    ecs_entity_t e1 = ecs_new(world, TagA);
    ecs_entity_t e2 = ecs_new(world, TagA);
    ecs_entity_t e3 = ecs_new(world, TagA);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "$this($y, $z), $this($z, $y), TagA($y)"
    });

    test_assert(r != NULL);

    int this_var = ecs_rule_find_var(r, "This");
    test_assert(this_var != -1);

    int y_var = ecs_rule_find_var(r, "y");
    test_assert(y_var != -1);

    int z_var = ecs_rule_find_var(r, "z");
    test_assert(z_var != -1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add_pair(world, e2, e1, e3);
    ecs_add_pair(world, e3, e1, e2);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(e1, e3), ecs_field_id(&it, 1));
        test_uint(ecs_pair(e1, e2), ecs_field_id(&it, 2));
        test_uint(TagA, ecs_field_id(&it, 3));
        test_uint(e2, ecs_field_src(&it, 1));
        test_uint(e3, ecs_field_src(&it, 2));
        test_uint(e2, ecs_field_src(&it, 3));
        test_uint(e1, ecs_iter_get_var(&it, this_var));
        test_uint(e2, ecs_iter_get_var(&it, y_var));
        test_uint(e3, ecs_iter_get_var(&it, z_var));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(e1, e2), ecs_field_id(&it, 1));
        test_uint(ecs_pair(e1, e3), ecs_field_id(&it, 2));
        test_uint(TagA, ecs_field_id(&it, 3));
        test_uint(e3, ecs_field_src(&it, 1));
        test_uint(e2, ecs_field_src(&it, 2));
        test_uint(e3, ecs_field_src(&it, 3));
        test_uint(e1, ecs_iter_get_var(&it, this_var));
        test_uint(e3, ecs_iter_get_var(&it, y_var));
        test_uint(e2, ecs_iter_get_var(&it, z_var));
        test_uint(e1, it.entities[0]);

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesVariables_2_cycle_pair_w_var_this_var() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);

    ecs_entity_t e1 = ecs_new(world, TagA);
    ecs_entity_t e2 = ecs_new(world, TagA);
    ecs_entity_t e3 = ecs_new(world, TagA);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "$x($this, $z), $x($z, $this), TagA($this)"
    });

    test_assert(r != NULL);

    int x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != -1);

    int this_var = ecs_rule_find_var(r, "This");
    test_assert(this_var != -1);

    int z_var = ecs_rule_find_var(r, "z");
    test_assert(z_var != -1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add_pair(world, e2, e1, e3);
    ecs_add_pair(world, e3, e1, e2);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(e1, e3), ecs_field_id(&it, 1));
        test_uint(ecs_pair(e1, e2), ecs_field_id(&it, 2));
        test_uint(TagA, ecs_field_id(&it, 3));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(e3, ecs_field_src(&it, 2));
        test_uint(0, ecs_field_src(&it, 3));
        test_uint(e1, ecs_iter_get_var(&it, x_var));
        test_uint(e2, ecs_iter_get_var(&it, this_var));
        test_uint(e3, ecs_iter_get_var(&it, z_var));
        test_uint(e2, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(e1, e2), ecs_field_id(&it, 1));
        test_uint(ecs_pair(e1, e3), ecs_field_id(&it, 2));
        test_uint(TagA, ecs_field_id(&it, 3));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(e2, ecs_field_src(&it, 2));
        test_uint(0, ecs_field_src(&it, 3));
        test_uint(e1, ecs_iter_get_var(&it, x_var));
        test_uint(e3, ecs_iter_get_var(&it, this_var));
        test_uint(e2, ecs_iter_get_var(&it, z_var));
        test_uint(e3, it.entities[0]);

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesVariables_2_cycle_pair_w_var_var_this() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);

    ecs_entity_t e1 = ecs_new(world, TagA);
    ecs_entity_t e2 = ecs_new(world, TagA);
    ecs_entity_t e3 = ecs_new(world, TagA);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "$x($y, $this), $x($this, $y), TagA($y)"
    });

    test_assert(r != NULL);

    int x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != -1);

    int y_var = ecs_rule_find_var(r, "y");
    test_assert(y_var != -1);

    int this_var = ecs_rule_find_var(r, "This");
    test_assert(this_var != -1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add_pair(world, e2, e1, e3);
    ecs_add_pair(world, e3, e1, e2);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(e1, e3), ecs_field_id(&it, 1));
        test_uint(ecs_pair(e1, e2), ecs_field_id(&it, 2));
        test_uint(TagA, ecs_field_id(&it, 3));
        test_uint(e2, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(e2, ecs_field_src(&it, 3));
        test_uint(e1, ecs_iter_get_var(&it, x_var));
        test_uint(e2, ecs_iter_get_var(&it, y_var));
        test_uint(e3, ecs_iter_get_var(&it, this_var));
        test_uint(e3, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(e1, e2), ecs_field_id(&it, 1));
        test_uint(ecs_pair(e1, e3), ecs_field_id(&it, 2));
        test_uint(TagA, ecs_field_id(&it, 3));
        test_uint(e3, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(e3, ecs_field_src(&it, 3));
        test_uint(e1, ecs_iter_get_var(&it, x_var));
        test_uint(e3, ecs_iter_get_var(&it, y_var));
        test_uint(e2, ecs_iter_get_var(&it, this_var));
        test_uint(e2, it.entities[0]);

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesVariables_2_cycle_pair_ent_var_var() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Likes);

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_entity_t e2 = ecs_new_entity(world, "e2");
    ecs_entity_t e3 = ecs_new_entity(world, "e3");

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "Likes($x, $y), Likes($y, $x)"
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

    ecs_add_pair(world, e1, Likes, e2);
    ecs_add_pair(world, e1, Likes, e1);
    ecs_add_pair(world, e2, Likes, e1);
    ecs_add_pair(world, e2, Likes, e3);
    ecs_add_pair(world, e3, Likes, e1);
    ecs_add_pair(world, e3, Likes, e2);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(Likes, e1), ecs_field_id(&it, 1));
        test_uint(ecs_pair(Likes, e1), ecs_field_id(&it, 2));
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(e1, ecs_field_src(&it, 2));
        test_uint(e1, ecs_iter_get_var(&it, x_var));
        test_uint(e1, ecs_iter_get_var(&it, y_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(Likes, e2), ecs_field_id(&it, 1));
        test_uint(ecs_pair(Likes, e1), ecs_field_id(&it, 2));
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(e2, ecs_field_src(&it, 2));
        test_uint(e1, ecs_iter_get_var(&it, x_var));
        test_uint(e2, ecs_iter_get_var(&it, y_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(Likes, e2), ecs_field_id(&it, 1));
        test_uint(ecs_pair(Likes, e3), ecs_field_id(&it, 2));
        test_uint(e3, ecs_field_src(&it, 1));
        test_uint(e2, ecs_field_src(&it, 2));
        test_uint(e3, ecs_iter_get_var(&it, x_var));
        test_uint(e2, ecs_iter_get_var(&it, y_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(Likes, e1), ecs_field_id(&it, 1));
        test_uint(ecs_pair(Likes, e2), ecs_field_id(&it, 2));
        test_uint(e2, ecs_field_src(&it, 1));
        test_uint(e1, ecs_field_src(&it, 2));
        test_uint(e2, ecs_iter_get_var(&it, x_var));
        test_uint(e1, ecs_iter_get_var(&it, y_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(Likes, e3), ecs_field_id(&it, 1));
        test_uint(ecs_pair(Likes, e2), ecs_field_id(&it, 2));
        test_uint(e2, ecs_field_src(&it, 1));
        test_uint(e3, ecs_field_src(&it, 2));
        test_uint(e2, ecs_iter_get_var(&it, x_var));
        test_uint(e3, ecs_iter_get_var(&it, y_var));

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesVariables_2_cycle_pair_ent_this_var() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Likes);

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_entity_t e2 = ecs_new_entity(world, "e2");
    ecs_entity_t e3 = ecs_new_entity(world, "e3");

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "Likes($this, $y), Likes($y, $this)"
    });

    test_assert(r != NULL);

    int this_var = ecs_rule_find_var(r, "This");
    test_assert(this_var != -1);
    test_assert(this_var != 0);

    int y_var = ecs_rule_find_var(r, "y");
    test_assert(y_var != -1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add_pair(world, e1, Likes, e2);
    ecs_add_pair(world, e1, Likes, e1);
    ecs_add_pair(world, e2, Likes, e1);
    ecs_add_pair(world, e2, Likes, e3);
    ecs_add_pair(world, e3, Likes, e1);
    ecs_add_pair(world, e3, Likes, e2);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(Likes, e1), ecs_field_id(&it, 1));
        test_uint(ecs_pair(Likes, e1), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(e1, ecs_field_src(&it, 2));
        test_uint(e1, ecs_iter_get_var(&it, this_var));
        test_uint(e1, ecs_iter_get_var(&it, y_var));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(Likes, e2), ecs_field_id(&it, 1));
        test_uint(ecs_pair(Likes, e1), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(e2, ecs_field_src(&it, 2));
        test_uint(e1, ecs_iter_get_var(&it, this_var));
        test_uint(e2, ecs_iter_get_var(&it, y_var));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(Likes, e2), ecs_field_id(&it, 1));
        test_uint(ecs_pair(Likes, e3), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(e2, ecs_field_src(&it, 2));
        test_uint(e3, ecs_iter_get_var(&it, this_var));
        test_uint(e2, ecs_iter_get_var(&it, y_var));
        test_uint(e3, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(Likes, e1), ecs_field_id(&it, 1));
        test_uint(ecs_pair(Likes, e2), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(e1, ecs_field_src(&it, 2));
        test_uint(e2, ecs_iter_get_var(&it, this_var));
        test_uint(e1, ecs_iter_get_var(&it, y_var));
        test_uint(e2, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(Likes, e3), ecs_field_id(&it, 1));
        test_uint(ecs_pair(Likes, e2), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(e3, ecs_field_src(&it, 2));
        test_uint(e2, ecs_iter_get_var(&it, this_var));
        test_uint(e3, ecs_iter_get_var(&it, y_var));
        test_uint(e2, it.entities[0]);

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesVariables_2_cycle_pair_ent_var_this() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Likes);

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_entity_t e2 = ecs_new_entity(world, "e2");
    ecs_entity_t e3 = ecs_new_entity(world, "e3");

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "Likes($x, $this), Likes($this, $x)"
    });

    test_assert(r != NULL);

    int x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != -1);

    int this_var = ecs_rule_find_var(r, "This");
    test_assert(this_var != -1);
    test_assert(this_var != 0);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add_pair(world, e1, Likes, e2);
    ecs_add_pair(world, e1, Likes, e1);
    ecs_add_pair(world, e2, Likes, e1);
    ecs_add_pair(world, e2, Likes, e3);
    ecs_add_pair(world, e3, Likes, e1);
    ecs_add_pair(world, e3, Likes, e2);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(Likes, e1), ecs_field_id(&it, 1));
        test_uint(ecs_pair(Likes, e1), ecs_field_id(&it, 2));
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(e1, ecs_iter_get_var(&it, x_var));
        test_uint(e1, ecs_iter_get_var(&it, this_var));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(Likes, e2), ecs_field_id(&it, 1));
        test_uint(ecs_pair(Likes, e1), ecs_field_id(&it, 2));
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(e1, ecs_iter_get_var(&it, x_var));
        test_uint(e2, ecs_iter_get_var(&it, this_var));
        test_uint(e2, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(Likes, e2), ecs_field_id(&it, 1));
        test_uint(ecs_pair(Likes, e3), ecs_field_id(&it, 2));
        test_uint(e3, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(e3, ecs_iter_get_var(&it, x_var));
        test_uint(e2, ecs_iter_get_var(&it, this_var));
        test_uint(e2, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(Likes, e1), ecs_field_id(&it, 1));
        test_uint(ecs_pair(Likes, e2), ecs_field_id(&it, 2));
        test_uint(e2, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(e2, ecs_iter_get_var(&it, x_var));
        test_uint(e1, ecs_iter_get_var(&it, this_var));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(Likes, e3), ecs_field_id(&it, 1));
        test_uint(ecs_pair(Likes, e2), ecs_field_id(&it, 2));
        test_uint(e2, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(e2, ecs_iter_get_var(&it, x_var));
        test_uint(e3, ecs_iter_get_var(&it, this_var));
        test_uint(e3, it.entities[0]);

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}
