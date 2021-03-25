
namespace flecs 
{

/** The world.
 * The world is the container of all ECS data and systems. If the world is
 * deleted, all data in the world will be deleted as well.
 */
class world final {
public:
    /** Create world.
     */
    world() 
        : m_world( ecs_init() )
        , m_owned( true ) { init_builtin_components(); }

    /** Create world with command line arguments.
     * Currently command line arguments are not interpreted, but they may be
     * used in the future to configure Flecs parameters.
     */
    world(int argc, char *argv[])
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
        if (m_owned && m_world) {
            ecs_fini(m_world); 
        }
    }

    /** Obtain pointer to C world object.
     */
    world_t* c_ptr() const {
        return m_world;
    }

    /** Enable tracing.
     *
     * @param level The tracing level.
     */
    static void enable_tracing(int level) {
        ecs_tracing_enable(level);
    }

    /** Progress world, run all systems.
     *
     * @param delta_time Custom delta_time. If 0 is provided, Flecs will automatically measure delta_tiem.
     */
    bool progress(FLECS_FLOAT delta_time = 0.0) const {
        return ecs_progress(m_world, delta_time);
    }

    /** Signal application should quit.
     * After calling this operation, the next call to progress() returns false.
     */
    void quit() {
        ecs_quit(m_world);
    }

    /** Test if quit() has been called.
     */
    bool should_quit() {
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
    FLECS_FLOAT frame_begin(float delta_time = 0) {
        return ecs_frame_begin(m_world, delta_time);
    }

    /** End frame. 
     * This operation must be called at the end of the frame, and always after
     * ecs_frame_begin.
     *
     * This function should only be ran from the main thread.
     */
    void frame_end() {
        ecs_frame_end(m_world);
    }

    /** Begin staging.
     * When an application does not use ecs_progress to control the main loop, it
     * can still use Flecs features such as the defer queue. When an application
     * needs to stage changes, it needs to call this function after ecs_frame_begin.
     * A call to ecs_staging_begin must be followed by a call to ecs_staging_end.
     * 
     * When staging is enabled, modifications to entities are stored to a stage.
     * This ensures that arrays are not modified while iterating. Modifications are
     * merged back to the "main stage" when ecs_staging_end is invoked.
     *
     * While the world is in staging mode, no structural changes (add/remove/...)
     * can be made to the world itself. Operations must be executed on a stage
     * instead (see ecs_get_stage).
     *
     * This function should only be ran from the main thread.
     *
     * @return Whether world is currently staged.
     */
    bool staging_begin() {
        return ecs_staging_begin(m_world);
    }

    /** End staging.
     * Leaves staging mode. After this operation the world may be directly mutated
     * again. By default this operation also merges data back into the world, unless
     * automerging was disabled explicitly.
     *
     * This function should only be ran from the main thread.
     */
    void staging_end() {
        ecs_staging_end(m_world);
    }

    /** Defer operations until end of frame. 
     * When this operation is invoked while iterating, operations inbetween the
     * defer_begin and defer_end operations are executed at the end of the frame.
     *
     * This operation is thread safe.
     */
    bool defer_begin() {
        return ecs_defer_begin(m_world);
    }

    /** End block of operations to defer. 
     * See defer_begin.
     *
     * This operation is thread safe.
     */
    bool defer_end() {
        return ecs_defer_end(m_world);
    }

    /** Configure world to have N stages.
     * This initializes N stages, which allows applications to defer operations to
     * multiple isolated defer queues. This is typically used for applications with
     * multiple threads, where each thread gets its own queue, and commands are
     * merged when threads are synchronized.
     *
     * Note that set_threads() already creates the appropriate number of stages. 
     * The set_stages() operation is useful for applications that want to manage 
     * their own stages and/or threads.
     * 
     * @param stages The number of stages.
     */
    void set_stages(int32_t stages) const {
        ecs_set_stages(m_world, stages);
    }

    /** Get number of configured stages.
     * Return number of stages set by set_stages.
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

    /** Enable/disable automerging for world or stage.
     * When automerging is enabled, staged data will automatically be merged 
     * with the world when staging ends. This happens at the end of progress(), 
     * at a sync point or when staging_end() is called.
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
    void set_automerge(bool automerge) {
        ecs_set_automerge(m_world, automerge);
    }

    /** Merge world or stage.
     * When automatic merging is disabled, an application can call this
     * operation on either an individual stage, or on the world which will merge
     * all stages. This operation may only be called when staging is not enabled
     * (either after progress() or after staging_end()).
     *
     * This operation may be called on an already merged stage or world.
     */
    void merge() {
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
    flecs::world get_stage(int32_t id) const {
        return flecs::world(ecs_get_stage(m_world, id));
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
     * This function allows the code to test whether the currently used world object
     * is readonly or whether it allows for writing.  
     *
     * @return True if the world or stage is readonly.
     */
    bool is_readonly() const {
        return ecs_stage_is_readonly(m_world);
    }

    /** Set number of threads.
     * This will distribute the load evenly across the configured number of 
     * threads for each system.
     *
     * @param threads Number of threads.
     */
    void set_threads(int32_t threads) const {
        ecs_set_threads(m_world, threads);
    }

    /** Get number of threads.
     *
     * @return Number of configured threads.
     */
    int32_t get_threads() const {
        return ecs_get_threads(m_world);
    }

    /** Get index of current thread.
     *
     * @return Unique index for current thread.
     */
    ECS_DEPRECATED("use get_stage_id")
    int32_t get_thread_index() const {
        return ecs_get_stage_id(m_world);
    }

    /** Set target FPS
     * This will ensure that the main loop (world::progress) does not run faster
     * than the specified frames per second.
     *
     * @param target_fps Target frames per second.
     */
    void set_target_fps(FLECS_FLOAT target_fps) const {
        ecs_set_target_fps(m_world, target_fps);
    }

    /** Get target FPS
     *
     * @return Configured frames per second.
     */
    FLECS_FLOAT get_target_fps() const {
        const ecs_world_info_t *stats = ecs_get_world_info(m_world);
        return stats->target_fps;
    }

    /** Get tick
     *
     * @return Monotonically increasing frame count.
     */
    int32_t get_tick() const {
        const ecs_world_info_t *stats = ecs_get_world_info(m_world);
        return stats->frame_count_total;
    }

    /** Set timescale
     *
     * @return Monotonically increasing frame count.
     */
    void set_time_scale(FLECS_FLOAT mul) const {
        ecs_set_time_scale(m_world, mul);
    }  

    /** Get timescale
     *
     * @return Monotonically increasing frame count.
     */
    FLECS_FLOAT get_time_scale() const {
        const ecs_world_info_t *stats = ecs_get_world_info(m_world);
        return stats->time_scale;
    }        

    /** Set world context.
     * Set a context value that can be accessed by anyone that has a reference
     * to the world.
     *
     * @param ctx The world context.
     */
    void set_context(void* ctx) const {
        ecs_set_context(m_world, ctx);
    }

    /** Get world context.
     *
     * @return The configured world context.
     */
    void* get_context() const {
        return ecs_get_context(m_world);
    }

    /** Preallocate memory for number of entities.
     * This function preallocates memory for the entity index.
     *
     * @param entity_count Number of entities to preallocate memory for.
     */
    void dim(int32_t entity_count) const {
        ecs_dim(m_world, entity_count);
    }

    /** Preallocate memory for type
     * This function preallocates memory for the component arrays of the
     * specified type.
     *
     * @param type Type to preallocate memory for.
     * @param entity_count Number of entities to preallocate memory for.
     */
    void dim_type(type_t t, int32_t entity_count) const {
        ecs_dim_type(m_world, t, entity_count);
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

    /** Disables inactive systems.
     *
     * This removes systems that are not matched with any entities from the main
     * loop. Systems are only added to the main loop after they first match with
     * entities, but are not removed automatically.
     *
     * This function allows an application to manually disable inactive systems
     * which removes them from the main loop. Doing so will cause Flecs to
     * rebuild the pipeline in the next iteration.
     *
     * @param level The tracing level.
     */
    void deactivate_systems() {
        ecs_deactivate_systems(m_world);
    }    

    /** Lookup entity by name.
     * 
     * @param name Entity name.
     */
    flecs::entity lookup(const char *name) const;

    /** Set singleton component.
     */
    template <typename T>
    void set(T value) const;

    /** Get mut singleton component.
     */
    template <typename T>
    T* get_mut() const;

    /** Mark singleton component as modified.
     */
    template <typename T>
    void modified() const;

    /** Patch singleton component.
     */
    template <typename T, typename Func>
    void patch(const Func& func) const;

    /** Get singleton component.
     */
    template <typename T>
    const T* get() const;

    /** Test if world has singleton component.
     */
    template <typename T>
    bool has() const;

    /** Remove singleton component.
     */
    template <typename T>
    void remove() const;

    /** Get id for type.
     */
    template <typename T>
    entity_t type_id() {
        return _::cpp_type<T>::id(m_world);
    }

    /** Get singleton entity for type.
     */
    template <typename T>
    flecs::entity singleton();

    /** Create alias for component.
     *
     * @tparam Component to create an alias for.
     * @param alias Alias for the component.
     */
    template <typename T>
    flecs::entity use(const char *alias = nullptr);

    /** Create alias for entity.
     *
     * @param name Name of the entity.
     * @param alias Alias for the entity.
     */
    flecs::entity use(const char *name, const char *alias = nullptr);    

    /** Create alias for entity.
     *
     * @param entity Entity for which to create the alias.
     * @param alias Alias for the entity.
     */
    void use(flecs::entity entity, const char *alias = nullptr);   

    /** Delete all entities matching a filter.
     *
     * @param filter The filter to use for matching.
     */
    void delete_entities(flecs::filter filter) const;

    /** Add component to all entities matching a filter.
     *
     * @tparam T The component to add.
     * @param filter The filter to use for matching.
     */
    template <typename T>
    void add(flecs::filter filter) const;

    /** Add type to all entities.
     *
     * @param type The type to add.
     */
    void add(flecs::type type) const;

    /** Add type to all entities matching a filter.
     *
     * @param type The type to add.
     * @param filter The filter to use for matching.
     */    
    void add(flecs::type type, flecs::filter filter) const;

    /** Add entity to all entities.
     *
     * @param entity The entity to add.
     */
    void add(flecs::entity entity) const;

    /** Add entity to all entities matching a filter.
     *
     * @param entity The entity to add.
     * @param filter The filter to use for matching.
     */    
    void add(flecs::entity entity, flecs::filter filter) const;

    /** Remove component from all entities matching a filter.
     *
     * @tparam T The component to remove.
     * @param filter The filter to use for matching.
     */    
    template <typename T>
    void remove(flecs::filter filter) const;

    /** Remove type from all entities.
     *
     * @param type The component to remove.
     */ 
    void remove(flecs::type type) const;

    /** Remove type from all entities matching a filter.
     *
     * @tparam T The component to remove.
     * @param filter The filter to use for matching.
     */     
    void remove(flecs::type type, flecs::filter filter) const;

    /** Remove entity from all entities.
     *
     * @param entity The entity to remove.
     */ 
    void remove(flecs::entity entity) const;

    /** Remove entity from all entities matching a filter.
     *
     * @param entity The entity to remove.
     * @param filter The filter to use for matching.
     */     
    void remove(flecs::entity entity, flecs::filter filter) const;

    /** Create iterable filter for entities in world.
     *
     * @param filter The filter to create.
     */ 
    world_filter filter(const flecs::filter& filter) const;

    flecs::filter_iterator begin() const;
    flecs::filter_iterator end() const;

    /** Count entities matching a component.
     *
     * @tparam T The component to use for matching.
     */
    template <typename T>
    int count() const {
        return ecs_count_type(
            m_world, _::cpp_type<T>::type(m_world));
    }

    /** Count entities matching a filter.
     *
     * @param filter The filter to use for matching.
     */
    int count(flecs::filter filter) const;

    /** Enable locking.
     * 
     * @param enabled True if locking should be enabled, false if not.
     */
    bool enable_locking(bool enabled) {
        return ecs_enable_locking(m_world, enabled);
    }

    /** Lock world.
     */
    void lock() {
        ecs_lock(m_world);
    }

    /** Unlock world.
     */
    void unlock() {
        ecs_unlock(m_world);
    }

    /** Create a prefab.
     */
    template <typename... Args>
    flecs::entity entity(Args &&... args) const;

    /** Create an entity.
     */
    template <typename... Args>
    flecs::entity prefab(Args &&... args) const;

    /** Create a type.
     */
    template <typename... Args>
    flecs::type type(Args &&... args) const;

    /** Create a module.
     */
    template <typename Module, typename... Args>
    flecs::entity module(Args &&... args) const;

    /** Import a module.
     */
    template <typename Module>
    flecs::entity import(); // Cannot be const because modules accept a non-const world

    /** Create an system.
     */
    template <typename... Comps, typename... Args>
    flecs::system<Comps...> system(Args &&... args) const;

    /** Create a query.
     */
    template <typename... Comps, typename... Args>
    flecs::query<Comps...> query(Args &&... args) const;

    /** Register a component.
     */
    template <typename T, typename... Args>
    flecs::entity component(Args &&... args) const;

    /** Register a POD component.
     */
    template <typename T, typename... Args>
    flecs::entity pod_component(Args &&... args) const;

    /** Register a relocatable component.
     */
    template <typename T, typename... Args>
    flecs::entity relocatable_component(Args &&... args) const;

    /** Create a snapshot.
     */
    template <typename... Args>
    flecs::snapshot snapshot(Args &&... args) const;
    
private:
    void init_builtin_components();

    world_t *m_world;
    bool m_owned;
};

} // namespace flecs
