#include "../private_api.h"

#ifdef FLECS_APP

static
int default_run_action(
    ecs_world_t *world,
    ecs_app_desc_t *desc)
{
    if (desc->init) {
        desc->init(world);
    }

    int result;
    while ((result = ecs_app_run_frame(world, desc)) == 0) { }

    if (result == 1) {
        return 0; /* Normal exit */
    } else {
        return result; /* Error code */
    }
}

static
int default_frame_action(
    ecs_world_t *world,
    const ecs_app_desc_t *desc)
{
    return !ecs_progress(world, desc->delta_time);
}

static ecs_app_run_action_t run_action = default_run_action;
static ecs_app_frame_action_t frame_action = default_frame_action;
static ecs_app_desc_t ecs_app_desc;

int ecs_app_run(
    ecs_world_t *world,
    ecs_app_desc_t *desc)
{
    ecs_app_desc = *desc;

    /* Don't set FPS & threads if custom run action is set, as the platform on
     * which the app is running may not support it. */
    if (run_action == default_run_action) {
        ecs_set_target_fps(world, ecs_app_desc.target_fps);
        ecs_set_threads(world, ecs_app_desc.threads);
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
    if (run_action != default_run_action) {
        ecs_err("run action already set");
        return -1;
    }

    run_action = callback;

    return 0;
}

int ecs_app_set_frame_action(
    ecs_app_frame_action_t callback)
{
    if (frame_action != default_frame_action) {
        ecs_err("frame action already set");
        return -1;
    }

    frame_action = callback;

    return 0;
}

#endif
