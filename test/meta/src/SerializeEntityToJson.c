#include <meta.h>
#include <stdio.h>
#include <limits.h>
#include <math.h>

void SerializeEntityToJson_serialize_empty(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t e = ecs_new(world);

    char *str = flecs_asprintf("{\"name\":\"#%u\"}", (uint32_t)e);
    char *json = ecs_entity_to_json(world, e, NULL);
    test_assert(json != NULL);
    test_json(json, str);
    ecs_os_free(json);
    ecs_os_free(str);

    ecs_fini(world);
}

void SerializeEntityToJson_serialize_w_name(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t e = ecs_entity(world, { .name = "Foo" });

    char *json = ecs_entity_to_json(world, e, NULL);
    test_assert(json != NULL);
    test_json(json, "{\"name\":\"Foo\"}");
    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeEntityToJson_serialize_w_name_1_tag(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);

    ecs_entity_t e = ecs_entity(world, { .name = "Foo" });
    ecs_add(world, e, Tag);

    char *json = ecs_entity_to_json(world, e, NULL);
    test_assert(json != NULL);
    test_json(json, "{\"name\":\"Foo\", \"tags\":[\"Tag\"]}");
    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeEntityToJson_serialize_w_name_2_tags(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_entity_t e = ecs_entity(world, { .name = "Foo" });
    ecs_add(world, e, TagA);
    ecs_add(world, e, TagB);

    char *json = ecs_entity_to_json(world, e, NULL);
    test_assert(json != NULL);
    test_json(json, 
        "{\"name\":\"Foo\", \"tags\":[\"TagA\", \"TagB\"]}");
    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeEntityToJson_serialize_w_name_1_pair(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Rel);
    ECS_TAG(world, Obj);

    ecs_entity_t e = ecs_entity(world, { .name = "Foo" });
    ecs_add_pair(world, e, Rel, Obj);

    char *json = ecs_entity_to_json(world, e, NULL);
    test_assert(json != NULL);
    test_json(json, 
        "{\"name\":\"Foo\", \"pairs\":{\"Rel\":\"Obj\"}}");
    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeEntityToJson_serialize_w_base(void) {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, TagA, (OnInstantiate, Inherit));
    ECS_ENTITY(world, TagB, (OnInstantiate, Inherit));

    ecs_entity_t base = ecs_entity(world, { .name = "Base" });
    ecs_add(world, base, TagA);

    ecs_entity_t e = ecs_entity(world, { .name = "Foo" });
    ecs_add_pair(world, e, EcsIsA, base);
    ecs_add(world, e, TagB);

    ecs_entity_to_json_desc_t desc = {
        .serialize_inherited = true
    };
    char *json = ecs_entity_to_json(world, e, &desc);
    test_assert(json != NULL);
    test_json(json, "{\"name\":\"Foo\", \"tags\":[\"TagB\"],\"pairs\":{\"IsA\":\"Base\"},\"inherited\":{\"Base\":{\"tags\":[\"TagA\"]}}}");

    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeEntityToJson_serialize_w_2_base(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_entity_t base_a = ecs_entity(world, { .name = "BaseA" });
    ecs_entity_t base_b = ecs_entity(world, { .name = "BaseB" });

    ecs_entity_t e = ecs_entity(world, { .name = "Foo" });
    ecs_add_pair(world, e, EcsIsA, base_a);
    ecs_add_pair(world, e, EcsIsA, base_b);

    char *json = ecs_entity_to_json(world, e, NULL);
    test_assert(json != NULL);
    test_json(json, "{\"name\":\"Foo\", \"pairs\":{\"flecs.core.IsA\":[\"BaseA\", \"BaseB\"]}}");

    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeEntityToJson_serialize_component_w_base(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "Position"}),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);

    ecs_entity_t base = ecs_entity(world, { .name = "Base" });
    ecs_set(world, base, Position, {10, 20});

    ecs_entity_t e = ecs_entity(world, { .name = "Foo" });
    ecs_add_pair(world, e, EcsIsA, base);

    ecs_entity_to_json_desc_t desc = {
        .serialize_inherited = true,
        .serialize_values = true
    };
    char *json = ecs_entity_to_json(world, e, &desc);
    test_assert(json != NULL);
    test_json(json, "{\"name\":\"Foo\", \"pairs\":{\"IsA\":\"Base\"},\"inherited\":{\"Base\":{\"components\":{\"Position\":{\"x\":10, \"y\":20}}}}}");

    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeEntityToJson_serialize_component_w_base_no_reflection_data(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);

    ecs_entity_t base = ecs_entity(world, { .name = "Base" });
    ecs_set(world, base, Position, {10, 20});

    ecs_entity_t e = ecs_entity(world, { .name = "Foo" });
    ecs_add_pair(world, e, EcsIsA, base);

    ecs_entity_to_json_desc_t desc = {
        .serialize_inherited = true,
        .serialize_values = true
    };
    char *json = ecs_entity_to_json(world, e, &desc);
    test_assert(json != NULL);
    test_json(json, "{\"name\":\"Foo\", \"pairs\":{\"IsA\":\"Base\"},\"inherited\":{\"Base\":{\"components\":{\"Position\":null}}}}");

    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeEntityToJson_serialize_component_w_base_w_owned(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "Position"}),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t ecs_id(Velocity) = ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "Velocity"}),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);

    ecs_entity_t base = ecs_entity(world, { .name = "Base" });
    ecs_set(world, base, Position, {10, 20});

    ecs_entity_t e = ecs_entity(world, { .name = "Foo" });
    ecs_add_pair(world, e, EcsIsA, base);
    ecs_set(world, e, Velocity, {1, 2});

    ecs_entity_to_json_desc_t desc = {
        .serialize_inherited = true,
        .serialize_values = true
    };
    char *json = ecs_entity_to_json(world, e, &desc);
    test_assert(json != NULL);
    test_json(json, "{\"name\":\"Foo\", \"pairs\":{\"IsA\":\"Base\"},\"inherited\":{\"Base\":{\"components\":{\"Position\":{\"x\":10, \"y\":20}}}}, \"components\":{\"Velocity\":{\"x\":1, \"y\":2}}}");

    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeEntityToJson_serialize_component_w_base_w_owned_no_reflection_data(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t base = ecs_entity(world, { .name = "Base" });
    ecs_set(world, base, Position, {10, 20});

    ecs_entity_t e = ecs_entity(world, { .name = "Foo" });
    ecs_add_pair(world, e, EcsIsA, base);
    ecs_set(world, e, Velocity, {1, 2});

    ecs_entity_to_json_desc_t desc = {
        .serialize_inherited = true,
        .serialize_values = true
    };
    char *json = ecs_entity_to_json(world, e, &desc);
    test_assert(json != NULL);
    test_json(json, "{\"name\":\"Foo\", \"pairs\":{\"IsA\":\"Base\"},\"inherited\":{\"Base\":{\"components\":{\"Position\":null}}}, \"components\":{\"Velocity\":null}}");

    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeEntityToJson_serialize_component_w_base_w_owned_override(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "Position"}),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t ecs_id(Velocity) = ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "Velocity"}),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t base = ecs_entity(world, { .name = "Base" });
    ecs_set(world, base, Position, {10, 20});

    ecs_entity_t e = ecs_entity(world, { .name = "Foo" });
    ecs_add_pair(world, e, EcsIsA, base);
    ecs_set(world, e, Velocity, {1, 2});

    ecs_entity_to_json_desc_t desc = {
        .serialize_inherited = true,
        .serialize_values = true
    };
    char *json = ecs_entity_to_json(world, e, &desc);
    test_assert(json != NULL);
    test_json(json, "{\"name\":\"Foo\", \"pairs\":{\"IsA\":\"Base\"},\"inherited\":{\"Base\":{\"components\":{\"Position\":{\"x\":10, \"y\":20}}}}, \"components\":{\"Position\":{\"x\":10, \"y\":20}, \"Velocity\":{\"x\":1, \"y\":2}}}");

    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeEntityToJson_serialize_component_w_base_w_owned_no_reflection_data_override(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t base = ecs_new(world);
    ecs_set(world, base, Position, {10, 20});

    ecs_entity_t e = ecs_new(world);
    ecs_add_pair(world, e, EcsIsA, base);
    ecs_set(world, e, Velocity, {1, 2});

    ecs_entity_to_json_desc_t desc = {
        .serialize_inherited = true,
        .serialize_values = true
    };
    char *json = ecs_entity_to_json(world, e, &desc);
    test_assert(json != NULL);
    test_json(json, "{\"name\":\"#514\", \"pairs\":{\"IsA\":\"#513\"},\"inherited\":{\"#513\":{\"components\":{\"Position\":null}}}, \"components\":{\"Position\":null, \"Velocity\":null}}");

    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeEntityToJson_serialize_w_nested_base(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_entity_t base_of_base = ecs_entity(world, { .name = "BaseOfBase" });
    ecs_entity_t base = ecs_entity(world, { .name = "Base" });
    ecs_add_pair(world, base, EcsIsA, base_of_base);

    ecs_entity_t e = ecs_entity(world, { .name = "Foo" });
    ecs_add_pair(world, e, EcsIsA, base);

    char *json = ecs_entity_to_json(world, e, NULL);
    test_assert(json != NULL);
    test_json(json, "{\"name\":\"Foo\", \"pairs\":{\"flecs.core.IsA\":\"Base\"}}");

    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeEntityToJson_serialize_w_1_component(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "Position"}),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t e = ecs_entity(world, { .name = "Foo" });
    ecs_set(world, e, Position, {10, 20});

    ecs_entity_to_json_desc_t desc = ECS_ENTITY_TO_JSON_INIT;
    desc.serialize_values = true;

    char *json = ecs_entity_to_json(world, e, &desc);
    test_assert(json != NULL);
    test_json(json, "{\"name\":\"Foo\", \"components\":{\"Position\":{\"x\":10, \"y\":20}}}");

    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeEntityToJson_serialize_w_2_components(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "Position"}),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t ecs_id(Mass) = ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "Mass"}),
        .members = {
            {"value", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t e = ecs_entity(world, { .name = "Foo" });
    ecs_set(world, e, Position, {10, 20});
    ecs_set(world, e, Mass, {1234});

    ecs_entity_to_json_desc_t desc = ECS_ENTITY_TO_JSON_INIT;
    desc.serialize_values = true;

    char *json = ecs_entity_to_json(world, e, &desc);
    test_assert(json != NULL);
    test_json(json, "{\"name\":\"Foo\", \"components\":{\"Position\":{\"x\":10, \"y\":20}, \"Mass\":{\"value\":1234}}}");

    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeEntityToJson_serialize_w_primitive_component(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t e = ecs_entity(world, { .name = "Foo" });
    ecs_set(world, e, ecs_i32_t, {10});

    ecs_entity_to_json_desc_t desc = ECS_ENTITY_TO_JSON_INIT;
    desc.serialize_values = true;

    char *json = ecs_entity_to_json(world, e, &desc);
    test_assert(json != NULL);
    test_json(json, "{\"name\":\"Foo\", \"components\":{\"flecs.meta.i32\":10}}");

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

    ecs_entity_t e = ecs_entity(world, { .name = "Foo" });
    ecs_set(world, e, Color, {1});

    ecs_entity_to_json_desc_t desc = ECS_ENTITY_TO_JSON_INIT;
    desc.serialize_values = true;

    char *json = ecs_entity_to_json(world, e, &desc);
    test_assert(json != NULL);
    test_json(json, "{\"name\":\"Foo\", \"components\":{\"Color\":\"Blue\"}}");

    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeEntityToJson_serialize_w_struct_and_enum_component(void) {
    typedef enum {
        Red, Green, Blue
    } Color;

    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = ecs_struct(world, {
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

    ecs_entity_t e = ecs_entity(world, { .name = "Foo" });
    ecs_set(world, e, Color, {1});
    ecs_set(world, e, Position, {10, 20});

    ecs_entity_to_json_desc_t desc = ECS_ENTITY_TO_JSON_INIT;
    desc.serialize_values = true;

    char *json = ecs_entity_to_json(world, e, &desc);
    test_assert(json != NULL);
    test_json(json, "{\"name\":\"Foo\", \"components\":{\"Position\":{\"x\":10, \"y\":20}, \"Color\":\"Blue\"}}");

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

    ecs_entity_t e = ecs_entity(world, { .name = "Foo" });
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

    ecs_entity_t ecs_id(Position) = ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "Position"}),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t e = ecs_entity(world, { .name = "Foo" });
    ecs_set(world, e, Position, {10, 20});

    ecs_entity_to_json_desc_t desc = ECS_ENTITY_TO_JSON_INIT;
    desc.serialize_values = true;
    desc.serialize_type_info = true;

    char *json = ecs_entity_to_json(world, e, &desc);
    test_assert(json != NULL);
    test_json(json, "{\"name\":\"Foo\", \"type_info\":{\"Position\":{\"x\":[\"int\"], \"y\":[\"int\"]}}, \"components\":{\"Position\":{\"x\":10, \"y\":20}}}");

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

    ecs_entity_t ecs_id(T) = ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "T"}),
        .members = {
            {"value", ecs_id(ecs_i32_t), .unit = u}
        }
    });
    test_assert(ecs_id(T) != 0);

    ecs_entity_t e = ecs_entity(world, { .name = "Foo" });
    ecs_set(world, e, T, {24});

    ecs_entity_to_json_desc_t desc = ECS_ENTITY_TO_JSON_INIT;
    desc.serialize_values = true;
    desc.serialize_type_info = true;

    char *json = ecs_entity_to_json(world, e, &desc);
    test_assert(json != NULL);
    test_json(json, "{\"name\":\"Foo\", \"type_info\":{\"T\":{\"value\":[\"int\", {\"unit\":\"celsius\", \"symbol\":\"°\"}]}}, \"components\":{\"T\":{\"value\":24}}}");

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

    ecs_entity_t ecs_id(T) = ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "T"}),
        .members = {
            {"value", ecs_id(ecs_i32_t), .unit = u}
        }
    });
    test_assert(ecs_id(T) != 0);

    ecs_entity_t e = ecs_entity(world, { .name = "Foo" });
    ecs_set(world, e, T, {24});

    ecs_entity_to_json_desc_t desc = ECS_ENTITY_TO_JSON_INIT;
    desc.serialize_values = true;
    desc.serialize_type_info = true;

    char *json = ecs_entity_to_json(world, e, &desc);
    test_assert(json != NULL);
    test_json(json, "{\"name\":\"Foo\", \"type_info\":{\"T\":{\"value\":[\"int\", {\"unit\":\"celsius\", \"symbol\":\"°\", \"quantity\":\"temperature\"}]}}, \"components\":{\"T\":{\"value\":24}}}");

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

    ecs_entity_t ecs_id(T) = ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "T"}),
        .members = {
            {"value", ecs_id(ecs_i32_t), .unit = u_3}
        }
    });
    test_assert(ecs_id(T) != 0);

    ecs_entity_t e = ecs_entity(world, { .name = "Foo" });
    ecs_set(world, e, T, {24});

    ecs_entity_to_json_desc_t desc = ECS_ENTITY_TO_JSON_INIT;
    desc.serialize_values = true;
    desc.serialize_type_info = true;

    char *json = ecs_entity_to_json(world, e, &desc);
    test_assert(json != NULL);
    test_assert(json != NULL);
    test_json(json, "{\"name\":\"Foo\", \"type_info\":{\"T\":{\"value\":[\"int\", {\"unit\":\"meters_per_second\", \"symbol\":\"m/s\"}]}}, \"components\":{\"T\":{\"value\":24}}}");

    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeEntityToJson_serialize_w_type_info_no_types(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_entity(world, { .name = "Foo" });
    ecs_set(world, e, Position, {10, 20});

    ecs_entity_to_json_desc_t desc = ECS_ENTITY_TO_JSON_INIT;
    desc.serialize_values = true;
    desc.serialize_type_info = true;

    char *json = ecs_entity_to_json(world, e, &desc);
    test_assert(json != NULL);
    test_json(json, "{\"name\":\"Foo\", \"type_info\":{\"Position\":0}, \"components\":{\"Position\":null}}");

    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeEntityToJson_serialize_w_type_info_no_components(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Foo);

    ecs_entity_t e = ecs_new(world);
    ecs_add(world, e, Foo);

    ecs_entity_to_json_desc_t desc = ECS_ENTITY_TO_JSON_INIT;
    desc.serialize_values = true;
    desc.serialize_type_info = true;

    char *json = ecs_entity_to_json(world, e, &desc);
    test_assert(json != NULL);
    char *expect = flecs_asprintf(
        "{\"name\":\"#%u\", \"tags\":[\"Foo\"],\"type_info\":{}}", (uint32_t)e);
    test_json(json, expect);

    ecs_os_free(json);
    ecs_os_free(expect);

    ecs_fini(world);
}

void SerializeEntityToJson_serialize_w_label(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);

    ecs_entity_t e = ecs_entity(world, { .name = "Foo" });
    ecs_add(world, e, Tag);
    ecs_doc_set_name(world, e, "My name");

    ecs_entity_to_json_desc_t desc = ECS_ENTITY_TO_JSON_INIT;
    desc.serialize_doc = true;
    char *json = ecs_entity_to_json(world, e, &desc);

    test_assert(json != NULL);
    test_json(json, "{\"name\":\"Foo\", \"doc\":{\"label\":\"My name\"}, \"tags\":[\"Tag\"], \"components\":{\"(flecs.doc.Description,flecs.core.Name)\":{\"value\":\"My name\"}}}");

    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeEntityToJson_serialize_w_label_no_name(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);

    ecs_entity_t e = ecs_new(world);
    ecs_add(world, e, Tag);

    char *str = flecs_asprintf("{\"name\":\"#%u\", \"doc\":{\"label\":\"#%u\"}, \"tags\":[\"Tag\"]}",
        (uint32_t)e,  (uint32_t)e);

    ecs_entity_to_json_desc_t desc = ECS_ENTITY_TO_JSON_INIT;
    desc.serialize_doc = true;
    char *json = ecs_entity_to_json(world, e, &desc);
    test_assert(json != NULL);
    test_json(json, str);

    ecs_os_free(json);
    ecs_os_free(str);

    ecs_fini(world);
}

void SerializeEntityToJson_serialize_w_brief(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);

    ecs_entity_t e = ecs_entity(world, { .name = "Foo" });
    ecs_add(world, e, Tag);
    ecs_doc_set_brief(world, e, "Brief description");

    ecs_entity_to_json_desc_t desc = ECS_ENTITY_TO_JSON_INIT;
    desc.serialize_doc = true;
    char *json = ecs_entity_to_json(world, e, &desc);
    test_assert(json != NULL);

    test_json(json, "{\"name\":\"Foo\", \"doc\":{\"label\":\"Foo\", \"brief\":\"Brief description\"}, \"tags\":[\"Tag\"], \"components\":{\"(flecs.doc.Description,flecs.doc.Brief)\":{\"value\":\"Brief description\"}}}");

    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeEntityToJson_serialize_w_brief_no_brief(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);

    ecs_entity_t e = ecs_entity(world, { .name = "Foo" });
    ecs_add(world, e, Tag);

    ecs_entity_to_json_desc_t desc = ECS_ENTITY_TO_JSON_INIT;
    desc.serialize_doc = true;
    char *json = ecs_entity_to_json(world, e, &desc);
    test_assert(json != NULL);

    test_json(json, "{\"name\":\"Foo\", \"doc\":{\"label\":\"Foo\"}, \"tags\":[\"Tag\"]}");

    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeEntityToJson_serialize_w_link(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);

    ecs_entity_t e = ecs_entity(world, { .name = "Foo" });
    ecs_add(world, e, Tag);
    ecs_doc_set_link(world, e, "Link");

    ecs_entity_to_json_desc_t desc = ECS_ENTITY_TO_JSON_INIT;
    desc.serialize_doc = true;
    char *json = ecs_entity_to_json(world, e, &desc);
    test_assert(json != NULL);

    test_json(json, "{\"name\":\"Foo\", \"doc\":{\"label\":\"Foo\", \"link\":\"Link\"}, \"tags\":[\"Tag\"], \"components\":{\"(flecs.doc.Description,flecs.doc.Link)\":{\"value\":\"Link\"}}}");

    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeEntityToJson_serialize_w_link_no_link(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);

    ecs_entity_t e = ecs_entity(world, { .name = "Foo" });
    ecs_add(world, e, Tag);

    ecs_entity_to_json_desc_t desc = ECS_ENTITY_TO_JSON_INIT;
    desc.serialize_doc = true;
    char *json = ecs_entity_to_json(world, e, &desc);
    test_assert(json != NULL);

    test_json(json, "{\"name\":\"Foo\", \"doc\":{\"label\":\"Foo\"}, \"tags\":[\"Tag\"]}");

    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeEntityToJson_serialize_color(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);

    ecs_entity_t e = ecs_entity(world, { .name = "Foo" });
    ecs_add(world, e, Tag);
    ecs_doc_set_color(world, e, "#47B576");

    ecs_entity_to_json_desc_t desc = ECS_ENTITY_TO_JSON_INIT;
    desc.serialize_doc = true;
    char *json = ecs_entity_to_json(world, e, &desc);
    test_assert(json != NULL);

    test_json(json, "{\"name\":\"Foo\", \"doc\":{\"label\":\"Foo\", \"color\":\"#47B576\"}, \"tags\":[\"Tag\"], \"components\":{\"(flecs.doc.Description,flecs.doc.Color)\":{\"value\":\"#47B576\"}}}");

    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeEntityToJson_serialize_w_doc_w_quotes(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);

    ecs_entity_t p = ecs_entity(world, { .name = "Parent" });
    ecs_entity_t e = ecs_entity(world, { .name = "Child" });
    ecs_add(world, e, Tag);
    ecs_add_pair(world, e, EcsChildOf, p);
    ecs_doc_set_name(world, e, "Doc \"name\"");
    ecs_doc_set_brief(world, e, "Doc \"brief\"");
    ecs_doc_set_link(world, e, "Doc \"link\"");
    ecs_doc_set_color(world, e, "Doc \"color\"");

    ecs_entity_to_json_desc_t desc = ECS_ENTITY_TO_JSON_INIT;
    desc.serialize_doc = true;
    char *json = ecs_entity_to_json(world, e, &desc);
    test_assert(json != NULL);

    test_json(json, "{\"parent\":\"Parent\", \"name\":\"Child\", \"doc\":{\"label\":\"Doc \\\"name\\\"\", \"brief\":\"Doc \\\"brief\\\"\", \"color\":\"Doc \\\"color\\\"\", \"link\":\"Doc \\\"link\\\"\"}, \"tags\":[\"Tag\"], \"components\":{\"(flecs.doc.Description,flecs.core.Name)\":{\"value\":\"Doc \\\"name\\\"\"}, \"(flecs.doc.Description,flecs.doc.Brief)\":{\"value\":\"Doc \\\"brief\\\"\"}, \"(flecs.doc.Description,flecs.doc.Link)\":{\"value\":\"Doc \\\"link\\\"\"}, \"(flecs.doc.Description,flecs.doc.Color)\":{\"value\":\"Doc \\\"color\\\"\"}}}");

    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeEntityToJson_serialize_from_core(void) {
    ecs_world_t *world = ecs_init();

    char *json = ecs_entity_to_json(world, EcsWorld, NULL);
    test_assert(json != NULL);
    test_json(json, "{\"parent\":\"flecs.core\", \"name\":\"World\", \"components\":{\"(flecs.core.Identifier,flecs.core.Symbol)\":null, \"(flecs.doc.Description,flecs.doc.Brief)\":{\"value\":\"Entity associated with world\"}}}");
    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeEntityToJson_serialize_w_1_alert(void) {
    ecs_world_t *world = ecs_init();

    ECS_IMPORT(world, FlecsAlerts);

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t alert = ecs_alert(world, {
        .entity = ecs_entity(world, { .name = "position_without_velocity" }),
        .query.expr = "Position, !Velocity",
        .message = "$this has Position but not Velocity"
    });

    ecs_entity_t e1 = ecs_entity(world, { .name = "e1" });
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

    test_json(json, "{\"name\":\"e1\", \"alerts\":true, \"components\":{\"flecs.alerts.AlertsActive\":{\"info_count\":0, \"warning_count\":0, \"error_count\":1}, \"Position\":null}, \"alerts\":[{\"alert\":\"position_without_velocity.e1_alert\", \"message\":\"e1 has Position but not Velocity\", \"severity\":\"Error\"}]}");
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
        .entity = ecs_entity(world, { .name = "position_without_velocity" }),
        .query.expr = "Position, !Velocity",
        .message = "$this has Position but not Velocity"
    });

    ecs_entity_t alert_2 = ecs_alert(world, {
        .entity = ecs_entity(world, { .name = "position_without_mass" }),
        .query.expr = "Position, !Mass",
        .message = "$this has Position but not Mass"
    });

    ecs_entity_t e1 = ecs_entity(world, { .name = "e1" });
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

    test_json(json, "{\"name\":\"e1\", \"alerts\":true, \"components\":{\"flecs.alerts.AlertsActive\":{\"info_count\":0, \"warning_count\":0, \"error_count\":2}, \"Position\":null}, \"alerts\":[{\"alert\":\"position_without_velocity.e1_alert_1\", \"message\":\"e1 has Position but not Velocity\", \"severity\":\"Error\"}, {\"alert\":\"position_without_mass.e1_alert_2\", \"message\":\"e1 has Position but not Mass\", \"severity\":\"Error\"}]}");
    // test_json(json, "{\"name\":\"e1\", \"alerts\":true, \"components\":{\"AlertsActive\":{\"info_count\":0, \"warning_count\":0, \"error_count\":2}, \"Position\":null, \"alerts\":[{\"alert\":\"position_without_mass.e1_alert_2\", \"message\":\"e1 has Position but not Mass\", \"severity\":\"Error\"}, {\"alert\":\"position_without_velocity.e1_alert_1\", \"message\":\"e1 has Position but not Velocity\", \"severity\":\"Error\"}]}");
    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeEntityToJson_serialize_w_child_alerts(void) {
    ecs_world_t *world = ecs_init();

    ECS_IMPORT(world, FlecsAlerts);

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t alert = ecs_alert(world, {
        .entity = ecs_entity(world, { .name = "position_without_velocity" }),
        .query.expr = "Position, !Velocity",
        .message = "$this has Position but not Velocity"
    });

    ecs_entity_t e1 = ecs_entity(world, { .name = "e1" });
    ecs_set(world, e1, Position, {10, 20});

    ecs_entity_t child_1 = ecs_entity(world, { .name = "child1" });
    ecs_set(world, child_1, Position, {10, 20});
    ecs_add_pair(world, child_1, EcsChildOf, e1);

    ecs_entity_t child_2 = ecs_entity(world, { .name = "child2" });
    ecs_set(world, child_2, Position, {10, 20});
    ecs_add_pair(world, child_2, EcsChildOf, e1);

    ecs_entity_t grandchild_1 = ecs_entity(world, { .name = "grandchild1" });
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

    test_json(json, "{\"name\":\"e1\", \"alerts\":true, \"components\":{\"flecs.alerts.AlertsActive\":{\"info_count\":0, \"warning_count\":0, \"error_count\":1}, \"Position\":null}, \"alerts\":[{\"alert\":\"position_without_velocity.e1_alert\", \"message\":\"e1 has Position but not Velocity\", \"severity\":\"Error\"}, {\"alert\":\"position_without_velocity.child1_alert\", \"message\":\"e1.child1 has Position but not Velocity\", \"severity\":\"Error\", \"path\":\"e1.child1\"}, {\"alert\":\"position_without_velocity.grandchild1_alert\", \"message\":\"e1.child1.grandchild1 has Position but not Velocity\", \"severity\":\"Error\", \"path\":\"e1.child1.grandchild1\"}, {\"alert\":\"position_without_velocity.child2_alert\", \"message\":\"e1.child2 has Position but not Velocity\", \"severity\":\"Error\", \"path\":\"e1.child2\"}]}");
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
        .entity = ecs_entity(world, { .name = "position_without_velocity" }),
        .query.expr = "Position, !Velocity",
        .message = "$this has Position but not Velocity",
        .severity_filters[0] = {
            .severity = EcsAlertWarning,
            .with = Tag
        }
    });

    ecs_entity_t e1 = ecs_entity(world, { .name = "e1" });
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

    test_json(json, "{\"name\":\"e1\", \"alerts\":true, \"tags\":[\"Tag\"], \"components\":{\"flecs.alerts.AlertsActive\":{\"info_count\":0, \"warning_count\":0, \"error_count\":1}, \"Position\":null}, \"alerts\":[{\"alert\":\"position_without_velocity.e1_alert\", \"message\":\"e1 has Position but not Velocity\", \"severity\":\"Warning\"}]}");
    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeEntityToJson_serialize_w_alerts_not_imported(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e1 = ecs_entity(world, { .name = "e1" });
    ecs_set(world, e1, Position, {10, 20});
    ecs_new_w_pair(world, EcsChildOf, e1);

    ecs_entity_to_json_desc_t desc = ECS_ENTITY_TO_JSON_INIT;
    desc.serialize_alerts = true;
    char *json = ecs_entity_to_json(world, e1, &desc);
    test_assert(json != NULL);

    test_json(json, "{\"name\":\"e1\", \"components\":{\"Position\":null}}");
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
        .entity = ecs_entity(world, { .name = "position_without_velocity" }),
        .query.expr = "Position, !Velocity"
    });

    ecs_entity_t e1 = ecs_entity(world, { .name = "e1" });
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

    test_json(json, "{\"name\":\"e1\", \"alerts\":true, \"tags\":[\"Tag\"], \"components\":{\"flecs.alerts.AlertsActive\":{\"info_count\":0, \"warning_count\":0, \"error_count\":1}, \"Position\":null}, \"alerts\":[{\"alert\":\"position_without_velocity.e1_alert\", \"severity\":\"Error\"}]}");
    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeEntityToJson_serialize_refs_childof(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Rel);

    ecs_entity_t e1 = ecs_entity(world, { .name = "e1" });

    ecs_entity_t child_1 = ecs_entity(world, { .name = "child1" });
    ecs_add_pair(world, child_1, EcsChildOf, e1);

    ecs_entity_t child_2 = ecs_entity(world, { .name = "child2" });
    ecs_add_pair(world, child_2, EcsChildOf, e1);

    ecs_entity_t child_3 = ecs_entity(world, { .name = "child3" });
    ecs_add_pair(world, child_3, Rel, e1);

    ecs_entity_to_json_desc_t desc = ECS_ENTITY_TO_JSON_INIT;
    desc.serialize_refs = EcsChildOf;
    char *json = ecs_entity_to_json(world, e1, &desc);
    test_assert(json != NULL);

    test_json(json, "{\"name\":\"e1\", \"refs\":{\"ChildOf\":[\"e1.child1\", \"e1.child2\"]}}");
    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeEntityToJson_serialize_refs_custom(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Rel);

    ecs_entity_t e1 = ecs_entity(world, { .name = "e1" });

    ecs_entity_t child_1 = ecs_entity(world, { .name = "child1" });
    ecs_add_pair(world, child_1, Rel, e1);

    ecs_entity_t child_2 = ecs_entity(world, { .name = "child2" });
    ecs_add_pair(world, child_2, Rel, e1);

    ecs_entity_t child_3 = ecs_entity(world, { .name = "child3" });
    ecs_add_pair(world, child_3, EcsChildOf, e1);

    ecs_entity_to_json_desc_t desc = ECS_ENTITY_TO_JSON_INIT;
    desc.serialize_refs = Rel;
    char *json = ecs_entity_to_json(world, e1, &desc);
    test_assert(json != NULL);

    test_json(json, "{\"name\":\"e1\", \"refs\":{\"Rel\":[\"child1\", \"child2\"]}}");
    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeEntityToJson_serialize_refs_wildcard(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Rel);

    ecs_entity_t e1 = ecs_entity(world, { .name = "e1" });

    ecs_entity_t child_1 = ecs_entity(world, { .name = "child1" });
    ecs_add_pair(world, child_1, Rel, e1);

    ecs_entity_t child_2 = ecs_entity(world, { .name = "child2" });
    ecs_add_pair(world, child_2, Rel, e1);

    ecs_entity_t child_3 = ecs_entity(world, { .name = "child3" });
    ecs_add_pair(world, child_3, EcsChildOf, e1);

    ecs_entity_to_json_desc_t desc = ECS_ENTITY_TO_JSON_INIT;
    desc.serialize_refs = EcsWildcard;
    char *json = ecs_entity_to_json(world, e1, &desc);
    test_assert(json != NULL);

    test_json(json, "{\"name\":\"e1\", \"refs\":{\"ChildOf\":[\"e1.child3\"], \"Rel\":[\"child1\", \"child2\"]}}");
    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeEntityToJson_serialize_matches_filter(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, TagC);

    ecs_entity_t e1 = ecs_entity(world, { .name = "e1" });
    ecs_add(world, e1, TagA);
    ecs_entity_t e2 = ecs_entity(world, { .name = "e2" });
    ecs_add(world, e2, TagB);

    ecs_query_t *f_a = ecs_query(world, {
        .entity = ecs_entity(world, { .name = "f_a"} ),
        .terms = {{ TagA }}
    });
    test_assert(f_a != NULL);

    ecs_query_t *f_b = ecs_query(world, {
        .entity = ecs_entity(world, { .name = "f_b"} ),
        .terms = {{ TagB }}
    });
    test_assert(f_b != NULL);

    ecs_query_t *f_c = ecs_query(world, {
        .entity = ecs_entity(world, { .name = "f_c"} ),
        .terms = {{ TagC }}
    });
    test_assert(f_c != NULL);

    ecs_entity_to_json_desc_t desc = ECS_ENTITY_TO_JSON_INIT;
    desc.serialize_matches = true;
    char *json = ecs_entity_to_json(world, e1, &desc);
    test_assert(json != NULL);
    test_json(json, "{\"name\":\"e1\", \"tags\":[\"TagA\"], \"matches\":[\"f_a\"]}");
    ecs_os_free(json);

    ecs_query_fini(f_a);
    ecs_query_fini(f_b);
    ecs_query_fini(f_c);

    ecs_fini(world);
}

void SerializeEntityToJson_serialize_matches_query(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, TagC);

    ecs_entity_t e1 = ecs_entity(world, { .name = "e1" });
    ecs_add(world, e1, TagA);
    ecs_entity_t e2 = ecs_entity(world, { .name = "e2" });
    ecs_add(world, e2, TagB);

    ecs_query_t *f_a = ecs_query(world, {
        .entity = ecs_entity(world, { .name = "f_a"} ),
        .terms = {{ TagA }}
    });
    test_assert(f_a != NULL);

    ecs_query_t *f_b = ecs_query(world, {
        .entity = ecs_entity(world, { .name = "f_b"} ),
        .terms = {{ TagB }}
    });
    test_assert(f_b != NULL);

    ecs_query_t *f_c = ecs_query(world, {
        .entity = ecs_entity(world, { .name = "f_c"} ),
        .terms = {{ TagC }}
    });
    test_assert(f_c != NULL);

    ecs_entity_to_json_desc_t desc = ECS_ENTITY_TO_JSON_INIT;
    desc.serialize_matches = true;
    char *json = ecs_entity_to_json(world, e1, &desc);
    test_assert(json != NULL);
    test_json(json, "{\"name\":\"e1\", \"tags\":[\"TagA\"], \"matches\":[\"f_a\"]}");
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

    ecs_entity_t e1 = ecs_entity(world, { .name = "e1" });
    ecs_add(world, e1, TagA);
    ecs_entity_t e2 = ecs_entity(world, { .name = "e2" });
    ecs_add(world, e2, TagB);

    ecs_query_t *f_a = ecs_query(world, {
        .entity = ecs_entity(world, { .name = "f_a"} ),
        .terms = {{ TagA }}
    });
    test_assert(f_a != NULL);

    ecs_query_t *f_b = ecs_query(world, {
        .entity = ecs_entity(world, { .name = "f_b"} ),
        .terms = {{ TagB }}
    });
    test_assert(f_b != NULL);

    ecs_query_t *f_c = ecs_query(world, {
        .entity = ecs_entity(world, { .name = "f_c"} ),
        .terms = {{ TagC }}
    });
    test_assert(f_c != NULL);

    ecs_entity_to_json_desc_t desc = ECS_ENTITY_TO_JSON_INIT;
    desc.serialize_matches = true;
    char *json = ecs_entity_to_json(world, e1, &desc);
    test_assert(json != NULL);
    test_json(json, "{\"name\":\"e1\", \"tags\":[\"TagA\"], \"matches\":[\"f_a\"]}");
    ecs_os_free(json);

    ecs_query_fini(f_a);
    ecs_query_fini(f_b);
    ecs_query_fini(f_c);

    ecs_fini(world);
}

void SerializeEntityToJson_serialize_no_matches(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, TagC);

    ecs_entity_t e1 = ecs_entity(world, { .name = "e1" });
    ecs_add(world, e1, TagA);
    ecs_entity_t e2 = ecs_entity(world, { .name = "e2" });
    ecs_add(world, e2, TagB);

    ecs_query_t *f_b = ecs_query(world, {
        .entity = ecs_entity(world, { .name = "f_b"} ),
        .terms = {{ TagB }}
    });
    test_assert(f_b != NULL);

    ecs_query_t *f_c = ecs_query(world, {
        .entity = ecs_entity(world, { .name = "f_c"} ),
        .terms = {{ TagC }}
    });
    test_assert(f_c != NULL);

    ecs_entity_to_json_desc_t desc = ECS_ENTITY_TO_JSON_INIT;
    desc.serialize_matches = true;
    char *json = ecs_entity_to_json(world, e1, &desc);
    test_assert(json != NULL);
    test_json(json, "{\"name\":\"e1\", \"tags\":[\"TagA\"], \"matches\":[]}");
    ecs_os_free(json);

    ecs_query_fini(f_b);
    ecs_query_fini(f_c);

    ecs_fini(world);
}

void SerializeEntityToJson_serialize_id_recycled(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t e = ecs_entity(world, { .name = "Foo" });
    ecs_delete(world, e);
    e = ecs_entity(world, { .name = "Foo" });

    ecs_entity_to_json_desc_t desc = ECS_ENTITY_TO_JSON_INIT;
    desc.serialize_entity_id = true;
    char *expect = flecs_asprintf("{\"name\":\"Foo\", \"id\":%u}",
        (uint32_t)e);
    char *json = ecs_entity_to_json(world, e, &desc);
    test_assert(json != NULL);
    test_json(json, expect);
    ecs_os_free(json);
    ecs_os_free(expect);

    ecs_fini(world);
}

void SerializeEntityToJson_serialize_union_target(void) {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Rel, Union);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);
    ECS_TAG(world, TgtC);

    ecs_entity_t e1 = ecs_entity(world, { .name = "e1" });
    ecs_add_pair(world, e1, Rel, TgtA);
    ecs_entity_t e2 = ecs_entity(world, { .name = "e2" });
    ecs_add_pair(world, e2, Rel, TgtB);
    ecs_entity_t e3 = ecs_entity(world, { .name = "e3" });
    ecs_add_pair(world, e3, Rel, TgtC);

    {
        char *json = ecs_entity_to_json(world, e1, NULL);
        test_assert(json != NULL);
        test_json(json, "{\"name\":\"e1\", \"pairs\":{\"Rel\":\"TgtA\"}}");
        ecs_os_free(json);
    }
    {
        char *json = ecs_entity_to_json(world, e2, NULL);
        test_assert(json != NULL);
        test_json(json, "{\"name\":\"e2\", \"pairs\":{\"Rel\":\"TgtB\"}}");
        ecs_os_free(json);
    }
    {
        char *json = ecs_entity_to_json(world, e3, NULL);
        test_assert(json != NULL);
        test_json(json, "{\"name\":\"e3\", \"pairs\":{\"Rel\":\"TgtC\"}}");
        ecs_os_free(json);
    }

    ecs_fini(world);
}

void SerializeEntityToJson_serialize_union_target_recycled(void) {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Rel, Union);

    ecs_entity_t e = ecs_new(world);
    ecs_delete(world, e);
    ECS_TAG(world, TgtA);

    test_assert(TgtA != (uint32_t)TgtA);

    ecs_entity_t e1 = ecs_entity(world, { .name = "e1" });
    ecs_add_pair(world, e1, Rel, TgtA);

    {
        char *json = ecs_entity_to_json(world, e1, NULL);
        test_assert(json != NULL);
        test_json(json, "{\"name\":\"e1\", \"pairs\":{\"Rel\":\"TgtA\"}}");
        ecs_os_free(json);
    }

    ecs_fini(world);
}

void SerializeEntityToJson_serialize_anonymous_w_builtin(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Foo);

    ecs_entity_t e = ecs_new(world);
    ecs_add(world, e, Foo);

    {
        char *expect = flecs_asprintf("{\"name\":\"#%u\", \"tags\":[\"Foo\"]}",
            (uint32_t)e);
        ecs_entity_to_json_desc_t desc = ECS_ENTITY_TO_JSON_INIT;
        desc.serialize_builtin = true;
        char *json = ecs_entity_to_json(world, e, &desc);
        test_json(json, expect);
        ecs_os_free(json);
        ecs_os_free(expect);
    }

    ecs_fini(world);
}

void SerializeEntityToJson_serialize_named_w_builtin(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Foo);

    ecs_entity_t e = ecs_entity(world, { .name = "e" });
    ecs_add(world, e, Foo);

    {
        ecs_entity_to_json_desc_t desc = ECS_ENTITY_TO_JSON_INIT;
        desc.serialize_builtin = true;
        char *json = ecs_entity_to_json(world, e, &desc);
        test_assert(json != NULL);
        test_json(json, "{\"name\":\"e\", \"tags\":[\"Foo\"], \"components\":{\"(flecs.core.Identifier,flecs.core.Name)\":null}}");
        ecs_os_free(json);
    }

    ecs_fini(world);
}

void SerializeEntityToJson_serialize_named_child_w_builtin(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Foo);

    ecs_entity_t p = ecs_entity(world, { .name = "p" });
    ecs_entity_t e = ecs_entity(world, { .parent = p, .name = "e" });
    ecs_add(world, e, Foo);

    {
        ecs_entity_to_json_desc_t desc = ECS_ENTITY_TO_JSON_INIT;
        desc.serialize_builtin = true;
        char *json = ecs_entity_to_json(world, e, &desc);
        test_assert(json != NULL);
        test_json(json, "{\"parent\":\"p\", \"name\":\"e\", \"tags\":[\"Foo\"],\"pairs\":{\"flecs.core.ChildOf\":\"p\"}, \"components\":{\"(flecs.core.Identifier,flecs.core.Name)\":null}}");
        ecs_os_free(json);
    }

    ecs_fini(world);
}

void SerializeEntityToJson_serialize_named_child_w_builtin_w_type_info(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Foo);

    ecs_entity_t p = ecs_entity(world, { .name = "p" });
    ecs_entity_t e = ecs_entity(world, { .parent = p, .name = "e" });
    ecs_add(world, e, Foo);

    {
        ecs_entity_to_json_desc_t desc = ECS_ENTITY_TO_JSON_INIT;
        desc.serialize_builtin = true;
        desc.serialize_type_info = true;
        char *json = ecs_entity_to_json(world, e, &desc);
        test_assert(json != NULL);
        test_json(json, "{\"parent\":\"p\", \"name\":\"e\", \"tags\":[\"Foo\"],\"pairs\":{\"flecs.core.ChildOf\":\"p\"},\"type_info\":{\"(flecs.core.Identifier,flecs.core.Name)\":0}, \"components\":{\"(flecs.core.Identifier,flecs.core.Name)\":null}}");
        ecs_os_free(json);
    }

    ecs_fini(world);
}

void SerializeEntityToJson_serialize_from_stage(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t e = ecs_entity(world, { .name = "Foo" });

    ecs_world_t *stage = ecs_get_stage(world, 0);

    char *json = ecs_entity_to_json(stage, e, NULL);
    test_assert(json != NULL);
    test_json(json, "{\"name\":\"Foo\"}");
    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeEntityToJson_serialize_sparse(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "Position"}),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    ecs_add_id(world, ecs_id(Position), EcsSparse);

    ecs_entity_t e = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_set_name(world, e, "Foo");

    char *json = ecs_entity_to_json(world, e, NULL);
    test_assert(json != NULL);
    test_json(json, "{\"name\":\"Foo\", \"components\":{\"Position\":{\"x\":10, \"y\":20}}}");
    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeEntityToJson_serialize_sparse_pair(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tgt);

    ecs_entity_t ecs_id(Position) = ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "Position"}),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    ecs_add_id(world, ecs_id(Position), EcsSparse);

    ecs_entity_t e = ecs_entity(world, { .name = "Foo" });
    ecs_set_pair(world, e, Position, Tgt, {10, 20});

    char *json = ecs_entity_to_json(world, e, NULL);
    test_assert(json != NULL);
    test_json(json, "{\"name\":\"Foo\", \"components\":{\"(Position,Tgt)\":{\"x\":10, \"y\":20}}}");
    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeEntityToJson_serialize_sparse_mixed(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "Position"}),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t ecs_id(Velocity) = ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "Velocity"}),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    ecs_add_id(world, ecs_id(Position), EcsSparse);

    ecs_entity_t e = ecs_insert(world, 
        ecs_value(Position, {10, 20}),
        ecs_value(Velocity, {1, 2}));
    ecs_set_name(world, e, "Foo");

    char *json = ecs_entity_to_json(world, e, NULL);
    test_assert(json != NULL);
    test_json(json, "{\"name\":\"Foo\", \"components\":{\"Position\":{\"x\":10, \"y\":20}, \"Velocity\":{\"x\":1, \"y\":2}}}");
    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeEntityToJson_serialize_sparse_inherited(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "Position"}),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    ecs_add_id(world, ecs_id(Position), EcsSparse);

    ecs_entity_t base = ecs_entity(world, { .name = "Base" });
    ecs_set(world, base, Position, {10, 20});
    ecs_entity_t e = ecs_entity(world, { .name = "Foo" });
    ecs_add_pair(world, e, EcsIsA, base);

    ecs_entity_to_json_desc_t desc = {
        .serialize_inherited = true,
        .serialize_values = true
    };
    char *json = ecs_entity_to_json(world, e, &desc);
    test_assert(json != NULL);
    test_json(json, "{\"name\":\"Foo\", \"pairs\":{\"IsA\":\"Base\"},\"inherited\":{\"Base\":{\"components\":{\"Position\":{\"x\":10, \"y\":20}}}}}");
    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeEntityToJson_serialize_sparse_inherited_pair(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tgt);

    ecs_entity_t ecs_id(Position) = ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "Position"}),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    ecs_add_id(world, ecs_id(Position), EcsSparse);

    ecs_entity_t base = ecs_entity(world, { .name = "Base" });
    ecs_set_pair(world, base, Position, Tgt, {10, 20});
    ecs_entity_t e = ecs_entity(world, { .name = "Foo" });
    ecs_add_pair(world, e, EcsIsA, base);

    ecs_entity_to_json_desc_t desc = {
        .serialize_inherited = true,
        .serialize_values = true
    };
    char *json = ecs_entity_to_json(world, e, &desc);
    test_assert(json != NULL);
    test_json(json, "{\"name\":\"Foo\", \"pairs\":{\"IsA\":\"Base\"},\"inherited\":{\"Base\":{\"components\":{\"(Position,Tgt)\":{\"x\":10, \"y\":20}}}}}");
    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeEntityToJson_serialize_sparse_inherited_mixed(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "Position"}),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t ecs_id(Velocity) = ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "Velocity"}),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    ecs_add_id(world, ecs_id(Position), EcsSparse);

    ecs_entity_t base = ecs_entity(world, { .name = "Base" });
    ecs_set(world, base, Position, {10, 20});
    ecs_set(world, base, Velocity, {1, 2});
    ecs_entity_t e = ecs_entity(world, { .name = "Foo" });
    ecs_add_pair(world, e, EcsIsA, base);

    ecs_entity_to_json_desc_t desc = {
        .serialize_inherited = true,
        .serialize_values = true
    };
    char *json = ecs_entity_to_json(world, e, &desc);
    test_assert(json != NULL);
    test_json(json, "{\"name\":\"Foo\", \"pairs\":{\"IsA\":\"Base\"},\"inherited\":{\"Base\":{\"components\":{\"Position\":{\"x\":10, \"y\":20}, \"Velocity\":{\"x\":1, \"y\":2}}}}, \"components\":{\"Velocity\":{\"x\":1, \"y\":2}}}");
    ecs_os_free(json);

    ecs_fini(world);
}
