#include <script.h>

static bool ir_enabled = false;
static ecs_script_eval_desc_t ir_desc = {0};

void Lambda_setup(void) {
    const char *ir_param = test_param("ir");
    ir_enabled = ir_param && !strcmp(ir_param, "enabled");
    ir_desc = (ecs_script_eval_desc_t){ .ir = ir_enabled };
}

void Lambda_map_vector(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Result(v: vector(i64))"
    LINE "const a = [1, 2, 3]"
    LINE "const b = a.map((el) -> {"
    LINE "    el + 1"
    LINE "})"
    LINE "e { Result: {b} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t result = ecs_lookup(world, "Result");
    test_assert(e != 0);
    test_assert(result != 0);
    const void *ptr = ecs_get_id(world, e, result);
    test_assert(ptr != NULL);
    char *str = ecs_ptr_to_expr(world, result, ptr);
    test_str(str, "{v: [2, 3, 4]}");
    ecs_os_free(str);

    ecs_fini(world);
}

void Lambda_map_vector_single_line(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Result(v: vector(i64))"
    LINE "const a = [1, 2, 3]"
    LINE "e { Result: {a.map((el) -> { el * 2 })} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t result = ecs_lookup(world, "Result");
    test_assert(e != 0);
    test_assert(result != 0);
    const void *ptr = ecs_get_id(world, e, result);
    test_assert(ptr != NULL);
    char *str = ecs_ptr_to_expr(world, result, ptr);
    test_str(str, "{v: [2, 4, 6]}");
    ecs_os_free(str);

    ecs_fini(world);
}

void Lambda_map_vector_w_index(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Result(v: vector(i64))"
    LINE "const a = [10, 20, 30]"
    LINE "e { Result: {a.map((i, el) -> { el + i })} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t result = ecs_lookup(world, "Result");
    test_assert(e != 0);
    test_assert(result != 0);
    const void *ptr = ecs_get_id(world, e, result);
    test_assert(ptr != NULL);
    char *str = ecs_ptr_to_expr(world, result, ptr);
    test_str(str, "{v: [10, 21, 32]}");
    ecs_os_free(str);

    ecs_fini(world);
}

void Lambda_map_vector_literal(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Result(v: vector(i64))"
    LINE "e { Result: {[1, 2, 3].map((el) -> { el * el })} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t result = ecs_lookup(world, "Result");
    test_assert(e != 0);
    test_assert(result != 0);
    const void *ptr = ecs_get_id(world, e, result);
    test_assert(ptr != NULL);
    char *str = ecs_ptr_to_expr(world, result, ptr);
    test_str(str, "{v: [1, 4, 9]}");
    ecs_os_free(str);

    ecs_fini(world);
}

void Lambda_map_vector_to_string(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Result(v: vector(string))"
    LINE "const a = [1, 2, 3]"
    LINE "e { Result: {a.map((el) -> { \"e{el}\" })} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t result = ecs_lookup(world, "Result");
    test_assert(e != 0);
    test_assert(result != 0);
    const void *ptr = ecs_get_id(world, e, result);
    test_assert(ptr != NULL);
    char *str = ecs_ptr_to_expr(world, result, ptr);
    test_str(str, "{v: [\"e1\", \"e2\", \"e3\"]}");
    ecs_os_free(str);

    ecs_fini(world);
}

void Lambda_map_string_vector(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Result(v: vector(string))"
    LINE "const a = [\"a\", \"b\", \"c\"]"
    LINE "e { Result: {a.map((el) -> { \"{el}{el}\" })} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t result = ecs_lookup(world, "Result");
    test_assert(e != 0);
    test_assert(result != 0);
    const void *ptr = ecs_get_id(world, e, result);
    test_assert(ptr != NULL);
    char *str = ecs_ptr_to_expr(world, result, ptr);
    test_str(str, "{v: [\"aa\", \"bb\", \"cc\"]}");
    ecs_os_free(str);

    ecs_fini(world);
}

void Lambda_map_array(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Result(v: vector(i64))"
    LINE "const a: array(i64, 3) = [1, 2, 3]"
    LINE "e { Result: {a.map((el) -> { el + 10 })} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t result = ecs_lookup(world, "Result");
    test_assert(e != 0);
    test_assert(result != 0);
    const void *ptr = ecs_get_id(world, e, result);
    test_assert(ptr != NULL);
    char *str = ecs_ptr_to_expr(world, result, ptr);
    test_str(str, "{v: [11, 12, 13]}");
    ecs_os_free(str);

    ecs_fini(world);
}

void Lambda_map_array_w_index(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Result(v: vector(i64))"
    LINE "const a: array(i64, 3) = [1, 2, 3]"
    LINE "e { Result: {a.map((i, el) -> { el * i })} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t result = ecs_lookup(world, "Result");
    test_assert(e != 0);
    test_assert(result != 0);
    const void *ptr = ecs_get_id(world, e, result);
    test_assert(ptr != NULL);
    char *str = ecs_ptr_to_expr(world, result, ptr);
    test_str(str, "{v: [0, 2, 6]}");
    ecs_os_free(str);

    ecs_fini(world);
}

void Lambda_map_empty_vector(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Result(v: vector(i64))"
    LINE "const a: vector(i64) = []"
    LINE "e { Result: {a.map((el) -> { el + 1 })} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t result = ecs_lookup(world, "Result");
    test_assert(e != 0);
    test_assert(result != 0);
    const void *ptr = ecs_get_id(world, e, result);
    test_assert(ptr != NULL);
    char *str = ecs_ptr_to_expr(world, result, ptr);
    test_str(str, "{v: []}");
    ecs_os_free(str);

    ecs_fini(world);
}

void Lambda_map_struct(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Result(v: vector(i64))"
    LINE "struct Position(x: i64, y: i64)"
    LINE "const a: vector(Position) = [{1, 2}, {3, 4}]"
    LINE "e { Result: {a.map((p) -> { p.x + p.y })} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t result = ecs_lookup(world, "Result");
    test_assert(e != 0);
    test_assert(result != 0);
    const void *ptr = ecs_get_id(world, e, result);
    test_assert(ptr != NULL);
    char *str = ecs_ptr_to_expr(world, result, ptr);
    test_str(str, "{v: [3, 7]}");
    ecs_os_free(str);

    ecs_fini(world);
}

void Lambda_map_map(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Result(v: vector(i32))"
    LINE "const m: map(i32, i32) = [1: 10]"
    LINE "e { Result: {m.map((v) -> { v * 2 })} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t result = ecs_lookup(world, "Result");
    test_assert(e != 0);
    test_assert(result != 0);
    const void *ptr = ecs_get_id(world, e, result);
    test_assert(ptr != NULL);
    char *str = ecs_ptr_to_expr(world, result, ptr);
    test_str(str, "{v: [20]}");
    ecs_os_free(str);

    ecs_fini(world);
}

void Lambda_map_map_w_key(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Result(v: vector(i32))"
    LINE "const m: map(i32, i32) = [1: 10]"
    LINE "e { Result: {m.map((k, v) -> { k * 100 + v })} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t result = ecs_lookup(world, "Result");
    test_assert(e != 0);
    test_assert(result != 0);
    const void *ptr = ecs_get_id(world, e, result);
    test_assert(ptr != NULL);
    char *str = ecs_ptr_to_expr(world, result, ptr);
    test_str(str, "{v: [110]}");
    ecs_os_free(str);

    ecs_fini(world);
}

void Lambda_map_map_multiple(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Result(v: i32)"
    LINE "const m: map(i32, i32) = [1: 10, 2: 20, 3: 30]"
    LINE "const v = m.map((k, v) -> { k + v })"
    LINE "e { Result: {v.count() * 1000 + v.sum()} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t result = ecs_lookup(world, "Result");
    test_assert(e != 0);
    test_assert(result != 0);
    const void *ptr = ecs_get_id(world, e, result);
    test_assert(ptr != NULL);
    char *str = ecs_ptr_to_expr(world, result, ptr);
    test_str(str, "{v: 3066}");
    ecs_os_free(str);

    ecs_fini(world);
}

void Lambda_map_empty_map(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Result(v: vector(i32))"
    LINE "const m: map(i32, i32) = []"
    LINE "e { Result: {m.map((v) -> { v * 2 })} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t result = ecs_lookup(world, "Result");
    test_assert(e != 0);
    test_assert(result != 0);
    const void *ptr = ecs_get_id(world, e, result);
    test_assert(ptr != NULL);
    char *str = ecs_ptr_to_expr(world, result, ptr);
    test_str(str, "{v: []}");
    ecs_os_free(str);

    ecs_fini(world);
}

void Lambda_map_values(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Result(v: map(i32, i32))"
    LINE "const m: map(i32, i32) = [1: 10, 2: 20, 3: 30]"
    LINE "e { Result: {m.map_values((v) -> { v * 2 })} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t result = ecs_lookup(world, "Result");
    test_assert(e != 0);
    test_assert(result != 0);
    const ecs_map_t *map = ecs_get_id(world, e, result);
    test_assert(map != NULL);
    test_int(ecs_map_count(map), 3);
    {
        const ecs_map_val_t *v = ecs_map_get(map, 1);
        test_assert(v != NULL);
        test_int(*(const int32_t*)v, 20);
    }
    {
        const ecs_map_val_t *v = ecs_map_get(map, 2);
        test_assert(v != NULL);
        test_int(*(const int32_t*)v, 40);
    }
    {
        const ecs_map_val_t *v = ecs_map_get(map, 3);
        test_assert(v != NULL);
        test_int(*(const int32_t*)v, 60);
    }

    ecs_fini(world);
}

void Lambda_map_values_w_key(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Result(v: map(i32, i32))"
    LINE "const m: map(i32, i32) = [1: 10, 2: 20, 3: 30]"
    LINE "e { Result: {m.map_values((k, v) -> { k * 100 + v })} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t result = ecs_lookup(world, "Result");
    test_assert(e != 0);
    test_assert(result != 0);
    const ecs_map_t *map = ecs_get_id(world, e, result);
    test_assert(map != NULL);
    test_int(ecs_map_count(map), 3);
    {
        const ecs_map_val_t *v = ecs_map_get(map, 1);
        test_assert(v != NULL);
        test_int(*(const int32_t*)v, 110);
    }
    {
        const ecs_map_val_t *v = ecs_map_get(map, 2);
        test_assert(v != NULL);
        test_int(*(const int32_t*)v, 220);
    }
    {
        const ecs_map_val_t *v = ecs_map_get(map, 3);
        test_assert(v != NULL);
        test_int(*(const int32_t*)v, 330);
    }

    ecs_fini(world);
}

void Lambda_map_values_empty(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Result(v: map(i32, i32))"
    LINE "const m: map(i32, i32) = []"
    LINE "e { Result: {m.map_values((v) -> { v * 2 })} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t result = ecs_lookup(world, "Result");
    test_assert(e != 0);
    test_assert(result != 0);
    const ecs_map_t *map = ecs_get_id(world, e, result);
    test_assert(map != NULL);
    test_int(ecs_map_count(map), 0);


    ecs_fini(world);
}

void Lambda_filter_vector(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Result(v: vector(i64))"
    LINE "const a = [1, 2, 3]"
    LINE "const b = a.filter((el) -> {"
    LINE "    el != 2"
    LINE "})"
    LINE "e { Result: {b} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t result = ecs_lookup(world, "Result");
    test_assert(e != 0);
    test_assert(result != 0);
    const void *ptr = ecs_get_id(world, e, result);
    test_assert(ptr != NULL);
    char *str = ecs_ptr_to_expr(world, result, ptr);
    test_str(str, "{v: [1, 3]}");
    ecs_os_free(str);

    ecs_fini(world);
}

void Lambda_filter_vector_w_index(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Result(v: vector(i64))"
    LINE "const a = [10, 20, 30, 40]"
    LINE "e { Result: {a.filter((i, el) -> { i % 2 == 0 })} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t result = ecs_lookup(world, "Result");
    test_assert(e != 0);
    test_assert(result != 0);
    const void *ptr = ecs_get_id(world, e, result);
    test_assert(ptr != NULL);
    char *str = ecs_ptr_to_expr(world, result, ptr);
    test_str(str, "{v: [10, 30]}");
    ecs_os_free(str);

    ecs_fini(world);
}

void Lambda_filter_vector_none(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Result(v: vector(i64))"
    LINE "const a = [1, 2, 3]"
    LINE "e { Result: {a.filter((el) -> { el > 10 })} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t result = ecs_lookup(world, "Result");
    test_assert(e != 0);
    test_assert(result != 0);
    const void *ptr = ecs_get_id(world, e, result);
    test_assert(ptr != NULL);
    char *str = ecs_ptr_to_expr(world, result, ptr);
    test_str(str, "{v: []}");
    ecs_os_free(str);

    ecs_fini(world);
}

void Lambda_filter_vector_all(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Result(v: vector(i64))"
    LINE "const a = [1, 2, 3]"
    LINE "e { Result: {a.filter((el) -> { el > 0 })} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t result = ecs_lookup(world, "Result");
    test_assert(e != 0);
    test_assert(result != 0);
    const void *ptr = ecs_get_id(world, e, result);
    test_assert(ptr != NULL);
    char *str = ecs_ptr_to_expr(world, result, ptr);
    test_str(str, "{v: [1, 2, 3]}");
    ecs_os_free(str);

    ecs_fini(world);
}

void Lambda_filter_string_vector(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Result(v: vector(string))"
    LINE "const a = [\"foo\", \"bar\", \"baz\"]"
    LINE "e { Result: {a.filter((el) -> { el != \"bar\" })} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t result = ecs_lookup(world, "Result");
    test_assert(e != 0);
    test_assert(result != 0);
    const void *ptr = ecs_get_id(world, e, result);
    test_assert(ptr != NULL);
    char *str = ecs_ptr_to_expr(world, result, ptr);
    test_str(str, "{v: [\"foo\", \"baz\"]}");
    ecs_os_free(str);

    ecs_fini(world);
}

void Lambda_filter_array(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Result(v: vector(i64))"
    LINE "const a: array(i64, 4) = [1, 2, 3, 4]"
    LINE "e { Result: {a.filter((el) -> { el > 2 })} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t result = ecs_lookup(world, "Result");
    test_assert(e != 0);
    test_assert(result != 0);
    const void *ptr = ecs_get_id(world, e, result);
    test_assert(ptr != NULL);
    char *str = ecs_ptr_to_expr(world, result, ptr);
    test_str(str, "{v: [3, 4]}");
    ecs_os_free(str);

    ecs_fini(world);
}

void Lambda_filter_empty_vector(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Result(v: vector(i64))"
    LINE "const a: vector(i64) = []"
    LINE "e { Result: {a.filter((el) -> { el > 0 })} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t result = ecs_lookup(world, "Result");
    test_assert(e != 0);
    test_assert(result != 0);
    const void *ptr = ecs_get_id(world, e, result);
    test_assert(ptr != NULL);
    char *str = ecs_ptr_to_expr(world, result, ptr);
    test_str(str, "{v: []}");
    ecs_os_free(str);

    ecs_fini(world);
}

void Lambda_filter_map(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Result(v: map(i32, i32))"
    LINE "const m: map(i32, i32) = [1: 10, 2: 20, 3: 30]"
    LINE "e { Result: {m.filter((v) -> { v != 20 })} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t result = ecs_lookup(world, "Result");
    test_assert(e != 0);
    test_assert(result != 0);
    const ecs_map_t *map = ecs_get_id(world, e, result);
    test_assert(map != NULL);
    test_int(ecs_map_count(map), 2);
    {
        const ecs_map_val_t *v = ecs_map_get(map, 1);
        test_assert(v != NULL);
        test_int(*(const int32_t*)v, 10);
    }
    {
        const ecs_map_val_t *v = ecs_map_get(map, 3);
        test_assert(v != NULL);
        test_int(*(const int32_t*)v, 30);
    }

    ecs_fini(world);
}

void Lambda_filter_map_w_key(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Result(v: map(i32, i32))"
    LINE "const m: map(i32, i32) = [1: 10, 2: 20, 3: 30]"
    LINE "e { Result: {m.filter((k, v) -> { k > 1 })} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t result = ecs_lookup(world, "Result");
    test_assert(e != 0);
    test_assert(result != 0);
    const ecs_map_t *map = ecs_get_id(world, e, result);
    test_assert(map != NULL);
    test_int(ecs_map_count(map), 2);
    {
        const ecs_map_val_t *v = ecs_map_get(map, 2);
        test_assert(v != NULL);
        test_int(*(const int32_t*)v, 20);
    }
    {
        const ecs_map_val_t *v = ecs_map_get(map, 3);
        test_assert(v != NULL);
        test_int(*(const int32_t*)v, 30);
    }

    ecs_fini(world);
}

void Lambda_reduce_vector(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Result(v: i64)"
    LINE "const a = [1, 2, 3]"
    LINE "const c = a.reduce((r, el) -> {"
    LINE "    r + el"
    LINE "})"
    LINE "e { Result: {c} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t result = ecs_lookup(world, "Result");
    test_assert(e != 0);
    test_assert(result != 0);
    const void *ptr = ecs_get_id(world, e, result);
    test_assert(ptr != NULL);
    char *str = ecs_ptr_to_expr(world, result, ptr);
    test_str(str, "{v: 6}");
    ecs_os_free(str);

    ecs_fini(world);
}

void Lambda_reduce_vector_w_init(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Result(v: i64)"
    LINE "const a = [1, 2, 3]"
    LINE "e { Result: {a.reduce((r, el) -> { r + el }, 10)} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t result = ecs_lookup(world, "Result");
    test_assert(e != 0);
    test_assert(result != 0);
    const void *ptr = ecs_get_id(world, e, result);
    test_assert(ptr != NULL);
    char *str = ecs_ptr_to_expr(world, result, ptr);
    test_str(str, "{v: 16}");
    ecs_os_free(str);

    ecs_fini(world);
}

void Lambda_reduce_vector_w_index(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Result(v: i64)"
    LINE "const a = [1, 2, 3]"
    LINE "e { Result: {a.reduce((r, i, el) -> { r + i * el })} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t result = ecs_lookup(world, "Result");
    test_assert(e != 0);
    test_assert(result != 0);
    const void *ptr = ecs_get_id(world, e, result);
    test_assert(ptr != NULL);
    char *str = ecs_ptr_to_expr(world, result, ptr);
    test_str(str, "{v: 8}");
    ecs_os_free(str);

    ecs_fini(world);
}

void Lambda_reduce_empty_vector(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Result(v: i64)"
    LINE "const a: vector(i64) = []"
    LINE "e { Result: {a.reduce((r, el) -> { r + el })} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t result = ecs_lookup(world, "Result");
    test_assert(e != 0);
    test_assert(result != 0);
    const void *ptr = ecs_get_id(world, e, result);
    test_assert(ptr != NULL);
    char *str = ecs_ptr_to_expr(world, result, ptr);
    test_str(str, "{v: 0}");
    ecs_os_free(str);

    ecs_fini(world);
}

void Lambda_reduce_empty_vector_w_init(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Result(v: i64)"
    LINE "const a: vector(i64) = []"
    LINE "e { Result: {a.reduce((r, el) -> { r + el }, 42)} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t result = ecs_lookup(world, "Result");
    test_assert(e != 0);
    test_assert(result != 0);
    const void *ptr = ecs_get_id(world, e, result);
    test_assert(ptr != NULL);
    char *str = ecs_ptr_to_expr(world, result, ptr);
    test_str(str, "{v: 42}");
    ecs_os_free(str);

    ecs_fini(world);
}

void Lambda_reduce_init_var(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Result(v: i64)"
    LINE "const a = [1, 2, 3]"
    LINE "const start = 100"
    LINE "e { Result: {a.reduce((r, el) -> { r + el }, start)} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t result = ecs_lookup(world, "Result");
    test_assert(e != 0);
    test_assert(result != 0);
    const void *ptr = ecs_get_id(world, e, result);
    test_assert(ptr != NULL);
    char *str = ecs_ptr_to_expr(world, result, ptr);
    test_str(str, "{v: 106}");
    ecs_os_free(str);

    ecs_fini(world);
}

void Lambda_reduce_init_other_type(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Result(v: i64)"
    LINE "const a = [\"a\", \"bb\", \"ccc\"]"
    LINE "e { Result: {a.reduce((r, el) -> { r + 1 }, 0)} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t result = ecs_lookup(world, "Result");
    test_assert(e != 0);
    test_assert(result != 0);
    const void *ptr = ecs_get_id(world, e, result);
    test_assert(ptr != NULL);
    char *str = ecs_ptr_to_expr(world, result, ptr);
    test_str(str, "{v: 3}");
    ecs_os_free(str);

    ecs_fini(world);
}

void Lambda_reduce_to_string(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Result(v: string)"
    LINE "const a = [1, 2, 3]"
    LINE "e { Result: {a.reduce((r, el) -> { \"{r}{el}\" }, \"\")} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t result = ecs_lookup(world, "Result");
    test_assert(e != 0);
    test_assert(result != 0);
    const void *ptr = ecs_get_id(world, e, result);
    test_assert(ptr != NULL);
    char *str = ecs_ptr_to_expr(world, result, ptr);
    test_str(str, "{v: \"123\"}");
    ecs_os_free(str);

    ecs_fini(world);
}

void Lambda_reduce_string_vector(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Result(v: string)"
    LINE "const a = [\"a\", \"b\", \"c\"]"
    LINE "e { Result: {a.reduce((r, el) -> { \"{r}{el}\" })} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t result = ecs_lookup(world, "Result");
    test_assert(e != 0);
    test_assert(result != 0);
    const void *ptr = ecs_get_id(world, e, result);
    test_assert(ptr != NULL);
    char *str = ecs_ptr_to_expr(world, result, ptr);
    test_str(str, "{v: \"abc\"}");
    ecs_os_free(str);

    ecs_fini(world);
}

void Lambda_reduce_array(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Result(v: i64)"
    LINE "const a: array(i64, 3) = [1, 2, 3]"
    LINE "e { Result: {a.reduce((r, el) -> { r * 10 + el })} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t result = ecs_lookup(world, "Result");
    test_assert(e != 0);
    test_assert(result != 0);
    const void *ptr = ecs_get_id(world, e, result);
    test_assert(ptr != NULL);
    char *str = ecs_ptr_to_expr(world, result, ptr);
    test_str(str, "{v: 123}");
    ecs_os_free(str);

    ecs_fini(world);
}

void Lambda_reduce_struct(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Position(x: i64, y: i64)"
    LINE "struct Result(v: Position)"
    LINE "struct Position(x: i64, y: i64)"
    LINE "const a: vector(Position) = [{1, 2}, {3, 4}, {5, 6}]"
    LINE "e { Result: {a.reduce((r, p) -> { {r.x + p.x, r.y + p.y} })} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t result = ecs_lookup(world, "Result");
    test_assert(e != 0);
    test_assert(result != 0);
    const void *ptr = ecs_get_id(world, e, result);
    test_assert(ptr != NULL);
    char *str = ecs_ptr_to_expr(world, result, ptr);
    test_str(str, "{v: {x: 9, y: 12}}");
    ecs_os_free(str);

    ecs_fini(world);
}

void Lambda_reduce_map(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Result(v: i32)"
    LINE "const m: map(i32, i32) = [1: 10, 2: 20, 3: 30]"
    LINE "e { Result: {m.reduce((r, v) -> { r + v })} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t result = ecs_lookup(world, "Result");
    test_assert(e != 0);
    test_assert(result != 0);
    const void *ptr = ecs_get_id(world, e, result);
    test_assert(ptr != NULL);
    char *str = ecs_ptr_to_expr(world, result, ptr);
    test_str(str, "{v: 60}");
    ecs_os_free(str);

    ecs_fini(world);
}

void Lambda_reduce_map_w_key(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Result(v: i32)"
    LINE "const m: map(i32, i32) = [1: 10, 2: 20, 3: 30]"
    LINE "e { Result: {m.reduce((r, k, v) -> { r + k * v })} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t result = ecs_lookup(world, "Result");
    test_assert(e != 0);
    test_assert(result != 0);
    const void *ptr = ecs_get_id(world, e, result);
    test_assert(ptr != NULL);
    char *str = ecs_ptr_to_expr(world, result, ptr);
    test_str(str, "{v: 140}");
    ecs_os_free(str);

    ecs_fini(world);
}

void Lambda_reduce_empty_map_w_init(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Result(v: i32)"
    LINE "const m: map(i32, i32) = []"
    LINE "e { Result: {m.reduce((r, v) -> { r + v }, 5)} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t result = ecs_lookup(world, "Result");
    test_assert(e != 0);
    test_assert(result != 0);
    const void *ptr = ecs_get_id(world, e, result);
    test_assert(ptr != NULL);
    char *str = ecs_ptr_to_expr(world, result, ptr);
    test_str(str, "{v: 5}");
    ecs_os_free(str);

    ecs_fini(world);
}

void Lambda_capture_var(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Result(v: vector(i64))"
    LINE "const a = [1, 2, 3]"
    LINE "const scale = 10"
    LINE "e { Result: {a.map((el) -> { el * scale })} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t result = ecs_lookup(world, "Result");
    test_assert(e != 0);
    test_assert(result != 0);
    const void *ptr = ecs_get_id(world, e, result);
    test_assert(ptr != NULL);
    char *str = ecs_ptr_to_expr(world, result, ptr);
    test_str(str, "{v: [10, 20, 30]}");
    ecs_os_free(str);

    ecs_fini(world);
}

void Lambda_shadow_var(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Result(v: vector(i64))"
    LINE "const a = [1, 2, 3]"
    LINE "const el = 100"
    LINE "e { Result: {a.map((el) -> { el + 1 })} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t result = ecs_lookup(world, "Result");
    test_assert(e != 0);
    test_assert(result != 0);
    const void *ptr = ecs_get_id(world, e, result);
    test_assert(ptr != NULL);
    char *str = ecs_ptr_to_expr(world, result, ptr);
    test_str(str, "{v: [2, 3, 4]}");
    ecs_os_free(str);

    ecs_fini(world);
}

void Lambda_nested(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Result(v: vector(i64))"
    LINE "const a = [1, 2, 3]"
    LINE "e { Result: {a.map((el) -> { a.reduce((r, x) -> { r + x * el }) })} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t result = ecs_lookup(world, "Result");
    test_assert(e != 0);
    test_assert(result != 0);
    const void *ptr = ecs_get_id(world, e, result);
    test_assert(ptr != NULL);
    char *str = ecs_ptr_to_expr(world, result, ptr);
    test_str(str, "{v: [6, 12, 18]}");
    ecs_os_free(str);

    ecs_fini(world);
}

void Lambda_nested_capture_outer_param(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Result(v: vector(i64))"
    LINE "const a = [1, 2]"
    LINE "const b = [10, 20]"
    LINE "e { Result: {a.map((x) -> { b.map((y) -> { x + y }).reduce((r, z) -> { r + z }) })} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t result = ecs_lookup(world, "Result");
    test_assert(e != 0);
    test_assert(result != 0);
    const void *ptr = ecs_get_id(world, e, result);
    test_assert(ptr != NULL);
    char *str = ecs_ptr_to_expr(world, result, ptr);
    test_str(str, "{v: [32, 34]}");
    ecs_os_free(str);

    ecs_fini(world);
}

void Lambda_chained(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Result(v: vector(i64))"
    LINE "const a = [1, 2, 3, 4]"
    LINE "e { Result: {a.map((el) -> { el * 2 }).filter((el) -> { el > 4 })} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t result = ecs_lookup(world, "Result");
    test_assert(e != 0);
    test_assert(result != 0);
    const void *ptr = ecs_get_id(world, e, result);
    test_assert(ptr != NULL);
    char *str = ecs_ptr_to_expr(world, result, ptr);
    test_str(str, "{v: [6, 8]}");
    ecs_os_free(str);

    ecs_fini(world);
}

void Lambda_chained_reduce(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Result(v: i64)"
    LINE "const a = [1, 2, 3, 4]"
    LINE "const r = a"
    LINE "  .map((el) -> { el * 2 })"
    LINE "  .filter((el) -> { el > 4 })"
    LINE "  .reduce((r, el) -> { r + el })"
    LINE "e { Result: {r} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t result = ecs_lookup(world, "Result");
    test_assert(e != 0);
    test_assert(result != 0);
    const void *ptr = ecs_get_id(world, e, result);
    test_assert(ptr != NULL);
    char *str = ecs_ptr_to_expr(world, result, ptr);
    test_str(str, "{v: 14}");
    ecs_os_free(str);

    ecs_fini(world);
}

void Lambda_in_for(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Result(v: vector(i64))"
    LINE "const a = [1, 2, 3]"
    LINE "for i in [0..2] {"
    LINE "  \"e_{i}\" {"
    LINE "    Result: {a.map((el) -> { el + i })}"
    LINE "  }"
    LINE "}"
    LINE "e { Result: {a.map((el) -> { el })} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t result = ecs_lookup(world, "Result");
    test_assert(e != 0);
    test_assert(result != 0);
    const void *ptr = ecs_get_id(world, e, result);
    test_assert(ptr != NULL);
    char *str = ecs_ptr_to_expr(world, result, ptr);
    test_str(str, "{v: [1, 2, 3]}");
    ecs_os_free(str);

    ecs_fini(world);
}

void Lambda_in_fn(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Result(v: i64)"
    LINE "fn scale(n: i64) -> i64 {"
    LINE "  [1, 2, 3].reduce((r, el) -> { r + el * n })"
    LINE "}"
    LINE "e { Result: {scale(10)} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t result = ecs_lookup(world, "Result");
    test_assert(e != 0);
    test_assert(result != 0);
    const void *ptr = ecs_get_id(world, e, result);
    test_assert(ptr != NULL);
    char *str = ecs_ptr_to_expr(world, result, ptr);
    test_str(str, "{v: 60}");
    ecs_os_free(str);

    ecs_fini(world);
}

void Lambda_in_template(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Result(v: vector(i64))"
    LINE "template Tmpl {"
    LINE "  prop n: i64 = 0"
    LINE "  Result: {[1, 2, 3].map((el) -> { el + n })}"
    LINE "}"
    LINE "e { Tmpl: {n: 100} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t result = ecs_lookup(world, "Result");
    test_assert(e != 0);
    test_assert(result != 0);
    const void *ptr = ecs_get_id(world, e, result);
    test_assert(ptr != NULL);
    char *str = ecs_ptr_to_expr(world, result, ptr);
    test_str(str, "{v: [101, 102, 103]}");
    ecs_os_free(str);

    ecs_fini(world);
}

void Lambda_in_match(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Result(v: vector(i64))"
    LINE "const a = [1, 2, 3]"
    LINE "const kind = 1"
    LINE "e { Result: {match kind { 1: a.map((el) -> { el * 2 }); 2: a }} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t result = ecs_lookup(world, "Result");
    test_assert(e != 0);
    test_assert(result != 0);
    const void *ptr = ecs_get_id(world, e, result);
    test_assert(ptr != NULL);
    char *str = ecs_ptr_to_expr(world, result, ptr);
    test_str(str, "{v: [2, 4, 6]}");
    ecs_os_free(str);

    ecs_fini(world);
}

void Lambda_in_if(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Result(v: vector(i64))"
    LINE "const a = [1, 2, 3]"
    LINE "if a.reduce((r, el) -> { r + el }) == 6 {"
    LINE "  e { Result: {a} }"
    LINE "}";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t result = ecs_lookup(world, "Result");
    test_assert(e != 0);
    test_assert(result != 0);
    const void *ptr = ecs_get_id(world, e, result);
    test_assert(ptr != NULL);
    char *str = ecs_ptr_to_expr(world, result, ptr);
    test_str(str, "{v: [1, 2, 3]}");
    ecs_os_free(str);

    ecs_fini(world);
}

void Lambda_sum_vector(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Result(v: i64)"
    LINE "const a = [1, 2, 3]"
    LINE "e { Result: {a.sum()} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t result = ecs_lookup(world, "Result");
    test_assert(e != 0);
    test_assert(result != 0);
    const void *ptr = ecs_get_id(world, e, result);
    test_assert(ptr != NULL);
    char *str = ecs_ptr_to_expr(world, result, ptr);
    test_str(str, "{v: 6}");
    ecs_os_free(str);

    ecs_fini(world);
}

void Lambda_sum_w_lambda(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Result(v: i64)"
    LINE "const a = [1, 2, 3]"
    LINE "e { Result: {a.sum((el) -> { el * 10 })} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t result = ecs_lookup(world, "Result");
    test_assert(e != 0);
    test_assert(result != 0);
    const void *ptr = ecs_get_id(world, e, result);
    test_assert(ptr != NULL);
    char *str = ecs_ptr_to_expr(world, result, ptr);
    test_str(str, "{v: 60}");
    ecs_os_free(str);

    ecs_fini(world);
}

void Lambda_sum_w_index(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Result(v: i64)"
    LINE "const a = [1, 2, 3]"
    LINE "e { Result: {a.sum((i, el) -> { i * el })} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t result = ecs_lookup(world, "Result");
    test_assert(e != 0);
    test_assert(result != 0);
    const void *ptr = ecs_get_id(world, e, result);
    test_assert(ptr != NULL);
    char *str = ecs_ptr_to_expr(world, result, ptr);
    test_str(str, "{v: 8}");
    ecs_os_free(str);

    ecs_fini(world);
}

void Lambda_sum_empty(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Result(v: i64)"
    LINE "const a: vector(i64) = []"
    LINE "e { Result: {a.sum()} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t result = ecs_lookup(world, "Result");
    test_assert(e != 0);
    test_assert(result != 0);
    const void *ptr = ecs_get_id(world, e, result);
    test_assert(ptr != NULL);
    char *str = ecs_ptr_to_expr(world, result, ptr);
    test_str(str, "{v: 0}");
    ecs_os_free(str);

    ecs_fini(world);
}

void Lambda_sum_array(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Result(v: i64)"
    LINE "const a: array(i64, 3) = [1, 2, 3]"
    LINE "e { Result: {a.sum()} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t result = ecs_lookup(world, "Result");
    test_assert(e != 0);
    test_assert(result != 0);
    const void *ptr = ecs_get_id(world, e, result);
    test_assert(ptr != NULL);
    char *str = ecs_ptr_to_expr(world, result, ptr);
    test_str(str, "{v: 6}");
    ecs_os_free(str);

    ecs_fini(world);
}

void Lambda_sum_float(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Result(v: f64)"
    LINE "const a = [1.5, 2.5]"
    LINE "e { Result: {a.sum()} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t result = ecs_lookup(world, "Result");
    test_assert(e != 0);
    test_assert(result != 0);
    const void *ptr = ecs_get_id(world, e, result);
    test_assert(ptr != NULL);
    char *str = ecs_ptr_to_expr(world, result, ptr);
    test_str(str, "{v: 4}");
    ecs_os_free(str);

    ecs_fini(world);
}

void Lambda_sum_map(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Result(v: i32)"
    LINE "const m: map(i32, i32) = [1: 10, 2: 20]"
    LINE "e { Result: {m.sum()} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t result = ecs_lookup(world, "Result");
    test_assert(e != 0);
    test_assert(result != 0);
    const void *ptr = ecs_get_id(world, e, result);
    test_assert(ptr != NULL);
    char *str = ecs_ptr_to_expr(world, result, ptr);
    test_str(str, "{v: 30}");
    ecs_os_free(str);

    ecs_fini(world);
}

void Lambda_sum_map_w_key(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Result(v: i32)"
    LINE "const m: map(i32, i32) = [1: 10, 2: 20]"
    LINE "e { Result: {m.sum((k, v) -> { k })} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t result = ecs_lookup(world, "Result");
    test_assert(e != 0);
    test_assert(result != 0);
    const void *ptr = ecs_get_id(world, e, result);
    test_assert(ptr != NULL);
    char *str = ecs_ptr_to_expr(world, result, ptr);
    test_str(str, "{v: 3}");
    ecs_os_free(str);

    ecs_fini(world);
}

void Lambda_sum_struct_member(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Result(v: i64)"
    LINE "struct Item(price: i64)"
    LINE "const a: vector(Item) = [{10}, {20}]"
    LINE "e { Result: {a.sum((it) -> { it.price })} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t result = ecs_lookup(world, "Result");
    test_assert(e != 0);
    test_assert(result != 0);
    const void *ptr = ecs_get_id(world, e, result);
    test_assert(ptr != NULL);
    char *str = ecs_ptr_to_expr(world, result, ptr);
    test_str(str, "{v: 30}");
    ecs_os_free(str);

    ecs_fini(world);
}

void Lambda_min_vector(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Result(v: i64)"
    LINE "const a = [3, 1, 2]"
    LINE "e { Result: {a.min()} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t result = ecs_lookup(world, "Result");
    test_assert(e != 0);
    test_assert(result != 0);
    const void *ptr = ecs_get_id(world, e, result);
    test_assert(ptr != NULL);
    char *str = ecs_ptr_to_expr(world, result, ptr);
    test_str(str, "{v: 1}");
    ecs_os_free(str);

    ecs_fini(world);
}

void Lambda_max_vector(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Result(v: i64)"
    LINE "const a = [3, 1, 2]"
    LINE "e { Result: {a.max()} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t result = ecs_lookup(world, "Result");
    test_assert(e != 0);
    test_assert(result != 0);
    const void *ptr = ecs_get_id(world, e, result);
    test_assert(ptr != NULL);
    char *str = ecs_ptr_to_expr(world, result, ptr);
    test_str(str, "{v: 3}");
    ecs_os_free(str);

    ecs_fini(world);
}

void Lambda_min_w_lambda(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Result(v: i64)"
    LINE "const a = [3, 1, 2]"
    LINE "e { Result: {a.min((el) -> { el * -1 })} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t result = ecs_lookup(world, "Result");
    test_assert(e != 0);
    test_assert(result != 0);
    const void *ptr = ecs_get_id(world, e, result);
    test_assert(ptr != NULL);
    char *str = ecs_ptr_to_expr(world, result, ptr);
    test_str(str, "{v: -3}");
    ecs_os_free(str);

    ecs_fini(world);
}

void Lambda_max_w_lambda(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Result(v: i64)"
    LINE "const a = [3, 1, 2]"
    LINE "e { Result: {a.max((i, el) -> { el + i * 10 })} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t result = ecs_lookup(world, "Result");
    test_assert(e != 0);
    test_assert(result != 0);
    const void *ptr = ecs_get_id(world, e, result);
    test_assert(ptr != NULL);
    char *str = ecs_ptr_to_expr(world, result, ptr);
    test_str(str, "{v: 22}");
    ecs_os_free(str);

    ecs_fini(world);
}

void Lambda_min_single(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Result(v: i64)"
    LINE "const a = [7]"
    LINE "e { Result: {a.min()} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t result = ecs_lookup(world, "Result");
    test_assert(e != 0);
    test_assert(result != 0);
    const void *ptr = ecs_get_id(world, e, result);
    test_assert(ptr != NULL);
    char *str = ecs_ptr_to_expr(world, result, ptr);
    test_str(str, "{v: 7}");
    ecs_os_free(str);

    ecs_fini(world);
}

void Lambda_max_float(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Result(v: f64)"
    LINE "const a = [1.5, -2.5, 0.5]"
    LINE "e { Result: {a.max()} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t result = ecs_lookup(world, "Result");
    test_assert(e != 0);
    test_assert(result != 0);
    const void *ptr = ecs_get_id(world, e, result);
    test_assert(ptr != NULL);
    char *str = ecs_ptr_to_expr(world, result, ptr);
    test_str(str, "{v: 1.5}");
    ecs_os_free(str);

    ecs_fini(world);
}

void Lambda_min_map(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Result(v: i32)"
    LINE "const m: map(i32, i32) = [1: 10, 2: 5, 3: 30]"
    LINE "e { Result: {m.min()} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t result = ecs_lookup(world, "Result");
    test_assert(e != 0);
    test_assert(result != 0);
    const void *ptr = ecs_get_id(world, e, result);
    test_assert(ptr != NULL);
    char *str = ecs_ptr_to_expr(world, result, ptr);
    test_str(str, "{v: 5}");
    ecs_os_free(str);

    ecs_fini(world);
}

void Lambda_max_map_w_key(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Result(v: i32)"
    LINE "const m: map(i32, i32) = [1: 10, 2: 5, 3: 30]"
    LINE "e { Result: {m.max((k, v) -> { k })} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t result = ecs_lookup(world, "Result");
    test_assert(e != 0);
    test_assert(result != 0);
    const void *ptr = ecs_get_id(world, e, result);
    test_assert(ptr != NULL);
    char *str = ecs_ptr_to_expr(world, result, ptr);
    test_str(str, "{v: 3}");
    ecs_os_free(str);

    ecs_fini(world);
}

void Lambda_any_w_lambda(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Result(v: bool)"
    LINE "const a = [1, 2, 3]"
    LINE "e { Result: {a.any((el) -> { el == 2 })} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t result = ecs_lookup(world, "Result");
    test_assert(e != 0);
    test_assert(result != 0);
    const void *ptr = ecs_get_id(world, e, result);
    test_assert(ptr != NULL);
    char *str = ecs_ptr_to_expr(world, result, ptr);
    test_str(str, "{v: true}");
    ecs_os_free(str);

    ecs_fini(world);
}

void Lambda_any_w_lambda_false(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Result(v: bool)"
    LINE "const a = [1, 2, 3]"
    LINE "e { Result: {a.any((el) -> { el > 3 })} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t result = ecs_lookup(world, "Result");
    test_assert(e != 0);
    test_assert(result != 0);
    const void *ptr = ecs_get_id(world, e, result);
    test_assert(ptr != NULL);
    char *str = ecs_ptr_to_expr(world, result, ptr);
    test_str(str, "{v: false}");
    ecs_os_free(str);

    ecs_fini(world);
}

void Lambda_any_bools(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Result(v: bool)"
    LINE "const a = [false, true]"
    LINE "e { Result: {a.any()} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t result = ecs_lookup(world, "Result");
    test_assert(e != 0);
    test_assert(result != 0);
    const void *ptr = ecs_get_id(world, e, result);
    test_assert(ptr != NULL);
    char *str = ecs_ptr_to_expr(world, result, ptr);
    test_str(str, "{v: true}");
    ecs_os_free(str);

    ecs_fini(world);
}

void Lambda_any_empty(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Result(v: bool)"
    LINE "const a: vector(i64) = []"
    LINE "e { Result: {a.any((el) -> { el > 0 })} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t result = ecs_lookup(world, "Result");
    test_assert(e != 0);
    test_assert(result != 0);
    const void *ptr = ecs_get_id(world, e, result);
    test_assert(ptr != NULL);
    char *str = ecs_ptr_to_expr(world, result, ptr);
    test_str(str, "{v: false}");
    ecs_os_free(str);

    ecs_fini(world);
}

void Lambda_any_map_w_key(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Result(v: bool)"
    LINE "const m: map(i32, i32) = [1: 10, 2: 20]"
    LINE "e { Result: {m.any((k, v) -> { k == 2 })} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t result = ecs_lookup(world, "Result");
    test_assert(e != 0);
    test_assert(result != 0);
    const void *ptr = ecs_get_id(world, e, result);
    test_assert(ptr != NULL);
    char *str = ecs_ptr_to_expr(world, result, ptr);
    test_str(str, "{v: true}");
    ecs_os_free(str);

    ecs_fini(world);
}

void Lambda_all_w_lambda(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Result(v: bool)"
    LINE "const a = [1, 2, 3]"
    LINE "e { Result: {a.all((el) -> { el > 0 })} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t result = ecs_lookup(world, "Result");
    test_assert(e != 0);
    test_assert(result != 0);
    const void *ptr = ecs_get_id(world, e, result);
    test_assert(ptr != NULL);
    char *str = ecs_ptr_to_expr(world, result, ptr);
    test_str(str, "{v: true}");
    ecs_os_free(str);

    ecs_fini(world);
}

void Lambda_all_w_lambda_false(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Result(v: bool)"
    LINE "const a = [1, 2, 3]"
    LINE "e { Result: {a.all((el) -> { el > 1 })} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t result = ecs_lookup(world, "Result");
    test_assert(e != 0);
    test_assert(result != 0);
    const void *ptr = ecs_get_id(world, e, result);
    test_assert(ptr != NULL);
    char *str = ecs_ptr_to_expr(world, result, ptr);
    test_str(str, "{v: false}");
    ecs_os_free(str);

    ecs_fini(world);
}

void Lambda_all_bools(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Result(v: bool)"
    LINE "const a = [true, false]"
    LINE "e { Result: {a.all()} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t result = ecs_lookup(world, "Result");
    test_assert(e != 0);
    test_assert(result != 0);
    const void *ptr = ecs_get_id(world, e, result);
    test_assert(ptr != NULL);
    char *str = ecs_ptr_to_expr(world, result, ptr);
    test_str(str, "{v: false}");
    ecs_os_free(str);

    ecs_fini(world);
}

void Lambda_all_empty(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Result(v: bool)"
    LINE "const a: vector(i64) = []"
    LINE "e { Result: {a.all((el) -> { el > 0 })} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t result = ecs_lookup(world, "Result");
    test_assert(e != 0);
    test_assert(result != 0);
    const void *ptr = ecs_get_id(world, e, result);
    test_assert(ptr != NULL);
    char *str = ecs_ptr_to_expr(world, result, ptr);
    test_str(str, "{v: true}");
    ecs_os_free(str);

    ecs_fini(world);
}

void Lambda_count_w_lambda(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Result(v: i32)"
    LINE "const a = [1, 2, 1, 3]"
    LINE "e { Result: {a.count((el) -> { el == 1 })} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t result = ecs_lookup(world, "Result");
    test_assert(e != 0);
    test_assert(result != 0);
    const void *ptr = ecs_get_id(world, e, result);
    test_assert(ptr != NULL);
    char *str = ecs_ptr_to_expr(world, result, ptr);
    test_str(str, "{v: 2}");
    ecs_os_free(str);

    ecs_fini(world);
}

void Lambda_count_w_index(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Result(v: i32)"
    LINE "const a = [1, 2, 1, 3]"
    LINE "e { Result: {a.count((i, el) -> { i > 0 })} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t result = ecs_lookup(world, "Result");
    test_assert(e != 0);
    test_assert(result != 0);
    const void *ptr = ecs_get_id(world, e, result);
    test_assert(ptr != NULL);
    char *str = ecs_ptr_to_expr(world, result, ptr);
    test_str(str, "{v: 3}");
    ecs_os_free(str);

    ecs_fini(world);
}

void Lambda_count_map(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Result(v: i32)"
    LINE "const m: map(i32, i32) = [1: 10, 2: 20, 3: 30]"
    LINE "e { Result: {m.count((v) -> { v > 10 })} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t result = ecs_lookup(world, "Result");
    test_assert(e != 0);
    test_assert(result != 0);
    const void *ptr = ecs_get_id(world, e, result);
    test_assert(ptr != NULL);
    char *str = ecs_ptr_to_expr(world, result, ptr);
    test_str(str, "{v: 2}");
    ecs_os_free(str);

    ecs_fini(world);
}

void Lambda_count_without_lambda(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Result(v: i32)"
    LINE "const a = [1, 2, 1, 3]"
    LINE "e { Result: {a.count()} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t result = ecs_lookup(world, "Result");
    test_assert(e != 0);
    test_assert(result != 0);
    const void *ptr = ecs_get_id(world, e, result);
    test_assert(ptr != NULL);
    char *str = ecs_ptr_to_expr(world, result, ptr);
    test_str(str, "{v: 4}");
    ecs_os_free(str);

    ecs_fini(world);
}

void Lambda_filter_then_sum(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Result(v: i64)"
    LINE "const a = [1, 2, 3, 4]"
    LINE "e { Result: {a.filter((el) -> { el % 2 == 0 }).sum()} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t result = ecs_lookup(world, "Result");
    test_assert(e != 0);
    test_assert(result != 0);
    const void *ptr = ecs_get_id(world, e, result);
    test_assert(ptr != NULL);
    char *str = ecs_ptr_to_expr(world, result, ptr);
    test_str(str, "{v: 6}");
    ecs_os_free(str);

    ecs_fini(world);
}

void Lambda_map_then_max(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Result(v: i64)"
    LINE "const a = [1, 2, 3]"
    LINE "e { Result: {a.map((el) -> { el * el }).max()} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t result = ecs_lookup(world, "Result");
    test_assert(e != 0);
    test_assert(result != 0);
    const void *ptr = ecs_get_id(world, e, result);
    test_assert(ptr != NULL);
    char *str = ecs_ptr_to_expr(world, result, ptr);
    test_str(str, "{v: 9}");
    ecs_os_free(str);

    ecs_fini(world);
}

void Lambda_sum_in_fn(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Result(v: i64)"
    LINE "fn total(v: i64) -> i64 {"
    LINE "  [1, 2, 3].sum((el) -> { el * v })"
    LINE "}"
    LINE "e { Result: {total(2)} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t result = ecs_lookup(world, "Result");
    test_assert(e != 0);
    test_assert(result != 0);
    const void *ptr = ecs_get_id(world, e, result);
    test_assert(ptr != NULL);
    char *str = ecs_ptr_to_expr(world, result, ptr);
    test_str(str, "{v: 12}");
    ecs_os_free(str);

    ecs_fini(world);
}

void Lambda_map_values_vector(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "const a = [1, 2, 3]"
    LINE "const b = a.map_values((el) -> { el })";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) != 0);

    ecs_fini(world);
}

void Lambda_min_empty(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "const a: vector(i64) = []"
    LINE "const b = a.min()"
    LINE "e {}";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) != 0);

    ecs_fini(world);
}

void Lambda_max_empty(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "const a: vector(i64) = []"
    LINE "const b = a.max()"
    LINE "e {}";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) != 0);

    ecs_fini(world);
}

void Lambda_sum_strings(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "const a = [\"a\", \"b\"]"
    LINE "const b = a.sum()";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) != 0);

    ecs_fini(world);
}

void Lambda_min_w_string_lambda(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "const a = [1, 2]"
    LINE "const b = a.min((el) -> { \"foo\" })";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) != 0);

    ecs_fini(world);
}

void Lambda_any_without_lambda_non_bool(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "const a = [1, 2]"
    LINE "const b = a.any()";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) != 0);

    ecs_fini(world);
}

void Lambda_sum_non_collection(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "const a = 10"
    LINE "const b = a.sum()";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) != 0);

    ecs_fini(world);
}

void Lambda_sum_too_many_args(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "const a = [1, 2]"
    LINE "const b = a.sum((el) -> { el }, 10)";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) != 0);

    ecs_fini(world);
}

void Lambda_count_too_many_args(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "const a = [1, 2]"
    LINE "const b = a.count((el) -> { el > 1 }, 10)";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) != 0);

    ecs_fini(world);
}

void Lambda_lambda_as_value(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "const a = (el) -> { el }";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) != 0);

    ecs_fini(world);
}

void Lambda_lambda_as_function_arg(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "const a = [1, 2, 3]"
    LINE "const b = foo((el) -> { el })";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) != 0);

    ecs_fini(world);
}

void Lambda_lambda_unknown_method(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "const a = [1, 2, 3]"
    LINE "const b = a.foo((el) -> { el })";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) != 0);

    ecs_fini(world);
}

void Lambda_map_non_collection(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "const a = 10"
    LINE "const b = a.map((el) -> { el })";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) != 0);

    ecs_fini(world);
}

void Lambda_map_too_many_params(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "const a = [1, 2, 3]"
    LINE "const b = a.map((i, j, el) -> { el })";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) != 0);

    ecs_fini(world);
}

void Lambda_reduce_too_few_params(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "const a = [1, 2, 3]"
    LINE "const b = a.reduce((el) -> { el })";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) != 0);

    ecs_fini(world);
}

void Lambda_reduce_too_many_params(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "const a = [1, 2, 3]"
    LINE "const b = a.reduce((r, i, j, el) -> { el })";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) != 0);

    ecs_fini(world);
}

void Lambda_duplicate_param(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "const a = [1, 2, 3]"
    LINE "const b = a.map((el, el) -> { el })";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) != 0);

    ecs_fini(world);
}

void Lambda_map_too_many_args(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "const a = [1, 2, 3]"
    LINE "const b = a.map((el) -> { el }, 10)";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) != 0);

    ecs_fini(world);
}

void Lambda_reduce_too_many_args(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "const a = [1, 2, 3]"
    LINE "const b = a.reduce((r, el) -> { el }, 10, 20)";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) != 0);

    ecs_fini(world);
}

void Lambda_reduce_init_before_lambda(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "const a = [1, 2, 3]"
    LINE "const b = a.reduce(10, (r, el) -> { el })";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) != 0);

    ecs_fini(world);
}

void Lambda_filter_non_bool(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "const a = [1, 2, 3]"
    LINE "const b = a.filter((el) -> { \"foo\" })";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) != 0);

    ecs_fini(world);
}

void Lambda_body_without_braces(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "const a = [1, 2, 3]"
    LINE "const b = a.map((el) -> el)";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) != 0);

    ecs_fini(world);
}

void Lambda_unresolved_in_body(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "const a = [1, 2, 3]"
    LINE "const b = a.map((el) -> { foo })";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) != 0);

    ecs_fini(world);
}

void Lambda_param_out_of_scope(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "const a = [1, 2, 3]"
    LINE "const b = a.map((el) -> { el })"
    LINE "const c = el";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) != 0);

    ecs_fini(world);
}

void Lambda_reduce_lambda_init(void) {
    ecs_world_t *world = ecs_init();

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL,
        HEAD "const v = [1, 2, 3]"
        LINE "const x = v.reduce((acc, el) -> { acc + el }, (el) -> { el })",
        &ir_desc, NULL) != 0);

    ecs_fini(world);
}


void Lambda_reduce_init_eval_error(void) {
    ecs_world_t *world = ecs_init();

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL,
        HEAD "const v = [1, 2, 3]"
        LINE "const x = v.reduce((acc, el) -> { acc + el }, v[10])",
        &ir_desc, NULL) != 0);

    ecs_fini(world);
}

void Lambda_invalid_collection(void) {
    ecs_world_t *world = ecs_init();

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL,
        HEAD "const x = $undefined.map((el) -> { el })",
        &ir_desc, NULL) != 0);

    ecs_fini(world);
}

void Lambda_contains_not_comparable(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t point = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Point" }),
        .members = {{ .name = "x", .type = ecs_id(ecs_f32_t) }}
    });

    ecs_type_hooks_t hooks = *ecs_get_hooks_id(world, point);
    hooks.cmp = NULL;
    hooks.equals = NULL;
    hooks.flags |= ECS_TYPE_HOOK_CMP_ILLEGAL|ECS_TYPE_HOOK_EQUALS_ILLEGAL;
    hooks.flags &= ECS_TYPE_HOOKS_ILLEGAL;
    ecs_set_hooks_id(world, point, &hooks);

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL,
        HEAD "const v: Point[] = [{1}, {2}]"
        LINE "const x = v.contains({1})",
        &ir_desc, NULL) != 0);

    ecs_fini(world);
}

void Lambda_sort_by_not_comparable(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t point = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Point" }),
        .members = {{ .name = "x", .type = ecs_id(ecs_f32_t) }}
    });

    ecs_type_hooks_t hooks = *ecs_get_hooks_id(world, point);
    hooks.cmp = NULL;
    hooks.equals = NULL;
    hooks.flags |= ECS_TYPE_HOOK_CMP_ILLEGAL|ECS_TYPE_HOOK_EQUALS_ILLEGAL;
    hooks.flags &= ECS_TYPE_HOOKS_ILLEGAL;
    ecs_set_hooks_id(world, point, &hooks);

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL,
        HEAD "const v: Point[] = [{2}, {1}]"
        LINE "const x = v.sort((el) -> { el })",
        &ir_desc, NULL) != 0);

    ecs_fini(world);
}

void Lambda_to_str(void) {
    ecs_world_t *world = ecs_init();

    ecs_script_t *script = ecs_script_parse(world, NULL,
        HEAD "const v = [1, 2, 3]"
        LINE "const a = v.map((el) -> { el * 2 })"
        LINE "const b = v.reduce((acc, i, el) -> { acc + el }, 10)",
        &ir_desc, NULL);
    test_assert(script != NULL);
    test_assert(ecs_script_eval(script, &ir_desc, NULL) == 0);

    char *str = ecs_script_ast_to_str(script, false);
    test_str(str,
    HEAD "{"
    LINE "  const: v = {1, 2, 3}"
    LINE "  const: a = $v.map((el) -> { ($el * 2) })"
    LINE "  const: b = $v.reduce((acc, i, el) -> { ($acc + $el) }, 10)"
    LINE "}"
    LINE "");
    ecs_os_free(str);

    ecs_script_free(script);
    ecs_fini(world);
}
