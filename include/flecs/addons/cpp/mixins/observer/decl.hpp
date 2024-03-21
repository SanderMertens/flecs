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

struct observer;

template<typename ... Components>
struct observer_builder;

/** @} */

}
