#include <script.h>

static bool ir_enabled = false;
static ecs_script_eval_desc_t ir_desc = {0};

void CollectionOps_setup(void) {
    const char *ir_param = test_param("ir");
    ir_enabled = ir_param && !strcmp(ir_param, "enabled");
    ir_desc = (ecs_script_eval_desc_t){ .ir = ir_enabled };
}

void CollectionOps_concat_vectors(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Result(v: vector(i64))"
    LINE "const a = [1, 2] + [3, 4]"
    LINE "e { Result: {a} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t result = ecs_lookup(world, "Result");
    test_assert(e != 0);
    test_assert(result != 0);
    const void *ptr = ecs_get_id(world, e, result);
    test_assert(ptr != NULL);
    char *str = ecs_ptr_to_expr(world, result, ptr);
    test_str(str, "{v: [1, 2, 3, 4]}");
    ecs_os_free(str);

    ecs_fini(world);
}

void CollectionOps_concat_vector_vars(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Result(v: vector(i64))"
    LINE "const a = [1, 2]"
    LINE "const b = [3]"
    LINE "const c = a + b + a"
    LINE "e { Result: {c} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t result = ecs_lookup(world, "Result");
    test_assert(e != 0);
    test_assert(result != 0);
    const void *ptr = ecs_get_id(world, e, result);
    test_assert(ptr != NULL);
    char *str = ecs_ptr_to_expr(world, result, ptr);
    test_str(str, "{v: [1, 2, 3, 1, 2]}");
    ecs_os_free(str);

    ecs_fini(world);
}

void CollectionOps_concat_typed_vector_literal(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Result(v: vector(f32))"
    LINE "const a: f32[] = [1.5, 2.5]"
    LINE "const b = a + [3]"
    LINE "e { Result: {b} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t result = ecs_lookup(world, "Result");
    test_assert(e != 0);
    test_assert(result != 0);
    const void *ptr = ecs_get_id(world, e, result);
    test_assert(ptr != NULL);
    char *str = ecs_ptr_to_expr(world, result, ptr);
    test_str(str, "{v: [1.5, 2.5, 3]}");
    ecs_os_free(str);

    ecs_fini(world);
}

void CollectionOps_concat_mixed_elem_types(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Result(v: vector(f64))"
    LINE "const a = [1, 2] + [0.5]"
    LINE "e { Result: {a} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t result = ecs_lookup(world, "Result");
    test_assert(e != 0);
    test_assert(result != 0);
    const void *ptr = ecs_get_id(world, e, result);
    test_assert(ptr != NULL);
    char *str = ecs_ptr_to_expr(world, result, ptr);
    test_str(str, "{v: [1, 2, 0.5]}");
    ecs_os_free(str);

    ecs_fini(world);
}

void CollectionOps_concat_array_vector(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Result(v: vector(i32))"
    LINE "const a: i32[2] = [1, 2]"
    LINE "const b: i32[] = [3]"
    LINE "const c = a + b + a"
    LINE "e { Result: {c} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t result = ecs_lookup(world, "Result");
    test_assert(e != 0);
    test_assert(result != 0);
    const void *ptr = ecs_get_id(world, e, result);
    test_assert(ptr != NULL);
    char *str = ecs_ptr_to_expr(world, result, ptr);
    test_str(str, "{v: [1, 2, 3, 1, 2]}");
    ecs_os_free(str);

    ecs_fini(world);
}

void CollectionOps_concat_empty(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Result(v: vector(i64))"
    LINE "const a = [1, 2]"
    LINE "const b = a + []"
    LINE "const c: i64[] = []"
    LINE "const d = c + a"
    LINE "e { Result: {b + d} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t result = ecs_lookup(world, "Result");
    test_assert(e != 0);
    test_assert(result != 0);
    const void *ptr = ecs_get_id(world, e, result);
    test_assert(ptr != NULL);
    char *str = ecs_ptr_to_expr(world, result, ptr);
    test_str(str, "{v: [1, 2, 1, 2]}");
    ecs_os_free(str);

    ecs_fini(world);
}

void CollectionOps_concat_string_vectors(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Result(v: vector(string))"
    LINE "const a = [\"foo\", \"bar\"]"
    LINE "const b = a + [\"hello world\"] + a"
    LINE "e { Result: {b} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t result = ecs_lookup(world, "Result");
    test_assert(e != 0);
    test_assert(result != 0);
    const void *ptr = ecs_get_id(world, e, result);
    test_assert(ptr != NULL);
    char *str = ecs_ptr_to_expr(world, result, ptr);
    test_str(str, "{v: [\"foo\", \"bar\", \"hello world\", \"foo\", \"bar\"]}");
    ecs_os_free(str);

    ecs_fini(world);
}

void CollectionOps_concat_struct_vectors(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Point(x: i32, y: i32)"
    LINE "struct Result(v: vector(Point))"
    LINE "const a: Point[] = [{1, 2}]"
    LINE "const b = a + [{3, 4}]"
    LINE "e { Result: {b} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t result = ecs_lookup(world, "Result");
    test_assert(e != 0);
    test_assert(result != 0);
    const void *ptr = ecs_get_id(world, e, result);
    test_assert(ptr != NULL);
    char *str = ecs_ptr_to_expr(world, result, ptr);
    test_str(str, "{v: [{x: 1, y: 2}, {x: 3, y: 4}]}");
    ecs_os_free(str);

    ecs_fini(world);
}

void CollectionOps_concat_strings(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Result(v: string)"
    LINE "const a = \"Hello\""
    LINE "const b = a + \", \" + \"World\""
    LINE "e { Result: {b} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t result = ecs_lookup(world, "Result");
    test_assert(e != 0);
    test_assert(result != 0);
    const void *ptr = ecs_get_id(world, e, result);
    test_assert(ptr != NULL);
    char *str = ecs_ptr_to_expr(world, result, ptr);
    test_str(str, "{v: \"Hello, World\"}");
    ecs_os_free(str);

    ecs_fini(world);
}

void CollectionOps_concat_string_literals(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = { .type = ecs_id(ecs_string_t) };
    char *str = NULL;
    v.ptr = &str;
    test_assert(ecs_expr_run(world, "\"foo\" + \"bar\"", &v, NULL) != NULL);
    test_str(str, "foobar");
    ecs_os_free(str);

    ecs_fini(world);
}

void CollectionOps_concat_empty_strings(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Result(a: string, b: string)"
    LINE "const a = \"\""
    LINE "e { Result: {a + \"\", a + \"x\"} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t result = ecs_lookup(world, "Result");
    test_assert(e != 0);
    test_assert(result != 0);
    const void *ptr = ecs_get_id(world, e, result);
    test_assert(ptr != NULL);
    char *str = ecs_ptr_to_expr(world, result, ptr);
    test_str(str, "{a: \"\", b: \"x\"}");
    ecs_os_free(str);

    ecs_fini(world);
}

void CollectionOps_concat_maps(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Result(v: i32)"
    LINE "const a: map(i32, i32) = [1: 10, 2: 20]"
    LINE "const b = a + [2: 200, 3: 300]"
    LINE "e { Result: {b.count() * 10000 + b[1] + b[2] + b[3]} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t result = ecs_lookup(world, "Result");
    test_assert(e != 0);
    test_assert(result != 0);
    const int32_t *ptr = ecs_get_id(world, e, result);
    test_assert(ptr != NULL);
    test_int(*ptr, 30510);

    ecs_fini(world);
}

void CollectionOps_concat_string_w_number(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "const a = \"foo\" + 10";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) != 0);

    ecs_fini(world);
}

void CollectionOps_concat_vector_w_elem(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "const a = [1, 2] + 3";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) != 0);

    ecs_fini(world);
}

void CollectionOps_concat_incompatible_elems(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "const a = [1, 2] + [\"foo\"]";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) != 0);

    ecs_fini(world);
}

void CollectionOps_subtract_vectors(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "const a = [1, 2] - [1]";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) != 0);

    ecs_fini(world);
}

void CollectionOps_slice_vector(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Result(a: vector(i64), b: vector(i64), c: vector(i64), d: vector(i64), e: vector(i64))"
    LINE "const v = [10, 20, 30, 40]"
    LINE "e { Result: {v[1..3], v[..2], v[2..], v[..], v[2..2]} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t result = ecs_lookup(world, "Result");
    test_assert(e != 0);
    test_assert(result != 0);
    const void *ptr = ecs_get_id(world, e, result);
    test_assert(ptr != NULL);
    char *str = ecs_ptr_to_expr(world, result, ptr);
    test_str(str, "{a: [20, 30], b: [10, 20], c: [30, 40], d: [10, 20, 30, 40], e: []}");
    ecs_os_free(str);

    ecs_fini(world);
}

void CollectionOps_slice_array(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Result(v: vector(i32))"
    LINE "const a: i32[4] = [1, 2, 3, 4]"
    LINE "e { Result: {a[1..3]} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t result = ecs_lookup(world, "Result");
    const void *ptr = ecs_get_id(world, e, result);
    test_assert(ptr != NULL);
    char *str = ecs_ptr_to_expr(world, result, ptr);
    test_str(str, "{v: [2, 3]}");
    ecs_os_free(str);

    ecs_fini(world);
}

void CollectionOps_slice_var_bounds(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Result(v: vector(string))"
    LINE "const v = [\"a\", \"b\", \"c\", \"d\"]"
    LINE "const from = 1"
    LINE "const to = v.count() - 1"
    LINE "e { Result: {v[from..to]} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t result = ecs_lookup(world, "Result");
    const void *ptr = ecs_get_id(world, e, result);
    test_assert(ptr != NULL);
    char *str = ecs_ptr_to_expr(world, result, ptr);
    test_str(str, "{v: [\"b\", \"c\"]}");
    ecs_os_free(str);

    ecs_fini(world);
}

void CollectionOps_slice_w_method(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Result(v: i32)"
    LINE "const v = [1, 2, 3, 4]"
    LINE "e { Result: {v[1..].count()} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t result = ecs_lookup(world, "Result");
    const int32_t *ptr = ecs_get_id(world, e, result);
    test_assert(ptr != NULL);
    test_int(*ptr, 3);

    ecs_fini(world);
}

void CollectionOps_slice_out_of_range(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "const v = [1, 2, 3]"
    LINE "const w = v[1..4]";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) != 0);

    ecs_fini(world);
}

void CollectionOps_slice_negative(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "const v = [1, 2, 3]"
    LINE "const w = v[-1..2]";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) != 0);

    ecs_fini(world);
}

void CollectionOps_slice_reversed_bounds(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "const v = [1, 2, 3]"
    LINE "const w = v[2..1]";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) != 0);

    ecs_fini(world);
}

void CollectionOps_slice_non_collection(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "const v = 10"
    LINE "const w = v[0..1]";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) != 0);

    ecs_fini(world);
}

void CollectionOps_append_compound(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "export mut v: i32[] = [1, 2]"
            LINE "async {"
            LINE "  v += [3]"
            LINE "  v += [4, 5]"
            LINE "  v += []"
            LINE "}",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_script_tasks_progress(world);

    ecs_value_t v = ecs_mut_var_get(world, ecs_lookup(world, "v"));
    char *str = ecs_ptr_to_expr(world, v.type, v.ptr);
    test_str(str, "[1, 2, 3, 4, 5]");
    ecs_os_free(str);

    ecs_fini(world);
}

void CollectionOps_append_self(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "export mut v: string[] = [\"a\", \"b\"]"
            LINE "async {"
            LINE "  v += v"
            LINE "}",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_script_tasks_progress(world);

    ecs_value_t v = ecs_mut_var_get(world, ecs_lookup(world, "v"));
    char *str = ecs_ptr_to_expr(world, v.type, v.ptr);
    test_str(str, "[\"a\", \"b\", \"a\", \"b\"]");
    ecs_os_free(str);

    ecs_fini(world);
}

void CollectionOps_append_converts_elems(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "export mut v: f32[] = [1.5]"
            LINE "async {"
            LINE "  const w: i32[] = [2, 3]"
            LINE "  v += w"
            LINE "}",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_script_tasks_progress(world);

    ecs_value_t v = ecs_mut_var_get(world, ecs_lookup(world, "v"));
    char *str = ecs_ptr_to_expr(world, v.type, v.ptr);
    test_str(str, "[1.5, 2, 3]");
    ecs_os_free(str);

    ecs_fini(world);
}

void CollectionOps_append_string_compound(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "export mut s: string = \"Hello\""
            LINE "async {"
            LINE "  s += \" \""
            LINE "  s = s + \"World\""
            LINE "}",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_script_tasks_progress(world);

    ecs_value_t v = ecs_mut_var_get(world, ecs_lookup(world, "s"));
    test_str(*(char**)v.ptr, "Hello World");

    ecs_fini(world);
}

void CollectionOps_append_local_mut(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "export mut out: i32 = 0"
            LINE "async {"
            LINE "  mut v: i32[] = []"
            LINE "  for i in 0..5 {"
            LINE "    v += [i]"
            LINE "  }"
            LINE "  out = v.sum()"
            LINE "}",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_script_tasks_progress(world);

    test_int(*(int32_t*)ecs_mut_var_get(world, ecs_lookup(world, "out")).ptr, 10);

    ecs_fini(world);
}

void CollectionOps_append_member(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "struct Item(values: vector(i32))"
            LINE "export mut items: Item[] = [{[1]}, {[2]}]"
            LINE "async {"
            LINE "  items[1].values += [3, 4]"
            LINE "}",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_script_tasks_progress(world);

    ecs_value_t v = ecs_mut_var_get(world, ecs_lookup(world, "items"));
    char *str = ecs_ptr_to_expr(world, v.type, v.ptr);
    test_str(str, "[{values: [1]}, {values: [2, 3, 4]}]");
    ecs_os_free(str);

    ecs_fini(world);
}

void CollectionOps_append_notifies(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t position = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Position" }),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "export mut v: i32[] = [1]"
            LINE "template T {"
            LINE "  Position: {v.count(), 0}"
            LINE "}"
            LINE "T e"
            LINE "async {"
            LINE "  v += [2, 3]"
            LINE "}",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_script_tasks_progress(world);
    ecs_progress(world, 0);

    const ecs_vec_t *p = ecs_get_id(world, ecs_lookup(world, "e"), position);
    test_assert(p != NULL);
    test_int(((float*)p)[0], 3);

    ecs_fini(world);
}

void CollectionOps_slice_assign(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "export mut a: i32[] = [1, 2, 3, 4]"
            LINE "export mut b: i32[] = [1, 2, 3, 4]"
            LINE "export mut c: i32[] = [1, 2, 3, 4]"
            LINE "export mut d: i32[] = [1, 2, 3, 4]"
            LINE "export mut e: i32[] = [1, 2, 3, 4]"
            LINE "async {"
            LINE "  a[1..3] = [7, 8, 9]"
            LINE "  b[1..1] = [5]"
            LINE "  c[0..2] = []"
            LINE "  d[..] = []"
            LINE "  e[2..] = [0]"
            LINE "}",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_script_tasks_progress(world);

    const char *names[] = {"a", "b", "c", "d", "e"};
    const char *expect[] = {
        "[1, 7, 8, 9, 4]", "[1, 5, 2, 3, 4]", "[3, 4]", "[]", "[1, 2, 0]"};
    for (int i = 0; i < 5; i ++) {
        ecs_value_t v = ecs_mut_var_get(world, ecs_lookup(world, names[i]));
        char *str = ecs_ptr_to_expr(world, v.type, v.ptr);
        test_str(str, expect[i]);
        ecs_os_free(str);
    }

    ecs_fini(world);
}

void CollectionOps_slice_assign_strings(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "export mut v: string[] = [\"a\", \"b\", \"c\"]"
            LINE "async {"
            LINE "  v[0..1] = [\"x\", \"y\"]"
            LINE "  v[2..] = v[..2]"
            LINE "}",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_script_tasks_progress(world);

    ecs_value_t v = ecs_mut_var_get(world, ecs_lookup(world, "v"));
    char *str = ecs_ptr_to_expr(world, v.type, v.ptr);
    test_str(str, "[\"x\", \"y\", \"x\", \"y\"]");
    ecs_os_free(str);

    ecs_fini(world);
}

void CollectionOps_slice_assign_self(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "export mut v: i32[] = [1, 2]"
            LINE "async {"
            LINE "  v[1..1] = v"
            LINE "}",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_script_tasks_progress(world);

    ecs_value_t v = ecs_mut_var_get(world, ecs_lookup(world, "v"));
    char *str = ecs_ptr_to_expr(world, v.type, v.ptr);
    test_str(str, "[1, 1, 2, 2]");
    ecs_os_free(str);

    ecs_fini(world);
}

void CollectionOps_slice_assign_array(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "export mut a: i32[4] = [1, 2, 3, 4]"
            LINE "async {"
            LINE "  a[1..3] = [5, 6]"
            LINE "}",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_script_tasks_progress(world);

    const int32_t *a = ecs_mut_var_get(world, ecs_lookup(world, "a")).ptr;
    test_int(a[0], 1);
    test_int(a[1], 5);
    test_int(a[2], 6);
    test_int(a[3], 4);

    ecs_fini(world);
}

void CollectionOps_slice_assign_array_count_mismatch(void) {
    ecs_world_t *world = ecs_init();

    ecs_log_set_level(-4);

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "export mut a: i32[4] = [1, 2, 3, 4]"
            LINE "export mut after: i32 = 0"
            LINE "async {"
            LINE "  a[1..3] = [5]"
            LINE "  after = 1"
            LINE "}",
        .ir = ir_enabled
    });
    test_assert(s != 0);

    ecs_script_tasks_progress(world);

    const int32_t *a = ecs_mut_var_get(world, ecs_lookup(world, "a")).ptr;
    test_int(a[0], 1);
    test_int(a[1], 2);
    test_int(a[2], 3);
    test_int(a[3], 4);
    test_int(*(int32_t*)ecs_mut_var_get(world, ecs_lookup(world, "after")).ptr, 0);

    ecs_fini(world);
}

void CollectionOps_slice_assign_out_of_range(void) {
    ecs_world_t *world = ecs_init();

    ecs_log_set_level(-4);

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "export mut v: i32[] = [1, 2, 3]"
            LINE "export mut after: i32 = 0"
            LINE "export mut idx: i32 = 4"
            LINE "async { v[1..idx] = []; after = 1 }"
            LINE "async { v[2..1] = []; after = 2 }",
        .ir = ir_enabled
    });
    test_assert(s != 0);

    ecs_script_tasks_progress(world);

    ecs_value_t v = ecs_mut_var_get(world, ecs_lookup(world, "v"));
    char *str = ecs_ptr_to_expr(world, v.type, v.ptr);
    test_str(str, "[1, 2, 3]");
    ecs_os_free(str);
    test_int(*(int32_t*)ecs_mut_var_get(world, ecs_lookup(world, "after")).ptr, 0);

    ecs_fini(world);
}

void CollectionOps_slice_assign_not_last(void) {
    ecs_world_t *world = ecs_init();

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL,
        HEAD "struct Point(x: i32, y: i32)"
        LINE "export mut v: Point[] = [{1, 2}]"
        LINE "async {"
        LINE "  v[0..1].x = 10"
        LINE "}", &ir_desc, NULL) != 0);

    ecs_fini(world);
}

void CollectionOps_push(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "export mut v: i32[] = [1]"
            LINE "async {"
            LINE "  v.push(2)"
            LINE "  v.push(3, 4)"
            LINE "  v.push(v[0])"
            LINE "}",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_script_tasks_progress(world);

    ecs_value_t v = ecs_mut_var_get(world, ecs_lookup(world, "v"));
    char *str = ecs_ptr_to_expr(world, v.type, v.ptr);
    test_str(str, "[1, 2, 3, 4, 1]");
    ecs_os_free(str);

    ecs_fini(world);
}

void CollectionOps_push_struct(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "struct Point(x: i32, y: i32)"
            LINE "export mut v: Point[] = []"
            LINE "async {"
            LINE "  v.push({1, 2})"
            LINE "  v.push({x: 3, y: 4})"
            LINE "}",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_script_tasks_progress(world);

    ecs_value_t v = ecs_mut_var_get(world, ecs_lookup(world, "v"));
    char *str = ecs_ptr_to_expr(world, v.type, v.ptr);
    test_str(str, "[{x: 1, y: 2}, {x: 3, y: 4}]");
    ecs_os_free(str);

    ecs_fini(world);
}

void CollectionOps_push_string(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "export mut v: string[] = []"
            LINE "async {"
            LINE "  for i in 0..20 {"
            LINE "    v.push(\"item {i}\")"
            LINE "  }"
            LINE "}",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_script_tasks_progress(world);

    const ecs_vec_t *v = ecs_mut_var_get(world, ecs_lookup(world, "v")).ptr;
    test_int(ecs_vec_count(v), 20);
    test_str(ecs_vec_first_t(v, char*)[0], "item 0");
    test_str(ecs_vec_first_t(v, char*)[19], "item 19");

    ecs_fini(world);
}

void CollectionOps_push_member_element(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "struct Item(values: vector(i32))"
            LINE "export mut items: Item[] = [{[1]}, {[2]}]"
            LINE "async {"
            LINE "  items[1].values.push(5)"
            LINE "  const i = 0"
            LINE "  items[i].values.push(6)"
            LINE "}",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_script_tasks_progress(world);

    ecs_value_t v = ecs_mut_var_get(world, ecs_lookup(world, "items"));
    char *str = ecs_ptr_to_expr(world, v.type, v.ptr);
    test_str(str, "[{values: [1, 6]}, {values: [2, 5]}]");
    ecs_os_free(str);

    ecs_fini(world);
}

void CollectionOps_push_local_mut(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "export mut out: i32 = 0"
            LINE "async {"
            LINE "  mut v: i32[] = []"
            LINE "  v.push(10)"
            LINE "  v.push(20)"
            LINE "  out = v.sum()"
            LINE "}",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_script_tasks_progress(world);

    test_int(*(int32_t*)ecs_mut_var_get(world, ecs_lookup(world, "out")).ptr, 30);

    ecs_fini(world);
}

void CollectionOps_push_in_function(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "export mut v: i32[] = []"
            LINE "fn add(x: i32) {"
            LINE "  v.push(x)"
            LINE "  v.push(x * 2)"
            LINE "}"
            LINE "async {"
            LINE "  add(3)"
            LINE "}",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_script_tasks_progress(world);

    ecs_value_t v = ecs_mut_var_get(world, ecs_lookup(world, "v"));
    char *str = ecs_ptr_to_expr(world, v.type, v.ptr);
    test_str(str, "[3, 6]");
    ecs_os_free(str);

    ecs_fini(world);
}

void CollectionOps_push_notifies(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t position = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Position" }),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "export mut v: i32[] = [1]"
            LINE "template T {"
            LINE "  Position: {v.count(), 0}"
            LINE "}"
            LINE "T e"
            LINE "async {"
            LINE "  v.push(2)"
            LINE "}",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_script_tasks_progress(world);
    ecs_progress(world, 0);

    const float *p = ecs_get_id(world, ecs_lookup(world, "e"), position);
    test_assert(p != NULL);
    test_int(p[0], 2);

    ecs_fini(world);
}

void CollectionOps_push_template_mut(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t position = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Position" }),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "template T {"
            LINE "  mut v: i32[] = []"
            LINE "  Position: {v.count(), v.sum()}"
            LINE "  async {"
            LINE "    v.push(4)"
            LINE "    v.push(5)"
            LINE "  }"
            LINE "}"
            LINE "T e",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_script_tasks_progress(world);
    ecs_progress(world, 0);

    const float *p = ecs_get_id(world, ecs_lookup(world, "e"), position);
    test_assert(p != NULL);
    test_int(p[0], 2);
    test_int(p[1], 9);

    ecs_fini(world);
}

void CollectionOps_push_component(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "struct Bag(items: vector(i32))"
            LINE "e { Bag: {[1]} }"
            LINE "async {"
            LINE "  e[Bag].items.push(2)"
            LINE "}",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_script_tasks_progress(world);

    ecs_entity_t bag = ecs_lookup(world, "Bag");
    const void *ptr = ecs_get_id(world, ecs_lookup(world, "e"), bag);
    test_assert(ptr != NULL);
    char *str = ecs_ptr_to_expr(world, bag, ptr);
    test_str(str, "{items: [1, 2]}");
    ecs_os_free(str);

    ecs_fini(world);
}

void CollectionOps_pop(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "export mut v: i32[] = [1, 2, 3, 4]"
            LINE "export mut out: i32 = 0"
            LINE "async {"
            LINE "  const x = v.pop()"
            LINE "  out = x * 10 + v.pop()"
            LINE "  v.pop()"
            LINE "}",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_script_tasks_progress(world);

    test_int(*(int32_t*)ecs_mut_var_get(world, ecs_lookup(world, "out")).ptr, 43);
    ecs_value_t v = ecs_mut_var_get(world, ecs_lookup(world, "v"));
    char *str = ecs_ptr_to_expr(world, v.type, v.ptr);
    test_str(str, "[1]");
    ecs_os_free(str);

    ecs_fini(world);
}

void CollectionOps_pop_string(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "export mut v: string[] = [\"foo\", \"bar\"]"
            LINE "export mut out: string = \"\""
            LINE "async {"
            LINE "  out = v.pop()"
            LINE "}",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_script_tasks_progress(world);

    test_str(*(char**)ecs_mut_var_get(world, ecs_lookup(world, "out")).ptr, "bar");
    const ecs_vec_t *v = ecs_mut_var_get(world, ecs_lookup(world, "v")).ptr;
    test_int(ecs_vec_count(v), 1);

    ecs_fini(world);
}

void CollectionOps_pop_empty(void) {
    ecs_world_t *world = ecs_init();

    ecs_log_set_level(-4);

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "export mut v: i32[] = []"
            LINE "export mut after: i32 = 0"
            LINE "async {"
            LINE "  v.pop()"
            LINE "  after = 1"
            LINE "}",
        .ir = ir_enabled
    });
    test_assert(s != 0);

    ecs_script_tasks_progress(world);

    test_int(*(int32_t*)ecs_mut_var_get(world, ecs_lookup(world, "after")).ptr, 0);

    ecs_fini(world);
}

void CollectionOps_insert(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "export mut v: i32[] = [1, 2]"
            LINE "async {"
            LINE "  v.insert(1, 9)"
            LINE "  v.insert(0, 7, 8)"
            LINE "  v.insert(v.count(), 10)"
            LINE "}",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_script_tasks_progress(world);

    ecs_value_t v = ecs_mut_var_get(world, ecs_lookup(world, "v"));
    char *str = ecs_ptr_to_expr(world, v.type, v.ptr);
    test_str(str, "[7, 8, 1, 9, 2, 10]");
    ecs_os_free(str);

    ecs_fini(world);
}

void CollectionOps_insert_out_of_range(void) {
    ecs_world_t *world = ecs_init();

    ecs_log_set_level(-4);

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "export mut v: i32[] = [1, 2]"
            LINE "export mut after: i32 = 0"
            LINE "async { v.insert(3, 0); after = 1 }"
            LINE "async { v.insert(-1, 0); after = 2 }",
        .ir = ir_enabled
    });
    test_assert(s != 0);

    ecs_script_tasks_progress(world);

    ecs_value_t v = ecs_mut_var_get(world, ecs_lookup(world, "v"));
    char *str = ecs_ptr_to_expr(world, v.type, v.ptr);
    test_str(str, "[1, 2]");
    ecs_os_free(str);
    test_int(*(int32_t*)ecs_mut_var_get(world, ecs_lookup(world, "after")).ptr, 0);

    ecs_fini(world);
}

void CollectionOps_remove(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "export mut v: string[] = [\"a\", \"b\", \"c\", \"d\"]"
            LINE "async {"
            LINE "  v.remove(1)"
            LINE "  v.remove(v.count() - 1)"
            LINE "}",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_script_tasks_progress(world);

    ecs_value_t v = ecs_mut_var_get(world, ecs_lookup(world, "v"));
    char *str = ecs_ptr_to_expr(world, v.type, v.ptr);
    test_str(str, "[\"a\", \"c\"]");
    ecs_os_free(str);

    ecs_fini(world);
}

void CollectionOps_remove_out_of_range(void) {
    ecs_world_t *world = ecs_init();

    ecs_log_set_level(-4);

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "export mut v: i32[] = [1, 2]"
            LINE "export mut after: i32 = 0"
            LINE "async { v.remove(2); after = 1 }",
        .ir = ir_enabled
    });
    test_assert(s != 0);

    ecs_script_tasks_progress(world);

    ecs_value_t v = ecs_mut_var_get(world, ecs_lookup(world, "v"));
    char *str = ecs_ptr_to_expr(world, v.type, v.ptr);
    test_str(str, "[1, 2]");
    ecs_os_free(str);
    test_int(*(int32_t*)ecs_mut_var_get(world, ecs_lookup(world, "after")).ptr, 0);

    ecs_fini(world);
}

void CollectionOps_clear(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "export mut v: string[] = [\"a\", \"b\"]"
            LINE "async {"
            LINE "  v.clear()"
            LINE "  v.push(\"c\")"
            LINE "}",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_script_tasks_progress(world);

    ecs_value_t v = ecs_mut_var_get(world, ecs_lookup(world, "v"));
    char *str = ecs_ptr_to_expr(world, v.type, v.ptr);
    test_str(str, "[\"c\"]");
    ecs_os_free(str);

    ecs_fini(world);
}

void CollectionOps_map_remove_clear(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "export mut a: map(i32, string) = [1: \"a\", 2: \"b\"]"
            LINE "export mut b: map(i32, string) = [1: \"a\", 2: \"b\"]"
            LINE "export mut out: i32 = 0"
            LINE "async {"
            LINE "  a.remove(1)"
            LINE "  a.remove(3)"
            LINE "  b.clear()"
            LINE "  out = a.count() * 10 + b.count()"
            LINE "}",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_script_tasks_progress(world);

    test_int(*(int32_t*)ecs_mut_var_get(world, ecs_lookup(world, "out")).ptr, 10);
    ecs_value_t v = ecs_mut_var_get(world, ecs_lookup(world, "a"));
    char *str = ecs_ptr_to_expr(world, v.type, v.ptr);
    test_str(str, "[2: \"b\"]");
    ecs_os_free(str);

    ecs_fini(world);
}

void CollectionOps_push_const(void) {
    ecs_world_t *world = ecs_init();

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL,
        HEAD "const v = [1, 2]"
        LINE "async {"
        LINE "  v.push(3)"
        LINE "}", &ir_desc, NULL) != 0);

    ecs_fini(world);
}

void CollectionOps_push_array(void) {
    ecs_world_t *world = ecs_init();

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL,
        HEAD "export mut v: i32[2] = [1, 2]"
        LINE "async {"
        LINE "  v.push(3)"
        LINE "}", &ir_desc, NULL) != 0);

    ecs_fini(world);
}

void CollectionOps_push_map(void) {
    ecs_world_t *world = ecs_init();

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL,
        HEAD "export mut v: map(i32, i32) = [1: 2]"
        LINE "async {"
        LINE "  v.push(3)"
        LINE "}", &ir_desc, NULL) != 0);

    ecs_fini(world);
}

void CollectionOps_push_outside_async(void) {
    ecs_world_t *world = ecs_init();

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL,
        HEAD "export mut v: i32[] = [1, 2]"
        LINE "const x = v.pop()", &ir_desc, NULL) != 0);

    ecs_fini(world);
}

void CollectionOps_push_as_value(void) {
    ecs_world_t *world = ecs_init();

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL,
        HEAD "export mut v: i32[] = [1, 2]"
        LINE "async {"
        LINE "  const x = v.push(3)"
        LINE "}", &ir_desc, NULL) != 0);

    ecs_fini(world);
}

void CollectionOps_push_wrong_arg_count(void) {
    ecs_world_t *world = ecs_init();

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL,
        HEAD "export mut v: i32[] = [1, 2]"
        LINE "async {"
        LINE "  v.push()"
        LINE "}", &ir_desc, NULL) != 0);

    test_assert(ecs_script_run_w_desc(world, NULL,
        HEAD "export mut w: i32[] = [1, 2]"
        LINE "async {"
        LINE "  w.pop(1)"
        LINE "}", &ir_desc, NULL) != 0);

    ecs_fini(world);
}

void CollectionOps_push_wrong_type(void) {
    ecs_world_t *world = ecs_init();

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL,
        HEAD "export mut v: i32[] = [1, 2]"
        LINE "async {"
        LINE "  v.push(\"foo\")"
        LINE "}", &ir_desc, NULL) != 0);

    ecs_fini(world);
}

void CollectionOps_contains(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Result(a: bool, b: bool, c: bool, d: bool, e: bool, f: bool)"
    LINE "const v = [1, 2, 3]"
    LINE "const s = [\"foo\", \"bar\"]"
    LINE "const m: map(i32, i32) = [1: 10]"
    LINE "e { Result: {v.contains(2), v.contains(5), s.contains(\"bar\"), s.contains(\"baz\"), m.contains(1), m.contains(10)} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t result = ecs_lookup(world, "Result");
    const void *ptr = ecs_get_id(world, e, result);
    test_assert(ptr != NULL);
    char *str = ecs_ptr_to_expr(world, result, ptr);
    test_str(str, "{a: true, b: false, c: true, d: false, e: true, f: false}");
    ecs_os_free(str);

    ecs_fini(world);
}

void CollectionOps_contains_struct(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Point(x: i32, y: i32)"
    LINE "struct Result(a: bool, b: bool)"
    LINE "const v: Point[] = [{1, 2}, {3, 4}]"
    LINE "e { Result: {v.contains({3, 4}), v.contains({4, 3})} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t result = ecs_lookup(world, "Result");
    const void *ptr = ecs_get_id(world, e, result);
    test_assert(ptr != NULL);
    char *str = ecs_ptr_to_expr(world, result, ptr);
    test_str(str, "{a: true, b: false}");
    ecs_os_free(str);

    ecs_fini(world);
}

void CollectionOps_index_of(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Result(a: i32, b: i32, c: i32)"
    LINE "const v = [\"a\", \"b\", \"c\", \"b\"]"
    LINE "e { Result: {v.index_of(\"b\"), v.index_of(\"c\"), v.index_of(\"d\")} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t result = ecs_lookup(world, "Result");
    const void *ptr = ecs_get_id(world, e, result);
    test_assert(ptr != NULL);
    char *str = ecs_ptr_to_expr(world, result, ptr);
    test_str(str, "{a: 1, b: 2, c: -1}");
    ecs_os_free(str);

    ecs_fini(world);
}

void CollectionOps_first_last(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Result(a: i32, b: i32, c: string)"
    LINE "const v = [5, 6, 7]"
    LINE "const a: string[2] = [\"x\", \"y\"]"
    LINE "e { Result: {v.first(), v.last(), a.last()} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t result = ecs_lookup(world, "Result");
    const void *ptr = ecs_get_id(world, e, result);
    test_assert(ptr != NULL);
    char *str = ecs_ptr_to_expr(world, result, ptr);
    test_str(str, "{a: 5, b: 7, c: \"y\"}");
    ecs_os_free(str);

    ecs_fini(world);
}

void CollectionOps_first_empty(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "const v: i32[] = []"
    LINE "const x = v.first()";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) != 0);

    ecs_fini(world);
}

void CollectionOps_reverse(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Result(a: vector(i64), b: vector(string), c: vector(i32))"
    LINE "const v = [1, 2, 3]"
    LINE "const s = [\"a\", \"b\"]"
    LINE "const a: i32[3] = [4, 5, 6]"
    LINE "e { Result: {v.reverse(), s.reverse(), a.reverse()} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t result = ecs_lookup(world, "Result");
    const void *ptr = ecs_get_id(world, e, result);
    test_assert(ptr != NULL);
    char *str = ecs_ptr_to_expr(world, result, ptr);
    test_str(str, "{a: [3, 2, 1], b: [\"b\", \"a\"], c: [6, 5, 4]}");
    ecs_os_free(str);

    ecs_fini(world);
}

void CollectionOps_sort(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Result(a: vector(i64), b: vector(string), c: vector(f64))"
    LINE "const v = [5, 3, 9, 1, 3, 7, 2, 8]"
    LINE "const s = [\"pear\", \"apple\", \"fig\"]"
    LINE "const f = [2.5, -1.0, 0.5]"
    LINE "e { Result: {v.sort(), s.sort(), f.sort()} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t result = ecs_lookup(world, "Result");
    const void *ptr = ecs_get_id(world, e, result);
    test_assert(ptr != NULL);
    char *str = ecs_ptr_to_expr(world, result, ptr);
    test_str(str, "{a: [1, 2, 3, 3, 5, 7, 8, 9], b: [\"apple\", \"fig\", \"pear\"], c: [-1, 0.5, 2.5]}");
    ecs_os_free(str);

    ecs_fini(world);
}

void CollectionOps_sort_w_lambda(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Item(name: string, height: i32)"
    LINE "struct Result(v: vector(Item), w: vector(i64))"
    LINE "const v: Item[] = [{\"a\", 3}, {\"b\", 1}, {\"c\", 2}, {\"d\", 1}]"
    LINE "const w = [1, 2, 3, 4]"
    LINE "e { Result: {v.sort((el) -> { el.height }), w.sort((i, el) -> { -i })} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t result = ecs_lookup(world, "Result");
    const void *ptr = ecs_get_id(world, e, result);
    test_assert(ptr != NULL);
    char *str = ecs_ptr_to_expr(world, result, ptr);
    test_str(str, "{v: [{name: \"b\", height: 1}, {name: \"d\", height: 1}, {name: \"c\", height: 2}, {name: \"a\", height: 3}], w: [4, 3, 2, 1]}");
    ecs_os_free(str);

    ecs_fini(world);
}

void CollectionOps_sort_empty(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Result(v: vector(i32))"
    LINE "const v: i32[] = []"
    LINE "e { Result: {v.sort()} }";

    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t result = ecs_lookup(world, "Result");
    const void *ptr = ecs_get_id(world, e, result);
    test_assert(ptr != NULL);
    char *str = ecs_ptr_to_expr(world, result, ptr);
    test_str(str, "{v: []}");
    ecs_os_free(str);

    ecs_fini(world);
}

void CollectionOps_contains_w_lambda(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "const v = [1, 2]"
    LINE "const x = v.contains((el) -> { el })";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) != 0);

    ecs_fini(world);
}

void CollectionOps_index_of_map(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "const m: map(i32, i32) = [1: 2]"
    LINE "const x = m.index_of(2)";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL, expr, &ir_desc, NULL) != 0);

    ecs_fini(world);
}

void CollectionOps_methods_in_async(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "export mut v: i32[] = [3, 1, 2]"
            LINE "export mut out: i32 = 0"
            LINE "async {"
            LINE "  v = v.sort()"
            LINE "  if v.contains(2) {"
            LINE "    out = v.index_of(2) * 100 + v.first() * 10 + v.last()"
            LINE "  }"
            LINE "}",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_script_tasks_progress(world);

    test_int(*(int32_t*)ecs_mut_var_get(world, ecs_lookup(world, "out")).ptr, 113);

    ecs_fini(world);
}
