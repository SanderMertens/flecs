/**
 * @file addons/monitor.c
 * @brief Stats addon module.
 */

#include "flecs.h"
#include "stats.h"

#ifdef FLECS_STATS

ECS_COMPONENT_DECLARE(FlecsStats);

ecs_entity_t EcsPeriod1s = 0;
ecs_entity_t EcsPeriod1m = 0;
ecs_entity_t EcsPeriod1h = 0;
ecs_entity_t EcsPeriod1d = 0;
ecs_entity_t EcsPeriod1w = 0;

#define FlecsDayIntervalCount (24)
#define FlecsWeekIntervalCount (168)

typedef struct {
    ecs_stats_api_t api;
    ecs_query_t *query;
} ecs_monitor_stats_ctx_t;

typedef struct {
    ecs_stats_api_t api;
} ecs_reduce_stats_ctx_t;

typedef struct {
    ecs_stats_api_t api;
    int32_t interval;
} ecs_aggregate_stats_ctx_t;

static
void MonitorStats(ecs_iter_t *it) {
    ecs_world_t *world = it->real_world;
    ecs_monitor_stats_ctx_t *ctx = it->ctx;

    EcsStatsHeader *hdr = ecs_field_w_size(it, 0, 0);

    ecs_ftime_t elapsed = hdr->elapsed;
    hdr->elapsed += it->delta_time;

    int32_t t_last = (int32_t)(elapsed * 60);
    int32_t t_next = (int32_t)(hdr->elapsed * 60);
    int32_t i, dif = t_next - t_last;
    void *stats_storage = ecs_os_alloca(ctx->api.stats_size);
    void *last = NULL;

    if (!dif) {
        hdr->reduce_count ++;
    }

    ecs_iter_t qit;
    int32_t cur = -1, count = 0;
    void *stats = NULL;
    ecs_map_t *stats_map = NULL;

    if (ctx->query) {
        /* Query results are stored in a map */
        qit = ecs_query_iter(it->world, ctx->query);
        stats_map = ECS_OFFSET_T(hdr, EcsStatsHeader);
    } else {
        /* No query, so tracking stats for single element */
        stats = ECS_OFFSET_T(hdr, EcsStatsHeader);
    }

    do {
        ecs_entity_t res = 0;
        if (ctx->query) {
            /* Query, fetch resource entity & stats pointer */
            if (cur == (count - 1)) {
                if (!ecs_query_next(&qit)) {
                    break;
                }

                cur = 0;
                count = qit.count;
            } else {
                cur ++;
            }

            res = qit.entities[cur];
            stats = ecs_map_ensure_alloc(stats_map, ctx->api.stats_size, res);
            ctx->api.set_t(stats, t_last % ECS_STAT_WINDOW);
        }

        if (!dif) {
            /* Copy last value so we can pass it to reduce_last */
            last = stats_storage;
            ecs_os_memset(last, 0, ctx->api.stats_size);
            ctx->api.copy_last(last, stats);
        }

        ctx->api.get(world, res, stats);

        if (!dif) {
            /* Still in same interval, combine with last measurement */
            ctx->api.reduce_last(stats, last, hdr->reduce_count);
        } else if (dif > 1) {
            /* More than 16ms has passed, backfill */
            for (i = 1; i < dif; i ++) {
                ctx->api.repeat_last(stats);
            }
        }

        if (last && ctx->api.fini) {
            ctx->api.fini(last);
        }

        if (!ctx->query) {
            break;
        }
    } while (true);

     if (dif > 1) {
        hdr->reduce_count = 0;
    }
}

static
void ReduceStats(ecs_iter_t *it) {
    ecs_reduce_stats_ctx_t *ctx = it->ctx;

    void *dst = ecs_field_w_size(it, 0, 0);
    void *src = ecs_field_w_size(it, 0, 1);

    dst = ECS_OFFSET_T(dst, EcsStatsHeader);
    src = ECS_OFFSET_T(src, EcsStatsHeader);

    if (!ctx->api.query_component_id) {
        ctx->api.reduce(dst, src);
    } else {
        ecs_map_iter_t mit = ecs_map_iter(src);
        while (ecs_map_next(&mit)) {
            void *src_el = ecs_map_ptr(&mit);
            void *dst_el = ecs_map_ensure_alloc(
                dst, ctx->api.stats_size, ecs_map_key(&mit));
            ctx->api.reduce(dst_el, src_el);
        }
    }
}

static
void AggregateStats(ecs_iter_t *it) {
    ecs_aggregate_stats_ctx_t *ctx = it->ctx;
    int32_t interval = ctx->interval;

    EcsStatsHeader *dst_hdr = ecs_field_w_size(it, 0, 0);
    EcsStatsHeader *src_hdr = ecs_field_w_size(it, 0, 1);

    void *dst = ECS_OFFSET_T(dst_hdr, EcsStatsHeader);
    void *src = ECS_OFFSET_T(src_hdr, EcsStatsHeader);
    void *dst_map = NULL;
    void *src_map = NULL;
    if (ctx->api.query_component_id) {
        dst_map = dst;
        src_map = src;
        dst = NULL;
        src = NULL;
    }

    void *stats_storage = ecs_os_alloca(ctx->api.stats_size);
    void *last = NULL;

    ecs_map_iter_t mit;
    if (src_map) {
        mit = ecs_map_iter(src_map);
    }

    do {
        if (src_map) {
            if (!ecs_map_next(&mit)) {
                break;
            }

            src = ecs_map_ptr(&mit);
            dst = ecs_map_ensure_alloc(
                dst_map, ctx->api.stats_size, ecs_map_key(&mit));
        }

        if (dst_hdr->reduce_count != 0) {
            /* Copy last value so we can pass it to reduce_last */
            last = stats_storage;
            ecs_os_memset(last, 0, ctx->api.stats_size);
            ctx->api.copy_last(last, dst);
        }

        /* Reduce from minutes to the current day */
        ctx->api.reduce(dst, src);

        if (dst_hdr->reduce_count != 0) {
            ctx->api.reduce_last(dst, last, dst_hdr->reduce_count);
        }

        if (last && ctx->api.fini != NULL) {
            ctx->api.fini(last);
        }

        if (!src_map) {
            break;
        }
    } while (true);

    /* A day has 60 24 minute intervals */
    dst_hdr->reduce_count ++;
    if (dst_hdr->reduce_count >= interval) {
        dst_hdr->reduce_count = 0;
    }
}

static
void flecs_monitor_ctx_free(
    void *ptr)
{
    ecs_monitor_stats_ctx_t *ctx = ptr;
    if (ctx->query) {
        ecs_query_fini(ctx->query);
    }
    ecs_os_free(ctx);
}

static
void flecs_reduce_ctx_free(
    void *ptr)
{
    ecs_os_free(ptr);
}

static
void flecs_aggregate_ctx_free(
    void *ptr)
{
    ecs_os_free(ptr);
}

void flecs_stats_api_import(
    ecs_world_t *world,
    ecs_stats_api_t *api)
{
    ecs_entity_t kind = api->monitor_component_id;
    ecs_entity_t prev = ecs_set_scope(world, kind);

    ecs_query_t *q = NULL;
    if (api->query_component_id) {
        q = ecs_query(world, {
            .terms = {{ .id = api->query_component_id }},
            .cache_kind = EcsQueryCacheNone,
            .flags = EcsQueryMatchDisabled
        });
    }

    // Called each frame, collects 60 measurements per second
    {
        ecs_monitor_stats_ctx_t *ctx = ecs_os_calloc_t(ecs_monitor_stats_ctx_t);
        ctx->api = *api;
        ctx->query = q;

        ecs_system(world, {
            .entity = ecs_entity(world, { .name = "Monitor1s", .add = ecs_ids(ecs_dependson(EcsPreFrame)) }),
            .query.terms = {{
                .id = ecs_pair(kind, EcsPeriod1s),
                .src.id = EcsWorld 
            }},
            .callback = MonitorStats,
            .ctx = ctx,
            .ctx_free = flecs_monitor_ctx_free
        });
    }

    // Called each second, reduces into 60 measurements per minute
    ecs_entity_t mw1m;
    {
        ecs_reduce_stats_ctx_t *ctx = ecs_os_calloc_t(ecs_reduce_stats_ctx_t);
        ctx->api = *api;

        mw1m = ecs_system(world, {
            .entity = ecs_entity(world, { .name = "Monitor1m", .add = ecs_ids(ecs_dependson(EcsPreFrame)) }),
            .query.terms = {{
                .id = ecs_pair(kind, EcsPeriod1m),
                .src.id = EcsWorld 
            }, {
                .id = ecs_pair(kind, EcsPeriod1s),
                .src.id = EcsWorld 
            }},
            .callback = ReduceStats,
            .interval = 1.0,
            .ctx = ctx,
            .ctx_free = flecs_reduce_ctx_free
        });
    }

    // Called each minute, reduces into 60 measurements per hour
    {
        ecs_reduce_stats_ctx_t *ctx = ecs_os_calloc_t(ecs_reduce_stats_ctx_t);
        ctx->api = *api;

        ecs_system(world, {
            .entity = ecs_entity(world, { .name = "Monitor1h", .add = ecs_ids(ecs_dependson(EcsPreFrame)) }),
            .query.terms = {{
                .id = ecs_pair(kind, EcsPeriod1h),
                .src.id = EcsWorld 
            }, {
                .id = ecs_pair(kind, EcsPeriod1m),
                .src.id = EcsWorld 
            }},
            .callback = ReduceStats,
            .rate = 60,
            .tick_source = mw1m,
            .ctx = ctx,
            .ctx_free = flecs_reduce_ctx_free
        });
    }

    // Called each minute, reduces into 60 measurements per day
    {
        ecs_aggregate_stats_ctx_t *ctx = ecs_os_calloc_t(ecs_aggregate_stats_ctx_t);
        ctx->api = *api;
        ctx->interval = FlecsDayIntervalCount;

        ecs_system(world, {
            .entity = ecs_entity(world, { .name = "Monitor1d", .add = ecs_ids(ecs_dependson(EcsPreFrame)) }),
            .query.terms = {{
                .id = ecs_pair(kind, EcsPeriod1d),
                .src.id = EcsWorld 
            }, {
                .id = ecs_pair(kind, EcsPeriod1m),
                .src.id = EcsWorld 
            }},
            .callback = AggregateStats,
            .rate = 60,
            .tick_source = mw1m,
            .ctx = ctx,
            .ctx_free = flecs_aggregate_ctx_free
        });
    }

    // Called each hour, reduces into 60 measurements per week
    {
        ecs_aggregate_stats_ctx_t *ctx = ecs_os_calloc_t(ecs_aggregate_stats_ctx_t);
        ctx->api = *api;
        ctx->interval = FlecsWeekIntervalCount;

        ecs_system(world, {
            .entity = ecs_entity(world, { .name = "Monitor1w", .add = ecs_ids(ecs_dependson(EcsPreFrame)) }),
            .query.terms = {{
                .id = ecs_pair(kind, EcsPeriod1w),
                .src.id = EcsWorld 
            }, {
                .id = ecs_pair(kind, EcsPeriod1h),
                .src.id = EcsWorld 
            }},
            .callback = AggregateStats,
            .rate = 60,
            .tick_source = mw1m,
            .ctx = ctx,
            .ctx_free = flecs_aggregate_ctx_free
        });
    }

    ecs_set_scope(world, prev);

    ecs_add_pair(world, EcsWorld, kind, EcsPeriod1s);
    ecs_add_pair(world, EcsWorld, kind, EcsPeriod1m);
    ecs_add_pair(world, EcsWorld, kind, EcsPeriod1h);
    ecs_add_pair(world, EcsWorld, kind, EcsPeriod1d);
    ecs_add_pair(world, EcsWorld, kind, EcsPeriod1w);
}

void FlecsStatsImport(
    ecs_world_t *world)
{
    ECS_MODULE_DEFINE(world, FlecsStats);
    ECS_IMPORT(world, FlecsPipeline);
    ECS_IMPORT(world, FlecsTimer);
#ifdef FLECS_META
    ECS_IMPORT(world, FlecsMeta);
#endif
#ifdef FLECS_UNITS
    ECS_IMPORT(world, FlecsUnits);
#endif
#ifdef FLECS_DOC
    ECS_IMPORT(world, FlecsDoc);
    ecs_doc_set_brief(world, ecs_id(FlecsStats), 
        "Module that automatically monitors statistics for the world & systems");
#endif

    ecs_set_name_prefix(world, "Ecs");

    EcsPeriod1s = ecs_entity(world, { .name = "EcsPeriod1s" });
    EcsPeriod1m = ecs_entity(world, { .name = "EcsPeriod1m" });
    EcsPeriod1h = ecs_entity(world, { .name = "EcsPeriod1h" });
    EcsPeriod1d = ecs_entity(world, { .name = "EcsPeriod1d" });
    EcsPeriod1w = ecs_entity(world, { .name = "EcsPeriod1w" });

    FlecsWorldSummaryImport(world);
    FlecsWorldMonitorImport(world);
    FlecsSystemMonitorImport(world);
    FlecsPipelineMonitorImport(world);
    
    if (ecs_os_has_time()) {
        ecs_measure_frame_time(world, true);
        ecs_measure_system_time(world, true);
    }
}

#endif
