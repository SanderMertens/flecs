#include <meta.h>

void DeserializeFromJson_struct_bool() {
    typedef struct {
        bool v;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "T"}),
        .members = {
            {"v", ecs_id(ecs_bool_t)}
        }
    });

    test_assert(t != 0);

    T value = {false};

    const char *ptr = ecs_parse_json(world, "{\"v\": true}", t, &value, NULL);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_bool(value.v, true);

    ecs_fini(world);
}

void DeserializeFromJson_struct_byte() {
    typedef struct {
        ecs_byte_t v;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "T"}),
        .members = {
            {"v", ecs_id(ecs_byte_t)}
        }
    });

    test_assert(t != 0);

    T value = {0};

    const char *ptr = ecs_parse_json(world, "{\"v\": 10}", t, &value, NULL);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_uint(value.v, 10);

    ecs_fini(world);
}

void DeserializeFromJson_struct_char() {
    typedef struct {
        ecs_char_t v;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "T"}),
        .members = {
            {"v", ecs_id(ecs_char_t)}
        }
    });

    test_assert(t != 0);

    T value = {0};

    const char *ptr = ecs_parse_json(world, "{\"v\": 10}", t, &value, NULL);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_uint(value.v, 10);

    ecs_fini(world);
}

void DeserializeFromJson_struct_char_literal() {
    typedef struct {
        ecs_char_t v;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "T"}),
        .members = {
            {"v", ecs_id(ecs_char_t)}
        }
    });

    test_assert(t != 0);

    T value = {0};

    const char *ptr = ecs_parse_json(world, "{\"v\": \"a\"}", t, &value, NULL);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_uint(value.v, 'a');

    ecs_fini(world);
}

void DeserializeFromJson_struct_i8() {
    typedef struct {
        ecs_i8_t v;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "T"}),
        .members = {
            {"v", ecs_id(ecs_i8_t)}
        }
    });

    test_assert(t != 0);

    T value = {0};

    const char *ptr = ecs_parse_json(world, "{\"v\": 10}", t, &value, NULL);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_int(value.v, 10);

    ecs_fini(world);
}

void DeserializeFromJson_struct_i16() {
    typedef struct {
        ecs_i16_t v;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "T"}),
        .members = {
            {"v", ecs_id(ecs_i16_t)}
        }
    });

    test_assert(t != 0);

    T value = {0};

    const char *ptr = ecs_parse_json(world, "{\"v\": 10}", t, &value, NULL);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_int(value.v, 10);

    ecs_fini(world);
}

void DeserializeFromJson_struct_i32() {
    typedef struct {
        ecs_i32_t v;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "T"}),
        .members = {
            {"v", ecs_id(ecs_i32_t)}
        }
    });

    test_assert(t != 0);

    T value = {0};

    const char *ptr = ecs_parse_json(world, "{\"v\": 10}", t, &value, NULL);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_int(value.v, 10);

    ecs_fini(world);
}

void DeserializeFromJson_struct_i64() {
    typedef struct {
        ecs_i64_t v;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "T"}),
        .members = {
            {"v", ecs_id(ecs_i64_t)}
        }
    });

    test_assert(t != 0);

    T value = {0};

    const char *ptr = ecs_parse_json(world, "{\"v\": 10}", t, &value, NULL);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_int(value.v, 10);

    ecs_fini(world);
}

void DeserializeFromJson_struct_iptr() {
    typedef struct {
        ecs_iptr_t v;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "T"}),
        .members = {
            {"v", ecs_id(ecs_iptr_t)}
        }
    });

    test_assert(t != 0);

    T value = {0};

    const char *ptr = ecs_parse_json(world, "{\"v\": 10}", t, &value, NULL);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_int(value.v, 10);

    ecs_fini(world);
}

void DeserializeFromJson_struct_u8() {
    typedef struct {
        ecs_u8_t v;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "T"}),
        .members = {
            {"v", ecs_id(ecs_u8_t)}
        }
    });

    test_assert(t != 0);

    T value = {0};

    const char *ptr = ecs_parse_json(world, "{\"v\": 10}", t, &value, NULL);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_uint(value.v, 10);

    ecs_fini(world);
}

void DeserializeFromJson_struct_u16() {
    typedef struct {
        ecs_u16_t v;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "T"}),
        .members = {
            {"v", ecs_id(ecs_u16_t)}
        }
    });

    test_assert(t != 0);

    T value = {0};

    const char *ptr = ecs_parse_json(world, "{\"v\": 10}", t, &value, NULL);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_uint(value.v, 10);

    ecs_fini(world);
}

void DeserializeFromJson_struct_u32() {
    typedef struct {
        ecs_u32_t v;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "T"}),
        .members = {
            {"v", ecs_id(ecs_u32_t)}
        }
    });

    test_assert(t != 0);

    T value = {0};

    const char *ptr = ecs_parse_json(world, "{\"v\": 10}", t, &value, NULL);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_uint(value.v, 10);

    ecs_fini(world);
}

void DeserializeFromJson_struct_u64() {
    typedef struct {
        ecs_u64_t v;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "T"}),
        .members = {
            {"v", ecs_id(ecs_u64_t)}
        }
    });

    test_assert(t != 0);

    T value = {0};

    const char *ptr = ecs_parse_json(world, "{\"v\": 10}", t, &value, NULL);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_uint(value.v, 10);

    ecs_fini(world);
}

void DeserializeFromJson_struct_uptr() {
    typedef struct {
        ecs_uptr_t v;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "T"}),
        .members = {
            {"v", ecs_id(ecs_uptr_t)}
        }
    });

    test_assert(t != 0);

    T value = {0};

    const char *ptr = ecs_parse_json(world, "{\"v\": 10}", t, &value, NULL);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_uint(value.v, 10);

    ecs_fini(world);
}

void DeserializeFromJson_struct_float() {
    typedef struct {
        ecs_f32_t v;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "T"}),
        .members = {
            {"v", ecs_id(ecs_f32_t)}
        }
    });

    test_assert(t != 0);

    T value = {0};

    const char *ptr = ecs_parse_json(world, "{\"v\": 10.5}", t, &value, NULL);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_flt(value.v, 10.5);

    ecs_fini(world);
}

void DeserializeFromJson_struct_double() {
    typedef struct {
        ecs_f64_t v;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "T"}),
        .members = {
            {"v", ecs_id(ecs_f64_t)}
        }
    });

    test_assert(t != 0);

    T value = {0};

    const char *ptr = ecs_parse_json(world, "{\"v\": 10.5}", t, &value, NULL);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_flt(value.v, 10.5);

    ecs_fini(world);
}

void DeserializeFromJson_struct_negative_int() {
    typedef struct {
        ecs_i32_t v;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "T"}),
        .members = {
            {"v", ecs_id(ecs_i32_t)}
        }
    });

    test_assert(t != 0);

    T value = {0};

    const char *ptr = ecs_parse_json(world, "{\"v\": -10}", t, &value, NULL);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_int(value.v, -10);

    ecs_fini(world);
}

void DeserializeFromJson_struct_negative_float() {
    typedef struct {
        ecs_f32_t v;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "T"}),
        .members = {
            {"v", ecs_id(ecs_f32_t)}
        }
    });

    test_assert(t != 0);

    T value = {0};

    const char *ptr = ecs_parse_json(world, "{\"v\": -10.5}", t, &value, NULL);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_flt(value.v, -10.5);

    ecs_fini(world);
}

void DeserializeFromJson_struct_string() {
    typedef struct {
        char* v;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "T"}),
        .members = {
            {"v", ecs_id(ecs_string_t)}
        }
    });

    test_assert(t != 0);

    T value = {0};

    const char *ptr = ecs_parse_json(world, "{\"v\": \"Hello World\"}", t, &value, NULL);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_str(value.v, "Hello World");

    ecs_os_free(value.v);

    ecs_fini(world);
}

void DeserializeFromJson_struct_entity() {
    typedef struct {
        ecs_entity_t v;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "T"}),
        .members = {
            {"v", ecs_id(ecs_entity_t)}
        }
    });

    test_assert(t != 0);

    T value = {0};

    const char *ptr = ecs_parse_json(world, "{\"v\": \"flecs.core\"}", t, &value, NULL);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_uint(value.v, EcsFlecsCore);

    ecs_fini(world);
}

void DeserializeFromJson_struct_enum() {
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
    const char *ptr = ecs_parse_json(world, "{\"v\": \"Red\"}", t, &value, NULL);
    test_assert(ptr != NULL);
    test_int(value.v, Red);
    }

    {
    T value = {0};
    const char *ptr = ecs_parse_json(world, "{\"v\": \"Blue\"}", t, &value, NULL);
    test_assert(ptr != NULL);
    test_int(value.v, Blue);
    }

    {
    T value = {0};
    const char *ptr = ecs_parse_json(world, "{\"v\": \"Green\"}", t, &value, NULL);
    test_assert(ptr != NULL);
    test_int(value.v, Green);
    }

    {
    ecs_log_set_level(-4);
    T value = {0};
    const char *ptr = ecs_parse_json(world, "{\"v\": \"Black\"}", t, &value, NULL);
    test_assert(ptr == NULL);
    }

    ecs_fini(world);
}

void DeserializeFromJson_struct_bitmask() {
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
    const char *ptr = ecs_parse_json(world, "{\"v\":\"Lettuce\"}", t, &value, NULL);
    test_assert(ptr != NULL);
    test_uint(value.v, Lettuce);
    }

    {
    T value = {0};
    const char *ptr = ecs_parse_json(world, "{\"v\":\"Lettuce|Bacon\"}", t, &value, NULL);
    test_assert(ptr != NULL);
    test_uint(value.v, Lettuce|Bacon);
    }

    {
    T value = {0};
    const char *ptr = ecs_parse_json(world, "{\"v\":\"Lettuce|Bacon|Tomato|Cheese\"}", t, &value, NULL);
    test_assert(ptr != NULL);
    test_uint(value.v, Lettuce|Bacon|Tomato|Cheese);
    }

    {
    T value = {0};
    const char *ptr = ecs_parse_json(world, "{\"v\":\"BLT\"}", t, &value, NULL);
    test_assert(ptr != NULL);
    test_uint(value.v, Lettuce|Bacon|Tomato);
    }

    {
    T value = {0};
    const char *ptr = ecs_parse_json(world, "{\"v\":0}", t, &value, NULL);
    test_assert(ptr != NULL);
    test_uint(value.v, 0);
    }

    {
    ecs_log_set_level(-4);
    T value = {0};
    const char *ptr = ecs_parse_json(world, "{\"v\":\"Foo\"}", t, &value, NULL);
    test_assert(ptr == NULL);
    }

    ecs_fini(world);
}

void DeserializeFromJson_struct_i32_i32() {
    typedef struct {
        ecs_i32_t x;
        ecs_i32_t y;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "T"}),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    test_assert(t != 0);

    T value = {0};

    const char *ptr = ecs_parse_json(world, "{\"x\": 10, \"y\": 20}", t, &value, NULL);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_int(value.x, 10);
    test_int(value.y, 20);

    ecs_fini(world);
}

void DeserializeFromJson_struct_nested_i32() {
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

    const char *ptr = ecs_parse_json(world, "{\"n_1\": {\"x\": 10}}", t, &value, NULL);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_int(value.n_1.x, 10);

    ecs_fini(world);
}

void DeserializeFromJson_struct_nested_i32_i32() {
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

    const char *ptr = ecs_parse_json(world, "{\"n_1\": {\"x\": 10, \"y\": 20}}", t, &value, NULL);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_int(value.n_1.x, 10);
    test_int(value.n_1.y, 20);

    ecs_fini(world);
}

void DeserializeFromJson_struct_2_nested_i32_i32() {
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
            {"n_2", n1}
        }
    });

    T value = {{0}};

    const char *ptr = ecs_parse_json(world,
        "{\"n_1\": {\"x\": 10, \"y\": 20}, \"n_2\": {\"x\": 30, \"y\": 40}}", 
        t, &value, NULL);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_int(value.n_1.x, 10);
    test_int(value.n_1.y, 20);
    test_int(value.n_2.x, 30);
    test_int(value.n_2.y, 40);

    ecs_fini(world);
}

void DeserializeFromJson_struct_i32_array_3() {
    typedef struct {
        ecs_entity_t x[3];
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "T"}),
        .members = {
            {"x", ecs_id(ecs_entity_t), 3}
        }
    });

    test_assert(t != 0);

    T value = {0};

    ecs_ensure(world, 10);
    ecs_ensure(world, 20);
    ecs_ensure(world, 30);

    const char *ptr = ecs_parse_json(world, "{\"x\": [10, 20, 30]}", t, &value, NULL);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_int(value.x[0], 10);
    test_int(value.x[1], 20);
    test_int(value.x[2], 30);

    ecs_fini(world);
}

void DeserializeFromJson_struct_struct_i32_array_3() {
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

    const char *ptr = ecs_parse_json(world, "{\"n_1\": [{\"x\": 10}, {\"x\": 20}, {\"x\": 30}]}", t, &value, NULL);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_int(value.n_1[0].x, 10);
    test_int(value.n_1[1].x, 20);
    test_int(value.n_1[2].x, 30);

    ecs_fini(world);
}

void DeserializeFromJson_struct_struct_i32_i32_array_3() {
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

    const char *ptr = ecs_parse_json(world, 
        "{\"n_1\": [{\"x\": 10, \"y\": 20}, {\"x\": 30, \"y\": 40}, {\"x\": 50, \"y\": 60}]}"
        , t, &value, NULL);
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

void DeserializeFromJson_struct_w_array_type_i32_i32() {
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

    const char *ptr = ecs_parse_json(world, "{\"n_1\": [10, 20]}", t, &value, NULL);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_int(value.n_1[0], 10);
    test_int(value.n_1[1], 20);

    ecs_fini(world);
}

void DeserializeFromJson_struct_w_2_array_type_i32_i32() {
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

    const char *ptr = ecs_parse_json(world, "{\"n_1\": [10, 20], \"n_2\": [30, 40]}", t, &value, NULL);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_int(value.n_1[0], 10);
    test_int(value.n_1[1], 20);
    test_int(value.n_2[0], 30);
    test_int(value.n_2[1], 40);

    ecs_fini(world);
}

void DeserializeFromJson_struct_w_nested_member_i32() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Point) = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "Point"}),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    test_assert(ecs_id(Point) != 0);

    ecs_entity_t ecs_id(Line) = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "Line"}),
        .members = {
            {"start", ecs_id(Point)},
            {"stop", ecs_id(Point)}
        }
    });

    test_assert(ecs_id(Line) != 0);

    Line value = {0};

    const char *ptr = ecs_parse_json(world, "{\"start.x\": 10}", 
        ecs_id(Line), &value, NULL);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_int(value.start.x, 10);

    ecs_fini(world);
}

void DeserializeFromJson_struct_w_2_nested_members_i32() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Point) = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "Point"}),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    test_assert(ecs_id(Point) != 0);

    ecs_entity_t ecs_id(Line) = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "Line"}),
        .members = {
            {"start", ecs_id(Point)},
            {"stop", ecs_id(Point)}
        }
    });

    test_assert(ecs_id(Line) != 0);

    Line value = {0};

    const char *ptr = ecs_parse_json(world, 
        "{\"start.x\": 10, \"start.y\": 20, \"stop.x\": 30, \"stop.y\": 40}", 
            ecs_id(Line), &value, NULL);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_int(value.start.x, 10);
    test_int(value.start.y, 20);
    test_int(value.stop.x, 30);
    test_int(value.stop.y, 40);

    ecs_fini(world);
}

void DeserializeFromJson_struct_w_nested_members_struct() {
    ecs_world_t *world = ecs_init();

    typedef struct {
        Line left;
        Line right;
    } TwoLines;

    ecs_entity_t ecs_id(Point) = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "Point"}),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    test_assert(ecs_id(Point) != 0);

    ecs_entity_t ecs_id(Line) = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "Line"}),
        .members = {
            {"start", ecs_id(Point)},
            {"stop", ecs_id(Point)}
        }
    });

    test_assert(ecs_id(Line) != 0);

    ecs_entity_t ecs_id(TwoLines) = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "TwoLines"}),
        .members = {
            {"left", ecs_id(Line)},
            {"right", ecs_id(Line)}
        }
    });

    test_assert(ecs_id(TwoLines) != 0);

    TwoLines value = {0};

    const char *ptr = ecs_parse_json(world, "{\"left.start\": {\"x\": 10, \"y\": 20}}", 
        ecs_id(TwoLines), &value, NULL);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_int(value.left.start.x, 10);
    test_int(value.left.start.y, 20);

    ecs_fini(world);
}

void DeserializeFromJson_struct_w_2_nested_members_struct() {
    ecs_world_t *world = ecs_init();

    typedef struct {
        Line left;
        Line right;
    } TwoLines;

    ecs_entity_t ecs_id(Point) = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "Point"}),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    test_assert(ecs_id(Point) != 0);

    ecs_entity_t ecs_id(Line) = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "Line"}),
        .members = {
            {"start", ecs_id(Point)},
            {"stop", ecs_id(Point)}
        }
    });

    test_assert(ecs_id(Line) != 0);

    ecs_entity_t ecs_id(TwoLines) = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "TwoLines"}),
        .members = {
            {"left", ecs_id(Line)},
            {"right", ecs_id(Line)}
        }
    });

    test_assert(ecs_id(TwoLines) != 0);

    TwoLines value = {0};

    const char *ptr = ecs_parse_json(world, "{\"left.start\": {\"x\": 10, \"y\": 20}, \"right.start\": {\"x\": 30, \"y\": 40}}", 
        ecs_id(TwoLines), &value, NULL);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_int(value.left.start.x, 10);
    test_int(value.left.start.y, 20);

    test_int(value.right.start.x, 30);
    test_int(value.right.start.y, 40);

    ecs_fini(world);
}

void DeserializeFromJson_deser_entity_1_component_1_member() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "Position"}),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    test_assert(ecs_id(Position) != 0);

    ecs_entity_t e = ecs_new_id(world);
    const char *ptr = ecs_parse_json_values(world, e, 
        "{\"ids\":[[\"Position\"]],\"values\":[{\"x\":10}]}", NULL);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    const Position *pos = ecs_get(world, e, Position);
    test_assert(pos != NULL);
    test_int(pos->x, 10);

    ecs_fini(world);
}

void DeserializeFromJson_deser_entity_1_component_1_member_w_spaces() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "Position"}),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    test_assert(ecs_id(Position) != 0);

    ecs_entity_t e = ecs_new_id(world);
    const char *ptr = ecs_parse_json_values(world, e, 
        " { \"ids\" : [ [ \"Position\" ] ] , \"values\" : [ { \"x\" :10 } ] }", NULL);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    const Position *pos = ecs_get(world, e, Position);
    test_assert(pos != NULL);
    test_int(pos->x, 10);

    ecs_fini(world);
}

void DeserializeFromJson_deser_entity_1_component_2_members() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "Position"}),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    test_assert(ecs_id(Position) != 0);

    ecs_entity_t e = ecs_new_id(world);
    const char *ptr = ecs_parse_json_values(world, e, 
        "{\"ids\":[[\"Position\"]],\"values\":[{\"x\":10, \"y\":20}]}", NULL);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    const Position *pos = ecs_get(world, e, Position);
    test_assert(pos != NULL);
    test_int(pos->x, 10);
    test_int(pos->y, 20);

    ecs_fini(world);
}

void DeserializeFromJson_deser_entity_2_components() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "Position"}),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    test_assert(ecs_id(Position) != 0);

    ecs_entity_t ecs_id(Velocity) = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "Velocity"}),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    test_assert(ecs_id(Velocity) != 0);

    ecs_entity_t e = ecs_new_id(world);
    const char *ptr = ecs_parse_json_values(world, e, 
        "{"
            "\"ids\":[[\"Position\"], [\"Velocity\"]],"
            "\"values\":["
                "{\"x\":10, \"y\":20}, "
                "{\"x\":1, \"y\":2}"
            "]}", NULL);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    const Position *pos = ecs_get(world, e, Position);
    test_assert(pos != NULL);
    test_int(pos->x, 10);
    test_int(pos->y, 20);

    const Velocity *vel = ecs_get(world, e, Velocity);
    test_assert(vel != NULL);
    test_int(vel->x, 1);
    test_int(vel->y, 2);

    ecs_fini(world);
}

void DeserializeFromJson_deser_entity_1_component_composite_member() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Point) = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "Point"}),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    test_assert(ecs_id(Point) != 0);

    ecs_entity_t ecs_id(Line) = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "Line"}),
        .members = {
            {"start", ecs_id(Point)},
            {"stop", ecs_id(Point)}
        }
    });

    test_assert(ecs_id(Line) != 0);

    ecs_entity_t e = ecs_new_id(world);
    const char *ptr = ecs_parse_json_values(world, e, 
        "{\"ids\":[[\"Line\"]],\"values\":[{\"start\": {\"x\":10, \"y\":20}, \"stop\": {\"x\":30, \"y\":40}}]}", NULL);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    const Line *line = ecs_get(world, e, Line);
    test_assert(line != NULL);
    test_int(line->start.x, 10);
    test_int(line->start.y, 20);
    test_int(line->stop.x, 30);
    test_int(line->stop.y, 40);

    ecs_fini(world);
}

void DeserializeFromJson_deser_entity_1_component_nested_member() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Point) = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "Point"}),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    test_assert(ecs_id(Point) != 0);

    ecs_entity_t ecs_id(Line) = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "Line"}),
        .members = {
            {"start", ecs_id(Point)},
            {"stop", ecs_id(Point)}
        }
    });

    test_assert(ecs_id(Line) != 0);

    ecs_entity_t e = ecs_new_id(world);
    const char *ptr = ecs_parse_json_values(world, e, 
        "{\"ids\":[[\"Line\"]],\"values\":[{\"start.x\": 10, \"start.y\": 20, \"stop.x\": 30, \"stop.y\": 40}]}", NULL);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    const Line *line = ecs_get(world, e, Line);
    test_assert(line != NULL);
    test_int(line->start.x, 10);
    test_int(line->start.y, 20);
    test_int(line->stop.x, 30);
    test_int(line->stop.y, 40);

    ecs_fini(world);
}

void DeserializeFromJson_deser_entity_1_pair() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tgt);

    ecs_entity_t ecs_id(Position) = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "Position"}),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    test_assert(ecs_id(Position) != 0);

    ecs_entity_t e = ecs_new_id(world);
    const char *ptr = ecs_parse_json_values(world, e, 
        "{\"ids\":[[\"Position\", \"Tgt\"]],\"values\":[{\"x\":10, \"y\":20}]}", NULL);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    const Position *pos = ecs_get_pair(world, e, Position, Tgt);
    test_assert(pos != NULL);
    test_int(pos->x, 10);
    test_int(pos->y, 20);

    ecs_fini(world);
}

void DeserializeFromJson_deser_entity_2_pairs() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tgt);

    ecs_entity_t ecs_id(Position) = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "Position"}),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    test_assert(ecs_id(Position) != 0);

    ecs_entity_t ecs_id(Velocity) = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "Velocity"}),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    test_assert(ecs_id(Velocity) != 0);

    ecs_entity_t e = ecs_new_id(world);
    const char *ptr = ecs_parse_json_values(world, e, 
        "{"
            "\"ids\":[[\"Position\", \"Tgt\"], [\"Velocity\", \"Tgt\"]],"
            "\"values\":["
                "{\"x\":10, \"y\":20}, "
                "{\"x\":1, \"y\":2}"
            "]}", NULL);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    const Position *pos = ecs_get_pair(world, e, Position, Tgt);
    test_assert(pos != NULL);
    test_int(pos->x, 10);
    test_int(pos->y, 20);

    const Velocity *vel = ecs_get_pair(world, e, Velocity, Tgt);
    test_assert(vel != NULL);
    test_int(vel->x, 1);
    test_int(vel->y, 2);

    ecs_fini(world);
}
