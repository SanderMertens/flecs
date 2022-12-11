#include <meta.h>

void Vars_declare_1_var() {
    ecs_world_t *world = ecs_init();

    ecs_vars_t vars = {0};
    ecs_vars_init(world, &vars);

    ecs_expr_var_t *v = ecs_vars_declare(
        &vars, "foo", ecs_id(ecs_i32_t));
    test_assert(v != NULL);
    test_assert(v->value.ptr != NULL);
    test_assert(v->value.type != 0);
    test_uint(v->value.type, ecs_id(ecs_i32_t));
    test_int(*(int32_t*)v->value.ptr, 0);

    const ecs_expr_var_t *v2 = ecs_vars_lookup(&vars, "foo");
    test_assert(v2 != NULL);
    test_assert(v == v2);

    ecs_vars_fini(&vars);

    ecs_fini(world);
}

void Vars_declare_2_vars() {
    ecs_world_t *world = ecs_init();

    ecs_vars_t vars = {0};
    ecs_vars_init(world, &vars);

    ecs_expr_var_t *foo = ecs_vars_declare(
        &vars, "foo", ecs_id(ecs_i32_t));
    test_assert(foo != NULL);
    test_assert(foo->value.ptr != NULL);
    test_assert(foo->value.type != 0);
    test_uint(foo->value.type, ecs_id(ecs_i32_t));
    test_int(*(int32_t*)foo->value.ptr, 0);

    ecs_expr_var_t *bar = ecs_vars_declare(
        &vars, "bar", ecs_id(ecs_u32_t));
    test_assert(bar != NULL);
    test_assert(bar->value.ptr != NULL);
    test_assert(bar->value.type != 0);
    test_uint(bar->value.type, ecs_id(ecs_u32_t));
    test_uint(*(ecs_u32_t*)bar->value.ptr, 0);

    test_assert(foo == ecs_vars_lookup(&vars, "foo"));
    test_assert(bar == ecs_vars_lookup(&vars, "bar"));
    test_assert(foo != bar);

    ecs_vars_fini(&vars);

    ecs_fini(world);
}

void Vars_declare_vars_nested_scope() {
    ecs_world_t *world = ecs_init();

    ecs_vars_t vars = {0};
    ecs_vars_init(world, &vars);

    ecs_vars_push(&vars);

    ecs_expr_var_t *foo = ecs_vars_declare(
        &vars, "foo", ecs_id(ecs_i32_t));
    test_assert(foo != NULL);
    test_assert(foo->value.ptr != NULL);
    test_assert(foo->value.type != 0);
    test_uint(foo->value.type, ecs_id(ecs_i32_t));
    test_int(*(int32_t*)foo->value.ptr, 0);
    test_assert(foo == ecs_vars_lookup(&vars, "foo"));

    test_int(0, ecs_vars_pop(&vars));

    test_assert(NULL == ecs_vars_lookup(&vars, "foo"));

    ecs_vars_fini(&vars);

    ecs_fini(world);
}

void Vars_declare_vars_2_scopes() {
    ecs_world_t *world = ecs_init();

    ecs_vars_t vars = {0};
    ecs_vars_init(world, &vars);

    ecs_expr_var_t *foo = ecs_vars_declare(
        &vars, "foo", ecs_id(ecs_i32_t));
    test_assert(foo != NULL);
    test_assert(foo->value.ptr != NULL);
    test_assert(foo->value.type != 0);
    test_uint(foo->value.type, ecs_id(ecs_i32_t));
    test_int(*(int32_t*)foo->value.ptr, 0);
    test_assert(foo == ecs_vars_lookup(&vars, "foo"));

    ecs_vars_push(&vars);

    ecs_expr_var_t *bar = ecs_vars_declare(
        &vars, "bar", ecs_id(ecs_u32_t));
    test_assert(bar != NULL);
    test_assert(bar->value.ptr != NULL);
    test_assert(bar->value.type != 0);
    test_uint(bar->value.type, ecs_id(ecs_u32_t));
    test_uint(*(ecs_u32_t*)bar->value.ptr, 0);

    test_assert(foo == ecs_vars_lookup(&vars, "foo"));
    test_assert(bar == ecs_vars_lookup(&vars, "bar"));
    test_assert(foo != bar);

    ecs_vars_pop(&vars);

    test_assert(foo == ecs_vars_lookup(&vars, "foo"));
    test_assert(NULL == ecs_vars_lookup(&vars, "bar"));

    ecs_vars_fini(&vars);

    ecs_fini(world);
}

void Vars_redeclare_var() {
    ecs_world_t *world = ecs_init();

    ecs_vars_t vars = {0};
    ecs_vars_init(world, &vars);

    ecs_expr_var_t *foo = ecs_vars_declare(
        &vars, "foo", ecs_id(ecs_i32_t));
    test_assert(foo != NULL);
    test_assert(foo->value.ptr != NULL);
    test_assert(foo->value.type != 0);
    test_uint(foo->value.type, ecs_id(ecs_i32_t));
    test_int(*(int32_t*)foo->value.ptr, 0);
    test_assert(foo == ecs_vars_lookup(&vars, "foo"));

    ecs_log_set_level(-4);
    ecs_expr_var_t *foo_2 = ecs_vars_declare(
        &vars, "foo", ecs_id(ecs_i32_t));
    test_assert(foo_2 == NULL);

    ecs_vars_fini(&vars);

    ecs_fini(world);
}

void Vars_i32_expr_w_i32_var() {
    ecs_world_t *world = ecs_init();

    ecs_vars_t vars = {0};
    ecs_vars_init(world, &vars);

    ecs_expr_var_t *foo = ecs_vars_declare(
        &vars, "foo", ecs_id(ecs_i32_t));
    *(int32_t*)foo->value.ptr = 10;

    int32_t v = 0;
    ecs_parse_expr_desc_t desc = { .vars = &vars };
    const char *ptr = ecs_parse_expr(
        world, "$foo", &ecs_value(ecs_i32_t, &v), &desc);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == 0);
    test_int(v, 10);

    ecs_vars_fini(&vars);

    ecs_fini(world);
}

void Vars_i32_expr_w_f32_var() {
    ecs_world_t *world = ecs_init();

    ecs_vars_t vars = {0};
    ecs_vars_init(world, &vars);

    ecs_expr_var_t *foo = ecs_vars_declare(
        &vars, "foo", ecs_id(ecs_f32_t));
    *(float*)foo->value.ptr = 10.5;

    int32_t v = 0;
    ecs_parse_expr_desc_t desc = { .vars = &vars };
    const char *ptr = ecs_parse_expr(
        world, "$foo", &ecs_value(ecs_i32_t, &v), &desc);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == 0);
    test_int(v, 10);

    ecs_vars_fini(&vars);

    ecs_fini(world);
}

void Vars_i32_expr_w_string_var() {
    ecs_world_t *world = ecs_init();

    ecs_vars_t vars = {0};
    ecs_vars_init(world, &vars);

    ecs_expr_var_t *foo = ecs_vars_declare(&vars, "foo", ecs_id(ecs_string_t));
    *(char**)foo->value.ptr = "10";

    int32_t v = 0;
    ecs_parse_expr_desc_t desc = { .vars = &vars };
    const char *ptr = ecs_parse_expr(
        world, "$foo", &ecs_value(ecs_i32_t, &v), &desc);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == 0);
    test_int(v, 10);

    *(char**)foo->value.ptr = NULL;
    ecs_vars_fini(&vars);

    ecs_fini(world);
}

void Vars_string_expr_w_string_var() {
    ecs_world_t *world = ecs_init();

    ecs_vars_t vars = {0};
    ecs_vars_init(world, &vars);

    ecs_expr_var_t *foo = ecs_vars_declare(&vars, "foo", ecs_id(ecs_string_t));
    *(char**)foo->value.ptr = "Hello World";

    char* v = NULL;
    ecs_parse_expr_desc_t desc = { .vars = &vars };
    const char *ptr = ecs_parse_expr(
        world, "$foo", &ecs_value(ecs_string_t, &v), &desc);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == 0);
    test_str(v, "Hello World");
    
    ecs_os_free(v);

    *(char**)foo->value.ptr = NULL;
    ecs_vars_fini(&vars);

    ecs_fini(world);
}

void Vars_struct_expr_w_i32_vars() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t point = ecs_struct_init(world, &(ecs_struct_desc_t){
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    ecs_vars_t vars = {0};
    ecs_vars_init(world, &vars);

    ecs_expr_var_t *foo = ecs_vars_declare(&vars, "foo", ecs_id(ecs_i32_t));
    *(int32_t*)foo->value.ptr = 10;
    ecs_expr_var_t *bar = ecs_vars_declare(&vars, "bar", ecs_id(ecs_i32_t));
    *(int32_t*)bar->value.ptr = 20;

    Position v = {0};
    ecs_parse_expr_desc_t desc = { .vars = &vars };
    const char *ptr = ecs_parse_expr(world, "{$foo, $bar}", &(ecs_value_t){point, &v}, &desc);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == 0);

    test_int(v.x, 10);
    test_int(v.y, 20);

    ecs_vars_fini(&vars);

    ecs_fini(world);
}

void Vars_struct_expr_w_struct_var() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t point = ecs_struct_init(world, &(ecs_struct_desc_t){
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    ecs_vars_t vars = {0};
    ecs_vars_init(world, &vars);

    ecs_expr_var_t *foo = ecs_vars_declare(&vars, "foo", point);
    *(Position*)foo->value.ptr = (Position){10, 20};

    Position v = {0};
    ecs_parse_expr_desc_t desc = { .vars = &vars };
    const char *ptr = ecs_parse_expr(world, "$foo", &(ecs_value_t){point, &v}, &desc);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == 0);

    test_int(v.x, 10);
    test_int(v.y, 20);

    ecs_vars_fini(&vars);

    ecs_fini(world);
}

void Vars_nested_struct_expr_w_struct_var() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t point = ecs_struct_init(world, &(ecs_struct_desc_t){
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t line = ecs_struct_init(world, &(ecs_struct_desc_t){
        .members = {
            {"start", point},
            {"stop", point}
        }
    });

    ecs_vars_t vars = {0};
    ecs_vars_init(world, &vars);

    ecs_expr_var_t *foo = ecs_vars_declare(&vars, "foo", point);
    *(Position*)foo->value.ptr = (Position){10, 20};
    ecs_expr_var_t *bar = ecs_vars_declare(&vars, "bar", point);
    *(Position*)bar->value.ptr = (Position){30, 40};

    Line v = {0};
    ecs_parse_expr_desc_t desc = { .vars = &vars };
    const char *ptr = ecs_parse_expr(world, 
        "{start: $foo, stop: $bar}", &(ecs_value_t){line, &v}, &desc);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == 0);

    test_int(v.start.x, 10);
    test_int(v.start.y, 20);
    test_int(v.stop.x, 30);
    test_int(v.stop.y, 40);

    ecs_vars_fini(&vars);

    ecs_fini(world);
}

void Vars_declare_w_value() {
    ecs_world_t *world = ecs_init();

    ecs_vars_t vars = {0};
    ecs_vars_init(world, &vars);

    ecs_value_t src;
    src.type = ecs_id(ecs_i32_t);
    src.ptr = ecs_value_new_t(world, ecs_i32_t);
    *(int32_t*)src.ptr = 10;
    void *src_ptr = src.ptr;

    ecs_expr_var_t *v = ecs_vars_declare_w_value(
        &vars, "foo", &src);
    test_assert(v != NULL);
    test_assert(v->value.ptr != NULL);
    test_assert(v->value.type != 0);
    test_uint(v->value.type, ecs_id(ecs_i32_t));
    test_int(*(int32_t*)v->value.ptr, 10);
    test_assert(v->value.ptr == src_ptr);
    test_assert(src.ptr == NULL);

    const ecs_expr_var_t *v2 = ecs_vars_lookup(&vars, "foo");
    test_assert(v2 != NULL);
    test_assert(v == v2);

    ecs_vars_fini(&vars);

    ecs_fini(world);
}

void Vars_redeclare_in_scope() {
    ecs_world_t *world = ecs_init();

    ecs_vars_t vars = {0};
    ecs_vars_init(world, &vars);

    ecs_expr_var_t *v1 = ecs_vars_declare(&vars, "foo", ecs_id(ecs_i32_t));
    test_assert(v1 != NULL);
    ecs_vars_push(&vars);
    ecs_expr_var_t *v2 = ecs_vars_declare(&vars, "foo", ecs_id(ecs_i32_t));
    test_assert(v2 != NULL);
    test_assert(v1 != v2);
    test_assert(ecs_vars_lookup(&vars, "foo") == v2);
    ecs_vars_pop(&vars);
    test_assert(ecs_vars_lookup(&vars, "foo") == v1);
    ecs_vars_fini(&vars);

    ecs_fini(world);
}
