/**
 * @file addons/monitor.c
 * @brief Monitor addon.
 */

#include "flecs.h"
#include "../private_api.h"

#ifdef FLECS_MONITOR

ECS_COMPONENT_DECLARE(FlecsMonitor);
ECS_COMPONENT_DECLARE(EcsWorldStats);
ECS_COMPONENT_DECLARE(EcsPipelineStats);

ecs_entity_t EcsPeriod1s = 0;
ecs_entity_t EcsPeriod1m = 0;
ecs_entity_t EcsPeriod1h = 0;
ecs_entity_t EcsPeriod1d = 0;
ecs_entity_t EcsPeriod1w = 0;

static int32_t flecs_day_interval_count = 24;
static int32_t flecs_week_interval_count = 168;

static
ECS_COPY(EcsPipelineStats, dst, src, {
    (void)dst;
    (void)src;
    ecs_abort(ECS_INVALID_OPERATION, "cannot copy pipeline stats component");
})

static
ECS_MOVE(EcsPipelineStats, dst, src, {
    ecs_os_memcpy_t(dst, src, EcsPipelineStats);
    ecs_os_zeromem(src);
})

static
ECS_DTOR(EcsPipelineStats, ptr, {
    ecs_pipeline_stats_fini(&ptr->stats);
})

static
void MonitorStats(ecs_iter_t *it) {
    ecs_world_t *world = it->real_world;

    EcsStatsHeader *hdr = ecs_field_w_size(it, 0, 1);
    ecs_id_t kind = ecs_pair_first(it->world, ecs_field_id(it, 1));
    void *stats = ECS_OFFSET_T(hdr, EcsStatsHeader);

    ecs_ftime_t elapsed = hdr->elapsed;
    hdr->elapsed += it->delta_time;

    int32_t t_last = (int32_t)(elapsed * 60);
    int32_t t_next = (int32_t)(hdr->elapsed * 60);
    int32_t i, dif = t_last - t_next;

    ecs_world_stats_t last_world = {0};
    ecs_pipeline_stats_t last_pipeline = {0};
    void *last = NULL;

    if (!dif) {
        /* Copy last value so we can pass it to reduce_last */
        if (kind == ecs_id(EcsWorldStats)) {
            last = &last_world;
            ecs_world_stats_copy_last(&last_world, stats);
        } else if (kind == ecs_id(EcsPipelineStats)) {
            last = &last_pipeline;
            ecs_pipeline_stats_copy_last(&last_pipeline, stats);
        }
    }

    if (kind == ecs_id(EcsWorldStats)) {
        ecs_world_stats_get(world, stats);
    } else if (kind == ecs_id(EcsPipelineStats)) {
        ecs_pipeline_stats_get(world, ecs_get_pipeline(world), stats);
    }

    if (!dif) {
        /* Still in same interval, combine with last measurement */
        hdr->reduce_count ++;
        if (kind == ecs_id(EcsWorldStats)) {
            ecs_world_stats_reduce_last(stats, last, hdr->reduce_count);
        } else if (kind == ecs_id(EcsPipelineStats)) {
            ecs_pipeline_stats_reduce_last(stats, last, hdr->reduce_count);
        }
    } else if (dif > 1) {
        /* More than 16ms has passed, backfill */
        for (i = 1; i < dif; i ++) {
            if (kind == ecs_id(EcsWorldStats)) {
                ecs_world_stats_repeat_last(stats);
            } else if (kind == ecs_id(EcsPipelineStats)) {
                ecs_world_stats_repeat_last(stats);
            }
        }
        hdr->reduce_count = 0;
    }

    if (last && kind == ecs_id(EcsPipelineStats)) {
        ecs_pipeline_stats_fini(last);
    }
}

static
void ReduceStats(ecs_iter_t *it) {
    void *dst = ecs_field_w_size(it, 0, 1);
    void *src = ecs_field_w_size(it, 0, 2);

    ecs_id_t kind = ecs_pair_first(it->world, ecs_field_id(it, 1));

    dst = ECS_OFFSET_T(dst, EcsStatsHeader);
    src = ECS_OFFSET_T(src, EcsStatsHeader);

    if (kind == ecs_id(EcsWorldStats)) {
        ecs_world_stats_reduce(dst, src);
    } else if (kind == ecs_id(EcsPipelineStats)) {
        ecs_pipeline_stats_reduce(dst, src);
    }
}

static
void AggregateStats(ecs_iter_t *it) {
    int32_t interval = *(int32_t*)it->ctx;

    EcsStatsHeader *dst_hdr = ecs_field_w_size(it, 0, 1);
    EcsStatsHeader *src_hdr = ecs_field_w_size(it, 0, 2);

    void *dst = ECS_OFFSET_T(dst_hdr, EcsStatsHeader);
    void *src = ECS_OFFSET_T(src_hdr, EcsStatsHeader);

    ecs_id_t kind = ecs_pair_first(it->world, ecs_field_id(it, 1));

    ecs_world_stats_t last_world = {0};
    ecs_pipeline_stats_t last_pipeline = {0};
    void *last = NULL;

    if (dst_hdr->reduce_count != 0) {
        /* Copy last value so we can pass it to reduce_last */
        if (kind == ecs_id(EcsWorldStats)) {
            last_world.t = 0;
            ecs_world_stats_copy_last(&last_world, dst);
            last = &last_world;
        } else if (kind == ecs_id(EcsPipelineStats)) {
            last_pipeline.t = 0;
            ecs_pipeline_stats_copy_last(&last_pipeline, dst);
            last = &last_pipeline;
        }
    }

    /* Reduce from minutes to the current day */
    if (kind == ecs_id(EcsWorldStats)) {
        ecs_world_stats_reduce(dst, src);
    } else if (kind == ecs_id(EcsPipelineStats)) {
        ecs_pipeline_stats_reduce(dst, src);
    }

    if (dst_hdr->reduce_count != 0) {
        if (kind == ecs_id(EcsWorldStats)) {
            ecs_world_stats_reduce_last(dst, last, dst_hdr->reduce_count);
        } else if (kind == ecs_id(EcsPipelineStats)) {
            ecs_pipeline_stats_reduce_last(dst, last, dst_hdr->reduce_count);
        }
    }

    /* A day has 60 24 minute intervals */
    dst_hdr->reduce_count ++;
    if (dst_hdr->reduce_count >= interval) {
        dst_hdr->reduce_count = 0;
    }

    if (last && kind == ecs_id(EcsPipelineStats)) {
        ecs_pipeline_stats_fini(last);
    }
}

static
void flecs_stats_monitor_import(
    ecs_world_t *world,
    ecs_id_t kind,
    size_t size)
{
    ecs_entity_t prev = ecs_set_scope(world, kind);

    // Called each frame, collects 60 measurements per second
    ecs_system_init(world, &(ecs_system_desc_t){
        .entity = ecs_entity(world, { .name = "Monitor1s", .add = {ecs_dependson(EcsPreFrame)} }),
        .query.filter.terms = {{
            .id = ecs_pair(kind, EcsPeriod1s),
            .src.id = EcsWorld 
        }},
        .callback = MonitorStats
    });

    // Called each second, reduces into 60 measurements per minute
    ecs_entity_t mw1m = ecs_system_init(world, &(ecs_system_desc_t){
        .entity = ecs_entity(world, { .name = "Monitor1m", .add = {ecs_dependson(EcsPreFrame)} }),
        .query.filter.terms = {{
            .id = ecs_pair(kind, EcsPeriod1m),
            .src.id = EcsWorld 
        }, {
            .id = ecs_pair(kind, EcsPeriod1s),
            .src.id = EcsWorld 
        }},
        .callback = ReduceStats,
        .interval = 1.0
    });

    // Called each minute, reduces into 60 measurements per hour
    ecs_system_init(world, &(ecs_system_desc_t){
        .entity = ecs_entity(world, { .name = "Monitor1h", .add = {ecs_dependson(EcsPreFrame)} }),
        .query.filter.terms = {{
            .id = ecs_pair(kind, EcsPeriod1h),
            .src.id = EcsWorld 
        }, {
            .id = ecs_pair(kind, EcsPeriod1m),
            .src.id = EcsWorld 
        }},
        .callback = ReduceStats,
        .rate = 60,
        .tick_source = mw1m
    });

    // Called each minute, reduces into 60 measurements per day
    ecs_system_init(world, &(ecs_system_desc_t){
        .entity = ecs_entity(world, { .name = "Monitor1d", .add = {ecs_dependson(EcsPreFrame)} }),
        .query.filter.terms = {{
            .id = ecs_pair(kind, EcsPeriod1d),
            .src.id = EcsWorld 
        }, {
            .id = ecs_pair(kind, EcsPeriod1m),
            .src.id = EcsWorld 
        }},
        .callback = AggregateStats,
        .rate = 60,
        .tick_source = mw1m,
        .ctx = &flecs_day_interval_count
    });

    // Called each hour, reduces into 60 measurements per week
    ecs_system_init(world, &(ecs_system_desc_t){
        .entity = ecs_entity(world, { .name = "Monitor1w", .add = {ecs_dependson(EcsPreFrame)} }),
        .query.filter.terms = {{
            .id = ecs_pair(kind, EcsPeriod1w),
            .src.id = EcsWorld 
        }, {
            .id = ecs_pair(kind, EcsPeriod1h),
            .src.id = EcsWorld 
        }},
        .callback = AggregateStats,
        .rate = 60,
        .tick_source = mw1m,
        .ctx = &flecs_week_interval_count
    });

    ecs_set_scope(world, prev);

    ecs_set_id(world, EcsWorld, ecs_pair(kind, EcsPeriod1s), size, NULL);
    ecs_set_id(world, EcsWorld, ecs_pair(kind, EcsPeriod1m), size, NULL);
    ecs_set_id(world, EcsWorld, ecs_pair(kind, EcsPeriod1h), size, NULL);
    ecs_set_id(world, EcsWorld, ecs_pair(kind, EcsPeriod1d), size, NULL);
    ecs_set_id(world, EcsWorld, ecs_pair(kind, EcsPeriod1w), size, NULL);
}

static
void flecs_world_monitor_import(
    ecs_world_t *world)
{
    ECS_COMPONENT_DEFINE(world, EcsWorldStats);

    flecs_stats_monitor_import(world, ecs_id(EcsWorldStats), 
        sizeof(EcsWorldStats));
}

static
void flecs_pipeline_monitor_import(
    ecs_world_t *world)
{
    ECS_COMPONENT_DEFINE(world, EcsPipelineStats);

    ecs_set_hooks(world, EcsPipelineStats, {
        .ctor = ecs_default_ctor,
        .copy = ecs_copy(EcsPipelineStats),
        .move = ecs_move(EcsPipelineStats),
        .dtor = ecs_dtor(EcsPipelineStats)
    });

    flecs_stats_monitor_import(world, ecs_id(EcsPipelineStats),
        sizeof(EcsPipelineStats));
}

void FlecsMonitorImport(
    ecs_world_t *world)
{
    ECS_MODULE_DEFINE(world, FlecsMonitor);

    ecs_set_name_prefix(world, "Ecs");

    EcsPeriod1s = ecs_new_entity(world, "EcsPeriod1s");
    EcsPeriod1m = ecs_new_entity(world, "EcsPeriod1m");
    EcsPeriod1h = ecs_new_entity(world, "EcsPeriod1h");
    EcsPeriod1d = ecs_new_entity(world, "EcsPeriod1d");
    EcsPeriod1w = ecs_new_entity(world, "EcsPeriod1w");

    flecs_world_monitor_import(world);
    flecs_pipeline_monitor_import(world);
    
    if (ecs_os_has_time()) {
        ecs_measure_frame_time(world, true);
        ecs_measure_system_time(world, true);
    }
}

#endif
