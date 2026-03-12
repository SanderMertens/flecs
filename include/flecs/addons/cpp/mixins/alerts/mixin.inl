/**
 * @file addons/cpp/mixins/alerts/mixin.inl
 * @brief Alert world mixin.
 */

/** Create alert.
 *
 * @ingroup cpp_addons_alerts
 * @memberof flecs::world
 */
template <typename... Comps, typename... Args>
flecs::alert_builder<Comps...> alert(Args &&... args) const;
