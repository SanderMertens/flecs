#pragma once

namespace flecs {

using WorldStats = EcsWorldStats;
using PipelineStats = EcsPipelineStats;
    
struct monitor {
    monitor(flecs::world& world);
};

}