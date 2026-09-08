/**
 * @file addons/cpp/lifecycle_traits.hpp
 * @brief Utilities for discovering and registering component lifecycle hooks.
 */

#pragma once

namespace flecs 
{

template <typename T>
struct is_flecs_constructible {
    static constexpr bool value = std::is_default_constructible<actual_type_t<T>>::value;
};

namespace _ {

template <typename T, bool Destroy>
ecs_xtor_t xtor(ecs_flags32_t& flags) {
    constexpr bool trivial = Destroy ? is_trivially_destructible_v<T> : is_trivially_constructible_v<T>;
    constexpr bool legal = Destroy ? is_destructible_v<T> : is_default_constructible_v<T>;
    if constexpr (trivial) {
        return nullptr;
    } else if constexpr (!legal) {
        flecs_static_assert(!Destroy || always_false<T>::value, "component type must be destructible");
        flags |= Destroy ? ECS_TYPE_HOOK_DTOR_ILLEGAL : ECS_TYPE_HOOK_CTOR_ILLEGAL;
        return nullptr;
    } else {
        return [](void *ptr, int32_t count, const ecs_type_info_t *info) {
            (void)info;
            ecs_assert(info->size == ECS_SIZEOF(T), ECS_INTERNAL_ERROR, nullptr);
            T *arr = static_cast<T*>(ptr);
            for (int32_t i = 0; i < count; i ++) {
                if constexpr (Destroy) {
                    arr[i].~T();
                } else {
                    FLECS_PLACEMENT_NEW(&arr[i], T);
                }
            }
        };
    }
}

template <typename T, bool Move, bool Construct, bool Destroy = false>
conditional_t<Move, ecs_move_t, ecs_copy_t> transfer(ecs_flags32_t& flags) {
    constexpr bool trivial = (Construct
        ? (Move ? is_trivially_move_constructible_v<T> : is_trivially_copy_constructible_v<T>)
        : (Move ? is_trivially_move_assignable_v<T> : is_trivially_copyable_v<T>)) &&
        (!Destroy || is_trivially_destructible_v<T>);
    constexpr bool legal = (Construct
        ? (Move ? is_move_constructible_v<T> : is_copy_constructible_v<T>)
        : (Move ? is_move_assignable_v<T> : is_copy_assignable_v<T>)) &&
        (!Destroy || is_destructible_v<T>);
    constexpr auto illegal = Move
        ? (Construct
            ? (Destroy ? ECS_TYPE_HOOK_CTOR_MOVE_DTOR_ILLEGAL : ECS_TYPE_HOOK_MOVE_CTOR_ILLEGAL)
            : (Destroy ? ECS_TYPE_HOOK_MOVE_DTOR_ILLEGAL : ECS_TYPE_HOOK_MOVE_ILLEGAL))
        : (Construct ? ECS_TYPE_HOOK_COPY_CTOR_ILLEGAL : ECS_TYPE_HOOK_COPY_ILLEGAL);
    if constexpr (trivial) {
        return nullptr;
    } else if constexpr (!legal) {
        flags |= illegal;
        return nullptr;
    } else {
        return [](void *dst_ptr, conditional_t<Move, void*, const void*> src_ptr,
            int32_t count, const ecs_type_info_t *info)
        {
            (void)info;
            ecs_assert(info->size == ECS_SIZEOF(T), ECS_INTERNAL_ERROR, nullptr);
            T *dst = static_cast<T*>(dst_ptr);
            auto src = static_cast<conditional_t<Move, T*, const T*>>(src_ptr);
            for (int32_t i = 0; i < count; i ++) {
                using Value = conditional_t<Move, T&&, const T&>;
                if constexpr (Destroy && !Construct && is_trivially_move_assignable_v<T>) {
                    dst[i].~T();
                }
                if constexpr (Construct) {
                    FLECS_PLACEMENT_NEW(&dst[i], T(static_cast<Value>(src[i])));
                } else {
                    dst[i] = static_cast<Value>(src[i]);
                }
                if constexpr (Destroy && (Construct || !is_trivially_move_assignable_v<T>)) {
                    src[i].~T();
                }
            }
        };
    }
}

// Traits to check for operator<, operator>, and operator==.
using std::void_t;

// These traits cause a "float comparison warning" in some compilers
// when `T` is float or double.
// Disable this warning with the following pragmas.
#if defined(__clang__)
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wfloat-equal"
#elif defined(__GNUC__) && !defined(__clang__)
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wfloat-equal"
#endif

// Trait to check for operator<.
template <typename T, typename = void>
struct has_operator_less : std::false_type {};

// Only enable if T has an operator< that takes T as the right-hand side (no implicit conversion).
template <typename T>
struct has_operator_less<T, void_t<decltype(std::declval<const T&>() < std::declval<const T&>())>> :
    std::is_same<decltype(std::declval<const T&>() < std::declval<const T&>()), bool> {};

// Trait to check for operator>.
template <typename T, typename = void>
struct has_operator_greater : std::false_type {};

// Only enable if T has an operator> that takes T as the right-hand side (no implicit conversion).
template <typename T>
struct has_operator_greater<T, void_t<decltype(std::declval<const T&>() > std::declval<const T&>())>> :
    std::is_same<decltype(std::declval<const T&>() > std::declval<const T&>()), bool> {};

// Trait to check for operator==.
template <typename T, typename = void>
struct has_operator_equal : std::false_type {};

// Only enable if T has an operator== that takes T as the right-hand side (no implicit conversion).
template <typename T>
struct has_operator_equal<T, void_t<decltype(std::declval<const T&>() == std::declval<const T&>())>> : 
    std::is_same<decltype(std::declval<const T&>() == std::declval<const T&>()), bool> {};

// Selects the best comparison strategy based on available operators.
template <typename T>
int compare_impl(const void *a, const void *b, const ecs_type_info_t *) {
    const T& lhs = *static_cast<const T*>(a);
    const T& rhs = *static_cast<const T*>(b);
    
    if constexpr (has_operator_less<T>::value && has_operator_equal<T>::value) {
        // 2. Compare function if `<` and `==` are defined (preferred)
        if (lhs == rhs) return 0;
        if (lhs < rhs) return -1;
        return 1;
    } else if constexpr (has_operator_greater<T>::value && has_operator_equal<T>::value) {
        // 3. Compare function if `>` and `==` are defined, deducing `<`
        if (lhs == rhs) return 0;
        if (lhs > rhs) return 1;
        return -1;
    } else if constexpr (has_operator_less<T>::value && has_operator_greater<T>::value) {
        // 1. Compare function if `<`, `>` are defined
        if (lhs < rhs) return -1;
        if (lhs > rhs) return 1;
        return 0;
    } else if constexpr (has_operator_less<T>::value) {
        // 4. Compare function if only `<` is defined
        if (lhs < rhs) return -1;
        if (rhs < lhs) return 1;
        return 0;
    } else if constexpr (has_operator_greater<T>::value) {
        // 5. Compare function if only `>` is defined
        if (lhs > rhs) return 1;
        if (rhs > lhs) return -1;
        return 0;
    } else {
        // This branch should never be instantiated due to the compare() check.
        return 0;
    }
}

// To have a generated compare hook, at least
// operator> or operator< must be defined.
template <typename T>
ecs_cmp_t compare() {
    if constexpr (has_operator_less<T>::value || has_operator_greater<T>::value) {
        return compare_impl<T>;
    } else {
        return nullptr;
    }
}

// Equals implementation.
template <typename T>
bool equals_impl(const void *a, const void *b, const ecs_type_info_t *) {
    const T& lhs = *static_cast<const T*>(a);
    const T& rhs = *static_cast<const T*>(b);
    return lhs == rhs;
}

template <typename T>
ecs_equals_t equals() {
    if constexpr (has_operator_equal<T>::value) {
        return equals_impl<T>;
    } else {
        return nullptr;
    }
}

// Re-enable the float comparison warning.
#if defined(__clang__)
    #pragma clang diagnostic pop
#elif defined(__GNUC__) && !defined(__clang__)
    #pragma GCC diagnostic pop
#endif

} // namespace _
} // namespace flecs
