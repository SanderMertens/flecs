/**
 * @file addons/cpp/mixins/query/decl.hpp
 * @brief Query declarations.
 */

#pragma once

namespace flecs {

struct query_base;

template<typename ... Components>
struct query;

template<typename ... Components>
struct query_builder;

}
