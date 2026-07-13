#include <script.h>

void Format_precision_f32_literal(void) {
    ecs_world_t *world = ecs_init();
    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *value = ecs_script_vars_define(
        vars, "value", ecs_f32_t);
    test_assert(value != NULL);
    *(ecs_f32_t*)value->value.ptr = 12.5f;

    const char *folding = test_param("folding");
    test_assert(folding != NULL);
    ecs_expr_eval_desc_t desc = {
        .vars = vars,
        .disable_folding = !strcmp(folding, "disabled")
    };

    char *result = NULL;
    test_assert(ecs_expr_run(world, "\"{value:.2}\"",
        &ecs_value_ptr(ecs_string_t, &result), &desc) != NULL);
    test_str(result, "12.50");
    ecs_os_free(result);

    ecs_script_vars_fini(vars);
    ecs_fini(world);
}

void Format_precision_f32_variable(void) {
    ecs_world_t *world = ecs_init();
    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *value = ecs_script_vars_define(
        vars, "value", ecs_f32_t);
    test_assert(value != NULL);
    *(ecs_f32_t*)value->value.ptr = 12.5f;

    ecs_script_var_t *precision = ecs_script_vars_define(
        vars, "precision", ecs_i32_t);
    test_assert(precision != NULL);
    *(ecs_i32_t*)precision->value.ptr = 2;

    const char *folding = test_param("folding");
    test_assert(folding != NULL);
    ecs_expr_eval_desc_t desc = {
        .vars = vars,
        .disable_folding = !strcmp(folding, "disabled")
    };

    char *result = NULL;
    test_assert(ecs_expr_run(world, "\"{value:.precision}\"",
        &ecs_value_ptr(ecs_string_t, &result), &desc) != NULL);
    test_str(result, "12.50");
    ecs_os_free(result);

    result = NULL;
    test_assert(ecs_expr_run(world, "\"{value:.$precision}\"",
        &ecs_value_ptr(ecs_string_t, &result), &desc) != NULL);
    test_str(result, "12.50");
    ecs_os_free(result);

    ecs_script_vars_fini(vars);
    ecs_fini(world);
}

void Format_precision_f32_expression(void) {
    ecs_world_t *world = ecs_init();
    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *value = ecs_script_vars_define(
        vars, "value", ecs_f32_t);
    test_assert(value != NULL);
    *(ecs_f32_t*)value->value.ptr = 12.5f;

    ecs_script_var_t *precision = ecs_script_vars_define(
        vars, "precision", ecs_i32_t);
    test_assert(precision != NULL);
    *(ecs_i32_t*)precision->value.ptr = 2;

    const char *folding = test_param("folding");
    test_assert(folding != NULL);
    ecs_expr_eval_desc_t desc = {
        .vars = vars,
        .disable_folding = !strcmp(folding, "disabled")
    };

    char *result = NULL;
    test_assert(ecs_expr_run(world, "\"{value:.(precision - 1 + 1)}\"",
        &ecs_value_ptr(ecs_string_t, &result), &desc) != NULL);
    test_str(result, "12.50");
    ecs_os_free(result);

    ecs_script_vars_fini(vars);
    ecs_fini(world);
}

void Format_precision_f64_literal(void) {
    ecs_world_t *world = ecs_init();
    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *value = ecs_script_vars_define(
        vars, "value", ecs_f64_t);
    test_assert(value != NULL);
    *(ecs_f64_t*)value->value.ptr = 12.5;

    const char *folding = test_param("folding");
    test_assert(folding != NULL);
    ecs_expr_eval_desc_t desc = {
        .vars = vars,
        .disable_folding = !strcmp(folding, "disabled")
    };

    char *result = NULL;
    test_assert(ecs_expr_run(world, "\"{value:.2}\"",
        &ecs_value_ptr(ecs_string_t, &result), &desc) != NULL);
    test_str(result, "12.50");
    ecs_os_free(result);

    ecs_script_vars_fini(vars);
    ecs_fini(world);
}

void Format_precision_f64_variable(void) {
    ecs_world_t *world = ecs_init();
    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *value = ecs_script_vars_define(
        vars, "value", ecs_f64_t);
    test_assert(value != NULL);
    *(ecs_f64_t*)value->value.ptr = 12.5;

    ecs_script_var_t *precision = ecs_script_vars_define(
        vars, "precision", ecs_i32_t);
    test_assert(precision != NULL);
    *(ecs_i32_t*)precision->value.ptr = 2;

    const char *folding = test_param("folding");
    test_assert(folding != NULL);
    ecs_expr_eval_desc_t desc = {
        .vars = vars,
        .disable_folding = !strcmp(folding, "disabled")
    };

    char *result = NULL;
    test_assert(ecs_expr_run(world, "\"{value:.precision}\"",
        &ecs_value_ptr(ecs_string_t, &result), &desc) != NULL);
    test_str(result, "12.50");
    ecs_os_free(result);

    result = NULL;
    test_assert(ecs_expr_run(world, "\"{value:.$precision}\"",
        &ecs_value_ptr(ecs_string_t, &result), &desc) != NULL);
    test_str(result, "12.50");
    ecs_os_free(result);

    ecs_script_vars_fini(vars);
    ecs_fini(world);
}

void Format_precision_f64_expression(void) {
    ecs_world_t *world = ecs_init();
    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *value = ecs_script_vars_define(
        vars, "value", ecs_f64_t);
    test_assert(value != NULL);
    *(ecs_f64_t*)value->value.ptr = 12.5;

    ecs_script_var_t *precision = ecs_script_vars_define(
        vars, "precision", ecs_i32_t);
    test_assert(precision != NULL);
    *(ecs_i32_t*)precision->value.ptr = 2;

    const char *folding = test_param("folding");
    test_assert(folding != NULL);
    ecs_expr_eval_desc_t desc = {
        .vars = vars,
        .disable_folding = !strcmp(folding, "disabled")
    };

    char *result = NULL;
    test_assert(ecs_expr_run(world, "\"{value:.(precision - 1 + 1)}\"",
        &ecs_value_ptr(ecs_string_t, &result), &desc) != NULL);
    test_str(result, "12.50");
    ecs_os_free(result);

    ecs_script_vars_fini(vars);
    ecs_fini(world);
}

void Format_min_width_f32_literal(void) {
    ecs_world_t *world = ecs_init();
    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *value = ecs_script_vars_define(
        vars, "value", ecs_f32_t);
    test_assert(value != NULL);
    *(ecs_f32_t*)value->value.ptr = 12.5f;

    const char *folding = test_param("folding");
    test_assert(folding != NULL);
    ecs_expr_eval_desc_t desc = {
        .vars = vars,
        .disable_folding = !strcmp(folding, "disabled")
    };

    char *result = NULL;
    test_assert(ecs_expr_run(world, "\"{value:13}\"",
        &ecs_value_ptr(ecs_string_t, &result), &desc) != NULL);
    test_str(result, "    12.500000");
    ecs_os_free(result);

    ecs_script_vars_fini(vars);
    ecs_fini(world);
}

void Format_min_width_f32_variable(void) {
    ecs_world_t *world = ecs_init();
    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *value = ecs_script_vars_define(
        vars, "value", ecs_f32_t);
    test_assert(value != NULL);
    *(ecs_f32_t*)value->value.ptr = 12.5f;

    ecs_script_var_t *width = ecs_script_vars_define(
        vars, "width", ecs_i32_t);
    test_assert(width != NULL);
    *(ecs_i32_t*)width->value.ptr = 13;

    const char *folding = test_param("folding");
    test_assert(folding != NULL);
    ecs_expr_eval_desc_t desc = {
        .vars = vars,
        .disable_folding = !strcmp(folding, "disabled")
    };

    char *result = NULL;
    test_assert(ecs_expr_run(world, "\"{value:width}\"",
        &ecs_value_ptr(ecs_string_t, &result), &desc) != NULL);
    test_str(result, "    12.500000");
    ecs_os_free(result);

    result = NULL;
    test_assert(ecs_expr_run(world, "\"{value:$width}\"",
        &ecs_value_ptr(ecs_string_t, &result), &desc) != NULL);
    test_str(result, "    12.500000");
    ecs_os_free(result);

    ecs_script_vars_fini(vars);
    ecs_fini(world);
}

void Format_min_width_f32_expression(void) {
    ecs_world_t *world = ecs_init();
    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *value = ecs_script_vars_define(
        vars, "value", ecs_f32_t);
    test_assert(value != NULL);
    *(ecs_f32_t*)value->value.ptr = 12.5f;

    ecs_script_var_t *width = ecs_script_vars_define(
        vars, "width", ecs_i32_t);
    test_assert(width != NULL);
    *(ecs_i32_t*)width->value.ptr = 13;

    const char *folding = test_param("folding");
    test_assert(folding != NULL);
    ecs_expr_eval_desc_t desc = {
        .vars = vars,
        .disable_folding = !strcmp(folding, "disabled")
    };

    char *result = NULL;
    test_assert(ecs_expr_run(world, "\"{value:(width - 2 + 2)}\"",
        &ecs_value_ptr(ecs_string_t, &result), &desc) != NULL);
    test_str(result, "    12.500000");
    ecs_os_free(result);

    ecs_script_vars_fini(vars);
    ecs_fini(world);
}

void Format_min_width_f64_literal(void) {
    ecs_world_t *world = ecs_init();
    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *value = ecs_script_vars_define(
        vars, "value", ecs_f64_t);
    test_assert(value != NULL);
    *(ecs_f64_t*)value->value.ptr = 12.5;

    const char *folding = test_param("folding");
    test_assert(folding != NULL);
    ecs_expr_eval_desc_t desc = {
        .vars = vars,
        .disable_folding = !strcmp(folding, "disabled")
    };

    char *result = NULL;
    test_assert(ecs_expr_run(world, "\"{value:13}\"",
        &ecs_value_ptr(ecs_string_t, &result), &desc) != NULL);
    test_str(result, "    12.500000");
    ecs_os_free(result);

    ecs_script_vars_fini(vars);
    ecs_fini(world);
}

void Format_min_width_f64_variable(void) {
    ecs_world_t *world = ecs_init();
    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *value = ecs_script_vars_define(
        vars, "value", ecs_f64_t);
    test_assert(value != NULL);
    *(ecs_f64_t*)value->value.ptr = 12.5;

    ecs_script_var_t *width = ecs_script_vars_define(
        vars, "width", ecs_i32_t);
    test_assert(width != NULL);
    *(ecs_i32_t*)width->value.ptr = 13;

    const char *folding = test_param("folding");
    test_assert(folding != NULL);
    ecs_expr_eval_desc_t desc = {
        .vars = vars,
        .disable_folding = !strcmp(folding, "disabled")
    };

    char *result = NULL;
    test_assert(ecs_expr_run(world, "\"{value:width}\"",
        &ecs_value_ptr(ecs_string_t, &result), &desc) != NULL);
    test_str(result, "    12.500000");
    ecs_os_free(result);

    result = NULL;
    test_assert(ecs_expr_run(world, "\"{value:$width}\"",
        &ecs_value_ptr(ecs_string_t, &result), &desc) != NULL);
    test_str(result, "    12.500000");
    ecs_os_free(result);

    ecs_script_vars_fini(vars);
    ecs_fini(world);
}

void Format_min_width_f64_expression(void) {
    ecs_world_t *world = ecs_init();
    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *value = ecs_script_vars_define(
        vars, "value", ecs_f64_t);
    test_assert(value != NULL);
    *(ecs_f64_t*)value->value.ptr = 12.5;

    ecs_script_var_t *width = ecs_script_vars_define(
        vars, "width", ecs_i32_t);
    test_assert(width != NULL);
    *(ecs_i32_t*)width->value.ptr = 13;

    const char *folding = test_param("folding");
    test_assert(folding != NULL);
    ecs_expr_eval_desc_t desc = {
        .vars = vars,
        .disable_folding = !strcmp(folding, "disabled")
    };

    char *result = NULL;
    test_assert(ecs_expr_run(world, "\"{value:(width - 2 + 2)}\"",
        &ecs_value_ptr(ecs_string_t, &result), &desc) != NULL);
    test_str(result, "    12.500000");
    ecs_os_free(result);

    ecs_script_vars_fini(vars);
    ecs_fini(world);
}

void Format_align_left_f32_literal(void) {
    ecs_world_t *world = ecs_init();
    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *value = ecs_script_vars_define(
        vars, "value", ecs_f32_t);
    test_assert(value != NULL);
    *(ecs_f32_t*)value->value.ptr = 12.5f;

    const char *folding = test_param("folding");
    test_assert(folding != NULL);
    ecs_expr_eval_desc_t desc = {
        .vars = vars,
        .disable_folding = !strcmp(folding, "disabled")
    };

    char *result = NULL;
    test_assert(ecs_expr_run(world, "\"{value:<13}\"",
        &ecs_value_ptr(ecs_string_t, &result), &desc) != NULL);
    test_str(result, "12.500000    ");
    ecs_os_free(result);

    ecs_script_vars_fini(vars);
    ecs_fini(world);
}

void Format_align_left_f32_variable(void) {
    ecs_world_t *world = ecs_init();
    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *value = ecs_script_vars_define(
        vars, "value", ecs_f32_t);
    test_assert(value != NULL);
    *(ecs_f32_t*)value->value.ptr = 12.5f;

    ecs_script_var_t *width = ecs_script_vars_define(
        vars, "width", ecs_i32_t);
    test_assert(width != NULL);
    *(ecs_i32_t*)width->value.ptr = 13;

    const char *folding = test_param("folding");
    test_assert(folding != NULL);
    ecs_expr_eval_desc_t desc = {
        .vars = vars,
        .disable_folding = !strcmp(folding, "disabled")
    };

    char *result = NULL;
    test_assert(ecs_expr_run(world, "\"{value:<width}\"",
        &ecs_value_ptr(ecs_string_t, &result), &desc) != NULL);
    test_str(result, "12.500000    ");
    ecs_os_free(result);

    result = NULL;
    test_assert(ecs_expr_run(world, "\"{value:<$width}\"",
        &ecs_value_ptr(ecs_string_t, &result), &desc) != NULL);
    test_str(result, "12.500000    ");
    ecs_os_free(result);

    ecs_script_vars_fini(vars);
    ecs_fini(world);
}

void Format_align_left_f32_expression(void) {
    ecs_world_t *world = ecs_init();
    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *value = ecs_script_vars_define(
        vars, "value", ecs_f32_t);
    test_assert(value != NULL);
    *(ecs_f32_t*)value->value.ptr = 12.5f;

    ecs_script_var_t *width = ecs_script_vars_define(
        vars, "width", ecs_i32_t);
    test_assert(width != NULL);
    *(ecs_i32_t*)width->value.ptr = 13;

    const char *folding = test_param("folding");
    test_assert(folding != NULL);
    ecs_expr_eval_desc_t desc = {
        .vars = vars,
        .disable_folding = !strcmp(folding, "disabled")
    };

    char *result = NULL;
    test_assert(ecs_expr_run(world, "\"{value:<(width - 2 + 2)}\"",
        &ecs_value_ptr(ecs_string_t, &result), &desc) != NULL);
    test_str(result, "12.500000    ");
    ecs_os_free(result);

    ecs_script_vars_fini(vars);
    ecs_fini(world);
}

void Format_align_left_f64_literal(void) {
    ecs_world_t *world = ecs_init();
    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *value = ecs_script_vars_define(
        vars, "value", ecs_f64_t);
    test_assert(value != NULL);
    *(ecs_f64_t*)value->value.ptr = 12.5;

    const char *folding = test_param("folding");
    test_assert(folding != NULL);
    ecs_expr_eval_desc_t desc = {
        .vars = vars,
        .disable_folding = !strcmp(folding, "disabled")
    };

    char *result = NULL;
    test_assert(ecs_expr_run(world, "\"{value:<13}\"",
        &ecs_value_ptr(ecs_string_t, &result), &desc) != NULL);
    test_str(result, "12.500000    ");
    ecs_os_free(result);

    ecs_script_vars_fini(vars);
    ecs_fini(world);
}

void Format_align_left_f64_variable(void) {
    ecs_world_t *world = ecs_init();
    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *value = ecs_script_vars_define(
        vars, "value", ecs_f64_t);
    test_assert(value != NULL);
    *(ecs_f64_t*)value->value.ptr = 12.5;

    ecs_script_var_t *width = ecs_script_vars_define(
        vars, "width", ecs_i32_t);
    test_assert(width != NULL);
    *(ecs_i32_t*)width->value.ptr = 13;

    const char *folding = test_param("folding");
    test_assert(folding != NULL);
    ecs_expr_eval_desc_t desc = {
        .vars = vars,
        .disable_folding = !strcmp(folding, "disabled")
    };

    char *result = NULL;
    test_assert(ecs_expr_run(world, "\"{value:<width}\"",
        &ecs_value_ptr(ecs_string_t, &result), &desc) != NULL);
    test_str(result, "12.500000    ");
    ecs_os_free(result);

    result = NULL;
    test_assert(ecs_expr_run(world, "\"{value:<$width}\"",
        &ecs_value_ptr(ecs_string_t, &result), &desc) != NULL);
    test_str(result, "12.500000    ");
    ecs_os_free(result);

    ecs_script_vars_fini(vars);
    ecs_fini(world);
}

void Format_align_left_f64_expression(void) {
    ecs_world_t *world = ecs_init();
    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *value = ecs_script_vars_define(
        vars, "value", ecs_f64_t);
    test_assert(value != NULL);
    *(ecs_f64_t*)value->value.ptr = 12.5;

    ecs_script_var_t *width = ecs_script_vars_define(
        vars, "width", ecs_i32_t);
    test_assert(width != NULL);
    *(ecs_i32_t*)width->value.ptr = 13;

    const char *folding = test_param("folding");
    test_assert(folding != NULL);
    ecs_expr_eval_desc_t desc = {
        .vars = vars,
        .disable_folding = !strcmp(folding, "disabled")
    };

    char *result = NULL;
    test_assert(ecs_expr_run(world, "\"{value:<(width - 2 + 2)}\"",
        &ecs_value_ptr(ecs_string_t, &result), &desc) != NULL);
    test_str(result, "12.500000    ");
    ecs_os_free(result);

    ecs_script_vars_fini(vars);
    ecs_fini(world);
}

void Format_align_center_f32_literal(void) {
    ecs_world_t *world = ecs_init();
    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *value = ecs_script_vars_define(
        vars, "value", ecs_f32_t);
    test_assert(value != NULL);
    *(ecs_f32_t*)value->value.ptr = 12.5f;

    const char *folding = test_param("folding");
    test_assert(folding != NULL);
    ecs_expr_eval_desc_t desc = {
        .vars = vars,
        .disable_folding = !strcmp(folding, "disabled")
    };

    char *result = NULL;
    test_assert(ecs_expr_run(world, "\"{value:^13}\"",
        &ecs_value_ptr(ecs_string_t, &result), &desc) != NULL);
    test_str(result, "  12.500000  ");
    ecs_os_free(result);

    ecs_script_vars_fini(vars);
    ecs_fini(world);
}

void Format_align_center_f32_variable(void) {
    ecs_world_t *world = ecs_init();
    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *value = ecs_script_vars_define(
        vars, "value", ecs_f32_t);
    test_assert(value != NULL);
    *(ecs_f32_t*)value->value.ptr = 12.5f;

    ecs_script_var_t *width = ecs_script_vars_define(
        vars, "width", ecs_i32_t);
    test_assert(width != NULL);
    *(ecs_i32_t*)width->value.ptr = 13;

    const char *folding = test_param("folding");
    test_assert(folding != NULL);
    ecs_expr_eval_desc_t desc = {
        .vars = vars,
        .disable_folding = !strcmp(folding, "disabled")
    };

    char *result = NULL;
    test_assert(ecs_expr_run(world, "\"{value:^width}\"",
        &ecs_value_ptr(ecs_string_t, &result), &desc) != NULL);
    test_str(result, "  12.500000  ");
    ecs_os_free(result);

    result = NULL;
    test_assert(ecs_expr_run(world, "\"{value:^$width}\"",
        &ecs_value_ptr(ecs_string_t, &result), &desc) != NULL);
    test_str(result, "  12.500000  ");
    ecs_os_free(result);

    ecs_script_vars_fini(vars);
    ecs_fini(world);
}

void Format_align_center_f32_expression(void) {
    ecs_world_t *world = ecs_init();
    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *value = ecs_script_vars_define(
        vars, "value", ecs_f32_t);
    test_assert(value != NULL);
    *(ecs_f32_t*)value->value.ptr = 12.5f;

    ecs_script_var_t *width = ecs_script_vars_define(
        vars, "width", ecs_i32_t);
    test_assert(width != NULL);
    *(ecs_i32_t*)width->value.ptr = 13;

    const char *folding = test_param("folding");
    test_assert(folding != NULL);
    ecs_expr_eval_desc_t desc = {
        .vars = vars,
        .disable_folding = !strcmp(folding, "disabled")
    };

    char *result = NULL;
    test_assert(ecs_expr_run(world, "\"{value:^(width - 2 + 2)}\"",
        &ecs_value_ptr(ecs_string_t, &result), &desc) != NULL);
    test_str(result, "  12.500000  ");
    ecs_os_free(result);

    ecs_script_vars_fini(vars);
    ecs_fini(world);
}

void Format_align_center_f64_literal(void) {
    ecs_world_t *world = ecs_init();
    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *value = ecs_script_vars_define(
        vars, "value", ecs_f64_t);
    test_assert(value != NULL);
    *(ecs_f64_t*)value->value.ptr = 12.5;

    const char *folding = test_param("folding");
    test_assert(folding != NULL);
    ecs_expr_eval_desc_t desc = {
        .vars = vars,
        .disable_folding = !strcmp(folding, "disabled")
    };

    char *result = NULL;
    test_assert(ecs_expr_run(world, "\"{value:^13}\"",
        &ecs_value_ptr(ecs_string_t, &result), &desc) != NULL);
    test_str(result, "  12.500000  ");
    ecs_os_free(result);

    ecs_script_vars_fini(vars);
    ecs_fini(world);
}

void Format_align_center_f64_variable(void) {
    ecs_world_t *world = ecs_init();
    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *value = ecs_script_vars_define(
        vars, "value", ecs_f64_t);
    test_assert(value != NULL);
    *(ecs_f64_t*)value->value.ptr = 12.5;

    ecs_script_var_t *width = ecs_script_vars_define(
        vars, "width", ecs_i32_t);
    test_assert(width != NULL);
    *(ecs_i32_t*)width->value.ptr = 13;

    const char *folding = test_param("folding");
    test_assert(folding != NULL);
    ecs_expr_eval_desc_t desc = {
        .vars = vars,
        .disable_folding = !strcmp(folding, "disabled")
    };

    char *result = NULL;
    test_assert(ecs_expr_run(world, "\"{value:^width}\"",
        &ecs_value_ptr(ecs_string_t, &result), &desc) != NULL);
    test_str(result, "  12.500000  ");
    ecs_os_free(result);

    result = NULL;
    test_assert(ecs_expr_run(world, "\"{value:^$width}\"",
        &ecs_value_ptr(ecs_string_t, &result), &desc) != NULL);
    test_str(result, "  12.500000  ");
    ecs_os_free(result);

    ecs_script_vars_fini(vars);
    ecs_fini(world);
}

void Format_align_center_f64_expression(void) {
    ecs_world_t *world = ecs_init();
    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *value = ecs_script_vars_define(
        vars, "value", ecs_f64_t);
    test_assert(value != NULL);
    *(ecs_f64_t*)value->value.ptr = 12.5;

    ecs_script_var_t *width = ecs_script_vars_define(
        vars, "width", ecs_i32_t);
    test_assert(width != NULL);
    *(ecs_i32_t*)width->value.ptr = 13;

    const char *folding = test_param("folding");
    test_assert(folding != NULL);
    ecs_expr_eval_desc_t desc = {
        .vars = vars,
        .disable_folding = !strcmp(folding, "disabled")
    };

    char *result = NULL;
    test_assert(ecs_expr_run(world, "\"{value:^(width - 2 + 2)}\"",
        &ecs_value_ptr(ecs_string_t, &result), &desc) != NULL);
    test_str(result, "  12.500000  ");
    ecs_os_free(result);

    ecs_script_vars_fini(vars);
    ecs_fini(world);
}

void Format_align_right_f32_literal(void) {
    ecs_world_t *world = ecs_init();
    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *value = ecs_script_vars_define(
        vars, "value", ecs_f32_t);
    test_assert(value != NULL);
    *(ecs_f32_t*)value->value.ptr = 12.5f;

    const char *folding = test_param("folding");
    test_assert(folding != NULL);
    ecs_expr_eval_desc_t desc = {
        .vars = vars,
        .disable_folding = !strcmp(folding, "disabled")
    };

    char *result = NULL;
    test_assert(ecs_expr_run(world, "\"{value:>13}\"",
        &ecs_value_ptr(ecs_string_t, &result), &desc) != NULL);
    test_str(result, "    12.500000");
    ecs_os_free(result);

    ecs_script_vars_fini(vars);
    ecs_fini(world);
}

void Format_align_right_f32_variable(void) {
    ecs_world_t *world = ecs_init();
    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *value = ecs_script_vars_define(
        vars, "value", ecs_f32_t);
    test_assert(value != NULL);
    *(ecs_f32_t*)value->value.ptr = 12.5f;

    ecs_script_var_t *width = ecs_script_vars_define(
        vars, "width", ecs_i32_t);
    test_assert(width != NULL);
    *(ecs_i32_t*)width->value.ptr = 13;

    const char *folding = test_param("folding");
    test_assert(folding != NULL);
    ecs_expr_eval_desc_t desc = {
        .vars = vars,
        .disable_folding = !strcmp(folding, "disabled")
    };

    char *result = NULL;
    test_assert(ecs_expr_run(world, "\"{value:>width}\"",
        &ecs_value_ptr(ecs_string_t, &result), &desc) != NULL);
    test_str(result, "    12.500000");
    ecs_os_free(result);

    result = NULL;
    test_assert(ecs_expr_run(world, "\"{value:>$width}\"",
        &ecs_value_ptr(ecs_string_t, &result), &desc) != NULL);
    test_str(result, "    12.500000");
    ecs_os_free(result);

    ecs_script_vars_fini(vars);
    ecs_fini(world);
}

void Format_align_right_f32_expression(void) {
    ecs_world_t *world = ecs_init();
    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *value = ecs_script_vars_define(
        vars, "value", ecs_f32_t);
    test_assert(value != NULL);
    *(ecs_f32_t*)value->value.ptr = 12.5f;

    ecs_script_var_t *width = ecs_script_vars_define(
        vars, "width", ecs_i32_t);
    test_assert(width != NULL);
    *(ecs_i32_t*)width->value.ptr = 13;

    const char *folding = test_param("folding");
    test_assert(folding != NULL);
    ecs_expr_eval_desc_t desc = {
        .vars = vars,
        .disable_folding = !strcmp(folding, "disabled")
    };

    char *result = NULL;
    test_assert(ecs_expr_run(world, "\"{value:>(width - 2 + 2)}\"",
        &ecs_value_ptr(ecs_string_t, &result), &desc) != NULL);
    test_str(result, "    12.500000");
    ecs_os_free(result);

    ecs_script_vars_fini(vars);
    ecs_fini(world);
}

void Format_align_right_f64_literal(void) {
    ecs_world_t *world = ecs_init();
    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *value = ecs_script_vars_define(
        vars, "value", ecs_f64_t);
    test_assert(value != NULL);
    *(ecs_f64_t*)value->value.ptr = 12.5;

    const char *folding = test_param("folding");
    test_assert(folding != NULL);
    ecs_expr_eval_desc_t desc = {
        .vars = vars,
        .disable_folding = !strcmp(folding, "disabled")
    };

    char *result = NULL;
    test_assert(ecs_expr_run(world, "\"{value:>13}\"",
        &ecs_value_ptr(ecs_string_t, &result), &desc) != NULL);
    test_str(result, "    12.500000");
    ecs_os_free(result);

    ecs_script_vars_fini(vars);
    ecs_fini(world);
}

void Format_align_right_f64_variable(void) {
    ecs_world_t *world = ecs_init();
    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *value = ecs_script_vars_define(
        vars, "value", ecs_f64_t);
    test_assert(value != NULL);
    *(ecs_f64_t*)value->value.ptr = 12.5;

    ecs_script_var_t *width = ecs_script_vars_define(
        vars, "width", ecs_i32_t);
    test_assert(width != NULL);
    *(ecs_i32_t*)width->value.ptr = 13;

    const char *folding = test_param("folding");
    test_assert(folding != NULL);
    ecs_expr_eval_desc_t desc = {
        .vars = vars,
        .disable_folding = !strcmp(folding, "disabled")
    };

    char *result = NULL;
    test_assert(ecs_expr_run(world, "\"{value:>width}\"",
        &ecs_value_ptr(ecs_string_t, &result), &desc) != NULL);
    test_str(result, "    12.500000");
    ecs_os_free(result);

    result = NULL;
    test_assert(ecs_expr_run(world, "\"{value:>$width}\"",
        &ecs_value_ptr(ecs_string_t, &result), &desc) != NULL);
    test_str(result, "    12.500000");
    ecs_os_free(result);

    ecs_script_vars_fini(vars);
    ecs_fini(world);
}

void Format_align_right_f64_expression(void) {
    ecs_world_t *world = ecs_init();
    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *value = ecs_script_vars_define(
        vars, "value", ecs_f64_t);
    test_assert(value != NULL);
    *(ecs_f64_t*)value->value.ptr = 12.5;

    ecs_script_var_t *width = ecs_script_vars_define(
        vars, "width", ecs_i32_t);
    test_assert(width != NULL);
    *(ecs_i32_t*)width->value.ptr = 13;

    const char *folding = test_param("folding");
    test_assert(folding != NULL);
    ecs_expr_eval_desc_t desc = {
        .vars = vars,
        .disable_folding = !strcmp(folding, "disabled")
    };

    char *result = NULL;
    test_assert(ecs_expr_run(world, "\"{value:>(width - 2 + 2)}\"",
        &ecs_value_ptr(ecs_string_t, &result), &desc) != NULL);
    test_str(result, "    12.500000");
    ecs_os_free(result);

    ecs_script_vars_fini(vars);
    ecs_fini(world);
}

void Format_fill_left_f32_literal(void) {
    ecs_world_t *world = ecs_init();
    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *value = ecs_script_vars_define(
        vars, "value", ecs_f32_t);
    test_assert(value != NULL);
    *(ecs_f32_t*)value->value.ptr = 12.5f;

    const char *folding = test_param("folding");
    test_assert(folding != NULL);
    ecs_expr_eval_desc_t desc = {
        .vars = vars,
        .disable_folding = !strcmp(folding, "disabled")
    };

    char *result = NULL;
    test_assert(ecs_expr_run(world, "\"{value:*<13}\"",
        &ecs_value_ptr(ecs_string_t, &result), &desc) != NULL);
    test_str(result, "12.500000****");
    ecs_os_free(result);

    ecs_script_vars_fini(vars);
    ecs_fini(world);
}

void Format_fill_left_f32_variable(void) {
    ecs_world_t *world = ecs_init();
    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *value = ecs_script_vars_define(
        vars, "value", ecs_f32_t);
    test_assert(value != NULL);
    *(ecs_f32_t*)value->value.ptr = 12.5f;

    ecs_script_var_t *width = ecs_script_vars_define(
        vars, "width", ecs_i32_t);
    test_assert(width != NULL);
    *(ecs_i32_t*)width->value.ptr = 13;

    const char *folding = test_param("folding");
    test_assert(folding != NULL);
    ecs_expr_eval_desc_t desc = {
        .vars = vars,
        .disable_folding = !strcmp(folding, "disabled")
    };

    char *result = NULL;
    test_assert(ecs_expr_run(world, "\"{value:*<width}\"",
        &ecs_value_ptr(ecs_string_t, &result), &desc) != NULL);
    test_str(result, "12.500000****");
    ecs_os_free(result);

    result = NULL;
    test_assert(ecs_expr_run(world, "\"{value:*<$width}\"",
        &ecs_value_ptr(ecs_string_t, &result), &desc) != NULL);
    test_str(result, "12.500000****");
    ecs_os_free(result);

    ecs_script_vars_fini(vars);
    ecs_fini(world);
}

void Format_fill_left_f32_expression(void) {
    ecs_world_t *world = ecs_init();
    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *value = ecs_script_vars_define(
        vars, "value", ecs_f32_t);
    test_assert(value != NULL);
    *(ecs_f32_t*)value->value.ptr = 12.5f;

    ecs_script_var_t *width = ecs_script_vars_define(
        vars, "width", ecs_i32_t);
    test_assert(width != NULL);
    *(ecs_i32_t*)width->value.ptr = 13;

    const char *folding = test_param("folding");
    test_assert(folding != NULL);
    ecs_expr_eval_desc_t desc = {
        .vars = vars,
        .disable_folding = !strcmp(folding, "disabled")
    };

    char *result = NULL;
    test_assert(ecs_expr_run(world, "\"{value:*<(width - 2 + 2)}\"",
        &ecs_value_ptr(ecs_string_t, &result), &desc) != NULL);
    test_str(result, "12.500000****");
    ecs_os_free(result);

    ecs_script_vars_fini(vars);
    ecs_fini(world);
}

void Format_fill_left_f64_literal(void) {
    ecs_world_t *world = ecs_init();
    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *value = ecs_script_vars_define(
        vars, "value", ecs_f64_t);
    test_assert(value != NULL);
    *(ecs_f64_t*)value->value.ptr = 12.5;

    const char *folding = test_param("folding");
    test_assert(folding != NULL);
    ecs_expr_eval_desc_t desc = {
        .vars = vars,
        .disable_folding = !strcmp(folding, "disabled")
    };

    char *result = NULL;
    test_assert(ecs_expr_run(world, "\"{value:*<13}\"",
        &ecs_value_ptr(ecs_string_t, &result), &desc) != NULL);
    test_str(result, "12.500000****");
    ecs_os_free(result);

    ecs_script_vars_fini(vars);
    ecs_fini(world);
}

void Format_fill_left_f64_variable(void) {
    ecs_world_t *world = ecs_init();
    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *value = ecs_script_vars_define(
        vars, "value", ecs_f64_t);
    test_assert(value != NULL);
    *(ecs_f64_t*)value->value.ptr = 12.5;

    ecs_script_var_t *width = ecs_script_vars_define(
        vars, "width", ecs_i32_t);
    test_assert(width != NULL);
    *(ecs_i32_t*)width->value.ptr = 13;

    const char *folding = test_param("folding");
    test_assert(folding != NULL);
    ecs_expr_eval_desc_t desc = {
        .vars = vars,
        .disable_folding = !strcmp(folding, "disabled")
    };

    char *result = NULL;
    test_assert(ecs_expr_run(world, "\"{value:*<width}\"",
        &ecs_value_ptr(ecs_string_t, &result), &desc) != NULL);
    test_str(result, "12.500000****");
    ecs_os_free(result);

    result = NULL;
    test_assert(ecs_expr_run(world, "\"{value:*<$width}\"",
        &ecs_value_ptr(ecs_string_t, &result), &desc) != NULL);
    test_str(result, "12.500000****");
    ecs_os_free(result);

    ecs_script_vars_fini(vars);
    ecs_fini(world);
}

void Format_fill_left_f64_expression(void) {
    ecs_world_t *world = ecs_init();
    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *value = ecs_script_vars_define(
        vars, "value", ecs_f64_t);
    test_assert(value != NULL);
    *(ecs_f64_t*)value->value.ptr = 12.5;

    ecs_script_var_t *width = ecs_script_vars_define(
        vars, "width", ecs_i32_t);
    test_assert(width != NULL);
    *(ecs_i32_t*)width->value.ptr = 13;

    const char *folding = test_param("folding");
    test_assert(folding != NULL);
    ecs_expr_eval_desc_t desc = {
        .vars = vars,
        .disable_folding = !strcmp(folding, "disabled")
    };

    char *result = NULL;
    test_assert(ecs_expr_run(world, "\"{value:*<(width - 2 + 2)}\"",
        &ecs_value_ptr(ecs_string_t, &result), &desc) != NULL);
    test_str(result, "12.500000****");
    ecs_os_free(result);

    ecs_script_vars_fini(vars);
    ecs_fini(world);
}

void Format_fill_center_f32_literal(void) {
    ecs_world_t *world = ecs_init();
    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *value = ecs_script_vars_define(
        vars, "value", ecs_f32_t);
    test_assert(value != NULL);
    *(ecs_f32_t*)value->value.ptr = 12.5f;

    const char *folding = test_param("folding");
    test_assert(folding != NULL);
    ecs_expr_eval_desc_t desc = {
        .vars = vars,
        .disable_folding = !strcmp(folding, "disabled")
    };

    char *result = NULL;
    test_assert(ecs_expr_run(world, "\"{value:*^13}\"",
        &ecs_value_ptr(ecs_string_t, &result), &desc) != NULL);
    test_str(result, "**12.500000**");
    ecs_os_free(result);

    ecs_script_vars_fini(vars);
    ecs_fini(world);
}

void Format_fill_center_f32_variable(void) {
    ecs_world_t *world = ecs_init();
    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *value = ecs_script_vars_define(
        vars, "value", ecs_f32_t);
    test_assert(value != NULL);
    *(ecs_f32_t*)value->value.ptr = 12.5f;

    ecs_script_var_t *width = ecs_script_vars_define(
        vars, "width", ecs_i32_t);
    test_assert(width != NULL);
    *(ecs_i32_t*)width->value.ptr = 13;

    const char *folding = test_param("folding");
    test_assert(folding != NULL);
    ecs_expr_eval_desc_t desc = {
        .vars = vars,
        .disable_folding = !strcmp(folding, "disabled")
    };

    char *result = NULL;
    test_assert(ecs_expr_run(world, "\"{value:*^width}\"",
        &ecs_value_ptr(ecs_string_t, &result), &desc) != NULL);
    test_str(result, "**12.500000**");
    ecs_os_free(result);

    result = NULL;
    test_assert(ecs_expr_run(world, "\"{value:*^$width}\"",
        &ecs_value_ptr(ecs_string_t, &result), &desc) != NULL);
    test_str(result, "**12.500000**");
    ecs_os_free(result);

    ecs_script_vars_fini(vars);
    ecs_fini(world);
}

void Format_fill_center_f32_expression(void) {
    ecs_world_t *world = ecs_init();
    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *value = ecs_script_vars_define(
        vars, "value", ecs_f32_t);
    test_assert(value != NULL);
    *(ecs_f32_t*)value->value.ptr = 12.5f;

    ecs_script_var_t *width = ecs_script_vars_define(
        vars, "width", ecs_i32_t);
    test_assert(width != NULL);
    *(ecs_i32_t*)width->value.ptr = 13;

    const char *folding = test_param("folding");
    test_assert(folding != NULL);
    ecs_expr_eval_desc_t desc = {
        .vars = vars,
        .disable_folding = !strcmp(folding, "disabled")
    };

    char *result = NULL;
    test_assert(ecs_expr_run(world, "\"{value:*^(width - 2 + 2)}\"",
        &ecs_value_ptr(ecs_string_t, &result), &desc) != NULL);
    test_str(result, "**12.500000**");
    ecs_os_free(result);

    ecs_script_vars_fini(vars);
    ecs_fini(world);
}

void Format_fill_center_f64_literal(void) {
    ecs_world_t *world = ecs_init();
    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *value = ecs_script_vars_define(
        vars, "value", ecs_f64_t);
    test_assert(value != NULL);
    *(ecs_f64_t*)value->value.ptr = 12.5;

    const char *folding = test_param("folding");
    test_assert(folding != NULL);
    ecs_expr_eval_desc_t desc = {
        .vars = vars,
        .disable_folding = !strcmp(folding, "disabled")
    };

    char *result = NULL;
    test_assert(ecs_expr_run(world, "\"{value:*^13}\"",
        &ecs_value_ptr(ecs_string_t, &result), &desc) != NULL);
    test_str(result, "**12.500000**");
    ecs_os_free(result);

    ecs_script_vars_fini(vars);
    ecs_fini(world);
}

void Format_fill_center_f64_variable(void) {
    ecs_world_t *world = ecs_init();
    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *value = ecs_script_vars_define(
        vars, "value", ecs_f64_t);
    test_assert(value != NULL);
    *(ecs_f64_t*)value->value.ptr = 12.5;

    ecs_script_var_t *width = ecs_script_vars_define(
        vars, "width", ecs_i32_t);
    test_assert(width != NULL);
    *(ecs_i32_t*)width->value.ptr = 13;

    const char *folding = test_param("folding");
    test_assert(folding != NULL);
    ecs_expr_eval_desc_t desc = {
        .vars = vars,
        .disable_folding = !strcmp(folding, "disabled")
    };

    char *result = NULL;
    test_assert(ecs_expr_run(world, "\"{value:*^width}\"",
        &ecs_value_ptr(ecs_string_t, &result), &desc) != NULL);
    test_str(result, "**12.500000**");
    ecs_os_free(result);

    result = NULL;
    test_assert(ecs_expr_run(world, "\"{value:*^$width}\"",
        &ecs_value_ptr(ecs_string_t, &result), &desc) != NULL);
    test_str(result, "**12.500000**");
    ecs_os_free(result);

    ecs_script_vars_fini(vars);
    ecs_fini(world);
}

void Format_fill_center_f64_expression(void) {
    ecs_world_t *world = ecs_init();
    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *value = ecs_script_vars_define(
        vars, "value", ecs_f64_t);
    test_assert(value != NULL);
    *(ecs_f64_t*)value->value.ptr = 12.5;

    ecs_script_var_t *width = ecs_script_vars_define(
        vars, "width", ecs_i32_t);
    test_assert(width != NULL);
    *(ecs_i32_t*)width->value.ptr = 13;

    const char *folding = test_param("folding");
    test_assert(folding != NULL);
    ecs_expr_eval_desc_t desc = {
        .vars = vars,
        .disable_folding = !strcmp(folding, "disabled")
    };

    char *result = NULL;
    test_assert(ecs_expr_run(world, "\"{value:*^(width - 2 + 2)}\"",
        &ecs_value_ptr(ecs_string_t, &result), &desc) != NULL);
    test_str(result, "**12.500000**");
    ecs_os_free(result);

    ecs_script_vars_fini(vars);
    ecs_fini(world);
}

void Format_fill_right_f32_literal(void) {
    ecs_world_t *world = ecs_init();
    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *value = ecs_script_vars_define(
        vars, "value", ecs_f32_t);
    test_assert(value != NULL);
    *(ecs_f32_t*)value->value.ptr = 12.5f;

    const char *folding = test_param("folding");
    test_assert(folding != NULL);
    ecs_expr_eval_desc_t desc = {
        .vars = vars,
        .disable_folding = !strcmp(folding, "disabled")
    };

    char *result = NULL;
    test_assert(ecs_expr_run(world, "\"{value:*>13}\"",
        &ecs_value_ptr(ecs_string_t, &result), &desc) != NULL);
    test_str(result, "****12.500000");
    ecs_os_free(result);

    ecs_script_vars_fini(vars);
    ecs_fini(world);
}

void Format_fill_right_f32_variable(void) {
    ecs_world_t *world = ecs_init();
    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *value = ecs_script_vars_define(
        vars, "value", ecs_f32_t);
    test_assert(value != NULL);
    *(ecs_f32_t*)value->value.ptr = 12.5f;

    ecs_script_var_t *width = ecs_script_vars_define(
        vars, "width", ecs_i32_t);
    test_assert(width != NULL);
    *(ecs_i32_t*)width->value.ptr = 13;

    const char *folding = test_param("folding");
    test_assert(folding != NULL);
    ecs_expr_eval_desc_t desc = {
        .vars = vars,
        .disable_folding = !strcmp(folding, "disabled")
    };

    char *result = NULL;
    test_assert(ecs_expr_run(world, "\"{value:*>width}\"",
        &ecs_value_ptr(ecs_string_t, &result), &desc) != NULL);
    test_str(result, "****12.500000");
    ecs_os_free(result);

    result = NULL;
    test_assert(ecs_expr_run(world, "\"{value:*>$width}\"",
        &ecs_value_ptr(ecs_string_t, &result), &desc) != NULL);
    test_str(result, "****12.500000");
    ecs_os_free(result);

    ecs_script_vars_fini(vars);
    ecs_fini(world);
}

void Format_fill_right_f32_expression(void) {
    ecs_world_t *world = ecs_init();
    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *value = ecs_script_vars_define(
        vars, "value", ecs_f32_t);
    test_assert(value != NULL);
    *(ecs_f32_t*)value->value.ptr = 12.5f;

    ecs_script_var_t *width = ecs_script_vars_define(
        vars, "width", ecs_i32_t);
    test_assert(width != NULL);
    *(ecs_i32_t*)width->value.ptr = 13;

    const char *folding = test_param("folding");
    test_assert(folding != NULL);
    ecs_expr_eval_desc_t desc = {
        .vars = vars,
        .disable_folding = !strcmp(folding, "disabled")
    };

    char *result = NULL;
    test_assert(ecs_expr_run(world, "\"{value:*>(width - 2 + 2)}\"",
        &ecs_value_ptr(ecs_string_t, &result), &desc) != NULL);
    test_str(result, "****12.500000");
    ecs_os_free(result);

    ecs_script_vars_fini(vars);
    ecs_fini(world);
}

void Format_fill_right_f64_literal(void) {
    ecs_world_t *world = ecs_init();
    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *value = ecs_script_vars_define(
        vars, "value", ecs_f64_t);
    test_assert(value != NULL);
    *(ecs_f64_t*)value->value.ptr = 12.5;

    const char *folding = test_param("folding");
    test_assert(folding != NULL);
    ecs_expr_eval_desc_t desc = {
        .vars = vars,
        .disable_folding = !strcmp(folding, "disabled")
    };

    char *result = NULL;
    test_assert(ecs_expr_run(world, "\"{value:*>13}\"",
        &ecs_value_ptr(ecs_string_t, &result), &desc) != NULL);
    test_str(result, "****12.500000");
    ecs_os_free(result);

    ecs_script_vars_fini(vars);
    ecs_fini(world);
}

void Format_fill_right_f64_variable(void) {
    ecs_world_t *world = ecs_init();
    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *value = ecs_script_vars_define(
        vars, "value", ecs_f64_t);
    test_assert(value != NULL);
    *(ecs_f64_t*)value->value.ptr = 12.5;

    ecs_script_var_t *width = ecs_script_vars_define(
        vars, "width", ecs_i32_t);
    test_assert(width != NULL);
    *(ecs_i32_t*)width->value.ptr = 13;

    const char *folding = test_param("folding");
    test_assert(folding != NULL);
    ecs_expr_eval_desc_t desc = {
        .vars = vars,
        .disable_folding = !strcmp(folding, "disabled")
    };

    char *result = NULL;
    test_assert(ecs_expr_run(world, "\"{value:*>width}\"",
        &ecs_value_ptr(ecs_string_t, &result), &desc) != NULL);
    test_str(result, "****12.500000");
    ecs_os_free(result);

    result = NULL;
    test_assert(ecs_expr_run(world, "\"{value:*>$width}\"",
        &ecs_value_ptr(ecs_string_t, &result), &desc) != NULL);
    test_str(result, "****12.500000");
    ecs_os_free(result);

    ecs_script_vars_fini(vars);
    ecs_fini(world);
}

void Format_fill_right_f64_expression(void) {
    ecs_world_t *world = ecs_init();
    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *value = ecs_script_vars_define(
        vars, "value", ecs_f64_t);
    test_assert(value != NULL);
    *(ecs_f64_t*)value->value.ptr = 12.5;

    ecs_script_var_t *width = ecs_script_vars_define(
        vars, "width", ecs_i32_t);
    test_assert(width != NULL);
    *(ecs_i32_t*)width->value.ptr = 13;

    const char *folding = test_param("folding");
    test_assert(folding != NULL);
    ecs_expr_eval_desc_t desc = {
        .vars = vars,
        .disable_folding = !strcmp(folding, "disabled")
    };

    char *result = NULL;
    test_assert(ecs_expr_run(world, "\"{value:*>(width - 2 + 2)}\"",
        &ecs_value_ptr(ecs_string_t, &result), &desc) != NULL);
    test_str(result, "****12.500000");
    ecs_os_free(result);

    ecs_script_vars_fini(vars);
    ecs_fini(world);
}

void Format_leading_zeros_f32_literal(void) {
    ecs_world_t *world = ecs_init();
    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *value = ecs_script_vars_define(
        vars, "value", ecs_f32_t);
    test_assert(value != NULL);
    *(ecs_f32_t*)value->value.ptr = 12.5f;

    const char *folding = test_param("folding");
    test_assert(folding != NULL);
    ecs_expr_eval_desc_t desc = {
        .vars = vars,
        .disable_folding = !strcmp(folding, "disabled")
    };

    char *result = NULL;
    test_assert(ecs_expr_run(world, "\"{value:013}\"",
        &ecs_value_ptr(ecs_string_t, &result), &desc) != NULL);
    test_str(result, "000012.500000");
    ecs_os_free(result);

    ecs_script_vars_fini(vars);
    ecs_fini(world);
}

void Format_leading_zeros_f32_variable(void) {
    ecs_world_t *world = ecs_init();
    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *value = ecs_script_vars_define(
        vars, "value", ecs_f32_t);
    test_assert(value != NULL);
    *(ecs_f32_t*)value->value.ptr = 12.5f;

    ecs_script_var_t *width = ecs_script_vars_define(
        vars, "width", ecs_i32_t);
    test_assert(width != NULL);
    *(ecs_i32_t*)width->value.ptr = 13;

    const char *folding = test_param("folding");
    test_assert(folding != NULL);
    ecs_expr_eval_desc_t desc = {
        .vars = vars,
        .disable_folding = !strcmp(folding, "disabled")
    };

    char *result = NULL;
    test_assert(ecs_expr_run(world, "\"{value:0width}\"",
        &ecs_value_ptr(ecs_string_t, &result), &desc) != NULL);
    test_str(result, "000012.500000");
    ecs_os_free(result);

    result = NULL;
    test_assert(ecs_expr_run(world, "\"{value:0$width}\"",
        &ecs_value_ptr(ecs_string_t, &result), &desc) != NULL);
    test_str(result, "000012.500000");
    ecs_os_free(result);

    ecs_script_vars_fini(vars);
    ecs_fini(world);
}

void Format_leading_zeros_f32_expression(void) {
    ecs_world_t *world = ecs_init();
    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *value = ecs_script_vars_define(
        vars, "value", ecs_f32_t);
    test_assert(value != NULL);
    *(ecs_f32_t*)value->value.ptr = 12.5f;

    ecs_script_var_t *width = ecs_script_vars_define(
        vars, "width", ecs_i32_t);
    test_assert(width != NULL);
    *(ecs_i32_t*)width->value.ptr = 13;

    const char *folding = test_param("folding");
    test_assert(folding != NULL);
    ecs_expr_eval_desc_t desc = {
        .vars = vars,
        .disable_folding = !strcmp(folding, "disabled")
    };

    char *result = NULL;
    test_assert(ecs_expr_run(world, "\"{value:0(width - 2 + 2)}\"",
        &ecs_value_ptr(ecs_string_t, &result), &desc) != NULL);
    test_str(result, "000012.500000");
    ecs_os_free(result);

    ecs_script_vars_fini(vars);
    ecs_fini(world);
}

void Format_leading_zeros_f64_literal(void) {
    ecs_world_t *world = ecs_init();
    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *value = ecs_script_vars_define(
        vars, "value", ecs_f64_t);
    test_assert(value != NULL);
    *(ecs_f64_t*)value->value.ptr = 12.5;

    const char *folding = test_param("folding");
    test_assert(folding != NULL);
    ecs_expr_eval_desc_t desc = {
        .vars = vars,
        .disable_folding = !strcmp(folding, "disabled")
    };

    char *result = NULL;
    test_assert(ecs_expr_run(world, "\"{value:013}\"",
        &ecs_value_ptr(ecs_string_t, &result), &desc) != NULL);
    test_str(result, "000012.500000");
    ecs_os_free(result);

    ecs_script_vars_fini(vars);
    ecs_fini(world);
}

void Format_leading_zeros_f64_variable(void) {
    ecs_world_t *world = ecs_init();
    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *value = ecs_script_vars_define(
        vars, "value", ecs_f64_t);
    test_assert(value != NULL);
    *(ecs_f64_t*)value->value.ptr = 12.5;

    ecs_script_var_t *width = ecs_script_vars_define(
        vars, "width", ecs_i32_t);
    test_assert(width != NULL);
    *(ecs_i32_t*)width->value.ptr = 13;

    const char *folding = test_param("folding");
    test_assert(folding != NULL);
    ecs_expr_eval_desc_t desc = {
        .vars = vars,
        .disable_folding = !strcmp(folding, "disabled")
    };

    char *result = NULL;
    test_assert(ecs_expr_run(world, "\"{value:0width}\"",
        &ecs_value_ptr(ecs_string_t, &result), &desc) != NULL);
    test_str(result, "000012.500000");
    ecs_os_free(result);

    result = NULL;
    test_assert(ecs_expr_run(world, "\"{value:0$width}\"",
        &ecs_value_ptr(ecs_string_t, &result), &desc) != NULL);
    test_str(result, "000012.500000");
    ecs_os_free(result);

    ecs_script_vars_fini(vars);
    ecs_fini(world);
}

void Format_leading_zeros_f64_expression(void) {
    ecs_world_t *world = ecs_init();
    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *value = ecs_script_vars_define(
        vars, "value", ecs_f64_t);
    test_assert(value != NULL);
    *(ecs_f64_t*)value->value.ptr = 12.5;

    ecs_script_var_t *width = ecs_script_vars_define(
        vars, "width", ecs_i32_t);
    test_assert(width != NULL);
    *(ecs_i32_t*)width->value.ptr = 13;

    const char *folding = test_param("folding");
    test_assert(folding != NULL);
    ecs_expr_eval_desc_t desc = {
        .vars = vars,
        .disable_folding = !strcmp(folding, "disabled")
    };

    char *result = NULL;
    test_assert(ecs_expr_run(world, "\"{value:0(width - 2 + 2)}\"",
        &ecs_value_ptr(ecs_string_t, &result), &desc) != NULL);
    test_str(result, "000012.500000");
    ecs_os_free(result);

    ecs_script_vars_fini(vars);
    ecs_fini(world);
}

void Format_always_sign_f32_literal(void) {
    ecs_world_t *world = ecs_init();
    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *value = ecs_script_vars_define(
        vars, "value", ecs_f32_t);
    test_assert(value != NULL);
    *(ecs_f32_t*)value->value.ptr = 12.5f;

    const char *folding = test_param("folding");
    test_assert(folding != NULL);
    ecs_expr_eval_desc_t desc = {
        .vars = vars,
        .disable_folding = !strcmp(folding, "disabled")
    };

    char *result = NULL;
    test_assert(ecs_expr_run(world, "\"{value:+13}\"",
        &ecs_value_ptr(ecs_string_t, &result), &desc) != NULL);
    test_str(result, "   +12.500000");
    ecs_os_free(result);

    ecs_script_vars_fini(vars);
    ecs_fini(world);
}

void Format_always_sign_f32_variable(void) {
    ecs_world_t *world = ecs_init();
    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *value = ecs_script_vars_define(
        vars, "value", ecs_f32_t);
    test_assert(value != NULL);
    *(ecs_f32_t*)value->value.ptr = 12.5f;

    ecs_script_var_t *width = ecs_script_vars_define(
        vars, "width", ecs_i32_t);
    test_assert(width != NULL);
    *(ecs_i32_t*)width->value.ptr = 13;

    const char *folding = test_param("folding");
    test_assert(folding != NULL);
    ecs_expr_eval_desc_t desc = {
        .vars = vars,
        .disable_folding = !strcmp(folding, "disabled")
    };

    char *result = NULL;
    test_assert(ecs_expr_run(world, "\"{value:+width}\"",
        &ecs_value_ptr(ecs_string_t, &result), &desc) != NULL);
    test_str(result, "   +12.500000");
    ecs_os_free(result);

    result = NULL;
    test_assert(ecs_expr_run(world, "\"{value:+$width}\"",
        &ecs_value_ptr(ecs_string_t, &result), &desc) != NULL);
    test_str(result, "   +12.500000");
    ecs_os_free(result);

    ecs_script_vars_fini(vars);
    ecs_fini(world);
}

void Format_always_sign_f32_expression(void) {
    ecs_world_t *world = ecs_init();
    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *value = ecs_script_vars_define(
        vars, "value", ecs_f32_t);
    test_assert(value != NULL);
    *(ecs_f32_t*)value->value.ptr = 12.5f;

    ecs_script_var_t *width = ecs_script_vars_define(
        vars, "width", ecs_i32_t);
    test_assert(width != NULL);
    *(ecs_i32_t*)width->value.ptr = 13;

    const char *folding = test_param("folding");
    test_assert(folding != NULL);
    ecs_expr_eval_desc_t desc = {
        .vars = vars,
        .disable_folding = !strcmp(folding, "disabled")
    };

    char *result = NULL;
    test_assert(ecs_expr_run(world, "\"{value:+(width - 2 + 2)}\"",
        &ecs_value_ptr(ecs_string_t, &result), &desc) != NULL);
    test_str(result, "   +12.500000");
    ecs_os_free(result);

    ecs_script_vars_fini(vars);
    ecs_fini(world);
}

void Format_always_sign_f64_literal(void) {
    ecs_world_t *world = ecs_init();
    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *value = ecs_script_vars_define(
        vars, "value", ecs_f64_t);
    test_assert(value != NULL);
    *(ecs_f64_t*)value->value.ptr = 12.5;

    const char *folding = test_param("folding");
    test_assert(folding != NULL);
    ecs_expr_eval_desc_t desc = {
        .vars = vars,
        .disable_folding = !strcmp(folding, "disabled")
    };

    char *result = NULL;
    test_assert(ecs_expr_run(world, "\"{value:+13}\"",
        &ecs_value_ptr(ecs_string_t, &result), &desc) != NULL);
    test_str(result, "   +12.500000");
    ecs_os_free(result);

    ecs_script_vars_fini(vars);
    ecs_fini(world);
}

void Format_always_sign_f64_variable(void) {
    ecs_world_t *world = ecs_init();
    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *value = ecs_script_vars_define(
        vars, "value", ecs_f64_t);
    test_assert(value != NULL);
    *(ecs_f64_t*)value->value.ptr = 12.5;

    ecs_script_var_t *width = ecs_script_vars_define(
        vars, "width", ecs_i32_t);
    test_assert(width != NULL);
    *(ecs_i32_t*)width->value.ptr = 13;

    const char *folding = test_param("folding");
    test_assert(folding != NULL);
    ecs_expr_eval_desc_t desc = {
        .vars = vars,
        .disable_folding = !strcmp(folding, "disabled")
    };

    char *result = NULL;
    test_assert(ecs_expr_run(world, "\"{value:+width}\"",
        &ecs_value_ptr(ecs_string_t, &result), &desc) != NULL);
    test_str(result, "   +12.500000");
    ecs_os_free(result);

    result = NULL;
    test_assert(ecs_expr_run(world, "\"{value:+$width}\"",
        &ecs_value_ptr(ecs_string_t, &result), &desc) != NULL);
    test_str(result, "   +12.500000");
    ecs_os_free(result);

    ecs_script_vars_fini(vars);
    ecs_fini(world);
}

void Format_always_sign_f64_expression(void) {
    ecs_world_t *world = ecs_init();
    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *value = ecs_script_vars_define(
        vars, "value", ecs_f64_t);
    test_assert(value != NULL);
    *(ecs_f64_t*)value->value.ptr = 12.5;

    ecs_script_var_t *width = ecs_script_vars_define(
        vars, "width", ecs_i32_t);
    test_assert(width != NULL);
    *(ecs_i32_t*)width->value.ptr = 13;

    const char *folding = test_param("folding");
    test_assert(folding != NULL);
    ecs_expr_eval_desc_t desc = {
        .vars = vars,
        .disable_folding = !strcmp(folding, "disabled")
    };

    char *result = NULL;
    test_assert(ecs_expr_run(world, "\"{value:+(width - 2 + 2)}\"",
        &ecs_value_ptr(ecs_string_t, &result), &desc) != NULL);
    test_str(result, "   +12.500000");
    ecs_os_free(result);

    ecs_script_vars_fini(vars);
    ecs_fini(world);
}

void Format_scientific_lower_f32_literal(void) {
    ecs_world_t *world = ecs_init();
    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *value = ecs_script_vars_define(
        vars, "value", ecs_f32_t);
    test_assert(value != NULL);
    *(ecs_f32_t*)value->value.ptr = 12.5f;

    const char *folding = test_param("folding");
    test_assert(folding != NULL);
    ecs_expr_eval_desc_t desc = {
        .vars = vars,
        .disable_folding = !strcmp(folding, "disabled")
    };

    char *result = NULL;
    test_assert(ecs_expr_run(world, "\"{value:.2e}\"",
        &ecs_value_ptr(ecs_string_t, &result), &desc) != NULL);
    test_str(result, "1.25e+01");
    ecs_os_free(result);

    ecs_script_vars_fini(vars);
    ecs_fini(world);
}

void Format_scientific_lower_f32_variable(void) {
    ecs_world_t *world = ecs_init();
    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *value = ecs_script_vars_define(
        vars, "value", ecs_f32_t);
    test_assert(value != NULL);
    *(ecs_f32_t*)value->value.ptr = 12.5f;

    ecs_script_var_t *precision = ecs_script_vars_define(
        vars, "precision", ecs_i32_t);
    test_assert(precision != NULL);
    *(ecs_i32_t*)precision->value.ptr = 2;

    const char *folding = test_param("folding");
    test_assert(folding != NULL);
    ecs_expr_eval_desc_t desc = {
        .vars = vars,
        .disable_folding = !strcmp(folding, "disabled")
    };

    char *result = NULL;
    test_assert(ecs_expr_run(world, "\"{value:.precisione}\"",
        &ecs_value_ptr(ecs_string_t, &result), &desc) != NULL);
    test_str(result, "1.25e+01");
    ecs_os_free(result);

    result = NULL;
    test_assert(ecs_expr_run(world, "\"{value:.$precisione}\"",
        &ecs_value_ptr(ecs_string_t, &result), &desc) != NULL);
    test_str(result, "1.25e+01");
    ecs_os_free(result);

    ecs_script_vars_fini(vars);
    ecs_fini(world);
}

void Format_scientific_lower_f32_expression(void) {
    ecs_world_t *world = ecs_init();
    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *value = ecs_script_vars_define(
        vars, "value", ecs_f32_t);
    test_assert(value != NULL);
    *(ecs_f32_t*)value->value.ptr = 12.5f;

    ecs_script_var_t *precision = ecs_script_vars_define(
        vars, "precision", ecs_i32_t);
    test_assert(precision != NULL);
    *(ecs_i32_t*)precision->value.ptr = 2;

    const char *folding = test_param("folding");
    test_assert(folding != NULL);
    ecs_expr_eval_desc_t desc = {
        .vars = vars,
        .disable_folding = !strcmp(folding, "disabled")
    };

    char *result = NULL;
    test_assert(ecs_expr_run(world, "\"{value:.(precision - 1 + 1)e}\"",
        &ecs_value_ptr(ecs_string_t, &result), &desc) != NULL);
    test_str(result, "1.25e+01");
    ecs_os_free(result);

    ecs_script_vars_fini(vars);
    ecs_fini(world);
}

void Format_scientific_lower_f64_literal(void) {
    ecs_world_t *world = ecs_init();
    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *value = ecs_script_vars_define(
        vars, "value", ecs_f64_t);
    test_assert(value != NULL);
    *(ecs_f64_t*)value->value.ptr = 12.5;

    const char *folding = test_param("folding");
    test_assert(folding != NULL);
    ecs_expr_eval_desc_t desc = {
        .vars = vars,
        .disable_folding = !strcmp(folding, "disabled")
    };

    char *result = NULL;
    test_assert(ecs_expr_run(world, "\"{value:.2e}\"",
        &ecs_value_ptr(ecs_string_t, &result), &desc) != NULL);
    test_str(result, "1.25e+01");
    ecs_os_free(result);

    ecs_script_vars_fini(vars);
    ecs_fini(world);
}

void Format_scientific_lower_f64_variable(void) {
    ecs_world_t *world = ecs_init();
    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *value = ecs_script_vars_define(
        vars, "value", ecs_f64_t);
    test_assert(value != NULL);
    *(ecs_f64_t*)value->value.ptr = 12.5;

    ecs_script_var_t *precision = ecs_script_vars_define(
        vars, "precision", ecs_i32_t);
    test_assert(precision != NULL);
    *(ecs_i32_t*)precision->value.ptr = 2;

    const char *folding = test_param("folding");
    test_assert(folding != NULL);
    ecs_expr_eval_desc_t desc = {
        .vars = vars,
        .disable_folding = !strcmp(folding, "disabled")
    };

    char *result = NULL;
    test_assert(ecs_expr_run(world, "\"{value:.precisione}\"",
        &ecs_value_ptr(ecs_string_t, &result), &desc) != NULL);
    test_str(result, "1.25e+01");
    ecs_os_free(result);

    result = NULL;
    test_assert(ecs_expr_run(world, "\"{value:.$precisione}\"",
        &ecs_value_ptr(ecs_string_t, &result), &desc) != NULL);
    test_str(result, "1.25e+01");
    ecs_os_free(result);

    ecs_script_vars_fini(vars);
    ecs_fini(world);
}

void Format_scientific_lower_f64_expression(void) {
    ecs_world_t *world = ecs_init();
    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *value = ecs_script_vars_define(
        vars, "value", ecs_f64_t);
    test_assert(value != NULL);
    *(ecs_f64_t*)value->value.ptr = 12.5;

    ecs_script_var_t *precision = ecs_script_vars_define(
        vars, "precision", ecs_i32_t);
    test_assert(precision != NULL);
    *(ecs_i32_t*)precision->value.ptr = 2;

    const char *folding = test_param("folding");
    test_assert(folding != NULL);
    ecs_expr_eval_desc_t desc = {
        .vars = vars,
        .disable_folding = !strcmp(folding, "disabled")
    };

    char *result = NULL;
    test_assert(ecs_expr_run(world, "\"{value:.(precision - 1 + 1)e}\"",
        &ecs_value_ptr(ecs_string_t, &result), &desc) != NULL);
    test_str(result, "1.25e+01");
    ecs_os_free(result);

    ecs_script_vars_fini(vars);
    ecs_fini(world);
}

void Format_scientific_upper_f32_literal(void) {
    ecs_world_t *world = ecs_init();
    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *value = ecs_script_vars_define(
        vars, "value", ecs_f32_t);
    test_assert(value != NULL);
    *(ecs_f32_t*)value->value.ptr = 12.5f;

    const char *folding = test_param("folding");
    test_assert(folding != NULL);
    ecs_expr_eval_desc_t desc = {
        .vars = vars,
        .disable_folding = !strcmp(folding, "disabled")
    };

    char *result = NULL;
    test_assert(ecs_expr_run(world, "\"{value:.2E}\"",
        &ecs_value_ptr(ecs_string_t, &result), &desc) != NULL);
    test_str(result, "1.25E+01");
    ecs_os_free(result);

    ecs_script_vars_fini(vars);
    ecs_fini(world);
}

void Format_scientific_upper_f32_variable(void) {
    ecs_world_t *world = ecs_init();
    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *value = ecs_script_vars_define(
        vars, "value", ecs_f32_t);
    test_assert(value != NULL);
    *(ecs_f32_t*)value->value.ptr = 12.5f;

    ecs_script_var_t *precision = ecs_script_vars_define(
        vars, "precision", ecs_i32_t);
    test_assert(precision != NULL);
    *(ecs_i32_t*)precision->value.ptr = 2;

    const char *folding = test_param("folding");
    test_assert(folding != NULL);
    ecs_expr_eval_desc_t desc = {
        .vars = vars,
        .disable_folding = !strcmp(folding, "disabled")
    };

    char *result = NULL;
    test_assert(ecs_expr_run(world, "\"{value:.precisionE}\"",
        &ecs_value_ptr(ecs_string_t, &result), &desc) != NULL);
    test_str(result, "1.25E+01");
    ecs_os_free(result);

    result = NULL;
    test_assert(ecs_expr_run(world, "\"{value:.$precisionE}\"",
        &ecs_value_ptr(ecs_string_t, &result), &desc) != NULL);
    test_str(result, "1.25E+01");
    ecs_os_free(result);

    ecs_script_vars_fini(vars);
    ecs_fini(world);
}

void Format_scientific_upper_f32_expression(void) {
    ecs_world_t *world = ecs_init();
    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *value = ecs_script_vars_define(
        vars, "value", ecs_f32_t);
    test_assert(value != NULL);
    *(ecs_f32_t*)value->value.ptr = 12.5f;

    ecs_script_var_t *precision = ecs_script_vars_define(
        vars, "precision", ecs_i32_t);
    test_assert(precision != NULL);
    *(ecs_i32_t*)precision->value.ptr = 2;

    const char *folding = test_param("folding");
    test_assert(folding != NULL);
    ecs_expr_eval_desc_t desc = {
        .vars = vars,
        .disable_folding = !strcmp(folding, "disabled")
    };

    char *result = NULL;
    test_assert(ecs_expr_run(world, "\"{value:.(precision - 1 + 1)E}\"",
        &ecs_value_ptr(ecs_string_t, &result), &desc) != NULL);
    test_str(result, "1.25E+01");
    ecs_os_free(result);

    ecs_script_vars_fini(vars);
    ecs_fini(world);
}

void Format_scientific_upper_f64_literal(void) {
    ecs_world_t *world = ecs_init();
    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *value = ecs_script_vars_define(
        vars, "value", ecs_f64_t);
    test_assert(value != NULL);
    *(ecs_f64_t*)value->value.ptr = 12.5;

    const char *folding = test_param("folding");
    test_assert(folding != NULL);
    ecs_expr_eval_desc_t desc = {
        .vars = vars,
        .disable_folding = !strcmp(folding, "disabled")
    };

    char *result = NULL;
    test_assert(ecs_expr_run(world, "\"{value:.2E}\"",
        &ecs_value_ptr(ecs_string_t, &result), &desc) != NULL);
    test_str(result, "1.25E+01");
    ecs_os_free(result);

    ecs_script_vars_fini(vars);
    ecs_fini(world);
}

void Format_scientific_upper_f64_variable(void) {
    ecs_world_t *world = ecs_init();
    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *value = ecs_script_vars_define(
        vars, "value", ecs_f64_t);
    test_assert(value != NULL);
    *(ecs_f64_t*)value->value.ptr = 12.5;

    ecs_script_var_t *precision = ecs_script_vars_define(
        vars, "precision", ecs_i32_t);
    test_assert(precision != NULL);
    *(ecs_i32_t*)precision->value.ptr = 2;

    const char *folding = test_param("folding");
    test_assert(folding != NULL);
    ecs_expr_eval_desc_t desc = {
        .vars = vars,
        .disable_folding = !strcmp(folding, "disabled")
    };

    char *result = NULL;
    test_assert(ecs_expr_run(world, "\"{value:.precisionE}\"",
        &ecs_value_ptr(ecs_string_t, &result), &desc) != NULL);
    test_str(result, "1.25E+01");
    ecs_os_free(result);

    result = NULL;
    test_assert(ecs_expr_run(world, "\"{value:.$precisionE}\"",
        &ecs_value_ptr(ecs_string_t, &result), &desc) != NULL);
    test_str(result, "1.25E+01");
    ecs_os_free(result);

    ecs_script_vars_fini(vars);
    ecs_fini(world);
}

void Format_scientific_upper_f64_expression(void) {
    ecs_world_t *world = ecs_init();
    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *value = ecs_script_vars_define(
        vars, "value", ecs_f64_t);
    test_assert(value != NULL);
    *(ecs_f64_t*)value->value.ptr = 12.5;

    ecs_script_var_t *precision = ecs_script_vars_define(
        vars, "precision", ecs_i32_t);
    test_assert(precision != NULL);
    *(ecs_i32_t*)precision->value.ptr = 2;

    const char *folding = test_param("folding");
    test_assert(folding != NULL);
    ecs_expr_eval_desc_t desc = {
        .vars = vars,
        .disable_folding = !strcmp(folding, "disabled")
    };

    char *result = NULL;
    test_assert(ecs_expr_run(world, "\"{value:.(precision - 1 + 1)E}\"",
        &ecs_value_ptr(ecs_string_t, &result), &desc) != NULL);
    test_str(result, "1.25E+01");
    ecs_os_free(result);

    ecs_script_vars_fini(vars);
    ecs_fini(world);
}

void Format_literal_value_precision(void) {
    ecs_world_t *world = ecs_init();

    const char *folding = test_param("folding");
    test_assert(folding != NULL);
    ecs_expr_eval_desc_t desc = {
        .disable_folding = !strcmp(folding, "disabled")
    };

    char *result = NULL;
    test_assert(ecs_expr_run(world, "\"{12.5:.2}\"",
        &ecs_value_ptr(ecs_string_t, &result), &desc) != NULL);
    test_str(result, "12.50");
    ecs_os_free(result);

    ecs_fini(world);
}

void Format_literal_value_min_width(void) {
    ecs_world_t *world = ecs_init();

    const char *folding = test_param("folding");
    test_assert(folding != NULL);
    ecs_expr_eval_desc_t desc = {
        .disable_folding = !strcmp(folding, "disabled")
    };

    char *result = NULL;
    test_assert(ecs_expr_run(world, "\"{12.5:13}\"",
        &ecs_value_ptr(ecs_string_t, &result), &desc) != NULL);
    test_str(result, "    12.500000");
    ecs_os_free(result);

    ecs_fini(world);
}

void Format_literal_value_align_left(void) {
    ecs_world_t *world = ecs_init();

    const char *folding = test_param("folding");
    test_assert(folding != NULL);
    ecs_expr_eval_desc_t desc = {
        .disable_folding = !strcmp(folding, "disabled")
    };

    char *result = NULL;
    test_assert(ecs_expr_run(world, "\"{12.5:<13}\"",
        &ecs_value_ptr(ecs_string_t, &result), &desc) != NULL);
    test_str(result, "12.500000    ");
    ecs_os_free(result);

    ecs_fini(world);
}

void Format_literal_value_align_center(void) {
    ecs_world_t *world = ecs_init();

    const char *folding = test_param("folding");
    test_assert(folding != NULL);
    ecs_expr_eval_desc_t desc = {
        .disable_folding = !strcmp(folding, "disabled")
    };

    char *result = NULL;
    test_assert(ecs_expr_run(world, "\"{12.5:^13}\"",
        &ecs_value_ptr(ecs_string_t, &result), &desc) != NULL);
    test_str(result, "  12.500000  ");
    ecs_os_free(result);

    ecs_fini(world);
}

void Format_literal_value_align_right(void) {
    ecs_world_t *world = ecs_init();

    const char *folding = test_param("folding");
    test_assert(folding != NULL);
    ecs_expr_eval_desc_t desc = {
        .disable_folding = !strcmp(folding, "disabled")
    };

    char *result = NULL;
    test_assert(ecs_expr_run(world, "\"{12.5:>13}\"",
        &ecs_value_ptr(ecs_string_t, &result), &desc) != NULL);
    test_str(result, "    12.500000");
    ecs_os_free(result);

    ecs_fini(world);
}

void Format_literal_value_fill_left(void) {
    ecs_world_t *world = ecs_init();

    const char *folding = test_param("folding");
    test_assert(folding != NULL);
    ecs_expr_eval_desc_t desc = {
        .disable_folding = !strcmp(folding, "disabled")
    };

    char *result = NULL;
    test_assert(ecs_expr_run(world, "\"{12.5:*<13}\"",
        &ecs_value_ptr(ecs_string_t, &result), &desc) != NULL);
    test_str(result, "12.500000****");
    ecs_os_free(result);

    ecs_fini(world);
}

void Format_literal_value_fill_center(void) {
    ecs_world_t *world = ecs_init();

    const char *folding = test_param("folding");
    test_assert(folding != NULL);
    ecs_expr_eval_desc_t desc = {
        .disable_folding = !strcmp(folding, "disabled")
    };

    char *result = NULL;
    test_assert(ecs_expr_run(world, "\"{12.5:*^13}\"",
        &ecs_value_ptr(ecs_string_t, &result), &desc) != NULL);
    test_str(result, "**12.500000**");
    ecs_os_free(result);

    ecs_fini(world);
}

void Format_literal_value_fill_right(void) {
    ecs_world_t *world = ecs_init();

    const char *folding = test_param("folding");
    test_assert(folding != NULL);
    ecs_expr_eval_desc_t desc = {
        .disable_folding = !strcmp(folding, "disabled")
    };

    char *result = NULL;
    test_assert(ecs_expr_run(world, "\"{12.5:*>13}\"",
        &ecs_value_ptr(ecs_string_t, &result), &desc) != NULL);
    test_str(result, "****12.500000");
    ecs_os_free(result);

    ecs_fini(world);
}

void Format_literal_value_leading_zeros(void) {
    ecs_world_t *world = ecs_init();

    const char *folding = test_param("folding");
    test_assert(folding != NULL);
    ecs_expr_eval_desc_t desc = {
        .disable_folding = !strcmp(folding, "disabled")
    };

    char *result = NULL;
    test_assert(ecs_expr_run(world, "\"{12.5:013}\"",
        &ecs_value_ptr(ecs_string_t, &result), &desc) != NULL);
    test_str(result, "000012.500000");
    ecs_os_free(result);

    ecs_fini(world);
}

void Format_literal_value_always_sign(void) {
    ecs_world_t *world = ecs_init();

    const char *folding = test_param("folding");
    test_assert(folding != NULL);
    ecs_expr_eval_desc_t desc = {
        .disable_folding = !strcmp(folding, "disabled")
    };

    char *result = NULL;
    test_assert(ecs_expr_run(world, "\"{12.5:+13}\"",
        &ecs_value_ptr(ecs_string_t, &result), &desc) != NULL);
    test_str(result, "   +12.500000");
    ecs_os_free(result);

    ecs_fini(world);
}

void Format_literal_value_scientific_lower(void) {
    ecs_world_t *world = ecs_init();

    const char *folding = test_param("folding");
    test_assert(folding != NULL);
    ecs_expr_eval_desc_t desc = {
        .disable_folding = !strcmp(folding, "disabled")
    };

    char *result = NULL;
    test_assert(ecs_expr_run(world, "\"{12.5:.2e}\"",
        &ecs_value_ptr(ecs_string_t, &result), &desc) != NULL);
    test_str(result, "1.25e+01");
    ecs_os_free(result);

    ecs_fini(world);
}

void Format_literal_value_scientific_upper(void) {
    ecs_world_t *world = ecs_init();

    const char *folding = test_param("folding");
    test_assert(folding != NULL);
    ecs_expr_eval_desc_t desc = {
        .disable_folding = !strcmp(folding, "disabled")
    };

    char *result = NULL;
    test_assert(ecs_expr_run(world, "\"{12.5:.2E}\"",
        &ecs_value_ptr(ecs_string_t, &result), &desc) != NULL);
    test_str(result, "1.25E+01");
    ecs_os_free(result);

    ecs_fini(world);
}

void Format_literal_value_combined_width_precision(void) {
    ecs_world_t *world = ecs_init();

    const char *folding = test_param("folding");
    test_assert(folding != NULL);
    ecs_expr_eval_desc_t desc = {
        .disable_folding = !strcmp(folding, "disabled")
    };

    char *result = NULL;
    test_assert(ecs_expr_run(world, "\"{12.5:13.2}\"",
        &ecs_value_ptr(ecs_string_t, &result), &desc) != NULL);
    test_str(result, "        12.50");
    ecs_os_free(result);

    ecs_fini(world);
}

void Format_fold_literal(void) {
    ecs_world_t *world = ecs_init();

    const char *folding = test_param("folding");
    test_assert(folding != NULL);
    bool disable_folding = !strcmp(folding, "disabled");
    ecs_expr_eval_desc_t desc = {
        .disable_folding = disable_folding
    };

    ecs_script_t *expr = ecs_expr_parse(world, "\"{12.5:.2}\"", &desc);
    test_assert(expr != NULL);

    char *ast = ecs_script_ast_to_str(expr, false);
    test_assert(ast != NULL);
    if (disable_folding) {
        test_assert(strstr(ast, "interpolated(") != NULL);
    } else {
        test_str(ast, "12.50");
    }
    ecs_os_free(ast);

    char *result = NULL;
    test_int(ecs_expr_eval(
        expr, &ecs_value_ptr(ecs_string_t, &result), &desc), 0);
    test_str(result, "12.50");
    ecs_os_free(result);

    ecs_script_free(expr);
    ecs_fini(world);
}

void Format_fold_const_expression(void) {
    ecs_world_t *world = ecs_init();
    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *value = ecs_script_vars_define(
        vars, "value", ecs_f64_t);
    test_assert(value != NULL);
    *(ecs_f64_t*)value->value.ptr = 12.5;
    value->is_const = true;

    ecs_script_var_t *precision = ecs_script_vars_define(
        vars, "precision", ecs_i32_t);
    test_assert(precision != NULL);
    *(ecs_i32_t*)precision->value.ptr = 2;
    precision->is_const = true;

    const char *folding = test_param("folding");
    test_assert(folding != NULL);
    bool disable_folding = !strcmp(folding, "disabled");
    ecs_expr_eval_desc_t desc = {
        .vars = vars,
        .disable_folding = disable_folding
    };

    ecs_script_t *expr = ecs_expr_parse(
        world, "\"{value:.(precision - 1 + 1)}\"", &desc);
    test_assert(expr != NULL);

    char *ast = ecs_script_ast_to_str(expr, false);
    test_assert(ast != NULL);
    if (disable_folding) {
        test_assert(strstr(ast, "interpolated(") != NULL);
    } else {
        test_str(ast, "12.50");
    }
    ecs_os_free(ast);

    char *result = NULL;
    test_int(ecs_expr_eval(
        expr, &ecs_value_ptr(ecs_string_t, &result), &desc), 0);
    test_str(result, "12.50");
    ecs_os_free(result);

    ecs_script_free(expr);
    ecs_script_vars_fini(vars);
    ecs_fini(world);
}

void Format_dont_fold_dynamic_expression(void) {
    ecs_world_t *world = ecs_init();
    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *value = ecs_script_vars_define(
        vars, "value", ecs_f64_t);
    test_assert(value != NULL);
    *(ecs_f64_t*)value->value.ptr = 12.5;

    ecs_script_var_t *precision = ecs_script_vars_define(
        vars, "precision", ecs_i32_t);
    test_assert(precision != NULL);
    *(ecs_i32_t*)precision->value.ptr = 2;

    const char *folding = test_param("folding");
    test_assert(folding != NULL);
    ecs_expr_eval_desc_t desc = {
        .vars = vars,
        .disable_folding = !strcmp(folding, "disabled")
    };

    ecs_script_t *expr = ecs_expr_parse(
        world, "\"{value:.(precision - 1 + 1)}\"", &desc);
    test_assert(expr != NULL);

    char *ast = ecs_script_ast_to_str(expr, false);
    test_assert(ast != NULL);
    test_assert(strstr(ast, "interpolated(") != NULL);
    ecs_os_free(ast);

    char *result = NULL;
    test_int(ecs_expr_eval(
        expr, &ecs_value_ptr(ecs_string_t, &result), &desc), 0);
    test_str(result, "12.50");
    ecs_os_free(result);

    ecs_script_free(expr);
    ecs_script_vars_fini(vars);
    ecs_fini(world);
}

void Format_reproducer_precision_variable_ending_in_e(void) {
    ecs_world_t *world = ecs_init();
    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *value = ecs_script_vars_define(
        vars, "value", ecs_f64_t);
    test_assert(value != NULL);
    *(ecs_f64_t*)value->value.ptr = 12.5;

    ecs_script_var_t *size = ecs_script_vars_define(
        vars, "size", ecs_i32_t);
    test_assert(size != NULL);
    *(ecs_i32_t*)size->value.ptr = 2;

    const char *folding = test_param("folding");
    test_assert(folding != NULL);
    ecs_expr_eval_desc_t desc = {
        .vars = vars,
        .disable_folding = !strcmp(folding, "disabled")
    };

    char *result = NULL;
    test_assert(ecs_expr_run(world, "\"{value:.size}\"",
        &ecs_value_ptr(ecs_string_t, &result), &desc) != NULL);
    test_str(result, "12.50");
    ecs_os_free(result);

    ecs_script_vars_fini(vars);
    ecs_fini(world);
}

void Format_reproducer_width_variable_named_e(void) {
    ecs_world_t *world = ecs_init();
    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *value = ecs_script_vars_define(
        vars, "value", ecs_f64_t);
    test_assert(value != NULL);
    *(ecs_f64_t*)value->value.ptr = 12.5;

    ecs_script_var_t *e = ecs_script_vars_define(vars, "e", ecs_i32_t);
    test_assert(e != NULL);
    *(ecs_i32_t*)e->value.ptr = 13;

    const char *folding = test_param("folding");
    test_assert(folding != NULL);
    ecs_expr_eval_desc_t desc = {
        .vars = vars,
        .disable_folding = !strcmp(folding, "disabled")
    };

    char *result = NULL;
    test_assert(ecs_expr_run(world, "\"{value:e}\"",
        &ecs_value_ptr(ecs_string_t, &result), &desc) != NULL);
    test_str(result, "    12.500000");
    ecs_os_free(result);

    ecs_script_vars_fini(vars);
    ecs_fini(world);
}
