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
