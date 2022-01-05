#pragma once

namespace flecs {

template <typename T>
flecs::entity ref<T>::entity() const {
    return flecs::entity(m_world, m_entity);
}

template <typename Base>
inline Base& entity_builder_i<Base>::add_switch(const type& sw) {
    return add_switch(sw.id());
}

template <typename Base>
inline Base& entity_builder_i<Base>::remove_switch(const type& sw) {
    return remove_switch(sw.id());
}

template <typename Base>
template <typename Func, if_t< is_callable<Func>::value > >
inline Base& entity_builder_i<Base>::set(const Func& func) {
    _::entity_with_invoker<Func>::invoke_get_mut(
        this->world_v(), this->id_v(), func);
    return to_base();
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

inline flecs::entity entity_view::get_object_for(
    flecs::entity_t relation, 
    flecs::id_t id) const 
{
    return flecs::entity(m_world, 
        ecs_get_object_for_id(m_world, m_id, relation, id));
}

template <typename T>
inline flecs::entity entity_view::get_object_for(flecs::entity_t relation) const {
    return get_object_for(relation, _::cpp_type<T>::id(m_world));
}

template <typename R, typename O>
inline flecs::entity entity_view::get_object_for(flecs::entity_t relation) const {
    return get_object_for(relation, _::cpp_type<R, O>::id(m_world));
}

inline flecs::entity entity_view::mut(const flecs::world& stage) const {
    ecs_assert(!stage.is_readonly(), ECS_INVALID_PARAMETER, 
        "cannot use readonly world/stage to create mutable handle");
    return flecs::entity(m_id).set_stage(stage.c_ptr());
}

inline flecs::entity entity_view::mut(const flecs::iter& it) const {
    ecs_assert(!it.world().is_readonly(), ECS_INVALID_PARAMETER, 
        "cannot use iterator created for readonly world/stage to create mutable handle");
    return flecs::entity(m_id).set_stage(it.world().c_ptr());
}

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
    return flecs::type(m_world, ecs_get_table(m_world, m_id));
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
inline void entity_view::each(flecs::id_t pred, flecs::id_t obj, const Func& func) const {
    flecs::world_t *real_world = const_cast<flecs::world_t*>(
        ecs_get_world(m_world));

    const ecs_table_t *table = ecs_get_table(m_world, m_id);
    if (!table) {
        return;
    }

    const ecs_vector_t *type = ecs_table_get_type(table);
    if (!type) {
        return;
    }

    flecs::id_t pattern = pred;
    if (obj) {
        pattern = ecs_pair(pred, obj);
    }

    int32_t cur = 0;
    id_t *ids = static_cast<ecs_id_t*>(
        _ecs_vector_first(type, ECS_VECTOR_T(ecs_id_t)));
    
    while (-1 != (cur = ecs_type_match(
        real_world, table, type, cur, pattern, 0, 0, 0, NULL, NULL, NULL))) 
    {
        flecs::id ent(m_world, ids[cur]);
        func(ent);
        cur ++;
    }
}

template <typename Func>
inline void entity_view::each(const flecs::entity_view& rel, const Func& func) const {
    return this->each(rel, flecs::Wildcard, [&](flecs::id id) {
        flecs::entity obj = id.object();
        func(obj);
    });
}

template <typename Func, if_t< is_callable<Func>::value > >
inline bool entity_view::get(const Func& func) const {
    return _::entity_with_invoker<Func>::invoke_get(m_world, m_id, func);
} 

inline flecs::entity entity_view::lookup(const char *path) const {
    auto id = ecs_lookup_path_w_sep(m_world, m_id, path, "::", "::", false);
    return flecs::entity(m_world, id);
}


#define flecs_me_ this->me()

// Entity mixin implementation
template <typename... Args>
inline flecs::entity entity_m_world::entity(Args &&... args) const {
    return flecs::entity(flecs_me_, std::forward<Args>(args)...);
}

template <typename T>
inline flecs::entity entity_m_world::entity(const char *name) const {
    return flecs::component<T>(flecs_me_, name, true);
}

template <typename... Args>
inline flecs::entity entity_m_world::prefab(Args &&... args) const {
    flecs::entity result = flecs::entity(flecs_me_, std::forward<Args>(args)...);
    result.add(flecs::Prefab);
    return result;
}

template <typename T>
inline flecs::entity entity_m_world::prefab(const char *name) const {
    flecs::entity result = flecs::component<T>(flecs_me_, name, true);
    result.add(flecs::Prefab);
    return result;
}

#undef flecs_me_

}
