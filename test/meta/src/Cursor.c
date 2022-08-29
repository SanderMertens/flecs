#include <meta.h>

void Cursor_set_bool() {
    ecs_world_t *world = ecs_init();

    ecs_bool_t value = false;

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, ecs_id(ecs_bool_t), &value);
    test_ok( ecs_meta_set_bool(&cur, true) );

    test_bool(value, true);

    ecs_fini(world);
}

void Cursor_set_byte() {
    ecs_world_t *world = ecs_init();

    ecs_byte_t value = 10;

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, ecs_id(ecs_byte_t), &value);
    test_ok( ecs_meta_set_uint(&cur, 20) );

    test_uint(value, 20);

    ecs_fini(world);
}

void Cursor_set_char() {
    ecs_world_t *world = ecs_init();

    ecs_char_t value = 10;

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, ecs_id(ecs_char_t), &value);
    test_ok( ecs_meta_set_int(&cur, 20) );

    test_int(value, 20);

    ecs_fini(world);
}

void Cursor_set_i8() {
    ecs_world_t *world = ecs_init();

    ecs_i8_t value = 10;

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, ecs_id(ecs_i8_t), &value);
    test_ok( ecs_meta_set_int(&cur, 20) );

    test_int(value, 20);

    ecs_fini(world);
}

void Cursor_set_i16() {
    ecs_world_t *world = ecs_init();

    ecs_i16_t value = 10;

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, ecs_id(ecs_i16_t), &value);
    test_ok( ecs_meta_set_int(&cur, 20) );

    test_int(value, 20);

    ecs_fini(world);
}

void Cursor_set_i32() {
    ecs_world_t *world = ecs_init();

    ecs_i32_t value = 10;

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, ecs_id(ecs_i32_t), &value);
    test_ok( ecs_meta_set_int(&cur, 20) );

    test_int(value, 20);

    ecs_fini(world);
}

void Cursor_set_i64() {
    ecs_world_t *world = ecs_init();

    ecs_i64_t value = 10;

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, ecs_id(ecs_i64_t), &value);
    test_ok( ecs_meta_set_int(&cur, 20) );

    test_int(value, 20);

    ecs_fini(world);
}

void Cursor_set_iptr() {
    ecs_world_t *world = ecs_init();

    ecs_iptr_t value = 10;

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, ecs_id(ecs_iptr_t), &value);
    test_ok( ecs_meta_set_int(&cur, 20) );

    test_int(value, 20);

    ecs_fini(world);
}

void Cursor_set_u8() {
    ecs_world_t *world = ecs_init();

    ecs_u8_t value = 10;

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, ecs_id(ecs_u8_t), &value);
    test_ok( ecs_meta_set_uint(&cur, 20) );

    test_uint(value, 20);

    ecs_fini(world);
}

void Cursor_set_u16() {
    ecs_world_t *world = ecs_init();

    ecs_u16_t value = 10;

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, ecs_id(ecs_u8_t), &value);
    test_ok( ecs_meta_set_uint(&cur, 20) );

    test_uint(value, 20);

    ecs_fini(world);
}

void Cursor_set_u32() {
    ecs_world_t *world = ecs_init();

    ecs_u32_t value = 10;

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, ecs_id(ecs_u32_t), &value);
    test_ok( ecs_meta_set_uint(&cur, 20) );

    test_uint(value, 20);

    ecs_fini(world);
}

void Cursor_set_u64() {
    ecs_world_t *world = ecs_init();

    ecs_u64_t value = 10;

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, ecs_id(ecs_u64_t), &value);
    test_ok( ecs_meta_set_uint(&cur, 20) );

    test_uint(value, 20);

    ecs_fini(world);
}

void Cursor_set_uptr() {
    ecs_world_t *world = ecs_init();

    ecs_uptr_t value = 10;

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, ecs_id(ecs_uptr_t), &value);
    test_ok( ecs_meta_set_uint(&cur, 20) );

    test_uint(value, 20);

    ecs_fini(world);
}

void Cursor_set_float() {
    ecs_world_t *world = ecs_init();

    ecs_f32_t value = 10;

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, ecs_id(ecs_f32_t), &value);
    test_ok( ecs_meta_set_float(&cur, 20.5) );

    test_flt(value, 20.5);

    ecs_fini(world);
}

void Cursor_set_double() {
    ecs_world_t *world = ecs_init();

    ecs_f64_t value = 10;

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, ecs_id(ecs_f64_t), &value);
    test_ok( ecs_meta_set_float(&cur, 20.5) );

    test_flt(value, 20.5);

    ecs_fini(world);
}

void Cursor_set_string() {
    ecs_world_t *world = ecs_init();

    char *value = NULL;

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, ecs_id(ecs_string_t), &value);
    test_ok( ecs_meta_set_string(&cur, "HelloWorld") );

    test_str(value, "HelloWorld");
    ecs_os_free(value);

    ecs_fini(world);
}

void Cursor_set_string_literal() {
    ecs_world_t *world = ecs_init();

    char *value = NULL;

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, ecs_id(ecs_string_t), &value);
    test_ok( ecs_meta_set_string_literal(&cur, "\"HelloWorld\"") );

    test_str(value, "HelloWorld");
    ecs_os_free(value);

    ecs_fini(world);
}

void Cursor_set_string_to_null() {
    ecs_world_t *world = ecs_init();

    char *value = ecs_os_strdup("HelloWorld");

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, ecs_id(ecs_string_t), &value);
    test_ok( ecs_meta_set_null(&cur) );

    test_str(value, NULL);
    ecs_os_free(value);

    ecs_fini(world);
}

void Cursor_set_entity() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t value = 0;

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, ecs_id(ecs_entity_t), &value);
    test_ok( ecs_meta_set_entity(&cur, EcsFlecs) );

    test_uint(value, EcsFlecs);

    ecs_fini(world);
}

void Cursor_set_entity_to_number() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t value = 0;

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, ecs_id(ecs_entity_t), &value);
    test_ok( ecs_meta_set_uint(&cur, 500) );

    test_uint(value, 500);

    ecs_fini(world);
}

void Cursor_set_entity_to_0() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t value = 0;

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, ecs_id(ecs_entity_t), &value);
    test_ok( ecs_meta_set_uint(&cur, 0) );

    test_uint(value, 0);

    ecs_fini(world);
}

void Cursor_set_enum() {
    ecs_world_t *world = ecs_init();

    typedef enum {
        Red, Green, Blue
    } Color;

    ecs_entity_t t = ecs_enum(world, {
        .constants = {
            { "Red" },
            { "Green" },
            { "Blue"}
        }
    });
    test_assert(t != 0);

    Color value = 0;
    ecs_meta_cursor_t cur = ecs_meta_cursor(world, t, &value);
    test_ok( ecs_meta_set_int(&cur, Green) );

    test_int(value, Green);

    ecs_fini(world);
}

void Cursor_set_bitmask() {
    ecs_world_t *world = ecs_init();

    uint32_t Bacon = 1;
    uint32_t Lettuce = 2;

    ecs_entity_t t = ecs_bitmask(world, {
        .constants = {
            { "Bacon" },
            { "Lettuce" }
        }
    });
    test_assert(t != 0);

    uint32_t value = 0;
    ecs_meta_cursor_t cur = ecs_meta_cursor(world, t, &value);
    test_ok( ecs_meta_set_uint(&cur, Bacon | Lettuce) );

    test_uint(value, Bacon | Lettuce);

    ecs_fini(world);
}

void Cursor_set_signed_as_unsigned() {
    ecs_world_t *world = ecs_init();

    int32_t value = 0;

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, ecs_id(ecs_i32_t), &value);
    test_ok( ecs_meta_set_uint(&cur, 10) );

    test_int(value, 10);

    ecs_fini(world);
}

void Cursor_set_unsigned_as_signed() {
    ecs_world_t *world = ecs_init();

    uint32_t value = 0;

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, ecs_id(ecs_u32_t), &value);
    test_ok( ecs_meta_set_int(&cur, 10) );

    test_int(value, 10);

    ecs_fini(world);
}

void Cursor_set_signed_as_unsigned_out_of_range() {
    ecs_log_set_level(-4);
    ecs_world_t *world = ecs_init();

    int8_t value = 0;

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, ecs_id(ecs_i8_t), &value);
    test_fail( ecs_meta_set_uint(&cur, 128) );

    test_int(value, 0);

    ecs_fini(world);
}

void Cursor_set_unsigned_as_signed_out_of_range() {
    ecs_log_set_level(-4);
    ecs_world_t *world = ecs_init();

    uint32_t value = 0;

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, ecs_id(ecs_u32_t), &value);
    test_fail( ecs_meta_set_int(&cur, -10) );

    test_int(value, 0);

    ecs_fini(world);
}

void Cursor_set_string_to_null_as_signed() {
    ecs_world_t *world = ecs_init();

    char *value = ecs_os_strdup("Hello");

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, ecs_id(ecs_string_t), &value);
    test_ok( ecs_meta_set_int(&cur, 0) );

    test_str(value, 0);

    ecs_fini(world);
}

void Cursor_set_string_to_null_as_unsigned() {
    ecs_world_t *world = ecs_init();

    char *value = ecs_os_strdup("Hello");

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, ecs_id(ecs_string_t), &value);
    test_ok( ecs_meta_set_uint(&cur, 0) );

    test_str(value, 0);

    ecs_fini(world);
}

void Cursor_set_entity_as_signed() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t value = 0;
    ecs_entity_t e = ecs_new_id(world);

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, ecs_id(ecs_entity_t), &value);
    test_ok( ecs_meta_set_int(&cur, (int64_t)e) );

    test_uint(value, e);

    ecs_fini(world);
}

void Cursor_set_entity_as_unsigned() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t value = 0;
    ecs_entity_t e = ecs_new_id(world);

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, ecs_id(ecs_entity_t), &value);
    test_ok( ecs_meta_set_uint(&cur, e) );

    test_uint(value, e);

    ecs_fini(world);
}

void Cursor_set_entity_as_signed_out_of_range() {
    ecs_log_set_level(-4);
    ecs_world_t *world = ecs_init();

    ecs_entity_t value = 0;

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, ecs_id(ecs_entity_t), &value);
    test_fail( ecs_meta_set_int(&cur, -10) );

    test_uint(value, 0);

    ecs_fini(world);
}

void Cursor_set_str_to_bool() {
    ecs_world_t *world = ecs_init();

    ecs_bool_t value = false;

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, ecs_id(ecs_bool_t), &value);
    test_ok( ecs_meta_set_string(&cur, "true") );

    test_bool(value, true);

    cur = ecs_meta_cursor(world, ecs_id(ecs_bool_t), &value);
    test_ok( ecs_meta_set_string(&cur, "false") );

    test_bool(value, false);

    ecs_fini(world);
}

void Cursor_set_str_to_char() {
    ecs_world_t *world = ecs_init();

    ecs_char_t value = false;

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, ecs_id(ecs_char_t), &value);
    test_ok( ecs_meta_set_string(&cur, "10") );

    test_bool(value, 10);

    ecs_fini(world);
}

void Cursor_set_str_literal_to_char() {
    ecs_world_t *world = ecs_init();

    ecs_char_t value = false;

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, ecs_id(ecs_char_t), &value);
    test_ok( ecs_meta_set_string_literal(&cur, "\"a\"") );

    test_int(value, 'a');

    ecs_fini(world);
}

void Cursor_set_str_to_i8() {
    ecs_world_t *world = ecs_init();

    ecs_i8_t value = 10;

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, ecs_id(ecs_i8_t), &value);
    test_ok( ecs_meta_set_string(&cur, "20") );

    test_int(value, 20);

    ecs_fini(world);
}

void Cursor_set_str_to_i16() {
    ecs_world_t *world = ecs_init();

    ecs_i16_t value = 10;

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, ecs_id(ecs_i16_t), &value);
    test_ok( ecs_meta_set_string(&cur, "20") );

    test_int(value, 20);

    ecs_fini(world);
}

void Cursor_set_str_to_i32() {
    ecs_world_t *world = ecs_init();

    ecs_i32_t value = 10;

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, ecs_id(ecs_i32_t), &value);
    test_ok( ecs_meta_set_string(&cur, "20") );

    test_int(value, 20);

    ecs_fini(world);
}

void Cursor_set_str_to_i64() {
    ecs_world_t *world = ecs_init();

    ecs_i64_t value = 10;

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, ecs_id(ecs_i64_t), &value);
    test_ok( ecs_meta_set_string(&cur, "20") );

    test_int(value, 20);

    ecs_fini(world);
}

void Cursor_set_str_to_f32() {
    ecs_world_t *world = ecs_init();

    ecs_f32_t value = 10.5;

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, ecs_id(ecs_f32_t), &value);
    test_ok( ecs_meta_set_string(&cur, "20.5") );

    test_flt(value, 20.5);

    ecs_fini(world);
}

void Cursor_set_str_to_f64() {
    ecs_world_t *world = ecs_init();

    ecs_f64_t value = 10.5;

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, ecs_id(ecs_f64_t), &value);
    test_ok( ecs_meta_set_string(&cur, "20.5") );

    test_flt(value, 20.5);

    ecs_fini(world);
}

void Cursor_set_str_to_entity() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t value = 0;

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, ecs_id(ecs_entity_t), &value);
    test_ok( ecs_meta_set_string(&cur, "flecs.core") );

    test_uint(value, EcsFlecsCore);

    ecs_fini(world);
}

void Cursor_set_str_to_invalid_bool() {
    ecs_log_set_level(-4);

    ecs_world_t *world = ecs_init();

    ecs_bool_t value = false;

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, ecs_id(ecs_bool_t), &value);
    test_fail( ecs_meta_set_string(&cur, "foo") );

    test_bool(value, false);

    ecs_fini(world);
}

void Cursor_set_str_to_invalid_entity() {
    ecs_log_set_level(-4);

    ecs_world_t *world = ecs_init();

    ecs_entity_t value = 0;

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, ecs_id(ecs_entity_t), &value);
    test_fail( ecs_meta_set_string(&cur, "flops.core") );

    test_uint(value, 0);

    ecs_fini(world);
}

void Cursor_struct_set_i32() {
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

    test_assert(t != 0);

    T value;

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, t, &value);
    test_ok( ecs_meta_push(&cur) );
    test_ok( ecs_meta_set_int(&cur, 10) );

    test_int(value.x, 10);

    ecs_fini(world);
}

void Cursor_struct_set_i32_i32() {
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

    T value;

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, t, &value);
    test_ok( ecs_meta_push(&cur) );
    test_ok( ecs_meta_set_int(&cur, 10) );
    test_ok( ecs_meta_next(&cur) );
    test_ok( ecs_meta_set_int(&cur, 20) );
    test_ok( ecs_meta_pop(&cur) );

    test_int(value.x, 10);
    test_int(value.y, 20);

    ecs_fini(world);
}

void Cursor_struct_set_i32_i32_i32() {
    typedef struct {
        ecs_i32_t x;
        ecs_i32_t y;
        ecs_i32_t z;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "T"}),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)},
            {"z", ecs_id(ecs_i32_t)},
        }
    });

    test_assert(t != 0);

    T value;

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, t, &value);
    test_ok( ecs_meta_push(&cur) );
    test_ok( ecs_meta_set_int(&cur, 10) );
    test_ok( ecs_meta_next(&cur) );
    test_ok( ecs_meta_set_int(&cur, 20) );
    test_ok( ecs_meta_next(&cur) );
    test_ok( ecs_meta_set_int(&cur, 30) );
    test_ok( ecs_meta_pop(&cur) );

    test_int(value.x, 10);
    test_int(value.y, 20);
    test_int(value.z, 30);

    ecs_fini(world);
}

void Cursor_struct_set_bool() {
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

    test_assert(t != 0);

    T value;

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, t, &value);
    test_ok( ecs_meta_push(&cur) );
    test_ok( ecs_meta_set_bool(&cur, true) );

    test_bool(value.x, true);

    ecs_fini(world);
}

void Cursor_struct_set_bool_bool() {
    typedef struct {
        ecs_bool_t x;
        ecs_bool_t y;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "T"}),
        .members = {
            {"x", ecs_id(ecs_bool_t)},
            {"y", ecs_id(ecs_bool_t)}
        }
    });

    test_assert(t != 0);

    T value;

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, t, &value);
    test_ok( ecs_meta_push(&cur) );
    test_ok( ecs_meta_set_bool(&cur, true) );
    test_ok( ecs_meta_next(&cur) );
    test_ok( ecs_meta_set_bool(&cur, false) );
    test_ok( ecs_meta_pop(&cur) );

    test_bool(value.x, true);
    test_bool(value.y, false);

    cur = ecs_meta_cursor(world, t, &value);
    test_ok( ecs_meta_push(&cur) );
    test_ok( ecs_meta_set_bool(&cur, false) );
    test_ok( ecs_meta_next(&cur) );
    test_ok( ecs_meta_set_bool(&cur, true) );
    test_ok( ecs_meta_pop(&cur) );

    test_bool(value.x, false);
    test_bool(value.y, true);

    ecs_fini(world);
}

void Cursor_struct_set_bool_bool_bool() {
    typedef struct {
        ecs_bool_t x;
        ecs_bool_t y;
        ecs_bool_t z;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "T"}),
        .members = {
            {"x", ecs_id(ecs_bool_t)},
            {"y", ecs_id(ecs_bool_t)},
            {"z", ecs_id(ecs_bool_t)}
        }
    });

    test_assert(t != 0);

    T value;

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, t, &value);
    test_ok( ecs_meta_push(&cur) );
    test_ok( ecs_meta_set_bool(&cur, true) );
    test_ok( ecs_meta_next(&cur) );
    test_ok( ecs_meta_set_bool(&cur, false) );
    test_ok( ecs_meta_next(&cur) );
    test_ok( ecs_meta_set_bool(&cur, true) );
    test_ok( ecs_meta_pop(&cur) );

    test_bool(value.x, true);
    test_bool(value.y, false);
    test_bool(value.z, true);

    ecs_fini(world);
}

void Cursor_struct_set_bool_bool_i32_bool() {
    typedef struct {
        ecs_bool_t x;
        ecs_bool_t y;
        ecs_i32_t z;
        ecs_bool_t w;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "T"}),
        .members = {
            {"x", ecs_id(ecs_bool_t)},
            {"y", ecs_id(ecs_bool_t)},
            {"z", ecs_id(ecs_i32_t)},
            {"w", ecs_id(ecs_bool_t)},
        }
    });

    test_assert(t != 0);

    T value;

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, t, &value);
    test_ok( ecs_meta_push(&cur) );
    test_ok( ecs_meta_set_bool(&cur, true) );
    test_ok( ecs_meta_next(&cur) );
    test_ok( ecs_meta_set_bool(&cur, false) );
    test_ok( ecs_meta_next(&cur) );
    test_ok( ecs_meta_set_int(&cur, 10) );
    test_ok( ecs_meta_next(&cur) );
    test_ok( ecs_meta_set_bool(&cur, true) );
    test_ok( ecs_meta_pop(&cur) );

    test_bool(value.x, true);
    test_bool(value.y, false);
    test_int(value.z, 10);
    test_bool(value.w, true);

    ecs_fini(world);
}

void Cursor_struct_nested_i32() {
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

    test_assert(n1 != 0);

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "T"}),
        .members = {
            {"n_1", n1}
        }
    });

    test_assert(t != 0);

    T value;

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, t, &value);
    test_ok( ecs_meta_push(&cur) );
    test_ok( ecs_meta_push(&cur) );
    test_ok( ecs_meta_set_int(&cur, 10) );
    test_ok( ecs_meta_pop(&cur) );
    test_ok( ecs_meta_pop(&cur) );

    test_int(value.n_1.x, 10);

    ecs_fini(world);
}

void Cursor_struct_2_nested_1_i32_2_i32() {
    typedef struct {
        ecs_i32_t x;
    } N1;

    typedef struct {
        N1 n_1;
        N1 n_2;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t n1 = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "N1"}),
        .members = {
            {"x", ecs_id(ecs_i32_t)}
        }
    });

    test_assert(n1 != 0);

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "T"}),
        .members = {
            {"n_1", n1},
            {"n_2", n1}
        }
    });

    test_assert(t != 0);

    T value;

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, t, &value);
    test_ok( ecs_meta_push(&cur) );
    test_ok( ecs_meta_push(&cur) );
    test_ok( ecs_meta_set_int(&cur, 10) );
    test_ok( ecs_meta_pop(&cur) );
    test_ok( ecs_meta_next(&cur) );
    test_ok( ecs_meta_push(&cur) );
    test_ok( ecs_meta_set_int(&cur, 20) );
    test_ok( ecs_meta_pop(&cur) );
    test_ok( ecs_meta_pop(&cur) );

    test_int(value.n_1.x, 10);
    test_int(value.n_2.x, 20);

    ecs_fini(world);
}

void Cursor_struct_2_nested_1_i32_i32_2_i32_i32() {
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

    test_assert(n1 != 0);

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "T"}),
        .members = {
            {"n_1", n1},
            {"n_2", n1}
        }
    });

    test_assert(t != 0);

    T value;

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, t, &value);
    test_ok( ecs_meta_push(&cur) );
    test_ok( ecs_meta_push(&cur) );
    test_ok( ecs_meta_set_int(&cur, 10) );
    test_ok( ecs_meta_next(&cur) );
    test_ok( ecs_meta_set_int(&cur, 20) );
    test_ok( ecs_meta_pop(&cur) );
    test_ok( ecs_meta_next(&cur) );
    test_ok( ecs_meta_push(&cur) );
    test_ok( ecs_meta_set_int(&cur, 30) );
    test_ok( ecs_meta_next(&cur) );
    test_ok( ecs_meta_set_int(&cur, 40) );
    test_ok( ecs_meta_pop(&cur) );
    test_ok( ecs_meta_pop(&cur) );

    test_int(value.n_1.x, 10);
    test_int(value.n_1.y, 20);
    test_int(value.n_2.x, 30);
    test_int(value.n_2.y, 40);

    ecs_fini(world);
}

void Cursor_struct_2_nested_1_i32_i32_2_i32_i32_w_padding_members() {
    typedef struct {
        ecs_i32_t x;
        ecs_i32_t y;
    } N1;

    typedef struct {
        bool p1;
        N1 n_1;
        bool p2;
        N1 n_2;
        bool p3;
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

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "T"}),
        .members = {
            {"p1", ecs_id(ecs_bool_t)},
            {"n_1", n1},
            {"p2", ecs_id(ecs_bool_t)},
            {"n_2", n1},
            {"p3", ecs_id(ecs_bool_t)},
        }
    });

    test_assert(t != 0);

    T value;

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, t, &value);
    test_ok( ecs_meta_push(&cur) );
    test_ok( ecs_meta_set_bool(&cur, true) );
    test_ok( ecs_meta_next(&cur) );
    test_ok( ecs_meta_push(&cur) );
    test_ok( ecs_meta_set_int(&cur, 10) );
    test_ok( ecs_meta_next(&cur) );
    test_ok( ecs_meta_set_int(&cur, 20) );
    test_ok( ecs_meta_pop(&cur) );
    test_ok( ecs_meta_next(&cur) );
    test_ok( ecs_meta_set_bool(&cur, true) );
    test_ok( ecs_meta_next(&cur) );
    test_ok( ecs_meta_push(&cur) );
    test_ok( ecs_meta_set_int(&cur, 30) );
    test_ok( ecs_meta_next(&cur) );
    test_ok( ecs_meta_set_int(&cur, 40) );
    test_ok( ecs_meta_pop(&cur) );
    test_ok( ecs_meta_next(&cur) );
    test_ok( ecs_meta_set_bool(&cur, true) );
    test_ok( ecs_meta_pop(&cur) );

    test_bool(value.p1, true);
    test_bool(value.p2, true);
    test_bool(value.p3, true);

    test_int(value.n_1.x, 10);
    test_int(value.n_1.y, 20);
    test_int(value.n_2.x, 30);
    test_int(value.n_2.y, 40);

    ecs_fini(world);
}

void Cursor_struct_set_array_i32_3() {
    typedef struct {
        ecs_i32_t x[3];
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "T"}),
        .members = {
            {"x", ecs_id(ecs_i32_t), 3}
        }
    });

    test_assert(t != 0);

    T value;

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, t, &value);
    test_ok( ecs_meta_push(&cur) );
        test_ok( ecs_meta_push(&cur) );
            test_ok( ecs_meta_set_int(&cur, 10) );
            test_ok( ecs_meta_next(&cur) );
            test_ok( ecs_meta_set_int(&cur, 20) );
            test_ok( ecs_meta_next(&cur) );
            test_ok( ecs_meta_set_int(&cur, 30) );
        test_ok( ecs_meta_pop(&cur) );
    test_ok( ecs_meta_pop(&cur) );

    test_int(value.x[0], 10);
    test_int(value.x[1], 20);
    test_int(value.x[2], 30);

    ecs_fini(world);
}

void Cursor_struct_set_2_array_i32_3() {
    typedef struct {
        ecs_i32_t x[3];
        ecs_i32_t y[3];
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "T"}),
        .members = {
            {"x", ecs_id(ecs_i32_t), 3},
            {"y", ecs_id(ecs_i32_t), 3}
        }
    });

    test_assert(t != 0);

    T value;

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, t, &value);
    test_ok( ecs_meta_push(&cur) );
        test_ok( ecs_meta_push(&cur) );
            test_ok( ecs_meta_set_int(&cur, 10) );
            test_ok( ecs_meta_next(&cur) );
            test_ok( ecs_meta_set_int(&cur, 20) );
            test_ok( ecs_meta_next(&cur) );
            test_ok( ecs_meta_set_int(&cur, 30) );
        test_ok( ecs_meta_pop(&cur) );

        test_ok( ecs_meta_next(&cur) );

        test_ok( ecs_meta_push(&cur) );
            test_ok( ecs_meta_set_int(&cur, 40) );
            test_ok( ecs_meta_next(&cur) );
            test_ok( ecs_meta_set_int(&cur, 50) );
            test_ok( ecs_meta_next(&cur) );
            test_ok( ecs_meta_set_int(&cur, 60) );
        test_ok( ecs_meta_pop(&cur) );
    test_ok( ecs_meta_pop(&cur) );

    test_int(value.x[0], 10);
    test_int(value.x[1], 20);
    test_int(value.x[2], 30);

    test_int(value.y[0], 40);
    test_int(value.y[1], 50);
    test_int(value.y[2], 60);

    ecs_fini(world);
}

void Cursor_struct_set_2_array_i32_3_w_padding_members() {
    typedef struct {
        ecs_bool_t p1;
        ecs_i32_t x[3];
        ecs_bool_t p2;
        ecs_i32_t y[3];
        ecs_bool_t p3;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "T"}),
        .members = {
            {"p1", ecs_id(ecs_bool_t)},
            {"x", ecs_id(ecs_i32_t), 3},
            {"p2", ecs_id(ecs_bool_t)},
            {"y", ecs_id(ecs_i32_t), 3},
            {"p3", ecs_id(ecs_bool_t)}
        }
    });

    test_assert(t != 0);

    T value;

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, t, &value);
    test_ok( ecs_meta_push(&cur) );
        test_ok( ecs_meta_set_bool(&cur, true) );
        test_ok( ecs_meta_next(&cur) );

        test_ok( ecs_meta_push(&cur) );
            test_ok( ecs_meta_set_int(&cur, 10) );
            test_ok( ecs_meta_next(&cur) );
            test_ok( ecs_meta_set_int(&cur, 20) );
            test_ok( ecs_meta_next(&cur) );
            test_ok( ecs_meta_set_int(&cur, 30) );
        test_ok( ecs_meta_pop(&cur) );

        test_ok( ecs_meta_next(&cur) );
        test_ok( ecs_meta_set_bool(&cur, true) );
        test_ok( ecs_meta_next(&cur) );

        test_ok( ecs_meta_push(&cur) );
            test_ok( ecs_meta_set_int(&cur, 40) );
            test_ok( ecs_meta_next(&cur) );
            test_ok( ecs_meta_set_int(&cur, 50) );
            test_ok( ecs_meta_next(&cur) );
            test_ok( ecs_meta_set_int(&cur, 60) );
        test_ok( ecs_meta_pop(&cur) );

        test_ok( ecs_meta_next(&cur) );
        test_ok( ecs_meta_set_bool(&cur, true) );
    test_ok( ecs_meta_pop(&cur) );

    test_bool(value.p1, true);
    test_bool(value.p2, true);
    test_bool(value.p3, true);

    test_int(value.x[0], 10);
    test_int(value.x[1], 20);
    test_int(value.x[2], 30);

    test_int(value.y[0], 40);
    test_int(value.y[1], 50);
    test_int(value.y[2], 60);

    ecs_fini(world);
}

void Cursor_struct_set_array_w_struct_i32() {
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

    test_assert(t != 0);

    T value;

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, t, &value);
    test_ok( ecs_meta_push(&cur) );
        test_ok( ecs_meta_push(&cur) );
            test_ok( ecs_meta_push(&cur) );
                test_ok( ecs_meta_set_int(&cur, 10) );
            test_ok( ecs_meta_pop(&cur) );
            test_ok( ecs_meta_next(&cur) );
            test_ok( ecs_meta_push(&cur) );
                test_ok( ecs_meta_set_int(&cur, 20) );
            test_ok( ecs_meta_pop(&cur) );
            test_ok( ecs_meta_next(&cur) );
            test_ok( ecs_meta_push(&cur) );
                test_ok( ecs_meta_set_int(&cur, 30) );
            test_ok( ecs_meta_pop(&cur) );
        test_ok( ecs_meta_pop(&cur) );
    test_ok( ecs_meta_pop(&cur) );

    test_int(value.n_1[0].x, 10);
    test_int(value.n_1[1].x, 20);
    test_int(value.n_1[2].x, 30);

    ecs_fini(world);
}

void Cursor_struct_set_2_array_w_struct_i32() {
    typedef struct {
        ecs_i32_t x;
    } N1;

    typedef struct {
        N1 n_1[3];
        N1 n_2[3];
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
            {"n_1", n1, 3},
            {"n_2", n1, 3}
        }
    });

    test_assert(t != 0);

    T value;

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, t, &value);
    test_ok( ecs_meta_push(&cur) );
        test_ok( ecs_meta_push(&cur) );
            test_ok( ecs_meta_push(&cur) );
                test_ok( ecs_meta_set_int(&cur, 10) );
            test_ok( ecs_meta_pop(&cur) );
            test_ok( ecs_meta_next(&cur) );
            test_ok( ecs_meta_push(&cur) );
                test_ok( ecs_meta_set_int(&cur, 20) );
            test_ok( ecs_meta_pop(&cur) );
            test_ok( ecs_meta_next(&cur) );
            test_ok( ecs_meta_push(&cur) );
                test_ok( ecs_meta_set_int(&cur, 30) );
            test_ok( ecs_meta_pop(&cur) );
        test_ok( ecs_meta_pop(&cur) );

        test_ok( ecs_meta_next(&cur) );

        test_ok( ecs_meta_push(&cur) );
            test_ok( ecs_meta_push(&cur) );
                test_ok( ecs_meta_set_int(&cur, 40) );
            test_ok( ecs_meta_pop(&cur) );
            test_ok( ecs_meta_next(&cur) );
            test_ok( ecs_meta_push(&cur) );
                test_ok( ecs_meta_set_int(&cur, 50) );
            test_ok( ecs_meta_pop(&cur) );
            test_ok( ecs_meta_next(&cur) );
            test_ok( ecs_meta_push(&cur) );
                test_ok( ecs_meta_set_int(&cur, 60) );
            test_ok( ecs_meta_pop(&cur) );
        test_ok( ecs_meta_pop(&cur) );
    test_ok( ecs_meta_pop(&cur) );

    test_int(value.n_1[0].x, 10);
    test_int(value.n_1[1].x, 20);
    test_int(value.n_1[2].x, 30);

    test_int(value.n_2[0].x, 40);
    test_int(value.n_2[1].x, 50);
    test_int(value.n_2[2].x, 60);

    ecs_fini(world);
}

void Cursor_struct_set_array_w_struct_w_array_i32() {
    typedef struct {
        ecs_i32_t x[3];
    } N1;

    typedef struct {
        N1 n_1[3];
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t n1 = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "N1"}),
        .members = {
            {"x", ecs_id(ecs_i32_t), 3}
        }
    });

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "T"}),
        .members = {
            {"n_1", n1, 3}
        }
    });

    test_assert(t != 0);

    T value;

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, t, &value);
    test_ok( ecs_meta_push(&cur) );
        test_ok( ecs_meta_push(&cur) );
            test_ok( ecs_meta_push(&cur) );
                test_ok( ecs_meta_push(&cur) );
                    test_ok( ecs_meta_set_int(&cur, 10) );
                    test_ok( ecs_meta_next(&cur) );
                    test_ok( ecs_meta_set_int(&cur, 11) );
                    test_ok( ecs_meta_next(&cur) );
                    test_ok( ecs_meta_set_int(&cur, 12) );
                test_ok( ecs_meta_pop(&cur) );
            test_ok( ecs_meta_pop(&cur) );

            test_ok( ecs_meta_next(&cur) );

            test_ok( ecs_meta_push(&cur) );
                test_ok( ecs_meta_push(&cur) );
                    test_ok( ecs_meta_set_int(&cur, 20) );
                    test_ok( ecs_meta_next(&cur) );
                    test_ok( ecs_meta_set_int(&cur, 21) );
                    test_ok( ecs_meta_next(&cur) );
                    test_ok( ecs_meta_set_int(&cur, 22) );
                test_ok( ecs_meta_pop(&cur) );
            test_ok( ecs_meta_pop(&cur) );

            test_ok( ecs_meta_next(&cur) );

            test_ok( ecs_meta_push(&cur) );
                test_ok( ecs_meta_push(&cur) );
                    test_ok( ecs_meta_set_int(&cur, 30) );
                    test_ok( ecs_meta_next(&cur) );
                    test_ok( ecs_meta_set_int(&cur, 31) );
                    test_ok( ecs_meta_next(&cur) );
                    test_ok( ecs_meta_set_int(&cur, 32) );
                test_ok( ecs_meta_pop(&cur) );
            test_ok( ecs_meta_pop(&cur) );
        test_ok( ecs_meta_pop(&cur) );
    test_ok( ecs_meta_pop(&cur) );

    test_int(value.n_1[0].x[0], 10);
    test_int(value.n_1[0].x[1], 11);
    test_int(value.n_1[0].x[2], 12);

    test_int(value.n_1[1].x[0], 20);
    test_int(value.n_1[1].x[1], 21);
    test_int(value.n_1[1].x[2], 22);

    test_int(value.n_1[2].x[0], 30);
    test_int(value.n_1[2].x[1], 31);
    test_int(value.n_1[2].x[2], 32);

    ecs_fini(world);
}

void Cursor_struct_set_2_array_w_struct_w_array_i32() {
    typedef struct {
        ecs_i32_t x[3];
    } N1;

    typedef struct {
        N1 n_1[3];
        N1 n_2[3];
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t n1 = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "N1"}),
        .members = {
            {"x", ecs_id(ecs_i32_t), 3}
        }
    });

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "T"}),
        .members = {
            {"n_1", n1, 3},
            {"n_2", n1, 3}
        }
    });

    test_assert(t != 0);

    T value;

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, t, &value);
    test_ok( ecs_meta_push(&cur) );
        test_ok( ecs_meta_push(&cur) );
            test_ok( ecs_meta_push(&cur) );
                test_ok( ecs_meta_push(&cur) );
                    test_ok( ecs_meta_set_int(&cur, 10) );
                    test_ok( ecs_meta_next(&cur) );
                    test_ok( ecs_meta_set_int(&cur, 11) );
                    test_ok( ecs_meta_next(&cur) );
                    test_ok( ecs_meta_set_int(&cur, 12) );
                test_ok( ecs_meta_pop(&cur) );
            test_ok( ecs_meta_pop(&cur) );

            test_ok( ecs_meta_next(&cur) );

            test_ok( ecs_meta_push(&cur) );
                test_ok( ecs_meta_push(&cur) );
                    test_ok( ecs_meta_set_int(&cur, 20) );
                    test_ok( ecs_meta_next(&cur) );
                    test_ok( ecs_meta_set_int(&cur, 21) );
                    test_ok( ecs_meta_next(&cur) );
                    test_ok( ecs_meta_set_int(&cur, 22) );
                test_ok( ecs_meta_pop(&cur) );
            test_ok( ecs_meta_pop(&cur) );

            test_ok( ecs_meta_next(&cur) );

            test_ok( ecs_meta_push(&cur) );
                test_ok( ecs_meta_push(&cur) );
                    test_ok( ecs_meta_set_int(&cur, 30) );
                    test_ok( ecs_meta_next(&cur) );
                    test_ok( ecs_meta_set_int(&cur, 31) );
                    test_ok( ecs_meta_next(&cur) );
                    test_ok( ecs_meta_set_int(&cur, 32) );
                test_ok( ecs_meta_pop(&cur) );
            test_ok( ecs_meta_pop(&cur) );
        test_ok( ecs_meta_pop(&cur) );

        test_ok( ecs_meta_next(&cur) );

        test_ok( ecs_meta_push(&cur) );
            test_ok( ecs_meta_push(&cur) );
                test_ok( ecs_meta_push(&cur) );
                    test_ok( ecs_meta_set_int(&cur, 40) );
                    test_ok( ecs_meta_next(&cur) );
                    test_ok( ecs_meta_set_int(&cur, 41) );
                    test_ok( ecs_meta_next(&cur) );
                    test_ok( ecs_meta_set_int(&cur, 42) );
                test_ok( ecs_meta_pop(&cur) );
            test_ok( ecs_meta_pop(&cur) );

            test_ok( ecs_meta_next(&cur) );

            test_ok( ecs_meta_push(&cur) );
                test_ok( ecs_meta_push(&cur) );
                    test_ok( ecs_meta_set_int(&cur, 50) );
                    test_ok( ecs_meta_next(&cur) );
                    test_ok( ecs_meta_set_int(&cur, 51) );
                    test_ok( ecs_meta_next(&cur) );
                    test_ok( ecs_meta_set_int(&cur, 52) );
                test_ok( ecs_meta_pop(&cur) );
            test_ok( ecs_meta_pop(&cur) );

            test_ok( ecs_meta_next(&cur) );

            test_ok( ecs_meta_push(&cur) );
                test_ok( ecs_meta_push(&cur) );
                    test_ok( ecs_meta_set_int(&cur, 60) );
                    test_ok( ecs_meta_next(&cur) );
                    test_ok( ecs_meta_set_int(&cur, 61) );
                    test_ok( ecs_meta_next(&cur) );
                    test_ok( ecs_meta_set_int(&cur, 62) );
                test_ok( ecs_meta_pop(&cur) );
            test_ok( ecs_meta_pop(&cur) );
        test_ok( ecs_meta_pop(&cur) );
    test_ok( ecs_meta_pop(&cur) );

    test_int(value.n_1[0].x[0], 10);
    test_int(value.n_1[0].x[1], 11);
    test_int(value.n_1[0].x[2], 12);

    test_int(value.n_1[1].x[0], 20);
    test_int(value.n_1[1].x[1], 21);
    test_int(value.n_1[1].x[2], 22);

    test_int(value.n_1[2].x[0], 30);
    test_int(value.n_1[2].x[1], 31);
    test_int(value.n_1[2].x[2], 32);

    test_int(value.n_2[0].x[0], 40);
    test_int(value.n_2[0].x[1], 41);
    test_int(value.n_2[0].x[2], 42);

    test_int(value.n_2[1].x[0], 50);
    test_int(value.n_2[1].x[1], 51);
    test_int(value.n_2[1].x[2], 52);

    test_int(value.n_2[2].x[0], 60);
    test_int(value.n_2[2].x[1], 61);
    test_int(value.n_2[2].x[2], 62);

    ecs_fini(world);
}


void Cursor_struct_member_i32() {
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

    test_assert(t != 0);

    T value;

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, t, &value);
    test_ok( ecs_meta_push(&cur) );
    test_ok( ecs_meta_member(&cur, "x"));
    test_ok( ecs_meta_set_int(&cur, 10) );

    test_bool(value.x, 10);

    ecs_fini(world);
}

void Cursor_struct_member_i32_i32() {
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

    T value;

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, t, &value);
    test_ok( ecs_meta_push(&cur) );
    test_ok( ecs_meta_member(&cur, "x"));
    test_ok( ecs_meta_set_int(&cur, 10) );
    test_ok( ecs_meta_member(&cur, "y"));
    test_ok( ecs_meta_set_int(&cur, 20) );

    test_bool(value.x, 10);
    test_bool(value.y, 20);

    ecs_fini(world);
}

void Cursor_struct_member_i32_i32_reverse() {
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

    T value;

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, t, &value);
    test_ok( ecs_meta_push(&cur) );
    test_ok( ecs_meta_member(&cur, "y"));
    test_ok( ecs_meta_set_int(&cur, 20) );
    test_ok( ecs_meta_member(&cur, "x"));
    test_ok( ecs_meta_set_int(&cur, 10) );

    test_bool(value.x, 10);
    test_bool(value.y, 20);

    ecs_fini(world);
}

void Cursor_struct_member_nested_i32_i32() {
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

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, t, &value);
    test_ok( ecs_meta_push(&cur) );
        test_ok( ecs_meta_member(&cur, "n_1"));
        test_ok( ecs_meta_push(&cur) );
            test_ok( ecs_meta_member(&cur, "y"));
            test_ok( ecs_meta_set_int(&cur, 20) );
            test_ok( ecs_meta_member(&cur, "x"));
            test_ok( ecs_meta_set_int(&cur, 10) );
        test_ok( ecs_meta_pop(&cur) );
    test_ok( ecs_meta_pop(&cur) );

    test_bool(value.n_1.x, 10);
    test_bool(value.n_1.y, 20);

    ecs_fini(world);
}

void Cursor_struct_member_2_nested_i32_i32() {
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

    T value = {{0}};

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, t, &value);
    test_ok( ecs_meta_push(&cur) );
        test_ok( ecs_meta_member(&cur, "n_1"));
        test_ok( ecs_meta_push(&cur) );
            test_ok( ecs_meta_member(&cur, "y"));
            test_ok( ecs_meta_set_int(&cur, 20) );
            test_ok( ecs_meta_member(&cur, "x"));
            test_ok( ecs_meta_set_int(&cur, 10) );
        test_ok( ecs_meta_pop(&cur) );

        test_ok( ecs_meta_member(&cur, "n_2"));
        test_ok( ecs_meta_push(&cur) );
            test_ok( ecs_meta_member(&cur, "y"));
            test_ok( ecs_meta_set_int(&cur, 40) );
            test_ok( ecs_meta_member(&cur, "x"));
            test_ok( ecs_meta_set_int(&cur, 30) );
        test_ok( ecs_meta_pop(&cur) );
    test_ok( ecs_meta_pop(&cur) );

    test_bool(value.n_1.x, 10);
    test_bool(value.n_1.y, 20);
    test_bool(value.n_2.x, 30);
    test_bool(value.n_2.y, 40);

    ecs_fini(world);
}

void Cursor_struct_member_2_nested_i32_i32_reverse() {
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

    T value = {{0}};

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, t, &value);
    test_ok( ecs_meta_push(&cur) );
        test_ok( ecs_meta_member(&cur, "n_2"));
        test_ok( ecs_meta_push(&cur) );
            test_ok( ecs_meta_member(&cur, "y"));
            test_ok( ecs_meta_set_int(&cur, 40) );
            test_ok( ecs_meta_member(&cur, "x"));
            test_ok( ecs_meta_set_int(&cur, 30) );
        test_ok( ecs_meta_pop(&cur) );

        test_ok( ecs_meta_member(&cur, "n_1"));
        test_ok( ecs_meta_push(&cur) );
            test_ok( ecs_meta_member(&cur, "y"));
            test_ok( ecs_meta_set_int(&cur, 20) );
            test_ok( ecs_meta_member(&cur, "x"));
            test_ok( ecs_meta_set_int(&cur, 10) );
        test_ok( ecs_meta_pop(&cur) );
    test_ok( ecs_meta_pop(&cur) );

    test_bool(value.n_1.x, 10);
    test_bool(value.n_1.y, 20);
    test_bool(value.n_2.x, 30);
    test_bool(value.n_2.y, 40);

    ecs_fini(world);
}

void Cursor_struct_w_array_type_i32_i32() {
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

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, t, &value);
    test_ok( ecs_meta_push(&cur) );
        test_ok( ecs_meta_push(&cur) );
            test_ok( ecs_meta_set_int(&cur, 10) );
            test_ok( ecs_meta_next(&cur) );
            test_ok( ecs_meta_set_int(&cur, 20) );
        test_ok( ecs_meta_pop(&cur) );
    test_ok( ecs_meta_pop(&cur) );

    test_int(value.n_1[0], 10);
    test_int(value.n_1[1], 20);

    ecs_fini(world);
}

void Cursor_struct_w_array_type_struct() {
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

    T value;

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, t, &value);
    test_ok( ecs_meta_push(&cur) );
        test_ok( ecs_meta_push(&cur) );
            test_ok( ecs_meta_push(&cur) );
                test_ok( ecs_meta_set_int(&cur, 10) );
                test_ok( ecs_meta_next(&cur) );
                test_ok( ecs_meta_set_int(&cur, 20) );
            test_ok( ecs_meta_pop(&cur) );

            test_ok( ecs_meta_next(&cur) );

            test_ok( ecs_meta_push(&cur) );
                test_ok( ecs_meta_set_int(&cur, 30) );
                test_ok( ecs_meta_next(&cur) );
                test_ok( ecs_meta_set_int(&cur, 40) );
            test_ok( ecs_meta_pop(&cur) );
        test_ok( ecs_meta_pop(&cur) );
    test_ok( ecs_meta_pop(&cur) );

    test_int(value.n_1[0].x, 10);
    test_int(value.n_1[0].y, 20);
    test_int(value.n_1[1].x, 30);
    test_int(value.n_1[1].y, 40);

    ecs_fini(world);
}

void Cursor_struct_w_2_array_type_i32_i32() {
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

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, t, &value);
    test_ok( ecs_meta_push(&cur) );
        test_ok( ecs_meta_push(&cur) );
            test_ok( ecs_meta_set_int(&cur, 10) );
            test_ok( ecs_meta_next(&cur) );
            test_ok( ecs_meta_set_int(&cur, 20) );
        test_ok( ecs_meta_pop(&cur) );

        test_ok( ecs_meta_next(&cur) );

        test_ok( ecs_meta_push(&cur) );
            test_ok( ecs_meta_set_int(&cur, 30) );
            test_ok( ecs_meta_next(&cur) );
            test_ok( ecs_meta_set_int(&cur, 40) );
        test_ok( ecs_meta_pop(&cur) );
    test_ok( ecs_meta_pop(&cur) );

    test_int(value.n_1[0], 10);
    test_int(value.n_1[1], 20);
    test_int(value.n_2[0], 30);
    test_int(value.n_2[1], 40);

    ecs_fini(world);
}

void Cursor_struct_w_2_array_type_struct() {
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

    T value;

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, t, &value);
    test_ok( ecs_meta_push(&cur) );
        test_ok( ecs_meta_push(&cur) );
            test_ok( ecs_meta_push(&cur) );
                test_ok( ecs_meta_set_int(&cur, 10) );
                test_ok( ecs_meta_next(&cur) );
                test_ok( ecs_meta_set_int(&cur, 20) );
            test_ok( ecs_meta_pop(&cur) );

            test_ok( ecs_meta_next(&cur) );

            test_ok( ecs_meta_push(&cur) );
                test_ok( ecs_meta_set_int(&cur, 30) );
                test_ok( ecs_meta_next(&cur) );
                test_ok( ecs_meta_set_int(&cur, 40) );
            test_ok( ecs_meta_pop(&cur) );
        test_ok( ecs_meta_pop(&cur) );

        test_ok( ecs_meta_next(&cur) );

        test_ok( ecs_meta_push(&cur) );
            test_ok( ecs_meta_push(&cur) );
                test_ok( ecs_meta_set_int(&cur, 50) );
                test_ok( ecs_meta_next(&cur) );
                test_ok( ecs_meta_set_int(&cur, 60) );
            test_ok( ecs_meta_pop(&cur) );

            test_ok( ecs_meta_next(&cur) );

            test_ok( ecs_meta_push(&cur) );
                test_ok( ecs_meta_set_int(&cur, 70) );
                test_ok( ecs_meta_next(&cur) );
                test_ok( ecs_meta_set_int(&cur, 80) );
            test_ok( ecs_meta_pop(&cur) );
        test_ok( ecs_meta_pop(&cur) );
    test_ok( ecs_meta_pop(&cur) );

    test_int(value.n_1[0].x, 10);
    test_int(value.n_1[0].y, 20);
    test_int(value.n_1[1].x, 30);
    test_int(value.n_1[1].y, 40);

    test_int(value.n_2[0].x, 50);
    test_int(value.n_2[0].y, 60);
    test_int(value.n_2[1].x, 70);
    test_int(value.n_2[1].y, 80);

    ecs_fini(world);
}

void Cursor_array_move_primitive() {
    ecs_world_t *world = ecs_init();

    typedef struct {
        int32_t a[3];
    } T;

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "T"}),
        .members = {
            {"a", ecs_id(ecs_i32_t), 3}
        }
    });


    T value = {0};

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, t, &value);
    test_int(0, ecs_meta_push(&cur));
    test_int(0, ecs_meta_push(&cur));

    test_int(0, ecs_meta_elem(&cur, 2));
    test_int(0, ecs_meta_set_int(&cur, 10));

    test_int(value.a[0], 0);
    test_int(value.a[1], 0);
    test_int(value.a[2], 10);

    test_int(0, ecs_meta_elem(&cur, 0));
    test_int(0, ecs_meta_set_int(&cur, 20));

    test_int(value.a[0], 20);
    test_int(value.a[1], 0);
    test_int(value.a[2], 10);

    test_int(0, ecs_meta_elem(&cur, 1));
    test_int(0, ecs_meta_set_int(&cur, 30));

    test_int(value.a[0], 20);
    test_int(value.a[1], 30);
    test_int(value.a[2], 10);
    
    test_int(0, ecs_meta_pop(&cur));
    test_int(0, ecs_meta_pop(&cur));

    ecs_fini(world);
}

void Cursor_array_move_struct() {
    ecs_world_t *world = ecs_init();

    typedef struct {
        ecs_i32_t x;
        ecs_i32_t y;
    } N;

    typedef struct {
        N a[3];
    } T;

    ecs_entity_t n = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "N"}),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "T"}),
        .members = {
            {"a", n, 3}
        }
    });


    T value = {0};

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, t, &value);
    test_int(0, ecs_meta_push(&cur));
        test_int(0, ecs_meta_push(&cur));

            test_int(0, ecs_meta_elem(&cur, 2));
            test_int(0, ecs_meta_push(&cur));
                test_int(0, ecs_meta_set_int(&cur, 10));
                test_int(0, ecs_meta_next(&cur));
                test_int(0, ecs_meta_set_int(&cur, 20));
            test_int(0, ecs_meta_pop(&cur));

            test_int(value.a[0].x, 0);
            test_int(value.a[0].y, 0);
            test_int(value.a[1].x, 0);
            test_int(value.a[1].y, 0);
            test_int(value.a[2].x, 10);
            test_int(value.a[2].y, 20);

            test_int(0, ecs_meta_elem(&cur, 0));
            test_int(0, ecs_meta_push(&cur));
                test_int(0, ecs_meta_set_int(&cur, 30));
                test_int(0, ecs_meta_next(&cur));
                test_int(0, ecs_meta_set_int(&cur, 40));
            test_int(0, ecs_meta_pop(&cur));

            test_int(value.a[0].x, 30);
            test_int(value.a[0].y, 40);
            test_int(value.a[1].x, 0);
            test_int(value.a[1].y, 0);
            test_int(value.a[2].x, 10);
            test_int(value.a[2].y, 20);

            test_int(0, ecs_meta_elem(&cur, 1));
            test_int(0, ecs_meta_push(&cur));
                test_int(0, ecs_meta_set_int(&cur, 50));
                test_int(0, ecs_meta_next(&cur));
                test_int(0, ecs_meta_set_int(&cur, 60));
            test_int(0, ecs_meta_pop(&cur));

            test_int(value.a[0].x, 30);
            test_int(value.a[0].y, 40);
            test_int(value.a[1].x, 50);
            test_int(value.a[1].y, 60);
            test_int(value.a[2].x, 10);
            test_int(value.a[2].y, 20);
        
        test_int(0, ecs_meta_pop(&cur));
    test_int(0, ecs_meta_pop(&cur));

    ecs_fini(world);
}

void Cursor_array_move_out_of_range() {
    ecs_world_t *world = ecs_init();

    typedef struct {
        int32_t a[3];
    } T;

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "T"}),
        .members = {
            {"a", ecs_id(ecs_i32_t), 3}
        }
    });

    T value = {0};

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, t, &value);
    test_int(0, ecs_meta_push(&cur));
    test_int(0, ecs_meta_push(&cur));
    
    test_assert(0 == ecs_meta_elem(&cur, 0)); /* Sanity check */
    test_assert(0 == ecs_meta_elem(&cur, 2)); /* Sanity check */

    ecs_log_set_level(-4);

    test_assert(0 != ecs_meta_elem(&cur, -1));
    test_assert(0 != ecs_meta_elem(&cur, 3));
    
    ecs_log_set_level(-1);
    
    test_int(0, ecs_meta_pop(&cur));
    test_int(0, ecs_meta_pop(&cur));

    ecs_fini(world);
}
