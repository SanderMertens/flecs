#include <meta.h>

void Cursor_set_bool(void) {
    ecs_world_t *world = ecs_init();

    ecs_bool_t value = false;

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, ecs_id(ecs_bool_t), &value);
    test_ok( ecs_meta_set_bool(&cur, true) );

    test_bool(value, true);

    ecs_fini(world);
}

void Cursor_set_byte(void) {
    ecs_world_t *world = ecs_init();

    ecs_byte_t value = 10;

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, ecs_id(ecs_byte_t), &value);
    test_ok( ecs_meta_set_uint(&cur, 20) );

    test_uint(value, 20);

    ecs_fini(world);
}

void Cursor_set_char(void) {
    ecs_world_t *world = ecs_init();

    ecs_char_t value = 10;

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, ecs_id(ecs_char_t), &value);
    test_ok( ecs_meta_set_int(&cur, 20) );

    test_int(value, 20);

    ecs_fini(world);
}

void Cursor_set_i8(void) {
    ecs_world_t *world = ecs_init();

    ecs_i8_t value = 10;

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, ecs_id(ecs_i8_t), &value);
    test_ok( ecs_meta_set_int(&cur, 20) );

    test_int(value, 20);

    ecs_fini(world);
}

void Cursor_set_i16(void) {
    ecs_world_t *world = ecs_init();

    ecs_i16_t value = 10;

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, ecs_id(ecs_i16_t), &value);
    test_ok( ecs_meta_set_int(&cur, 20) );

    test_int(value, 20);

    ecs_fini(world);
}

void Cursor_set_i32(void) {
    ecs_world_t *world = ecs_init();

    ecs_i32_t value = 10;

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, ecs_id(ecs_i32_t), &value);
    test_ok( ecs_meta_set_int(&cur, 20) );

    test_int(value, 20);

    ecs_fini(world);
}

void Cursor_set_i64(void) {
    ecs_world_t *world = ecs_init();

    ecs_i64_t value = 10;

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, ecs_id(ecs_i64_t), &value);
    test_ok( ecs_meta_set_int(&cur, 20) );

    test_int(value, 20);

    ecs_fini(world);
}

void Cursor_set_iptr(void) {
    ecs_world_t *world = ecs_init();

    ecs_iptr_t value = 10;

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, ecs_id(ecs_iptr_t), &value);
    test_ok( ecs_meta_set_int(&cur, 20) );

    test_int(value, 20);

    ecs_fini(world);
}

void Cursor_set_u8(void) {
    ecs_world_t *world = ecs_init();

    ecs_u8_t value = 10;

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, ecs_id(ecs_u8_t), &value);
    test_ok( ecs_meta_set_uint(&cur, 20) );

    test_uint(value, 20);

    ecs_fini(world);
}

void Cursor_set_u16(void) {
    ecs_world_t *world = ecs_init();

    ecs_u16_t value = 10;

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, ecs_id(ecs_u8_t), &value);
    test_ok( ecs_meta_set_uint(&cur, 20) );

    test_uint(value, 20);

    ecs_fini(world);
}

void Cursor_set_u32(void) {
    ecs_world_t *world = ecs_init();

    ecs_u32_t value = 10;

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, ecs_id(ecs_u32_t), &value);
    test_ok( ecs_meta_set_uint(&cur, 20) );

    test_uint(value, 20);

    ecs_fini(world);
}

void Cursor_set_u64(void) {
    ecs_world_t *world = ecs_init();

    {
        ecs_u64_t value = 10;
        ecs_meta_cursor_t cur = ecs_meta_cursor(world, ecs_id(ecs_u64_t), &value);
        test_ok( ecs_meta_set_uint(&cur, 20) );
        test_uint(value, 20);
    }

    {
        ecs_u64_t value = 10;
        ecs_meta_cursor_t cur = ecs_meta_cursor(world, ecs_id(ecs_u64_t), &value);
        test_ok( ecs_meta_set_uint(&cur, 2366700781656087864) );
        test_uint(value, 2366700781656087864);
    }

    ecs_fini(world);
}

void Cursor_set_uptr(void) {
    ecs_world_t *world = ecs_init();

    ecs_uptr_t value = 10;

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, ecs_id(ecs_uptr_t), &value);
    test_ok( ecs_meta_set_uint(&cur, 20) );

    test_uint(value, 20);

    ecs_fini(world);
}

void Cursor_set_float(void) {
    ecs_world_t *world = ecs_init();

    ecs_f32_t value = 10;

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, ecs_id(ecs_f32_t), &value);
    test_ok( ecs_meta_set_float(&cur, 20.5) );

    test_flt(value, 20.5);

    ecs_fini(world);
}

void Cursor_set_double(void) {
    ecs_world_t *world = ecs_init();

    ecs_f64_t value = 10;

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, ecs_id(ecs_f64_t), &value);
    test_ok( ecs_meta_set_float(&cur, 20.5) );

    test_flt(value, 20.5);

    ecs_fini(world);
}

void Cursor_set_string(void) {
    ecs_world_t *world = ecs_init();

    char *value = NULL;

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, ecs_id(ecs_string_t), &value);
    test_ok( ecs_meta_set_string(&cur, "HelloWorld") );

    test_str(value, "HelloWorld");
    ecs_os_free(value);

    ecs_fini(world);
}

void Cursor_set_string_literal(void) {
    ecs_world_t *world = ecs_init();

    char *value = NULL;

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, ecs_id(ecs_string_t), &value);
    test_ok( ecs_meta_set_string_literal(&cur, "\"HelloWorld\"") );

    test_str(value, "HelloWorld");
    ecs_os_free(value);

    ecs_fini(world);
}

void Cursor_set_string_to_null(void) {
    ecs_world_t *world = ecs_init();

    char *value = ecs_os_strdup("HelloWorld");

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, ecs_id(ecs_string_t), &value);
    test_ok( ecs_meta_set_null(&cur) );

    test_str(value, NULL);
    ecs_os_free(value);

    ecs_fini(world);
}

void Cursor_set_entity(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t value = 0;

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, ecs_id(ecs_entity_t), &value);
    test_ok( ecs_meta_set_entity(&cur, EcsFlecs) );

    test_uint(value, EcsFlecs);

    ecs_fini(world);
}

void Cursor_set_entity_to_number(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t value = 0;

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, ecs_id(ecs_entity_t), &value);
    test_ok( ecs_meta_set_uint(&cur, 500) );

    test_uint(value, 500);

    ecs_fini(world);
}

void Cursor_set_entity_to_0(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t value = 0;

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, ecs_id(ecs_entity_t), &value);
    test_ok( ecs_meta_set_uint(&cur, 0) );

    test_uint(value, 0);

    ecs_fini(world);
}

void Cursor_set_id(void) {
    ecs_world_t *world = ecs_init();

    ecs_id_t value = 0;

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, ecs_id(ecs_id_t), &value);
    test_ok( ecs_meta_set_uint(&cur, 500) );

    test_uint(value, 500);

    ecs_fini(world);
}

void Cursor_set_id_to_number(void) {
    ecs_world_t *world = ecs_init();

    ecs_id_t value = 0;

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, ecs_id(ecs_id_t), &value);
    test_ok( ecs_meta_set_uint(&cur, 500) );

    test_uint(value, 500);

    ecs_fini(world);
}

void Cursor_set_id_to_0(void) {
    ecs_world_t *world = ecs_init();

    ecs_id_t value = 0;

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, ecs_id(ecs_id_t), &value);
    test_ok( ecs_meta_set_uint(&cur, 0) );

    test_uint(value, 0);

    ecs_fini(world);
}

void Cursor_set_enum(void) {
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

void Cursor_set_bitmask(void) {
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

void Cursor_set_signed_as_unsigned(void) {
    ecs_world_t *world = ecs_init();

    int32_t value = 0;

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, ecs_id(ecs_i32_t), &value);
    test_ok( ecs_meta_set_uint(&cur, 10) );

    test_int(value, 10);

    ecs_fini(world);
}

void Cursor_set_unsigned_as_signed(void) {
    ecs_world_t *world = ecs_init();

    uint32_t value = 0;

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, ecs_id(ecs_u32_t), &value);
    test_ok( ecs_meta_set_int(&cur, 10) );

    test_int(value, 10);

    ecs_fini(world);
}

void Cursor_set_signed_as_unsigned_out_of_range(void) {
    ecs_log_set_level(-4);
    ecs_world_t *world = ecs_init();

    int8_t value = 0;

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, ecs_id(ecs_i8_t), &value);
    test_fail( ecs_meta_set_uint(&cur, 128) );

    test_int(value, 0);

    ecs_fini(world);
}

void Cursor_set_unsigned_as_signed_out_of_range(void) {
    ecs_log_set_level(-4);
    ecs_world_t *world = ecs_init();

    uint32_t value = 0;

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, ecs_id(ecs_u32_t), &value);
    test_fail( ecs_meta_set_int(&cur, -10) );

    test_int(value, 0);

    ecs_fini(world);
}

void Cursor_set_string_to_null_as_signed(void) {
    ecs_world_t *world = ecs_init();

    char *value = ecs_os_strdup("Hello");

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, ecs_id(ecs_string_t), &value);
    test_ok( ecs_meta_set_int(&cur, 0) );

    test_str(value, 0);

    ecs_fini(world);
}

void Cursor_set_string_to_null_as_unsigned(void) {
    ecs_world_t *world = ecs_init();

    char *value = ecs_os_strdup("Hello");

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, ecs_id(ecs_string_t), &value);
    test_ok( ecs_meta_set_uint(&cur, 0) );

    test_str(value, 0);

    ecs_fini(world);
}

void Cursor_set_entity_as_signed(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t value = 0;
    ecs_entity_t e = ecs_new(world);

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, ecs_id(ecs_entity_t), &value);
    test_ok( ecs_meta_set_int(&cur, (int64_t)e) );

    test_uint(value, e);

    ecs_fini(world);
}

void Cursor_set_entity_as_unsigned(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t value = 0;
    ecs_entity_t e = ecs_new(world);

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, ecs_id(ecs_entity_t), &value);
    test_ok( ecs_meta_set_uint(&cur, e) );

    test_uint(value, e);

    ecs_fini(world);
}

void Cursor_set_entity_as_signed_out_of_range(void) {
    ecs_log_set_level(-4);
    ecs_world_t *world = ecs_init();

    ecs_entity_t value = 0;

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, ecs_id(ecs_entity_t), &value);
    test_fail( ecs_meta_set_int(&cur, -10) );

    test_uint(value, 0);

    ecs_fini(world);
}

void Cursor_set_id_as_signed(void) {
    ecs_world_t *world = ecs_init();

    ecs_id_t value = 0;
    ecs_id_t e = ecs_new(world);

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, ecs_id(ecs_id_t), &value);
    test_ok( ecs_meta_set_int(&cur, (int64_t)e) );

    test_uint(value, e);

    ecs_fini(world);
}

void Cursor_set_id_as_unsigned(void) {
    ecs_world_t *world = ecs_init();

    ecs_id_t value = 0;
    ecs_id_t e = ecs_new(world);

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, ecs_id(ecs_id_t), &value);
    test_ok( ecs_meta_set_uint(&cur, e) );

    test_uint(value, e);

    ecs_fini(world);
}

void Cursor_set_id_as_signed_out_of_range(void) {
    ecs_log_set_level(-4);
    ecs_world_t *world = ecs_init();

    ecs_id_t value = 0;

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, ecs_id(ecs_id_t), &value);
    test_fail( ecs_meta_set_int(&cur, -10) );

    test_uint(value, 0);

    ecs_fini(world);
}

void Cursor_set_str_to_bool(void) {
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

void Cursor_set_str_to_char(void) {
    ecs_world_t *world = ecs_init();

    ecs_char_t value = false;

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, ecs_id(ecs_char_t), &value);
    test_ok( ecs_meta_set_string(&cur, "10") );

    test_bool(value, 10);

    ecs_fini(world);
}

void Cursor_set_str_literal_to_char(void) {
    ecs_world_t *world = ecs_init();

    ecs_char_t value = false;

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, ecs_id(ecs_char_t), &value);
    test_ok( ecs_meta_set_string_literal(&cur, "\"a\"") );

    test_int(value, 'a');

    ecs_fini(world);
}

void Cursor_set_str_to_i8(void) {
    ecs_world_t *world = ecs_init();

    ecs_i8_t value = 10;

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, ecs_id(ecs_i8_t), &value);
    test_ok( ecs_meta_set_string(&cur, "20") );

    test_int(value, 20);

    ecs_fini(world);
}

void Cursor_set_str_to_i16(void) {
    ecs_world_t *world = ecs_init();

    ecs_i16_t value = 10;

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, ecs_id(ecs_i16_t), &value);
    test_ok( ecs_meta_set_string(&cur, "20") );

    test_int(value, 20);

    ecs_fini(world);
}

void Cursor_set_str_to_i32(void) {
    ecs_world_t *world = ecs_init();

    ecs_i32_t value = 10;

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, ecs_id(ecs_i32_t), &value);
    test_ok( ecs_meta_set_string(&cur, "20") );

    test_int(value, 20);

    ecs_fini(world);
}

void Cursor_set_str_to_i64(void) {
    ecs_world_t *world = ecs_init();

    {
        ecs_i64_t value = 10;
        ecs_meta_cursor_t cur = ecs_meta_cursor(world, ecs_id(ecs_i64_t), &value);
        test_ok( ecs_meta_set_string(&cur, "20") );
        test_int(value, 20);
    }

    {
        ecs_i64_t value = 10;
        ecs_meta_cursor_t cur = ecs_meta_cursor(world, ecs_id(ecs_i64_t), &value);
        test_ok( ecs_meta_set_string(&cur, "2366700781656087864") );
        test_int(value, 2366700781656087864);
    }

    ecs_fini(world);
}

void Cursor_set_str_to_u64(void) {
    ecs_world_t *world = ecs_init();

    {
        ecs_u64_t value = 10;
        ecs_meta_cursor_t cur = ecs_meta_cursor(world, ecs_id(ecs_u64_t), &value);
        test_ok( ecs_meta_set_string(&cur, "20") );
        test_uint(value, 20);
    }

    {
        ecs_u64_t value = 10;
        ecs_meta_cursor_t cur = ecs_meta_cursor(world, ecs_id(ecs_u64_t), &value);
        test_ok( ecs_meta_set_string(&cur, "2366700781656087864") );
        test_uint(value, 2366700781656087864);
    }

    ecs_fini(world);
}

void Cursor_set_str_to_f32(void) {
    ecs_world_t *world = ecs_init();

    ecs_f32_t value = 10.5;

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, ecs_id(ecs_f32_t), &value);
    test_ok( ecs_meta_set_string(&cur, "20.5") );

    test_flt(value, 20.5);

    ecs_fini(world);
}

void Cursor_set_str_to_f64(void) {
    ecs_world_t *world = ecs_init();

    ecs_f64_t value = 10.5;

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, ecs_id(ecs_f64_t), &value);
    test_ok( ecs_meta_set_string(&cur, "20.5") );

    test_flt(value, 20.5);

    ecs_fini(world);
}

void Cursor_set_str_to_entity(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t value = 0;

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, ecs_id(ecs_entity_t), &value);
    test_ok( ecs_meta_set_string(&cur, "flecs.core") );

    test_uint(value, EcsFlecsCore);

    ecs_fini(world);
}

void Cursor_set_str_to_id(void) {
    ecs_world_t *world = ecs_init();

    ecs_id_t value = 0;

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, ecs_id(ecs_id_t), &value);
    test_ok( ecs_meta_set_string(&cur, "flecs.core") );

    test_uint(value, EcsFlecsCore);

    ecs_fini(world);
}

void Cursor_set_str_to_invalid_bool(void) {
    ecs_log_set_level(-4);

    ecs_world_t *world = ecs_init();

    ecs_bool_t value = false;

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, ecs_id(ecs_bool_t), &value);
    test_fail( ecs_meta_set_string(&cur, "foo") );

    test_bool(value, false);

    ecs_fini(world);
}

void Cursor_set_str_to_invalid_entity(void) {
    ecs_log_set_level(-4);

    ecs_world_t *world = ecs_init();

    ecs_entity_t value = 0;

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, ecs_id(ecs_entity_t), &value);
    test_fail( ecs_meta_set_string(&cur, "flops.core") );

    test_uint(value, 0);

    ecs_fini(world);
}

void Cursor_set_str_to_invalid_id(void) {
    ecs_log_set_level(-4);

    ecs_world_t *world = ecs_init();

    ecs_id_t value = 0;

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, ecs_id(ecs_id_t), &value);
    test_fail( ecs_meta_set_string(&cur, "flops.core") );

    test_uint(value, 0);

    ecs_fini(world);
}

void Cursor_struct_set_i32(void) {
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

void Cursor_struct_set_i32_i32(void) {
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

void Cursor_struct_set_i32_i32_i32(void) {
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

void Cursor_struct_set_bool(void) {
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

void Cursor_struct_set_bool_bool(void) {
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

void Cursor_struct_set_bool_bool_bool(void) {
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

void Cursor_struct_set_bool_bool_i32_bool(void) {
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

void Cursor_struct_nested_i32(void) {
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

void Cursor_struct_2_nested_1_i32_2_i32(void) {
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

void Cursor_struct_2_nested_1_i32_i32_2_i32_i32(void) {
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

void Cursor_struct_2_nested_1_i32_i32_2_i32_i32_w_padding_members(void) {
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

void Cursor_struct_set_array_i32_3(void) {
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

void Cursor_struct_set_2_array_i32_3(void) {
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

void Cursor_struct_set_2_array_i32_3_w_padding_members(void) {
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

void Cursor_struct_set_array_w_struct_i32(void) {
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

void Cursor_struct_set_2_array_w_struct_i32(void) {
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

void Cursor_struct_set_array_w_struct_w_array_i32(void) {
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

void Cursor_struct_set_2_array_w_struct_w_array_i32(void) {
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


void Cursor_struct_member_i32(void) {
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

void Cursor_struct_member_i32_i32(void) {
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

void Cursor_struct_member_i32_i32_reverse(void) {
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

void Cursor_struct_member_nested_i32_i32(void) {
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

void Cursor_struct_member_2_nested_i32_i32(void) {
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

void Cursor_struct_member_2_nested_i32_i32_reverse(void) {
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

void Cursor_struct_dotmember(void) {
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
    test_ok( ecs_meta_dotmember(&cur, "n_1.y"));
    test_ok( ecs_meta_set_int(&cur, 20) );
    test_ok( ecs_meta_dotmember(&cur, "n_1.x"));
    test_ok( ecs_meta_set_int(&cur, 10) );

    test_ok( ecs_meta_dotmember(&cur, "n_2.y"));
    test_ok( ecs_meta_set_int(&cur, 40) );
    test_ok( ecs_meta_dotmember(&cur, "n_2.x"));
    test_ok( ecs_meta_set_int(&cur, 30) );
    test_ok( ecs_meta_pop(&cur) );

    test_bool(value.n_1.x, 10);
    test_bool(value.n_1.y, 20);
    test_bool(value.n_2.x, 30);
    test_bool(value.n_2.y, 40);

    ecs_fini(world);
}

void Cursor_struct_dotmember_assign_twice(void) {
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
    test_ok( ecs_meta_dotmember(&cur, "n_1.y"));
    test_ok( ecs_meta_set_int(&cur, 20) );
    test_ok( ecs_meta_set_int(&cur, 25) );
    test_ok( ecs_meta_dotmember(&cur, "n_1.x"));
    test_ok( ecs_meta_set_int(&cur, 10) );
    test_ok( ecs_meta_set_int(&cur, 15) );

    test_ok( ecs_meta_dotmember(&cur, "n_2.y"));
    test_ok( ecs_meta_set_int(&cur, 40) );
    test_ok( ecs_meta_set_int(&cur, 45) );
    test_ok( ecs_meta_dotmember(&cur, "n_2.x"));
    test_ok( ecs_meta_set_int(&cur, 30) );
    test_ok( ecs_meta_set_int(&cur, 35) );
    test_ok( ecs_meta_pop(&cur) );

    test_bool(value.n_1.x, 15);
    test_bool(value.n_1.y, 25);
    test_bool(value.n_2.x, 35);
    test_bool(value.n_2.y, 45);

    ecs_fini(world);
}

void Cursor_struct_member_after_dotmember(void) {
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
        test_ok( ecs_meta_dotmember(&cur, "n_2.y"));
        test_ok( ecs_meta_set_int(&cur, 40) );
        test_ok( ecs_meta_dotmember(&cur, "n_2.x"));
        test_ok( ecs_meta_set_int(&cur, 30) );

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

void Cursor_struct_next_after_dotmember(void) {
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
        test_ok( ecs_meta_dotmember(&cur, "n_2.y"));
        test_ok( ecs_meta_set_int(&cur, 40) );
        test_ok( ecs_meta_dotmember(&cur, "n_2.x"));
        test_ok( ecs_meta_set_int(&cur, 30) );

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

void Cursor_struct_pop_after_dotmember(void) {
    typedef struct {
        ecs_i32_t x;
        ecs_i32_t y;
    } N2;

    typedef struct {
        N2 n_1;
        N2 n_2;
    } N1;

    typedef struct {
        N1 n_1;
        N1 n_2;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t n2 = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "N2"}),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t n1 = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "N1"}),
        .members = {
            {"n_1", n2},
            {"n_2", n2}
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
            test_ok( ecs_meta_dotmember(&cur, "n_2.y"));
            test_ok( ecs_meta_set_int(&cur, 80) );
            test_ok( ecs_meta_dotmember(&cur, "n_2.x"));
            test_ok( ecs_meta_set_int(&cur, 70) );
            test_ok( ecs_meta_dotmember(&cur, "n_1.y"));
            test_ok( ecs_meta_set_int(&cur, 60) );
            test_ok( ecs_meta_dotmember(&cur, "n_1.x"));
            test_ok( ecs_meta_set_int(&cur, 50) );
        test_ok( ecs_meta_pop(&cur) );

        test_ok( ecs_meta_member(&cur, "n_1"));
        test_ok( ecs_meta_push(&cur) );
            test_ok( ecs_meta_dotmember(&cur, "n_2.y"));
            test_ok( ecs_meta_set_int(&cur, 40) );
            test_ok( ecs_meta_dotmember(&cur, "n_2.x"));
            test_ok( ecs_meta_set_int(&cur, 30) );
            test_ok( ecs_meta_dotmember(&cur, "n_1.y"));
            test_ok( ecs_meta_set_int(&cur, 20) );
            test_ok( ecs_meta_dotmember(&cur, "n_1.x"));
            test_ok( ecs_meta_set_int(&cur, 10) );
        test_ok( ecs_meta_pop(&cur) );
    test_ok( ecs_meta_pop(&cur) );

    test_bool(value.n_1.n_1.x, 10);
    test_bool(value.n_1.n_1.y, 20);
    test_bool(value.n_1.n_2.x, 30);
    test_bool(value.n_1.n_2.y, 40);

    test_bool(value.n_2.n_1.x, 50);
    test_bool(value.n_2.n_1.y, 60);
    test_bool(value.n_2.n_2.x, 70);
    test_bool(value.n_2.n_2.y, 80);

    ecs_fini(world);
}

void Cursor_struct_w_array_type_i32_i32(void) {
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

void Cursor_struct_w_array_type_struct(void) {
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

void Cursor_struct_w_2_array_type_i32_i32(void) {
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

void Cursor_struct_w_2_array_type_struct(void) {
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

void Cursor_array_i32_3(void) {
    ecs_world_t *world = ecs_init();

    typedef struct {
        int32_t a[3];
    } T;

    ecs_entity_t t = ecs_array_init(world, &(ecs_array_desc_t){
        .entity = ecs_entity(world, {.name = "T"}),
        .type = ecs_id(ecs_i32_t),
        .count = 3
    });

    T value = {0};

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, t, &value);
    test_int(0, ecs_meta_push(&cur));

    test_int(0, ecs_meta_set_int(&cur, 10));
    test_int(0, ecs_meta_next(&cur));
    test_int(0, ecs_meta_set_int(&cur, 20));
    test_int(0, ecs_meta_next(&cur));
    test_int(0, ecs_meta_set_int(&cur, 30));

    test_int(value.a[0], 10);
    test_int(value.a[1], 20);
    test_int(value.a[2], 30);

    test_int(0, ecs_meta_pop(&cur));

    ecs_fini(world);
}

void Cursor_array_struct_3(void) {
    ecs_world_t *world = ecs_init();

    typedef struct {
        int32_t x, y;
    } N;

    typedef struct {
        N a[2];
    } T;

    ecs_entity_t n = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "N"}),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t t = ecs_array_init(world, &(ecs_array_desc_t){
        .entity = ecs_entity(world, {.name = "T"}),
        .type = n,
        .count = 2
    });

    T value = {0};

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, t, &value);
    test_int(0, ecs_meta_push(&cur));
        test_int(0, ecs_meta_push(&cur));
            test_int(0, ecs_meta_set_int(&cur, 10));
            test_int(0, ecs_meta_next(&cur));
            test_int(0, ecs_meta_set_int(&cur, 20));
        test_int(0, ecs_meta_pop(&cur));

        test_int(0, ecs_meta_next(&cur));

        test_int(0, ecs_meta_push(&cur));
            test_int(0, ecs_meta_set_int(&cur, 30));
            test_int(0, ecs_meta_next(&cur));
            test_int(0, ecs_meta_set_int(&cur, 40));
        test_int(0, ecs_meta_pop(&cur));
    test_int(0, ecs_meta_pop(&cur));

    test_int(value.a[0].x, 10);
    test_int(value.a[0].y, 20);
    test_int(value.a[1].x, 30);
    test_int(value.a[1].y, 40);

    ecs_fini(world);
}

void Cursor_array_move_primitive(void) {
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

void Cursor_array_move_struct(void) {
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

void Cursor_array_move_out_of_range(void) {
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

typedef const char* const_string_t;

#define OpaqueType(t)\
    typedef struct { \
        t value; \
    } Opaque_##t; \
    \
    static void t##_set(void *ptr, t value) { \
        ((Opaque_##t*)ptr)->value = value; \
    }

OpaqueType(bool)
OpaqueType(char)
OpaqueType(int64_t)
OpaqueType(uint64_t)
OpaqueType(double)
OpaqueType(const_string_t)

typedef struct {
    ecs_entity_t value;
} Opaque_entity;

typedef struct {
    ecs_id_t value;
} Opaque_id;

static void Opaque_entity_set(void *ptr, ecs_world_t *world, ecs_entity_t value) {
    ((Opaque_entity*)ptr)->value = value;
}

static void Opaque_id_set(void *ptr, ecs_world_t *world, ecs_id_t value) {
    ((Opaque_id*)ptr)->value = value;
}

void Cursor_opaque_set_bool(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Opaque_bool);

    ecs_opaque(world, {
        .entity = ecs_id(Opaque_bool),
        .type.as_type = ecs_id(ecs_bool_t),
        .type.assign_bool = bool_set
    });

    Opaque_bool v = { false };

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, ecs_id(Opaque_bool), &v);
    test_int(0, ecs_meta_set_bool(&cur, true));
    test_bool(v.value, true);
    test_int(0, ecs_meta_set_bool(&cur, false));
    test_bool(v.value, false);

    ecs_fini(world);
}

void Cursor_opaque_set_char(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Opaque_char);

    ecs_opaque(world, {
        .entity = ecs_id(Opaque_char),
        .type.as_type = ecs_id(ecs_char_t),
        .type.assign_char = char_set
    });

    Opaque_char v = { 0 };

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, ecs_id(Opaque_char), &v);
    test_int(0, ecs_meta_set_char(&cur, 'a'));
    test_int(v.value, 'a');
    test_int(0, ecs_meta_set_char(&cur, 'A'));
    test_int(v.value, 'A');

    ecs_fini(world);
}

void Cursor_opaque_set_int(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Opaque_int64_t);

    ecs_opaque(world, {
        .entity = ecs_id(Opaque_int64_t),
        .type.as_type = ecs_id(ecs_i64_t),
        .type.assign_int = int64_t_set
    });

    Opaque_int64_t v = { 0 };

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, ecs_id(Opaque_int64_t), &v);
    test_int(0, ecs_meta_set_int(&cur, 10));
    test_int(v.value, 10);
    test_int(0, ecs_meta_set_int(&cur, -10));
    test_int(v.value, -10);

    ecs_fini(world);
}

void Cursor_opaque_set_uint(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Opaque_uint64_t);

    ecs_opaque(world, {
        .entity = ecs_id(Opaque_uint64_t),
        .type.as_type = ecs_id(ecs_i64_t),
        .type.assign_uint = uint64_t_set
    });

    Opaque_uint64_t v = { 0 };

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, ecs_id(Opaque_uint64_t), &v);
    test_int(0, ecs_meta_set_uint(&cur, 10));
    test_int(v.value, 10);
    test_int(0, ecs_meta_set_uint(&cur, 20));
    test_int(v.value, 20);

    ecs_fini(world);
}

void Cursor_opaque_set_float(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Opaque_double);

    ecs_opaque(world, {
        .entity = ecs_id(Opaque_double),
        .type.as_type = ecs_id(ecs_f64_t),
        .type.assign_float = double_set
    });

    Opaque_double v = { 0 };

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, ecs_id(Opaque_double), &v);
    test_int(0, ecs_meta_set_float(&cur, 10.5));
    test_flt(v.value, 10.5);
    test_int(0, ecs_meta_set_float(&cur, 20.5));
    test_flt(v.value, 20.5);

    ecs_fini(world);
}

void Cursor_opaque_set_string(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Opaque_const_string_t);

    ecs_opaque(world, {
        .entity = ecs_id(Opaque_const_string_t),
        .type.as_type = ecs_id(ecs_string_t),
        .type.assign_string = const_string_t_set
    });

    Opaque_const_string_t v = { 0 };

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, ecs_id(Opaque_const_string_t), &v);
    test_int(0, ecs_meta_set_string(&cur, "Hello World"));
    test_str(v.value, "Hello World");

    ecs_fini(world);
}

void Cursor_opaque_set_entity(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Opaque_entity);

    ecs_opaque(world, {
        .entity = ecs_id(Opaque_entity),
        .type.as_type = ecs_id(ecs_entity_t),
        .type.assign_entity = Opaque_entity_set
    });

    Opaque_entity v = { 0 };
    ecs_entity_t e1 = ecs_new(world);
    ecs_entity_t e2 = ecs_new(world);

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, ecs_id(Opaque_entity), &v);
    test_int(0, ecs_meta_set_entity(&cur, e1));
    test_uint(v.value, e1);
    test_int(0, ecs_meta_set_entity(&cur, e2));
    test_uint(v.value, e2);

    ecs_fini(world);
}

void Cursor_opaque_set_id(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Opaque_id);

    ecs_opaque(world, {
        .entity = ecs_id(Opaque_id),
        .type.as_type = ecs_id(ecs_id_t),
        .type.assign_id = Opaque_id_set
    });

    Opaque_id v = { 0 };
    ecs_entity_t e1 = ecs_new(world);
    ecs_entity_t e2 = ecs_new(world);

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, ecs_id(Opaque_id), &v);
    test_int(0, ecs_meta_set_id(&cur, e1));
    test_uint(v.value, e1);
    test_int(0, ecs_meta_set_id(&cur, e2));
    test_uint(v.value, e2);

    ecs_fini(world);
}

typedef struct {
    size_t count;
    int32_t *array;
} IntVec;

static size_t IntVec_count(const void *ptr) {
    const IntVec *data = ptr;
    return data->count;
}

static void* IntVec_ensure(void *ptr, size_t index) {
    IntVec *data = ptr;
    if (data->count <= index) {
        data->count = index + 1;
        data->array = ecs_os_realloc_n(data->array, int32_t, data->count);
    }
    return &data->array[index];
}

static void IntVec_resize(void *ptr, size_t size) {
    IntVec *data = ptr;
    if (data->count != size) {
        data->count = size;
        if (!data->count) {
            ecs_os_free(data->array);
            data->array = NULL;
        } else {
            data->array = ecs_os_realloc_n(data->array, int32_t, size);
        }
    }
}

void Cursor_opaque_set_int_vec(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, IntVec);

    ecs_opaque(world, {
        .entity = ecs_id(IntVec),
        .type.as_type = ecs_vector(world, { .type = ecs_id(ecs_i32_t) }),
        .type.ensure_element = IntVec_ensure,
        .type.count = IntVec_count,
        .type.resize = IntVec_resize
    });

    IntVec v = { 0 };

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, ecs_id(IntVec), &v);
    test_int(0, ecs_meta_push(&cur));
    test_int(0, ecs_meta_set_int(&cur, 10));
    test_int(0, ecs_meta_next(&cur));
    test_int(0, ecs_meta_set_int(&cur, 20));
    test_int(0, ecs_meta_next(&cur));
    test_int(0, ecs_meta_set_int(&cur, 30));
    test_int(0, ecs_meta_pop(&cur));

    test_int(v.count, 3);
    test_int(v.array[0], 10);
    test_int(v.array[1], 20);
    test_int(v.array[2], 30);

    ecs_os_free(v.array);

    ecs_fini(world);
}

void Cursor_opaque_set_int_vec_empty(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, IntVec);

    ecs_opaque(world, {
        .entity = ecs_id(IntVec),
        .type.as_type = ecs_vector(world, { .type = ecs_id(ecs_i32_t)} ),
        .type.ensure_element = IntVec_ensure,
        .type.count = IntVec_count,
        .type.resize = IntVec_resize
    });

    IntVec v = { 0 };

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, ecs_id(IntVec), &v);
    test_int(0, ecs_meta_push(&cur));
    test_int(0, ecs_meta_pop(&cur));

    test_int(v.count, 0);
    test_assert(v.array == NULL);

    ecs_fini(world);
}

void Cursor_opaque_set_int_vec_resize_smaller(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, IntVec);

    ecs_opaque(world, {
        .entity = ecs_id(IntVec),
        .type.as_type = ecs_vector(world, { .type = ecs_id(ecs_i32_t)} ),
        .type.ensure_element = IntVec_ensure,
        .type.count = IntVec_count,
        .type.resize = IntVec_resize
    });

    IntVec v = { 0 };

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, ecs_id(IntVec), &v);
    test_int(0, ecs_meta_push(&cur));
    test_int(0, ecs_meta_set_int(&cur, 10));
    test_int(0, ecs_meta_next(&cur));
    test_int(0, ecs_meta_set_int(&cur, 20));
    test_int(0, ecs_meta_next(&cur));
    test_int(0, ecs_meta_set_int(&cur, 30));
    test_int(0, ecs_meta_pop(&cur));

    test_int(v.count, 3);
    test_int(v.array[0], 10);
    test_int(v.array[1], 20);
    test_int(v.array[2], 30);

    cur = ecs_meta_cursor(world, ecs_id(IntVec), &v);
    test_int(0, ecs_meta_push(&cur));
    test_int(0, ecs_meta_set_int(&cur, 40));
    test_int(0, ecs_meta_next(&cur));
    test_int(0, ecs_meta_set_int(&cur, 50));
    test_int(0, ecs_meta_pop(&cur));

    test_int(v.count, 2);
    test_int(v.array[0], 40);
    test_int(v.array[1], 50);

    ecs_os_free(v.array);

    ecs_fini(world);
}

void Cursor_opaque_set_int_vec_resize_larger(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, IntVec);

    ecs_opaque(world, {
        .entity = ecs_id(IntVec),
        .type.as_type = ecs_vector(world, { .type = ecs_id(ecs_i32_t)} ),
        .type.ensure_element = IntVec_ensure,
        .type.count = IntVec_count,
        .type.resize = IntVec_resize
    });

    IntVec v = { 0 };

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, ecs_id(IntVec), &v);
    test_int(0, ecs_meta_push(&cur));
    test_int(0, ecs_meta_set_int(&cur, 10));
    test_int(0, ecs_meta_next(&cur));
    test_int(0, ecs_meta_set_int(&cur, 20));
    test_int(0, ecs_meta_next(&cur));
    test_int(0, ecs_meta_set_int(&cur, 30));
    test_int(0, ecs_meta_pop(&cur));

    test_int(v.count, 3);
    test_int(v.array[0], 10);
    test_int(v.array[1], 20);
    test_int(v.array[2], 30);

    cur = ecs_meta_cursor(world, ecs_id(IntVec), &v);
    test_int(0, ecs_meta_push(&cur));
    test_int(0, ecs_meta_set_int(&cur, 40));
    test_int(0, ecs_meta_next(&cur));
    test_int(0, ecs_meta_set_int(&cur, 50));
    test_int(0, ecs_meta_next(&cur));
    test_int(0, ecs_meta_set_int(&cur, 60));
    test_int(0, ecs_meta_next(&cur));
    test_int(0, ecs_meta_set_int(&cur, 70));
    test_int(0, ecs_meta_pop(&cur));

    test_int(v.count, 4);
    test_int(v.array[0], 40);
    test_int(v.array[1], 50);
    test_int(v.array[2], 60);
    test_int(v.array[3], 70);

    ecs_os_free(v.array);

    ecs_fini(world);
}

void Cursor_opaque_set_int_vec_resize_empty(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, IntVec);

    ecs_opaque(world, {
        .entity = ecs_id(IntVec),
        .type.as_type = ecs_vector(world, { .type = ecs_id(ecs_i32_t)} ),
        .type.ensure_element = IntVec_ensure,
        .type.count = IntVec_count,
        .type.resize = IntVec_resize
    });

    IntVec v = { 0 };

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, ecs_id(IntVec), &v);
    test_int(0, ecs_meta_push(&cur));
    test_int(0, ecs_meta_set_int(&cur, 10));
    test_int(0, ecs_meta_next(&cur));
    test_int(0, ecs_meta_set_int(&cur, 20));
    test_int(0, ecs_meta_next(&cur));
    test_int(0, ecs_meta_set_int(&cur, 30));
    test_int(0, ecs_meta_pop(&cur));

    test_int(v.count, 3);
    test_int(v.array[0], 10);
    test_int(v.array[1], 20);
    test_int(v.array[2], 30);

    cur = ecs_meta_cursor(world, ecs_id(IntVec), &v);
    test_int(0, ecs_meta_push(&cur));
    test_int(0, ecs_meta_pop(&cur));

    test_int(v.count, 0);
    test_assert(v.array == NULL);

    ecs_fini(world);
}

typedef struct {
    int32_t value;
} Int;

typedef struct {
    size_t count;
    Int *array;
} BoxedIntVec;

static void Int_assign_int(void *ptr, int64_t value) {
    Int *data = ptr;
    data->value = value;
}

static size_t BoxedIntVec_count(const void *ptr) {
    const BoxedIntVec *data = ptr;
    return data->count;
}

static void* BoxedIntVec_ensure(void *ptr, size_t index) {
    BoxedIntVec *data = ptr;
    if (data->count <= index) {
        data->count = index + 1;
        data->array = ecs_os_realloc_n(data->array, Int, data->count);
    }
    return &data->array[index];
}

static void BoxedIntVec_resize(void *ptr, size_t size) {
    BoxedIntVec *data = ptr;
    if (data->count != size) {
        data->count = size;
        if (!data->count) {
            ecs_os_free(data->array);
            data->array = NULL;
        } else {
            data->array = ecs_os_realloc_n(data->array, Int, size);
        }
    }
}

void Cursor_opaque_set_opaque_int_vec(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, BoxedIntVec);
    ECS_COMPONENT(world, Int);

    ecs_opaque(world, {
        .entity = ecs_id(Int),
        .type.as_type = ecs_id(ecs_i32_t),
        .type.assign_int = Int_assign_int
    });

    ecs_opaque(world, {
        .entity = ecs_id(BoxedIntVec),
        .type.as_type = ecs_vector(world, { .type = ecs_id(Int)} ),
        .type.ensure_element = BoxedIntVec_ensure,
        .type.count = BoxedIntVec_count,
        .type.resize = BoxedIntVec_resize
    });

    BoxedIntVec v = { 0 };
    ecs_meta_cursor_t cur = ecs_meta_cursor(world, ecs_id(BoxedIntVec), &v);
    test_int(0, ecs_meta_push(&cur));
    test_int(0, ecs_meta_set_int(&cur, 10));
    test_int(0, ecs_meta_next(&cur));
    test_int(0, ecs_meta_set_int(&cur, 20));
    test_int(0, ecs_meta_next(&cur));
    test_int(0, ecs_meta_set_int(&cur, 30));
    test_int(0, ecs_meta_pop(&cur));

    test_int(v.count, 3);
    test_int(v.array[0].value, 10);
    test_int(v.array[1].value, 20);
    test_int(v.array[2].value, 30);
    
    ecs_os_free(v.array);

    ecs_fini(world);
}

typedef struct {
    int32_t _dummy_1;
    int32_t x;
    int32_t _dummy_2;
    int32_t y;
} OpaqueStruct;

static void* OpaqueStruct_member(void *ptr, const char *member) {
    OpaqueStruct *data = ptr;
    if (!strcmp(member, "x")) {
        return &data->x;
    } else if (!strcmp(member, "y")) {
        return &data->y;
    } else {
        return NULL;
    }
}

void Cursor_opaque_set_struct(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, OpaqueStruct);

    ecs_opaque(world, {
        .entity = ecs_id(OpaqueStruct),
        .type.as_type = ecs_struct(world, {
            .members = {
                {"x", .type = ecs_id(ecs_i32_t)},
                {"y", .type = ecs_id(ecs_i32_t)}
            }
        }),
        .type.ensure_member = OpaqueStruct_member
    });

    OpaqueStruct v = { 0 };
    ecs_meta_cursor_t cur = ecs_meta_cursor(world, ecs_id(OpaqueStruct), &v);
    test_int(0, ecs_meta_push(&cur));
    test_int(0, ecs_meta_member(&cur, "x"));
    test_int(0, ecs_meta_set_int(&cur, 10));
    test_int(0, ecs_meta_member(&cur, "y"));
    test_int(0, ecs_meta_set_int(&cur, 20));
    test_int(0, ecs_meta_pop(&cur));
    test_int(v.x, 10);
    test_int(v.y, 20);

    cur = ecs_meta_cursor(world, ecs_id(OpaqueStruct), &v);
    test_int(0, ecs_meta_push(&cur));
    test_int(0, ecs_meta_member(&cur, "y"));
    test_int(0, ecs_meta_set_int(&cur, 30));
    test_int(0, ecs_meta_member(&cur, "x"));
    test_int(0, ecs_meta_set_int(&cur, 40));
    test_int(0, ecs_meta_pop(&cur));
    test_int(v.x, 40);
    test_int(v.y, 30);

    ecs_fini(world);
}

typedef struct {
    int32_t _dummy_1;
    Position start;
    int32_t _dummy_2;
    Position stop;
} OpaqueNested;

static void* OpaqueNested_member(void *ptr, const char *member) {
    OpaqueNested *data = ptr;
    if (!strcmp(member, "start")) {
        return &data->start;
    } else if (!strcmp(member, "stop")) {
        return &data->stop;
    } else {
        return NULL;
    }
}

void Cursor_opaque_set_nested_struct(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, OpaqueNested);

    ecs_struct(world, {
        .entity = ecs_id(Position),
        .members = {
            {"x", .type = ecs_id(ecs_i32_t)},
            {"y", .type = ecs_id(ecs_i32_t)}
        }
    });

    ecs_opaque(world, {
        .entity = ecs_id(OpaqueNested),
        .type.as_type = ecs_struct(world, {
            .members = {
                {"start", .type = ecs_id(Position)},
                {"stop", .type = ecs_id(Position)}
            }
        }),
        .type.ensure_member = OpaqueNested_member
    });

    OpaqueNested v = { 0 };
    ecs_meta_cursor_t cur = ecs_meta_cursor(world, ecs_id(OpaqueNested), &v);
    test_int(0, ecs_meta_push(&cur));
        test_int(0, ecs_meta_member(&cur, "start"));
        test_int(0, ecs_meta_push(&cur));
            test_int(0, ecs_meta_member(&cur, "x"));
            test_int(0, ecs_meta_set_int(&cur, 10));
            test_int(0, ecs_meta_member(&cur, "y"));
            test_int(0, ecs_meta_set_int(&cur, 20));
        test_int(0, ecs_meta_pop(&cur));
        test_int(0, ecs_meta_member(&cur, "stop"));
        test_int(0, ecs_meta_push(&cur));
            test_int(0, ecs_meta_member(&cur, "x"));
            test_int(0, ecs_meta_set_int(&cur, 30));
            test_int(0, ecs_meta_member(&cur, "y"));
            test_int(0, ecs_meta_set_int(&cur, 40));
        test_int(0, ecs_meta_pop(&cur));
    test_int(0, ecs_meta_pop(&cur));

    test_int(v.start.x, 10);
    test_int(v.start.y, 20);
    test_int(v.stop.x, 30);
    test_int(v.stop.y, 40);

    cur = ecs_meta_cursor(world, ecs_id(OpaqueNested), &v);
    test_int(0, ecs_meta_push(&cur));
        test_int(0, ecs_meta_member(&cur, "stop"));
        test_int(0, ecs_meta_push(&cur));
            test_int(0, ecs_meta_member(&cur, "y"));
            test_int(0, ecs_meta_set_int(&cur, 50));
            test_int(0, ecs_meta_member(&cur, "x"));
            test_int(0, ecs_meta_set_int(&cur, 60));
        test_int(0, ecs_meta_pop(&cur));
        test_int(0, ecs_meta_member(&cur, "start"));
        test_int(0, ecs_meta_push(&cur));
            test_int(0, ecs_meta_member(&cur, "y"));
            test_int(0, ecs_meta_set_int(&cur, 70));
            test_int(0, ecs_meta_member(&cur, "x"));
            test_int(0, ecs_meta_set_int(&cur, 80));
        test_int(0, ecs_meta_pop(&cur));
    test_int(0, ecs_meta_pop(&cur));

    test_int(v.start.x, 80);
    test_int(v.start.y, 70);
    test_int(v.stop.x, 60);
    test_int(v.stop.y, 50);

    ecs_fini(world);
}

typedef struct {
    int32_t _dummy_1;
    OpaqueStruct start;
    int32_t _dummy_2;
    OpaqueStruct stop;
} OpaqueNestedOpaque;

static void* OpaqueNestedOpaque_member(void *ptr, const char *member) {
    OpaqueNestedOpaque *data = ptr;
    if (!strcmp(member, "start")) {
        return &data->start;
    } else if (!strcmp(member, "stop")) {
        return &data->stop;
    } else {
        return NULL;
    }
}

void Cursor_opaque_set_nested_opaque_struct(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, OpaqueStruct);
    ECS_COMPONENT(world, OpaqueNestedOpaque);

    ecs_opaque(world, {
        .entity = ecs_id(OpaqueStruct),
        .type.as_type = ecs_struct(world, {
            .members = {
                {"x", .type = ecs_id(ecs_i32_t)},
                {"y", .type = ecs_id(ecs_i32_t)}
            }
        }),
        .type.ensure_member = OpaqueStruct_member
    });

    ecs_opaque(world, {
        .entity = ecs_id(OpaqueNestedOpaque),
        .type.as_type = ecs_struct(world, {
            .members = {
                {"start", .type = ecs_id(OpaqueStruct)},
                {"stop", .type = ecs_id(OpaqueStruct)}
            }
        }),
        .type.ensure_member = OpaqueNestedOpaque_member
    });

    OpaqueNestedOpaque v = { 0 };
    ecs_meta_cursor_t cur = ecs_meta_cursor(world, ecs_id(OpaqueNestedOpaque), &v);
    test_int(0, ecs_meta_push(&cur));
        test_int(0, ecs_meta_member(&cur, "start"));
        test_int(0, ecs_meta_push(&cur));
            test_int(0, ecs_meta_member(&cur, "x"));
            test_int(0, ecs_meta_set_int(&cur, 10));
            test_int(0, ecs_meta_member(&cur, "y"));
            test_int(0, ecs_meta_set_int(&cur, 20));
        test_int(0, ecs_meta_pop(&cur));
        test_int(0, ecs_meta_member(&cur, "stop"));
        test_int(0, ecs_meta_push(&cur));
            test_int(0, ecs_meta_member(&cur, "x"));
            test_int(0, ecs_meta_set_int(&cur, 30));
            test_int(0, ecs_meta_member(&cur, "y"));
            test_int(0, ecs_meta_set_int(&cur, 40));
        test_int(0, ecs_meta_pop(&cur));
    test_int(0, ecs_meta_pop(&cur));

    test_int(v.start.x, 10);
    test_int(v.start.y, 20);
    test_int(v.stop.x, 30);
    test_int(v.stop.y, 40);

    cur = ecs_meta_cursor(world, ecs_id(OpaqueNestedOpaque), &v);
    test_int(0, ecs_meta_push(&cur));
        test_int(0, ecs_meta_member(&cur, "stop"));
        test_int(0, ecs_meta_push(&cur));
            test_int(0, ecs_meta_member(&cur, "y"));
            test_int(0, ecs_meta_set_int(&cur, 50));
            test_int(0, ecs_meta_member(&cur, "x"));
            test_int(0, ecs_meta_set_int(&cur, 60));
        test_int(0, ecs_meta_pop(&cur));
        test_int(0, ecs_meta_member(&cur, "start"));
        test_int(0, ecs_meta_push(&cur));
            test_int(0, ecs_meta_member(&cur, "y"));
            test_int(0, ecs_meta_set_int(&cur, 70));
            test_int(0, ecs_meta_member(&cur, "x"));
            test_int(0, ecs_meta_set_int(&cur, 80));
        test_int(0, ecs_meta_pop(&cur));
    test_int(0, ecs_meta_pop(&cur));

    test_int(v.start.x, 80);
    test_int(v.start.y, 70);
    test_int(v.stop.x, 60);
    test_int(v.stop.y, 50);

    ecs_fini(world);
}

typedef struct {
    int32_t el_1;
    int32_t _dummy_1;
    int32_t el_2;
    int32_t _dummy_2;
    int32_t el_3;
    int32_t _dummy_3;
} OpaqueArray;

static void* OpaqueArray_ensure(void *ptr, size_t index) {
    OpaqueArray *data = ptr;
    switch(index) {
    case 0: return &data->el_1;
    case 1: return &data->el_2;
    case 2: return &data->el_3;
    default:
        break;
    }
    return NULL;
}

void Cursor_opaque_set_array(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, OpaqueArray);

    ecs_opaque(world, {
        .entity = ecs_id(OpaqueArray),
        .type.as_type = ecs_array(world, { .type = ecs_id(ecs_i32_t), .count = 3 }),
        .type.ensure_element = OpaqueArray_ensure
    });

    OpaqueArray v = { 0 };
    ecs_meta_cursor_t cur = ecs_meta_cursor(world, ecs_id(OpaqueArray), &v);
    test_int(0, ecs_meta_push(&cur));
    test_int(0, ecs_meta_set_int(&cur, 10));
    test_int(0, ecs_meta_next(&cur));
    test_int(0, ecs_meta_set_int(&cur, 20));
    test_int(0, ecs_meta_next(&cur));
    test_int(0, ecs_meta_set_int(&cur, 30));
    test_int(0, ecs_meta_pop(&cur));

    test_int(v.el_1, 10);
    test_int(v.el_2, 20);
    test_int(v.el_3, 30);

    ecs_fini(world);
}

void Cursor_opaque_set_array_2_of_3_elems(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, OpaqueArray);

    ecs_opaque(world, {
        .entity = ecs_id(OpaqueArray),
        .type.as_type = ecs_array(world, { .type = ecs_id(ecs_i32_t), .count = 3 }),
        .type.ensure_element = OpaqueArray_ensure
    });

    OpaqueArray v = { 0 };
    ecs_meta_cursor_t cur = ecs_meta_cursor(world, ecs_id(OpaqueArray), &v);
    test_int(0, ecs_meta_push(&cur));
    test_int(0, ecs_meta_set_int(&cur, 10));
    test_int(0, ecs_meta_next(&cur));
    test_int(0, ecs_meta_set_int(&cur, 20));
    test_int(0, ecs_meta_pop(&cur));

    test_int(v.el_1, 10);
    test_int(v.el_2, 20);
    test_int(v.el_3, 0);

    ecs_fini(world);
}

void Cursor_opaque_set_array_empty(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, OpaqueArray);

    ecs_opaque(world, {
        .entity = ecs_id(OpaqueArray),
        .type.as_type = ecs_array(world, { .type = ecs_id(ecs_i32_t), .count = 3 }),
        .type.ensure_element = OpaqueArray_ensure
    });

    OpaqueArray v = { 0 };
    ecs_meta_cursor_t cur = ecs_meta_cursor(world, ecs_id(OpaqueArray), &v);
    test_int(0, ecs_meta_push(&cur));
    test_int(0, ecs_meta_pop(&cur));

    test_int(v.el_1, 0);
    test_int(v.el_2, 0);
    test_int(v.el_3, 0);

    ecs_fini(world);
}

void Cursor_opaque_set_uint_to_int(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Opaque_int64_t);

    ecs_opaque(world, {
        .entity = ecs_id(Opaque_int64_t),
        .type.as_type = ecs_id(ecs_i64_t),
        .type.assign_int = int64_t_set
    });

    Opaque_int64_t v = { 0 };

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, ecs_id(Opaque_int64_t), &v);
    test_int(0, ecs_meta_set_uint(&cur, 10));
    test_int(v.value, 10);
    test_int(0, ecs_meta_set_uint(&cur, 100));
    test_int(v.value, 100);

    ecs_log_set_level(-4);
    test_int(-1, ecs_meta_set_uint(&cur, (uint64_t)INT64_MAX + 1));
    test_int(v.value, 100);

    ecs_fini(world);
}

void Cursor_opaque_set_float_to_int(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Opaque_int64_t);

    ecs_opaque(world, {
        .entity = ecs_id(Opaque_int64_t),
        .type.as_type = ecs_id(ecs_i64_t),
        .type.assign_int = int64_t_set
    });

    Opaque_int64_t v = { 0 };

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, ecs_id(Opaque_int64_t), &v);
    test_int(0, ecs_meta_set_float(&cur, 10));
    test_int(v.value, 10);
    test_int(0, ecs_meta_set_float(&cur, 100));
    test_int(v.value, 100);

    ecs_log_set_level(-4);
    test_int(-1, ecs_meta_set_float(&cur, (double)INT64_MAX + 10000));
    test_int(v.value, 100);

    ecs_fini(world);
}

void Cursor_opaque_set_int_to_uint(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Opaque_uint64_t);

    ecs_opaque(world, {
        .entity = ecs_id(Opaque_uint64_t),
        .type.as_type = ecs_id(ecs_u64_t),
        .type.assign_uint = uint64_t_set
    });

    Opaque_uint64_t v = { 0 };

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, ecs_id(Opaque_uint64_t), &v);
    test_int(0, ecs_meta_set_int(&cur, 10));
    test_int(v.value, 10);
    test_int(0, ecs_meta_set_int(&cur, 100));
    test_int(v.value, 100);

    ecs_log_set_level(-4);
    test_int(-1, ecs_meta_set_int(&cur, -1));
    test_int(v.value, 100);

    ecs_fini(world);
}

void Cursor_opaque_set_float_to_uint(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Opaque_uint64_t);

    ecs_opaque(world, {
        .entity = ecs_id(Opaque_uint64_t),
        .type.as_type = ecs_id(ecs_u64_t),
        .type.assign_uint = uint64_t_set
    });

    Opaque_uint64_t v = { 0 };

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, ecs_id(Opaque_uint64_t), &v);
    test_int(0, ecs_meta_set_float(&cur, 10));
    test_int(v.value, 10);
    test_int(0, ecs_meta_set_float(&cur, 100));
    test_int(v.value, 100);

    ecs_log_set_level(-4);
    test_int(-1, ecs_meta_set_float(&cur, -1));
    test_int(v.value, 100);

    ecs_fini(world);
}

void Cursor_opaque_set_int_to_float(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Opaque_double);

    ecs_opaque(world, {
        .entity = ecs_id(Opaque_double),
        .type.as_type = ecs_id(ecs_f64_t),
        .type.assign_float = double_set
    });

    Opaque_double v = { 0 };

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, ecs_id(Opaque_double), &v);
    test_int(0, ecs_meta_set_int(&cur, 10));
    test_int(v.value, 10);
    test_int(0, ecs_meta_set_int(&cur, 100));
    test_int(v.value, 100);

    ecs_fini(world);
}

void Cursor_opaque_set_uint_to_float(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Opaque_double);

    ecs_opaque(world, {
        .entity = ecs_id(Opaque_double),
        .type.as_type = ecs_id(ecs_f64_t),
        .type.assign_float = double_set
    });

    Opaque_double v = { 0 };

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, ecs_id(Opaque_double), &v);
    test_int(0, ecs_meta_set_uint(&cur, 10));
    test_int(v.value, 10);
    test_int(0, ecs_meta_set_uint(&cur, 100));
    test_int(v.value, 100);

    ecs_fini(world);
}

void Cursor_opaque_set_string_to_char(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Opaque_char);

    ecs_opaque(world, {
        .entity = ecs_id(Opaque_char),
        .type.as_type = ecs_id(ecs_char_t),
        .type.assign_char = char_set
    });

    Opaque_char v = { 0 };

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, ecs_id(Opaque_char), &v);
    test_int(0, ecs_meta_set_string(&cur, "a"));
    test_int(v.value, 'a');
    test_int(0, ecs_meta_set_string(&cur, "b"));
    test_int(v.value, 'b');

    ecs_log_set_level(-4);
    test_int(-1, ecs_meta_set_string(&cur, "ab"));
    test_int(v.value, 'b');

    ecs_fini(world);
}

void Cursor_opaque_set_int_to_char(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Opaque_char);

    ecs_opaque(world, {
        .entity = ecs_id(Opaque_char),
        .type.as_type = ecs_id(ecs_char_t),
        .type.assign_char = char_set
    });

    Opaque_char v = { 0 };

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, ecs_id(Opaque_char), &v);
    test_int(0, ecs_meta_set_int(&cur, 10));
    test_int(v.value, 10);
    test_int(0, ecs_meta_set_int(&cur, 100));
    test_int(v.value, 100);

    ecs_log_set_level(-4);
    test_int(-1, ecs_meta_set_int(&cur, 1000));
    test_int(v.value, 100);
    test_int(-1, ecs_meta_set_int(&cur, -10));
    test_int(v.value, 100);

    ecs_fini(world);
}

void Cursor_opaque_set_uint_to_char(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Opaque_char);

    ecs_opaque(world, {
        .entity = ecs_id(Opaque_char),
        .type.as_type = ecs_id(ecs_char_t),
        .type.assign_char = char_set
    });

    Opaque_char v = { 0 };

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, ecs_id(Opaque_char), &v);
    test_int(0, ecs_meta_set_uint(&cur, 10));
    test_int(v.value, 10);
    test_int(0, ecs_meta_set_uint(&cur, 100));
    test_int(v.value, 100);

    ecs_log_set_level(-4);
    test_int(-1, ecs_meta_set_uint(&cur, 1000));
    test_int(v.value, 100);

    ecs_fini(world);
}

void Cursor_opaque_set_char_to_int(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Opaque_int64_t);

    ecs_opaque(world, {
        .entity = ecs_id(Opaque_int64_t),
        .type.as_type = ecs_id(ecs_i64_t),
        .type.assign_int = int64_t_set
    });

    Opaque_int64_t v = { 0 };

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, ecs_id(Opaque_int64_t), &v);
    test_int(0, ecs_meta_set_char(&cur, 'a'));
    test_int(v.value, 'a');
    test_int(0, ecs_meta_set_char(&cur, 'b'));
    test_int(v.value, 'b');

    ecs_fini(world);
}

void Cursor_opaque_set_char_to_uint(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Opaque_uint64_t);

    ecs_opaque(world, {
        .entity = ecs_id(Opaque_uint64_t),
        .type.as_type = ecs_id(ecs_u64_t),
        .type.assign_uint = uint64_t_set
    });

    Opaque_uint64_t v = { 0 };

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, ecs_id(Opaque_uint64_t), &v);
    test_int(0, ecs_meta_set_char(&cur, 'a'));
    test_int(v.value, 'a');
    test_int(0, ecs_meta_set_char(&cur, 'b'));
    test_int(v.value, 'b');

    ecs_fini(world);
}

typedef struct {
    OpaqueStruct foo;
    OpaqueStruct bar;
} Struct_w_OpaqueStruct;

void Cursor_struct_w_2_opaque_structs(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, OpaqueStruct);
    ECS_COMPONENT(world, Struct_w_OpaqueStruct);

    ecs_opaque(world, {
        .entity = ecs_id(OpaqueStruct),
        .type.as_type = ecs_struct(world, {
            .members = {
                {"x", .type = ecs_id(ecs_i32_t)},
                {"y", .type = ecs_id(ecs_i32_t)}
            }
        }),
        .type.ensure_member = OpaqueStruct_member
    });

    ecs_struct(world, {
        .entity = ecs_id(Struct_w_OpaqueStruct),
        .members = {
            {"foo", ecs_id(OpaqueStruct)},
            {"bar", ecs_id(OpaqueStruct)},
        }
    });

    Struct_w_OpaqueStruct v = {0};
    ecs_meta_cursor_t cur = ecs_meta_cursor(world, ecs_id(Struct_w_OpaqueStruct), &v);
    test_int(0, ecs_meta_push(&cur));
        test_int(0, ecs_meta_member(&cur, "foo"));
        test_int(0, ecs_meta_push(&cur));
            test_int(0, ecs_meta_member(&cur, "x"));
            test_int(0, ecs_meta_set_int(&cur, 10));
            test_int(0, ecs_meta_member(&cur, "y"));
            test_int(0, ecs_meta_set_int(&cur, 20));
        test_int(0, ecs_meta_pop(&cur));
        test_int(0, ecs_meta_member(&cur, "bar"));
        test_int(0, ecs_meta_push(&cur));
            test_int(0, ecs_meta_member(&cur, "x"));
            test_int(0, ecs_meta_set_int(&cur, 30));
            test_int(0, ecs_meta_member(&cur, "y"));
            test_int(0, ecs_meta_set_int(&cur, 40));
        test_int(0, ecs_meta_pop(&cur));
    test_int(0, ecs_meta_pop(&cur));

    test_int(v.foo.x, 10);
    test_int(v.foo.y, 20);
    test_int(v.bar.x, 30);
    test_int(v.bar.y, 40);

    ecs_fini(world);
}

typedef struct {
    IntVec foo;
    IntVec bar;
} Struct_w_IntVec;

void Cursor_struct_w_2_opaque_vectors(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, IntVec);
    ECS_COMPONENT(world, Struct_w_IntVec);

    ecs_opaque(world, {
        .entity = ecs_id(IntVec),
        .type.as_type = ecs_vector(world, { .type = ecs_id(ecs_i32_t) }),
        .type.ensure_element = IntVec_ensure,
        .type.count = IntVec_count,
        .type.resize = IntVec_resize
    });

    ecs_struct(world, {
        .entity = ecs_id(Struct_w_IntVec),
        .members = {
            {"foo", ecs_id(IntVec)},
            {"bar", ecs_id(IntVec)},
        }
    });

    Struct_w_IntVec v = {0};
    ecs_meta_cursor_t cur = ecs_meta_cursor(world, ecs_id(Struct_w_IntVec), &v);

    test_int(0, ecs_meta_push(&cur));
        test_int(0, ecs_meta_member(&cur, "foo"));
        test_int(0, ecs_meta_push(&cur));
            test_int(0, ecs_meta_set_int(&cur, 10));
            test_int(0, ecs_meta_next(&cur));
            test_int(0, ecs_meta_set_int(&cur, 20));
            test_int(0, ecs_meta_next(&cur));
            test_int(0, ecs_meta_set_int(&cur, 30));
        test_int(0, ecs_meta_pop(&cur));
        test_int(0, ecs_meta_member(&cur, "bar"));
        test_int(0, ecs_meta_push(&cur));
            test_int(0, ecs_meta_set_int(&cur, 40));
            test_int(0, ecs_meta_next(&cur));
            test_int(0, ecs_meta_set_int(&cur, 50));
            test_int(0, ecs_meta_next(&cur));
            test_int(0, ecs_meta_set_int(&cur, 60));
        test_int(0, ecs_meta_pop(&cur));
    test_int(0, ecs_meta_pop(&cur));

    test_int(v.foo.count, 3);
    test_int(v.foo.array[0], 10);
    test_int(v.foo.array[1], 20);
    test_int(v.foo.array[2], 30);

    test_int(v.bar.count, 3);
    test_int(v.bar.array[0], 40);
    test_int(v.bar.array[1], 50);
    test_int(v.bar.array[2], 60);

    ecs_os_free(v.foo.array);
    ecs_os_free(v.bar.array);

    ecs_fini(world);
}

typedef struct {
    OpaqueArray foo;
    OpaqueArray bar;
} Struct_w_OpaqueArray;

void Cursor_struct_w_2_opaque_arrays(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, OpaqueArray);
    ECS_COMPONENT(world, Struct_w_OpaqueArray);

    ecs_opaque(world, {
        .entity = ecs_id(OpaqueArray),
        .type.as_type = ecs_array(world, { .type = ecs_id(ecs_i32_t), .count = 3 }),
        .type.ensure_element = OpaqueArray_ensure
    });

    ecs_struct(world, {
        .entity = ecs_id(Struct_w_OpaqueArray),
        .members = {
            {"foo", ecs_id(OpaqueArray)},
            {"bar", ecs_id(OpaqueArray)},
        }
    });

    Struct_w_OpaqueArray v = {0};
    ecs_meta_cursor_t cur = ecs_meta_cursor(world, ecs_id(Struct_w_OpaqueArray), &v);

    test_int(0, ecs_meta_push(&cur));
        test_int(0, ecs_meta_member(&cur, "foo"));
        test_int(0, ecs_meta_push(&cur));
            test_int(0, ecs_meta_set_int(&cur, 10));
            test_int(0, ecs_meta_next(&cur));
            test_int(0, ecs_meta_set_int(&cur, 20));
            test_int(0, ecs_meta_next(&cur));
            test_int(0, ecs_meta_set_int(&cur, 30));
        test_int(0, ecs_meta_pop(&cur));
        test_int(0, ecs_meta_member(&cur, "bar"));
        test_int(0, ecs_meta_push(&cur));
            test_int(0, ecs_meta_set_int(&cur, 40));
            test_int(0, ecs_meta_next(&cur));
            test_int(0, ecs_meta_set_int(&cur, 50));
            test_int(0, ecs_meta_next(&cur));
            test_int(0, ecs_meta_set_int(&cur, 60));
        test_int(0, ecs_meta_pop(&cur));
    test_int(0, ecs_meta_pop(&cur));

    test_int(v.foo.el_1, 10);
    test_int(v.foo.el_2, 20);
    test_int(v.foo.el_3, 30);

    test_int(v.bar.el_1, 40);
    test_int(v.bar.el_2, 50);
    test_int(v.bar.el_3, 60);

    ecs_fini(world);
}

typedef struct {
    OpaqueStruct foo;
    OpaqueStruct bar;
    OpaqueStruct zoo;
} Struct_w_3_OpaqueStructs;

void Cursor_struct_w_3_opaque_structs(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, OpaqueStruct);
    ECS_COMPONENT(world, Struct_w_3_OpaqueStructs);

    ecs_opaque(world, {
        .entity = ecs_id(OpaqueStruct),
        .type.as_type = ecs_struct(world, {
            .members = {
                {"x", .type = ecs_id(ecs_i32_t)},
                {"y", .type = ecs_id(ecs_i32_t)}
            }
        }),
        .type.ensure_member = OpaqueStruct_member
    });

    ecs_struct(world, {
        .entity = ecs_id(Struct_w_3_OpaqueStructs),
        .members = {
            {"foo", ecs_id(OpaqueStruct)},
            {"bar", ecs_id(OpaqueStruct)},
            {"zoo", ecs_id(OpaqueStruct)},
        }
    });

    Struct_w_3_OpaqueStructs v = {0};
    ecs_meta_cursor_t cur = ecs_meta_cursor(world, ecs_id(Struct_w_3_OpaqueStructs), &v);
    test_int(0, ecs_meta_push(&cur));
        test_int(0, ecs_meta_member(&cur, "foo"));
        test_int(0, ecs_meta_push(&cur));
            test_int(0, ecs_meta_member(&cur, "x"));
            test_int(0, ecs_meta_set_int(&cur, 10));
            test_int(0, ecs_meta_member(&cur, "y"));
            test_int(0, ecs_meta_set_int(&cur, 20));
        test_int(0, ecs_meta_pop(&cur));
        test_int(0, ecs_meta_member(&cur, "bar"));
        test_int(0, ecs_meta_push(&cur));
            test_int(0, ecs_meta_member(&cur, "x"));
            test_int(0, ecs_meta_set_int(&cur, 30));
            test_int(0, ecs_meta_member(&cur, "y"));
            test_int(0, ecs_meta_set_int(&cur, 40));
        test_int(0, ecs_meta_pop(&cur));
        test_int(0, ecs_meta_member(&cur, "zoo"));
        test_int(0, ecs_meta_push(&cur));
            test_int(0, ecs_meta_member(&cur, "x"));
            test_int(0, ecs_meta_set_int(&cur, 50));
            test_int(0, ecs_meta_member(&cur, "y"));
            test_int(0, ecs_meta_set_int(&cur, 60));
        test_int(0, ecs_meta_pop(&cur));
    test_int(0, ecs_meta_pop(&cur));

    test_int(v.foo.x, 10);
    test_int(v.foo.y, 20);
    test_int(v.bar.x, 30);
    test_int(v.bar.y, 40);
    test_int(v.zoo.x, 50);
    test_int(v.zoo.y, 60);

    ecs_fini(world);
}

typedef struct {
    IntVec foo;
    IntVec bar;
    IntVec zoo;
} Struct_w_3_IntVecs;

void Cursor_struct_w_3_opaque_vectors(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, IntVec);
    ECS_COMPONENT(world, Struct_w_3_IntVecs);

    ecs_opaque(world, {
        .entity = ecs_id(IntVec),
        .type.as_type = ecs_vector(world, { .type = ecs_id(ecs_i32_t) }),
        .type.ensure_element = IntVec_ensure,
        .type.count = IntVec_count,
        .type.resize = IntVec_resize
    });

    ecs_struct(world, {
        .entity = ecs_id(Struct_w_3_IntVecs),
        .members = {
            {"foo", ecs_id(IntVec)},
            {"bar", ecs_id(IntVec)},
            {"zoo", ecs_id(IntVec)},
        }
    });

    Struct_w_3_IntVecs v = {0};
    ecs_meta_cursor_t cur = ecs_meta_cursor(world, ecs_id(Struct_w_3_IntVecs), &v);

    test_int(0, ecs_meta_push(&cur));
        test_int(0, ecs_meta_member(&cur, "foo"));
        test_int(0, ecs_meta_push(&cur));
            test_int(0, ecs_meta_set_int(&cur, 10));
            test_int(0, ecs_meta_next(&cur));
            test_int(0, ecs_meta_set_int(&cur, 20));
            test_int(0, ecs_meta_next(&cur));
            test_int(0, ecs_meta_set_int(&cur, 30));
        test_int(0, ecs_meta_pop(&cur));
        test_int(0, ecs_meta_member(&cur, "bar"));
        test_int(0, ecs_meta_push(&cur));
            test_int(0, ecs_meta_set_int(&cur, 40));
            test_int(0, ecs_meta_next(&cur));
            test_int(0, ecs_meta_set_int(&cur, 50));
            test_int(0, ecs_meta_next(&cur));
            test_int(0, ecs_meta_set_int(&cur, 60));
        test_int(0, ecs_meta_pop(&cur));
        test_int(0, ecs_meta_member(&cur, "zoo"));
        test_int(0, ecs_meta_push(&cur));
            test_int(0, ecs_meta_set_int(&cur, 70));
            test_int(0, ecs_meta_next(&cur));
            test_int(0, ecs_meta_set_int(&cur, 80));
            test_int(0, ecs_meta_next(&cur));
            test_int(0, ecs_meta_set_int(&cur, 90));
        test_int(0, ecs_meta_pop(&cur));
    test_int(0, ecs_meta_pop(&cur));

    test_int(v.foo.count, 3);
    test_int(v.foo.array[0], 10);
    test_int(v.foo.array[1], 20);
    test_int(v.foo.array[2], 30);

    test_int(v.bar.count, 3);
    test_int(v.bar.array[0], 40);
    test_int(v.bar.array[1], 50);
    test_int(v.bar.array[2], 60);

    test_int(v.zoo.count, 3);
    test_int(v.zoo.array[0], 70);
    test_int(v.zoo.array[1], 80);
    test_int(v.zoo.array[2], 90);
    
    ecs_os_free(v.foo.array);
    ecs_os_free(v.bar.array);
    ecs_os_free(v.zoo.array);

    ecs_fini(world);
}

typedef struct {
    OpaqueArray foo;
    OpaqueArray bar;
    OpaqueArray zoo;
} Struct_w_3_OpaqueArrays;

void Cursor_struct_w_3_opaque_arrays(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, OpaqueArray);
    ECS_COMPONENT(world, Struct_w_3_OpaqueArrays);

    ecs_opaque(world, {
        .entity = ecs_id(OpaqueArray),
        .type.as_type = ecs_array(world, { .type = ecs_id(ecs_i32_t), .count = 3 }),
        .type.ensure_element = OpaqueArray_ensure
    });

    ecs_struct(world, {
        .entity = ecs_id(Struct_w_3_OpaqueArrays),
        .members = {
            {"foo", ecs_id(OpaqueArray)},
            {"bar", ecs_id(OpaqueArray)},
            {"zoo", ecs_id(OpaqueArray)},
        }
    });

    Struct_w_3_OpaqueArrays v = {0};
    ecs_meta_cursor_t cur = ecs_meta_cursor(world, ecs_id(Struct_w_3_OpaqueArrays), &v);

    test_int(0, ecs_meta_push(&cur));
        test_int(0, ecs_meta_member(&cur, "foo"));
        test_int(0, ecs_meta_push(&cur));
            test_int(0, ecs_meta_set_int(&cur, 10));
            test_int(0, ecs_meta_next(&cur));
            test_int(0, ecs_meta_set_int(&cur, 20));
            test_int(0, ecs_meta_next(&cur));
            test_int(0, ecs_meta_set_int(&cur, 30));
        test_int(0, ecs_meta_pop(&cur));
        test_int(0, ecs_meta_member(&cur, "bar"));
        test_int(0, ecs_meta_push(&cur));
            test_int(0, ecs_meta_set_int(&cur, 40));
            test_int(0, ecs_meta_next(&cur));
            test_int(0, ecs_meta_set_int(&cur, 50));
            test_int(0, ecs_meta_next(&cur));
            test_int(0, ecs_meta_set_int(&cur, 60));
        test_int(0, ecs_meta_pop(&cur));
        test_int(0, ecs_meta_member(&cur, "zoo"));
        test_int(0, ecs_meta_push(&cur));
            test_int(0, ecs_meta_set_int(&cur, 70));
            test_int(0, ecs_meta_next(&cur));
            test_int(0, ecs_meta_set_int(&cur, 80));
            test_int(0, ecs_meta_next(&cur));
            test_int(0, ecs_meta_set_int(&cur, 90));
        test_int(0, ecs_meta_pop(&cur));
    test_int(0, ecs_meta_pop(&cur));

    test_int(v.foo.el_1, 10);
    test_int(v.foo.el_2, 20);
    test_int(v.foo.el_3, 30);

    test_int(v.bar.el_1, 40);
    test_int(v.bar.el_2, 50);
    test_int(v.bar.el_3, 60);

    test_int(v.zoo.el_1, 70);
    test_int(v.zoo.el_2, 80);
    test_int(v.zoo.el_3, 90);

    ecs_fini(world);
}

static void* OpaqueStructIntVec_member(void *ptr, const char *member) {
    Struct_w_IntVec *data = ptr;
    if (!strcmp(member, "foo")) {
        return &data->foo;
    } else if (!strcmp(member, "bar")) {
        return &data->bar;
    } else {
        return NULL;
    }
}

void Cursor_opaque_struct_w_opaque_vec(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, IntVec);
    ECS_COMPONENT(world, Struct_w_IntVec);

    ecs_opaque(world, {
        .entity = ecs_id(IntVec),
        .type.as_type = ecs_vector(world, { .type = ecs_id(ecs_i32_t) }),
        .type.ensure_element = IntVec_ensure,
        .type.count = IntVec_count,
        .type.resize = IntVec_resize
    });

    ecs_entity_t os = ecs_struct(world, {
        .members = {
            {"foo", ecs_id(IntVec)},
            {"bar", ecs_id(IntVec)},
        }
    });

    ecs_opaque(world, {
        .entity = ecs_id(Struct_w_IntVec),
        .type.as_type = os,
        .type.ensure_member = OpaqueStructIntVec_member,
    });

    Struct_w_IntVec v = {0};
    ecs_meta_cursor_t cur = ecs_meta_cursor(world, ecs_id(Struct_w_IntVec), &v);
    test_int(0, ecs_meta_push(&cur));
        test_int(0, ecs_meta_member(&cur, "foo"));
        test_int(0, ecs_meta_push(&cur));
            test_int(0, ecs_meta_set_int(&cur, 10));
            test_int(0, ecs_meta_next(&cur));
            test_int(0, ecs_meta_set_int(&cur, 20));
            test_int(0, ecs_meta_next(&cur));
            test_int(0, ecs_meta_set_int(&cur, 30));
        test_int(0, ecs_meta_pop(&cur));
        test_int(0, ecs_meta_member(&cur, "bar"));
        test_int(0, ecs_meta_push(&cur));
            test_int(0, ecs_meta_set_int(&cur, 40));
            test_int(0, ecs_meta_next(&cur));
            test_int(0, ecs_meta_set_int(&cur, 50));
            test_int(0, ecs_meta_next(&cur));
            test_int(0, ecs_meta_set_int(&cur, 60));
        test_int(0, ecs_meta_pop(&cur));
    test_int(0, ecs_meta_pop(&cur));

    test_int(v.foo.count, 3);
    test_int(v.foo.array[0], 10);
    test_int(v.foo.array[1], 20);
    test_int(v.foo.array[2], 30);
    test_int(v.bar.array[0], 40);
    test_int(v.bar.array[1], 50);
    test_int(v.bar.array[2], 60);

    ecs_os_free(v.foo.array);
    ecs_os_free(v.bar.array);

    ecs_fini(world);
}

static void* OpaqueStructElem_member(void *ptr, const char *member) {
    if (!strcmp(member, "i")) {
        return ptr;
    } else {
        return NULL;
    }
}

void Cursor_opaque_vec_w_opaque_elem(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, IntVec);
    ECS_COMPONENT(world, Struct_w_IntVec);

    ecs_entity_t os = ecs_struct(world, {
        .members = {
            {"i", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t oelem = ecs_opaque(world, {
        .entity = ecs_component(world, {
            .type.size = ECS_SIZEOF(ecs_i32_t),
            .type.alignment = ECS_ALIGNOF(ecs_i32_t)
        }),
        .type.as_type = os,
        .type.ensure_member = OpaqueStructElem_member,
    });

    ecs_opaque(world, {
        .entity = ecs_id(IntVec),
        .type.as_type = ecs_vector(world, { .type = oelem }),
        .type.ensure_element = IntVec_ensure,
        .type.count = IntVec_count,
        .type.resize = IntVec_resize
    });

    IntVec v = {0};
    ecs_meta_cursor_t cur = ecs_meta_cursor(world, ecs_id(IntVec), &v);
    test_int(0, ecs_meta_push(&cur));
        test_int(0, ecs_meta_push(&cur));
            test_int(0, ecs_meta_member(&cur, "i"));
            test_int(0, ecs_meta_set_int(&cur, 10));
        test_int(0, ecs_meta_pop(&cur));
        test_int(0, ecs_meta_next(&cur));
        test_int(0, ecs_meta_push(&cur));
            test_int(0, ecs_meta_member(&cur, "i"));
            test_int(0, ecs_meta_set_int(&cur, 20));
        test_int(0, ecs_meta_pop(&cur));
        test_int(0, ecs_meta_next(&cur));
        test_int(0, ecs_meta_push(&cur));
            test_int(0, ecs_meta_member(&cur, "i"));
            test_int(0, ecs_meta_set_int(&cur, 30));
        test_int(0, ecs_meta_pop(&cur));
    test_int(0, ecs_meta_pop(&cur));

    test_int(v.count, 3);
    test_int(v.array[0], 10);
    test_int(v.array[1], 20);
    test_int(v.array[2], 30);
    
    ecs_os_free(v.array);

    ecs_fini(world);
}

void Cursor_next_out_of_bounds(void) {
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
    test_ok( ecs_meta_next(&cur) );
    test_ok( ecs_meta_next(&cur) );

    ecs_log_set_level(-4);
    test_fail( ecs_meta_next(&cur) );

    ecs_fini(world);
}

void Cursor_set_out_of_bounds(void) {
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
    test_ok( ecs_meta_next(&cur) );
    test_ok( ecs_meta_next(&cur) );
    ecs_log_set_level(-4);
    test_fail(ecs_meta_set_int(&cur, 10));

    ecs_fini(world);
}

void Cursor_get_member_id(void) {
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
    test_assert(ecs_meta_get_member_id(&cur) != 0);
    test_assert(ecs_meta_get_member_id(&cur) == ecs_lookup(world, "T.x"));
    test_ok( ecs_meta_next(&cur) );
    test_assert(ecs_meta_get_member_id(&cur) != 0);
    test_assert(ecs_meta_get_member_id(&cur) == ecs_lookup(world, "T.y"));
    test_ok( ecs_meta_pop(&cur) );

    ecs_fini(world);
}
