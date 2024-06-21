#include <meta.h>

void SerializeQueryInfoToJson_1_tag(void) {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Foo, (OnInstantiate, Inherit));

    ecs_query_t *q = ecs_query(world, {
        .expr = "Foo"
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    ecs_iter_to_json_desc_t desc = {
        .serialize_query_info = true,
        .dont_serialize_results = true
    };

    char *json = ecs_iter_to_json(&it, &desc);
    test_str(json, "{\"query_info\":{\"vars\":[\"this\"], \"terms\":[{\"inout\":\"default\", \"has_value\":false, \"can_inherit\":true, \"oper\":\"and\", \"src\":{\"var\":\"this\"}, \"first\":{\"entity\":\"Foo\"}, \"trav\":{\"entity\":\"flecs.core.IsA\", \"symbol\":\"EcsIsA\"}, \"flags\":[\"self\", \"up\"]}]}}");
    ecs_os_free(json);

    ecs_query_fini(q);

    ecs_fini(world);
}

void SerializeQueryInfoToJson_1_component(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Position"
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    ecs_iter_to_json_desc_t desc = {
        .serialize_query_info = true,
        .dont_serialize_results = true
    };

    char *json = ecs_iter_to_json(&it, &desc);
    test_str(json, "{\"query_info\":{\"vars\":[\"this\"], \"terms\":[{\"inout\":\"default\", \"has_value\":true, \"can_inherit\":true, \"oper\":\"and\", \"src\":{\"var\":\"this\"}, \"first\":{\"entity\":\"Position\", \"symbol\":\"Position\", \"type\":true}, \"trav\":{\"entity\":\"flecs.core.IsA\", \"symbol\":\"EcsIsA\"}, \"flags\":[\"self\", \"up\"]}]}}");
    ecs_os_free(json);

    ecs_query_fini(q);

    ecs_fini(world);
}

void SerializeQueryInfoToJson_1_pair(void) {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Rel, (OnInstantiate, Inherit));
    ECS_ENTITY(world, Tgt, (OnInstantiate, Inherit));

    ecs_query_t *q = ecs_query(world, {
        .expr = "(Rel, Tgt)"
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    ecs_iter_to_json_desc_t desc = {
        .serialize_query_info = true,
        .dont_serialize_results = true
    };

    char *json = ecs_iter_to_json(&it, &desc);
    test_str(json, "{\"query_info\":{\"vars\":[\"this\"], \"terms\":[{\"inout\":\"default\", \"has_value\":false, \"can_inherit\":true, \"oper\":\"and\", \"src\":{\"var\":\"this\"}, \"first\":{\"entity\":\"Rel\"}, \"second\":{\"entity\":\"Tgt\"}, \"trav\":{\"entity\":\"flecs.core.IsA\", \"symbol\":\"EcsIsA\"}, \"flags\":[\"self\", \"up\"]}]}}");
    ecs_os_free(json);

    ecs_query_fini(q);

    ecs_fini(world);
}

void SerializeQueryInfoToJson_1_pair_w_wildcard(void) {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Rel, (OnInstantiate, Inherit));

    ecs_query_t *q = ecs_query(world, {
        .expr = "(Rel, *)"
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    ecs_iter_to_json_desc_t desc = {
        .serialize_query_info = true,
        .dont_serialize_results = true
    };

    char *json = ecs_iter_to_json(&it, &desc);
    test_str(json, "{\"query_info\":{\"vars\":[\"this\"], \"terms\":[{\"inout\":\"default\", \"has_value\":false, \"can_inherit\":true, \"oper\":\"and\", \"src\":{\"var\":\"this\"}, \"first\":{\"entity\":\"Rel\"}, \"second\":{\"var\":\"*\"}, \"trav\":{\"entity\":\"flecs.core.IsA\", \"symbol\":\"EcsIsA\"}, \"flags\":[\"self\", \"up\"]}]}}");
    ecs_os_free(json);

    ecs_query_fini(q);

    ecs_fini(world);
}

void SerializeQueryInfoToJson_1_pair_w_any(void) {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Rel, (OnInstantiate, Inherit));

    ecs_query_t *q = ecs_query(world, {
        .expr = "(Rel, _)"
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    ecs_iter_to_json_desc_t desc = {
        .serialize_query_info = true,
        .dont_serialize_results = true
    };

    char *json = ecs_iter_to_json(&it, &desc);
    test_str(json, "{\"query_info\":{\"vars\":[\"this\"], \"terms\":[{\"inout\":\"default\", \"has_value\":false, \"can_inherit\":true, \"oper\":\"and\", \"src\":{\"var\":\"this\"}, \"first\":{\"entity\":\"Rel\"}, \"second\":{\"var\":\"_\"}, \"trav\":{\"entity\":\"flecs.core.IsA\", \"symbol\":\"EcsIsA\"}, \"flags\":[\"self\", \"up\"]}]}}");
    ecs_os_free(json);
    
    ecs_query_fini(q);

    ecs_fini(world);
}

void SerializeQueryInfoToJson_1_tag_fixed_src(void) {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Foo, (OnInstantiate, Inherit));
    ECS_ENTITY(world, e, (OnInstantiate, Inherit));

    ecs_query_t *q = ecs_query(world, {
        .expr = "Foo(e)"
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    ecs_iter_to_json_desc_t desc = {
        .serialize_query_info = true,
        .dont_serialize_results = true
    };

    char *json = ecs_iter_to_json(&it, &desc);
    test_str(json, "{\"query_info\":{\"vars\":[], \"terms\":[{\"inout\":\"default\", \"has_value\":false, \"can_inherit\":true, \"oper\":\"and\", \"src\":{\"entity\":\"e\"}, \"first\":{\"entity\":\"Foo\"}, \"trav\":{\"entity\":\"flecs.core.IsA\", \"symbol\":\"EcsIsA\"}, \"flags\":[\"self\", \"up\"]}]}}");
    ecs_os_free(json);

    ecs_query_fini(q);

    ecs_fini(world);
}

void SerializeQueryInfoToJson_1_tag_var_src(void) {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Foo, (OnInstantiate, Inherit));

    ecs_query_t *q = ecs_query(world, {
        .expr = "Foo($v)"
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    ecs_iter_to_json_desc_t desc = {
        .serialize_query_info = true,
        .dont_serialize_results = true
    };

    char *json = ecs_iter_to_json(&it, &desc);
    test_str(json, "{\"query_info\":{\"vars\":[\"v\"], \"terms\":[{\"inout\":\"default\", \"has_value\":false, \"can_inherit\":true, \"oper\":\"and\", \"src\":{\"var\":\"v\"}, \"first\":{\"entity\":\"Foo\"}, \"trav\":{\"entity\":\"flecs.core.IsA\", \"symbol\":\"EcsIsA\"}, \"flags\":[\"self\", \"up\"]}]}}");
    ecs_os_free(json);

    ecs_query_fini(q);

    ecs_fini(world);
}

void SerializeQueryInfoToJson_1_component_in(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);

    ecs_query_t *q = ecs_query(world, {
        .expr = "[in] Position"
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    ecs_iter_to_json_desc_t desc = {
        .serialize_query_info = true,
        .dont_serialize_results = true
    };

    char *json = ecs_iter_to_json(&it, &desc);
    test_str(json, "{\"query_info\":{\"vars\":[\"this\"], \"terms\":[{\"inout\":\"in\", \"has_value\":true, \"can_inherit\":true, \"oper\":\"and\", \"src\":{\"var\":\"this\"}, \"first\":{\"entity\":\"Position\", \"symbol\":\"Position\", \"type\":true}, \"trav\":{\"entity\":\"flecs.core.IsA\", \"symbol\":\"EcsIsA\"}, \"flags\":[\"self\", \"up\"]}]}}");
    ecs_os_free(json);

    ecs_query_fini(q);

    ecs_fini(world);
}

void SerializeQueryInfoToJson_1_component_inout(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);

    ecs_query_t *q = ecs_query(world, {
        .expr = "[inout] Position"
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    ecs_iter_to_json_desc_t desc = {
        .serialize_query_info = true,
        .dont_serialize_results = true
    };

    char *json = ecs_iter_to_json(&it, &desc);
    test_str(json, "{\"query_info\":{\"vars\":[\"this\"], \"terms\":[{\"inout\":\"inout\", \"has_value\":true, \"can_inherit\":true, \"oper\":\"and\", \"src\":{\"var\":\"this\"}, \"first\":{\"entity\":\"Position\", \"symbol\":\"Position\", \"type\":true}, \"trav\":{\"entity\":\"flecs.core.IsA\", \"symbol\":\"EcsIsA\"}, \"flags\":[\"self\", \"up\"]}]}}");
    ecs_os_free(json);

    ecs_query_fini(q);

    ecs_fini(world);
}

void SerializeQueryInfoToJson_1_component_out(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);

    ecs_query_t *q = ecs_query(world, {
        .expr = "[out] Position"
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    ecs_iter_to_json_desc_t desc = {
        .serialize_query_info = true,
        .dont_serialize_results = true
    };

    char *json = ecs_iter_to_json(&it, &desc);
    test_str(json, "{\"query_info\":{\"vars\":[\"this\"], \"terms\":[{\"inout\":\"out\", \"has_value\":true, \"can_inherit\":true, \"oper\":\"and\", \"src\":{\"var\":\"this\"}, \"first\":{\"entity\":\"Position\", \"symbol\":\"Position\", \"type\":true}, \"trav\":{\"entity\":\"flecs.core.IsA\", \"symbol\":\"EcsIsA\"}, \"flags\":[\"self\", \"up\"]}]}}");
    ecs_os_free(json);

    ecs_query_fini(q);

    ecs_fini(world);
}

void SerializeQueryInfoToJson_1_component_none(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);

    ecs_query_t *q = ecs_query(world, {
        .expr = "[none] Position"
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    ecs_iter_to_json_desc_t desc = {
        .serialize_query_info = true,
        .dont_serialize_results = true
    };

    char *json = ecs_iter_to_json(&it, &desc);
    test_str(json, "{\"query_info\":{\"vars\":[\"this\"], \"terms\":[{\"inout\":\"none\", \"has_value\":false, \"can_inherit\":true, \"oper\":\"and\", \"src\":{\"var\":\"this\"}, \"first\":{\"entity\":\"Position\", \"symbol\":\"Position\", \"type\":true}, \"trav\":{\"entity\":\"flecs.core.IsA\", \"symbol\":\"EcsIsA\"}, \"flags\":[\"self\", \"up\"]}]}}");
    ecs_os_free(json);

    ecs_query_fini(q);

    ecs_fini(world);
}

void SerializeQueryInfoToJson_1_tag_not(void) {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Foo, (OnInstantiate, Inherit));

    ecs_query_t *q = ecs_query(world, {
        .expr = "!Foo"
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    ecs_iter_to_json_desc_t desc = {
        .serialize_query_info = true,
        .dont_serialize_results = true
    };

    char *json = ecs_iter_to_json(&it, &desc);
    test_str(json, "{\"query_info\":{\"vars\":[\"this\"], \"terms\":[{\"inout\":\"none\", \"has_value\":false, \"can_inherit\":true, \"oper\":\"not\", \"src\":{\"var\":\"this\"}, \"first\":{\"entity\":\"Foo\"}, \"trav\":{\"entity\":\"flecs.core.IsA\", \"symbol\":\"EcsIsA\"}, \"flags\":[\"self\", \"up\"]}]}}");
    ecs_os_free(json);

    ecs_query_fini(q);

    ecs_fini(world);
}

void SerializeQueryInfoToJson_2_tags_or(void) {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Foo, (OnInstantiate, Inherit));
    ECS_ENTITY(world, Bar, (OnInstantiate, Inherit));

    ecs_query_t *q = ecs_query(world, {
        .expr = "Foo || Bar"
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    ecs_iter_to_json_desc_t desc = {
        .serialize_query_info = true,
        .dont_serialize_results = true
    };

    char *json = ecs_iter_to_json(&it, &desc);
    test_str(json, "{\"query_info\":{\"vars\":[\"this\"], \"terms\":[{\"inout\":\"default\", \"has_value\":false, \"can_inherit\":true, \"oper\":\"or\", \"src\":{\"var\":\"this\"}, \"first\":{\"entity\":\"Foo\"}, \"trav\":{\"entity\":\"flecs.core.IsA\", \"symbol\":\"EcsIsA\"}, \"flags\":[\"self\", \"up\"]}, {\"inout\":\"default\", \"has_value\":false, \"can_inherit\":true, \"oper\":\"and\", \"src\":{\"var\":\"this\"}, \"first\":{\"entity\":\"Bar\"}, \"trav\":{\"entity\":\"flecs.core.IsA\", \"symbol\":\"EcsIsA\"}, \"flags\":[\"self\", \"up\"]}]}}");
    ecs_os_free(json);

    ecs_query_fini(q);

    ecs_fini(world);
}

void SerializeQueryInfoToJson_1_tag_optional(void) {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Foo, (OnInstantiate, Inherit));

    ecs_query_t *q = ecs_query(world, {
        .expr = "?Foo"
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    ecs_iter_to_json_desc_t desc = {
        .serialize_query_info = true,
        .dont_serialize_results = true
    };

    char *json = ecs_iter_to_json(&it, &desc);
    test_str(json, "{\"query_info\":{\"vars\":[\"this\"], \"terms\":[{\"inout\":\"default\", \"has_value\":false, \"can_inherit\":true, \"oper\":\"optional\", \"src\":{\"var\":\"this\"}, \"first\":{\"entity\":\"Foo\"}, \"trav\":{\"entity\":\"flecs.core.IsA\", \"symbol\":\"EcsIsA\"}, \"flags\":[\"self\", \"up\"]}]}}");
    ecs_os_free(json);

    ecs_query_fini(q);

    ecs_fini(world);
}

void SerializeQueryInfoToJson_1_tag_self(void) {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Foo, (OnInstantiate, Inherit));

    ecs_query_t *q = ecs_query(world, {
        .expr = "Foo(self)"
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    ecs_iter_to_json_desc_t desc = {
        .serialize_query_info = true,
        .dont_serialize_results = true
    };

    char *json = ecs_iter_to_json(&it, &desc);
    test_str(json, "{\"query_info\":{\"vars\":[\"this\"], \"terms\":[{\"inout\":\"default\", \"has_value\":false, \"can_inherit\":true, \"oper\":\"and\", \"src\":{\"var\":\"this\"}, \"first\":{\"entity\":\"Foo\"}, \"flags\":[\"self\"]}]}}");
    ecs_os_free(json);

    ecs_query_fini(q);

    ecs_fini(world);
}

void SerializeQueryInfoToJson_1_tag_self_dont_inherit(void) {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Foo, (OnInstantiate, DontInherit));

    ecs_query_t *q = ecs_query(world, {
        .expr = "Foo(self)"
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    ecs_iter_to_json_desc_t desc = {
        .serialize_query_info = true,
        .dont_serialize_results = true
    };

    char *json = ecs_iter_to_json(&it, &desc);
    test_str(json, "{\"query_info\":{\"vars\":[\"this\"], \"terms\":[{\"inout\":\"default\", \"has_value\":false, \"oper\":\"and\", \"src\":{\"var\":\"this\"}, \"first\":{\"entity\":\"Foo\"}, \"flags\":[\"self\"]}]}}");
    ecs_os_free(json);

    ecs_query_fini(q);

    ecs_fini(world);
}

void SerializeQueryInfoToJson_1_tag_up(void) {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Foo, (OnInstantiate, Inherit));

    ecs_query_t *q = ecs_query(world, {
        .expr = "Foo(up)"
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    ecs_iter_to_json_desc_t desc = {
        .serialize_query_info = true,
        .dont_serialize_results = true
    };

    char *json = ecs_iter_to_json(&it, &desc);
    test_str(json, "{\"query_info\":{\"vars\":[\"this\"], \"terms\":[{\"inout\":\"default\", \"has_value\":false, \"can_inherit\":true, \"oper\":\"and\", \"src\":{\"var\":\"this\"}, \"first\":{\"entity\":\"Foo\"}, \"trav\":{\"entity\":\"flecs.core.ChildOf\", \"symbol\":\"EcsChildOf\"}, \"flags\":[\"up\"]}]}}");
    ecs_os_free(json);

    ecs_query_fini(q);

    ecs_fini(world);
}

void SerializeQueryInfoToJson_1_tag_cascade(void) {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Foo, (OnInstantiate, Inherit));

    ecs_query_t *q = ecs_query(world, {
        .expr = "Foo(cascade)"
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    ecs_iter_to_json_desc_t desc = {
        .serialize_query_info = true,
        .dont_serialize_results = true
    };

    char *json = ecs_iter_to_json(&it, &desc);
    test_str(json, "{\"query_info\":{\"vars\":[\"this\"], \"terms\":[{\"inout\":\"default\", \"has_value\":false, \"can_inherit\":true, \"oper\":\"and\", \"src\":{\"var\":\"this\"}, \"first\":{\"entity\":\"Foo\"}, \"trav\":{\"entity\":\"flecs.core.ChildOf\", \"symbol\":\"EcsChildOf\"}, \"flags\":[\"cascade\"]}]}}");
    ecs_os_free(json);
    
    ecs_query_fini(q);

    ecs_fini(world);
}

void SerializeQueryInfoToJson_0_term(void) {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Foo, (OnInstantiate, Inherit));

    ecs_query_t *q = ecs_query(world, {
        .expr = "Foo(#0)"
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    ecs_iter_to_json_desc_t desc = {
        .serialize_query_info = true,
        .dont_serialize_results = true
    };

    char *json = ecs_iter_to_json(&it, &desc);
    test_str(json, "{\"query_info\":{\"vars\":[], \"terms\":[{\"inout\":\"default\", \"has_value\":false, \"can_inherit\":true, \"oper\":\"and\", \"src\":{\"entity\":\"#0\"}, \"first\":{\"entity\":\"Foo\"}, \"flags\":[]}]}}");
    ecs_os_free(json);
    
    ecs_query_fini(q);

    ecs_fini(world);
}

void SerializeQueryInfoToJson_anonymous_tag(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t foo = ecs_new(world);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ foo }}
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    ecs_iter_to_json_desc_t desc = {
        .serialize_query_info = true,
        .dont_serialize_results = true
    };

    char *json = ecs_iter_to_json(&it, &desc);
    char *expect = flecs_asprintf("{\"query_info\":{\"vars\":[\"this\"], \"terms\":[{\"inout\":\"default\", \"has_value\":false, \"oper\":\"and\", \"src\":{\"var\":\"this\"}, \"first\":{\"entity\":\"#%u\"}, \"flags\":[\"self\"]}]}}",
        foo);
    test_str(json, expect);
    ecs_os_free(json);
    ecs_os_free(expect);

    ecs_query_fini(q);

    ecs_fini(world);
}

void SerializeQueryInfoToJson_anonymous_pair(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t rel = ecs_new(world);
    ecs_entity_t tgt = ecs_new(world);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ ecs_pair(rel, tgt) }}
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    ecs_iter_to_json_desc_t desc = {
        .serialize_query_info = true,
        .dont_serialize_results = true
    };

    char *json = ecs_iter_to_json(&it, &desc);
    char *expect = flecs_asprintf("{\"query_info\":{\"vars\":[\"this\"], \"terms\":[{\"inout\":\"default\", \"has_value\":false, \"oper\":\"and\", \"src\":{\"var\":\"this\"}, \"first\":{\"entity\":\"#%u\"}, \"second\":{\"entity\":\"#%u\"}, \"flags\":[\"self\"]}]}}",
        (uint32_t)rel, (uint32_t)tgt);
    test_str(json, expect);
    ecs_os_free(json);
    ecs_os_free(expect);

    ecs_query_fini(q);

    ecs_fini(world);
}

void SerializeQueryInfoToJson_anonymous_component(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t comp = ecs_component(world, {
        .type.size = 4,
        .type.alignment = 4
    });

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ comp }}
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    ecs_iter_to_json_desc_t desc = {
        .serialize_query_info = true,
        .dont_serialize_results = true
    };

    char *json = ecs_iter_to_json(&it, &desc);
    char *expect = flecs_asprintf("{\"query_info\":{\"vars\":[\"this\"], \"terms\":[{\"inout\":\"default\", \"has_value\":true, \"oper\":\"and\", \"src\":{\"var\":\"this\"}, \"first\":{\"entity\":\"#%u\", \"type\":true}, \"flags\":[\"self\"]}]}}",
        comp);
    test_str(json, expect);
    ecs_os_free(json);
    ecs_os_free(expect);

    ecs_query_fini(q);

    ecs_fini(world);
}

void SerializeQueryInfoToJson_anonymous_tag_recycled(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t foo = ecs_new(world);
    ecs_delete(world, foo);
    foo = ecs_new(world);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ foo }}
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    ecs_iter_to_json_desc_t desc = {
        .serialize_query_info = true,
        .dont_serialize_results = true
    };

    char *json = ecs_iter_to_json(&it, &desc);
    char *expect = flecs_asprintf("{\"query_info\":{\"vars\":[\"this\"], \"terms\":[{\"inout\":\"default\", \"has_value\":false, \"oper\":\"and\", \"src\":{\"var\":\"this\"}, \"first\":{\"entity\":\"#%u\"}, \"flags\":[\"self\"]}]}}",
        foo);
    test_str(json, expect);
    ecs_os_free(json);
    ecs_os_free(expect);

    ecs_query_fini(q);

    ecs_fini(world);
}

void SerializeQueryInfoToJson_anonymous_pair_recycled(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t rel = ecs_new(world);
    ecs_entity_t tgt = ecs_new(world);
    ecs_delete(world, rel);
    ecs_delete(world, tgt);
    rel = ecs_new(world);
    tgt = ecs_new(world);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ ecs_pair(rel, tgt) }}
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    ecs_iter_to_json_desc_t desc = {
        .serialize_query_info = true,
        .dont_serialize_results = true
    };

    char *json = ecs_iter_to_json(&it, &desc);
    char *expect = flecs_asprintf("{\"query_info\":{\"vars\":[\"this\"], \"terms\":[{\"inout\":\"default\", \"has_value\":false, \"oper\":\"and\", \"src\":{\"var\":\"this\"}, \"first\":{\"entity\":\"#%u\"}, \"second\":{\"entity\":\"#%u\"}, \"flags\":[\"self\"]}]}}",
        (uint32_t)rel, (uint32_t)tgt);
    test_str(json, expect);
    ecs_os_free(json);
    ecs_os_free(expect);

    ecs_query_fini(q);

    ecs_fini(world);
}

void SerializeQueryInfoToJson_anonymous_component_recycled(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t comp = ecs_new(world);
    ecs_delete(world, comp);
    comp = ecs_component(world, {
        .type.size = 4,
        .type.alignment = 4
    });

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ comp }}
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    ecs_iter_to_json_desc_t desc = {
        .serialize_query_info = true,
        .dont_serialize_results = true
    };

    char *json = ecs_iter_to_json(&it, &desc);
    char *expect = flecs_asprintf("{\"query_info\":{\"vars\":[\"this\"], \"terms\":[{\"inout\":\"default\", \"has_value\":true, \"oper\":\"and\", \"src\":{\"var\":\"this\"}, \"first\":{\"entity\":\"#%u\", \"type\":true}, \"flags\":[\"self\"]}]}}",
        comp);
    test_str(json, expect);
    ecs_os_free(json);
    ecs_os_free(expect);

    ecs_query_fini(q);

    ecs_fini(world);
}
