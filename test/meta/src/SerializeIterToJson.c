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

    ecs_query_t *q = ecs_query(world, { .expr = "Position" });
    ecs_iter_t it = ecs_query_iter(world, q);

    char *json = ecs_iter_to_json(&it, NULL);
    test_assert(json != NULL);
    test_json(json, "{\"results\":[]}");

    ecs_os_free(json);

    ecs_query_fini(q);

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

    ecs_entity_t e1 = ecs_entity(world, { .name = "Foo" });
    ecs_entity_t e2 = ecs_entity(world, { .name = "Bar" });

    ecs_set(world, e1, Position, {10, 20});
    ecs_set(world, e2, Position, {30, 40});

    ecs_query_t *q = ecs_query(world, { .expr = "Position" });
    ecs_iter_t it = ecs_query_iter(world, q);

    char *json = ecs_iter_to_json(&it, NULL);
    test_json(json, "{\"results\":[{\"name\":\"Foo\", \"fields\":{\"values\":[{\"x\":10, \"y\":20}]}}, {\"name\":\"Bar\", \"fields\":{\"values\":[{\"x\":30, \"y\":40}]}}]}");

    ecs_os_free(json);

    ecs_query_fini(q);

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

    ecs_entity_t e1 = ecs_entity(world, { .name = "Foo" });
    ecs_entity_t e2 = ecs_entity(world, { .name = "Bar" });

    ecs_set(world, e1, Position, {10, 20});
    ecs_set(world, e2, Position, {30, 40});

    ecs_set(world, e1, Mass, {1});
    ecs_set(world, e2, Mass, {2});

    ecs_query_t *q = ecs_query(world, { .expr = "Position, Mass" });
    ecs_iter_t it = ecs_query_iter(world, q);

    char *json = ecs_iter_to_json(&it, NULL);
    test_json(json, "{\"results\":[{\"name\":\"Foo\", \"fields\":{\"values\":[{\"x\":10, \"y\":20}, {\"value\":1}]}}, {\"name\":\"Bar\", \"fields\":{\"values\":[{\"x\":30, \"y\":40}, {\"value\":2}]}}]}");

    ecs_os_free(json);

    ecs_query_fini(q);

    ecs_fini(world);
}

void SerializeIterToJson_serialize_1_tag_2_ents_same_table(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, MyTag);

    ecs_entity_t e1 = ecs_entity(world, { .name = "Foo" });
    ecs_entity_t e2 = ecs_entity(world, { .name = "Bar" });

    ecs_add(world, e1, MyTag);
    ecs_add(world, e2, MyTag);

    ecs_query_t *q = ecs_query(world, { .expr = "MyTag" });
    ecs_iter_t it = ecs_query_iter(world, q);

    char *json = ecs_iter_to_json(&it, NULL);
    test_json(json, "{\"results\":[{\"name\":\"Foo\", \"fields\":{}}, {\"name\":\"Bar\", \"fields\":{}}]}");

    ecs_os_free(json);

    ecs_query_fini(q);

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

    ecs_entity_t e1 = ecs_entity(world, { .name = "Foo" });
    ecs_entity_t e2 = ecs_entity(world, { .name = "Bar" });

    ecs_set(world, e1, Position, {10, 20});
    ecs_set(world, e2, Position, {30, 40});

    ecs_add(world, e1, MyTag);
    ecs_add(world, e2, MyTag);

    ecs_query_t *q = ecs_query(world, { .expr = "Position, MyTag" });
    ecs_iter_t it = ecs_query_iter(world, q);

    char *json = ecs_iter_to_json(&it, NULL);
    test_json(json, "{\"results\":[{\"name\":\"Foo\", \"fields\":{\"values\":[{\"x\":10, \"y\":20}, 0]}}, {\"name\":\"Bar\", \"fields\":{\"values\":[{\"x\":30, \"y\":40}, 0]}}]}");

    ecs_os_free(json);

    ecs_query_fini(q);

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

    ecs_entity_t e1 = ecs_entity(world, { .name = "Foo" });
    ecs_entity_t e2 = ecs_entity(world, { .name = "Bar" });
    ecs_entity_t e3 = ecs_entity(world, { .name = "Hello" });
    ecs_entity_t e4 = ecs_entity(world, { .name = "World" });

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

    ecs_query_t *q = ecs_query(world, { .expr = "Position, TagA" });
    ecs_iter_t it = ecs_query_iter(world, q);

    char *json = ecs_iter_to_json(&it, NULL);
    test_json(json, "{\"results\":[{\"name\":\"Foo\", \"fields\":{\"values\":[{\"x\":10, \"y\":20}, 0]}}, {\"name\":\"Bar\", \"fields\":{\"values\":[{\"x\":30, \"y\":40}, 0]}}, {\"name\":\"Hello\", \"fields\":{\"values\":[{\"x\":50, \"y\":60}, 0]}}, {\"name\":\"World\", \"fields\":{\"values\":[{\"x\":70, \"y\":80}, 0]}}]}");

    ecs_os_free(json);

    ecs_query_fini(q);

    ecs_fini(world);
}

void SerializeIterToJson_serialize_2_comps_1_owned_2_ents(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Mass);

    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    ecs_add_pair(world, ecs_id(Mass), EcsOnInstantiate, EcsInherit);

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

    ecs_entity_t base = ecs_entity(world, { .name = "Base" });
    ecs_entity_t e1 = ecs_entity(world, { .name = "Foo" });
    ecs_entity_t e2 = ecs_entity(world, { .name = "Bar" });

    ecs_set(world, base, Mass, {100});
    ecs_set(world, e1, Position, {10, 20});
    ecs_set(world, e2, Position, {30, 40});

    ecs_add_pair(world, e1, EcsIsA, base);
    ecs_add_pair(world, e2, EcsIsA, base);

    ecs_query_t *q = ecs_query(world, { .expr = "Position, Mass(up IsA)" });
    ecs_iter_t it = ecs_query_iter(world, q);

    char *json = ecs_iter_to_json(&it, NULL);
    test_json(json, "{\"results\":[{\"name\":\"Foo\", \"fields\":{\"sources\":[0, \"Base\"], \"values\":[{\"x\":10, \"y\":20}, {\"value\":100}]}}, {\"name\":\"Bar\", \"fields\":{\"sources\":[0, \"Base\"], \"values\":[{\"x\":30, \"y\":40}, {\"value\":100}]}}]}");

    ecs_os_free(json);

    ecs_query_fini(q);

    ecs_fini(world);
}

void SerializeIterToJson_serialize_w_pair_wildcard(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Rel);
    ECS_TAG(world, ObjA);
    ECS_TAG(world, ObjB);

    ecs_entity_t e1 = ecs_entity(world, { .name = "Foo" });
    ecs_entity_t e2 = ecs_entity(world, { .name = "Bar" });

    ecs_add_pair(world, e1, Rel, ObjA);
    ecs_add_pair(world, e2, Rel, ObjB);

    ecs_query_t *q = ecs_query(world, { .expr = "(Rel, *)" });
    ecs_iter_t it = ecs_query_iter(world, q);

    char *json = ecs_iter_to_json(&it, NULL);
    test_json(json, "{\"results\":[{\"name\":\"Foo\", \"fields\":{\"ids\":[[\"Rel\",\"ObjA\"]]}}, {\"name\":\"Bar\", \"fields\":{\"ids\":[[\"Rel\",\"ObjB\"]]}}]}");

    ecs_os_free(json);

    ecs_query_fini(q);

    ecs_fini(world);
}

void SerializeIterToJson_serialize_w_var(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Rel);
    ECS_TAG(world, ObjA);
    ECS_TAG(world, ObjB);

    ecs_entity_t e1 = ecs_entity(world, { .name = "Foo" });
    ecs_entity_t e2 = ecs_entity(world, { .name = "Bar" });

    ecs_add_pair(world, e1, Rel, ObjA);
    ecs_add_pair(world, e2, Rel, ObjB);

    ecs_query_t *r = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "(Rel, $X)"
    });

    ecs_iter_t it = ecs_query_iter(world, r);

    char *json = ecs_iter_to_json(&it, NULL);
    test_json(json, "{\"results\":[{\"name\":\"Foo\", \"vars\":{\"X\":\"ObjA\"},\"fields\":{\"ids\":[[\"Rel\",\"ObjA\"]]}}, {\"name\":\"Bar\", \"vars\":{\"X\":\"ObjB\"},\"fields\":{\"ids\":[[\"Rel\",\"ObjB\"]]}}]}");

    ecs_os_free(json);

    ecs_query_fini(r);

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

    ecs_entity_t e1 = ecs_entity(world, { .name = "Foo" });
    ecs_entity_t e2 = ecs_entity(world, { .name = "Bar" });

    ecs_add_pair(world, e1, RelX, ObjA);
    ecs_add_pair(world, e2, RelX, ObjB);

    ecs_add_pair(world, e1, RelY, ObjC);
    ecs_add_pair(world, e2, RelY, ObjD);

    ecs_query_t *r = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "(RelX, $X), (RelY, $Y)"
    });

    ecs_iter_t it = ecs_query_iter(world, r);

    char *json = ecs_iter_to_json(&it, NULL);
    test_json(json, "{\"results\":[{\"name\":\"Foo\", \"vars\":{\"X\":\"ObjA\", \"Y\":\"ObjC\"},\"fields\":{\"ids\":[[\"RelX\",\"ObjA\"], [\"RelY\",\"ObjC\"]]}}, {\"name\":\"Bar\", \"vars\":{\"X\":\"ObjB\", \"Y\":\"ObjD\"},\"fields\":{\"ids\":[[\"RelX\",\"ObjB\"], [\"RelY\",\"ObjD\"]]}}]}");

    ecs_os_free(json);

    ecs_query_fini(r);

    ecs_fini(world);
}

void SerializeIterToJson_serialize_type_info_1_tags(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);

    ecs_entity_t e1 = ecs_entity(world, { .name = "Foo" });
    ecs_entity_t e2 = ecs_entity(world, { .name = "Bar" });

    ecs_add(world, e1, TagA);
    ecs_add(world, e2, TagA);

    ecs_query_t *q = ecs_query(world, { .expr = "TagA" });
    ecs_iter_t it = ecs_query_iter(world, q);

    ecs_iter_to_json_desc_t desc = ECS_ITER_TO_JSON_INIT;
    desc.serialize_type_info = true;
    char *json = ecs_iter_to_json(&it, &desc);

    test_json(json, "{\"type_info\":{}, \"results\":[{\"name\":\"Foo\", \"fields\":{}}, {\"name\":\"Bar\", \"fields\":{}}]}");

    ecs_os_free(json);

    ecs_query_fini(q);

    ecs_fini(world);
}

void SerializeIterToJson_serialize_type_info_2_tags(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_entity_t e1 = ecs_entity(world, { .name = "Foo" });
    ecs_entity_t e2 = ecs_entity(world, { .name = "Bar" });

    ecs_add(world, e1, TagA);
    ecs_add(world, e1, TagB);
    ecs_add(world, e2, TagA);
    ecs_add(world, e2, TagB);

    ecs_query_t *q = ecs_query(world, { .expr = "TagA, TagB" });
    ecs_iter_t it = ecs_query_iter(world, q);

    ecs_iter_to_json_desc_t desc = ECS_ITER_TO_JSON_INIT;
    desc.serialize_type_info = true;
    char *json = ecs_iter_to_json(&it, &desc);

    test_json(json, "{\"type_info\":{}, \"results\":[{\"name\":\"Foo\", \"fields\":{}}, {\"name\":\"Bar\", \"fields\":{}}]}");
    ecs_os_free(json);

    ecs_query_fini(q);

    ecs_fini(world);
}

void SerializeIterToJson_serialize_type_info_1_component(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e1 = ecs_entity(world, { .name = "Foo" });
    ecs_entity_t e2 = ecs_entity(world, { .name = "Bar" });

    ecs_add(world, e1, Position);
    ecs_add(world, e2, Position);

    ecs_query_t *q = ecs_query(world, { .expr = "Position" });
    ecs_iter_t it = ecs_query_iter(world, q);

    ecs_iter_to_json_desc_t desc = ECS_ITER_TO_JSON_INIT;
    desc.serialize_type_info = true;
    char *json = ecs_iter_to_json(&it, &desc);

    test_json(json, "{\"type_info\":{\"Position\":0}, \"results\":[{\"name\":\"Foo\", \"fields\":{\"values\":[0]}}, {\"name\":\"Bar\", \"fields\":{\"values\":[0]}}]}");

    ecs_os_free(json);

    ecs_query_fini(q);

    ecs_fini(world);
}

void SerializeIterToJson_serialize_type_info_2_components(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t e1 = ecs_entity(world, { .name = "Foo" });
    ecs_entity_t e2 = ecs_entity(world, { .name = "Bar" });

    ecs_add(world, e1, Position);
    ecs_add(world, e2, Position);

    ecs_add(world, e1, Velocity);
    ecs_add(world, e2, Velocity);

    ecs_query_t *q = ecs_query(world, { .expr = "Position, Velocity" });
    ecs_iter_t it = ecs_query_iter(world, q);

    ecs_iter_to_json_desc_t desc = ECS_ITER_TO_JSON_INIT;
    desc.serialize_type_info = true;
    char *json = ecs_iter_to_json(&it, &desc);

    test_json(json, "{\"type_info\":{\"Position\":0, \"Velocity\":0}, \"results\":[{\"name\":\"Foo\", \"fields\":{\"values\":[0, 0]}}, {\"name\":\"Bar\", \"fields\":{\"values\":[0, 0]}}]}");

    ecs_os_free(json);

    ecs_query_fini(q);

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

    ecs_entity_t e1 = ecs_entity(world, { .name = "Foo" });
    ecs_entity_t e2 = ecs_entity(world, { .name = "Bar" });

    ecs_add(world, e1, Position);
    ecs_add(world, e2, Position);

    ecs_query_t *q = ecs_query(world, { .expr = "Position" });
    ecs_iter_t it = ecs_query_iter(world, q);

    ecs_iter_to_json_desc_t desc = ECS_ITER_TO_JSON_INIT;
    desc.serialize_type_info = true;
    char *json = ecs_iter_to_json(&it, &desc);

    test_json(json, "{\"type_info\":{\"Position\":{\"x\":[\"int\"], \"y\":[\"int\"]}}, \"results\":[{\"name\":\"Foo\", \"fields\":{\"values\":[{\"x\":0, \"y\":0}]}}, {\"name\":\"Bar\", \"fields\":{\"values\":[{\"x\":0, \"y\":0}]}}]}");

    ecs_os_free(json);

    ecs_query_fini(q);

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

    ecs_entity_t e1 = ecs_entity(world, { .name = "Foo" });
    ecs_entity_t e2 = ecs_entity(world, { .name = "Bar" });

    ecs_add(world, e1, Position);
    ecs_add(world, e2, Position);

    ecs_add(world, e1, Velocity);
    ecs_add(world, e2, Velocity);

    ecs_query_t *q = ecs_query(world, { .expr = "Position, Velocity" });
    ecs_iter_t it = ecs_query_iter(world, q);

    ecs_iter_to_json_desc_t desc = ECS_ITER_TO_JSON_INIT;
    desc.serialize_type_info = true;
    char *json = ecs_iter_to_json(&it, &desc);

    test_json(json, "{\"type_info\":{\"Position\":{\"x\":[\"int\"], \"y\":[\"int\"]}, \"Velocity\":0}, \"results\":[{\"name\":\"Foo\", \"fields\":{\"values\":[{\"x\":0, \"y\":0}, 0]}}, {\"name\":\"Bar\", \"fields\":{\"values\":[{\"x\":0, \"y\":0}, 0]}}]}");

    ecs_os_free(json);

    ecs_query_fini(q);

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

    ecs_entity_t e1 = ecs_entity(world, { .name = "Foo" });
    ecs_entity_t e2 = ecs_entity(world, { .name = "Bar" });

    ecs_add(world, e1, Position);
    ecs_add(world, e2, Position);

    ecs_add(world, e1, Velocity);
    ecs_add(world, e2, Velocity);

    ecs_query_t *q = ecs_query(world, { .expr = "Position, Velocity" });
    ecs_iter_t it = ecs_query_iter(world, q);

    ecs_iter_to_json_desc_t desc = ECS_ITER_TO_JSON_INIT;
    desc.serialize_type_info = true;
    char *json = ecs_iter_to_json(&it, &desc);

    test_json(json, "{\"type_info\":{\"Position\":{\"x\":[\"int\"], \"y\":[\"int\"]}, \"Velocity\":{\"x\":[\"int\"], \"y\":[\"int\"]}}, \"results\":[{\"name\":\"Foo\", \"fields\":{\"values\":[{\"x\":0, \"y\":0}, {\"x\":0, \"y\":0}]}}, {\"name\":\"Bar\", \"fields\":{\"values\":[{\"x\":0, \"y\":0}, {\"x\":0, \"y\":0}]}}]}");

    ecs_os_free(json);

    ecs_query_fini(q);

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

    ecs_entity_t e1 = ecs_entity(world, { .name = "Foo" });
    ecs_entity_t e2 = ecs_entity(world, { .name = "Bar" });

    ecs_set(world, e1, T, {24});
    ecs_set(world, e2, T, {16});

    ecs_query_t *q = ecs_query(world, { .expr = "T" });
    ecs_iter_t it = ecs_query_iter(world, q);

    ecs_iter_to_json_desc_t desc = ECS_ITER_TO_JSON_INIT;
    desc.serialize_type_info = true;
    char *json = ecs_iter_to_json(&it, &desc);

    test_json(json, "{\"type_info\":{\"T\":{\"value\":[\"int\", {\"unit\":\"celsius\", \"symbol\":\"°\"}]}}, \"results\":[{\"name\":\"Foo\", \"fields\":{\"values\":[{\"value\":24}]}}, {\"name\":\"Bar\", \"fields\":{\"values\":[{\"value\":16}]}}]}");

    ecs_os_free(json);

    ecs_query_fini(q);

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

    ecs_entity_t e1 = ecs_entity(world, { .name = "Foo" });
    ecs_entity_t e2 = ecs_entity(world, { .name = "Bar" });

    ecs_set(world, e1, T, {24});
    ecs_set(world, e2, T, {16});

    ecs_query_t *q = ecs_query(world, { .expr = "T" });
    ecs_iter_t it = ecs_query_iter(world, q);

    ecs_iter_to_json_desc_t desc = ECS_ITER_TO_JSON_INIT;
    desc.serialize_type_info = true;
    char *json = ecs_iter_to_json(&it, &desc);

    test_json(json, "{\"type_info\":{\"T\":{\"value\":[\"int\", {\"unit\":\"celsius\", \"symbol\":\"°\", \"quantity\":\"temperature\"}]}}, \"results\":[{\"name\":\"Foo\", \"fields\":{\"values\":[{\"value\":24}]}}, {\"name\":\"Bar\", \"fields\":{\"values\":[{\"value\":16}]}}]}");

    ecs_os_free(json);

    ecs_query_fini(q);

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

    ecs_entity_t e1 = ecs_entity(world, { .name = "Foo" });
    ecs_entity_t e2 = ecs_entity(world, { .name = "Bar" });

    ecs_set(world, e1, T, {24});
    ecs_set(world, e2, T, {16});

    ecs_query_t *q = ecs_query(world, { .expr = "T" });
    ecs_iter_t it = ecs_query_iter(world, q);

    ecs_iter_to_json_desc_t desc = ECS_ITER_TO_JSON_INIT;
    desc.serialize_type_info = true;
    char *json = ecs_iter_to_json(&it, &desc);

    test_json(json, "{\"type_info\":{\"T\":{\"value\":[\"int\", {\"unit\":\"meters_per_second\", \"symbol\":\"m/s\"}]}}, \"results\":[{\"name\":\"Foo\", \"fields\":{\"values\":[{\"value\":24}]}}, {\"name\":\"Bar\", \"fields\":{\"values\":[{\"value\":16}]}}]}");

    ecs_os_free(json);

    ecs_query_fini(q);

    ecs_fini(world);
}

void SerializeIterToJson_serialize_w_entity_label(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);

    ecs_entity_t e1 = ecs_entity(world, { .name = "foo_bar" });
    ecs_entity_t e2 = ecs_entity(world, { .name = "hello_world" });
    ecs_doc_set_name(world, e2, "Hello World");

    ecs_add(world, e1, Tag);
    ecs_add(world, e2, Tag);

    ecs_query_t *q = ecs_query(world, { .expr = "Tag" });
    ecs_iter_t it = ecs_query_iter(world, q);

    ecs_iter_to_json_desc_t desc = ECS_ITER_TO_JSON_INIT;
    desc.serialize_doc = true;
    char *json = ecs_iter_to_json(&it, &desc);

    test_json(json, "{\"results\":[{\"name\":\"foo_bar\", \"doc\":{\"label\":\"foo_bar\"}, \"fields\":{}}, {\"name\":\"hello_world\", \"doc\":{\"label\":\"Hello World\"}, \"fields\":{}}]}");

    ecs_os_free(json);

    ecs_query_fini(q);

    ecs_fini(world);
}

void SerializeIterToJson_serialize_w_entity_label_w_str(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);

    ecs_entity_t e1 = ecs_entity(world, { .name = "foo_bar" });
    ecs_entity_t e2 = ecs_entity(world, { .name = "hello_world" });
    ecs_doc_set_name(world, e2, "Hello \"World\"");

    ecs_add(world, e1, Tag);
    ecs_add(world, e2, Tag);

    ecs_query_t *q = ecs_query(world, { .expr = "Tag" });
    ecs_iter_t it = ecs_query_iter(world, q);

    ecs_iter_to_json_desc_t desc = ECS_ITER_TO_JSON_INIT;
    desc.serialize_doc = true;
    char *json = ecs_iter_to_json(&it, &desc);

    test_json(json, "{\"results\":[{\"name\":\"foo_bar\", \"doc\":{\"label\":\"foo_bar\"}, \"fields\":{}}, {\"name\":\"hello_world\", \"doc\":{\"label\":\"Hello \\\"World\\\"\"}, \"fields\":{}}]}");

    ecs_os_free(json);

    ecs_query_fini(q);

    ecs_fini(world);
}

void SerializeIterToJson_serialize_entity_label_w_newline(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Foo);

    ecs_entity_t e = ecs_set_name(world, 0, "e");
    ecs_doc_set_name(world, e, "foo\nbar");
    ecs_add(world, e, Foo);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Foo"
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);

    ecs_iter_to_json_desc_t desc = ECS_ITER_TO_JSON_INIT;
    desc.serialize_doc = true;
    char *json = ecs_iter_to_json(&it, &desc);
    test_json(json, "{\"results\":[{\"name\":\"e\", \"doc\":{\"label\":\"foo\\nbar\"}, \"fields\":{}}]}");

    ecs_os_free(json);

    ecs_query_fini(q);

    ecs_fini(world);
}

void SerializeIterToJson_serialize_w_var_labels(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Rel);
    ECS_TAG(world, ObjA);
    ECS_TAG(world, ObjB);

    ecs_entity_t e1 = ecs_entity(world, { .name = "Foo" });
    ecs_entity_t e2 = ecs_entity(world, { .name = "Bar" });

    ecs_add_pair(world, e1, Rel, ObjA);
    ecs_add_pair(world, e2, Rel, ObjB);

    ecs_doc_set_name(world, ObjA, "Object A");

    ecs_query_t *r = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "(Rel, $X)"
    });

    ecs_iter_t it = ecs_query_iter(world, r);

    ecs_iter_to_json_desc_t desc = ECS_ITER_TO_JSON_INIT;
    desc.serialize_doc = true;
    desc.serialize_full_paths = false;
    char *json = ecs_iter_to_json(&it, &desc);

    test_json(json, "{\"results\":[{\"name\":\"Foo\", \"doc\":{\"label\":\"Foo\"}, \"vars\":{\"X\":\"Object A\"},\"fields\":{\"ids\":[[\"Rel\",\"ObjA\"]]}}, {\"name\":\"Bar\", \"doc\":{\"label\":\"Bar\"}, \"vars\":{\"X\":\"ObjB\"},\"fields\":{\"ids\":[[\"Rel\",\"ObjB\"]]}}]}");

    ecs_os_free(json);

    ecs_query_fini(r);

    ecs_fini(world);
}

void SerializeIterToJson_serialize_color(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);

    ecs_entity_t e1 = ecs_entity(world, { .name = "foo_bar" });
    ecs_entity_t e2 = ecs_entity(world, { .name = "hello_world" });
    ecs_doc_set_color(world, e2, "#47B576");

    ecs_add(world, e1, Tag);
    ecs_add(world, e2, Tag);

    ecs_query_t *q = ecs_query(world, { .expr = "Tag" });
    ecs_iter_t it = ecs_query_iter(world, q);

    ecs_iter_to_json_desc_t desc = ECS_ITER_TO_JSON_INIT;
    desc.serialize_doc = true;
    char *json = ecs_iter_to_json(&it, &desc);

    test_json(json, "{\"results\":[{\"name\":\"foo_bar\", \"doc\":{\"label\":\"foo_bar\"}, \"fields\":{}}, {\"name\":\"hello_world\", \"doc\":{\"label\":\"hello_world\", \"color\":\"#47B576\"}, \"fields\":{}}]}");

    ecs_os_free(json);

    ecs_query_fini(q);

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

    ecs_entity_t e = ecs_entity(world, { .name = "Foo" });
    ecs_add_pair(world, e, Rel, Obj);
    ecs_set(world, Obj, T, {10});

    ecs_query_t *r = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "(Rel, $X), T($X)"
    });

    ecs_iter_t it = ecs_query_iter(world, r);

    char *json = ecs_iter_to_json(&it, NULL);

    test_json(json, "{\"results\":[{\"name\":\"Foo\", \"vars\":{\"X\":\"Obj\"},\"fields\":{\"ids\":[[\"Rel\",\"Obj\"], 0], \"sources\":[0, \"Obj\"], \"values\":[0, {\"x\":10}]}}]}");

    ecs_os_free(json);

    ecs_query_fini(r);

    ecs_fini(world);
}

void SerializeIterToJson_serialize_w_optional_tag(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_entity_t e1 = ecs_entity(world, { .name = "e1" });
    ecs_entity_t e2 = ecs_entity(world, { .name = "e2" });

    ecs_add(world, e1, TagB);
    ecs_add(world, e1, TagA);
    ecs_add(world, e2, TagA);

    ecs_query_t *q = ecs_query(world, { .expr = "TagA, ?TagB" });
    ecs_iter_t it = ecs_query_iter(world, q);

    char *json = ecs_iter_to_json(&it, NULL);

    test_json(json, "{\"results\":[{\"name\":\"e1\", \"fields\":{\"is_set\":[true, true]}}, {\"name\":\"e2\", \"fields\":{\"is_set\":[true, false]}}]}");

    ecs_os_free(json);

    ecs_query_fini(q);

    ecs_fini(world);
}

void SerializeIterToJson_serialize_w_optional_component(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t e1 = ecs_entity(world, { .name = "e1" });
    ecs_entity_t e2 = ecs_entity(world, { .name = "e2" });

    ecs_add(world, e1, Velocity);
    ecs_add(world, e1, Position);
    ecs_add(world, e2, Position);

    ecs_query_t *q = ecs_query(world, { .expr = "Position, ?Velocity" });
    ecs_iter_t it = ecs_query_iter(world, q);

    char *json = ecs_iter_to_json(&it, NULL);

    test_json(json, "{\"results\":[{\"name\":\"e1\", \"fields\":{\"is_set\":[true, true], \"values\":[0, 0]}}, {\"name\":\"e2\", \"fields\":{\"is_set\":[true, false], \"values\":[0, 0]}}]}");

    ecs_os_free(json);

    ecs_query_fini(q);

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

    ecs_entity_t e1 = ecs_entity(world, { .name = "e1" });
    ecs_entity_t e2 = ecs_entity(world, { .name = "e2" });

    ecs_set(world, e1, Velocity, {1, 2});
    ecs_set(world, e1, Position, {10, 20});
    ecs_set(world, e2, Position, {10, 20});

    ecs_query_t *q = ecs_query(world, { .expr = "Position, ?Velocity" });
    ecs_iter_t it = ecs_query_iter(world, q);

    char *json = ecs_iter_to_json(&it, NULL);

    test_json(json, "{\"results\":[{\"name\":\"e1\", \"fields\":{\"is_set\":[true, true], \"values\":[{\"x\":10, \"y\":20}, {\"x\":1, \"y\":2}]}}, {\"name\":\"e2\", \"fields\":{\"is_set\":[true, false], \"values\":[{\"x\":10, \"y\":20}, 0]}}]}");

    ecs_os_free(json);

    ecs_query_fini(q);

    ecs_fini(world);
}

void SerializeIterToJson_serialize_w_inout_filter_tag(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);

    ecs_entity_t e1 = ecs_entity(world, { .name = "e1" });

    ecs_add(world, e1, TagA);

    ecs_query_t *q = ecs_query(world, { .expr = "[none] TagA" });
    ecs_iter_t it = ecs_query_iter(world, q);

    char *json = ecs_iter_to_json(&it, NULL);

    test_json(json, "{\"results\":[{\"name\":\"e1\", \"fields\":{}}]}");

    ecs_os_free(json);

    ecs_query_fini(q);

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

    ecs_entity_t e1 = ecs_entity(world, { .name = "e1" });

    ecs_set(world, e1, Position, {10, 20});
    ecs_set(world, e1, Velocity, {1, 2});

    ecs_query_t *q = ecs_query(world, { .expr = "[none] Position, Velocity" });
    ecs_iter_t it = ecs_query_iter(world, q);

    char *json = ecs_iter_to_json(&it, NULL);

    test_json(json, "{\"results\":[{\"name\":\"e1\", \"fields\":{\"values\":[0, {\"x\":1, \"y\":2}]}}]}");

    ecs_os_free(json);

    ecs_query_fini(q);

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

    ecs_entity_t e1 = ecs_entity(world, { .name = "e1" });

    ecs_set(world, e1, Position, {10, 20});
    ecs_set(world, e1, Velocity, {1, 2});

    ecs_query_t *q = ecs_query(world, { .expr = "[none] Position, Velocity" });
    ecs_iter_t it = ecs_query_iter(world, q);

    char *json = ecs_iter_to_json(&it, NULL);

    test_json(json, "{\"results\":[{\"name\":\"e1\", \"fields\":{\"values\":[0, {\"x\":1, \"y\":2}]}}]}");

    ecs_os_free(json);

    ecs_query_fini(q);

    ecs_fini(world);
}

void SerializeIterToJson_serialize_w_inout_out_tag(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);

    ecs_entity_t e1 = ecs_entity(world, { .name = "e1" });

    ecs_add(world, e1, TagA);

    ecs_query_t *q = ecs_query(world, { .expr = "[out] TagA" });
    ecs_iter_t it = ecs_query_iter(world, q);

    char *json = ecs_iter_to_json(&it, NULL);

    test_json(json, "{\"results\":[{\"name\":\"e1\", \"fields\":{}}]}");

    ecs_os_free(json);

    ecs_query_fini(q);

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

    ecs_entity_t e1 = ecs_entity(world, { .name = "e1" });

    ecs_set(world, e1, Position, {10, 20});
    ecs_set(world, e1, Velocity, {1, 2});

    ecs_query_t *q = ecs_query(world, { .expr = "[out] Position, Velocity" });
    ecs_iter_t it = ecs_query_iter(world, q);

    char *json = ecs_iter_to_json(&it, NULL);

    test_json(json, "{\"results\":[{\"name\":\"e1\", \"fields\":{\"values\":[0, {\"x\":1, \"y\":2}]}}]}");

    ecs_os_free(json);

    ecs_query_fini(q);

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

    ecs_entity_t e1 = ecs_entity(world, { .name = "e1" });

    ecs_set(world, e1, Position, {10, 20});
    ecs_set(world, e1, Velocity, {1, 2});

    ecs_query_t *q = ecs_query(world, { .expr = "[out] Position, Velocity" });
    ecs_iter_t it = ecs_query_iter(world, q);

    char *json = ecs_iter_to_json(&it, NULL);

    test_json(json, "{\"results\":[{\"name\":\"e1\", \"fields\":{\"values\":[{\"x\":10, \"y\":20}, {\"x\":1, \"y\":2}]}}]}");

    ecs_os_free(json);

    ecs_query_fini(q);

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

    ecs_entity_t e1 = ecs_entity(world, { .name = "e1" });

    ecs_set(world, e1, Position, {10, 20});

    ecs_query_t *r = ecs_query(world, { .expr = "Position($E)" });
    ecs_iter_t it = ecs_query_iter(world, r);

    char *json = ecs_iter_to_json(&it, NULL);

    test_json(json, "{\"results\":[{\"vars\":{\"E\":\"e1\"},\"fields\":{\"sources\":[\"e1\"], \"values\":[{\"x\":10, \"y\":20}]}}]}");

    ecs_os_free(json);

    ecs_query_fini(r);

    ecs_fini(world);
}

void SerializeIterToJson_serialize_ids(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);

    ecs_query_t *q = ecs_query(world, { .expr = "Tag" });

    ecs_entity_t e = ecs_entity(world, { .name = "e" });
    ecs_add(world, e, Tag);

    ecs_iter_t it = ecs_query_iter(world, q);
    ecs_iter_to_json_desc_t desc = ECS_ITER_TO_JSON_INIT;
    desc.serialize_entity_ids = true;
    desc.serialize_values = false;
    char *json = ecs_iter_to_json(&it, &desc);
    test_assert(json != NULL);

    char *expect = flecs_asprintf(
        "{\"results\":[{\"name\":\"e\", \"id\":%u, \"fields\":{}}]}",
        (uint32_t)e);
    test_json(json, expect);

    ecs_os_free(json);
    ecs_os_free(expect);

    ecs_query_fini(q);

    ecs_fini(world);
}

void SerializeIterToJson_serialize_ids_2_entities(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);

    ecs_query_t *q = ecs_query(world, { .expr = "Tag" });

    ecs_entity_t e1 = ecs_entity(world, { .name = "e1" });
    ecs_add(world, e1, Tag);

    ecs_entity_t e2 = ecs_entity(world, { .name = "e2" });
    ecs_add(world, e2, Tag);

    ecs_iter_t it = ecs_query_iter(world, q);
    ecs_iter_to_json_desc_t desc = ECS_ITER_TO_JSON_INIT;
    desc.serialize_entity_ids = true;
    char *json = ecs_iter_to_json(&it, &desc);
    test_assert(json != NULL);

    char *expect = flecs_asprintf(
        "{\"results\":[{\"name\":\"e1\", \"id\":%u, \"fields\":{}}, {\"name\":\"e2\", \"id\":%u, \"fields\":{}}]}",
        (uint32_t)e1, (uint32_t)e2);
    test_json(json, expect);

    ecs_os_free(json);
    ecs_os_free(expect);

    ecs_query_fini(q);

    ecs_fini(world);
}

void SerializeIterToJson_serialize_anonymous(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);

    ecs_query_t *q = ecs_query(world, { .expr = "Tag" });

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
    ecs_iter_to_json_desc_t desc = ECS_ITER_TO_JSON_INIT;
    char *json = ecs_iter_to_json(&it, &desc);
    test_assert(json != NULL);

    char *expect = flecs_asprintf(
        "{\"results\":[{\"name\":\"#5000\", \"fields\":{}}, {\"name\":\"#10000\", \"fields\":{}}, {\"name\":\"#100000\", \"fields\":{}}, {\"name\":\"#1000000\", \"fields\":{}}]}");
    test_json(json, expect);

    ecs_os_free(json);
    ecs_os_free(expect);

    ecs_query_fini(q);

    ecs_fini(world);
}

void SerializeIterToJson_serialize_anonymous_ids(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);

    ecs_query_t *q = ecs_query(world, { .expr = "Tag" });

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
    ecs_iter_to_json_desc_t desc = ECS_ITER_TO_JSON_INIT;
    desc.serialize_entity_ids = true;
    char *json = ecs_iter_to_json(&it, &desc);
    test_assert(json != NULL);

    char *expect = flecs_asprintf(
        "{\"results\":[{\"name\":\"#5000\", \"id\":5000, \"fields\":{}}, {\"name\":\"#10000\", \"id\":10000, \"fields\":{}}, {\"name\":\"#100000\", \"id\":100000, \"fields\":{}}, {\"name\":\"#1000000\", \"id\":1000000, \"fields\":{}}]}");
    test_json(json, expect);

    ecs_os_free(json);
    ecs_os_free(expect);

    ecs_query_fini(q);

    ecs_fini(world);
}

void SerializeIterToJson_serialize_variable_anonymous(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);

    ecs_query_t *q = ecs_query(world, { .expr = "Tag($Entity)" });

    ecs_entity_t e = 10000;
    ecs_make_alive(world, 10000);
    ecs_add(world, e, Tag);

    ecs_iter_t it = ecs_query_iter(world, q);
    ecs_iter_to_json_desc_t desc = ECS_ITER_TO_JSON_INIT;
    desc.serialize_values = false;
    char *json = ecs_iter_to_json(&it, &desc);
    test_assert(json != NULL);

    char *expect = flecs_asprintf(
        "{\"results\":[{\"vars\":{\"Entity\":\"#10000\"},\"fields\":{\"sources\":[\"#10000\"]}}]}");
    test_json(json, expect);

    ecs_os_free(json);
    ecs_os_free(expect);

    ecs_query_fini(q);

    ecs_fini(world);
}

void SerializeIterToJson_serialize_variable_anonymous_no_full_path(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);

    ecs_query_t *q = ecs_query(world, { .expr = "Tag($Entity)" });

    ecs_entity_t e = 10000;
    ecs_make_alive(world, 10000);
    ecs_add(world, e, Tag);

    ecs_iter_t it = ecs_query_iter(world, q);
    ecs_iter_to_json_desc_t desc = ECS_ITER_TO_JSON_INIT;
    desc.serialize_values = false;
    desc.serialize_full_paths = false;
    char *json = ecs_iter_to_json(&it, &desc);
    test_assert(json != NULL);

    char *expect = flecs_asprintf(
        "{\"results\":[{\"vars\":{\"Entity\":\"#10000\"},\"fields\":{\"sources\":[\"#10000\"]}}]}");
    test_json(json, expect);

    ecs_os_free(json);
    ecs_os_free(expect);

    ecs_query_fini(q);

    ecs_fini(world);
}

void SerializeIterToJson_serialize_anonymous_tag(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t tag = 10000;
    ecs_make_alive(world, tag);
    ecs_query_t *q = ecs_query(world, { .expr = "#10000" });

    ecs_entity_t e = ecs_entity(world, { .name = "e" });
    ecs_add_id(world, e, tag);

    ecs_iter_t it = ecs_query_iter(world, q);
    ecs_iter_to_json_desc_t desc = ECS_ITER_TO_JSON_INIT;
    char *json = ecs_iter_to_json(&it, &desc);
    test_assert(json != NULL);

    char *expect = flecs_asprintf(
        "{\"results\":[{\"name\":\"e\", \"fields\":{}}]}");
    test_json(json, expect);

    ecs_os_free(json);
    ecs_os_free(expect);

    ecs_query_fini(q);

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

    ecs_query_t *q = ecs_query(world, { .expr = "#10000" });

    ecs_entity_t e = ecs_entity(world, { .name = "e" });
    int32_t *ptr = ecs_ensure_id(world, e, tag);
    *ptr = 10;

    ecs_iter_t it = ecs_query_iter(world, q);
    ecs_iter_to_json_desc_t desc = ECS_ITER_TO_JSON_INIT;
    desc.serialize_values = true;
    desc.serialize_type_info = true;
    char *json = ecs_iter_to_json(&it, &desc);
    test_assert(json != NULL);

    char *expect = flecs_asprintf(
        "{\"type_info\":{\"#10000\":{\"value\":[\"int\"]}}, \"results\":[{\"name\":\"e\", \"fields\":{\"values\":[{\"value\":10}]}}]}");
    test_json(json, expect);

    ecs_os_free(json);
    ecs_os_free(expect);

    ecs_query_fini(q);

    ecs_fini(world);
}

void SerializeIterToJson_serialize_anonymous_pair(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t rel = 10000;
    ecs_make_alive(world, rel);
    ecs_entity_t tgt = 20000;
    ecs_make_alive(world, tgt);
    ecs_query_t *q = ecs_query(world, { .expr = "(#10000, *)" });

    ecs_entity_t e = ecs_entity(world, { .name = "e" });
    ecs_add_id(world, e, ecs_pair(rel, tgt));

    ecs_iter_t it = ecs_query_iter(world, q);
    ecs_iter_to_json_desc_t desc = ECS_ITER_TO_JSON_INIT;
    char *json = ecs_iter_to_json(&it, &desc);
    test_assert(json != NULL);

    char *expect = flecs_asprintf(
        "{\"results\":[{\"name\":\"e\", \"fields\":{\"ids\":[[\"#10000\",\"#20000\"]]}}]}");
    test_json(json, expect);

    ecs_os_free(json);
    ecs_os_free(expect);

    ecs_query_fini(q);

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
        ecs_entity_t p = ecs_new(world);
        ecs_entity_t e = ecs_new_w_pair(world, EcsChildOf, p);
        ecs_set(world, e, T, {4});
    }

    ecs_query_t *q = ecs_query(world, { .expr = "T" });
    ecs_iter_t it = ecs_query_iter(world, q);

    ecs_log_set_level(-4);
    char *json = ecs_iter_to_json(&it, NULL);
    test_assert(json == NULL);

    ecs_query_fini(q);

    ecs_fini(world);
}

void SerializeIterToJson_serialize_recycled_pair_id(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t rel = ecs_entity(world, { .name = "Rel" });
    ecs_entity_t tgt = ecs_entity(world, { .name = "Tgt" });
    ecs_delete(world, tgt);
    tgt = ecs_entity(world, { .name = "Tgt" });

    ecs_entity_t e = ecs_new_w_pair(world, rel, tgt);
    ecs_set_name(world, e, "ent");

    ecs_query_t *q = ecs_query(world, {
        .terms[0] = {
            .first.id = rel,
            .second.id = EcsWildcard
        }
    });

    ecs_iter_t it = ecs_query_iter(world, q);
    char *json = ecs_iter_to_json(&it, NULL);
    test_assert(json != NULL);
    test_json(json, "{\"results\":[{\"name\":\"ent\", \"fields\":{\"ids\":[[\"Rel\",\"Tgt\"]]}}]}");

    ecs_os_free(json);

    ecs_query_fini(q);

    ecs_fini(world);
}

void SerializeIterToJson_serialize_w_alert(void) {
    ecs_world_t *world = ecs_init();

    ECS_IMPORT(world, FlecsAlerts);

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t e1 = ecs_entity(world, { .name = "e1" });
    ecs_entity_t e2 = ecs_entity(world, { .name = "e2" });

    ecs_alert(world, {
        .entity = ecs_entity(world, { .name = "position_without_velocity" }),
        .query.expr = "Position, !Velocity",
    });

    ecs_set(world, e1, Position, {10, 20});
    ecs_set(world, e2, Position, {30, 40});
    ecs_set(world, e1, Velocity, {1, 2});

    ecs_progress(world, 1.0); /* Evaluate alert logic */

    ecs_query_t *q = ecs_query(world, { .expr = "Position" });
    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);

    ecs_iter_to_json_desc_t desc = ECS_ITER_TO_JSON_INIT;
    desc.serialize_values = false;
    char *json = ecs_iter_to_json(&it, &desc);
    test_assert(json != NULL);
    test_json(json, "{\"results\":[{\"name\":\"e1\", \"fields\":{}}, {\"name\":\"e2\", \"alerts\":true, \"fields\":{}}]}");

    ecs_os_free(json);

    ecs_query_fini(q);

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

    ecs_entity_t e1 = ecs_entity(world, { .name = "Foo" });
    ecs_set(world, e1, Position, {10, 20});

    ecs_query_t *q = ecs_query(world, {
        .expr = "Position($x)"
    });
    ecs_iter_t it = ecs_query_iter(world, q);

    char *json = ecs_iter_to_json(&it, NULL);
    test_json(json, "{\"results\":[{\"vars\":{\"x\":\"Foo\"},\"fields\":{\"sources\":[\"Foo\"], \"values\":[{\"x\":10, \"y\":20}]}}]}");
    ecs_os_free(json);

    ecs_query_fini(q);

    ecs_fini(world);
}

void SerializeIterToJson_serialize_paged_iterator(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e3 = ecs_insert(world, ecs_value(Position, {20, 30}));
    ecs_entity_t e4 = ecs_insert(world, ecs_value(Position, {30, 40}));
    ecs_insert(world, ecs_value(Position, {10, 20}));

    ecs_query_t *r = ecs_query_init(world, &(ecs_query_desc_t){
        .terms = {{ .id = ecs_id(Position) }}
    });

    ecs_iter_t it = ecs_query_iter(world, r);
    ecs_iter_t pit = ecs_page_iter(&it, 1, 3);

    test_bool(true, ecs_page_next(&pit));
    test_int(3, pit.count);
    test_uint(e2, pit.entities[0]);
    test_uint(e3, pit.entities[1]);
    test_uint(e4, pit.entities[2]);

    Position *p = ecs_field(&pit, Position, 0);
    test_int(p[0].x, 10);
    test_int(p[0].y, 20);
    test_int(p[1].x, 20);
    test_int(p[1].y, 30);
    test_int(p[2].x, 30);
    test_int(p[2].y, 40);

    test_bool(false, ecs_page_next(&pit));

    ecs_query_fini(r);

    ecs_fini(world);
}

void SerializeIterToJson_serialize_paged_iterator_w_optional_component(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e3 = ecs_insert(world, ecs_value(Position, {20, 30}));
    ecs_entity_t e4 = ecs_insert(world, ecs_value(Position, {30, 40}));
    ecs_entity_t e5 = ecs_insert(world, ecs_value(Position, {40, 50}));

    ecs_set(world, e4, Velocity, {1, 2});
    ecs_set(world, e5, Velocity, {2, 3});

    ecs_query_t *r = ecs_query_init(world, &(ecs_query_desc_t){
        .terms = {
            { .id = ecs_id(Position) },
            { .id = ecs_id(Velocity), .oper = EcsOptional }
        }
    });

    ecs_iter_t it = ecs_query_iter(world, r);
    ecs_iter_t pit = ecs_page_iter(&it, 1, 3);

    test_bool(true, ecs_page_next(&pit));
    test_int(2, pit.count);
    test_bool(true, ecs_field_is_set(&pit, 0));
    test_bool(false, ecs_field_is_set(&pit, 1));
    test_uint(e2, pit.entities[0]);
    test_uint(e3, pit.entities[1]);

    Position *p = ecs_field(&pit, Position, 0);
    test_int(p[0].x, 10);
    test_int(p[0].y, 20);
    test_int(p[1].x, 20);
    test_int(p[1].y, 30);

    test_bool(true, ecs_page_next(&pit));
    test_int(1, pit.count);
    test_bool(true, ecs_field_is_set(&pit, 0));
    test_bool(true, ecs_field_is_set(&pit, 1));

    test_uint(e4, pit.entities[0]);
    p = ecs_field(&pit, Position, 0);
    test_int(p[0].x, 30);
    test_int(p[0].y, 40);

    Velocity *v = ecs_field(&pit, Velocity, 1);
    test_int(v[0].x, 1);
    test_int(v[0].y, 2);

    test_bool(false, ecs_page_next(&pit));

    ecs_query_fini(r);

    ecs_fini(world);
}

void SerializeIterToJson_serialize_paged_iterator_w_optional_tag(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Tag);

    ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e3 = ecs_insert(world, ecs_value(Position, {20, 30}));
    ecs_entity_t e4 = ecs_insert(world, ecs_value(Position, {30, 40}));
    ecs_entity_t e5 = ecs_insert(world, ecs_value(Position, {40, 50}));

    ecs_add(world, e4, Tag);
    ecs_add(world, e5, Tag);

    ecs_query_t *r = ecs_query_init(world, &(ecs_query_desc_t){
        .terms = {
            { .id = ecs_id(Position) },
            { .id = Tag, .oper = EcsOptional }
        }
    });

    ecs_iter_t it = ecs_query_iter(world, r);
    ecs_iter_t pit = ecs_page_iter(&it, 1, 3);

    test_bool(true, ecs_page_next(&pit));
    test_int(2, pit.count);
    test_bool(true, ecs_field_is_set(&pit, 0));
    test_bool(false, ecs_field_is_set(&pit, 1));
    test_uint(e2, pit.entities[0]);
    test_uint(e3, pit.entities[1]);

    Position *p = ecs_field(&pit, Position, 0);
    test_int(p[0].x, 10);
    test_int(p[0].y, 20);
    test_int(p[1].x, 20);
    test_int(p[1].y, 30);

    test_bool(true, ecs_page_next(&pit));
    test_int(1, pit.count);
    test_bool(true, ecs_field_is_set(&pit, 0));
    test_bool(true, ecs_field_is_set(&pit, 1));

    test_uint(e4, pit.entities[0]);
    p = ecs_field(&pit, Position, 0);
    test_int(p[0].x, 30);
    test_int(p[0].y, 40);

    test_bool(false, ecs_page_next(&pit));

    ecs_query_fini(r);

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

    ecs_query_t *r = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "(Rel, $Var)"
    });

    int32_t var = ecs_query_find_var(r, "Var");

    ecs_iter_t it = ecs_query_iter(world, r);
    ecs_iter_t pit = ecs_page_iter(&it, 1, 3);

    test_assert(var < pit.variable_count);
    test_json("Var", pit.variable_names[var]);

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

    ecs_query_fini(r);

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

    ecs_entity_t e1 = ecs_entity(world, { .name = "e1" });
    ecs_entity_t e2 = ecs_entity(world, { .name = "e2" });
    ecs_entity_t e3 = ecs_entity(world, { .name = "e3" });

    ecs_set(world, e1, Position, {10, 20});
    ecs_set(world, e2, Position, {20, 30});
    ecs_set(world, e3, Position, {30, 40});

    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);
    ecs_add(world, e2, Bar);

    ecs_set(world, e2, Velocity, {1, 1});
    ecs_set(world, e3, Mass, {100});

    ecs_query_t *f = ecs_query(world, {
        .terms = {
            { .id = ecs_id(Position) }
        }
    });

    ecs_iter_t it = ecs_query_iter(world, f);

    ecs_iter_to_json_desc_t desc = ECS_ITER_TO_JSON_INIT;
    desc.serialize_table = true;
    char *json = ecs_iter_to_json(&it, &desc);
    test_assert(json != NULL);

    test_json(json, "{\"results\":[{\"name\":\"e1\", \"tags\":[\"Foo\"], \"components\":{\"Position\":{\"x\":10, \"y\":20}}}, {\"name\":\"e2\", \"tags\":[\"Foo\", \"Bar\"], \"components\":{\"Position\":{\"x\":20, \"y\":30}, \"Velocity\":{\"x\":1, \"y\":1}}}, {\"name\":\"e3\", \"components\":{\"Position\":{\"x\":30, \"y\":40}, \"Mass\":{\"value\":100}}}]}");
    
    ecs_os_free(json);

    ecs_query_fini(f);

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

    ecs_entity_t e1 = ecs_entity(world, { .name = "e1" });
    ecs_entity_t e2 = ecs_entity(world, { .name = "e2" });
    ecs_entity_t e3 = ecs_entity(world, { .name = "e3" });

    ecs_set(world, e1, Position, {10, 20});
    ecs_set(world, e2, Position, {20, 30});
    ecs_set(world, e3, Position, {30, 40});

    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);
    ecs_add(world, e2, Bar);

    ecs_set(world, e2, Velocity, {1, 1});
    ecs_set(world, e3, Mass, {100});

    ecs_query_t *f = ecs_query(world, {
        .terms = {
            { .id = ecs_id(Position) }
        }
    });

    ecs_iter_t it = ecs_query_iter(world, f);

    ecs_iter_to_json_desc_t desc = ECS_ITER_TO_JSON_INIT;
    desc.serialize_table = true;
    desc.serialize_full_paths = false;
    char *json = ecs_iter_to_json(&it, &desc);
    test_assert(json != NULL);

    test_json(json, "{\"results\":[{\"name\":\"e1\", \"tags\":[\"Foo\"], \"components\":{\"position\":{\"x\":10, \"y\":20}}}, {\"name\":\"e2\", \"tags\":[\"Foo\", \"Bar\"], \"components\":{\"position\":{\"x\":20, \"y\":30}, \"velocity\":{\"x\":1, \"y\":1}}}, {\"name\":\"e3\", \"components\":{\"position\":{\"x\":30, \"y\":40}, \"mass\":{\"value\":100}}}]}");
    
    ecs_os_free(json);

    ecs_query_fini(f);

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

    ecs_entity_t parent = ecs_entity(world, { .name = "Parent" });
    ecs_doc_set_name(world, parent, "parent");

    ecs_entity_t e1 = ecs_entity(world, { .name = "e1" });
    ecs_entity_t e2 = ecs_entity(world, { .name = "e2" });
    ecs_entity_t e3 = ecs_entity(world, { .name = "e3" });

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

    ecs_query_t *f = ecs_query(world, {
        .terms = {
            { .id = ecs_id(Position) },
            { .first.id = EcsChildOf, .second.name = "p", .second.id = EcsIsVariable }
        }
    });

    ecs_iter_t it = ecs_query_iter(world, f);

    ecs_iter_to_json_desc_t desc = ECS_ITER_TO_JSON_INIT;
    desc.serialize_table = true;
    desc.serialize_full_paths = false;
    char *json = ecs_iter_to_json(&it, &desc);
    test_assert(json != NULL);

    test_json(json, "{\"results\":[{\"parent\":\"Parent\", \"name\":\"e1\", \"tags\":[\"Foo\"],\"vars\":{\"p\":\"parent\"}, \"components\":{\"Position\":{\"x\":10, \"y\":20}}}, {\"parent\":\"Parent\", \"name\":\"e2\", \"tags\":[\"Foo\", \"Bar\"],\"vars\":{\"p\":\"parent\"}, \"components\":{\"Position\":{\"x\":20, \"y\":30}, \"Velocity\":{\"x\":1, \"y\":1}}}, {\"parent\":\"Parent\", \"name\":\"e3\", \"vars\":{\"p\":\"parent\"}, \"components\":{\"Position\":{\"x\":30, \"y\":40}, \"Mass\":{\"value\":100}}}]}");

    ecs_os_free(json);

    ecs_query_fini(f);

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
    test_json(json_1, json_2);

    ecs_os_free(json_1);
    ecs_os_free(json_2);

    ecs_fini(world);
}

void SerializeIterToJson_serialize_w_offset(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);

    ecs_entity_t e1 = ecs_entity(world, { .name = "e1" });
    ecs_add(world, e1, Tag);

    ecs_entity_t e2 = ecs_entity(world, { .name = "e2" });
    ecs_add(world, e2, Tag);

    ecs_query_t *r = ecs_query(world, { .expr = "Tag($this), ?$this(_)" });
    ecs_iter_t it = ecs_query_iter(world, r);

    ecs_iter_to_json_desc_t desc = ECS_ITER_TO_JSON_INIT;
    char *json = ecs_iter_to_json(&it, &desc);

    test_json(json, 
        "{\"results\":[{\"name\":\"e1\", \"fields\":{\"is_set\":[true, false], \"ids\":[0, 0]}}, {\"name\":\"e2\", \"fields\":{\"is_set\":[true, false], \"ids\":[0, 0]}}]}");

    ecs_os_free(json);

    ecs_query_fini(r);

    ecs_fini(world);
}

void SerializeIterToJson_serialize_labels_w_offset(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);

    ecs_entity_t e1 = ecs_new(world);
    ecs_add(world, e1, Tag);
    ecs_doc_set_name(world, e1, "e1");

    ecs_entity_t e2 = ecs_new(world);
    ecs_add(world, e2, Tag);
    ecs_doc_set_name(world, e2, "e2");

    ecs_query_t *r = ecs_query(world, { .expr = "Tag($this), ?$this(_)" });
    ecs_iter_t it = ecs_query_iter(world, r);

    ecs_iter_to_json_desc_t desc = ECS_ITER_TO_JSON_INIT;
    desc.serialize_doc = true;
    char *json = ecs_iter_to_json(&it, &desc);

    char *expect = flecs_asprintf(
        "{\"results\":[{\"name\":\"#%u\", \"doc\":{\"label\":\"e1\"}, \"fields\":{\"is_set\":[true, false], \"ids\":[0, 0]}}, {\"name\":\"#%u\", \"doc\":{\"label\":\"e2\"}, \"fields\":{\"is_set\":[true, false], \"ids\":[0, 0]}}]}",
        (uint32_t)e1, (uint32_t)e2);

    test_json(json, expect);

    ecs_os_free(json);
    ecs_os_free(expect);

    ecs_query_fini(r);

    ecs_fini(world);
}

void SerializeIterToJson_serialize_colors_w_offset(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);

    ecs_entity_t e1 = ecs_new(world);
    ecs_add(world, e1, Tag);
    ecs_doc_set_color(world, e1, "e1");

    ecs_entity_t e2 = ecs_new(world);
    ecs_add(world, e2, Tag);
    ecs_doc_set_color(world, e2, "e2");

    ecs_query_t *r = ecs_query(world, { .expr = "Tag($this), ?$this(_)" });
    ecs_iter_t it = ecs_query_iter(world, r);

    ecs_iter_to_json_desc_t desc = ECS_ITER_TO_JSON_INIT;
    desc.serialize_doc = true;
    char *json = ecs_iter_to_json(&it, &desc);

    char *expect = flecs_asprintf(
        "{\"results\":[{\"name\":\"#%u\", \"doc\":{\"label\":\"#%u\", \"color\":\"e1\"}, \"fields\":{\"is_set\":[true, false], \"ids\":[0, 0]}}, {\"name\":\"#%u\", \"doc\":{\"label\":\"#%u\", \"color\":\"e2\"}, \"fields\":{\"is_set\":[true, false], \"ids\":[0, 0]}}]}",
        (uint32_t)e1, (uint32_t)e1, (uint32_t)e2, (uint32_t)e2);

    test_json(json, expect);

    ecs_os_free(json);
    ecs_os_free(expect);

    ecs_query_fini(r);

    ecs_fini(world);
}

void SerializeIterToJson_serialize_anonymous_entities_w_offset(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);

    ecs_entity_t e1 = ecs_new(world);
    ecs_add(world, e1, Tag);

    ecs_entity_t e2 = ecs_new(world);
    ecs_add(world, e2, Tag);

    ecs_query_t *r = ecs_query(world, { .expr = "Tag($this), ?$this(_)" });
    ecs_iter_t it = ecs_query_iter(world, r);

    ecs_iter_to_json_desc_t desc = ECS_ITER_TO_JSON_INIT;
    char *json = ecs_iter_to_json(&it, &desc);

    char *expect = flecs_asprintf(
        "{\"results\":[{\"name\":\"#%u\", \"fields\":{\"is_set\":[true, false], \"ids\":[0, 0]}}, {\"name\":\"#%u\", \"fields\":{\"is_set\":[true, false], \"ids\":[0, 0]}}]}",
            (uint32_t)e1, (uint32_t)e2);

    test_json(json, expect);

    ecs_os_free(json);
    ecs_os_free(expect);

    ecs_query_fini(r);

    ecs_fini(world);
}

void SerializeIterToJson_serialize_vars_for_query(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Foo);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Foo, (ChildOf, $p)"
    });

    ecs_entity_t p = ecs_entity(world, { .name = "Parent" });
    ecs_entity_t e1 = ecs_entity(world, { .name = "e1", .parent = p });
    ecs_entity_t e2 = ecs_entity(world, { .name = "e2", .parent = p });
    ecs_add_id(world, e1, Foo);
    ecs_add_id(world, e2, Foo);

    ecs_iter_to_json_desc_t desc = ECS_ITER_TO_JSON_INIT;
    ecs_iter_t it = ecs_query_iter(world, q);
    char *json = ecs_iter_to_json(&it, &desc);
    test_assert(json != NULL);
    test_json(json, "{\"results\":[{\"parent\":\"Parent\", \"name\":\"e1\", \"vars\":{\"p\":\"Parent\"},\"fields\":{\"ids\":[0, [\"flecs.core.ChildOf\",\"Parent\"]]}}, {\"parent\":\"Parent\", \"name\":\"e2\", \"vars\":{\"p\":\"Parent\"},\"fields\":{\"ids\":[0, [\"flecs.core.ChildOf\",\"Parent\"]]}}]}");

    ecs_os_free(json);

    ecs_query_fini(q);

    ecs_fini(world);
}

void SerializeIterToJson_serialize_var_labels_for_query(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Foo);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Foo, (ChildOf, $p)"
    });

    ecs_entity_t p = ecs_entity(world, { .name = "Parent" });
    ecs_doc_set_name(world, p, "parent");
    ecs_entity_t e1 = ecs_entity(world, { .name = "e1", .parent = p });
    ecs_entity_t e2 = ecs_entity(world, { .name = "e2", .parent = p });
    ecs_add_id(world, e1, Foo);
    ecs_add_id(world, e2, Foo);

    ecs_iter_to_json_desc_t desc = ECS_ITER_TO_JSON_INIT;
    desc.serialize_full_paths = false;
    ecs_iter_t it = ecs_query_iter(world, q);
    char *json = ecs_iter_to_json(&it, &desc);
    test_assert(json != NULL);
    test_json(json, "{\"results\":[{\"parent\":\"Parent\", \"name\":\"e1\", \"vars\":{\"p\":\"parent\"},\"fields\":{\"ids\":[0, [\"flecs.core.ChildOf\",\"Parent\"]]}}, {\"parent\":\"Parent\", \"name\":\"e2\", \"vars\":{\"p\":\"parent\"},\"fields\":{\"ids\":[0, [\"flecs.core.ChildOf\",\"Parent\"]]}}]}");

    ecs_os_free(json);

    ecs_query_fini(q);

    ecs_fini(world);
}

void SerializeIterToJson_serialize_null_doc_name(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);

    ecs_entity_t e = ecs_entity(world, { .name = "foo" });
    ecs_doc_set_name(world, e, "bar");
    ecs_doc_set_name(world, e, NULL);
    ecs_add(world, e, Tag);

    ecs_query_t *q = ecs_query(world, {
        .terms[0] = {
            .id = Tag
        }
    });
    test_assert(q != NULL);

    ecs_iter_to_json_desc_t desc = ECS_ITER_TO_JSON_INIT;
    desc.serialize_doc = true;

    ecs_iter_t it = ecs_query_iter(world, q);
    char *json = ecs_iter_to_json(&it, &desc);
    test_assert(json != NULL);
    test_json(json, "{\"results\":[{\"name\":\"foo\", \"doc\":{\"label\":\"foo\"}, \"fields\":{}}]}");
    ecs_os_free(json);

    ecs_query_fini(q);

    ecs_fini(world);
}

void SerializeIterToJson_serialize_rule_w_optional(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_entity_t e1 = ecs_new_w(world, Foo);
    ecs_set_name(world, e1, "e1");
    ecs_entity_t e2 = ecs_new_w(world, Foo);
    ecs_set_name(world, e2, "e2");
    ecs_add(world, e2, Bar);

    ecs_query_t *r = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "Foo, ?Bar"
    });

    ecs_iter_t it = ecs_query_iter(world, r);

    char *json = ecs_iter_to_json(&it, NULL);
    test_json(json, "{\"results\":[{\"name\":\"e1\", \"fields\":{\"is_set\":[true, false]}}, {\"name\":\"e2\", \"fields\":{\"is_set\":[true, true]}}]}");

    ecs_os_free(json);

    ecs_query_fini(r);

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

    ecs_entity_t e1 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_set_name(world, e1, "e1");

    ecs_entity_t e2 = ecs_insert(world, ecs_value(Position, {30, 40}));
    ecs_set_name(world, e2, "e2");
    ecs_set(world, e2, Velocity, {1, 2});

    ecs_entity_t e3 = ecs_insert(world, ecs_value(Position, {50, 60}));
    ecs_set_name(world, e3, "e3");
    ecs_add(world, e3, Foo);

    ecs_query_t *r = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = "Position, ?Velocity"
    });

    ecs_iter_t it = ecs_query_iter(world, r);

    char *json = ecs_iter_to_json(&it, NULL);
    test_json(json, "{\"results\":[{\"name\":\"e1\", \"fields\":{\"is_set\":[true, false], \"values\":[{\"x\":10, \"y\":20}, 0]}}, {\"name\":\"e2\", \"fields\":{\"is_set\":[true, true], \"values\":[{\"x\":30, \"y\":40}, {\"x\":1, \"y\":2}]}}, {\"name\":\"e3\", \"fields\":{\"is_set\":[true, false], \"values\":[{\"x\":50, \"y\":60}, 0]}}]}");

    ecs_os_free(json);

    ecs_query_fini(r);

    ecs_fini(world);
}

void SerializeIterToJson_serialize_entity_w_flecs_core_parent(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Foo);

    ecs_entity_t e1 = ecs_new_w(world, Foo);
    ecs_set_name(world, e1, "e1");

    ecs_entity_t flecs_core_parent = 
        ecs_entity(world, { .name = "flecs.core.bob" });
    ecs_add_pair(world, e1, EcsChildOf, flecs_core_parent);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Foo"
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);

    char *json = ecs_iter_to_json(&it, NULL);
    test_json(json, "{\"results\":[{\"parent\":\"flecs.core.bob\", \"name\":\"e1\", \"fields\":{}}]}");

    ecs_os_free(json);

    ecs_query_fini(q);

    ecs_fini(world);
}

void SerializeIterToJson_no_fields(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_entity(world, { .name = "e1" });
    ecs_set(world, e, Position, {10, 20});

    ecs_query_t *q = ecs_query(world, { .expr = "Position" });
    ecs_iter_t it = ecs_query_iter(world, q);

    ecs_iter_to_json_desc_t desc = ECS_ITER_TO_JSON_INIT;
    desc.serialize_fields = false;
    char *json = ecs_iter_to_json(&it, &desc);
    test_assert(json != NULL);
    test_json(json, "{\"results\":[{\"name\":\"e1\"}]}");

    ecs_os_free(json);

    ecs_query_fini(q);

    ecs_fini(world);
}

void SerializeIterToJson_no_fields_w_vars(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t p = ecs_entity(world, { .name = "parent" });
    ecs_entity_t e = ecs_entity(world, { .name = "e1", .parent = p });
    ecs_set(world, e, Position, {10, 20});

    ecs_query_t *q = ecs_query(world, { .expr = "Position, (ChildOf, $p)" });
    ecs_iter_t it = ecs_query_iter(world, q);

    ecs_iter_to_json_desc_t desc = ECS_ITER_TO_JSON_INIT;
    desc.serialize_fields = false;
    char *json = ecs_iter_to_json(&it, &desc);
    test_assert(json != NULL);
    test_json(json, "{\"results\":[{\"parent\":\"parent\", \"name\":\"e1\", \"vars\":{\"p\":\"parent\"}}]}");

    ecs_os_free(json);

    ecs_query_fini(q);

    ecs_fini(world);
}

void SerializeIterToJson_serialize_from_stage(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, MyTag);

    ecs_entity_t e1 = ecs_entity(world, { .name = "Foo" });
    ecs_entity_t e2 = ecs_entity(world, { .name = "Bar" });

    ecs_add(world, e1, MyTag);
    ecs_add(world, e2, MyTag);

    ecs_query_t *q = ecs_query(world, { .expr = "MyTag" });

    ecs_world_t *stage = ecs_get_stage(world, 0);
    ecs_iter_t it = ecs_query_iter(stage, q);

    char *json = ecs_iter_to_json(&it, NULL);
    test_json(json, "{\"results\":[{\"name\":\"Foo\", \"fields\":{}}, {\"name\":\"Bar\", \"fields\":{}}]}");

    ecs_os_free(json);

    ecs_query_fini(q);

    ecs_fini(world);
}

void SerializeIterToJson_serialize_sparse(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_id(Position),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    ecs_add_id(world, ecs_id(Position), EcsSparse);

    ecs_entity_t e1 = ecs_entity(world, { .name = "Foo" });
    ecs_entity_t e2 = ecs_entity(world, { .name = "Bar" });

    ecs_set(world, e1, Position, {10, 20});
    ecs_set(world, e2, Position, {30, 40});

    ecs_query_t *q = ecs_query(world, { .expr = "Position" });
    ecs_iter_t it = ecs_query_iter(world, q);

    char *json = ecs_iter_to_json(&it, NULL);
    test_json(json, "{\"results\":[{\"name\":\"Foo\", \"fields\":{\"values\":[{\"x\":10, \"y\":20}]}}, {\"name\":\"Bar\", \"fields\":{\"values\":[{\"x\":30, \"y\":40}]}}]}");

    ecs_os_free(json);

    ecs_query_fini(q);

    ecs_fini(world);
}
