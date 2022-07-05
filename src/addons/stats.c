
#include "../private_api.h"

#ifdef FLECS_SYSTEM
#include "../addons/system/system.h"
#endif

#ifdef FLECS_PIPELINE
#include "../addons/pipeline/pipeline.h"
#endif

#ifdef FLECS_STATS

#include <stdio.h>

#define ECS_GAUGE_RECORD(m, t, value)\
    flecs_gauge_record(m, t, (float)(value))

#define ECS_COUNTER_RECORD(m, t, value)\
    flecs_counter_record(m, t, (float)(value))

#define ECS_METRIC_FIRST(stats)\
    ECS_CAST(ecs_metric_t*, ECS_OFFSET(&stats->first_, ECS_SIZEOF(int32_t)))

#define ECS_METRIC_LAST(stats)\
    ECS_CAST(ecs_metric_t*, ECS_OFFSET(&stats->last_, -ECS_SIZEOF(ecs_metric_t)))

static
int32_t t_next(
    int32_t t)
{
    return (t + 1) % ECS_STAT_WINDOW;
}

static
int32_t t_prev(
    int32_t t)
{
    return (t - 1 + ECS_STAT_WINDOW) % ECS_STAT_WINDOW;
}

static
void flecs_gauge_record(
    ecs_metric_t *m,
    int32_t t,
    float value)
{
    m->gauge.avg[t] = value;
    m->gauge.min[t] = value;
    m->gauge.max[t] = value;
}

static
float flecs_counter_record(
    ecs_metric_t *m,
    int32_t t,
    float value)
{
    int32_t tp = t_prev(t);
    float prev = m->counter.value[tp];
    m->counter.value[t] = value;
    flecs_gauge_record(m, t, value - prev);
    return value - prev;
}

static
void flecs_metric_print(
    const char *name,
    float value)
{
    ecs_size_t len = ecs_os_strlen(name);
    ecs_trace("%s: %*s %.2f", name, 32 - len, "", (double)value);
}

static
void flecs_gauge_print(
    const char *name,
    int32_t t,
    const ecs_metric_t *m)
{
    flecs_metric_print(name, m->gauge.avg[t]);
}

static
void flecs_counter_print(
    const char *name,
    int32_t t,
    const ecs_metric_t *m)
{
    flecs_metric_print(name, m->counter.rate.avg[t]);
}

void ecs_metric_reduce(
    ecs_metric_t *dst,
    const ecs_metric_t *src,
    int32_t t_dst,
    int32_t t_src)
{
    ecs_check(dst != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(src != NULL, ECS_INVALID_PARAMETER, NULL);

    bool min_set = false;
    dst->gauge.avg[t_dst] = 0;
    dst->gauge.min[t_dst] = 0;
    dst->gauge.max[t_dst] = 0;

    ecs_float_t fwindow = (ecs_float_t)ECS_STAT_WINDOW;

    int32_t i;
    for (i = 0; i < ECS_STAT_WINDOW; i ++) {
        int32_t t = (t_src + i) % ECS_STAT_WINDOW;
        dst->gauge.avg[t_dst] += src->gauge.avg[t] / fwindow;

        if (!min_set || (src->gauge.min[t] < dst->gauge.min[t_dst])) {
            dst->gauge.min[t_dst] = src->gauge.min[t];
            min_set = true;
        }
        if ((src->gauge.max[t] > dst->gauge.max[t_dst])) {
            dst->gauge.max[t_dst] = src->gauge.max[t];
        }
    }
    
    dst->counter.value[t_dst] = src->counter.value[t_src];

error:
    return;
}

void ecs_metric_reduce_last(
    ecs_metric_t *m,
    int32_t prev,
    int32_t count)
{
    ecs_check(m != NULL, ECS_INVALID_PARAMETER, NULL);
    int32_t t = t_next(prev);

    if (m->gauge.min[t] < m->gauge.min[prev]) {
        m->gauge.min[prev] = m->gauge.min[t];
    }

    if (m->gauge.max[t] > m->gauge.max[prev]) {
        m->gauge.max[prev] = m->gauge.max[t];
    }

    ecs_float_t fcount = (ecs_float_t)(count + 1);
    ecs_float_t cur = m->gauge.avg[prev];
    ecs_float_t next = m->gauge.avg[t];

    cur *= ((fcount - 1) / fcount);
    next *= 1 / fcount;

    m->gauge.avg[prev] = cur + next;
    m->counter.value[prev] = m->counter.value[t];

error:
    return;
}

void ecs_metric_copy(
    ecs_metric_t *m,
    int32_t dst,
    int32_t src)
{
    ecs_check(m != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(dst != src, ECS_INVALID_PARAMETER, NULL);

    m->gauge.avg[dst] = m->gauge.avg[src];
    m->gauge.min[dst] = m->gauge.min[src];
    m->gauge.max[dst] = m->gauge.max[src];
    m->counter.value[dst] = m->counter.value[src];

error:
    return;
}

static
void flecs_stats_reduce(
    ecs_metric_t *dst_cur,
    ecs_metric_t *dst_last,
    ecs_metric_t *src_cur,
    int32_t t_dst,
    int32_t t_src)
{
    for (; dst_cur <= dst_last; dst_cur ++, src_cur ++) {
        ecs_metric_reduce(dst_cur, src_cur, t_dst, t_src);
    }
}

static
void flecs_stats_reduce_last(
    ecs_metric_t *dst_cur,
    ecs_metric_t *dst_last,
    ecs_metric_t *src_cur,
    int32_t t_dst,
    int32_t t_src,
    int32_t count)
{
    int32_t t_dst_next = t_next(t_dst);
    for (; dst_cur <= dst_last; dst_cur ++, src_cur ++) {
        /* Reduce into previous value */
        ecs_metric_reduce_last(dst_cur, t_dst, count);

        /* Restore old value */
        dst_cur->gauge.avg[t_dst_next] = src_cur->gauge.avg[t_src];
        dst_cur->gauge.min[t_dst_next] = src_cur->gauge.min[t_src];
        dst_cur->gauge.max[t_dst_next] = src_cur->gauge.max[t_src];
        dst_cur->counter.value[t_dst_next] = src_cur->counter.value[t_src];
    }
}

static
void flecs_stats_repeat_last(
    ecs_metric_t *cur,
    ecs_metric_t *last,
    int32_t t)
{
    int32_t prev = t_prev(t);
    for (; cur <= last; cur ++) {
        ecs_metric_copy(cur, t, prev);
    }
}

static
void flecs_stats_copy_last(
    ecs_metric_t *dst_cur,
    ecs_metric_t *dst_last,
    ecs_metric_t *src_cur,
    int32_t t_dst,
    int32_t t_src)
{
    for (; dst_cur <= dst_last; dst_cur ++, src_cur ++) {
        dst_cur->gauge.avg[t_dst] = src_cur->gauge.avg[t_src];
        dst_cur->gauge.min[t_dst] = src_cur->gauge.min[t_src];
        dst_cur->gauge.max[t_dst] = src_cur->gauge.max[t_src];
        dst_cur->counter.value[t_dst] = src_cur->counter.value[t_src];
    }
}

void ecs_world_stats_get(
    const ecs_world_t *world,
    ecs_world_stats_t *s)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(s != NULL, ECS_INVALID_PARAMETER, NULL);

    world = ecs_get_world(world);

    int32_t t = s->t = t_next(s->t);

    float delta_world_time = ECS_COUNTER_RECORD(&s->world_time_total_raw, t, world->info.world_time_total_raw);
    ECS_COUNTER_RECORD(&s->world_time_total, t, world->info.world_time_total);
    ECS_COUNTER_RECORD(&s->frame_time_total, t, world->info.frame_time_total);
    ECS_COUNTER_RECORD(&s->system_time_total, t, world->info.system_time_total);
    ECS_COUNTER_RECORD(&s->merge_time_total, t, world->info.merge_time_total);

    float delta_frame_count = ECS_COUNTER_RECORD(&s->frame_count_total, t, world->info.frame_count_total);
    ECS_COUNTER_RECORD(&s->merge_count_total, t, world->info.merge_count_total);
    ECS_COUNTER_RECORD(&s->pipeline_build_count_total, t, world->info.pipeline_build_count_total);
    ECS_COUNTER_RECORD(&s->systems_ran_frame, t, world->info.systems_ran_frame);

    if (delta_world_time != 0.0f && delta_frame_count != 0.0f) {
        ECS_GAUGE_RECORD(
            &s->fps, t, 1.0f / (delta_world_time / (float)delta_frame_count));
    } else {
        ECS_GAUGE_RECORD(&s->fps, t, 0);
    }

    ECS_GAUGE_RECORD(&s->delta_time, t, delta_world_time);

    ECS_GAUGE_RECORD(&s->entity_count, t, flecs_sparse_count(ecs_eis(world)));
    ECS_GAUGE_RECORD(&s->entity_not_alive_count, t, 
        flecs_sparse_not_alive_count(ecs_eis(world)));

    ECS_GAUGE_RECORD(&s->id_count, t, world->info.id_count);
    ECS_GAUGE_RECORD(&s->tag_id_count, t, world->info.tag_id_count);
    ECS_GAUGE_RECORD(&s->component_id_count, t, world->info.component_id_count);
    ECS_GAUGE_RECORD(&s->pair_id_count, t, world->info.pair_id_count);
    ECS_GAUGE_RECORD(&s->wildcard_id_count, t, world->info.wildcard_id_count);
    ECS_GAUGE_RECORD(&s->component_count, t, ecs_sparse_count(world->type_info));

    ECS_GAUGE_RECORD(&s->query_count, t, ecs_count_id(world, EcsQuery));
    ECS_GAUGE_RECORD(&s->observer_count, t, ecs_count_id(world, EcsObserver));
    ECS_GAUGE_RECORD(&s->system_count, t, ecs_count_id(world, EcsSystem));

    ECS_COUNTER_RECORD(&s->id_create_count, t, world->info.id_create_total);
    ECS_COUNTER_RECORD(&s->id_delete_count, t, world->info.id_delete_total);
    ECS_COUNTER_RECORD(&s->table_create_count, t, world->info.table_create_total);
    ECS_COUNTER_RECORD(&s->table_delete_count, t, world->info.table_delete_total);

    ECS_COUNTER_RECORD(&s->new_count, t, world->info.new_count);
    ECS_COUNTER_RECORD(&s->bulk_new_count, t, world->info.bulk_new_count);
    ECS_COUNTER_RECORD(&s->delete_count, t, world->info.delete_count);
    ECS_COUNTER_RECORD(&s->clear_count, t, world->info.clear_count);
    ECS_COUNTER_RECORD(&s->add_count, t, world->info.add_count);
    ECS_COUNTER_RECORD(&s->remove_count, t, world->info.remove_count);
    ECS_COUNTER_RECORD(&s->set_count, t, world->info.set_count);
    ECS_COUNTER_RECORD(&s->discard_count, t, world->info.discard_count);

    ECS_GAUGE_RECORD(&s->table_count, t, world->info.table_count);
    ECS_GAUGE_RECORD(&s->empty_table_count, t, world->info.empty_table_count);
    ECS_GAUGE_RECORD(&s->tag_table_count, t, world->info.tag_table_count);
    ECS_GAUGE_RECORD(&s->trivial_table_count, t, world->info.trivial_table_count);
    ECS_GAUGE_RECORD(&s->table_storage_count, t, world->info.table_storage_count);
    ECS_GAUGE_RECORD(&s->table_record_count, t, world->info.table_record_count);

    ECS_COUNTER_RECORD(&s->alloc_count, t, ecs_os_api_malloc_count + 
        ecs_os_api_calloc_count);
    ECS_COUNTER_RECORD(&s->realloc_count, t, ecs_os_api_realloc_count);
    ECS_COUNTER_RECORD(&s->free_count, t, ecs_os_api_free_count);

    int64_t outstanding_allocs = ecs_os_api_malloc_count + 
        ecs_os_api_calloc_count - ecs_os_api_free_count;
    ECS_GAUGE_RECORD(&s->outstanding_alloc_count, t, outstanding_allocs);

error:
    return;
}

void ecs_world_stats_reduce(
    ecs_world_stats_t *dst,
    const ecs_world_stats_t *src)
{
    flecs_stats_reduce(ECS_METRIC_FIRST(dst), ECS_METRIC_LAST(dst), 
        ECS_METRIC_FIRST(src), (dst->t = t_next(dst->t)), src->t);
}

void ecs_world_stats_reduce_last(
    ecs_world_stats_t *dst,
    const ecs_world_stats_t *src,
    int32_t count)
{
    flecs_stats_reduce_last(ECS_METRIC_FIRST(dst), ECS_METRIC_LAST(dst), 
        ECS_METRIC_FIRST(src), (dst->t = t_prev(dst->t)), src->t, count);
}

void ecs_world_stats_repeat_last(
    ecs_world_stats_t *stats)
{
    flecs_stats_repeat_last(ECS_METRIC_FIRST(stats), ECS_METRIC_LAST(stats),
        (stats->t = t_next(stats->t)));
}

void ecs_world_stats_copy_last(
    ecs_world_stats_t *dst,
    const ecs_world_stats_t *src)
{
    flecs_stats_copy_last(ECS_METRIC_FIRST(dst), ECS_METRIC_LAST(dst),
        ECS_METRIC_FIRST(src), dst->t, t_next(src->t));
}

void ecs_query_stats_get(
    const ecs_world_t *world,
    const ecs_query_t *query,
    ecs_query_stats_t *s)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(query != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(s != NULL, ECS_INVALID_PARAMETER, NULL);
    (void)world;

    int32_t t = s->t = t_next(s->t);

    if (query->filter.flags & EcsFilterMatchThis) {
        ECS_GAUGE_RECORD(&s->matched_entity_count, t, 
            ecs_query_entity_count(query));
        ECS_GAUGE_RECORD(&s->matched_table_count, t, 
            ecs_query_table_count(query));
        ECS_GAUGE_RECORD(&s->matched_empty_table_count, t, 
            ecs_query_empty_table_count(query));
    } else {
        ECS_GAUGE_RECORD(&s->matched_entity_count, t, 0);
        ECS_GAUGE_RECORD(&s->matched_table_count, t, 0);
        ECS_GAUGE_RECORD(&s->matched_empty_table_count, t, 0);
    }

error:
    return;
}

void ecs_query_stats_reduce(
    ecs_query_stats_t *dst,
    const ecs_query_stats_t *src)
{
    flecs_stats_reduce(ECS_METRIC_FIRST(dst), ECS_METRIC_LAST(dst), 
        ECS_METRIC_FIRST(src), (dst->t = t_next(dst->t)), src->t);
}

void ecs_query_stats_reduce_last(
    ecs_query_stats_t *dst,
    const ecs_query_stats_t *src,
    int32_t count)
{
    flecs_stats_reduce_last(ECS_METRIC_FIRST(dst), ECS_METRIC_LAST(dst), 
        ECS_METRIC_FIRST(src), (dst->t = t_prev(dst->t)), src->t, count);
}

void ecs_query_stats_repeat_last(
    ecs_query_stats_t *stats)
{
    flecs_stats_repeat_last(ECS_METRIC_FIRST(stats), ECS_METRIC_LAST(stats),
        (stats->t = t_next(stats->t)));
}

void ecs_query_stats_copy_last(
    ecs_query_stats_t *dst,
    const ecs_query_stats_t *src)
{
    flecs_stats_copy_last(ECS_METRIC_FIRST(dst), ECS_METRIC_LAST(dst),
        ECS_METRIC_FIRST(src), dst->t, t_next(src->t));
}

#ifdef FLECS_SYSTEM

bool ecs_system_stats_get(
    const ecs_world_t *world,
    ecs_entity_t system,
    ecs_system_stats_t *s)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(s != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(system != 0, ECS_INVALID_PARAMETER, NULL);

    world = ecs_get_world(world);

    const ecs_system_t *ptr = ecs_poly_get(world, system, ecs_system_t);
    if (!ptr) {
        return false;
    }

    ecs_query_stats_get(world, ptr->query, &s->query);
    int32_t t = s->query.t;

    ECS_COUNTER_RECORD(&s->time_spent, t, ptr->time_spent);
    ECS_COUNTER_RECORD(&s->invoke_count, t, ptr->invoke_count);
    ECS_GAUGE_RECORD(&s->active, t, !ecs_has_id(world, system, EcsEmpty));
    ECS_GAUGE_RECORD(&s->enabled, t, !ecs_has_id(world, system, EcsDisabled));

    s->task = !(ptr->query->filter.flags & EcsFilterMatchThis);

    return true;
error:
    return false;
}

void ecs_system_stats_reduce(
    ecs_system_stats_t *dst,
    const ecs_system_stats_t *src)
{
    ecs_query_stats_reduce(&dst->query, &src->query);
    dst->task = src->task;
    flecs_stats_reduce(ECS_METRIC_FIRST(dst), ECS_METRIC_LAST(dst), 
        ECS_METRIC_FIRST(src), dst->query.t, src->query.t);
}

void ecs_system_stats_reduce_last(
    ecs_system_stats_t *dst,
    const ecs_system_stats_t *src,
    int32_t count)
{
    ecs_query_stats_reduce_last(&dst->query, &src->query, count);
    dst->task = src->task;
    flecs_stats_reduce_last(ECS_METRIC_FIRST(dst), ECS_METRIC_LAST(dst), 
        ECS_METRIC_FIRST(src), dst->query.t, src->query.t, count);
}

void ecs_system_stats_repeat_last(
    ecs_system_stats_t *stats)
{
    ecs_query_stats_repeat_last(&stats->query);
    flecs_stats_repeat_last(ECS_METRIC_FIRST(stats), ECS_METRIC_LAST(stats),
        (stats->query.t));
}

void ecs_system_stats_copy_last(
    ecs_system_stats_t *dst,
    const ecs_system_stats_t *src)
{
    ecs_query_stats_copy_last(&dst->query, &src->query);
    dst->task = src->task;
    flecs_stats_copy_last(ECS_METRIC_FIRST(dst), ECS_METRIC_LAST(dst),
        ECS_METRIC_FIRST(src), dst->query.t, t_next(src->query.t));
}

#endif

#ifdef FLECS_PIPELINE

bool ecs_pipeline_stats_get(
    ecs_world_t *stage,
    ecs_entity_t pipeline,
    ecs_pipeline_stats_t *s)
{
    ecs_check(stage != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(s != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(pipeline != 0, ECS_INVALID_PARAMETER, NULL);

    const ecs_world_t *world = ecs_get_world(stage);
    const EcsPipeline *pq = ecs_get(world, pipeline, EcsPipeline);
    if (!pq) {
        return false;
    }

    int32_t sys_count = 0, active_sys_count = 0;

    /* Count number of active systems */
    ecs_iter_t it = ecs_query_iter(stage, pq->query);
    while (ecs_query_next(&it)) {
        if (flecs_id_record_get_table(pq->idr_inactive, it.table) != NULL) {
            continue;
        }
        active_sys_count += it.count;
    }

    /* Count total number of systems in pipeline */
    it = ecs_query_iter(stage, pq->query);
    while (ecs_query_next(&it)) {
        sys_count += it.count;
    }   

    /* Also count synchronization points */
    ecs_vector_t *ops = pq->ops;
    ecs_pipeline_op_t *op = ecs_vector_first(ops, ecs_pipeline_op_t);
    ecs_pipeline_op_t *op_last = ecs_vector_last(ops, ecs_pipeline_op_t);
    int32_t pip_count = active_sys_count + ecs_vector_count(ops);

    if (!sys_count) {
        return false;
    }

    if (ecs_map_is_initialized(&s->system_stats) && !sys_count) {
        ecs_map_fini(&s->system_stats);
    }
    if (!ecs_map_is_initialized(&s->system_stats) && sys_count) {
        ecs_map_init(&s->system_stats, ecs_system_stats_t, sys_count);
    }

    /* Make sure vector is large enough to store all systems & sync points */
    ecs_entity_t *systems = NULL;
    if (pip_count) {
        ecs_vector_set_count(&s->systems, ecs_entity_t, pip_count);
        systems = ecs_vector_first(s->systems, ecs_entity_t);

        /* Populate systems vector, keep track of sync points */
        it = ecs_query_iter(stage, pq->query);
        
        int32_t i, i_system = 0, ran_since_merge = 0;
        while (ecs_query_next(&it)) {
            if (flecs_id_record_get_table(pq->idr_inactive, it.table) != NULL) {
                continue;
            }

            for (i = 0; i < it.count; i ++) {
                systems[i_system ++] = it.entities[i];
                ran_since_merge ++;
                if (op != op_last && ran_since_merge == op->count) {
                    ran_since_merge = 0;
                    op++;
                    systems[i_system ++] = 0; /* 0 indicates a merge point */
                }
            }
        }

        systems[i_system ++] = 0; /* Last merge */
        ecs_assert(pip_count == i_system, ECS_INTERNAL_ERROR, NULL);
    } else {
        ecs_vector_free(s->systems);
        s->systems = NULL;
    }

    /* Separately populate system stats map from build query, which includes
     * systems that aren't currently active */
    it = ecs_query_iter(stage, pq->query);
    while (ecs_query_next(&it)) {
        int i;
        for (i = 0; i < it.count; i ++) {
            ecs_system_stats_t *sys_stats = ecs_map_ensure(
                    &s->system_stats, ecs_system_stats_t, it.entities[i]);
            sys_stats->query.t = s->t;
            ecs_system_stats_get(world, it.entities[i], sys_stats);
        }
    }

    s->t = t_next(s->t);

    return true;
error:
    return false;
}

void ecs_pipeline_stats_fini(
    ecs_pipeline_stats_t *stats)
{
    ecs_map_fini(&stats->system_stats);
    ecs_vector_free(stats->systems);
}

void ecs_pipeline_stats_reduce(
    ecs_pipeline_stats_t *dst,
    const ecs_pipeline_stats_t *src)
{
    int32_t system_count = ecs_vector_count(src->systems);
    ecs_vector_set_count(&dst->systems, ecs_entity_t, system_count);
    ecs_entity_t *dst_systems = ecs_vector_first(dst->systems, ecs_entity_t);
    ecs_entity_t *src_systems = ecs_vector_first(src->systems, ecs_entity_t);
    ecs_os_memcpy_n(dst_systems, src_systems, ecs_entity_t, system_count);

    if (!ecs_map_is_initialized(&dst->system_stats)) {
        ecs_map_init(&dst->system_stats, ecs_system_stats_t,
            ecs_map_count(&src->system_stats));
    }

    ecs_map_iter_t it = ecs_map_iter(&src->system_stats);
    ecs_system_stats_t *sys_src, *sys_dst;
    ecs_entity_t key;
    while ((sys_src = ecs_map_next(&it, ecs_system_stats_t, &key))) {
        sys_dst = ecs_map_ensure(&dst->system_stats, ecs_system_stats_t, key);
        sys_dst->query.t = dst->t;
        ecs_system_stats_reduce(sys_dst, sys_src);
    }
    dst->t = t_next(dst->t);
}

void ecs_pipeline_stats_reduce_last(
    ecs_pipeline_stats_t *dst,
    const ecs_pipeline_stats_t *src,
    int32_t count)
{
    if (!ecs_map_is_initialized(&dst->system_stats)) {
        ecs_map_init(&dst->system_stats, ecs_system_stats_t,
            ecs_map_count(&src->system_stats));
    }

    ecs_map_iter_t it = ecs_map_iter(&src->system_stats);
    ecs_system_stats_t *sys_src, *sys_dst;
    ecs_entity_t key;
    while ((sys_src = ecs_map_next(&it, ecs_system_stats_t, &key))) {
        sys_dst = ecs_map_ensure(&dst->system_stats, ecs_system_stats_t, key);
        sys_dst->query.t = dst->t;
        ecs_system_stats_reduce_last(sys_dst, sys_src, count);
    }
    dst->t = t_prev(dst->t);
}

void ecs_pipeline_stats_repeat_last(
    ecs_pipeline_stats_t *stats)
{
    ecs_map_iter_t it = ecs_map_iter(&stats->system_stats);
    ecs_system_stats_t *sys;
    ecs_entity_t key;
    while ((sys = ecs_map_next(&it, ecs_system_stats_t, &key))) {
        sys->query.t = stats->t;
        ecs_system_stats_repeat_last(sys);
    }
    stats->t = t_next(stats->t);
}

void ecs_pipeline_stats_copy_last(
    ecs_pipeline_stats_t *dst,
    const ecs_pipeline_stats_t *src)
{
    if (!ecs_map_is_initialized(&dst->system_stats)) {
        ecs_map_init(&dst->system_stats, ecs_system_stats_t,
            ecs_map_count(&src->system_stats));
    }

    ecs_map_iter_t it = ecs_map_iter(&src->system_stats);
    ecs_system_stats_t *sys_src, *sys_dst;
    ecs_entity_t key;
    while ((sys_src = ecs_map_next(&it, ecs_system_stats_t, &key))) {
        sys_dst = ecs_map_ensure(&dst->system_stats, ecs_system_stats_t, key);
        sys_dst->query.t = dst->t;
        ecs_system_stats_copy_last(sys_dst, sys_src);
    }
}

#endif

void ecs_world_stats_log(
    const ecs_world_t *world,
    const ecs_world_stats_t *s)
{
    int32_t t = s->t;

    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(s != NULL, ECS_INVALID_PARAMETER, NULL);

    world = ecs_get_world(world);    
    
    flecs_counter_print("Frame", t, &s->frame_count_total);
    ecs_trace("-------------------------------------");
    flecs_counter_print("pipeline rebuilds", t, &s->pipeline_build_count_total);
    flecs_counter_print("systems invocations", t, &s->systems_ran_frame);
    ecs_trace("");
    flecs_metric_print("target FPS", world->info.target_fps);
    flecs_metric_print("time scale", world->info.time_scale);
    ecs_trace("");
    flecs_gauge_print("actual FPS", t, &s->fps);
    flecs_counter_print("frame time", t, &s->frame_time_total);
    flecs_counter_print("system time", t, &s->system_time_total);
    flecs_counter_print("merge time", t, &s->merge_time_total);
    flecs_counter_print("simulation time elapsed", t, &s->world_time_total);
    ecs_trace("");
    flecs_gauge_print("id count", t, &s->id_count);
    flecs_gauge_print("tag id count", t, &s->tag_id_count);
    flecs_gauge_print("component id count", t, &s->component_id_count);
    flecs_gauge_print("pair id count", t, &s->pair_id_count);
    flecs_gauge_print("wildcard id count", t, &s->wildcard_id_count);
    flecs_gauge_print("component count", t, &s->component_count);
    ecs_trace("");
    flecs_gauge_print("alive entity count", t, &s->entity_count);
    flecs_gauge_print("not alive entity count", t, &s->entity_not_alive_count);
    ecs_trace("");
    flecs_gauge_print("query count", t, &s->query_count);
    flecs_gauge_print("observer count", t, &s->observer_count);
    flecs_gauge_print("system count", t, &s->system_count);
    ecs_trace("");
    flecs_gauge_print("table count", t, &s->table_count);
    flecs_gauge_print("empty table count", t, &s->empty_table_count);
    flecs_gauge_print("tag table count", t, &s->tag_table_count);
    flecs_gauge_print("trivial table count", t, &s->trivial_table_count);
    flecs_gauge_print("table storage count", t, &s->table_storage_count);
    flecs_gauge_print("table cache record count", t, &s->table_record_count);
    ecs_trace("");
    flecs_counter_print("table create count", t, &s->table_create_count);
    flecs_counter_print("table delete count", t, &s->table_delete_count);
    flecs_counter_print("id create count", t, &s->id_create_count);
    flecs_counter_print("id delete count", t, &s->id_delete_count);
    ecs_trace("");
    flecs_counter_print("deferred new operations", t, &s->new_count);
    flecs_counter_print("deferred bulk_new operations", t, &s->bulk_new_count);
    flecs_counter_print("deferred delete operations", t, &s->delete_count);
    flecs_counter_print("deferred clear operations", t, &s->clear_count);
    flecs_counter_print("deferred add operations", t, &s->add_count);
    flecs_counter_print("deferred remove operations", t, &s->remove_count);
    flecs_counter_print("deferred set operations", t, &s->set_count);
    flecs_counter_print("discarded operations", t, &s->discard_count);
    ecs_trace("");
    
error:
    return;
}

#endif
