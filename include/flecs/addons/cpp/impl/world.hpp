#pragma once

namespace flecs 
{

// emplace for T(flecs::entity, Args...)
template <typename T, typename ... Args, if_t<
    std::is_constructible<actual_type_t<T>, flecs::entity, Args...>::value >>
inline void emplace(world_t *world, id_t entity, Args&&... args) {
    flecs::entity self(world, entity);
    emplace<T>(world, entity, self, std::forward<Args>(args)...);
}

inline void world::init_builtin_components() {
    component<Component>("flecs::core::Component");
    component<Type>("flecs::core::Type");
    component<Identifier>("flecs::core::Identifier");
    component<Trigger>("flecs::core::Trigger");
    component<Observer>("flecs::core::Observer");
    component<Query>("flecs::core::Query");

#   ifdef FLECS_DOC
    component<doc::Description>("flecs::doc::Description");
#   endif
#   ifdef FLECS_REST
    component<rest::Rest>("flecs::rest::Rest");
#   endif
#   ifdef FLECS_SYSTEM
    component<TickSource>("flecs::system::TickSource");
#   endif
#   ifdef FLECS_TIMER
    component<RateFilter>("flecs::timer::RateFilter");
    component<Timer>("flecs::timer::Timer");
#   endif
}

template <typename T>
inline flecs::entity world::use(const char *alias) {
    entity_t e = _::cpp_type<T>::id(m_world);
    const char *name = alias;
    if (!name) {
        // If no name is defined, use the entity name without the scope
        name = ecs_get_name(m_world, e);
    }
    ecs_use(m_world, e, name);
    return flecs::entity(m_world, e);
}

inline flecs::entity world::use(const char *name, const char *alias) {
    entity_t e = ecs_lookup_path_w_sep(m_world, 0, name, "::", "::", true);
    ecs_assert(e != 0, ECS_INVALID_PARAMETER, NULL);

    ecs_use(m_world, e, alias);
    return flecs::entity(m_world, e);
}

inline void world::use(flecs::entity e, const char *alias) {
    entity_t eid = e.id();
    const char *name = alias;
    if (!name) {
        // If no name is defined, use the entity name without the scope
        ecs_get_name(m_world, eid);
    }
    ecs_use(m_world, eid, alias);
}

inline flecs::entity world::set_scope(const flecs::entity& s) const {
    return flecs::entity(ecs_set_scope(m_world, s.id()));
}

inline flecs::entity world::get_scope() const {
    return flecs::entity(m_world, ecs_get_scope(m_world));
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
inline flecs::entity world::singleton() {
    return flecs::entity(m_world, _::cpp_type<T>::id(m_world));
}

template <typename Func, if_t< is_callable<Func>::value > >
void world::get(const Func& func) {
    static_assert(arity<Func>::value == 1, "singleton component must be the only argument");
    _::entity_with_invoker<Func>::invoke_get(
        this->m_world, this->singleton<first_arg_t<Func>>(), func);
}

template <typename Func, if_t< is_callable<Func>::value > >
void world::set(const Func& func) {
    static_assert(arity<Func>::value == 1, "singleton component must be the only argument");
    _::entity_with_invoker<Func>::invoke_get_mut(
        this->m_world, this->singleton<first_arg_t<Func>>(), func);
}

} // namespace flecs
