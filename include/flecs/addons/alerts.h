/**
 * @file addons/alerts.h
 * @brief Alerts module.
 *
 * The alerts module enables applications to register alerts for when certain
 * conditions are met. Alerts are registered as queries, and automatically
 * become active when entities match the alert query.
 */

#ifdef FLECS_ALERTS

/**
 * @defgroup c_addons_alerts Alert
 * @brief  * The alerts module enables applications to register alerts for when
 *           certain conditions are met. Alerts are registered as queries, and 
 *           automatically become active for matching entities.
 * 
 * \ingroup c_addons
 * @{
 */

#ifndef FLECS_ALERTS_H
#define FLECS_ALERTS_H

#ifndef FLECS_RULES
#define FLECS_RULES
#endif

#ifndef FLECS_PIPELINE
#define FLECS_PIPELINE
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Module id */
FLECS_API extern ECS_COMPONENT_DECLARE(FlecsAlerts);

/* Module components */
FLECS_API extern ECS_COMPONENT_DECLARE(EcsAlert);
FLECS_API extern ECS_COMPONENT_DECLARE(EcsAlertInstance);
FLECS_API extern ECS_COMPONENT_DECLARE(EcsAlertsActive);

/** Alert information. Added to each alert instance */
typedef struct EcsAlertInstance {
    char *message;
} EcsAlertInstance;

/** Number of active alerts. Added to alert source */
typedef struct EcsAlertsActive {
    int32_t count;
} EcsAlertsActive;

typedef struct ecs_alert_desc_t { 
    int32_t _canary;

    /* Entity associated with alert */
    ecs_entity_t entity;

    /* Alert query. An alert will be created for each entity that matches the
     * specified query. The query must have at least one term that uses the
     * $this variable (default). */
    ecs_filter_desc_t filter;

    /* Template for alert message. This string is used to generate the alert
     * message and may refer to variables in the query result. The format for
     * the template expressions is as specified by ecs_interpolate_string.
     * 
     * Examples:
     *   "$this has Position but not Velocity"
     *   "$this has a parent entity $parent without Position"
     */
    const char *message;

    /* Description of metric. Will only be set if FLECS_DOC addon is enabled */
    const char *brief;
} ecs_alert_desc_t;

/** Create a new alert.
 * An alert is a query that is evaluated periodically and creates alert 
 * instances for each entity that matches the query. Alerts can be used to 
 * automate detection of errors in an application.
 * 
 * Alerts are automatically cleared when a query is no longer true for an alert
 * instance. At most one alert instance will be created per matched entity.
 * 
 * Alert instances have three components:
 * - AlertInstance: contains the alert message for the instance
 * - MetricSource: contains the entity that triggered the alert
 * - MetricValue: contains how long the alert has been active
 * 
 * Alerts reuse components from the metrics addon so that alert instances can be
 * tracked and discovered as metrics. Just like metrics, alert instances are
 * created as children of the alert.
 * 
 * When an entity has active alerts, it will have the EcsAlertsActive component
 * which contains the number of active alerts for the entity. This component
 * will be automatically removed once all alerts are cleared for the entity.
 * 
 * @param world The world.
 * @param desc Alert description.
 * @return The alert entity.
 */
FLECS_API
ecs_entity_t ecs_alert_init(
    ecs_world_t *world,
    const ecs_alert_desc_t *desc);

#define ecs_alert(world, ...)\
    ecs_alert_init(world, &(ecs_alert_desc_t)__VA_ARGS__)

/* Module import */
FLECS_API
void FlecsAlertsImport(
    ecs_world_t *world);

#ifdef __cplusplus
}
#endif

#endif

/** @} */

#endif
