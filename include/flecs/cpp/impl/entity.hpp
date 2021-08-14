
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

template <typename Base>
inline const Base& entity_builder<Base>::add(const type& type) const {
    ecs_add_type(this->base_world(), this->base_id(), type.c_ptr());
    return *this;
}

template <typename Base>
inline const Base& entity_builder<Base>::remove(const type& type) const {
    ecs_remove_type(this->base_world(), this->base_id(), type.c_ptr());
    return *this;
}

template <typename Base>
inline const Base& entity_builder<Base>::add_owned(const type& type) const {
    return add_owned(type.id());
}

template <typename Base>
inline const Base& entity_builder<Base>::add_switch(const type& sw) const {
    return add_switch(sw.id());
}

template <typename Base>
inline const Base& entity_builder<Base>::remove_switch(const type& sw) const {
    return remove_switch(sw.id());
}

template <typename Base>
template <typename Func, if_t< is_callable<Func>::value > >
inline const Base& entity_builder<Base>::set(const Func& func) const {
    _::entity_with_invoker<Func>::invoke_get_mut(
        this->base_world(), this->base_id(), func);
    return *this;
}

template <typename Base>
template <typename T>
inline const Base& entity_builder<Base>::component() const {
    component_for_id<T>(this->base_world(), this->base_id());
    return *this;
}

inline bool entity_view::has_switch(const flecs::type& type) const {
    return ecs_has_id(m_world, m_id, flecs::Switch | type.id());
}

inline flecs::entity entity_view::get_case(const flecs::type& sw) const {
    return flecs::entity(m_world, ecs_get_case(m_world, m_id, sw.id()));
}

inline flecs::entity entity_view::get_case(flecs::id_t sw) const {
    return flecs::entity(m_world, ecs_get_case(m_world, m_id, sw));
}

template <typename T>
inline flecs::entity entity_view::get_case() const {
    return get_case(_::cpp_type<T>::id(m_world));
}

inline flecs::entity entity_view::get_object(
    flecs::entity_t relation, 
    int32_t index) const 
{
    return flecs::entity(m_world, 
        ecs_get_object(m_world, m_id, relation, index));
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
    ecs_type_t type = ecs_type_from_id(m_world, m_id);
    return flecs::type(m_world, type);
}

inline child_iterator entity_view::children() const {
    ecs_assert(m_id != 0, ECS_INVALID_PARAMETER, NULL);
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
        flecs::id ent(m_world, id);
        func(ent); 

        // Case is not stored in type, so handle separately
        if ((id & ECS_ROLE_MASK) == flecs::Switch) {
            ent = flecs::id(
                m_world, flecs::Case | ecs_get_case(
                        m_world, m_id, ent.object().id()));
            func(ent);
        }
    }
}

template <typename Func>
inline void entity_view::match(id_t pattern, const Func& func) const {
    const ecs_vector_t *type = ecs_get_type(m_world, m_id);
    if (!type) {
        return;
    }

    id_t *ids = static_cast<ecs_id_t*>(
        _ecs_vector_first(type, ECS_VECTOR_T(ecs_id_t)));
    int32_t cur = 0;

    while (-1 != (cur = ecs_type_index_of(type, cur, pattern))) {
        flecs::id ent(m_world, ids[cur]);
        func(ent);
        cur ++;
    }
}

template <typename Func>
inline void entity_view::each(const flecs::entity_view& rel, const Func& func) const {
    return this->match(ecs_pair(rel, flecs::Wildcard), [&](flecs::id id) {
        flecs::entity obj = id.object();
        func(obj);
    });
}

template <typename Func, if_t< is_callable<Func>::value > >
inline bool entity_view::get(const Func& func) const {
    return _::entity_with_invoker<Func>::invoke_get(m_world, m_id, func);
}

template <typename T>
inline flecs::entity entity_view::get_parent() {
    return flecs::entity(m_world, ecs_get_parent_w_entity(m_world, m_id, 
            _::cpp_type<T>::id(m_world)));
}

inline flecs::entity entity_view::get_parent(flecs::entity_view e) {
    return flecs::entity(m_world, 
        ecs_get_parent_w_entity(m_world, m_id, e.id()));
}    

inline flecs::entity entity_view::lookup(const char *path) const {
    auto id = ecs_lookup_path_w_sep(m_world, m_id, path, "::", "::", false);
    return flecs::entity(m_world, id);
}

}
