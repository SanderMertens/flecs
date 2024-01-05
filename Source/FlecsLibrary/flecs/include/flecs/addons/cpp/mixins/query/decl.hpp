/**
 * @file addons/cpp/mixins/query/decl.hpp
 * @brief Query declarations.
 */

#pragma once

namespace flecs {

/**
 * @defgroup cpp_core_queries Queries
 * @ingroup cpp_core
 * Cached query implementation. Fast to iterate, but slower to create than flecs::filter.
 *
 * @{
 */

struct query_base;

template<typename ... Components>
struct query;

template<typename ... Components>
struct query_builder;

/** @} */

}
