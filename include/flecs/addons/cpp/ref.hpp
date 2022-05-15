namespace flecs
{

template <typename T>
struct ref {
    ref()
        : m_world( nullptr )
        , m_ref() { }

    ref(world_t *world, entity_t entity) 
        : m_world( world )
        , m_ref() 
    {
        auto comp_id = _::cpp_type<T>::id(world);

        ecs_assert(_::cpp_type<T>::size() != 0, 
                ECS_INVALID_PARAMETER, NULL);

        m_ref = ecs_ref_init_id(m_world, entity, comp_id);
    }

    T* operator->() {
        T* result = static_cast<T*>(ecs_ref_get_id(
            m_world, &m_ref, _::cpp_type<T>::id(m_world)));

        ecs_assert(result != NULL, ECS_INVALID_PARAMETER, NULL);

        return result;
    }

    T* get() {
        return static_cast<T*>(ecs_ref_get_id(
            m_world, &m_ref, _::cpp_type<T>::id(m_world)));
    }

    flecs::entity entity() const;

private:
    world_t *m_world;
    flecs::ref_t m_ref;
};

}
