#ifndef FLECS_PIPELINE_PRIVATE_H
#define FLECS_PIPELINE_PRIVATE_H

#include "../../private_api.h"
#include "../system/system.h"
#include "flecs/modules/pipeline.h"

/** Instruction data for pipeline.
 * This type is the element type in the "ops" vector of a pipeline and contains
 * information about the set of systems that need to be ran before a merge. */
typedef struct ecs_pipeline_op_t {
    int32_t count;              /**< Number of systems to run before merge */
} ecs_pipeline_op_t;

typedef struct EcsPipelineQuery {
    ecs_query_t *query;
    ecs_query_t *build_query;
    int32_t match_count;
    ecs_vector_t *ops;
} EcsPipelineQuery;

////////////////////////////////////////////////////////////////////////////////
//// Pipeline API
////////////////////////////////////////////////////////////////////////////////

int32_t ecs_pipeline_update(
    ecs_world_t *world,
    ecs_entity_t pipeline);

int32_t ecs_pipeline_begin(
    ecs_world_t *world,
    ecs_entity_t pipeline);

void ecs_pipeline_end(
    ecs_world_t *world);

void ecs_pipeline_progress(
    ecs_world_t *world,
    ecs_entity_t pipeline,
    FLECS_FLOAT delta_time);


////////////////////////////////////////////////////////////////////////////////
//// Worker API
////////////////////////////////////////////////////////////////////////////////

void ecs_worker_begin(
    ecs_world_t *world);

bool ecs_worker_sync(
    ecs_world_t *world);

void ecs_worker_end(
    ecs_world_t *world);

void ecs_workers_progress(
    ecs_world_t *world);

#endif
