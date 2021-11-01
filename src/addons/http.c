/* This is a heavily modified version of the EmbeddableWebServer (see copyright
 * below). This version has been stripped from everything not strictly necessary
 * for receiving/replying to simple HTTP requests, and has been modified to use
 * the Flecs OS API. */

/* EmbeddableWebServer Copyright (c) 2016, 2019, 2020 Forrest Heller, and 
 * CONTRIBUTORS (see below) - All rights reserved.
 *
 * CONTRIBUTORS:
 * Martin Pulec - bug fixes, warning fixes, IPv6 support
 * Daniel Barry - bug fix (ifa_addr != NULL)
 * 
 * Released under the BSD 2-clause license:
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice, 
 * this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice, 
 * this list of conditions and the following disclaimer in the documentation 
 * and/or other materials provided with the distribution. THIS SOFTWARE IS 
 * PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS 
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES 
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN 
 * NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY 
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND 
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF 
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
 */

#include "../private_api.h"

#ifdef FLECS_HTTP

#ifdef _MSC_VER
#pragma comment(lib, "Ws2_32.lib")
#include <WinSock2.h>
#include <Ws2tcpip.h>
#include <Windows.h>
typedef SOCKET ecs_http_socket_t;
#else
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <strings.h>
#include <signal.h>
typedef int ecs_http_socket_t;
#endif

/* Max length of request method */
#define ECS_HTTP_METHOD_LEN_MAX (8) 

/* Dequeue count before connection purge */
#define ECS_HTTP_CONNECTION_PURGE_COUNT (10)

/* Max length of headers in reply */
#define ECS_HTTP_REPLY_HEADER_SIZE (1024)

/* Receive buffer size */
#define ECS_HTTP_SEND_RECV_BUFFER_SIZE (16 * 1024)

/* Max length of request (path + query + headers + body) */
#define ECS_HTTP_REQUEST_LEN_MAX (10 * 1024 * 1024)

/* HTTP server struct */
struct ecs_http_server_t {
    bool should_run;
    bool running;

    ecs_http_socket_t sock;
    ecs_os_mutex_t lock;
    ecs_os_thread_t thread;

    ecs_http_reply_action_t callback;
    void *ctx;

    ecs_sparse_t *connections; /* sparse<http_connection_t> */
    ecs_sparse_t *requests; /* sparse<http_request_t> */

    bool initialized;

    uint16_t port;
    const char *ipaddr;
};

/** Fragment state, used by HTTP request parser */
typedef enum  {
    HttpFragStateBegin,
    HttpFragStateMethod,
    HttpFragStatePath,
    HttpFragStateVersion,
    HttpFragStateHeaderStart,
    HttpFragStateHeaderName,
    HttpFragStateHeaderValueStart,
    HttpFragStateHeaderValue,
    HttpFragStateCR,
    HttpFragStateCRLF,
    HttpFragStateCRLFCR,
    HttpFragStateBody,
    HttpFragStateDone
} HttpFragState;

/** A fragment is a partially received HTTP request */
typedef struct {
    HttpFragState state;
    ecs_strbuf_t buf;
    ecs_http_method_t method;
    int32_t body_offset;
    int32_t query_offset;
    int32_t header_offsets[ECS_HTTP_HEADER_COUNT_MAX];
    int32_t header_value_offsets[ECS_HTTP_HEADER_COUNT_MAX];
    int32_t header_count;
    int32_t param_offsets[ECS_HTTP_QUERY_PARAM_COUNT_MAX];
    int32_t param_value_offsets[ECS_HTTP_QUERY_PARAM_COUNT_MAX];
    int32_t param_count;
    char header_buf[32];
    char *header_buf_ptr;
    int32_t content_length;
    bool parse_content_length;
    bool invalid;
} ecs_http_fragment_t;

/** Extend public connection type with fragment data */
typedef struct {
    ecs_http_connection_t pub;
    ecs_http_fragment_t frag;
    ecs_http_socket_t sock;
    int32_t dequeue_count; /* used to purge inactive connections */
} ecs_http_connection_impl_t;

typedef struct {
    ecs_http_request_t pub;
    void *res;
} ecs_http_request_impl_t;

static
ecs_size_t http_send(
    ecs_http_socket_t sock, 
    const void *buf, 
    ecs_size_t size, 
    int flags)
{
#ifndef _MSC_VER
    return flecs_to_i32(send(sock, buf, flecs_to_size_t(size), flags));
#else
    return flecs_to_i32(send(sock, buf, size, flags));
#endif
}

static
ecs_size_t http_recv(
    ecs_http_socket_t sock,
    void *buf,
    ecs_size_t size,
    int flags)
{
#ifndef _MSC_VER
    return flecs_to_i32(recv(sock, buf, flecs_to_size_t(size), flags));
#else
    return flecs_to_i32(recv(sock, buf, size, flags));
#endif
}

static
int http_getnameinfo(
    const struct sockaddr* addr,
    ecs_size_t addr_len,
    char *host,
    ecs_size_t host_len,
    char *port,
    ecs_size_t port_len,
    int flags)
{
    ecs_assert(addr_len > 0, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(host_len > 0, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(port_len > 0, ECS_INTERNAL_ERROR, NULL);
    return getnameinfo(addr, (uint32_t)addr_len, host, (uint32_t)host_len, 
        port, (uint32_t)port_len, flags);
}

static
int http_bind(
    ecs_http_socket_t sock,
    const struct sockaddr* addr,
    ecs_size_t addr_len)
{
    ecs_assert(addr_len > 0, ECS_INTERNAL_ERROR, NULL);
    return bind(sock, addr, (uint32_t)addr_len);
}

static
void http_close(
    ecs_http_socket_t sock)
{
#ifdef _MSC_VER
    closesocket(sock);
#else
    shutdown(sock, SHUT_RDWR);
    close(sock);
#endif
}

static
ecs_http_socket_t http_accept(
    ecs_http_socket_t sock,
    struct sockaddr* addr,
    ecs_size_t *addr_len)
{
    socklen_t len = (socklen_t)addr_len[0];
    ecs_http_socket_t result = accept(sock, addr, &len);
    addr_len[0] = (ecs_size_t)len;
    return result;
}

static 
void reply_free(ecs_http_reply_t* response) {
    ecs_os_free(response->body.content);
}

static
void request_free(ecs_http_request_impl_t *req) {
    ecs_os_free(req->res);
    flecs_sparse_remove(req->pub.conn->server->requests, req->pub.id);
}

static
void connection_free(ecs_http_connection_impl_t *conn) {
    if (conn->sock) {
        http_close(conn->sock);
    }
    flecs_sparse_remove(conn->pub.server->connections, conn->pub.id);
}

// https://stackoverflow.com/questions/10156409/convert-hex-string-char-to-int
static
char hex_2_int(char a, char b){
    a = (a <= '9') ? (char)(a - '0') : (char)((a & 0x7) + 9);
    b = (b <= '9') ? (char)(b - '0') : (char)((b & 0x7) + 9);
    return (char)((a << 4) + b);
}

static
void decode_url_str(
    char *str) 
{
    char ch, *ptr, *dst = str;
    for (ptr = str; (ch = *ptr); ptr++) {
        if (ch == '%') {
            dst[0] = hex_2_int(ptr[1], ptr[2]);
            dst ++;
            ptr += 2;
        } else {
            dst[0] = ptr[0];
            dst ++;
        }
    }
    dst[0] = '\0';
}

static
void parse_method(
    ecs_http_fragment_t *frag)
{
    char *method = ecs_strbuf_get_small(&frag->buf);
    if (!ecs_os_strcmp(method, "GET")) frag->method = EcsHttpGet;
    else if (!ecs_os_strcmp(method, "POST")) frag->method = EcsHttpPost;
    else if (!ecs_os_strcmp(method, "PUT")) frag->method = EcsHttpPut;
    else if (!ecs_os_strcmp(method, "DELETE")) frag->method = EcsHttpDelete;
    else {
        frag->method = EcsHttpMethodUnsupported;
        frag->invalid = true;
    }
    ecs_strbuf_reset(&frag->buf);
}

static
bool header_writable(
    ecs_http_fragment_t *frag)
{
    return frag->header_count < ECS_HTTP_HEADER_COUNT_MAX;
}

static
void header_buf_reset(
    ecs_http_fragment_t *frag)
{
    frag->header_buf[0] = '\0';
    frag->header_buf_ptr = frag->header_buf;
}

static
void header_buf_append(
    ecs_http_fragment_t *frag,
    char ch)
{
    if ((frag->header_buf_ptr - frag->header_buf) < 
        ECS_SIZEOF(frag->header_buf)) 
    {
        frag->header_buf_ptr[0] = ch;
        frag->header_buf_ptr ++;
    } else {
        frag->header_buf_ptr[0] = '\0';
    }
}

static
void enqueue_request(
    ecs_http_connection_impl_t *conn)
{
    ecs_http_server_t *srv = conn->pub.server;
    ecs_http_fragment_t *frag = &conn->frag;

    if (frag->invalid) { /* invalid request received, don't enqueue */
        ecs_strbuf_reset(&frag->buf);
    } else {
        char *res = ecs_strbuf_get(&frag->buf);
        if (res) {
            ecs_os_mutex_lock(srv->lock);
            ecs_http_request_impl_t *req = flecs_sparse_add(
                srv->requests, ecs_http_request_impl_t);
            req->pub.id = flecs_sparse_last_id(srv->requests);
            ecs_os_mutex_unlock(srv->lock);

            req->pub.conn = (ecs_http_connection_t*)conn;
            req->pub.method = frag->method;
            req->pub.path = res + 1;
            if (frag->body_offset) {
                req->pub.body = &res[frag->body_offset];
            }
            int32_t i, count = frag->header_count;
            for (i = 0; i < count; i ++) {
                req->pub.headers[i].key = &res[frag->header_offsets[i]];
                req->pub.headers[i].value = &res[frag->header_value_offsets[i]];
            }
            count = frag->param_count;
            for (i = 0; i < count; i ++) {
                req->pub.params[i].key = &res[frag->param_offsets[i]];
                req->pub.params[i].value = &res[frag->param_value_offsets[i]];
                decode_url_str((char*)req->pub.params[i].value);
            }

            req->pub.header_count = frag->header_count;
            req->pub.param_count = frag->param_count;
            req->res = res;
        }
    }
}

static
bool parse_request(
    ecs_http_connection_impl_t *conn,
    const char* req_frag, 
    ecs_size_t req_frag_len) 
{
    ecs_http_fragment_t *frag = &conn->frag;

    int32_t i;
    for (i = 0; i < req_frag_len; i++) {
        char c = req_frag[i];
        switch (frag->state) {
        case HttpFragStateBegin:
            ecs_os_memset_t(frag, 0, ecs_http_fragment_t);
            frag->buf.max = ECS_HTTP_METHOD_LEN_MAX;
            frag->state = HttpFragStateMethod;
            frag->header_buf_ptr = frag->header_buf;
            /* fallthrough */
        case HttpFragStateMethod:
            if (c == ' ') {
                parse_method(frag);
                frag->state = HttpFragStatePath;
                frag->buf.max = ECS_HTTP_REQUEST_LEN_MAX;
            } else {
                ecs_strbuf_appendch(&frag->buf, c);
            }
            break;
        case HttpFragStatePath:
            if (c == ' ') {
                frag->state = HttpFragStateVersion;
                ecs_strbuf_appendch(&frag->buf, '\0');
            } else {
                if (c == '?' || c == '=' || c == '&') {
                    ecs_strbuf_appendch(&frag->buf, '\0');
                    int32_t offset = ecs_strbuf_written(&frag->buf);
                    if (c == '?' || c == '&') {
                        frag->param_offsets[frag->param_count] = offset;
                    } else {
                        frag->param_value_offsets[frag->param_count] = offset;
                        frag->param_count ++;
                    }
                } else {
                    ecs_strbuf_appendch(&frag->buf, c);
                }
            }
            break;
        case HttpFragStateVersion:
            if (c == '\r') {
                frag->state = HttpFragStateCR;
            } /* version is not stored */
            break;
        case HttpFragStateHeaderStart:
            if (header_writable(frag)) {
                frag->header_offsets[frag->header_count] = 
                    ecs_strbuf_written(&frag->buf);
            }
            header_buf_reset(frag);
            frag->state = HttpFragStateHeaderName;
            /* fallthrough */
        case HttpFragStateHeaderName:
            if (c == ':') {
                frag->state = HttpFragStateHeaderValueStart;
                header_buf_append(frag, '\0');
                frag->parse_content_length = !ecs_os_strcmp(
                    frag->header_buf, "Content-Length");

                if (header_writable(frag)) {
                    ecs_strbuf_appendch(&frag->buf, '\0');
                    frag->header_value_offsets[frag->header_count] =
                        ecs_strbuf_written(&frag->buf);
                }
            } else if (c == '\r') {
                frag->state = HttpFragStateCR;
            } else  {
                header_buf_append(frag, c);
                if (header_writable(frag)) {
                    ecs_strbuf_appendch(&frag->buf, c);
                }
            }
            break;
        case HttpFragStateHeaderValueStart:
            header_buf_reset(frag);
            frag->state = HttpFragStateHeaderValue;
            if (c == ' ') { /* skip first space */
                break;
            }
            /* fallthrough */
        case HttpFragStateHeaderValue:
            if (c == '\r') {
                if (frag->parse_content_length) {
                    header_buf_append(frag, '\0');
                    int32_t len = atoi(frag->header_buf);
                    if (len < 0) {
                        frag->invalid = true;
                    } else {
                        frag->content_length = len;
                    }
                    frag->parse_content_length = false;
                }
                if (header_writable(frag)) {
                    int32_t cur = ecs_strbuf_written(&frag->buf);
                    if (frag->header_offsets[frag->header_count] < cur &&
                        frag->header_value_offsets[frag->header_count] < cur)
                    {
                        ecs_strbuf_appendch(&frag->buf, '\0');
                        frag->header_count ++;
                    }
                }
                frag->state = HttpFragStateCR;
            } else {
                if (frag->parse_content_length) {
                    header_buf_append(frag, c);
                }
                if (header_writable(frag)) {
                    ecs_strbuf_appendch(&frag->buf, c);
                }
            }
            break;
        case HttpFragStateCR:
            if (c == '\n') {
                frag->state = HttpFragStateCRLF;
            } else {
                frag->state = HttpFragStateHeaderStart;
            }
            break;
        case HttpFragStateCRLF:
            if (c == '\r') {
                frag->state = HttpFragStateCRLFCR;
            } else {
                frag->state = HttpFragStateHeaderStart;
                i--;
            }
            break;
        case HttpFragStateCRLFCR:
            if (c == '\n') {
                if (frag->content_length != 0) {
                    frag->body_offset = ecs_strbuf_written(&frag->buf);
                    frag->state = HttpFragStateBody;
                } else {
                    frag->state = HttpFragStateDone;
                }
            } else {
                frag->state = HttpFragStateHeaderStart;
            }
            break;
        case HttpFragStateBody: {
                ecs_strbuf_appendch(&frag->buf, c);
                if ((ecs_strbuf_written(&frag->buf) - frag->body_offset) == 
                    frag->content_length) 
                {
                    frag->state = HttpFragStateDone;
                }
            }
            break;
        case HttpFragStateDone:
            break;
        }
    }

    if (frag->state == HttpFragStateDone) {
        frag->state = HttpFragStateBegin;
        enqueue_request(conn);
        return true;
    } else {
        return false;
    }
}

static
void append_send_headers(
    ecs_strbuf_t *hdrs,
    int code, 
    const char* status, 
    const char* content_type,  
    ecs_strbuf_t *extra_headers,
    ecs_size_t content_len) 
{
    ecs_strbuf_appendstr(hdrs, "HTTP/1.1 ");
    ecs_strbuf_append(hdrs, "%d ", code);
    ecs_strbuf_appendstr(hdrs, status);
    ecs_strbuf_appendstr(hdrs, "\r\n");

    ecs_strbuf_appendstr(hdrs, "Content-Type: ");
    ecs_strbuf_appendstr(hdrs, content_type);
    ecs_strbuf_appendstr(hdrs, "\r\n");

    ecs_strbuf_appendstr(hdrs, "Content-Length: ");
    ecs_strbuf_append(hdrs, "%d", content_len);
    ecs_strbuf_appendstr(hdrs, "\r\n");

    ecs_strbuf_appendstr(hdrs, "Server: flecs\r\n");

    ecs_strbuf_mergebuff(hdrs, extra_headers);

    ecs_strbuf_appendstr(hdrs, "\r\n");
}

static
void send_reply(
    ecs_http_connection_impl_t* conn, 
    ecs_http_reply_t* reply) 
{
    char hdrs[ECS_HTTP_REPLY_HEADER_SIZE];
    ecs_strbuf_t hdr_buf = ECS_STRBUF_INIT;
    hdr_buf.buf = hdrs;
    hdr_buf.max = ECS_HTTP_REPLY_HEADER_SIZE;
    hdr_buf.buf = hdrs;

    char *content = ecs_strbuf_get(&reply->body);
    int32_t content_length = reply->body.length - 1;

    /* First, send the response HTTP headers */
    append_send_headers(&hdr_buf, reply->code, reply->status, 
        reply->content_type, &reply->headers, content_length);

    ecs_size_t hdrs_len = ecs_strbuf_written(&hdr_buf);
    hdrs[hdrs_len] = '\0';
    ecs_size_t written = http_send(conn->sock, hdrs, hdrs_len, 0);

    if (written != hdrs_len) {
        ecs_err("failed to write HTTP response headers to '%s:%d': %s",
            conn->pub.host, conn->pub.port, ecs_os_strerror(errno));
        return;
    }

    /* Second, send response body */
    if (content_length > 0) {
        written = http_send(conn->sock, content, content_length, 0);
        if (written != content_length) {
            ecs_err("failed to write HTTP response body to '%s:%d': %s",
                conn->pub.host, conn->pub.port, ecs_os_strerror(errno));
        }
    }
}

static
void recv_request(
    ecs_http_connection_impl_t *conn)
{
    ecs_size_t bytes_read;
    char recv_buf[ECS_HTTP_SEND_RECV_BUFFER_SIZE];

    while ((bytes_read = http_recv(
        conn->sock, recv_buf, ECS_SIZEOF(recv_buf), 0)) > 0) 
    {
        if (parse_request(conn, recv_buf, bytes_read)) {
            return;
        }
    }
}

static
void init_connection(
    ecs_http_server_t *srv, 
    ecs_http_socket_t sock_conn,
    struct sockaddr_storage *remote_addr, 
    ecs_size_t remote_addr_len) 
{
    /* Create new connection */
    ecs_os_mutex_lock(srv->lock);
    ecs_http_connection_impl_t *conn = flecs_sparse_add(
        srv->connections, ecs_http_connection_impl_t);
    conn->pub.id = flecs_sparse_last_id(srv->connections);
    ecs_os_mutex_unlock(srv->lock);

    char *remote_host = conn->pub.host;
    char *remote_port = conn->pub.port;

    /* Fetch name & port info */
    if (http_getnameinfo((struct sockaddr*) remote_addr, remote_addr_len,
        remote_host, ECS_SIZEOF(conn->pub.host),
        remote_port, ECS_SIZEOF(conn->pub.port),
            NI_NUMERICHOST | NI_NUMERICSERV))
    {
        ecs_os_strcpy(remote_host, "unknown");
        ecs_os_strcpy(remote_port, "unknown");
    }

    conn->pub.server = srv;
    conn->sock = sock_conn;
    recv_request(conn);

    ecs_dbg("http: connection established from '%s:%s'", 
        remote_host, remote_port);
}

static 
int accept_connections(
    ecs_http_server_t* srv, 
    const struct sockaddr* addr, 
    ecs_size_t addr_len) 
{
#ifdef _MSC_VER
    /* If on Windows, test if winsock needs to be initialized */
    SOCKET testsocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (SOCKET_ERROR == testsocket && WSANOTINITIALISED == WSAGetLastError()) {
        WSADATA data = { 0 };
        int result = WSAStartup(MAKEWORD(2, 2), &data);
        if (result) {
            ecs_warn("WSAStartup failed with GetLastError = %d\n", 
                GetLastError());
            return -1;
        }
    } else {
        http_close(testsocket);
    }
#endif

    /* Resolve name + port (used for logging) */
    char addr_host[256];
    char addr_port[20];

    if (http_getnameinfo(
        addr, addr_len, addr_host, ECS_SIZEOF(addr_host), addr_port, 
        ECS_SIZEOF(addr_port), NI_NUMERICHOST | NI_NUMERICSERV))
    {
        ecs_os_strcpy(addr_host, "unknown");
        ecs_os_strcpy(addr_port, "unknown");
    }

    srv->sock = socket(addr->sa_family, SOCK_STREAM, IPPROTO_TCP);
    if (srv->sock <= 0) {
        ecs_err("unable to create new connection socket: %s", 
            ecs_os_strerror(errno));
        return -1;
    }

    int reuse = 1;
    int result = setsockopt(srv->sock, SOL_SOCKET, SO_REUSEADDR, 
        (char*)&reuse, ECS_SIZEOF(reuse)); 
    if (result) {
        ecs_warn("failed to setsockopt: %s", ecs_os_strerror(errno));
    }

    if (addr->sa_family == AF_INET6) {
        int ipv6only = 0;
        if (setsockopt(srv->sock, IPPROTO_IPV6, IPV6_V6ONLY, 
            (char*)&ipv6only, ECS_SIZEOF(ipv6only)))
        {
            ecs_warn("failed to setsockopt: %s", ecs_os_strerror(errno));
        }
    }
    
    result = http_bind(srv->sock, addr, addr_len);
    if (result) {
        ecs_err("http: failed to bind to '%s:%s': %s", 
            addr_host, addr_port, ecs_os_strerror(errno));
        return -1;
    }

    result = listen(srv->sock, SOMAXCONN);
    if (result) {
        ecs_warn("http: could not listen for SOMAXCONN (%d) connections: %s", 
            SOMAXCONN, ecs_os_strerror(errno));
    }

    ecs_trace("http: listening for incoming connections on '%s:%s'",
        addr_host, addr_port);

    ecs_http_socket_t sock_conn;
    struct sockaddr_storage remote_addr;
    ecs_size_t remote_addr_len;

    while (srv->should_run) {
        remote_addr_len = ECS_SIZEOF(remote_addr);
        sock_conn = http_accept(srv->sock, (struct sockaddr*) &remote_addr, 
            &remote_addr_len);

        if (sock_conn == -1) {
            ecs_dbg("http: connection attempt failed: %s", 
                ecs_os_strerror(errno));
            continue;
        }

        init_connection(srv, sock_conn, &remote_addr, remote_addr_len);
    }

    if (srv->sock && errno != EBADF) {
        http_close(srv->sock);
    }

    return 0;
}

static
void* http_server_thread(void* arg) {
    ecs_http_server_t *srv = arg;
    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_port = htons(srv->port) };

    if (!srv->ipaddr) {
        addr.sin_addr.s_addr = htonl(INADDR_ANY);
    } else {
        inet_pton(AF_INET, srv->ipaddr, &(addr.sin_addr));
    }

    accept_connections(srv, (struct sockaddr*)&addr, ECS_SIZEOF(addr));
    return NULL;
}

static
void handle_request(
    ecs_http_server_t *srv,
    ecs_http_request_impl_t *req)
{
    ecs_http_reply_t reply = ECS_HTTP_REPLY_INIT;
    ecs_http_connection_impl_t *conn = 
        (ecs_http_connection_impl_t*)req->pub.conn;

    if (srv->callback((ecs_http_request_t*)req, &reply, srv->ctx) == 0) {
        reply.code = 404;
        reply.status = "Resource not found";
    }

    send_reply(conn, &reply);
    ecs_dbg("http: reply sent to '%s:%s'", conn->pub.host, conn->pub.port);

    reply_free(&reply);
    request_free(req);
    connection_free(conn);
}

static
void dequeue_requests(
    ecs_http_server_t *srv)
{
    ecs_os_mutex_lock(srv->lock);

    int32_t i, count = flecs_sparse_count(srv->requests);
    for (i = count - 1; i >= 0; i --) {
        ecs_http_request_impl_t *req = flecs_sparse_get_dense(
            srv->requests, ecs_http_request_impl_t, i);
        handle_request(srv, req);
    }

    count = flecs_sparse_count(srv->connections);
    for (i = count - 1; i >= 0; i --) {
        ecs_http_connection_impl_t *conn = flecs_sparse_get_dense(
            srv->connections, ecs_http_connection_impl_t, i);
        conn->dequeue_count ++;
        if (conn->dequeue_count > ECS_HTTP_CONNECTION_PURGE_COUNT) {
            connection_free(conn);
        }
    }

    ecs_os_mutex_unlock(srv->lock);
}

const char* ecs_http_get_header(
    const ecs_http_request_t* req,
    const char* name) 
{
    for (ecs_size_t i = 0; i < req->header_count; i++) {
        if (!ecs_os_strcmp(req->headers[i].key, name)) {
            return req->headers[i].value;
        }
    }
    return NULL;
}

const char* ecs_http_get_param(
    const ecs_http_request_t* req,
    const char* name) 
{
    for (ecs_size_t i = 0; i < req->param_count; i++) {
        if (!ecs_os_strcmp(req->params[i].key, name)) {
            return req->params[i].value;
        }
    }
    return NULL;
}

ecs_http_server_t* ecs_http_server_init(
    const ecs_http_server_desc_t *desc) 
{
    ecs_http_server_t* srv = ecs_os_calloc_t(ecs_http_server_t);
    srv->lock = ecs_os_mutex_new();

    srv->should_run = false;
    srv->initialized = true;

    srv->callback = desc->callback;
    srv->ctx = desc->ctx;
    srv->port = desc->port;
    srv->ipaddr = desc->ipaddr;

    srv->connections = flecs_sparse_new(ecs_http_connection_impl_t);
    srv->requests = flecs_sparse_new(ecs_http_request_impl_t);

#ifndef _MSC_VER
    /* Ignore pipe signal. SIGPIPE can occur when a message is sent to a client
     * but te client already disconnected. */
    signal(SIGPIPE, SIG_IGN);
#endif

    return srv;
}

void ecs_http_server_fini(
    ecs_http_server_t* srv) 
{
    ecs_http_server_stop(srv);
    ecs_os_mutex_free(srv->lock);
    flecs_sparse_free(srv->connections);
    flecs_sparse_free(srv->requests);
    ecs_os_free(srv);
}

int ecs_http_server_start(
    ecs_http_server_t *srv)
{
    ecs_assert(srv != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(srv->initialized, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(!srv->should_run, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(!srv->thread, ECS_INVALID_PARAMETER, NULL);

    srv->should_run = true;

    srv->thread = ecs_os_thread_new(http_server_thread, srv);
    if (!srv->thread) {
        return -1;
    }

    return 0;
}

void ecs_http_server_stop(
    ecs_http_server_t* srv) 
{
    ecs_assert(srv != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(srv->initialized, ECS_INVALID_OPERATION, NULL);
    ecs_assert(srv->should_run, ECS_INVALID_PARAMETER, NULL);

    /* Stop server thread */
    ecs_trace("http: shutting down server thread");
    srv->should_run = false;
    if (srv->sock >= 0) {
        http_close(srv->sock);
    }

    ecs_os_thread_join(srv->thread);

    /* Close all connections */
    int i, count = flecs_sparse_count(srv->connections);
    for (i = count - 1; i >= 0; i --) {
        connection_free(flecs_sparse_get_dense(
            srv->connections, ecs_http_connection_impl_t, i));
    }

    /* Cleanup all outstanding requests */
    count = flecs_sparse_count(srv->requests);
    for (i = count - 1; i >= 0; i --) {
        request_free(flecs_sparse_get_dense(
            srv->requests, ecs_http_request_impl_t, i));
    }

    ecs_assert(flecs_sparse_count(srv->connections) == 0, 
        ECS_INTERNAL_ERROR, NULL);
    ecs_assert(flecs_sparse_count(srv->requests) == 0, 
        ECS_INTERNAL_ERROR, NULL);

    srv->thread = 0;
}

void ecs_http_server_dequeue(
    ecs_http_server_t* srv)
{
    ecs_assert(srv != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(srv->initialized, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(srv->should_run, ECS_INVALID_PARAMETER, NULL);
    dequeue_requests(srv);
}

#endif
