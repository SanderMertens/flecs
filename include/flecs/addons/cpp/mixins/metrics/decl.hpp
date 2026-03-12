/**
 * @file addons/cpp/mixins/metrics/decl.hpp
 * @brief Metrics declarations.
 */

#pragma once

#include "builder.hpp"

namespace flecs {

/**
 * @defgroup cpp_addons_metrics Metrics
 * @ingroup cpp_addons
 * The metrics module extracts metrics from components and makes them available
 * through a unified component interface.
 *
 * @{
 */

/** Metrics module. */
struct metrics {
    /** Metric value component. */
    using Value = EcsMetricValue;
    /** Metric source component. */
    using Source = EcsMetricSource;

    /** Metric instance tag. */
    struct Instance { };
    /** Metric tag. */
    struct Metric { };
    /** Counter metric kind. */
    struct Counter { };
    /** Counter increment metric kind. */
    struct CounterIncrement { };
    /** Counter id metric kind. */
    struct CounterId { };
    /** Gauge metric kind. */
    struct Gauge { };

    /** Construct the metrics module. */
    metrics(flecs::world& world);
};

/** @} */

}
