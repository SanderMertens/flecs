#include "../private_api.h"

#ifdef FLECS_APP

static ecs_frame_action_t frame_action;

int ecs_app_run(
    ecs_world_t *world,
    const ecs_app_desc_t *desc)
{
    ecs_set_target_fps(world, desc->target_fps);
    ecs_set_threads(world, desc->threads);

    ecs_frame_action_t callback = frame_action;
    if (!callback) {
        callback = ecs_app_run_frame;
    }

    if (desc->enable_rest) {
#ifdef FLECS_REST
        ecs_set(world, EcsWorld, EcsRest, {.port = 0});
#else
        ecs_warn("cannot enable remote API, REST addon not available");
#endif
    }

    int result;
    while ((result = callback(world, desc)) == 0) { }

    return result;
}

int ecs_app_run_frame(
    ecs_world_t *world,
    const ecs_app_desc_t *desc)
{
    return !ecs_progress(world, desc->delta_time);
}

int ecs_app_set_frame_action(
    ecs_frame_action_t callback)
{
    if (frame_action) {
        ecs_err("frame action already set");
        return -1;
    }

    frame_action = callback;

    return 0;
}

#endif
