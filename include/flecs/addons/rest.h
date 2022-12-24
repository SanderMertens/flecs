/**
 * @file addons/rest.h
 * @brief REST API addon.
 *
 * A small REST API that uses the HTTP server and JSON serializer to provide
 * access to application data for remote applications.
 * 
 * A description of the API can be found in docs/RestApi.md
 */

#ifdef FLECS_REST

/**
 * @defgroup c_addons_rest Rest
 * @brief REST API for querying and mutating entities.
 * 
 * \ingroup c_addons
 * @{
 */

/* Used for the HTTP server */
#ifndef FLECS_HTTP
#define FLECS_HTTP
#endif

/* Used for building the JSON replies */
#ifndef FLECS_JSON
#define FLECS_JSON
#endif

/* Query engine used */
#ifndef FLECS_RULES
#define FLECS_RULES
#endif

/* For the REST system */
#ifndef FLECS_PIPELINE
#define FLECS_PIPELINE
#endif

#ifndef FLECS_REST_H
#define FLECS_REST_H

#ifdef __cplusplus
extern "C" {
#endif

#define ECS_REST_DEFAULT_PORT (27750)

/** Component that instantiates the REST API */
FLECS_API extern const ecs_entity_t ecs_id(EcsRest);

typedef struct {
    uint16_t port;      /**< Port of server (optional, default = 27750) */
    char *ipaddr;       /**< Interface address (optional, default = 0.0.0.0) */
    void *impl;
} EcsRest;

/* Global statistics */
extern int64_t ecs_rest_request_count;
extern int64_t ecs_rest_entity_count;
extern int64_t ecs_rest_entity_error_count;
extern int64_t ecs_rest_query_count;
extern int64_t ecs_rest_query_error_count;
extern int64_t ecs_rest_query_name_count;
extern int64_t ecs_rest_query_name_error_count;
extern int64_t ecs_rest_query_name_from_cache_count;
extern int64_t ecs_rest_enable_count;
extern int64_t ecs_rest_enable_error_count;
extern int64_t ecs_rest_delete_count;
extern int64_t ecs_rest_delete_error_count;
extern int64_t ecs_rest_world_stats_count;
extern int64_t ecs_rest_pipeline_stats_count;
extern int64_t ecs_rest_stats_error_count;

/* Module import */
FLECS_API
void FlecsRestImport(
    ecs_world_t *world);

#ifdef __cplusplus
}
#endif

#endif

/** @} */

#endif
