/**
 * @file component_actions.c
 * @brief Logic executed after adding/removing a component.
 */

#include "private_api.h"

/* Invoke component hook. */
void flecs_invoke_hook(
    ecs_world_t *world,
    ecs_table_t *table,
    const ecs_component_record_t *cr,
    const ecs_table_record_t *tr,
    int32_t count,
    int32_t row,
    const ecs_entity_t *entities,
    ecs_id_t id,
    const ecs_type_info_t *ti,
    ecs_entity_t event,
    ecs_iter_action_t hook);

/* Invoke replace hook */
void flecs_invoke_replace_hook(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_entity_t entity,
    ecs_id_t id,
    const void *old_ptr,
    const void *new_ptr,
    const ecs_type_info_t *ti);

/* Add action for sparse components. */
bool flecs_sparse_on_add(
    ecs_world_t *world,
    ecs_table_t *table,
    int32_t row,
    int32_t count,
    const ecs_type_t *added,
    bool construct);

/* Add action for single sparse component. */
bool flecs_sparse_on_add_cr(
    ecs_world_t *world,
    ecs_table_t *table,
    int32_t row,
    ecs_component_record_t *cr,
    bool construct,
    void **ptr_out);

/* Run actions for creating new entity in table. */
void flecs_actions_new(
    ecs_world_t *world,
    ecs_table_t *table,
    int32_t row,
    int32_t count,
    const ecs_table_diff_t *diff,
    ecs_flags32_t flags,
    bool construct,
    bool sparse);

/* Run actions for deleting an entity. */
void flecs_actions_delete(
    ecs_world_t *world,
    ecs_table_t *table,
    int32_t row,
    int32_t count,
    const ecs_table_diff_t *diff);

/* Same as flecs_actions_delete, but for entities whose parent is also deleted */
void flecs_actions_delete_tree(
    ecs_world_t *world,
    ecs_table_t *table,
    int32_t row,
    int32_t count,
    const ecs_table_diff_t *diff);

/* Run actions for added components in table move */
void flecs_actions_move_add(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_table_t *other_table,
    int32_t row,
    int32_t count,
    const ecs_table_diff_t *diff,
    ecs_flags32_t flags,
    bool construct,
    bool sparse);

/* Run actions for removed components in table move */
void flecs_actions_move_remove(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_table_t *other_table,
    int32_t row,
    int32_t count,
    const ecs_table_diff_t *diff);

/* Run on set actions. */
void flecs_notify_on_set(
    ecs_world_t *world,
    ecs_table_t *table,
    int32_t row,
    ecs_id_t id,
    bool invoke_hook);

/* Same as flecs_notify_on_set but for multiple component ids. */
void flecs_notify_on_set_ids(
    ecs_world_t *world,
    ecs_table_t *table,
    int32_t row,
    int32_t count,
    ecs_type_t *ids);
