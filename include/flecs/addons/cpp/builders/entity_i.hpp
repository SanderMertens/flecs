namespace flecs
{

template <typename Base>
class entity_builder_i {
public:
    /** Add a component to an entity.
     * To ensure the component is initialized, it should have a constructor.
     * 
     * @tparam T the component type to add.
     */
    template <typename T>
    Base& add() {
        flecs_static_assert(is_flecs_constructible<T>::value,
            "cannot default construct type: add T::T() or use emplace<T>()");
        ecs_add_id(this->world_v(), this->id_v(), _::cpp_type<T>::id(this->world_v()));
        return *this;
    }

    /** Add an entity to an entity.
     * Add an entity to the entity. This is typically used for tagging.
     *
     * @param entity The entity to add.
     */
    Base& add(entity_t entity) {
        ecs_add_id(this->world_v(), this->id_v(), entity);
        return *this;
    }

    /** Add a pair.
     * This operation adds a pair to the entity.
     *
     * @param relation The relation id.
     * @param object The object id.
     */
    Base& add(entity_t relation, entity_t object) {
        ecs_add_pair(this->world_v(), this->id_v(), relation, object);
        return *this;
    }

    /** Add a pair.
     * This operation adds a pair to the entity.
     *
     * @tparam R the relation type.
     * @tparam O the object type.
     */
    template<typename R, typename O>
    Base& add() {
        return this->add<R>(_::cpp_type<O>::id(this->world_v()));
    }

    /** Add a pair.
     * This operation adds a pair to the entity.
     *
     * @tparam R the relation type.
     * @param object the object type.
     */
    template<typename R>
    Base& add(entity_t object) {
        flecs_static_assert(is_flecs_constructible<R>::value,
            "cannot default construct type: add T::T() or use emplace<T>()");      
        return this->add(_::cpp_type<R>::id(this->world_v()), object);
    }

    /** Shortcut for add(IsA, obj).
     *
     * @param object the object id.
     */
    Base& is_a(entity_t object) {
        return this->add(flecs::IsA, object);
    }

    /** Shortcut for add(IsA, obj).
     *
     * @tparam T the type associated with the object.
     */
    template <typename T>
    Base& is_a() {
        return this->add(flecs::IsA, _::cpp_type<T>::id(this->world_v()));
    }

    /** Shortcut for add(ChildOf, obj).
     *
     * @param object the object id.
     */
    Base& child_of(entity_t object) {
        return this->add(flecs::ChildOf, object);
    }

    /** Shortcut for add(ChildOf, obj).
     *
     * @tparam T the type associated with the object.
     */
    template <typename T>
    Base& child_of() {
        return this->add(flecs::ChildOf, _::cpp_type<T>::id(this->world_v()));
    }
 
    /** Add a pair with object type.
     * This operation adds a pair to the entity. The relation part of the pair
     * should not be a component.
     *
     * @param relation the relation type.
     * @tparam O the object type.
     */
    template<typename O>
    Base& add_w_object(entity_t relation) {
        flecs_static_assert(is_flecs_constructible<O>::value,
            "cannot default construct type: add T::T() or use emplace<T>()");      
        return this->add(relation,  _::cpp_type<O>::id(this->world_v()));
    }

    /** Remove a component from an entity.
     *
     * @tparam T the type of the component to remove.
     */
    template <typename T>
    Base& remove() {
        ecs_remove_id(this->world_v(), this->id_v(), _::cpp_type<T>::id(this->world_v()));
        return *this;
    }

    /** Remove an entity from an entity.
     *
     * @param entity The entity to remove.
     */
    Base& remove(entity_t entity) {
        ecs_remove_id(this->world_v(), this->id_v(), entity);
        return *this;
    }

    /** Remove a pair.
     * This operation removes a pair from the entity.
     *
     * @param relation The relation id.
     * @param object The object id.
     */
    Base& remove(entity_t relation, entity_t object) {
        ecs_remove_pair(this->world_v(), this->id_v(), relation, object);
        return *this;
    }

    /** Removes a pair.
     * This operation removes a pair from the entity.
     *
     * @tparam Relation the relation type.
     * @tparam Object the object type.
     */
    template<typename Relation, typename Object>
    Base& remove() {
        return this->remove<Relation>(_::cpp_type<Object>::id(this->world_v()));
    }

    /** Remove a pair.
     * This operation adds a pair to the entity.
     *
     * @tparam Relation the relation type.
     * @param object the object type.
     */
    template<typename Relation>
    Base& remove(entity_t object) {
        return this->remove(_::cpp_type<Relation>::id(this->world_v()), object);
    }  

    /** Removes a pair with object type.
     * This operation removes a pair from the entity.
     *
     * @param relation the relation type.
     * @tparam Object the object type.
     */
    template<typename Object>
    Base& remove_w_object(entity_t relation) {
        return this->remove(relation, _::cpp_type<Object>::id(this->world_v()));
    }    

    /** Add owned flag for component (forces ownership when instantiating)
     *
     * @param entity The entity for which to add the OVERRIDE flag
     */    
    Base& override(entity_t entity) {
        ecs_add_id(this->world_v(), this->id_v(), ECS_OVERRIDE | entity);
        return *this;  
    }

    /** Add owned flag for component (forces ownership when instantiating)
     *
     * @tparam T The component for which to add the OVERRIDE flag
     */    
    template <typename T>
    Base& override() {
        ecs_add_id(this->world_v(), this->id_v(), ECS_OVERRIDE | _::cpp_type<T>::id(this->world_v()));
        return *this;  
    }

    /** Set value, add owned flag.
     *
     * @tparam T The component to set and for which to add the OVERRIDE flag
     */    
    template <typename T>
    Base& set_override(T&& val) {
        this->override<T>();
        this->set<T>(std::forward<T>(val));
        return *this;  
    }    

    /** Add a switch to an entity by id.
     * The switch entity must be a type, that is it must have the EcsType
     * component. Entities created with flecs::type are valid here.
     *
     * @param sw The switch entity id to add.
     */    
    Base& add_switch(entity_t sw) {
        ecs_add_id(this->world_v(), this->id_v(), ECS_SWITCH | sw);
        return *this;  
    }

    /** Add a switch to an entity by C++ type.
     * The C++ type must be associated with a switch type.
     *
     * @tparam T The switch to add.
     */ 
    template <typename T>
    Base& add_switch() {
        ecs_add_id(this->world_v(), this->id_v(), 
            ECS_SWITCH | _::cpp_type<T>::id());
        return *this;  
    }

    /** Add a switch to an entity.
     * Any instance of flecs::type can be used as a switch.
     *
     * @param sw The switch to add.
     */     
    Base& add_switch(const type& sw);

    /** Remove a switch from an entity by id.
     *
     * @param sw The switch to remove.
     */    
    Base& remove_switch(entity_t sw) {
        ecs_remove_id(this->world_v(), this->id_v(), ECS_SWITCH | sw);
        return *this;  
    }
    
    /** Add a switch to an entity by C++ type.
     * The C++ type must be associated with a switch type.
     *
     * @tparam T The switch to remove.
     */ 
    template <typename T>
    Base& remove_switch() {
        ecs_remove_id(this->world_v(), this->id_v(), 
            ECS_SWITCH | _::cpp_type<T>::id());
        return *this;  
    }

    /** Remove a switch from an entity.
     * Any instance of flecs::type can be used as a switch.
     *
     * @param sw The switch to remove.
     */      
    Base& remove_switch(const type& sw);

    /** Add a switch to an entity by id.
     * The case must belong to a switch that is already added to the entity.
     *
     * @param sw_case The case entity id to add.
     */    
    Base& add_case(entity_t sw_case) {
        ecs_add_id(this->world_v(), this->id_v(), ECS_CASE | sw_case);
        return *this;
    }

    /** Add a switch to an entity by id.
     * The case must belong to a switch that is already added to the entity.
     *
     * @tparam T The case to add.
     */   
    template<typename T>
    Base& add_case() {
        return this->add_case(_::cpp_type<T>::id());
    }

    /** Remove a case from an entity by id.
     * The case must belong to a switch that is already added to the entity.
     *
     * @param sw_case The case entity id to remove.
     */    
    Base& remove_case(entity_t sw_case) {
        ecs_remove_id(this->world_v(), this->id_v(), ECS_CASE | sw_case);
        return *this;  
    }

    /** Remove a switch from an entity by id.
     * The case must belong to a switch that is already added to the entity.
     *
     * @tparam T The case to remove.
     */   
    template<typename T>
    Base& remove_case() {
        return this->remove_case(_::cpp_type<T>::id());
    }

    /** Enable an entity.
     * Enabled entities are matched with systems and can be searched with
     * queries.
     */
    Base& enable() {
        ecs_enable(this->world_v(), this->id_v(), true);
        return *this;
    }

    /** Disable an entity.
     * Disabled entities are not matched with systems and cannot be searched 
     * with queries, unless explicitly specified in the query expression.
     */
    Base& disable() {
        ecs_enable(this->world_v(), this->id_v(), false);
        return *this;
    }

    /** Enable a component.
     * This sets the enabled bit for this component. If this is the first time
     * the component is enabled or disabled, the bitset is added.
     *
     * @tparam T The component to enable.
     */   
    template<typename T>
    Base& enable() {
        ecs_enable_component_w_id(this->world_v(), this->id_v(), _::cpp_type<T>::id(), true);
        return *this;
    }  

    /** Disable a component.
     * This sets the enabled bit for this component. If this is the first time
     * the component is enabled or disabled, the bitset is added.
     *
     * @tparam T The component to enable.
     */   
    template<typename T>
    Base& disable() {
        ecs_enable_component_w_id(this->world_v(), this->id_v(), _::cpp_type<T>::id(), false);
        return *this;
    }  

    /** Enable a component.
     * See enable<T>.
     *
     * @param comp The component to enable.
     */   
    Base& enable(entity_t comp) {
        ecs_enable_component_w_id(this->world_v(), this->id_v(), comp, true);
        return *this;       
    }

    /** Disable a component.
     * See disable<T>.
     *
     * @param comp The component to disable.
     */   
    Base& disable(entity_t comp) {
        ecs_enable_component_w_id(this->world_v(), this->id_v(), comp, false);
        return *this;       
    }

    Base& set_ptr(entity_t comp, size_t size, const void *ptr) {
        ecs_set_id(this->world_v(), this->id_v(), comp, size, ptr);
        return *this;
    }

    Base& set_ptr(entity_t comp, const void *ptr) {
        const flecs::Component *cptr = ecs_get(
            this->world_v(), comp, EcsComponent);

        /* Can't set if it's not a component */
        ecs_assert(cptr != NULL, ECS_INVALID_PARAMETER, NULL);

        return set_ptr(comp, cptr->size, ptr);
    }

    template<typename T, if_t< 
        !is_callable<T>::value && is_actual<T>::value> = 0 >
    Base& set(T&& value) {
        flecs::set<T>(this->world_v(), this->id_v(), std::forward<T&&>(value));
        return *this;
    }

    template<typename T, if_t< 
        !is_callable<T>::value && is_actual<T>::value > = 0>
    Base& set(const T& value) {
        flecs::set<T>(this->world_v(), this->id_v(), value);
        return *this;
    }

    template<typename T, typename A = actual_type_t<T>, if_not_t< 
        is_callable<T>::value || is_actual<T>::value > = 0>
    Base& set(A&& value) {
        flecs::set<T>(this->world_v(), this->id_v(), std::forward<A&&>(value));
        return *this;
    }

    template<typename T, typename A = actual_type_t<T>, if_not_t<
        is_callable<T>::value || is_actual<T>::value > = 0>
    Base& set(const A& value) {
        flecs::set<T>(this->world_v(), this->id_v(), value);
        return *this;
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
        typename A = actual_type_t<P>, if_not_t< is_pair<R>::value> = 0>
    Base& set(const A& value) {
        flecs::set<P>(this->world_v(), this->id_v(), value);
        return *this;
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
    Base& set(entity_t object, const R& value) {
        auto relation = _::cpp_type<R>::id(this->world_v());
        flecs::set(this->world_v(), this->id_v(), value, 
            ecs_pair(relation, object));
        return *this;
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
    Base& set_w_object(entity_t relation, const O& value) {
        auto object = _::cpp_type<O>::id(this->world_v());
        flecs::set(this->world_v(), this->id_v(), value, 
            ecs_pair(relation, object));
        return *this;
    }

    template <typename R, typename O>
    Base& set_w_object(const O& value) {
        flecs::set<pair_object<R, O>>(this->world_v(), this->id_v(), value);
        return *this;
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
    Base& set(const Func& func);

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
    Base& emplace(Args&&... args) {
        flecs::emplace<T>(this->world_v(), this->id_v(), 
            std::forward<Args>(args)...);
        return *this;
    }

    /** Entities created in function will have the current entity.
     *
     * @param func The function to call.
     */
    template <typename Func>
    Base& with(const Func& func) {
        ecs_id_t prev = ecs_set_with(this->world_v(), this->id_v());
        func();
        ecs_set_with(this->world_v(), prev);
        return *this;
    }

    /** Entities created in function will have (Relation, this) 
     * This operation is thread safe.
     *
     * @tparam Relation The relation to use.
     * @param func The function to call.
     */
    template <typename Relation, typename Func>
    Base& with(const Func& func) {
        with(_::cpp_type<Relation>::id(this->world_v()), func);
        return *this;
    }  

    /** Entities created in function will have (relation, this) 
     *
     * @param relation The relation to use.
     * @param func The function to call.
     */
    template <typename Func>
    Base& with(id_t relation, const Func& func) {
        ecs_id_t prev = ecs_set_with(this->world_v(), 
            ecs_pair(relation, this->id_v()));
        func();
        ecs_set_with(this->world_v(), prev);
        return *this;
    }

    /** The function will be ran with the scope set to the current entity. */
    template <typename Func>
    Base& scope(const Func& func) {
        ecs_entity_t prev = ecs_set_scope(this->world_v(), this->id_v());
        func();
        ecs_set_scope(this->world_v(), prev);
        return *this;
    }

    /** Associate entity with type.
     * This operation enables using a type to refer to an entity, as it
     * associates the entity id with the provided type.
     *
     * If the entity does not have a name, a name will be derived from the type.
     * If the entity already is a component, the provided type must match in
     * size with the component size of the entity. After this operation the
     * entity will be a component (it will have the EcsComponent component) if
     * the type has a non-zero size.
     *
     * @tparam T the type to associate with the entity.
     */
    template <typename T>
    Base& component();

    /* Set the entity name.
     */
    Base& set_name(const char *name) {
        ecs_set_name(this->world_v(), this->id_v(), name);
        return *this;
    }

    virtual ~entity_builder_i() { }

protected:
    virtual flecs::world_t* world_v() = 0;
    virtual flecs::entity_t id_v() = 0;

    operator Base&() {
        return *static_cast<Base*>(this);
    }
};

}
