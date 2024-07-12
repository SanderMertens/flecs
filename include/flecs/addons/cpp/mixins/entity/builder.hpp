/**
 * @file addons/cpp/mixins/entity/builder.hpp
 * @brief Entity builder.
 */

#pragma once

namespace flecs
{

/** Entity builder. 
 * @ingroup cpp_entities
 */
template <typename Self>
struct entity_builder : entity_view {

    using entity_view::entity_view;

    /** Add a component to an entity.
     * To ensure the component is initialized, it should have a constructor.
     * 
     * @tparam T the component type to add.
     */
    template <typename T>
    const Self& add() const  {
        flecs_static_assert(is_flecs_constructible<T>::value,
            "cannot default construct type: add T::T() or use emplace<T>()");
        ecs_add_id(this->world_, this->id_, _::type<T>::id(this->world_));
        return to_base();
    }

     /** Add pair for enum constant.
     * This operation will add a pair to the entity where the first element is
     * the enumeration type, and the second element the enumeration constant.
     * 
     * The operation may be used with regular (C style) enumerations as well as
     * enum classes.
     * 
     * @param value The enumeration value.
     */
    template <typename E, if_t< is_enum<E>::value > = 0>
    const Self& add(E value) const  {
        flecs::entity_t first = _::type<E>::id(this->world_);
        const auto& et = enum_type<E>(this->world_);
        flecs::entity_t second = et.entity(value);

        ecs_assert(second, ECS_INVALID_PARAMETER, "Component was not found in reflection data.");
        return this->add(first, second);
    }

    /** Add an entity to an entity.
     * Add an entity to the entity. This is typically used for tagging.
     *
     * @param component The component to add.
     */
    const Self& add(id_t component) const  {
        ecs_add_id(this->world_, this->id_, component);
        return to_base();
    }

    /** Add a pair.
     * This operation adds a pair to the entity.
     *
     * @param first The first element of the pair.
     * @param second The second element of the pair.
     */
    const Self& add(entity_t first, entity_t second) const  {
        ecs_add_pair(this->world_, this->id_, first, second);
        return to_base();
    }

    /** Add a pair.
     * This operation adds a pair to the entity.
     *
     * @tparam First The first element of the pair
     * @tparam Second The second element of the pair
     */
    template<typename First, typename Second>
    const Self& add() const  {
        return this->add<First>(_::type<Second>::id(this->world_));
    }

    /** Add a pair.
     * This operation adds a pair to the entity.
     *
     * @tparam First The first element of the pair
     * @param second The second element of the pair.
     */
    template<typename First, typename Second, if_not_t< is_enum<Second>::value > = 0>
    const Self& add(Second second) const  {
        flecs_static_assert(is_flecs_constructible<First>::value,
            "cannot default construct type: add T::T() or use emplace<T>()");
        return this->add(_::type<First>::id(this->world_), second);
    }

    /** Add a pair.
     * This operation adds a pair to the entity that consists out of a tag
     * combined with an enum constant.
     *
     * @tparam First The first element of the pair
     * @param constant the enum constant.
     */
    template<typename First, typename Second, if_t< is_enum<Second>::value > = 0>
    const Self& add(Second constant) const  {
        flecs_static_assert(is_flecs_constructible<First>::value,
            "cannot default construct type: add T::T() or use emplace<T>()");
        const auto& et = enum_type<Second>(this->world_);
        return this->add<First>(et.entity(constant));
    }

    /** Add a pair.
     * This operation adds a pair to the entity.
     *
     * @param first The first element of the pair
     * @tparam Second The second element of the pair
     */
    template<typename Second>
    const Self& add_second(flecs::entity_t first) const  {
        return this->add(first, _::type<Second>::id(this->world_));
    }

    /** Conditional add.
     * This operation adds if condition is true, removes if condition is false.
     * 
     * @param cond The condition to evaluate.
     * @param component The component to add.
     */
    const Self& add_if(bool cond, flecs::id_t component) const  {
        if (cond) {
            return this->add(component);
        } else {
            return this->remove(component);
        }
    }

    /** Conditional add.
     * This operation adds if condition is true, removes if condition is false.
     * 
     * @tparam T The component to add.
     * @param cond The condition to evaluate.
     */
    template <typename T>
    const Self& add_if(bool cond) const  {
        if (cond) {
            return this->add<T>();
        } else {
            return this->remove<T>();
        }
    }

    /** Conditional add.
     * This operation adds if condition is true, removes if condition is false.
     * 
     * @param cond The condition to evaluate.
     * @param first The first element of the pair.
     * @param second The second element of the pair.
     */
    const Self& add_if(bool cond, flecs::entity_t first, flecs::entity_t second) const  {
        if (cond) {
            return this->add(first, second);
        } else {
            /* If second is 0 or if relationship is exclusive, use wildcard for
             * second which will remove all instances of the relationship.
             * Replacing 0 with Wildcard will make it possible to use the second
             * as the condition. */
            if (!second || ecs_has_id(this->world_, first, flecs::Exclusive)) {
                second = flecs::Wildcard;
            }
            return this->remove(first, second);
        }
    }

    /** Conditional add.
     * This operation adds if condition is true, removes if condition is false.
     * 
     * @tparam First The first element of the pair
     * @param cond The condition to evaluate.
     * @param second The second element of the pair.
     */
    template <typename First>
    const Self& add_if(bool cond, flecs::entity_t second) const  {
        return this->add_if(cond, _::type<First>::id(this->world_), second);
    }

    /** Conditional add.
     * This operation adds if condition is true, removes if condition is false.
     * 
     * @tparam First The first element of the pair
     * @tparam Second The second element of the pair
     * @param cond The condition to evaluate.
     */
    template <typename First, typename Second>
    const Self& add_if(bool cond) const  {
        return this->add_if<First>(cond, _::type<Second>::id(this->world_));
    }

    /** Conditional add.
     * This operation adds if condition is true, removes if condition is false.
     * 
     * @param cond The condition to evaluate.
     * @param constant The enumeration constant.
     */
    template <typename E, if_t< is_enum<E>::value > = 0>
    const Self& add_if(bool cond, E constant) const  {
        const auto& et = enum_type<E>(this->world_);
        return this->add_if<E>(cond, et.entity(constant));
    }

    /** Shortcut for `add(IsA, entity)`.
     *
     * @param second The second element of the pair.
     */
    const Self& is_a(entity_t second) const  {
        return this->add(flecs::IsA, second);
    }

    /** Shortcut for `add(IsA, entity)`.
     *
     * @tparam T the type associated with the entity.
     */
    template <typename T>
    const Self& is_a() const  {
        return this->add(flecs::IsA, _::type<T>::id(this->world_));
    }

    /** Shortcut for `add(ChildOf, entity)`.
     *
     * @param second The second element of the pair.
     */
    const Self& child_of(entity_t second) const  {
        return this->add(flecs::ChildOf, second);
    }

    /** Shortcut for `add(DependsOn, entity)`.
     *
     * @param second The second element of the pair.
     */
    const Self& depends_on(entity_t second) const  {
        return this->add(flecs::DependsOn, second);
    }

     /** Shortcut for `add(DependsOn, entity)`.
     *
     * @param second The second element of the pair.
     */
    template <typename E, if_t<is_enum<E>::value> = 0>
    const Self& depends_on(E second) const {
        const auto& et = enum_type<E>(this->world_);
        flecs::entity_t target = et.entity(second);
        return depends_on(target);
    }

    /** Shortcut for `add(SlotOf, entity)`.
     *
     * @param second The second element of the pair.
     */
    const Self& slot_of(entity_t second) const  {
        return this->add(flecs::SlotOf, second);
    }

    /** Shortcut for `add(SlotOf, target(ChildOf))`.
     */
    const Self& slot() const  {
        ecs_check(ecs_get_target(world_, id_, flecs::ChildOf, 0), 
            ECS_INVALID_PARAMETER, "add ChildOf pair before using slot()");
        return this->slot_of(this->target(flecs::ChildOf));
    error:
        return to_base();
    }

    /** Shortcut for `add(ChildOf, entity)`.
     *
     * @tparam T the type associated with the entity.
     */
    template <typename T>
    const Self& child_of() const  {
        return this->child_of(_::type<T>::id(this->world_));
    }
 
    /** Shortcut for `add(DependsOn, entity)`.
     *
     * @tparam T the type associated with the entity.
     */
    template <typename T>
    const Self& depends_on() const  {
        return this->depends_on(_::type<T>::id(this->world_));
    }

    /** Shortcut for `add(SlotOf, entity)`.
     *
     * @tparam T the type associated with the entity.
     */
    template <typename T>
    const Self& slot_of() const  {
        return this->slot_of(_::type<T>::id(this->world_));
    }

    /** Remove a component from an entity.
     *
     * @tparam T the type of the component to remove.
     */
    template <typename T, if_not_t< is_enum<T>::value > = 0>
    const Self& remove() const {
        ecs_remove_id(this->world_, this->id_, _::type<T>::id(this->world_));
        return to_base();
    }

     /** Remove pair for enum.
     * This operation will remove any `(Enum, *)` pair from the entity.
     * 
     * @tparam E The enumeration type.
     */
    template <typename E, if_t< is_enum<E>::value > = 0>
    const Self& remove() const  {
        flecs::entity_t first = _::type<E>::id(this->world_);
        return this->remove(first, flecs::Wildcard);
    }

    /** Remove an entity from an entity.
     *
     * @param entity The entity to remove.
     */
    const Self& remove(entity_t entity) const  {
        ecs_remove_id(this->world_, this->id_, entity);
        return to_base();
    }

    /** Remove a pair.
     * This operation removes a pair from the entity.
     *
     * @param first The first element of the pair.
     * @param second The second element of the pair.
     */
    const Self& remove(entity_t first, entity_t second) const  {
        ecs_remove_pair(this->world_, this->id_, first, second);
        return to_base();
    }

    /** Removes a pair.
     * This operation removes a pair from the entity.
     *
     * @tparam First The first element of the pair
     * @tparam Second The second element of the pair
     */
    template<typename First, typename Second>
    const Self& remove() const  {
        return this->remove<First>(_::type<Second>::id(this->world_));
    }

    /** Remove a pair.
     * This operation removes the pair from the entity.
     *
     * @tparam First The first element of the pair
     * @param second The second element of the pair.
     */
    template<typename First, typename Second, if_not_t< is_enum<Second>::value > = 0>
    const Self& remove(Second second) const  {
        return this->remove(_::type<First>::id(this->world_), second);
    }

    /** Removes a pair.
     * This operation removes a pair from the entity.
     *
     * @tparam Second The second element of the pair
     * @param first The first element of the pair
     */
    template<typename Second>
    const Self& remove_second(flecs::entity_t first) const  {
        return this->remove(first, _::type<Second>::id(this->world_));
    }

    /** Remove a pair.
     * This operation removes the pair from the entity.
     *
     * @tparam First The first element of the pair
     * @param constant the enum constant.
     */
    template<typename First, typename Second, if_t< is_enum<Second>::value > = 0>
    const Self& remove(Second constant) const  {
        const auto& et = enum_type<Second>(this->world_);
        flecs::entity_t second = et.entity(constant);
        return this->remove<First>(second);
    }  

    /** Mark id for auto-overriding.
     * When an entity inherits from a base entity (using the `IsA` relationship)
     * any ids marked for auto-overriding on the base will be overridden
     * automatically by the entity.
     *
     * @param id The id to mark for overriding.
     */
    const Self& auto_override(flecs::id_t id) const  {
        return this->add(ECS_AUTO_OVERRIDE | id);
    }

    /** Mark pair for auto-overriding.
     * @see auto_override(flecs::id_t) const
     *
     * @param first The first element of the pair.
     * @param second The second element of the pair.
     */
    const Self& auto_override(flecs::entity_t first, flecs::entity_t second) const  {
        return this->auto_override(ecs_pair(first, second));
    }

    /** Mark component for auto-overriding.
     * @see auto_override(flecs::id_t) const
     *
     * @tparam T The component to mark for overriding.
     */
    template <typename T>
    const Self& auto_override() const  {
        return this->auto_override(_::type<T>::id(this->world_));
    }

    /** Mark pair for auto-overriding.
     * @see auto_override(flecs::id_t) const
     *
     * @tparam First The first element of the pair.
     * @param second The second element of the pair.
     */
    template <typename First>
    const Self& auto_override(flecs::entity_t second) const  {
        return this->auto_override(_::type<First>::id(this->world_), second);
    }

    /** Mark pair for auto-overriding.
     * @see auto_override(flecs::id_t) const
     *
     * @tparam First The first element of the pair.
     * @tparam Second The second element of the pair.
     */
    template <typename First, typename Second>
    const Self& auto_override() const  {
        return this->auto_override<First>(_::type<Second>::id(this->world_));
    }

    /** Set component, mark component for auto-overriding.
     * @see auto_override(flecs::id_t) const
     *
     * @tparam T The component to set and for which to add the OVERRIDE flag
     * @param val The value to set.
     */
    template <typename T>
    const Self& set_auto_override(const T& val) const  {
        this->auto_override<T>();
        return this->set<T>(val);
    }

    /** Set component, mark component for auto-overriding.
     * @see auto_override(flecs::id_t) const
     *
     * @tparam T The component to set and for which to add the OVERRIDE flag
     * @param val The value to set.
     */
    template <typename T>
    const Self& set_auto_override(T&& val) const  {
        this->auto_override<T>();
        return this->set<T>(FLECS_FWD(val));
    }

    /** Set pair, mark component for auto-overriding.
     * @see auto_override(flecs::id_t) const
     *
     * @tparam First The first element of the pair.
     * @param second The second element of the pair.
     * @param val The value to set.
     */
    template <typename First>
    const Self& set_auto_override(flecs::entity_t second, const First& val) const  {
        this->auto_override<First>(second);
        return this->set<First>(second, val);
    }

    /** Set pair, mark component for auto-overriding.
     * @see auto_override(flecs::id_t) const
     *
     * @tparam First The first element of the pair.
     * @param second The second element of the pair.
     * @param val The value to set.
     */
    template <typename First>
    const Self& set_auto_override(flecs::entity_t second, First&& val) const  {
        this->auto_override<First>(second);
        return this->set<First>(second, FLECS_FWD(val));
    }

    /** Set component, mark component for auto-overriding.
     * @see auto_override(flecs::id_t) const
     *
     * @tparam First The first element of the pair.
     * @tparam Second The second element of the pair.
     * @param val The value to set.
     */
    template <typename First, typename Second, typename P = pair<First, Second>, 
        typename A = actual_type_t<P>, if_not_t< flecs::is_pair<First>::value> = 0>    
    const Self& set_auto_override(const A& val) const  {
        this->auto_override<First, Second>();
        return this->set<First, Second>(val);
    }

    /** Set component, mark component for auto-overriding.
     * @see auto_override(flecs::id_t) const
     *
     * @tparam First The first element of the pair.
     * @tparam Second The second element of the pair.
     * @param val The value to set.
     */
    template <typename First, typename Second, typename P = pair<First, Second>, 
        typename A = actual_type_t<P>, if_not_t< flecs::is_pair<First>::value> = 0>    
    const Self& set_auto_override(A&& val) const  {
        this->auto_override<First, Second>();
        return this->set<First, Second>(FLECS_FWD(val));
    }

    /** Emplace component, mark component for auto-overriding.
     * @see auto_override(flecs::id_t) const
     *
     * @tparam T The component to emplace and override.
     * @param args The arguments to pass to the constructor of `T`.
     */
    template <typename T, typename ... Args>
    const Self& emplace_auto_override(Args&&... args) const  {
        this->auto_override<T>();

        flecs::emplace<T>(this->world_, this->id_, 
            _::type<T>::id(this->world_), FLECS_FWD(args)...);

        return to_base();  
    }

    /** Emplace pair, mark pair for auto-overriding.
     * @see auto_override(flecs::id_t) const
     *
     * @tparam First The first element of the pair to emplace and override.
     * @tparam Second The second element of the pair to emplace and override.
     * @param args The arguments to pass to the constructor of `Second`.
     */
    template <typename First, typename Second, typename P = pair<First, Second>, 
        typename A = actual_type_t<P>, if_not_t< flecs::is_pair<First>::value> = 0,
            typename ... Args>
    const Self& emplace_auto_override(Args&&... args) const  {
        this->auto_override<First, Second>();

        flecs::emplace<A>(this->world_, this->id_, 
            ecs_pair(_::type<First>::id(this->world_),
                _::type<Second>::id(this->world_)),
            FLECS_FWD(args)...);

        return to_base();  
    }

    /** Enable an entity.
     * Enabled entities are matched with systems and can be searched with
     * queries.
     */
    const Self& enable() const  {
        ecs_enable(this->world_, this->id_, true);
        return to_base();
    }

    /** Disable an entity.
     * Disabled entities are not matched with systems and cannot be searched 
     * with queries, unless explicitly specified in the query expression.
     */
    const Self& disable() const  {
        ecs_enable(this->world_, this->id_, false);
        return to_base();
    }

    /** Enable an id.
     * This sets the enabled bit for this component. If this is the first time
     * the component is enabled or disabled, the bitset is added.
     * 
     * @param id The id to enable.
     * @param toggle True to enable, false to disable (default = true).
     *
     * @see ecs_enable_id()
     */
    const Self& enable(flecs::id_t id, bool toggle = true) const  {
        ecs_enable_id(this->world_, this->id_, id, toggle);
        return to_base();       
    }

    /** Enable a component.
     * @see enable(flecs::id_t) const
     *
     * @tparam T The component to enable.
     */
    template<typename T>
    const Self& enable() const  {
        return this->enable(_::type<T>::id(this->world_));
    }

    /** Enable a pair.
     * @see enable(flecs::id_t) const
     *
     * @param first The first element of the pair.
     * @param second The second element of the pair.
     */
    const Self& enable(flecs::id_t first, flecs::id_t second) const  {
        return this->enable(ecs_pair(first, second));
    }

    /** Enable a pair.
     * @see enable(flecs::id_t) const
     *
     * @tparam First The first element of the pair.
     * @param second The second element of the pair.
     */
    template<typename First>
    const Self& enable(flecs::id_t second) const  {
        return this->enable(_::type<First>::id(), second);
    }

    /** Enable a pair.
     * @see enable(flecs::id_t) const
     *
     * @tparam First The first element of the pair.
     * @tparam Second The second element of the pair.
     */
    template<typename First, typename Second>
    const Self& enable() const  {
        return this->enable<First>(_::type<Second>::id());
    }

    /** Disable an id.
     * This sets the enabled bit for this id. If this is the first time
     * the id is enabled or disabled, the bitset is added.
     *
     * @param id The id to disable.
     *
     * @see ecs_enable_id()
     * @see enable(flecs::id_t) const
     */
    const Self& disable(flecs::id_t id) const  {
        return this->enable(id, false);
    }

    /** Disable a component.
     * @see disable(flecs::id_t) const
     *
     * @tparam T The component to enable.
     */
    template<typename T>
    const Self& disable() const  {
        return this->disable(_::type<T>::id());
    }

    /** Disable a pair.
     * @see disable(flecs::id_t) const
     *
     * @param first The first element of the pair.
     * @param second The second element of the pair.
     */
    const Self& disable(flecs::id_t first, flecs::id_t second) const  {
        return this->disable(ecs_pair(first, second));
    }

    /** Disable a pair.
     * @see disable(flecs::id_t) const
     *
     * @tparam First The first element of the pair.
     * @param second The second element of the pair.
     */
    template<typename First>
    const Self& disable(flecs::id_t second) const  {
        return this->disable(_::type<First>::id(), second);
    }

    /** Disable a pair.
     * @see disable(flecs::id_t) const
     *
     * @tparam First The first element of the pair.
     * @tparam Second The second element of the pair.
     */
    template<typename First, typename Second>
    const Self& disable() const  {
        return this->disable<First>(_::type<Second>::id());
    }

    const Self& set_ptr(entity_t comp, size_t size, const void *ptr) const  {
        ecs_set_id(this->world_, this->id_, comp, size, ptr);
        return to_base();
    }

    const Self& set_ptr(entity_t comp, const void *ptr) const  {
        const flecs::Component *cptr = ecs_get(
            this->world_, comp, EcsComponent);

        /* Can't set if it's not a component */
        ecs_assert(cptr != NULL, ECS_INVALID_PARAMETER, NULL);

        return set_ptr(comp, cptr->size, ptr);
    }

    template<typename T, if_t<is_actual<T>::value> = 0 >
    const Self& set(T&& value) const  {
        flecs::set<T>(this->world_, this->id_, FLECS_FWD(value));
        return to_base();
    }

    template<typename T, if_t<is_actual<T>::value > = 0>
    const Self& set(const T& value) const  {
        flecs::set<T>(this->world_, this->id_, value);
        return to_base();
    }

    template<typename T, typename A = actual_type_t<T>, if_not_t< 
        is_actual<T>::value > = 0>
    const Self& set(A&& value) const  {
        flecs::set<T>(this->world_, this->id_, FLECS_FWD(value));
        return to_base();
    }

    template<typename T, typename A = actual_type_t<T>, if_not_t<
        is_actual<T>::value > = 0>
    const Self& set(const A& value) const  {
        flecs::set<T>(this->world_, this->id_, value);
        return to_base();
    }

    /** Set a pair for an entity.
     * This operation sets the pair value, and uses First as type. If the
     * entity did not yet have the pair, it will be added.
     *
     * @tparam First The first element of the pair.
     * @tparam Second The second element of the pair
     * @param value The value to set.
     */
    template <typename First, typename Second, typename P = pair<First, Second>, 
        typename A = actual_type_t<P>, if_not_t< flecs::is_pair<First>::value> = 0>
    const Self& set(A&& value) const  {
        flecs::set<P>(this->world_, this->id_, FLECS_FWD(value));
        return to_base();
    }

    /** Set a pair for an entity.
     * This operation sets the pair value, and uses First as type. If the
     * entity did not yet have the pair, it will be added.
     *
     * @tparam First The first element of the pair.
     * @tparam Second The second element of the pair
     * @param value The value to set.
     */
    template <typename First, typename Second, typename P = pair<First, Second>, 
        typename A = actual_type_t<P>, if_not_t< flecs::is_pair<First>::value> = 0>
    const Self& set(const A& value) const  {
        flecs::set<P>(this->world_, this->id_, value);
        return to_base();
    }

    /** Set a pair for an entity.
     * This operation sets the pair value, and uses First as type. If the
     * entity did not yet have the pair, it will be added.
     *
     * @tparam First The first element of the pair.
     * @param second The second element of the pair.
     * @param value The value to set.
     */
    template <typename First, typename Second, if_not_t< is_enum<Second>::value > = 0>
    const Self& set(Second second, const First& value) const  {
        auto first = _::type<First>::id(this->world_);
        flecs::set(this->world_, this->id_, value, 
            ecs_pair(first, second));
        return to_base();
    }

    /** Set a pair for an entity.
     * This operation sets the pair value, and uses First as type. If the
     * entity did not yet have the pair, it will be added.
     *
     * @tparam First The first element of the pair.
     * @param second The second element of the pair.
     * @param value The value to set.
     */
    template <typename First, typename Second, if_not_t< is_enum<Second>::value > = 0>
    const Self& set(Second second, First&& value) const  {
        auto first = _::type<First>::id(this->world_);
        flecs::set(this->world_, this->id_, FLECS_FWD(value), 
            ecs_pair(first, second));
        return to_base();
    }

    /** Set a pair for an entity.
     * This operation sets the pair value, and uses First as type. If the
     * entity did not yet have the pair, it will be added.
     *
     * @tparam First The first element of the pair.
     * @param constant The enum constant.
     * @param value The value to set.
     */
    template <typename First, typename Second, if_t< is_enum<Second>::value > = 0>
    const Self& set(Second constant, const First& value) const  {
        const auto& et = enum_type<Second>(this->world_);
        flecs::entity_t second = et.entity(constant);
        return set<First>(second, value);
    }

    /** Set a pair for an entity.
     * This operation sets the pair value, and uses Second as type. If the
     * entity did not yet have the pair, it will be added.
     *
     * @tparam Second The second element of the pair
     * @param first The first element of the pair.
     * @param value The value to set.
     */
    template <typename Second>
    const Self& set_second(entity_t first, const Second& value) const  {
        auto second = _::type<Second>::id(this->world_);
        flecs::set(this->world_, this->id_, value, 
            ecs_pair(first, second));
        return to_base();
    }

    /** Set a pair for an entity.
     * This operation sets the pair value, and uses Second as type. If the
     * entity did not yet have the pair, it will be added.
     *
     * @tparam Second The second element of the pair
     * @param first The first element of the pair.
     * @param value The value to set.
     */
    template <typename Second>
    const Self& set_second(entity_t first, Second&& value) const  {
        auto second = _::type<Second>::id(this->world_);
        flecs::set(this->world_, this->id_, FLECS_FWD(value), 
            ecs_pair(first, second));
        return to_base();
    }

    template <typename First, typename Second>
    const Self& set_second(const Second& value) const  {
        flecs::set<pair_object<First, Second>>(this->world_, this->id_, value);
        return to_base();
    }    

    /** Set 1..N components.
     * This operation accepts a callback with as arguments the components to
     * set. If the entity does not have all of the provided components, they
     * will be added.
     *
     * This operation is faster than individually calling get for each component
     * as it only obtains entity metadata once. When this operation is called
     * while deferred, its performance is equivalent to that of calling ensure
     * for each component separately.
     *
     * The operation will invoke modified for each component after the callback
     * has been invoked.
     *
     * @param func The callback to invoke.
     */
    template <typename Func>
    const Self& insert(const Func& func) const;

    /** Emplace component.
     * Emplace constructs a component in the storage, which prevents calling the
     * destructor on the value passed into the function.
     *
     * Emplace attempts the following signatures to construct the component:
     *
     * @code
     * T{Args...}
     * T{flecs::entity, Args...}
     * @endcode
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
    template<typename T, typename ... Args, typename A = actual_type_t<T>>
    const Self& emplace(Args&&... args) const  {
        flecs::emplace<A>(this->world_, this->id_, 
            _::type<T>::id(this->world_), FLECS_FWD(args)...);
        return to_base();
    }

    template <typename First, typename Second, typename ... Args, typename P = pair<First, Second>, 
        typename A = actual_type_t<P>, if_not_t< flecs::is_pair<First>::value> = 0>
    const Self& emplace(Args&&... args) const  {
        flecs::emplace<A>(this->world_, this->id_, 
            ecs_pair(_::type<First>::id(this->world_),
                _::type<Second>::id(this->world_)),
            FLECS_FWD(args)...);
        return to_base();
    }

    template <typename First, typename ... Args>
    const Self& emplace_first(flecs::entity_t second, Args&&... args) const  {
        flecs::emplace<First>(this->world_, this->id_, 
            ecs_pair(_::type<First>::id(this->world_), second),
            FLECS_FWD(args)...);
        return to_base();
    }

    template <typename Second, typename ... Args>
    const Self& emplace_second(flecs::entity_t first, Args&&... args) const  {
        flecs::emplace<Second>(this->world_, this->id_, 
            ecs_pair(first, _::type<Second>::id(this->world_)),
            FLECS_FWD(args)...);
        return to_base();
    }

    /** Entities created in function will have the current entity.
     * This operation is thread safe.
     *
     * @param func The function to call.
     */
    template <typename Func>
    const Self& with(const Func& func) const  {
        ecs_id_t prev = ecs_set_with(this->world_, this->id_);
        func();
        ecs_set_with(this->world_, prev);
        return to_base();
    }

    /** Entities created in function will have `(First, this)`.
     * This operation is thread safe.
     *
     * @tparam First The first element of the pair
     * @param func The function to call.
     */
    template <typename First, typename Func>
    const Self& with(const Func& func) const  {
        with(_::type<First>::id(this->world_), func);
        return to_base();
    }

    /** Entities created in function will have `(first, this)`.
     * This operation is thread safe.
     *
     * @param first The first element of the pair.
     * @param func The function to call.
     */
    template <typename Func>
    const Self& with(entity_t first, const Func& func) const  {
        ecs_id_t prev = ecs_set_with(this->world_, 
            ecs_pair(first, this->id_));
        func();
        ecs_set_with(this->world_, prev);
        return to_base();
    }

    /** The function will be ran with the scope set to the current entity. */
    template <typename Func>
    const Self& scope(const Func& func) const  {
        ecs_entity_t prev = ecs_set_scope(this->world_, this->id_);
        func();
        ecs_set_scope(this->world_, prev);
        return to_base();
    }

    /** Return world scoped to entity */
    scoped_world scope() const {
        return scoped_world(world_, id_);
    }

    /* Set the entity name.
     */
    const Self& set_name(const char *name) const  {
        ecs_set_name(this->world_, this->id_, name);
        return to_base();
    }

    /* Set entity alias.
     */
    const Self& set_alias(const char *name) const  {
        ecs_set_alias(this->world_, this->id_, name);
        return to_base();
    }

#   ifdef FLECS_DOC
#   include "../doc/entity_builder.inl"
#   endif

#   ifdef FLECS_META
#   include "../meta/entity_builder.inl"
#   endif

#   ifdef FLECS_JSON
#   include "../json/entity_builder.inl"
#   endif

#   include "../event/entity_builder.inl"

protected:
    const Self& to_base() const  {
        return *static_cast<const Self*>(this);
    }
};

}
