#include <meta.h>
#include <stdio.h>
#include <limits.h>
#include <math.h>

void SerializeToJson_struct_bool(void) {
    typedef struct {
        ecs_bool_t x;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "T"}),
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

void SerializeToJson_struct_byte(void) {
    typedef struct {
        ecs_byte_t x;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "T"}),
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

void SerializeToJson_struct_char(void) {
    typedef struct {
        ecs_char_t x;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "T"}),
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

void SerializeToJson_struct_i8(void) {
    typedef struct {
        ecs_i8_t x;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "T"}),
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

void SerializeToJson_struct_i16(void) {
    typedef struct {
        ecs_i16_t x;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "T"}),
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

void SerializeToJson_struct_i32(void) {
    typedef struct {
        ecs_i32_t x;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "T"}),
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

void SerializeToJson_struct_i64(void) {
    typedef struct {
        ecs_i64_t x;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "T"}),
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

void SerializeToJson_struct_iptr(void) {
    typedef struct {
        ecs_iptr_t x;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "T"}),
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

void SerializeToJson_struct_u8(void) {
    typedef struct {
        ecs_u8_t x;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "T"}),
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

void SerializeToJson_struct_u16(void) {
    typedef struct {
        ecs_u16_t x;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "T"}),
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

void SerializeToJson_struct_u32(void) {
    typedef struct {
        ecs_u32_t x;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "T"}),
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

void SerializeToJson_struct_u64(void) {
    typedef struct {
        ecs_u64_t x;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "T"}),
        .members = {
            {"x", ecs_id(ecs_u64_t)}
        }
    });

    {
        T value = {0};
        char *expr = ecs_ptr_to_json(world, t, &value);
        test_assert(expr != NULL);
        test_str(expr, "{\"x\":0}");
        ecs_os_free(expr);
    }

    {
        T value = {10};
        char *expr = ecs_ptr_to_json(world, t, &value);
        test_assert(expr != NULL);
        test_str(expr, "{\"x\":10}");
        ecs_os_free(expr);
    }

    {
        T value = {2366700781656087864};
        char *expr = ecs_ptr_to_json(world, t, &value);
        test_assert(expr != NULL);
        test_str(expr, "{\"x\":\"2366700781656087864\"}");
        ecs_os_free(expr);
    }

    ecs_fini(world);
}

void SerializeToJson_struct_uptr(void) {
    typedef struct {
        ecs_uptr_t x;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "T"}),
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

void SerializeToJson_struct_float(void) {
    typedef struct {
        ecs_f32_t x;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "T"}),
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

void SerializeToJson_struct_float_nan(void) {
    typedef struct {
        ecs_f32_t x;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "T"}),
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

void SerializeToJson_struct_float_inf(void) {
    typedef struct {
        ecs_f32_t x;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "T"}),
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

void SerializeToJson_struct_double(void) {
    typedef struct {
        ecs_f64_t x;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "T"}),
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

void SerializeToJson_struct_double_nan(void) {
    typedef struct {
        ecs_f64_t x;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "T"}),
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

void SerializeToJson_struct_double_inf(void) {
    typedef struct {
        ecs_f64_t x;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "T"}),
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

void SerializeToJson_struct_double_large(void) {
    typedef struct {
        ecs_f64_t x;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "T"}),
        .members = {
            {"x", ecs_id(ecs_f64_t)}
        }
    });

    {
        T value = {60000};
        char *expr = ecs_ptr_to_json(world, t, &value);
        test_assert(expr != NULL);
        test_str(expr, "{\"x\":\"6e4\"}");
        ecs_os_free(expr);
    }
    {
        T value = {1.9885e30};
        char *expr = ecs_ptr_to_json(world, t, &value);
        test_assert(expr != NULL);
        test_str(expr, "{\"x\":\"1.98849999999e30\"}");
        ecs_os_free(expr);
    }

    ecs_fini(world);
}

void SerializeToJson_struct_double_large_e(void) {
    typedef struct {
        ecs_f64_t x;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "T"}),
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

void SerializeToJson_struct_string(void) {
    typedef struct {
        char* x;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "T"}),
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

void SerializeToJson_struct_entity(void) {
    typedef struct {
        ecs_entity_t x;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "T"}),
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

void SerializeToJson_struct_entity_0(void) {
    typedef struct {
        ecs_entity_t x;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "T"}),
        .members = {
            {"x", ecs_id(ecs_entity_t)}
        }
    });

    T value = {0};
    char *expr = ecs_ptr_to_json(world, t, &value);
    test_assert(expr != NULL);
    test_str(expr, "{\"x\":\"#0\"}");
    ecs_os_free(expr);

    ecs_fini(world);
}

void SerializeToJson_struct_entity_10k(void) {
    typedef struct {
        ecs_entity_t x;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "T"}),
        .members = {
            {"x", ecs_id(ecs_entity_t)}
        }
    });

    ecs_make_alive(world, 10000);

    T value = {10000};
    char *expr = ecs_ptr_to_json(world, t, &value);
    test_assert(expr != NULL);
    test_str(expr, "{\"x\":\"#10000\"}");
    ecs_os_free(expr);

    ecs_fini(world);
}

void SerializeToJson_struct_id(void) {
    typedef struct {
        ecs_id_t x;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "T"}),
        .members = {
            {"x", ecs_id(ecs_id_t)}
        }
    });

    T value = {EcsFlecsCore};
    char *expr = ecs_ptr_to_json(world, t, &value);
    test_assert(expr != NULL);
    test_str(expr, "{\"x\":[\"flecs.core\"]}");
    ecs_os_free(expr);

    ecs_fini(world);
}

void SerializeToJson_struct_entity_after_float(void) {
    typedef struct {
        int32_t v;
        ecs_entity_t e;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "T"}),
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

void SerializeToJson_struct_enum(void) {
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
        .entity = ecs_entity(world, {.name = "T"}),
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
    ecs_log_set_level(-4);
    char *expr = ecs_ptr_to_json(world, t, &value);
    test_assert(expr == NULL);
    }

    ecs_fini(world);
}

void SerializeToJson_struct_bitmask(void) {
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
        .entity = ecs_entity(world, {.name = "T"}),
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
    ecs_log_set_level(-4);
    char *expr = ecs_ptr_to_json(world, t, &value);
    test_assert(expr == NULL);
    }

    ecs_fini(world);
}

void SerializeToJson_struct_i32_i32(void) {
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

    T value = {10, 20};
    char *expr = ecs_ptr_to_json(world, t, &value);
    test_assert(expr != NULL);
    test_str(expr, "{\"x\":10, \"y\":20}");
    ecs_os_free(expr);

    ecs_fini(world);
}

void SerializeToJson_struct_nested_i32(void) {
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

    T value = {{10}};
    char *expr = ecs_ptr_to_json(world, t, &value);
    test_assert(expr != NULL);
    test_str(expr, "{\"n_1\":{\"x\":10}}");
    ecs_os_free(expr);

    ecs_fini(world);
}

void SerializeToJson_struct_nested_i32_i32(void) {
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

    T value = {{10, 20}};
    char *expr = ecs_ptr_to_json(world, t, &value);
    test_assert(expr != NULL);
    test_str(expr, "{\"n_1\":{\"x\":10, \"y\":20}}");
    ecs_os_free(expr);

    ecs_fini(world);
}

void SerializeToJson_struct_2_nested_i32_i32(void) {
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

    T value = {{10, 20}, {30, 40}};
    char *expr = ecs_ptr_to_json(world, t, &value);
    test_assert(expr != NULL);
    test_str(expr, "{\"n_1\":{\"x\":10, \"y\":20}, \"n_2\":{\"x\":30, \"y\":40}}");
    ecs_os_free(expr);

    ecs_fini(world);
}

void SerializeToJson_struct_i32_array_3(void) {
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

    T value = {{10, 20, 30}};
    char *expr = ecs_ptr_to_json(world, t, &value);
    test_assert(expr != NULL);
    test_str(expr, "{\"x\":[10, 20, 30]}");
    ecs_os_free(expr);

    ecs_fini(world);
}

void SerializeToJson_struct_struct_i32_array_3(void) {
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

    T value = {{ {10}, {20}, {30} }};
    char *expr = ecs_ptr_to_json(world, t, &value);
    test_assert(expr != NULL);
    test_str(expr, "{\"n_1\":[{\"x\":10}, {\"x\":20}, {\"x\":30}]}");
    ecs_os_free(expr);

    ecs_fini(world);
}

void SerializeToJson_struct_struct_i32_i32_array_3(void) {
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

    T value = {{ {10, 20}, {30, 40}, {50, 60} }};
    char *expr = ecs_ptr_to_json(world, t, &value);
    test_assert(expr != NULL);
    test_str(expr, "{\"n_1\":[{\"x\":10, \"y\":20}, {\"x\":30, \"y\":40}, {\"x\":50, \"y\":60}]}");
    ecs_os_free(expr);

    ecs_fini(world);
}

void SerializeToJson_struct_w_array_type_i32_i32(void) {
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

    T value = { .n_1 = { 10, 20 }};
    char *expr = ecs_ptr_to_json(world, t, &value);
    test_assert(expr != NULL);
    test_str(expr, "{\"n_1\":[10, 20]}");
    ecs_os_free(expr);

    ecs_fini(world);
}

void SerializeToJson_struct_w_2_array_type_i32_i32(void) {
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

    T value = { .n_1 = { 10, 20 }, .n_2 = {30, 40} };
    char *expr = ecs_ptr_to_json(world, t, &value);
    test_assert(expr != NULL);
    test_str(expr, "{\"n_1\":[10, 20], \"n_2\":[30, 40]}");
    ecs_os_free(expr);

    ecs_fini(world);
}

void SerializeToJson_struct_partial(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_id(Position),
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

void SerializeToJson_array_i32_3(void) {
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

void SerializeToJson_array_struct_i32_i32(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_id(Position),
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

void SerializeToJson_array_array_i32_3(void) {
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

void SerializeToJson_vector_i32_3(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_vector_init(world, &(ecs_vector_desc_t){
        .type = ecs_id(ecs_i32_t)
    });

    ecs_vec_t v;
    ecs_vec_init_t(NULL, &v, ecs_i32_t, 3);
    ecs_vec_append_t(NULL, &v, ecs_i32_t)[0] = 10;
    ecs_vec_append_t(NULL, &v, ecs_i32_t)[0] = 20;
    ecs_vec_append_t(NULL, &v, ecs_i32_t)[0] = 30;

    char *expr = ecs_ptr_to_json(world, t, &v);
    test_assert(expr != NULL);
    test_str(expr, "[10, 20, 30]");
    ecs_os_free(expr);

    ecs_vec_fini_t(NULL, &v, ecs_i32_t);

    ecs_fini(world);
}

void SerializeToJson_vector_struct_i32_i32(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_id(Position),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t t = ecs_vector_init(world, &(ecs_vector_desc_t){
        .type = ecs_id(Position)
    });

    ecs_vec_t v;
    ecs_vec_init_t(NULL, &v, Position, 3);
    ecs_vec_append_t(NULL, &v, Position)[0] = (Position){10, 20};
    ecs_vec_append_t(NULL, &v, Position)[0] = (Position){30, 40};
    ecs_vec_append_t(NULL, &v, Position)[0] = (Position){50, 60};

    char *expr = ecs_ptr_to_json(world, t, &v);
    test_assert(expr != NULL);
    test_str(expr, "[{\"x\":10, \"y\":20}, {\"x\":30, \"y\":40}, {\"x\":50, \"y\":60}]");
    ecs_os_free(expr);

    ecs_vec_fini_t(NULL, &v, Position);

    ecs_fini(world);
}

void SerializeToJson_vector_array_i32_3(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t a = ecs_array_init(world, &(ecs_array_desc_t){
        .entity = ecs_entity(world, { .name = "a" }),
        .type = ecs_id(ecs_i32_t),
        .count = 2
    });

    ecs_entity_t t = ecs_vector_init(world, &(ecs_vector_desc_t){
        .type = a
    });

    typedef int32_t IntArray[2];

    ecs_vec_t v;
    ecs_vec_init_t(NULL, &v, IntArray, 3);
    {
        IntArray *av = ecs_vec_append_t(NULL, &v, IntArray);
        av[0][0] = 10;
        av[0][1] = 20;
    }
    {
        IntArray *av = ecs_vec_append_t(NULL, &v, IntArray);
        av[0][0] = 30;
        av[0][1] = 40;
    }
    {
        IntArray *av = ecs_vec_append_t(NULL, &v, IntArray);
        av[0][0] = 50;
        av[0][1] = 60;
    }

    char *expr = ecs_ptr_to_json(world, t, &v);
    test_assert(expr != NULL);
    test_str(expr, "[[10, 20], [30, 40], [50, 60]]");
    ecs_os_free(expr);

    ecs_vec_fini_t(NULL, &v, IntArray);

    ecs_fini(world);
}

void SerializeToJson_serialize_from_stage(void) {
    typedef struct {
        ecs_char_t x;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "T"}),
        .members = {
            {"x", ecs_id(ecs_char_t)}
        }
    });

    T value = {'a'};

    ecs_world_t *stage = ecs_get_stage(world, 0);

    char *expr = ecs_ptr_to_json(stage, t, &value);
    test_assert(expr != NULL);
    test_str(expr, "{\"x\":\"a\"}");
    ecs_os_free(expr);

    ecs_fini(world);
}
