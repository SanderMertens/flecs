/**
 * @file addons/cpp/entity_view.hpp
 * @brief Entity class with only read-only operations.
 * 
 * This class provides read-only access to entities. Using this class to store 
 * entities in components ensures valid handles, as this class will always store
 * the actual world vs. a stage. The constructors of this class will never 
 * create a new entity.
 *
 * To obtain a mutable handle to the entity, use the mut() function.
 */

#pragma once

#include "entity_component_tuple.hpp"

/**
 * @ingroup cpp_entities
 * @{
 */

namespace flecs
{

/** Entity view.
 * Class with read operations for entities. Base for flecs::entity.
 * 
 * @ingroup cpp_entities
 */
struct entity_view : public id {

    /** Default constructor. Creates an empty entity view. */
    entity_view() : flecs::id() { }

    /** Wrap an existing entity ID.
     *
     * @param world The world in which the entity is created.
     * @param id The entity ID.
     */
    explicit entity_view(flecs::world_t *world, flecs::id_t id)
        : flecs::id(world
            ? const_cast<flecs::world_t*>(ecs_get_world(world))
            : nullptr
        , id ) { }

    /** Implicit conversion from flecs::entity_t to flecs::entity_view. */
    entity_view(entity_t id) 
        : flecs::id( nullptr, id ) { }

    /** Get entity ID.
     * @return The integer entity ID.
     */
    entity_t id() const {
        return id_;
    }

    /** Check if entity is valid.
     * An entity is valid if:
     * - its ID is not 0
     * - the ID contains a valid bit pattern for an entity
     * - the entity is alive (see is_alive())
     *
     * @return True if the entity is valid, false otherwise.
     * @see ecs_is_valid()
     */
    bool is_valid() const {
        return world_ && ecs_is_valid(world_, id_);
    }
  
    /** Conversion to bool. Returns true if entity is valid. */
    explicit operator bool() const {
        return is_valid();
    }

    /** Check if entity is alive.
     *
     * @return True if the entity is alive, false otherwise.
     * @see ecs_is_alive()
     */
    bool is_alive() const {
        return world_ && ecs_is_alive(world_, id_);
    }

    /** Return the entity name.
     *
     * @return The entity name.
     */
    flecs::string_view name() const {
        return flecs::string_view(ecs_get_name(world_, id_));
    }

    /** Return the entity symbol.
     *
     * @return The entity symbol.
     */
    flecs::string_view symbol() const {
        return flecs::string_view(ecs_get_symbol(world_, id_));
    }

    /** Return the entity path.
     *
     * @param sep The separator used between path elements.
     * @param init_sep The initial separator prepended to the path.
     * @return The hierarchical entity path.
     */
    flecs::string path(const char *sep = "::", const char *init_sep = "::") const {
        return path_from(0, sep, init_sep);
    }

    /** Return the entity path relative to a parent.
     *
     * @param parent The parent entity to compute the path relative to.
     * @param sep The separator used between path elements.
     * @param init_sep The initial separator prepended to the path.
     * @return The relative hierarchical entity path.
     */
    flecs::string path_from(flecs::entity_t parent, const char *sep = "::", const char *init_sep = "::") const {
        char *path = ecs_get_path_w_sep(world_, parent, id_, sep, init_sep);
        return flecs::string(path);
    }

    /** Return the entity path relative to a typed parent.
     *
     * @tparam Parent The parent type to compute the path relative to.
     * @param sep The separator used between path elements.
     * @param init_sep The initial separator prepended to the path.
     * @return The relative hierarchical entity path.
     */
    template <typename Parent>
    flecs::string path_from(const char *sep = "::", const char *init_sep = "::") const {
        return path_from(_::type<Parent>::id(world_), sep, init_sep);
    }

    /** Check if entity is enabled (does not have the Disabled tag).
     *
     * @return True if the entity is enabled, false otherwise.
     */
    bool enabled() const {
        return !ecs_has_id(world_, id_, flecs::Disabled);
    }

    /** Get the entity's type.
     *
     * @return The entity's type.
     */
    flecs::type type() const;

    /** Get the entity's table.
     *
     * @return The entity's table.
     */
    flecs::table table() const;

    /** Get table range for the entity.
     * Return a range with the entity's row as offset and count set to 1. If
     * the entity is not stored in a table, the function returns a range with
     * count 0.
     *
     * @return The entity's table range.
     */
    flecs::table_range range() const;

    /** Iterate (component) IDs of an entity.
     * The function parameter must match the following signature:
     *
     * @code
     * void(*)(flecs::id id)
     * @endcode
     *
     * @param func The function invoked for each ID.
     */
    template <typename Func>
    void each(const Func& func) const;

    /** Iterate matching pair IDs of an entity.
     * The function parameter must match the following signature:
     *
     * @code
     * void(*)(flecs::id id)
     * @endcode
     *
     * @param first The first element of the pair to match.
     * @param second The second element of the pair to match.
     * @param func The function invoked for each ID.
     */
    template <typename Func>
    void each(flecs::id_t first, flecs::id_t second, const Func& func) const;

    /** Iterate targets for a given relationship.
     * The function parameter must match the following signature:
     *
     * @code
     * void(*)(flecs::entity target)
     * @endcode
     *
     * @param rel The relationship for which to iterate the targets.
     * @param func The function invoked for each target.
     */
    template <typename Func>
    void each(const flecs::entity_view& rel, const Func& func) const;

    /** Iterate targets for a given relationship.
     * The function parameter must match the following signature:
     *
     * @code
     * void(*)(flecs::entity target)
     * @endcode
     *
     * @tparam First The relationship for which to iterate the targets.
     * @param func The function invoked for each target.     
     */
    template <typename First, typename Func>
    void each(const Func& func) const { 
        return each(_::type<First>::id(world_), func);
    }

    /** Iterate children for an entity.
     * The function parameter must match the following signature:
     *
     * @code
     * void(*)(flecs::entity target)
     * @endcode
     *
     * @param rel The relationship to follow.
     * @param func The function invoked for each child.
     */
    template <typename Func>
    void children(flecs::entity_t rel, Func&& func) const {
        /* When the entity is a wildcard, this would attempt to query for all
         * entities with (ChildOf, *) or (ChildOf, _) instead of querying for
         * the children of the wildcard entity. */
        if (id_ == flecs::Wildcard || id_ == flecs::Any) {
            /* This is correct, wildcard entities don't have children. */
            return;
        }

        flecs::world world(world_);

        ecs_iter_t it = ecs_children_w_rel(world_, rel, id_);
        while (ecs_children_next(&it)) {
            _::each_delegate<Func>(FLECS_MOV(func)).invoke(&it);
        }
    }

    /** Iterate children for an entity.
     * The function parameter must match the following signature:
     *
     * @code
     * void(*)(flecs::entity target)
     * @endcode
     *
     * @tparam Rel The relationship to follow.
     * @param func The function invoked for each child.
     */
    template <typename Rel, typename Func>
    void children(Func&& func) const {
        children(_::type<Rel>::id(world_), FLECS_MOV(func));
    }

    /** Iterate children for an entity.
     * The function parameter must match the following signature:
     *
     * @code
     * void(*)(flecs::entity target)
     * @endcode
     *
     * This operation follows the ChildOf relationship.
     *
     * @param func The function invoked for each child.
     */
    template <typename Func>
    void children(Func&& func) const {
        children(flecs::ChildOf, FLECS_MOV(func));
    }


    template <typename... T, typename... Args>
    decltype(auto) try_get(Args... args) const {
        return _::get_component<false, false>(world_, id_,
            _::make_id<T...>(world_, args...));
    }

    template <typename... T, typename... Args>
    decltype(auto) try_get_second(Args... args) const {
        return _::get_component<false, false>(world_, id_,
            _::second_id<T...>(world_, args...));
    }

    template <typename... T>
    auto try_get_n() const {
        static_assert(sizeof...(T) > 1 && sizeof...(T) < 9,
            "component tuple requires between two and eight components");
        return typename tuple_builder<sizeof...(T), T...>::type_const_ptr {try_get<T>()...};
    }

    template <typename Func, if_t<is_callable<Func>::value> = 0>
    bool get(const Func& func) const;

    template <typename... T, typename... Args>
    decltype(auto) get(Args... args) const {
        return _::get_component<false, true>(world_, id_,
            _::make_id<T...>(world_, args...));
    }

    template <typename... T, typename... Args>
    decltype(auto) get_second(Args... args) const {
        return _::get_component<false, true>(world_, id_,
            _::second_id<T...>(world_, args...));
    }

    template <typename... T>
    auto get_n() const {
        static_assert(sizeof...(T) > 1 && sizeof...(T) < 9,
            "component tuple requires between two and eight components");
        return typename tuple_builder<sizeof...(T), T...>::type_const {get<T>()...};
    }

    template <typename... T, typename... Args>
    decltype(auto) try_get_mut(Args... args) const {
        return _::get_component<true, false>(world_, id_,
            _::make_id<T...>(world_, args...));
    }

    template <typename... T, typename... Args>
    decltype(auto) try_get_mut_second(Args... args) const {
        return _::get_component<true, false>(world_, id_,
            _::second_id<T...>(world_, args...));
    }

    template <typename... T>
    auto try_get_mut_n() const {
        static_assert(sizeof...(T) > 1 && sizeof...(T) < 9,
            "component tuple requires between two and eight components");
        return typename tuple_builder<sizeof...(T), T...>::type_ptr {try_get_mut<T>()...};
    }

    template <typename... T, typename... Args>
    decltype(auto) get_mut(Args... args) const {
        return _::get_component<true, true>(world_, id_,
            _::make_id<T...>(world_, args...));
    }

    template <typename... T, typename... Args>
    decltype(auto) get_mut_second(Args... args) const {
        return _::get_component<true, true>(world_, id_,
            _::second_id<T...>(world_, args...));
    }

    template <typename... T>
    auto get_mut_n() const {
        static_assert(sizeof...(T) > 1 && sizeof...(T) < 9,
            "component tuple requires between two and eight components");
        return typename tuple_builder<sizeof...(T), T...>::type {get_mut<T>()...};
    }

    /** Get enum constant for enum relationship.
     *
     * @tparam Enum The enum type.
     * @return The enum constant value.
     */
    template<typename Enum>
    Enum get_constant() const;
    
    /** Get target for a given pair.
     * This operation returns the target for a given pair. The optional
     * index can be used to iterate through targets, in case the entity has
     * multiple instances for the same relationship.
     *
     * @tparam First The first element of the pair.
     * @param index The index (0 for the first instance of the relationship).
     * @return The target entity.
     */
    template<typename First>
    flecs::entity target(int32_t index = 0) const;

    /** Get target for a given pair.
     * This operation returns the target for a given pair. The optional
     * index can be used to iterate through targets, in case the entity has
     * multiple instances for the same relationship.
     *
     * @param first The first element of the pair for which to retrieve the target.
     * @param index The index (0 for the first instance of the relationship).
     * @return The target entity.
     */
    flecs::entity target(flecs::entity_t first, int32_t index = 0) const;

    /** Get the target of a pair for a given relationship ID.
     * This operation returns the first entity that has the provided component ID
     * by following the specified relationship. If the entity itself has the
     * component ID, then the entity will be returned. If the component ID cannot
     * be found on the entity or by following the
     * relationship, the operation will return 0.
     *
     * This operation can be used to lookup, for example, which prefab is providing
     * a component by specifying the IsA pair:
     *
     * @code
     * // Is Position provided by the entity or one of its base entities?
     * ecs_get_target_for_id(world, entity, EcsIsA, ecs_id(Position))
     * @endcode
     *
     * @param relationship The relationship to follow.
     * @param id The component ID to lookup.
     * @return The entity for which the target has been found.
     */
    flecs::entity target_for(flecs::entity_t relationship, flecs::id_t id) const;

    /** Get the target of a pair for a given relationship ID.
     *
     * @tparam T The component type to lookup.
     * @param relationship The relationship to follow.
     * @return The entity for which the target has been found.
     */
    template <typename T>
    flecs::entity target_for(flecs::entity_t relationship) const;

    /** Get the target of a pair for a given relationship ID.
     *
     * @tparam First The first element of the pair to lookup.
     * @tparam Second The second element of the pair to lookup.
     * @param relationship The relationship to follow.
     * @return The entity for which the target has been found.
     */
    template <typename First, typename Second>
    flecs::entity target_for(flecs::entity_t relationship) const;

    /** Get the depth for a given relationship.
     *
     * @param rel The relationship.
     * @return The depth.
     */
    int32_t depth(flecs::entity_t rel) const {
        return ecs_get_depth(world_, id_, rel);
    }

    /** Get the depth for a given relationship.
     *
     * @tparam Rel The relationship.
     * @return The depth.
     */
    template<typename Rel>
    int32_t depth() const {
        return this->depth(_::type<Rel>::id(world_));
    }

    /** Get parent of entity.
     * Short for target(flecs::ChildOf).
     * 
     * @return The parent of the entity.
     */
    flecs::entity parent() const;
    
    /** Lookup an entity by name.
     * Lookup an entity in the scope of this entity. The provided path may
     * contain double colons as scope separators, for example: "Foo::Bar".
     *
     * @param path The name of the entity to lookup.
     * @param search_path When false, only the entity's scope is searched.
     * @return The found entity, or entity::null if no entity matched.
     */
    flecs::entity lookup(const char *path, bool search_path = false) const;

    template <typename... T, typename... Args>
    bool has(Args... args) const {
        return _::has_component(world_, id_, _::make_id<T...>(world_, args...));
    }

    template <typename Second>
    bool has_second(flecs::entity_t first) const {
        return has(first, _::type<Second>::id(world_));
    }

    template <typename... T, typename... Args>
    bool owns(Args... args) const {
        return ecs_owns_id(world_, id_, _::make_id<T...>(world_, args...).id);
    }

    template <typename Second>
    bool owns_second(flecs::entity_t first) const {
        return owns(first, _::type<Second>::id(world_));
    }

    template <typename... T, typename... Args>
    bool enabled(Args... args) const {
        return ecs_is_enabled_id(world_, id_, _::make_id<T...>(world_, args...).id);
    }

    /** Clone an entity.
     * Create a copy of the current entity with all of its components.
     *
     * @param clone_value If true, clone component values. If false, only clone the entity's type.
     * @param dst_id If nonzero, clone to this entity ID instead of creating a new one.
     * @return The cloned entity.
     */
    flecs::entity clone(bool clone_value = true, flecs::entity_t dst_id = 0) const;

    /** Return a mutable entity handle for the current stage.
     * When an entity handle created from the world is used while the world is
     * in staged mode, it will only allow for read-only operations since
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
    flecs::entity mut(const flecs::world& stage) const;

    /** Same as mut(world), but for an iterator.
     * This operation allows for the construction of a mutable entity handle
     * from an iterator.
     *
     * @param it An iterator that contains a reference to the world or stage.
     * @return An entity handle that allows for mutations in the current stage.
     */
    flecs::entity mut(const flecs::iter& it) const;

    /** Same as mut(world), but for an entity.
     * This operation allows for the construction of a mutable entity handle
     * from another entity. This is useful in each() functions, which only 
     * provide a handle to the entity being iterated over.
     *
     * @param e Another mutable entity.
     * @return An entity handle that allows for mutations in the current stage.
     */
    flecs::entity mut(const flecs::entity_view& e) const;

#   ifdef FLECS_JSON
#   include "mixins/json/entity_view.inl"
#   endif
#   ifdef FLECS_DOC
#   include "mixins/doc/entity_view.inl"
#   endif
#   ifdef FLECS_ALERTS
#   include "mixins/alerts/entity_view.inl"
#   endif

#   include "mixins/enum/entity_view.inl"
#   include "mixins/event/entity_view.inl"

private:
    flecs::entity set_stage(world_t *stage);
};

}

/** @} */
