/**
 * @file addons/cpp/mixins/rule/decl.hpp
 * @brief Rule declarations.
 */

#pragma once

namespace flecs {

struct rule_base;

template<typename ... Components>
struct rule;

template<typename ... Components>
struct rule_builder;

}
