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
 * @ingroup c_addons
 * Systems are a query + function that can be ran manually or by a pipeline.
 *
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

/** Use with ecs_system_init() to create or update a system. */
typedef struct ecs_system_desc_t {
    int32_t _canary;

    /** Existing entity to associate with system (optional) */
    ecs_entity_t entity;

    /** System query parameters */
    ecs_query_desc_t query;

    /** Callback that is ran for each result returned by the system's query. This
     * means that this callback can be invoked multiple times per system per
     * frame, typically once for each matching table. */
    ecs_iter_action_t callback;

    /** Callback that is invoked when a system is ran.
     * When left to NULL, the default system runner is used, which calls the
     * "callback" action for each result returned from the system's query.
     *
     * It should not be assumed that the input iterator can always be iterated
     * with ecs_query_next(). When a system is multithreaded and/or paged, the
     * iterator can be either a worker or paged iterator. The correct function 
     * to use for iteration is ecs_iter_next().
     *
     * An implementation can test whether the iterator is a query iterator by
     * testing whether the it->next value is equal to ecs_query_next(). */
    ecs_run_action_t run;

    /** Context to be passed to callback (as ecs_iter_t::param) */
    void *ctx;

    /** Callback to free ctx. */
    ecs_ctx_free_t ctx_free;

    /** Context associated with callback (for language bindings). */
    void *callback_ctx;

    /** Callback to free callback ctx. */
    ecs_ctx_free_t callback_ctx_free;

    /** Context associated with run (for language bindings). */
    void *run_ctx;

    /** Callback to free run ctx. */
    ecs_ctx_free_t run_ctx_free;
    
    /** Interval in seconds at which the system should run */
    ecs_ftime_t interval;

    /** Rate at which the system should run */
    int32_t rate;

    /** External tick source that determines when system ticks */
    ecs_entity_t tick_source;

    /** If true, system will be ran on multiple threads */
    bool multi_threaded;

    /** If true, system will have access to the actual world. Cannot be true at the
     * same time as multi_threaded. */
    bool immediate;
} ecs_system_desc_t;

/** Create a system */
FLECS_API
ecs_entity_t ecs_system_init(
    ecs_world_t *world,
    const ecs_system_desc_t *desc);

/** System type, get with ecs_system_get() */
typedef struct ecs_system_t {
    ecs_header_t hdr;

    /** See ecs_system_desc_t */
    ecs_run_action_t run;

    /** See ecs_system_desc_t */
    ecs_iter_action_t action;

    /** System query */
    ecs_query_t *query;

    /** Entity associated with query */
    ecs_entity_t query_entity;

    /** Tick source associated with system */
    ecs_entity_t tick_source;

    /** Is system multithreaded */
    bool multi_threaded;

    /** Is system ran in immediate mode */
    bool immediate;

    /** Userdata for system */
    void *ctx;

    /** Callback language binding context */
    void *callback_ctx;

    /** Run language binding context */
    void *run_ctx;

    /** Callback to free ctx. */
    ecs_ctx_free_t ctx_free;

    /** Callback to free callback ctx. */
    ecs_ctx_free_t callback_ctx_free;

    /** Callback to free run ctx. */
    ecs_ctx_free_t run_ctx_free;

    /** Time spent on running system */
    ecs_ftime_t time_spent;

    /** Time passed since last invocation */
    ecs_ftime_t time_passed;

    /** Last frame for which the system was considered */
    int64_t last_frame;

    /* Mixins */
    ecs_world_t *world;
    ecs_entity_t entity;
    flecs_poly_dtor_t dtor;      
} ecs_system_t;

/** Get system object.
 * Returns the system object. Can be used to access various information about
 * the system, like the query and context.
 *
 * @param world The world.
 * @param system The system.
 * @return The system object.
 */
FLECS_API
const ecs_system_t* ecs_system_get(
    const ecs_world_t *world,
    ecs_entity_t system);

#ifndef FLECS_LEGACY

/** Forward declare a system. */
#define ECS_SYSTEM_DECLARE(id) ecs_entity_t ecs_id(id)

/** Define a forward declared system.
 *
 * Example:
 *
 * @code
 * ECS_SYSTEM_DEFINE(world, Move, EcsOnUpdate, Position, Velocity);
 * @endcode
 */
#define ECS_SYSTEM_DEFINE(world, id_, phase, ...) \
    { \
        ecs_system_desc_t desc = {0}; \
        ecs_entity_desc_t edesc = {0}; \
        ecs_id_t add_ids[3] = {\
            ((phase) ? ecs_pair(EcsDependsOn, (phase)) : 0), \
            (phase), \
            0 \
        };\
        edesc.id = ecs_id(id_);\
        edesc.name = #id_;\
        edesc.add = add_ids;\
        desc.entity = ecs_entity_init(world, &edesc);\
        desc.query.expr = #__VA_ARGS__; \
        desc.callback = id_; \
        ecs_id(id_) = ecs_system_init(world, &desc); \
    } \
    ecs_assert(ecs_id(id_) != 0, ECS_INVALID_PARAMETER, "failed to create system %s", #id_)

/** Declare & define a system.
 *
 * Example:
 *
 * @code
 * ECS_SYSTEM(world, Move, EcsOnUpdate, Position, Velocity);
 * @endcode
 */
#define ECS_SYSTEM(world, id, phase, ...) \
    ecs_entity_t ecs_id(id) = 0; ECS_SYSTEM_DEFINE(world, id, phase, __VA_ARGS__);\
    ecs_entity_t id = ecs_id(id);\
    (void)ecs_id(id);\
    (void)id

/** Shorthand for creating a system with ecs_system_init().
 *
 * Example:
 *
 * @code
 * ecs_system(world, {
 *   .entity = ecs_entity(world, {
 *     .name = "MyEntity",
 *     .add = ecs_ids( ecs_dependson(EcsOnUpdate) )
 *   }),
 *   .query.terms = {
 *     { ecs_id(Position) },
 *     { ecs_id(Velocity) }
 *   },
 *   .callback = Move
 * });
 * @endcode
 */
#define ecs_system(world, ...)\
    ecs_system_init(world, &(ecs_system_desc_t) __VA_ARGS__ )

#endif

/** Run a specific system manually.
 * This operation runs a single system manually. It is an efficient way to
 * invoke logic on a set of entities, as manual systems are only matched to
 * tables at creation time or after creation time, when a new table is created.
 *
 * Manual systems are useful to evaluate lists of pre-matched entities at
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
 * combination with the param argument lets applications use manual systems
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

/** Same as ecs_run(), but subdivides entities across number of provided stages.
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

/** System module import function.
 * Usage:
 * @code
 * ECS_IMPORT(world, FlecsSystem)
 * @endcode
 * 
 * @param world The world.
 */
FLECS_API
void FlecsSystemImport(
    ecs_world_t *world);

#ifdef __cplusplus
}
#endif

#endif

/** @} */

#endif
