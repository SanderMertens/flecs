#include <script.h>

void Function_simple(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Position" }),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    const char *expr =
    HEAD "fn add(a: i32, b: i32) -> i32 { a + b }"
    LINE "Foo = Position: {add(2, 3), add(10, 20)}";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    test_assert(foo != 0);
    const Position *p = ecs_get(world, foo, Position);
    test_assert(p != NULL);
    test_int(p->x, 5);
    test_int(p->y, 30);

    ecs_fini(world);
}

void Function_no_args(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Position" }),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    const char *expr =
    HEAD "fn five() -> i32 { 5 }"
    LINE "Foo = Position: {five(), five()}";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    test_assert(foo != 0);
    const Position *p = ecs_get(world, foo, Position);
    test_assert(p != NULL);
    test_int(p->x, 5);
    test_int(p->y, 5);

    ecs_fini(world);
}

void Function_with_locals(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Position" }),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    const char *expr =
    HEAD "fn poly(x: i32) -> i32 {"
    LINE "    const x2 = i32: x * x"
    LINE "    const x3 = i32: x2 * x"
    LINE "    x3 + x2"
    LINE "}"
    LINE "Foo = Position: {poly(3), poly(2)}";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    test_assert(foo != 0);
    const Position *p = ecs_get(world, foo, Position);
    test_assert(p != NULL);
    test_int(p->x, 36);
    test_int(p->y, 12);

    ecs_fini(world);
}

void Function_called_multiple_times(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Position" }),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    const char *expr =
    HEAD "fn dbl(a: i32) -> i32 { a + a }"
    LINE "Foo = Position: {dbl(3), dbl(7)}";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    test_assert(foo != 0);
    const Position *p = ecs_get(world, foo, Position);
    test_assert(p != NULL);
    test_int(p->x, 6);
    test_int(p->y, 14);

    ecs_fini(world);
}

void Function_missing_return_expr(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "fn bad(a: i32) -> i32 { const x = i32: a }";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run(world, NULL, expr, NULL) != 0);

    ecs_fini(world);
}

void Function_nested(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Position" }),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    const char *expr =
    HEAD "fn add(a: i32, b: i32) -> i32 { a + b }"
    LINE "fn add3(a: i32, b: i32, c: i32) -> i32 { add(add(a, b), c) }"
    LINE "Foo = Position: {add3(1, 2, 3), add3(10, 20, 30)}";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    test_assert(foo != 0);
    const Position *p = ecs_get(world, foo, Position);
    test_assert(p != NULL);
    test_int(p->x, 6);
    test_int(p->y, 60);

    ecs_fini(world);
}

void Function_string_arg(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Position" }),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    const char *expr =
    HEAD "fn len(s: string) -> i32 {"
    LINE "    match s {"
    LINE "        \"a\": 1"
    LINE "        \"ab\": 2"
    LINE "        \"abc\": 3"
    LINE "    }"
    LINE "}"
    LINE "Foo = Position: {len(\"a\"), len(\"abc\")}";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    test_assert(foo != 0);
    const Position *p = ecs_get(world, foo, Position);
    test_assert(p != NULL);
    test_int(p->x, 1);
    test_int(p->y, 3);

    ecs_fini(world);
}

typedef struct FnStrings {
    char *a;
    char *b;
} FnStrings;

ECS_CTOR(FnStrings, ptr, {
    ptr->a = NULL;
    ptr->b = NULL;
})

ECS_DTOR(FnStrings, ptr, {
    ecs_os_free(ptr->a);
    ecs_os_free(ptr->b);
})

ECS_MOVE(FnStrings, dst, src, {
    ecs_os_free(dst->a);
    ecs_os_free(dst->b);
    dst->a = src->a;
    dst->b = src->b;
    src->a = NULL;
    src->b = NULL;
})

ECS_COPY(FnStrings, dst, src, {
    ecs_os_free(dst->a);
    ecs_os_free(dst->b);
    dst->a = ecs_os_strdup(src->a);
    dst->b = ecs_os_strdup(src->b);
})

void Function_string_return(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, FnStrings);

    ecs_set_hooks(world, FnStrings, {
        .ctor = ecs_ctor(FnStrings),
        .dtor = ecs_dtor(FnStrings),
        .move = ecs_move(FnStrings),
        .copy = ecs_copy(FnStrings)
    });

    ecs_struct(world, {
        .entity = ecs_id(FnStrings),
        .members = {
            {"a", ecs_id(ecs_string_t)},
            {"b", ecs_id(ecs_string_t)}
        }
    });

    const char *expr =
    HEAD "fn pick(i: i32) -> string {"
    LINE "    match i {"
    LINE "        0: \"zero\""
    LINE "        1: \"one\""
    LINE "    }"
    LINE "}"
    LINE "Foo = FnStrings: {pick(0), pick(1)}";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    test_assert(foo != 0);
    const FnStrings *p = ecs_get(world, foo, FnStrings);
    test_assert(p != NULL);
    test_str(p->a, "zero");
    test_str(p->b, "one");

    ecs_fini(world);
}

void Function_string_arg_and_return(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, FnStrings);

    ecs_set_hooks(world, FnStrings, {
        .ctor = ecs_ctor(FnStrings),
        .dtor = ecs_dtor(FnStrings),
        .move = ecs_move(FnStrings),
        .copy = ecs_copy(FnStrings)
    });

    ecs_struct(world, {
        .entity = ecs_id(FnStrings),
        .members = {
            {"a", ecs_id(ecs_string_t)},
            {"b", ecs_id(ecs_string_t)}
        }
    });

    const char *expr =
    HEAD "fn echo(s: string) -> string { s }"
    LINE "Foo = FnStrings: {echo(\"hello\"), echo(\"world\")}";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    test_assert(foo != 0);
    const FnStrings *p = ecs_get(world, foo, FnStrings);
    test_assert(p != NULL);
    test_str(p->a, "hello");
    test_str(p->b, "world");

    ecs_fini(world);
}

void Function_struct_arg(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Position" }),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    const char *expr =
    HEAD "fn sum(p: Position) -> f32 { p.x + p.y }"
    LINE "const a = Position: {3, 4}"
    LINE "Foo = Position: {sum($a), sum($a) * 2}";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    test_assert(foo != 0);
    const Position *p = ecs_get(world, foo, Position);
    test_assert(p != NULL);
    test_int(p->x, 7);
    test_int(p->y, 14);

    ecs_fini(world);
}

void Function_struct_return(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Position" }),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    const char *expr =
    HEAD "fn make(x: f32, y: f32) -> Position { {x, y} }"
    LINE "const r: make(10, 20)"
    LINE "Foo = Position: {$r.x, $r.y}";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    test_assert(foo != 0);
    const Position *p = ecs_get(world, foo, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void Function_struct_arg_and_return(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Position" }),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    const char *expr =
    HEAD "fn add_pos(a: Position, b: Position) -> Position {"
    LINE "    {a.x + b.x, a.y + b.y}"
    LINE "}"
    LINE "const a = Position: {1, 2}"
    LINE "const b = Position: {10, 20}"
    LINE "const r: add_pos($a, $b)"
    LINE "Foo = Position: {$r.x, $r.y}";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    test_assert(foo != 0);
    const Position *p = ecs_get(world, foo, Position);
    test_assert(p != NULL);
    test_int(p->x, 11);
    test_int(p->y, 22);

    ecs_fini(world);
}

void Function_recursion(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Position" }),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    const char *expr =
    HEAD "fn factorial(n: i32) -> i32 {"
    LINE "    match n {"
    LINE "        0: 1"
    LINE "        _: factorial(n - 1) * n"
    LINE "    }"
    LINE "}"
    LINE "Foo = Position: {factorial(5), factorial(6)}";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    test_assert(foo != 0);
    const Position *p = ecs_get(world, foo, Position);
    test_assert(p != NULL);
    test_int(p->x, 120);
    test_int(p->y, 720);

    ecs_fini(world);
}

void Function_undefined_var_in_body(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "fn bad(a: i32) -> i32 { a + nope }";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run(world, NULL, expr, NULL) != 0);

    ecs_fini(world);
}

void Function_runtime_error_propagation(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "fn pick(i: i32) -> i32 {"
    LINE "    match i {"
    LINE "        1: 10"
    LINE "    }"
    LINE "}"
    LINE "const r: pick(2)";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run(world, NULL, expr, NULL) != 0);

    ecs_fini(world);
}

void Function_match_expr(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Position" }),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    const char *expr =
    HEAD "fn classify(x: i32) -> i32 {"
    LINE "    match x {"
    LINE "        0: 100"
    LINE "        1: 200"
    LINE "        2: 300"
    LINE "    }"
    LINE "}"
    LINE "Foo = Position: {classify(0), classify(2)}";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    test_assert(foo != 0);
    const Position *p = ecs_get(world, foo, Position);
    test_assert(p != NULL);
    test_int(p->x, 100);
    test_int(p->y, 300);

    ecs_fini(world);
}

void Function_missing_arrow(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "fn add(a: i32, b: i32) i32 { a + b }";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run(world, NULL, expr, NULL) != 0);

    ecs_fini(world);
}

void Function_missing_return_type(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "fn add(a: i32, b: i32) -> { a + b }";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run(world, NULL, expr, NULL) != 0);

    ecs_fini(world);
}

void Function_missing_colon_in_param(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "fn add(a i32, b: i32) -> i32 { a + b }";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run(world, NULL, expr, NULL) != 0);

    ecs_fini(world);
}

void Function_missing_close_paren(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "fn add(a: i32, b: i32 -> i32 { a + b }";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run(world, NULL, expr, NULL) != 0);

    ecs_fini(world);
}

void Function_trailing_comma_in_params(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Position" }),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    const char *expr =
    HEAD "fn add(a: i32, b: i32,) -> i32 { a + b }"
    LINE "Foo = Position: {add(1, 2), add(3, 4)}";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    test_assert(foo != 0);
    const Position *p = ecs_get(world, foo, Position);
    test_assert(p != NULL);
    test_int(p->x, 3);
    test_int(p->y, 7);

    ecs_fini(world);
}

void Function_unclosed_body(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "fn add(a: i32) -> i32 { a";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run(world, NULL, expr, NULL) != 0);

    ecs_fini(world);
}

void Function_empty_body(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "fn add(a: i32) -> i32 { }";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run(world, NULL, expr, NULL) != 0);

    ecs_fini(world);
}

void Function_non_const_stmt_in_body(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "fn add(a: i32) -> i32 {"
    LINE "    Foo"
    LINE "    a"
    LINE "}";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run(world, NULL, expr, NULL) != 0);

    ecs_fini(world);
}

void Function_fn_as_identifier(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "fnord {}"
    LINE "myfn {}"
    LINE "fn_var {}";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    test_assert(ecs_lookup(world, "fnord") != 0);
    test_assert(ecs_lookup(world, "myfn") != 0);
    test_assert(ecs_lookup(world, "fn_var") != 0);

    ecs_fini(world);
}

void Function_fn_in_entity_scope(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Position" }),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    const char *expr =
    HEAD "Foo {"
    LINE "    fn add(a: i32, b: i32) -> i32 { a + b }"
    LINE "    InScope = Position: {add(1, 2), add(3, 4)}"
    LINE "}"
    LINE "OutScope = Position: {Foo.add(5, 6), Foo.add(7, 8)}";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t inscope = ecs_lookup(world, "Foo.InScope");
    test_assert(inscope != 0);
    const Position *p = ecs_get(world, inscope, Position);
    test_assert(p != NULL);
    test_int(p->x, 3);
    test_int(p->y, 7);

    ecs_entity_t outscope = ecs_lookup(world, "OutScope");
    test_assert(outscope != 0);
    p = ecs_get(world, outscope, Position);
    test_assert(p != NULL);
    test_int(p->x, 11);
    test_int(p->y, 15);

    ecs_fini(world);
}

void Function_fn_in_template(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "template Tmpl {"
    LINE "    fn add(a: i32, b: i32) -> i32 { a + b }"
    LINE "}";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run(world, NULL, expr, NULL) != 0);

    ecs_fini(world);
}

void Function_unresolved_param_type(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "fn f(a: NoSuchType) -> i32 { 1 }";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run(world, NULL, expr, NULL) != 0);

    ecs_fini(world);
}

void Function_unresolved_return_type(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "fn f(a: i32) -> NoSuchType { 1 }";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run(world, NULL, expr, NULL) != 0);

    ecs_fini(world);
}

void Function_duplicate_param(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "fn f(a: i32, a: i32) -> i32 { a }";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run(world, NULL, expr, NULL) != 0);

    ecs_fini(world);
}

void Function_const_missing_initializer(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "fn f(a: i32) -> i32 {"
    LINE "    const x: "
    LINE "    a"
    LINE "}";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run(world, NULL, expr, NULL) != 0);

    ecs_fini(world);
}

void Function_const_unresolved_type(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "fn f(a: i32) -> i32 {"
    LINE "    const x = NoSuchType: 1"
    LINE "    a + x"
    LINE "}";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run(world, NULL, expr, NULL) != 0);

    ecs_fini(world);
}

void Function_const_redeclared(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "fn f(a: i32) -> i32 {"
    LINE "    const x = i32: 1"
    LINE "    const x = i32: 2"
    LINE "    a + x"
    LINE "}";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run(world, NULL, expr, NULL) != 0);

    ecs_fini(world);
}

void Function_too_many_params(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "fn f("
    LINE "    a0: i32, a1: i32, a2: i32, a3: i32,"
    LINE "    a4: i32, a5: i32, a6: i32, a7: i32,"
    LINE "    a8: i32, a9: i32, a10: i32, a11: i32,"
    LINE "    a12: i32, a13: i32, a14: i32, a15: i32,"
    LINE "    a16: i32"
    LINE ") -> i32 { a0 }";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run(world, NULL, expr, NULL) != 0);

    ecs_fini(world);
}

void Function_too_few_args(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "fn add(a: i32, b: i32) -> i32 { a + b }"
    LINE "const r: add(1)";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run(world, NULL, expr, NULL) != 0);

    ecs_fini(world);
}

void Function_too_many_args(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "fn add(a: i32, b: i32) -> i32 { a + b }"
    LINE "const r: add(1, 2, 3)";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run(world, NULL, expr, NULL) != 0);

    ecs_fini(world);
}

void Function_wrong_arg_type(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "fn echo(s: string) -> string { s }"
    LINE "const r: echo(Foo)";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run(world, NULL, expr, NULL) != 0);

    ecs_fini(world);
}

void Function_undefined_fn(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "const r: nofn(1, 2)";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run(world, NULL, expr, NULL) != 0);

    ecs_fini(world);
}

void Function_forward_reference(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "const r: g(1)"
    LINE "fn g(a: i32) -> i32 { a + 1 }";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run(world, NULL, expr, NULL) != 0);

    ecs_fini(world);
}

void Function_fn_in_module(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Position" }),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    const char *expr =
    HEAD "module M"
    LINE "fn add(a: i32, b: i32) -> i32 { a + b }";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t add = ecs_lookup(world, "M.add");
    test_assert(add != 0);

    (void)ecs_id(Position);

    ecs_fini(world);
}

void Function_fn_in_module_qualified_call(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Position" }),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    const char *expr =
    HEAD "module M"
    LINE "fn add(a: i32, b: i32) -> i32 { a + b }";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    const char *call =
    HEAD "Foo = Position: {M.add(1, 2), M.add(10, 20)}";

    test_assert(ecs_script_run(world, NULL, call, NULL) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    test_assert(foo != 0);
    const Position *p = ecs_get(world, foo, Position);
    test_assert(p != NULL);
    test_int(p->x, 3);
    test_int(p->y, 30);

    ecs_fini(world);
}

void Function_fn_in_module_w_using(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Position" }),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    const char *expr =
    HEAD "module M"
    LINE "fn add(a: i32, b: i32) -> i32 { a + b }";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    const char *call =
    HEAD "using M"
    LINE "Foo = Position: {add(1, 2), add(10, 20)}";

    test_assert(ecs_script_run(world, NULL, call, NULL) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    test_assert(foo != 0);
    const Position *p = ecs_get(world, foo, Position);
    test_assert(p != NULL);
    test_int(p->x, 3);
    test_int(p->y, 30);

    ecs_fini(world);
}

void Function_fn_in_namespace_called_from_fn_w_using(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Position" }),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    const char *expr =
    HEAD "module foo"
    LINE "fn sum(a: i32, b: i32) -> i32 { a + b }";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    const char *call =
    HEAD "using foo"
    LINE "fn test(a: i32, b: i32) -> i32 { sum(a, b) }"
    LINE "Foo = Position: {test(1, 2), test(10, 20)}";

    test_assert(ecs_script_run(world, NULL, call, NULL) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    test_assert(foo != 0);
    const Position *p = ecs_get(world, foo, Position);
    test_assert(p != NULL);
    test_int(p->x, 3);
    test_int(p->y, 30);

    ecs_fini(world);
}

void Function_fn_in_nested_namespace_called_from_fn_w_using(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Position" }),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    const char *expr =
    HEAD "module foo.bar"
    LINE "fn sum(a: i32, b: i32) -> i32 { a + b }";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    const char *call =
    HEAD "using foo"
    LINE "fn test(a: i32, b: i32) -> i32 { bar.sum(a, b) }"
    LINE "Foo = Position: {test(1, 2), test(10, 20)}";

    test_assert(ecs_script_run(world, NULL, call, NULL) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    test_assert(foo != 0);
    const Position *p = ecs_get(world, foo, Position);
    test_assert(p != NULL);
    test_int(p->x, 3);
    test_int(p->y, 30);

    ecs_fini(world);
}

void Function_f32_arg_and_return(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Position" }),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    const char *expr =
    HEAD "fn half(a: f32) -> f32 { a / 2 }"
    LINE "Foo = Position: {half(10), half(3)}";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    test_assert(foo != 0);
    const Position *p = ecs_get(world, foo, Position);
    test_assert(p != NULL);
    test_flt(p->x, 5.0);
    test_flt(p->y, 1.5);

    ecs_fini(world);
}

void Function_f64_arg_and_return(void) {
    ecs_world_t *world = ecs_init();

    typedef struct { double x, y; } PositionD;
    ecs_entity_t ecs_id(PositionD) = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "PositionD" }),
        .members = {
            {"x", ecs_id(ecs_f64_t)},
            {"y", ecs_id(ecs_f64_t)}
        }
    });

    const char *expr =
    HEAD "fn dbl(a: f64) -> f64 { a + a }"
    LINE "Foo = PositionD: {dbl(1.5), dbl(2.25)}";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    test_assert(foo != 0);
    const PositionD *p = ecs_get_id(world, foo, ecs_id(PositionD));
    test_assert(p != NULL);
    test_flt(p->x, 3.0);
    test_flt(p->y, 4.5);

    ecs_fini(world);
}

void Function_bool_arg_and_return(void) {
    ecs_world_t *world = ecs_init();

    typedef struct { bool a, b; } BoolPair;
    ecs_entity_t ecs_id(BoolPair) = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "BoolPair" }),
        .members = {
            {"a", ecs_id(ecs_bool_t)},
            {"b", ecs_id(ecs_bool_t)}
        }
    });

    const char *expr =
    HEAD "fn is_zero(i: i32) -> bool { i == 0 }"
    LINE "Foo = BoolPair: {is_zero(0), is_zero(1)}";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    test_assert(foo != 0);
    const BoolPair *p = ecs_get_id(world, foo, ecs_id(BoolPair));
    test_assert(p != NULL);
    test_bool(p->a, true);
    test_bool(p->b, false);

    ecs_fini(world);
}

void Function_entity_arg_and_return(void) {
    ecs_world_t *world = ecs_init();

    typedef struct { ecs_entity_t a, b; } EntPair;
    ecs_entity_t ecs_id(EntPair) = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "EntPair" }),
        .members = {
            {"a", ecs_id(ecs_entity_t)},
            {"b", ecs_id(ecs_entity_t)}
        }
    });

    const char *expr =
    HEAD "Alpha {}"
    LINE "Beta {}"
    LINE "fn identity(e: entity) -> entity { e }"
    LINE "Foo = EntPair: {identity(Alpha), identity(Beta)}";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t alpha = ecs_lookup(world, "Alpha");
    ecs_entity_t beta = ecs_lookup(world, "Beta");
    test_assert(alpha != 0);
    test_assert(beta != 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    test_assert(foo != 0);
    const EntPair *p = ecs_get_id(world, foo, ecs_id(EntPair));
    test_assert(p != NULL);
    test_uint(p->a, alpha);
    test_uint(p->b, beta);

    ecs_fini(world);
}

void Function_enum_arg_and_return(void) {
    ecs_world_t *world = ecs_init();

    typedef enum { Red, Green, Blue } Color;
    ecs_enum(world, {
        .entity = ecs_entity(world, { .name = "Color" }),
        .constants = {
            {"Red"}, {"Green"}, {"Blue"}
        }
    });

    ecs_entity_t ecs_id(Position) = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Position" }),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    const char *expr =
    HEAD "fn to_int(c: Color) -> i32 {"
    LINE "    match c {"
    LINE "        Red: 100"
    LINE "        Green: 200"
    LINE "        Blue: 300"
    LINE "    }"
    LINE "}"
    LINE "Foo = Position: {to_int(Red), to_int(Blue)}";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    test_assert(foo != 0);
    const Position *p = ecs_get(world, foo, Position);
    test_assert(p != NULL);
    test_int(p->x, 100);
    test_int(p->y, 300);

    ecs_fini(world);
}

void Function_bitmask_arg_and_return(void) {
    ecs_world_t *world = ecs_init();

    ecs_bitmask(world, {
        .entity = ecs_entity(world, { .name = "Toppings" }),
        .constants = {
            {"Lettuce"}, {"Bacon"}, {"Tomato"}
        }
    });

    ecs_entity_t ecs_id(Position) = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Position" }),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    const char *expr =
    HEAD "fn to_int(t: Toppings) -> i32 { t + 0 }"
    LINE "Foo = Position: {to_int(Lettuce|Bacon), to_int(Tomato)}";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    test_assert(foo != 0);
    const Position *p = ecs_get(world, foo, Position);
    test_assert(p != NULL);
    test_int(p->x, 3);
    test_int(p->y, 4);

    ecs_fini(world);
}

void Function_mixed_int_arg_float_return(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Position" }),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    const char *expr =
    HEAD "fn to_f32(a: i32) -> f32 { a + 0.5 }"
    LINE "Foo = Position: {to_f32(1), to_f32(4)}";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    test_assert(foo != 0);
    const Position *p = ecs_get(world, foo, Position);
    test_assert(p != NULL);
    test_flt(p->x, 1.5);
    test_flt(p->y, 4.5);

    ecs_fini(world);
}

void Function_nested_struct_arg(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Point) = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Point" }),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    ecs_entity_t ecs_id(Line) = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Line" }),
        .members = {
            {"start", ecs_id(Point)},
            {"stop", ecs_id(Point)}
        }
    });

    const char *expr =
    HEAD "fn pass(l: Line) -> Line { l }"
    LINE "const v = Line: {start: {1, 2}, stop: {3, 4}}"
    LINE "const r: pass($v)"
    LINE "Foo = Line: {start: {$r.start.x, $r.start.y}, stop: {$r.stop.x, $r.stop.y}}";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    test_assert(foo != 0);
    const Line *l = ecs_get(world, foo, Line);
    test_assert(l != NULL);
    test_flt(l->start.x, 1);
    test_flt(l->start.y, 2);
    test_flt(l->stop.x, 3);
    test_flt(l->stop.y, 4);

    ecs_fini(world);
}

void Function_array_arg(void) {
    ecs_world_t *world = ecs_init();

    typedef struct { float values[3]; } Vec3F;
    ecs_entity_t ecs_id(Vec3F) = ecs_array(world, {
        .entity = ecs_entity(world, { .name = "Vec3F" }),
        .type = ecs_id(ecs_f32_t),
        .count = 3
    });

    const char *expr =
    HEAD "fn ident(a: Vec3F) -> Vec3F { a }"
    LINE "const v = Vec3F: [10, 20, 30]"
    LINE "const r: ident($v)"
    LINE "Foo { Vec3F: $r }";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    test_assert(foo != 0);
    const Vec3F *v = ecs_get_id(world, foo, ecs_id(Vec3F));
    test_assert(v != NULL);
    test_flt(v->values[0], 10);
    test_flt(v->values[1], 20);
    test_flt(v->values[2], 30);

    ecs_fini(world);
}

void Function_struct_with_string_return(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, FnStrings);

    ecs_set_hooks(world, FnStrings, {
        .ctor = ecs_ctor(FnStrings),
        .dtor = ecs_dtor(FnStrings),
        .move = ecs_move(FnStrings),
        .copy = ecs_copy(FnStrings)
    });

    ecs_struct(world, {
        .entity = ecs_id(FnStrings),
        .members = {
            {"a", ecs_id(ecs_string_t)},
            {"b", ecs_id(ecs_string_t)}
        }
    });

    const char *expr =
    HEAD "fn make() -> FnStrings { {a: \"hello\", b: \"world\"} }"
    LINE "const r: make()"
    LINE "Foo = FnStrings: {$r.a, $r.b}";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    test_assert(foo != 0);
    const FnStrings *p = ecs_get(world, foo, FnStrings);
    test_assert(p != NULL);
    test_str(p->a, "hello");
    test_str(p->b, "world");

    ecs_fini(world);
}

void Function_call_in_arithmetic(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Position" }),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    const char *expr =
    HEAD "fn five() -> i32 { 5 }"
    LINE "Foo = Position: {five() + 1, five() * 2}";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    test_assert(foo != 0);
    const Position *p = ecs_get(world, foo, Position);
    test_assert(p != NULL);
    test_int(p->x, 6);
    test_int(p->y, 10);

    ecs_fini(world);
}

void Function_call_in_match_value(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Position" }),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    const char *expr =
    HEAD "fn ten() -> i32 { 10 }"
    LINE "fn pick(i: i32) -> i32 {"
    LINE "    match i {"
    LINE "        0: ten()"
    LINE "        _: ten() * 2"
    LINE "    }"
    LINE "}"
    LINE "Foo = Position: {pick(0), pick(1)}";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    test_assert(foo != 0);
    const Position *p = ecs_get(world, foo, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void Function_call_in_if_condition(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Position" }),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    const char *expr =
    HEAD "fn truthy() -> bool { true }"
    LINE "if truthy() {"
    LINE "    Foo = Position: {1, 2}"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    test_assert(foo != 0);
    const Position *p = ecs_get(world, foo, Position);
    test_assert(p != NULL);
    test_int(p->x, 1);
    test_int(p->y, 2);

    ecs_fini(world);
}

void Function_call_in_for_body(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Position" }),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    const char *expr =
    HEAD "fn val(i: i32) -> i32 { i * 10 }"
    LINE "for i in 0..3 {"
    LINE "    \"e_{$i}\" { Position: {val(i), val(i) + 1} }"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t e0 = ecs_lookup(world, "e_0");
    ecs_entity_t e1 = ecs_lookup(world, "e_1");
    ecs_entity_t e2 = ecs_lookup(world, "e_2");
    test_assert(e0 != 0);
    test_assert(e1 != 0);
    test_assert(e2 != 0);

    const Position *p = ecs_get(world, e2, Position);
    test_assert(p != NULL);
    test_int(p->x, 20);
    test_int(p->y, 21);

    ecs_fini(world);
}

void Function_call_in_with(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Position" }),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    const char *expr =
    HEAD "fn make_pos() -> Position { {10, 20} }"
    LINE "const p: make_pos()"
    LINE "with $p {"
    LINE "    Foo {}"
    LINE "    Bar {}"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    ecs_entity_t bar = ecs_lookup(world, "Bar");
    test_assert(foo != 0);
    test_assert(bar != 0);

    const Position *fp = ecs_get(world, foo, Position);
    const Position *bp = ecs_get(world, bar, Position);
    test_assert(fp != NULL);
    test_assert(bp != NULL);
    test_int(fp->x, 10);
    test_int(fp->y, 20);
    test_int(bp->x, 10);
    test_int(bp->y, 20);

    ecs_fini(world);
}

void Function_call_in_const_rhs(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Position" }),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    const char *expr =
    HEAD "fn add(a: i32, b: i32) -> i32 { a + b }"
    LINE "const x: add(1, 2)"
    LINE "const y: add(10, 20)"
    LINE "Foo = Position: {$x, $y}";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    test_assert(foo != 0);
    const Position *p = ecs_get(world, foo, Position);
    test_assert(p != NULL);
    test_int(p->x, 3);
    test_int(p->y, 30);

    ecs_fini(world);
}

void Function_call_via_expr_run(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "fn add(a: i32, b: i32) -> i32 { a + b }";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_value_t v = {0};
    ecs_expr_eval_desc_t desc = {0};
    test_assert(ecs_expr_run(world, "add(2, 3)", &v, &desc) != NULL);
    test_assert(v.ptr != NULL);
    test_int(*(int32_t*)v.ptr, 5);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Function_to_str_roundtrip(void) {
    ecs_world_t *world = ecs_init();

    const char *src =
    "fn add(a: i32, b: i32) -> i32 { a + b }";

    ecs_script_t *script = ecs_script_parse(world, NULL, src, NULL, NULL);
    test_assert(script != NULL);

    char *str = ecs_script_ast_to_str(script, false);
    test_assert(str != NULL);
    test_assert(strstr(str, "fn") != NULL);
    test_assert(strstr(str, "add") != NULL);
    test_assert(strstr(str, "->") != NULL);
    test_assert(strstr(str, "i32") != NULL);

    ecs_os_free(str);
    ecs_script_free(script);

    ecs_fini(world);
}

void Function_arrow_no_spaces(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Position" }),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    const char *expr =
    HEAD "fn add(a: i32, b: i32)->i32 { a + b }"
    LINE "Foo = Position: {add(1, 2), add(3, 4)}";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    test_assert(foo != 0);
    const Position *p = ecs_get(world, foo, Position);
    test_assert(p != NULL);
    test_int(p->x, 3);
    test_int(p->y, 7);

    ecs_fini(world);
}

void Function_arrow_split_dash_gt(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "fn add(a: i32, b: i32) - > i32 { a + b }";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run(world, NULL, expr, NULL) != 0);

    ecs_fini(world);
}

void Function_nested_runtime_error(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "fn inner(i: i32) -> i32 {"
    LINE "    match i {"
    LINE "        1: 10"
    LINE "    }"
    LINE "}"
    LINE "fn outer(i: i32) -> i32 { inner(i) + 1 }"
    LINE "const r: outer(2)";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run(world, NULL, expr, NULL) != 0);

    ecs_fini(world);
}

void Function_const_runtime_error(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "fn fail(i: i32) -> i32 {"
    LINE "    match i {"
    LINE "        1: 10"
    LINE "    }"
    LINE "}"
    LINE "fn caller(i: i32) -> i32 {"
    LINE "    const x = i32: fail(i)"
    LINE "    x + 1"
    LINE "}"
    LINE "const r: caller(2)";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run(world, NULL, expr, NULL) != 0);

    ecs_fini(world);
}
