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
    ecs_defer_begin(world);
    ecs_set(world, e1, Position, {10, 20});
    ecs_set(world, e2, Position, {20, 30});
    ecs_defer_end(world);
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
    ecs_defer_begin(world);
    ecs_set(world, e1, Position, {10, 20});
    ecs_defer_end(world);
    ecs_defer_begin(world);
    ecs_set(world, e2, Position, {20, 30});
    ecs_defer_end(world);
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
    ecs_defer_begin(world);
    ecs_defer_end(world);
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
    ecs_defer_begin(world);
    ecs_set(world, e1, Position, {10, 20});
    ecs_set(world, e2, Position, {20, 30});
    ecs_defer_end(world);
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

static
void Move(ecs_iter_t *it) { }

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
