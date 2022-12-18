/**
 * @file addons/cpp/mixins/rest/impl.hpp
 * @brief Rest module implementation.
 */

#pragma once

namespace flecs {
namespace rest {
namespace _ {

inline void init(flecs::world& world) {
    world.component<Rest>("flecs::rest::Rest");
}
 
} // namespace _
} // namespace rest
} // namespace flecs
