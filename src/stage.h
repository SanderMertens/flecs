/**
 * @file stage.h
 * @brief Stage functions.
 */

#ifndef FLECS_STAGE_H
#define FLECS_STAGE_H

/* Initialize stage data structures */
void flecs_stage_init(
    ecs_world_t *world,
    ecs_stage_t *stage);

/* Deinitialize stage */
void flecs_stage_deinit(
    ecs_world_t *world,
    ecs_stage_t *stage);

/* Post-frame merge actions */
void flecs_stage_merge_post_frame(
    ecs_world_t *world,
    ecs_stage_t *stage);  

bool flecs_defer_begin(
    ecs_world_t *world,
    ecs_stage_t *stage);

bool flecs_defer_modified(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t entity,
    ecs_entity_t component);

bool flecs_defer_new(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t entity,
    ecs_id_t id);

bool flecs_defer_clone(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t entity,
    ecs_entity_t src,
    bool clone_value);

bool flecs_defer_bulk_new(
    ecs_world_t *world,
    ecs_stage_t *stage,
    int32_t count,
    ecs_id_t id,
    const ecs_entity_t **ids_out);

bool flecs_defer_delete(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t entity);

bool flecs_defer_clear(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t entity);

bool flecs_defer_on_delete_action(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_id_t id,
    ecs_entity_t action);

bool flecs_defer_enable(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t entity,
    ecs_entity_t component,
    bool enable);    

bool flecs_defer_add(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t entity,
    ecs_id_t id);

bool flecs_defer_remove(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t entity,
    ecs_id_t id);

bool flecs_defer_set(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_defer_op_kind_t op_kind,
    ecs_entity_t entity,
    ecs_entity_t component,
    ecs_size_t size,
    const void *value,
    void **value_out);

bool flecs_defer_end(
    ecs_world_t *world,
    ecs_stage_t *stage);

bool flecs_defer_purge(
    ecs_world_t *world,
    ecs_stage_t *stage);

#endif
