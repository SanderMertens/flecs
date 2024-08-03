#include <script.h>

void Error_multi_line_comment_after_newline_before_newline_scope_open(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "Parent"
    LINE "/* Some Comment() */"
    LINE ""
    LINE "{"
    LINE " Child{}"
    LINE "}"
    LINE "Foo{}";

    ecs_log_set_level(-4); /* Newline after multiline comment is not ignored */
    test_assert(ecs_script_run(world, NULL, expr) != 0);

    ecs_fini(world);
}

void Error_missing_end_of_scope(void) {
    ecs_log_set_level(-4);
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "Parent {"
    LINE " Child {}";

    test_assert(ecs_script_run(world, NULL, expr) != 0);

    test_assert(ecs_get_scope(world) == 0);
    test_assert(ecs_get_with(world) == 0);

    ecs_fini(world);
}

void Error_with_n_tags_2_levels_invalid_tag(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, TagC);
    ECS_TAG(world, TagD);
    ECS_TAG(world, TagE);

    const char *expr =
    HEAD "with TagA, TagB {"
    LINE " with TagC, TagD, TagE {"
    LINE "  Foo {}"
    LINE " }"
    LINE " HelloA"
    LINE " with TagF, TagG, TagH {"
    LINE "  Bar {}"
    LINE " }"
    LINE " HelloB {}"
    LINE "}"
    LINE "HelloC {}";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run(world, NULL, expr) != 0);

    ecs_fini(world);
}

void Error_assignment_to_non_component(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "Foo { Position: {x: 10, y: 20} }";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run(world, NULL, expr) != 0);

    ecs_fini(world);
}

void Error_struct_w_member_w_assignment_to_nothing(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "flecs.meta.struct Position {"
    LINE "  x { flecs.meta.Member: }"
    LINE "}";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run(world, NULL, expr) != 0);

    ecs_fini(world);
}

void Error_struct_w_member_w_assignment_to_empty_scope(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "flecs.meta.struct Position {"
    LINE "  x { flecs.meta.member: { }"
    LINE "}";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run(world, NULL, expr) != 0);

    ecs_fini(world);
}

void Error_invalid_nested_assignment(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "Foo {"
    LINE "Bar { Hello }";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run(world, NULL, expr) != 0);

    ecs_fini(world);
}

void Error_invalid_partial_pair_assignment(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "Foo { (Hello, }";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run(world, NULL, expr) != 0);

    ecs_fini(world);
}

void Error_empty_assignment(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "Foo {";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run(world, NULL, expr) != 0);

    ecs_fini(world);
}

void Error_empty_assignment_before_end_of_scope(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "{Foo {}";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run(world, NULL, expr) != 0);

    ecs_fini(world);
}

void Error_default_type_with_tag(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "with Foo {"
    LINE "  e1 = 10, 20"
    LINE "  e2 = 30, 40"
    LINE "}";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run(world, NULL, expr) != 0);

    ecs_fini(world);
}

void Error_invalid_oneof(void) {
    ecs_log_set_level(-4);
    
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "flecs.meta.enum Color {"
    LINE "  flecs.meta.constant Red"
    LINE "  flecs.meta.constant Green"
    LINE "  flecs.meta.constant Blue"
    LINE "}"
    LINE "e { (Color, Foo) }";

    test_assert(ecs_script_run(world, NULL, expr) != 0);

    ecs_entity_t color = ecs_lookup(world, "Color");
    ecs_entity_t foo = ecs_lookup(world, "Foo");
    ecs_entity_t e = ecs_lookup(world, "e");

    test_assert(color != 0);
    test_assert(foo == 0);
    test_assert(e != 0);

    test_assert( !ecs_has_pair(world, e, color, EcsWildcard));

    ecs_fini(world);
}

void Error_unterminated_multiline_string(void) {
    ecs_world_t *world = ecs_init();

    ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "String"}),
        .members = {
            {"value", ecs_id(ecs_string_t)}
        }
    });

    const char *expr =
    HEAD "Foo { String: {value: `start"
    LINE "Hello World"
    LINE "Foo Bar"
    LINE "Special characters }{\"\"'',"
    LINE "}}";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run(world, NULL, expr) != 0);

    ecs_fini(world);
}

void Error_const_var_redeclare(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "const var_x = 10"
    LINE "const var_x = 20";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run(world, NULL, expr) != 0);

    ecs_fini(world);
}

void Error_typed_const_w_composite_type_invalid_assignment(void) {
    ecs_world_t *world = ecs_init();

    ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "Position"}),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    const char *expr =
    HEAD "const var_pos = Position: Position{10, 20}"
    LINE "a { $var_pos }"
    LINE "";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run(world, NULL, expr) != 0);

    ecs_fini(world);
}

void Error_unterminated_multi_line_comment_in_value(void) {
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
    LINE "e {Position: {\n"
    LINE "  x: 10\n"
    LINE "  /*\n"
    LINE "   * foo\n"
    LINE "  y: 20\n"
    LINE "}}";
    ecs_log_set_level(-4);
    test_assert(ecs_script_run(world, NULL, expr) != 0);

    ecs_fini(world);
}

void Error_pair_w_rel_var_invalid_type(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Rel);
    ECS_TAG(world, Tgt);

    const char *expr =
    LINE "const rel = 10\n"
    LINE "ent {\n"
    LINE "  ($rel, Tgt)\n"
    LINE "}\n"
    LINE "\n";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run(world, NULL, expr) != 0);

    ecs_fini(world);
}

void Error_pair_w_tgt_var_invalid_type(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Rel);
    ECS_TAG(world, Tgt);

    const char *expr =
    LINE "const tgt = 10\n"
    LINE "ent {\n"
    LINE "  (Rel, $tgt)\n"
    LINE "}\n"
    LINE "\n";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run(world, NULL, expr) != 0);

    ecs_fini(world);
}

void Error_with_value_not_a_component(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    LINE "with Foo(10) {\n"
    LINE "  Bar {}\n"
    LINE "}\n"
    LINE "\n";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run(world, NULL, expr) != 0);

    ecs_fini(world);
}

void Error_component_in_with_scope(void) {
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
    LINE "with Position(10, 20) {\n"
    LINE "  Bar\n"
    LINE "}\n"
    LINE "\n";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run(world, NULL, expr) != 0);

    ecs_fini(world);
}

void Error_component_in_with_scope_nested(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_struct(world, {
        .entity = ecs_id(Position),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    ecs_struct(world, {
        .entity = ecs_id(Velocity),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    const char *expr =
    LINE "with Position(10, 20) {\n"
    LINE "  with Velocity(10, 20) {\n"
    LINE "    Bar\n"
    LINE "  }\n"
    LINE "}\n"
    LINE "\n";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run(world, NULL, expr) != 0);

    ecs_fini(world);
}

void Error_assign_after_with_in_scope(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_struct(world, {
        .entity = ecs_id(Position),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    ecs_struct(world, {
        .entity = ecs_id(Velocity),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    const char *expr =
    LINE "foo {\n"
    LINE "  Position: {10, 20}\n"
    LINE "  with Position(30, 40) {\n"
    LINE "  }\n"
    LINE "  Velocity: {1, 2}\n"
    LINE "}\n";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run(world, NULL, expr) == 0);

    ecs_entity_t foo = ecs_lookup(world, "foo");
    test_assert(foo != 0);

    test_assert(ecs_has(world, foo, Position));
    test_assert(ecs_has(world, foo, Velocity));

    const Position *p = ecs_get(world, foo, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    const Velocity *v = ecs_get(world, foo, Velocity);
    test_assert(v != NULL);
    test_int(v->x, 1);
    test_int(v->y, 2);

    ecs_fini(world);
}

void Error_not_an_array_component(void) {
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
    LINE "foo { Position: [10, 20] }\n";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run(world, NULL, expr) != 0);

    ecs_fini(world);
}

void Error_array_component_w_curly_brackets(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_array(world, {
        .entity = ecs_id(Position),
        .type = ecs_id(ecs_f32_t),
        .count = 2
    });

    const char *expr =
    LINE "foo { Position: {10, 20} }\n";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run(world, NULL, expr) != 0);

    ecs_fini(world);
}

void Error_unknown_identifier(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "using flecs.meta"
    LINE
    LINE "struct Comp {"
    LINE "  value = entity"
    LINE "}"
    LINE
    LINE "Foo { Comp: {A} }";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run(world, NULL, expr) != 0);

    ecs_fini(world);
}

void Error_unknown_identifier_for_int_field(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "using flecs.meta"
    LINE
    LINE "struct Comp {"
    LINE "  value = i32"
    LINE "}"
    LINE
    LINE "Foo { Comp: {A} }";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run(world, NULL, expr) != 0);

    ecs_fini(world);
}

void Error_prefab_w_slot_no_parent(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "slot Base {}";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run(world, NULL, expr) != 0);

    ecs_fini(world);
}

void Error_tag_not_found(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "Foo {"
    LINE " Bar"
    LINE "}";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run(world, NULL, expr) != 0);

    ecs_fini(world);
}

void Error_component_not_found(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "Foo {"
    LINE " Position: {10, 20}"
    LINE "}";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run(world, NULL, expr) != 0);

    ecs_fini(world);
}

void Error_pair_first_not_found(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tgt);

    const char *expr =
    HEAD "Foo {"
    LINE " (Rel, Tgt)"
    LINE "}";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run(world, NULL, expr) != 0);

    ecs_fini(world);
}

void Error_pair_second_not_found(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Rel);

    const char *expr =
    HEAD "Foo {"
    LINE " (Rel, Tgt)"
    LINE "}";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run(world, NULL, expr) != 0);

    ecs_fini(world);
}

void Error_kind_not_found(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Rel);

    const char *expr =
    HEAD "Foo Bar";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run(world, NULL, expr) != 0);

    ecs_fini(world);
}

void Error_base_not_found(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Rel);

    const char *expr =
    HEAD "Foo : Bar";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run(world, NULL, expr) != 0);

    ecs_fini(world);
}

void Error_entity_w_anonymous_tag(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    LINE "MyEntity {"
    LINE "  _"
    LINE "}";

    ecs_log_set_level(-4);
    ecs_entity_t s = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code = expr
    });
    test_assert(s == 0);

    ecs_fini(world);
}

void Error_member_expr_without_value_end_of_scope(void) {
    test_quarantine("Mon Aug 5"); // address when porting expressions over to new parser

    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "Position"}),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    test_assert(ecs_script_run(world, NULL, "Position(x:)") != 0);

    ecs_fini(world);
}

void Error_member_expr_without_value_comma(void) {
    test_quarantine("Mon Aug 5"); // address when porting expressions over to new parser

    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "Position"}),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    test_assert(ecs_script_run(world, NULL, "Position(x:,0)") != 0);

    ecs_fini(world);
}

void Error_member_expr_without_value_newline(void) {
    test_quarantine("Mon Aug 5"); // address when porting expressions over to new parser

    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "Position"}),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    test_assert(ecs_script_run(world, NULL, "Position(x:\n)") != 0);

    ecs_fini(world);
}

void Error_2_member_expr_without_value(void) {
    test_quarantine("Mon Aug 5"); // address when porting expressions over to new parser

    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "Position"}),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    test_assert(ecs_script_run(world, NULL, "Position(x:y:)") != 0);

    ecs_fini(world);
}

void Error_expr_junk_after_number(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "Position"}),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    ecs_log_set_level(-4);
    test_assert(ecs_script_run(world, NULL, "Position(0abc)") != 0);

    ecs_fini(world);
}

void Error_expr_junk_after_unary_minus(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "Position"}),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    ecs_log_set_level(-4);
    test_assert(ecs_script_run(world, NULL, "Position(-abc)") != 0);

    ecs_fini(world);
}

void Error_expr_comma_after_nothing(void) {
    test_quarantine("Mon Aug 5"); // address when porting expressions over to new parser

    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "Position"}),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    test_assert(ecs_script_run(world, NULL, "Position(,)") != 0);

    ecs_fini(world);
}

void Error_expr_digit_with_two_dots(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "Position"}),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    ecs_log_set_level(-4);
    test_assert(ecs_script_run(world, NULL, "Position(10.10.10)") != 0);

    ecs_fini(world);
}

void Error_template_empty(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    LINE "template Tree {"
    LINE "}";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run(world, NULL, expr) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    test_assert(tree != 0);

    ecs_fini(world);
}

void Error_template_unresolved_tag(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    LINE "template Tree {"
    LINE "  Foo"
    LINE "}"
    LINE "Tree ent()";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run(world, NULL, expr) != 0);

    ecs_fini(world);
}

void Error_template_unresolved_component(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "using flecs.meta"
    LINE "template Tree {"
    LINE "  struct Position {"
    LINE "    x = f32"
    LINE "  }"
    LINE ""
    LINE "  Position: {10, 20}"
    LINE "}";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run(world, NULL, expr) != 0);

    ecs_fini(world);
}

void Error_template_unresolved_pair_relationship(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "Bar {}"
    LINE "template Tree {"
    LINE "  (Foo, Bar)"
    LINE "}"
    LINE "Tree ent()";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run(world, NULL, expr) != 0);

    ecs_fini(world);
}

void Error_template_unresolved_pair_target(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "Foo {}"
    LINE "template Tree {"
    LINE "  (Foo, Bar)"
    LINE "}"
    LINE "Tree ent()";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run(world, NULL, expr) != 0);

    ecs_fini(world);
}

void Error_template_unresolved_with_tag(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    LINE "template Tree {"
    LINE "  with Foo {"
    LINE "  }"
    LINE "}"
    LINE "Tree ent()";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run(world, NULL, expr) != 0);

    ecs_fini(world);
}

void Error_template_unresolved_with_component(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "using flecs.meta"
    LINE "template Tree {"
    LINE "  struct Position {"
    LINE "    x = f32"
    LINE "  }"
    LINE ""
    LINE "  with Position(10, 20) {"
    LINE "  }"
    LINE "}";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run(world, NULL, expr) != 0);

    ecs_fini(world);
}

void Error_template_unresolved_with_pair_relationship(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "Bar {}"
    LINE "template Tree {"
    LINE "  with (Foo, Bar) {"
    LINE "  }"
    LINE "}"
    LINE "Tree ent()";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run(world, NULL, expr) != 0);

    ecs_fini(world);
}

void Error_template_unresolved_with_pair_target(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "Foo {}"
    LINE "template Tree {"
    LINE "  with (Foo, Bar) {"
    LINE "  }"
    LINE "}"
    LINE "Tree ent()";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run(world, NULL, expr) != 0);

    ecs_fini(world);
}

void Error_template_unresolved_tag_in_child(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "template Tree {"
    LINE "  child {"
    LINE "    Foo"
    LINE "  }"
    LINE "}"
    LINE "Tree ent()";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run(world, NULL, expr) != 0);

    ecs_fini(world);
}

void Error_template_prop_no_type(void) {
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
    LINE "template Tree {"
    LINE "  prop height"
    LINE "}";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run(world, NULL, expr) != 0);

    ecs_fini(world);
}

void Error_template_prop_no_default(void) {
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
    LINE "template Tree {"
    LINE "  prop height: flecs.meta.f32"
    LINE "}";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run(world, NULL, expr) != 0);

    ecs_fini(world);
}

void Error_template_w_composite_prop_invalid_assignment(void) {
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
    LINE "template Tree {"
    LINE "  prop pos = Position: Position{10, 20}"
    LINE "  child { $pos }"
    LINE "}"
    LINE ""
    LINE "t { Tree: {pos: {20, 30}} }";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run(world, NULL, expr) != 0);

    ecs_fini(world);
}

void Error_template_redeclare_prop_as_const(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    LINE "template Foo {\n"
    LINE "  prop x = flecs.meta.f32: 10\n"
    LINE "  prop y = flecs.meta.f32: 10\n"
    LINE "  const y = flecs.meta.f32: 20\n"
    LINE "}"
    LINE "ent { Foo: {} }\n"
    LINE "\n";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run(world, NULL, expr) != 0);

    ecs_fini(world);
}

void Error_template_redeclare_prop_as_prop(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    LINE "template Foo {\n"
    LINE "  prop x = flecs.meta.f32: 10\n"
    LINE "  prop y = flecs.meta.f32: 10\n"
    LINE "  prop y = flecs.meta.f32: 20\n"
    LINE "}"
    LINE "ent { Foo: {} }\n"
    LINE "\n";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run(world, NULL, expr) != 0);

    ecs_fini(world);
}

void Error_template_redeclare_const_as_const(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    LINE "template Foo {\n"
    LINE "  prop x = flecs.meta.f32: 10\n"
    LINE "  const y = flecs.meta.f32: 10\n"
    LINE "  const y = flecs.meta.f32: 20\n"
    LINE "}"
    LINE "ent { Foo: {} }\n"
    LINE "\n";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run(world, NULL, expr) != 0);

    ecs_fini(world);
}

void Error_run_template_after_error(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "Foo {}"
    LINE "template Tree {"
    LINE "  Foo"
    LINE "}";

    const char *expr_err =
    HEAD "Foo {}"
    LINE "template Tree {"
    LINE "  Fo"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    {
        ecs_entity_t tree = ecs_lookup(world, "Tree");
        test_assert(tree != 0);

        ecs_entity_t foo = ecs_lookup(world, "Foo");
        test_assert(foo != 0);
    }

    ecs_log_set_level(-4);
    test_assert(ecs_script_run(world, NULL, expr_err) != 0);
    ecs_log_set_level(-1);

    /* Because script is not managed, entiites created by script aren't deleted */

    {
        ecs_entity_t tree = ecs_lookup(world, "Tree");
        test_assert(tree != 0);

        ecs_entity_t foo = ecs_lookup(world, "Foo");
        test_assert(foo != 0);
    }

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    {
        ecs_entity_t tree = ecs_lookup(world, "Tree");
        test_assert(tree != 0);

        ecs_entity_t foo = ecs_lookup(world, "Foo");
        test_assert(foo != 0);
    }

    ecs_fini(world);
}

void Error_update_template_after_error(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "Foo {}"
    LINE "template Tree {"
    LINE "  Foo"
    LINE "}";

    const char *expr_err =
    HEAD "Foo {}"
    LINE "template Tree {"
    LINE "  Fo"
    LINE "}";

    ecs_entity_t s = ecs_script(world, { .code = expr });
    test_assert(s != 0);

    {
        ecs_entity_t tree = ecs_lookup(world, "Tree");
        test_assert(tree != 0);

        ecs_entity_t foo = ecs_lookup(world, "Foo");
        test_assert(foo != 0);
    }

    ecs_log_set_level(-4);
    test_assert(ecs_script_update(world, s, 0, expr_err) != 0);
    ecs_log_set_level(-1);

    {
        ecs_entity_t tree = ecs_lookup(world, "Tree");
        test_assert(tree == 0);

        ecs_entity_t foo = ecs_lookup(world, "Foo");
        test_assert(foo == 0);
    }

    test_assert(ecs_script_update(world, s, 0, expr) == 0);

    {
        ecs_entity_t tree = ecs_lookup(world, "Tree");
        test_assert(tree != 0);

        ecs_entity_t foo = ecs_lookup(world, "Foo");
        test_assert(foo != 0);
    }

    ecs_fini(world);
}

void Error_template_in_template(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "template Tree {"
    LINE "  template Bark {"
    LINE "  }"
    LINE "}";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run(world, NULL, expr) != 0);

    ecs_fini(world);
}
