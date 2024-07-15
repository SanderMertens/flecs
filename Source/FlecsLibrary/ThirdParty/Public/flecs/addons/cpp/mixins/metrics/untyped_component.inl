/**
 * @file addons/cpp/mixins/meta/untyped_component.inl
 * @brief Metrics component mixin.
 */

/**
 * @memberof flecs::component
 * @ingroup cpp_addons_metrics
 * 
 * @{
 */

/** Register member as metric.
 * When no explicit name is provided, this operation will derive the metric name
 * from the member name. When the member name is "value", the operation will use
 * the name of the component.
 * 
 * When the brief parameter is provided, it is set on the metric as if 
 * set_doc_brief is used. The brief description can be obtained with 
 * get_doc_brief.
 * 
 * @tparam Kind Metric kind (Counter, CounterIncrement or Gauge).
 * @param parent Parent entity of the metric (optional).
 * @param brief Description for metric (optional).
 * @param name Name of metric (optional).
 */
template <typename Kind>
untyped_component& metric(
    flecs::entity_t parent = 0, 
    const char *brief = nullptr, 
    const char *name = nullptr);

/** @} */
