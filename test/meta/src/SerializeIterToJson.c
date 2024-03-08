#include <meta.h>
#include <stdio.h>
#include <limits.h>
#include <math.h>

void SerializeIterToJson_serialize_1_comps_empty(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_id(Position),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    ecs_query_t *q = ecs_query_new(world, "Position");
    ecs_iter_t it = ecs_query_iter(world, q);

    char *json = ecs_iter_to_json(world, &it, NULL);
    test_assert(json != NULL);
    test_str(json, "{\"ids\":[[\"Position\"]], \"results\":[]}");

    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeIterToJson_serialize_1_comps_2_ents_same_table(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_id(Position),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t e1 = ecs_new_entity(world, "Foo");
    ecs_entity_t e2 = ecs_new_entity(world, "Bar");

    ecs_set(world, e1, Position, {10, 20});
    ecs_set(world, e2, Position, {30, 40});

    ecs_query_t *q = ecs_query_new(world, "Position");
    ecs_iter_t it = ecs_query_iter(world, q);

    char *json = ecs_iter_to_json(world, &it, NULL);
    test_str(json, 
    "{"
        "\"ids\":[[\"Position\"]], "
        "\"results\":[{"
            "\"ids\":[[\"Position\"]], "
            "\"sources\":[0], "
            "\"entities\":["
                "\"Foo\", \"Bar\""
            "], "
            "\"values\":[["
                "{\"x\":10, \"y\":20}, "
                "{\"x\":30, \"y\":40}"
            "]]"
        "}]"
    "}");

    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeIterToJson_serialize_2_comps_2_ents_same_table(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Mass);

    ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_id(Position),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_id(Mass),
        .members = {
            {"value", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t e1 = ecs_new_entity(world, "Foo");
    ecs_entity_t e2 = ecs_new_entity(world, "Bar");

    ecs_set(world, e1, Position, {10, 20});
    ecs_set(world, e2, Position, {30, 40});

    ecs_set(world, e1, Mass, {1});
    ecs_set(world, e2, Mass, {2});

    ecs_query_t *q = ecs_query_new(world, "Position, Mass");
    ecs_iter_t it = ecs_query_iter(world, q);

    char *json = ecs_iter_to_json(world, &it, NULL);
    test_str(json, 
    "{"
        "\"ids\":[[\"Position\"], [\"Mass\"]], "
        "\"results\":[{"
            "\"ids\":[[\"Position\"], [\"Mass\"]], "
            "\"sources\":[0, 0], "
            "\"entities\":["
                "\"Foo\", \"Bar\""
            "], "
            "\"values\":[["
                "{\"x\":10, \"y\":20}, "
                "{\"x\":30, \"y\":40}"
            "], ["
                "{\"value\":1}, "
                "{\"value\":2}"
            "]]"
        "}]"
    "}");

    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeIterToJson_serialize_1_tag_2_ents_same_table(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, MyTag);

    ecs_entity_t e1 = ecs_new_entity(world, "Foo");
    ecs_entity_t e2 = ecs_new_entity(world, "Bar");

    ecs_add(world, e1, MyTag);
    ecs_add(world, e2, MyTag);

    ecs_query_t *q = ecs_query_new(world, "MyTag");
    ecs_iter_t it = ecs_query_iter(world, q);

    char *json = ecs_iter_to_json(world, &it, NULL);
    test_str(json, 
    "{"
        "\"ids\":[[\"MyTag\"]], "
        "\"results\":[{"
            "\"ids\":[[\"MyTag\"]], "
            "\"sources\":[0], "
            "\"entities\":["
                "\"Foo\", \"Bar\""
            "]"
        "}]"
    "}");

    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeIterToJson_serialize_1_tag_1_comp_2_ents_same_table(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, MyTag);

    ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_id(Position),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t e1 = ecs_new_entity(world, "Foo");
    ecs_entity_t e2 = ecs_new_entity(world, "Bar");

    ecs_set(world, e1, Position, {10, 20});
    ecs_set(world, e2, Position, {30, 40});

    ecs_add(world, e1, MyTag);
    ecs_add(world, e2, MyTag);

    ecs_query_t *q = ecs_query_new(world, "Position, MyTag");
    ecs_iter_t it = ecs_query_iter(world, q);

    char *json = ecs_iter_to_json(world, &it, NULL);
    test_str(json, 
    "{"
        "\"ids\":[[\"Position\"], [\"MyTag\"]], "
        "\"results\":[{"
            "\"ids\":[[\"Position\"], [\"MyTag\"]], "
            "\"sources\":[0, 0], "
            "\"entities\":["
                "\"Foo\", \"Bar\""
            "], "
            "\"values\":[["
                "{\"x\":10, \"y\":20}, "
                "{\"x\":30, \"y\":40}"
            "], 0]"
        "}]"
    "}");

    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeIterToJson_serialize_1_tag_1_comp_4_ents_two_tables(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_id(Position),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t e1 = ecs_new_entity(world, "Foo");
    ecs_entity_t e2 = ecs_new_entity(world, "Bar");
    ecs_entity_t e3 = ecs_new_entity(world, "Hello");
    ecs_entity_t e4 = ecs_new_entity(world, "World");

    ecs_set(world, e1, Position, {10, 20});
    ecs_set(world, e2, Position, {30, 40});
    ecs_set(world, e3, Position, {50, 60});
    ecs_set(world, e4, Position, {70, 80});

    ecs_add(world, e1, TagA);
    ecs_add(world, e2, TagA);

    ecs_add(world, e3, TagA);
    ecs_add(world, e4, TagA);

    ecs_add(world, e3, TagB);
    ecs_add(world, e4, TagB);

    ecs_query_t *q = ecs_query_new(world, "Position, TagA");
    ecs_iter_t it = ecs_query_iter(world, q);

    char *json = ecs_iter_to_json(world, &it, NULL);
    test_str(json, 
    "{"
        "\"ids\":[[\"Position\"], [\"TagA\"]], "
        "\"results\":[{"
            "\"ids\":[[\"Position\"], [\"TagA\"]], "
            "\"sources\":[0, 0], "
            "\"entities\":["
                "\"Foo\", \"Bar\""
            "], "
            "\"values\":[["
                "{\"x\":10, \"y\":20}, "
                "{\"x\":30, \"y\":40}"
            "], 0]"
        "}, {"
            "\"ids\":[[\"Position\"], [\"TagA\"]], "
            "\"sources\":[0, 0], "
            "\"entities\":["
                "\"Hello\", \"World\""
            "], "
            "\"values\":[["
                "{\"x\":50, \"y\":60}, "
                "{\"x\":70, \"y\":80}"
            "], 0]"
        "}]"
    "}");

    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeIterToJson_serialize_2_comps_1_owned_2_ents(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Mass);

    ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_id(Position),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_id(Mass),
        .members = {
            {"value", ecs_id(ecs_i32_t)},
        }
    });

    ecs_entity_t base = ecs_new_entity(world, "Base");
    ecs_entity_t e1 = ecs_new_entity(world, "Foo");
    ecs_entity_t e2 = ecs_new_entity(world, "Bar");

    ecs_set(world, base, Mass, {100});
    ecs_set(world, e1, Position, {10, 20});
    ecs_set(world, e2, Position, {30, 40});

    ecs_add_pair(world, e1, EcsIsA, base);
    ecs_add_pair(world, e2, EcsIsA, base);

    ecs_query_t *q = ecs_query_new(world, "Position, Mass(up)");
    ecs_iter_t it = ecs_query_iter(world, q);

    char *json = ecs_iter_to_json(world, &it, NULL);
    test_str(json, 
    "{"
        "\"ids\":[[\"Position\"], [\"Mass\"]], "
        "\"results\":[{"
            "\"ids\":[[\"Position\"], [\"Mass\"]], "
            "\"sources\":[0, \"Base\"], "
            "\"entities\":["
                "\"Foo\", \"Bar\""
            "], "
            "\"values\":[["
                "{\"x\":10, \"y\":20}, "
                "{\"x\":30, \"y\":40}"
            "], {"
                "\"value\":100"
            "}]"
        "}]"
    "}");

    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeIterToJson_serialize_w_pair_wildcard(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Rel);
    ECS_TAG(world, ObjA);
    ECS_TAG(world, ObjB);

    ecs_entity_t e1 = ecs_new_entity(world, "Foo");
    ecs_entity_t e2 = ecs_new_entity(world, "Bar");

    ecs_add_pair(world, e1, Rel, ObjA);
    ecs_add_pair(world, e2, Rel, ObjB);

    ecs_query_t *q = ecs_query_new(world, "(Rel, *)");
    ecs_iter_t it = ecs_query_iter(world, q);

    char *json = ecs_iter_to_json(world, &it, NULL);
    test_str(json, 
    "{"
        "\"ids\":[[\"Rel\",\"*\"]], "
        "\"results\":[{"
            "\"ids\":[[\"Rel\",\"ObjA\"]], "
            "\"sources\":[0], "
            "\"entities\":["
                "\"Foo\""
            "]"
        "}, {"
            "\"ids\":[[\"Rel\",\"ObjB\"]], "
            "\"sources\":[0], "
            "\"entities\":["
                "\"Bar\""
            "]"
        "}]"
    "}");

    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeIterToJson_serialize_w_var(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Rel);
    ECS_TAG(world, ObjA);
    ECS_TAG(world, ObjB);

    ecs_entity_t e1 = ecs_new_entity(world, "Foo");
    ecs_entity_t e2 = ecs_new_entity(world, "Bar");

    ecs_add_pair(world, e1, Rel, ObjA);
    ecs_add_pair(world, e2, Rel, ObjB);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "(Rel, $X)"
    });

    ecs_iter_t it = ecs_rule_iter(world, r);

    char *json = ecs_iter_to_json(world, &it, NULL);
    test_str(json, 
    "{"
        "\"ids\":[[\"Rel\",\"*\"]], "
        "\"vars\":[\"X\"], "
        "\"results\":[{"
            "\"ids\":[[\"Rel\",\"ObjA\"]], "
            "\"sources\":[0], "
            "\"vars\":[\"ObjA\"], "
            "\"entities\":["
                "\"Foo\""
            "]"
        "}, {"
            "\"ids\":[[\"Rel\",\"ObjB\"]], "
            "\"sources\":[0], "
            "\"vars\":[\"ObjB\"], "
            "\"entities\":["
                "\"Bar\""
            "]"
        "}]"
    "}");

    ecs_os_free(json);

    ecs_rule_fini(r);

    ecs_fini(world);
}

void SerializeIterToJson_serialize_w_2_vars(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelX);
    ECS_TAG(world, ObjA);
    ECS_TAG(world, ObjB);

    ECS_TAG(world, RelY);
    ECS_TAG(world, ObjC);
    ECS_TAG(world, ObjD);

    ecs_entity_t e1 = ecs_new_entity(world, "Foo");
    ecs_entity_t e2 = ecs_new_entity(world, "Bar");

    ecs_add_pair(world, e1, RelX, ObjA);
    ecs_add_pair(world, e2, RelX, ObjB);

    ecs_add_pair(world, e1, RelY, ObjC);
    ecs_add_pair(world, e2, RelY, ObjD);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "(RelX, $X), (RelY, $Y)"
    });

    ecs_iter_t it = ecs_rule_iter(world, r);

    char *json = ecs_iter_to_json(world, &it, NULL);
    test_str(json, 
    "{"
        "\"ids\":[[\"RelX\",\"*\"], [\"RelY\",\"*\"]], "
        "\"vars\":[\"X\", \"Y\"], "
        "\"results\":[{"
            "\"ids\":[[\"RelX\",\"ObjA\"], [\"RelY\",\"ObjC\"]], "
            "\"sources\":[0, 0], "
            "\"vars\":[\"ObjA\", \"ObjC\"], "
            "\"entities\":["
                "\"Foo\""
            "]"
        "}, {"
            "\"ids\":[[\"RelX\",\"ObjB\"], [\"RelY\",\"ObjD\"]], "
            "\"sources\":[0, 0], "
            "\"vars\":[\"ObjB\", \"ObjD\"], "
            "\"entities\":["
                "\"Bar\""
            "]"
        "}]"
    "}");

    ecs_os_free(json);

    ecs_rule_fini(r);

    ecs_fini(world);
}

void SerializeIterToJson_serialize_type_info_1_tags(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);

    ecs_entity_t e1 = ecs_new_entity(world, "Foo");
    ecs_entity_t e2 = ecs_new_entity(world, "Bar");

    ecs_add(world, e1, TagA);
    ecs_add(world, e2, TagA);

    ecs_query_t *q = ecs_query_new(world, "TagA");
    ecs_iter_t it = ecs_query_iter(world, q);

    ecs_iter_to_json_desc_t desc = ECS_ITER_TO_JSON_INIT;
    desc.serialize_type_info = true;
    char *json = ecs_iter_to_json(world, &it, &desc);

    test_str(json, 
    "{"
        "\"ids\":[[\"TagA\"]], "
        "\"results\":[{"
            "\"ids\":[[\"TagA\"]], "
            "\"sources\":[0], "
            "\"entities\":["
                "\"Foo\", \"Bar\""
            "]"
        "}]"
    "}");

    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeIterToJson_serialize_type_info_2_tags(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_entity_t e1 = ecs_new_entity(world, "Foo");
    ecs_entity_t e2 = ecs_new_entity(world, "Bar");

    ecs_add(world, e1, TagA);
    ecs_add(world, e1, TagB);
    ecs_add(world, e2, TagA);
    ecs_add(world, e2, TagB);

    ecs_query_t *q = ecs_query_new(world, "TagA, TagB");
    ecs_iter_t it = ecs_query_iter(world, q);

    ecs_iter_to_json_desc_t desc = ECS_ITER_TO_JSON_INIT;
    desc.serialize_type_info = true;
    char *json = ecs_iter_to_json(world, &it, &desc);

    test_str(json, 
    "{"
        "\"ids\":[[\"TagA\"], [\"TagB\"]], "
        "\"results\":[{"
            "\"ids\":[[\"TagA\"], [\"TagB\"]], "
            "\"sources\":[0, 0], "
            "\"entities\":["
                "\"Foo\", \"Bar\""
            "]"
        "}]"
    "}");

    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeIterToJson_serialize_type_info_1_component(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e1 = ecs_new_entity(world, "Foo");
    ecs_entity_t e2 = ecs_new_entity(world, "Bar");

    ecs_add(world, e1, Position);
    ecs_add(world, e2, Position);

    ecs_query_t *q = ecs_query_new(world, "Position");
    ecs_iter_t it = ecs_query_iter(world, q);

    ecs_iter_to_json_desc_t desc = ECS_ITER_TO_JSON_INIT;
    desc.serialize_type_info = true;
    char *json = ecs_iter_to_json(world, &it, &desc);

    test_str(json, 
    "{"
        "\"ids\":[[\"Position\"]], "
        "\"type_info\":{\"Position\":0}, "
        "\"results\":[{"
            "\"ids\":[[\"Position\"]], "
            "\"sources\":[0], "
            "\"entities\":["
                "\"Foo\", \"Bar\""
            "], "
            "\"values\":[0]"
        "}]"
    "}");

    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeIterToJson_serialize_type_info_2_components(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t e1 = ecs_new_entity(world, "Foo");
    ecs_entity_t e2 = ecs_new_entity(world, "Bar");

    ecs_add(world, e1, Position);
    ecs_add(world, e2, Position);

    ecs_add(world, e1, Velocity);
    ecs_add(world, e2, Velocity);

    ecs_query_t *q = ecs_query_new(world, "Position, Velocity");
    ecs_iter_t it = ecs_query_iter(world, q);

    ecs_iter_to_json_desc_t desc = ECS_ITER_TO_JSON_INIT;
    desc.serialize_type_info = true;
    char *json = ecs_iter_to_json(world, &it, &desc);

    test_str(json, 
    "{"
        "\"ids\":[[\"Position\"], [\"Velocity\"]], "
        "\"type_info\":{\"Position\":0, \"Velocity\":0}, "
        "\"results\":[{"
            "\"ids\":[[\"Position\"], [\"Velocity\"]], "
            "\"sources\":[0, 0], "
            "\"entities\":["
                "\"Foo\", \"Bar\""
            "], "
            "\"values\":[0, 0]"
        "}]"
    "}");

    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeIterToJson_serialize_type_info_1_struct(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "Position"}),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    test_assert(t == ecs_id(Position));

    ecs_entity_t e1 = ecs_new_entity(world, "Foo");
    ecs_entity_t e2 = ecs_new_entity(world, "Bar");

    ecs_add(world, e1, Position);
    ecs_add(world, e2, Position);

    ecs_query_t *q = ecs_query_new(world, "Position");
    ecs_iter_t it = ecs_query_iter(world, q);

    ecs_iter_to_json_desc_t desc = ECS_ITER_TO_JSON_INIT;
    desc.serialize_type_info = true;
    char *json = ecs_iter_to_json(world, &it, &desc);

    test_str(json, 
    "{"
        "\"ids\":[[\"Position\"]], "
        "\"type_info\":{\"Position\":{\"x\":[\"int\"], \"y\":[\"int\"]}}, "
        "\"results\":[{"
            "\"ids\":[[\"Position\"]], "
            "\"sources\":[0], "
            "\"entities\":["
                "\"Foo\", \"Bar\""
            "], "
            "\"values\":[["
                "{\"x\":0, \"y\":0}, "
                "{\"x\":0, \"y\":0}"
            "]]"
        "}]"
    "}");

    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeIterToJson_serialize_type_info_1_component_1_struct(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "Position"}),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    test_assert(t == ecs_id(Position));

    ecs_entity_t e1 = ecs_new_entity(world, "Foo");
    ecs_entity_t e2 = ecs_new_entity(world, "Bar");

    ecs_add(world, e1, Position);
    ecs_add(world, e2, Position);

    ecs_add(world, e1, Velocity);
    ecs_add(world, e2, Velocity);

    ecs_query_t *q = ecs_query_new(world, "Position, Velocity");
    ecs_iter_t it = ecs_query_iter(world, q);

    ecs_iter_to_json_desc_t desc = ECS_ITER_TO_JSON_INIT;
    desc.serialize_type_info = true;
    char *json = ecs_iter_to_json(world, &it, &desc);

    test_str(json, 
    "{"
        "\"ids\":[[\"Position\"], [\"Velocity\"]], "
        "\"type_info\":{"
            "\"Position\":{\"x\":[\"int\"], \"y\":[\"int\"]}, "
            "\"Velocity\":0"
        "}, "
        "\"results\":[{"
            "\"ids\":[[\"Position\"], [\"Velocity\"]], "
            "\"sources\":[0, 0], "
            "\"entities\":["
                "\"Foo\", \"Bar\""
            "], "
            "\"values\":[["
                "{\"x\":0, \"y\":0}, "
                "{\"x\":0, \"y\":0}"
            "], 0]"
        "}]"
    "}");

    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeIterToJson_serialize_type_info_2_structs(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "Position"}),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });
    test_assert(t == ecs_id(Position));

    t = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "Velocity"}),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });
    test_assert(t == ecs_id(Velocity));

    ecs_entity_t e1 = ecs_new_entity(world, "Foo");
    ecs_entity_t e2 = ecs_new_entity(world, "Bar");

    ecs_add(world, e1, Position);
    ecs_add(world, e2, Position);

    ecs_add(world, e1, Velocity);
    ecs_add(world, e2, Velocity);

    ecs_query_t *q = ecs_query_new(world, "Position, Velocity");
    ecs_iter_t it = ecs_query_iter(world, q);

    ecs_iter_to_json_desc_t desc = ECS_ITER_TO_JSON_INIT;
    desc.serialize_type_info = true;
    char *json = ecs_iter_to_json(world, &it, &desc);

    test_str(json, 
    "{"
        "\"ids\":[[\"Position\"], [\"Velocity\"]], "
        "\"type_info\":{"
            "\"Position\":{\"x\":[\"int\"], \"y\":[\"int\"]}, "
            "\"Velocity\":{\"x\":[\"int\"], \"y\":[\"int\"]}"
        "}, "
        "\"results\":[{"
            "\"ids\":[[\"Position\"], [\"Velocity\"]], "
            "\"sources\":[0, 0], "
            "\"entities\":["
                "\"Foo\", \"Bar\""
            "], "
            "\"values\":[["
                "{\"x\":0, \"y\":0}, "
                "{\"x\":0, \"y\":0}"
            "], ["
                "{\"x\":0, \"y\":0}, "
                "{\"x\":0, \"y\":0}"
            "]]"
        "}]"
    "}");

    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeIterToJson_serialize_type_info_w_unit(void) {
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

    ecs_entity_t e1 = ecs_new_entity(world, "Foo");
    ecs_entity_t e2 = ecs_new_entity(world, "Bar");

    ecs_set(world, e1, T, {24});
    ecs_set(world, e2, T, {16});

    ecs_query_t *q = ecs_query_new(world, "T");
    ecs_iter_t it = ecs_query_iter(world, q);

    ecs_iter_to_json_desc_t desc = ECS_ITER_TO_JSON_INIT;
    desc.serialize_type_info = true;
    char *json = ecs_iter_to_json(world, &it, &desc);

    test_str(json, 
    "{"
        "\"ids\":[[\"T\"]], "
        "\"type_info\":{"
            "\"T\":{\"value\":[\"int\", {"
                "\"unit\":\"celsius\", \"symbol\":\"°\"}]"
            "}"
        "}, "
        "\"results\":[{"
            "\"ids\":[[\"T\"]], "
            "\"sources\":[0], "
            "\"entities\":["
                "\"Foo\", \"Bar\""
            "], "
            "\"values\":[["
                "{\"value\":24}, "
                "{\"value\":16}"
            "]]"
        "}]"
    "}");

    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeIterToJson_serialize_type_info_w_unit_quantity(void) {
    typedef struct {
        ecs_i32_t value;    
    } T;

    ecs_world_t *world = ecs_init();

    ecs_entity_t qt = ecs_quantity_init(world, &(ecs_entity_desc_t){
        .name = "temperature"
    });
    test_assert(qt != 0);

    ecs_entity_t u = ecs_unit_init(world, &(ecs_unit_desc_t){
        .entity = ecs_entity(world, {.name = "celsius"}),
        .symbol = "°",
        .quantity = qt
    });
    test_assert(u != 0);

    ecs_entity_t ecs_id(T) = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "T"}),
        .members = {
            {"value", ecs_id(ecs_i32_t), .unit = u}
        }
    });
    test_assert(ecs_id(T) != 0);

    ecs_entity_t e1 = ecs_new_entity(world, "Foo");
    ecs_entity_t e2 = ecs_new_entity(world, "Bar");

    ecs_set(world, e1, T, {24});
    ecs_set(world, e2, T, {16});

    ecs_query_t *q = ecs_query_new(world, "T");
    ecs_iter_t it = ecs_query_iter(world, q);

    ecs_iter_to_json_desc_t desc = ECS_ITER_TO_JSON_INIT;
    desc.serialize_type_info = true;
    char *json = ecs_iter_to_json(world, &it, &desc);

    test_str(json, 
    "{"
        "\"ids\":[[\"T\"]], "
        "\"type_info\":{"
            "\"T\":{\"value\":[\"int\", {"
                "\"unit\":\"celsius\", \"symbol\":\"°\", \"quantity\":\"temperature\"}]"
            "}"
        "}, "
        "\"results\":[{"
            "\"ids\":[[\"T\"]], "
            "\"sources\":[0], "
            "\"entities\":["
                "\"Foo\", \"Bar\""
            "], "
            "\"values\":[["
                "{\"value\":24}, "
                "{\"value\":16}"
            "]]"
        "}]"
    "}");

    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeIterToJson_serialize_type_info_w_unit_over(void) {
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

    ecs_entity_t e1 = ecs_new_entity(world, "Foo");
    ecs_entity_t e2 = ecs_new_entity(world, "Bar");

    ecs_set(world, e1, T, {24});
    ecs_set(world, e2, T, {16});

    ecs_query_t *q = ecs_query_new(world, "T");
    ecs_iter_t it = ecs_query_iter(world, q);

    ecs_iter_to_json_desc_t desc = ECS_ITER_TO_JSON_INIT;
    desc.serialize_type_info = true;
    char *json = ecs_iter_to_json(world, &it, &desc);

    test_str(json, 
    "{"
        "\"ids\":[[\"T\"]], "
        "\"type_info\":{"
            "\"T\":{\"value\":[\"int\", {"
                "\"unit\":\"meters_per_second\", \"symbol\":\"m/s\"}]"
            "}"
        "}, "
        "\"results\":[{"
            "\"ids\":[[\"T\"]], "
            "\"sources\":[0], "
            "\"entities\":["
                "\"Foo\", \"Bar\""
            "], "
            "\"values\":[["
                "{\"value\":24}, "
                "{\"value\":16}"
            "]]"
        "}]"
    "}");

    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeIterToJson_serialize_w_entity_label(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);

    ecs_entity_t e1 = ecs_new_entity(world, "foo_bar");
    ecs_entity_t e2 = ecs_new_entity(world, "hello_world");
    ecs_doc_set_name(world, e2, "Hello World");

    ecs_add(world, e1, Tag);
    ecs_add(world, e2, Tag);

    ecs_query_t *q = ecs_query_new(world, "Tag");
    ecs_iter_t it = ecs_query_iter(world, q);

    ecs_iter_to_json_desc_t desc = ECS_ITER_TO_JSON_INIT;
    desc.serialize_entity_labels = true;
    char *json = ecs_iter_to_json(world, &it, &desc);

    test_str(json, 
    "{"
        "\"ids\":[[\"Tag\"]], "
        "\"results\":[{"
            "\"ids\":[[\"Tag\"]], "
            "\"sources\":[0], "
            "\"entities\":["
                "\"foo_bar\""
            "]"
        "}, {"
            "\"ids\":[[\"Tag\"]], "
            "\"sources\":[0], "
            "\"entities\":["
                "\"hello_world\""
            "], "
            "\"entity_labels\":["
                "\"Hello World\""
            "]"
        "}]"
    "}");

    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeIterToJson_serialize_w_entity_label_w_str(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);

    ecs_entity_t e1 = ecs_new_entity(world, "foo_bar");
    ecs_entity_t e2 = ecs_new_entity(world, "hello_world");
    ecs_doc_set_name(world, e2, "Hello \"World\"");

    ecs_add(world, e1, Tag);
    ecs_add(world, e2, Tag);

    ecs_query_t *q = ecs_query_new(world, "Tag");
    ecs_iter_t it = ecs_query_iter(world, q);

    ecs_iter_to_json_desc_t desc = ECS_ITER_TO_JSON_INIT;
    desc.serialize_entity_labels = true;
    char *json = ecs_iter_to_json(world, &it, &desc);

    test_str(json, 
    "{"
        "\"ids\":[[\"Tag\"]], "
        "\"results\":[{"
            "\"ids\":[[\"Tag\"]], "
            "\"sources\":[0], "
            "\"entities\":["
                "\"foo_bar\""
            "]"
        "}, {"
            "\"ids\":[[\"Tag\"]], "
            "\"sources\":[0], "
            "\"entities\":["
                "\"hello_world\""
            "], "
            "\"entity_labels\":["
                "\"Hello \\\"World\\\"\""
            "]"
        "}]"
    "}");

    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeIterToJson_serialize_w_var_labels(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Rel);
    ECS_TAG(world, ObjA);
    ECS_TAG(world, ObjB);

    ecs_entity_t e1 = ecs_new_entity(world, "Foo");
    ecs_entity_t e2 = ecs_new_entity(world, "Bar");

    ecs_add_pair(world, e1, Rel, ObjA);
    ecs_add_pair(world, e2, Rel, ObjB);

    ecs_doc_set_name(world, ObjA, "Object A");

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "(Rel, $X)"
    });

    ecs_iter_t it = ecs_rule_iter(world, r);

    ecs_iter_to_json_desc_t desc = ECS_ITER_TO_JSON_INIT;
    desc.serialize_variable_labels = true;
    char *json = ecs_iter_to_json(world, &it, &desc);

    test_str(json, 
    "{"
        "\"ids\":[[\"Rel\",\"*\"]], "
        "\"vars\":[\"X\"], "
        "\"results\":[{"
            "\"ids\":[[\"Rel\",\"ObjA\"]], "
            "\"sources\":[0], "
            "\"vars\":[\"ObjA\"], "
            "\"var_labels\":[\"Object A\"], "
            "\"entities\":["
                "\"Foo\""
            "]"
        "}, {"
            "\"ids\":[[\"Rel\",\"ObjB\"]], "
            "\"sources\":[0], "
            "\"vars\":[\"ObjB\"], "
            "\"var_labels\":[\"ObjB\"], "
            "\"entities\":["
                "\"Bar\""
            "]"
        "}]"
    "}");

    ecs_os_free(json);

    ecs_rule_fini(r);

    ecs_fini(world);
}

void SerializeIterToJson_serialize_color(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);

    ecs_entity_t e1 = ecs_new_entity(world, "foo_bar");
    ecs_entity_t e2 = ecs_new_entity(world, "hello_world");
    ecs_doc_set_color(world, e2, "#47B576");

    ecs_add(world, e1, Tag);
    ecs_add(world, e2, Tag);

    ecs_query_t *q = ecs_query_new(world, "Tag");
    ecs_iter_t it = ecs_query_iter(world, q);

    ecs_iter_to_json_desc_t desc = ECS_ITER_TO_JSON_INIT;
    desc.serialize_colors = true;
    char *json = ecs_iter_to_json(world, &it, &desc);

    test_str(json, 
    "{"
        "\"ids\":[[\"Tag\"]], "
        "\"results\":[{"
            "\"ids\":[[\"Tag\"]], "
            "\"sources\":[0], "
            "\"entities\":["
                "\"foo_bar\""
            "]"
        "}, {"
            "\"ids\":[[\"Tag\"]], "
            "\"sources\":[0], "
            "\"entities\":["
                "\"hello_world\""
            "], "
            "\"colors\":["
                "\"#47B576\""
            "]"
        "}]"
    "}");

    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeIterToJson_serialize_w_var_component(void) {
    typedef struct {
        ecs_i32_t x;
    } T;

    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Rel);
    ECS_TAG(world, Obj);

    ecs_entity_t ecs_id(T) = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "T"}),
        .members = {
            {"x", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t e = ecs_new_entity(world, "Foo");
    ecs_add_pair(world, e, Rel, Obj);
    ecs_set(world, Obj, T, {10});

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "(Rel, $X), T($X)"
    });

    ecs_iter_t it = ecs_rule_iter(world, r);

    char *json = ecs_iter_to_json(world, &it, NULL);

    test_str(json, 
    "{"
        "\"ids\":[[\"Rel\",\"*\"], [\"T\"]], "
        "\"vars\":[\"X\"], "
        "\"results\":[{"
            "\"ids\":[[\"Rel\",\"Obj\"], [\"T\"]], "
            "\"sources\":[0, \"Obj\"], "
            "\"vars\":[\"Obj\"], "
            "\"entities\":["
                "\"Foo\""
            "], "
            "\"values\":[0, {\"x\":10}]"
        "}]"
    "}");

    ecs_os_free(json);

    ecs_rule_fini(r);

    ecs_fini(world);
}

void SerializeIterToJson_serialize_w_optional_tag(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_entity_t e2 = ecs_new_entity(world, "e2");

    ecs_add(world, e1, TagB);
    ecs_add(world, e1, TagA);
    ecs_add(world, e2, TagA);

    ecs_query_t *q = ecs_query_new(world, "TagA, ?TagB");
    ecs_iter_t it = ecs_query_iter(world, q);

    char *json = ecs_iter_to_json(world, &it, NULL);

    test_str(json, 
    "{"
        "\"ids\":[[\"TagA\"], [\"TagB\"]], "
        "\"results\":[{"
            "\"ids\":[[\"TagA\"], [\"TagB\"]], "
            "\"sources\":[0, 0], "
            "\"is_set\":[true, true], "
            "\"entities\":["
                "\"e1\""
            "]"
        "}, {"
            "\"ids\":[[\"TagA\"], [\"TagB\"]], "
            "\"sources\":[0, 0], "
            "\"is_set\":[true, false], "
            "\"entities\":["
                "\"e2\""
            "]"
        "}]"
    "}");

    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeIterToJson_serialize_w_optional_component(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_entity_t e2 = ecs_new_entity(world, "e2");

    ecs_add(world, e1, Velocity);
    ecs_add(world, e1, Position);
    ecs_add(world, e2, Position);

    ecs_query_t *q = ecs_query_new(world, "Position, ?Velocity");
    ecs_iter_t it = ecs_query_iter(world, q);

    char *json = ecs_iter_to_json(world, &it, NULL);

    test_str(json, 
    "{"
        "\"ids\":[[\"Position\"], [\"Velocity\"]], "
        "\"results\":[{"
            "\"ids\":[[\"Position\"], [\"Velocity\"]], "
            "\"sources\":[0, 0], "
            "\"is_set\":[true, true], "
            "\"entities\":["
                "\"e1\""
            "], "
            "\"values\":[0, 0]"
        "}, {"
            "\"ids\":[[\"Position\"], [\"Velocity\"]], "
            "\"sources\":[0, 0], "
            "\"is_set\":[true, false], "
            "\"entities\":["
                "\"e2\""
            "], "
            "\"values\":[0, 0]"
        "}]"
    "}");

    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeIterToJson_serialize_w_optional_reflected_component(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "Position"}),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t ecs_id(Velocity) = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "Velocity"}),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_entity_t e2 = ecs_new_entity(world, "e2");

    ecs_set(world, e1, Velocity, {1, 2});
    ecs_set(world, e1, Position, {10, 20});
    ecs_set(world, e2, Position, {10, 20});

    ecs_query_t *q = ecs_query_new(world, "Position, ?Velocity");
    ecs_iter_t it = ecs_query_iter(world, q);

    char *json = ecs_iter_to_json(world, &it, NULL);

    test_str(json, 
    "{"
        "\"ids\":[[\"Position\"], [\"Velocity\"]], "
        "\"results\":[{"
            "\"ids\":[[\"Position\"], [\"Velocity\"]], "
            "\"sources\":[0, 0], "
            "\"is_set\":[true, true], "
            "\"entities\":["
                "\"e1\""
            "], "
            "\"values\":[[{\"x\":10, \"y\":20}], [{\"x\":1, \"y\":2}]]"
        "}, {"
            "\"ids\":[[\"Position\"], [\"Velocity\"]], "
            "\"sources\":[0, 0], "
            "\"is_set\":[true, false], "
            "\"entities\":["
                "\"e2\""
            "], "
            "\"values\":[[{\"x\":10, \"y\":20}], []]"
        "}]"
    "}");

    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeIterToJson_serialize_w_inout_filter_tag(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);

    ecs_entity_t e1 = ecs_new_entity(world, "e1");

    ecs_add(world, e1, TagA);

    ecs_query_t *q = ecs_query_new(world, "[none] TagA");
    ecs_iter_t it = ecs_query_iter(world, q);

    char *json = ecs_iter_to_json(world, &it, NULL);

    test_str(json, 
    "{"
        "\"ids\":[[\"TagA\"]], "
        "\"results\":[{"
            "\"ids\":[[\"TagA\"]], "
            "\"sources\":[0], "
            "\"entities\":["
                "\"e1\""
            "]"
        "}]"
    "}");

    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeIterToJson_serialize_w_inout_filter_component(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_id(Velocity),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t e1 = ecs_new_entity(world, "e1");

    ecs_set(world, e1, Position, {10, 20});
    ecs_set(world, e1, Velocity, {1, 2});

    ecs_query_t *q = ecs_query_new(world, "[none] Position, Velocity");
    ecs_iter_t it = ecs_query_iter(world, q);

    char *json = ecs_iter_to_json(world, &it, NULL);

    test_str(json, 
    "{"
        "\"ids\":[[\"Position\"], [\"Velocity\"]], "
        "\"results\":[{"
            "\"ids\":[[\"Position\"], [\"Velocity\"]], "
            "\"sources\":[0, 0], "
            "\"entities\":["
                "\"e1\""
            "], "
            "\"values\":[0, [{\"x\":1, \"y\":2}]]"
        "}]"
    "}");

    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeIterToJson_serialize_w_inout_filter_reflected_component(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_id(Position),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_id(Velocity),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t e1 = ecs_new_entity(world, "e1");

    ecs_set(world, e1, Position, {10, 20});
    ecs_set(world, e1, Velocity, {1, 2});

    ecs_query_t *q = ecs_query_new(world, "[none] Position, Velocity");
    ecs_iter_t it = ecs_query_iter(world, q);

    char *json = ecs_iter_to_json(world, &it, NULL);

    test_str(json, 
    "{"
        "\"ids\":[[\"Position\"], [\"Velocity\"]], "
        "\"results\":[{"
            "\"ids\":[[\"Position\"], [\"Velocity\"]], "
            "\"sources\":[0, 0], "
            "\"entities\":["
                "\"e1\""
            "], "
            "\"values\":[0, [{\"x\":1, \"y\":2}]]"
        "}]"
    "}");

    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeIterToJson_serialize_w_inout_out_tag(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);

    ecs_entity_t e1 = ecs_new_entity(world, "e1");

    ecs_add(world, e1, TagA);

    ecs_query_t *q = ecs_query_new(world, "[out] TagA");
    ecs_iter_t it = ecs_query_iter(world, q);

    char *json = ecs_iter_to_json(world, &it, NULL);

    test_str(json, 
    "{"
        "\"ids\":[[\"TagA\"]], "
        "\"results\":[{"
            "\"ids\":[[\"TagA\"]], "
            "\"sources\":[0], "
            "\"entities\":["
                "\"e1\""
            "]"
        "}]"
    "}");

    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeIterToJson_serialize_w_inout_out_component(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_id(Velocity),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t e1 = ecs_new_entity(world, "e1");

    ecs_set(world, e1, Position, {10, 20});
    ecs_set(world, e1, Velocity, {1, 2});

    ecs_query_t *q = ecs_query_new(world, "[out] Position, Velocity");
    ecs_iter_t it = ecs_query_iter(world, q);

    char *json = ecs_iter_to_json(world, &it, NULL);

    test_str(json, 
    "{"
        "\"ids\":[[\"Position\"], [\"Velocity\"]], "
        "\"results\":[{"
            "\"ids\":[[\"Position\"], [\"Velocity\"]], "
            "\"sources\":[0, 0], "
            "\"entities\":["
                "\"e1\""
            "], "
            "\"values\":[0, [{\"x\":1, \"y\":2}]]"
        "}]"
    "}");

    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeIterToJson_serialize_w_inout_out_reflected_component(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_id(Position),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_id(Velocity),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t e1 = ecs_new_entity(world, "e1");

    ecs_set(world, e1, Position, {10, 20});
    ecs_set(world, e1, Velocity, {1, 2});

    ecs_query_t *q = ecs_query_new(world, "[out] Position, Velocity");
    ecs_iter_t it = ecs_query_iter(world, q);

    char *json = ecs_iter_to_json(world, &it, NULL);

    test_str(json, 
    "{"
        "\"ids\":[[\"Position\"], [\"Velocity\"]], "
        "\"results\":[{"
            "\"ids\":[[\"Position\"], [\"Velocity\"]], "
            "\"sources\":[0, 0], "
            "\"entities\":["
                "\"e1\""
            "], "
            "\"values\":[0, [{\"x\":1, \"y\":2}]]"
        "}]"
    "}");

    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeIterToJson_serialize_component_from_var(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_id(Position),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t e1 = ecs_new_entity(world, "e1");

    ecs_set(world, e1, Position, {10, 20});

    ecs_rule_t *r = ecs_rule_new(world, "Position($E)");
    ecs_iter_t it = ecs_rule_iter(world, r);

    char *json = ecs_iter_to_json(world, &it, NULL);

    test_str(json, 
    "{"
        "\"ids\":[[\"Position\"]], "
        "\"vars\":[\"E\"], "
        "\"results\":[{"
            "\"ids\":[[\"Position\"]], "
            "\"sources\":[\"e1\"], "
            "\"vars\":[\"e1\"], "
            "\"values\":[{\"x\":10, \"y\":20}]"
        "}]"
    "}");

    ecs_os_free(json);

    ecs_rule_fini(r);

    ecs_fini(world);
}

void SerializeIterToJson_serialize_ids(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);

    ecs_query_t *q = ecs_query_new(world, "Tag");

    ecs_entity_t e = ecs_new_entity(world, "e");
    ecs_add(world, e, Tag);

    ecs_iter_t it = ecs_query_iter(world, q);
    ecs_iter_to_json_desc_t desc = ECS_ITER_TO_JSON_INIT;
    desc.serialize_ids = false;
    desc.serialize_entity_ids = true;
    desc.serialize_values = false;
    desc.serialize_is_set = false;
    desc.serialize_sources = false;
    char *json = ecs_iter_to_json(world, &it, &desc);
    test_assert(json != NULL);

    char *expect = ecs_asprintf(
        "{\"ids\":[[\"Tag\"]], \"results\":[{\"entities\":[\"e\"], \"entity_ids\":[%u]}]}",
        (uint32_t)e);
    test_str(json, expect);

    ecs_os_free(json);
    ecs_os_free(expect);

    ecs_fini(world);
}

void SerializeIterToJson_serialize_ids_2_entities(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);

    ecs_query_t *q = ecs_query_new(world, "Tag");

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_add(world, e1, Tag);

    ecs_entity_t e2 = ecs_new_entity(world, "e2");
    ecs_add(world, e2, Tag);

    ecs_iter_t it = ecs_query_iter(world, q);
    ecs_iter_to_json_desc_t desc = ECS_ITER_TO_JSON_INIT;
    desc.serialize_ids = false;
    desc.serialize_entity_ids = true;
    desc.serialize_values = false;
    desc.serialize_is_set = false;
    desc.serialize_sources = false;
    char *json = ecs_iter_to_json(world, &it, &desc);
    test_assert(json != NULL);

    char *expect = ecs_asprintf(
        "{\"ids\":[[\"Tag\"]], \"results\":[{\"entities\":[\"e1\", \"e2\"], \"entity_ids\":[%u, %u]}]}",
        (uint32_t)e1, (uint32_t)e2);
    test_str(json, expect);

    ecs_os_free(json);
    ecs_os_free(expect);

    ecs_fini(world);
}

void SerializeIterToJson_serialize_anonymous(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);

    ecs_query_t *q = ecs_query_new(world, "Tag");

    ecs_entity_t e1 = 5000;
    ecs_entity_t e2 = 10000;
    ecs_entity_t e3 = 100000;
    ecs_entity_t e4 = 1000000;
    ecs_make_alive(world, 5000);
    ecs_make_alive(world, 10000);
    ecs_make_alive(world, 100000);
    ecs_make_alive(world, 1000000);
    ecs_add(world, e1, Tag);
    ecs_add(world, e2, Tag);
    ecs_add(world, e3, Tag);
    ecs_add(world, e4, Tag);

    ecs_iter_t it = ecs_query_iter(world, q);
    ecs_iter_to_json_desc_t desc = {0};
    desc.serialize_entities = true;
    char *json = ecs_iter_to_json(world, &it, &desc);
    test_assert(json != NULL);

    char *expect = ecs_asprintf(
        "{\"results\":[{\"entities\":[5000, 10000, 100000, 1000000]}]}");
    test_str(json, expect);

    ecs_os_free(json);
    ecs_os_free(expect);

    ecs_fini(world);
}

void SerializeIterToJson_serialize_anonymous_ids(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);

    ecs_query_t *q = ecs_query_new(world, "Tag");

    ecs_entity_t e1 = 5000;
    ecs_entity_t e2 = 10000;
    ecs_entity_t e3 = 100000;
    ecs_entity_t e4 = 1000000;
    ecs_make_alive(world, 5000);
    ecs_make_alive(world, 10000);
    ecs_make_alive(world, 100000);
    ecs_make_alive(world, 1000000);
    ecs_add(world, e1, Tag);
    ecs_add(world, e2, Tag);
    ecs_add(world, e3, Tag);
    ecs_add(world, e4, Tag);

    ecs_iter_t it = ecs_query_iter(world, q);
    ecs_iter_to_json_desc_t desc = {0};
    desc.serialize_entity_ids = true;
    char *json = ecs_iter_to_json(world, &it, &desc);
    test_assert(json != NULL);

    char *expect = ecs_asprintf(
        "{\"results\":[{\"entity_ids\":[5000, 10000, 100000, 1000000]}]}");
    test_str(json, expect);

    ecs_os_free(json);
    ecs_os_free(expect);

    ecs_fini(world);
}

void SerializeIterToJson_serialize_variable_ids(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);

    ecs_rule_t *q = ecs_rule_new(world, "Tag($Entity)");

    ecs_entity_t e = ecs_new_entity(world, "e");
    ecs_add(world, e, Tag);

    ecs_iter_t it = ecs_rule_iter(world, q);
    ecs_iter_to_json_desc_t desc = ECS_ITER_TO_JSON_INIT;
    desc.serialize_ids = false;
    desc.serialize_variable_ids = true;
    desc.serialize_values = false;
    desc.serialize_is_set = false;
    desc.serialize_sources = false;
    char *json = ecs_iter_to_json(world, &it, &desc);
    test_assert(json != NULL);

    char *expect = ecs_asprintf(
        "{\"ids\":[[\"Tag\"]], \"vars\":[\"Entity\"], \"results\":[{\"vars\":[\"e\"], \"var_ids\":[%u]}]}",
        (uint32_t)e);
    test_str(json, expect);

    ecs_os_free(json);
    ecs_os_free(expect);

    ecs_rule_fini(q);

    ecs_fini(world);
}

void SerializeIterToJson_serialize_variable_ids_2_entities(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);

    ecs_rule_t *q = ecs_rule_new(world, "Tag($Entity)");

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_add(world, e1, Tag);

    ecs_entity_t e2 = ecs_new_entity(world, "e2");
    ecs_add(world, e2, Tag);

    ecs_iter_t it = ecs_rule_iter(world, q);
    ecs_iter_to_json_desc_t desc = ECS_ITER_TO_JSON_INIT;
    desc.serialize_ids = false;
    desc.serialize_variable_ids = true;
    desc.serialize_values = false;
    desc.serialize_is_set = false;
    desc.serialize_sources = false;
    char *json = ecs_iter_to_json(world, &it, &desc);
    test_assert(json != NULL);

    char *expect = ecs_asprintf(
        "{\"ids\":[[\"Tag\"]], \"vars\":[\"Entity\"], \"results\":[{\"vars\":[\"e1\"], \"var_ids\":[%u]}, {\"vars\":[\"e2\"], \"var_ids\":[%u]}]}",
        (uint32_t)e1, (uint32_t)e2);
    test_str(json, expect);

    ecs_os_free(json);
    ecs_os_free(expect);

    ecs_rule_fini(q);

    ecs_fini(world);
}

void SerializeIterToJson_serialize_variable_anonymous(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);

    ecs_rule_t *q = ecs_rule_new(world, "Tag($Entity)");

    ecs_entity_t e = 10000;
    ecs_make_alive(world, 10000);
    ecs_add(world, e, Tag);

    ecs_iter_t it = ecs_rule_iter(world, q);
    ecs_iter_to_json_desc_t desc = ECS_ITER_TO_JSON_INIT;
    desc.serialize_ids = false;
    desc.serialize_values = false;
    desc.serialize_is_set = false;
    desc.serialize_sources = false;
    char *json = ecs_iter_to_json(world, &it, &desc);
    test_assert(json != NULL);

    char *expect = ecs_asprintf(
        "{\"ids\":[[\"Tag\"]], \"vars\":[\"Entity\"], \"results\":[{\"vars\":[\"10000\"]}]}",
        (uint32_t)e);
    test_str(json, expect);

    ecs_os_free(json);
    ecs_os_free(expect);

    ecs_rule_fini(q);

    ecs_fini(world);
}

void SerializeIterToJson_serialize_variable_anonymous_ids(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);

    ecs_rule_t *q = ecs_rule_new(world, "Tag($Entity)");

    ecs_entity_t e = 10000;
    ecs_make_alive(world, 10000);
    ecs_add(world, e, Tag);

    ecs_iter_t it = ecs_rule_iter(world, q);
    ecs_iter_to_json_desc_t desc = ECS_ITER_TO_JSON_INIT;
    desc.serialize_ids = false;
    desc.serialize_variables = false;
    desc.serialize_variable_ids = true;
    desc.serialize_values = false;
    desc.serialize_is_set = false;
    desc.serialize_sources = false;
    char *json = ecs_iter_to_json(world, &it, &desc);
    test_assert(json != NULL);

    char *expect = ecs_asprintf(
        "{\"ids\":[[\"Tag\"]], \"vars\":[\"Entity\"], \"results\":[{\"var_ids\":[10000]}]}",
        (uint32_t)e);
    test_str(json, expect);

    ecs_os_free(json);
    ecs_os_free(expect);

    ecs_rule_fini(q);

    ecs_fini(world);
}

void SerializeIterToJson_serialize_anonymous_tag(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t tag = 10000;
    ecs_make_alive(world, tag);
    ecs_query_t *q = ecs_query_new(world, "10000");

    ecs_entity_t e = ecs_new_entity(world, "e");
    ecs_add_id(world, e, tag);

    ecs_iter_t it = ecs_query_iter(world, q);
    ecs_iter_to_json_desc_t desc = {0};
    desc.serialize_term_ids = true;
    desc.serialize_ids = true;
    desc.serialize_entities = true;
    char *json = ecs_iter_to_json(world, &it, &desc);
    test_assert(json != NULL);

    char *expect = ecs_asprintf(
        "{\"ids\":[[\"10000\"]], \"results\":[{\"ids\":[[\"10000\"]], \"entities\":[\"e\"]}]}");
    test_str(json, expect);

    ecs_os_free(json);
    ecs_os_free(expect);

    ecs_fini(world);
}

void SerializeIterToJson_serialize_anonymous_component(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t tag = 10000;
    ecs_make_alive(world, tag);
    ecs_struct(world, {
        .entity = tag,
        .members = {
            { "value", ecs_id(ecs_i32_t) }
        }
    });

    ecs_query_t *q = ecs_query_new(world, "10000");

    ecs_entity_t e = ecs_new_entity(world, "e");
    int32_t *ptr = ecs_ensure_id(world, e, tag);
    *ptr = 10;

    ecs_iter_t it = ecs_query_iter(world, q);
    ecs_iter_to_json_desc_t desc = {0};
    desc.serialize_term_ids = true;
    desc.serialize_ids = true;
    desc.serialize_entities = true;
    desc.serialize_values = true;
    desc.serialize_type_info = true;
    char *json = ecs_iter_to_json(world, &it, &desc);
    test_assert(json != NULL);

    char *expect = ecs_asprintf(
        "{\"ids\":[[\"10000\"]], \"type_info\":{\"10000\":{\"value\":[\"int\"]}}, "
        "\"results\":[{\"ids\":[[\"10000\"]], \"entities\":[\"e\"], \"values\":[[{\"value\":10}]]}]}");
    test_str(json, expect);

    ecs_os_free(json);
    ecs_os_free(expect);

    ecs_fini(world);
}

void SerializeIterToJson_serialize_anonymous_pair(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t rel = 10000;
    ecs_make_alive(world, rel);
    ecs_entity_t tgt = 20000;
    ecs_make_alive(world, tgt);
    ecs_query_t *q = ecs_query_new(world, "(10000, 20000)");

    ecs_entity_t e = ecs_new_entity(world, "e");
    ecs_add_id(world, e, ecs_pair(rel, tgt));

    ecs_iter_t it = ecs_query_iter(world, q);
    ecs_iter_to_json_desc_t desc = {0};
    desc.serialize_term_ids = true;
    desc.serialize_ids = true;
    desc.serialize_entities = true;
    char *json = ecs_iter_to_json(world, &it, &desc);
    test_assert(json != NULL);

    char *expect = ecs_asprintf(
        "{\"ids\":[[\"10000\",\"20000\"]], \"results\":[{\"ids\":[[\"10000\",\"20000\"]], \"entities\":[\"e\"]}]}");
    test_str(json, expect);

    ecs_os_free(json);
    ecs_os_free(expect);

    ecs_fini(world);
}

void SerializeIterToJson_serialize_invalid_value(void) {
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

    ecs_entity_t ecs_id(T) = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "T"}),
        .members = {
            {"x", e}
        }
    });

    for (int i = 0; i < 10; i ++) {
        ecs_entity_t p = ecs_new_id(world);
        ecs_entity_t e = ecs_new_w_pair(world, EcsChildOf, p);
        ecs_set(world, e, T, {4});
    }

    ecs_query_t *q = ecs_query_new(world, "T");
    ecs_iter_t it = ecs_query_iter(world, q);

    ecs_log_set_level(-4);
    char *json = ecs_iter_to_json(world, &it, NULL);
    test_assert(json == NULL);

    ecs_fini(world);
}

void SerializeIterToJson_serialize_recycled_pair_id(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t rel = ecs_new_entity(world, "Rel");
    ecs_entity_t tgt = ecs_new_entity(world, "Tgt");
    ecs_delete(world, tgt);
    tgt = ecs_new_entity(world, "Tgt");

    ecs_entity_t e = ecs_new_w_pair(world, rel, tgt);
    ecs_set_name(world, e, "ent");

    ecs_query_t *q = ecs_query(world, {
        .filter.terms[0] = {
            .first.id = rel,
            .second.id = tgt
        }
    });

    ecs_iter_t it = ecs_query_iter(world, q);
    char *json = ecs_iter_to_json(world, &it, NULL);
    test_assert(json != NULL);
    test_str(json, "{\"ids\":[[\"Rel\",\"Tgt\"]], \"results\":["
        "{\"ids\":[[\"Rel\",\"Tgt\"]], \"sources\":[0], \"entities\":[\"ent\"]}"
    "]}");

    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeIterToJson_serialize_w_alert(void) {
    ecs_world_t *world = ecs_init();

    ECS_IMPORT(world, FlecsAlerts);

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_entity_t e2 = ecs_new_entity(world, "e2");

    ecs_alert(world, {
        .entity = ecs_new_entity(world, "position_without_velocity"),
        .filter.expr = "Position, !Velocity",
    });

    ecs_set(world, e1, Position, {10, 20});
    ecs_set(world, e2, Position, {30, 40});
    ecs_set(world, e1, Velocity, {1, 2});

    ecs_progress(world, 1.0); /* Evaluate alert logic */

    ecs_query_t *q = ecs_query_new(world, "Position");
    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);

    ecs_iter_to_json_desc_t desc = ECS_ITER_TO_JSON_INIT;
    desc.serialize_ids = false;
    desc.serialize_values = false;
    desc.serialize_is_set = false;
    desc.serialize_sources = false;
    char *json = ecs_iter_to_json(world, &it, &desc);
    test_assert(json != NULL);
    test_str(json, "{\"ids\":[[\"Position\"]], \"results\":["
        "{\"entities\":[\"e1\"]}, "
        "{\"entities\":[\"e2\"], \"alerts\":true}]}");

    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeIterToJson_serialize_no_this_alert_imported(void) {
    ecs_world_t *world = ecs_init();

    ECS_IMPORT(world, FlecsAlerts);

    ECS_COMPONENT(world, Position);

    ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_id(Position),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t e1 = ecs_new_entity(world, "Foo");
    ecs_set(world, e1, Position, {10, 20});

    ecs_rule_t *q = ecs_rule(world, {
        .expr = "Position($x)"
    });
    ecs_iter_t it = ecs_rule_iter(world, q);

    char *json = ecs_iter_to_json(world, &it, NULL);
    test_str(json, 
    "{"
        "\"ids\":[[\"Position\"]], "
        "\"vars\":[\"x\"], "
        "\"results\":[{"
            "\"ids\":[[\"Position\"]], "
            "\"sources\":[\"Foo\"], "
            "\"vars\":[\"Foo\"], "
            "\"values\":["
                "{\"x\":10, \"y\":20}"
            "]"
        "}]"
    "}");
    ecs_os_free(json);

    ecs_rule_fini(q);

    ecs_fini(world);
}

void SerializeIterToJson_serialize_paged_iterator(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_set(world, 0, Position, {10, 20});
    ecs_entity_t e2 = ecs_set(world, 0, Position, {10, 20});
    ecs_entity_t e3 = ecs_set(world, 0, Position, {20, 30});
    ecs_entity_t e4 = ecs_set(world, 0, Position, {30, 40});
    ecs_set(world, 0, Position, {10, 20});

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .terms = {{ .id = ecs_id(Position) }}
    });

    ecs_iter_t it = ecs_rule_iter(world, r);
    ecs_iter_t pit = ecs_page_iter(&it, 1, 3);

    test_bool(true, ecs_page_next(&pit));
    test_int(3, pit.count);
    test_uint(e2, pit.entities[0]);
    test_uint(e3, pit.entities[1]);
    test_uint(e4, pit.entities[2]);

    Position *p = ecs_field(&pit, Position, 1);
    test_int(p[0].x, 10);
    test_int(p[0].y, 20);
    test_int(p[1].x, 20);
    test_int(p[1].y, 30);
    test_int(p[2].x, 30);
    test_int(p[2].y, 40);

    test_bool(false, ecs_page_next(&pit));

    ecs_rule_fini(r);

    ecs_fini(world);
}

void SerializeIterToJson_serialize_paged_iterator_w_optional_component(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_set(world, 0, Position, {10, 20});
    ecs_entity_t e2 = ecs_set(world, 0, Position, {10, 20});
    ecs_entity_t e3 = ecs_set(world, 0, Position, {20, 30});
    ecs_entity_t e4 = ecs_set(world, 0, Position, {30, 40});
    ecs_entity_t e5 = ecs_set(world, 0, Position, {40, 50});

    ecs_set(world, e4, Velocity, {1, 2});
    ecs_set(world, e5, Velocity, {2, 3});

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .terms = {
            { .id = ecs_id(Position) },
            { .id = ecs_id(Velocity), .oper = EcsOptional }
        }
    });

    ecs_iter_t it = ecs_rule_iter(world, r);
    ecs_iter_t pit = ecs_page_iter(&it, 1, 3);

    test_bool(true, ecs_page_next(&pit));
    test_int(2, pit.count);
    test_bool(true, ecs_field_is_set(&pit, 1));
    test_bool(false, ecs_field_is_set(&pit, 2));
    test_uint(e2, pit.entities[0]);
    test_uint(e3, pit.entities[1]);

    Position *p = ecs_field(&pit, Position, 1);
    test_int(p[0].x, 10);
    test_int(p[0].y, 20);
    test_int(p[1].x, 20);
    test_int(p[1].y, 30);

    test_bool(true, ecs_page_next(&pit));
    test_int(1, pit.count);
    test_bool(true, ecs_field_is_set(&pit, 1));
    test_bool(true, ecs_field_is_set(&pit, 2));

    test_uint(e4, pit.entities[0]);
    p = ecs_field(&pit, Position, 1);
    test_int(p[0].x, 30);
    test_int(p[0].y, 40);

    Velocity *v = ecs_field(&pit, Velocity, 2);
    test_int(v[0].x, 1);
    test_int(v[0].y, 2);

    test_bool(false, ecs_page_next(&pit));

    ecs_rule_fini(r);

    ecs_fini(world);
}

void SerializeIterToJson_serialize_paged_iterator_w_optional_tag(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Tag);

    ecs_set(world, 0, Position, {10, 20});
    ecs_entity_t e2 = ecs_set(world, 0, Position, {10, 20});
    ecs_entity_t e3 = ecs_set(world, 0, Position, {20, 30});
    ecs_entity_t e4 = ecs_set(world, 0, Position, {30, 40});
    ecs_entity_t e5 = ecs_set(world, 0, Position, {40, 50});

    ecs_add(world, e4, Tag);
    ecs_add(world, e5, Tag);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .terms = {
            { .id = ecs_id(Position) },
            { .id = Tag, .oper = EcsOptional }
        }
    });

    ecs_iter_t it = ecs_rule_iter(world, r);
    ecs_iter_t pit = ecs_page_iter(&it, 1, 3);

    test_bool(true, ecs_page_next(&pit));
    test_int(2, pit.count);
    test_bool(true, ecs_field_is_set(&pit, 1));
    test_bool(false, ecs_field_is_set(&pit, 2));
    test_uint(e2, pit.entities[0]);
    test_uint(e3, pit.entities[1]);

    Position *p = ecs_field(&pit, Position, 1);
    test_int(p[0].x, 10);
    test_int(p[0].y, 20);
    test_int(p[1].x, 20);
    test_int(p[1].y, 30);

    test_bool(true, ecs_page_next(&pit));
    test_int(1, pit.count);
    test_bool(true, ecs_field_is_set(&pit, 1));
    test_bool(true, ecs_field_is_set(&pit, 2));

    test_uint(e4, pit.entities[0]);
    p = ecs_field(&pit, Position, 1);
    test_int(p[0].x, 30);
    test_int(p[0].y, 40);

    test_bool(false, ecs_page_next(&pit));

    ecs_rule_fini(r);

    ecs_fini(world);
}

void SerializeIterToJson_serialize_paged_iterator_w_vars(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Rel);
    ECS_TAG(world, ObjA);
    ECS_TAG(world, ObjB);

    ecs_new_w_pair(world, Rel, ObjA);
    ecs_entity_t e2 = ecs_new_w_pair(world, Rel, ObjA);
    ecs_entity_t e3 = ecs_new_w_pair(world, Rel, ObjA);
    ecs_entity_t e4 = ecs_new_w_pair(world, Rel, ObjB);
    ecs_new_w_pair(world, Rel, ObjB);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "(Rel, $Var)"
    });

    int32_t var = ecs_rule_find_var(r, "Var");

    ecs_iter_t it = ecs_rule_iter(world, r);
    ecs_iter_t pit = ecs_page_iter(&it, 1, 3);

    test_assert(var < pit.variable_count);
    test_str("Var", pit.variable_names[var]);

    test_bool(true, ecs_page_next(&pit));
    test_int(2, pit.count);
    test_uint(e2, pit.entities[0]);
    test_uint(e3, pit.entities[1]);
    test_uint(ObjA, ecs_iter_get_var(&pit, var));

    test_bool(true, ecs_page_next(&pit));
    test_int(1, pit.count);
    test_uint(e4, pit.entities[0]);
    test_uint(ObjB, ecs_iter_get_var(&pit, var));

    test_bool(false, ecs_page_next(&pit));

    ecs_rule_fini(r);

    ecs_fini(world);
}

void SerializeIterToJson_serialize_table(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_id(Position),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_id(Velocity),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_id(Mass),
        .members = {
            {"value", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_entity_t e2 = ecs_new_entity(world, "e2");
    ecs_entity_t e3 = ecs_new_entity(world, "e3");

    ecs_set(world, e1, Position, {10, 20});
    ecs_set(world, e2, Position, {20, 30});
    ecs_set(world, e3, Position, {30, 40});

    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);
    ecs_add(world, e2, Bar);

    ecs_set(world, e2, Velocity, {1, 1});
    ecs_set(world, e3, Mass, {100});

    ecs_filter_t *f = ecs_filter(world, {
        .terms = {
            { .id = ecs_id(Position) }
        }
    });

    ecs_iter_t it = ecs_filter_iter(world, f);

    ecs_iter_to_json_desc_t desc = {0};
    desc.serialize_table = true;
    desc.serialize_ids = true;
    desc.serialize_entities = true;
    char *json = ecs_iter_to_json(world, &it, &desc);
    test_assert(json != NULL);

    test_str(json, "{\"results\":["
        "{"
            "\"ids\":[[\"Position\"], [\"Foo\"]], "
            "\"entities\":[\"e1\"], "
            "\"values\":[[{\"x\":10, \"y\":20}], 0]"
        "}, {"
            "\"ids\":[[\"Position\"], [\"Velocity\"], [\"Foo\"], [\"Bar\"]], "
            "\"entities\":[\"e2\"], "
            "\"values\":[[{\"x\":20, \"y\":30}], [{\"x\":1, \"y\":1}], 0, 0]"
        "}, {"
            "\"ids\":[[\"Position\"], [\"Mass\"]], "
            "\"entities\":[\"e3\"], "
            "\"values\":[[{\"x\":30, \"y\":40}], [{\"value\":100}]]}]"
        "}");
    
    ecs_os_free(json);

    ecs_filter_fini(f);

    ecs_fini(world);
}

void SerializeIterToJson_serialize_table_w_id_labels(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_id(Position),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_id(Velocity),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_id(Mass),
        .members = {
            {"value", ecs_id(ecs_i32_t)}
        }
    });

    ecs_doc_set_name(world, ecs_id(Position), "position");
    ecs_doc_set_name(world, ecs_id(Velocity), "velocity");
    ecs_doc_set_name(world, ecs_id(Mass), "mass");

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_entity_t e2 = ecs_new_entity(world, "e2");
    ecs_entity_t e3 = ecs_new_entity(world, "e3");

    ecs_set(world, e1, Position, {10, 20});
    ecs_set(world, e2, Position, {20, 30});
    ecs_set(world, e3, Position, {30, 40});

    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);
    ecs_add(world, e2, Bar);

    ecs_set(world, e2, Velocity, {1, 1});
    ecs_set(world, e3, Mass, {100});

    ecs_filter_t *f = ecs_filter(world, {
        .terms = {
            { .id = ecs_id(Position) }
        }
    });

    ecs_iter_t it = ecs_filter_iter(world, f);

    ecs_iter_to_json_desc_t desc = {0};
    desc.serialize_table = true;
    desc.serialize_entities = true;
    desc.serialize_id_labels = true;
    char *json = ecs_iter_to_json(world, &it, &desc);
    test_assert(json != NULL);

    test_str(json, "{\"results\":["
        "{"
            "\"id_labels\":[[\"position\"], [\"Foo\"]], "
            "\"entities\":[\"e1\"], "
            "\"values\":[[{\"x\":10, \"y\":20}], 0]"
        "}, {"
            "\"id_labels\":[[\"position\"], [\"velocity\"], [\"Foo\"], [\"Bar\"]], "
            "\"entities\":[\"e2\"], "
            "\"values\":[[{\"x\":20, \"y\":30}], [{\"x\":1, \"y\":1}], 0, 0]"
        "}, {"
            "\"id_labels\":[[\"position\"], [\"mass\"]], "
            "\"entities\":[\"e3\"], "
            "\"values\":[[{\"x\":30, \"y\":40}], [{\"value\":100}]]}]"
        "}");
    
    ecs_os_free(json);

    ecs_filter_fini(f);

    ecs_fini(world);
}

void SerializeIterToJson_serialize_table_w_var_labels(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_id(Position),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_id(Velocity),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_id(Mass),
        .members = {
            {"value", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t parent = ecs_new_entity(world, "Parent");
    ecs_doc_set_name(world, parent, "parent");

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_entity_t e2 = ecs_new_entity(world, "e2");
    ecs_entity_t e3 = ecs_new_entity(world, "e3");

    ecs_add_pair(world, e1, EcsChildOf, parent);
    ecs_add_pair(world, e2, EcsChildOf, parent);
    ecs_add_pair(world, e3, EcsChildOf, parent);

    ecs_set(world, e1, Position, {10, 20});
    ecs_set(world, e2, Position, {20, 30});
    ecs_set(world, e3, Position, {30, 40});

    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);
    ecs_add(world, e2, Bar);

    ecs_set(world, e2, Velocity, {1, 1});
    ecs_set(world, e3, Mass, {100});

    ecs_rule_t *f = ecs_rule(world, {
        .terms = {
            { .id = ecs_id(Position) },
            { .first.id = EcsChildOf, .second.name = "p", .second.flags = EcsIsVariable }
        }
    });

    ecs_iter_t it = ecs_rule_iter(world, f);

    ecs_iter_to_json_desc_t desc = {0};
    desc.serialize_table = true;
    desc.serialize_entities = true;
    desc.serialize_variable_labels = true;
    desc.serialize_ids = true;
    char *json = ecs_iter_to_json(world, &it, &desc);
    test_assert(json != NULL);

    test_str(json, "{\"vars\":[\"p\"], \"results\":["
        "{"
            "\"ids\":[[\"Position\"], [\"Foo\"]], "
            "\"var_labels\":[\"parent\"], "
            "\"parent\":\"Parent\", "
            "\"entities\":[\"e1\"], "
            "\"values\":[[{\"x\":10, \"y\":20}], 0]"
        "}, {"
            "\"ids\":[[\"Position\"], [\"Velocity\"], [\"Foo\"], [\"Bar\"]], "
            "\"var_labels\":[\"parent\"], "
            "\"parent\":\"Parent\", "
            "\"entities\":[\"e2\"], "
            "\"values\":[[{\"x\":20, \"y\":30}], [{\"x\":1, \"y\":1}], 0, 0]"
        "}, {"
            "\"ids\":[[\"Position\"], [\"Mass\"]], "
            "\"var_labels\":[\"parent\"], "
            "\"parent\":\"Parent\", "
            "\"entities\":[\"e3\"], "
            "\"values\":[[{\"x\":30, \"y\":40}], [{\"value\":100}]]}]"
        "}");

    ecs_os_free(json);

    ecs_rule_fini(f);

    ecs_fini(world);
}

void SerializeIterToJson_serialize_table_w_private(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_id(Position),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_id(Velocity),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_id(Mass),
        .members = {
            {"value", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t parent = ecs_new_entity(world, "Parent");
    ecs_doc_set_name(world, parent, "parent");

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_entity_t e2 = ecs_new_entity(world, "e2");
    ecs_entity_t e3 = ecs_new_entity(world, "e3");

    ecs_add_pair(world, e1, EcsChildOf, parent);
    ecs_add_pair(world, e2, EcsChildOf, parent);
    ecs_add_pair(world, e3, EcsChildOf, parent);

    ecs_set(world, e1, Position, {10, 20});
    ecs_set(world, e2, Position, {20, 30});
    ecs_set(world, e3, Position, {30, 40});

    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);
    ecs_add(world, e2, Bar);

    ecs_set(world, e2, Velocity, {1, 1});
    ecs_set(world, e3, Mass, {100});

    ecs_rule_t *f = ecs_rule(world, {
        .terms = {
            { .id = ecs_id(Position) },
            { .first.id = EcsChildOf, .second.name = "p", .second.flags = EcsIsVariable }
        }
    });

    ecs_iter_t it = ecs_rule_iter(world, f);

    ecs_iter_to_json_desc_t desc = {0};
    desc.serialize_table = true;
    desc.serialize_entities = true;
    desc.serialize_variable_labels = true;
    desc.serialize_ids = true;
    desc.serialize_private = true;
    char *json = ecs_iter_to_json(world, &it, &desc);
    test_assert(json != NULL);

    test_str(json, "{\"vars\":[\"p\"], \"results\":["
        "{"
            "\"ids\":[[\"Position\"], [\"Foo\"], [\"flecs.core.Identifier\",\"flecs.core.Name\"], [\"flecs.core.ChildOf\",\"Parent\"]], "
            "\"var_labels\":[\"parent\"], "
            "\"parent\":\"Parent\", "
            "\"entities\":[\"e1\"], "
            "\"values\":[[{\"x\":10, \"y\":20}], 0, 0, 0]"
        "}, {"
            "\"ids\":[[\"Position\"], [\"Velocity\"], [\"Foo\"], [\"Bar\"], [\"flecs.core.Identifier\",\"flecs.core.Name\"], [\"flecs.core.ChildOf\",\"Parent\"]], "
            "\"var_labels\":[\"parent\"], "
            "\"parent\":\"Parent\", "
            "\"entities\":[\"e2\"], "
            "\"values\":[[{\"x\":20, \"y\":30}], [{\"x\":1, \"y\":1}], 0, 0, 0, 0]"
        "}, {"
            "\"ids\":[[\"Position\"], [\"Mass\"], [\"flecs.core.Identifier\",\"flecs.core.Name\"], [\"flecs.core.ChildOf\",\"Parent\"]], "
            "\"var_labels\":[\"parent\"], "
            "\"parent\":\"Parent\", "
            "\"entities\":[\"e3\"], "
            "\"values\":[[{\"x\":30, \"y\":40}], [{\"value\":100}], 0, 0]}]"
        "}");

    ecs_os_free(json);

    ecs_rule_fini(f);

    ecs_fini(world);
}

void SerializeIterToJson_serialize_world(void) {
    ecs_world_t *world = ecs_init();

    /* We can't test if the output for an exact value as this would change any
     * time something is added/removed to the world. */

    char *json_1 = ecs_world_to_json(world, NULL);
    test_assert(json_1 != NULL);

    char *json_2 = ecs_world_to_json(world, NULL);
    test_assert(json_2 != NULL);

    test_assert(json_1 != json_2);
    test_str(json_1, json_2);

    ecs_os_free(json_1);
    ecs_os_free(json_2);

    ecs_fini(world);
}

void SerializeIterToJson_serialize_w_offset(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_add(world, e1, Tag);

    ecs_entity_t e2 = ecs_new_entity(world, "e2");
    ecs_add(world, e2, Tag);

    ecs_rule_t *r = ecs_rule_new(world, "Tag($this), ?$this(_)");
    ecs_iter_t it = ecs_rule_iter(world, r);

    ecs_iter_to_json_desc_t desc = {0};
    desc.serialize_entities = true;
    char *json = ecs_iter_to_json(world, &it, &desc);

    test_str(json, 
        "{\"results\":[{\"entities\":[\"e1\"]}, {\"entities\":[\"e2\"]}]}");

    ecs_os_free(json);

    ecs_rule_fini(r);

    ecs_fini(world);
}

void SerializeIterToJson_serialize_labels_w_offset(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);

    ecs_entity_t e1 = ecs_new_id(world);
    ecs_add(world, e1, Tag);
    ecs_doc_set_name(world, e1, "e1");

    ecs_entity_t e2 = ecs_new_id(world);
    ecs_add(world, e2, Tag);
    ecs_doc_set_name(world, e2, "e2");

    ecs_rule_t *r = ecs_rule_new(world, "Tag($this), ?$this(_)");
    ecs_iter_t it = ecs_rule_iter(world, r);

    ecs_iter_to_json_desc_t desc = {0};
    desc.serialize_entity_labels = true;
    char *json = ecs_iter_to_json(world, &it, &desc);

    test_str(json, 
        "{\"results\":[{\"entity_labels\":[\"e1\"]}, {\"entity_labels\":[\"e2\"]}]}");

    ecs_os_free(json);

    ecs_rule_fini(r);

    ecs_fini(world);
}

void SerializeIterToJson_serialize_colors_w_offset(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);

    ecs_entity_t e1 = ecs_new_id(world);
    ecs_add(world, e1, Tag);
    ecs_doc_set_color(world, e1, "e1");

    ecs_entity_t e2 = ecs_new_id(world);
    ecs_add(world, e2, Tag);
    ecs_doc_set_color(world, e2, "e2");

    ecs_rule_t *r = ecs_rule_new(world, "Tag($this), ?$this(_)");
    ecs_iter_t it = ecs_rule_iter(world, r);

    ecs_iter_to_json_desc_t desc = {0};
    desc.serialize_colors = true;
    char *json = ecs_iter_to_json(world, &it, &desc);

    test_str(json, 
        "{\"results\":[{\"colors\":[\"e1\"]}, {\"colors\":[\"e2\"]}]}");

    ecs_os_free(json);

    ecs_rule_fini(r);

    ecs_fini(world);
}

void SerializeIterToJson_serialize_anonymous_entities_w_offset(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);

    ecs_entity_t e1 = ecs_new_id(world);
    ecs_add(world, e1, Tag);

    ecs_entity_t e2 = ecs_new_id(world);
    ecs_add(world, e2, Tag);

    ecs_rule_t *r = ecs_rule_new(world, "Tag($this), ?$this(_)");
    ecs_iter_t it = ecs_rule_iter(world, r);

    ecs_iter_to_json_desc_t desc = {0};
    desc.serialize_entities = true;
    char *json = ecs_iter_to_json(world, &it, &desc);

    char *expect = ecs_asprintf(
        "{\"results\":[{\"entities\":[%u]}, {\"entities\":[%u]}]}",
            (uint32_t)e1, (uint32_t)e2);

    test_str(json, expect);

    ecs_os_free(json);
    ecs_os_free(expect);

    ecs_rule_fini(r);

    ecs_fini(world);
}

void SerializeIterToJson_serialize_term_labels(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_doc_set_name(world, ecs_id(Position), "position");

    ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_id(Position),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    ecs_query_t *q = ecs_query_new(world, "Position");
    ecs_iter_t it = ecs_query_iter(world, q);

    ecs_iter_to_json_desc_t desc = {0};
    desc.serialize_term_labels = true;
    char *json = ecs_iter_to_json(world, &it, &desc);
    test_assert(json != NULL);
    test_str(json, "{\"id_labels\":[[\"position\"]], \"results\":[]}");

    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeIterToJson_serialize_id_labels(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_doc_set_name(world, ecs_id(Position), "position");

    ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_id(Position),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    ecs_entity_t e = ecs_new_entity(world, "ent");
    ecs_set(world, e, Position, {1, 2});

    ecs_query_t *q = ecs_query_new(world, "Position");
    ecs_iter_t it = ecs_query_iter(world, q);

    ecs_iter_to_json_desc_t desc = {0};
    desc.serialize_id_labels = true;
    char *json = ecs_iter_to_json(world, &it, &desc);
    test_assert(json != NULL);
    test_str(json, "{\"results\":[{\"id_labels\":[[\"position\"]]}]}");

    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeIterToJson_serialize_id_labels_w_str(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_doc_set_name(world, ecs_id(Position), "\"position\"");

    ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_id(Position),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    ecs_entity_t e = ecs_new_entity(world, "ent");
    ecs_set(world, e, Position, {1, 2});

    ecs_query_t *q = ecs_query_new(world, "Position");
    ecs_iter_t it = ecs_query_iter(world, q);

    ecs_iter_to_json_desc_t desc = {0};
    desc.serialize_id_labels = true;
    char *json = ecs_iter_to_json(world, &it, &desc);
    test_assert(json != NULL);
    test_str(json, "{\"results\":[{\"id_labels\":[[\"\\\"position\\\"\"]]}]}");

    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeIterToJson_serialize_vars_for_query(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_query_t *q = ecs_query(world, {
        .filter.terms[0] = {
            .id = ecs_id(Position)
        }
    });

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_set(world, e1, Position, {1, 2});
    ecs_entity_t e2 = ecs_new_entity(world, "e2");
    ecs_set(world, e2, Position, {3, 4});

    ecs_iter_to_json_desc_t desc = {0};
    desc.serialize_entities = true;
    desc.serialize_variables = true;
    ecs_iter_t it = ecs_query_iter(world, q);
    char *json = ecs_iter_to_json(world, &it, &desc);
    test_assert(json != NULL);
    test_str(json, "{\"results\":[{\"entities\":[\"e1\", \"e2\"]}]}");

    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeIterToJson_serialize_var_labels_for_query(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_query_t *q = ecs_query(world, {
        .filter.terms[0] = {
            .id = ecs_id(Position)
        }
    });

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_set(world, e1, Position, {1, 2});
    ecs_entity_t e2 = ecs_new_entity(world, "e2");
    ecs_set(world, e2, Position, {3, 4});

    ecs_iter_to_json_desc_t desc = {0};
    desc.serialize_entities = true;
    desc.serialize_variable_labels = true;
    ecs_iter_t it = ecs_query_iter(world, q);
    char *json = ecs_iter_to_json(world, &it, &desc);
    test_assert(json != NULL);
    test_str(json, "{\"results\":[{\"entities\":[\"e1\", \"e2\"]}]}");

    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeIterToJson_serialize_var_ids_for_query(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_query_t *q = ecs_query(world, {
        .filter.terms[0] = {
            .id = ecs_id(Position)
        }
    });

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_set(world, e1, Position, {1, 2});
    ecs_entity_t e2 = ecs_new_entity(world, "e2");
    ecs_set(world, e2, Position, {3, 4});

    ecs_iter_to_json_desc_t desc = {0};
    desc.serialize_entities = true;
    desc.serialize_variable_ids = true;
    ecs_iter_t it = ecs_query_iter(world, q);
    char *json = ecs_iter_to_json(world, &it, &desc);
    test_assert(json != NULL);
    test_str(json, "{\"results\":[{\"entities\":[\"e1\", \"e2\"]}]}");

    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeIterToJson_serialize_null_doc_name(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);

    ecs_entity_t e = ecs_new_entity(world, "foo");
    ecs_doc_set_name(world, e, "bar");
    ecs_doc_set_name(world, e, NULL);
    ecs_add(world, e, Tag);

    ecs_query_t *q = ecs_query(world, {
        .filter.terms[0] = {
            .id = Tag
        }
    });
    test_assert(q != NULL);

    ecs_iter_to_json_desc_t desc = {0};
    desc.serialize_entities = true;
    desc.serialize_entity_labels = true;

    ecs_iter_t it = ecs_query_iter(world, q);
    char *json = ecs_iter_to_json(world, &it, &desc);
    test_assert(json != NULL);
    test_str(json, "{\"results\":[{\"entities\":[\"foo\"]}]}");
    ecs_os_free(json);

    ecs_fini(world);
}

void SerializeIterToJson_serialize_rule_w_optional(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_entity_t e1 = ecs_new(world, Foo);
    ecs_set_name(world, e1, "e1");
    ecs_entity_t e2 = ecs_new(world, Foo);
    ecs_set_name(world, e2, "e2");
    ecs_add(world, e2, Bar);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "Foo, ?Bar"
    });

    ecs_iter_t it = ecs_rule_iter(world, r);

    char *json = ecs_iter_to_json(world, &it, NULL);
    test_str(json, 
    "{"
        "\"ids\":[[\"Foo\"], [\"Bar\"]], "
        "\"results\":[{"
            "\"ids\":[[\"Foo\"], [\"Bar\"]], "
            "\"sources\":[0, 0], "
            "\"is_set\":[true, false], "
            "\"entities\":["
                "\"e1\""
            "]"
        "}, {"
            "\"ids\":[[\"Foo\"], [\"Bar\"]], "
            "\"sources\":[0, 0], "
            "\"is_set\":[true, true], "
            "\"entities\":["
                "\"e2\""
            "]"
        "}]"
    "}");

    ecs_os_free(json);

    ecs_rule_fini(r);

    ecs_fini(world);
}

void SerializeIterToJson_serialize_rule_w_optional_component(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TAG(world, Foo);

    ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_id(Position),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_id(Velocity),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t e1 = ecs_set(world, 0, Position, {10, 20});
    ecs_set_name(world, e1, "e1");

    ecs_entity_t e2 = ecs_set(world, 0, Position, {30, 40});
    ecs_set_name(world, e2, "e2");
    ecs_set(world, e2, Velocity, {1, 2});

    ecs_entity_t e3 = ecs_set(world, 0, Position, {50, 60});
    ecs_set_name(world, e3, "e3");
    ecs_add(world, e3, Foo);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "Position, ?Velocity"
    });

    ecs_iter_t it = ecs_rule_iter(world, r);

    char *json = ecs_iter_to_json(world, &it, NULL);
    test_str(json, 
    "{"
        "\"ids\":[[\"Position\"], [\"Velocity\"]], "
        "\"results\":[{"
            "\"ids\":[[\"Position\"], [\"Velocity\"]], "
            "\"sources\":[0, 0], "
            "\"is_set\":[true, false], "
            "\"entities\":["
                "\"e1\""
            "], "
            "\"values\":[[{\"x\":10, \"y\":20}], []]"
        "}, {"
            "\"ids\":[[\"Position\"], [\"Velocity\"]], "
            "\"sources\":[0, 0], "
            "\"is_set\":[true, true], "
            "\"entities\":["
                "\"e2\""
            "], "
            "\"values\":[[{\"x\":30, \"y\":40}], [{\"x\":1, \"y\":2}]]"
        "}, {"
            "\"ids\":[[\"Position\"], [\"Velocity\"]], "
            "\"sources\":[0, 0], "
            "\"is_set\":[true, false], "
            "\"entities\":["
                "\"e3\""
            "], "
            "\"values\":[[{\"x\":50, \"y\":60}], []]"
        "}]"
    "}");

    ecs_os_free(json);

    ecs_rule_fini(r);

    ecs_fini(world);
}

void SerializeIterToJson_serialize_entity_w_flecs_core_parent(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Foo);

    ecs_entity_t e1 = ecs_new(world, Foo);
    ecs_set_name(world, e1, "e1");

    ecs_entity_t flecs_core_parent = 
        ecs_new_from_fullpath(world, "flecs.core.bob");
    ecs_add_pair(world, e1, EcsChildOf, flecs_core_parent);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "Foo"
    });

    ecs_iter_t it = ecs_rule_iter(world, r);

    char *json = ecs_iter_to_json(world, &it, NULL);
    test_str(json, 
    "{"
        "\"ids\":[[\"Foo\"]], "
        "\"results\":[{"
            "\"ids\":[[\"Foo\"]], "
            "\"sources\":[0], "
            "\"parent\":\"flecs.core.bob\", "
            "\"entities\":["
                "\"e1\""
            "]"
        "}]"
    "}");

    ecs_os_free(json);

    ecs_rule_fini(r);

    ecs_fini(world);
}
