#include <addons.h>

void Rest_teardown(void) {
    ecs_world_t *world = ecs_init();

    ecs_singleton_set(world, EcsRest, {27760});

    ecs_fini(world);

    test_assert(true); // Ensure teardown was successful
}

void Rest_get(void) {
    ecs_world_t *world = ecs_init();

    ecs_http_server_t *srv = ecs_rest_server_init(world, NULL);
    test_assert(srv != NULL);

    ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
    test_int(0, ecs_http_server_request(srv, "GET",
        "/entity/flecs/core?label=true", NULL, &reply));
    test_int(reply.code, 200);
    
    char *reply_str = ecs_strbuf_get(&reply.body);
    test_assert(reply_str != NULL);
    test_str(reply_str,
            "{\"parent\":\"flecs\", \"name\":\"core\", \"tags\":[\"flecs.core.Module\"]}");
    ecs_os_free(reply_str);

    ecs_rest_server_fini(srv);

    ecs_fini(world);
}

void Rest_get_cached(void) {
    ecs_world_t *world = ecs_init();

    ecs_http_server_t *srv = ecs_rest_server_init(world, 
        &(ecs_http_server_desc_t){
            .cache_timeout = 1.0
        });
    test_assert(srv != NULL);

    {
        ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
        test_int(0, ecs_http_server_request(srv, "GET",
            "/entity/flecs/core", NULL, &reply));
        test_int(reply.code, 200);
        
        char *reply_str = ecs_strbuf_get(&reply.body);
        test_assert(reply_str != NULL);
        test_str(reply_str,
            "{\"parent\":\"flecs\", \"name\":\"core\", \"tags\":[\"flecs.core.Module\"]}");
        ecs_os_free(reply_str);
    }

    {
        ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
        test_int(0, ecs_http_server_request(srv, "GET",
            "/entity/flecs/core", NULL, &reply));
        test_int(reply.code, 200);
        
        char *reply_str = ecs_strbuf_get(&reply.body);
        test_assert(reply_str != NULL);
        test_str(reply_str,
            "{\"parent\":\"flecs\", \"name\":\"core\", \"tags\":[\"flecs.core.Module\"]}");
        ecs_os_free(reply_str);
    }

    ecs_rest_server_fini(srv);

    ecs_fini(world);
}

void Rest_get_cached_invalid(void) {
    ecs_world_t *world = ecs_init();

    ecs_http_server_t *srv = ecs_rest_server_init(world, 
        &(ecs_http_server_desc_t){
            .cache_timeout = 1.0
        });
    test_assert(srv != NULL);

    {
        ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
        test_int(-1, ecs_http_server_request(srv, "GET",
            "/entity/flecs/core/Wor", NULL, &reply));
        test_int(reply.code, 404);
        
        char *reply_str = ecs_strbuf_get(&reply.body);
        test_assert(reply_str != NULL);
        test_str(reply_str,
            "{\"error\":\"entity \'flecs/core/Wor\' not found\"}");
        ecs_os_free(reply_str);
    }

    {
        ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
        test_int(-1, ecs_http_server_request(srv, "GET",
            "/entity/flecs/core/Wor", NULL, &reply));
        test_int(reply.code, 404);
        
        char *reply_str = ecs_strbuf_get(&reply.body);
        test_assert(reply_str != NULL);
        test_str(reply_str,
            "{\"error\":\"entity \'flecs/core/Wor\' not found\"}");
        ecs_os_free(reply_str);
    }

    ecs_rest_server_fini(srv);

    ecs_fini(world);
}

void Rest_try_query(void) {
    ecs_world_t *world = ecs_init();

    ecs_http_server_t *srv = ecs_rest_server_init(world, NULL);
    test_assert(srv != NULL);

    ecs_log_set_level(-4);

    {
        ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
        test_int(-1, ecs_http_server_request(srv, "GET",
            "/query?expr=Foo", NULL, &reply));
        test_int(reply.code, 400); // No try, should error
        ecs_strbuf_reset(&reply.body);
    }

    {
        ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
        test_int(0, ecs_http_server_request(srv, "GET",
            "/query?expr=Foo&try=true", NULL, &reply));
        test_int(reply.code, 200); // With try, should not error
        ecs_strbuf_reset(&reply.body);
    }

    ecs_rest_server_fini(srv);

    ecs_fini(world);
}

void Rest_query(void) {
    ecs_world_t *world = ecs_init();

    ecs_http_server_t *srv = ecs_rest_server_init(world, NULL);
    test_assert(srv != NULL);

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_entity(world, { .name = "e" });
    ecs_set(world, e, Position, {10, 20});

    ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
    test_int(0, ecs_http_server_request(srv, "GET",
        "/query?expr=Position", NULL, &reply));
    test_int(reply.code, 200);
    
    char *reply_str = ecs_strbuf_get(&reply.body);
    test_assert(reply_str != NULL);
    test_str(reply_str,
        "{\"results\":[{\"name\":\"e\", \"fields\":{\"values\":[0]}}]}");
    ecs_os_free(reply_str);

    ecs_rest_server_fini(srv);

    ecs_fini(world);
}

void Rest_named_query(void) {
    ecs_world_t *world = ecs_init();

    ecs_http_server_t *srv = ecs_rest_server_init(world, NULL);
    test_assert(srv != NULL);

    ECS_COMPONENT(world, Position);

    ecs_query(world, {
        .entity = ecs_entity(world, { .name = "position_query" }),
        .terms = {
            { .id = ecs_id(Position) }
        }
    });

    ecs_entity_t e = ecs_entity(world, { .name = "e" });
    ecs_set(world, e, Position, {10, 20});

    ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
    test_int(0, ecs_http_server_request(srv, "GET",
        "/query?name=position_query", NULL, &reply));
    test_int(reply.code, 200);
    
    char *reply_str = ecs_strbuf_get(&reply.body);
    test_assert(reply_str != NULL);
    test_str(reply_str,
        "{\"results\":[{\"name\":\"e\", \"fields\":{\"values\":[0]}}]}");
    ecs_os_free(reply_str);

    ecs_rest_server_fini(srv);

    ecs_fini(world);
}

void Rest_call(void) {
    ecs_world_t *world = ecs_init();

    test_int(ecs_script_run(world, NULL,
        "fn add(arg_1: i32, arg_2: i32) -> i32 { arg_1 + arg_2 }",
        NULL), 0);

    ecs_http_server_t *srv = ecs_rest_server_init(world, NULL);
    test_assert(srv != NULL);

    ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
    test_int(0, ecs_http_server_request(srv, "GET",
        "/call/add?arg_1=10&arg_2=20", NULL, &reply));
    test_int(reply.code, 200);

    char *reply_str = ecs_strbuf_get(&reply.body);
    test_assert(reply_str != NULL);
    test_str(reply_str, "30");
    ecs_os_free(reply_str);

    ecs_rest_server_fini(srv);
    ecs_fini(world);
}

void Rest_call_string(void) {
    ecs_world_t *world = ecs_init();

    test_int(ecs_script_run(world, NULL,
        "fn echo(value: string) -> string { value }", NULL), 0);

    ecs_http_server_t *srv = ecs_rest_server_init(world, NULL);
    test_assert(srv != NULL);

    ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
    test_int(0, ecs_http_server_request(srv, "GET",
        "/call/echo?value=hello%20world", NULL, &reply));
    test_int(reply.code, 200);

    char *reply_str = ecs_strbuf_get(&reply.body);
    test_assert(reply_str != NULL);
    test_str(reply_str, "\"hello world\"");
    ecs_os_free(reply_str);

    ecs_rest_server_fini(srv);
    ecs_fini(world);
}

void Rest_call_missing_argument(void) {
    ecs_world_t *world = ecs_init();

    test_int(ecs_script_run(world, NULL,
        "fn add(arg_1: i32, arg_2: i32) -> i32 { arg_1 + arg_2 }",
        NULL), 0);

    ecs_http_server_t *srv = ecs_rest_server_init(world, NULL);
    test_assert(srv != NULL);

    ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
    test_int(-1, ecs_http_server_request(srv, "GET",
        "/call/add?arg_1=10", NULL, &reply));
    test_int(reply.code, 400);

    char *reply_str = ecs_strbuf_get(&reply.body);
    test_assert(reply_str != NULL);
    test_str(reply_str, "{\"error\":\"missing argument 'arg_2'\"}");
    ecs_os_free(reply_str);

    ecs_rest_server_fini(srv);
    ecs_fini(world);
}

void Rest_call_not_found(void) {
    ecs_world_t *world = ecs_init();

    ecs_http_server_t *srv = ecs_rest_server_init(world, NULL);
    test_assert(srv != NULL);

    ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
    test_int(-1, ecs_http_server_request(srv, "GET",
        "/call/missing", NULL, &reply));
    test_int(reply.code, 404);

    char *reply_str = ecs_strbuf_get(&reply.body);
    test_assert(reply_str != NULL);
    test_str(reply_str, "{\"error\":\"entity 'missing' not found\"}");
    ecs_os_free(reply_str);

    ecs_rest_server_fini(srv);
    ecs_fini(world);
}

void Rest_entity_not_found_w_dot_sep(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t parent = ecs_entity(world, { .name = "foo" });
    ecs_entity(world, { .name = "bar", .parent = parent });

    ecs_http_server_t *srv = ecs_rest_server_init(world, NULL);
    test_assert(srv != NULL);

    ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
    test_int(-1, ecs_http_server_request(srv, "GET",
        "/entity/foo.bar", NULL, &reply));
    test_int(reply.code, 404);

    char *reply_str = ecs_strbuf_get(&reply.body);
    test_assert(reply_str != NULL);
    test_str(reply_str, "{\"error\":\"entity 'foo.bar' not found, "
        "did you mean '/entity/foo/bar'?\"}");
    ecs_os_free(reply_str);

    ecs_rest_server_fini(srv);
    ecs_fini(world);
}

void Rest_tables(void) {
    ecs_world_t *world = ecs_init();

    ecs_http_server_t *srv = ecs_rest_server_init(world, NULL);
    test_assert(srv != NULL);

    ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
    test_int(0, ecs_http_server_request(srv, "GET",
        "/tables", NULL, &reply));
    test_int(reply.code, 200);
    
    char *reply_str = ecs_strbuf_get(&reply.body);
    test_assert(reply_str != NULL);
    ecs_os_free(reply_str);

    ecs_rest_server_fini(srv);

    ecs_fini(world);
}

void Rest_components(void) {
    ecs_world_t *world = ecs_init();

    ecs_http_server_t *srv = ecs_rest_server_init(world, NULL);
    test_assert(srv != NULL);

    ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
    test_int(0, ecs_http_server_request(srv, "GET",
        "/components", NULL, &reply));
    test_int(reply.code, 200);
    
    char *reply_str = ecs_strbuf_get(&reply.body);
    test_assert(reply_str != NULL);
    ecs_os_free(reply_str);

    ecs_rest_server_fini(srv);

    ecs_fini(world);
}

void Rest_type_info_non_existing_entity(void) {
    ecs_world_t *world = ecs_init();

    ecs_http_server_t *srv = ecs_rest_server_init(world, NULL);
    test_assert(srv != NULL);

    ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
    test_int(-1, ecs_http_server_request(srv, "GET",
        "/type_info/transform/DoesNotExist", NULL, &reply));
    test_int(reply.code, 404);

    char *reply_str = ecs_strbuf_get(&reply.body);
    test_assert(reply_str != NULL);
    test_str(reply_str,
        "{\"error\":\"entity 'transform/DoesNotExist' not found\"}");
    ecs_os_free(reply_str);

    ecs_rest_server_fini(srv);

    ecs_fini(world);
}

void Rest_type_info_not_component(void) {
    ecs_world_t *world = ecs_init();

    ecs_http_server_t *srv = ecs_rest_server_init(world, NULL);
    test_assert(srv != NULL);

    ecs_entity(world, { .name = "NotAComponent" });

    ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
    test_int(0, ecs_http_server_request(srv, "GET",
        "/type_info/NotAComponent", NULL, &reply));
    test_int(reply.code, 200);

    char *reply_str = ecs_strbuf_get(&reply.body);
    test_assert(reply_str != NULL);
    test_str(reply_str, "0");
    ecs_os_free(reply_str);

    ecs_rest_server_fini(srv);

    ecs_fini(world);
}

void Rest_type_info_component_without_reflection(void) {
    ecs_world_t *world = ecs_init();

    ecs_http_server_t *srv = ecs_rest_server_init(world, NULL);
    test_assert(srv != NULL);

    ecs_component(world, {
        .entity = ecs_entity(world, { .name = "NoReflection" }),
        .type = {
            .size = ECS_SIZEOF(ecs_i32_t),
            .alignment = ECS_ALIGNOF(ecs_i32_t)
        }
    });

    ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
    test_int(0, ecs_http_server_request(srv, "GET",
        "/type_info/NoReflection", NULL, &reply));
    test_int(reply.code, 200);

    char *reply_str = ecs_strbuf_get(&reply.body);
    test_assert(reply_str != NULL);
    test_str(reply_str, "0");
    ecs_os_free(reply_str);

    ecs_rest_server_fini(srv);

    ecs_fini(world);
}

void Rest_type_info_component_with_reflection(void) {
    ecs_world_t *world = ecs_init();

    ecs_http_server_t *srv = ecs_rest_server_init(world, NULL);
    test_assert(srv != NULL);

    ecs_entity_t transform = ecs_entity(world, { .name = "transform" });
    test_assert(transform != 0);

    ecs_entity_t position_component = ecs_struct(world, {
        .entity = ecs_entity(world, {
            .name = "PositionComponent",
            .parent = transform
        }),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });
    test_assert(position_component != 0);

    ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
    test_int(0, ecs_http_server_request(srv, "GET",
        "/type_info/transform/PositionComponent", NULL, &reply));
    test_int(reply.code, 200);

    char *reply_str = ecs_strbuf_get(&reply.body);
    test_assert(reply_str != NULL);
    test_str(reply_str, "{\"x\":[\"float\"], \"y\":[\"float\"]}");
    ecs_os_free(reply_str);

    ecs_rest_server_fini(srv);

    ecs_fini(world);
}

void Rest_request_commands(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_http_server_t *srv = ecs_rest_server_init(world, NULL);
    test_assert(srv != NULL);

    {
        ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
        test_int(0, ecs_http_server_request(srv, "GET",
            "/commands/capture", NULL, &reply));
        test_int(reply.code, 200);

        char *reply_str = ecs_strbuf_get(&reply.body);
        test_assert(reply_str != NULL);
        test_str(reply_str, "{\"frame\":0}");
        ecs_os_free(reply_str);
    }

    ecs_entity_t e1 = ecs_new(world);
    ecs_entity_t e2 = ecs_new(world);

    ecs_frame_begin(world, 0);
    ecs_world_t *stage_1 = ecs_is_stage(world) ? world : ecs_get_stage(world, 0);
    ecs_set(stage_1, e1, Position, {10, 20});
    ecs_set(stage_1, e2, Position, {20, 30});
    ecs_merge(stage_1);
    ecs_frame_end(world);

    {
        ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
        test_int(0, ecs_http_server_request(srv, "GET",
            "/commands/frame/0", NULL, &reply));
        test_int(reply.code, 200);

        char *reply_str = ecs_strbuf_get(&reply.body);
        test_assert(reply_str != NULL);
        ecs_os_free(reply_str);
    }

    ecs_rest_server_fini(srv);

    ecs_fini(world);
}

void Rest_request_commands_2_syncs(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_http_server_t *srv = ecs_rest_server_init(world, NULL);
    test_assert(srv != NULL);

    {
        ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
        test_int(0, ecs_http_server_request(srv, "GET",
            "/commands/capture", NULL, &reply));
        test_int(reply.code, 200);

        char *reply_str = ecs_strbuf_get(&reply.body);
        test_assert(reply_str != NULL);
        test_str(reply_str, "{\"frame\":0}");
        ecs_os_free(reply_str);
    }

    ecs_entity_t e1 = ecs_new(world);
    ecs_entity_t e2 = ecs_new(world);

    ecs_frame_begin(world, 0);
    ecs_world_t *stage_1 = ecs_is_stage(world) ? world : ecs_get_stage(world, 0);
    ecs_set(stage_1, e1, Position, {10, 20});
    ecs_merge(stage_1);
    ecs_world_t *stage_2 = ecs_is_stage(world) ? world : ecs_get_stage(world, 0);
    ecs_set(stage_2, e2, Position, {20, 30});
    ecs_merge(stage_2);
    ecs_frame_end(world);

    {
        ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
        test_int(0, ecs_http_server_request(srv, "GET",
            "/commands/frame/0", NULL, &reply));
        test_int(reply.code, 200);

        char *reply_str = ecs_strbuf_get(&reply.body);
        test_assert(reply_str != NULL);
        ecs_os_free(reply_str);
    }

    ecs_rest_server_fini(srv);

    ecs_fini(world);
}

void Rest_request_commands_no_frames(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_http_server_t *srv = ecs_rest_server_init(world, NULL);
    test_assert(srv != NULL);

    {
        ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
        test_int(-1, ecs_http_server_request(srv, "GET",
            "/commands/frame/0", NULL, &reply));
        test_int(reply.code, 404);

        char *reply_str = ecs_strbuf_get(&reply.body);
        test_assert(reply_str != NULL);
        ecs_os_free(reply_str);
    }

    ecs_rest_server_fini(srv);

    ecs_fini(world);
}

void Rest_request_commands_no_commands(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_http_server_t *srv = ecs_rest_server_init(world, NULL);
    test_assert(srv != NULL);

    {
        ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
        test_int(0, ecs_http_server_request(srv, "GET",
            "/commands/capture", NULL, &reply));
        test_int(reply.code, 200);

        char *reply_str = ecs_strbuf_get(&reply.body);
        test_assert(reply_str != NULL);
        test_str(reply_str, "{\"frame\":0}");
        ecs_os_free(reply_str);
    }

    ecs_frame_begin(world, 0);
    ecs_world_t *stage_1 = ecs_is_stage(world) ? world : ecs_get_stage(world, 0);
    ecs_merge(stage_1);
    ecs_frame_end(world);

    {
        ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
        test_int(0, ecs_http_server_request(srv, "GET",
            "/commands/frame/0", NULL, &reply));
        test_int(reply.code, 200);

        char *reply_str = ecs_strbuf_get(&reply.body);
        test_assert(reply_str != NULL);
        ecs_os_free(reply_str);
    }

    ecs_rest_server_fini(srv);

    ecs_fini(world);
}

void Rest_request_commands_garbage_collect(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_http_server_t *srv = ecs_rest_server_init(world, NULL);
    test_assert(srv != NULL);

    {
        ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
        test_int(0, ecs_http_server_request(srv, "GET",
            "/commands/capture", NULL, &reply));
        test_int(reply.code, 200);

        char *reply_str = ecs_strbuf_get(&reply.body);
        test_assert(reply_str != NULL);
        test_str(reply_str, "{\"frame\":0}");
        ecs_os_free(reply_str);
    }

    ecs_entity_t e1 = ecs_new(world);
    ecs_entity_t e2 = ecs_new(world);

    ecs_frame_begin(world, 0);
    ecs_world_t *stage_1 = ecs_is_stage(world) ? world : ecs_get_stage(world, 0);
    ecs_set(stage_1, e1, Position, {10, 20});
    ecs_set(stage_1, e2, Position, {20, 30});
    ecs_merge(stage_1);
    ecs_frame_end(world);

    /* Retained for a minute at 60 FPS */
    for (int i = 0; i < 60 * 60; i ++) {
        ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
        test_int(0, ecs_http_server_request(srv, "GET",
            "/commands/frame/0", NULL, &reply));
        test_int(reply.code, 200);

        char *reply_str = ecs_strbuf_get(&reply.body);
        test_assert(reply_str != NULL);
        ecs_os_free(reply_str);

        ecs_progress(world, 0.016);
    }

    /* New request hits garbage collection */
    {
        ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
        test_int(0, ecs_http_server_request(srv, "GET",
            "/commands/capture", NULL, &reply));
        test_int(reply.code, 200);
        char *reply_str = ecs_strbuf_get(&reply.body);
        test_assert(reply_str != NULL);
        test_str(reply_str, "{\"frame\":3601}");
        ecs_os_free(reply_str);
    }

    /* Garbage collected */
    {
        ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
        test_int(-1, ecs_http_server_request(srv, "GET",
            "/commands/frame/0", NULL, &reply));
        test_int(reply.code, 404);
        char *reply_str = ecs_strbuf_get(&reply.body);
        test_assert(reply_str != NULL);
        ecs_os_free(reply_str);
    }

    /* Garbage collected */
    {
        ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
        test_int(0, ecs_http_server_request(srv, "GET",
            "/commands/frame/3601", NULL, &reply));
        test_int(reply.code, 200);
        char *reply_str = ecs_strbuf_get(&reply.body);
        test_assert(reply_str != NULL);
        ecs_os_free(reply_str);
    }

    ecs_rest_server_fini(srv);

    ecs_fini(world);
}

void Rest_script_error(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main.flecs", .sep = "/" }),
        .code = ""
    });

    ecs_http_server_t *srv = ecs_rest_server_init(world, NULL);
    test_assert(srv != NULL);

    {
        ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
        ecs_log_set_level(-4);
        test_int(-1, ecs_http_server_request(srv, "PUT",
            "/script/main.flecs?code=struct%20Position%20%7B%0A%20%20x%20%3A%0A%7D",
            NULL, &reply));
        test_int(reply.code, 400);
        char *reply_str = ecs_strbuf_get(&reply.body);
        test_assert(reply_str != NULL);
        ecs_os_free(reply_str);
    }

    ecs_rest_server_fini(srv);

    ecs_fini(world);
}

void Rest_script_error_new_script_deferred(void) {
    ecs_world_t *world = ecs_init();

    ecs_http_server_t *srv = ecs_rest_server_init(world, NULL);
    test_assert(srv != NULL);

    {
        ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
        ecs_log_set_level(-4);
        ecs_world_t *stage_1 = ecs_is_stage(world) ? world : ecs_get_stage(world, 0);
        test_int(-1, ecs_http_server_request(srv, "PUT",
            "/script/main.flecs?code=foo%2B%7B",
            NULL, &reply));
        ecs_merge(stage_1);
        test_int(reply.code, 400);
        char *reply_str = ecs_strbuf_get(&reply.body);
        test_assert(reply_str != NULL);
        ecs_os_free(reply_str);
    }

    ecs_rest_server_fini(srv);

    ecs_fini(world);
}

void Rest_script_update(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main.flecs", .sep = "/" }),
        .code = ""
    });

    ecs_http_server_t *srv = ecs_rest_server_init(world, NULL);
    test_assert(srv != NULL);

    {
        ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
        test_int(0, ecs_http_server_request(srv, "PUT",
            "/script/main.flecs?code=e%20%7B%7D",
            NULL, &reply));
        test_int(reply.code, 200);
        char *reply_str = ecs_strbuf_get(&reply.body);
        test_str(reply_str, "{}");
        test_assert(ecs_lookup(world, "e") != 0);
        ecs_os_free(reply_str);
    }

    ecs_rest_server_fini(srv);

    ecs_fini(world);
}

void Rest_script_update_w_body(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main.flecs", .sep = "/" }),
        .code = ""
    });

    ecs_http_server_t *srv = ecs_rest_server_init(world, NULL);
    test_assert(srv != NULL);

    {
        ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
        test_int(0, ecs_http_server_request(srv, "PUT",
            "/script/main.flecs", "e {}", &reply));
        test_int(reply.code, 200);
        char *reply_str = ecs_strbuf_get(&reply.body);
        test_str(reply_str, "{}");
        ecs_os_free(reply_str);
    }

    ecs_rest_server_fini(srv);

    ecs_fini(world);
}

void Rest_import_rest_after_mini(void) {
    ecs_world_t *world = ecs_mini();

    ECS_IMPORT(world, FlecsRest);

    test_assert(ecs_id(EcsRest) != 0);

    ecs_fini(world);
}

static void Move(ecs_iter_t *it) { }

void Rest_get_pipeline_stats_after_delete_system(void) {
    ecs_world_t *world = ecs_init();

    ECS_IMPORT(world, FlecsStats);

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_SYSTEM(world, Move, EcsOnUpdate, Position, Velocity);

    ecs_http_server_t *srv = ecs_rest_server_init(world, NULL);
    test_assert(srv != NULL);

    ecs_progress(world, 1.0);

    ecs_delete(world, ecs_id(Move));

    ecs_progress(world, 1.0);

    ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
    test_int(0, ecs_http_server_request(srv, "GET",
        "/stats/pipeline?name=all&period=1m", NULL, &reply));
    test_int(reply.code, 200);
    
    char *reply_str = ecs_strbuf_get(&reply.body);
    test_assert(reply_str != NULL);
    ecs_os_free(reply_str);

    ecs_rest_server_fini(srv);

    ecs_fini(world);
}

void Rest_request_world_summary_before_monitor_sys_run(void) {
    ecs_world_t *world = ecs_init();

    ECS_IMPORT(world, FlecsStats);

    ecs_http_server_t *srv = ecs_rest_server_init(world, NULL);
    test_assert(srv != NULL);

    ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
    test_int(0, ecs_http_server_request(srv, "GET",
        "/entity/flecs/core/World?values=NULL, true", NULL, &reply));
    test_int(reply.code, 200);
    
    char *reply_str = ecs_strbuf_get(&reply.body);
    test_assert(reply_str != NULL);
    ecs_os_free(reply_str);

    ecs_rest_server_fini(srv);

    ecs_fini(world);
}

void Rest_escape_backslash(void) {
    ecs_world_t *world = ecs_init();

    ECS_IMPORT(world, FlecsStats);

    ecs_http_server_t *srv = ecs_rest_server_init(world, NULL);
    test_assert(srv != NULL);

    ecs_entity(world, { .name = "foo/bar" });

    ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
    test_int(0, ecs_http_server_request(srv, "GET",
        "/entity/foo%5C%2Fbar", NULL, &reply));
    test_int(reply.code, 200);
    
    char *reply_str = ecs_strbuf_get(&reply.body);
    test_assert(reply_str != NULL);
    test_str(reply_str, "{\"name\":\"foo/bar\"}");
    ecs_os_free(reply_str);

    ecs_rest_server_fini(srv);

    ecs_fini(world);
}

void Rest_entity_name_w_spaces(void) {
    ecs_world_t *world = ecs_init();

    ecs_http_server_t *srv = ecs_rest_server_init(world, NULL);
    test_assert(srv != NULL);

    ecs_entity(world, { .name = "foo bar" });

    ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
    test_int(0, ecs_http_server_request(srv, "GET",
        "/entity/foo+bar", NULL, &reply));
    test_int(reply.code, 200);

    char *reply_str = ecs_strbuf_get(&reply.body);
    test_assert(reply_str != NULL);
    test_str(reply_str, "{\"name\":\"foo bar\"}");
    ecs_os_free(reply_str);

    ecs_rest_server_fini(srv);

    ecs_fini(world);
}

void Rest_query_w_spaces(void) {
    ecs_world_t *world = ecs_init();

    ecs_http_server_t *srv = ecs_rest_server_init(world, NULL);
    test_assert(srv != NULL);

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t e = ecs_entity(world, { .name = "e" });
    ecs_set(world, e, Position, {10, 20});
    ecs_set(world, e, Velocity, {1, 2});

    ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
    test_int(0, ecs_http_server_request(srv, "GET",
        "/query?expr=Position,+Velocity", NULL, &reply));
    test_int(reply.code, 200);

    char *reply_str = ecs_strbuf_get(&reply.body);
    test_assert(reply_str != NULL);
    test_str(reply_str,
        "{\"results\":[{\"name\":\"e\", \"fields\":{\"values\":[0, 0]}}]}");
    ecs_os_free(reply_str);

    ecs_rest_server_fini(srv);

    ecs_fini(world);
}

void Rest_request_small_buffer_plus_one(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main.flecs", .sep = "/" }),
        .code = ""
    });

    ecs_http_server_t *srv = ecs_rest_server_init(world, NULL);
    test_assert(srv != NULL);

    const char *payload = 
        "e0_0 { }; e0_1 { }; e0_2 { }; e0_3 { }; e0_4 { }; e0_5 { }; e0_6 { }; e0_7 { }; e0_8 { }; e0_9 { };\n"
        "e1_0 { }; e1_1 { }; e1_2 { }; e1_3 { }; e1_4 { }; e1_5 { }; e1_6 { }; e1_7 { }; e1_8 { }; e1_9 { };\n"
        "e2_0 { }; e2_1 { }; e2_2 { }; e2_3 { }; e2_4 { }; e2_5 { }; e2_6 { }; e2_7 { }; e2_8 { }; e2_9 { };\n"
        "e3_0 { }; e3_1 { }; e3_2 { }; e3_3 { }; e3_4 { }; e3_5 { }; e3_6 { }; e3_7 { }; e3_8 { }; e3_9 { };\n"
        "e4_0 { }; e4_1 { }; e4_2 { }; e4_3 { }; e4_4 { }; e4_5 { }; e4_6 { }; e4_7 { }; e4_8 { }; e4_9 { };\n"
        "e5_0 { }; e5_1 { }; e5_2 { }; e5_3 { }; e5_4 { }; e5_5 { }; e5_6 { }; e5_7 { }; e5_8 { }; e5_9 { };\n"
        "e6_0 { }; e6_1 { }; e6_2 { }; e6_3 { }; e6_4 { }; e6_5 { }; e6_6 { }; e6_7 { }; e6_8 { }; e6_9 { };\n"
        "e7_0 { }; e7_1 { }; e7_2 { }; e7_3 { }; e7_4 { }; e7_5 { }; e7_6 { }; e7_7 { }; e7_8 { }; e7_9 { };\n"
        "e8_0 { }; e8_1 { }; e8_2 { }; e8_3 { }; e8_4 { }; e8_5 { }; e8_6 { }; e8_7 { }; e8_8 { }; e8_9 { };\n"
        "e9_0 { }; e9_1 { }; e9_2 { }; e9_3 { }; e9_4 {}; e9_5 {}; e9_6 {};\n";

    {
        ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
        ecs_log_set_level(-4);
        test_int(0, ecs_http_server_request(srv, "PUT",
            "/script/main.flecs",
            payload, &reply));
        test_int(reply.code, 200);
        char *reply_str = ecs_strbuf_get(&reply.body);
        test_str(reply_str, "{}");
        ecs_os_free(reply_str);
    }

    ecs_rest_server_fini(srv);

    ecs_fini(world);
}

void Rest_request_ending_in_pct(void) {
    ecs_world_t *world = ecs_init();

    ecs_http_server_t *srv = ecs_rest_server_init(world, NULL);
    test_assert(srv != NULL);

    ecs_entity(world, { .name = "foo/bar" });

    ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
    ecs_log_set_level(-4);
    test_int(-1, ecs_http_server_request(srv, "GET",
        "/entity/foo%", NULL, &reply));
    test_int(reply.code, 404);
    
    char *reply_str = ecs_strbuf_get(&reply.body);
    test_assert(reply_str != NULL);
    test_str(reply_str, "{\"error\":\"entity 'foo%' not found\"}");
    ecs_os_free(reply_str);

    ecs_rest_server_fini(srv);

    ecs_fini(world);
}

void Rest_request_ending_in_2_pct(void) {
    ecs_world_t *world = ecs_init();

    ecs_http_server_t *srv = ecs_rest_server_init(world, NULL);
    test_assert(srv != NULL);

    ecs_entity(world, { .name = "foo/bar" });

    ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
    ecs_log_set_level(-4);
    test_int(-1, ecs_http_server_request(srv, "GET",
        "/entity/foo%%", NULL, &reply));
    test_int(reply.code, 404);
    
    char *reply_str = ecs_strbuf_get(&reply.body);
    test_assert(reply_str != NULL);
    test_str(reply_str, "{\"error\":\"entity 'foo' not found\"}");
    ecs_os_free(reply_str);

    ecs_rest_server_fini(srv);

    ecs_fini(world);
}

void Rest_request_ending_in_pct_single_digit(void) {
    ecs_world_t *world = ecs_init();

    ecs_http_server_t *srv = ecs_rest_server_init(world, NULL);
    test_assert(srv != NULL);

    ecs_entity(world, { .name = "foo/bar" });

    ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
    ecs_log_set_level(-4);
    test_int(-1, ecs_http_server_request(srv, "GET",
        "/entity/foo%9", NULL, &reply));
    test_int(reply.code, 404);
    
    char *reply_str = ecs_strbuf_get(&reply.body);
    test_assert(reply_str != NULL);
    test_str(reply_str, "{\"error\":\"entity 'foo`' not found\"}");
    ecs_os_free(reply_str);

    ecs_rest_server_fini(srv);

    ecs_fini(world);
}

void Rest_request_ending_in_pct_invalid_code(void) {
    ecs_world_t *world = ecs_init();

    ecs_http_server_t *srv = ecs_rest_server_init(world, NULL);
    test_assert(srv != NULL);

    ecs_entity(world, { .name = "foo/bar" });

    ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
    ecs_log_set_level(-4);
    test_int(-1, ecs_http_server_request(srv, "GET",
        "/entity/foo%--", NULL, &reply));
    test_int(reply.code, 404);
    
    char *reply_str = ecs_strbuf_get(&reply.body);
    test_assert(reply_str != NULL);
    test_str(reply_str, "{\"error\":\"entity 'foo' not found\"}");
    ecs_os_free(reply_str);

    ecs_rest_server_fini(srv);

    ecs_fini(world);
}

void Rest_world_has_build_info(void) {
    ecs_world_t *world = ecs_mini();

    ECS_IMPORT(world, FlecsRest);

    ecs_http_server_t *srv = ecs_rest_server_init(world, NULL);
    test_assert(srv != NULL);

    ecs_entity_t build_info = ecs_lookup(world, "flecs.core.BuildInfo");
    test_assert(build_info != 0);
    test_assert(ecs_has_id(world, EcsWorld, build_info));

    ecs_rest_server_fini(srv);

    ecs_fini(world);
}

void Rest_world_default(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ecs_entity_t e = ecs_entity(world, { .name = "e" });
    ecs_set(world, e, Position, {10, 20});

    ecs_http_server_t *srv = ecs_rest_server_init(world, NULL);
    test_assert(srv != NULL);

    ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
    test_int(0, ecs_http_server_request(srv, "GET",
        "/world", NULL, &reply));
    test_int(reply.code, 200);

    char *reply_str = ecs_strbuf_get(&reply.body);
    test_assert(reply_str != NULL);

    ecs_strbuf_t buf = ECS_STRBUF_INIT;
    test_int(0, ecs_world_to_json_buf(world, &buf, NULL));
    char *expect = ecs_strbuf_get(&buf);
    test_str(reply_str, expect);
    ecs_os_free(expect);
    ecs_os_free(reply_str);

    ecs_rest_server_fini(srv);

    ecs_fini(world);
}

void Rest_world_builtin(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ecs_entity_t e = ecs_entity(world, { .name = "e" });
    ecs_set(world, e, Position, {10, 20});

    ecs_http_server_t *srv = ecs_rest_server_init(world, NULL);
    test_assert(srv != NULL);

    ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
    test_int(0, ecs_http_server_request(srv, "GET",
        "/world?builtin=true", NULL, &reply));
    test_int(reply.code, 200);

    char *reply_str = ecs_strbuf_get(&reply.body);
    test_assert(reply_str != NULL);

    ecs_world_to_json_desc_t desc = { .serialize_builtin = true };
    ecs_strbuf_t buf = ECS_STRBUF_INIT;
    test_int(0, ecs_world_to_json_buf(world, &buf, &desc));
    char *expect = ecs_strbuf_get(&buf);
    test_str(reply_str, expect);
    ecs_os_free(expect);
    ecs_os_free(reply_str);

    ecs_rest_server_fini(srv);

    ecs_fini(world);
}

void Rest_world_modules(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ecs_entity_t e = ecs_entity(world, { .name = "e" });
    ecs_set(world, e, Position, {10, 20});

    ecs_http_server_t *srv = ecs_rest_server_init(world, NULL);
    test_assert(srv != NULL);

    ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
    test_int(0, ecs_http_server_request(srv, "GET",
        "/world?modules=true", NULL, &reply));
    test_int(reply.code, 200);

    char *reply_str = ecs_strbuf_get(&reply.body);
    test_assert(reply_str != NULL);

    ecs_world_to_json_desc_t desc = { .serialize_modules = true };
    ecs_strbuf_t buf = ECS_STRBUF_INIT;
    test_int(0, ecs_world_to_json_buf(world, &buf, &desc));
    char *expect = ecs_strbuf_get(&buf);
    test_str(reply_str, expect);
    ecs_os_free(expect);
    ecs_os_free(reply_str);

    ecs_rest_server_fini(srv);

    ecs_fini(world);
}

void Rest_world_builtin_and_modules(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ecs_entity_t e = ecs_entity(world, { .name = "e" });
    ecs_set(world, e, Position, {10, 20});

    ecs_http_server_t *srv = ecs_rest_server_init(world, NULL);
    test_assert(srv != NULL);

    ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
    test_int(0, ecs_http_server_request(srv, "GET",
        "/world?builtin=true&modules=true", NULL, &reply));
    test_int(reply.code, 200);

    char *reply_str = ecs_strbuf_get(&reply.body);
    test_assert(reply_str != NULL);

    ecs_world_to_json_desc_t desc = {
        .serialize_builtin = true,
        .serialize_modules = true
    };
    ecs_strbuf_t buf = ECS_STRBUF_INIT;
    test_int(0, ecs_world_to_json_buf(world, &buf, &desc));
    char *expect = ecs_strbuf_get(&buf);
    test_str(reply_str, expect);
    ecs_os_free(expect);
    ecs_os_free(reply_str);

    ecs_rest_server_fini(srv);

    ecs_fini(world);
}

void Rest_world_explicit_false(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ecs_entity_t e = ecs_entity(world, { .name = "e" });
    ecs_set(world, e, Position, {10, 20});

    ecs_http_server_t *srv = ecs_rest_server_init(world, NULL);
    test_assert(srv != NULL);

    ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
    test_int(0, ecs_http_server_request(srv, "GET",
        "/world?builtin=false&modules=false", NULL, &reply));
    test_int(reply.code, 200);

    char *reply_str = ecs_strbuf_get(&reply.body);
    test_assert(reply_str != NULL);

    ecs_strbuf_t buf = ECS_STRBUF_INIT;
    test_int(0, ecs_world_to_json_buf(world, &buf, NULL));
    char *expect = ecs_strbuf_get(&buf);
    test_str(reply_str, expect);
    ecs_os_free(expect);
    ecs_os_free(reply_str);

    ecs_rest_server_fini(srv);

    ecs_fini(world);
}

void Rest_script_update_new_script(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_http_server_t *srv = ecs_rest_server_init(world, NULL);
    test_assert(srv != NULL);

    {
        ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
        test_int(0, ecs_http_server_request(srv, "PUT",
            "/script/main.flecs?code=e%20%7B%7D",
            NULL, &reply));
        test_int(reply.code, 200);
        char *reply_str = ecs_strbuf_get(&reply.body);
        test_str(reply_str, "{}");
        test_assert(ecs_lookup(world, "e") != 0);
        ecs_os_free(reply_str);
    }

    ecs_rest_server_fini(srv);

    ecs_fini(world);
}

void Rest_query_error_restores_log(void) {
    ecs_world_t *world = ecs_init();
    ecs_query_t *q = ecs_query(world, {
        .entity = ecs_entity(world, {.name = "Q"}),
        .expr = "(ChildOf, $parent)"
    });
    test_assert(q != NULL);
    ecs_http_server_t *srv = ecs_rest_server_init(world, NULL);
    test_assert(srv != NULL);
    ecs_os_api_log_t prev_log = ecs_os_api.log_;

    for (int nested = 0; nested < 2; nested ++) {
        if (nested) {
            ecs_log_start_capture(true);
        }
        ecs_os_api_log_t active_log = ecs_os_api.log_;
        ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
        ecs_http_server_request(srv, "GET",
            "/query?name=Q&vars=parent:&try=true", NULL, &reply);
        test_int(reply.code, 400);
        test_assert(ecs_os_api.log_ == active_log);
        char *body = ecs_strbuf_get(&reply.body);
        test_assert(body != NULL);
        test_assert(strstr(body, "error") != NULL);
        ecs_os_free(body);
        ecs_strbuf_reset(&reply.headers);

        if (nested) {
            ecs_err("outer capture survived");
            char *err = ecs_log_stop_capture();
            test_str(err, "outer capture survived");
            ecs_os_free(err);
        }
        test_assert(ecs_os_api.log_ == prev_log);
    }

    ecs_rest_server_fini(srv);
    ecs_query_fini(q);
    ecs_fini(world);
}

void Rest_call_zero_arguments(void) {
    ecs_world_t *world = ecs_init();
    test_int(ecs_script_run(world, NULL,
        "fn answer() -> i32 { 42 }", NULL), 0);
    ecs_http_server_t *srv = ecs_rest_server_init(world, NULL);
    ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
    test_int(ecs_http_server_request(srv, "GET", "/call/answer", NULL, &reply), 0);
    test_int(reply.code, 200);
    char *body = ecs_strbuf_get(&reply.body);
    test_str(body, "42");
    ecs_os_free(body);
    ecs_rest_server_fini(srv);
    ecs_fini(world);
}

void Rest_call_invalid_argument_after_string(void) {
    ecs_world_t *world = ecs_init();
    test_int(ecs_script_run(world, NULL,
        "fn echo(value: string, count: i32) -> string { value }", NULL), 0);
    ecs_http_server_t *srv = ecs_rest_server_init(world, NULL);
    ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
    test_int(ecs_http_server_request(srv, "GET",
        "/call/echo?value=hello&count=invalid", NULL, &reply), -1);
    test_int(reply.code, 400);
    char *body = ecs_strbuf_get(&reply.body);
    test_str(body, "{\"error\":\"invalid value for argument 'count'\"}");
    ecs_os_free(body);
    ecs_rest_server_fini(srv);
    ecs_fini(world);
}

void Rest_call_argument_expression(void) {
    ecs_world_t *world = ecs_init();
    test_int(ecs_script_run(world, NULL,
        "fn square(value: i32) -> i32 { value * value }", NULL), 0);
    ecs_http_server_t *srv = ecs_rest_server_init(world, NULL);
    ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
    test_int(ecs_http_server_request(srv, "GET",
        "/call/square?value=3%2B4", NULL, &reply), 0);
    test_int(reply.code, 200);
    char *body = ecs_strbuf_get(&reply.body);
    test_str(body, "49");
    ecs_os_free(body);
    ecs_rest_server_fini(srv);
    ecs_fini(world);
}

void Rest_app_enable_rest(void) {
    ecs_world_t *world = ecs_init();

    test_int(ecs_app_run(world, &(ecs_app_desc_t){
        .enable_rest = true,
        .port = 27761,
        .frames = 1
    }), 0);

    const EcsRest *rest = ecs_get(world, EcsWorld, EcsRest);
    test_assert(rest != NULL);
    test_uint(rest->port, 27761);
    test_assert(rest->impl != NULL);
    test_assert(rest->impl->srv != NULL);

    ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
    test_int(ecs_http_server_request(rest->impl->srv, "GET",
        "/entity/flecs/core", NULL, &reply), 0);
    test_int(reply.code, 200);
    char *body = ecs_strbuf_get(&reply.body);
    test_assert(body != NULL);
    ecs_os_free(body);

    ecs_fini(world);
}

void Rest_readonly(void) {
    ecs_world_t *world = ecs_init();

    ecs_singleton_set(world, EcsRest, { .port = 27762, .readonly = true });

    const EcsRest *rest = ecs_singleton_get(world, EcsRest);
    test_assert(rest != NULL);
    test_assert(rest->impl != NULL);
    test_bool(rest->impl->readonly, true);
    ecs_http_server_t *srv = rest->impl->srv;
    test_assert(srv != NULL);

    ecs_entity_t e = ecs_entity(world, { .name = "foo" });

    {
        ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
        test_int(ecs_http_server_request(srv, "GET",
            "/entity/foo", NULL, &reply), 0);
        test_int(reply.code, 200);
        ecs_os_free(ecs_strbuf_get(&reply.body));
    }

    {
        ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
        test_int(ecs_http_server_request(srv, "PUT",
            "/entity/bar", NULL, &reply), -1);
        test_int(reply.code, 403);
        ecs_os_free(ecs_strbuf_get(&reply.body));
        test_assert(ecs_lookup(world, "bar") == 0);
    }

    {
        ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
        test_int(ecs_http_server_request(srv, "DELETE",
            "/entity/foo", NULL, &reply), -1);
        test_int(reply.code, 403);
        ecs_os_free(ecs_strbuf_get(&reply.body));
        test_assert(ecs_is_alive(world, e));
    }

    {
        ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
        test_int(ecs_http_server_request(srv, "PUT",
            "/script/my_script.flecs", "bar {}", &reply), -1);
        test_int(reply.code, 403);
        ecs_os_free(ecs_strbuf_get(&reply.body));
        test_assert(ecs_lookup(world, "my_script.flecs") == 0);
        test_assert(ecs_lookup(world, "bar") == 0);
    }

    {
        ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
        test_int(ecs_http_server_request(srv, "GET",
            "/call/flecs/script/math/sqr?v=7", NULL, &reply), -1);
        test_int(reply.code, 403);
        ecs_os_free(ecs_strbuf_get(&reply.body));
    }

    ecs_fini(world);
}

void Rest_command_new_session(void) {
    ecs_world_t *world = ecs_init();

    ecs_http_server_t *srv = ecs_rest_server_init(world, NULL);
    test_assert(srv != NULL);

    ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
    test_int(0, ecs_http_server_request(srv, "PUT",
        "/shell/session", NULL, &reply));
    test_int(reply.code, 200);
    char *reply_str = ecs_strbuf_get(&reply.body);
    unsigned long long session = 0;
    test_int(sscanf(reply_str, "{\"session\":%llu}", &session), 1);
    ecs_os_free(reply_str);

    test_assert(ecs_is_alive(world, (ecs_entity_t)session));
    test_assert(ecs_has(world, (ecs_entity_t)session, EcsCommandSession));

    ecs_rest_server_fini(srv);
    ecs_fini(world);
}

void Rest_command_run(void) {
    ecs_world_t *world = ecs_init();

    ecs_http_server_t *srv = ecs_rest_server_init(world, NULL);
    test_assert(srv != NULL);

    ecs_entity(world, { .name = "parent.child" });

    ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
    test_int(0, ecs_http_server_request(srv, "PUT",
        "/shell/session", NULL, &reply));
    char *reply_str = ecs_strbuf_get(&reply.body);
    unsigned long long session = 0;
    test_int(sscanf(reply_str, "{\"session\":%llu}", &session), 1);
    ecs_os_free(reply_str);

    char path[128];
    ecs_os_snprintf(path, 128, "/shell/session/%llu", session);

    reply = ECS_HTTP_REPLY_INIT;
    test_int(0, ecs_http_server_request(srv, "PUT", path, "ls parent", &reply));
    test_int(reply.code, 200);
    reply_str = ecs_strbuf_get(&reply.body);
    test_str(reply_str, "{\"results\":[{\"type\":\"flecs.script.typecache.vector<entity>\", "
        "\"result\":[\"parent.child\"], \"more\":false}], \"errors\":[]}");
    ecs_os_free(reply_str);

    reply = ECS_HTTP_REPLY_INIT;
    test_int(0, ecs_http_server_request(srv, "PUT", path, "pwd", &reply));
    test_int(reply.code, 200);
    reply_str = ecs_strbuf_get(&reply.body);
    test_str(reply_str, "{\"results\":[{\"type\":\"flecs.meta.entity\", "
        "\"result\":\"#0\"}], \"errors\":[]}");
    ecs_os_free(reply_str);

    reply = ECS_HTTP_REPLY_INIT;
    test_int(0, ecs_http_server_request(srv, "PUT", path, "rm parent.child",
        &reply));
    test_int(reply.code, 200);
    reply_str = ecs_strbuf_get(&reply.body);
    test_str(reply_str, "{\"results\":[{}], \"errors\":[]}");
    ecs_os_free(reply_str);
    test_assert(ecs_lookup(world, "parent.child") == 0);

    ecs_rest_server_fini(srv);
    ecs_fini(world);
}

void Rest_command_run_error(void) {
    ecs_world_t *world = ecs_init();

    ecs_http_server_t *srv = ecs_rest_server_init(world, NULL);
    test_assert(srv != NULL);

    ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
    test_int(0, ecs_http_server_request(srv, "PUT",
        "/shell/session", NULL, &reply));
    char *reply_str = ecs_strbuf_get(&reply.body);
    unsigned long long session = 0;
    test_int(sscanf(reply_str, "{\"session\":%llu}", &session), 1);
    ecs_os_free(reply_str);

    char path[128];
    ecs_os_snprintf(path, 128, "/shell/session/%llu", session);

    reply = ECS_HTTP_REPLY_INIT;
    test_int(-1, ecs_http_server_request(srv, "PUT", path, "foo", &reply));
    test_int(reply.code, 400);
    reply_str = ecs_strbuf_get(&reply.body);
    test_str(reply_str, "{\"results\":[{}], \"errors\":[\"unknown command 'foo'\"]}");
    ecs_os_free(reply_str);

    ecs_rest_server_fini(srv);
    ecs_fini(world);
}

void Rest_command_run_offset_limit(void) {
    ecs_world_t *world = ecs_init();

    ecs_http_server_t *srv = ecs_rest_server_init(world, NULL);
    test_assert(srv != NULL);

    ecs_entity(world, { .name = "parent.a" });
    ecs_entity(world, { .name = "parent.b" });
    ecs_entity(world, { .name = "parent.c" });
    ecs_entity(world, { .name = "parent.d" });

    ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
    test_int(0, ecs_http_server_request(srv, "PUT",
        "/shell/session", NULL, &reply));
    char *reply_str = ecs_strbuf_get(&reply.body);
    unsigned long long session = 0;
    test_int(sscanf(reply_str, "{\"session\":%llu}", &session), 1);
    ecs_os_free(reply_str);

    char path[128];
    ecs_os_snprintf(path, 128, "/shell/session/%llu?offset=1&limit=2",
        session);

    reply = ECS_HTTP_REPLY_INIT;
    test_int(0, ecs_http_server_request(srv, "PUT", path, "ls parent", &reply));
    test_int(reply.code, 200);
    reply_str = ecs_strbuf_get(&reply.body);
    test_str(reply_str, "{\"results\":[{"
        "\"type\":\"flecs.script.typecache.vector<entity>\", "
        "\"result\":[\"parent.b\", \"parent.c\"], \"more\":true}], "
        "\"errors\":[]}");
    ecs_os_free(reply_str);

    ecs_os_snprintf(path, 128, "/shell/session/%llu?offset=2&limit=2",
        session);

    reply = ECS_HTTP_REPLY_INIT;
    test_int(0, ecs_http_server_request(srv, "PUT", path, "ls parent", &reply));
    test_int(reply.code, 200);
    reply_str = ecs_strbuf_get(&reply.body);
    test_str(reply_str, "{\"results\":[{"
        "\"type\":\"flecs.script.typecache.vector<entity>\", "
        "\"result\":[\"parent.c\", \"parent.d\"], \"more\":false}], "
        "\"errors\":[]}");
    ecs_os_free(reply_str);

    ecs_rest_server_fini(srv);
    ecs_fini(world);
}

void Rest_command_run_string_lines(void) {
    ecs_world_t *world = ecs_init();

    ecs_http_server_t *srv = ecs_rest_server_init(world, NULL);
    test_assert(srv != NULL);

    test_int(ecs_script_run(world, NULL,
        "struct A(x: f32)\n"
        "struct B(a: A)\n"
        "struct C(b: B)\n", NULL), 0);

    ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
    test_int(0, ecs_http_server_request(srv, "PUT",
        "/shell/session", NULL, &reply));
    char *reply_str = ecs_strbuf_get(&reply.body);
    unsigned long long session = 0;
    test_int(sscanf(reply_str, "{\"session\":%llu}", &session), 1);
    ecs_os_free(reply_str);

    char path[128];
    ecs_os_snprintf(path, 128, "/shell/session/%llu?offset=1&limit=1",
        session);

    reply = ECS_HTTP_REPLY_INIT;
    test_int(0, ecs_http_server_request(srv, "PUT", path, "schema C", &reply));
    test_int(reply.code, 200);
    reply_str = ecs_strbuf_get(&reply.body);
    test_str(reply_str, "{\"results\":[{\"type\":\"flecs.meta.string\", "
        "\"result\":\"struct B(a: A)\\n\", \"more\":true}], \"errors\":[]}");
    ecs_os_free(reply_str);

    ecs_rest_server_fini(srv);
    ecs_fini(world);
}

void Rest_command_history(void) {
    ecs_world_t *world = ecs_init();

    ecs_http_server_t *srv = ecs_rest_server_init(world, NULL);
    test_assert(srv != NULL);

    ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
    test_int(0, ecs_http_server_request(srv, "PUT",
        "/shell/session", NULL, &reply));
    char *reply_str = ecs_strbuf_get(&reply.body);
    unsigned long long session = 0;
    test_int(sscanf(reply_str, "{\"session\":%llu}", &session), 1);
    ecs_os_free(reply_str);

    char path[128];
    ecs_os_snprintf(path, 128, "/shell/session/%llu", session);

    const char *cmds[] = { "pwd", "ls", "tree" };
    int i;
    for (i = 0; i < 3; i ++) {
        reply = ECS_HTTP_REPLY_INIT;
        test_int(0, ecs_http_server_request(srv, "PUT", path, cmds[i], &reply));
        ecs_strbuf_reset(&reply.body);
    }

    ecs_os_snprintf(path, 128, "/shell/session/%llu/history", session);
    reply = ECS_HTTP_REPLY_INIT;
    test_int(0, ecs_http_server_request(srv, "GET", path, NULL, &reply));
    test_int(reply.code, 200);
    reply_str = ecs_strbuf_get(&reply.body);
    test_str(reply_str,
        "{\"history\":[\"pwd\", \"ls\", \"tree\"], \"more\":false}");
    ecs_os_free(reply_str);

    ecs_os_snprintf(path, 128, "/shell/session/%llu/history?limit=2",
        session);
    reply = ECS_HTTP_REPLY_INIT;
    test_int(0, ecs_http_server_request(srv, "GET", path, NULL, &reply));
    test_int(reply.code, 200);
    reply_str = ecs_strbuf_get(&reply.body);
    test_str(reply_str, "{\"history\":[\"pwd\", \"ls\"], \"more\":true}");
    ecs_os_free(reply_str);

    ecs_rest_server_fini(srv);
    ecs_fini(world);
}

void Rest_command_history_results(void) {
    ecs_world_t *world = ecs_init();

    ecs_http_server_t *srv = ecs_rest_server_init(world, NULL);
    test_assert(srv != NULL);

    ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
    test_int(0, ecs_http_server_request(srv, "PUT",
        "/shell/session", NULL, &reply));
    char *reply_str = ecs_strbuf_get(&reply.body);
    unsigned long long session = 0;
    test_int(sscanf(reply_str, "{\"session\":%llu}", &session), 1);
    ecs_os_free(reply_str);

    char path[128];
    ecs_os_snprintf(path, 128, "/shell/session/%llu", session);

    const char *cmds[] = { "pwd", "unknown" };
    int i;
    for (i = 0; i < 2; i ++) {
        reply = ECS_HTTP_REPLY_INIT;
        ecs_http_server_request(srv, "PUT", path, cmds[i], &reply);
        ecs_strbuf_reset(&reply.body);
    }

    ecs_os_snprintf(path, 128,
        "/shell/session/%llu/history?results=true", session);
    reply = ECS_HTTP_REPLY_INIT;
    test_int(0, ecs_http_server_request(srv, "GET", path, NULL, &reply));
    test_int(reply.code, 200);
    reply_str = ecs_strbuf_get(&reply.body);
    test_str(reply_str,
        "{\"history\":[\"pwd\", \"unknown\"], \"results\":["
        "{\"scope\":\"#0\", \"type\":\"flecs.meta.entity\", "
        "\"result\":\"#0\", \"errors\":[]}, "
        "{\"scope\":\"#0\", \"errors\":[\"unknown command 'unknown'\"]}], "
        "\"more\":false}");
    ecs_os_free(reply_str);

    ecs_os_snprintf(path, 128,
        "/shell/session/%llu/history?results=true&offset=1&limit=1",
        session);
    reply = ECS_HTTP_REPLY_INIT;
    test_int(0, ecs_http_server_request(srv, "GET", path, NULL, &reply));
    test_int(reply.code, 200);
    reply_str = ecs_strbuf_get(&reply.body);
    test_str(reply_str,
        "{\"history\":[\"unknown\"], \"results\":["
        "{\"scope\":\"#0\", \"errors\":[\"unknown command 'unknown'\"]}], "
        "\"more\":false}");
    ecs_os_free(reply_str);

    ecs_rest_server_fini(srv);
    ecs_fini(world);
}

void Rest_command_delete_session(void) {
    ecs_world_t *world = ecs_init();

    ecs_http_server_t *srv = ecs_rest_server_init(world, NULL);
    test_assert(srv != NULL);

    ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
    test_int(0, ecs_http_server_request(srv, "PUT",
        "/shell/session", NULL, &reply));
    char *reply_str = ecs_strbuf_get(&reply.body);
    unsigned long long session = 0;
    test_int(sscanf(reply_str, "{\"session\":%llu}", &session), 1);
    ecs_os_free(reply_str);

    char path[128];
    ecs_os_snprintf(path, 128, "/shell/session/%llu", session);

    reply = ECS_HTTP_REPLY_INIT;
    test_int(0, ecs_http_server_request(srv, "DELETE", path, NULL, &reply));
    test_int(reply.code, 200);
    ecs_strbuf_reset(&reply.body);
    test_assert(!ecs_is_alive(world, (ecs_entity_t)session));

    reply = ECS_HTTP_REPLY_INIT;
    test_int(-1, ecs_http_server_request(srv, "PUT", path, "pwd", &reply));
    test_int(reply.code, 404);
    reply_str = ecs_strbuf_get(&reply.body);
    test_str(reply_str, "{\"error\":\"invalid command session\"}");
    ecs_os_free(reply_str);

    ecs_rest_server_fini(srv);
    ecs_fini(world);
}

void Rest_command_invalid_session(void) {
    ecs_world_t *world = ecs_init();

    ecs_http_server_t *srv = ecs_rest_server_init(world, NULL);
    test_assert(srv != NULL);

    ecs_entity_t e = ecs_new(world);
    char path[128];
    ecs_os_snprintf(path, 128, "/shell/session/%u", (uint32_t)e);

    ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
    test_int(-1, ecs_http_server_request(srv, "PUT", path, "pwd", &reply));
    test_int(reply.code, 404);
    char *reply_str = ecs_strbuf_get(&reply.body);
    test_str(reply_str, "{\"error\":\"invalid command session\"}");
    ecs_os_free(reply_str);

    reply = ECS_HTTP_REPLY_INIT;
    test_int(-1, ecs_http_server_request(srv, "GET",
        "/shell/session/foo/history", NULL, &reply));
    test_int(reply.code, 404);
    ecs_strbuf_reset(&reply.body);

    ecs_rest_server_fini(srv);
    ecs_fini(world);
}

#define REST_FS_MAX (16)

typedef struct Rest_fs_file_t {
    char path[128];
    char *data;
    size_t size;
    size_t pos;
    bool used;
} Rest_fs_file_t;

typedef struct Rest_fs_dir_t {
    char path[128];
    bool used;
} Rest_fs_dir_t;

static Rest_fs_file_t Rest_fs_files[REST_FS_MAX];
static Rest_fs_dir_t Rest_fs_dirs[REST_FS_MAX];

static Rest_fs_file_t* Rest_fs_file_find(const char *path) {
    int32_t i;
    for (i = 0; i < REST_FS_MAX; i ++) {
        if (Rest_fs_files[i].used && !strcmp(Rest_fs_files[i].path, path)) {
            return &Rest_fs_files[i];
        }
    }
    return NULL;
}

static Rest_fs_dir_t* Rest_fs_dir_find(const char *path, size_t len) {
    int32_t i;
    for (i = 0; i < REST_FS_MAX; i ++) {
        if (Rest_fs_dirs[i].used && strlen(Rest_fs_dirs[i].path) == len &&
            !strncmp(Rest_fs_dirs[i].path, path, len))
        {
            return &Rest_fs_dirs[i];
        }
    }
    return NULL;
}

static bool Rest_fs_parent_exists(const char *path) {
    const char *sep = strrchr(path, '/');
    return !sep || Rest_fs_dir_find(path, (size_t)(sep - path)) != NULL;
}

static FILE* Rest_fs_fopen(const char *path, const char *mode) {
    Rest_fs_file_t *f = Rest_fs_file_find(path);
    if (!strchr(mode, 'w')) {
        if (f) {
            f->pos = 0;
        }
        return (FILE*)f;
    }

    if (!Rest_fs_parent_exists(path)) {
        return NULL;
    }

    if (!f) {
        int32_t i;
        for (i = 0; i < REST_FS_MAX; i ++) {
            if (!Rest_fs_files[i].used) {
                f = &Rest_fs_files[i];
                break;
            }
        }

        test_assert(f != NULL);
        test_assert(strlen(path) < sizeof(f->path));
        strcpy(f->path, path);
        f->used = true;
    }

    free(f->data);
    f->data = NULL;
    f->size = 0;
    f->pos = 0;
    return (FILE*)f;
}

static size_t Rest_fs_fread(void *ptr, size_t size, size_t count, FILE *file) {
    Rest_fs_file_t *f = (Rest_fs_file_t*)file;
    size_t requested = size * count;
    if (requested > (f->size - f->pos)) {
        requested = f->size - f->pos;
    }

    if (requested) {
        memcpy(ptr, &f->data[f->pos], requested);
    }
    f->pos += requested;
    return size ? requested / size : 0;
}

static size_t Rest_fs_fwrite(
    const void *ptr,
    size_t size,
    size_t count,
    FILE *file)
{
    Rest_fs_file_t *f = (Rest_fs_file_t*)file;
    size_t len = size * count;
    if (!len) {
        return count;
    }
    f->data = realloc(f->data, f->pos + len);
    memcpy(&f->data[f->pos], ptr, len);
    f->pos += len;
    if (f->pos > f->size) {
        f->size = f->pos;
    }
    return count;
}

static void Rest_fs_fclose(FILE *file) {
    test_assert(file != NULL);
}

static int Rest_fs_mkdir(const char *path) {
    if (Rest_fs_dir_find(path, strlen(path))) {
        return 0;
    }

    if (!Rest_fs_parent_exists(path)) {
        return -1;
    }

    int32_t i;
    for (i = 0; i < REST_FS_MAX; i ++) {
        if (!Rest_fs_dirs[i].used) {
            test_assert(strlen(path) < sizeof(Rest_fs_dirs[i].path));
            strcpy(Rest_fs_dirs[i].path, path);
            Rest_fs_dirs[i].used = true;
            return 0;
        }
    }

    test_assert(false);
    return -1;
}

static int Rest_fs_rmdir(const char *path) {
    Rest_fs_dir_t *d = Rest_fs_dir_find(path, strlen(path));
    if (!d) {
        return -1;
    }

    size_t len = strlen(path);
    int32_t i;
    for (i = 0; i < REST_FS_MAX; i ++) {
        if (Rest_fs_files[i].used && !strncmp(Rest_fs_files[i].path, path, len) &&
            Rest_fs_files[i].path[len] == '/')
        {
            return -1;
        }
        if (Rest_fs_dirs[i].used && !strncmp(Rest_fs_dirs[i].path, path, len) &&
            Rest_fs_dirs[i].path[len] == '/')
        {
            return -1;
        }
    }

    d->used = false;
    return 0;
}

static int Rest_fs_remove(const char *path) {
    Rest_fs_file_t *f = Rest_fs_file_find(path);
    if (!f) {
        return -1;
    }

    free(f->data);
    memset(f, 0, sizeof(*f));
    return 0;
}

static int32_t Rest_fs_file_count(void) {
    int32_t i, count = 0;
    for (i = 0; i < REST_FS_MAX; i ++) {
        count += Rest_fs_files[i].used;
    }
    return count;
}

static int32_t Rest_fs_dir_count(void) {
    int32_t i, count = 0;
    for (i = 0; i < REST_FS_MAX; i ++) {
        count += Rest_fs_dirs[i].used;
    }
    return count;
}

void Rest_setup(void) {
    memset(Rest_fs_files, 0, sizeof(Rest_fs_files));
    memset(Rest_fs_dirs, 0, sizeof(Rest_fs_dirs));

    ecs_os_set_api_defaults();
    ecs_os_api_t os_api = ecs_os_api;
    os_api.fopen_ = Rest_fs_fopen;
    os_api.fread_ = Rest_fs_fread;
    os_api.fwrite_ = Rest_fs_fwrite;
    os_api.fclose_ = Rest_fs_fclose;
    os_api.mkdir_ = Rest_fs_mkdir;
    os_api.rmdir_ = Rest_fs_rmdir;
    os_api.remove_ = Rest_fs_remove;
    ecs_os_set_api(&os_api);
}


static void Rest_resource_callback(
    const ecs_function_ctx_t *ctx,
    int32_t argc,
    const ecs_value_t *argv,
    ecs_value_t *result)
{
    (void)argc;
    (void)argv;
    ecs_shell_resource_create(ctx->world,
        ecs_shell_get_session(ctx->world), "text/plain", "Hello", 5,
        result->ptr);
}

void Rest_shell_resource(void) {
    ecs_world_t *world = ecs_init();

    ecs_http_server_t *srv = ecs_rest_server_init(world, NULL);
    test_assert(srv != NULL);

    ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
    test_int(0, ecs_http_server_request(srv, "PUT",
        "/shell/session", NULL, &reply));
    char *reply_str = ecs_strbuf_get(&reply.body);
    unsigned long long session = 0;
    test_int(sscanf(reply_str, "{\"session\":%llu}", &session), 1);
    ecs_os_free(reply_str);

    ecs_shell_resource_t r = {0};
    test_int(0, ecs_shell_resource_create(world, (ecs_entity_t)session,
        "text/plain", "Hello World", 11, &r));
    test_str(r.name, "1");

    char path[128];
    ecs_os_snprintf(path, 128, "/shell/session/%llu/resource/%s",
        session, r.name);

    reply = ECS_HTTP_REPLY_INIT;
    test_int(0, ecs_http_server_request(srv, "GET", path, NULL, &reply));
    test_int(reply.code, 200);
    test_str(reply.content_type, "text/plain");
    reply_str = ecs_strbuf_get(&reply.body);
    test_str(reply_str, "Hello World");
    ecs_os_free(reply_str);

    ecs_os_free(r.name);
    ecs_os_free(r.content_type);

    ecs_rest_server_fini(srv);
    ecs_fini(world);
}

void Rest_shell_resource_binary(void) {
    ecs_world_t *world = ecs_init();

    ecs_http_server_t *srv = ecs_rest_server_init(world, NULL);
    test_assert(srv != NULL);

    ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
    test_int(0, ecs_http_server_request(srv, "PUT",
        "/shell/session", NULL, &reply));
    char *reply_str = ecs_strbuf_get(&reply.body);
    unsigned long long session = 0;
    test_int(sscanf(reply_str, "{\"session\":%llu}", &session), 1);
    ecs_os_free(reply_str);

    const unsigned char data[] = { 0x89, 'P', 'N', 'G', 0, 1, 2, 0, 0xff };
    test_int(0, ecs_shell_resource_create(world, (ecs_entity_t)session,
        "image/png", data, ECS_SIZEOF(data), NULL));

    char path[128];
    ecs_os_snprintf(path, 128, "/shell/session/%llu/resource/1", session);

    reply = ECS_HTTP_REPLY_INIT;
    test_int(0, ecs_http_server_request(srv, "GET", path, NULL, &reply));
    test_int(reply.code, 200);
    test_str(reply.content_type, "image/png");
    test_int(ecs_strbuf_written(&reply.body), ECS_SIZEOF(data));
    reply_str = ecs_strbuf_get(&reply.body);
    test_assert(!memcmp(reply_str, data, sizeof(data)));
    ecs_os_free(reply_str);

    ecs_rest_server_fini(srv);
    ecs_fini(world);
}

void Rest_shell_resource_unknown(void) {
    ecs_world_t *world = ecs_init();

    ecs_http_server_t *srv = ecs_rest_server_init(world, NULL);
    test_assert(srv != NULL);

    ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
    test_int(0, ecs_http_server_request(srv, "PUT",
        "/shell/session", NULL, &reply));
    char *reply_str = ecs_strbuf_get(&reply.body);
    unsigned long long session = 0;
    test_int(sscanf(reply_str, "{\"session\":%llu}", &session), 1);
    ecs_os_free(reply_str);

    test_int(0, ecs_shell_resource_create(world, (ecs_entity_t)session,
        "text/plain", "Hello", 5, NULL));

    char path[128];
    ecs_os_snprintf(path, 128, "/shell/session/%llu/resource/2", session);

    reply = ECS_HTTP_REPLY_INIT;
    test_int(-1, ecs_http_server_request(srv, "GET", path, NULL, &reply));
    test_int(reply.code, 404);
    reply_str = ecs_strbuf_get(&reply.body);
    test_str(reply_str, "{\"error\":\"invalid resource\"}");
    ecs_os_free(reply_str);

    ecs_os_snprintf(path, 128, "/shell/session/%llu/resource/../%llu/1",
        session, session);

    reply = ECS_HTTP_REPLY_INIT;
    test_int(-1, ecs_http_server_request(srv, "GET", path, NULL, &reply));
    test_int(reply.code, 404);
    ecs_strbuf_reset(&reply.body);

    ecs_os_snprintf(path, 128, "/shell/session/%llu/resource/", session);

    reply = ECS_HTTP_REPLY_INIT;
    test_int(-1, ecs_http_server_request(srv, "GET", path, NULL, &reply));
    test_int(reply.code, 404);
    ecs_strbuf_reset(&reply.body);

    ecs_rest_server_fini(srv);
    ecs_fini(world);
}

void Rest_shell_resource_other_session(void) {
    ecs_world_t *world = ecs_init();

    ecs_http_server_t *srv = ecs_rest_server_init(world, NULL);
    test_assert(srv != NULL);

    ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
    test_int(0, ecs_http_server_request(srv, "PUT",
        "/shell/session", NULL, &reply));
    char *reply_str = ecs_strbuf_get(&reply.body);
    unsigned long long session_a = 0;
    test_int(sscanf(reply_str, "{\"session\":%llu}", &session_a), 1);
    ecs_os_free(reply_str);

    reply = ECS_HTTP_REPLY_INIT;
    test_int(0, ecs_http_server_request(srv, "PUT",
        "/shell/session", NULL, &reply));
    reply_str = ecs_strbuf_get(&reply.body);
    unsigned long long session_b = 0;
    test_int(sscanf(reply_str, "{\"session\":%llu}", &session_b), 1);
    ecs_os_free(reply_str);

    test_int(0, ecs_shell_resource_create(world, (ecs_entity_t)session_a,
        "text/plain", "Hello", 5, NULL));

    char path[128];
    ecs_os_snprintf(path, 128, "/shell/session/%llu/resource/1", session_b);

    reply = ECS_HTTP_REPLY_INIT;
    test_int(-1, ecs_http_server_request(srv, "GET", path, NULL, &reply));
    test_int(reply.code, 404);
    ecs_strbuf_reset(&reply.body);

    ecs_os_snprintf(path, 128, "/shell/session/%llu/resource/1", session_a);

    reply = ECS_HTTP_REPLY_INIT;
    test_int(0, ecs_http_server_request(srv, "GET", path, NULL, &reply));
    test_int(reply.code, 200);
    reply_str = ecs_strbuf_get(&reply.body);
    test_str(reply_str, "Hello");
    ecs_os_free(reply_str);

    ecs_rest_server_fini(srv);
    ecs_fini(world);
}

void Rest_shell_resource_deleted_session(void) {
    ecs_world_t *world = ecs_init();

    ecs_http_server_t *srv = ecs_rest_server_init(world, NULL);
    test_assert(srv != NULL);

    ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
    test_int(0, ecs_http_server_request(srv, "PUT",
        "/shell/session", NULL, &reply));
    char *reply_str = ecs_strbuf_get(&reply.body);
    unsigned long long session = 0;
    test_int(sscanf(reply_str, "{\"session\":%llu}", &session), 1);
    ecs_os_free(reply_str);

    test_int(0, ecs_shell_resource_create(world, (ecs_entity_t)session,
        "text/plain", "Hello", 5, NULL));

    char path[128];
    ecs_os_snprintf(path, 128, "/shell/session/%llu", session);

    reply = ECS_HTTP_REPLY_INIT;
    test_int(0, ecs_http_server_request(srv, "DELETE", path, NULL, &reply));
    test_int(reply.code, 200);
    ecs_strbuf_reset(&reply.body);

    char file[128];
    ecs_os_snprintf(file, 128, ".flecs/shell/%llu/1", session);
    test_assert(ecs_os_fopen(file, "rb") == NULL);
    test_int(Rest_fs_file_count(), 0);
    test_int(Rest_fs_dir_count(), 2);

    ecs_os_snprintf(path, 128, "/shell/session/%llu/resource/1", session);

    reply = ECS_HTTP_REPLY_INIT;
    test_int(-1, ecs_http_server_request(srv, "GET", path, NULL, &reply));
    test_int(reply.code, 404);
    reply_str = ecs_strbuf_get(&reply.body);
    test_str(reply_str, "{\"error\":\"invalid command session\"}");
    ecs_os_free(reply_str);

    ecs_rest_server_fini(srv);
    ecs_fini(world);
}

void Rest_shell_resource_command(void) {
    ecs_world_t *world = ecs_init();

    ecs_http_server_t *srv = ecs_rest_server_init(world, NULL);
    test_assert(srv != NULL);

    ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
    test_int(0, ecs_http_server_request(srv, "PUT",
        "/shell/session", NULL, &reply));
    char *reply_str = ecs_strbuf_get(&reply.body);
    unsigned long long session = 0;
    test_int(sscanf(reply_str, "{\"session\":%llu}", &session), 1);
    ecs_os_free(reply_str);

    ecs_entity_t commands = ecs_entity(world, { .name = "commands" });
    ecs_function(world, {
        .name = "capture",
        .parent = commands,
        .return_type = ecs_id(ecs_shell_resource_t),
        .callback = Rest_resource_callback
    });

    EcsScriptShell *shell = ecs_singleton_ensure(world, EcsScriptShell);
    ecs_vec_append_t(NULL, &shell->command_paths, ecs_entity_t)[0] = commands;
    ecs_singleton_modified(world, EcsScriptShell);

    char path[128];
    ecs_os_snprintf(path, 128, "/shell/session/%llu", session);

    reply = ECS_HTTP_REPLY_INIT;
    test_int(0, ecs_http_server_request(srv, "PUT", path, "capture", &reply));
    test_int(reply.code, 200);
    reply_str = ecs_strbuf_get(&reply.body);
    test_str(reply_str, "{\"results\":[{"
        "\"type\":\"flecs.script.shell.resource\", "
        "\"result\":{\"name\":\"1\", \"content_type\":\"text/plain\"}}], "
        "\"errors\":[]}");
    ecs_os_free(reply_str);

    ecs_os_snprintf(path, 128, "/shell/session/%llu/resource/1", session);

    reply = ECS_HTTP_REPLY_INIT;
    test_int(0, ecs_http_server_request(srv, "GET", path, NULL, &reply));
    test_int(reply.code, 200);
    test_str(reply.content_type, "text/plain");
    reply_str = ecs_strbuf_get(&reply.body);
    test_str(reply_str, "Hello");
    ecs_os_free(reply_str);

    ecs_rest_server_fini(srv);
    ecs_fini(world);
}

void Rest_command_run_multiple(void) {
    ecs_world_t *world = ecs_init();

    ecs_http_server_t *srv = ecs_rest_server_init(world, NULL);
    test_assert(srv != NULL);

    ecs_entity(world, { .name = "parent.child" });

    ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
    test_int(0, ecs_http_server_request(srv, "PUT",
        "/shell/session", NULL, &reply));
    char *reply_str = ecs_strbuf_get(&reply.body);
    unsigned long long session = 0;
    test_int(sscanf(reply_str, "{\"session\":%llu}", &session), 1);
    ecs_os_free(reply_str);

    char path[128];
    ecs_os_snprintf(path, 128, "/shell/session/%llu", session);

    reply = ECS_HTTP_REPLY_INIT;
    test_int(0, ecs_http_server_request(srv, "PUT", path,
        "cd parent; rm child; ls", &reply));
    test_int(reply.code, 200);
    reply_str = ecs_strbuf_get(&reply.body);
    test_str(reply_str, "{\"results\":["
        "{\"type\":\"flecs.meta.entity\", \"result\":\"parent\"}, "
        "{}, "
        "{\"type\":\"flecs.script.typecache.vector<entity>\", "
        "\"result\":[], \"more\":false}], \"errors\":[]}");
    ecs_os_free(reply_str);
    test_assert(ecs_lookup(world, "parent.child") == 0);

    ecs_rest_server_fini(srv);
    ecs_fini(world);
}

void Rest_command_run_multiple_error(void) {
    ecs_world_t *world = ecs_init();

    ecs_http_server_t *srv = ecs_rest_server_init(world, NULL);
    test_assert(srv != NULL);

    ecs_entity(world, { .name = "parent.child" });

    ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
    test_int(0, ecs_http_server_request(srv, "PUT",
        "/shell/session", NULL, &reply));
    char *reply_str = ecs_strbuf_get(&reply.body);
    unsigned long long session = 0;
    test_int(sscanf(reply_str, "{\"session\":%llu}", &session), 1);
    ecs_os_free(reply_str);

    char path[128];
    ecs_os_snprintf(path, 128, "/shell/session/%llu", session);

    reply = ECS_HTTP_REPLY_INIT;
    test_int(-1, ecs_http_server_request(srv, "PUT", path,
        "pwd; foo; rm parent.child", &reply));
    test_int(reply.code, 400);
    reply_str = ecs_strbuf_get(&reply.body);
    test_str(reply_str, "{\"results\":["
        "{\"type\":\"flecs.meta.entity\", \"result\":\"#0\"}, {}], "
        "\"errors\":[\"unknown command 'foo'\"]}");
    ecs_os_free(reply_str);
    test_assert(ecs_lookup(world, "parent.child") != 0);

    ecs_rest_server_fini(srv);
    ecs_fini(world);
}

void Rest_command_run_multiple_offset_limit(void) {
    ecs_world_t *world = ecs_init();

    ecs_http_server_t *srv = ecs_rest_server_init(world, NULL);
    test_assert(srv != NULL);

    ecs_entity(world, { .name = "foo.a" });
    ecs_entity(world, { .name = "foo.b" });
    ecs_entity(world, { .name = "bar.c" });
    ecs_entity(world, { .name = "bar.d" });

    ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
    test_int(0, ecs_http_server_request(srv, "PUT",
        "/shell/session", NULL, &reply));
    char *reply_str = ecs_strbuf_get(&reply.body);
    unsigned long long session = 0;
    test_int(sscanf(reply_str, "{\"session\":%llu}", &session), 1);
    ecs_os_free(reply_str);

    char path[128];
    ecs_os_snprintf(path, 128, "/shell/session/%llu?limit=1", session);

    reply = ECS_HTTP_REPLY_INIT;
    test_int(0, ecs_http_server_request(srv, "PUT", path,
        "ls foo; ls bar", &reply));
    test_int(reply.code, 200);
    reply_str = ecs_strbuf_get(&reply.body);
    test_str(reply_str, "{\"results\":["
        "{\"type\":\"flecs.script.typecache.vector<entity>\", "
        "\"result\":[\"foo.a\"], \"more\":true}, "
        "{\"type\":\"flecs.script.typecache.vector<entity>\", "
        "\"result\":[\"bar.c\"], \"more\":true}], \"errors\":[]}");
    ecs_os_free(reply_str);

    ecs_rest_server_fini(srv);
    ecs_fini(world);
}

void Rest_root_shell_instructions(void) {
    ecs_world_t *world = ecs_init();

    ecs_http_server_t *srv = ecs_rest_server_init(world, NULL);
    test_assert(srv != NULL);

    ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
    test_int(0, ecs_http_server_request(srv, "GET", "/", NULL, &reply));
    test_int(reply.code, 200);
    char *reply_str = ecs_strbuf_get(&reply.body);
    test_assert(strstr(reply_str, "PUT /shell/session\n") != NULL);
    test_assert(strstr(reply_str, "PUT /shell/session/<id>\n") != NULL);
    ecs_os_free(reply_str);

    ecs_rest_server_fini(srv);
    ecs_fini(world);
}

void Rest_get_stats_wo_stats_import(void) {
    ecs_world_t *world = ecs_init();

    ecs_http_server_t *srv = ecs_rest_server_init(world, NULL);
    test_assert(srv != NULL);

    ecs_log_set_level(-4);

    ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
    test_int(-1, ecs_http_server_request(srv, "GET",
        "/stats/world", NULL, &reply));
    test_int(reply.code, 400);
    ecs_strbuf_reset(&reply.body);

    ecs_http_reply_t reply_2 = ECS_HTTP_REPLY_INIT;
    test_int(-1, ecs_http_server_request(srv, "GET",
        "/stats/pipeline", NULL, &reply_2));
    test_int(reply_2.code, 400);
    ecs_strbuf_reset(&reply_2.body);

    ecs_rest_server_fini(srv);
    ecs_fini(world);
}

void Rest_get_component_invalid_id(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t foo = ecs_entity(world, { .name = "Foo" });

    ecs_http_server_t *srv = ecs_rest_server_init(world, NULL);
    test_assert(srv != NULL);

    ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
    test_int(-1, ecs_http_server_request(srv, "GET",
        "/component/Foo?component=#0", NULL, &reply));
    test_int(reply.code, 400);
    ecs_strbuf_reset(&reply.body);

    ecs_rest_server_fini(srv);
    ecs_fini(world);
}

void Rest_put_component_wildcard(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t foo = ecs_entity(world, { .name = "Foo" });

    ecs_http_server_t *srv = ecs_rest_server_init(world, NULL);
    test_assert(srv != NULL);

    ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
    test_int(-1, ecs_http_server_request(srv, "PUT",
        "/component/Foo?component=*", NULL, &reply));
    test_int(reply.code, 400);
    ecs_strbuf_reset(&reply.body);

    ecs_rest_server_fini(srv);
    ecs_fini(world);
}

void Rest_put_component_invalid_id(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t foo = ecs_entity(world, { .name = "Foo" });

    ecs_http_server_t *srv = ecs_rest_server_init(world, NULL);
    test_assert(srv != NULL);

    ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
    test_int(-1, ecs_http_server_request(srv, "PUT",
        "/component/Foo?component=#0", NULL, &reply));
    test_int(reply.code, 400);
    ecs_strbuf_reset(&reply.body);

    ecs_rest_server_fini(srv);
    ecs_fini(world);
}

void Rest_put_component_invalid_pair(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t foo = ecs_entity(world, { .name = "Foo" });

    ecs_http_server_t *srv = ecs_rest_server_init(world, NULL);
    test_assert(srv != NULL);

    ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
    test_int(-1, ecs_http_server_request(srv, "PUT",
        "/component/Foo?component=(flecs.core.ChildOf,#0)", NULL, &reply));
    test_int(reply.code, 400);
    ecs_strbuf_reset(&reply.body);

    ecs_rest_server_fini(srv);
    ecs_fini(world);
}

void Rest_put_component_value_wildcard(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ecs_struct(world, {
        .entity = ecs_id(Position),
        .members = {
            { "x", ecs_id(ecs_f32_t) },
            { "y", ecs_id(ecs_f32_t) }
        }
    });
    ecs_entity_t foo = ecs_entity(world, { .name = "Foo" });
    ecs_set(world, foo, Position, {10, 20});

    ecs_http_server_t *srv = ecs_rest_server_init(world, NULL);
    test_assert(srv != NULL);

    ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
    test_int(-1, ecs_http_server_request(srv, "PUT",
        "/component/Foo?component=(Position,*)&value={\"x\":1}", NULL, &reply));
    test_int(reply.code, 400);
    ecs_strbuf_reset(&reply.body);

    ecs_rest_server_fini(srv);
    ecs_fini(world);
}

void Rest_delete_component_invalid_id(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t foo = ecs_entity(world, { .name = "Foo" });

    ecs_http_server_t *srv = ecs_rest_server_init(world, NULL);
    test_assert(srv != NULL);

    ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
    test_int(-1, ecs_http_server_request(srv, "DELETE",
        "/component/Foo?component=#0", NULL, &reply));
    test_int(reply.code, 400);
    ecs_strbuf_reset(&reply.body);

    ecs_rest_server_fini(srv);
    ecs_fini(world);
}

void Rest_delete_component_invalid_pair(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t foo = ecs_entity(world, { .name = "Foo" });

    ecs_http_server_t *srv = ecs_rest_server_init(world, NULL);
    test_assert(srv != NULL);

    ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
    test_int(-1, ecs_http_server_request(srv, "DELETE",
        "/component/Foo?component=(flecs.core.ChildOf,#0)", NULL, &reply));
    test_int(reply.code, 400);
    ecs_strbuf_reset(&reply.body);

    ecs_rest_server_fini(srv);
    ecs_fini(world);
}

void Rest_toggle_invalid_id(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t foo = ecs_entity(world, { .name = "Foo" });

    ecs_http_server_t *srv = ecs_rest_server_init(world, NULL);
    test_assert(srv != NULL);

    ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
    test_int(-1, ecs_http_server_request(srv, "PUT",
        "/toggle/Foo?component=#0", NULL, &reply));
    test_int(reply.code, 400);
    ecs_strbuf_reset(&reply.body);

    ecs_rest_server_fini(srv);
    ecs_fini(world);
}

void Rest_toggle_wildcard_pair(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t tgl = ecs_entity(world, { .name = "Tgl" });
    ecs_add_id(world, tgl, EcsCanToggle);
    ecs_entity_t foo = ecs_entity(world, { .name = "Foo" });
    ecs_add_id(world, foo, tgl);

    ecs_http_server_t *srv = ecs_rest_server_init(world, NULL);
    test_assert(srv != NULL);

    ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
    test_int(-1, ecs_http_server_request(srv, "PUT",
        "/toggle/Foo?component=(Tgl,*)", NULL, &reply));
    test_int(reply.code, 400);
    ecs_strbuf_reset(&reply.body);

    ecs_rest_server_fini(srv);
    ecs_fini(world);
}

void Rest_toggle_pair_not_owned(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t tgl = ecs_entity(world, { .name = "Tgl" });
    ecs_add_id(world, tgl, EcsCanToggle);
    ecs_entity_t foo = ecs_entity(world, { .name = "Foo" });
    ecs_add_id(world, foo, tgl);
    ecs_entity(world, { .name = "Bar" });

    ecs_http_server_t *srv = ecs_rest_server_init(world, NULL);
    test_assert(srv != NULL);

    ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
    test_int(-1, ecs_http_server_request(srv, "PUT",
        "/toggle/Foo?component=(Tgl,Bar)&enable=false", NULL, &reply));
    test_int(reply.code, 400);
    ecs_strbuf_reset(&reply.body);

    ecs_rest_server_fini(srv);
    ecs_fini(world);
}

void Rest_toggle_builtin_entity(void) {
    ecs_world_t *world = ecs_init();

    ecs_http_server_t *srv = ecs_rest_server_init(world, NULL);
    test_assert(srv != NULL);

    ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
    test_int(-1, ecs_http_server_request(srv, "PUT",
        "/toggle/flecs/core/World?enable=false", NULL, &reply));
    test_int(reply.code, 400);
    ecs_strbuf_reset(&reply.body);

    test_assert(!ecs_has_id(world, EcsWorld, EcsDisabled));

    ecs_rest_server_fini(srv);
    ecs_fini(world);
}

void Rest_delete_entity_w_panic(void) {
    ecs_world_t *world = ecs_init();

    ecs_http_server_t *srv = ecs_rest_server_init(world, NULL);
    test_assert(srv != NULL);

    ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
    test_int(-1, ecs_http_server_request(srv, "DELETE",
        "/entity/flecs", NULL, &reply));
    test_int(reply.code, 400);
    ecs_strbuf_reset(&reply.body);

    test_assert(ecs_is_alive(world, EcsFlecs));

    ecs_rest_server_fini(srv);
    ecs_fini(world);
}

void Rest_delete_builtin_entity(void) {
    ecs_world_t *world = ecs_init();

    ecs_http_server_t *srv = ecs_rest_server_init(world, NULL);
    test_assert(srv != NULL);

    ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
    test_int(-1, ecs_http_server_request(srv, "DELETE",
        "/entity/flecs/core/World", NULL, &reply));
    test_int(reply.code, 400);
    ecs_strbuf_reset(&reply.body);

    test_assert(ecs_is_alive(world, EcsWorld));

    ecs_rest_server_fini(srv);
    ecs_fini(world);
}

void Rest_delete_builtin_module(void) {
    ecs_world_t *world = ecs_init();

    ecs_http_server_t *srv = ecs_rest_server_init(world, NULL);
    test_assert(srv != NULL);

    ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
    test_int(-1, ecs_http_server_request(srv, "DELETE",
        "/entity/flecs/core", NULL, &reply));
    test_int(reply.code, 400);
    ecs_strbuf_reset(&reply.body);

    test_assert(ecs_is_alive(world, EcsFlecsCore));

    ecs_rest_server_fini(srv);
    ecs_fini(world);
}

void Rest_delete_entity_used_by_query(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ecs_entity_t foo = ecs_entity(world, { .name = "Foo" });
    ecs_set(world, foo, Position, {10, 20});
    ecs_query_t *q = ecs_query(world, { .terms = {{ ecs_id(Position) }}, .cache_kind = EcsQueryCacheAuto });
    test_assert(q != NULL);

    ecs_http_server_t *srv = ecs_rest_server_init(world, NULL);
    test_assert(srv != NULL);

    ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
    test_int(-1, ecs_http_server_request(srv, "DELETE",
        "/entity/Position", NULL, &reply));
    test_int(reply.code, 400);
    ecs_strbuf_reset(&reply.body);

    test_assert(ecs_is_alive(world, ecs_id(Position)));
    ecs_query_fini(q);

    ecs_rest_server_fini(srv);
    ecs_fini(world);
}

void Rest_delete_entity_w_user_panic(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t bar = ecs_entity(world, { .name = "Bar" });
    ecs_add_pair(world, bar, EcsOnDelete, EcsPanic);
    ecs_entity_t foo = ecs_new(world);
    ecs_add_id(world, foo, bar);

    ecs_http_server_t *srv = ecs_rest_server_init(world, NULL);
    test_assert(srv != NULL);

    ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
    test_int(-1, ecs_http_server_request(srv, "DELETE",
        "/entity/Bar", NULL, &reply));
    test_int(reply.code, 400);
    ecs_strbuf_reset(&reply.body);

    test_assert(ecs_is_alive(world, bar));

    ecs_rest_server_fini(srv);
    ecs_fini(world);
}

void Rest_delete_entity_w_on_delete_target_panic(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t rel = ecs_entity(world, { .name = "Rel" });
    ecs_add_pair(world, rel, EcsOnDeleteTarget, EcsPanic);
    ecs_entity_t tgt = ecs_entity(world, { .name = "Tgt" });
    ecs_entity_t foo = ecs_new(world);
    ecs_add_pair(world, foo, rel, tgt);

    ecs_http_server_t *srv = ecs_rest_server_init(world, NULL);
    test_assert(srv != NULL);

    ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
    test_int(-1, ecs_http_server_request(srv, "DELETE",
        "/entity/Tgt", NULL, &reply));
    test_int(reply.code, 400);
    ecs_strbuf_reset(&reply.body);

    test_assert(ecs_is_alive(world, tgt));

    ecs_rest_server_fini(srv);
    ecs_fini(world);
}

void Rest_delete_entity_used_by_builtin_entity(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t tag = ecs_entity(world, { .name = "Tag" });
    ecs_add_id(world, EcsWorld, tag);

    ecs_http_server_t *srv = ecs_rest_server_init(world, NULL);
    test_assert(srv != NULL);

    ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
    test_int(-1, ecs_http_server_request(srv, "DELETE",
        "/entity/Tag", NULL, &reply));
    test_int(reply.code, 400);
    ecs_strbuf_reset(&reply.body);

    test_assert(ecs_is_alive(world, tag));

    ecs_rest_server_fini(srv);
    ecs_fini(world);
}

void Rest_delete_component_from_builtin_entity(void) {
    ecs_world_t *world = ecs_init();

    ecs_http_server_t *srv = ecs_rest_server_init(world, NULL);
    test_assert(srv != NULL);

    ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
    test_int(-1, ecs_http_server_request(srv, "DELETE",
        "/component/flecs/core/World?component=(flecs.core.ChildOf,flecs.core)", NULL, &reply));
    test_int(reply.code, 400);
    ecs_strbuf_reset(&reply.body);

    test_assert(ecs_has_pair(world, EcsWorld, EcsChildOf, EcsFlecsCore));

    ecs_rest_server_fini(srv);
    ecs_fini(world);
}

void Rest_delete_component_from_component_in_use(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ecs_entity_t foo = ecs_entity(world, { .name = "Foo" });
    ecs_set(world, foo, Position, {10, 20});

    ecs_http_server_t *srv = ecs_rest_server_init(world, NULL);
    test_assert(srv != NULL);

    ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
    test_int(-1, ecs_http_server_request(srv, "DELETE",
        "/component/Position?component=flecs.core.Component", NULL, &reply));
    test_int(reply.code, 400);
    ecs_strbuf_reset(&reply.body);

    test_assert(ecs_has(world, ecs_id(Position), EcsComponent));

    ecs_rest_server_fini(srv);
    ecs_fini(world);
}

void Rest_delete_wildcard_from_component_in_use(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ecs_entity_t foo = ecs_entity(world, { .name = "Foo" });
    ecs_set(world, foo, Position, {10, 20});

    ecs_http_server_t *srv = ecs_rest_server_init(world, NULL);
    test_assert(srv != NULL);

    ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
    test_int(-1, ecs_http_server_request(srv, "DELETE",
        "/component/Position?component=*", NULL, &reply));
    test_int(reply.code, 400);
    ecs_strbuf_reset(&reply.body);

    test_assert(ecs_has(world, ecs_id(Position), EcsComponent));

    ecs_rest_server_fini(srv);
    ecs_fini(world);
}

void Rest_add_childof_self(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t foo = ecs_entity(world, { .name = "Foo" });

    ecs_http_server_t *srv = ecs_rest_server_init(world, NULL);
    test_assert(srv != NULL);

    ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
    test_int(-1, ecs_http_server_request(srv, "PUT",
        "/component/Foo?component=(flecs.core.ChildOf,Foo)", NULL, &reply));
    test_int(reply.code, 400);
    ecs_strbuf_reset(&reply.body);

    test_assert(!ecs_has_pair(world, foo, EcsChildOf, foo));

    ecs_rest_server_fini(srv);
    ecs_fini(world);
}

void Rest_add_childof_cycle(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t foo = ecs_entity(world, { .name = "Foo" });
    ecs_entity_t bar = ecs_entity(world, { .name = "Foo.Bar" });

    ecs_http_server_t *srv = ecs_rest_server_init(world, NULL);
    test_assert(srv != NULL);

    ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
    test_int(-1, ecs_http_server_request(srv, "PUT",
        "/component/Foo?component=(flecs.core.ChildOf,Foo.Bar)", NULL, &reply));
    test_int(reply.code, 400);
    ecs_strbuf_reset(&reply.body);

    test_assert(!ecs_has_pair(world, foo, EcsChildOf, bar));

    ecs_rest_server_fini(srv);
    ecs_fini(world);
}

void Rest_add_isa_self(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t foo = ecs_entity(world, { .name = "Foo" });

    ecs_http_server_t *srv = ecs_rest_server_init(world, NULL);
    test_assert(srv != NULL);

    ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
    test_int(-1, ecs_http_server_request(srv, "PUT",
        "/component/Foo?component=(flecs.core.IsA,Foo)", NULL, &reply));
    test_int(reply.code, 400);
    ecs_strbuf_reset(&reply.body);

    test_assert(!ecs_has_pair(world, foo, EcsIsA, foo));

    ecs_rest_server_fini(srv);
    ecs_fini(world);
}

void Rest_add_isa_cycle(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t foo = ecs_entity(world, { .name = "Foo" });
    ecs_entity_t bar = ecs_entity(world, { .name = "Bar" });
    ecs_add_pair(world, bar, EcsIsA, foo);

    ecs_http_server_t *srv = ecs_rest_server_init(world, NULL);
    test_assert(srv != NULL);

    ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
    test_int(-1, ecs_http_server_request(srv, "PUT",
        "/component/Foo?component=(flecs.core.IsA,Bar)", NULL, &reply));
    test_int(reply.code, 400);
    ecs_strbuf_reset(&reply.body);

    test_assert(!ecs_has_pair(world, foo, EcsIsA, bar));

    ecs_rest_server_fini(srv);
    ecs_fini(world);
}

void Rest_add_childof_flecs_core(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t foo = ecs_entity(world, { .name = "Foo" });

    ecs_http_server_t *srv = ecs_rest_server_init(world, NULL);
    test_assert(srv != NULL);

    ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
    test_int(-1, ecs_http_server_request(srv, "PUT",
        "/component/Foo?component=(flecs.core.ChildOf,flecs.core)", NULL, &reply));
    test_int(reply.code, 400);
    ecs_strbuf_reset(&reply.body);

    test_assert(!ecs_has_pair(world, foo, EcsChildOf, EcsFlecsCore));

    ecs_rest_server_fini(srv);
    ecs_fini(world);
}

void Rest_reparent_flecs_module(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t foo = ecs_entity(world, { .name = "Foo" });

    ecs_http_server_t *srv = ecs_rest_server_init(world, NULL);
    test_assert(srv != NULL);

    ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
    test_int(-1, ecs_http_server_request(srv, "PUT",
        "/component/flecs?component=(flecs.core.ChildOf,Foo)", NULL, &reply));
    test_int(reply.code, 400);
    ecs_strbuf_reset(&reply.body);

    test_assert(!ecs_has_pair(world, EcsFlecs, EcsChildOf, foo));

    ecs_rest_server_fini(srv);
    ecs_fini(world);
}

void Rest_reparent_flecs_core_module(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t foo = ecs_entity(world, { .name = "Foo" });

    ecs_http_server_t *srv = ecs_rest_server_init(world, NULL);
    test_assert(srv != NULL);

    ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
    test_int(-1, ecs_http_server_request(srv, "PUT",
        "/component/flecs/core?component=(flecs.core.ChildOf,Foo)", NULL, &reply));
    test_int(reply.code, 400);
    ecs_strbuf_reset(&reply.body);

    test_assert(!ecs_has_pair(world, EcsFlecsCore, EcsChildOf, foo));

    ecs_rest_server_fini(srv);
    ecs_fini(world);
}

void Rest_reparent_builtin_entity(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t foo = ecs_entity(world, { .name = "Foo" });

    ecs_http_server_t *srv = ecs_rest_server_init(world, NULL);
    test_assert(srv != NULL);

    ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
    test_int(-1, ecs_http_server_request(srv, "PUT",
        "/component/flecs/core/World?component=(flecs.core.ChildOf,Foo)", NULL, &reply));
    test_int(reply.code, 400);
    ecs_strbuf_reset(&reply.body);

    test_assert(ecs_has_pair(world, EcsWorld, EcsChildOf, EcsFlecsCore));

    ecs_rest_server_fini(srv);
    ecs_fini(world);
}

void Rest_add_isa_final(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t foo = ecs_entity(world, { .name = "Foo" });
    ecs_entity_t bar = ecs_entity(world, { .name = "Bar" });
    ecs_add_id(world, bar, EcsFinal);

    ecs_http_server_t *srv = ecs_rest_server_init(world, NULL);
    test_assert(srv != NULL);

    ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
    test_int(-1, ecs_http_server_request(srv, "PUT",
        "/component/Foo?component=(flecs.core.IsA,Bar)", NULL, &reply));
    test_int(reply.code, 400);
    ecs_strbuf_reset(&reply.body);

    test_assert(!ecs_has_pair(world, foo, EcsIsA, bar));

    ecs_rest_server_fini(srv);
    ecs_fini(world);
}

void Rest_add_target_trait_by_itself(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t foo = ecs_entity(world, { .name = "Foo" });
    ecs_entity_t bar = ecs_entity(world, { .name = "Bar" });
    ecs_add_id(world, bar, EcsTarget);

    ecs_http_server_t *srv = ecs_rest_server_init(world, NULL);
    test_assert(srv != NULL);

    ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
    test_int(-1, ecs_http_server_request(srv, "PUT",
        "/component/Foo?component=Bar", NULL, &reply));
    test_int(reply.code, 400);
    ecs_strbuf_reset(&reply.body);

    test_assert(!ecs_has_id(world, foo, bar));

    ecs_rest_server_fini(srv);
    ecs_fini(world);
}


void Rest_add_target_trait_as_relationship(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t foo = ecs_entity(world, { .name = "Foo" });
    ecs_entity_t bar = ecs_entity(world, { .name = "Bar" });
    ecs_add_id(world, bar, EcsTarget);
    ecs_entity(world, { .name = "Tgt" });

    ecs_http_server_t *srv = ecs_rest_server_init(world, NULL);
    test_assert(srv != NULL);

    ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
    test_int(-1, ecs_http_server_request(srv, "PUT",
        "/component/Foo?component=(Bar,Tgt)", NULL, &reply));
    test_int(reply.code, 400);
    ecs_strbuf_reset(&reply.body);

    ecs_rest_server_fini(srv);
    ecs_fini(world);
}

void Rest_add_relationship_trait_as_target(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t foo = ecs_entity(world, { .name = "Foo" });
    ecs_entity_t bar = ecs_entity(world, { .name = "Bar" });
    ecs_add_id(world, bar, EcsRelationship);
    ecs_entity(world, { .name = "Rel" });

    ecs_http_server_t *srv = ecs_rest_server_init(world, NULL);
    test_assert(srv != NULL);

    ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
    test_int(-1, ecs_http_server_request(srv, "PUT",
        "/component/Foo?component=(Rel,Bar)", NULL, &reply));
    test_int(reply.code, 400);
    ecs_strbuf_reset(&reply.body);

    ecs_rest_server_fini(srv);
    ecs_fini(world);
}

void Rest_add_singleton_to_other_entity(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t foo = ecs_entity(world, { .name = "Foo" });
    ecs_entity_t bar = ecs_entity(world, { .name = "Bar" });
    ecs_add_id(world, bar, EcsSingleton);

    ecs_http_server_t *srv = ecs_rest_server_init(world, NULL);
    test_assert(srv != NULL);

    ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
    test_int(-1, ecs_http_server_request(srv, "PUT",
        "/component/Foo?component=Bar", NULL, &reply));
    test_int(reply.code, 400);
    ecs_strbuf_reset(&reply.body);

    test_assert(!ecs_has_id(world, foo, bar));

    ecs_rest_server_fini(srv);
    ecs_fini(world);
}

void Rest_add_trait_to_component_in_use(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ecs_entity_t foo = ecs_entity(world, { .name = "Foo" });
    ecs_set(world, foo, Position, {10, 20});

    ecs_http_server_t *srv = ecs_rest_server_init(world, NULL);
    test_assert(srv != NULL);

    ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
    test_int(-1, ecs_http_server_request(srv, "PUT",
        "/component/Position?component=flecs.core.Sparse", NULL, &reply));
    test_int(reply.code, 400);
    ecs_strbuf_reset(&reply.body);

    test_assert(!ecs_has_id(world, ecs_id(Position), EcsSparse));

    ecs_rest_server_fini(srv);
    ecs_fini(world);
}

void Rest_add_trait_to_queried_component(void) {
    ecs_world_t *world = ecs_init();

    ecs_http_server_t *srv = ecs_rest_server_init(world, NULL);
    test_assert(srv != NULL);

    ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
    test_int(-1, ecs_http_server_request(srv, "PUT",
        "/component/flecs/core/IsA?component=flecs.core.Exclusive", NULL, &reply));
    test_int(reply.code, 400);
    ecs_strbuf_reset(&reply.body);

    test_assert(!ecs_has_id(world, EcsIsA, EcsExclusive));

    ecs_rest_server_fini(srv);
    ecs_fini(world);
}

void Rest_remove_trait_from_component_in_use(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t foo = ecs_entity(world, { .name = "Foo" });
    ecs_entity_t rel = ecs_entity(world, { .name = "Rel" });
    ecs_add_id(world, rel, EcsTraversable);
    ecs_entity_t tgt = ecs_entity(world, { .name = "Tgt" });
    ecs_add_pair(world, foo, rel, tgt);

    ecs_http_server_t *srv = ecs_rest_server_init(world, NULL);
    test_assert(srv != NULL);

    ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
    test_int(-1, ecs_http_server_request(srv, "DELETE",
        "/component/Rel?component=flecs.core.Traversable", NULL, &reply));
    test_int(reply.code, 400);
    ecs_strbuf_reset(&reply.body);

    test_assert(ecs_has_id(world, rel, EcsTraversable));

    ecs_rest_server_fini(srv);
    ecs_fini(world);
}

void Rest_add_final_to_inherited_entity(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t foo = ecs_entity(world, { .name = "Foo" });
    ecs_entity_t bar = ecs_entity(world, { .name = "Bar" });
    ecs_add_id(world, bar, EcsPrefab);
    ecs_add_pair(world, foo, EcsIsA, bar);

    ecs_http_server_t *srv = ecs_rest_server_init(world, NULL);
    test_assert(srv != NULL);

    ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
    test_int(-1, ecs_http_server_request(srv, "PUT",
        "/component/Bar?component=flecs.core.Final", NULL, &reply));
    test_int(reply.code, 400);
    ecs_strbuf_reset(&reply.body);

    test_assert(!ecs_has_id(world, bar, EcsFinal));

    ecs_rest_server_fini(srv);
    ecs_fini(world);
}

void Rest_set_component_invalid_size(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity(world, { .name = "Bar" });

    ecs_http_server_t *srv = ecs_rest_server_init(world, NULL);
    test_assert(srv != NULL);

    ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
    test_int(-1, ecs_http_server_request(srv, "PUT",
        "/component/Bar?component=flecs.core.Component&value={\"size\":3}", NULL, &reply));
    test_int(reply.code, 400);
    ecs_strbuf_reset(&reply.body);

    ecs_rest_server_fini(srv);
    ecs_fini(world);
}

void Rest_set_component_in_use(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ecs_entity_t foo = ecs_entity(world, { .name = "Foo" });
    ecs_set(world, foo, Position, {10, 20});

    ecs_http_server_t *srv = ecs_rest_server_init(world, NULL);
    test_assert(srv != NULL);

    ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
    test_int(-1, ecs_http_server_request(srv, "PUT",
        "/component/Position?component=flecs.core.Component&value={\"size\":1,\"alignment\":1}", NULL, &reply));
    test_int(reply.code, 400);
    ecs_strbuf_reset(&reply.body);

    test_int(ecs_get(world, ecs_id(Position), EcsComponent)->size, ECS_SIZEOF(Position));

    ecs_rest_server_fini(srv);
    ecs_fini(world);
}

void Rest_set_component_size_overflow(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t foo = ecs_entity(world, { .name = "Foo" });
    ecs_entity(world, { .name = "Bar" });

    ecs_http_server_t *srv = ecs_rest_server_init(world, NULL);
    test_assert(srv != NULL);

    ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
    test_int(-1, ecs_http_server_request(srv, "PUT",
        "/component/Bar?component=flecs.core.Component&value={\"size\":2147483647,\"alignment\":4}", NULL, &reply));
    test_int(reply.code, 400);
    ecs_strbuf_reset(&reply.body);

    ecs_rest_server_fini(srv);
    ecs_fini(world);
}

void Rest_set_parent_self(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t foo = ecs_entity(world, { .name = "Foo" });

    ecs_http_server_t *srv = ecs_rest_server_init(world, NULL);
    test_assert(srv != NULL);

    ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
    test_int(-1, ecs_http_server_request(srv, "PUT",
        "/component/Foo?component=flecs.core.Parent&value={\"value\":\"Foo\"}", NULL, &reply));
    test_int(reply.code, 400);
    ecs_strbuf_reset(&reply.body);

    ecs_rest_server_fini(srv);
    ecs_fini(world);
}

void Rest_set_parent_invalid(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t foo = ecs_entity(world, { .name = "Foo" });

    ecs_http_server_t *srv = ecs_rest_server_init(world, NULL);
    test_assert(srv != NULL);

    ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
    test_int(-1, ecs_http_server_request(srv, "PUT",
        "/component/Foo?component=flecs.core.Parent&value={\"value\":\"#0\"}", NULL, &reply));
    test_int(reply.code, 400);
    ecs_strbuf_reset(&reply.body);

    ecs_rest_server_fini(srv);
    ecs_fini(world);
}

void Rest_rename_builtin_entity(void) {
    ecs_world_t *world = ecs_init();

    ecs_http_server_t *srv = ecs_rest_server_init(world, NULL);
    test_assert(srv != NULL);

    ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
    test_int(-1, ecs_http_server_request(srv, "PUT",
        "/component/flecs/core/World?component=(flecs.core.Identifier,flecs.core.Name)&value={\"value\":\"Foo\"}", NULL, &reply));
    test_int(reply.code, 400);
    ecs_strbuf_reset(&reply.body);

    test_str(ecs_get_name(world, EcsWorld), "World");

    ecs_rest_server_fini(srv);
    ecs_fini(world);
}

void Rest_rename_flecs_module(void) {
    ecs_world_t *world = ecs_init();

    ecs_http_server_t *srv = ecs_rest_server_init(world, NULL);
    test_assert(srv != NULL);

    ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
    test_int(-1, ecs_http_server_request(srv, "PUT",
        "/component/flecs?component=(flecs.core.Identifier,flecs.core.Name)&value={\"value\":\"Foo\"}", NULL, &reply));
    test_int(reply.code, 400);
    ecs_strbuf_reset(&reply.body);

    test_str(ecs_get_name(world, EcsFlecs), "flecs");

    ecs_rest_server_fini(srv);
    ecs_fini(world);
}

void Rest_set_array_cyclic(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity(world, { .name = "Bar" });

    ecs_http_server_t *srv = ecs_rest_server_init(world, NULL);
    test_assert(srv != NULL);

    ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
    test_int(-1, ecs_http_server_request(srv, "PUT",
        "/component/Bar?component=flecs.meta.array&value={\"type\":\"Bar\",\"count\":2}", NULL, &reply));
    test_int(reply.code, 400);
    ecs_strbuf_reset(&reply.body);

    ecs_rest_server_fini(srv);
    ecs_fini(world);
}

void Rest_set_array_size_overflow(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ecs_entity(world, { .name = "Bar" });

    ecs_http_server_t *srv = ecs_rest_server_init(world, NULL);
    test_assert(srv != NULL);

    ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
    test_int(-1, ecs_http_server_request(srv, "PUT",
        "/component/Bar?component=flecs.meta.array&value={\"type\":\"Position\",\"count\":2000000000}", NULL, &reply));
    test_int(reply.code, 400);
    ecs_strbuf_reset(&reply.body);

    ecs_rest_server_fini(srv);
    ecs_fini(world);
}

void Rest_set_member_cyclic(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ecs_entity(world, { .name = "Position.z" });

    ecs_http_server_t *srv = ecs_rest_server_init(world, NULL);
    test_assert(srv != NULL);

    ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
    test_int(-1, ecs_http_server_request(srv, "PUT",
        "/component/Position/z?component=flecs.meta.member&value={\"type\":\"Position\"}", NULL, &reply));
    test_int(reply.code, 400);
    ecs_strbuf_reset(&reply.body);

    ecs_rest_server_fini(srv);
    ecs_fini(world);
}

void Rest_set_enum_non_primitive_underlying_type(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ecs_entity(world, { .name = "Bar" });

    ecs_http_server_t *srv = ecs_rest_server_init(world, NULL);
    test_assert(srv != NULL);

    ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
    test_int(-1, ecs_http_server_request(srv, "PUT",
        "/component/Bar?component=flecs.meta.enum&value={\"underlying_type\":\"Position\"}", NULL, &reply));
    test_int(reply.code, 400);
    ecs_strbuf_reset(&reply.body);

    ecs_rest_server_fini(srv);
    ecs_fini(world);
}

void Rest_set_enum_float_underlying_type(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity(world, { .name = "Bar" });

    ecs_http_server_t *srv = ecs_rest_server_init(world, NULL);
    test_assert(srv != NULL);

    ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
    test_int(-1, ecs_http_server_request(srv, "PUT",
        "/component/Bar?component=flecs.meta.enum&value={\"underlying_type\":\"flecs.meta.f32\"}", NULL, &reply));
    test_int(reply.code, 400);
    ecs_strbuf_reset(&reply.body);

    ecs_rest_server_fini(srv);
    ecs_fini(world);
}

void Rest_set_meta_type(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity(world, { .name = "Bar" });

    ecs_http_server_t *srv = ecs_rest_server_init(world, NULL);
    test_assert(srv != NULL);

    ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
    test_int(-1, ecs_http_server_request(srv, "PUT",
        "/component/Bar?component=flecs.meta.type&value={\"kind\":\"StructType\"}", NULL, &reply));
    test_int(reply.code, 400);
    ecs_strbuf_reset(&reply.body);

    ecs_rest_server_fini(srv);
    ecs_fini(world);
}

void Rest_query_negative_offset(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ecs_entity_t foo = ecs_entity(world, { .name = "Foo" });
    ecs_set(world, foo, Position, {10, 20});

    ecs_http_server_t *srv = ecs_rest_server_init(world, NULL);
    test_assert(srv != NULL);

    ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
    test_int(-1, ecs_http_server_request(srv, "GET",
        "/query?expr=Position&offset=-1", NULL, &reply));
    test_int(reply.code, 400);
    ecs_strbuf_reset(&reply.body);

    ecs_rest_server_fini(srv);
    ecs_fini(world);
}

void Rest_request_commands_negative_frame(void) {
    ecs_world_t *world = ecs_init();

    ecs_http_server_t *srv = ecs_rest_server_init(world, NULL);
    test_assert(srv != NULL);

    ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
    test_int(-1, ecs_http_server_request(srv, "GET",
        "/commands/frame/-1", NULL, &reply));
    test_int(reply.code, 404);
    ecs_strbuf_reset(&reply.body);

    ecs_rest_server_fini(srv);
    ecs_fini(world);
}

void Rest_command_history_offset_limit_overflow(void) {
    ecs_world_t *world = ecs_init();

    ecs_http_server_t *srv = ecs_rest_server_init(world, NULL);
    test_assert(srv != NULL);

    ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
    test_int(0, ecs_http_server_request(srv, "PUT",
        "/shell/session", NULL, &reply));
    char *reply_str = ecs_strbuf_get(&reply.body);
    unsigned long long session = 0;
    test_int(sscanf(reply_str, "{\"session\":%llu}", &session), 1);
    ecs_os_free(reply_str);

    char path[256];
    ecs_os_snprintf(path, 256, "/shell/session/%llu", session);

    reply = ECS_HTTP_REPLY_INIT;
    test_int(0, ecs_http_server_request(srv, "PUT", path, "pwd", &reply));
    ecs_strbuf_reset(&reply.body);

    ecs_os_snprintf(path, 256, "/shell/session/%llu/history"
        "?offset=2147483647&limit=2147483647&results=true", session);

    reply = ECS_HTTP_REPLY_INIT;
    test_int(0, ecs_http_server_request(srv, "GET", path, NULL, &reply));
    test_int(reply.code, 200);
    reply_str = ecs_strbuf_get(&reply.body);
    test_str(reply_str, "{\"history\":[], \"results\":[], \"more\":false}");
    ecs_os_free(reply_str);

    ecs_rest_server_fini(srv);
    ecs_fini(world);
}

void Rest_script_save_file_open_fails(void) {
    ecs_world_t *world = ecs_init();

    test_int(0, ecs_os_mkdir("dir"));
    FILE *f = ecs_os_fopen("dir/s.flecs", "w");
    test_assert(f != NULL);
    ecs_os_fwrite("Foo {}", 6, 1, f);
    ecs_os_fclose(f);

    ecs_entity_t s = ecs_entity(world, { .name = "s" });
    test_assert(s == ecs_script(world, {
        .entity = s,
        .filename = "dir/s.flecs"
    }));

    test_int(0, ecs_os_remove("dir/s.flecs"));
    test_int(0, ecs_os_rmdir("dir"));

    ecs_http_server_t *srv = ecs_rest_server_init(world, NULL);
    test_assert(srv != NULL);

    ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
    test_int(-1, ecs_http_server_request(srv, "PUT",
        "/script/s?save_file=true", "Bar {}", &reply));
    test_int(reply.code, 500);
    ecs_strbuf_reset(&reply.body);

    ecs_rest_server_fini(srv);
    ecs_fini(world);
}

void Rest_put_component_not_copyable(void) {
    ecs_world_t *world = ecs_init();

    ecs_query_t *q = ecs_query(world, {
        .entity = ecs_entity(world, { .name = "q" }),
        .expr = "flecs.core.Component"
    });
    test_assert(q != NULL);

    ecs_http_server_t *srv = ecs_rest_server_init(world, NULL);
    test_assert(srv != NULL);

    ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
    test_int(-1, ecs_http_server_request(srv, "PUT",
        "/component/q?component=(flecs.core.Poly,flecs.core.Query)&value={}",
        NULL, &reply));
    test_int(reply.code, 400);
    ecs_strbuf_reset(&reply.body);

    ecs_rest_server_fini(srv);
    ecs_fini(world);
}

static int32_t rest_dtor_invoked = 0;

static void Rest_count_dtor(
    void *ptr,
    int32_t count,
    const ecs_type_info_t *ti)
{
    (void)ptr;
    (void)ti;
    rest_dtor_invoked += count;
}

void Rest_put_component_not_copy_assignable(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ecs_struct(world, {
        .entity = ecs_id(Position),
        .members = {
            { "x", ecs_id(ecs_f32_t) },
            { "y", ecs_id(ecs_f32_t) }
        }
    });
    ecs_set_hooks(world, Position, {
        .dtor = Rest_count_dtor,
        .flags = ECS_TYPE_HOOK_COPY_ILLEGAL
    });

    ecs_entity_t foo = ecs_entity(world, { .name = "Foo" });
    ecs_add(world, foo, Position);

    ecs_http_server_t *srv = ecs_rest_server_init(world, NULL);
    test_assert(srv != NULL);

    rest_dtor_invoked = 0;

    ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
    test_int(-1, ecs_http_server_request(srv, "PUT",
        "/component/Foo?component=Position&value={\"x\":1}", NULL, &reply));
    test_int(reply.code, 400);
    ecs_strbuf_reset(&reply.body);

    test_int(rest_dtor_invoked, 0);

    ecs_rest_server_fini(srv);
    ecs_fini(world);
}

void Rest_put_component_not_constructible(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ecs_struct(world, {
        .entity = ecs_id(Position),
        .members = {
            { "x", ecs_id(ecs_f32_t) },
            { "y", ecs_id(ecs_f32_t) }
        }
    });
    ecs_set_hooks(world, Position, {
        .flags = ECS_TYPE_HOOK_CTOR_ILLEGAL
    });

    ecs_entity_t foo = ecs_entity(world, { .name = "Foo" });

    ecs_http_server_t *srv = ecs_rest_server_init(world, NULL);
    test_assert(srv != NULL);

    ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
    test_int(-1, ecs_http_server_request(srv, "PUT",
        "/component/Foo?component=Position&value={\"x\":1}", NULL, &reply));
    test_int(reply.code, 400);
    ecs_strbuf_reset(&reply.body);

    test_assert(!ecs_has(world, foo, Position));

    ecs_rest_server_fini(srv);
    ecs_fini(world);
}

void Rest_add_component_not_constructible(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ecs_set_hooks(world, Position, {
        .flags = ECS_TYPE_HOOK_CTOR_ILLEGAL
    });

    ecs_entity_t foo = ecs_entity(world, { .name = "Foo" });

    ecs_http_server_t *srv = ecs_rest_server_init(world, NULL);
    test_assert(srv != NULL);

    ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
    test_int(-1, ecs_http_server_request(srv, "PUT",
        "/component/Foo?component=Position", NULL, &reply));
    test_int(reply.code, 400);
    ecs_strbuf_reset(&reply.body);

    test_assert(!ecs_has(world, foo, Position));

    ecs_rest_server_fini(srv);
    ecs_fini(world);
}

void Rest_get_stats_user_period(void) {
    ecs_world_t *world = ecs_init();

    ECS_IMPORT(world, FlecsStats);

    ecs_http_server_t *srv = ecs_rest_server_init(world, NULL);
    test_assert(srv != NULL);

    ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
    test_int(0, ecs_http_server_request(srv, "PUT",
        "/entity/flecs/stats/PeriodFoo", NULL, &reply));
    test_int(reply.code, 200);
    ecs_strbuf_reset(&reply.body);

    reply = (ecs_http_reply_t)ECS_HTTP_REPLY_INIT;
    test_int(-1, ecs_http_server_request(srv, "GET",
        "/stats/world?period=Foo", NULL, &reply));
    test_int(reply.code, 400);
    ecs_strbuf_reset(&reply.body);

    reply = (ecs_http_reply_t)ECS_HTTP_REPLY_INIT;
    test_int(-1, ecs_http_server_request(srv, "GET",
        "/stats/pipeline?period=Foo", NULL, &reply));
    test_int(reply.code, 400);
    ecs_strbuf_reset(&reply.body);

    ecs_rest_server_fini(srv);
    ecs_fini(world);
}

static void Rest_SysNoop(ecs_iter_t *it) {
    (void)it;
}

void Rest_get_pipeline_stats_after_add_system(void) {
    ecs_world_t *world = ecs_init();

    ECS_IMPORT(world, FlecsStats);

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new(world);
    ecs_set(world, e, Position, {10, 20});

    ecs_http_server_t *srv = ecs_rest_server_init(world, NULL);
    test_assert(srv != NULL);

    ecs_progress(world, 1.0);

    ecs_entity_t s1 = ecs_system(world, {
        .entity = ecs_new_w_pair(world, EcsDependsOn, EcsOnUpdate),
        .query.terms = {{ ecs_id(Position) }},
        .callback = Rest_SysNoop
    });
    test_assert(s1 != 0);

    ecs_entity_t s2 = ecs_system(world, {
        .entity = ecs_new_w_pair(world, EcsDependsOn, EcsOnUpdate),
        .query.terms = {{ ecs_id(Position) }},
        .callback = Rest_SysNoop,
        .immediate = true
    });
    test_assert(s2 != 0);

    ecs_progress(world, 0.1);

    ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
    test_int(0, ecs_http_server_request(srv, "GET",
        "/stats/pipeline?name=flecs.pipeline.BuiltinPipeline&period=1m",
        NULL, &reply));
    test_int(reply.code, 200);
    ecs_strbuf_reset(&reply.body);

    ecs_rest_server_fini(srv);
    ecs_fini(world);
}

static ecs_entity_t rest_nested_tag = 0;
static ecs_entity_t rest_nested_tag_2 = 0;

static void Rest_AddNestedTag(ecs_iter_t *it) {
    int32_t i;
    for (i = 0; i < it->count; i ++) {
        ecs_add_id(it->stage, it->entities[i], rest_nested_tag);
    }
}

static void Rest_OnNestedTag(ecs_iter_t *it) {
    int32_t i;
    for (i = 0; i < it->count; i ++) {
        ecs_add_id(it->stage, it->entities[i], rest_nested_tag_2);
    }
}

void Rest_request_commands_nested(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    rest_nested_tag = Foo;
    rest_nested_tag_2 = Bar;

    ecs_observer(world, {
        .query.terms = {{ Foo }},
        .events = { EcsOnAdd },
        .callback = Rest_OnNestedTag
    });

    ecs_system(world, {
        .entity = ecs_new_w_pair(world, EcsDependsOn, EcsOnUpdate),
        .query.terms = {{ ecs_id(Position) }},
        .callback = Rest_AddNestedTag
    });

    ecs_entity_t e = ecs_new(world);
    ecs_set(world, e, Position, {10, 20});

    ecs_http_server_t *srv = ecs_rest_server_init(world, NULL);
    test_assert(srv != NULL);

    ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
    test_int(0, ecs_http_server_request(srv, "GET",
        "/commands/capture", NULL, &reply));
    test_int(reply.code, 200);
    char *reply_str = ecs_strbuf_get(&reply.body);
    test_str(reply_str, "{\"frame\":0}");
    ecs_os_free(reply_str);

    ecs_progress(world, 0);

    test_assert(ecs_has(world, e, Foo));
    test_assert(ecs_has(world, e, Bar));

    reply = (ecs_http_reply_t)ECS_HTTP_REPLY_INIT;
    test_int(0, ecs_http_server_request(srv, "GET",
        "/commands/frame/0", NULL, &reply));
    test_int(reply.code, 200);
    reply_str = ecs_strbuf_get(&reply.body);
    test_assert(reply_str != NULL);
    test_assert(strstr(reply_str, "\"id\":\"Foo\"") != NULL);
    test_assert(strstr(reply_str, "\"id\":\"Bar\"") != NULL);
    ecs_os_free(reply_str);

    ecs_rest_server_fini(srv);
    ecs_fini(world);
}

void Rest_request_commands_after_server_fini(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_http_server_t *srv = ecs_rest_server_init(world, NULL);
    test_assert(srv != NULL);

    ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
    test_int(0, ecs_http_server_request(srv, "GET",
        "/commands/capture", NULL, &reply));
    test_int(reply.code, 200);
    ecs_strbuf_reset(&reply.body);

    ecs_rest_server_fini(srv);

    ecs_entity_t e = ecs_new(world);

    ecs_frame_begin(world, 0);
    ecs_world_t *stage = ecs_get_stage(world, 0);
    ecs_set(stage, e, Position, {10, 20});
    ecs_merge(stage);
    ecs_frame_end(world);

    test_assert(ecs_has(world, e, Position));

    ecs_fini(world);
}

void Rest_remove_rest_component(void) {
    ecs_world_t *world = ecs_init();

    ecs_singleton_set(world, EcsRest, {27761});

    const EcsRest *rest = ecs_singleton_get(world, EcsRest);
    test_assert(rest != NULL);
    test_assert(rest->impl != NULL);
    ecs_http_server_t *srv = rest->impl->srv;
    test_assert(srv != NULL);

    ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
    test_int(-1, ecs_http_server_request(srv, "DELETE",
        "/component/flecs/rest/Rest?component=flecs.rest.Rest", NULL, &reply));
    test_int(reply.code, 400);
    ecs_strbuf_reset(&reply.body);

    test_assert(ecs_singleton_get(world, EcsRest) != NULL);

    reply = (ecs_http_reply_t)ECS_HTTP_REPLY_INIT;
    test_int(0, ecs_http_server_request(srv, "GET", "/entity/flecs/rest",
        NULL, &reply));
    test_int(reply.code, 200);
    ecs_strbuf_reset(&reply.body);

    ecs_fini(world);
}

void Rest_delete_rest_entity(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t e = ecs_entity(world, { .name = "Server" });
    ecs_set(world, e, EcsRest, {27762});

    const EcsRest *rest = ecs_get(world, e, EcsRest);
    test_assert(rest != NULL);
    test_assert(rest->impl != NULL);
    ecs_http_server_t *srv = rest->impl->srv;
    test_assert(srv != NULL);

    ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
    test_int(-1, ecs_http_server_request(srv, "DELETE",
        "/entity/Server", NULL, &reply));
    test_int(reply.code, 400);
    ecs_strbuf_reset(&reply.body);

    test_assert(ecs_is_alive(world, e));
    test_assert(ecs_has(world, e, EcsRest));

    reply = (ecs_http_reply_t)ECS_HTTP_REPLY_INIT;
    test_int(0, ecs_http_server_request(srv, "GET", "/entity/Server",
        NULL, &reply));
    test_int(reply.code, 200);
    ecs_strbuf_reset(&reply.body);

    ecs_fini(world);
}

void Rest_delete_rest_entity_parent(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t p = ecs_entity(world, { .name = "Parent" });
    ecs_entity_t e = ecs_entity(world, { .name = "Server", .parent = p });
    ecs_set(world, e, EcsRest, {27763});

    const EcsRest *rest = ecs_get(world, e, EcsRest);
    test_assert(rest != NULL);
    test_assert(rest->impl != NULL);
    ecs_http_server_t *srv = rest->impl->srv;
    test_assert(srv != NULL);

    ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
    test_int(-1, ecs_http_server_request(srv, "DELETE",
        "/entity/Parent", NULL, &reply));
    test_int(reply.code, 400);
    ecs_strbuf_reset(&reply.body);

    test_assert(ecs_is_alive(world, p));
    test_assert(ecs_is_alive(world, e));
    test_assert(ecs_has(world, e, EcsRest));

    ecs_fini(world);
}

void Rest_disable_rest_module_w_uninitialized_rest(void) {
    ecs_world_t *world = ecs_init();

    ECS_IMPORT(world, FlecsRest);
    ecs_entity_t rest = ecs_lookup(world, "flecs.rest");
    test_assert(rest != 0);

    ecs_entity_t e = ecs_new(world);
    ecs_add(world, e, EcsRest);

    ecs_enable(world, rest, false);
    test_assert(ecs_has_id(world, rest, EcsDisabled));

    ecs_enable(world, rest, true);
    test_assert(!ecs_has_id(world, rest, EcsDisabled));

    ecs_fini(world);
}

void Rest_disable_rest_module(void) {
    ecs_world_t *world = ecs_init();

    ecs_singleton_set(world, EcsRest, {27764});

    ecs_entity_t rest_module = ecs_lookup(world, "flecs.rest");
    test_assert(rest_module != 0);

    const EcsRest *rest = ecs_singleton_get(world, EcsRest);
    test_assert(rest != NULL);
    test_assert(rest->impl != NULL);
    ecs_http_server_t *srv = rest->impl->srv;
    test_assert(srv != NULL);

    ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
    test_int(-1, ecs_http_server_request(srv, "PUT",
        "/toggle/flecs/rest?enable=false", NULL, &reply));
    test_int(reply.code, 400);
    ecs_strbuf_reset(&reply.body);
    test_assert(!ecs_has_id(world, rest_module, EcsDisabled));

    reply = (ecs_http_reply_t)ECS_HTTP_REPLY_INIT;
    test_int(-1, ecs_http_server_request(srv, "PUT",
        "/component/flecs/rest?component=flecs.core.Disabled", NULL, &reply));
    test_int(reply.code, 400);
    ecs_strbuf_reset(&reply.body);
    test_assert(!ecs_has_id(world, rest_module, EcsDisabled));

    ecs_fini(world);
}

void Rest_put_entity_w_deleted_parent_id(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t e = ecs_new(world);
    ecs_delete(world, e);

    ecs_http_server_t *srv = ecs_rest_server_init(world, NULL);
    test_assert(srv != NULL);

    char path[128];
    ecs_os_snprintf(path, 128, "/entity/#%u/x", (uint32_t)e);

    ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
    test_int(-1, ecs_http_server_request(srv, "PUT", path, NULL, &reply));
    test_assert(reply.code >= 400);
    ecs_strbuf_reset(&reply.body);

    ecs_rest_server_fini(srv);
    ecs_fini(world);
}

void Rest_put_entity_w_deleted_parent_id_not_recycled(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t e1 = ecs_new(world);
    ecs_entity_t e2 = ecs_new(world);
    ecs_delete(world, e1);
    ecs_delete(world, e2);

    ecs_http_server_t *srv = ecs_rest_server_init(world, NULL);
    test_assert(srv != NULL);

    char path[128];
    ecs_os_snprintf(path, 128, "/entity/#%u/x", (uint32_t)e1);

    ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
    test_int(-1, ecs_http_server_request(srv, "PUT", path, NULL, &reply));
    test_assert(reply.code >= 400);
    ecs_strbuf_reset(&reply.body);

    ecs_rest_server_fini(srv);
    ecs_fini(world);
}

void Rest_put_entity_nested_in_flecs_core(void) {
    ecs_world_t *world = ecs_init();

    ecs_http_server_t *srv = ecs_rest_server_init(world, NULL);
    test_assert(srv != NULL);

    ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
    test_int(-1, ecs_http_server_request(srv, "PUT",
        "/entity/flecs/core/Foo/Bar", NULL, &reply));
    test_int(reply.code, 400);
    ecs_strbuf_reset(&reply.body);

    test_assert(ecs_lookup(world, "flecs.core.Foo") == 0);

    ecs_rest_server_fini(srv);
    ecs_fini(world);
}

void Rest_put_script_nested_in_flecs_core(void) {
    ecs_world_t *world = ecs_init();

    ecs_http_server_t *srv = ecs_rest_server_init(world, NULL);
    test_assert(srv != NULL);

    ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
    test_int(-1, ecs_http_server_request(srv, "PUT",
        "/script/flecs/core/Foo/script", "Bar {}", &reply));
    test_int(reply.code, 400);
    ecs_strbuf_reset(&reply.body);

    test_assert(ecs_lookup(world, "flecs.core.Foo") == 0);

    ecs_rest_server_fini(srv);
    ecs_fini(world);
}

void Rest_put_script_invalid_entity(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t rel = ecs_entity(world, { .name = "Rel" });
    ecs_add_id(world, rel, EcsRelationship);

    ecs_http_server_t *srv = ecs_rest_server_init(world, NULL);
    test_assert(srv != NULL);

    ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
    test_int(-1, ecs_http_server_request(srv, "PUT",
        "/script/Rel", "x {}", &reply));
    test_int(reply.code, 400);
    ecs_strbuf_reset(&reply.body);

    reply = (ecs_http_reply_t)ECS_HTTP_REPLY_INIT;
    test_int(-1, ecs_http_server_request(srv, "PUT",
        "/script/flecs/core/ChildOf", "x {}", &reply));
    test_int(reply.code, 400);
    ecs_strbuf_reset(&reply.body);

    reply = (ecs_http_reply_t)ECS_HTTP_REPLY_INIT;
    test_int(-1, ecs_http_server_request(srv, "PUT",
        "/script/*", "x {}", &reply));
    test_int(reply.code, 400);
    ecs_strbuf_reset(&reply.body);

    test_assert(ecs_lookup(world, "x") == 0);

    ecs_rest_server_fini(srv);
    ecs_fini(world);
}

void Rest_command_delete_session_builtin_entity(void) {
    ecs_world_t *world = ecs_init();

    ecs_http_server_t *srv = ecs_rest_server_init(world, NULL);
    test_assert(srv != NULL);

    ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
    test_int(0, ecs_http_server_request(srv, "PUT",
        "/shell/session", NULL, &reply));
    ecs_strbuf_reset(&reply.body);

    reply = (ecs_http_reply_t)ECS_HTTP_REPLY_INIT;
    test_int(0, ecs_http_server_request(srv, "PUT",
        "/component/flecs/core?component=flecs.script.shell.CommandSession",
        NULL, &reply));
    ecs_strbuf_reset(&reply.body);

    char path[128];
    ecs_os_snprintf(path, 128, "/shell/session/%u", (uint32_t)EcsFlecsCore);

    reply = (ecs_http_reply_t)ECS_HTTP_REPLY_INIT;
    test_int(-1, ecs_http_server_request(srv, "DELETE", path, NULL, &reply));
    test_assert(reply.code >= 400);
    ecs_strbuf_reset(&reply.body);

    test_assert(ecs_is_alive(world, EcsFlecsCore));

    ecs_rest_server_fini(srv);
    ecs_fini(world);
}

void Rest_command_delete_session_w_panic(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t bar = ecs_entity(world, { .name = "Bar" });
    ecs_add_pair(world, bar, EcsOnDelete, EcsPanic);
    ecs_entity_t foo = ecs_entity(world, { .name = "Foo" });
    ecs_add_id(world, foo, bar);

    ecs_http_server_t *srv = ecs_rest_server_init(world, NULL);
    test_assert(srv != NULL);

    ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
    test_int(0, ecs_http_server_request(srv, "PUT",
        "/shell/session", NULL, &reply));
    ecs_strbuf_reset(&reply.body);

    reply = (ecs_http_reply_t)ECS_HTTP_REPLY_INIT;
    test_int(0, ecs_http_server_request(srv, "PUT",
        "/component/Bar?component=flecs.script.shell.CommandSession",
        NULL, &reply));
    ecs_strbuf_reset(&reply.body);

    char path[128];
    ecs_os_snprintf(path, 128, "/shell/session/%u", (uint32_t)bar);

    reply = (ecs_http_reply_t)ECS_HTTP_REPLY_INIT;
    test_int(-1, ecs_http_server_request(srv, "DELETE", path, NULL, &reply));
    test_int(reply.code, 400);
    ecs_strbuf_reset(&reply.body);

    test_assert(ecs_is_alive(world, bar));
    test_assert(ecs_has_id(world, foo, bar));

    ecs_rest_server_fini(srv);
    ecs_fini(world);
}

void Rest_rename_module(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t script_module = ecs_lookup(world, "flecs.script");
    test_assert(script_module != 0);

    ecs_entity(world, { .name = "Bar" });

    ecs_http_server_t *srv = ecs_rest_server_init(world, NULL);
    test_assert(srv != NULL);

    ecs_log_set_level(-4);

    ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
    test_int(-1, ecs_http_server_request(srv, "PUT",
        "/component/flecs/script?component=(flecs.core.Identifier,flecs.core.Name)"
        "&value={\"value\":\"zz\"}", NULL, &reply));
    test_int(reply.code, 400);
    ecs_strbuf_reset(&reply.body);

    reply = (ecs_http_reply_t)ECS_HTTP_REPLY_INIT;
    test_int(-1, ecs_http_server_request(srv, "PUT",
        "/component/flecs/script?component=(flecs.core.ChildOf,Bar)",
        NULL, &reply));
    test_int(reply.code, 400);
    ecs_strbuf_reset(&reply.body);

    reply = (ecs_http_reply_t)ECS_HTTP_REPLY_INIT;
    test_int(-1, ecs_http_server_request(srv, "DELETE",
        "/component/flecs/script?component=(flecs.core.Identifier,flecs.core.Name)",
        NULL, &reply));
    test_int(reply.code, 400);
    ecs_strbuf_reset(&reply.body);

    reply = (ecs_http_reply_t)ECS_HTTP_REPLY_INIT;
    test_int(-1, ecs_http_server_request(srv, "DELETE",
        "/component/flecs/script?component=flecs.core.Module",
        NULL, &reply));
    test_int(reply.code, 400);
    ecs_strbuf_reset(&reply.body);

    test_assert(ecs_lookup(world, "flecs.script") == script_module);

    reply = (ecs_http_reply_t)ECS_HTTP_REPLY_INIT;
    test_int(0, ecs_http_server_request(srv, "PUT",
        "/shell/session", NULL, &reply));
    test_int(reply.code, 200);
    ecs_strbuf_reset(&reply.body);

    ecs_rest_server_fini(srv);
    ecs_fini(world);
}

void Rest_command_new_session_shell_module_name_taken(void) {
    ecs_world_t *world = ecs_init();

    ecs_http_server_t *srv = ecs_rest_server_init(world, NULL);
    test_assert(srv != NULL);

    ecs_log_set_level(-4);

    ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
    test_int(0, ecs_http_server_request(srv, "PUT",
        "/entity/flecs/script/shell", NULL, &reply));
    ecs_strbuf_reset(&reply.body);

    reply = (ecs_http_reply_t)ECS_HTTP_REPLY_INIT;
    test_int(-1, ecs_http_server_request(srv, "PUT",
        "/shell/session", NULL, &reply));
    test_assert(reply.code >= 400);
    ecs_strbuf_reset(&reply.body);

    reply = (ecs_http_reply_t)ECS_HTTP_REPLY_INIT;
    test_int(-1, ecs_http_server_request(srv, "PUT",
        "/shell/session/1", "pwd", &reply));
    test_assert(reply.code >= 400);
    ecs_strbuf_reset(&reply.body);

    ecs_rest_server_fini(srv);
    ecs_fini(world);
}

void Rest_add_isa_to_component_in_use(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ecs_entity_t foo = ecs_entity(world, { .name = "Foo" });
    ecs_set(world, foo, Position, {10, 20});
    ecs_entity(world, { .name = "Bar" });

    ecs_http_server_t *srv = ecs_rest_server_init(world, NULL);
    test_assert(srv != NULL);

    ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
    test_int(-1, ecs_http_server_request(srv, "PUT",
        "/component/Position?component=(flecs.core.IsA,Bar)", NULL, &reply));
    test_int(reply.code, 400);
    ecs_strbuf_reset(&reply.body);

    test_assert(!ecs_has_pair(world, ecs_id(Position), EcsIsA, EcsWildcard));

    ecs_rest_server_fini(srv);
    ecs_fini(world);
}

void Rest_add_isa_to_trait_component(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ecs_add_id(world, ecs_id(Position), EcsCanToggle);

    ecs_entity_t foo = ecs_entity(world, { .name = "Foo" });
    ecs_entity_t bar = ecs_entity(world, { .name = "Bar" });
    ecs_add_id(world, foo, bar);

    ecs_http_server_t *srv = ecs_rest_server_init(world, NULL);
    test_assert(srv != NULL);

    ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
    test_int(-1, ecs_http_server_request(srv, "PUT",
        "/component/Bar?component=(flecs.core.IsA,Position)", NULL, &reply));
    test_int(reply.code, 400);
    ecs_strbuf_reset(&reply.body);

    test_assert(!ecs_has_pair(world, bar, EcsIsA, EcsWildcard));

    ecs_rest_server_fini(srv);
    ecs_fini(world);
}

void Rest_delete_base_of_component_in_use(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t base = ecs_entity(world, { .name = "Base" });
    ecs_entity_t bar = ecs_entity(world, { .name = "Bar" });
    ecs_add_pair(world, bar, EcsIsA, base);
    ecs_entity_t foo = ecs_entity(world, { .name = "Foo" });
    ecs_add_id(world, foo, bar);

    ecs_http_server_t *srv = ecs_rest_server_init(world, NULL);
    test_assert(srv != NULL);

    ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
    test_int(-1, ecs_http_server_request(srv, "DELETE",
        "/entity/Base", NULL, &reply));
    test_int(reply.code, 400);
    ecs_strbuf_reset(&reply.body);

    test_assert(ecs_is_alive(world, base));
    test_assert(ecs_has_pair(world, bar, EcsIsA, base));

    reply = (ecs_http_reply_t)ECS_HTTP_REPLY_INIT;
    test_int(-1, ecs_http_server_request(srv, "DELETE",
        "/component/Bar?component=(flecs.core.IsA,Base)", NULL, &reply));
    test_int(reply.code, 400);
    ecs_strbuf_reset(&reply.body);

    test_assert(ecs_has_pair(world, bar, EcsIsA, base));

    ecs_rest_server_fini(srv);
    ecs_fini(world);
}

void Rest_add_flag_pair(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity(world, { .name = "Foo" });
    ecs_entity_t bar = ecs_entity(world, { .name = "Bar" });

    ecs_http_server_t *srv = ecs_rest_server_init(world, NULL);
    test_assert(srv != NULL);

    ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
    test_int(-1, ecs_http_server_request(srv, "PUT",
        "/component/Bar?component=(flecs.core.Flag,Foo)", NULL, &reply));
    test_int(reply.code, 400);
    ecs_strbuf_reset(&reply.body);

    ecs_entity_t flag = ecs_lookup(world, "flecs.core.Flag");
    test_assert(flag != 0);
    test_assert(!ecs_has_pair(world, bar, flag, EcsWildcard));

    ecs_rest_server_fini(srv);
    ecs_fini(world);
}

void Rest_set_empty_struct(void) {
    ecs_world_t *world = ecs_init();

    ecs_http_server_t *srv = ecs_rest_server_init(world, NULL);
    test_assert(srv != NULL);

    ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
    test_int(0, ecs_http_server_request(srv, "PUT",
        "/entity/S", NULL, &reply));
    ecs_strbuf_reset(&reply.body);

    reply = (ecs_http_reply_t)ECS_HTTP_REPLY_INIT;
    ecs_http_server_request(srv, "PUT",
        "/component/S?component=flecs.meta.struct&value={}", NULL, &reply);
    ecs_strbuf_reset(&reply.body);

    reply = (ecs_http_reply_t)ECS_HTTP_REPLY_INIT;
    test_int(0, ecs_http_server_request(srv, "GET",
        "/entity/S?values=true", NULL, &reply));
    test_int(reply.code, 200);
    ecs_strbuf_reset(&reply.body);

    ecs_rest_server_fini(srv);
    ecs_fini(world);
}

void Rest_query_info_w_value_pair(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity(world, { .name = "Rel" });

    ecs_http_server_t *srv = ecs_rest_server_init(world, NULL);
    test_assert(srv != NULL);

    ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
    test_int(0, ecs_http_server_request(srv, "GET",
        "/query?expr=(Rel,%20@99999)&query_info=true", NULL, &reply));
    test_int(reply.code, 200);
    ecs_strbuf_reset(&reply.body);

    ecs_rest_server_fini(srv);
    ecs_fini(world);
}

void Rest_query_and_from_empty_type(void) {
    ecs_world_t *world = ecs_init();

    ecs_http_server_t *srv = ecs_rest_server_init(world, NULL);
    test_assert(srv != NULL);

    ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
    test_int(0, ecs_http_server_request(srv, "GET",
        "/query?expr=flecs.core.Component,%20and%7Cflecs.meta.f32"
        "&values=true&fields=true&field_info=true", NULL, &reply));
    test_int(reply.code, 200);
    ecs_strbuf_reset(&reply.body);

    reply = (ecs_http_reply_t)ECS_HTTP_REPLY_INIT;
    test_int(0, ecs_http_server_request(srv, "GET",
        "/query?expr=flecs.core.Component,%20or%7Cflecs.meta.f32"
        "&values=true&fields=true&field_info=true", NULL, &reply));
    test_int(reply.code, 200);
    ecs_strbuf_reset(&reply.body);

    reply = (ecs_http_reply_t)ECS_HTTP_REPLY_INIT;
    test_int(0, ecs_http_server_request(srv, "GET",
        "/query?expr=%5Bout%5D%20flecs.core.Component,%20and%7Cflecs.meta.f32"
        "&values=true", NULL, &reply));
    test_int(reply.code, 200);
    ecs_strbuf_reset(&reply.body);

    ecs_rest_server_fini(srv);
    ecs_fini(world);
}

void Rest_query_or_component_and_tag(void) {
    ecs_world_t *world = ecs_init();

    ecs_http_server_t *srv = ecs_rest_server_init(world, NULL);
    test_assert(srv != NULL);

    ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
    test_int(0, ecs_http_server_request(srv, "GET",
        "/query?expr=flecs.core.Component%20%7C%7C%20flecs.core.Final"
        "&values=true&fields=true&field_info=true", NULL, &reply));
    test_int(reply.code, 200);
    ecs_strbuf_reset(&reply.body);

    reply = (ecs_http_reply_t)ECS_HTTP_REPLY_INIT;
    test_int(0, ecs_http_server_request(srv, "GET",
        "/query?expr=flecs.core.Final%20%7C%7C%20flecs.core.Component"
        "&values=true&fields=true&field_info=true", NULL, &reply));
    test_int(reply.code, 200);
    ecs_strbuf_reset(&reply.body);

    ecs_rest_server_fini(srv);
    ecs_fini(world);
}

void Rest_set_map_on_type_in_use(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t s = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "S" }),
        .members = {
            { "a", ecs_id(ecs_f32_t) }
        }
    });

    ecs_entity_t foo = ecs_entity(world, { .name = "Foo" });
    ecs_add_id(world, foo, s);

    ecs_http_server_t *srv = ecs_rest_server_init(world, NULL);
    test_assert(srv != NULL);

    ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
    test_int(-1, ecs_http_server_request(srv, "PUT",
        "/component/S?component=flecs.meta.map"
        "&value={\"key_type\":\"flecs.meta.i32\",\"type\":\"flecs.meta.i32\"}",
        NULL, &reply));
    test_int(reply.code, 400);
    ecs_strbuf_reset(&reply.body);

    test_assert(!ecs_has(world, s, EcsMap));

    ecs_rest_server_fini(srv);
    ecs_fini(world);
}

void Rest_set_vector_cyclic(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t v = ecs_vector(world, {
        .entity = ecs_entity(world, { .name = "V" }),
        .type = ecs_id(ecs_f32_t)
    });
    test_assert(v != 0);

    ecs_entity_t s = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "S" }),
        .members = {
            { "v", v }
        }
    });
    test_assert(s != 0);

    ecs_http_server_t *srv = ecs_rest_server_init(world, NULL);
    test_assert(srv != NULL);

    ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
    test_int(-1, ecs_http_server_request(srv, "PUT",
        "/component/V?component=flecs.meta.vector&value={\"type\":\"S\"}",
        NULL, &reply));
    test_int(reply.code, 400);
    ecs_strbuf_reset(&reply.body);

    reply = (ecs_http_reply_t)ECS_HTTP_REPLY_INIT;
    test_int(0, ecs_http_server_request(srv, "GET",
        "/type_info/S", NULL, &reply));
    test_int(reply.code, 200);
    ecs_strbuf_reset(&reply.body);

    reply = (ecs_http_reply_t)ECS_HTTP_REPLY_INIT;
    test_int(-1, ecs_http_server_request(srv, "PUT",
        "/component/V?component=flecs.meta.vector&value={\"type\":\"V\"}",
        NULL, &reply));
    test_int(reply.code, 400);
    ecs_strbuf_reset(&reply.body);

    ecs_rest_server_fini(srv);
    ecs_fini(world);
}

void Rest_change_type_nested_in_type_in_use(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t inner = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Inner" }),
        .members = {
            { "x", ecs_id(ecs_f32_t) }
        },
        .create_member_entities = true
    });

    ecs_entity_t vec = ecs_vector(world, {
        .entity = ecs_entity(world, { .name = "Vec" }),
        .type = ecs_id(ecs_f32_t)
    });

    ecs_entity_t outer = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Outer" }),
        .members = {
            { "i", inner },
            { "v", vec },
            { "s", ecs_id(ecs_string_t) }
        }
    });

    ecs_entity_t foo = ecs_entity(world, { .name = "Foo" });
    ecs_add_id(world, foo, outer);

    ecs_size_t inner_size = ecs_get(world, inner, EcsComponent)->size;

    ecs_http_server_t *srv = ecs_rest_server_init(world, NULL);
    test_assert(srv != NULL);

    ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
    test_int(0, ecs_http_server_request(srv, "PUT",
        "/entity/Inner/q", NULL, &reply));
    ecs_strbuf_reset(&reply.body);

    reply = (ecs_http_reply_t)ECS_HTTP_REPLY_INIT;
    test_int(-1, ecs_http_server_request(srv, "PUT",
        "/component/Inner/q?component=flecs.meta.member"
        "&value={\"type\":\"flecs.meta.string\",\"count\":8}", NULL, &reply));
    test_int(reply.code, 400);
    ecs_strbuf_reset(&reply.body);

    test_int(ecs_get(world, inner, EcsComponent)->size, inner_size);

    reply = (ecs_http_reply_t)ECS_HTTP_REPLY_INIT;
    test_int(-1, ecs_http_server_request(srv, "PUT",
        "/component/Vec?component=flecs.meta.vector"
        "&value={\"type\":\"flecs.meta.string\"}", NULL, &reply));
    test_int(reply.code, 400);
    ecs_strbuf_reset(&reply.body);

    test_uint(ecs_get(world, vec, EcsVector)->type, ecs_id(ecs_f32_t));

    reply = (ecs_http_reply_t)ECS_HTTP_REPLY_INIT;
    test_int(0, ecs_http_server_request(srv, "PUT",
        "/component/Foo?component=Outer&value={\"s\":\"hello\"}", NULL, &reply));
    ecs_strbuf_reset(&reply.body);

    reply = (ecs_http_reply_t)ECS_HTTP_REPLY_INIT;
    test_int(0, ecs_http_server_request(srv, "GET",
        "/entity/Foo?values=true", NULL, &reply));
    test_int(reply.code, 200);
    ecs_strbuf_reset(&reply.body);

    ecs_rest_server_fini(srv);
    ecs_fini(world);
}

void Rest_put_entity_hierarchy_too_deep(void) {
    ecs_world_t *world = ecs_init();

    ecs_http_server_t *srv = ecs_rest_server_init(world, NULL);
    test_assert(srv != NULL);

    ecs_strbuf_t buf = ECS_STRBUF_INIT;
    ecs_strbuf_appendlit(&buf, "/entity/a");
    int32_t i;
    for (i = 0; i < 140; i ++) {
        ecs_strbuf_appendlit(&buf, "/a");
    }
    char *path = ecs_strbuf_get(&buf);

    ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
    test_int(-1, ecs_http_server_request(srv, "PUT", path, NULL, &reply));
    test_int(reply.code, 400);
    ecs_strbuf_reset(&reply.body);
    ecs_os_free(path);

    reply = (ecs_http_reply_t)ECS_HTTP_REPLY_INIT;
    test_int(0, ecs_http_server_request(srv, "PUT", "/entity/a/a/a", NULL,
        &reply));
    test_int(reply.code, 200);
    ecs_strbuf_reset(&reply.body);

    ecs_rest_server_fini(srv);
    ecs_fini(world);
}

void Rest_reparent_hierarchy_too_deep(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t root_a = ecs_entity(world, { .name = "A" });
    ecs_entity_t cur = root_a;
    int32_t i;
    for (i = 0; i < 80; i ++) {
        cur = ecs_new_w_pair(world, EcsChildOf, cur);
    }

    ecs_entity_t root_b = ecs_entity(world, { .name = "B" });
    ecs_entity_t leaf_b = root_b;
    for (i = 0; i < 80; i ++) {
        leaf_b = ecs_new_w_pair(world, EcsChildOf, leaf_b);
    }

    ecs_http_server_t *srv = ecs_rest_server_init(world, NULL);
    test_assert(srv != NULL);

    char path[256];
    ecs_os_snprintf(path, 256,
        "/component/A?component=(flecs.core.ChildOf,%%23%u)", (uint32_t)leaf_b);

    ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
    test_int(-1, ecs_http_server_request(srv, "PUT", path, NULL, &reply));
    test_int(reply.code, 400);
    ecs_strbuf_reset(&reply.body);

    test_assert(!ecs_has_pair(world, root_a, EcsChildOf, EcsWildcard));

    ecs_rest_server_fini(srv);
    ecs_fini(world);
}
