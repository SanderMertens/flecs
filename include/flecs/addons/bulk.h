/**
 * @file bulk.h
 * @brief Bulk operations operate on all entities that match a provided filter.
 */

#ifdef FLECS_BULK

#ifndef FLECS_BULK_H
#define FLECS_BULK_H

#ifdef __cplusplus
extern "C" {
#endif

/** Add an entity to entities matching a filter.
 * This operation is the same as ecs_add_id, but is applied to all entities
 * that match the provided filter.
 *
 * @param world The world.
 * @param entity_add The entity to add.
 * @param filter The filter.
 */
FLECS_API
void ecs_bulk_add_entity(
    ecs_world_t *world,
    ecs_entity_t entity_add,
    const ecs_filter_t *filter);

/** Add a type to entities matching a filter.
 * This operation is the same as ecs_add_type but is applied to all entities
 * that match the provided filter.
 *
 * @param world The world.
 * @param type The type to add.
 * @param filter The filter.
 */
FLECS_API
void ecs_bulk_add_type(
    ecs_world_t *world,
    ecs_type_t type,
    const ecs_filter_t *filter);

/** Add a component / type / tag to entities matching a filter.
 * This operation is the same as ecs_add but is applied to all entities
 * that match the provided filter.
 *
 * @param world The world.
 * @param type The component, type or tag to add.
 * @param filter The filter.
 */
#define ecs_bulk_add(world, type, filter)\
    ecs_bulk_add_type(world, ecs_type(type), filter)

/** Removes an entity from entities matching a filter.
 * This operation is the same as ecs_remove_id, but is applied to all 
 * entities that match the provided filter.
 *
 * @param world The world.
 * @param entity_remove The entity to remove.
 * @param filter The filter.
 */
FLECS_API
void ecs_bulk_remove_entity(
    ecs_world_t *world,
    ecs_entity_t entity_remove,
    const ecs_filter_t *filter);

/** Remove a type from entities matching a filter.
 * This operation is the same as ecs_remove_type but is applied to all entities
 * that match the provided filter.
 *
 * @param world The world.
 * @param type The type to remove.
 * @param filter The filter.
 */
FLECS_API
void ecs_bulk_remove_type(
    ecs_world_t *world,
    ecs_type_t type,
    const ecs_filter_t *filter);

/** Add a component / type / tag to entities matching a filter.
 * This operation is the same as ecs_remove but is applied to all entities
 * that match the provided filter.
 *
 * @param world The world.
 * @param type The component, type or tag to remove.
 * @param filter The filter.
 */
#define ecs_bulk_remove(world, type, filter)\
    ecs_bulk_remove_type(world, ecs_type(type), filter)

/** Add / remove type from entities matching a filter.
 * Combination of ecs_bulk_add_type and ecs_bulk_remove_type.
 *
 * @param world The world.
 * @param to_add The type to add.
 * @param to_remove The type to remove.
 * @param filter The filter.
 */
FLECS_API
void ecs_bulk_add_remove_type(
    ecs_world_t *world,
    ecs_type_t to_add,
    ecs_type_t to_remove,
    const ecs_filter_t *filter);

/** Add / remove component, type or tag from entities matching a filter.
 * Combination of ecs_bulk_add and ecs_bulk_remove.
 *
 * @param world The world.
 * @param to_add The component, type or tag to add.
 * @param to_remove The component, type or tag to remove.
 * @param filter The filter.
 */
#define ecs_bulk_add_remove(world, to_add, to_remove, filter)\
    ecs_bulk_add_remove_type(world, ecs_type(to_add), ecs_type(to_remove), filter)

/** Delete entities matching a filter.
 * This operation is the same as ecs_delete, but applies to all entities that
 * match a filter.
 *
 * @param world The world.
 * @param filter The filter.
 */
FLECS_API
void ecs_bulk_delete(
    ecs_world_t *world,
    const ecs_filter_t *filter);

#ifdef __cplusplus
}
#endif

#endif

#endif
