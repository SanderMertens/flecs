
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

    system_runner_fluent& filter(filter filter) {
        m_filter = filter;
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
    flecs::filter m_filter;
    int32_t m_offset;
    int32_t m_limit;
    int32_t m_stage_current;
    int32_t m_stage_count;
};


////////////////////////////////////////////////////////////////////////////////
//// Register a system with Flecs
////////////////////////////////////////////////////////////////////////////////

template<typename ... Components>
class system final : public entity, public system_builder_i<system<Components ...>, Components ...> {
    using Class = system<Components ...>;
public:
    explicit system(const flecs::world& world, const char *name = nullptr, const char *signature = nullptr) 
        : entity(world, name)
        , system_builder_i<Class, Components ...>(world.c_ptr(), &m_desc)
        , m_desc({})
        , m_finalized(false)
        { 
            ecs_assert(m_id != 0, ECS_INTERNAL_ERROR, NULL);
            m_desc.entity.entity = m_id;
            m_desc.entity.add[0] = flecs::OnUpdate;
            system_builder_i<Class, Components ...>::populate_filter_from_pack();
            system_builder_i<Class, Components ...>::expr(signature);
        }

    explicit system(const flecs::world& world, flecs::entity id) 
        : entity(world, id.id())
        , m_desc({})
        , m_finalized(true) { }

    system(const system& obj) 
        : entity(obj.entity::m_world, obj.m_id)
        , system_builder_i<system<Components...>, Components ...>(obj.entity::m_world, &m_desc)
    {
        m_desc = obj.m_desc;
        m_finalized = obj.m_finalized;
    }

    system(system&& obj) 
        : entity(obj.entity::m_world, obj.m_id)
        , system_builder_i<system<Components...>, Components ...>(obj.entity::m_world, &m_desc)
    {
        m_desc = obj.m_desc;
        m_finalized = obj.m_finalized;
    }

    template <typename T>
    system& order_by(int(*compare)(flecs::entity_t, const T*, flecs::entity_t, const T*)) {
        return this->order_by(flecs::_::cpp_type<T>::id(m_world),
            reinterpret_cast<ecs_compare_action_t>(compare));
    }

    system& order_by(flecs::entity component, int(*compare)(flecs::entity_t, const void*, flecs::entity_t, const void*)) {
        if (m_finalized) {
            ecs_query_t *q = query().c_ptr();
            ecs_assert(q != NULL, ECS_INVALID_PARAMETER, NULL);
            ecs_query_order_by(m_world, q, component.id(), compare);
            return *this;
        } else {
            return system_builder_i<Class, Components ...>::order_by(component, compare);
        }
    }    

    template <typename T>
    system& group_by(int(*rank)(flecs::world_t*, flecs::entity_t, flecs::type_t type)) {
        return this->group_by(flecs::_::cpp_type<T>::id(m_world), rank);
    }

    system& group_by(flecs::entity component, int(*rank)(flecs::world_t*, flecs::entity_t, flecs::type_t type)) {
        if (m_finalized) {
            ecs_query_t *q = query().c_ptr();
            ecs_assert(q != NULL, ECS_INVALID_PARAMETER, NULL);
            ecs_query_group_by(m_world, q, component.id(), rank);
            return *this;
        } else {
            return system_builder_i<Class, Components ...>::group_by(component, rank);
        }
    }

    /** Set system interval.
     * This operation will cause the system to be ran at the specified interval.
     *
     * The timer is synchronous, and is incremented each frame by delta_time.
     *
     * @param interval The interval value.
     */
    system& interval(FLECS_FLOAT interval) {
        if (m_finalized) {
            ecs_set_interval(m_world, m_id, interval);
            return *this;
        } else {
            return system_builder_i<Class, Components ...>::interval(interval);
        }
    }

    /** Set system rate.
     * This operation will cause the system to be ran at a multiple of the 
     * provided tick source. The tick source may be any entity, including
     * another system.
     *
     * @param tick_source The tick source.
     * @param rate The multiple at which to run the system.
     */
    system& rate(const flecs::entity& tick_source, int32_t rate) {
        if (m_finalized) {
            ecs_set_rate(m_world, m_id, rate, tick_source.id());
            return *this;
        } else {
            return system_builder_i<Class, Components ...>::rate(tick_source, rate);
        }
    }

    /** Set system rate.
     * This operation will cause the system to be ran at a multiple of the 
     * frame tick frequency. If a tick source was provided, this just updates
     * the rate of the system.
     *
     * @param rate The multiple at which to run the system.
     */
    system& rate(int32_t rate) {
        if (m_finalized) {
            ecs_set_rate(m_world, m_id, rate, m_desc.tick_source);
            return *this;
        } else {
            return system_builder_i<Class, Components ...>::rate(rate);
        }
    }    

    /** Get interval.
     * Get interval at which the system is running.
     *
     * @return The timer entity.
     */
    FLECS_FLOAT interval() {
        ecs_assert(m_finalized, ECS_INVALID_PARAMETER, NULL);
        return ecs_get_interval(m_world, m_id);
    }

    void enable() {
        ecs_enable(m_world, m_id, true);
    }

    void disable() {
        ecs_enable(m_world, m_id, false);
    }

    ECS_DEPRECATED("use interval")
    void period(FLECS_FLOAT period) {
        this->interval(period);
    }

    ECS_DEPRECATED("use interval")
    void set_period(FLECS_FLOAT period) const {
        ecs_set_interval(m_world, m_id, period);
    }

    system& set_context(void *ctx) {
        if (m_finalized) {
            EcsContext ctx_value = { ctx };
            ecs_set_ptr(m_world, m_id, EcsContext, &ctx_value);
            return *this;
        } else {
            return system_builder_i<Class, Components ...>::set_context(ctx);
        }
    }

    void* get_context() const {
        ecs_assert(m_finalized, ECS_INVALID_PARAMETER, NULL);

        const EcsContext *ctx = static_cast<const EcsContext*>(
            ecs_get_w_id(m_world, m_id, ecs_id(EcsContext)));
        if (ctx) {
            return const_cast<void*>(ctx->ctx);
        } else {
            return NULL;
        }
    }

    query_base query() const {
        ecs_assert(m_finalized, ECS_INVALID_PARAMETER, NULL);
        return query_base(m_world, ecs_get_query(m_world, m_id));
    }

    system_runner_fluent run(FLECS_FLOAT delta_time = 0.0f, void *param = nullptr) const {
        ecs_assert(m_finalized, ECS_INVALID_PARAMETER, NULL);
        return system_runner_fluent(m_world, m_id, 0, 0, delta_time, param);
    }

    system_runner_fluent run_worker(
        int32_t stage_current, 
        int32_t stage_count, 
        FLECS_FLOAT delta_time = 0.0f, 
        void *param = nullptr) const 
    {
        ecs_assert(m_finalized, ECS_INVALID_PARAMETER, NULL);
        return system_runner_fluent(m_world, m_id, stage_current, stage_count, delta_time, param);
    }    

    // put using outside of action so we can still use it without it being
    // flagged as deprecated.
    template <typename Func>
    using action_invoker_t = typename _::iter_invoker<
        typename std::decay<Func>::type, Components...>;

    template <typename Func>
    ECS_DEPRECATED("use each or iter")
    system& action(Func&& func) {
        create_system<action_invoker_t<Func>>(std::forward<Func>(func), false);
        return *this;
    }

    /* Iter (or each) is mandatory and always the last thing that 
     * is added in the fluent method chain. Create system signature from both 
     * template parameters and anything provided by the signature method. */
    template <typename Func>
    system& iter(Func&& func) {
        ecs_assert(!m_finalized, ECS_INVALID_PARAMETER, NULL);
        using Invoker = typename _::iter_invoker<
            typename std::decay<Func>::type, Components...>;
        create_system<Invoker>(std::forward<Func>(func), false);
        return *this;
    }

    /* Each is similar to action, but accepts a function that operates on a
     * single entity */
    template <typename Func>
    system& each(Func&& func) {
        ecs_assert(!m_finalized, ECS_INVALID_PARAMETER, NULL);
        using Invoker = typename _::each_invoker<
            typename std::decay<Func>::type, Components...>;
        create_system<Invoker>(std::forward<Func>(func), true);
        return *this;
    }

    ~system() = default;
private:
    template <typename Invoker, typename Func>
    entity_t create_system(Func&& func, bool is_each) {
        ecs_assert(m_id != 0, ECS_INTERNAL_ERROR, NULL);

        auto ctx = FLECS_NEW(Invoker)(std::forward<Func>(func));

        entity_t e, kind = m_desc.entity.add[0];
        bool is_trigger = kind == flecs::OnAdd || kind == flecs::OnRemove;

        if (is_trigger) {
            ecs_trigger_desc_t desc = {};
            ecs_term_t *term = &m_desc.query.filter.terms[0];

            if (ecs_term_is_set(term)) {
                desc.term = *term;
            } else {
                desc.expr = m_desc.query.filter.expr;
            }

            desc.entity.entity = m_desc.entity.entity;
            desc.events[0] = kind;
            desc.callback = Invoker::run;
            desc.ctx = m_desc.ctx;
            desc.binding_ctx = ctx;
            desc.binding_ctx_free = reinterpret_cast<
                ecs_ctx_free_t>(_::free_obj<Invoker>);

            e = ecs_trigger_init(m_world, &desc);
        } else {
            m_desc.callback = Invoker::run;
            m_desc.query.filter.substitute_default = is_each;
            m_desc.binding_ctx = ctx;
            m_desc.binding_ctx_free = reinterpret_cast<
                ecs_ctx_free_t>(_::free_obj<Invoker>);

            e = ecs_system_init(m_world, &m_desc);
        }

        ecs_assert(e == m_id, ECS_INTERNAL_ERROR, NULL);

        m_finalized = true;

        return e;
    }

    ecs_system_desc_t m_desc;
    bool m_finalized;
};

} // namespace flecs
