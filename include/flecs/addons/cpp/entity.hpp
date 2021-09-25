
namespace flecs {

template<typename T, typename Base>
class entity_builder_base {
public:
    const Base& base() const { return *static_cast<const Base*>(this); }
    flecs::world_t* base_world() const { return base().world(); }
    flecs::entity_t base_id() const { return base().id(); }
    operator const Base&() const {
        return this->base();
    }
};

}

#ifdef FLECS_DEPRECATED
#include "../deprecated/entity.hpp"
#else
namespace flecs
{
template <typename Base>
class entity_builder_deprecated { };

template <typename Base>
class entity_deprecated { };
}
#endif

namespace flecs
{

/** Fluent API for chaining entity operations
 * This class contains entity operations that can be chained. For example, by
 * using this class, an entity can be created like this:
 *
 * flecs::entity e = flecs::entity(world)
 *   .add<Position>()
 *   .add<Velocity>();
 */
struct entity_builder_tag { }; // Tag to prevent ambiguous base

template <typename Base>
class entity_builder : public entity_builder_base<entity_builder_tag, Base> {
public:
    /** Add a component to an entity.
     * To ensure the component is initialized, it should have a constructor.
     * 
     * @tparam T the component type to add.
     */
    template <typename T>
    const Base& add() const {
        flecs_static_assert(is_flecs_constructible<T>::value,
            "cannot default construct type: add T::T() or use emplace<T>()");
        ecs_add_id(this->base_world(), this->base_id(), _::cpp_type<T>::id(this->base_world()));
        return *this;
    }

    /** Add an entity to an entity.
     * Add an entity to the entity. This is typically used for tagging.
     *
     * @param entity The entity to add.
     */
    const Base& add(entity_t entity) const {
        ecs_add_id(this->base_world(), this->base_id(), entity);
        return *this;
    }

    /** Add a pair.
     * This operation adds a pair to the entity.
     *
     * @param relation The relation id.
     * @param object The object id.
     */
    const Base& add(entity_t relation, entity_t object) const {
        ecs_add_pair(this->base_world(), this->base_id(), relation, object);
        return *this;
    }

    /** Add a pair.
     * This operation adds a pair to the entity.
     *
     * @tparam R the relation type.
     * @tparam O the object type.
     */
    template<typename R, typename O>
    const Base& add() const {
        return this->add<R>(_::cpp_type<O>::id(this->base_world()));
    }

    /** Add a pair.
     * This operation adds a pair to the entity.
     *
     * @tparam R the relation type.
     * @param object the object type.
     */
    template<typename R>
    const Base& add(entity_t object) const {
        flecs_static_assert(is_flecs_constructible<R>::value,
            "cannot default construct type: add T::T() or use emplace<T>()");      
        return this->add(_::cpp_type<R>::id(this->base_world()), object);
    }

    /** Shortcut for add(IsA. obj).
     *
     * @param object the object id.
     */
    const Base& is_a(entity_t object) const {
        return this->add(flecs::IsA, object);
    }

    template <typename T>
    const Base& is_a() const {
        return this->add(flecs::IsA, _::cpp_type<T>::id(this->base_world()));
    }

    /** Shortcut for add(ChildOf. obj).
     *
     * @param object the object id.
     */
    const Base& child_of(entity_t object) const {
        return this->add(flecs::ChildOf, object);
    }

    /** Shortcut for add(ChildOf. obj).
     *
     * @param object the object id.
     */
    template <typename T>
    const Base& child_of() const {
        return this->add(flecs::ChildOf, _::cpp_type<T>::id(this->base_world()));
    }
 
    /** Add a pair with object type.
     * This operation adds a pair to the entity. The relation part of the pair
     * should not be a component.
     *
     * @param relation the relation type.
     * @tparam O the object type.
     */
    template<typename O>
    const Base& add_w_object(entity_t relation) const {
        flecs_static_assert(is_flecs_constructible<O>::value,
            "cannot default construct type: add T::T() or use emplace<T>()");      
        return this->add(relation,  _::cpp_type<O>::id(this->base_world()));
    }

    /** Remove a component from an entity.
     *
     * @tparam T the type of the component to remove.
     */
    template <typename T>
    const Base& remove() const {
        ecs_remove_id(this->base_world(), this->base_id(), _::cpp_type<T>::id(this->base_world()));
        return *this;
    }

    /** Remove an entity from an entity.
     *
     * @param entity The entity to remove.
     */
    const Base& remove(entity_t entity) const {
        ecs_remove_id(this->base_world(), this->base_id(), entity);
        return *this;
    }

    /** Remove a pair.
     * This operation removes a pair from the entity.
     *
     * @param relation The relation id.
     * @param object The object id.
     */
    const Base& remove(entity_t relation, entity_t object) const {
        ecs_remove_pair(this->base_world(), this->base_id(), relation, object);
        return *this;
    }

    /** Removes a pair.
     * This operation removes a pair from the entity.
     *
     * @tparam Relation the relation type.
     * @tparam Object the object type.
     */
    template<typename Relation, typename Object>
    const Base& remove() const {
        return this->remove<Relation>(_::cpp_type<Object>::id(this->base_world()));
    }

    /** Remove a pair.
     * This operation adds a pair to the entity.
     *
     * @tparam Relation the relation type.
     * @param object the object type.
     */
    template<typename Relation>
    const Base& remove(entity_t object) const {
        return this->remove(_::cpp_type<Relation>::id(this->base_world()), object);
    }  

    /** Removes a pair with object type.
     * This operation removes a pair from the entity.
     *
     * @param relation the relation type.
     * @tparam Object the object type.
     */
    template<typename Object>
    const Base& remove_w_object(entity_t relation) const {
        return this->remove(relation, _::cpp_type<Object>::id(this->base_world()));
    }    

    /** Add owned flag for component (forces ownership when instantiating)
     *
     * @param entity The entity for which to add the OVERRIDE flag
     */    
    const Base& override(entity_t entity) const {
        ecs_add_id(this->base_world(), this->base_id(), ECS_OVERRIDE | entity);
        return *this;  
    }

    /** Add owned flag for component (forces ownership when instantiating)
     *
     * @tparam T The component for which to add the OVERRIDE flag
     */    
    template <typename T>
    const Base& override() const {
        ecs_add_id(this->base_world(), this->base_id(), ECS_OVERRIDE | _::cpp_type<T>::id(this->base_world()));
        return *this;  
    }

    /** Set value, add owned flag.
     *
     * @tparam T The component to set and for which to add the OVERRIDE flag
     */    
    template <typename T>
    const Base& set_override(T&& val) const {
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
    const Base& add_switch(entity_t sw) const {
        ecs_add_id(this->base_world(), this->base_id(), ECS_SWITCH | sw);
        return *this;  
    }

    /** Add a switch to an entity by C++ type.
     * The C++ type must be associated with a switch type.
     *
     * @param sw The switch to add.
     */ 
    template <typename T>
    const Base& add_switch() const {
        ecs_add_id(this->base_world(), this->base_id(), 
            ECS_SWITCH | _::cpp_type<T>::id());
        return *this;  
    }

    /** Add a switch to an entity.
     * Any instance of flecs::type can be used as a switch.
     *
     * @param sw The switch to add.
     */     
    const Base& add_switch(const type& sw) const;

    /** Remove a switch from an entity by id.
     *
     * @param sw The switch entity id to remove.
     */    
    const Base& remove_switch(entity_t sw) const {
        ecs_remove_id(this->base_world(), this->base_id(), ECS_SWITCH | sw);
        return *this;  
    }
    
    /** Add a switch to an entity by C++ type.
     * The C++ type must be associated with a switch type.
     *
     * @param sw The switch to add.
     */ 
    template <typename T>
    const Base& remove_switch() const {
        ecs_remove_id(this->base_world(), this->base_id(), 
            ECS_SWITCH | _::cpp_type<T>::id());
        return *this;  
    }

    /** Remove a switch from an entity.
     * Any instance of flecs::type can be used as a switch.
     *
     * @param sw The switch to remove.
     */      
    const Base& remove_switch(const type& sw) const;

    /** Add a switch to an entity by id.
     * The case must belong to a switch that is already added to the entity.
     *
     * @param sw_case The case entity id to add.
     */    
    const Base& add_case(entity_t sw_case) const {
        ecs_add_id(this->base_world(), this->base_id(), ECS_CASE | sw_case);
        return *this;
    }

    /** Add a switch to an entity by id.
     * The case must belong to a switch that is already added to the entity.
     *
     * @tparam T The case to add.
     */   
    template<typename T>
    const Base& add_case() const {
        return this->add_case(_::cpp_type<T>::id());
    }

    /** Remove a case from an entity by id.
     * The case must belong to a switch that is already added to the entity.
     *
     * @param sw_case The case entity id to remove.
     */    
    const Base& remove_case(entity_t sw_case) const {
        ecs_remove_id(this->base_world(), this->base_id(), ECS_CASE | sw_case);
        return *this;  
    }

    /** Remove a switch from an entity by id.
     * The case must belong to a switch that is already added to the entity.
     *
     * @tparam T The case to remove.
     */   
    template<typename T>
    const Base& remove_case() const {
        return this->remove_case(_::cpp_type<T>::id());
    }

    /** Enable an entity.
     * Enabled entities are matched with systems and can be searched with
     * queries.
     */
    const Base& enable() const {
        ecs_enable(this->base_world(), this->base_id(), true);
        return *this;
    }

    /** Disable an entity.
     * Disabled entities are not matched with systems and cannot be searched 
     * with queries, unless explicitly specified in the query expression.
     */
    const Base& disable() const {
        ecs_enable(this->base_world(), this->base_id(), false);
        return *this;
    }

    /** Enable a component.
     * This sets the enabled bit for this component. If this is the first time
     * the component is enabled or disabled, the bitset is added.
     *
     * @tparam T The component to enable.
     */   
    template<typename T>
    const Base& enable() const {
        ecs_enable_component_w_id(this->base_world(), this->base_id(), _::cpp_type<T>::id(), true);
        return *this;
    }  

    /** Disable a component.
     * This sets the enabled bit for this component. If this is the first time
     * the component is enabled or disabled, the bitset is added.
     *
     * @tparam T The component to enable.
     */   
    template<typename T>
    const Base& disable() const {
        ecs_enable_component_w_id(this->base_world(), this->base_id(), _::cpp_type<T>::id(), false);
        return *this;
    }  

    /** Enable a component.
     * See enable<T>.
     *
     * @param component The component to enable.
     */   
    const Base& enable(entity_t comp) const {
        ecs_enable_component_w_id(this->base_world(), this->base_id(), comp, true);
        return *this;       
    }

    /** Disable a component.
     * See disable<T>.
     *
     * @param component The component to disable.
     */   
    const Base& disable(entity_t comp) const {
        ecs_enable_component_w_id(this->base_world(), this->base_id(), comp, false);
        return *this;       
    }

    const Base& set_ptr(entity_t comp, size_t size, const void *ptr) {
        ecs_set_id(this->base_world(), this->base_id(), comp, size, ptr);
        return *this;
    }

    const Base& set_ptr(entity_t comp, const void *ptr) {
        const flecs::Component *cptr = ecs_get(
            this->base_world(), comp, EcsComponent);

        /* Can't set if it's not a component */
        ecs_assert(cptr != NULL, ECS_INVALID_PARAMETER, NULL);

        return set_ptr(comp, cptr->size, ptr);
    }

    template<typename T, if_t< 
        !is_callable<T>::value && is_actual<T>::value> = 0 >
    const Base& set(T&& value) const {
        flecs::set<T>(this->base_world(), this->base_id(), std::forward<T&&>(value));
        return *this;
    }

    template<typename T, if_t< 
        !is_callable<T>::value && is_actual<T>::value > = 0>
    const Base& set(const T& value) const {
        flecs::set<T>(this->base_world(), this->base_id(), value);
        return *this;
    }

    template<typename T, typename A = actual_type_t<T>, if_not_t< 
        is_callable<T>::value || is_actual<T>::value > = 0>
    const Base& set(A&& value) const {
        flecs::set<T>(this->base_world(), this->base_id(), std::forward<A&&>(value));
        return *this;
    }

    template<typename T, typename A = actual_type_t<T>, if_not_t<
        is_callable<T>::value || is_actual<T>::value > = 0>
    const Base& set(const A& value) const {
        flecs::set<T>(this->base_world(), this->base_id(), value);
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
    const Base& set(const A& value) const {
        flecs::set<P>(this->base_world(), this->base_id(), value);
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
    const Base& set(entity_t object, const R& value) const {
        auto relation = _::cpp_type<R>::id(this->base_world());
        flecs::set(this->base_world(), this->base_id(), value, 
            ecs_pair(relation, object));
        return *this;
    }

    /** Set a pair for an entity.
     * This operation sets the pair value, and uses the relation as type. If the
     * entity did not yet have the pair, it will be added.
     *
     * @tparam Object The object part of the pair.
     * @param relation The relation part of the pair.
     * @param value The value to set.
     */
    template <typename O>
    const Base& set_w_object(entity_t relation, const O& value) const {
        auto object = _::cpp_type<O>::id(this->base_world());
        flecs::set(this->base_world(), this->base_id(), value, 
            ecs_pair(relation, object));
        return *this;
    }

    template <typename R, typename O>
    const Base& set_w_object(const O& value) const {
        flecs::set<pair_object<R, O>>(this->base_world(), this->base_id(), value);
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
    const Base& set(const Func& func) const;

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
    const Base& emplace(Args&&... args) const {
        flecs::emplace<T>(this->base_world(), this->base_id(), 
            std::forward<Args>(args)...);
        return *this;
    }

    /** Entities created in function will have the current entity.
     *
     * @param func The function to call.
     */
    template <typename Func>
    const Base& with(const Func& func) const {
        ecs_id_t prev = ecs_set_with(this->base_world(), this->base_id());
        func();
        ecs_set_with(this->base_world(), prev);
        return *this;
    }

    /** Entities created in function will have (Relation, this) 
     * This operation is thread safe.
     *
     * @tparam Relation The relation to use.
     * @param func The function to call.
     */
    template <typename Relation, typename Func>
    const Base& with(const Func& func) const {
        with(_::cpp_type<Relation>::id(this->base_world()), func);
        return *this;
    }  

    /** Entities created in function will have (relation, this) 
     *
     * @param relation The relation to use.
     * @param func The function to call.
     */
    template <typename Func>
    const Base& with(id_t relation, const Func& func) const {
        ecs_id_t prev = ecs_set_with(this->base_world(), 
            ecs_pair(relation, this->base_id()));
        func();
        ecs_set_with(this->base_world(), prev);
        return *this;
    }

    /** The function will be ran with the scope set to the current entity. */
    template <typename Func>
    const Base& scope(const Func& func) const {
        ecs_entity_t prev = ecs_set_scope(this->base_world(), this->base_id());
        func();
        ecs_set_scope(this->base_world(), prev);
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
    const Base& component() const;

    /* Set the entity name.
     */
    const Base& set_name(const char *name) const {
        ecs_set_name(this->base_world(), this->base_id(), name);
        return *this;
    }    
};

////////////////////////////////////////////////////////////////////////////////
//// Quick and safe access to a component pointer
////////////////////////////////////////////////////////////////////////////////

template <typename T>
class ref {
public:
    ref()
        : m_world( nullptr )
        , m_entity( 0 )
        , m_ref() { }

    ref(world_t *world, entity_t entity) 
        : m_world( world )
        , m_entity( entity )
        , m_ref() 
    {
        auto comp_id = _::cpp_type<T>::id(world);

        ecs_assert(_::cpp_type<T>::size() != 0, 
                ECS_INVALID_PARAMETER, NULL);

        ecs_get_ref_w_id(
            m_world, &m_ref, m_entity, comp_id);
    }

    const T* operator->() {
        const T* result = static_cast<const T*>(ecs_get_ref_w_id(
            m_world, &m_ref, m_entity, _::cpp_type<T>::id(m_world)));

        ecs_assert(result != NULL, ECS_INVALID_PARAMETER, NULL);

        return result;
    }

    const T* get() {
        if (m_entity) {
            ecs_get_ref_w_id(
                m_world, &m_ref, m_entity, _::cpp_type<T>::id(m_world));    
        }

        return static_cast<T*>(m_ref.ptr);
    }

    flecs::entity entity() const;

private:
    world_t *m_world;
    entity_t m_entity;
    flecs::ref_t m_ref;
};

/** Entity class
 * This class provides access to entities. */
class entity : 
    public entity_view,
    public entity_builder<entity>, 
    public entity_deprecated<entity>, 
    public entity_builder_deprecated<entity>
{
public:
    /** Default constructor.
     */
    entity()
        : flecs::entity_view() { }

    /** Create entity.
     *
     * @param world The world in which to create the entity.
     */
    explicit entity(world_t *world) 
        : flecs::entity_view() 
    {
        m_world = world;
        m_id = ecs_new(world, 0);
    }

    /** Create a named entity.
     * Named entities can be looked up with the lookup functions. Entity names
     * may be scoped, where each element in the name is separated by "::".
     * For example: "Foo::Bar". If parts of the hierarchy in the scoped name do
     * not yet exist, they will be automatically created.
     *
     * @param world The world in which to create the entity.
     * @param name The entity name.
     * @param is_component If true, the entity will be created from the pool of component ids (default = false).
     */
    explicit entity(world_t *world, const char *name) 
        : flecs::entity_view()
    { 
        m_world = world;

        ecs_entity_desc_t desc = {};
        desc.name = name;
        desc.sep = "::";
        m_id = ecs_entity_init(world, &desc);
    }

    /** Wrap an existing entity id.
     *
     * @param world The world in which the entity is created.
     * @param id The entity id.
     */
    explicit entity(world_t *world, entity_t id)
        : flecs::entity_view()
    {
        m_world = world;
        m_id = id;
    }

    /** Conversion from flecs::entity_t to flecs::entity. */
    explicit entity(entity_t id) 
        : flecs::entity_view( nullptr, id ) { }

    /** Get entity id.
     * @return The integer entity id.
     */
    entity_t id() const {
        return m_id;
    }

    /** Get mutable component value.
     * This operation returns a mutable pointer to the component. If the entity
     * did not yet have the component, it will be added. If a base entity had
     * the component, it will be overridden, and the value of the base component
     * will be copied to the entity before this function returns.
     *
     * @tparam T The component to get.
     * @param is_added If provided, this parameter will be set to true if the component was added.
     * @return Pointer to the component value.
     */
    template <typename T>
    T* get_mut(bool *is_added = nullptr) const {
        auto comp_id = _::cpp_type<T>::id(m_world);
        ecs_assert(_::cpp_type<T>::size() != 0, ECS_INVALID_PARAMETER, NULL);
        return static_cast<T*>(
            ecs_get_mut_id(m_world, m_id, comp_id, is_added));
    }

    /** Get mutable component value (untyped).
     * This operation returns a mutable pointer to the component. If the entity
     * did not yet have the component, it will be added. If a base entity had
     * the component, it will be overridden, and the value of the base component
     * will be copied to the entity before this function returns.
     *
     * @param component The component to get.
     * @param is_added If provided, this parameter will be set to true if the component was added.
     * @return Pointer to the component value.
     */
    void* get_mut(entity_t comp, bool *is_added = nullptr) const {
        return ecs_get_mut_id(m_world, m_id, comp, is_added);
    }

    /** Get mutable pointer for a pair.
     * This operation gets the value for a pair from the entity.
     *
     * @tparam Relation the relation type.
     * @tparam Object the object type.
     */
    template <typename Relation, typename Object>
    Relation* get_mut(bool *is_added = nullptr) const {
        return this->get_mut<Relation>(
            _::cpp_type<Object>::id(m_world), is_added);
    }

    /** Get mutable pointer for a pair.
     * This operation gets the value for a pair from the entity.
     *
     * @tparam Relation the relation type.
     * @param object the object.
     */
    template <typename Relation>
    Relation* get_mut(entity_t object, bool *is_added = nullptr) const {
        auto comp_id = _::cpp_type<Relation>::id(m_world);
        ecs_assert(_::cpp_type<Relation>::size() != 0, ECS_INVALID_PARAMETER, NULL);
        return static_cast<Relation*>(
            ecs_get_mut_id(m_world, m_id, 
                ecs_pair(comp_id, object), is_added));
    }

    /** Get mutable pointer for a pair (untyped).
     * This operation gets the value for a pair from the entity. If neither the
     * relation or object are a component, the operation will fail.
     *
     * @param relation the relation.
     * @param object the object.
     */
    void* get_mut(entity_t relation, entity_t object, bool *is_added = nullptr) const {
        return ecs_get_mut_id(m_world, m_id, 
                ecs_pair(relation, object), is_added);
    }

    /** Get mutable pointer for the object from a pair.
     * This operation gets the value for a pair from the entity.
     *
     * @tparam Object the object type.
     * @param relation the relation.
     */
    template <typename Object>
    Object* get_mut_w_object(entity_t relation, bool *is_added = nullptr) const {
        auto comp_id = _::cpp_type<Object>::id(m_world);
        ecs_assert(_::cpp_type<Object>::size() != 0, ECS_INVALID_PARAMETER, NULL);
        return static_cast<Object*>(
            ecs_get_mut_id(m_world, m_id, 
                ecs_pair(relation, comp_id), is_added));
    }           

    /** Signal that component was modified.
     *
     * @tparam T component that was modified.
     */
    template <typename T>
    void modified() const {
        auto comp_id = _::cpp_type<T>::id(m_world);
        ecs_assert(_::cpp_type<T>::size() != 0, ECS_INVALID_PARAMETER, NULL);
        this->modified(comp_id);
    } 

    /** Signal that the relation part of a pair was modified.
     *
     * @tparam Relation the relation type.
     * @tparam Object the object type.
     */
    template <typename Relation, typename Object>
    void modified() const {
        this->modified<Relation>(_::cpp_type<Object>::id(m_world));
    }

    /** Signal that the relation part of a pair was modified.
     *
     * @tparam Relation the relation type.
     * @param object the object.
     */
    template <typename Relation>
    void modified(entity_t object) const {
        auto comp_id = _::cpp_type<Relation>::id(m_world);
        ecs_assert(_::cpp_type<Relation>::size() != 0, ECS_INVALID_PARAMETER, NULL);
        this->modified(comp_id, object);
    }

    /** Signal that a pair has modified (untyped).
     * If neither the relation or object part of the pair are a component, the
     * operation will fail.
     *
     * @param relation the relation.
     * @param object the object.
     */
    void modified(entity_t relation, entity_t object) const {
        this->modified(ecs_pair(relation, object));
    }

    /** Signal that component was modified.
     *
     * @param component component that was modified.
     */
    void modified(entity_t comp) const {
        ecs_modified_id(m_world, m_id, comp);
    }

    /** Get reference to component.
     * A reference allows for quick and safe access to a component value, and is
     * a faster alternative to repeatedly calling 'get' for the same component.
     *
     * @tparam T component for which to get a reference.
     * @return The reference.
     */
    template <typename T>
    ref<T> get_ref() const {
        // Ensure component is registered
        _::cpp_type<T>::id(m_world);
        ecs_assert(_::cpp_type<T>::size() != 0, ECS_INVALID_PARAMETER, NULL);
        return ref<T>(m_world, m_id);
    }

    /** Clear an entity.
     * This operation removes all components from an entity without recycling
     * the entity id.
     */
    void clear() const {
        ecs_clear(m_world, m_id);
    }

    /** Delete an entity.
     * Entities have to be deleted explicitly, and are not deleted when the
     * flecs::entity object goes out of scope.
     */
    void destruct() const {
        ecs_delete(m_world, m_id);
    }

    /** Entity id 0.
     * This function is useful when the API must provide an entity object that
     * belongs to a world, but the entity id is 0.
     *
     * @param world The world.
     */
    static
    flecs::entity null(const flecs::world& world) {
        return flecs::entity(world.get_world().c_ptr(), 
            static_cast<entity_t>(0));
    }

    static
    flecs::entity null() {
        return flecs::entity(static_cast<entity_t>(0));
    }     
};

/** Prefab class */
class prefab final : public entity {
public:
    explicit prefab(world_t *world, const char *name = nullptr) 
        : entity(world, name)
    {
        this->add(flecs::Prefab);
    }
};

} // namespace flecs
