/**
 * @file addons/cpp/ref.hpp
 * @brief Class that caches data to speedup get operations.
 */

#pragma once

namespace flecs
{

/**
 * @defgroup cpp_ref Refs
 * @brief Refs are a fast mechanism for referring to a specific entity/component.
 * 
 * \ingroup cpp_core
 * @{
 */

/** Component reference.
 * Reference to a component from a specific entity.
 */
template <typename T>
struct ref {
    ref(world_t *world, entity_t entity, flecs::id_t id = 0)
        : m_ref()
    {
        // the world we were called with may be a stage; convert it to a world
        // here if that is the case
        m_world = world ? const_cast<flecs::world_t *>(ecs_get_world(world))
            : nullptr;
        if (!id) {
            id = _::cpp_type<T>::id(world);
        }

        ecs_assert(_::cpp_type<T>::size() != 0, ECS_INVALID_PARAMETER, NULL);

        m_ref = ecs_ref_init_id(m_world, entity, id);
    }

    T* operator->() {
        T* result = static_cast<T*>(ecs_ref_get_id(
            m_world, &m_ref, this->m_ref.id));

        ecs_assert(result != NULL, ECS_INVALID_PARAMETER, NULL);

        return result;
    }

    T* get() {
        return static_cast<T*>(ecs_ref_get_id(
            m_world, &m_ref, this->m_ref.id));
    }

    flecs::entity entity() const;

private:
    world_t *m_world;
    flecs::ref_t m_ref;
};

/** @} */

}
