/**
 * @file addons/system.h
 * @brief System module.
 *
 * The system module allows for creating and running systems. A system is a
 * query in combination with a callback function. In addition systems have
 * support for time management and can be monitored by the stats addon.
 */

#ifdef FLECS_SYSTEM

/**
 * @defgroup c_addons_system System
 * @brief Systems are a query + function that can be ran manually or by a pipeline.
 * 
 * \ingroup c_addons
 * @{
 */

#ifndef FLECS_MODULE
#define FLECS_MODULE
#endif

#ifndef FLECS_SYSTEM_H
#define FLECS_SYSTEM_H

#ifdef __cplusplus
extern "C" {
#endif

/** Component used to provide a tick source to systems */
typedef struct EcsTickSource {
    bool tick;                 /**< True if providing tick */
    ecs_ftime_t time_elapsed;  /**< Time elapsed since last tick */
} EcsTickSource;

/** Use with ecs_system_init */
typedef struct ecs_system_desc_t {
    int32_t _canary;

    /** Existing entity to associate with system (optional) */
    ecs_entity_t entity;

    /** System query parameters */
    ecs_query_desc_t query;

    /** Callback that is invoked when a system is ran. 
     * When left to NULL, the default system runner is used, which calls the 
     * "callback" action for each result returned from the system's query. 
     * 
     * It should not be assumed that the input iterator can always be iterated
     * with ecs_query_next. When a system is multithreaded and/or paged, the
     * iterator can be either a worker or paged iterator. Future use cases may
     * introduce additional inputs for a system, such as rules and filters. The
     * correct function to use for iteration is ecs_iter_next.
     * 
     * An implementation can test whether the iterator is a query iterator by
     * testing whether the it->next value is equal to ecs_query_next. */
    ecs_run_action_t run;

    /** Callback that is ran for each result returned by the system's query. This
     * means that this callback can be invoked multiple times per system per
     * frame, typically once for each matching table. */
    ecs_iter_action_t callback;

    /** Context to be passed to callback (as ecs_iter_t::param) */
    void *ctx;

    /** Binding context, for when system is implemented in other language */
    void *binding_ctx;

    /** Functions that are invoked during system cleanup to free context data.
     * When set, functions are called unconditionally, even when the ctx
     * pointers are NULL. */
    ecs_ctx_free_t ctx_free;
    ecs_ctx_free_t binding_ctx_free;

    /** Interval in seconds at which the system should run */
    ecs_ftime_t interval;

    /** Rate at which the system should run */
    int32_t rate;

    /** External tick soutce that determines when system ticks */
    ecs_entity_t tick_source;

    /** If true, system will be ran on multiple threads */
    bool multi_threaded;

    /** If true, system will have access to actuall world. Cannot be true at the
     * same time as multi_threaded. */
    bool no_readonly;
} ecs_system_desc_t;

/** Create a system */
FLECS_API
ecs_entity_t ecs_system_init(
    ecs_world_t *world,
    const ecs_system_desc_t *desc);

#ifndef FLECS_LEGACY

/** Forward declare a system. */
#define ECS_SYSTEM_DECLARE(id) ecs_entity_t ecs_id(id)

/** Define a forward declared system.
 * 
 * Example:
 *   ECS_SYSTEM_DEFINE(world, Move, EcsOnUpdate, Position, Velocity);
 */
#define ECS_SYSTEM_DEFINE(world, id_, phase, ...) \
    { \
        ecs_system_desc_t desc = {0}; \
        ecs_entity_desc_t edesc = {0}; \
        edesc.id = ecs_id(id_);\
        edesc.name = #id_;\
        edesc.add[0] = ((phase) ? ecs_pair(EcsDependsOn, (phase)) : 0); \
        edesc.add[1] = (phase); \
        desc.entity = ecs_entity_init(world, &edesc);\
        desc.query.filter.expr = #__VA_ARGS__; \
        desc.callback = id_; \
        ecs_id(id_) = ecs_system_init(world, &desc); \
    } \
    ecs_assert(ecs_id(id_) != 0, ECS_INVALID_PARAMETER, NULL);

/** Declare & define a system.
 * 
 * Example:
 *   ECS_SYSTEM(world, Move, EcsOnUpdate, Position, Velocity);
 */
#define ECS_SYSTEM(world, id, phase, ...) \
    ecs_entity_t ecs_id(id) = 0; ECS_SYSTEM_DEFINE(world, id, phase, __VA_ARGS__);\
    ecs_entity_t id = ecs_id(id);\
    (void)ecs_id(id);\
    (void)id;

/** Shorthand for creating a system with ecs_system_init.
 *
 * Example:
 *   ecs_system(world, {
 *     .entity = ecs_entity(world, {
 *       .name = "MyEntity",
 *       .add = { ecs_dependson(EcsOnUpdate) }
 *     },
 *     .query.filter.terms = {
 *       { ecs_id(Position) },
 *       { ecs_id(Velocity) }
 *     },
 *     .callback = Move
 *   });
 */
#define ecs_system(world, ...)\
    ecs_system_init(world, &(ecs_system_desc_t) __VA_ARGS__ )

#endif

/** Run a specific system manually.
 * This operation runs a single system manually. It is an efficient way to
 * invoke logic on a set of entities, as manual systems are only matched to
 * tables at creation time or after creation time, when a new table is created.
 *
 * Manual systems are useful to evaluate lists of prematched entities at
 * application defined times. Because none of the matching logic is evaluated
 * before the system is invoked, manual systems are much more efficient than
 * manually obtaining a list of entities and retrieving their components.
 *
 * An application may pass custom data to a system through the param parameter.
 * This data can be accessed by the system through the param member in the
 * ecs_iter_t value that is passed to the system callback.
 *
 * Any system may interrupt execution by setting the interrupted_by member in
 * the ecs_iter_t value. This is particularly useful for manual systems, where
 * the value of interrupted_by is returned by this operation. This, in
 * cominbation with the param argument lets applications use manual systems
 * to lookup entities: once the entity has been found its handle is passed to
 * interrupted_by, which is then subsequently returned.
 *
 * @param world The world.
 * @param system The system to run.
 * @param delta_time The time passed since the last system invocation.
 * @param param A user-defined parameter to pass to the system.
 * @return handle to last evaluated entity if system was interrupted.
 */
FLECS_API
ecs_entity_t ecs_run(
    ecs_world_t *world,
    ecs_entity_t system,
    ecs_ftime_t delta_time,
    void *param);

/** Same as ecs_run, but subdivides entities across number of provided stages.
 * 
 * @param world The world.
 * @param system The system to run.
 * @param stage_current The id of the current stage.
 * @param stage_count The total number of stages.
 * @param delta_time The time passed since the last system invocation.
 * @param param A user-defined parameter to pass to the system.
 * @return handle to last evaluated entity if system was interrupted.
 */
FLECS_API
ecs_entity_t ecs_run_worker(
    ecs_world_t *world,
    ecs_entity_t system,
    int32_t stage_current,
    int32_t stage_count,
    ecs_ftime_t delta_time,
    void *param);

/** Run system with offset/limit and type filter.
 * This operation is the same as ecs_run, but filters the entities that will be
 * iterated by the system.
 * 
 * Entities can be filtered in two ways. Offset and limit control the range of
 * entities that is iterated over. The range is applied to all entities matched
 * with the system, thus may cover multiple archetypes.
 * 
 * The type filter controls which entity types the system will evaluate. Only
 * types that contain all components in the type filter will be iterated over. A
 * type filter is only evaluated once per table, which makes filtering cheap if
 * the number of entities is large and the number of tables is small, but not as
 * cheap as filtering in the system signature.
 * 
 * @param world The world.
 * @param system The system to invoke.
 * @param delta_time The time passed since the last system invocation.
 * @param param A user-defined parameter to pass to the system.
 * @return handle to last evaluated entity if system was interrupted.
 */
FLECS_API
ecs_entity_t ecs_run_w_filter(
    ecs_world_t *world,
    ecs_entity_t system,
    ecs_ftime_t delta_time,
    int32_t offset,
    int32_t limit,
    void *param);

/** Get the query object for a system.
 * Systems use queries under the hood. This enables an application to get access
 * to the underlying query object of a system. This can be useful when, for 
 * example, an application needs to enable sorting for a system.
 *
 * @param world The world.
 * @param system The system from which to obtain the query.
 * @return The query.
 */
FLECS_API
ecs_query_t* ecs_system_get_query(
    const ecs_world_t *world,
    ecs_entity_t system);

/** Get system context.
 * This operation returns the context pointer set for the system. If
 * the provided entity is not a system, the function will return NULL.
 *
 * @param world The world.
 * @param system The system from which to obtain the context.
 * @return The context.
 */
FLECS_API
void* ecs_get_system_ctx(
    const ecs_world_t *world,
    ecs_entity_t system);

/** Get system binding context.
 * The binding context is a context typically used to attach any language 
 * binding specific data that is needed when invoking a callback that is 
 * implemented in another language.
 *
 * @param world The world.
 * @param system The system from which to obtain the context.
 * @return The context.
 */
FLECS_API
void* ecs_get_system_binding_ctx(
    const ecs_world_t *world,
    ecs_entity_t system);

FLECS_API
void FlecsSystemImport(
    ecs_world_t *world);

#ifdef __cplusplus
}
#endif

#endif

/** @} */

#endif
