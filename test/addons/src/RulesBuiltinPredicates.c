#include <addons.h>

void RulesBuiltinPredicates_this_eq_id() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ent = ecs_new_entity(world, "ent");

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "$this == ent"
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(false, ecs_field_is_set(&it, 1));
        test_uint(ent, it.entities[0]);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesBuiltinPredicates_this_eq_name() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ent = ecs_new_entity(world, "ent");

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "$this == \"ent\""
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(false, ecs_field_is_set(&it, 1));
        test_uint(ent, it.entities[0]);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesBuiltinPredicates_this_eq_var() {
    ecs_world_t *world = ecs_init();

    ecs_log_set_level(-4);
    ecs_rule_t *r = ecs_rule(world, {
        .expr = "$this == $x"
    });

    test_assert(r == NULL);

    ecs_fini(world);
}

void RulesBuiltinPredicates_this_eq_id_written() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);

    ecs_entity_t ent_1 = ecs_new_entity(world, "ent_1");
    ecs_add(world, ent_1, RelA);
    ecs_entity_t ent_2 = ecs_new_entity(world, "ent_2");
    ecs_add(world, ent_2, RelA);
    ecs_entity_t ent_3 = ecs_new_entity(world, "ent_3");
    ecs_add(world, ent_3, RelA);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "RelA($this), $this == ent_2"
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(true, ecs_field_is_set(&it, 1));
        test_uint(false, ecs_field_is_set(&it, 2));
        test_uint(ent_2, it.entities[0]);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesBuiltinPredicates_this_eq_id_written_no_match() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);

    ecs_entity_t ent_1 = ecs_new_entity(world, "ent_1");
    ecs_add(world, ent_1, RelA);
    ecs_entity_t ent_2 = ecs_new_entity(world, "ent_2");
    ecs_add(world, ent_2, RelA);
    ecs_entity_t ent_3 = ecs_new_entity(world, "ent_3");
    ecs_add(world, ent_3, RelA);
    ecs_entity_t ent_4 = ecs_new_entity(world, "ent_4");
    ecs_add(world, ent_4, RelB);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "RelB($this), $this == ent_2"
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesBuiltinPredicates_this_eq_name_written() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);

    ecs_entity_t ent_1 = ecs_new_entity(world, "ent_1");
    ecs_add(world, ent_1, RelA);
    ecs_entity_t ent_2 = ecs_new_entity(world, "ent_2");
    ecs_add(world, ent_2, RelA);
    ecs_entity_t ent_3 = ecs_new_entity(world, "ent_3");
    ecs_add(world, ent_3, RelA);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "RelA($this), $this == \"ent_2\""
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(true, ecs_field_is_set(&it, 1));
        test_uint(false, ecs_field_is_set(&it, 2));
        test_uint(ent_2, it.entities[0]);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesBuiltinPredicates_this_eq_name_written_no_match() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);

    ecs_entity_t ent_1 = ecs_new_entity(world, "ent_1");
    ecs_add(world, ent_1, RelA);
    ecs_entity_t ent_2 = ecs_new_entity(world, "ent_2");
    ecs_add(world, ent_2, RelA);
    ecs_entity_t ent_3 = ecs_new_entity(world, "ent_3");
    ecs_add(world, ent_3, RelA);
    ecs_entity_t ent_4 = ecs_new_entity(world, "ent_4");
    ecs_add(world, ent_4, RelB);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "RelB($this), $this == \"ent_2\""
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesBuiltinPredicates_this_eq_var_written() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);

    ecs_entity_t ent_2 = ecs_new_entity(world, "ent_2");
    ecs_entity_t ent_1 = ecs_new_entity(world, "ent_1");
    ecs_add_pair(world, ent_1, RelA, ent_2);
    ecs_add_pair(world, ent_2, RelA, ent_2);
    ecs_entity_t ent_3 = ecs_new_entity(world, "ent_3");
    ecs_add_pair(world, ent_3, RelA, ent_2);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "RelA($this, $x), $this == $x"
    });

    test_assert(r != NULL);
    int x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(RelA, ent_2), ecs_field_id(&it, 1));
        test_uint(true, ecs_field_is_set(&it, 1));
        test_uint(false, ecs_field_is_set(&it, 2));
        test_uint(ent_2, ecs_iter_get_var(&it, x_var));
        test_uint(ent_2, it.entities[0]);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesBuiltinPredicates_var_eq_id() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ent = ecs_new_entity(world, "ent");

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "$x == ent"
    });

    test_assert(r != NULL);
    
    int x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(false, ecs_field_is_set(&it, 1));
        test_uint(ent, ecs_iter_get_var(&it, x_var));
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesBuiltinPredicates_var_eq_name() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ent = ecs_new_entity(world, "ent");

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "$x == \"ent\""
    });

    test_assert(r != NULL);
    
    int x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(false, ecs_field_is_set(&it, 1));
        test_uint(ent, ecs_iter_get_var(&it, x_var));
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesBuiltinPredicates_var_eq_var() {
    ecs_world_t *world = ecs_init();

    ecs_log_set_level(-4);
    ecs_rule_t *r = ecs_rule(world, {
        .expr = "$x == $y"
    });

    test_assert(r == NULL);

    ecs_fini(world);
}

void RulesBuiltinPredicates_var_eq_id_written() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);

    ecs_entity_t ent_1 = ecs_new_entity(world, "ent_1");
    ecs_add(world, ent_1, RelA);
    ecs_entity_t ent_2 = ecs_new_entity(world, "ent_2");
    ecs_add(world, ent_2, RelA);
    ecs_entity_t ent_3 = ecs_new_entity(world, "ent_3");
    ecs_add(world, ent_3, RelA);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "RelA($x), $x == ent_2"
    });

    test_assert(r != NULL);
    
    int x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(true, ecs_field_is_set(&it, 1));
        test_uint(false, ecs_field_is_set(&it, 2));
        test_uint(ent_2, ecs_iter_get_var(&it, x_var));
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesBuiltinPredicates_var_eq_id_written_no_match() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);

    ecs_entity_t ent_1 = ecs_new_entity(world, "ent_1");
    ecs_add(world, ent_1, RelA);
    ecs_entity_t ent_2 = ecs_new_entity(world, "ent_2");
    ecs_add(world, ent_2, RelA);
    ecs_entity_t ent_3 = ecs_new_entity(world, "ent_3");
    ecs_add(world, ent_3, RelA);
    ecs_entity_t ent_4 = ecs_new_entity(world, "ent_4");
    ecs_add(world, ent_4, RelB);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "RelB($x), $x == ent_2"
    });

    test_assert(r != NULL);
    
    int x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesBuiltinPredicates_var_eq_name_written() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);

    ecs_entity_t ent_1 = ecs_new_entity(world, "ent_1");
    ecs_add(world, ent_1, RelA);
    ecs_entity_t ent_2 = ecs_new_entity(world, "ent_2");
    ecs_add(world, ent_2, RelA);
    ecs_entity_t ent_3 = ecs_new_entity(world, "ent_3");
    ecs_add(world, ent_3, RelA);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "RelA($x), $x == \"ent_2\""
    });

    test_assert(r != NULL);
    
    int x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(true, ecs_field_is_set(&it, 1));
        test_uint(false, ecs_field_is_set(&it, 2));
        test_uint(ent_2, ecs_iter_get_var(&it, x_var));
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesBuiltinPredicates_var_eq_name_written_no_match() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);

    ecs_entity_t ent_1 = ecs_new_entity(world, "ent_1");
    ecs_add(world, ent_1, RelA);
    ecs_entity_t ent_2 = ecs_new_entity(world, "ent_2");
    ecs_add(world, ent_2, RelA);
    ecs_entity_t ent_3 = ecs_new_entity(world, "ent_3");
    ecs_add(world, ent_3, RelA);
    ecs_entity_t ent_4 = ecs_new_entity(world, "ent_4");
    ecs_add(world, ent_4, RelB);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "RelB($x), $x == \"ent_2\""
    });

    test_assert(r != NULL);
    
    int x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesBuiltinPredicates_var_eq_var_written() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);

    ecs_entity_t ent_2 = ecs_new_entity(world, "ent_2");
    ecs_entity_t ent_1 = ecs_new_entity(world, "ent_1");
    ecs_add_pair(world, ent_1, RelA, ent_2);
    ecs_add_pair(world, ent_2, RelA, ent_2);
    ecs_entity_t ent_3 = ecs_new_entity(world, "ent_3");
    ecs_add_pair(world, ent_3, RelA, ent_2);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "RelA($x, $y), $x == $y"
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
        test_uint(ecs_pair(RelA, ent_2), ecs_field_id(&it, 1));
        test_uint(true, ecs_field_is_set(&it, 1));
        test_uint(false, ecs_field_is_set(&it, 2));
        test_uint(ent_2, ecs_iter_get_var(&it, x_var));
        test_uint(ent_2, ecs_iter_get_var(&it, y_var));
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesBuiltinPredicates_this_neq_id() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    ecs_entity_t ent_1 = ecs_new_entity(world, "ent_1");
    ecs_entity_t ent_2 = ecs_new_entity(world, "ent_2");
    ecs_entity_t ent_3 = ecs_new_entity(world, "ent_3");
    ecs_entity_t ent_4 = ecs_new_entity(world, "ent_4");
    ecs_add(world, ent_4, Tag);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "$this != ent_2"
    });

    test_assert(r != NULL);

    bool ent_1_found = false;
    bool ent_3_found = false;
    bool ent_4_found = false;

    ecs_iter_t it = ecs_rule_iter(world, r);
    while (ecs_rule_next(&it)) {
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

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesBuiltinPredicates_this_neq_name() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    ecs_entity_t ent_1 = ecs_new_entity(world, "ent_1");
    ecs_entity_t ent_2 = ecs_new_entity(world, "ent_2");
    ecs_entity_t ent_3 = ecs_new_entity(world, "ent_3");
    ecs_entity_t ent_4 = ecs_new_entity(world, "ent_4");
    ecs_add(world, ent_4, Tag);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "$this != \"ent_2\""
    });

    test_assert(r != NULL);

    bool ent_1_found = false;
    bool ent_3_found = false;
    bool ent_4_found = false;

    ecs_iter_t it = ecs_rule_iter(world, r);
    while (ecs_rule_next(&it)) {
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

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesBuiltinPredicates_this_neq_var() {
    ecs_world_t *world = ecs_init();

    ecs_log_set_level(-4);
    ecs_rule_t *r = ecs_rule(world, {
        .expr = "$this != $x"
    });

    test_assert(r == NULL);

    ecs_fini(world);
}

void RulesBuiltinPredicates_this_neq_id_written() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);

    ecs_entity_t ent_1 = ecs_new_entity(world, "ent_1");
    ecs_add(world, ent_1, RelA);
    ecs_entity_t ent_2 = ecs_new_entity(world, "ent_2");
    ecs_add(world, ent_2, RelA);
    ecs_entity_t ent_3 = ecs_new_entity(world, "ent_3");
    ecs_add(world, ent_3, RelA);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "RelA($this), $this != ent_2"
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(true, ecs_field_is_set(&it, 1));
        test_uint(false, ecs_field_is_set(&it, 2));
        test_uint(ent_1, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(true, ecs_field_is_set(&it, 1));
        test_uint(false, ecs_field_is_set(&it, 2));
        test_uint(ent_3, it.entities[0]);

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesBuiltinPredicates_this_neq_id_written_no_match() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);

    ecs_entity_t ent_1 = ecs_new_entity(world, "ent_1");
    ecs_add(world, ent_1, RelA);
    ecs_entity_t ent_2 = ecs_new_entity(world, "ent_2");
    ecs_add(world, ent_2, RelA);
    ecs_entity_t ent_3 = ecs_new_entity(world, "ent_3");
    ecs_add(world, ent_3, RelA);
    ecs_entity_t ent_4 = ecs_new_entity(world, "ent_4");
    ecs_add(world, ent_4, RelB);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "RelB($this), $this != ent_2"
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(RelB, ecs_field_id(&it, 1));
        test_uint(true, ecs_field_is_set(&it, 1));
        test_uint(false, ecs_field_is_set(&it, 2));
        test_uint(ent_4, it.entities[0]);

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesBuiltinPredicates_this_neq_name_written() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);

    ecs_entity_t ent_1 = ecs_new_entity(world, "ent_1");
    ecs_add(world, ent_1, RelA);
    ecs_entity_t ent_2 = ecs_new_entity(world, "ent_2");
    ecs_add(world, ent_2, RelA);
    ecs_entity_t ent_3 = ecs_new_entity(world, "ent_3");
    ecs_add(world, ent_3, RelA);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "RelA($this), $this != \"ent_2\""
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(true, ecs_field_is_set(&it, 1));
        test_uint(false, ecs_field_is_set(&it, 2));
        test_uint(ent_1, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(true, ecs_field_is_set(&it, 1));
        test_uint(false, ecs_field_is_set(&it, 2));
        test_uint(ent_3, it.entities[0]);

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesBuiltinPredicates_this_neq_name_written_no_match() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);

    ecs_entity_t ent_1 = ecs_new_entity(world, "ent_1");
    ecs_add(world, ent_1, RelA);
    ecs_entity_t ent_2 = ecs_new_entity(world, "ent_2");
    ecs_add(world, ent_2, RelA);
    ecs_entity_t ent_3 = ecs_new_entity(world, "ent_3");
    ecs_add(world, ent_3, RelA);
    ecs_entity_t ent_4 = ecs_new_entity(world, "ent_4");
    ecs_add(world, ent_4, RelB);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "RelB($this), $this != \"ent_2\""
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(RelB, ecs_field_id(&it, 1));
        test_uint(true, ecs_field_is_set(&it, 1));
        test_uint(false, ecs_field_is_set(&it, 2));
        test_uint(ent_4, it.entities[0]);

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesBuiltinPredicates_this_neq_var_written() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);

    ecs_entity_t ent_2 = ecs_new_entity(world, "ent_2");
    ecs_entity_t ent_1 = ecs_new_entity(world, "ent_1");
    ecs_add_pair(world, ent_1, RelA, ent_2);
    ecs_add_pair(world, ent_2, RelA, ent_2);
    ecs_entity_t ent_3 = ecs_new_entity(world, "ent_3");
    ecs_add_pair(world, ent_3, RelA, ent_2);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "RelA($this, $x), $this != $x"
    });

    test_assert(r != NULL);
    int x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(RelA, ent_2), ecs_field_id(&it, 1));
        test_uint(true, ecs_field_is_set(&it, 1));
        test_uint(false, ecs_field_is_set(&it, 2));
        test_uint(ent_2, ecs_iter_get_var(&it, x_var));
        test_uint(ent_1, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(RelA, ent_2), ecs_field_id(&it, 1));
        test_uint(true, ecs_field_is_set(&it, 1));
        test_uint(false, ecs_field_is_set(&it, 2));
        test_uint(ent_2, ecs_iter_get_var(&it, x_var));
        test_uint(ent_3, it.entities[0]);

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesBuiltinPredicates_var_neq_id() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    ecs_entity_t ent_1 = ecs_new_entity(world, "ent_1");
    ecs_entity_t ent_2 = ecs_new_entity(world, "ent_2");
    ecs_entity_t ent_3 = ecs_new_entity(world, "ent_3");
    ecs_entity_t ent_4 = ecs_new_entity(world, "ent_4");
    ecs_add(world, ent_4, Tag);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "$x != ent_2"
    });

    test_assert(r != NULL);

    int x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != -1);

    bool ent_1_found = false;
    bool ent_3_found = false;
    bool ent_4_found = false;

    ecs_iter_t it = ecs_rule_iter(world, r);
    while (ecs_rule_next(&it)) {
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

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesBuiltinPredicates_var_neq_name() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    ecs_entity_t ent_1 = ecs_new_entity(world, "ent_1");
    ecs_entity_t ent_2 = ecs_new_entity(world, "ent_2");
    ecs_entity_t ent_3 = ecs_new_entity(world, "ent_3");
    ecs_entity_t ent_4 = ecs_new_entity(world, "ent_4");
    ecs_add(world, ent_4, Tag);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "$x != \"ent_2\""
    });

    test_assert(r != NULL);

    int x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != -1);

    bool ent_1_found = false;
    bool ent_3_found = false;
    bool ent_4_found = false;

    ecs_iter_t it = ecs_rule_iter(world, r);
    while (ecs_rule_next(&it)) {
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

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesBuiltinPredicates_var_neq_var() {
    ecs_world_t *world = ecs_init();

    ecs_log_set_level(-4);
    ecs_rule_t *r = ecs_rule(world, {
        .expr = "$x != $y"
    });

    test_assert(r == NULL);

    ecs_fini(world);
}

void RulesBuiltinPredicates_var_neq_id_written() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);

    ecs_entity_t ent_1 = ecs_new_entity(world, "ent_1");
    ecs_add(world, ent_1, RelA);
    ecs_entity_t ent_2 = ecs_new_entity(world, "ent_2");
    ecs_add(world, ent_2, RelA);
    ecs_entity_t ent_3 = ecs_new_entity(world, "ent_3");
    ecs_add(world, ent_3, RelA);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "RelA($x), $x != ent_2"
    });

    test_assert(r != NULL);
    
    int x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(true, ecs_field_is_set(&it, 1));
        test_uint(false, ecs_field_is_set(&it, 2));
        test_uint(ent_1, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(true, ecs_field_is_set(&it, 1));
        test_uint(false, ecs_field_is_set(&it, 2));
        test_uint(ent_3, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesBuiltinPredicates_var_neq_id_written_no_match() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);

    ecs_entity_t ent_1 = ecs_new_entity(world, "ent_1");
    ecs_add(world, ent_1, RelA);
    ecs_entity_t ent_2 = ecs_new_entity(world, "ent_2");
    ecs_add(world, ent_2, RelA);
    ecs_entity_t ent_3 = ecs_new_entity(world, "ent_3");
    ecs_add(world, ent_3, RelA);
    ecs_entity_t ent_4 = ecs_new_entity(world, "ent_4");
    ecs_add(world, ent_4, RelB);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "RelB($x), $x != ent_2"
    });

    test_assert(r != NULL);
    
    int x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(RelB, ecs_field_id(&it, 1));
        test_uint(true, ecs_field_is_set(&it, 1));
        test_uint(false, ecs_field_is_set(&it, 2));
        test_uint(ent_4, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesBuiltinPredicates_var_neq_name_written() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);

    ecs_entity_t ent_1 = ecs_new_entity(world, "ent_1");
    ecs_add(world, ent_1, RelA);
    ecs_entity_t ent_2 = ecs_new_entity(world, "ent_2");
    ecs_add(world, ent_2, RelA);
    ecs_entity_t ent_3 = ecs_new_entity(world, "ent_3");
    ecs_add(world, ent_3, RelA);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "RelA($x), $x != \"ent_2\""
    });

    test_assert(r != NULL);
    
    int x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(true, ecs_field_is_set(&it, 1));
        test_uint(false, ecs_field_is_set(&it, 2));
        test_uint(ent_1, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(true, ecs_field_is_set(&it, 1));
        test_uint(false, ecs_field_is_set(&it, 2));
        test_uint(ent_3, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesBuiltinPredicates_var_neq_name_written_no_match() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);

    ecs_entity_t ent_1 = ecs_new_entity(world, "ent_1");
    ecs_add(world, ent_1, RelA);
    ecs_entity_t ent_2 = ecs_new_entity(world, "ent_2");
    ecs_add(world, ent_2, RelA);
    ecs_entity_t ent_3 = ecs_new_entity(world, "ent_3");
    ecs_add(world, ent_3, RelA);
    ecs_entity_t ent_4 = ecs_new_entity(world, "ent_4");
    ecs_add(world, ent_4, RelB);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "RelB($x), $x != \"ent_2\""
    });

    test_assert(r != NULL);
    
    int x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(RelB, ecs_field_id(&it, 1));
        test_uint(true, ecs_field_is_set(&it, 1));
        test_uint(false, ecs_field_is_set(&it, 2));
        test_uint(ent_4, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesBuiltinPredicates_var_neq_var_written() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);

    ecs_entity_t ent_2 = ecs_new_entity(world, "ent_2");
    ecs_entity_t ent_1 = ecs_new_entity(world, "ent_1");
    ecs_add_pair(world, ent_1, RelA, ent_2);
    ecs_add_pair(world, ent_2, RelA, ent_2);
    ecs_entity_t ent_3 = ecs_new_entity(world, "ent_3");
    ecs_add_pair(world, ent_3, RelA, ent_2);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "RelA($x, $y), $x != $y"
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
        test_uint(ecs_pair(RelA, ent_2), ecs_field_id(&it, 1));
        test_uint(true, ecs_field_is_set(&it, 1));
        test_uint(false, ecs_field_is_set(&it, 2));
        test_uint(ent_1, ecs_iter_get_var(&it, x_var));
        test_uint(ent_2, ecs_iter_get_var(&it, y_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(RelA, ent_2), ecs_field_id(&it, 1));
        test_uint(true, ecs_field_is_set(&it, 1));
        test_uint(false, ecs_field_is_set(&it, 2));
        test_uint(ent_3, ecs_iter_get_var(&it, x_var));
        test_uint(ent_2, ecs_iter_get_var(&it, y_var));

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesBuiltinPredicates_this_2_neq_id() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    ecs_entity_t ent_1 = ecs_new_entity(world, "ent_1");
    ecs_entity_t ent_2 = ecs_new_entity(world, "ent_2");
    ecs_entity_t ent_3 = ecs_new_entity(world, "ent_3");
    ecs_entity_t ent_4 = ecs_new_entity(world, "ent_4");
    ecs_entity_t ent_5 = ecs_new_entity(world, "ent_5");
    ecs_add(world, ent_5, Tag);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "$this != ent_2, $this != ent_3"
    });

    test_assert(r != NULL);

    bool ent_1_found = false;
    bool ent_4_found = false;
    bool ent_5_found = false;

    ecs_iter_t it = ecs_rule_iter(world, r);
    while (ecs_rule_next(&it)) {
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

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesBuiltinPredicates_this_2_neq_name() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    ecs_entity_t ent_1 = ecs_new_entity(world, "ent_1");
    ecs_entity_t ent_2 = ecs_new_entity(world, "ent_2");
    ecs_entity_t ent_3 = ecs_new_entity(world, "ent_3");
    ecs_entity_t ent_4 = ecs_new_entity(world, "ent_4");
    ecs_entity_t ent_5 = ecs_new_entity(world, "ent_5");
    ecs_add(world, ent_5, Tag);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "$this != \"ent_2\", $this != \"ent_3\""
    });

    test_assert(r != NULL);

    bool ent_1_found = false;
    bool ent_4_found = false;
    bool ent_5_found = false;

    ecs_iter_t it = ecs_rule_iter(world, r);
    while (ecs_rule_next(&it)) {
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

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesBuiltinPredicates_var_2_neq_id() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    ecs_entity_t ent_1 = ecs_new_entity(world, "ent_1");
    ecs_entity_t ent_2 = ecs_new_entity(world, "ent_2");
    ecs_entity_t ent_3 = ecs_new_entity(world, "ent_3");
    ecs_entity_t ent_4 = ecs_new_entity(world, "ent_4");
    ecs_entity_t ent_5 = ecs_new_entity(world, "ent_5");
    ecs_add(world, ent_5, Tag);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "$x != ent_2, $x != ent_3"
    });

    test_assert(r != NULL);

    int x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != -1);

    bool ent_1_found = false;
    bool ent_4_found = false;
    bool ent_5_found = false;

    ecs_iter_t it = ecs_rule_iter(world, r);
    while (ecs_rule_next(&it)) {
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

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesBuiltinPredicates_var_2_neq_name() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    ecs_entity_t ent_1 = ecs_new_entity(world, "ent_1");
    ecs_entity_t ent_2 = ecs_new_entity(world, "ent_2");
    ecs_entity_t ent_3 = ecs_new_entity(world, "ent_3");
    ecs_entity_t ent_4 = ecs_new_entity(world, "ent_4");
    ecs_entity_t ent_5 = ecs_new_entity(world, "ent_5");
    ecs_add(world, ent_5, Tag);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "$x != \"ent_2\", $x != \"ent_3\""
    });

    test_assert(r != NULL);

    int x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != -1);

    bool ent_1_found = false;
    bool ent_4_found = false;
    bool ent_5_found = false;

    ecs_iter_t it = ecs_rule_iter(world, r);
    while (ecs_rule_next(&it)) {
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

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesBuiltinPredicates_this_2_neq_id_written() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);

    ecs_entity_t ent_1 = ecs_new_entity(world, "ent_1");
    ecs_add(world, ent_1, RelA);
    ecs_entity_t ent_2 = ecs_new_entity(world, "ent_2");
    ecs_add(world, ent_2, RelA);
    ecs_entity_t ent_3 = ecs_new_entity(world, "ent_3");
    ecs_add(world, ent_3, RelA);
    ecs_entity_t ent_4 = ecs_new_entity(world, "ent_4");
    ecs_add(world, ent_4, RelA);
    ecs_entity_t ent_5 = ecs_new_entity(world, "ent_5");
    ecs_add(world, ent_5, RelA);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "RelA($this), $this != ent_2, $this != ent_3"
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(true, ecs_field_is_set(&it, 1));
        test_uint(false, ecs_field_is_set(&it, 2));
        test_uint(ent_1, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(2, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(true, ecs_field_is_set(&it, 1));
        test_uint(false, ecs_field_is_set(&it, 2));
        test_uint(ent_4, it.entities[0]);
        test_uint(ent_5, it.entities[1]);

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesBuiltinPredicates_this_2_neq_name_written() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);

    ecs_entity_t ent_1 = ecs_new_entity(world, "ent_1");
    ecs_add(world, ent_1, RelA);
    ecs_entity_t ent_2 = ecs_new_entity(world, "ent_2");
    ecs_add(world, ent_2, RelA);
    ecs_entity_t ent_3 = ecs_new_entity(world, "ent_3");
    ecs_add(world, ent_3, RelA);
    ecs_entity_t ent_4 = ecs_new_entity(world, "ent_4");
    ecs_add(world, ent_4, RelA);
    ecs_entity_t ent_5 = ecs_new_entity(world, "ent_5");
    ecs_add(world, ent_5, RelA);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "RelA($this), $this != \"ent_2\", $this != \"ent_3\""
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(true, ecs_field_is_set(&it, 1));
        test_uint(false, ecs_field_is_set(&it, 2));
        test_uint(ent_1, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(2, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(true, ecs_field_is_set(&it, 1));
        test_uint(false, ecs_field_is_set(&it, 2));
        test_uint(ent_4, it.entities[0]);
        test_uint(ent_5, it.entities[1]);

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesBuiltinPredicates_var_2_neq_id_written() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);

    ecs_entity_t ent_1 = ecs_new_entity(world, "ent_1");
    ecs_add(world, ent_1, RelA);
    ecs_entity_t ent_2 = ecs_new_entity(world, "ent_2");
    ecs_add(world, ent_2, RelA);
    ecs_entity_t ent_3 = ecs_new_entity(world, "ent_3");
    ecs_add(world, ent_3, RelA);
    ecs_entity_t ent_4 = ecs_new_entity(world, "ent_4");
    ecs_add(world, ent_4, RelA);
    ecs_entity_t ent_5 = ecs_new_entity(world, "ent_5");
    ecs_add(world, ent_5, RelA);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "RelA($x), $x != ent_2, $x != ent_3"
    });

    test_assert(r != NULL);

    int x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(true, ecs_field_is_set(&it, 1));
        test_uint(false, ecs_field_is_set(&it, 2));
        test_uint(ent_1, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(true, ecs_field_is_set(&it, 1));
        test_uint(false, ecs_field_is_set(&it, 2));
        test_uint(ent_4, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(true, ecs_field_is_set(&it, 1));
        test_uint(false, ecs_field_is_set(&it, 2));
        test_uint(ent_5, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesBuiltinPredicates_var_2_neq_name_written() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);

    ecs_entity_t ent_1 = ecs_new_entity(world, "ent_1");
    ecs_add(world, ent_1, RelA);
    ecs_entity_t ent_2 = ecs_new_entity(world, "ent_2");
    ecs_add(world, ent_2, RelA);
    ecs_entity_t ent_3 = ecs_new_entity(world, "ent_3");
    ecs_add(world, ent_3, RelA);
    ecs_entity_t ent_4 = ecs_new_entity(world, "ent_4");
    ecs_add(world, ent_4, RelA);
    ecs_entity_t ent_5 = ecs_new_entity(world, "ent_5");
    ecs_add(world, ent_5, RelA);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "RelA($x), $x != \"ent_2\", $x != \"ent_3\""
    });

    test_assert(r != NULL);

    int x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(true, ecs_field_is_set(&it, 1));
        test_uint(false, ecs_field_is_set(&it, 2));
        test_uint(ent_1, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(true, ecs_field_is_set(&it, 1));
        test_uint(false, ecs_field_is_set(&it, 2));
        test_uint(ent_4, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(true, ecs_field_is_set(&it, 1));
        test_uint(false, ecs_field_is_set(&it, 2));
        test_uint(ent_5, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesBuiltinPredicates_this_2_or_id() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);

    /* ecs_entity_t ent_1 = */ ecs_new_entity(world, "ent_1");
    ecs_entity_t ent_2 = ecs_new_entity(world, "ent_2");
    ecs_entity_t ent_3 = ecs_new_entity(world, "ent_3");
    /* ecs_entity_t ent_4 = */ ecs_new_entity(world, "ent_4");
    /* ecs_entity_t ent_5 = */ ecs_new_entity(world, "ent_5");

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "$this == ent_2 || $this == ent_3"
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(false, ecs_field_is_set(&it, 1));
        test_uint(ent_2, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(false, ecs_field_is_set(&it, 1));
        test_uint(ent_3, it.entities[0]);

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesBuiltinPredicates_this_2_or_name() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);

    /* ecs_entity_t ent_1 = */ ecs_new_entity(world, "ent_1");
    ecs_entity_t ent_2 = ecs_new_entity(world, "ent_2");
    ecs_entity_t ent_3 = ecs_new_entity(world, "ent_3");
    /* ecs_entity_t ent_4 = */ ecs_new_entity(world, "ent_4");
    /* ecs_entity_t ent_5 = */ ecs_new_entity(world, "ent_5");

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "$this == \"ent_2\" || $this == \"ent_3\""
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(false, ecs_field_is_set(&it, 1));
        test_uint(ent_2, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(false, ecs_field_is_set(&it, 1));
        test_uint(ent_3, it.entities[0]);

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesBuiltinPredicates_var_2_or_id() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);

    /* ecs_entity_t ent_1 = */ ecs_new_entity(world, "ent_1");
    ecs_entity_t ent_2 = ecs_new_entity(world, "ent_2");
    ecs_entity_t ent_3 = ecs_new_entity(world, "ent_3");
    /* ecs_entity_t ent_4 = */ ecs_new_entity(world, "ent_4");
    /* ecs_entity_t ent_5 = */ ecs_new_entity(world, "ent_5");

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "$x == ent_2 || $x == ent_3"
    });

    test_assert(r != NULL);
    int x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(false, ecs_field_is_set(&it, 1));
        test_uint(ent_2, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(false, ecs_field_is_set(&it, 1));
        test_uint(ent_3, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesBuiltinPredicates_var_2_or_name() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);

    /* ecs_entity_t ent_1 = */ ecs_new_entity(world, "ent_1");
    ecs_entity_t ent_2 = ecs_new_entity(world, "ent_2");
    ecs_entity_t ent_3 = ecs_new_entity(world, "ent_3");
    /* ecs_entity_t ent_4 = */ ecs_new_entity(world, "ent_4");
    /* ecs_entity_t ent_5 = */ ecs_new_entity(world, "ent_5");

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "$x == \"ent_2\" || $x == \"ent_3\""
    });

    test_assert(r != NULL);
    int x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(false, ecs_field_is_set(&it, 1));
        test_uint(ent_2, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(false, ecs_field_is_set(&it, 1));
        test_uint(ent_3, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesBuiltinPredicates_this_2_or_id_written() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);

    ecs_entity_t ent_1 = ecs_new_entity(world, "ent_1");
    ecs_add(world, ent_1, RelA);
    ecs_entity_t ent_2 = ecs_new_entity(world, "ent_2");
    ecs_add(world, ent_2, RelA);
    ecs_entity_t ent_3 = ecs_new_entity(world, "ent_3");
    ecs_add(world, ent_3, RelA);
    ecs_entity_t ent_4 = ecs_new_entity(world, "ent_4");
    ecs_add(world, ent_4, RelA);
    ecs_entity_t ent_5 = ecs_new_entity(world, "ent_5");
    ecs_add(world, ent_5, RelA);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "RelA($this), $this == ent_2 || $this == ent_3"
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(true, ecs_field_is_set(&it, 1));
        test_uint(false, ecs_field_is_set(&it, 2));
        test_uint(ent_2, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(true, ecs_field_is_set(&it, 1));
        test_uint(false, ecs_field_is_set(&it, 2));
        test_uint(ent_3, it.entities[0]);

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesBuiltinPredicates_this_2_or_name_written() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);

    ecs_entity_t ent_1 = ecs_new_entity(world, "ent_1");
    ecs_add(world, ent_1, RelA);
    ecs_entity_t ent_2 = ecs_new_entity(world, "ent_2");
    ecs_add(world, ent_2, RelA);
    ecs_entity_t ent_3 = ecs_new_entity(world, "ent_3");
    ecs_add(world, ent_3, RelA);
    ecs_entity_t ent_4 = ecs_new_entity(world, "ent_4");
    ecs_add(world, ent_4, RelA);
    ecs_entity_t ent_5 = ecs_new_entity(world, "ent_5");
    ecs_add(world, ent_5, RelA);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "RelA($this), $this == \"ent_2\" || $this == \"ent_3\""
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(true, ecs_field_is_set(&it, 1));
        test_uint(false, ecs_field_is_set(&it, 2));
        test_uint(ent_2, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(true, ecs_field_is_set(&it, 1));
        test_uint(false, ecs_field_is_set(&it, 2));
        test_uint(ent_3, it.entities[0]);

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesBuiltinPredicates_var_2_or_id_written() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);

    ecs_entity_t ent_1 = ecs_new_entity(world, "ent_1");
    ecs_add(world, ent_1, RelA);
    ecs_entity_t ent_2 = ecs_new_entity(world, "ent_2");
    ecs_add(world, ent_2, RelA);
    ecs_entity_t ent_3 = ecs_new_entity(world, "ent_3");
    ecs_add(world, ent_3, RelA);
    ecs_entity_t ent_4 = ecs_new_entity(world, "ent_4");
    ecs_add(world, ent_4, RelA);
    ecs_entity_t ent_5 = ecs_new_entity(world, "ent_5");
    ecs_add(world, ent_5, RelA);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "RelA($x), $x == ent_2 || $x == ent_3"
    });

    test_assert(r != NULL);

    int x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(true, ecs_field_is_set(&it, 1));
        test_uint(false, ecs_field_is_set(&it, 2));
        test_uint(ent_2, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(true, ecs_field_is_set(&it, 1));
        test_uint(false, ecs_field_is_set(&it, 2));
        test_uint(ent_3, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesBuiltinPredicates_var_2_or_name_written() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);

    ecs_entity_t ent_1 = ecs_new_entity(world, "ent_1");
    ecs_add(world, ent_1, RelA);
    ecs_entity_t ent_2 = ecs_new_entity(world, "ent_2");
    ecs_add(world, ent_2, RelA);
    ecs_entity_t ent_3 = ecs_new_entity(world, "ent_3");
    ecs_add(world, ent_3, RelA);
    ecs_entity_t ent_4 = ecs_new_entity(world, "ent_4");
    ecs_add(world, ent_4, RelA);
    ecs_entity_t ent_5 = ecs_new_entity(world, "ent_5");
    ecs_add(world, ent_5, RelA);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "RelA($x), $x == \"ent_2\" || $x == \"ent_3\""
    });

    test_assert(r != NULL);

    int x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(true, ecs_field_is_set(&it, 1));
        test_uint(false, ecs_field_is_set(&it, 2));
        test_uint(ent_2, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(RelA, ecs_field_id(&it, 1));
        test_uint(true, ecs_field_is_set(&it, 1));
        test_uint(false, ecs_field_is_set(&it, 2));
        test_uint(ent_3, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesBuiltinPredicates_this_match_eq() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    ecs_entity_t ent_1 = ecs_new_entity(world, "ent_1");
    /* ecs_entity_t ent_2 = */ ecs_new_entity(world, "ent2");
    ecs_entity_t ent_3 = ecs_new_entity(world, "nt_3");
    ecs_entity_t ent_4 = ecs_new_entity(world, "ent_4");
    ecs_entity_t ent_5 = ecs_new_entity(world, "ent5");
    ecs_add(world, ent_5, Tag);
    ecs_entity_t ent_6 = ecs_new_entity(world, "ent_6");
    ecs_add(world, ent_6, Tag);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "$this ~= \"nt_\""
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(false, ecs_field_is_set(&it, 1));
        test_uint(ent_1, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(2, it.count);
        test_uint(false, ecs_field_is_set(&it, 1));
        test_uint(ent_3, it.entities[0]);
        test_uint(ent_4, it.entities[1]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(false, ecs_field_is_set(&it, 1));
        test_uint(ent_6, it.entities[0]);

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesBuiltinPredicates_var_match_eq() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    ecs_entity_t ent_1 = ecs_new_entity(world, "ent_1");
    /* ecs_entity_t ent_2 = */ ecs_new_entity(world, "ent2");
    ecs_entity_t ent_3 = ecs_new_entity(world, "nt_3");
    ecs_entity_t ent_4 = ecs_new_entity(world, "ent_4");
    ecs_entity_t ent_5 = ecs_new_entity(world, "ent5");
    ecs_add(world, ent_5, Tag);
    ecs_entity_t ent_6 = ecs_new_entity(world, "ent_6");
    ecs_add(world, ent_6, Tag);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "$x ~= \"nt_\""
    });

    test_assert(r != NULL);

    int x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(false, ecs_field_is_set(&it, 1));
        test_uint(ent_1, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(false, ecs_field_is_set(&it, 1));
        test_uint(ent_3, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(false, ecs_field_is_set(&it, 1));
        test_uint(ent_4, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(false, ecs_field_is_set(&it, 1));
        test_uint(ent_6, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesBuiltinPredicates_this_match_eq_written() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);
    ECS_TAG(world, RelA);

    ecs_entity_t ent_1 = ecs_new_entity(world, "ent_1");
    ecs_add(world, ent_1, RelA);
    ecs_entity_t ent_2 = ecs_new_entity(world, "ent2");
    ecs_add(world, ent_2, RelA);
    ecs_entity_t ent_3 = ecs_new_entity(world, "nt_3");
    ecs_add(world, ent_3, RelA);
    ecs_entity_t ent_4 = ecs_new_entity(world, "ent_4");
    ecs_add(world, ent_4, RelA);
    ecs_entity_t ent_5 = ecs_new_entity(world, "ent5");
    ecs_add(world, ent_5, RelA);
    ecs_entity_t ent_6 = ecs_new_entity(world, "ent_6");
    ecs_add(world, ent_6, RelA);
    ecs_add(world, ent_6, Tag);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "RelA($this), $this ~= \"nt_\""
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(true, ecs_field_is_set(&it, 1));
        test_uint(false, ecs_field_is_set(&it, 2));
        test_uint(ent_1, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(2, it.count);
        test_uint(true, ecs_field_is_set(&it, 1));
        test_uint(false, ecs_field_is_set(&it, 2));
        test_uint(ent_3, it.entities[0]);
        test_uint(ent_4, it.entities[1]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(true, ecs_field_is_set(&it, 1));
        test_uint(false, ecs_field_is_set(&it, 2));
        test_uint(ent_6, it.entities[0]);

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesBuiltinPredicates_var_match_eq_written() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);
    ECS_TAG(world, RelA);

    ecs_entity_t ent_1 = ecs_new_entity(world, "ent_1");
    ecs_add(world, ent_1, RelA);
    ecs_entity_t ent_2 = ecs_new_entity(world, "ent2");
    ecs_add(world, ent_2, RelA);
    ecs_entity_t ent_3 = ecs_new_entity(world, "nt_3");
    ecs_add(world, ent_3, RelA);
    ecs_entity_t ent_4 = ecs_new_entity(world, "ent_4");
    ecs_add(world, ent_4, RelA);
    ecs_entity_t ent_5 = ecs_new_entity(world, "ent5");
    ecs_add(world, ent_5, RelA);
    ecs_entity_t ent_6 = ecs_new_entity(world, "ent_6");
    ecs_add(world, ent_6, RelA);
    ecs_add(world, ent_6, Tag);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "RelA($x), $x ~= \"nt_\""
    });

    test_assert(r != NULL);

    int x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(true, ecs_field_is_set(&it, 1));
        test_uint(false, ecs_field_is_set(&it, 2));
        test_uint(ent_1, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(true, ecs_field_is_set(&it, 1));
        test_uint(false, ecs_field_is_set(&it, 2));
        test_uint(ent_3, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(true, ecs_field_is_set(&it, 1));
        test_uint(false, ecs_field_is_set(&it, 2));
        test_uint(ent_4, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(true, ecs_field_is_set(&it, 1));
        test_uint(false, ecs_field_is_set(&it, 2));
        test_uint(ent_6, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesBuiltinPredicates_this_match_neq() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    ecs_entity_t ent_1 = ecs_new_entity(world, "ent_1");
    ecs_entity_t ent_2 = ecs_new_entity(world, "ent2");
    ecs_entity_t ent_3 = ecs_new_entity(world, "nt_3");
    ecs_entity_t ent_4 = ecs_new_entity(world, "ent_4");
    ecs_entity_t ent_5 = ecs_new_entity(world, "ent5");
    ecs_add(world, ent_5, Tag);
    ecs_entity_t ent_6 = ecs_new_entity(world, "ent_6");
    ecs_add(world, ent_6, Tag);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "$this ~= \"!nt_\""
    });

    test_assert(r != NULL);

    bool ent_2_found = false;
    bool ent_5_found = false;

    ecs_iter_t it = ecs_rule_iter(world, r);
    while (ecs_rule_next(&it)) {
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

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesBuiltinPredicates_var_match_neq() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    ecs_entity_t ent_1 = ecs_new_entity(world, "ent_1");
    ecs_entity_t ent_2 = ecs_new_entity(world, "ent2");
    ecs_entity_t ent_3 = ecs_new_entity(world, "nt_3");
    ecs_entity_t ent_4 = ecs_new_entity(world, "ent_4");
    ecs_entity_t ent_5 = ecs_new_entity(world, "ent5");
    ecs_add(world, ent_5, Tag);
    ecs_entity_t ent_6 = ecs_new_entity(world, "ent_6");
    ecs_add(world, ent_6, Tag);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "$x ~= \"!nt_\""
    });

    test_assert(r != NULL);

    int x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != -1);

    bool ent_2_found = false;
    bool ent_5_found = false;

    ecs_iter_t it = ecs_rule_iter(world, r);
    while (ecs_rule_next(&it)) {
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

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesBuiltinPredicates_this_match_neq_written() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);
    ECS_TAG(world, RelA);

    ecs_entity_t ent_1 = ecs_new_entity(world, "ent_1");
    ecs_add(world, ent_1, RelA);
    ecs_entity_t ent_2 = ecs_new_entity(world, "ent2");
    ecs_add(world, ent_2, RelA);
    ecs_entity_t ent_3 = ecs_new_entity(world, "nt_3");
    ecs_add(world, ent_3, RelA);
    ecs_entity_t ent_4 = ecs_new_entity(world, "ent_4");
    ecs_add(world, ent_4, RelA);
    ecs_entity_t ent_5 = ecs_new_entity(world, "ent5");
    ecs_add(world, ent_5, RelA);
    ecs_entity_t ent_6 = ecs_new_entity(world, "ent_6");
    ecs_add(world, ent_6, RelA);
    ecs_add(world, ent_6, Tag);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "RelA($this), $this ~= \"!nt_\""
    });

    test_assert(r != NULL);

    bool ent_2_found = false;
    bool ent_5_found = false;

    ecs_iter_t it = ecs_rule_iter(world, r);
    while (ecs_rule_next(&it)) {
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

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesBuiltinPredicates_var_match_neq_written() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);
    ECS_TAG(world, RelA);

    ecs_entity_t ent_1 = ecs_new_entity(world, "ent_1");
    ecs_add(world, ent_1, RelA);
    ecs_entity_t ent_2 = ecs_new_entity(world, "ent2");
    ecs_add(world, ent_2, RelA);
    ecs_entity_t ent_3 = ecs_new_entity(world, "nt_3");
    ecs_add(world, ent_3, RelA);
    ecs_entity_t ent_4 = ecs_new_entity(world, "ent_4");
    ecs_add(world, ent_4, RelA);
    ecs_entity_t ent_5 = ecs_new_entity(world, "ent5");
    ecs_add(world, ent_5, RelA);
    ecs_entity_t ent_6 = ecs_new_entity(world, "ent_6");
    ecs_add(world, ent_6, RelA);
    ecs_add(world, ent_6, Tag);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "$x ~= \"!nt_\""
    });

    test_assert(r != NULL);

    int x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != -1);

    bool ent_2_found = false;
    bool ent_5_found = false;

    ecs_iter_t it = ecs_rule_iter(world, r);
    while (ecs_rule_next(&it)) {
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

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesBuiltinPredicates_this_match_2_neq() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    ecs_entity_t ent_1 = ecs_new_entity(world, "ent_1");
    ecs_entity_t ent_2 = ecs_new_entity(world, "ent2");
    ecs_entity_t ent_3 = ecs_new_entity(world, "nt_3");
    ecs_entity_t ent_4 = ecs_new_entity(world, "ent_4");
    ecs_entity_t ent_5 = ecs_new_entity(world, "ent5");
    ecs_add(world, ent_5, Tag);
    ecs_entity_t ent_6 = ecs_new_entity(world, "ent_6");
    ecs_add(world, ent_6, Tag);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "$this ~= \"!nt_\", $this ~= \"!_3\""
    });

    test_assert(r != NULL);

    bool ent_2_found = false;
    bool ent_5_found = false;

    ecs_iter_t it = ecs_rule_iter(world, r);
    while (ecs_rule_next(&it)) {
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

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesBuiltinPredicates_var_match_2_neq() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    ecs_entity_t ent_1 = ecs_new_entity(world, "ent_1");
    ecs_entity_t ent_2 = ecs_new_entity(world, "ent2");
    ecs_entity_t ent_3 = ecs_new_entity(world, "nt_3");
    ecs_entity_t ent_4 = ecs_new_entity(world, "ent_4");
    ecs_entity_t ent_5 = ecs_new_entity(world, "ent5");
    ecs_add(world, ent_5, Tag);
    ecs_entity_t ent_6 = ecs_new_entity(world, "ent_6");
    ecs_add(world, ent_6, Tag);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "$x ~= \"!nt_\", $x ~= \"!_3\""
    });

    test_assert(r != NULL);

    int x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != -1);

    bool ent_2_found = false;
    bool ent_5_found = false;

    ecs_iter_t it = ecs_rule_iter(world, r);
    while (ecs_rule_next(&it)) {
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

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesBuiltinPredicates_this_match_2_neq_written() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);
    ECS_TAG(world, RelA);

    ecs_entity_t ent_1 = ecs_new_entity(world, "ent_1");
    ecs_add(world, ent_1, RelA);
    ecs_entity_t ent_2 = ecs_new_entity(world, "ent2");
    ecs_add(world, ent_2, RelA);
    ecs_entity_t ent_3 = ecs_new_entity(world, "nt_3");
    ecs_add(world, ent_3, RelA);
    ecs_entity_t ent_4 = ecs_new_entity(world, "ent_4");
    ecs_add(world, ent_4, RelA);
    ecs_entity_t ent_5 = ecs_new_entity(world, "ent5");
    ecs_add(world, ent_5, RelA);
    ecs_entity_t ent_6 = ecs_new_entity(world, "ent_6");
    ecs_add(world, ent_6, RelA);
    ecs_add(world, ent_6, Tag);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "RelA($this), $this ~= \"!nt_\", $this ~= \"!_3\""
    });

    test_assert(r != NULL);

    bool ent_2_found = false;
    bool ent_5_found = false;

    ecs_iter_t it = ecs_rule_iter(world, r);
    while (ecs_rule_next(&it)) {
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

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesBuiltinPredicates_var_match_2_neq_written() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);
    ECS_TAG(world, RelA);

    ecs_entity_t ent_1 = ecs_new_entity(world, "ent_1");
    ecs_add(world, ent_1, RelA);
    ecs_entity_t ent_2 = ecs_new_entity(world, "ent2");
    ecs_add(world, ent_2, RelA);
    ecs_entity_t ent_3 = ecs_new_entity(world, "nt_3");
    ecs_add(world, ent_3, RelA);
    ecs_entity_t ent_4 = ecs_new_entity(world, "ent_4");
    ecs_add(world, ent_4, RelA);
    ecs_entity_t ent_5 = ecs_new_entity(world, "ent5");
    ecs_add(world, ent_5, RelA);
    ecs_entity_t ent_6 = ecs_new_entity(world, "ent_6");
    ecs_add(world, ent_6, RelA);
    ecs_add(world, ent_6, Tag);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "RelA($x), $x ~= \"!nt_\", $x ~= \"!_3\""
    });

    test_assert(r != NULL);

    int x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != -1);

    bool ent_2_found = false;
    bool ent_5_found = false;

    ecs_iter_t it = ecs_rule_iter(world, r);
    while (ecs_rule_next(&it)) {
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

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesBuiltinPredicates_this_match_2_or() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    ecs_entity_t ent_1 = ecs_new_entity(world, "ent_1");
    /* ecs_entity_t ent_2 = */ ecs_new_entity(world, "ent2");
    ecs_entity_t ent_3 = ecs_new_entity(world, "nt_3");
    /* ecs_entity_t ent_4 = */ ecs_new_entity(world, "ent_4");
    ecs_entity_t ent_5 = ecs_new_entity(world, "ent5");
    ecs_add(world, ent_5, Tag);
    ecs_entity_t ent_6 = ecs_new_entity(world, "ent_6");
    ecs_add(world, ent_6, Tag);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "$this ~= \"1\" || $this ~= \"_3\""
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(false, ecs_field_is_set(&it, 1));
        test_uint(ent_1, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(false, ecs_field_is_set(&it, 1));
        test_uint(ent_3, it.entities[0]);

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesBuiltinPredicates_this_match_2_or_written() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);
    ECS_TAG(world, RelA);

    ecs_entity_t ent_1 = ecs_new_entity(world, "ent_1");
    ecs_add(world, ent_1, RelA);
    ecs_entity_t ent_2 = ecs_new_entity(world, "ent2");
    ecs_add(world, ent_2, RelA);
    ecs_entity_t ent_3 = ecs_new_entity(world, "nt_3");
    ecs_add(world, ent_3, RelA);
    ecs_entity_t ent_4 = ecs_new_entity(world, "ent_4");
    ecs_add(world, ent_4, RelA);
    ecs_entity_t ent_5 = ecs_new_entity(world, "ent5");
    ecs_add(world, ent_5, RelA);
    ecs_entity_t ent_6 = ecs_new_entity(world, "ent_6");
    ecs_add(world, ent_6, RelA);
    ecs_add(world, ent_6, Tag);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "RelA($this), $this ~= \"1\" || $this ~= \"_3\""
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(true, ecs_field_is_set(&it, 1));
        test_uint(false, ecs_field_is_set(&it, 2));
        test_uint(ent_1, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(true, ecs_field_is_set(&it, 1));
        test_uint(false, ecs_field_is_set(&it, 2));
        test_uint(ent_3, it.entities[0]);

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesBuiltinPredicates_this_match_3_or() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    ecs_entity_t ent_1 = ecs_new_entity(world, "ent_1");
    /* ecs_entity_t ent_2 = */ ecs_new_entity(world, "ent2");
    ecs_entity_t ent_3 = ecs_new_entity(world, "nt_3");
    /* ecs_entity_t ent_4 = */ ecs_new_entity(world, "ent_4");
    ecs_entity_t ent_5 = ecs_new_entity(world, "ent5");
    ecs_add(world, ent_5, Tag);
    ecs_entity_t ent_6 = ecs_new_entity(world, "ent_6");
    ecs_add(world, ent_6, Tag);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "$this ~= \"1\" || $this ~= \"_3\" || $this ~= \"nt_6\""
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(false, ecs_field_is_set(&it, 1));
        test_uint(ent_1, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(false, ecs_field_is_set(&it, 1));
        test_uint(ent_3, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(false, ecs_field_is_set(&it, 1));
        test_uint(ent_6, it.entities[0]);

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesBuiltinPredicates_this_match_3_or_written() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);
    ECS_TAG(world, RelA);

    ecs_entity_t ent_1 = ecs_new_entity(world, "ent_1");
    ecs_add(world, ent_1, RelA);
    ecs_entity_t ent_2 = ecs_new_entity(world, "ent2");
    ecs_add(world, ent_2, RelA);
    ecs_entity_t ent_3 = ecs_new_entity(world, "nt_3");
    ecs_add(world, ent_3, RelA);
    ecs_entity_t ent_4 = ecs_new_entity(world, "ent_4");
    ecs_add(world, ent_4, RelA);
    ecs_entity_t ent_5 = ecs_new_entity(world, "ent5");
    ecs_add(world, ent_5, RelA);
    ecs_entity_t ent_6 = ecs_new_entity(world, "ent_6");
    ecs_add(world, ent_6, RelA);
    ecs_add(world, ent_6, Tag);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "RelA($this), $this ~= \"1\" || $this ~= \"_3\" || $this ~= \"nt_6\""
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(true, ecs_field_is_set(&it, 1));
        test_uint(false, ecs_field_is_set(&it, 2));
        test_uint(ent_1, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(true, ecs_field_is_set(&it, 1));
        test_uint(false, ecs_field_is_set(&it, 2));
        test_uint(ent_3, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(true, ecs_field_is_set(&it, 1));
        test_uint(false, ecs_field_is_set(&it, 2));
        test_uint(ent_6, it.entities[0]);

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesBuiltinPredicates_unresolved_by_name() {
    ecs_world_t *world = ecs_init();

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "$this == ent",
        .flags = EcsFilterUnresolvedByName
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_entity_t ent = ecs_new_entity(world, "ent");

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(false, ecs_field_is_set(&it, 1));
        test_uint(ent, it.entities[0]);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}
