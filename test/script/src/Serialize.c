#include <script.h>
#include <math.h>

void Serialize_bool(void) {
    ecs_world_t *world = ecs_init();

    {
    bool value = true;
    char *expr = ecs_ptr_to_expr(world, ecs_id(ecs_bool_t), &value);
    test_assert(expr != NULL);
    test_str(expr, "true");
    ecs_os_free(expr);
    }

    {
    bool value = false;
    char *expr = ecs_ptr_to_expr(world, ecs_id(ecs_bool_t), &value);
    test_assert(expr != NULL);
    test_str(expr, "false");
    ecs_os_free(expr);
    }

    ecs_fini(world);
}

void Serialize_byte(void) {
    ecs_world_t *world = ecs_init();

    {
    ecs_byte_t value = 0;
    char *expr = ecs_ptr_to_expr(world, ecs_id(ecs_byte_t), &value);
    test_assert(expr != NULL);
    test_str(expr, "0");
    ecs_os_free(expr);
    }

    {
    ecs_byte_t value = 10;
    char *expr = ecs_ptr_to_expr(world, ecs_id(ecs_byte_t), &value);
    test_assert(expr != NULL);
    test_str(expr, "10");
    ecs_os_free(expr);
    }

    {
    ecs_byte_t value = 11;
    char *expr = ecs_ptr_to_expr(world, ecs_id(ecs_byte_t), &value);
    test_assert(expr != NULL);
    test_str(expr, "11");
    ecs_os_free(expr);
    }

    ecs_fini(world);
}

void Serialize_char(void) {
    ecs_world_t *world = ecs_init();

    {
    ecs_char_t value = 0;
    char *expr = ecs_ptr_to_expr(world, ecs_id(ecs_char_t), &value);
    test_assert(expr != NULL);
    test_str(expr, "0");
    ecs_os_free(expr);
    }

    {
    ecs_char_t value = 'a';
    char *expr = ecs_ptr_to_expr(world, ecs_id(ecs_char_t), &value);
    test_assert(expr != NULL);
    test_str(expr, "\"a\"");
    ecs_os_free(expr);
    }

    {
    ecs_char_t value = '"';
    char *expr = ecs_ptr_to_expr(world, ecs_id(ecs_char_t), &value);
    test_assert(expr != NULL);
    test_str(expr, "\"\\\"\"");
    ecs_os_free(expr);
    }

    {
    ecs_char_t value = '\'';
    char *expr = ecs_ptr_to_expr(world, ecs_id(ecs_char_t), &value);
    test_assert(expr != NULL);
    test_str(expr, "\"'\"");
    ecs_os_free(expr);
    }

    ecs_fini(world);
}

void Serialize_i8(void) {
    ecs_world_t *world = ecs_init();

    {
    ecs_i8_t value = 0;
    char *expr = ecs_ptr_to_expr(world, ecs_id(ecs_i8_t), &value);
    test_assert(expr != NULL);
    test_str(expr, "0");
    ecs_os_free(expr);
    }

    {
    ecs_i8_t value = 10;
    char *expr = ecs_ptr_to_expr(world, ecs_id(ecs_i8_t), &value);
    test_assert(expr != NULL);
    test_str(expr, "10");
    ecs_os_free(expr);
    }

    {
    ecs_i8_t value = -10;
    char *expr = ecs_ptr_to_expr(world, ecs_id(ecs_i8_t), &value);
    test_assert(expr != NULL);
    test_str(expr, "-10");
    ecs_os_free(expr);
    }

    ecs_fini(world);
}

void Serialize_i16(void) {
    ecs_world_t *world = ecs_init();

    {
    ecs_i16_t value = 0;
    char *expr = ecs_ptr_to_expr(world, ecs_id(ecs_i16_t), &value);
    test_assert(expr != NULL);
    test_str(expr, "0");
    ecs_os_free(expr);
    }

    {
    ecs_i16_t value = 10;
    char *expr = ecs_ptr_to_expr(world, ecs_id(ecs_i16_t), &value);
    test_assert(expr != NULL);
    test_str(expr, "10");
    ecs_os_free(expr);
    }

    {
    ecs_i16_t value = -10;
    char *expr = ecs_ptr_to_expr(world, ecs_id(ecs_i16_t), &value);
    test_assert(expr != NULL);
    test_str(expr, "-10");
    ecs_os_free(expr);
    }

    ecs_fini(world);
}

void Serialize_i32(void) {
    ecs_world_t *world = ecs_init();

    {
    ecs_i32_t value = 0;
    char *expr = ecs_ptr_to_expr(world, ecs_id(ecs_i32_t), &value);
    test_assert(expr != NULL);
    test_str(expr, "0");
    ecs_os_free(expr);
    }

    {
    ecs_i32_t value = 10;
    char *expr = ecs_ptr_to_expr(world, ecs_id(ecs_i32_t), &value);
    test_assert(expr != NULL);
    test_str(expr, "10");
    ecs_os_free(expr);
    }

    {
    ecs_i32_t value = -10;
    char *expr = ecs_ptr_to_expr(world, ecs_id(ecs_i32_t), &value);
    test_assert(expr != NULL);
    test_str(expr, "-10");
    ecs_os_free(expr);
    }

    ecs_fini(world);
}

void Serialize_i64(void) {
    ecs_world_t *world = ecs_init();

    {
    ecs_i64_t value = 0;
    char *expr = ecs_ptr_to_expr(world, ecs_id(ecs_i64_t), &value);
    test_assert(expr != NULL);
    test_str(expr, "0");
    ecs_os_free(expr);
    }

    {
    ecs_i64_t value = 10;
    char *expr = ecs_ptr_to_expr(world, ecs_id(ecs_i64_t), &value);
    test_assert(expr != NULL);
    test_str(expr, "10");
    ecs_os_free(expr);
    }

    {
    ecs_i64_t value = -10;
    char *expr = ecs_ptr_to_expr(world, ecs_id(ecs_i64_t), &value);
    test_assert(expr != NULL);
    test_str(expr, "-10");
    ecs_os_free(expr);
    }

    ecs_fini(world);
}

void Serialize_iptr(void) {
    ecs_world_t *world = ecs_init();

    {
    ecs_iptr_t value = 0;
    char *expr = ecs_ptr_to_expr(world, ecs_id(ecs_iptr_t), &value);
    test_assert(expr != NULL);
    test_str(expr, "0");
    ecs_os_free(expr);
    }

    {
    ecs_iptr_t value = 10;
    char *expr = ecs_ptr_to_expr(world, ecs_id(ecs_iptr_t), &value);
    test_assert(expr != NULL);
    test_str(expr, "10");
    ecs_os_free(expr);
    }

    {
    ecs_iptr_t value = -10;
    char *expr = ecs_ptr_to_expr(world, ecs_id(ecs_iptr_t), &value);
    test_assert(expr != NULL);
    test_str(expr, "-10");
    ecs_os_free(expr);
    }

    ecs_fini(world);
}

void Serialize_u8(void) {
    ecs_world_t *world = ecs_init();

    {
    ecs_u8_t value = 0;
    char *expr = ecs_ptr_to_expr(world, ecs_id(ecs_u8_t), &value);
    test_assert(expr != NULL);
    test_str(expr, "0");
    ecs_os_free(expr);
    }

    {
    ecs_u8_t value = 10;
    char *expr = ecs_ptr_to_expr(world, ecs_id(ecs_u8_t), &value);
    test_assert(expr != NULL);
    test_str(expr, "10");
    ecs_os_free(expr);
    }

    ecs_fini(world);
}

void Serialize_u16(void) {
    ecs_world_t *world = ecs_init();

    {
    ecs_u16_t value = 0;
    char *expr = ecs_ptr_to_expr(world, ecs_id(ecs_u16_t), &value);
    test_assert(expr != NULL);
    test_str(expr, "0");
    ecs_os_free(expr);
    }

    {
    ecs_u16_t value = 10;
    char *expr = ecs_ptr_to_expr(world, ecs_id(ecs_u16_t), &value);
    test_assert(expr != NULL);
    test_str(expr, "10");
    ecs_os_free(expr);
    }

    ecs_fini(world);
}

void Serialize_u32(void) {
    ecs_world_t *world = ecs_init();

    {
    ecs_u32_t value = 0;
    char *expr = ecs_ptr_to_expr(world, ecs_id(ecs_u32_t), &value);
    test_assert(expr != NULL);
    test_str(expr, "0");
    ecs_os_free(expr);
    }

    {
    ecs_u32_t value = 10;
    char *expr = ecs_ptr_to_expr(world, ecs_id(ecs_u32_t), &value);
    test_assert(expr != NULL);
    test_str(expr, "10");
    ecs_os_free(expr);
    }

    ecs_fini(world);
}

void Serialize_u64(void) {
    ecs_world_t *world = ecs_init();

    {
    ecs_u64_t value = 0;
    char *expr = ecs_ptr_to_expr(world, ecs_id(ecs_u64_t), &value);
    test_assert(expr != NULL);
    test_str(expr, "0");
    ecs_os_free(expr);
    }

    {
    ecs_u64_t value = 10;
    char *expr = ecs_ptr_to_expr(world, ecs_id(ecs_u64_t), &value);
    test_assert(expr != NULL);
    test_str(expr, "10");
    ecs_os_free(expr);
    }

    {
    ecs_u64_t value = 2366700781656087864;
    char *expr = ecs_ptr_to_expr(world, ecs_id(ecs_u64_t), &value);
    test_assert(expr != NULL);
    test_str(expr, "2366700781656087864");
    ecs_os_free(expr);
    }

    ecs_fini(world);
}

void Serialize_uptr(void) {
    ecs_world_t *world = ecs_init();

    {
    ecs_uptr_t value = 0;
    char *expr = ecs_ptr_to_expr(world, ecs_id(ecs_uptr_t), &value);
    test_assert(expr != NULL);
    test_str(expr, "0");
    ecs_os_free(expr);
    }

    {
    ecs_uptr_t value = 10;
    char *expr = ecs_ptr_to_expr(world, ecs_id(ecs_uptr_t), &value);
    test_assert(expr != NULL);
    test_str(expr, "10");
    ecs_os_free(expr);
    }

    ecs_fini(world);
}

void Serialize_float(void) {
    ecs_world_t *world = ecs_init();

    {
    ecs_f32_t value = 0;
    char *expr = ecs_ptr_to_expr(world, ecs_id(ecs_f32_t), &value);
    test_assert(expr != NULL);
    test_str(expr, "0");
    ecs_os_free(expr);
    }

    {
    ecs_f32_t value = 10.5;
    char *expr = ecs_ptr_to_expr(world, ecs_id(ecs_f32_t), &value);
    test_assert(expr != NULL);
    test_str(expr, "10.5");
    ecs_os_free(expr);
    }

    {
    ecs_f32_t value = -10.5;
    char *expr = ecs_ptr_to_expr(world, ecs_id(ecs_f32_t), &value);
    test_assert(expr != NULL);
    test_str(expr, "-10.5");
    ecs_os_free(expr);
    }

    ecs_fini(world);
}

void Serialize_double(void) {
    ecs_world_t *world = ecs_init();

    {
    ecs_f64_t value = 0;
    char *expr = ecs_ptr_to_expr(world, ecs_id(ecs_f64_t), &value);
    test_assert(expr != NULL);
    test_str(expr, "0");
    ecs_os_free(expr);
    }

    {
    ecs_f64_t value = 10.5;
    char *expr = ecs_ptr_to_expr(world, ecs_id(ecs_f64_t), &value);
    test_assert(expr != NULL);
    test_str(expr, "10.5");
    ecs_os_free(expr);
    }

    {
    ecs_f64_t value = -10.5;
    char *expr = ecs_ptr_to_expr(world, ecs_id(ecs_f64_t), &value);
    test_assert(expr != NULL);
    test_str(expr, "-10.5");
    ecs_os_free(expr);
    }

    ecs_fini(world);
}

void Serialize_float_nan(void) {
    ecs_world_t *world = ecs_init();

    {
    ecs_f32_t value = NAN;
    char *expr = ecs_ptr_to_expr(world, ecs_id(ecs_f32_t), &value);
    test_assert(expr != NULL);
    test_str(expr, "NaN");
    ecs_os_free(expr);
    }

    ecs_fini(world);
}

void Serialize_float_inf(void) {
    ecs_world_t *world = ecs_init();

    {
    ecs_f32_t value = INFINITY;
    char *expr = ecs_ptr_to_expr(world, ecs_id(ecs_f32_t), &value);
    test_assert(expr != NULL);
    test_str(expr, "Inf");
    ecs_os_free(expr);
    }

    {
    ecs_f32_t value = -INFINITY;
    char *expr = ecs_ptr_to_expr(world, ecs_id(ecs_f32_t), &value);
    test_assert(expr != NULL);
    test_str(expr, "Inf");
    ecs_os_free(expr);
    }

    ecs_fini(world);
}

void Serialize_double_nan(void) {
    ecs_world_t *world = ecs_init();

    {
    ecs_f64_t value = NAN;
    char *expr = ecs_ptr_to_expr(world, ecs_id(ecs_f64_t), &value);
    test_assert(expr != NULL);
    test_str(expr, "NaN");
    ecs_os_free(expr);
    }

    ecs_fini(world);
}

void Serialize_double_inf(void) {
    ecs_world_t *world = ecs_init();

    {
    ecs_f64_t value = INFINITY;
    char *expr = ecs_ptr_to_expr(world, ecs_id(ecs_f64_t), &value);
    test_assert(expr != NULL);
    test_str(expr, "Inf");
    ecs_os_free(expr);
    }

    {
    ecs_f64_t value = -INFINITY;
    char *expr = ecs_ptr_to_expr(world, ecs_id(ecs_f64_t), &value);
    test_assert(expr != NULL);
    test_str(expr, "Inf");
    ecs_os_free(expr);
    }

    ecs_fini(world);
}

void Serialize_string(void) {
    ecs_world_t *world = ecs_init();

    {
    const char* value = 0;
    char *expr = ecs_ptr_to_expr(world, ecs_id(ecs_string_t), &value);
    test_assert(expr != NULL);
    test_str(expr, "null");
    ecs_os_free(expr);
    }

    {
    const char *value = "Hello World";
    char *expr = ecs_ptr_to_expr(world, ecs_id(ecs_string_t), &value);
    test_assert(expr != NULL);
    test_str(expr, "\"Hello World\"");
    ecs_os_free(expr);
    }

    {
    const char *value = "Hello \\World";
    char *expr = ecs_ptr_to_expr(world, ecs_id(ecs_string_t), &value);
    test_assert(expr != NULL);
    test_str(expr, "\"Hello \\\\World\"");
    ecs_os_free(expr);
    }

    {
    const char *value = "Hello \"World\"";
    char *expr = ecs_ptr_to_expr(world, ecs_id(ecs_string_t), &value);
    test_assert(expr != NULL);
    test_str(expr, "\"Hello \\\"World\\\"\"");
    ecs_os_free(expr);
    }

    ecs_fini(world);
}

void Serialize_entity(void) {
    ecs_world_t *world = ecs_init();

    {
    ecs_entity_t value = 0;
    char *expr = ecs_ptr_to_expr(world, ecs_id(ecs_entity_t), &value);
    test_assert(expr != NULL);
    test_str(expr, "#0");
    ecs_os_free(expr);
    }

    {
    ecs_entity_t value = EcsFlecsCore;
    char *expr = ecs_ptr_to_expr(world, ecs_id(ecs_entity_t), &value);
    test_assert(expr != NULL);
    test_str(expr, "flecs.core");
    ecs_os_free(expr);
    }

    ecs_fini(world);
}

void Serialize_entity_10k(void) {
    ecs_world_t *world = ecs_init();

    {
    ecs_entity_t value = 10000;
    char *expr = ecs_ptr_to_expr(world, ecs_id(ecs_entity_t), &value);
    test_assert(expr != NULL);
    test_str(expr, "#10000");
    ecs_os_free(expr);
    }

    ecs_fini(world);
}

void Serialize_id(void) {
    ecs_world_t *world = ecs_init();

    {
    ecs_id_t value = 0;
    char *expr = ecs_ptr_to_expr(world, ecs_id(ecs_id_t), &value);
    test_assert(expr != NULL);
    test_str(expr, "#0");
    ecs_os_free(expr);
    }

    {
    ecs_id_t value = EcsFlecsCore;
    char *expr = ecs_ptr_to_expr(world, ecs_id(ecs_id_t), &value);
    test_assert(expr != NULL);
    test_str(expr, "flecs.core");
    ecs_os_free(expr);
    }

    {
    ecs_id_t value = ecs_pair(EcsChildOf, EcsFlecsCore);
    char *expr = ecs_ptr_to_expr(world, ecs_id(ecs_id_t), &value);
    test_assert(expr != NULL);
    test_str(expr, "(ChildOf,flecs.core)");
    ecs_os_free(expr);
    }

    ecs_fini(world);
}

void Serialize_enum(void) {
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
    T value = Red;
    char *expr = ecs_ptr_to_expr(world, t, &value);
    test_assert(expr != NULL);
    test_str(expr, "Red");
    ecs_os_free(expr);
    }

    {
    T value = Blue;
    char *expr = ecs_ptr_to_expr(world, t, &value);
    test_assert(expr != NULL);
    test_str(expr, "Blue");
    ecs_os_free(expr);
    }

    {
    T value = Green;
    char *expr = ecs_ptr_to_expr(world, t, &value);
    test_assert(expr != NULL);
    test_str(expr, "Green");
    ecs_os_free(expr);
    }

    {
    T value = 10;
    ecs_log_set_level(-4);
    char *expr = ecs_ptr_to_expr(world, t, &value);
    test_assert(expr == NULL);
    }

    ecs_fini(world);
}

void Serialize_bitmask(void) {
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

    test_assert(t != 0);

    {
    uint32_t value = 0;
    char *expr = ecs_ptr_to_expr(world, t, &value);
    test_assert(expr != NULL);
    test_str(expr, "0");
    ecs_os_free(expr);
    }

    {
    uint32_t value = Lettuce;
    char *expr = ecs_ptr_to_expr(world, t, &value);
    test_assert(expr != NULL);
    test_str(expr, "Lettuce");
    ecs_os_free(expr);
    }

    {
    uint32_t value = Bacon;
    char *expr = ecs_ptr_to_expr(world, t, &value);
    test_assert(expr != NULL);
    test_str(expr, "Bacon");
    ecs_os_free(expr);
    }

    {
    uint32_t value = Lettuce | Bacon;
    char *expr = ecs_ptr_to_expr(world, t, &value);
    test_assert(expr != NULL);
    test_str(expr, "Bacon|Lettuce");
    ecs_os_free(expr);
    }

    {
    uint32_t value = Lettuce | Bacon | Tomato | Cheese;
    char *expr = ecs_ptr_to_expr(world, t, &value);
    test_assert(expr != NULL);
    test_str(expr, "Bacon|Tomato|Lettuce|Cheese");
    ecs_os_free(expr);
    }

    {
    ecs_log_set_level(-4);
    uint32_t value = 16;
    char *expr = ecs_ptr_to_expr(world, t, &value);
    test_assert(expr == NULL);
    }

    ecs_fini(world);
}

void Serialize_struct_enum(void) {
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
    char *expr = ecs_ptr_to_expr(world, t, &value);
    test_assert(expr != NULL);
    test_str(expr, "{x: Red}");
    ecs_os_free(expr);
    }

    {
    T value = {Blue};
    char *expr = ecs_ptr_to_expr(world, t, &value);
    test_assert(expr != NULL);
    test_str(expr, "{x: Blue}");
    ecs_os_free(expr);
    }

    {
    T value = {Green};
    char *expr = ecs_ptr_to_expr(world, t, &value);
    test_assert(expr != NULL);
    test_str(expr, "{x: Green}");
    ecs_os_free(expr);
    }

    {
    ecs_log_set_level(-4);
    T value = {10};
    char *expr = ecs_ptr_to_expr(world, t, &value);
    test_assert(expr == NULL);
    }

    ecs_fini(world);
}

void Serialize_struct_bitmask(void) {
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
    char *expr = ecs_ptr_to_expr(world, t, &value);
    test_assert(expr != NULL);
    test_str(expr, "{x: 0}");
    ecs_os_free(expr);
    }

    {
    T value = {Lettuce};
    char *expr = ecs_ptr_to_expr(world, t, &value);
    test_assert(expr != NULL);
    test_str(expr, "{x: Lettuce}");
    ecs_os_free(expr);
    }

    {
    T value = {Bacon};
    char *expr = ecs_ptr_to_expr(world, t, &value);
    test_assert(expr != NULL);
    test_str(expr, "{x: Bacon}");
    ecs_os_free(expr);
    }

    {
    T value = {Lettuce | Bacon};
    char *expr = ecs_ptr_to_expr(world, t, &value);
    test_assert(expr != NULL);
    test_str(expr, "{x: Bacon|Lettuce}");
    ecs_os_free(expr);
    }

    {
    T value = {Lettuce | Bacon | Tomato | Cheese};
    char *expr = ecs_ptr_to_expr(world, t, &value);
    test_assert(expr != NULL);
    test_str(expr, "{x: Bacon|Tomato|Lettuce|Cheese}");
    ecs_os_free(expr);
    }

    {
    ecs_log_set_level(-4);
    T value = {16};
    char *expr = ecs_ptr_to_expr(world, t, &value);
    test_assert(expr == NULL);
    }

    ecs_fini(world);
}

void Serialize_struct_i32(void) {
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

    T value = {10};
    char *expr = ecs_ptr_to_expr(world, t, &value);
    test_assert(expr != NULL);
    test_str(expr, "{x: 10}");
    ecs_os_free(expr);

    ecs_fini(world);
}

void Serialize_struct_i32_i32(void) {
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

    T value = {10, 20};
    char *expr = ecs_ptr_to_expr(world, t, &value);
    test_assert(expr != NULL);
    test_str(expr, "{x: 10, y: 20}");
    ecs_os_free(expr);

    ecs_fini(world);
}

void Serialize_struct_entity(void) {
    typedef struct {
        ecs_entity_t e;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "T"}),
        .members = {
            {"e", ecs_id(ecs_entity_t)}
        }
    });

    T value = {EcsFlecsCore};
    char *expr = ecs_ptr_to_expr(world, t, &value);
    test_assert(expr != NULL);
    test_str(expr, "{e: flecs.core}");
    ecs_os_free(expr);

    ecs_fini(world);
}

void Serialize_struct_id(void) {
    typedef struct {
        ecs_id_t e;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "T"}),
        .members = {
            {"e", ecs_id(ecs_id_t)}
        }
    });

    T value = {EcsFlecsCore};
    char *expr = ecs_ptr_to_expr(world, t, &value);
    test_assert(expr != NULL);
    test_str(expr, "{e: flecs.core}");
    ecs_os_free(expr);

    ecs_fini(world);
}

void Serialize_entity_entity_after_float(void) {
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
    char *expr = ecs_ptr_to_expr(world, t, &value);
    test_assert(expr != NULL);
    test_str(expr, "{v: 10, e: flecs.core}");
    ecs_os_free(expr);

    ecs_fini(world);
}

void Serialize_struct_nested_i32(void) {
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
    char *expr = ecs_ptr_to_expr(world, t, &value);
    test_assert(expr != NULL);
    test_str(expr, "{n_1: {x: 10}}");
    ecs_os_free(expr);

    ecs_fini(world);
}

void Serialize_struct_nested_i32_i32(void) {
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
    char *expr = ecs_ptr_to_expr(world, t, &value);
    test_assert(expr != NULL);
    test_str(expr, "{n_1: {x: 10, y: 20}}");
    ecs_os_free(expr);

    ecs_fini(world);
}

void Serialize_struct_2_nested_i32_i32(void) {
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

    T value = {{10, 20}, {30, 40}};
    char *expr = ecs_ptr_to_expr(world, t, &value);
    test_assert(expr != NULL);
    test_str(expr, "{n_1: {x: 10, y: 20}, n_2: {x: 30, y: 40}}");
    ecs_os_free(expr);

    ecs_fini(world);
}

void Serialize_struct_i32_array_3(void) {
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
    char *expr = ecs_ptr_to_expr(world, t, &value);
    test_assert(expr != NULL);
    test_str(expr, "{x: [10, 20, 30]}");
    ecs_os_free(expr);

    ecs_fini(world);
}

void Serialize_struct_struct_i32_array_3(void) {
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
    char *expr = ecs_ptr_to_expr(world, t, &value);
    test_assert(expr != NULL);
    test_str(expr, "{n_1: [{x: 10}, {x: 20}, {x: 30}]}");
    ecs_os_free(expr);

    ecs_fini(world);
}

void Serialize_struct_struct_i32_i32_array_3(void) {
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
    char *expr = ecs_ptr_to_expr(world, t, &value);
    test_assert(expr != NULL);
    test_str(expr, "{n_1: [{x: 10, y: 20}, {x: 30, y: 40}, {x: 50, y: 60}]}");
    ecs_os_free(expr);

    ecs_fini(world);
}

void Serialize_struct_w_array_type_i32_i32(void) {
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
    char *expr = ecs_ptr_to_expr(world, t, &value);
    test_assert(expr != NULL);
    test_str(expr, "{n_1: [10, 20]}");
    ecs_os_free(expr);

    ecs_fini(world);
}

void Serialize_struct_w_2_array_type_i32_i32(void) {
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

    T value = { .n_1 = { 10, 20 }, { 30, 40 }};
    char *expr = ecs_ptr_to_expr(world, t, &value);
    test_assert(expr != NULL);
    test_str(expr, "{n_1: [10, 20], n_2: [30, 40]}");
    ecs_os_free(expr);

    ecs_fini(world);
}

void Serialize_struct_w_array_type_struct(void) {
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

    T value = { .n_1 = {{ 10, 20 }, { 30, 40 }}};
    char *expr = ecs_ptr_to_expr(world, t, &value);
    test_assert(expr != NULL);
    test_str(expr, "{n_1: [{x: 10, y: 20}, {x: 30, y: 40}]}");
    ecs_os_free(expr);

    ecs_fini(world);
}

void Serialize_struct_w_2_array_type_struct(void) {
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

    T value = { 
        .n_1 = {{ 10, 20 }, { 30, 40 }}, 
        .n_2 = {{ 50, 60 }, { 70, 80 }} 
    };

    char *expr = ecs_ptr_to_expr(world, t, &value);
    test_assert(expr != NULL);
    test_str(expr, "{n_1: [{x: 10, y: 20}, {x: 30, y: 40}], n_2: [{x: 50, y: 60}, {x: 70, y: 80}]}");
    ecs_os_free(expr);

    ecs_fini(world);
}

void Serialize_struct_partial(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, PositionI);

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_id(PositionI),
        .members = {
            {"y", ecs_id(ecs_i32_t), .offset = offsetof(PositionI, y)}
        }
    });

    PositionI value = {10, 20};
    char *expr = ecs_ptr_to_expr(world, t, &value);
    test_assert(expr != NULL);
    test_str(expr, "{y: 20}");
    ecs_os_free(expr);

    ecs_fini(world);
}

void Serialize_array_i32_3(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_array_init(world, &(ecs_array_desc_t){
        .type = ecs_id(ecs_i32_t),
        .count = 3
    });

    int32_t value[] = {10, 20, 30};
    char *expr = ecs_ptr_to_expr(world, t, value);
    test_assert(expr != NULL);
    test_str(expr, "[10, 20, 30]");
    ecs_os_free(expr);

    ecs_fini(world);
}

void Serialize_array_struct_i32_i32(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, PositionI);

    ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_id(PositionI),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t t = ecs_array_init(world, &(ecs_array_desc_t){
        .type = ecs_id(PositionI),
        .count = 3
    });

    PositionI value[] = {{10, 20}, {30, 40}, {50, 60}};
    char *expr = ecs_ptr_to_expr(world, t, value);
    test_assert(expr != NULL);
    test_str(expr, "[{x: 10, y: 20}, {x: 30, y: 40}, {x: 50, y: 60}]");
    ecs_os_free(expr);

    ecs_fini(world);
}

void Serialize_array_array_i32_3(void) {
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

    char *expr = ecs_ptr_to_expr(world, t, value);
    test_assert(expr != NULL);
    test_str(expr, "[[10, 20], [30, 40], [50, 60]]");
    ecs_os_free(expr);

    ecs_fini(world);
}

void Serialize_vector_i32_3(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_vector_init(world, &(ecs_vector_desc_t){
        .type = ecs_id(ecs_i32_t)
    });

    ecs_vec_t v;
    ecs_vec_init_t(NULL, &v, ecs_i32_t, 3);
    ecs_vec_append_t(NULL, &v, ecs_i32_t)[0] = 10;
    ecs_vec_append_t(NULL, &v, ecs_i32_t)[0] = 20;
    ecs_vec_append_t(NULL, &v, ecs_i32_t)[0] = 30;

    char *expr = ecs_ptr_to_expr(world, t, &v);
    test_assert(expr != NULL);
    test_str(expr, "[10, 20, 30]");
    ecs_os_free(expr);

    ecs_vec_fini_t(NULL, &v, ecs_i32_t);

    ecs_fini(world);
}

void Serialize_vector_struct_i32_i32(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, PositionI);

    ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_id(PositionI),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t t = ecs_vector_init(world, &(ecs_vector_desc_t){
        .type = ecs_id(PositionI)
    });

    ecs_vec_t v;
    ecs_vec_init_t(NULL, &v, PositionI, 3);
    ecs_vec_append_t(NULL, &v, PositionI)[0] = (PositionI){10, 20};
    ecs_vec_append_t(NULL, &v, PositionI)[0] = (PositionI){30, 40};
    ecs_vec_append_t(NULL, &v, PositionI)[0] = (PositionI){50, 60};

    char *expr = ecs_ptr_to_expr(world, t, &v);
    test_assert(expr != NULL);
    test_str(expr, "[{x: 10, y: 20}, {x: 30, y: 40}, {x: 50, y: 60}]");
    ecs_os_free(expr);

    ecs_vec_fini_t(NULL, &v, PositionI);

    ecs_fini(world);
}

void Serialize_vector_array_i32_3(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t a = ecs_array_init(world, &(ecs_array_desc_t){
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

    char *expr = ecs_ptr_to_expr(world, t, &v);
    test_assert(expr != NULL);
    test_str(expr, "[[10, 20], [30, 40], [50, 60]]");
    ecs_os_free(expr);

    ecs_vec_fini_t(NULL, &v, IntArray);

    ecs_fini(world);
}

void Serialize_escape_simple_string(void) {
    ecs_world_t *world = ecs_init();

    char *str = flecs_astresc('"', "Hello World");
    test_str(str, "Hello World");
    ecs_os_free(str);

    ecs_fini(world);
}

void Serialize_escape_newline(void) {
    ecs_world_t *world = ecs_init();

    char *str = flecs_astresc('"', "\n");
    test_str(str, "\\n");
    ecs_os_free(str);

    ecs_fini(world);
}

void Serialize_escape_2_newlines(void) {
    ecs_world_t *world = ecs_init();

    char *str = flecs_astresc('"', "\n\n");
    test_str(str, "\\n\\n");
    ecs_os_free(str);

    ecs_fini(world);
}

void Serialize_escape_string_w_trailing_newline(void) {
    ecs_world_t *world = ecs_init();

    char *str = flecs_astresc('"', "Hello World\n");
    test_str(str, "Hello World\\n");
    ecs_os_free(str);

    ecs_fini(world);
}

void Serialize_escape_string_w_2_trailing_newlines(void) {
    ecs_world_t *world = ecs_init();

    char *str = flecs_astresc('"', "Hello World\n\n");
    test_str(str, "Hello World\\n\\n");
    ecs_os_free(str);

    ecs_fini(world);
}

void Serialize_escape_string_w_delim(void) {
    ecs_world_t *world = ecs_init();

    char *str = flecs_astresc('"', "\"Hello World\"");
    test_str(str, "\\\"Hello World\\\"");
    ecs_os_free(str);

    ecs_fini(world);
}
