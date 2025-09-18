/**
 * @file storage/ordered_children.h
 * @brief Storage for ordered list of entity (child) ids.
 */

#ifndef FLECS_ORDERED_CHILDREN_H
#define FLECS_ORDERED_CHILDREN_H

/* Initialize ordered children storage. */
void flecs_ordered_children_init(
    ecs_world_t *world,
    ecs_component_record_t *cr);

/* Free ordered children storage. */
void flecs_ordered_children_fini(
    ecs_world_t *world,
    ecs_component_record_t *cr);

/* Populate ordered children storage with existing children. */
void flecs_ordered_children_populate(
    ecs_world_t *world,
    ecs_component_record_t *cr);

/* Clear ordered children storage. */
void flecs_ordered_children_clear(
    ecs_component_record_t *cr);

/* Reparent entities in ordered children storage. */
void flecs_ordered_children_reparent(
    ecs_world_t *world,
    const ecs_table_t *src,
    const ecs_table_t *dst,
    int32_t row,
    int32_t count);

/* Unparent entities in ordered children storage. */
void flecs_ordered_children_unparent(
    ecs_world_t *world,
    const ecs_table_t *src,
    int32_t row,
    int32_t count);

/* Reorder entities in ordered children storage. */
void flecs_ordered_children_reorder(
    ecs_world_t *world,
    ecs_entity_t parent,
    const ecs_entity_t *children,
    int32_t child_count);

#endif
