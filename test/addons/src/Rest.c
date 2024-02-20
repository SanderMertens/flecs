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
        "{\"path\":\"flecs.core.World\", \"label\":\"World\", \"ids\":[]}");
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
            "/entity/flecs/core/World?label=true", &reply));
        test_int(reply.code, 200);
        
        char *reply_str = ecs_strbuf_get(&reply.body);
        test_assert(reply_str != NULL);
        test_str(reply_str,
            "{\"path\":\"flecs.core.World\", \"label\":\"World\", \"ids\":[]}");
        ecs_os_free(reply_str);
    }

    {
        ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
        test_int(0, ecs_http_server_request(srv, "GET",
            "/entity/flecs/core/World?label=true", &reply));
        test_int(reply.code, 200);
        
        char *reply_str = ecs_strbuf_get(&reply.body);
        test_assert(reply_str != NULL);
        test_str(reply_str,
            "{\"path\":\"flecs.core.World\", \"label\":\"World\", \"ids\":[]}");
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
            "/query?q=Foo", &reply));
        test_int(reply.code, 400); // No try, should error
        ecs_strbuf_reset(&reply.body);
    }

    {
        ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
        test_int(0, ecs_http_server_request(srv, "GET",
            "/query?q=Foo&try=true", &reply));
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

    ecs_entity_t e = ecs_new_entity(world, "e");
    ecs_set(world, e, Position, {10, 20});

    ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
    test_int(0, ecs_http_server_request(srv, "GET",
        "/query?q=Position", &reply));
    test_int(reply.code, 200);
    
    char *reply_str = ecs_strbuf_get(&reply.body);
    test_assert(reply_str != NULL);
    test_str(reply_str,
        "{\"results\":[{\"entities\":[\"e\"]}]}");
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
        .filter.entity = ecs_entity(world, { .name = "position_query" }),
        .filter.terms = {
            { .id = ecs_id(Position) }
        }
    });

    ecs_entity_t e = ecs_new_entity(world, "e");
    ecs_set(world, e, Position, {10, 20});

    ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
    test_int(0, ecs_http_server_request(srv, "GET",
        "/query?name=position_query", &reply));
    test_int(reply.code, 200);
    
    char *reply_str = ecs_strbuf_get(&reply.body);
    test_assert(reply_str != NULL);
    test_str(reply_str,
        "{\"results\":[{\"entities\":[\"e\"]}]}");
    ecs_os_free(reply_str);

    ecs_rest_server_fini(srv);

    ecs_fini(world);
}
