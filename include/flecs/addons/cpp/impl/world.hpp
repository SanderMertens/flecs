/**
 * @file addons/cpp/impl/world.hpp
 * @brief World implementation.
 */

#pragma once

namespace flecs 
{

inline void world::init_builtin_components() {
    component<Component>("flecs::core::Component");
    component<Identifier>("flecs::core::Identifier");
    component<Poly>("flecs::core::Poly");

#   ifdef FLECS_SYSTEM
    _::system_init(*this);
#   endif
#   ifdef FLECS_TIMER
    _::timer_init(*this);
#   endif
#   ifdef FLECS_DOC
    doc::_::init(*this);
#   endif
#   ifdef FLECS_REST
    rest::_::init(*this);
#   endif
#   ifdef FLECS_META
    meta::_::init(*this);
#   endif
}

template <typename T>
inline flecs::entity world::use(const char *alias) const {
    entity_t e = _::cpp_type<T>::id(m_world);
    const char *name = alias;
    if (!name) {
        // If no name is defined, use the entity name without the scope
        name = ecs_get_name(m_world, e);
    }
    ecs_set_alias(m_world, e, name);
    return flecs::entity(m_world, e);
}

inline flecs::entity world::use(const char *name, const char *alias) const {
    entity_t e = ecs_lookup_path_w_sep(m_world, 0, name, "::", "::", true);
    ecs_assert(e != 0, ECS_INVALID_PARAMETER, NULL);

    ecs_set_alias(m_world, e, alias);
    return flecs::entity(m_world, e);
}

inline void world::use(flecs::entity e, const char *alias) const {
    entity_t eid = e.id();
    const char *name = alias;
    if (!name) {
        // If no name is defined, use the entity name without the scope
        ecs_get_name(m_world, eid);
    }
    ecs_set_alias(m_world, eid, alias);
}

inline flecs::entity world::set_scope(const flecs::entity_t s) const {
    return flecs::entity(ecs_set_scope(m_world, s));
}

inline flecs::entity world::get_scope() const {
    return flecs::entity(m_world, ecs_get_scope(m_world));
}

template <typename T>
inline flecs::entity world::set_scope() const {
    return set_scope( _::cpp_type<T>::id(m_world) ); 
}

inline entity world::lookup(const char *name) const {
    auto e = ecs_lookup_path_w_sep(m_world, 0, name, "::", "::", true);
    return flecs::entity(*this, e);
}

template <typename T>
T* world::get_mut() const {
    flecs::entity e(m_world, _::cpp_type<T>::id(m_world));
    return e.get_mut<T>();
}

template <typename T>
void world::modified() const {
    flecs::entity e(m_world, _::cpp_type<T>::id(m_world));
    return e.modified<T>();
}

template <typename T>
ref<T> world::get_ref() const {
    flecs::entity e(m_world, _::cpp_type<T>::id(m_world));
    return e.get_ref<T>();
}

template <typename T>
const T* world::get() const {
    flecs::entity e(m_world, _::cpp_type<T>::id(m_world));
    return e.get<T>();
}

template <typename T>
bool world::has() const {
    flecs::entity e(m_world, _::cpp_type<T>::id(m_world));
    return e.has<T>();
}

template <typename T>
void world::add() const {
    flecs::entity e(m_world, _::cpp_type<T>::id(m_world));
    e.add<T>();
}

template <typename T>
void world::remove() const {
    flecs::entity e(m_world, _::cpp_type<T>::id(m_world));
    e.remove<T>();
}

template <typename T>
inline flecs::entity world::singleton() const {
    return flecs::entity(m_world, _::cpp_type<T>::id(m_world));
}

template <typename Func, if_t< is_callable<Func>::value > >
void world::get(const Func& func) const {
    static_assert(arity<Func>::value == 1, "singleton component must be the only argument");
    _::entity_with_invoker<Func>::invoke_get(
        this->m_world, this->singleton<first_arg_t<Func>>(), func);
}

template <typename Func, if_t< is_callable<Func>::value > >
void world::set(const Func& func) const {
    static_assert(arity<Func>::value == 1, "singleton component must be the only argument");
    _::entity_with_invoker<Func>::invoke_get_mut(
        this->m_world, this->singleton<first_arg_t<Func>>(), func);
}

inline flecs::entity world::get_alive(flecs::entity_t e) const {
    e = ecs_get_alive(m_world, e);
    return flecs::entity(m_world, e);
}
/* Prevent clashing with Unreal define. Unreal applications will have to use
 *  ecs_ensure. */
#ifndef ensure
inline flecs::entity world::ensure(flecs::entity_t e) const {
    ecs_ensure(m_world, e);
    return flecs::entity(m_world, e);
}
#endif

template <typename E>
inline flecs::entity enum_data<E>::entity() const {
    return flecs::entity(world_, impl_.id);
}

template <typename E>
inline flecs::entity enum_data<E>::entity(int value) const {
    return flecs::entity(world_, impl_.constants[value].id);
}

template <typename E>
inline flecs::entity enum_data<E>::entity(E value) const {
    return flecs::entity(world_, impl_.constants[static_cast<int>(value)].id);
}

/** Use provided scope for operations ran on returned world.
 * Operations need to be ran in a single statement.
 */
inline flecs::scoped_world world::scope(id_t parent) const {
    return scoped_world(m_world, parent);
}

template <typename T>
inline flecs::scoped_world world::scope() const {
    flecs::id_t parent = _::cpp_type<T>::id(m_world);
    return scoped_world(m_world, parent);
}

} // namespace flecs
