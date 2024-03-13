#include <meta.h>
#include <stdio.h>
#include <limits.h>
#include <math.h>

void SerializeEntityToJson_serialize_empty(void) {
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

void SerializeEntityToJson_serialize_w_name(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t e = ecs_new_entity(world, "Foo");

    char *json = ecs_entity_to_json(world, e, NULL);
    test_assert(json != NULL);
    test_str(json, "{\"path\":\"Foo\", \"ids\":[]}");
    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeEntityToJson_serialize_w_name_1_tag(void) {
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

void SerializeEntityToJson_serialize_w_name_2_tags(void) {
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

void SerializeEntityToJson_serialize_w_name_1_pair(void) {
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

void SerializeEntityToJson_serialize_w_base(void) {
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

void SerializeEntityToJson_serialize_w_base_override(void) {
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

void SerializeEntityToJson_serialize_w_2_base(void) {
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

void SerializeEntityToJson_serialize_w_nested_base(void) {
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

void SerializeEntityToJson_serialize_w_1_component(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "Position"}),
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

void SerializeEntityToJson_serialize_w_2_components(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "Position"}),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t ecs_id(Mass) = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "Mass"}),
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

void SerializeEntityToJson_serialize_w_primitive_component(void) {
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

void SerializeEntityToJson_serialize_w_enum_component(void) {
    typedef enum {
        Red, Green, Blue
    } Color;

    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Color) = ecs_enum_init(world, &(ecs_enum_desc_t){
        .entity = ecs_entity(world, {.name = "Color"}),
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

void SerializeEntityToJson_serialize_w_struct_and_enum_component(void) {
    typedef enum {
        Red, Green, Blue
    } Color;

    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "Position"}),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    test_assert(ecs_id(Position) != 0);

    ecs_entity_t ecs_id(Color) = ecs_enum_init(world, &(ecs_enum_desc_t){
        .entity = ecs_entity(world, {.name = "Color"}),
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

void SerializeEntityToJson_serialize_w_invalid_enum_component(void) {
    typedef enum {
        Red, Green, Blue
    } Color;

    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Color) = ecs_enum_init(world, &(ecs_enum_desc_t){
        .entity = ecs_entity(world, {.name = "Color"}),
        .constants = {
            {"Red"}, {"Blue"}, {"Green"}
        }
    });

    test_assert(ecs_id(Color) != 0);

    ecs_entity_t e = ecs_new_entity(world, "Foo");
    ecs_set(world, e, Color, {100});

    ecs_entity_to_json_desc_t desc = ECS_ENTITY_TO_JSON_INIT;
    desc.serialize_values = true;

    ecs_log_set_level(-4);
    char *json = ecs_entity_to_json(world, e, &desc);
    test_assert(json == NULL);

    ecs_fini(world);
}

void SerializeEntityToJson_serialize_w_type_info(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "Position"}),
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

void SerializeEntityToJson_serialize_w_type_info_unit(void) {
    typedef struct {
        ecs_i32_t value;    
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t u = ecs_unit_init(world, &(ecs_unit_desc_t){
        .entity = ecs_entity(world, {.name = "celsius"}),
        .symbol = "°"
    });
    test_assert(u != 0);

    ecs_entity_t ecs_id(T) = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "T"}),
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

void SerializeEntityToJson_serialize_w_type_info_unit_quantity(void) {
    typedef struct {
        ecs_i32_t value;    
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t q = ecs_quantity_init(world, &(ecs_entity_desc_t){
        .name = "temperature"
    });
    test_assert(q != 0);

    ecs_entity_t u = ecs_unit_init(world, &(ecs_unit_desc_t){
        .entity = ecs_entity(world, {.name = "celsius"}),
        .symbol = "°",
        .quantity = q
    });
    test_assert(u != 0);

    ecs_entity_t ecs_id(T) = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "T"}),
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

void SerializeEntityToJson_serialize_w_type_info_unit_over(void) {
    typedef struct {
        ecs_i32_t value;    
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t u_1 = ecs_unit_init(world, &(ecs_unit_desc_t){
        .entity = ecs_entity(world, {.name = "seconds"}),
        .symbol = "s"
    });
    test_assert(u_1 != 0);

    ecs_entity_t u_2 = ecs_unit_init(world, &(ecs_unit_desc_t){
        .entity = ecs_entity(world, {.name = "meters"}),
        .symbol = "m"
    });
    test_assert(u_2 != 0);

    ecs_entity_t u_3 = ecs_unit_init(world, &(ecs_unit_desc_t){
        .entity = ecs_entity(world, {.name = "meters_per_second"}),
        .symbol = "m/s",
        .base = u_2,
        .over = u_1
    });
    test_assert(u_3 != 0);

    ecs_entity_t ecs_id(T) = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "T"}),
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

void SerializeEntityToJson_serialize_wo_private(void) {
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

void SerializeEntityToJson_serialize_w_private(void) {
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
        "\"ids\":[[\"Tag\"], [\"flecs.core.Identifier\", \"flecs.core.Name\"]]}");

    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeEntityToJson_serialize_w_label(void) {
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

void SerializeEntityToJson_serialize_w_label_no_name(void) {
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

void SerializeEntityToJson_serialize_w_id_labels(void) {
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

void SerializeEntityToJson_serialize_w_brief(void) {
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

void SerializeEntityToJson_serialize_w_brief_no_brief(void) {
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

void SerializeEntityToJson_serialize_w_link(void) {
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

void SerializeEntityToJson_serialize_w_link_no_link(void) {
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

void SerializeEntityToJson_serialize_color(void) {
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

void SerializeEntityToJson_serialize_union_relationship(void) {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Movement, Union);
    ECS_TAG(world, Running);
    ECS_TAG(world, Walking);

    ecs_entity_t e = ecs_new_entity(world, "Foo");
    ecs_add_pair(world, e, Movement, Running);

    ecs_entity_to_json_desc_t desc = ECS_ENTITY_TO_JSON_INIT;
    char *json = ecs_entity_to_json(world, e, &desc);
    test_assert(json != NULL);

    test_str(json, "{"
        "\"path\":\"Foo\", "
        "\"ids\":["
            "[\"Movement\", \"Running\"]"
        "]}");

    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeEntityToJson_serialize_union_relationship_invalid_entity(void) {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Movement, Union);
    ecs_entity_t Running = ecs_new_entity(world, "Running");

    ecs_entity_t e = ecs_new_entity(world, "Foo");
    ecs_add_pair(world, e, Movement, Running);
    ecs_delete(world, Running);

    ecs_entity_to_json_desc_t desc = ECS_ENTITY_TO_JSON_INIT;
    char *json = ecs_entity_to_json(world, e, &desc);
    test_assert(json != NULL);

    test_str(json, "{"
        "\"path\":\"Foo\", "
        "\"ids\":[]}");

    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeEntityToJson_serialize_union_relationship_invalid_entity_w_labels(void) {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Movement, Union);
    ecs_entity_t Running = ecs_new_entity(world, "Running");

    ecs_entity_t e = ecs_new_entity(world, "Foo");
    ecs_add_pair(world, e, Movement, Running);
    ecs_delete(world, Running);

    ecs_entity_to_json_desc_t desc = ECS_ENTITY_TO_JSON_INIT;
    desc.serialize_id_labels = true;
    char *json = ecs_entity_to_json(world, e, &desc);
    test_assert(json != NULL);

    test_str(json, "{"
        "\"path\":\"Foo\", "
        "\"ids\":[], "
        "\"id_labels\":[]}");

    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeEntityToJson_serialize_w_union_property(void) {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Movement, Union);

    ecs_entity_to_json_desc_t desc = ECS_ENTITY_TO_JSON_INIT;
    char *json = ecs_entity_to_json(world, Movement, &desc);
    test_assert(json != NULL);

    test_str(json, "{"
        "\"path\":\"Movement\", "
        "\"ids\":["
            "[\"flecs.core.Union\"]"
        "]}");

    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeEntityToJson_serialize_union_relationship_w_labels(void) {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Movement, Union);
    ECS_TAG(world, Running);
    ECS_TAG(world, Walking);

    ecs_entity_t e = ecs_new_entity(world, "Foo");
    ecs_add_pair(world, e, Movement, Running);

    ecs_entity_to_json_desc_t desc = ECS_ENTITY_TO_JSON_INIT;
    desc.serialize_id_labels = true;
    char *json = ecs_entity_to_json(world, e, &desc);
    test_assert(json != NULL);

    test_str(json, "{"
        "\"path\":\"Foo\", "
        "\"ids\":["
            "[\"Movement\", \"Running\"]"
        "], "
        "\"id_labels\":["
            "[\"Movement\", \"Running\"]"
        "]}");

    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeEntityToJson_serialize_w_doc_w_quotes(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);

    ecs_entity_t p = ecs_new_entity(world, "Parent");
    ecs_entity_t e = ecs_new_entity(world, "Child");
    ecs_add(world, e, Tag);
    ecs_add_pair(world, e, EcsChildOf, p);
    ecs_doc_set_name(world, e, "Doc \"name\"");
    ecs_doc_set_brief(world, e, "Doc \"brief\"");
    ecs_doc_set_link(world, e, "Doc \"link\"");
    ecs_doc_set_color(world, e, "Doc \"color\"");

    ecs_entity_to_json_desc_t desc = ECS_ENTITY_TO_JSON_INIT;
    desc.serialize_label = true;
    desc.serialize_brief = true;
    desc.serialize_color = true;
    desc.serialize_link = true;
    char *json = ecs_entity_to_json(world, e, &desc);
    test_assert(json != NULL);

    test_str(json, "{"
        "\"path\":\"Parent.Child\", "
        "\"label\":\"Doc \\\"name\\\"\", " 
        "\"brief\":\"Doc \\\"brief\\\"\", "
        "\"link\":\"Doc \\\"link\\\"\", "
        "\"color\":\"Doc \\\"color\\\"\", "
        "\"ids\":[[\"Tag\"]]"
    "}");

    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeEntityToJson_serialize_from_core(void) {
    ecs_world_t *world = ecs_init();

    char *json = ecs_entity_to_json(world, EcsWorld, NULL);
    test_assert(json != NULL);
    test_str(json, "{\"path\":\"flecs.core.World\", \"ids\":[]}");
    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeEntityToJson_serialize_w_1_alert(void) {
    ecs_world_t *world = ecs_init();

    ECS_IMPORT(world, FlecsAlerts);

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t alert = ecs_alert(world, {
        .entity = ecs_new_entity(world, "position_without_velocity"),
        .filter.expr = "Position, !Velocity",
        .message = "$this has Position but not Velocity"
    });

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_set(world, e1, Position, {10, 20});

    ecs_progress(world, 1.0); /* Evaluate alert logic */

    /* Give alert instance name so we don't have to test unstable entity ids */
    ecs_entity_t ai = ecs_get_alert(world, e1, alert);
    test_assert(ai != 0);
    ecs_set_name(world, ai, "e1_alert");

    ecs_entity_to_json_desc_t desc = ECS_ENTITY_TO_JSON_INIT;
    desc.serialize_alerts = true;
    char *json = ecs_entity_to_json(world, e1, &desc);
    test_assert(json != NULL);

    test_str(json, "{"
        "\"path\":\"e1\", "
        "\"ids\":[[\"Position\"]], "
        "\"alerts\":[{"
            "\"alert\":\"position_without_velocity.e1_alert\", "
            "\"message\":\"e1 has Position but not Velocity\", "
            "\"severity\":\"Error\""
        "}]"
    "}");
    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeEntityToJson_serialize_w_2_alerts(void) {
    ecs_world_t *world = ecs_init();

    ECS_IMPORT(world, FlecsAlerts);

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);

    ecs_entity_t alert_1 = ecs_alert(world, {
        .entity = ecs_new_entity(world, "position_without_velocity"),
        .filter.expr = "Position, !Velocity",
        .message = "$this has Position but not Velocity"
    });

    ecs_entity_t alert_2 = ecs_alert(world, {
        .entity = ecs_new_entity(world, "position_without_mass"),
        .filter.expr = "Position, !Mass",
        .message = "$this has Position but not Mass"
    });

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_set(world, e1, Position, {10, 20});

    ecs_progress(world, 1.0); /* Evaluate alert logic */

    /* Give alert instance name so we don't have to test unstable entity ids */
    ecs_entity_t ai = ecs_get_alert(world, e1, alert_1);
    test_assert(ai != 0);
    ecs_set_name(world, ai, "e1_alert_1");

    ai = ecs_get_alert(world, e1, alert_2);
    test_assert(ai != 0);
    ecs_set_name(world, ai, "e1_alert_2");

    ecs_entity_to_json_desc_t desc = ECS_ENTITY_TO_JSON_INIT;
    desc.serialize_alerts = true;
    char *json = ecs_entity_to_json(world, e1, &desc);
    test_assert(json != NULL);

    test_str(json, "{"
        "\"path\":\"e1\", "
        "\"ids\":[[\"Position\"]], "
        "\"alerts\":[{"
            "\"alert\":\"position_without_mass.e1_alert_2\", "
            "\"message\":\"e1 has Position but not Mass\", "
            "\"severity\":\"Error\""
        "}, {"
            "\"alert\":\"position_without_velocity.e1_alert_1\", "
            "\"message\":\"e1 has Position but not Velocity\", "
            "\"severity\":\"Error\""
        "}]"
    "}");
    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeEntityToJson_serialize_w_child_alerts(void) {
    ecs_world_t *world = ecs_init();

    ECS_IMPORT(world, FlecsAlerts);

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t alert = ecs_alert(world, {
        .entity = ecs_new_entity(world, "position_without_velocity"),
        .filter.expr = "Position, !Velocity",
        .message = "$this has Position but not Velocity"
    });

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_set(world, e1, Position, {10, 20});

    ecs_entity_t child_1 = ecs_new_entity(world, "child1");
    ecs_set(world, child_1, Position, {10, 20});
    ecs_add_pair(world, child_1, EcsChildOf, e1);

    ecs_entity_t child_2 = ecs_new_entity(world, "child2");
    ecs_set(world, child_2, Position, {10, 20});
    ecs_add_pair(world, child_2, EcsChildOf, e1);

    ecs_entity_t grandchild_1 = ecs_new_entity(world, "grandchild1");
    ecs_set(world, grandchild_1, Position, {10, 20});
    ecs_add_pair(world, grandchild_1, EcsChildOf, child_1);

    ecs_progress(world, 1.0); /* Evaluate alert logic */

    /* Give alert instances name so we don't have to test unstable entity ids */
    ecs_entity_t e1_ai = ecs_get_alert(world, e1, alert);
    test_assert(e1_ai != 0);
    ecs_set_name(world, e1_ai, "e1_alert");

    ecs_entity_t child1_ai = ecs_get_alert(world, child_1, alert);
    test_assert(child1_ai != 0);
    ecs_set_name(world, child1_ai, "child1_alert");

    ecs_entity_t child2_ai = ecs_get_alert(world, child_2, alert);
    test_assert(child2_ai != 0);
    ecs_set_name(world, child2_ai, "child2_alert");

    ecs_entity_t grandchild1_ai = ecs_get_alert(world, grandchild_1, alert);
    test_assert(grandchild1_ai != 0);
    ecs_set_name(world, grandchild1_ai, "grandchild1_alert");

    ecs_entity_to_json_desc_t desc = ECS_ENTITY_TO_JSON_INIT;
    desc.serialize_alerts = true;
    char *json = ecs_entity_to_json(world, e1, &desc);
    test_assert(json != NULL);

    test_str(json, "{"
        "\"path\":\"e1\", "
        "\"ids\":[[\"Position\"]], "
        "\"alerts\":[{"
            "\"alert\":\"position_without_velocity.e1_alert\", "
            "\"message\":\"e1 has Position but not Velocity\", "
            "\"severity\":\"Error\""
        "}, {"
            "\"alert\":\"position_without_velocity.child1_alert\", "
            "\"message\":\"e1.child1 has Position but not Velocity\", "
            "\"severity\":\"Error\", "
            "\"path\":\"e1.child1\""
        "}, {"
            "\"alert\":\"position_without_velocity.grandchild1_alert\", "
            "\"message\":\"e1.child1.grandchild1 has Position but not Velocity\", "
            "\"severity\":\"Error\", "
            "\"path\":\"e1.child1.grandchild1\""
        "}, {"
            "\"alert\":\"position_without_velocity.child2_alert\", "
            "\"message\":\"e1.child2 has Position but not Velocity\", "
            "\"severity\":\"Error\", "
            "\"path\":\"e1.child2\""
        "}]"
    "}");
    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeEntityToJson_serialize_w_severity_filter_alert(void) {
    ecs_world_t *world = ecs_init();

    ECS_IMPORT(world, FlecsAlerts);

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TAG(world, Tag);

    ecs_entity_t alert = ecs_alert(world, {
        .entity = ecs_new_entity(world, "position_without_velocity"),
        .filter.expr = "Position, !Velocity",
        .message = "$this has Position but not Velocity",
        .severity_filters[0] = {
            .severity = EcsAlertWarning,
            .with = Tag
        }
    });

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_set(world, e1, Position, {10, 20});
    ecs_add(world, e1, Tag);

    ecs_progress(world, 1.0); /* Evaluate alert logic */

    /* Give alert instance name so we don't have to test unstable entity ids */
    ecs_entity_t ai = ecs_get_alert(world, e1, alert);
    test_assert(ai != 0);
    ecs_set_name(world, ai, "e1_alert");

    ecs_entity_to_json_desc_t desc = ECS_ENTITY_TO_JSON_INIT;
    desc.serialize_alerts = true;
    char *json = ecs_entity_to_json(world, e1, &desc);
    test_assert(json != NULL);

    test_str(json, "{"
        "\"path\":\"e1\", "
        "\"ids\":[[\"Position\"], [\"Tag\"]], "
        "\"alerts\":[{"
            "\"alert\":\"position_without_velocity.e1_alert\", "
            "\"message\":\"e1 has Position but not Velocity\", "
            "\"severity\":\"Warning\""
        "}]"
    "}");
    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeEntityToJson_serialize_w_alerts_not_imported(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_set(world, e1, Position, {10, 20});
    ecs_new_w_pair(world, EcsChildOf, e1);

    ecs_entity_to_json_desc_t desc = ECS_ENTITY_TO_JSON_INIT;
    desc.serialize_alerts = true;
    char *json = ecs_entity_to_json(world, e1, &desc);
    test_assert(json != NULL);

    test_str(json, "{"
        "\"path\":\"e1\", "
        "\"ids\":[[\"Position\"]]"
    "}");
    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeEntityToJson_serialize_w_alerts_no_message(void) {
    ecs_world_t *world = ecs_init();

    ECS_IMPORT(world, FlecsAlerts);

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TAG(world, Tag);

    ecs_entity_t alert = ecs_alert(world, {
        .entity = ecs_new_entity(world, "position_without_velocity"),
        .filter.expr = "Position, !Velocity"
    });

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_set(world, e1, Position, {10, 20});
    ecs_add(world, e1, Tag);

    ecs_progress(world, 1.0); /* Evaluate alert logic */

    /* Give alert instance name so we don't have to test unstable entity ids */
    ecs_entity_t ai = ecs_get_alert(world, e1, alert);
    test_assert(ai != 0);
    ecs_set_name(world, ai, "e1_alert");

    ecs_entity_to_json_desc_t desc = ECS_ENTITY_TO_JSON_INIT;
    desc.serialize_alerts = true;
    char *json = ecs_entity_to_json(world, e1, &desc);
    test_assert(json != NULL);

    test_str(json, "{"
        "\"path\":\"e1\", "
        "\"ids\":[[\"Position\"], [\"Tag\"]], "
        "\"alerts\":[{"
            "\"alert\":\"position_without_velocity.e1_alert\", "
            "\"severity\":\"Error\""
        "}]"
    "}");
    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeEntityToJson_serialize_refs_childof(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Rel);

    ecs_entity_t e1 = ecs_new_entity(world, "e1");

    ecs_entity_t child_1 = ecs_new_entity(world, "child1");
    ecs_add_pair(world, child_1, EcsChildOf, e1);

    ecs_entity_t child_2 = ecs_new_entity(world, "child2");
    ecs_add_pair(world, child_2, EcsChildOf, e1);

    ecs_entity_t child_3 = ecs_new_entity(world, "child3");
    ecs_add_pair(world, child_3, Rel, e1);

    ecs_entity_to_json_desc_t desc = ECS_ENTITY_TO_JSON_INIT;
    desc.serialize_refs = EcsChildOf;
    char *json = ecs_entity_to_json(world, e1, &desc);
    test_assert(json != NULL);

    test_str(json, "{"
        "\"path\":\"e1\", "
        "\"ids\":[], "
        "\"refs\":{"
            "\"ChildOf\":["
                "\"e1.child1\", "
                "\"e1.child2\""
            "]"
        "}"
    "}");
    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeEntityToJson_serialize_refs_custom(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Rel);

    ecs_entity_t e1 = ecs_new_entity(world, "e1");

    ecs_entity_t child_1 = ecs_new_entity(world, "child1");
    ecs_add_pair(world, child_1, Rel, e1);

    ecs_entity_t child_2 = ecs_new_entity(world, "child2");
    ecs_add_pair(world, child_2, Rel, e1);

    ecs_entity_t child_3 = ecs_new_entity(world, "child3");
    ecs_add_pair(world, child_3, EcsChildOf, e1);

    ecs_entity_to_json_desc_t desc = ECS_ENTITY_TO_JSON_INIT;
    desc.serialize_refs = Rel;
    char *json = ecs_entity_to_json(world, e1, &desc);
    test_assert(json != NULL);

    test_str(json, "{"
        "\"path\":\"e1\", "
        "\"ids\":[], "
        "\"refs\":{"
            "\"Rel\":["
                "\"child1\", "
                "\"child2\""
            "]"
        "}"
    "}");
    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeEntityToJson_serialize_refs_wildcard(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Rel);

    ecs_entity_t e1 = ecs_new_entity(world, "e1");

    ecs_entity_t child_1 = ecs_new_entity(world, "child1");
    ecs_add_pair(world, child_1, Rel, e1);

    ecs_entity_t child_2 = ecs_new_entity(world, "child2");
    ecs_add_pair(world, child_2, Rel, e1);

    ecs_entity_t child_3 = ecs_new_entity(world, "child3");
    ecs_add_pair(world, child_3, EcsChildOf, e1);

    ecs_entity_to_json_desc_t desc = ECS_ENTITY_TO_JSON_INIT;
    desc.serialize_refs = EcsWildcard;
    char *json = ecs_entity_to_json(world, e1, &desc);
    test_assert(json != NULL);

    test_str(json, "{"
        "\"path\":\"e1\", "
        "\"ids\":[], "
        "\"refs\":{"
            "\"ChildOf\":["
                "\"e1.child3\""
            "], "
            "\"Rel\":["
                "\"child1\", "
                "\"child2\""
            "]"
        "}"
    "}");
    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeEntityToJson_serialize_no_ids(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);

    ecs_entity_t e = ecs_new_entity(world, "Foo");
    ecs_add(world, e, Tag);

    ecs_entity_to_json_desc_t desc = ECS_ENTITY_TO_JSON_INIT;
    desc.serialize_ids = false;
    char *json = ecs_entity_to_json(world, e, &desc);
    test_assert(json != NULL);
    test_str(json, "{\"path\":\"Foo\"}");
    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeEntityToJson_serialize_matches_filter(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, TagC);

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_add(world, e1, TagA);
    ecs_entity_t e2 = ecs_new_entity(world, "e2");
    ecs_add(world, e2, TagB);

    ecs_filter_t *f_a = ecs_filter(world, {
        .entity = ecs_entity(world, { .name = "f_a"} ),
        .terms = {{ TagA }}
    });
    test_assert(f_a != NULL);

    ecs_filter_t *f_b = ecs_filter(world, {
        .entity = ecs_entity(world, { .name = "f_b"} ),
        .terms = {{ TagB }}
    });
    test_assert(f_b != NULL);

    ecs_filter_t *f_c = ecs_filter(world, {
        .entity = ecs_entity(world, { .name = "f_c"} ),
        .terms = {{ TagC }}
    });
    test_assert(f_c != NULL);

    ecs_entity_to_json_desc_t desc = ECS_ENTITY_TO_JSON_INIT;
    desc.serialize_matches = true;
    char *json = ecs_entity_to_json(world, e1, &desc);
    test_assert(json != NULL);
    test_str(json, "{"
            "\"path\":\"e1\", "
            "\"ids\":[[\"TagA\"]], "
            "\"matches\":["
                "\"f_a\""
            "]"
        "}");
    ecs_os_free(json);

    ecs_filter_fini(f_a);
    ecs_filter_fini(f_b);
    ecs_filter_fini(f_c);

    ecs_fini(world);
}

void SerializeEntityToJson_serialize_matches_query(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, TagC);

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_add(world, e1, TagA);
    ecs_entity_t e2 = ecs_new_entity(world, "e2");
    ecs_add(world, e2, TagB);

    ecs_query_t *f_a = ecs_query(world, {
        .filter.entity = ecs_entity(world, { .name = "f_a"} ),
        .filter.terms = {{ TagA }}
    });
    test_assert(f_a != NULL);

    ecs_query_t *f_b = ecs_query(world, {
        .filter.entity = ecs_entity(world, { .name = "f_b"} ),
        .filter.terms = {{ TagB }}
    });
    test_assert(f_b != NULL);

    ecs_query_t *f_c = ecs_query(world, {
        .filter.entity = ecs_entity(world, { .name = "f_c"} ),
        .filter.terms = {{ TagC }}
    });
    test_assert(f_c != NULL);

    ecs_entity_to_json_desc_t desc = ECS_ENTITY_TO_JSON_INIT;
    desc.serialize_matches = true;
    char *json = ecs_entity_to_json(world, e1, &desc);
    test_assert(json != NULL);
    test_str(json, "{"
            "\"path\":\"e1\", "
            "\"ids\":[[\"TagA\"]], "
            "\"matches\":["
                "\"f_a\""
            "]"
        "}");
    ecs_os_free(json);

    ecs_query_fini(f_a);
    ecs_query_fini(f_b);
    ecs_query_fini(f_c);

    ecs_fini(world);
}

void SerializeEntityToJson_serialize_matches_rule(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, TagC);

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_add(world, e1, TagA);
    ecs_entity_t e2 = ecs_new_entity(world, "e2");
    ecs_add(world, e2, TagB);

    ecs_rule_t *f_a = ecs_rule(world, {
        .entity = ecs_entity(world, { .name = "f_a"} ),
        .terms = {{ TagA }}
    });
    test_assert(f_a != NULL);

    ecs_rule_t *f_b = ecs_rule(world, {
        .entity = ecs_entity(world, { .name = "f_b"} ),
        .terms = {{ TagB }}
    });
    test_assert(f_b != NULL);

    ecs_rule_t *f_c = ecs_rule(world, {
        .entity = ecs_entity(world, { .name = "f_c"} ),
        .terms = {{ TagC }}
    });
    test_assert(f_c != NULL);

    ecs_entity_to_json_desc_t desc = ECS_ENTITY_TO_JSON_INIT;
    desc.serialize_matches = true;
    char *json = ecs_entity_to_json(world, e1, &desc);
    test_assert(json != NULL);
    test_str(json, "{"
            "\"path\":\"e1\", "
            "\"ids\":[[\"TagA\"]], "
            "\"matches\":["
                "\"f_a\""
            "]"
        "}");
    ecs_os_free(json);

    ecs_rule_fini(f_a);
    ecs_rule_fini(f_b);
    ecs_rule_fini(f_c);

    ecs_fini(world);
}

void SerializeEntityToJson_serialize_no_matches(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, TagC);

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_add(world, e1, TagA);
    ecs_entity_t e2 = ecs_new_entity(world, "e2");
    ecs_add(world, e2, TagB);

    ecs_filter_t *f_b = ecs_filter(world, {
        .entity = ecs_entity(world, { .name = "f_b"} ),
        .terms = {{ TagB }}
    });
    test_assert(f_b != NULL);

    ecs_filter_t *f_c = ecs_filter(world, {
        .entity = ecs_entity(world, { .name = "f_c"} ),
        .terms = {{ TagC }}
    });
    test_assert(f_c != NULL);

    ecs_entity_to_json_desc_t desc = ECS_ENTITY_TO_JSON_INIT;
    desc.serialize_matches = true;
    char *json = ecs_entity_to_json(world, e1, &desc);
    test_assert(json != NULL);
    test_str(json, "{"
            "\"path\":\"e1\", "
            "\"ids\":[[\"TagA\"]], "
            "\"matches\":["
            "]"
        "}");
    ecs_os_free(json);

    ecs_filter_fini(f_b);
    ecs_filter_fini(f_c);

    ecs_fini(world);
}
