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

struct metrics {
    using Value = EcsMetricValue;
    using Source = EcsMetricSource;

    struct Instance { };
    struct Metric { };
    struct Counter { };
    struct CounterIncrement { };
    struct CounterId { };
    struct Gauge { };

    metrics(flecs::world& world);
};

/** @} */

}
