/**
 * @file addons/cpp/mixins/filter/decl.hpp
 * @brief Query declarations.
 */

#pragma once

namespace flecs {

/**
<<<<<<< HEAD
 * @defgroup cpp_core_filters Filters
 * @ingroup cpp_core
 * Filters are cheaper to create, but slower to iterate than flecs::query.
 *
=======
 * @defgroup cpp_core_filters Querys
 * @brief Querys are cheaper to create, but slower to iterate than flecs::query.
 * 
 * \ingroup cpp_core
>>>>>>> 45a6e85b8 (v4)
 * @{
 */

struct filter_base;

template<typename ... Components>
struct filter;

template<typename ... Components>
struct filter_builder;

/** @} */

}
