
/** Create a new trigger.
 * 
 * @tparam Components The components to match on.
 * @tparam Args Arguments passed to the constructor of flecs::trigger_builder.
 * @return Trigger builder.
 */
template <typename... Components, typename... Args>
flecs::trigger_builder<Components...> trigger(Args &&... args) const;
