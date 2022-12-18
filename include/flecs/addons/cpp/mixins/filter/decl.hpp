/**
 * @file addons/cpp/mixins/filter/decl.hpp
 * @brief Filter declarations.
 */

#pragma once

namespace flecs {

struct filter_base;

template<typename ... Components>
struct filter;

template<typename ... Components>
struct filter_builder;

}
