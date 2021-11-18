/**
 * @file rest.h
 * @brief REST API addon.
 *
 * A small REST API that uses the HTTP server and JSON serializer to provide
 * access to application data for remote applications.
 * 
 * The endpoints exposed by the REST API are:
 * 
 * /entity/<path>
 *   The entity endpoint requests data from an entity. The path is the entity
 *   path or name of the entity to query for. The format of the response is
 *   the same as what is returned by ecs_entity_to_json.
 * 
 *   Example:
 *     /entity/my_entity
 *     /entity/parent/child
 *     /entity/420
 * 
 * /query?q=<query>
 *   The query endpoint requests data for a query. The implementation uses the
 *   rules query engine. The format of the response is the same as what is
 *   returned by ecs_iter_to_json.
 * 
 *   Example:
 *     /query?q=Position
 *     /query?q=Position%2CVelocity
 */

#ifdef FLECS_REST

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

/* Component that instantiates the REST API */
FLECS_API extern const ecs_entity_t ecs_id(EcsRest);

typedef struct {
    uint16_t port;        /* Port of server (optional, default = 27750) */
    char *ipaddr;         /* Interface address (optional, default = 0.0.0.0) */
    void *impl;
} EcsRest;

/* Module import */
FLECS_API
void FlecsRestImport(
    ecs_world_t *world);

#ifdef __cplusplus
}
#endif

#endif

#endif
