/**
 * @file addons/cpp/mixins/entity/mixin.inl
 * @brief Entity world mixin.
 */

/** Create a prefab that's associated with a type.
 * 
 * @memberof flecs::world
 * @ingroup cpp_entities
 */
template <typename T>
flecs::entity prefab(const char *name = nullptr) const;

/** Create a prefab.
 * 
 * @memberof flecs::world
 * @ingroup cpp_entities
 */
template <typename... Args>
flecs::entity prefab(Args &&... args) const;
