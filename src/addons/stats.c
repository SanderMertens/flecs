
#include "../private_api.h"

#ifdef FLECS_SYSTEM
#include "../addons/system/system.h"
#endif

#ifdef FLECS_PIPELINE
#include "../addons/pipeline/pipeline.h"
#endif

#ifdef FLECS_STATS

#include <stdio.h>

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
void _record_gauge(
    ecs_gauge_t *m,
    int32_t t,
    float value)
{
    m->avg[t] = value;
    m->min[t] = value;
    m->max[t] = value;
}

static
float _record_counter(
    ecs_counter_t *m,
    int32_t t,
    float value)
{
    int32_t tp = t_prev(t);
    float prev = m->value[tp];
    m->value[t] = value;
    _record_gauge((ecs_gauge_t*)m, t, value - prev);
    return value - prev;
}

/* Macro's to silence conversion warnings without adding casts everywhere */
#define record_gauge(m, t, value)\
    _record_gauge(m, t, (float)value)

#define record_counter(m, t, value)\
    _record_counter(m, t, (float)value)

static
void print_value(
    const char *name,
    float value)
{
    ecs_size_t len = ecs_os_strlen(name);
    ecs_trace("%s: %*s %.2f", name, 32 - len, "", (double)value);
}

static
void print_gauge(
    const char *name,
    int32_t t,
    const ecs_gauge_t *m)
{
    print_value(name, m->avg[t]);
}

static
void print_counter(
    const char *name,
    int32_t t,
    const ecs_counter_t *m)
{
    print_value(name, m->rate.avg[t]);
}

void ecs_gauge_reduce(
    ecs_gauge_t *dst,
    int32_t t_dst,
    ecs_gauge_t *src,
    int32_t t_src)
{
    ecs_check(dst != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(src != NULL, ECS_INVALID_PARAMETER, NULL);

    bool min_set = false;
    dst->min[t_dst] = 0;
    dst->avg[t_dst] = 0;
    dst->max[t_dst] = 0;

    int32_t i;
    for (i = 0; i < ECS_STAT_WINDOW; i ++) {
        int32_t t = (t_src + i) % ECS_STAT_WINDOW;
        dst->avg[t_dst] += src->avg[t] / (float)ECS_STAT_WINDOW;
        if (!min_set || (src->min[t] < dst->min[t_dst])) {
            dst->min[t_dst] = src->min[t];
            min_set = true;
        }
        if ((src->max[t] > dst->max[t_dst])) {
            dst->max[t_dst] = src->max[t];
        }
    }
error:
    return;
}

void ecs_get_world_stats(
    const ecs_world_t *world,
    ecs_world_stats_t *s)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(s != NULL, ECS_INVALID_PARAMETER, NULL);

    world = ecs_get_world(world);

    int32_t t = s->t = t_next(s->t);

    float delta_world_time = record_counter(&s->world_time_total_raw, t, world->info.world_time_total_raw);
    record_counter(&s->world_time_total, t, world->info.world_time_total);
    record_counter(&s->frame_time_total, t, world->info.frame_time_total);
    record_counter(&s->system_time_total, t, world->info.system_time_total);
    record_counter(&s->merge_time_total, t, world->info.merge_time_total);

    float delta_frame_count = record_counter(&s->frame_count_total, t, world->info.frame_count_total);
    record_counter(&s->merge_count_total, t, world->info.merge_count_total);
    record_counter(&s->pipeline_build_count_total, t, world->info.pipeline_build_count_total);
    record_counter(&s->systems_ran_frame, t, world->info.systems_ran_frame);

    if (delta_world_time != 0.0f && delta_frame_count != 0.0f) {
        record_gauge(
            &s->fps, t, 1.0f / (delta_world_time / (float)delta_frame_count));
    } else {
        record_gauge(&s->fps, t, 0);
    }

    record_gauge(&s->entity_count, t, flecs_sparse_count(ecs_eis(world)));
    record_gauge(&s->entity_not_alive_count, t, 
        flecs_sparse_not_alive_count(ecs_eis(world)));

    record_gauge(&s->id_count, t, world->info.id_count);
    record_gauge(&s->tag_id_count, t, world->info.tag_id_count);
    record_gauge(&s->component_id_count, t, world->info.component_id_count);
    record_gauge(&s->pair_id_count, t, world->info.pair_id_count);
    record_gauge(&s->wildcard_id_count, t, world->info.wildcard_id_count);
    record_gauge(&s->component_count, t, ecs_sparse_count(world->type_info));

    record_gauge(&s->query_count, t, flecs_sparse_count(world->queries));
    record_gauge(&s->trigger_count, t, ecs_count(world, EcsTrigger));
    record_gauge(&s->observer_count, t, ecs_count(world, EcsObserver));
    record_gauge(&s->system_count, t, ecs_count(world, EcsSystem));

    record_counter(&s->id_create_count, t, world->info.id_create_total);
    record_counter(&s->id_delete_count, t, world->info.id_delete_total);
    record_counter(&s->table_create_count, t, world->info.table_create_total);
    record_counter(&s->table_delete_count, t, world->info.table_delete_total);

    record_counter(&s->new_count, t, world->new_count);
    record_counter(&s->bulk_new_count, t, world->bulk_new_count);
    record_counter(&s->delete_count, t, world->delete_count);
    record_counter(&s->clear_count, t, world->clear_count);
    record_counter(&s->add_count, t, world->add_count);
    record_counter(&s->remove_count, t, world->remove_count);
    record_counter(&s->set_count, t, world->set_count);
    record_counter(&s->discard_count, t, world->discard_count);

    /* Compute table statistics */
    int32_t empty_table_count = 0;
    int32_t singleton_table_count = 0;

    int32_t i, count = flecs_sparse_count(&world->store.tables);
    for (i = 0; i < count; i ++) {
        ecs_table_t *table = flecs_sparse_get_dense(&world->store.tables, 
            ecs_table_t, i);
        int32_t entity_count = ecs_table_count(table);

        if (!entity_count) {
            empty_table_count ++;
        }

        /* Singleton tables are tables that have just one entity that also has
         * itself in the table type. */
        if (entity_count == 1) {
            ecs_entity_t *entities = ecs_storage_first_t(
                &table->data.entities, ecs_entity_t);
            if (ecs_search(world, table, entities[0], 0)) {
                singleton_table_count ++;
            }
        }
    }

    /* Correct for root table */
    count --;
    empty_table_count --;

    if (count != world->info.table_count) {
        ecs_warn("world::table_count (%d) is not equal to computed number (%d)",
            world->info.table_count, count);
    }
    if (empty_table_count != world->info.empty_table_count) {
        ecs_warn("world::empty_table_count (%d) is not equal to computed"
            " number (%d)",
                world->info.empty_table_count, empty_table_count);
    }

    record_gauge(&s->table_count, t, count);
    record_gauge(&s->empty_table_count, t, empty_table_count);
    record_gauge(&s->singleton_table_count, t, singleton_table_count);
    record_gauge(&s->tag_table_count, t, world->info.tag_table_count);
    record_gauge(&s->trivial_table_count, t, world->info.trivial_table_count);
    record_gauge(&s->table_storage_count, t, world->info.table_storage_count);
    record_gauge(&s->table_record_count, t, world->info.table_record_count);

error:
    return;
}

void ecs_get_query_stats(
    const ecs_world_t *world,
    const ecs_query_t *query,
    ecs_query_stats_t *s)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(query != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(s != NULL, ECS_INVALID_PARAMETER, NULL);
    (void)world;

    int32_t t = s->t = t_next(s->t);

    ecs_iter_t it = ecs_query_iter(world, (ecs_query_t*)query);
    record_gauge(&s->matched_entity_count, t, ecs_iter_count(&it));
    record_gauge(&s->matched_table_count, t, ecs_query_table_count(query));
    record_gauge(&s->matched_empty_table_count, t, 
        ecs_query_empty_table_count(query));
error:
    return;
}

#ifdef FLECS_SYSTEM
bool ecs_get_system_stats(
    const ecs_world_t *world,
    ecs_entity_t system,
    ecs_system_stats_t *s)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(s != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(system != 0, ECS_INVALID_PARAMETER, NULL);

    world = ecs_get_world(world);

    const EcsSystem *ptr = ecs_get(world, system, EcsSystem);
    if (!ptr) {
        return false;
    }

    ecs_get_query_stats(world, ptr->query, &s->query_stats);
    int32_t t = s->query_stats.t;

    record_counter(&s->time_spent, t, ptr->time_spent);
    record_counter(&s->invoke_count, t, ptr->invoke_count);
    record_gauge(&s->active, t, !ecs_has_id(world, system, EcsInactive));
    record_gauge(&s->enabled, t, !ecs_has_id(world, system, EcsDisabled));

    return true;
error:
    return false;
}
#endif


#ifdef FLECS_PIPELINE

static 
ecs_system_stats_t* get_system_stats(
    ecs_map_t *systems,
    ecs_entity_t system)
{
    ecs_check(systems != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(system != 0, ECS_INVALID_PARAMETER, NULL);

    ecs_system_stats_t *s = ecs_map_get(systems, ecs_system_stats_t, system);
    if (!s) {
        s = ecs_map_ensure(systems, ecs_system_stats_t, system);
    }

    return s;
error:
    return NULL;
}

bool ecs_get_pipeline_stats(
    ecs_world_t *stage,
    ecs_entity_t pipeline,
    ecs_pipeline_stats_t *s)
{
    ecs_check(stage != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(s != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(pipeline != 0, ECS_INVALID_PARAMETER, NULL);

    const ecs_world_t *world = ecs_get_world(stage);

    const EcsPipelineQuery *pq = ecs_get(world, pipeline, EcsPipelineQuery);
    if (!pq) {
        return false;
    }

    int32_t sys_count = 0, active_sys_count = 0;

    /* Count number of active systems */
    ecs_iter_t it = ecs_query_iter(stage, pq->query);
    while (ecs_query_next(&it)) {
        active_sys_count += it.count;
    }

    /* Count total number of systems in pipeline */
    it = ecs_query_iter(stage, pq->build_query);
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

    if (s->system_stats && !sys_count) {
        ecs_map_free(s->system_stats);
    }
    if (!s->system_stats && sys_count) {
        s->system_stats = ecs_map_new(ecs_system_stats_t, sys_count);
    }
    if (!sys_count) {
        s->system_stats = NULL;
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
    it = ecs_query_iter(stage, pq->build_query);
    while (ecs_query_next(&it)) {
        int i;
        for (i = 0; i < it.count; i ++) {
            ecs_system_stats_t *sys_stats = get_system_stats(
                s->system_stats, it.entities[i]);
            ecs_get_system_stats(world, it.entities[i], sys_stats);
        }
    }

    return true;
error:
    return false;
}

void ecs_pipeline_stats_fini(
    ecs_pipeline_stats_t *stats)
{
    ecs_map_free(stats->system_stats);
    ecs_vector_free(stats->systems);
}

#endif

void ecs_dump_world_stats(
    const ecs_world_t *world,
    const ecs_world_stats_t *s)
{
    int32_t t = s->t;

    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(s != NULL, ECS_INVALID_PARAMETER, NULL);

    world = ecs_get_world(world);    
    
    print_counter("Frame", t, &s->frame_count_total);
    ecs_trace("-------------------------------------");
    print_counter("pipeline rebuilds", t, &s->pipeline_build_count_total);
    print_counter("systems invocations", t, &s->systems_ran_frame);
    ecs_trace("");
    print_value("target FPS", world->info.target_fps);
    print_value("time scale", world->info.time_scale);
    ecs_trace("");
    print_gauge("actual FPS", t, &s->fps);
    print_counter("frame time", t, &s->frame_time_total);
    print_counter("system time", t, &s->system_time_total);
    print_counter("merge time", t, &s->merge_time_total);
    print_counter("simulation time elapsed", t, &s->world_time_total);
    ecs_trace("");
    print_gauge("id count", t, &s->id_count);
    print_gauge("tag id count", t, &s->tag_id_count);
    print_gauge("component id count", t, &s->component_id_count);
    print_gauge("pair id count", t, &s->pair_id_count);
    print_gauge("wildcard id count", t, &s->wildcard_id_count);
    print_gauge("component count", t, &s->component_count);
    ecs_trace("");
    print_gauge("alive entity count", t, &s->entity_count);
    print_gauge("not alive entity count", t, &s->entity_not_alive_count);
    ecs_trace("");
    print_gauge("query count", t, &s->query_count);
    print_gauge("trigger count", t, &s->trigger_count);
    print_gauge("observer count", t, &s->observer_count);
    print_gauge("system count", t, &s->system_count);
    ecs_trace("");
    print_gauge("table count", t, &s->table_count);
    print_gauge("empty table count", t, &s->empty_table_count);
    print_gauge("tag table count", t, &s->tag_table_count);
    print_gauge("trivial table count", t, &s->trivial_table_count);
    print_gauge("table storage count", t, &s->table_storage_count);
    print_gauge("table cache record count", t, &s->table_record_count);
    print_gauge("singleton table count", t, &s->singleton_table_count);
    ecs_trace("");
    print_counter("table create count", t, &s->table_create_count);
    print_counter("table delete count", t, &s->table_delete_count);
    print_counter("id create count", t, &s->id_create_count);
    print_counter("id delete count", t, &s->id_delete_count);
    ecs_trace("");
    print_counter("deferred new operations", t, &s->new_count);
    print_counter("deferred bulk_new operations", t, &s->bulk_new_count);
    print_counter("deferred delete operations", t, &s->delete_count);
    print_counter("deferred clear operations", t, &s->clear_count);
    print_counter("deferred add operations", t, &s->add_count);
    print_counter("deferred remove operations", t, &s->remove_count);
    print_counter("deferred set operations", t, &s->set_count);
    print_counter("discarded operations", t, &s->discard_count);
    ecs_trace("");
    
error:
    return;
}

#endif
