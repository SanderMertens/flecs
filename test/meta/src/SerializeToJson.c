#include <meta.h>

void SerializeToJson_struct_bool() {
    typedef struct {
        ecs_bool_t x;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t) {
        .entity.name = "T",
        .members = {
            {"x", ecs_id(ecs_bool_t)}
        }
    });

    {
    T value = {true};
    char *expr = ecs_ptr_to_json(world, t, &value);
    test_assert(expr != NULL);
    test_str(expr, "{\"x\": true}");
    ecs_os_free(expr);
    }

    {
    T value = {false};
    char *expr = ecs_ptr_to_json(world, t, &value);
    test_assert(expr != NULL);
    test_str(expr, "{\"x\": false}");
    ecs_os_free(expr);
    }

    ecs_fini(world);
}

void SerializeToJson_struct_byte() {
    typedef struct {
        ecs_byte_t x;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t) {
        .entity.name = "T",
        .members = {
            {"x", ecs_id(ecs_byte_t)}
        }
    });

    T value = {10};
    char *expr = ecs_ptr_to_json(world, t, &value);
    test_assert(expr != NULL);
    test_str(expr, "{\"x\": 10}");
    ecs_os_free(expr);

    ecs_fini(world);
}

void SerializeToJson_struct_char() {
    typedef struct {
        ecs_char_t x;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t) {
        .entity.name = "T",
        .members = {
            {"x", ecs_id(ecs_char_t)}
        }
    });

    T value = {'a'};
    char *expr = ecs_ptr_to_json(world, t, &value);
    test_assert(expr != NULL);
    test_str(expr, "{\"x\": \"a\"}");
    ecs_os_free(expr);

    ecs_fini(world);
}

void SerializeToJson_struct_i8() {
    typedef struct {
        ecs_i8_t x;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t) {
        .entity.name = "T",
        .members = {
            {"x", ecs_id(ecs_i8_t)}
        }
    });

    T value = {10};
    char *expr = ecs_ptr_to_json(world, t, &value);
    test_assert(expr != NULL);
    test_str(expr, "{\"x\": 10}");
    ecs_os_free(expr);

    ecs_fini(world);
}

void SerializeToJson_struct_i16() {
    typedef struct {
        ecs_i16_t x;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t) {
        .entity.name = "T",
        .members = {
            {"x", ecs_id(ecs_i16_t)}
        }
    });

    T value = {10};
    char *expr = ecs_ptr_to_json(world, t, &value);
    test_assert(expr != NULL);
    test_str(expr, "{\"x\": 10}");
    ecs_os_free(expr);

    ecs_fini(world);
}

void SerializeToJson_struct_i32() {
    typedef struct {
        ecs_i32_t x;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t) {
        .entity.name = "T",
        .members = {
            {"x", ecs_id(ecs_i32_t)}
        }
    });

    T value = {10};
    char *expr = ecs_ptr_to_json(world, t, &value);
    test_assert(expr != NULL);
    test_str(expr, "{\"x\": 10}");
    ecs_os_free(expr);

    ecs_fini(world);
}

void SerializeToJson_struct_i64() {
    typedef struct {
        ecs_i64_t x;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t) {
        .entity.name = "T",
        .members = {
            {"x", ecs_id(ecs_i64_t)}
        }
    });

    T value = {10};
    char *expr = ecs_ptr_to_json(world, t, &value);
    test_assert(expr != NULL);
    test_str(expr, "{\"x\": 10}");
    ecs_os_free(expr);

    ecs_fini(world);
}

void SerializeToJson_struct_iptr() {
    typedef struct {
        ecs_iptr_t x;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t) {
        .entity.name = "T",
        .members = {
            {"x", ecs_id(ecs_iptr_t)}
        }
    });

    T value = {10};
    char *expr = ecs_ptr_to_json(world, t, &value);
    test_assert(expr != NULL);
    test_str(expr, "{\"x\": 10}");
    ecs_os_free(expr);

    ecs_fini(world);
}

void SerializeToJson_struct_u8() {
    typedef struct {
        ecs_u8_t x;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t) {
        .entity.name = "T",
        .members = {
            {"x", ecs_id(ecs_u8_t)}
        }
    });

    T value = {10};
    char *expr = ecs_ptr_to_json(world, t, &value);
    test_assert(expr != NULL);
    test_str(expr, "{\"x\": 10}");
    ecs_os_free(expr);

    ecs_fini(world);
}

void SerializeToJson_struct_u16() {
    typedef struct {
        ecs_u16_t x;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t) {
        .entity.name = "T",
        .members = {
            {"x", ecs_id(ecs_u16_t)}
        }
    });

    T value = {10};
    char *expr = ecs_ptr_to_json(world, t, &value);
    test_assert(expr != NULL);
    test_str(expr, "{\"x\": 10}");
    ecs_os_free(expr);

    ecs_fini(world);
}

void SerializeToJson_struct_u32() {
    typedef struct {
        ecs_u32_t x;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t) {
        .entity.name = "T",
        .members = {
            {"x", ecs_id(ecs_u32_t)}
        }
    });

    T value = {10};
    char *expr = ecs_ptr_to_json(world, t, &value);
    test_assert(expr != NULL);
    test_str(expr, "{\"x\": 10}");
    ecs_os_free(expr);

    ecs_fini(world);
}

void SerializeToJson_struct_u64() {
    typedef struct {
        ecs_u64_t x;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t) {
        .entity.name = "T",
        .members = {
            {"x", ecs_id(ecs_u64_t)}
        }
    });

    T value = {10};
    char *expr = ecs_ptr_to_json(world, t, &value);
    test_assert(expr != NULL);
    test_str(expr, "{\"x\": 10}");
    ecs_os_free(expr);

    ecs_fini(world);
}

void SerializeToJson_struct_uptr() {
    typedef struct {
        ecs_uptr_t x;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t) {
        .entity.name = "T",
        .members = {
            {"x", ecs_id(ecs_uptr_t)}
        }
    });

    T value = {10};
    char *expr = ecs_ptr_to_json(world, t, &value);
    test_assert(expr != NULL);
    test_str(expr, "{\"x\": 10}");
    ecs_os_free(expr);

    ecs_fini(world);
}

void SerializeToJson_struct_float() {
    typedef struct {
        ecs_f32_t x;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t) {
        .entity.name = "T",
        .members = {
            {"x", ecs_id(ecs_f32_t)}
        }
    });

    T value = {10.5};
    char *expr = ecs_ptr_to_json(world, t, &value);
    test_assert(expr != NULL);
    test_str(expr, "{\"x\": 10.500000}");
    ecs_os_free(expr);

    ecs_fini(world);
}

void SerializeToJson_struct_double() {
    typedef struct {
        ecs_f64_t x;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t) {
        .entity.name = "T",
        .members = {
            {"x", ecs_id(ecs_f64_t)}
        }
    });

    T value = {10.5};
    char *expr = ecs_ptr_to_json(world, t, &value);
    test_assert(expr != NULL);
    test_str(expr, "{\"x\": 10.500000}");
    ecs_os_free(expr);

    ecs_fini(world);
}

void SerializeToJson_struct_string() {
    typedef struct {
        char* x;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t) {
        .entity.name = "T",
        .members = {
            {"x", ecs_id(ecs_string_t)}
        }
    });

    T value = {"Hello World"};
    char *expr = ecs_ptr_to_json(world, t, &value);
    test_assert(expr != NULL);
    test_str(expr, "{\"x\": \"Hello World\"}");
    ecs_os_free(expr);

    ecs_fini(world);
}

void SerializeToJson_struct_entity() {
    typedef struct {
        ecs_entity_t x;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t) {
        .entity.name = "T",
        .members = {
            {"x", ecs_id(ecs_entity_t)}
        }
    });

    T value = {EcsFlecsCore};
    char *expr = ecs_ptr_to_json(world, t, &value);
    test_assert(expr != NULL);
    test_str(expr, "{\"x\": \"flecs.core\"}");
    ecs_os_free(expr);

    ecs_fini(world);
}

void SerializeToJson_struct_i32_i32() {
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

    T value = {10, 20};
    char *expr = ecs_ptr_to_json(world, t, &value);
    test_assert(expr != NULL);
    test_str(expr, "{\"x\": 10, \"y\": 20}");
    ecs_os_free(expr);

    ecs_fini(world);
}

void SerializeToJson_struct_nested_i32() {
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
    char *expr = ecs_ptr_to_json(world, t, &value);
    test_assert(expr != NULL);
    test_str(expr, "{\"n_1\": {\"x\": 10}}");
    ecs_os_free(expr);

    ecs_fini(world);
}

void SerializeToJson_struct_nested_i32_i32() {
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
    char *expr = ecs_ptr_to_json(world, t, &value);
    test_assert(expr != NULL);
    test_str(expr, "{\"n_1\": {\"x\": 10, \"y\": 20}}");
    ecs_os_free(expr);

    ecs_fini(world);
}

void SerializeToJson_struct_2_nested_i32_i32() {
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

    T value = {{10, 20}, {30, 40}};
    char *expr = ecs_ptr_to_json(world, t, &value);
    test_assert(expr != NULL);
    test_str(expr, "{\"n_1\": {\"x\": 10, \"y\": 20}, \"n_2\": {\"x\": 30, \"y\": 40}}");
    ecs_os_free(expr);

    ecs_fini(world);
}

void SerializeToJson_struct_i32_array_3() {
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
    char *expr = ecs_ptr_to_json(world, t, &value);
    test_assert(expr != NULL);
    test_str(expr, "{\"x\": [10, 20, 30]}");
    ecs_os_free(expr);

    ecs_fini(world);
}

void SerializeToJson_struct_struct_i32_array_3() {
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
    char *expr = ecs_ptr_to_json(world, t, &value);
    test_assert(expr != NULL);
    test_str(expr, "{\"n_1\": [{\"x\": 10}, {\"x\": 20}, {\"x\": 30}]}");
    ecs_os_free(expr);

    ecs_fini(world);
}

void SerializeToJson_struct_struct_i32_i32_array_3() {
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
    char *expr = ecs_ptr_to_json(world, t, &value);
    test_assert(expr != NULL);
    test_str(expr, "{\"n_1\": [{\"x\": 10, \"y\": 20}, {\"x\": 30, \"y\": 40}, {\"x\": 50, \"y\": 60}]}");
    ecs_os_free(expr);

    ecs_fini(world);
}
