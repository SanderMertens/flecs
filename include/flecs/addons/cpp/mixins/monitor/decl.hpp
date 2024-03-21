/**
 * @file addons/cpp/mixins/monitor/decl.hpp
 * @brief Monitor module declarations.
 */

#pragma once

namespace flecs {

/**
 * @defgroup cpp_addons_monitor Monitor
 * @ingroup cpp_addons
 * The monitor addon periodically tracks statistics for the world and systems.
 *
 * @{
 */

/** Component that stores world statistics */
using WorldStats = EcsWorldStats;

/** Component that stores system/pipeline statistics */
using PipelineStats = EcsPipelineStats;

/** Component with world summary stats */
using WorldSummary = EcsWorldSummary;

struct monitor {
    monitor(flecs::world& world);
};

/** @} */

}
