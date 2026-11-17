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

constexpr size_t aggregate_initializers_max = 32;

template <typename T, typename... Visited>
constexpr bool is_deep_copy_constructible();

struct aggregate_field_any {
    constexpr explicit aggregate_field_any(size_t) { }

    template <typename U>
    operator U() const;
};

template <typename T, typename... Visited>
struct deep_copy_constructible_trait
    : std::bool_constant<is_deep_copy_constructible<T, Visited...>()> { };

template <typename... Visited>
struct aggregate_field_copyable {
    constexpr explicit aggregate_field_copyable(size_t) { }

    template <typename U, enable_if_t<std::conjunction_v<
        std::negation<is_same<U, aggregate_field_copyable>>,
        deep_copy_constructible_trait<U, Visited...>>, int> = 0>
    operator U() const;
};

template <typename T, typename Probe, typename Indices, typename = void>
struct is_aggregate_initializable : std::false_type { };

template <typename T, typename Probe, size_t... I>
struct is_aggregate_initializable<T, Probe, std::index_sequence<I...>,
    std::void_t<decltype(T{ Probe{I}... })>> : std::true_type { };

template <typename T, size_t N = 0, bool Found = false>
constexpr size_t aggregate_initializer_count() {
    constexpr bool valid = is_aggregate_initializable<T,
        aggregate_field_any, std::make_index_sequence<N>>::value;
    if constexpr (Found && !valid) {
        return N - 1;
    } else if constexpr (N == aggregate_initializers_max) {
        return valid ? N : 0;
    } else {
        return aggregate_initializer_count<T, N + 1, Found || valid>();
    }
}

template <typename T, typename... Visited>
constexpr bool is_aggregate_deep_copy_constructible() {
    constexpr size_t count = aggregate_initializer_count<T>();
    if constexpr (count == 0) {
        return true;
    } else {
        return is_aggregate_initializable<T,
            aggregate_field_copyable<T, Visited...>,
            std::make_index_sequence<count>>::value;
    }
}

template <typename T, typename = void>
struct is_allocator_aware_container : std::false_type { };

template <typename T>
struct is_allocator_aware_container<T, std::void_t<
    typename T::value_type, typename T::allocator_type>> : std::true_type { };

template <typename T>
struct is_std_pair : std::false_type { };

template <typename First, typename Second>
struct is_std_pair<std::pair<First, Second>> : std::true_type { };

template <typename T>
struct is_initializer_list : std::false_type { };

template <typename Element>
struct is_initializer_list<std::initializer_list<Element>> : std::true_type { };

template <typename T, typename... Visited>
constexpr bool is_deep_copy_constructible() {
    using U = std::remove_cv_t<T>;
    if constexpr ((is_same_v<U, Visited> || ...)) {
        return true;
    } else if constexpr (!is_copy_constructible_v<U>) {
        return false;
    } else if constexpr (is_std_pair<U>::value) {
        return is_deep_copy_constructible<typename U::first_type, U, Visited...>() &&
            is_deep_copy_constructible<typename U::second_type, U, Visited...>();
    } else if constexpr (is_initializer_list<U>::value) {
        return is_deep_copy_constructible<typename U::value_type, U, Visited...>();
    } else if constexpr (is_allocator_aware_container<U>::value) {
        return is_deep_copy_constructible<typename U::value_type, U, Visited...>();
    } else if constexpr (std::is_aggregate_v<U> && !std::is_union_v<U>) {
        return is_aggregate_deep_copy_constructible<U, Visited...>();
    } else {
        return true;
    }
}

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

template <typename T, bool Move, bool Construct, bool Destroy, bool Legal>
struct transfer_hook {
    static conditional_t<Move, ecs_move_t, ecs_copy_t> callback() {
        return nullptr;
    }
};

template <typename T, bool Move, bool Construct, bool Destroy>
struct transfer_hook<T, Move, Construct, Destroy, true> {
    static void invoke(void *dst_ptr, conditional_t<Move, void*, const void*> src_ptr,
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
    }

    static conditional_t<Move, ecs_move_t, ecs_copy_t> callback() {
        return invoke;
    }
};

template <typename T, bool Move, bool Construct, bool Destroy = false>
conditional_t<Move, ecs_move_t, ecs_copy_t> transfer(ecs_flags32_t& flags) {
    constexpr bool trivial = (Construct
        ? (Move ? is_trivially_move_constructible_v<T> : is_trivially_copy_constructible_v<T>)
        : (Move ? is_trivially_move_assignable_v<T> : is_trivially_copyable_v<T>)) &&
        (!Destroy || is_trivially_destructible_v<T>);
    constexpr bool legal = (Construct
        ? (Move ? is_move_constructible_v<T> : is_copy_constructible_v<T>)
        : (Move ? is_move_assignable_v<T> : is_copy_assignable_v<T>)) &&
        (!Destroy || is_destructible_v<T>) &&
        (Move || !is_copy_constructible_v<T> || is_deep_copy_constructible<T>());
    constexpr auto illegal = Move
        ? (Construct
            ? (Destroy ? ECS_TYPE_HOOK_CTOR_MOVE_DTOR_ILLEGAL : ECS_TYPE_HOOK_MOVE_CTOR_ILLEGAL)
            : (Destroy ? ECS_TYPE_HOOK_MOVE_DTOR_ILLEGAL : ECS_TYPE_HOOK_MOVE_ILLEGAL))
        : (Construct ? ECS_TYPE_HOOK_COPY_CTOR_ILLEGAL : ECS_TYPE_HOOK_COPY_ILLEGAL);
    if constexpr (trivial) {
        return nullptr;
    } else {
        if constexpr (!legal) {
            flags |= illegal;
        }
        return transfer_hook<T, Move, Construct, Destroy, legal>::callback();
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
