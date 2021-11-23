#ifndef FLECS_PIPELINE_PRIVATE_H
#define FLECS_PIPELINE_PRIVATE_H

#include "../../private_api.h"

/** Instruction data for pipeline.
 * This type is the element type in the "ops" vector of a pipeline and contains
 * information about the set of systems that need to be ran before a merge. */
typedef struct ecs_pipeline_op_t {
    int32_t count;              /* Number of systems to run before merge */
    bool multi_threaded;        /* Whether systems can be ran multi threaded */
    bool no_staging;            /* Whether systems are staged or not */
} ecs_pipeline_op_t;

typedef struct EcsPipelineQuery {
    ecs_query_t *query;
    ecs_query_t *build_query;
    ecs_vector_t *ops;
    int32_t match_count;
    int32_t rebuild_count;
    ecs_entity_t last_system;
} EcsPipelineQuery;

////////////////////////////////////////////////////////////////////////////////
//// Pipeline API
////////////////////////////////////////////////////////////////////////////////

/** Update a pipeline (internal function).
 * Before running a pipeline, it must be updated. During this update phase
 * all systems in the pipeline are collected, ordered and sync points are 
 * inserted where necessary. This operation may only be called when staging is
 * disabled.
 *
 * Because multiple threads may run a pipeline, preparing the pipeline must 
 * happen synchronously, which is why this function is separate from 
 * ecs_run_pipeline. Not running the prepare step may cause systems to not get
 * ran, or ran in the wrong order.
 *
 * If 0 is provided for the pipeline id, the default pipeline will be ran (this
 * is either the builtin pipeline or the pipeline set with set_pipeline()).
 * 
 * @param world The world.
 * @param pipeline The pipeline to run.
 * @return The number of elements in the pipeline.
 */
bool ecs_pipeline_update(
    ecs_world_t *world,
    ecs_entity_t pipeline,
    bool start_of_frame); 

int32_t ecs_pipeline_reset_iter(
    ecs_world_t *world,
    const EcsPipelineQuery *pq,
    ecs_iter_t *iter_out,
    ecs_pipeline_op_t **op_out,
    ecs_pipeline_op_t **last_op_out);

////////////////////////////////////////////////////////////////////////////////
//// Worker API
////////////////////////////////////////////////////////////////////////////////

void ecs_worker_begin(
    ecs_world_t *world);

int32_t ecs_worker_sync(
    ecs_world_t *world,
    const EcsPipelineQuery *pq,
    ecs_iter_t *it,
    int32_t i,
    ecs_pipeline_op_t **op_out,
    ecs_pipeline_op_t **last_op_out);

void ecs_worker_end(
    ecs_world_t *world);

void ecs_workers_progress(
    ecs_world_t *world,
    ecs_entity_t pipeline,
    FLECS_FLOAT delta_time);

#endif
