/**
 * @file storage/non_fragmenting_childof.h
 * @brief Non-fragmenting storage for hierarchies.
 */

#ifndef FLECS_NON_FRAGMENTING_CHILDOF
#define FLECS_NON_FRAGMENTING_CHILDOF

void flecs_bootstrap_parent_component(
    ecs_world_t *world);

void flecs_on_non_fragmenting_child_move_add(
    ecs_world_t *world,
    const ecs_table_t *dst,
    const ecs_table_t *src,
    int32_t row,
    int32_t count);

void flecs_on_non_fragmenting_child_move_remove(
    ecs_world_t *world,
    const ecs_table_t *dst,
    const ecs_table_t *src,
    int32_t row,
    int32_t count,
    bool update_parent_records);

void flecs_non_fragmenting_childof_reparent(
    ecs_world_t *world,
    const ecs_table_t *dst,
    const ecs_table_t *src,
    int32_t row,
    int32_t count);

void flecs_non_fragmenting_childof_unparent(
    ecs_world_t *world,
    const ecs_table_t *dst,
    const ecs_table_t *src,
    int32_t row,
    int32_t count);

bool flecs_component_has_non_fragmenting_childof(
    ecs_component_record_t *cr);

int flecs_add_non_fragmenting_child_w_records(
    ecs_world_t *world,
    ecs_entity_t parent,
    ecs_entity_t entity,
    ecs_component_record_t *cr,
    const ecs_record_t *r);

#endif
