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
 * @ingroup cpp_core
 * Entity operations.
 *
 * @{
 */

namespace flecs
{

/** Entity.
 * Class with read/write operations for entities.
 *
 * @ingroup cpp_entities
*/
struct entity : entity_builder<entity>
{
    entity() : entity_builder<entity>() { }

    /** Wrap an existing entity id.
     *
     * @param world The world in which the entity is created.
     * @param id The entity id.
     */
    explicit entity(const flecs::world_t *world, flecs::entity_t id) {
        world_ = const_cast<flecs::world_t*>(world);
        id_ = id;
    }

    /** Create a new entity.
     *
     * @param world The world in which to create the entity.
     */
    explicit entity(world_t *world)
        : entity_builder()
    {
        world_ = world;
        id_ = ecs_cpp_new(world, 0, nullptr, nullptr, nullptr);
    }

    /** Create a named entity.
     *
     * @param world The world in which to create the entity.
     * @param name The entity name.
     * @param sep String used to indicate scoping (Foo::Bar).
     * @param root_sep String used to indicate name is fully scoped (::Foo::Bar).
     */
    explicit entity(
        world_t *world, 
        const char *name, 
        const char *sep = "::", 
        const char *root_sep = "::") : entity_builder()
    {
        world_ = world;

        ecs_entity_desc_t desc = {};
        desc.name = name;
        desc.sep = sep;
        desc.root_sep = root_sep;
        id_ = ecs_entity_init(world, &desc);
    }

    /** Create a named entity for parent using ChildOf hierarchy storage.
     * 
     * @param world The world in which to create the entity.
     * @param name The entity name.
     * @param sep String used to indicate scoping (Foo::Bar).
     * @param root_sep String used to indicate name is fully scoped (::Foo::Bar).
     */
    explicit entity(
        world_t *world, 
        flecs::entity_t parent, 
        const char *name, 
        const char *sep = "::", 
        const char *root_sep = "::") : entity_builder()
    {
        world_ = world;

        ecs_entity_desc_t desc = {};
        desc.name = name;
        desc.parent = parent;
        desc.sep = sep;
        desc.root_sep = root_sep;
        id_ = ecs_entity_init(world, &desc);
    }

    /** Create a named entity for parent using Parent hierarchy storage.
     * The specified name cannot be a scoped identifier. For example:
     * - OK: "Foo"
     * - Not OK: "Foo::Bar"
     * 
     * @param world The world in which to create the entity.
     * @param name The entity name (optional).
     */
    explicit entity(
        world_t *world, 
        const flecs::Parent& parent,
        const char *name = nullptr) : entity_builder()
    {
        world_ = world;
        id_ = ecs_new_w_parent(world, parent.value, name);
    }

    /** Conversion from flecs::entity_t to flecs::entity.
     *
     * @param id The entity_t value to convert.
     */
    explicit entity(entity_t id)
        : entity_builder( nullptr, id ) { }

    #ifndef ensure

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
    T& ensure() const {
        auto comp_id = _::type<T>::id(world_);
        ecs_assert(_::type<T>::size() != 0, ECS_INVALID_PARAMETER,
            "operation invalid for empty type");
        return *static_cast<T*>(ecs_ensure_id(world_, id_, comp_id, sizeof(T)));
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
    void* ensure(entity_t comp) const {
        const flecs::type_info_t *ti = ecs_get_type_info(world_, comp);
        ecs_assert(ti != nullptr && ti->size != 0, ECS_INVALID_PARAMETER, 
            "provided component is not a type or has size 0");
        return ecs_ensure_id(world_, id_, comp, static_cast<size_t>(ti->size));
    }

    /** Get mutable pointer for a pair.
     * This operation gets the value for a pair from the entity.
     *
     * @tparam First The first part of the pair.
     * @tparam Second the second part of the pair.
     */
    template <typename First, typename Second, typename P = pair<First, Second>,
        typename A = actual_type_t<P>, if_not_t< flecs::is_pair<First>::value> = 0>
    A& ensure() const {
        return *static_cast<A*>(ecs_ensure_id(world_, id_, ecs_pair(
            _::type<First>::id(world_),
            _::type<Second>::id(world_)), sizeof(A)));
    }

    /** Get mutable pointer for the first element of a pair.
     * This operation gets the value for a pair from the entity.
     *
     * @tparam First The first part of the pair.
     * @param second The second element of the pair.
     */
    template <typename First>
    First& ensure(entity_t second) const {
        auto first = _::type<First>::id(world_);
        ecs_assert(_::type<First>::size() != 0, ECS_INVALID_PARAMETER,
            "operation invalid for empty type");
        return *static_cast<First*>(
            ecs_ensure_id(world_, id_, ecs_pair(first, second), sizeof(First)));
    }

    /** Get mutable pointer for a pair (untyped).
     * This operation gets the value for a pair from the entity. If neither the
     * first nor second element of the pair is a component, the operation will
     * fail.
     *
     * @param first The first element of the pair.
     * @param second The second element of the pair.
     */
    void* ensure(entity_t first, entity_t second) const {
        return ensure(ecs_pair(first, second));
    }

    /** Get mutable pointer for the second element of a pair.
     * This operation gets the value for a pair from the entity.
     *
     * @tparam Second The second element of the pair.
     * @param first The first element of the pair.
     */
    template <typename Second>
    Second& ensure_second(entity_t first) const {
        auto second = _::type<Second>::id(world_);
        ecs_assert( ecs_get_type_info(world_, ecs_pair(first, second)) != NULL,
            ECS_INVALID_PARAMETER, "pair is not a component");
        ecs_assert( ecs_get_type_info(world_, ecs_pair(first, second))->component == second,
            ECS_INVALID_PARAMETER, "type of pair is not Second");
        ecs_assert(_::type<Second>::size() != 0, ECS_INVALID_PARAMETER,
            "operation invalid for empty type");
        return *static_cast<Second*>(
            ecs_ensure_id(world_, id_, ecs_pair(first, second), sizeof(Second)));
    }

    #endif

    /** Signal that component was modified.
     *
     * @tparam T component that was modified.
     */
    template <typename T>
    void modified() const {
        auto comp_id = _::type<T>::id(world_);
        ecs_assert(_::type<T>::size() != 0, ECS_INVALID_PARAMETER,
            "operation invalid for empty type");
        this->modified(comp_id);
    }

    /** Signal that the first element of a pair was modified.
     *
     * @tparam First The first part of the pair.
     * @tparam Second the second part of the pair.
     */
    template <typename First, typename Second, typename A = actual_type_t<flecs::pair<First, Second>>>
    void modified() const {
        auto first = _::type<First>::id(world_);
        auto second = _::type<Second>::id(world_);
        ecs_assert(_::type<A>::size() != 0, ECS_INVALID_PARAMETER,
            "operation invalid for empty type");
        this->modified(first, second);
    }

    /** Signal that the first part of a pair was modified.
     *
     * @tparam First The first part of the pair.
     * @param second The second element of the pair.
     */
    template <typename First>
    void modified(entity_t second) const {
        auto first = _::type<First>::id(world_);
        ecs_assert(_::type<First>::size() != 0, ECS_INVALID_PARAMETER,
            "operation invalid for empty type");
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
        ecs_modified_id(world_, id_, comp);
    }

    /** Get reference to component specified by id.
     * A reference allows for quick and safe access to a component value, and is
     * a faster alternative to repeatedly calling 'get' for the same component.
     * 
     * The method accepts a component id argument, which can be used to create a 
     * ref to a component that is different from the provided type. This allows 
     * for creating a base type ref that points to a derived type:
     * 
     * @code
     * flecs::ref<Base> r = e.get_ref<Base>(world.id<Derived>());
     * @endcode
     * 
     * If the provided component id is not binary compatible with the specified
     * type, the behavior is undefined.
     *
     * @tparam T component for which to get a reference.
     * @return The reference.
     */
    template <typename T, if_t< is_actual<T>::value > = 0>
    ref<T> get_ref_w_id(flecs::id_t component) const {
        _::type<T>::id(world_); // ensure type is registered
        return ref<T>(world_, id_, component);
    }

    /** Get reference to component.
     * A reference allows for quick and safe access to a component value, and is
     * a faster alternative to repeatedly calling 'get' for the same component.
     *
     * @tparam T component for which to get a reference.
     * @return The reference.
     */
    template <typename T, if_t< is_actual<T>::value > = 0>
    ref<T> get_ref() const {
        return ref<T>(world_, id_, _::type<T>::id(world_));
    }

    /** Get reference to component.
     * Overload for when T is not the same as the actual type, which happens
     * when using pair types.
     * A reference allows for quick and safe access to a component value, and is
     * a faster alternative to repeatedly calling 'get' for the same component.
     *
     * @tparam T component for which to get a reference.
     * @return The reference.
     */
    template <typename T, typename A = actual_type_t<T>, if_t< flecs::is_pair<T>::value > = 0>
    ref<A> get_ref() const {
        return ref<A>(world_, id_,
                      ecs_pair(_::type<typename T::first>::id(world_),
                               _::type<typename T::second>::id(world_)));
    }


    template <typename First, typename Second, typename P = flecs::pair<First, Second>,
        typename A = actual_type_t<P>>
    ref<A> get_ref() const {
        return ref<A>(world_, id_,
            ecs_pair(_::type<First>::id(world_), _::type<Second>::id(world_)));
    }

    template <typename First>
    ref<First> get_ref(flecs::entity_t second) const {
        auto first = _::type<First>::id(world_);
        return ref<First>(world_, id_, ecs_pair(first, second));
    }

    untyped_ref get_ref(flecs::id_t component) const {
        return untyped_ref(world_, id_, component);
    } 

    untyped_ref get_ref(flecs::id_t first, flecs::id_t second) const {
        return untyped_ref(world_, id_, ecs_pair(first, second));
    } 

    template <typename Second>
    ref<Second> get_ref_second(flecs::entity_t first) const {
        auto second = _::type<Second>::id(world_);
        ecs_assert( ecs_get_type_info(world_, ecs_pair(first, second)) != NULL,
            ECS_INVALID_PARAMETER, "pair is not a component");
        ecs_assert( ecs_get_type_info(world_, ecs_pair(first, second))->component == second,
            ECS_INVALID_PARAMETER, "type of pair is not Second");
        return ref<Second>(world_, id_, ecs_pair(first, second));
    }

    /** Clear an entity.
     * This operation removes all components from an entity without recycling
     * the entity id.
     *
     * @see ecs_clear()
     */
    void clear() const {
        ecs_clear(world_, id_);
    }

    /** Delete an entity.
     * Entities have to be deleted explicitly, and are not deleted when the
     * entity object goes out of scope.
     *
     * @see ecs_delete()
     */
    void destruct() const {
        ecs_delete(world_, id_);
    }

    /** Create child */
    template <typename ... Args>
    flecs::entity child(flecs::entity_t r = flecs::ChildOf, Args&&... args) {
        flecs::world world(world_);
        return world.entity(FLECS_FWD(args)...).add(r, id_);
    }

    template <typename R, typename ... Args>
    flecs::entity child(Args&&... args) {
        flecs::world world(world_);
        return world.entity(FLECS_FWD(args)...).add(_::type<R>::id(world_), id_);
    }

    /** Set child order.
     * Changes the order of children as returned by entity::children(). Only 
     * applicableo to entities with the flecs::OrderedChildren trait.
     * 
     * @see ecs_set_child_order()
     */
    void set_child_order(flecs::entity_t *children, int32_t child_count) const {
        ecs_set_child_order(world_, id_, children, child_count);
    }

    /** Return entity as entity_view.
     * This returns an entity_view instance for the entity which is a readonly
     * version of the entity class.
     *
     * This is similar to a regular upcast, except that this method ensures that
     * the entity_view instance is instantiated with a world vs. a stage, which
     * a regular upcast does not guarantee.
     */
    flecs::entity_view view() const {
        return flecs::entity_view(
            const_cast<flecs::world_t*>(ecs_get_world(world_)), id_);
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
        result.world_ = const_cast<flecs::world_t*>(world);
        return result;
    }

    static
    flecs::entity null() {
        return flecs::entity();
    }

#   ifdef FLECS_JSON
#   include "mixins/json/entity.inl"
#   endif
};

} // namespace flecs

/** @} */
