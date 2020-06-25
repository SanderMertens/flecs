#ifndef FLECS_PIPELINE_PRIVATE_H
#define FLECS_PIPELINE_PRIVATE_H

#include "../../flecs_private.h"
#include "flecs/modules/pipeline.h"

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
    float delta_time);


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
