/**
 * @file addons/cpp/mixins/monitor/impl.hpp
 * @brief Monitor module implementation.
 */

#pragma once

namespace flecs {

inline monitor::monitor(flecs::world& world) {
    /* Import C module  */
    FlecsMonitorImport(world);

}

}