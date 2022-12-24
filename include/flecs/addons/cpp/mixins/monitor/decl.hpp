/**
 * @file addons/cpp/mixins/monitor/decl.hpp
 * @brief Monitor module declarations.
 */

#pragma once

namespace flecs {

/**
 * @defgroup cpp_addons_monitor Monitor
 * @brief The monitor addon periodically tracks statistics for the world and systems.
 * 
 * \ingroup cpp_addons
 * @{
 */

/** Component that stores world statistics */
using WorldStats = EcsWorldStats;

/** Component that stores system/pipeline statistics */
using PipelineStats = EcsPipelineStats;
    
struct monitor {
    monitor(flecs::world& world);
};

/** @} */

}