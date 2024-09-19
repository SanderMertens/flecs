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
FLECS_API_STRUCT
using WorldStats = EcsWorldStats;

/** Component that stores system/pipeline statistics */
FLECS_API_STRUCT
using PipelineStats = EcsPipelineStats;

/** Component with world summary stats */
FLECS_API_STRUCT
using WorldSummary = EcsWorldSummary;

FLECS_API_STRUCT
struct stats {
    stats(flecs::world& world);
};

/** @} */

}
