/**
 * @file addons/cpp/mixins/system/decl.hpp
 * @brief System module declarations.
 */

#pragma once

namespace flecs {

/**
 * @defgroup cpp_addons_systems Systems
 * @ingroup cpp_addons
 * Systems are a query and a function that can be run manually or by a pipeline.
 *
 * @{
 */

/** Tick source component. */
using TickSource = EcsTickSource;

/** Forward declaration for system. */
struct system;

/** Forward declaration for system builder. */
template<typename ... Components>
struct system_builder;

namespace _ {

void system_init(flecs::world& world);

/** @} */

} // namespace _
} // namespace flecs
