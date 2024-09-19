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
FLECS_API_STRUCT
struct alerts {
    using AlertsActive = EcsAlertsActive;
    using Instance = EcsAlertInstance;

    struct Alert { };
    struct Info { };
    struct Warning { };
    struct Error { };

    alerts(flecs::world& world);
};

FLECS_API_STRUCT
template <typename ... Components>
struct alert;

FLECS_API_STRUCT
template <typename ... Components>
struct alert_builder;

/** @} */

}
