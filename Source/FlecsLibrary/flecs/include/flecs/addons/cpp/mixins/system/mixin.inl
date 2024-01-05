/**
 * @file addons/cpp/mixins/system/mixin.inl
 * @brief System module world mixin.
 */

/** 
 * \memberof flecs::world
 * \ingroup cpp_addons_system
*/

/** Upcast entity to a system.
 * The provided entity must be a system.
 * 
 * @param e The entity.
 * @return A system object.
 */
flecs::system system(flecs::entity e) const;

/** Create a new system.
 * 
 * @tparam Components The components to match on.
 * @tparam Args Arguments passed to the constructor of flecs::system_builder.
 * @return System builder.
 */
template <typename... Components, typename... Args>
flecs::system_builder<Components...> system(Args &&... args) const;

/** @} */
