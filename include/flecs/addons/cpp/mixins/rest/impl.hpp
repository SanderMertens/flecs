#pragma once

namespace flecs {
namespace rest {
namespace _ {

inline void init(flecs::world& world) {
    world.component<rest::Rest>("flecs::rest::Rest");
}
 
} // namespace _
} // namespace rest
} // namespace flecs
