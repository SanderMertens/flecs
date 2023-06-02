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

#ifndef FLECS_UNITS
#define FLECS_UNITS
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
FLECS_API extern ECS_COMPONENT_DECLARE(EcsAlertSource);
FLECS_API extern ECS_COMPONENT_DECLARE(EcsAlertsActive);

/** Alert information. Added to each alert instance */
typedef struct EcsAlertInstance {
    char *message;
    double duration;
} EcsAlertInstance;

/** Source of alert. Added to each alert instance */
typedef struct EcsAlertSource {
    ecs_entity_t entity;
} EcsAlertSource;

/** Number of active alerts. Added to alert source */
typedef struct EcsAlertsActive {
    int32_t count;
} EcsAlertsActive;

typedef struct ecs_alert_desc_t { 
    int32_t _canary;
    ecs_entity_t entity;
    ecs_filter_desc_t filter;
    const char *message;
} ecs_alert_desc_t;

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
