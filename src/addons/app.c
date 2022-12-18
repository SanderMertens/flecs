/**
 * @file addons/app.c
 * @brief App addon.
 */

#include "../private_api.h"

#ifdef FLECS_APP

static
int flecs_default_run_action(
    ecs_world_t *world,
    ecs_app_desc_t *desc)
{
    if (desc->init) {
        desc->init(world);
    }

    int result = 0;
    if (desc->frames) {
        int32_t i;
        for (i = 0; i < desc->frames; i ++) {
            if ((result = ecs_app_run_frame(world, desc)) != 0) {
                break;
            }
        }
    } else {
        while ((result = ecs_app_run_frame(world, desc)) == 0) { }
    }

    if (result == 1) {
        return 0; /* Normal exit */
    } else {
        return result; /* Error code */
    }
}

static
int flecs_default_frame_action(
    ecs_world_t *world,
    const ecs_app_desc_t *desc)
{
    return !ecs_progress(world, desc->delta_time);
}

static ecs_app_run_action_t run_action = flecs_default_run_action;
static ecs_app_frame_action_t frame_action = flecs_default_frame_action;
static ecs_app_desc_t ecs_app_desc;

int ecs_app_run(
    ecs_world_t *world,
    ecs_app_desc_t *desc)
{
    ecs_app_desc = *desc;

    /* Don't set FPS & threads if custom run action is set, as the platform on
     * which the app is running may not support it. */
    if (run_action == flecs_default_run_action) {
        if (ecs_app_desc.target_fps != 0) {
            ecs_set_target_fps(world, ecs_app_desc.target_fps);
        }
        if (ecs_app_desc.threads) {
            ecs_set_threads(world, ecs_app_desc.threads);
        }
    }

    /* REST server enables connecting to app with explorer */
    if (desc->enable_rest) {
#ifdef FLECS_REST
        ecs_set(world, EcsWorld, EcsRest, {.port = 0});
#else
        ecs_warn("cannot enable remote API, REST addon not available");
#endif
    }

    /* Monitoring periodically collects statistics */
    if (desc->enable_monitor) {
#ifdef FLECS_MONITOR
        ECS_IMPORT(world, FlecsMonitor);
#else
        ecs_warn("cannot enable monitoring, MONITOR addon not available");
#endif
    }

    return run_action(world, &ecs_app_desc);
}

int ecs_app_run_frame(
    ecs_world_t *world,
    const ecs_app_desc_t *desc)
{
    return frame_action(world, desc);
}

int ecs_app_set_run_action(
    ecs_app_run_action_t callback)
{
    if (run_action != flecs_default_run_action) {
        ecs_err("run action already set");
        return -1;
    }

    run_action = callback;

    return 0;
}

int ecs_app_set_frame_action(
    ecs_app_frame_action_t callback)
{
    if (frame_action != flecs_default_frame_action) {
        ecs_err("frame action already set");
        return -1;
    }

    frame_action = callback;

    return 0;
}

#endif
