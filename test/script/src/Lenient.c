#include <script.h>

static int lenient_warn_count = 0;

static void lenient_warn_callback(
    int32_t level,
    const char *file,
    int32_t line,
    const char *msg)
{
    (void)file;
    (void)line;
    (void)msg;
    if (level == -2) {
        lenient_warn_count ++;
    }
}

static void lenient_warn_capture(void) {
    ecs_os_set_api_defaults();
    ecs_os_api_t os_api = ecs_os_api;
    os_api.log_ = lenient_warn_callback;
    ecs_os_set_api(&os_api);
    ecs_log_set_level(-2);
    lenient_warn_count = 0;
}

static void lenient_warn_release(void) {
    ecs_os_set_api_defaults();
    ecs_log_set_level(-1);
}

void Lenient_unknown_tag(void) {
    ecs_world_t *world = ecs_init();

    ecs_script_set_lenient(world, true);
    test_bool(ecs_script_get_lenient(world), true);

    const char *expr =
    HEAD "e {"
    LINE "  NsUnknownTag"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);
    test_assert(ecs_lookup(world, "NsUnknownTag") == 0);

    const ecs_type_t *type = ecs_get_type(world, e);
    test_assert(type != NULL);
    test_int(type->count, 1); /* (Identifier, Name) */

    ecs_fini(world);
}

void Lenient_unknown_component_w_value(void) {
    ecs_world_t *world = ecs_init();

    ecs_script_set_lenient(world, true);

    const char *expr =
    HEAD "e {"
    LINE "  NsUnknownComp: {x: 10, y: 20}"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);
    test_assert(ecs_lookup(world, "NsUnknownComp") == 0);

    ecs_fini(world);
}

void Lenient_unknown_component_w_nested_value(void) {
    ecs_world_t *world = ecs_init();

    ecs_script_set_lenient(world, true);

    const char *expr =
    HEAD "e {"
    LINE "  NsUnknownComp: {"
    LINE "    a: {b: 1, c: {d: 2}},"
    LINE "    e: [1, 2, 3]"
    LINE "  }"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);
    test_assert(ecs_lookup(world, "NsUnknownComp") == 0);

    ecs_fini(world);
}

void Lenient_unknown_component_in_scope(void) {
    ecs_world_t *world = ecs_init();

    ecs_script_set_lenient(world, true);

    ECS_COMPONENT(world, Position);
    ecs_struct(world, {
        .entity = ecs_id(Position),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    const char *expr =
    HEAD "e {"
    LINE "  night_shift.NsWalkRectDecl: {w: 1}"
    LINE "  Position: {x: 10, y: 20}"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);
    test_assert(ecs_lookup(world, "night_shift") == 0);
    test_assert(ecs_lookup(world, "night_shift.NsWalkRectDecl") == 0);

    const Position *p = ecs_get(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void Lenient_unknown_pair(void) {
    ecs_world_t *world = ecs_init();

    ecs_script_set_lenient(world, true);

    const char *expr =
    HEAD "e {"
    LINE "  (NsUnknownRel, NsUnknownTgt)"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);
    test_assert(ecs_lookup(world, "NsUnknownRel") == 0);
    test_assert(ecs_lookup(world, "NsUnknownTgt") == 0);

    ecs_fini(world);
}

void Lenient_unknown_member_on_known_component(void) {
    ecs_world_t *world = ecs_init();

    ecs_script_set_lenient(world, true);

    ECS_COMPONENT(world, Position);
    ecs_struct(world, {
        .entity = ecs_id(Position),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    const char *expr =
    HEAD "e {"
    LINE "  Position: {x: 10, unknown_member: 5, y: 20}"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);

    const Position *p = ecs_get(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void Lenient_unknown_nested_member_on_known_component(void) {
    ecs_world_t *world = ecs_init();

    ecs_script_set_lenient(world, true);

    ECS_COMPONENT(world, Position);
    ecs_struct(world, {
        .entity = ecs_id(Position),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    const char *expr =
    HEAD "e {"
    LINE "  Position: {x: 10, unknown_member: {a: 1, b: {c: 2}}, y: 20}"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);

    const Position *p = ecs_get(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void Lenient_unknown_function_in_expr(void) {
    ecs_world_t *world = ecs_init();

    ecs_script_set_lenient(world, true);

    ECS_COMPONENT(world, Position);
    ecs_struct(world, {
        .entity = ecs_id(Position),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    const char *expr =
    HEAD "a {"
    LINE "  Position: {x: wayfinding.blades(4), y: 20}"
    LINE "}"
    LINE "b {"
    LINE "  Position: {x: 1, y: 2}"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t a = ecs_lookup(world, "a");
    test_assert(a != 0);
    test_assert(ecs_get(world, a, Position) == NULL);

    ecs_entity_t b = ecs_lookup(world, "b");
    test_assert(b != 0);
    const Position *p = ecs_get(world, b, Position);
    test_assert(p != NULL);
    test_int(p->x, 1);
    test_int(p->y, 2);

    ecs_fini(world);
}

void Lenient_unknown_vector_in_for(void) {
    ecs_world_t *world = ecs_init();

    ecs_script_set_lenient(world, true);

    const char *expr =
    HEAD "for v in $ns_unknown_vector {"
    LINE "  e {}"
    LINE "}"
    LINE "after {}";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    test_assert(ecs_lookup(world, "e") == 0);
    test_assert(ecs_lookup(world, "after") != 0);

    ecs_fini(world);
}

void Lenient_unknown_function_in_for(void) {
    ecs_world_t *world = ecs_init();

    ecs_script_set_lenient(world, true);

    const char *expr =
    HEAD "for v in wayfinding.points(4) {"
    LINE "  e {}"
    LINE "}"
    LINE "after {}";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    test_assert(ecs_lookup(world, "e") == 0);
    test_assert(ecs_lookup(world, "after") != 0);

    ecs_fini(world);
}

void Lenient_warn_once_per_name(void) {
    lenient_warn_capture();

    ecs_world_t *world = ecs_init();

    ecs_script_set_lenient(world, true);

    const char *expr =
    HEAD "a { NsUnknownTag }"
    LINE "b { NsUnknownTag }"
    LINE "c { NsUnknownTag }";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);
    test_int(lenient_warn_count, 1);

    ecs_fini(world);

    lenient_warn_release();
}

void Lenient_warn_per_distinct_name(void) {
    lenient_warn_capture();

    ecs_world_t *world = ecs_init();

    ecs_script_set_lenient(world, true);

    const char *expr =
    HEAD "a { NsUnknownTagA }"
    LINE "b { NsUnknownTagB }"
    LINE "c { NsUnknownTagA }"
    LINE "d { NsUnknownTagB }";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);
    test_int(lenient_warn_count, 2);

    ecs_fini(world);

    lenient_warn_release();
}

void Lenient_strict_unknown_tag_errors(void) {
    ecs_world_t *world = ecs_init();

    test_bool(ecs_script_get_lenient(world), false);

    const char *expr =
    HEAD "e {"
    LINE "  NsUnknownTag"
    LINE "}";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run(world, NULL, expr, NULL) != 0);
    ecs_log_set_level(-1);

    ecs_fini(world);
}

void Lenient_strict_unknown_member_errors(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ecs_struct(world, {
        .entity = ecs_id(Position),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    const char *expr =
    HEAD "e {"
    LINE "  Position: {x: 10, unknown_member: 5}"
    LINE "}";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run(world, NULL, expr, NULL) != 0);
    ecs_log_set_level(-1);

    ecs_fini(world);
}

void Lenient_lenient_disabled_after_enable(void) {
    ecs_world_t *world = ecs_init();

    ecs_script_set_lenient(world, true);
    test_assert(ecs_script_run(world, NULL, "a { NsUnknownTag }", NULL) == 0);

    ecs_script_set_lenient(world, false);
    test_bool(ecs_script_get_lenient(world), false);

    ecs_log_set_level(-4);
    test_assert(ecs_script_run(world, NULL, "b { NsUnknownTag }", NULL) != 0);
    ecs_log_set_level(-1);

    ecs_fini(world);
}

void Lenient_no_placeholder_then_strict_load(void) {
    ecs_world_t *world = ecs_init();

    ecs_script_set_lenient(world, true);

    const char *expr =
    HEAD "e {"
    LINE "  Position: {x: 10, y: 20}"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    test_assert(ecs_lookup(world, "Position") == 0);

    ECS_COMPONENT(world, Position);
    ecs_struct(world, {
        .entity = ecs_id(Position),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    ecs_script_set_lenient(world, false);

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);
    const Position *p = ecs_get(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void Lenient_template_w_unknown_component(void) {
    ecs_world_t *world = ecs_init();

    ecs_script_set_lenient(world, true);

    ECS_COMPONENT(world, Position);
    ecs_struct(world, {
        .entity = ecs_id(Position),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    const char *expr =
    HEAD "template Lamppost {"
    LINE "  prop height: f32 = 5"
    LINE "  Position: {x: 1, y: $height}"
    LINE "  Nightlight: {intensity: 3}"
    LINE "  HoloCycle"
    LINE "}"
    LINE ""
    LINE "e {"
    LINE "  Lamppost: {height: 7}"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    test_assert(ecs_lookup(world, "Nightlight") == 0);
    test_assert(ecs_lookup(world, "HoloCycle") == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);

    const Position *p = ecs_get(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 1);
    test_int(p->y, 7);

    ecs_fini(world);
}

void Lenient_template_child_w_unknown_component(void) {
    ecs_world_t *world = ecs_init();

    ecs_script_set_lenient(world, true);

    ECS_COMPONENT(world, Position);
    ecs_struct(world, {
        .entity = ecs_id(Position),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    const char *expr =
    HEAD "template Lamppost {"
    LINE "  prop height: f32 = 5"
    LINE "  Position: {x: 1, y: $height}"
    LINE "  head {"
    LINE "    Position: {x: 3, y: 4}"
    LINE "    Nightlight: {intensity: 3}"
    LINE "  }"
    LINE "}"
    LINE ""
    LINE "e {"
    LINE "  Lamppost: {height: 7}"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);

    ecs_entity_t head = ecs_lookup(world, "e.head");
    test_assert(head != 0);

    const Position *p = ecs_get(world, head, Position);
    test_assert(p != NULL);
    test_int(p->x, 3);
    test_int(p->y, 4);

    ecs_fini(world);
}

void Lenient_isa_unresolved_errors(void) {
    ecs_world_t *world = ecs_init();

    ecs_script_set_lenient(world, true);

    const char *expr =
    HEAD "e : NsUnknownPrefab {"
    LINE "}";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run(world, NULL, expr, NULL) != 0);
    ecs_log_set_level(-1);

    ecs_fini(world);
}

void Lenient_eval_desc(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "e {"
    LINE "  NsUnknownTag"
    LINE "}";

    ecs_script_eval_desc_t desc = { .lenient = true };
    ecs_script_t *script = ecs_script_parse(world, NULL, expr, &desc, NULL);
    test_assert(script != NULL);
    test_assert(ecs_script_eval(script, &desc, NULL) == 0);
    ecs_script_free(script);

    test_assert(ecs_lookup(world, "e") != 0);
    test_assert(ecs_lookup(world, "NsUnknownTag") == 0);

    ecs_fini(world);
}

void Lenient_managed_script_desc(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ecs_struct(world, {
        .entity = ecs_id(Position),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    const char *expr =
    HEAD "e {"
    LINE "  NsUnknownTag"
    LINE "  Position: {x: 10, y: 20}"
    LINE "}";

    ecs_entity_t s = ecs_script(world, {
        .code = expr,
        .lenient = true
    });
    test_assert(s != 0);

    const EcsScript *sc = ecs_get(world, s, EcsScript);
    test_assert(sc != NULL);
    test_assert(sc->error == NULL);

    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);
    test_assert(ecs_lookup(world, "NsUnknownTag") == 0);

    const Position *p = ecs_get(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void Lenient_managed_script_strict_errors(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "e {"
    LINE "  NsUnknownTag"
    LINE "}";

    ecs_log_set_level(-4);
    ecs_entity_t s = ecs_script(world, {
        .code = expr
    });
    ecs_log_set_level(-1);
    test_assert(s != 0);

    const EcsScript *sc = ecs_get(world, s, EcsScript);
    test_assert(sc != NULL);
    test_assert(sc->error != NULL);

    ecs_fini(world);
}

void Lenient_unknown_with_tag(void) {
    ecs_world_t *world = ecs_init();

    ecs_script_set_lenient(world, true);

    ECS_TAG(world, Foo);

    const char *expr =
    HEAD "with NsUnknownTag, Foo {"
    LINE "  e {}"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);
    test_assert(ecs_has_id(world, e, Foo));
    test_assert(ecs_lookup(world, "NsUnknownTag") == 0);

    ecs_fini(world);
}
