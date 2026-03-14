/**
 * @file addons/http/http.h
 * @brief Internal types and functions for HTTP addon.
 */

#ifdef ECS_TARGET_MSVC
#pragma comment(lib, "Ws2_32.lib")
#endif

#if defined(ECS_TARGET_WINDOWS)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
typedef SOCKET ecs_http_socket_t;
#else
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netdb.h>
#include <strings.h>
#include <signal.h>
#include <fcntl.h>
#ifdef __FreeBSD__
#include <netinet/in.h>
#endif
typedef int ecs_http_socket_t;

#if !defined(MSG_NOSIGNAL)
#define MSG_NOSIGNAL (0)
#endif

#endif

#define ECS_HTTP_METHOD_LEN_MAX (8)
#define ECS_HTTP_CONNECTION_PURGE_TIMEOUT (1.0)         /* seconds */
#define ECS_HTTP_CONNECTION_PURGE_RETRY_COUNT (5)
#define ECS_HTTP_REQUEST_RECV_RETRY (10)
#define ECS_HTTP_MIN_DEQUEUE_INTERVAL (50)              /* milliseconds */
#define ECS_HTTP_MIN_STATS_INTERVAL (10 * 1000)         /* milliseconds */
#define ECS_HTTP_SEND_RECV_BUFFER_SIZE (64 * 1024)      /* bytes */
#define ECS_HTTP_REQUEST_LEN_MAX (10 * 1024 * 1024)     /* bytes */
#define ECS_HTTP_SEND_QUEUE_MAX (256)

typedef struct ecs_http_send_request_t {
    ecs_http_socket_t sock;
    char *headers;
    int32_t header_length;
    char *content;
    int32_t content_length;
} ecs_http_send_request_t;

/* Ring buffer queue for outgoing HTTP responses (head/tail indices) */
typedef struct ecs_http_send_queue_t {
    ecs_http_send_request_t requests[ECS_HTTP_SEND_QUEUE_MAX];
    int32_t head;
    int32_t tail;
    ecs_os_thread_t thread;
    int32_t wait_ms;
} ecs_http_send_queue_t;

typedef struct ecs_http_request_key_t {
    const char *array;
    ecs_size_t count;
} ecs_http_request_key_t;

typedef struct ecs_http_request_entry_t {
    char *content;
    int32_t content_length;
    int code;
    double time;
} ecs_http_request_entry_t;

struct ecs_http_server_t {
    bool should_run;
    bool running;

    ecs_http_socket_t sock;
    ecs_os_mutex_t lock;
    ecs_os_thread_t thread;

    ecs_http_reply_action_t callback;
    void *ctx;

    double cache_timeout;
    double cache_purge_timeout;

    ecs_sparse_t connections; /* sparse<http_connection_t> */
    ecs_sparse_t requests; /* sparse<http_request_t> */

    bool initialized;

    uint16_t port;
    const char *ipaddr;

    double dequeue_timeout; /* used to not lock request queue too often */
    double stats_timeout; /* used for periodic reporting of statistics */

    double request_time; /* time spent on requests in last stats interval */
    double request_time_total; /* total time spent on requests */
    int32_t requests_processed; /* requests processed in last stats interval */
    int32_t requests_processed_total; /* total requests processed */
    int32_t dequeue_count; /* number of dequeues in last stats interval */ 
    ecs_http_send_queue_t send_queue;

    ecs_hashmap_t request_cache;
};

/* State machine states for incremental HTTP request parsing */
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

/* Accumulates data from a partially received HTTP request */
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
    int32_t content_length;
    char *header_buf_ptr;
    char header_buf[32];
    bool parse_content_length;
    bool invalid;
} ecs_http_fragment_t;

/* Internal connection type extending the public ecs_http_connection_t */
typedef struct {
    ecs_http_connection_t pub;
    ecs_http_socket_t sock;

    /* Both timeout and retry threshold must be exceeded before purge */
    double dequeue_timeout;
    int32_t dequeue_retries;    
} ecs_http_connection_impl_t;

typedef struct {
    ecs_http_request_t pub;
    uint64_t conn_id; /* for sanity check */
    char *res;
    int32_t req_len;
} ecs_http_request_impl_t;
