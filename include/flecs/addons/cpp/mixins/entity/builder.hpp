/**
 * @file addons/cpp/mixins/entity/builder.hpp
 * @brief Entity builder.
 */

#pragma once

namespace flecs
{

/** Entity builder. 
 * \ingroup cpp_entities
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
    Self& add() {
        flecs_static_assert(is_flecs_constructible<T>::value,
            "cannot default construct type: add T::T() or use emplace<T>()");
        ecs_add_id(this->m_world, this->m_id, _::cpp_type<T>::id(this->m_world));
        return to_base();
    }

    /** Add an entity to an entity.
     * Add an entity to the entity. This is typically used for tagging.
     *
     * @param component The component to add.
     */
    Self& add(id_t component) {
        ecs_add_id(this->m_world, this->m_id, component);
        return to_base();
    }

    /** Add a pair.
     * This operation adds a pair to the entity.
     *
     * @param first The first element of the pair.
     * @param second The second element of the pair.
     */
    Self& add(entity_t first, entity_t second) {
        ecs_add_pair(this->m_world, this->m_id, first, second);
        return to_base();
    }

    /** Add a pair.
     * This operation adds a pair to the entity.
     *
     * @tparam First The first element of the pair
     * @tparam Second The second element of the pair
     */
    template<typename First, typename Second>
    Self& add() {
        return this->add<First>(_::cpp_type<Second>::id(this->m_world));
    }

    /** Add a pair.
     * This operation adds a pair to the entity.
     *
     * @tparam First The first element of the pair
     * @param second The second element of the pair.
     */
    template<typename First, typename Second, if_not_t< is_enum<Second>::value > = 0>
    Self& add(Second second) {
        flecs_static_assert(is_flecs_constructible<First>::value,
            "cannot default construct type: add T::T() or use emplace<T>()");
        return this->add(_::cpp_type<First>::id(this->m_world), second);
    }

    /** Add a pair.
     * This operation adds a pair to the entity that consists out of a tag
     * combined with an enum constant.
     *
     * @tparam First The first element of the pair
     * @param constant the enum constant.
     */
    template<typename First, typename Second, if_t< is_enum<Second>::value > = 0>
    Self& add(Second constant) {
        flecs_static_assert(is_flecs_constructible<First>::value,
            "cannot default construct type: add T::T() or use emplace<T>()");
        const auto& et = enum_type<Second>(this->m_world);
        return this->add<First>(et.entity(constant));
    }

    /** Add a pair.
     * This operation adds a pair to the entity.
     *
     * @param first The first element of the pair
     * @tparam Second The second element of the pair
     */
    template<typename Second>
    Self& add_second(flecs::entity_t first) {
        return this->add(first, _::cpp_type<Second>::id(this->m_world));
    }

    /** Conditional add.
     * This operation adds if condition is true, removes if condition is false.
     * 
     * @param cond The condition to evaluate.
     * @param component The component to add.
     */
    Self& add_if(bool cond, flecs::id_t component) {
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
    Self& add_if(bool cond) {
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
    Self& add_if(bool cond, flecs::entity_t first, flecs::entity_t second) {
        if (cond) {
            return this->add(first, second);
        } else {
            /* If second is 0 or if relationship is exclusive, use wildcard for
             * second which will remove all instances of the relationship.
             * Replacing 0 with Wildcard will make it possible to use the second
             * as the condition. */
            if (!second || ecs_has_id(this->m_world, first, flecs::Exclusive)) {
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
    Self& add_if(bool cond, flecs::entity_t second) {
        return this->add_if(cond, _::cpp_type<First>::id(this->m_world), second);
    }

    /** Conditional add.
     * This operation adds if condition is true, removes if condition is false.
     * 
     * @tparam First The first element of the pair
     * @tparam Second The second element of the pair
     * @param cond The condition to evaluate.
     */
    template <typename First, typename Second>
    Self& add_if(bool cond) {
        return this->add_if<First>(cond, _::cpp_type<Second>::id(this->m_world));
    }

    /** Conditional add.
     * This operation adds if condition is true, removes if condition is false.
     * 
     * @param cond The condition to evaluate.
     * @param constant The enumeration constant.
     */
    template <typename E, if_t< is_enum<E>::value > = 0>
    Self& add_if(bool cond, E constant) {
        const auto& et = enum_type<E>(this->m_world);
        return this->add_if<E>(cond, et.entity(constant));
    }

    /** Shortcut for add(IsA, entity).
     *
     * @param second The second element of the pair.
     */
    Self& is_a(entity_t second) {
        return this->add(flecs::IsA, second);
    }

    /** Shortcut for add(IsA, entity).
     *
     * @tparam T the type associated with the entity.
     */
    template <typename T>
    Self& is_a() {
        return this->add(flecs::IsA, _::cpp_type<T>::id(this->m_world));
    }

    /** Shortcut for add(ChildOf, entity).
     *
     * @param second The second element of the pair.
     */
    Self& child_of(entity_t second) {
        return this->add(flecs::ChildOf, second);
    }

    /** Shortcut for add(DependsOn, entity).
     *
     * @param second The second element of the pair.
     */
    Self& depends_on(entity_t second) {
        return this->add(flecs::DependsOn, second);
    }

    /** Shortcut for add(SlotOf, entity).
     *
     * @param second The second element of the pair.
     */
    Self& slot_of(entity_t second) {
        return this->add(flecs::SlotOf, second);
    }

    /** Shortcut for add(SlotOf, target(ChildOf)).
     */
    Self& slot() {
        ecs_check(ecs_get_target(m_world, m_id, flecs::ChildOf, 0), 
            ECS_INVALID_PARAMETER, "add ChildOf pair before using slot()");
        return this->slot_of(this->target(flecs::ChildOf));
    error:
        return to_base();
    }

    /** Shortcut for add(ChildOf, entity).
     *
     * @tparam T the type associated with the entity.
     */
    template <typename T>
    Self& child_of() {
        return this->child_of(_::cpp_type<T>::id(this->m_world));
    }
 
    /** Shortcut for add(DependsOn, entity).
     *
     * @tparam T the type associated with the entity.
     */
    template <typename T>
    Self& depends_on() {
        return this->depends_on(_::cpp_type<T>::id(this->m_world));
    }

    /** Shortcut for add(SlotOf, entity).
     *
     * @tparam T the type associated with the entity.
     */
    template <typename T>
    Self& slot_of() {
        return this->slot_of(_::cpp_type<T>::id(this->m_world));
    }

    /** Remove a component from an entity.
     *
     * @tparam T the type of the component to remove.
     */
    template <typename T, if_not_t< is_enum<T>::value > = 0>
    Self& remove() {
        ecs_remove_id(this->m_world, this->m_id, _::cpp_type<T>::id(this->m_world));
        return to_base();
    }

    /** Remove an entity from an entity.
     *
     * @param entity The entity to remove.
     */
    Self& remove(entity_t entity) {
        ecs_remove_id(this->m_world, this->m_id, entity);
        return to_base();
    }

    /** Remove a pair.
     * This operation removes a pair from the entity.
     *
     * @param first The first element of the pair.
     * @param second The second element of the pair.
     */
    Self& remove(entity_t first, entity_t second) {
        ecs_remove_pair(this->m_world, this->m_id, first, second);
        return to_base();
    }

    /** Removes a pair.
     * This operation removes a pair from the entity.
     *
     * @tparam First The first element of the pair
     * @tparam Second The second element of the pair
     */
    template<typename First, typename Second>
    Self& remove() {
        return this->remove<First>(_::cpp_type<Second>::id(this->m_world));
    }

    /** Remove a pair.
     * This operation adds a pair to the entity.
     *
     * @tparam First The first element of the pair
     * @param second The second element of the pair.
     */
    template<typename First, typename Second, if_not_t< is_enum<Second>::value > = 0>
    Self& remove(Second second) {
        return this->remove(_::cpp_type<First>::id(this->m_world), second);
    }

    /** Removes a pair.
     * This operation removes a pair from the entity.
     *
     * @tparam Second The second element of the pair
     * @param first The first element of the pair
     */
    template<typename Second>
    Self& remove_second(flecs::entity_t first) {
        return this->remove(first, _::cpp_type<Second>::id(this->m_world));
    }

    /** Remove a pair.
     * This operation adds a pair to the entity.
     *
     * @tparam First The first element of the pair
     * @param constant the enum constant.
     */
    template<typename First, typename Second, if_t< is_enum<Second>::value > = 0>
    Self& remove(Second constant) {
        const auto& et = enum_type<Second>(this->m_world);
        flecs::entity_t second = et.entity(constant);
        return this->remove<First>(second);
    }  

    /** Mark id for auto-overriding.
     * When an entity inherits from a base entity (using the IsA relationship)
     * any ids marked for auto-overriding on the base will be overridden
     * automatically by the entity.
     *
     * @param id The id to mark for overriding.
     */    
    Self& override(flecs::id_t id) {
        return this->add(ECS_OVERRIDE | id);
    }

    /** Mark pair for auto-overriding.
     * @see override(flecs::id_t id)
     *
     * @param first The first element of the pair.
     * @param second The second element of the pair.
     */     
    Self& override(flecs::entity_t first, flecs::entity_t second) {
        return this->override(ecs_pair(first, second));
    }

    /** Mark component for auto-overriding.
     * @see override(flecs::id_t id)
     *
     * @tparam T The component to mark for overriding.
     */     
    template <typename T>
    Self& override() {
        return this->override(_::cpp_type<T>::id(this->m_world));
    }

    /** Mark pair for auto-overriding.
     * @see override(flecs::id_t id)
     *
     * @tparam First The first element of the pair.
     * @param second The second element of the pair.
     */     
    template <typename First>
    Self& override(flecs::entity_t second) {
        return this->override(_::cpp_type<First>::id(this->m_world), second);
    }

    /** Mark pair for auto-overriding.
     * @see override(flecs::id_t id)
     *
     * @tparam First The first element of the pair.
     * @tparam Second The second element of the pair.
     */     
    template <typename First, typename Second>
    Self& override() {
        return this->override<First>(_::cpp_type<Second>::id(this->m_world));
    }

    /** Set component, mark component for auto-overriding.
     * @see override(flecs::id_t id)
     *
     * @tparam T The component to set and for which to add the OVERRIDE flag
     */    
    template <typename T>
    Self& set_override(const T& val) {
        this->override<T>();
        return this->set<T>(val);
    }

    /** Set component, mark component for auto-overriding.
     * @see override(flecs::id_t id)
     *
     * @tparam T The component to set and for which to add the OVERRIDE flag
     */    
    template <typename T>
    Self& set_override(T&& val) {
        this->override<T>();
        return this->set<T>(FLECS_FWD(val));
    }

    /** Set pair, mark component for auto-overriding.
     * @see override(flecs::id_t id)
     *
     * @tparam First The first element of the pair.
     * @param second The second element of the pair.
     */    
    template <typename First>
    Self& set_override(flecs::entity_t second, const First& val) {
        this->override<First>(second);
        return this->set<First>(second, val);
    }

    /** Set pair, mark component for auto-overriding.
     * @see override(flecs::id_t id)
     *
     * @tparam First The first element of the pair.
     * @param second The second element of the pair.
     */    
    template <typename First>
    Self& set_override(flecs::entity_t second, First&& val) {
        this->override<First>(second);
        return this->set<First>(second, FLECS_FWD(val));
    }

    /** Set component, mark component for auto-overriding.
     * @see override(flecs::id_t id)
     *
     * @tparam First The first element of the pair.
     * @tparam Second The second element of the pair.
     */    
    template <typename First, typename Second, typename P = pair<First, Second>, 
        typename A = actual_type_t<P>, if_not_t< flecs::is_pair<First>::value> = 0>    
    Self& set_override(const A& val) {
        this->override<First, Second>();
        return this->set<First, Second>(val);
    }

    /** Set component, mark component for auto-overriding.
     * @see override(flecs::id_t id)
     *
     * @tparam First The first element of the pair.
     * @tparam Second The second element of the pair.
     */    
    template <typename First, typename Second, typename P = pair<First, Second>, 
        typename A = actual_type_t<P>, if_not_t< flecs::is_pair<First>::value> = 0>    
    Self& set_override(A&& val) {
        this->override<First, Second>();
        return this->set<First, Second>(FLECS_FWD(val));
    }

    /** Emplace component, mark component for auto-overriding.
     * @see override(flecs::id_t id)
     *
     * @tparam T The component to emplace and override.
     */    
    template <typename T, typename ... Args>
    Self& emplace_override(Args&&... args) {
        this->override<T>();

        flecs::emplace<T>(this->m_world, this->m_id, 
            _::cpp_type<T>::id(this->m_world), FLECS_FWD(args)...);

        return to_base();  
    }

    /** Emplace pair, mark pair for auto-overriding.
     * @see override(flecs::id_t id)
     *
     * @tparam First The first element of the pair to emplace and override.
     * @tparam Second The second element of the pair to emplace and override.
     */    
    template <typename First, typename Second, typename P = pair<First, Second>, 
        typename A = actual_type_t<P>, if_not_t< flecs::is_pair<First>::value> = 0,
            typename ... Args>
    Self& emplace_override(Args&&... args) {
        this->override<First, Second>();

        flecs::emplace<A>(this->m_world, this->m_id, 
            ecs_pair(_::cpp_type<First>::id(this->m_world),
                _::cpp_type<Second>::id(this->m_world)),
                    FLECS_FWD(args)...);

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
    Self& add(E value) {
        flecs::entity_t first = _::cpp_type<E>::id(this->m_world);
        const auto& et = enum_type<E>(this->m_world);
        flecs::entity_t second = et.entity(value);
        return this->add(first, second);
    }

    /** Remove pair for enum.
     * This operation will remove any (Enum, *) pair from the entity.
     * 
     * @tparam E The enumeration type.
     */
    template <typename E, if_t< is_enum<E>::value > = 0>
    Self& remove() {
        flecs::entity_t first = _::cpp_type<E>::id(this->m_world);
        return this->remove(first, flecs::Wildcard);
    }

    /** Enable an entity.
     * Enabled entities are matched with systems and can be searched with
     * queries.
     */
    Self& enable() {
        ecs_enable(this->m_world, this->m_id, true);
        return to_base();
    }

    /** Disable an entity.
     * Disabled entities are not matched with systems and cannot be searched 
     * with queries, unless explicitly specified in the query expression.
     */
    Self& disable() {
        ecs_enable(this->m_world, this->m_id, false);
        return to_base();
    }

    /** Enable an id.
     * This sets the enabled bit for this component. If this is the first time
     * the component is enabled or disabled, the bitset is added.
     * 
     * @param id The id to enable.
     * @param toggle True to enable, false to disable (default = true).
     */   
    Self& enable(flecs::id_t id, bool toggle = true) {
        ecs_enable_id(this->m_world, this->m_id, id, toggle);
        return to_base();       
    }

    /** Enable a component.
     * @see enable(flecs::id_t id)
     *
     * @tparam T The component to enable.
     */   
    template<typename T>
    Self& enable() {
        return this->enable(_::cpp_type<T>::id());
    }

    /** Enable a pair.
     * @see enable(flecs::id_t id)
     *
     * @param first The first element of the pair.
     * @param second The second element of the pair.
     */   
    Self& enable(flecs::id_t first, flecs::id_t second) {
        return this->enable(ecs_pair(first, second));
    }

    /** Enable a pair.
     * @see enable(flecs::id_t id)
     *
     * @tparam First The first element of the pair.
     * @param second The second element of the pair.
     */   
    template<typename First>
    Self& enable(flecs::id_t second) {
        return this->enable(_::cpp_type<First>::id(), second);
    }

    /** Enable a pair.
     * @see enable(flecs::id_t id)
     *
     * @tparam First The first element of the pair.
     * @tparam Second The second element of the pair.
     */   
    template<typename First, typename Second>
    Self& enable() {
        return this->enable<First>(_::cpp_type<Second>::id());
    }

    /** Disable an id.
     * This sets the enabled bit for this id. If this is the first time
     * the id is enabled or disabled, the bitset is added.
     *
     * @param id The id to disable.
     */   
    Self& disable(flecs::id_t id) {
        return this->enable(id, false);
    }

    /** Disable a component.
     * @see disable(flecs::id_t id)
     *
     * @tparam T The component to enable.
     */   
    template<typename T>
    Self& disable() {
        return this->disable(_::cpp_type<T>::id());
    }

    /** Disable a pair.
     * @see disable(flecs::id_t id)
     *
     * @param first The first element of the pair.
     * @param second The second element of the pair.
     */   
    Self& disable(flecs::id_t first, flecs::id_t second) {
        return this->disable(ecs_pair(first, second));
    }

    /** Disable a pair.
     * @see disable(flecs::id_t id)
     *
     * @tparam First The first element of the pair.
     * @param second The second element of the pair.
     */   
    template<typename First>
    Self& disable(flecs::id_t second) {
        return this->disable(_::cpp_type<First>::id(), second);
    }

    /** Disable a pair.
     * @see disable(flecs::id_t id)
     *
     * @tparam First The first element of the pair.
     * @tparam Second The second element of the pair.
     */   
    template<typename First, typename Second>
    Self& disable() {
        return this->disable<First>(_::cpp_type<Second>::id());
    }

    Self& set_ptr(entity_t comp, size_t size, const void *ptr) {
        ecs_set_id(this->m_world, this->m_id, comp, size, ptr);
        return to_base();
    }

    Self& set_ptr(entity_t comp, const void *ptr) {
        const flecs::Component *cptr = ecs_get(
            this->m_world, comp, EcsComponent);

        /* Can't set if it's not a component */
        ecs_assert(cptr != NULL, ECS_INVALID_PARAMETER, NULL);

        return set_ptr(comp, cptr->size, ptr);
    }

    template<typename T, if_t< 
        !is_callable<T>::value && is_actual<T>::value> = 0 >
    Self& set(T&& value) {
        flecs::set<T>(this->m_world, this->m_id, FLECS_FWD(value));
        return to_base();
    }

    template<typename T, if_t< 
        !is_callable<T>::value && is_actual<T>::value > = 0>
    Self& set(const T& value) {
        flecs::set<T>(this->m_world, this->m_id, value);
        return to_base();
    }

    template<typename T, typename A = actual_type_t<T>, if_not_t< 
        is_callable<T>::value || is_actual<T>::value > = 0>
    Self& set(A&& value) {
        flecs::set<T>(this->m_world, this->m_id, FLECS_FWD(value));
        return to_base();
    }

    template<typename T, typename A = actual_type_t<T>, if_not_t<
        is_callable<T>::value || is_actual<T>::value > = 0>
    Self& set(const A& value) {
        flecs::set<T>(this->m_world, this->m_id, value);
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
    Self& set(A&& value) {
        flecs::set<P>(this->m_world, this->m_id, FLECS_FWD(value));
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
    Self& set(const A& value) {
        flecs::set<P>(this->m_world, this->m_id, value);
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
    Self& set(Second second, const First& value) {
        auto first = _::cpp_type<First>::id(this->m_world);
        flecs::set(this->m_world, this->m_id, value, 
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
    Self& set(Second second, First&& value) {
        auto first = _::cpp_type<First>::id(this->m_world);
        flecs::set(this->m_world, this->m_id, FLECS_FWD(value), 
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
    Self& set(Second constant, const First& value) {
        const auto& et = enum_type<Second>(this->m_world);
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
    Self& set_second(entity_t first, const Second& value) {
        auto second = _::cpp_type<Second>::id(this->m_world);
        flecs::set(this->m_world, this->m_id, value, 
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
    Self& set_second(entity_t first, Second&& value) {
        auto second = _::cpp_type<Second>::id(this->m_world);
        flecs::set(this->m_world, this->m_id, FLECS_FWD(value), 
            ecs_pair(first, second));
        return to_base();
    }

    template <typename First, typename Second>
    Self& set_second(const Second& value) {
        flecs::set<pair_object<First, Second>>(this->m_world, this->m_id, value);
        return to_base();
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
    Self& set(const Func& func);

    /** Emplace component.
     * Emplace constructs a component in the storage, which prevents calling the
     * destructor on the value passed into the function.
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
    template<typename T, typename ... Args, typename A = actual_type_t<T>>
    Self& emplace(Args&&... args) {
        flecs::emplace<A>(this->m_world, this->m_id, 
            _::cpp_type<T>::id(this->m_world), FLECS_FWD(args)...);
        return to_base();
    }

    template <typename First, typename Second, typename ... Args, typename P = pair<First, Second>, 
        typename A = actual_type_t<P>, if_not_t< flecs::is_pair<First>::value> = 0>
    Self& emplace(Args&&... args) {
        flecs::emplace<A>(this->m_world, this->m_id, 
            ecs_pair(_::cpp_type<First>::id(this->m_world),
                _::cpp_type<Second>::id(this->m_world)),
            FLECS_FWD(args)...);
        return to_base();
    }

    template <typename First, typename ... Args>
    Self& emplace_first(flecs::entity_t second, Args&&... args) {
        flecs::emplace<First>(this->m_world, this->m_id, 
            ecs_pair(_::cpp_type<First>::id(this->m_world), second),
            FLECS_FWD(args)...);
        return to_base();
    }

    template <typename Second, typename ... Args>
    Self& emplace_second(flecs::entity_t first, Args&&... args) {
        flecs::emplace<Second>(this->m_world, this->m_id, 
            ecs_pair(first, _::cpp_type<Second>::id(this->m_world)),
            FLECS_FWD(args)...);
        return to_base();
    }

    /** Entities created in function will have the current entity.
     *
     * @param func The function to call.
     */
    template <typename Func>
    Self& with(const Func& func) {
        ecs_id_t prev = ecs_set_with(this->m_world, this->m_id);
        func();
        ecs_set_with(this->m_world, prev);
        return to_base();
    }

    /** Entities created in function will have (First, this).
     * This operation is thread safe.
     *
     * @tparam First The first element of the pair
     * @param func The function to call.
     */
    template <typename First, typename Func>
    Self& with(const Func& func) {
        with(_::cpp_type<First>::id(this->m_world), func);
        return to_base();
    }

    /** Entities created in function will have (first, this).
     *
     * @param first The first element of the pair.
     * @param func The function to call.
     */
    template <typename Func>
    Self& with(entity_t first, const Func& func) {
        ecs_id_t prev = ecs_set_with(this->m_world, 
            ecs_pair(first, this->m_id));
        func();
        ecs_set_with(this->m_world, prev);
        return to_base();
    }

    /** The function will be ran with the scope set to the current entity. */
    template <typename Func>
    Self& scope(const Func& func) {
        ecs_entity_t prev = ecs_set_scope(this->m_world, this->m_id);
        func();
        ecs_set_scope(this->m_world, prev);
        return to_base();
    }

    /* Set the entity name.
     */
    Self& set_name(const char *name) {
        ecs_set_name(this->m_world, this->m_id, name);
        return to_base();
    }

#   ifdef FLECS_DOC
#   include "../doc/entity_builder.inl"
#   endif

#   ifdef FLECS_META
#   include "../meta/entity_builder.inl"
#   endif

protected:
    Self& to_base() {
        return *static_cast<Self*>(this);
    }
};

}
