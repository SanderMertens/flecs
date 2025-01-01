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
 * @defgroup c_addons_alerts Alerts
 * @ingroup c_addons
 * Create alerts from monitoring queries.
 *
 * @{
 */

#ifndef FLECS_ALERTS_H
#define FLECS_ALERTS_H

#ifndef FLECS_PIPELINE
#define FLECS_PIPELINE
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define ECS_ALERT_MAX_SEVERITY_FILTERS (4)

/** Module id. */
FLECS_API extern ECS_COMPONENT_DECLARE(FlecsAlerts);

/* Module components */

FLECS_API extern ECS_COMPONENT_DECLARE(EcsAlert);          /**< Component added to alert, and used as first element of alert severity pair. */
FLECS_API extern ECS_COMPONENT_DECLARE(EcsAlertInstance);  /**< Component added to alert instance. */
FLECS_API extern ECS_COMPONENT_DECLARE(EcsAlertsActive);   /**< Component added to alert source which tracks how many active alerts there are. */
FLECS_API extern ECS_COMPONENT_DECLARE(EcsAlertTimeout);   /**< Component added to alert which tracks how long an alert has been inactive. */

/* Alert severity tags */
FLECS_API extern ECS_TAG_DECLARE(EcsAlertInfo);            /**< Info alert severity. */
FLECS_API extern ECS_TAG_DECLARE(EcsAlertWarning);         /**< Warning alert severity. */
FLECS_API extern ECS_TAG_DECLARE(EcsAlertError);           /**< Error alert severity. */
FLECS_API extern ECS_TAG_DECLARE(EcsAlertCritical);        /**< Critical alert severity. */

/** Component added to alert instance. */
typedef struct EcsAlertInstance {
    char *message; /**< Generated alert message */
} EcsAlertInstance;

/** Map with active alerts for entity. */
typedef struct EcsAlertsActive {
    int32_t info_count;    /**< Number of alerts for source with info severity */
    int32_t warning_count; /**< Number of alerts for source with warning severity */
    int32_t error_count;   /**< Number of alerts for source with error severity */
    ecs_map_t alerts;
} EcsAlertsActive;

/** Alert severity filter. 
 * A severity filter can adjust the severity of an alert based on whether an
 * entity in the alert query has a specific component. For example, a filter
 * could check if an entity has the "Production" tag, and increase the default
 * severity of an alert from Warning to Error.
 */
typedef struct ecs_alert_severity_filter_t {
    ecs_entity_t severity; /* Severity kind */
    ecs_id_t with;         /* Component to match */
    const char *var;       /* Variable to match component on. Do not include the
                            * '$' character. Leave to NULL for $this. */
    int32_t _var_index;    /* Index of variable in filter (do not set) */
} ecs_alert_severity_filter_t;

/** Alert descriptor, used with ecs_alert_init(). */
typedef struct ecs_alert_desc_t {
    int32_t _canary;

    /** Entity associated with alert */
    ecs_entity_t entity;

    /** Alert query. An alert will be created for each entity that matches the
     * specified query. The query must have at least one term that uses the
     * $this variable (default). */
    ecs_query_desc_t query;

    /** Template for alert message. This string is used to generate the alert
     * message and may refer to variables in the query result. The format for
     * the template expressions is as specified by ecs_script_string_interpolate().
     *
     * Examples:
     *
     *     "$this has Position but not Velocity"
     *     "$this has a parent entity $parent without Position"
     */
    const char *message;

    /** User friendly name. Will only be set if FLECS_DOC addon is enabled. */
    const char *doc_name;

    /** Description of alert. Will only be set if FLECS_DOC addon is enabled */
    const char *brief;

    /** Metric kind. Must be EcsAlertInfo, EcsAlertWarning, EcsAlertError or
     * EcsAlertCritical. Defaults to EcsAlertError. */
    ecs_entity_t severity;

    /** Severity filters can be used to assign different severities to the same
     * alert. This prevents having to create multiple alerts, and allows
     * entities to transition between severities without resetting the
     * alert duration (optional). */
    ecs_alert_severity_filter_t severity_filters[ECS_ALERT_MAX_SEVERITY_FILTERS];

    /** The retain period specifies how long an alert must be inactive before it
     * is cleared. This makes it easier to track noisy alerts. While an alert is
     * inactive its duration won't increase.
     * When the retain period is 0, the alert will clear immediately after it no
     * longer matches the alert query. */
    ecs_ftime_t retain_period;

    /** Alert when member value is out of range. Uses the warning/error ranges
     * assigned to the member in the MemberRanges component (optional). */
    ecs_entity_t member;

    /** (Component) id of member to monitor. If left to 0 this will be set to
     * the parent entity of the member (optional). */
    ecs_id_t id;

    /** Variable from which to fetch the member (optional). When left to NULL
     * 'id' will be obtained from $this. */
    const char *var;
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
 * which contains a map with active alerts for the entity. This component
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

/** Create a new alert.
 * @see ecs_alert_init()
 */
#define ecs_alert(world, ...)\
    ecs_alert_init(world, &(ecs_alert_desc_t)__VA_ARGS__)

/** Return number of active alerts for entity.
 * When a valid alert entity is specified for the alert parameter, the operation
 * will return whether the specified alert is active for the entity. When no
 * alert is specified, the operation will return the total number of active
 * alerts for the entity.
 *
 * @param world The world.
 * @param entity The entity.
 * @param alert The alert to test for (optional).
 * @return The number of active alerts for the entity.
 */
FLECS_API
int32_t ecs_get_alert_count(
    const ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_t alert);

/** Return alert instance for specified alert.
 * This operation returns the alert instance for the specified alert. If the
 * alert is not active for the entity, the operation will return 0.
 *
 * @param world The world.
 * @param entity The entity.
 * @param alert The alert to test for.
 * @return The alert instance for the specified alert.
 */
FLECS_API
ecs_entity_t ecs_get_alert(
    const ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_t alert);

/** Alert module import function.
 * Usage:
 * @code
 * ECS_IMPORT(world, FlecsAlerts)
 * @endcode
 * 
 * @param world The world.
 */
FLECS_API
void FlecsAlertsImport(
    ecs_world_t *world);

#ifdef __cplusplus
}
#endif

#endif

/** @} */

#endif
