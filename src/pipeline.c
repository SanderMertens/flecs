#include "flecs_private.h"

static
int compare_entity(
    ecs_entity_t e1, 
    void *ptr1, 
    ecs_entity_t e2, 
    void *ptr2) 
{
    return e1 - e2;
}

static
int rank_phase(
    ecs_world_t *world,
    ecs_entity_t rank_component,
    ecs_type_t type) 
{
    return ecs_type_get_entity_for_xor(world, type, rank_component);
}

typedef enum ComponentWriteState {
    NotWritten = 0,
    WriteToMain,
    WriteToStage
} ComponentWriteState;

static
bool check_column_component(
    ecs_sig_column_t *column,
    bool is_active,
    int32_t component,
    int8_t *write_state)    
{
    int8_t state = write_state[component];

    if (column->from_kind == EcsFromSelf && column->oper_kind != EcsOperNot) {
        switch(column->inout_kind) {
        case EcsInOut:
        case EcsIn:
            if (state == WriteToStage) {
                return true;
            }
        case EcsOut:
            if (is_active && column->inout_kind != EcsIn) {
                write_state[component] = WriteToMain;
            }
        };
    } else if (column->from_kind == EcsFromEmpty || column->oper_kind == EcsOperNot) {
        switch(column->inout_kind) {
        case EcsInOut:
        case EcsOut:
            if (is_active) {
                write_state[component] = WriteToStage;
            }
        default:
            break;
        };
    }

    return false;
}

static
bool check_column(
    ecs_sig_column_t *column,
    bool is_active,
    int8_t *write_state)
{
    if (column->oper_kind != EcsOperOr) {
        return check_column_component(
            column, is_active,column->is.component, write_state);
    }  

    return false;
}

static
void build_pipeline(
    ecs_world_t *world,
    EcsPipelineQuery *pq)
{
    if (pq->query->prev_match_count == pq->query->match_count) {
        /* No need to rebuild the pipeline */
        return;
    }

    world->stats.pipeline_build_count_total ++;

    int8_t *write_state = ecs_os_calloc(ECS_HI_COMPONENT_ID, sizeof(int8_t));
    ecs_pipeline_op_t *op = NULL;
    ecs_vector_t *ops = NULL;
    ecs_query_t *query = pq->build_query;

    if (pq->ops) {
        ecs_vector_free(pq->ops);
    }

    /* Iterate systems in pipeline, add ops for running / merging */
    ecs_query_iter_t it = ecs_query_iter(query, 0, 0);
    while (ecs_query_next(&it)) {
        ecs_rows_t *rows = &it.rows;
        EcsColSystem *sys = ecs_column(rows, EcsColSystem, 1);

        int i;
        for (i = 0; i < rows->count; i ++) {

            ecs_query_t *q = sys[i].query;
            if (!q) {
                continue;
            }

            bool needs_merge = false;
            bool is_active = !ecs_has_entity(
                world, rows->entities[i], EcsInactive);

            ecs_vector_each(q->sig.columns, ecs_sig_column_t, column, {
                needs_merge |= check_column(column, is_active, write_state);
            });

            if (needs_merge) {
                /* After merge all components will be merged, so reset state */
                memset(write_state, 0, ECS_HI_COMPONENT_ID * sizeof(int8_t));
                op = NULL;

                /* Re-evaluate columns to set write flags if system is active.
                 * If system is inactive, it can't write anything and so it
                 * should not insert unnecessary merges.  */
                needs_merge = false;
                if (is_active) {
                    ecs_vector_each(q->sig.columns, ecs_sig_column_t, column, {
                        needs_merge |= check_column(column, true, write_state);
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

    ecs_os_free(write_state);

    pq->ops = ops;
}

static
int32_t iter_reset(
    ecs_world_t *world,
    EcsPipelineQuery *pq,
    ecs_query_iter_t *it_out,
    ecs_pipeline_op_t **op_out,
    ecs_entity_t move_to)
{
    /* If query has matched with new tables, we need to rebuild the pipeline */
    build_pipeline(world, pq);

    ecs_pipeline_op_t *op = ecs_vector_first(pq->ops, ecs_pipeline_op_t);
    int32_t ran_since_merge = 0;

    ecs_query_iter_t it = ecs_query_iter(pq->query, 0, 0);
    while (ecs_query_next(&it)) {
        ecs_rows_t *rows = &it.rows;

        int32_t i;
        for(i = 0; i < rows->count; i ++) {
            ecs_entity_t e = rows->entities[i];

            ran_since_merge ++;
            if (ran_since_merge == op->count) {
                ran_since_merge = 0;
                op ++;
            }

            if (e == move_to) {
                *it_out = it;
                *op_out = op;
                return i;
            }
        }
    }

    ecs_abort(ECS_UNSUPPORTED, NULL);

    return -1;
}

void ecs_progress_pipeline(
    ecs_world_t *world,
    ecs_entity_t pipeline,
    float delta_time)
{
    ecs_assert(!world->in_progress, ECS_INTERNAL_ERROR, NULL);

    /* Evaluate monitors and rebuild pipeline if rematching occurred */
    ecs_eval_component_monitors(world);

    EcsPipelineQuery *pq = ecs_get_mut(
        world, pipeline, EcsPipelineQuery, NULL);

    ecs_assert(pq != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(pq->query != NULL, ECS_INTERNAL_ERROR, NULL);

    build_pipeline(world, pq);

    ecs_time_t start = {0};
    if (world->measure_frame_time) {
        ecs_time_measure(&start);
    }

    ecs_vector_t *ops = pq->ops;
    ecs_pipeline_op_t *op = ecs_vector_first(ops, ecs_pipeline_op_t);
    ecs_pipeline_op_t *op_last = ecs_vector_last(ops, ecs_pipeline_op_t);
    int32_t ran_since_merge = 0;

    world->stats.systems_ran_frame = 0;

    ecs_staging_begin(world);
    
    ecs_query_iter_t it = ecs_query_iter(pq->query, 0, 0);
    while (ecs_query_next(&it)) {
        ecs_rows_t *rows = &it.rows;
        EcsColSystem *sys = ecs_column(rows, EcsColSystem, 1);

        int32_t i;
        for(i = 0; i < rows->count; i ++) {
            ecs_entity_t e = rows->entities[i];

            ecs_run_intern(world, world, e, &sys[i], delta_time, 0, 0, 
                NULL, NULL);

            ran_since_merge ++;
            world->stats.systems_ran_frame ++;

            if (ran_since_merge == op->count) {
                ran_since_merge = 0;

                if (op++ != op_last) {
                    ecs_staging_end(world, false);
                    ecs_staging_begin(world);

                    /* If the set of matched systems changed as a result of the
                     * merge, we have to reset the iterator and move it to our
                     * current position (system). If there are a lot of systems
                     * in the pipeline this can be an expensive operation, but
                     * should happen infrequently. */
                    if (pq->match_count != pq->query->match_count) {
                        i = iter_reset(world, pq, &it, &op, e);
                        op_last = ecs_vector_last(pq->ops, ecs_pipeline_op_t);
                        sys = ecs_column(rows, EcsColSystem, 1);
                    }
                }                
            }
        }
    }

    ecs_staging_end(world, false);

    if (world->measure_frame_time) {
        world->stats.system_time_total += ecs_time_measure(&start);
    }
}

static 
void EcsOnAddPipeline(
    ecs_rows_t *rows)
{
    ecs_world_t *world = rows->world;
    ecs_entity_t *entities = rows->entities;

    int32_t i;
    for (i = rows->count - 1; i >= 0; i --) {
        ecs_entity_t pipeline = entities[i];
        ecs_sig_t sig = { 0 };

#ifndef NDEBUG
        const EcsType *type_ptr = ecs_get_ptr(world, pipeline, EcsType);
        ecs_assert(type_ptr != NULL, ECS_INTERNAL_ERROR, NULL);

        char *str = ecs_type_str(world, type_ptr->normalized);
        ecs_trace_1("pipeline #[green]%s#[normal] created with #[red][%s]",
            ecs_get_name(world, pipeline), str);
        free(str);
#endif
        ecs_trace_push();

        /* Build signature for pipeline quey that matches EcsColSystems, has the
         * pipeline as a XOR column, and ignores systems with EcsInactive and
         * EcsDisabledIntern. Note that EcsDisabled is automatically ignored by
         * the regular query matching */
        ecs_sig_add(&sig, EcsFromSelf, EcsOperAnd, EcsIn, ecs_entity(EcsColSystem), 0);
        ecs_sig_add(&sig, EcsFromSelf, EcsOperAnd, EcsIn, ECS_XOR | pipeline, 0);
        ecs_sig_add(&sig, EcsFromSelf, EcsOperNot, EcsIn, EcsInactive, 0);
        ecs_sig_add(&sig, EcsFromSelf, EcsOperNot, EcsIn, EcsDisabledIntern, 0);

        /* Create the query. Sort the query by system id and phase */
        ecs_query_t *query = ecs_query_new_w_sig(world, 0, &sig);
        ecs_query_sort(world, query, 0, compare_entity);
        ecs_query_sort_types(world, query, pipeline, rank_phase);

        /* Build signature for pipeline build query. The build query includes
         * systems that are inactive, as an inactive system may become active as
         * a result of another system, and as a result the correct merge 
         * operations need to be put in place. */
        ecs_sig_add(&sig, EcsFromSelf, EcsOperAnd, EcsIn, ecs_entity(EcsColSystem), 0);
        ecs_sig_add(&sig, EcsFromSelf, EcsOperAnd, EcsIn, ECS_XOR | pipeline, 0);
        ecs_sig_add(&sig, EcsFromSelf, EcsOperNot, EcsIn, EcsDisabledIntern, 0);

        /* Use the same sorting functions for the build query */
        ecs_query_t *build_query = ecs_query_new_w_sig(world, 0, &sig);
        ecs_query_sort(world, build_query, 0, compare_entity);
        ecs_query_sort_types(world, build_query, pipeline, rank_phase);       

        EcsPipelineQuery *pq = ecs_get_mut(
            world, pipeline, EcsPipelineQuery, NULL);
        ecs_assert(pq != NULL, ECS_INTERNAL_ERROR, NULL);

        pq->query = query;
        pq->build_query = build_query;
        pq->match_count = -1;
        pq->ops = NULL;

        ecs_trace_pop();
    }
}

void ecs_init_pipeline_builtins(
    ecs_world_t *world)
{
    /* -- Pipeline creation infrastructure & create builtin pipeline -- */

    /* Phases of the builtin pipeline are regular entities. Names are set so
     * they can be resolved by type expressions. */
    ecs_set(world, EcsPreFrame, EcsName, {"EcsPreFrame"});
    ecs_set(world, EcsOnLoad, EcsName, {"EcsOnLoad"});
    ecs_set(world, EcsPostLoad, EcsName, {"EcsPostLoad"});
    ecs_set(world, EcsPreUpdate, EcsName, {"EcsPreUpdate"});
    ecs_set(world, EcsOnUpdate, EcsName, {"EcsOnUpdate"});
    ecs_set(world, EcsOnValidate, EcsName, {"EcsOnValidate"});
    ecs_set(world, EcsPostUpdate, EcsName, {"EcsPostUpdate"});
    ecs_set(world, EcsPreStore, EcsName, {"EcsPreStore"});
    ecs_set(world, EcsOnStore, EcsName, {"EcsOnStore"});
    ecs_set(world, EcsPostFrame, EcsName, {"EcsPostFrame"});

    /* When the Pipeline tag is added a pipeline will be created */
    ECS_TRIGGER(world, EcsOnAddPipeline, EcsOnAdd, EcsPipeline, 0);

    /* Create the builtin pipeline */
    world->pipeline = ecs_new_pipeline(world, "EcsBuiltinPipeline",
        "EcsPreFrame, EcsOnLoad, EcsPostLoad, EcsPreUpdate, EcsOnUpdate,"
        " EcsOnValidate, EcsPostUpdate, EcsPreStore, EcsOnStore, EcsPostFrame");
}
