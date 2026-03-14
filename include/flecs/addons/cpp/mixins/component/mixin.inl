/**
 * @file addons/cpp/mixins/component/mixin.inl
 * @brief Component mixin.
 */

/** Find or register a component.
 * 
 * @ingroup cpp_components
 * @memberof flecs::world
 */
template <typename T, typename... Args>
flecs::component<T> component(Args &&... args) const;

/** Find or register an untyped component.
 * Method available on the flecs::world class.
 * 
 * @ingroup cpp_components
 * @memberof flecs::world
 */
template <typename... Args>
flecs::untyped_component component(Args &&... args) const;
