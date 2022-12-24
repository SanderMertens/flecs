/**
 * @file addons/cpp/mixins/query/decl.hpp
 * @brief Query declarations.
 */

#pragma once

namespace flecs {

/**
 * @defgroup cpp_core_queries Queries
 * @brief Cached query implementation. Fast to iterate, but slower to create than flecs::filter.
 * 
 * \ingroup cpp_core
 * @{
 */

struct query_base;

template<typename ... Components>
struct query;

template<typename ... Components>
struct query_builder;

/** @} */

}
