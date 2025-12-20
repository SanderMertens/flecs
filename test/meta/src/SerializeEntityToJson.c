#include "flecs.h"
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

void SerializeEntityToJson_serialize_w_base_dont_inherit_tag(void) {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, TagA, (OnInstantiate, Inherit));
    ECS_ENTITY(world, TagB, (OnInstantiate, DontInherit));

    ecs_entity_t base = ecs_entity(world, { .name = "Base" });
    ecs_add(world, base, TagA);
    ecs_add(world, base, TagB);

    ecs_entity_t e = ecs_entity(world, { .name = "Foo" });
    ecs_add_pair(world, e, EcsIsA, base);

    ecs_entity_to_json_desc_t desc = {
        .serialize_inherited = true
    };
    char *json = ecs_entity_to_json(world, e, &desc);
    test_assert(json != NULL);
    test_json(json, "{\"name\":\"Foo\", \"pairs\":{\"IsA\":\"Base\"},\"inherited\":{\"Base\":{\"tags\":[\"TagA\"]}}}");

    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeEntityToJson_serialize_w_base_dont_inherit_component(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    ecs_add_pair(world, ecs_id(Velocity), EcsOnInstantiate, EcsDontInherit);

    ecs_entity_t base = ecs_entity(world, { .name = "Base" });
    ecs_add(world, base, Position);
    ecs_add(world, base, Velocity);

    ecs_entity_t e = ecs_entity(world, { .name = "Foo" });
    ecs_add_pair(world, e, EcsIsA, base);

    ecs_entity_to_json_desc_t desc = {
        .serialize_inherited = true
    };
    char *json = ecs_entity_to_json(world, e, &desc);
    test_assert(json != NULL);
    test_json(json, "{\"name\":\"Foo\", \"pairs\":{\"IsA\":\"Base\"},\"inherited\":{\"Base\":{\"components\":{\"Position\":null}}}}");

    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeEntityToJson_serialize_w_base_dont_inherit_pair(void) {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, RelA, (OnInstantiate, Inherit));
    ECS_ENTITY(world, RelB, (OnInstantiate, DontInherit));
    ECS_TAG(world, Tgt);

    ecs_entity_t base = ecs_entity(world, { .name = "Base" });
    ecs_add_pair(world, base, RelA, Tgt);
    ecs_add_pair(world, base, RelB, Tgt);

    ecs_entity_t e = ecs_entity(world, { .name = "Foo" });
    ecs_add_pair(world, e, EcsIsA, base);

    ecs_entity_to_json_desc_t desc = {
        .serialize_inherited = true
    };
    char *json = ecs_entity_to_json(world, e, &desc);
    test_assert(json != NULL);
    test_json(json, "{\"name\":\"Foo\", \"pairs\":{\"IsA\":\"Base\"},\"inherited\":{\"Base\":{\"pairs\":{\"RelA\":\"Tgt\"}}}}");

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
    test_json(json, "{\"name\":\"Foo\", \"pairs\":{\"IsA\":\"Base\"},\"inherited\":{\"Base\":{}}, \"components\":{\"Position\":{\"x\":10, \"y\":20}, \"Velocity\":{\"x\":1, \"y\":2}}}");

    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeEntityToJson_serialize_component_w_base_w_owned_no_reflection_data_override(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t base = ecs_new(world);
    ecs_set(world, base, Position, {10, 20});
    /* Give base a name so we don't have to test unstable entity ids */
    ecs_set_name(world, base, "Base");

    ecs_entity_t e = ecs_new(world);
    ecs_add_pair(world, e, EcsIsA, base);
    ecs_set(world, e, Velocity, {1, 2});
    /* Give our test entity a name so we don't have to test unstable entity ids */
    ecs_set_name(world, e, "SomeEntity");

    ecs_entity_to_json_desc_t desc = {
        .serialize_inherited = true,
        .serialize_values = true
    };
    char *json = ecs_entity_to_json(world, e, &desc);
    test_assert(json != NULL);
    test_json(json, "{\"name\":\"SomeEntity\", \"pairs\":{\"IsA\":\"Base\"},\"inherited\":{\"Base\":{}}, \"components\":{\"Position\":null, \"Velocity\":null}}");

    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeEntityToJson_serialize_component_tag_pair_w_all_inherit_kinds(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity(world, { .name = "t_override", .add_expr = "(OnInstantiate, Override)" });
    ecs_entity(world, { .name = "t_inherit", .add_expr = "(OnInstantiate, Inherit)" });
    ecs_entity(world, { .name = "t_dont_inherit", .add_expr = "(OnInstantiate, DontInherit)" });

    ecs_component(world, { 
        .entity = ecs_entity(world, { .name = "c_override", .add_expr = "(OnInstantiate, Override)" }),
        .type.size = 4, .type.alignment = 4 
    });
    ecs_component(world, { 
        .entity = ecs_entity(world, { .name = "c_inherit", .add_expr = "(OnInstantiate, Inherit)" }),
        .type.size = 4, .type.alignment = 4 
    });
    ecs_component(world, { 
        .entity = ecs_entity(world, { .name = "c_dont_inherit", .add_expr = "(OnInstantiate, DontInherit)" }),
        .type.size = 4, .type.alignment = 4 
    });

    ecs_entity(world, { .name = "tgt" });

    ecs_entity_t base = ecs_entity(world, { 
        .name = "base",
        .add_expr = "t_override, t_inherit, t_dont_inherit, "\
                    "c_override, c_inherit, c_dont_inherit, "\
                    "(t_override, tgt), (t_inherit, tgt), (t_dont_inherit, tgt), "
                    "(c_override, tgt), (c_inherit, tgt), (c_dont_inherit, tgt)"
    });
    test_assert(base != 0);

    ecs_entity_t e = ecs_entity(world, {
        .name = "e",
        .add_expr = "(IsA, base)"
    });
    test_assert(e != 0);

    ecs_entity_to_json_desc_t desc = {
        .serialize_inherited = true
    };

    char *json = ecs_entity_to_json(world, e, &desc);
    test_assert(json != NULL);
    test_json(json, "{\"name\":\"e\", \"tags\":[\"t_override\"],\"pairs\":{\"IsA\":\"base\", \"t_override\":\"tgt\"},\"inherited\":{\"base\":{\"tags\":[\"t_inherit\"], \"pairs\":{\"t_inherit\":\"tgt\"}, \"components\":{\"c_inherit\":null, \"(c_inherit,tgt)\":null}}}, \"components\":{\"c_override\":null, \"(c_override,tgt)\":null}}");

    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeEntityToJson_serialize_component_tag_pair_w_manual_override(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity(world, { .name = "t_inherit", .add_expr = "(OnInstantiate, Inherit)" });
    ecs_entity(world, { .name = "t_dont_inherit", .add_expr = "(OnInstantiate, DontInherit)" });

    ecs_component(world, { 
        .entity = ecs_entity(world, { .name = "c_inherit", .add_expr = "(OnInstantiate, Inherit)" }),
        .type.size = 4, .type.alignment = 4 
    });
    ecs_component(world, { 
        .entity = ecs_entity(world, { .name = "c_dont_inherit", .add_expr = "(OnInstantiate, DontInherit)" }),
        .type.size = 4, .type.alignment = 4 
    });

    ecs_entity(world, { .name = "tgt" });

    ecs_entity_t base = ecs_entity(world, { 
        .name = "base",
        .add_expr = "t_inherit, t_dont_inherit, "\
                    "c_inherit, c_dont_inherit, "\
                    "(t_inherit, tgt), (t_dont_inherit, tgt), "
                    "(c_inherit, tgt), (c_dont_inherit, tgt)"
    });
    test_assert(base != 0);

    ecs_entity_t e = ecs_entity(world, {
        .name = "e",
        .add_expr = "(IsA, base), t_inherit, c_inherit, (t_inherit, tgt), (c_inherit, tgt)"
    });
    test_assert(e != 0);

    ecs_entity_to_json_desc_t desc = {
        .serialize_inherited = true
    };

    char *json = ecs_entity_to_json(world, e, &desc);
    test_assert(json != NULL);
    test_json(json, "{\"name\":\"e\", \"tags\":[\"t_inherit\"],\"pairs\":{\"IsA\":\"base\", \"t_inherit\":\"tgt\"},\"inherited\":{\"base\":{\"tags\":[\"t_inherit\"], \"pairs\":{\"t_inherit\":\"tgt\"}, \"components\":{\"c_inherit\":null, \"(c_inherit,tgt)\":null}}}, \"components\":{\"c_inherit\":null, \"(c_inherit,tgt)\":null}}");

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
    test_json(json, "{\"parent\":\"flecs.core\", \"name\":\"World\", \"components\":{\"(flecs.core.Identifier,flecs.core.Symbol)\":{\"value\":\"flecs.core.World\"}, \"(flecs.doc.Description,flecs.doc.Brief)\":{\"value\":\"Entity associated with world\"}}}");
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

    test_json(json, "{\"name\":\"e1\", \"has_alerts\":true, \"components\":{\"flecs.alerts.AlertsActive\":{\"info_count\":0, \"warning_count\":0, \"error_count\":1}, \"Position\":null}, \"alerts\":[{\"alert\":\"position_without_velocity.e1_alert\", \"message\":\"e1 has Position but not Velocity\", \"severity\":\"Error\"}]}");
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

    /* Since alerts can come in any order, test against the two possibilities: */
    const char* alerts_option1 = "{\"name\":\"e1\", \"has_alerts\":true, \"components\":{\"flecs.alerts.AlertsActive\":{\"info_count\":0, \"warning_count\":0, \"error_count\":2}, \"Position\":null}, \"alerts\":[{\"alert\":\"position_without_velocity.e1_alert_1\", \"message\":\"e1 has Position but not Velocity\", \"severity\":\"Error\"}, {\"alert\":\"position_without_mass.e1_alert_2\", \"message\":\"e1 has Position but not Mass\", \"severity\":\"Error\"}]}";
    const char* alerts_option2 = "{\"name\":\"e1\", \"has_alerts\":true, \"components\":{\"flecs.alerts.AlertsActive\":{\"info_count\":0, \"warning_count\":0, \"error_count\":2}, \"Position\":null}, \"alerts\":[{\"alert\":\"position_without_mass.e1_alert_2\", \"message\":\"e1 has Position but not Mass\", \"severity\":\"Error\"}, {\"alert\":\"position_without_velocity.e1_alert_1\", \"message\":\"e1 has Position but not Velocity\", \"severity\":\"Error\"}]}";

    if(ecs_os_strcmp(json, alerts_option1) && ecs_os_strcmp(json, alerts_option2)) {
        // neither matched, so throw an assert.
        test_json(json, alerts_option1);
    }

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

    test_json(json, "{\"name\":\"e1\", \"has_alerts\":true, \"components\":{\"flecs.alerts.AlertsActive\":{\"info_count\":0, \"warning_count\":0, \"error_count\":1}, \"Position\":null}, \"alerts\":[{\"alert\":\"position_without_velocity.e1_alert\", \"message\":\"e1 has Position but not Velocity\", \"severity\":\"Error\"}, {\"alert\":\"position_without_velocity.child1_alert\", \"message\":\"e1.child1 has Position but not Velocity\", \"severity\":\"Error\", \"path\":\"e1.child1\"}, {\"alert\":\"position_without_velocity.grandchild1_alert\", \"message\":\"e1.child1.grandchild1 has Position but not Velocity\", \"severity\":\"Error\", \"path\":\"e1.child1.grandchild1\"}, {\"alert\":\"position_without_velocity.child2_alert\", \"message\":\"e1.child2 has Position but not Velocity\", \"severity\":\"Error\", \"path\":\"e1.child2\"}]}");
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

    test_json(json, "{\"name\":\"e1\", \"has_alerts\":true, \"tags\":[\"Tag\"], \"components\":{\"flecs.alerts.AlertsActive\":{\"info_count\":0, \"warning_count\":0, \"error_count\":1}, \"Position\":null}, \"alerts\":[{\"alert\":\"position_without_velocity.e1_alert\", \"message\":\"e1 has Position but not Velocity\", \"severity\":\"Warning\"}]}");
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

    test_json(json, "{\"name\":\"e1\", \"has_alerts\":true, \"tags\":[\"Tag\"], \"components\":{\"flecs.alerts.AlertsActive\":{\"info_count\":0, \"warning_count\":0, \"error_count\":1}, \"Position\":null}, \"alerts\":[{\"alert\":\"position_without_velocity.e1_alert\", \"severity\":\"Error\"}]}");
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
    test_quarantine("25 Jun 2025");

    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Rel, DontFragment, Exclusive);
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
    test_quarantine("25 Jun 2025");

    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Rel, DontFragment, Exclusive);

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
        test_json(json, "{\"name\":\"e\", \"tags\":[\"Foo\"], \"components\":{\"(flecs.core.Identifier,flecs.core.Name)\":{\"value\":\"e\"}}}");
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
        test_json(json, "{\"parent\":\"p\", \"name\":\"e\", \"tags\":[\"Foo\"],\"pairs\":{\"flecs.core.ChildOf\":\"p\"}, \"components\":{\"(flecs.core.Identifier,flecs.core.Name)\":{\"value\":\"e\"}}}");
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
        test_json(json, "{\"parent\":\"p\", \"name\":\"e\", \"tags\":[\"Foo\"],\"pairs\":{\"flecs.core.ChildOf\":\"p\"},\"type_info\":{\"(flecs.core.Identifier,flecs.core.Name)\":{\"value\":[\"text\"]}}, \"components\":{\"(flecs.core.Identifier,flecs.core.Name)\":{\"value\":\"e\"}}}");
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
    test_json(json, "{\"name\":\"Foo\", \"pairs\":{\"IsA\":\"Base\"},\"inherited\":{\"Base\":{\"components\":{\"Position\":{\"x\":10, \"y\":20}}}}, \"components\":{\"Velocity\":{\"x\":1, \"y\":2}}}");
    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeEntityToJson_serialize_sparse_w_type_info(void) {
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

    ecs_entity_to_json_desc_t desc = ECS_ENTITY_TO_JSON_INIT;
    desc.serialize_type_info = true;
    char *json = ecs_entity_to_json(world, e, &desc);
    test_assert(json != NULL);
    test_json(json, "{\"name\":\"Foo\", \"type_info\":{\"Position\":{\"x\":[\"int\"], \"y\":[\"int\"]}}, \"components\":{\"Position\":{\"x\":10, \"y\":20}}}");
    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeEntityToJson_serialize_sparse_tag(void) {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, SparseTag, Sparse);

    ecs_entity_t e = ecs_entity(world, { .name = "e" });
    ecs_add(world, e, SparseTag);

    ecs_entity_to_json_desc_t desc = {
        .serialize_values = true,
        .serialize_type_info = true
    };

    char *json = ecs_entity_to_json(world, e, &desc);
    test_str(json, "{\"name\":\"e\", \"type_info\":{}}");
    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeEntityToJson_serialize_auto_override_w_inherited(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_entity(world, { .name = "e"});
    ecs_auto_override(world, e, Position);

    ecs_entity_to_json_desc_t desc = {
        .serialize_inherited = true
    };

    char *json = ecs_entity_to_json(world, e, &desc);
    test_assert(json != NULL);
    test_json(json, "{\"name\":\"e\", \"tags\":[\"auto_override|Position\"]}");
    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeEntityToJson_serialize_auto_override(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_entity(world, { .name = "e"});
    ecs_auto_override(world, e, Position);

    char *json = ecs_entity_to_json(world, e, NULL);
    test_assert(json != NULL);
    test_json(json, "{\"name\":\"e\", \"tags\":[\"auto_override|Position\"]}");
    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeEntityToJson_serialize_auto_override_pair(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Rel);
    ECS_TAG(world, Tgt);

    ecs_entity_t e = ecs_entity(world, { .name = "e"});
    ecs_auto_override_pair(world, e, Rel, Tgt);

    char *json = ecs_entity_to_json(world, e, NULL);
    test_assert(json != NULL);
    test_json(json, "{\"name\":\"e\", \"tags\":[\"auto_override|(Rel,Tgt)\"]}");
    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeEntityToJson_serialize_auto_override_fullpath(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Rel);
    ECS_TAG(world, Tgt);

    ecs_entity_t tag = ecs_entity(world, { .name = "tags.foo"});

    ecs_entity_t e = ecs_entity(world, { .name = "e"});
    ecs_auto_override_id(world, e, tag);

    {
        ecs_entity_to_json_desc_t desc = {
            .serialize_full_paths = true
        };

        char *json = ecs_entity_to_json(world, e, &desc);
        test_assert(json != NULL);
        test_json(json, "{\"name\":\"e\", \"tags\":[\"auto_override|tags.foo\"]}");
        ecs_os_free(json);
    }

    {
        ecs_entity_to_json_desc_t desc = {
            .serialize_full_paths = false
        };

        char *json = ecs_entity_to_json(world, e, &desc);
        test_assert(json != NULL);
        test_json(json, "{\"name\":\"e\", \"tags\":[\"auto_override|foo\"]}");
        ecs_os_free(json);
    }

    ecs_fini(world);
}

void SerializeEntityToJson_serialize_auto_override_pair_fullpath(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Rel);
    ECS_TAG(world, Tgt);

    ecs_entity_t rel = ecs_entity(world, { .name = "tags.rel"});
    ecs_entity_t tgt = ecs_entity(world, { .name = "tags.tgt"});

    ecs_entity_t e = ecs_entity(world, { .name = "e"});
    ecs_auto_override_pair(world, e, rel, tgt);

    {
        ecs_entity_to_json_desc_t desc = {
            .serialize_full_paths = true
        };

        char *json = ecs_entity_to_json(world, e, &desc);
        test_assert(json != NULL);
        test_json(json, "{\"name\":\"e\", \"tags\":[\"auto_override|(tags.rel,tags.tgt)\"]}");
        ecs_os_free(json);
    }

    {
        ecs_entity_to_json_desc_t desc = {
            .serialize_full_paths = false
        };

        char *json = ecs_entity_to_json(world, e, &desc);
        test_assert(json != NULL);
        test_json(json, "{\"name\":\"e\", \"tags\":[\"auto_override|(rel,tgt)\"]}");
        ecs_os_free(json);
    }

    ecs_fini(world);
}

void SerializeEntityToJson_serialize_toggle(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_add_id(world, ecs_id(Position), EcsCanToggle);

    ecs_entity_t e = ecs_entity(world, { .name = "e"});
    ecs_add(world, e, Position);
    ecs_enable_component(world, e, Position, false);

    char *json = ecs_entity_to_json(world, e, NULL);
    test_assert(json != NULL);
    test_json(json, "{\"name\":\"e\", \"tags\":[\"toggle|Position\"], \"components\":{\"Position\":null}}");
    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeEntityToJson_serialize_toggle_pair(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Rel);
    ECS_TAG(world, Tgt);

    ecs_add_id(world, Rel, EcsCanToggle);

    ecs_entity_t e = ecs_entity(world, { .name = "e"});
    ecs_add_pair(world, e, Rel, Tgt);
    ecs_enable_pair(world, e, Rel, Tgt, false);

    char *json = ecs_entity_to_json(world, e, NULL);
    test_assert(json != NULL);
    test_json(json, "{\"name\":\"e\", \"tags\":[\"toggle|(Rel,Tgt)\"],\"pairs\":{\"Rel\":[\"Tgt\"]}}");
    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeEntityToJson_serialize_null_doc_name(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t e = ecs_new(world);
    ecs_set_pair(world, e, EcsDocDescription, EcsName, { NULL });

    char *json = ecs_entity_to_json(world, e, NULL);
    test_assert(json != NULL);
    
    char *expect = flecs_asprintf(
        "{\"name\":\"#%u\", \"components\":{\"(flecs.doc.Description,flecs.core.Name)\":{\"value\":null}}}", (uint32_t)e);

    test_json(json, expect);
    ecs_os_free(json);
    ecs_os_free(expect);

    ecs_fini(world);
}

void SerializeEntityToJson_serialize_base_w_invalid_component(void) {
    typedef enum {
        Red, Green, Blue
    } Color;

    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Color) = ecs_enum(world, {
        .entity = ecs_entity(world, { .name = "Color" }),
        .constants = {
            {"Red"}, {"Green"}, {"Blue"}
        }
    });

    ecs_add_pair(world, ecs_id(Color), EcsOnInstantiate, EcsInherit);

    ecs_entity_t base = ecs_entity(world, { .name = "base" });
    ecs_set(world, base, Color, {100});

    ecs_entity_t e = ecs_entity(world, { .name = "e" });
    ecs_add_pair(world, e, EcsIsA, base);

    ecs_entity_to_json_desc_t desc = {
        .serialize_inherited = true,
        .serialize_values = true
    };

    ecs_log_set_level(-4);
    char *json = ecs_entity_to_json(world, e, &desc);
    test_assert(json == NULL);

    ecs_fini(world);
}

bool serialize_w_blacklist_blcb(const ecs_world_t *world, ecs_entity_t e) {
    return false;
}

void SerializeEntityToJson_serialize_w_blacklist(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_entity(world, { .name = "e" });

    ecs_add(world, e, Position);
    ecs_set(world, e, Position, { 2, 3 });

    ecs_entity_to_json_desc_t desc = ECS_ENTITY_TO_JSON_INIT;
    desc.component_filter = &serialize_w_blacklist_blcb;

    char *json = ecs_entity_to_json(world, e, &desc);
    test_assert(json != NULL);
    test_json(json, "{\"name\":\"e\"}");
    ecs_os_free(json);

    ecs_fini(world);
}

bool serialize_w_allow_blacklist_blcb(const ecs_world_t *world, ecs_entity_t e) {
    return true;
}

void SerializeEntityToJson_serialize_w_allow_blacklist(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "Position"}),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t e = ecs_entity(world, { .name = "e" });

    ecs_add(world, e, Position);
    ecs_set(world, e, Position, { 2, 3 });

    ecs_entity_to_json_desc_t desc = ECS_ENTITY_TO_JSON_INIT;
    desc.component_filter = &serialize_w_allow_blacklist_blcb;

    char *json = ecs_entity_to_json(world, e, &desc);
    test_assert(json != NULL);
    test_json(json, "{\"name\":\"e\", \"components\":{\"Position\":{\"x\":2, \"y\":3}}}");
    ecs_os_free(json);

    ecs_fini(world);
}

ECS_DECLARE(NoSerialize);

bool serialize_w_partial_blacklist_blcb(const ecs_world_t *world, ecs_entity_t e) {
    return !ecs_has_id(world, e, ecs_id(NoSerialize));
}

void SerializeEntityToJson_serialize_w_partial_blacklist(void) {
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

    ECS_TAG(world, NoSerialize);
    ecs_add_id(world, NoSerialize, EcsTrait);
    ecs_add_id(world, ecs_id(Velocity), NoSerialize);

    ecs_entity_t e = ecs_entity(world, { .name = "e" });

    ecs_add(world, e, Position);
    ecs_set(world, e, Position, { 2, 3 });

    ecs_entity_to_json_desc_t desc = ECS_ENTITY_TO_JSON_INIT;
    desc.component_filter = &serialize_w_allow_blacklist_blcb;

    char *json = ecs_entity_to_json(world, e, &desc);
    test_assert(json != NULL);
    test_json(json, "{\"name\":\"e\", \"components\":{\"Position\":{\"x\":2, \"y\":3}}}");
    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeEntityToJson_serialize_value_pair(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Rel);

    ecs_entity_t e = ecs_entity(world, { .name = "e" });
    ecs_add_id(world, e, ecs_value_pair(Rel, 1));

    ecs_entity_to_json_desc_t desc = ECS_ENTITY_TO_JSON_INIT;
    char *json = ecs_entity_to_json(world, e, &desc);
    test_assert(json != NULL);
    test_json(json, "{\"name\":\"e\", \"pairs\":{\"Rel\":\"@1\"}}");
    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeEntityToJson_serialize_parent_component(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t p = ecs_entity(world, { .name = "p" });
    ecs_entity_t e = ecs_entity(world, { .name = "e" });
    ecs_set(world, e, EcsParent, {p});

    ecs_entity_to_json_desc_t desc = ECS_ENTITY_TO_JSON_INIT;
    char *json = ecs_entity_to_json(world, e, &desc);
    test_assert(json != NULL);
    test_json(json, "{\"parent\":\"p\", \"name\":\"e\", \"pairs\":{\"flecs.core.ParentDepth\":\"@1\"}, \"components\":{\"flecs.core.Parent\":{\"value\":\"p\"}}}");
    ecs_os_free(json);

    ecs_fini(world);
}
