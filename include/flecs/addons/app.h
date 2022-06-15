/**
 * @file app.h
 * @brief App addon.
 *
 * The app addon is a wrapper around the application's main loop. Its main
 * purpose is to provide a hook to modules that need to take control of the
 * main loop, as is for example the case with native applications that use
 * emscripten with webGL.
 */

#ifdef FLECS_APP

#ifndef FLECS_PIPELINE
#define FLECS_PIPELINE
#endif

#ifndef FLECS_APP_H
#define FLECS_APP_H

#ifdef __cplusplus
extern "C" {
#endif

/** Callback type for init action. */
typedef int(*ecs_app_init_action_t)(
    ecs_world_t *world);

/** Used with ecs_app_run. */
typedef struct ecs_app_desc_t {
    ecs_ftime_t target_fps; /* Target FPS. */
    ecs_ftime_t delta_time; /* Frame time increment (0 for measured values) */
    int32_t threads;          /* Number of threads. */
    bool enable_rest;         /* Allows HTTP clients to access ECS data */
    bool enable_monitor;      /* Periodically collect statistics */

    ecs_app_init_action_t init; /* If set, function is ran before starting the
                                 * main loop. */

    void *ctx;                /* Reserved for custom run/frame actions */
} ecs_app_desc_t;

/** Callback type for run action. */
typedef int(*ecs_app_run_action_t)(
    ecs_world_t *world, 
    ecs_app_desc_t *desc);

/** Callback type for frame action. */
typedef int(*ecs_app_frame_action_t)(
    ecs_world_t *world, 
    const ecs_app_desc_t *desc);

/** Run application.
 * This will run the application with the parameters specified in desc. After
 * the application quits (ecs_quit is called) the world will be cleaned up.
 * 
 * If a custom run action is set, it will be invoked by this operation. The
 * default run action calls the frame action in a loop until it returns a
 * non-zero value.
 * 
 * @param world The world.
 * @param desc Application parameters.
 */
FLECS_API
int ecs_app_run(
    ecs_world_t *world,
    ecs_app_desc_t *desc);

/** Default frame callback.
 * This operation will run a single frame. By default this operation will invoke
 * ecs_progress directly, unless a custom frame action is set.
 * 
 * @param world The world.
 * @param desc The desc struct passed to ecs_app_run.
 * @return value returned by ecs_progress
 */
FLECS_API
int ecs_app_run_frame(
    ecs_world_t *world,
    const ecs_app_desc_t *desc);

/** Set custom run action.
 * See ecs_app_run.
 * 
 * @param callback The run action.
 */
FLECS_API
int ecs_app_set_run_action(
    ecs_app_run_action_t callback);

/** Set custom frame action.
 * See ecs_app_run_frame.
 * 
 * @param callback The frame action.
 */
FLECS_API
int ecs_app_set_frame_action(
    ecs_app_frame_action_t callback);

#ifdef __cplusplus
}
#endif

#endif

#endif // FLECS_APP
