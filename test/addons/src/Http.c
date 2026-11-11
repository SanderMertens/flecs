#include <addons.h>

#ifdef ECS_TARGET_POSIX
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <unistd.h>
#endif

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

static int32_t http_param_count;

static bool OnRequestParamCount(
    const ecs_http_request_t* request,
    ecs_http_reply_t *reply,
    void *ctx)
{
    http_param_count = request->param_count;
    test_str(ecs_http_get_param(request, "p31"), "x");
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

void Http_query_params_max(void) {
    ecs_set_os_api_impl();

    ecs_http_server_t *srv = ecs_http_server_init(&(ecs_http_server_desc_t){
        .port = 27754,
        .callback = OnRequestParamCount
    });

    test_assert(srv != NULL);

    http_param_count = 0;
    ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
    test_int(0, ecs_http_server_request(srv, "GET",
        "/?p0=x&p1=x&p2=x&p3=x&p4=x&p5=x&p6=x&p7=x&p8=x&p9=x&"
        "p10=x&p11=x&p12=x&p13=x&p14=x&p15=x&p16=x&p17=x&p18=x&p19=x&"
        "p20=x&p21=x&p22=x&p23=x&p24=x&p25=x&p26=x&p27=x&p28=x&p29=x&"
        "p30=x&p31=x&p32=x", NULL, &reply));
    test_int(http_param_count, ECS_HTTP_QUERY_PARAM_COUNT_MAX);

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

static bool OnRequestHeadersBody(
    const ecs_http_request_t *request,
    ecs_http_reply_t *reply,
    void *ctx)
{
    int32_t *header_count = ctx;
    test_int(request->method, EcsHttpPost);
    test_str(request->path, "hello world");
    test_str(ecs_http_get_param(request, "a"), "one two");
    test_str(ecs_http_get_param(request, "b"), "three four");
    test_str(request->body, "abc\r\nxyz");
    test_int(request->header_count, *header_count);
    test_str(request->headers[0].key, "X-Header");
    test_str(request->headers[0].value, "value");
    ecs_strbuf_appendstr(&reply->body, request->body);
    return true;
}

static void Http_headers_body_request(int32_t count) {
    ecs_set_os_api_impl();
    int32_t expected_count = count + 1;
    if (expected_count > ECS_HTTP_HEADER_COUNT_MAX) {
        expected_count = ECS_HTTP_HEADER_COUNT_MAX;
    }
    ecs_http_server_t *srv = ecs_http_server_init(&(ecs_http_server_desc_t){
        .callback = OnRequestHeadersBody,
        .ctx = &expected_count
    });
    test_assert(srv != NULL);
    ecs_strbuf_t req = ECS_STRBUF_INIT;
    ecs_strbuf_appendstr(&req,
        "POST /hello+world?a=one%20two&b=three+four HTTP/1.1\r\n");
    for (int32_t i = 0; i < count; i ++) {
        ecs_strbuf_appendstr(&req, "X-Header: value\r\n");
    }
    ecs_strbuf_appendstr(&req, "Content-Length:8\r\n\r\nabc\r\nxyz");
    char *text = ecs_strbuf_get(&req);
    ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
    test_int(ecs_http_server_http_request(srv, text, 0, &reply), 0);
    test_int(reply.code, 200);
    char *body = ecs_strbuf_get(&reply.body);
    test_str(body, "abc\r\nxyz");
    ecs_os_free(body);
    ecs_os_free(text);
    ecs_http_server_fini(srv);
}

void Http_headers_and_body(void) {
    Http_headers_body_request(1);
}

void Http_headers_max_with_body(void) {
    Http_headers_body_request(ECS_HTTP_HEADER_COUNT_MAX + 1);
}

void Http_empty_target_encoded_path(void) {
    ecs_set_os_api_impl();

    ecs_http_server_t *srv = ecs_http_server_init(&(ecs_http_server_desc_t){
        .port = 27754,
        .callback = OnRequestEcho
    });

    test_assert(srv != NULL);

    ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
    test_int(-1, ecs_http_server_http_request(srv,
        "GET  %41%41%41\r\nHost: x\r\n\r\n", 0, &reply));
    test_int(reply.code, 400);

    ecs_http_server_fini(srv);
}

void Http_fragmented_request(void) {
#ifdef ECS_TARGET_POSIX
    ecs_set_os_api_impl();

    ecs_http_server_t *srv = ecs_http_server_init(&(ecs_http_server_desc_t){
        .port = 27755,
        .ipaddr = "127.0.0.1",
        .callback = OnRequestEcho
    });

    test_assert(srv != NULL);
    test_int(0, ecs_http_server_start(srv));

    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(27755);
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    int sock = -1;
    int connected = -1;
    for (int i = 0; i < 100 && connected; i ++) {
        sock = socket(AF_INET, SOCK_STREAM, 0);
        test_assert(sock >= 0);
        connected = connect(sock, (struct sockaddr*)&addr, sizeof(addr));
        if (connected) {
            close(sock);
            ecs_os_sleep(0, 10 * 1000 * 1000);
        }
    }
    test_int(connected, 0);

    const char *part_1 = "GET /frag HT";
    const char *part_2 = "TP/1.1\r\nHost: x\r\n\r\n";
    test_assert(send(sock, part_1, strlen(part_1), 0) > 0);
    ecs_os_sleep(0, 200 * 1000 * 1000);
    test_assert(send(sock, part_2, strlen(part_2), 0) > 0);

    fcntl(sock, F_SETFL, fcntl(sock, F_GETFL, 0) | O_NONBLOCK);

    char buf[1024];
    ssize_t received = 0;
    for (int i = 0; i < 200 && received <= 0; i ++) {
        ecs_http_server_dequeue(srv, 0.1f);
        ecs_os_sleep(0, 10 * 1000 * 1000);
        received = recv(sock, buf, sizeof(buf), 0);
    }

    close(sock);
    ecs_http_server_fini(srv);

    test_assert(received > 0);
#else
    test_assert(true);
#endif
}
