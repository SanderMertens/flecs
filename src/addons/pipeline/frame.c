#include "../../private_api.h"

#ifdef FLECS_PIPELINE
#include "pipeline.h"

static
ecs_ftime_t flecs_insert_sleep(
    ecs_world_t *world,
    ecs_time_t *stop)
{
    flecs_poly_assert(world, ecs_world_t);  

    ecs_time_t start = *stop, now = start;
    ecs_ftime_t delta_time = (ecs_ftime_t)ecs_time_measure(stop);

    if (ECS_EQZERO(world->info.target_fps)) {
        return delta_time;
    }

    ecs_os_perf_trace_push("flecs.insert_sleep");

    ecs_ftime_t target_delta_time =
        ((ecs_ftime_t)1.0 / (ecs_ftime_t)world->info.target_fps);

    /* Calculate the time we need to sleep by taking the measured delta from the
     * previous frame, and subtracting it from target_delta_time. */
    ecs_ftime_t sleep = target_delta_time - delta_time;

    /* Pick a sleep interval that is 4 times smaller than the time one frame
     * should take. */
    ecs_ftime_t sleep_time = sleep / (ecs_ftime_t)4.0;

    do {
        /* Only call sleep when sleep_time is not 0. On some platforms, even
         * a sleep with a timeout of 0 can cause stutter. */
        if (ECS_NEQZERO(sleep_time)) {
            ecs_sleepf((double)sleep_time);
        }

        now = start;
        delta_time = (ecs_ftime_t)ecs_time_measure(&now);
    } while ((target_delta_time - delta_time) >
        (sleep_time / (ecs_ftime_t)2.0));

    ecs_os_perf_trace_pop("flecs.insert_sleep");

    *stop = now;
    return delta_time;
}

static
ecs_ftime_t flecs_start_measure_frame(
    ecs_world_t *world,
    ecs_ftime_t user_delta_time)
{
    flecs_poly_assert(world, ecs_world_t);  

    ecs_ftime_t delta_time = 0;

    if ((world->flags & EcsWorldMeasureFrameTime) ||
        (ECS_EQZERO(user_delta_time)))
    {
        ecs_time_t t = world->frame_start_time;
        do {
            if (world->frame_start_time.nanosec || world->frame_start_time.sec){
                delta_time = flecs_insert_sleep(world, &t);
            } else {
                ecs_time_measure(&t);
                if (ECS_NEQZERO(world->info.target_fps)) {
                    delta_time = (ecs_ftime_t)1.0 / world->info.target_fps;
                } else {
                    /* Best guess */
                    delta_time = (ecs_ftime_t)1.0 / (ecs_ftime_t)60.0;

                    if (ECS_EQZERO(delta_time)) {
                        delta_time = user_delta_time;
                        break;
                    }
                }
            }

        /* Keep trying while delta_time is zero */
        } while (ECS_EQZERO(delta_time));

        world->frame_start_time = t;

        /* Keep track of total time passed in world */
        world->info.world_time_total_raw += (double)delta_time;
    }

    return (ecs_ftime_t)delta_time;
}

static
void flecs_stop_measure_frame(
    ecs_world_t* world)
{
    flecs_poly_assert(world, ecs_world_t);  

    if (world->flags & EcsWorldMeasureFrameTime) {
        ecs_time_t t = world->frame_start_time;
        world->info.frame_time_total += (ecs_ftime_t)ecs_time_measure(&t);
    }
}

ecs_ftime_t ecs_frame_begin(
    ecs_world_t *world,
    ecs_ftime_t user_delta_time)
{
    flecs_poly_assert(world, ecs_world_t);
    ecs_check(!(world->flags & EcsWorldReadonly), ECS_INVALID_OPERATION, 
        "cannot begin frame while world is in readonly mode");
    ecs_check(!(world->flags & EcsWorldFrameInProgress), ECS_INVALID_OPERATION, 
        "cannot begin frame while frame is already in progress");
    ecs_check(ECS_NEQZERO(user_delta_time) || ecs_os_has_time(), 
        ECS_MISSING_OS_API, "get_time");

    /* Start measuring total frame time */
    ecs_ftime_t delta_time = flecs_start_measure_frame(world, user_delta_time);
    if (ECS_EQZERO(user_delta_time)) {
        user_delta_time = delta_time;
    }

    world->info.delta_time_raw = user_delta_time;
    world->info.delta_time = user_delta_time * world->info.time_scale;

    /* Keep track of total scaled time passed in world */
    world->info.world_time_total += (double)world->info.delta_time;

    /* Command buffer capturing */
    world->on_commands_active = world->on_commands;
    world->on_commands = NULL;

    world->on_commands_ctx_active = world->on_commands_ctx;
    world->on_commands_ctx = NULL;

    ecs_run_aperiodic(world, 0);

    world->flags |= EcsWorldFrameInProgress;

    return world->info.delta_time;
error:
    return (ecs_ftime_t)0;
}

void ecs_frame_end(
    ecs_world_t *world)
{
    flecs_poly_assert(world, ecs_world_t);
    ecs_check(!(world->flags & EcsWorldReadonly), ECS_INVALID_OPERATION, 
        "cannot end frame while world is in readonly mode");
    ecs_check((world->flags & EcsWorldFrameInProgress), ECS_INVALID_OPERATION, 
        "cannot end frame while frame is not in progress");

    world->info.frame_count_total ++;
    
    int32_t i, count = world->stage_count;
    for (i = 0; i < count; i ++) {
        flecs_stage_merge_post_frame(world, world->stages[i]);
    }

    flecs_stop_measure_frame(world);

    /* Reset command handler each frame */
    world->on_commands_active = NULL;
    world->on_commands_ctx_active = NULL;

    world->flags &= ~EcsWorldFrameInProgress;
    
error:
    return;
}

#endif
