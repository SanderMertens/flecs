#include <script.h>

void ExprAst_binary_f32_var_add_f32_var(void) {
    ecs_world_t *world = ecs_init();

    ecs_script_vars_t *vars = ecs_script_vars_init(world);
    ecs_script_var_t *foo = ecs_script_vars_define(vars, "foo", ecs_f32_t);
    *(ecs_f32_t*)foo->value.ptr = 10;
    ecs_script_var_t *bar = ecs_script_vars_define(vars, "bar", ecs_f32_t);
    *(ecs_f32_t*)bar->value.ptr = 20;

    ecs_expr_eval_desc_t desc = { .vars = vars, .type = ecs_id(ecs_f32_t) };
    ecs_script_t *expr = ecs_expr_parse(world, "$foo + $bar", &desc);

    char *ast = ecs_script_ast_to_str(expr, false);
    test_str(ast, "($foo + $bar)");
    ecs_os_free(ast);

    float v;
    test_int(0, ecs_expr_eval(expr, &ecs_value_ptr(ecs_f32_t, &v), &desc));
    test_int(v, 30);

    ecs_script_vars_fini(vars);
    ecs_script_free(expr);

    ecs_fini(world);
}

void ExprAst_binary_f32_var_add_int(void) {
    ecs_world_t *world = ecs_init();

    ecs_script_vars_t *vars = ecs_script_vars_init(world);
    ecs_script_var_t *foo = ecs_script_vars_define(vars, "foo", ecs_f32_t);
    *(ecs_f32_t*)foo->value.ptr = 10;

    ecs_expr_eval_desc_t desc = { .vars = vars, .type = ecs_id(ecs_f32_t) };
    ecs_script_t *expr = ecs_expr_parse(world, "$foo + 3", &desc);

    char *ast = ecs_script_ast_to_str(expr, false);
    test_str(ast, "($foo + 3)");
    ecs_os_free(ast);

    float v;
    test_int(0, ecs_expr_eval(expr, &ecs_value_ptr(ecs_f32_t, &v), &desc));
    test_int(v, 13);

    ecs_script_vars_fini(vars);
    ecs_script_free(expr);

    ecs_fini(world);
}

void ExprAst_binary_f32_var_div_int(void) {
    ecs_world_t *world = ecs_init();

    ecs_script_vars_t *vars = ecs_script_vars_init(world);
    ecs_script_var_t *foo = ecs_script_vars_define(vars, "foo", ecs_f32_t);
    *(ecs_f32_t*)foo->value.ptr = 10;

    ecs_expr_eval_desc_t desc = { .vars = vars, .type = ecs_id(ecs_f32_t) };
    ecs_script_t *expr = ecs_expr_parse(world, "$foo / 2", &desc);

    char *ast = ecs_script_ast_to_str(expr, false);
    test_str(ast, "($foo / 2)");
    ecs_os_free(ast);

    float v;
    test_int(0, ecs_expr_eval(expr, &ecs_value_ptr(ecs_f32_t, &v), &desc));
    test_int(v, 5);

    ecs_script_vars_fini(vars);
    ecs_script_free(expr);

    ecs_fini(world);
}

void ExprAst_binary_f32_var_add_flt(void) {
    ecs_world_t *world = ecs_init();

    ecs_script_vars_t *vars = ecs_script_vars_init(world);
    ecs_script_var_t *foo = ecs_script_vars_define(vars, "foo", ecs_f32_t);
    *(ecs_f32_t*)foo->value.ptr = 10.5;

    ecs_expr_eval_desc_t desc = { .vars = vars, .type = ecs_id(ecs_f32_t) };
    ecs_script_t *expr = ecs_expr_parse(world, "$foo + 3.5", &desc);

    char *ast = ecs_script_ast_to_str(expr, false);
    test_str(ast, "($foo + 3.5)");
    ecs_os_free(ast);

    float v;
    test_int(0, ecs_expr_eval(expr, &ecs_value_ptr(ecs_f32_t, &v), &desc));
    test_int(v, 14);

    ecs_script_vars_fini(vars);
    ecs_script_free(expr);

    ecs_fini(world);
}

void ExprAst_binary_f32_var_div_by_int_sub_int(void) {
    ecs_world_t *world = ecs_init();

    ecs_script_vars_t *vars = ecs_script_vars_init(world);
    ecs_script_var_t *foo = ecs_script_vars_define(vars, "foo", ecs_f32_t);
    *(ecs_f32_t*)foo->value.ptr = 10;

    ecs_expr_eval_desc_t desc = { .vars = vars, .type = ecs_id(ecs_f32_t) };
    ecs_script_t *expr = ecs_expr_parse(world, "$foo / 2 - 4", &desc);

    char *ast = ecs_script_ast_to_str(expr, false);
    test_str(ast, "(($foo / 2) - 4)");
    ecs_os_free(ast);

    float v;
    test_int(0, ecs_expr_eval(expr, &ecs_value_ptr(ecs_f32_t, &v), &desc));
    test_int(v, 1);

    ecs_script_vars_fini(vars);
    ecs_script_free(expr);

    ecs_fini(world);
}

void ExprAst_interpolated_string_var(void) {
    ecs_world_t *world = ecs_init();

    ecs_script_vars_t *vars = ecs_script_vars_init(world);
    ecs_script_var_t *foo = ecs_script_vars_define(vars, "foo", ecs_i32_t);
    *(ecs_i32_t*)foo->value.ptr = 10;

    {
        ecs_expr_eval_desc_t desc = { .vars = vars };
        ecs_script_t *expr = ecs_expr_parse(world, "\"Hello $foo\"", &desc);

        char *ast = ecs_script_ast_to_str(expr, false);
        test_str(ast, "interpolated(\"Hello \", string($foo))");
        ecs_os_free(ast);

        char *v = NULL;
        test_int(0, ecs_expr_eval(expr, &ecs_value_ptr(ecs_string_t, &v), &desc));
        test_str(v, "Hello 10");
        ecs_os_free(v);

        ecs_script_free(expr);
    }

    {
        ecs_expr_eval_desc_t desc = { .vars = vars, .disable_folding = true };
        ecs_script_t *expr = ecs_expr_parse(world, "\"Hello $foo\"", &desc);

        char *ast = ecs_script_ast_to_str(expr, false);
        test_str(ast, "interpolated(\"Hello \", string($foo))");
        ecs_os_free(ast);

        char *v = NULL;
        test_int(0, ecs_expr_eval(expr, &ecs_value_ptr(ecs_string_t, &v), &desc));
        test_str(v, "Hello 10");
        ecs_os_free(v);

        ecs_script_free(expr);
    }

    ecs_script_vars_fini(vars);

    ecs_fini(world);
}

void ExprAst_interpolated_string_curly_brackets(void) {
    ecs_world_t *world = ecs_init();

    {
        ecs_script_t *expr = ecs_expr_parse(world, "\"Hello {10 + 20}\"", NULL);

        char *ast = ecs_script_ast_to_str(expr, false);
        test_str(ast, "Hello 30");
        ecs_os_free(ast);

        char *v = NULL;
        test_int(0, ecs_expr_eval(expr, &ecs_value_ptr(ecs_string_t, &v), NULL));
        test_str(v, "Hello 30");
        ecs_os_free(v);

        ecs_script_free(expr);
    }

    {
        ecs_expr_eval_desc_t desc = { .disable_folding = true };
        ecs_script_t *expr = ecs_expr_parse(world, "\"Hello {10 + 20}\"", &desc);

        char *ast = ecs_script_ast_to_str(expr, false);
        test_str(ast, "interpolated(\"Hello \", string((10 + 20)))");
        ecs_os_free(ast);

        char *v = NULL;
        test_int(0, ecs_expr_eval(expr, &ecs_value_ptr(ecs_string_t, &v), &desc));
        test_str(v, "Hello 30");
        ecs_os_free(v);

        ecs_script_free(expr);
    }

    ecs_fini(world);
}

void ExprAst_interpolated_string_curly_brackets_w_var(void) {
    ecs_world_t *world = ecs_init();

    ecs_script_vars_t *vars = ecs_script_vars_init(world);
    ecs_script_var_t *foo = ecs_script_vars_define(vars, "foo", ecs_i32_t);
    *(ecs_i32_t*)foo->value.ptr = 10;

    {
        ecs_expr_eval_desc_t desc = { .vars = vars };
        ecs_script_t *expr = ecs_expr_parse(world, "\"Hello {$foo + 10}\"", &desc);

        char *ast = ecs_script_ast_to_str(expr, false);
        test_str(ast, "interpolated(\"Hello \", string(($foo + 10)))");
        ecs_os_free(ast);

        char *v = NULL;
        test_int(0, ecs_expr_eval(expr, &ecs_value_ptr(ecs_string_t, &v), &desc));
        test_str(v, "Hello 20");
        ecs_os_free(v);

        ecs_script_free(expr);
    }

    {
        ecs_expr_eval_desc_t desc = { .vars = vars, .disable_folding = true };
        ecs_script_t *expr = ecs_expr_parse(world, "\"Hello {$foo + 10}\"", &desc);

        char *ast = ecs_script_ast_to_str(expr, false);
        test_str(ast, "interpolated(\"Hello \", string(($foo + i32(10))))");
        ecs_os_free(ast);

        char *v = NULL;
        test_int(0, ecs_expr_eval(expr, &ecs_value_ptr(ecs_string_t, &v), &desc));
        test_str(v, "Hello 20");
        ecs_os_free(v);

        ecs_script_free(expr);
    }

    ecs_script_vars_fini(vars);

    ecs_fini(world);
}

void ExprAst_template_w_foldable_const(void) {
    ecs_world_t *world = ecs_init();

    ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "Position"}),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    const char *expr =
    HEAD "Foo {}"
    LINE
    LINE "template Bar {"
    LINE "  const a: 10"
    LINE "  const b: $a + 10"
    LINE "  Position: {$a, $b}"
    LINE "}"
    LINE
    LINE "Bar e";

    const char *result_before =
    HEAD "{"
    LINE "  entity: Foo "
    LINE "  template: Bar {"
    LINE "    const: a = 10"
    LINE "    const: b = ($a + 10)"
    LINE "    component: Position: {$a, $b}"
    LINE "  }"
    LINE "  entity: Bar e "
    LINE "}\n";

    const char *result_after =
    HEAD "{"
    LINE "  entity: Foo "
    LINE "  template: Bar {"
    LINE "    const: a = 10"
    LINE "    const: b = 20"
    LINE "    component: Position: {x: 10, y: 20}"
    LINE "  }"
    LINE "  entity: Bar e "
    LINE "}\n";

    ecs_script_t *script = ecs_script_parse(world, NULL, expr, NULL, NULL);
    test_assert(script != NULL);
    {
        char *ast = ecs_script_ast_to_str(script, false);
        test_str(ast, result_before);
        ecs_os_free(ast);
    }

    test_assert(ecs_script_eval(script, NULL, NULL) == 0);
    {
        char *ast = ecs_script_ast_to_str(script, false);
        test_str(ast, result_after);
        ecs_os_free(ast);
    }

    ecs_script_free(script);

    ecs_fini(world);
}
