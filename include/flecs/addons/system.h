/**
 * @file system.h
 * @brief System module.
 *
 * The system module allows for creating and running systems. A system is a
 * query in combination with a callback function. In addition systems have
 * support for time management and can be monitored by the stats addon.
 */

#ifdef FLECS_SYSTEM

#ifndef FLECS_MODULE
#define FLECS_MODULE
#endif

#include "../addons/module.h"

#ifndef FLECS_SYSTEMS_H
#define FLECS_SYSTEMS_H

#ifdef __cplusplus
extern "C" {
#endif


////////////////////////////////////////////////////////////////////////////////
//// Components
////////////////////////////////////////////////////////////////////////////////

/* Component used to provide a tick source to systems */
typedef struct EcsTickSource {
    bool tick;                 /* True if providing tick */
    FLECS_FLOAT time_elapsed;  /* Time elapsed since last tick */
} EcsTickSource;

////////////////////////////////////////////////////////////////////////////////
//// Systems API
////////////////////////////////////////////////////////////////////////////////

/** System status change callback */
typedef enum ecs_system_status_t {
    EcsSystemStatusNone = 0,
    EcsSystemEnabled,
    EcsSystemDisabled,
    EcsSystemActivated,
    EcsSystemDeactivated
} ecs_system_status_t;

/** System status action.
 * The status action is invoked whenever a system is enabled or disabled. Note
 * that a system may be enabled but may not actually match any entities. In this
 * case the system is enabled but not _active_.
 *
 * In addition to communicating the enabled / disabled status, the action also
 * communicates changes in the activation status of the system. A system becomes
 * active when it has one or more matching entities, and becomes inactive when
 * it no longer matches any entities.
 * 
 * A system switches between enabled and disabled when an application invokes the
 * ecs_enable operation with a state different from the state of the system, for
 * example the system is disabled, and ecs_enable is invoked with enabled: true.
 *
 * Additionally a system may switch between enabled and disabled when it is an
 * EcsOnDemand system, and interest is generated or lost for one of its [out]
 * columns.
 *
 * @param world The world.
 * @param system The system for which to set the action.
 * @param action The action.
 * @param ctx Context that will be passed to the action when invoked.
 */
typedef void (*ecs_system_status_action_t)(
    ecs_world_t *world,
    ecs_entity_t system,
    ecs_system_status_t status,
    void *ctx);

/* Use with ecs_system_init */
typedef struct ecs_system_desc_t {
    /* System entity creation parameters */
    ecs_entity_desc_t entity;

    /* System query parameters */
    ecs_query_desc_t query;

    /* System callback, invoked when system is ran */
    ecs_iter_action_t callback;

    /* System status callback, invoked when system status changes */
    ecs_system_status_action_t status_callback;

    /* Associate with entity */
    ecs_entity_t self;    

    /* Context to be passed to callback (as ecs_iter_t::param) */
    void *ctx;

    /* Context to be passed to system status callback */
    void *status_ctx;

    /* Binding context, for when system is implemented in other language */
    void *binding_ctx;

    /* Functions that are invoked during system cleanup to free context data.
     * When set, functions are called unconditionally, even when the ctx
     * pointers are NULL. */
    ecs_ctx_free_t ctx_free;
    ecs_ctx_free_t status_ctx_free;
    ecs_ctx_free_t binding_ctx_free;   

    /* Interval in seconds at which the system should run */
    FLECS_FLOAT interval;

    /* Rate at which the system should run */
    int32_t rate;

    /* External tick soutce that determines when system ticks */
    ecs_entity_t tick_source;     
} ecs_system_desc_t;

/* Create a system */
FLECS_API
ecs_entity_t ecs_system_init(
    ecs_world_t *world,
    const ecs_system_desc_t *desc);

#ifndef FLECS_LEGACY
#define ECS_SYSTEM(world, id, kind, ...) \
    ecs_iter_action_t ecs_iter_action(id) = id;\
    ecs_entity_t id = ecs_system_init(world, &(ecs_system_desc_t){\
        .entity = { .name = #id, .add = {kind} },\
        .query.filter.expr = #__VA_ARGS__,\
        .callback = ecs_iter_action(id)\
    });\
    ecs_assert(id != 0, ECS_INVALID_PARAMETER, NULL);\
    (void)ecs_iter_action(id);\
    (void)id;
#endif

/* Deprecated, use ecs_trigger_init */
#define ECS_TRIGGER(world, trigger_name, kind, component) \
    ecs_entity_t __F##trigger_name = ecs_trigger_init(world, &(ecs_trigger_desc_t){\
        .entity.name = #trigger_name,\
        .callback = trigger_name,\
        .expr = #component,\
        .events = {kind},\
    });\
    ecs_entity_t trigger_name = __F##trigger_name;\
    ecs_assert(trigger_name != 0, ECS_INVALID_PARAMETER, NULL);\
    (void)__F##trigger_name;\
    (void)trigger_name;

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
 * @param delta_time: The time passed since the last system invocation.
 * @param param A user-defined parameter to pass to the system.
 * @return handle to last evaluated entity if system was interrupted.
 */
FLECS_API
ecs_entity_t ecs_run(
    ecs_world_t *world,
    ecs_entity_t system,
    FLECS_FLOAT delta_time,
    void *param);

/** Same as ecs_run, but subdivides entities across number of provided stages.
 * 
 * @param world The world.
 * @param system The system to run.
 * @param stage_current The id of the current stage.
 * @param stage_count The total number of stages.
 * @param delta_time: The time passed since the last system invocation.
 * @param param A user-defined parameter to pass to the system.
 * @return handle to last evaluated entity if system was interrupted.
 */
FLECS_API
ecs_entity_t ecs_run_worker(
    ecs_world_t *world,
    ecs_entity_t system,
    int32_t stage_current,
    int32_t stage_count,
    FLECS_FLOAT delta_time,
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
 * @param delta_time: The time passed since the last system invocation.
 * @param filter A component or type to filter matched entities.
 * @param param A user-defined parameter to pass to the system.
 * @return handle to last evaluated entity if system was interrupted.
 */
FLECS_API
ecs_entity_t ecs_run_w_filter(
    ecs_world_t *world,
    ecs_entity_t system,
    FLECS_FLOAT delta_time,
    int32_t offset,
    int32_t limit,
    const ecs_filter_t *filter,
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
ecs_query_t* ecs_get_system_query(
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


////////////////////////////////////////////////////////////////////////////////
//// Module
////////////////////////////////////////////////////////////////////////////////

/* Pipeline component is empty: components and tags in module are static */
typedef struct FlecsSystem {
    int32_t dummy; 
} FlecsSystem;

FLECS_API
void FlecsSystemImport(
    ecs_world_t *world);

#define FlecsSystemImportHandles(handles)

#ifdef __cplusplus
}
#endif

#endif

#endif
