/**
 * @file addons/metrics.h
 * @brief Metrics module.
 *
 * The metrics module extracts metrics from components and makes them available
 * through a unified component interface.
 */

#ifdef FLECS_METRICS

/**
 * @defgroup c_addons_monitor Monitor
 * @brief  * The metrics module extracts metrics from components and makes them 
 *           available through a unified component interface.
 * 
 * \ingroup c_addons
 * @{
 */

#ifndef FLECS_METRICS_H
#define FLECS_METRICS_H

#ifndef FLECS_META
#define FLECS_META
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

FLECS_API extern ECS_COMPONENT_DECLARE(FlecsMetrics);

/** Tag added to metrics, and used as first element of metric kind pair */
FLECS_API extern ECS_TAG_DECLARE(EcsMetric);

/** Metric that has monotonically increasing value */
FLECS_API extern ECS_TAG_DECLARE(EcsCounter);

/** Counter metric that is auto-incremented by source value */
FLECS_API extern ECS_TAG_DECLARE(EcsCounterIncrement);

/** Metric that represents current value */
FLECS_API extern ECS_TAG_DECLARE(EcsGauge);

/** Tag added to metric instances */
FLECS_API extern ECS_COMPONENT_DECLARE(EcsMetricInstance);

/** Component with metric instance value */
FLECS_API extern ECS_COMPONENT_DECLARE(EcsMetricValue);

/** Component with entity source of metric instance */
FLECS_API extern ECS_COMPONENT_DECLARE(EcsMetricSource);

typedef struct EcsMetricValue {
    double value;
} EcsMetricValue;

typedef struct EcsMetricSource {
    ecs_entity_t entity;
} EcsMetricSource;

typedef struct ecs_metric_desc_t {
    /* Entity associated with metric */
    ecs_entity_t entity;
    
    /* Entity associated with member that stores metric value. Must not be set
     * at the same time as id. */
    ecs_entity_t member;

    /* Tracks whether entities have the specified component id. Must not be set
     * at the same time as member. */
    ecs_id_t id;

    /* If id is a (R, *) wildcard and relationship R has the OneOf property, the
     * setting this value to true will track individual targets. */
    bool targets;

    /* Must be either EcsGauge, EcsCounter or EcsCounterIncrement. */
    ecs_entity_t kind;

    /* Description of metric. Will only be set if FLECS_DOC addon is enabled */
    const char *brief;
} ecs_metric_desc_t;

FLECS_API
ecs_entity_t ecs_metric_init(
    ecs_world_t *world,
    const ecs_metric_desc_t *desc);

/** Shorthand for creating a metric with ecs_metric_init.
 *
 * Example:
 *   ecs_metric(world, {
 *     .member = ecs_lookup_fullpath(world, "Position.x")
 *     .kind = EcsGauge
 *   });
 */
#define ecs_metric(world, ...)\
    ecs_metric_init(world, &(ecs_metric_desc_t) __VA_ARGS__ )

/* Module import */
FLECS_API
void FlecsMetricsImport(
    ecs_world_t *world);

#ifdef __cplusplus
}
#endif

#endif

/** @} */

#endif
