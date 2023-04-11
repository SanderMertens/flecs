/**
 * @file addons/cpp/mixins/meta/untyped_component.inl
 * @brief Metrics component mixin.
 */

/**
 * \memberof flecs::component
 * \ingroup cpp_addons_metrics
 * 
 * @{
 */

/** Register member as metric.
 * 
 * @tparam Kind Metric kind (Counter, CounterIncrement or Gauge).
 * @param parent Parent entity of the metric.
 * 
 * \ingroup cpp_addons_metrics
 * \memberof flecs::world
 */
template <typename Kind>
untyped_component& metric(flecs::entity_t parent = 0);

/** Register member as metric.
 * 
 * @tparam Kind Metric kind (Counter, CounterIncrement or Gauge).
 * @param parent Parent entity of the metric.
 * 
 * \ingroup cpp_addons_metrics
 * \memberof flecs::world
 */
template <typename Kind>
untyped_component& metric(const char *parent);


/** @} */
