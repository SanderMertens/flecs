#include <meta.h>

void SerializeTypeInfoToJson_bool() {
    ecs_world_t *world = ecs_init();

    char *str = ecs_type_info_to_json(world, ecs_id(ecs_bool_t));
    test_assert(str != NULL);
    test_str(str, "[\"bool\"]");
    ecs_os_free(str);

    ecs_fini(world);
}

void SerializeTypeInfoToJson_byte() {
    ecs_world_t *world = ecs_init();

    char *str = ecs_type_info_to_json(world, ecs_id(ecs_byte_t));
    test_assert(str != NULL);
    test_str(str, "[\"byte\"]");
    ecs_os_free(str);

    ecs_fini(world);
}

void SerializeTypeInfoToJson_char() {
    ecs_world_t *world = ecs_init();

    char *str = ecs_type_info_to_json(world, ecs_id(ecs_char_t));
    test_assert(str != NULL);
    test_str(str, "[\"text\"]");
    ecs_os_free(str);

    ecs_fini(world);
}

void SerializeTypeInfoToJson_i8() {
    ecs_world_t *world = ecs_init();

    char *str = ecs_type_info_to_json(world, ecs_id(ecs_i8_t));
    test_assert(str != NULL);
    test_str(str, "[\"int\"]");
    ecs_os_free(str);

    ecs_fini(world);
}

void SerializeTypeInfoToJson_i16() {
    ecs_world_t *world = ecs_init();

    char *str = ecs_type_info_to_json(world, ecs_id(ecs_i16_t));
    test_assert(str != NULL);
    test_str(str, "[\"int\"]");
    ecs_os_free(str);

    ecs_fini(world);
}

void SerializeTypeInfoToJson_i32() {
    ecs_world_t *world = ecs_init();

    char *str = ecs_type_info_to_json(world, ecs_id(ecs_i32_t));
    test_assert(str != NULL);
    test_str(str, "[\"int\"]");
    ecs_os_free(str);

    ecs_fini(world);
}

void SerializeTypeInfoToJson_i64() {
    ecs_world_t *world = ecs_init();

    char *str = ecs_type_info_to_json(world, ecs_id(ecs_i64_t));
    test_assert(str != NULL);
    test_str(str, "[\"int\"]");
    ecs_os_free(str);

    ecs_fini(world);
}

void SerializeTypeInfoToJson_iptr() {
    ecs_world_t *world = ecs_init();

    char *str = ecs_type_info_to_json(world, ecs_id(ecs_iptr_t));
    test_assert(str != NULL);
    test_str(str, "[\"int\"]");
    ecs_os_free(str);

    ecs_fini(world);
}

void SerializeTypeInfoToJson_u8() {
    ecs_world_t *world = ecs_init();

    char *str = ecs_type_info_to_json(world, ecs_id(ecs_u8_t));
    test_assert(str != NULL);
    test_str(str, "[\"int\"]");
    ecs_os_free(str);

    ecs_fini(world);
}

void SerializeTypeInfoToJson_u16() {
    ecs_world_t *world = ecs_init();

    char *str = ecs_type_info_to_json(world, ecs_id(ecs_u16_t));
    test_assert(str != NULL);
    test_str(str, "[\"int\"]");
    ecs_os_free(str);

    ecs_fini(world);
}

void SerializeTypeInfoToJson_u32() {
    ecs_world_t *world = ecs_init();

    char *str = ecs_type_info_to_json(world, ecs_id(ecs_u32_t));
    test_assert(str != NULL);
    test_str(str, "[\"int\"]");
    ecs_os_free(str);

    ecs_fini(world);
}

void SerializeTypeInfoToJson_u64() {
    ecs_world_t *world = ecs_init();

    char *str = ecs_type_info_to_json(world, ecs_id(ecs_u64_t));
    test_assert(str != NULL);
    test_str(str, "[\"int\"]");
    ecs_os_free(str);

    ecs_fini(world);
}

void SerializeTypeInfoToJson_uptr() {
    ecs_world_t *world = ecs_init();

    char *str = ecs_type_info_to_json(world, ecs_id(ecs_uptr_t));
    test_assert(str != NULL);
    test_str(str, "[\"int\"]");
    ecs_os_free(str);

    ecs_fini(world);
}

void SerializeTypeInfoToJson_float() {
    ecs_world_t *world = ecs_init();

    char *str = ecs_type_info_to_json(world, ecs_id(ecs_f32_t));
    test_assert(str != NULL);
    test_str(str, "[\"float\"]");
    ecs_os_free(str);

    ecs_fini(world);
}

void SerializeTypeInfoToJson_double() {
    ecs_world_t *world = ecs_init();

    char *str = ecs_type_info_to_json(world, ecs_id(ecs_f64_t));
    test_assert(str != NULL);
    test_str(str, "[\"float\"]");
    ecs_os_free(str);

    ecs_fini(world);
}

void SerializeTypeInfoToJson_string() {
    ecs_world_t *world = ecs_init();

    char *str = ecs_type_info_to_json(world, ecs_id(ecs_string_t));
    test_assert(str != NULL);
    test_str(str, "[\"text\"]");
    ecs_os_free(str);

    ecs_fini(world);
}

void SerializeTypeInfoToJson_entity() {
    ecs_world_t *world = ecs_init();

    char *str = ecs_type_info_to_json(world, ecs_id(ecs_entity_t));
    test_assert(str != NULL);
    test_str(str, "[\"entity\"]");
    ecs_os_free(str);

    ecs_fini(world);
}

void SerializeTypeInfoToJson_enum() {
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

void SerializeTypeInfoToJson_bitmask() {
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

void SerializeTypeInfoToJson_struct() {
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

void SerializeTypeInfoToJson_nested_struct() {
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

void SerializeTypeInfoToJson_array_type() {
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

void SerializeTypeInfoToJson_vector_type() {
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

void SerializeTypeInfoToJson_struct_array_i32_2() {
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

void SerializeTypeInfoToJson_struct_array_struct_2() {
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

void SerializeTypeInfoToJson_struct_array_type() {
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

void SerializeTypeInfoToJson_struct_vector_type() {
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

void SerializeTypeInfoToJson_custom_primitive_type() {
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

void SerializeTypeInfoToJson_custom_array_type() {
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

void SerializeTypeInfoToJson_custom_vector_type() {
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

void SerializeTypeInfoToJson_custom_struct_type() {
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

void SerializeTypeInfoToJson_struct_w_error_range() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t s = ecs_struct(world, {
        .members = {
            { "value", ecs_id(ecs_i32_t), .error = {-1, 1} }
        }
    });

    char *str = ecs_type_info_to_json(world, s);
    test_assert(str != NULL);
    test_str(str, "{\"value\":[\"int\", {\"error\":[-1, 1]}]}");
    ecs_os_free(str);

    ecs_fini(world);
}

void SerializeTypeInfoToJson_struct_w_warning_range() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t s = ecs_struct(world, {
        .members = {
            { "value", ecs_id(ecs_i32_t), .warning = {-1, 1} }
        }
    });

    char *str = ecs_type_info_to_json(world, s);
    test_assert(str != NULL);
    test_str(str, "{\"value\":[\"int\", {\"warning\":[-1, 1]}]}");
    ecs_os_free(str);

    ecs_fini(world);
}

void SerializeTypeInfoToJson_struct_w_error_and_warning_range() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t s = ecs_struct(world, {
        .members = {
            { "value", ecs_id(ecs_i32_t), .warning = {-1, 1}, .error = {-2, 2} }
        }
    });

    char *str = ecs_type_info_to_json(world, s);
    test_assert(str != NULL);
    test_str(str, "{\"value\":[\"int\", {\"error\":[-2, 2],"
        " \"warning\":[-1, 1]}]}");
    ecs_os_free(str);

    ecs_fini(world);
}
