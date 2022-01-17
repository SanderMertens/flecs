
/** Register a component.
 */
template <typename T, typename... Args>
flecs::component<T> component(Args &&... args) const;

/** Register a component.
 */
template <typename... Args>
flecs::untyped_component component(Args &&... args) const;
