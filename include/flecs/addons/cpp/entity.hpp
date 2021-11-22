#include "builders/entity_i.hpp"

namespace flecs
{

/** Entity class
 * This class provides access to entities. */
class entity : 
    public entity_view,
    public entity_builder_i<entity>
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

    /** Conversion from flecs::entity_t to flecs::entity. 
     * 
     * @param id The entity_t value to convert.
     */
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
     * @param comp The component to get.
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
     * @param comp component that was modified.
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

protected:
    flecs::world_t* world_v() override {
        return m_world;
    }

    flecs::entity_t id_v() override {
        return m_id;
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
