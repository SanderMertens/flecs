
namespace flecs 
{

////////////////////////////////////////////////////////////////////////////////
//// Fluent interface to run a system manually
////////////////////////////////////////////////////////////////////////////////

class system_runner_fluent {
public:
    system_runner_fluent(
        world_t *world, 
        entity_t id, 
        int32_t stage_current, 
        int32_t stage_count, 
        FLECS_FLOAT delta_time, 
        void *param)
        : m_stage(world)
        , m_id(id)
        , m_delta_time(delta_time)
        , m_param(param)
        , m_filter()
        , m_offset(0)
        , m_limit(0)
        , m_stage_current(stage_current)
        , m_stage_count(stage_count) { }

    template <typename F>
    system_runner_fluent& filter(const F& f) {
        m_filter = f;
        return *this;
    }

    system_runner_fluent& offset(int32_t offset) {
        m_offset = offset;
        return *this;
    }

    system_runner_fluent& limit(int32_t limit) {
        m_limit = limit;
        return *this;
    }

    system_runner_fluent& stage(flecs::world& stage) {
        m_stage = stage.c_ptr();
        return *this;
    }

    ~system_runner_fluent() {
        if (m_stage_count) {
            ecs_run_worker(
                m_stage, m_id, m_stage_current, m_stage_count, m_delta_time,
                m_param);            
        } else {
            ecs_run_w_filter(
                m_stage, m_id, m_delta_time, m_offset, m_limit, 
                m_filter.c_ptr(), m_param);
        }
    }

private:
    world_t *m_stage;
    entity_t m_id;
    FLECS_FLOAT m_delta_time;
    void *m_param;
    flecs::filter<> m_filter;
    int32_t m_offset;
    int32_t m_limit;
    int32_t m_stage_current;
    int32_t m_stage_count;
};


////////////////////////////////////////////////////////////////////////////////
//// Register a system with Flecs
////////////////////////////////////////////////////////////////////////////////

template<typename ... Components>
class system : public entity
{
public:
    explicit system() 
        : entity() { }

    explicit system(flecs::world_t *world, flecs::entity_t id)
        : entity(world, id) { }

    /** Set system interval.
     * This operation will cause the system to be ran at the specified interval.
     *
     * The timer is synchronous, and is incremented each frame by delta_time.
     *
     * @param interval The interval value.
     */
    void interval(FLECS_FLOAT interval) {
        ecs_set_interval(m_world, m_id, interval);
    }

    /** Set system rate.
     * This operation will cause the system to be ran at a multiple of the 
     * provided tick source. The tick source may be any entity, including
     * another system.
     *
     * @param tick_source The tick source.
     * @param rate The multiple at which to run the system.
     */
    void rate(const flecs::entity& tick_source, int32_t rate) {
        ecs_set_rate(m_world, m_id, rate, tick_source.id());
    }

    /** Set system rate.
     * This operation will cause the system to be ran at a multiple of the 
     * frame tick frequency. If a tick source was provided, this just updates
     * the rate of the system.
     *
     * @param rate The multiple at which to run the system.
     */
    void rate(int32_t rate) {
        ecs_set_rate(m_world, m_id, rate, 0);
    }    

    /** Get interval.
     * Get interval at which the system is running.
     *
     * @return The timer entity.
     */
    FLECS_FLOAT interval() {
        return ecs_get_interval(m_world, m_id);
    }

    void enable() {
        ecs_enable(m_world, m_id, true);
    }

    void disable() {
        ecs_enable(m_world, m_id, false);
    }

    void ctx(void *ctx) {
        if (ecs_has(m_world, m_id, EcsSystem)) {
            ecs_system_desc_t desc = {};
            desc.entity.entity = m_id;
            desc.ctx = ctx;
            ecs_system_init(m_world, &desc);
        } else {
            ecs_trigger_desc_t desc = {};
            desc.entity.entity = m_id;
            desc.ctx = ctx;
            ecs_trigger_init(m_world, &desc);
        }
    }

    void* ctx() const {
        if (ecs_has(m_world, m_id, EcsSystem)) {
            return ecs_get_system_ctx(m_world, m_id);
        } else {
            return ecs_get_trigger_ctx(m_world, m_id);
        }
    }    

    ECS_DEPRECATED("use interval")
    void period(FLECS_FLOAT period) {
        this->interval(period);
    }

    ECS_DEPRECATED("use interval")
    void set_period(FLECS_FLOAT period) const {
        this->interval(period);
    }

    ECS_DEPRECATED("use ctx(void*)")
    void set_context(void *ptr) {
        ctx(ptr);
    }

    ECS_DEPRECATED("use void* ctx()")
    void* get_context() const {
        return ctx();
    }

    query_base query() const {
        return query_base(m_world, ecs_get_system_query(m_world, m_id));
    }

    system_runner_fluent run(FLECS_FLOAT delta_time = 0.0f, void *param = nullptr) const {
        return system_runner_fluent(m_world, m_id, 0, 0, delta_time, param);
    }

    system_runner_fluent run_worker(
        int32_t stage_current, 
        int32_t stage_count, 
        FLECS_FLOAT delta_time = 0.0f, 
        void *param = nullptr) const 
    {
        return system_runner_fluent(
            m_world, m_id, stage_current, stage_count, delta_time, param);
    }
};

} // namespace flecs
