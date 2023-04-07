/**
 * @file addons/cpp/mixins/metrics/decl.hpp
 * @brief Metrics declarations.
 */

#pragma once

#include "builder.hpp"

namespace flecs {

struct metrics {
    using Instance = EcsMetricInstance;
    using Source = EcsMetricSource;

    struct Counter { };
    struct CounterIncrement { };
    struct Gauge { };

    metrics(flecs::world& world);
};

}
