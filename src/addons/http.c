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

#if defined(ECS_TARGET_WINDOWS)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
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

/* Timeout (s) before connection purge */
#define ECS_HTTP_CONNECTION_PURGE_TIMEOUT (1.0)

/* Number of dequeues before purging */
#define ECS_HTTP_CONNECTION_PURGE_RETRY_COUNT (5)

/* Minimum interval between dequeueing requests (ms) */
#define ECS_HTTP_MIN_DEQUEUE_INTERVAL (100)

/* Minimum interval between printing statistics (ms) */
#define ECS_HTTP_MIN_STATS_INTERVAL (10 * 1000)

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

    ecs_ftime_t dequeue_timeout; /* used to not lock request queue too often */
    ecs_ftime_t stats_timeout; /* used for periodic reporting of statistics */

    ecs_ftime_t request_time; /* time spent on requests in last stats interval */
    ecs_ftime_t request_time_total; /* total time spent on requests */
    int32_t requests_processed; /* requests processed in last stats interval */
    int32_t requests_processed_total; /* total requests processed */
    int32_t dequeue_count; /* number of dequeues in last stats interval */
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

    /* Connection is purged after both timeout expires and connection has
     * exceeded retry count. This ensures that a connection does not immediately
     * timeout when a frame takes longer than usual */
    ecs_ftime_t dequeue_timeout;
    int32_t dequeue_retries;    
} ecs_http_connection_impl_t;

typedef struct {
    ecs_http_request_t pub;
    uint64_t conn_id; /* for sanity check */
    void *res;
} ecs_http_request_impl_t;

static
ecs_size_t http_send(
    ecs_http_socket_t sock, 
    const void *buf, 
    ecs_size_t size, 
    int flags)
{
#ifndef ECS_TARGET_MSVC
    ssize_t send_bytes = send(sock, buf, flecs_itosize(size), flags);
    return flecs_itoi32(send_bytes);
#else
    int send_bytes = send(sock, buf, size, flags);
    return flecs_itoi32(send_bytes);
#endif
}

static
ecs_size_t http_recv(
    ecs_http_socket_t sock,
    void *buf,
    ecs_size_t size,
    int flags)
{
    ecs_size_t ret;
#ifndef ECS_TARGET_MSVC
    ssize_t recv_bytes = recv(sock, buf, flecs_itosize(size), flags);
    ret = flecs_itoi32(recv_bytes);
#else
    int recv_bytes = recv(sock, buf, size, flags);
    ret = flecs_itoi32(recv_bytes);
#endif
    if (ret == -1) {
        ecs_dbg("recv failed: %s (sock = %d)", ecs_os_strerror(errno), sock);
    } else if (ret == 0) {
        ecs_dbg("recv: received 0 bytes (sock = %d)", sock);
    }

    return ret;
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
#if defined(ECS_TARGET_WINDOWS)
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
    ecs_assert(response != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_os_free(response->body.content);
}

static
void request_free(ecs_http_request_impl_t *req) {
    ecs_assert(req != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(req->pub.conn != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(req->pub.conn->server != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(req->pub.conn->server->requests != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(req->pub.conn->id == req->conn_id, ECS_INTERNAL_ERROR, NULL);
    ecs_os_free(req->res);
    flecs_sparse_remove(req->pub.conn->server->requests, req->pub.id);
}

static
void connection_free(ecs_http_connection_impl_t *conn) {
    ecs_assert(conn != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(conn->pub.id != 0, ECS_INTERNAL_ERROR, NULL);
    uint64_t conn_id = conn->pub.id;

    if (conn->sock) {
        http_close(conn->sock);
    }

    flecs_sparse_remove(conn->pub.server->connections, conn_id);
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
    else if (!ecs_os_strcmp(method, "OPTIONS")) frag->method = EcsHttpOptions;
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
            req->conn_id = conn->pub.id;
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
    uint64_t conn_id,
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
        if (conn->pub.id == conn_id) {
            enqueue_request(conn);
        }
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
        ecs_err("failed to write HTTP response headers to '%s:%s': %s",
            conn->pub.host, conn->pub.port, ecs_os_strerror(errno));
        return;
    }

    /* Second, send response body */
    if (content_length > 0) {
        written = http_send(conn->sock, content, content_length, 0);
        if (written != content_length) {
            ecs_err("failed to write HTTP response body to '%s:%s': %s",
                conn->pub.host, conn->pub.port, ecs_os_strerror(errno));
        }
    }
}

static
void recv_request(
    ecs_http_server_t *srv,
    ecs_http_connection_impl_t *conn, 
    uint64_t conn_id,
    ecs_http_socket_t sock)
{
    ecs_size_t bytes_read;
    char recv_buf[ECS_HTTP_SEND_RECV_BUFFER_SIZE];

    while ((bytes_read = http_recv(
        sock, recv_buf, ECS_SIZEOF(recv_buf), 0)) > 0) 
    {
        ecs_os_mutex_lock(srv->lock);
        bool is_alive = conn->pub.id == conn_id;
        if (is_alive) {
            conn->dequeue_timeout = 0;
            conn->dequeue_retries = 0;
        }
        ecs_os_mutex_unlock(srv->lock);

        if (is_alive) {
            if (parse_request(conn, conn_id, recv_buf, bytes_read)) {
                return;
            }
        } else {
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
    uint64_t conn_id = conn->pub.id = flecs_sparse_last_id(srv->connections);
    conn->pub.server = srv;
    conn->sock = sock_conn;
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

    ecs_dbg_2("http: connection established from '%s:%s'", 
        remote_host, remote_port);

    recv_request(srv, conn, conn_id, sock_conn);

    ecs_dbg_2("http: request received from '%s:%s'", 
        remote_host, remote_port);
}

static
void accept_connections(
    ecs_http_server_t* srv, 
    const struct sockaddr* addr, 
    ecs_size_t addr_len) 
{
#ifdef ECS_TARGET_WINDOWS
    /* If on Windows, test if winsock needs to be initialized */
    SOCKET testsocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (SOCKET_ERROR == testsocket && WSANOTINITIALISED == WSAGetLastError()) {
        WSADATA data = { 0 };
        int result = WSAStartup(MAKEWORD(2, 2), &data);
        if (result) {
            ecs_warn("WSAStartup failed with GetLastError = %d\n", 
                GetLastError());
            return;
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

    ecs_os_mutex_lock(srv->lock);
    if (srv->should_run) {
        ecs_dbg_2("http: initializing connection socket");
        
        srv->sock = socket(addr->sa_family, SOCK_STREAM, IPPROTO_TCP);
        if (srv->sock < 0) {
            ecs_err("unable to create new connection socket: %s", 
                ecs_os_strerror(errno));
            ecs_os_mutex_unlock(srv->lock);
            goto done;
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
            ecs_os_mutex_unlock(srv->lock);
            goto done;
        }

        result = listen(srv->sock, SOMAXCONN);
        if (result) {
            ecs_warn("http: could not listen for SOMAXCONN (%d) connections: %s", 
                SOMAXCONN, ecs_os_strerror(errno));
        }

        ecs_trace("http: listening for incoming connections on '%s:%s'",
            addr_host, addr_port);
    }
    ecs_os_mutex_unlock(srv->lock);

    ecs_http_socket_t sock_conn;
    struct sockaddr_storage remote_addr;
    ecs_size_t remote_addr_len;

    while (srv->should_run) {
        remote_addr_len = ECS_SIZEOF(remote_addr);
        sock_conn = http_accept(srv->sock, (struct sockaddr*) &remote_addr, 
            &remote_addr_len);

        if (sock_conn == -1) {
            if (srv->should_run) {
                ecs_dbg("http: connection attempt failed: %s", 
                    ecs_os_strerror(errno));
            }
            continue;
        }

        init_connection(srv, sock_conn, &remote_addr, remote_addr_len);
    }

done:
    if (srv->sock && errno != EBADF) {
        http_close(srv->sock);
        srv->sock = 0;
    }

    ecs_trace("http: no longer accepting connections on '%s:%s'",
        addr_host, addr_port);
}

static
void* http_server_thread(void* arg) {
    ecs_http_server_t *srv = arg;
    struct sockaddr_in addr;
    ecs_os_zeromem(&addr);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(srv->port);

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
    ecs_dbg_2("http: reply sent to '%s:%s'", conn->pub.host, conn->pub.port);

    reply_free(&reply);
    request_free(req);
    connection_free(conn);
}

static
int32_t dequeue_requests(
    ecs_http_server_t *srv,
    float delta_time)
{
    ecs_os_mutex_lock(srv->lock);

    int32_t i, request_count = flecs_sparse_count(srv->requests);
    for (i = request_count - 1; i >= 1; i --) {
        ecs_http_request_impl_t *req = flecs_sparse_get_dense(
            srv->requests, ecs_http_request_impl_t, i);
        handle_request(srv, req);
    }

    int32_t connections_count = flecs_sparse_count(srv->connections);
    for (i = connections_count - 1; i >= 1; i --) {
        ecs_http_connection_impl_t *conn = flecs_sparse_get_dense(
            srv->connections, ecs_http_connection_impl_t, i);

        conn->dequeue_timeout += delta_time;
        conn->dequeue_retries ++;
        
        if ((conn->dequeue_timeout > 
            (ecs_ftime_t)ECS_HTTP_CONNECTION_PURGE_TIMEOUT) &&
             (conn->dequeue_retries > ECS_HTTP_CONNECTION_PURGE_RETRY_COUNT)) 
        {
            ecs_dbg("http: purging connection '%s:%s' (sock = %d)", 
                conn->pub.host, conn->pub.port, conn->sock);
            connection_free(conn);
        }
    }

    ecs_os_mutex_unlock(srv->lock);

    return request_count;
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
    ecs_check(ecs_os_has_threading(), ECS_UNSUPPORTED, 
        "missing OS API implementation");

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

    /* Start at id 1 */
    flecs_sparse_new_id(srv->connections);
    flecs_sparse_new_id(srv->requests);

#ifndef ECS_TARGET_WINDOWS
    /* Ignore pipe signal. SIGPIPE can occur when a message is sent to a client
     * but te client already disconnected. */
    signal(SIGPIPE, SIG_IGN);
#endif

    return srv;
error:
    return NULL;
}

void ecs_http_server_fini(
    ecs_http_server_t* srv) 
{
    if (srv->should_run) {
        ecs_http_server_stop(srv);
    }
    ecs_os_mutex_free(srv->lock);
    flecs_sparse_free(srv->connections);
    flecs_sparse_free(srv->requests);
    ecs_os_free(srv);
}

int ecs_http_server_start(
    ecs_http_server_t *srv)
{
    ecs_check(srv != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(srv->initialized, ECS_INVALID_PARAMETER, NULL);
    ecs_check(!srv->should_run, ECS_INVALID_PARAMETER, NULL);
    ecs_check(!srv->thread, ECS_INVALID_PARAMETER, NULL);

    srv->should_run = true;

    ecs_dbg("http: starting server thread");

    srv->thread = ecs_os_thread_new(http_server_thread, srv);
    if (!srv->thread) {
        goto error;
    }

    return 0;
error:
    return -1;
}

void ecs_http_server_stop(
    ecs_http_server_t* srv) 
{
    ecs_check(srv != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(srv->initialized, ECS_INVALID_OPERATION, NULL);
    ecs_check(srv->should_run, ECS_INVALID_PARAMETER, NULL);

    /* Stop server thread */
    ecs_dbg("http: shutting down server thread");

    ecs_os_mutex_lock(srv->lock);
    srv->should_run = false;
    if (srv->sock >= 0) {
        http_close(srv->sock);
    }
    ecs_os_mutex_unlock(srv->lock);

    ecs_os_thread_join(srv->thread);

    ecs_trace("http: server thread shut down");

    /* Cleanup all outstanding requests */
    int i, count = flecs_sparse_count(srv->requests);
    for (i = count - 1; i >= 1; i --) {
        request_free(flecs_sparse_get_dense(
            srv->requests, ecs_http_request_impl_t, i));
    }

    /* Close all connections */
    count = flecs_sparse_count(srv->connections);
    for (i = count - 1; i >= 1; i --) {
        connection_free(flecs_sparse_get_dense(
            srv->connections, ecs_http_connection_impl_t, i));
    }

    ecs_assert(flecs_sparse_count(srv->connections) == 1, 
        ECS_INTERNAL_ERROR, NULL);
    ecs_assert(flecs_sparse_count(srv->requests) == 1,
        ECS_INTERNAL_ERROR, NULL);

    srv->thread = 0;
error:
    return;
}

void ecs_http_server_dequeue(
    ecs_http_server_t* srv,
    float delta_time)
{
    ecs_check(srv != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(srv->initialized, ECS_INVALID_PARAMETER, NULL);
    ecs_check(srv->should_run, ECS_INVALID_PARAMETER, NULL);
    
    srv->dequeue_timeout += delta_time;
    srv->stats_timeout += delta_time;

    if ((1000 * srv->dequeue_timeout) > 
        (ecs_ftime_t)ECS_HTTP_MIN_DEQUEUE_INTERVAL) 
    {
        srv->dequeue_timeout = 0;

        ecs_time_t t = {0};
        ecs_time_measure(&t);
        int32_t request_count = dequeue_requests(srv, srv->dequeue_timeout);
        srv->requests_processed += request_count;
        srv->requests_processed_total += request_count;
        ecs_ftime_t time_spent = (ecs_ftime_t)ecs_time_measure(&t);
        srv->request_time += time_spent;
        srv->request_time_total += time_spent;
        srv->dequeue_count ++;
    }

    if ((1000 * srv->stats_timeout) > 
        (ecs_ftime_t)ECS_HTTP_MIN_STATS_INTERVAL) 
    {
        srv->stats_timeout = 0;
        ecs_dbg("http: processed %d requests in %.3fs (avg %.3fs / dequeue)",
            srv->requests_processed, (double)srv->request_time, 
            (double)(srv->request_time / (ecs_ftime_t)srv->dequeue_count));
        srv->requests_processed = 0;
        srv->request_time = 0;
        srv->dequeue_count = 0;
    }

error:
    return;
}

#endif
