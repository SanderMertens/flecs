#include <script.h>

static ecs_query_cache_kind_t disable_folding = false;

void Deserialize_setup(void) {
    const char *folding_param = test_param("folding");
    if (folding_param) {
        if (!strcmp(folding_param, "disabled")) {
            disable_folding = true;
        } else if (!strcmp(folding_param, "enabled")) {
            // already set to default
        } else {
            printf("unexpected value for folding '%s'\n", folding_param);
        }
    }
}

void Deserialize_bool(void) {
    ecs_world_t *world = ecs_init();

    bool value = false;
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(world, 
        "true", &ecs_value_ptr(ecs_bool_t, &value), &desc);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');        

    test_bool(value, true);

    ecs_fini(world);
}

void Deserialize_byte(void) {
    ecs_world_t *world = ecs_init();

    ecs_byte_t value = 0;
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(world, 
        "10", &ecs_value_ptr(ecs_byte_t, &value), &desc);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');        

    test_int(value, 10);

    ecs_fini(world);
}

void Deserialize_char(void) {
    ecs_world_t *world = ecs_init();

    ecs_char_t value = 0;
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(world, 
        "10", &ecs_value_ptr(ecs_char_t, &value), &desc);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');        

    test_int(value, 10);

    ecs_fini(world);
}

void Deserialize_char_literal(void) {
    ecs_world_t *world = ecs_init();

    ecs_char_t value = 0;
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(world, 
        "\"a\"", &ecs_value_ptr(ecs_char_t, &value), &desc);
    test_assert(ptr != NULL);

    test_int(value, 'a');

    ecs_fini(world);
}

void Deserialize_i8(void) {
    ecs_world_t *world = ecs_init();

    ecs_i8_t value = 0;
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(world, 
        "10", &ecs_value_ptr(ecs_i8_t, &value), &desc);
    test_assert(ptr != NULL);

    test_int(value, 10);

    ecs_fini(world);
}

void Deserialize_i16(void) {
    ecs_world_t *world = ecs_init();

    ecs_i16_t value = 0;
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(world, 
        "10", &ecs_value_ptr(ecs_i16_t, &value), &desc);
    test_assert(ptr != NULL);

    test_int(value, 10);

    ecs_fini(world);
}

void Deserialize_i32(void) {
    ecs_world_t *world = ecs_init();

    ecs_i32_t value = 0;
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(world, 
        "10", &ecs_value_ptr(ecs_i32_t, &value), &desc);
    test_assert(ptr != NULL);

    test_int(value, 10);

    ecs_fini(world);
}

void Deserialize_i64(void) {
    ecs_world_t *world = ecs_init();

    ecs_i64_t value = 0;
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(world, 
        "10", &ecs_value_ptr(ecs_i64_t, &value), &desc);
    test_assert(ptr != NULL);

    test_int(value, 10);

    ecs_fini(world);
}

void Deserialize_iptr(void) {
    ecs_world_t *world = ecs_init();

    ecs_iptr_t value = 0;
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(world, 
        "10", &ecs_value_ptr(ecs_iptr_t, &value), &desc);
    test_assert(ptr != NULL);

    test_int(value, 10);

    ecs_fini(world);
}

void Deserialize_u8(void) {
    ecs_world_t *world = ecs_init();

    ecs_u8_t value = 0;
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(world, 
        "10", &ecs_value_ptr(ecs_u8_t, &value), &desc);
    test_assert(ptr != NULL);

    test_int(value, 10);

    ecs_fini(world);
}

void Deserialize_u16(void) {
    ecs_world_t *world = ecs_init();

    ecs_u16_t value = 0;
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(world, 
        "10", &ecs_value_ptr(ecs_u16_t, &value), &desc);
    test_assert(ptr != NULL);

    test_int(value, 10);

    ecs_fini(world);
}

void Deserialize_u32(void) {
    ecs_world_t *world = ecs_init();

    ecs_u32_t value = 0;
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(world, 
        "10", &ecs_value_ptr(ecs_u32_t, &value), &desc);
    test_assert(ptr != NULL);

    test_int(value, 10);

    ecs_fini(world);
}

void Deserialize_u64(void) {
    ecs_world_t *world = ecs_init();

    {
        ecs_u64_t value = 0;
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
        const char *ptr = ecs_expr_run(
            world, "0", &ecs_value_ptr(ecs_u64_t, &value), &desc);
        test_assert(ptr != NULL);
        test_int(value, 0);
    }

    {
        ecs_u64_t value = 0;
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
        const char *ptr = ecs_expr_run(
            world, "10", &ecs_value_ptr(ecs_u64_t, &value), &desc);
        test_assert(ptr != NULL);
        test_int(value, 10);
    }

    {
        ecs_u64_t value = 0;
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
        const char *ptr = ecs_expr_run(
            world, "2366700781656087864", &ecs_value_ptr(ecs_u64_t, &value), &desc);
        test_assert(ptr != NULL);
        test_int(value, 2366700781656087864);
    }

    ecs_fini(world);
}

void Deserialize_uptr(void) {
    ecs_world_t *world = ecs_init();

    ecs_uptr_t value = 0;
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(world, 
        "10", &ecs_value_ptr(ecs_uptr_t, &value), &desc);
    test_assert(ptr != NULL);

    test_int(value, 10);

    ecs_fini(world);
}

void Deserialize_float(void) {
    ecs_world_t *world = ecs_init();

    ecs_f32_t value = 0;
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(world, 
        "10.5", &ecs_value_ptr(ecs_f32_t, &value), &desc);
    test_assert(ptr != NULL);

    test_flt(value, 10.5);

    ecs_fini(world);
}

void Deserialize_double(void) {
    ecs_world_t *world = ecs_init();

    ecs_f64_t value = 0;
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(world, 
        "10.5", &ecs_value_ptr(ecs_f64_t, &value), &desc);
    test_assert(ptr != NULL);

    test_flt(value, 10.5);

    ecs_fini(world);
}

void Deserialize_negative_int(void) {
    ecs_world_t *world = ecs_init();

    ecs_i8_t value = 0;
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(world, 
        "-10", &ecs_value_ptr(ecs_i8_t, &value), &desc);
    test_assert(ptr != NULL);

    test_int(value, -10);

    ecs_fini(world);
}

void Deserialize_negative_float(void) {
    ecs_world_t *world = ecs_init();

    ecs_f32_t value = 0;
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(world, 
        "-10.5", &ecs_value_ptr(ecs_f32_t, &value), &desc);
    test_assert(ptr != NULL);

    test_flt(value, -10.5);

    ecs_fini(world);
}

void Deserialize_invalid_i8(void) {
    ecs_world_t *world = ecs_init();

    uint64_t value = 0;

    ecs_log_set_level(-4);
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(
        world, "a", &ecs_value_ptr(ecs_i8_t, &value), &desc);
    test_assert(ptr == NULL);

    ecs_fini(world);
}

void Deserialize_invalid_i16(void) {
    ecs_world_t *world = ecs_init();

    uint64_t value = 0;

    ecs_log_set_level(-4);
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(
        world, "a", &ecs_value_ptr(ecs_i16_t, &value), &desc);
    test_assert(ptr == NULL);

    ecs_fini(world);
}

void Deserialize_invalid_i32(void) {
    ecs_world_t *world = ecs_init();

    uint64_t value = 0;

    ecs_log_set_level(-4);
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(
        world, "a", &ecs_value_ptr(ecs_i32_t, &value), &desc);
    test_assert(ptr == NULL);

    ecs_fini(world);
}

void Deserialize_invalid_i64(void) {
    ecs_world_t *world = ecs_init();

    uint64_t value = 0;

    ecs_log_set_level(-4);
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(
        world, "a", &ecs_value_ptr(ecs_i64_t, &value), &desc);
    test_assert(ptr == NULL);

    ecs_fini(world);
}

void Deserialize_invalid_iptr(void) {
    ecs_world_t *world = ecs_init();

    uint64_t value = 0;

    ecs_log_set_level(-4);
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(
        world, "a", &ecs_value_ptr(ecs_iptr_t, &value), &desc);
    test_assert(ptr == NULL);

    ecs_fini(world);
}

void Deserialize_invalid_u8(void) {
    ecs_world_t *world = ecs_init();

    uint64_t value = 0;

    ecs_log_set_level(-4);
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(
        world, "a", &ecs_value_ptr(ecs_u8_t, &value), &desc);
    test_assert(ptr == NULL);

    ecs_fini(world);
}

void Deserialize_invalid_u16(void) {
    ecs_world_t *world = ecs_init();

    uint64_t value = 0;

    ecs_log_set_level(-4);
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(
        world, "a", &ecs_value_ptr(ecs_u16_t, &value), &desc);
    test_assert(ptr == NULL);

    ecs_fini(world);
}

void Deserialize_invalid_u32(void) {
    ecs_world_t *world = ecs_init();

    uint64_t value = 0;

    ecs_log_set_level(-4);
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(
        world, "a", &ecs_value_ptr(ecs_u32_t, &value), &desc);
    test_assert(ptr == NULL);

    ecs_fini(world);
}

void Deserialize_invalid_u64(void) {
    ecs_world_t *world = ecs_init();

    uint64_t value = 0;

    ecs_log_set_level(-4);
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(
        world, "a", &ecs_value_ptr(ecs_u64_t, &value), &desc);
    test_assert(ptr == NULL);

    ecs_fini(world);
}

void Deserialize_invalid_uptr(void) {
    ecs_world_t *world = ecs_init();

    uint64_t value = 0;

    ecs_log_set_level(-4);
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(
        world, "a", &ecs_value_ptr(ecs_uptr_t, &value), &desc);
    test_assert(ptr == NULL);

    ecs_fini(world);
}

void Deserialize_invalid_float(void) {
    ecs_world_t *world = ecs_init();

    uint64_t value = 0;

    ecs_log_set_level(-4);
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(
        world, "a", &ecs_value_ptr(ecs_f32_t, &value), &desc);
    test_assert(ptr == NULL);

    ecs_fini(world);
}

void Deserialize_invalid_double(void) {
    ecs_world_t *world = ecs_init();

    uint64_t value = 0;

    ecs_log_set_level(-4);
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(
        world, "a", &ecs_value_ptr(ecs_f64_t, &value), &desc);
    test_assert(ptr == NULL);

    ecs_fini(world);
}

void Deserialize_string(void) {
    ecs_world_t *world = ecs_init();

    ecs_string_t value = 0;
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(world, 
        "\"Hello World\"", &ecs_value_ptr(ecs_string_t, &value), &desc);
    test_assert(ptr != NULL);

    test_str(value, "Hello World");

    ecs_os_free(value);

    ecs_fini(world);
}

void Deserialize_entity(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t value = 0;
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(world, 
        "flecs.core", &ecs_value_ptr(ecs_entity_t, &value), &desc);
    test_assert(ptr != NULL);
    test_uint(value, EcsFlecsCore);

    ptr = ecs_expr_run(world, 
        "0", &ecs_value_ptr(ecs_entity_t, &value), &desc);
    test_assert(ptr != NULL);
    test_uint(value, 0);

    ecs_fini(world);
}

void Deserialize_id(void) {
    ecs_world_t *world = ecs_init();

    ecs_id_t value = 0;
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(world, 
        "flecs.core", &ecs_value_ptr(ecs_id_t, &value), &desc);
    test_assert(ptr != NULL);
    test_uint(value, EcsFlecsCore);

    ptr = ecs_expr_run(world, 
        "0", &ecs_value_ptr(ecs_id_t, &value), &desc);
    test_assert(ptr != NULL);
    test_uint(value, 0);

    ecs_fini(world);
}

void Deserialize_enum(void) {
    typedef enum {
        Red, Blue, Green
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_enum_init(world, &(ecs_enum_desc_t){
        .entity = ecs_entity(world, { .name = "Color" }),
        .constants = {
            {"Red"}, {"Blue"}, {"Green"}
        }
    });

    test_assert(t != 0);

    {
    T value = 0;
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(world, 
        "Red",
        &(ecs_value_t){t, &value}, &desc);
    test_assert(ptr != NULL);
    test_int(value, Red);
    }

    {
    T value = 0;
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(world, 
        "Blue",
        &(ecs_value_t){t, &value}, &desc);
    test_assert(ptr != NULL);
    test_int(value, Blue);
    }

    {
    T value = 0;
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(world, 
        "Green",
        &(ecs_value_t){t, &value}, &desc);
    test_assert(ptr != NULL);
    test_int(value, Green);
    }

    {
    ecs_log_set_level(-4);
    T value = 0;
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(world, 
        "Black",
        &(ecs_value_t){t, &value}, &desc);
    test_assert(ptr == NULL);
    }

    ecs_fini(world);
}

void Deserialize_bitmask(void) {
    uint64_t Lettuce = 0x1;
    uint64_t Bacon =   0x1 << 1;
    uint64_t Tomato =  0x1 << 2;
    uint64_t Cheese =  0x1 << 3;

    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_bitmask_init(world, &(ecs_bitmask_desc_t){
        .entity = ecs_entity(world, { .name = "Toppings" }),
        .constants = {
            {"Lettuce"}, {"Bacon"}, {"Tomato"}, {"Cheese"}, {"BLT", Bacon | Lettuce | Tomato}
        }
    });

    {
    uint64_t value = 0;
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(world, 
        "Lettuce",
        &(ecs_value_t){t, &value}, &desc);
    test_assert(ptr != NULL);
    test_uint(value, Lettuce);
    }

    {
    uint64_t value = 0;
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(world, 
        "Lettuce|Bacon",
        &(ecs_value_t){t, &value}, &desc);
    test_assert(ptr != NULL);
    test_uint(value, Lettuce|Bacon);
    }

    {
    uint64_t value = 0;
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(world, 
        "Lettuce|Bacon|Tomato|Cheese",
        &(ecs_value_t){t, &value}, &desc);
    test_assert(ptr != NULL);
    test_uint(value, Lettuce|Bacon|Tomato|Cheese);
    }

    {
    uint64_t value = 0;
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(world, 
        "Lettuce | Bacon | Tomato | Cheese",
        &(ecs_value_t){t, &value}, &desc);
    test_assert(ptr != NULL);
    test_uint(value, Lettuce|Bacon|Tomato|Cheese);
    }

    {
    uint64_t value = 0;
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(world, 
        "BLT",
        &(ecs_value_t){t, &value}, &desc);
    test_assert(ptr != NULL);
    test_uint(value, Lettuce|Bacon|Tomato);
    }

    {
    uint64_t value = 0;
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(world, 
        "0",
        &(ecs_value_t){t, &value}, &desc);
    test_assert(ptr != NULL);
    test_uint(value, 0);
    }

    {
    ecs_log_set_level(-4);
    uint64_t value = 0;
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(world, 
        "Foo",
        &(ecs_value_t){t, &value}, &desc);
    test_assert(ptr == NULL);
    }

    ecs_fini(world);
}

void Deserialize_struct_enum(void) {
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

    ecs_entity_t t = ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "T"}),
        .members = {
            {"v", e}
        }
    });

    test_assert(t != 0);

    {
    T value = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(world, 
        "{v: Red}",
        &(ecs_value_t){t, &value}, &desc);
    test_assert(ptr != NULL);
    test_int(value.v, Red);
    }

    {
    T value = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(world, 
        "{v: Blue}",
        &(ecs_value_t){t, &value}, &desc);
    test_assert(ptr != NULL);
    test_int(value.v, Blue);
    }

    {
    T value = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(world, 
        "{v: Green}",
        &(ecs_value_t){t, &value}, &desc);
    test_assert(ptr != NULL);
    test_int(value.v, Green);
    }

    {
    ecs_log_set_level(-4);
    T value = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(world, 
        "{v: Black}",
        &(ecs_value_t){t, &value}, &desc);
    test_assert(ptr == NULL);
    }

    ecs_fini(world);
}

void Deserialize_struct_bitmask(void) {
    uint64_t Lettuce = 0x1;
    uint64_t Bacon =   0x1 << 1;
    uint64_t Tomato =  0x1 << 2;
    uint64_t Cheese =  0x1 << 3;

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

    ecs_entity_t t = ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "T"}),
        .members = {
            {"v", b}
        }
    });

    test_assert(t != 0);

    {
    T value = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(world, 
        "{v:Lettuce}",
        &(ecs_value_t){t, &value}, &desc);
    test_assert(ptr != NULL);
    test_uint(value.v, Lettuce);
    }

    {
    T value = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(world, 
        "{v:Lettuce|Bacon}",
        &(ecs_value_t){t, &value}, &desc);
    test_assert(ptr != NULL);
    test_uint(value.v, Lettuce|Bacon);
    }

    {
    T value = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(world, 
        "{v:Lettuce|Bacon|Tomato|Cheese}",
        &(ecs_value_t){t, &value}, &desc);
    test_assert(ptr != NULL);
    test_uint(value.v, Lettuce|Bacon|Tomato|Cheese);
    }

    {
    T value = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(world, 
        "{v:BLT}",
        &(ecs_value_t){t, &value}, &desc);
    test_assert(ptr != NULL);
    test_uint(value.v, Lettuce|Bacon|Tomato);
    }

    {
    T value = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(world, 
        "{v:0}",
        &(ecs_value_t){t, &value}, &desc);
    test_assert(ptr != NULL);
    test_uint(value.v, 0);
    }

    {
    ecs_log_set_level(-4);
    T value = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(world, 
        "{v:Foo\"}",
        &(ecs_value_t){t, &value}, &desc);
    test_assert(ptr == NULL);
    }

    ecs_fini(world);
}

void Deserialize_struct_i32(void) {
    typedef struct {
        int32_t x;
    } T;
    
    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "T"}),
        .members = {
            {"x", ecs_id(ecs_i32_t)}
        }
    });

    T value = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(world, 
        "{10}",
        &(ecs_value_t){t, &value}, &desc);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_int(value.x, 10);

    ecs_fini(world);
}

void Deserialize_struct_i32_neg(void) {
    typedef struct {
        int32_t x;
    } T;
    
    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "T"}),
        .members = {
            {"x", ecs_id(ecs_i32_t)}
        }
    });

    T value = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(world, 
        "{-10}",
        &(ecs_value_t){t, &value}, &desc);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_int(value.x, -10);

    ecs_fini(world);
}

void Deserialize_struct_i32_i32(void) {
    typedef struct {
        int32_t x;
        int32_t y;
    } T;
    
    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "T"}),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    T value = {0, 0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(world, 
        "{10, 20}",
        &(ecs_value_t){t, &value}, &desc);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_int(value.x, 10);
    test_int(value.y, 20);

    ecs_fini(world);
}

void Deserialize_struct_string(void) {
    typedef struct {
        char *value;
    } T;
    
    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "T"}),
        .members = {
            {"value", ecs_id(ecs_string_t)}
        }
    });

    T value = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(world, 
        "{\"Hello World\"}",
        &(ecs_value_t){t, &value}, &desc);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_str(value.value, "Hello World");

    ecs_os_free(value.value);

    ecs_fini(world);
}

void Deserialize_struct_string_from_name(void) {
    typedef struct {
        char *value;
    } T;
    
    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "T"}),
        .members = {
            {"value", ecs_id(ecs_string_t)}
        }
    });

    T value = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(world, 
        "{flecs.core.name()}", 
        &(ecs_value_t){t, &value}, &desc);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_str(value.value, "core");

    ecs_os_free(value.value);

    ecs_fini(world);
}

void Deserialize_struct_string_from_path(void) {
    typedef struct {
        char *value;
    } T;
    
    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "T"}),
        .members = {
            {"value", ecs_id(ecs_string_t)}
        }
    });

    T value = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(world, 
        "{flecs.core.path()}", 
        &(ecs_value_t){t, &value}, &desc);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_str(value.value, "flecs.core");

    ecs_os_free(value.value);

    ecs_fini(world);
}

void Deserialize_struct_string_from_var(void) {
    typedef struct {
        char *value;
    } T;
    
    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "T"}),
        .members = {
            {"value", ecs_id(ecs_string_t)}
        }
    });

    T value = {0};

    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *foo = ecs_script_vars_define(vars, "foo", ecs_string_t);
    *(char**)foo->value.ptr = "Hello World";

    ecs_expr_eval_desc_t desc = { .vars = vars, .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(world, 
        "{$foo}", 
        &(ecs_value_t){t, &value}, &desc);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_str(value.value, "Hello World");

    ecs_os_free(value.value);

    *(char**)foo->value.ptr = NULL;
    ecs_script_vars_fini(vars);

    ecs_fini(world);
}

void Deserialize_struct_entity(void) {
    typedef struct {
        ecs_entity_t entity;
    } T;
    
    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "T"}),
        .members = {
            {"entity", ecs_id(ecs_entity_t)}
        }
    });

    T value = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(world, 
        "{flecs.core}",
        &(ecs_value_t){t, &value}, &desc);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_int(value.entity, EcsFlecsCore);

    ecs_fini(world);
}

void Deserialize_struct_id(void) {
    typedef struct {
        ecs_id_t entity;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "T"}),
        .members = {
            {"entity", ecs_id(ecs_id_t)}
        }
    });

    T value = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(world, 
        "{flecs.core}",
        &(ecs_value_t){t, &value}, &desc);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_int(value.entity, EcsFlecsCore);

    ecs_fini(world);
}

void Deserialize_struct_nested_i32(void) {
    typedef struct {
        ecs_i32_t x;
    } N1;

    typedef struct {
        N1 n_1;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t n1 = ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "N1"}),
        .members = {
            {"x", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t t = ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "T"}),
        .members = {
            {"n_1", n1},
        }
    });

    T value = {{0}};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(world, 
        "{{10}}",
        &(ecs_value_t){t, &value}, &desc);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_int(value.n_1.x, 10);

    ecs_fini(world);
}

void Deserialize_struct_nested_empty(void) {
    typedef struct {
        ecs_i32_t x;
    } N1;

    typedef struct {
        N1 n_1;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t n1 = ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "N1"}),
        .members = {
            {"x", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t t = ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "T"}),
        .members = {
            {"n_1", n1},
        }
    });

    T value = {{0}};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(world, 
        "{{}}",
        &(ecs_value_t){t, &value}, &desc);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_int(value.n_1.x, 0);

    ecs_fini(world);
}

void Deserialize_struct_nested_i32_i32(void) {
    typedef struct {
        ecs_i32_t x;
        ecs_i32_t y;
    } N1;

    typedef struct {
        N1 n_1;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t n1 = ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "N1"}),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t t = ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "T"}),
        .members = {
            {"n_1", n1},
        }
    });

    T value = {{0}};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(world, 
        "{{10, 20}}",
        &(ecs_value_t){t, &value}, &desc);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_int(value.n_1.x, 10);
    test_int(value.n_1.y, 20);

    ecs_fini(world);
}

void Deserialize_struct_2_nested_i32_i32(void) {
    typedef struct {
        ecs_i32_t x;
        ecs_i32_t y;
    } N1;

    typedef struct {
        N1 n_1;
        N1 n_2;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t n1 = ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "N1"}),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t t = ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "T"}),
        .members = {
            {"n_1", n1},
            {"n_2", n1},
        }
    });

    T value = {{0}};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(world, 
        "{{10, 20}, {30, 40}}",
        &(ecs_value_t){t, &value}, &desc);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_int(value.n_1.x, 10);
    test_int(value.n_1.y, 20);
    test_int(value.n_2.x, 30);
    test_int(value.n_2.y, 40);

    ecs_fini(world);
}

void Deserialize_struct_member_i32(void) {
    typedef struct {
        int32_t x;
    } T;
    
    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "T"}),
        .members = {
            {"x", ecs_id(ecs_i32_t)}
        }
    });

    T value = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(world, 
        "{x: 10}",
        &(ecs_value_t){t, &value}, &desc);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_int(value.x, 10);

    ecs_fini(world);
}

void Deserialize_struct_member_i32_neg(void) {
    typedef struct {
        int32_t x;
    } T;
    
    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "T"}),
        .members = {
            {"x", ecs_id(ecs_i32_t)}
        }
    });

    T value = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(world, 
        "{x: -10}",
        &(ecs_value_t){t, &value}, &desc);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_int(value.x, -10);

    ecs_fini(world);
}

void Deserialize_struct_member_i32_i32(void) {
    typedef struct {
        int32_t x;
        int32_t y;
    } T;
    
    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "T"}),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    T value = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(world, 
        "{x: 10, y: 20}",
        &(ecs_value_t){t, &value}, &desc);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_int(value.x, 10);
    test_int(value.y, 20);

    ecs_fini(world);
}

void Deserialize_struct_member_nested_i32(void) {
    typedef struct {
        ecs_i32_t x;
    } N1;

    typedef struct {
        N1 n_1;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t n1 = ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "N1"}),
        .members = {
            {"x", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t t = ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "T"}),
        .members = {
            {"n_1", n1},
        }
    });

    T value = {{0}};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(world, 
        "{n_1: {x: 10}}",
        &(ecs_value_t){t, &value}, &desc);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_int(value.n_1.x, 10);

    ecs_fini(world);
}

void Deserialize_struct_member_nested_i32_i32(void) {
    typedef struct {
        ecs_i32_t x;
        ecs_i32_t y;
    } N1;

    typedef struct {
        N1 n_1;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t n1 = ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "N1"}),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t t = ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "T"}),
        .members = {
            {"n_1", n1},
        }
    });

    T value = {{0}};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(world, 
        "{n_1: {x: 10, y: 20}}",
        &(ecs_value_t){t, &value}, &desc);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_int(value.n_1.x, 10);
    test_int(value.n_1.y, 20);

    ecs_fini(world);
}

void Deserialize_struct_member_2_nested_i32_i32(void) {
    typedef struct {
        ecs_i32_t x;
        ecs_i32_t y;
    } N1;

    typedef struct {
        N1 n_1;
        N1 n_2;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t n1 = ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "N1"}),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t t = ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "T"}),
        .members = {
            {"n_1", n1},
            {"n_2", n1},
        }
    });

    T value = {{0}};

    const char *expr = 
    HEAD "{"
    LINE " n_1: {x: 10, y: 20},"
    LINE " n_2: {x: 30, y: 40}"
    LINE "}";
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(world, expr, &(ecs_value_t){t, &value}, &desc);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_int(value.n_1.x, 10);
    test_int(value.n_1.y, 20);
    test_int(value.n_2.x, 30);
    test_int(value.n_2.y, 40);

    ecs_fini(world);
}

void Deserialize_struct_from_var(void) {
    typedef struct {
        int32_t x;
        int32_t y;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(T) = ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "T"}),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    T value = {0};

    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *foo = ecs_script_vars_define(vars, "foo", T);
    T *var = foo->value.ptr;
    var->x = 10;
    var->y = 20;

    {
        ecs_expr_eval_desc_t desc = { .vars = vars, .disable_folding = disable_folding };
        const char *ptr = ecs_expr_run(world, 
            "$foo", 
            &(ecs_value_t){ecs_id(T), &value}, &desc);
        test_assert(ptr != NULL);
        test_assert(ptr[0] == '\0');
        test_int(value.x, 10);
        test_int(value.y, 20);
    }

    ecs_script_vars_fini(vars);

    ecs_fini(world);
}

void Deserialize_struct_member_from_var(void) {
    typedef struct {
        int32_t value;
    } T;
    
    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "T"}),
        .members = {
            {"value", ecs_id(ecs_i32_t)}
        }
    });

    T value = {0};

    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *foo = ecs_script_vars_define(vars, "foo", ecs_i32_t);
    T *var = foo->value.ptr;
    var->value = 10;

    ecs_expr_eval_desc_t desc = { .vars = vars, .disable_folding = disable_folding };

    {
        const char *ptr = ecs_expr_run(world, 
            "{ $foo }", 
            &(ecs_value_t){t, &value}, &desc);
        test_assert(ptr != NULL);
        test_assert(ptr[0] == '\0');
        test_int(value.value, 10);
    }

    value.value = 0;

    {
        const char *ptr = ecs_expr_run(world, 
            "{ value: $foo }", 
            &(ecs_value_t){t, &value}, &desc);
        test_assert(ptr != NULL);
        test_assert(ptr[0] == '\0');
        test_int(value.value, 10);
    }

    ecs_script_vars_fini(vars);

    ecs_fini(world);
}

void Deserialize_struct_member_auto_var(void) {
    typedef struct {
        int32_t value;
    } T;
    
    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "T"}),
        .members = {
            {"value", ecs_id(ecs_i32_t)}
        }
    });

    T value = {0};

    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *foo = ecs_script_vars_define(vars, "value", ecs_i32_t);
    T *var = foo->value.ptr;
    var->value = 10;

    ecs_expr_eval_desc_t desc = { .vars = vars, .disable_folding = disable_folding };

    {
        const char *ptr = ecs_expr_run(world, 
            "{ value: $ }", 
            &(ecs_value_t){t, &value}, &desc);
        test_assert(ptr != NULL);
        test_assert(ptr[0] == '\0');
        test_int(value.value, 10);
    }

    ecs_script_vars_fini(vars);

    ecs_fini(world);
}

void Deserialize_struct_member_auto_vars(void) {
    typedef struct {
        int32_t x;
        int32_t y;
    } T;
    
    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "T"}),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    T value = {0};

    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *x = ecs_script_vars_define(vars, "x", ecs_i32_t);
    *(int32_t*)x->value.ptr = 10;
    ecs_script_var_t *y = ecs_script_vars_define(vars, "y", ecs_i32_t);
    *(int32_t*)y->value.ptr = 20;

    ecs_expr_eval_desc_t desc = { .vars = vars, .disable_folding = disable_folding };

    {
        const char *ptr = ecs_expr_run(world, 
            "{ x: $, y: $ }", 
            &(ecs_value_t){t, &value}, &desc);
        test_assert(ptr != NULL);
        test_assert(ptr[0] == '\0');
        test_int(value.x, 10);
        test_int(value.y, 20);
    }

    ecs_script_vars_fini(vars);

    ecs_fini(world);
}

void Deserialize_struct_nested_member_auto_var(void) {
    ecs_world_t *world = ecs_init();

    typedef struct {
        int32_t x;
        int32_t y;
    } Point;

    typedef struct {
        Point start;
        Point stop;
    } Line;

    ecs_entity_t point = ecs_struct(world, {
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t ecs_id(Line) = ecs_struct(world, {
        .members = {
            {"start", point},
            {"stop", point}
        }
    });

    Line value = {0};

    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *foo = ecs_script_vars_define(vars, "x", ecs_i32_t);
    *(int32_t*)foo->value.ptr = 10;

    ecs_expr_eval_desc_t desc = { .vars = vars, .disable_folding = disable_folding };

    {
        const char *ptr = ecs_expr_run(world, 
            "{ stop: { x: $ }}", 
            &(ecs_value_t){ecs_id(Line), &value}, &desc);
        test_assert(ptr != NULL);
        test_assert(ptr[0] == '\0');
        test_int(value.start.x, 0);
        test_int(value.start.y, 0);
        test_int(value.stop.x, 10);
        test_int(value.stop.y, 0);
    }

    ecs_script_vars_fini(vars);

    ecs_fini(world);
}

void Deserialize_struct_nested_member_auto_vars(void) {
    ecs_world_t *world = ecs_init();

    typedef struct {
        int32_t x;
        int32_t y;
    } Point;

    typedef struct {
        Point start;
        Point stop;
    } Line;

    ecs_entity_t point = ecs_struct(world, {
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t ecs_id(Line) = ecs_struct(world, {
        .members = {
            {"start", point},
            {"stop", point}
        }
    });

    Line value = {0};

    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *x = ecs_script_vars_define(vars, "x", ecs_i32_t);
    *(int32_t*)x->value.ptr = 10;
    ecs_script_var_t *y = ecs_script_vars_define(vars, "y", ecs_i32_t);
    *(int32_t*)y->value.ptr = 20;

    ecs_expr_eval_desc_t desc = { .vars = vars, .disable_folding = disable_folding };

    {
        const char *ptr = ecs_expr_run(world, 
            "{ stop: { x: $, y: $ }}", 
            &(ecs_value_t){ecs_id(Line), &value}, &desc);
        test_assert(ptr != NULL);
        test_assert(ptr[0] == '\0');
        test_int(value.start.x, 0);
        test_int(value.start.y, 0);
        test_int(value.stop.x, 10);
        test_int(value.stop.y, 20);
    }

    ecs_script_vars_fini(vars);

    ecs_fini(world);
}

void Deserialize_struct_auto_vars(void) {
    typedef struct {
        int32_t x;
        int32_t y;
    } T;
    
    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "T"}),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    T value = {0};

    ecs_script_vars_t *vars = ecs_script_vars_init(world);

    ecs_script_var_t *x = ecs_script_vars_define(vars, "x", ecs_i32_t);
    *(int32_t*)x->value.ptr = 10;
    ecs_script_var_t *y = ecs_script_vars_define(vars, "y", ecs_i32_t);
    *(int32_t*)y->value.ptr = 20;

    ecs_expr_eval_desc_t desc = { .vars = vars, .disable_folding = disable_folding };

    {
        const char *ptr = ecs_expr_run(world, 
            "{ $, $ }", 
            &(ecs_value_t){t, &value}, &desc);
        test_assert(ptr != NULL);
        test_assert(ptr[0] == '\0');
        test_int(value.x, 10);
        test_int(value.y, 20);
    }

    ecs_script_vars_fini(vars);

    ecs_fini(world);
}

void Deserialize_struct_member_2_nested_i32_i32_reverse(void) {
    typedef struct {
        ecs_i32_t x;
        ecs_i32_t y;
    } N1;

    typedef struct {
        N1 n_1;
        N1 n_2;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t n1 = ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "N1"}),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t t = ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "T"}),
        .members = {
            {"n_1", n1},
            {"n_2", n1},
        }
    });

    T value = {{0}};

    const char *expr = 
    HEAD "{"
    LINE " n_2: {x: 30, y: 40},"
    LINE " n_1: {x: 10, y: 20}"
    LINE "}";
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(world, expr, &(ecs_value_t){t, &value}, &desc);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_int(value.n_1.x, 10);
    test_int(value.n_1.y, 20);
    test_int(value.n_2.x, 30);
    test_int(value.n_2.y, 40);

    ecs_fini(world);
}

void Deserialize_struct_w_add_assign_expr(void) {
    ecs_world_t *world = ecs_init();

    typedef struct {
        int32_t x;
        int32_t y;
    } Point;

    ECS_COMPONENT(world, Point);

    ecs_entity_t t = ecs_struct(world, {
        .entity = ecs_id(Point),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };

    Point value = {10, 20};

    const char *ptr = ecs_expr_run(world, 
        "{x += 1, y += 2}",
        &(ecs_value_t){t, &value}, &desc);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_int(value.x, 11);
    test_int(value.y, 22);

    ecs_fini(world);
}

void Deserialize_struct_w_mul_assign_expr(void) {
    ecs_world_t *world = ecs_init();

    typedef struct {
        int32_t x;
        int32_t y;
    } Point;

    ECS_COMPONENT(world, Point);

    ecs_entity_t t = ecs_struct(world, {
        .entity = ecs_id(Point),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };

    Point value = {10, 20};

    const char *ptr = ecs_expr_run(world, 
        "{x *= 2, y *= 4}",
        &(ecs_value_t){t, &value}, &desc);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_int(value.x, 20);
    test_int(value.y, 80);

    ecs_fini(world);
}

void Deserialize_struct_w_add_assign_expr_invalid_type(void) {
    ecs_world_t *world = ecs_init();

    typedef struct {
        char *x;
        char *y;
    } Point;

    ECS_COMPONENT(world, Point);

    ecs_entity_t t = ecs_struct(world, {
        .entity = ecs_id(Point),
        .members = {
            {"x", ecs_id(ecs_string_t)},
            {"y", ecs_id(ecs_string_t)}
        }
    });

    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };

    Point value = {0};

    ecs_log_set_level(-4);
    const char *ptr = ecs_expr_run(world, 
        "{x += 1, y += 2}",
        &(ecs_value_t){t, &value}, &desc);
    test_assert(ptr == NULL);

    ecs_fini(world);
}

void Deserialize_struct_i32_array_3(void) {
    typedef struct {
        int32_t x[3];
    } T;
    
    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "T"}),
        .members = {
            {"x", ecs_id(ecs_i32_t), 3}
        }
    });

    T value = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(world, 
        "{x: [10, 20, 30]}",
        &(ecs_value_t){t, &value}, &desc);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_int(value.x[0], 10);
    test_int(value.x[1], 20);
    test_int(value.x[2], 30);

    ecs_fini(world);
}

void Deserialize_struct_i32_array_3_empty(void) {
    typedef struct {
        int32_t x[3];
    } T;
    
    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "T"}),
        .members = {
            {"x", ecs_id(ecs_i32_t), 3}
        }
    });

    T value = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(world, 
        "{x: []}",
        &(ecs_value_t){t, &value}, &desc);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_int(value.x[0], 0);
    test_int(value.x[1], 0);
    test_int(value.x[2], 0);

    ecs_fini(world);
}

void Deserialize_struct_struct_i32_array_3(void) {
    typedef struct {
        ecs_i32_t x;
    } N1;

    typedef struct {
        N1 n_1[3];
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t n1 = ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "N1"}),
        .members = {
            {"x", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t t = ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "T"}),
        .members = {
            {"n_1", n1, 3}
        }
    });

    T value = {{{0}}};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(world, 
        "{n_1: [{x: 10}, {x: 20}, {x: 30}]}",
        &(ecs_value_t){t, &value}, &desc);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_int(value.n_1[0].x, 10);
    test_int(value.n_1[1].x, 20);
    test_int(value.n_1[2].x, 30);

    ecs_fini(world);
}

void Deserialize_struct_struct_i32_i32_array_3(void) {
    typedef struct {
        ecs_i32_t x;
        ecs_i32_t y;
    } N1;

    typedef struct {
        N1 n_1[3];
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t n1 = ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "N1"}),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t t = ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "T"}),
        .members = {
            {"n_1", n1, 3}
        }
    });

    T value = {{{0}}};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(world, 
        "{n_1: [{x: 10, y: 20}, {x: 30, y: 40}, {x: 50, y: 60}]}",
        &(ecs_value_t){t, &value}, &desc);
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

void Deserialize_struct_w_array_type_i32_i32(void) {
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

    ecs_entity_t t = ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "T"}),
        .members = {
            {"n_1", n1}
        }
    });

    T value = {{ 0 }};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(world, 
        "{n_1: [10, 20]}",
        &(ecs_value_t){t, &value}, &desc);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_int(value.n_1[0], 10);
    test_int(value.n_1[1], 20);

    ecs_fini(world);
}

void Deserialize_struct_w_2_array_type_i32_i32(void) {
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

    ecs_entity_t t = ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "T"}),
        .members = {
            {"n_1", n1},
            {"n_2", n1}
        }
    });

    T value = {{ 0 }};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(world, 
        "{n_1: [10, 20], n_2: [30, 40]}",
        &(ecs_value_t){t, &value}, &desc);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_int(value.n_1[0], 10);
    test_int(value.n_1[1], 20);
    test_int(value.n_2[0], 30);
    test_int(value.n_2[1], 40);

    ecs_fini(world);
}

void Deserialize_struct_w_array_type_struct(void) {
    typedef struct {
        ecs_i32_t x;
        ecs_i32_t y;
    } N1;

    typedef N1 A1[2];

    typedef struct {
        A1 n_1;
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t n1 = ecs_struct(world, {
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

    ecs_entity_t t = ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "T"}),
        .members = {
            {"n_1", a1}
        }
    });

    test_assert(t != 0);

    T value;
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(world, 
        "{n_1: [{x: 10, y: 20}, {x: 30, y: 40}]}", 
        &(ecs_value_t){t, &value}, &desc);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_int(value.n_1[0].x, 10);
    test_int(value.n_1[0].y, 20);
    test_int(value.n_1[1].x, 30);
    test_int(value.n_1[1].y, 40);

    ecs_fini(world);
}

void Deserialize_struct_w_2_array_type_struct(void) {
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

    ecs_entity_t n1 = ecs_struct(world, {
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

    ecs_entity_t t = ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "T"}),
        .members = {
            {"n_1", a1},
            {"n_2", a1}
        }
    });

    test_assert(t != 0);

    T value;
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(world, 
        "{n_1: [{x: 10, y: 20}, {x: 30, y: 40}], n_2: [{x: 50, y: 60}, {x: 70, y: 80}]}", 
        &(ecs_value_t){t, &value}, &desc);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

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

void Deserialize_struct_w_newline(void) {
    ecs_world_t *world = ecs_init();

    typedef struct {
        int32_t x;
        int32_t y;
    } Point;

    ECS_COMPONENT(world, Point);

    ecs_struct(world, {
        .entity = ecs_id(Point),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    Point value = {0};

    {
        const char *ptr = ecs_expr_run(world, 
            "{\n"
            "  10, 20\n"
            "}",
            &(ecs_value_t){ecs_id(Point), &value}, NULL);

        test_assert(ptr != NULL);
        test_assert(ptr[0] == '\0');
        test_int(value.x, 10);
        test_int(value.y, 20);
    }

    ecs_fini(world);
}

void Deserialize_struct_w_members_newline(void) {
    ecs_world_t *world = ecs_init();

    typedef struct {
        int32_t x;
        int32_t y;
    } Point;

    ECS_COMPONENT(world, Point);

    ecs_struct(world, {
        .entity = ecs_id(Point),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    Point value = {0};

    {
        const char *ptr = ecs_expr_run(world, 
            "{\n"
            "  x: 10,\n"
            "  y: 20\n"
            "}",
            &(ecs_value_t){ecs_id(Point), &value}, NULL);

        test_assert(ptr != NULL);
        test_assert(ptr[0] == '\0');
        test_int(value.x, 10);
        test_int(value.y, 20);
    }

    ecs_fini(world);
}

void Deserialize_struct_w_trailing_comma(void) {
    ecs_world_t *world = ecs_init();

    typedef struct {
        int32_t x;
        int32_t y;
    } Point;

    ECS_COMPONENT(world, Point);

    ecs_struct(world, {
        .entity = ecs_id(Point),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    Point value = {0};

    {
        const char *ptr = ecs_expr_run(world, 
            "{\n"
            "  x: 10,\n"
            "  y: 20,\n"
            "}",
            &(ecs_value_t){ecs_id(Point), &value}, NULL);

        test_assert(ptr != NULL);
        test_assert(ptr[0] == '\0');
        test_int(value.x, 10);
        test_int(value.y, 20);
    }

    ecs_fini(world);
}

void Deserialize_array_w_trailing_comma(void) {
    ecs_world_t *world = ecs_init();

    typedef int32_t Ints[2];

    ECS_COMPONENT(world, Ints);

    ecs_array(world, {
        .entity = ecs_id(Ints),
        .type = ecs_id(ecs_i32_t),
        .count = 2
    });

    Ints value = {0};

    {
        const char *ptr = ecs_expr_run(world, 
            "[\n"
            "  10,\n"
            "  20,\n"
            "]",
            &(ecs_value_t){ecs_id(Ints), &value}, NULL);

        test_assert(ptr != NULL);
        test_assert(ptr[0] == '\0');
        test_int(value[0], 10);
        test_int(value[1], 20);
    }

    ecs_fini(world);
}

void Deserialize_array_i32_2(void) {
    typedef int32_t Ints[2];

    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Ints) = ecs_array(world, {
        .entity = ecs_entity(world, {.name = "Ints"}),
        .type = ecs_id(ecs_i32_t),
        .count = 2
    });

    test_assert(ecs_id(Ints) != 0);

    Ints value = {0, 0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(world, 
        "[10, 20]",
        &(ecs_value_t){ecs_id(Ints), &value}, &desc);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_int(value[0], 10);
    test_int(value[1], 20);

    ecs_fini(world);
}

void Deserialize_array_string_2(void) {
    typedef char* Strings[2];

    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Strings) = ecs_array(world, {
        .entity = ecs_entity(world, {.name = "Strings"}),
        .type = ecs_id(ecs_string_t),
        .count = 2
    });

    test_assert(ecs_id(Strings) != 0);

    Strings value = {0, 0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(world, 
        "[\"Hello\", \"World\"]",
        &(ecs_value_t){ecs_id(Strings), &value}, &desc);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_str(value[0], "Hello");
    test_str(value[1], "World");

    ecs_os_free(value[0]);
    ecs_os_free(value[1]);

    ecs_fini(world);
}

void Deserialize_vector_i32_0(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t vec = ecs_vector(world, {
        .entity = ecs_entity(world, { .name = "Ints" }),
        .type = ecs_id(ecs_i32_t)
    });

    test_assert(vec != 0);

    ecs_vec_t value = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(world, 
        "[]",
        &(ecs_value_t){vec, &value}, &desc);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_int(value.count, 0);
    test_assert(value.array == NULL);

    ecs_fini(world);
}

void Deserialize_vector_i32_2(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t vec = ecs_vector(world, {
        .entity = ecs_entity(world, { .name = "Ints" }),
        .type = ecs_id(ecs_i32_t)
    });

    test_assert(vec != 0);

    ecs_vec_t value = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(world, 
        "[10, 20]",
        &(ecs_value_t){vec, &value}, &desc);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_int(value.count, 2);
    test_int(((int32_t*)value.array)[0], 10);
    test_int(((int32_t*)value.array)[1], 20);

    ecs_os_free(value.array);

    ecs_fini(world);
}

void Deserialize_vector_i32_0_into_2(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t vec = ecs_vector(world, {
        .entity = ecs_entity(world, { .name = "Ints" }),
        .type = ecs_id(ecs_i32_t)
    });

    test_assert(vec != 0);

    ecs_vec_t value = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(world, 
        "[10, 20]",
        &(ecs_value_t){vec, &value}, &desc);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_int(value.count, 2);
    test_int(((int32_t*)value.array)[0], 10);
    test_int(((int32_t*)value.array)[1], 20);

    ptr = ecs_expr_run(world, "[]", &(ecs_value_t){vec, &value}, &desc);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');
    test_int(value.count, 0);
    test_int(value.size, 2);
    test_assert(value.array != NULL);

    ecs_os_free(value.array);

    ecs_fini(world);
}

void Deserialize_vector_i32_1_into_2(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t vec = ecs_vector(world, {
        .entity = ecs_entity(world, { .name = "Ints" }),
        .type = ecs_id(ecs_i32_t)
    });

    test_assert(vec != 0);

    ecs_vec_t value = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(world, 
        "[10, 20]",
        &(ecs_value_t){vec, &value}, &desc);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_int(value.count, 2);
    test_int(((int32_t*)value.array)[0], 10);
    test_int(((int32_t*)value.array)[1], 20);

    ptr = ecs_expr_run(world, "[30]", &(ecs_value_t){vec, &value}, &desc);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');
    test_int(value.count, 1);
    test_int(((int32_t*)value.array)[0], 30);

    ecs_os_free(value.array);

    ecs_fini(world);
}

void Deserialize_vector_string_2(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t vec = ecs_vector(world, {
        .entity = ecs_entity(world, { .name = "Strings" }),
        .type = ecs_id(ecs_string_t)
    });

    test_assert(vec != 0);

    ecs_vec_t value = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(world, 
        "[\"Hello\", \"World\"]",
        &(ecs_value_t){vec, &value}, &desc);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_int(value.count, 2);
    test_str(((char**)value.array)[0], "Hello");
    test_str(((char**)value.array)[1], "World");

    ecs_os_free(((char**)value.array)[0]);
    ecs_os_free(((char**)value.array)[1]);
    ecs_os_free(value.array);

    ecs_fini(world);
}

void Deserialize_vector_string_0(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t vec = ecs_vector(world, {
        .entity = ecs_entity(world, { .name = "Strings" }),
        .type = ecs_id(ecs_string_t)
    });

    test_assert(vec != 0);

    ecs_vec_t value = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(world, "[]",
        &(ecs_value_t){vec, &value}, &desc);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_int(value.count, 0);
    test_assert(value.array == NULL);

    ecs_fini(world);
}

void Deserialize_vector_string_0_into_2(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t vec = ecs_vector(world, {
        .entity = ecs_entity(world, { .name = "Strings" }),
        .type = ecs_id(ecs_string_t)
    });

    test_assert(vec != 0);

    ecs_vec_t value = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(world, 
        "[\"Hello\", \"World\"]",
        &(ecs_value_t){vec, &value}, &desc);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_int(value.count, 2);
    test_str(((char**)value.array)[0], "Hello");
    test_str(((char**)value.array)[1], "World");

    ptr = ecs_expr_run(world, "[]", &(ecs_value_t){vec, &value}, &desc);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_int(value.count, 0);
    test_assert(value.array != NULL);

    ecs_os_free(value.array);

    ecs_fini(world);
}

void Deserialize_vector_string_1_into_2(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t vec = ecs_vector(world, {
        .entity = ecs_entity(world, { .name = "Strings" }),
        .type = ecs_id(ecs_string_t)
    });

    test_assert(vec != 0);

    ecs_vec_t value = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(world, 
        "[\"Hello\", \"World\"]",
        &(ecs_value_t){vec, &value}, &desc);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_int(value.count, 2);
    test_str(((char**)value.array)[0], "Hello");
    test_str(((char**)value.array)[1], "World");

    ptr = ecs_expr_run(world, "[\"Foo\"]", &(ecs_value_t){vec, &value}, &desc);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_int(value.count, 1);
    test_assert(value.array != NULL);
    test_str(((char**)value.array)[0], "Foo");
    ecs_os_free(((char**)value.array)[0]);
    ecs_os_free(value.array);

    ecs_fini(world);
}

void Deserialize_vector_struct_0(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Position" }),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    ecs_entity_t vec = ecs_vector(world, {
        .type = ecs_id(Position)
    });

    test_assert(vec != 0);

    ecs_vec_t value = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(world, "[]",
        &(ecs_value_t){vec, &value}, &desc);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_int(value.count, 0);
    ecs_os_free(value.array);

    ecs_fini(world);
}

void Deserialize_vector_struct_2(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Position" }),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    ecs_entity_t vec = ecs_vector(world, {
        .type = ecs_id(Position)
    });

    test_assert(vec != 0);

    ecs_vec_t value = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(world, "[{10, 20}, {30, 40}]",
        &(ecs_value_t){vec, &value}, &desc);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_int(value.count, 2);
    test_int(((Position*)value.array)[0].x, 10);
    test_int(((Position*)value.array)[0].y, 20);
    test_int(((Position*)value.array)[1].x, 30);
    test_int(((Position*)value.array)[1].y, 40);
    ecs_os_free(value.array);

    ecs_fini(world);
}

void Deserialize_vector_struct_2_empty(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Position" }),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    ecs_set_hooks(world, Position, {
        .ctor = flecs_default_ctor
    });

    ecs_entity_t vec = ecs_vector(world, {
        .type = ecs_id(Position)
    });

    test_assert(vec != 0);

    ecs_vec_t value = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(world, "[{}, {}]",
        &(ecs_value_t){vec, &value}, &desc);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_int(value.count, 2);
    test_int(((Position*)value.array)[0].x, 0);
    test_int(((Position*)value.array)[0].y, 0);
    test_int(((Position*)value.array)[1].x, 0);
    test_int(((Position*)value.array)[1].y, 0);
    ecs_os_free(value.array);

    ecs_fini(world);
}

void Deserialize_vector_struct_0_into_2(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Position" }),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    ecs_entity_t vec = ecs_vector(world, {
        .type = ecs_id(Position)
    });

    test_assert(vec != 0);

    ecs_vec_t value = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(world, "[{10, 20}, {30, 40}]",
        &(ecs_value_t){vec, &value}, &desc);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_int(value.count, 2);
    test_int(((Position*)value.array)[0].x, 10);
    test_int(((Position*)value.array)[0].y, 20);
    test_int(((Position*)value.array)[1].x, 30);
    test_int(((Position*)value.array)[1].y, 40);

    ptr = ecs_expr_run(world, "[]",
        &(ecs_value_t){vec, &value}, &desc);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_int(value.count, 0);

    ecs_os_free(value.array);

    ecs_fini(world);
}

void Deserialize_vector_struct_1_into_2(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Position" }),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    ecs_entity_t vec = ecs_vector(world, {
        .type = ecs_id(Position)
    });

    test_assert(vec != 0);

    ecs_vec_t value = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(world, "[{10, 20}, {30, 40}]",
        &(ecs_value_t){vec, &value}, &desc);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_int(value.count, 2);
    test_int(((Position*)value.array)[0].x, 10);
    test_int(((Position*)value.array)[0].y, 20);
    test_int(((Position*)value.array)[1].x, 30);
    test_int(((Position*)value.array)[1].y, 40);

    ptr = ecs_expr_run(world, "[{50, 60}]",
        &(ecs_value_t){vec, &value}, &desc);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_int(value.count, 1);
    test_int(((Position*)value.array)[0].x, 50);
    test_int(((Position*)value.array)[0].y, 60);
    ecs_os_free(value.array);

    ecs_fini(world);
}

void Deserialize_discover_type_int(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, 
        "10", &v, &desc) != NULL);
    test_uint(v.type, ecs_id(ecs_i64_t));
    test_assert(v.ptr != NULL);
    test_int(*(uint64_t*)v.ptr, 10);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Deserialize_discover_type_negative_int(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, 
        "-10", &v, &desc) != NULL);
    test_uint(v.type, ecs_id(ecs_i64_t));
    test_assert(v.ptr != NULL);
    test_int(*(int64_t*)v.ptr, -10);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Deserialize_discover_type_float(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, 
        "10.5", &v, &desc) != NULL);
    test_uint(v.type, ecs_id(ecs_f64_t));
    test_assert(v.ptr != NULL);
    test_flt(*(ecs_f64_t*)v.ptr, 10.5);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Deserialize_discover_type_negative_float(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, 
        "-10.5", &v, &desc) != NULL);
    test_uint(v.type, ecs_id(ecs_f64_t));
    test_assert(v.ptr != NULL);
    test_flt(*(ecs_f64_t*)v.ptr, -10.5);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Deserialize_discover_type_string(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, 
        "\"foo\"", &v, &desc) != NULL);
    test_uint(v.type, ecs_id(ecs_string_t));
    test_assert(v.ptr != NULL);
    test_str(*(ecs_string_t*)v.ptr, "foo");
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Deserialize_discover_type_multiline_string(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, 
        "`foo\nbar`", &v, &desc) != NULL);
    test_uint(v.type, ecs_id(ecs_string_t));
    test_assert(v.ptr != NULL);
    test_str(*(ecs_string_t*)v.ptr, "foo\nbar");
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Deserialize_discover_type_entity(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t foo = ecs_entity(world, { .name = "foo" });

    ecs_value_t v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, 
        "foo", &v, &desc) != NULL);
    test_uint(v.type, ecs_id(ecs_entity_t));
    test_assert(v.ptr != NULL);
    test_uint(*(ecs_entity_t*)v.ptr, foo);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Deserialize_discover_type_bool(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, 
        "true", &v, &desc) != NULL);
    test_uint(v.type, ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(ecs_bool_t*)v.ptr, true);
    ecs_value_free(world, v.type, v.ptr);

    ecs_os_zeromem(&v);

    test_assert(ecs_expr_run(world, 
        "false", &v, &desc) != NULL);
    test_uint(v.type, ecs_id(ecs_bool_t));
    test_assert(v.ptr != NULL);
    test_bool(*(ecs_bool_t*)v.ptr, false);
    ecs_value_free(world, v.type, v.ptr);

    ecs_fini(world);
}

void Deserialize_discover_type_unknown(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};

    ecs_log_set_level(-4);
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, 
        "{10}", &v, &desc) == NULL);

    test_assert(ecs_expr_run(world, 
        "[10]", &v, &desc) == NULL);

    ecs_fini(world);
}

void Deserialize_discover_type_invalid(void) {
    ecs_world_t *world = ecs_init();

    ecs_value_t v = {0};

    ecs_log_set_level(-4);
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    test_assert(ecs_expr_run(world, 
        "-", &v, &desc) == NULL);

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

void Deserialize_opaque_struct(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, OpaqueStruct);

    ecs_entity_t s = ecs_struct(world, {
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)},
        }
    });

    ecs_opaque(world, {
        .entity = ecs_id(OpaqueStruct),
        .type.as_type = s,
        .type.ensure_member = OpaqueStruct_member
    });

    OpaqueStruct v;
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(world, 
        "{10, 20}", &ecs_value_ptr(OpaqueStruct, &v), &desc);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');        

    test_int(v.x, 10);
    test_int(v.y, 20);

    ecs_fini(world);
}

void Deserialize_opaque_struct_w_member(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, OpaqueStruct);

    ecs_entity_t s = ecs_struct(world, {
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)},
        }
    });

    ecs_opaque(world, {
        .entity = ecs_id(OpaqueStruct),
        .type.as_type = s,
        .type.ensure_member = OpaqueStruct_member
    });

    OpaqueStruct v;
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(world, 
        "{x: 10, y: 20}", &ecs_value_ptr(OpaqueStruct, &v), &desc);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');        

    test_int(v.x, 10);
    test_int(v.y, 20);

    ecs_fini(world);
}

void Deserialize_opaque_struct_w_member_reverse(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, OpaqueStruct);

    ecs_entity_t s = ecs_struct(world, {
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)},
        }
    });

    ecs_opaque(world, {
        .entity = ecs_id(OpaqueStruct),
        .type.as_type = s,
        .type.ensure_member = OpaqueStruct_member
    });

    OpaqueStruct v;
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(world, 
        "{y: 10, x: 20}", &ecs_value_ptr(OpaqueStruct, &v), &desc);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');        

    test_int(v.x, 20);
    test_int(v.y, 10);

    ecs_fini(world);
}

typedef struct Opaque_string {
    int32_t len;
    char *value;
} Opaque_string;

typedef struct Struct_w_opaque {
    Opaque_string str;
} Struct_w_opaque;

static void Opaque_string_set(void *ptr, const char *value) {
    ((Opaque_string*)ptr)->value = ecs_os_strdup(value);
}

typedef struct Opaque_entity {
    ecs_world_t *world;
    ecs_entity_t value;
} Opaque_entity;

typedef struct Struct_w_opaque_entity {
    Opaque_entity entity;
} Struct_w_opaque_entity;

static void Opaque_entity_set(void *ptr, ecs_world_t *world, ecs_entity_t value) {
    ((Opaque_entity*)ptr)->world = world;
    ((Opaque_entity*)ptr)->value = value;
}

void Deserialize_struct_w_opaque_member(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Opaque_string);
    ECS_COMPONENT(world, Struct_w_opaque);

    ecs_opaque(world, {
        .entity = ecs_id(Opaque_string),
        .type.as_type = ecs_id(ecs_string_t),
        .type.assign_string = Opaque_string_set
    });

    ecs_struct(world, {
        .entity = ecs_id(Struct_w_opaque),
        .members = {
            {"str", ecs_id(Opaque_string)},
        }
    });

    Struct_w_opaque v = {{0, NULL}};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(world, 
        "{str: \"foobar\"}", &ecs_value_ptr(Struct_w_opaque, &v), &desc);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');        

    test_str(v.str.value, "foobar");
    ecs_os_free(v.str.value);

    ecs_fini(world);
}

void Deserialize_opaque_string(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Opaque_string);

    ecs_opaque(world, {
        .entity = ecs_id(Opaque_string),
        .type.as_type = ecs_id(ecs_string_t),
        .type.assign_string = Opaque_string_set
    });

    Opaque_string v = {0, NULL};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(world, 
        "\"foobar\"", &ecs_value_ptr(Opaque_string, &v), &desc);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');        

    test_str(v.value, "foobar");
    ecs_os_free(v.value);

    ecs_fini(world);
}

void Deserialize_opaque_entity(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Opaque_entity);

    ecs_opaque(world, {
        .entity = ecs_id(Opaque_entity),
        .type.as_type = ecs_id(ecs_entity_t),
        .type.assign_entity = Opaque_entity_set
    });

    ecs_entity_t e = ecs_entity(world, { .name = "Foo" });

    Opaque_entity v;
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(world, 
        "Foo", &ecs_value_ptr(Opaque_entity, &v), &desc);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');        

    test_assert(v.world == world);
    test_uint(v.value, e);

    ecs_fini(world);
}

void Deserialize_struct_w_opaque_entity_member(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Opaque_entity);
    ECS_COMPONENT(world, Struct_w_opaque_entity);

    ecs_opaque(world, {
        .entity = ecs_id(Opaque_entity),
        .type.as_type = ecs_id(ecs_entity_t),
        .type.assign_entity = Opaque_entity_set
    });

    ecs_struct(world, {
        .entity = ecs_id(Struct_w_opaque_entity),
        .members = {
            {"entity", ecs_id(Opaque_entity)},
        }
    });

    ecs_entity_t e = ecs_entity(world, { .name = "Foo" });

    Struct_w_opaque_entity v;
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(world, 
        "{entity: Foo}", &ecs_value_ptr(Struct_w_opaque_entity, &v), &desc);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_assert(v.entity.world == world);
    test_uint(v.entity.value, e);

    ecs_fini(world);
}

typedef struct IntVec {
    bool _dummy; // make sure type doesn't accidentally line up with ecs_vec_t 
    size_t count;
    int32_t *array;
} IntVec;

typedef struct StringVec {
    bool _dummy;
    size_t count;
    char **array;
} StringVec;

typedef struct String {
    bool _dummy; // make sure type doesn't line up with char*
    char *value;
} String;

typedef struct OpaqueStringVec {
    bool _dummy;
    size_t count;
    String *array;
} OpaqueStringVec;

ECS_CTOR(String, ptr, {
    ptr->value = NULL;
})

ECS_MOVE(String, dst, src, {
    if (dst->value) {
        ecs_os_free(dst->value);
    }
    dst->value = src->value;
    src->value = NULL;
})

ECS_COPY(String, dst, src, {
    if (dst->value) {
        ecs_os_free(dst->value);
    }
    if (src->value) {
        dst->value = ecs_os_strdup(src->value);
    } else {
        dst->value = NULL;
    }
})

ECS_DTOR(String, ptr, {
    ecs_os_free(ptr->value);
})

static void String_assign(void *ptr, const char *value) {
    String *s = ptr;
    if (s->value) {
        ecs_os_free(s->value);
    }
    s->value = ecs_os_strdup(value);
}

static 
size_t IntVec_count(const void *ptr) {
    const IntVec *data = ptr;
    return data->count;
}

static 
void* IntVec_ensure(void *ptr, size_t index) {
    IntVec *data = ptr;
    test_assert(data != NULL);
    if (data->count <= index) {
        data->count = index + 1;
        data->array = ecs_os_realloc_n(data->array, int32_t, data->count);
    }
    return &data->array[index];
}

static 
void IntVec_resize(void *ptr, size_t size) {
    IntVec *data = ptr;
    test_assert(data != NULL);
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

static
size_t StringVec_count(const void *ptr) {
    const StringVec *data = ptr;
    test_assert(data != NULL);
    return data->count;
}

static 
void* StringVec_ensure(void *ptr, size_t index) {
    StringVec *data = ptr;
    test_assert(data != NULL);
    if (data->count <= index) {
        data->count = index + 1;
        data->array = ecs_os_realloc_n(data->array, char*, data->count);
        data->array[index] = NULL;
    }
    return &data->array[index];
}

static 
void StringVec_resize(void *ptr, size_t size) {
    StringVec *data = ptr;
    test_assert(data != NULL);
    if (data->count != size) {
        for (size_t i = size; i < data->count; i ++) {
            ecs_os_free(data->array[i]);
        }
        data->count = size;
        if (!data->count) {
            ecs_os_free(data->array);
            data->array = NULL;
        } else {
            data->array = ecs_os_realloc_n(data->array, char*, size);
        }
    }
}

static
size_t OpaqueStringVec_count(const void *ptr) {
    const OpaqueStringVec *data = ptr;
    test_assert(data != NULL);
    return data->count;
}

static 
void* OpaqueStringVec_ensure(void *ptr, size_t index) {
    OpaqueStringVec *data = ptr;
    test_assert(data != NULL);
    if (data->count <= index) {
        data->count = index + 1;
        data->array = ecs_os_realloc_n(data->array, String, data->count);
        data->array[index].value = NULL;
    }
    return &data->array[index];
}

static 
void OpaqueStringVec_resize(void *ptr, size_t size) {
    OpaqueStringVec *data = ptr;
    test_assert(data != NULL);
    if (data->count != size) {
        for (size_t i = size; i < data->count; i ++) {
            ecs_os_free(data->array[i].value);
        }
        data->count = size;
        if (!data->count) {
            ecs_os_free(data->array);
            data->array = NULL;
        } else {
            data->array = ecs_os_realloc_n(data->array, String, size);
        }
    }
}

void Deserialize_opaque_vector_i32_0(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, IntVec);

    ecs_entity_t vec = ecs_opaque(world, {
        .entity = ecs_id(IntVec),
        .type.as_type = ecs_vector(world, { .type = ecs_id(ecs_i32_t) }),
        .type.ensure_element = IntVec_ensure,
        .type.count = IntVec_count,
        .type.resize = IntVec_resize
    });

    IntVec value = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(world, "[]", &(ecs_value_t){vec, &value}, &desc);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_int(value.count, 0);
    test_assert(value.array == NULL);

    ecs_fini(world);
}

void Deserialize_opaque_vector_i32_2(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, IntVec);

    ecs_entity_t vec = ecs_opaque(world, {
        .entity = ecs_id(IntVec),
        .type.as_type = ecs_vector(world, { .type = ecs_id(ecs_i32_t) }),
        .type.ensure_element = IntVec_ensure,
        .type.count = IntVec_count,
        .type.resize = IntVec_resize
    });

    IntVec value = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(world, "[10, 20]",
        &(ecs_value_t){vec, &value}, &desc);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_int(value.count, 2);
    test_int(value.array[0], 10);
    test_int(value.array[1], 20);

    ecs_os_free(value.array);

    ecs_fini(world);
}

void Deserialize_opaque_vector_i32_0_into_2(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, IntVec);

    ecs_entity_t vec = ecs_opaque(world, {
        .entity = ecs_id(IntVec),
        .type.as_type = ecs_vector(world, { .type = ecs_id(ecs_i32_t) }),
        .type.ensure_element = IntVec_ensure,
        .type.count = IntVec_count,
        .type.resize = IntVec_resize
    });

    IntVec value = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(world, "[10, 20]",
        &(ecs_value_t){vec, &value}, &desc);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_int(value.count, 2);
    test_int(value.array[0], 10);
    test_int(value.array[1], 20);

    ptr = ecs_expr_run(world, "[]", &(ecs_value_t){vec, &value}, &desc);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_int(value.count, 0);

    ecs_os_free(value.array);

    ecs_fini(world);
}

void Deserialize_opaque_vector_i32_1_into_2(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, IntVec);

    ecs_entity_t vec = ecs_opaque(world, {
        .entity = ecs_id(IntVec),
        .type.as_type = ecs_vector(world, { .type = ecs_id(ecs_i32_t) }),
        .type.ensure_element = IntVec_ensure,
        .type.count = IntVec_count,
        .type.resize = IntVec_resize
    });

    IntVec value = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(world, "[10, 20]",
        &(ecs_value_t){vec, &value}, &desc);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_int(value.count, 2);
    test_int(value.array[0], 10);
    test_int(value.array[1], 20);

    ptr = ecs_expr_run(world, "[30]",
        &(ecs_value_t){vec, &value}, &desc);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_int(value.count, 1);
    test_int(value.array[0], 30);

    ecs_os_free(value.array);

    ecs_fini(world);
}

void Deserialize_opaque_vector_string_0(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, StringVec);

    ecs_entity_t vec = ecs_opaque(world, {
        .entity = ecs_id(StringVec),
        .type.as_type = ecs_vector(world, { .type = ecs_id(ecs_string_t) }),
        .type.ensure_element = StringVec_ensure,
        .type.count = StringVec_count,
        .type.resize = StringVec_resize
    });

    StringVec value = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(world, "[]", &(ecs_value_t){vec, &value}, &desc);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_int(value.count, 0);
    test_assert(value.array == NULL);

    ecs_fini(world);
}

void Deserialize_opaque_vector_string_2(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, StringVec);

    ecs_entity_t vec = ecs_opaque(world, {
        .entity = ecs_id(StringVec),
        .type.as_type = ecs_vector(world, { .type = ecs_id(ecs_string_t) }),
        .type.ensure_element = StringVec_ensure,
        .type.count = StringVec_count,
        .type.resize = StringVec_resize
    });

    StringVec value = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(world, "[\"Hello\", \"World\"]", &(ecs_value_t){vec, &value}, &desc);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_int(value.count, 2);
    test_str(value.array[0], "Hello");
    test_str(value.array[1], "World");

    ecs_os_free(value.array[0]);
    ecs_os_free(value.array[1]);
    ecs_os_free(value.array);

    ecs_fini(world);
}

void Deserialize_opaque_vector_string_0_into_2(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, StringVec);

    ecs_entity_t vec = ecs_opaque(world, {
        .entity = ecs_id(StringVec),
        .type.as_type = ecs_vector(world, { .type = ecs_id(ecs_string_t) }),
        .type.ensure_element = StringVec_ensure,
        .type.count = StringVec_count,
        .type.resize = StringVec_resize
    });

    StringVec value = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(world, "[\"Hello\", \"World\"]", &(ecs_value_t){vec, &value}, &desc);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_int(value.count, 2);
    test_str(value.array[0], "Hello");
    test_str(value.array[1], "World");

    ptr = ecs_expr_run(world, "[]", &(ecs_value_t){vec, &value}, &desc);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_int(value.count, 0);
    ecs_os_free(value.array);

    ecs_fini(world);
}

void Deserialize_opaque_vector_string_1_into_2(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, StringVec);

    ecs_entity_t vec = ecs_opaque(world, {
        .entity = ecs_id(StringVec),
        .type.as_type = ecs_vector(world, { .type = ecs_id(ecs_string_t) }),
        .type.ensure_element = StringVec_ensure,
        .type.count = StringVec_count,
        .type.resize = StringVec_resize
    });

    StringVec value = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(world, "[\"Hello\", \"World\"]", &(ecs_value_t){vec, &value}, &desc);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_int(value.count, 2);
    test_str(value.array[0], "Hello");
    test_str(value.array[1], "World");

    ptr = ecs_expr_run(world, "[\"Foo\"]", &(ecs_value_t){vec, &value}, &desc);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_int(value.count, 1);
    test_str(value.array[0], "Foo");
    ecs_os_free(value.array[0]);
    ecs_os_free(value.array);

    ecs_fini(world);
}

void Deserialize_opaque_vector_opaque_string_0(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, String);
    ECS_COMPONENT(world, StringVec);

    ecs_entity_t s = ecs_opaque(world, {
        .entity = ecs_id(String),
        .type.as_type = ecs_primitive(world, { .kind = EcsString }),
        .type.assign_string = String_assign
    });

    ecs_entity_t vec = ecs_opaque(world, {
        .entity = ecs_id(StringVec),
        .type.as_type = ecs_vector(world, { .type = s }),
        .type.ensure_element = StringVec_ensure,
        .type.count = StringVec_count,
        .type.resize = StringVec_resize
    });

    StringVec value = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(world, "[]", &(ecs_value_t){vec, &value}, &desc);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_int(value.count, 0);
    ecs_os_free(value.array);

    ecs_fini(world);
}

void Deserialize_opaque_vector_opaque_string_2(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, String);
    ECS_COMPONENT(world, OpaqueStringVec);

    ecs_entity_t s = ecs_opaque(world, {
        .entity = ecs_id(String),
        .type.as_type = ecs_primitive(world, { .kind = EcsString }),
        .type.assign_string = String_assign
    });

    ecs_entity_t vec = ecs_opaque(world, {
        .entity = ecs_id(OpaqueStringVec),
        .type.as_type = ecs_vector(world, { .type = s }),
        .type.ensure_element = OpaqueStringVec_ensure,
        .type.count = OpaqueStringVec_count,
        .type.resize = OpaqueStringVec_resize
    });

    ecs_set_hooks(world, String, {
        .ctor = ecs_ctor(String),
        .move = ecs_move(String),
        .copy = ecs_copy(String),
        .dtor = ecs_dtor(String)
    });

    OpaqueStringVec value = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(world, "[\"Hello\"]", &(ecs_value_t){vec, &value}, &desc);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_int(value.count, 1);
    test_str(value.array[0].value, "Hello");
    // test_str(value.array[1].value, "World");

    ecs_os_free(value.array[0].value);
    // ecs_os_free(value.array[1].value);
    ecs_os_free(value.array);

    ecs_fini(world);
}

void Deserialize_opaque_vector_opaque_string_0_into_2(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, String);
    ECS_COMPONENT(world, OpaqueStringVec);

    ecs_entity_t s = ecs_opaque(world, {
        .entity = ecs_id(String),
        .type.as_type = ecs_primitive(world, { .kind = EcsString }),
        .type.assign_string = String_assign
    });

    ecs_entity_t vec = ecs_opaque(world, {
        .entity = ecs_id(OpaqueStringVec),
        .type.as_type = ecs_vector(world, { .type = s }),
        .type.ensure_element = OpaqueStringVec_ensure,
        .type.count = OpaqueStringVec_count,
        .type.resize = OpaqueStringVec_resize
    });

    ecs_set_hooks(world, String, {
        .ctor = ecs_ctor(String),
        .move = ecs_move(String),
        .copy = ecs_copy(String),
        .dtor = ecs_dtor(String)
    });

    OpaqueStringVec value = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(world, "[\"Hello\", \"World\"]", &(ecs_value_t){vec, &value}, &desc);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_int(value.count, 2);
    test_str(value.array[0].value, "Hello");
    test_str(value.array[1].value, "World");

    ptr = ecs_expr_run(world, "[]", &(ecs_value_t){vec, &value}, &desc);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_int(value.count, 0);

    ecs_fini(world);
}

void Deserialize_opaque_vector_opaque_string_1_into_2(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, String);
    ECS_COMPONENT(world, OpaqueStringVec);

    ecs_entity_t s = ecs_opaque(world, {
        .entity = ecs_id(String),
        .type.as_type = ecs_primitive(world, { .kind = EcsString }),
        .type.assign_string = String_assign
    });

    ecs_set_hooks(world, String, {
        .ctor = ecs_ctor(String),
        .move = ecs_move(String),
        .copy = ecs_copy(String),
        .dtor = ecs_dtor(String)
    });

    ecs_entity_t vec = ecs_opaque(world, {
        .entity = ecs_id(OpaqueStringVec),
        .type.as_type = ecs_vector(world, { .type = s }),
        .type.ensure_element = OpaqueStringVec_ensure,
        .type.count = OpaqueStringVec_count,
        .type.resize = OpaqueStringVec_resize
    });

    OpaqueStringVec value = {0};
    ecs_expr_eval_desc_t desc = { .disable_folding = disable_folding };
    const char *ptr = ecs_expr_run(world, "[\"Hello\", \"World\"]", &(ecs_value_t){vec, &value}, &desc);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_int(value.count, 2);
    test_str(value.array[0].value, "Hello");
    test_str(value.array[1].value, "World");

    ptr = ecs_expr_run(world, "[\"Foo\"]", &(ecs_value_t){vec, &value}, &desc);
    test_assert(ptr != NULL);
    test_assert(ptr[0] == '\0');

    test_int(value.count, 1);
    test_str(value.array[0].value, "Foo");
    ecs_os_free(value.array[0].value);
    ecs_os_free(value.array);

    ecs_fini(world);
}
