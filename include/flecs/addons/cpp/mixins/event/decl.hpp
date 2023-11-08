/**
 * @file addons/cpp/mixins/event/decl.hpp
 * @brief Event declarations.
 */

#pragma once

#include "builder.hpp"

namespace flecs {
namespace _ {

// Utility to derive event type from function
template  <typename Func, typename U = int>
struct event_from_func;

// Specialization for observer callbacks with a single argument
template  <typename Func>
struct event_from_func<Func, if_t< arity<Func>::value == 1>> {
    using type = decay_t<first_arg_t<Func>>;
};

// Specialization for observer callbacks with an initial entity src argument
template  <typename Func>
struct event_from_func<Func, if_t< arity<Func>::value == 2>> {
    using type = decay_t<second_arg_t<Func>>;
};

template <typename Func>
using event_from_func_t = typename event_from_func<Func>::type;

}
}
