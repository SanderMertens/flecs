/**
 * @file addons/cpp/mixins/metrics/decl.hpp
 * @brief Metrics declarations.
 */

#pragma once

#include "builder.hpp"

namespace flecs {

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

}
