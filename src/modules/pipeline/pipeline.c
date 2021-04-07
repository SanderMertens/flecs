#include "flecs.h"

#ifdef FLECS_PIPELINE

#include "pipeline.h"

ECS_TYPE_DECL(EcsPipelineQuery);

static ECS_CTOR(EcsPipelineQuery, ptr, {
    memset(ptr, 0, _size);
})

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
int rank_phase(
    ecs_world_t *world,
    ecs_entity_t rank_component,
    ecs_type_t type) 
{
    const EcsType *pipeline_type = ecs_get(world, rank_component, EcsType);
    ecs_assert(pipeline_type != NULL, ECS_INTERNAL_ERROR, NULL);

    /* Find tag in system that belongs to pipeline */
    ecs_entity_t *sys_comps = ecs_vector_first(type, ecs_entity_t);
    int32_t c, t, count = ecs_vector_count(type);

    ecs_entity_t *tags = ecs_vector_first(pipeline_type->normalized, ecs_entity_t);
    int32_t tag_count = ecs_vector_count(pipeline_type->normalized);

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

    return (int)result;
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
bool check_column_component(
    ecs_sig_column_t *column,
    bool is_active,
    ecs_entity_t component,
    write_state_t *write_state)    
{
    int32_t state = get_write_state(write_state->components, component);

    if ((column->from_kind == EcsFromAny || column->from_kind == EcsFromOwned) 
      && column->oper_kind != EcsOperNot) 
    {
        switch(column->inout_kind) {
        case EcsInOut:
        case EcsIn:
            if (state == WriteToStage || write_state->wildcard) {
                return true;
            }
            // fall through
        case EcsOut:
            if (is_active && column->inout_kind != EcsIn) {
                set_write_state(write_state, component, WriteToMain);
            }
        };
    } else if (column->from_kind == EcsFromEmpty || 
               column->oper_kind == EcsOperNot) 
    {
        bool needs_merge = false;

        switch(column->inout_kind) {
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

        switch(column->inout_kind) {
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
bool check_column(
    ecs_sig_column_t *column,
    bool is_active,
    write_state_t *write_state)
{
    if (column->oper_kind != EcsOperOr) {
        return check_column_component(
            column, is_active, column->is.component, write_state);
    }  

    return false;
}

static
bool build_pipeline(
    ecs_world_t *world,
    ecs_entity_t pipeline,
    EcsPipelineQuery *pq)
{
    (void)pipeline;

    ecs_query_iter(pq->query);

    if (pq->match_count == pq->query->match_count) {
        /* No need to rebuild the pipeline */
        return false;
    }

    ecs_trace_2("rebuilding pipeline #[green]%s", 
        ecs_get_name(world, pipeline));

    world->stats.pipeline_build_count_total ++;

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

    /* Iterate systems in pipeline, add ops for running / merging */
    ecs_iter_t it = ecs_query_iter(query);
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

            ecs_vector_each(q->sig.columns, ecs_sig_column_t, column, {
                needs_merge |= check_column(column, is_active, &ws);
            });

            if (needs_merge) {
                /* After merge all components will be merged, so reset state */
                reset_write_state(&ws);
                op = NULL;

                /* Re-evaluate columns to set write flags if system is active.
                 * If system is inactive, it can't write anything and so it
                 * should not insert unnecessary merges.  */
                needs_merge = false;
                if (is_active) {
                    ecs_vector_each(q->sig.columns, ecs_sig_column_t, column, {
                        needs_merge |= check_column(column, true, &ws);
                    });
                }

                /* The component states were just reset, so if we conclude that
                 * another merge is needed something is wrong. */
                ecs_assert(needs_merge == false, ECS_INTERNAL_ERROR, NULL);        
            }

            if (!op) {
                op = ecs_vector_add(&ops, ecs_pipeline_op_t);
                op->count = 0;
            }

            /* Don't increase count for inactive systems, as they are ignored by
             * the query used to run the pipeline. */
            if (is_active) {
                op->count ++;
            }
        }
    }

    ecs_map_free(ws.components);

    /* Force sort of query as this could increase the match_count */
    pq->match_count = pq->query->match_count;
    pq->ops = ops;

    return true;
}

static
int32_t iter_reset(
    const EcsPipelineQuery *pq,
    ecs_iter_t *iter_out,
    ecs_pipeline_op_t **op_out,
    ecs_entity_t move_to)
{
    ecs_pipeline_op_t *op = ecs_vector_first(pq->ops, ecs_pipeline_op_t);
    int32_t ran_since_merge = 0;

    ecs_iter_t it = ecs_query_iter(pq->query);
    while (ecs_query_next(&it)) {
        int32_t i;
        for(i = 0; i < it.count; i ++) {
            ecs_entity_t e = it.entities[i];

            ran_since_merge ++;
            if (ran_since_merge == op->count) {
                ran_since_merge = 0;
                op ++;
            }

            if (e == move_to) {
                *iter_out = it;
                *op_out = op;
                return i;
            }
        }
    }

    ecs_abort(ECS_UNSUPPORTED, NULL);

    return -1;
}

int32_t ecs_pipeline_update(
    ecs_world_t *world,
    ecs_entity_t pipeline,
    bool start_of_frame)
{
    ecs_assert(world != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(world->magic == ECS_WORLD_MAGIC, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(!world->is_readonly, ECS_INVALID_OPERATION, NULL);
    ecs_assert(pipeline != 0, ECS_INTERNAL_ERROR, NULL);

    /* If any entity mutations happened that could have affected query matching
     * notify appropriate queries so caches are up to date. This includes the
     * pipeline query. */
    if (start_of_frame) {
        ecs_eval_component_monitors(world);
    }

    EcsPipelineQuery *pq = ecs_get_mut(world, pipeline, EcsPipelineQuery, NULL);
    ecs_assert(pq != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(pq->query != NULL, ECS_INTERNAL_ERROR, NULL);

    build_pipeline(world, pipeline, pq);

    return ecs_vector_count(pq->ops);
}

void ecs_pipeline_run(
    ecs_world_t *world,
    ecs_entity_t pipeline,
    FLECS_FLOAT delta_time)
{
    ecs_assert(world != NULL, ECS_INVALID_OPERATION, NULL);

    if (!pipeline) {
        pipeline = world->pipeline;
    }    

    /* If the world is passed to ecs_pipeline_run, the function will take care
     * of staging, so the world should not be in staged mode when called. */
    if (world->magic == ECS_WORLD_MAGIC) {
        ecs_assert(!world->is_readonly, ECS_INVALID_OPERATION, NULL);

        /* Forward to worker_progress. This function handles staging, threading
         * and synchronization across workers. */
        ecs_workers_progress(world, pipeline, delta_time);
        return;

    /* If a stage is passed, the function could be ran from a worker thread. In
     * that case the main thread should manage staging, and staging should be
     * enabled. */
    } else {
        ecs_assert(world->magic == ECS_STAGE_MAGIC, ECS_INVALID_PARAMETER, NULL);
    }

    ecs_stage_t *stage = ecs_stage_from_world(&world);  
    
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
    
    ecs_iter_t it = ecs_query_iter(pq->query);
    while (ecs_query_next(&it)) {
        EcsSystem *sys = ecs_term(&it, EcsSystem, 1);

        int32_t i;
        for(i = 0; i < it.count; i ++) {
            ecs_entity_t e = it.entities[i];

            ecs_run_intern(world, stage, e, &sys[i], stage_index, stage_count, 
                delta_time, 0, 0, NULL, NULL);

            ran_since_merge ++;
            world->stats.systems_ran_frame ++;

            if (op != op_last && ran_since_merge == op->count) {
                ran_since_merge = 0;
                op++;

                /* If the set of matched systems changed as a result of the
                 * merge, we have to reset the iterator and move it to our
                 * current position (system). If there are a lot of systems
                 * in the pipeline this can be an expensive operation, but
                 * should happen infrequently. */
                if (ecs_worker_sync(stage->thread_ctx)) {
                    i = iter_reset(pq, &it, &op, e);
                    op_last = ecs_vector_last(pq->ops, ecs_pipeline_op_t);
                    sys = ecs_term(&it, EcsSystem, 1);
                }
            }
        }
    }

    ecs_worker_end(stage->thread_ctx);
}

static
void add_pipeline_tags_to_sig(
    ecs_world_t *world,
    ecs_sig_t *sig,
    ecs_type_t type)
{
    (void)world;
    
    int32_t i, count = ecs_vector_count(type);
    ecs_entity_t *entities = ecs_vector_first(type, ecs_entity_t);

    for (i = 0; i < count; i ++) {
        if (!i) {
            ecs_sig_add(world, sig, EcsFromAny, EcsOperAnd, EcsIn, entities[i], 
                0, NULL, NULL, 0, NULL);
        } else {
            ecs_sig_add(world, sig, EcsFromAny, EcsOperOr, EcsIn, entities[i], 
                0, NULL, NULL, 0, NULL);
        }
    }
}

static 
void EcsOnAddPipeline(
    ecs_iter_t *it)
{
    ecs_world_t *world = it->world;
    ecs_entity_t *entities = it->entities;

    int32_t i;
    for (i = it->count - 1; i >= 0; i --) {
        ecs_entity_t pipeline = entities[i];
        ecs_sig_t sig = { 0 };

        const EcsType *type_ptr = ecs_get(world, pipeline, EcsType);
        ecs_assert(type_ptr != NULL, ECS_INTERNAL_ERROR, NULL);
        
#ifndef NDEBUG
        char *str = ecs_type_str(world, type_ptr->normalized);
        ecs_trace_1("pipeline #[green]%s#[normal] created with #[red][%s]",
            ecs_get_name(world, pipeline), str);
        ecs_os_free(str);
#endif
        ecs_log_push();

        /* Build signature for pipeline quey that matches EcsSystems, has the
         * pipeline as a XOR column, and ignores systems with EcsInactive and
         * EcsDisabledIntern. Note that EcsDisabled is automatically ignored by
         * the regular query matching */
        ecs_sig_add(world, &sig, EcsFromAny, EcsOperAnd, EcsIn, 
            ecs_id(EcsSystem), 0, NULL, NULL, 0, NULL);
        ecs_sig_add(world, &sig, EcsFromAny, EcsOperNot, EcsIn, EcsInactive, 0, 
            NULL, NULL, 0, NULL);
        ecs_sig_add(world, &sig, EcsFromAny, EcsOperNot, EcsIn, 
            EcsDisabledIntern, 0, NULL, NULL, 0, NULL);
        add_pipeline_tags_to_sig(world, &sig, type_ptr->normalized);

        /* Create the query. Sort the query by system id and phase */
        ecs_query_t *query = ecs_query_new_w_sig(world, 0, &sig);
        ecs_query_order_by(world, query, 0, compare_entity);
        ecs_query_group_by(world, query, pipeline, rank_phase);

        /* Build signature for pipeline build query. The build query includes
         * systems that are inactive, as an inactive system may become active as
         * a result of another system, and as a result the correct merge 
         * operations need to be put in place. */
        ecs_sig_add(world, &sig, EcsFromAny, EcsOperAnd, EcsIn, 
            ecs_id(EcsSystem), 0, NULL, NULL, 0, NULL);
        ecs_sig_add(world, &sig, EcsFromAny, EcsOperNot, EcsIn, 
            EcsDisabledIntern, 0, NULL, NULL, 0, NULL);
        add_pipeline_tags_to_sig(world, &sig, type_ptr->normalized);

        /* Use the same sorting functions for the build query */
        ecs_query_t *build_query = ecs_query_new_w_sig(world, 0, &sig);
        ecs_query_order_by(world, build_query, 0, compare_entity);
        ecs_query_group_by(world, build_query, pipeline, rank_phase);       

        EcsPipelineQuery *pq = ecs_get_mut(
            world, pipeline, EcsPipelineQuery, NULL);
        ecs_assert(pq != NULL, ECS_INTERNAL_ERROR, NULL);

        pq->query = query;
        pq->build_query = build_query;
        pq->match_count = -1;
        pq->ops = NULL;

        ecs_log_pop();
    }
}

/* -- Public API -- */

bool ecs_progress(
    ecs_world_t *world,
    FLECS_FLOAT user_delta_time)
{
    float delta_time = ecs_frame_begin(world, user_delta_time);

    ecs_pipeline_run(world, 0, delta_time);

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
    ecs_iter_t it = ecs_query_iter(pq->build_query);

    /* Make sure that we defer adding the inactive tags until after iterating
     * the query */
    ecs_defer_none(world, &world->stage);

    while( ecs_query_next(&it)) {
        EcsSystem *sys = ecs_term(&it, EcsSystem, 1);

        int32_t i;
        for (i = 0; i < it.count; i ++) {
            ecs_query_t *query = sys[i].query;
            if (query) {
                if (!ecs_vector_count(query->tables)) {
                    ecs_add_id(world, it.entities[i], EcsInactive);
                }
            }
        }
    }

    ecs_defer_flush(world, &world->stage);
}

void ecs_set_pipeline(
    ecs_world_t *world,
    ecs_entity_t pipeline)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(world->magic == ECS_WORLD_MAGIC, ECS_INVALID_PARAMETER, NULL);
    ecs_assert( ecs_get(world, pipeline, EcsPipelineQuery) != NULL, 
        ECS_INVALID_PARAMETER, NULL);

    world->pipeline = pipeline;
}

ecs_entity_t ecs_get_pipeline(
    const ecs_world_t *world)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    world = ecs_get_world(world);
    return world->pipeline;
}

ecs_entity_t ecs_new_pipeline(
    ecs_world_t *world,
    ecs_entity_t e,
    const char *name,
    const char *expr)
{
    ecs_assert(world->magic == ECS_WORLD_MAGIC, ECS_INVALID_PARAMETER, NULL);

    ecs_entity_t result = ecs_new_type(world, e, name, expr);
    ecs_assert(ecs_get(world, result, EcsType) != NULL, 
        ECS_INTERNAL_ERROR, NULL);

    ecs_add_id(world, result, EcsPipeline);

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
}

void FlecsPipelineImport(
    ecs_world_t *world)
{
    ECS_MODULE(world, FlecsPipeline);

    ECS_IMPORT(world, FlecsSystem);

    ecs_set_name_prefix(world, "Ecs");

    ecs_bootstrap_tag(world, EcsPipeline);
    ecs_bootstrap_component(world, EcsPipelineQuery);

    /* Phases of the builtin pipeline are regular entities. Names are set so
     * they can be resolved by type expressions. */
    ecs_bootstrap_tag(world, EcsPreFrame);
    ecs_bootstrap_tag(world, EcsOnLoad);
    ecs_bootstrap_tag(world, EcsPostLoad);
    ecs_bootstrap_tag(world, EcsPreUpdate);
    ecs_bootstrap_tag(world, EcsOnUpdate);
    ecs_bootstrap_tag(world, EcsOnValidate);
    ecs_bootstrap_tag(world, EcsPostUpdate);
    ecs_bootstrap_tag(world, EcsPreStore);
    ecs_bootstrap_tag(world, EcsOnStore);
    ecs_bootstrap_tag(world, EcsPostFrame);

    ECS_TYPE_IMPL(EcsPipelineQuery);

    /* Set ctor and dtor for PipelineQuery */
    ecs_set(world, ecs_id(EcsPipelineQuery), EcsComponentLifecycle, {
        .ctor = ecs_ctor(EcsPipelineQuery),
        .dtor = ecs_dtor(EcsPipelineQuery)
    });

    /* When the Pipeline tag is added a pipeline will be created */
    ECS_TRIGGER(world, EcsOnAddPipeline, EcsOnAdd, Pipeline);

    /* Create the builtin pipeline */
    world->pipeline = ecs_new_pipeline(world, 0, "BuiltinPipeline",
        "PreFrame, OnLoad, PostLoad, PreUpdate, OnUpdate,"
        " OnValidate, PostUpdate, PreStore, OnStore, PostFrame");

    /* Cleanup thread administration when world is destroyed */
    ecs_atfini(world, FlecsPipelineFini, NULL);
}

#endif
