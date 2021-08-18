
namespace flecs 
{

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

}
