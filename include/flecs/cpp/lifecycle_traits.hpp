namespace flecs 
{

namespace _ 
{

// T()
// Can't coexist with T(flecs::entity) or T(flecs::world, flecs::entity)
template <typename T>
void ctor_impl(
    ecs_world_t*, ecs_entity_t, const ecs_entity_t*, void *ptr, size_t size,
    int32_t count, void*)
{
    (void)size; ecs_assert(size == sizeof(T), ECS_INTERNAL_ERROR, NULL);
    T *arr = static_cast<T*>(ptr);
    for (int i = 0; i < count; i ++) {
        FLECS_PLACEMENT_NEW(&arr[i], T);
    }
}

// T(flecs::world, flecs::entity)
template <typename T>
void ctor_world_entity_impl(
    ecs_world_t* world, ecs_entity_t, const ecs_entity_t* ids, void *ptr, 
    size_t size, int32_t count, void*);

// ~T()
template <typename T>
void dtor_impl(
    ecs_world_t*, ecs_entity_t, const ecs_entity_t*, void *ptr, size_t size,
    int32_t count, void*)
{
    (void)size; ecs_assert(size == sizeof(T), ECS_INTERNAL_ERROR, NULL);
    T *arr = static_cast<T*>(ptr);
    for (int i = 0; i < count; i ++) {
        arr[i].~T();
    }
}

// T& operator=(const T&)
template <typename T>
void copy_impl(
    ecs_world_t*, ecs_entity_t, const ecs_entity_t*, const ecs_entity_t*, 
    void *dst_ptr, const void *src_ptr, size_t size, int32_t count, void*)
{
    (void)size; ecs_assert(size == sizeof(T), ECS_INTERNAL_ERROR, NULL);
    T *dst_arr = static_cast<T*>(dst_ptr);
    const T *src_arr = static_cast<const T*>(src_ptr);
    for (int i = 0; i < count; i ++) {
        dst_arr[i] = src_arr[i];
    }
}

// T& operator=(T&&)
template <typename T>
void move_impl(
    ecs_world_t*, ecs_entity_t, const ecs_entity_t*, const ecs_entity_t*,
    void *dst_ptr, void *src_ptr, size_t size, int32_t count, void*)
{
    (void)size; ecs_assert(size == sizeof(T), ECS_INTERNAL_ERROR, NULL);
    T *dst_arr = static_cast<T*>(dst_ptr);
    T *src_arr = static_cast<T*>(src_ptr);
    for (int i = 0; i < count; i ++) {
        dst_arr[i] = std::move(src_arr[i]);
    }
}

// T(T&)
template <typename T>
void copy_ctor_impl(
    ecs_world_t*, ecs_entity_t, const EcsComponentLifecycle*, 
    const ecs_entity_t*, const ecs_entity_t*, void *dst_ptr, 
    const void *src_ptr, size_t size, int32_t count, void*)
{
    (void)size; ecs_assert(size == sizeof(T), ECS_INTERNAL_ERROR, NULL);
    T *dst_arr = static_cast<T*>(dst_ptr);
    const T *src_arr = static_cast<const T*>(src_ptr);
    for (int i = 0; i < count; i ++) {
        FLECS_PLACEMENT_NEW(&dst_arr[i], T(src_arr[i]));
    }
}

// T(T&&)
template <typename T>
void move_ctor_impl(
    ecs_world_t*, ecs_entity_t, const EcsComponentLifecycle*, 
    const ecs_entity_t*, const ecs_entity_t*, void *dst_ptr, 
    void *src_ptr, size_t size, int32_t count, void*)
{
    (void)size; ecs_assert(size == sizeof(T), ECS_INTERNAL_ERROR, NULL);
    T *dst_arr = static_cast<T*>(dst_ptr);
    T *src_arr = static_cast<T*>(src_ptr);
    for (int i = 0; i < count; i ++) {
        FLECS_PLACEMENT_NEW(&dst_arr[i], T(std::move(src_arr[i])));
    }
}

// T(T&&), ~T()
// Typically used when moving to a new table, and removing from the old table
template <typename T>
void merge_impl(
    ecs_world_t*, ecs_entity_t, const EcsComponentLifecycle*, 
    const ecs_entity_t*, const ecs_entity_t*, void *dst_ptr, 
    void *src_ptr, size_t size, int32_t count, void*)
{
    (void)size; ecs_assert(size == sizeof(T), ECS_INTERNAL_ERROR, NULL);
    T *dst_arr = static_cast<T*>(dst_ptr);
    T *src_arr = static_cast<T*>(src_ptr);
    for (int i = 0; i < count; i ++) {
        FLECS_PLACEMENT_NEW(&dst_arr[i], T(std::move(src_arr[i])));
        src_arr[i].~T();
    }
}

// Utility to return callback and whether callback is allowed
template <typename T>
struct lifecycle_callback_result {
    T callback;  // The callback function
    bool illegal; // If true, callback is not allowed
    
    static const lifecycle_callback_result is_illegal;
    static const lifecycle_callback_result not_set;
};

template <typename T>
const lifecycle_callback_result<T> 
    lifecycle_callback_result<T>::is_illegal = {nullptr, true};

template <typename T>
const lifecycle_callback_result<T> 
    lifecycle_callback_result<T>::not_set = {nullptr, false};

using ctor_result = lifecycle_callback_result<ecs_xtor_t>;
using dtor_result = lifecycle_callback_result<ecs_xtor_t>;
using copy_result = lifecycle_callback_result<ecs_copy_t>;
using move_result = lifecycle_callback_result<ecs_move_t>;
using copy_ctor_result = lifecycle_callback_result<ecs_copy_ctor_t>;
using move_ctor_result = lifecycle_callback_result<ecs_move_ctor_t>;
using merge_result = lifecycle_callback_result<ecs_move_ctor_t>;

} // _

// Trait to test if type has flecs constructor
template <typename T>
struct has_flecs_ctor {
    static constexpr bool value = 
        std::is_constructible<actual_type_t<T>, 
            flecs::world&, flecs::entity>::value;
};

// Trait to test if type is constructible by flecs
template <typename T>
struct is_flecs_constructible {
    static constexpr bool value = 
        std::is_default_constructible<actual_type_t<T>>::value ||
        std::is_constructible<actual_type_t<T>, 
            flecs::world&, flecs::entity>::value;
};

// Trait to test if type has a self constructor (flecs::entity, Args...)
template <typename T, typename ... Args>
struct is_self_constructible {
    static constexpr bool value = 
        std::is_constructible<actual_type_t<T>, 
            flecs::entity, Args...>::value;
};

namespace _
{

// Trivially constructible
template <typename T, if_t< std::is_trivially_constructible<T>::value > = 0>
ctor_result ctor() {
    return ctor_result::not_set;
}

// Not constructible by flecs
template <typename T, if_t< 
    ! std::is_default_constructible<T>::value &&
    ! has_flecs_ctor<T>::value > = 0>
ctor_result ctor() {
    return ctor_result::is_illegal;
}

// Default constructible
template <typename T, if_t<
    ! std::is_trivially_constructible<T>::value &&
    std::is_default_constructible<T>::value &&
    ! has_flecs_ctor<T>::value > = 0>
ctor_result ctor() {
    return {ctor_impl<T>, false};
}

// Flecs constructible: T(flecs::world, flecs::entity)
template <typename T, if_t< has_flecs_ctor<T>::value > = 0>
ctor_result ctor() {
    return {ctor_world_entity_impl<T>, false};
}

// No dtor
template <typename T, if_t< std::is_trivially_destructible<T>::value > = 0>
dtor_result dtor() {
    return dtor_result::not_set;
}

// Dtor
template <typename T, if_t<
    std::is_destructible<T>::value &&
    ! std::is_trivially_destructible<T>::value > = 0>
dtor_result dtor() {
    return {dtor_impl<T>, false};
}

// Assert when the type cannot be destructed
template <typename T, if_not_t< std::is_destructible<T>::value > = 0>
dtor_result dtor() {
    flecs_static_assert(always_false<T>::value, 
        "component type must be destructible");
    return dtor_result::is_illegal;
}

// Trivially copyable
template <typename T, if_t< std::is_trivially_copyable<T>::value > = 0>
copy_result copy() {
    return copy_result::not_set;
}

// Not copyable
template <typename T, if_t<
    ! std::is_trivially_copyable<T>::value &&
    ! std::is_copy_assignable<T>::value > = 0>
copy_result copy() {
    return copy_result::is_illegal;
}

// Copy assignment
template <typename T, if_t<
    std::is_copy_assignable<T>::value &&
    ! std::is_trivially_copyable<T>::value > = 0>
copy_result copy() {
    return {copy_impl<T>, false};
}

// Trivially move assignable
template <typename T, if_t< std::is_trivially_move_assignable<T>::value > = 0>
move_result move() {
    return move_result::not_set;
}

// Component types must be move assignable
template <typename T, if_not_t< std::is_move_assignable<T>::value > = 0>
move_result move() {
    flecs_static_assert(always_false<T>::value,
        "component type must be move assignable");
    return move_result::is_illegal;
}

// Move assignment
template <typename T, if_t<
    std::is_move_assignable<T>::value &&
    ! std::is_trivially_move_assignable<T>::value > = 0>
move_result move() {
    return {move_impl<T>, false};
}

// Trivially copy constructible
template <typename T, if_t<
    std::is_trivially_copy_constructible<T>::value > = 0>
copy_ctor_result copy_ctor() {
    return copy_ctor_result::not_set;
}

// No copy ctor
template <typename T, if_t< ! std::is_copy_constructible<T>::value > = 0>
copy_ctor_result copy_ctor() {
    return copy_ctor_result::is_illegal;
}

// Copy ctor
template <typename T, if_t<
    std::is_copy_constructible<T>::value &&
    ! std::is_trivially_copy_constructible<T>::value > = 0>
copy_ctor_result copy_ctor() {
    return {copy_ctor_impl<T>, false};
}

// Trivially move constructible
template <typename T, if_t<
    std::is_trivially_move_constructible<T>::value > = 0>
move_ctor_result move_ctor() {
    return move_ctor_result::not_set;
}

// Component types must be move constructible
template <typename T, if_not_t< std::is_move_constructible<T>::value > = 0>
move_ctor_result move_ctor() {
    flecs_static_assert(always_false<T>::value,
        "component type must be move constructible");    
    return move_ctor_result::is_illegal;
}

// Move ctor
template <typename T, if_t<
    std::is_move_constructible<T>::value &&
    ! std::is_trivially_move_constructible<T>::value > = 0>
move_ctor_result move_ctor() {
    return {move_ctor_impl<T>, false};
}

// Trivial merge (move assign + dtor)
template <typename T, if_t<
    std::is_trivially_move_constructible<T>::value  &&
    std::is_trivially_destructible<T>::value > = 0>
move_ctor_result merge() {
    return move_ctor_result::not_set;
}

// Component types must be move constructible and destructible
template <typename T, if_t<
    ! std::is_move_constructible<T>::value ||
    ! std::is_destructible<T>::value > = 0>
move_ctor_result merge() {
    flecs_static_assert(always_false<T>::value,
        "component type must be move constructible and destructible");
    return move_ctor_result::is_illegal;
}

// Merge (move assign + dtor)
template <typename T, if_t<
    !(std::is_trivially_move_constructible<T>::value  &&
      std::is_trivially_destructible<T>::value) &&
    std::is_move_constructible<T>::value &&
    std::is_destructible<T>::value > = 0>
move_ctor_result merge() {
    return {merge_impl<T>, false};
}

} // _
} // flecs
