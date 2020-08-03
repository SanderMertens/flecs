#include "flecs.h"

#ifdef FLECS_TIMER

#include "../private_api.h"
#include "system/system.h"

ecs_type_t ecs_type(EcsTimer);
ecs_type_t ecs_type(EcsRateFilter);

static
void AddTickSource(ecs_iter_t *it) {
    int32_t i;
    for (i = 0; i < it->count; i ++) {
        ecs_set(it->world, it->entities[i], EcsTickSource, {0});
    }
}

static
void ProgressTimers(ecs_iter_t *it) {
    EcsTimer *timer = ecs_column(it, EcsTimer, 1);
    EcsTickSource *tick_source = ecs_column(it, EcsTickSource, 2);

    ecs_assert(timer != NULL, ECS_INTERNAL_ERROR, NULL);

    int i;
    for (i = 0; i < it->count; i ++) {
        tick_source[i].tick = false;

        if (!timer[i].active) {
            continue;
        }

        float time_elapsed = timer[i].time + it->world->stats.delta_time_raw;
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

static
void ProgressRateFilters(ecs_iter_t *it) {
    EcsRateFilter *filter = ecs_column(it, EcsRateFilter, 1);
    EcsTickSource *tick_dst = ecs_column(it, EcsTickSource, 2);

    int i;
    for (i = 0; i < it->count; i ++) {
        ecs_entity_t src = filter[i].src;
        bool inc = false;

        filter[i].time_elapsed += it->delta_time;

        if (src) {
            const EcsTickSource *tick_src = ecs_get(it->world, src, EcsTickSource);
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

    EcsSystem *system_data = ecs_get_mut(world, timer, EcsSystem, NULL);
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

    EcsTimer *value = ecs_get_mut(world, timer, EcsTimer, NULL);
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

    EcsSystem *system_data = ecs_get_mut(world, timer, EcsSystem, NULL);
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

    EcsTimer *value = ecs_get_mut(world, timer, EcsTimer, NULL);
    if (value) {
        return value->timeout;
    } else {
        return 0;
    }
}

void ecs_start_timer(
    ecs_world_t *world,
    ecs_entity_t timer)
{
    EcsTimer *ptr = ecs_get_mut(world, timer, EcsTimer, NULL);
    ecs_assert(ptr != NULL, ECS_INVALID_PARAMETER, NULL);

    ptr->active = true;
    ptr->time = 0;
}

void ecs_stop_timer(
    ecs_world_t *world,
    ecs_entity_t timer)
{
    EcsTimer *ptr = ecs_get_mut(world, timer, EcsTimer, NULL);
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

    EcsSystem *system_data = ecs_get_mut(world, filter, EcsSystem, NULL);
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

    EcsSystem *system_data = ecs_get_mut(world, system, EcsSystem, NULL);
    ecs_assert(system_data != NULL, ECS_INVALID_PARAMETER, NULL);

    system_data->tick_source = tick_source;
}

void FlecsTimerImport(
    ecs_world_t *world)
{    
    ECS_MODULE(world, FlecsTimer);

    ECS_IMPORT(world, FlecsPipeline);

    ecs_set_name_prefix(world, "Ecs");

    ecs_bootstrap_component(world, EcsTimer);
    ecs_bootstrap_component(world, EcsRateFilter);

    /* Add EcsTickSource to timers and rate filters */
    ECS_SYSTEM(world, AddTickSource, EcsPreFrame, [in] Timer || RateFilter, [out] !flecs.system.TickSource);

    /* Timer handling */
    ECS_SYSTEM(world, ProgressTimers, EcsPreFrame, Timer, flecs.system.TickSource);

    /* Rate filter handling */
    ECS_SYSTEM(world, ProgressRateFilters, EcsPreFrame, [in] RateFilter, [out] flecs.system.TickSource);
}

#endif
