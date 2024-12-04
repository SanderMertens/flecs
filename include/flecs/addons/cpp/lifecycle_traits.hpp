/**
 * @file addons/cpp/lifecycle_traits.hpp
 * @brief Utilities for discovering and registering component lifecycle hooks.
 */

#pragma once

namespace flecs 
{

namespace _ 
{

// T()
// Can't coexist with T(flecs::entity) or T(flecs::world, flecs::entity)
template <typename T>
void ctor_impl(void *ptr, int32_t count, const ecs_type_info_t *info) {
    (void)info; ecs_assert(info->size == ECS_SIZEOF(T),
        ECS_INTERNAL_ERROR, NULL);
    T *arr = static_cast<T*>(ptr);
    for (int i = 0; i < count; i ++) {
        FLECS_PLACEMENT_NEW(&arr[i], T);
    }
}

// ~T()
template <typename T>
void dtor_impl(void *ptr, int32_t count, const ecs_type_info_t *info) {
    (void)info; ecs_assert(info->size == ECS_SIZEOF(T), 
        ECS_INTERNAL_ERROR, NULL);
    T *arr = static_cast<T*>(ptr);
    for (int i = 0; i < count; i ++) {
        arr[i].~T();
    }
}

// T& operator=(const T&)
template <typename T>
void copy_impl(void *dst_ptr, const void *src_ptr, int32_t count, 
    const ecs_type_info_t *info)
{
    (void)info; ecs_assert(info->size == ECS_SIZEOF(T), 
        ECS_INTERNAL_ERROR, NULL);
    T *dst_arr = static_cast<T*>(dst_ptr);
    const T *src_arr = static_cast<const T*>(src_ptr);
    for (int i = 0; i < count; i ++) {
        dst_arr[i] = src_arr[i];
    }
}

// T& operator=(T&&)
template <typename T>
void move_impl(void *dst_ptr, void *src_ptr, int32_t count, 
    const ecs_type_info_t *info)
{
    (void)info; ecs_assert(info->size == ECS_SIZEOF(T), 
        ECS_INTERNAL_ERROR, NULL);
    T *dst_arr = static_cast<T*>(dst_ptr);
    T *src_arr = static_cast<T*>(src_ptr);
    for (int i = 0; i < count; i ++) {
        dst_arr[i] = FLECS_MOV(src_arr[i]);
    }
}

// T(T&)
template <typename T>
void copy_ctor_impl(void *dst_ptr, const void *src_ptr, int32_t count, 
    const ecs_type_info_t *info)
{
    (void)info; ecs_assert(info->size == ECS_SIZEOF(T), 
        ECS_INTERNAL_ERROR, NULL);
    T *dst_arr = static_cast<T*>(dst_ptr);
    const T *src_arr = static_cast<const T*>(src_ptr);
    for (int i = 0; i < count; i ++) {
        FLECS_PLACEMENT_NEW(&dst_arr[i], T(src_arr[i]));
    }
}

// T(T&&)
template <typename T>
void move_ctor_impl(void *dst_ptr, void *src_ptr, int32_t count, 
    const ecs_type_info_t *info)
{
    (void)info; ecs_assert(info->size == ECS_SIZEOF(T), 
        ECS_INTERNAL_ERROR, NULL);
    T *dst_arr = static_cast<T*>(dst_ptr);
    T *src_arr = static_cast<T*>(src_ptr);
    for (int i = 0; i < count; i ++) {
        FLECS_PLACEMENT_NEW(&dst_arr[i], T(FLECS_MOV(src_arr[i])));
    }
}

// T(T&&), ~T()
// Typically used when moving to a new table, and removing from the old table
template <typename T>
void ctor_move_dtor_impl(void *dst_ptr, void *src_ptr, int32_t count, 
    const ecs_type_info_t *info)
{
    (void)info; ecs_assert(info->size == ECS_SIZEOF(T), 
        ECS_INTERNAL_ERROR, NULL);
    T *dst_arr = static_cast<T*>(dst_ptr);
    T *src_arr = static_cast<T*>(src_ptr);
    for (int i = 0; i < count; i ++) {
        FLECS_PLACEMENT_NEW(&dst_arr[i], T(FLECS_MOV(src_arr[i])));
        src_arr[i].~T();
    }
}

// Move assign + dtor (non-trivial move assignment)
// Typically used when moving a component to a deleted component
template <typename T, if_not_t<
    std::is_trivially_move_assignable<T>::value > = 0>
void move_dtor_impl(void *dst_ptr, void *src_ptr, int32_t count, 
    const ecs_type_info_t *info)
{
    (void)info; ecs_assert(info->size == ECS_SIZEOF(T), 
        ECS_INTERNAL_ERROR, NULL);
    T *dst_arr = static_cast<T*>(dst_ptr);
    T *src_arr = static_cast<T*>(src_ptr);
    for (int i = 0; i < count; i ++) {
        // Move assignment should free dst & assign dst to src
        dst_arr[i] = FLECS_MOV(src_arr[i]);
        // Destruct src. Move should have left object in a state where it no
        // longer holds resources, but it still needs to be destructed.
        src_arr[i].~T();
    }
}

// Move assign + dtor (trivial move assignment)
// Typically used when moving a component to a deleted component
template <typename T, if_t<
    std::is_trivially_move_assignable<T>::value > = 0>
void move_dtor_impl(void *dst_ptr, void *src_ptr, int32_t count, 
    const ecs_type_info_t *info)
{
    (void)info; ecs_assert(info->size == ECS_SIZEOF(T), 
        ECS_INTERNAL_ERROR, NULL);
    T *dst_arr = static_cast<T*>(dst_ptr);
    T *src_arr = static_cast<T*>(src_ptr);
    for (int i = 0; i < count; i ++) {
        // Cleanup resources of dst
        dst_arr[i].~T();
        // Copy src to dst
        dst_arr[i] = FLECS_MOV(src_arr[i]);
        // No need to destruct src. Since this is a trivial move the code
        // should be agnostic to the address of the component which means we
        // can pretend nothing got destructed.
    }
}

} // _

// Trait to test if type is constructible by flecs
template <typename T>
struct is_flecs_constructible {
    static constexpr bool value = 
        std::is_default_constructible<actual_type_t<T>>::value;
};

namespace _
{

// Trivially constructible
template <typename T, if_t< std::is_trivially_constructible<T>::value > = 0>
ecs_xtor_t ctor(ecs_flags32_t &) {
    return nullptr;
}

// Not constructible by flecs
template <typename T, if_t< 
    ! std::is_default_constructible<T>::value > = 0>
ecs_xtor_t ctor(ecs_flags32_t &flags) {
    flags |= ECS_TYPE_HOOK_CTOR_ILLEGAL;
    return nullptr;
}

// Default constructible
template <typename T, if_t<
    ! std::is_trivially_constructible<T>::value &&
    std::is_default_constructible<T>::value > = 0>
ecs_xtor_t ctor(ecs_flags32_t &) {
    return ctor_impl<T>;
}

// No dtor
template <typename T, if_t< std::is_trivially_destructible<T>::value > = 0>
ecs_xtor_t dtor(ecs_flags32_t &) {
    return nullptr;
}

// Dtor
template <typename T, if_t<
    std::is_destructible<T>::value &&
    ! std::is_trivially_destructible<T>::value > = 0>
ecs_xtor_t dtor(ecs_flags32_t &) {
    return dtor_impl<T>;
}

// Assert when the type cannot be destructed
template <typename T, if_not_t< std::is_destructible<T>::value > = 0>
ecs_xtor_t dtor(ecs_flags32_t &flags) {
    flecs_static_assert(always_false<T>::value, 
        "component type must be destructible");
    flags |= ECS_TYPE_HOOK_DTOR_ILLEGAL;
    return nullptr;
}

// Trivially copyable
template <typename T, if_t< std::is_trivially_copyable<T>::value > = 0>
ecs_copy_t copy(ecs_flags32_t &) {
    return nullptr;
}

// Not copyable
template <typename T, if_t<
    ! std::is_trivially_copyable<T>::value &&
    ! std::is_copy_assignable<T>::value > = 0>
ecs_copy_t copy(ecs_flags32_t &flags) {
    flags |= ECS_TYPE_HOOK_COPY_ILLEGAL;
    return nullptr;
}

// Copy assignment
template <typename T, if_t<
    std::is_copy_assignable<T>::value &&
    ! std::is_trivially_copyable<T>::value > = 0>
ecs_copy_t copy(ecs_flags32_t &) {
    return copy_impl<T>;
}

// Trivially move assignable
template <typename T, if_t< std::is_trivially_move_assignable<T>::value > = 0>
ecs_move_t move(ecs_flags32_t &) {
    return nullptr;
}

// Component types must be move assignable
template <typename T, if_not_t< std::is_move_assignable<T>::value > = 0>
ecs_move_t move(ecs_flags32_t &flags) {
    flags |= ECS_TYPE_HOOK_MOVE_ILLEGAL;
    return nullptr;
}

// Move assignment
template <typename T, if_t<
    std::is_move_assignable<T>::value &&
    ! std::is_trivially_move_assignable<T>::value > = 0>
ecs_move_t move(ecs_flags32_t &) {
    return move_impl<T>;
}

// Trivially copy constructible
template <typename T, if_t<
    std::is_trivially_copy_constructible<T>::value > = 0>
ecs_copy_t copy_ctor(ecs_flags32_t &) {
    return nullptr;
}

// No copy ctor
template <typename T, if_t< ! std::is_copy_constructible<T>::value > = 0>
ecs_copy_t copy_ctor(ecs_flags32_t &flags) {
       flags |= ECS_TYPE_HOOK_COPY_CTOR_ILLEGAL;
    return nullptr;

}

// Copy ctor
template <typename T, if_t<
    std::is_copy_constructible<T>::value &&
    ! std::is_trivially_copy_constructible<T>::value > = 0>
ecs_copy_t copy_ctor(ecs_flags32_t &) {
    return copy_ctor_impl<T>;
}

// Trivially move constructible
template <typename T, if_t<
    std::is_trivially_move_constructible<T>::value > = 0>
ecs_move_t move_ctor(ecs_flags32_t &) {
    return nullptr;
}

// Component types must be move constructible
template <typename T, if_not_t< std::is_move_constructible<T>::value > = 0>
ecs_move_t move_ctor(ecs_flags32_t &flags) {
    flags |= ECS_TYPE_HOOK_MOVE_CTOR_ILLEGAL;
    return nullptr;
}

// Move ctor
template <typename T, if_t<
    std::is_move_constructible<T>::value &&
    ! std::is_trivially_move_constructible<T>::value > = 0>
ecs_move_t move_ctor(ecs_flags32_t &) {
    return move_ctor_impl<T>;
}

// Trivial merge (move assign + dtor)
template <typename T, if_t<
    std::is_trivially_move_constructible<T>::value  &&
    std::is_trivially_destructible<T>::value > = 0>
ecs_move_t ctor_move_dtor(ecs_flags32_t &) {
    return nullptr;
}

// Component types must be move constructible and destructible
template <typename T, if_t<
    ! std::is_move_constructible<T>::value ||
    ! std::is_destructible<T>::value > = 0>
ecs_move_t ctor_move_dtor(ecs_flags32_t &flags) {
    flags |= ECS_TYPE_HOOK_CTOR_MOVE_DTOR_ILLEGAL;
    return nullptr;
}

// Merge ctor + dtor
template <typename T, if_t<
    !(std::is_trivially_move_constructible<T>::value &&
      std::is_trivially_destructible<T>::value) &&
    std::is_move_constructible<T>::value &&
    std::is_destructible<T>::value > = 0>
ecs_move_t ctor_move_dtor(ecs_flags32_t &) {
    return ctor_move_dtor_impl<T>;
}

// Trivial merge (move assign + dtor)
template <typename T, if_t<
    std::is_trivially_move_assignable<T>::value  &&
    std::is_trivially_destructible<T>::value > = 0>
ecs_move_t move_dtor(ecs_flags32_t &) {
    return nullptr;
}

// Component types must be move constructible and destructible
template <typename T, if_t<
    ! std::is_move_assignable<T>::value ||
    ! std::is_destructible<T>::value > = 0>
ecs_move_t move_dtor(ecs_flags32_t &flags) {
    flags |= ECS_TYPE_HOOK_MOVE_DTOR_ILLEGAL;
    return nullptr;
}

// Merge assign + dtor
template <typename T, if_t<
    !(std::is_trivially_move_assignable<T>::value &&
      std::is_trivially_destructible<T>::value) &&
    std::is_move_assignable<T>::value &&
    std::is_destructible<T>::value > = 0>
ecs_move_t move_dtor(ecs_flags32_t &) {
    return move_dtor_impl<T>;
}

// Traits to check for operator<, operator>, and operator==
template<typename...>
using void_t = void;

// These traits causes a "float comparison warning" in some compilers
// when `T` is float or double.
// Disable this warning with the following pragmas:
#if defined(__clang__)
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wfloat-equal"
#elif defined(__GNUC__) && !defined(__clang__)
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wfloat-equal"
#endif

// Trait to check for operator<
template <typename T, typename = void>
struct has_operator_less : std::false_type {};

// Only enable if T has an operator< that takes T as the right-hand side (no implicit conversion)
template <typename T>
struct has_operator_less<T, void_t<decltype(std::declval<const T&>() < std::declval<const T&>())>> : 
    std::is_same<decltype(std::declval<const T&>() < std::declval<const T&>()), bool> {};

// Trait to check for operator>
template <typename T, typename = void>
struct has_operator_greater : std::false_type {};

// Only enable if T has an operator> that takes T as the right-hand side (no implicit conversion)
template <typename T>
struct has_operator_greater<T, void_t<decltype(std::declval<const T&>() > std::declval<const T&>())>> : 
    std::is_same<decltype(std::declval<const T&>() > std::declval<const T&>()), bool> {};

// Trait to check for operator==
template <typename T, typename = void>
struct has_operator_equal : std::false_type {};

// Only enable if T has an operator== that takes T as the right-hand side (no implicit conversion)
template <typename T>
struct has_operator_equal<T, void_t<decltype(std::declval<const T&>() == std::declval<const T&>())>> : 
    std::is_same<decltype(std::declval<const T&>() == std::declval<const T&>()), bool> {};

// 1. Compare function if `<`, `>`, are defined
template <typename T, if_t<
    has_operator_less<T>::value &&
    has_operator_greater<T>::value &&
    !has_operator_equal<T>::value > = 0>
int compare_impl(const void *a, const void *b, const ecs_type_info_t *) {
    const T& lhs = *static_cast<const T*>(a);
    const T& rhs = *static_cast<const T*>(b);
    if (lhs < rhs) return -1;
    if (lhs > rhs) return 1;
    return 0;
}

// 2. Compare function if `<` and `==` are defined, ignoring `>`
// if defined.
template <typename T, if_t<
    has_operator_less<T>::value &&
    has_operator_equal<T>::value > = 0>
int compare_impl(const void *a, const void *b, const ecs_type_info_t *) {
    const T& lhs = *static_cast<const T*>(a);
    const T& rhs = *static_cast<const T*>(b);
    if (lhs == rhs) return 0;
    if (lhs < rhs) return -1;
    return 1; // If not less and not equal, must be greater
}

// 3. Compare function if `>` and `==` are defined, deducing `<`
template <typename T, if_t<    
    has_operator_greater<T>::value &&
    has_operator_equal<T>::value &&
    !has_operator_less<T>::value > = 0>
int compare_impl(const void *a, const void *b, const ecs_type_info_t *) {
    const T& lhs = *static_cast<const T*>(a);
    const T& rhs = *static_cast<const T*>(b);
    if (lhs == rhs) return 0;
    if (lhs > rhs) return 1;
    return -1; // If not greater and not equal, must be less
}

// 4. Compare function if only `<` is defined, deducing the rest
template <typename T, if_t<
    has_operator_less<T>::value &&
    !has_operator_greater<T>::value &&
    !has_operator_equal<T>::value > = 0>
int compare_impl(const void *a, const void *b, const ecs_type_info_t *) {
    const T& lhs = *static_cast<const T*>(a);
    const T& rhs = *static_cast<const T*>(b);
    if (lhs < rhs) return -1;
    if (rhs < lhs) return 1;
    return 0; // If neither is less, they must be equal
}

// 5. Compare function if only `>` is defined, deducing the rest
template <typename T, if_t<
    has_operator_greater<T>::value &&
    !has_operator_less<T>::value &&
    !has_operator_equal<T>::value > = 0>
int compare_impl(const void *a, const void *b, const ecs_type_info_t *) {
    const T& lhs = *static_cast<const T*>(a);
    const T& rhs = *static_cast<const T*>(b);
    if (lhs > rhs) return 1;
    if (rhs > lhs) return -1;
    return 0; // If neither is greater, they must be equal
}

// In order to have a generated compare hook, at least
// operator> or operator< must be defined:
template <typename T, if_t<
    has_operator_less<T>::value ||
    has_operator_greater<T>::value > = 0>
ecs_cmp_t compare() {
    return compare_impl<T>;
}

template <typename T, if_t<
    !has_operator_less<T>::value &&
    !has_operator_greater<T>::value > = 0>
ecs_cmp_t compare() {
    return NULL;
}

// Equals function enabled only if `==` is defined
template <typename T, if_t<
    has_operator_equal<T>::value > = 0>
bool equals_impl(const void *a, const void *b, const ecs_type_info_t *) {
    const T& lhs = *static_cast<const T*>(a);
    const T& rhs = *static_cast<const T*>(b);
    return lhs == rhs;
}

template <typename T, if_t<
    has_operator_equal<T>::value > = 0>
ecs_equals_t equals() {
    return equals_impl<T>;
}

template <typename T, if_t<
    !has_operator_equal<T>::value > = 0>
ecs_equals_t equals() {
    return NULL;
}

// re-enable the float comparison warning:
#if defined(__clang__)
    #pragma clang diagnostic pop
#elif defined(__GNUC__) && !defined(__clang__)
    #pragma GCC diagnostic pop
#endif

} // _
} // flecs
