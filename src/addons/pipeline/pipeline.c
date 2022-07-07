#include "flecs.h"
#include "../system/system.h"

#ifdef FLECS_PIPELINE
#include "pipeline.h"

static ECS_DTOR(EcsPipeline, ptr, {
    ecs_vector_free(ptr->ops);
    ecs_os_free(ptr->iters);
})

typedef enum ComponentWriteState {
    NotWritten = 0,
    WriteToMain,
    WriteToStage
} ComponentWriteState;

typedef struct write_state_t {
    ecs_map_t *components;
    bool wildcard;
} write_state_t;

static
int32_t get_write_state(
    ecs_map_t *write_state,
    ecs_entity_t component)
{
    int32_t *ptr = ecs_map_get(write_state, int32_t, component);
    if (ptr) {
        return *ptr;
    } else {
        return 0;
    }
}

static
void set_write_state(
    write_state_t *write_state,
    ecs_entity_t component,
    int32_t value)
{
    if (component == EcsWildcard) {
        ecs_assert(value == WriteToStage, ECS_INTERNAL_ERROR, NULL);
        write_state->wildcard = true;
    } else {
        ecs_map_set(write_state->components, component, &value);
    }
}

static
void reset_write_state(
    write_state_t *write_state)
{
    ecs_map_clear(write_state->components);
    write_state->wildcard = false;
}

static
int32_t get_any_write_state(
    write_state_t *write_state)
{
    if (write_state->wildcard) {
        return WriteToStage;
    }

    ecs_map_iter_t it = ecs_map_iter(write_state->components);
    int32_t *elem;
    while ((elem = ecs_map_next(&it, int32_t, NULL))) {
        if (*elem == WriteToStage) {
            return WriteToStage;
        }
    }

    return 0;
}

static
bool check_term_component(
    ecs_term_t *term,
    bool is_active,
    ecs_entity_t component,
    write_state_t *write_state)    
{
    int32_t state = get_write_state(write_state->components, component);

    ecs_term_id_t *subj = &term->subj;

    if ((subj->set.mask & EcsSelf) && subj->entity == EcsThis && term->oper != EcsNot) {
        switch(term->inout) {
        case EcsInOutFilter:
            /* Ignore terms that aren't read/written */
            break;
        case EcsInOutDefault:
        case EcsInOut:
        case EcsIn:
            if (state == WriteToStage || write_state->wildcard) {
                return true;
            }
            // fall through
        case EcsOut:
            if (is_active && term->inout != EcsIn) {
                set_write_state(write_state, component, WriteToMain);
            }
        };

    } else if (!subj->entity || term->oper == EcsNot) {
        bool needs_merge = false;

        switch(term->inout) {
        case EcsInOutDefault:
        case EcsIn:
        case EcsInOut:
            if (state == WriteToStage) {
                needs_merge = true;
            }
            if (component == EcsWildcard) {
                if (get_any_write_state(write_state) == WriteToStage) {
                    needs_merge = true;
                }
            }
            break;
        default:
            break;
        };

        switch(term->inout) {
        case EcsInOutDefault:
            if ((!(subj->set.mask & EcsSelf) || (subj->entity != EcsThis)) && (subj->set.mask != EcsNothing)) {
                /* Default inout behavior is [inout] for This terms, and [in]
                 * for terms that match other entities */
                break;
            }
            // fall through
        case EcsInOut:
        case EcsOut:
            if (is_active) {
                set_write_state(write_state, component, WriteToStage);
            }
            break;
        default:
            break;
        };   

        if (needs_merge) {
            return true;
        }
    }

    return false;
}

static
bool check_term(
    ecs_term_t *term,
    bool is_active,
    write_state_t *write_state)
{
    if (term->oper != EcsOr) {
        return check_term_component(
            term, is_active, term->id, write_state);
    }  

    return false;
}

static
bool check_terms(
    ecs_filter_t *filter,
    bool is_active,
    write_state_t *ws)
{
    bool needs_merge = false;
    ecs_term_t *terms = filter->terms;
    int32_t t, term_count = filter->term_count;

    /* Check This terms first. This way if a term indicating writing to a stage
     * was added before the term, it won't cause merging. */
    for (t = 0; t < term_count; t ++) {
        ecs_term_t *term = &terms[t];
        if (term->subj.entity == EcsThis) {
            needs_merge |= check_term(term, is_active, ws);
        }
    }

    /* Now check staged terms */
    for (t = 0; t < term_count; t ++) {
        ecs_term_t *term = &terms[t];
        if (term->subj.entity != EcsThis) {
            needs_merge |= check_term(term, is_active, ws);
        }
    }

    return needs_merge;
}

static
bool flecs_pipeline_is_inactive(
    const EcsPipeline *pq,
    ecs_table_t *table)
{
    return flecs_id_record_get_table(pq->idr_inactive, table) != NULL;
}

static
EcsPoly* flecs_pipeline_term_system(
    ecs_iter_t *it)
{
    int32_t index = ecs_search(it->world, it->table, ecs_poly_id(EcsSystem), 0);
    ecs_assert(index != -1, ECS_INTERNAL_ERROR, NULL);
    EcsPoly *poly = ecs_table_get_column(it->table, index);
    ecs_assert(poly != NULL, ECS_INTERNAL_ERROR, NULL);
    poly = &poly[it->offset];
    return poly;
}

static
bool flecs_pipeline_build(
    ecs_world_t *world,
    ecs_entity_t pipeline,
    EcsPipeline *pq)
{
    (void)pipeline;

    ecs_query_iter(world, pq->query);

    if (pq->match_count == pq->query->match_count) {
        /* No need to rebuild the pipeline */
        return false;
    }

    world->info.pipeline_build_count_total ++;
    pq->rebuild_count ++;

    write_state_t ws = {
        .components = ecs_map_new(int32_t, ECS_HI_COMPONENT_ID),
        .wildcard = false
    };

    ecs_pipeline_op_t *op = NULL;
    ecs_vector_t *ops = NULL;
    ecs_query_t *query = pq->query;

    if (pq->ops) {
        ecs_vector_free(pq->ops);
    }

    bool multi_threaded = false;
    bool no_staging = false;
    bool first = true;

    /* Iterate systems in pipeline, add ops for running / merging */
    ecs_iter_t it = ecs_query_iter(world, query);
    while (ecs_query_next(&it)) {
        EcsPoly *poly = flecs_pipeline_term_system(&it);

        int i;
        for (i = 0; i < it.count; i ++) {
            ecs_system_t *sys = ecs_poly(poly[i].poly, ecs_system_t);
            ecs_query_t *q = sys->query;
            if (!q) {
                continue;
            }

            bool needs_merge = false;
            bool is_active = !ecs_has_id(
                world, it.entities[i], EcsEmpty);
            needs_merge = check_terms(&q->filter, is_active, &ws);

            if (is_active) {
                if (first) {
                    multi_threaded = sys->multi_threaded;
                    no_staging = sys->no_staging;
                    first = false;
                }

                if (sys->multi_threaded != multi_threaded) {
                    needs_merge = true;
                    multi_threaded = sys->multi_threaded;
                }
                if (sys->no_staging != no_staging) {
                    needs_merge = true;
                    no_staging = sys->no_staging;
                }
            }

            if (needs_merge) {
                /* After merge all components will be merged, so reset state */
                reset_write_state(&ws);
                op = NULL;

                /* Re-evaluate columns to set write flags if system is active.
                 * If system is inactive, it can't write anything and so it
                 * should not insert unnecessary merges.  */
                needs_merge = false;
                if (is_active) {
                    needs_merge = check_terms(&q->filter, true, &ws);
                }

                /* The component states were just reset, so if we conclude that
                 * another merge is needed something is wrong. */
                ecs_assert(needs_merge == false, ECS_INTERNAL_ERROR, NULL);        
            }

            if (!op) {
                op = ecs_vector_add(&ops, ecs_pipeline_op_t);
                op->count = 0;
                op->multi_threaded = false;
                op->no_staging = false;
            }

            /* Don't increase count for inactive systems, as they are ignored by
             * the query used to run the pipeline. */
            if (is_active) {
                if (!op->count) {
                    op->multi_threaded = multi_threaded;
                    op->no_staging = no_staging;
                }
                op->count ++;
            }
        }
    }

    ecs_map_free(ws.components);

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
            op->multi_threaded, !op->no_staging);
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
                            !op[op_index].no_staging);
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

void ecs_pipeline_reset_iter(
    ecs_world_t *world,
    EcsPipeline *pq)
{
    ecs_pipeline_op_t *op = ecs_vector_first(pq->ops, ecs_pipeline_op_t);
    int32_t i, ran_since_merge = 0, op_index = 0, iter_count = pq->iter_count;

    /* Free state of existing iterators */
    for (i = 0; i < iter_count; i ++) {
        ecs_iter_fini(&pq->iters[i]);
    }

    if (!pq->last_system) {
        /* It's possible that all systems that were ran were removed entirely
         * from the pipeline (they could have been deleted or disabled). In that
         * case (which should be very rare) the pipeline can't make assumptions
         * about where to continue, so end frame. */
        pq->cur_i = -1;
        return;
    }

    /* Create new iterators */
    for (i = 0; i < iter_count; i ++) {
        pq->iters[i] = ecs_query_iter(world, pq->query);
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
                return;
            }
        }
    }

    ecs_abort(ECS_INTERNAL_ERROR, NULL);
}

bool ecs_pipeline_update(
    ecs_world_t *world,
    ecs_entity_t pipeline,
    bool start_of_frame)
{
    ecs_poly_assert(world, ecs_world_t);
    ecs_assert(!(world->flags & EcsWorldReadonly), ECS_INVALID_OPERATION, NULL);
    ecs_assert(pipeline != 0, ECS_INTERNAL_ERROR, NULL);

    /* If any entity mutations happened that could have affected query matching
     * notify appropriate queries so caches are up to date. This includes the
     * pipeline query. */
    if (start_of_frame) {
        ecs_run_aperiodic(world, 0);
    }

    EcsPipeline *pq = ecs_get_mut(world, pipeline, EcsPipeline);
    ecs_assert(pq != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(pq->query != NULL, ECS_INTERNAL_ERROR, NULL);

    bool rebuilt = flecs_pipeline_build(world, pipeline, pq);
     if (start_of_frame) {
        /* Initialize iterators */
        int32_t i, count = pq->iter_count;
        for (i = 0; i < count; i ++) {
            pq->iters[i] = ecs_query_iter(world, pq->query);
        }
        pq->cur_op = ecs_vector_first(pq->ops, ecs_pipeline_op_t);
    } else if (rebuilt) {
        /* If pipeline got updated and we were mid frame, reset iterators */
        ecs_pipeline_reset_iter(world, pq);
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
    ecs_assert(world != NULL, ECS_INVALID_OPERATION, NULL);

    if (!pipeline) {
        pipeline = world->pipeline;
    }

    ecs_assert(pipeline != 0, ECS_INVALID_PARAMETER, NULL);

    /* If the world is passed to ecs_run_pipeline, the function will take care
     * of staging, so the world should not be in staged mode when called. */
    if (ecs_poly_is(world, ecs_world_t)) {
        ecs_assert(!(world->flags & EcsWorldReadonly), 
            ECS_INVALID_OPERATION, NULL);

        /* Forward to worker_progress. This function handles staging, threading
         * and synchronization across workers. */
        ecs_workers_progress(world, pipeline, delta_time);
        return;

    /* If a stage is passed, the function could be ran from a worker thread. In
     * that case the main thread should manage staging, and staging should be
     * enabled. */
    } else {
        ecs_poly_assert(world, ecs_stage_t);
    }

    ecs_stage_t *stage = flecs_stage_from_world(&world);  

    EcsPipeline *pq = (EcsPipeline*)ecs_get(world, pipeline, EcsPipeline);
    ecs_assert(pq != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(pq->query != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_vector_t *ops = pq->ops;
    ecs_pipeline_op_t *op = ecs_vector_first(ops, ecs_pipeline_op_t);
    ecs_pipeline_op_t *op_last = ecs_vector_last(ops, ecs_pipeline_op_t);
    int32_t ran_since_merge = 0;

    int32_t stage_index = ecs_get_stage_id(stage->thread_ctx);
    int32_t stage_count = ecs_get_stage_count(world);

    ecs_worker_begin(stage->thread_ctx);

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
                if (!op->no_staging) {
                    s = stage;
                }

                ecs_run_intern(world, s, e, sys, stage_index, 
                    stage_count, delta_time, 0, 0, NULL);
            }

            ran_since_merge ++;
            world->info.systems_ran_frame ++;

            if (op != op_last && ran_since_merge == op->count) {
                ran_since_merge = 0;

                /* If the set of matched systems changed as a result of the
                 * merge, we have to reset the iterator and move it to our
                 * current position (system). If there are a lot of systems
                 * in the pipeline this can be an expensive operation, but
                 * should happen infrequently. */
                bool rebuild = ecs_worker_sync(world, pq);
                pq = (EcsPipeline*)ecs_get(world, pipeline, EcsPipeline);
                if (rebuild) {
                    i = pq->cur_i;
                    count = it->count;
                }

                poly = flecs_pipeline_term_system(it);
                op = pq->cur_op;
                op_last = ecs_vector_last(pq->ops, ecs_pipeline_op_t);
            }
        }
    }

    ecs_worker_end(stage->thread_ctx);
}

bool ecs_progress(
    ecs_world_t *world,
    ecs_ftime_t user_delta_time)
{
    float delta_time = ecs_frame_begin(world, user_delta_time);
    
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

    world->pipeline = pipeline;
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

    ecs_entity_t result = ecs_entity_init(world, &desc->entity);
    if (!result) {
        return 0;
    }

    ecs_query_desc_t qd = desc->query;
    if (!qd.order_by) {
        qd.order_by = flecs_entity_compare;
    }

    ecs_query_t *query = ecs_query_init(world, &qd);
    if (!query) {
        ecs_delete(world, result);
        return 0;
    }

    ecs_assert(query->filter.terms[0].id == EcsSystem,
        ECS_INVALID_PARAMETER, NULL);

    ecs_set(world, result, EcsPipeline, {
        .query = query,
        .match_count = -1,
        .idr_inactive = flecs_id_record_ensure(world, EcsEmpty)
    });

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
        .entity = { .name = "BuiltinPipeline" },
        .query = {
            .filter.terms = {
                { .id = EcsSystem },
                { .id = EcsPhase, .subj.set = { 
                    .mask = EcsCascade, .relation = EcsDependsOn
                }}
            },
            .order_by = flecs_entity_compare
        }
    });

    /* Cleanup thread administration when world is destroyed */
    ecs_atfini(world, FlecsPipelineFini, NULL);
}

#endif
