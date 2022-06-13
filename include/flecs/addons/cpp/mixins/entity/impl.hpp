#pragma once

namespace flecs {

template <typename T>
flecs::entity ref<T>::entity() const {
    return flecs::entity(m_world, m_ref.entity);
}

template <typename Self>
template <typename Func, if_t< is_callable<Func>::value > >
inline Self& entity_builder<Self>::set(const Func& func) {
    _::entity_with_invoker<Func>::invoke_get_mut(
        this->m_world, this->m_id, func);
    return to_base();
}

template <typename T, if_t< is_enum<T>::value > >
const T* entity_view::get() const {
    entity_t r = _::cpp_type<T>::id(m_world);
    entity_t c = ecs_get_object(m_world, m_id, r, 0);

    if (c) {
        // Get constant value from constant entity
        const T* v = static_cast<const T*>(ecs_get_id(m_world, c, r));
        ecs_assert(v != NULL, ECS_INTERNAL_ERROR, 
            "missing enum constant value");
        return v;
    } else {
        // If there is no matching pair for (r, *), try just r
        return static_cast<const T*>(ecs_get_id(m_world, m_id, r));
    }
}

template<typename R>
inline flecs::entity entity_view::get_object(int32_t index) const 
{
    return flecs::entity(m_world, 
        ecs_get_object(m_world, m_id, _::cpp_type<R>::id(m_world), index));
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
    return flecs::entity(stage, m_id);
}   

inline flecs::type entity_view::type() const {
    return flecs::type(m_world, ecs_get_type(m_world, m_id));
}

inline flecs::table entity_view::table() const {
    return flecs::table(m_world, ecs_get_table(m_world, m_id));
}

template <typename Func>
inline void entity_view::each(const Func& func) const {
    const ecs_type_t *type = ecs_get_type(m_world, m_id);
    if (!type) {
        return;
    }

    const ecs_id_t *ids = type->array;
    int32_t count = type->count;

    for (int i = 0; i < count; i ++) {
        ecs_id_t id = ids[i];
        flecs::id ent(m_world, id);
        func(ent); 

        // Union object is not stored in type, so handle separately
        if (ECS_PAIR_FIRST(id) == EcsUnion) {
            ent = flecs::id(m_world, ECS_PAIR_SECOND(id),
                ecs_get_object(m_world, m_id, ECS_PAIR_SECOND(id), 0));
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

    const ecs_type_t *type = ecs_table_get_type(table);
    if (!type) {
        return;
    }

    flecs::id_t pattern = pred;
    if (obj) {
        pattern = ecs_pair(pred, obj);
    }

    int32_t cur = 0;
    id_t *ids = type->array;
    
    while (-1 != (cur = ecs_search_offset(real_world, table, cur, pattern, 0)))
    {
        flecs::id ent(m_world, ids[cur]);
        func(ent);
        cur ++;
    }
}

template <typename Func>
inline void entity_view::each(const flecs::entity_view& rel, const Func& func) const {
    return this->each(rel, flecs::Wildcard, [&](flecs::id id) {
        flecs::entity obj = id.second();
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

inline flecs::entity entity_view::clone(bool copy_value, flecs::entity_t dst_id) const {
    if (!dst_id) {
        dst_id = ecs_new_id(m_world);
    }

    flecs::entity dst = flecs::entity(m_world, dst_id);
    ecs_clone(m_world, dst_id, m_id, copy_value);
    return dst;
}

// Entity mixin implementation
template <typename... Args>
inline flecs::entity world::entity(Args &&... args) const {
    return flecs::entity(m_world, FLECS_FWD(args)...);
}

template <typename E, if_t< is_enum<E>::value >>
inline flecs::entity world::id(E value) const {
    flecs::entity_t constant = enum_type<E>(m_world).entity(value);
    return flecs::entity(m_world, constant);
}

template <typename T>
inline flecs::entity world::entity(const char *name) const {
    return flecs::entity(m_world, 
        _::cpp_type<T>::id_explicit(m_world, name, true, 0, false) );
}

template <typename... Args>
inline flecs::entity world::prefab(Args &&... args) const {
    flecs::entity result = flecs::entity(m_world, FLECS_FWD(args)...);
    result.add(flecs::Prefab);
    return result;
}

template <typename T>
inline flecs::entity world::prefab(const char *name) const {
    flecs::entity result = flecs::component<T>(m_world, name, true);
    result.add(flecs::Prefab);
    return result;
}

}
