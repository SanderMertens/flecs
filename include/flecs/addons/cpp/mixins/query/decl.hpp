/**
 * @file addons/cpp/mixins/query/decl.hpp
 * @brief Query declarations.
 */

#pragma once

FLECS_API_NAMESPACE namespace flecs {

/**
 * @defgroup cpp_core_queries Queries
 * @ingroup cpp_core
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
