/**
 * @file addons/cpp/mixins/observer/mixin.inl
 * @brief Observer world mixin.
 */

/** Observer builder.
 * 
 * \memberof flecs::world
 * \ingroup cpp_observers
 */

/** Upcast entity to an observer.
 * The provided entity must be an observer.
 * 
 * @param e The entity.
 * @return An observer object.
 */
flecs::observer observer(flecs::entity e) const;

/** Create a new observer.
 * 
 * @tparam Components The components to match on.
 * @tparam Args Arguments passed to the constructor of flecs::observer_builder.
 * @return Observer builder.
 */
template <typename... Components, typename... Args>
flecs::observer_builder<Components...> observer(Args &&... args) const;

/** @} */
