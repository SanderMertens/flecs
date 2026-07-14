/**
 * @file addons/cpp/mixins/entity_ranges/mixin.inl
 * @brief Entity ranges world mixin.
 */

/**
 * @defgroup cpp_addons_entity_ranges Entity Ranges
 * @ingroup cpp_addons
 * Entity id range management.
 *
 * @{
 * @}
 */

/**
 * @memberof flecs::world
 * @ingroup cpp_addons_entity_ranges
 *
 * @{
 */

/** Create a new entity id range.
 * @see ecs_entity_range_new()
 */
const ecs_entity_range_t* range_new(uint32_t min, uint32_t max) const;

/** Set the active entity id range.
 * @see ecs_entity_range_set()
 */
void range_set(const ecs_entity_range_t *range) const;

/** Get the currently active entity id range.
 * @see ecs_entity_range_get()
 */
const ecs_entity_range_t* range_get() const;

/** @} */
