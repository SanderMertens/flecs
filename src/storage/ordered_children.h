/**
 * @file storage/ordered_children.h
 * @brief Storage for ordered list of entity (child) ids.
 */

#ifndef FLECS_ORDERED_CHILDREN_H
#define FLECS_ORDERED_CHILDREN_H

void flecs_ordered_children_init(
    ecs_world_t *world,
    ecs_component_record_t *cr);

void flecs_ordered_children_fini(
    ecs_world_t *world,
    ecs_component_record_t *cr);

void flecs_ordered_children_populate(
    ecs_world_t *world,
    ecs_component_record_t *cr);

void flecs_ordered_children_clear(
    ecs_component_record_t *cr);

void flecs_ordered_children_reparent(
    ecs_world_t *world,
    const ecs_table_t *dst,
    const ecs_table_t *src,
    int32_t row,
    int32_t count);

void flecs_ordered_children_unparent(
    ecs_world_t *world,
    const ecs_table_t *src,
    int32_t row,
    int32_t count);

void flecs_ordered_children_reorder(
    ecs_world_t *world,
    ecs_entity_t parent,
    const ecs_entity_t *children,
    int32_t child_count);

void flecs_ordered_entities_append(
    ecs_world_t *world,
    ecs_component_record_t *cr,
    ecs_entity_t e);

void flecs_ordered_entities_remove(
    ecs_world_t *world,
    ecs_component_record_t *cr,
    ecs_entity_t e);

#endif
