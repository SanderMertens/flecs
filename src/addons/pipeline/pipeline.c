#include "flecs.h"
#include "../system/system.h"

#ifdef FLECS_PIPELINE
#include "pipeline.h"

static ECS_DTOR(EcsPipelineQuery, ptr, {
    ecs_vector_free(ptr->ops);
})

static
int compare_entity(
    ecs_entity_t e1, 
    const void *ptr1, 
    ecs_entity_t e2, 
    const void *ptr2) 
{
    (void)ptr1;
    (void)ptr2;
    return (e1 > e2) - (e1 < e2);
}

static
uint64_t group_by_phase(
    ecs_world_t *world,
    ecs_type_t type,
    ecs_entity_t pipeline,
    void *ctx) 
{
    (void)ctx;
    
    const EcsType *pt = ecs_get(world, pipeline, EcsType);
    ecs_assert(pt != NULL, ECS_INTERNAL_ERROR, NULL);

    /* Find tag in system that belongs to pipeline */
    ecs_entity_t *sys_comps = ecs_vector_first(type, ecs_entity_t);
    int32_t c, t, count = ecs_vector_count(type);
    
    ecs_type_t pipeline_type = NULL;
    if (pt->normalized) {
        pipeline_type = pt->normalized->type;
    }

    if (!pipeline_type) {
        return 0;
    }

    ecs_entity_t *tags = ecs_vector_first(pipeline_type, ecs_entity_t);
    int32_t tag_count = ecs_vector_count(pipeline_type);

    ecs_entity_t result = 0;

    for (c = 0; c < count; c ++) {
        ecs_entity_t comp = sys_comps[c];
        for (t = 0; t < tag_count; t ++) {
            if (comp == tags[t]) {
                result = comp;
                break;
            }
        }
        if (result) {
            break;
        }
    }

    ecs_assert(result != 0, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(result < INT_MAX, ECS_INTERNAL_ERROR, NULL);

    return result;
}

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
bool build_pipeline(
    ecs_world_t *world,
    ecs_entity_t pipeline,
    EcsPipelineQuery *pq)
{
    (void)pipeline;

    ecs_query_iter(world, pq->query);

    if (pq->match_count == pq->query->match_count) {
        /* No need to rebuild the pipeline */
        return false;
    }

    world->stats.pipeline_build_count_total ++;
    pq->rebuild_count ++;

    write_state_t ws = {
        .components = ecs_map_new(int32_t, ECS_HI_COMPONENT_ID),
        .wildcard = false
    };

    ecs_pipeline_op_t *op = NULL;
    ecs_vector_t *ops = NULL;
    ecs_query_t *query = pq->build_query;

    if (pq->ops) {
        ecs_vector_free(pq->ops);
    }

    bool multi_threaded = false;
    bool no_staging = false;
    bool first = true;

    /* Iterate systems in pipeline, add ops for running / merging */
    ecs_iter_t it = ecs_query_iter(world, query);
    while (ecs_query_next(&it)) {
        EcsSystem *sys = ecs_term(&it, EcsSystem, 1);

        int i;
        for (i = 0; i < it.count; i ++) {      
            ecs_query_t *q = sys[i].query;
            if (!q) {
                continue;
            }

            bool needs_merge = false;
            bool is_active = !ecs_has_id(
                world, it.entities[i], EcsInactive);
            needs_merge = check_terms(&q->filter, is_active, &ws);

            if (is_active) {
                if (first) {
                    multi_threaded = sys[i].multi_threaded;
                    no_staging = sys[i].no_staging;
                    first = false;
                }

                if (sys[i].multi_threaded != multi_threaded) {
                    needs_merge = true;
                    multi_threaded = sys[i].multi_threaded;
                }
                if (sys[i].no_staging != no_staging) {
                    needs_merge = true;
                    no_staging = sys[i].no_staging;
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

    ecs_assert(op != NULL, ECS_INTERNAL_ERROR, NULL);

    /* Add schedule to debug tracing */
    ecs_dbg("#[green]pipeline#[reset] rebuild:");
    ecs_log_push();

    ecs_dbg("#[green]schedule#[reset]: threading: %d, staging: %d:", 
        op->multi_threaded, !op->no_staging);
    ecs_log_push();
    
    it = ecs_query_iter(world, pq->query);
    while (ecs_query_next(&it)) {
        EcsSystem *sys = ecs_term(&it, EcsSystem, 1);
        for (i = 0; i < it.count; i ++) {
#ifndef NDEBUG
            char *path = ecs_get_fullpath(world, it.entities[i]);
            ecs_dbg("#[green]system#[reset] %s", path);
            ecs_os_free(path);
#endif
            ran_since_merge ++;
            if (ran_since_merge == op[op_index].count) {
                ecs_dbg("#[magenta]merge#[reset]");
                ecs_log_pop();
                ran_since_merge = 0;
                op_index ++;
                if (op_index < ecs_vector_count(ops)) {
                    ecs_dbg("#[green]schedule#[reset]: threading: %d, staging: %d:",
                        op[op_index].multi_threaded, !op[op_index].no_staging);
                }
                ecs_log_push();
            }

            if (sys[i].last_frame == (world->stats.frame_count_total + 1)) {
                last_system = it.entities[i];

                /* Can't break from loop yet. It's possible that previously
                 * inactive systems that ran before the last ran system are now
                 * active. */
            }
        }
    }

    ecs_log_pop();
    ecs_log_pop();

    /* Force sort of query as this could increase the match_count */
    pq->match_count = pq->query->match_count;
    pq->ops = ops;
    pq->last_system = last_system;

    return true;
}

int32_t ecs_pipeline_reset_iter(
    ecs_world_t *world,
    const EcsPipelineQuery *pq,
    ecs_iter_t *iter_out,
    ecs_pipeline_op_t **op_out,
    ecs_pipeline_op_t **last_op_out)
{
    ecs_pipeline_op_t *op = ecs_vector_first(pq->ops, ecs_pipeline_op_t);
    int32_t i, ran_since_merge = 0, op_index = 0;

    if (!pq->last_system) {
        /* It's possible that all systems that were ran were removed entirely
         * from the pipeline (they could have been deleted or disabled). In that
         * case (which should be very rare) the pipeline can't make assumptions
         * about where to continue, so end frame. */
        return -1;
    }

    /* Move iterator to last ran system */
    *iter_out = ecs_query_iter(world, pq->query);
    while (ecs_query_next(iter_out)) {
        for (i = 0; i < iter_out->count; i ++) {
            ran_since_merge ++;
            if (ran_since_merge == op[op_index].count) {
                ran_since_merge = 0;
                op_index ++;
            }

            if (iter_out->entities[i] == pq->last_system) {
                *op_out = &op[op_index];
                *last_op_out = ecs_vector_last(pq->ops, ecs_pipeline_op_t);
                return i;
            }
        }
    }

    ecs_abort(ECS_INTERNAL_ERROR, NULL);

    return -1;
}

bool ecs_pipeline_update(
    ecs_world_t *world,
    ecs_entity_t pipeline,
    bool start_of_frame)
{
    ecs_poly_assert(world, ecs_world_t);
    ecs_assert(!world->is_readonly, ECS_INVALID_OPERATION, NULL);
    ecs_assert(pipeline != 0, ECS_INTERNAL_ERROR, NULL);

    /* If any entity mutations happened that could have affected query matching
     * notify appropriate queries so caches are up to date. This includes the
     * pipeline query. */
    if (start_of_frame) {
        flecs_eval_component_monitors(world);
    }

    bool added = false;
    EcsPipelineQuery *pq = ecs_get_mut(world, pipeline, EcsPipelineQuery, &added);
    ecs_assert(added == false, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(pq != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(pq->query != NULL, ECS_INTERNAL_ERROR, NULL);

    return build_pipeline(world, pipeline, pq);
}

void ecs_run_pipeline(
    ecs_world_t *world,
    ecs_entity_t pipeline,
    FLECS_FLOAT delta_time)
{
    ecs_assert(world != NULL, ECS_INVALID_OPERATION, NULL);

    if (!pipeline) {
        pipeline = world->pipeline;
    }

    ecs_assert(pipeline != 0, ECS_INVALID_PARAMETER, NULL);

    /* If the world is passed to ecs_run_pipeline, the function will take care
     * of staging, so the world should not be in staged mode when called. */
    if (ecs_poly_is(world, ecs_world_t)) {
        ecs_assert(!world->is_readonly, ECS_INVALID_OPERATION, NULL);

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
    
    const EcsPipelineQuery *pq = ecs_get(world, pipeline, EcsPipelineQuery);
    ecs_assert(pq != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(pq->query != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_vector_t *ops = pq->ops;
    ecs_pipeline_op_t *op = ecs_vector_first(ops, ecs_pipeline_op_t);
    ecs_pipeline_op_t *op_last = ecs_vector_last(ops, ecs_pipeline_op_t);
    int32_t ran_since_merge = 0;

    int32_t stage_index = ecs_get_stage_id(stage->thread_ctx);
    int32_t stage_count = ecs_get_stage_count(world);

    ecs_worker_begin(stage->thread_ctx);

    ecs_iter_t it = ecs_query_iter(world, pq->query);
    while (ecs_query_next(&it)) {
        EcsSystem *sys = ecs_term(&it, EcsSystem, 1);

        int32_t i;
        for(i = 0; i < it.count; i ++) {
            ecs_entity_t e = it.entities[i];

            if (!stage_index) {
                ecs_dbg_3("pipeline: run system %s", ecs_get_name(world, e));
            }

            if (!stage_index || op->multi_threaded) {
                ecs_stage_t *s = NULL;
                if (!op->no_staging) {
                    s = stage;
                }

                ecs_run_intern(world, s, e, &sys[i], stage_index, 
                    stage_count, delta_time, 0, 0, NULL);
            }

            sys[i].last_frame = world->stats.frame_count_total + 1;

            ran_since_merge ++;
            world->stats.systems_ran_frame ++;

            if (op != op_last && ran_since_merge == op->count) {
                ran_since_merge = 0;

                if (!stage_index) {
                    ecs_dbg_3("merge");
                }

                /* If the set of matched systems changed as a result of the
                 * merge, we have to reset the iterator and move it to our
                 * current position (system). If there are a lot of systems
                 * in the pipeline this can be an expensive operation, but
                 * should happen infrequently. */
                i = ecs_worker_sync(world, pq, &it, i, &op, &op_last);
                sys = ecs_term(&it, EcsSystem, 1);
            }
        }
    }

    ecs_worker_end(stage->thread_ctx);
}

static
void add_pipeline_tags_to_sig(
    ecs_world_t *world,
    ecs_term_t *terms,
    ecs_type_t type)
{
    (void)world;
    
    int32_t i, count = ecs_vector_count(type);
    ecs_entity_t *entities = ecs_vector_first(type, ecs_entity_t);

    for (i = 0; i < count; i ++) {
        terms[i] = (ecs_term_t){
            .inout = EcsIn,
            .oper = EcsOr,
            .pred.entity = entities[i],
            .subj = {
                .entity = EcsThis,
                .set.mask = EcsSelf | EcsSuperSet
            }
        };
    }
}

static
ecs_query_t* build_pipeline_query(
    ecs_world_t *world,
    ecs_entity_t pipeline,
    const char *name,
    bool not_inactive)
{
    const EcsType *type_ptr = ecs_get(world, pipeline, EcsType);
    ecs_assert(type_ptr != NULL, ECS_INTERNAL_ERROR, NULL);
    
    ecs_type_t type = NULL;
    if (type_ptr->normalized) {
        type = type_ptr->normalized->type;
    }

    int32_t type_count = ecs_vector_count(type);
    int32_t term_count = 1;

    if (not_inactive) {
        term_count ++;
    }

    ecs_term_t *terms = ecs_os_malloc(
        (type_count + term_count) * ECS_SIZEOF(ecs_term_t));

    terms[0] = (ecs_term_t){
        .inout = EcsIn,
        .oper = EcsAnd,
        .pred.entity = ecs_id(EcsSystem),
        .subj = {
            .entity = EcsThis,
            .set.mask = EcsSelf | EcsSuperSet
        }
    };

    if (not_inactive) {
        terms[1] = (ecs_term_t){
            .inout = EcsIn,
            .oper = EcsNot,
            .pred.entity = EcsInactive,
            .subj = {
                .entity = EcsThis,
                .set.mask = EcsSelf | EcsSuperSet
            }
        };
    }

    add_pipeline_tags_to_sig(world, &terms[term_count], type);

    ecs_query_t *result = ecs_query_init(world, &(ecs_query_desc_t){
        .filter = {
            .name = name,
            .terms_buffer = terms,
            .terms_buffer_count = term_count + type_count
        },
        .order_by = compare_entity,
        .group_by = group_by_phase,
        .group_by_id = pipeline
    });

    ecs_assert(result != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_os_free(terms);

    return result;
}

static 
void OnUpdatePipeline(
    ecs_iter_t *it)
{
    ecs_world_t *world = it->world;
    ecs_entity_t *entities = it->entities;

    int32_t i;
    for (i = it->count - 1; i >= 0; i --) {
        ecs_entity_t pipeline = entities[i];
        
        ecs_trace("#[green]pipeline#[reset] %s created",
            ecs_get_name(world, pipeline));
        ecs_log_push();

        /* Build signature for pipeline query that matches EcsSystems, has the
         * pipeline phases as OR columns, and ignores systems with EcsInactive. 
         * Note that EcsDisabled is automatically ignored 
         * by the regular query matching */
        ecs_query_t *query = build_pipeline_query(
            world, pipeline, "BuiltinPipelineQuery", true);
        ecs_assert(query != NULL, ECS_INTERNAL_ERROR, NULL);

        /* Build signature for pipeline build query. The build query includes
         * systems that are inactive, as an inactive system may become active as
         * a result of another system, and as a result the correct merge 
         * operations need to be put in place. */
        ecs_query_t *build_query = build_pipeline_query(
            world, pipeline, "BuiltinPipelineBuildQuery", false);
        ecs_assert(build_query != NULL, ECS_INTERNAL_ERROR, NULL);

        bool added = false;
        EcsPipelineQuery *pq = ecs_get_mut(
            world, pipeline, EcsPipelineQuery, &added);
        ecs_assert(pq != NULL, ECS_INTERNAL_ERROR, NULL);

        if (added) {
            /* Should not modify pipeline after it has been used */
            ecs_assert(pq->ops == NULL, ECS_INVALID_OPERATION, NULL);

            if (pq->query) {
                ecs_query_fini(pq->query);
            }
            if (pq->build_query) {
                ecs_query_fini(pq->build_query);
            }
        }

        pq->query = query;
        pq->build_query = build_query;
        pq->match_count = -1;
        pq->ops = NULL;
        pq->last_system = 0;

        ecs_log_pop();
    }
}

/* -- Public API -- */

bool ecs_progress(
    ecs_world_t *world,
    FLECS_FLOAT user_delta_time)
{
    float delta_time = ecs_frame_begin(world, user_delta_time);

    ecs_dbg_3("#[normal]begin progress(dt = %.2f)", (double)delta_time);

    ecs_run_pipeline(world, 0, delta_time);

    ecs_dbg_3("#[normal]end progress");

    ecs_frame_end(world);

    return !world->should_quit;
}

void ecs_set_time_scale(
    ecs_world_t *world,
    FLECS_FLOAT scale)
{
    world->stats.time_scale = scale;
}

void ecs_reset_clock(
    ecs_world_t *world)
{
    world->stats.world_time_total = 0;
    world->stats.world_time_total_raw = 0;
}

void ecs_deactivate_systems(
    ecs_world_t *world)
{
    ecs_assert(!world->is_readonly, ECS_INVALID_WHILE_ITERATING, NULL);

    ecs_entity_t pipeline = world->pipeline;
    const EcsPipelineQuery *pq = ecs_get( world, pipeline, EcsPipelineQuery);
    ecs_assert(pq != NULL, ECS_INTERNAL_ERROR, NULL);

    /* Iterate over all systems, add EcsInvalid tag if queries aren't matched
     * with any tables */
    ecs_iter_t it = ecs_query_iter(world, pq->build_query);

    /* Make sure that we defer adding the inactive tags until after iterating
     * the query */
    flecs_defer_none(world, &world->stage);

    while( ecs_query_next(&it)) {
        EcsSystem *sys = ecs_term(&it, EcsSystem, 1);

        int32_t i;
        for (i = 0; i < it.count; i ++) {
            ecs_query_t *query = sys[i].query;
            if (query) {
                if (!ecs_query_table_count(query)) {
                    ecs_add_id(world, it.entities[i], EcsInactive);
                }
            }
        }
    }

    flecs_defer_flush(world, &world->stage);
}

void ecs_set_pipeline(
    ecs_world_t *world,
    ecs_entity_t pipeline)
{
    ecs_poly_assert(world, ecs_world_t);
    ecs_check( ecs_get(world, pipeline, EcsPipelineQuery) != NULL, 
        ECS_INVALID_PARAMETER, NULL);

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
}

void FlecsPipelineImport(
    ecs_world_t *world)
{
    ECS_MODULE(world, FlecsPipeline);

    ECS_IMPORT(world, FlecsSystem);

    ecs_set_name_prefix(world, "Ecs");

    flecs_bootstrap_tag(world, EcsPipeline);
    flecs_bootstrap_component(world, EcsPipelineQuery);

    /* Phases of the builtin pipeline are regular entities. Names are set so
     * they can be resolved by type expressions. */
    flecs_bootstrap_tag(world, EcsPreFrame);
    flecs_bootstrap_tag(world, EcsOnLoad);
    flecs_bootstrap_tag(world, EcsPostLoad);
    flecs_bootstrap_tag(world, EcsPreUpdate);
    flecs_bootstrap_tag(world, EcsOnUpdate);
    flecs_bootstrap_tag(world, EcsOnValidate);
    flecs_bootstrap_tag(world, EcsPostUpdate);
    flecs_bootstrap_tag(world, EcsPreStore);
    flecs_bootstrap_tag(world, EcsOnStore);
    flecs_bootstrap_tag(world, EcsPostFrame);

    /* Set ctor and dtor for PipelineQuery */
    ecs_set(world, ecs_id(EcsPipelineQuery), EcsComponentLifecycle, {
        .ctor = ecs_default_ctor,
        .dtor = ecs_dtor(EcsPipelineQuery)
    });

    /* When the Pipeline tag is added a pipeline will be created */
    ecs_observer_init(world, &(ecs_observer_desc_t) {
        .entity.name = "OnUpdatePipeline",
        .filter.terms = {
            { .id = EcsPipeline },
            { .id = ecs_id(EcsType) }
        },
        .events = { EcsOnSet },
        .callback = OnUpdatePipeline
    });

    /* Create the builtin pipeline */
    world->pipeline = ecs_type_init(world, &(ecs_type_desc_t){
        .entity = {
            .name = "BuiltinPipeline",
            .add = {EcsPipeline}
        },
        .ids = {
            EcsPreFrame, EcsOnLoad, EcsPostLoad, EcsPreUpdate, EcsOnUpdate,
            EcsOnValidate, EcsPostUpdate, EcsPreStore, EcsOnStore, EcsPostFrame
         }
    });

    /* Cleanup thread administration when world is destroyed */
    ecs_atfini(world, FlecsPipelineFini, NULL);
}

#endif
