/**
 * @file addons/cpp/impl/world.hpp
 * @brief World implementation.
 */

#pragma once

namespace flecs 
{

inline void world::init_builtin_components() {
    this->component<Component>();
    this->component<Identifier>();
    this->component<Poly>();

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
    entity_t e = _::type<T>::id(world_);
    const char *name = alias;
    if (!name) {
        // If no name is defined, use the entity name without the scope
        name = ecs_get_name(world_, e);
    }
    ecs_set_alias(world_, e, name);
    return flecs::entity(world_, e);
}

inline flecs::entity world::use(const char *name, const char *alias) const {
    entity_t e = ecs_lookup_path_w_sep(world_, 0, name, "::", "::", true);
    ecs_assert(e != 0, ECS_INVALID_PARAMETER, NULL);

    ecs_set_alias(world_, e, alias);
    return flecs::entity(world_, e);
}

inline void world::use(flecs::entity e, const char *alias) const {
    entity_t eid = e.id();
    const char *name = alias;
    if (!name) {
        // If no name is defined, use the entity name without the scope
        name = ecs_get_name(world_, eid);
    }
    ecs_set_alias(world_, eid, name);
}

inline flecs::entity world::set_scope(const flecs::entity_t s) const {
    return flecs::entity(ecs_set_scope(world_, s));
}

inline flecs::entity world::get_scope() const {
    return flecs::entity(world_, ecs_get_scope(world_));
}

template <typename T>
inline flecs::entity world::set_scope() const {
    return set_scope( _::type<T>::id(world_) ); 
}

inline entity world::lookup(const char *name, const char *sep, const char *root_sep, bool recursive) const {
    auto e = ecs_lookup_path_w_sep(world_, 0, name, sep, root_sep, recursive);
    return flecs::entity(*this, e);
}

#ifndef ensure
template <typename T>
inline T& world::ensure() const {
    flecs::entity e(world_, _::type<T>::id(world_));
    return e.ensure<T>();
}
#endif

template <typename T>
inline void world::modified() const {
    flecs::entity e(world_, _::type<T>::id(world_));
    e.modified<T>();
}

template <typename First, typename Second>
inline void world::set(Second second, const First& value) const {
    flecs::entity e(world_, _::type<First>::id(world_));
    e.set<First>(second, value);
}

template <typename First, typename Second>
inline void world::set(Second second, First&& value) const {
    flecs::entity e(world_, _::type<First>::id(world_));
    e.set<First>(second, value);
}

template <typename T>
inline ref<T> world::get_ref() const {
    flecs::entity e(world_, _::type<T>::id(world_));
    return e.get_ref<T>();
}

template <typename T>
inline const T* world::get() const {
    flecs::entity e(world_, _::type<T>::id(world_));
    return e.get<T>();
}

template <typename First, typename Second, typename P, typename A>
const A* world::get() const {
    flecs::entity e(world_, _::type<First>::id(world_));
    return e.get<First, Second>();
}

template <typename First, typename Second>
const First* world::get(Second second) const {
    flecs::entity e(world_, _::type<First>::id(world_));
    return e.get<First>(second);
}

template <typename T>
T* world::get_mut() const {
    flecs::entity e(world_, _::type<T>::id(world_));
    return e.get_mut<T>();
}

template <typename First, typename Second, typename P, typename A>
A* world::get_mut() const {
    flecs::entity e(world_, _::type<First>::id(world_));
    return e.get_mut<First, Second>();
}

template <typename First, typename Second>
First* world::get_mut(Second second) const {
    flecs::entity e(world_, _::type<First>::id(world_));
    return e.get_mut<First>(second);
}

template <typename T>
inline bool world::has() const {
    flecs::entity e(world_, _::type<T>::id(world_));
    return e.has<T>();
}

template <typename First, typename Second>
inline bool world::has() const {
    flecs::entity e(world_, _::type<First>::id(world_));
    return e.has<First, Second>();
}

template <typename First>
inline bool world::has(flecs::id_t second) const {
    flecs::entity e(world_, _::type<First>::id(world_));
    return e.has<First>(second);
}

inline bool world::has(flecs::id_t first, flecs::id_t second) const {
    flecs::entity e(world_, first);
    return e.has(first, second);
}

template <typename T>
inline void world::add() const {
    flecs::entity e(world_, _::type<T>::id(world_));
    e.add<T>();
}

template <typename First, typename Second>
inline void world::add() const {
    flecs::entity e(world_, _::type<First>::id(world_));
    e.add<First, Second>();
}

template <typename First>
inline void world::add(flecs::entity_t second) const {
    flecs::entity e(world_, _::type<First>::id(world_));
    e.add<First>(second);
}

inline void world::add(flecs::entity_t first, flecs::entity_t second) const {
    flecs::entity e(world_, first);
    e.add(first, second);
}

template <typename T>
inline void world::remove() const {
    flecs::entity e(world_, _::type<T>::id(world_));
    e.remove<T>();
}

template <typename First, typename Second>
inline void world::remove() const {
    flecs::entity e(world_, _::type<First>::id(world_));
    e.remove<First, Second>();
}

template <typename First>
inline void world::remove(flecs::entity_t second) const {
    flecs::entity e(world_, _::type<First>::id(world_));
    e.remove<First>(second);
}

inline void world::remove(flecs::entity_t first, flecs::entity_t second) const {
    flecs::entity e(world_, first);
    e.remove(first, second);
}

template <typename Func>
inline void world::children(Func&& f) const {
    this->entity(0).children(FLECS_FWD(f));
}

template <typename T>
inline flecs::entity world::singleton() const {
    return flecs::entity(world_, _::type<T>::id(world_));
}

template <typename First>
inline flecs::entity world::target(int32_t index) const
{
    return flecs::entity(world_,
        ecs_get_target(world_, _::type<First>::id(world_), _::type<First>::id(world_), index));
}

template <typename T>
inline flecs::entity world::target(
    flecs::entity_t relationship,
    int32_t index) const
{
    return flecs::entity(world_,
        ecs_get_target(world_, _::type<T>::id(world_), relationship, index));
}

inline flecs::entity world::target(
    flecs::entity_t relationship,
    int32_t index) const
{
    return flecs::entity(world_,
        ecs_get_target(world_, relationship, relationship, index));
}

template <typename Func, if_t< is_callable<Func>::value > >
inline void world::get(const Func& func) const {
    static_assert(arity<Func>::value == 1, "singleton component must be the only argument");
    _::entity_with_delegate<Func>::invoke_get(
        this->world_, this->singleton<first_arg_t<Func>>(), func);
}

template <typename Func, if_t< is_callable<Func>::value > >
inline void world::set(const Func& func) const {
    static_assert(arity<Func>::value == 1, "singleton component must be the only argument");
    _::entity_with_delegate<Func>::invoke_ensure(
        this->world_, this->singleton<first_arg_t<Func>>(), func);
}

inline flecs::entity world::get_alive(flecs::entity_t e) const {
    e = ecs_get_alive(world_, e);
    return flecs::entity(world_, e);
}

inline flecs::entity world::make_alive(flecs::entity_t e) const {
    ecs_make_alive(world_, e);
    return flecs::entity(world_, e);
}

template <typename E>
inline flecs::entity enum_data<E>::entity() const {
    return flecs::entity(world_, impl_.id);
}

template <typename E>
inline flecs::entity enum_data<E>::entity(underlying_type_t<E> value) const {
    int index = index_by_value(value);
    if (index >= 0) {
        return flecs::entity(world_, impl_.constants[index].id);
    }
#ifdef FLECS_META
    // Reflection data lookup failed. Try value lookup amongst flecs::Constant relationships
    flecs::world world = flecs::world(world_);
    return world.query_builder()
        .with(flecs::ChildOf, world.id<E>())
        .with(flecs::Constant, world.id<int32_t>())
        .build()
        .find([value](flecs::entity constant) {
            const int32_t *constant_value = constant.get_second<int32_t>(flecs::Constant);
            ecs_assert(constant_value, ECS_INTERNAL_ERROR, NULL);
            return value == static_cast<underlying_type_t<E>>(*constant_value);
        });
#else
    return flecs::entity::null(world_);
#endif
}

template <typename E>
inline flecs::entity enum_data<E>::entity(E value) const {
    return entity(static_cast<underlying_type_t<E>>(value));
}

/** Use provided scope for operations ran on returned world.
 * Operations need to be ran in a single statement.
 */
inline flecs::scoped_world world::scope(id_t parent) const {
    return scoped_world(world_, parent);
}

template <typename T>
inline flecs::scoped_world world::scope() const {
    flecs::id_t parent = _::type<T>::id(world_);
    return scoped_world(world_, parent);
}

inline flecs::scoped_world world::scope(const char* name) const {
  return scope(entity(name));
}

} // namespace flecs
