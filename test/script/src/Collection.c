#include <script.h>

void Collection_range_bracketed(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Position" }),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    const char *expr =
    HEAD "for i in [0..3] {"
    LINE "  \"e_{i}\" {"
    LINE "    Position: {i, i * 2}"
    LINE "  }"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t e_0 = ecs_lookup(world, "e_0");
    ecs_entity_t e_1 = ecs_lookup(world, "e_1");
    ecs_entity_t e_2 = ecs_lookup(world, "e_2");
    ecs_entity_t e_3 = ecs_lookup(world, "e_3");

    test_assert(e_0 != 0);
    test_assert(e_1 != 0);
    test_assert(e_2 != 0);
    test_assert(e_3 == 0);

    {
        const Position *p = ecs_get(world, e_0, Position);
        test_assert(p != NULL);
        test_int(p->x, 0);
        test_int(p->y, 0);
    }

    {
        const Position *p = ecs_get(world, e_1, Position);
        test_assert(p != NULL);
        test_int(p->x, 1);
        test_int(p->y, 2);
    }

    {
        const Position *p = ecs_get(world, e_2, Position);
        test_assert(p != NULL);
        test_int(p->x, 2);
        test_int(p->y, 4);
    }

    ecs_fini(world);
}

void Collection_range_bare(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Position" }),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    const char *expr =
    HEAD "for i in 0..3 {"
    LINE "  \"e_{i}\" {"
    LINE "    Position: {i, i * 2}"
    LINE "  }"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t e_0 = ecs_lookup(world, "e_0");
    ecs_entity_t e_1 = ecs_lookup(world, "e_1");
    ecs_entity_t e_2 = ecs_lookup(world, "e_2");
    ecs_entity_t e_3 = ecs_lookup(world, "e_3");

    test_assert(e_0 != 0);
    test_assert(e_1 != 0);
    test_assert(e_2 != 0);
    test_assert(e_3 == 0);

    {
        const Position *p = ecs_get(world, e_2, Position);
        test_assert(p != NULL);
        test_int(p->x, 2);
        test_int(p->y, 4);
    }

    ecs_fini(world);
}

void Collection_range_bracketed_index_value(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Position" }),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    const char *expr =
    HEAD "for (i, v) in [2..5] {"
    LINE "  \"e_{i}\" {"
    LINE "    Position: {v, v * 2}"
    LINE "  }"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t e_0 = ecs_lookup(world, "e_0");
    ecs_entity_t e_1 = ecs_lookup(world, "e_1");
    ecs_entity_t e_2 = ecs_lookup(world, "e_2");
    ecs_entity_t e_3 = ecs_lookup(world, "e_3");

    test_assert(e_0 != 0);
    test_assert(e_1 != 0);
    test_assert(e_2 != 0);
    test_assert(e_3 == 0);

    {
        const Position *p = ecs_get(world, e_0, Position);
        test_assert(p != NULL);
        test_int(p->x, 2);
        test_int(p->y, 4);
    }

    {
        const Position *p = ecs_get(world, e_1, Position);
        test_assert(p != NULL);
        test_int(p->x, 3);
        test_int(p->y, 6);
    }

    {
        const Position *p = ecs_get(world, e_2, Position);
        test_assert(p != NULL);
        test_int(p->x, 4);
        test_int(p->y, 8);
    }

    ecs_fini(world);
}

void Collection_range_bare_index_value(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Position" }),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    const char *expr =
    HEAD "for (i, v) in 2..5 {"
    LINE "  \"e_{i}\" {"
    LINE "    Position: {v, v * 2}"
    LINE "  }"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t e_0 = ecs_lookup(world, "e_0");
    ecs_entity_t e_1 = ecs_lookup(world, "e_1");
    ecs_entity_t e_2 = ecs_lookup(world, "e_2");
    ecs_entity_t e_3 = ecs_lookup(world, "e_3");

    test_assert(e_0 != 0);
    test_assert(e_1 != 0);
    test_assert(e_2 != 0);
    test_assert(e_3 == 0);

    {
        const Position *p = ecs_get(world, e_0, Position);
        test_assert(p != NULL);
        test_int(p->x, 2);
        test_int(p->y, 4);
    }

    ecs_fini(world);
}

void Collection_range_w_expr_endpoints(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "for i in [1 + 1..2 * 3] {"
    LINE "  \"e_{i}\" {}"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    test_assert(ecs_lookup(world, "e_1") == 0);
    test_assert(ecs_lookup(world, "e_2") != 0);
    test_assert(ecs_lookup(world, "e_3") != 0);
    test_assert(ecs_lookup(world, "e_4") != 0);
    test_assert(ecs_lookup(world, "e_5") != 0);
    test_assert(ecs_lookup(world, "e_6") == 0);

    ecs_fini(world);
}

void Collection_range_w_vars(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "const from = 1"
    LINE "const to = 4"
    LINE "for i in [from..to] {"
    LINE "  \"e_{i}\" {}"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    test_assert(ecs_lookup(world, "e_0") == 0);
    test_assert(ecs_lookup(world, "e_1") != 0);
    test_assert(ecs_lookup(world, "e_2") != 0);
    test_assert(ecs_lookup(world, "e_3") != 0);
    test_assert(ecs_lookup(world, "e_4") == 0);

    ecs_fini(world);
}

void Collection_range_empty(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "for i in [3..3] {"
    LINE "  \"e_{i}\" {}"
    LINE "}"
    LINE "after {}";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    test_assert(ecs_lookup(world, "e_3") == 0);
    test_assert(ecs_lookup(world, "after") != 0);

    ecs_fini(world);
}

void Collection_range_reverse_empty(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "for i in [3..1] {"
    LINE "  \"e_{i}\" {}"
    LINE "}"
    LINE "after {}";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    test_assert(ecs_lookup(world, "e_1") == 0);
    test_assert(ecs_lookup(world, "e_2") == 0);
    test_assert(ecs_lookup(world, "e_3") == 0);
    test_assert(ecs_lookup(world, "after") != 0);

    ecs_fini(world);
}

void Collection_range_negative(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(PositionI) = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "PositionI" }),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    const char *expr =
    HEAD "for (i, v) in [-2..1] {"
    LINE "  \"e_{i}\" {"
    LINE "    PositionI: {i, v}"
    LINE "  }"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t e_0 = ecs_lookup(world, "e_0");
    ecs_entity_t e_1 = ecs_lookup(world, "e_1");
    ecs_entity_t e_2 = ecs_lookup(world, "e_2");
    ecs_entity_t e_3 = ecs_lookup(world, "e_3");

    test_assert(e_0 != 0);
    test_assert(e_1 != 0);
    test_assert(e_2 != 0);
    test_assert(e_3 == 0);

    {
        const PositionI *p = ecs_get(world, e_0, PositionI);
        test_assert(p != NULL);
        test_int(p->x, 0);
        test_int(p->y, -2);
    }

    {
        const PositionI *p = ecs_get(world, e_2, PositionI);
        test_assert(p != NULL);
        test_int(p->x, 2);
        test_int(p->y, 0);
    }

    ecs_fini(world);
}

void Collection_range_single(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(PositionI) = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "PositionI" }),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    const char *expr =
    HEAD "for (i, v) in [5..6] {"
    LINE "  \"e_{i}\" {"
    LINE "    PositionI: {i, v}"
    LINE "  }"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t e_0 = ecs_lookup(world, "e_0");
    test_assert(e_0 != 0);
    test_assert(ecs_lookup(world, "e_1") == 0);

    const PositionI *p = ecs_get(world, e_0, PositionI);
    test_assert(p != NULL);
    test_int(p->x, 0);
    test_int(p->y, 5);

    ecs_fini(world);
}

void Collection_range_nested(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "for x in [0..2] {"
    LINE "  for y in [0..2] {"
    LINE "    \"e_{x}_{y}\" {}"
    LINE "  }"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    test_assert(ecs_lookup(world, "e_0_0") != 0);
    test_assert(ecs_lookup(world, "e_0_1") != 0);
    test_assert(ecs_lookup(world, "e_1_0") != 0);
    test_assert(ecs_lookup(world, "e_1_1") != 0);
    test_assert(ecs_lookup(world, "e_2_0") == 0);
    test_assert(ecs_lookup(world, "e_0_2") == 0);

    ecs_fini(world);
}

void Collection_range_too_many_vars(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "for (a, b, c) in [0..3] {"
    LINE "  \"e_{a}\" {}"
    LINE "}";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run(world, NULL, expr, NULL) != 0);

    ecs_fini(world);
}

void Collection_array_elem(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t arr = ecs_array(world, {
        .entity = ecs_entity(world, { .name = "Arr" }),
        .type = ecs_id(ecs_i32_t),
        .count = 3
    });

    test_assert(arr != 0);

    ecs_entity_t ecs_id(Position) = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Position" }),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    const char *expr =
    HEAD "const a: Arr = [10, 20, 30]"
    LINE "for elem in a {"
    LINE "  \"e_{elem}\" {"
    LINE "    Position: {elem, elem * 2}"
    LINE "  }"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t e_10 = ecs_lookup(world, "e_10");
    ecs_entity_t e_20 = ecs_lookup(world, "e_20");
    ecs_entity_t e_30 = ecs_lookup(world, "e_30");

    test_assert(e_10 != 0);
    test_assert(e_20 != 0);
    test_assert(e_30 != 0);

    {
        const Position *p = ecs_get(world, e_10, Position);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 20);
    }

    {
        const Position *p = ecs_get(world, e_20, Position);
        test_assert(p != NULL);
        test_int(p->x, 20);
        test_int(p->y, 40);
    }

    {
        const Position *p = ecs_get(world, e_30, Position);
        test_assert(p != NULL);
        test_int(p->x, 30);
        test_int(p->y, 60);
    }

    ecs_fini(world);
}

void Collection_array_index_elem(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t arr = ecs_array(world, {
        .entity = ecs_entity(world, { .name = "Arr" }),
        .type = ecs_id(ecs_i32_t),
        .count = 3
    });

    test_assert(arr != 0);

    ecs_entity_t ecs_id(Position) = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Position" }),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    const char *expr =
    HEAD "const a: Arr = [10, 20, 30]"
    LINE "for (index, elem) in a {"
    LINE "  \"e_{index}\" {"
    LINE "    Position: {elem, elem * 2}"
    LINE "  }"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t e_0 = ecs_lookup(world, "e_0");
    ecs_entity_t e_1 = ecs_lookup(world, "e_1");
    ecs_entity_t e_2 = ecs_lookup(world, "e_2");

    test_assert(e_0 != 0);
    test_assert(e_1 != 0);
    test_assert(e_2 != 0);
    test_assert(ecs_lookup(world, "e_3") == 0);

    {
        const Position *p = ecs_get(world, e_0, Position);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 20);
    }

    {
        const Position *p = ecs_get(world, e_1, Position);
        test_assert(p != NULL);
        test_int(p->x, 20);
        test_int(p->y, 40);
    }

    {
        const Position *p = ecs_get(world, e_2, Position);
        test_assert(p != NULL);
        test_int(p->x, 30);
        test_int(p->y, 60);
    }

    ecs_fini(world);
}

void Collection_array_struct_elem(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Position" }),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    ecs_entity_t point = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Point" }),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    test_assert(point != 0);

    ecs_entity_t arr = ecs_array(world, {
        .entity = ecs_entity(world, { .name = "Points" }),
        .type = point,
        .count = 2
    });

    test_assert(arr != 0);

    const char *expr =
    HEAD "const points: Points = [{10, 20}, {30, 40}]"
    LINE "for (i, p) in points {"
    LINE "  \"e_{i}\" {"
    LINE "    Position: {p.x, p.y}"
    LINE "  }"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t e_0 = ecs_lookup(world, "e_0");
    ecs_entity_t e_1 = ecs_lookup(world, "e_1");

    test_assert(e_0 != 0);
    test_assert(e_1 != 0);

    {
        const Position *p = ecs_get(world, e_0, Position);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 20);
    }

    {
        const Position *p = ecs_get(world, e_1, Position);
        test_assert(p != NULL);
        test_int(p->x, 30);
        test_int(p->y, 40);
    }

    ecs_fini(world);
}

void Collection_array_string_elem(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t arr = ecs_array(world, {
        .entity = ecs_entity(world, { .name = "Strings" }),
        .type = ecs_id(ecs_string_t),
        .count = 2
    });

    test_assert(arr != 0);

    const char *expr =
    HEAD "const names: Strings = [\"Foo\", \"Bar\"]"
    LINE "for name in names {"
    LINE "  \"e_{name}\" {}"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    test_assert(ecs_lookup(world, "e_Foo") != 0);
    test_assert(ecs_lookup(world, "e_Bar") != 0);

    ecs_fini(world);
}

void Collection_array_entity_elem(void) {
    ecs_world_t *world = ecs_init();

    typedef struct {
        ecs_entity_t value;
    } EntityVal;

    ecs_entity_t ecs_id(EntityVal) = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "EntityVal" }),
        .members = {
            {"value", ecs_id(ecs_entity_t)}
        }
    });

    ecs_entity_t arr = ecs_array(world, {
        .entity = ecs_entity(world, { .name = "Entities" }),
        .type = ecs_id(ecs_entity_t),
        .count = 2
    });

    test_assert(arr != 0);

    const char *expr =
    HEAD "k1 {}"
    LINE "k2 {}"
    LINE "const targets: Entities = [k1, k2]"
    LINE "for (i, e) in targets {"
    LINE "  \"e_{i}\" {"
    LINE "    EntityVal: {e}"
    LINE "  }"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t k1 = ecs_lookup(world, "k1");
    ecs_entity_t k2 = ecs_lookup(world, "k2");
    ecs_entity_t e_0 = ecs_lookup(world, "e_0");
    ecs_entity_t e_1 = ecs_lookup(world, "e_1");

    test_assert(k1 != 0);
    test_assert(k2 != 0);
    test_assert(e_0 != 0);
    test_assert(e_1 != 0);

    {
        const EntityVal *v = ecs_get(world, e_0, EntityVal);
        test_assert(v != NULL);
        test_uint(v->value, k1);
    }

    {
        const EntityVal *v = ecs_get(world, e_1, EntityVal);
        test_assert(v != NULL);
        test_uint(v->value, k2);
    }

    ecs_fini(world);
}

void Collection_array_member_of_struct(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t arr = ecs_array(world, {
        .entity = ecs_entity(world, { .name = "Arr" }),
        .type = ecs_id(ecs_i32_t),
        .count = 3
    });

    test_assert(arr != 0);

    ecs_entity_t s = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "S" }),
        .members = {
            {"arr", arr}
        }
    });

    test_assert(s != 0);

    const char *expr =
    HEAD "const s: S = {arr: [10, 20, 30]}"
    LINE "for elem in s.arr {"
    LINE "  \"e_{elem}\" {}"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    test_assert(ecs_lookup(world, "e_10") != 0);
    test_assert(ecs_lookup(world, "e_20") != 0);
    test_assert(ecs_lookup(world, "e_30") != 0);

    ecs_fini(world);
}

void Collection_array_const_var_from_c(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t arr = ecs_array(world, {
        .entity = ecs_entity(world, { .name = "Arr" }),
        .type = ecs_id(ecs_i32_t),
        .count = 3
    });

    test_assert(arr != 0);

    int32_t values[3] = {10, 20, 30};

    test_assert(0 != ecs_const_var(world, {
        .name = "a",
        .type = arr,
        .value = values
    }));

    const char *expr =
    HEAD "for elem in a {"
    LINE "  \"e_{elem}\" {}"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    test_assert(ecs_lookup(world, "e_10") != 0);
    test_assert(ecs_lookup(world, "e_20") != 0);
    test_assert(ecs_lookup(world, "e_30") != 0);

    ecs_fini(world);
}

void Collection_array_nested(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t arr = ecs_array(world, {
        .entity = ecs_entity(world, { .name = "Arr" }),
        .type = ecs_id(ecs_i32_t),
        .count = 2
    });

    test_assert(arr != 0);

    const char *expr =
    HEAD "const outer: Arr = [1, 2]"
    LINE "const inner: Arr = [3, 4]"
    LINE "for a in outer {"
    LINE "  for b in inner {"
    LINE "    \"e_{a}_{b}\" {}"
    LINE "  }"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    test_assert(ecs_lookup(world, "e_1_3") != 0);
    test_assert(ecs_lookup(world, "e_1_4") != 0);
    test_assert(ecs_lookup(world, "e_2_3") != 0);
    test_assert(ecs_lookup(world, "e_2_4") != 0);

    ecs_fini(world);
}

void Collection_array_in_entity_scope(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t arr = ecs_array(world, {
        .entity = ecs_entity(world, { .name = "Arr" }),
        .type = ecs_id(ecs_i32_t),
        .count = 2
    });

    test_assert(arr != 0);

    ecs_entity_t ecs_id(Position) = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Position" }),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    const char *expr =
    HEAD "const a: Arr = [10, 20]"
    LINE "parent {"
    LINE "  for (i, elem) in a {"
    LINE "    \"child_{i}\" {"
    LINE "      Position: {elem, elem}"
    LINE "    }"
    LINE "  }"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t parent = ecs_lookup(world, "parent");
    test_assert(parent != 0);

    ecs_entity_t child_0 = ecs_lookup(world, "parent.child_0");
    ecs_entity_t child_1 = ecs_lookup(world, "parent.child_1");

    test_assert(child_0 != 0);
    test_assert(child_1 != 0);
    test_assert(ecs_lookup(world, "child_0") == 0);

    {
        const Position *p = ecs_get(world, child_0, Position);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 10);
    }

    {
        const Position *p = ecs_get(world, child_1, Position);
        test_assert(p != NULL);
        test_int(p->x, 20);
        test_int(p->y, 20);
    }

    ecs_fini(world);
}

void Collection_array_elem_in_if(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t arr = ecs_array(world, {
        .entity = ecs_entity(world, { .name = "Arr" }),
        .type = ecs_id(ecs_i32_t),
        .count = 3
    });

    test_assert(arr != 0);

    const char *expr =
    HEAD "const a: Arr = [10, 20, 30]"
    LINE "for elem in a {"
    LINE "  if elem > 10 {"
    LINE "    \"e_{elem}\" {}"
    LINE "  }"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    test_assert(ecs_lookup(world, "e_10") == 0);
    test_assert(ecs_lookup(world, "e_20") != 0);
    test_assert(ecs_lookup(world, "e_30") != 0);

    ecs_fini(world);
}

void Collection_array_anonymous_entities(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t arr = ecs_array(world, {
        .entity = ecs_entity(world, { .name = "Arr" }),
        .type = ecs_id(ecs_i32_t),
        .count = 3
    });

    test_assert(arr != 0);

    ecs_entity_t ecs_id(Position) = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Position" }),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    const char *expr =
    HEAD "const a: Arr = [10, 20, 30]"
    LINE "for elem in a {"
    LINE "  {"
    LINE "    Position: {elem, elem * 2}"
    LINE "  }"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_query_t *q = ecs_query(world, { .expr = "Position" });
    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    int32_t count = 0;
    float sum_x = 0, sum_y = 0;
    while (ecs_query_next(&it)) {
        Position *p = ecs_field(&it, Position, 0);
        int32_t i;
        for (i = 0; i < it.count; i ++) {
            count ++;
            sum_x += p[i].x;
            sum_y += p[i].y;
        }
    }

    test_int(count, 3);
    test_int((int32_t)sum_x, 60);
    test_int((int32_t)sum_y, 120);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Collection_array_too_many_vars(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t arr = ecs_array(world, {
        .entity = ecs_entity(world, { .name = "Arr" }),
        .type = ecs_id(ecs_i32_t),
        .count = 3
    });

    test_assert(arr != 0);

    const char *expr =
    HEAD "const a: Arr = [10, 20, 30]"
    LINE "for (x, y, z) in a {"
    LINE "  \"e_{x}\" {}"
    LINE "}";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run(world, NULL, expr, NULL) != 0);

    ecs_fini(world);
}

void Collection_vector_elem(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t vec = ecs_vector(world, {
        .entity = ecs_entity(world, { .name = "Vec" }),
        .type = ecs_id(ecs_i32_t)
    });

    test_assert(vec != 0);

    ecs_entity_t ecs_id(Position) = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Position" }),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    const char *expr =
    HEAD "const v: Vec = [10, 20, 30]"
    LINE "for elem in v {"
    LINE "  \"e_{elem}\" {"
    LINE "    Position: {elem, elem * 2}"
    LINE "  }"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t e_10 = ecs_lookup(world, "e_10");
    ecs_entity_t e_20 = ecs_lookup(world, "e_20");
    ecs_entity_t e_30 = ecs_lookup(world, "e_30");

    test_assert(e_10 != 0);
    test_assert(e_20 != 0);
    test_assert(e_30 != 0);

    {
        const Position *p = ecs_get(world, e_10, Position);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 20);
    }

    {
        const Position *p = ecs_get(world, e_30, Position);
        test_assert(p != NULL);
        test_int(p->x, 30);
        test_int(p->y, 60);
    }

    ecs_fini(world);
}

void Collection_vector_index_elem(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t vec = ecs_vector(world, {
        .entity = ecs_entity(world, { .name = "Vec" }),
        .type = ecs_id(ecs_i32_t)
    });

    test_assert(vec != 0);

    ecs_entity_t ecs_id(Position) = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Position" }),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    const char *expr =
    HEAD "const v: Vec = [10, 20, 30]"
    LINE "for (index, elem) in v {"
    LINE "  \"e_{index}\" {"
    LINE "    Position: {elem, elem * 2}"
    LINE "  }"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t e_0 = ecs_lookup(world, "e_0");
    ecs_entity_t e_1 = ecs_lookup(world, "e_1");
    ecs_entity_t e_2 = ecs_lookup(world, "e_2");

    test_assert(e_0 != 0);
    test_assert(e_1 != 0);
    test_assert(e_2 != 0);

    {
        const Position *p = ecs_get(world, e_0, Position);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 20);
    }

    {
        const Position *p = ecs_get(world, e_2, Position);
        test_assert(p != NULL);
        test_int(p->x, 30);
        test_int(p->y, 60);
    }

    ecs_fini(world);
}

void Collection_vector_struct_elem(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Position" }),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    ecs_entity_t point = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Point" }),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    test_assert(point != 0);

    ecs_entity_t vec = ecs_vector(world, {
        .entity = ecs_entity(world, { .name = "Points" }),
        .type = point
    });

    test_assert(vec != 0);

    const char *expr =
    HEAD "const points: Points = [{10, 20}, {30, 40}]"
    LINE "for (i, p) in points {"
    LINE "  \"e_{i}\" {"
    LINE "    Position: {p.x, p.y}"
    LINE "  }"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t e_0 = ecs_lookup(world, "e_0");
    ecs_entity_t e_1 = ecs_lookup(world, "e_1");

    test_assert(e_0 != 0);
    test_assert(e_1 != 0);

    {
        const Position *p = ecs_get(world, e_0, Position);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 20);
    }

    {
        const Position *p = ecs_get(world, e_1, Position);
        test_assert(p != NULL);
        test_int(p->x, 30);
        test_int(p->y, 40);
    }

    ecs_fini(world);
}

void Collection_vector_string_elem(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t vec = ecs_vector(world, {
        .entity = ecs_entity(world, { .name = "Strings" }),
        .type = ecs_id(ecs_string_t)
    });

    test_assert(vec != 0);

    const char *expr =
    HEAD "const names: Strings = [\"Hello\", \"World\"]"
    LINE "for name in names {"
    LINE "  \"e_{name}\" {}"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    test_assert(ecs_lookup(world, "e_Hello") != 0);
    test_assert(ecs_lookup(world, "e_World") != 0);

    ecs_fini(world);
}

void Collection_vector_empty(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t vec = ecs_vector(world, {
        .entity = ecs_entity(world, { .name = "Vec" }),
        .type = ecs_id(ecs_i32_t)
    });

    test_assert(vec != 0);

    const char *expr =
    HEAD "const v: Vec = []"
    LINE "for elem in v {"
    LINE "  \"e_{elem}\" {}"
    LINE "}"
    LINE "after {}";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    test_assert(ecs_lookup(world, "after") != 0);

    ecs_fini(world);
}

void Collection_vector_nested(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t vec = ecs_vector(world, {
        .entity = ecs_entity(world, { .name = "Vec" }),
        .type = ecs_id(ecs_i32_t)
    });

    test_assert(vec != 0);

    const char *expr =
    HEAD "const outer: Vec = [1, 2]"
    LINE "const inner: Vec = [3, 4]"
    LINE "for a in outer {"
    LINE "  for b in inner {"
    LINE "    \"e_{a}_{b}\" {}"
    LINE "  }"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    test_assert(ecs_lookup(world, "e_1_3") != 0);
    test_assert(ecs_lookup(world, "e_1_4") != 0);
    test_assert(ecs_lookup(world, "e_2_3") != 0);
    test_assert(ecs_lookup(world, "e_2_4") != 0);

    ecs_fini(world);
}

void Collection_vector_too_many_vars(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t vec = ecs_vector(world, {
        .entity = ecs_entity(world, { .name = "Vec" }),
        .type = ecs_id(ecs_i32_t)
    });

    test_assert(vec != 0);

    const char *expr =
    HEAD "const v: Vec = [10, 20, 30]"
    LINE "for (x, y, z) in v {"
    LINE "  \"e_{x}\" {}"
    LINE "}";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run(world, NULL, expr, NULL) != 0);

    ecs_fini(world);
}

void Collection_map_elem(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t m = ecs_map_type(world, {
        .entity = ecs_entity(world, { .name = "Map" }),
        .key_type = ecs_id(ecs_i32_t),
        .type = ecs_id(ecs_i32_t)
    });

    test_assert(m != 0);

    const char *expr =
    HEAD "const m: Map = [1: 10, 2: 20, 3: 30]"
    LINE "for elem in m {"
    LINE "  \"e_{elem}\" {}"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    test_assert(ecs_lookup(world, "e_10") != 0);
    test_assert(ecs_lookup(world, "e_20") != 0);
    test_assert(ecs_lookup(world, "e_30") != 0);

    ecs_fini(world);
}

void Collection_map_key_elem(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t m = ecs_map_type(world, {
        .entity = ecs_entity(world, { .name = "Map" }),
        .key_type = ecs_id(ecs_i32_t),
        .type = ecs_id(ecs_i32_t)
    });

    test_assert(m != 0);

    ecs_entity_t ecs_id(Position) = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Position" }),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    const char *expr =
    HEAD "const m: Map = [1: 10, 2: 20, 3: 30]"
    LINE "for (key, elem) in m {"
    LINE "  \"e_{key}\" {"
    LINE "    Position: {elem, elem * 2}"
    LINE "  }"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t e_1 = ecs_lookup(world, "e_1");
    ecs_entity_t e_2 = ecs_lookup(world, "e_2");
    ecs_entity_t e_3 = ecs_lookup(world, "e_3");

    test_assert(e_1 != 0);
    test_assert(e_2 != 0);
    test_assert(e_3 != 0);

    {
        const Position *p = ecs_get(world, e_1, Position);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 20);
    }

    {
        const Position *p = ecs_get(world, e_2, Position);
        test_assert(p != NULL);
        test_int(p->x, 20);
        test_int(p->y, 40);
    }

    {
        const Position *p = ecs_get(world, e_3, Position);
        test_assert(p != NULL);
        test_int(p->x, 30);
        test_int(p->y, 60);
    }

    ecs_fini(world);
}

void Collection_map_key_index_elem(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t m = ecs_map_type(world, {
        .entity = ecs_entity(world, { .name = "Map" }),
        .key_type = ecs_id(ecs_i32_t),
        .type = ecs_id(ecs_i32_t)
    });

    test_assert(m != 0);

    ecs_entity_t ecs_id(PositionI) = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "PositionI" }),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    const char *expr =
    HEAD "const m: Map = [1: 10, 2: 20, 3: 30]"
    LINE "for (key, index, elem) in m {"
    LINE "  \"e_{key}\" {"
    LINE "    PositionI: {index, elem * 2}"
    LINE "  }"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t e_1 = ecs_lookup(world, "e_1");
    ecs_entity_t e_2 = ecs_lookup(world, "e_2");
    ecs_entity_t e_3 = ecs_lookup(world, "e_3");

    test_assert(e_1 != 0);
    test_assert(e_2 != 0);
    test_assert(e_3 != 0);

    const PositionI *p1 = ecs_get(world, e_1, PositionI);
    const PositionI *p2 = ecs_get(world, e_2, PositionI);
    const PositionI *p3 = ecs_get(world, e_3, PositionI);

    test_assert(p1 != NULL);
    test_assert(p2 != NULL);
    test_assert(p3 != NULL);

    test_int(p1->y, 20);
    test_int(p2->y, 40);
    test_int(p3->y, 60);

    test_assert(p1->x >= 0 && p1->x < 3);
    test_assert(p2->x >= 0 && p2->x < 3);
    test_assert(p3->x >= 0 && p3->x < 3);
    test_assert(p1->x != p2->x);
    test_assert(p1->x != p3->x);
    test_assert(p2->x != p3->x);
    test_int(p1->x + p2->x + p3->x, 3);

    ecs_fini(world);
}

void Collection_map_single_entry(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t m = ecs_map_type(world, {
        .entity = ecs_entity(world, { .name = "Map" }),
        .key_type = ecs_id(ecs_i32_t),
        .type = ecs_id(ecs_i32_t)
    });

    test_assert(m != 0);

    ecs_entity_t ecs_id(PositionI) = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "PositionI" }),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    const char *expr =
    HEAD "const m: Map = [7: 42]"
    LINE "for (key, index, elem) in m {"
    LINE "  \"e_{key}\" {"
    LINE "    PositionI: {index, elem}"
    LINE "  }"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t e_7 = ecs_lookup(world, "e_7");
    test_assert(e_7 != 0);

    const PositionI *p = ecs_get(world, e_7, PositionI);
    test_assert(p != NULL);
    test_int(p->x, 0);
    test_int(p->y, 42);

    ecs_fini(world);
}

void Collection_map_entity_key(void) {
    ecs_world_t *world = ecs_init();

    typedef struct {
        ecs_entity_t value;
    } EntityVal;

    ecs_entity_t ecs_id(EntityVal) = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "EntityVal" }),
        .members = {
            {"value", ecs_id(ecs_entity_t)}
        }
    });

    ecs_entity_t m = ecs_map_type(world, {
        .entity = ecs_entity(world, { .name = "Map" }),
        .key_type = ecs_id(ecs_entity_t),
        .type = ecs_id(ecs_i32_t)
    });

    test_assert(m != 0);

    const char *expr =
    HEAD "k1 {}"
    LINE "k2 {}"
    LINE "const m: Map = [k1: 10, k2: 20]"
    LINE "for (key, elem) in m {"
    LINE "  \"e_{elem}\" {"
    LINE "    EntityVal: {key}"
    LINE "  }"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t k1 = ecs_lookup(world, "k1");
    ecs_entity_t k2 = ecs_lookup(world, "k2");
    ecs_entity_t e_10 = ecs_lookup(world, "e_10");
    ecs_entity_t e_20 = ecs_lookup(world, "e_20");

    test_assert(k1 != 0);
    test_assert(k2 != 0);
    test_assert(e_10 != 0);
    test_assert(e_20 != 0);

    {
        const EntityVal *v = ecs_get(world, e_10, EntityVal);
        test_assert(v != NULL);
        test_uint(v->value, k1);
    }

    {
        const EntityVal *v = ecs_get(world, e_20, EntityVal);
        test_assert(v != NULL);
        test_uint(v->value, k2);
    }

    ecs_fini(world);
}

void Collection_map_u8_key(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t m = ecs_map_type(world, {
        .entity = ecs_entity(world, { .name = "Map" }),
        .key_type = ecs_id(ecs_u8_t),
        .type = ecs_id(ecs_i32_t)
    });

    test_assert(m != 0);

    ecs_entity_t ecs_id(PositionI) = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "PositionI" }),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    const char *expr =
    HEAD "const m: Map = [1: 10, 2: 20]"
    LINE "for (key, elem) in m {"
    LINE "  \"e_{key}\" {"
    LINE "    PositionI: {elem, elem * 2}"
    LINE "  }"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t e_1 = ecs_lookup(world, "e_1");
    ecs_entity_t e_2 = ecs_lookup(world, "e_2");

    test_assert(e_1 != 0);
    test_assert(e_2 != 0);

    {
        const PositionI *p = ecs_get(world, e_1, PositionI);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 20);
    }

    {
        const PositionI *p = ecs_get(world, e_2, PositionI);
        test_assert(p != NULL);
        test_int(p->x, 20);
        test_int(p->y, 40);
    }

    ecs_fini(world);
}

void Collection_map_struct_value(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Vec3) = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Vec3" }),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)},
            {"z", ecs_id(ecs_f32_t)}
        }
    });

    test_assert(ecs_id(Vec3) != 0);

    ecs_entity_t m = ecs_map_type(world, {
        .entity = ecs_entity(world, { .name = "Map" }),
        .key_type = ecs_id(ecs_i32_t),
        .type = ecs_id(Vec3)
    });

    test_assert(m != 0);

    ecs_entity_t ecs_id(Position) = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Position" }),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    const char *expr =
    HEAD "const m: Map = [1: {1, 2, 3}, 2: {4, 5, 6}]"
    LINE "for (key, elem) in m {"
    LINE "  \"e_{key}\" {"
    LINE "    Position: {elem.x + elem.y, elem.z * 2}"
    LINE "  }"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t e_1 = ecs_lookup(world, "e_1");
    ecs_entity_t e_2 = ecs_lookup(world, "e_2");

    test_assert(e_1 != 0);
    test_assert(e_2 != 0);

    {
        const Position *p = ecs_get(world, e_1, Position);
        test_assert(p != NULL);
        test_int(p->x, 3);
        test_int(p->y, 6);
    }

    {
        const Position *p = ecs_get(world, e_2, Position);
        test_assert(p != NULL);
        test_int(p->x, 9);
        test_int(p->y, 12);
    }

    ecs_fini(world);
}

void Collection_map_string_value(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t m = ecs_map_type(world, {
        .entity = ecs_entity(world, { .name = "Map" }),
        .key_type = ecs_id(ecs_i32_t),
        .type = ecs_id(ecs_string_t)
    });

    test_assert(m != 0);

    const char *expr =
    HEAD "const m: Map = [1: \"Hello\", 2: \"World\"]"
    LINE "for (key, elem) in m {"
    LINE "  \"e_{elem}\" {}"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    test_assert(ecs_lookup(world, "e_Hello") != 0);
    test_assert(ecs_lookup(world, "e_World") != 0);

    ecs_fini(world);
}

void Collection_map_empty(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t m = ecs_map_type(world, {
        .entity = ecs_entity(world, { .name = "Map" }),
        .key_type = ecs_id(ecs_i32_t),
        .type = ecs_id(ecs_i32_t)
    });

    test_assert(m != 0);

    const char *expr =
    HEAD "const m: Map = []"
    LINE "for (key, elem) in m {"
    LINE "  \"e_{key}\" {}"
    LINE "}"
    LINE "after {}";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    test_assert(ecs_lookup(world, "after") != 0);

    ecs_fini(world);
}

void Collection_map_too_many_vars(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t m = ecs_map_type(world, {
        .entity = ecs_entity(world, { .name = "Map" }),
        .key_type = ecs_id(ecs_i32_t),
        .type = ecs_id(ecs_i32_t)
    });

    test_assert(m != 0);

    const char *expr =
    HEAD "const m: Map = [1: 10]"
    LINE "for (a, b, c, d) in m {"
    LINE "  \"e_{a}\" {}"
    LINE "}";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run(world, NULL, expr, NULL) != 0);

    ecs_fini(world);
}

void Collection_nested_struct_arrays(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t arr = ecs_array(world, {
        .entity = ecs_entity(world, { .name = "Arr" }),
        .type = ecs_id(ecs_i32_t),
        .count = 3
    });

    test_assert(arr != 0);

    ecs_entity_t s = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "S" }),
        .members = {
            {"arr", arr}
        }
    });

    test_assert(s != 0);

    ecs_entity_t sarr = ecs_array(world, {
        .entity = ecs_entity(world, { .name = "SArr" }),
        .type = s,
        .count = 2
    });

    test_assert(sarr != 0);

    const char *expr =
    HEAD "const items: SArr = [{arr: [1, 2, 3]}, {arr: [4, 5, 6]}]"
    LINE "for item in items {"
    LINE "  for elem in item.arr {"
    LINE "    \"e_{elem}\" {}"
    LINE "  }"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    test_assert(ecs_lookup(world, "e_1") != 0);
    test_assert(ecs_lookup(world, "e_2") != 0);
    test_assert(ecs_lookup(world, "e_3") != 0);
    test_assert(ecs_lookup(world, "e_4") != 0);
    test_assert(ecs_lookup(world, "e_5") != 0);
    test_assert(ecs_lookup(world, "e_6") != 0);

    ecs_fini(world);
}

void Collection_template_w_for_array(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t arr = ecs_array(world, {
        .entity = ecs_entity(world, { .name = "Arr" }),
        .type = ecs_id(ecs_i32_t),
        .count = 3
    });

    test_assert(arr != 0);

    ecs_entity_t ecs_id(Position) = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Position" }),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    const char *expr =
    HEAD "template Foo {"
    LINE "  prop points: Arr = [1, 2, 3]"
    LINE "  for (i, p) in points {"
    LINE "    \"child_{i}\" {"
    LINE "      Position: {p, p * 2}"
    LINE "    }"
    LINE "  }"
    LINE "}"
    LINE "Foo e()"
    LINE "Foo f(points: [4, 5, 6])";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t f = ecs_lookup(world, "f");
    test_assert(e != 0);
    test_assert(f != 0);

    ecs_entity_t e_child_0 = ecs_lookup(world, "e.child_0");
    ecs_entity_t e_child_2 = ecs_lookup(world, "e.child_2");
    ecs_entity_t f_child_0 = ecs_lookup(world, "f.child_0");
    ecs_entity_t f_child_2 = ecs_lookup(world, "f.child_2");

    test_assert(e_child_0 != 0);
    test_assert(e_child_2 != 0);
    test_assert(f_child_0 != 0);
    test_assert(f_child_2 != 0);

    {
        const Position *p = ecs_get(world, e_child_0, Position);
        test_assert(p != NULL);
        test_int(p->x, 1);
        test_int(p->y, 2);
    }

    {
        const Position *p = ecs_get(world, e_child_2, Position);
        test_assert(p != NULL);
        test_int(p->x, 3);
        test_int(p->y, 6);
    }

    {
        const Position *p = ecs_get(world, f_child_0, Position);
        test_assert(p != NULL);
        test_int(p->x, 4);
        test_int(p->y, 8);
    }

    {
        const Position *p = ecs_get(world, f_child_2, Position);
        test_assert(p != NULL);
        test_int(p->x, 6);
        test_int(p->y, 12);
    }

    ecs_fini(world);
}

void Collection_template_w_for_vector(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t vec = ecs_vector(world, {
        .entity = ecs_entity(world, { .name = "Vec" }),
        .type = ecs_id(ecs_i32_t)
    });

    test_assert(vec != 0);

    const char *expr =
    HEAD "template Foo {"
    LINE "  prop points: Vec = [10, 20]"
    LINE "  for (i, p) in points {"
    LINE "    \"child_{p}\" {}"
    LINE "  }"
    LINE "}"
    LINE "Foo e()"
    LINE "Foo f(points: [30, 40, 50])";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    test_assert(ecs_lookup(world, "e.child_10") != 0);
    test_assert(ecs_lookup(world, "e.child_20") != 0);
    test_assert(ecs_lookup(world, "e.child_30") == 0);
    test_assert(ecs_lookup(world, "f.child_30") != 0);
    test_assert(ecs_lookup(world, "f.child_40") != 0);
    test_assert(ecs_lookup(world, "f.child_50") != 0);

    ecs_fini(world);
}

void Collection_template_w_for_map(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t m = ecs_map_type(world, {
        .entity = ecs_entity(world, { .name = "Map" }),
        .key_type = ecs_id(ecs_i32_t),
        .type = ecs_id(ecs_i32_t)
    });

    test_assert(m != 0);

    ecs_entity_t ecs_id(PositionI) = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "PositionI" }),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    const char *expr =
    HEAD "template Foo {"
    LINE "  prop values: Map = [1: 10, 2: 20]"
    LINE "  for (key, elem) in values {"
    LINE "    \"child_{key}\" {"
    LINE "      PositionI: {key, elem}"
    LINE "    }"
    LINE "  }"
    LINE "}"
    LINE "Foo e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t child_1 = ecs_lookup(world, "e.child_1");
    ecs_entity_t child_2 = ecs_lookup(world, "e.child_2");

    test_assert(child_1 != 0);
    test_assert(child_2 != 0);

    {
        const PositionI *p = ecs_get(world, child_1, PositionI);
        test_assert(p != NULL);
        test_int(p->x, 1);
        test_int(p->y, 10);
    }

    {
        const PositionI *p = ecs_get(world, child_2, PositionI);
        test_assert(p != NULL);
        test_int(p->x, 2);
        test_int(p->y, 20);
    }

    ecs_fini(world);
}

void Collection_template_w_for_range_bracketed(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "template Foo {"
    LINE "  for i in [0..2] {"
    LINE "    \"child_{i}\" {}"
    LINE "  }"
    LINE "}"
    LINE "Foo e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    test_assert(ecs_lookup(world, "e.child_0") != 0);
    test_assert(ecs_lookup(world, "e.child_1") != 0);
    test_assert(ecs_lookup(world, "e.child_2") == 0);

    ecs_fini(world);
}

void Collection_iterate_int_literal(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "for e in 10 {"
    LINE "  \"e_{e}\" {}"
    LINE "}";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run(world, NULL, expr, NULL) != 0);

    ecs_fini(world);
}

void Collection_iterate_int_var(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "const x = 10"
    LINE "for e in x {"
    LINE "  \"e_{e}\" {}"
    LINE "}";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run(world, NULL, expr, NULL) != 0);

    ecs_fini(world);
}

void Collection_iterate_entity(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "Foo {}"
    LINE "for e in Foo {"
    LINE "  \"e_{e}\" {}"
    LINE "}";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run(world, NULL, expr, NULL) != 0);

    ecs_fini(world);
}

void Collection_iterate_undefined_var(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "for e in nosuchvar {"
    LINE "  \"e_{e}\" {}"
    LINE "}";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run(world, NULL, expr, NULL) != 0);

    ecs_fini(world);
}

void Collection_duplicate_loop_vars(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "for (a, a) in [0..3] {"
    LINE "  \"e_{a}\" {}"
    LINE "}";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run(world, NULL, expr, NULL) != 0);

    ecs_fini(world);
}

void Collection_duplicate_loop_vars_map(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t m = ecs_map_type(world, {
        .entity = ecs_entity(world, { .name = "Map" }),
        .key_type = ecs_id(ecs_i32_t),
        .type = ecs_id(ecs_i32_t)
    });

    test_assert(m != 0);

    const char *expr =
    HEAD "const m: Map = [1: 10]"
    LINE "for (a, b, a) in m {"
    LINE "  \"e_{a}\" {}"
    LINE "}";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run(world, NULL, expr, NULL) != 0);

    ecs_fini(world);
}

void Collection_collection_literal_not_supported(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "for e in [1, 2, 3] {"
    LINE "  \"e_{e}\" {}"
    LINE "}";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run(world, NULL, expr, NULL) != 0);

    ecs_fini(world);
}

void Collection_missing_in_keyword(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "for e [0..3] {"
    LINE "  \"e_{e}\" {}"
    LINE "}";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run(world, NULL, expr, NULL) != 0);

    ecs_fini(world);
}
