/**
 * @file addons/cpp/utils/function_traits.hpp
 * @brief Compile time utilities to inspect properties of functions.
 *
 * Code from: https://stackoverflow.com/questions/27024238/c-template-mechanism-to-get-the-number-of-function-arguments-which-would-work
 */

#pragma once

namespace flecs {
namespace _ {

/** @private Argument list type holder. */
template <typename ... Args>
struct arg_list { };

/** @private Base type that contains the function traits definitions. */
template <typename ReturnType, typename... Args>
struct function_traits_defs
{
    static constexpr bool is_callable = true;
    static constexpr size_t arity = sizeof...(Args);
    using return_type = ReturnType;
    using args = arg_list<Args ...>;
};

/** @private Primary template for function_traits_impl. */
template <typename T>
struct function_traits_impl {
    static constexpr bool is_callable = false;
};

/** @private Specialization for free functions. */
template <typename ReturnType, typename... Args>
struct function_traits_impl<ReturnType(Args...)>
    : function_traits_defs<ReturnType, Args...> {};

/** @private Specialization for function pointers. */
template <typename ReturnType, typename... Args>
struct function_traits_impl<ReturnType(*)(Args...)>
    : function_traits_defs<ReturnType, Args...> {};

/** @private Specialization for member function pointers. */
template <typename ClassType, typename ReturnType, typename... Args>
struct function_traits_impl<ReturnType(ClassType::*)(Args...)>
    : function_traits_defs<ReturnType, Args...> {};

/** @private Specialization for const member function pointers. */
template <typename ClassType, typename ReturnType, typename... Args>
struct function_traits_impl<ReturnType(ClassType::*)(Args...) const>
    : function_traits_defs<ReturnType, Args...> {};

/** @private Specialization for const& member function pointers. */
template <typename ClassType, typename ReturnType, typename... Args>
struct function_traits_impl<ReturnType(ClassType::*)(Args...) const&>
    : function_traits_defs<ReturnType, Args...> {};

/** @private Specialization for const&& member function pointers. */
template <typename ClassType, typename ReturnType, typename... Args>
struct function_traits_impl<ReturnType(ClassType::*)(Args...) const&&>
    : function_traits_defs<ReturnType, Args...> {};

/** @private Specialization for volatile member function pointers. */
template <typename ClassType, typename ReturnType, typename... Args>
struct function_traits_impl<ReturnType(ClassType::*)(Args...) volatile>
    : function_traits_defs<ReturnType, Args...> {};

/** @private Specialization for volatile& member function pointers. */
template <typename ClassType, typename ReturnType, typename... Args>
struct function_traits_impl<ReturnType(ClassType::*)(Args...) volatile&>
    : function_traits_defs<ReturnType, Args...> {};

/** @private Specialization for volatile&& member function pointers. */
template <typename ClassType, typename ReturnType, typename... Args>
struct function_traits_impl<ReturnType(ClassType::*)(Args...) volatile&&>
    : function_traits_defs<ReturnType, Args...> {};

/** @private Specialization for const volatile member function pointers. */
template <typename ClassType, typename ReturnType, typename... Args>
struct function_traits_impl<ReturnType(ClassType::*)(Args...) const volatile>
    : function_traits_defs<ReturnType, Args...> {};

/** @private Specialization for const volatile& member function pointers. */
template <typename ClassType, typename ReturnType, typename... Args>
struct function_traits_impl<ReturnType(ClassType::*)(Args...) const volatile&>
    : function_traits_defs<ReturnType, Args...> {};

/** @private Specialization for const volatile&& member function pointers. */
template <typename ClassType, typename ReturnType, typename... Args>
struct function_traits_impl<ReturnType(ClassType::*)(Args...) const volatile&&>
    : function_traits_defs<ReturnType, Args...> {};

/** @private Primary template for function_traits_no_cv. */
template <typename T, typename V = void>
struct function_traits_no_cv
    : function_traits_impl<T> {};

/** @private Specialization for callable objects with operator(). */
template <typename T>
struct function_traits_no_cv<T, decltype((void)&T::operator())>
    : function_traits_impl<decltype(&T::operator())> {};

/** @private Front facing template that decays T before extracting traits. */
template <typename T>
struct function_traits
    : function_traits_no_cv< decay_t<T> > {};

} // namespace _


/** Trait to check if a type is callable. */
template <typename T>
struct is_callable {
    static constexpr bool value = _::function_traits<T>::is_callable;
};

/** Trait to get the number of arguments of a callable. */
template <typename T>
struct arity {
    static constexpr int value = _::function_traits<T>::arity;
};

/** Get the return type of a callable. */
template <typename T>
using return_type_t = typename _::function_traits<T>::return_type;

/** Get the argument list type of a callable. */
template <typename T>
using arg_list_t = typename _::function_traits<T>::args;

/** Extract the first argument type from a callable (implementation). */
template<typename Func, typename ... Args>
struct first_arg_impl;

/** Extract the first argument type from an arg_list. */
template<typename Func, typename T, typename ... Args>
struct first_arg_impl<Func, _::arg_list<T, Args ...> > {
    using type = T;
};

/** Get the first argument type of a callable. */
template<typename Func>
struct first_arg {
    using type = typename first_arg_impl<Func, arg_list_t<Func>>::type;
};

/** Convenience alias for the first argument type of a callable. */
template <typename Func>
using first_arg_t = typename first_arg<Func>::type;

/** Extract the second argument type from a callable (implementation). */
template<typename Func, typename ... Args>
struct second_arg_impl;

/** Extract the second argument type from an arg_list. */
template<typename Func, typename First, typename T, typename ... Args>
struct second_arg_impl<Func, _::arg_list<First, T, Args ...> > {
    using type = T;
};

/** Get the second argument type of a callable. */
template<typename Func>
struct second_arg {
    using type = typename second_arg_impl<Func, arg_list_t<Func>>::type;
};

/** Convenience alias for the second argument type of a callable. */
template <typename Func>
using second_arg_t = typename second_arg<Func>::type;

} // namespace flecs
