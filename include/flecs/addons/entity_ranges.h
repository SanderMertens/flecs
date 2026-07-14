/**
 * @file addons/entity_ranges.h
 * @brief Entity id range management.
 */

#ifdef FLECS_ENTITY_RANGES

#ifndef FLECS_ENTITY_RANGES_H
#define FLECS_ENTITY_RANGES_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup c_addons_entity_ranges Entity Ranges
 * @ingroup c_addons
 * Entity id range management.
 *
 * @{
 */

/** Type that stores an entity id range.
 * Returned by ecs_entity_range_new(), used with ecs_entity_range_set().
 */
typedef struct ecs_entity_range_t {
    uint32_t min;           /**< First id in range (inclusive). */
    uint32_t max;           /**< Last id in range (inclusive, 0 = unlimited). */
    uint32_t cur;           /**< Last issued id in range. */
    ecs_vec_t recycled;     /**< Recycled entity ids (vec<entity_t>). */
} ecs_entity_range_t;

/** Create a new entity range.
 * This function creates a range that constrains new entity identifiers returned
 * by the specified [min, max] interval. Each range maintains its own list of
 * recycled entity ids, which ensures that recycled ids always respect the
 * configured range. If `max` is set to 0, the range is unbounded.
 *
 * Entity ranges cannot be deleted once created. Use ecs_entity_range_set() to
 * activate a range.
 *
 * @param world The world.
 * @param min The first entity id in the range (inclusive).
 * @param max The last entity id in the range (inclusive, 0 = unlimited).
 * @return A pointer to the new range. Does not need to be freed.
 */
FLECS_API
const ecs_entity_range_t* ecs_entity_range_new(
    ecs_world_t *world,
    uint32_t min,
    uint32_t max);

/** Set the active entity range.
 * This function activates a range created with ecs_entity_range_new().
 * When a range is activated, new entity identifiers will fall within the
 * specified [min, max] interval, including recycled identifiers.
 *
 * When the active range is out of available ids, operations that create new
 * entity ids will assert.
 *
 * The operation only accepts ranges that have been created by
 * ecs_entity_range_new().
 *
 * @param world The world.
 * @param range The range to activate.
 */
FLECS_API
void ecs_entity_range_set(
    ecs_world_t *world,
    const ecs_entity_range_t *range);

/** Get the currently active entity id range.
 * Returns the range set by ecs_entity_range_set(), or NULL if no range is
 * active.
 *
 * @param world The world.
 * @return The active range, or NULL.
 */
FLECS_API
const ecs_entity_range_t* ecs_entity_range_get(
    const ecs_world_t *world);

/** @} */

#ifdef __cplusplus
}
#endif

#endif

#endif // FLECS_ENTITY_RANGES
