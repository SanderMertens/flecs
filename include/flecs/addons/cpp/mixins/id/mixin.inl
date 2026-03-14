/**
 * @file addons/cpp/mixins/id/mixin.inl
 * @brief ID world mixin.
 */

/** Get ID from a type.
 * 
 * @memberof flecs::world
 */
template <typename T>
flecs::id id() const;

/** ID factory.
 * 
 * @memberof flecs::world
 */
template <typename ... Args>
flecs::id id(Args&&... args) const;

/** Get pair ID from relationship and object.
 * 
 * @memberof flecs::world
 */
template <typename First, typename Second>
flecs::id pair() const;

/** Get pair ID from relationship and object.
 * 
 * @memberof flecs::world
 */
template <typename First>
flecs::id pair(entity_t o) const;

/** Get pair ID from relationship and object.
 * 
 * @memberof flecs::world
 */
flecs::id pair(entity_t r, entity_t o) const;
