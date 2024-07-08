/**
 * @file addons/cpp/mixins/stats/impl.hpp
 * @brief Monitor module implementation.
 */

#pragma once

namespace flecs {

inline stats::stats(flecs::world& world) {
    /* Import C module  */
    FlecsStatsImport(world);

    world.component<WorldSummary>();
    world.component<WorldStats>();
    world.component<PipelineStats>();
}

}
