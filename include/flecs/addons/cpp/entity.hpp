#pragma once

#include "entity_view.hpp"
#include "mixins/entity/builder.hpp"

namespace flecs
{

/** Entity class
 * This class provides access to entities. */
struct entity : entity_builder<entity>
{
    entity() : entity_builder<entity>() { }

    /** Create entity.
     *
     * @param world The world in which to create the entity.
     */
    explicit entity(world_t *world) 
        : entity_builder() 
    {
        m_world = world;
        m_id = ecs_new(world, 0);
    }

    /** Wrap an existing entity id.
     *
     * @param world The world in which the entity is created.
     * @param id The entity id.
     */
    explicit entity(const flecs::world_t *world, flecs::id_t id) {
        m_world = const_cast<flecs::world_t*>(world);
        m_id = id;
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
        : entity_builder()
    { 
        m_world = world;

        ecs_entity_desc_t desc = {};
        desc.name = name;
        desc.sep = "::";
        desc.root_sep = "::";
        m_id = ecs_entity_init(world, &desc);
    }

    /** Conversion from flecs::entity_t to flecs::entity. 
     * 
     * @param id The entity_t value to convert.
     */
    explicit entity(entity_t id) 
        : entity_builder( nullptr, id ) { }

    /** Get mutable component value.
     * This operation returns a mutable pointer to the component. If the entity
     * did not yet have the component, it will be added. If a base entity had
     * the component, it will be overridden, and the value of the base component
     * will be copied to the entity before this function returns.
     *
     * @tparam T The component to get.
     * @return Pointer to the component value.
     */
    template <typename T>
    T* get_mut() const {
        auto comp_id = _::cpp_type<T>::id(m_world);
        ecs_assert(_::cpp_type<T>::size() != 0, ECS_INVALID_PARAMETER, NULL);
        return static_cast<T*>(ecs_get_mut_id(m_world, m_id, comp_id));
    }

    /** Get mutable component value (untyped).
     * This operation returns a mutable pointer to the component. If the entity
     * did not yet have the component, it will be added. If a base entity had
     * the component, it will be overridden, and the value of the base component
     * will be copied to the entity before this function returns.
     *
     * @param comp The component to get.
     * @return Pointer to the component value.
     */
    void* get_mut(entity_t comp) const {
        return ecs_get_mut_id(m_world, m_id, comp);
    }

    /** Get mutable pointer for a pair.
     * This operation gets the value for a pair from the entity.
     *
     * @tparam Relation the relation type.
     * @tparam Object the object type.
     */
    template <typename Relation, typename Object>
    Relation* get_mut() const {
        return this->get_mut<Relation>(_::cpp_type<Object>::id(m_world));
    }

    /** Get mutable pointer for a pair.
     * This operation gets the value for a pair from the entity.
     *
     * @tparam Relation the relation type.
     * @param object the object.
     */
    template <typename Relation>
    Relation* get_mut(entity_t object) const {
        auto comp_id = _::cpp_type<Relation>::id(m_world);
        ecs_assert(_::cpp_type<Relation>::size() != 0, ECS_INVALID_PARAMETER, NULL);
        return static_cast<Relation*>(
            ecs_get_mut_id(m_world, m_id, ecs_pair(comp_id, object)));
    }

    /** Get mutable pointer for a pair (untyped).
     * This operation gets the value for a pair from the entity. If neither the
     * relation or object are a component, the operation will fail.
     *
     * @param relation the relation.
     * @param object the object.
     */
    void* get_mut(entity_t relation, entity_t object) const {
        return ecs_get_mut_id(m_world, m_id, ecs_pair(relation, object));
    }

    /** Get mutable pointer for the object from a pair.
     * This operation gets the value for a pair from the entity.
     *
     * @tparam Object the object type.
     * @param relation the relation.
     */
    template <typename Object>
    Object* get_mut_w_object(entity_t relation) const {
        auto comp_id = _::cpp_type<Object>::id(m_world);
        ecs_assert(_::cpp_type<Object>::size() != 0, ECS_INVALID_PARAMETER, NULL);
        return static_cast<Object*>(
            ecs_get_mut_id(m_world, m_id, ecs_pair(relation, comp_id)));
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
    flecs::entity null(const flecs::world_t *world) {
        flecs::entity result;
        result.m_world = const_cast<flecs::world_t*>(world);
        return result;
    }

    static
    flecs::entity null() {
        return flecs::entity();
    }
};

} // namespace flecs
