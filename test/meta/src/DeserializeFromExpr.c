#include <meta.h>

void DeserializeFromExpr_bool() {
    ecs_world_t *world = ecs_init();

    bool value = false;

    const char *ptr = ecs_parse_expr(world, "true", &ecs_value(ecs_bool_t, &value), NULL);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');        

    test_bool(value, true);

    ecs_fini(world);
}

void DeserializeFromExpr_byte() {
    ecs_world_t *world = ecs_init();

    ecs_byte_t value = 0;

    const char *ptr = ecs_parse_expr(world, "10", &ecs_value(ecs_byte_t, &value), NULL);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');        

    test_int(value, 10);

    ecs_fini(world);
}

void DeserializeFromExpr_char() {
    ecs_world_t *world = ecs_init();

    ecs_char_t value = 0;

    const char *ptr = ecs_parse_expr(world, "10", &ecs_value(ecs_char_t, &value), NULL);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');        

    test_int(value, 10);

    ecs_fini(world);
}

void DeserializeFromExpr_char_literal() {
    ecs_world_t *world = ecs_init();

    ecs_char_t value = 0;

    const char *ptr = ecs_parse_expr(world, "\"a\"", &ecs_value(ecs_char_t, &value), NULL);
    test_assert(ptr != NULL);

    test_int(value, 'a');

    ecs_fini(world);
}

void DeserializeFromExpr_i8() {
    ecs_world_t *world = ecs_init();

    ecs_i8_t value = 0;

    const char *ptr = ecs_parse_expr(world, "10", &ecs_value(ecs_i8_t, &value), NULL);
    test_assert(ptr != NULL);

    test_int(value, 10);

    ecs_fini(world);
}

void DeserializeFromExpr_i16() {
    ecs_world_t *world = ecs_init();

    ecs_i16_t value = 0;

    const char *ptr = ecs_parse_expr(world, "10", &ecs_value(ecs_i16_t, &value), NULL);
    test_assert(ptr != NULL);

    test_int(value, 10);

    ecs_fini(world);
}

void DeserializeFromExpr_i32() {
    ecs_world_t *world = ecs_init();

    ecs_i32_t value = 0;

    const char *ptr = ecs_parse_expr(world, "10", &ecs_value(ecs_i32_t, &value), NULL);
    test_assert(ptr != NULL);

    test_int(value, 10);

    ecs_fini(world);
}

void DeserializeFromExpr_i64() {
    ecs_world_t *world = ecs_init();

    ecs_i64_t value = 0;

    const char *ptr = ecs_parse_expr(world, "10", &ecs_value(ecs_i64_t, &value), NULL);
    test_assert(ptr != NULL);

    test_int(value, 10);

    ecs_fini(world);
}

void DeserializeFromExpr_iptr() {
    ecs_world_t *world = ecs_init();

    ecs_iptr_t value = 0;

    const char *ptr = ecs_parse_expr(world, "10", &ecs_value(ecs_iptr_t, &value), NULL);
    test_assert(ptr != NULL);

    test_int(value, 10);

    ecs_fini(world);
}

void DeserializeFromExpr_u8() {
    ecs_world_t *world = ecs_init();

    ecs_u8_t value = 0;

    const char *ptr = ecs_parse_expr(world, "10", &ecs_value(ecs_u8_t, &value), NULL);
    test_assert(ptr != NULL);

    test_int(value, 10);

    ecs_fini(world);
}

void DeserializeFromExpr_u16() {
    ecs_world_t *world = ecs_init();

    ecs_u16_t value = 0;

    const char *ptr = ecs_parse_expr(world, "10", &ecs_value(ecs_u16_t, &value), NULL);
    test_assert(ptr != NULL);

    test_int(value, 10);

    ecs_fini(world);
}

void DeserializeFromExpr_u32() {
    ecs_world_t *world = ecs_init();

    ecs_u32_t value = 0;

    const char *ptr = ecs_parse_expr(world, "10", &ecs_value(ecs_u32_t, &value), NULL);
    test_assert(ptr != NULL);

    test_int(value, 10);

    ecs_fini(world);
}

void DeserializeFromExpr_u64() {
    ecs_world_t *world = ecs_init();

    ecs_u64_t value = 0;

    const char *ptr = ecs_parse_expr(world, "10", &ecs_value(ecs_u64_t, &value), NULL);
    test_assert(ptr != NULL);

    test_int(value, 10);

    ecs_fini(world);
}

void DeserializeFromExpr_uptr() {
    ecs_world_t *world = ecs_init();

    ecs_uptr_t value = 0;

    const char *ptr = ecs_parse_expr(world, "10", &ecs_value(ecs_uptr_t, &value), NULL);
    test_assert(ptr != NULL);

    test_int(value, 10);

    ecs_fini(world);
}

void DeserializeFromExpr_float() {
    ecs_world_t *world = ecs_init();

    ecs_f32_t value = 0;

    const char *ptr = ecs_parse_expr(world, "10.5", &ecs_value(ecs_f32_t, &value), NULL);
    test_assert(ptr != NULL);

    test_flt(value, 10.5);

    ecs_fini(world);
}

void DeserializeFromExpr_double() {
    ecs_world_t *world = ecs_init();

    ecs_f64_t value = 0;

    const char *ptr = ecs_parse_expr(world, "10.5", &ecs_value(ecs_f64_t, &value), NULL);
    test_assert(ptr != NULL);

    test_flt(value, 10.5);

    ecs_fini(world);
}

void DeserializeFromExpr_negative_int() {
    ecs_world_t *world = ecs_init();

    ecs_i8_t value = 0;

    const char *ptr = ecs_parse_expr(world, "-10", &ecs_value(ecs_i8_t, &value), NULL);
    test_assert(ptr != NULL);

    test_int(value, -10);

    ecs_fini(world);
}

void DeserializeFromExpr_negative_float() {
    ecs_world_t *world = ecs_init();

    ecs_f32_t value = 0;

    const char *ptr = ecs_parse_expr(world, "-10.5", &ecs_value(ecs_f32_t, &value), NULL);
    test_assert(ptr != NULL);

    test_flt(value, -10.5);

    ecs_fini(world);
}

void DeserializeFromExpr_string() {
    ecs_world_t *world = ecs_init();

    ecs_string_t value = 0;

    const char *ptr = ecs_parse_expr(world, "\"Hello World\"", &ecs_value(ecs_string_t, &value), NULL);
    test_assert(ptr != NULL);

    test_str(value, "Hello World");

    ecs_os_free(value);

    ecs_fini(world);
}

void DeserializeFromExpr_entity() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t value = 0;
    const char *ptr = ecs_parse_expr(world, "flecs.core", &ecs_value(ecs_entity_t, &value), NULL);
    test_assert(ptr != NULL);
    test_uint(value, EcsFlecsCore);

    ptr = ecs_parse_expr(world, "0", &ecs_value(ecs_entity_t, &value), NULL);
    test_assert(ptr != NULL);
    test_uint(value, 0);

    ecs_fini(world);
}

void DeserializeFromExpr_enum() {
    typedef enum {
        Red, Blue, Green
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_enum_init(world, &(ecs_enum_desc_t){
        .constants = {
            {"Red"}, {"Blue"}, {"Green"}
        }
    });

    test_assert(t != 0);

    {
    T value = 0;
    const char *ptr = ecs_parse_expr(world, "Red", &(ecs_value_t){t, &value}, NULL);
    test_assert(ptr != NULL);
    test_int(value, Red);
    }

    {
    T value = 0;
    const char *ptr = ecs_parse_expr(world, "Blue", &(ecs_value_t){t, &value}, NULL);
    test_assert(ptr != NULL);
    test_int(value, Blue);
    }

    {
    T value = 0;
    const char *ptr = ecs_parse_expr(world, "Green", &(ecs_value_t){t, &value}, NULL);
    test_assert(ptr != NULL);
    test_int(value, Green);
    }

    {
    ecs_log_set_level(-4);
    T value = 0;
    const char *ptr = ecs_parse_expr(world, "Black", &(ecs_value_t){t, &value}, NULL);
    test_assert(ptr == NULL);
    }

    ecs_fini(world);
}

void DeserializeFromExpr_bitmask() {
    uint32_t Lettuce = 0x1;
    uint32_t Bacon =   0x1 << 1;
    uint32_t Tomato =  0x1 << 2;
    uint32_t Cheese =  0x1 << 3;

    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_bitmask_init(world, &(ecs_bitmask_desc_t){
        .constants = {
            {"Lettuce"}, {"Bacon"}, {"Tomato"}, {"Cheese"}, {"BLT", Bacon | Lettuce | Tomato}
        }
    });

    {
    uint32_t value = 0;
    const char *ptr = ecs_parse_expr(world, "Lettuce", &(ecs_value_t){t, &value}, NULL);
    test_assert(ptr != NULL);
    test_uint(value, Lettuce);
    }

    {
    uint32_t value = 0;
    const char *ptr = ecs_parse_expr(world, "Lettuce|Bacon", &(ecs_value_t){t, &value}, NULL);
    test_assert(ptr != NULL);
    test_uint(value, Lettuce|Bacon);
    }

    {
    uint32_t value = 0;
    const char *ptr = ecs_parse_expr(world, "Lettuce|Bacon|Tomato|Cheese", &(ecs_value_t){t, &value}, NULL);
    test_assert(ptr != NULL);
    test_uint(value, Lettuce|Bacon|Tomato|Cheese);
    }

    {
    uint32_t value = 0;
    const char *ptr = ecs_parse_expr(world, "BLT", &(ecs_value_t){t, &value}, NULL);
    test_assert(ptr != NULL);
    test_uint(value, Lettuce|Bacon|Tomato);
    }

    {
    uint32_t value = 0;
    const char *ptr = ecs_parse_expr(world, "0", &(ecs_value_t){t, &value}, NULL);
    test_assert(ptr != NULL);
    test_uint(value, 0);
    }

    {
    ecs_log_set_level(-4);
    uint32_t value = 0;
    const char *ptr = ecs_parse_expr(world, "Foo", &(ecs_value_t){t, &value}, NULL);
    test_assert(ptr == NULL);
    }

    ecs_fini(world);
}

void DeserializeFromExpr_struct_enum() {
    typedef enum {
        Red, Blue, Green
    } E;

    typedef struct {
        E v;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t e = ecs_enum_init(world, &(ecs_enum_desc_t){
        .constants = {
            {"Red"}, {"Blue"}, {"Green"}
        }
    });

    test_assert(e != 0);

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "T"}),
        .members = {
            {"v", e}
        }
    });

    test_assert(t != 0);

    {
    T value = {0};
    const char *ptr = ecs_parse_expr(world, "{v: Red}", &(ecs_value_t){t, &value}, NULL);
    test_assert(ptr != NULL);
    test_int(value.v, Red);
    }

    {
    T value = {0};
    const char *ptr = ecs_parse_expr(world, "{v: Blue}", &(ecs_value_t){t, &value}, NULL);
    test_assert(ptr != NULL);
    test_int(value.v, Blue);
    }

    {
    T value = {0};
    const char *ptr = ecs_parse_expr(world, "{v: Green}", &(ecs_value_t){t, &value}, NULL);
    test_assert(ptr != NULL);
    test_int(value.v, Green);
    }

    {
    ecs_log_set_level(-4);
    T value = {0};
    const char *ptr = ecs_parse_expr(world, "{v: Black}", &(ecs_value_t){t, &value}, NULL);
    test_assert(ptr == NULL);
    }

    ecs_fini(world);
}

void DeserializeFromExpr_struct_bitmask() {
    uint32_t Lettuce = 0x1;
    uint32_t Bacon =   0x1 << 1;
    uint32_t Tomato =  0x1 << 2;
    uint32_t Cheese =  0x1 << 3;

    typedef struct {
        ecs_u32_t v;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t b = ecs_bitmask_init(world, &(ecs_bitmask_desc_t){
        .constants = {
            {"Lettuce"}, {"Bacon"}, {"Tomato"}, {"Cheese"}, {"BLT", Bacon | Lettuce | Tomato}
        }
    });

    test_assert(b != 0);

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "T"}),
        .members = {
            {"v", b}
        }
    });

    test_assert(t != 0);

    {
    T value = {0};
    const char *ptr = ecs_parse_expr(world, "{v:Lettuce}", &(ecs_value_t){t, &value}, NULL);
    test_assert(ptr != NULL);
    test_uint(value.v, Lettuce);
    }

    {
    T value = {0};
    const char *ptr = ecs_parse_expr(world, "{v:Lettuce|Bacon}", &(ecs_value_t){t, &value}, NULL);
    test_assert(ptr != NULL);
    test_uint(value.v, Lettuce|Bacon);
    }

    {
    T value = {0};
    const char *ptr = ecs_parse_expr(world, "{v:Lettuce|Bacon|Tomato|Cheese}", &(ecs_value_t){t, &value}, NULL);
    test_assert(ptr != NULL);
    test_uint(value.v, Lettuce|Bacon|Tomato|Cheese);
    }

    {
    T value = {0};
    const char *ptr = ecs_parse_expr(world, "{v:BLT}", &(ecs_value_t){t, &value}, NULL);
    test_assert(ptr != NULL);
    test_uint(value.v, Lettuce|Bacon|Tomato);
    }

    {
    T value = {0};
    const char *ptr = ecs_parse_expr(world, "{v:0}", &(ecs_value_t){t, &value}, NULL);
    test_assert(ptr != NULL);
    test_uint(value.v, 0);
    }

    {
    ecs_log_set_level(-4);
    T value = {0};
    const char *ptr = ecs_parse_expr(world, "{v:Foo\"}", &(ecs_value_t){t, &value}, NULL);
    test_assert(ptr == NULL);
    }

    ecs_fini(world);
}

void DeserializeFromExpr_struct_i32() {
    typedef struct {
        int32_t x;
    } T;
    
    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "T"}),
        .members = {
            {"x", ecs_id(ecs_i32_t)}
        }
    });

    T value = {0};

    const char *ptr = ecs_parse_expr(world, "{10}", &(ecs_value_t){t, &value}, NULL);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_int(value.x, 10);

    ecs_fini(world);
}

void DeserializeFromExpr_struct_i32_neg() {
    typedef struct {
        int32_t x;
    } T;
    
    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "T"}),
        .members = {
            {"x", ecs_id(ecs_i32_t)}
        }
    });

    T value = {0};

    const char *ptr = ecs_parse_expr(world, "{-10}", &(ecs_value_t){t, &value}, NULL);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_int(value.x, -10);

    ecs_fini(world);
}

void DeserializeFromExpr_struct_i32_i32() {
    typedef struct {
        int32_t x;
        int32_t y;
    } T;
    
    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "T"}),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    T value = {0, 0};

    const char *ptr = ecs_parse_expr(world, "{10, 20}", &(ecs_value_t){t, &value}, NULL);
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

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "T"}),
        .members = {
            {"entity", ecs_id(ecs_entity_t)}
        }
    });

    T value = {0};

    const char *ptr = ecs_parse_expr(world, "{flecs.core}", &(ecs_value_t){t, &value}, NULL);
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

    ecs_entity_t n1 = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "N1"}),
        .members = {
            {"x", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "T"}),
        .members = {
            {"n_1", n1},
        }
    });

    T value = {{0}};

    const char *ptr = ecs_parse_expr(world, "{{10}}", &(ecs_value_t){t, &value}, NULL);
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

    ecs_entity_t n1 = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "N1"}),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "T"}),
        .members = {
            {"n_1", n1},
        }
    });

    T value = {{0}};

    const char *ptr = ecs_parse_expr(world, "{{10, 20}}", &(ecs_value_t){t, &value}, NULL);
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

    ecs_entity_t n1 = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "N1"}),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "T"}),
        .members = {
            {"n_1", n1},
            {"n_2", n1},
        }
    });

    T value = {{0}};

    const char *ptr = ecs_parse_expr(world, "{{10, 20}, {30, 40}}", &(ecs_value_t){t, &value}, NULL);
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

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "T"}),
        .members = {
            {"x", ecs_id(ecs_i32_t)}
        }
    });

    T value = {0};

    const char *ptr = ecs_parse_expr(world, "{x: 10}", &(ecs_value_t){t, &value}, NULL);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_int(value.x, 10);

    ecs_fini(world);
}

void DeserializeFromExpr_struct_member_i32_neg() {
    typedef struct {
        int32_t x;
    } T;
    
    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "T"}),
        .members = {
            {"x", ecs_id(ecs_i32_t)}
        }
    });

    T value = {0};

    const char *ptr = ecs_parse_expr(world, "{x: -10}", &(ecs_value_t){t, &value}, NULL);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_int(value.x, -10);

    ecs_fini(world);
}

void DeserializeFromExpr_struct_member_i32_i32() {
    typedef struct {
        int32_t x;
        int32_t y;
    } T;
    
    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "T"}),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    T value = {0};

    const char *ptr = ecs_parse_expr(world, "{x: 10, y: 20}", &(ecs_value_t){t, &value}, NULL);
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

    ecs_entity_t n1 = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "N1"}),
        .members = {
            {"x", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "T"}),
        .members = {
            {"n_1", n1},
        }
    });

    T value = {{0}};

    const char *ptr = ecs_parse_expr(world, "{n_1: {x: 10}}", &(ecs_value_t){t, &value}, NULL);
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

    ecs_entity_t n1 = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "N1"}),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "T"}),
        .members = {
            {"n_1", n1},
        }
    });

    T value = {{0}};

    const char *ptr = ecs_parse_expr(world, "{n_1: {x: 10, y: 20}}", &(ecs_value_t){t, &value}, NULL);
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

    ecs_entity_t n1 = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "N1"}),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "T"}),
        .members = {
            {"n_1", n1},
            {"n_2", n1},
        }
    });

    T value = {{0}};

    const char *expr = 
    HEAD "{"
    LINE " n_1: {x: 10, y: 20},"
    LINE " n_2: {x: 30, y: 40}"
    LINE "}";

    const char *ptr = ecs_parse_expr(world, expr, &(ecs_value_t){t, &value}, NULL);
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

    ecs_entity_t n1 = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "N1"}),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "T"}),
        .members = {
            {"n_1", n1},
            {"n_2", n1},
        }
    });

    T value = {{0}};

    const char *expr = 
    HEAD "{"
    LINE " n_2: {x: 30, y: 40},"
    LINE " n_1: {x: 10, y: 20}"
    LINE "}";

    const char *ptr = ecs_parse_expr(world, expr, &(ecs_value_t){t, &value}, NULL);
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

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "T"}),
        .members = {
            {"x", ecs_id(ecs_i32_t), 3}
        }
    });

    T value = {0};

    const char *ptr = ecs_parse_expr(world, "{x: [10, 20, 30]}", &(ecs_value_t){t, &value}, NULL);
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

    ecs_entity_t n1 = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "N1"}),
        .members = {
            {"x", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "T"}),
        .members = {
            {"n_1", n1, 3}
        }
    });

    T value = {{{0}}};

    const char *ptr = ecs_parse_expr(world, "{n_1: [{x: 10}, {x: 20}, {x: 30}]}", &(ecs_value_t){t, &value}, NULL);
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

    ecs_entity_t n1 = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "N1"}),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "T"}),
        .members = {
            {"n_1", n1, 3}
        }
    });

    T value = {{{0}}};

    const char *ptr = ecs_parse_expr(world, "{n_1: [{x: 10, y: 20}, {x: 30, y: 40}, {x: 50, y: 60}]}", &(ecs_value_t){t, &value}, NULL);
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

void DeserializeFromExpr_struct_w_array_type_i32_i32() {
    typedef int32_t N1[2];

    typedef struct {
        N1 n_1;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t n1 = ecs_array_init(world, &(ecs_array_desc_t){
        .entity = ecs_entity(world, {.name = "vec2"}),
        .type = ecs_id(ecs_i32_t),
        .count = 2
    });

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "T"}),
        .members = {
            {"n_1", n1}
        }
    });

    T value = {{ 0 }};

    const char *ptr = ecs_parse_expr(world, "{n_1: [10, 20]}", &(ecs_value_t){t, &value}, NULL);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_int(value.n_1[0], 10);
    test_int(value.n_1[1], 20);

    ecs_fini(world);
}

void DeserializeFromExpr_struct_w_2_array_type_i32_i32() {
    typedef int32_t N1[2];

    typedef struct {
        N1 n_1;
        N1 n_2;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t n1 = ecs_array_init(world, &(ecs_array_desc_t){
        .entity = ecs_entity(world, {.name = "vec2"}),
        .type = ecs_id(ecs_i32_t),
        .count = 2
    });

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "T"}),
        .members = {
            {"n_1", n1},
            {"n_2", n1}
        }
    });

    T value = {{ 0 }};

    const char *ptr = ecs_parse_expr(world, "{n_1: [10, 20], n_2: [30, 40]}", &(ecs_value_t){t, &value}, NULL);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_int(value.n_1[0], 10);
    test_int(value.n_1[1], 20);
    test_int(value.n_2[0], 30);
    test_int(value.n_2[1], 40);

    ecs_fini(world);
}

void DeserializeFromExpr_struct_w_array_type_struct() {
    typedef struct {
        ecs_i32_t x;
        ecs_i32_t y;
    } N1;

    typedef N1 A1[2];

    typedef struct {
        A1 n_1;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t n1 = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "N1"}),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    test_assert(n1 != 0);

    ecs_entity_t a1 = ecs_array_init(world, &(ecs_array_desc_t){
        .entity = ecs_entity(world, {.name = "A1"}),
        .type = n1,
        .count = 2
    });

    test_assert(a1 != 0);

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "T"}),
        .members = {
            {"n_1", a1}
        }
    });

    test_assert(t != 0);

    T value;

    const char *ptr = ecs_parse_expr(world, "{n_1: [{x: 10, y: 20}, {x: 30, y: 40}]}", &(ecs_value_t){t, &value}, NULL);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_int(value.n_1[0].x, 10);
    test_int(value.n_1[0].y, 20);
    test_int(value.n_1[1].x, 30);
    test_int(value.n_1[1].y, 40);

    ecs_fini(world);
}

void DeserializeFromExpr_struct_w_2_array_type_struct() {
    typedef struct {
        ecs_i32_t x;
        ecs_i32_t y;
    } N1;

    typedef N1 A1[2];

    typedef struct {
        A1 n_1;
        A1 n_2;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t n1 = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "N1"}),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    test_assert(n1 != 0);

    ecs_entity_t a1 = ecs_array_init(world, &(ecs_array_desc_t){
        .entity = ecs_entity(world, {.name = "A1"}),
        .type = n1,
        .count = 2
    });

    test_assert(a1 != 0);

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "T"}),
        .members = {
            {"n_1", a1},
            {"n_2", a1}
        }
    });

    test_assert(t != 0);

    T value;

    const char *ptr = ecs_parse_expr(world, 
        "{n_1: [{x: 10, y: 20}, {x: 30, y: 40}], n_2: [{x: 50, y: 60}, {x: 70, y: 80}]}", 
        &(ecs_value_t){t, &value}, NULL);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_int(value.n_1[0].x, 10);
    test_int(value.n_1[0].y, 20);
    test_int(value.n_1[1].x, 30);
    test_int(value.n_1[1].y, 40);

    test_int(value.n_2[0].x, 50);
    test_int(value.n_2[0].y, 60);
    test_int(value.n_2[1].x, 70);
    test_int(value.n_2[1].y, 80);

    ecs_fini(world);
}

void DeserializeFromExpr_discover_type_int() {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_parse_expr(world, "10", &v, NULL) != NULL);
    test_uint(v.type, ecs_id(ecs_u64_t));
    test_assert(v.ptr != NULL);
    test_int(*(uint64_t*)v.ptr, 10);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void DeserializeFromExpr_discover_type_negative_int() {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_parse_expr(world, "-10", &v, NULL) != NULL);
    test_uint(v.type, ecs_id(ecs_i64_t));
    test_assert(v.ptr != NULL);
    test_int(*(int64_t*)v.ptr, -10);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void DeserializeFromExpr_discover_type_float() {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_parse_expr(world, "10.5", &v, NULL) != NULL);
    test_uint(v.type, ecs_id(ecs_f64_t));
    test_assert(v.ptr != NULL);
    test_flt(*(ecs_f64_t*)v.ptr, 10.5);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void DeserializeFromExpr_discover_type_negative_float() {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_parse_expr(world, "-10.5", &v, NULL) != NULL);
    test_uint(v.type, ecs_id(ecs_f64_t));
    test_assert(v.ptr != NULL);
    test_flt(*(ecs_f64_t*)v.ptr, -10.5);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void DeserializeFromExpr_discover_type_string() {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_parse_expr(world, "\"foo\"", &v, NULL) != NULL);
    test_uint(v.type, ecs_id(ecs_string_t));
    test_assert(v.ptr != NULL);
    test_str(*(ecs_string_t*)v.ptr, "foo");
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void DeserializeFromExpr_discover_type_multiline_string() {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_parse_expr(world, "`foo\nbar`", &v, NULL) != NULL);
    test_uint(v.type, ecs_id(ecs_string_t));
    test_assert(v.ptr != NULL);
    test_str(*(ecs_string_t*)v.ptr, "foo\nbar");
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void DeserializeFromExpr_discover_type_entity() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t foo = ecs_new_entity(world, "foo");

    ecs_value_t v = {0};
    test_assert(ecs_parse_expr(world, "foo", &v, NULL) != NULL);
    test_uint(v.type, ecs_id(ecs_entity_t));
    test_assert(v.ptr != NULL);
    test_uint(*(ecs_entity_t*)v.ptr, foo);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void DeserializeFromExpr_discover_type_bool() {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    test_assert(ecs_parse_expr(world, "true", &v, NULL) != NULL);
    test_uint(v.type, ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(ecs_bool_t*)v.ptr, true);
    ecs_value_free(world, v.type, v.ptr);

    ecs_os_zeromem(&v);
    test_assert(ecs_parse_expr(world, "false", &v, NULL) != NULL);
    test_uint(v.type, ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(ecs_bool_t*)v.ptr, false);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void DeserializeFromExpr_discover_type_unknown() {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};

    ecs_log_set_level(-4);
    test_assert(ecs_parse_expr(world, "{10}", &v, NULL) == NULL);
    test_assert(ecs_parse_expr(world, "[10]", &v, NULL) == NULL);

    ecs_fini(world);
}

void DeserializeFromExpr_discover_type_invalid() {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};

    ecs_log_set_level(-4);
    test_assert(ecs_parse_expr(world, "-", &v, NULL) == NULL);

    ecs_fini(world);
}
