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
    test_str(expr, "0");
    ecs_os_free(expr);
    }

    {
    ecs_byte_t value = 10;
    char *expr = ecs_ptr_to_expr(world, ecs_id(ecs_byte_t), &value);
    test_assert(expr != NULL);
    test_str(expr, "10");
    ecs_os_free(expr);
    }

    {
    ecs_byte_t value = 11;
    char *expr = ecs_ptr_to_expr(world, ecs_id(ecs_byte_t), &value);
    test_assert(expr != NULL);
    test_str(expr, "11");
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

void SerializeToExpr_entity() {
    ecs_world_t *world = ecs_init();

    {
    ecs_entity_t value = 0;
    char *expr = ecs_ptr_to_expr(world, ecs_id(ecs_entity_t), &value);
    test_assert(expr != NULL);
    test_str(expr, "0");
    ecs_os_free(expr);
    }

    {
    ecs_entity_t value = EcsFlecsCore;
    char *expr = ecs_ptr_to_expr(world, ecs_id(ecs_entity_t), &value);
    test_assert(expr != NULL);
    test_str(expr, "flecs.core");
    ecs_os_free(expr);
    }

    ecs_fini(world);
}

void SerializeToExpr_enum() {
    typedef enum {
        Red, Blue, Green
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_enum_init(world, &(ecs_enum_desc_t) {
        .constants = {
            {"Red"}, {"Blue"}, {"Green"}
        }
    });

    test_assert(t != 0);

    {
    T value = Red;
    char *expr = ecs_ptr_to_expr(world, t, &value);
    test_assert(expr != NULL);
    test_str(expr, "Red");
    ecs_os_free(expr);
    }

    {
    T value = Blue;
    char *expr = ecs_ptr_to_expr(world, t, &value);
    test_assert(expr != NULL);
    test_str(expr, "Blue");
    ecs_os_free(expr);
    }

    {
    T value = Green;
    char *expr = ecs_ptr_to_expr(world, t, &value);
    test_assert(expr != NULL);
    test_str(expr, "Green");
    ecs_os_free(expr);
    }

    {
    T value = 10;
    char *expr = ecs_ptr_to_expr(world, t, &value);
    test_assert(expr == NULL);
    }

    ecs_fini(world);
}

void SerializeToExpr_bitmask() {
    uint32_t Lettuce = 0x1;
    uint32_t Bacon =   0x1 << 1;
    uint32_t Tomato =  0x1 << 2;
    uint32_t Cheese =  0x1 << 3;

    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_bitmask_init(world, &(ecs_bitmask_desc_t) {
        .constants = {
            {"Lettuce"}, {"Bacon"}, {"Tomato"}, {"Cheese"}, {"BLT", Bacon | Lettuce | Tomato}
        }
    });

    test_assert(t != 0);

    {
    uint32_t value = 0;
    char *expr = ecs_ptr_to_expr(world, t, &value);
    test_assert(expr != NULL);
    test_str(expr, "0");
    ecs_os_free(expr);
    }

    {
    uint32_t value = Lettuce;
    char *expr = ecs_ptr_to_expr(world, t, &value);
    test_assert(expr != NULL);
    test_str(expr, "Lettuce");
    ecs_os_free(expr);
    }

    {
    uint32_t value = Bacon;
    char *expr = ecs_ptr_to_expr(world, t, &value);
    test_assert(expr != NULL);
    test_str(expr, "Bacon");
    ecs_os_free(expr);
    }

    {
    uint32_t value = Lettuce | Bacon;
    char *expr = ecs_ptr_to_expr(world, t, &value);
    test_assert(expr != NULL);
    test_str(expr, "Lettuce|Bacon");
    ecs_os_free(expr);
    }

    {
    uint32_t value = Lettuce | Bacon | Tomato | Cheese;
    char *expr = ecs_ptr_to_expr(world, t, &value);
    test_assert(expr != NULL);
    test_str(expr, "Cheese|Lettuce|Bacon|Tomato");
    ecs_os_free(expr);
    }

    {
    uint32_t value = 16;
    char *expr = ecs_ptr_to_expr(world, t, &value);
    test_assert(expr == NULL);
    }

    ecs_fini(world);
}

void SerializeToExpr_struct_enum() {
    typedef enum {
        Red, Blue, Green
    } E;

    typedef struct {
        E x;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t e = ecs_enum_init(world, &(ecs_enum_desc_t) {
        .constants = {
            {"Red"}, {"Blue"}, {"Green"}
        }
    });

    test_assert(e != 0);

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t) {
        .entity.name = "T",
        .members = {
            {"x", e}
        }
    });

    test_assert(t != 0);

    {
    T value = {Red};
    char *expr = ecs_ptr_to_expr(world, t, &value);
    test_assert(expr != NULL);
    test_str(expr, "{x: Red}");
    ecs_os_free(expr);
    }

    {
    T value = {Blue};
    char *expr = ecs_ptr_to_expr(world, t, &value);
    test_assert(expr != NULL);
    test_str(expr, "{x: Blue}");
    ecs_os_free(expr);
    }

    {
    T value = {Green};
    char *expr = ecs_ptr_to_expr(world, t, &value);
    test_assert(expr != NULL);
    test_str(expr, "{x: Green}");
    ecs_os_free(expr);
    }

    {
    T value = {10};
    char *expr = ecs_ptr_to_expr(world, t, &value);
    test_assert(expr == NULL);
    }

    ecs_fini(world);
}

void SerializeToExpr_struct_bitmask() {
    typedef struct {
        uint32_t x;
    } T;

    uint32_t Lettuce = 0x1;
    uint32_t Bacon =   0x1 << 1;
    uint32_t Tomato =  0x1 << 2;
    uint32_t Cheese =  0x1 << 3;

    ecs_world_t *world = ecs_init();

    ecs_entity_t b = ecs_bitmask_init(world, &(ecs_bitmask_desc_t) {
        .constants = {
            {"Lettuce"}, {"Bacon"}, {"Tomato"}, {"Cheese"}, {"BLT", Bacon | Lettuce | Tomato}
        }
    });

    test_assert(b != 0);

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t) {
        .entity.name = "T",
        .members = {
            {"x", b}
        }
    });

    test_assert(t != 0);

    {
    T value = {0};
    char *expr = ecs_ptr_to_expr(world, t, &value);
    test_assert(expr != NULL);
    test_str(expr, "{x: 0}");
    ecs_os_free(expr);
    }

    {
    T value = {Lettuce};
    char *expr = ecs_ptr_to_expr(world, t, &value);
    test_assert(expr != NULL);
    test_str(expr, "{x: Lettuce}");
    ecs_os_free(expr);
    }

    {
    T value = {Bacon};
    char *expr = ecs_ptr_to_expr(world, t, &value);
    test_assert(expr != NULL);
    test_str(expr, "{x: Bacon}");
    ecs_os_free(expr);
    }

    {
    T value = {Lettuce | Bacon};
    char *expr = ecs_ptr_to_expr(world, t, &value);
    test_assert(expr != NULL);
    test_str(expr, "{x: Lettuce|Bacon}");
    ecs_os_free(expr);
    }

    {
    T value = {Lettuce | Bacon | Tomato | Cheese};
    char *expr = ecs_ptr_to_expr(world, t, &value);
    test_assert(expr != NULL);
    test_str(expr, "{x: Cheese|Lettuce|Bacon|Tomato}");
    ecs_os_free(expr);
    }

    {
    T value = {16};
    char *expr = ecs_ptr_to_expr(world, t, &value);
    test_assert(expr == NULL);
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
    test_str(expr, "{x: 10}");
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
    test_str(expr, "{x: 10, y: 20}");
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
    test_str(expr, "{e: flecs.core}");
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
    test_str(expr, "{n_1: {x: 10}}");
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
    test_str(expr, "{n_1: {x: 10, y: 20}}");
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
    test_str(expr, "{n_1: {x: 10, y: 20}, n_2: {x: 30, y: 40}}");
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
    test_str(expr, "{x: [10, 20, 30]}");
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
    test_str(expr, "{n_1: [{x: 10}, {x: 20}, {x: 30}]}");
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
    test_str(expr, "{n_1: [{x: 10, y: 20}, {x: 30, y: 40}, {x: 50, y: 60}]}");
    ecs_os_free(expr);

    ecs_fini(world);
}
