/**
 * @file addons/cpp/mixins/monitor/decl.hpp
 * @brief Monitor module declarations.
 */

#pragma once

namespace flecs {

using WorldStats = EcsWorldStats;
using PipelineStats = EcsPipelineStats;
    
struct monitor {
    monitor(flecs::world& world);
};

}