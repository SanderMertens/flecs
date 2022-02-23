#pragma once

namespace flecs
{

template <typename Self>
struct entity_builder : entity_view {

    using entity_view::entity_view;

    /** Add a component to an entity.
     * To ensure the component is initialized, it should have a constructor.
     * 
     * @tparam T the component type to add.
     */
    template <typename T>
    Self& add() {
        flecs_static_assert(is_flecs_constructible<T>::value,
            "cannot default construct type: add T::T() or use emplace<T>()");
        ecs_add_id(this->m_world, this->m_id, _::cpp_type<T>::id(this->m_world));
        return to_base();
    }

    /** Add an entity to an entity.
     * Add an entity to the entity. This is typically used for tagging.
     *
     * @param entity The entity to add.
     */
    Self& add(entity_t entity) {
        ecs_add_id(this->m_world, this->m_id, entity);
        return to_base();
    }

    /** Add a pair.
     * This operation adds a pair to the entity.
     *
     * @param relation The relation id.
     * @param object The object id.
     */
    Self& add(entity_t relation, entity_t object) {
        ecs_add_pair(this->m_world, this->m_id, relation, object);
        return to_base();
    }

    /** Add a pair.
     * This operation adds a pair to the entity.
     *
     * @tparam R the relation type.
     * @tparam O the object type.
     */
    template<typename R, typename O>
    Self& add() {
        return this->add<R>(_::cpp_type<O>::id(this->m_world));
    }

    /** Add a pair.
     * This operation adds a pair to the entity.
     *
     * @tparam R the relation type.
     * @param object the object type.
     */
    template<typename R>
    Self& add(entity_t object) {
        flecs_static_assert(is_flecs_constructible<R>::value,
            "cannot default construct type: add T::T() or use emplace<T>()");      
        return this->add(_::cpp_type<R>::id(this->m_world), object);
    }

    /** Shortcut for add(IsA, obj).
     *
     * @param object the object id.
     */
    Self& is_a(entity_t object) {
        return this->add(flecs::IsA, object);
    }

    /** Shortcut for add(IsA, obj).
     *
     * @tparam T the type associated with the object.
     */
    template <typename T>
    Self& is_a() {
        return this->add(flecs::IsA, _::cpp_type<T>::id(this->m_world));
    }

    /** Shortcut for add(ChildOf, obj).
     *
     * @param object the object id.
     */
    Self& child_of(entity_t object) {
        return this->add(flecs::ChildOf, object);
    }

    /** Shortcut for add(ChildOf, obj).
     *
     * @tparam T the type associated with the object.
     */
    template <typename T>
    Self& child_of() {
        return this->add(flecs::ChildOf, _::cpp_type<T>::id(this->m_world));
    }
 
    /** Add a pair with object type.
     * This operation adds a pair to the entity. The relation part of the pair
     * should not be a component.
     *
     * @param relation the relation type.
     * @tparam O the object type.
     */
    template<typename O>
    Self& add_w_object(entity_t relation) {
        flecs_static_assert(is_flecs_constructible<O>::value,
            "cannot default construct type: add T::T() or use emplace<T>()");      
        return this->add(relation,  _::cpp_type<O>::id(this->m_world));
    }

    /** Remove a component from an entity.
     *
     * @tparam T the type of the component to remove.
     */
    template <typename T, if_not_t< is_enum<T>::value > = 0>
    Self& remove() {
        ecs_remove_id(this->m_world, this->m_id, _::cpp_type<T>::id(this->m_world));
        return to_base();
    }

    /** Remove an entity from an entity.
     *
     * @param entity The entity to remove.
     */
    Self& remove(entity_t entity) {
        ecs_remove_id(this->m_world, this->m_id, entity);
        return to_base();
    }

    /** Remove a pair.
     * This operation removes a pair from the entity.
     *
     * @param relation The relation id.
     * @param object The object id.
     */
    Self& remove(entity_t relation, entity_t object) {
        ecs_remove_pair(this->m_world, this->m_id, relation, object);
        return to_base();
    }

    /** Removes a pair.
     * This operation removes a pair from the entity.
     *
     * @tparam Relation the relation type.
     * @tparam Object the object type.
     */
    template<typename Relation, typename Object>
    Self& remove() {
        return this->remove<Relation>(_::cpp_type<Object>::id(this->m_world));
    }

    /** Remove a pair.
     * This operation adds a pair to the entity.
     *
     * @tparam Relation the relation type.
     * @param object the object type.
     */
    template<typename Relation>
    Self& remove(entity_t object) {
        return this->remove(_::cpp_type<Relation>::id(this->m_world), object);
    }  

    /** Removes a pair with object type.
     * This operation removes a pair from the entity.
     *
     * @param relation the relation type.
     * @tparam Object the object type.
     */
    template<typename Object>
    Self& remove_w_object(entity_t relation) {
        return this->remove(relation, _::cpp_type<Object>::id(this->m_world));
    }    

    /** Add owned flag for component (forces ownership when instantiating)
     *
     * @param entity The entity for which to add the OVERRIDE flag
     */    
    Self& override(entity_t entity) {
        ecs_add_id(this->m_world, this->m_id, ECS_OVERRIDE | entity);
        return to_base();  
    }

    /** Add owned flag for component (forces ownership when instantiating)
     *
     * @tparam T The component for which to add the OVERRIDE flag
     */    
    template <typename T>
    Self& override() {
        ecs_add_id(this->m_world, this->m_id, ECS_OVERRIDE | _::cpp_type<T>::id(this->m_world));
        return to_base();  
    }

    /** Set value, add owned flag.
     *
     * @tparam T The component to set and for which to add the OVERRIDE flag
     */    
    template <typename T>
    Self& set_override(T&& val) {
        this->override<T>();
        this->set<T>(FLECS_FWD(val));
        return to_base();  
    }

    /** Set value, add owned flag.
     *
     * @tparam T The component to set and for which to add the OVERRIDE flag
     */    
    template <typename T>
    Self& set_override(const T& val) {
        this->override<T>();
        this->set<T>(val);
        return to_base();  
    }    

    /** Add a switch to an entity by id.
     * The switch entity must be a type, that is it must have the EcsType
     * component. Entities created with flecs::type are valid here.
     *
     * @param sw The switch entity id to add.
     */    
    Self& add_switch(entity_t sw) {
        ecs_add_id(this->m_world, this->m_id, ECS_SWITCH | sw);
        return to_base();  
    }

    /** Add a switch to an entity by C++ type.
     * The C++ type must be associated with a switch type.
     *
     * @tparam T The switch to add.
     */ 
    template <typename T>
    Self& add_switch() {
        ecs_add_id(this->m_world, this->m_id, 
            ECS_SWITCH | _::cpp_type<T>::id());
        return to_base();  
    }

    /** Add a switch to an entity.
     * Any instance of flecs::type can be used as a switch.
     *
     * @param sw The switch to add.
     */     
    Self& add_switch(const flecs::type& sw);

    /** Remove a switch from an entity by id.
     *
     * @param sw The switch to remove.
     */    
    Self& remove_switch(entity_t sw) {
        ecs_remove_id(this->m_world, this->m_id, ECS_SWITCH | sw);
        return to_base();  
    }
    
    /** Add a switch to an entity by C++ type.
     * The C++ type must be associated with a switch type.
     *
     * @tparam T The switch to remove.
     */ 
    template <typename T>
    Self& remove_switch() {
        ecs_remove_id(this->m_world, this->m_id, 
            ECS_SWITCH | _::cpp_type<T>::id());
        return to_base();  
    }

    /** Remove a switch from an entity.
     * Any instance of flecs::type can be used as a switch.
     *
     * @param sw The switch to remove.
     */      
    Self& remove_switch(const flecs::type& sw);

    /** Add a switch to an entity by id.
     * The case must belong to a switch that is already added to the entity.
     *
     * @param sw_case The case entity id to add.
     */    
    Self& add_case(entity_t sw_case) {
        ecs_add_id(this->m_world, this->m_id, ECS_CASE | sw_case);
        return to_base();
    }

    /** Add a switch to an entity by id.
     * The case must belong to a switch that is already added to the entity.
     *
     * @tparam T The case to add.
     */   
    template<typename T>
    Self& add_case() {
        return this->add_case(_::cpp_type<T>::id());
    }

    /** Remove a case from an entity by id.
     * The case must belong to a switch that is already added to the entity.
     *
     * @param sw_case The case entity id to remove.
     */    
    Self& remove_case(entity_t sw_case) {
        ecs_remove_id(this->m_world, this->m_id, ECS_CASE | sw_case);
        return to_base();  
    }

    /** Remove a switch from an entity by id.
     * The case must belong to a switch that is already added to the entity.
     *
     * @tparam T The case to remove.
     */   
    template<typename T>
    Self& remove_case() {
        return this->remove_case(_::cpp_type<T>::id());
    }

    /** Add pair for enum constant.
     * This operation will add a pair to the entity where R is the enumeration
     * type, and O is the entity representing the enum constant.
     * 
     * The operation may be used with regular (C style) enumerations as well as
     * enum classes.
     * 
     * @param value The enumeration value.
     */
    template <typename E, if_t< is_enum<E>::value > = 0>
    Self& add(E value) {
        flecs::entity_t r = _::cpp_type<E>::id(this->m_world);
        const auto& et = enum_type<E>(this->m_world);
        flecs::entity_t o = et.entity(value);
        return this->add(r, o);
    }

    /** Remove pair for enum.
     * This operation will remove any (Enum, *) pair from the entity.
     * 
     * @tparam E The enumeration type.
     */
    template <typename E, if_t< is_enum<E>::value > = 0>
    Self& remove() {
        flecs::entity_t r = _::cpp_type<E>::id(this->m_world);
        return this->remove(r, flecs::Wildcard);
    }

    /** Enable an entity.
     * Enabled entities are matched with systems and can be searched with
     * queries.
     */
    Self& enable() {
        ecs_enable(this->m_world, this->m_id, true);
        return to_base();
    }

    /** Disable an entity.
     * Disabled entities are not matched with systems and cannot be searched 
     * with queries, unless explicitly specified in the query expression.
     */
    Self& disable() {
        ecs_enable(this->m_world, this->m_id, false);
        return to_base();
    }

    /** Enable a component.
     * This sets the enabled bit for this component. If this is the first time
     * the component is enabled or disabled, the bitset is added.
     *
     * @tparam T The component to enable.
     */   
    template<typename T>
    Self& enable() {
        ecs_enable_component_w_id(this->m_world, this->m_id, _::cpp_type<T>::id(), true);
        return to_base();
    }  

    /** Disable a component.
     * This sets the enabled bit for this component. If this is the first time
     * the component is enabled or disabled, the bitset is added.
     *
     * @tparam T The component to enable.
     */   
    template<typename T>
    Self& disable() {
        ecs_enable_component_w_id(this->m_world, this->m_id, _::cpp_type<T>::id(), false);
        return to_base();
    }  

    /** Enable a component.
     * See enable<T>.
     *
     * @param comp The component to enable.
     */   
    Self& enable(entity_t comp) {
        ecs_enable_component_w_id(this->m_world, this->m_id, comp, true);
        return to_base();       
    }

    /** Disable a component.
     * See disable<T>.
     *
     * @param comp The component to disable.
     */   
    Self& disable(entity_t comp) {
        ecs_enable_component_w_id(this->m_world, this->m_id, comp, false);
        return to_base();       
    }

    Self& set_ptr(entity_t comp, size_t size, const void *ptr) {
        ecs_set_id(this->m_world, this->m_id, comp, size, ptr);
        return to_base();
    }

    Self& set_ptr(entity_t comp, const void *ptr) {
        const flecs::Component *cptr = ecs_get(
            this->m_world, comp, EcsComponent);

        /* Can't set if it's not a component */
        ecs_assert(cptr != NULL, ECS_INVALID_PARAMETER, NULL);

        return set_ptr(comp, cptr->size, ptr);
    }

    template<typename T, if_t< 
        !is_callable<T>::value && is_actual<T>::value> = 0 >
    Self& set(T&& value) {
        flecs::set<T>(this->m_world, this->m_id, FLECS_FWD(value));
        return to_base();
    }

    template<typename T, if_t< 
        !is_callable<T>::value && is_actual<T>::value > = 0>
    Self& set(const T& value) {
        flecs::set<T>(this->m_world, this->m_id, value);
        return to_base();
    }

    template<typename T, typename A = actual_type_t<T>, if_not_t< 
        is_callable<T>::value || is_actual<T>::value > = 0>
    Self& set(A&& value) {
        flecs::set<T>(this->m_world, this->m_id, FLECS_FWD(value));
        return to_base();
    }

    template<typename T, typename A = actual_type_t<T>, if_not_t<
        is_callable<T>::value || is_actual<T>::value > = 0>
    Self& set(const A& value) {
        flecs::set<T>(this->m_world, this->m_id, value);
        return to_base();
    }

    /** Set a pair for an entity.
     * This operation sets the pair value, and uses the relation as type. If the
     * entity did not yet have the pair, it will be added.
     *
     * @tparam R The relation part of the pair.
     * @tparam O The object part of the pair.
     * @param value The value to set.
     */
    template <typename R, typename O, typename P = pair<R, O>, 
        typename A = actual_type_t<P>, if_not_t< flecs::is_pair<R>::value> = 0>
    Self& set(const A& value) {
        flecs::set<P>(this->m_world, this->m_id, value);
        return to_base();
    }

    /** Set a pair for an entity.
     * This operation sets the pair value, and uses the relation as type. If the
     * entity did not yet have the pair, it will be added.
     *
     * @tparam R The relation part of the pair.
     * @param object The object part of the pair.
     * @param value The value to set.
     */
    template <typename R>
    Self& set(entity_t object, const R& value) {
        auto relation = _::cpp_type<R>::id(this->m_world);
        flecs::set(this->m_world, this->m_id, value, 
            ecs_pair(relation, object));
        return to_base();
    }

    /** Set a pair for an entity.
     * This operation sets the pair value, and uses the relation as type. If the
     * entity did not yet have the pair, it will be added.
     *
     * @tparam O The object part of the pair.
     * @param relation The relation part of the pair.
     * @param value The value to set.
     */
    template <typename O>
    Self& set_w_object(entity_t relation, const O& value) {
        auto object = _::cpp_type<O>::id(this->m_world);
        flecs::set(this->m_world, this->m_id, value, 
            ecs_pair(relation, object));
        return to_base();
    }

    template <typename R, typename O>
    Self& set_w_object(const O& value) {
        flecs::set<pair_object<R, O>>(this->m_world, this->m_id, value);
        return to_base();
    }    

    /** Set 1..N components.
     * This operation accepts a callback with as arguments the components to
     * set. If the entity does not have all of the provided components, they
     * will be added.
     *
     * This operation is faster than individually calling get for each component
     * as it only obtains entity metadata once. When this operation is called
     * while deferred, its performance is equivalent to that of calling get_mut
     * for each component separately.
     *
     * The operation will invoke modified for each component after the callback
     * has been invoked.
     *
     * @param func The callback to invoke.
     */
    template <typename Func, if_t< is_callable<Func>::value > = 0>
    Self& set(const Func& func);

    /** Emplace component.
     * Emplace constructs a component in the storage, which prevents calling the
     * destructor on the object passed into the function.
     *
     * Emplace attempts the following signatures to construct the component:
     *  T{Args...}
     *  T{flecs::entity, Args...}
     *
     * If the second signature matches, emplace will pass in the current entity 
     * as argument to the constructor, which is useful if the component needs
     * to be aware of the entity to which it has been added.
     *
     * Emplace may only be called for components that have not yet been added
     * to the entity.
     *
     * @tparam T the component to emplace
     * @param args The arguments to pass to the constructor of T
     */
    template <typename T, typename ... Args>
    Self& emplace(Args&&... args) {
        flecs::emplace<T>(this->m_world, this->m_id, 
            FLECS_FWD(args)...);
        return to_base();
    }

    /** Entities created in function will have the current entity.
     *
     * @param func The function to call.
     */
    template <typename Func>
    Self& with(const Func& func) {
        ecs_id_t prev = ecs_set_with(this->m_world, this->m_id);
        func();
        ecs_set_with(this->m_world, prev);
        return to_base();
    }

    /** Entities created in function will have (Relation, this) 
     * This operation is thread safe.
     *
     * @tparam Relation The relation to use.
     * @param func The function to call.
     */
    template <typename Relation, typename Func>
    Self& with(const Func& func) {
        with(_::cpp_type<Relation>::id(this->m_world), func);
        return to_base();
    }

    /** Entities created in function will have (relation, this) 
     *
     * @param relation The relation to use.
     * @param func The function to call.
     */
    template <typename Func>
    Self& with(id_t relation, const Func& func) {
        ecs_id_t prev = ecs_set_with(this->m_world, 
            ecs_pair(relation, this->m_id));
        func();
        ecs_set_with(this->m_world, prev);
        return to_base();
    }

    /** The function will be ran with the scope set to the current entity. */
    template <typename Func>
    Self& scope(const Func& func) {
        ecs_entity_t prev = ecs_set_scope(this->m_world, this->m_id);
        func();
        ecs_set_scope(this->m_world, prev);
        return to_base();
    }

    /* Set the entity name.
     */
    Self& set_name(const char *name) {
        ecs_set_name(this->m_world, this->m_id, name);
        return to_base();
    }

#   ifdef FLECS_DOC
#   include "../doc/entity_builder_mixin.inl"
#   endif

protected:
    Self& to_base() {
        return *static_cast<Self*>(this);
    }
};

}
