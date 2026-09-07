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
    /** Default constructor. Creates an empty entity. */
    entity() : entity_builder<entity>() { }

    /** Wrap an existing entity ID.
     *
     * @param world The world in which the entity is created.
     * @param id The entity ID.
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

    /** Create a named entity for a parent using ChildOf hierarchy storage.
     *
     * @param world The world in which to create the entity.
     * @param parent The parent entity ID.
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

    /** Create a named entity for a parent using Parent hierarchy storage.
     * The specified name cannot be a scoped identifier. For example:
     * - OK: "Foo"
     * - Not OK: "Foo::Bar"
     *
     * @param world The world in which to create the entity.
     * @param parent The parent entity.
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

    template <typename... T, typename... Args>
    decltype(auto) ensure(Args... args) const {
        return _::get_component<true, true, true>(world_, id_,
            _::make_id<T...>(world_, args...));
    }

    template <typename Second>
    Second& ensure_second(entity_t first) const {
        return _::get_component<true, true, true>(world_, id_,
            _::second_id<Second>(world_, first));
    }

    #endif

    template <typename... T, typename... Args>
    void modified(Args... args) const {
        auto id = _::make_id<T...>(world_, args...);
        using A = typename decltype(id)::type;
        if constexpr (!std::is_void_v<A>) {
            ecs_assert(_::type<A>::size() != 0, ECS_INVALID_PARAMETER,
                "operation invalid for empty type");
        }
        ecs_modified_id(world_, id_, id.id);
    }

    /** Get reference to component specified by component ID.
     * A reference allows for quick and safe access to a component value, and is
     * a faster alternative to repeatedly calling get() for the same component.
     * 
     * The method accepts a component ID argument, which can be used to create a
     * ref to a component that is different from the provided type. This allows 
     * for creating a base type ref that points to a derived type:
     * 
     * @code
     * flecs::ref<Base> r = e.get_ref_w_id<Base>(world.id<Derived>());
     * @endcode
     * 
     * If the provided component ID is not binary compatible with the specified
     * type, the behavior is undefined.
     *
     * @tparam T Component for which to get a reference.
     * @param component The component ID to reference.
     * @return The reference.
     */
    template <typename T, if_t< is_actual<T>::value > = 0>
    ref<T> get_ref_w_id(flecs::id_t component) const {
        _::type<T>::id(world_); // Ensure type is registered.
        return ref<T>(world_, id_, component);
    }

    template <typename... T, typename... Args>
    auto get_ref(Args... args) const {
        auto id = _::make_id<T...>(world_, args...);
        using A = typename decltype(id)::type;
        using Ref = conditional_t<std::is_void_v<A>, untyped_ref, ref<A>>;
        return Ref(world_, id_, id.id);
    }

    template <typename Second>
    ref<Second> get_ref_second(flecs::entity_t first) const {
        return ref<Second>(world_, id_, _::second_id<Second>(world_, first).id);
    }

    /** Clear an entity.
     * This operation removes all components from an entity without recycling
     * the entity ID.
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

    /** Set child order.
     * Changes the order of children as returned by entity::children(). Only
     * applicable to entities with the flecs::OrderedChildren trait.
     *
     * @param children Array of child entity IDs in the desired order.
     * @param child_count Number of children in the array.
     *
     * @see ecs_set_child_order()
     */
    void set_child_order(flecs::entity_t *children, int32_t child_count) const {
        ecs_set_child_order(world_, id_, children, child_count);
    }

    /** Return the entity as an entity_view.
     * This returns an entity_view instance for the entity, which is a read-only
     * version of the entity class.
     *
     * This is similar to a regular upcast, except that this method ensures that
     * the entity_view instance is instantiated with a world vs. a stage, which
     * a regular upcast does not guarantee.
     *
     * @return The entity_view.
     */
    flecs::entity_view view() const {
        return flecs::entity_view(
            const_cast<flecs::world_t*>(ecs_get_world(world_)), id_);
    }

    /** Entity ID 0.
     * This function is useful when the API must provide an entity that
     * belongs to a world, but the entity ID is 0.
     *
     * @param world The world.
     * @return An entity with ID 0.
     */
    static
    flecs::entity null(const flecs::world_t *world) {
        flecs::entity result;
        result.world_ = const_cast<flecs::world_t*>(world);
        return result;
    }

    /** Entity ID 0 without a world.
     *
     * @return An entity with ID 0 and no world.
     */
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
