
#ifdef FLECS_DEPRECATED
#include "../addons/deprecated/entity.hpp"
#else
template <typename Base>
class entity_builder_deprecated { };
class entity_deprecated { };
#endif

namespace flecs 
{

/** Class that stores a flecs id.
 * A flecs id is an identifier that can store an entity id, an relation-object 
 * pair, or role annotated id (such as SWITCH | Movement).
 */
class id {
public:
    id() 
        : m_world(nullptr)
        , m_id(0) { }

    explicit id(flecs::id_t value) 
        : m_world(nullptr)
        , m_id(value) { }

    explicit id(flecs::world_t *world, flecs::id_t value) 
        : m_world(world)
        , m_id(value) { }

    explicit id(flecs::world_t *world, flecs::id_t relation, flecs::id_t object)
        : m_world(world)
        , m_id(ecs_pair(relation, object)) { }

    explicit id(flecs::id_t relation, flecs::id_t object)
        : m_world(nullptr)
        , m_id(ecs_pair(relation, object)) { }

    explicit id(const flecs::id& relation, const flecs::id& object)
        : m_world(relation.world())
        , m_id(ecs_pair(relation.m_id, object.m_id)) { }

    /** Test if id is pair (has relation, object) */
    bool is_pair() const {
        return (m_id & ECS_ROLE_MASK) == flecs::Pair;
    }

    /* Test if id has the Switch role */
    bool is_switch() const {
        return (m_id & ECS_ROLE_MASK) == flecs::Switch;
    }

    /* Test if id has the Case role */
    bool is_case() const {
        return (m_id & ECS_ROLE_MASK) == flecs::Case;
    }

    /* Return id with role added */
    flecs::id add_role(flecs::id_t role) const {
        return flecs::id(m_world, m_id | role);
    }

    /* Return id with role removed */
    flecs::entity remove_role(flecs::id_t role) const;

    /* Return id without role */
    flecs::entity remove_role() const;

    /* Test if id has specified role */
    bool has_role(flecs::id_t role) const {
        return ((m_id & ECS_ROLE_MASK) == role);
    }

    /* Test if id has any role */
    bool has_role() const {
        return (m_id & ECS_ROLE_MASK) != 0;
    }

    flecs::entity role() const;

    /* Test if id has specified relation */
    bool has_relation(flecs::id_t relation) const {
        if (!is_pair()) {
            return false;
        }
        return ECS_PAIR_RELATION(m_id) == relation;
    }

    /** Get relation from pair.
     * If the id is not a pair, this operation will fail. When the id has a
     * world, the operation will ensure that the returned id has the correct
     * generation count.
     */
    flecs::entity relation() const;

    /** Get object from pair.
     * If the id is not a pair, this operation will fail. When the id has a
     * world, the operation will ensure that the returned id has the correct
     * generation count.
     */
    flecs::entity object() const;

    /** Get world. */
    flecs::world_t* world() const {
        return m_world;
    }

    /** Convert id to string. */
    flecs::string role_str() const {
        return flecs::string_view( ecs_role_str(m_id & ECS_ROLE_MASK));
    }

protected:
    /* World is optional, but guarantees that entity identifiers extracted from
     * the id are valid */
    flecs::world_t *m_world;
    flecs::id_t m_id;
};

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
        return this->add<Relation>(_::cpp_type<Object>::id(world()));
    }

    /** Add a pair.
     * This operation adds a pair to the entity.
     *
     * @tparam Relation the relation type.
     * @param object the object type.
     */
    template<typename Relation>
    base_type& add(const Base& object) const {
        return this->add(_::cpp_type<Relation>::id(world()), object.id());
    }    

    /** Add a pair with object type.
     * This operation adds a pair to the entity. The relation part of the pair
     * should not be a component.
     *
     * @param relation the relation type.
     * @tparam Object the object type.
     */
    template<typename Object>
    base_type& add_object(const Base& relation) const {
        return this->add(relation.id(),  _::cpp_type<Object>::id(world()));
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
        return this->remove<Relation>(_::cpp_type<Object>::id(world()));
    }

    /** Remove a pair.
     * This operation adds a pair to the entity.
     *
     * @tparam Relation the relation type.
     * @param object the object type.
     */
    template<typename Relation>
    base_type& remove(const Base& object) const {
        return this->remove(_::cpp_type<Relation>::id(world()), object.id());
    }  

    /** Removes a pair with object type.
     * This operation removes a pair from the entity.
     *
     * @param relation the relation type.
     * @tparam Object the object type.
     */
    template<typename Object>
    base_type& remove_object(const Base& relation) const {
        return this->remove(relation.id(), _::cpp_type<Object>::id(world()));
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

        T& ptr = *static_cast<T*>(
            ecs_get_mut_w_id(world(), id(), comp_id, NULL));

        ptr = std::move(value);

        ecs_modified_w_id(world(), id(), comp_id);

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
    const base_type& set(T&& value) const {
        auto comp_id = _::cpp_type<T>::id(world());

        ecs_assert(_::cpp_type<T>::size() != 0, ECS_INVALID_PARAMETER, NULL);

        T& ptr = *static_cast<T*>(
            ecs_get_mut_w_id(world(), id(), comp_id, NULL));

        ptr = std::move(value);

        ecs_modified_w_id(world(), id(), comp_id);

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
    public id,
    public entity_builder<entity>, 
    public entity_deprecated<entity>, 
    public entity_builder_deprecated<entity> 
{
public:
    /** Default constructor.
     */
    explicit entity()
        : flecs::id() { }

    /** Create entity.
     *
     * @param world The world in which to create the entity.
     */
    explicit entity(const flecs::world& world) 
        : flecs::id( world.get_world(), ecs_new_w_type(world.c_ptr(), 0) ) { }

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
        : flecs::id( world.get_world(), 0)
        { 
            m_id = ecs_lookup_path_w_sep(m_world, 0, name, "::", "::");

            if (!m_id) {
                if (is_component) {
                    m_id = ecs_new_component_id(m_world);
                }

                m_id = ecs_add_path_w_sep(
                    world.c_ptr(), m_id, 0, name, "::", "::");
            }
        }

    /** Wrap an existing entity id.
     *
     * @param world The world in which the entity is created.
     * @param id The entity id.
     */
    explicit entity(const flecs::world& world, const entity& id)
        : flecs::id( world.get_world(), id.id() ) { }

    /** Wrap an existing entity id.
     *
     * @param world Pointer to the world in which the entity is created.
     * @param id The entity id.
     */
    explicit entity(world_t *world, const entity& id) 
        : flecs::id( flecs::world(world).get_world(), id.id() ) { }

    /** Implicit conversion from flecs::entity_t to flecs::entity. */
    entity(entity_t id) 
        : flecs::id( nullptr, id ) { }

    /** Implicit conversion from flecs::id to flecs::entity. */
    entity(flecs::id value) 
        : flecs::id( value ) { }

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
        return flecs::entity(world.get_world().c_ptr(), static_cast<entity_t>(0));
    }

    static
    flecs::entity null() {
        return flecs::entity(static_cast<entity_t>(0));
    }

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
    bool is_valid() {
        return ecs_is_valid(m_world, m_id);
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
            ecs_get_w_entity(m_world, m_id, static_cast<ecs_entity_t>(ecs_id(EcsName))));
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
    flecs::entity mut(const flecs::world& stage) const {
        ecs_assert(!stage.is_readonly(), ECS_INVALID_PARAMETER, 
            "cannot use readonly world/stage to create mutable handle");
        return flecs::entity(m_id).set_stage(stage.c_ptr());
    }

    /** Same as mut(world), but for iterator.
     * This operation allows for the construction of a mutable entity handle
     * from an iterator.
     *
     * @param stage An created for the current stage.
     * @return An entity handle that allows for mutations in the current stage.
     */
    flecs::entity mut(const flecs::iter& it) const {
        ecs_assert(!it.world().is_readonly(), ECS_INVALID_PARAMETER, 
            "cannot use iterator created for readonly world/stage to create mutable handle");
        return flecs::entity(m_id).set_stage(it.world().c_ptr());
    }

    /** Same as mut(world), but for entity.
     * This operation allows for the construction of a mutable entity handle
     * from another entity. This is useful in each() functions, which only 
     * provide a handle to the entity being iterated over.
     *
     * @param stage An created for the current stage.
     * @return An entity handle that allows for mutations in the current stage.
     */
    flecs::entity mut(const flecs::entity& e) const {
        ecs_assert(!e.world().is_readonly(), ECS_INVALID_PARAMETER, 
            "cannot use entity created for readonly world/stage to create mutable handle");
        return flecs::entity(m_id).set_stage(e.m_world);
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

    /** Iterate contents (type) of an entity.
     */
    template <typename Func>
    void each(const Func& func) const {
        const ecs_vector_t *type = ecs_get_type(m_world, m_id);
        if (!type) {
            return;
        }

        const ecs_id_t *ids = static_cast<ecs_id_t*>(
            _ecs_vector_first(type, ECS_VECTOR_T(ecs_id_t)));
        int32_t count = ecs_vector_count(type);

        for (int i = 0; i < count; i ++) {
            ecs_id_t id = ids[i];
            flecs::entity ent(m_world, id);
            func(ent); 

            // Case is not stored in type, so handle separately
            if ((id & ECS_ROLE_MASK) == flecs::Switch) {
                ent = flecs::entity(
                    m_world, flecs::Case | ecs_get_case(
                            m_world, m_id, ent.object().id()));
                func(ent);
            }
        }
    }

    /** Iterate contents (type) of an entity for a specific relationship.
     */
    template <typename Func>
    void each(flecs::entity_t rel, const Func& func) const {
        const ecs_vector_t *type = ecs_get_type(m_world, m_id);
        if (!type) {
            return;
        }

        const ecs_id_t *ids = static_cast<ecs_id_t*>(
            _ecs_vector_first(type, ECS_VECTOR_T(ecs_id_t)));
        int32_t count = ecs_vector_count(type);

        // First, skip to the point where the relationship starts
        // TODO: replace this with an O(1) search when the new table lookup
        //       datastructures land
        int i;
        for (i = 0; i < count; i ++) {
            ecs_id_t id = ids[i];
            if (ECS_PAIR_RELATION(id) == rel) {
                break;
            }
        }

        // Iterate all entries until the relationship stops
        for (; i < count; i ++) {
            ecs_id_t id = ids[i];
            if (ECS_PAIR_RELATION(id) != rel) {
                break;
            }

            flecs::id id_cl(m_world, ids[i]);
            flecs::entity ent(m_world, id_cl.object());
            func(ent);         
        }
    }    

    /** Get component value.
     * 
     * @tparam T The component to get.
     * @return Pointer to the component value, nullptr if the entity does not
     *         have the component.
     */
    template <typename T>
    const T* get() const {
        auto comp_id = _::cpp_type<T>::id(m_world);
        ecs_assert(_::cpp_type<T>::size() != 0, ECS_INVALID_PARAMETER, NULL);
        return static_cast<const T*>(
            ecs_get_w_entity(m_world, m_id, comp_id));
    }

    /** Get component value (untyped).
     * 
     * @param component The component to get.
     * @return Pointer to the component value, nullptr if the entity does not
     *         have the component.
     */
    const void* get(const flecs::entity& component) const {
        return ecs_get_w_entity(m_world, m_id, component.id());
    }

    /** Get a pair.
     * This operation gets the value for a pair from the entity.
     *
     * @tparam Relation the relation type.
     * @tparam Object the object type.
     */
    template<typename Relation, typename Object>
    const Relation* get() const {
        return this->get<Relation>(_::cpp_type<Object>::id(m_world));
    }

    /** Get a pair.
     * This operation gets the value for a pair from the entity. 
     *
     * @tparam Relation the relation type.
     * @param object the object.
     */
    template<typename Relation>
    const Relation* get(const flecs::entity& object) const {
        auto comp_id = _::cpp_type<Relation>::id(m_world);
        ecs_assert(_::cpp_type<Relation>::size() != 0, ECS_INVALID_PARAMETER, NULL);
        return static_cast<const Relation*>(
            ecs_get_w_entity(m_world, m_id, ecs_pair(comp_id, object.id())));
    }

    /** Get a pair (untyped).
     * This operation gets the value for a pair from the entity. If neither the
     * relation nor the object part of the pair are components, the operation 
     * will fail.
     *
     * @param relation the relation.
     * @param object the object.
     */
    const void* get(const flecs::entity& relation, const flecs::entity& object) const {
        return ecs_get_w_entity(m_world, m_id, ecs_pair(relation.id(), object.id()));
    }

    /** Get the object part from a pair.
     * This operation gets the value for a pair from the entity. The relation
     * part of the pair should not be a component.
     *
     * @tparam Object the object type.
     * @param relation the relation.
     */
    template<typename Object>
    const Object* get_object(const flecs::entity& relation) const {
        auto comp_id = _::cpp_type<Object>::id(m_world);
        ecs_assert(_::cpp_type<Object>::size() != 0, ECS_INVALID_PARAMETER, NULL);
        return static_cast<const Object*>(
            ecs_get_w_entity(m_world, m_id, ecs_pair(relation.id(), comp_id)));
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
        return ecs_get_mut_w_entity(m_world, m_id, component_id, is_added);
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
    Relation* get_mut(const flecs::entity& object, bool *is_added = nullptr) const {
        auto comp_id = _::cpp_type<Relation>::id(m_world);
        ecs_assert(_::cpp_type<Relation>::size() != 0, ECS_INVALID_PARAMETER, NULL);
        return static_cast<Relation*>(
            ecs_get_mut_w_entity(m_world, m_id, 
                ecs_pair(comp_id, object.id()), is_added));
    }

    /** Get mutable pointer for a pair (untyped).
     * This operation gets the value for a pair from the entity. If neither the
     * relation or object are a component, the operation will fail.
     *
     * @param relation the relation.
     * @param object the object.
     */
    void* get_mut(const flecs::entity& relation, const flecs::entity& object, bool *is_added = nullptr) const {
        return ecs_get_mut_w_entity(m_world, m_id, 
                ecs_pair(relation.id(), object.id()), is_added);
    }

    /** Get mutable pointer for the object from a pair.
     * This operation gets the value for a pair from the entity.
     *
     * @tparam Object the object type.
     * @param relation the relation.
     */
    template <typename Object>
    Object* get_mut_object(const flecs::entity& relation, bool *is_added = nullptr) const {
        auto comp_id = _::cpp_type<Object>::id(m_world);
        ecs_assert(_::cpp_type<Object>::size() != 0, ECS_INVALID_PARAMETER, NULL);
        return static_cast<Object*>(
            ecs_get_mut_w_entity(m_world, m_id, 
                ecs_pair(relation.id(), comp_id), is_added));
    }           

    /** Signal that component was modified.
     *
     * @tparam T component that was modified.
     */
    template <typename T>
    void modified() const {
        auto comp_id = _::cpp_type<T>::id(m_world);
        ecs_assert(_::cpp_type<T>::size() != 0, ECS_INVALID_PARAMETER, NULL);
        ecs_modified_w_entity(m_world, m_id, comp_id);
    }

    /** Signal that component was modified.
     *
     * @param component component that was modified.
     */
    void modified(const flecs::entity& component) const {
        ecs_modified_w_entity(m_world, m_id, component.id());
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
    void modified(const flecs::entity& object) const {
        auto comp_id = _::cpp_type<Relation>::id(m_world);
        ecs_assert(_::cpp_type<Relation>::size() != 0, ECS_INVALID_PARAMETER, NULL);
        ecs_modified_w_entity(m_world, m_id, ecs_pair(comp_id, object.id()));
    }

    /** Signal that a pair has modified (untyped).
     * If neither the relation or object part of the pair are a component, the
     * operation will fail.
     *
     * @param relation the relation.
     * @param object the object.
     */
    void modified(const flecs::entity& relation, const flecs::entity& object) const {
        ecs_modified_w_entity(m_world, m_id, 
            ecs_pair(relation.id(), object.id()));
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
        ecs_clear(m_world, m_id);
    }

    /** Delete an entity.
     * Entities have to be deleted explicitly, and are not deleted when the
     * flecs::entity object goes out of scope.
     */
    void destruct() const {
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
        auto id = ecs_lookup_path_w_sep(m_world, m_id, path, "::", "::");
        return entity(m_world, id);
    }

    /** Check if entity has the provided type.
     *
     * @param entity The type pointer to check.
     * @return True if the entity has the provided type, false otherwise.
     */
    bool has(type_t type) const {
        return ecs_has_type(m_world, m_id, type);
    }

    /** Check if entity has the provided entity.
     *
     * @param entity The entity to check.
     * @return True if the entity has the provided entity, false otherwise.
     */
    bool has(const entity& e) const {
        return ecs_has_entity(m_world, m_id, e.id());
    }     

    /** Check if entity has the provided component.
     *
     * @tparam T The component to check.
     * @return True if the entity has the provided component, false otherwise.
     */
    template <typename T>
    bool has() const {
        return ecs_has_entity(m_world, m_id, _::cpp_type<T>::id(m_world));
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
    bool has(const flecs::entity& object) const {
        auto comp_id = _::cpp_type<Relation>::id(m_world);
        ecs_assert(_::cpp_type<Relation>::size() != 0, 
            ECS_INVALID_PARAMETER, NULL);
        return ecs_has_entity(m_world, m_id, 
            ecs_pair(comp_id, object.id()));
    }

    /** Check if entity has the provided pair.
     *
     * @param relation The relation.
     * @param object The object.
     * @return True if the entity has the provided component, false otherwise.
     */
    bool has(const flecs::entity& relation, const flecs::entity& object) const {
        return ecs_has_entity(m_world, m_id, 
            ecs_pair(relation.id(), object.id()));
    }

    /** Check if entity has the provided pair.
     *
     * @tparam Object The object type.
     * @param relation The relation.
     * @return True if the entity has the provided component, false otherwise.
     */
    template <typename Object>
    bool has_object(const flecs::entity& relation) const {
        auto comp_id = _::cpp_type<Object>::id(m_world);
        ecs_assert(_::cpp_type<Object>::size() != 0, 
            ECS_INVALID_PARAMETER, NULL);
        return ecs_has_entity(m_world, m_id, 
            ecs_pair(relation.id(), comp_id));
    }

    /** Check if entity owns the provided type.
     * An type is owned if it is not shared from a base entity.
     *
     * @param type The type to check.
     * @return True if the entity owns the provided type, false otherwise.
     */
    bool owns(type_t type) const {
        return ecs_type_owns_type(
            m_world, ecs_get_type(m_world, m_id), type, true);
    }

    /** Check if entity owns the provided entity.
     * An entity is owned if it is not shared from a base entity.
     *
     * @param entity The entity to check.
     * @return True if the entity owns the provided entity, false otherwise.
     */
    bool owns(const flecs::entity& e) const {
        return ecs_owns_entity(m_world, m_id, e.id(), true);
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

    /** Check if entity has the provided case.
     *
     * @param sw_case The case to check.
     * @return True if the entity has the provided case, false otherwise.
     */
    bool has_case(const flecs::entity& sw_case) const {
        return ecs_has_entity(m_world, m_id, flecs::Case | sw_case.id());
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

    /** Get case for switch.
     *
     * @param sw The switch for which to obtain the case.
     * @return True if the entity has the provided case, false otherwise.
     */
    flecs::entity get_case(const flecs::entity& sw) const {
        return flecs::entity(m_world, ecs_get_case(m_world, m_id, sw.id()));
    }

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
     * @param entity The component to test.
     * @return True if the component is enabled, false if it has been disabled.
     */
    bool is_enabled(const flecs::entity& e) {
        return ecs_is_component_enabled_w_entity(
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
    child_iterator children() const;

    /** Used by builder class. Do not invoke. */
    template <typename Func>
    void invoke(Func&& action) const {
        action(m_world, m_id);
    }    

protected:
    flecs::entity set_stage(world_t *stage) {
        m_world = stage;
        return *this;
    }
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
