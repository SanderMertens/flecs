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
        "/entity/flecs/core/World?label=true", &reply));
    test_int(reply.code, 200);
    
    char *reply_str = ecs_strbuf_get(&reply.body);
    test_assert(reply_str != NULL);
    test_str(reply_str,
            "{\"parent\":\"flecs.core\", \"name\":\"World\", "
                "\"components\":{\"(flecs.core.Identifier,flecs.core.Symbol)\":null, \"(flecs.doc.Description,flecs.doc.Brief)\":{\"value\":\"Entity associated with world\"}}}");
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
            "/entity/flecs/core/World", &reply));
        test_int(reply.code, 200);
        
        char *reply_str = ecs_strbuf_get(&reply.body);
        test_assert(reply_str != NULL);
        test_str(reply_str,
            "{\"parent\":\"flecs.core\", \"name\":\"World\", "
                "\"components\":{\"(flecs.core.Identifier,flecs.core.Symbol)\":null, \"(flecs.doc.Description,flecs.doc.Brief)\":{\"value\":\"Entity associated with world\"}}}");
        ecs_os_free(reply_str);
    }

    {
        ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
        test_int(0, ecs_http_server_request(srv, "GET",
            "/entity/flecs/core/World", &reply));
        test_int(reply.code, 200);
        
        char *reply_str = ecs_strbuf_get(&reply.body);
        test_assert(reply_str != NULL);
        test_str(reply_str,
            "{\"parent\":\"flecs.core\", \"name\":\"World\", "
                "\"components\":{\"(flecs.core.Identifier,flecs.core.Symbol)\":null, \"(flecs.doc.Description,flecs.doc.Brief)\":{\"value\":\"Entity associated with world\"}}}");
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
            "/entity/flecs/core/Wor", &reply));
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
            "/entity/flecs/core/Wor", &reply));
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
            "/query?expr=Foo", &reply));
        test_int(reply.code, 400); // No try, should error
        ecs_strbuf_reset(&reply.body);
    }

    {
        ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
        test_int(0, ecs_http_server_request(srv, "GET",
            "/query?expr=Foo&try=true", &reply));
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
        "/query?expr=Position", &reply));
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
        "/query?name=position_query", &reply));
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
        "/tables", &reply));
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
            "/commands/capture", &reply));
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
            "/commands/frame/0", &reply));
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
            "/commands/capture", &reply));
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
            "/commands/frame/0", &reply));
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
            "/commands/frame/0", &reply));
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
            "/commands/capture", &reply));
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
            "/commands/frame/0", &reply));
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
            "/commands/capture", &reply));
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
            "/commands/frame/0", &reply));
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
            "/commands/capture", &reply));
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
            "/commands/frame/0", &reply));
        test_int(reply.code, 404);
        char *reply_str = ecs_strbuf_get(&reply.body);
        test_assert(reply_str != NULL);
        ecs_os_free(reply_str);
    }

    /* Garbage collected */
    {
        ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
        test_int(0, ecs_http_server_request(srv, "GET",
            "/commands/frame/3601", &reply));
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
        .entity = ecs_entity(world, { .name = "main.flecs" }),
        .code = ""
    });

    ecs_http_server_t *srv = ecs_rest_server_init(world, NULL);
    test_assert(srv != NULL);

    {
        ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
        ecs_log_set_level(-4);
        test_int(-1, ecs_http_server_request(srv, "PUT",
            "/script/main.flecs?code=struct%20Position%20%7B%0A%20%20x%20%3A%0A%7D",
            &reply));
        test_int(reply.code, 400);
        char *reply_str = ecs_strbuf_get(&reply.body);
        test_assert(reply_str != NULL);
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
        "/stats/pipeline?name=all&period=1m", &reply));
    test_int(reply.code, 200);
    
    char *reply_str = ecs_strbuf_get(&reply.body);
    test_assert(reply_str != NULL);
    ecs_os_free(reply_str);

    ecs_rest_server_fini(srv);

    ecs_fini(world);
}
