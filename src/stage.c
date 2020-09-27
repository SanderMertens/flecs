#include "private_api.h"

/* -- Private functions -- */

void ecs_stage_merge(
    ecs_world_t *world,
    ecs_stage_t *stage)
{
    ecs_assert(stage != &world->stage, ECS_INTERNAL_ERROR, NULL);

    ecs_assert(stage->defer == 0, ECS_INVALID_PARAMETER, NULL);
    if (ecs_vector_count(stage->defer_merge_queue)) {
        stage->defer ++;
        stage->defer_queue = stage->defer_merge_queue;
        ecs_defer_flush(world, stage);
        ecs_vector_clear(stage->defer_merge_queue);
        ecs_assert(stage->defer_queue == NULL, ECS_INVALID_PARAMETER, NULL);
    }    
}

void ecs_stage_defer_begin(
    ecs_world_t *world,
    ecs_stage_t *stage)
{    
    ecs_defer_none(world, stage);
    if (stage->defer == 1) {
        stage->defer_queue = stage->defer_merge_queue;
    }
}

void ecs_stage_defer_end(
    ecs_world_t *world,
    ecs_stage_t *stage)
{ 
    stage->defer --;
    if (!stage->defer) {
        stage->defer_merge_queue = stage->defer_queue;
        stage->defer_queue = NULL;
    }
}

void ecs_stage_merge_post_frame(
    ecs_world_t *world,
    ecs_stage_t *stage)
{
    /* Execute post frame actions */
    ecs_vector_each(stage->post_frame_actions, ecs_action_elem_t, action, {
        action->action(world, action->ctx);
    });

    ecs_vector_free(stage->post_frame_actions);
    stage->post_frame_actions = NULL;
}

void ecs_stage_init(
    ecs_world_t *world,
    ecs_stage_t *stage)
{
    memset(stage, 0, sizeof(ecs_stage_t));
    if (stage == &world->stage) {
        stage->id = 0;
    } else if (stage == &world->temp_stage) {
        stage->id = 1;
    }
}

void ecs_stage_deinit(
    ecs_world_t *world,
    ecs_stage_t *stage)
{
    ecs_vector_free(stage->defer_queue);
    ecs_vector_free(stage->defer_merge_queue);
}

