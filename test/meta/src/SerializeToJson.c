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


void SerializeToJson_enum_underlying_i8(void) {
    enum E { Red, Blue, Green };

    ecs_world_t *world = ecs_init();

    ecs_entity_t e = ecs_enum(world, {
        .constants = {
            {"Red"}, {"Blue"}, {"Green"}
        },
        .underlying_type = ecs_id(ecs_i8_t)
    });

    test_assert(e != 0);

    {
    int8_t value = Red;
    char *expr = ecs_ptr_to_json(world, e, &value);
    test_assert(expr != NULL);
    test_str(expr, "\"Red\"");
    ecs_os_free(expr);
    }

    {
    int8_t value = Blue;
    char *expr = ecs_ptr_to_json(world, e, &value);
    test_assert(expr != NULL);
    test_str(expr, "\"Blue\"");
    ecs_os_free(expr);
    }

    {
    int8_t value = Green;
    char *expr = ecs_ptr_to_json(world, e, &value);
    test_assert(expr != NULL);
    test_str(expr, "\"Green\"");
    ecs_os_free(expr);
    }

    {
    int8_t value = 10;
    ecs_log_set_level(-4);
    char *expr = ecs_ptr_to_json(world, e, &value);
    test_assert(expr == NULL);
    }

    ecs_fini(world);
}

void SerializeToJson_enum_underlying_i16(void) {
    enum E { Red, Blue, Green };

    ecs_world_t *world = ecs_init();

    ecs_entity_t e = ecs_enum(world, {
        .constants = {
            {"Red"}, {"Blue"}, {"Green"}
        },
        .underlying_type = ecs_id(ecs_i16_t)
    });

    test_assert(e != 0);

    {
    int16_t value = Red;
    char *expr = ecs_ptr_to_json(world, e, &value);
    test_assert(expr != NULL);
    test_str(expr, "\"Red\"");
    ecs_os_free(expr);
    }

    {
    int16_t value = Blue;
    char *expr = ecs_ptr_to_json(world, e, &value);
    test_assert(expr != NULL);
    test_str(expr, "\"Blue\"");
    ecs_os_free(expr);
    }

    {
    int16_t value = Green;
    char *expr = ecs_ptr_to_json(world, e, &value);
    test_assert(expr != NULL);
    test_str(expr, "\"Green\"");
    ecs_os_free(expr);
    }

    {
    int16_t value = 10;
    ecs_log_set_level(-4);
    char *expr = ecs_ptr_to_json(world, e, &value);
    test_assert(expr == NULL);
    }

    ecs_fini(world);
}

void SerializeToJson_enum_underlying_i32(void) {
    enum E { Red, Blue, Green };

    ecs_world_t *world = ecs_init();

    ecs_entity_t e = ecs_enum(world, {
        .constants = {
            {"Red"}, {"Blue"}, {"Green"}
        },
        .underlying_type = ecs_id(ecs_i32_t)
    });

    test_assert(e != 0);

    {
    int32_t value = Red;
    char *expr = ecs_ptr_to_json(world, e, &value);
    test_assert(expr != NULL);
    test_str(expr, "\"Red\"");
    ecs_os_free(expr);
    }

    {
    int32_t value = Blue;
    char *expr = ecs_ptr_to_json(world, e, &value);
    test_assert(expr != NULL);
    test_str(expr, "\"Blue\"");
    ecs_os_free(expr);
    }

    {
    int32_t value = Green;
    char *expr = ecs_ptr_to_json(world, e, &value);
    test_assert(expr != NULL);
    test_str(expr, "\"Green\"");
    ecs_os_free(expr);
    }

    {
    int32_t value = 10;
    ecs_log_set_level(-4);
    char *expr = ecs_ptr_to_json(world, e, &value);
    test_assert(expr == NULL);
    }

    ecs_fini(world);
}

void SerializeToJson_enum_underlying_i64(void) {
    enum E { Red, Blue, Green };

    int64_t Large = 1ll << 40;

    ecs_world_t *world = ecs_init();

    ecs_entity_t e = ecs_enum(world, {
        .constants = {
            {"Red"}, {"Blue"}, {"Green"}, {"Large", Large}
        },
        .underlying_type = ecs_id(ecs_i64_t)
    });

    test_assert(e != 0);

    {
    int64_t value = Red;
    char *expr = ecs_ptr_to_json(world, e, &value);
    test_assert(expr != NULL);
    test_str(expr, "\"Red\"");
    ecs_os_free(expr);
    }

    {
    int64_t value = Blue;
    char *expr = ecs_ptr_to_json(world, e, &value);
    test_assert(expr != NULL);
    test_str(expr, "\"Blue\"");
    ecs_os_free(expr);
    }

    {
    int64_t value = Green;
    char *expr = ecs_ptr_to_json(world, e, &value);
    test_assert(expr != NULL);
    test_str(expr, "\"Green\"");
    ecs_os_free(expr);
    }

    {
    int64_t value = Large;
    char *expr = ecs_ptr_to_json(world, e, &value);
    test_assert(expr != NULL);
    test_str(expr, "\"Large\"");
    ecs_os_free(expr);
    }

    {
    int64_t value = 10;
    ecs_log_set_level(-4);
    char *expr = ecs_ptr_to_json(world, e, &value);
    test_assert(expr == NULL);
    }

    ecs_fini(world);
}

void SerializeToJson_enum_underlying_iptr(void) {
    enum E { Red, Blue, Green };

    ecs_world_t *world = ecs_init();

    ecs_entity_t e = ecs_enum(world, {
        .constants = {
            {"Red"}, {"Blue"}, {"Green"}
        },
        .underlying_type = ecs_id(ecs_iptr_t)
    });

    test_assert(e != 0);

    {
    intptr_t value = Red;
    char *expr = ecs_ptr_to_json(world, e, &value);
    test_assert(expr != NULL);
    test_str(expr, "\"Red\"");
    ecs_os_free(expr);
    }

    {
    intptr_t value = Blue;
    char *expr = ecs_ptr_to_json(world, e, &value);
    test_assert(expr != NULL);
    test_str(expr, "\"Blue\"");
    ecs_os_free(expr);
    }

    {
    intptr_t value = Green;
    char *expr = ecs_ptr_to_json(world, e, &value);
    test_assert(expr != NULL);
    test_str(expr, "\"Green\"");
    ecs_os_free(expr);
    }

    {
    intptr_t value = 10;
    ecs_log_set_level(-4);
    char *expr = ecs_ptr_to_json(world, e, &value);
    test_assert(expr == NULL);
    }

    ecs_fini(world);
}

void SerializeToJson_enum_underlying_u8(void) {
    enum E { Red, Blue, Green };

    ecs_world_t *world = ecs_init();

    ecs_entity_t e = ecs_enum(world, {
        .constants = {
            {"Red"}, {"Blue"}, {"Green"}
        },
        .underlying_type = ecs_id(ecs_u8_t)
    });

    test_assert(e != 0);

    {
    uint8_t value = Red;
    char *expr = ecs_ptr_to_json(world, e, &value);
    test_assert(expr != NULL);
    test_str(expr, "\"Red\"");
    ecs_os_free(expr);
    }

    {
    uint8_t value = Blue;
    char *expr = ecs_ptr_to_json(world, e, &value);
    test_assert(expr != NULL);
    test_str(expr, "\"Blue\"");
    ecs_os_free(expr);
    }

    {
    uint8_t value = Green;
    char *expr = ecs_ptr_to_json(world, e, &value);
    test_assert(expr != NULL);
    test_str(expr, "\"Green\"");
    ecs_os_free(expr);
    }

    {
    uint8_t value = 10;
    ecs_log_set_level(-4);
    char *expr = ecs_ptr_to_json(world, e, &value);
    test_assert(expr == NULL);
    }

    ecs_fini(world);
}

void SerializeToJson_enum_underlying_u16(void) {
    enum E { Red, Blue, Green };

    ecs_world_t *world = ecs_init();

    ecs_entity_t e = ecs_enum(world, {
        .constants = {
            {"Red"}, {"Blue"}, {"Green"}
        },
        .underlying_type = ecs_id(ecs_u16_t)
    });

    test_assert(e != 0);

    {
    uint16_t value = Red;
    char *expr = ecs_ptr_to_json(world, e, &value);
    test_assert(expr != NULL);
    test_str(expr, "\"Red\"");
    ecs_os_free(expr);
    }

    {
    uint16_t value = Blue;
    char *expr = ecs_ptr_to_json(world, e, &value);
    test_assert(expr != NULL);
    test_str(expr, "\"Blue\"");
    ecs_os_free(expr);
    }

    {
    uint16_t value = Green;
    char *expr = ecs_ptr_to_json(world, e, &value);
    test_assert(expr != NULL);
    test_str(expr, "\"Green\"");
    ecs_os_free(expr);
    }

    {
    uint16_t value = 10;
    ecs_log_set_level(-4);
    char *expr = ecs_ptr_to_json(world, e, &value);
    test_assert(expr == NULL);
    }

    ecs_fini(world);
}

void SerializeToJson_enum_underlying_u32(void) {
    enum E { Red, Blue, Green };

    ecs_world_t *world = ecs_init();

    ecs_entity_t e = ecs_enum(world, {
        .constants = {
            {"Red"}, {"Blue"}, {"Green"}
        },
        .underlying_type = ecs_id(ecs_u32_t)
    });

    test_assert(e != 0);

    {
    uint32_t value = Red;
    char *expr = ecs_ptr_to_json(world, e, &value);
    test_assert(expr != NULL);
    test_str(expr, "\"Red\"");
    ecs_os_free(expr);
    }

    {
    uint32_t value = Blue;
    char *expr = ecs_ptr_to_json(world, e, &value);
    test_assert(expr != NULL);
    test_str(expr, "\"Blue\"");
    ecs_os_free(expr);
    }

    {
    uint32_t value = Green;
    char *expr = ecs_ptr_to_json(world, e, &value);
    test_assert(expr != NULL);
    test_str(expr, "\"Green\"");
    ecs_os_free(expr);
    }

    {
    uint32_t value = 10;
    ecs_log_set_level(-4);
    char *expr = ecs_ptr_to_json(world, e, &value);
    test_assert(expr == NULL);
    }

    ecs_fini(world);
}

void SerializeToJson_enum_underlying_u64(void) {
    enum E { Red, Blue, Green };

    ecs_world_t *world = ecs_init();

    ecs_entity_t e = ecs_enum(world, {
        .constants = {
            {"Red"}, {"Blue"}, {"Green"}
        },
        .underlying_type = ecs_id(ecs_u64_t)
    });

    test_assert(e != 0);

    {
    uint64_t value = Red;
    char *expr = ecs_ptr_to_json(world, e, &value);
    test_assert(expr != NULL);
    test_str(expr, "\"Red\"");
    ecs_os_free(expr);
    }

    {
    uint64_t value = Blue;
    char *expr = ecs_ptr_to_json(world, e, &value);
    test_assert(expr != NULL);
    test_str(expr, "\"Blue\"");
    ecs_os_free(expr);
    }

    {
    uint64_t value = Green;
    char *expr = ecs_ptr_to_json(world, e, &value);
    test_assert(expr != NULL);
    test_str(expr, "\"Green\"");
    ecs_os_free(expr);
    }

    {
    uint64_t value = 10;
    ecs_log_set_level(-4);
    char *expr = ecs_ptr_to_json(world, e, &value);
    test_assert(expr == NULL);
    }

    ecs_fini(world);
}

void SerializeToJson_enum_underlying_uptr(void) {
    enum E { Red, Blue, Green };

    ecs_world_t *world = ecs_init();

    ecs_entity_t e = ecs_enum(world, {
        .constants = {
            {"Red"}, {"Blue"}, {"Green"}
        },
        .underlying_type = ecs_id(ecs_uptr_t)
    });

    test_assert(e != 0);

    {
    uintptr_t value = Red;
    char *expr = ecs_ptr_to_json(world, e, &value);
    test_assert(expr != NULL);
    test_str(expr, "\"Red\"");
    ecs_os_free(expr);
    }

    {
    uintptr_t value = Blue;
    char *expr = ecs_ptr_to_json(world, e, &value);
    test_assert(expr != NULL);
    test_str(expr, "\"Blue\"");
    ecs_os_free(expr);
    }

    {
    uintptr_t value = Green;
    char *expr = ecs_ptr_to_json(world, e, &value);
    test_assert(expr != NULL);
    test_str(expr, "\"Green\"");
    ecs_os_free(expr);
    }

    {
    uintptr_t value = 10;
    ecs_log_set_level(-4);
    char *expr = ecs_ptr_to_json(world, e, &value);
    test_assert(expr == NULL);
    }

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

void SerializeToJson_struct_uptr_large(void) {
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

#if UINTPTR_MAX > 0xFFFFFFFFu
    T value = { (uintptr_t)0x1234567890ull };
    char *expr = ecs_ptr_to_json(world, t, &value);
    test_assert(expr != NULL);
    test_str(expr, "{\"x\":78187493520}");
    ecs_os_free(expr);
#else
    T value = { (uintptr_t)0x12345678u };
    char *expr = ecs_ptr_to_json(world, t, &value);
    test_assert(expr != NULL);
    test_str(expr, "{\"x\":305419896}");
    ecs_os_free(expr);
#endif

    ecs_fini(world);
}

void SerializeToJson_enum_negative_constant(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t e = ecs_enum_init(world, &(ecs_enum_desc_t){
        .entity = ecs_entity(world, {.name = "Color"}),
        .constants = {
            {"Red", -1}
        }
    });

    test_assert(e != 0);

    int32_t value = -1;
    char *expr = ecs_ptr_to_json(world, e, &value);
    test_assert(expr != NULL);
    test_str(expr, "\"Red\"");
    ecs_os_free(expr);

    ecs_fini(world);
}

void SerializeToJson_struct_string_w_control_char(void) {
    typedef struct {
        char *s;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "T"}),
        .members = {
            {"s", ecs_id(ecs_string_t)}
        }
    });

    T value = { "a\033b" };
    char *expr = ecs_ptr_to_json(world, t, &value);
    test_assert(expr != NULL);
    test_str(expr, "{\"s\":\"a\\u001bb\"}");
    ecs_os_free(expr);

    ecs_fini(world);
}

void SerializeToJson_map_i64_i32_1(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_map_type_init(world, &(ecs_map_desc_t){
        .key_type = ecs_id(ecs_i64_t),
        .type = ecs_id(ecs_i32_t)
    });

    ecs_map_t m;
    ecs_map_init(&m, NULL);
    *(int32_t*)ecs_map_ensure(&m, 10) = 100;

    char *expr = ecs_ptr_to_json(world, t, &m);
    test_assert(expr != NULL);
    test_str(expr, "{\"10\":100}");
    ecs_os_free(expr);

    ecs_map_fini(&m);

    ecs_fini(world);
}

void SerializeToJson_map_i64_i32_3(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_map_type_init(world, &(ecs_map_desc_t){
        .key_type = ecs_id(ecs_i64_t),
        .type = ecs_id(ecs_i32_t)
    });

    ecs_map_t m;
    ecs_map_init(&m, NULL);
    *(int32_t*)ecs_map_ensure(&m, 10) = 100;
    *(int32_t*)ecs_map_ensure(&m, 20) = 200;
    *(int32_t*)ecs_map_ensure(&m, 30) = 300;

    char *expr = ecs_ptr_to_json(world, t, &m);
    test_assert(expr != NULL);
    test_str(expr, "{\"10\":100, \"20\":200, \"30\":300}");
    ecs_os_free(expr);

    ecs_map_fini(&m);

    ecs_fini(world);
}

void SerializeToJson_map_i64_string(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_map_type_init(world, &(ecs_map_desc_t){
        .key_type = ecs_id(ecs_i64_t),
        .type = ecs_id(ecs_string_t)
    });

    ecs_map_t m;
    ecs_map_init(&m, NULL);
    *(char**)ecs_map_ensure(&m, 10) = ecs_os_strdup("Hello");

    char *expr = ecs_ptr_to_json(world, t, &m);
    test_assert(expr != NULL);
    test_str(expr, "{\"10\":\"Hello\"}");
    ecs_os_free(expr);

    ecs_map_iter_t it = ecs_map_iter(&m);
    while (ecs_map_next(&it)) {
        ecs_os_free(*(char**)&it.res[1]);
    }

    ecs_map_fini(&m);

    ecs_fini(world);
}

void SerializeToJson_map_entity_i32(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t e1 = ecs_entity(world, { .name = "e1" });

    ecs_entity_t t = ecs_map_type_init(world, &(ecs_map_desc_t){
        .key_type = ecs_id(ecs_entity_t),
        .type = ecs_id(ecs_i32_t)
    });

    ecs_map_t m;
    ecs_map_init(&m, NULL);
    *(int32_t*)ecs_map_ensure(&m, e1) = 100;

    char *expr = ecs_ptr_to_json(world, t, &m);
    test_assert(expr != NULL);
    test_str(expr, "{\"e1\":100}");
    ecs_os_free(expr);

    ecs_map_fini(&m);

    ecs_fini(world);
}

void SerializeToJson_map_i64_struct(void) {
    ecs_world_t *world = ecs_init();

    typedef struct {
        int32_t x, y, z;
    } N;

    ecs_entity_t n = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "N"}),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)},
            {"z", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t t = ecs_map_type_init(world, &(ecs_map_desc_t){
        .key_type = ecs_id(ecs_i64_t),
        .type = n
    });

    ecs_map_t m;
    ecs_map_init(&m, NULL);
    N *ptr = ecs_map_ensure_alloc_t(&m, N, 10);
    ptr->x = 1;
    ptr->y = 2;
    ptr->z = 3;

    char *expr = ecs_ptr_to_json(world, t, &m);
    test_assert(expr != NULL);
    test_str(expr, "{\"10\":{\"x\":1, \"y\":2, \"z\":3}}");
    ecs_os_free(expr);

    ecs_map_iter_t it = ecs_map_iter(&m);
    while (ecs_map_next(&it)) {
        ecs_os_free(ecs_map_ptr(&it));
    }

    ecs_map_fini(&m);

    ecs_fini(world);
}

void SerializeToJson_map_empty(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_map_type_init(world, &(ecs_map_desc_t){
        .key_type = ecs_id(ecs_i64_t),
        .type = ecs_id(ecs_i32_t)
    });

    ecs_map_t m;
    ecs_map_init(&m, NULL);

    char *expr = ecs_ptr_to_json(world, t, &m);
    test_assert(expr != NULL);
    test_str(expr, "{}");
    ecs_os_free(expr);

    ecs_map_fini(&m);

    ecs_fini(world);
}

void SerializeToJson_map_uninitialized(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_map_type_init(world, &(ecs_map_desc_t){
        .key_type = ecs_id(ecs_i64_t),
        .type = ecs_id(ecs_i32_t)
    });

    ecs_map_t m = {0};

    char *expr = ecs_ptr_to_json(world, t, &m);
    test_assert(expr != NULL);
    test_str(expr, "{}");
    ecs_os_free(expr);

    ecs_fini(world);
}

void SerializeToJson_struct_w_map_i64_i32(void) {
    typedef struct {
        ecs_map_t m;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t mt = ecs_map_type_init(world, &(ecs_map_desc_t){
        .key_type = ecs_id(ecs_i64_t),
        .type = ecs_id(ecs_i32_t)
    });

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "T"}),
        .members = {
            {"m", mt}
        }
    });

    T value;
    ecs_map_init(&value.m, NULL);
    *(int32_t*)ecs_map_ensure(&value.m, 10) = 100;

    char *expr = ecs_ptr_to_json(world, t, &value);
    test_assert(expr != NULL);
    test_str(expr, "{\"m\":{\"10\":100}}");
    ecs_os_free(expr);

    ecs_map_fini(&value.m);

    ecs_fini(world);
}

void SerializeToJson_map_bool_i32(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_map_type_init(world, &(ecs_map_desc_t){
        .key_type = ecs_id(ecs_bool_t),
        .type = ecs_id(ecs_i32_t)
    });

    ecs_map_t m;
    ecs_map_init(&m, NULL);
    *(int32_t*)ecs_map_ensure(&m, false) = 10;
    *(int32_t*)ecs_map_ensure(&m, true) = 20;

    char *expr = ecs_ptr_to_json(world, t, &m);
    test_assert(expr != NULL);
    test_str(expr, "{\"false\":10, \"true\":20}");
    ecs_os_free(expr);

    ecs_map_fini(&m);

    ecs_fini(world);
}

void SerializeToJson_map_char_i32(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_map_type_init(world, &(ecs_map_desc_t){
        .key_type = ecs_id(ecs_char_t),
        .type = ecs_id(ecs_i32_t)
    });

    ecs_map_t m;
    ecs_map_init(&m, NULL);
    *(int32_t*)ecs_map_ensure(&m, 'a') = 100;

    char *expr = ecs_ptr_to_json(world, t, &m);
    test_assert(expr != NULL);
    test_str(expr, "{\"a\":100}");
    ecs_os_free(expr);

    ecs_map_fini(&m);

    ecs_fini(world);
}

void SerializeToJson_map_u64_i32(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_map_type_init(world, &(ecs_map_desc_t){
        .key_type = ecs_id(ecs_u64_t),
        .type = ecs_id(ecs_i32_t)
    });

    ecs_map_t m;
    ecs_map_init(&m, NULL);
    *(int32_t*)ecs_map_ensure(&m, UINT64_MAX) = 100;

    char *expr = ecs_ptr_to_json(world, t, &m);
    test_assert(expr != NULL);
    test_str(expr, "{\"18446744073709551615\":100}");
    ecs_os_free(expr);

    ecs_map_fini(&m);

    ecs_fini(world);
}

void SerializeToJson_map_i32_i32_negative_key(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_map_type_init(world, &(ecs_map_desc_t){
        .key_type = ecs_id(ecs_i32_t),
        .type = ecs_id(ecs_i32_t)
    });

    ecs_map_t m;
    ecs_map_init(&m, NULL);
    *(int32_t*)ecs_map_ensure(&m, (uint32_t)-10) = 100;

    char *expr = ecs_ptr_to_json(world, t, &m);
    test_assert(expr != NULL);
    test_str(expr, "{\"-10\":100}");
    ecs_os_free(expr);

    ecs_map_fini(&m);

    ecs_fini(world);
}

void SerializeToJson_map_enum_i32(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t c = ecs_enum_init(world, &(ecs_enum_desc_t){
        .entity = ecs_entity(world, {.name = "Color"}),
        .constants = {
            {"Red"}, {"Green"}, {"Blue"}
        }
    });

    ecs_entity_t t = ecs_map_type_init(world, &(ecs_map_desc_t){
        .key_type = c,
        .type = ecs_id(ecs_i32_t)
    });

    ecs_map_t m;
    ecs_map_init(&m, NULL);
    *(int32_t*)ecs_map_ensure(&m, 1) = 100;

    char *expr = ecs_ptr_to_json(world, t, &m);
    test_assert(expr != NULL);
    test_str(expr, "{\"Green\":100}");
    ecs_os_free(expr);

    ecs_map_fini(&m);

    ecs_fini(world);
}

void SerializeToJson_map_enum_i32_invalid_key(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t c = ecs_enum_init(world, &(ecs_enum_desc_t){
        .entity = ecs_entity(world, {.name = "Color"}),
        .constants = {
            {"Red"}, {"Green"}, {"Blue"}
        }
    });

    ecs_entity_t t = ecs_map_type_init(world, &(ecs_map_desc_t){
        .key_type = c,
        .type = ecs_id(ecs_i32_t)
    });

    ecs_map_t m;
    ecs_map_init(&m, NULL);
    *(int32_t*)ecs_map_ensure(&m, 10) = 100;

    ecs_log_set_level(-4);
    char *expr = ecs_ptr_to_json(world, t, &m);
    test_assert(expr == NULL);

    ecs_map_fini(&m);

    ecs_fini(world);
}

void SerializeToJson_map_bitmask_i32(void) {
    ecs_world_t *world = ecs_init();

    uint32_t Lettuce = 0x1;
    uint32_t Bacon =   0x1 << 1;

    ecs_entity_t b = ecs_bitmask_init(world, &(ecs_bitmask_desc_t){
        .entity = ecs_entity(world, {.name = "Toppings"}),
        .constants = {
            {"Lettuce"}, {"Bacon"}, {"Tomato"}
        }
    });

    ecs_entity_t t = ecs_map_type_init(world, &(ecs_map_desc_t){
        .key_type = b,
        .type = ecs_id(ecs_i32_t)
    });

    {
    ecs_map_t m;
    ecs_map_init(&m, NULL);
    *(int32_t*)ecs_map_ensure(&m, Lettuce | Bacon) = 100;

    char *expr = ecs_ptr_to_json(world, t, &m);
    test_assert(expr != NULL);
    test_str(expr, "{\"Bacon|Lettuce\":100}");
    ecs_os_free(expr);

    ecs_map_fini(&m);
    }

    {
    ecs_map_t m;
    ecs_map_init(&m, NULL);
    *(int32_t*)ecs_map_ensure(&m, 0) = 100;

    char *expr = ecs_ptr_to_json(world, t, &m);
    test_assert(expr != NULL);
    test_str(expr, "{\"0\":100}");
    ecs_os_free(expr);

    ecs_map_fini(&m);
    }

    ecs_fini(world);
}

void SerializeToJson_map_id_pair_i32(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t rel = ecs_entity(world, { .name = "Rel" });
    ecs_entity_t tgt = ecs_entity(world, { .name = "Tgt" });

    ecs_entity_t t = ecs_map_type_init(world, &(ecs_map_desc_t){
        .key_type = ecs_id(ecs_id_t),
        .type = ecs_id(ecs_i32_t)
    });

    ecs_map_t m;
    ecs_map_init(&m, NULL);
    *(int32_t*)ecs_map_ensure(&m, ecs_pair(rel, tgt)) = 100;

    char *expr = ecs_ptr_to_json(world, t, &m);
    test_assert(expr != NULL);
    test_str(expr, "{\"(Rel,Tgt)\":100}");
    ecs_os_free(expr);

    ecs_map_fini(&m);

    ecs_fini(world);
}

void SerializeToJson_map_i64_enum_underlying_i8(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t c = ecs_enum_init(world, &(ecs_enum_desc_t){
        .entity = ecs_entity(world, {.name = "Color"}),
        .underlying_type = ecs_id(ecs_i8_t),
        .constants = {
            {"Red"}, {"Green"}, {"Blue"}
        }
    });

    ecs_entity_t t = ecs_map_type_init(world, &(ecs_map_desc_t){
        .key_type = ecs_id(ecs_i64_t),
        .type = c
    });

    ecs_map_t m;
    ecs_map_init(&m, NULL);
    *(int8_t*)ecs_map_ensure(&m, 10) = 0;
    *(int8_t*)ecs_map_ensure(&m, 20) = 1;
    *(int8_t*)ecs_map_ensure(&m, 30) = 2;

    char *expr = ecs_ptr_to_json(world, t, &m);
    test_assert(expr != NULL);
    test_str(expr, "{\"10\":\"Red\", \"20\":\"Green\", \"30\":\"Blue\"}");
    ecs_os_free(expr);

    ecs_map_fini(&m);

    ecs_fini(world);
}

void SerializeToJson_map_i64_enum_underlying_i16(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t c = ecs_enum_init(world, &(ecs_enum_desc_t){
        .entity = ecs_entity(world, {.name = "Color"}),
        .underlying_type = ecs_id(ecs_i16_t),
        .constants = {
            {"Red"}, {"Green"}, {"Blue"}
        }
    });

    ecs_entity_t t = ecs_map_type_init(world, &(ecs_map_desc_t){
        .key_type = ecs_id(ecs_i64_t),
        .type = c
    });

    ecs_map_t m;
    ecs_map_init(&m, NULL);
    *(int16_t*)ecs_map_ensure(&m, 10) = 0;
    *(int16_t*)ecs_map_ensure(&m, 20) = 1;
    *(int16_t*)ecs_map_ensure(&m, 30) = 2;

    char *expr = ecs_ptr_to_json(world, t, &m);
    test_assert(expr != NULL);
    test_str(expr, "{\"10\":\"Red\", \"20\":\"Green\", \"30\":\"Blue\"}");
    ecs_os_free(expr);

    ecs_map_fini(&m);

    ecs_fini(world);
}

void SerializeToJson_map_i64_enum_underlying_i32(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t c = ecs_enum_init(world, &(ecs_enum_desc_t){
        .entity = ecs_entity(world, {.name = "Color"}),
        .underlying_type = ecs_id(ecs_i32_t),
        .constants = {
            {"Red"}, {"Green"}, {"Blue"}
        }
    });

    ecs_entity_t t = ecs_map_type_init(world, &(ecs_map_desc_t){
        .key_type = ecs_id(ecs_i64_t),
        .type = c
    });

    ecs_map_t m;
    ecs_map_init(&m, NULL);
    *(int32_t*)ecs_map_ensure(&m, 10) = 0;
    *(int32_t*)ecs_map_ensure(&m, 20) = 1;
    *(int32_t*)ecs_map_ensure(&m, 30) = 2;

    char *expr = ecs_ptr_to_json(world, t, &m);
    test_assert(expr != NULL);
    test_str(expr, "{\"10\":\"Red\", \"20\":\"Green\", \"30\":\"Blue\"}");
    ecs_os_free(expr);

    ecs_map_fini(&m);

    ecs_fini(world);
}

void SerializeToJson_value_bool(void) {
    ecs_world_t *world = ecs_init();

    bool value = true;
    ecs_value_t v = ecs_value_init(world, ecs_id(ecs_bool_t), &value);

    char *json = ecs_ptr_to_json(world, ecs_id(ecs_value_t), &v);
    test_assert(json != NULL);
    test_str(json, "{\"bool\":true}");
    ecs_os_free(json);

    ecs_value_fini(world, &v);

    ecs_fini(world);
}

void SerializeToJson_value_u16(void) {
    ecs_world_t *world = ecs_init();

    uint16_t value = 10;
    ecs_value_t v = ecs_value_init(world, ecs_id(ecs_u16_t), &value);

    char *json = ecs_ptr_to_json(world, ecs_id(ecs_value_t), &v);
    test_assert(json != NULL);
    test_str(json, "{\"u16\":10}");
    ecs_os_free(json);

    ecs_value_fini(world, &v);

    ecs_fini(world);
}

void SerializeToJson_value_i64(void) {
    ecs_world_t *world = ecs_init();

    int64_t value = 10;
    ecs_value_t v = ecs_value_init(world, ecs_id(ecs_i64_t), &value);

    char *json = ecs_ptr_to_json(world, ecs_id(ecs_value_t), &v);
    test_assert(json != NULL);
    test_str(json, "{\"i64\":10}");
    ecs_os_free(json);

    ecs_value_fini(world, &v);

    ecs_fini(world);
}

void SerializeToJson_value_f64(void) {
    ecs_world_t *world = ecs_init();

    double value = 10.5;
    ecs_value_t v = ecs_value_init(world, ecs_id(ecs_f64_t), &value);

    char *json = ecs_ptr_to_json(world, ecs_id(ecs_value_t), &v);
    test_assert(json != NULL);
    test_str(json, "{\"f64\":10.5}");
    ecs_os_free(json);

    ecs_value_fini(world, &v);

    ecs_fini(world);
}

void SerializeToJson_value_string(void) {
    ecs_world_t *world = ecs_init();

    char *value = "Hello World";
    ecs_value_t v = ecs_value_init(world, ecs_id(ecs_string_t), &value);

    char *json = ecs_ptr_to_json(world, ecs_id(ecs_value_t), &v);
    test_assert(json != NULL);
    test_str(json, "{\"string\":\"Hello World\"}");
    ecs_os_free(json);

    ecs_value_fini(world, &v);

    ecs_fini(world);
}

void SerializeToJson_value_entity(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t value = EcsFlecsCore;
    ecs_value_t v = ecs_value_init(world, ecs_id(ecs_entity_t), &value);

    char *json = ecs_ptr_to_json(world, ecs_id(ecs_value_t), &v);
    test_assert(json != NULL);
    test_str(json, "{\"entity\":\"flecs.core\"}");
    ecs_os_free(json);

    ecs_value_fini(world, &v);

    ecs_fini(world);
}

void SerializeToJson_value_enum(void) {
    typedef enum {
        Red, Green, Blue
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_enum(world, {
        .entity = ecs_entity(world, { .name = "T" }),
        .constants = {
            { "Red" }, { "Green" }, { "Blue" }
        }
    });

    T value = Green;
    ecs_value_t v = ecs_value_init(world, t, &value);

    char *json = ecs_ptr_to_json(world, ecs_id(ecs_value_t), &v);
    test_assert(json != NULL);
    test_str(json, "{\"T\":\"Green\"}");
    ecs_os_free(json);

    ecs_value_fini(world, &v);

    ecs_fini(world);
}

void SerializeToJson_value_struct(void) {
    typedef struct {
        int32_t x;
        int32_t y;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "T" }),
        .members = {
            { "x", ecs_id(ecs_i32_t) },
            { "y", ecs_id(ecs_i32_t) }
        }
    });

    T value = {10, 20};
    ecs_value_t v = ecs_value_init(world, t, &value);

    char *json = ecs_ptr_to_json(world, ecs_id(ecs_value_t), &v);
    test_assert(json != NULL);
    test_str(json, "{\"T\":{\"x\":10, \"y\":20}}");
    ecs_os_free(json);

    ecs_value_fini(world, &v);

    ecs_fini(world);
}

void SerializeToJson_value_array(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_array(world, {
        .entity = ecs_entity(world, { .name = "T" }),
        .type = ecs_id(ecs_i32_t),
        .count = 3
    });

    int32_t value[] = {1, 2, 3};
    ecs_value_t v = ecs_value_init(world, t, value);

    char *json = ecs_ptr_to_json(world, ecs_id(ecs_value_t), &v);
    test_assert(json != NULL);
    test_str(json, "{\"T\":[1, 2, 3]}");
    ecs_os_free(json);

    ecs_value_fini(world, &v);

    ecs_fini(world);
}

void SerializeToJson_value_vector(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_vector(world, {
        .entity = ecs_entity(world, { .name = "T" }),
        .type = ecs_id(ecs_i32_t)
    });

    ecs_vec_t vec = {0};
    ecs_vec_init_t(NULL, &vec, int32_t, 3);
    ecs_vec_append_t(NULL, &vec, int32_t)[0] = 1;
    ecs_vec_append_t(NULL, &vec, int32_t)[0] = 2;
    ecs_vec_append_t(NULL, &vec, int32_t)[0] = 3;

    ecs_value_t v = ecs_value_init(world, t, &vec);

    char *json = ecs_ptr_to_json(world, ecs_id(ecs_value_t), &v);
    test_assert(json != NULL);
    test_str(json, "{\"T\":[1, 2, 3]}");
    ecs_os_free(json);

    ecs_value_fini(world, &v);
    ecs_vec_fini_t(NULL, &vec, int32_t);

    ecs_fini(world);
}

void SerializeToJson_value_map(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_map_type(world, {
        .entity = ecs_entity(world, { .name = "T" }),
        .key_type = ecs_id(ecs_i64_t),
        .type = ecs_id(ecs_i32_t)
    });

    ecs_map_t map = {0};
    ecs_map_init(&map, NULL);
    ecs_map_val_t *value = ecs_map_ensure(&map, 10);
    *(int32_t*)value = 100;

    ecs_value_t v = ecs_value_init(world, t, &map);

    char *json = ecs_ptr_to_json(world, ecs_id(ecs_value_t), &v);
    test_assert(json != NULL);
    test_str(json, "{\"T\":{\"10\":100}}");
    ecs_os_free(json);

    ecs_value_fini(world, &v);
    ecs_map_fini(&map);

    ecs_fini(world);
}

void SerializeToJson_struct_w_value(void) {
    typedef struct {
        ecs_value_t v;
    } S;

    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(S) = ecs_struct(world, {
        .members = {
            { "v", ecs_id(ecs_value_t) }
        }
    });

    int32_t i = 10;
    S value = { .v = ecs_value_init(world, ecs_id(ecs_i32_t), &i) };

    char *json = ecs_ptr_to_json(world, ecs_id(S), &value);
    test_assert(json != NULL);
    test_str(json, "{\"v\":{\"i32\":10}}");
    ecs_os_free(json);

    ecs_value_fini(world, &value.v);

    ecs_fini(world);
}

void SerializeToJson_value_roundtrip(void) {
    typedef struct {
        int32_t x;
        int32_t y;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "T" }),
        .members = {
            { "x", ecs_id(ecs_i32_t) },
            { "y", ecs_id(ecs_i32_t) }
        }
    });

    T value = {10, 20};
    ecs_value_t v = ecs_value_init(world, t, &value);

    char *json = ecs_ptr_to_json(world, ecs_id(ecs_value_t), &v);
    test_assert(json != NULL);

    ecs_value_t v2 = {0};
    const char *ptr = ecs_ptr_from_json(
        world, ecs_id(ecs_value_t), &v2, json, NULL);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');
    ecs_os_free(json);

    test_uint(v2.type, t);
    test_assert(v2.ptr != NULL);
    T *vptr = v2.ptr;
    test_int(vptr->x, 10);
    test_int(vptr->y, 20);

    ecs_value_fini(world, &v);
    ecs_value_fini(world, &v2);

    ecs_fini(world);
}

void SerializeToJson_ecs_struct_1_member(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "T"}),
        .members = {
            {"x", ecs_id(ecs_i32_t)}
        }
    });

    const EcsStruct *ptr = ecs_get(world, t, EcsStruct);
    test_assert(ptr != NULL);

    char *expr = ecs_ptr_to_json(world, ecs_id(EcsStruct), ptr);
    test_assert(expr != NULL);
    test_str(expr, "{\"members\":[{\"name\":\"x\", \"type\":\"flecs.meta.i32\", \"count\":0, \"offset\":0, \"unit\":\"#0\", \"use_offset\":false, \"range\":{\"min\":0, \"max\":0}, \"error_range\":{\"min\":0, \"max\":0}, \"warning_range\":{\"min\":0, \"max\":0}, \"size\":4, \"member\":\"#0\"}]}");
    ecs_os_free(expr);

    ecs_fini(world);
}

void SerializeToJson_ecs_struct_2_members(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "T"}),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    const EcsStruct *ptr = ecs_get(world, t, EcsStruct);
    test_assert(ptr != NULL);

    char *expr = ecs_ptr_to_json(world, ecs_id(EcsStruct), ptr);
    test_assert(expr != NULL);
    test_str(expr, "{\"members\":[{\"name\":\"x\", \"type\":\"flecs.meta.i32\", \"count\":0, \"offset\":0, \"unit\":\"#0\", \"use_offset\":false, \"range\":{\"min\":0, \"max\":0}, \"error_range\":{\"min\":0, \"max\":0}, \"warning_range\":{\"min\":0, \"max\":0}, \"size\":4, \"member\":\"#0\"}, {\"name\":\"y\", \"type\":\"flecs.meta.f32\", \"count\":0, \"offset\":4, \"unit\":\"#0\", \"use_offset\":false, \"range\":{\"min\":0, \"max\":0}, \"error_range\":{\"min\":0, \"max\":0}, \"warning_range\":{\"min\":0, \"max\":0}, \"size\":4, \"member\":\"#0\"}]}");
    ecs_os_free(expr);

    ecs_fini(world);
}

void SerializeToJson_ecs_struct_3_members(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "T"}),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_f32_t)},
            {"z", ecs_id(ecs_string_t)}
        }
    });

    const EcsStruct *ptr = ecs_get(world, t, EcsStruct);
    test_assert(ptr != NULL);

    char *expr = ecs_ptr_to_json(world, ecs_id(EcsStruct), ptr);
    test_assert(expr != NULL);
    test_str(expr, "{\"members\":[{\"name\":\"x\", \"type\":\"flecs.meta.i32\", \"count\":0, \"offset\":0, \"unit\":\"#0\", \"use_offset\":false, \"range\":{\"min\":0, \"max\":0}, \"error_range\":{\"min\":0, \"max\":0}, \"warning_range\":{\"min\":0, \"max\":0}, \"size\":4, \"member\":\"#0\"}, {\"name\":\"y\", \"type\":\"flecs.meta.f32\", \"count\":0, \"offset\":4, \"unit\":\"#0\", \"use_offset\":false, \"range\":{\"min\":0, \"max\":0}, \"error_range\":{\"min\":0, \"max\":0}, \"warning_range\":{\"min\":0, \"max\":0}, \"size\":4, \"member\":\"#0\"}, {\"name\":\"z\", \"type\":\"flecs.meta.string\", \"count\":0, \"offset\":8, \"unit\":\"#0\", \"use_offset\":false, \"range\":{\"min\":0, \"max\":0}, \"error_range\":{\"min\":0, \"max\":0}, \"warning_range\":{\"min\":0, \"max\":0}, \"size\":8, \"member\":\"#0\"}]}");
    ecs_os_free(expr);

    ecs_fini(world);
}
