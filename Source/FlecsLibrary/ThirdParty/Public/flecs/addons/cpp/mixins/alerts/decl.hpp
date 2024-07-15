/**
 * @file addons/cpp/mixins/alerts/decl.hpp
 * @brief Alert declarations.
 */

#pragma once

namespace flecs {

/**
 * @defgroup cpp_addons_alerts Alerts
 * @ingroup cpp_addons
 * Alert implementation.
 *
 * @{
 */

/** Module */
struct alerts {
    using AlertsActive = EcsAlertsActive;
    using Instance = EcsAlertInstance;

    struct Alert { };
    struct Info { };
    struct Warning { };
    struct Error { };

    alerts(flecs::world& world);
};

template <typename ... Components>
struct alert;

template <typename ... Components>
struct alert_builder;

/** @} */

}
