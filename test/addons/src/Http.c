#include <addons.h>
#ifndef _WIN32
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
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

static bool OnRequestBody(
    const ecs_http_request_t* request,
    ecs_http_reply_t *reply,
    void *ctx)
{
    ecs_strbuf_appendstr(&reply->body, request->body);
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

#ifndef _WIN32
static void* Http_stop_thread(void *arg) {
    ecs_http_server_stop((ecs_http_server_t*)arg);
    return NULL;
}

void Http_stop_double_close(void) {
    ecs_set_os_api_impl();

    int probe = open("/dev/null", O_RDONLY);
    test_assert(probe >= 0);
    close(probe);
    int base = probe;

    ecs_http_server_t *srv = ecs_http_server_init(&(ecs_http_server_desc_t){
        .port = 27791,
        .callback = OnRequest
    });
    test_assert(srv != NULL);
    test_int(ecs_http_server_start(srv), 0);

    ecs_os_sleep(0, 200 * 1000 * 1000);
    probe = -1;
    {
        int fd;
        for (fd = base; fd < base + 16; fd ++) {
            struct stat st;
            if (!fstat(fd, &st) && S_ISSOCK(st.st_mode)) {
                probe = fd;
                break;
            }
        }
    }
    test_assert(probe != -1);

    ecs_os_thread_t stopper = ecs_os_thread_new(Http_stop_thread, srv);
    test_assert(stopper != 0);

    while (fcntl(probe, F_GETFD) != -1) { }

    int reused[8];
    int i;
    for (i = 0; i < 8; i ++) {
        reused[i] = open("/dev/null", O_RDONLY);
        test_assert(reused[i] >= 0);
    }

    ecs_os_thread_join(stopper);

    for (i = 0; i < 8; i ++) {
        test_assert(fcntl(reused[i], F_GETFD) != -1);
        close(reused[i]);
    }

    ecs_http_server_fini(srv);
}
#else
void Http_stop_double_close(void) { }
#endif

#ifndef _WIN32
void Http_request_headers_then_body(void) {
    ecs_set_os_api_impl();

    ecs_http_server_t *srv = ecs_http_server_init(&(ecs_http_server_desc_t){
        .port = 27792,
        .callback = OnRequestBody
    });
    test_assert(srv != NULL);
    test_int(ecs_http_server_start(srv), 0);
    ecs_os_sleep(0, 200 * 1000 * 1000);

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    test_assert(sock >= 0);
    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(27792);
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    test_int(connect(sock, (struct sockaddr*)&addr, sizeof(addr)), 0);

    const char *hdr = "PUT /body HTTP/1.1\r\nContent-Length: 8\r\n\r\n";
    test_int(send(sock, hdr, strlen(hdr), 0), (int)strlen(hdr));
    ecs_os_sleep(0, 50 * 1000 * 1000);
    test_int(send(sock, "abcdefgh", 8, 0), 8);

    char buf[512];
    int32_t len = 0;
    for (int i = 0; i < 300; i ++) {
        ecs_http_server_dequeue(srv, (ecs_ftime_t)0.01);
        ssize_t n = recv(sock, buf + len, sizeof(buf) - 1 - len, MSG_DONTWAIT);
        if (n == 0 || (n < 0 && errno != EAGAIN && errno != EWOULDBLOCK)) {
            break;
        }
        if (n > 0) {
            len += (int32_t)n;
        }
        ecs_os_sleep(0, 10 * 1000 * 1000);
    }
    buf[len] = '\0';
    close(sock);

    test_assert(strstr(buf, "HTTP/1.1 200 OK\r\n") == buf);
    test_assert(strstr(buf, "\r\n\r\nabcdefgh") != NULL);

    ecs_http_server_fini(srv);
}

void Http_request_body_in_chunks(void) {
    ecs_set_os_api_impl();

    ecs_http_server_t *srv = ecs_http_server_init(&(ecs_http_server_desc_t){
        .port = 27793,
        .callback = OnRequestBody
    });
    test_assert(srv != NULL);
    test_int(ecs_http_server_start(srv), 0);
    ecs_os_sleep(0, 200 * 1000 * 1000);

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    test_assert(sock >= 0);
    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(27793);
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    test_int(connect(sock, (struct sockaddr*)&addr, sizeof(addr)), 0);

    const char *hdr = "PUT /body HTTP/1.1\r\nContent-Length: 8\r\n\r\n";
    test_int(send(sock, hdr, strlen(hdr), 0), (int)strlen(hdr));
    const char *body = "abcdefgh";
    for (int i = 0; i < 8; i ++) {
        ecs_os_sleep(0, 30 * 1000 * 1000);
        test_int(send(sock, body + i, 1, 0), 1);
    }

    char buf[512];
    int32_t len = 0;
    for (int i = 0; i < 300; i ++) {
        ecs_http_server_dequeue(srv, (ecs_ftime_t)0.01);
        ssize_t n = recv(sock, buf + len, sizeof(buf) - 1 - len, MSG_DONTWAIT);
        if (n == 0 || (n < 0 && errno != EAGAIN && errno != EWOULDBLOCK)) {
            break;
        }
        if (n > 0) {
            len += (int32_t)n;
        }
        ecs_os_sleep(0, 10 * 1000 * 1000);
    }
    buf[len] = '\0';
    close(sock);

    test_assert(strstr(buf, "HTTP/1.1 200 OK\r\n") == buf);
    test_assert(strstr(buf, "\r\n\r\nabcdefgh") != NULL);

    ecs_http_server_fini(srv);
}
#else
void Http_request_headers_then_body(void) { }

void Http_request_body_in_chunks(void) { }
#endif
