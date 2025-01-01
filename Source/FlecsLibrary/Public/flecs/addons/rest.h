/**
 * @file addons/rest.h
 * @brief REST API addon.
 *
 * A small REST API that uses the HTTP server and JSON serializer to provide
 * access to application data for remote applications.
 *
 * A description of the API can be found in docs/FlecsRemoteApi.md
 */

#ifdef FLECS_REST

/**
 * @defgroup c_addons_rest Rest
 * @ingroup c_addons
 * REST API for querying and mutating entities.
 *
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

/** Component that instantiates the REST API. */
FLECS_API extern const ecs_entity_t ecs_id(EcsRest);

/** Component that creates a REST API server when instantiated. */
typedef struct {
    uint16_t port;      /**< Port of server (optional, default = 27750) */
    char *ipaddr;       /**< Interface address (optional, default = 0.0.0.0) */
    void *impl;
} EcsRest;

/** Create HTTP server for REST API.
 * This allows for the creation of a REST server that can be managed by the
 * application without using Flecs systems.
 *
 * @param world The world.
 * @param desc The HTTP server descriptor.
 * @return The HTTP server, or NULL if failed.
 */
FLECS_API
ecs_http_server_t* ecs_rest_server_init(
    ecs_world_t *world,
    const ecs_http_server_desc_t *desc);

/** Cleanup REST HTTP server.
 * The server must have been created with ecs_rest_server_init().
 */
FLECS_API
void ecs_rest_server_fini(
    ecs_http_server_t *srv);

/** Rest module import function.
 * Usage:
 * @code
 * ECS_IMPORT(world, FlecsRest)
 * @endcode
 * 
 * @param world The world.
 */
FLECS_API
void FlecsRestImport(
    ecs_world_t *world);

#ifdef __cplusplus
}
#endif

#endif

/** @} */

#endif
