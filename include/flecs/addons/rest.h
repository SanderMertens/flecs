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
 *   Parameters:
 *    The following parameters can be added to the endpoint:
 * 
 *    - path : bool
 *      Add path (name) for entity.
 *        Default: true
 * 
 *    - base : bool
 *      Add base components.
 *        Default: true
 * 
 *    - values : bool
 *      Add component values.
 *        Default: true
 * 
 *    - private : bool
 *      Add private components.
 *        Default: false
 * 
 *    - type_info : bool
 *      Add reflection data for component types. Requires values=true.
 *        Default: false
 * 
 * 
 * /query?q=<query>
 *   The query endpoint requests data for a query. The implementation uses the
 *   rules query engine. The format of the response is the same as what is
 *   returned by ecs_iter_to_json.
 * 
 *   Example:
 *     /query?q=Position
 *     /query?q=Position%2CVelocity
 * 
 *   Parameters:
 *    The following parameters can be added to the endpoint:
 * 
 *    - term_ids : bool
 *      Add top-level "ids" array with components as specified by query.
 *        Default: true
 * 
 *    - ids : bool
 *      Add result-specific "ids" array with components as matched. Can be
 *      different from top-level "ids" array for queries with wildcards.
 *        Default: true
 * 
 *    - subjects : bool
 *      Add result-specific "subjects" array with component source. A 0 element
 *      indicates the component is matched on the current (This) entity.
 *        Default: true
 *      
 *    - variables : bool
 *      Add result-specific "variables" array with values for variables, if any.
 *        Default: true
 * 
 *    - is_set : bool
 *      Add result-specific "is_set" array with boolean elements indicating
 *      whether component was matched (used for optional terms).
 *        Default: true
 * 
 *    - values : bool
 *      Add result-specific "values" array with component values. A 0 element
 *      indicates a component that could not be serialized, which can be either
 *      because no reflection data was registered, because the component has no
 *      data, or because the query didn't request it.
 *        Default: true
 * 
 *    - entities : bool
 *      Add result-specific "entities" array with matched entities.
 *        Default: true
 * 
 *    - duration : bool
 *      Include measurement on how long it took to serialize result.
 *        Default: false
 *      
 *    - type_info : bool
 *        Add top-level "type_info" array with reflection data on the type in
 *        the query. If a query element has a component that has no reflection
 *        data, a 0 element is added to the array.
 *          Default: false
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
