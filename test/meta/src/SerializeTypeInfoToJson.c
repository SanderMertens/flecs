#include <meta.h>

void SerializeTypeInfoToJson_bool(void) {
    ecs_world_t *world = ecs_init();

    char *str = ecs_type_info_to_json(world, ecs_id(ecs_bool_t));
    test_assert(str != NULL);
    test_str(str, "[\"bool\"]");
    ecs_os_free(str);

    ecs_fini(world);
}

void SerializeTypeInfoToJson_byte(void) {
    ecs_world_t *world = ecs_init();

    char *str = ecs_type_info_to_json(world, ecs_id(ecs_byte_t));
    test_assert(str != NULL);
    test_str(str, "[\"byte\"]");
    ecs_os_free(str);

    ecs_fini(world);
}

void SerializeTypeInfoToJson_char(void) {
    ecs_world_t *world = ecs_init();

    char *str = ecs_type_info_to_json(world, ecs_id(ecs_char_t));
    test_assert(str != NULL);
    test_str(str, "[\"text\"]");
    ecs_os_free(str);

    ecs_fini(world);
}

void SerializeTypeInfoToJson_i8(void) {
    ecs_world_t *world = ecs_init();

    char *str = ecs_type_info_to_json(world, ecs_id(ecs_i8_t));
    test_assert(str != NULL);
    test_str(str, "[\"int\"]");
    ecs_os_free(str);

    ecs_fini(world);
}

void SerializeTypeInfoToJson_i16(void) {
    ecs_world_t *world = ecs_init();

    char *str = ecs_type_info_to_json(world, ecs_id(ecs_i16_t));
    test_assert(str != NULL);
    test_str(str, "[\"int\"]");
    ecs_os_free(str);

    ecs_fini(world);
}

void SerializeTypeInfoToJson_i32(void) {
    ecs_world_t *world = ecs_init();

    char *str = ecs_type_info_to_json(world, ecs_id(ecs_i32_t));
    test_assert(str != NULL);
    test_str(str, "[\"int\"]");
    ecs_os_free(str);

    ecs_fini(world);
}

void SerializeTypeInfoToJson_i64(void) {
    ecs_world_t *world = ecs_init();

    char *str = ecs_type_info_to_json(world, ecs_id(ecs_i64_t));
    test_assert(str != NULL);
    test_str(str, "[\"int\"]");
    ecs_os_free(str);

    ecs_fini(world);
}

void SerializeTypeInfoToJson_iptr(void) {
    ecs_world_t *world = ecs_init();

    char *str = ecs_type_info_to_json(world, ecs_id(ecs_iptr_t));
    test_assert(str != NULL);
    test_str(str, "[\"int\"]");
    ecs_os_free(str);

    ecs_fini(world);
}

void SerializeTypeInfoToJson_u8(void) {
    ecs_world_t *world = ecs_init();

    char *str = ecs_type_info_to_json(world, ecs_id(ecs_u8_t));
    test_assert(str != NULL);
    test_str(str, "[\"int\"]");
    ecs_os_free(str);

    ecs_fini(world);
}

void SerializeTypeInfoToJson_u16(void) {
    ecs_world_t *world = ecs_init();

    char *str = ecs_type_info_to_json(world, ecs_id(ecs_u16_t));
    test_assert(str != NULL);
    test_str(str, "[\"int\"]");
    ecs_os_free(str);

    ecs_fini(world);
}

void SerializeTypeInfoToJson_u32(void) {
    ecs_world_t *world = ecs_init();

    char *str = ecs_type_info_to_json(world, ecs_id(ecs_u32_t));
    test_assert(str != NULL);
    test_str(str, "[\"int\"]");
    ecs_os_free(str);

    ecs_fini(world);
}

void SerializeTypeInfoToJson_u64(void) {
    ecs_world_t *world = ecs_init();

    char *str = ecs_type_info_to_json(world, ecs_id(ecs_u64_t));
    test_assert(str != NULL);
    test_str(str, "[\"int\"]");
    ecs_os_free(str);

    ecs_fini(world);
}

void SerializeTypeInfoToJson_uptr(void) {
    ecs_world_t *world = ecs_init();

    char *str = ecs_type_info_to_json(world, ecs_id(ecs_uptr_t));
    test_assert(str != NULL);
    test_str(str, "[\"int\"]");
    ecs_os_free(str);

    ecs_fini(world);
}

void SerializeTypeInfoToJson_float(void) {
    ecs_world_t *world = ecs_init();

    char *str = ecs_type_info_to_json(world, ecs_id(ecs_f32_t));
    test_assert(str != NULL);
    test_str(str, "[\"float\"]");
    ecs_os_free(str);

    ecs_fini(world);
}

void SerializeTypeInfoToJson_double(void) {
    ecs_world_t *world = ecs_init();

    char *str = ecs_type_info_to_json(world, ecs_id(ecs_f64_t));
    test_assert(str != NULL);
    test_str(str, "[\"float\"]");
    ecs_os_free(str);

    ecs_fini(world);
}

void SerializeTypeInfoToJson_string(void) {
    ecs_world_t *world = ecs_init();

    char *str = ecs_type_info_to_json(world, ecs_id(ecs_string_t));
    test_assert(str != NULL);
    test_str(str, "[\"text\"]");
    ecs_os_free(str);

    ecs_fini(world);
}

void SerializeTypeInfoToJson_entity(void) {
    ecs_world_t *world = ecs_init();

    char *str = ecs_type_info_to_json(world, ecs_id(ecs_entity_t));
    test_assert(str != NULL);
    test_str(str, "[\"entity\"]");
    ecs_os_free(str);

    ecs_fini(world);
}

void SerializeTypeInfoToJson_id(void) {
    ecs_world_t *world = ecs_init();

    char *str = ecs_type_info_to_json(world, ecs_id(ecs_id_t));
    test_assert(str != NULL);
    test_str(str, "[\"id\"]");
    ecs_os_free(str);

    ecs_fini(world);
}

void SerializeTypeInfoToJson_enum(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t e = ecs_enum_init(world, &(ecs_enum_desc_t){
        .constants = {
            {"Red"}, {"Blue"}, {"Green"}
        }
    });

    char *str = ecs_type_info_to_json(world, e);
    test_assert(str != NULL);
    test_str(str, "[\"enum\", \"Red\", \"Blue\", \"Green\"]");
    ecs_os_free(str);
    
    ecs_fini(world);
}

void SerializeTypeInfoToJson_bitmask(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t b = ecs_bitmask_init(world, &(ecs_bitmask_desc_t){
        .constants = {
            {"Red"}, {"Blue"}, {"Green"}
        }
    });

    char *str = ecs_type_info_to_json(world, b);
    test_assert(str != NULL);
    test_str(str, "[\"bitmask\", \"Red\", \"Blue\", \"Green\"]");
    ecs_os_free(str);
    
    ecs_fini(world);
}

void SerializeTypeInfoToJson_struct(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    char *str = ecs_type_info_to_json(world, t);
    test_assert(str != NULL);
    test_str(str, "{\"x\":[\"float\"], \"y\":[\"float\"]}");
    ecs_os_free(str);

    ecs_fini(world);
}

void SerializeTypeInfoToJson_nested_struct(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    ecs_entity_t l = ecs_struct_init(world, &(ecs_struct_desc_t){
        .members = {
            {"start", t},
            {"stop", t}
        }
    });

    char *str = ecs_type_info_to_json(world, l);
    test_assert(str != NULL);
    test_str(str, "{\"start\":{\"x\":[\"float\"], \"y\":[\"float\"]}, \"stop\":{\"x\":[\"float\"], \"y\":[\"float\"]}}");
    ecs_os_free(str);

    ecs_fini(world);
}

void SerializeTypeInfoToJson_array_type(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t at = ecs_array(world, {
        .type = ecs_id(ecs_i32_t),
        .count = 3
    });

    char *str = ecs_type_info_to_json(world, at);
    test_assert(str != NULL);
    test_str(str, "[\"array\", [\"int\"], 3]");
    ecs_os_free(str);

    ecs_fini(world);
}

void SerializeTypeInfoToJson_vector_type(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t at = ecs_vector(world, {
        .type = ecs_id(ecs_i32_t)
    });

    char *str = ecs_type_info_to_json(world, at);
    test_assert(str != NULL);
    test_str(str, "[\"vector\", [\"int\"]]");
    ecs_os_free(str);

    ecs_fini(world);
}

void SerializeTypeInfoToJson_struct_array_i32_2(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "T"}),
        .members = {
            {"a", ecs_id(ecs_i32_t), 2}
        }
    });

    char *str = ecs_type_info_to_json(world, t);
    test_assert(str != NULL);
    test_str(str, "{\"a\":[\"array\", [\"int\"], 2]}");
    ecs_os_free(str);

    ecs_fini(world);
}

void SerializeTypeInfoToJson_struct_array_struct_2(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t u = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "U"}),
        .members = {
            {"x", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "T"}),
        .members = {
            {"a", u, 2}
        }
    });

    char *str = ecs_type_info_to_json(world, t);
    test_assert(str != NULL);
    test_str(str, "{\"a\":[\"array\", {\"x\":[\"int\"]}, 2]}");
    ecs_os_free(str);

    ecs_fini(world);
}

void SerializeTypeInfoToJson_struct_array_type(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t a = ecs_array_init(world, &(ecs_array_desc_t){
        .type = ecs_id(ecs_i32_t),
        .count = 3
    });

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "T"}),
        .members = {
            {"a", a}
        }
    });

    char *str = ecs_type_info_to_json(world, t);
    test_assert(str != NULL);
    test_str(str, "{\"a\":[\"array\", [\"int\"], 3]}");
    ecs_os_free(str);

    ecs_fini(world);
}

void SerializeTypeInfoToJson_struct_vector_type(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t a = ecs_vector_init(world, &(ecs_vector_desc_t){
        .type = ecs_id(ecs_i32_t)
    });

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "T"}),
        .members = {
            {"a", a}
        }
    });

    char *str = ecs_type_info_to_json(world, t);
    test_assert(str != NULL);
    test_str(str, "{\"a\":[\"vector\", [\"int\"]]}");
    ecs_os_free(str);

    ecs_fini(world);
}

static
int DummySerialize(const ecs_serializer_t *s, const void *ptr) {
    return 0;
}

void SerializeTypeInfoToJson_custom_primitive_type(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ct = ecs_opaque(world, {
        .entity = ecs_component(world, {
            .type.size = 4,
            .type.alignment = 4
        }),
        .type.as_type = ecs_id(ecs_i32_t),
        .type.serialize = DummySerialize
    });

    char *str = ecs_type_info_to_json(world, ct);
    test_assert(str != NULL);
    test_str(str, "[\"int\"]");
    ecs_os_free(str);

    ecs_fini(world);
}

void SerializeTypeInfoToJson_custom_array_type(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t at = ecs_array(world, {
        .type = ecs_id(ecs_i32_t),
        .count = 3
    });

    char *str = ecs_type_info_to_json(world, at);
    test_assert(str != NULL);
    test_str(str, "[\"array\", [\"int\"], 3]");
    ecs_os_free(str);

    ecs_fini(world);
}

void SerializeTypeInfoToJson_custom_vector_type(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ct = ecs_opaque(world, {
        .entity = ecs_component(world, {
            .type.size = 4,
            .type.alignment = 4
        }),
        .type.as_type = ecs_vector(world, {
            .type = ecs_id(ecs_i32_t)
        }),
        .type.serialize = DummySerialize
    });

    char *str = ecs_type_info_to_json(world, ct);
    test_assert(str != NULL);
    test_str(str, "[\"vector\", [\"int\"]]");
    ecs_os_free(str);

    ecs_fini(world);
}

void SerializeTypeInfoToJson_custom_struct_type(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t s = ecs_struct(world, {
        .members = {
            { "x", ecs_id(ecs_i32_t) },
            { "y", ecs_id(ecs_i32_t) }
        }
    });

    ecs_entity_t ct = ecs_opaque(world, {
        .entity = ecs_component(world, {
            .type.size = 4,
            .type.alignment = 4
        }),
        .type.as_type = s,
        .type.serialize = DummySerialize
    });

    char *str = ecs_type_info_to_json(world, ct);
    test_assert(str != NULL);
    test_str(str, "{\"x\":[\"int\"], \"y\":[\"int\"]}");
    ecs_os_free(str);

    ecs_fini(world);
}

void SerializeTypeInfoToJson_struct_w_value_range(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t s = ecs_struct(world, {
        .members = {
            { "value", ecs_id(ecs_i32_t), .range = {-1, 1} }
        }
    });

    char *str = ecs_type_info_to_json(world, s);
    test_assert(str != NULL);
    test_str(str, "{\"value\":[\"int\", {\"range\":[-1, 1]}]}");
    ecs_os_free(str);

    ecs_fini(world);
}

void SerializeTypeInfoToJson_struct_w_error_range(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t s = ecs_struct(world, {
        .members = {
            { "value", ecs_id(ecs_i32_t), .error_range = {-1, 1} }
        }
    });

    char *str = ecs_type_info_to_json(world, s);
    test_assert(str != NULL);
    test_str(str, "{\"value\":[\"int\", {\"error_range\":[-1, 1]}]}");
    ecs_os_free(str);

    ecs_fini(world);
}

void SerializeTypeInfoToJson_struct_w_warning_range(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t s = ecs_struct(world, {
        .members = {
            { "value", ecs_id(ecs_i32_t), .warning_range = {-1, 1} }
        }
    });

    char *str = ecs_type_info_to_json(world, s);
    test_assert(str != NULL);
    test_str(str, "{\"value\":[\"int\", {\"warning_range\":[-1, 1]}]}");
    ecs_os_free(str);

    ecs_fini(world);
}

void SerializeTypeInfoToJson_struct_w_error_and_warning_range(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t s = ecs_struct(world, {
        .members = {
            { "value", ecs_id(ecs_i32_t), 
                .warning_range = {-1, 1}, 
                .error_range = {-2, 2} 
            }
        }
    });

    char *str = ecs_type_info_to_json(world, s);
    test_assert(str != NULL);
    test_str(str, "{\"value\":[\"int\", {\"error_range\":[-2, 2],"
        " \"warning_range\":[-1, 1]}]}");
    ecs_os_free(str);

    ecs_fini(world);
}

void SerializeTypeInfoToJson_struct_nested(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t s = ecs_struct(world, {
        .members = {
            { "x", ecs_id(ecs_i32_t) },
            { "y", ecs_id(ecs_i32_t) }
        }
    });

    ecs_entity_t n = ecs_struct(world, {
        .members = {
            { "one", s }
        }
    });

    char *ti = ecs_type_info_to_json(world, n);
    test_assert(ti != NULL);
    test_str(ti, 
        "{\"one\":{\"x\":[\"int\"], \"y\":[\"int\"]}}"
    );
    ecs_os_free(ti);

    ecs_fini(world);
}

void SerializeTypeInfoToJson_struct_nested_2_lvls(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t s = ecs_struct(world, {
        .members = {
            { "a", ecs_id(ecs_i32_t) },
            { "b", ecs_id(ecs_i32_t) },
            { "c", ecs_id(ecs_i32_t) }
        }
    });

    ecs_entity_t n_1 = ecs_struct(world, {
        .members = {
            { "x", s },
            { "y", s }
        }
    });

    ecs_entity_t n_2 = ecs_struct(world, {
        .members = {
            { "one", n_1 }
        }
    });

    char *ti = ecs_type_info_to_json(world, n_2);
    test_assert(ti != NULL);
    test_str(ti, 
        "{\"one\":{"
            "\"x\":{\"a\":[\"int\"], \"b\":[\"int\"], \"c\":[\"int\"]}, "
            "\"y\":{\"a\":[\"int\"], \"b\":[\"int\"], \"c\":[\"int\"]}"
        "}}"
    );
    ecs_os_free(ti);

    ecs_fini(world);
}

void SerializeTypeInfoToJson_struct_nested_2_members(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t s = ecs_struct(world, {
        .members = {
            { "x", ecs_id(ecs_i32_t) },
            { "y", ecs_id(ecs_i32_t) }
        }
    });

    ecs_entity_t n = ecs_struct(world, {
        .members = {
            { "one", s },
            { "two", s },
        }
    });

    char *ti = ecs_type_info_to_json(world, n);
    test_assert(ti != NULL);
    test_str(ti, 
        "{\"one\":{\"x\":[\"int\"], \"y\":[\"int\"]}, \"two\":{\"x\":[\"int\"], \"y\":[\"int\"]}}"
    );
    ecs_os_free(ti);

    ecs_fini(world);
}

void SerializeTypeInfoToJson_struct_nested_3_members(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t s = ecs_struct(world, {
        .members = {
            { "x", ecs_id(ecs_i32_t) },
            { "y", ecs_id(ecs_i32_t) }
        }
    });

    ecs_entity_t n = ecs_struct(world, {
        .members = {
            { "one", s },
            { "two", ecs_id(ecs_i32_t) },
            { "three", s },
        }
    });

    char *ti = ecs_type_info_to_json(world, n);
    test_assert(ti != NULL);
    test_str(ti, 
        "{"
          "\"one\":{\"x\":[\"int\"], \"y\":[\"int\"]}, "
          "\"two\":[\"int\"], "
          "\"three\":{\"x\":[\"int\"], \"y\":[\"int\"]}"
        "}"
    );
    ecs_os_free(ti);

    ecs_fini(world);
}
