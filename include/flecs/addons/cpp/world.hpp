/**
 * @file addons/cpp/world.hpp
 * @brief World class.
 */

#pragma once

namespace flecs
{

/* Static helper functions to assign a component value */

// set(T&&), T = constructible
template <typename T, if_t< is_flecs_constructible<T>::value > = 0>
inline void set(world_t *world, flecs::entity_t entity, T&& value, flecs::id_t id) {
    ecs_assert(_::cpp_type<T>::size() != 0, ECS_INVALID_PARAMETER, NULL);

    if (!ecs_is_deferred(world)) {
        T& dst = *static_cast<T*>(ecs_get_mut_id(world, entity, id));
        dst = FLECS_MOV(value);

        ecs_modified_id(world, entity, id);
    } else {
        T& dst = *static_cast<T*>(ecs_get_mut_modified_id(world, entity, id));
        dst = FLECS_MOV(value);
    }
}

// set(const T&), T = constructible
template <typename T, if_t< is_flecs_constructible<T>::value > = 0>
inline void set(world_t *world, flecs::entity_t entity, const T& value, flecs::id_t id) {
    ecs_assert(_::cpp_type<T>::size() != 0, ECS_INVALID_PARAMETER, NULL);

    if (!ecs_is_deferred(world)) {
        T& dst = *static_cast<T*>(ecs_get_mut_id(world, entity, id));
        dst = FLECS_MOV(value);

        ecs_modified_id(world, entity, id);
    } else {
        T& dst = *static_cast<T*>(ecs_get_mut_modified_id(world, entity, id));
        dst = FLECS_MOV(value);
    }
}

// set(T&&), T = not constructible
template <typename T, if_not_t< is_flecs_constructible<T>::value > = 0>
inline void set(world_t *world, flecs::entity_t entity, T&& value, flecs::id_t id) {
    ecs_assert(_::cpp_type<T>::size() != 0, ECS_INVALID_PARAMETER, NULL);

    if (!ecs_is_deferred(world)) {
        T& dst = *static_cast<remove_reference_t<T>*>(ecs_get_mut_id(world, entity, id));
        dst = FLECS_MOV(value);

        ecs_modified_id(world, entity, id);
    } else {
        T& dst = *static_cast<remove_reference_t<T>*>(ecs_get_mut_modified_id(world, entity, id));
        dst = FLECS_MOV(value);
    }
}

// set(const T&), T = not constructible
template <typename T, if_not_t< is_flecs_constructible<T>::value > = 0>
inline void set(world_t *world, flecs::entity_t entity, const T& value, flecs::id_t id) {
    ecs_assert(_::cpp_type<T>::size() != 0, ECS_INVALID_PARAMETER, NULL);

    if (!ecs_is_deferred(world)) {
        T& dst = *static_cast<remove_reference_t<T>*>(ecs_get_mut_id(world, entity, id));
        dst = FLECS_MOV(value);

        ecs_modified_id(world, entity, id);
    } else {
        T& dst = *static_cast<remove_reference_t<T>*>(ecs_get_mut_modified_id(world, entity, id));
        dst = FLECS_MOV(value);
    }
}

// emplace for T(Args...)
template <typename T, typename ... Args, if_t< 
    std::is_constructible<actual_type_t<T>, Args...>::value ||
    std::is_default_constructible<actual_type_t<T>>::value > = 0>
inline void emplace(world_t *world, flecs::entity_t entity, flecs::id_t id, Args&&... args) {
    ecs_assert(_::cpp_type<T>::size() != 0, ECS_INVALID_PARAMETER, NULL);
    T& dst = *static_cast<T*>(ecs_emplace_id(world, entity, id));
    
    FLECS_PLACEMENT_NEW(&dst, T{FLECS_FWD(args)...});

    ecs_modified_id(world, entity, id);    
}

// set(T&&)
template <typename T, typename A>
inline void set(world_t *world, entity_t entity, A&& value) {
    id_t id = _::cpp_type<T>::id(world);
    flecs::set(world, entity, FLECS_FWD(value), id);
}

// set(const T&)
template <typename T, typename A>
inline void set(world_t *world, entity_t entity, const A& value) {
    id_t id = _::cpp_type<T>::id(world);
    flecs::set(world, entity, value, id);
}

/** Return id without generation.
 * 
 * @see ecs_strip_generation
 */
inline flecs::id_t strip_generation(flecs::entity_t e) {
    return ecs_strip_generation(e);
}

/** Return entity generation.
 */
inline uint32_t get_generation(flecs::entity_t e) {
    return ECS_GENERATION(e);
}

struct scoped_world;

/**
 * @defgroup cpp_world World
 * @brief World operations.
 * 
 * \ingroup cpp_core
 * @{
 */

/** The world.
 * The world is the container of all ECS data and systems. If the world is
 * deleted, all data in the world will be deleted as well.
 */
struct world {
    /** Create world.
     */
    explicit world() 
        : m_world( ecs_init() )
        , m_owned( true ) { init_builtin_components(); }

    /** Create world with command line arguments.
     * Currently command line arguments are not interpreted, but they may be
     * used in the future to configure Flecs parameters.
     */
    explicit world(int argc, char *argv[])
        : m_world( ecs_init_w_args(argc, argv) )
        , m_owned( true ) { init_builtin_components(); }

    /** Create world from C world.
     */
    explicit world(world_t *w)
        : m_world( w ) 
        , m_owned( false ) { }

    /** Not allowed to copy a world. May only take a reference. 
     */
    world(const world& obj) = delete;

    world(world&& obj) {
        m_world = obj.m_world;
        m_owned = obj.m_owned;
        obj.m_world = nullptr;
        obj.m_owned = false;
    }

    /* Implicit conversion to world_t* */
    operator world_t*() const { return m_world; }

    /** Not allowed to copy a world. May only take a reference.
     */
    world& operator=(const world& obj) = delete;

    world& operator=(world&& obj) {
        this->~world();

        m_world = obj.m_world;
        m_owned = obj.m_owned;
        obj.m_world = nullptr;
        obj.m_owned = false;
        return *this;
    }
    
    ~world() { 
        if (m_owned && ecs_stage_is_async(m_world)) {
            ecs_async_stage_free(m_world);
        } else
        if (m_owned && m_world) {
            ecs_fini(m_world); 
        }
    }

    /** Deletes and recreates the world. */
    void reset() {
        // Can only reset the world if we own the world object.
        ecs_assert(this->m_owned, ECS_INVALID_OPERATION, NULL);
        ecs_fini(m_world);
        m_world = ecs_init();
    }

    /** Obtain pointer to C world object.
     */
    world_t* c_ptr() const {
        return m_world;
    }

    /** Signal application should quit.
     * After calling this operation, the next call to progress() returns false.
     */
    void quit() const {
        ecs_quit(m_world);
    }

    /** Register action to be executed when world is destroyed.
     */
    void atfini(ecs_fini_action_t action, void *ctx) const {
        ecs_atfini(m_world, action, ctx);
    }

    /** Test if quit() has been called.
     */
    bool should_quit() const {
        return ecs_should_quit(m_world);
    }

    /** Begin frame.
     * When an application does not use progress() to control the main loop, it
     * can still use Flecs features such as FPS limiting and time measurements.
     * This operation needs to be invoked whenever a new frame is about to get 
     * processed.
     *
     * Calls to frame_begin must always be followed by frame_end.
     *
     * The function accepts a delta_time parameter, which will get passed to 
     * systems. This value is also used to compute the amount of time the 
     * function needs to sleep to ensure it does not exceed the target_fps, when 
     * it is set. When 0 is provided for delta_time, the time will be measured.
     *
     * This function should only be ran from the main thread.
     *
     * @param delta_time Time elapsed since the last frame.
     * @return The provided delta_time, or measured time if 0 was provided.
     */
    ecs_ftime_t frame_begin(float delta_time = 0) const {
        return ecs_frame_begin(m_world, delta_time);
    }

    /** End frame. 
     * This operation must be called at the end of the frame, and always after
     * ecs_frame_begin.
     *
     * This function should only be ran from the main thread.
     */
    void frame_end() const {
        ecs_frame_end(m_world);
    }

    /** Begin staging.
     * When an application does not use ecs_progress to control the main loop, it
     * can still use Flecs features such as the defer queue. When an application
     * needs to stage changes, it needs to call this function after ecs_frame_begin.
     * A call to ecs_readonly_begin must be followed by a call to ecs_readonly_end.
     * 
     * When staging is enabled, modifications to entities are stored to a stage.
     * This ensures that arrays are not modified while iterating. Modifications are
     * merged back to the "main stage" when ecs_readonly_end is invoked.
     *
     * While the world is in staging mode, no structural changes (add/remove/...)
     * can be made to the world itself. Operations must be executed on a stage
     * instead (see ecs_get_stage).
     *
     * This function should only be ran from the main thread.
     *
     * @return Whether world is currently staged.
     */
    bool readonly_begin() const {
        return ecs_readonly_begin(m_world);
    }

    /** End staging.
     * Leaves staging mode. After this operation the world may be directly mutated
     * again. By default this operation also merges data back into the world, unless
     * automerging was disabled explicitly.
     *
     * This function should only be ran from the main thread.
     */
    void readonly_end() const {
        ecs_readonly_end(m_world);
    }

    /** Defer operations until end of frame. 
     * When this operation is invoked while iterating, operations inbetween the
     * defer_begin and defer_end operations are executed at the end of the frame.
     *
     * This operation is thread safe.
     */
    bool defer_begin() const {
        return ecs_defer_begin(m_world);
    }

    /** End block of operations to defer. 
     * See defer_begin.
     *
     * This operation is thread safe.
     */
    bool defer_end() const {
        return ecs_defer_end(m_world);
    }

    /** Test whether deferring is enabled.
     */
    bool is_deferred() const {
        return ecs_is_deferred(m_world);
    }

    /** Configure world to have N stages.
     * This initializes N stages, which allows applications to defer operations to
     * multiple isolated defer queues. This is typically used for applications with
     * multiple threads, where each thread gets its own queue, and commands are
     * merged when threads are synchronized.
     *
     * Note that set_threads() already creates the appropriate number of stages. 
     * The set_stage_count() operation is useful for applications that want to manage 
     * their own stages and/or threads.
     * 
     * @param stages The number of stages.
     */
    void set_stage_count(int32_t stages) const {
        ecs_set_stage_count(m_world, stages);
    }

    /** Get number of configured stages.
     * Return number of stages set by set_stage_count.
     *
     * @return The number of stages used for threading.
     */
    int32_t get_stage_count() const {
        return ecs_get_stage_count(m_world);
    }

    /** Get current stage id.
     * The stage id can be used by an application to learn about which stage it
     * is using, which typically corresponds with the worker thread id.
     *
     * @return The stage id.
     */
    int32_t get_stage_id() const {
        return ecs_get_stage_id(m_world);
    }

    /** Test if is a stage.
     * If this function returns false, it is guaranteed that this is a valid
     * world object.
     * 
     * @return True if the world is a stage, false if not.
     */
    bool is_stage() const {
        ecs_assert(
            ecs_poly_is(m_world, ecs_world_t) || 
            ecs_poly_is(m_world, ecs_stage_t),
                ECS_INVALID_PARAMETER, NULL);
        return ecs_poly_is(m_world, ecs_stage_t);
    }

    /** Enable/disable automerging for world or stage.
     * When automerging is enabled, staged data will automatically be merged 
     * with the world when staging ends. This happens at the end of progress(), 
     * at a sync point or when readonly_end() is called.
     *
     * Applications can exercise more control over when data from a stage is 
     * merged by disabling automerging. This requires an application to 
     * explicitly call merge() on the stage.
     *
     * When this function is invoked on the world, it sets all current stages to
     * the provided value and sets the default for new stages. When this 
     * function is invoked on a stage, automerging is only set for that specific 
     * stage. 
     *
     * @param automerge Whether to enable or disable automerging.
     */
    void set_automerge(bool automerge) const {
        ecs_set_automerge(m_world, automerge);
    }

    /** Merge world or stage.
     * When automatic merging is disabled, an application can call this
     * operation on either an individual stage, or on the world which will merge
     * all stages. This operation may only be called when staging is not enabled
     * (either after progress() or after readonly_end()).
     *
     * This operation may be called on an already merged stage or world.
     */
    void merge() const {
        ecs_merge(m_world);
    }

    /** Get stage-specific world pointer.
     * Flecs threads can safely invoke the API as long as they have a private 
     * context to write to, also referred to as the stage. This function returns a
     * pointer to a stage, disguised as a world pointer.
     *
     * Note that this function does not(!) create a new world. It simply wraps the
     * existing world in a thread-specific context, which the API knows how to
     * unwrap. The reason the stage is returned as an ecs_world_t is so that it
     * can be passed transparently to the existing API functions, vs. having to 
     * create a dediated API for threading.
     *
     * @param stage_id The index of the stage to retrieve.
     * @return A thread-specific pointer to the world. 
     */
    flecs::world get_stage(int32_t stage_id) const {
        return flecs::world(ecs_get_stage(m_world, stage_id));
    }

    /** Create asynchronous stage.
     * An asynchronous stage can be used to asynchronously queue operations for
     * later merging with the world. An asynchronous stage is similar to a regular
     * stage, except that it does not allow reading from the world.
     *
     * Asynchronous stages are never merged automatically, and must therefore be
     * manually merged with the ecs_merge function. It is not necessary to call
     * defer_begin or defer_end before and after enqueuing commands, as an 
     * asynchronous stage unconditionally defers operations.
     *
     * The application must ensure that no commands are added to the stage while the
     * stage is being merged.
     *
     * An asynchronous stage must be cleaned up by ecs_async_stage_free. 
     *
     * @return The stage.
     */
    flecs::world async_stage() const {
        auto result = flecs::world(ecs_async_stage_new(m_world));
        result.m_owned = true;
        return result;
    }

    /** Get actual world.
     * If the current object points to a stage, this operation will return the
     * actual world.
     *
     * @return The actual world.
     */
    flecs::world get_world() const {
        /* Safe cast, mutability is checked */
        return flecs::world(
            m_world ? const_cast<flecs::world_t*>(ecs_get_world(m_world)) : nullptr);
    }

    /** Test whether the current world object is readonly.
     * This function allows the code to test whether the currently used world
     * object is readonly or whether it allows for writing.
     *
     * @return True if the world or stage is readonly.
     */
    bool is_readonly() const {
        return ecs_stage_is_readonly(m_world);
    }

    /** Set world context.
     * Set a context value that can be accessed by anyone that has a reference
     * to the world.
     *
     * @param ctx The world context.
     */
    void set_ctx(void* ctx, ecs_ctx_free_t ctx_free = nullptr) const {
        ecs_set_ctx(m_world, ctx, ctx_free);
    }

    /** Get world context.
     *
     * @return The configured world context.
     */
    void* get_ctx() const {
        return ecs_get_ctx(m_world);
    }

    /** Set world binding context.
     * Set a context value that can be accessed by anyone that has a reference
     * to the world.
     *
     * @param ctx The world context.
     */
    void set_binding_ctx(void* ctx, ecs_ctx_free_t ctx_free = nullptr) const {
        ecs_set_binding_ctx(m_world, ctx, ctx_free);
    }

    /** Get world binding context.
     *
     * @return The configured world context.
     */
    void* get_binding_ctx() const {
        return ecs_get_binding_ctx(m_world);
    }

    /** Preallocate memory for number of entities.
     * This function preallocates memory for the entity index.
     *
     * @param entity_count Number of entities to preallocate memory for.
     */
    void dim(int32_t entity_count) const {
        ecs_dim(m_world, entity_count);
    }

    /** Set entity range.
     * This function limits the range of issued entity ids between min and max.
     *
     * @param min Minimum entity id issued.
     * @param max Maximum entity id issued.
     */
    void set_entity_range(entity_t min, entity_t max) const {
        ecs_set_entity_range(m_world, min, max);
    }

    /** Enforce that operations cannot modify entities outside of range.
     * This function ensures that only entities within the specified range can
     * be modified. Use this function if specific parts of the code only are
     * allowed to modify a certain set of entities, as could be the case for
     * networked applications.
     *
     * @param enabled True if range check should be enabled, false if not.
     */
    void enable_range_check(bool enabled) const {
        ecs_enable_range_check(m_world, enabled);
    }

    /** Set current scope.
     *
     * @param scope The scope to set.
     * @return The current scope;
     * @see ecs_set_scope
     */
    flecs::entity set_scope(const flecs::entity_t scope) const;

    /** Get current scope.
     *
     * @return The current scope.
     * * @see ecs_get_scope
     */
    flecs::entity get_scope() const;

    /** Same as set_scope but with type.
     * * @see ecs_set_scope
     */
    template <typename T>
    flecs::entity set_scope() const;

    /** Set search path.
     *  @see ecs_set_lookup_path
     */
    flecs::entity_t* set_lookup_path(const flecs::entity_t *search_path) const {
        return ecs_set_lookup_path(m_world, search_path);
    }

    /** Lookup entity by name.
     * 
     * @param name Entity name.
     * @param search_path When false, only the current scope is searched.
     * @result The entity if found, or 0 if not found.
     */
    flecs::entity lookup(const char *name, bool search_path = true) const;

    /** Set singleton component.
     */
    template <typename T, if_t< !is_callable<T>::value > = 0>
    void set(const T& value) const {
        flecs::set<T>(m_world, _::cpp_type<T>::id(m_world), value);
    }

    /** Set singleton component.
     */
    template <typename T, if_t< !is_callable<T>::value > = 0>
    void set(T&& value) const {
        flecs::set<T>(m_world, _::cpp_type<T>::id(m_world), 
            FLECS_FWD(value));
    }

    /** Set singleton pair.
     */
    template <typename First, typename Second, typename P = flecs::pair<First, Second>, 
        typename A = actual_type_t<P>, if_not_t< flecs::is_pair<First>::value> = 0>
    void set(const A& value) const {
        flecs::set<P>(m_world, _::cpp_type<First>::id(m_world), value);
    }

    /** Set singleton pair.
     */
    template <typename First, typename Second, typename P = flecs::pair<First, Second>, 
        typename A = actual_type_t<P>, if_not_t< flecs::is_pair<First>::value> = 0>
    void set(A&& value) const {
        flecs::set<P>(m_world, _::cpp_type<First>::id(m_world), FLECS_FWD(value));
    }

    /** Set singleton pair.
     */
    template <typename First, typename Second>
    void set(Second second, const First& value) const;

    /** Set singleton pair.
     */
    template <typename First, typename Second>
    void set(Second second, First&& value) const;

    /** Set singleton component inside a callback.
     */
    template <typename Func, if_t< is_callable<Func>::value > = 0 >
    void set(const Func& func) const;

    template <typename T, typename ... Args>
    void emplace(Args&&... args) const {
        flecs::id_t component_id = _::cpp_type<T>::id(m_world);
        flecs::emplace<T>(m_world, component_id, component_id,
            FLECS_FWD(args)...);
    }        

    /** Get mut singleton component.
     */
    template <typename T>
    T* get_mut() const;

    /** Mark singleton component as modified.
     */
    template <typename T>
    void modified() const;

    /** Get ref singleton component.
     */
    template <typename T>
    ref<T> get_ref() const;

    /** Get singleton component.
     */
    template <typename T>
    const T* get() const;

    /** Get singleton pair.
     */
    template <typename First, typename Second, typename P = flecs::pair<First, Second>, 
        typename A = actual_type_t<P>>
    const A* get() const;

    /** Get singleton pair.
     */
    template <typename First, typename Second>
    const First* get(Second second) const;
    
    /** Get singleton component inside a callback.
     */
    template <typename Func, if_t< is_callable<Func>::value > = 0 >
    void get(const Func& func) const;

    /** Test if world has singleton component.
     */
    template <typename T>
    bool has() const;

    /** Test if world has the provided pair.
     * 
     * @tparam First The first element of the pair
     * @tparam Second The second element of the pair
     */
    template <typename First, typename Second>
    bool has() const;

    /** Test if world has the provided pair.
     * 
     * @tparam First The first element of the pair
     * @param second The second element of the pair.
     */
    template <typename First>
    bool has(flecs::id_t second) const;

    /** Test if world has the provided pair.
     * 
     * @param first The first element of the pair
     * @param second The second element of the pair
     */
    bool has(flecs::id_t first, flecs::id_t second) const;

    /** Add singleton component.
     */
    template <typename T>
    void add() const;

    /** Adds a pair to the singleton component.
     * 
     * @tparam First The first element of the pair
     * @tparam Second The second element of the pair
     */
    template <typename First, typename Second>
    void add() const;

    /** Adds a pair to the singleton component.
     * 
     * @tparam First The first element of the pair
     * @param second The second element of the pair.
     */
    template <typename First>
    void add(flecs::entity_t second) const;

    /** Adds a pair to the singleton entity.
     * 
     * @param first The first element of the pair
     * @param second The second element of the pair
     */
    void add(flecs::entity_t first, flecs::entity_t second) const;

    /** Remove singleton component.
     */
    template <typename T>
    void remove() const;

    /** Removes the pair singleton component.
     * 
     * @tparam First The first element of the pair
     * @tparam Second The second element of the pair
     */
    template <typename First, typename Second>
    void remove() const;

    /** Removes the pair singleton component.
     * 
     * @tparam First The first element of the pair
     * @param second The second element of the pair.
     */
    template <typename First>
    void remove(flecs::entity_t second) const;

    /** Removes the pair singleton component.
     * 
     * @param first The first element of the pair
     * @param second The second element of the pair
     */
    void remove(flecs::entity_t first, flecs::entity_t second) const;

    /** Iterate entities in root of world 
     * Accepts a callback with the following signature:
     *  void(*)(flecs::entity e);
     */
    template <typename Func>
    void children(Func&& f) const;

    /** Get singleton entity for type.
     */
    template <typename T>
    flecs::entity singleton() const;

    /** Get target for a given pair from a singleton entity.
     * This operation returns the target for a given pair. The optional
     * index can be used to iterate through targets, in case the entity has
     * multiple instances for the same relationship.
     *
     * @tparam First The first element of the pair.
     * @param index The index (0 for the first instance of the relationship).
     */
    template<typename First>
    flecs::entity target(int32_t index = 0) const;

    /** Get target for a given pair from a singleton entity.
     * This operation returns the target for a given pair. The optional
     * index can be used to iterate through targets, in case the entity has
     * multiple instances for the same relationship.
     *
     * @param first The first element of the pair for which to retrieve the target.
     * @param index The index (0 for the first instance of the relationship).
     */
    template<typename T>
    flecs::entity target(flecs::entity_t first, int32_t index = 0) const;

    /** Get target for a given pair from a singleton entity.
     * This operation returns the target for a given pair. The optional
     * index can be used to iterate through targets, in case the entity has
     * multiple instances for the same relationship.
     *
     * @param first The first element of the pair for which to retrieve the target.
     * @param index The index (0 for the first instance of the relationship).
     */
    flecs::entity target(flecs::entity_t first, int32_t index = 0) const;

    /** Create alias for component.
     *
     * @tparam T to create an alias for.
     * @param alias Alias for the component.
     * @return Entity representing the component.
     */
    template <typename T>
    flecs::entity use(const char *alias = nullptr) const;

    /** Create alias for entity.
     *
     * @param name Name of the entity.
     * @param alias Alias for the entity.
     */
    flecs::entity use(const char *name, const char *alias = nullptr) const;    

    /** Create alias for entity.
     *
     * @param entity Entity for which to create the alias.
     * @param alias Alias for the entity.
     */
    void use(flecs::entity entity, const char *alias = nullptr) const;   

    /** Count entities matching a component.
     *
     * @param component_id The component id.
     */
    int count(flecs::id_t component_id) const {
        return ecs_count_id(m_world, component_id);
    }

    /** Count entities matching a pair.
     *
     * @param first The first element of the pair.
     * @param second The second element of the pair.
     */
    int count(flecs::entity_t first, flecs::entity_t second) const {
        return ecs_count_id(m_world, ecs_pair(first, second));
    }

    /** Count entities matching a component.
     *
     * @tparam T The component type.
     */
    template <typename T>
    int count() const {
        return count(_::cpp_type<T>::id(m_world));
    }

    /** Count entities matching a pair.
     *
     * @tparam First The first element of the pair.
     * @param second The second element of the pair.
     */
    template <typename First>
    int count(flecs::entity_t second) const {
        return count(_::cpp_type<First>::id(m_world), second);
    }

    /** Count entities matching a pair.
     *
     * @tparam First The first element of the pair.
     * @tparam Second The second element of the pair.
     */
    template <typename First, typename Second>
    int count() const {
        return count( 
            _::cpp_type<First>::id(m_world),
            _::cpp_type<Second>::id(m_world));
    }

    /** All entities created in function are created with id.
     */
    template <typename Func>
    void with(id_t with_id, const Func& func) const {
        ecs_id_t prev = ecs_set_with(m_world, with_id);
        func();
        ecs_set_with(m_world, prev);    
    }

    /** All entities created in function are created with type.
     */
    template <typename T, typename Func>
    void with(const Func& func) const {
        with(this->id<T>(), func);
    }

    /** All entities created in function are created with pair.
     */
    template <typename First, typename Second, typename Func>
    void with(const Func& func) const {
        with(ecs_pair(this->id<First>(), this->id<Second>()), func);
    }

    /** All entities created in function are created with pair.
     */
    template <typename First, typename Func>
    void with(id_t second, const Func& func) const {
        with(ecs_pair(this->id<First>(), second), func);
    } 

    /** All entities created in function are created with pair.
     */
    template <typename Func>
    void with(id_t first, id_t second, const Func& func) const {
        with(ecs_pair(first, second), func);
    }

    /** All entities created in function are created in scope. All operations
     * called in function (such as lookup) are relative to scope.
     */
    template <typename Func>
    void scope(id_t parent, const Func& func) const {
        ecs_entity_t prev = ecs_set_scope(m_world, parent);
        func();
        ecs_set_scope(m_world, prev);
    }
    
    /** Same as scope(parent, func), but with T as parent.
     */
    template <typename T, typename Func>
    void scope(const Func& func) const {
        flecs::id_t parent = _::cpp_type<T>::id(m_world);
        scope(parent, func);
    }

    /** Use provided scope for operations ran on returned world.
     * Operations need to be ran in a single statement.
     */
    flecs::scoped_world scope(id_t parent) const;

    template <typename T>
    flecs::scoped_world scope() const;

    flecs::scoped_world scope(const char* name) const;

    /** Delete all entities with specified id. */
    void delete_with(id_t the_id) const {
        ecs_delete_with(m_world, the_id);
    }

    /** Delete all entities with specified pair. */
    void delete_with(entity_t first, entity_t second) const {
        delete_with(ecs_pair(first, second));
    }

    /** Delete all entities with specified component. */
    template <typename T>
    void delete_with() const {
        delete_with(_::cpp_type<T>::id(m_world));
    }

    /** Delete all entities with specified pair. */
    template <typename First, typename Second>
    void delete_with() const {
        delete_with(_::cpp_type<First>::id(m_world), _::cpp_type<Second>::id(m_world));
    }

    /** Delete all entities with specified pair. */
    template <typename First>
    void delete_with(entity_t second) const {
        delete_with(_::cpp_type<First>::id(m_world), second);
    }

    /** Remove all instances of specified id. */
    void remove_all(id_t the_id) const {
        ecs_remove_all(m_world, the_id);
    }

    /** Remove all instances of specified pair. */
    void remove_all(entity_t first, entity_t second) const {
        remove_all(ecs_pair(first, second));
    }

    /** Remove all instances of specified component. */
    template <typename T>
    void remove_all() const {
        remove_all(_::cpp_type<T>::id(m_world));
    }

    /** Remove all instances of specified pair. */
    template <typename First, typename Second>
    void remove_all() const {
        remove_all(_::cpp_type<First>::id(m_world), _::cpp_type<Second>::id(m_world));
    }

    /** Remove all instances of specified pair. */
    template <typename First>
    void remove_all(entity_t second) const {
        remove_all(_::cpp_type<First>::id(m_world), second);
    }

    /** Defer all operations called in function. If the world is already in
     * deferred mode, do nothing.
     */
    template <typename Func>
    void defer(const Func& func) const {
        ecs_defer_begin(m_world);
        func();
        ecs_defer_end(m_world);
    }

    /** Suspend deferring operations.
     * 
     * @see ecs_defer_suspend
     */
    void defer_suspend() const {
        ecs_defer_suspend(m_world);
    }

    /** Resume deferring operations.
     * 
     * @see ecs_defer_suspend
     */
    void defer_resume() const {
        ecs_defer_resume(m_world);
    }

    /** Check if entity id exists in the world.
     * 
     * @see ecs_exists
     */
    bool exists(flecs::entity_t e) const {
        return ecs_exists(m_world, e);
    }

    /** Check if entity id exists in the world.
     *
     * @see ecs_is_alive
     */
    bool is_alive(flecs::entity_t e) const {
        return ecs_is_alive(m_world, e);
    }

    /** Check if entity id is valid.
     * Invalid entities cannot be used with API functions.
     * 
     * @see ecs_is_valid
     */
    bool is_valid(flecs::entity_t e) const {
        return ecs_is_valid(m_world, e);
    }

    /** Get alive entity for id.
     * Returns the entity with the current generation.
     * 
     * @see ecs_get_alive
     */
    flecs::entity get_alive(flecs::entity_t e) const;

/* Prevent clashing with Unreal define. Unreal applications will have to use
 *  ecs_ensure. */
#ifndef ensure
    /** Ensures that entity with provided generation is alive.
     * Ths operation will fail if an entity exists with the same id and a 
     * different, non-zero generation.
     * 
     * @see ecs_ensure
     */
    flecs::entity ensure(flecs::entity_t e) const;
#endif

    /* Run callback after completing frame */
    void run_post_frame(ecs_fini_action_t action, void *ctx) const {
        ecs_run_post_frame(m_world, action, ctx);
    }

    /** Get the world info.
     * @see ecs_get_world_info
     */
    const flecs::world_info_t* get_info() const{
        return ecs_get_world_info(m_world);
    }

    /** Get delta_time */
    ecs_ftime_t delta_time() const {
        return get_info()->delta_time;
    }

#   include "mixins/id/mixin.inl"
#   include "mixins/component/mixin.inl"
#   include "mixins/entity/mixin.inl"
#   include "mixins/event/mixin.inl"
#   include "mixins/term/mixin.inl"
#   include "mixins/filter/mixin.inl"
#   include "mixins/observer/mixin.inl"
#   include "mixins/query/mixin.inl"
#   include "mixins/enum/mixin.inl"

#   ifdef FLECS_MODULE
#   include "mixins/module/mixin.inl"
#   endif
#   ifdef FLECS_PIPELINE
#   include "mixins/pipeline/mixin.inl"
#   endif
#   ifdef FLECS_SNAPSHOT
#   include "mixins/snapshot/mixin.inl"
#   endif
#   ifdef FLECS_SYSTEM
#   include "mixins/system/mixin.inl"
#   endif
#   ifdef FLECS_TIMER
#   include "mixins/timer/mixin.inl"
#   endif
#   ifdef FLECS_RULES
#   include "mixins/rule/mixin.inl"
#   endif
#   ifdef FLECS_PLECS
#   include "mixins/plecs/mixin.inl"
#   endif
#   ifdef FLECS_META
#   include "mixins/meta/world.inl"
#   endif
#   ifdef FLECS_JSON
#   include "mixins/json/world.inl"
#   endif
#   ifdef FLECS_APP
#   include "mixins/app/mixin.inl"
#   endif
#   ifdef FLECS_METRICS
#   include "mixins/metrics/mixin.inl"
#   endif
#   ifdef FLECS_ALERTS
#   include "mixins/alerts/mixin.inl"
#   endif

public:
    void init_builtin_components();

    world_t *m_world;
    bool m_owned;
};

/** Scoped world.
 * Utility class used by the world::scope method to create entities in a scope.
 */
struct scoped_world : world {
    scoped_world(
        flecs::world_t *w, 
        flecs::entity_t s) : world(nullptr)
    {
        m_prev_scope = ecs_set_scope(w, s);
        m_world = w;
        m_owned = false;
    }

    ~scoped_world() {
        ecs_set_scope(m_world, m_prev_scope);
    }

    scoped_world(const scoped_world& obj) : world(nullptr) {
        m_prev_scope = obj.m_prev_scope;
        m_world = obj.m_world;
        m_owned = obj.m_owned;
    }

    flecs::entity_t m_prev_scope;
};

/** @} */

} // namespace flecs
