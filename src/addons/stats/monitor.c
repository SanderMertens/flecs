/**
 * @file addons/stats/monitor.c
 * @brief Stats addon module.
 */

#include "flecs.h"

#ifdef FLECS_STATS

#include "stats.h"

ECS_COMPONENT_DECLARE(FlecsStats);

ecs_entity_t EcsPeriod1s = 0;
ecs_entity_t EcsPeriod1m = 0;
ecs_entity_t EcsPeriod1h = 0;
ecs_entity_t EcsPeriod1d = 0;
ecs_entity_t EcsPeriod1w = 0;

typedef struct {
    ecs_stats_api_t api;
    ecs_query_t *query;
    int32_t interval;
} ecs_monitor_stats_ctx_t;

static void MonitorStats(ecs_iter_t *it) {
    ecs_world_t *world = it->real_world;
    ecs_monitor_stats_ctx_t *ctx = it->ctx;

    EcsStatsHeader *hdr = ecs_field_w_size(it, ecs_field_size(it, 0), 0);

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
        if (ctx->api.is_pointer) {
            stats = *((void**)stats);
        }
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
                if (!count) {
                    cur = -1;
                    continue;
                }
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
            /* More than one interval has passed, backfill */
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

static void ReduceStats(ecs_iter_t *it) {
    ecs_monitor_stats_ctx_t *ctx = it->ctx;
    int32_t interval = ctx->interval;

    EcsStatsHeader *dst_hdr = ecs_field_w_size(it, ecs_field_size(it, 0), 0);
    EcsStatsHeader *src_hdr = ecs_field_w_size(it, ecs_field_size(it, 1), 1);

    void *dst = ECS_OFFSET_T(dst_hdr, EcsStatsHeader);
    void *src = ECS_OFFSET_T(src_hdr, EcsStatsHeader);
    void *dst_map = NULL;
    void *src_map = NULL;
    if (ctx->api.query_component_id) {
        dst_map = dst;
        src_map = src;
        dst = NULL;
        src = NULL;
    } else {
        if (ctx->api.is_pointer) {
            dst = *((void**)dst);
            src = *((void**)src);
        }
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

        /* Reduce stats into the current aggregation interval */
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

    /* Increment reduce count, reset when interval is reached */
    dst_hdr->reduce_count ++;
    if (dst_hdr->reduce_count >= interval) {
        dst_hdr->reduce_count = 0;
    }
}

static void flecs_monitor_ctx_free(
    void *ptr)
{
    ecs_monitor_stats_ctx_t *ctx = ptr;
    if (ctx->query) {
        ecs_query_fini(ctx->query);
    }
    ecs_os_free(ctx);
}

void flecs_stats_api_import(
    ecs_world_t *world,
    ecs_stats_api_t *api)
{
    ecs_entity_t kind = api->monitor_component_id;
    ecs_entity_t prev = ecs_set_scope(world, kind);
    struct {
        const char *name;
        ecs_entity_t period;
        ecs_entity_t source;
        int32_t interval;
    } periods[] = {
        {"Monitor1s", EcsPeriod1s, 0, 0},
        {"Monitor1m", EcsPeriod1m, EcsPeriod1s, 1},
        {"Monitor1h", EcsPeriod1h, EcsPeriod1m, 1},
        {"Monitor1d", EcsPeriod1d, EcsPeriod1m, 24},
        {"Monitor1w", EcsPeriod1w, EcsPeriod1h, 168}
    };
    ecs_entity_t minute = 0;
    for (int32_t i = 0; i < 5; i ++) {
        ecs_monitor_stats_ctx_t *ctx = ecs_os_calloc_t(ecs_monitor_stats_ctx_t);
        ctx->api = *api;
        ctx->interval = periods[i].interval;
        if (!i && api->query_component_id) {
            ctx->query = ecs_query(world, {
                .terms = {{ .id = api->query_component_id }},
                .cache_kind = EcsQueryCacheNone,
                .flags = EcsQueryMatchDisabled
            });
        }
        ecs_system_desc_t desc = {
            .entity = ecs_entity(world, { .name = periods[i].name }),
            .phase = EcsPreFrame,
            .query.terms = {{
                .id = ecs_pair(kind, periods[i].period),
                .src.id = EcsWorld
            }},
            .callback = i ? ReduceStats : MonitorStats,
            .interval = i == 1 ? 1 : 0,
            .rate = i > 1 ? 60 : 0,
            .tick_source = i > 1 ? minute : 0,
            .ctx = ctx,
            .ctx_free = flecs_monitor_ctx_free
        };
        if (i) {
            desc.query.terms[1].id = ecs_pair(kind, periods[i].source);
            desc.query.terms[1].src.id = EcsWorld;
        }
        ecs_entity_t system = ecs_system_init(world, &desc);
        if (i == 1) {
            minute = system;
        }
    }
    ecs_set_scope(world, prev);
    for (int32_t i = 0; i < 5; i ++) {
        ecs_add_pair(world, EcsWorld, kind, periods[i].period);
    }
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
    
    flecs_stats_memory_register_reflection(world);

    ecs_add(world, ecs_id(EcsWorldMemory), EcsWorldMemory);
    
    if (ecs_os_has_time()) {
        ecs_measure_frame_time(world, true);
        ecs_measure_system_time(world, true);
    }
}

#endif
