

namespace flecs 
{

namespace _
{

template <typename T>
inline void ctor_world_entity_impl(
    ecs_world_t* world, ecs_entity_t, const ecs_entity_t* ids, void *ptr, 
    size_t size, int32_t count, void*)
{
    (void)size; ecs_assert(size == sizeof(T), ECS_INTERNAL_ERROR, NULL);
    T *arr = static_cast<T*>(ptr);
    flecs::world w(world);
    for (int i = 0; i < count; i ++) {
        flecs::entity e(world, ids[i]);
        FLECS_PLACEMENT_NEW(&arr[i], T(w, e));
    }
}

} // _
} // flecs
