/**
 * @file storage/non_fragmenting_childof.h
 * @brief Non-fragmenting storage for hierarchies.
 */

#ifndef FLECS_NON_FRAGMENTING_CHILDOF_H
#define FLECS_NON_FRAGMENTING_CHILDOF_H

void flecs_bootstrap_parent_component(
    ecs_world_t *world);

void flecs_non_fragmenting_childof_count(
    ecs_world_t *world,
    ecs_component_record_t *cr,
    const ecs_table_t *table,
    int32_t count);

void flecs_non_fragmenting_childof_move(
    ecs_world_t *world,
    const ecs_table_t *dst,
    const ecs_table_t *src,
    int32_t row,
    int32_t count);

void flecs_non_fragmenting_childof_reparent(
    ecs_world_t *world,
    const ecs_table_t *dst,
    const ecs_table_t *src,
    int32_t row,
    int32_t count);

void flecs_non_fragmenting_childof_unparent(
    ecs_world_t *world,
    const ecs_table_t *src,
    int32_t row,
    int32_t count);

void flecs_add_non_fragmenting_child_w_records(
    ecs_world_t *world,
    ecs_entity_t parent,
    ecs_entity_t entity,
    ecs_component_record_t *cr,
    const ecs_record_t *r);

static inline
bool flecs_component_has_non_fragmenting_childof(
    const ecs_component_record_t *cr)
{
    return (cr->flags & EcsIdOrderedChildren) &&
        cr->pair->non_fragmenting_child_count;
}

#endif
