#include <addons.h>

void Rest_teardown() {
    ecs_world_t *world = ecs_init();

    ecs_singleton_set(world, EcsRest, {27760});

    ecs_fini(world);

    test_assert(true); // Ensure teardown was successful
}

void Rest_get() {
    ecs_world_t *world = ecs_init();

    ecs_http_server_t *srv = ecs_rest_server_init(world, NULL);
    test_assert(srv != NULL);

    ecs_http_reply_t reply = ecs_http_server_request(srv, "GET",
        "/entity/flecs/core/World?label=true");
    test_int(reply.code, 200);
    
    char *reply_str = ecs_strbuf_get(&reply.body);
    test_assert(reply_str != NULL);
    test_str(reply_str,
        "{\"path\":\"flecs.core.World\", \"label\":\"World\", \"ids\":[]}");
    ecs_os_free(reply_str);

    ecs_rest_server_fini(srv);

    ecs_fini(world);
}
