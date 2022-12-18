/**
 * @file addons/cpp/utils/function_traits.hpp
 * @brief Compile time utilities to inspect properties of functions.
 *
 * Code from: https://stackoverflow.com/questions/27024238/c-template-mechanism-to-get-the-number-of-function-arguments-which-would-work
 */

namespace flecs {
namespace _ {

template <typename ... Args>
struct arg_list { };

// Base type that contains the traits
template <typename ReturnType, typename... Args>
struct function_traits_defs
{
    static constexpr bool is_callable = true;
    static constexpr size_t arity = sizeof...(Args);
    using return_type = ReturnType;
    using args = arg_list<Args ...>;
};

// Primary template for function_traits_impl
template <typename T>
struct function_traits_impl {
    static constexpr bool is_callable = false;
};

// Template specializations for the different kinds of function types (whew)
template <typename ReturnType, typename... Args>
struct function_traits_impl<ReturnType(Args...)>
    : function_traits_defs<ReturnType, Args...> {};

template <typename ReturnType, typename... Args>
struct function_traits_impl<ReturnType(*)(Args...)>
    : function_traits_defs<ReturnType, Args...> {};

template <typename ClassType, typename ReturnType, typename... Args>
struct function_traits_impl<ReturnType(ClassType::*)(Args...)>
    : function_traits_defs<ReturnType, Args...> {};

template <typename ClassType, typename ReturnType, typename... Args>
struct function_traits_impl<ReturnType(ClassType::*)(Args...) const>
    : function_traits_defs<ReturnType, Args...> {};    

template <typename ClassType, typename ReturnType, typename... Args>
struct function_traits_impl<ReturnType(ClassType::*)(Args...) const&>
    : function_traits_defs<ReturnType, Args...> {};
    
template <typename ClassType, typename ReturnType, typename... Args>
struct function_traits_impl<ReturnType(ClassType::*)(Args...) const&&>
    : function_traits_defs<ReturnType, Args...> {};
    
template <typename ClassType, typename ReturnType, typename... Args>
struct function_traits_impl<ReturnType(ClassType::*)(Args...) volatile>
    : function_traits_defs<ReturnType, Args...> {};

template <typename ClassType, typename ReturnType, typename... Args>
struct function_traits_impl<ReturnType(ClassType::*)(Args...) volatile&>
    : function_traits_defs<ReturnType, Args...> {};
    
template <typename ClassType, typename ReturnType, typename... Args>
struct function_traits_impl<ReturnType(ClassType::*)(Args...) volatile&&>
    : function_traits_defs<ReturnType, Args...> {};

template <typename ClassType, typename ReturnType, typename... Args>
struct function_traits_impl<ReturnType(ClassType::*)(Args...) const volatile>
    : function_traits_defs<ReturnType, Args...> {};

template <typename ClassType, typename ReturnType, typename... Args>
struct function_traits_impl<ReturnType(ClassType::*)(Args...) const volatile&>
    : function_traits_defs<ReturnType, Args...> {};
    
template <typename ClassType, typename ReturnType, typename... Args>
struct function_traits_impl<ReturnType(ClassType::*)(Args...) const volatile&&>
    : function_traits_defs<ReturnType, Args...> {};

// Primary template for function_traits_no_cv. If T is not a function, the
// compiler will attempt to instantiate this template and fail, because its base
// is undefined.
template <typename T, typename V = void>
struct function_traits_no_cv
    : function_traits_impl<T> {};

// Specialized template for function types
template <typename T>
struct function_traits_no_cv<T, decltype((void)&T::operator())>
    : function_traits_impl<decltype(&T::operator())> {};
 
// Front facing template that decays T before ripping it apart.
template <typename T>
struct function_traits
    : function_traits_no_cv< decay_t<T> > {};

} // _


template <typename T>
struct is_callable {
    static constexpr bool value = _::function_traits<T>::is_callable;
};

template <typename T>
struct arity {
    static constexpr int value = _::function_traits<T>::arity;
};

template <typename T>
using return_type_t = typename _::function_traits<T>::return_type;

template <typename T>
using arg_list_t = typename _::function_traits<T>::args;


template<typename Func, typename ... Args>
struct first_arg_impl;

template<typename Func, typename T, typename ... Args>
struct first_arg_impl<Func, _::arg_list<T, Args ...> > {
    using type = T;
};

template<typename Func>
struct first_arg {
    using type = typename first_arg_impl<Func, arg_list_t<Func>>::type;
};

template <typename Func>
using first_arg_t = typename first_arg<Func>::type;

} // flecs
