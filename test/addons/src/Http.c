#include <addons.h>

static bool OnRequest(
    const ecs_http_request_t* request, 
    ecs_http_reply_t *reply,
    void *ctx)
{
    return true;
}

static bool OnRequestEcho(
    const ecs_http_request_t* request,
    ecs_http_reply_t *reply,
    void *ctx)
{
    ecs_strbuf_appendstr(&reply->body, request->path);
    const char *q = ecs_http_get_param(request, "q");
    if (q) {
        ecs_strbuf_appendch(&reply->body, '|');
        ecs_strbuf_appendstr(&reply->body, q);
    }
    return true;
}

void Http_decode_plus(void) {
    ecs_set_os_api_impl();

    ecs_http_server_t *srv = ecs_http_server_init(&(ecs_http_server_desc_t){
        .port = 27754,
        .callback = OnRequestEcho
    });

    test_assert(srv != NULL);

    ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
    test_int(0, ecs_http_server_request(srv, "GET",
        "/foo+bar?q=hello+world", NULL, &reply));
    test_int(reply.code, 200);

    char *reply_str = ecs_strbuf_get(&reply.body);
    test_assert(reply_str != NULL);
    test_str(reply_str, "foo bar|hello world");
    ecs_os_free(reply_str);

    ecs_http_server_fini(srv);
}

void Http_teardown(void) {
    ecs_set_os_api_impl();

    ecs_http_server_t *srv = ecs_http_server_init(&(ecs_http_server_desc_t){
        .port = 27750,
        .callback = OnRequest
    });

    test_assert(srv != NULL);
    
    ecs_http_server_fini(srv);
}

void Http_teardown_started(void) {
    ecs_set_os_api_impl();

    ecs_http_server_t *srv = ecs_http_server_init(&(ecs_http_server_desc_t){
        .port = 27751,
        .callback = OnRequest
    });

    test_assert(srv != NULL);

    test_int(ecs_http_server_start(srv), 0);
    
    ecs_http_server_fini(srv);
}

void Http_teardown_stopped(void) {
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

void Http_stop_start(void) {
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
