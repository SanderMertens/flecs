#include <meta.h>

void DeserializeFromJson_struct_bool(void) {
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

    const char *ptr = ecs_ptr_from_json(world, t, &value, "{\"v\": true}", NULL);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_bool(value.v, true);

    ecs_fini(world);
}

void DeserializeFromJson_struct_byte(void) {
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

    const char *ptr = ecs_ptr_from_json(world, t, &value, "{\"v\": 10}", NULL);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_uint(value.v, 10);

    ecs_fini(world);
}

void DeserializeFromJson_struct_char(void) {
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

    const char *ptr = ecs_ptr_from_json(world, t, &value, "{\"v\": 10}", NULL);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_uint(value.v, 10);

    ecs_fini(world);
}

void DeserializeFromJson_struct_char_literal(void) {
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

    const char *ptr = ecs_ptr_from_json(world, t, &value, "{\"v\": \"a\"}", NULL);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_uint(value.v, 'a');

    ecs_fini(world);
}

void DeserializeFromJson_struct_i8(void) {
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

    const char *ptr = ecs_ptr_from_json(world, t, &value, "{\"v\": 10}", NULL);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_int(value.v, 10);

    ecs_fini(world);
}

void DeserializeFromJson_struct_i16(void) {
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

    const char *ptr = ecs_ptr_from_json(world, t, &value, "{\"v\": 10}", NULL);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_int(value.v, 10);

    ecs_fini(world);
}

void DeserializeFromJson_struct_i32(void) {
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

    const char *ptr = ecs_ptr_from_json(world, t, &value, "{\"v\": 10}", NULL);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_int(value.v, 10);

    ecs_fini(world);
}

void DeserializeFromJson_struct_i64(void) {
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

    const char *ptr = ecs_ptr_from_json(world, t, &value, "{\"v\": 10}", NULL);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_int(value.v, 10);

    ecs_fini(world);
}

void DeserializeFromJson_struct_iptr(void) {
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

    const char *ptr = ecs_ptr_from_json(world, t, &value, "{\"v\": 10}", NULL);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_int(value.v, 10);

    ecs_fini(world);
}

void DeserializeFromJson_struct_u8(void) {
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

    const char *ptr = ecs_ptr_from_json(world, t, &value, "{\"v\": 10}", NULL);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_uint(value.v, 10);

    ecs_fini(world);
}

void DeserializeFromJson_struct_u16(void) {
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

    const char *ptr = ecs_ptr_from_json(world, t, &value, "{\"v\": 10}", NULL);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_uint(value.v, 10);

    ecs_fini(world);
}

void DeserializeFromJson_struct_u32(void) {
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

    const char *ptr = ecs_ptr_from_json(world, t, &value, "{\"v\": 10}", NULL);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_uint(value.v, 10);

    ecs_fini(world);
}

void DeserializeFromJson_struct_u64(void) {
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

    {
        T value = {0};
        const char *ptr = ecs_ptr_from_json(world, t, &value, "{\"v\": 0}", NULL);
        test_assert(ptr != NULL);
        test_assert(ptr[0] == '\0');
        test_uint(value.v, 0);
    }

    {
        T value = {0};
        const char *ptr = ecs_ptr_from_json(world, t, &value, "{\"v\": 10}", NULL);
        test_assert(ptr != NULL);
        test_assert(ptr[0] == '\0');
        test_uint(value.v, 10);
    }

    {
        T value = {0};
        const char *ptr = ecs_ptr_from_json(
            world, t, &value, "{\"v\": 2366700781656087864}", NULL);
        test_assert(ptr != NULL);
        test_assert(ptr[0] == '\0');
        test_uint(value.v, 2366700781656087864);
    }

    {
        T value = {0};
        const char *ptr = ecs_ptr_from_json(
            world, t, &value, "{\"v\": \"2366700781656087864\"}", NULL);
        test_assert(ptr != NULL);
        test_assert(ptr[0] == '\0');
        test_uint(value.v, 2366700781656087864);
    }

    ecs_fini(world);
}

void DeserializeFromJson_struct_uptr(void) {
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

    const char *ptr = ecs_ptr_from_json(world, t, &value, "{\"v\": 10}", NULL);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_uint(value.v, 10);

    ecs_fini(world);
}

void DeserializeFromJson_struct_float(void) {
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

    const char *ptr = ecs_ptr_from_json(world, t, &value, "{\"v\": 10.5}", NULL);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_flt(value.v, 10.5);

    ecs_fini(world);
}

void DeserializeFromJson_struct_double(void) {
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

    const char *ptr = ecs_ptr_from_json(world, t, &value, "{\"v\": 10.5}", NULL);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_flt(value.v, 10.5);

    ecs_fini(world);
}

void DeserializeFromJson_struct_negative_int(void) {
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

    const char *ptr = ecs_ptr_from_json(world, t, &value, "{\"v\": -10}", NULL);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_int(value.v, -10);

    ecs_fini(world);
}

void DeserializeFromJson_struct_negative_float(void) {
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

    const char *ptr = ecs_ptr_from_json(world, t, &value, "{\"v\": -10.5}", NULL);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_flt(value.v, -10.5);

    ecs_fini(world);
}

void DeserializeFromJson_struct_string(void) {
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

    const char *ptr = ecs_ptr_from_json(world, t, &value, "{\"v\": \"Hello World\"}", NULL);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_str(value.v, "Hello World");

    ecs_os_free(value.v);

    ecs_fini(world);
}

void DeserializeFromJson_struct_entity(void) {
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

    const char *ptr = ecs_ptr_from_json(world, t, &value, "{\"v\": \"flecs.core\"}", NULL);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_uint(value.v, EcsFlecsCore);

    ecs_fini(world);
}

void DeserializeFromJson_struct_id(void) {
    typedef struct {
        ecs_id_t v;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "T"}),
        .members = {
            {"v", ecs_id(ecs_id_t)}
        }
    });

    test_assert(t != 0);

    T value = {0};

    const char *ptr = ecs_ptr_from_json(world, t, &value, "{\"v\": \"flecs.core\"}", NULL);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_uint(value.v, EcsFlecsCore);

    ecs_fini(world);
}

void DeserializeFromJson_struct_enum(void) {
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
    const char *ptr = ecs_ptr_from_json(world, t, &value, "{\"v\": \"Red\"}", NULL);
    test_assert(ptr != NULL);
    test_int(value.v, Red);
    }

    {
    T value = {0};
    const char *ptr = ecs_ptr_from_json(world, t, &value, "{\"v\": \"Blue\"}", NULL);
    test_assert(ptr != NULL);
    test_int(value.v, Blue);
    }

    {
    T value = {0};
    const char *ptr = ecs_ptr_from_json(world, t, &value, "{\"v\": \"Green\"}", NULL);
    test_assert(ptr != NULL);
    test_int(value.v, Green);
    }

    {
    ecs_log_set_level(-4);
    T value = {0};
    const char *ptr = ecs_ptr_from_json(world, t, &value, "{\"v\": \"Black\"}", NULL);
    test_assert(ptr == NULL);
    }

    ecs_fini(world);
}

void DeserializeFromJson_struct_bitmask(void) {
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
    const char *ptr = ecs_ptr_from_json(world, t, &value, "{\"v\":\"Lettuce\"}", NULL);
    test_assert(ptr != NULL);
    test_uint(value.v, Lettuce);
    }

    {
    T value = {0};
    const char *ptr = ecs_ptr_from_json(world, t, &value, "{\"v\":\"Lettuce|Bacon\"}", NULL);
    test_assert(ptr != NULL);
    test_uint(value.v, Lettuce|Bacon);
    }

    {
    T value = {0};
    const char *ptr = ecs_ptr_from_json(world, t, &value, "{\"v\":\"Lettuce|Bacon|Tomato|Cheese\"}", NULL);
    test_assert(ptr != NULL);
    test_uint(value.v, Lettuce|Bacon|Tomato|Cheese);
    }

    {
    T value = {0};
    const char *ptr = ecs_ptr_from_json(world, t, &value, "{\"v\":\"BLT\"}", NULL);
    test_assert(ptr != NULL);
    test_uint(value.v, Lettuce|Bacon|Tomato);
    }

    {
    T value = {0};
    const char *ptr = ecs_ptr_from_json(world, t, &value, "{\"v\":0}", NULL);
    test_assert(ptr != NULL);
    test_uint(value.v, 0);
    }

    {
    ecs_log_set_level(-4);
    T value = {0};
    const char *ptr = ecs_ptr_from_json(world, t, &value, "{\"v\":\"Foo\"}", NULL);
    test_assert(ptr == NULL);
    }

    ecs_fini(world);
}

void DeserializeFromJson_struct_i32_i32(void) {
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

    const char *ptr = ecs_ptr_from_json(world, t, &value, "{\"x\": 10, \"y\": 20}", NULL);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_int(value.x, 10);
    test_int(value.y, 20);

    ecs_fini(world);
}

void DeserializeFromJson_struct_nested_i32(void) {
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

    const char *ptr = ecs_ptr_from_json(world, t, &value, "{\"n_1\": {\"x\": 10}}", NULL);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_int(value.n_1.x, 10);

    ecs_fini(world);
}

void DeserializeFromJson_struct_nested_i32_i32(void) {
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

    const char *ptr = ecs_ptr_from_json(world, t, &value, "{\"n_1\": {\"x\": 10, \"y\": 20}}", NULL);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_int(value.n_1.x, 10);
    test_int(value.n_1.y, 20);

    ecs_fini(world);
}

void DeserializeFromJson_struct_2_nested_i32_i32(void) {
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

    const char *ptr = ecs_ptr_from_json(world, t, &value,
        "{\"n_1\": {\"x\": 10, \"y\": 20}, \"n_2\": {\"x\": 30, \"y\": 40}}", 
        NULL);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_int(value.n_1.x, 10);
    test_int(value.n_1.y, 20);
    test_int(value.n_2.x, 30);
    test_int(value.n_2.y, 40);

    ecs_fini(world);
}

void DeserializeFromJson_struct_i32_array_3(void) {
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

    const char *ptr = ecs_ptr_from_json(world, t, &value, "{\"x\": [10, 20, 30]}", NULL);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_int(value.x[0], 10);
    test_int(value.x[1], 20);
    test_int(value.x[2], 30);

    ecs_fini(world);
}

void DeserializeFromJson_struct_struct_i32_array_3(void) {
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

    const char *ptr = ecs_ptr_from_json(world, t, &value, "{\"n_1\": [{\"x\": 10}, {\"x\": 20}, {\"x\": 30}]}", NULL);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_int(value.n_1[0].x, 10);
    test_int(value.n_1[1].x, 20);
    test_int(value.n_1[2].x, 30);

    ecs_fini(world);
}

void DeserializeFromJson_struct_struct_i32_i32_array_3(void) {
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

    const char *ptr = ecs_ptr_from_json(world, t, &value,
        "{\"n_1\": [{\"x\": 10, \"y\": 20}, {\"x\": 30, \"y\": 40}, {\"x\": 50, \"y\": 60}]}"
        , NULL);
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

void DeserializeFromJson_struct_w_array_type_i32_i32(void) {
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

    const char *ptr = ecs_ptr_from_json(world, t, &value, "{\"n_1\": [10, 20]}", NULL);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_int(value.n_1[0], 10);
    test_int(value.n_1[1], 20);

    ecs_fini(world);
}

void DeserializeFromJson_struct_w_2_array_type_i32_i32(void) {
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

    const char *ptr = ecs_ptr_from_json(world, t, &value, "{\"n_1\": [10, 20], \"n_2\": [30, 40]}", NULL);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_int(value.n_1[0], 10);
    test_int(value.n_1[1], 20);
    test_int(value.n_2[0], 30);
    test_int(value.n_2[1], 40);

    ecs_fini(world);
}

void DeserializeFromJson_struct_w_nested_member_i32(void) {
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

    const char *ptr = ecs_ptr_from_json(world, ecs_id(Line), &value,
        "{\"start.x\": 10}", NULL);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_int(value.start.x, 10);

    ecs_fini(world);
}

void DeserializeFromJson_struct_w_2_nested_members_i32(void) {
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

    const char *ptr = ecs_ptr_from_json(world, ecs_id(Line), &value, 
        "{\"start.x\": 10, \"start.y\": 20, \"stop.x\": 30, \"stop.y\": 40}", 
            NULL);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_int(value.start.x, 10);
    test_int(value.start.y, 20);
    test_int(value.stop.x, 30);
    test_int(value.stop.y, 40);

    ecs_fini(world);
}

void DeserializeFromJson_struct_w_nested_members_struct(void) {
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

    const char *ptr = ecs_ptr_from_json(world, ecs_id(TwoLines), &value,
        "{\"left.start\": {\"x\": 10, \"y\": 20}}", NULL);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_int(value.left.start.x, 10);
    test_int(value.left.start.y, 20);

    ecs_fini(world);
}

void DeserializeFromJson_struct_w_2_nested_members_struct(void) {
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

    const char *ptr = ecs_ptr_from_json(world, ecs_id(TwoLines), &value,
        "{\"left.start\": {\"x\": 10, \"y\": 20}, \"right.start\": {\"x\": 30, \"y\": 40}}", 
        NULL);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_int(value.left.start.x, 10);
    test_int(value.left.start.y, 20);

    test_int(value.right.start.x, 30);
    test_int(value.right.start.y, 40);

    ecs_fini(world);
}

void DeserializeFromJson_deser_entity_1_component_1_member(void) {
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
    const char *ptr = ecs_entity_from_json(world, e, 
        "{\"ids\":[[\"Position\"]],\"values\":[{\"x\":10}]}", NULL);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    const Position *pos = ecs_get(world, e, Position);
    test_assert(pos != NULL);
    test_int(pos->x, 10);

    ecs_fini(world);
}

void DeserializeFromJson_deser_entity_1_component_1_member_w_spaces(void) {
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
    const char *ptr = ecs_entity_from_json(world, e, 
        " { \"ids\" : [ [ \"Position\" ] ] , \"values\" : [ { \"x\" : 10 } ] }", NULL);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    const Position *pos = ecs_get(world, e, Position);
    test_assert(pos != NULL);
    test_int(pos->x, 10);

    ecs_fini(world);
}

void DeserializeFromJson_deser_entity_1_component_2_members(void) {
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
    const char *ptr = ecs_entity_from_json(world, e, 
        "{\"ids\":[[\"Position\"]],\"values\":[{\"x\":10, \"y\":20}]}", NULL);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    const Position *pos = ecs_get(world, e, Position);
    test_assert(pos != NULL);
    test_int(pos->x, 10);
    test_int(pos->y, 20);

    ecs_fini(world);
}

void DeserializeFromJson_deser_entity_2_components(void) {
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
    const char *ptr = ecs_entity_from_json(world, e, 
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

void DeserializeFromJson_deser_entity_1_component_composite_member(void) {
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
    const char *ptr = ecs_entity_from_json(world, e, 
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

void DeserializeFromJson_deser_entity_1_component_nested_member(void) {
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
    const char *ptr = ecs_entity_from_json(world, e, 
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

void DeserializeFromJson_deser_entity_1_pair(void) {
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
    const char *ptr = ecs_entity_from_json(world, e, 
        "{\"ids\":[[\"Position\", \"Tgt\"]],\"values\":[{\"x\":10, \"y\":20}]}", NULL);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    const Position *pos = ecs_get_pair(world, e, Position, Tgt);
    test_assert(pos != NULL);
    test_int(pos->x, 10);
    test_int(pos->y, 20);

    ecs_fini(world);
}

void DeserializeFromJson_deser_entity_2_pairs(void) {
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
    const char *ptr = ecs_entity_from_json(world, e, 
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

void DeserializeFromJson_deser_entity_empty(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t e = ecs_new_id(world);
    const char *r = ecs_entity_from_json(world, e, "{}", NULL);
    test_str(r, "");

    ecs_fini(world);
}

void DeserializeFromJson_deser_entity_w_path(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t e = ecs_new_id(world);
    const char *r = ecs_entity_from_json(world, e, "{\"path\":\"ent\"}", NULL);
    test_str(r, "");
    test_str(ecs_get_name(world, e), "ent");

    ecs_fini(world);
}

void DeserializeFromJson_deser_entity_w_path_and_ids(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new_id(world);
    const char *r = ecs_entity_from_json(world, e, 
        "{\"path\":\"ent\", \"ids\":[[\"Position\"]]}", NULL);
    test_str(r, "");
    test_str(ecs_get_name(world, e), "ent");
    test_assert(ecs_has(world, e, Position));

    ecs_fini(world);
}

void DeserializeFromJson_deser_entity_w_path_and_ids_and_values(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_struct(world, {
        .entity = ecs_id(Position),
        .members = {
            { "x", .type = ecs_id(ecs_i32_t) },
            { "y", .type = ecs_id(ecs_i32_t) }
        }
    });

    ecs_entity_t e = ecs_new_id(world);
    const char *r = ecs_entity_from_json(world, e, 
        "{\"path\":\"ent\", \"ids\":[[\"Position\"]], \"values\":[{\"x\":10, \"y\":20}]}", NULL);
    test_str(r, "");
    test_str(ecs_get_name(world, e), "ent");
    test_assert(ecs_has(world, e, Position));

    const Position *p = ecs_get(world, e, Position);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void DeserializeFromJson_deser_entity_w_ids(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new_id(world);
    const char *r = ecs_entity_from_json(world, e, 
        "{\"ids\":[[\"Position\"]]}", NULL);
    test_str(r, "");
    test_str(ecs_get_name(world, e), NULL);
    test_assert(ecs_has(world, e, Position));

    ecs_fini(world);
}

void DeserializeFromJson_ser_deser_mini(void) {
    ecs_world_t *world = ecs_mini();

    char *json = ecs_world_to_json(world, NULL);
    test_assert(json != NULL);

    const char *r = ecs_world_from_json(world, json, NULL);
    test_str(r, "");
    ecs_os_free(json);

    ecs_fini(world);
}

void DeserializeFromJson_ser_deser_init(void) {
    ecs_world_t *world = ecs_init();

    char *json = ecs_world_to_json(world, NULL);
    test_assert(json != NULL);

    const char *r = ecs_world_from_json(world, json, NULL);
    test_str(r, "");
    ecs_os_free(json);

    ecs_fini(world);
}

void DeserializeFromJson_ser_deser_mini_serialize_builtin(void) {
    ecs_world_t *world = ecs_mini();

    ecs_world_to_json_desc_t desc = {0};
    desc.serialize_builtin = true;
    char *json = ecs_world_to_json(world, &desc);
    test_assert(json != NULL);

    const char *r = ecs_world_from_json(world, json, NULL);
    test_str(r, "");
    ecs_os_free(json);

    ecs_fini(world);
}

void DeserializeFromJson_ser_deser_mini_serialize_modules(void) {
    ecs_world_t *world = ecs_mini();

    ecs_world_to_json_desc_t desc = {0};
    desc.serialize_modules = true;
    char *json = ecs_world_to_json(world, &desc);
    test_assert(json != NULL);

    const char *r = ecs_world_from_json(world, json, NULL);
    test_str(r, "");
    ecs_os_free(json);

    ecs_fini(world);
}

void DeserializeFromJson_ser_deser_mini_serialize_builtin_modules(void) {
    ecs_world_t *world = ecs_mini();

    ecs_world_to_json_desc_t desc = {0};
    desc.serialize_builtin = true;
    desc.serialize_modules = true;
    char *json = ecs_world_to_json(world, &desc);
    test_assert(json != NULL);

    const char *r = ecs_world_from_json(world, json, NULL);
    test_str(r, "");
    ecs_os_free(json);

    ecs_fini(world);
}

void DeserializeFromJson_ser_deser_init_serialize_builtin(void) {
    ecs_world_t *world = ecs_init();

    ecs_world_to_json_desc_t desc = {0};
    desc.serialize_builtin = true;
    char *json = ecs_world_to_json(world, &desc);
    test_assert(json != NULL);

    const char *r = ecs_world_from_json(world, json, NULL);
    test_str(r, "");
    ecs_os_free(json);

    ecs_fini(world);
}

void DeserializeFromJson_ser_deser_init_serialize_modules(void) {
    ecs_world_t *world = ecs_init();

    ecs_world_to_json_desc_t desc = {0};
    desc.serialize_modules = true;
    char *json = ecs_world_to_json(world, &desc);
    test_assert(json != NULL);

    const char *r = ecs_world_from_json(world, json, NULL);
    test_str(r, "");
    ecs_os_free(json);

    ecs_fini(world);
}

void DeserializeFromJson_ser_deser_init_serialize_builtin_modules(void) {
    ecs_world_t *world = ecs_init();

    ecs_world_to_json_desc_t desc = {0};
    desc.serialize_builtin = true;
    desc.serialize_modules = true;
    char *json = ecs_world_to_json(world, &desc);
    test_assert(json != NULL);

    const char *r = ecs_world_from_json(world, json, NULL);
    test_str(r, "");
    ecs_os_free(json);

    ecs_fini(world);
}

void DeserializeFromJson_ser_deser_new_world(void) {
    ecs_world_t *world = ecs_init();

    ecs_world_to_json_desc_t desc = {0};
    desc.serialize_builtin = true;
    desc.serialize_modules = true;
    char *json = ecs_world_to_json(world, &desc);
    test_assert(json != NULL);

    ecs_fini(world);
    world = ecs_init();

    const char *r = ecs_world_from_json(world, json, NULL);
    test_str(r, "");
    ecs_os_free(json);

    ecs_fini(world);
}

void DeserializeFromJson_ser_deser_new_world_1_entity_w_tag(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);

    ecs_entity_t e = ecs_new(world, Tag);
    test_assert(e != 0);

    char *json = ecs_world_to_json(world, NULL);
    test_assert(json != NULL);

    ecs_fini(world);
    world = ecs_init();
    ECS_TAG_DEFINE(world, Tag); /* pin Tag */

    const char *r = ecs_world_from_json(world, json, NULL);
    test_str(r, "");
    ecs_os_free(json);

    test_assert(ecs_is_alive(world, e));
    test_assert(ecs_is_alive(world, Tag));
    test_assert(ecs_has(world, e, Tag));
    test_str(ecs_get_name(world, Tag), "Tag");

    ecs_fini(world);
}

void DeserializeFromJson_ser_deser_new_world_1_entity_w_component(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new(world, Position);
    test_assert(e != 0);

    char *json = ecs_world_to_json(world, NULL);
    test_assert(json != NULL);

    ecs_fini(world);
    world = ecs_init();
    ECS_COMPONENT_DEFINE(world, Position); /* pin Position id */

    const char *r = ecs_world_from_json(world, json, NULL);
    test_str(r, "");
    ecs_os_free(json);

    test_assert(ecs_is_alive(world, e));
    test_assert(ecs_is_alive(world, ecs_id(Position)));
    test_assert(ecs_has(world, e, Position));
    test_str(ecs_get_name(world, ecs_id(Position)), "Position");

    const Position *p = ecs_get(world, e, Position);
    test_assert(p != NULL);

    ecs_fini(world);
}

void DeserializeFromJson_ser_deser_new_world_1_entity_w_component_meta(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ecs_struct(world, {
        .entity = ecs_id(Position),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)},
        }
    });

    ecs_entity_t e = ecs_set(world, 0, Position, {10, 20});
    test_assert(e != 0);

    char *json = ecs_world_to_json(world, NULL);
    test_assert(json != NULL);

    ecs_fini(world);
    world = ecs_init();

    ECS_COMPONENT_DEFINE(world, Position); /* pin Position id */
    ecs_struct(world, {
        .entity = ecs_id(Position),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)},
        }
    });

    const char *r = ecs_world_from_json(world, json, NULL);
    test_str(r, "");
    ecs_os_free(json);

    test_assert(ecs_is_alive(world, e));
    test_assert(ecs_is_alive(world, ecs_id(Position)));
    test_assert(ecs_has(world, e, Position));
    test_str(ecs_get_name(world, ecs_id(Position)), "Position");

    const Position *p = ecs_get(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void DeserializeFromJson_ser_deser_new_world_1_named_entity_w_tag(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);

    ecs_entity_t e = ecs_new_entity(world, "foo");
    ecs_add(world, e, Tag);
    test_assert(e != 0);

    char *json = ecs_world_to_json(world, NULL);
    test_assert(json != NULL);

    ecs_fini(world);
    world = ecs_init();
    ECS_TAG_DEFINE(world, Tag); /* pin Tag */

    const char *r = ecs_world_from_json(world, json, NULL);
    test_str(r, "");
    ecs_os_free(json);

    e = ecs_lookup_fullpath(world, "foo");
    test_assert(e != 0);
    test_assert(ecs_is_alive(world, Tag));
    test_assert(ecs_has(world, e, Tag));
    test_str(ecs_get_name(world, Tag), "Tag");

    ecs_fini(world);
}

void DeserializeFromJson_ser_deser_new_world_1_named_entity_w_component(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new_entity(world, "foo");
    ecs_add(world, e, Position);
    test_assert(e != 0);

    char *json = ecs_world_to_json(world, NULL);
    test_assert(json != NULL);

    ecs_fini(world);
    world = ecs_init();
    ECS_COMPONENT_DEFINE(world, Position); /* pin Position id */

    const char *r = ecs_world_from_json(world, json, NULL);
    test_str(r, "");
    ecs_os_free(json);

    e = ecs_lookup_fullpath(world, "foo");
    test_assert(e != 0);
    test_assert(ecs_is_alive(world, e));
    test_assert(ecs_is_alive(world, ecs_id(Position)));
    test_assert(ecs_has(world, e, Position));
    test_str(ecs_get_name(world, ecs_id(Position)), "Position");

    const Position *p = ecs_get(world, e, Position);
    test_assert(p != NULL);

    ecs_fini(world);
}

void DeserializeFromJson_ser_deser_new_world_1_named_entity_w_component_meta(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ecs_struct(world, {
        .entity = ecs_id(Position),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)},
        }
    });

    ecs_entity_t e = ecs_new_entity(world, "foo");
    ecs_set(world, e, Position, {10, 20});
    test_assert(e != 0);

    char *json = ecs_world_to_json(world, NULL);
    test_assert(json != NULL);

    ecs_fini(world);
    world = ecs_init();

    ECS_COMPONENT_DEFINE(world, Position); /* pin Position id */
    ecs_struct(world, {
        .entity = ecs_id(Position),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)},
        }
    });

    const char *r = ecs_world_from_json(world, json, NULL);
    test_str(r, "");
    ecs_os_free(json);

    e = ecs_lookup_fullpath(world, "foo");
    test_assert(e != 0);
    test_assert(ecs_is_alive(world, e));
    test_assert(ecs_is_alive(world, ecs_id(Position)));
    test_assert(ecs_has(world, e, Position));
    test_str(ecs_get_name(world, ecs_id(Position)), "Position");

    const Position *p = ecs_get(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void DeserializeFromJson_ser_deser_new_world_1_entity_w_tag_serialize_all(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);

    ecs_entity_t e = ecs_new(world, Tag);
    test_assert(e != 0);

    ecs_world_to_json_desc_t desc = {0};
    desc.serialize_builtin = true;
    desc.serialize_modules = true;
    char *json = ecs_world_to_json(world, &desc);
    test_assert(json != NULL);

    ecs_fini(world);
    world = ecs_init();
    ECS_TAG_DEFINE(world, Tag); /* pin Tag */

    const char *r = ecs_world_from_json(world, json, NULL);
    test_str(r, "");
    ecs_os_free(json);

    test_assert(ecs_is_alive(world, e));
    test_assert(ecs_is_alive(world, Tag));
    test_assert(ecs_has(world, e, Tag));
    test_str(ecs_get_name(world, Tag), "Tag");

    ecs_fini(world);
}

void DeserializeFromJson_ser_deser_new_world_1_entity_w_component_serialize_all(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new(world, Position);
    test_assert(e != 0);

    ecs_world_to_json_desc_t desc = {0};
    desc.serialize_builtin = true;
    desc.serialize_modules = true;
    char *json = ecs_world_to_json(world, &desc);
    test_assert(json != NULL);

    ecs_fini(world);
    world = ecs_init();
    ECS_COMPONENT_DEFINE(world, Position); /* pin Position id */

    const char *r = ecs_world_from_json(world, json, NULL);
    test_str(r, "");
    ecs_os_free(json);

    test_assert(ecs_is_alive(world, e));
    test_assert(ecs_is_alive(world, ecs_id(Position)));
    test_assert(ecs_has(world, e, Position));
    test_str(ecs_get_name(world, ecs_id(Position)), "Position");

    const Position *p = ecs_get(world, e, Position);
    test_assert(p != NULL);

    ecs_fini(world);
}

void DeserializeFromJson_ser_deser_new_world_1_entity_w_component_meta_serialize_all(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ecs_struct(world, {
        .entity = ecs_id(Position),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)},
        }
    });

    ecs_entity_t e = ecs_set(world, 0, Position, {10, 20});
    test_assert(e != 0);

    ecs_world_to_json_desc_t desc = {0};
    desc.serialize_builtin = true;
    desc.serialize_modules = true;
    char *json = ecs_world_to_json(world, &desc);
    test_assert(json != NULL);

    ecs_fini(world);
    world = ecs_init();

    ECS_COMPONENT_DEFINE(world, Position); /* pin Position id */
    ecs_struct(world, {
        .entity = ecs_id(Position),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)},
        }
    });

    const char *r = ecs_world_from_json(world, json, NULL);
    test_str(r, "");
    ecs_os_free(json);

    test_assert(ecs_is_alive(world, e));
    test_assert(ecs_is_alive(world, ecs_id(Position)));
    test_assert(ecs_has(world, e, Position));
    test_str(ecs_get_name(world, ecs_id(Position)), "Position");

    const Position *p = ecs_get(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void DeserializeFromJson_ser_deser_new_world_1_named_entity_w_tag_serialize_all(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);

    ecs_entity_t e = ecs_new_entity(world, "foo");
    ecs_add(world, e, Tag);
    test_assert(e != 0);

    ecs_world_to_json_desc_t desc = {0};
    desc.serialize_builtin = true;
    desc.serialize_modules = true;
    char *json = ecs_world_to_json(world, &desc);
    test_assert(json != NULL);

    ecs_fini(world);
    world = ecs_init();
    ECS_TAG_DEFINE(world, Tag); /* pin Tag */

    const char *r = ecs_world_from_json(world, json, NULL);
    test_str(r, "");
    ecs_os_free(json);

    e = ecs_lookup_fullpath(world, "foo");
    test_assert(e != 0);
    test_assert(ecs_is_alive(world, Tag));
    test_assert(ecs_has(world, e, Tag));
    test_str(ecs_get_name(world, Tag), "Tag");

    ecs_fini(world);
}

void DeserializeFromJson_ser_deser_new_world_1_named_entity_w_component_serialize_all(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new_entity(world, "foo");
    ecs_add(world, e, Position);
    test_assert(e != 0);

    ecs_world_to_json_desc_t desc = {0};
    desc.serialize_builtin = true;
    desc.serialize_modules = true;
    char *json = ecs_world_to_json(world, &desc);
    test_assert(json != NULL);

    ecs_fini(world);
    world = ecs_init();
    ECS_COMPONENT_DEFINE(world, Position); /* pin Position id */

    const char *r = ecs_world_from_json(world, json, NULL);
    test_str(r, "");
    ecs_os_free(json);

    e = ecs_lookup_fullpath(world, "foo");
    test_assert(e != 0);
    test_assert(ecs_is_alive(world, e));
    test_assert(ecs_is_alive(world, ecs_id(Position)));
    test_assert(ecs_has(world, e, Position));
    test_str(ecs_get_name(world, ecs_id(Position)), "Position");

    const Position *p = ecs_get(world, e, Position);
    test_assert(p != NULL);

    ecs_fini(world);
}

void DeserializeFromJson_ser_deser_new_world_1_named_entity_w_component_meta_serialize_all(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ecs_struct(world, {
        .entity = ecs_id(Position),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)},
        }
    });

    ecs_entity_t e = ecs_new_entity(world, "foo");
    ecs_set(world, e, Position, {10, 20});
    test_assert(e != 0);

    ecs_world_to_json_desc_t desc = {0};
    desc.serialize_builtin = true;
    desc.serialize_modules = true;
    char *json = ecs_world_to_json(world, &desc);
    test_assert(json != NULL);

    ecs_fini(world);
    world = ecs_init();

    ECS_COMPONENT_DEFINE(world, Position); /* pin Position id */
    ecs_struct(world, {
        .entity = ecs_id(Position),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)},
        }
    });

    const char *r = ecs_world_from_json(world, json, NULL);
    test_str(r, "");
    ecs_os_free(json);

    e = ecs_lookup_fullpath(world, "foo");
    test_assert(e != 0);
    test_assert(ecs_is_alive(world, e));
    test_assert(ecs_is_alive(world, ecs_id(Position)));
    test_assert(ecs_has(world, e, Position));
    test_str(ecs_get_name(world, ecs_id(Position)), "Position");

    const Position *p = ecs_get(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void DeserializeFromJson_ser_deser_new_world_3_entities_w_component_meta(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ecs_struct(world, {
        .entity = ecs_id(Position),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)},
        }
    });

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_entity_t e2 = ecs_new_entity(world, "e2");
    ecs_entity_t e3 = ecs_new_entity(world, "e3");

    ecs_set(world, e1, Position, {10, 20});
    ecs_set(world, e2, Position, {30, 40});
    ecs_set(world, e3, Position, {50, 60});

    char *json = ecs_world_to_json(world, NULL);
    test_assert(json != NULL);

    ecs_fini(world);
    world = ecs_init();

    ECS_COMPONENT_DEFINE(world, Position); /* pin Position id */
    ecs_struct(world, {
        .entity = ecs_id(Position),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)},
        }
    });

    const char *r = ecs_world_from_json(world, json, NULL);
    test_str(r, "");
    ecs_os_free(json);

    e1 = ecs_lookup_fullpath(world, "e1");
    e2 = ecs_lookup_fullpath(world, "e2");
    e3 = ecs_lookup_fullpath(world, "e3");

    test_assert(e1 != 0);
    test_assert(e2 != 0);
    test_assert(e3 != 0);

    test_assert(ecs_has(world, e1, Position));
    test_assert(ecs_has(world, e2, Position));
    test_assert(ecs_has(world, e3, Position));

    {
        const Position *p = ecs_get(world, e1, Position);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 20);
    }
    {
        const Position *p = ecs_get(world, e2, Position);
        test_assert(p != NULL);
        test_int(p->x, 30);
        test_int(p->y, 40);
    }
    {
        const Position *p = ecs_get(world, e3, Position);
        test_assert(p != NULL);
        test_int(p->x, 50);
        test_int(p->y, 60);
    }

    ecs_fini(world);
}

void DeserializeFromJson_ser_deser_new_world_3_entities_w_2_components_meta(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ecs_struct(world, {
        .entity = ecs_id(Position),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)},
        }
    });

    ECS_COMPONENT(world, Mass);
    ecs_struct(world, {
        .entity = ecs_id(Mass),
        .members = {
            {"value", ecs_id(ecs_i32_t)},
        }
    });

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_entity_t e2 = ecs_new_entity(world, "e2");
    ecs_entity_t e3 = ecs_new_entity(world, "e3");

    ecs_set(world, e1, Position, {10, 20});
    ecs_set(world, e2, Position, {30, 40});
    ecs_set(world, e3, Position, {50, 60});

    ecs_set(world, e1, Mass, {100});
    ecs_set(world, e2, Mass, {200});
    ecs_set(world, e3, Mass, {300});

    char *json = ecs_world_to_json(world, NULL);
    test_assert(json != NULL);

    ecs_fini(world);
    world = ecs_init();

    ECS_COMPONENT_DEFINE(world, Position); /* pin Position id */
    ecs_struct(world, {
        .entity = ecs_id(Position),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)},
        }
    });

    ECS_COMPONENT_DEFINE(world, Mass); /* pin Mass id */
    ecs_struct(world, {
        .entity = ecs_id(Mass),
        .members = {
            {"value", ecs_id(ecs_i32_t)},
        }
    });

    const char *r = ecs_world_from_json(world, json, NULL);
    test_str(r, "");
    ecs_os_free(json);

    e1 = ecs_lookup_fullpath(world, "e1");
    e2 = ecs_lookup_fullpath(world, "e2");
    e3 = ecs_lookup_fullpath(world, "e3");

    test_assert(e1 != 0);
    test_assert(e2 != 0);
    test_assert(e3 != 0);

    test_assert(ecs_has(world, e1, Position));
    test_assert(ecs_has(world, e2, Position));
    test_assert(ecs_has(world, e3, Position));

    {
        const Position *p = ecs_get(world, e1, Position);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 20);
    }
    {
        const Position *p = ecs_get(world, e2, Position);
        test_assert(p != NULL);
        test_int(p->x, 30);
        test_int(p->y, 40);
    }
    {
        const Position *p = ecs_get(world, e3, Position);
        test_assert(p != NULL);
        test_int(p->x, 50);
        test_int(p->y, 60);
    }

    test_assert(ecs_has(world, e1, Mass));
    test_assert(ecs_has(world, e2, Mass));
    test_assert(ecs_has(world, e3, Mass));

    {
        const Mass *m = ecs_get(world, e1, Mass);
        test_assert(m != NULL);
        test_int(m->value, 100);
    }
    {
        const Mass *m = ecs_get(world, e2, Mass);
        test_assert(m != NULL);
        test_int(m->value, 200);
    }
    {
        const Mass *m = ecs_get(world, e3, Mass);
        test_assert(m != NULL);
        test_int(m->value, 300);
    }

    ecs_fini(world);
}

void DeserializeFromJson_ser_deser_new_world_4_entities_2_tables_w_component_meta(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ecs_struct(world, {
        .entity = ecs_id(Position),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)},
        }
    });

    ECS_TAG(world, Tag);

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_entity_t e2 = ecs_new_entity(world, "e2");
    ecs_entity_t e3 = ecs_new_entity(world, "e3");
    ecs_entity_t e4 = ecs_new_entity(world, "e4");

    ecs_set(world, e1, Position, {10, 20});
    ecs_set(world, e2, Position, {30, 40});
    ecs_set(world, e3, Position, {50, 60});
    ecs_set(world, e4, Position, {70, 80});

    ecs_add(world, e3, Tag);
    ecs_add(world, e4, Tag);

    char *json = ecs_world_to_json(world, NULL);
    test_assert(json != NULL);

    ecs_fini(world);
    world = ecs_init();

    ECS_COMPONENT_DEFINE(world, Position); /* pin Position id */
    ecs_struct(world, {
        .entity = ecs_id(Position),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)},
        }
    });

    ECS_TAG_DEFINE(world, Tag); /* pin Tag id */

    const char *r = ecs_world_from_json(world, json, NULL);
    test_str(r, "");
    ecs_os_free(json);

    e1 = ecs_lookup_fullpath(world, "e1");
    e2 = ecs_lookup_fullpath(world, "e2");
    e3 = ecs_lookup_fullpath(world, "e3");
    e4 = ecs_lookup_fullpath(world, "e4");

    test_assert(e1 != 0);
    test_assert(e2 != 0);
    test_assert(e3 != 0);
    test_assert(e4 != 0);

    test_assert(ecs_has(world, e1, Position));
    test_assert(ecs_has(world, e2, Position));
    test_assert(ecs_has(world, e3, Position));
    test_assert(ecs_has(world, e4, Position));

    test_assert(!ecs_has(world, e1, Tag));
    test_assert(!ecs_has(world, e2, Tag));
    test_assert(ecs_has(world, e3, Tag));
    test_assert(ecs_has(world, e4, Tag));

    {
        const Position *p = ecs_get(world, e1, Position);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 20);
    }
    {
        const Position *p = ecs_get(world, e2, Position);
        test_assert(p != NULL);
        test_int(p->x, 30);
        test_int(p->y, 40);
    }
    {
        const Position *p = ecs_get(world, e3, Position);
        test_assert(p != NULL);
        test_int(p->x, 50);
        test_int(p->y, 60);
    }
    {
        const Position *p = ecs_get(world, e4, Position);
        test_assert(p != NULL);
        test_int(p->x, 70);
        test_int(p->y, 80);
    }

    ecs_fini(world);
}

typedef struct EntityType {
    ecs_entity_t a;
    ecs_entity_t b;
} EntityType;

void DeserializeFromJson_ser_deser_new_world_component_w_anon_entity_member(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, EntityType);
    ecs_struct(world, {
        .entity = ecs_id(EntityType),
        .members = {
            {"a", ecs_id(ecs_entity_t)},
            {"b", ecs_id(ecs_entity_t)}
        }
    });

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_entity_t a1 = ecs_new_id(world);
    ecs_entity_t a2 = ecs_new_id(world);
    ecs_set(world, e1, EntityType, {a1, a2});

    ecs_entity_t e2 = ecs_new_entity(world, "e2");
    ecs_add_id(world, e2, a1);

    ecs_entity_t e3 = ecs_new_entity(world, "e3");
    ecs_add_id(world, e3, a2);

    char *json = ecs_world_to_json(world, NULL);
    test_assert(json != NULL);

    ecs_fini(world);
    world = ecs_init();

    ECS_COMPONENT_DEFINE(world, EntityType); /* pin EntityType id */
    ecs_struct(world, {
        .entity = ecs_id(EntityType),
        .members = {
            {"a", ecs_id(ecs_entity_t)},
            {"b", ecs_id(ecs_entity_t)}
        }
    });

    const char *r = ecs_world_from_json(world, json, NULL);
    test_str(r, "");
    ecs_os_free(json);

    e1 = ecs_lookup_fullpath(world, "e1");
    e2 = ecs_lookup_fullpath(world, "e2");
    e3 = ecs_lookup_fullpath(world, "e3");

    test_assert(e1 != 0);
    test_assert(e2 != 0);
    test_assert(e3 != 0);

    test_assert(ecs_has(world, e1, EntityType));

    const EntityType *et = ecs_get(world, e1, EntityType);
    test_assert(et != NULL);
    test_assert(et->a != 0);
    test_assert(et->b != 0);
    test_assert(et->a != et->b);
    test_assert(ecs_get_table(world, et->a) == NULL);
    test_assert(ecs_get_table(world, et->b) == NULL);

    test_assert(ecs_has_id(world, e2, et->a));
    test_assert(!ecs_has_id(world, e3, et->a));
    test_assert(!ecs_has_id(world, e2, et->b));
    test_assert(ecs_has_id(world, e3, et->b));

    ecs_fini(world);
}

void DeserializeFromJson_ser_deser_new_world_component_w_named_entity_member(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, EntityType);
    ecs_struct(world, {
        .entity = ecs_id(EntityType),
        .members = {
            {"a", ecs_id(ecs_entity_t)},
            {"b", ecs_id(ecs_entity_t)}
        }
    });

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_entity_t n1 = ecs_new_entity(world, "n1");
    ecs_entity_t n2 = ecs_new_entity(world, "n2");
    ecs_set(world, e1, EntityType, {n1, n2});

    ecs_entity_t e2 = ecs_new_entity(world, "e2");
    ecs_add_id(world, e2, n1);

    ecs_entity_t e3 = ecs_new_entity(world, "e3");
    ecs_add_id(world, e3, n2);

    char *json = ecs_world_to_json(world, NULL);
    test_assert(json != NULL);

    ecs_fini(world);
    world = ecs_init();

    ECS_COMPONENT_DEFINE(world, EntityType); /* pin EntityType id */
    ecs_struct(world, {
        .entity = ecs_id(EntityType),
        .members = {
            {"a", ecs_id(ecs_entity_t)},
            {"b", ecs_id(ecs_entity_t)}
        }
    });

    const char *r = ecs_world_from_json(world, json, NULL);
    test_str(r, "");
    ecs_os_free(json);

    e1 = ecs_lookup_fullpath(world, "e1");
    e2 = ecs_lookup_fullpath(world, "e2");
    e3 = ecs_lookup_fullpath(world, "e3");

    test_assert(e1 != 0);
    test_assert(e2 != 0);
    test_assert(e3 != 0);

    test_assert(ecs_has(world, e1, EntityType));

    const EntityType *et = ecs_get(world, e1, EntityType);
    test_assert(et != NULL);
    test_assert(et->a != 0);
    test_assert(et->b != 0);
    test_assert(et->a != et->b);
    test_str(ecs_get_name(world, et->a), "n1");
    test_str(ecs_get_name(world, et->b), "n2");

    test_assert(ecs_has_id(world, e2, et->a));
    test_assert(!ecs_has_id(world, e3, et->a));
    test_assert(!ecs_has_id(world, e2, et->b));
    test_assert(ecs_has_id(world, e3, et->b));

    ecs_fini(world);
}

void DeserializeFromJson_ser_deser_new_world_component_w_anon_and_named_entity_member(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, EntityType);
    ecs_struct(world, {
        .entity = ecs_id(EntityType),
        .members = {
            {"a", ecs_id(ecs_entity_t)},
            {"b", ecs_id(ecs_entity_t)}
        }
    });

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_entity_t n1 = ecs_new_entity(world, "n1");
    ecs_entity_t a1 = ecs_new_id(world);
    ecs_set(world, e1, EntityType, {n1, a1});

    ecs_entity_t e2 = ecs_new_entity(world, "e2");
    ecs_add_id(world, e2, n1);

    ecs_entity_t e3 = ecs_new_entity(world, "e3");
    ecs_add_id(world, e3, a1);

    char *json = ecs_world_to_json(world, NULL);
    test_assert(json != NULL);

    ecs_fini(world);
    world = ecs_init();

    ECS_COMPONENT_DEFINE(world, EntityType); /* pin EntityType id */
    ecs_struct(world, {
        .entity = ecs_id(EntityType),
        .members = {
            {"a", ecs_id(ecs_entity_t)},
            {"b", ecs_id(ecs_entity_t)}
        }
    });

    const char *r = ecs_world_from_json(world, json, NULL);
    test_str(r, "");
    ecs_os_free(json);

    e1 = ecs_lookup_fullpath(world, "e1");
    e2 = ecs_lookup_fullpath(world, "e2");
    e3 = ecs_lookup_fullpath(world, "e3");

    test_assert(e1 != 0);
    test_assert(e2 != 0);
    test_assert(e3 != 0);

    test_assert(ecs_has(world, e1, EntityType));

    const EntityType *et = ecs_get(world, e1, EntityType);
    test_assert(et != NULL);
    test_assert(et->a != 0);
    test_assert(et->b != 0);
    test_assert(et->a != et->b);
    test_str(ecs_get_name(world, et->a), "n1");
    test_assert(ecs_get_table(world, et->b) == NULL);

    test_assert(ecs_has_id(world, e2, et->a));
    test_assert(!ecs_has_id(world, e3, et->a));
    test_assert(!ecs_has_id(world, e2, et->b));
    test_assert(ecs_has_id(world, e3, et->b));

    ecs_fini(world);
}

void DeserializeFromJson_ser_deser_new_world_component_w_anon_entity_with_self(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t e1 = ecs_new_id(world);
    ecs_add_id(world, e1, e1);

    ecs_entity_t e2 = ecs_new_entity(world, "e2");
    ecs_add_id(world, e2, e1);

    char *json = ecs_world_to_json(world, NULL);
    test_assert(json != NULL);

    ecs_fini(world);
    world = ecs_init();

    const char *r = ecs_world_from_json(world, json, NULL);
    test_str(r, "");
    ecs_os_free(json);

    e2 = ecs_lookup_fullpath(world, "e2");
    test_assert(e2 != 0);

    const ecs_type_t *type = ecs_get_type(world, e2);
    test_assert(type != NULL);
    test_int(type->count, 2);

    ecs_entity_t e = type->array[0];
    test_assert(e != 0);
    test_assert(ecs_is_valid(world, e));
    test_assert(ecs_has_id(world, e, e));

    ecs_fini(world);
}

void DeserializeFromJson_ser_deser_new_world_component_w_named_entity_with_self(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_add_id(world, e1, e1);

    ecs_entity_t e2 = ecs_new_entity(world, "e2");
    ecs_add_id(world, e2, e1);

    char *json = ecs_world_to_json(world, NULL);
    test_assert(json != NULL);

    ecs_fini(world);
    world = ecs_init();

    const char *r = ecs_world_from_json(world, json, NULL);
    test_str(r, "");
    ecs_os_free(json);

    e1 = ecs_lookup_fullpath(world, "e1");
    test_assert(e1 != 0);

    e2 = ecs_lookup_fullpath(world, "e2");
    test_assert(e2 != 0);

    test_assert(ecs_has_id(world, e2, e1));
    test_assert(ecs_has_id(world, e1, e1));

    ecs_fini(world);
}

void DeserializeFromJson_ser_deser_new_world_2_entities_w_anon_parent(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_entity_t parent = ecs_new_id(world);
    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_entity_t e2 = ecs_new_entity(world, "e2");
    ecs_add_pair(world, e1, EcsChildOf, parent);
    ecs_add_pair(world, e2, EcsChildOf, parent);
    ecs_add(world, e1, TagA);
    ecs_add(world, e2, TagB);

    ecs_entity_t foo = ecs_new_entity(world, "foo");
    ecs_add_pair(world, foo, TagA, e1);
    ecs_add_pair(world, foo, TagB, e2);

    char *json = ecs_world_to_json(world, NULL);
    test_assert(json != NULL);

    ecs_fini(world);
    world = ecs_init();

    const char *r = ecs_world_from_json(world, json, NULL);
    test_str(r, "");
    ecs_os_free(json);

    foo = ecs_lookup_fullpath(world, "foo");
    test_assert(foo != 0);

    e1 = ecs_get_target(world, foo, TagA, 0);
    test_assert(e1 != 0);
    test_str(ecs_get_name(world, e1), "e1");
    test_assert(ecs_has(world, e1, TagA));

    e2 = ecs_get_target(world, foo, TagB, 0);
    test_assert(e2 != 0);
    test_str(ecs_get_name(world, e2), "e2");
    test_assert(ecs_has(world, e2, TagB));

    test_assert(ecs_get_target(world, e1, EcsChildOf, 0) != 0);
    test_assert(ecs_get_target(world, e2, EcsChildOf, 0) != 0);
    test_assert(ecs_get_target(world, e1, EcsChildOf, 0) ==
        ecs_get_target(world, e2, EcsChildOf, 0));

    ecs_fini(world);
}

void DeserializeFromJson_ser_deser_new_world_2_entities_w_named_parent(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_entity_t parent = ecs_new_entity(world, "parent");
    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_entity_t e2 = ecs_new_entity(world, "e2");
    ecs_add_pair(world, e1, EcsChildOf, parent);
    ecs_add_pair(world, e2, EcsChildOf, parent);
    ecs_add(world, e1, TagA);
    ecs_add(world, e2, TagB);

    char *json = ecs_world_to_json(world, NULL);
    test_assert(json != NULL);

    ecs_fini(world);
    world = ecs_init();

    const char *r = ecs_world_from_json(world, json, NULL);
    test_str(r, "");
    ecs_os_free(json);

    parent = ecs_lookup_fullpath(world, "parent");
    test_assert(parent != 0);
    test_str(ecs_get_name(world, parent), "parent");

    e1 = ecs_lookup_fullpath(world, "parent.e1");
    test_assert(e1 != 0);
    test_str(ecs_get_name(world, e1), "e1");

    e2 = ecs_lookup_fullpath(world, "parent.e2");
    test_assert(e2 != 0);
    test_str(ecs_get_name(world, e2), "e2");

    test_assert(ecs_get_target(world, e1, EcsChildOf, 0) == parent);
    test_assert(ecs_get_target(world, e2, EcsChildOf, 0) == parent);

    ecs_fini(world);
}

void DeserializeFromJson_ser_deser_new_world_2_entities_w_anon_parent_w_cycle(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_entity_t parent = ecs_new_id(world);
    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_entity_t e2 = ecs_new_entity(world, "e2");
    ecs_add_pair(world, e1, EcsChildOf, parent);
    ecs_add_pair(world, e2, EcsChildOf, parent);
    ecs_add(world, e1, TagA);
    ecs_add(world, e2, TagB);

    ecs_entity_t foo = ecs_new_entity(world, "foo");
    ecs_add_pair(world, foo, TagA, e1);
    ecs_add_pair(world, foo, TagB, e2);

    ecs_add_pair(world, parent, TagA, e1);
    ecs_add_pair(world, parent, TagB, e2);

    char *json = ecs_world_to_json(world, NULL);
    test_assert(json != NULL);

    ecs_fini(world);
    world = ecs_init();

    const char *r = ecs_world_from_json(world, json, NULL);
    test_str(r, "");
    ecs_os_free(json);

    foo = ecs_lookup_fullpath(world, "foo");
    test_assert(foo != 0);

    e1 = ecs_get_target(world, foo, TagA, 0);
    test_assert(e1 != 0);
    test_str(ecs_get_name(world, e1), "e1");
    test_assert(ecs_has(world, e1, TagA));

    e2 = ecs_get_target(world, foo, TagB, 0);
    test_assert(e2 != 0);
    test_str(ecs_get_name(world, e2), "e2");
    test_assert(ecs_has(world, e2, TagB));

    test_assert(ecs_get_target(world, e1, EcsChildOf, 0) != 0);
    test_assert(ecs_get_target(world, e2, EcsChildOf, 0) != 0);
    test_assert(ecs_get_target(world, e1, EcsChildOf, 0) ==
        ecs_get_target(world, e2, EcsChildOf, 0));

    parent = ecs_get_target(world, e1, EcsChildOf, 0);
    test_assert(ecs_has_pair(world, parent, TagA, e1));
    test_assert(ecs_has_pair(world, parent, TagB, e2));

    ecs_fini(world);
}

void DeserializeFromJson_ser_deser_new_world_2_entities_w_named_parent_w_cycle(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_entity_t parent = ecs_new_entity(world, "parent");
    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_entity_t e2 = ecs_new_entity(world, "e2");
    ecs_add_pair(world, e1, EcsChildOf, parent);
    ecs_add_pair(world, e2, EcsChildOf, parent);
    ecs_add(world, e1, TagA);
    ecs_add(world, e2, TagB);

    ecs_add_pair(world, parent, TagA, e1);
    ecs_add_pair(world, parent, TagB, e2);

    char *json = ecs_world_to_json(world, NULL);
    test_assert(json != NULL);

    ecs_fini(world);
    world = ecs_init();

    const char *r = ecs_world_from_json(world, json, NULL);
    test_str(r, "");
    ecs_os_free(json);

    parent = ecs_lookup_fullpath(world, "parent");
    test_assert(parent != 0);
    test_str(ecs_get_name(world, parent), "parent");

    e1 = ecs_lookup_fullpath(world, "parent.e1");
    test_assert(e1 != 0);
    test_str(ecs_get_name(world, e1), "e1");

    e2 = ecs_lookup_fullpath(world, "parent.e2");
    test_assert(e2 != 0);
    test_str(ecs_get_name(world, e2), "e2");

    test_assert(ecs_get_target(world, e1, EcsChildOf, 0) == parent);
    test_assert(ecs_get_target(world, e2, EcsChildOf, 0) == parent);

    test_assert(ecs_has_pair(world, parent, TagA, e1));
    test_assert(ecs_has_pair(world, parent, TagB, e2));

    ecs_fini(world);
}

void DeserializeFromJson_ser_deser_restore_1_entity_to_empty_table(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ecs_struct(world, {
        .entity = ecs_id(Position),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)},
        }
    });

    ECS_COMPONENT(world, Velocity);
    
    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_set(world, e1, Position, {10, 20});

    char *json = ecs_world_to_json(world, NULL);
    test_assert(json != NULL);

    ecs_entity_t e2 = ecs_new_entity(world, "e2");
    ecs_set(world, e2, Position, {30, 40});
    ecs_set(world, e2, Velocity, {3, 4});
    ecs_set(world, e1, Velocity, {1, 2});

    const char *r = ecs_world_from_json(world, json, NULL);
    test_str(r, "");
    ecs_os_free(json);

    test_assert(e1 == ecs_lookup_fullpath(world, "e1"));
    test_assert(e2 == ecs_lookup_fullpath(world, "e2"));

    test_assert(ecs_has(world, e1, Position));
    test_assert(ecs_has(world, e2, Position));
    test_assert(!ecs_has(world, e1, Velocity));
    test_assert(ecs_has(world, e2, Velocity));

    {
        const Position *p = ecs_get(world, e1, Position);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 20);
    }
    {
        const Position *p = ecs_get(world, e2, Position);
        test_assert(p != NULL);
        test_int(p->x, 30);
        test_int(p->y, 40);
    }
    {
        const Velocity *v = ecs_get(world, e2, Velocity);
        test_assert(v != NULL);
        test_int(v->x, 3);
        test_int(v->y, 4);
    }

    ecs_fini(world);
}

void DeserializeFromJson_ser_deser_restore_1_entity_to_non_empty_table(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ecs_struct(world, {
        .entity = ecs_id(Position),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)},
        }
    });

    ECS_COMPONENT(world, Velocity);
    ecs_struct(world, {
        .entity = ecs_id(Velocity),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)},
        }
    });

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_set(world, e1, Position, {10, 20});
    ecs_set(world, e1, Velocity, {1, 2});

    ecs_entity_t e2 = ecs_new_entity(world, "e2");
    ecs_set(world, e2, Position, {30, 40});
    ecs_set(world, e2, Velocity, {3, 4});

    char *json = ecs_world_to_json(world, NULL);
    test_assert(json != NULL);

    ecs_remove(world, e1, Position);

    const char *r = ecs_world_from_json(world, json, NULL);
    test_str(r, "");
    ecs_os_free(json);

    test_assert(e1 == ecs_lookup_fullpath(world, "e1"));
    test_assert(e2 == ecs_lookup_fullpath(world, "e2"));

    test_assert(ecs_has(world, e1, Position));
    test_assert(ecs_has(world, e2, Position));
    test_assert(ecs_has(world, e1, Velocity));
    test_assert(ecs_has(world, e2, Velocity));

    {
        const Position *p = ecs_get(world, e1, Position);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 20);
    }
    {
        const Position *p = ecs_get(world, e2, Position);
        test_assert(p != NULL);
        test_int(p->x, 30);
        test_int(p->y, 40);
    }
    {
        const Velocity *v = ecs_get(world, e1, Velocity);
        test_assert(v != NULL);
        test_int(v->x, 1);
        test_int(v->y, 2);
    }
    {
        const Velocity *v = ecs_get(world, e2, Velocity);
        test_assert(v != NULL);
        test_int(v->x, 3);
        test_int(v->y, 4);
    }

    ecs_fini(world);
}

void DeserializeFromJson_ser_deser_restore_1_anon_entity_to_empty_table(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ecs_struct(world, {
        .entity = ecs_id(Position),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)},
        }
    });

    ECS_COMPONENT(world, Velocity);
    
    ecs_entity_t e1 = ecs_new_id(world);
    ecs_set(world, e1, Position, {10, 20});

    char *json = ecs_world_to_json(world, NULL);
    test_assert(json != NULL);

    ecs_entity_t e2 = ecs_new_id(world);
    ecs_set(world, e2, Position, {30, 40});
    ecs_set(world, e2, Velocity, {3, 4});
    ecs_set(world, e1, Velocity, {1, 2});

    const char *r = ecs_world_from_json(world, json, NULL);
    test_str(r, "");
    ecs_os_free(json);

    test_assert(ecs_is_alive(world, e1));
    test_assert(ecs_is_alive(world, e2));

    test_assert(ecs_has(world, e1, Position));
    test_assert(ecs_has(world, e2, Position));
    test_assert(!ecs_has(world, e1, Velocity));
    test_assert(ecs_has(world, e2, Velocity));

    {
        const Position *p = ecs_get(world, e1, Position);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 20);
    }
    {
        const Position *p = ecs_get(world, e2, Position);
        test_assert(p != NULL);
        test_int(p->x, 30);
        test_int(p->y, 40);
    }
    {
        const Velocity *v = ecs_get(world, e2, Velocity);
        test_assert(v != NULL);
        test_int(v->x, 3);
        test_int(v->y, 4);
    }

    ecs_fini(world);
}

void DeserializeFromJson_ser_deser_restore_1_anon_entity_to_non_empty_table(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ecs_struct(world, {
        .entity = ecs_id(Position),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)},
        }
    });

    ECS_COMPONENT(world, Velocity);
    ecs_struct(world, {
        .entity = ecs_id(Velocity),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)},
        }
    });

    ecs_entity_t e1 = ecs_new_id(world);
    ecs_set(world, e1, Position, {10, 20});
    ecs_set(world, e1, Velocity, {1, 2});

    ecs_entity_t e2 = ecs_new_id(world);
    ecs_set(world, e2, Position, {30, 40});
    ecs_set(world, e2, Velocity, {3, 4});

    char *json = ecs_world_to_json(world, NULL);
    test_assert(json != NULL);

    ecs_remove(world, e1, Position);

    const char *r = ecs_world_from_json(world, json, NULL);
    test_str(r, "");
    ecs_os_free(json);

    test_assert(ecs_is_alive(world, e1));
    test_assert(ecs_is_alive(world, e2));

    test_assert(ecs_has(world, e1, Position));
    test_assert(ecs_has(world, e2, Position));
    test_assert(ecs_has(world, e1, Velocity));
    test_assert(ecs_has(world, e2, Velocity));

    {
        const Position *p = ecs_get(world, e1, Position);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 20);
    }
    {
        const Position *p = ecs_get(world, e2, Position);
        test_assert(p != NULL);
        test_int(p->x, 30);
        test_int(p->y, 40);
    }
    {
        const Velocity *v = ecs_get(world, e1, Velocity);
        test_assert(v != NULL);
        test_int(v->x, 1);
        test_int(v->y, 2);
    }
    {
        const Velocity *v = ecs_get(world, e2, Velocity);
        test_assert(v != NULL);
        test_int(v->x, 3);
        test_int(v->y, 4);
    }

    ecs_fini(world);
}

void DeserializeFromJson_ser_deser_restore_1_deleted_entity_to_empty_table(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ecs_struct(world, {
        .entity = ecs_id(Position),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)},
        }
    });

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_set(world, e1, Position, {10, 20});

    char *json = ecs_world_to_json(world, NULL);
    test_assert(json != NULL);

    ecs_delete(world, e1);

    const char *r = ecs_world_from_json(world, json, NULL);
    test_str(r, "");
    ecs_os_free(json);

    e1 = ecs_lookup_fullpath(world, "e1");
    test_assert(e1 != 0);
    test_assert(ecs_has(world, e1, Position));

    const Position *p = ecs_get(world, e1, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void DeserializeFromJson_ser_deser_restore_1_deleted_entity_to_non_empty_table(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ecs_struct(world, {
        .entity = ecs_id(Position),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)},
        }
    });

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_set(world, e1, Position, {10, 20});

    ecs_entity_t e2 = ecs_new_entity(world, "e2");
    ecs_set(world, e2, Position, {30, 40});

    char *json = ecs_world_to_json(world, NULL);
    test_assert(json != NULL);

    ecs_delete(world, e1);

    const char *r = ecs_world_from_json(world, json, NULL);
    test_str(r, "");
    ecs_os_free(json);


    e1 = ecs_lookup_fullpath(world, "e1");
    test_assert(e1 != 0);
    test_assert(ecs_has(world, e1, Position));

    test_assert(ecs_is_alive(world, e2));
    test_assert(ecs_has(world, e2, Position));

    {
        const Position *p = ecs_get(world, e1, Position);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 20);
    }
    {
        const Position *p = ecs_get(world, e2, Position);
        test_assert(p != NULL);
        test_int(p->x, 30);
        test_int(p->y, 40);
    }

    ecs_fini(world);
}

void DeserializeFromJson_ser_deser_restore_1_deleted_anon_entity_to_empty_table(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ecs_struct(world, {
        .entity = ecs_id(Position),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)},
        }
    });

    ecs_entity_t e1 = ecs_new_id(world);
    ecs_set(world, e1, Position, {10, 20});

    char *json = ecs_world_to_json(world, NULL);
    test_assert(json != NULL);

    ecs_delete(world, e1);
    test_assert(!ecs_is_alive(world, e1));

    const char *r = ecs_world_from_json(world, json, NULL);
    test_str(r, "");
    ecs_os_free(json);

    ecs_iter_t it = ecs_term_iter(world, &(ecs_term_t){ .id = ecs_id(Position) });
    e1 = ecs_iter_first(&it);
    test_assert(e1 != 0);

    test_assert(ecs_is_alive(world, e1));
    test_assert(ecs_has(world, e1, Position));

    const Position *p = ecs_get(world, e1, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void DeserializeFromJson_ser_deser_restore_1_deleted_anon_entity_to_non_empty_table(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ecs_struct(world, {
        .entity = ecs_id(Position),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)},
        }
    });

    ecs_entity_t e1 = ecs_new_id(world);
    ecs_set(world, e1, Position, {10, 20});

    ecs_entity_t e2 = ecs_new_id(world);
    ecs_set(world, e2, Position, {30, 40});

    char *json = ecs_world_to_json(world, NULL);
    test_assert(json != NULL);

    ecs_delete(world, e1);
    test_assert(!ecs_is_alive(world, e1));

    const char *r = ecs_world_from_json(world, json, NULL);
    test_str(r, "");
    ecs_os_free(json);

    ecs_iter_t it = ecs_term_iter(world, &(ecs_term_t){ .id = ecs_id(Position) });
    test_assert(ecs_term_next(&it) == true);
    test_int(it.count, 2);
    if (it.entities[0] == e2) {
        e1 = it.entities[1];
    } else {
        e1 = it.entities[0];
    }
    test_assert(ecs_term_next(&it) == false);

    test_assert(ecs_has(world, e1, Position));
    test_assert(ecs_is_alive(world, e2));
    test_assert(ecs_has(world, e2, Position));

    {
        const Position *p = ecs_get(world, e1, Position);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 20);
    }
    {
        const Position *p = ecs_get(world, e2, Position);
        test_assert(p != NULL);
        test_int(p->x, 30);
        test_int(p->y, 40);
    }

    ecs_fini(world);
}

void DeserializeFromJson_ser_deser_restore_1_deleted_w_ref(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ecs_struct(world, {
        .entity = ecs_id(Position),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)},
        }
    });

    ECS_TAG(world, TagA);

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_set(world, e1, Position, {10, 20});

    ecs_entity_t e2 = ecs_new_entity(world, "e2");
    ecs_set(world, e2, Position, {30, 40});
    ecs_add_pair(world, e2, TagA, e1);

    char *json = ecs_world_to_json(world, NULL);
    test_assert(json != NULL);

    ecs_delete(world, e1);
    test_assert(!ecs_is_alive(world, e1));
    test_assert(ecs_get_target(world, e2, TagA, 0) == 0);

    const char *r = ecs_world_from_json(world, json, NULL);
    test_str(r, "");
    ecs_os_free(json);

    e1 = ecs_lookup_fullpath(world, "e1");
    test_assert(e1 != 0);
    test_assert(ecs_get_target(world, e2, TagA, 0) == e1);

    test_assert(ecs_is_alive(world, e1));
    test_assert(ecs_has(world, e1, Position));
    test_assert(ecs_has(world, e2, Position));

    test_str(ecs_get_name(world, e1), "e1");
    test_str(ecs_get_name(world, e2), "e2");

    {
        const Position *p = ecs_get(world, e1, Position);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 20);
    }
    {
        const Position *p = ecs_get(world, e2, Position);
        test_assert(p != NULL);
        test_int(p->x, 30);
        test_int(p->y, 40);
    }

    ecs_fini(world);
}

void DeserializeFromJson_ser_deser_restore_1_deleted_anon_w_ref(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ecs_struct(world, {
        .entity = ecs_id(Position),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)},
        }
    });

    ECS_TAG(world, TagA);

    ecs_entity_t e1 = ecs_new_id(world);
    ecs_set(world, e1, Position, {10, 20});

    ecs_entity_t e2 = ecs_new_id(world);
    ecs_set(world, e2, Position, {30, 40});
    ecs_add_pair(world, e2, TagA, e1);

    char *json = ecs_world_to_json(world, NULL);
    test_assert(json != NULL);

    ecs_delete(world, e1);
    test_assert(!ecs_is_alive(world, e1));

    const char *r = ecs_world_from_json(world, json, NULL);
    test_str(r, "");
    ecs_os_free(json);

    test_assert(ecs_get_target(world, e2, TagA, 0) != 0);
    e1 = ecs_get_target(world, e2, TagA, 0);

    test_assert(ecs_is_alive(world, e1));
    test_assert(ecs_has(world, e1, Position));
    test_assert(ecs_has(world, e2, Position));

    {
        const Position *p = ecs_get(world, e1, Position);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 20);
    }
    {
        const Position *p = ecs_get(world, e2, Position);
        test_assert(p != NULL);
        test_int(p->x, 30);
        test_int(p->y, 40);
    }

    ecs_fini(world);
}

void DeserializeFromJson_ser_deser_restore_1_deleted_w_cycle_ref(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ecs_struct(world, {
        .entity = ecs_id(Position),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)},
        }
    });

    ECS_TAG(world, TagA);

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_set(world, e1, Position, {10, 20});

    ecs_entity_t e2 = ecs_new_entity(world, "e2");
    ecs_set(world, e2, Position, {30, 40});
    
    ecs_add_pair(world, e1, TagA, e2);
    ecs_add_pair(world, e2, TagA, e1);

    char *json = ecs_world_to_json(world, NULL);
    test_assert(json != NULL);

    ecs_delete(world, e1);
    test_assert(!ecs_is_alive(world, e1));
    test_assert(ecs_get_target(world, e2, TagA, 0) == 0);

    const char *r = ecs_world_from_json(world, json, NULL);
    test_str(r, "");
    ecs_os_free(json);

    e1 = ecs_lookup_fullpath(world, "e1");
    test_assert(e1 != 0);
    test_assert(ecs_get_target(world, e2, TagA, 0) == e1);
    test_assert(ecs_get_target(world, e1, TagA, 0) == e2);

    test_assert(ecs_is_alive(world, e1));
    test_assert(ecs_has(world, e1, Position));
    test_assert(ecs_has(world, e2, Position));

    test_str(ecs_get_name(world, e1), "e1");
    test_str(ecs_get_name(world, e2), "e2");

    {
        const Position *p = ecs_get(world, e1, Position);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 20);
    }
    {
        const Position *p = ecs_get(world, e2, Position);
        test_assert(p != NULL);
        test_int(p->x, 30);
        test_int(p->y, 40);
    }

    ecs_fini(world);
}

void DeserializeFromJson_ser_deser_restore_1_deleted_anon_w_cycle_ref(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ecs_struct(world, {
        .entity = ecs_id(Position),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)},
        }
    });

    ECS_TAG(world, TagA);

    ecs_entity_t e1 = ecs_new_id(world);
    ecs_set(world, e1, Position, {10, 20});

    ecs_entity_t e2 = ecs_new_id(world);
    ecs_set(world, e2, Position, {30, 40});
    
    ecs_add_pair(world, e1, TagA, e2);
    ecs_add_pair(world, e2, TagA, e1);

    char *json = ecs_world_to_json(world, NULL);
    test_assert(json != NULL);

    ecs_delete(world, e1);
    test_assert(!ecs_is_alive(world, e1));
    test_assert(ecs_get_target(world, e2, TagA, 0) == 0);

    const char *r = ecs_world_from_json(world, json, NULL);
    test_str(r, "");
    ecs_os_free(json);

    e1 = ecs_get_target(world, e2, TagA, 0);
    test_assert(e1 != 0);
    test_assert(ecs_get_target(world, e1, TagA, 0) == e2);

    test_assert(ecs_is_alive(world, e1));
    test_assert(ecs_has(world, e1, Position));
    test_assert(ecs_has(world, e2, Position));

    {
        const Position *p = ecs_get(world, e1, Position);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 20);
    }
    {
        const Position *p = ecs_get(world, e2, Position);
        test_assert(p != NULL);
        test_int(p->x, 30);
        test_int(p->y, 40);
    }

    ecs_fini(world);
}

void DeserializeFromJson_ser_deser_restore_1_deleted_w_recycled(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ecs_struct(world, {
        .entity = ecs_id(Position),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)},
        }
    });

    ECS_TAG(world, TagA);

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_set(world, e1, Position, {10, 20});

    ecs_entity_t e2 = ecs_new_entity(world, "e2");
    ecs_set(world, e2, Position, {30, 40});
    
    ecs_add_pair(world, e1, TagA, e2);
    ecs_add_pair(world, e2, TagA, e1);

    char *json = ecs_world_to_json(world, NULL);
    test_assert(json != NULL);

    ecs_delete(world, e1);
    test_assert(!ecs_is_alive(world, e1));
    test_assert(ecs_get_target(world, e2, TagA, 0) == 0);

    ecs_entity_t e3 = ecs_new_entity(world, "e3");
    ecs_set(world, e3, Position, {50, 60});

    const char *r = ecs_world_from_json(world, json, NULL);
    test_str(r, "");
    ecs_os_free(json);

    e1 = ecs_lookup_fullpath(world, "e1");
    test_assert(e1 != 0);
    test_assert(e1 != e3);
    test_assert(ecs_get_target(world, e2, TagA, 0) == e1);
    test_assert(ecs_get_target(world, e1, TagA, 0) == e2);

    test_assert(ecs_is_alive(world, e1));
    test_assert(ecs_is_alive(world, e2));
    test_assert(ecs_is_alive(world, e3));
    test_assert(ecs_has(world, e1, Position));
    test_assert(ecs_has(world, e2, Position));
    test_assert(ecs_has(world, e3, Position));

    test_str(ecs_get_name(world, e1), "e1");
    test_str(ecs_get_name(world, e2), "e2");
    test_str(ecs_get_name(world, e3), "e3");

    {
        const Position *p = ecs_get(world, e1, Position);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 20);
    }
    {
        const Position *p = ecs_get(world, e2, Position);
        test_assert(p != NULL);
        test_int(p->x, 30);
        test_int(p->y, 40);
    }
    {
        const Position *p = ecs_get(world, e3, Position);
        test_assert(p != NULL);
        test_int(p->x, 50);
        test_int(p->y, 60);
    }

    ecs_fini(world);
}

void DeserializeFromJson_ser_deser_restore_1_deleted_anon_w_recycled(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ecs_struct(world, {
        .entity = ecs_id(Position),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)},
        }
    });

    ECS_TAG(world, TagA);

    ecs_entity_t e1 = ecs_new_id(world);
    ecs_set(world, e1, Position, {10, 20});

    ecs_entity_t e2 = ecs_new_id(world);
    ecs_set(world, e2, Position, {30, 40});
    
    ecs_add_pair(world, e1, TagA, e2);
    ecs_add_pair(world, e2, TagA, e1);

    char *json = ecs_world_to_json(world, NULL);
    test_assert(json != NULL);

    ecs_delete(world, e1);
    test_assert(!ecs_is_alive(world, e1));
    test_assert(ecs_get_target(world, e2, TagA, 0) == 0);

    ecs_entity_t e3 = ecs_new_id(world);
    ecs_set(world, e3, Position, {50, 60});

    const char *r = ecs_world_from_json(world, json, NULL);
    test_str(r, "");
    ecs_os_free(json);

    e1 = ecs_get_target(world, e2, TagA, 0);
    test_assert(e1 != 0);
    test_assert(e1 != e3);
    test_assert(ecs_get_target(world, e1, TagA, 0) == e2);

    test_assert(ecs_is_alive(world, e1));
    test_assert(ecs_is_alive(world, e2));
    test_assert(ecs_is_alive(world, e3));
    test_assert(ecs_has(world, e1, Position));
    test_assert(ecs_has(world, e2, Position));
    test_assert(ecs_has(world, e3, Position));

    {
        const Position *p = ecs_get(world, e1, Position);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 20);
    }
    {
        const Position *p = ecs_get(world, e2, Position);
        test_assert(p != NULL);
        test_int(p->x, 30);
        test_int(p->y, 40);
    }
    {
        const Position *p = ecs_get(world, e3, Position);
        test_assert(p != NULL);
        test_int(p->x, 50);
        test_int(p->y, 60);
    }

    ecs_fini(world);
}

void OnSet_count(ecs_iter_t *it) {
    int32_t *count = it->ctx;
    test_assert(count != NULL);
    *count += it->count;
}

void DeserializeFromJson_ser_deser_on_set_3_entities(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ecs_struct(world, {
        .entity = ecs_id(Position),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)},
        }
    });
    
    int32_t count = 0;

    ecs_observer(world, {
        .filter.terms[0].id = ecs_id(Position),
        .events = { EcsOnSet },
        .callback = OnSet_count,
        .ctx = &count
    });

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_entity_t e2 = ecs_new_entity(world, "e2");
    ecs_entity_t e3 = ecs_new_entity(world, "e3");
    ecs_set(world, e1, Position, {10, 20});
    ecs_set(world, e2, Position, {30, 40});
    ecs_set(world, e3, Position, {50, 60});

    test_int(count, 3);
    count = 0;

    char *json = ecs_world_to_json(world, NULL);
    test_assert(json != NULL);

    const char *r = ecs_world_from_json(world, json, NULL);
    test_str(r, "");
    ecs_os_free(json);

    test_int(count, 3);

    ecs_fini(world);
}

void DeserializeFromJson_ser_deser_on_set_3_entities_2_restored(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ecs_struct(world, {
        .entity = ecs_id(Position),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)},
        }
    });
    
    int32_t count = 0;

    ecs_observer(world, {
        .filter.terms[0].id = ecs_id(Position),
        .events = { EcsOnSet },
        .callback = OnSet_count,
        .ctx = &count
    });

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_entity_t e2 = ecs_new_entity(world, "e2");
    ecs_set(world, e1, Position, {10, 20});
    ecs_set(world, e2, Position, {30, 40});

    test_int(count, 2);
    count = 0;

    char *json = ecs_world_to_json(world, NULL);
    test_assert(json != NULL);

    ecs_entity_t e3 = ecs_new_entity(world, "e3");
    ecs_set(world, e3, Position, {50, 60});

    test_int(count, 1);
    count = 0;

    const char *r = ecs_world_from_json(world, json, NULL);
    test_str(r, "");
    ecs_os_free(json);

    test_int(count, 2);

    ecs_fini(world);
}

void DeserializeFromJson_ser_deser_on_set_3_entities_1_restored(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ecs_struct(world, {
        .entity = ecs_id(Position),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)},
        }
    });
    
    int32_t count = 0;

    ecs_observer(world, {
        .filter.terms[0].id = ecs_id(Position),
        .events = { EcsOnSet },
        .callback = OnSet_count,
        .ctx = &count
    });

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_set(world, e1, Position, {10, 20});

    test_int(count, 1);
    count = 0;

    char *json = ecs_world_to_json(world, NULL);
    test_assert(json != NULL);

    ecs_entity_t e2 = ecs_new_entity(world, "e2");
    ecs_entity_t e3 = ecs_new_entity(world, "e3");
    ecs_set(world, e2, Position, {30, 40});
    ecs_set(world, e3, Position, {50, 60});

    test_int(count, 2);
    count = 0;

    const char *r = ecs_world_from_json(world, json, NULL);
    test_str(r, "");
    ecs_os_free(json);

    test_int(count, 1);

    ecs_fini(world);
}

void DeserializeFromJson_ser_deser_3_entities_after_remove_all(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ecs_struct(world, {
        .entity = ecs_id(Position),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)},
        }
    });

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_entity_t e2 = ecs_new_entity(world, "e2");
    ecs_entity_t e3 = ecs_new_entity(world, "e3");
    ecs_set(world, e1, Position, {10, 20});
    ecs_set(world, e2, Position, {30, 40});
    ecs_set(world, e3, Position, {50, 60});

    char *json = ecs_world_to_json(world, NULL);
    test_assert(json != NULL);

    ecs_remove_all(world, ecs_id(Position));
    test_assert(ecs_is_alive(world, e1));
    test_assert(ecs_is_alive(world, e2));
    test_assert(ecs_is_alive(world, e3));
    test_assert(!ecs_has(world, e1, Position));
    test_assert(!ecs_has(world, e2, Position));
    test_assert(!ecs_has(world, e3, Position));

    const char *r = ecs_world_from_json(world, json, NULL);
    test_str(r, "");
    ecs_os_free(json);

    e1 = ecs_lookup_fullpath(world, "e1");
    e2 = ecs_lookup_fullpath(world, "e2");
    e3 = ecs_lookup_fullpath(world, "e3");

    test_assert(ecs_is_alive(world, e1));
    test_assert(ecs_is_alive(world, e2));
    test_assert(ecs_is_alive(world, e3));
    test_assert(ecs_has(world, e1, Position));
    test_assert(ecs_has(world, e2, Position));
    test_assert(ecs_has(world, e3, Position));

    {
        const Position *p = ecs_get(world, e1, Position);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 20);
    }
    {
        const Position *p = ecs_get(world, e2, Position);
        test_assert(p != NULL);
        test_int(p->x, 30);
        test_int(p->y, 40);
    }
    {
        const Position *p = ecs_get(world, e3, Position);
        test_assert(p != NULL);
        test_int(p->x, 50);
        test_int(p->y, 60);
    }

    ecs_fini(world);
}

void DeserializeFromJson_ser_deser_3_entities_after_delete_with(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ecs_struct(world, {
        .entity = ecs_id(Position),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)},
        }
    });

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_entity_t e2 = ecs_new_entity(world, "e2");
    ecs_entity_t e3 = ecs_new_entity(world, "e3");
    ecs_set(world, e1, Position, {10, 20});
    ecs_set(world, e2, Position, {30, 40});
    ecs_set(world, e3, Position, {50, 60});

    char *json = ecs_world_to_json(world, NULL);
    test_assert(json != NULL);

    ecs_delete_with(world, ecs_id(Position));
    test_assert(!ecs_is_alive(world, e1));
    test_assert(!ecs_is_alive(world, e2));
    test_assert(!ecs_is_alive(world, e3));

    const char *r = ecs_world_from_json(world, json, NULL);
    test_str(r, "");
    ecs_os_free(json);

    e1 = ecs_lookup_fullpath(world, "e1");
    e2 = ecs_lookup_fullpath(world, "e2");
    e3 = ecs_lookup_fullpath(world, "e3");

    test_assert(ecs_is_alive(world, e1));
    test_assert(ecs_is_alive(world, e2));
    test_assert(ecs_is_alive(world, e3));
    test_assert(ecs_has(world, e1, Position));
    test_assert(ecs_has(world, e2, Position));
    test_assert(ecs_has(world, e3, Position));

    {
        const Position *p = ecs_get(world, e1, Position);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 20);
    }
    {
        const Position *p = ecs_get(world, e2, Position);
        test_assert(p != NULL);
        test_int(p->x, 30);
        test_int(p->y, 40);
    }
    {
        const Position *p = ecs_get(world, e3, Position);
        test_assert(p != NULL);
        test_int(p->x, 50);
        test_int(p->y, 60);
    }

    ecs_fini(world);
}

typedef struct StringType {
    char *value;
} StringType;

static int StringType_ctor_invoked = 0;

ECS_CTOR(StringType, ptr, {
    ptr->value = NULL;
    StringType_ctor_invoked ++;
})

ECS_DTOR(StringType, ptr, {
    ecs_os_free(ptr->value);
})

ECS_MOVE(StringType, dst, src, {
    if (dst->value) {
        ecs_os_free(dst->value);
    }
    dst->value = src->value;
    src->value = NULL;
})

ECS_COPY(StringType, dst, src, {
    if (dst->value) {
        ecs_os_free(dst->value);
    }
    dst->value = ecs_os_strdup(src->value);
})

void DeserializeFromJson_ser_deser_w_hooks(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, StringType);
    ecs_struct(world, {
        .entity = ecs_id(StringType),
        .members = {
            {"value", ecs_id(ecs_string_t)}
        }
    });

    ecs_set_hooks(world, StringType, {
        .ctor = ecs_default_ctor,
        .dtor = ecs_dtor(StringType),
        .move = ecs_move(StringType),
        .copy = ecs_copy(StringType)
    });

    ecs_entity_t e1 = ecs_set(world, 0, StringType, {"foo"});
    ecs_entity_t e2 = ecs_set(world, 0, StringType, {"bar"});
    ecs_entity_t e3 = ecs_set(world, 0, StringType, {"hello"});
    ecs_entity_t e4 = ecs_set(world, 0, StringType, {"world"});

    ecs_set_name(world, e1, "e1");
    ecs_set_name(world, e2, "e2");
    ecs_set_name(world, e3, "e3");
    ecs_set_name(world, e4, "e4");

    char *json = ecs_world_to_json(world, NULL);
    test_assert(json != NULL);

    ecs_remove(world, e1, StringType);
    ecs_set(world, e2, StringType, { NULL });
    ecs_set(world, e3, StringType, { "zoo" });
    ecs_delete(world, e4);

    const char *r = ecs_world_from_json(world, json, NULL);
    test_str(r, "");
    ecs_os_free(json);

    test_assert(ecs_lookup_fullpath(world, "e1") == e1);
    test_assert(ecs_lookup_fullpath(world, "e2") == e2);
    test_assert(ecs_lookup_fullpath(world, "e3") == e3);
    e4 = ecs_lookup_fullpath(world, "e4");
    test_assert(e4 != 0);

    test_assert(ecs_is_alive(world, e1));
    test_assert(ecs_is_alive(world, e2));
    test_assert(ecs_is_alive(world, e3));
    test_assert(ecs_is_alive(world, e4));

    test_assert(ecs_has(world, e1, StringType));
    test_assert(ecs_has(world, e2, StringType));
    test_assert(ecs_has(world, e3, StringType));
    test_assert(ecs_has(world, e4, StringType));

    {
        const StringType *st = ecs_get(world, e1, StringType);
        test_assert(st != NULL);
        test_str(st->value, "foo");
    }
    {
        const StringType *st = ecs_get(world, e2, StringType);
        test_assert(st != NULL);
        test_str(st->value, "bar");
    }
    {
        const StringType *st = ecs_get(world, e3, StringType);
        test_assert(st != NULL);
        test_str(st->value, "hello");
    }
    {
        const StringType *st = ecs_get(world, e4, StringType);
        test_assert(st != NULL);
        test_str(st->value, "world");
    }

    ecs_fini(world);
}

void DeserializeFromJson_ser_deser_large_data(void) {
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

    char *long_str = ecs_os_malloc_n(char, 1024);
    ecs_os_memset(long_str, 'a', 1023);
    long_str[1023] = '\0';
    T value = {
        .v = long_str
    };

    char *long_str_ser = ecs_asprintf("{\"v\":\"%s\"}", long_str);
    char *json = ecs_ptr_to_json(world, t, &value);
    test_assert(json != NULL);
    test_str(json, long_str_ser);
    ecs_os_free(long_str_ser);
    value.v = NULL;

    const char *ptr = ecs_ptr_from_json(world, t, &value, json, NULL);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_str(value.v, long_str);
    ecs_os_free(value.v);
    ecs_os_free(long_str);
    ecs_os_free(json);

    ecs_fini(world);
}
