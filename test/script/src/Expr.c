#include <script.h>

void Expr_add_2_int_literals(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_script_expr_run(world, "10 + 20", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_u64_t));
    test_assert(v.ptr != NULL);
    test_uint(*(uint64_t*)v.ptr, 10 + 20);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_add_2_int_literals_twice(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_script_expr_run(world, "10 + 20", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_u64_t));
    test_assert(v.ptr != NULL);
    test_uint(*(uint64_t*)v.ptr, 10 + 20);

    test_assert(ecs_script_expr_run(world, "10 + 20", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_u64_t));
    test_assert(v.ptr != NULL);
    test_uint(*(uint64_t*)v.ptr, 10 + 20);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_sub_2_int_literals(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_script_expr_run(world, "20 - 10", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_i64_t));
    test_assert(v.ptr != NULL);
    test_int(*(int64_t*)v.ptr, 20 - 10);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_mul_2_int_literals(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_script_expr_run(world, "20 * 10", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_u64_t));
    test_assert(v.ptr != NULL);
    test_uint(*(ecs_u64_t*)v.ptr, 20 * 10);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_div_2_int_literals(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_script_expr_run(world, "10 / 2", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_f64_t));
    test_assert(v.ptr != NULL);
    test_flt(*(ecs_f64_t*)v.ptr, 10 / 2);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_add_3_int_literals(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_script_expr_run(world, "10 + 20 + 30", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_u64_t));
    test_assert(v.ptr != NULL);
    test_uint(*(uint64_t*)v.ptr, 10 + 20 + 30);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_add_3_int_literals_twice(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_script_expr_run(world, "10 + 20 + 30", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_u64_t));
    test_assert(v.ptr != NULL);
    test_uint(*(uint64_t*)v.ptr, 10 + 20 + 30);

    test_assert(ecs_script_expr_run(world, "10 + 20 + 30", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_u64_t));
    test_assert(v.ptr != NULL);
    test_uint(*(uint64_t*)v.ptr, 10 + 20 + 30);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_sub_3_int_literals(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_script_expr_run(world, "30 - 10 - 5", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_i64_t));
    test_assert(v.ptr != NULL);
    test_int(*(ecs_i64_t*)v.ptr, 30 - 10 - 5);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_mul_3_int_literals(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_script_expr_run(world, "2 * 5 * 10", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_u64_t));
    test_assert(v.ptr != NULL);
    test_uint(*(ecs_u64_t*)v.ptr, 2 * 5 * 10);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_div_3_int_literals(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_script_expr_run(world, "40 / 5 / 2", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_f64_t));
    test_assert(v.ptr != NULL);
    test_flt(*(ecs_f64_t*)v.ptr, 40 / 5 / 2);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_int_to_bool(void) {
    ecs_world_t *world = ecs_init();

    bool b = false;
    ecs_value_t v = { .type = ecs_id(ecs_bool_t), .ptr = &b };

    test_assert(ecs_script_expr_run(world, "10", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_flt(*(ecs_bool_t*)v.ptr, true);

    test_assert(ecs_script_expr_run(world, "0", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_flt(*(ecs_bool_t*)v.ptr, false);

    test_assert(ecs_script_expr_run(world, "256", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_flt(*(ecs_bool_t*)v.ptr, true);

    ecs_fini(world);
}

void Expr_bool_to_int(void) {
    ecs_world_t *world = ecs_init();

    int32_t i = 0;
    ecs_value_t v = { .type = ecs_id(ecs_i32_t), .ptr = &i };

    test_assert(ecs_script_expr_run(world, "true", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_i32_t));
    test_assert(v.ptr != NULL);
    test_flt(*(ecs_i32_t*)v.ptr, 1);

    test_assert(ecs_script_expr_run(world, "false", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_i32_t));
    test_assert(v.ptr != NULL);
    test_flt(*(ecs_i32_t*)v.ptr, 0);

    ecs_fini(world);
}

void Expr_bool_to_uint(void) {
    ecs_world_t *world = ecs_init();

    uint32_t i = 0;
    ecs_value_t v = { .type = ecs_id(ecs_u32_t), .ptr = &i };

    test_assert(ecs_script_expr_run(world, "true", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_u32_t));
    test_assert(v.ptr != NULL);
    test_flt(*(ecs_u32_t*)v.ptr, 1);

    test_assert(ecs_script_expr_run(world, "false", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_u32_t));
    test_assert(v.ptr != NULL);
    test_flt(*(ecs_u32_t*)v.ptr, 0);

    ecs_fini(world);
}

void Expr_add_mul_3_int_literals(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_script_expr_run(world, "10 + 20 * 2", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_u64_t));
    test_assert(v.ptr != NULL);
    test_uint(*(uint64_t*)v.ptr, 10 + 20 * 2);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_sub_mul_3_int_literals(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_script_expr_run(world, "50 - 10 * 2", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_i64_t));
    test_assert(v.ptr != NULL);
    test_int(*(int64_t*)v.ptr, 50 - 10 * 2);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_div_mul_3_int_literals(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_script_expr_run(world, "10 / 5 * 2", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_f64_t));
    test_assert(v.ptr != NULL);
    test_flt(*(ecs_f64_t*)v.ptr, 10 / 5 * 2);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_add_div_3_int_literals(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_script_expr_run(world, "10 + 30 / 2", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_f64_t));
    test_assert(v.ptr != NULL);
    test_uint(*(ecs_f64_t*)v.ptr, 10 + 30 / 2);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_sub_div_3_int_literals(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_script_expr_run(world, "30 - 10 / 2", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_f64_t));
    test_assert(v.ptr != NULL);
    test_uint(*(ecs_f64_t*)v.ptr, 30 - 10 / 2);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_mul_div_3_int_literals(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_script_expr_run(world, "20 * 10 / 2", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_f64_t));
    test_assert(v.ptr != NULL);
    test_uint(*(ecs_f64_t*)v.ptr, 20 * 10 / 2);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_mul_add_mul_add_int_literals(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_script_expr_run(world, "2 * 4 + 6 * 8 + 10", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_u64_t));
    test_assert(v.ptr != NULL);
    test_uint(*(ecs_u64_t*)v.ptr, 2 * 4 + 6 * 8 + 10);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_mul_sub_mul_sub_int_literals(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_script_expr_run(world, "2 * 4 - 6 * 8 - 10", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_i64_t));
    test_assert(v.ptr != NULL);
    test_int(*(ecs_i64_t*)v.ptr, 2 * 4 - 6 * 8 - 10);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_mul_div_mul_div_int_literals(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_script_expr_run(world, "2 * 4 / 6 * 8 / 10", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_f64_t));
    test_assert(v.ptr != NULL);
    test_flt(*(ecs_f64_t*)v.ptr, 2.0 * 4.0 / 6.0 * 8.0 / 10.0);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_div_add_div_add_int_literals(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_script_expr_run(world, "2 / 4 + 6 / 8 + 10", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_f64_t));
    test_assert(v.ptr != NULL);
    test_flt(*(ecs_f64_t*)v.ptr, 2.0 / 4.0 + 6.0 / 8.0 + 10.0);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_div_sub_div_sub_int_literals(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_script_expr_run(world, "2 / 4 - 6 / 8 - 10", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_f64_t));
    test_assert(v.ptr != NULL);
    test_flt(*(ecs_f64_t*)v.ptr, 2.0 / 4.0 - 6.0 / 8.0 - 10.0);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_div_mul_div_mul_int_literals(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_script_expr_run(world, "2 / 4 * 6 / 8 * 10", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_f64_t));
    test_assert(v.ptr != NULL);
    test_flt(*(ecs_f64_t*)v.ptr, 2.0 / 4.0 * 6.0 / 8.0 * 10.0);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_div_sub_div_mul_int_literals(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_script_expr_run(world, "2 / 4 - 6 / 8 * 10", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_f64_t));
    test_assert(v.ptr != NULL);
    test_flt(*(ecs_f64_t*)v.ptr, 2.0 / 4.0 - 6.0 / 8.0 * 10.0);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_add_2_flt_literals(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_script_expr_run(world, "10.5 + 20.0", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_f64_t));
    test_assert(v.ptr != NULL);
    test_flt(*(ecs_f64_t*)v.ptr, 10.5 + 20.0);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_sub_2_flt_literals(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_script_expr_run(world, "20.5 - 10.0", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_f64_t));
    test_assert(v.ptr != NULL);
    test_flt(*(ecs_f64_t*)v.ptr, 20.5 - 10.0);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_mul_2_flt_literals(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_script_expr_run(world, "20.5 * 10.0", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_f64_t));
    test_assert(v.ptr != NULL);
    test_flt(*(ecs_f64_t*)v.ptr, 20.5 * 10.0);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_div_2_flt_literals(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_script_expr_run(world, "10.5 / 2.0", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_f64_t));
    test_assert(v.ptr != NULL);
    test_flt(*(ecs_f64_t*)v.ptr, 10.5 / 2.0);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_add_2_int_neg_literals(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_script_expr_run(world, "-10 + -20", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_i64_t));
    test_assert(v.ptr != NULL);
    test_int(*(ecs_i64_t*)v.ptr, -10 + -20);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_sub_2_int_neg_literals(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_script_expr_run(world, "-10 - -20", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_i64_t));
    test_assert(v.ptr != NULL);
    test_int(*(ecs_i64_t*)v.ptr, -10 - -20);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_mul_2_int_neg_literals(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_script_expr_run(world, "-10 * -20", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_i64_t));
    test_assert(v.ptr != NULL);
    test_int(*(ecs_i64_t*)v.ptr, -10 * -20);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_div_2_int_neg_literals(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_script_expr_run(world, "-10 / -20", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_f64_t));
    test_assert(v.ptr != NULL);
    test_flt(*(ecs_f64_t*)v.ptr, -10.0 / -20.0);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_mul_lparen_add_add_rparen_int_literals(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_script_expr_run(world, "10 * (20 + 30)", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_u64_t));
    test_assert(v.ptr != NULL);
    test_uint(*(ecs_u64_t*)v.ptr, 10 * (20 + 30));
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_mul_lparen_add_add_add_rparen_int_literals(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_script_expr_run(world, "10 * (20 + 30 + 40)", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_u64_t));
    test_assert(v.ptr != NULL);
    test_uint(*(ecs_u64_t*)v.ptr, 10 * (20 + 30 + 40));
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_mul_lparen_add_add_rparen_add_int_literals(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_script_expr_run(world, "10 * (20 + 30) + 40", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_u64_t));
    test_assert(v.ptr != NULL);
    test_uint(*(ecs_u64_t*)v.ptr, 10 * (20 + 30) + 40);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_lparen_add_add_rparen_mul_int_literals(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_script_expr_run(world, "(20 + 30) * 10", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_u64_t));
    test_assert(v.ptr != NULL);
    test_uint(*(ecs_u64_t*)v.ptr, (20 + 30) * 10);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_lparen_add_add_add_rparen_mul_int_literals(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_script_expr_run(world, "(20 + 30 + 40) * 10", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_u64_t));
    test_assert(v.ptr != NULL);
    test_uint(*(ecs_u64_t*)v.ptr, (20 + 30 + 40) * 10);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_double_paren_add_add(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_script_expr_run(world, "((20 + 30))", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_u64_t));
    test_assert(v.ptr != NULL);
    test_uint(*(ecs_u64_t*)v.ptr, ((20 + 30)));
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_double_paren_literal(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_script_expr_run(world, "((20))", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_u64_t));
    test_assert(v.ptr != NULL);
    test_uint(*(ecs_u64_t*)v.ptr, ((20)));
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_lparen_add_add_rparen_mul_lparen_add_add_rparen(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_script_expr_run(world, "(10 + 20) * (20 + 30)", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_u64_t));
    test_assert(v.ptr != NULL);
    test_uint(*(ecs_u64_t*)v.ptr, (10 + 20) * (20 + 30));
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
    test_assert(ecs_script_expr_run(world, "10 + 20", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_f32_t));
    test_assert(v.ptr != NULL);
    test_flt(value, 10 + 20);

    ecs_fini(world);
}

void Expr_struct_result_add_2_int_literals(void) {
    ecs_world_t *world = ecs_init();

    typedef struct {
        int32_t value;
    } Mass;

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .members = {
            {"value", ecs_id(ecs_i32_t)}
        }
    });

    Mass v = {0};
    const char *ptr = ecs_script_expr_run(world, "{10 + 20}", &(ecs_value_t){
        .type = t, .ptr = &v
    }, NULL);
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

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    Mass v = {0};
    const char *ptr = ecs_script_expr_run(world, "{10 + 20, 20 + 30}", &(ecs_value_t){
        .type = t, .ptr = &v
    }, NULL);
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

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .members = {
            {"value", ecs_id(ecs_i32_t)}
        }
    });

    Mass v = {0};
    const char *ptr = ecs_script_expr_run(world, "{10 + 20 + 30}", &(ecs_value_t){
        .type = t, .ptr = &v
    }, NULL);
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

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .members = {
            {"value", ecs_id(ecs_i32_t)}
        }
    });

    Mass v = {0};
    const char *ptr = ecs_script_expr_run(world, "{(10)}", &(ecs_value_t){
        .type = t, .ptr = &v
    }, NULL);
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
    ecs_script_expr_run_desc_t desc = { .vars = vars };
    const char *ptr = ecs_script_expr_run(world, "$foo + 20", &v, &desc);
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
    ecs_script_expr_run_desc_t desc = { .vars = vars };
    const char *ptr = ecs_script_expr_run(world, "20 + $foo", &v, &desc);
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

    ecs_script_expr_run_desc_t desc = { .vars = vars };
    {
        ecs_value_t v = {0};
        const char *ptr = ecs_script_expr_run(world, "$foo.x", &v, &desc);
        test_assert(ptr != NULL);
        test_assert(!ptr[0]);
        test_uint(v.type, ecs_id(ecs_i32_t));
        test_int(*(ecs_i32_t*)v.ptr, 10);
        ecs_value_free(world, v.type, v.ptr);
    }
    {
        ecs_value_t v = {0};
        const char *ptr = ecs_script_expr_run(world, "$foo.y", &v, &desc);
        test_assert(ptr != NULL);
        test_assert(!ptr[0]);
        test_uint(v.type, ecs_id(ecs_i32_t));
        test_int(*(ecs_i32_t*)v.ptr, 20);
        ecs_value_free(world, v.type, v.ptr);
    }

    ecs_script_vars_fini(vars);
    ecs_fini(world);
}

void Expr_bool_cond_and_bool(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_script_expr_run(world, "true && true", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);

    test_assert(ecs_script_expr_run(world, "true && false", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);

    test_assert(ecs_script_expr_run(world, "false && true", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);

    test_assert(ecs_script_expr_run(world, "false && false", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_bool_cond_or_bool(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_script_expr_run(world, "true || true", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);

    test_assert(ecs_script_expr_run(world, "true || false", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);

    test_assert(ecs_script_expr_run(world, "false || true", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);

    test_assert(ecs_script_expr_run(world, "false || false", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_int_cond_and_int(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_script_expr_run(world, "10 && 10", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);

    test_assert(ecs_script_expr_run(world, "10 && 0", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);

    test_assert(ecs_script_expr_run(world, "0 && 10", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);

    test_assert(ecs_script_expr_run(world, "0 && 0", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_int_cond_or_int(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_script_expr_run(world, "10 || 10", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);

    test_assert(ecs_script_expr_run(world, "10 || 0", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);

    test_assert(ecs_script_expr_run(world, "0 || 10", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);

    test_assert(ecs_script_expr_run(world, "0 || 0", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_bool_cond_and_int(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_script_expr_run(world, "true && 10", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);

    test_assert(ecs_script_expr_run(world, "true && 0", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);

    test_assert(ecs_script_expr_run(world, "false && 10", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);

    test_assert(ecs_script_expr_run(world, "false && 0", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_int_cond_and_bool(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_script_expr_run(world, "10 && true", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);

    test_assert(ecs_script_expr_run(world, "10 && false", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);

    test_assert(ecs_script_expr_run(world, "0 && true", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);

    test_assert(ecs_script_expr_run(world, "0 && false", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_bool_cond_or_int(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_script_expr_run(world, "true || 10", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);

    test_assert(ecs_script_expr_run(world, "true || 0", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);

    test_assert(ecs_script_expr_run(world, "false || 10", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);

    test_assert(ecs_script_expr_run(world, "false || 0", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_int_cond_or_bool(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_script_expr_run(world, "10 || true", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);

    test_assert(ecs_script_expr_run(world, "10 || false", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);

    test_assert(ecs_script_expr_run(world, "0 || true", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);

    test_assert(ecs_script_expr_run(world, "0 || false", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_cond_eq_bool(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_script_expr_run(world, "true == true", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);

    test_assert(ecs_script_expr_run(world, "true == false", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);

    test_assert(ecs_script_expr_run(world, "false == true", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);

    test_assert(ecs_script_expr_run(world, "false == false", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_cond_eq_int(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_script_expr_run(world, "10 == 10", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);

    test_assert(ecs_script_expr_run(world, "10 == 20", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);

    test_assert(ecs_script_expr_run(world, "10 == 0", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);

    test_assert(ecs_script_expr_run(world, "0 == 0", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_cond_neq_bool(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_script_expr_run(world, "true != true", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);

    test_assert(ecs_script_expr_run(world, "true != false", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);

    test_assert(ecs_script_expr_run(world, "false != true", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);

    test_assert(ecs_script_expr_run(world, "false != false", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_cond_neq_int(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_script_expr_run(world, "10 != 10", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);

    test_assert(ecs_script_expr_run(world, "10 != 20", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);

    test_assert(ecs_script_expr_run(world, "10 != 0", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);

    test_assert(ecs_script_expr_run(world, "0 != 0", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_cond_eq_bool_int(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};

    ecs_log_set_level(-4);
    test_assert(ecs_script_expr_run(world, "true == 1", &v, NULL) == NULL);
    test_assert(ecs_script_expr_run(world, "true == 0", &v, NULL) == NULL);
    test_assert(ecs_script_expr_run(world, "false == 0", &v, NULL) == NULL);

    ecs_fini(world);
}

void Expr_cond_eq_int_flt(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};

    ecs_log_set_level(-4);
    test_assert(ecs_script_expr_run(world, "1 == 1.0", &v, NULL) == NULL);
    test_assert(ecs_script_expr_run(world, "1 == 0.0", &v, NULL) == NULL);
    test_assert(ecs_script_expr_run(world, "0 == 0.0", &v, NULL) == NULL);

    ecs_fini(world);
}

void Expr_cond_eq_cond_and(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_script_expr_run(world, "true == true && false", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true == true && false);

    test_assert(ecs_script_expr_run(world, "true && false == false", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true && false == false);

    test_assert(ecs_script_expr_run(world, "true && true == true", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true && true == true);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_cond_eq_cond_or(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_script_expr_run(world, "true == true || false", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true == true || false);

    test_assert(ecs_script_expr_run(world, "true || false == false", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true || false == false);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_cond_gt_bool(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_script_expr_run(world, "true > false", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);

    test_assert(ecs_script_expr_run(world, "true > true", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);

    test_assert(ecs_script_expr_run(world, "false > true", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);

    test_assert(ecs_script_expr_run(world, "false > false", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_cond_gt_int(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_script_expr_run(world, "10 > 5", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);

    test_assert(ecs_script_expr_run(world, "10 > 10", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);

    test_assert(ecs_script_expr_run(world, "5 > 10", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);

    test_assert(ecs_script_expr_run(world, "5 > 5", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_cond_gt_flt(void) {
        ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_script_expr_run(world, "10.5 > 5.5", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);

    test_assert(ecs_script_expr_run(world, "10.5 > 10.5", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);

    test_assert(ecs_script_expr_run(world, "5.5 > 10.5", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);

    test_assert(ecs_script_expr_run(world, "5.5 > 5.5", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_cond_gteq_bool(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_script_expr_run(world, "true >= false", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);

    test_assert(ecs_script_expr_run(world, "true >= true", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);

    test_assert(ecs_script_expr_run(world, "false >= true", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);

    test_assert(ecs_script_expr_run(world, "false >= false", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_cond_gteq_int(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_script_expr_run(world, "10 >= 5", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);

    test_assert(ecs_script_expr_run(world, "10 >= 10", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);

    test_assert(ecs_script_expr_run(world, "5 >= 10", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);

    test_assert(ecs_script_expr_run(world, "5 >= 5", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_cond_gteq_flt(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_script_expr_run(world, "10.5 >= 5.5", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);

    test_assert(ecs_script_expr_run(world, "10.5 >= 10.5", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);

    test_assert(ecs_script_expr_run(world, "5.5 >= 10.5", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);

    test_assert(ecs_script_expr_run(world, "5.5 >= 5.5", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_cond_lt_bool(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_script_expr_run(world, "true < false", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);

    test_assert(ecs_script_expr_run(world, "true < true", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);

    test_assert(ecs_script_expr_run(world, "false < true", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);

    test_assert(ecs_script_expr_run(world, "false < false", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_cond_lt_int(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_script_expr_run(world, "10 < 5", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);

    test_assert(ecs_script_expr_run(world, "10 < 10", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);

    test_assert(ecs_script_expr_run(world, "5 < 10", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);

    test_assert(ecs_script_expr_run(world, "5 < 5", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_cond_lt_flt(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_script_expr_run(world, "10.5 < 5.5", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);

    test_assert(ecs_script_expr_run(world, "10.5 < 10.5", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);

    test_assert(ecs_script_expr_run(world, "5.5 < 10.5", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);

    test_assert(ecs_script_expr_run(world, "5.5 < 5.5", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_cond_lteq_bool(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_script_expr_run(world, "true <= false", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);

    test_assert(ecs_script_expr_run(world, "true <= true", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);

    test_assert(ecs_script_expr_run(world, "false <= true", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);

    test_assert(ecs_script_expr_run(world, "false <= false", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_cond_lteq_int(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_script_expr_run(world, "10 <= 5", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);

    test_assert(ecs_script_expr_run(world, "10 <= 10", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);

    test_assert(ecs_script_expr_run(world, "5 <= 10", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);

    test_assert(ecs_script_expr_run(world, "5 <= 5", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_cond_lteq_flt(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_script_expr_run(world, "10.5 <= 5.5", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);

    test_assert(ecs_script_expr_run(world, "10.5 <= 10.5", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);

    test_assert(ecs_script_expr_run(world, "5.5 <= 10.5", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);

    test_assert(ecs_script_expr_run(world, "5.5 <= 5.5", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_min_lparen_int_rparen(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_script_expr_run(world, "-(10)", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_i64_t));
    test_assert(v.ptr != NULL);
    test_int(*(ecs_i64_t*)v.ptr, -10);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_min_lparen_int_add_int_rparen(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_script_expr_run(world, "-(10 + 20)", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_i64_t));
    test_assert(v.ptr != NULL);
    test_int(*(ecs_i64_t*)v.ptr, -30);
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
    ecs_script_expr_run_desc_t desc = { .vars = vars };
    test_assert(ecs_script_expr_run(world, "-$foo", &v, &desc) != NULL);
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
    test_assert(ecs_script_expr_run(world, "-(10)", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_i64_t));
    test_assert(v.ptr != NULL);
    test_int(vi, -10);

    ecs_fini(world);
}

void Expr_min_lparen_int_rparen_to_i32(void) {
    ecs_world_t *world = ecs_init();

    ecs_i32_t vi = 0;
    ecs_value_t v = { .type = ecs_id(ecs_i32_t), .ptr = &vi };
    test_assert(ecs_script_expr_run(world, "-(10)", &v, NULL) != NULL);
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

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .members = {
            {"value", ecs_id(ecs_i32_t)}
        }
    });

    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *var = ecs_script_vars_define(
        vars, "foo", ecs_u64_t);
    *(ecs_u64_t*)var->value.ptr = 10;

    Mass v = {0};
    ecs_script_expr_run_desc_t desc = { .vars = vars };
    const char *ptr = ecs_script_expr_run(world, "{-$foo}", &(ecs_value_t){
        .type = t, .ptr = &v
    }, &desc);
    test_assert(ptr != NULL);
    test_assert(!ptr[0]);

    test_uint(v.value, -10);
    ecs_script_vars_fini(vars);

    ecs_fini(world);
}

void Expr_struct_w_min_lparen_int_rparen(void) {
    ecs_world_t *world = ecs_init();

    typedef struct {
        int32_t value;
    } Mass;

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .members = {
            {"value", ecs_id(ecs_i32_t)}
        }
    });

    Mass v = {0};
    const char *ptr = ecs_script_expr_run(world, "{-(10)}", &(ecs_value_t){
        .type = t, .ptr = &v
    }, NULL);
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

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .members = {
            {"value", ecs_id(ecs_i32_t)}
        }
    });

    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *var = ecs_script_vars_define(
        vars, "foo", ecs_u64_t);
    *(ecs_u64_t*)var->value.ptr = 10;

    Mass v = {0};
    ecs_script_expr_run_desc_t desc = { .vars = vars };
    const char *ptr = ecs_script_expr_run(world, "{-($foo)}", &(ecs_value_t){
        .type = t, .ptr = &v
    }, &desc);
    test_assert(ptr != NULL);
    test_assert(!ptr[0]);

    test_uint(v.value, -10);
    ecs_script_vars_fini(vars);

    ecs_fini(world);
}

void Expr_shift_left_int(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_script_expr_run(world, "1 << 2", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_u64_t));
    test_assert(v.ptr != NULL);
    test_uint(*(uint64_t*)v.ptr, 1 << 2);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_shift_right_int(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_script_expr_run(world, "4 >> 2", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_u64_t));
    test_assert(v.ptr != NULL);
    test_uint(*(uint64_t*)v.ptr, 4 >> 2);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_shift_left_int_add_int(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_script_expr_run(world, "1 << 2 + 10", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_u64_t));
    test_assert(v.ptr != NULL);
    test_uint(*(uint64_t*)v.ptr, 1 << (2 + 10));
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_shift_left_int_mul_int(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_script_expr_run(world, "1 << 2 * 10", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_u64_t));
    test_assert(v.ptr != NULL);
    test_uint(*(uint64_t*)v.ptr, 1 << 2 * 10);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_add_int_shift_left_int(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_script_expr_run(world, "10 + 1 << 2", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_u64_t));
    test_assert(v.ptr != NULL);
    test_uint(*(uint64_t*)v.ptr, (10 + 1) << 2);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_mul_int_shift_left_int(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_script_expr_run(world, "10 * 1 << 2", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_u64_t));
    test_assert(v.ptr != NULL);
    test_uint(*(uint64_t*)v.ptr, 10 * 1 << 2);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_add_int_shift_left_int_add_int(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_script_expr_run(world, "10 + 1 << 2 + 2", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_u64_t));
    test_assert(v.ptr != NULL);
    test_uint(*(uint64_t*)v.ptr, (10 + 1) << (2 + 2));
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_mul_int_shift_left_int_mul_int(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_script_expr_run(world, "10 * 1 << 2 * 2", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_u64_t));
    test_assert(v.ptr != NULL);
    test_uint(*(uint64_t*)v.ptr, 10 * 1 << 2 * 2);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_entity_expr(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t foo = ecs_entity(world, { .name = "foo" });
    test_assert(foo != 0);

    ecs_value_t v = {0};
    test_assert(ecs_script_expr_run(world, "foo", &v, NULL) != NULL);
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
    test_assert(ecs_script_expr_run(world, "parent.foo", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_entity_t));
    test_assert(v.ptr != NULL);
    test_uint(*(ecs_entity_t*)v.ptr, foo);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_entity_parent_func(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t parent = ecs_entity(world, { .name = "parent" });
    test_assert(parent != 0);

    ecs_entity_t foo = ecs_entity(world, { .name = "parent.foo" });
    test_assert(foo != 0);

    ecs_value_t v = {0};
    test_assert(ecs_script_expr_run(world, "parent.foo.parent()", &v, NULL) != NULL);
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
    test_assert(ecs_script_expr_run(world, "parent.foo.name()", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_string_t));
    test_assert(v.ptr != NULL);
    test_str(*(char**)v.ptr, "foo");
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Expr_entity_doc_name_func(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t parent = ecs_entity(world, { .name = "parent" });
    test_assert(parent != 0);

    ecs_entity_t foo = ecs_entity(world, { .name = "parent.foo" });
    test_assert(foo != 0);
    ecs_doc_set_name(world, foo, "FooDoc");

    ecs_value_t v = {0};
    test_assert(ecs_script_expr_run(world, "parent.foo.doc_name()", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_string_t));
    test_assert(v.ptr != NULL);
    test_str(*(char**)v.ptr, "FooDoc");
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
    test_assert(ecs_script_expr_run(world, "parent.foo.parent().name()", &v, NULL) != NULL);
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
    ecs_script_expr_run_desc_t desc = { .vars = vars };
    test_assert(ecs_script_expr_run(world, "$foo.parent()", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_entity_t));
    test_assert(v.ptr != NULL);
    test_uint(*(ecs_entity_t*)v.ptr, parent);
    ecs_value_free(world, v.type, v.ptr);

    ecs_script_vars_fini(vars);

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
    ecs_script_expr_run_desc_t desc = { .vars = vars };
    test_assert(ecs_script_expr_run(world, "$foo.name()", &v, &desc) != NULL);
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
    ecs_script_expr_run_desc_t desc = { .vars = vars };
    test_assert(ecs_script_expr_run(world, "$foo.doc_name()", &v, &desc) != NULL);
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
    ecs_script_expr_run_desc_t desc = { .vars = vars };
    test_assert(ecs_script_expr_run(world, "$foo.parent().name()", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_string_t));
    test_assert(v.ptr != NULL);
    test_str(*(char**)v.ptr, "parent");
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

    ecs_entity_t ecs_id(Position) = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "Position"}),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    ecs_entity_t e = ecs_entity(world, { .name = "e" });
    ecs_set(world, e, Position, {10, 20});

    ecs_value_t v = {0};
    test_assert(ecs_script_expr_run(world, "e[Position]", &v, NULL) != NULL);
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

    ecs_entity_t ecs_id(Position) = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "Position"}),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    ecs_entity_t e = ecs_entity(world, { .name = "e" });
    ecs_set(world, e, Position, {10, 20});

    ecs_value_t v = {0};
    test_assert(ecs_script_expr_run(world, "e[Position].x", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_f32_t));
    test_assert(v.ptr != NULL);
    {
        float *ptr = v.ptr;
        test_int(*ptr, 10);
        ecs_value_free(world, v.type, v.ptr);
    }

    v = (ecs_value_t){0};
    test_assert(ecs_script_expr_run(world, "e[Position].y", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_f32_t));
    test_assert(v.ptr != NULL);
    {
        float *ptr = v.ptr;
        test_int(*ptr, 20);
        ecs_value_free(world, v.type, v.ptr);
    }

    ecs_fini(world);
}
