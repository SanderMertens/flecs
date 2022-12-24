/**
 * @file addons/cpp/mixins/entity/mixin.inl
 * @brief Entity world mixin.
 */

/** Create an entity.
 * 
 * \memberof flecs::world
 * \ingroup cpp_entities
 */
template <typename... Args>
flecs::entity entity(Args &&... args) const;

/** Convert enum constant to entity.
 * 
 * \memberof flecs::world
 * \ingroup cpp_entities
 */
template <typename E, if_t< is_enum<E>::value > = 0>
flecs::entity id(E value) const;

/** Convert enum constant to entity.
 * 
 * \memberof flecs::world
 * \ingroup cpp_entities
 */
template <typename E, if_t< is_enum<E>::value > = 0>
flecs::entity entity(E value) const;

/** Create a prefab.
 * 
 * \memberof flecs::world
 * \ingroup cpp_entities
 */
template <typename... Args>
flecs::entity prefab(Args &&... args) const;

/** Create an entity that's associated with a type.
 * 
 * \memberof flecs::world
 * \ingroup cpp_entities
 */
template <typename T>
flecs::entity entity(const char *name = nullptr) const;

/** Create a prefab that's associated with a type.
 * 
 * \memberof flecs::world
 * \ingroup cpp_entities
 */
template <typename T>
flecs::entity prefab(const char *name = nullptr) const;
