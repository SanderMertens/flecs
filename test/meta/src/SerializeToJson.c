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
    test_str(expr, "{\"x\": \"Red\"}");
    ecs_os_free(expr);
    }

    {
    T value = {Blue};
    char *expr = ecs_ptr_to_json(world, t, &value);
    test_assert(expr != NULL);
    test_str(expr, "{\"x\": \"Blue\"}");
    ecs_os_free(expr);
    }

    {
    T value = {Green};
    char *expr = ecs_ptr_to_json(world, t, &value);
    test_assert(expr != NULL);
    test_str(expr, "{\"x\": \"Green\"}");
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
    test_str(expr, "{\"x\": 0}");
    ecs_os_free(expr);
    }

    {
    T value = {Lettuce};
    char *expr = ecs_ptr_to_json(world, t, &value);
    test_assert(expr != NULL);
    test_str(expr, "{\"x\": \"Lettuce\"}");
    ecs_os_free(expr);
    }

    {
    T value = {Bacon};
    char *expr = ecs_ptr_to_json(world, t, &value);
    test_assert(expr != NULL);
    test_str(expr, "{\"x\": \"Bacon\"}");
    ecs_os_free(expr);
    }

    {
    T value = {Lettuce | Bacon};
    char *expr = ecs_ptr_to_json(world, t, &value);
    test_assert(expr != NULL);
    test_str(expr, "{\"x\": \"Lettuce|Bacon\"}");
    ecs_os_free(expr);
    }

    {
    T value = {Lettuce | Bacon | Tomato | Cheese};
    char *expr = ecs_ptr_to_json(world, t, &value);
    test_assert(expr != NULL);
    test_str(expr, "{\"x\": \"Cheese|Lettuce|Bacon|Tomato\"}");
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

void SerializeToJson_serialize_entity_empty() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t e = ecs_new_id(world);

    char *json = ecs_entity_to_json(world, e);
    test_assert(json != NULL);
    test_str(json, "{\"valid\": true, \"path\": \"446\", \"type\": []}");
    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeToJson_serialize_entity_w_name() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t e = ecs_set_name(world, 0, "Foo");

    char *json = ecs_entity_to_json(world, e);
    test_assert(json != NULL);
    test_str(json, "{\"valid\": true, \"path\": \"Foo\", \"type\": [{\"pred\":\"Identifier\", \"obj\":\"Name\"}]}");
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
    test_str(json, "{\"valid\": true, \"path\": \"Foo\", \"type\": [{\"pred\":\"Tag\"}, {\"pred\":\"Identifier\", \"obj\":\"Name\"}]}");
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
        "{\"valid\": true, \"path\": \"Foo\", \"type\": [{\"pred\":\"TagA\"}, {\"pred\":\"TagB\"}, {\"pred\":\"Identifier\", \"obj\":\"Name\"}]}");
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
        "{\"valid\": true, \"path\": \"Foo\", \"type\": [{\"pred\":\"Identifier\", \"obj\":\"Name\"}, {\"pred\":\"Rel\", \"obj\":\"Obj\"}]}");
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
    test_str(json, "{\"valid\": true, "
        "\"path\": \"Foo\", "
        "\"is_a\": {\"Base\": {\"type\": ["
            "{\"pred\":\"TagA\"}"
        "]}}, "
        "\"type\": ["
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
    test_str(json, "{\"valid\": true, "
        "\"path\": \"Foo\", "
        "\"is_a\": {\"Base\": {\"type\": ["
            "{\"pred\":\"TagA\", \"hidden\": true}"
        "]}}, "
        "\"type\": ["
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
    test_str(json, "{\"valid\": true, "
        "\"path\": \"Foo\", "
        "\"type\": ["
            "{\"pred\":\"Position\", \"data\": {\"x\": 10, \"y\": 20}}, {\"pred\":\"Identifier\", \"obj\":\"Name\"}"
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
    test_str(json, "{\"valid\": true, "
        "\"path\": \"Foo\", "
        "\"type\": ["
            "{\"pred\":\"Position\", \"data\": {\"x\": 10, \"y\": 20}}, "
            "{\"pred\":\"Mass\", \"data\": {\"value\": 1234}}, "
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
    test_str(json, "{\"valid\": true, "
        "\"path\": \"Foo\", "
        "\"type\": ["
            "{\"pred\":\"flecs.meta.i32\", \"data\": 10}, {\"pred\":\"Identifier\", \"obj\":\"Name\"}"
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
    test_str(json, "{\"valid\": true, "
        "\"path\": \"Foo\", "
        "\"type\": ["
            "{\"pred\":\"Color\", \"data\": \"Blue\"}, {\"pred\":\"Identifier\", \"obj\":\"Name\"}"
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
    test_str(json, "{\"valid\": true, "
        "\"path\": \"Foo\", "
        "\"type\": ["
            "{\"pred\":\"Position\", \"data\": {\"x\": 10, \"y\": 20}}, "
            "{\"pred\":\"Color\", \"data\": \"Blue\"}, "
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
