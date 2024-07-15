/**
 * @file addons/metrics.h
 * @brief Metrics module.
 *
 * The metrics module extracts metrics from components and makes them available
 * through a unified component interface.
 */

#ifdef FLECS_METRICS

/**
 * @defgroup c_addons_metrics Metrics
 * @ingroup c_addons
 * Collect user-defined metrics from ECS data.
 *
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

/** Flecs metrics module. */
FLECS_API extern ECS_COMPONENT_DECLARE(FlecsMetrics);

/** Tag added to metrics, and used as first element of metric kind pair. */
FLECS_API extern ECS_TAG_DECLARE(EcsMetric);

/** Metric that has monotonically increasing value. */
FLECS_API extern ECS_TAG_DECLARE(EcsCounter);

/** Counter metric that is auto-incremented by source value. */
FLECS_API extern ECS_TAG_DECLARE(EcsCounterIncrement);

/** Counter metric that counts the number of entities with an id. */
FLECS_API extern ECS_TAG_DECLARE(EcsCounterId);

/** Metric that represents current value. */
FLECS_API extern ECS_TAG_DECLARE(EcsGauge);

/** Tag added to metric instances. */
FLECS_API extern ECS_TAG_DECLARE(EcsMetricInstance);

/** Component with metric instance value. */
FLECS_API extern ECS_COMPONENT_DECLARE(EcsMetricValue);

/** Component with entity source of metric instance. */
FLECS_API extern ECS_COMPONENT_DECLARE(EcsMetricSource);

/** Component that stores metric value. */
typedef struct EcsMetricValue {
    double value;
} EcsMetricValue;

/** Component that stores metric source. */
typedef struct EcsMetricSource {
    ecs_entity_t entity;
} EcsMetricSource;

/** Used with ecs_metric_init to create metric. */
typedef struct ecs_metric_desc_t {
    int32_t _canary;

    /** Entity associated with metric */
    ecs_entity_t entity;

    /** Entity associated with member that stores metric value. Must not be set
     * at the same time as id. Cannot be combined with EcsCounterId. */
    ecs_entity_t member;

    /* Member dot expression. Can be used instead of member and supports nested
     * members. Must be set together with id and should not be set at the same
     * time as member. */
    const char *dotmember;

    /** Tracks whether entities have the specified component id. Must not be set
     * at the same time as member. */
    ecs_id_t id;

    /** If id is a (R, *) wildcard and relationship R has the OneOf property,
     * setting this value to true will track individual targets.
     * If the kind is EcsCountId and the id is a (R, *) wildcard, this value
     * will create a metric per target. */
    bool targets;

    /** Must be EcsGauge, EcsCounter, EcsCounterIncrement or EcsCounterId */
    ecs_entity_t kind;

    /** Description of metric. Will only be set if FLECS_DOC addon is enabled */
    const char *brief;
} ecs_metric_desc_t;

/** Create a new metric.
 * Metrics are entities that store values measured from a range of different
 * properties in the ECS storage. Metrics provide a single unified interface to
 * discovering and reading these values, which can be useful for monitoring
 * utilities, or for debugging.
 *
 * Examples of properties that can be measured by metrics are:
 *  - Component member values
 *  - How long an entity has had a specific component
 *  - How long an entity has had a specific target for a relationship
 *  - How many entities have a specific component
 *
 * Metrics can either be created as a "gauge" or "counter". A gauge is a metric
 * that represents the value of something at a specific point in time, for
 * example "velocity". A counter metric represents a value that is monotonically
 * increasing, for example "miles driven".
 *
 * There are three different kinds of counter metric kinds:
 * - EcsCounter
 *   When combined with a member, this will store the actual value of the member
 *   in the metric. This is useful for values that are already counters, such as
 *   a MilesDriven component.
 *   This kind creates a metric per entity that has the member/id.
 *
 * - EcsCounterIncrement
 *   When combined with a member, this will increment the value of the metric by
 *   the value of the member * delta_time. This is useful for values that are
 *   not counters, such as a Velocity component.
 *   This kind creates a metric per entity that has the member.
 *
 * - EcsCounterId
 *   This metric kind will count the number of entities with a specific
 *   (component) id. This kind creates a single metric instance for regular ids,
 *   and a metric instance per target for wildcard ids when targets is set.
 *
 * @param world The world.
 * @param desc Metric description.
 * @return The metric entity.
 */
FLECS_API
ecs_entity_t ecs_metric_init(
    ecs_world_t *world,
    const ecs_metric_desc_t *desc);

/** Shorthand for creating a metric with ecs_metric_init().
 *
 * Example:
 *
 * @code
 * ecs_metric(world, {
 *   .member = ecs_lookup(world, "Position.x")
 *   .kind = EcsGauge
 * });
 * @endcode
 */
#define ecs_metric(world, ...)\
    ecs_metric_init(world, &(ecs_metric_desc_t) __VA_ARGS__ )

/** Metrics module import function.
 * Usage:
 * @code
 * ECS_IMPORT(world, FlecsMetrics)
 * @endcode
 * 
 * @param world The world.
 */
FLECS_API
void FlecsMetricsImport(
    ecs_world_t *world);

#ifdef __cplusplus
}
#endif

#endif

/** @} */

#endif
