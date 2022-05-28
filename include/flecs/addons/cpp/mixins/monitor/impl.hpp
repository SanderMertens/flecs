#pragma once

namespace flecs {

inline monitor::monitor(flecs::world& world) {
    /* Import C module  */
    FlecsMonitorImport(world);

}

}