#include <meta.h>

void SerializeToExpr_bool() {
    ecs_world_t *world = ecs_init();

    {
    bool value = true;
    char *expr = ecs_ptr_to_expr(world, ecs_id(ecs_bool_t), &value);
    test_assert(expr != NULL);
    test_str(expr, "true");
    ecs_os_free(expr);
    }

    {
    bool value = false;
    char *expr = ecs_ptr_to_expr(world, ecs_id(ecs_bool_t), &value);
    test_assert(expr != NULL);
    test_str(expr, "false");
    ecs_os_free(expr);
    }

    ecs_fini(world);
}

void SerializeToExpr_byte() {
    ecs_world_t *world = ecs_init();

    {
    ecs_byte_t value = 0;
    char *expr = ecs_ptr_to_expr(world, ecs_id(ecs_byte_t), &value);
    test_assert(expr != NULL);
    test_str(expr, "0x0");
    ecs_os_free(expr);
    }

    {
    ecs_byte_t value = 10;
    char *expr = ecs_ptr_to_expr(world, ecs_id(ecs_byte_t), &value);
    test_assert(expr != NULL);
    test_str(expr, "0xa");
    ecs_os_free(expr);
    }

    {
    ecs_byte_t value = 11;
    char *expr = ecs_ptr_to_expr(world, ecs_id(ecs_byte_t), &value);
    test_assert(expr != NULL);
    test_str(expr, "0xb");
    ecs_os_free(expr);
    }

    ecs_fini(world);
}

void SerializeToExpr_char() {
    ecs_world_t *world = ecs_init();

    {
    ecs_char_t value = 0;
    char *expr = ecs_ptr_to_expr(world, ecs_id(ecs_char_t), &value);
    test_assert(expr != NULL);
    test_str(expr, "0");
    ecs_os_free(expr);
    }

    {
    ecs_char_t value = 'a';
    char *expr = ecs_ptr_to_expr(world, ecs_id(ecs_char_t), &value);
    test_assert(expr != NULL);
    test_str(expr, "\"a\"");
    ecs_os_free(expr);
    }

    {
    ecs_char_t value = '"';
    char *expr = ecs_ptr_to_expr(world, ecs_id(ecs_char_t), &value);
    test_assert(expr != NULL);
    test_str(expr, "\"\\\"\"");
    ecs_os_free(expr);
    }

    {
    ecs_char_t value = '\'';
    char *expr = ecs_ptr_to_expr(world, ecs_id(ecs_char_t), &value);
    test_assert(expr != NULL);
    test_str(expr, "\"'\"");
    ecs_os_free(expr);
    }

    ecs_fini(world);
}

void SerializeToExpr_i8() {
    ecs_world_t *world = ecs_init();

    {
    ecs_i8_t value = 0;
    char *expr = ecs_ptr_to_expr(world, ecs_id(ecs_i8_t), &value);
    test_assert(expr != NULL);
    test_str(expr, "0");
    ecs_os_free(expr);
    }

    {
    ecs_i8_t value = 10;
    char *expr = ecs_ptr_to_expr(world, ecs_id(ecs_i8_t), &value);
    test_assert(expr != NULL);
    test_str(expr, "10");
    ecs_os_free(expr);
    }

    {
    ecs_i8_t value = -10;
    char *expr = ecs_ptr_to_expr(world, ecs_id(ecs_i8_t), &value);
    test_assert(expr != NULL);
    test_str(expr, "-10");
    ecs_os_free(expr);
    }

    ecs_fini(world);
}

void SerializeToExpr_i16() {
    ecs_world_t *world = ecs_init();

    {
    ecs_i16_t value = 0;
    char *expr = ecs_ptr_to_expr(world, ecs_id(ecs_i16_t), &value);
    test_assert(expr != NULL);
    test_str(expr, "0");
    ecs_os_free(expr);
    }

    {
    ecs_i16_t value = 10;
    char *expr = ecs_ptr_to_expr(world, ecs_id(ecs_i16_t), &value);
    test_assert(expr != NULL);
    test_str(expr, "10");
    ecs_os_free(expr);
    }

    {
    ecs_i16_t value = -10;
    char *expr = ecs_ptr_to_expr(world, ecs_id(ecs_i16_t), &value);
    test_assert(expr != NULL);
    test_str(expr, "-10");
    ecs_os_free(expr);
    }

    ecs_fini(world);
}

void SerializeToExpr_i32() {
    ecs_world_t *world = ecs_init();

    {
    ecs_i32_t value = 0;
    char *expr = ecs_ptr_to_expr(world, ecs_id(ecs_i32_t), &value);
    test_assert(expr != NULL);
    test_str(expr, "0");
    ecs_os_free(expr);
    }

    {
    ecs_i32_t value = 10;
    char *expr = ecs_ptr_to_expr(world, ecs_id(ecs_i32_t), &value);
    test_assert(expr != NULL);
    test_str(expr, "10");
    ecs_os_free(expr);
    }

    {
    ecs_i32_t value = -10;
    char *expr = ecs_ptr_to_expr(world, ecs_id(ecs_i32_t), &value);
    test_assert(expr != NULL);
    test_str(expr, "-10");
    ecs_os_free(expr);
    }

    ecs_fini(world);
}

void SerializeToExpr_i64() {
    ecs_world_t *world = ecs_init();

    {
    ecs_i64_t value = 0;
    char *expr = ecs_ptr_to_expr(world, ecs_id(ecs_i64_t), &value);
    test_assert(expr != NULL);
    test_str(expr, "0");
    ecs_os_free(expr);
    }

    {
    ecs_i64_t value = 10;
    char *expr = ecs_ptr_to_expr(world, ecs_id(ecs_i64_t), &value);
    test_assert(expr != NULL);
    test_str(expr, "10");
    ecs_os_free(expr);
    }

    {
    ecs_i64_t value = -10;
    char *expr = ecs_ptr_to_expr(world, ecs_id(ecs_i64_t), &value);
    test_assert(expr != NULL);
    test_str(expr, "-10");
    ecs_os_free(expr);
    }

    ecs_fini(world);
}

void SerializeToExpr_iptr() {
    ecs_world_t *world = ecs_init();

    {
    ecs_iptr_t value = 0;
    char *expr = ecs_ptr_to_expr(world, ecs_id(ecs_iptr_t), &value);
    test_assert(expr != NULL);
    test_str(expr, "0");
    ecs_os_free(expr);
    }

    {
    ecs_iptr_t value = 10;
    char *expr = ecs_ptr_to_expr(world, ecs_id(ecs_iptr_t), &value);
    test_assert(expr != NULL);
    test_str(expr, "10");
    ecs_os_free(expr);
    }

    {
    ecs_iptr_t value = -10;
    char *expr = ecs_ptr_to_expr(world, ecs_id(ecs_iptr_t), &value);
    test_assert(expr != NULL);
    test_str(expr, "-10");
    ecs_os_free(expr);
    }

    ecs_fini(world);
}

void SerializeToExpr_u8() {
    ecs_world_t *world = ecs_init();

    {
    ecs_u8_t value = 0;
    char *expr = ecs_ptr_to_expr(world, ecs_id(ecs_u8_t), &value);
    test_assert(expr != NULL);
    test_str(expr, "0");
    ecs_os_free(expr);
    }

    {
    ecs_u8_t value = 10;
    char *expr = ecs_ptr_to_expr(world, ecs_id(ecs_u8_t), &value);
    test_assert(expr != NULL);
    test_str(expr, "10");
    ecs_os_free(expr);
    }

    ecs_fini(world);
}

void SerializeToExpr_u16() {
    ecs_world_t *world = ecs_init();

    {
    ecs_u16_t value = 0;
    char *expr = ecs_ptr_to_expr(world, ecs_id(ecs_u16_t), &value);
    test_assert(expr != NULL);
    test_str(expr, "0");
    ecs_os_free(expr);
    }

    {
    ecs_u16_t value = 10;
    char *expr = ecs_ptr_to_expr(world, ecs_id(ecs_u16_t), &value);
    test_assert(expr != NULL);
    test_str(expr, "10");
    ecs_os_free(expr);
    }

    ecs_fini(world);
}

void SerializeToExpr_u32() {
    ecs_world_t *world = ecs_init();

    {
    ecs_u32_t value = 0;
    char *expr = ecs_ptr_to_expr(world, ecs_id(ecs_u32_t), &value);
    test_assert(expr != NULL);
    test_str(expr, "0");
    ecs_os_free(expr);
    }

    {
    ecs_u32_t value = 10;
    char *expr = ecs_ptr_to_expr(world, ecs_id(ecs_u32_t), &value);
    test_assert(expr != NULL);
    test_str(expr, "10");
    ecs_os_free(expr);
    }

    ecs_fini(world);
}

void SerializeToExpr_u64() {
    ecs_world_t *world = ecs_init();

    {
    ecs_u64_t value = 0;
    char *expr = ecs_ptr_to_expr(world, ecs_id(ecs_u64_t), &value);
    test_assert(expr != NULL);
    test_str(expr, "0");
    ecs_os_free(expr);
    }

    {
    ecs_u64_t value = 10;
    char *expr = ecs_ptr_to_expr(world, ecs_id(ecs_u64_t), &value);
    test_assert(expr != NULL);
    test_str(expr, "10");
    ecs_os_free(expr);
    }

    ecs_fini(world);
}

void SerializeToExpr_uptr() {
    ecs_world_t *world = ecs_init();

    {
    ecs_uptr_t value = 0;
    char *expr = ecs_ptr_to_expr(world, ecs_id(ecs_uptr_t), &value);
    test_assert(expr != NULL);
    test_str(expr, "0");
    ecs_os_free(expr);
    }

    {
    ecs_uptr_t value = 10;
    char *expr = ecs_ptr_to_expr(world, ecs_id(ecs_uptr_t), &value);
    test_assert(expr != NULL);
    test_str(expr, "10");
    ecs_os_free(expr);
    }

    ecs_fini(world);
}

void SerializeToExpr_float() {
    ecs_world_t *world = ecs_init();

    {
    ecs_f32_t value = 0;
    char *expr = ecs_ptr_to_expr(world, ecs_id(ecs_f32_t), &value);
    test_assert(expr != NULL);
    test_str(expr, "0.000000");
    ecs_os_free(expr);
    }

    {
    ecs_f32_t value = 10.5;
    char *expr = ecs_ptr_to_expr(world, ecs_id(ecs_f32_t), &value);
    test_assert(expr != NULL);
    test_str(expr, "10.500000");
    ecs_os_free(expr);
    }

    {
    ecs_f32_t value = -10.5;
    char *expr = ecs_ptr_to_expr(world, ecs_id(ecs_f32_t), &value);
    test_assert(expr != NULL);
    test_str(expr, "-10.500000");
    ecs_os_free(expr);
    }

    ecs_fini(world);
}

void SerializeToExpr_double() {
    ecs_world_t *world = ecs_init();

    {
    ecs_f64_t value = 0;
    char *expr = ecs_ptr_to_expr(world, ecs_id(ecs_f64_t), &value);
    test_assert(expr != NULL);
    test_str(expr, "0.000000");
    ecs_os_free(expr);
    }

    {
    ecs_f64_t value = 10.5;
    char *expr = ecs_ptr_to_expr(world, ecs_id(ecs_f64_t), &value);
    test_assert(expr != NULL);
    test_str(expr, "10.500000");
    ecs_os_free(expr);
    }

    {
    ecs_f64_t value = -10.5;
    char *expr = ecs_ptr_to_expr(world, ecs_id(ecs_f64_t), &value);
    test_assert(expr != NULL);
    test_str(expr, "-10.500000");
    ecs_os_free(expr);
    }

    ecs_fini(world);
}

void SerializeToExpr_string() {
    ecs_world_t *world = ecs_init();

    {
    const char* value = 0;
    char *expr = ecs_ptr_to_expr(world, ecs_id(ecs_string_t), &value);
    test_assert(expr != NULL);
    test_str(expr, "null");
    ecs_os_free(expr);
    }

    {
    const char *value = "Hello World";
    char *expr = ecs_ptr_to_expr(world, ecs_id(ecs_string_t), &value);
    test_assert(expr != NULL);
    test_str(expr, "\"Hello World\"");
    ecs_os_free(expr);
    }

    {
    const char *value = "Hello \\World";
    char *expr = ecs_ptr_to_expr(world, ecs_id(ecs_string_t), &value);
    test_assert(expr != NULL);
    test_str(expr, "\"Hello \\\\World\"");
    ecs_os_free(expr);
    }

    {
    const char *value = "Hello \"World\"";
    char *expr = ecs_ptr_to_expr(world, ecs_id(ecs_string_t), &value);
    test_assert(expr != NULL);
    test_str(expr, "\"Hello \\\"World\\\"\"");
    ecs_os_free(expr);
    }

    ecs_fini(world);
}

void SerializeToExpr_struct_i32() {
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

    T value = {10};
    char *expr = ecs_ptr_to_expr(world, t, &value);
    test_assert(expr != NULL);
    test_str(expr, "{x = 10}");
    ecs_os_free(expr);

    ecs_fini(world);
}

void SerializeToExpr_struct_i32_i32() {
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

    T value = {10, 20};
    char *expr = ecs_ptr_to_expr(world, t, &value);
    test_assert(expr != NULL);
    test_str(expr, "{x = 10, y = 20}");
    ecs_os_free(expr);

    ecs_fini(world);
}

void SerializeToExpr_struct_entity() {
    typedef struct {
        ecs_entity_t e;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t) {
        .entity.name = "T",
        .members = {
            {"e", ecs_id(ecs_entity_t)}
        }
    });

    T value = {EcsFlecsCore};
    char *expr = ecs_ptr_to_expr(world, t, &value);
    test_assert(expr != NULL);
    test_str(expr, "{e = core}");
    ecs_os_free(expr);

    ecs_fini(world);
}

void SerializeToExpr_struct_nested_i32() {
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

    T value = {{10}};
    char *expr = ecs_ptr_to_expr(world, t, &value);
    test_assert(expr != NULL);
    test_str(expr, "{n_1 = {x = 10}}");
    ecs_os_free(expr);

    ecs_fini(world);
}

void SerializeToExpr_struct_nested_i32_i32() {
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

    T value = {{10, 20}};
    char *expr = ecs_ptr_to_expr(world, t, &value);
    test_assert(expr != NULL);
    test_str(expr, "{n_1 = {x = 10, y = 20}}");
    ecs_os_free(expr);

    ecs_fini(world);
}

void SerializeToExpr_struct_2_nested_i32_i32() {
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
            {"n_2", n1}
        }
    });

    T value = {{10, 20}, {30, 40}};
    char *expr = ecs_ptr_to_expr(world, t, &value);
    test_assert(expr != NULL);
    test_str(expr, "{n_1 = {x = 10, y = 20}, n_2 = {x = 30, y = 40}}");
    ecs_os_free(expr);

    ecs_fini(world);
}

void SerializeToExpr_struct_i32_array_3() {
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

    T value = {{10, 20, 30}};
    char *expr = ecs_ptr_to_expr(world, t, &value);
    test_assert(expr != NULL);
    test_str(expr, "{x = [10, 20, 30]}");
    ecs_os_free(expr);

    ecs_fini(world);
}

void SerializeToExpr_struct_struct_i32_array_3() {
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

    T value = {{ {10}, {20}, {30} }};
    char *expr = ecs_ptr_to_expr(world, t, &value);
    test_assert(expr != NULL);
    test_str(expr, "{n_1 = [{x = 10}, {x = 20}, {x = 30}]}");
    ecs_os_free(expr);

    ecs_fini(world);
}

void SerializeToExpr_struct_struct_i32_i32_array_3() {
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

    T value = {{ {10, 20}, {30, 40}, {50, 60} }};
    char *expr = ecs_ptr_to_expr(world, t, &value);
    test_assert(expr != NULL);
    test_str(expr, "{n_1 = [{x = 10, y = 20}, {x = 30, y = 40}, {x = 50, y = 60}]}");
    ecs_os_free(expr);

    ecs_fini(world);
}
