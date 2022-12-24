/**
 * @file addons/cpp/entity.hpp
 * @brief Entity class.
 * 
 * This class provides read/write access to entities.
 */

#pragma once

#include "entity_view.hpp"
#include "mixins/entity/builder.hpp"

/**
 * @defgroup cpp_entities Entities
 * @brief Entity operations.
 * 
 * \ingroup cpp_core
 * @{
 */

namespace flecs
{

/** Entity.
 * Class with read/write operations for entities.
 * 
 * \ingroup cpp_entities
*/
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
     * @tparam First The first part of the pair.
     * @tparam Second the second part of the pair.
     */
    template <typename First, typename Second, typename P = pair<First, Second>, 
        typename A = actual_type_t<P>, if_not_t< flecs::is_pair<First>::value> = 0>
    A* get_mut() const {
        return static_cast<A*>(ecs_get_mut_id(m_world, m_id, ecs_pair(
            _::cpp_type<First>::id(m_world),
            _::cpp_type<Second>::id(m_world))));
    }

    /** Get mutable pointer for a pair.
     * This operation gets the value for a pair from the entity.
     *
     * @tparam First The first part of the pair.
     * @param second The second element of the pair.
     */
    template <typename First>
    First* get_mut(entity_t second) const {
        auto comp_id = _::cpp_type<First>::id(m_world);
        ecs_assert(_::cpp_type<First>::size() != 0, ECS_INVALID_PARAMETER, NULL);
        return static_cast<First*>(
            ecs_get_mut_id(m_world, m_id, ecs_pair(comp_id, second)));
    }

    /** Get mutable pointer for a pair (untyped).
     * This operation gets the value for a pair from the entity. If neither the
     * first nor second element of the pair is a component, the operation will 
     * fail.
     *
     * @param first The first element of the pair.
     * @param second The second element of the pair.
     */
    void* get_mut(entity_t first, entity_t second) const {
        return ecs_get_mut_id(m_world, m_id, ecs_pair(first, second));
    }

    /** Get mutable pointer for the second element of a pair.
     * This operation gets the value for a pair from the entity.
     *
     * @tparam Second The second element of the pair.
     * @param first The first element of the pair.
     */
    template <typename Second>
    Second* get_mut_second(entity_t first) const {
        auto second = _::cpp_type<Second>::id(m_world);
        ecs_assert(_::cpp_type<Second>::size() != 0, ECS_INVALID_PARAMETER, NULL);
        return static_cast<Second*>(
            ecs_get_mut_id(m_world, m_id, ecs_pair(first, second)));
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

    /** Signal that the first element of a pair was modified.
     *
     * @tparam First The first part of the pair.
     * @tparam Second the second part of the pair.
     */
    template <typename First, typename Second>
    void modified() const {
        this->modified<First>(_::cpp_type<Second>::id(m_world));
    }

    /** Signal that the first part of a pair was modified.
     *
     * @tparam First The first part of the pair.
     * @param second The second element of the pair.
     */
    template <typename First>
    void modified(entity_t second) const {
        auto first = _::cpp_type<First>::id(m_world);
        ecs_assert(_::cpp_type<First>::size() != 0, ECS_INVALID_PARAMETER, NULL);
        this->modified(first, second);
    }

    /** Signal that a pair has modified (untyped).
     * If neither the first or second element of the pair are a component, the
     * operation will fail.
     *
     * @param first The first element of the pair.
     * @param second The second element of the pair.
     */
    void modified(entity_t first, entity_t second) const {
        this->modified(ecs_pair(first, second));
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
        return ref<T>(m_world, m_id);
    }

    template <typename First, typename Second, typename P = flecs::pair<First, Second>, 
        typename A = actual_type_t<P>>
    ref<A> get_ref() const {
        // Ensure component is registered
        _::cpp_type<A>::id(m_world);
        return ref<A>(m_world, m_id, 
            ecs_pair(_::cpp_type<First>::id(m_world),
                _::cpp_type<Second>::id(m_world)));
    }

    template <typename First>
    ref<First> get_ref(flecs::entity_t second) const {
        // Ensure component is registered
        _::cpp_type<First>::id(m_world);
        return ref<First>(m_world, m_id, 
            ecs_pair(_::cpp_type<First>::id(m_world), second));
    }

    template <typename Second>
    ref<Second> get_ref_second(flecs::entity_t first) const {
        // Ensure component is registered
        _::cpp_type<Second>::id(m_world);
        return ref<Second>(m_world, m_id, 
            ecs_pair(first, _::cpp_type<Second>::id(m_world)));
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
     * entity object goes out of scope.
     */
    void destruct() const {
        ecs_delete(m_world, m_id);
    }

    /** Entity id 0.
     * This function is useful when the API must provide an entity that
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

/** @} */
