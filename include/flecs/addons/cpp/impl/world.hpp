/**
 * @file addons/cpp/impl/world.hpp
 * @brief World implementation.
 */

#pragma once

namespace flecs
{

/** Initialize built-in components. */
inline void world::init_builtin_components() {
    this->component<Component>();
    this->component<Identifier>();
    this->component<Poly>();
    this->component<Parent>();

    /* If meta is not defined and we're using enum reflection, make sure that
     * primitive types are registered. This ensures we can set components of
     * underlying_type_t<E> when registering constants. */
#   if !defined(FLECS_META) && !defined(FLECS_CPP_NO_ENUM_REFLECTION)
    this->component<uint8_t>("flecs::meta::u8");
    this->component<uint16_t>("flecs::meta::u16");
    this->component<uint32_t>("flecs::meta::u32");
    this->component<uint64_t>("flecs::meta::u64");
    this->component<int8_t>("flecs::meta::i8");
    this->component<int16_t>("flecs::meta::i16");
    this->component<int32_t>("flecs::meta::i32");
    this->component<int64_t>("flecs::meta::i64");
#   endif

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
#   ifdef FLECS_SCRIPT
    script::_::init(*this);
#   endif
}

/** Import a type (entity/component) as an alias. */
template <typename T>
inline flecs::entity world::use(const char *alias) const {
    entity_t e = _::type<T>::id(world_);
    const char *name = alias;
    if (!name) {
        // If no name is defined, use the entity name without the scope.
        name = ecs_get_name(world_, e);
    }
    ecs_set_alias(world_, e, name);
    return flecs::entity(world_, e);
}

/** Import an entity by name as an alias. */
inline flecs::entity world::use(const char *name, const char *alias) const {
    entity_t e = ecs_lookup_path_w_sep(world_, 0, name, "::", "::", true);
    ecs_assert(e != 0, ECS_INVALID_PARAMETER, NULL);

    ecs_set_alias(world_, e, alias);
    return flecs::entity(world_, e);
}

/** Import an entity as an alias. */
inline void world::use(flecs::entity e, const char *alias) const {
    entity_t eid = e.id();
    const char *name = alias;
    if (!name) {
        // If no name is defined, use the entity name without the scope.
        name = ecs_get_name(world_, eid);
    }
    ecs_set_alias(world_, eid, name);
}

/** Set the current scope. */
inline flecs::entity world::set_scope(const flecs::entity_t s) const {
    return flecs::entity(ecs_set_scope(world_, s));
}

/** Get the current scope. */
inline flecs::entity world::get_scope() const {
    return flecs::entity(world_, ecs_get_scope(world_));
}

/** Set the current scope to a type. */
template <typename T>
inline flecs::entity world::set_scope() const {
    return set_scope( _::type<T>::id(world_) );
}

/** Look up an entity by name. */
inline entity world::lookup(const char *name, const char *sep, const char *root_sep, bool recursive) const {
    auto e = ecs_lookup_path_w_sep(world_, 0, name, sep, root_sep, recursive);
    return flecs::entity(*this, e);
}

#ifndef ensure
/** Ensure a singleton component exists and return a mutable reference. */
template <typename T>
inline T& world::ensure() const {
    flecs::entity e(world_, _::type<T>::id(world_));
    return e.ensure<T>();
}
#endif

/** Mark a singleton component as modified. */
template <typename T>
inline void world::modified() const {
    flecs::entity e(world_, _::type<T>::id(world_));
    e.modified<T>();
}

/** Set a pair component value on a singleton. */
template <typename First, typename Second>
inline void world::set(Second second, const First& value) const {
    flecs::entity e(world_, _::type<First>::id(world_));
    e.set<First>(second, value);
}

/** Set a pair component value on a singleton (move). */
template <typename First, typename Second>
inline void world::set(Second second, First&& value) const {
    flecs::entity e(world_, _::type<First>::id(world_));
    e.set<First>(second, value);
}

/** Get a ref for a singleton component. */
template <typename T>
inline ref<T> world::get_ref() const {
    flecs::entity e(world_, _::type<T>::id(world_));
    return e.get_ref<T>();
}

/** Try to get a singleton value by component ID (returns nullptr if not found). */
inline const void* world::try_get(flecs::id_t id) const {
    flecs::entity e(world_, id);
    return e.try_get(id);
}

/** Try to get a singleton pair value by first and second IDs (returns nullptr if not found). */
inline const void* world::try_get(flecs::entity_t r, flecs::entity_t t) const {
    flecs::entity e(world_, r);
    return e.try_get(r, t);
}

/** Try to get a singleton component (returns nullptr if not found). */
template <typename T>
inline const T* world::try_get() const {
    flecs::entity e(world_, _::type<T>::id(world_));
    return e.try_get<T>();
}

/** Try to get a singleton pair component (returns nullptr if not found). */
template <typename First, typename Second, typename P, typename A>
inline const A* world::try_get() const {
    flecs::entity e(world_, _::type<First>::id(world_));
    return e.try_get<First, Second>();
}

/** Try to get a singleton pair component by second entity (returns nullptr if not found). */
template <typename First, typename Second>
inline const First* world::try_get(Second second) const {
    flecs::entity e(world_, _::type<First>::id(world_));
    return e.try_get<First>(second);
}

/** Get a singleton component value by component ID. */
inline const void* world::get(flecs::id_t id) const {
    flecs::entity e(world_, id);
    return e.get(id);
}

/** Get a singleton pair component value by first and second IDs. */
inline const void* world::get(flecs::entity_t r, flecs::entity_t t) const {
    flecs::entity e(world_, r);
    return e.get(r, t);
}

/** Get a singleton component. */
template <typename T>
inline const T& world::get() const {
    flecs::entity e(world_, _::type<T>::id(world_));
    return e.get<T>();
}

/** Get a singleton pair component. */
template <typename First, typename Second, typename P, typename A>
inline const A& world::get() const {
    flecs::entity e(world_, _::type<First>::id(world_));
    return e.get<First, Second>();
}

/** Get a singleton pair component by second entity. */
template <typename First, typename Second>
const First& world::get(Second second) const {
    flecs::entity e(world_, _::type<First>::id(world_));
    return e.get<First>(second);
}

/** Try to get a mutable singleton component by ID (returns nullptr if not found). */
inline void* world::try_get_mut(flecs::id_t id) const {
    flecs::entity e(world_, id);
    return e.try_get_mut(id);
}

/** Try to get a mutable singleton pair component by first and second IDs (returns nullptr if not found). */
inline void* world::try_get_mut(flecs::entity_t r, flecs::entity_t t) const {
    flecs::entity e(world_, r);
    return e.try_get_mut(r, t);
}

/** Try to get a mutable singleton component (returns nullptr if not found). */
template <typename T>
inline T* world::try_get_mut() const {
    flecs::entity e(world_, _::type<T>::id(world_));
    return e.try_get_mut<T>();
}

/** Try to get a mutable singleton pair component (returns nullptr if not found). */
template <typename First, typename Second, typename P, typename A>
inline A* world::try_get_mut() const {
    flecs::entity e(world_, _::type<First>::id(world_));
    return e.try_get_mut<First, Second>();
}

/** Try to get a mutable singleton pair component by second entity (returns nullptr if not found). */
template <typename First, typename Second>
inline First* world::try_get_mut(Second second) const {
    flecs::entity e(world_, _::type<First>::id(world_));
    return e.try_get_mut<First>(second);
}

/** Get a mutable singleton component by ID. */
inline void* world::get_mut(flecs::id_t id) const {
    flecs::entity e(world_, id);
    return e.get_mut(id);
}

/** Get a mutable singleton pair component by first and second IDs. */
inline void* world::get_mut(flecs::entity_t r, flecs::entity_t t) const {
    flecs::entity e(world_, r);
    return e.get_mut(r, t);
}

/** Get a mutable singleton component. */
template <typename T>
inline T& world::get_mut() const {
    flecs::entity e(world_, _::type<T>::id(world_));
    return e.get_mut<T>();
}

/** Get a mutable singleton pair component. */
template <typename First, typename Second, typename P, typename A>
inline A& world::get_mut() const {
    flecs::entity e(world_, _::type<First>::id(world_));
    return e.get_mut<First, Second>();
}

/** Get a mutable singleton pair component by second entity. */
template <typename First, typename Second>
inline First& world::get_mut(Second second) const {
    flecs::entity e(world_, _::type<First>::id(world_));
    return e.get_mut<First>(second);
}

/** Check for singleton component. */
template <typename T>
inline bool world::has() const {
    flecs::entity e(world_, _::type<T>::id(world_));
    return e.has<T>();
}

/** Check for singleton pair component. */
template <typename First, typename Second>
inline bool world::has() const {
    flecs::entity e(world_, _::type<First>::id(world_));
    return e.has<First, Second>();
}

/** Check for singleton pair component. */
template <typename First>
inline bool world::has(flecs::id_t second) const {
    flecs::entity e(world_, _::type<First>::id(world_));
    return e.has<First>(second);
}

/** Check for singleton pair by entity IDs. */
inline bool world::has(flecs::id_t first, flecs::id_t second) const {
    flecs::entity e(world_, first);
    return e.has(first, second);
}

/** Check for singleton enum constant. */
template <typename E, if_t< is_enum<E>::value > >
inline bool world::has(E value) const {
    flecs::entity e(world_, _::type<E>::id(world_));
    return e.has(value);
}

/** Add a singleton component. */
template <typename T>
inline void world::add() const {
    flecs::entity e(world_, _::type<T>::id(world_));
    e.add<T>();
}

/** Add a singleton pair component. */
template <typename First, typename Second>
inline void world::add() const {
    flecs::entity e(world_, _::type<First>::id(world_));
    e.add<First, Second>();
}

/** Add a singleton pair component. */
template <typename First>
inline void world::add(flecs::entity_t second) const {
    flecs::entity e(world_, _::type<First>::id(world_));
    e.add<First>(second);
}

/** Add a singleton pair by entity IDs. */
inline void world::add(flecs::entity_t first, flecs::entity_t second) const {
    flecs::entity e(world_, first);
    e.add(first, second);
}

/** Add a singleton enum constant value. */
template <typename E, if_t< is_enum<E>::value > >
inline void world::add(E value) const {
    flecs::entity e(world_, _::type<E>::id(world_));
    e.add(value);
}

/** Remove a singleton component. */
template <typename T>
inline void world::remove() const {
    flecs::entity e(world_, _::type<T>::id(world_));
    e.remove<T>();
}

/** Remove a singleton pair component. */
template <typename First, typename Second>
inline void world::remove() const {
    flecs::entity e(world_, _::type<First>::id(world_));
    e.remove<First, Second>();
}

/** Remove a singleton pair component by second entity. */
template <typename First>
inline void world::remove(flecs::entity_t second) const {
    flecs::entity e(world_, _::type<First>::id(world_));
    e.remove<First>(second);
}

/** Remove a singleton pair by entity IDs. */
inline void world::remove(flecs::entity_t first, flecs::entity_t second) const {
    flecs::entity e(world_, first);
    e.remove(first, second);
}

/** Iterate over children of the root entity. */
template <typename Func>
inline void world::children(Func&& f) const {
    this->entity(0).children(FLECS_FWD(f));
}

/** Get the singleton entity for a component type. */
template <typename T>
inline flecs::entity world::singleton() const {
    return flecs::entity(world_, _::type<T>::id(world_));
}

/** Get the target entity for a relationship on a singleton. */
template <typename First>
inline flecs::entity world::target(int32_t index) const
{
    return flecs::entity(world_,
        ecs_get_target(world_, _::type<First>::id(world_), _::type<First>::id(world_), index));
}

/** Get the target entity for a relationship on a component entity. */
template <typename T>
inline flecs::entity world::target(
    flecs::entity_t relationship,
    int32_t index) const
{
    return flecs::entity(world_,
        ecs_get_target(world_, _::type<T>::id(world_), relationship, index));
}

/** Get the target entity for a relationship. */
inline flecs::entity world::target(
    flecs::entity_t relationship,
    int32_t index) const
{
    return flecs::entity(world_,
        ecs_get_target(world_, relationship, relationship, index));
}

/** Get a singleton component using a callback. */
template <typename Func, if_t< is_callable<Func>::value > >
inline void world::get(const Func& func) const {
    static_assert(arity<Func>::value == 1, "singleton component must be the only argument");
    _::entity_with_delegate<Func>::invoke_get(
        this->world_, this->singleton<first_arg_t<Func>>(), func);
}

/** Set a singleton component using a callback. */
template <typename Func, if_t< is_callable<Func>::value > >
inline void world::set(const Func& func) const {
    static_assert(arity<Func>::value == 1, "singleton component must be the only argument");
    _::entity_with_delegate<Func>::invoke_ensure(
        this->world_, this->singleton<first_arg_t<Func>>(), func);
}

/** Get an alive entity from an ID. */
inline flecs::entity world::get_alive(flecs::entity_t e) const {
    e = ecs_get_alive(world_, e);
    return flecs::entity(world_, e);
}

/** Ensure an entity ID is alive. */
inline flecs::entity world::make_alive(flecs::entity_t e) const {
    ecs_make_alive(world_, e);
    return flecs::entity(world_, e);
}

/** Get the entity for an enum type. */
template <typename E>
inline flecs::entity enum_data<E>::entity() const {
    return flecs::entity(world_, _::type<E>::id(world_));
}

/** Get the entity for an enum constant by underlying_type value. */
template <typename E>
inline flecs::entity enum_data<E>::entity(underlying_type_t<E> value) const {
    int index = index_by_value(value);
    if (index >= 0) {
        int32_t constant_i = impl_.constants[index].index;
        flecs::entity_t entity = flecs_component_ids_get(world_, constant_i);
        return flecs::entity(world_, entity);
    }
#ifdef FLECS_META
    // Reflection data lookup failed. Try a value lookup among flecs::Constant relationships.
    flecs::world world = flecs::world(world_);
    return world.query_builder()
        .with(flecs::ChildOf, world.id<E>())
        .with(flecs::Constant, world.id<int32_t>())
        .build()
        .find([value](flecs::entity constant) {
            const int32_t& constant_value = constant.get_second<int32_t>(flecs::Constant);
            return value == static_cast<underlying_type_t<E>>(constant_value);
        });
#else
    return flecs::entity::null(world_);
#endif
}

/** Get the entity for an enum constant. */
template <typename E>
inline flecs::entity enum_data<E>::entity(E value) const {
    return entity(static_cast<underlying_type_t<E>>(value));
}

/** Use the provided scope for operations run on the returned world.
 * Operations need to be run in a single statement.
 */
inline flecs::scoped_world world::scope(id_t parent) const {
    return scoped_world(world_, parent);
}

/** Use the provided scope (by type) for operations run on the returned world. */
template <typename T>
inline flecs::scoped_world world::scope() const {
    flecs::id_t parent = _::type<T>::id(world_);
    return scoped_world(world_, parent);
}

/** Use the provided scope (by name) for operations run on the returned world. */
inline flecs::scoped_world world::scope(const char* name) const {
  return scope(entity(name));
}

} // namespace flecs
