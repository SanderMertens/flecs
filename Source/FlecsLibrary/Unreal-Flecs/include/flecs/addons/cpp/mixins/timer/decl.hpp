/**
 * @file addons/cpp/mixins/timer/decl.hpp
 * @brief Timer module declarations.
 */

#pragma once

namespace flecs {

/**
 * @defgroup cpp_addons_timer Timer
 * @brief Run systems at a time interval.
 * 
 * \ingroup cpp_addons
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
