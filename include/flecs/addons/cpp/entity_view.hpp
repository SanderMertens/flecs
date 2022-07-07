#pragma once

namespace flecs
{

/** Entity view class
 * This class provides readonly access to entities. Using this class to store 
 * entities in components ensures valid handles, as this class will always store
 * the actual world vs. a stage. The constructors of this class will never 
 * create a new entity.
 *
 * To obtain a mutable handle to the entity, use the "mut" function.
 */
struct entity_view : public id {

    entity_view() : flecs::id() { }

    /** Wrap an existing entity id.
     *
     * @param world The world in which the entity is created.
     * @param id The entity id.
     */
    explicit entity_view(flecs::world_t *world, flecs::id_t id)
        : flecs::id(world 
            ? const_cast<flecs::world_t*>(ecs_get_world(world))
            : nullptr
        , id ) { }

    /** Implicit conversion from flecs::entity_t to flecs::entity_view. */
    entity_view(entity_t id) 
        : flecs::id( nullptr, id ) { }

    /** Get entity id.
     * @return The integer entity id.
     */
    entity_t id() const {
        return m_id;
    }

    /** Check is entity is valid.
     *
     * @return True if the entity is alive, false otherwise.
     */
    bool is_valid() const {
        return m_world && ecs_is_valid(m_world, m_id);
    }
  
    explicit operator bool() const {
        return is_valid();
    }

    /** Check is entity is alive.
     *
     * @return True if the entity is alive, false otherwise.
     */
    bool is_alive() const {
        return m_world && ecs_is_alive(m_world, m_id);
    }

    /** Return the entity name.
     *
     * @return The entity name.
     */
    flecs::string_view name() const {
        return flecs::string_view(ecs_get_name(m_world, m_id));
    }

    /** Return the entity symbol.
     *
     * @return The entity symbol.
     */
    flecs::string_view symbol() const {
        return flecs::string_view(ecs_get_symbol(m_world, m_id));
    }

    /** Return the entity path.
     *
     * @return The hierarchical entity path.
     */
    flecs::string path(const char *sep = "::", const char *init_sep = "::") const {
        char *path = ecs_get_path_w_sep(m_world, 0, m_id, sep, init_sep);
        return flecs::string(path);
    }   

    bool enabled() {
        return !ecs_has_id(m_world, m_id, flecs::Disabled);
    }

    /** Get the entity's type.
     *
     * @return The entity's type.
     */
    flecs::type type() const;

    /** Get the entity's table.
     *
     * @return Returns the entity's table.
     */
    flecs::table table() const;

    /** Iterate (component) ids of an entity.
     * The function parameter must match the following signature:
     *   void(*)(flecs::id id)
     *
     * @param func The function invoked for each id.
     */
    template <typename Func>
    void each(const Func& func) const;

    /** Iterate matching relation ids of an entity.
     * The function parameter must match the following signature:
     *   void(*)(flecs::id id)
     *
     * @param func The function invoked for each id.
     */
    template <typename Func>
    void each(flecs::id_t rel, flecs::id_t obj, const Func& func) const;

    /** Iterate objects for a given relationship.
     * The function parameter must match the following signature:
     *   void(*)(flecs::entity object)
     *
     * @param rel The relationship for which to iterate the objects.
     * @param func The function invoked for each object.
     */
    template <typename Func>
    void each(const flecs::entity_view& rel, const Func& func) const;

    /** Iterate objects for a given relationship.
     * The function parameter must match the following signature:
     *   void(*)(flecs::entity object)
     *
     * @tparam Rel The relationship for which to iterate the objects.
     * @param func The function invoked for each object.     
     */
    template <typename Rel, typename Func>
    void each(const Func& func) const { 
        return each(_::cpp_type<Rel>::id(m_world), func);
    }

    /** Iterate children for entity.
     * The function parameter must match the following signature:
     *   void(*)(flecs::entity object)
     *
     * @param func The function invoked for each child.     
     */
    template <typename Func>
    void children(Func&& func) const {
        flecs::world world(m_world);

        ecs_term_t terms[2];
        ecs_filter_t f = ECS_FILTER_INIT;
        f.terms = terms;
        f.term_count = 2;

        ecs_filter_desc_t desc = {};
        desc.terms[0].id = ecs_pair(flecs::ChildOf, m_id);
        desc.terms[1].id = flecs::Prefab;
        desc.terms[1].oper = EcsOptional;
        desc.storage = &f;
        ecs_filter_init(m_world, &desc);

        ecs_iter_t it = ecs_filter_iter(m_world, &f);
        while (ecs_filter_next(&it)) {
            _::each_invoker<Func>(FLECS_MOV(func)).invoke(&it);
        }

        ecs_filter_fini(&f);
    }

    /** Get component value.
     * 
     * @tparam T The component to get.
     * @return Pointer to the component value, nullptr if the entity does not
     *         have the component.
     */
    template <typename T, if_t< is_actual<T>::value > = 0>
    const T* get() const {
        auto comp_id = _::cpp_type<T>::id(m_world);
        ecs_assert(_::cpp_type<T>::size() != 0, ECS_INVALID_PARAMETER, NULL);
        return static_cast<const T*>(ecs_get_id(m_world, m_id, comp_id));
    }

    /** Get component value.
     * Overload for when T is not the same as the actual type, which happens
     * when using pair types.
     * 
     * @tparam T The component to get.
     * @return Pointer to the component value, nullptr if the entity does not
     *         have the component.
     */
    template <typename T, typename A = actual_type_t<T>, 
        if_t< flecs::is_pair<T>::value > = 0>
    const A* get() const {
        auto comp_id = _::cpp_type<T>::id(m_world);
        ecs_assert(_::cpp_type<A>::size() != 0, ECS_INVALID_PARAMETER, NULL);
        return static_cast<const A*>(ecs_get_id(m_world, m_id, comp_id));
    }

    /** Get a pair.
     * This operation gets the value for a pair from the entity.
     *
     * @tparam R the relation type.
     * @tparam O the object type.
     */
    template <typename R, typename O, typename P = pair<R, O>, 
        typename A = actual_type_t<P>, if_not_t< flecs::is_pair<R>::value > = 0>
    const A* get() const {
        return this->get<P>();
    }

    /** Get a pair.
     * This operation gets the value for a pair from the entity. 
     *
     * @tparam R the relation type.
     * @param object the object.
     */
    template<typename R, typename O, if_not_t< is_enum<O>::value> = 0>
    const R* get(O object) const {
        auto comp_id = _::cpp_type<R>::id(m_world);
        ecs_assert(_::cpp_type<R>::size() != 0, ECS_INVALID_PARAMETER, NULL);
        return static_cast<const R*>(
            ecs_get_id(m_world, m_id, ecs_pair(comp_id, object)));
    }

    /** Get a pair.
     * This operation gets the value for a pair from the entity. 
     *
     * @tparam R the relation type.
     * @param constant the enum constant.
     */
    template<typename R, typename O, if_t<is_enum<O>::value> = 0>
    const R* get(O constant) const {
        const auto& et = enum_type<O>(this->m_world);
        flecs::entity_t object = et.entity(constant);
        return get<R>(object);
    }

    /** Get component value (untyped).
     * 
     * @param comp The component to get.
     * @return Pointer to the component value, nullptr if the entity does not
     *         have the component.
     */
    const void* get(flecs::id_t comp) const {
        return ecs_get_id(m_world, m_id, comp);
    }

    /** Get a pair (untyped).
     * This operation gets the value for a pair from the entity. If neither the
     * relation nor the object part of the pair are components, the operation 
     * will fail.
     *
     * @param relation the relation.
     * @param object the object.
     */
    const void* get(flecs::entity_t relation, flecs::entity_t object) const {
        return ecs_get_id(m_world, m_id, ecs_pair(relation, object));
    }

    /** Get 1..N components.
     * This operation accepts a callback with as arguments the components to
     * retrieve. The callback will only be invoked when the entity has all
     * the components.
     *
     * This operation is faster than individually calling get for each component
     * as it only obtains entity metadata once.
     * 
     * While the callback is invoked the table in which the components are
     * stored is locked, which prevents mutations that could cause invalidation
     * of the component references. Note that this is not an actual lock: 
     * invalid access causes a runtime panic and so it is still up to the 
     * application to ensure access is protected.
     * 
     * The component arguments must be references and can be either const or
     * non-const. When all arguments are const, the function will read-lock the
     * table (see ecs_read_begin). If one or more arguments are non-const the
     * function will write-lock the table (see ecs_write_begin).
     * 
     * Example:
     *   e.get([](Position& p, Velocity& v) { // write lock
     *     p.x += v.x;
     *   });
     * 
     *   e.get([](const Position& p) {        // read lock
     *     std::cout << p.x << std::endl;
     *   });
     *
     * @param func The callback to invoke.
     * @return True if the entity has all components, false if not.
     */
    template <typename Func, if_t< is_callable<Func>::value > = 0>
    bool get(const Func& func) const;

    /** Get enum constant.
     * 
     * @tparam T The enum type for which to get the constant
     * @return Constant entity if found, 0 entity if not.
     */
    template <typename T, if_t< is_enum<T>::value > = 0>
    const T* get() const;

    /** Get the object part from a pair.
     * This operation gets the value for a pair from the entity. The relation
     * part of the pair should not be a component.
     *
     * @tparam O the object type.
     * @param relation the relation.
     */
    template<typename O>
    const O* get_w_object(flecs::entity_t relation) const {
        auto comp_id = _::cpp_type<O>::id(m_world);
        ecs_assert(_::cpp_type<O>::size() != 0, ECS_INVALID_PARAMETER, NULL);
        return static_cast<const O*>(
            ecs_get_id(m_world, m_id, ecs_pair(relation, comp_id)));
    }

    /** Get the object part from a pair.
     * This operation gets the value for a pair from the entity. The relation
     * part of the pair should not be a component.
     *
     * @tparam R the relation type.
     * @tparam O the object type.
     */
    template<typename R, typename O>
    const O* get_w_object() const {
        return get<pair_object<R, O>>();
    }

    /** Get object for a given relation.
     * This operation returns the object for a given relation. The optional
     * index can be used to iterate through objects, in case the entity has
     * multiple instances for the same relation.
     *
     * @tparam R The relation for which to retrieve the object.
     * @param index The index (0 for the first instance of the relation).
     */
    template<typename R>
    flecs::entity get_object(int32_t index = 0) const;

    /** Get object for a given relation.
     * This operation returns the object for a given relation. The optional
     * index can be used to iterate through objects, in case the entity has
     * multiple instances for the same relation.
     *
     * @param relation The relation for which to retrieve the object.
     * @param index The index (0 for the first instance of the relation).
     */
    flecs::entity get_object(flecs::entity_t relation, int32_t index = 0) const;

    /** Get the object of a relation for a given id.
     * This operation returns the first entity that has the provided id by following
     * the specified relationship. If the entity itself has the id then entity will
     * be returned. If the id cannot be found on the entity or by following the
     * relation, the operation will return 0.
     * 
     * This operation can be used to lookup, for example, which prefab is providing
     * a component by specifying the IsA relation:
     * 
     *   // Is Position provided by the entity or one of its base entities?
     *   ecs_get_object_for_id(world, entity, EcsIsA, ecs_id(Position))
     * 
     * @param relation The relationship to follow.
     * @param id The id to lookup.
     * @return The entity for which the object has been found.
     */
    flecs::entity get_object_for(flecs::entity_t relation, flecs::id_t id) const;

    template <typename T>
    flecs::entity get_object_for(flecs::entity_t relation) const;

    template <typename R, typename O>
    flecs::entity get_object_for(flecs::entity_t relation) const;
    
    /** Lookup an entity by name.
     * Lookup an entity in the scope of this entity. The provided path may
     * contain double colons as scope separators, for example: "Foo::Bar".
     *
     * @param path The name of the entity to lookup.
     * @return The found entity, or entity::null if no entity matched.
     */
    flecs::entity lookup(const char *path) const;

    /** Check if entity has the provided entity.
     *
     * @param e The entity to check.
     * @return True if the entity has the provided entity, false otherwise.
     */
    bool has(flecs::id_t e) const {
        return ecs_has_id(m_world, m_id, e);
    }     

    /** Check if entity has the provided component.
     *
     * @tparam T The component to check.
     * @return True if the entity has the provided component, false otherwise.
     */
    template <typename T>
    bool has() const {
        flecs::id_t cid = _::cpp_type<T>::id(m_world);
        bool result = ecs_has_id(m_world, m_id, cid);
        if (result) {
            return result;
        }

        if (is_enum<T>::value) {
            return ecs_has_pair(m_world, m_id, cid, flecs::Wildcard);
        }

        return false;
    }

    /** Check if entity has the provided enum constant.
     *
     * @tparam E The enum type (can be deduced).
     * @param value The enum constant to check. 
     * @return True if the entity has the provided constant, false otherwise.
     */
    template <typename E, if_t< is_enum<E>::value > = 0>
    bool has(E value) const {
        auto r = _::cpp_type<E>::id(m_world);
        auto o = enum_type<E>(m_world).entity(value);
        return ecs_has_pair(m_world, m_id, r, o);
    }

    /** Check if entity has the provided pair.
     *
     * @tparam Relation The relation type.
     * @tparam Object The object type.
     * @return True if the entity has the provided component, false otherwise.
     */
    template <typename Relation, typename Object>
    bool has() const {
        return this->has<Relation>(_::cpp_type<Object>::id(m_world));
    }

    /** Check if entity has the provided pair.
     *
     * @tparam R The relation type.
     * @param object The object.
     * @return True if the entity has the provided component, false otherwise.
     */
    template<typename R, typename O, if_not_t< is_enum<O>::value > = 0>
    bool has(O object) const {
        auto comp_id = _::cpp_type<R>::id(m_world);
        return ecs_has_id(m_world, m_id, ecs_pair(comp_id, object));
    }

    /** Check if entity has the provided pair.
     *
     * @tparam R The relation type.
     * @param value The enum constant.
     * @return True if the entity has the provided component, false otherwise.
     */
    template<typename R, typename E, if_t< is_enum<E>::value > = 0>
    bool has(E value) const {
        const auto& et = enum_type<E>(this->m_world);
        flecs::entity_t object = et.entity(value);
        return has<R>(object);
    }

    /** Check if entity has the provided pair.
     *
     * @param relation The relation.
     * @param object The object.
     * @return True if the entity has the provided component, false otherwise.
     */
    bool has(flecs::id_t relation, flecs::id_t object) const {
        return ecs_has_id(m_world, m_id, ecs_pair(relation, object));
    }

    /** Check if entity has the provided pair.
     *
     * @tparam Object The object type.
     * @param relation The relation.
     * @return True if the entity has the provided component, false otherwise.
     */
    template <typename Object>
    bool has_w_object(flecs::id_t relation) const {
        auto comp_id = _::cpp_type<Object>::id(m_world);
        return ecs_has_id(m_world, m_id, ecs_pair(relation, comp_id));
    }

    /** Check if entity owns the provided entity.
     * An entity is owned if it is not shared from a base entity.
     *
     * @param e The entity to check.
     * @return True if the entity owns the provided entity, false otherwise.
     */
    bool owns(flecs::id_t e) const {
        return ecs_owns_id(m_world, m_id, e);
    }

    /** Check if entity owns the provided pair.
     *
     * @tparam Relation The relation type.
     * @param object The object.
     * @return True if the entity owns the provided component, false otherwise.
     */
    template <typename Relation>
    bool owns(flecs::id_t object) const {
        auto comp_id = _::cpp_type<Relation>::id(m_world);
        return owns(ecs_pair(comp_id, object));
    }

    /** Check if entity owns the provided pair.
     *
     * @param relation The relation.
     * @param object The object.
     * @return True if the entity owns the provided component, false otherwise.
     */
    bool owns(flecs::id_t relation, flecs::id_t object) const {
        return owns(ecs_pair(relation, object));
    }

    /** Check if entity owns the provided component.
     * An component is owned if it is not shared from a base entity.
     *
     * @tparam T The component to check.
     * @return True if the entity owns the provided component, false otherwise.
     */
    template <typename T>
    bool owns() const {
        return owns(_::cpp_type<T>::id(m_world));
    }

    /** Test if component is enabled.
     *
     * @tparam T The component to test.
     * @return True if the component is enabled, false if it has been disabled.
     */
    template<typename T>
    bool is_enabled() {
        return ecs_is_component_enabled_w_id(
            m_world, m_id, _::cpp_type<T>::id(m_world));
    }

    /** Test if component is enabled.
     *
     * @param e The component to test.
     * @return True if the component is enabled, false if it has been disabled.
     */
    bool is_enabled(const flecs::entity_view& e) {
        return ecs_is_component_enabled_w_id(
            m_world, m_id, e);
    }

    /** Get current delta time.
     * Convenience function so system implementations can get delta_time, even
     * if they are using the .each() function.
     *
     * @return Current delta_time.
     */
    ecs_ftime_t delta_time() const {
        const ecs_world_info_t *stats = ecs_get_world_info(m_world);
        return stats->delta_time;
    }

    flecs::entity clone(bool clone_value = true, flecs::entity_t dst_id = 0) const;

    /** Return mutable entity handle for current stage 
     * When an entity handle created from the world is used while the world is
     * in staged mode, it will only allow for readonly operations since 
     * structural changes are not allowed on the world while in staged mode.
     * 
     * To do mutations on the entity, this operation provides a handle to the
     * entity that uses the stage instead of the actual world.
     *
     * Note that staged entity handles should never be stored persistently, in
     * components or elsewhere. An entity handle should always point to the
     * main world.
     *
     * Also note that this operation is not necessary when doing mutations on an
     * entity outside of a system. It is allowed to do entity operations 
     * directly on the world, as long as the world is not in staged mode.
     *
     * @param stage The current stage.
     * @return An entity handle that allows for mutations in the current stage.
     */
    flecs::entity mut(const flecs::world& stage) const;

    /** Same as mut(world), but for iterator.
     * This operation allows for the construction of a mutable entity handle
     * from an iterator.
     *
     * @param it An iterator that contains a reference to the world or stage.
     * @return An entity handle that allows for mutations in the current stage.
     */
    flecs::entity mut(const flecs::iter& it) const;

    /** Same as mut(world), but for entity.
     * This operation allows for the construction of a mutable entity handle
     * from another entity. This is useful in each() functions, which only 
     * provide a handle to the entity being iterated over.
     *
     * @param e Another mutable entity.
     * @return An entity handle that allows for mutations in the current stage.
     */
    flecs::entity mut(const flecs::entity_view& e) const;

#   ifdef FLECS_JSON
#   include "mixins/json/entity.inl"
#   endif
#   ifdef FLECS_DOC
#   include "mixins/doc/entity_view.inl"
#   endif

#   include "mixins/enum/entity_view.inl"

private:
    flecs::entity set_stage(world_t *stage);
};

}
