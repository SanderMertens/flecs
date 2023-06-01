/**
 * @file addons/cpp/mixins/alerts/decl.hpp
 * @brief Alert declarations.
 */

#pragma once

namespace flecs {

/**
 * @defgroup cpp_addons_alerts Alerts
 * @brief Alert implementation.
 * 
 * \ingroup cpp_addons
 * @{
 */

static const flecs::entity_t Alert = ecs_id(EcsAlert);
using AlertInstance = EcsAlertInstance;
using AlertSource = EcsAlertSource;
using AlertsActive = EcsAlertsActive;

/** Module */
struct alerts {
    alerts(flecs::world& world);
};

template <typename ... Components>
struct alert;

template <typename ... Components>
struct alert_builder;

/** @} */

}
