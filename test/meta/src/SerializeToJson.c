#include <meta.h>
#include <stdio.h>
#include <limits.h>
#include <math.h>

void SerializeToJson_struct_bool() {
    typedef struct {
        ecs_bool_t x;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
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

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
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

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
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

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
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

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
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

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
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

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
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

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
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

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
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

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
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

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
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

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
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

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
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

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.name = "T",
        .members = {
            {"x", ecs_id(ecs_f32_t)}
        }
    });

    T value = {10.5};
    char *expr = ecs_ptr_to_json(world, t, &value);
    test_assert(expr != NULL);
    test_str(expr, "{\"x\":10.5}");
    ecs_os_free(expr);

    ecs_fini(world);
}

void SerializeToJson_struct_float_nan() {
    typedef struct {
        ecs_f32_t x;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.name = "T",
        .members = {
            {"x", ecs_id(ecs_f32_t)}
        }
    });

    T value = {NAN};
    char *expr = ecs_ptr_to_json(world, t, &value);
    test_assert(expr != NULL);
    test_str(expr, "{\"x\":\"NaN\"}");
    ecs_os_free(expr);

    ecs_fini(world);
}

void SerializeToJson_struct_float_inf() {
    typedef struct {
        ecs_f32_t x;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.name = "T",
        .members = {
            {"x", ecs_id(ecs_f32_t)}
        }
    });

    {
        T value = {INFINITY};
        char *expr = ecs_ptr_to_json(world, t, &value);
        test_assert(expr != NULL);
        test_str(expr, "{\"x\":\"Inf\"}");
        ecs_os_free(expr);
    }
    {
        T value = {-INFINITY};
        char *expr = ecs_ptr_to_json(world, t, &value);
        test_assert(expr != NULL);
        test_str(expr, "{\"x\":\"Inf\"}");
        ecs_os_free(expr);
    }

    ecs_fini(world);
}

void SerializeToJson_struct_double() {
    typedef struct {
        ecs_f64_t x;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.name = "T",
        .members = {
            {"x", ecs_id(ecs_f64_t)}
        }
    });

    T value = {10.5};
    char *expr = ecs_ptr_to_json(world, t, &value);
    test_assert(expr != NULL);
    test_str(expr, "{\"x\":10.5}");
    ecs_os_free(expr);

    ecs_fini(world);
}

void SerializeToJson_struct_double_nan() {
    typedef struct {
        ecs_f64_t x;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.name = "T",
        .members = {
            {"x", ecs_id(ecs_f64_t)}
        }
    });

    T value = {NAN};
    char *expr = ecs_ptr_to_json(world, t, &value);
    test_assert(expr != NULL);
    test_str(expr, "{\"x\":\"NaN\"}");
    ecs_os_free(expr);

    ecs_fini(world);
}

void SerializeToJson_struct_double_inf() {
    typedef struct {
        ecs_f64_t x;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.name = "T",
        .members = {
            {"x", ecs_id(ecs_f64_t)}
        }
    });

    {
        T value = {INFINITY};
        char *expr = ecs_ptr_to_json(world, t, &value);
        test_assert(expr != NULL);
        test_str(expr, "{\"x\":\"Inf\"}");
        ecs_os_free(expr);
    }
    {
        T value = {-INFINITY};
        char *expr = ecs_ptr_to_json(world, t, &value);
        test_assert(expr != NULL);
        test_str(expr, "{\"x\":\"Inf\"}");
        ecs_os_free(expr);
    }

    ecs_fini(world);
}

void SerializeToJson_struct_double_large() {
    typedef struct {
        ecs_f64_t x;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.name = "T",
        .members = {
            {"x", ecs_id(ecs_f64_t)}
        }
    });

    T value = {60000};
    char *expr = ecs_ptr_to_json(world, t, &value);
    test_assert(expr != NULL);
    test_str(expr, "{\"x\":\"6e4\"}");
    ecs_os_free(expr);

    ecs_fini(world);
}

void SerializeToJson_struct_double_large_e() {
    typedef struct {
        ecs_f64_t x;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.name = "T",
        .members = {
            {"x", ecs_id(ecs_f64_t)}
        }
    });

    T value = {5.9722e24};
    char *expr = ecs_ptr_to_json(world, t, &value);
    test_assert(expr != NULL);
    test_str(expr, "{\"x\":\"5.9722e24\"}");
    ecs_os_free(expr);

    ecs_fini(world);
}

void SerializeToJson_struct_string() {
    typedef struct {
        char* x;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
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

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
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

void SerializeToJson_struct_entity_after_float() {
    typedef struct {
        int32_t v;
        ecs_entity_t e;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.name = "T",
        .members = {
            {"v", ecs_id(ecs_i32_t)},
            {"e", ecs_id(ecs_entity_t)}
        }
    });

    T value = {10, EcsFlecsCore};
    char *expr = ecs_ptr_to_json(world, t, &value);
    test_assert(expr != NULL);
    test_str(expr, "{\"v\":10, \"e\":\"flecs.core\"}");
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

    ecs_entity_t e = ecs_enum_init(world, &(ecs_enum_desc_t){
        .constants = {
            {"Red"}, {"Blue"}, {"Green"}
        }
    });

    test_assert(e != 0);

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
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

    ecs_entity_t b = ecs_bitmask_init(world, &(ecs_bitmask_desc_t){
        .constants = {
            {"Lettuce"}, {"Bacon"}, {"Tomato"}, {"Cheese"}, {"BLT", Bacon | Lettuce | Tomato}
        }
    });

    test_assert(b != 0);

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
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
    test_str(expr, "{\"x\":\"Bacon|Lettuce\"}");
    ecs_os_free(expr);
    }

    {
    T value = {Lettuce | Bacon | Tomato | Cheese};
    char *expr = ecs_ptr_to_json(world, t, &value);
    test_assert(expr != NULL);
    test_str(expr, "{\"x\":\"Bacon|Tomato|Lettuce|Cheese\"}");
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

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
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

    ecs_entity_t n1 = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.name = "N1",
        .members = {
            {"x", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
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

    ecs_entity_t n1 = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.name = "N1",
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
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

    ecs_entity_t n1 = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.name = "N1",
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
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

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
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

    ecs_entity_t n1 = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.name = "N1",
        .members = {
            {"x", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
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

    ecs_entity_t n1 = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.name = "N1",
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
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

    ecs_entity_t n1 = ecs_array_init(world, &(ecs_array_desc_t){
        .entity.name = "vec2",
        .type = ecs_id(ecs_i32_t),
        .count = 2
    });

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
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

    ecs_entity_t n1 = ecs_array_init(world, &(ecs_array_desc_t){
        .entity.name = "vec2",
        .type = ecs_id(ecs_i32_t),
        .count = 2
    });

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
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

void SerializeToJson_struct_partial() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.entity = ecs_id(Position),
        .members = {
            {"y", ecs_id(ecs_i32_t), .offset = offsetof(Position, y)}
        }
    });

    Position value = {10, 20};
    char *expr = ecs_ptr_to_json(world, t, &value);
    test_assert(expr != NULL);
    test_str(expr, "{\"y\":20}");
    ecs_os_free(expr);

    ecs_fini(world);
}

void SerializeToJson_array_i32_3() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_array_init(world, &(ecs_array_desc_t){
        .type = ecs_id(ecs_i32_t),
        .count = 3
    });

    int32_t value[] = {10, 20, 30};
    char *expr = ecs_ptr_to_json(world, t, value);
    test_assert(expr != NULL);
    test_str(expr, "[10, 20, 30]");
    ecs_os_free(expr);

    ecs_fini(world);
}

void SerializeToJson_array_struct_i32_i32() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.entity = ecs_id(Position),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t t = ecs_array_init(world, &(ecs_array_desc_t){
        .type = ecs_id(Position),
        .count = 3
    });

    Position value[] = {{10, 20}, {30, 40}, {50, 60}};
    char *expr = ecs_ptr_to_json(world, t, value);
    test_assert(expr != NULL);
    test_str(expr, "[{\"x\":10, \"y\":20}, {\"x\":30, \"y\":40}, {\"x\":50, \"y\":60}]");
    ecs_os_free(expr);

    ecs_fini(world);
}

void SerializeToJson_array_array_i32_3() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t a = ecs_array_init(world, &(ecs_array_desc_t){
        .type = ecs_id(ecs_i32_t),
        .count = 2
    });

    ecs_entity_t t = ecs_array_init(world, &(ecs_array_desc_t){
        .type = a,
        .count = 3
    });

    typedef int32_t A[2];
    A value[] = {{10, 20}, {30, 40}, {50, 60}};

    char *expr = ecs_ptr_to_json(world, t, value);
    test_assert(expr != NULL);
    test_str(expr, "[[10, 20], [30, 40], [50, 60]]");
    ecs_os_free(expr);

    ecs_fini(world);
}

void SerializeToJson_serialize_entity_empty() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t e = ecs_new_id(world);

    char *str = ecs_os_strdup("{\"path\":\"446\", \"ids\":[]}");
    ecs_os_sprintf(
        str, "{\"path\":\"%u\", \"ids\":[]}", (uint32_t)e);

    char *json = ecs_entity_to_json(world, e, NULL);
    test_assert(json != NULL);
    test_str(json, str);
    ecs_os_free(json);
    ecs_os_free(str);

    ecs_fini(world);
}

void SerializeToJson_serialize_entity_w_name() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t e = ecs_new_entity(world, "Foo");

    char *json = ecs_entity_to_json(world, e, NULL);
    test_assert(json != NULL);
    test_str(json, "{\"path\":\"Foo\", \"ids\":[]}");
    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeToJson_serialize_entity_w_name_1_tag() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);

    ecs_entity_t e = ecs_new_entity(world, "Foo");
    ecs_add(world, e, Tag);

    char *json = ecs_entity_to_json(world, e, NULL);
    test_assert(json != NULL);
    test_str(json, "{\"path\":\"Foo\", \"ids\":[[\"Tag\"]]}");
    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeToJson_serialize_entity_w_name_2_tags() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_entity_t e = ecs_new_entity(world, "Foo");
    ecs_add(world, e, TagA);
    ecs_add(world, e, TagB);

    char *json = ecs_entity_to_json(world, e, NULL);
    test_assert(json != NULL);
    test_str(json, 
        "{\"path\":\"Foo\", \"ids\":[[\"TagA\"], [\"TagB\"]]}");
    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeToJson_serialize_entity_w_name_1_pair() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Rel);
    ECS_TAG(world, Obj);

    ecs_entity_t e = ecs_new_entity(world, "Foo");
    ecs_add_pair(world, e, Rel, Obj);

    char *json = ecs_entity_to_json(world, e, NULL);
    test_assert(json != NULL);
    test_str(json, 
        "{\"path\":\"Foo\", \"ids\":[[\"Rel\", \"Obj\"]]}");
    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeToJson_serialize_entity_w_base() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_entity_t base = ecs_new_entity(world, "Base");
    ecs_add(world, base, TagA);

    ecs_entity_t e = ecs_new_entity(world, "Foo");
    ecs_add_pair(world, e, EcsIsA, base);
    ecs_add(world, e, TagB);

    char *json = ecs_entity_to_json(world, e, NULL);
    test_assert(json != NULL);
    test_str(json, "{"
        "\"path\":\"Foo\", "
        "\"is_a\":[{\"path\":\"Base\", \"ids\":[[\"TagA\"]]}], "
        "\"ids\":[[\"TagB\"]]"
        "}");

    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeToJson_serialize_entity_w_base_override() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_entity_t base = ecs_new_entity(world, "Base");
    ecs_add(world, base, TagA);

    ecs_entity_t e = ecs_new_entity(world, "Foo");
    ecs_add_pair(world, e, EcsIsA, base);
    ecs_add(world, e, TagA);
    ecs_add(world, e, TagB);

    ecs_entity_to_json_desc_t desc = ECS_ENTITY_TO_JSON_INIT;
    desc.serialize_hidden = true;

    char *json = ecs_entity_to_json(world, e, &desc);
    test_assert(json != NULL);
    test_str(json, "{"
        "\"path\":\"Foo\", "
        "\"is_a\":[{\"path\":\"Base\", \"ids\":[[\"TagA\"]], \"hidden\":[true]}], "
        "\"ids\":["
        "[\"TagA\"], [\"TagB\"]"
        "]}");

    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeToJson_serialize_entity_w_2_base() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_entity_t base_a = ecs_new_entity(world, "BaseA");
    ecs_entity_t base_b = ecs_new_entity(world, "BaseB");

    ecs_entity_t e = ecs_new_entity(world, "Foo");
    ecs_add_pair(world, e, EcsIsA, base_a);
    ecs_add_pair(world, e, EcsIsA, base_b);

    char *json = ecs_entity_to_json(world, e, NULL);
    test_assert(json != NULL);
    test_str(json, "{"
        "\"path\":\"Foo\", "
        "\"is_a\":[{\"path\":\"BaseA\", \"ids\":[]}, "
                  "{\"path\":\"BaseB\", \"ids\":[]}], "
        "\"ids\":[]"
        "}");

    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeToJson_serialize_entity_w_nested_base() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_entity_t base_of_base = ecs_new_entity(world, "BaseOfBase");
    ecs_entity_t base = ecs_new_entity(world, "Base");
    ecs_add_pair(world, base, EcsIsA, base_of_base);

    ecs_entity_t e = ecs_new_entity(world, "Foo");
    ecs_add_pair(world, e, EcsIsA, base);

    char *json = ecs_entity_to_json(world, e, NULL);
    test_assert(json != NULL);
    test_str(json, "{"
        "\"path\":\"Foo\", "
        "\"is_a\":[{\"path\":\"BaseOfBase\", \"ids\":[]}, "
                  "{\"path\":\"Base\", \"ids\":[]}], "
        "\"ids\":[]"
        "}");

    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeToJson_serialize_entity_w_1_component() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.name = "Position",
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t e = ecs_new_entity(world, "Foo");
    ecs_set(world, e, Position, {10, 20});

    ecs_entity_to_json_desc_t desc = ECS_ENTITY_TO_JSON_INIT;
    desc.serialize_values = true;

    char *json = ecs_entity_to_json(world, e, &desc);
    test_assert(json != NULL);
    test_str(json, "{"
        "\"path\":\"Foo\", "
        "\"ids\":[[\"Position\"]], "
        "\"values\":[{\"x\":10, \"y\":20}]"
        "}");

    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeToJson_serialize_entity_w_2_components() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.name = "Position",
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t ecs_id(Mass) = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.name = "Mass",
        .members = {
            {"value", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t e = ecs_new_entity(world, "Foo");
    ecs_set(world, e, Position, {10, 20});
    ecs_set(world, e, Mass, {1234});

    ecs_entity_to_json_desc_t desc = ECS_ENTITY_TO_JSON_INIT;
    desc.serialize_values = true;

    char *json = ecs_entity_to_json(world, e, &desc);
    test_assert(json != NULL);
    test_str(json, "{"
        "\"path\":\"Foo\", "
        "\"ids\":[[\"Position\"], [\"Mass\"]], "
        "\"values\":[{\"x\":10, \"y\":20}, {\"value\":1234}]"
        "}");

    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeToJson_serialize_entity_w_primitive_component() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t e = ecs_new_entity(world, "Foo");
    ecs_set(world, e, ecs_i32_t, {10});

    ecs_entity_to_json_desc_t desc = ECS_ENTITY_TO_JSON_INIT;
    desc.serialize_values = true;

    char *json = ecs_entity_to_json(world, e, &desc);
    test_assert(json != NULL);
    test_str(json, "{"
        "\"path\":\"Foo\", "
        "\"ids\":[[\"flecs.meta.i32\"]], "
        "\"values\":[10]"
        "}");

    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeToJson_serialize_entity_w_enum_component() {
    typedef enum {
        Red, Green, Blue
    } Color;

    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Color) = ecs_enum_init(world, &(ecs_enum_desc_t){
        .entity.name = "Color",
        .constants = {
            {"Red"}, {"Blue"}, {"Green"}
        }
    });

    test_assert(ecs_id(Color) != 0);

    ecs_entity_t e = ecs_new_entity(world, "Foo");
    ecs_set(world, e, Color, {1});

    ecs_entity_to_json_desc_t desc = ECS_ENTITY_TO_JSON_INIT;
    desc.serialize_values = true;

    char *json = ecs_entity_to_json(world, e, &desc);
    test_assert(json != NULL);
    test_str(json, "{"
        "\"path\":\"Foo\", "
        "\"ids\":[[\"Color\"]], "
        "\"values\":[\"Blue\"]"
        "}");

    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeToJson_serialize_entity_w_struct_and_enum_component() {
    typedef enum {
        Red, Green, Blue
    } Color;

    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.name = "Position",
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    test_assert(ecs_id(Position) != 0);

    ecs_entity_t ecs_id(Color) = ecs_enum_init(world, &(ecs_enum_desc_t){
        .entity.name = "Color",
        .constants = {
            {"Red"}, {"Blue"}, {"Green"}
        }
    });

    test_assert(ecs_id(Color) != 0);

    ecs_entity_t e = ecs_new_entity(world, "Foo");
    ecs_set(world, e, Color, {1});
    ecs_set(world, e, Position, {10, 20});

    ecs_entity_to_json_desc_t desc = ECS_ENTITY_TO_JSON_INIT;
    desc.serialize_values = true;

    char *json = ecs_entity_to_json(world, e, &desc);
    test_assert(json != NULL);
    test_str(json, "{"
        "\"path\":\"Foo\", "
        "\"ids\":[[\"Position\"], [\"Color\"]], "
        "\"values\":[{\"x\":10, \"y\":20}, \"Blue\"]"
        "}");

    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeToJson_serialize_entity_w_invalid_enum_component() {
    typedef enum {
        Red, Green, Blue
    } Color;

    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Color) = ecs_enum_init(world, &(ecs_enum_desc_t){
        .entity.name = "Color",
        .constants = {
            {"Red"}, {"Blue"}, {"Green"}
        }
    });

    test_assert(ecs_id(Color) != 0);

    ecs_entity_t e = ecs_new_entity(world, "Foo");
    ecs_set(world, e, Color, {100});

    ecs_entity_to_json_desc_t desc = ECS_ENTITY_TO_JSON_INIT;
    desc.serialize_values = true;

    char *json = ecs_entity_to_json(world, e, &desc);
    test_assert(json == NULL);

    ecs_fini(world);
}

void SerializeToJson_serialize_entity_w_type_info() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.name = "Position",
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t e = ecs_new_entity(world, "Foo");
    ecs_set(world, e, Position, {10, 20});

    ecs_entity_to_json_desc_t desc = ECS_ENTITY_TO_JSON_INIT;
    desc.serialize_values = true;
    desc.serialize_type_info = true;

    char *json = ecs_entity_to_json(world, e, &desc);
    test_assert(json != NULL);
    test_str(json, "{"
        "\"path\":\"Foo\", "
        "\"ids\":[[\"Position\"]], "
        "\"values\":[{\"x\":10, \"y\":20}], "
        "\"type_info\":[{\"x\":[\"int\"], \"y\":[\"int\"]}]"
        "}");

    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeToJson_serialize_entity_w_type_info_unit() {
    typedef struct {
        ecs_i32_t value;    
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t u = ecs_unit_init(world, &(ecs_unit_desc_t){
        .entity.name = "celsius",
        .symbol = "°"
    });
    test_assert(u != 0);

    ecs_entity_t ecs_id(T) = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.name = "T",
        .members = {
            {"value", ecs_id(ecs_i32_t), .unit = u}
        }
    });
    test_assert(ecs_id(T) != 0);

    ecs_entity_t e = ecs_new_entity(world, "Foo");
    ecs_set(world, e, T, {24});

    ecs_entity_to_json_desc_t desc = ECS_ENTITY_TO_JSON_INIT;
    desc.serialize_values = true;
    desc.serialize_type_info = true;

    char *json = ecs_entity_to_json(world, e, &desc);
    test_assert(json != NULL);
    test_str(json, "{"
        "\"path\":\"Foo\", "
        "\"ids\":[[\"T\"]], "
        "\"values\":[{\"value\":24}], "
        "\"type_info\":[{\"value\":[\"int\", {"
            "\"unit\":\"celsius\", \"symbol\":\"°\"}]"
        "}]"
        "}");

    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeToJson_serialize_entity_w_type_info_unit_quantity() {
    typedef struct {
        ecs_i32_t value;    
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t q = ecs_quantity_init(world, &(ecs_entity_desc_t){
        .name = "temperature"
    });
    test_assert(q != 0);

    ecs_entity_t u = ecs_unit_init(world, &(ecs_unit_desc_t){
        .entity.name = "celsius",
        .symbol = "°",
        .quantity = q
    });
    test_assert(u != 0);

    ecs_entity_t ecs_id(T) = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.name = "T",
        .members = {
            {"value", ecs_id(ecs_i32_t), .unit = u}
        }
    });
    test_assert(ecs_id(T) != 0);

    ecs_entity_t e = ecs_new_entity(world, "Foo");
    ecs_set(world, e, T, {24});

    ecs_entity_to_json_desc_t desc = ECS_ENTITY_TO_JSON_INIT;
    desc.serialize_values = true;
    desc.serialize_type_info = true;

    char *json = ecs_entity_to_json(world, e, &desc);
    test_assert(json != NULL);
    test_str(json, "{"
        "\"path\":\"Foo\", "
        "\"ids\":[[\"T\"]], "
        "\"values\":[{\"value\":24}], "
        "\"type_info\":[{\"value\":[\"int\", {"
            "\"unit\":\"celsius\", \"symbol\":\"°\", \"quantity\":\"temperature\"}]"
        "}]"
        "}");

    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeToJson_serialize_entity_w_type_info_unit_over() {
    typedef struct {
        ecs_i32_t value;    
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t u_1 = ecs_unit_init(world, &(ecs_unit_desc_t){
        .entity.name = "seconds",
        .symbol = "s"
    });
    test_assert(u_1 != 0);

    ecs_entity_t u_2 = ecs_unit_init(world, &(ecs_unit_desc_t){
        .entity.name = "meters",
        .symbol = "m"
    });
    test_assert(u_2 != 0);

    ecs_entity_t u_3 = ecs_unit_init(world, &(ecs_unit_desc_t){
        .entity.name = "meters_per_second",
        .symbol = "m/s",
        .base = u_2,
        .over = u_1
    });
    test_assert(u_3 != 0);

    ecs_entity_t ecs_id(T) = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.name = "T",
        .members = {
            {"value", ecs_id(ecs_i32_t), .unit = u_3}
        }
    });
    test_assert(ecs_id(T) != 0);

    ecs_entity_t e = ecs_new_entity(world, "Foo");
    ecs_set(world, e, T, {24});

    ecs_entity_to_json_desc_t desc = ECS_ENTITY_TO_JSON_INIT;
    desc.serialize_values = true;
    desc.serialize_type_info = true;

    char *json = ecs_entity_to_json(world, e, &desc);
    test_assert(json != NULL);
    test_assert(json != NULL);
    test_str(json, "{"
        "\"path\":\"Foo\", "
        "\"ids\":[[\"T\"]], "
        "\"values\":[{\"value\":24}], "
        "\"type_info\":[{\"value\":[\"int\", {"
            "\"unit\":\"meters_per_second\", \"symbol\":\"m/s\"}]"
        "}]"
        "}");

    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeToJson_serialize_entity_wo_private() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);

    ecs_add_id(world, Tag, EcsPrivate);

    ecs_entity_t e = ecs_new_entity(world, "Foo");
    ecs_add(world, e, Tag);

    char *json = ecs_entity_to_json(world, e, NULL);
    test_assert(json != NULL);
    test_str(json, "{"
        "\"path\":\"Foo\", "
        "\"ids\":[]}");

    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeToJson_serialize_entity_w_private() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);

    ecs_add_id(world, Tag, EcsPrivate);

    ecs_entity_t e = ecs_new_entity(world, "Foo");
    ecs_add(world, e, Tag);

    ecs_entity_to_json_desc_t desc = ECS_ENTITY_TO_JSON_INIT;
    desc.serialize_private = true;

    char *json = ecs_entity_to_json(world, e, &desc);
    test_assert(json != NULL);
    test_assert(json != NULL);
    test_str(json, "{"
        "\"path\":\"Foo\", "
        "\"ids\":[[\"Tag\"]]}");

    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeToJson_serialize_entity_w_label() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);

    ecs_entity_t e = ecs_new_entity(world, "Foo");
    ecs_add(world, e, Tag);
    ecs_doc_set_name(world, e, "My name");

    ecs_entity_to_json_desc_t desc = ECS_ENTITY_TO_JSON_INIT;
    desc.serialize_label = true;
    char *json = ecs_entity_to_json(world, e, &desc);

    test_assert(json != NULL);
    test_str(json, "{"
        "\"path\":\"Foo\", "
        "\"label\":\"My name\", "
        "\"ids\":["
            "[\"Tag\"]"
        "]}");

    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeToJson_serialize_entity_w_label_no_name() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);

    ecs_entity_t e = ecs_new_id(world);
    ecs_add(world, e, Tag);

    char *str = ecs_os_malloc(
        ecs_os_strlen(
            "{"
                "\"path\":\"000\", "
                "\"label\":\"000\", "
                "\"ids\":["
                    "[\"Tag\"]"
            "]}"
        ) + 1
    );

    ecs_os_sprintf(str, 
        "{"
            "\"path\":\"%u\", "
            "\"label\":\"%u\", "
            "\"ids\":["
                "[\"Tag\"]"
        "]}", (uint32_t)e,  (uint32_t)e);

    ecs_entity_to_json_desc_t desc = ECS_ENTITY_TO_JSON_INIT;
    desc.serialize_label = true;
    char *json = ecs_entity_to_json(world, e, &desc);
    test_assert(json != NULL);
    test_str(json, str);

    ecs_os_free(json);
    ecs_os_free(str);

    ecs_fini(world);
}

void SerializeToJson_serialize_entity_w_id_labels() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);

    ecs_entity_t e = ecs_new_entity(world, "Foo");
    ecs_add(world, e, Tag);
    ecs_doc_set_name(world, Tag, "My tag");

    ecs_entity_to_json_desc_t desc = ECS_ENTITY_TO_JSON_INIT;
    desc.serialize_id_labels = true;
    char *json = ecs_entity_to_json(world, e, &desc);
    test_assert(json != NULL);

    test_str(json, "{"
        "\"path\":\"Foo\", "
        "\"ids\":["
            "[\"Tag\"]"
        "], "
        "\"id_labels\":["
            "[\"My tag\"]"
        "]}");

    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeToJson_serialize_entity_w_brief() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);

    ecs_entity_t e = ecs_new_entity(world, "Foo");
    ecs_add(world, e, Tag);
    ecs_doc_set_brief(world, e, "Brief description");

    ecs_entity_to_json_desc_t desc = ECS_ENTITY_TO_JSON_INIT;
    desc.serialize_brief = true;
    char *json = ecs_entity_to_json(world, e, &desc);
    test_assert(json != NULL);

    test_str(json, "{"
        "\"path\":\"Foo\", "
        "\"brief\":\"Brief description\", "
        "\"ids\":["
            "[\"Tag\"]"
        "]}");

    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeToJson_serialize_entity_w_brief_no_brief() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);

    ecs_entity_t e = ecs_new_entity(world, "Foo");
    ecs_add(world, e, Tag);

    ecs_entity_to_json_desc_t desc = ECS_ENTITY_TO_JSON_INIT;
    desc.serialize_brief = true;
    char *json = ecs_entity_to_json(world, e, &desc);
    test_assert(json != NULL);

    test_str(json, "{"
        "\"path\":\"Foo\", "
        "\"ids\":["
            "[\"Tag\"]"
        "]}");

    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeToJson_serialize_entity_w_link() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);

    ecs_entity_t e = ecs_new_entity(world, "Foo");
    ecs_add(world, e, Tag);
    ecs_doc_set_link(world, e, "Link");

    ecs_entity_to_json_desc_t desc = ECS_ENTITY_TO_JSON_INIT;
    desc.serialize_link = true;
    char *json = ecs_entity_to_json(world, e, &desc);
    test_assert(json != NULL);

    test_str(json, "{"
        "\"path\":\"Foo\", "
        "\"link\":\"Link\", "
        "\"ids\":["
            "[\"Tag\"]"
        "]}");

    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeToJson_serialize_entity_w_link_no_link() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);

    ecs_entity_t e = ecs_new_entity(world, "Foo");
    ecs_add(world, e, Tag);

    ecs_entity_to_json_desc_t desc = ECS_ENTITY_TO_JSON_INIT;
    desc.serialize_link = true;
    char *json = ecs_entity_to_json(world, e, &desc);
    test_assert(json != NULL);

    test_str(json, "{"
        "\"path\":\"Foo\", "
        "\"ids\":["
            "[\"Tag\"]"
        "]}");

    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeToJson_serialize_entity_color() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);

    ecs_entity_t e = ecs_new_entity(world, "Foo");
    ecs_add(world, e, Tag);
    ecs_doc_set_color(world, e, "#47B576");

    ecs_entity_to_json_desc_t desc = ECS_ENTITY_TO_JSON_INIT;
    desc.serialize_color = true;
    char *json = ecs_entity_to_json(world, e, &desc);
    test_assert(json != NULL);

    test_str(json, "{"
        "\"path\":\"Foo\", "
        "\"color\":\"#47B576\", "
        "\"ids\":["
            "[\"Tag\"]"
        "]}");

    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeToJson_serialize_entity_w_meta_ids() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);

    ecs_entity_t p = ecs_new_entity(world, "Parent");
    ecs_entity_t e = ecs_new_entity(world, "Child");
    ecs_add(world, e, Tag);
    ecs_add_pair(world, e, EcsChildOf, p);
    ecs_doc_set_name(world, e, "Doc name");

    {
        ecs_entity_to_json_desc_t desc = ECS_ENTITY_TO_JSON_INIT;
        char *json = ecs_entity_to_json(world, e, &desc);
        test_assert(json != NULL);

        test_str(json, "{"
            "\"path\":\"Parent.Child\", "
            "\"ids\":["
                "[\"Tag\"]"
            "]}");

        ecs_os_free(json);
    }
    {
        ecs_entity_to_json_desc_t desc = ECS_ENTITY_TO_JSON_INIT;
        desc.serialize_meta_ids = true;
        char *json = ecs_entity_to_json(world, e, &desc);
        test_assert(json != NULL);

        test_str(json, "{"
            "\"path\":\"Parent.Child\", "
            "\"ids\":["
                "[\"Tag\"], "
                "[\"flecs.core.Identifier\", \"flecs.core.Name\"], "
                "[\"flecs.core.ChildOf\", \"Parent\"], "
                "[\"flecs.doc.Description\", \"flecs.core.Name\"]"
            "]}");

        ecs_os_free(json);
    }

    ecs_fini(world);
}

void SerializeToJson_serialize_iterator_1_comps_empty() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_struct_init(world, &(ecs_struct_desc_t){
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

    ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.entity = ecs_id(Position),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t e1 = ecs_new_entity(world, "Foo");
    ecs_entity_t e2 = ecs_new_entity(world, "Bar");

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

    ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.entity = ecs_id(Position),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.entity = ecs_id(Mass),
        .members = {
            {"value", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t e1 = ecs_new_entity(world, "Foo");
    ecs_entity_t e2 = ecs_new_entity(world, "Bar");

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

    ecs_entity_t e1 = ecs_new_entity(world, "Foo");
    ecs_entity_t e2 = ecs_new_entity(world, "Bar");

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

    ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.entity = ecs_id(Position),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t e1 = ecs_new_entity(world, "Foo");
    ecs_entity_t e2 = ecs_new_entity(world, "Bar");

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

    ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.entity = ecs_id(Position),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t e1 = ecs_new_entity(world, "Foo");
    ecs_entity_t e2 = ecs_new_entity(world, "Bar");
    ecs_entity_t e3 = ecs_new_entity(world, "Hello");
    ecs_entity_t e4 = ecs_new_entity(world, "World");

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

void SerializeToJson_serialize_iterator_2_comps_1_owned_2_ents() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Mass);

    ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.entity = ecs_id(Position),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.entity = ecs_id(Mass),
        .members = {
            {"value", ecs_id(ecs_i32_t)},
        }
    });

    ecs_entity_t base = ecs_new_entity(world, "Base");
    ecs_entity_t e1 = ecs_new_entity(world, "Foo");
    ecs_entity_t e2 = ecs_new_entity(world, "Bar");

    ecs_set(world, base, Mass, {100});
    ecs_set(world, e1, Position, {10, 20});
    ecs_set(world, e2, Position, {30, 40});

    ecs_add_pair(world, e1, EcsIsA, base);
    ecs_add_pair(world, e2, EcsIsA, base);

    ecs_query_t *q = ecs_query_new(world, "Position, Mass(super)");
    ecs_iter_t it = ecs_query_iter(world, q);

    char *json = ecs_iter_to_json(world, &it, NULL);
    test_str(json, 
    "{"
        "\"ids\":[\"Position\", \"Mass\"], "
        "\"results\":[{"
            "\"ids\":[\"Position\", \"Mass\"], "
            "\"subjects\":[0, \"Base\"], "
            "\"is_set\":[true, true], "
            "\"entities\":["
                "\"Foo\", \"Bar\""
            "], "
            "\"values\":[["
                "{\"x\":10, \"y\":20}, "
                "{\"x\":30, \"y\":40}"
            "], {"
                "\"value\":100"
            "}]"
        "}]"
    "}");

    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeToJson_serialize_iterator_w_pair_wildcard() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Rel);
    ECS_TAG(world, ObjA);
    ECS_TAG(world, ObjB);

    ecs_entity_t e1 = ecs_new_entity(world, "Foo");
    ecs_entity_t e2 = ecs_new_entity(world, "Bar");

    ecs_add_pair(world, e1, Rel, ObjA);
    ecs_add_pair(world, e2, Rel, ObjB);

    ecs_query_t *q = ecs_query_new(world, "(Rel, *)");
    ecs_iter_t it = ecs_query_iter(world, q);

    char *json = ecs_iter_to_json(world, &it, NULL);
    test_str(json, 
    "{"
        "\"ids\":[\"(Rel,*)\"], "
        "\"results\":[{"
            "\"ids\":[\"(Rel,ObjA)\"], "
            "\"subjects\":[0], "
            "\"is_set\":[true], "
            "\"entities\":["
                "\"Foo\""
            "], "
            "\"values\":[0]"
        "}, {"
            "\"ids\":[\"(Rel,ObjB)\"], "
            "\"subjects\":[0], "
            "\"is_set\":[true], "
            "\"entities\":["
                "\"Bar\""
            "], "
            "\"values\":[0]"
        "}]"
    "}");

    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeToJson_serialize_iterator_w_var() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Rel);
    ECS_TAG(world, ObjA);
    ECS_TAG(world, ObjB);

    ecs_entity_t e1 = ecs_new_entity(world, "Foo");
    ecs_entity_t e2 = ecs_new_entity(world, "Bar");

    ecs_add_pair(world, e1, Rel, ObjA);
    ecs_add_pair(world, e2, Rel, ObjB);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "(Rel, $X)"
    });

    ecs_iter_t it = ecs_rule_iter(world, r);

    char *json = ecs_iter_to_json(world, &it, NULL);
    test_str(json, 
    "{"
        "\"ids\":[\"(Rel,*)\"], "
        "\"vars\":[\"X\"], "
        "\"results\":[{"
            "\"ids\":[\"(Rel,ObjA)\"], "
            "\"subjects\":[0], "
            "\"vars\":[\"ObjA\"], "
            "\"is_set\":[true], "
            "\"entities\":["
                "\"Foo\""
            "], "
            "\"values\":[0]"
        "}, {"
            "\"ids\":[\"(Rel,ObjB)\"], "
            "\"subjects\":[0], "
            "\"vars\":[\"ObjB\"], "
            "\"is_set\":[true], "
            "\"entities\":["
                "\"Bar\""
            "], "
            "\"values\":[0]"
        "}]"
    "}");

    ecs_os_free(json);

    ecs_rule_fini(r);

    ecs_fini(world);
}

void SerializeToJson_serialize_iterator_w_2_vars() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelX);
    ECS_TAG(world, ObjA);
    ECS_TAG(world, ObjB);

    ECS_TAG(world, RelY);
    ECS_TAG(world, ObjC);
    ECS_TAG(world, ObjD);

    ecs_entity_t e1 = ecs_new_entity(world, "Foo");
    ecs_entity_t e2 = ecs_new_entity(world, "Bar");

    ecs_add_pair(world, e1, RelX, ObjA);
    ecs_add_pair(world, e2, RelX, ObjB);

    ecs_add_pair(world, e1, RelY, ObjC);
    ecs_add_pair(world, e2, RelY, ObjD);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "(RelX, $X), (RelY, $Y)"
    });

    ecs_iter_t it = ecs_rule_iter(world, r);

    char *json = ecs_iter_to_json(world, &it, NULL);
    test_str(json, 
    "{"
        "\"ids\":[\"(RelX,*)\", \"(RelY,*)\"], "
        "\"vars\":[\"X\", \"Y\"], "
        "\"results\":[{"
            "\"ids\":[\"(RelX,ObjA)\", \"(RelY,ObjC)\"], "
            "\"subjects\":[0, 0], "
            "\"vars\":[\"ObjA\", \"ObjC\"], "
            "\"is_set\":[true, true], "
            "\"entities\":["
                "\"Foo\""
            "], "
            "\"values\":[0, 0]"
        "}, {"
            "\"ids\":[\"(RelX,ObjB)\", \"(RelY,ObjD)\"], "
            "\"subjects\":[0, 0], "
            "\"vars\":[\"ObjB\", \"ObjD\"], "
            "\"is_set\":[true, true], "
            "\"entities\":["
                "\"Bar\""
            "], "
            "\"values\":[0, 0]"
        "}]"
    "}");

    ecs_os_free(json);

    ecs_rule_fini(r);

    ecs_fini(world);
}

void SerializeToJson_serialize_iterator_type_info_1_tags() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);

    ecs_entity_t e1 = ecs_new_entity(world, "Foo");
    ecs_entity_t e2 = ecs_new_entity(world, "Bar");

    ecs_add(world, e1, TagA);
    ecs_add(world, e2, TagA);

    ecs_query_t *q = ecs_query_new(world, "TagA");
    ecs_iter_t it = ecs_query_iter(world, q);

    ecs_iter_to_json_desc_t desc = ECS_ITER_TO_JSON_INIT;
    desc.serialize_type_info = true;
    char *json = ecs_iter_to_json(world, &it, &desc);

    test_str(json, 
    "{"
        "\"ids\":[\"TagA\"], "
        "\"type_info\":{\"TagA\":0}, "
        "\"results\":[{"
            "\"ids\":[\"TagA\"], "
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

void SerializeToJson_serialize_iterator_type_info_2_tags() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_entity_t e1 = ecs_new_entity(world, "Foo");
    ecs_entity_t e2 = ecs_new_entity(world, "Bar");

    ecs_add(world, e1, TagA);
    ecs_add(world, e1, TagB);
    ecs_add(world, e2, TagA);
    ecs_add(world, e2, TagB);

    ecs_query_t *q = ecs_query_new(world, "TagA, TagB");
    ecs_iter_t it = ecs_query_iter(world, q);

    ecs_iter_to_json_desc_t desc = ECS_ITER_TO_JSON_INIT;
    desc.serialize_type_info = true;
    char *json = ecs_iter_to_json(world, &it, &desc);

    test_str(json, 
    "{"
        "\"ids\":[\"TagA\", \"TagB\"], "
        "\"type_info\":{\"TagA\":0, \"TagB\":0}, "
        "\"results\":[{"
            "\"ids\":[\"TagA\", \"TagB\"], "
            "\"subjects\":[0, 0], "
            "\"is_set\":[true, true], "
            "\"entities\":["
                "\"Foo\", \"Bar\""
            "], "
            "\"values\":[0, 0]"
        "}]"
    "}");

    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeToJson_serialize_iterator_type_info_1_component() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e1 = ecs_new_entity(world, "Foo");
    ecs_entity_t e2 = ecs_new_entity(world, "Bar");

    ecs_add(world, e1, Position);
    ecs_add(world, e2, Position);

    ecs_query_t *q = ecs_query_new(world, "Position");
    ecs_iter_t it = ecs_query_iter(world, q);

    ecs_iter_to_json_desc_t desc = ECS_ITER_TO_JSON_INIT;
    desc.serialize_type_info = true;
    char *json = ecs_iter_to_json(world, &it, &desc);

    test_str(json, 
    "{"
        "\"ids\":[\"Position\"], "
        "\"type_info\":{\"Position\":0}, "
        "\"results\":[{"
            "\"ids\":[\"Position\"], "
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

void SerializeToJson_serialize_iterator_type_info_2_components() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t e1 = ecs_new_entity(world, "Foo");
    ecs_entity_t e2 = ecs_new_entity(world, "Bar");

    ecs_add(world, e1, Position);
    ecs_add(world, e2, Position);

    ecs_add(world, e1, Velocity);
    ecs_add(world, e2, Velocity);

    ecs_query_t *q = ecs_query_new(world, "Position, Velocity");
    ecs_iter_t it = ecs_query_iter(world, q);

    ecs_iter_to_json_desc_t desc = ECS_ITER_TO_JSON_INIT;
    desc.serialize_type_info = true;
    char *json = ecs_iter_to_json(world, &it, &desc);

    test_str(json, 
    "{"
        "\"ids\":[\"Position\", \"Velocity\"], "
        "\"type_info\":{\"Position\":0, \"Velocity\":0}, "
        "\"results\":[{"
            "\"ids\":[\"Position\", \"Velocity\"], "
            "\"subjects\":[0, 0], "
            "\"is_set\":[true, true], "
            "\"entities\":["
                "\"Foo\", \"Bar\""
            "], "
            "\"values\":[0, 0]"
        "}]"
    "}");

    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeToJson_serialize_iterator_type_info_1_struct() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.name = "Position",
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    test_assert(t == ecs_id(Position));

    ecs_entity_t e1 = ecs_new_entity(world, "Foo");
    ecs_entity_t e2 = ecs_new_entity(world, "Bar");

    ecs_add(world, e1, Position);
    ecs_add(world, e2, Position);

    ecs_query_t *q = ecs_query_new(world, "Position");
    ecs_iter_t it = ecs_query_iter(world, q);

    ecs_iter_to_json_desc_t desc = ECS_ITER_TO_JSON_INIT;
    desc.serialize_type_info = true;
    char *json = ecs_iter_to_json(world, &it, &desc);

    test_str(json, 
    "{"
        "\"ids\":[\"Position\"], "
        "\"type_info\":{\"Position\":{\"x\":[\"int\"], \"y\":[\"int\"]}}, "
        "\"results\":[{"
            "\"ids\":[\"Position\"], "
            "\"subjects\":[0], "
            "\"is_set\":[true], "
            "\"entities\":["
                "\"Foo\", \"Bar\""
            "], "
            "\"values\":[["
                "{\"x\":0, \"y\":0}, "
                "{\"x\":0, \"y\":0}"
            "]]"
        "}]"
    "}");

    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeToJson_serialize_iterator_type_info_1_component_1_struct() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.name = "Position",
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    test_assert(t == ecs_id(Position));

    ecs_entity_t e1 = ecs_new_entity(world, "Foo");
    ecs_entity_t e2 = ecs_new_entity(world, "Bar");

    ecs_add(world, e1, Position);
    ecs_add(world, e2, Position);

    ecs_add(world, e1, Velocity);
    ecs_add(world, e2, Velocity);

    ecs_query_t *q = ecs_query_new(world, "Position, Velocity");
    ecs_iter_t it = ecs_query_iter(world, q);

    ecs_iter_to_json_desc_t desc = ECS_ITER_TO_JSON_INIT;
    desc.serialize_type_info = true;
    char *json = ecs_iter_to_json(world, &it, &desc);

    test_str(json, 
    "{"
        "\"ids\":[\"Position\", \"Velocity\"], "
        "\"type_info\":{"
            "\"Position\":{\"x\":[\"int\"], \"y\":[\"int\"]}, "
            "\"Velocity\":0"
        "}, "
        "\"results\":[{"
            "\"ids\":[\"Position\", \"Velocity\"], "
            "\"subjects\":[0, 0], "
            "\"is_set\":[true, true], "
            "\"entities\":["
                "\"Foo\", \"Bar\""
            "], "
            "\"values\":[["
                "{\"x\":0, \"y\":0}, "
                "{\"x\":0, \"y\":0}"
            "], 0]"
        "}]"
    "}");

    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeToJson_serialize_iterator_type_info_2_structs() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.name = "Position",
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });
    test_assert(t == ecs_id(Position));

    t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.name = "Velocity",
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });
    test_assert(t == ecs_id(Velocity));

    ecs_entity_t e1 = ecs_new_entity(world, "Foo");
    ecs_entity_t e2 = ecs_new_entity(world, "Bar");

    ecs_add(world, e1, Position);
    ecs_add(world, e2, Position);

    ecs_add(world, e1, Velocity);
    ecs_add(world, e2, Velocity);

    ecs_query_t *q = ecs_query_new(world, "Position, Velocity");
    ecs_iter_t it = ecs_query_iter(world, q);

    ecs_iter_to_json_desc_t desc = ECS_ITER_TO_JSON_INIT;
    desc.serialize_type_info = true;
    char *json = ecs_iter_to_json(world, &it, &desc);

    test_str(json, 
    "{"
        "\"ids\":[\"Position\", \"Velocity\"], "
        "\"type_info\":{"
            "\"Position\":{\"x\":[\"int\"], \"y\":[\"int\"]}, "
            "\"Velocity\":{\"x\":[\"int\"], \"y\":[\"int\"]}"
        "}, "
        "\"results\":[{"
            "\"ids\":[\"Position\", \"Velocity\"], "
            "\"subjects\":[0, 0], "
            "\"is_set\":[true, true], "
            "\"entities\":["
                "\"Foo\", \"Bar\""
            "], "
            "\"values\":[["
                "{\"x\":0, \"y\":0}, "
                "{\"x\":0, \"y\":0}"
            "], ["
                "{\"x\":0, \"y\":0}, "
                "{\"x\":0, \"y\":0}"
            "]]"
        "}]"
    "}");

    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeToJson_serialize_iterator_type_info_w_unit() {
    typedef struct {
        ecs_i32_t value;    
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t u = ecs_unit_init(world, &(ecs_unit_desc_t){
        .entity.name = "celsius",
        .symbol = "°"
    });
    test_assert(u != 0);

    ecs_entity_t ecs_id(T) = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.name = "T",
        .members = {
            {"value", ecs_id(ecs_i32_t), .unit = u}
        }
    });
    test_assert(ecs_id(T) != 0);

    ecs_entity_t e1 = ecs_new_entity(world, "Foo");
    ecs_entity_t e2 = ecs_new_entity(world, "Bar");

    ecs_set(world, e1, T, {24});
    ecs_set(world, e2, T, {16});

    ecs_query_t *q = ecs_query_new(world, "T");
    ecs_iter_t it = ecs_query_iter(world, q);

    ecs_iter_to_json_desc_t desc = ECS_ITER_TO_JSON_INIT;
    desc.serialize_type_info = true;
    char *json = ecs_iter_to_json(world, &it, &desc);

    test_str(json, 
    "{"
        "\"ids\":[\"T\"], "
        "\"type_info\":{"
            "\"T\":{\"value\":[\"int\", {"
                "\"unit\":\"celsius\", \"symbol\":\"°\"}]"
            "}"
        "}, "
        "\"results\":[{"
            "\"ids\":[\"T\"], "
            "\"subjects\":[0], "
            "\"is_set\":[true], "
            "\"entities\":["
                "\"Foo\", \"Bar\""
            "], "
            "\"values\":[["
                "{\"value\":24}, "
                "{\"value\":16}"
            "]]"
        "}]"
    "}");

    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeToJson_serialize_iterator_type_info_w_unit_quantity() {
    typedef struct {
        ecs_i32_t value;    
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t qt = ecs_quantity_init(world, &(ecs_entity_desc_t){
        .name = "temperature"
    });
    test_assert(qt != 0);

    ecs_entity_t u = ecs_unit_init(world, &(ecs_unit_desc_t){
        .entity.name = "celsius",
        .symbol = "°",
        .quantity = qt
    });
    test_assert(u != 0);

    ecs_entity_t ecs_id(T) = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.name = "T",
        .members = {
            {"value", ecs_id(ecs_i32_t), .unit = u}
        }
    });
    test_assert(ecs_id(T) != 0);

    ecs_entity_t e1 = ecs_new_entity(world, "Foo");
    ecs_entity_t e2 = ecs_new_entity(world, "Bar");

    ecs_set(world, e1, T, {24});
    ecs_set(world, e2, T, {16});

    ecs_query_t *q = ecs_query_new(world, "T");
    ecs_iter_t it = ecs_query_iter(world, q);

    ecs_iter_to_json_desc_t desc = ECS_ITER_TO_JSON_INIT;
    desc.serialize_type_info = true;
    char *json = ecs_iter_to_json(world, &it, &desc);

    test_str(json, 
    "{"
        "\"ids\":[\"T\"], "
        "\"type_info\":{"
            "\"T\":{\"value\":[\"int\", {"
                "\"unit\":\"celsius\", \"symbol\":\"°\", \"quantity\":\"temperature\"}]"
            "}"
        "}, "
        "\"results\":[{"
            "\"ids\":[\"T\"], "
            "\"subjects\":[0], "
            "\"is_set\":[true], "
            "\"entities\":["
                "\"Foo\", \"Bar\""
            "], "
            "\"values\":[["
                "{\"value\":24}, "
                "{\"value\":16}"
            "]]"
        "}]"
    "}");

    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeToJson_serialize_iterator_type_info_w_unit_over() {
    typedef struct {
        ecs_i32_t value;    
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t u_1 = ecs_unit_init(world, &(ecs_unit_desc_t){
        .entity.name = "seconds",
        .symbol = "s"
    });
    test_assert(u_1 != 0);

    ecs_entity_t u_2 = ecs_unit_init(world, &(ecs_unit_desc_t){
        .entity.name = "meters",
        .symbol = "m"
    });
    test_assert(u_2 != 0);

    ecs_entity_t u_3 = ecs_unit_init(world, &(ecs_unit_desc_t){
        .entity.name = "meters_per_second",
        .base = u_2,
        .over = u_1
    });
    test_assert(u_3 != 0);

    ecs_entity_t ecs_id(T) = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.name = "T",
        .members = {
            {"value", ecs_id(ecs_i32_t), .unit = u_3}
        }
    });
    test_assert(ecs_id(T) != 0);

    ecs_entity_t e1 = ecs_new_entity(world, "Foo");
    ecs_entity_t e2 = ecs_new_entity(world, "Bar");

    ecs_set(world, e1, T, {24});
    ecs_set(world, e2, T, {16});

    ecs_query_t *q = ecs_query_new(world, "T");
    ecs_iter_t it = ecs_query_iter(world, q);

    ecs_iter_to_json_desc_t desc = ECS_ITER_TO_JSON_INIT;
    desc.serialize_type_info = true;
    char *json = ecs_iter_to_json(world, &it, &desc);

    test_str(json, 
    "{"
        "\"ids\":[\"T\"], "
        "\"type_info\":{"
            "\"T\":{\"value\":[\"int\", {"
                "\"unit\":\"meters_per_second\", \"symbol\":\"m/s\"}]"
            "}"
        "}, "
        "\"results\":[{"
            "\"ids\":[\"T\"], "
            "\"subjects\":[0], "
            "\"is_set\":[true], "
            "\"entities\":["
                "\"Foo\", \"Bar\""
            "], "
            "\"values\":[["
                "{\"value\":24}, "
                "{\"value\":16}"
            "]]"
        "}]"
    "}");

    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeToJson_serialize_iterator_w_entity_label() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);

    ecs_entity_t e1 = ecs_new_entity(world, "foo_bar");
    ecs_entity_t e2 = ecs_new_entity(world, "hello_world");
    ecs_doc_set_name(world, e2, "Hello World");

    ecs_add(world, e1, Tag);
    ecs_add(world, e2, Tag);

    ecs_query_t *q = ecs_query_new(world, "Tag");
    ecs_iter_t it = ecs_query_iter(world, q);

    ecs_iter_to_json_desc_t desc = ECS_ITER_TO_JSON_INIT;
    desc.serialize_entity_labels = true;
    char *json = ecs_iter_to_json(world, &it, &desc);

    test_str(json, 
    "{"
        "\"ids\":[\"Tag\"], "
        "\"results\":[{"
            "\"ids\":[\"Tag\"], "
            "\"subjects\":[0], "
            "\"is_set\":[true], "
            "\"entities\":["
                "\"foo_bar\""
            "], "
            "\"entity_labels\":["
                "\"foo_bar\""
            "], "
            "\"values\":[0]"
        "}, {"
            "\"ids\":[\"Tag\"], "
            "\"subjects\":[0], "
            "\"is_set\":[true], "
            "\"entities\":["
                "\"hello_world\""
            "], "
            "\"entity_labels\":["
                "\"Hello World\""
            "], "
            "\"values\":[0]"
        "}]"
    "}");

    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeToJson_serialize_iterator_w_var_labels() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Rel);
    ECS_TAG(world, ObjA);
    ECS_TAG(world, ObjB);

    ecs_entity_t e1 = ecs_new_entity(world, "Foo");
    ecs_entity_t e2 = ecs_new_entity(world, "Bar");

    ecs_add_pair(world, e1, Rel, ObjA);
    ecs_add_pair(world, e2, Rel, ObjB);

    ecs_doc_set_name(world, ObjA, "Object A");

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "(Rel, $X)"
    });

    ecs_iter_t it = ecs_rule_iter(world, r);

    ecs_iter_to_json_desc_t desc = ECS_ITER_TO_JSON_INIT;
    desc.serialize_variable_labels = true;
    char *json = ecs_iter_to_json(world, &it, &desc);

    test_str(json, 
    "{"
        "\"ids\":[\"(Rel,*)\"], "
        "\"vars\":[\"X\"], "
        "\"results\":[{"
            "\"ids\":[\"(Rel,ObjA)\"], "
            "\"subjects\":[0], "
            "\"vars\":[\"ObjA\"], "
            "\"var_labels\":[\"Object A\"], "
            "\"is_set\":[true], "
            "\"entities\":["
                "\"Foo\""
            "], "
            "\"values\":[0]"
        "}, {"
            "\"ids\":[\"(Rel,ObjB)\"], "
            "\"subjects\":[0], "
            "\"vars\":[\"ObjB\"], "
            "\"var_labels\":[\"ObjB\"], "
            "\"is_set\":[true], "
            "\"entities\":["
                "\"Bar\""
            "], "
            "\"values\":[0]"
        "}]"
    "}");

    ecs_os_free(json);

    ecs_rule_fini(r);

    ecs_fini(world);
}

void SerializeToJson_serialize_iterator_color() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);

    ecs_entity_t e1 = ecs_new_entity(world, "foo_bar");
    ecs_entity_t e2 = ecs_new_entity(world, "hello_world");
    ecs_doc_set_color(world, e2, "#47B576");

    ecs_add(world, e1, Tag);
    ecs_add(world, e2, Tag);

    ecs_query_t *q = ecs_query_new(world, "Tag");
    ecs_iter_t it = ecs_query_iter(world, q);

    ecs_iter_to_json_desc_t desc = ECS_ITER_TO_JSON_INIT;
    desc.serialize_colors = true;
    char *json = ecs_iter_to_json(world, &it, &desc);

    test_str(json, 
    "{"
        "\"ids\":[\"Tag\"], "
        "\"results\":[{"
            "\"ids\":[\"Tag\"], "
            "\"subjects\":[0], "
            "\"is_set\":[true], "
            "\"entities\":["
                "\"foo_bar\""
            "], "
            "\"colors\":["
                "0"
            "], "
            "\"values\":[0]"
        "}, {"
            "\"ids\":[\"Tag\"], "
            "\"subjects\":[0], "
            "\"is_set\":[true], "
            "\"entities\":["
                "\"hello_world\""
            "], "
            "\"colors\":["
                "\"#47B576\""
            "], "
            "\"values\":[0]"
        "}]"
    "}");

    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeToJson_serialize_iterator_w_var_component() {
    typedef struct {
        ecs_i32_t x;
    } T;

    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Rel);
    ECS_TAG(world, Obj);

    ecs_entity_t ecs_id(T) = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.name = "T",
        .members = {
            {"x", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t e = ecs_new_entity(world, "Foo");
    ecs_add_pair(world, e, Rel, Obj);
    ecs_set(world, Obj, T, {10});

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "(Rel, $X), T($X)"
    });

    ecs_iter_t it = ecs_rule_iter(world, r);

    char *json = ecs_iter_to_json(world, &it, NULL);

    test_str(json, 
    "{"
        "\"ids\":[\"(Rel,*)\", \"T\"], "
        "\"vars\":[\"X\"], "
        "\"results\":[{"
            "\"ids\":[\"(Rel,Obj)\", \"T\"], "
            "\"subjects\":[0, \"Obj\"], "
            "\"vars\":[\"Obj\"], "
            "\"is_set\":[true, true], "
            "\"entities\":["
                "\"Foo\""
            "], "
            "\"values\":[0, {\"x\":10}]"
        "}]"
    "}");

    ecs_os_free(json);

    ecs_rule_fini(r);

    ecs_fini(world);
}

void SerializeToJson_serialize_iterator_w_optional_tag() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_entity_t e2 = ecs_new_entity(world, "e2");

    ecs_add(world, e1, TagB);
    ecs_add(world, e1, TagA);
    ecs_add(world, e2, TagA);

    ecs_query_t *q = ecs_query_new(world, "TagA, ?TagB");
    ecs_iter_t it = ecs_query_iter(world, q);

    char *json = ecs_iter_to_json(world, &it, NULL);

    test_str(json, 
    "{"
        "\"ids\":[\"TagA\", \"TagB\"], "
        "\"results\":[{"
            "\"ids\":[\"TagA\", \"TagB\"], "
            "\"subjects\":[0, 0], "
            "\"is_set\":[true, true], "
            "\"entities\":["
                "\"e1\""
            "], "
            "\"values\":[0, 0]"
        "}, {"
            "\"ids\":[\"TagA\", \"TagB\"], "
            "\"subjects\":[0, 0], "
            "\"is_set\":[true, false], "
            "\"entities\":["
                "\"e2\""
            "], "
            "\"values\":[0, 0]"
        "}]"
    "}");

    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeToJson_serialize_iterator_w_optional_component() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_entity_t e2 = ecs_new_entity(world, "e2");

    ecs_add(world, e1, Velocity);
    ecs_add(world, e1, Position);
    ecs_add(world, e2, Position);

    ecs_query_t *q = ecs_query_new(world, "Position, ?Velocity");
    ecs_iter_t it = ecs_query_iter(world, q);

    char *json = ecs_iter_to_json(world, &it, NULL);

    test_str(json, 
    "{"
        "\"ids\":[\"Position\", \"Velocity\"], "
        "\"results\":[{"
            "\"ids\":[\"Position\", \"Velocity\"], "
            "\"subjects\":[0, 0], "
            "\"is_set\":[true, true], "
            "\"entities\":["
                "\"e1\""
            "], "
            "\"values\":[0, 0]"
        "}, {"
            "\"ids\":[\"Position\", \"Velocity\"], "
            "\"subjects\":[0, 0], "
            "\"is_set\":[true, false], "
            "\"entities\":["
                "\"e2\""
            "], "
            "\"values\":[0, 0]"
        "}]"
    "}");

    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeToJson_serialize_iterator_w_optional_reflected_component() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.name = "Position",
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t ecs_id(Velocity) = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.name = "Velocity",
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_entity_t e2 = ecs_new_entity(world, "e2");

    ecs_set(world, e1, Velocity, {1, 2});
    ecs_set(world, e1, Position, {10, 20});
    ecs_set(world, e2, Position, {10, 20});

    ecs_query_t *q = ecs_query_new(world, "Position, ?Velocity");
    ecs_iter_t it = ecs_query_iter(world, q);

    char *json = ecs_iter_to_json(world, &it, NULL);

    test_str(json, 
    "{"
        "\"ids\":[\"Position\", \"Velocity\"], "
        "\"results\":[{"
            "\"ids\":[\"Position\", \"Velocity\"], "
            "\"subjects\":[0, 0], "
            "\"is_set\":[true, true], "
            "\"entities\":["
                "\"e1\""
            "], "
            "\"values\":[[{\"x\":10, \"y\":20}], [{\"x\":1, \"y\":2}]]"
        "}, {"
            "\"ids\":[\"Position\", \"Velocity\"], "
            "\"subjects\":[0, 0], "
            "\"is_set\":[true, false], "
            "\"entities\":["
                "\"e2\""
            "], "
            "\"values\":[[{\"x\":10, \"y\":20}], []]"
        "}]"
    "}");

    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeToJson_serialize_iterator_w_inout_filter_tag() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);

    ecs_entity_t e1 = ecs_new_entity(world, "e1");

    ecs_add(world, e1, TagA);

    ecs_query_t *q = ecs_query_new(world, "[filter] TagA");
    ecs_iter_t it = ecs_query_iter(world, q);

    char *json = ecs_iter_to_json(world, &it, NULL);

    test_str(json, 
    "{"
        "\"ids\":[\"TagA\"], "
        "\"results\":[{"
            "\"ids\":[\"TagA\"], "
            "\"subjects\":[0], "
            "\"is_set\":[true], "
            "\"entities\":["
                "\"e1\""
            "], "
            "\"values\":[0]"
        "}]"
    "}");

    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeToJson_serialize_iterator_w_inout_filter_component() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.entity = ecs_id(Velocity),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t e1 = ecs_new_entity(world, "e1");

    ecs_set(world, e1, Position, {10, 20});
    ecs_set(world, e1, Velocity, {1, 2});

    ecs_query_t *q = ecs_query_new(world, "[filter] Position, Velocity");
    ecs_iter_t it = ecs_query_iter(world, q);

    char *json = ecs_iter_to_json(world, &it, NULL);

    test_str(json, 
    "{"
        "\"ids\":[\"Position\", \"Velocity\"], "
        "\"results\":[{"
            "\"ids\":[\"Position\", \"Velocity\"], "
            "\"subjects\":[0, 0], "
            "\"is_set\":[true, true], "
            "\"entities\":["
                "\"e1\""
            "], "
            "\"values\":[0, [{\"x\":1, \"y\":2}]]"
        "}]"
    "}");

    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeToJson_serialize_iterator_w_inout_filter_reflected_component() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.entity = ecs_id(Position),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.entity = ecs_id(Velocity),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t e1 = ecs_new_entity(world, "e1");

    ecs_set(world, e1, Position, {10, 20});
    ecs_set(world, e1, Velocity, {1, 2});

    ecs_query_t *q = ecs_query_new(world, "[filter] Position, Velocity");
    ecs_iter_t it = ecs_query_iter(world, q);

    char *json = ecs_iter_to_json(world, &it, NULL);

    test_str(json, 
    "{"
        "\"ids\":[\"Position\", \"Velocity\"], "
        "\"results\":[{"
            "\"ids\":[\"Position\", \"Velocity\"], "
            "\"subjects\":[0, 0], "
            "\"is_set\":[true, true], "
            "\"entities\":["
                "\"e1\""
            "], "
            "\"values\":[0, [{\"x\":1, \"y\":2}]]"
        "}]"
    "}");

    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeToJson_serialize_iterator_w_inout_out_tag() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);

    ecs_entity_t e1 = ecs_new_entity(world, "e1");

    ecs_add(world, e1, TagA);

    ecs_query_t *q = ecs_query_new(world, "[out] TagA");
    ecs_iter_t it = ecs_query_iter(world, q);

    char *json = ecs_iter_to_json(world, &it, NULL);

    test_str(json, 
    "{"
        "\"ids\":[\"TagA\"], "
        "\"results\":[{"
            "\"ids\":[\"TagA\"], "
            "\"subjects\":[0], "
            "\"is_set\":[true], "
            "\"entities\":["
                "\"e1\""
            "], "
            "\"values\":[0]"
        "}]"
    "}");

    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeToJson_serialize_iterator_w_inout_out_component() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.entity = ecs_id(Velocity),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t e1 = ecs_new_entity(world, "e1");

    ecs_set(world, e1, Position, {10, 20});
    ecs_set(world, e1, Velocity, {1, 2});

    ecs_query_t *q = ecs_query_new(world, "[out] Position, Velocity");
    ecs_iter_t it = ecs_query_iter(world, q);

    char *json = ecs_iter_to_json(world, &it, NULL);

    test_str(json, 
    "{"
        "\"ids\":[\"Position\", \"Velocity\"], "
        "\"results\":[{"
            "\"ids\":[\"Position\", \"Velocity\"], "
            "\"subjects\":[0, 0], "
            "\"is_set\":[true, true], "
            "\"entities\":["
                "\"e1\""
            "], "
            "\"values\":[0, [{\"x\":1, \"y\":2}]]"
        "}]"
    "}");

    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeToJson_serialize_iterator_w_inout_out_reflected_component() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.entity = ecs_id(Position),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.entity = ecs_id(Velocity),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t e1 = ecs_new_entity(world, "e1");

    ecs_set(world, e1, Position, {10, 20});
    ecs_set(world, e1, Velocity, {1, 2});

    ecs_query_t *q = ecs_query_new(world, "[out] Position, Velocity");
    ecs_iter_t it = ecs_query_iter(world, q);

    char *json = ecs_iter_to_json(world, &it, NULL);

    test_str(json, 
    "{"
        "\"ids\":[\"Position\", \"Velocity\"], "
        "\"results\":[{"
            "\"ids\":[\"Position\", \"Velocity\"], "
            "\"subjects\":[0, 0], "
            "\"is_set\":[true, true], "
            "\"entities\":["
                "\"e1\""
            "], "
            "\"values\":[0, [{\"x\":1, \"y\":2}]]"
        "}]"
    "}");

    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeToJson_serialize_iterator_component_from_var() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.entity = ecs_id(Position),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t e1 = ecs_new_entity(world, "e1");

    ecs_set(world, e1, Position, {10, 20});

    ecs_rule_t *r = ecs_rule_new(world, "Position($E)");
    ecs_iter_t it = ecs_rule_iter(world, r);

    char *json = ecs_iter_to_json(world, &it, NULL);

    test_str(json, 
    "{"
        "\"ids\":[\"Position\"], "
        "\"vars\":[\"E\"], "
        "\"results\":[{"
            "\"ids\":[\"Position\"], "
            "\"subjects\":[\"e1\"], "
            "\"vars\":[\"e1\"], "
            "\"is_set\":[true], "
            "\"values\":[{\"x\":10, \"y\":20}]"
        "}]"
    "}");

    ecs_os_free(json);

    ecs_rule_fini(r);

    ecs_fini(world);
}

void SerializeToJson_serialize_iterator_ids() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);

    ecs_query_t *q = ecs_query_new(world, "Tag");

    ecs_entity_t e = ecs_new_entity(world, "e");
    ecs_add(world, e, Tag);

    ecs_iter_t it = ecs_query_iter(world, q);
    ecs_iter_to_json_desc_t desc = ECS_ITER_TO_JSON_INIT;
    desc.serialize_ids = false;
    desc.serialize_entity_ids = true;
    desc.serialize_values = false;
    desc.serialize_is_set = false;
    desc.serialize_subjects = false;
    char *json = ecs_iter_to_json(world, &it, &desc);
    test_assert(json != NULL);

    char *expect = ecs_asprintf(
        "{\"ids\":[\"Tag\"], \"results\":[{\"entities\":[\"e\"], \"entity_ids\":[%u]}]}",
        (uint32_t)e);
    test_str(json, expect);

    ecs_os_free(json);
    ecs_os_free(expect);

    ecs_fini(world);
}

void SerializeToJson_serialize_iterator_ids_2_entities() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);

    ecs_query_t *q = ecs_query_new(world, "Tag");

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_add(world, e1, Tag);

    ecs_entity_t e2 = ecs_new_entity(world, "e2");
    ecs_add(world, e2, Tag);

    ecs_iter_t it = ecs_query_iter(world, q);
    ecs_iter_to_json_desc_t desc = ECS_ITER_TO_JSON_INIT;
    desc.serialize_ids = false;
    desc.serialize_entity_ids = true;
    desc.serialize_values = false;
    desc.serialize_is_set = false;
    desc.serialize_subjects = false;
    char *json = ecs_iter_to_json(world, &it, &desc);
    test_assert(json != NULL);

    char *expect = ecs_asprintf(
        "{\"ids\":[\"Tag\"], \"results\":[{\"entities\":[\"e1\", \"e2\"], \"entity_ids\":[%u, %u]}]}",
        (uint32_t)e1, (uint32_t)e2);
    test_str(json, expect);

    ecs_os_free(json);
    ecs_os_free(expect);

    ecs_fini(world);
}

void SerializeToJson_serialize_iterator_variable_ids() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);

    ecs_rule_t *q = ecs_rule_new(world, "Tag($Entity)");

    ecs_entity_t e = ecs_new_entity(world, "e");
    ecs_add(world, e, Tag);

    ecs_iter_t it = ecs_rule_iter(world, q);
    ecs_iter_to_json_desc_t desc = ECS_ITER_TO_JSON_INIT;
    desc.serialize_ids = false;
    desc.serialize_variable_ids = true;
    desc.serialize_values = false;
    desc.serialize_is_set = false;
    desc.serialize_subjects = false;
    char *json = ecs_iter_to_json(world, &it, &desc);
    test_assert(json != NULL);

    char *expect = ecs_asprintf(
        "{\"ids\":[\"Tag\"], \"vars\":[\"Entity\"], \"results\":[{\"vars\":[\"e\"], \"var_ids\":[%u]}]}",
        (uint32_t)e);
    test_str(json, expect);

    ecs_os_free(json);
    ecs_os_free(expect);

    ecs_rule_fini(q);

    ecs_fini(world);
}

void SerializeToJson_serialize_iterator_variable_ids_2_entities() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);

    ecs_rule_t *q = ecs_rule_new(world, "Tag($Entity)");

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_add(world, e1, Tag);

    ecs_entity_t e2 = ecs_new_entity(world, "e2");
    ecs_add(world, e2, Tag);

    ecs_iter_t it = ecs_rule_iter(world, q);
    ecs_iter_to_json_desc_t desc = ECS_ITER_TO_JSON_INIT;
    desc.serialize_ids = false;
    desc.serialize_variable_ids = true;
    desc.serialize_values = false;
    desc.serialize_is_set = false;
    desc.serialize_subjects = false;
    char *json = ecs_iter_to_json(world, &it, &desc);
    test_assert(json != NULL);

    char *expect = ecs_asprintf(
        "{\"ids\":[\"Tag\"], \"vars\":[\"Entity\"], \"results\":[{\"vars\":[\"e1\"], \"var_ids\":[%u]}, {\"vars\":[\"e2\"], \"var_ids\":[%u]}]}",
        (uint32_t)e1, (uint32_t)e2);
    test_str(json, expect);

    ecs_os_free(json);
    ecs_os_free(expect);

    ecs_rule_fini(q);

    ecs_fini(world);
}

void SerializeToJson_serialize_paged_iterator() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_set(world, 0, Position, {10, 20});
    ecs_entity_t e2 = ecs_set(world, 0, Position, {10, 20});
    ecs_entity_t e3 = ecs_set(world, 0, Position, {20, 30});
    ecs_entity_t e4 = ecs_set(world, 0, Position, {30, 40});
    ecs_set(world, 0, Position, {10, 20});

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .terms = {{ .id = ecs_id(Position) }}
    });

    ecs_iter_t it = ecs_rule_iter(world, r);
    ecs_iter_t pit = ecs_page_iter(&it, 1, 3);

    test_bool(true, ecs_page_next(&pit));
    test_int(3, pit.count);
    test_uint(e2, pit.entities[0]);
    test_uint(e3, pit.entities[1]);
    test_uint(e4, pit.entities[2]);

    Position *p = ecs_term(&pit, Position, 1);
    test_int(p[0].x, 10);
    test_int(p[0].y, 20);
    test_int(p[1].x, 20);
    test_int(p[1].y, 30);
    test_int(p[2].x, 30);
    test_int(p[2].y, 40);

    test_bool(false, ecs_page_next(&pit));

    ecs_rule_fini(r);

    ecs_fini(world);
}

void SerializeToJson_serialize_paged_iterator_w_optional_component() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_set(world, 0, Position, {10, 20});
    ecs_entity_t e2 = ecs_set(world, 0, Position, {10, 20});
    ecs_entity_t e3 = ecs_set(world, 0, Position, {20, 30});
    ecs_entity_t e4 = ecs_set(world, 0, Position, {30, 40});
    ecs_entity_t e5 = ecs_set(world, 0, Position, {40, 50});

    ecs_set(world, e4, Velocity, {1, 2});
    ecs_set(world, e5, Velocity, {2, 3});

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .terms = {
            { .id = ecs_id(Position) },
            { .id = ecs_id(Velocity), .oper = EcsOptional }
        }
    });

    ecs_iter_t it = ecs_rule_iter(world, r);
    ecs_iter_t pit = ecs_page_iter(&it, 1, 3);

    test_bool(true, ecs_page_next(&pit));
    test_int(2, pit.count);
    test_bool(true, ecs_term_is_set(&pit, 1));
    test_bool(false, ecs_term_is_set(&pit, 2));
    test_uint(e2, pit.entities[0]);
    test_uint(e3, pit.entities[1]);

    Position *p = ecs_term(&pit, Position, 1);
    test_int(p[0].x, 10);
    test_int(p[0].y, 20);
    test_int(p[1].x, 20);
    test_int(p[1].y, 30);

    test_bool(true, ecs_page_next(&pit));
    test_int(1, pit.count);
    test_bool(true, ecs_term_is_set(&pit, 1));
    test_bool(true, ecs_term_is_set(&pit, 2));

    test_uint(e4, pit.entities[0]);
    p = ecs_term(&pit, Position, 1);
    test_int(p[0].x, 30);
    test_int(p[0].y, 40);

    Velocity *v = ecs_term(&pit, Velocity, 2);
    test_int(v[0].x, 1);
    test_int(v[0].y, 2);

    test_bool(false, ecs_page_next(&pit));

    ecs_rule_fini(r);

    ecs_fini(world);
}

void SerializeToJson_serialize_paged_iterator_w_optional_tag() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Tag);

    ecs_set(world, 0, Position, {10, 20});
    ecs_entity_t e2 = ecs_set(world, 0, Position, {10, 20});
    ecs_entity_t e3 = ecs_set(world, 0, Position, {20, 30});
    ecs_entity_t e4 = ecs_set(world, 0, Position, {30, 40});
    ecs_entity_t e5 = ecs_set(world, 0, Position, {40, 50});

    ecs_add(world, e4, Tag);
    ecs_add(world, e5, Tag);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .terms = {
            { .id = ecs_id(Position) },
            { .id = Tag, .oper = EcsOptional }
        }
    });

    ecs_iter_t it = ecs_rule_iter(world, r);
    ecs_iter_t pit = ecs_page_iter(&it, 1, 3);

    test_bool(true, ecs_page_next(&pit));
    test_int(2, pit.count);
    test_bool(true, ecs_term_is_set(&pit, 1));
    test_bool(false, ecs_term_is_set(&pit, 2));
    test_uint(e2, pit.entities[0]);
    test_uint(e3, pit.entities[1]);

    Position *p = ecs_term(&pit, Position, 1);
    test_int(p[0].x, 10);
    test_int(p[0].y, 20);
    test_int(p[1].x, 20);
    test_int(p[1].y, 30);

    test_bool(true, ecs_page_next(&pit));
    test_int(1, pit.count);
    test_bool(true, ecs_term_is_set(&pit, 1));
    test_bool(true, ecs_term_is_set(&pit, 2));

    test_uint(e4, pit.entities[0]);
    p = ecs_term(&pit, Position, 1);
    test_int(p[0].x, 30);
    test_int(p[0].y, 40);

    test_bool(false, ecs_page_next(&pit));

    ecs_rule_fini(r);

    ecs_fini(world);
}

void SerializeToJson_serialize_paged_iterator_w_vars() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Rel);
    ECS_TAG(world, ObjA);
    ECS_TAG(world, ObjB);

    ecs_new_w_pair(world, Rel, ObjA);
    ecs_entity_t e2 = ecs_new_w_pair(world, Rel, ObjA);
    ecs_entity_t e3 = ecs_new_w_pair(world, Rel, ObjA);
    ecs_entity_t e4 = ecs_new_w_pair(world, Rel, ObjB);
    ecs_new_w_pair(world, Rel, ObjB);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "(Rel, $Var)"
    });

    int32_t var = ecs_rule_find_var(r, "Var");

    ecs_iter_t it = ecs_rule_iter(world, r);
    ecs_iter_t pit = ecs_page_iter(&it, 1, 3);

    test_assert(var < pit.variable_count);
    test_str("Var", pit.variable_names[var]);

    test_bool(true, ecs_page_next(&pit));
    test_int(2, pit.count);
    test_uint(e2, pit.entities[0]);
    test_uint(e3, pit.entities[1]);
    test_uint(ObjA, ecs_iter_get_var(&pit, var));

    test_bool(true, ecs_page_next(&pit));
    test_int(1, pit.count);
    test_uint(e4, pit.entities[0]);
    test_uint(ObjB, ecs_iter_get_var(&pit, var));

    test_bool(false, ecs_page_next(&pit));

    ecs_rule_fini(r);

    ecs_fini(world);
}
