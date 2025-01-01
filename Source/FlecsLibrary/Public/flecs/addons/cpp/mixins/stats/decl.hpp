/**
 * @file addons/cpp/mixins/stats/decl.hpp
 * @brief Stats module declarations.
 */

#pragma once

namespace flecs {

/**
 * @defgroup cpp_addons_stats Stats
 * @ingroup cpp_addons
 * The stats addon tracks statistics for the world and systems.
 *
 * @{
 */

/** Component that stores world statistics */
using WorldStats = EcsWorldStats;

/** Component that stores system/pipeline statistics */
using PipelineStats = EcsPipelineStats;

/** Component with world summary stats */
using WorldSummary = EcsWorldSummary;

struct stats {
    stats(flecs::world& world);
};

/** @} */

}
