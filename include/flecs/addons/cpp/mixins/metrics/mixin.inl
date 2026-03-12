/**
 * @file addons/cpp/mixins/metrics/mixin.inl
 * @brief Metrics world mixin.
 */

/** Create a metric.
 *
 * @ingroup cpp_addons_metrics
 * @memberof flecs::world
 */
template <typename... Args>
flecs::metric_builder metric(Args &&... args) const;
