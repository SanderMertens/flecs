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

void Http_stop_start_stresstest() {
    ecs_set_os_api_impl();

    ecs_http_server_t *srv = ecs_http_server_init(&(ecs_http_server_desc_t){
        .port = 27754,
        .callback = OnRequest
    });

    test_assert(srv != NULL);

    test_int(ecs_http_server_start(srv), 0);

    for (int i = 0; i < 10000; i ++) {
        ecs_http_server_stop(srv);
        test_int(ecs_http_server_start(srv), 0);
    }
    
    ecs_http_server_fini(srv);
}

void Http_stop_start_stresstest_w_0_sleep() {
    ecs_set_os_api_impl();

    ecs_http_server_t *srv = ecs_http_server_init(&(ecs_http_server_desc_t){
        .port = 27755,
        .callback = OnRequest
    });

    test_assert(srv != NULL);

    test_int(ecs_http_server_start(srv), 0);

    for (int i = 0; i < 10000; i ++) {
        ecs_http_server_stop(srv);
        test_int(ecs_http_server_start(srv), 0);
        ecs_os_sleep(0, 0);
    }
    
    ecs_http_server_fini(srv);
}

void Http_stop_start_stresstest_w_100ns_sleep() {
    ecs_set_os_api_impl();

    ecs_http_server_t *srv = ecs_http_server_init(&(ecs_http_server_desc_t){
        .port = 27756,
        .callback = OnRequest
    });

    test_assert(srv != NULL);

    test_int(ecs_http_server_start(srv), 0);

    for (int i = 0; i < 1000; i ++) {
        ecs_http_server_stop(srv);
        test_int(ecs_http_server_start(srv), 0);
        ecs_os_sleep(0, 100);
    }
    
    ecs_http_server_fini(srv);
}

void Http_stop_start_stresstest_w_100us_sleep() {
    ecs_set_os_api_impl();

    ecs_http_server_t *srv = ecs_http_server_init(&(ecs_http_server_desc_t){
        .port = 27757,
        .callback = OnRequest
    });

    test_assert(srv != NULL);

    test_int(ecs_http_server_start(srv), 0);

    for (int i = 0; i < 100; i ++) {
        ecs_http_server_stop(srv);
        test_int(ecs_http_server_start(srv), 0);
        ecs_os_sleep(0, 100 * 1000);
    }
    
    ecs_http_server_fini(srv);
}

void Http_stop_start_stresstest_w_10ms_sleep() {
    ecs_set_os_api_impl();

    ecs_http_server_t *srv = ecs_http_server_init(&(ecs_http_server_desc_t){
        .port = 27758,
        .callback = OnRequest
    });

    test_assert(srv != NULL);

    test_int(ecs_http_server_start(srv), 0);

    for (int i = 0; i < 10; i ++) {
        ecs_http_server_stop(srv);
        test_int(ecs_http_server_start(srv), 0);
        ecs_os_sleep(0, 10 * 1000 * 1000);
    }
    
    ecs_http_server_fini(srv);
}
