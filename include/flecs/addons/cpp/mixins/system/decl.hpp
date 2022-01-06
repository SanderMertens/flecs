#pragma once

namespace flecs {

using TickSource = EcsTickSource;

struct system;

template<typename ... Components>
struct system_builder;

}
