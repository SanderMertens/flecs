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

FLECS_API_STRUCT using Timer = EcsTimer;
FLECS_API_STRUCT using RateFilter = EcsRateFilter;

FLECS_API_STRUCT struct timer;

/** @} */

namespace _ {

void timer_init(flecs::world& world);

} // namespace _
} // namespace flecs
