#include <meta.h>

void DeserializeFromExpr_bool() {
    ecs_world_t *world = ecs_init();

    bool value = false;

    const char *ptr = ecs_parse_expr(
        world, NULL, "true", NULL, ecs_id(ecs_bool_t), &value);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');        

    test_bool(value, true);

    ecs_fini(world);
}

void DeserializeFromExpr_byte() {
    ecs_world_t *world = ecs_init();

    ecs_byte_t value = 0;

    const char *ptr = ecs_parse_expr(
        world, NULL, "10", NULL, ecs_id(ecs_byte_t), &value);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');        

    test_int(value, 10);

    ecs_fini(world);
}

void DeserializeFromExpr_char() {
    ecs_world_t *world = ecs_init();

    ecs_char_t value = 0;

    const char *ptr = ecs_parse_expr(
        world, NULL, "10", NULL, ecs_id(ecs_char_t), &value);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');        

    test_int(value, 10);

    ecs_fini(world);
}

void DeserializeFromExpr_char_literal() {
    ecs_world_t *world = ecs_init();

    ecs_char_t value = 0;

    const char *ptr = ecs_parse_expr(
        world, NULL, "\"a\"", NULL, ecs_id(ecs_char_t), &value);
    test_assert(ptr != NULL);

    test_int(value, 'a');

    ecs_fini(world);
}

void DeserializeFromExpr_i8() {
    ecs_world_t *world = ecs_init();

    ecs_i8_t value = 0;

    const char *ptr = ecs_parse_expr(
        world, NULL, "10", NULL, ecs_id(ecs_i8_t), &value);
    test_assert(ptr != NULL);

    test_int(value, 10);

    ecs_fini(world);
}

void DeserializeFromExpr_i16() {
    ecs_world_t *world = ecs_init();

    ecs_i16_t value = 0;

    const char *ptr = ecs_parse_expr(
        world, NULL, "10", NULL, ecs_id(ecs_i16_t), &value);
    test_assert(ptr != NULL);

    test_int(value, 10);

    ecs_fini(world);
}

void DeserializeFromExpr_i32() {
    ecs_world_t *world = ecs_init();

    ecs_i32_t value = 0;

    const char *ptr = ecs_parse_expr(
        world, NULL, "10", NULL, ecs_id(ecs_i32_t), &value);
    test_assert(ptr != NULL);

    test_int(value, 10);

    ecs_fini(world);
}

void DeserializeFromExpr_i64() {
    ecs_world_t *world = ecs_init();

    ecs_i64_t value = 0;

    const char *ptr = ecs_parse_expr(
        world, NULL, "10", NULL, ecs_id(ecs_i64_t), &value);
    test_assert(ptr != NULL);

    test_int(value, 10);

    ecs_fini(world);
}

void DeserializeFromExpr_iptr() {
    ecs_world_t *world = ecs_init();

    ecs_iptr_t value = 0;

    const char *ptr = ecs_parse_expr(
        world, NULL, "10", NULL, ecs_id(ecs_iptr_t), &value);
    test_assert(ptr != NULL);

    test_int(value, 10);

    ecs_fini(world);
}

void DeserializeFromExpr_u8() {
    ecs_world_t *world = ecs_init();

    ecs_u8_t value = 0;

    const char *ptr = ecs_parse_expr(
        world, NULL, "10", NULL, ecs_id(ecs_u8_t), &value);
    test_assert(ptr != NULL);

    test_int(value, 10);

    ecs_fini(world);
}

void DeserializeFromExpr_u16() {
    ecs_world_t *world = ecs_init();

    ecs_u16_t value = 0;

    const char *ptr = ecs_parse_expr(
        world, NULL, "10", NULL, ecs_id(ecs_u16_t), &value);
    test_assert(ptr != NULL);

    test_int(value, 10);

    ecs_fini(world);
}

void DeserializeFromExpr_u32() {
    ecs_world_t *world = ecs_init();

    ecs_u32_t value = 0;

    const char *ptr = ecs_parse_expr(
        world, NULL, "10", NULL, ecs_id(ecs_u32_t), &value);
    test_assert(ptr != NULL);

    test_int(value, 10);

    ecs_fini(world);
}

void DeserializeFromExpr_u64() {
    ecs_world_t *world = ecs_init();

    ecs_u64_t value = 0;

    const char *ptr = ecs_parse_expr(
        world, NULL, "10", NULL, ecs_id(ecs_u64_t), &value);
    test_assert(ptr != NULL);

    test_int(value, 10);

    ecs_fini(world);
}

void DeserializeFromExpr_uptr() {
    ecs_world_t *world = ecs_init();

    ecs_uptr_t value = 0;

    const char *ptr = ecs_parse_expr(
        world, NULL, "10", NULL, ecs_id(ecs_uptr_t), &value);
    test_assert(ptr != NULL);

    test_int(value, 10);

    ecs_fini(world);
}

void DeserializeFromExpr_float() {
    ecs_world_t *world = ecs_init();

    ecs_f32_t value = 0;

    const char *ptr = ecs_parse_expr(
        world, NULL, "10.5", NULL, ecs_id(ecs_f32_t), &value);
    test_assert(ptr != NULL);

    test_flt(value, 10.5);

    ecs_fini(world);
}

void DeserializeFromExpr_double() {
    ecs_world_t *world = ecs_init();

    ecs_f64_t value = 0;

    const char *ptr = ecs_parse_expr(
        world, NULL, "10.5", NULL, ecs_id(ecs_f64_t), &value);
    test_assert(ptr != NULL);

    test_flt(value, 10.5);

    ecs_fini(world);
}

void DeserializeFromExpr_negative_int() {
    ecs_world_t *world = ecs_init();

    ecs_i8_t value = 0;

    const char *ptr = ecs_parse_expr(
        world, NULL, "-10", NULL, ecs_id(ecs_i8_t), &value);
    test_assert(ptr != NULL);

    test_int(value, -10);

    ecs_fini(world);
}

void DeserializeFromExpr_negative_float() {
    ecs_world_t *world = ecs_init();

    ecs_f32_t value = 0;

    const char *ptr = ecs_parse_expr(
        world, NULL, "-10.5", NULL, ecs_id(ecs_f32_t), &value);
    test_assert(ptr != NULL);

    test_int(value, -10.5);

    ecs_fini(world);
}

void DeserializeFromExpr_string() {
    ecs_world_t *world = ecs_init();

    ecs_string_t value = 0;

    const char *ptr = ecs_parse_expr(
        world, NULL, "\"Hello World\"", NULL, ecs_id(ecs_string_t), &value);
    test_assert(ptr != NULL);

    test_str(value, "Hello World");

    ecs_os_free(value);

    ecs_fini(world);
}

void DeserializeFromExpr_struct_i32() {
    typedef struct {
        int32_t x;
    } T;
    
    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t) {
        .entity.name = "T",
        .members = {
            {"x", ecs_id(ecs_i32_t)}
        }
    });

    T value = {0};

    const char *ptr = ecs_parse_expr(world, NULL, "{10}", NULL, t, &value);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_int(value.x, 10);

    ecs_fini(world);
}

void DeserializeFromExpr_struct_i32_i32() {
    typedef struct {
        int32_t x;
        int32_t y;
    } T;
    
    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t) {
        .entity.name = "T",
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    T value = {0, 0};

    const char *ptr = ecs_parse_expr(world, NULL, "{10, 20}", NULL, t, &value);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_int(value.x, 10);
    test_int(value.y, 20);

    ecs_fini(world);
}

void DeserializeFromExpr_struct_entity() {
    typedef struct {
        ecs_entity_t entity;
    } T;
    
    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t) {
        .entity.name = "T",
        .members = {
            {"entity", ecs_id(ecs_entity_t)}
        }
    });

    T value = {0};

    const char *ptr = ecs_parse_expr(
        world, NULL, "{flecs.core}", NULL, t, &value);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_int(value.entity, EcsFlecsCore);

    ecs_fini(world);
}

void DeserializeFromExpr_struct_nested_i32() {
    typedef struct {
        ecs_i32_t x;
    } N1;

    typedef struct {
        N1 n_1;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t n1 = ecs_struct_init(world, &(ecs_struct_desc_t) {
        .entity.name = "N1",
        .members = {
            {"x", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t) {
        .entity.name = "T",
        .members = {
            {"n_1", n1},
        }
    });

    T value = {{0}};

    const char *ptr = ecs_parse_expr(world, NULL, "{{10}}", NULL, t, &value);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_int(value.n_1.x, 10);

    ecs_fini(world);
}

void DeserializeFromExpr_struct_nested_i32_i32() {
    typedef struct {
        ecs_i32_t x;
        ecs_i32_t y;
    } N1;

    typedef struct {
        N1 n_1;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t n1 = ecs_struct_init(world, &(ecs_struct_desc_t) {
        .entity.name = "N1",
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t) {
        .entity.name = "T",
        .members = {
            {"n_1", n1},
        }
    });

    T value = {{0}};

    const char *ptr = ecs_parse_expr(world, NULL, "{{10, 20}}", NULL, t, &value);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_int(value.n_1.x, 10);
    test_int(value.n_1.y, 20);

    ecs_fini(world);
}

void DeserializeFromExpr_struct_2_nested_i32_i32() {
    typedef struct {
        ecs_i32_t x;
        ecs_i32_t y;
    } N1;

    typedef struct {
        N1 n_1;
        N1 n_2;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t n1 = ecs_struct_init(world, &(ecs_struct_desc_t) {
        .entity.name = "N1",
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t) {
        .entity.name = "T",
        .members = {
            {"n_1", n1},
            {"n_2", n1},
        }
    });

    T value = {{0}};

    const char *ptr = ecs_parse_expr(
        world, NULL, "{{10, 20}, {30, 40}}", NULL, t, &value);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_int(value.n_1.x, 10);
    test_int(value.n_1.y, 20);
    test_int(value.n_2.x, 30);
    test_int(value.n_2.y, 40);

    ecs_fini(world);
}

void DeserializeFromExpr_struct_member_i32() {
    typedef struct {
        int32_t x;
    } T;
    
    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t) {
        .entity.name = "T",
        .members = {
            {"x", ecs_id(ecs_i32_t)}
        }
    });

    T value = {0};

    const char *ptr = ecs_parse_expr(world, NULL, "{x = 10}", NULL, t, &value);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_int(value.x, 10);

    ecs_fini(world);
}

void DeserializeFromExpr_struct_member_i32_i32() {
    typedef struct {
        int32_t x;
        int32_t y;
    } T;
    
    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t) {
        .entity.name = "T",
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    T value = {0};

    const char *ptr = ecs_parse_expr(world, NULL, "{x = 10, y = 20}", NULL, t, &value);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_int(value.x, 10);
    test_int(value.y, 20);

    ecs_fini(world);
}

void DeserializeFromExpr_struct_member_nested_i32() {
    typedef struct {
        ecs_i32_t x;
    } N1;

    typedef struct {
        N1 n_1;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t n1 = ecs_struct_init(world, &(ecs_struct_desc_t) {
        .entity.name = "N1",
        .members = {
            {"x", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t) {
        .entity.name = "T",
        .members = {
            {"n_1", n1},
        }
    });

    T value = {{0}};

    const char *ptr = ecs_parse_expr(world, NULL, "{n_1 = {x = 10}}", NULL, t, &value);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_int(value.n_1.x, 10);

    ecs_fini(world);
}

void DeserializeFromExpr_struct_member_nested_i32_i32() {
    typedef struct {
        ecs_i32_t x;
        ecs_i32_t y;
    } N1;

    typedef struct {
        N1 n_1;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t n1 = ecs_struct_init(world, &(ecs_struct_desc_t) {
        .entity.name = "N1",
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t) {
        .entity.name = "T",
        .members = {
            {"n_1", n1},
        }
    });

    T value = {{0}};

    const char *ptr = ecs_parse_expr(world, NULL, "{n_1 = {x = 10, y = 20}}", NULL, t, &value);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_int(value.n_1.x, 10);
    test_int(value.n_1.y, 20);

    ecs_fini(world);
}

void DeserializeFromExpr_struct_member_2_nested_i32_i32() {
    typedef struct {
        ecs_i32_t x;
        ecs_i32_t y;
    } N1;

    typedef struct {
        N1 n_1;
        N1 n_2;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t n1 = ecs_struct_init(world, &(ecs_struct_desc_t) {
        .entity.name = "N1",
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t) {
        .entity.name = "T",
        .members = {
            {"n_1", n1},
            {"n_2", n1},
        }
    });

    T value = {{0}};

    const char *expr = 
    HEAD "{"
    LINE " n_1 = {x = 10, y = 20},"
    LINE " n_2 = {x = 30, y = 40}"
    LINE "}";

    const char *ptr = ecs_parse_expr(world, NULL, expr, NULL, t, &value);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_int(value.n_1.x, 10);
    test_int(value.n_1.y, 20);
    test_int(value.n_2.x, 30);
    test_int(value.n_2.y, 40);

    ecs_fini(world);
}

void DeserializeFromExpr_struct_member_2_nested_i32_i32_reverse() {
    typedef struct {
        ecs_i32_t x;
        ecs_i32_t y;
    } N1;

    typedef struct {
        N1 n_1;
        N1 n_2;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t n1 = ecs_struct_init(world, &(ecs_struct_desc_t) {
        .entity.name = "N1",
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t) {
        .entity.name = "T",
        .members = {
            {"n_1", n1},
            {"n_2", n1},
        }
    });

    T value = {{0}};

    const char *expr = 
    HEAD "{"
    LINE " n_2 = {x = 30, y = 40},"
    LINE " n_1 = {x = 10, y = 20}"
    LINE "}";

    const char *ptr = ecs_parse_expr(world, NULL, expr, NULL, t, &value);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_int(value.n_1.x, 10);
    test_int(value.n_1.y, 20);
    test_int(value.n_2.x, 30);
    test_int(value.n_2.y, 40);

    ecs_fini(world);
}

void DeserializeFromExpr_struct_i32_array_3() {
    typedef struct {
        int32_t x[3];
    } T;
    
    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t) {
        .entity.name = "T",
        .members = {
            {"x", ecs_id(ecs_i32_t), 3}
        }
    });

    T value = {0};

    const char *ptr = ecs_parse_expr(world, NULL, "{x = [10, 20, 30]}", NULL, t, &value);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_int(value.x[0], 10);
    test_int(value.x[1], 20);
    test_int(value.x[2], 30);

    ecs_fini(world);
}

void DeserializeFromExpr_struct_struct_i32_array_3() {
    typedef struct {
        ecs_i32_t x;
    } N1;

    typedef struct {
        N1 n_1[3];
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t n1 = ecs_struct_init(world, &(ecs_struct_desc_t) {
        .entity.name = "N1",
        .members = {
            {"x", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t) {
        .entity.name = "T",
        .members = {
            {"n_1", n1, 3}
        }
    });

    T value = {{{0}}};

    const char *ptr = ecs_parse_expr(world, NULL, "{n_1 = [{x = 10}, {x = 20}, {x = 30}]}", NULL, t, &value);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_int(value.n_1[0].x, 10);
    test_int(value.n_1[1].x, 20);
    test_int(value.n_1[2].x, 30);

    ecs_fini(world);
}

void DeserializeFromExpr_struct_struct_i32_i32_array_3() {
    typedef struct {
        ecs_i32_t x;
        ecs_i32_t y;
    } N1;

    typedef struct {
        N1 n_1[3];
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t n1 = ecs_struct_init(world, &(ecs_struct_desc_t) {
        .entity.name = "N1",
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t) {
        .entity.name = "T",
        .members = {
            {"n_1", n1, 3}
        }
    });

    T value = {{{0}}};

    const char *ptr = ecs_parse_expr(world, NULL, "{n_1 = [{x = 10, y = 20}, {x = 30, y = 40}, {x = 50, y = 60}]}", NULL, t, &value);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_int(value.n_1[0].x, 10);
    test_int(value.n_1[0].y, 20);
    test_int(value.n_1[1].x, 30);
    test_int(value.n_1[1].y, 40);
    test_int(value.n_1[2].x, 50);
    test_int(value.n_1[2].y, 60);

    ecs_fini(world);
}

void DeserializeFromExpr_entity() {
    // Implement testcase
}
