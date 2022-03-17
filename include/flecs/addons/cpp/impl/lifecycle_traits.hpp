

namespace flecs 
{

namespace _
{

template <typename T>
inline void ctor_world_entity_impl(
    ecs_world_t* world, const ecs_entity_t* ids, void *ptr, 
    int32_t count, const ecs_type_info_t *info)
{
    (void)info; ecs_assert(info->size == ECS_SIZEOF(T), 
        ECS_INTERNAL_ERROR, NULL);
    T *arr = static_cast<T*>(ptr);
    flecs::world w(world);
    for (int i = 0; i < count; i ++) {
        flecs::entity e(world, ids[i]);
        FLECS_PLACEMENT_NEW(&arr[i], T(w, e));
    }
}

} // _
} // flecs
