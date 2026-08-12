#include <script.h>

void ConstVar_get_bool(void) {
    ecs_world_t *world = ecs_init();

    ecs_bool_t v = true;
    test_assert(0 != ecs_const_var(world, {
        .name = "v",
        .type = ecs_id(ecs_bool_t),
        .value = &v
    }));

    test_bool(ecs_const_var_get_t(world, "v", ecs_bool_t), true);

    ecs_fini(world);
}

void ConstVar_get_char(void) {
    ecs_world_t *world = ecs_init();

    ecs_char_t v = 'a';
    test_assert(0 != ecs_const_var(world, {
        .name = "v",
        .type = ecs_id(ecs_char_t),
        .value = &v
    }));

    test_int(ecs_const_var_get_t(world, "v", ecs_char_t), 'a');

    ecs_fini(world);
}

void ConstVar_get_byte(void) {
    ecs_world_t *world = ecs_init();

    ecs_byte_t v = 10;
    test_assert(0 != ecs_const_var(world, {
        .name = "v",
        .type = ecs_id(ecs_byte_t),
        .value = &v
    }));

    test_uint(ecs_const_var_get_t(world, "v", ecs_byte_t), 10);

    ecs_fini(world);
}

void ConstVar_get_u8(void) {
    ecs_world_t *world = ecs_init();

    ecs_u8_t v = 10;
    test_assert(0 != ecs_const_var(world, {
        .name = "v",
        .type = ecs_id(ecs_u8_t),
        .value = &v
    }));

    test_uint(ecs_const_var_get_t(world, "v", ecs_u8_t), 10);

    ecs_fini(world);
}

void ConstVar_get_u16(void) {
    ecs_world_t *world = ecs_init();

    ecs_u16_t v = 1000;
    test_assert(0 != ecs_const_var(world, {
        .name = "v",
        .type = ecs_id(ecs_u16_t),
        .value = &v
    }));

    test_uint(ecs_const_var_get_t(world, "v", ecs_u16_t), 1000);

    ecs_fini(world);
}

void ConstVar_get_u32(void) {
    ecs_world_t *world = ecs_init();

    ecs_u32_t v = 100000;
    test_assert(0 != ecs_const_var(world, {
        .name = "v",
        .type = ecs_id(ecs_u32_t),
        .value = &v
    }));

    test_uint(ecs_const_var_get_t(world, "v", ecs_u32_t), 100000);

    ecs_fini(world);
}

void ConstVar_get_u64(void) {
    ecs_world_t *world = ecs_init();

    ecs_u64_t v = 10000000000;
    test_assert(0 != ecs_const_var(world, {
        .name = "v",
        .type = ecs_id(ecs_u64_t),
        .value = &v
    }));

    test_uint(ecs_const_var_get_t(world, "v", ecs_u64_t), 10000000000);

    ecs_fini(world);
}

void ConstVar_get_i8(void) {
    ecs_world_t *world = ecs_init();

    ecs_i8_t v = -10;
    test_assert(0 != ecs_const_var(world, {
        .name = "v",
        .type = ecs_id(ecs_i8_t),
        .value = &v
    }));

    test_int(ecs_const_var_get_t(world, "v", ecs_i8_t), -10);

    ecs_fini(world);
}

void ConstVar_get_i16(void) {
    ecs_world_t *world = ecs_init();

    ecs_i16_t v = -1000;
    test_assert(0 != ecs_const_var(world, {
        .name = "v",
        .type = ecs_id(ecs_i16_t),
        .value = &v
    }));

    test_int(ecs_const_var_get_t(world, "v", ecs_i16_t), -1000);

    ecs_fini(world);
}

void ConstVar_get_i32(void) {
    ecs_world_t *world = ecs_init();

    ecs_i32_t v = -100000;
    test_assert(0 != ecs_const_var(world, {
        .name = "v",
        .type = ecs_id(ecs_i32_t),
        .value = &v
    }));

    test_int(ecs_const_var_get_t(world, "v", ecs_i32_t), -100000);

    ecs_fini(world);
}

void ConstVar_get_i64(void) {
    ecs_world_t *world = ecs_init();

    ecs_i64_t v = -10000000000;
    test_assert(0 != ecs_const_var(world, {
        .name = "v",
        .type = ecs_id(ecs_i64_t),
        .value = &v
    }));

    test_int(ecs_const_var_get_t(world, "v", ecs_i64_t), -10000000000);

    ecs_fini(world);
}

void ConstVar_get_f32(void) {
    ecs_world_t *world = ecs_init();

    ecs_f32_t v = 10.5;
    test_assert(0 != ecs_const_var(world, {
        .name = "v",
        .type = ecs_id(ecs_f32_t),
        .value = &v
    }));

    test_flt(ecs_const_var_get_t(world, "v", ecs_f32_t), 10.5);

    ecs_fini(world);
}

void ConstVar_get_f64(void) {
    ecs_world_t *world = ecs_init();

    ecs_f64_t v = 10.5;
    test_assert(0 != ecs_const_var(world, {
        .name = "v",
        .type = ecs_id(ecs_f64_t),
        .value = &v
    }));

    test_flt(ecs_const_var_get_t(world, "v", ecs_f64_t), 10.5);

    ecs_fini(world);
}

void ConstVar_get_uptr(void) {
    ecs_world_t *world = ecs_init();

    ecs_uptr_t v = 1000;
    test_assert(0 != ecs_const_var(world, {
        .name = "v",
        .type = ecs_id(ecs_uptr_t),
        .value = &v
    }));

    test_uint(ecs_const_var_get_t(world, "v", ecs_uptr_t), 1000);

    ecs_fini(world);
}

void ConstVar_get_iptr(void) {
    ecs_world_t *world = ecs_init();

    ecs_iptr_t v = -1000;
    test_assert(0 != ecs_const_var(world, {
        .name = "v",
        .type = ecs_id(ecs_iptr_t),
        .value = &v
    }));

    test_int(ecs_const_var_get_t(world, "v", ecs_iptr_t), -1000);

    ecs_fini(world);
}

void ConstVar_get_string(void) {
    ecs_world_t *world = ecs_init();

    ecs_string_t v = "Hello World";
    test_assert(0 != ecs_const_var(world, {
        .name = "v",
        .type = ecs_id(ecs_string_t),
        .value = &v
    }));

    ecs_string_t str = ecs_const_var_get_t(world, "v", ecs_string_t);
    test_str(str, "Hello World");
    ecs_os_free(str);

    ecs_fini(world);
}

void ConstVar_get_string_is_owned(void) {
    ecs_world_t *world = ecs_init();

    ecs_string_t v = "Hello World";
    ecs_entity_t var = ecs_const_var(world, {
        .name = "v",
        .type = ecs_id(ecs_string_t),
        .value = &v
    });
    test_assert(var != 0);

    ecs_value_t value = ecs_const_var_get(world, var);
    test_assert(value.ptr != NULL);

    ecs_string_t str = ecs_const_var_get_t(world, "v", ecs_string_t);
    test_str(str, "Hello World");
    test_assert(str != *(ecs_string_t*)value.ptr);
    ecs_os_free(str);

    test_str(*(ecs_string_t*)value.ptr, "Hello World");

    ecs_fini(world);
}

typedef struct {
    char *name;
    int32_t value;
} StringStruct;

void ConstVar_get_struct_w_string_is_owned(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(StringStruct) = ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "StringStruct"}),
        .members = {
            {"name", ecs_id(ecs_string_t)},
            {"value", ecs_id(ecs_i32_t)}
        }
    });

    StringStruct v = { "Hello World", 10 };
    ecs_entity_t var = ecs_const_var(world, {
        .name = "v",
        .type = ecs_id(StringStruct),
        .value = &v
    });
    test_assert(var != 0);

    ecs_value_t value = ecs_const_var_get(world, var);
    test_assert(value.ptr != NULL);

    StringStruct s = ecs_const_var_get_t(world, "v", StringStruct);
    test_str(s.name, "Hello World");
    test_int(s.value, 10);
    test_assert(s.name != ((StringStruct*)value.ptr)->name);
    ecs_ptr_fini(world, ecs_id(StringStruct), &s);

    test_str(((StringStruct*)value.ptr)->name, "Hello World");

    ecs_fini(world);
}

void ConstVar_get_entity(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t e = ecs_entity(world, { .name = "e" });

    ecs_entity_t v = e;
    test_assert(0 != ecs_const_var(world, {
        .name = "v",
        .type = ecs_id(ecs_entity_t),
        .value = &v
    }));

    test_uint(ecs_const_var_get_t(world, "v", ecs_entity_t), e);

    ecs_fini(world);
}

void ConstVar_get_id(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t e = ecs_entity(world, { .name = "e" });

    ecs_id_t v = ecs_pair(EcsChildOf, e);
    test_assert(0 != ecs_const_var(world, {
        .name = "v",
        .type = ecs_id(ecs_id_t),
        .value = &v
    }));

    test_uint(ecs_const_var_get_t(world, "v", ecs_id_t),
        ecs_pair(EcsChildOf, e));

    ecs_fini(world);
}

void ConstVar_get_struct(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Point) = ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "Point"}),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    Point v = { 10.5, 20.5 };
    test_assert(0 != ecs_const_var(world, {
        .name = "v",
        .type = ecs_id(Point),
        .value = &v
    }));

    Point p = ecs_const_var_get_t(world, "v", Point);
    test_flt(p.x, 10.5);
    test_flt(p.y, 20.5);

    ecs_fini(world);
}

void ConstVar_get_nested_struct(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Point) = ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "Point"}),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    ecs_entity_t ecs_id(Line) = ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "Line"}),
        .members = {
            {"start", ecs_id(Point)},
            {"stop", ecs_id(Point)}
        }
    });

    Line v = { {10.5, 20.5}, {30.5, 40.5} };
    test_assert(0 != ecs_const_var(world, {
        .name = "v",
        .type = ecs_id(Line),
        .value = &v
    }));

    Line l = ecs_const_var_get_t(world, "v", Line);
    test_flt(l.start.x, 10.5);
    test_flt(l.start.y, 20.5);
    test_flt(l.stop.x, 30.5);
    test_flt(l.stop.y, 40.5);

    ecs_fini(world);
}

void ConstVar_get_in_parent(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t parent = ecs_entity(world, { .name = "parent" });

    ecs_i32_t v = 10;
    test_assert(0 != ecs_const_var(world, {
        .name = "x",
        .parent = parent,
        .type = ecs_id(ecs_i32_t),
        .value = &v
    }));

    test_int(ecs_const_var_get_t(world, "parent.x", ecs_i32_t), 10);

    ecs_fini(world);
}

void ConstVar_get_from_script(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "export const x: i32 = 10";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    test_int(ecs_const_var_get_t(world, "x", ecs_i32_t), 10);

    ecs_fini(world);
}

void ConstVar_get_not_found(void) {
    ecs_world_t *world = ecs_init();

    ecs_log_set_level(-4);

    ecs_i32_t v = 10;
    test_assert(0 != ecs_const_var(world, {
        .name = "v",
        .type = ecs_id(ecs_i32_t),
        .value = &v
    }));

    test_int(ecs_const_var_get_t(world, "w", ecs_i32_t), 0);

    ecs_fini(world);
}

void ConstVar_get_not_a_const_var(void) {
    ecs_world_t *world = ecs_init();

    ecs_log_set_level(-4);

    ecs_entity(world, { .name = "v" });

    test_int(ecs_const_var_get_t(world, "v", ecs_i32_t), 0);

    ecs_fini(world);
}

void ConstVar_get_w_invalid_size(void) {
    ecs_world_t *world = ecs_init();

    ecs_log_set_level(-4);

    ecs_i32_t v = 10;
    test_assert(0 != ecs_const_var(world, {
        .name = "v",
        .type = ecs_id(ecs_i32_t),
        .value = &v
    }));

    ecs_i64_t out = 0;

    void *ptr = ecs_const_var_get_w_type(
        world, "v", ecs_id(ecs_i32_t), ECS_SIZEOF(ecs_i64_t), &out);
    test_assert(ptr == &out);
    test_int(out, 0);

    ecs_fini(world);
}

void ConstVar_get_w_invalid_type(void) {
    ecs_world_t *world = ecs_init();

    ecs_log_set_level(-4);

    ecs_i32_t v = 10;
    test_assert(0 != ecs_const_var(world, {
        .name = "v",
        .type = ecs_id(ecs_i32_t),
        .value = &v
    }));

    ecs_entity_t not_a_type = ecs_entity(world, { .name = "not_a_type" });
    ecs_i32_t out = 0;

    void *ptr = ecs_const_var_get_w_type(
        world, "v", not_a_type, ECS_SIZEOF(ecs_i32_t), &out);
    test_assert(ptr == &out);
    test_int(out, 0);

    ecs_fini(world);
}

void ConstVar_get_i32_as_i64(void) {
    ecs_world_t *world = ecs_init();

    ecs_i32_t v = -10;
    test_assert(0 != ecs_const_var(world, {
        .name = "v",
        .type = ecs_id(ecs_i32_t),
        .value = &v
    }));

    test_int(ecs_const_var_get_t(world, "v", ecs_i64_t), -10);

    ecs_fini(world);
}

void ConstVar_get_i32_as_f64(void) {
    ecs_world_t *world = ecs_init();

    ecs_i32_t v = 10;
    test_assert(0 != ecs_const_var(world, {
        .name = "v",
        .type = ecs_id(ecs_i32_t),
        .value = &v
    }));

    test_flt(ecs_const_var_get_t(world, "v", ecs_f64_t), 10);

    ecs_fini(world);
}

void ConstVar_get_f64_as_i32(void) {
    ecs_world_t *world = ecs_init();

    ecs_f64_t v = 10.0;
    test_assert(0 != ecs_const_var(world, {
        .name = "v",
        .type = ecs_id(ecs_f64_t),
        .value = &v
    }));

    test_int(ecs_const_var_get_t(world, "v", ecs_i32_t), 10);

    ecs_fini(world);
}

void ConstVar_get_u32_as_u64(void) {
    ecs_world_t *world = ecs_init();

    ecs_u32_t v = 100000;
    test_assert(0 != ecs_const_var(world, {
        .name = "v",
        .type = ecs_id(ecs_u32_t),
        .value = &v
    }));

    test_uint(ecs_const_var_get_t(world, "v", ecs_u64_t), 100000);

    ecs_fini(world);
}

void ConstVar_get_u8_as_i32(void) {
    ecs_world_t *world = ecs_init();

    ecs_u8_t v = 10;
    test_assert(0 != ecs_const_var(world, {
        .name = "v",
        .type = ecs_id(ecs_u8_t),
        .value = &v
    }));

    test_int(ecs_const_var_get_t(world, "v", ecs_i32_t), 10);

    ecs_fini(world);
}

void ConstVar_get_bool_as_i32(void) {
    ecs_world_t *world = ecs_init();

    ecs_bool_t v = true;
    test_assert(0 != ecs_const_var(world, {
        .name = "v",
        .type = ecs_id(ecs_bool_t),
        .value = &v
    }));

    test_int(ecs_const_var_get_t(world, "v", ecs_i32_t), 1);

    ecs_fini(world);
}

void ConstVar_get_i32_as_string(void) {
    ecs_world_t *world = ecs_init();

    ecs_i32_t v = 10;
    test_assert(0 != ecs_const_var(world, {
        .name = "v",
        .type = ecs_id(ecs_i32_t),
        .value = &v
    }));

    ecs_string_t str = ecs_const_var_get_t(world, "v", ecs_string_t);
    test_str(str, "10");
    ecs_os_free(str);

    ecs_fini(world);
}

void ConstVar_get_string_as_i32(void) {
    ecs_world_t *world = ecs_init();

    ecs_string_t v = "10";
    test_assert(0 != ecs_const_var(world, {
        .name = "v",
        .type = ecs_id(ecs_string_t),
        .value = &v
    }));

    test_int(ecs_const_var_get_t(world, "v", ecs_i32_t), 10);

    ecs_fini(world);
}

void ConstVar_get_struct_as_i32(void) {
    ecs_world_t *world = ecs_init();

    ecs_log_set_level(-4);

    ecs_entity_t ecs_id(Point) = ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "Point"}),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    Point v = { 10.5, 20.5 };
    test_assert(0 != ecs_const_var(world, {
        .name = "v",
        .type = ecs_id(Point),
        .value = &v
    }));

    test_int(ecs_const_var_get_t(world, "v", ecs_i32_t), 0);

    ecs_fini(world);
}

void ConstVar_get_i32_as_struct(void) {
    ecs_world_t *world = ecs_init();

    ecs_log_set_level(-4);

    ecs_entity_t ecs_id(Point) = ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "Point"}),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });
    test_assert(ecs_id(Point) != 0);

    ecs_i32_t v = 10;
    test_assert(0 != ecs_const_var(world, {
        .name = "v",
        .type = ecs_id(ecs_i32_t),
        .value = &v
    }));

    Point p = ecs_const_var_get_t(world, "v", Point);
    test_flt(p.x, 0);
    test_flt(p.y, 0);

    ecs_fini(world);
}

void ConstVar_get_struct_as_other_struct(void) {
    ecs_world_t *world = ecs_init();

    ecs_log_set_level(-4);

    ecs_entity_t ecs_id(Point) = ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "Point"}),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    ecs_entity_t ecs_id(Mass) = ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "Mass"}),
        .members = {
            {"value", ecs_id(ecs_f32_t)}
        }
    });
    test_assert(ecs_id(Mass) != 0);

    Point v = { 10.5, 20.5 };
    test_assert(0 != ecs_const_var(world, {
        .name = "v",
        .type = ecs_id(Point),
        .value = &v
    }));

    Mass m = ecs_const_var_get_t(world, "v", Mass);
    test_flt(m.value, 0);

    ecs_fini(world);
}
