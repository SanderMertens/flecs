
namespace flecs {

inline flecs::entity id::entity() const {
    ecs_assert(!is_pair(), ECS_INVALID_OPERATION, NULL);
    ecs_assert(!role(), ECS_INVALID_OPERATION, NULL);
    return flecs::entity(m_world, m_id);
}

inline flecs::entity id::role() const {
    return flecs::entity(m_world, m_id & ECS_ROLE_MASK);
}

inline flecs::entity id::relation() const {
    ecs_assert(is_pair(), ECS_INVALID_OPERATION, NULL);

    flecs::entity_t e = ECS_PAIR_RELATION(m_id);
    if (m_world) {
        return flecs::entity(m_world, ecs_get_alive(m_world, e));
    } else {
        return flecs::entity(e);
    }
}

inline flecs::entity id::object() const {
    flecs::entity_t e = ECS_PAIR_OBJECT(m_id);
    if (m_world) {
        return flecs::entity(m_world, ecs_get_alive(m_world, e));
    } else {
        return flecs::entity(m_world, e);
    }
}

inline flecs::entity id::add_role(flecs::id_t role) const {
    return flecs::entity(m_world, m_id | role);
}

inline flecs::entity id::remove_role(flecs::id_t role) const {
    (void)role;
    ecs_assert((m_id & ECS_ROLE_MASK) == role, ECS_INVALID_PARAMETER, NULL);
    return flecs::entity(m_world, m_id & ECS_COMPONENT_MASK);
}

inline flecs::entity id::remove_role() const {
    return flecs::entity(m_world, m_id & ECS_COMPONENT_MASK);
}

inline flecs::entity id::remove_generation() const {
    return flecs::entity(m_world, static_cast<uint32_t>(m_id));
}

inline flecs::world id::world() const {
    return flecs::world(m_world);
}


// Id mixin implementation

#define me_ this->me()

template <typename T>
inline flecs::id id_m_world::id() const {
    return flecs::id(me_, _::cpp_type<T>::id(me_));
}

template <typename ... Args>
inline flecs::id id_m_world::id(Args&&... args) const {
    return flecs::id(me_, std::forward<Args>(args)...);
}

template <typename R, typename O>
inline flecs::id id_m_world::pair() const {
    return flecs::id(
        me_, 
        ecs_pair(
            _::cpp_type<R>::id(me_), 
            _::cpp_type<O>::id(me_)));
}

template <typename R>
inline flecs::id id_m_world::pair(entity_t o) const {
    return flecs::id(
        me_,
        ecs_pair(
            _::cpp_type<R>::id(me_), 
            o));
}

inline flecs::id id_m_world::pair(entity_t r, entity_t o) const {
    return flecs::id(
        me_,
        ecs_pair(r, o));
}

#undef me_

}
