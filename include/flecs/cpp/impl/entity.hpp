
namespace flecs 
{

////////////////////////////////////////////////////////////////////////////////
//// Entity range, allows for operating on a range of consecutive entities
////////////////////////////////////////////////////////////////////////////////

class ECS_DEPRECATED("do not use") entity_range final {
public:
    entity_range(const world& world, int32_t count) 
        : m_world(world.c_ptr())
        , m_ids( ecs_bulk_new_w_type(m_world, nullptr, count)) { }

    entity_range(const world& world, int32_t count, flecs::type type) 
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

inline bool entity_view::has_switch(const flecs::type& type) const {
    return ecs_has_entity(m_world, m_id, flecs::Switch | type.id());
}

inline flecs::entity_view entity_view::get_case(const flecs::type& sw) const {
    return flecs::entity(m_world, ecs_get_case(m_world, m_id, sw.id()));
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

inline entity id::lo() const {
    return entity(world(), ecs_entity_t_lo(m_id));
}

inline entity id::hi() const {
    return entity(world(), ecs_entity_t_hi(m_id));
}

inline entity id::comb(entity_view lo, entity_view hi) {
    return entity(lo.world(), 
        ecs_entity_t_comb(lo.id(), hi.id()));
}

inline flecs::entity entity_view::mut(const flecs::world& stage) const {
    ecs_assert(!stage.is_readonly(), ECS_INVALID_PARAMETER, 
        "cannot use readonly world/stage to create mutable handle");
    return flecs::entity(m_id).set_stage(stage.c_ptr());
}

/** Same as mut(world), but for iterator.
    * This operation allows for the construction of a mutable entity handle
    * from an iterator.
    *
    * @param stage An created for the current stage.
    * @return An entity handle that allows for mutations in the current stage.
    */
inline flecs::entity entity_view::mut(const flecs::iter& it) const {
    ecs_assert(!it.world().is_readonly(), ECS_INVALID_PARAMETER, 
        "cannot use iterator created for readonly world/stage to create mutable handle");
    return flecs::entity(m_id).set_stage(it.world().c_ptr());
}

/** Same as mut(world), but for entity.
    * This operation allows for the construction of a mutable entity handle
    * from another entity. This is useful in each() functions, which only 
    * provide a handle to the entity being iterated over.
    *
    * @param stage An created for the current stage.
    * @return An entity handle that allows for mutations in the current stage.
    */
inline flecs::entity entity_view::mut(const flecs::entity_view& e) const {
    ecs_assert(!e.world().is_readonly(), ECS_INVALID_PARAMETER, 
        "cannot use entity created for readonly world/stage to create mutable handle");
    return flecs::entity(m_id).set_stage(e.m_world);
}

inline flecs::entity entity_view::set_stage(world_t *stage) {
    m_world = stage;
    return flecs::entity(m_world, m_id);
}   

inline flecs::type entity_view::type() const {
    return flecs::type(m_world, ecs_get_type(m_world, m_id));
}

inline flecs::type entity_view::to_type() const {
    ecs_type_t type = ecs_type_from_entity(m_world, m_id);
    return flecs::type(m_world, type);
}

inline child_iterator entity_view::children() const {
    return flecs::child_iterator(*this);
}

template <typename Func>
inline void entity_view::each(const Func& func) const {
    const ecs_vector_t *type = ecs_get_type(m_world, m_id);
    if (!type) {
        return;
    }

    const ecs_id_t *ids = static_cast<ecs_id_t*>(
        _ecs_vector_first(type, ECS_VECTOR_T(ecs_id_t)));
    int32_t count = ecs_vector_count(type);

    for (int i = 0; i < count; i ++) {
        ecs_id_t id = ids[i];
        flecs::entity ent(m_world, id);
        func(ent); 

        // Case is not stored in type, so handle separately
        if ((id & ECS_ROLE_MASK) == flecs::Switch) {
            ent = flecs::entity(
                m_world, flecs::Case | ecs_get_case(
                        m_world, m_id, ent.object().id()));
            func(ent);
        }
    }
}

template <typename Func>
inline void entity_view::each(const flecs::entity_view& rel, const Func& func) const {
    const ecs_vector_t *type = ecs_get_type(m_world, m_id);
    if (!type) {
        return;
    }

    flecs::entity_t rel_id = rel.remove_generation().id();

    const ecs_id_t *ids = static_cast<ecs_id_t*>(
        _ecs_vector_first(type, ECS_VECTOR_T(ecs_id_t)));
    int32_t count = ecs_vector_count(type);

    // First, skip to the point where the relationship starts
    // TODO: replace this with an O(1) search when the new table lookup
    //       datastructures land
    int i;
    for (i = 0; i < count; i ++) {
        ecs_id_t id = ids[i];
        if (ECS_PAIR_RELATION(id) == rel_id) {
            break;
        }
    }

    // Iterate all entries until the relationship stops
    for (; i < count; i ++) {
        ecs_id_t id = ids[i];
        if (ECS_PAIR_RELATION(id) != rel_id) {
            break;
        }

        flecs::id id_cl(m_world, ids[i]);
        flecs::entity ent(m_world, id_cl.object());
        func(ent);         
    }
}

}
