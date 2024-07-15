/**
 * @file addons/pipeline.h
 * @brief Pipeline module.
 *
 * The pipeline module provides support for running systems automatically and
 * on multiple threads. A pipeline is a collection of tags that can be added to
 * systems. When ran, a pipeline will query for all systems that have the tags
 * that belong to a pipeline, and run them.
 *
 * The module defines a number of builtin tags (EcsPreUpdate, EcsOnUpdate,
 * EcsPostUpdate etc.) that are registered with the builtin pipeline. The
 * builtin pipeline is ran by default when calling ecs_progress(). An
 * application can set a custom pipeline with the ecs_set_pipeline() function.
 */

#ifdef FLECS_PIPELINE

/**
 * @defgroup c_addons_pipeline Pipeline
 * @ingroup c_addons
 * Pipelines order and schedule systems for execution.
 *
 * @{
 */

#ifndef FLECS_MODULE
#define FLECS_MODULE
#endif

#ifndef FLECS_SYSTEM
#define FLECS_SYSTEM
#endif

#if !defined(FLECS_OS_API_IMPL) && !defined(FLECS_NO_OS_API_IMPL)
#define FLECS_OS_API_IMPL
#endif

#ifndef FLECS_PIPELINE_H
#define FLECS_PIPELINE_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef FLECS_LEGACY

/** Convenience macro to create a predeclared pipeline. 
 * Usage:
 * @code
 * ECS_ENTITY_DECLARE(MyPipeline);
 * ECS_PIPELINE_DEFINE(world, MyPipeline, Update || Physics || Render)
 * @endcode
 */
#define ECS_PIPELINE_DEFINE(world, id_, ...) \
    { \
        ecs_pipeline_desc_t desc = {0}; \
        ecs_entity_desc_t edesc = {0}; \
        edesc.id = id_;\
        edesc.name = #id_;\
        desc.entity = ecs_entity_init(world, &edesc);\
        desc.query.expr = #__VA_ARGS__; \
        id_ = ecs_pipeline_init(world, &desc); \
        ecs_id(id_) = id_;\
    } \
    ecs_assert(id_ != 0, ECS_INVALID_PARAMETER, "failed to create pipeline");

/** Convenience macro to create a pipeline. 
 * Usage:
 * @code
 * ECS_PIPELINE(world, MyPipeline, Update || Physics || Render)
 * @endcode
 * 
 */
#define ECS_PIPELINE(world, id, ...) \
    ecs_entity_t id = 0, ecs_id(id) = 0; ECS_PIPELINE_DEFINE(world, id, __VA_ARGS__);\
    (void)id;\
    (void)ecs_id(id);

/** Convenience macro to create a pipeline. 
 * See ecs_pipeline_init().
 */
#define ecs_pipeline(world, ...)\
    ecs_pipeline_init(world, &(ecs_pipeline_desc_t) __VA_ARGS__ )

#endif

/** Pipeline descriptor, used with ecs_pipeline_init(). */
typedef struct ecs_pipeline_desc_t {
    /** Existing entity to associate with pipeline (optional). */
    ecs_entity_t entity;

    /** The pipeline query. 
     * Pipelines are queries that are matched with system entities. Pipeline
     * queries are the same as regular queries, which means the same query rules
     * apply. A common mistake is to try a pipeline that matches systems in a
     * list of phases by specifying all the phases, like:
     *   OnUpdate, OnPhysics, OnRender
     * 
     * That however creates a query that matches entities with OnUpdate _and_
     * OnPhysics _and_ OnRender tags, which is likely undesired. Instead, a
     * query could use the or operator match a system that has one of the
     * specified phases:
     *   OnUpdate || OnPhysics || OnRender
     * 
     * This will return the correct set of systems, but they likely won't be in
     * the correct order. To make sure systems are returned in the correct order
     * two query ordering features can be used:
     * - group_by
     * - order_by
     * 
     * Take a look at the system manual for a more detailed explanation of
     * how query features can be applied to pipelines, and how the builtin
     * pipeline query works.
    */
    ecs_query_desc_t query;
} ecs_pipeline_desc_t;

/** Create a custom pipeline.
 * 
 * @param world The world.
 * @param desc The pipeline descriptor.
 * @return The pipeline, 0 if failed.
 */
FLECS_API
ecs_entity_t ecs_pipeline_init(
    ecs_world_t *world,
    const ecs_pipeline_desc_t *desc);

/** Set a custom pipeline.
 * This operation sets the pipeline to run when ecs_progress() is invoked.
 *
 * @param world The world.
 * @param pipeline The pipeline to set.
 */
FLECS_API
void ecs_set_pipeline(
    ecs_world_t *world,
    ecs_entity_t pipeline);

/** Get the current pipeline.
 * This operation gets the current pipeline.
 *
 * @param world The world.
 * @return The current pipeline.
 */
FLECS_API
ecs_entity_t ecs_get_pipeline(
    const ecs_world_t *world);

/** Progress a world.
 * This operation progresses the world by running all systems that are both
 * enabled and periodic on their matching entities.
 *
 * An application can pass a delta_time into the function, which is the time
 * passed since the last frame. This value is passed to systems so they can
 * update entity values proportional to the elapsed time since their last
 * invocation.
 *
 * When an application passes 0 to delta_time, ecs_progress() will automatically
 * measure the time passed since the last frame. If an application does not uses
 * time management, it should pass a non-zero value for delta_time (1.0 is
 * recommended). That way, no time will be wasted measuring the time.
 *
 * @param world The world to progress.
 * @param delta_time The time passed since the last frame.
 * @return false if ecs_quit() has been called, true otherwise.
 */
FLECS_API
bool ecs_progress(
    ecs_world_t *world,
    ecs_ftime_t delta_time);

/** Set time scale.
 * Increase or decrease simulation speed by the provided multiplier.
 *
 * @param world The world.
 * @param scale The scale to apply (default = 1).
 */
FLECS_API
void ecs_set_time_scale(
    ecs_world_t *world,
    ecs_ftime_t scale);

/** Reset world clock.
 * Reset the clock that keeps track of the total time passed in the simulation.
 *
 * @param world The world.
 */
FLECS_API
void ecs_reset_clock(
    ecs_world_t *world);

/** Run pipeline.
 * This will run all systems in the provided pipeline. This operation may be
 * invoked from multiple threads, and only when staging is disabled, as the
 * pipeline manages staging and, if necessary, synchronization between threads.
 *
 * If 0 is provided for the pipeline id, the default pipeline will be ran (this
 * is either the builtin pipeline or the pipeline set with set_pipeline()).
 *
 * When using progress() this operation will be invoked automatically for the
 * default pipeline (either the builtin pipeline or the pipeline set with
 * set_pipeline()). An application may run additional pipelines.
 *
 * @param world The world.
 * @param pipeline The pipeline to run.
 * @param delta_time The delta_time to pass to systems.
 */
FLECS_API
void ecs_run_pipeline(
    ecs_world_t *world,
    ecs_entity_t pipeline,
    ecs_ftime_t delta_time);


////////////////////////////////////////////////////////////////////////////////
//// Threading
////////////////////////////////////////////////////////////////////////////////

/** Set number of worker threads.
 * Setting this value to a value higher than 1 will start as many threads and
 * will cause systems to evenly distribute matched entities across threads. The
 * operation may be called multiple times to reconfigure the number of threads
 * used, but never while running a system / pipeline.
 * Calling ecs_set_threads() will also end the use of task threads setup with
 * ecs_set_task_threads() and vice-versa.
 * 
 * @param world The world.
 * @param threads The number of threads to create. 
 */
FLECS_API
void ecs_set_threads(
    ecs_world_t *world,
    int32_t threads);

/** Set number of worker task threads.
 * ecs_set_task_threads() is similar to ecs_set_threads(), except threads are treated
 * as short-lived tasks and will be created and joined around each update of the world.
 * Creation and joining of these tasks will use the os_api_t tasks APIs rather than the
 * the standard thread API functions, although they may be the same if desired.
 * This function is useful for multithreading world updates using an external
 * asynchronous job system rather than long running threads by providing the APIs
 * to create tasks for your job system and then wait on their conclusion.
 * The operation may be called multiple times to reconfigure the number of task threads
 * used, but never while running a system / pipeline.
 * Calling ecs_set_task_threads() will also end the use of threads setup with
 * ecs_set_threads() and vice-versa 
 * 
 * @param world The world.
 * @param task_threads The number of task threads to create. 
 */
FLECS_API
void ecs_set_task_threads(
    ecs_world_t *world,
    int32_t task_threads);

/** Returns true if task thread use have been requested. 
 * 
 * @param world The world.
 * @result Whether the world is using task threads.
 */
FLECS_API
bool ecs_using_task_threads(
    ecs_world_t *world);

////////////////////////////////////////////////////////////////////////////////
//// Module
////////////////////////////////////////////////////////////////////////////////

/** Pipeline module import function.
 * Usage:
 * @code
 * ECS_IMPORT(world, FlecsPipeline)
 * @endcode
 * 
 * @param world The world.
 */
FLECS_API
void FlecsPipelineImport(
    ecs_world_t *world);

#ifdef __cplusplus
}
#endif

#endif

/** @} */

#endif
