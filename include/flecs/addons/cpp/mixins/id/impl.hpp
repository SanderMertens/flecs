/**
 * @file addons/cpp/mixins/id/impl.hpp
 * @brief Id class implementation.
 */

#pragma once

namespace flecs {

inline flecs::entity id::entity() const {
    ecs_assert(!is_pair(), ECS_INVALID_OPERATION, NULL);
    ecs_assert(!flags(), ECS_INVALID_OPERATION, NULL);
    return flecs::entity(m_world, m_id);
}

inline flecs::entity id::flags() const {
    return flecs::entity(m_world, m_id & ECS_ID_FLAGS_MASK);
}

inline flecs::entity id::first() const {
    ecs_assert(is_pair(), ECS_INVALID_OPERATION, NULL);

    flecs::entity_t e = ECS_PAIR_FIRST(m_id);
    if (m_world) {
        return flecs::entity(m_world, ecs_get_alive(m_world, e));
    } else {
        return flecs::entity(e);
    }
}

inline flecs::entity id::second() const {
    flecs::entity_t e = ECS_PAIR_SECOND(m_id);
    if (m_world) {
        return flecs::entity(m_world, ecs_get_alive(m_world, e));
    } else {
        return flecs::entity(m_world, e);
    }
}

inline flecs::entity id::add_flags(flecs::id_t flags) const {
    return flecs::entity(m_world, m_id | flags);
}

inline flecs::entity id::remove_flags(flecs::id_t flags) const {
    (void)flags;
    ecs_assert((m_id & ECS_ID_FLAGS_MASK) == flags, ECS_INVALID_PARAMETER, NULL);
    return flecs::entity(m_world, m_id & ECS_COMPONENT_MASK);
}

inline flecs::entity id::remove_flags() const {
    return flecs::entity(m_world, m_id & ECS_COMPONENT_MASK);
}

inline flecs::entity id::remove_generation() const {
    return flecs::entity(m_world, static_cast<uint32_t>(m_id));
}

inline flecs::world id::world() const {
    return flecs::world(m_world);
}

inline flecs::entity id::type_id() const {
    return flecs::entity(m_world, ecs_get_typeid(m_world, m_id));
}


// Id mixin implementation

template <typename T>
inline flecs::id world::id() const {
    return flecs::id(m_world, _::cpp_type<T>::id(m_world));
}

template <typename ... Args>
inline flecs::id world::id(Args&&... args) const {
    return flecs::id(m_world, FLECS_FWD(args)...);
}

template <typename First, typename Second>
inline flecs::id world::pair() const {
    return flecs::id(
        m_world, 
        ecs_pair(
            _::cpp_type<First>::id(m_world), 
            _::cpp_type<Second>::id(m_world)));
}

template <typename First>
inline flecs::id world::pair(entity_t o) const {
    return flecs::id(
        m_world,
        ecs_pair(
            _::cpp_type<First>::id(m_world), 
            o));
}

inline flecs::id world::pair(entity_t r, entity_t o) const {
    return flecs::id(
        m_world,
        ecs_pair(r, o));
}

}
