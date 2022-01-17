#pragma once

namespace flecs {

using Timer = EcsTimer;
using RateFilter = EcsRateFilter;

struct timer;

namespace _ {

void timer_init(flecs::world& world);

} // namespace _
} // namespace flecs
