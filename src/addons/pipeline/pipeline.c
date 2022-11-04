#include "flecs.h"
#include "../system/system.h"

#ifdef FLECS_PIPELINE
#include "pipeline.h"

static ECS_DTOR(EcsPipeline, ptr, {
    ecs_vector_free(ptr->state->ops);
    ecs_os_free(ptr->state->iters);
})

typedef enum ecs_write_kind_t {
    WriteStateNone = 0,
    WriteStateToStage,
} ecs_write_kind_t;

typedef struct ecs_write_state_t {
    ecs_map_t *ids;
    ecs_map_t *wildcard_ids;
    bool write_barrier;
} ecs_write_state_t;

static
ecs_write_kind_t flecs_pipeline_get_write_state(
    ecs_write_state_t *write_state,
    ecs_id_t id)
{
    ecs_write_kind_t result = WriteStateNone;

    if (write_state->write_barrier) {
        /* Any component could have been written */
        return WriteStateToStage;
    }

    if (id == EcsWildcard) {
        /* Using a wildcard for id indicates read barrier. Return true if any
         * components could have been staged */
        if (ecs_map_count(write_state->ids) || 
            ecs_map_count(write_state->wildcard_ids)) 
        {
            return WriteStateToStage;
        }
    }

    if (!ecs_id_is_wildcard(id)) {
        if (ecs_map_get(write_state->ids, bool, id)) {
            result = WriteStateToStage;
        }
    } else {
        ecs_map_iter_t it = ecs_map_iter(write_state->ids);
        ecs_id_t write_id;
        while (ecs_map_next(&it, bool, &write_id)) {
            if (ecs_id_match(write_id, id)) {
                return WriteStateToStage;
            }
        }
    }

    if (write_state->wildcard_ids) {
        ecs_map_iter_t it = ecs_map_iter(write_state->wildcard_ids);
        ecs_id_t write_id;
        while (ecs_map_next(&it, bool, &write_id)) {
            if (ecs_id_match(id, write_id)) {
                return WriteStateToStage;
            }
        }
    }

    return result;
}

static
void flecs_pipeline_set_write_state(
    ecs_write_state_t *write_state,
    ecs_id_t id)
{
    if (id == EcsWildcard) {
        /* If writing to wildcard, flag all components as written */
        write_state->write_barrier = true;
        return;
    }

    ecs_map_t *ids;
    if (ecs_id_is_wildcard(id)) {
        ids = write_state->wildcard_ids;
    } else {
        ids = write_state->ids;
    }

    bool value = true;
    ecs_map_set(ids, id, &value);
}

static
void flecs_pipeline_reset_write_state(
    ecs_write_state_t *write_state)
{
    ecs_map_clear(write_state->ids);
    ecs_map_clear(write_state->wildcard_ids);
    write_state->write_barrier = false;
}

static
bool flecs_pipeline_check_term(
    ecs_world_t *world,
    ecs_term_t *term,
    bool is_active,
    ecs_write_state_t *write_state)    
{
    (void)world;

    ecs_term_id_t *src = &term->src;
    if (src->flags & EcsInOutNone) {
        return false;
    }

    ecs_id_t id = term->id;
    ecs_oper_kind_t oper = term->oper;
    ecs_inout_kind_t inout = term->inout;
    bool from_any = ecs_term_match_0(term);
    bool from_this = ecs_term_match_this(term);
    bool is_shared = !from_any && (!from_this || !(src->flags & EcsSelf));

    ecs_write_kind_t ws = flecs_pipeline_get_write_state(write_state, id);

    if (from_this && ws >= WriteStateToStage) {
        /* A staged write could have happened for an id that's matched on the
         * main storage. Even if the id isn't read, still insert a merge so that
         * a write to the main storage after the staged write doesn't get 
         * overwritten. */
        return true;
    }

    if (inout == EcsInOutDefault) {
        if (from_any) {
            /* If no inout kind is specified for terms without a source, this is
             * not interpreted as a read/write annotation but just a (component)
             * id that's passed to a system. */
            return false;
        } else if (is_shared) {
            inout = EcsIn;
        } else {
            /* Default for owned terms is InOut */
            inout = EcsInOut;
        }
    }

    if (oper == EcsNot && inout == EcsOut) {
        /* If a Not term is combined with Out, it signals that the system 
         * intends to add a component that the entity doesn't yet have */
        from_any = true;
    }

    if (from_any) {
        switch(inout) {
        case EcsOut:
        case EcsInOut:
            if (is_active) {
                /* Only flag component as written if system is active */
                flecs_pipeline_set_write_state(write_state, id);
            }
            break;
        default:
            break;
        }

        switch(inout) {
        case EcsIn:
        case EcsInOut:
            if (ws == WriteStateToStage) {
                /* If a system does a get/get_mut, the component is fetched from
                 * the main store so it must be merged first */
                return true;
            }
        default:
            break;
        }
    }

    return false;
}

static
bool flecs_pipeline_check_terms(
    ecs_world_t *world,
    ecs_filter_t *filter,
    bool is_active,
    ecs_write_state_t *ws)
{
    bool needs_merge = false;
    ecs_term_t *terms = filter->terms;
    int32_t t, term_count = filter->term_count;

    /* Check This terms first. This way if a term indicating writing to a stage
     * was added before the term, it won't cause merging. */
    for (t = 0; t < term_count; t ++) {
        ecs_term_t *term = &terms[t];
        if (ecs_term_match_this(term)) {
            needs_merge |= flecs_pipeline_check_term(world, term, is_active, ws);
        }
    }

    /* Now check staged terms */
    for (t = 0; t < term_count; t ++) {
        ecs_term_t *term = &terms[t];
        if (!ecs_term_match_this(term)) {
            needs_merge |= flecs_pipeline_check_term(world, term, is_active, ws);
        }
    }

    return needs_merge;
}

static
bool flecs_pipeline_is_inactive(
    const ecs_pipeline_state_t *pq,
    ecs_table_t *table)
{
    return flecs_id_record_get_table(pq->idr_inactive, table) != NULL;
}

static
EcsPoly* flecs_pipeline_term_system(
    ecs_iter_t *it)
{
    int32_t index = ecs_search(it->real_world, it->table, 
        ecs_poly_id(EcsSystem), 0);
    ecs_assert(index != -1, ECS_INTERNAL_ERROR, NULL);
    EcsPoly *poly = ecs_table_get_column(it->table, index);
    ecs_assert(poly != NULL, ECS_INTERNAL_ERROR, NULL);
    poly = &poly[it->offset];
    return poly;
}

static
bool flecs_pipeline_build(
    ecs_world_t *world,
    ecs_pipeline_state_t *pq)
{
    ecs_iter_t it = ecs_query_iter(world, pq->query);
    ecs_iter_fini(&it);

    if (pq->match_count == pq->query->match_count) {
        /* No need to rebuild the pipeline */
        return false;
    }

    world->info.pipeline_build_count_total ++;
    pq->rebuild_count ++;

    ecs_write_state_t ws = {
        .ids = ecs_map_new(bool, &world->allocator, ECS_HI_COMPONENT_ID),
        .wildcard_ids = ecs_map_new(bool, &world->allocator, ECS_HI_COMPONENT_ID)
    };

    ecs_pipeline_op_t *op = NULL;
    ecs_vector_t *ops = NULL;
    ecs_query_t *query = pq->query;

    if (pq->ops) {
        ecs_vector_free(pq->ops);
    }

    bool multi_threaded = false;
    bool no_readonly = false;
    bool first = true;

    /* Iterate systems in pipeline, add ops for running / merging */
    it = ecs_query_iter(world, query);
    while (ecs_query_next(&it)) {
        EcsPoly *poly = flecs_pipeline_term_system(&it);
        bool is_active = ecs_table_get_index(world, it.table, EcsEmpty) == -1;

        int i;
        for (i = 0; i < it.count; i ++) {
            ecs_system_t *sys = ecs_poly(poly[i].poly, ecs_system_t);
            ecs_query_t *q = sys->query;
            if (!q) {
                continue;
            }

            bool needs_merge = false;
            needs_merge = flecs_pipeline_check_terms(
                world, &q->filter, is_active, &ws);

            if (is_active) {
                if (first) {
                    multi_threaded = sys->multi_threaded;
                    no_readonly = sys->no_readonly;
                    first = false;
                }

                if (sys->multi_threaded != multi_threaded) {
                    needs_merge = true;
                    multi_threaded = sys->multi_threaded;
                }
                if (sys->no_readonly != no_readonly) {
                    needs_merge = true;
                    no_readonly = sys->no_readonly;
                }
            }

            if (no_readonly) {
                needs_merge = true;
            }

            if (needs_merge) {
                /* After merge all components will be merged, so reset state */
                flecs_pipeline_reset_write_state(&ws);

                /* An inactive system can insert a merge if one of its 
                 * components got written, which could make the system 
                 * active. If this is the only system in the pipeline operation,
                 * it results in an empty operation when we get here. If that's
                 * the case, reuse the empty operation for the next op. */
                if (op && op->count) {
                    op = NULL;
                }

                /* Re-evaluate columns to set write flags if system is active.
                 * If system is inactive, it can't write anything and so it
                 * should not insert unnecessary merges.  */
                needs_merge = false;
                if (is_active) {
                    needs_merge = flecs_pipeline_check_terms(
                        world, &q->filter, true, &ws);
                }

                /* The component states were just reset, so if we conclude that
                 * another merge is needed something is wrong. */
                ecs_assert(needs_merge == false, ECS_INTERNAL_ERROR, NULL);        
            }

            if (!op) {
                op = ecs_vector_add(&ops, ecs_pipeline_op_t);
                op->count = 0;
                op->multi_threaded = false;
                op->no_readonly = false;
            }

            /* Don't increase count for inactive systems, as they are ignored by
             * the query used to run the pipeline. */
            if (is_active) {
                if (!op->count) {
                    op->multi_threaded = multi_threaded;
                    op->no_readonly = no_readonly;
                }
                op->count ++;
            }
        }
    }

    ecs_map_free(ws.ids);
    ecs_map_free(ws.wildcard_ids);

    /* Find the system ran last this frame (helps workers reset iter) */
    ecs_entity_t last_system = 0;
    op = ecs_vector_first(ops, ecs_pipeline_op_t);
    int32_t i, ran_since_merge = 0, op_index = 0;

    if (!op) {
        ecs_dbg("#[green]pipeline#[reset] is empty");
        return true;
    } else {
        ecs_assert(op != NULL, ECS_INTERNAL_ERROR, NULL);

        /* Add schedule to debug tracing */
        ecs_dbg("#[bold]pipeline rebuild");
        ecs_log_push_1();

        ecs_dbg("#[green]schedule#[reset]: threading: %d, staging: %d:", 
            op->multi_threaded, !op->no_readonly);
        ecs_log_push_1();

        it = ecs_query_iter(world, pq->query);
        while (ecs_query_next(&it)) {
            if (flecs_pipeline_is_inactive(pq, it.table)) {
                continue;
            }

            EcsPoly *poly = flecs_pipeline_term_system(&it);

            for (i = 0; i < it.count; i ++) {
                ecs_system_t *sys = ecs_poly(poly[i].poly, ecs_system_t);
                if (ecs_should_log_1()) {
                    char *path = ecs_get_fullpath(world, it.entities[i]);
                    ecs_dbg("#[green]system#[reset] %s", path);
                    ecs_os_free(path);
                }

                ran_since_merge ++;
                if (ran_since_merge == op[op_index].count) {
                    ecs_dbg("#[magenta]merge#[reset]");
                    ecs_log_pop_1();
                    ran_since_merge = 0;
                    op_index ++;
                    if (op_index < ecs_vector_count(ops)) {
                        ecs_dbg(
                            "#[green]schedule#[reset]: "
                            "threading: %d, staging: %d:",
                            op[op_index].multi_threaded, 
                            !op[op_index].no_readonly);
                    }
                    ecs_log_push_1();
                }

                if (sys->last_frame == (world->info.frame_count_total + 1)) {
                    last_system = it.entities[i];

                    /* Can't break from loop yet. It's possible that previously
                     * inactive systems that ran before the last ran system are 
                     * now active. */
                }
            }
        }

        ecs_log_pop_1();
        ecs_log_pop_1();
    }

    /* Force sort of query as this could increase the match_count */
    pq->match_count = pq->query->match_count;
    pq->ops = ops;
    pq->last_system = last_system;

    return true;
}

void flecs_pipeline_fini_iter(
    ecs_pipeline_state_t *pq)
{
    int32_t i, iter_count = pq->iter_count;
    for (i = 0; i < iter_count; i ++) {
        ecs_iter_fini(&pq->iters[i]);
    }
}

void flecs_pipeline_reset_iter(
    ecs_world_t *world,
    ecs_pipeline_state_t *pq)
{
    ecs_pipeline_op_t *op = ecs_vector_first(pq->ops, ecs_pipeline_op_t);
    int32_t i, ran_since_merge = 0, op_index = 0, iter_count = pq->iter_count;

    flecs_pipeline_fini_iter(pq);

    if (!pq->last_system) {
        /* It's possible that all systems that were ran were removed entirely
         * from the pipeline (they could have been deleted or disabled). In that
         * case (which should be very rare) the pipeline can't make assumptions
         * about where to continue, so end frame. */
        pq->cur_i = -1;
        pq->ran_since_merge = 0;
        return;
    }

    /* Create new iterators */
    for (i = 0; i < iter_count; i ++) {
        ecs_world_t *stage = ecs_get_stage(world, i);
        pq->iters[i] = ecs_query_iter(stage, pq->query);
    }

    /* Move iterator to last ran system */
    ecs_iter_t *it = &pq->iters[0];
    while (ecs_query_next(it)) {
        /* Progress worker iterators */
        for (i = 1; i < iter_count; i ++) {
            bool hasnext = ecs_query_next(&pq->iters[i]);
            ecs_assert_var(hasnext, ECS_INTERNAL_ERROR, NULL);
            ecs_assert(it->table == pq->iters[i].table, 
                ECS_INTERNAL_ERROR, NULL);
            ecs_assert(it->count == pq->iters[i].count,
                ECS_INTERNAL_ERROR, NULL);
        }

        if (flecs_pipeline_is_inactive(pq, it->table)) {
            continue;
        }

        for (i = 0; i < it->count; i ++) {
            ran_since_merge ++;
            if (ran_since_merge == op[op_index].count) {
                ran_since_merge = 0;
                op_index ++;
            }

            if (it->entities[i] == pq->last_system) {
                pq->cur_op = &op[op_index];
                pq->cur_i = i;
                pq->ran_since_merge = ran_since_merge;
                return;
            }
        }
    }

    ecs_abort(ECS_INTERNAL_ERROR, NULL);
}

bool flecs_pipeline_update(
    ecs_world_t *world,
    ecs_pipeline_state_t *pq,
    bool start_of_frame)
{
    ecs_poly_assert(world, ecs_world_t);
    ecs_assert(!(world->flags & EcsWorldReadonly), ECS_INVALID_OPERATION, NULL);

    /* If any entity mutations happened that could have affected query matching
     * notify appropriate queries so caches are up to date. This includes the
     * pipeline query. */
    if (start_of_frame) {
        ecs_run_aperiodic(world, 0);
    }

    ecs_assert(pq != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(pq->query != NULL, ECS_INTERNAL_ERROR, NULL);

    bool rebuilt = flecs_pipeline_build(world, pq);
     if (start_of_frame) {
        /* Initialize iterators */
        int32_t i, count = pq->iter_count;
        for (i = 0; i < count; i ++) {
            ecs_world_t *stage = ecs_get_stage(world, i);
            pq->iters[i] = ecs_query_iter(stage, pq->query);
        }
        pq->cur_op = ecs_vector_first(pq->ops, ecs_pipeline_op_t);
    } else if (rebuilt) {
        /* If pipeline got updated and we were mid frame, reset iterators */
        flecs_pipeline_reset_iter(world, pq);
        return true;
    } else {
        pq->cur_op += 1;
    }

    return false;
}

void ecs_run_pipeline(
    ecs_world_t *world,
    ecs_entity_t pipeline,
    ecs_ftime_t delta_time)
{
    if (!pipeline) {
        pipeline = world->pipeline;
    }

    EcsPipeline *pq = (EcsPipeline*)ecs_get(world, pipeline, EcsPipeline);
    flecs_run_pipeline(world, pq->state, delta_time);
}

void flecs_run_pipeline(
    ecs_world_t *world,
    ecs_pipeline_state_t *pq,
    ecs_ftime_t delta_time)
{
    ecs_assert(world != NULL, ECS_INVALID_OPERATION, NULL);
    ecs_assert(pq != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(pq->query != NULL, ECS_INTERNAL_ERROR, NULL);

    /* If the world is passed to ecs_run_pipeline, the function will take care
     * of staging, so the world should not be in staged mode when called. */
    if (ecs_poly_is(world, ecs_world_t)) {
        ecs_assert(!(world->flags & EcsWorldReadonly), 
            ECS_INVALID_OPERATION, NULL);

        /* Forward to worker_progress. This function handles staging, threading
         * and synchronization across workers. */
        flecs_workers_progress(world, pq, delta_time);
        return;

    /* If a stage is passed, the function could be ran from a worker thread. In
     * that case the main thread should manage staging, and staging should be
     * enabled. */
    } else {
        ecs_poly_assert(world, ecs_stage_t);
    }

    ecs_stage_t *stage = flecs_stage_from_world(&world);  
    ecs_vector_t *ops = pq->ops;
    ecs_pipeline_op_t *op = ecs_vector_first(ops, ecs_pipeline_op_t);
    ecs_pipeline_op_t *op_last = ecs_vector_last(ops, ecs_pipeline_op_t);
    int32_t ran_since_merge = 0;

    int32_t stage_index = ecs_get_stage_id(stage->thread_ctx);
    int32_t stage_count = ecs_get_stage_count(world);

    flecs_worker_begin(stage->thread_ctx);

    ecs_time_t st = {0};
    bool measure_time = false;
    if (!stage_index && (world->flags & EcsWorldMeasureSystemTime)) {
        ecs_time_measure(&st);
        measure_time = true;
    }

    ecs_iter_t *it = &pq->iters[stage_index];
    while (ecs_query_next(it)) {
        if (flecs_pipeline_is_inactive(pq, it->table)) {
            continue; /* Skip inactive systems */
        }

        EcsPoly *poly = flecs_pipeline_term_system(it);
        int32_t i, count = it->count;
        for(i = 0; i < count; i ++) {
            ecs_entity_t e = it->entities[i];
            ecs_system_t *sys = ecs_poly(poly[i].poly, ecs_system_t);

            ecs_assert(sys->entity == e, ECS_INTERNAL_ERROR, NULL);

            sys->last_frame = world->info.frame_count_total + 1;

            if (!stage_index || op->multi_threaded) {
                ecs_stage_t *s = NULL;
                if (!op->no_readonly) {
                    s = stage;
                }

                ecs_run_intern(world, s, e, sys, stage_index, 
                    stage_count, delta_time, 0, 0, NULL);
            }

            ran_since_merge ++;
            world->info.systems_ran_frame ++;

            if (ran_since_merge == op->count) {
                if (op == op_last) {
                    ecs_iter_fini(it);
                    goto done;
                }

                if (measure_time) {
                    /* Don't include merge time in system time */
                    world->info.system_time_total += 
                        (ecs_ftime_t)ecs_time_measure(&st);
                }

                ran_since_merge = 0;

                /* If the set of matched systems changed as a result of the
                 * merge, we have to reset the iterator and move it to our
                 * current position (system). If there are a lot of systems
                 * in the pipeline this can be an expensive operation, but
                 * should happen infrequently. */
                bool rebuild = flecs_worker_sync(world, pq);
                if (rebuild) {
                    i = pq->cur_i;
                    ran_since_merge = pq->ran_since_merge;
                    count = it->count;
                }

                poly = flecs_pipeline_term_system(it);
                op = pq->cur_op;
                op_last = ecs_vector_last(pq->ops, ecs_pipeline_op_t);

                if (measure_time) {
                    /* Reset timer after merge */
                    ecs_time_measure(&st);
                }
            }
        }
    }

done:
    if (measure_time) {
        world->info.system_time_total += (ecs_ftime_t)ecs_time_measure(&st);
    }

    flecs_worker_end(stage->thread_ctx);
}

bool ecs_progress(
    ecs_world_t *world,
    ecs_ftime_t user_delta_time)
{
    ecs_ftime_t delta_time = ecs_frame_begin(world, user_delta_time);
    
    ecs_dbg_3("#[bold]progress#[reset](dt = %.2f)", (double)delta_time);
    ecs_log_push_3();
    ecs_run_pipeline(world, 0, delta_time);
    ecs_log_pop_3();

    ecs_frame_end(world);

    return !ECS_BIT_IS_SET(world->flags, EcsWorldQuit);
}

void ecs_set_time_scale(
    ecs_world_t *world,
    ecs_ftime_t scale)
{
    world->info.time_scale = scale;
}

void ecs_reset_clock(
    ecs_world_t *world)
{
    world->info.world_time_total = 0;
    world->info.world_time_total_raw = 0;
}

void ecs_set_pipeline(
    ecs_world_t *world,
    ecs_entity_t pipeline)
{
    ecs_poly_assert(world, ecs_world_t);
    ecs_check( ecs_get(world, pipeline, EcsPipeline) != NULL, 
        ECS_INVALID_PARAMETER, "not a pipeline");

    int32_t thread_count = ecs_get_stage_count(world);
    if (thread_count > 1) {
        ecs_set_threads(world, 1);
    }
    world->pipeline = pipeline;
    if (thread_count > 1) {
        ecs_set_threads(world, thread_count);
    }
error:
    return;
}

ecs_entity_t ecs_get_pipeline(
    const ecs_world_t *world)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    world = ecs_get_world(world);
    return world->pipeline;
error:
    return 0;
}

ecs_entity_t ecs_pipeline_init(
    ecs_world_t *world,
    const ecs_pipeline_desc_t *desc)
{
    ecs_poly_assert(world, ecs_world_t);
    ecs_assert(desc != NULL, ECS_INVALID_PARAMETER, NULL);

    ecs_entity_t result = desc->entity;
    if (!result) {
        result = ecs_new(world, 0);
    }

    ecs_query_desc_t qd = desc->query;
    if (!qd.order_by) {
        qd.order_by = flecs_entity_compare;
    }
    qd.entity = result;

    ecs_query_t *query = ecs_query_init(world, &qd);
    if (!query) {
        ecs_delete(world, result);
        return 0;
    }

    ecs_assert(query->filter.terms[0].id == EcsSystem,
        ECS_INVALID_PARAMETER, NULL);

    ecs_pipeline_state_t *pq = ecs_os_calloc_t(ecs_pipeline_state_t);
    pq->query = query;
    pq->match_count = -1;
    pq->idr_inactive = flecs_id_record_ensure(world, EcsEmpty);
    ecs_set(world, result, EcsPipeline, { pq });

    return result;
}

/* -- Module implementation -- */

static
void FlecsPipelineFini(
    ecs_world_t *world,
    void *ctx)
{
    (void)ctx;
    if (ecs_get_stage_count(world)) {
        ecs_set_threads(world, 0);
    }

    ecs_assert(world->workers_running == 0, ECS_INTERNAL_ERROR, NULL);
}

#define flecs_bootstrap_phase(world, phase, depends_on)\
    flecs_bootstrap_tag(world, phase);\
    _flecs_bootstrap_phase(world, phase, depends_on)
static
void _flecs_bootstrap_phase(
    ecs_world_t *world,
    ecs_entity_t phase,
    ecs_entity_t depends_on)
{
    ecs_add_id(world, phase, EcsPhase);
    if (depends_on) {
        ecs_add_pair(world, phase, EcsDependsOn, depends_on);
    }
}

void FlecsPipelineImport(
    ecs_world_t *world)
{
    ECS_MODULE(world, FlecsPipeline);
    ECS_IMPORT(world, FlecsSystem);

    ecs_set_name_prefix(world, "Ecs");

    flecs_bootstrap_component(world, EcsPipeline);
    flecs_bootstrap_tag(world, EcsPhase);

    flecs_bootstrap_phase(world, EcsPreFrame,   0);
    flecs_bootstrap_phase(world, EcsOnLoad,     EcsPreFrame);
    flecs_bootstrap_phase(world, EcsPostLoad,   EcsOnLoad);
    flecs_bootstrap_phase(world, EcsPreUpdate,  EcsPostLoad);
    flecs_bootstrap_phase(world, EcsOnUpdate,   EcsPreUpdate);
    flecs_bootstrap_phase(world, EcsOnValidate, EcsOnUpdate);
    flecs_bootstrap_phase(world, EcsPostUpdate, EcsOnValidate);
    flecs_bootstrap_phase(world, EcsPreStore,   EcsPostUpdate);
    flecs_bootstrap_phase(world, EcsOnStore,    EcsPreStore);
    flecs_bootstrap_phase(world, EcsPostFrame,  EcsOnStore);

    ecs_set_hooks(world, EcsPipeline, {
        .ctor = ecs_default_ctor,
        .dtor = ecs_dtor(EcsPipeline)
    });

    world->pipeline = ecs_pipeline_init(world, &(ecs_pipeline_desc_t){
        .entity = ecs_entity(world, { .name = "BuiltinPipeline" }),
        .query = {
            .filter.terms = {
                { .id = EcsSystem },
                { .id = EcsPhase, .src.flags = EcsCascade, .src.trav = EcsDependsOn }
            },
            .order_by = flecs_entity_compare
        }
    });

    /* Cleanup thread administration when world is destroyed */
    ecs_atfini(world, FlecsPipelineFini, NULL);
}

#endif
