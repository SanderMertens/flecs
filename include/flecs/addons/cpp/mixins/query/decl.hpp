/**
 * @file addons/cpp/mixins/query/decl.hpp
 * @brief Query declarations.
 */

#pragma once

namespace flecs {

/**
 * @defgroup cpp_core_queries Queries
 * @ingroup cpp_core
 *
 * @{
 */

struct query_base;

FLECS_API_STRUCT
template<typename ... Components>
struct query;

FLECS_API_STRUCT
template<typename ... Components>
struct query_builder;

/** @} */

}
