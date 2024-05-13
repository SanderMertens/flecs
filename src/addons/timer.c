/**
 * @file addons/timer.c
 * @brief Timer addon.
 */

#include "flecs.h"
#include "system/system.h"
#include "../private_api.h"

#ifdef FLECS_TIMER

static
void AddTickSource(ecs_iter_t *it) {
    int32_t i;
    for (i = 0; i < it->count; i ++) {
        ecs_set(it->world, it->entities[i], EcsTickSource, {0});
    }
}

static
void ProgressTimers(ecs_iter_t *it) {
    EcsTimer *timer = ecs_field(it, EcsTimer, 0);
    EcsTickSource *tick_source = ecs_field(it, EcsTickSource, 1);

    ecs_assert(timer != NULL, ECS_INTERNAL_ERROR, NULL);

    int i;
    for (i = 0; i < it->count; i ++) {
        tick_source[i].tick = false;

        if (!timer[i].active) {
            continue;
        }

        const ecs_world_info_t *info = ecs_get_world_info(it->world);
        ecs_ftime_t time_elapsed = timer[i].time + info->delta_time_raw;
        ecs_ftime_t timeout = timer[i].timeout;
        
        if (time_elapsed >= timeout) {
            ecs_ftime_t t = time_elapsed - timeout;
            if (t > timeout) {
                t = 0;
            }

            timer[i].time = t; /* Initialize with remainder */
            tick_source[i].tick = true;
            tick_source[i].time_elapsed = time_elapsed - timer[i].overshoot;
            timer[i].overshoot = t;

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
    EcsRateFilter *filter = ecs_field(it, EcsRateFilter, 0);
    EcsTickSource *tick_dst = ecs_field(it, EcsTickSource, 1);

    int i;
    for (i = 0; i < it->count; i ++) {
        ecs_entity_t src = filter[i].src;
        bool inc = false;

        filter[i].time_elapsed += it->delta_time;

        if (src) {
            const EcsTickSource *tick_src = ecs_get(
                it->world, src, EcsTickSource);
            if (tick_src) {
                inc = tick_src->tick;
            } else {
                inc = true;
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

static
void ProgressTickSource(ecs_iter_t *it) {
    EcsTickSource *tick_src = ecs_field(it, EcsTickSource, 0);

    /* If tick source has no filters, tick unconditionally */
    int i;
    for (i = 0; i < it->count; i ++) {
        tick_src[i].tick = true;
        tick_src[i].time_elapsed = it->delta_time;
    }
}

ecs_entity_t ecs_set_timeout(
    ecs_world_t *world,
    ecs_entity_t timer,
    ecs_ftime_t timeout)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);

    if (!timer) {
        timer = ecs_entity(world, {0});
    }

    ecs_set(world, timer, EcsTimer, {
        .timeout = timeout,
        .single_shot = true,
        .active = true
    });

    ecs_system_t *system_data = flecs_poly_get(world, timer, ecs_system_t);
    if (system_data) {
        system_data->tick_source = timer;
    }

error:
    return timer;
}

ecs_ftime_t ecs_get_timeout(
    const ecs_world_t *world,
    ecs_entity_t timer)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(timer != 0, ECS_INVALID_PARAMETER, NULL);

    const EcsTimer *value = ecs_get(world, timer, EcsTimer);
    if (value) {
        return value->timeout;
    }
error:
    return 0;
}

ecs_entity_t ecs_set_interval(
    ecs_world_t *world,
    ecs_entity_t timer,
    ecs_ftime_t interval)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);

    if (!timer) {
        timer = ecs_new_w(world, EcsTimer);
    }

    EcsTimer *t = ecs_ensure(world, timer, EcsTimer);
    ecs_check(t != NULL, ECS_INTERNAL_ERROR, NULL);
    t->timeout = interval;
    t->active = true;
    ecs_modified(world, timer, EcsTimer);

    ecs_system_t *system_data = flecs_poly_get(world, timer, ecs_system_t);
    if (system_data) {
        system_data->tick_source = timer;
    }
error:
    return timer;  
}

ecs_ftime_t ecs_get_interval(
    const ecs_world_t *world,
    ecs_entity_t timer)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);

    if (!timer) {
        return 0;
    }

    const EcsTimer *value = ecs_get(world, timer, EcsTimer);
    if (value) {
        return value->timeout;
    }
error:
    return 0;
}

void ecs_start_timer(
    ecs_world_t *world,
    ecs_entity_t timer)
{
    EcsTimer *ptr = ecs_ensure(world, timer, EcsTimer);
    ecs_check(ptr != NULL, ECS_INTERNAL_ERROR, NULL);
    ptr->active = true;
    ptr->time = 0;
error:
    return;
}

void ecs_stop_timer(
    ecs_world_t *world,
    ecs_entity_t timer)
{
    EcsTimer *ptr = ecs_ensure(world, timer, EcsTimer);
    ecs_check(ptr != NULL, ECS_INTERNAL_ERROR, NULL);
    ptr->active = false;
error:
    return;
}

void ecs_reset_timer(
    ecs_world_t *world,
    ecs_entity_t timer)
{
    EcsTimer *ptr = ecs_ensure(world, timer, EcsTimer);
    ecs_check(ptr != NULL, ECS_INTERNAL_ERROR, NULL);
    ptr->time = 0;
error:
    return;   
}

ecs_entity_t ecs_set_rate(
    ecs_world_t *world,
    ecs_entity_t filter,
    int32_t rate,
    ecs_entity_t source)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);

    if (!filter) {
        filter = ecs_entity(world, {0});
    }

    ecs_set(world, filter, EcsRateFilter, {
        .rate = rate,
        .src = source
    });

    ecs_system_t *system_data = flecs_poly_get(world, filter, ecs_system_t);
    if (system_data) {
        system_data->tick_source = filter;
    }  

error:
    return filter;     
}

void ecs_set_tick_source(
    ecs_world_t *world,
    ecs_entity_t system,
    ecs_entity_t tick_source)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(system != 0, ECS_INVALID_PARAMETER, NULL);
    ecs_check(tick_source != 0, ECS_INVALID_PARAMETER, NULL);

    ecs_system_t *system_data = flecs_poly_get(world, system, ecs_system_t);
    ecs_check(system_data != NULL, ECS_INVALID_PARAMETER, NULL);

    system_data->tick_source = tick_source;
error:
    return;
}

static
void RandomizeTimers(ecs_iter_t *it) {
    EcsTimer *timer = ecs_field(it, EcsTimer, 0);
    int32_t i;
    for (i = 0; i < it->count; i ++) {
        timer[i].time = 
            ((ecs_ftime_t)rand() / (ecs_ftime_t)RAND_MAX) * timer[i].timeout;
    }
}

void ecs_randomize_timers(
    ecs_world_t *world)
{
    ecs_observer(world, {
        .entity = ecs_entity(world, { .name = "flecs.timer.RandomizeTimers" }),
        .query.terms = {{
            .id = ecs_id(EcsTimer)
        }},
        .events = {EcsOnSet},
        .yield_existing = true,
        .callback = RandomizeTimers
    });
}

void FlecsTimerImport(
    ecs_world_t *world)
{    
    ECS_MODULE(world, FlecsTimer);
    ECS_IMPORT(world, FlecsPipeline);
#ifdef FLECS_DOC
    ECS_IMPORT(world, FlecsDoc);
    ecs_doc_set_brief(world, ecs_id(FlecsTimer), 
        "Module that implements system timers (used by .interval)");
#endif

    ecs_set_name_prefix(world, "Ecs");

    flecs_bootstrap_component(world, EcsTimer);
    flecs_bootstrap_component(world, EcsRateFilter);

    ecs_set_hooks(world, EcsTimer, {
        .ctor = flecs_default_ctor
    });

    /* Add EcsTickSource to timers and rate filters */
    ecs_system(world, {
        .entity = ecs_entity(world, {.name = "AddTickSource", .add = ecs_ids( ecs_dependson(EcsPreFrame) )}),
        .query.terms = {
            { .id = ecs_id(EcsTimer), .oper = EcsOr },
            { .id = ecs_id(EcsRateFilter), .oper = EcsAnd },
            { .id = ecs_id(EcsTickSource), .oper = EcsNot, .inout = EcsOut}
        },
        .callback = AddTickSource
    });

    /* Timer handling */
    ecs_system(world, {
        .entity = ecs_entity(world, {.name = "ProgressTimers", .add = ecs_ids( ecs_dependson(EcsPreFrame))}),
        .query.terms = {
            { .id = ecs_id(EcsTimer) },
            { .id = ecs_id(EcsTickSource) }
        },
        .callback = ProgressTimers
    });

    /* Rate filter handling */
    ecs_system(world, {
        .entity = ecs_entity(world, {.name = "ProgressRateFilters", .add = ecs_ids( ecs_dependson(EcsPreFrame))}),
        .query.terms = {
            { .id = ecs_id(EcsRateFilter), .inout = EcsIn },
            { .id = ecs_id(EcsTickSource), .inout = EcsOut }
        },
        .callback = ProgressRateFilters
    });

    /* TickSource without a timer or rate filter just increases each frame */
    ecs_system(world, {
        .entity = ecs_entity(world, { .name = "ProgressTickSource", .add = ecs_ids( ecs_dependson(EcsPreFrame))}),
        .query.terms = {
            { .id = ecs_id(EcsTickSource), .inout = EcsOut },
            { .id = ecs_id(EcsRateFilter), .oper = EcsNot },
            { .id = ecs_id(EcsTimer), .oper = EcsNot }
        },
        .callback = ProgressTickSource
    });
}

#endif
