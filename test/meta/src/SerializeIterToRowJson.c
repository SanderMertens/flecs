#include <meta.h>

void SerializeIterToRowJson_serialize_this_w_1_tag(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_rule_t *q = ecs_rule(world, {
        .expr = "TagA"
    });

    test_assert(q != NULL);

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_entity_t e2 = ecs_new_entity(world, "e2");
    ecs_entity_t e3 = ecs_new_entity(world, "e3");

    ecs_add(world, e1, TagA);
    ecs_add(world, e2, TagA);
    ecs_add(world, e3, TagA);
    ecs_add(world, e3, TagB);

    ecs_iter_t it = ecs_rule_iter(world, q);
    char *json = ecs_iter_to_json(world, &it, &(ecs_iter_to_json_desc_t) {
        .serialize_rows = true
    });
    test_assert(json != NULL);

    char* expect = "{\"results\":["
        "{\"name\":\"e1\", \"tags\":[\"TagA\"]}, "
        "{\"name\":\"e2\", \"tags\":[\"TagA\"]}, "
        "{\"name\":\"e3\", \"tags\":[\"TagA\"]}]}";
    test_str(json, expect);

    ecs_os_free(json);

    ecs_rule_fini(q);

    ecs_fini(world);
}

void SerializeIterToRowJson_serialize_this_w_1_tag_w_parent(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_rule_t *q = ecs_rule(world, {
        .expr = "TagA"
    });

    test_assert(q != NULL);

    ecs_entity_t p1 = ecs_new_entity(world, "p1");
    ecs_entity_t p2 = ecs_new_entity(world, "p2");

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_entity_t e2 = ecs_new_entity(world, "e2");
    ecs_entity_t e3 = ecs_new_entity(world, "e3");
    
    ecs_add_pair(world, e1, EcsChildOf, p1);
    ecs_add_pair(world, e2, EcsChildOf, p1);
    ecs_add_pair(world, e3, EcsChildOf, p2);

    ecs_add(world, e1, TagA);
    ecs_add(world, e2, TagA);
    ecs_add(world, e3, TagA);
    ecs_add(world, e3, TagB);

    ecs_iter_t it = ecs_rule_iter(world, q);
    char *json = ecs_iter_to_json(world, &it, &(ecs_iter_to_json_desc_t) {
        .serialize_rows = true
    });
    test_assert(json != NULL);

    char* expect = "{\"results\":["
        "{\"parent\":\"p1\", \"name\":\"e1\", \"tags\":[\"TagA\"]}, "
        "{\"parent\":\"p1\", \"name\":\"e2\", \"tags\":[\"TagA\"]}, "
        "{\"parent\":\"p2\", \"name\":\"e3\", \"tags\":[\"TagA\"]}]}";
    test_str(json, expect);

    ecs_os_free(json);

    ecs_rule_fini(q);

    ecs_fini(world);
}

void SerializeIterToRowJson_serialize_this_w_1_tag_no_name(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_rule_t *q = ecs_rule(world, {
        .expr = "TagA"
    });

    test_assert(q != NULL);

    ecs_entity_t p1 = ecs_new_entity(world, "p1");
    ecs_entity_t p2 = ecs_new_entity(world, "p1");

    ecs_entity_t e1 = ecs_new_id(world);
    ecs_entity_t e2 = ecs_new_id(world);
    ecs_entity_t e3 = ecs_new_id(world);
    
    ecs_add_pair(world, e1, EcsChildOf, p1);
    ecs_add_pair(world, e2, EcsChildOf, p1);
    ecs_add_pair(world, e3, EcsChildOf, p2);

    ecs_add(world, e1, TagA);
    ecs_add(world, e2, TagA);
    ecs_add(world, e3, TagA);
    ecs_add(world, e3, TagB);

    ecs_iter_t it = ecs_rule_iter(world, q);
    char *json = ecs_iter_to_json(world, &it, &(ecs_iter_to_json_desc_t) {
        .serialize_rows = true
    });
    test_assert(json != NULL);

    char* expect_fmt = "{\"results\":["
        "{\"parent\":\"p1\", \"name\":\"%u\", \"tags\":[\"TagA\"]}, "
        "{\"parent\":\"p1\", \"name\":\"%u\", \"tags\":[\"TagA\"]}, "
        "{\"parent\":\"p1\", \"name\":\"%u\", \"tags\":[\"TagA\"]}]}";

    char *expect = ecs_asprintf(expect_fmt, 
        (uint32_t)e1, 
        (uint32_t)e2, 
        (uint32_t)e3);
    test_str(json, expect);

    ecs_os_free(json);
    ecs_os_free(expect);

    ecs_rule_fini(q);

    ecs_fini(world);
}

void SerializeIterToRowJson_serialize_this_w_1_tag_doc_name(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_doc_set_name(world, TagA, "The Tag A");
    ecs_doc_set_name(world, TagB, "The Tag B");

    ecs_rule_t *q = ecs_rule(world, {
        .expr = "TagA"
    });

    test_assert(q != NULL);

    ecs_entity_t p1 = ecs_new_entity(world, "p1");
    ecs_entity_t p2 = ecs_new_entity(world, "p1");

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_entity_t e2 = ecs_new_entity(world, "e2");
    ecs_entity_t e3 = ecs_new_entity(world, "e3");
    
    ecs_add_pair(world, e1, EcsChildOf, p1);
    ecs_add_pair(world, e2, EcsChildOf, p1);
    ecs_add_pair(world, e3, EcsChildOf, p2);

    ecs_add(world, e1, TagA);
    ecs_add(world, e2, TagA);
    ecs_add(world, e3, TagA);
    ecs_add(world, e3, TagB);

    ecs_iter_t it = ecs_rule_iter(world, q);
    char *json = ecs_iter_to_json(world, &it, &(ecs_iter_to_json_desc_t) {
        .serialize_rows = true
    });
    test_assert(json != NULL);

    char* expect = "{\"results\":["
        "{\"parent\":\"p1\", \"name\":\"e1\", \"tags\":[\"The Tag A\"]}, "
        "{\"parent\":\"p1\", \"name\":\"e2\", \"tags\":[\"The Tag A\"]}, "
        "{\"parent\":\"p1\", \"name\":\"e3\", \"tags\":[\"The Tag A\"]}]}";
    test_str(json, expect);

    ecs_os_free(json);

    ecs_rule_fini(q);

    ecs_fini(world);
}

void SerializeIterToRowJson_serialize_this_w_2_tag(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_rule_t *q = ecs_rule(world, {
        .expr = "TagA, TagB"
    });

    test_assert(q != NULL);

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_entity_t e2 = ecs_new_entity(world, "e2");
    ecs_entity_t e3 = ecs_new_entity(world, "e3");

    ecs_add(world, e1, TagA);
    ecs_add(world, e2, TagA);
    ecs_add(world, e3, TagA);

    ecs_add(world, e1, TagB);
    ecs_add(world, e2, TagB);
    ecs_add(world, e3, TagB);

    ecs_iter_t it = ecs_rule_iter(world, q);
    char *json = ecs_iter_to_json(world, &it, &(ecs_iter_to_json_desc_t) {
        .serialize_rows = true
    });
    test_assert(json != NULL);

    char* expect = "{\"results\":["
        "{\"name\":\"e1\", \"tags\":[\"TagA\", \"TagB\"]}, "
        "{\"name\":\"e2\", \"tags\":[\"TagA\", \"TagB\"]}, "
        "{\"name\":\"e3\", \"tags\":[\"TagA\", \"TagB\"]}]}";
    test_str(json, expect);

    ecs_os_free(json);

    ecs_rule_fini(q);

    ecs_fini(world);
}

void SerializeIterToRowJson_serialize_this_w_1_component(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_struct(world, {
        .entity = ecs_id(Position),
        .members = {
            { "x", ecs_id(ecs_i32_t) },
            { "y", ecs_id(ecs_i32_t) }
        }
    });

    ecs_rule_t *q = ecs_rule(world, {
        .expr = "Position"
    });

    test_assert(q != NULL);

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_entity_t e2 = ecs_new_entity(world, "e2");
    ecs_entity_t e3 = ecs_new_entity(world, "e3");

    ecs_set(world, e1, Position, {10, 20});
    ecs_set(world, e2, Position, {20, 30});
    ecs_set(world, e3, Position, {30, 40});

    ecs_iter_t it = ecs_rule_iter(world, q);
    char *json = ecs_iter_to_json(world, &it, &(ecs_iter_to_json_desc_t) {
        .serialize_rows = true
    });
    test_assert(json != NULL);

    char* expect = "{\"results\":["
        "{\"name\":\"e1\", \"components\":{\"Position\":{\"x\":10, \"y\":20}}}, "
        "{\"name\":\"e2\", \"components\":{\"Position\":{\"x\":20, \"y\":30}}}, "
        "{\"name\":\"e3\", \"components\":{\"Position\":{\"x\":30, \"y\":40}}}]}";
    
    test_str(json, expect);

    ecs_os_free(json);

    ecs_rule_fini(q);

    ecs_fini(world);
}

void SerializeIterToRowJson_serialize_this_w_2_component(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Mass);

    ecs_struct(world, {
        .entity = ecs_id(Position),
        .members = {
            { "x", ecs_id(ecs_i32_t) },
            { "y", ecs_id(ecs_i32_t) }
        }
    });

    ecs_struct(world, {
        .entity = ecs_id(Mass),
        .members = {
            { "value", ecs_id(ecs_i32_t) },
        }
    });


    ecs_rule_t *q = ecs_rule(world, {
        .expr = "Position, Mass"
    });

    test_assert(q != NULL);

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_entity_t e2 = ecs_new_entity(world, "e2");
    ecs_entity_t e3 = ecs_new_entity(world, "e3");

    ecs_set(world, e1, Position, {10, 20});
    ecs_set(world, e2, Position, {20, 30});
    ecs_set(world, e3, Position, {30, 40});

    ecs_set(world, e1, Mass, {1});
    ecs_set(world, e2, Mass, {2});
    ecs_set(world, e3, Mass, {3});

    ecs_iter_t it = ecs_rule_iter(world, q);
    char *json = ecs_iter_to_json(world, &it, &(ecs_iter_to_json_desc_t) {
        .serialize_rows = true
    });
    test_assert(json != NULL);

    char* expect = "{\"results\":["
        "{\"name\":\"e1\", \"components\":{\"Position\":{\"x\":10, \"y\":20}, \"Mass\":{\"value\":1}}}, "
        "{\"name\":\"e2\", \"components\":{\"Position\":{\"x\":20, \"y\":30}, \"Mass\":{\"value\":2}}}, "
        "{\"name\":\"e3\", \"components\":{\"Position\":{\"x\":30, \"y\":40}, \"Mass\":{\"value\":3}}}]}";
    test_str(json, expect);

    ecs_os_free(json);

    ecs_rule_fini(q);

    ecs_fini(world);
}

void SerializeIterToRowJson_serialize_this_w_2_component_1_shared(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Mass);

    ecs_struct(world, {
        .entity = ecs_id(Position),
        .members = {
            { "x", ecs_id(ecs_i32_t) },
            { "y", ecs_id(ecs_i32_t) }
        }
    });

    ecs_struct(world, {
        .entity = ecs_id(Mass),
        .members = {
            { "value", ecs_id(ecs_i32_t) },
        }
    });


    ecs_rule_t *q = ecs_rule(world, {
        .expr = "Position, Mass"
    });

    test_assert(q != NULL);

    ecs_entity_t base_1 = ecs_new_entity(world, "base_1");
    ecs_entity_t base_2 = ecs_new_entity(world, "base_2");
    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_entity_t e2 = ecs_new_entity(world, "e2");
    ecs_entity_t e3 = ecs_new_entity(world, "e3");

    ecs_set(world, e1, Position, {10, 20});
    ecs_set(world, e2, Position, {20, 30});
    ecs_set(world, e3, Position, {30, 40});

    ecs_add_pair(world, e1, EcsIsA, base_1);
    ecs_add_pair(world, e2, EcsIsA, base_1);
    ecs_add_pair(world, e3, EcsIsA, base_2);

    ecs_set(world, base_1, Mass, {1});
    ecs_set(world, base_2, Mass, {2});

    ecs_iter_t it = ecs_rule_iter(world, q);
    char *json = ecs_iter_to_json(world, &it, &(ecs_iter_to_json_desc_t) {
        .serialize_rows = true
    });
    test_assert(json != NULL);

    char* expect = "{\"results\":["
        "{\"name\":\"e1\", \"components\":{\"Position\":{\"x\":10, \"y\":20}, \"Mass\":{\"value\":1}}}, "
        "{\"name\":\"e2\", \"components\":{\"Position\":{\"x\":20, \"y\":30}, \"Mass\":{\"value\":1}}}, "
        "{\"name\":\"e3\", \"components\":{\"Position\":{\"x\":30, \"y\":40}, \"Mass\":{\"value\":2}}}]}";
    test_str(json, expect);

    ecs_os_free(json);

    ecs_rule_fini(q);

    ecs_fini(world);
}

void SerializeIterToRowJson_serialize_this_w_1_pair(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_rule_t *q = ecs_rule(world, {
        .expr = "(RelA, *)"
    });

    test_assert(q != NULL);

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_entity_t e2 = ecs_new_entity(world, "e2");
    ecs_entity_t e3 = ecs_new_entity(world, "e3");

    ecs_add_pair(world, e1, RelA, TgtA);
    ecs_add_pair(world, e2, RelA, TgtA);
    ecs_add_pair(world, e3, RelA, TgtB);

    ecs_iter_t it = ecs_rule_iter(world, q);
    char *json = ecs_iter_to_json(world, &it, &(ecs_iter_to_json_desc_t) {
        .serialize_rows = true
    });
    test_assert(json != NULL);

    char* expect = "{\"results\":["
        "{\"name\":\"e1\", \"pairs\":{\"RelA\":\"TgtA\"}}, "
        "{\"name\":\"e2\", \"pairs\":{\"RelA\":\"TgtA\"}}, "
        "{\"name\":\"e3\", \"pairs\":{\"RelA\":\"TgtB\"}}]}";
    test_str(json, expect);

    ecs_os_free(json);
    
    ecs_rule_fini(q);

    ecs_fini(world);
}

void SerializeIterToRowJson_serialize_this_w_1_pair_component(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Mass);

    ecs_struct(world, {
        .entity = ecs_id(Position),
        .members = {
            { "x", ecs_id(ecs_i32_t) },
            { "y", ecs_id(ecs_i32_t) }
        }
    });

    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_rule_t *q = ecs_rule(world, {
        .expr = "(Position, *)"
    });

    test_assert(q != NULL);

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_entity_t e2 = ecs_new_entity(world, "e2");
    ecs_entity_t e3 = ecs_new_entity(world, "e3");

    ecs_set_pair(world, e1, Position, TgtA, {10, 20});
    ecs_set_pair(world, e2, Position, TgtA, {20, 30});
    ecs_set_pair(world, e3, Position, TgtB, {30, 40});

    ecs_iter_t it = ecs_rule_iter(world, q);
    char *json = ecs_iter_to_json(world, &it, &(ecs_iter_to_json_desc_t) {
        .serialize_rows = true
    });
    test_assert(json != NULL);

    char* expect = "{\"results\":["
        "{\"name\":\"e1\", \"pairs\":{\"Position\":\"TgtA\"}, \"components\":{\"(Position,TgtA)\":{\"x\":10, \"y\":20}}}, "
        "{\"name\":\"e2\", \"pairs\":{\"Position\":\"TgtA\"}, \"components\":{\"(Position,TgtA)\":{\"x\":20, \"y\":30}}}, "
        "{\"name\":\"e3\", \"pairs\":{\"Position\":\"TgtB\"}, \"components\":{\"(Position,TgtA)\":{\"x\":30, \"y\":40}}}]}";
    test_str(json, expect);

    ecs_os_free(json);
    
    ecs_rule_fini(q);

    ecs_fini(world);
}

void SerializeIterToRowJson_serialize_this_w_2_pair(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_rule_t *q = ecs_rule(world, {
        .expr = "(RelA, *), (RelB, *)"
    });

    test_assert(q != NULL);

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_entity_t e2 = ecs_new_entity(world, "e2");
    ecs_entity_t e3 = ecs_new_entity(world, "e3");

    ecs_add_pair(world, e1, RelA, TgtA);
    ecs_add_pair(world, e2, RelA, TgtA);
    ecs_add_pair(world, e3, RelA, TgtA);

    ecs_add_pair(world, e1, RelB, TgtA);
    ecs_add_pair(world, e2, RelB, TgtA);
    ecs_add_pair(world, e3, RelB, TgtB);

    ecs_iter_t it = ecs_rule_iter(world, q);
    char *json = ecs_iter_to_json(world, &it, &(ecs_iter_to_json_desc_t) {
        .serialize_rows = true
    });
    test_assert(json != NULL);

    char* expect = "{\"results\":["
        "{\"name\":\"e1\", \"pairs\":{\"RelA\":\"TgtA\", \"RelB\":\"TgtA\"}}, "
        "{\"name\":\"e2\", \"pairs\":{\"RelA\":\"TgtA\", \"RelB\":\"TgtA\"}}, "
        "{\"name\":\"e3\", \"pairs\":{\"RelA\":\"TgtA\", \"RelB\":\"TgtB\"}}]}";
    test_str(json, expect);

    ecs_os_free(json);
    
    ecs_rule_fini(q);

    ecs_fini(world);
}

void SerializeIterToRowJson_serialize_this_w_1_var(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_rule_t *q = ecs_rule(world, {
        .expr = "(RelA, $a)"
    });

    test_assert(q != NULL);

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_entity_t e2 = ecs_new_entity(world, "e2");
    ecs_entity_t e3 = ecs_new_entity(world, "e3");

    ecs_add_pair(world, e1, RelA, TgtA);
    ecs_add_pair(world, e2, RelA, TgtA);
    ecs_add_pair(world, e3, RelA, TgtB);

    ecs_iter_t it = ecs_rule_iter(world, q);
    char *json = ecs_iter_to_json(world, &it, &(ecs_iter_to_json_desc_t) {
        .serialize_rows = true
    });
    test_assert(json != NULL);

    char* expect = "{\"results\":["
        "{\"name\":\"e1\", \"pairs\":{\"RelA\":\"TgtA\"},\"vars\":{\"a\":\"TgtA\"}}, "
        "{\"name\":\"e2\", \"pairs\":{\"RelA\":\"TgtA\"},\"vars\":{\"a\":\"TgtA\"}}, "
        "{\"name\":\"e3\", \"pairs\":{\"RelA\":\"TgtB\"},\"vars\":{\"a\":\"TgtB\"}}]}";
    test_str(json, expect);

    ecs_os_free(json);

    ecs_rule_fini(q);

    ecs_fini(world);
}

void SerializeIterToRowJson_serialize_this_w_2_var(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_rule_t *q = ecs_rule(world, {
        .expr = "(RelA, $a), (RelB, $b)"
    });

    test_assert(q != NULL);

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_entity_t e2 = ecs_new_entity(world, "e2");
    ecs_entity_t e3 = ecs_new_entity(world, "e3");

    ecs_add_pair(world, e1, RelA, TgtA);
    ecs_add_pair(world, e2, RelA, TgtA);
    ecs_add_pair(world, e3, RelA, TgtA);

    ecs_add_pair(world, e1, RelB, TgtA);
    ecs_add_pair(world, e2, RelB, TgtA);
    ecs_add_pair(world, e3, RelB, TgtB);

    ecs_iter_t it = ecs_rule_iter(world, q);
    char *json = ecs_iter_to_json(world, &it, &(ecs_iter_to_json_desc_t) {
        .serialize_rows = true
    });
    test_assert(json != NULL);

    char* expect = "{\"results\":["
        "{\"name\":\"e1\", \"pairs\":{\"RelA\":\"TgtA\", \"RelB\":\"TgtA\"},\"vars\":{\"a\":\"TgtA\", \"b\":\"TgtA\"}}, "
        "{\"name\":\"e2\", \"pairs\":{\"RelA\":\"TgtA\", \"RelB\":\"TgtA\"},\"vars\":{\"a\":\"TgtA\", \"b\":\"TgtA\"}}, "
        "{\"name\":\"e3\", \"pairs\":{\"RelA\":\"TgtA\", \"RelB\":\"TgtB\"},\"vars\":{\"a\":\"TgtA\", \"b\":\"TgtB\"}}]}";
    test_str(json, expect);

    ecs_os_free(json);

    ecs_rule_fini(q);

    ecs_fini(world);
}

void SerializeIterToRowJson_serialize_this_w_2_var_doc_name(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_doc_set_name(world, TgtA, "Target A");
    ecs_doc_set_name(world, TgtB, "Target B");

    ecs_rule_t *q = ecs_rule(world, {
        .expr = "(RelA, $a), (RelB, $b)"
    });

    test_assert(q != NULL);

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_entity_t e2 = ecs_new_entity(world, "e2");
    ecs_entity_t e3 = ecs_new_entity(world, "e3");

    ecs_add_pair(world, e1, RelA, TgtA);
    ecs_add_pair(world, e2, RelA, TgtA);
    ecs_add_pair(world, e3, RelA, TgtA);

    ecs_add_pair(world, e1, RelB, TgtA);
    ecs_add_pair(world, e2, RelB, TgtA);
    ecs_add_pair(world, e3, RelB, TgtB);

    ecs_iter_t it = ecs_rule_iter(world, q);
    char *json = ecs_iter_to_json(world, &it, &(ecs_iter_to_json_desc_t) {
        .serialize_rows = true
    });
    test_assert(json != NULL);

    char* expect = "{\"results\":["
        "{\"name\":\"e1\", \"pairs\":{\"RelA\":\"Target A\", \"RelB\":\"Target A\"},\"vars\":{\"a\":\"Target A\", \"b\":\"Target A\"}}, "
        "{\"name\":\"e2\", \"pairs\":{\"RelA\":\"Target A\", \"RelB\":\"Target A\"},\"vars\":{\"a\":\"Target A\", \"b\":\"Target A\"}}, "
        "{\"name\":\"e3\", \"pairs\":{\"RelA\":\"Target A\", \"RelB\":\"Target B\"},\"vars\":{\"a\":\"Target A\", \"b\":\"Target B\"}}]}";
    test_str(json, expect);

    ecs_os_free(json);

    ecs_rule_fini(q);

    ecs_fini(world);
}

void SerializeIterToRowJson_serialize_this_w_1_tag_component_pair_var(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);
    ECS_TAG(world, RelA);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ECS_COMPONENT(world, Position);

    ecs_struct(world, {
        .entity = ecs_id(Position),
        .members = {
            { "x", ecs_id(ecs_i32_t) },
            { "y", ecs_id(ecs_i32_t) }
        }
    });

    ecs_rule_t *q = ecs_rule(world, {
        .expr = "TagA, (RelA, $a), Position"
    });

    test_assert(q != NULL);

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_entity_t e2 = ecs_new_entity(world, "e2");
    ecs_entity_t e3 = ecs_new_entity(world, "e3");

    ecs_add(world, e1, TagA);
    ecs_add(world, e2, TagA);
    ecs_add(world, e3, TagA);

    ecs_add_pair(world, e1, RelA, TgtA);
    ecs_add_pair(world, e2, RelA, TgtA);
    ecs_add_pair(world, e3, RelA, TgtA);

    ecs_set(world, e1, Position, {10, 20});
    ecs_set(world, e2, Position, {20, 30});
    ecs_set(world, e3, Position, {30, 40});

    ecs_iter_t it = ecs_rule_iter(world, q);
    char *json = ecs_iter_to_json(world, &it, &(ecs_iter_to_json_desc_t) {
        .serialize_rows = true
    });
    test_assert(json != NULL);

    char* expect = "{\"results\":["
        "{\"name\":\"e1\", \"tags\":[\"TagA\"],\"pairs\":{\"RelA\":\"TgtA\"},\"vars\":{\"a\":\"TgtA\"}, \"components\":{\"Position\":{\"x\":10, \"y\":20}}}, "
        "{\"name\":\"e2\", \"tags\":[\"TagA\"],\"pairs\":{\"RelA\":\"TgtA\"},\"vars\":{\"a\":\"TgtA\"}, \"components\":{\"Position\":{\"x\":20, \"y\":30}}}, "
        "{\"name\":\"e3\", \"tags\":[\"TagA\"],\"pairs\":{\"RelA\":\"TgtA\"},\"vars\":{\"a\":\"TgtA\"}, \"components\":{\"Position\":{\"x\":30, \"y\":40}}}]}";

    test_str(json, expect);

    ecs_os_free(json);

    ecs_rule_fini(q);

    ecs_fini(world);
}

void SerializeIterToRowJson_serialize_this_w_2_tag_component_pair_var(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Mass);

    ecs_struct(world, {
        .entity = ecs_id(Position),
        .members = {
            { "x", ecs_id(ecs_i32_t) },
            { "y", ecs_id(ecs_i32_t) }
        }
    });

    ecs_struct(world, {
        .entity = ecs_id(Mass),
        .members = {
            { "value", ecs_id(ecs_i32_t) },
        }
    });


    ecs_rule_t *q = ecs_rule(world, {
        .expr = "TagA, TagB, (RelA, $a), (RelB, $b), Position, Mass"
    });

    test_assert(q != NULL);

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_entity_t e2 = ecs_new_entity(world, "e2");
    ecs_entity_t e3 = ecs_new_entity(world, "e3");

    ecs_add(world, e1, TagA);
    ecs_add(world, e2, TagA);
    ecs_add(world, e3, TagA);

    ecs_add(world, e1, TagB);
    ecs_add(world, e2, TagB);
    ecs_add(world, e3, TagB);

    ecs_add_pair(world, e1, RelA, TgtA);
    ecs_add_pair(world, e2, RelA, TgtA);
    ecs_add_pair(world, e3, RelA, TgtA);

    ecs_add_pair(world, e1, RelB, TgtA);
    ecs_add_pair(world, e2, RelB, TgtA);
    ecs_add_pair(world, e3, RelB, TgtB);

    ecs_set(world, e1, Position, {10, 20});
    ecs_set(world, e2, Position, {20, 30});
    ecs_set(world, e3, Position, {30, 40});

    ecs_set(world, e1, Mass, {1});
    ecs_set(world, e2, Mass, {2});
    ecs_set(world, e3, Mass, {3});

    ecs_iter_t it = ecs_rule_iter(world, q);
    char *json = ecs_iter_to_json(world, &it, &(ecs_iter_to_json_desc_t) {
        .serialize_rows = true
    });
    test_assert(json != NULL);

    char* expect = "{\"results\":["
        "{\"name\":\"e1\", \"tags\":[\"TagA\", \"TagB\"],\"pairs\":{\"RelA\":\"TgtA\", \"RelB\":\"TgtA\"},\"vars\":{\"a\":\"TgtA\", \"b\":\"TgtA\"}, \"components\":{\"Position\":{\"x\":10, \"y\":20}, \"Mass\":{\"value\":1}}}, "
        "{\"name\":\"e2\", \"tags\":[\"TagA\", \"TagB\"],\"pairs\":{\"RelA\":\"TgtA\", \"RelB\":\"TgtA\"},\"vars\":{\"a\":\"TgtA\", \"b\":\"TgtA\"}, \"components\":{\"Position\":{\"x\":20, \"y\":30}, \"Mass\":{\"value\":2}}}, "
        "{\"name\":\"e3\", \"tags\":[\"TagA\", \"TagB\"],\"pairs\":{\"RelA\":\"TgtA\", \"RelB\":\"TgtB\"},\"vars\":{\"a\":\"TgtA\", \"b\":\"TgtB\"}, \"components\":{\"Position\":{\"x\":30, \"y\":40}, \"Mass\":{\"value\":3}}}]}";
    test_str(json, expect);

    ecs_os_free(json);

    ecs_rule_fini(q);

    ecs_fini(world);
}

void SerializeIterToRowJson_serialize_var_w_1_tag(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_rule_t *q = ecs_rule(world, {
        .expr = "TagA($x)"
    });

    test_assert(q != NULL);

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_entity_t e2 = ecs_new_entity(world, "e2");
    ecs_entity_t e3 = ecs_new_entity(world, "e3");

    ecs_add(world, e1, TagA);
    ecs_add(world, e2, TagA);
    ecs_add(world, e3, TagA);
    ecs_add(world, e3, TagB);

    ecs_iter_t it = ecs_rule_iter(world, q);
    char *json = ecs_iter_to_json(world, &it, &(ecs_iter_to_json_desc_t) {
        .serialize_rows = true
    });
    test_assert(json != NULL);

    char* expect = "{\"results\":["
        "{\"tags\":[\"TagA\"],\"vars\":{\"x\":\"e1\"}}, "
        "{\"tags\":[\"TagA\"],\"vars\":{\"x\":\"e2\"}}, "
        "{\"tags\":[\"TagA\"],\"vars\":{\"x\":\"e3\"}}]}";
    test_str(json, expect);

    ecs_os_free(json);

    ecs_rule_fini(q);

    ecs_fini(world);
}

void SerializeIterToRowJson_serialize_var_w_1_component(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_struct(world, {
        .entity = ecs_id(Position),
        .members = {
            { "x", ecs_id(ecs_i32_t) },
            { "y", ecs_id(ecs_i32_t) }
        }
    });

    ecs_rule_t *q = ecs_rule(world, {
        .expr = "Position($x)"
    });

    test_assert(q != NULL);

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_entity_t e2 = ecs_new_entity(world, "e2");
    ecs_entity_t e3 = ecs_new_entity(world, "e3");

    ecs_set(world, e1, Position, {10, 20});
    ecs_set(world, e2, Position, {20, 30});
    ecs_set(world, e3, Position, {30, 40});

    ecs_iter_t it = ecs_rule_iter(world, q);
    char *json = ecs_iter_to_json(world, &it, &(ecs_iter_to_json_desc_t) {
        .serialize_rows = true
    });
    test_assert(json != NULL);

    char* expect = "{\"results\":["
        "{\"vars\":{\"x\":\"e1\"}, \"components\":{\"Position\":{\"x\":10, \"y\":20}}}, "
        "{\"vars\":{\"x\":\"e2\"}, \"components\":{\"Position\":{\"x\":20, \"y\":30}}}, "
        "{\"vars\":{\"x\":\"e3\"}, \"components\":{\"Position\":{\"x\":30, \"y\":40}}}]}";
    
    test_str(json, expect);

    ecs_os_free(json);

    ecs_rule_fini(q);

    ecs_fini(world);
}

void SerializeIterToRowJson_serialize_var_w_1_pair(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_rule_t *q = ecs_rule(world, {
        .expr = "RelA($x, *)"
    });

    test_assert(q != NULL);

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_entity_t e2 = ecs_new_entity(world, "e2");
    ecs_entity_t e3 = ecs_new_entity(world, "e3");

    ecs_add_pair(world, e1, RelA, TgtA);
    ecs_add_pair(world, e2, RelA, TgtA);
    ecs_add_pair(world, e3, RelA, TgtB);

    ecs_iter_t it = ecs_rule_iter(world, q);
    char *json = ecs_iter_to_json(world, &it, &(ecs_iter_to_json_desc_t) {
        .serialize_rows = true
    });
    test_assert(json != NULL);

    char* expect = "{\"results\":["
        "{\"pairs\":{\"RelA\":\"TgtA\"},\"vars\":{\"x\":\"e1\"}}, "
        "{\"pairs\":{\"RelA\":\"TgtA\"},\"vars\":{\"x\":\"e2\"}}, "
        "{\"pairs\":{\"RelA\":\"TgtB\"},\"vars\":{\"x\":\"e3\"}}]}";
    test_str(json, expect);

    ecs_os_free(json);

    ecs_rule_fini(q);

    ecs_fini(world);
}

void SerializeIterToRowJson_serialize_var_w_1_var(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_rule_t *q = ecs_rule(world, {
        .expr = "RelA($x, $a)"
    });

    test_assert(q != NULL);

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_entity_t e2 = ecs_new_entity(world, "e2");
    ecs_entity_t e3 = ecs_new_entity(world, "e3");

    ecs_add_pair(world, e1, RelA, TgtA);
    ecs_add_pair(world, e2, RelA, TgtA);
    ecs_add_pair(world, e3, RelA, TgtB);

    ecs_iter_t it = ecs_rule_iter(world, q);
    char *json = ecs_iter_to_json(world, &it, &(ecs_iter_to_json_desc_t) {
        .serialize_rows = true
    });
    test_assert(json != NULL);

    char* expect = "{\"results\":["
        "{\"pairs\":{\"RelA\":\"TgtA\"},\"vars\":{\"x\":\"e1\", \"a\":\"TgtA\"}}, "
        "{\"pairs\":{\"RelA\":\"TgtA\"},\"vars\":{\"x\":\"e2\", \"a\":\"TgtA\"}}, "
        "{\"pairs\":{\"RelA\":\"TgtB\"},\"vars\":{\"x\":\"e3\", \"a\":\"TgtB\"}}]}";
    test_str(json, expect);

    ecs_os_free(json);

    ecs_rule_fini(q);

    ecs_fini(world);
}

void SerializeIterToRowJson_serialize_var_w_2_component_1_shared(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Mass);

    ecs_struct(world, {
        .entity = ecs_id(Position),
        .members = {
            { "x", ecs_id(ecs_i32_t) },
            { "y", ecs_id(ecs_i32_t) }
        }
    });

    ecs_struct(world, {
        .entity = ecs_id(Mass),
        .members = {
            { "value", ecs_id(ecs_i32_t) },
        }
    });

    ecs_rule_t *q = ecs_rule(world, {
        .expr = "Position($x), Mass($x)"
    });

    test_assert(q != NULL);

    ecs_entity_t base_1 = ecs_new_entity(world, "base_1");
    ecs_entity_t base_2 = ecs_new_entity(world, "base_2");
    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_entity_t e2 = ecs_new_entity(world, "e2");
    ecs_entity_t e3 = ecs_new_entity(world, "e3");

    ecs_set(world, e1, Position, {10, 20});
    ecs_set(world, e2, Position, {20, 30});
    ecs_set(world, e3, Position, {30, 40});

    ecs_add_pair(world, e1, EcsIsA, base_1);
    ecs_add_pair(world, e2, EcsIsA, base_1);
    ecs_add_pair(world, e3, EcsIsA, base_2);

    ecs_set(world, base_1, Mass, {1});
    ecs_set(world, base_2, Mass, {2});

    ecs_iter_t it = ecs_rule_iter(world, q);
    char *json = ecs_iter_to_json(world, &it, &(ecs_iter_to_json_desc_t) {
        .serialize_rows = true
    });
    test_assert(json != NULL);

    char* expect = "{\"results\":["
        "{\"vars\":{\"x\":\"e1\"}, \"components\":{\"Position\":{\"x\":10, \"y\":20}, \"Mass\":{\"value\":1}}}, "
        "{\"vars\":{\"x\":\"e2\"}, \"components\":{\"Position\":{\"x\":20, \"y\":30}, \"Mass\":{\"value\":1}}}, "
        "{\"vars\":{\"x\":\"e3\"}, \"components\":{\"Position\":{\"x\":30, \"y\":40}, \"Mass\":{\"value\":2}}}]}";
    test_str(json, expect);

    ecs_os_free(json);

    ecs_rule_fini(q);

    ecs_fini(world);
}

void SerializeIterToRowJson_serialize_var_w_1_tag_component_pair_var(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);
    ECS_TAG(world, RelA);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ECS_COMPONENT(world, Position);

    ecs_struct(world, {
        .entity = ecs_id(Position),
        .members = {
            { "x", ecs_id(ecs_i32_t) },
            { "y", ecs_id(ecs_i32_t) }
        }
    });

    ecs_rule_t *q = ecs_rule(world, {
        .expr = "TagA($x), RelA($x, $a), Position($x)"
    });

    test_assert(q != NULL);

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_entity_t e2 = ecs_new_entity(world, "e2");
    ecs_entity_t e3 = ecs_new_entity(world, "e3");

    ecs_add(world, e1, TagA);
    ecs_add(world, e2, TagA);
    ecs_add(world, e3, TagA);

    ecs_add_pair(world, e1, RelA, TgtA);
    ecs_add_pair(world, e2, RelA, TgtA);
    ecs_add_pair(world, e3, RelA, TgtA);

    ecs_set(world, e1, Position, {10, 20});
    ecs_set(world, e2, Position, {20, 30});
    ecs_set(world, e3, Position, {30, 40});

    ecs_iter_t it = ecs_rule_iter(world, q);
    char *json = ecs_iter_to_json(world, &it, &(ecs_iter_to_json_desc_t) {
        .serialize_rows = true
    });
    test_assert(json != NULL);

    char* expect = "{\"results\":["
        "{\"tags\":[\"TagA\"],\"pairs\":{\"RelA\":\"TgtA\"},\"vars\":{\"x\":\"e1\", \"a\":\"TgtA\"}, \"components\":{\"Position\":{\"x\":10, \"y\":20}}}, "
        "{\"tags\":[\"TagA\"],\"pairs\":{\"RelA\":\"TgtA\"},\"vars\":{\"x\":\"e2\", \"a\":\"TgtA\"}, \"components\":{\"Position\":{\"x\":20, \"y\":30}}}, "
        "{\"tags\":[\"TagA\"],\"pairs\":{\"RelA\":\"TgtA\"},\"vars\":{\"x\":\"e3\", \"a\":\"TgtA\"}, \"components\":{\"Position\":{\"x\":30, \"y\":40}}}]}";
    test_str(json, expect);

    ecs_os_free(json);

    ecs_rule_fini(q);

    ecs_fini(world);
}

void SerializeIterToRowJson_serialize_var_w_2_tag_component_pair_var(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Mass);

    ecs_struct(world, {
        .entity = ecs_id(Position),
        .members = {
            { "x", ecs_id(ecs_i32_t) },
            { "y", ecs_id(ecs_i32_t) }
        }
    });

    ecs_struct(world, {
        .entity = ecs_id(Mass),
        .members = {
            { "value", ecs_id(ecs_i32_t) },
        }
    });


    ecs_rule_t *q = ecs_rule(world, {
        .expr = "TagA($x), TagB($x), RelA($x, $a), RelB($x, $b), Position($x), Mass($x)"
    });

    test_assert(q != NULL);

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_entity_t e2 = ecs_new_entity(world, "e2");
    ecs_entity_t e3 = ecs_new_entity(world, "e3");

    ecs_add(world, e1, TagA);
    ecs_add(world, e2, TagA);
    ecs_add(world, e3, TagA);

    ecs_add(world, e1, TagB);
    ecs_add(world, e2, TagB);
    ecs_add(world, e3, TagB);

    ecs_add_pair(world, e1, RelA, TgtA);
    ecs_add_pair(world, e2, RelA, TgtA);
    ecs_add_pair(world, e3, RelA, TgtA);

    ecs_add_pair(world, e1, RelB, TgtA);
    ecs_add_pair(world, e2, RelB, TgtA);
    ecs_add_pair(world, e3, RelB, TgtB);

    ecs_set(world, e1, Position, {10, 20});
    ecs_set(world, e2, Position, {20, 30});
    ecs_set(world, e3, Position, {30, 40});

    ecs_set(world, e1, Mass, {1});
    ecs_set(world, e2, Mass, {2});
    ecs_set(world, e3, Mass, {3});

    ecs_iter_t it = ecs_rule_iter(world, q);
    char *json = ecs_iter_to_json(world, &it, &(ecs_iter_to_json_desc_t) {
        .serialize_rows = true
    });
    test_assert(json != NULL);

    char* expect = "{\"results\":["
        "{\"tags\":[\"TagA\", \"TagB\"],\"pairs\":{\"RelA\":\"TgtA\", \"RelB\":\"TgtA\"},\"vars\":{\"x\":\"e1\", \"a\":\"TgtA\", \"b\":\"TgtA\"}, \"components\":{\"Position\":{\"x\":10, \"y\":20}, \"Mass\":{\"value\":1}}}, "
        "{\"tags\":[\"TagA\", \"TagB\"],\"pairs\":{\"RelA\":\"TgtA\", \"RelB\":\"TgtA\"},\"vars\":{\"x\":\"e2\", \"a\":\"TgtA\", \"b\":\"TgtA\"}, \"components\":{\"Position\":{\"x\":20, \"y\":30}, \"Mass\":{\"value\":2}}}, "
        "{\"tags\":[\"TagA\", \"TagB\"],\"pairs\":{\"RelA\":\"TgtA\", \"RelB\":\"TgtB\"},\"vars\":{\"x\":\"e3\", \"a\":\"TgtA\", \"b\":\"TgtB\"}, \"components\":{\"Position\":{\"x\":30, \"y\":40}, \"Mass\":{\"value\":3}}}]}";
    test_str(json, expect);

    ecs_os_free(json);

    ecs_rule_fini(q);

    ecs_fini(world);
}

void SerializeIterToRowJson_serialize_fixed_w_1_tag(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_entity_t e1 = ecs_new_entity(world, "e1");

    ecs_rule_t *q = ecs_rule(world, {
        .expr = "TagA(e1)"
    });

    test_assert(q != NULL);

    ecs_add(world, e1, TagA);

    ecs_iter_t it = ecs_rule_iter(world, q);
    char *json = ecs_iter_to_json(world, &it, &(ecs_iter_to_json_desc_t) {
        .serialize_rows = true
    });
    test_assert(json != NULL);

    char* expect = "{\"results\":[{\"tags\":[\"TagA\"]}]}";
    test_str(json, expect);

    ecs_os_free(json);

    ecs_rule_fini(q);

    ecs_fini(world);
}

void SerializeIterToRowJson_serialize_fixed_w_1_component(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_struct(world, {
        .entity = ecs_id(Position),
        .members = {
            { "x", ecs_id(ecs_i32_t) },
            { "y", ecs_id(ecs_i32_t) }
        }
    });

    ecs_entity_t e1 = ecs_new_entity(world, "e1");

    ecs_rule_t *q = ecs_rule(world, {
        .expr = "Position(e1)"
    });

    test_assert(q != NULL);

    ecs_set(world, e1, Position, {10, 20});

    ecs_iter_t it = ecs_rule_iter(world, q);
    char *json = ecs_iter_to_json(world, &it, &(ecs_iter_to_json_desc_t) {
        .serialize_rows = true
    });
    test_assert(json != NULL);

    char* expect = "{\"results\":[{\"components\":{\"Position\":{\"x\":10, \"y\":20}}}]}";
    
    test_str(json, expect);

    ecs_os_free(json);

    ecs_rule_fini(q);

    ecs_fini(world);
}

void SerializeIterToRowJson_serialize_fixed_w_1_pair(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_entity_t e1 = ecs_new_entity(world, "e1");

    ecs_rule_t *q = ecs_rule(world, {
        .expr = "RelA(e1, *)"
    });

    test_assert(q != NULL);

    ecs_add_pair(world, e1, RelA, TgtA);

    ecs_iter_t it = ecs_rule_iter(world, q);
    char *json = ecs_iter_to_json(world, &it, &(ecs_iter_to_json_desc_t) {
        .serialize_rows = true
    });
    test_assert(json != NULL);

    char* expect = "{\"results\":[{\"pairs\":{\"RelA\":\"TgtA\"}}]}";
    test_str(json, expect);

    ecs_os_free(json);
    
    ecs_rule_fini(q);

    ecs_fini(world);
}

void SerializeIterToRowJson_serialize_fixed_w_1_var(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_entity_t e1 = ecs_new_entity(world, "e1");

    ecs_rule_t *q = ecs_rule(world, {
        .expr = "RelA(e1, $a)"
    });

    test_assert(q != NULL);

    ecs_add_pair(world, e1, RelA, TgtA);

    ecs_iter_t it = ecs_rule_iter(world, q);
    char *json = ecs_iter_to_json(world, &it, &(ecs_iter_to_json_desc_t) {
        .serialize_rows = true
    });
    test_assert(json != NULL);

    char* expect = "{\"results\":[{\"pairs\":{\"RelA\":\"TgtA\"},\"vars\":{\"a\":\"TgtA\"}}]}";
    test_str(json, expect);

    ecs_os_free(json);

    ecs_rule_fini(q);

    ecs_fini(world);
}

void SerializeIterToRowJson_serialize_fixed_w_2_component_1_shared(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Mass);

    ecs_struct(world, {
        .entity = ecs_id(Position),
        .members = {
            { "x", ecs_id(ecs_i32_t) },
            { "y", ecs_id(ecs_i32_t) }
        }
    });

    ecs_struct(world, {
        .entity = ecs_id(Mass),
        .members = {
            { "value", ecs_id(ecs_i32_t) },
        }
    });

    ecs_entity_t e1 = ecs_new_entity(world, "e1");

    ecs_rule_t *q = ecs_rule(world, {
        .expr = "Position(e1), Mass(e1)"
    });

    test_assert(q != NULL);

    ecs_entity_t base_1 = ecs_new_entity(world, "base_1");

    ecs_set(world, e1, Position, {10, 20});
    ecs_add_pair(world, e1, EcsIsA, base_1);
    ecs_set(world, base_1, Mass, {1});

    ecs_iter_t it = ecs_rule_iter(world, q);
    char *json = ecs_iter_to_json(world, &it, &(ecs_iter_to_json_desc_t) {
        .serialize_rows = true
    });
    test_assert(json != NULL);

    char* expect = "{\"results\":[{\"components\":{\"Position\":{\"x\":10, \"y\":20}, \"Mass\":{\"value\":1}}}]}";
    test_str(json, expect);

    ecs_os_free(json);

    ecs_rule_fini(q);

    ecs_fini(world);
}

void SerializeIterToRowJson_serialize_fixed_w_1_tag_component_pair_var(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);
    ECS_TAG(world, RelA);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ECS_COMPONENT(world, Position);

    ecs_struct(world, {
        .entity = ecs_id(Position),
        .members = {
            { "x", ecs_id(ecs_i32_t) },
            { "y", ecs_id(ecs_i32_t) }
        }
    });

    ecs_entity_t e1 = ecs_new_entity(world, "e1");

    ecs_rule_t *q = ecs_rule(world, {
        .expr = "TagA(e1), RelA(e1, $a), Position(e1)"
    });

    test_assert(q != NULL);

    ecs_add(world, e1, TagA);
    ecs_add_pair(world, e1, RelA, TgtA);
    ecs_set(world, e1, Position, {10, 20});

    ecs_iter_t it = ecs_rule_iter(world, q);
    char *json = ecs_iter_to_json(world, &it, &(ecs_iter_to_json_desc_t) {
        .serialize_rows = true
    });
    test_assert(json != NULL);

    char* expect = "{\"results\":[{\"tags\":[\"TagA\"],\"pairs\":{\"RelA\":\"TgtA\"},\"vars\":{\"a\":\"TgtA\"}, \"components\":{\"Position\":{\"x\":10, \"y\":20}}}]}";
    test_str(json, expect);

    ecs_os_free(json);

    ecs_rule_fini(q);

    ecs_fini(world);
}

void SerializeIterToRowJson_serialize_fixed_w_2_tag_component_pair_var(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Mass);

    ecs_struct(world, {
        .entity = ecs_id(Position),
        .members = {
            { "x", ecs_id(ecs_i32_t) },
            { "y", ecs_id(ecs_i32_t) }
        }
    });

    ecs_struct(world, {
        .entity = ecs_id(Mass),
        .members = {
            { "value", ecs_id(ecs_i32_t) },
        }
    });

    ecs_entity_t e1 = ecs_new_entity(world, "e1");

    ecs_rule_t *q = ecs_rule(world, {
        .expr = "TagA(e1), TagB(e1), RelA(e1, $a), RelB(e1, $b), Position(e1), Mass(e1)"
    });

    test_assert(q != NULL);

    ecs_add(world, e1, TagA);
    ecs_add(world, e1, TagB);
    ecs_add_pair(world, e1, RelA, TgtA);
    ecs_add_pair(world, e1, RelB, TgtA);
    ecs_set(world, e1, Position, {10, 20});
    ecs_set(world, e1, Mass, {1});

    ecs_iter_t it = ecs_rule_iter(world, q);
    char *json = ecs_iter_to_json(world, &it, &(ecs_iter_to_json_desc_t) {
        .serialize_rows = true
    });
    test_assert(json != NULL);

    char* expect = "{\"results\":["
        "{\"tags\":[\"TagA\", \"TagB\"],\"pairs\":{\"RelA\":\"TgtA\", \"RelB\":\"TgtA\"},"
        "\"vars\":{\"a\":\"TgtA\", \"b\":\"TgtA\"}, "
        "\"components\":{\"Position\":{\"x\":10, \"y\":20}, \"Mass\":{\"value\":1}}}]}";
    test_str(json, expect);

    ecs_os_free(json);
    
    ecs_rule_fini(q);

    ecs_fini(world);
}

void SerializeIterToRowJson_serialize_not(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Mass);

    ecs_struct(world, {
        .entity = ecs_id(Position),
        .members = {
            { "x", ecs_id(ecs_i32_t) },
            { "y", ecs_id(ecs_i32_t) }
        }
    });

    ecs_struct(world, {
        .entity = ecs_id(Mass),
        .members = {
            { "value", ecs_id(ecs_i32_t) },
        }
    });


    ecs_rule_t *q = ecs_rule(world, {
        .expr = "Position, !Mass"
    });

    test_assert(q != NULL);

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_entity_t e2 = ecs_new_entity(world, "e2");
    ecs_entity_t e3 = ecs_new_entity(world, "e3");

    ecs_set(world, e1, Position, {10, 20});
    ecs_set(world, e2, Position, {20, 30});
    ecs_set(world, e3, Position, {30, 40});

    ecs_set(world, e1, Mass, {1});
    ecs_set(world, e2, Mass, {2});

    ecs_iter_t it = ecs_rule_iter(world, q);
    char *json = ecs_iter_to_json(world, &it, &(ecs_iter_to_json_desc_t) {
        .serialize_rows = true
    });
    test_assert(json != NULL);

    char* expect = "{\"results\":[{\"name\":\"e3\", \"is_set\":[true, false], \"components\":{\"Position\":{\"x\":30, \"y\":40}}}]}";
    test_str(json, expect);

    ecs_os_free(json);

    ecs_rule_fini(q);

    ecs_fini(world);
}

void SerializeIterToRowJson_serialize_not_pair_wildcard(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_rule_t *q = ecs_rule(world, {
        .expr = "(RelA, *), !(RelB, *)"
    });

    test_assert(q != NULL);

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_entity_t e2 = ecs_new_entity(world, "e2");
    ecs_entity_t e3 = ecs_new_entity(world, "e3");

    ecs_add_pair(world, e1, RelA, TgtA);
    ecs_add_pair(world, e2, RelA, TgtA);
    ecs_add_pair(world, e3, RelA, TgtA);
    ecs_add_pair(world, e3, RelB, TgtA);

    ecs_iter_t it = ecs_rule_iter(world, q);
    char *json = ecs_iter_to_json(world, &it, &(ecs_iter_to_json_desc_t) {
        .serialize_rows = true
    });
    test_assert(json != NULL);

    char* expect = "{\"results\":["
        "{\"name\":\"e1\", \"pairs\":{\"RelA\":\"TgtA\"},\"is_set\":[true, false]}, "
        "{\"name\":\"e2\", \"pairs\":{\"RelA\":\"TgtA\"},\"is_set\":[true, false]}]}";
    test_str(json, expect);

    ecs_os_free(json);

    ecs_rule_fini(q);

    ecs_fini(world);
}

void SerializeIterToRowJson_serialize_not_pair_var(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_rule_t *q = ecs_rule(world, {
        .expr = "(RelA, *), !(RelB, $var)"
    });

    test_assert(q != NULL);

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_entity_t e2 = ecs_new_entity(world, "e2");
    ecs_entity_t e3 = ecs_new_entity(world, "e3");

    ecs_add_pair(world, e1, RelA, TgtA);
    ecs_add_pair(world, e2, RelA, TgtA);
    ecs_add_pair(world, e3, RelA, TgtA);

    ecs_add_pair(world, e1, RelB, TgtA);
    ecs_add_pair(world, e2, RelB, TgtA);

    ecs_iter_t it = ecs_rule_iter(world, q);
    char *json = ecs_iter_to_json(world, &it, &(ecs_iter_to_json_desc_t) {
        .serialize_rows = true
    });
    test_assert(json != NULL);

    char* expect = "{\"results\":["
        "{\"name\":\"e3\", \"pairs\":{\"RelA\":\"TgtA\"},\"vars\":{\"var\":\"*\"},\"is_set\":[true, false]}]}";
    test_str(json, expect);

    ecs_os_free(json);

    ecs_rule_fini(q);

    ecs_fini(world);
}

void SerializeIterToRowJson_serialize_not_pair_var_constrained(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_rule_t *q = ecs_rule(world, {
        .expr = "(RelA, $var), !(RelB, $var)"
    });

    test_assert(q != NULL);

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_entity_t e2 = ecs_new_entity(world, "e2");
    ecs_entity_t e3 = ecs_new_entity(world, "e3");

    ecs_add_pair(world, e1, RelA, TgtA);
    ecs_add_pair(world, e2, RelA, TgtA);
    ecs_add_pair(world, e3, RelA, TgtA);

    ecs_add_pair(world, e1, RelB, TgtB);
    ecs_add_pair(world, e2, RelB, TgtB);
    ecs_add_pair(world, e3, RelB, TgtA);

    ecs_iter_t it = ecs_rule_iter(world, q);
    char *json = ecs_iter_to_json(world, &it, &(ecs_iter_to_json_desc_t) {
        .serialize_rows = true
    });
    test_assert(json != NULL);

    char* expect = "{\"results\":["
        "{\"name\":\"e1\", \"pairs\":{\"RelA\":\"TgtA\"},\"vars\":{\"var\":\"TgtA\"},\"is_set\":[true, false]}, "
        "{\"name\":\"e2\", \"pairs\":{\"RelA\":\"TgtA\"},\"vars\":{\"var\":\"TgtA\"},\"is_set\":[true, false]}]}";
    test_str(json, expect);

    ecs_os_free(json);

    ecs_rule_fini(q);

    ecs_fini(world);
}

void SerializeIterToRowJson_serialize_optional(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Mass);

    ecs_struct(world, {
        .entity = ecs_id(Position),
        .members = {
            { "x", ecs_id(ecs_i32_t) },
            { "y", ecs_id(ecs_i32_t) }
        }
    });

    ecs_struct(world, {
        .entity = ecs_id(Mass),
        .members = {
            { "value", ecs_id(ecs_i32_t) },
        }
    });


    ecs_rule_t *q = ecs_rule(world, {
        .expr = "Position, ?Mass"
    });

    test_assert(q != NULL);

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_entity_t e2 = ecs_new_entity(world, "e2");
    ecs_entity_t e3 = ecs_new_entity(world, "e3");

    ecs_set(world, e1, Position, {10, 20});
    ecs_set(world, e2, Position, {20, 30});
    ecs_set(world, e3, Position, {30, 40});

    ecs_set(world, e1, Mass, {1});
    ecs_set(world, e2, Mass, {2});

    ecs_iter_t it = ecs_rule_iter(world, q);
    char *json = ecs_iter_to_json(world, &it, &(ecs_iter_to_json_desc_t) {
        .serialize_rows = true
    });
    test_assert(json != NULL);

    char* expect = "{\"results\":["
        "{\"name\":\"e3\", \"is_set\":[true, false], \"components\":{\"Position\":{\"x\":30, \"y\":40}}}, "
        "{\"name\":\"e1\", \"is_set\":[true, true], \"components\":{\"Position\":{\"x\":10, \"y\":20}, \"Mass\":{\"value\":1}}}, "
        "{\"name\":\"e2\", \"is_set\":[true, true], \"components\":{\"Position\":{\"x\":20, \"y\":30}, \"Mass\":{\"value\":2}}}]}";
    test_str(json, expect);

    ecs_os_free(json);

    ecs_rule_fini(q);

    ecs_fini(world);
}

void SerializeIterToRowJson_serialize_optional_pair_wildcard(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_rule_t *q = ecs_rule(world, {
        .expr = "(RelA, *), ?(RelB, *)"
    });

    test_assert(q != NULL);

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_entity_t e2 = ecs_new_entity(world, "e2");
    ecs_entity_t e3 = ecs_new_entity(world, "e3");

    ecs_add_pair(world, e1, RelA, TgtA);
    ecs_add_pair(world, e2, RelA, TgtA);
    ecs_add_pair(world, e3, RelA, TgtA);

    ecs_add_pair(world, e1, RelB, TgtA);
    ecs_add_pair(world, e2, RelB, TgtA);

    ecs_iter_t it = ecs_rule_iter(world, q);
    char *json = ecs_iter_to_json(world, &it, &(ecs_iter_to_json_desc_t) {
        .serialize_rows = true
    });
    test_assert(json != NULL);

    char* expect = "{\"results\":["
        "{\"name\":\"e3\", \"pairs\":{\"RelA\":\"TgtA\"},\"is_set\":[true, false]}, "
        "{\"name\":\"e1\", \"pairs\":{\"RelA\":\"TgtA\", \"RelB\":\"TgtA\"},\"is_set\":[true, true]}, "
        "{\"name\":\"e2\", \"pairs\":{\"RelA\":\"TgtA\", \"RelB\":\"TgtA\"},\"is_set\":[true, true]}]}";
    test_str(json, expect);

    ecs_os_free(json);

    ecs_rule_fini(q);

    ecs_fini(world);
}

void SerializeIterToRowJson_serialize_optional_pair_var(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_rule_t *q = ecs_rule(world, {
        .expr = "(RelA, *), ?(RelB, $var)"
    });

    test_assert(q != NULL);

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_entity_t e2 = ecs_new_entity(world, "e2");
    ecs_entity_t e3 = ecs_new_entity(world, "e3");

    ecs_add_pair(world, e1, RelA, TgtA);
    ecs_add_pair(world, e2, RelA, TgtA);
    ecs_add_pair(world, e3, RelA, TgtA);

    ecs_add_pair(world, e1, RelB, TgtA);
    ecs_add_pair(world, e2, RelB, TgtA);

    ecs_iter_t it = ecs_rule_iter(world, q);
    char *json = ecs_iter_to_json(world, &it, &(ecs_iter_to_json_desc_t) {
        .serialize_rows = true
    });
    test_assert(json != NULL);

    char* expect = "{\"results\":["
        "{\"name\":\"e3\", \"pairs\":{\"RelA\":\"TgtA\"},\"vars\":{\"var\":\"*\"},\"is_set\":[true, false]}, "
        "{\"name\":\"e1\", \"pairs\":{\"RelA\":\"TgtA\", \"RelB\":\"TgtA\"},\"vars\":{\"var\":\"TgtA\"},\"is_set\":[true, true]}, "
        "{\"name\":\"e2\", \"pairs\":{\"RelA\":\"TgtA\", \"RelB\":\"TgtA\"},\"vars\":{\"var\":\"TgtA\"},\"is_set\":[true, true]}]}";
    test_str(json, expect);

    ecs_os_free(json);

    ecs_rule_fini(q);

    ecs_fini(world);
}

void SerializeIterToRowJson_serialize_optional_pair_var_constrained(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_rule_t *q = ecs_rule(world, {
        .expr = "(RelA, $var), ?(RelB, $var)"
    });

    test_assert(q != NULL);

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_entity_t e2 = ecs_new_entity(world, "e2");
    ecs_entity_t e3 = ecs_new_entity(world, "e3");

    ecs_add_pair(world, e1, RelA, TgtA);
    ecs_add_pair(world, e2, RelA, TgtA);
    ecs_add_pair(world, e3, RelA, TgtA);

    ecs_add_pair(world, e1, RelB, TgtB);
    ecs_add_pair(world, e2, RelB, TgtB);
    ecs_add_pair(world, e3, RelB, TgtA);

    ecs_iter_t it = ecs_rule_iter(world, q);
    char *json = ecs_iter_to_json(world, &it, &(ecs_iter_to_json_desc_t) {
        .serialize_rows = true
    });
    test_assert(json != NULL);

    char* expect = "{\"results\":["
        "{\"name\":\"e1\", \"pairs\":{\"RelA\":\"TgtA\"},\"vars\":{\"var\":\"TgtA\"},\"is_set\":[true, false]}, "
        "{\"name\":\"e2\", \"pairs\":{\"RelA\":\"TgtA\"},\"vars\":{\"var\":\"TgtA\"},\"is_set\":[true, false]}, "
        "{\"name\":\"e3\", \"pairs\":{\"RelA\":\"TgtA\", \"RelB\":\"TgtA\"},\"vars\":{\"var\":\"TgtA\"},\"is_set\":[true, true]}]}";
    test_str(json, expect);

    ecs_os_free(json);

    ecs_rule_fini(q);

    ecs_fini(world);
}

void SerializeIterToRowJson_serialize_or(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Mass);

    ecs_struct(world, {
        .entity = ecs_id(Position),
        .members = {
            { "x", ecs_id(ecs_i32_t) },
            { "y", ecs_id(ecs_i32_t) }
        }
    });

    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_rule_t *q = ecs_rule(world, {
        .expr = "(Position, TgtA) || (Position, TgtB)"
    });

    test_assert(q != NULL);

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_entity_t e2 = ecs_new_entity(world, "e2");
    ecs_entity_t e3 = ecs_new_entity(world, "e3");

    ecs_set_pair(world, e1, Position, TgtA, {10, 20});
    ecs_set_pair(world, e2, Position, TgtA, {20, 30});
    ecs_set_pair(world, e3, Position, TgtB, {30, 40});

    ecs_iter_t it = ecs_rule_iter(world, q);
    char *json = ecs_iter_to_json(world, &it, &(ecs_iter_to_json_desc_t) {
        .serialize_rows = true
    });
    test_assert(json != NULL);

    char* expect = "{\"results\":["
        "{\"name\":\"e1\", \"pairs\":{\"Position\":\"TgtA\"}, \"components\":{\"(Position,TgtA)\":{\"x\":10, \"y\":20}}}, "
        "{\"name\":\"e2\", \"pairs\":{\"Position\":\"TgtA\"}, \"components\":{\"(Position,TgtA)\":{\"x\":20, \"y\":30}}}, "
        "{\"name\":\"e3\", \"pairs\":{\"Position\":\"TgtB\"}, \"components\":{\"(Position,TgtA)\":{\"x\":30, \"y\":40}}}]}";
    test_str(json, expect);

    ecs_os_free(json);

    ecs_rule_fini(q);

    ecs_fini(world);
}

void SerializeIterToRowJson_serialize_scope(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_rule_t *q = ecs_rule(world, {
        .expr = "TagA, !{ChildOf($child, $this), TagB($child)}, TagB"
    });

    test_assert(q != NULL);

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_entity_t e2 = ecs_new_entity(world, "e2");
    ecs_entity_t e3 = ecs_new_entity(world, "e3");

    ecs_add(world, e1, TagA);
    ecs_add(world, e2, TagA);
    ecs_add(world, e3, TagA);

    ecs_add(world, e1, TagB);
    ecs_add(world, e2, TagB);
    ecs_add(world, e3, TagB);

    ecs_entity_t child_e1_1 = ecs_new_entity(world, "child_1");
    ecs_add_pair(world, child_e1_1, EcsChildOf, e1);

    ecs_entity_t child_e2_1 = ecs_new_entity(world, "child_1");
    ecs_add_pair(world, child_e2_1, EcsChildOf, e2);

    ecs_entity_t child_e2_2 = ecs_new_entity(world, "child_2");
    ecs_add_pair(world, child_e2_2, EcsChildOf, e2);
    ecs_add(world, child_e2_2, TagB);

    ecs_iter_t it = ecs_rule_iter(world, q);
    char *json = ecs_iter_to_json(world, &it, &(ecs_iter_to_json_desc_t) {
        .serialize_rows = true
    });
    test_assert(json != NULL);

    /* TODO: this result is a bit wonky because rules don't correctly handle
     *       hiding fields within scopes yet. This test is just here to make
     *       the serializer doesn't crash, and will be updated after the query
     *       refactor. */
    char* expect = "{\"results\":["
        "{\"name\":\"e1\", \"tags\":[\"TagA\", \"TagB\"],\"pairs\":{\"ChildOf\":\"e1\"},\"vars\":{\"child\":\"*\"},\"is_set\":[true, false, true, false, false, true]}, "
        "{\"name\":\"e3\", \"tags\":[\"TagA\", \"TagB\", \"TagB\"],\"pairs\":{\"ChildOf\":\"e2\"},\"vars\":{\"child\":\"*\"},\"is_set\":[true, false, true, true, false, true]}]}";
    test_str(json, expect);

    ecs_os_free(json);

    ecs_rule_fini(q);

    ecs_fini(world);
}

void SerializeIterToRowJson_serialize_eq(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_entity_t e2 = ecs_new_entity(world, "e2");
    ecs_entity_t e3 = ecs_new_entity(world, "e3");

    ecs_rule_t *q = ecs_rule(world, {
        .expr = "TagA, $this == e2"
    });

    test_assert(q != NULL);

    ecs_add(world, e1, TagA);
    ecs_add(world, e2, TagA);
    ecs_add(world, e3, TagA);
    ecs_add(world, e3, TagB);

    ecs_iter_t it = ecs_rule_iter(world, q);
    char *json = ecs_iter_to_json(world, &it, &(ecs_iter_to_json_desc_t) {
        .serialize_rows = true
    });
    test_assert(json != NULL);

    char* expect = "{\"results\":[{\"name\":\"e2\", \"tags\":[\"TagA\"]}]}";
    test_str(json, expect);

    ecs_os_free(json);

    ecs_rule_fini(q);

    ecs_fini(world);
}

void SerializeIterToRowJson_serialize_neq(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_entity_t e2 = ecs_new_entity(world, "e2");
    ecs_entity_t e3 = ecs_new_entity(world, "e3");

    ecs_rule_t *q = ecs_rule(world, {
        .expr = "TagA, $this != e2"
    });

    test_assert(q != NULL);

    ecs_add(world, e1, TagA);
    ecs_add(world, e2, TagA);
    ecs_add(world, e3, TagA);
    ecs_add(world, e3, TagB);

    ecs_iter_t it = ecs_rule_iter(world, q);
    char *json = ecs_iter_to_json(world, &it, &(ecs_iter_to_json_desc_t) {
        .serialize_rows = true
    });
    test_assert(json != NULL);

    char* expect = "{\"results\":["
        "{\"name\":\"e1\", \"tags\":[\"TagA\"],\"is_set\":[true, false]}, "
        "{\"name\":\"e3\", \"tags\":[\"TagA\"],\"is_set\":[true, false]}]}";
    test_str(json, expect);

    ecs_os_free(json);

    ecs_rule_fini(q);

    ecs_fini(world);
}

void SerializeIterToRowJson_serialize_eq_m(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_entity_t e2 = ecs_new_entity(world, "e2");
    ecs_entity_t e3 = ecs_new_entity(world, "e3");

    ecs_rule_t *q = ecs_rule(world, {
        .expr = "TagA, $this ~= \"e2\""
    });

    test_assert(q != NULL);

    ecs_add(world, e1, TagA);
    ecs_add(world, e2, TagA);
    ecs_add(world, e3, TagA);
    ecs_add(world, e3, TagB);

    ecs_iter_t it = ecs_rule_iter(world, q);
    char *json = ecs_iter_to_json(world, &it, &(ecs_iter_to_json_desc_t) {
        .serialize_rows = true
    });
    test_assert(json != NULL);

    char* expect = "{\"results\":[{\"name\":\"e2\", \"tags\":[\"TagA\"]}]}";
    test_str(json, expect);

    ecs_os_free(json);

    ecs_rule_fini(q);

    ecs_fini(world);
}

void SerializeIterToRowJson_serialize_table(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Mass);

    ecs_struct(world, {
        .entity = ecs_id(Position),
        .members = {
            { "x", ecs_id(ecs_i32_t) },
            { "y", ecs_id(ecs_i32_t) }
        }
    });

    ecs_struct(world, {
        .entity = ecs_id(Mass),
        .members = {
            { "value", ecs_id(ecs_i32_t) },
        }
    });

    ecs_rule_t *q = ecs_rule(world, {
        .expr = "TagA"
    });

    test_assert(q != NULL);

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_entity_t e2 = ecs_new_entity(world, "e2");
    ecs_entity_t e3 = ecs_new_entity(world, "e3");

    ecs_add(world, e1, TagA);
    ecs_add(world, e2, TagA);
    ecs_add(world, e3, TagA);

    ecs_add(world, e1, TagB);
    ecs_add(world, e2, TagB);
    ecs_add(world, e3, TagB);

    ecs_add_pair(world, e1, RelA, TgtA);
    ecs_add_pair(world, e2, RelA, TgtA);
    ecs_add_pair(world, e3, RelA, TgtA);

    ecs_add_pair(world, e1, RelB, TgtA);
    ecs_add_pair(world, e2, RelB, TgtA);
    ecs_add_pair(world, e3, RelB, TgtB);

    ecs_set(world, e1, Position, {10, 20});
    ecs_set(world, e2, Position, {20, 30});
    ecs_set(world, e3, Position, {30, 40});

    ecs_set(world, e1, Mass, {1});
    ecs_set(world, e2, Mass, {2});
    ecs_set(world, e3, Mass, {3});

    ecs_iter_t it = ecs_rule_iter(world, q);
    char *json = ecs_iter_to_json(world, &it, &(ecs_iter_to_json_desc_t) {
        .serialize_table = true,
        .serialize_rows = true
    });
    test_assert(json != NULL);

    char* expect = "{\"results\":["
        "{\"name\":\"e1\", \"tags\":[\"TagA\", \"TagB\"],\"pairs\":{\"RelA\":\"TgtA\", \"RelB\":\"TgtA\"}, \"components\":{\"Position\":{\"x\":10, \"y\":20}, \"Mass\":{\"value\":1}}}, "
        "{\"name\":\"e2\", \"tags\":[\"TagA\", \"TagB\"],\"pairs\":{\"RelA\":\"TgtA\", \"RelB\":\"TgtA\"}, \"components\":{\"Position\":{\"x\":20, \"y\":30}, \"Mass\":{\"value\":2}}}, "
        "{\"name\":\"e3\", \"tags\":[\"TagA\", \"TagB\"],\"pairs\":{\"RelA\":\"TgtA\", \"RelB\":\"TgtB\"}, \"components\":{\"Position\":{\"x\":30, \"y\":40}, \"Mass\":{\"value\":3}}}]}";
    test_str(json, expect);

    ecs_os_free(json);

    ecs_rule_fini(q);

    ecs_fini(world);
}

void SerializeIterToRowJson_serialize_table_w_eq(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Mass);

    ecs_struct(world, {
        .entity = ecs_id(Position),
        .members = {
            { "x", ecs_id(ecs_i32_t) },
            { "y", ecs_id(ecs_i32_t) }
        }
    });

    ecs_struct(world, {
        .entity = ecs_id(Mass),
        .members = {
            { "value", ecs_id(ecs_i32_t) },
        }
    });

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_entity_t e2 = ecs_new_entity(world, "e2");
    ecs_entity_t e3 = ecs_new_entity(world, "e3");

    ecs_rule_t *q = ecs_rule(world, {
        .expr = "TagA, $this == e2"
    });

    test_assert(q != NULL);

    ecs_add(world, e1, TagA);
    ecs_add(world, e2, TagA);
    ecs_add(world, e3, TagA);

    ecs_add(world, e1, TagB);
    ecs_add(world, e2, TagB);
    ecs_add(world, e3, TagB);

    ecs_add_pair(world, e1, RelA, TgtA);
    ecs_add_pair(world, e2, RelA, TgtA);
    ecs_add_pair(world, e3, RelA, TgtA);

    ecs_add_pair(world, e1, RelB, TgtA);
    ecs_add_pair(world, e2, RelB, TgtA);
    ecs_add_pair(world, e3, RelB, TgtB);

    ecs_set(world, e1, Position, {10, 20});
    ecs_set(world, e2, Position, {20, 30});
    ecs_set(world, e3, Position, {30, 40});

    ecs_set(world, e1, Mass, {1});
    ecs_set(world, e2, Mass, {2});
    ecs_set(world, e3, Mass, {3});

    ecs_iter_t it = ecs_rule_iter(world, q);
    char *json = ecs_iter_to_json(world, &it, &(ecs_iter_to_json_desc_t) {
        .serialize_table = true,
        .serialize_rows = true
    });
    test_assert(json != NULL);

    char* expect = "{\"results\":["
        "{\"name\":\"e2\", "
            "\"tags\":[\"TagA\", \"TagB\"],"
            "\"pairs\":{\"RelA\":\"TgtA\", \"RelB\":\"TgtA\"}, "
            "\"components\":{"
                "\"Position\":{\"x\":20, \"y\":30}, "
                "\"Mass\":{\"value\":2}}}]}";
    test_str(json, expect);
    ecs_os_free(json);

    ecs_rule_fini(q);

    ecs_fini(world);
}

void SerializeIterToRowJson_serialize_table_w_neq(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Mass);

    ecs_struct(world, {
        .entity = ecs_id(Position),
        .members = {
            { "x", ecs_id(ecs_i32_t) },
            { "y", ecs_id(ecs_i32_t) }
        }
    });

    ecs_struct(world, {
        .entity = ecs_id(Mass),
        .members = {
            { "value", ecs_id(ecs_i32_t) },
        }
    });

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_entity_t e2 = ecs_new_entity(world, "e2");
    ecs_entity_t e3 = ecs_new_entity(world, "e3");

    ecs_rule_t *q = ecs_rule(world, {
        .expr = "TagA, $this != e2"
    });

    test_assert(q != NULL);

    ecs_add(world, e1, TagA);
    ecs_add(world, e2, TagA);
    ecs_add(world, e3, TagA);

    ecs_add(world, e1, TagB);
    ecs_add(world, e2, TagB);
    ecs_add(world, e3, TagB);

    ecs_add_pair(world, e1, RelA, TgtA);
    ecs_add_pair(world, e2, RelA, TgtA);
    ecs_add_pair(world, e3, RelA, TgtA);

    ecs_add_pair(world, e1, RelB, TgtA);
    ecs_add_pair(world, e2, RelB, TgtA);
    ecs_add_pair(world, e3, RelB, TgtB);

    ecs_set(world, e1, Position, {10, 20});
    ecs_set(world, e2, Position, {20, 30});
    ecs_set(world, e3, Position, {30, 40});

    ecs_set(world, e1, Mass, {1});
    ecs_set(world, e2, Mass, {2});
    ecs_set(world, e3, Mass, {3});

    ecs_iter_t it = ecs_rule_iter(world, q);
    char *json = ecs_iter_to_json(world, &it, &(ecs_iter_to_json_desc_t) {
        .serialize_table = true,
        .serialize_rows = true
    });
    test_assert(json != NULL);

    char* expect = "{\"results\":["
        "{\"name\":\"e1\", \"tags\":[\"TagA\", \"TagB\"],\"pairs\":{\"RelA\":\"TgtA\", \"RelB\":\"TgtA\"}, \"components\":{\"Position\":{\"x\":10, \"y\":20}, \"Mass\":{\"value\":1}}}, "
        "{\"name\":\"e3\", \"tags\":[\"TagA\", \"TagB\"],\"pairs\":{\"RelA\":\"TgtA\", \"RelB\":\"TgtB\"}, \"components\":{\"Position\":{\"x\":30, \"y\":40}, \"Mass\":{\"value\":3}}}]}";
    test_str(json, expect);

    ecs_os_free(json);
    
    ecs_rule_fini(q);

    ecs_fini(world);
}

void SerializeIterToRowJson_serialize_table_w_2_pair_targets(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);
    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_rule_t *q = ecs_rule(world, {
        .expr = "TagA"
    });

    test_assert(q != NULL);

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_entity_t e2 = ecs_new_entity(world, "e2");
    ecs_entity_t e3 = ecs_new_entity(world, "e3");

    ecs_add(world, e1, TagA);
    ecs_add(world, e2, TagA);
    ecs_add(world, e3, TagA);

    ecs_add_pair(world, e1, RelA, TgtA);
    ecs_add_pair(world, e2, RelA, TgtA);
    ecs_add_pair(world, e3, RelA, TgtA);
    ecs_add_pair(world, e3, RelA, TgtB);

    ecs_iter_t it = ecs_rule_iter(world, q);
    char *json = ecs_iter_to_json(world, &it, &(ecs_iter_to_json_desc_t) {
        .serialize_table = true,
        .serialize_rows = true
    });
    test_assert(json != NULL);

    char* expect = "{\"results\":["
        "{\"name\":\"e1\", \"tags\":[\"TagA\"],\"pairs\":{\"RelA\":\"TgtA\"}}, "
        "{\"name\":\"e2\", \"tags\":[\"TagA\"],\"pairs\":{\"RelA\":\"TgtA\"}}, "
        "{\"name\":\"e3\", \"tags\":[\"TagA\"],\"pairs\":{\"RelA\":[\"TgtA\", \"TgtB\"]}}]}";
    test_str(json, expect);

    ecs_os_free(json);

    ecs_rule_fini(q);

    ecs_fini(world);
}

void SerializeIterToRowJson_serialize_table_w_2_pair_targets_2_rel(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);
    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);
    ECS_TAG(world, TgtC);

    ecs_rule_t *q = ecs_rule(world, {
        .expr = "TagA"
    });

    test_assert(q != NULL);

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_entity_t e2 = ecs_new_entity(world, "e2");
    ecs_entity_t e3 = ecs_new_entity(world, "e3");

    ecs_add(world, e1, TagA);
    ecs_add(world, e2, TagA);
    ecs_add(world, e3, TagA);

    ecs_add_pair(world, e1, RelA, TgtA);
    ecs_add_pair(world, e2, RelA, TgtA);
    ecs_add_pair(world, e3, RelA, TgtA);
    ecs_add_pair(world, e3, RelA, TgtB);

    ecs_add_pair(world, e3, RelB, TgtB);
    ecs_add_pair(world, e3, RelB, TgtC);

    ecs_iter_t it = ecs_rule_iter(world, q);
    char *json = ecs_iter_to_json(world, &it, &(ecs_iter_to_json_desc_t) {
        .serialize_table = true,
        .serialize_rows = true
    });
    test_assert(json != NULL);

    char* expect = "{\"results\":["
        "{\"name\":\"e1\", \"tags\":[\"TagA\"],\"pairs\":{\"RelA\":\"TgtA\"}}, "
        "{\"name\":\"e2\", \"tags\":[\"TagA\"],\"pairs\":{\"RelA\":\"TgtA\"}}, "
        "{\"name\":\"e3\", \"tags\":[\"TagA\"],\"pairs\":{\"RelA\":[\"TgtA\", \"TgtB\"], \"RelB\":[\"TgtB\", \"TgtC\"]}}]}";
    test_str(json, expect);

    ecs_os_free(json);

    ecs_rule_fini(q);

    ecs_fini(world);
}

void SerializeIterToRowJson_serialize_table_w_3_pair_targets(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);
    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);
    ECS_TAG(world, TgtC);
    ECS_TAG(world, TgtD);

    ecs_rule_t *q = ecs_rule(world, {
        .expr = "TagA"
    });

    test_assert(q != NULL);

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_entity_t e2 = ecs_new_entity(world, "e2");
    ecs_entity_t e3 = ecs_new_entity(world, "e3");

    ecs_add(world, e1, TagA);
    ecs_add(world, e2, TagA);
    ecs_add(world, e3, TagA);

    ecs_add_pair(world, e1, RelA, TgtA);

    ecs_add_pair(world, e2, RelA, TgtA);
    ecs_add_pair(world, e2, RelA, TgtB);

    ecs_add_pair(world, e3, RelA, TgtB);
    ecs_add_pair(world, e3, RelA, TgtC);
    ecs_add_pair(world, e3, RelA, TgtD);

    ecs_iter_t it = ecs_rule_iter(world, q);
    char *json = ecs_iter_to_json(world, &it, &(ecs_iter_to_json_desc_t) {
        .serialize_table = true,
        .serialize_rows = true
    });
    test_assert(json != NULL);

    char* expect = "{\"results\":["
        "{\"name\":\"e1\", \"tags\":[\"TagA\"],\"pairs\":{\"RelA\":\"TgtA\"}}, "
        "{\"name\":\"e2\", \"tags\":[\"TagA\"],\"pairs\":{\"RelA\":[\"TgtA\", \"TgtB\"]}}, "
        "{\"name\":\"e3\", \"tags\":[\"TagA\"],\"pairs\":{\"RelA\":[\"TgtB\", \"TgtC\", \"TgtD\"]}}]}";
    test_str(json, expect);

    ecs_os_free(json);

    ecs_rule_fini(q);

    ecs_fini(world);
}

void SerializeIterToRowJson_serialize_table_w_3_pair_targets_2_rel(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);
    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);
    ECS_TAG(world, TgtC);
    ECS_TAG(world, TgtD);

    ecs_rule_t *q = ecs_rule(world, {
        .expr = "TagA"
    });

    test_assert(q != NULL);

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_entity_t e2 = ecs_new_entity(world, "e2");
    ecs_entity_t e3 = ecs_new_entity(world, "e3");

    ecs_add(world, e1, TagA);
    ecs_add(world, e2, TagA);
    ecs_add(world, e3, TagA);

    ecs_add_pair(world, e1, RelA, TgtA);

    ecs_add_pair(world, e2, RelA, TgtA);
    ecs_add_pair(world, e2, RelA, TgtB);

    ecs_add_pair(world, e3, RelA, TgtA);
    ecs_add_pair(world, e3, RelA, TgtB);
    ecs_add_pair(world, e3, RelA, TgtC);

    ecs_add_pair(world, e3, RelB, TgtB);
    ecs_add_pair(world, e3, RelB, TgtC);
    ecs_add_pair(world, e3, RelB, TgtD);

    ecs_iter_t it = ecs_rule_iter(world, q);
    char *json = ecs_iter_to_json(world, &it, &(ecs_iter_to_json_desc_t) {
        .serialize_table = true,
        .serialize_rows = true
    });
    test_assert(json != NULL);

    char* expect = "{\"results\":["
        "{\"name\":\"e1\", \"tags\":[\"TagA\"],\"pairs\":{\"RelA\":\"TgtA\"}}, "
        "{\"name\":\"e2\", \"tags\":[\"TagA\"],\"pairs\":{\"RelA\":[\"TgtA\", \"TgtB\"]}}, "
        "{\"name\":\"e3\", \"tags\":[\"TagA\"],\"pairs\":{\"RelA\":[\"TgtA\", \"TgtB\", \"TgtC\"], \"RelB\":[\"TgtB\", \"TgtC\", \"TgtD\"]}}]}";
    test_str(json, expect);

    ecs_os_free(json);

    ecs_rule_fini(q);

    ecs_fini(world);
}

void SerializeIterToRowJson_serialize_everything(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Mass);

    ecs_struct(world, {
        .entity = ecs_id(Position),
        .members = {
            { "x", ecs_id(ecs_i32_t) },
            { "y", ecs_id(ecs_i32_t) }
        }
    });

    ecs_struct(world, {
        .entity = ecs_id(Mass),
        .members = {
            { "value", ecs_id(ecs_i32_t) },
        }
    });

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_entity_t e2 = ecs_new_entity(world, "e2");
    ecs_entity_t e3 = ecs_new_entity(world, "e3");

    ecs_rule_t *q = ecs_rule(world, {
        .expr = "_"
    });

    test_assert(q != NULL);

    ecs_add(world, e1, TagA);
    ecs_add(world, e2, TagA);
    ecs_add(world, e3, TagA);

    ecs_add(world, e1, TagB);
    ecs_add(world, e2, TagB);
    ecs_add(world, e3, TagB);

    ecs_add_pair(world, e1, RelA, TgtA);
    ecs_add_pair(world, e2, RelA, TgtA);
    ecs_add_pair(world, e3, RelA, TgtA);

    ecs_add_pair(world, e1, RelB, TgtA);
    ecs_add_pair(world, e2, RelB, TgtA);
    ecs_add_pair(world, e3, RelB, TgtB);

    ecs_set(world, e1, Position, {10, 20});
    ecs_set(world, e2, Position, {20, 30});
    ecs_set(world, e3, Position, {30, 40});

    ecs_set(world, e1, Mass, {1});
    ecs_set(world, e2, Mass, {2});
    ecs_set(world, e3, Mass, {3});

    ecs_iter_t it = ecs_rule_iter(world, q);
    char *json = ecs_iter_to_json(world, &it, &(ecs_iter_to_json_desc_t) {
        .serialize_rows = true
    });
    test_assert(json != NULL);
    /* JSON string would change too often to match exactly. This test is more
     * intended as a catch all, to make sure the serializer doesn't crash. */
    ecs_os_free(json);
    
    ecs_rule_fini(q);

    ecs_fini(world);
}

void SerializeIterToRowJson_serialize_everything_table(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Mass);

    ecs_add_id(world, TagA, EcsPrivate);

    ecs_struct(world, {
        .entity = ecs_id(Position),
        .members = {
            { "x", ecs_id(ecs_i32_t) },
            { "y", ecs_id(ecs_i32_t) }
        }
    });

    ecs_struct(world, {
        .entity = ecs_id(Mass),
        .members = {
            { "value", ecs_id(ecs_i32_t) },
        }
    });

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_entity_t e2 = ecs_new_entity(world, "e2");
    ecs_entity_t e3 = ecs_new_entity(world, "e3");

    ecs_rule_t *q = ecs_rule(world, {
        .expr = "_"
    });

    test_assert(q != NULL);

    ecs_add(world, e1, TagA);
    ecs_add(world, e2, TagA);
    ecs_add(world, e3, TagA);

    ecs_add(world, e1, TagB);
    ecs_add(world, e2, TagB);
    ecs_add(world, e3, TagB);

    ecs_add_pair(world, e1, RelA, TgtA);
    ecs_add_pair(world, e2, RelA, TgtA);
    ecs_add_pair(world, e3, RelA, TgtA);

    ecs_add_pair(world, e1, RelB, TgtA);
    ecs_add_pair(world, e2, RelB, TgtA);
    ecs_add_pair(world, e3, RelB, TgtB);

    ecs_set(world, e1, Position, {10, 20});
    ecs_set(world, e2, Position, {20, 30});
    ecs_set(world, e3, Position, {30, 40});

    ecs_set(world, e1, Mass, {1});
    ecs_set(world, e2, Mass, {2});
    ecs_set(world, e3, Mass, {3});

    ecs_iter_t it = ecs_rule_iter(world, q);
    char *json = ecs_iter_to_json(world, &it, &(ecs_iter_to_json_desc_t) {
        .serialize_rows = true,
        .serialize_table = true
    });
    test_assert(json != NULL);
    /* JSON string would change too often to match exactly. This test is more
     * intended as a catch all, to make sure the serializer doesn't crash. */
    test_assert(!strstr(json, "\"ChildOf\":")); /* No private components */
    test_assert(!strstr(json, "\"Identifier\":"));
    test_assert(!strstr(json, "\"Poly\":"));
    test_assert(!strstr(json, "[\"TagA\""));
    ecs_os_free(json);

    ecs_rule_fini(q);

    ecs_fini(world);
}

void SerializeIterToRowJson_serialize_everything_table_w_private(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Mass);

    ecs_add_id(world, TagA, EcsPrivate);

    ecs_struct(world, {
        .entity = ecs_id(Position),
        .members = {
            { "x", ecs_id(ecs_i32_t) },
            { "y", ecs_id(ecs_i32_t) }
        }
    });

    ecs_struct(world, {
        .entity = ecs_id(Mass),
        .members = {
            { "value", ecs_id(ecs_i32_t) },
        }
    });

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_entity_t e2 = ecs_new_entity(world, "e2");
    ecs_entity_t e3 = ecs_new_entity(world, "e3");

    ecs_rule_t *q = ecs_rule(world, {
        .expr = "_"
    });

    test_assert(q != NULL);

    ecs_add(world, e1, TagA);
    ecs_add(world, e2, TagA);
    ecs_add(world, e3, TagA);

    ecs_add(world, e1, TagB);
    ecs_add(world, e2, TagB);
    ecs_add(world, e3, TagB);

    ecs_add_pair(world, e1, RelA, TgtA);
    ecs_add_pair(world, e2, RelA, TgtA);
    ecs_add_pair(world, e3, RelA, TgtA);

    ecs_add_pair(world, e1, RelB, TgtA);
    ecs_add_pair(world, e2, RelB, TgtA);
    ecs_add_pair(world, e3, RelB, TgtB);

    ecs_set(world, e1, Position, {10, 20});
    ecs_set(world, e2, Position, {20, 30});
    ecs_set(world, e3, Position, {30, 40});

    ecs_set(world, e1, Mass, {1});
    ecs_set(world, e2, Mass, {2});
    ecs_set(world, e3, Mass, {3});

    ecs_iter_t it = ecs_rule_iter(world, q);
    char *json = ecs_iter_to_json(world, &it, &(ecs_iter_to_json_desc_t) {
        .serialize_rows = true,
        .serialize_table = true,
        .serialize_private = true
    });
    test_assert(json != NULL);
    // printf("%s\n", json);
    /* JSON string would change too often to match exactly. This test is more
     * intended as a catch all, to make sure the serializer doesn't crash. */
    test_assert(strstr(json, "\"ChildOf\":") != NULL); /* Private components */
    test_assert(strstr(json, "\"Identifier\":") != NULL);
    test_assert(strstr(json, "\"Poly\":") != NULL);
    test_assert(strstr(json, "[\"TagA\"") != NULL);
    ecs_os_free(json);
    
    ecs_rule_fini(q);

    ecs_fini(world);
}

void SerializeIterToRowJson_serialize_w_type_info(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_struct(world, {
        .entity = ecs_id(Position),
        .members = {
            { "x", ecs_id(ecs_i32_t) },
            { "y", ecs_id(ecs_i32_t) }
        }
    });

    ecs_rule_t *q = ecs_rule(world, {
        .expr = "Position"
    });

    test_assert(q != NULL);

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_entity_t e2 = ecs_new_entity(world, "e2");
    ecs_entity_t e3 = ecs_new_entity(world, "e3");

    ecs_set(world, e1, Position, {10, 20});
    ecs_set(world, e2, Position, {20, 30});
    ecs_set(world, e3, Position, {30, 40});

    ecs_iter_t it = ecs_rule_iter(world, q);
    char *json = ecs_iter_to_json(world, &it, &(ecs_iter_to_json_desc_t) {
        .serialize_rows = true,
        .serialize_type_info = true
    });
    test_assert(json != NULL);

    char* expect = "{\"type_info\":{\"Position\":{\"x\":[\"int\"], \"y\":[\"int\"]}}, "
        "\"results\":["
            "{\"name\":\"e1\", \"components\":{\"Position\":{\"x\":10, \"y\":20}}}, "
            "{\"name\":\"e2\", \"components\":{\"Position\":{\"x\":20, \"y\":30}}}, "
            "{\"name\":\"e3\", \"components\":{\"Position\":{\"x\":30, \"y\":40}}}]}";
    
    test_str(json, expect);

    ecs_os_free(json);

    ecs_rule_fini(q);

    ecs_fini(world);
}

void SerializeIterToRowJson_serialize_w_field_info(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Mass);

    ecs_struct(world, {
        .entity = ecs_id(Position),
        .members = {
            { "x", ecs_id(ecs_i32_t) },
            { "y", ecs_id(ecs_i32_t) }
        }
    });

    ecs_struct(world, {
        .entity = ecs_id(Mass),
        .members = {
            { "value", ecs_id(ecs_i32_t) }
        }
    });

    ecs_rule_t *q = ecs_rule(world, {
        .expr = "Position, ?Mass"
    });

    test_assert(q != NULL);

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_entity_t e2 = ecs_new_entity(world, "e2");
    ecs_entity_t e3 = ecs_new_entity(world, "e3");

    ecs_set(world, e1, Position, {10, 20});
    ecs_set(world, e2, Position, {20, 30});
    ecs_set(world, e3, Position, {30, 40});

    ecs_set(world, e3, Mass, {100});

    ecs_iter_t it = ecs_rule_iter(world, q);
    char *json = ecs_iter_to_json(world, &it, &(ecs_iter_to_json_desc_t) {
        .serialize_rows = true,
        .serialize_field_info = true
    });
    test_assert(json != NULL);

    char* expect = "{"
        "\"field_info\":["
            "{\"id\":\"Position\", \"type\":\"Position\", \"symbol\":\"Position\", \"schema\":{\"x\":[\"int\"], \"y\":[\"int\"]}}, "
            "{\"id\":\"Mass\", \"optional\":true, \"type\":\"Mass\", \"symbol\":\"Mass\", \"schema\":{\"value\":[\"int\"]}}], "
        "\"results\":["
            "{\"name\":\"e1\", \"is_set\":[true, false], \"components\":{\"Position\":{\"x\":10, \"y\":20}}}, "
            "{\"name\":\"e2\", \"is_set\":[true, false], \"components\":{\"Position\":{\"x\":20, \"y\":30}}}, "
            "{\"name\":\"e3\", \"is_set\":[true, true], \"components\":{\"Position\":{\"x\":30, \"y\":40}, \"Mass\":{\"value\":100}}}]}";
    test_str(json, expect);

    ecs_os_free(json);

    ecs_rule_fini(q);

    ecs_fini(world);
}

void SerializeIterToRowJson_serialize_w_field_info_pair_w_0_target(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Mass);

    ecs_struct(world, {
        .entity = ecs_id(Position),
        .members = {
            { "x", ecs_id(ecs_i32_t) },
            { "y", ecs_id(ecs_i32_t) }
        }
    });

    ecs_struct(world, {
        .entity = ecs_id(Mass),
        .members = {
            { "value", ecs_id(ecs_i32_t) }
        }
    });

    ecs_rule_t *q = ecs_rule(world, {
        .expr = "Position, ?Mass, (ChildOf, 0)"
    });

    test_assert(q != NULL);

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_entity_t e2 = ecs_new_entity(world, "e2");
    ecs_entity_t e3 = ecs_new_entity(world, "e3");

    ecs_set(world, e1, Position, {10, 20});
    ecs_set(world, e2, Position, {20, 30});
    ecs_set(world, e3, Position, {30, 40});

    ecs_set(world, e3, Mass, {100});

    ecs_iter_t it = ecs_rule_iter(world, q);
    char *json = ecs_iter_to_json(world, &it, &(ecs_iter_to_json_desc_t) {
        .serialize_rows = true,
        .serialize_field_info = true
    });
    test_assert(json != NULL);

    char* expect = "{\"field_info\":["
        "{\"id\":\"Position\", \"type\":\"Position\", \"symbol\":\"Position\", \"schema\":{\"x\":[\"int\"], \"y\":[\"int\"]}}, "
        "{\"id\":\"Mass\", \"optional\":true, \"type\":\"Mass\", \"symbol\":\"Mass\", \"schema\":{\"value\":[\"int\"]}}, "
        "{\"id\":\"(ChildOf,0)\", \"exclusive\":true}], "
    "\"results\":["
        "{\"name\":\"e1\", \"pairs\":{\"ChildOf\":\"0\"},\"is_set\":[true, false, true], \"components\":{\"Position\":{\"x\":10, \"y\":20}}}, "
        "{\"name\":\"e2\", \"pairs\":{\"ChildOf\":\"0\"},\"is_set\":[true, false, true], \"components\":{\"Position\":{\"x\":20, \"y\":30}}}, "
        "{\"name\":\"e3\", \"pairs\":{\"ChildOf\":\"0\"},\"is_set\":[true, true, true], \"components\":{\"Position\":{\"x\":30, \"y\":40}, \"Mass\":{\"value\":100}}}]}";
    test_str(json, expect);

    ecs_os_free(json);

    ecs_rule_fini(q);

    ecs_fini(world);
}

void SerializeIterToRowJson_serialize_w_field_info_pair_w_not_tag(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Foo);

    ecs_struct(world, {
        .entity = ecs_id(Position),
        .members = {
            { "x", ecs_id(ecs_i32_t) },
            { "y", ecs_id(ecs_i32_t) }
        }
    });

    ecs_rule_t *q = ecs_rule(world, {
        .expr = "Position, !Foo"
    });

    test_assert(q != NULL);

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_entity_t e2 = ecs_new_entity(world, "e2");
    ecs_entity_t e3 = ecs_new_entity(world, "e3");

    ecs_set(world, e1, Position, {10, 20});
    ecs_set(world, e2, Position, {20, 30});
    ecs_set(world, e3, Position, {30, 40});

    ecs_add(world, e3, Foo);

    ecs_iter_t it = ecs_rule_iter(world, q);
    char *json = ecs_iter_to_json(world, &it, &(ecs_iter_to_json_desc_t) {
        .serialize_rows = true,
        .serialize_field_info = true
    });
    test_assert(json != NULL);

    char* expect = "{"
        "\"field_info\":["
            "{\"id\":\"Position\", \"type\":\"Position\", \"symbol\":\"Position\", \"schema\":{\"x\":[\"int\"], \"y\":[\"int\"]}}, "
            "{\"id\":\"Foo\", \"not\":true}], "
        "\"results\":["
            "{\"name\":\"e1\", \"is_set\":[true, false], \"components\":{\"Position\":{\"x\":10, \"y\":20}}}, "
            "{\"name\":\"e2\", \"is_set\":[true, false], \"components\":{\"Position\":{\"x\":20, \"y\":30}}}]}";
    test_str(json, expect);

    ecs_os_free(json);

    ecs_rule_fini(q);

    ecs_fini(world);
}

void SerializeIterToRowJson_serialize_w_field_info_pair_w_not_pair(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Foo);

    ecs_struct(world, {
        .entity = ecs_id(Position),
        .members = {
            { "x", ecs_id(ecs_i32_t) },
            { "y", ecs_id(ecs_i32_t) }
        }
    });

    ecs_rule_t *q = ecs_rule(world, {
        .expr = "Position, !(ChildOf, flecs)"
    });

    test_assert(q != NULL);

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_entity_t e2 = ecs_new_entity(world, "e2");
    ecs_entity_t e3 = ecs_new_entity(world, "e3");

    ecs_set(world, e1, Position, {10, 20});
    ecs_set(world, e2, Position, {20, 30});
    ecs_set(world, e3, Position, {30, 40});

    ecs_add(world, e3, Foo);

    ecs_iter_t it = ecs_rule_iter(world, q);
    char *json = ecs_iter_to_json(world, &it, &(ecs_iter_to_json_desc_t) {
        .serialize_rows = true,
        .serialize_field_info = true
    });
    test_assert(json != NULL);

    char* expect = "{"
        "\"field_info\":["
            "{\"id\":\"Position\", \"type\":\"Position\", \"symbol\":\"Position\", \"schema\":{\"x\":[\"int\"], \"y\":[\"int\"]}}, "
            "{\"id\":\"(ChildOf,flecs)\", \"not\":true}], "
        "\"results\":["
            "{\"name\":\"e1\", \"is_set\":[true, false], \"components\":{\"Position\":{\"x\":10, \"y\":20}}}, "
            "{\"name\":\"e2\", \"is_set\":[true, false], \"components\":{\"Position\":{\"x\":20, \"y\":30}}}, "
            "{\"name\":\"e3\", \"is_set\":[true, false], \"components\":{\"Position\":{\"x\":30, \"y\":40}}}]}";
    test_str(json, expect);

    ecs_os_free(json);

    ecs_rule_fini(q);

    ecs_fini(world);
}

void SerializeIterToRowJson_serialize_w_field_info_pair_w_not_component(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Mass);

    ecs_struct(world, {
        .entity = ecs_id(Position),
        .members = {
            { "x", ecs_id(ecs_i32_t) },
            { "y", ecs_id(ecs_i32_t) }
        }
    });

    ecs_struct(world, {
        .entity = ecs_id(Mass),
        .members = {
            { "value", ecs_id(ecs_i32_t) }
        }
    });

    ecs_rule_t *q = ecs_rule(world, {
        .expr = "Position, !Mass"
    });

    test_assert(q != NULL);

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_entity_t e2 = ecs_new_entity(world, "e2");
    ecs_entity_t e3 = ecs_new_entity(world, "e3");

    ecs_set(world, e1, Position, {10, 20});
    ecs_set(world, e2, Position, {20, 30});
    ecs_set(world, e3, Position, {30, 40});

    ecs_set(world, e3, Mass, {100});

    ecs_iter_t it = ecs_rule_iter(world, q);
    char *json = ecs_iter_to_json(world, &it, &(ecs_iter_to_json_desc_t) {
        .serialize_rows = true,
        .serialize_field_info = true
    });
    test_assert(json != NULL);

    char* expect = "{\"field_info\":["
        "{\"id\":\"Position\", \"type\":\"Position\", \"symbol\":\"Position\", \"schema\":{\"x\":[\"int\"], \"y\":[\"int\"]}}, "
        "{\"id\":\"Mass\", \"not\":true}], "
    "\"results\":["
        "{\"name\":\"e1\", \"is_set\":[true, false], \"components\":{\"Position\":{\"x\":10, \"y\":20}}}, "
        "{\"name\":\"e2\", \"is_set\":[true, false], \"components\":{\"Position\":{\"x\":20, \"y\":30}}}]}";
    test_str(json, expect);

    ecs_os_free(json);

    ecs_rule_fini(q);

    ecs_fini(world);
}

void SerializeIterToRowJson_serialize_w_field_info_w_or(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Mass);

    ecs_struct(world, {
        .entity = ecs_id(Position),
        .members = {
            { "x", ecs_id(ecs_i32_t) },
            { "y", ecs_id(ecs_i32_t) }
        }
    });

    ecs_struct(world, {
        .entity = ecs_id(Mass),
        .members = {
            { "value", ecs_id(ecs_i32_t) }
        }
    });

    ecs_rule_t *q = ecs_rule(world, {
        .expr = "Position || Mass"
    });

    test_assert(q != NULL);

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_entity_t e2 = ecs_new_entity(world, "e2");
    ecs_entity_t e3 = ecs_new_entity(world, "e3");

    ecs_set(world, e1, Position, {10, 20});
    ecs_set(world, e2, Position, {20, 30});
    ecs_set(world, e3, Mass, {100});

    ecs_iter_t it = ecs_rule_iter(world, q);
    char *json = ecs_iter_to_json(world, &it, &(ecs_iter_to_json_desc_t) {
        .serialize_rows = true,
        .serialize_field_info = true
    });
    test_assert(json != NULL);

    char* expect = "{\"field_info\":[{\"id\":0}], "
        "\"results\":["
            "{\"name\":\"e1\", \"tags\":[\"Position\"]}, "
            "{\"name\":\"e2\", \"tags\":[\"Position\"]}, "
            "{\"name\":\"e3\", \"tags\":[\"Mass\"]}]}";
    test_str(json, expect);

    ecs_os_free(json);

    ecs_rule_fini(q);

    ecs_fini(world);
}

void SerializeIterToRowJson_serialize_recycled_id(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_rule_t *q = ecs_rule(world, {
        .expr = "TagA"
    });

    test_assert(q != NULL);

    ecs_entity_t _e1 = ecs_new_id(world);
    ecs_delete(world, _e1);
    ecs_entity_t e1 = ecs_new_id(world);
    test_assert(e1 != _e1);
    test_assert((uint32_t)e1 == _e1);

    ecs_add(world, e1, TagA);

    ecs_iter_t it = ecs_rule_iter(world, q);
    char *json = ecs_iter_to_json(world, &it, &(ecs_iter_to_json_desc_t) {
        .serialize_rows = true
    });
    test_assert(json != NULL);

    char* expect = ecs_asprintf("{\"results\":["
        "{\"name\":\"%u\", \"tags\":[\"TagA\"]}]}", (uint32_t)e1);
    test_str(json, expect);

    ecs_os_free(json);
    ecs_os_free(expect);

    ecs_rule_fini(q);

    ecs_fini(world);
}

void SerializeIterToRowJson_serialize_entity_w_flecs_core_parent(void) {
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

    char *json = ecs_iter_to_json(world, &it, &(ecs_iter_to_json_desc_t) {
        .serialize_rows = true
    });

    test_str(json, "{\"results\":["
        "{\"parent\":\"flecs.core.bob\", \"name\":\"e1\", \"tags\":[\"Foo\"]}]}");

    ecs_os_free(json);

    ecs_rule_fini(r);

    ecs_fini(world);
}
