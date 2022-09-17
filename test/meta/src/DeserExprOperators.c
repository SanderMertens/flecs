#include <meta.h>

void DeserExprOperators_add_2_int_literals() {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_parse_expr(world, "10 + 20", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_u64_t));
    test_assert(v.ptr != NULL);
    test_uint(*(uint64_t*)v.ptr, 10 + 20);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void DeserExprOperators_add_2_int_literals_twice() {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_parse_expr(world, "10 + 20", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_u64_t));
    test_assert(v.ptr != NULL);
    test_uint(*(uint64_t*)v.ptr, 10 + 20);

    test_assert(ecs_parse_expr(world, "10 + 20", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_u64_t));
    test_assert(v.ptr != NULL);
    test_uint(*(uint64_t*)v.ptr, 10 + 20);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void DeserExprOperators_sub_2_int_literals() {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_parse_expr(world, "20 - 10", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_i64_t));
    test_assert(v.ptr != NULL);
    test_int(*(int64_t*)v.ptr, 20 - 10);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void DeserExprOperators_mul_2_int_literals() {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_parse_expr(world, "20 * 10", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_u64_t));
    test_assert(v.ptr != NULL);
    test_uint(*(ecs_u64_t*)v.ptr, 20 * 10);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void DeserExprOperators_div_2_int_literals() {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_parse_expr(world, "10 / 2", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_f64_t));
    test_assert(v.ptr != NULL);
    test_flt(*(ecs_f64_t*)v.ptr, 10 / 2);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void DeserExprOperators_add_3_int_literals() {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_parse_expr(world, "10 + 20 + 30", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_u64_t));
    test_assert(v.ptr != NULL);
    test_uint(*(uint64_t*)v.ptr, 10 + 20 + 30);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void DeserExprOperators_add_3_int_literals_twice() {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_parse_expr(world, "10 + 20 + 30", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_u64_t));
    test_assert(v.ptr != NULL);
    test_uint(*(uint64_t*)v.ptr, 10 + 20 + 30);

    test_assert(ecs_parse_expr(world, "10 + 20 + 30", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_u64_t));
    test_assert(v.ptr != NULL);
    test_uint(*(uint64_t*)v.ptr, 10 + 20 + 30);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void DeserExprOperators_sub_3_int_literals() {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_parse_expr(world, "30 - 10 - 5", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_i64_t));
    test_assert(v.ptr != NULL);
    test_int(*(ecs_i64_t*)v.ptr, 30 - 10 - 5);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void DeserExprOperators_mul_3_int_literals() {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_parse_expr(world, "2 * 5 * 10", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_u64_t));
    test_assert(v.ptr != NULL);
    test_uint(*(ecs_u64_t*)v.ptr, 2 * 5 * 10);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void DeserExprOperators_div_3_int_literals() {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_parse_expr(world, "40 / 5 / 2", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_f64_t));
    test_assert(v.ptr != NULL);
    test_flt(*(ecs_f64_t*)v.ptr, 40 / 5 / 2);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void DeserExprOperators_int_to_bool() {
    ecs_world_t *world = ecs_init();

    bool b = false;
    ecs_value_t v = { .type = ecs_id(ecs_bool_t), .ptr = &b };

    test_assert(ecs_parse_expr(world, "10", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_flt(*(ecs_bool_t*)v.ptr, true);

    test_assert(ecs_parse_expr(world, "0", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_flt(*(ecs_bool_t*)v.ptr, false);

    test_assert(ecs_parse_expr(world, "256", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_flt(*(ecs_bool_t*)v.ptr, true);

    ecs_fini(world);
}

void DeserExprOperators_add_mul_3_int_literals() {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_parse_expr(world, "10 + 20 * 2", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_u64_t));
    test_assert(v.ptr != NULL);
    test_uint(*(uint64_t*)v.ptr, 10 + 20 * 2);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void DeserExprOperators_sub_mul_3_int_literals() {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_parse_expr(world, "50 - 10 * 2", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_i64_t));
    test_assert(v.ptr != NULL);
    test_int(*(int64_t*)v.ptr, 50 - 10 * 2);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void DeserExprOperators_div_mul_3_int_literals() {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_parse_expr(world, "10 / 5 * 2", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_f64_t));
    test_assert(v.ptr != NULL);
    test_flt(*(ecs_f64_t*)v.ptr, 10 / 5 * 2);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void DeserExprOperators_add_div_3_int_literals() {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_parse_expr(world, "10 + 30 / 2", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_f64_t));
    test_assert(v.ptr != NULL);
    test_uint(*(ecs_f64_t*)v.ptr, 10 + 30 / 2);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void DeserExprOperators_sub_div_3_int_literals() {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_parse_expr(world, "30 - 10 / 2", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_f64_t));
    test_assert(v.ptr != NULL);
    test_uint(*(ecs_f64_t*)v.ptr, 30 - 10 / 2);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void DeserExprOperators_mul_div_3_int_literals() {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_parse_expr(world, "20 * 10 / 2", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_f64_t));
    test_assert(v.ptr != NULL);
    test_uint(*(ecs_f64_t*)v.ptr, 20 * 10 / 2);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void DeserExprOperators_mul_add_mul_add_int_literals() {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_parse_expr(world, "2 * 4 + 6 * 8 + 10", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_u64_t));
    test_assert(v.ptr != NULL);
    test_uint(*(ecs_u64_t*)v.ptr, 2 * 4 + 6 * 8 + 10);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void DeserExprOperators_mul_sub_mul_sub_int_literals() {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_parse_expr(world, "2 * 4 - 6 * 8 - 10", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_i64_t));
    test_assert(v.ptr != NULL);
    test_int(*(ecs_i64_t*)v.ptr, 2 * 4 - 6 * 8 - 10);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void DeserExprOperators_mul_div_mul_div_int_literals() {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_parse_expr(world, "2 * 4 / 6 * 8 / 10", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_f64_t));
    test_assert(v.ptr != NULL);
    test_flt(*(ecs_f64_t*)v.ptr, 2.0 * 4.0 / 6.0 * 8.0 / 10.0);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void DeserExprOperators_div_add_div_add_int_literals() {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_parse_expr(world, "2 / 4 + 6 / 8 + 10", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_f64_t));
    test_assert(v.ptr != NULL);
    test_flt(*(ecs_f64_t*)v.ptr, 2.0 / 4.0 + 6.0 / 8.0 + 10.0);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void DeserExprOperators_div_sub_div_sub_int_literals() {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_parse_expr(world, "2 / 4 - 6 / 8 - 10", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_f64_t));
    test_assert(v.ptr != NULL);
    test_flt(*(ecs_f64_t*)v.ptr, 2.0 / 4.0 - 6.0 / 8.0 - 10.0);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void DeserExprOperators_div_mul_div_mul_int_literals() {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_parse_expr(world, "2 / 4 * 6 / 8 * 10", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_f64_t));
    test_assert(v.ptr != NULL);
    test_flt(*(ecs_f64_t*)v.ptr, 2.0 / 4.0 * 6.0 / 8.0 * 10.0);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void DeserExprOperators_div_sub_div_mul_int_literals() {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_parse_expr(world, "2 / 4 - 6 / 8 * 10", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_f64_t));
    test_assert(v.ptr != NULL);
    test_flt(*(ecs_f64_t*)v.ptr, 2.0 / 4.0 - 6.0 / 8.0 * 10.0);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void DeserExprOperators_add_2_flt_literals() {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_parse_expr(world, "10.5 + 20.0", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_f64_t));
    test_assert(v.ptr != NULL);
    test_flt(*(ecs_f64_t*)v.ptr, 10.5 + 20.0);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void DeserExprOperators_sub_2_flt_literals() {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_parse_expr(world, "20.5 - 10.0", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_f64_t));
    test_assert(v.ptr != NULL);
    test_flt(*(ecs_f64_t*)v.ptr, 20.5 - 10.0);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void DeserExprOperators_mul_2_flt_literals() {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_parse_expr(world, "20.5 * 10.0", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_f64_t));
    test_assert(v.ptr != NULL);
    test_flt(*(ecs_f64_t*)v.ptr, 20.5 * 10.0);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void DeserExprOperators_div_2_flt_literals() {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_parse_expr(world, "10.5 / 2.0", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_f64_t));
    test_assert(v.ptr != NULL);
    test_flt(*(ecs_f64_t*)v.ptr, 10.5 / 2.0);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void DeserExprOperators_add_2_int_neg_literals() {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_parse_expr(world, "-10 + -20", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_i64_t));
    test_assert(v.ptr != NULL);
    test_int(*(ecs_i64_t*)v.ptr, -10 + -20);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void DeserExprOperators_sub_2_int_neg_literals() {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_parse_expr(world, "-10 - -20", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_i64_t));
    test_assert(v.ptr != NULL);
    test_int(*(ecs_i64_t*)v.ptr, -10 - -20);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void DeserExprOperators_mul_2_int_neg_literals() {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_parse_expr(world, "-10 * -20", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_i64_t));
    test_assert(v.ptr != NULL);
    test_int(*(ecs_i64_t*)v.ptr, -10 * -20);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void DeserExprOperators_div_2_int_neg_literals() {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_parse_expr(world, "-10 / -20", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_f64_t));
    test_assert(v.ptr != NULL);
    test_flt(*(ecs_f64_t*)v.ptr, -10.0 / -20.0);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void DeserExprOperators_mul_lparen_add_add_rparen_int_literals() {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_parse_expr(world, "10 * (20 + 30)", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_u64_t));
    test_assert(v.ptr != NULL);
    test_uint(*(ecs_u64_t*)v.ptr, 10 * (20 + 30));
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void DeserExprOperators_mul_lparen_add_add_add_rparen_int_literals() {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_parse_expr(world, "10 * (20 + 30 + 40)", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_u64_t));
    test_assert(v.ptr != NULL);
    test_uint(*(ecs_u64_t*)v.ptr, 10 * (20 + 30 + 40));
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void DeserExprOperators_mul_lparen_add_add_rparen_add_int_literals() {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_parse_expr(world, "10 * (20 + 30) + 40", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_u64_t));
    test_assert(v.ptr != NULL);
    test_uint(*(ecs_u64_t*)v.ptr, 10 * (20 + 30) + 40);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void DeserExprOperators_lparen_add_add_rparen_mul_int_literals() {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_parse_expr(world, "(20 + 30) * 10", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_u64_t));
    test_assert(v.ptr != NULL);
    test_uint(*(ecs_u64_t*)v.ptr, (20 + 30) * 10);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void DeserExprOperators_lparen_add_add_add_rparen_mul_int_literals() {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_parse_expr(world, "(20 + 30 + 40) * 10", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_u64_t));
    test_assert(v.ptr != NULL);
    test_uint(*(ecs_u64_t*)v.ptr, (20 + 30 + 40) * 10);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void DeserExprOperators_double_paren_add_add() {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_parse_expr(world, "((20 + 30))", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_u64_t));
    test_assert(v.ptr != NULL);
    test_uint(*(ecs_u64_t*)v.ptr, ((20 + 30)));
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void DeserExprOperators_double_paren_literal() {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_parse_expr(world, "((20))", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_u64_t));
    test_assert(v.ptr != NULL);
    test_uint(*(ecs_u64_t*)v.ptr, ((20)));
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void DeserExprOperators_lparen_add_add_rparen_mul_lparen_add_add_rparen() {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_parse_expr(world, "(10 + 20) * (20 + 30)", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_u64_t));
    test_assert(v.ptr != NULL);
    test_uint(*(ecs_u64_t*)v.ptr, (10 + 20) * (20 + 30));
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void DeserExprOperators_float_result_add_2_int_literals() {
    ecs_world_t *world = ecs_init();

    float value = 0;
    ecs_value_t v = {
        .type = ecs_id(ecs_f32_t),
        .ptr = &value
    };
    test_assert(ecs_parse_expr(world, "10 + 20", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_f32_t));
    test_assert(v.ptr != NULL);
    test_flt(value, 10 + 20);

    ecs_fini(world);
}

void DeserExprOperators_struct_result_add_2_int_literals() {
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
    const char *ptr = ecs_parse_expr(world, "{10 + 20}", &(ecs_value_t){
        .type = t, .ptr = &v
    }, NULL);
    test_assert(ptr != NULL);
    test_assert(!ptr[0]);

    test_uint(v.value, 10 + 20);

    ecs_fini(world);
}

void DeserExprOperators_struct_result_add_2_2_fields_int_literals() {
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
    const char *ptr = ecs_parse_expr(world, "{10 + 20, 20 + 30}", &(ecs_value_t){
        .type = t, .ptr = &v
    }, NULL);
    test_assert(ptr != NULL);
    test_assert(!ptr[0]);

    test_uint(v.x, 10 + 20);
    test_uint(v.y, 20 + 30);

    ecs_fini(world);
}

void DeserExprOperators_struct_result_add_3_int_literals() {
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
    const char *ptr = ecs_parse_expr(world, "{10 + 20 + 30}", &(ecs_value_t){
        .type = t, .ptr = &v
    }, NULL);
    test_assert(ptr != NULL);
    test_assert(!ptr[0]);

    test_uint(v.value, 10 + 20 + 30);

    ecs_fini(world);
}

void DeserExprOperators_struct_result_lparen_int_rparen() {
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
    const char *ptr = ecs_parse_expr(world, "{(10)}", &(ecs_value_t){
        .type = t, .ptr = &v
    }, NULL);
    test_assert(ptr != NULL);
    test_assert(!ptr[0]);

    test_uint(v.value, 10);

    ecs_fini(world);
}

void DeserExprOperators_add_to_var() {
    ecs_world_t *world = ecs_init();

    ecs_vars_t vars = {0};
    ecs_vars_init(world, &vars);

    ecs_expr_var_t *var = ecs_vars_declare(
        &vars, "foo", ecs_id(ecs_i32_t));
    *(int32_t*)var->value.ptr = 10;

    ecs_value_t v = {0};
    ecs_parse_expr_desc_t desc = { .vars = &vars };
    const char *ptr = ecs_parse_expr(world, "$foo + 20", &v, &desc);
    test_assert(ptr != NULL);
    test_assert(!ptr[0]);
    test_int(*(int32_t*)v.ptr, 10 + 20);
    ecs_value_free(world, v.type, v.ptr);

    ecs_vars_fini(&vars);
    ecs_fini(world);
}

void DeserExprOperators_add_var_to() {
    ecs_world_t *world = ecs_init();

    ecs_vars_t vars = {0};
    ecs_vars_init(world, &vars);

    ecs_expr_var_t *var = ecs_vars_declare(
        &vars, "foo", ecs_id(ecs_i32_t));
    *(int32_t*)var->value.ptr = 10;

    ecs_value_t v = {0};
    ecs_parse_expr_desc_t desc = { .vars = &vars };
    const char *ptr = ecs_parse_expr(world, "20 + $foo", &v, &desc);
    test_assert(ptr != NULL);
    test_assert(!ptr[0]);
    test_int(*(int32_t*)v.ptr, 20 + 10);
    ecs_value_free(world, v.type, v.ptr);

    ecs_vars_fini(&vars);
    ecs_fini(world);
}

void DeserExprOperators_bool_cond_and_bool() {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_parse_expr(world, "true && true", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);

    test_assert(ecs_parse_expr(world, "true && false", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);

    test_assert(ecs_parse_expr(world, "false && true", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);

    test_assert(ecs_parse_expr(world, "false && false", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void DeserExprOperators_bool_cond_or_bool() {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_parse_expr(world, "true || true", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);

    test_assert(ecs_parse_expr(world, "true || false", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);

    test_assert(ecs_parse_expr(world, "false || true", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);

    test_assert(ecs_parse_expr(world, "false || false", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void DeserExprOperators_int_cond_and_int() {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_parse_expr(world, "10 && 10", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);

    test_assert(ecs_parse_expr(world, "10 && 0", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);

    test_assert(ecs_parse_expr(world, "0 && 10", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);

    test_assert(ecs_parse_expr(world, "0 && 0", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void DeserExprOperators_int_cond_or_int() {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_parse_expr(world, "10 || 10", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);

    test_assert(ecs_parse_expr(world, "10 || 0", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);

    test_assert(ecs_parse_expr(world, "0 || 10", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);

    test_assert(ecs_parse_expr(world, "0 || 0", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void DeserExprOperators_bool_cond_and_int() {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_parse_expr(world, "true && 10", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);

    test_assert(ecs_parse_expr(world, "true && 0", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);

    test_assert(ecs_parse_expr(world, "false && 10", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);

    test_assert(ecs_parse_expr(world, "false && 0", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void DeserExprOperators_int_cond_and_bool() {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_parse_expr(world, "10 && true", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);

    test_assert(ecs_parse_expr(world, "10 && false", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);

    test_assert(ecs_parse_expr(world, "0 && true", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);

    test_assert(ecs_parse_expr(world, "0 && false", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void DeserExprOperators_bool_cond_or_int() {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_parse_expr(world, "true || 10", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);

    test_assert(ecs_parse_expr(world, "true || 0", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);

    test_assert(ecs_parse_expr(world, "false || 10", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);

    test_assert(ecs_parse_expr(world, "false || 0", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void DeserExprOperators_int_cond_or_bool() {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_parse_expr(world, "10 || true", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);

    test_assert(ecs_parse_expr(world, "10 || false", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);

    test_assert(ecs_parse_expr(world, "0 || true", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);

    test_assert(ecs_parse_expr(world, "0 || false", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void DeserExprOperators_cond_eq_bool() {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_parse_expr(world, "true == true", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);

    test_assert(ecs_parse_expr(world, "true == false", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);

    test_assert(ecs_parse_expr(world, "false == true", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);

    test_assert(ecs_parse_expr(world, "false == false", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void DeserExprOperators_cond_eq_int() {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_parse_expr(world, "10 == 10", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);

    test_assert(ecs_parse_expr(world, "10 == 20", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);

    test_assert(ecs_parse_expr(world, "10 == 0", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);

    test_assert(ecs_parse_expr(world, "0 == 0", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void DeserExprOperators_cond_neq_bool() {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_parse_expr(world, "true != true", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);

    test_assert(ecs_parse_expr(world, "true != false", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);

    test_assert(ecs_parse_expr(world, "false != true", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);

    test_assert(ecs_parse_expr(world, "false != false", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void DeserExprOperators_cond_neq_int() {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_parse_expr(world, "10 != 10", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);

    test_assert(ecs_parse_expr(world, "10 != 20", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);

    test_assert(ecs_parse_expr(world, "10 != 0", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);

    test_assert(ecs_parse_expr(world, "0 != 0", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void DeserExprOperators_cond_eq_bool_int() {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};

    ecs_log_set_level(-4);
    test_assert(ecs_parse_expr(world, "true == 1", &v, NULL) == NULL);
    test_assert(ecs_parse_expr(world, "true == 0", &v, NULL) == NULL);
    test_assert(ecs_parse_expr(world, "false == 0", &v, NULL) == NULL);

    ecs_fini(world);
}

void DeserExprOperators_cond_eq_int_flt() {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};

    ecs_log_set_level(-4);
    test_assert(ecs_parse_expr(world, "1 == 1.0", &v, NULL) == NULL);
    test_assert(ecs_parse_expr(world, "1 == 0.0", &v, NULL) == NULL);
    test_assert(ecs_parse_expr(world, "0 == 0.0", &v, NULL) == NULL);

    ecs_fini(world);
}

void DeserExprOperators_cond_eq_cond_and() {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_parse_expr(world, "true == true && false", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true == true && false);

    test_assert(ecs_parse_expr(world, "true && false == false", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true && false == false);

    test_assert(ecs_parse_expr(world, "true && true == true", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true && true == true);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void DeserExprOperators_cond_eq_cond_or() {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_parse_expr(world, "true == true || false", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true == true || false);

    test_assert(ecs_parse_expr(world, "true || false == false", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true || false == false);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void DeserExprOperators_cond_gt_bool() {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_parse_expr(world, "true > false", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);

    test_assert(ecs_parse_expr(world, "true > true", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);

    test_assert(ecs_parse_expr(world, "false > true", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);

    test_assert(ecs_parse_expr(world, "false > false", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void DeserExprOperators_cond_gt_int() {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_parse_expr(world, "10 > 5", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);

    test_assert(ecs_parse_expr(world, "10 > 10", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);

    test_assert(ecs_parse_expr(world, "5 > 10", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);

    test_assert(ecs_parse_expr(world, "5 > 5", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void DeserExprOperators_cond_gt_flt() {
        ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_parse_expr(world, "10.5 > 5.5", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);

    test_assert(ecs_parse_expr(world, "10.5 > 10.5", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);

    test_assert(ecs_parse_expr(world, "5.5 > 10.5", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);

    test_assert(ecs_parse_expr(world, "5.5 > 5.5", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void DeserExprOperators_cond_gteq_bool() {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_parse_expr(world, "true >= false", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);

    test_assert(ecs_parse_expr(world, "true >= true", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);

    test_assert(ecs_parse_expr(world, "false >= true", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);

    test_assert(ecs_parse_expr(world, "false >= false", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void DeserExprOperators_cond_gteq_int() {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_parse_expr(world, "10 >= 5", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);

    test_assert(ecs_parse_expr(world, "10 >= 10", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);

    test_assert(ecs_parse_expr(world, "5 >= 10", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);

    test_assert(ecs_parse_expr(world, "5 >= 5", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void DeserExprOperators_cond_gteq_flt() {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_parse_expr(world, "10.5 >= 5.5", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);

    test_assert(ecs_parse_expr(world, "10.5 >= 10.5", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);

    test_assert(ecs_parse_expr(world, "5.5 >= 10.5", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);

    test_assert(ecs_parse_expr(world, "5.5 >= 5.5", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void DeserExprOperators_cond_lt_bool() {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_parse_expr(world, "true < false", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);

    test_assert(ecs_parse_expr(world, "true < true", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);

    test_assert(ecs_parse_expr(world, "false < true", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);

    test_assert(ecs_parse_expr(world, "false < false", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void DeserExprOperators_cond_lt_int() {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_parse_expr(world, "10 < 5", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);

    test_assert(ecs_parse_expr(world, "10 < 10", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);

    test_assert(ecs_parse_expr(world, "5 < 10", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);

    test_assert(ecs_parse_expr(world, "5 < 5", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void DeserExprOperators_cond_lt_flt() {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_parse_expr(world, "10.5 < 5.5", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);

    test_assert(ecs_parse_expr(world, "10.5 < 10.5", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);

    test_assert(ecs_parse_expr(world, "5.5 < 10.5", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);

    test_assert(ecs_parse_expr(world, "5.5 < 5.5", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void DeserExprOperators_cond_lteq_bool() {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_parse_expr(world, "true <= false", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);

    test_assert(ecs_parse_expr(world, "true <= true", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);

    test_assert(ecs_parse_expr(world, "false <= true", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);

    test_assert(ecs_parse_expr(world, "false <= false", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void DeserExprOperators_cond_lteq_int() {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_parse_expr(world, "10 <= 5", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);

    test_assert(ecs_parse_expr(world, "10 <= 10", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);

    test_assert(ecs_parse_expr(world, "5 <= 10", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);

    test_assert(ecs_parse_expr(world, "5 <= 5", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void DeserExprOperators_cond_lteq_flt() {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_parse_expr(world, "10.5 <= 5.5", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, false);

    test_assert(ecs_parse_expr(world, "10.5 <= 10.5", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);

    test_assert(ecs_parse_expr(world, "5.5 <= 10.5", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);

    test_assert(ecs_parse_expr(world, "5.5 <= 5.5", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(bool*)v.ptr, true);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void DeserExprOperators_min_lparen_int_rparen() {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_parse_expr(world, "-(10)", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_i64_t));
    test_assert(v.ptr != NULL);
    test_int(*(ecs_i64_t*)v.ptr, -10);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void DeserExprOperators_min_lparen_int_add_int_rparen() {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_parse_expr(world, "-(10 + 20)", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_i64_t));
    test_assert(v.ptr != NULL);
    test_int(*(ecs_i64_t*)v.ptr, -30);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void DeserExprOperators_min_var() {
    ecs_world_t *world = ecs_init();

    ecs_vars_t vars = {0};
    ecs_vars_init(world, &vars);

    ecs_expr_var_t *var = ecs_vars_declare(
        &vars, "foo", ecs_id(ecs_u64_t));
    *(ecs_u64_t*)var->value.ptr = 10;

    ecs_value_t v = {0};
    ecs_parse_expr_desc_t desc = { .vars = &vars };
    test_assert(ecs_parse_expr(world, "-$foo", &v, &desc) != NULL);
    test_assert(v.type == ecs_id(ecs_i64_t));
    test_assert(v.ptr != NULL);
    test_int(*(ecs_i64_t*)v.ptr, -10);
    ecs_value_free(world, v.type, v.ptr);

    ecs_vars_fini(&vars);

    ecs_fini(world);
}

void DeserExprOperators_min_lparen_int_rparen_to_i64() {
    ecs_world_t *world = ecs_init();

    ecs_i64_t vi = 0;
    ecs_value_t v = { .type = ecs_id(ecs_i64_t), .ptr = &vi };
    test_assert(ecs_parse_expr(world, "-(10)", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_i64_t));
    test_assert(v.ptr != NULL);
    test_int(vi, -10);

    ecs_fini(world);
}

void DeserExprOperators_min_lparen_int_rparen_to_i32() {
    ecs_world_t *world = ecs_init();

    ecs_i32_t vi = 0;
    ecs_value_t v = { .type = ecs_id(ecs_i32_t), .ptr = &vi };
    test_assert(ecs_parse_expr(world, "-(10)", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_i32_t));
    test_assert(v.ptr != NULL);
    test_int(vi, -10);

    ecs_fini(world);
}

void DeserExprOperators_struct_w_min_var() {
    ecs_world_t *world = ecs_init();

    typedef struct {
        int32_t value;
    } Mass;

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .members = {
            {"value", ecs_id(ecs_i32_t)}
        }
    });

    ecs_vars_t vars = {0};
    ecs_vars_init(world, &vars);

    ecs_expr_var_t *var = ecs_vars_declare(
        &vars, "foo", ecs_id(ecs_u64_t));
    *(ecs_u64_t*)var->value.ptr = 10;

    Mass v = {0};
    ecs_parse_expr_desc_t desc = { .vars = &vars };
    const char *ptr = ecs_parse_expr(world, "{-$foo}", &(ecs_value_t){
        .type = t, .ptr = &v
    }, &desc);
    test_assert(ptr != NULL);
    test_assert(!ptr[0]);

    test_uint(v.value, -10);
    ecs_vars_fini(&vars);

    ecs_fini(world);
}

void DeserExprOperators_struct_w_min_lparen_int_rparen() {
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
    const char *ptr = ecs_parse_expr(world, "{-(10)}", &(ecs_value_t){
        .type = t, .ptr = &v
    }, NULL);
    test_assert(ptr != NULL);
    test_assert(!ptr[0]);

    test_uint(v.value, -10);

    ecs_fini(world);
}

void DeserExprOperators_struct_w_min_lparen_var_rparen() {
    ecs_world_t *world = ecs_init();

    typedef struct {
        int32_t value;
    } Mass;

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .members = {
            {"value", ecs_id(ecs_i32_t)}
        }
    });

    ecs_vars_t vars = {0};
    ecs_vars_init(world, &vars);

    ecs_expr_var_t *var = ecs_vars_declare(
        &vars, "foo", ecs_id(ecs_u64_t));
    *(ecs_u64_t*)var->value.ptr = 10;

    Mass v = {0};
    ecs_parse_expr_desc_t desc = { .vars = &vars };
    const char *ptr = ecs_parse_expr(world, "{-($foo)}", &(ecs_value_t){
        .type = t, .ptr = &v
    }, &desc);
    test_assert(ptr != NULL);
    test_assert(!ptr[0]);

    test_uint(v.value, -10);
    ecs_vars_fini(&vars);

    ecs_fini(world);
}

void DeserExprOperators_shift_left_int() {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_parse_expr(world, "1 << 2", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_u64_t));
    test_assert(v.ptr != NULL);
    test_uint(*(uint64_t*)v.ptr, 1 << 2);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void DeserExprOperators_shift_right_int() {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_parse_expr(world, "4 >> 2", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_u64_t));
    test_assert(v.ptr != NULL);
    test_uint(*(uint64_t*)v.ptr, 4 >> 2);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void DeserExprOperators_shift_left_int_add_int() {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_parse_expr(world, "1 << 2 + 10", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_u64_t));
    test_assert(v.ptr != NULL);
    test_uint(*(uint64_t*)v.ptr, 1 << (2 + 10));
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void DeserExprOperators_shift_left_int_mul_int() {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_parse_expr(world, "1 << 2 * 10", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_u64_t));
    test_assert(v.ptr != NULL);
    test_uint(*(uint64_t*)v.ptr, 1 << 2 * 10);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void DeserExprOperators_add_int_shift_left_int() {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_parse_expr(world, "10 + 1 << 2", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_u64_t));
    test_assert(v.ptr != NULL);
    test_uint(*(uint64_t*)v.ptr, (10 + 1) << 2);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void DeserExprOperators_mul_int_shift_left_int() {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_parse_expr(world, "10 * 1 << 2", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_u64_t));
    test_assert(v.ptr != NULL);
    test_uint(*(uint64_t*)v.ptr, 10 * 1 << 2);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void DeserExprOperators_add_int_shift_left_int_add_int() {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_parse_expr(world, "10 + 1 << 2 + 2", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_u64_t));
    test_assert(v.ptr != NULL);
    test_uint(*(uint64_t*)v.ptr, (10 + 1) << (2 + 2));
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void DeserExprOperators_mul_int_shift_left_int_mul_int() {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_parse_expr(world, "10 * 1 << 2 * 2", &v, NULL) != NULL);
    test_assert(v.type == ecs_id(ecs_u64_t));
    test_assert(v.ptr != NULL);
    test_uint(*(uint64_t*)v.ptr, 10 * 1 << 2 * 2);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}
