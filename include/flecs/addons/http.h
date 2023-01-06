/**
 * @file addons/http.h
 * @brief HTTP addon.
 * 
 * Minimalistic HTTP server that can receive and reply to simple HTTP requests.
 * The main goal of this addon is to enable remotely connecting to a running
 * Flecs application (for example, with a web-based UI) and request/visualize
 * data from the ECS world.
 * 
 * Each server instance creates a single thread used for receiving requests.
 * Receiving requests are enqueued and handled when the application calls
 * ecs_http_server_dequeue. This increases latency of request handling vs.
 * responding directly in the receive thread, but is better suited for 
 * retrieving data from ECS applications, as requests can be processed by an ECS
 * system without having to lock the world.
 * 
 * This server is intended to be used in a development environment.
 */

#ifdef FLECS_HTTP

/**
 * @defgroup c_addons_http Http
 * @brief Simple HTTP server used for serving up REST API.
 * 
 * \ingroup c_addons
 * @{
 */

#if !defined(FLECS_OS_API_IMPL) && !defined(FLECS_NO_OS_API_IMPL)
#define FLECS_OS_API_IMPL
#endif

#ifndef FLECS_HTTP_H
#define FLECS_HTTP_H

/* Maximum number of headers in request */
#define ECS_HTTP_HEADER_COUNT_MAX (32)

/* Maximum number of query parameters in request */
#define ECS_HTTP_QUERY_PARAM_COUNT_MAX (32)

#ifdef __cplusplus
extern "C" {
#endif

/** HTTP server */
typedef struct ecs_http_server_t ecs_http_server_t;

/** A connection manages communication with the remote host */
typedef struct {
    uint64_t id;
    ecs_http_server_t *server;

    char host[128];
    char port[16];
} ecs_http_connection_t;

/** Helper type used for headers & URL query parameters */
typedef struct {
    const char *key;
    const char *value;
} ecs_http_key_value_t;

/** Supported request methods */
typedef enum {
    EcsHttpGet,
    EcsHttpPost,
    EcsHttpPut,
    EcsHttpDelete,
    EcsHttpOptions,
    EcsHttpMethodUnsupported
} ecs_http_method_t;

/** A request */
typedef struct {
    uint64_t id;

    ecs_http_method_t method;
    char *path;
    char *body;
    ecs_http_key_value_t headers[ECS_HTTP_HEADER_COUNT_MAX];
    ecs_http_key_value_t params[ECS_HTTP_HEADER_COUNT_MAX];
    int32_t header_count;
    int32_t param_count;

    ecs_http_connection_t *conn;
} ecs_http_request_t;

/** A reply */
typedef struct {
    int code;                   /**< default = 200 */
    ecs_strbuf_t body;          /**< default = "" */
    const char* status;         /**< default = OK */
    const char* content_type;   /**< default = application/json */
    ecs_strbuf_t headers;       /**< default = "" */
} ecs_http_reply_t;

#define ECS_HTTP_REPLY_INIT \
    (ecs_http_reply_t){200, ECS_STRBUF_INIT, "OK", "application/json", ECS_STRBUF_INIT}

/* Global statistics. */
extern int64_t ecs_http_request_received_count;
extern int64_t ecs_http_request_invalid_count;
extern int64_t ecs_http_request_handled_ok_count;
extern int64_t ecs_http_request_handled_error_count;
extern int64_t ecs_http_request_not_handled_count;
extern int64_t ecs_http_request_preflight_count;
extern int64_t ecs_http_send_ok_count;
extern int64_t ecs_http_send_error_count;
extern int64_t ecs_http_busy_count;

/** Request callback.
 * Invoked for each valid request. The function should populate the reply and
 * return true. When the function returns false, the server will reply with a 
 * 404 (Not found) code. */
typedef bool (*ecs_http_reply_action_t)(
    const ecs_http_request_t* request, 
    ecs_http_reply_t *reply,
    void *ctx);

/** Used with ecs_http_server_init. */
typedef struct {
    ecs_http_reply_action_t callback; /**< Function called for each request  */
    void *ctx;                        /**< Passed to callback (optional) */
    uint16_t port;                    /**< HTTP port */
    const char *ipaddr;               /**< Interface to listen on (optional) */
    int32_t send_queue_wait_ms;       /**< Send queue wait time when empty */
} ecs_http_server_desc_t;

/** Create server. 
 * Use ecs_http_server_start to start receiving requests.
 * 
 * @param desc Server configuration parameters.
 * @return The new server, or NULL if creation failed.
 */
FLECS_API
ecs_http_server_t* ecs_http_server_init(
    const ecs_http_server_desc_t *desc);

/** Destroy server. 
 * This operation will stop the server if it was still running.
 * 
 * @param server The server to destroy.
 */
FLECS_API
void ecs_http_server_fini(
    ecs_http_server_t* server);

/** Start server. 
 * After this operation the server will be able to accept requests.
 * 
 * @param server The server to start.
 * @return Zero if successful, non-zero if failed.
 */
FLECS_API
int ecs_http_server_start(
    ecs_http_server_t* server);

/** Process server requests. 
 * This operation invokes the reply callback for each received request. No new
 * requests will be enqueued while processing requests.
 * 
 * @param server The server for which to process requests.
 */
FLECS_API
void ecs_http_server_dequeue(
    ecs_http_server_t* server,
    ecs_ftime_t delta_time);

/** Stop server. 
 * After this operation no new requests can be received.
 * 
 * @param server The server.
 */
FLECS_API
void ecs_http_server_stop(
    ecs_http_server_t* server);

/** Find header in request. 
 * 
 * @param req The request.
 * @param name name of the header to find
 * @return The header value, or NULL if not found.
*/
FLECS_API
const char* ecs_http_get_header(
    const ecs_http_request_t* req,
    const char* name);

/** Find query parameter in request. 
 * 
 * @param req The request.
 * @param name The parameter name.
 * @return The decoded parameter value, or NULL if not found.
 */
FLECS_API
const char* ecs_http_get_param(
    const ecs_http_request_t* req,
    const char* name);

#ifdef __cplusplus
}
#endif

/** @} */

#endif // FLECS_HTTP_H

#endif // FLECS_HTTP
