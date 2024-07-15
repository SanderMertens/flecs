/**
 * @file addons/cpp/mixins/alerts/entity_view.inl
 * @brief Alerts entity mixin.
 */

/** Return number of alerts for entity.
 * 
 * @memberof flecs::entity_view
 * @ingroup cpp_addons_alerts
 */
int32_t alert_count(flecs::entity_t alert = 0) const {
    return ecs_get_alert_count(world_, id_, alert);
}
