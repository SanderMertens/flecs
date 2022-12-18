/**
 * @file addons/pipeline/pipeline.h
 * @brief Internal functions/types for pipeline addon.
 */

#ifndef FLECS_PIPELINE_PRIVATE_H
#define FLECS_PIPELINE_PRIVATE_H

#include "../../private_api.h"

/** Instruction data for pipeline.
 * This type is the element type in the "ops" vector of a pipeline. */
typedef struct ecs_pipeline_op_t {
    int32_t offset;             /* Offset in systems vector */
    int32_t count;              /* Number of systems to run before next op */
    bool multi_threaded;        /* Whether systems can be ran multi threaded */
    bool no_readonly;           /* Whether systems are staged or not */
} ecs_pipeline_op_t;

typedef struct ecs_pipeline_state_t {
    ecs_query_t *query;         /* Pipeline query */
    ecs_vec_t ops;              /* Pipeline schedule */
    ecs_vec_t systems;          /* Vector with system ids */


    ecs_entity_t last_system;   /* Last system ran by pipeline */
    ecs_id_record_t *idr_inactive; /* Cached record for quick inactive test */
    int32_t match_count;        /* Used to track of rebuild is necessary */
    int32_t rebuild_count;      /* Number of pipeline rebuilds */
    ecs_iter_t *iters;          /* Iterator for worker(s) */
    int32_t iter_count;

    /* Members for continuing pipeline iteration after pipeline rebuild */
    ecs_pipeline_op_t *cur_op;  /* Current pipeline op */
    int32_t cur_i;              /* Index in current result */
    int32_t ran_since_merge;    /* Index in current op */
    bool no_readonly;           /* Is pipeline in readonly mode */
} ecs_pipeline_state_t;

typedef struct EcsPipeline {
    /* Stable ptr so threads can safely access while entity/components move */
    ecs_pipeline_state_t *state;
} EcsPipeline;

////////////////////////////////////////////////////////////////////////////////
//// Pipeline API
////////////////////////////////////////////////////////////////////////////////

bool flecs_pipeline_update(
    ecs_world_t *world,
    ecs_pipeline_state_t *pq,
    bool start_of_frame);

void flecs_run_pipeline(
    ecs_world_t *world,
    ecs_pipeline_state_t *pq,
    ecs_ftime_t delta_time);

////////////////////////////////////////////////////////////////////////////////
//// Worker API
////////////////////////////////////////////////////////////////////////////////

bool flecs_worker_begin(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_pipeline_state_t *pq,
    bool start_of_frame);

void flecs_worker_end(
    ecs_world_t *world,
    ecs_stage_t *stage);

bool flecs_worker_sync(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_pipeline_state_t *pq,
    ecs_pipeline_op_t **cur_op,
    int32_t *cur_i);

void flecs_workers_progress(
    ecs_world_t *world,
    ecs_pipeline_state_t *pq,
    ecs_ftime_t delta_time);

#endif
