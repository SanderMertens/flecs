#include "flecs_private.h"

/* Auto-add EcsTickSource to each entity with EcsTimer or EcsRateFilter */
void EcsAddTickSource(ecs_rows_t *rows) {
    int32_t i;
    for (i = 0; i < rows->count; i ++) {
        ecs_set(rows->world, rows->entities[i], EcsTickSource, {0});
    }
}

void EcsProgressTimers(ecs_rows_t *rows) {
    ECS_COLUMN(rows, EcsTimer, timer, 1);
    ECS_COLUMN(rows, EcsTickSource, tick_source, 2);

    ecs_assert(timer != NULL, ECS_INTERNAL_ERROR, NULL);

    int i;
    for (i = 0; i < rows->count; i ++) {
        tick_source[i].tick = false;

        if (!timer[i].active) {
            continue;
        }

        float time_elapsed = timer[i].time + rows->delta_time;
        float timeout = timer[i].timeout;
        
        if (time_elapsed >= timeout) {
            float t = time_elapsed - timeout;
            if (t > timeout) {
                t = 0;
            }

            timer[i].time = t; /* Initialize with remainder */
            
            tick_source[i].tick = true;
            tick_source[i].time_elapsed = time_elapsed;

            if (timer[i].single_shot) {
                timer[i].active = false;
            }
        } else {
            timer[i].time = time_elapsed;
        }  
    }
}

void EcsProgressRateFilters(ecs_rows_t *rows) {
    ECS_COLUMN(rows, EcsRateFilter, filter, 1);
    ECS_COLUMN(rows, EcsTickSource, tick_dst, 2);

    int i;
    for (i = 0; i < rows->count; i ++) {
        ecs_entity_t src = filter[i].src;
        bool inc = false;

        filter[i].time_elapsed += rows->delta_time;

        if (src) {
            EcsTickSource *tick_src = ecs_get_ptr(rows->world, src, EcsTickSource);
            if (tick_src) {
                inc = tick_src->tick;
            }
        } else {
            inc = true;
        }

        if (inc) {
            filter[i].tick_count ++;

            bool triggered = !(filter[i].tick_count % filter[i].rate);
            tick_dst[i].tick = triggered;
            tick_dst[i].time_elapsed = filter[i].time_elapsed;

            if (triggered) {
                filter[i].time_elapsed = 0;
            }            
        } else {
            tick_dst[i].tick = false;
        }
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
        .single_shot = true,
        .active = true
    });

    EcsColSystem *system_data = ecs_get_ptr(world, timer, EcsColSystem);
    if (system_data) {
        system_data->tick_source = timer;
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
        .timeout = interval,
        .active = true
    });

    EcsColSystem *system_data = ecs_get_ptr(world, timer, EcsColSystem);
    if (system_data) {
        system_data->tick_source = timer;
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

void ecs_start_timer(
    ecs_world_t *world,
    ecs_entity_t timer)
{
    EcsTimer *ptr = ecs_get_ptr(world, timer, EcsTimer);
    ecs_assert(ptr != NULL, ECS_INVALID_PARAMETER, NULL);

    ptr->active = true;
    ptr->time = 0;
}

void ecs_stop_timer(
    ecs_world_t *world,
    ecs_entity_t timer)
{
    EcsTimer *ptr = ecs_get_ptr(world, timer, EcsTimer);
    ecs_assert(ptr != NULL, ECS_INVALID_PARAMETER, NULL);

    ptr->active = false;
}

ecs_entity_t ecs_set_rate_filter(
    ecs_world_t *world,
    ecs_entity_t filter,
    int32_t rate,
    ecs_entity_t source)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);

    filter = ecs_set(world, filter, EcsRateFilter, {
        .rate = rate,
        .src = source
    });

    EcsColSystem *system_data = ecs_get_ptr(world, filter, EcsColSystem);
    if (system_data) {
        system_data->tick_source = filter;
    }  

    return filter;     
}

void ecs_set_tick_source(
    ecs_world_t *world,
    ecs_entity_t system,
    ecs_entity_t tick_source)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(system != 0, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(tick_source != 0, ECS_INVALID_PARAMETER, NULL);

    EcsColSystem *system_data = ecs_get_ptr(world, system, EcsColSystem);
    ecs_assert(system_data != NULL, ECS_INVALID_PARAMETER, NULL);

    system_data->tick_source = tick_source;
}
