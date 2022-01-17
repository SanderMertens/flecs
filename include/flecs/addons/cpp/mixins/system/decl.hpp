#pragma once

namespace flecs {

using TickSource = EcsTickSource;

struct system;

template<typename ... Components>
struct system_builder;

namespace _ {

void system_init(flecs::world& world);

} // namespace _
} // namespace flecs
