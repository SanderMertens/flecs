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
    ecs_assert(_::type<T>::size() != 0, ECS_INVALID_PARAMETER,
            "operation invalid for empty type");

    if (!ecs_is_deferred(world)) {
        T& dst = *static_cast<T*>(ecs_ensure_id(world, entity, id));
        dst = FLECS_MOV(value);

        ecs_modified_id(world, entity, id);
    } else {
        T& dst = *static_cast<T*>(ecs_ensure_modified_id(world, entity, id));
        dst = FLECS_MOV(value);
    }
}

// set(const T&), T = constructible
template <typename T, if_t< is_flecs_constructible<T>::value > = 0>
inline void set(world_t *world, flecs::entity_t entity, const T& value, flecs::id_t id) {
    ecs_assert(_::type<T>::size() != 0, ECS_INVALID_PARAMETER,
            "operation invalid for empty type");

    if (!ecs_is_deferred(world)) {
        T& dst = *static_cast<T*>(ecs_ensure_id(world, entity, id));
        dst = FLECS_MOV(value);

        ecs_modified_id(world, entity, id);
    } else {
        T& dst = *static_cast<T*>(ecs_ensure_modified_id(world, entity, id));
        dst = FLECS_MOV(value);
    }
}

// set(T&&), T = not constructible
template <typename T, if_not_t< is_flecs_constructible<T>::value > = 0>
inline void set(world_t *world, flecs::entity_t entity, T&& value, flecs::id_t id) {
    ecs_assert(_::type<T>::size() != 0, ECS_INVALID_PARAMETER,
            "operation invalid for empty type");

    if (!ecs_is_deferred(world)) {
        T& dst = *static_cast<remove_reference_t<T>*>(ecs_ensure_id(world, entity, id));
        dst = FLECS_MOV(value);

        ecs_modified_id(world, entity, id);
    } else {
        T& dst = *static_cast<remove_reference_t<T>*>(ecs_ensure_modified_id(world, entity, id));
        dst = FLECS_MOV(value);
    }
}

// set(const T&), T = not constructible
template <typename T, if_not_t< is_flecs_constructible<T>::value > = 0>
inline void set(world_t *world, flecs::entity_t entity, const T& value, flecs::id_t id) {
    ecs_assert(_::type<T>::size() != 0, ECS_INVALID_PARAMETER,
            "operation invalid for empty type");

    if (!ecs_is_deferred(world)) {
        T& dst = *static_cast<remove_reference_t<T>*>(ecs_ensure_id(world, entity, id));
        dst = FLECS_MOV(value);

        ecs_modified_id(world, entity, id);
    } else {
        T& dst = *static_cast<remove_reference_t<T>*>(ecs_ensure_modified_id(world, entity, id));
        dst = FLECS_MOV(value);
    }
}

// emplace for T(Args...)
template <typename T, typename ... Args, if_t<
    std::is_constructible<actual_type_t<T>, Args...>::value ||
    std::is_default_constructible<actual_type_t<T>>::value > = 0>
inline void emplace(world_t *world, flecs::entity_t entity, flecs::id_t id, Args&&... args) {
    ecs_assert(_::type<T>::size() != 0, ECS_INVALID_PARAMETER,
            "operation invalid for empty type");
    T& dst = *static_cast<T*>(ecs_emplace_id(world, entity, id, nullptr));

    FLECS_PLACEMENT_NEW(&dst, T{FLECS_FWD(args)...});

    ecs_modified_id(world, entity, id);
}

// set(T&&)
template <typename T, typename A>
inline void set(world_t *world, entity_t entity, A&& value) {
    id_t id = _::type<T>::id(world);
    flecs::set(world, entity, FLECS_FWD(value), id);
}

// set(const T&)
template <typename T, typename A>
inline void set(world_t *world, entity_t entity, const A& value) {
    id_t id = _::type<T>::id(world);
    flecs::set(world, entity, value, id);
}

/** Return id without generation.
 *
 * @see ecs_strip_generation()
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
 * @ingroup cpp_core
 * World operations.
 *
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
        : world_( ecs_init() ) { 
            init_builtin_components(); 
        }

    /** Create world with command line arguments.
     * Currently command line arguments are not interpreted, but they may be
     * used in the future to configure Flecs parameters.
     */
    explicit world(int argc, char *argv[])
        : world_( ecs_init_w_args(argc, argv) ) { 
            init_builtin_components(); 
        }

    /** Create world from C world.
     */
    explicit world(world_t *w)
        : world_( w ) { 
            if (w) {
                flecs_poly_claim(w);
            }
        }

    /** Not allowed to copy a world. May only take a reference.
     */
    world(const world& obj) {
        this->world_ = obj.world_;
        flecs_poly_claim(this->world_);
    }

    world& operator=(const world& obj) noexcept {
        this->world_ = obj.world_;
        flecs_poly_claim(this->world_);
        return *this;
    }

    world(world&& obj) noexcept {
        world_ = obj.world_;
        obj.world_ = nullptr;
    }

    world& operator=(world&& obj) noexcept {
        world_ = obj.world_;
        obj.world_ = nullptr;
        return *this;
    }

    ~world() {
        if (world_) {
            if (!flecs_poly_release(world_)) {
                if (ecs_stage_get_id(world_) == -1) {
                    ecs_stage_free(world_);
                } else {
                    // before we call ecs_fini(), we increment the reference count back to 1
                    // otherwise, copies of this object created during ecs_fini (e.g. a component on_remove hook)
                    // would call again this destructor and ecs_fini().
                    flecs_poly_claim(world_);
                    ecs_fini(world_);
                }
            }
        }
    }

    /* Implicit conversion to world_t* */
    operator world_t*() const { return world_; }

    /** Make current world object owner of the world. This may only be called on
     * one flecs::world object, an may only be called  once. Failing to do so
     * will result in undefined behavior.
     * 
     * This operation allows a custom (C) world to be wrapped by a C++ object,
     * and transfer ownership so that the world is automatically cleaned up.
     */
    void make_owner() {
        flecs_poly_release(world_);
    }

    /** Deletes and recreates the world. */
    void reset() {
        /* Make sure there's only one reference to the world */
        ecs_assert(flecs_poly_refcount(world_) == 1, ECS_INVALID_OPERATION,
            "reset would invalidate other handles");
        ecs_fini(world_);
        world_ = ecs_init();
    }

    /** Obtain pointer to C world object.
     */
    world_t* c_ptr() const {
        return world_;
    }

    /** Signal application should quit.
     * After calling this operation, the next call to progress() returns false.
     */
    void quit() const {
        ecs_quit(world_);
    }

    /** Register action to be executed when world is destroyed.
     */
    void atfini(ecs_fini_action_t action, void *ctx = nullptr) const {
        ecs_atfini(world_, action, ctx);
    }

    /** Test if quit() has been called.
     */
    bool should_quit() const {
        return ecs_should_quit(world_);
    }

    /** Begin frame.
     * When an application does not use progress() to control the main loop, it
     * can still use Flecs features such as FPS limiting and time measurements.
     * This operation needs to be invoked whenever a new frame is about to get
     * processed.
     *
     * Calls to frame_begin() must always be followed by frame_end().
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
     *
     * @see ecs_frame_begin()
     * @see flecs::world::frame_end()
     */
    ecs_ftime_t frame_begin(float delta_time = 0) const {
        return ecs_frame_begin(world_, delta_time);
    }

    /** End frame.
     * This operation must be called at the end of the frame, and always after
     * frame_begin().
     *
     * This function should only be ran from the main thread.
     *
     * @see ecs_frame_end()
     * @see flecs::world::frame_begin()
     */
    void frame_end() const {
        ecs_frame_end(world_);
    }

    /** Begin readonly mode.
     *
     * @param multi_threaded Whether to enable readonly/multi threaded mode.
     * 
     * @return Whether world is currently readonly.
     *
     * @see ecs_readonly_begin()
     * @see flecs::world::is_readonly()
     * @see flecs::world::readonly_end()
     */
    bool readonly_begin(bool multi_threaded = false) const {
        return ecs_readonly_begin(world_, multi_threaded);
    }

    /** End readonly mode.
     * 
     * @see ecs_readonly_end()
     * @see flecs::world::is_readonly()
     * @see flecs::world::readonly_begin()
     */
    void readonly_end() const {
        ecs_readonly_end(world_);
    }

    /** Defer operations until end of frame.
     * When this operation is invoked while iterating, operations inbetween the
     * defer_begin() and defer_end() operations are executed at the end of the frame.
     *
     * This operation is thread safe.
     *
     * @return true if world changed from non-deferred mode to deferred mode.
     *
     * @see ecs_defer_begin()
     * @see flecs::world::defer()
     * @see flecs::world::defer_end()
     * @see flecs::world::is_deferred()
     * @see flecs::world::defer_resume()
     * @see flecs::world::defer_suspend()
     */
    bool defer_begin() const {
        return ecs_defer_begin(world_);
    }

    /** End block of operations to defer.
     * See defer_begin().
     *
     * This operation is thread safe.
     *
     * @return true if world changed from deferred mode to non-deferred mode.
     *
     * @see ecs_defer_end()
     * @see flecs::world::defer()
     * @see flecs::world::defer_begin()
     * @see flecs::world::is_deferred()
     * @see flecs::world::defer_resume()
     * @see flecs::world::defer_suspend()
     */
    bool defer_end() const {
        return ecs_defer_end(world_);
    }

    /** Test whether deferring is enabled.
     *
     * @return True if deferred, false if not.
     *
     * @see ecs_is_deferred()
     * @see flecs::world::defer()
     * @see flecs::world::defer_begin()
     * @see flecs::world::defer_end()
     * @see flecs::world::defer_resume()
     * @see flecs::world::defer_suspend()
     */
    bool is_deferred() const {
        return ecs_is_deferred(world_);
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
     *
     * @see ecs_set_stage_count()
     * @see flecs::world::get_stage_count()
     */
    void set_stage_count(int32_t stages) const {
        ecs_set_stage_count(world_, stages);
    }

    /** Get number of configured stages.
     * Return number of stages set by set_stage_count().
     *
     * @return The number of stages used for threading.
     *
     * @see ecs_get_stage_count()
     * @see flecs::world::set_stage_count()
     */
    int32_t get_stage_count() const {
        return ecs_get_stage_count(world_);
    }

    /** Get current stage id.
     * The stage id can be used by an application to learn about which stage it
     * is using, which typically corresponds with the worker thread id.
     *
     * @return The stage id.
     */
    int32_t get_stage_id() const {
        return ecs_stage_get_id(world_);
    }

    /** Test if is a stage.
     * If this function returns false, it is guaranteed that this is a valid
     * world object.
     *
     * @return True if the world is a stage, false if not.
     */
    bool is_stage() const {
        ecs_assert(
            flecs_poly_is(world_, ecs_world_t) ||
            flecs_poly_is(world_, ecs_stage_t), 
                ECS_INVALID_PARAMETER, 
                "flecs::world instance contains invalid reference to world or stage");
        return flecs_poly_is(world_, ecs_stage_t);
    }

    /** Merge world or stage.
     * When automatic merging is disabled, an application can call this
     * operation on either an individual stage, or on the world which will merge
     * all stages. This operation may only be called when staging is not enabled
     * (either after progress() or after readonly_end()).
     *
     * This operation may be called on an already merged stage or world.
     *
     * @see ecs_merge()
     */
    void merge() const {
        ecs_merge(world_);
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
        return flecs::world(ecs_get_stage(world_, stage_id));
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
     * @return The stage.
     */
    flecs::world async_stage() const {
        ecs_world_t *as = ecs_stage_new(world_);
        flecs_poly_release(as); // world object will claim
        return flecs::world(as);
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
            world_ ? const_cast<flecs::world_t*>(ecs_get_world(world_)) : nullptr);
    }

    /** Test whether the current world object is readonly.
     * This function allows the code to test whether the currently used world
     * object is readonly or whether it allows for writing.
     *
     * @return True if the world or stage is readonly.
     *
     * @see ecs_stage_is_readonly()
     * @see flecs::world::readonly_begin()
     * @see flecs::world::readonly_end()
     */
    bool is_readonly() const {
        return ecs_stage_is_readonly(world_);
    }

    /** Set world context.
     * Set a context value that can be accessed by anyone that has a reference
     * to the world.
     *
     * @param ctx A pointer to a user defined structure.
     * @param ctx_free A function that is invoked with ctx when the world is freed.
     *
     *
     * @see ecs_set_ctx()
     * @see flecs::world::get_ctx()
     */
    void set_ctx(void* ctx, ecs_ctx_free_t ctx_free = nullptr) const {
        ecs_set_ctx(world_, ctx, ctx_free);
    }

    /** Get world context.
     * This operation retrieves a previously set world context.
     *
     * @return The context set with set_binding_ctx(). If no context was set, the
     *         function returns NULL.
     *
     * @see ecs_get_ctx()
     * @see flecs::world::set_ctx()
     */
    void* get_ctx() const {
        return ecs_get_ctx(world_);
    }

    /** Set world binding context.
     *
     * Same as set_ctx() but for binding context. A binding context is intended
     * specifically for language bindings to store binding specific data.
     *
     * @param ctx A pointer to a user defined structure.
     * @param ctx_free A function that is invoked with ctx when the world is freed.
     *
     * @see ecs_set_binding_ctx()
     * @see flecs::world::get_binding_ctx()
     */
    void set_binding_ctx(void* ctx, ecs_ctx_free_t ctx_free = nullptr) const {
        ecs_set_binding_ctx(world_, ctx, ctx_free);
    }

    /** Get world binding context.
     * This operation retrieves a previously set world binding context.
     *
     * @return The context set with set_binding_ctx(). If no context was set, the
     *         function returns NULL.
     *
     * @see ecs_get_binding_ctx()
     * @see flecs::world::set_binding_ctx()
     */
    void* get_binding_ctx() const {
        return ecs_get_binding_ctx(world_);
    }

    /** Preallocate memory for number of entities.
     * This function preallocates memory for the entity index.
     *
     * @param entity_count Number of entities to preallocate memory for.
     *
     * @see ecs_dim()
     */
    void dim(int32_t entity_count) const {
        ecs_dim(world_, entity_count);
    }

    /** Set entity range.
     * This function limits the range of issued entity ids between min and max.
     *
     * @param min Minimum entity id issued.
     * @param max Maximum entity id issued.
     *
     * @see ecs_set_entity_range()
     */
    void set_entity_range(entity_t min, entity_t max) const {
        ecs_set_entity_range(world_, min, max);
    }

    /** Enforce that operations cannot modify entities outside of range.
     * This function ensures that only entities within the specified range can
     * be modified. Use this function if specific parts of the code only are
     * allowed to modify a certain set of entities, as could be the case for
     * networked applications.
     *
     * @param enabled True if range check should be enabled, false if not.
     *
     * @see ecs_enable_range_check()
     */
    void enable_range_check(bool enabled = true) const {
        ecs_enable_range_check(world_, enabled);
    }

    /** Set current scope.
     *
     * @param scope The scope to set.
     * @return The current scope;
     *
     * @see ecs_set_scope()
     * @see flecs::world::get_scope()
     */
    flecs::entity set_scope(const flecs::entity_t scope) const;

    /** Get current scope.
     *
     * @return The current scope.
     *
     * @see ecs_get_scope()
     * @see flecs::world::set_scope()
     */
    flecs::entity get_scope() const;

    /** Same as set_scope but with type.
     *
     * @see ecs_set_scope()
     * @see flecs::world::get_scope()
     */
    template <typename T>
    flecs::entity set_scope() const;

    /** Set search path.
     *
     * @see ecs_set_lookup_path()
     * @see flecs::world::lookup()
     */
    flecs::entity_t* set_lookup_path(const flecs::entity_t *search_path) const {
        return ecs_set_lookup_path(world_, search_path);
    }

    /** Lookup entity by name.
     *
     * @param name Entity name.
     * @param recursive When false, only the current scope is searched.
     * @result The entity if found, or 0 if not found.
     */
    flecs::entity lookup(const char *name, const char *sep = "::", const char *root_sep = "::", bool recursive = true) const;

    /** Set singleton component.
     */
    template <typename T, if_t< !is_callable<T>::value > = 0>
    void set(const T& value) const {
        flecs::set<T>(world_, _::type<T>::id(world_), value);
    }

    /** Set singleton component.
     */
    template <typename T, if_t< !is_callable<T>::value > = 0>
    void set(T&& value) const {
        flecs::set<T>(world_, _::type<T>::id(world_),
            FLECS_FWD(value));
    }

    /** Set singleton pair.
     */
    template <typename First, typename Second, typename P = flecs::pair<First, Second>,
        typename A = actual_type_t<P>, if_not_t< flecs::is_pair<First>::value> = 0>
    void set(const A& value) const {
        flecs::set<P>(world_, _::type<First>::id(world_), value);
    }

    /** Set singleton pair.
     */
    template <typename First, typename Second, typename P = flecs::pair<First, Second>,
        typename A = actual_type_t<P>, if_not_t< flecs::is_pair<First>::value> = 0>
    void set(A&& value) const {
        flecs::set<P>(world_, _::type<First>::id(world_), FLECS_FWD(value));
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
        flecs::id_t component_id = _::type<T>::id(world_);
        flecs::emplace<T>(world_, component_id, component_id, FLECS_FWD(args)...);
    }

    /** Ensure singleton component.
     */
    #ifndef ensure
    template <typename T>
    T& ensure() const;
    #endif

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

    /** Get mutable singleton component.
     */
    template <typename T>
    T* get_mut() const;

    /** Get mutable singleton pair.
     */
    template <typename First, typename Second, typename P = flecs::pair<First, Second>,
        typename A = actual_type_t<P>>
    A* get_mut() const;

    /** Get mutable singleton pair.
     */
    template <typename First, typename Second>
    First* get_mut(Second second) const;

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
     *
     * @code
     * void(*)(flecs::entity e);
     * @endcode
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
        return ecs_count_id(world_, component_id);
    }

    /** Count entities matching a pair.
     *
     * @param first The first element of the pair.
     * @param second The second element of the pair.
     */
    int count(flecs::entity_t first, flecs::entity_t second) const {
        return ecs_count_id(world_, ecs_pair(first, second));
    }

    /** Count entities matching a component.
     *
     * @tparam T The component type.
     */
    template <typename T>
    int count() const {
        return count(_::type<T>::id(world_));
    }

    /** Count entities matching a pair.
     *
     * @tparam First The first element of the pair.
     * @param second The second element of the pair.
     */
    template <typename First>
    int count(flecs::entity_t second) const {
        return count(_::type<First>::id(world_), second);
    }

    /** Count entities matching a pair.
     *
     * @tparam First The first element of the pair.
     * @tparam Second The second element of the pair.
     */
    template <typename First, typename Second>
    int count() const {
        return count(
            _::type<First>::id(world_),
            _::type<Second>::id(world_));
    }

    /** All entities created in function are created with id.
     */
    template <typename Func>
    void with(id_t with_id, const Func& func) const {
        ecs_id_t prev = ecs_set_with(world_, with_id);
        func();
        ecs_set_with(world_, prev);
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
        ecs_entity_t prev = ecs_set_scope(world_, parent);
        func();
        ecs_set_scope(world_, prev);
    }

    /** Same as scope(parent, func), but with T as parent.
     */
    template <typename T, typename Func>
    void scope(const Func& func) const {
        flecs::id_t parent = _::type<T>::id(world_);
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
        ecs_delete_with(world_, the_id);
    }

    /** Delete all entities with specified pair. */
    void delete_with(entity_t first, entity_t second) const {
        delete_with(ecs_pair(first, second));
    }

    /** Delete all entities with specified component. */
    template <typename T>
    void delete_with() const {
        delete_with(_::type<T>::id(world_));
    }

    /** Delete all entities with specified pair. */
    template <typename First, typename Second>
    void delete_with() const {
        delete_with(_::type<First>::id(world_), _::type<Second>::id(world_));
    }

    /** Delete all entities with specified pair. */
    template <typename First>
    void delete_with(entity_t second) const {
        delete_with(_::type<First>::id(world_), second);
    }

    /** Remove all instances of specified id. */
    void remove_all(id_t the_id) const {
        ecs_remove_all(world_, the_id);
    }

    /** Remove all instances of specified pair. */
    void remove_all(entity_t first, entity_t second) const {
        remove_all(ecs_pair(first, second));
    }

    /** Remove all instances of specified component. */
    template <typename T>
    void remove_all() const {
        remove_all(_::type<T>::id(world_));
    }

    /** Remove all instances of specified pair. */
    template <typename First, typename Second>
    void remove_all() const {
        remove_all(_::type<First>::id(world_), _::type<Second>::id(world_));
    }

    /** Remove all instances of specified pair. */
    template <typename First>
    void remove_all(entity_t second) const {
        remove_all(_::type<First>::id(world_), second);
    }

    /** Defer all operations called in function.
     *
     * @see flecs::world::defer_begin()
     * @see flecs::world::defer_end()
     * @see flecs::world::defer_is_deferred()
     * @see flecs::world::defer_resume()
     * @see flecs::world::defer_suspend()
     */
    template <typename Func>
    void defer(const Func& func) const {
        ecs_defer_begin(world_);
        func();
        ecs_defer_end(world_);
    }

    /** Suspend deferring operations.
     *
     * @see ecs_defer_suspend()
     * @see flecs::world::defer()
     * @see flecs::world::defer_begin()
     * @see flecs::world::defer_end()
     * @see flecs::world::defer_is_deferred()
     * @see flecs::world::defer_resume()
     */
    void defer_suspend() const {
        ecs_defer_suspend(world_);
    }

    /** Resume deferring operations.
     *
     * @see ecs_defer_resume()
     * @see flecs::world::defer()
     * @see flecs::world::defer_begin()
     * @see flecs::world::defer_end()
     * @see flecs::world::defer_is_deferred()
     * @see flecs::world::defer_suspend()
     */
    void defer_resume() const {
        ecs_defer_resume(world_);
    }

    /** Check if entity id exists in the world.
     *
     * @see ecs_exists()
     * @see flecs::world::is_alive()
     * @see flecs::world::is_valid()
     */
    bool exists(flecs::entity_t e) const {
        return ecs_exists(world_, e);
    }

    /** Check if entity id exists in the world.
     *
     * @see ecs_is_alive()
     * @see flecs::world::exists()
     * @see flecs::world::is_valid()
     */
    bool is_alive(flecs::entity_t e) const {
        return ecs_is_alive(world_, e);
    }

    /** Check if entity id is valid.
     * Invalid entities cannot be used with API functions.
     *
     * @see ecs_is_valid()
     * @see flecs::world::exists()
     * @see flecs::world::is_alive()
     */
    bool is_valid(flecs::entity_t e) const {
        return ecs_is_valid(world_, e);
    }

    /** Get alive entity for id.
     * Returns the entity with the current generation.
     *
     * @see ecs_get_alive()
     */
    flecs::entity get_alive(flecs::entity_t e) const;

    /**
     * @see ecs_make_alive()
     */
    flecs::entity make_alive(flecs::entity_t e) const;

    /* Run callback after completing frame */
    void run_post_frame(ecs_fini_action_t action, void *ctx) const {
        ecs_run_post_frame(world_, action, ctx);
    }

    /** Get the world info.
     *
     * @see ecs_get_world_info()
     */
    const flecs::world_info_t* get_info() const{
        return ecs_get_world_info(world_);
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
#   include "mixins/observer/mixin.inl"
#   include "mixins/query/mixin.inl"
#   include "mixins/enum/mixin.inl"

#   ifdef FLECS_MODULE
#   include "mixins/module/mixin.inl"
#   endif
#   ifdef FLECS_PIPELINE
#   include "mixins/pipeline/mixin.inl"
#   endif
#   ifdef FLECS_SYSTEM
#   include "mixins/system/mixin.inl"
#   endif
#   ifdef FLECS_TIMER
#   include "mixins/timer/mixin.inl"
#   endif
#   ifdef FLECS_SCRIPT
#   include "mixins/script/mixin.inl"
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

    world_t *world_;
};

/** Scoped world.
 * Utility class used by the world::scope method to create entities in a scope.
 */
struct scoped_world : world {
    scoped_world(
        flecs::world_t *w,
        flecs::entity_t s) : world(w)
    {
        prev_scope_ = ecs_set_scope(w, s);
    }

    ~scoped_world() {
        ecs_set_scope(world_, prev_scope_);
    }

    scoped_world(const scoped_world& obj) : world(nullptr) {
        prev_scope_ = obj.prev_scope_;
        world_ = obj.world_;
        flecs_poly_claim(world_);
    }

    flecs::entity_t prev_scope_;
};

/** @} */

} // namespace flecs
