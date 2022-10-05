#include <addons.h>

static bool OnRequest(
    const ecs_http_request_t* request, 
    ecs_http_reply_t *reply,
    void *ctx)
{
    return true;
}

void Http_teardown() {
    ecs_set_os_api_impl();

    ecs_http_server_t *srv = ecs_http_server_init(&(ecs_http_server_desc_t){
        .port = 27750,
        .callback = OnRequest
    });

    test_assert(srv != NULL);
    
    ecs_http_server_fini(srv);
}

void Http_teardown_started() {
    ecs_set_os_api_impl();

    ecs_http_server_t *srv = ecs_http_server_init(&(ecs_http_server_desc_t){
        .port = 27751,
        .callback = OnRequest
    });

    test_assert(srv != NULL);

    test_int(ecs_http_server_start(srv), 0);
    
    ecs_http_server_fini(srv);
}

void Http_teardown_stopped() {
    ecs_set_os_api_impl();

    ecs_http_server_t *srv = ecs_http_server_init(&(ecs_http_server_desc_t){
        .port = 27752,
        .callback = OnRequest
    });

    test_assert(srv != NULL);

    test_int(ecs_http_server_start(srv), 0);
    ecs_http_server_stop(srv);
    
    ecs_http_server_fini(srv);
}

void Http_stop_start() {
    ecs_set_os_api_impl();

    ecs_http_server_t *srv = ecs_http_server_init(&(ecs_http_server_desc_t){
        .port = 27753,
        .callback = OnRequest
    });

    test_assert(srv != NULL);

    test_int(ecs_http_server_start(srv), 0);
    ecs_http_server_stop(srv);
    test_int(ecs_http_server_start(srv), 0);
    
    ecs_http_server_fini(srv);
}
