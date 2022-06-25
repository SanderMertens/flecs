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

typedef struct {
    ecs_query_t *query;         /* Pipeline query */
    
    ecs_vector_t *ops;          /* Pipeline schedule */
    int32_t match_count;        /* Used to track of rebuild is necessary */
    int32_t rebuild_count;      /* Number of pipeline rebuilds */
    ecs_entity_t last_system;   /* Last system ran by pipeline */

    ecs_id_record_t *idr_inactive; /* Cached record for quick inactive test */

    ecs_iter_t *iters;          /* Iterator for worker(s) */
    int32_t iter_count;

    /* Members for continuing pipeline iteration after pipeline rebuild */
    ecs_pipeline_op_t *cur_op;  /* Current pipeline op */
    int32_t cur_i;              /* Index in current result */
} EcsPipeline;

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

void ecs_pipeline_reset_iter(
    ecs_world_t *world,
    EcsPipeline *q);

////////////////////////////////////////////////////////////////////////////////
//// Worker API
////////////////////////////////////////////////////////////////////////////////

void ecs_worker_begin(
    ecs_world_t *world);

bool ecs_worker_sync(
    ecs_world_t *world,
    EcsPipeline *p);

void ecs_worker_end(
    ecs_world_t *world);

void ecs_workers_progress(
    ecs_world_t *world,
    ecs_entity_t pipeline,
    ecs_ftime_t delta_time);

#endif
