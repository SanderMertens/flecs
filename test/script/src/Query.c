#include <script.h>

static bool ir_enabled = false;
static ecs_script_eval_desc_t ir_desc = {0};

void Query_setup(void) {
    const char *ir_param = test_param("ir");
    ir_enabled = ir_param && !strcmp(ir_param, "enabled");
    ir_desc = (ecs_script_eval_desc_t){ .ir = ir_enabled };
}

void Query_one_term(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t tag = ecs_entity(world, { .name = "Tag" });
    ecs_entity_t a = ecs_entity(world, { .name = "a" });
    ecs_entity_t b = ecs_entity(world, { .name = "b" });
    ecs_add_id(world, a, tag);
    ecs_add_id(world, b, tag);

    const char *expr =
    HEAD "Marker {}"
    LINE "query q { Tag }"
    LINE "for e in q {"
    LINE "  \"m_{e.name()}\" { Marker }"
    LINE "}";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t marker = ecs_lookup(world, "Marker");
    test_assert(marker != 0);
    ecs_entity_t m_a = ecs_lookup(world, "m_a");
    ecs_entity_t m_b = ecs_lookup(world, "m_b");
    test_assert(m_a != 0);
    test_assert(m_b != 0);
    test_assert(ecs_has_id(world, m_a, marker));
    test_assert(ecs_has_id(world, m_b, marker));

    ecs_fini(world);
}

void Query_multiple_terms(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t tag_a = ecs_entity(world, { .name = "TagA" });
    ecs_entity_t tag_b = ecs_entity(world, { .name = "TagB" });
    ecs_entity_t a = ecs_entity(world, { .name = "a" });
    ecs_entity_t b = ecs_entity(world, { .name = "b" });
    ecs_add_id(world, a, tag_a);
    ecs_add_id(world, a, tag_b);
    ecs_add_id(world, b, tag_a);

    const char *expr =
    HEAD "query q { TagA, TagB }"
    LINE "for e in q {"
    LINE "  \"m_{e.name()}\" {}"
    LINE "}";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    test_assert(ecs_lookup(world, "m_a") != 0);
    test_assert(ecs_lookup(world, "m_b") == 0);

    ecs_fini(world);
}

void Query_multiline(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t tag_a = ecs_entity(world, { .name = "TagA" });
    ecs_entity_t tag_b = ecs_entity(world, { .name = "TagB" });
    ecs_entity_t a = ecs_entity(world, { .name = "a" });
    ecs_entity_t b = ecs_entity(world, { .name = "b" });
    ecs_add_id(world, a, tag_a);
    ecs_add_id(world, a, tag_b);
    ecs_add_id(world, b, tag_a);

    const char *expr =
    HEAD "query q {"
    LINE "  TagA,"
    LINE "  TagB"
    LINE "}"
    LINE ""
    LINE "for e in q {"
    LINE "  \"m_{e.name()}\" {}"
    LINE "}";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    test_assert(ecs_lookup(world, "m_a") != 0);
    test_assert(ecs_lookup(world, "m_b") == 0);

    ecs_fini(world);
}

void Query_pair(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t parent = ecs_entity(world, { .name = "parent" });
    ecs_entity_t other = ecs_entity(world, { .name = "other" });
    ecs_entity(world, { .name = "a", .parent = parent });
    ecs_entity(world, { .name = "b", .parent = parent });
    ecs_entity(world, { .name = "c", .parent = other });

    const char *expr =
    HEAD "query q { (flecs.core.ChildOf, parent) }"
    LINE "for e in q {"
    LINE "  \"m_{e.name()}\" {}"
    LINE "}";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    test_assert(ecs_lookup(world, "m_a") != 0);
    test_assert(ecs_lookup(world, "m_b") != 0);
    test_assert(ecs_lookup(world, "m_c") == 0);

    ecs_fini(world);
}

void Query_not_term(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t tag_a = ecs_entity(world, { .name = "TagA" });
    ecs_entity_t tag_b = ecs_entity(world, { .name = "TagB" });
    ecs_entity_t a = ecs_entity(world, { .name = "a" });
    ecs_entity_t b = ecs_entity(world, { .name = "b" });
    ecs_add_id(world, a, tag_a);
    ecs_add_id(world, a, tag_b);
    ecs_add_id(world, b, tag_a);

    const char *expr =
    HEAD "query q { TagA, !TagB }"
    LINE "for e in q {"
    LINE "  \"m_{e.name()}\" {}"
    LINE "}";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    test_assert(ecs_lookup(world, "m_a") == 0);
    test_assert(ecs_lookup(world, "m_b") != 0);

    ecs_fini(world);
}

void Query_optional_term_present(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t tag = ecs_entity(world, { .name = "Tag" });
    ecs_entity_t position = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Position" }),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    ecs_entity_t a = ecs_entity(world, { .name = "a" });
    ecs_add_id(world, a, tag);
    ecs_set_id(world, a, position, sizeof(Position), &(Position){10, 20});

    const char *expr =
    HEAD "query q { Tag, ?Position }"
    LINE "for (e, p) in q {"
    LINE "  \"m_{e.name()}\" { Position: {p.x + 1, p.y + 1} }"
    LINE "}";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t m_a = ecs_lookup(world, "m_a");
    test_assert(m_a != 0);
    const Position *p = ecs_get_id(world, m_a, position);
    test_assert(p != NULL);
    test_int(p->x, 11);
    test_int(p->y, 21);

    ecs_fini(world);
}

void Query_optional_term_absent(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t tag = ecs_entity(world, { .name = "Tag" });
    ecs_entity_t position = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Position" }),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    ecs_entity_t a = ecs_entity(world, { .name = "a" });
    ecs_add_id(world, a, tag);

    const char *expr =
    HEAD "query q { Tag, ?Position }"
    LINE "for (e, p) in q {"
    LINE "  \"m_{e.name()}\" { Position: {p.x + 1, p.y + 1} }"
    LINE "}";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t m_a = ecs_lookup(world, "m_a");
    test_assert(m_a != 0);
    const Position *p = ecs_get_id(world, m_a, position);
    test_assert(p != NULL);
    test_int(p->x, 1);
    test_int(p->y, 1);

    ecs_fini(world);
}

void Query_malformed_expr(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity(world, { .name = "Tag" });

    const char *expr =
    HEAD "e {}"
    LINE ""
    LINE "query q { Tag, (ChildOf }"
    LINE "for e in q {"
    LINE "}";

    ecs_log_set_level(-4);
    ecs_script_eval_result_t result = {0};
    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, &result) != 0);
    test_assert(result.error != NULL);
    test_int(result.line, 3);
    test_assert(result.column > 10);
    ecs_os_free(result.error);

    ecs_fini(world);
}

void Query_missing_scope_close(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity(world, { .name = "Tag" });

    const char *expr =
    HEAD "query q { Tag";

    ecs_log_set_level(-4);
    ecs_script_eval_result_t result = {0};
    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, &result) != 0);
    test_assert(result.error != NULL);
    ecs_os_free(result.error);

    ecs_fini(world);
}

void Query_empty_expr(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "query q { }";

    ecs_log_set_level(-4);
    ecs_script_eval_result_t result = {0};
    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, &result) != 0);
    test_assert(result.error != NULL);
    ecs_os_free(result.error);

    ecs_fini(world);
}

void Query_unresolved_name(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "query q { DoesNotExist }";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) != 0);

    ecs_fini(world);
}

void Query_wait_for_unresolved_name(void) {
    ecs_world_t *world = ecs_init();

    ecs_log_set_level(-4);

    ecs_entity_t s = ecs_script(world, { .ir = ir_enabled,
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "query q { MissingTag }"
            LINE "for e in q {"
            LINE "  \"m_{e.name()}\" {}"
            LINE "}"
    });
    test_assert(s != 0);

    {
        const EcsScript *sc = ecs_get(world, s, EcsScript);
        test_assert(sc != NULL);
        test_assert(sc->error != NULL);
    }

    ecs_entity_t tag = ecs_entity(world, { .name = "MissingTag" });
    ecs_entity_t a = ecs_entity(world, { .name = "a" });
    ecs_add_id(world, a, tag);

    {
        const EcsScript *sc = ecs_get(world, s, EcsScript);
        test_assert(sc != NULL);
        test_assert(sc->error == NULL);
        test_assert(ecs_lookup(world, "m_a") != 0);
    }

    ecs_fini(world);
}

void Query_update_w_partial_name(void) {
    ecs_world_t *world = ecs_init();

    ecs_log_set_level(-4);

    ecs_entity_t s = ecs_script(world, { .ir = ir_enabled,
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "struct Position(x: f32, y: f32)"
            LINE "struct Velocity(x: f32, y: f32)"
            LINE ""
            LINE "query Move {"
            LINE "   Po"
            LINE "}"
    });
    test_assert(s != 0);

    test_assert(ecs_script_update(world, s, 0,
        HEAD "struct Position(x: f32, y: f32)"
        LINE "struct Velocity(x: f32, y: f32)"
        LINE ""
        LINE "query Move {"
        LINE "   Pos"
        LINE "}") != 0);

    test_assert(ecs_script_update(world, s, 0,
        HEAD "struct Position(x: f32, y: f32)"
        LINE "struct Velocity(x: f32, y: f32)"
        LINE ""
        LINE "query Move {"
        LINE "   Posi"
        LINE "}") != 0);

    test_assert(ecs_script_update(world, s, 0,
        HEAD "struct Position(x: f32, y: f32)"
        LINE "struct Velocity(x: f32, y: f32)"
        LINE ""
        LINE "query Move {"
        LINE "   Posit"
        LINE "}") != 0);

    test_assert(ecs_script_update(world, s, 0,
        HEAD "struct Position(x: f32, y: f32)"
        LINE "struct Velocity(x: f32, y: f32)"
        LINE ""
        LINE "query Move {"
        LINE "   Position"
        LINE "}") == 0);

    {
        const EcsScript *sc = ecs_get(world, s, EcsScript);
        test_assert(sc != NULL);
        test_assert(sc->error == NULL);
    }

    ecs_fini(world);
}

void Query_query_as_member_name(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t s = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Settings" }),
        .members = {
            {"query", ecs_id(ecs_i32_t)}
        }
    });

    const char *expr =
    HEAD "e { Settings: {query: 10} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);
    const int32_t *v = ecs_get_id(world, e, s);
    test_assert(v != NULL);
    test_int(*v, 10);

    ecs_fini(world);
}

void Query_query_as_entity_name(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "query {}";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) != 0);

    expr =
    HEAD "\"query\" {}";

    ecs_log_set_level(-1);
    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);
    test_assert(ecs_lookup(world, "query") != 0);

    ecs_fini(world);
}

void Query_for_entity_and_two_fields(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t position = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Position" }),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    ecs_entity_t velocity = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Velocity" }),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    ecs_entity_t a = ecs_entity(world, { .name = "a" });
    ecs_set_id(world, a, position, sizeof(Position), &(Position){10, 20});
    ecs_set_id(world, a, velocity, sizeof(Velocity), &(Velocity){1, 2});

    ecs_entity_t b = ecs_entity(world, { .name = "b" });
    ecs_set_id(world, b, position, sizeof(Position), &(Position){30, 40});
    ecs_set_id(world, b, velocity, sizeof(Velocity), &(Velocity){3, 4});

    ecs_entity_t c = ecs_entity(world, { .name = "c" });
    ecs_set_id(world, c, position, sizeof(Position), &(Position){50, 60});

    const char *expr =
    HEAD "query movers { Position, Velocity }"
    LINE "for (e, p, v) in movers {"
    LINE "  \"m_{e.name()}\" { Position: {p.x + v.x, p.y + v.y} }"
    LINE "}";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t m_a = ecs_lookup(world, "m_a");
    ecs_entity_t m_b = ecs_lookup(world, "m_b");
    test_assert(m_a != 0);
    test_assert(m_b != 0);
    test_assert(ecs_lookup(world, "m_c") == 0);

    const Position *p = ecs_get_id(world, m_a, position);
    test_assert(p != NULL);
    test_int(p->x, 11);
    test_int(p->y, 22);

    p = ecs_get_id(world, m_b, position);
    test_assert(p != NULL);
    test_int(p->x, 33);
    test_int(p->y, 44);

    ecs_fini(world);
}

void Query_for_two_fields(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t position = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Position" }),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    ecs_entity_t velocity = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Velocity" }),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    ecs_entity_t a = ecs_entity(world, { .name = "a" });
    ecs_set_id(world, a, position, sizeof(Position), &(Position){10, 20});
    ecs_set_id(world, a, velocity, sizeof(Velocity), &(Velocity){1, 2});

    const char *expr =
    HEAD "query movers { Position, Velocity }"
    LINE "for (p, v) in movers {"
    LINE "  m { Position: {p.x + v.x, p.y + v.y} }"
    LINE "}";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t m = ecs_lookup(world, "m");
    test_assert(m != 0);
    const Position *p = ecs_get_id(world, m, position);
    test_assert(p != NULL);
    test_int(p->x, 11);
    test_int(p->y, 22);

    ecs_fini(world);
}

void Query_for_tag_query(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t tag = ecs_entity(world, { .name = "Tag" });
    ecs_entity_t a = ecs_entity(world, { .name = "a" });
    ecs_add_id(world, a, tag);

    const char *expr =
    HEAD "Found {}"
    LINE "query q { Tag }"
    LINE "for e in q {"
    LINE "  \"f_{e.name()}\" { Found }"
    LINE "}";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t found = ecs_lookup(world, "Found");
    test_assert(found != 0);
    ecs_entity_t f_a = ecs_lookup(world, "f_a");
    test_assert(f_a != 0);
    test_assert(ecs_has_id(world, f_a, found));

    ecs_fini(world);
}

void Query_for_paren_vars(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t position = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Position" }),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    ecs_entity_t a = ecs_entity(world, { .name = "a" });
    ecs_set_id(world, a, position, sizeof(Position), &(Position){10, 20});

    const char *expr =
    HEAD "query q { Position }"
    LINE "for (e, p) in q {"
    LINE "  \"m_{e.name()}\" { Position: {p.x, p.y} }"
    LINE "}"
    LINE "for p in q {"
    LINE "  \"n_{p.y}\" {}"
    LINE "}";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t m_a = ecs_lookup(world, "m_a");
    test_assert(m_a != 0);
    const Position *p = ecs_get_id(world, m_a, position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    test_assert(ecs_lookup(world, "n_20") != 0);

    ecs_fini(world);
}

void Query_for_wrong_var_count(void) {
    ecs_world_t *world = ecs_init();

    ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Position" }),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    const char *expr =
    HEAD "query q { Position }"
    LINE "for (a, b, c) in q {"
    LINE "}";

    ecs_log_set_level(-4);
    ecs_script_eval_result_t result = {0};
    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, &result) != 0);
    test_assert(result.error != NULL);
    test_assert(strstr(result.error, "expected 1 or 2") != NULL);
    test_int(result.line, 2);
    ecs_os_free(result.error);

    ecs_fini(world);
}

void Query_for_tag_query_wrong_var_count(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity(world, { .name = "Tag" });

    const char *expr =
    HEAD "query q { Tag }"
    LINE "for (a, b) in q {"
    LINE "}";

    ecs_log_set_level(-4);
    ecs_script_eval_result_t result = {0};
    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, &result) != 0);
    test_assert(result.error != NULL);
    test_assert(strstr(result.error, "no data fields") != NULL);
    ecs_os_free(result.error);

    ecs_fini(world);
}

void Query_for_assign_to_value(void) {
    ecs_world_t *world = ecs_init();

    ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Position" }),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    const char *expr =
    HEAD "query q { Position }"
    LINE "for (e, p) in q {"
    LINE "  p = {1, 2}"
    LINE "}";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) != 0);

    ecs_fini(world);
}

void Query_for_assign_to_member(void) {
    ecs_world_t *world = ecs_init();

    ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Position" }),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    const char *expr =
    HEAD "query q { Position }"
    LINE "for (e, p) in q {"
    LINE "  p.x = 1"
    LINE "}";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) != 0);

    ecs_fini(world);
}

void Query_for_continue(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t position = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Position" }),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    ecs_entity_t a = ecs_entity(world, { .name = "a" });
    ecs_set_id(world, a, position, sizeof(Position), &(Position){1, 0});
    ecs_entity_t b = ecs_entity(world, { .name = "b" });
    ecs_set_id(world, b, position, sizeof(Position), &(Position){2, 0});
    ecs_entity_t c = ecs_entity(world, { .name = "c" });
    ecs_set_id(world, c, position, sizeof(Position), &(Position){3, 0});

    const char *expr =
    HEAD "query q { Position }"
    LINE "for (e, p) in q {"
    LINE "  if p.x == 2 {"
    LINE "    continue"
    LINE "  }"
    LINE "  \"m_{e.name()}\" {}"
    LINE "}";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    test_assert(ecs_lookup(world, "m_a") != 0);
    test_assert(ecs_lookup(world, "m_b") == 0);
    test_assert(ecs_lookup(world, "m_c") != 0);

    ecs_fini(world);
}

void Query_for_break(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t tag = ecs_entity(world, { .name = "Tag" });
    ecs_entity_t a = ecs_entity(world, { .name = "a" });
    ecs_add_id(world, a, tag);
    ecs_entity_t b = ecs_entity(world, { .name = "b" });
    ecs_add_id(world, b, tag);

    const char *expr =
    HEAD "query q { Tag }"
    LINE "for e in q {"
    LINE "  \"m_{e.name()}\" {}"
    LINE "  break"
    LINE "}";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    int32_t count = (ecs_lookup(world, "m_a") != 0) +
        (ecs_lookup(world, "m_b") != 0);
    test_int(count, 1);

    ecs_fini(world);
}

void Query_for_paren_vars_many(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t a = ecs_entity(world, { .name = "a" });

    const char *expr =
    HEAD "struct A(v: i32)"
    LINE "struct B(v: i32)"
    LINE "struct C(v: i32)"
    LINE "struct D(v: i32)"
    LINE "a { A: {1}; B: {2}; C: {3}; D: {4} }"
    LINE "query q { A, B, C, D }"
    LINE "for (e, x, y, z, w) in q {"
    LINE "  \"m_{e.name()}_{x.v}_{y.v}_{z.v}_{w.v}\" {}"
    LINE "}"
    LINE "for (x, y, z, w) in q {"
    LINE "  \"n_{x.v}_{y.v}_{z.v}_{w.v}\" {}"
    LINE "}";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    test_assert(a != 0);
    test_assert(ecs_lookup(world, "m_a_1_2_3_4") != 0);
    test_assert(ecs_lookup(world, "n_1_2_3_4") != 0);

    ecs_fini(world);
}

void Query_for_bracket_vars(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity(world, { .name = "Tag" });

    const char *expr =
    HEAD "query q { Tag }"
    LINE "for [e] in q {"
    LINE "}";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) != 0);

    ecs_fini(world);
}

void Query_for_body_adds_to_iterated_table(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t tag = ecs_entity(world, { .name = "Tag" });
    ecs_add_id(world, ecs_entity(world, { .name = "a" }), tag);
    ecs_add_id(world, ecs_entity(world, { .name = "b" }), tag);
    ecs_add_id(world, ecs_entity(world, { .name = "c" }), tag);
    ecs_add_id(world, ecs_entity(world, { .name = "d" }), tag);

    const char *expr =
    HEAD "query q { Tag }"
    LINE "for e in q {"
    LINE "  \"m_{e.name()}\" { Tag }"
    LINE "}";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    test_assert(ecs_lookup(world, "m_a") != 0);
    test_assert(ecs_lookup(world, "m_b") != 0);
    test_assert(ecs_lookup(world, "m_c") != 0);
    test_assert(ecs_lookup(world, "m_d") != 0);
    test_assert(ecs_lookup(world, "m_m_a") == 0);

    ecs_fini(world);
}

void Query_unbound_var(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t likes = ecs_entity(world, { .name = "Likes" });
    ecs_entity_t tag = ecs_entity(world, { .name = "Tag" });
    ecs_entity_t a = ecs_entity(world, { .name = "a" });
    ecs_entity_t b = ecs_entity(world, { .name = "b" });
    ecs_entity_t c = ecs_entity(world, { .name = "c" });
    ecs_add_id(world, b, tag);
    ecs_add_pair(world, a, likes, b);
    ecs_add_pair(world, c, likes, a);

    const char *expr =
    HEAD "query q { (Likes, $x), Tag($x) }"
    LINE "for e in q {"
    LINE "  \"m_{e.name()}\" {}"
    LINE "}";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    test_assert(ecs_lookup(world, "m_a") != 0);
    test_assert(ecs_lookup(world, "m_c") == 0);

    ecs_fini(world);
}

void Query_if_true(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t tag = ecs_entity(world, { .name = "Tag" });
    ecs_entity_t a = ecs_entity(world, { .name = "a" });
    ecs_add_id(world, a, tag);

    const char *expr =
    HEAD "query q { Tag }"
    LINE "if q {"
    LINE "  yes {}"
    LINE "} else {"
    LINE "  no {}"
    LINE "}";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    test_assert(ecs_lookup(world, "yes") != 0);
    test_assert(ecs_lookup(world, "no") == 0);

    ecs_fini(world);
}

void Query_if_false(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity(world, { .name = "Tag" });

    const char *expr =
    HEAD "query q { Tag }"
    LINE "if q {"
    LINE "  yes {}"
    LINE "} else {"
    LINE "  no {}"
    LINE "}";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    test_assert(ecs_lookup(world, "yes") == 0);
    test_assert(ecs_lookup(world, "no") != 0);

    ecs_fini(world);
}

void Query_if_not(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t tag = ecs_entity(world, { .name = "Tag" });
    ecs_entity(world, { .name = "Other" });
    ecs_entity_t a = ecs_entity(world, { .name = "a" });
    ecs_add_id(world, a, tag);

    const char *expr =
    HEAD "query q { Tag }"
    LINE "query r { Other }"
    LINE "if !q {"
    LINE "  not_q {}"
    LINE "}"
    LINE "if !r {"
    LINE "  not_r {}"
    LINE "}";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    test_assert(ecs_lookup(world, "not_q") == 0);
    test_assert(ecs_lookup(world, "not_r") != 0);

    ecs_fini(world);
}

void Query_if_and_or(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t tag = ecs_entity(world, { .name = "Tag" });
    ecs_entity(world, { .name = "Other" });
    ecs_entity_t a = ecs_entity(world, { .name = "a" });
    ecs_add_id(world, a, tag);

    const char *expr =
    HEAD "const x = true"
    LINE "query q { Tag }"
    LINE "query r { Other }"
    LINE "if q && x {"
    LINE "  q_and_x {}"
    LINE "}"
    LINE "if r && x {"
    LINE "  r_and_x {}"
    LINE "}"
    LINE "if r || q {"
    LINE "  r_or_q {}"
    LINE "}"
    LINE "if q {"
    LINE "  q_true {}"
    LINE "} else if r {"
    LINE "  r_true {}"
    LINE "}";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    test_assert(ecs_lookup(world, "q_and_x") != 0);
    test_assert(ecs_lookup(world, "r_and_x") == 0);
    test_assert(ecs_lookup(world, "r_or_q") != 0);
    test_assert(ecs_lookup(world, "q_true") != 0);
    test_assert(ecs_lookup(world, "r_true") == 0);

    ecs_fini(world);
}

void Query_const_from_query(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t tag = ecs_entity(world, { .name = "Tag" });
    ecs_entity_t a = ecs_entity(world, { .name = "a" });
    ecs_add_id(world, a, tag);

    const char *expr =
    HEAD "query q { Tag }"
    LINE "const any = q"
    LINE "if any {"
    LINE "  yes {}"
    LINE "}";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    test_assert(ecs_lookup(world, "yes") != 0);

    ecs_fini(world);
}

void Query_if_in_template(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t parent = ecs_entity(world, { .name = "parent" });
    ecs_entity(world, { .name = "empty" });
    ecs_entity(world, { .name = "a", .parent = parent });

    const char *expr =
    HEAD "template Full {"
    LINE "  query kids { (flecs.core.ChildOf, parent) }"
    LINE "  if kids {"
    LINE "    chevron {}"
    LINE "  }"
    LINE "}"
    LINE "template Empty {"
    LINE "  query kids { (flecs.core.ChildOf, empty) }"
    LINE "  if kids {"
    LINE "    chevron {}"
    LINE "  }"
    LINE "}"
    LINE "n1 { Full }"
    LINE "n2 { Empty }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    test_assert(ecs_lookup(world, "n1.chevron") != 0);
    test_assert(ecs_lookup(world, "n2.chevron") == 0);

    ecs_fini(world);
}

void Query_scope(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity(world, { .name = "Tag" });

    const char *expr =
    HEAD "parent {"
    LINE "  query q { Tag }"
    LINE "}"
    LINE "if q {"
    LINE "}";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) != 0);

    ecs_fini(world);
}

void Query_redeclare(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity(world, { .name = "Tag" });

    const char *expr =
    HEAD "query q { Tag }"
    LINE "query q { Tag }";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) != 0);

    ecs_fini(world);
}

void Query_identifier_name(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t tag = ecs_entity(world, { .name = "Tag" });
    ecs_entity_t a = ecs_entity(world, { .name = "a" });
    ecs_entity_t b = ecs_entity(world, { .name = "b" });
    ecs_add_id(world, a, tag);
    ecs_add_id(world, b, tag);

    const char *expr =
    HEAD "query q { Tag, (flecs.core.Identifier, flecs.core.Name) }"
    LINE "for (e, n) in q {"
    LINE "  \"m_{n.value}\" {}"
    LINE "}";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    test_assert(ecs_lookup(world, "m_a") != 0);
    test_assert(ecs_lookup(world, "m_b") != 0);

    ecs_fini(world);
}

void Query_using(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t tag = ecs_entity(world, { .name = "game.Tag" });
    ecs_entity_t a = ecs_entity(world, { .name = "a" });
    ecs_add_id(world, a, tag);

    const char *expr =
    HEAD "using game"
    LINE "query q { Tag }"
    LINE "for e in q {"
    LINE "  \"m_{e.name()}\" {}"
    LINE "}";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    test_assert(ecs_lookup(world, "m_a") != 0);

    ecs_fini(world);
}

void Query_ast_to_str(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity(world, { .name = "Tag" });

    const char *expr =
    HEAD "query q { Tag }"
    LINE "for e in q {"
    LINE "}";

    ecs_script_t *script = ecs_script_parse(world, NULL, expr, NULL, NULL);
    test_assert(script != NULL);

    char *str = ecs_script_ast_to_str(script, false);
    test_assert(str != NULL);
    test_assert(strstr(str, "query: q { Tag }") != NULL);
    ecs_os_free(str);

    ecs_script_free(script);

    ecs_fini(world);
}
