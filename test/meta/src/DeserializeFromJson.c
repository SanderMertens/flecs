#include <meta.h>

void DeserializeFromJson_struct_bool() {
    typedef struct {
        bool v;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t) {
        .entity.name = "T",
        .members = {
            {"v", ecs_id(ecs_bool_t)}
        }
    });

    test_assert(t != 0);

    T value = {false};

    const char *ptr = ecs_parse_json(world, NULL, "{\"v\": true}", NULL, t, &value);
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

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t) {
        .entity.name = "T",
        .members = {
            {"v", ecs_id(ecs_byte_t)}
        }
    });

    test_assert(t != 0);

    T value = {0};

    const char *ptr = ecs_parse_json(world, NULL, "{\"v\": 10}", NULL, t, &value);
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

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t) {
        .entity.name = "T",
        .members = {
            {"v", ecs_id(ecs_char_t)}
        }
    });

    test_assert(t != 0);

    T value = {0};

    const char *ptr = ecs_parse_json(world, NULL, "{\"v\": 10}", NULL, t, &value);
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

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t) {
        .entity.name = "T",
        .members = {
            {"v", ecs_id(ecs_char_t)}
        }
    });

    test_assert(t != 0);

    T value = {0};

    const char *ptr = ecs_parse_json(world, NULL, "{\"v\": \"a\"}", NULL, t, &value);
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

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t) {
        .entity.name = "T",
        .members = {
            {"v", ecs_id(ecs_i8_t)}
        }
    });

    test_assert(t != 0);

    T value = {0};

    const char *ptr = ecs_parse_json(world, NULL, "{\"v\": 10}", NULL, t, &value);
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

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t) {
        .entity.name = "T",
        .members = {
            {"v", ecs_id(ecs_i16_t)}
        }
    });

    test_assert(t != 0);

    T value = {0};

    const char *ptr = ecs_parse_json(world, NULL, "{\"v\": 10}", NULL, t, &value);
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

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t) {
        .entity.name = "T",
        .members = {
            {"v", ecs_id(ecs_i32_t)}
        }
    });

    test_assert(t != 0);

    T value = {0};

    const char *ptr = ecs_parse_json(world, NULL, "{\"v\": 10}", NULL, t, &value);
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

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t) {
        .entity.name = "T",
        .members = {
            {"v", ecs_id(ecs_i64_t)}
        }
    });

    test_assert(t != 0);

    T value = {0};

    const char *ptr = ecs_parse_json(world, NULL, "{\"v\": 10}", NULL, t, &value);
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

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t) {
        .entity.name = "T",
        .members = {
            {"v", ecs_id(ecs_iptr_t)}
        }
    });

    test_assert(t != 0);

    T value = {0};

    const char *ptr = ecs_parse_json(world, NULL, "{\"v\": 10}", NULL, t, &value);
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

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t) {
        .entity.name = "T",
        .members = {
            {"v", ecs_id(ecs_u8_t)}
        }
    });

    test_assert(t != 0);

    T value = {0};

    const char *ptr = ecs_parse_json(world, NULL, "{\"v\": 10}", NULL, t, &value);
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

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t) {
        .entity.name = "T",
        .members = {
            {"v", ecs_id(ecs_u16_t)}
        }
    });

    test_assert(t != 0);

    T value = {0};

    const char *ptr = ecs_parse_json(world, NULL, "{\"v\": 10}", NULL, t, &value);
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

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t) {
        .entity.name = "T",
        .members = {
            {"v", ecs_id(ecs_u32_t)}
        }
    });

    test_assert(t != 0);

    T value = {0};

    const char *ptr = ecs_parse_json(world, NULL, "{\"v\": 10}", NULL, t, &value);
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

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t) {
        .entity.name = "T",
        .members = {
            {"v", ecs_id(ecs_u64_t)}
        }
    });

    test_assert(t != 0);

    T value = {0};

    const char *ptr = ecs_parse_json(world, NULL, "{\"v\": 10}", NULL, t, &value);
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

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t) {
        .entity.name = "T",
        .members = {
            {"v", ecs_id(ecs_uptr_t)}
        }
    });

    test_assert(t != 0);

    T value = {0};

    const char *ptr = ecs_parse_json(world, NULL, "{\"v\": 10}", NULL, t, &value);
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

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t) {
        .entity.name = "T",
        .members = {
            {"v", ecs_id(ecs_f32_t)}
        }
    });

    test_assert(t != 0);

    T value = {0};

    const char *ptr = ecs_parse_json(world, NULL, "{\"v\": 10.5}", NULL, t, &value);
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

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t) {
        .entity.name = "T",
        .members = {
            {"v", ecs_id(ecs_f64_t)}
        }
    });

    test_assert(t != 0);

    T value = {0};

    const char *ptr = ecs_parse_json(world, NULL, "{\"v\": 10.5}", NULL, t, &value);
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

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t) {
        .entity.name = "T",
        .members = {
            {"v", ecs_id(ecs_i32_t)}
        }
    });

    test_assert(t != 0);

    T value = {0};

    const char *ptr = ecs_parse_json(world, NULL, "{\"v\": -10}", NULL, t, &value);
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

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t) {
        .entity.name = "T",
        .members = {
            {"v", ecs_id(ecs_f32_t)}
        }
    });

    test_assert(t != 0);

    T value = {0};

    const char *ptr = ecs_parse_json(world, NULL, "{\"v\": -10.5}", NULL, t, &value);
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

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t) {
        .entity.name = "T",
        .members = {
            {"v", ecs_id(ecs_string_t)}
        }
    });

    test_assert(t != 0);

    T value = {0};

    const char *ptr = ecs_parse_json(world, NULL, "{\"v\": \"Hello World\"}", NULL, t, &value);
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

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t) {
        .entity.name = "T",
        .members = {
            {"v", ecs_id(ecs_entity_t)}
        }
    });

    test_assert(t != 0);

    T value = {0};

    const char *ptr = ecs_parse_json(world, NULL, "{\"v\": \"flecs.core\"}", NULL, t, &value);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_uint(value.v, EcsFlecsCore);

    ecs_fini(world);
}

void DeserializeFromJson_struct_i32_i32() {
    typedef struct {
        ecs_i32_t x;
        ecs_i32_t y;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t) {
        .entity.name = "T",
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    test_assert(t != 0);

    T value = {0};

    const char *ptr = ecs_parse_json(world, NULL, "{\"x\": 10, \"y\": 20}", NULL, t, &value);
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

    const char *ptr = ecs_parse_json(world, NULL, "{\"n_1\": {\"x\": 10}}", NULL, t, &value);
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

    const char *ptr = ecs_parse_json(world, NULL, "{\"n_1\": {\"x\": 10, \"y\": 20}}", NULL, t, &value);
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

    T value = {{0}};

    const char *ptr = ecs_parse_json(world, NULL, 
        "{\"n_1\": {\"x\": 10, \"y\": 20}, \"n_2\": {\"x\": 30, \"y\": 40}}", NULL, t, &value);
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

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t) {
        .entity.name = "T",
        .members = {
            {"x", ecs_id(ecs_entity_t), 3}
        }
    });

    test_assert(t != 0);

    T value = {0};

    const char *ptr = ecs_parse_json(world, NULL, "{\"x\": [10, 20, 30]}", NULL, t, &value);
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

    const char *ptr = ecs_parse_json(world, NULL, "{\"n_1\": [{\"x\": 10}, {\"x\": 20}, {\"x\": 30}]}", NULL, t, &value);
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

    const char *ptr = ecs_parse_json(world, NULL, 
        "{\"n_1\": [{\"x\": 10, \"y\": 20}, {\"x\": 30, \"y\": 40}, {\"x\": 50, \"y\": 60}]}", NULL, t, &value);
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
