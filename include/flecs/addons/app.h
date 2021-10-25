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

#include "pipeline.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Used with ecs_app_run. */
typedef struct ecs_app_desc_t {
    FLECS_FLOAT target_fps;   /* Target FPS. */
    int32_t threads;          /* Number of threads. */
    FLECS_FLOAT delta_time;   /* Frame time increment (0 for measured values) */
} ecs_app_desc_t;

/** Run application.
 * This will run the application with the parameters specified in desc. After
 * the application quits (ecs_quit is called) the world will be cleaned up.
 * 
 * @param world The world.
 * @param desc Application parameters.
 */
FLECS_API
int ecs_app_run(
    ecs_world_t *world,
    const ecs_app_desc_t *desc);

/** Callback type for frame action. */
typedef int(*ecs_frame_action_t)(
    ecs_world_t *world, 
    const ecs_app_desc_t *desc);

/** Default frame callback.
 * This function will call ecs_progress until it returns a non-zero value (the
 * application called ecs_quit). It is the default frame callback that will be
 * used by ecs_app_run, unless it is overridden.
 * 
 * Applications, though typically modules, can override the frame callback by
 * using the ecs_app_set_frame_action function. This enables a module to take
 * control of the main loop when necessary.
 * 
 * All worlds share the same frame accallbacktion. When a module attempts to 
 * overwrite a frame callback (except the default one), an error will be thrown. 
 * This prevents applications from importing modules with conflicting 
 * requirements (e.g. two modules that * both need control over the main loop).
 * 
 * A custom frame callback may call this function once after or before it has 
 * ran its own logic.
 * 
 * @param world The world.
 * @param desc The desc struct passed to ecs_app_run.
 * @return value returned by ecs_progress
 */
FLECS_API
int ecs_app_run_frame(
    ecs_world_t *world,
    const ecs_app_desc_t *desc);

/** Set custom frame action.
 * See ecs_app_run_frame.
 * 
 * @param callback The frame action.
 */
FLECS_API
int ecs_app_set_frame_action(
    ecs_frame_action_t callback);

#ifdef __cplusplus
}
#endif

#endif

#endif // FLECS_APP
