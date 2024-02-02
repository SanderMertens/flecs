/**
 * @file addons/cpp/mixins/rule/decl.hpp
 * @brief Rule declarations.
 */

#pragma once

namespace flecs {

/**
 * @defgroup cpp_addons_rules Rules
 * @ingroup cpp_addons
 * Rules are an advanced query engine for matching against entity graphs.
 *
 * @{
 */

struct rule_base;

template<typename ... Components>
struct rule;

template<typename ... Components>
struct rule_builder;

/** @} */

}
