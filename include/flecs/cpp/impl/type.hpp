
namespace flecs 
{

inline flecs::type entity::type() const {
    return flecs::type(m_world, ecs_get_type(m_world, m_id));
}

inline flecs::type entity::to_type() const {
    ecs_type_t type = ecs_type_from_entity(m_world, m_id);
    return flecs::type(m_world, type);
}

inline child_iterator entity::children() const {
    return flecs::child_iterator(*this);
}

} // namespace flecs
