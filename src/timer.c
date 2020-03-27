#include "flecs_private.h"

void EcsProgressTimers(ecs_rows_t *rows) {
    ECS_COLUMN(rows, EcsTimer, timer, 1);

    ecs_assert(timer != NULL, ECS_INTERNAL_ERROR, NULL);

    int i;
    for (i = 0; i < rows->count; i ++) {
        float time_elapsed = timer[i].time + rows->delta_time;
        float timeout = timer[i].timeout;
        
        if (time_elapsed >= timeout) {
            int32_t fired_count = timer[i].fired_count;
            
            if (!timer[i].single_shot || !fired_count) {
                float t = time_elapsed - timeout;
                if (t > timeout) {
                    t = 0;
                }

                timer[i].time = t; /* Initialize with remainder */
                timer[i].time_elapsed = time_elapsed;
                timer[i].fired_count = fired_count + 1;
            }
        } else {
            timer[i].time = time_elapsed;
            timer[i].time_elapsed = 0;
        }     
    }
}

void EcsClearTimers(ecs_rows_t *rows) {
    ECS_COLUMN(rows, EcsTimer, timer, 1);

    ecs_assert(timer != NULL, ECS_INTERNAL_ERROR, NULL);

    int i;
    for (i = 0; i < rows->count; i ++) {
        timer[i].time_elapsed = 0;
    }
}

ecs_entity_t ecs_set_timeout(
    ecs_world_t *world,
    ecs_entity_t timer,
    float timeout)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);

    timer = ecs_set(world, timer, EcsTimer, {
        .timeout = timeout,
        .single_shot = true
    });

    EcsColSystem *system_data = ecs_get_ptr(world, timer, EcsColSystem);
    if (system_data) {
        system_data->timer = timer;
    }

    return timer;
}

float ecs_get_timeout(
    ecs_world_t *world,
    ecs_entity_t timer)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(timer != 0, ECS_INVALID_PARAMETER, NULL);

    EcsTimer *value = ecs_get_ptr(world, timer, EcsTimer);
    if (value) {
        return value->timeout;
    } else {
        return 0;
    }
}

ecs_entity_t ecs_set_interval(
    ecs_world_t *world,
    ecs_entity_t timer,
    float interval)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);

    timer = ecs_set(world, timer, EcsTimer, {
        .timeout = interval
    });

    EcsColSystem *system_data = ecs_get_ptr(world, timer, EcsColSystem);
    if (system_data) {
        system_data->timer = timer;
    }  

    return timer;  
}

float ecs_get_interval(
    ecs_world_t *world,
    ecs_entity_t timer)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);

    if (!timer) {
        return 0;
    }

    EcsTimer *value = ecs_get_ptr(world, timer, EcsTimer);
    if (value) {
        return value->timeout;
    } else {
        return 0;
    }
}

void ecs_set_timer(
    ecs_world_t *world,
    ecs_entity_t system,
    ecs_entity_t timer)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(system != 0, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(timer != 0, ECS_INVALID_PARAMETER, NULL);

    EcsColSystem *system_data = ecs_get_ptr(world, system, EcsColSystem);
    ecs_assert(system_data != NULL, ECS_INVALID_PARAMETER, NULL);

    system_data->timer = timer;
}

/* Deprecated functions */
void ecs_set_period(
    ecs_world_t *world,
    ecs_entity_t system,
    float period)
{
    ecs_set_interval(world, system, period);
}

float ecs_get_period(
    ecs_world_t *world,
    ecs_entity_t system)
{
    return ecs_get_interval(world, system);
}
