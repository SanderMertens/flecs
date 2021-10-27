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
    test_str(expr, "{\"x\":true}");
    ecs_os_free(expr);
    }

    {
    T value = {false};
    char *expr = ecs_ptr_to_json(world, t, &value);
    test_assert(expr != NULL);
    test_str(expr, "{\"x\":false}");
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
    test_str(expr, "{\"x\":10}");
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
    test_str(expr, "{\"x\":\"a\"}");
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
    test_str(expr, "{\"x\":10}");
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
    test_str(expr, "{\"x\":10}");
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
    test_str(expr, "{\"x\":10}");
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
    test_str(expr, "{\"x\":10}");
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
    test_str(expr, "{\"x\":10}");
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
    test_str(expr, "{\"x\":10}");
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
    test_str(expr, "{\"x\":10}");
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
    test_str(expr, "{\"x\":10}");
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
    test_str(expr, "{\"x\":10}");
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
    test_str(expr, "{\"x\":10}");
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
    test_str(expr, "{\"x\":10.500000}");
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
    test_str(expr, "{\"x\":10.500000}");
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
    test_str(expr, "{\"x\":\"Hello World\"}");
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
    test_str(expr, "{\"x\":\"flecs.core\"}");
    ecs_os_free(expr);

    ecs_fini(world);
}

void SerializeToJson_struct_enum() {
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
    char *expr = ecs_ptr_to_json(world, t, &value);
    test_assert(expr != NULL);
    test_str(expr, "{\"x\":\"Red\"}");
    ecs_os_free(expr);
    }

    {
    T value = {Blue};
    char *expr = ecs_ptr_to_json(world, t, &value);
    test_assert(expr != NULL);
    test_str(expr, "{\"x\":\"Blue\"}");
    ecs_os_free(expr);
    }

    {
    T value = {Green};
    char *expr = ecs_ptr_to_json(world, t, &value);
    test_assert(expr != NULL);
    test_str(expr, "{\"x\":\"Green\"}");
    ecs_os_free(expr);
    }

    {
    T value = {10};
    char *expr = ecs_ptr_to_json(world, t, &value);
    test_assert(expr == NULL);
    }

    ecs_fini(world);
}

void SerializeToJson_struct_bitmask() {
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
    char *expr = ecs_ptr_to_json(world, t, &value);
    test_assert(expr != NULL);
    test_str(expr, "{\"x\":0}");
    ecs_os_free(expr);
    }

    {
    T value = {Lettuce};
    char *expr = ecs_ptr_to_json(world, t, &value);
    test_assert(expr != NULL);
    test_str(expr, "{\"x\":\"Lettuce\"}");
    ecs_os_free(expr);
    }

    {
    T value = {Bacon};
    char *expr = ecs_ptr_to_json(world, t, &value);
    test_assert(expr != NULL);
    test_str(expr, "{\"x\":\"Bacon\"}");
    ecs_os_free(expr);
    }

    {
    T value = {Lettuce | Bacon};
    char *expr = ecs_ptr_to_json(world, t, &value);
    test_assert(expr != NULL);
    test_str(expr, "{\"x\":\"Lettuce|Bacon\"}");
    ecs_os_free(expr);
    }

    {
    T value = {Lettuce | Bacon | Tomato | Cheese};
    char *expr = ecs_ptr_to_json(world, t, &value);
    test_assert(expr != NULL);
    test_str(expr, "{\"x\":\"Cheese|Lettuce|Bacon|Tomato\"}");
    ecs_os_free(expr);
    }

    {
    T value = {16};
    char *expr = ecs_ptr_to_json(world, t, &value);
    test_assert(expr == NULL);
    }

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
    test_str(expr, "{\"x\":10, \"y\":20}");
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
    test_str(expr, "{\"n_1\":{\"x\":10}}");
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
    test_str(expr, "{\"n_1\":{\"x\":10, \"y\":20}}");
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
    test_str(expr, "{\"n_1\":{\"x\":10, \"y\":20}, \"n_2\":{\"x\":30, \"y\":40}}");
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
    test_str(expr, "{\"x\":[10, 20, 30]}");
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
    test_str(expr, "{\"n_1\":[{\"x\":10}, {\"x\":20}, {\"x\":30}]}");
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
    test_str(expr, "{\"n_1\":[{\"x\":10, \"y\":20}, {\"x\":30, \"y\":40}, {\"x\":50, \"y\":60}]}");
    ecs_os_free(expr);

    ecs_fini(world);
}

void SerializeToJson_struct_w_array_type_i32_i32() {
    typedef int32_t N1[2];

    typedef struct {
        N1 n_1;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t n1 = ecs_array_init(world, &(ecs_array_desc_t) {
        .entity.name = "vec2",
        .type = ecs_id(ecs_i32_t),
        .count = 2
    });

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t) {
        .entity.name = "T",
        .members = {
            {"n_1", n1}
        }
    });

    T value = { .n_1 = { 10, 20 }};
    char *expr = ecs_ptr_to_json(world, t, &value);
    test_assert(expr != NULL);
    test_str(expr, "{\"n_1\":[10, 20]}");
    ecs_os_free(expr);

    ecs_fini(world);
}

void SerializeToJson_struct_w_2_array_type_i32_i32() {
    typedef int32_t N1[2];

    typedef struct {
        N1 n_1;
        N1 n_2;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t n1 = ecs_array_init(world, &(ecs_array_desc_t) {
        .entity.name = "vec2",
        .type = ecs_id(ecs_i32_t),
        .count = 2
    });

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t) {
        .entity.name = "T",
        .members = {
            {"n_1", n1},
            {"n_2", n1}
        }
    });

    T value = { .n_1 = { 10, 20 }, .n_2 = {30, 40} };
    char *expr = ecs_ptr_to_json(world, t, &value);
    test_assert(expr != NULL);
    test_str(expr, "{\"n_1\":[10, 20], \"n_2\":[30, 40]}");
    ecs_os_free(expr);

    ecs_fini(world);
}

void SerializeToJson_serialize_entity_empty() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t e = ecs_new_id(world);

    char *str = ecs_os_strdup("{\"path\":\"446\", \"type\":[]}");
    ecs_os_sprintf(
        str, "{\"path\":\"%u\", \"type\":[]}", (uint32_t)e);

    char *json = ecs_entity_to_json(world, e);
    test_assert(json != NULL);
    test_str(json, str);
    ecs_os_free(json);
    ecs_os_free(str);

    ecs_fini(world);
}

void SerializeToJson_serialize_entity_w_name() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t e = ecs_set_name(world, 0, "Foo");

    char *json = ecs_entity_to_json(world, e);
    test_assert(json != NULL);
    test_str(json, "{\"path\":\"Foo\", \"type\":[{\"pred\":\"Identifier\", \"obj\":\"Name\"}]}");
    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeToJson_serialize_entity_w_name_1_tag() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);

    ecs_entity_t e = ecs_set_name(world, 0, "Foo");
    ecs_add(world, e, Tag);

    char *json = ecs_entity_to_json(world, e);
    test_assert(json != NULL);
    test_str(json, "{\"path\":\"Foo\", \"type\":[{\"pred\":\"Tag\"}, {\"pred\":\"Identifier\", \"obj\":\"Name\"}]}");
    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeToJson_serialize_entity_w_name_2_tags() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_entity_t e = ecs_set_name(world, 0, "Foo");
    ecs_add(world, e, TagA);
    ecs_add(world, e, TagB);

    char *json = ecs_entity_to_json(world, e);
    test_assert(json != NULL);
    test_str(json, 
        "{\"path\":\"Foo\", \"type\":[{\"pred\":\"TagA\"}, {\"pred\":\"TagB\"}, {\"pred\":\"Identifier\", \"obj\":\"Name\"}]}");
    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeToJson_serialize_entity_w_name_1_pair() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Rel);
    ECS_TAG(world, Obj);

    ecs_entity_t e = ecs_set_name(world, 0, "Foo");
    ecs_add_pair(world, e, Rel, Obj);

    char *json = ecs_entity_to_json(world, e);
    test_assert(json != NULL);
    test_str(json, 
        "{\"path\":\"Foo\", \"type\":[{\"pred\":\"Identifier\", \"obj\":\"Name\"}, {\"pred\":\"Rel\", \"obj\":\"Obj\"}]}");
    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeToJson_serialize_entity_w_base() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_entity_t base = ecs_set_name(world, 0, "Base");
    ecs_add(world, base, TagA);

    ecs_entity_t e = ecs_set_name(world, 0, "Foo");
    ecs_add_pair(world, e, EcsIsA, base);
    ecs_add(world, e, TagB);

    char *json = ecs_entity_to_json(world, e);
    test_assert(json != NULL);
    test_str(json, "{"
        "\"path\":\"Foo\", "
        "\"is_a\":{\"Base\":{\"type\":["
            "{\"pred\":\"TagA\"}"
        "]}}, "
        "\"type\":["
            "{\"pred\":\"TagB\"}, {\"pred\":\"Identifier\", \"obj\":\"Name\"}"
        "]}");

    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeToJson_serialize_entity_w_base_override() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_entity_t base = ecs_set_name(world, 0, "Base");
    ecs_add(world, base, TagA);

    ecs_entity_t e = ecs_set_name(world, 0, "Foo");
    ecs_add_pair(world, e, EcsIsA, base);
    ecs_add(world, e, TagA);
    ecs_add(world, e, TagB);

    char *json = ecs_entity_to_json(world, e);
    test_assert(json != NULL);
    test_str(json, "{"
        "\"path\":\"Foo\", "
        "\"is_a\":{\"Base\":{\"type\":["
            "{\"pred\":\"TagA\", \"hidden\":true}"
        "]}}, "
        "\"type\":["
            "{\"pred\":\"TagA\"}, {\"pred\":\"TagB\"}, {\"pred\":\"Identifier\", \"obj\":\"Name\"}"
        "]}");

    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeToJson_serialize_entity_w_1_component() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = ecs_struct_init(world, &(ecs_struct_desc_t) {
        .entity.name = "Position",
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t e = ecs_set_name(world, 0, "Foo");
    ecs_set(world, e, Position, {10, 20});

    char *json = ecs_entity_to_json(world, e);
    test_assert(json != NULL);
    test_str(json, "{"
        "\"path\":\"Foo\", "
        "\"type\":["
            "{\"pred\":\"Position\", \"value\":{\"x\":10, \"y\":20}}, {\"pred\":\"Identifier\", \"obj\":\"Name\"}"
        "]}");

    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeToJson_serialize_entity_w_2_components() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = ecs_struct_init(world, &(ecs_struct_desc_t) {
        .entity.name = "Position",
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t ecs_id(Mass) = ecs_struct_init(world, &(ecs_struct_desc_t) {
        .entity.name = "Mass",
        .members = {
            {"value", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t e = ecs_set_name(world, 0, "Foo");
    ecs_set(world, e, Position, {10, 20});
    ecs_set(world, e, Mass, {1234});

    char *json = ecs_entity_to_json(world, e);
    test_assert(json != NULL);
    test_str(json, "{"
        "\"path\":\"Foo\", "
        "\"type\":["
            "{\"pred\":\"Position\", \"value\":{\"x\":10, \"y\":20}}, "
            "{\"pred\":\"Mass\", \"value\":{\"value\":1234}}, "
            "{\"pred\":\"Identifier\", \"obj\":\"Name\"}"
        "]}");

    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeToJson_serialize_entity_w_primitive_component() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t e = ecs_set_name(world, 0, "Foo");
    ecs_set(world, e, ecs_i32_t, {10});

    char *json = ecs_entity_to_json(world, e);
    test_assert(json != NULL);
    test_str(json, "{"
        "\"path\":\"Foo\", "
        "\"type\":["
            "{\"pred\":\"flecs.meta.i32\", \"value\":10}, {\"pred\":\"Identifier\", \"obj\":\"Name\"}"
        "]}");

    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeToJson_serialize_entity_w_enum_component() {
    typedef enum {
        Red, Green, Blue
    } Color;

    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Color) = ecs_enum_init(world, &(ecs_enum_desc_t) {
        .entity.name = "Color",
        .constants = {
            {"Red"}, {"Blue"}, {"Green"}
        }
    });

    test_assert(ecs_id(Color) != 0);

    ecs_entity_t e = ecs_set_name(world, 0, "Foo");
    ecs_set(world, e, Color, {1});

    char *json = ecs_entity_to_json(world, e);
    test_assert(json != NULL);
    test_str(json, "{"
        "\"path\":\"Foo\", "
        "\"type\":["
            "{\"pred\":\"Color\", \"value\":\"Blue\"}, {\"pred\":\"Identifier\", \"obj\":\"Name\"}"
        "]}");

    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeToJson_serialize_entity_w_struct_and_enum_component() {
    typedef enum {
        Red, Green, Blue
    } Color;

    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = ecs_struct_init(world, &(ecs_struct_desc_t) {
        .entity.name = "Position",
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    test_assert(ecs_id(Position) != 0);

    ecs_entity_t ecs_id(Color) = ecs_enum_init(world, &(ecs_enum_desc_t) {
        .entity.name = "Color",
        .constants = {
            {"Red"}, {"Blue"}, {"Green"}
        }
    });

    test_assert(ecs_id(Color) != 0);

    ecs_entity_t e = ecs_set_name(world, 0, "Foo");
    ecs_set(world, e, Color, {1});
    ecs_set(world, e, Position, {10, 20});

    char *json = ecs_entity_to_json(world, e);
    test_assert(json != NULL);
    test_str(json, "{"
        "\"path\":\"Foo\", "
        "\"type\":["
            "{\"pred\":\"Position\", \"value\":{\"x\":10, \"y\":20}}, "
            "{\"pred\":\"Color\", \"value\":\"Blue\"}, "
            "{\"pred\":\"Identifier\", \"obj\":\"Name\"}"
        "]}");

    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeToJson_serialize_entity_w_invalid_enum_component() {
    typedef enum {
        Red, Green, Blue
    } Color;

    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Color) = ecs_enum_init(world, &(ecs_enum_desc_t) {
        .entity.name = "Color",
        .constants = {
            {"Red"}, {"Blue"}, {"Green"}
        }
    });

    test_assert(ecs_id(Color) != 0);

    ecs_entity_t e = ecs_set_name(world, 0, "Foo");
    ecs_set(world, e, Color, {100});

    char *json = ecs_entity_to_json(world, e);
    test_assert(json == NULL);

    ecs_fini(world);
}

void SerializeToJson_serialize_iterator_1_comps_empty() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_struct_init(world, &(ecs_struct_desc_t) {
        .entity.entity = ecs_id(Position),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    ecs_query_t *q = ecs_query_new(world, "Position");
    ecs_iter_t it = ecs_query_iter(world, q);

    char *json = ecs_iter_to_json(world, &it, NULL);
    test_assert(json != NULL);
    test_str(json, "{\"ids\":[\"Position\"], \"results\":[]}");

    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeToJson_serialize_iterator_1_comps_2_ents_same_table() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_struct_init(world, &(ecs_struct_desc_t) {
        .entity.entity = ecs_id(Position),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t e1 = ecs_set_name(world, 0, "Foo");
    ecs_entity_t e2 = ecs_set_name(world, 0, "Bar");

    ecs_set(world, e1, Position, {10, 20});
    ecs_set(world, e2, Position, {30, 40});

    ecs_query_t *q = ecs_query_new(world, "Position");
    ecs_iter_t it = ecs_query_iter(world, q);

    char *json = ecs_iter_to_json(world, &it, NULL);
    test_str(json, 
    "{"
        "\"ids\":[\"Position\"], "
        "\"results\":[{"
            "\"ids\":[\"Position\"], "
            "\"subjects\":[0], "
            "\"is_set\":[true], "
            "\"entities\":["
                "\"Foo\", \"Bar\""
            "], "
            "\"values\":[["
                "{\"x\":10, \"y\":20}, "
                "{\"x\":30, \"y\":40}"
            "]]"
        "}]"
    "}");

    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeToJson_serialize_iterator_2_comps_2_ents_same_table() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Mass);

    ecs_struct_init(world, &(ecs_struct_desc_t) {
        .entity.entity = ecs_id(Position),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    ecs_struct_init(world, &(ecs_struct_desc_t) {
        .entity.entity = ecs_id(Mass),
        .members = {
            {"value", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t e1 = ecs_set_name(world, 0, "Foo");
    ecs_entity_t e2 = ecs_set_name(world, 0, "Bar");

    ecs_set(world, e1, Position, {10, 20});
    ecs_set(world, e2, Position, {30, 40});

    ecs_set(world, e1, Mass, {1});
    ecs_set(world, e2, Mass, {2});

    ecs_query_t *q = ecs_query_new(world, "Position, Mass");
    ecs_iter_t it = ecs_query_iter(world, q);

    char *json = ecs_iter_to_json(world, &it, NULL);
    test_str(json, 
    "{"
        "\"ids\":[\"Position\", \"Mass\"], "
        "\"results\":[{"
            "\"ids\":[\"Position\", \"Mass\"], "
            "\"subjects\":[0, 0], "
            "\"is_set\":[true, true], "
            "\"entities\":["
                "\"Foo\", \"Bar\""
            "], "
            "\"values\":[["
                "{\"x\":10, \"y\":20}, "
                "{\"x\":30, \"y\":40}"
            "], ["
                "{\"value\":1}, "
                "{\"value\":2}"
            "]]"
        "}]"
    "}");

    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeToJson_serialize_iterator_1_tag_2_ents_same_table() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, MyTag);

    ecs_entity_t e1 = ecs_set_name(world, 0, "Foo");
    ecs_entity_t e2 = ecs_set_name(world, 0, "Bar");

    ecs_add(world, e1, MyTag);
    ecs_add(world, e2, MyTag);

    ecs_query_t *q = ecs_query_new(world, "MyTag");
    ecs_iter_t it = ecs_query_iter(world, q);

    char *json = ecs_iter_to_json(world, &it, NULL);
    test_str(json, 
    "{"
        "\"ids\":[\"MyTag\"], "
        "\"results\":[{"
            "\"ids\":[\"MyTag\"], "
            "\"subjects\":[0], "
            "\"is_set\":[true], "
            "\"entities\":["
                "\"Foo\", \"Bar\""
            "], "
            "\"values\":[0]"
        "}]"
    "}");

    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeToJson_serialize_iterator_1_tag_1_comp_2_ents_same_table() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, MyTag);

    ecs_struct_init(world, &(ecs_struct_desc_t) {
        .entity.entity = ecs_id(Position),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t e1 = ecs_set_name(world, 0, "Foo");
    ecs_entity_t e2 = ecs_set_name(world, 0, "Bar");

    ecs_set(world, e1, Position, {10, 20});
    ecs_set(world, e2, Position, {30, 40});

    ecs_add(world, e1, MyTag);
    ecs_add(world, e2, MyTag);

    ecs_query_t *q = ecs_query_new(world, "Position, MyTag");
    ecs_iter_t it = ecs_query_iter(world, q);

    char *json = ecs_iter_to_json(world, &it, NULL);
    test_str(json, 
    "{"
        "\"ids\":[\"Position\", \"MyTag\"], "
        "\"results\":[{"
            "\"ids\":[\"Position\", \"MyTag\"], "
            "\"subjects\":[0, 0], "
            "\"is_set\":[true, true], "
            "\"entities\":["
                "\"Foo\", \"Bar\""
            "], "
            "\"values\":[["
                "{\"x\":10, \"y\":20}, "
                "{\"x\":30, \"y\":40}"
            "], 0]"
        "}]"
    "}");

    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeToJson_serialize_iterator_1_tag_1_comp_4_ents_two_tables() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_struct_init(world, &(ecs_struct_desc_t) {
        .entity.entity = ecs_id(Position),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t e1 = ecs_set_name(world, 0, "Foo");
    ecs_entity_t e2 = ecs_set_name(world, 0, "Bar");
    ecs_entity_t e3 = ecs_set_name(world, 0, "Hello");
    ecs_entity_t e4 = ecs_set_name(world, 0, "World");

    ecs_set(world, e1, Position, {10, 20});
    ecs_set(world, e2, Position, {30, 40});
    ecs_set(world, e3, Position, {50, 60});
    ecs_set(world, e4, Position, {70, 80});

    ecs_add(world, e1, TagA);
    ecs_add(world, e2, TagA);

    ecs_add(world, e3, TagA);
    ecs_add(world, e4, TagA);

    ecs_add(world, e3, TagB);
    ecs_add(world, e4, TagB);

    ecs_query_t *q = ecs_query_new(world, "Position, TagA");
    ecs_iter_t it = ecs_query_iter(world, q);

    char *json = ecs_iter_to_json(world, &it, NULL);
    test_str(json, 
    "{"
        "\"ids\":[\"Position\", \"TagA\"], "
        "\"results\":[{"
            "\"ids\":[\"Position\", \"TagA\"], "
            "\"subjects\":[0, 0], "
            "\"is_set\":[true, true], "
            "\"entities\":["
                "\"Foo\", \"Bar\""
            "], "
            "\"values\":[["
                "{\"x\":10, \"y\":20}, "
                "{\"x\":30, \"y\":40}"
            "], 0]"
        "}, {"
            "\"ids\":[\"Position\", \"TagA\"], "
            "\"subjects\":[0, 0], "
            "\"is_set\":[true, true], "
            "\"entities\":["
                "\"Hello\", \"World\""
            "], "
            "\"values\":[["
                "{\"x\":50, \"y\":60}, "
                "{\"x\":70, \"y\":80}"
            "], 0]"
        "}]"
    "}");

    ecs_os_free(json);

    ecs_fini(world);
}
