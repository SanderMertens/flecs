/**
 * @file pipeline.h
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
 * application can set a custom pipeline with the ecs_set_pipeline function.
 */

#ifdef FLECS_PIPELINE

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

#define ECS_PIPELINE_DEFINE(world, id, ...)\
    id = ecs_pipeline_init(world, &(ecs_pipeline_desc_t){ \
        .entity.name = #id, \
        .query.filter.expr = #__VA_ARGS__\
    });\
    ecs_id(id) = id;\
    ecs_assert(id != 0, ECS_INVALID_OPERATION, NULL);

#define ECS_PIPELINE(world, id, ...) \
    ecs_entity_t ecs_id(id), ECS_PIPELINE_DEFINE(world, id, __VA_ARGS__);\
    (void)id;\
    (void)ecs_id(id);

/* Pipeline descriptor (used with ecs_pipeline_init) */
typedef struct ecs_pipeline_desc_t {
    /* Entity descriptor */
    ecs_entity_desc_t entity;
    
    /* Query descriptor. The first term of the query must match the EcsSystem
     * component. */
    ecs_query_desc_t query;
} ecs_pipeline_desc_t;

/** Create a custom pipeline.
 */
FLECS_API
ecs_entity_t ecs_pipeline_init(
    ecs_world_t *world,
    const ecs_pipeline_desc_t *desc);

/** Set a custom pipeline.
 * This operation sets the pipeline to run when ecs_progress is invoked.
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
 * When an application passes 0 to delta_time, ecs_progress will automatically
 * measure the time passed since the last frame. If an application does not uses
 * time management, it should pass a non-zero value for delta_time (1.0 is
 * recommended). That way, no time will be wasted measuring the time.
 *
 * @param world The world to progress.
 * @param delta_time The time passed since the last frame.
 * @return false if ecs_quit has been called, true otherwise.
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
 * Note: calling this function from an application currently only works in
 * single threaded applications with a single stage.
 *
 * @param world The world.
 * @param pipeline The pipeline to run.
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
 * used, but never while running a system / pipeline. */
FLECS_API
void ecs_set_threads(
    ecs_world_t *world,
    int32_t threads);

////////////////////////////////////////////////////////////////////////////////
//// Module
////////////////////////////////////////////////////////////////////////////////

FLECS_API
void FlecsPipelineImport(
    ecs_world_t *world);

#ifdef __cplusplus
}
#endif

#endif

#endif
