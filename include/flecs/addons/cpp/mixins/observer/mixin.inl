
/** Create a new observer.
 * 
 * @tparam Components The components to match on.
 * @tparam Args Arguments passed to the constructor of flecs::observer_builder.
 * @return Observer builder.
 */
template <typename... Components, typename... Args>
flecs::observer_builder<Components...> observer(Args &&... args) const;
