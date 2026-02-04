#include <script.h>

static ecs_query_cache_kind_t disable_folding = false;

void Expr_setup(void) {
    const char *folding_param = test_param("folding");
    if (folding_param) {
        if (!strcmp(folding_param, "disabled")) {
            disable_folding = true;
        } else if (!strcmp(folding_param, "enabled")) {
            // already set to default
        } else {
            printf("unexpected value for folding '%s'\n", folding_param);
        }
    }
}

void Expr_add_2_int_literals(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, "10 + 20", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_i64_t));
    test_assert(v.ptr != NULL);
    test_uint(*(uint64_t*)v.ptr, 10 + 20);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_add_2_int_literals_twice(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, "10 + 20", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_i64_t));
    test_assert(v.ptr != NULL);
    test_uint(*(uint64_t*)v.ptr, 10 + 20);

    test_assert(ecs_expr_run(world, "10 + 20", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_i64_t));
    test_assert(v.ptr != NULL);
    test_uint(*(uint64_t*)v.ptr, 10 + 20);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_sub_2_int_literals(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, "20 - 10", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_i64_t));
    test_assert(v.ptr != NULL);
    test_int(*(int64_t*)v.ptr, 20 - 10);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_mul_2_int_literals(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, "20 * 10", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_i64_t));
    test_assert(v.ptr != NULL);
    test_uint(*(ecs_u64_t*)v.ptr, 20 * 10);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_div_2_int_literals(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, "10 / 2", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_f64_t));
    test_assert(v.ptr != NULL);
    test_flt(*(ecs_f64_t*)v.ptr, 10 / 2);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_add_3_int_literals(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, "10 + 20 + 30", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_i64_t));
    test_assert(v.ptr != NULL);
    test_uint(*(uint64_t*)v.ptr, 10 + 20 + 30);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_add_3_int_literals_dec_hex_bin(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, "10 + 0xABCDEF1 + 0b1101", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_i64_t));
    test_assert(v.ptr != NULL);
    test_uint(*(uint64_t*)v.ptr, 10 + 0xABCDEF1 + 0b1101);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_add_3_int_literals_twice(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, "10 + 20 + 30", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_i64_t));
    test_assert(v.ptr != NULL);
    test_uint(*(uint64_t*)v.ptr, 10 + 20 + 30);

    test_assert(ecs_expr_run(world, "10 + 20 + 30", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_i64_t));
    test_assert(v.ptr != NULL);
    test_uint(*(uint64_t*)v.ptr, 10 + 20 + 30);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_sub_3_int_literals(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, "30 - 10 - 5", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_i64_t));
    test_assert(v.ptr != NULL);
    test_int(*(ecs_i64_t*)v.ptr, 30 - 10 - 5);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_mul_3_int_literals(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, "2 * 5 * 10", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_i64_t));
    test_assert(v.ptr != NULL);
    test_uint(*(ecs_i64_t*)v.ptr, 2 * 5 * 10);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_div_3_int_literals(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, "40 / 5 / 2", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_f64_t));
    test_assert(v.ptr != NULL);
    test_flt(*(ecs_f64_t*)v.ptr, 40 / 5 / 2);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_mod_2_int_literals(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, "10 % 3", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_i64_t));
    test_assert(v.ptr != NULL);
    test_flt(*(ecs_i64_t*)v.ptr, 1);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_mod_2_flt_literals(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, "10.5 % 3.5", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_i64_t));
    test_assert(v.ptr != NULL);
    test_flt(*(ecs_i64_t*)v.ptr, 1);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_add_no_space(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, "10+20", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_i64_t));
    test_assert(v.ptr != NULL);
    test_uint(*(uint64_t*)v.ptr, 10 + 20);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_sub_no_space(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, "20-10", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_i64_t));
    test_assert(v.ptr != NULL);
    test_uint(*(uint64_t*)v.ptr, 20 - 10);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_div_no_space(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, "20/2", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_f64_t));
    test_assert(v.ptr != NULL);
    test_uint(*(double*)v.ptr, 20 / 2);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_mul_no_space(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, "10*2", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_i64_t));
    test_assert(v.ptr != NULL);
    test_uint(*(uint64_t*)v.ptr, 10 * 2);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_add_no_space_var(void) {
    ecs_world_t *world = ecs_init();

    ecs_script_vars_t *vars = ecs_script_vars_init(world);
    ecs_script_var_t *var = ecs_script_vars_define(
        vars, "foo", ecs_i64_t);
    *(int32_t*)var->value.ptr = 20;

    ecs_expr_eval_desc_t desc = { .vars = vars, .disable_folding = disable_folding };

    ecs_value_t v = {0};
    test_assert(ecs_expr_run(world, "10+$foo", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_i64_t));
    test_assert(v.ptr != NULL);
    test_uint(*(int64_t*)v.ptr, 10 + 20);
    ecs_value_free(world, v.type, v.ptr);

    ecs_script_vars_fini(vars);

    ecs_fini(world);
}

void Expr_sub_no_space_var(void) {
    ecs_world_t *world = ecs_init();

    ecs_script_vars_t *vars = ecs_script_vars_init(world);
    ecs_script_var_t *var = ecs_script_vars_define(
        vars, "foo", ecs_i64_t);
    *(int32_t*)var->value.ptr = 10;

    ecs_expr_eval_desc_t desc = { .vars = vars, .disable_folding = disable_folding };

    ecs_value_t v = {0};
    test_assert(ecs_expr_run(world, "20-$foo", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_i64_t));
    test_assert(v.ptr != NULL);
    test_uint(*(int64_t*)v.ptr, 20 - 10);
    ecs_value_free(world, v.type, v.ptr);

    ecs_script_vars_fini(vars);

    ecs_fini(world);
}

void Expr_div_no_space_var(void) {
    ecs_world_t *world = ecs_init();

    ecs_script_vars_t *vars = ecs_script_vars_init(world);
    ecs_script_var_t *var = ecs_script_vars_define(
        vars, "foo", ecs_i64_t);
    *(int32_t*)var->value.ptr = 2;

    ecs_expr_eval_desc_t desc = { .vars = vars, .disable_folding = disable_folding };

    ecs_value_t v = {0};
    test_assert(ecs_expr_run(world, "20/$foo", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_f64_t));
    test_assert(v.ptr != NULL);
    test_uint(*(double*)v.ptr, 20 / 2);
    ecs_value_free(world, v.type, v.ptr);

    ecs_script_vars_fini(vars);

    ecs_fini(world);
}

void Expr_mul_no_space_var(void) {
    ecs_world_t *world = ecs_init();

    ecs_script_vars_t *vars = ecs_script_vars_init(world);
    ecs_script_var_t *var = ecs_script_vars_define(
        vars, "foo", ecs_i64_t);
    *(int32_t*)var->value.ptr = 2;

    ecs_expr_eval_desc_t desc = { .vars = vars, .disable_folding = disable_folding };

    ecs_value_t v = {0};
    test_assert(ecs_expr_run(world, "10*$foo", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_i64_t));
    test_assert(v.ptr != NULL);
    test_uint(*(int64_t*)v.ptr, 10 * 2);
    ecs_value_free(world, v.type, v.ptr);

    ecs_script_vars_fini(vars);

    ecs_fini(world);
}

void Expr_add_no_space_var_reverse(void) {
    ecs_world_t *world = ecs_init();

    ecs_script_vars_t *vars = ecs_script_vars_init(world);
    ecs_script_var_t *var = ecs_script_vars_define(
        vars, "foo", ecs_i64_t);
    *(int32_t*)var->value.ptr = 20;

    ecs_expr_eval_desc_t desc = { .vars = vars, .disable_folding = disable_folding };

    ecs_value_t v = {0};
    test_assert(ecs_expr_run(world, "$foo+10", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_i64_t));
    test_assert(v.ptr != NULL);
    test_uint(*(int64_t*)v.ptr, 10 + 20);
    ecs_value_free(world, v.type, v.ptr);

    ecs_script_vars_fini(vars);

    ecs_fini(world);
}

void Expr_sub_no_space_var_reverse(void) {
    ecs_world_t *world = ecs_init();

    ecs_script_vars_t *vars = ecs_script_vars_init(world);
    ecs_script_var_t *var = ecs_script_vars_define(
        vars, "foo", ecs_i64_t);
    *(int32_t*)var->value.ptr = 20;

    ecs_expr_eval_desc_t desc = { .vars = vars, .disable_folding = disable_folding };

    ecs_value_t v = {0};
    test_assert(ecs_expr_run(world, "$foo-10", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_i64_t));
    test_assert(v.ptr != NULL);
    test_uint(*(int64_t*)v.ptr, 20 - 10);
    ecs_value_free(world, v.type, v.ptr);

    ecs_script_vars_fini(vars);

    ecs_fini(world);
}

void Expr_div_no_space_var_reverse(void) {
    ecs_world_t *world = ecs_init();

    ecs_script_vars_t *vars = ecs_script_vars_init(world);
    ecs_script_var_t *var = ecs_script_vars_define(
        vars, "foo", ecs_i64_t);
    *(int32_t*)var->value.ptr = 20;

    ecs_expr_eval_desc_t desc = { .vars = vars, .disable_folding = disable_folding };

    ecs_value_t v = {0};
    test_assert(ecs_expr_run(world, "$foo/2", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_f64_t));
    test_assert(v.ptr != NULL);
    test_uint(*(double*)v.ptr, 20 / 2);
    ecs_value_free(world, v.type, v.ptr);

    ecs_script_vars_fini(vars);

    ecs_fini(world);
}

void Expr_mul_no_space_var_reverse(void) {
    ecs_world_t *world = ecs_init();

    ecs_script_vars_t *vars = ecs_script_vars_init(world);
    ecs_script_var_t *var = ecs_script_vars_define(
        vars, "foo", ecs_i64_t);
    *(int32_t*)var->value.ptr = 10;

    ecs_expr_eval_desc_t desc = { .vars = vars, .disable_folding = disable_folding };

    ecs_value_t v = {0};
    test_assert(ecs_expr_run(world, "$foo*2", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_i64_t));
    test_assert(v.ptr != NULL);
    test_uint(*(int64_t*)v.ptr, 10 * 2);
    ecs_value_free(world, v.type, v.ptr);

    ecs_script_vars_fini(vars);

    ecs_fini(world);
}

void Expr_div_by_0(void) {
    ecs_world_t *world = ecs_init();

    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    ecs_log_set_level(-4);
    test_assert(ecs_expr_parse(world, "10 / 0", &desc) == NULL);

    ecs_fini(world);
}

void Expr_div_by_0_var(void) {
    ecs_world_t *world = ecs_init();

    ecs_script_vars_t *vars = ecs_script_vars_init(world);
    ecs_script_var_t *var = ecs_script_vars_define(
        vars, "foo", ecs_i32_t);
    *(int32_t*)var->value.ptr = 0;

    ecs_expr_eval_desc_t desc = { .vars = vars, .disable_folding = disable_folding };

    ecs_value_t v = {0};
    ecs_log_set_level(-4);
    test_assert(ecs_expr_run(world, "10 / $foo", &v, &desc) == NULL);

    ecs_script_vars_fini(vars);

    ecs_fini(world);
}

void Expr_mod_by_0(void) {
    ecs_world_t *world = ecs_init();

    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    ecs_log_set_level(-4);
    test_assert(ecs_expr_parse(world, "10 % 0", &desc) == NULL);

    ecs_fini(world);
}

void Expr_mod_by_0_var(void) {
    ecs_world_t *world = ecs_init();

    ecs_script_vars_t *vars = ecs_script_vars_init(world);
    ecs_script_var_t *var = ecs_script_vars_define(
        vars, "foo", ecs_i32_t);
    *(int32_t*)var->value.ptr = 0;

    ecs_expr_eval_desc_t desc = { .vars = vars, .disable_folding = disable_folding };

    ecs_value_t v = {0};
    ecs_log_set_level(-4);
    test_assert(ecs_expr_run(world, "10 % $foo", &v, &desc) == NULL);

    ecs_script_vars_fini(vars);

    ecs_fini(world);
}

void Expr_int_to_bool(void) {
    ecs_world_t *world = ecs_init();

    bool b = false;
    ecs_value_t v = { .type = ecs_id(ecs_bool_t), .ptr = &b };
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, "10", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_flt(*(ecs_bool_t*)v.ptr, true);

    test_assert(ecs_expr_run(world, "0", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_flt(*(ecs_bool_t*)v.ptr, false);

    test_assert(ecs_expr_run(world, "256", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_flt(*(ecs_bool_t*)v.ptr, true);

    ecs_fini(world);
}

void Expr_bool_to_int(void) {
    ecs_world_t *world = ecs_init();

    int32_t i = 0;
    ecs_value_t v = { .type = ecs_id(ecs_i32_t), .ptr = &i };
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, "true", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_i32_t));
    test_assert(v.ptr != NULL);
    test_flt(*(ecs_i32_t*)v.ptr, 1);

    test_assert(ecs_expr_run(world, "false", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_i32_t));
    test_assert(v.ptr != NULL);
    test_flt(*(ecs_i32_t*)v.ptr, 0);

    ecs_fini(world);
}

void Expr_bool_to_uint(void) {
    ecs_world_t *world = ecs_init();

    uint32_t i = 0;
    ecs_value_t v = { .type = ecs_id(ecs_u32_t), .ptr = &i };
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, "true", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_u32_t));
    test_assert(v.ptr != NULL);
    test_flt(*(ecs_u32_t*)v.ptr, 1);

    test_assert(ecs_expr_run(world, "false", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_u32_t));
    test_assert(v.ptr != NULL);
    test_flt(*(ecs_u32_t*)v.ptr, 0);

    ecs_fini(world);
}

void Expr_char(void) {
    ecs_world_t *world = ecs_init();

    char c = 0;
    ecs_value_t v = { .type = ecs_id(ecs_char_t), .ptr = &c };
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, "'a'", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_char_t));
    test_assert(v.ptr != NULL);
    test_uint(*(ecs_char_t*)v.ptr, 'a');

    test_assert(ecs_expr_run(world, "'\\''", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_char_t));
    test_assert(v.ptr != NULL);
    test_uint(*(ecs_char_t*)v.ptr, '\'');

    test_assert(ecs_expr_run(world, "'\\\\'", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_char_t));
    test_assert(v.ptr != NULL);
    test_uint(*(ecs_char_t*)v.ptr, '\\');

    ecs_fini(world);
}

void Expr_char_to_int(void) {
    ecs_world_t *world = ecs_init();

    int32_t c = 0;
    ecs_value_t v = { .type = ecs_id(ecs_i32_t), .ptr = &c };
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, "'a'", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_i32_t));
    test_assert(v.ptr != NULL);
    test_uint(*(ecs_i32_t*)v.ptr, 'a');

    test_assert(ecs_expr_run(world, "'\\''", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_i32_t));
    test_assert(v.ptr != NULL);
    test_uint(*(ecs_i32_t*)v.ptr, '\'');

    test_assert(ecs_expr_run(world, "'\\\\'", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_i32_t));
    test_assert(v.ptr != NULL);
    test_uint(*(ecs_i32_t*)v.ptr, '\\');

    ecs_fini(world);
}

void Expr_add_mul_3_int_literals(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, "10 + 20 * 2", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_i64_t));
    test_assert(v.ptr != NULL);
    test_uint(*(uint64_t*)v.ptr, 10 + 20 * 2);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_sub_mul_3_int_literals(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, "50 - 10 * 2", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_i64_t));
    test_assert(v.ptr != NULL);
    test_int(*(int64_t*)v.ptr, 50 - 10 * 2);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_div_mul_3_int_literals(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, "10 / 5 * 2", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_f64_t));
    test_assert(v.ptr != NULL);
    test_flt(*(ecs_f64_t*)v.ptr, 10 / 5 * 2);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_add_div_3_int_literals(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, "10 + 30 / 2", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_f64_t));
    test_assert(v.ptr != NULL);
    test_uint(*(ecs_f64_t*)v.ptr, 10 + 30 / 2);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_sub_div_3_int_literals(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, "30 - 10 / 2", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_f64_t));
    test_assert(v.ptr != NULL);
    test_uint(*(ecs_f64_t*)v.ptr, 30 - 10 / 2);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_mul_div_3_int_literals(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, "20 * 10 / 2", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_f64_t));
    test_assert(v.ptr != NULL);
    test_uint(*(ecs_f64_t*)v.ptr, 20 * 10 / 2);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_mul_add_mul_add_int_literals(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, "2 * 4 + 6 * 8 + 10", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_i64_t));
    test_assert(v.ptr != NULL);
    test_uint(*(ecs_i64_t*)v.ptr, 2 * 4 + 6 * 8 + 10);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_mul_sub_mul_sub_int_literals(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, "2 * 4 - 6 * 8 - 10", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_i64_t));
    test_assert(v.ptr != NULL);
    test_int(*(ecs_i64_t*)v.ptr, 2 * 4 - 6 * 8 - 10);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_mul_div_mul_div_int_literals(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, "2 * 4 / 6 * 8 / 10", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_f64_t));
    test_assert(v.ptr != NULL);
    test_flt(*(ecs_f64_t*)v.ptr, 2.0 * 4.0 / 6.0 * 8.0 / 10.0);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_div_add_div_add_int_literals(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, "2 / 4 + 6 / 8 + 10", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_f64_t));
    test_assert(v.ptr != NULL);
    test_flt(*(ecs_f64_t*)v.ptr, 2.0 / 4.0 + 6.0 / 8.0 + 10.0);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_div_sub_div_sub_int_literals(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, "2 / 4 - 6 / 8 - 10", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_f64_t));
    test_assert(v.ptr != NULL);
    test_flt(*(ecs_f64_t*)v.ptr, 2.0 / 4.0 - 6.0 / 8.0 - 10.0);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_div_mul_div_mul_int_literals(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, "2 / 4 * 6 / 8 * 10", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_f64_t));
    test_assert(v.ptr != NULL);
    test_flt(*(ecs_f64_t*)v.ptr, 2.0 / 4.0 * 6.0 / 8.0 * 10.0);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_div_sub_div_mul_int_literals(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, "2 / 4 - 6 / 8 * 10", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_f64_t));
    test_assert(v.ptr != NULL);
    test_flt(*(ecs_f64_t*)v.ptr, 2.0 / 4.0 - 6.0 / 8.0 * 10.0);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_add_2_flt_literals(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, "10.5 + 20.0", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_f64_t));
    test_assert(v.ptr != NULL);
    test_flt(*(ecs_f64_t*)v.ptr, 10.5 + 20.0);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_sub_2_flt_literals(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, "20.5 - 10.0", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_f64_t));
    test_assert(v.ptr != NULL);
    test_flt(*(ecs_f64_t*)v.ptr, 20.5 - 10.0);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_mul_2_flt_literals(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, "20.5 * 10.0", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_f64_t));
    test_assert(v.ptr != NULL);
    test_flt(*(ecs_f64_t*)v.ptr, 20.5 * 10.0);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_div_2_flt_literals(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, "10.5 / 2.0", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_f64_t));
    test_assert(v.ptr != NULL);
    test_flt(*(ecs_f64_t*)v.ptr, 10.5 / 2.0);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_add_2_int_neg_literals(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, "-10 + -20", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_i64_t));
    test_assert(v.ptr != NULL);
    test_int(*(ecs_i64_t*)v.ptr, -10 + -20);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_sub_2_int_neg_literals(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, "-10 - -20", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_i64_t));
    test_assert(v.ptr != NULL);
    test_int(*(ecs_i64_t*)v.ptr, -10 - -20);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_mul_2_int_neg_literals(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, "-10 * -20", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_i64_t));
    test_assert(v.ptr != NULL);
    test_int(*(ecs_i64_t*)v.ptr, -10 * -20);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_div_2_int_neg_literals(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, "-10 / -20", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_f64_t));
    test_assert(v.ptr != NULL);
    test_flt(*(ecs_f64_t*)v.ptr, -10.0 / -20.0);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_mul_lparen_add_add_rparen_int_literals(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, "10 * (20 + 30)", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_i64_t));
    test_assert(v.ptr != NULL);
    test_uint(*(ecs_i64_t*)v.ptr, 10 * (20 + 30));
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_mul_lparen_add_add_add_rparen_int_literals(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, "10 * (20 + 30 + 40)", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_i64_t));
    test_assert(v.ptr != NULL);
    test_uint(*(ecs_i64_t*)v.ptr, 10 * (20 + 30 + 40));
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_mul_lparen_add_add_rparen_add_int_literals(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, "10 * (20 + 30) + 40", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_i64_t));
    test_assert(v.ptr != NULL);
    test_uint(*(ecs_i64_t*)v.ptr, 10 * (20 + 30) + 40);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_lparen_add_add_rparen_mul_int_literals(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, "(20 + 30) * 10", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_i64_t));
    test_assert(v.ptr != NULL);
    test_uint(*(ecs_i64_t*)v.ptr, (20 + 30) * 10);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_lparen_add_add_add_rparen_mul_int_literals(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, "(20 + 30 + 40) * 10", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_i64_t));
    test_assert(v.ptr != NULL);
    test_uint(*(ecs_i64_t*)v.ptr, (20 + 30 + 40) * 10);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_double_paren_add_add(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, "((20 + 30))", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_i64_t));
    test_assert(v.ptr != NULL);
    test_uint(*(ecs_i64_t*)v.ptr, ((20 + 30)));
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_double_paren_literal(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, "((20))", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_i64_t));
    test_assert(v.ptr != NULL);
    test_uint(*(ecs_i64_t*)v.ptr, ((20)));
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_lparen_add_add_rparen_mul_lparen_add_add_rparen(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, "(10 + 20) * (20 + 30)", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_i64_t));
    test_assert(v.ptr != NULL);
    test_uint(*(ecs_i64_t*)v.ptr, (10 + 20) * (20 + 30));
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_float_result_add_2_int_literals(void) {
    ecs_world_t *world = ecs_init();

    float value = 0;
    ecs_value_t v = {
        .type = ecs_id(ecs_f32_t),
        .ptr = &value
    };
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, "10 + 20", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_f32_t));
    test_assert(v.ptr != NULL);
    test_flt(value, 10 + 20);

    ecs_fini(world);
}

void Expr_struct_result_implicit_members(void) {
    ecs_world_t *world = ecs_init();

    typedef struct {
        int32_t x;
        int32_t y;
    } Position;

    ecs_entity_t t = ecs_struct(world, {
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    Position v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(world, "{5 + 5, 10 + 10}", &(ecs_value_t){
        .type = t, .ptr = &v
    }, &desc);

    test_assert(ptr != NULL);
    test_assert(!ptr[0]);

    test_uint(v.x, 10);
    test_uint(v.y, 20);

    ecs_fini(world);
}

void Expr_struct_result_explicit_members(void) {
    ecs_world_t *world = ecs_init();

    typedef struct {
        int32_t x;
        int32_t y;
    } Position;

    ecs_entity_t t = ecs_struct(world, {
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    Position v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(world, "{x: 5 + 5, y: 10 + 10}", &(ecs_value_t){
        .type = t, .ptr = &v
    }, &desc);
    test_assert(ptr != NULL);
    test_assert(!ptr[0]);

    test_uint(v.x, 10);
    test_uint(v.y, 20);

    ecs_fini(world);
}


void Expr_struct_result_explicit_members_reverse(void) {
    ecs_world_t *world = ecs_init();

    typedef struct {
        int32_t x;
        int32_t y;
    } Position;

    ecs_entity_t t = ecs_struct(world, {
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    Position v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(world, "{y: 5 + 5, x: 10 + 10}", &(ecs_value_t){
        .type = t, .ptr = &v
    }, &desc);
    test_assert(ptr != NULL);
    test_assert(!ptr[0]);

    test_uint(v.y, 10);
    test_uint(v.x, 20);

    ecs_fini(world);
}

void Expr_struct_result_nested_implicit_members(void) {
    ecs_world_t *world = ecs_init();

    typedef struct {
        int32_t x;
        int32_t y;
    } Point;

    typedef struct {
        Point start;
        Point stop;
    } Line;

    ecs_entity_t point = ecs_struct(world, {
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t line = ecs_struct(world, {
        .members = {
            {"start", point},
            {"stop", point}
        }
    });

    Line v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(world, "{{5 + 5, 10 + 10}, {10 + 20, 20 + 20}}", 
        &(ecs_value_t){
            .type = line, .ptr = &v
        }, &desc);
    test_assert(ptr != NULL);
    test_assert(!ptr[0]);

    test_uint(v.start.x, 10);
    test_uint(v.start.y, 20);
    test_uint(v.stop.x, 30);
    test_uint(v.stop.y, 40);

    ecs_fini(world);
}

void Expr_struct_result_nested_explicit_members(void) {
    ecs_world_t *world = ecs_init();

    typedef struct {
        int32_t x;
        int32_t y;
    } Point;

    typedef struct {
        Point start;
        Point stop;
    } Line;

    ecs_entity_t point = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Point" }),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t line = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Line" }),
        .members = {
            {"start", point},
            {"stop", point}
        }
    });

    Line v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(world, 
        "{start: {x: 5 + 5, y: 10 + 10}, stop: {x: 10 + 20, y: 20 + 20}}", 
        &(ecs_value_t){
            .type = line, .ptr = &v
        }, &desc);
    test_assert(ptr != NULL);
    test_assert(!ptr[0]);

    test_uint(v.start.x, 10);
    test_uint(v.start.y, 20);
    test_uint(v.stop.x, 30);
    test_uint(v.stop.y, 40);

    ecs_fini(world);
}

void Expr_struct_result_nested_explicit_members_reverse(void) {
    ecs_world_t *world = ecs_init();

    typedef struct {
        int32_t x;
        int32_t y;
    } Point;

    typedef struct {
        Point start;
        Point stop;
    } Line;

    ecs_entity_t point = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Point" }),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t line = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Line" }),
        .members = {
            {"start", point},
            {"stop", point}
        }
    });

    Line v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(world, 
        "{stop: {x: 5 + 5, y: 10 + 10}, start: {x: 10 + 20, y: 20 + 20}}", 
        &(ecs_value_t){
            .type = line, .ptr = &v
        }, &desc);
    test_assert(ptr != NULL);
    test_assert(!ptr[0]);

    test_uint(v.stop.x, 10);
    test_uint(v.stop.y, 20);
    test_uint(v.start.x, 30);
    test_uint(v.start.y, 40);

    ecs_fini(world);
}

void Expr_struct_result_nested_explicit_dotmembers(void) {
    ecs_world_t *world = ecs_init();

    typedef struct {
        int32_t x;
        int32_t y;
    } Point;

    typedef struct {
        Point start;
        Point stop;
    } Line;

    ecs_entity_t point = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Point" }),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t line = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Line" }),
        .members = {
            {"start", point},
            {"stop", point}
        }
    });

    Line v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(world, 
        "{stop.x: 5 + 5, start.x: 10 + 10, stop.y: 10 + 20, start.y: 20 + 20}", 
        &(ecs_value_t){
            .type = line, .ptr = &v
        }, &desc);
    test_assert(ptr != NULL);
    test_assert(!ptr[0]);

    test_uint(v.stop.x, 10);
    test_uint(v.stop.y, 30);
    test_uint(v.start.x, 20);
    test_uint(v.start.y, 40);

    ecs_fini(world);
}

void Expr_struct_result_add_2_int_literals(void) {
    ecs_world_t *world = ecs_init();

    typedef struct {
        int32_t value;
    } Mass;

    ecs_entity_t t = ecs_struct(world, {
        .members = {
            {"value", ecs_id(ecs_i32_t)}
        }
    });

    Mass v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(world, "{10 + 20}", &(ecs_value_t){
        .type = t, .ptr = &v
    }, &desc);
    test_assert(ptr != NULL);
    test_assert(!ptr[0]);

    test_uint(v.value, 10 + 20);

    ecs_fini(world);
}

void Expr_struct_result_add_2_2_fields_int_literals(void) {
    ecs_world_t *world = ecs_init();

    typedef struct {
        int32_t x;
        int32_t y;
    } Mass;

    ecs_entity_t t = ecs_struct(world, {
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    Mass v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(world, "{10 + 20, 20 + 30}", &(ecs_value_t){
        .type = t, .ptr = &v
    }, &desc);
    test_assert(ptr != NULL);
    test_assert(!ptr[0]);

    test_uint(v.x, 10 + 20);
    test_uint(v.y, 20 + 30);

    ecs_fini(world);
}

void Expr_struct_result_add_3_int_literals(void) {
    ecs_world_t *world = ecs_init();

    typedef struct {
        int32_t value;
    } Mass;

    ecs_entity_t t = ecs_struct(world, {
        .members = {
            {"value", ecs_id(ecs_i32_t)}
        }
    });

    Mass v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(world, "{10 + 20 + 30}", &(ecs_value_t){
        .type = t, .ptr = &v
    }, &desc);
    test_assert(ptr != NULL);
    test_assert(!ptr[0]);

    test_uint(v.value, 10 + 20 + 30);

    ecs_fini(world);
}

void Expr_struct_result_lparen_int_rparen(void) {
    ecs_world_t *world = ecs_init();

    typedef struct {
        int32_t value;
    } Mass;

    ecs_entity_t t = ecs_struct(world, {
        .members = {
            {"value", ecs_id(ecs_i32_t)}
        }
    });

    Mass v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(world, "{(10)}", &(ecs_value_t){
        .type = t, .ptr = &v
    }, &desc);
    test_assert(ptr != NULL);
    test_assert(!ptr[0]);

    test_uint(v.value, 10);

    ecs_fini(world);
}

void Expr_add_to_var(void) {
    ecs_world_t *world = ecs_init();

    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *var = ecs_script_vars_define(
        vars, "foo", ecs_i32_t);
    *(int32_t*)var->value.ptr = 10;

    ecs_value_t v = {0};
    ecs_expr_eval_desc_t desc = { .vars = vars, .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(world, "$foo + 20", &v, &desc);
    test_assert(ptr != NULL);
    test_assert(!ptr[0]);
    test_int(*(int32_t*)v.ptr, 10 + 20);
    ecs_value_free(world, v.type, v.ptr);

    ecs_script_vars_fini(vars);
    ecs_fini(world);
}

void Expr_add_var_to(void) {
    ecs_world_t *world = ecs_init();

    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *var = ecs_script_vars_define(
        vars, "foo", ecs_i32_t);
    *(int32_t*)var->value.ptr = 10;

    ecs_value_t v = {0};
    ecs_expr_eval_desc_t desc = { .vars = vars, .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(world, "20 + $foo", &v, &desc);
    test_assert(ptr != NULL);
    test_assert(!ptr[0]);
    test_int(*(int32_t*)v.ptr, 20 + 10);
    ecs_value_free(world, v.type, v.ptr);

    ecs_script_vars_fini(vars);
    ecs_fini(world);
}

void Expr_var_member(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, PositionI);

    ecs_struct(world, {
        .entity = ecs_id(PositionI),
        .members = {
            { .name = "x", .type = ecs_id(ecs_i32_t) },
            { .name = "y", .type = ecs_id(ecs_i32_t) }
        }
    });

    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *var = ecs_script_vars_define(
        vars, "foo", PositionI);
    *(PositionI*)var->value.ptr = (PositionI){10, 20};

    ecs_expr_eval_desc_t desc = { .vars = vars, .disable_folding = disable_folding };
    {
        ecs_value_t v = {0};
        const char *ptr = ecs_expr_run(world, "$foo.x", &v, &desc);
        test_assert(ptr != NULL);
        test_assert(!ptr[0]);
        test_uint(v.type, ecs_id(ecs_i32_t));
        test_int(*(ecs_i32_t*)v.ptr, 10);
        ecs_value_free(world, v.type, v.ptr);
    }
    {
        ecs_value_t v = {0};
        const char *ptr = ecs_expr_run(world, "$foo.y", &v, &desc);
        test_assert(ptr != NULL);
        test_assert(!ptr[0]);
        test_uint(v.type, ecs_id(ecs_i32_t));
        test_int(*(ecs_i32_t*)v.ptr, 20);
        ecs_value_free(world, v.type, v.ptr);
    }

    ecs_script_vars_fini(vars);
    ecs_fini(world);
}

void Expr_var_member_member(void) {
    ecs_world_t *world = ecs_init();

    typedef struct {
        int32_t x;
        int32_t y;
    } Point;

    typedef struct {
        Point start;
        Point stop;
    } Line;

    ecs_entity_t ecs_id(Point) = ecs_struct(world, {
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t ecs_id(Line) = ecs_struct(world, {
        .members = {
            {"start", ecs_id(Point)},
            {"stop", ecs_id(Point)}
        }
    });

    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *var = ecs_script_vars_define(
        vars, "foo", Line);
    *(Line*)var->value.ptr = (Line){{10, 20}, {30, 40}};

    ecs_expr_eval_desc_t desc = { .vars = vars, .disable_folding = disable_folding };
    {
        ecs_value_t v = {0};
        const char *ptr = ecs_expr_run(world, "$foo.start.x", &v, &desc);
        test_assert(ptr != NULL);
        test_assert(!ptr[0]);
        test_uint(v.type, ecs_id(ecs_i32_t));
        test_int(*(ecs_i32_t*)v.ptr, 10);
        ecs_value_free(world, v.type, v.ptr);
    }
    {
        ecs_value_t v = {0};
        const char *ptr = ecs_expr_run(world, "$foo.start.y", &v, &desc);
        test_assert(ptr != NULL);
        test_assert(!ptr[0]);
        test_uint(v.type, ecs_id(ecs_i32_t));
        test_int(*(ecs_i32_t*)v.ptr, 20);
        ecs_value_free(world, v.type, v.ptr);
    }
    {
        ecs_value_t v = {0};
        const char *ptr = ecs_expr_run(world, "$foo.stop.x", &v, &desc);
        test_assert(ptr != NULL);
        test_assert(!ptr[0]);
        test_uint(v.type, ecs_id(ecs_i32_t));
        test_int(*(ecs_i32_t*)v.ptr, 30);
        ecs_value_free(world, v.type, v.ptr);
    }
    {
        ecs_value_t v = {0};
        const char *ptr = ecs_expr_run(world, "$foo.stop.y", &v, &desc);
        test_assert(ptr != NULL);
        test_assert(!ptr[0]);
        test_uint(v.type, ecs_id(ecs_i32_t));
        test_int(*(ecs_i32_t*)v.ptr, 40);
        ecs_value_free(world, v.type, v.ptr);
    }

    ecs_script_vars_fini(vars);
    ecs_fini(world);
}

void Expr_var_element(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t array = ecs_array(world, {
        .entity = ecs_entity(world, { .name = "array" }),
        .type = ecs_id(ecs_i32_t),
        .count = 2
    });

    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *var = ecs_script_vars_define_id(
        vars, "foo", array);
    ((int*)var->value.ptr)[0] = 10;
    ((int*)var->value.ptr)[1] = 20;

    ecs_expr_eval_desc_t desc = { .vars = vars, .disable_folding = disable_folding };
    {
        ecs_value_t v = {0};
        const char *ptr = ecs_expr_run(world, "$foo[0]", &v, &desc);
        test_assert(ptr != NULL);
        test_assert(!ptr[0]);
        test_uint(v.type, ecs_id(ecs_i32_t));
        test_int(*(ecs_i32_t*)v.ptr, 10);
        ecs_value_free(world, v.type, v.ptr);
    }
    {
        ecs_value_t v = {0};
        const char *ptr = ecs_expr_run(world, "$foo[1]", &v, &desc);
        test_assert(ptr != NULL);
        test_assert(!ptr[0]);
        test_uint(v.type, ecs_id(ecs_i32_t));
        test_int(*(ecs_i32_t*)v.ptr, 20);
        ecs_value_free(world, v.type, v.ptr);
    }

    ecs_script_vars_fini(vars);
    ecs_fini(world);
}

void Expr_var_element_element(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ints = ecs_array(world, {
        .type = ecs_id(ecs_i32_t),
        .count = 2
    });

    ecs_entity_t arrays = ecs_array(world, {
        .type = ints,
        .count = 2
    });

    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *var = ecs_script_vars_define_id(
        vars, "foo", arrays);

    typedef int32_t Ints[2];
    Ints value[] = {{10, 20}, {30, 40}, {50, 60}};
    ecs_os_memcpy(var->value.ptr, value, sizeof(value));

    ecs_expr_eval_desc_t desc = { .vars = vars, .disable_folding = disable_folding };
    {
        ecs_value_t v = {0};
        const char *ptr = ecs_expr_run(world, "$foo[0][0]", &v, &desc);
        test_assert(ptr != NULL);
        test_assert(!ptr[0]);
        test_uint(v.type, ecs_id(ecs_i32_t));
        test_int(*(ecs_i32_t*)v.ptr, 10);
        ecs_value_free(world, v.type, v.ptr);
    }
    {
        ecs_value_t v = {0};
        const char *ptr = ecs_expr_run(world, "$foo[0][1]", &v, &desc);
        test_assert(ptr != NULL);
        test_assert(!ptr[0]);
        test_uint(v.type, ecs_id(ecs_i32_t));
        test_int(*(ecs_i32_t*)v.ptr, 20);
        ecs_value_free(world, v.type, v.ptr);
    }
    {
        ecs_value_t v = {0};
        const char *ptr = ecs_expr_run(world, "$foo[1][0]", &v, &desc);
        test_assert(ptr != NULL);
        test_assert(!ptr[0]);
        test_uint(v.type, ecs_id(ecs_i32_t));
        test_int(*(ecs_i32_t*)v.ptr, 30);
        ecs_value_free(world, v.type, v.ptr);
    }
    {
        ecs_value_t v = {0};
        const char *ptr = ecs_expr_run(world, "$foo[1][1]", &v, &desc);
        test_assert(ptr != NULL);
        test_assert(!ptr[0]);
        test_uint(v.type, ecs_id(ecs_i32_t));
        test_int(*(ecs_i32_t*)v.ptr, 40);
        ecs_value_free(world, v.type, v.ptr);
    }

    ecs_script_vars_fini(vars);
    ecs_fini(world);
}

void Expr_var_member_element(void) {
    ecs_world_t *world = ecs_init();

    typedef struct {
        int32_t x[2];
        int32_t y[2];
    } Points;

    ecs_entity_t Ints = ecs_array(world, {
        .type = ecs_id(ecs_i32_t),
        .count = 2
    });

    ecs_entity_t ecs_id(Points) = ecs_struct(world, {
        .members = {
            {"x", Ints },
            {"y", Ints }
        }
    });

    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *var = ecs_script_vars_define(
        vars, "foo", Points);
    *((Points*)var->value.ptr) = (Points){{10, 20}, {30, 40}};

    ecs_expr_eval_desc_t desc = { .vars = vars, .disable_folding = disable_folding };
    {
        ecs_value_t v = {0};
        const char *ptr = ecs_expr_run(world, "$foo.x[0]", &v, &desc);
        test_assert(ptr != NULL);
        test_assert(!ptr[0]);
        test_uint(v.type, ecs_id(ecs_i32_t));
        test_int(*(ecs_i32_t*)v.ptr, 10);
        ecs_value_free(world, v.type, v.ptr);
    }
    {
        ecs_value_t v = {0};
        const char *ptr = ecs_expr_run(world, "$foo.x[1]", &v, &desc);
        test_assert(ptr != NULL);
        test_assert(!ptr[0]);
        test_uint(v.type, ecs_id(ecs_i32_t));
        test_int(*(ecs_i32_t*)v.ptr, 20);
        ecs_value_free(world, v.type, v.ptr);
    }
    {
        ecs_value_t v = {0};
        const char *ptr = ecs_expr_run(world, "$foo.y[0]", &v, &desc);
        test_assert(ptr != NULL);
        test_assert(!ptr[0]);
        test_uint(v.type, ecs_id(ecs_i32_t));
        test_int(*(ecs_i32_t*)v.ptr, 30);
        ecs_value_free(world, v.type, v.ptr);
    }
    {
        ecs_value_t v = {0};
        const char *ptr = ecs_expr_run(world, "$foo.y[1]", &v, &desc);
        test_assert(ptr != NULL);
        test_assert(!ptr[0]);
        test_uint(v.type, ecs_id(ecs_i32_t));
        test_int(*(ecs_i32_t*)v.ptr, 40);
        ecs_value_free(world, v.type, v.ptr);
    }

    ecs_script_vars_fini(vars);
    ecs_fini(world);
}

void Expr_var_member_element_inline(void) {
    ecs_world_t *world = ecs_init();

    typedef struct {
        int32_t x[2];
        int32_t y[2];
    } Points;

    ecs_entity_t ecs_id(Points) = ecs_struct(world, {
        .members = {
            {"x", ecs_id(ecs_i32_t), .count = 2},
            {"y", ecs_id(ecs_i32_t), .count = 2}
        }
    });

    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *var = ecs_script_vars_define(
        vars, "foo", Points);
    *((Points*)var->value.ptr) = (Points){{10, 20}, {30, 40}};

    ecs_expr_eval_desc_t desc = { .vars = vars, .disable_folding = disable_folding };
    {
        ecs_value_t v = {0};
        const char *ptr = ecs_expr_run(world, "$foo.x[0]", &v, &desc);
        test_assert(ptr != NULL);
        test_assert(!ptr[0]);
        test_uint(v.type, ecs_id(ecs_i32_t));
        test_int(*(ecs_i32_t*)v.ptr, 10);
        ecs_value_free(world, v.type, v.ptr);
    }
    {
        ecs_value_t v = {0};
        const char *ptr = ecs_expr_run(world, "$foo.x[1]", &v, &desc);
        test_assert(ptr != NULL);
        test_assert(!ptr[0]);
        test_uint(v.type, ecs_id(ecs_i32_t));
        test_int(*(ecs_i32_t*)v.ptr, 20);
        ecs_value_free(world, v.type, v.ptr);
    }
    {
        ecs_value_t v = {0};
        const char *ptr = ecs_expr_run(world, "$foo.y[0]", &v, &desc);
        test_assert(ptr != NULL);
        test_assert(!ptr[0]);
        test_uint(v.type, ecs_id(ecs_i32_t));
        test_int(*(ecs_i32_t*)v.ptr, 30);
        ecs_value_free(world, v.type, v.ptr);
    }
    {
        ecs_value_t v = {0};
        const char *ptr = ecs_expr_run(world, "$foo.y[1]", &v, &desc);
        test_assert(ptr != NULL);
        test_assert(!ptr[0]);
        test_uint(v.type, ecs_id(ecs_i32_t));
        test_int(*(ecs_i32_t*)v.ptr, 40);
        ecs_value_free(world, v.type, v.ptr);
    }

    ecs_script_vars_fini(vars);
    ecs_fini(world);
}

void Expr_var_element_member(void) {
    ecs_world_t *world = ecs_init();

    typedef struct {
        int32_t x;
        int32_t y;
    } Point;

    ecs_entity_t ecs_id(Point) = ecs_struct(world, {
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t array = ecs_array(world, {
        .type = ecs_id(Point),
        .count = 2
    });

    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *var = ecs_script_vars_define_id(
        vars, "foo", array);
    ((Point*)var->value.ptr)[0] = (Point){10, 20};
    ((Point*)var->value.ptr)[1] = (Point){30, 40};

    ecs_expr_eval_desc_t desc = { .vars = vars, .disable_folding = disable_folding };
    {
        ecs_value_t v = {0};
        const char *ptr = ecs_expr_run(world, "$foo[0].x", &v, &desc);
        test_assert(ptr != NULL);
        test_assert(!ptr[0]);
        test_uint(v.type, ecs_id(ecs_i32_t));
        test_int(*(ecs_i32_t*)v.ptr, 10);
        ecs_value_free(world, v.type, v.ptr);
    }
    {
        ecs_value_t v = {0};
        const char *ptr = ecs_expr_run(world, "$foo[0].y", &v, &desc);
        test_assert(ptr != NULL);
        test_assert(!ptr[0]);
        test_uint(v.type, ecs_id(ecs_i32_t));
        test_int(*(ecs_i32_t*)v.ptr, 20);
        ecs_value_free(world, v.type, v.ptr);
    }
    {
        ecs_value_t v = {0};
        const char *ptr = ecs_expr_run(world, "$foo[1].x", &v, &desc);
        test_assert(ptr != NULL);
        test_assert(!ptr[0]);
        test_uint(v.type, ecs_id(ecs_i32_t));
        test_int(*(ecs_i32_t*)v.ptr, 30);
        ecs_value_free(world, v.type, v.ptr);
    }
    {
        ecs_value_t v = {0};
        const char *ptr = ecs_expr_run(world, "$foo[1].y", &v, &desc);
        test_assert(ptr != NULL);
        test_assert(!ptr[0]);
        test_uint(v.type, ecs_id(ecs_i32_t));
        test_int(*(ecs_i32_t*)v.ptr, 40);
        ecs_value_free(world, v.type, v.ptr);
    }

    ecs_script_vars_fini(vars);
    ecs_fini(world);
}

void Expr_bool_cond_and_bool(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, "true && true", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);

    test_assert(ecs_expr_run(world, "true && false", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);

    test_assert(ecs_expr_run(world, "false && true", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);

    test_assert(ecs_expr_run(world, "false && false", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_bool_cond_or_bool(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, "true || true", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);

    test_assert(ecs_expr_run(world, "true || false", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);

    test_assert(ecs_expr_run(world, "false || true", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);

    test_assert(ecs_expr_run(world, "false || false", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_int_cond_and_int(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, "10 && 10", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);

    test_assert(ecs_expr_run(world, "10 && 0", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);

    test_assert(ecs_expr_run(world, "0 && 10", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);

    test_assert(ecs_expr_run(world, "0 && 0", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_int_cond_or_int(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, "10 || 10", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);

    test_assert(ecs_expr_run(world, "10 || 0", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);

    test_assert(ecs_expr_run(world, "0 || 10", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);

    test_assert(ecs_expr_run(world, "0 || 0", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_bool_cond_and_int(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, "true && 10", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);

    test_assert(ecs_expr_run(world, "true && 0", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);

    test_assert(ecs_expr_run(world, "false && 10", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);

    test_assert(ecs_expr_run(world, "false && 0", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_int_cond_and_bool(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, "10 && true", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);

    test_assert(ecs_expr_run(world, "10 && false", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);

    test_assert(ecs_expr_run(world, "0 && true", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);

    test_assert(ecs_expr_run(world, "0 && false", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_bool_cond_or_int(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, "true || 10", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);

    test_assert(ecs_expr_run(world, "true || 0", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);

    test_assert(ecs_expr_run(world, "false || 10", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);

    test_assert(ecs_expr_run(world, "false || 0", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_int_cond_or_bool(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, "10 || true", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);

    test_assert(ecs_expr_run(world, "10 || false", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);

    test_assert(ecs_expr_run(world, "0 || true", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);

    test_assert(ecs_expr_run(world, "0 || false", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_cond_eq_bool(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, "true == true", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);

    test_assert(ecs_expr_run(world, "true == false", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);

    test_assert(ecs_expr_run(world, "false == true", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);

    test_assert(ecs_expr_run(world, "false == false", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_cond_eq_int(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, "10 == 10", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);

    test_assert(ecs_expr_run(world, "10 == 20", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);

    test_assert(ecs_expr_run(world, "10 == 0", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);

    test_assert(ecs_expr_run(world, "0 == 0", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_cond_eq_enum(void) {
    ecs_world_t *world = ecs_init();

    ecs_enum(world, {
        .entity = ecs_entity(world, { .name = "Color" }),
        .constants = {
            {"Red"}, 
            {"Green"}, 
            {"Blue"}
        }
    });

    typedef enum {
        Red, Green, Blue
    } Color;

    ecs_entity_t ecs_id(Color) = ecs_enum(world, {
        .entity = ecs_entity(world, { .name = "Color" }),
        .constants = {
            {"Red"}, 
            {"Green"}, 
            {"Blue"}
        }
    });

    ecs_script_vars_t *vars = ecs_script_vars_init(world);
    ecs_script_var_t *x_var = ecs_script_vars_define(vars, "x", Color);
    ecs_script_var_t *y_var = ecs_script_vars_define(vars, "y", Color);

    ecs_value_t v = {0};
    ecs_expr_eval_desc_t desc = { .vars = vars, .disable_folding = disable_folding };

    {
        *(Color*)x_var->value.ptr = Red;
        *(Color*)y_var->value.ptr = Red;
        test_assert(ecs_expr_run(world, "$x == $y", &v, &desc) != NULL);
        test_assert(v.type == ecs_id(ecs_bool_t));
        test_assert(v.ptr != NULL);
        test_bool(*(bool*)v.ptr, true);
    }

    {
        *(Color*)x_var->value.ptr = Red;
        *(Color*)y_var->value.ptr = Blue;
        test_assert(ecs_expr_run(world, "$x == $y", &v, &desc) != NULL);
        test_assert(v.type == ecs_id(ecs_bool_t));
        test_assert(v.ptr != NULL);
        test_bool(*(bool*)v.ptr, false);
    }

    {
        *(Color*)x_var->value.ptr = Red;
        *(Color*)y_var->value.ptr = Green;
        test_assert(ecs_expr_run(world, "$x == $y", &v, &desc) != NULL);
        test_assert(v.type == ecs_id(ecs_bool_t));
        test_assert(v.ptr != NULL);
        test_bool(*(bool*)v.ptr, false);
    }

    {
        *(Color*)x_var->value.ptr = Red;
        test_assert(ecs_expr_run(world, "$x == 0", &v, &desc) != NULL);
        test_assert(v.type == ecs_id(ecs_bool_t));
        test_assert(v.ptr != NULL);
        test_bool(*(bool*)v.ptr, true);
    }

    {
        *(Color*)x_var->value.ptr = Green;
        test_assert(ecs_expr_run(world, "$x == 0", &v, &desc) != NULL);
        test_assert(v.type == ecs_id(ecs_bool_t));
        test_assert(v.ptr != NULL);
        test_bool(*(bool*)v.ptr, false);
    }

    {
        *(Color*)x_var->value.ptr = Blue;
        test_assert(ecs_expr_run(world, "$x == 0", &v, &desc) != NULL);
        test_assert(v.type == ecs_id(ecs_bool_t));
        test_assert(v.ptr != NULL);
        test_bool(*(bool*)v.ptr, false);
    }

    {
        *(Color*)x_var->value.ptr = Red;
        test_assert(ecs_expr_run(world, "$x == 1", &v, &desc) != NULL);
        test_assert(v.type == ecs_id(ecs_bool_t));
        test_assert(v.ptr != NULL);
        test_bool(*(bool*)v.ptr, false);
    }

    {
        *(Color*)x_var->value.ptr = Green;
        test_assert(ecs_expr_run(world, "$x == 1", &v, &desc) != NULL);
        test_assert(v.type == ecs_id(ecs_bool_t));
        test_assert(v.ptr != NULL);
        test_bool(*(bool*)v.ptr, true);
    }

    {
        *(Color*)x_var->value.ptr = Blue;
        test_assert(ecs_expr_run(world, "$x == 1", &v, &desc) != NULL);
        test_assert(v.type == ecs_id(ecs_bool_t));
        test_assert(v.ptr != NULL);
        test_bool(*(bool*)v.ptr, false);
    }

    {
        *(Color*)x_var->value.ptr = Red;
        test_assert(ecs_expr_run(world, "$x == 2", &v, &desc) != NULL);
        test_assert(v.type == ecs_id(ecs_bool_t));
        test_assert(v.ptr != NULL);
        test_bool(*(bool*)v.ptr, false);
    }

    {
        *(Color*)x_var->value.ptr = Green;
        test_assert(ecs_expr_run(world, "$x == 2", &v, &desc) != NULL);
        test_assert(v.type == ecs_id(ecs_bool_t));
        test_assert(v.ptr != NULL);
        test_bool(*(bool*)v.ptr, false);
    }

    {
        *(Color*)x_var->value.ptr = Blue;
        test_assert(ecs_expr_run(world, "$x == 2", &v, &desc) != NULL);
        test_assert(v.type == ecs_id(ecs_bool_t));
        test_assert(v.ptr != NULL);
        test_bool(*(bool*)v.ptr, true);
    }

    ecs_value_free(world, v.type, v.ptr);

    ecs_script_vars_fini(vars);

    ecs_fini(world);
}

void Expr_cond_eq_string(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, "\"foo\" == \"foo\"", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);

    test_assert(ecs_expr_run(world, "\"foo\" == \"bar\"", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_cond_eq_entity(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, "flecs.core == flecs.core", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);

    test_assert(ecs_expr_run(world, "flecs.core == flecs", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);

    test_assert(ecs_expr_run(world, "flecs.core == 0", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_cond_neq_bool(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, "true != true", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);

    test_assert(ecs_expr_run(world, "true != false", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);

    test_assert(ecs_expr_run(world, "false != true", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);

    test_assert(ecs_expr_run(world, "false != false", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_cond_neq_int(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, "10 != 10", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);

    test_assert(ecs_expr_run(world, "10 != 20", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);

    test_assert(ecs_expr_run(world, "10 != 0", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);

    test_assert(ecs_expr_run(world, "0 != 0", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_cond_neq_enum(void) {
    ecs_world_t *world = ecs_init();

    ecs_enum(world, {
        .entity = ecs_entity(world, { .name = "Color" }),
        .constants = {
            {"Red"}, 
            {"Green"}, 
            {"Blue"}
        }
    });

    typedef enum {
        Red, Green, Blue
    } Color;

    ecs_entity_t ecs_id(Color) = ecs_enum(world, {
        .entity = ecs_entity(world, { .name = "Color" }),
        .constants = {
            {"Red"}, 
            {"Green"}, 
            {"Blue"}
        }
    });

    ecs_script_vars_t *vars = ecs_script_vars_init(world);
    ecs_script_var_t *x_var = ecs_script_vars_define(vars, "x", Color);
    ecs_script_var_t *y_var = ecs_script_vars_define(vars, "y", Color);

    ecs_value_t v = {0};
    ecs_expr_eval_desc_t desc = { .vars = vars, .disable_folding = disable_folding };

    {
        *(Color*)x_var->value.ptr = Red;
        *(Color*)y_var->value.ptr = Red;
        test_assert(ecs_expr_run(world, "$x != $y", &v, &desc) != NULL);
        test_assert(v.type == ecs_id(ecs_bool_t));
        test_assert(v.ptr != NULL);
        test_bool(*(bool*)v.ptr, false);
    }

    {
        *(Color*)x_var->value.ptr = Red;
        *(Color*)y_var->value.ptr = Blue;
        test_assert(ecs_expr_run(world, "$x != $y", &v, &desc) != NULL);
        test_assert(v.type == ecs_id(ecs_bool_t));
        test_assert(v.ptr != NULL);
        test_bool(*(bool*)v.ptr, true);
    }

    {
        *(Color*)x_var->value.ptr = Red;
        *(Color*)y_var->value.ptr = Green;
        test_assert(ecs_expr_run(world, "$x != $y", &v, &desc) != NULL);
        test_assert(v.type == ecs_id(ecs_bool_t));
        test_assert(v.ptr != NULL);
        test_bool(*(bool*)v.ptr, true);
    }

    {
        *(Color*)x_var->value.ptr = Red;
        test_assert(ecs_expr_run(world, "$x != 0", &v, &desc) != NULL);
        test_assert(v.type == ecs_id(ecs_bool_t));
        test_assert(v.ptr != NULL);
        test_bool(*(bool*)v.ptr, false);
    }

    {
        *(Color*)x_var->value.ptr = Green;
        test_assert(ecs_expr_run(world, "$x != 0", &v, &desc) != NULL);
        test_assert(v.type == ecs_id(ecs_bool_t));
        test_assert(v.ptr != NULL);
        test_bool(*(bool*)v.ptr, true);
    }

    {
        *(Color*)x_var->value.ptr = Blue;
        test_assert(ecs_expr_run(world, "$x != 0", &v, &desc) != NULL);
        test_assert(v.type == ecs_id(ecs_bool_t));
        test_assert(v.ptr != NULL);
        test_bool(*(bool*)v.ptr, true);
    }

    {
        *(Color*)x_var->value.ptr = Red;
        test_assert(ecs_expr_run(world, "$x != 1", &v, &desc) != NULL);
        test_assert(v.type == ecs_id(ecs_bool_t));
        test_assert(v.ptr != NULL);
        test_bool(*(bool*)v.ptr, true);
    }

    {
        *(Color*)x_var->value.ptr = Green;
        test_assert(ecs_expr_run(world, "$x != 1", &v, &desc) != NULL);
        test_assert(v.type == ecs_id(ecs_bool_t));
        test_assert(v.ptr != NULL);
        test_bool(*(bool*)v.ptr, false);
    }

    {
        *(Color*)x_var->value.ptr = Blue;
        test_assert(ecs_expr_run(world, "$x != 1", &v, &desc) != NULL);
        test_assert(v.type == ecs_id(ecs_bool_t));
        test_assert(v.ptr != NULL);
        test_bool(*(bool*)v.ptr, true);
    }

    {
        *(Color*)x_var->value.ptr = Red;
        test_assert(ecs_expr_run(world, "$x != 2", &v, &desc) != NULL);
        test_assert(v.type == ecs_id(ecs_bool_t));
        test_assert(v.ptr != NULL);
        test_bool(*(bool*)v.ptr, true);
    }

    {
        *(Color*)x_var->value.ptr = Green;
        test_assert(ecs_expr_run(world, "$x != 2", &v, &desc) != NULL);
        test_assert(v.type == ecs_id(ecs_bool_t));
        test_assert(v.ptr != NULL);
        test_bool(*(bool*)v.ptr, true);
    }

    {
        *(Color*)x_var->value.ptr = Blue;
        test_assert(ecs_expr_run(world, "$x != 2", &v, &desc) != NULL);
        test_assert(v.type == ecs_id(ecs_bool_t));
        test_assert(v.ptr != NULL);
        test_bool(*(bool*)v.ptr, false);
    }

    ecs_script_vars_fini(vars);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_cond_neq_string(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, "\"foo\" != \"foo\"", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);

    test_assert(ecs_expr_run(world, "\"foo\" != \"bar\"", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_cond_neq_entity(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, "flecs.core != flecs.core", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);

    test_assert(ecs_expr_run(world, "flecs.core != flecs", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);

    test_assert(ecs_expr_run(world, "flecs.core != 0", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_cond_eq_bool_int(void) {
    ecs_world_t *world = ecs_init();

    {
        ecs_value_t v = {0};
        test_assert(ecs_expr_run(world, "true == 1", &v, NULL) != NULL);
        test_assert(v.type == ecs_id(ecs_bool_t));
        test_assert(*(bool*)v.ptr == true);
        ecs_value_free(world, v.type, v.ptr);
    }

    {
        ecs_value_t v = {0};
        test_assert(ecs_expr_run(world, "true == 2", &v, NULL) != NULL);
        test_assert(v.type == ecs_id(ecs_bool_t));
        test_assert(*(bool*)v.ptr == true);
        ecs_value_free(world, v.type, v.ptr);
    }

    {
        ecs_value_t v = {0};
        test_assert(ecs_expr_run(world, "true == 0", &v, NULL) != NULL);
        test_assert(v.type == ecs_id(ecs_bool_t));
        test_assert(*(bool*)v.ptr == false);
        ecs_value_free(world, v.type, v.ptr);
    }

    {
        ecs_value_t v = {0};
        test_assert(ecs_expr_run(world, "false == 1", &v, NULL) != NULL);
        test_assert(v.type == ecs_id(ecs_bool_t));
        test_assert(*(bool*)v.ptr == false);
        ecs_value_free(world, v.type, v.ptr);
    }

    {
        ecs_value_t v = {0};
        test_assert(ecs_expr_run(world, "false == 2", &v, NULL) != NULL);
        test_assert(v.type == ecs_id(ecs_bool_t));
        test_assert(*(bool*)v.ptr == false);
        ecs_value_free(world, v.type, v.ptr);
    }

    {
        ecs_value_t v = {0};
        test_assert(ecs_expr_run(world, "false == 0", &v, NULL) != NULL);
        test_assert(v.type == ecs_id(ecs_bool_t));
        test_assert(*(bool*)v.ptr == true);
        ecs_value_free(world, v.type, v.ptr);
    }

    ecs_fini(world);
}

void Expr_cond_eq_int_flt(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};

    ecs_log_set_level(-4);
    test_assert(ecs_expr_run(world, "1 == 1.0", &v, NULL) == NULL);
    test_assert(ecs_expr_run(world, "1 == 0.0", &v, NULL) == NULL);
    test_assert(ecs_expr_run(world, "0 == 0.0", &v, NULL) == NULL);

    test_assert(ecs_expr_run(world, "1 != 1.0", &v, NULL) == NULL);
    test_assert(ecs_expr_run(world, "1 != 0.0", &v, NULL) == NULL);
    test_assert(ecs_expr_run(world, "0 != 0.0", &v, NULL) == NULL);

    ecs_fini(world);
}

void Expr_cond_eq_cond_and(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, "true == true && false", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true == true && false);

    test_assert(ecs_expr_run(world, "true && false == false", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true && false == false);

    test_assert(ecs_expr_run(world, "true && true == true", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true && true == true);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_cond_eq_cond_or(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, "true == true || false", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true == true || false);

    test_assert(ecs_expr_run(world, "true || false == false", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true || false == false);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_cond_gt_bool(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, "true > false", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);

    test_assert(ecs_expr_run(world, "true > true", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);

    test_assert(ecs_expr_run(world, "false > true", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);

    test_assert(ecs_expr_run(world, "false > false", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_cond_gt_int(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, "10 > 5", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);

    test_assert(ecs_expr_run(world, "10 > 10", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);

    test_assert(ecs_expr_run(world, "5 > 10", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);

    test_assert(ecs_expr_run(world, "5 > 5", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_cond_gt_flt(void) {
        ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, "10.5 > 5.5", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);

    test_assert(ecs_expr_run(world, "10.5 > 10.5", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);

    test_assert(ecs_expr_run(world, "5.5 > 10.5", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);

    test_assert(ecs_expr_run(world, "5.5 > 5.5", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_cond_gteq_bool(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, "true >= false", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);

    test_assert(ecs_expr_run(world, "true >= true", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);

    test_assert(ecs_expr_run(world, "false >= true", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);

    test_assert(ecs_expr_run(world, "false >= false", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_cond_gteq_int(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, "10 >= 5", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);

    test_assert(ecs_expr_run(world, "10 >= 10", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);

    test_assert(ecs_expr_run(world, "5 >= 10", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);

    test_assert(ecs_expr_run(world, "5 >= 5", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_cond_gteq_flt(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, "10.5 >= 5.5", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);

    test_assert(ecs_expr_run(world, "10.5 >= 10.5", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);

    test_assert(ecs_expr_run(world, "5.5 >= 10.5", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);

    test_assert(ecs_expr_run(world, "5.5 >= 5.5", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_cond_lt_bool(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, "true < false", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);

    test_assert(ecs_expr_run(world, "true < true", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);

    test_assert(ecs_expr_run(world, "false < true", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);

    test_assert(ecs_expr_run(world, "false < false", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_cond_lt_int(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, "10 < 5", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);

    test_assert(ecs_expr_run(world, "10 < 10", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);

    test_assert(ecs_expr_run(world, "5 < 10", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);

    test_assert(ecs_expr_run(world, "5 < 5", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_cond_lt_flt(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, "10.5 < 5.5", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);

    test_assert(ecs_expr_run(world, "10.5 < 10.5", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);

    test_assert(ecs_expr_run(world, "5.5 < 10.5", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);

    test_assert(ecs_expr_run(world, "5.5 < 5.5", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_cond_lteq_bool(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, "true <= false", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);

    test_assert(ecs_expr_run(world, "true <= true", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);

    test_assert(ecs_expr_run(world, "false <= true", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);

    test_assert(ecs_expr_run(world, "false <= false", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_cond_lteq_int(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, "10 <= 5", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);

    test_assert(ecs_expr_run(world, "10 <= 10", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);

    test_assert(ecs_expr_run(world, "5 <= 10", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);

    test_assert(ecs_expr_run(world, "5 <= 5", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_cond_lteq_flt(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, "10.5 <= 5.5", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);

    test_assert(ecs_expr_run(world, "10.5 <= 10.5", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);

    test_assert(ecs_expr_run(world, "5.5 <= 10.5", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);

    test_assert(ecs_expr_run(world, "5.5 <= 5.5", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_min_lparen_int_rparen(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, "-(10)", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_i64_t));
    test_assert(v.ptr != NULL);
    test_int(*(ecs_i64_t*)v.ptr, -10);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_min_lparen_int_add_int_rparen(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, "-(10 + 20)", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_i64_t));
    test_assert(v.ptr != NULL);
    test_int(*(ecs_i64_t*)v.ptr, -30);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_min_number_hex(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, "0xABCDEF9", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_i64_t));
    test_assert(v.ptr != NULL);
    test_int(*(ecs_i64_t*)v.ptr, 0xABCDEF9);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_min_number_bin(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, "0b10", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_i64_t));
    test_assert(v.ptr != NULL);
    test_int(*(ecs_i64_t*)v.ptr, 0b10);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_min_var(void) {
    ecs_world_t *world = ecs_init();

    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *var = ecs_script_vars_define(
        vars, "foo", ecs_u64_t);
    *(ecs_u64_t*)var->value.ptr = 10;

    ecs_value_t v = {0};
    ecs_expr_eval_desc_t desc = { .vars = vars };
    test_assert(ecs_expr_run(world, "-$foo", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_i64_t));
    test_assert(v.ptr != NULL);
    test_int(*(ecs_i64_t*)v.ptr, -10);
    ecs_value_free(world, v.type, v.ptr);

    ecs_script_vars_fini(vars);

    ecs_fini(world);
}

void Expr_min_lparen_int_rparen_to_i64(void) {
    ecs_world_t *world = ecs_init();

    ecs_i64_t vi = 0;
    ecs_value_t v = { .type = ecs_id(ecs_i64_t), .ptr = &vi };
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, "-(10)", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_i64_t));
    test_assert(v.ptr != NULL);
    test_int(vi, -10);

    ecs_fini(world);
}

void Expr_min_lparen_int_rparen_to_i32(void) {
    ecs_world_t *world = ecs_init();

    ecs_i32_t vi = 0;
    ecs_value_t v = { .type = ecs_id(ecs_i32_t), .ptr = &vi };
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };;
    test_assert(ecs_expr_run(world, "-(10)", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_i32_t));
    test_assert(v.ptr != NULL);
    test_int(vi, -10);

    ecs_fini(world);
}

void Expr_struct_w_min_var(void) {
    ecs_world_t *world = ecs_init();

    typedef struct {
        int32_t value;
    } Mass;

    ecs_entity_t t = ecs_struct(world, {
        .members = {
            {"value", ecs_id(ecs_i32_t)}
        }
    });

    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *var = ecs_script_vars_define(
        vars, "foo", ecs_i64_t);
    *(ecs_i64_t*)var->value.ptr = 10;

    Mass v = {0};
    ecs_expr_eval_desc_t desc = { .vars = vars, .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(world, "{-$foo}", &(ecs_value_t){
        .type = t, .ptr = &v
    }, &desc);
    test_assert(ptr != NULL);
    test_assert(!ptr[0]);

    test_int(v.value, -10);
    ecs_script_vars_fini(vars);

    ecs_fini(world);
}

void Expr_min_var_add(void) {
    ecs_world_t *world = ecs_init();

    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *var = ecs_script_vars_define(
        vars, "foo", ecs_i64_t);
    *(ecs_i64_t*)var->value.ptr = 10;

    int64_t v = 0;
    ecs_expr_eval_desc_t desc = { .vars = vars, .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(world, "-$foo + 5", &(ecs_value_t){
        .type = ecs_id(ecs_i64_t), .ptr = &v
    }, &desc);
    test_assert(ptr != NULL);
    test_assert(!ptr[0]);

    test_int(v, -5);
    ecs_script_vars_fini(vars);

    ecs_fini(world);
}

void Expr_min_var_sub(void) {
    ecs_world_t *world = ecs_init();

    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *var = ecs_script_vars_define(
        vars, "foo", ecs_i64_t);
    *(ecs_i64_t*)var->value.ptr = 10;

    int64_t v = 0;
    ecs_expr_eval_desc_t desc = { .vars = vars, .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(world, "-$foo - 5", &(ecs_value_t){
        .type = ecs_id(ecs_i64_t), .ptr = &v
    }, &desc);
    test_assert(ptr != NULL);
    test_assert(!ptr[0]);

    test_int(v, -15);
    ecs_script_vars_fini(vars);

    ecs_fini(world);
}

void Expr_min_var_mul(void) {
    ecs_world_t *world = ecs_init();

    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *var = ecs_script_vars_define(
        vars, "foo", ecs_i64_t);
    *(ecs_i64_t*)var->value.ptr = 10;

    int64_t v = 0;
    ecs_expr_eval_desc_t desc = { .vars = vars, .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(world, "-$foo * 5", &(ecs_value_t){
        .type = ecs_id(ecs_i64_t), .ptr = &v
    }, &desc);
    test_assert(ptr != NULL);
    test_assert(!ptr[0]);

    test_int(v, -50);
    ecs_script_vars_fini(vars);

    ecs_fini(world);
}

void Expr_min_var_div(void) {
    ecs_world_t *world = ecs_init();

    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *var = ecs_script_vars_define(
        vars, "foo", ecs_i64_t);
    *(ecs_i64_t*)var->value.ptr = 10;

    int64_t v = 0;
    ecs_expr_eval_desc_t desc = { .vars = vars, .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(world, "-$foo / 5", &(ecs_value_t){
        .type = ecs_id(ecs_i64_t), .ptr = &v
    }, &desc);
    test_assert(ptr != NULL);
    test_assert(!ptr[0]);

    test_int(v, -2);
    ecs_script_vars_fini(vars);

    ecs_fini(world);
}

void Expr_struct_w_min_lparen_int_rparen(void) {
    ecs_world_t *world = ecs_init();

    typedef struct {
        int32_t value;
    } Mass;

    ecs_entity_t t = ecs_struct(world, {
        .members = {
            {"value", ecs_id(ecs_i32_t)}
        }
    });

    Mass v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(world, "{-(10)}", &(ecs_value_t){
        .type = t, .ptr = &v
    }, &desc);
    test_assert(ptr != NULL);
    test_assert(!ptr[0]);

    test_uint(v.value, -10);

    ecs_fini(world);
}

void Expr_struct_w_min_lparen_var_rparen(void) {
    ecs_world_t *world = ecs_init();

    typedef struct {
        int32_t value;
    } Mass;

    ecs_entity_t t = ecs_struct(world, {
        .members = {
            {"value", ecs_id(ecs_i32_t)}
        }
    });

    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *var = ecs_script_vars_define(
        vars, "foo", ecs_u64_t);
    *(ecs_u64_t*)var->value.ptr = 10;

    Mass v = {0};
    ecs_expr_eval_desc_t desc = { .vars = vars, .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(world, "{-($foo)}", &(ecs_value_t){
        .type = t, .ptr = &v
    }, &desc);
    test_assert(ptr != NULL);
    test_assert(!ptr[0]);

    test_uint(v.value, -10);
    ecs_script_vars_fini(vars);

    ecs_fini(world);
}

void Expr_not_bool(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, "!false", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);
    ecs_value_free(world, v.type, v.ptr);
    
    ecs_os_zeromem(&v);

    test_assert(ecs_expr_run(world, "!true", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_not_int(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, "!0", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);
    ecs_value_free(world, v.type, v.ptr);

    ecs_os_zeromem(&v);

    test_assert(ecs_expr_run(world, "!10", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_not_paren_int(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, "!(0)", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);
    ecs_value_free(world, v.type, v.ptr);

    ecs_os_zeromem(&v);

    test_assert(ecs_expr_run(world, "!(10)", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_not_paren_expr(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, "!(10 - 10)", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);
    ecs_value_free(world, v.type, v.ptr);

    ecs_os_zeromem(&v);

    test_assert(ecs_expr_run(world, "!(5 + 5)", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_not_var(void) {
    ecs_world_t *world = ecs_init();

    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *var = ecs_script_vars_define(
        vars, "foo", ecs_i32_t);
    test_assert(var != NULL);
    *(int32_t*)var->value.ptr = 10;

    ecs_value_t v = {0};
    ecs_expr_eval_desc_t desc = { .vars = vars, .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, "!$foo", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_uint(*(bool*)v.ptr, false);
    ecs_value_free(world, v.type, v.ptr);

    ecs_os_zeromem(&v);

    *(int32_t*)var->value.ptr = 0;
    test_assert(ecs_expr_run(world, "!$foo", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_uint(*(bool*)v.ptr, true);
    ecs_value_free(world, v.type, v.ptr);

    ecs_script_vars_fini(vars);

    ecs_fini(world);
}

void Expr_shift_left_int(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, "1 << 2", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_i64_t));
    test_assert(v.ptr != NULL);
    test_uint(*(ecs_i64_t*)v.ptr, 1 << 2);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_shift_right_int(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, "4 >> 2", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_i64_t));
    test_assert(v.ptr != NULL);
    test_uint(*(ecs_i64_t*)v.ptr, 4 >> 2);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_shift_left_int_add_int(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, "1 << 2 + 10", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_i64_t));
    test_assert(v.ptr != NULL);
    test_uint(*(ecs_i64_t*)v.ptr, 1 << (2 + 10));
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_shift_left_int_mul_int(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, "1 << 2 * 10", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_i64_t));
    test_assert(v.ptr != NULL);
    test_uint(*(ecs_i64_t*)v.ptr, 1 << 2 * 10);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_add_int_shift_left_int(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, "10 + 1 << 2", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_i64_t));
    test_assert(v.ptr != NULL);
    test_uint(*(ecs_i64_t*)v.ptr, (10 + 1) << 2);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_mul_int_shift_left_int(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, "10 * 1 << 2", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_i64_t));
    test_assert(v.ptr != NULL);
    test_uint(*(ecs_i64_t*)v.ptr, 10 * 1 << 2);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_add_int_shift_left_int_add_int(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, "10 + 1 << 2 + 2", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_i64_t));
    test_assert(v.ptr != NULL);
    test_uint(*(ecs_i64_t*)v.ptr, (10 + 1) << (2 + 2));
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_mul_int_shift_left_int_mul_int(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, "10 * 1 << 2 * 2", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_i64_t));
    test_assert(v.ptr != NULL);
    test_uint(*(ecs_i64_t*)v.ptr, 10 * 1 << 2 * 2);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_entity_0_expr(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, "#0", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_entity_t));
    test_assert(v.ptr != NULL);
    test_uint(*(ecs_entity_t*)v.ptr, 0);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_entity_expr(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t foo = ecs_entity(world, { .name = "foo" });
    test_assert(foo != 0);

    ecs_value_t v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, "foo", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_entity_t));
    test_assert(v.ptr != NULL);
    test_uint(*(ecs_entity_t*)v.ptr, foo);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_entity_path_expr(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t parent = ecs_entity(world, { .name = "parent" });
    test_assert(parent != 0);

    ecs_entity_t foo = ecs_entity(world, { .name = "parent.foo" });
    test_assert(foo != 0);

    ecs_value_t v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, "parent.foo", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_entity_t));
    test_assert(v.ptr != NULL);
    test_uint(*(ecs_entity_t*)v.ptr, foo);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_root_lookup_func(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t foo = ecs_entity(world, { .name = "foo" });
    test_assert(foo != 0);

    ecs_value_t v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, "#0.lookup(\"foo\")", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_entity_t));
    test_assert(v.ptr != NULL);
    test_uint(*(ecs_entity_t*)v.ptr, foo);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_root_lookup_func_not_found(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t foo = ecs_entity(world, { .name = "foo" });
    test_assert(foo != 0);

    ecs_value_t v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, "#0.lookup(\"bar\")", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_entity_t));
    test_assert(v.ptr != NULL);
    test_uint(*(ecs_entity_t*)v.ptr, 0);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_root_lookup_func_w_entity_arg(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t foo = ecs_entity(world, { .name = "foo" });
    test_assert(foo != 0);

    ecs_value_t v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };

    ecs_log_set_level(-4);
    test_assert(
        ecs_expr_run(world, "#0.lookup(foo)", &v, &desc) == NULL);

    ecs_fini(world);
}

void Expr_entity_lookup_func(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t parent = ecs_entity(world, { .name = "parent" });
    test_assert(parent != 0);

    ecs_entity_t foo = ecs_entity(world, { .name = "parent.foo" });
    test_assert(foo != 0);

    ecs_value_t v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, "parent.lookup(\"foo\")", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_entity_t));
    test_assert(v.ptr != NULL);
    test_uint(*(ecs_entity_t*)v.ptr, foo);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_entity_lookup_func_not_found(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t parent = ecs_entity(world, { .name = "parent" });
    test_assert(parent != 0);

    ecs_entity_t foo = ecs_entity(world, { .name = "parent.foo" });
    test_assert(foo != 0);

    ecs_value_t v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, "parent.lookup(\"bar\")", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_entity_t));
    test_assert(v.ptr != NULL);
    test_uint(*(ecs_entity_t*)v.ptr, 0);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_entity_lookup_func_w_entity_arg(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t parent = ecs_entity(world, { .name = "parent" });
    test_assert(parent != 0);

    ecs_entity_t foo = ecs_entity(world, { .name = "parent.foo" });
    test_assert(foo != 0);

    ecs_value_t v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };

    ecs_log_set_level(-4);
    test_assert(
        ecs_expr_run(world, "parent.lookup(parent.foo)", &v, &desc) == NULL);

    ecs_fini(world);
}

void Expr_entity_parent_func(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t parent = ecs_entity(world, { .name = "parent" });
    test_assert(parent != 0);

    ecs_entity_t foo = ecs_entity(world, { .name = "parent.foo" });
    test_assert(foo != 0);

    ecs_value_t v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, "parent.foo.parent()", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_entity_t));
    test_assert(v.ptr != NULL);
    test_uint(*(ecs_entity_t*)v.ptr, parent);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_entity_name_func(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t parent = ecs_entity(world, { .name = "parent" });
    test_assert(parent != 0);

    ecs_entity_t foo = ecs_entity(world, { .name = "parent.foo" });
    test_assert(foo != 0);

    ecs_value_t v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, "parent.foo.name()", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_string_t));
    test_assert(v.ptr != NULL);
    test_str(*(char**)v.ptr, "foo");
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_entity_has_func(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t e = ecs_entity(world, { .name = "e" });
    test_assert(e != 0);

    ecs_add(world, e, Position);

    {
        ecs_value_t v = {0};
        ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
        test_assert(ecs_expr_run(world, "e.has(Position)", &v, &desc) != NULL);
        test_assert(v.type == ecs_id(ecs_bool_t));
        test_assert(v.ptr != NULL);
        test_bool(*(bool*)v.ptr, true);
        ecs_value_free(world, v.type, v.ptr);
    }

    {
        ecs_value_t v = {0};
        ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
        test_assert(ecs_expr_run(world, "e.has(Velocity)", &v, &desc) != NULL);
        test_assert(v.type == ecs_id(ecs_bool_t));
        test_assert(v.ptr != NULL);
        test_bool(*(bool*)v.ptr, false);
        ecs_value_free(world, v.type, v.ptr);
    }

    ecs_fini(world);
}

void Expr_entity_has_func_w_pair(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Likes);
    ECS_TAG(world, Apples);
    ECS_TAG(world, Pears);

    ecs_entity_t e = ecs_entity(world, { .name = "e" });
    test_assert(e != 0);

    ecs_add_pair(world, e, Likes, Apples);

    {
        ecs_value_t v = {0};
        ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
        test_assert(ecs_expr_run(world, "e.has(flecs.script.core.pair(Likes, Apples))", &v, &desc) != NULL);
        test_assert(v.type == ecs_id(ecs_bool_t));
        test_assert(v.ptr != NULL);
        test_bool(*(bool*)v.ptr, true);
        ecs_value_free(world, v.type, v.ptr);
    }

    {
        ecs_value_t v = {0};
        ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
        test_assert(ecs_expr_run(world, "e.has(flecs.script.core.pair(Likes, Pears))", &v, &desc) != NULL);
        test_assert(v.type == ecs_id(ecs_bool_t));
        test_assert(v.ptr != NULL);
        test_bool(*(bool*)v.ptr, false);
        ecs_value_free(world, v.type, v.ptr);
    }

    ecs_fini(world);
}

void Expr_entity_has_func_w_pair_pair_invalid(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Likes);
    ECS_TAG(world, Apples);
    ECS_TAG(world, Pears);

    ecs_entity_t e = ecs_entity(world, { .name = "e" });
    test_assert(e != 0);

    ecs_add_pair(world, e, Likes, Apples);

    {
        ecs_value_t v = {0};
        ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
        ecs_log_set_level(-4);
        test_assert(ecs_expr_run(world, 
            "e.has(flecs.script.core.pair(flecs.script.core.pair(Likes, Apples), Pears))", 
            &v, &desc) == NULL);
    }

    ecs_fini(world);
}

void Expr_entity_doc_name_func(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t parent = ecs_entity(world, { .name = "parent" });
    test_assert(parent != 0);

    ecs_entity_t foo = ecs_entity(world, { .name = "parent.foo" });
    test_assert(foo != 0);
    ecs_doc_set_name(world, foo, "FooName");
    ecs_doc_set_uuid(world, foo, "FooUuid");
    ecs_doc_set_brief(world, foo, "FooBrief");
    ecs_doc_set_detail(world, foo, "FooDetail");
    ecs_doc_set_link(world, foo, "FooLink");
    ecs_doc_set_color(world, foo, "FooColor");

    ecs_value_t v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, "parent.foo.doc_name()", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_string_t));
    test_assert(v.ptr != NULL);
    test_str(*(char**)v.ptr, "FooName");
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_entity_doc_uuid_func(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t parent = ecs_entity(world, { .name = "parent" });
    test_assert(parent != 0);

    ecs_entity_t foo = ecs_entity(world, { .name = "parent.foo" });
    test_assert(foo != 0);
    ecs_doc_set_name(world, foo, "FooName");
    ecs_doc_set_uuid(world, foo, "FooUuid");
    ecs_doc_set_brief(world, foo, "FooBrief");
    ecs_doc_set_detail(world, foo, "FooDetail");
    ecs_doc_set_link(world, foo, "FooLink");
    ecs_doc_set_color(world, foo, "FooColor");

    ecs_value_t v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, "parent.foo.doc_uuid()", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_string_t));
    test_assert(v.ptr != NULL);
    test_str(*(char**)v.ptr, "FooUuid");
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_entity_doc_brief_func(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t parent = ecs_entity(world, { .name = "parent" });
    test_assert(parent != 0);

    ecs_entity_t foo = ecs_entity(world, { .name = "parent.foo" });
    test_assert(foo != 0);
    ecs_doc_set_name(world, foo, "FooName");
    ecs_doc_set_uuid(world, foo, "FooUuid");
    ecs_doc_set_brief(world, foo, "FooBrief");
    ecs_doc_set_detail(world, foo, "FooDetail");
    ecs_doc_set_link(world, foo, "FooLink");
    ecs_doc_set_color(world, foo, "FooColor");

    ecs_value_t v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, "parent.foo.doc_brief()", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_string_t));
    test_assert(v.ptr != NULL);
    test_str(*(char**)v.ptr, "FooBrief");
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_entity_doc_detail_func(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t parent = ecs_entity(world, { .name = "parent" });
    test_assert(parent != 0);

    ecs_entity_t foo = ecs_entity(world, { .name = "parent.foo" });
    test_assert(foo != 0);
    ecs_doc_set_name(world, foo, "FooName");
    ecs_doc_set_uuid(world, foo, "FooUuid");
    ecs_doc_set_brief(world, foo, "FooBrief");
    ecs_doc_set_detail(world, foo, "FooDetail");
    ecs_doc_set_link(world, foo, "FooLink");
    ecs_doc_set_color(world, foo, "FooColor");

    ecs_value_t v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, "parent.foo.doc_detail()", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_string_t));
    test_assert(v.ptr != NULL);
    test_str(*(char**)v.ptr, "FooDetail");
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_entity_doc_link_func(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t parent = ecs_entity(world, { .name = "parent" });
    test_assert(parent != 0);

    ecs_entity_t foo = ecs_entity(world, { .name = "parent.foo" });
    test_assert(foo != 0);
    ecs_doc_set_name(world, foo, "FooName");
    ecs_doc_set_uuid(world, foo, "FooUuid");
    ecs_doc_set_brief(world, foo, "FooBrief");
    ecs_doc_set_detail(world, foo, "FooDetail");
    ecs_doc_set_link(world, foo, "FooLink");
    ecs_doc_set_color(world, foo, "FooColor");

    ecs_value_t v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, "parent.foo.doc_link()", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_string_t));
    test_assert(v.ptr != NULL);
    test_str(*(char**)v.ptr, "FooLink");
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_entity_doc_color_func(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t parent = ecs_entity(world, { .name = "parent" });
    test_assert(parent != 0);

    ecs_entity_t foo = ecs_entity(world, { .name = "parent.foo" });
    test_assert(foo != 0);
    ecs_doc_set_name(world, foo, "FooName");
    ecs_doc_set_uuid(world, foo, "FooUuid");
    ecs_doc_set_brief(world, foo, "FooBrief");
    ecs_doc_set_detail(world, foo, "FooDetail");
    ecs_doc_set_link(world, foo, "FooLink");
    ecs_doc_set_color(world, foo, "FooColor");

    ecs_value_t v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, "parent.foo.doc_color()", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_string_t));
    test_assert(v.ptr != NULL);
    test_str(*(char**)v.ptr, "FooColor");
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_entity_chain_func(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t parent = ecs_entity(world, { .name = "parent" });
    test_assert(parent != 0);

    ecs_entity_t foo = ecs_entity(world, { .name = "parent.foo" });
    test_assert(foo != 0);

    ecs_value_t v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, "parent.foo.parent().name()", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_string_t));
    test_assert(v.ptr != NULL);
    test_str(*(char**)v.ptr, "parent");
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_var_parent_func(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t parent = ecs_entity(world, { .name = "parent" });
    test_assert(parent != 0);

    ecs_entity_t child = ecs_entity(world, { .name = "parent.child" });
    test_assert(child != 0);

    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *var = ecs_script_vars_define(
        vars, "foo", ecs_entity_t);
    test_assert(var != NULL);
    *(ecs_entity_t*)var->value.ptr = child;

    ecs_value_t v = {0};
    ecs_expr_eval_desc_t desc = { .vars = vars, .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, "$foo.parent()", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_entity_t));
    test_assert(v.ptr != NULL);
    test_uint(*(ecs_entity_t*)v.ptr, parent);
    ecs_value_free(world, v.type, v.ptr);

    ecs_script_vars_fini(vars);

    ecs_fini(world);
}

void Expr_entity_path_func(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t parent = ecs_entity(world, { .name = "parent" });
    test_assert(parent != 0);

    ecs_entity_t foo = ecs_entity(world, { .name = "parent.foo" });
    test_assert(foo != 0);

    ecs_value_t v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, "parent.foo.path()", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_string_t));
    test_assert(v.ptr != NULL);
    test_str(*(char**)v.ptr, "parent.foo");
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_var_name_func(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t parent = ecs_entity(world, { .name = "parent" });
    test_assert(parent != 0);

    ecs_entity_t child = ecs_entity(world, { .name = "parent.child" });
    test_assert(child != 0);

    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *var = ecs_script_vars_define(
        vars, "foo", ecs_entity_t);
    test_assert(var != NULL);
    *(ecs_entity_t*)var->value.ptr = child;

    ecs_value_t v = {0};
    ecs_expr_eval_desc_t desc = { .vars = vars, .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, "$foo.name()", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_string_t));
    test_assert(v.ptr != NULL);
    test_str(*(char**)v.ptr, "child");
    ecs_value_free(world, v.type, v.ptr);

    ecs_script_vars_fini(vars);

    ecs_fini(world);
}

void Expr_var_doc_name_func(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t parent = ecs_entity(world, { .name = "parent" });
    test_assert(parent != 0);

    ecs_entity_t child = ecs_entity(world, { .name = "parent.child" });
    test_assert(child != 0);
    ecs_doc_set_name(world, child, "ChildDoc");

    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *var = ecs_script_vars_define(
        vars, "foo", ecs_entity_t);
    test_assert(var != NULL);
    *(ecs_entity_t*)var->value.ptr = child;

    ecs_value_t v = {0};
    ecs_expr_eval_desc_t desc = { .vars = vars, .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, "$foo.doc_name()", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_string_t));
    test_assert(v.ptr != NULL);
    test_str(*(char**)v.ptr, "ChildDoc");
    ecs_value_free(world, v.type, v.ptr);

    ecs_script_vars_fini(vars);

    ecs_fini(world);
}

void Expr_var_chain_func(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t parent = ecs_entity(world, { .name = "parent" });
    test_assert(parent != 0);

    ecs_entity_t child = ecs_entity(world, { .name = "parent.child" });
    test_assert(child != 0);

    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *var = ecs_script_vars_define(
        vars, "foo", ecs_entity_t);
    test_assert(var != NULL);
    *(ecs_entity_t*)var->value.ptr = child;

    ecs_value_t v = {0};
    ecs_expr_eval_desc_t desc = { .vars = vars, .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, "$foo.parent().name()", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_string_t));
    test_assert(v.ptr != NULL);
    test_str(*(char**)v.ptr, "parent");
    ecs_value_free(world, v.type, v.ptr);

    ecs_script_vars_fini(vars);

    ecs_fini(world);
}

static ecs_entity_t func_function = 0;
static void *func_ctx = NULL;

static
void func_callback(
    const ecs_function_ctx_t *ctx,
    int32_t argc,
    const ecs_value_t *argv,
    ecs_value_t *result) 
{
    test_assert(result != NULL);
    *(ecs_i32_t*)result->ptr = 10;
    func_function = ctx->function;
    func_ctx = ctx->ctx;
}

static
void func_1_arg_callback(
    const ecs_function_ctx_t *ctx,
    int32_t argc,
    const ecs_value_t *argv,
    ecs_value_t *result) 
{
    test_assert(result != NULL);
    test_int(argc, 1);
    test_assert(argv != NULL);

    *(ecs_i32_t*)result->ptr = *(int32_t*)argv[0].ptr;
    func_function = ctx->function;
    func_ctx = ctx->ctx;
}

static
void func_2_args_callback(
    const ecs_function_ctx_t *ctx,
    int32_t argc,
    const ecs_value_t *argv,
    ecs_value_t *result) 
{
    test_assert(result != NULL);
    test_int(argc, 2);
    test_assert(argv != NULL);

    *(ecs_i32_t*)result->ptr = 
        *(int32_t*)argv[0].ptr +
        *(int32_t*)argv[1].ptr;

    func_function = ctx->function;
    func_ctx = ctx->ctx;
}

static
void i64_method_1_arg_callback(
    const ecs_function_ctx_t *ctx,
    int32_t argc,
    const ecs_value_t *argv,
    ecs_value_t *result) 
{
    test_assert(result != NULL);
    test_int(argc, 1);
    test_assert(argv != NULL);

    *(ecs_i64_t*)result->ptr = 
        *(int64_t*)argv[0].ptr +
        *(int64_t*)argv[1].ptr;
    func_function = ctx->function;
    func_ctx = ctx->ctx;
}

static
void func_1_position_arg_callback(
    const ecs_function_ctx_t *ctx,
    int32_t argc,
    const ecs_value_t *argv,
    ecs_value_t *result) 
{
    test_assert(result != NULL);
    test_int(argc, 1);
    test_assert(argv != NULL);

    Position *p = argv[0].ptr;

    *(ecs_i32_t*)result->ptr = p->x + p->y;
    func_function = ctx->function;
    func_ctx = ctx->ctx;
}

static
void func_1_array_arg_callback(
    const ecs_function_ctx_t *ctx,
    int32_t argc,
    const ecs_value_t *argv,
    ecs_value_t *result) 
{
    test_assert(result != NULL);
    test_int(argc, 1);
    test_assert(argv != NULL);

    typedef int32_t Ints[2];

    Ints *p = argv[0].ptr;

    *(ecs_i32_t*)result->ptr = (*p)[0] + (*p)[1];
    func_function = ctx->function;
    func_ctx = ctx->ctx;
}

void Expr_root_func_void_return(void) {
    install_test_abort();

    ecs_world_t *world = ecs_init();

    test_expect_abort();
    ecs_function(world, {
        .name = "func",
        .callback = func_callback
    });
}

void Expr_root_func(void) {
    ecs_world_t *world = ecs_init();

    int ctx;

    ecs_entity_t func = ecs_function(world, {
        .name = "func",
        .return_type = ecs_id(ecs_i32_t),
        .callback = func_callback,
        .ctx = &ctx
    });

    int32_t v = 0;
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, "func()", 
        &ecs_value_ptr(ecs_i32_t, &v), &desc) != NULL);
    test_int(v, 10);

    test_assert(func_function == func);
    test_assert(func_ctx == &ctx);

    ecs_fini(world);
}

void Expr_scoped_func(void) {
    ecs_world_t *world = ecs_init();

    int ctx;

    ecs_entity_t func = ecs_function(world, {
        .name = "func",
        .parent = ecs_entity(world, { .name = "parent" }),
        .return_type = ecs_id(ecs_i32_t),
        .callback = func_callback,
        .ctx = &ctx
    });

    int32_t v = 0;
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, "parent.func()",
        &ecs_value_ptr(ecs_i32_t, &v), &desc) != NULL);
    test_int(v, 10);

    test_assert(func_function == func);
    test_assert(func_ctx == &ctx);

    ecs_fini(world);
}

void Expr_root_func_w_1_arg(void) {
    ecs_world_t *world = ecs_init();

    int ctx;

    ecs_entity_t func = ecs_function(world, {
        .name = "func",
        .return_type = ecs_id(ecs_i32_t),
        .params = {
            { "a", ecs_id(ecs_i32_t) }
        },
        .callback = func_1_arg_callback,
        .ctx = &ctx
    });

    int32_t v = 0;
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, "func(10)", 
        &ecs_value_ptr(ecs_i32_t, &v), &desc) != NULL);
    test_int(v, 10);

    test_assert(ecs_expr_run(world, "func(20)", 
        &ecs_value_ptr(ecs_i32_t, &v), &desc) != NULL);
    test_int(v, 20);

    test_assert(func_function == func);
    test_assert(func_ctx == &ctx);

    ecs_fini(world);
}

void Expr_root_func_w_1_arg_expr(void) {
    ecs_world_t *world = ecs_init();

    int ctx;

    ecs_entity_t func = ecs_function(world, {
        .name = "func",
        .return_type = ecs_id(ecs_i32_t),
        .params = {
            { "a", ecs_id(ecs_i32_t) }
        },
        .callback = func_1_arg_callback,
        .ctx = &ctx
    });

    int32_t v = 0;
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, "func(10 + 20)", 
        &ecs_value_ptr(ecs_i32_t, &v), &desc) != NULL);
    test_int(v, 30);

    test_assert(ecs_expr_run(world, "func(20 + 30)", 
        &ecs_value_ptr(ecs_i32_t, &v), &desc) != NULL);
    test_int(v, 50);

    test_assert(func_function == func);
    test_assert(func_ctx == &ctx);

    ecs_fini(world);
}

void Expr_root_func_w_2_args(void) {
    ecs_world_t *world = ecs_init();

    int ctx;

    ecs_entity_t func = ecs_function(world, {
        .name = "func",
        .return_type = ecs_id(ecs_i32_t),
        .params = {
            { "a", ecs_id(ecs_i32_t) },
            { "b", ecs_id(ecs_i32_t) }
        },
        .callback = func_2_args_callback,
        .ctx = &ctx
    });

    int32_t v = 0;
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, "func(10, 20)", 
        &ecs_value_ptr(ecs_i32_t, &v), &desc) != NULL);
    test_int(v, 30);

    test_assert(ecs_expr_run(world, "func(20, 30)", 
        &ecs_value_ptr(ecs_i32_t, &v), &desc) != NULL);
    test_int(v, 50);

    test_assert(func_function == func);
    test_assert(func_ctx == &ctx);

    ecs_fini(world);
}

void Expr_root_func_w_enum_arg(void) {
    ecs_world_t *world = ecs_init();

    int ctx;

    ecs_entity_t color = ecs_enum(world, {
        .constants = {
            {"Red"}, 
            {"Green"}, 
            {"Blue"}
        }
    });

    ecs_entity_t func = ecs_function(world, {
        .name = "func",
        .return_type = color,
        .params = {
            { "a", color }
        },
        .callback = func_1_arg_callback,
        .ctx = &ctx
    });

    int32_t v = 0;
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, "func(Red)", 
        &ecs_value_ptr(ecs_i32_t, &v), &desc) != NULL);
    test_int(v, 0);

    test_assert(ecs_expr_run(world, "func(Green)", 
        &ecs_value_ptr(ecs_i32_t, &v), &desc) != NULL);
    test_int(v, 1);

    test_assert(ecs_expr_run(world, "func(Blue)", 
        &ecs_value_ptr(ecs_i32_t, &v), &desc) != NULL);
    test_int(v, 2);

    test_assert(func_function == func);
    test_assert(func_ctx == &ctx);

    ecs_fini(world);
}

void Expr_root_func_w_struct_arg(void) {
    ecs_world_t *world = ecs_init();

    int ctx;

    ECS_COMPONENT(world, Position);

    ecs_struct(world, {
        .entity = ecs_id(Position),
        .members = {
            { .name = "x", .type = ecs_id(ecs_f32_t) },
            { .name = "y", .type = ecs_id(ecs_f32_t) }
        }
    });

    ecs_entity_t func = ecs_function(world, {
        .name = "func",
        .return_type = ecs_id(ecs_i32_t),
        .params = {
            { "a", ecs_id(Position) }
        },
        .callback = func_1_position_arg_callback,
        .ctx = &ctx
    });

    int32_t v = 0;
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, "func({10, 20})", 
        &ecs_value_ptr(ecs_i32_t, &v), &desc) != NULL);
    test_int(v, 30);

    test_assert(ecs_expr_run(world, "func({30, 40})", 
        &ecs_value_ptr(ecs_i32_t, &v), &desc) != NULL);
    test_int(v, 70);

    test_assert(func_function == func);
    test_assert(func_ctx == &ctx);

    ecs_fini(world);
}

void Expr_root_func_w_array_arg(void) {
    ecs_world_t *world = ecs_init();

    int ctx;

    typedef int32_t Ints[2];

    ECS_COMPONENT(world, Ints);

    ecs_array(world, {
        .entity = ecs_id(Ints),
        .type = ecs_id(ecs_i32_t),
        .count = 2
    });

    ecs_entity_t func = ecs_function(world, {
        .name = "func",
        .return_type = ecs_id(ecs_i32_t),
        .params = {
            { "a", ecs_id(Ints) }
        },
        .callback = func_1_array_arg_callback,
        .ctx = &ctx
    });

    int32_t v = 0;
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, "func([10, 20])", 
        &ecs_value_ptr(ecs_i32_t, &v), &desc) != NULL);
    test_int(v, 30);

    test_assert(ecs_expr_run(world, "func([30, 40])", 
        &ecs_value_ptr(ecs_i32_t, &v), &desc) != NULL);
    test_int(v, 70);

    test_assert(func_function == func);
    test_assert(func_ctx == &ctx);

    ecs_fini(world);
}

void Expr_root_func_mismatching_args(void) {
    ecs_world_t *world = ecs_init();

    int ctx;

    ecs_function(world, {
        .name = "func",
        .return_type = ecs_id(ecs_i32_t),
        .params = {
            { "a", ecs_id(ecs_i32_t) },
            { "b", ecs_id(ecs_i32_t) }
        },
        .callback = func_2_args_callback,
        .ctx = &ctx
    });

    int32_t v = 0;
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };

    ecs_log_set_level(-4);

    test_assert(ecs_expr_run(world, "func(10)", 
        &ecs_value_ptr(ecs_i32_t, &v), &desc) == NULL);

    test_assert(ecs_expr_run(world, "func(10, 20, 30)", 
        &ecs_value_ptr(ecs_i32_t, &v), &desc) == NULL);

    ecs_fini(world);
}

void Expr_method_w_1_arg(void) {
    ecs_world_t *world = ecs_init();

    int ctx;

    ecs_entity_t func = ecs_method(world, {
        .name = "add",
        .parent = ecs_id(ecs_i64_t),
        .return_type = ecs_id(ecs_i64_t),
        .params = {
            { "a", ecs_id(ecs_i64_t) },
        },
        .callback = i64_method_1_arg_callback,
        .ctx = &ctx
    });

    int32_t v = 0;
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, "(10).add(20)", 
        &ecs_value_ptr(ecs_i32_t, &v), &desc) != NULL);
    test_int(v, 30);

    test_assert(ecs_expr_run(world, "(20).add(30))",
        &ecs_value_ptr(ecs_i32_t, &v), &desc) != NULL);
    test_int(v, 50);

    test_assert(func_function == func);
    test_assert(func_ctx == &ctx);

    ecs_fini(world);
}

static
void fn_sum_i32(
    const ecs_function_ctx_t *ctx,
    int32_t argc,
    const ecs_value_t *argv,
    ecs_value_t *result,
    int32_t elem_count)
{
    int32_t *v = argv[0].ptr;

    ((double*)result->ptr)[0] = 0;
    for (int32_t i = 0; i < elem_count; i ++) {
        ((double*)result->ptr)[0] += v[i];
    }
}

static
void fn_sum_i64(
    const ecs_function_ctx_t *ctx,
    int32_t argc,
    const ecs_value_t *argv,
    ecs_value_t *result,
    int32_t elem_count)
{
    int64_t *v = argv[0].ptr;

    ((double*)result->ptr)[0] = 0;
    for (int32_t i = 0; i < elem_count; i ++) {
        ((double*)result->ptr)[0] += v[i];
    }
}

static
void fn_sum_f32(
    const ecs_function_ctx_t *ctx,
    int32_t argc,
    const ecs_value_t *argv,
    ecs_value_t *result,
    int32_t elem_count)
{
    float *v = argv[0].ptr;

    ((double*)result->ptr)[0] = 0;
    for (int32_t i = 0; i < elem_count; i ++) {
        ((double*)result->ptr)[0] += v[i];
    }
}

static
void fn_sum_f64(
    const ecs_function_ctx_t *ctx,
    int32_t argc,
    const ecs_value_t *argv,
    ecs_value_t *result,
    int32_t elem_count)
{
    double *v = argv[0].ptr;

    ((double*)result->ptr)[0] = 0;
    for (int32_t i = 0; i < elem_count; i ++) {
        ((double*)result->ptr)[0] += v[i];
    }
}

void Expr_vector_func_sum_w_int_literal(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t func = ecs_function(world, {
        .name = "sum",
        .return_type = ecs_id(ecs_f64_t),
        .params = {
            { "v", EcsScriptVectorType },
        },
        .vector_callbacks = {
            [EcsI32] = fn_sum_i32,
            [EcsI64] = fn_sum_i64,
            [EcsF32] = fn_sum_f32,
            [EcsF64] = fn_sum_f64
        }
    });

    test_assert(func != 0);

    double v = 0;
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, "sum(20)", 
        &ecs_value_ptr(ecs_f64_t, &v), &desc) != NULL);
    test_int(v, 20);

    test_assert(ecs_expr_run(world, "sum(30)",
        &ecs_value_ptr(ecs_f64_t, &v), &desc) != NULL);
    test_int(v, 30);

    ecs_fini(world);
}

void Expr_vector_func_sum_w_float_literal(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t func = ecs_function(world, {
        .name = "sum",
        .return_type = ecs_id(ecs_f64_t),
        .params = {
            { "v", EcsScriptVectorType },
        },
        .vector_callbacks = {
            [EcsI32] = fn_sum_i32,
            [EcsI64] = fn_sum_i64,
            [EcsF32] = fn_sum_f32,
            [EcsF64] = fn_sum_f64
        }
    });

    test_assert(func != 0);

    double v = 0;
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, "sum(10.5)", 
        &ecs_value_ptr(ecs_f64_t, &v), &desc) != NULL);
    test_assert(v == 10.5);

    test_assert(ecs_expr_run(world, "sum(0.5)",
        &ecs_value_ptr(ecs_f64_t, &v), &desc) != NULL);
    test_assert(v == 0.5);

    ecs_fini(world);
}

void Expr_vector_func_sum_w_i32(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t func = ecs_function(world, {
        .name = "sum",
        .return_type = ecs_id(ecs_f64_t),
        .params = {
            { "v", EcsScriptVectorType },
        },
        .vector_callbacks = {
            [EcsI32] = fn_sum_i32,
            [EcsI64] = fn_sum_i64,
            [EcsF32] = fn_sum_f32,
            [EcsF64] = fn_sum_f64
        }
    });

    test_assert(func != 0);

    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *var = ecs_script_vars_define(
        vars, "foo", ecs_i32_t);
    test_assert(var != NULL);
    *(int32_t*)var->value.ptr = 20;

    double v = 0;
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding, .vars = vars };
    test_assert(ecs_expr_run(world, "sum(foo)", 
        &ecs_value_ptr(ecs_f64_t, &v), &desc) != NULL);
    test_int(v, 20);

    ecs_script_vars_fini(vars);

    ecs_fini(world);
}

void Expr_vector_func_sum_w_i64(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t func = ecs_function(world, {
        .name = "sum",
        .return_type = ecs_id(ecs_f64_t),
        .params = {
            { "v", EcsScriptVectorType },
        },
        .vector_callbacks = {
            [EcsI32] = fn_sum_i32,
            [EcsI64] = fn_sum_i64,
            [EcsF32] = fn_sum_f32,
            [EcsF64] = fn_sum_f64
        }
    });

    test_assert(func != 0);

    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *var = ecs_script_vars_define(
        vars, "foo", ecs_i64_t);
    test_assert(var != NULL);
    *(int64_t*)var->value.ptr = 20;

    double v = 0;
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding, .vars = vars };
    test_assert(ecs_expr_run(world, "sum(foo)", 
        &ecs_value_ptr(ecs_f64_t, &v), &desc) != NULL);
    test_int(v, 20);

    ecs_script_vars_fini(vars);

    ecs_fini(world);
}

void Expr_vector_func_sum_w_int_literal_no_int_callback(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t func = ecs_function(world, {
        .name = "sum",
        .return_type = ecs_id(ecs_f64_t),
        .params = {
            { "v", EcsScriptVectorType },
        },
        .vector_callbacks = {
            [EcsF32] = fn_sum_f32,
            [EcsF64] = fn_sum_f64
        }
    });

    test_assert(func != 0);

    double v = 0;
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, "sum(20)", 
        &ecs_value_ptr(ecs_f64_t, &v), &desc) != NULL);
    test_int(v, 20);

    test_assert(ecs_expr_run(world, "sum(30)",
        &ecs_value_ptr(ecs_f64_t, &v), &desc) != NULL);
    test_int(v, 30);

    ecs_fini(world);
}

void Expr_vector_func_sum_w_i32_no_int_callback(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t func = ecs_function(world, {
        .name = "sum",
        .return_type = ecs_id(ecs_f64_t),
        .params = {
            { "v", EcsScriptVectorType },
        },
        .vector_callbacks = {
            [EcsF32] = fn_sum_f32,
            [EcsF64] = fn_sum_f64
        }
    });

    test_assert(func != 0);

    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *var = ecs_script_vars_define(
        vars, "foo", ecs_i32_t);
    test_assert(var != NULL);
    *(int32_t*)var->value.ptr = 20;

    double v = 0;
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding, .vars = vars };
    test_assert(ecs_expr_run(world, "sum(foo)", 
        &ecs_value_ptr(ecs_f64_t, &v), &desc) != NULL);
    test_int(v, 20);

    ecs_script_vars_fini(vars);

    ecs_fini(world);
}

void Expr_vector_func_sum_w_i64_no_int_callback(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t func = ecs_function(world, {
        .name = "sum",
        .return_type = ecs_id(ecs_f64_t),
        .params = {
            { "v", EcsScriptVectorType },
        },
        .vector_callbacks = {
            [EcsF32] = fn_sum_f32,
            [EcsF64] = fn_sum_f64
        }
    });

    test_assert(func != 0);

    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *var = ecs_script_vars_define(
        vars, "foo", ecs_i64_t);
    test_assert(var != NULL);
    *(int64_t*)var->value.ptr = 20;

    double v = 0;
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding, .vars = vars };
    test_assert(ecs_expr_run(world, "sum(foo)", 
        &ecs_value_ptr(ecs_f64_t, &v), &desc) != NULL);
    test_int(v, 20);

    ecs_script_vars_fini(vars);

    ecs_fini(world);
}

void Expr_vector_func_sum_w_struct(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t func = ecs_function(world, {
        .name = "sum",
        .return_type = ecs_id(ecs_f64_t),
        .params = {
            { "v", EcsScriptVectorType },
        },
        .vector_callbacks = {
            [EcsI32] = fn_sum_i32,
            [EcsI64] = fn_sum_i64,
            [EcsF32] = fn_sum_f32,
            [EcsF64] = fn_sum_f64
        }
    });

    test_assert(func != 0);

    ecs_entity_t ecs_id(Position) = ecs_struct(world, {
        .members = {
            { "x", ecs_id(ecs_f32_t) },
            { "y", ecs_id(ecs_f32_t) }
        }
    });

    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *var = ecs_script_vars_define(
        vars, "foo", Position);
    test_assert(var != NULL);
    ((Position*)var->value.ptr)->x = 10;
    ((Position*)var->value.ptr)->y = 20;

    double v = 0;
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding, .vars = vars };
    test_assert(ecs_expr_run(world, "sum(foo)", 
        &ecs_value_ptr(ecs_f64_t, &v), &desc) != NULL);
    test_int(v, 30);

    ecs_script_vars_fini(vars);

    ecs_fini(world);
}

void Expr_vector_func_sum_w_array(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t func = ecs_function(world, {
        .name = "sum",
        .return_type = ecs_id(ecs_f64_t),
        .params = {
            { "v", EcsScriptVectorType },
        },
        .vector_callbacks = {
            [EcsI32] = fn_sum_i32,
            [EcsI64] = fn_sum_i64,
            [EcsF32] = fn_sum_f32,
            [EcsF64] = fn_sum_f64
        }
    });

    test_assert(func != 0);

    ecs_entity_t ecs_id(IntVec) = ecs_array(world, {
        .type = ecs_id(ecs_i32_t),
        .count = 3
    });

    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *var = ecs_script_vars_define(
        vars, "foo", IntVec);
    test_assert(var != NULL);
    int32_t *array = var->value.ptr;
    array[0] = 10;
    array[1] = 20;
    array[2] = 30;

    double v = 0;
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding, .vars = vars };
    test_assert(ecs_expr_run(world, "sum(foo)", 
        &ecs_value_ptr(ecs_f64_t, &v), &desc) != NULL);
    test_int(v, 60);

    ecs_script_vars_fini(vars);

    ecs_fini(world);
}

void Expr_vector_func_sum_w_vector(void) {
    test_quarantine("2 Feb 2026");

    ecs_world_t *world = ecs_init();

    ecs_entity_t func = ecs_function(world, {
        .name = "sum",
        .return_type = ecs_id(ecs_f64_t),
        .params = {
            { "v", EcsScriptVectorType },
        },
        .vector_callbacks = {
            [EcsI32] = fn_sum_i32,
            [EcsI64] = fn_sum_i64,
            [EcsF32] = fn_sum_f32,
            [EcsF64] = fn_sum_f64
        }
    });

    test_assert(func != 0);

    ecs_entity_t ecs_id(IntVec) = ecs_vector(world, {
        .type = ecs_id(ecs_i32_t)
    });

    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *var = ecs_script_vars_define(
        vars, "foo", IntVec);
    test_assert(var != NULL);
    ecs_vec_t *vec = var->value.ptr;
    ecs_vec_init_t(NULL, vec, int32_t, 3);
    ecs_vec_set_count_t(NULL, vec, int32_t, 3);
    int32_t *array = ecs_vec_first(vec);
    array[0] = 10;
    array[1] = 20;
    array[2] = 30;

    double v = 0;
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding, .vars = vars };
    test_assert(ecs_expr_run(world, "sum(foo)", 
        &ecs_value_ptr(ecs_f64_t, &v), &desc) != NULL);
    test_int(v, 60);

    ecs_script_vars_fini(vars);

    ecs_fini(world);
}

static
void fn_add_i32(
    const ecs_function_ctx_t *ctx,
    int32_t argc,
    const ecs_value_t *argv,
    ecs_value_t *result,
    int32_t elem_count)
{
    int32_t *a = argv[0].ptr;
    int32_t *b = argv[1].ptr;
    int32_t *r = result->ptr;

    for (int32_t i = 0; i < elem_count; i ++) {
        r[i] = a[i] + b[i];
    }
}

static
void fn_add_i64(
    const ecs_function_ctx_t *ctx,
    int32_t argc,
    const ecs_value_t *argv,
    ecs_value_t *result,
    int32_t elem_count)
{
    int64_t *a = argv[0].ptr;
    int64_t *b = argv[1].ptr;
    int64_t *r = result->ptr;

    for (int32_t i = 0; i < elem_count; i ++) {
        r[i] = a[i] + b[i];
    }
}

static
void fn_add_f32(
    const ecs_function_ctx_t *ctx,
    int32_t argc,
    const ecs_value_t *argv,
    ecs_value_t *result,
    int32_t elem_count)
{
    float *a = argv[0].ptr;
    float *b = argv[1].ptr;
    float *r = result->ptr;

    for (int32_t i = 0; i < elem_count; i ++) {
        r[i] = a[i] + b[i];
    }
}

static
void fn_add_f64(
    const ecs_function_ctx_t *ctx,
    int32_t argc,
    const ecs_value_t *argv,
    ecs_value_t *result,
    int32_t elem_count)
{
    double *a = argv[0].ptr;
    double *b = argv[1].ptr;
    double *r = result->ptr;

    for (int32_t i = 0; i < elem_count; i ++) {
        r[i] = a[i] + b[i];
    }
}

void Expr_vector_func_add_w_int_literal(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t func = ecs_function(world, {
        .name = "add",
        .return_type = EcsScriptVectorType,
        .params = {
            { "a", EcsScriptVectorType },
            { "b", EcsScriptVectorType },
        },
        .vector_callbacks = {
            [EcsI32] = fn_add_i32,
            [EcsI64] = fn_add_i64,
            [EcsF32] = fn_add_f32,
            [EcsF64] = fn_add_f64
        }
    });

    test_assert(func != 0);

    ecs_value_t v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, "add(10, 20)", &v, &desc) != NULL);
    test_uint(v.type, ecs_id(ecs_i64_t));
    test_int(*(int64_t*)v.ptr, 30);

    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_vector_func_add_w_float_literal(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t func = ecs_function(world, {
        .name = "add",
        .return_type = EcsScriptVectorType,
        .params = {
            { "a", EcsScriptVectorType },
            { "b", EcsScriptVectorType },
        },
        .vector_callbacks = {
            [EcsI32] = fn_add_i32,
            [EcsI64] = fn_add_i64,
            [EcsF32] = fn_add_f32,
            [EcsF64] = fn_add_f64
        }
    });

    test_assert(func != 0);

    ecs_value_t v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, "add(1.5, 2.5)", &v, &desc) != NULL);
    test_uint(v.type, ecs_id(ecs_f64_t));
    test_int(*(double*)v.ptr, 4);

    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_vector_func_add_w_i32_w_literal(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t func = ecs_function(world, {
        .name = "add",
        .return_type = EcsScriptVectorType,
        .params = {
            { "a", EcsScriptVectorType },
            { "b", EcsScriptVectorType },
        },
        .vector_callbacks = {
            [EcsI32] = fn_add_i32,
            [EcsI64] = fn_add_i64,
            [EcsF32] = fn_add_f32,
            [EcsF64] = fn_add_f64
        }
    });

    test_assert(func != 0);

    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *foo_var = ecs_script_vars_define(
        vars, "foo", ecs_i32_t);
    test_assert(foo_var != NULL);
    *(int32_t*)foo_var->value.ptr = 10;

    ecs_value_t v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding, .vars = vars };
    test_assert(ecs_expr_run(world, "add(foo, 20)", &v, &desc) != NULL);
    test_uint(v.type, ecs_id(ecs_i32_t));
    test_int(*(int32_t*)v.ptr, 30);

    ecs_value_free(world, v.type, v.ptr);

    ecs_script_vars_fini(vars);

    ecs_fini(world);
}

void Expr_vector_func_add_w_i64_w_literal(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t func = ecs_function(world, {
        .name = "add",
        .return_type = EcsScriptVectorType,
        .params = {
            { "a", EcsScriptVectorType },
            { "b", EcsScriptVectorType },
        },
        .vector_callbacks = {
            [EcsI32] = fn_add_i32,
            [EcsI64] = fn_add_i64,
            [EcsF32] = fn_add_f32,
            [EcsF64] = fn_add_f64
        }
    });

    test_assert(func != 0);

    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *foo_var = ecs_script_vars_define(
        vars, "foo", ecs_i64_t);
    test_assert(foo_var != NULL);
    *(int64_t*)foo_var->value.ptr = 10;

    ecs_value_t v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding, .vars = vars };
    test_assert(ecs_expr_run(world, "add(foo, 20)", &v, &desc) != NULL);
    test_uint(v.type, ecs_id(ecs_i64_t));
    test_int(*(int64_t*)v.ptr, 30);

    ecs_value_free(world, v.type, v.ptr);

    ecs_script_vars_fini(vars);

    ecs_fini(world);
}

void Expr_vector_func_add_w_i32(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t func = ecs_function(world, {
        .name = "add",
        .return_type = EcsScriptVectorType,
        .params = {
            { "a", EcsScriptVectorType },
            { "b", EcsScriptVectorType },
        },
        .vector_callbacks = {
            [EcsI32] = fn_add_i32,
            [EcsI64] = fn_add_i64,
            [EcsF32] = fn_add_f32,
            [EcsF64] = fn_add_f64
        }
    });

    test_assert(func != 0);

    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *foo_var = ecs_script_vars_define(
        vars, "foo", ecs_i32_t);
    test_assert(foo_var != NULL);
    *(int32_t*)foo_var->value.ptr = 10;

    ecs_script_var_t *bar_var = ecs_script_vars_define(
        vars, "bar", ecs_i32_t);
    test_assert(bar_var != NULL);
    *(int32_t*)bar_var->value.ptr = 20;

    ecs_value_t v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding, .vars = vars };
    test_assert(ecs_expr_run(world, "add(foo, bar)", &v, &desc) != NULL);
    test_uint(v.type, ecs_id(ecs_i32_t));
    test_int(*(int32_t*)v.ptr, 30);

    ecs_value_free(world, v.type, v.ptr);

    ecs_script_vars_fini(vars);

    ecs_fini(world);
}

void Expr_vector_func_add_w_i64(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t func = ecs_function(world, {
        .name = "add",
        .return_type = EcsScriptVectorType,
        .params = {
            { "a", EcsScriptVectorType },
            { "b", EcsScriptVectorType },
        },
        .vector_callbacks = {
            [EcsI32] = fn_add_i32,
            [EcsI64] = fn_add_i64,
            [EcsF32] = fn_add_f32,
            [EcsF64] = fn_add_f64
        }
    });

    test_assert(func != 0);

    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *foo_var = ecs_script_vars_define(
        vars, "foo", ecs_i64_t);
    test_assert(foo_var != NULL);
    *(int64_t*)foo_var->value.ptr = 10;

    ecs_script_var_t *bar_var = ecs_script_vars_define(
        vars, "bar", ecs_i64_t);
    test_assert(bar_var != NULL);
    *(int64_t*)bar_var->value.ptr = 20;

    ecs_value_t v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding, .vars = vars };
    test_assert(ecs_expr_run(world, "add(foo, bar)", &v, &desc) != NULL);
    test_uint(v.type, ecs_id(ecs_i64_t));
    test_int(*(int64_t*)v.ptr, 30);

    ecs_value_free(world, v.type, v.ptr);

    ecs_script_vars_fini(vars);

    ecs_fini(world);
}

void Expr_vector_func_add_w_f32_w_literal(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t func = ecs_function(world, {
        .name = "add",
        .return_type = EcsScriptVectorType,
        .params = {
            { "a", EcsScriptVectorType },
            { "b", EcsScriptVectorType },
        },
        .vector_callbacks = {
            [EcsI32] = fn_add_i32,
            [EcsI64] = fn_add_i64,
            [EcsF32] = fn_add_f32,
            [EcsF64] = fn_add_f64
        }
    });

    test_assert(func != 0);

    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *foo_var = ecs_script_vars_define(
        vars, "foo", ecs_f32_t);
    test_assert(foo_var != NULL);
    *(ecs_f32_t*)foo_var->value.ptr = 1.5;

    ecs_value_t v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding, .vars = vars };
    test_assert(ecs_expr_run(world, "add(foo, 0.5)", &v, &desc) != NULL);
    test_uint(v.type, ecs_id(ecs_f32_t));
    test_int(*(ecs_f32_t*)v.ptr, 2);

    ecs_value_free(world, v.type, v.ptr);

    ecs_script_vars_fini(vars);

    ecs_fini(world);
}

void Expr_vector_func_add_w_f64_w_literal(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t func = ecs_function(world, {
        .name = "add",
        .return_type = EcsScriptVectorType,
        .params = {
            { "a", EcsScriptVectorType },
            { "b", EcsScriptVectorType },
        },
        .vector_callbacks = {
            [EcsI32] = fn_add_i32,
            [EcsI64] = fn_add_i64,
            [EcsF32] = fn_add_f32,
            [EcsF64] = fn_add_f64
        }
    });

    test_assert(func != 0);

    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *foo_var = ecs_script_vars_define(
        vars, "foo", ecs_f64_t);
    test_assert(foo_var != NULL);
    *(ecs_f64_t*)foo_var->value.ptr = 1.5;

    ecs_value_t v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding, .vars = vars };
    test_assert(ecs_expr_run(world, "add(foo, 0.5)", &v, &desc) != NULL);
    test_uint(v.type, ecs_id(ecs_f64_t));
    test_int(*(ecs_f64_t*)v.ptr, 2);

    ecs_value_free(world, v.type, v.ptr);

    ecs_script_vars_fini(vars);

    ecs_fini(world);
}

void Expr_vector_func_add_w_struct(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t func = ecs_function(world, {
        .name = "add",
        .return_type = EcsScriptVectorType,
        .params = {
            { "a", EcsScriptVectorType },
            { "b", EcsScriptVectorType },
        },
        .vector_callbacks = {
            [EcsI32] = fn_add_i32,
            [EcsI64] = fn_add_i64,
            [EcsF32] = fn_add_f32,
            [EcsF64] = fn_add_f64
        }
    });

    test_assert(func != 0);

    ecs_entity_t ecs_id(Position) = ecs_struct(world, {
        .members = {
            { "x", ecs_id(ecs_f32_t) },
            { "y", ecs_id(ecs_f32_t) }
        }
    });

    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *foo = ecs_script_vars_define(
        vars, "foo", Position);
    test_assert(foo != NULL);
    ((Position*)foo->value.ptr)->x = 10;
    ((Position*)foo->value.ptr)->y = 20;

    ecs_script_var_t *bar = ecs_script_vars_define(
        vars, "bar", Position);
    test_assert(bar != NULL);
    ((Position*)bar->value.ptr)->x = 30;
    ((Position*)bar->value.ptr)->y = 40;

    ecs_value_t v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding, .vars = vars };
    test_assert(ecs_expr_run(world, "add(foo, bar)", &v, &desc) != NULL);
    test_uint(v.type, ecs_id(Position));
    test_int(((Position*)v.ptr)->x, 40);
    test_int(((Position*)v.ptr)->y, 60);

    ecs_value_free(world, v.type, v.ptr);

    ecs_script_vars_fini(vars);

    ecs_fini(world);
}

void Expr_vector_add_struct_int_literal(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Position" }),
        .members = {
            { "x", ecs_id(ecs_f32_t) },
            { "y", ecs_id(ecs_f32_t) }
        }
    });

    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *foo = ecs_script_vars_define(
        vars, "foo", Position);
    test_assert(foo != NULL);
    ((Position*)foo->value.ptr)->x = 10;
    ((Position*)foo->value.ptr)->y = 20;

    ecs_value_t v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding, .vars = vars };
    test_assert(ecs_expr_run(world, "foo + 2", &v, &desc) != NULL);
    test_uint(v.type, ecs_id(Position));
    test_int(((Position*)v.ptr)->x, 12);
    test_int(((Position*)v.ptr)->y, 22);

    ecs_value_free(world, v.type, v.ptr);

    ecs_script_vars_fini(vars);

    ecs_fini(world);
}

void Expr_vector_add_struct_float_literal(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Position" }),
        .members = {
            { "x", ecs_id(ecs_f32_t) },
            { "y", ecs_id(ecs_f32_t) }
        }
    });

    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *foo = ecs_script_vars_define(
        vars, "foo", Position);
    test_assert(foo != NULL);
    ((Position*)foo->value.ptr)->x = 10;
    ((Position*)foo->value.ptr)->y = 20;

    ecs_value_t v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding, .vars = vars };
    test_assert(ecs_expr_run(world, "foo + 0.5", &v, &desc) != NULL);
    test_uint(v.type, ecs_id(Position));
    test_int(((Position*)v.ptr)->x * 10, 105);
    test_int(((Position*)v.ptr)->y * 10, 205);

    ecs_value_free(world, v.type, v.ptr);

    ecs_script_vars_fini(vars);

    ecs_fini(world);
}

void Expr_vector_add_struct_i32(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Position" }),
        .members = {
            { "x", ecs_id(ecs_f32_t) },
            { "y", ecs_id(ecs_f32_t) }
        }
    });

    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *foo = ecs_script_vars_define(
        vars, "foo", Position);
    test_assert(foo != NULL);
    ((Position*)foo->value.ptr)->x = 10;
    ((Position*)foo->value.ptr)->y = 20;

    ecs_script_var_t *foo_var = ecs_script_vars_define(
        vars, "bar", ecs_i32_t);
    test_assert(foo_var != NULL);
    *(ecs_i32_t*)foo_var->value.ptr = 2;

    ecs_value_t v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding, .vars = vars };
    test_assert(ecs_expr_run(world, "foo + bar", &v, &desc) != NULL);
    test_uint(v.type, ecs_id(Position));
    test_int(((Position*)v.ptr)->x, 12);
    test_int(((Position*)v.ptr)->y, 22);

    ecs_value_free(world, v.type, v.ptr);

    ecs_script_vars_fini(vars);

    ecs_fini(world);
}

void Expr_vector_add_struct_i64(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Position" }),
        .members = {
            { "x", ecs_id(ecs_f32_t) },
            { "y", ecs_id(ecs_f32_t) }
        }
    });

    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *foo = ecs_script_vars_define(
        vars, "foo", Position);
    test_assert(foo != NULL);
    ((Position*)foo->value.ptr)->x = 10;
    ((Position*)foo->value.ptr)->y = 20;

    ecs_script_var_t *foo_var = ecs_script_vars_define(
        vars, "bar", ecs_i64_t);
    test_assert(foo_var != NULL);
    *(ecs_i64_t*)foo_var->value.ptr = 2;

    ecs_value_t v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding, .vars = vars };
    test_assert(ecs_expr_run(world, "foo + bar", &v, &desc) != NULL);
    test_uint(v.type, ecs_id(Position));
    test_int(((Position*)v.ptr)->x, 12);
    test_int(((Position*)v.ptr)->y, 22);

    ecs_value_free(world, v.type, v.ptr);

    ecs_script_vars_fini(vars);

    ecs_fini(world);
}

void Expr_vector_add_struct_f32(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Position" }),
        .members = {
            { "x", ecs_id(ecs_f32_t) },
            { "y", ecs_id(ecs_f32_t) }
        }
    });

    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *foo = ecs_script_vars_define(
        vars, "foo", Position);
    test_assert(foo != NULL);
    ((Position*)foo->value.ptr)->x = 10;
    ((Position*)foo->value.ptr)->y = 20;

    ecs_script_var_t *foo_var = ecs_script_vars_define(
        vars, "bar", ecs_f32_t);
    test_assert(foo_var != NULL);
    *(ecs_f32_t*)foo_var->value.ptr = 0.5;

    ecs_value_t v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding, .vars = vars };
    test_assert(ecs_expr_run(world, "foo + bar", &v, &desc) != NULL);
    test_uint(v.type, ecs_id(Position));
    test_int(((Position*)v.ptr)->x * 10.0, 105);
    test_int(((Position*)v.ptr)->y * 10.0, 205);

    ecs_value_free(world, v.type, v.ptr);

    ecs_script_vars_fini(vars);

    ecs_fini(world);
}

void Expr_vector_add_struct_f64(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Position" }),
        .members = {
            { "x", ecs_id(ecs_f32_t) },
            { "y", ecs_id(ecs_f32_t) }
        }
    });

    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *foo = ecs_script_vars_define(
        vars, "foo", Position);
    test_assert(foo != NULL);
    ((Position*)foo->value.ptr)->x = 10;
    ((Position*)foo->value.ptr)->y = 20;

    ecs_script_var_t *foo_var = ecs_script_vars_define(
        vars, "bar", ecs_f64_t);
    test_assert(foo_var != NULL);
    *(ecs_f64_t*)foo_var->value.ptr = 0.5;

    ecs_value_t v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding, .vars = vars };
    test_assert(ecs_expr_run(world, "foo + bar", &v, &desc) != NULL);
    test_uint(v.type, ecs_id(Position));
    test_int(((Position*)v.ptr)->x * 10.0, 105);
    test_int(((Position*)v.ptr)->y * 10.0, 205);

    ecs_value_free(world, v.type, v.ptr);

    ecs_script_vars_fini(vars);

    ecs_fini(world);
}

void Expr_interpolate_string_w_i32_var(void) {
    ecs_world_t *world = ecs_init();

    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *v = ecs_script_vars_define(
        vars, "foo", ecs_i32_t);
    test_assert(v != NULL);
    *(int32_t*)v->value.ptr = 10;

    char *result = ecs_script_string_interpolate(world, "$foo", vars);
    test_assert(result != NULL);
    test_str(result, "10");
    ecs_os_free(result);

    ecs_script_vars_fini(vars);

    ecs_fini(world);
}

void Expr_interpolate_string_w_string_var(void) {
    ecs_world_t *world = ecs_init();

    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *v = ecs_script_vars_define(
        vars, "foo", ecs_string_t);
    test_assert(v != NULL);
    *(ecs_string_t*)v->value.ptr = ecs_os_strdup("Hello World");

    char *result = ecs_script_string_interpolate(world, "$foo", vars);
    test_assert(result != NULL);
    test_str(result, "Hello World");
    ecs_os_free(result);

    ecs_script_vars_fini(vars);

    ecs_fini(world);
}

void Expr_interpolate_string_w_entity_var(void) {
    ecs_world_t *world = ecs_init();

    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *v = ecs_script_vars_define(
        vars, "foo", ecs_entity_t);
    test_assert(v != NULL);
    *(ecs_entity_t*)v->value.ptr = ecs_id(ecs_i32_t);

    char *result = ecs_script_string_interpolate(world, "$foo", vars);
    test_assert(result != NULL);
    test_str(result, "flecs.meta.i32");
    ecs_os_free(result);

    ecs_script_vars_fini(vars);

    ecs_fini(world);
}

void Expr_interpolate_string_w_id_var(void) {
    ecs_world_t *world = ecs_init();

    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *v = ecs_script_vars_define(
        vars, "foo", ecs_id_t);
    test_assert(v != NULL);
    *(ecs_id_t*)v->value.ptr = ecs_id(ecs_i32_t);

    char *result = ecs_script_string_interpolate(world, "$foo", vars);
    test_assert(result != NULL);
    test_str(result, "flecs.meta.i32");
    ecs_os_free(result);

    ecs_script_vars_fini(vars);

    ecs_fini(world);
}

void Expr_interpolate_string_w_var_not_found(void) {
    ecs_world_t *world = ecs_init();

    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_log_set_level(-4);
    char *result = ecs_script_string_interpolate(world, "$foo", vars);
    test_assert(result == NULL);

    ecs_script_vars_fini(vars);

    ecs_fini(world);
}

void Expr_interpolate_string_w_entity_var_0(void) {
    ecs_world_t *world = ecs_init();

    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *v = ecs_script_vars_define(
        vars, "foo", ecs_entity_t);
    test_assert(v != NULL);
    *(ecs_entity_t*)v->value.ptr = 0;

    char *result = ecs_script_string_interpolate(world, "$foo", vars);
    test_assert(result != NULL);
    test_str(result, "#0");
    ecs_os_free(result);

    ecs_script_vars_fini(vars);

    ecs_fini(world);
}

void Expr_interpolate_string_w_var_special_chars(void) {
    ecs_world_t *world = ecs_init();

    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *v = ecs_script_vars_define(
        vars, "_foo_bar_10", ecs_i32_t);
    test_assert(v != NULL);
    *(int32_t*)v->value.ptr = 10;

    char *result = ecs_script_string_interpolate(world, "$_foo_bar_10", vars);
    test_assert(result != NULL);
    test_str(result, "10");
    ecs_os_free(result);

    ecs_script_vars_fini(vars);

    ecs_fini(world);
}

void Expr_interpolate_string_w_var_before_after_text(void) {
    ecs_world_t *world = ecs_init();

    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *v = ecs_script_vars_define(
        vars, "foo", ecs_i32_t);
    test_assert(v != NULL);
    *(int32_t*)v->value.ptr = 10;

    char *result = ecs_script_string_interpolate(world, "Hello $foo World", vars);
    test_assert(result != NULL);
    test_str(result, "Hello 10 World");
    ecs_os_free(result);

    ecs_script_vars_fini(vars);

    ecs_fini(world);
}

void Expr_interpolate_string_w_curly_brackets_var(void) {
    ecs_world_t *world = ecs_init();

    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *v = ecs_script_vars_define(
        vars, "foo", ecs_i32_t);
    test_assert(v != NULL);
    *(int32_t*)v->value.ptr = 10;

    char *result = ecs_script_string_interpolate(world, "Hello {$foo}World", vars);
    test_assert(result != NULL);
    test_str(result, "Hello 10World");
    ecs_os_free(result);

    ecs_script_vars_fini(vars);

    ecs_fini(world);
}

void Expr_interpolate_string_w_curly_brackets_expr(void) {
    ecs_world_t *world = ecs_init();

    char *result = ecs_script_string_interpolate(world, "Hello {10 + 20}World", NULL);
    test_assert(result != NULL);
    test_str(result, "Hello 30World");
    ecs_os_free(result);

    ecs_fini(world);
}

void Expr_interpolate_string_w_curly_brackets_expr_w_var(void) {
    ecs_world_t *world = ecs_init();

    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *v = ecs_script_vars_define(
        vars, "foo", ecs_i32_t);
    test_assert(v != NULL);
    *(int32_t*)v->value.ptr = 10;

    char *result = ecs_script_string_interpolate(world, "Hello {$foo + 5}World", vars);
    test_assert(result != NULL);
    test_str(result, "Hello 15World");
    ecs_os_free(result);

    ecs_script_vars_fini(vars);

    ecs_fini(world);
}

void Expr_interpolate_string_w_curly_brackets_expr_w_composite_var(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, PositionI);

    ecs_struct(world, {
        .entity = ecs_id(PositionI),
        .members = {
            { .name = "x", .type = ecs_id(ecs_i32_t) },
            { .name = "y", .type = ecs_id(ecs_i32_t) }
        }
    });

    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *v = ecs_script_vars_define(
        vars, "foo", PositionI);
    test_assert(v != NULL);
    *(PositionI*)v->value.ptr = (PositionI){10, 20};

    char *result = ecs_script_string_interpolate(
        world, "Hello {$foo.x + $foo.y}World", vars);
    test_assert(result != NULL);
    test_str(result, "Hello 30World");
    ecs_os_free(result);

    ecs_script_vars_fini(vars);

    ecs_fini(world);
}

void Expr_interpolate_string_w_escape_var_operator(void) {
    ecs_world_t *world = ecs_init();

    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *v = ecs_script_vars_define(
        vars, "foo", ecs_i32_t);
    test_assert(v != NULL);
    *(int32_t*)v->value.ptr = 10;

    char *result = ecs_script_string_interpolate(world, "Hello \\$foo World", vars);
    test_assert(result != NULL);
    test_str(result, "Hello $foo World");
    ecs_os_free(result);

    ecs_script_vars_fini(vars);

    ecs_fini(world);
}

void Expr_interpolate_string_w_escape_curly_brackets(void) {
    ecs_world_t *world = ecs_init();

    char *result = ecs_script_string_interpolate(world, "Hello \\{10 + 20}World", NULL);
    test_assert(result != NULL);
    test_str(result, "Hello {10 + 20}World");
    ecs_os_free(result);

    ecs_fini(world);
}

void Expr_interpolate_string_w_func(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t parent = ecs_entity(world, { .name = "parent" });
    test_assert(parent != 0);

    ecs_entity_t child = ecs_entity(world, { .name = "parent.child" });
    test_assert(child != 0);

    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *var = ecs_script_vars_define(
        vars, "foo", ecs_entity_t);
    test_assert(var != NULL);
    *(ecs_entity_t*)var->value.ptr = child;

    char *result = ecs_script_string_interpolate(
        world, "Hello {$foo.name()} World", vars);
    test_assert(result != NULL);
    test_str(result, "Hello child World");
    ecs_os_free(result);

    ecs_script_vars_fini(vars);

    ecs_fini(world);
}

void Expr_interpolate_string_w_func_chain(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t parent = ecs_entity(world, { .name = "parent" });
    test_assert(parent != 0);
    ecs_doc_set_name(world, parent, "Parent");

    ecs_entity_t child = ecs_entity(world, { .name = "parent.child" });
    test_assert(child != 0);

    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *var = ecs_script_vars_define(
        vars, "foo", ecs_entity_t);
    test_assert(var != NULL);
    *(ecs_entity_t*)var->value.ptr = child;

    char *result = ecs_script_string_interpolate(
        world, "Hello {$foo.parent().doc_name()} World", vars);
    test_assert(result != NULL);
    test_str(result, "Hello Parent World");
    ecs_os_free(result);

    ecs_script_vars_fini(vars);

    ecs_fini(world);
}

void Expr_interpolate_in_expr_var_name(void) {
    ecs_world_t *world = ecs_init();

    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *foo = ecs_script_vars_define(
        vars, "foo", ecs_string_t);
    test_assert(foo != NULL);
    *(char**)foo->value.ptr = ecs_os_strdup("World");

    ecs_expr_eval_desc_t desc = { .vars = vars, .disable_folding = disable_folding };

    char *v = NULL;
    test_assert(NULL != ecs_expr_run(world, "\"$foo\"", 
        &ecs_value_ptr(ecs_string_t, &v), &desc));
    test_str(v, "World");
    ecs_os_free(v);

    ecs_script_vars_fini(vars);

    ecs_fini(world);
}

void Expr_interpolate_in_expr_var_name_w_pre(void) {
    ecs_world_t *world = ecs_init();

    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *foo = ecs_script_vars_define(
        vars, "foo", ecs_string_t);
    test_assert(foo != NULL);
    *(char**)foo->value.ptr = ecs_os_strdup("World");

    ecs_expr_eval_desc_t desc = { .vars = vars, .disable_folding = disable_folding };

    char *v = NULL;
    test_assert(NULL != ecs_expr_run(world, "\"Hello $foo\"", 
        &ecs_value_ptr(ecs_string_t, &v), &desc));
    test_str(v, "Hello World");
    ecs_os_free(v);

    ecs_script_vars_fini(vars);

    ecs_fini(world);
}

void Expr_interpolate_in_expr_var_name_w_post(void) {
    ecs_world_t *world = ecs_init();

    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *foo = ecs_script_vars_define(
        vars, "foo", ecs_string_t);
    test_assert(foo != NULL);
    *(char**)foo->value.ptr = ecs_os_strdup("World");

    ecs_expr_eval_desc_t desc = { .vars = vars, .disable_folding = disable_folding };

    char *v = NULL;
    test_assert(NULL != ecs_expr_run(world, "\"$foo Hello\"", 
        &ecs_value_ptr(ecs_string_t, &v), &desc));
    test_str(v, "World Hello");
    ecs_os_free(v);

    ecs_script_vars_fini(vars);

    ecs_fini(world);
}

void Expr_interpolate_in_expr_var_name_w_pre_post(void) {
    ecs_world_t *world = ecs_init();

    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *foo = ecs_script_vars_define(
        vars, "foo", ecs_string_t);
    test_assert(foo != NULL);
    *(char**)foo->value.ptr = ecs_os_strdup("World");

    ecs_expr_eval_desc_t desc = { .vars = vars, .disable_folding = disable_folding };

    char *v = NULL;
    test_assert(NULL != ecs_expr_run(world, "\"Hello $foo!\"", 
        &ecs_value_ptr(ecs_string_t, &v), &desc));
    test_str(v, "Hello World!");
    ecs_os_free(v);

    ecs_script_vars_fini(vars);

    ecs_fini(world);
}

void Expr_interpolate_in_expr_var_name_bool(void) {
    ecs_world_t *world = ecs_init();

    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *foo = ecs_script_vars_define(
        vars, "foo", ecs_bool_t);
    test_assert(foo != NULL);
    *(bool*)foo->value.ptr = true;

    ecs_expr_eval_desc_t desc = { .vars = vars, .disable_folding = disable_folding };

    char *v = NULL;
    test_assert(NULL != ecs_expr_run(world, "\"$foo\"", 
        &ecs_value_ptr(ecs_string_t, &v), &desc));
    test_str(v, "true");
    ecs_os_free(v);

    ecs_script_vars_fini(vars);

    ecs_fini(world);
}

void Expr_interpolate_in_expr_var_name_char(void) {
    ecs_world_t *world = ecs_init();

    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *foo = ecs_script_vars_define(
        vars, "foo", ecs_char_t);
    test_assert(foo != NULL);
    *(char*)foo->value.ptr = 'a';

    ecs_expr_eval_desc_t desc = { .vars = vars, .disable_folding = disable_folding };

    char *v = NULL;
    test_assert(NULL != ecs_expr_run(world, "\"$foo\"", 
        &ecs_value_ptr(ecs_string_t, &v), &desc));
    test_str(v, "a");
    ecs_os_free(v);

    ecs_script_vars_fini(vars);

    ecs_fini(world);
}

void Expr_interpolate_in_expr_var_name_i32(void) {
    ecs_world_t *world = ecs_init();

    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *foo = ecs_script_vars_define(
        vars, "foo", ecs_i32_t);
    test_assert(foo != NULL);
    *(int32_t*)foo->value.ptr = 10;

    ecs_expr_eval_desc_t desc = { .vars = vars, .disable_folding = disable_folding };

    char *v = NULL;
    test_assert(NULL != ecs_expr_run(world, "\"$foo\"", 
        &ecs_value_ptr(ecs_string_t, &v), &desc));
    test_str(v, "10");
    ecs_os_free(v);

    ecs_script_vars_fini(vars);

    ecs_fini(world);
}

void Expr_interpolate_in_expr_var_name_u32(void) {
    ecs_world_t *world = ecs_init();

    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *foo = ecs_script_vars_define(
        vars, "foo", ecs_u32_t);
    test_assert(foo != NULL);
    *(ecs_u32_t*)foo->value.ptr = 10;

    ecs_expr_eval_desc_t desc = { .vars = vars, .disable_folding = disable_folding };

    char *v = NULL;
    test_assert(NULL != ecs_expr_run(world, "\"$foo\"", 
        &ecs_value_ptr(ecs_string_t, &v), &desc));
    test_str(v, "10");
    ecs_os_free(v);

    ecs_script_vars_fini(vars);

    ecs_fini(world);
}

void Expr_interpolate_in_expr_var_name_f32(void) {
    ecs_world_t *world = ecs_init();

    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *foo = ecs_script_vars_define(
        vars, "foo", ecs_f32_t);
    test_assert(foo != NULL);
    *(ecs_f32_t*)foo->value.ptr = 10.5;

    ecs_expr_eval_desc_t desc = { .vars = vars, .disable_folding = disable_folding };

    char *v = NULL;
    test_assert(NULL != ecs_expr_run(world, "\"$foo\"", 
        &ecs_value_ptr(ecs_string_t, &v), &desc));
    test_str(v, "10.500000");
    ecs_os_free(v);

    ecs_script_vars_fini(vars);

    ecs_fini(world);
}

void Expr_interpolate_in_expr_var_name_entity(void) {
    ecs_world_t *world = ecs_init();

    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *foo = ecs_script_vars_define(
        vars, "foo", ecs_entity_t);
    test_assert(foo != NULL);
    *(ecs_entity_t*)foo->value.ptr = EcsFlecsCore;

    ecs_expr_eval_desc_t desc = { .vars = vars, .disable_folding = disable_folding };

    char *v = NULL;
    test_assert(NULL != ecs_expr_run(world, "\"$foo\"", 
        &ecs_value_ptr(ecs_string_t, &v), &desc));
    test_str(v, "flecs.core");
    ecs_os_free(v);

    ecs_script_vars_fini(vars);

    ecs_fini(world);
}

void Expr_interpolate_in_expr_w_curly_brackets(void) {
    ecs_world_t *world = ecs_init();

    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };

    char *v = NULL;
    test_assert(NULL != ecs_expr_run(world, "\"{10 + 20}\"", 
        &ecs_value_ptr(ecs_string_t, &v), &desc));
    test_str(v, "30");
    ecs_os_free(v);

    ecs_fini(world);
}

void Expr_interpolate_in_expr_w_curly_brackets_w_var(void) {
    ecs_world_t *world = ecs_init();

    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *foo = ecs_script_vars_define(
        vars, "foo", ecs_i32_t);
    test_assert(foo != NULL);
    *(int32_t*)foo->value.ptr = 10;

    ecs_expr_eval_desc_t desc = { .vars = vars, .disable_folding = disable_folding };

    char *v = NULL;
    test_assert(NULL != ecs_expr_run(world, "\"{$foo * 2}\"", 
        &ecs_value_ptr(ecs_string_t, &v), &desc));
    test_str(v, "20");
    ecs_os_free(v);

    ecs_script_vars_fini(vars);

    ecs_fini(world);
}

void Expr_iter_to_vars_no_data(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Foo);

    ecs_query_t *query = ecs_query(world, { .expr = "Foo" });
    test_assert(query != NULL);

    ecs_entity_t e1 = ecs_new_w(world, Foo);
    ecs_entity_t e2 = ecs_new_w(world, Foo);

    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_iter_t it = ecs_query_iter(world, query);
    test_bool(ecs_query_next(&it), true);
    test_int(it.count, 2);
    test_uint(it.entities[0], e1);
    test_uint(it.entities[1], e2);

    ecs_script_vars_from_iter(&it, vars, 0);
    {
        ecs_script_var_t *v = ecs_script_vars_lookup(vars, "this");
        test_assert(v != NULL);
        test_assert(v->value.type == ecs_id(ecs_entity_t));
        test_assert(v->value.ptr != NULL);
        test_uint(*(ecs_entity_t*)v->value.ptr, e1);
    }

    ecs_script_vars_from_iter(&it, vars, 1);
    {
        ecs_script_var_t *v = ecs_script_vars_lookup(vars, "this");
        test_assert(v != NULL);
        test_assert(v->value.type == ecs_id(ecs_entity_t));
        test_assert(v->value.ptr != NULL);
        test_uint(*(ecs_entity_t*)v->value.ptr, e2);
    }

    test_bool(ecs_query_next(&it), false);

    ecs_script_vars_fini(vars);
    ecs_query_fini(query);

    ecs_fini(world);
}

void Expr_iter_to_vars_1_comp(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, PositionI);

    ecs_query_t *query = ecs_query(world, { .expr = "PositionI" });
    test_assert(query != NULL);

    ecs_entity_t e1 = ecs_new_w(world, PositionI);
    ecs_entity_t e2 = ecs_new_w(world, PositionI);

    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_iter_t it = ecs_query_iter(world, query);
    test_bool(ecs_query_next(&it), true);
    test_int(it.count, 2);
    test_uint(it.entities[0], e1);
    test_uint(it.entities[1], e2);

    ecs_script_vars_from_iter(&it, vars, 0);
    {
        ecs_script_var_t *v = ecs_script_vars_lookup(vars, "this");
        test_assert(v != NULL);
        test_assert(v->value.type == ecs_id(ecs_entity_t));
        test_assert(v->value.ptr != NULL);
        test_uint(*(ecs_entity_t*)v->value.ptr, e1);
    }
    {
        ecs_script_var_t *v = ecs_script_vars_lookup(vars, "0");
        test_assert(v != NULL);
        test_assert(v->value.type == ecs_id(PositionI));
        test_assert(v->value.ptr != NULL);
        test_assert(v->value.ptr == ecs_get(world, e1, PositionI));
    }

    ecs_script_vars_from_iter(&it, vars, 1);
    {
        ecs_script_var_t *v = ecs_script_vars_lookup(vars, "this");
        test_assert(v != NULL);
        test_assert(v->value.type == ecs_id(ecs_entity_t));
        test_assert(v->value.ptr != NULL);
        test_uint(*(ecs_entity_t*)v->value.ptr, e2);
    }
    {
        ecs_script_var_t *v = ecs_script_vars_lookup(vars, "0");
        test_assert(v != NULL);
        test_assert(v->value.type == ecs_id(PositionI));
        test_assert(v->value.ptr != NULL);
        test_assert(v->value.ptr == ecs_get(world, e2, PositionI));
    }

    test_bool(ecs_query_next(&it), false);

    ecs_script_vars_fini(vars);
    ecs_query_fini(query);

    ecs_fini(world);
}

void Expr_iter_to_vars_2_comps(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, PositionI);
    ECS_COMPONENT(world, Velocity);

    ecs_query_t *query = ecs_query(world, { .expr = "PositionI, Velocity" });
    test_assert(query != NULL);

    ecs_entity_t e1 = ecs_new_w(world, PositionI);
    ecs_entity_t e2 = ecs_new_w(world, PositionI);
    ecs_add(world, e1, Velocity);
    ecs_add(world, e2, Velocity);

    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_iter_t it = ecs_query_iter(world, query);
    test_bool(ecs_query_next(&it), true);
    test_int(it.count, 2);
    test_uint(it.entities[0], e1);
    test_uint(it.entities[1], e2);

    ecs_script_vars_from_iter(&it, vars, 0);
    {
        ecs_script_var_t *v = ecs_script_vars_lookup(vars, "this");
        test_assert(v != NULL);
        test_assert(v->value.type == ecs_id(ecs_entity_t));
        test_assert(v->value.ptr != NULL);
        test_uint(*(ecs_entity_t*)v->value.ptr, e1);
    }
    {
        ecs_script_var_t *v = ecs_script_vars_lookup(vars, "0");
        test_assert(v != NULL);
        test_assert(v->value.type == ecs_id(PositionI));
        test_assert(v->value.ptr != NULL);
        test_assert(v->value.ptr == ecs_get(world, e1, PositionI));
    }
    {
        ecs_script_var_t *v = ecs_script_vars_lookup(vars, "1");
        test_assert(v != NULL);
        test_assert(v->value.type == ecs_id(Velocity));
        test_assert(v->value.ptr != NULL);
        test_assert(v->value.ptr == ecs_get(world, e1, Velocity));
    }

    ecs_script_vars_from_iter(&it, vars, 1);
    {
        ecs_script_var_t *v = ecs_script_vars_lookup(vars, "this");
        test_assert(v != NULL);
        test_assert(v->value.type == ecs_id(ecs_entity_t));
        test_assert(v->value.ptr != NULL);
        test_uint(*(ecs_entity_t*)v->value.ptr, e2);
    }
    {
        ecs_script_var_t *v = ecs_script_vars_lookup(vars, "0");
        test_assert(v != NULL);
        test_assert(v->value.type == ecs_id(PositionI));
        test_assert(v->value.ptr != NULL);
        test_assert(v->value.ptr == ecs_get(world, e2, PositionI));
    }
    {
        ecs_script_var_t *v = ecs_script_vars_lookup(vars, "1");
        test_assert(v != NULL);
        test_assert(v->value.type == ecs_id(Velocity));
        test_assert(v->value.ptr != NULL);
        test_assert(v->value.ptr == ecs_get(world, e2, Velocity));
    }

    test_bool(ecs_query_next(&it), false);

    ecs_script_vars_fini(vars);
    ecs_query_fini(query);

    ecs_fini(world);
}

void Expr_iter_to_vars_1_comp_1_tag(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, PositionI);
    ECS_COMPONENT(world, Velocity);
    ECS_TAG(world, Foo);

    ecs_query_t *query = ecs_query(world, { .expr = "PositionI, Foo, Velocity" });
    test_assert(query != NULL);

    ecs_entity_t e1 = ecs_new_w(world, PositionI);
    ecs_entity_t e2 = ecs_new_w(world, PositionI);
    ecs_add(world, e1, Velocity);
    ecs_add(world, e2, Velocity);
    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);

    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_iter_t it = ecs_query_iter(world, query);
    test_bool(ecs_query_next(&it), true);
    test_int(it.count, 2);
    test_uint(it.entities[0], e1);
    test_uint(it.entities[1], e2);

    ecs_script_vars_from_iter(&it, vars, 0);
    {
        ecs_script_var_t *v = ecs_script_vars_lookup(vars, "this");
        test_assert(v != NULL);
        test_assert(v->value.type == ecs_id(ecs_entity_t));
        test_assert(v->value.ptr != NULL);
        test_uint(*(ecs_entity_t*)v->value.ptr, e1);
    }
    {
        ecs_script_var_t *v = ecs_script_vars_lookup(vars, "0");
        test_assert(v != NULL);
        test_assert(v->value.type == ecs_id(PositionI));
        test_assert(v->value.ptr != NULL);
        test_assert(v->value.ptr == ecs_get(world, e1, PositionI));
    }
    test_assert(ecs_script_vars_lookup(vars, "1") == NULL);
    {
        ecs_script_var_t *v = ecs_script_vars_lookup(vars, "2");
        test_assert(v != NULL);
        test_assert(v->value.type == ecs_id(Velocity));
        test_assert(v->value.ptr != NULL);
        test_assert(v->value.ptr == ecs_get(world, e1, Velocity));
    }

    ecs_script_vars_from_iter(&it, vars, 1);
    {
        ecs_script_var_t *v = ecs_script_vars_lookup(vars, "this");
        test_assert(v != NULL);
        test_assert(v->value.type == ecs_id(ecs_entity_t));
        test_assert(v->value.ptr != NULL);
        test_uint(*(ecs_entity_t*)v->value.ptr, e2);
    }
    {
        ecs_script_var_t *v = ecs_script_vars_lookup(vars, "0");
        test_assert(v != NULL);
        test_assert(v->value.type == ecs_id(PositionI));
        test_assert(v->value.ptr != NULL);
        test_assert(v->value.ptr == ecs_get(world, e2, PositionI));
    }
    test_assert(ecs_script_vars_lookup(vars, "1") == NULL);
    {
        ecs_script_var_t *v = ecs_script_vars_lookup(vars, "2");
        test_assert(v != NULL);
        test_assert(v->value.type == ecs_id(Velocity));
        test_assert(v->value.ptr != NULL);
        test_assert(v->value.ptr == ecs_get(world, e2, Velocity));
    }

    test_bool(ecs_query_next(&it), false);

    ecs_script_vars_fini(vars);
    ecs_query_fini(query);

    ecs_fini(world);
}

void Expr_iter_to_vars_w_1_query_var(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, PositionI);

    ecs_query_t *query = ecs_query(world, { .expr = "PositionI($x)" });
    test_assert(query != NULL);

    ecs_entity_t e1 = ecs_new_w(world, PositionI);
    ecs_entity_t e2 = ecs_new_w(world, PositionI);

    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_iter_t it = ecs_query_iter(world, query);
    test_bool(ecs_query_next(&it), true);
    test_int(it.count, 0);

    ecs_script_vars_from_iter(&it, vars, 0);
    test_assert(ecs_script_vars_lookup(vars, "this") == NULL);
    {
        ecs_script_var_t *v = ecs_script_vars_lookup(vars, "x");
        test_assert(v != NULL);
        test_assert(v->value.type == ecs_id(ecs_entity_t));
        test_assert(v->value.ptr != NULL);
        test_assert(*(ecs_entity_t*)v->value.ptr == e1);
    }
    {
        ecs_script_var_t *v = ecs_script_vars_lookup(vars, "0");
        test_assert(v != NULL);
        test_assert(v->value.type == ecs_id(PositionI));
        test_assert(v->value.ptr != NULL);
        test_assert(v->value.ptr == ecs_get(world, e1, PositionI));
    }

    test_bool(ecs_query_next(&it), true);
    test_int(it.count, 0);

    ecs_script_vars_from_iter(&it, vars, 0);
    test_assert(ecs_script_vars_lookup(vars, "this") == NULL);
    {
        ecs_script_var_t *v = ecs_script_vars_lookup(vars, "x");
        test_assert(v != NULL);
        test_assert(v->value.type == ecs_id(ecs_entity_t));
        test_assert(v->value.ptr != NULL);
        test_assert(*(ecs_entity_t*)v->value.ptr == e2);
    }
    {
        ecs_script_var_t *v = ecs_script_vars_lookup(vars, "0");
        test_assert(v != NULL);
        test_assert(v->value.type == ecs_id(PositionI));
        test_assert(v->value.ptr != NULL);
        test_assert(v->value.ptr == ecs_get(world, e2, PositionI));
    }

    test_bool(ecs_query_next(&it), false);

    ecs_script_vars_fini(vars);
    ecs_query_fini(query);

    ecs_fini(world);
}

void Expr_iter_to_vars_w_2_query_vars(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, PositionI);

    ecs_query_t *query = ecs_query(world, { .expr = "PositionI($x), ChildOf($x, $y)" });
    test_assert(query != NULL);

    ecs_entity_t parent = ecs_new(world);
    ecs_entity_t e1 = ecs_new_w(world, PositionI);
    ecs_entity_t e2 = ecs_new_w(world, PositionI);
    ecs_add_pair(world, e1, EcsChildOf, parent);
    ecs_add_pair(world, e2, EcsChildOf, parent);

    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_iter_t it = ecs_query_iter(world, query);
    test_bool(ecs_query_next(&it), true);
    test_int(it.count, 0);

    ecs_script_vars_from_iter(&it, vars, 0);
    test_assert(ecs_script_vars_lookup(vars, "this") == NULL);
    {
        ecs_script_var_t *v = ecs_script_vars_lookup(vars, "x");
        test_assert(v != NULL);
        test_assert(v->value.type == ecs_id(ecs_entity_t));
        test_assert(v->value.ptr != NULL);
        test_assert(*(ecs_entity_t*)v->value.ptr == e1);
    }
    {
        ecs_script_var_t *v = ecs_script_vars_lookup(vars, "y");
        test_assert(v != NULL);
        test_assert(v->value.type == ecs_id(ecs_entity_t));
        test_assert(v->value.ptr != NULL);
        test_assert(*(ecs_entity_t*)v->value.ptr == parent);
    }
    {
        ecs_script_var_t *v = ecs_script_vars_lookup(vars, "0");
        test_assert(v != NULL);
        test_assert(v->value.type == ecs_id(PositionI));
        test_assert(v->value.ptr != NULL);
        test_assert(v->value.ptr == ecs_get(world, e1, PositionI));
    }

    test_bool(ecs_query_next(&it), true);
    test_int(it.count, 0);

    ecs_script_vars_from_iter(&it, vars, 0);
    test_assert(ecs_script_vars_lookup(vars, "this") == NULL);
    {
        ecs_script_var_t *v = ecs_script_vars_lookup(vars, "x");
        test_assert(v != NULL);
        test_assert(v->value.type == ecs_id(ecs_entity_t));
        test_assert(v->value.ptr != NULL);
        test_assert(*(ecs_entity_t*)v->value.ptr == e2);
    }
    {
        ecs_script_var_t *v = ecs_script_vars_lookup(vars, "y");
        test_assert(v != NULL);
        test_assert(v->value.type == ecs_id(ecs_entity_t));
        test_assert(v->value.ptr != NULL);
        test_assert(*(ecs_entity_t*)v->value.ptr == parent);
    }
    {
        ecs_script_var_t *v = ecs_script_vars_lookup(vars, "0");
        test_assert(v != NULL);
        test_assert(v->value.type == ecs_id(PositionI));
        test_assert(v->value.ptr != NULL);
        test_assert(v->value.ptr == ecs_get(world, e2, PositionI));
    }

    test_bool(ecs_query_next(&it), false);

    ecs_script_vars_fini(vars);
    ecs_query_fini(query);

    ecs_fini(world);
}

void Expr_component_expr(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "Position"}),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    ecs_entity_t e = ecs_entity(world, { .name = "e" });
    ecs_set(world, e, Position, {10, 20});

    ecs_value_t v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, "e[Position]", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(Position));
    test_assert(v.ptr != NULL);

    Position *p = v.ptr;
    test_int(p->x, 10);
    test_int(p->y, 20);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_component_member_expr(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "Position"}),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    ecs_entity_t e = ecs_entity(world, { .name = "e" });
    ecs_set(world, e, Position, {10, 20});

    ecs_value_t v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, "e[Position].x", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_f32_t));
    test_assert(v.ptr != NULL);
    {
        float *ptr = v.ptr;
        test_int(*ptr, 10);
        ecs_value_free(world, v.type, v.ptr);
    }

    v = (ecs_value_t){0};

    test_assert(ecs_expr_run(world, "e[Position].y", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_f32_t));
    test_assert(v.ptr != NULL);
    {
        float *ptr = v.ptr;
        test_int(*ptr, 20);
        ecs_value_free(world, v.type, v.ptr);
    }

    ecs_fini(world);
}

void Expr_component_expr_string(void) {
    ecs_world_t *world = ecs_init();

    typedef struct {
        char *value;
    } String;

    ecs_entity_t ecs_id(String) = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "String" }),
        .members = {
            {"value", ecs_id(ecs_string_t)}
        }
    });

    ecs_entity_t e = ecs_entity(world, { .name = "e" });
    ecs_set(world, e, String, { "Hello World" });

    ecs_value_t v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, "e[String]", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(String));
    test_assert(v.ptr != NULL);
    {
        String *ptr = v.ptr;
        test_str(ptr->value, "Hello World");
        ecs_value_free(world, v.type, v.ptr);
    }

    const String *s = ecs_get(world, e, String);
    test_assert(s != NULL);
    test_str(s->value, "Hello World");

    ecs_fini(world);
}

void Expr_component_member_expr_string(void) {
    ecs_world_t *world = ecs_init();

    typedef struct {
        char *value;
    } String;

    ecs_entity_t ecs_id(String) = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "String" }),
        .members = {
            {"value", ecs_id(ecs_string_t)}
        }
    });

    ecs_entity_t e = ecs_entity(world, { .name = "e" });
    ecs_set(world, e, String, { "Hello World" });

    ecs_value_t v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, "e[String].value", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_string_t));
    test_assert(v.ptr != NULL);
    {
        char **ptr = v.ptr;
        test_str(*ptr, "Hello World");
        ecs_value_free(world, v.type, v.ptr);
    }

    ecs_fini(world);
}

void Expr_component_elem_expr(void) {
    ecs_world_t *world = ecs_init();

    typedef char* Strings[2];

    ecs_entity_t ecs_id(Strings) = ecs_array(world, {
        .entity = ecs_entity(world, { .name = "Strings" }),
        .type = ecs_id(ecs_string_t),
        .count = 2
    });

    ecs_entity_t e = ecs_entity(world, { .name = "e" });
    ecs_set(world, e, Strings, { "Hello", "World" });

    ecs_value_t v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, "e[Strings][0]", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_string_t));
    test_assert(v.ptr != NULL);
    {
        char **ptr = v.ptr;
        test_str(*ptr, "Hello");
        ecs_value_free(world, v.type, v.ptr);
    }

    ecs_os_zeromem(&v);

    test_assert(ecs_expr_run(world, "e[Strings][1]", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_string_t));
    test_assert(v.ptr != NULL);
    {
        char **ptr = v.ptr;
        test_str(*ptr, "World");
        ecs_value_free(world, v.type, v.ptr);
    }

    ecs_fini(world);
}

void Expr_component_elem_expr_string(void) {
    ecs_world_t *world = ecs_init();

    typedef struct {
        char *value[2];
    } String;

    ecs_entity_t arr = ecs_array(world, {
        .type = ecs_id(ecs_string_t),
        .count = 2
    });

    ecs_entity_t ecs_id(String) = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "String" }),
        .members = {
            {"value", arr}
        }
    });

    ecs_entity_t e = ecs_entity(world, { .name = "e" });
    ecs_set(world, e, String, {{ "Hello", "World" }});

    ecs_value_t v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, "e[String].value[0]", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_string_t));
    test_assert(v.ptr != NULL);
    {
        char **ptr = v.ptr;
        test_str(*ptr, "Hello");
        ecs_value_free(world, v.type, v.ptr);
    }

    ecs_os_zeromem(&v);

    test_assert(ecs_expr_run(world, "e[String].value[1]", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_string_t));
    test_assert(v.ptr != NULL);
    {
        char **ptr = v.ptr;
        test_str(*ptr, "World");
        ecs_value_free(world, v.type, v.ptr);
    }

    ecs_fini(world);
}

void Expr_component_inline_elem_expr_string(void) {
    ecs_world_t *world = ecs_init();

    typedef struct {
        char *value[2];
    } String;

    ecs_entity_t ecs_id(String) = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "String" }),
        .members = {
            {"value", ecs_id(ecs_string_t), .count = 2}
        }
    });

    ecs_entity_t e = ecs_entity(world, { .name = "e" });
    ecs_set(world, e, String, {{ "Hello", "World" }});

    ecs_value_t v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, "e[String].value[0]", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_string_t));
    test_assert(v.ptr != NULL);
    {
        char **ptr = v.ptr;
        test_str(*ptr, "Hello");
        ecs_value_free(world, v.type, v.ptr);
    }

    ecs_os_zeromem(&v);

    test_assert(ecs_expr_run(world, "e[String].value[1]", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_string_t));
    test_assert(v.ptr != NULL);
    {
        char **ptr = v.ptr;
        test_str(*ptr, "World");
        ecs_value_free(world, v.type, v.ptr);
    }

    ecs_fini(world);
}

void Expr_component_expr_in_object(void) {
    ecs_world_t *world = ecs_init();

    typedef struct {
        int32_t x;
        int32_t y;
    } Point;

    typedef struct {
        Point start;
        Point stop;
    } Line;

    ecs_entity_t ecs_id(Point) = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Point" }),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t ecs_id(Line) = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Line" }),
        .members = {
            {"start", ecs_id(Point)},
            {"stop", ecs_id(Point)}
        }
    });

    ecs_entity_t e1 = ecs_entity(world, { .name = "e1" });
    ecs_set(world, e1, Point, {10, 20});

    ecs_entity_t e2 = ecs_entity(world, { .name = "e2" });
    ecs_set(world, e2, Point, {30, 40});

    ecs_value_t v = { .type = ecs_id(Line) };
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, "{ e1[Point], e2[Point] }", &v, &desc) != NULL);

    test_assert(v.ptr != NULL);
    Line *ptr = v.ptr;
    test_int(ptr->start.x, 10);
    test_int(ptr->start.y, 20);
    test_int(ptr->stop.x, 30);
    test_int(ptr->stop.y, 40);

    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_component_member_expr_in_object(void) {
    ecs_world_t *world = ecs_init();

    typedef struct {
        int32_t x;
        int32_t y;
    } Point;

    ecs_entity_t ecs_id(Point) = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Point" }),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t e1 = ecs_entity(world, { .name = "e1" });
    ecs_set(world, e1, Point, {10, 20});

    ecs_value_t v = { .type = ecs_id(Point) };
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, "{ e1[Point].x, e1[Point].y }", &v, &desc) != NULL);

    test_assert(v.ptr != NULL);
    Point *ptr = v.ptr;
    test_int(ptr->x, 10);
    test_int(ptr->y, 20);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_var_expr(void) {
    ecs_world_t *world = ecs_init();

    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *foo = ecs_script_vars_define(vars, "foo", ecs_i32_t);
    *(int32_t*)foo->value.ptr = 10;

    int32_t v = 0;
    ecs_expr_eval_desc_t desc = { .vars = vars, .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(
        world, "$foo", &ecs_value_ptr(ecs_i32_t, &v), &desc);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == 0);
    test_int(v, 10);

    ecs_script_vars_fini(vars);

    ecs_fini(world);
}

void Expr_var_member_expr(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(PositionI) = ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "PositionI"}),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *foo = ecs_script_vars_define(vars, "foo", PositionI);
    *(PositionI*)foo->value.ptr = (PositionI){10, 20};

    int32_t v = 0;
    ecs_expr_eval_desc_t desc = { .vars = vars, .disable_folding = disable_folding };

    {
        const char *ptr = ecs_expr_run(
            world, "$foo.x", &ecs_value_ptr(ecs_i32_t, &v), &desc);
        test_assert(ptr != NULL);
        test_assert(ptr[0] == 0);
        test_int(v, 10);
    }

    {
        const char *ptr = ecs_expr_run(
            world, "$foo.y", &ecs_value_ptr(ecs_i32_t, &v), &desc);
        test_assert(ptr != NULL);
        test_assert(ptr[0] == 0);
        test_int(v, 20);
    }

    ecs_script_vars_fini(vars);

    ecs_fini(world);
}

void Expr_var_elem_expr(void) {
    ecs_world_t *world = ecs_init();

    typedef int32_t Ints[2];

    ecs_entity_t ecs_id(Ints) = ecs_array(world, {
        .entity = ecs_entity(world, { .name = "Ints" }),
        .type = ecs_id(ecs_i32_t),
        .count = 2
    });

    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *foo = ecs_script_vars_define(vars, "foo", Ints);
    Ints *var = foo->value.ptr;
    (*var)[0] = 10;
    (*var)[1] = 20;

    int32_t v = 0;
    ecs_expr_eval_desc_t desc = { .vars = vars, .disable_folding = disable_folding };

    {
        const char *ptr = ecs_expr_run(
            world, "$foo[0]", &ecs_value_ptr(ecs_i32_t, &v), &desc);
        test_assert(ptr != NULL);
        test_assert(ptr[0] == 0);
        test_int(v, 10);
    }

    {
        const char *ptr = ecs_expr_run(
            world, "$foo[1]", &ecs_value_ptr(ecs_i32_t, &v), &desc);
        test_assert(ptr != NULL);
        test_assert(ptr[0] == 0);
        test_int(v, 20);
    }

    ecs_script_vars_fini(vars);

    ecs_fini(world);
}

void Expr_var_expr_string(void) {
    ecs_world_t *world = ecs_init();

    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *foo = ecs_script_vars_define(vars, "foo", ecs_string_t);
    *(char**)foo->value.ptr = "Hello World";

    char* v = NULL;
    ecs_expr_eval_desc_t desc = { .vars = vars, .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(
        world, "$foo", &ecs_value_ptr(ecs_string_t, &v), &desc);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == 0);
    test_str(v, "Hello World");
    
    ecs_os_free(v);

    *(char**)foo->value.ptr = NULL;
    ecs_script_vars_fini(vars);

    ecs_fini(world);
}

void Expr_var_member_expr_string(void) {
    ecs_world_t *world = ecs_init();

    typedef struct {
        char *x;
        char *y;
    } Strings;

    ecs_entity_t ecs_id(Strings) = ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "Strings"}),
        .members = {
            {"x", ecs_id(ecs_string_t)},
            {"y", ecs_id(ecs_string_t)}
        }
    });

    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *foo = ecs_script_vars_define(vars, "foo", Strings);
    Strings *var = foo->value.ptr;
    var->x = "Hello";
    var->y = "World";

    char *v = 0;
    ecs_expr_eval_desc_t desc = { .vars = vars, .disable_folding = disable_folding };

    {
        const char *ptr = ecs_expr_run(
            world, "$foo.x", &ecs_value_ptr(ecs_string_t, &v), &desc);
        test_assert(ptr != NULL);
        test_assert(ptr[0] == 0);
        test_str(v, "Hello");
        ecs_os_free(v);
    }

    v = NULL;

    {
        const char *ptr = ecs_expr_run(
            world, "$foo.y", &ecs_value_ptr(ecs_string_t, &v), &desc);
        test_assert(ptr != NULL);
        test_assert(ptr[0] == 0);
        test_str(v, "World");
        ecs_os_free(v);
    }

    test_str(var->x, "Hello");
    test_str(var->y, "World");

    var->x = NULL;
    var->y = NULL;
    ecs_script_vars_fini(vars);

    ecs_fini(world);
}

void Expr_var_elem_expr_string(void) {
    ecs_world_t *world = ecs_init();

    typedef char* Strings[2];

    ecs_entity_t ecs_id(Strings) = ecs_array(world, {
        .entity = ecs_entity(world, { .name = "Strings" }),
        .type = ecs_id(ecs_string_t),
        .count = 2
    });

    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *foo = ecs_script_vars_define(vars, "foo", Strings);
    Strings *var = foo->value.ptr;
    (*var)[0] = "Hello";
    (*var)[1] = "World";

    char *v = NULL;
    ecs_expr_eval_desc_t desc = { .vars = vars, .disable_folding = disable_folding };

    {
        const char *ptr = ecs_expr_run(
            world, "$foo[0]", &ecs_value_ptr(ecs_string_t, &v), &desc);
        test_assert(ptr != NULL);
        test_assert(ptr[0] == 0);
        test_str(v, "Hello");
        ecs_os_free(v);
    }

    v = NULL;

    {
        const char *ptr = ecs_expr_run(
            world, "$foo[1]", &ecs_value_ptr(ecs_string_t, &v), &desc);
        test_assert(ptr != NULL);
        test_assert(ptr[0] == 0);
        test_str(v, "World");
        ecs_os_free(v);
    }

    test_str((*var)[0], "Hello");
    test_str((*var)[1], "World");
    (*var)[0] = NULL;
    (*var)[1] = NULL;

    ecs_script_vars_fini(vars);

    ecs_fini(world);
}

void Expr_var_inline_elem_expr_string(void) {
    ecs_world_t *world = ecs_init();

    typedef struct {
        char *value[2];
    } Strings;

    ecs_entity_t ecs_id(Strings) = ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "Strings"}),
        .members = {
            {"value", ecs_id(ecs_string_t), .count = 2},
        }
    });

    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *foo = ecs_script_vars_define(vars, "foo", Strings);
    Strings *var = foo->value.ptr;
    var->value[0] = "Hello";
    var->value[1] = "World";

    char *v = 0;
    ecs_expr_eval_desc_t desc = { .vars = vars, .disable_folding = disable_folding };

    {
        const char *ptr = ecs_expr_run(
            world, "$foo.value[0]", &ecs_value_ptr(ecs_string_t, &v), &desc);
        test_assert(ptr != NULL);
        test_assert(ptr[0] == 0);
        test_str(v, "Hello");
        ecs_os_free(v);
    }

    v = NULL;

    {
        const char *ptr = ecs_expr_run(
            world, "$foo.value[1]", &ecs_value_ptr(ecs_string_t, &v), &desc);
        test_assert(ptr != NULL);
        test_assert(ptr[0] == 0);
        test_str(v, "World");
        ecs_os_free(v);
    }

    test_str(var->value[0], "Hello");
    test_str(var->value[1], "World");

    var->value[0] = NULL;
    var->value[1] = NULL;
    ecs_script_vars_fini(vars);

    ecs_fini(world);
}

void Expr_var_expr_no_desc(void) {
    ecs_world_t *world = ecs_init();

    int32_t v = 0;

    ecs_log_set_level(-4);
    const char *ptr = ecs_expr_run(
        world, "$foo", &ecs_value_ptr(ecs_i32_t, &v), NULL);
    test_assert(ptr == NULL);

    ecs_fini(world);
}

void Expr_var_expr_desc_w_no_vars(void) {
    ecs_world_t *world = ecs_init();

    int32_t v = 0;

    ecs_log_set_level(-4);
    ecs_expr_eval_desc_t desc = { .vars = NULL, .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(
        world, "$foo", &ecs_value_ptr(ecs_i32_t, &v), &desc);
    test_assert(ptr == NULL);

    ecs_fini(world);
}

void Expr_parse_eval(void) {
    ecs_world_t *world = ecs_init();

    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *foo = ecs_script_vars_define(vars, "foo", ecs_i32_t);
    *(int32_t*)foo->value.ptr = 10;

    int32_t v = 0;
    ecs_expr_eval_desc_t desc = { .vars = vars, .disable_folding = disable_folding };

    ecs_script_t *s = ecs_expr_parse(world, "$foo + 20", &desc);
    test_assert(s != NULL);

    test_int(0, ecs_expr_eval(s, 
        &ecs_value_ptr(ecs_i32_t, &v), &desc));
    test_int(v, 30);

    ecs_script_vars_fini(vars);
    ecs_script_free(s);

    ecs_fini(world);
}

void Expr_parse_eval_multiple_times(void) {
    ecs_world_t *world = ecs_init();

    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *foo = ecs_script_vars_define(vars, "foo", ecs_i32_t);
    *(int32_t*)foo->value.ptr = 10;

    int32_t v = 0;
    ecs_expr_eval_desc_t desc = { .vars = vars, .disable_folding = disable_folding };

    ecs_script_t *s = ecs_expr_parse(world, "$foo + 20", &desc);
    test_assert(s != NULL);

    test_int(0, ecs_expr_eval(s, 
        &ecs_value_ptr(ecs_i32_t, &v), &desc));
    test_int(v, 30);

    *(int32_t*)foo->value.ptr = 30;

    test_int(0, ecs_expr_eval(s, 
        &ecs_value_ptr(ecs_i32_t, &v), &desc));
    test_int(v, 50);

    ecs_script_vars_fini(vars);
    ecs_script_free(s);

    ecs_fini(world);
}

void Expr_parse_error(void) {
    ecs_world_t *world = ecs_init();

    ecs_log_set_level(-4);
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    ecs_script_t *s = ecs_expr_parse(world, "10 +", &desc);
    test_assert(s == NULL);

    ecs_fini(world);
}

void Expr_parse_eval_error(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_struct(world, {
        .entity = ecs_id(Position),
        .members = {
            { "x", ecs_id(ecs_f32_t) },
            { "y", ecs_id(ecs_f32_t) }
        }
    });

    ecs_entity_t e = ecs_entity(world, { .name = "e" });
    ecs_add(world, e, Position);

    ecs_log_set_level(-4);
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    ecs_script_t *s = ecs_expr_parse(world, "e[Position]", &desc);
    test_assert(s != NULL);

    ecs_remove(world, e, Position);

    Position v;
    test_assert(0 != ecs_expr_eval(s, 
        &ecs_value_ptr(Position, &v), NULL));

    ecs_script_free(s);

    ecs_fini(world);
}

void Expr_remainder_after_number(void) {
    ecs_world_t *world = ecs_init();

    int32_t v = 0;
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(world, "10 foo", 
        &ecs_value_ptr(ecs_i32_t, &v), &desc);
    test_assert(ptr != NULL);
    test_str(ptr, " foo");
    test_int(v, 10);

    ecs_fini(world);
}

void Expr_remainder_after_string(void) {
    ecs_world_t *world = ecs_init();

    char *v = 0;
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(world, "\"bar\" foo", 
        &ecs_value_ptr(ecs_string_t, &v), &desc);
    test_assert(ptr != NULL);
    test_str(ptr, " foo");
    test_str(v, "bar");
    
    ecs_os_free(v);

    ecs_fini(world);
}

void Expr_remainder_after_unary(void) {
    ecs_world_t *world = ecs_init();

    bool v = false;
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(world, "!false foo", 
        &ecs_value_ptr(ecs_bool_t, &v), &desc);
    test_assert(ptr != NULL);
    test_str(ptr, " foo");
    test_bool(v, true);

    ecs_fini(world);
}

void Expr_remainder_after_binary(void) {
    ecs_world_t *world = ecs_init();

    int32_t v = false;
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(world, "10 + 20 foo", 
        &ecs_value_ptr(ecs_i32_t, &v), &desc);
    test_assert(ptr != NULL);
    test_str(ptr, " foo");
    test_int(v, 30);

    ecs_fini(world);
}

void Expr_remainder_after_parens(void) {
    ecs_world_t *world = ecs_init();

    int32_t v = false;
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(world, "(10 + 20) foo", 
        &ecs_value_ptr(ecs_i32_t, &v), &desc);
    test_assert(ptr != NULL);
    test_str(ptr, " foo");
    test_int(v, 30);

    ecs_fini(world);
}

void Expr_remainder_after_initializer(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_struct(world, {
        .entity = ecs_id(Position),
        .members = {
            { "x", ecs_id(ecs_f32_t) },
            { "y", ecs_id(ecs_f32_t) }
        }
    });

    Position v = {0, 0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(world, "{10, 20} foo", 
        &ecs_value_ptr(Position, &v), &desc);
    test_assert(ptr != NULL);
    test_str(ptr, " foo");
    test_int(v.x, 10);
    test_int(v.y, 20);

    ecs_fini(world);
}

void Expr_remainder_after_collection_initializer(void) {
    ecs_world_t *world = ecs_init();

    typedef int32_t Ints[2];

    ECS_COMPONENT(world, Ints);

    ecs_array(world, {
        .entity = ecs_id(Ints),
        .type = ecs_id(ecs_i32_t),
        .count = 2
    });

    Ints v = {0, 0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(world, "[10, 20] foo", 
        &ecs_value_ptr(Ints, &v), &desc);
    test_assert(ptr != NULL);
    test_str(ptr, " foo");
    test_int(v[0], 10);
    test_int(v[1], 20);

    ecs_fini(world);
}

void Expr_remainder_after_initializer_w_newlines(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_struct(world, {
        .entity = ecs_id(Position),
        .members = {
            { "x", ecs_id(ecs_f32_t) },
            { "y", ecs_id(ecs_f32_t) }
        }
    });

    Position v = {0, 0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(world, "{\n10,\n 20\n} foo", 
        &ecs_value_ptr(Position, &v), &desc);
    test_assert(ptr != NULL);
    test_str(ptr, " foo");
    test_int(v.x, 10);
    test_int(v.y, 20);

    ecs_fini(world);
}

void Expr_remainder_after_initializer_w_crlf(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_struct(world, {
        .entity = ecs_id(Position),
        .members = {
            { "x", ecs_id(ecs_f32_t) },
            { "y", ecs_id(ecs_f32_t) }
        }
    });

    Position v = {0, 0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(world, "{\r\n10,\r\n 20\r\n} foo", 
        &ecs_value_ptr(Position, &v), &desc);
    test_assert(ptr != NULL);
    test_str(ptr, " foo");
    test_int(v.x, 10);
    test_int(v.y, 20);

    ecs_fini(world);
}

void Expr_remainder_after_initializer_before_parens(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_struct(world, {
        .entity = ecs_id(Position),
        .members = {
            { "x", ecs_id(ecs_f32_t) },
            { "y", ecs_id(ecs_f32_t) }
        }
    });

    Position v = {0, 0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(world, "{10, 20} ()", 
        &ecs_value_ptr(Position, &v), &desc);
    test_assert(ptr != NULL);
    test_str(ptr, " ()");
    test_int(v.x, 10);
    test_int(v.y, 20);

    ecs_fini(world);
}

void Expr_space_at_start(void) {
    ecs_world_t *world = ecs_init();

    int32_t v = false;
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(world, " 10 + 20", 
        &ecs_value_ptr(ecs_i32_t, &v), &desc);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == 0);
    test_int(v, 30);

    ecs_fini(world);
}

void Expr_newline_at_start(void) {
    ecs_world_t *world = ecs_init();

    int32_t v = false;
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(world, "\n10 + 20", 
        &ecs_value_ptr(ecs_i32_t, &v), &desc);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == 0);
    test_int(v, 30);

    ecs_fini(world);
}

void Expr_crlf_at_start(void) {
    ecs_world_t *world = ecs_init();

    int32_t v = false;
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(world, "\r\n10 + 20", 
        &ecs_value_ptr(ecs_i32_t, &v), &desc);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == 0);
    test_int(v, 30);

    ecs_fini(world);
}

void Expr_global_const_var(void) {
    ecs_world_t *world = ecs_init();

    int32_t v = 10;

    test_assert(0 != ecs_const_var(world, {
        .name = "FOO",
        .type = ecs_id(ecs_i32_t),
        .value = &v
    }));

    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(world, "$FOO + 20",
        &ecs_value_ptr(ecs_i32_t, &v), &desc);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == 0);
    test_int(v, 30);

    ecs_fini(world);
}

void Expr_scoped_global_const_var(void) {
    ecs_world_t *world = ecs_init();

    int32_t v = 10;

    ecs_entity_t parent = ecs_entity(world, {
        .name = "parent"
    });

    test_assert(0 != ecs_const_var(world, {
        .name = "FOO",
        .parent = parent,
        .type = ecs_id(ecs_i32_t),
        .value = &v
    }));

    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(world, 
        "parent.$FOO + 20",
        &ecs_value_ptr(ecs_i32_t, &v), &desc);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == 0);
    test_int(v, 30);

    ecs_fini(world);
}

void Expr_escape_newline(void) {
    ecs_world_t *world = ecs_init();

    char *v = NULL;
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, "\"Hello\\nWorld\"",
        &ecs_value_ptr(ecs_string_t, &v), &desc) != NULL);
    test_str(v, "Hello\nWorld");
    ecs_os_free(v);

    ecs_fini(world);
}

void Expr_match_i32_1_i_case(void) {
    ecs_world_t *world = ecs_init();

    ecs_script_vars_t *vars = ecs_script_vars_init(world);
    ecs_script_var_t *var = ecs_script_vars_define(vars, "i", ecs_i32_t);
    ecs_expr_eval_desc_t desc = {
        .vars = vars, .disable_folding = disable_folding };

    const char *expr = 
    HEAD "match $i {"
    LINE "  1: 10"
    LINE "}";

    ecs_script_t *s = ecs_expr_parse(world, expr, &desc);
    test_assert(s != NULL);

    {
        *(int32_t*)var->value.ptr = 0;
        ecs_value_t result = {0};
        ecs_log_set_level(-4);
        test_assert(0 != ecs_expr_eval(s, &result, &desc));
        ecs_log_set_level(-1);
    }

    {
        *(int32_t*)var->value.ptr = 1;
        ecs_value_t result = {0};
        test_assert(0 == ecs_expr_eval(s, &result, &desc));
        test_assert(result.type == ecs_id(ecs_i64_t));
        test_int(*(int64_t*)result.ptr, 10);
        ecs_value_free(world, result.type, result.ptr);
    }

    ecs_script_vars_fini(vars);
    ecs_script_free(s);

    ecs_fini(world);
}

void Expr_match_i32_2_i_cases(void) {
    ecs_world_t *world = ecs_init();

    ecs_script_vars_t *vars = ecs_script_vars_init(world);
    ecs_script_var_t *var = ecs_script_vars_define(vars, "i", ecs_i32_t);
    ecs_expr_eval_desc_t desc = {
        .vars = vars, .disable_folding = disable_folding };

    const char *expr = 
    HEAD "match $i {"
    LINE "  1: 10"
    LINE "  2: 20"
    LINE "}";

    ecs_script_t *s = ecs_expr_parse(world, expr, &desc);
    test_assert(s != NULL);

    {
        *(int32_t*)var->value.ptr = 0;
        ecs_value_t result = {0};
        ecs_log_set_level(-4);
        test_assert(0 != ecs_expr_eval(s, &result, &desc));
        ecs_log_set_level(-1);
    }

    {
        *(int32_t*)var->value.ptr = 1;
        ecs_value_t result = {0};
        test_assert(0 == ecs_expr_eval(s, &result, &desc));
        test_assert(result.type == ecs_id(ecs_i64_t));
        test_int(*(int64_t*)result.ptr, 10);
        ecs_value_free(world, result.type, result.ptr);
    }

    {
        *(int32_t*)var->value.ptr = 2;
        ecs_value_t result = {0};
        test_assert(0 == ecs_expr_eval(s, &result, &desc));
        test_assert(result.type == ecs_id(ecs_i64_t));
        test_int(*(int64_t*)result.ptr, 20);
        ecs_value_free(world, result.type, result.ptr);
    }

    ecs_script_vars_fini(vars);
    ecs_script_free(s);

    ecs_fini(world);
}

void Expr_match_i32_2_i_f_cases(void) {
    ecs_world_t *world = ecs_init();

    ecs_script_vars_t *vars = ecs_script_vars_init(world);
    ecs_script_var_t *var = ecs_script_vars_define(vars, "i", ecs_i32_t);
    ecs_expr_eval_desc_t desc = {
        .vars = vars, .disable_folding = disable_folding };

    const char *expr = 
    HEAD "match $i {"
    LINE "  1: 10"
    LINE "  2: 20.5"
    LINE "}";

    ecs_script_t *s = ecs_expr_parse(world, expr, &desc);
    test_assert(s != NULL);

    {
        *(int32_t*)var->value.ptr = 0;
        ecs_value_t result = {0};
        ecs_log_set_level(-4);
        test_assert(0 != ecs_expr_eval(s, &result, &desc));
        ecs_log_set_level(-1);
    }

    {
        *(int32_t*)var->value.ptr = 1;
        ecs_value_t result = {0};
        test_assert(0 == ecs_expr_eval(s, &result, &desc));
        test_assert(result.type == ecs_id(ecs_f64_t));
        test_assert(*(ecs_f64_t*)result.ptr == 10);
        ecs_value_free(world, result.type, result.ptr);
    }

    {
        *(int32_t*)var->value.ptr = 2;
        ecs_value_t result = {0};
        test_assert(0 == ecs_expr_eval(s, &result, &desc));
        test_assert(result.type == ecs_id(ecs_f64_t));
        test_assert(*(ecs_f64_t*)result.ptr == 20.5);
        ecs_value_free(world, result.type, result.ptr);
    }

    ecs_script_vars_fini(vars);
    ecs_script_free(s);

    ecs_fini(world);
}

void Expr_match_i32_2_f_i_cases(void) {
    ecs_world_t *world = ecs_init();

    ecs_script_vars_t *vars = ecs_script_vars_init(world);
    ecs_script_var_t *var = ecs_script_vars_define(vars, "i", ecs_i32_t);
    ecs_expr_eval_desc_t desc = {
        .vars = vars, .disable_folding = disable_folding };

    const char *expr = 
    HEAD "match $i {"
    LINE "  1: 10.5"
    LINE "  2: 20"
    LINE "}";

    ecs_script_t *s = ecs_expr_parse(world, expr, &desc);
    test_assert(s != NULL);

    {
        *(int32_t*)var->value.ptr = 0;
        ecs_value_t result = {0};
        ecs_log_set_level(-4);
        test_assert(0 != ecs_expr_eval(s, &result, &desc));
        ecs_log_set_level(-1);
    }

    {
        *(int32_t*)var->value.ptr = 1;
        ecs_value_t result = {0};
        test_assert(0 == ecs_expr_eval(s, &result, &desc));
        test_assert(result.type == ecs_id(ecs_f64_t));
        test_assert(*(ecs_f64_t*)result.ptr == 10.5);
        ecs_value_free(world, result.type, result.ptr);
    }

    {
        *(int32_t*)var->value.ptr = 2;
        ecs_value_t result = {0};
        test_assert(0 == ecs_expr_eval(s, &result, &desc));
        test_assert(result.type == ecs_id(ecs_f64_t));
        test_assert(*(ecs_f64_t*)result.ptr == 20);
        ecs_value_free(world, result.type, result.ptr);
    }

    ecs_script_vars_fini(vars);
    ecs_script_free(s);

    ecs_fini(world);
}

void Expr_match_i32_3_i_cases(void) {
    ecs_world_t *world = ecs_init();

    ecs_script_vars_t *vars = ecs_script_vars_init(world);
    ecs_script_var_t *var = ecs_script_vars_define(vars, "i", ecs_i32_t);
    ecs_expr_eval_desc_t desc = {
        .vars = vars, .disable_folding = disable_folding };

    const char *expr = 
    HEAD "match $i {"
    LINE "  1: 10"
    LINE "  2: 20"
    LINE "  3: 30"
    LINE "}";

    ecs_script_t *s = ecs_expr_parse(world, expr, &desc);
    test_assert(s != NULL);

    {
        *(int32_t*)var->value.ptr = 0;
        ecs_value_t result = {0};
        ecs_log_set_level(-4);
        test_assert(0 != ecs_expr_eval(s, &result, &desc));
        ecs_log_set_level(-1);
    }

    {
        *(int32_t*)var->value.ptr = 1;
        ecs_value_t result = {0};
        test_assert(0 == ecs_expr_eval(s, &result, &desc));
        test_assert(result.type == ecs_id(ecs_i64_t));
        test_int(*(int64_t*)result.ptr, 10);
        ecs_value_free(world, result.type, result.ptr);
    }

    {
        *(int32_t*)var->value.ptr = 2;
        ecs_value_t result = {0};
        test_assert(0 == ecs_expr_eval(s, &result, &desc));
        test_assert(result.type == ecs_id(ecs_i64_t));
        test_int(*(int64_t*)result.ptr, 20);
        ecs_value_free(world, result.type, result.ptr);
    }

    {
        *(int32_t*)var->value.ptr = 3;
        ecs_value_t result = {0};
        test_assert(0 == ecs_expr_eval(s, &result, &desc));
        test_assert(result.type == ecs_id(ecs_i64_t));
        test_int(*(int64_t*)result.ptr, 30);
        ecs_value_free(world, result.type, result.ptr);
    }

    ecs_script_vars_fini(vars);
    ecs_script_free(s);

    ecs_fini(world);
}

void Expr_match_i32_3_i_i_f_cases(void) {
    ecs_world_t *world = ecs_init();

    ecs_script_vars_t *vars = ecs_script_vars_init(world);
    ecs_script_var_t *var = ecs_script_vars_define(vars, "i", ecs_i32_t);
    ecs_expr_eval_desc_t desc = {
        .vars = vars, .disable_folding = disable_folding };

    const char *expr = 
    HEAD "match $i {"
    LINE "  1: 10"
    LINE "  2: 20"
    LINE "  3: 30.5"
    LINE "}";

    ecs_script_t *s = ecs_expr_parse(world, expr, &desc);
    test_assert(s != NULL);

    {
        *(int32_t*)var->value.ptr = 0;
        ecs_value_t result = {0};
        ecs_log_set_level(-4);
        test_assert(0 != ecs_expr_eval(s, &result, &desc));
        ecs_log_set_level(-1);
    }

    {
        *(int32_t*)var->value.ptr = 1;
        ecs_value_t result = {0};
        test_assert(0 == ecs_expr_eval(s, &result, &desc));
        test_assert(result.type == ecs_id(ecs_f64_t));
        test_assert(*(ecs_f64_t*)result.ptr == 10);
        ecs_value_free(world, result.type, result.ptr);
    }

    {
        *(int32_t*)var->value.ptr = 2;
        ecs_value_t result = {0};
        test_assert(0 == ecs_expr_eval(s, &result, &desc));
        test_assert(result.type == ecs_id(ecs_f64_t));
        test_assert(*(ecs_f64_t*)result.ptr == 20);
        ecs_value_free(world, result.type, result.ptr);
    }

    {
        *(int32_t*)var->value.ptr = 3;
        ecs_value_t result = {0};
        test_assert(0 == ecs_expr_eval(s, &result, &desc));
        test_assert(result.type == ecs_id(ecs_f64_t));
        test_assert(*(ecs_f64_t*)result.ptr == 30.5);
        ecs_value_free(world, result.type, result.ptr);
    }

    ecs_script_vars_fini(vars);
    ecs_script_free(s);

    ecs_fini(world);
}

void Expr_match_i32_3_i_f_i_cases(void) {
    ecs_world_t *world = ecs_init();

    ecs_script_vars_t *vars = ecs_script_vars_init(world);
    ecs_script_var_t *var = ecs_script_vars_define(vars, "i", ecs_i32_t);
    ecs_expr_eval_desc_t desc = {
        .vars = vars, .disable_folding = disable_folding };

    const char *expr = 
    HEAD "match $i {"
    LINE "  1: 10"
    LINE "  2: 20.5"
    LINE "  3: 30"
    LINE "}";

    ecs_script_t *s = ecs_expr_parse(world, expr, &desc);
    test_assert(s != NULL);

    {
        *(int32_t*)var->value.ptr = 0;
        ecs_value_t result = {0};
        ecs_log_set_level(-4);
        test_assert(0 != ecs_expr_eval(s, &result, &desc));
        ecs_log_set_level(-1);
    }

    {
        *(int32_t*)var->value.ptr = 1;
        ecs_value_t result = {0};
        test_assert(0 == ecs_expr_eval(s, &result, &desc));
        test_assert(result.type == ecs_id(ecs_f64_t));
        test_assert(*(ecs_f64_t*)result.ptr == 10);
        ecs_value_free(world, result.type, result.ptr);
    }

    {
        *(int32_t*)var->value.ptr = 2;
        ecs_value_t result = {0};
        test_assert(0 == ecs_expr_eval(s, &result, &desc));
        test_assert(result.type == ecs_id(ecs_f64_t));
        test_assert(*(ecs_f64_t*)result.ptr == 20.5);
        ecs_value_free(world, result.type, result.ptr);
    }

    {
        *(int32_t*)var->value.ptr = 3;
        ecs_value_t result = {0};
        test_assert(0 == ecs_expr_eval(s, &result, &desc));
        test_assert(result.type == ecs_id(ecs_f64_t));
        test_assert(*(ecs_f64_t*)result.ptr == 30);
        ecs_value_free(world, result.type, result.ptr);
    }

    ecs_script_vars_fini(vars);
    ecs_script_free(s);

    ecs_fini(world);
}

void Expr_match_i32_3_f_i_i_cases(void) {
    ecs_world_t *world = ecs_init();

    ecs_script_vars_t *vars = ecs_script_vars_init(world);
    ecs_script_var_t *var = ecs_script_vars_define(vars, "i", ecs_i32_t);
    ecs_expr_eval_desc_t desc = {
        .vars = vars, .disable_folding = disable_folding };

    const char *expr = 
    HEAD "match $i {"
    LINE "  1: 10.5"
    LINE "  2: 20"
    LINE "  3: 30"
    LINE "}";

    ecs_script_t *s = ecs_expr_parse(world, expr, &desc);
    test_assert(s != NULL);

    {
        *(int32_t*)var->value.ptr = 0;
        ecs_value_t result = {0};
        ecs_log_set_level(-4);
        test_assert(0 != ecs_expr_eval(s, &result, &desc));
        ecs_log_set_level(-1);
    }

    {
        *(int32_t*)var->value.ptr = 1;
        ecs_value_t result = {0};
        test_assert(0 == ecs_expr_eval(s, &result, &desc));
        test_assert(result.type == ecs_id(ecs_f64_t));
        test_assert(*(ecs_f64_t*)result.ptr == 10.5);
        ecs_value_free(world, result.type, result.ptr);
    }

    {
        *(int32_t*)var->value.ptr = 2;
        ecs_value_t result = {0};
        test_assert(0 == ecs_expr_eval(s, &result, &desc));
        test_assert(result.type == ecs_id(ecs_f64_t));
        test_assert(*(ecs_f64_t*)result.ptr == 20);
        ecs_value_free(world, result.type, result.ptr);
    }

    {
        *(int32_t*)var->value.ptr = 3;
        ecs_value_t result = {0};
        test_assert(0 == ecs_expr_eval(s, &result, &desc));
        test_assert(result.type == ecs_id(ecs_f64_t));
        test_assert(*(ecs_f64_t*)result.ptr == 30);
        ecs_value_free(world, result.type, result.ptr);
    }

    ecs_script_vars_fini(vars);
    ecs_script_free(s);

    ecs_fini(world);
}

void Expr_match_i32_1_struct_case(void) {
    ecs_world_t *world = ecs_init();

    typedef struct {
        int32_t x;
        int32_t y;
    } Position;

    ecs_entity_t ecs_id(Position) = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Position" }),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    ecs_script_vars_t *vars = ecs_script_vars_init(world);
    ecs_script_var_t *var = ecs_script_vars_define(vars, "i", ecs_i32_t);
    ecs_expr_eval_desc_t desc = {
        .vars = vars, .disable_folding = disable_folding, 
        .type = ecs_id(Position) };

    const char *expr = 
    HEAD "match $i {"
    LINE "  1: {10, 20}"
    LINE "}";

    ecs_script_t *s = ecs_expr_parse(world, expr, &desc);
    test_assert(s != NULL);

    {
        *(int32_t*)var->value.ptr = 0;
        Position p = {0};
        ecs_value_t result = { .type = ecs_id(Position), .ptr = &p };
        ecs_log_set_level(-4);
        test_assert(0 != ecs_expr_eval(s, &result, &desc));
        ecs_log_set_level(-1);
    }

    {
        *(int32_t*)var->value.ptr = 1;
        Position p = {0};
        ecs_value_t result = { .type = ecs_id(Position), .ptr = &p };
        test_assert(0 == ecs_expr_eval(s, &result, &desc));
        test_assert(result.type == ecs_id(Position));
        test_int(p.x, 10);
        test_int(p.y, 20);
    }

    ecs_script_vars_fini(vars);
    ecs_script_free(s);

    ecs_fini(world);
}

void Expr_match_i32_2_struct_cases(void) {
    ecs_world_t *world = ecs_init();

    typedef struct {
        int32_t x;
        int32_t y;
    } Position;

    ecs_entity_t ecs_id(Position) = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Position" }),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    ecs_script_vars_t *vars = ecs_script_vars_init(world);
    ecs_script_var_t *var = ecs_script_vars_define(vars, "i", ecs_i32_t);
    ecs_expr_eval_desc_t desc = {
        .vars = vars, .disable_folding = disable_folding, 
        .type = ecs_id(Position) };

    const char *expr = 
    HEAD "match $i {"
    LINE "  1: {10, 20}"
    LINE "  2: {30, 40}"
    LINE "}";

    ecs_script_t *s = ecs_expr_parse(world, expr, &desc);
    test_assert(s != NULL);

    {
        *(int32_t*)var->value.ptr = 0;
        Position p = {0};
        ecs_value_t result = { .type = ecs_id(Position), .ptr = &p };
        ecs_log_set_level(-4);
        test_assert(0 != ecs_expr_eval(s, &result, &desc));
        ecs_log_set_level(-1);
    }

    {
        *(int32_t*)var->value.ptr = 1;
        Position p = {0};
        ecs_value_t result = { .type = ecs_id(Position), .ptr = &p };
        test_assert(0 == ecs_expr_eval(s, &result, &desc));
        test_assert(result.type == ecs_id(Position));
        test_int(p.x, 10);
        test_int(p.y, 20);
    }

    {
        *(int32_t*)var->value.ptr = 2;
        Position p = {0};
        ecs_value_t result = { .type = ecs_id(Position), .ptr = &p };
        test_assert(0 == ecs_expr_eval(s, &result, &desc));
        test_assert(result.type == ecs_id(Position));
        test_int(p.x, 30);
        test_int(p.y, 40);
    }

    ecs_script_vars_fini(vars);
    ecs_script_free(s);

    ecs_fini(world);
}

void Expr_match_i32_3_struct_cases(void) {
    ecs_world_t *world = ecs_init();

    typedef struct {
        int32_t x;
        int32_t y;
    } Position;

    ecs_entity_t ecs_id(Position) = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Position" }),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    ecs_script_vars_t *vars = ecs_script_vars_init(world);
    ecs_script_var_t *var = ecs_script_vars_define(vars, "i", ecs_i32_t);
    ecs_expr_eval_desc_t desc = {
        .vars = vars, .disable_folding = disable_folding, 
        .type = ecs_id(Position) };

    const char *expr = 
    HEAD "match $i {"
    LINE "  1: {10, 20}"
    LINE "  2: {30, 40}"
    LINE "  3: {50, 60}"
    LINE "}";

    ecs_script_t *s = ecs_expr_parse(world, expr, &desc);
    test_assert(s != NULL);

    {
        *(int32_t*)var->value.ptr = 0;
        Position p = {0};
        ecs_value_t result = { .type = ecs_id(Position), .ptr = &p };
        ecs_log_set_level(-4);
        test_assert(0 != ecs_expr_eval(s, &result, &desc));
        ecs_log_set_level(-1);
    }

    {
        *(int32_t*)var->value.ptr = 1;
        Position p = {0};
        ecs_value_t result = { .type = ecs_id(Position), .ptr = &p };
        test_assert(0 == ecs_expr_eval(s, &result, &desc));
        test_assert(result.type == ecs_id(Position));
        test_int(p.x, 10);
        test_int(p.y, 20);
    }

    {
        *(int32_t*)var->value.ptr = 2;
        Position p = {0};
        ecs_value_t result = { .type = ecs_id(Position), .ptr = &p };
        test_assert(0 == ecs_expr_eval(s, &result, &desc));
        test_assert(result.type == ecs_id(Position));
        test_int(p.x, 30);
        test_int(p.y, 40);
    }

    {
        *(int32_t*)var->value.ptr = 3;
        Position p = {0};
        ecs_value_t result = { .type = ecs_id(Position), .ptr = &p };
        test_assert(0 == ecs_expr_eval(s, &result, &desc));
        test_assert(result.type == ecs_id(Position));
        test_int(p.x, 50);
        test_int(p.y, 60);
    }

    ecs_script_vars_fini(vars);
    ecs_script_free(s);

    ecs_fini(world);
}

void Expr_match_i32_empty_struct_cases(void) {
    ecs_world_t *world = ecs_init();

    typedef struct {
        int32_t x;
        int32_t y;
    } Position;

    ecs_entity_t ecs_id(Position) = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Position" }),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    ecs_script_vars_t *vars = ecs_script_vars_init(world);
    ecs_script_var_t *var = ecs_script_vars_define(vars, "i", ecs_i32_t);
    ecs_expr_eval_desc_t desc = {
        .vars = vars, .disable_folding = disable_folding, 
        .type = ecs_id(Position) };

    const char *expr = 
    HEAD "match $i {"
    LINE "  1: {10, 20}"
    LINE "  2: {30, 40}"
    LINE "  3: {}"
    LINE "}";

    ecs_script_t *s = ecs_expr_parse(world, expr, &desc);
    test_assert(s != NULL);

    {
        *(int32_t*)var->value.ptr = 0;
        Position p = {0};
        ecs_value_t result = { .type = ecs_id(Position), .ptr = &p };
        ecs_log_set_level(-4);
        test_assert(0 != ecs_expr_eval(s, &result, &desc));
        ecs_log_set_level(-1);
    }

    {
        *(int32_t*)var->value.ptr = 1;
        Position p = {0};
        ecs_value_t result = { .type = ecs_id(Position), .ptr = &p };
        test_assert(0 == ecs_expr_eval(s, &result, &desc));
        test_assert(result.type == ecs_id(Position));
        test_int(p.x, 10);
        test_int(p.y, 20);
    }

    {
        *(int32_t*)var->value.ptr = 2;
        Position p = {0};
        ecs_value_t result = { .type = ecs_id(Position), .ptr = &p };
        test_assert(0 == ecs_expr_eval(s, &result, &desc));
        test_assert(result.type == ecs_id(Position));
        test_int(p.x, 30);
        test_int(p.y, 40);
    }

    {
        *(int32_t*)var->value.ptr = 3;
        Position p = {0};
        ecs_value_t result = { .type = ecs_id(Position), .ptr = &p };
        test_assert(0 == ecs_expr_eval(s, &result, &desc));
        test_assert(result.type == ecs_id(Position));
        test_int(p.x, 0);
        test_int(p.y, 0);
    }

    ecs_script_vars_fini(vars);
    ecs_script_free(s);

    ecs_fini(world);
}

void Expr_match_i32_struct_cases_unknown_type(void) {
    ecs_world_t *world = ecs_init();

    ecs_script_vars_t *vars = ecs_script_vars_init(world);
    ecs_script_vars_define(vars, "i", ecs_i32_t);
    ecs_expr_eval_desc_t desc = {
        .vars = vars, .disable_folding = disable_folding };

    const char *expr = 
    HEAD "match $i {"
    LINE "  1: {10, 20}"
    LINE "  2: {30, 40}"
    LINE "}";

    ecs_log_set_level(-4);
    ecs_script_t *s = ecs_expr_parse(world, expr, &desc);
    test_assert(s == NULL);

    ecs_script_vars_fini(vars);

    ecs_fini(world);
}

void Expr_match_i32_1_collection_case(void) {
    ecs_world_t *world = ecs_init();

    typedef int32_t Ints[2];

    ecs_entity_t ecs_id(Ints) = ecs_array(world, {
        .type = ecs_id(ecs_i32_t),
        .count = 2
    });

    ecs_script_vars_t *vars = ecs_script_vars_init(world);
    ecs_script_var_t *var = ecs_script_vars_define(vars, "i", ecs_i32_t);
    ecs_expr_eval_desc_t desc = {
        .vars = vars, .disable_folding = disable_folding, 
        .type = ecs_id(Ints) };

    const char *expr = 
    HEAD "match $i {"
    LINE "  1: [10, 20]"
    LINE "}";

    ecs_script_t *s = ecs_expr_parse(world, expr, &desc);
    test_assert(s != NULL);

    {
        *(int32_t*)var->value.ptr = 0;
        Ints p = {0};
        ecs_value_t result = { .type = ecs_id(Ints), .ptr = &p };
        ecs_log_set_level(-4);
        test_assert(0 != ecs_expr_eval(s, &result, &desc));
        ecs_log_set_level(-1);
    }

    {
        *(int32_t*)var->value.ptr = 1;
        Ints p = {0};
        ecs_value_t result = { .type = ecs_id(Ints), .ptr = &p };
        test_assert(0 == ecs_expr_eval(s, &result, &desc));
        test_assert(result.type == ecs_id(Ints));
        test_int(p[0], 10);
        test_int(p[1], 20);
    }

    ecs_script_vars_fini(vars);
    ecs_script_free(s);

    ecs_fini(world);
}

void Expr_match_i32_2_collection_cases(void) {
    ecs_world_t *world = ecs_init();

    typedef int32_t Ints[2];

    ecs_entity_t ecs_id(Ints) = ecs_array(world, {
        .type = ecs_id(ecs_i32_t),
        .count = 2
    });

    ecs_script_vars_t *vars = ecs_script_vars_init(world);
    ecs_script_var_t *var = ecs_script_vars_define(vars, "i", ecs_i32_t);
    ecs_expr_eval_desc_t desc = {
        .vars = vars, .disable_folding = disable_folding, 
        .type = ecs_id(Ints) };

    const char *expr = 
    HEAD "match $i {"
    LINE "  1: [10, 20]"
    LINE "  2: [20, 30]"
    LINE "}";

    ecs_script_t *s = ecs_expr_parse(world, expr, &desc);
    test_assert(s != NULL);

    {
        *(int32_t*)var->value.ptr = 0;
        Ints p = {0};
        ecs_value_t result = { .type = ecs_id(Ints), .ptr = &p };
        ecs_log_set_level(-4);
        test_assert(0 != ecs_expr_eval(s, &result, &desc));
        ecs_log_set_level(-1);
    }

    {
        *(int32_t*)var->value.ptr = 1;
        Ints p = {0};
        ecs_value_t result = { .type = ecs_id(Ints), .ptr = &p };
        test_assert(0 == ecs_expr_eval(s, &result, &desc));
        test_assert(result.type == ecs_id(Ints));
        test_int(p[0], 10);
        test_int(p[1], 20);
    }

    {
        *(int32_t*)var->value.ptr = 2;
        Ints p = {0};
        ecs_value_t result = { .type = ecs_id(Ints), .ptr = &p };
        test_assert(0 == ecs_expr_eval(s, &result, &desc));
        test_assert(result.type == ecs_id(Ints));
        test_int(p[0], 20);
        test_int(p[1], 30);
    }

    ecs_script_vars_fini(vars);
    ecs_script_free(s);

    ecs_fini(world);
}

void Expr_match_i32_3_collection_cases(void) {
    ecs_world_t *world = ecs_init();

    typedef int32_t Ints[2];

    ecs_entity_t ecs_id(Ints) = ecs_array(world, {
        .type = ecs_id(ecs_i32_t),
        .count = 2
    });

    ecs_script_vars_t *vars = ecs_script_vars_init(world);
    ecs_script_var_t *var = ecs_script_vars_define(vars, "i", ecs_i32_t);
    ecs_expr_eval_desc_t desc = {
        .vars = vars, .disable_folding = disable_folding, 
        .type = ecs_id(Ints) };

    const char *expr = 
    HEAD "match $i {"
    LINE "  1: [10, 20]"
    LINE "  2: [20, 30]"
    LINE "  3: [30, 40]"
    LINE "}";

    ecs_script_t *s = ecs_expr_parse(world, expr, &desc);
    test_assert(s != NULL);

    {
        *(int32_t*)var->value.ptr = 0;
        Ints p = {0};
        ecs_value_t result = { .type = ecs_id(Ints), .ptr = &p };
        ecs_log_set_level(-4);
        test_assert(0 != ecs_expr_eval(s, &result, &desc));
        ecs_log_set_level(-1);
    }

    {
        *(int32_t*)var->value.ptr = 1;
        Ints p = {0};
        ecs_value_t result = { .type = ecs_id(Ints), .ptr = &p };
        test_assert(0 == ecs_expr_eval(s, &result, &desc));
        test_assert(result.type == ecs_id(Ints));
        test_int(p[0], 10);
        test_int(p[1], 20);
    }

    {
        *(int32_t*)var->value.ptr = 2;
        Ints p = {0};
        ecs_value_t result = { .type = ecs_id(Ints), .ptr = &p };
        test_assert(0 == ecs_expr_eval(s, &result, &desc));
        test_assert(result.type == ecs_id(Ints));
        test_int(p[0], 20);
        test_int(p[1], 30);
    }

    {
        *(int32_t*)var->value.ptr = 3;
        Ints p = {0};
        ecs_value_t result = { .type = ecs_id(Ints), .ptr = &p };
        test_assert(0 == ecs_expr_eval(s, &result, &desc));
        test_assert(result.type == ecs_id(Ints));
        test_int(p[0], 30);
        test_int(p[1], 40);
    }

    ecs_script_vars_fini(vars);
    ecs_script_free(s);

    ecs_fini(world);
}

void Expr_match_i32_empty_collection_case(void) {
    ecs_world_t *world = ecs_init();

    typedef int32_t Ints[2];

    ecs_entity_t ecs_id(Ints) = ecs_array(world, {
        .entity = ecs_entity(world, { .name = "Ints" }),
        .type = ecs_id(ecs_i32_t),
        .count = 2
    });

    ecs_script_vars_t *vars = ecs_script_vars_init(world);
    ecs_script_var_t *var = ecs_script_vars_define(vars, "i", ecs_i32_t);
    ecs_expr_eval_desc_t desc = {
        .vars = vars, .disable_folding = disable_folding, 
        .type = ecs_id(Ints) };

    const char *expr = 
    HEAD "match $i {"
    LINE "  1: [10, 20]"
    LINE "  2: [20, 30]"
    LINE "  3: []"
    LINE "}";

    ecs_script_t *s = ecs_expr_parse(world, expr, &desc);
    test_assert(s != NULL);

    {
        *(int32_t*)var->value.ptr = 0;
        Ints p = {0};
        ecs_value_t result = { .type = ecs_id(Ints), .ptr = &p };
        ecs_log_set_level(-4);
        test_assert(0 != ecs_expr_eval(s, &result, &desc));
        ecs_log_set_level(-1);
    }

    {
        *(int32_t*)var->value.ptr = 1;
        Ints p = {0};
        ecs_value_t result = { .type = ecs_id(Ints), .ptr = &p };
        test_assert(0 == ecs_expr_eval(s, &result, &desc));
        test_assert(result.type == ecs_id(Ints));
        test_int(p[0], 10);
        test_int(p[1], 20);
    }

    {
        *(int32_t*)var->value.ptr = 2;
        Ints p = {0};
        ecs_value_t result = { .type = ecs_id(Ints), .ptr = &p };
        test_assert(0 == ecs_expr_eval(s, &result, &desc));
        test_assert(result.type == ecs_id(Ints));
        test_int(p[0], 20);
        test_int(p[1], 30);
    }

    {
        *(int32_t*)var->value.ptr = 3;
        Ints p = {0};
        ecs_value_t result = { .type = ecs_id(Ints), .ptr = &p };
        test_assert(0 == ecs_expr_eval(s, &result, &desc));
        test_assert(result.type == ecs_id(Ints));
        test_int(p[0], 0);
        test_int(p[1], 0);
    }

    ecs_script_vars_fini(vars);
    ecs_script_free(s);

    ecs_fini(world);
}

void Expr_match_i32_collection_case_unknown_type(void) {
    ecs_world_t *world = ecs_init();

    ecs_script_vars_t *vars = ecs_script_vars_init(world);
    ecs_script_vars_define(vars, "i", ecs_i32_t);
    ecs_expr_eval_desc_t desc = {
        .vars = vars, .disable_folding = disable_folding };

    const char *expr = 
    HEAD "match $i {"
    LINE "  1: [10, 20]"
    LINE "  2: [30, 40]"
    LINE "}";

    ecs_log_set_level(-4);
    ecs_script_t *s = ecs_expr_parse(world, expr, &desc);
    test_assert(s == NULL);

    ecs_script_vars_fini(vars);

    ecs_fini(world);
}

void Expr_match_i32_struct_invalid_case_type(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Position" }),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    ecs_script_vars_t *vars = ecs_script_vars_init(world);
    ecs_script_vars_define(vars, "i", ecs_i32_t);
    ecs_expr_eval_desc_t desc = {
        .vars = vars, .disable_folding = disable_folding, 
        .type = ecs_id(Position) };

    const char *expr = 
    HEAD "match $i {"
    LINE "  1: 10"
    LINE "}";

    ecs_log_set_level(-4);
    ecs_script_t *s = ecs_expr_parse(world, expr, &desc);
    test_assert(s == NULL);
    ecs_script_vars_fini(vars);

    ecs_fini(world);
}

void Expr_match_i32_collection_invalid_case_type(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Ints) = ecs_array(world, {
        .type = ecs_id(ecs_i32_t),
        .count = 2
    });

    ecs_script_vars_t *vars = ecs_script_vars_init(world);
    ecs_script_vars_define(vars, "i", ecs_i32_t);
    ecs_expr_eval_desc_t desc = {
        .vars = vars, .disable_folding = disable_folding,
        .type = ecs_id(Ints) };

    const char *expr = 
    HEAD "match $i {"
    LINE "  1: 10"
    LINE "}";

    ecs_log_set_level(-4);
    ecs_script_t *s = ecs_expr_parse(world, expr, &desc);
    test_assert(s == NULL);

    ecs_script_vars_fini(vars);

    ecs_fini(world);
}

void Expr_match_i32_string(void) {
    ecs_world_t *world = ecs_init();

    ecs_script_vars_t *vars = ecs_script_vars_init(world);
    ecs_script_var_t *var = ecs_script_vars_define(vars, "i", ecs_i32_t);
    ecs_expr_eval_desc_t desc = {
        .vars = vars, .disable_folding = disable_folding };

    const char *expr = 
    HEAD "match $i {"
    LINE "  1: \"100\""
    LINE "  2: \"200\""
    LINE "  3: \"300\""
    LINE "}";

    ecs_script_t *s = ecs_expr_parse(world, expr, &desc);
    test_assert(s != NULL);

    {
        *(int32_t*)var->value.ptr = 0;
        ecs_value_t result = {0};
        ecs_log_set_level(-4);
        test_assert(0 != ecs_expr_eval(s, &result, &desc));
        ecs_log_set_level(-1);
    }

    {
        *(int32_t*)var->value.ptr = 1;
        ecs_value_t result = {0};
        test_assert(0 == ecs_expr_eval(s, &result, &desc));
        test_assert(result.type == ecs_id(ecs_string_t));
        test_str(*(char**)result.ptr, "100");
        ecs_value_free(world, result.type, result.ptr);
    }

    {
        *(int32_t*)var->value.ptr = 2;
        ecs_value_t result = {0};
        test_assert(0 == ecs_expr_eval(s, &result, &desc));
        test_assert(result.type == ecs_id(ecs_string_t));
        test_str(*(char**)result.ptr, "200");
        ecs_value_free(world, result.type, result.ptr);
    }

    {
        *(int32_t*)var->value.ptr = 3;
        ecs_value_t result = {0};
        test_assert(0 == ecs_expr_eval(s, &result, &desc));
        test_assert(result.type == ecs_id(ecs_string_t));
        test_str(*(char**)result.ptr, "300");
        ecs_value_free(world, result.type, result.ptr);
    }

    ecs_script_vars_fini(vars);
    ecs_script_free(s);

    ecs_fini(world);
}

void Expr_match_enum_string(void) {
    ecs_world_t *world = ecs_init();

    typedef enum {
        Red, Green, Blue
    } Color;

    ecs_entity_t ecs_id(Color) = ecs_enum(world, {
        .entity = ecs_entity(world, { .name = "Color" }),
        .constants = {
            {"Red"}, 
            {"Green"}, 
            {"Blue"}
        }
    });

    ecs_script_vars_t *vars = ecs_script_vars_init(world);
    ecs_script_var_t *var = ecs_script_vars_define(vars, "i", Color);
    ecs_expr_eval_desc_t desc = {
        .vars = vars, .disable_folding = disable_folding };

    const char *expr = 
    HEAD "match $i {"
    LINE "  Red:   \"Red\""
    LINE "  Green: \"Green\""
    LINE "  Blue:  \"Blue\""
    LINE "}";

    ecs_script_t *s = ecs_expr_parse(world, expr, &desc);
    test_assert(s != NULL);

    {
        *(Color*)var->value.ptr = 100;
        ecs_value_t result = {0};
        ecs_log_set_level(-4);
        test_assert(0 != ecs_expr_eval(s, &result, &desc));
        ecs_log_set_level(-1);
    }

    {
        *(Color*)var->value.ptr = Red;
        ecs_value_t result = {0};
        test_assert(0 == ecs_expr_eval(s, &result, &desc));
        test_assert(result.type == ecs_id(ecs_string_t));
        test_str(*(char**)result.ptr, "Red");
        ecs_value_free(world, result.type, result.ptr);
    }

    {
        *(Color*)var->value.ptr = Green;
        ecs_value_t result = {0};
        test_assert(0 == ecs_expr_eval(s, &result, &desc));
        test_assert(result.type == ecs_id(ecs_string_t));
        test_str(*(char**)result.ptr, "Green");
        ecs_value_free(world, result.type, result.ptr);
    }

    {
        *(Color*)var->value.ptr = Blue;
        ecs_value_t result = {0};
        test_assert(0 == ecs_expr_eval(s, &result, &desc));
        test_assert(result.type == ecs_id(ecs_string_t));
        test_str(*(char**)result.ptr, "Blue");
        ecs_value_free(world, result.type, result.ptr);
    }

    ecs_script_vars_fini(vars);
    ecs_script_free(s);

    ecs_fini(world);
}

void Expr_match_string_i(void) {
    ecs_world_t *world = ecs_init();

    ecs_script_vars_t *vars = ecs_script_vars_init(world);
    ecs_script_var_t *var = ecs_script_vars_define(vars, "i", ecs_string_t);
    ecs_expr_eval_desc_t desc = {
        .vars = vars, .disable_folding = disable_folding };

    const char *expr = 
    HEAD "match $i {"
    LINE "  \"100\": 1"
    LINE "  \"200\": 2"
    LINE "  \"300\": 3"
    LINE "}";

    ecs_script_t *s = ecs_expr_parse(world, expr, &desc);
    test_assert(s != NULL);

    {
        *(char**)var->value.ptr = NULL;
        ecs_value_t result = {0};
        ecs_log_set_level(-4);
        test_assert(0 != ecs_expr_eval(s, &result, &desc));
        ecs_log_set_level(-1);
    }

    {
        *(char**)var->value.ptr = "10";
        ecs_value_t result = {0};
        ecs_log_set_level(-4);
        test_assert(0 != ecs_expr_eval(s, &result, &desc));
        ecs_log_set_level(-1);
    }

    {
        *(char**)var->value.ptr = "100";
        ecs_value_t result = {0};
        test_assert(0 == ecs_expr_eval(s, &result, &desc));
        test_assert(result.type == ecs_id(ecs_i64_t));
        test_int(*(int64_t*)result.ptr, 1);
        ecs_value_free(world, result.type, result.ptr);
    }

    {
        *(char**)var->value.ptr = "200";
        ecs_value_t result = {0};
        test_assert(0 == ecs_expr_eval(s, &result, &desc));
        test_assert(result.type == ecs_id(ecs_i64_t));
        test_int(*(int64_t*)result.ptr, 2);
        ecs_value_free(world, result.type, result.ptr);
    }

    {
        *(char**)var->value.ptr = "300";
        ecs_value_t result = {0};
        test_assert(0 == ecs_expr_eval(s, &result, &desc));
        test_assert(result.type == ecs_id(ecs_i64_t));
        test_int(*(int64_t*)result.ptr, 3);
        ecs_value_free(world, result.type, result.ptr);
    }

    *(char**)var->value.ptr = NULL;

    ecs_script_vars_fini(vars);
    ecs_script_free(s);

    ecs_fini(world);
}

void Expr_match_w_any(void) {
    ecs_world_t *world = ecs_init();

    ecs_script_vars_t *vars = ecs_script_vars_init(world);
    ecs_script_var_t *var = ecs_script_vars_define(vars, "i", ecs_i32_t);
    ecs_expr_eval_desc_t desc = {
        .vars = vars, .disable_folding = disable_folding };

    const char *expr = 
    HEAD "match $i {"
    LINE "  1: 10"
    LINE "  2: 20"
    LINE "  3: 30"
    LINE "  _: 40"
    LINE "}";

    ecs_script_t *s = ecs_expr_parse(world, expr, &desc);
    test_assert(s != NULL);

    {
        *(int32_t*)var->value.ptr = 0;
        ecs_value_t result = {0};
        test_assert(0 == ecs_expr_eval(s, &result, &desc));
        test_assert(result.type == ecs_id(ecs_i64_t));
        test_int(*(int64_t*)result.ptr, 40);
        ecs_value_free(world, result.type, result.ptr);
    }

    {
        *(int32_t*)var->value.ptr = 1;
        ecs_value_t result = {0};
        test_assert(0 == ecs_expr_eval(s, &result, &desc));
        test_assert(result.type == ecs_id(ecs_i64_t));
        test_int(*(int64_t*)result.ptr, 10);
        ecs_value_free(world, result.type, result.ptr);
    }

    {
        *(int32_t*)var->value.ptr = 2;
        ecs_value_t result = {0};
        test_assert(0 == ecs_expr_eval(s, &result, &desc));
        test_assert(result.type == ecs_id(ecs_i64_t));
        test_int(*(int64_t*)result.ptr, 20);
        ecs_value_free(world, result.type, result.ptr);
    }

    {
        *(int32_t*)var->value.ptr = 3;
        ecs_value_t result = {0};
        test_assert(0 == ecs_expr_eval(s, &result, &desc));
        test_assert(result.type == ecs_id(ecs_i64_t));
        test_int(*(int64_t*)result.ptr, 30);
        ecs_value_free(world, result.type, result.ptr);
    }

    {
        *(int32_t*)var->value.ptr = 4;
        ecs_value_t result = {0};
        test_assert(0 == ecs_expr_eval(s, &result, &desc));
        test_assert(result.type == ecs_id(ecs_i64_t));
        test_int(*(int64_t*)result.ptr, 40);
        ecs_value_free(world, result.type, result.ptr);
    }

    ecs_script_vars_fini(vars);
    ecs_script_free(s);

    ecs_fini(world);
}

void Expr_match_w_any_not_last(void) {
    ecs_world_t *world = ecs_init();

    ecs_script_vars_t *vars = ecs_script_vars_init(world);
    ecs_script_vars_define(vars, "i", ecs_i32_t);
    ecs_expr_eval_desc_t desc = {
        .vars = vars, .disable_folding = disable_folding };

    const char *expr = 
    HEAD "match $i {"
    LINE "  1: 10"
    LINE "  2: 20"
    LINE "  _: 40"
    LINE "  3: 30"
    LINE "}";

    ecs_log_set_level(-4);
    ecs_script_t *s = ecs_expr_parse(world, expr, &desc);
    test_assert(s == NULL);

    ecs_script_vars_fini(vars);

    ecs_fini(world);
}

void Expr_match_w_any_first(void) {
    ecs_world_t *world = ecs_init();

    ecs_script_vars_t *vars = ecs_script_vars_init(world);
    ecs_script_vars_define(vars, "i", ecs_i32_t);
    ecs_expr_eval_desc_t desc = {
        .vars = vars, .disable_folding = disable_folding };

    const char *expr = 
    HEAD "match $i {"
    LINE "  _: 40"
    LINE "  1: 10"
    LINE "  2: 20"
    LINE "  3: 30"
    LINE "}";

    ecs_log_set_level(-4);
    ecs_script_t *s = ecs_expr_parse(world, expr, &desc);
    test_assert(s == NULL);

    ecs_script_vars_fini(vars);

    ecs_fini(world);
}

void Expr_match_w_any_mismatching_type(void) {
    ecs_world_t *world = ecs_init();

    ecs_script_vars_t *vars = ecs_script_vars_init(world);
    ecs_script_vars_define(vars, "i", ecs_i32_t);
    ecs_expr_eval_desc_t desc = {
        .vars = vars, .disable_folding = disable_folding };

    const char *expr = 
    HEAD "match $i {"
    LINE "  1: 10"
    LINE "  2: 20"
    LINE "  3: 30"
    LINE "  _: \"Hello\""
    LINE "}";

    ecs_log_set_level(-4);
    ecs_script_t *s = ecs_expr_parse(world, expr, &desc);
    test_assert(s == NULL);

    ecs_script_vars_fini(vars);

    ecs_fini(world);
}

void Expr_match_i_w_any_f(void) {
    ecs_world_t *world = ecs_init();

    ecs_script_vars_t *vars = ecs_script_vars_init(world);
    ecs_script_var_t *var = ecs_script_vars_define(vars, "i", ecs_i32_t);
    ecs_expr_eval_desc_t desc = {
        .vars = vars, .disable_folding = disable_folding };

    const char *expr = 
    HEAD "match $i {"
    LINE "  1: 10"
    LINE "  2: 20"
    LINE "  _: 30.5"
    LINE "}";

    ecs_script_t *s = ecs_expr_parse(world, expr, &desc);
    test_assert(s != NULL);

    {
        *(int32_t*)var->value.ptr = 0;
        ecs_value_t result = {0};
        test_assert(0 == ecs_expr_eval(s, &result, &desc));
        test_assert(result.type == ecs_id(ecs_f64_t));
        test_assert(*(ecs_f64_t*)result.ptr == 30.5);
        ecs_value_free(world, result.type, result.ptr);
    }

    {
        *(int32_t*)var->value.ptr = 1;
        ecs_value_t result = {0};
        test_assert(0 == ecs_expr_eval(s, &result, &desc));
        test_assert(result.type == ecs_id(ecs_f64_t));
        test_assert(*(ecs_f64_t*)result.ptr == 10);
        ecs_value_free(world, result.type, result.ptr);
    }

    {
        *(int32_t*)var->value.ptr = 2;
        ecs_value_t result = {0};
        test_assert(0 == ecs_expr_eval(s, &result, &desc));
        test_assert(result.type == ecs_id(ecs_f64_t));
        test_assert(*(ecs_f64_t*)result.ptr == 20);
        ecs_value_free(world, result.type, result.ptr);
    }

    {
        *(int32_t*)var->value.ptr = 3;
        ecs_value_t result = {0};
        test_assert(0 == ecs_expr_eval(s, &result, &desc));
        test_assert(result.type == ecs_id(ecs_f64_t));
        test_assert(*(ecs_f64_t*)result.ptr == 30.5);
        ecs_value_free(world, result.type, result.ptr);
    }

    ecs_script_vars_fini(vars);
    ecs_script_free(s);

    ecs_fini(world);
}

void Expr_identifier_as_var(void) {
    ecs_world_t *world = ecs_init();

    ecs_script_vars_t *vars = ecs_script_vars_init(world);
    ecs_script_var_t *var = ecs_script_vars_define(
        vars, "foo", ecs_i64_t);
    *(int32_t*)var->value.ptr = 20;

    ecs_expr_eval_desc_t desc = { .vars = vars, .disable_folding = disable_folding };

    ecs_value_t v = {0};
    test_assert(ecs_expr_run(world, "foo", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_i64_t));
    test_assert(v.ptr != NULL);
    test_uint(*(int64_t*)v.ptr, 20);
    ecs_value_free(world, v.type, v.ptr);

    ecs_script_vars_fini(vars);

    ecs_fini(world);
}

void Expr_expr_w_identifier_as_var(void) {
    ecs_world_t *world = ecs_init();

    ecs_script_vars_t *vars = ecs_script_vars_init(world);
    ecs_script_var_t *var = ecs_script_vars_define(
        vars, "foo", ecs_i64_t);
    *(int32_t*)var->value.ptr = 20;

    ecs_expr_eval_desc_t desc = { .vars = vars, .disable_folding = disable_folding };

    ecs_value_t v = {0};
    test_assert(ecs_expr_run(world, "10 + foo", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_i64_t));
    test_assert(v.ptr != NULL);
    test_uint(*(int64_t*)v.ptr, 10 + 20);
    ecs_value_free(world, v.type, v.ptr);

    ecs_script_vars_fini(vars);

    ecs_fini(world);
}

void Expr_identifier_as_const_var(void) {
    ecs_world_t *world = ecs_init();

    int32_t value = 10;

    test_assert(0 != ecs_const_var(world, {
        .name = "FOO",
        .type = ecs_id(ecs_i32_t),
        .value = &value
    }));

    ecs_value_t v = {0};
    test_assert(ecs_expr_run(world, "FOO", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_i32_t));
    test_assert(v.ptr != NULL);
    test_uint(*(int32_t*)v.ptr, 10);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_initializer_w_identifier_as_var(void) {
    ecs_world_t *world = ecs_init();

    typedef struct {
        int32_t x;
        int32_t y;
    } Position;

    ecs_entity_t ecs_id(Position) = ecs_struct(world, {
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    ecs_script_vars_t *vars = ecs_script_vars_init(world);
    ecs_script_var_t *x_var = ecs_script_vars_define(vars, "x", ecs_i32_t);
    *(int32_t*)x_var->value.ptr = 10;
    ecs_script_var_t *y_var = ecs_script_vars_define(vars, "y", ecs_i32_t);
    *(int32_t*)y_var->value.ptr = 20;

    ecs_expr_eval_desc_t desc = { .vars = vars, .disable_folding = disable_folding };

    Position p;
    ecs_value_t v = { .type = ecs_id(Position), .ptr = &p };
    test_assert(ecs_expr_run(world, "{x, y}", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(Position));
    test_assert(v.ptr != NULL);
    test_int(p.x, 10);
    test_int(p.y, 20);

    ecs_script_vars_fini(vars);

    ecs_fini(world);
}

void Expr_new_entity(void) {
    ecs_world_t *world = ecs_init();

    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };

    ecs_entity_t e = 0;
    ecs_value_t v = { .type = ecs_id(ecs_entity_t), .ptr = &e };
    test_assert(ecs_expr_run(world, "new {}", &v, &desc) != NULL);
    test_assert(e != 0);
    test_assert(ecs_is_alive(world, e));
    test_int(ecs_get_type(world, e)->count, 0);

    ecs_fini(world);
}

void Expr_new_entity_w_component(void) {
    ecs_world_t *world = ecs_init();

    typedef struct {
        int32_t x;
        int32_t y;
    } Position;

    ecs_entity_t ecs_id(Position) = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Position" }),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };

    ecs_entity_t e = 0;
    ecs_value_t v = { .type = ecs_id(ecs_entity_t), .ptr = &e };
    test_assert(ecs_expr_run(world, "new { Position: {10, 20} }", &v, &desc) != NULL);
    test_assert(e != 0);
    test_assert(ecs_is_alive(world, e));
    test_int(ecs_get_type(world, e)->count, 1);

    const Position *p = ecs_get(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void Expr_new_entity_w_component_w_newline(void) {
    ecs_world_t *world = ecs_init();

    typedef struct {
        int32_t x;
        int32_t y;
    } Position;

    ecs_entity_t ecs_id(Position) = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Position" }),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };

    ecs_entity_t e = 0;
    ecs_value_t v = { .type = ecs_id(ecs_entity_t), .ptr = &e };
    test_assert(ecs_expr_run(world, "new {\n Position: {\n10, 20\n}\n}", &v, &desc) != NULL);
    test_assert(e != 0);
    test_assert(ecs_is_alive(world, e));
    test_int(ecs_get_type(world, e)->count, 1);

    const Position *p = ecs_get(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void Expr_new_entity_w_kind(void) {
    ecs_world_t *world = ecs_init();

    typedef struct {
        int32_t x;
        int32_t y;
    } Position;

    ecs_entity_t ecs_id(Position) = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Position" }),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };

    ecs_entity_t e = 0;
    ecs_value_t v = { .type = ecs_id(ecs_entity_t), .ptr = &e };
    test_assert(ecs_expr_run(world, "new Position(10, 20)", &v, &desc) != NULL);
    test_assert(e != 0);
    test_assert(ecs_is_alive(world, e));
    test_int(ecs_get_type(world, e)->count, 1);

    const Position *p = ecs_get(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void Expr_new_entity_w_component_w_vars(void) {
    ecs_world_t *world = ecs_init();

    typedef struct {
        int32_t x;
        int32_t y;
    } Position;

    ecs_entity_t ecs_id(Position) = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Position" }),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    ecs_script_vars_t *vars = ecs_script_vars_init(world);
    ecs_script_var_t *x_var = ecs_script_vars_define(vars, "x", ecs_i32_t);
    *(int32_t*)x_var->value.ptr = 10;
    ecs_script_var_t *y_var = ecs_script_vars_define(vars, "y", ecs_i32_t);
    *(int32_t*)y_var->value.ptr = 20;

    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding, .vars = vars };

    ecs_entity_t e = 0;
    ecs_value_t v = { .type = ecs_id(ecs_entity_t), .ptr = &e };
    test_assert(ecs_expr_run(world, "new { Position: {$x, $y} }", &v, &desc) != NULL);
    test_assert(e != 0);
    test_assert(ecs_is_alive(world, e));
    test_int(ecs_get_type(world, e)->count, 1);

    const Position *p = ecs_get(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_script_vars_fini(vars);

    ecs_fini(world);
}

void Expr_new_named_entity(void) {
    ecs_world_t *world = ecs_init();

    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };

    ecs_entity_t e = 0;
    ecs_value_t v = { .type = ecs_id(ecs_entity_t), .ptr = &e };
    test_assert(ecs_expr_run(world, "new Foo {}", &v, &desc) != NULL);
    test_assert(e != 0);
    test_assert(ecs_is_alive(world, e));
    test_str(ecs_get_name(world, e), "Foo");

    ecs_fini(world);
}

void Expr_new_named_entity_w_component(void) {
    ecs_world_t *world = ecs_init();

    typedef struct {
        int32_t x;
        int32_t y;
    } Position;

    ecs_entity_t ecs_id(Position) = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Position" }),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };

    ecs_entity_t e = 0;
    ecs_value_t v = { .type = ecs_id(ecs_entity_t), .ptr = &e };
    test_assert(ecs_expr_run(world, "new Foo { Position: {10, 20} }", &v, &desc) != NULL);
    test_assert(e != 0);
    test_assert(ecs_is_alive(world, e));
    test_str(ecs_get_name(world, e), "Foo");

    const Position *p = ecs_get(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void Expr_new_named_entity_w_kind(void) {
    ecs_world_t *world = ecs_init();

    typedef struct {
        int32_t x;
        int32_t y;
    } Position;

    ecs_entity_t ecs_id(Position) = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Position" }),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };

    ecs_entity_t e = 0;
    ecs_value_t v = { .type = ecs_id(ecs_entity_t), .ptr = &e };
    test_assert(ecs_expr_run(world, "new Position Foo(10, 20)", &v, &desc) != NULL);
    test_assert(e != 0);
    test_assert(ecs_is_alive(world, e));
    test_str(ecs_get_name(world, e), "Foo");

    const Position *p = ecs_get(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void Expr_new_name_expr_entity(void) {
    ecs_world_t *world = ecs_init();

    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };

    ecs_entity_t e = 0;
    ecs_value_t v = { .type = ecs_id(ecs_entity_t), .ptr = &e };
    test_assert(ecs_expr_run(world, "new \"Foo\" {}", &v, &desc) != NULL);
    test_assert(e != 0);
    test_assert(ecs_is_alive(world, e));
    test_str(ecs_get_name(world, e), "Foo");

    ecs_fini(world);
}

void Expr_new_name_expr_entity_w_component(void) {
    ecs_world_t *world = ecs_init();

    typedef struct {
        int32_t x;
        int32_t y;
    } Position;

    ecs_entity_t ecs_id(Position) = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Position" }),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };

    ecs_entity_t e = 0;
    ecs_value_t v = { .type = ecs_id(ecs_entity_t), .ptr = &e };
    test_assert(ecs_expr_run(world, "new \"Foo\" { Position: {10, 20} }", &v, &desc) != NULL);
    test_assert(e != 0);
    test_assert(ecs_is_alive(world, e));
    test_str(ecs_get_name(world, e), "Foo");

    const Position *p = ecs_get(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void Expr_new_name_expr_entity_w_kind(void) {
    ecs_world_t *world = ecs_init();

    typedef struct {
        int32_t x;
        int32_t y;
    } Position;

    ecs_entity_t ecs_id(Position) = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Position" }),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };

    ecs_entity_t e = 0;
    ecs_value_t v = { .type = ecs_id(ecs_entity_t), .ptr = &e };
    test_assert(ecs_expr_run(world, "new Position \"Foo\"(10, 20)", &v, &desc) != NULL);
    test_assert(e != 0);
    test_assert(ecs_is_alive(world, e));
    test_str(ecs_get_name(world, e), "Foo");

    const Position *p = ecs_get(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void Expr_new_entity_w_unterminated_scope(void) {
    ecs_world_t *world = ecs_init();

    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };

    ecs_entity_t e = 0;
    ecs_value_t v = { .type = ecs_id(ecs_entity_t), .ptr = &e };
    ecs_log_set_level(-4);
    test_assert(ecs_expr_run(world, "new {", &v, &desc) == NULL);
    test_assert(e == 0);

    ecs_fini(world);
}
