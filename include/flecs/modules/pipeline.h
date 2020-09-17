#ifdef FLECS_PIPELINE
#define FLECS_SYSTEM

#include "flecs/modules/system.h"

#ifndef FLECS_PIPELINE_H
#define FLECS_PIPELINE_H

#ifdef __cplusplus
extern "C" {
#endif

////////////////////////////////////////////////////////////////////////////////
//// Pipeline API
////////////////////////////////////////////////////////////////////////////////

#ifndef FLECS_LEGACY
#define ECS_PIPELINE(world, name, ...) \
    ecs_entity_t name = ecs_new_pipeline(world, 0, #name, #__VA_ARGS__);
#endif

/** Set a custom pipeline.
 * This operation sets the pipeline to run when ecs_progress is invoked.
 *
 * @param world The world.
 * @param pipeline The pipeline to set.
 */
FLECS_EXPORT
void ecs_set_pipeline(
    ecs_world_t *world,
    ecs_entity_t pipeline);       

/** Get the current pipeline.
 * This operation gets the current pipeline.
 *
 * @param world The world.
 * @param pipeline The pipeline to set.
 */
FLECS_EXPORT
ecs_entity_t ecs_get_pipeline(
    ecs_world_t *world);  

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
FLECS_EXPORT
bool ecs_progress(
    ecs_world_t *world,
    float delta_time);   

/** Set target frames per second (FPS) for application.
 * Setting the target FPS ensures that ecs_progress is not invoked faster than
 * the specified FPS. When enabled, ecs_progress tracks the time passed since
 * the last invocation, and sleeps the remaining time of the frame (if any).
 *
 * This feature ensures systems are ran at a consistent interval, as well as
 * conserving CPU time by not running systems more often than required.
 *
 * Note that ecs_progress only sleeps if there is time left in the frame. Both
 * time spent in flecs as time spent outside of flecs are taken into
 * account.
 *
 * @param world The world.
 * @param fps The target FPS.
 */
FLECS_EXPORT
void ecs_set_target_fps(
    ecs_world_t *world,
    float fps);

/** Set time scale.
 * Increase or decrease simulation speed by the provided multiplier.
 *
 * @param world The world.
 * @param scale The scale to apply (default = 1).
 */
FLECS_EXPORT 
void ecs_set_time_scale(
    ecs_world_t *world,
    float scale);

/** Reset world clock.
 * Reset the clock that keeps track of the total time passed in the simulation.
 *
 * @param world The world.
 */
FLECS_EXPORT
void ecs_reset_clock(
    ecs_world_t *world);

/** Signal exit
 * This operation signals that the application should quit. It will cause
 * ecs_progress to return false.
 *
 * @param world The world to quit.
 */
FLECS_EXPORT
void ecs_quit(
    ecs_world_t *world);

/** Deactivate systems that are not matched with tables.
 * By default Flecs deactivates systems that are not matched with any tables.
 * However, once a system has been matched with a table it remains activated, to
 * prevent systems from continuously becoming active and inactive.
 *
 * To re-deactivate systems, an application can invoke this function, which will
 * deactivate all systems that are not matched with any tables.
 *
 * @param world The world.
 */
FLECS_EXPORT
void ecs_deactivate_systems(
    ecs_world_t *world);


////////////////////////////////////////////////////////////////////////////////
//// Threading
////////////////////////////////////////////////////////////////////////////////

/** Set number of worker threads.
 * Setting this value to a value higher than 1 will start as many threads and
 * will cause systems to evenly distribute matched entities across threads. The
 * operation may be called multiple times to reconfigure the number of threads
 * used, but never while running a system / pipeline. */
FLECS_EXPORT
void ecs_set_threads(
    ecs_world_t *world,
    int32_t threads);

/** Get current number of threads. */
FLECS_EXPORT
int32_t ecs_get_threads(
    ecs_world_t *world);

/** Get current thread index */
FLECS_EXPORT
int32_t ecs_get_thread_index(
    ecs_world_t *world);


////////////////////////////////////////////////////////////////////////////////
//// Module
////////////////////////////////////////////////////////////////////////////////

/* Pipeline component is empty: components and tags in module are static */
typedef struct FlecsPipeline {
    int32_t dummy; 
} FlecsPipeline;

FLECS_EXPORT
void FlecsPipelineImport(
    ecs_world_t *world);

#define FlecsPipelineImportHandles(handles)

#ifdef __cplusplus
}
#endif

#endif

#endif
