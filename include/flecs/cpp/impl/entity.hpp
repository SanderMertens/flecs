
namespace flecs 
{

////////////////////////////////////////////////////////////////////////////////
//// Entity range, allows for operating on a range of consecutive entities
////////////////////////////////////////////////////////////////////////////////


class ECS_DEPRECATED("do not use") entity_range final {
public:
    entity_range(const world& world, std::int32_t count) 
        : m_world(world.c_ptr())
        , m_ids( ecs_bulk_new_w_type(m_world, nullptr, count)) { }

    entity_range(const world& world, std::int32_t count, flecs::type type) 
        : m_world(world.c_ptr())
        , m_ids( ecs_bulk_new_w_type(m_world, type.c_ptr(), count)) { }

private:
    world_t *m_world;
    const entity_t *m_ids;
};

template <typename T>
flecs::entity ref<T>::entity() const {
    return flecs::entity(m_world, m_entity);
}

template <typename base>
inline typename entity_builder<base>::base_type& entity_builder<base>::add(const type& type) const {
    ecs_add_type(world(), id(), type.c_ptr());
    return *base();
}

template <typename base>
inline typename entity_builder<base>::base_type& entity_builder<base>::remove(const type& type) const {
    ecs_remove_type(world(), id(), type.c_ptr());
    return *base();
}

template <typename base>
inline typename entity_builder<base>::base_type& entity_builder<base>::add_owned(const type& type) const {
    return add_owned(type.id());
}

template <typename base>
inline typename entity_builder<base>::base_type& entity_builder<base>::add_switch(const type& sw) const {
    return add_switch(sw.id());
}

template <typename base>
inline typename entity_builder<base>::base_type& entity_builder<base>::remove_switch(const type& sw) const {
    return remove_switch(sw.id());
}

inline bool entity::has_switch(const flecs::type& type) const {
    return ecs_has_entity(m_world, m_id, flecs::Switch | type.id());
}

inline flecs::entity entity::get_case(const flecs::type& sw) const {
    return flecs::entity(m_world, ecs_get_case(m_world, m_id, sw.id()));
}

}
