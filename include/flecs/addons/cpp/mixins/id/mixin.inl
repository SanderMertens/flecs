/**
 * @file addons/cpp/mixins/id/mixin.inl
 * @brief Id world mixin.
 */

/** Get id from a type.
 * 
 * \memberof flecs::world
 */
template <typename T>
flecs::id id() const;

/** Id factory.
 * 
 * \memberof flecs::world
 */
template <typename ... Args>
flecs::id id(Args&&... args) const;

/** Get pair id from relationship, object.
 * 
 * \memberof flecs::world
 */
template <typename First, typename Second>
flecs::id pair() const;

/** Get pair id from relationship, object.
 * 
 * \memberof flecs::world
 */
template <typename First>
flecs::id pair(entity_t o) const;

/** Get pair id from relationship, object.
 * 
 * \memberof flecs::world
 */
flecs::id pair(entity_t r, entity_t o) const;
