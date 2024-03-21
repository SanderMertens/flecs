/**
 * @file addons/cpp/mixins/timer/decl.hpp
 * @brief Timer module declarations.
 */

#pragma once

namespace flecs {

/**
 * @defgroup cpp_addons_timer Timer
 * @ingroup cpp_addons
 * Run systems at a time interval.
 *
 * @{
 */

using Timer = EcsTimer;
using RateFilter = EcsRateFilter;

struct timer;

/** @} */

namespace _ {

void timer_init(flecs::world& world);

} // namespace _
} // namespace flecs
