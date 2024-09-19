/**
 * @file addons/cpp/mixins/observer/decl.hpp
 * @brief Observer declarations.
 */

#pragma once

namespace flecs {

/**
 * @defgroup cpp_observers Observers
 * @ingroup cpp_core
 * Observers let applications register callbacks for ECS events.
 *
 * @{
 */

FLECS_API_STRUCT struct observer;

FLECS_API_STRUCT
template<typename ... Components>
struct observer_builder;

/** @} */

}
