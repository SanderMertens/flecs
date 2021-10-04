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
class entity_view : public id {
public:
    entity_view() : flecs::id() { }

    /** Wrap an existing entity id.
     *
     * @param world The world in which the entity is created.
     * @param id The entity id.
     */
    explicit entity_view(const flecs::world& world, const entity_view& id)
        : flecs::id( world.get_world(), id.id() ) { }

    /** Wrap an existing entity id.
     *
     * @param world Pointer to the world in which the entity is created.
     * @param id The entity id.
     */
    explicit entity_view(world_t *world, const entity_view& id) 
        : flecs::id( flecs::world(world).get_world(), id.id() ) { }

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
     * @return The entity name, or an empty string if the entity has no name.
     */
    flecs::string_view name() const {
        return flecs::string_view(ecs_get_name(m_world, m_id));
    }

    /** Return the entity path.
     *
     * @return The hierarchical entity path, or an empty string if the entity 
     *         has no name.
     */
    flecs::string path(const char *sep = "::", const char *init_sep = "::") const {
        char *path = ecs_get_path_w_sep(m_world, 0, m_id, sep, init_sep);
        return flecs::string(path);
    }   

    bool enabled() {
        return !ecs_has_id(m_world, m_id, flecs::Disabled);
    }

    /** Return the type.
     *
     * @return Returns the entity type.
     */
    flecs::type type() const;

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
        if_not_t< is_actual<T>::value > = 0>
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
    template<typename R>
    const R* get(flecs::id_t object) const {
        auto comp_id = _::cpp_type<R>::id(m_world);
        ecs_assert(_::cpp_type<R>::size() != 0, ECS_INVALID_PARAMETER, NULL);
        return static_cast<const R*>(
            ecs_get_id(m_world, m_id, ecs_pair(comp_id, object)));
    }

    /** Get component value (untyped).
     * 
     * @param component The component to get.
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
     * @param func The callback to invoke.
     * @return True if the entity has all components, false if not.
     */
    template <typename Func, if_t< is_callable<Func>::value > = 0>
    bool get(const Func& func) const;

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
     * @param entity The entity to check.
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
        return ecs_has_id(m_world, m_id, _::cpp_type<T>::id(m_world));
    }

    /** Check if entity has the provided pair.
     *
     * @tparam Relation The relation type.
     * @param Object The object type.
     * @return True if the entity has the provided component, false otherwise.
     */
    template <typename Relation, typename Object>
    bool has() const {
        return this->has<Relation>(_::cpp_type<Object>::id(m_world));
    }

    /** Check if entity has the provided pair.
     *
     * @tparam Relation The relation type.
     * @param object The object.
     * @return True if the entity has the provided component, false otherwise.
     */
    template <typename Relation>
    bool has(flecs::id_t object) const {
        auto comp_id = _::cpp_type<Relation>::id(m_world);
        return ecs_has_id(m_world, m_id, ecs_pair(comp_id, object));
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
     * @param entity The entity to check.
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

    /** Check if entity has the provided switch.
     *
     * @param sw The switch to check.
     * @return True if the entity has the provided switch, false otherwise.
     */
    bool has_switch(const flecs::type& sw) const;

    template <typename T>
    bool has_switch() const {
        return ecs_has_id(m_world, m_id, 
            flecs::Switch | _::cpp_type<T>::id(m_world));
    }

    /** Check if entity has the provided case.
     *
     * @param sw_case The case to check.
     * @return True if the entity has the provided case, false otherwise.
     */
    bool has_case(flecs::id_t sw_case) const {
        return ecs_has_id(m_world, m_id, flecs::Case | sw_case);
    }

    template<typename T>
    bool has_case() const {
        return this->has_case(_::cpp_type<T>::id(m_world));
    }

    /** Get case for switch.
     *
     * @param sw The switch for which to obtain the case.
     * @return True if the entity has the provided case, false otherwise.
     */
    flecs::entity get_case(flecs::id_t sw) const;

    /** Get case for switch.
     *
     * @param sw The switch for which to obtain the case.
     * @return True if the entity has the provided case, false otherwise.
     */
    template<typename T> 
    flecs::entity get_case() const;

    /** Get case for switch.
     *
     * @param sw The switch for which to obtain the case.
     * @return True if the entity has the provided case, false otherwise.
     */
    flecs::entity get_case(const flecs::type& sw) const;

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
     * @param entity The component to test.
     * @return True if the component is enabled, false if it has been disabled.
     */
    bool is_enabled(const flecs::entity_view& e) {
        return ecs_is_component_enabled_w_id(
            m_world, m_id, e.id());
    }

    /** Get current delta time.
     * Convenience function so system implementations can get delta_time, even
     * if they are using the .each() function.
     *
     * @return Current delta_time.
     */
    FLECS_FLOAT delta_time() const {
        const ecs_world_info_t *stats = ecs_get_world_info(m_world);
        return stats->delta_time;
    }

    /** Return iterator to entity children.
     * Enables depth-first iteration over entity children.
     *
     * @return Iterator to child entities.
     */
    template <typename Func>
    void children(Func&& func) const;

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
     * @param stage An created for the current stage.
     * @return An entity handle that allows for mutations in the current stage.
     */
    flecs::entity mut(const flecs::iter& it) const;

    /** Same as mut(world), but for entity.
     * This operation allows for the construction of a mutable entity handle
     * from another entity. This is useful in each() functions, which only 
     * provide a handle to the entity being iterated over.
     *
     * @param stage An created for the current stage.
     * @return An entity handle that allows for mutations in the current stage.
     */
    flecs::entity mut(const flecs::entity_view& e) const;

private:
    flecs::entity set_stage(world_t *stage);
};

}
