
#include "deprecated/entity.hpp"

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
template <typename Base>
class entity_builder {
    using base_type = const Base;
public:
    /** Add a component to an entity.
     * To ensure the component is initialized, it should have a constructor.
     * 
     * @tparam T the component type to add.
     */
    template <typename T>
    base_type& add() const {
        ecs_add_entity(world(), id(), _::cpp_type<T>::id(world()));
        return *base();
    }

    /** Add an entity to an entity.
     * Add an entity to the entity. This is typically used for tagging.
     *
     * @param entity The entity to add.
     */
    base_type& add(const Base& entity) const {
        ecs_add_entity(world(), id(), entity.id());
        return *base();
    }

    /** Add a type to an entity.
     * A type is a vector of component ids. This operation adds all components
     * in a single operation, and is a more efficient version of doing 
     * individual add operations.
     * 
     * @param type The type to add.
     */
    base_type& add(const type& type) const;

    /** Add a pair.
     * This operation adds a pair to the entity.
     *
     * @param relation The relation id.
     * @param object The object id.
     */
    base_type& add(const Base& relation, const Base& object) const {
        ecs_add_pair(world(), id(), relation.id(), object.id());
        return *base();
    }

    /** Add a pair.
     * This operation adds a pair to the entity.
     *
     * @tparam Relation the relation type.
     * @tparam Object the object type.
     */
    template<typename Relation, typename Object>
    base_type& add() const {
        ecs_add_pair(world(), id(),
            _::cpp_type<Relation>::id(world()), 
            _::cpp_type<Object>::id(world()));
        return *base(); 
    }

    /** Remove a component from an entity.
     *
     * @tparam T the type of the component to remove.
     */
    template <typename T>
    base_type& remove() const {
        ecs_remove_entity(world(), id(), _::cpp_type<T>::id(world()));
        return *base();
    }

    /** Remove an entity from an entity.
     *
     * @param entity The entity to remove.
     */
    base_type& remove(const Base& entity) const {
        ecs_remove_entity(world(), id(), entity.id());
        return *base();
    }

    /** Remove a type from an entity.
     * A type is a vector of component ids. This operation adds all components
     * in a single operation, and is a more efficient version of doing 
     * individual add operations.
     *
     * @param type the type to remove.
     */
    base_type& remove(const type& type) const;

    /** Remove a pair.
     * This operation removes a pair from the entity.
     *
     * @param relation The relation id.
     * @param object The object id.
     */
    base_type& remove(const Base& relation, const Base& object) const {
        ecs_remove_pair(world(), id(), relation.id(), object.id());
        return *base();
    }

    /** Removes a pair.
     * This operation removes a pair from the entity.
     *
     * @tparam Relation the relation type.
     * @tparam Object the object type.
     */
    template<typename Relation, typename Object>
    base_type& remove() const {
        ecs_remove_pair(world(), id(),
            _::cpp_type<Relation>::id(world()), 
            _::cpp_type<Object>::id(world()));
        return *base(); 
    }

    /** Add owned flag for component (forces ownership when instantiating)
     *
     * @param entity The entity for which to add the OWNED flag
     */    
    base_type& add_owned(const Base& entity) const {
        ecs_add_entity(world(), id(), ECS_OWNED | entity.id());
        return *base();  
    }

    /** Add owned flag for component (forces ownership when instantiating)
     *
     * @tparam T The component for which to add the OWNED flag
     */    
    template <typename T>
    base_type& add_owned() const {
        ecs_add_entity(world(), id(), ECS_OWNED | _::cpp_type<T>::id(world()));
        return *base();  
    }

    /** Add owned flag for type entity.
     * This will ensure that all components in the type are owned for instances
     * of this entity.
     *
     * @param type The type for which to add the OWNED flag
     */    
    base_type& add_owned(const type& type) const;

    /** Add a switch to an entity by id.
     * The switch entity must be a type, that is it must have the EcsType
     * component. Entities created with flecs::type are valid here.
     *
     * @param sw The switch entity id to add.
     */    
    base_type& add_switch(const Base& sw) const {
        ecs_add_entity(world(), id(), ECS_SWITCH | sw.id());
        return *base();  
    }

    /** Add a switch to an entity.
     * Any instance of flecs::type can be used as a switch.
     *
     * @param sw The switch to add.
     */     
    base_type& add_switch(const type& sw) const;

    /** Remove a switch from an entity by id.
     *
     * @param sw The switch entity id to remove.
     */    
    base_type& remove_switch(const Base& sw) const {
        ecs_remove_entity(world(), id(), ECS_SWITCH | sw.id());
        return *base();  
    }
    
    /** Remove a switch from an entity.
     * Any instance of flecs::type can be used as a switch.
     *
     * @param sw The switch to remove.
     */      
    base_type& remove_switch(const type& sw) const;

    /** Add a switch to an entity by id.
     * The case must belong to a switch that is already added to the entity.
     *
     * @param sw_case The case entity id to add.
     */    
    base_type& add_case(const Base& sw_case) const {
        ecs_add_entity(world(), id(), ECS_CASE | sw_case.id());
        return *base();
    }

    /** Add a switch to an entity by id.
     * The case must belong to a switch that is already added to the entity.
     *
     * @tparam T The case to add.
     */   
    template<typename T>
    base_type& add_case() const {
        return this->add_case(_::cpp_type<T>::id());
    }

    /** Remove a case from an entity by id.
     * The case must belong to a switch that is already added to the entity.
     *
     * @param sw_case The case entity id to remove.
     */    
    base_type& remove_case(const Base& sw_case) const {
        ecs_remove_entity(world(), id(), ECS_CASE | sw_case.id());
        return *base();  
    }

    /** Remove a switch from an entity by id.
     * The case must belong to a switch that is already added to the entity.
     *
     * @tparam T The case to remove.
     */   
    template<typename T>
    base_type& remove_case() const {
        return this->remove_case(_::cpp_type<T>::id());
    }

    /** Enable an entity.
     * Enabled entities are matched with systems and can be searched with
     * queries.
     */
    base_type& enable() const {
        ecs_enable(world(), id(), true);
        return *base();
    }

    /** Disable an entity.
     * Disabled entities are not matched with systems and cannot be searched 
     * with queries, unless explicitly specified in the query expression.
     */
    base_type& disable() const {
        ecs_enable(world(), id(), false);
        return *base();
    }

    /** Enable a component.
     * This sets the enabled bit for this component. If this is the first time
     * the component is enabled or disabled, the bitset is added.
     *
     * @tparam T The component to enable.
     */   
    template<typename T>
    base_type& enable() const {
        ecs_enable_component_w_entity(world(), id(), _::cpp_type<T>::id(), true);
        return *base();
    }  

    /** Disable a component.
     * This sets the enabled bit for this component. If this is the first time
     * the component is enabled or disabled, the bitset is added.
     *
     * @tparam T The component to enable.
     */   
    template<typename T>
    base_type& disable() const {
        ecs_enable_component_w_entity(world(), id(), _::cpp_type<T>::id(), false);
        return *base();
    }  

    /** Enable a component.
     * See enable<T>.
     *
     * @param component The component to enable.
     */   
    base_type& enable(const Base& component) const {
        ecs_enable_component_w_entity(world(), id(), component.id(), true);
        return *base();       
    }

    /** Disable a component.
     * See disable<T>.
     *
     * @param component The component to disable.
     */   
    base_type& disable(const Base& component) const {
        ecs_enable_component_w_entity(world(), id(), component.id(), false);
        return *base();       
    }

    /** Set a component for an entity.
     * This operation sets the component value. If the entity did not yet
     * have the component, it will be added.
     *
     * @tparam T The component to set.
     * @param value The value to assign to the component.
     */
    template <typename T>
    const base_type& set(const T& value) const {
        auto comp_id = _::cpp_type<T>::id(world());

        ecs_assert(_::cpp_type<T>::size() != 0, 
            ECS_INVALID_PARAMETER, NULL);

        ecs_set_ptr_w_entity(world(), id(), 
            comp_id, sizeof(T), &value);

        return *base();
    }

    /** Set a pair for an entity.
     * This operation sets the pair value, and uses the relation as type. If the
     * entity did not yet have the pair, it will be added.
     *
     * @tparam Relation The relation part of the pair.
     * @tparam Object The object part of the pair.
     * @param value The value to set.
     */
    template <typename Relation, typename Object>
    const base_type& set(const Relation& value) const {
        auto comp_id = _::cpp_type<Relation>::id(world());

        ecs_assert(_::cpp_type<Relation>::size() != 0, 
            ECS_INVALID_PARAMETER, NULL);

        ecs_set_ptr_w_entity(world(), id(),
            ecs_pair(comp_id, _::cpp_type<Object>::id(world())),
            sizeof(Relation), &value);

        return *base();
    }

    /** Set a pair for an entity.
     * This operation sets the pair value, and uses the relation as type. If the
     * entity did not yet have the pair, it will be added.
     *
     * @tparam Relation The relation part of the pair.
     * @param object The object part of the pair.
     * @param value The value to set.
     */
    template <typename Relation>
    const base_type& set(const Base& object, const Relation& value) const {
        auto comp_id = _::cpp_type<Relation>::id(world());

        ecs_assert(_::cpp_type<Relation>::size() != 0, 
            ECS_INVALID_PARAMETER, NULL);

        ecs_set_ptr_w_entity(world(), id(),
            ecs_pair(comp_id, object.id()),
            sizeof(Relation), &value);

        return *base();
    }    

    /** Set a pair for an entity.
     * This operation sets the pair value, and uses the relation as type. If the
     * entity did not yet have the pair, it will be added.
     *
     * @tparam Object The object part of the pair.
     * @param relation The relation part of the pair.
     * @param value The value to set.
     */
    template <typename Object>
    const base_type& set_object(const Base& relation, const Object& value) const {
        auto comp_id = _::cpp_type<Object>::id(world());

        ecs_assert(_::cpp_type<Object>::size() != 0, 
            ECS_INVALID_PARAMETER, NULL);

        ecs_set_ptr_w_entity(world(), id(),
            ecs_pair(relation.id(), comp_id),
            sizeof(Object), &value);

        return *base();
    }

    /** Patch a component value.
     * This operation allows an application to partially overwrite a component 
     * value. The operation invokes a function with a reference to the value to
     * write, and a boolean indicating if the component already existed.
     *
     * @tparam T The component to patch.
     * @param func The function invoked by this operation.
     */
    template <typename T, typename Func>
    const base_type& patch(const Func& func) const {
        auto comp_id = _::cpp_type<T>::id(world());

        ecs_assert(_::cpp_type<T>::size() != 0, 
            ECS_INVALID_PARAMETER, NULL);

        bool is_added;
        T *ptr = static_cast<T*>(ecs_get_mut_w_entity(
            world(), id(), comp_id, &is_added));
        ecs_assert(ptr != NULL, ECS_INTERNAL_ERROR, NULL);

        func(*ptr);
        ecs_modified_w_entity(world(), id(), comp_id);

        return *base();
    }

private:
    const Base* base() const { return static_cast<const Base*>(this); }
    flecs::world_t* world() const { return base()->world().c_ptr(); }
    flecs::entity_t id() const { return base()->id(); }
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

        ecs_get_ref_w_entity(
            m_world, &m_ref, m_entity, comp_id);
    }

    const T* operator->() {
        const T* result = static_cast<const T*>(ecs_get_ref_w_entity(
            m_world, &m_ref, m_entity, _::cpp_type<T>::id(m_world)));

        ecs_assert(result != NULL, ECS_INVALID_PARAMETER, NULL);

        return result;
    }

    const T* get() {
        if (m_entity) {
            ecs_get_ref_w_entity(
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


////////////////////////////////////////////////////////////////////////////////

/** Entity class
 * This class provides access to entity operations. */
class entity : 
    public entity_builder<entity>, 
    public entity_deprecated<entity>, 
    public entity_builder_deprecated<entity> 
{
public:
    /** Default constructor.
     */
    explicit entity()
        : m_world( nullptr )
        , m_id( 0 ) { }

    /** Create entity.
     *
     * @param world The world in which to create the entity.
     */
    explicit entity(const flecs::world& world) 
        : m_world( world.c_ptr() )
        , m_id( ecs_new_w_type(m_world, 0) ) { }

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
    explicit entity(const flecs::world& world, const char *name, bool is_component = false) 
        : m_world( world.c_ptr() )
        , m_id( ecs_lookup_path_w_sep(m_world, 0, name, "::", "::") ) 
        { 
            if (!m_id) {
                if (is_component) {
                    m_id = ecs_new_component_id(m_world);
                }

                m_id = ecs_add_path_w_sep(
                    m_world, m_id, 0, name, "::", "::");
            }
        }

    /** Wrap an existing entity id.
     *
     * @param world The world in which the entity is created.
     * @param id The entity id.
     */
    explicit entity(const flecs::world& world, const entity& id) 
        : m_world( world.c_ptr() )
        , m_id(id.id()) { }

    /** Wrap an existing entity id.
     *
     * @param world Pointer to the world in which the entity is created.
     * @param id The entity id.
     */
    explicit entity(world_t *world, const entity& id) 
        : m_world( world )
        , m_id(id.id()) { }

    /** Allow conversion from flecs::entity_t to flecs::entity */
    entity(entity_t id)
        : m_world( nullptr )
        , m_id(id) { }

    /** Equality operator. */
    bool operator==(const entity& e) {
        return this->id() == e.id();
    }  

    /** Inequality operator. */
    bool operator!=(const entity& e) {
        return this->id() != e.id();
    }            

    explicit operator bool() {
        return m_id != 0;
    }

    /** Entity id 0.
     * This function is useful when the API must provide an entity object that
     * belongs to a world, but the entity id is 0.
     *
     * @param world The world.
     */
    static
    flecs::entity null(const flecs::world& world) {
        return flecs::entity(world.c_ptr(), (entity_t)0);
    }

    static
    flecs::entity null() {
        return flecs::entity((entity_t)0);
    }    

    /** Get entity id.
     * @return The integer entity id.
     */
    entity_t id() const {
        return m_id;
    }

    /** Get lo entity id.
     * @return A new entity containing the lower 32 bits of the entity id.
     */
    flecs::entity lo() const {
        return flecs::entity(m_world, ecs_entity_t_lo(m_id));
    }

    /** Get hi entity id.
     * @return A new entity containing the higher 32 bits of the entity id.
     */
    flecs::entity hi() const {
        return flecs::entity(m_world, ecs_entity_t_hi(m_id));
    }

    /** Combine two entity ids.
     * @return A new entity that combines the provided entity ids in the lower
     *         and higher 32 bits of the entity id.
     */
    static 
    flecs::entity comb(flecs::entity lo, flecs::entity hi) {
        return flecs::entity(lo.world(), 
            ecs_entity_t_comb(lo.id(), hi.id()));
    }

    /** Add role.
     * Roles are added to entity ids in types to indicate which role they play.
     * Examples of roles are flecs::Instanceof and flecs::Childof. 
     *
     * @return A new entity with the specified role set.
     */
    flecs::entity add_role(entity_t role) const {
        return flecs::entity(m_world, m_id | role);
    }

    /** Remove role.
     * Roles are added to entity ids in types to indicate which role they play.
     * Examples of roles are flecs::Instanceof and flecs::Childof. 
     *    
     * @return A new entity with any roles removed.
     */
    flecs::entity remove_role() const {
        return flecs::entity(m_world, m_id & ECS_COMPONENT_MASK);
    }

    /** Check if entity has specified role.
     * Roles are added to entity ids in types to indicate which role they play.
     * Examples of roles are flecs::Instanceof and flecs::Childof. 
     *    
     * @return True if the entity has the role, false otherwise.
     */
    bool has_role(entity_t role) const {        
        return ((m_id & ECS_ROLE_MASK) == role);
    }

    /** Check is entity is alive.
     *
     * @return True if the entity is alive, false otherwise.
     */
    bool is_alive() {
        return ecs_is_alive(m_world, m_id);
    }

    /** Return the entity name.
     *
     * @return The entity name, or an empty string if the entity has no name.
     */
    flecs::string_view name() const {
        const EcsName *name = static_cast<const EcsName*>(
            ecs_get_w_entity(m_world, m_id, ecs_entity(EcsName)));
        return flecs::string_view(name ? name->value : nullptr);
    }

    /** Return the entity path.
     *
     * @return The hierarchical entity path, or an empty string if the entity 
     *         has no name.
     */
    flecs::string path(const char *sep = "::", const char *init_sep = "::") const {
        char *path = ecs_get_path_w_sep(m_world, 0, m_id, 0, sep, init_sep);
        return flecs::string(path);
    }   

    bool enabled() {
        return !ecs_has_entity(m_world, m_id, flecs::Disabled);
    }

    /** Return the world.
     *
     * @return The world the entity is stored in.
     */
    flecs::world world() const {
        return flecs::world(m_world);
    }

    /** Return the type.
     *
     * @return Returns the entity type.
     */
    flecs::type type() const;

    /** Return type containing the entity.
     *
     * @return A type that contains only this entity.
     */
    flecs::type to_type() const;

    /** Get component value.
     * 
     * @tparam T The component to get.
     * @return Pointer to the component value, nullptr if the entity does not
     *         have the component.
     */
    template <typename T>
    const T* get() const {
        ecs_assert(m_world != NULL, ECS_INVALID_PARAMETER, NULL);
        ecs_assert(m_id != 0, ECS_INVALID_PARAMETER, NULL);

        auto comp_id = _::cpp_type<T>::id(m_world);

        ecs_assert(_::cpp_type<T>::size() != 0, 
                ECS_INVALID_PARAMETER, NULL);

        return static_cast<const T*>(
            ecs_get_w_entity(m_world, m_id, comp_id));
    }

    /** Get component value (untyped).
     * 
     * @param component The component to get.
     * @return Pointer to the component value, nullptr if the entity does not
     *         have the component.
     */
    const void* get(flecs::entity component) const {
        ecs_assert(m_world != NULL, ECS_INVALID_PARAMETER, NULL);
        ecs_assert(m_id != 0, ECS_INVALID_PARAMETER, NULL);
        return ecs_get_w_entity(m_world, m_id, component.id());
    }

    /** Get component value (untyped).
     * 
     * @param component The id of the component to get.
     * @return Pointer to the component value, nullptr if the entity does not
     *         have the component.
     */
    const void* get(entity_t component_id) const {
        ecs_assert(m_world != NULL, ECS_INVALID_PARAMETER, NULL);
        ecs_assert(m_id != 0, ECS_INVALID_PARAMETER, NULL);
        return ecs_get_w_entity(m_world, m_id, component_id);
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
        ecs_assert(m_world != NULL, ECS_INVALID_PARAMETER, NULL);
        ecs_assert(m_id != 0, ECS_INVALID_PARAMETER, NULL);

        auto comp_id = _::cpp_type<T>::id(m_world);

        ecs_assert(_::cpp_type<T>::size() != 0, 
                ECS_INVALID_PARAMETER, NULL);

        return static_cast<T*>(
            ecs_get_mut_w_entity(m_world, m_id, comp_id, is_added));
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
    void* get_mut(flecs::entity component, bool *is_added = nullptr) const {
        ecs_assert(m_world != NULL, ECS_INVALID_PARAMETER, NULL);
        ecs_assert(m_id != 0, ECS_INVALID_PARAMETER, NULL);
        return ecs_get_mut_w_entity(m_world, m_id, component.id(), is_added);
    }

    /** Get mutable component value (untyped).
     * This operation returns a mutable pointer to the component. If the entity
     * did not yet have the component, it will be added. If a base entity had
     * the component, it will be overridden, and the value of the base component
     * will be copied to the entity before this function returns.
     *
     * @param component The id of the component to get.
     * @param is_added If provided, this parameter will be set to true if the component was added.
     * @return Pointer to the component value.
     */
    void* get_mut(entity_t component_id, bool *is_added = nullptr) const {
        ecs_assert(m_world != NULL, ECS_INVALID_PARAMETER, NULL);
        ecs_assert(m_id != 0, ECS_INVALID_PARAMETER, NULL);
        return ecs_get_mut_w_entity(m_world, m_id, component_id, is_added);
    }

    /** Signal that component was modified.
     *
     * @tparam T component that was modified.
     */
    template <typename T>
    void modified() const {
        ecs_assert(m_world != NULL, ECS_INVALID_PARAMETER, NULL);
        ecs_assert(m_id != 0, ECS_INVALID_PARAMETER, NULL);

        auto comp_id = _::cpp_type<T>::id(m_world);

        ecs_assert(_::cpp_type<T>::size() != 0, 
                ECS_INVALID_PARAMETER, NULL);

        ecs_modified_w_entity(m_world, m_id, comp_id);
    }

    /** Signal that component was modified.
     *
     * @param component component that was modified.
     */
    void modified(flecs::entity component) const {
        ecs_assert(m_world != NULL, ECS_INVALID_PARAMETER, NULL);
        ecs_assert(m_id != 0, ECS_INVALID_PARAMETER, NULL);
        ecs_modified_w_entity(m_world, m_id, component.id());
    }

    /** Signal that component was modified.
     *
     * @param component id of component that was modified.
     */
    void modified(entity_t component) const {
        ecs_assert(m_world != NULL, ECS_INVALID_PARAMETER, NULL);
        ecs_assert(m_id != 0, ECS_INVALID_PARAMETER, NULL);
        ecs_modified_w_entity(m_world, m_id, component);
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
        ecs_assert(m_world != NULL, ECS_INVALID_PARAMETER, NULL);
        ecs_assert(m_id != 0, ECS_INVALID_PARAMETER, NULL);

        // Ensure component is registered
        _::cpp_type<T>::id(m_world);

        ecs_assert(_::cpp_type<T>::size() != 0, 
                ECS_INVALID_PARAMETER, NULL);

        return ref<T>(m_world, m_id);
    }

    /** Get parent from an entity.
     * This operation retrieves the parent entity that has the specified 
     * component. If no parent with the specified component is found, an entity
     * with id 0 is returned. If multiple parents have the specified component,
     * the operation returns the first encountered one.
     *
     * @tparam T The component for which to find the parent.
     * @return The parent entity.
     */
    template <typename T>
    flecs::entity get_parent() {
        return flecs::entity(m_world, ecs_get_parent_w_entity(m_world, m_id, 
            _::cpp_type<T>::id(m_world)));
    }

    flecs::entity get_parent(flecs::entity e) {
        return flecs::entity(m_world, ecs_get_parent_w_entity(m_world, m_id, e.id()));
    }    

    /** Clear an entity.
     * This operation removes all components from an entity without recycling
     * the entity id.
     */
    void clear() const {
        ecs_assert(m_world != NULL, ECS_INVALID_PARAMETER, NULL);
        ecs_assert(m_id != 0, ECS_INVALID_PARAMETER, NULL);
        ecs_clear(m_world, m_id);
    }

    /** Delete an entity.
     * Entities have to be deleted explicitly, and are not deleted when the
     * flecs::entity object goes out of scope.
     */
    void destruct() const {
        ecs_assert(m_world != NULL, ECS_INVALID_PARAMETER, NULL);
        ecs_assert(m_id != 0, ECS_INVALID_PARAMETER, NULL);
        ecs_delete(m_world, m_id);
    }

    /** Lookup an entity by name.
     * Lookup an entity in the scope of this entity. The provided path may
     * contain double colons as scope separators, for example: "Foo::Bar".
     *
     * @param path The name of the entity to lookup.
     * @return The found entity, or entity::null if no entity matched.
     */
    entity lookup(const char *path) const {
        ecs_assert(m_world != NULL, ECS_INVALID_PARAMETER, NULL);
        ecs_assert(m_id != 0, ECS_INVALID_PARAMETER, NULL);
        auto id = ecs_lookup_path_w_sep(m_world, m_id, path, "::", "::");
        return entity(m_world, id);
    }

    /** Check if entity has the provided entity.
     *
     * @param entity The entity id to check.
     * @return True if the entity has the provided entity id, false otherwise.
     */
    bool has(entity_t e) const {
        ecs_assert(m_world != NULL, ECS_INVALID_PARAMETER, NULL);
        ecs_assert(m_id != 0, ECS_INVALID_PARAMETER, NULL);
        return ecs_has_entity(m_world, m_id, e);
    }

    /** Check if entity has the provided parent.
     *
     * @param parent The parent id to check.
     * @return True if the entity has the provided parent id, false otherwise.
     */
    bool has_childof(entity_t parent) const {
        ecs_assert(m_world != NULL, ECS_INVALID_PARAMETER, NULL);
        ecs_assert(m_id != 0, ECS_INVALID_PARAMETER, NULL);
        return ecs_has_entity(m_world, m_id, ECS_CHILDOF | parent);
    }    

    /** Check if entity has the provided base.
     *
     * @param base The entity id to check.
     * @return True if the entity has the provided base id, false otherwise.
     */
    bool has_instanceof(entity_t base) const {
        ecs_assert(m_world != NULL, ECS_INVALID_PARAMETER, NULL);
        ecs_assert(m_id != 0, ECS_INVALID_PARAMETER, NULL);
        return ecs_has_entity(m_world, m_id, ECS_INSTANCEOF | base);
    }

    /** Check if entity has the provided type.
     *
     * @param entity The type pointer to check.
     * @return True if the entity has the provided type, false otherwise.
     */
    bool has(type_t type) const {
        ecs_assert(m_world != NULL, ECS_INVALID_PARAMETER, NULL);
        ecs_assert(m_id != 0, ECS_INVALID_PARAMETER, NULL);
        return ecs_has_type(m_world, m_id, type);
    }

    /** Check if entity has the provided entity.
     *
     * @param entity The entity to check.
     * @return True if the entity has the provided entity, false otherwise.
     */
    bool has(const entity& e) const {
        return has(e.id());
    }

    /** Check if entity has the provided parent.
     *
     * @param parent The entity to check.
     * @return True if the entity has the provided parent, false otherwise.
     */
    bool has_childof(const entity& parent) const {
        return has_childof(parent.id());
    }  

    /** Check if entity has the provided base.
     *
     * @param base The entity to check.
     * @return True if the entity has the provided base, false otherwise.
     */
    bool has_instanceof(const entity& base) const {
        return has_instanceof(base.id());
    }        

    /** Check if entity has the provided component.
     *
     * @tparam T The component to check.
     * @return True if the entity has the provided component, false otherwise.
     */
    template <typename T>
    bool has() const {
        return has(_::cpp_type<T>::id(m_world));
    }

    /** Check if entity owns the provided entity id.
     * An entity id is owned if it is not shared from a base entity.
     *
     * @param entity The entity id to check.
     * @return True if the entity owns the provided entity id, false otherwise.
     */
    bool owns(entity_t e) const {
        ecs_assert(m_world != NULL, ECS_INVALID_PARAMETER, NULL);
        ecs_assert(m_id != 0, ECS_INVALID_PARAMETER, NULL);
        return ecs_owns_entity(m_world, m_id, e, true);
    }

    /** Check if entity owns the provided type.
     * An type is owned if it is not shared from a base entity.
     *
     * @param type The type to check.
     * @return True if the entity owns the provided type, false otherwise.
     */
    bool owns(type_t type) const {
        ecs_assert(m_world != NULL, ECS_INVALID_PARAMETER, NULL);
        ecs_assert(m_id != 0, ECS_INVALID_PARAMETER, NULL);
        return ecs_type_owns_type(m_world, ecs_get_type(m_world, m_id), type, true);
    }

    /** Check if entity owns the provided entity.
     * An entity is owned if it is not shared from a base entity.
     *
     * @param entity The entity to check.
     * @return True if the entity owns the provided entity, false otherwise.
     */
    bool owns(const entity& e) const {
        return owns(e.id());
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

    /** Check if entity has the provided trait.
     *
     * @tparam T The trait to check.
     * @tparam C The component for which to check the trait.
     * @return True if the entity has the provided trait, false otherwise.
     */
    template<typename T, typename C>
    bool has_trait() const {
        ecs_assert(m_world != NULL, ECS_INVALID_PARAMETER, NULL);
        ecs_assert(m_id != 0, ECS_INVALID_PARAMETER, NULL);
        return ecs_has_entity(m_world, m_id, ecs_trait(
            _::cpp_type<C>::id(m_world), 
            _::cpp_type<T>::id(m_world)));
    }

    /** Check if entity has the provided trait.
     *
     * @tparam T The trait to check.
     * @param component The component for which to check the trait.
     * @return True if the entity has the provided trait, false otherwise.
     */
    template<typename T>
    bool has_trait(flecs::entity component) const {
        ecs_assert(m_world != NULL, ECS_INVALID_PARAMETER, NULL);
        ecs_assert(m_id != 0, ECS_INVALID_PARAMETER, NULL);
        return ecs_has_entity(m_world, m_id, ecs_trait(
            component.id(), _::cpp_type<T>::id(m_world)));
    }

    /** Check if entity has the provided trait tag.
     * The provided trait tag should not be a component.
     *
     * @tparam C The component for which to check the trait tag.
     * @param trait The trait tag to check.
     * @return True if the entity has the provided trait tag, false otherwise.
     */
    template<typename C>
    bool has_trait_tag(flecs::entity trait) const {
        ecs_assert(m_world != NULL, ECS_INVALID_PARAMETER, NULL);
        ecs_assert(m_id != 0, ECS_INVALID_PARAMETER, NULL);
        return ecs_has_entity(m_world, m_id, ecs_trait(
           _::cpp_type<C>::id(m_world), trait.id()));
    }

    /** Check if entity has the provided trait.
     * The provided trait should not be a component.
     *
     * @param trait The trait to check.
     * @param component The component for which to check the trait.
     * @return True if the entity has the provided trait, false otherwise.
     */
    bool has_trait(flecs::entity trait, flecs::entity e) const {
        ecs_assert(m_world != NULL, ECS_INVALID_PARAMETER, NULL);
        ecs_assert(m_id != 0, ECS_INVALID_PARAMETER, NULL);
        return ecs_has_entity(m_world, m_id, ecs_trait(
            e.id(), trait.id()));
    }

    /** Check if entity has the provided switch.
     *
     * @param sw The switch to check.
     * @return True if the entity has the provided switch, false otherwise.
     */
    bool has_switch(const flecs::type& sw) const;

    /** Check if entity has the provided case id.
     *
     * @param sw_case The case id to check.
     * @return True if the entity has the provided case, false otherwise.
     */
    bool has_case(entity_t sw_case) const {
        ecs_assert(m_world != NULL, ECS_INVALID_PARAMETER, NULL);
        ecs_assert(m_id != 0, ECS_INVALID_PARAMETER, NULL);
        return ecs_has_entity(m_world, m_id, flecs::Case | sw_case);
    }

    /** Check if entity has the provided case.
     *
     * @param sw_case The case to check.
     * @return True if the entity has the provided case, false otherwise.
     */
    bool has_case(flecs::entity sw_case) const {
        return this->has_case(sw_case.id());
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
    flecs::entity get_case(const flecs::type& sw) const;

    /** Test if component is enabled.
     *
     * @tparam T The component to test.
     * @return True if the component is enabled, false if it has been disabled.
     */
    template<typename T>
    bool is_enabled() {
        return ecs_is_component_enabled_w_entity(
            m_world, m_id, _::cpp_type<T>::id(m_world));
    }

    /** Test if component is enabled.
     *
     * @param id The component to test.
     * @return True if the component is enabled, false if it has been disabled.
     */
    bool is_enabled(flecs::entity_t id) {
        return ecs_is_component_enabled_w_entity(
            m_world, m_id, id);
    }

    /** Test if component is enabled.
     *
     * @param entity The component to test.
     * @return True if the component is enabled, false if it has been disabled.
     */
    bool is_enabled(const flecs::entity& e) {
        return is_enabled(e.id());
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
    child_iterator children() const;

    /** Used by builder class. Do not invoke. */
    template <typename Func>
    void invoke(Func&& action) const {
        action(m_world, m_id);
    }    

protected:
    world_t *m_world;
    entity_t m_id; 
};

/** Prefab class */
class prefab final : public entity {
public:
    explicit prefab(const flecs::world& world, const char *name = nullptr) 
        : entity(world, name)
    {
        this->add(flecs::Prefab);
    }
};

} // namespace flecs
