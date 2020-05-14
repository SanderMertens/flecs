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
            if (column->inout_kind != EcsIn) {
                write_state[component] = WriteToMain;
            }
        };
    } else if (column->from_kind == EcsFromEmpty || column->oper_kind == EcsOperNot) {
        switch(column->inout_kind) {
        case EcsInOut:
        case EcsOut:
            write_state[component] = WriteToStage;
        default:
            break;
        };
    }

    return false;
}

static
bool check_column(
    ecs_sig_column_t *column,
    int8_t *write_state)
{
    if (column->oper_kind != EcsOperOr) {
        return check_column_component(column, column->is.component, write_state);
    }  

    return false;
}

static
ecs_vector_t* build_pipeline(
    ecs_world_t *world,
    ecs_query_t *query)
{
    int8_t *write_state = ecs_os_calloc(ECS_HI_COMPONENT_ID, sizeof(int8_t));
    ecs_vector_t *ops = NULL;
    ecs_pipeline_op_t *op = NULL;

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
            ecs_vector_each(q->sig.columns, ecs_sig_column_t, column, {
                needs_merge |= check_column(column, write_state);
            });

            if (needs_merge) {
                /* After merge all components will be merged, so reset state */
                memset(write_state, 0, ECS_HI_COMPONENT_ID * sizeof(int8_t));
                op = NULL;

                /* Re-evaluate columns to set write flags */
                needs_merge = false;
                ecs_vector_each(q->sig.columns, ecs_sig_column_t, column, {
                    needs_merge |= check_column(column, write_state);
                });

                /* The component states were just reset, so if we conclude that
                 * another merge is needed something is wrong. */
                ecs_assert(needs_merge == false, ECS_INTERNAL_ERROR, NULL);        
            }

            if (!op) {
                op = ecs_vector_add(&ops, ecs_pipeline_op_t);
                op->count = 0;
            }

            op->count ++;
        }
    }

    ecs_os_free(write_state);

    return ops;
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
        ecs_sig_add(&sig, EcsFromSelf, EcsOperAnd, EcsIn, EEcsColSystem, 0);
        ecs_sig_add(&sig, EcsFromSelf, EcsOperAnd, EcsIn, ECS_XOR | pipeline, 0);
        ecs_sig_add(&sig, EcsFromSelf, EcsOperNot, EcsIn, EEcsInactive, 0);
        ecs_sig_add(&sig, EcsFromSelf, EcsOperNot, EcsIn, EEcsDisabledIntern, 0);

        /* Create the query. Sort the query by system id and phase */
        ecs_query_t *query = ecs_query_new_w_sig(world, 0, &sig);
        ecs_query_sort(world, query, 0, compare_entity);
        ecs_query_sort_types(world, query, pipeline, rank_phase);

        ecs_set(world, pipeline, EcsPipelineQuery, { 
            .query = query,
            .match_count = query->match_count,
            .ops = build_pipeline(world, query)
        });

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
    world->builtin_pipeline = ecs_new_pipeline(world, "EcsBuiltinPipeline",
        "EcsPreFrame, EcsOnLoad, EcsPostLoad, EcsPreUpdate, EcsOnUpdate,"
        " EcsOnValidate, EcsPostUpdate, EcsPreStore, EcsOnStore, EcsPostFrame");
}

void ecs_progress_pipeline(
    ecs_world_t *world,
    ecs_entity_t pipeline,
    float delta_time)
{
    /* Start with a merge so we know for sure that we start with a consistent
     * state. If there is nothing to merge, this will essentially be a no-op. */
    ecs_staging_end(world, false);
    ecs_staging_begin(world);

    EcsPipelineQuery *query = ecs_get_mut(
        world, pipeline, EcsPipelineQuery, NULL);

    ecs_assert(query != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(query->query != NULL, ECS_INTERNAL_ERROR, NULL);

    if (query->match_count != query->query->match_count) {
        ecs_vector_free(query->ops);
        query->ops = build_pipeline(world, query->query);
    }

    ecs_time_t start = {0};
    ecs_time_measure(&start);

    int32_t count_since_merge = 0;
    ecs_vector_t *ops = query->ops;
    ecs_pipeline_op_t *op = ecs_vector_first(ops);
    
    ecs_query_iter_t it = ecs_query_iter(query->query, 0, 0);
    while (ecs_query_next(&it)) {
        ecs_rows_t *rows = &it.rows;
        ECS_COLUMN(rows, EcsColSystem, sys, 1);

        int32_t i;
        for(i = 0; i < rows->count; i ++) {
            ecs_entity_t e = rows->entities[i];

            ecs_run_intern(world, world, e, &sys[i], delta_time, 0, 0, 
                NULL, NULL);

            count_since_merge ++;

            if (count_since_merge == op->count) {
                ecs_staging_end(world, false);
                ecs_staging_begin(world);
                count_since_merge = 0;
                op ++;
            }
        }
    }

    world->system_time_total += ecs_time_measure(&start);
}
