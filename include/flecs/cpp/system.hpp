
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
class system final : public entity {
public:
    explicit system(const flecs::world& world, const char *name = nullptr, const char *signature = nullptr) 
        : entity(world, name)
        , m_kind(static_cast<ecs_entity_t>(OnUpdate)) 
        , m_signature(signature)
        , m_interval(0.0)
        , m_on_demand(false)
        , m_hidden(false)
        , m_finalized(false) { 
            ecs_assert(m_id != 0, ECS_INTERNAL_ERROR, NULL);
        }

    explicit system(const flecs::world& world, flecs::entity id) 
        : entity(world, id.id())
        , m_finalized(true) { }

    system& signature(const char *signature) {
        ecs_assert(!m_finalized, ECS_INVALID_PARAMETER, NULL);
        ecs_assert(!m_signature, ECS_INVALID_PARAMETER, NULL);
        m_signature = signature;
        return *this;
    }

    system& kind(entity_t kind) {
        ecs_assert(!m_finalized, ECS_INVALID_PARAMETER, NULL);
        m_kind = static_cast<ecs_entity_t>(kind);
        return *this;
    }

    system& interval(FLECS_FLOAT interval) {
        if (!m_finalized) {
            m_interval = interval;
        } else {
            ecs_set_interval(m_world, m_id, interval);
        }
        return *this;
    }

    FLECS_FLOAT interval() {
        return ecs_get_interval(m_world, m_id);
    }

    ECS_DEPRECATED("use interval")
    system& period(FLECS_FLOAT period) {
        return this->interval(period);
    }

    system& on_demand() {
        ecs_assert(!m_finalized, ECS_INVALID_PARAMETER, NULL);
        m_on_demand = true;
        return *this;
    }

    system& hidden() {
        ecs_assert(!m_finalized, ECS_INVALID_PARAMETER, NULL);
        m_hidden = true;
        return *this;
    }

    /** Same as query::order_by */
    template <typename T>
    system& order_by(int(*compare)(flecs::entity_t, const T*, flecs::entity_t, const T*)) {
        ecs_compare_action_t cmp = reinterpret_cast<ecs_compare_action_t>(compare);
        return this->order_by(
            flecs::entity(m_world, _::cpp_type<T>::id(m_world)), cmp);
    }

    /** Same as query::order_by */
    system& order_by(flecs::entity component, int(*compare)(flecs::entity_t, const void*, flecs::entity_t, const void*)) {
        if (!m_finalized) {
            m_order_by = reinterpret_cast<ecs_compare_action_t>(compare);
            m_order_by_component = component;
        } else {
            const EcsQuery *q = static_cast<const EcsQuery*>(
                ecs_get_w_id(m_world, m_id, ecs_id(EcsQuery)));

            ecs_assert(q != NULL, ECS_INVALID_OPERATION, NULL);
            ecs_query_order_by(m_world, q->query, 
                component.id(), reinterpret_cast<ecs_compare_action_t>(compare));
        }
        return *this;
    }

    /** Same as query::group_by */
    template <typename T>
    system& group_by(int(*rank)(flecs::world_t*, flecs::entity_t, flecs::type_t type)) {
        ecs_rank_type_action_t rnk = reinterpret_cast<ecs_rank_type_action_t>(rank);
        return this->group_by(
            flecs::entity(m_world, _::cpp_type<T>::id(m_world)), rnk);
    }

    /** Same as query::group_by */
    system& group_by(flecs::entity component, int(*rank)(flecs::world_t*, flecs::entity_t, flecs::type_t type)) {
        if (!m_finalized) {
            m_group_by = reinterpret_cast<ecs_rank_type_action_t>(rank);
            m_group_by_component = component;
        } else {
            const EcsQuery *q = static_cast<const EcsQuery*>(
                ecs_get_w_id(m_world, m_id, ecs_id(EcsQuery)));
            ecs_assert(q != NULL, ECS_INVALID_OPERATION, NULL);
            ecs_query_group_by(m_world, q->query, component.id(),
                reinterpret_cast<ecs_rank_type_action_t>(rank));
        }
        return *this;
    }    

    void enable() {
        ecs_assert(m_finalized, ECS_INVALID_PARAMETER, NULL);
        ecs_enable(m_world, m_id, true);
    }

    void disable() {
        ecs_assert(m_finalized, ECS_INVALID_PARAMETER, NULL);
        ecs_enable(m_world, m_id, false);
    }

    void set_period(FLECS_FLOAT period) const {
        ecs_assert(m_finalized, ECS_INVALID_PARAMETER, NULL);
        ecs_set_interval(m_world, m_id, period);
    }

    void set_context(void *ctx) {
        if (!m_finalized) {
            m_ctx = ctx;
        } else {
            EcsContext ctx_value = { ctx };
            ecs_set_ptr(m_world, m_id, EcsContext, &ctx_value);
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
        const EcsQuery *q = static_cast<const EcsQuery*>(
            ecs_get_w_id(m_world, m_id, ecs_id(EcsQuery)));
        return query_base(m_world, q->query);
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
        return system_runner_fluent(m_world, m_id, stage_current, stage_count, delta_time, param);
    }    

    // put using outside of action so we can still use it without it being
    // flagged as deprecated.
    template <typename Func>
    using action_invoker_t = typename _::iter_invoker<typename std::decay<Func>::type, Components...>;

    template <typename Func>
    ECS_DEPRECATED("use each or iter")
    system& action(Func&& func) {
        ecs_assert(!m_finalized, ECS_INVALID_PARAMETER, NULL);

        auto ctx = FLECS_NEW(action_invoker_t<Func>)(std::forward<Func>(func));
        this->set<_::SystemCppContext>({ctx});

        create_system(action_invoker_t<Func>::run, false);

        return *this;
    }

     /* Iter (or each) is mandatory and always the last thing that 
      * is added in the fluent method chain. Create system signature from both 
      * template parameters and anything provided by the signature method. */
    template <typename Func>
    system& iter(Func&& func) {
        ecs_assert(!m_finalized, ECS_INVALID_PARAMETER, NULL);
        using invoker_t = typename _::iter_invoker<typename std::decay<Func>::type, Components...>;
        auto ctx = FLECS_NEW(invoker_t)(std::forward<Func>(func));
        this->set<_::SystemCppContext>({ctx});

        create_system(invoker_t::run, false);

        return *this;
    }    

    /* Each is similar to action, but accepts a function that operates on a
     * single entity */
    template <typename Func>
    system& each(Func&& func) {
        using invoker_t = typename _::each_invoker<typename std::decay<Func>::type, Components...>;
        auto ctx = FLECS_NEW(invoker_t)(std::forward<Func>(func));
        this->set<_::SystemCppContext>({ctx});

        create_system(invoker_t::run, true);

        return *this;
    }

    ~system() = default;
private:
    template <typename Invoker>
    entity_t create_system(Invoker invoker, bool is_each) {
        ecs_assert(m_id != 0, ECS_INTERNAL_ERROR, NULL);

        entity_t e;
        bool is_trigger = m_kind == flecs::OnAdd || m_kind == flecs::OnRemove;

        if (is_trigger) {
            // Don't add ANY source to each function if this is a trigger
            is_each = false;
        }

        flecs::string signature = build_signature(is_each);

        if (!signature.length()) {
            signature = flecs::string_view("0");
        }

        if (is_trigger) {
            e = ecs_new_trigger(
                m_world, 
                m_id,
                nullptr, 
                m_kind, 
                signature.c_str(), 
                invoker);
        } else {
            e = ecs_new_system(
                m_world, 
                m_id,
                nullptr, 
                m_kind, 
                signature.c_str(), 
                invoker);
        }

        ecs_assert(e == m_id, ECS_INTERNAL_ERROR, NULL);

        if (m_interval != 0) {
            ecs_set_interval(m_world, e, m_interval);
        }

        m_finalized = true;

        if (m_ctx) {
            this->set_context(m_ctx);
        }

        if (m_order_by) {
            this->order_by(m_order_by_component, m_order_by);
        }

        if (m_group_by) {
            this->group_by(m_group_by_component, m_group_by);
        }

        return e;
    }

    flecs::string build_signature(bool is_each) {
        bool is_set = false;

        flecs::stringstream str;
        if (_::pack_args_to_string<Components ...>(m_world, str, is_each)) {
            is_set = true;
        }

        if (m_signature) {
            if (is_set) {
                str << ",";
            }
            str << m_signature;
            is_set = true;
        }

        if (m_hidden) {
            if (is_set) {
                str << ",";
            }            
            str << "SYSTEM:Hidden";
            is_set = true;
        }    

        if (m_on_demand) {
            if (is_set) {
                str << ",";
            }            
            str << "SYSTEM:EcsOnDemand";
            is_set = true;
        } 

        return str.str();       
    }

    ecs_entity_t m_kind;
    const char *m_signature = nullptr;
    void *m_ctx = nullptr;

    ecs_compare_action_t m_order_by = nullptr;
    flecs::entity m_order_by_component;

    ecs_rank_type_action_t m_group_by = nullptr;
    flecs::entity m_group_by_component;

    FLECS_FLOAT m_interval;
    bool m_on_demand;
    bool m_hidden;
    bool m_finalized; // After set to true, system is created & sig is fixed
};

} // namespace flecs
