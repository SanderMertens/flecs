/**
 * @file addons/cpp/mixins/observer/decl.hpp
 * @brief Observer declarations.
 */

#pragma once

namespace flecs {

/**
 * @defgroup cpp_observers Observers
 * @brief Observers let applications register callbacks for ECS events.
 * 
 * \ingroup cpp_core
 * @{
 */

struct observer;

template<typename ... Components>
struct observer_builder;

/** @} */

}
