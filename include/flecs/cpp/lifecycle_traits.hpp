namespace flecs 
{

namespace _ 
{

inline void ecs_ctor_illegal(ecs_world_t* w, ecs_entity_t id, const ecs_entity_t*, 
    void *, size_t, int32_t, void*)
{
    char *path = ecs_get_path_w_sep(w, 0, id, "::", "::");
    ecs_abort(ECS_INVALID_OPERATION, 
        "cannnot default construct %s, add %s::%s() or use emplace<T>", 
        path, path, ecs_get_name(w, id));
    ecs_os_free(path);
}

inline void ecs_dtor_illegal(ecs_world_t* w, ecs_entity_t id, const ecs_entity_t*, 
    void *, size_t, int32_t, void*)
{
    char *path = ecs_get_path_w_sep(w, 0, id, "::", "::");
    ecs_abort(ECS_INVALID_OPERATION, "cannnot destruct %s, add ~%s::%s()", 
        path, path, ecs_get_name(w, id));
    ecs_os_free(path);
}

inline void ecs_copy_illegal(ecs_world_t* w, ecs_entity_t id, const ecs_entity_t*, 
    const ecs_entity_t*, void *, const void *, size_t, int32_t, void*)
{
    char *path = ecs_get_path_w_sep(w, 0, id, "::", "::");
    ecs_abort(ECS_INVALID_OPERATION, 
        "cannnot copy assign %s, add %s& %s::operator =(const %s&)", path, 
        ecs_get_name(w, id), path, ecs_get_name(w, id), ecs_get_name(w, id));
    ecs_os_free(path);
}

inline void ecs_move_illegal(ecs_world_t* w, ecs_entity_t id, const ecs_entity_t*, 
    const ecs_entity_t*, void *, void *, size_t, int32_t, void*)
{
    char *path = ecs_get_path_w_sep(w, 0, id, "::", "::");
    ecs_abort(ECS_INVALID_OPERATION, 
        "cannnot move assign %s, add %s& %s::operator =(%s&&)", path, 
        ecs_get_name(w, id), path, ecs_get_name(w, id), ecs_get_name(w, id));
    ecs_os_free(path);
}

inline void ecs_copy_ctor_illegal(ecs_world_t* w, ecs_entity_t id, 
    const EcsComponentLifecycle*, const ecs_entity_t*, const ecs_entity_t*, 
    void *, const void *, size_t, int32_t, void*)
{
    char *path = ecs_get_path_w_sep(w, 0, id, "::", "::");
    ecs_abort(ECS_INVALID_OPERATION, 
        "cannnot copy construct %s, add %s::%s(const %s&)",
        path, path, ecs_get_name(w, id), ecs_get_name(w, id));
    ecs_os_free(path);
}

inline void ecs_move_ctor_illegal(ecs_world_t* w, ecs_entity_t id, 
    const EcsComponentLifecycle*, const ecs_entity_t*, const ecs_entity_t*, 
    void *, void *, size_t, int32_t, void*)
{
    char *path = ecs_get_path_w_sep(w, 0, id, "::", "::");
    ecs_abort(ECS_INVALID_OPERATION, 
        "cannnot move construct %s, add %s::%s(%s&&)",
        path, path, ecs_get_name(w, id), ecs_get_name(w, id));
    ecs_os_free(path);
}


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
void ctor_move_dtor_impl(
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

// Move assign + dtor (non-trivial move assigmnment)
// Typically used when moving a component to a deleted component
template <typename T, if_not_t<
    std::is_trivially_move_assignable<T>::value > = 0>
void move_dtor_impl(
    ecs_world_t*, ecs_entity_t, const EcsComponentLifecycle*, 
    const ecs_entity_t*, const ecs_entity_t*, void *dst_ptr, 
    void *src_ptr, size_t size, int32_t count, void*)
{
    (void)size; ecs_assert(size == sizeof(T), ECS_INTERNAL_ERROR, NULL);
    T *dst_arr = static_cast<T*>(dst_ptr);
    T *src_arr = static_cast<T*>(src_ptr);
    for (int i = 0; i < count; i ++) {
        // Move assignment should free dst & assign dst to src
        dst_arr[i] = std::move(src_arr[i]);
        // Destruct src. Move should have left object in a state where it no
        // longer holds resources, but it still needs to be destructed.
        src_arr[i].~T();
    }
}

// Move assign + dtor (trivial move assigmnment)
// Typically used when moving a component to a deleted component
template <typename T, if_t<
    std::is_trivially_move_assignable<T>::value > = 0>
void move_dtor_impl(
    ecs_world_t*, ecs_entity_t, const EcsComponentLifecycle*, 
    const ecs_entity_t*, const ecs_entity_t*, void *dst_ptr, 
    void *src_ptr, size_t size, int32_t count, void*)
{
    (void)size; ecs_assert(size == sizeof(T), ECS_INTERNAL_ERROR, NULL);
    T *dst_arr = static_cast<T*>(dst_ptr);
    T *src_arr = static_cast<T*>(src_ptr);
    for (int i = 0; i < count; i ++) {
        // Cleanup resources of dst
        dst_arr[i].~T();
        // Copy src to dst
        dst_arr[i] = std::move(src_arr[i]);
        // No need to destruct src. Since this is a trivial move the code
        // should be agnostic to the address of the component which means we
        // can pretend nothing got destructed.
    }
}

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
ecs_xtor_t ctor() {
    return nullptr;
}

// Not constructible by flecs
template <typename T, if_t< 
    ! std::is_default_constructible<T>::value &&
    ! has_flecs_ctor<T>::value > = 0>
ecs_xtor_t ctor() {
    return ecs_ctor_illegal;
}

// Default constructible
template <typename T, if_t<
    ! std::is_trivially_constructible<T>::value &&
    std::is_default_constructible<T>::value &&
    ! has_flecs_ctor<T>::value > = 0>
ecs_xtor_t ctor() {
    return ctor_impl<T>;
}

// Flecs constructible: T(flecs::world, flecs::entity)
template <typename T, if_t< has_flecs_ctor<T>::value > = 0>
ecs_xtor_t ctor() {
    return ctor_world_entity_impl<T>;
}

// No dtor
template <typename T, if_t< std::is_trivially_destructible<T>::value > = 0>
ecs_xtor_t dtor() {
    return nullptr;
}

// Dtor
template <typename T, if_t<
    std::is_destructible<T>::value &&
    ! std::is_trivially_destructible<T>::value > = 0>
ecs_xtor_t dtor() {
    return dtor_impl<T>;
}

// Assert when the type cannot be destructed
template <typename T, if_not_t< std::is_destructible<T>::value > = 0>
ecs_xtor_t dtor() {
    flecs_static_assert(always_false<T>::value, 
        "component type must be destructible");
    return ecs_dtor_illegal;
}

// Trivially copyable
template <typename T, if_t< std::is_trivially_copyable<T>::value > = 0>
ecs_copy_t copy() {
    return nullptr;
}

// Not copyable
template <typename T, if_t<
    ! std::is_trivially_copyable<T>::value &&
    ! std::is_copy_assignable<T>::value > = 0>
ecs_copy_t copy() {
    return ecs_copy_illegal;
}

// Copy assignment
template <typename T, if_t<
    std::is_copy_assignable<T>::value &&
    ! std::is_trivially_copyable<T>::value > = 0>
ecs_copy_t copy() {
    return copy_impl<T>;
}

// Trivially move assignable
template <typename T, if_t< std::is_trivially_move_assignable<T>::value > = 0>
ecs_move_t move() {
    return nullptr;
}

// Component types must be move assignable
template <typename T, if_not_t< std::is_move_assignable<T>::value > = 0>
ecs_move_t move() {
    flecs_static_assert(always_false<T>::value,
        "component type must be move assignable");
    return ecs_move_illegal;
}

// Move assignment
template <typename T, if_t<
    std::is_move_assignable<T>::value &&
    ! std::is_trivially_move_assignable<T>::value > = 0>
ecs_move_t move() {
    return move_impl<T>;
}

// Trivially copy constructible
template <typename T, if_t<
    std::is_trivially_copy_constructible<T>::value > = 0>
ecs_copy_ctor_t copy_ctor() {
    return nullptr;
}

// No copy ctor
template <typename T, if_t< ! std::is_copy_constructible<T>::value > = 0>
ecs_copy_ctor_t copy_ctor() {
    return ecs_copy_ctor_illegal;
}

// Copy ctor
template <typename T, if_t<
    std::is_copy_constructible<T>::value &&
    ! std::is_trivially_copy_constructible<T>::value > = 0>
ecs_copy_ctor_t copy_ctor() {
    return copy_ctor_impl<T>;
}

// Trivially move constructible
template <typename T, if_t<
    std::is_trivially_move_constructible<T>::value > = 0>
ecs_move_ctor_t move_ctor() {
    return nullptr;
}

// Component types must be move constructible
template <typename T, if_not_t< std::is_move_constructible<T>::value > = 0>
ecs_move_ctor_t move_ctor() {
    flecs_static_assert(always_false<T>::value,
        "component type must be move constructible");    
    return ecs_move_ctor_illegal;
}

// Move ctor
template <typename T, if_t<
    std::is_move_constructible<T>::value &&
    ! std::is_trivially_move_constructible<T>::value > = 0>
ecs_move_ctor_t move_ctor() {
    return move_ctor_impl<T>;
}

// Trivial merge (move assign + dtor)
template <typename T, if_t<
    std::is_trivially_move_constructible<T>::value  &&
    std::is_trivially_destructible<T>::value > = 0>
ecs_move_ctor_t ctor_move_dtor() {
    return nullptr;
}

// Component types must be move constructible and destructible
template <typename T, if_t<
    ! std::is_move_constructible<T>::value ||
    ! std::is_destructible<T>::value > = 0>
ecs_move_ctor_t ctor_move_dtor() {
    flecs_static_assert(always_false<T>::value,
        "component type must be move constructible and destructible");
    return ecs_move_ctor_illegal;
}

// Merge ctor + dtor
template <typename T, if_t<
    !(std::is_trivially_move_constructible<T>::value &&
      std::is_trivially_destructible<T>::value) &&
    std::is_move_constructible<T>::value &&
    std::is_destructible<T>::value > = 0>
ecs_move_ctor_t ctor_move_dtor() {
    return ctor_move_dtor_impl<T>;
}

// Trivial merge (move assign + dtor)
template <typename T, if_t<
    std::is_trivially_move_assignable<T>::value  &&
    std::is_trivially_destructible<T>::value > = 0>
ecs_move_ctor_t move_dtor() {
    return nullptr;
}

// Component types must be move constructible and destructible
template <typename T, if_t<
    ! std::is_move_assignable<T>::value ||
    ! std::is_destructible<T>::value > = 0>
ecs_move_ctor_t move_dtor() {
    flecs_static_assert(always_false<T>::value,
        "component type must be move constructible and destructible");
    return ecs_move_ctor_illegal;
}

// Merge assign + dtor
template <typename T, if_t<
    !(std::is_trivially_move_assignable<T>::value &&
      std::is_trivially_destructible<T>::value) &&
    std::is_move_assignable<T>::value &&
    std::is_destructible<T>::value > = 0>
ecs_move_ctor_t move_dtor() {
    return move_dtor_impl<T>;
}

} // _
} // flecs
