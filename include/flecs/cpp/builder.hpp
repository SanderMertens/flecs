
namespace flecs {

template <typename Base>
class term_id {
public:
    term_id() : m_term_id(nullptr) { }

    term_id(flecs::world_t *world, ecs_term_id_t *tid, Base& base) 
        : m_world(world)
        , m_term_id(tid)
        , m_base(base) { }

    template<typename T>
    Base& entity() {
        m_term_id->entity = _::cpp_type<T>::id(m_world);
        return m_base;
    }

    Base& entity(const flecs::id& id) {
        m_term_id->entity = id.raw_id();
        return m_base;
    }

    Base& name(const char *name) {
        /* Const cast is safe, when the value is actually used to construct a
         * query, it will be duplicated. */
        m_term_id->name = const_cast<char*>(name);
        return m_base;
    }

    Base& set(uint8_t set_mask, const id relation = flecs::id(flecs::IsA), 
        int32_t min_depth = 0, int32_t max_depth = 0)
    {
        m_term_id->set = set_mask;
        m_term_id->relation = relation.raw_id();
        m_term_id->min_depth = min_depth;
        m_term_id->max_depth = max_depth;
        return m_base;
    }
    
private:
    flecs::world_t *m_world;
    ecs_term_id_t *m_term_id;
    Base& m_base;
};

// Class that describes a term
class term {
public:
    term(const world& world) 
        : m_world(world.c_ptr())
        , m_term({}) { }

    term(flecs::world_t *world) 
        : m_world(world)
        , m_term({}) { }

    template<typename T>
    term& id() {
        m_term.id = _::cpp_type<T>::id(m_world);
        return *this;
    }

    template<typename R, typename O>
    term& id() {
        m_term.id = ecs_pair(
            _::cpp_type<R>::id(m_world),
            _::cpp_type<O>::id(m_world));
        return *this;
    }    

    term& id(const flecs::id_t id) {
        m_term.id = id;
        return *this;
    }

    term& id(const flecs::id_t r, const flecs::id_t o) {
        m_term.id = ecs_pair(r, o);
        return *this;
    }    

    term_id<term> predicate() {
        return term_id<term>(m_world, &m_term.pred, *this);
    }

    term_id<term> subject() {
        return term_id<term>(m_world, &m_term.args[0], *this);
    }

    term_id<term> object() {
        return term_id<term>(m_world, &m_term.args[1], *this);
    }

    operator ecs_term_t() const {
        return m_term;
    }

private:
    flecs::world_t *m_world;
    ecs_term_t m_term;
};


// Filter builder interface
template<typename Base, typename ... Components>
class filter_builder_i {
public:
    filter_builder_i(flecs::world_t *world, ecs_filter_desc_t *desc) 
        : m_world(world)
        , m_desc(desc)
        , m_term(0) { }

    Base& expr(const char *expr) {
        m_desc->expr = expr;
        return *static_cast<Base*>(this);
    }

    Base& substitute_default(bool value = true) {
        m_desc->substitute_default = value;
        return *static_cast<Base*>(this);
    }

    Base& term() {
        m_term++;
        return *static_cast<Base*>(this);
    }

    template<typename T>
    Base& term() {
        m_desc->terms[m_term] = flecs::term(m_world).id<T>();
        m_term ++;
        return *static_cast<Base*>(this);
    }

    template<typename R, typename O>
    Base& term() {
        m_desc->terms[m_term ++] = flecs::term(m_world).id<R, O>();
        return *static_cast<Base*>(this);
    }    

    Base& term(const id_t id) {
        m_desc->terms[m_term ++] = flecs::term(m_world).id(id);
        return *static_cast<Base*>(this);
    }

    Base& term(const id_t r, const id_t o) {
        m_desc->terms[m_term ++] = flecs::term(m_world).id(r, o);
        return *static_cast<Base*>(this);
    }    

    Base& term(const flecs::term& term) {
        m_desc->terms[m_term ++] = term;
        return *static_cast<Base*>(this);
    }

    Base& role(const id_t role) {
        ecs_assert(m_term > 0, ECS_INVALID_PARAMETER, NULL);
        m_desc->terms[m_term - 1].role = role;
    }

    Base& inout(flecs::inout_kind_t inout) {
        ecs_assert(m_term > 0, ECS_INVALID_PARAMETER, NULL);
        m_desc->terms[m_term - 1].inout = static_cast<ecs_inout_kind_t>(inout);
        return *static_cast<Base*>(this);
    }

    Base& oper(flecs::oper_kind_t oper) {
        ecs_assert(m_term > 0, ECS_INVALID_PARAMETER, NULL);
        m_desc->terms[m_term - 1].oper = static_cast<ecs_oper_kind_t>(oper);
        return *static_cast<Base*>(this);
    }

    Base& predicate(entity_t entity, const char *name = nullptr, 
        uint8_t set_mask = 0, entity_t relation = 0, int32_t min_depth = 0, 
        int32_t max_depth = 0)
    {
        ecs_assert(m_term > 0, ECS_INVALID_PARAMETER, NULL);
        ecs_term_t *t = &m_desc->terms[m_term - 1];
        t->pred.entity = entity;
        t->pred.name = const_cast<char*>(name);
        t->pred.set = set_mask,
        t->pred.relation = relation;
        t->pred.min_depth = min_depth;
        t->pred.max_depth = max_depth;
        return *static_cast<Base*>(this);
    }

    Base& subject(entity_t entity, const char *name = nullptr, 
        uint8_t set_mask = 0, entity_t relation = 0, int32_t min_depth = 0, 
        int32_t max_depth = 0)
    {
        ecs_assert(m_term > 0, ECS_INVALID_PARAMETER, NULL);
        ecs_term_t *t = &m_desc->terms[m_term - 1];
        t->args[0].entity = entity;
        t->args[0].name = const_cast<char*>(name);
        t->args[0].set = set_mask,
        t->args[0].relation = relation;
        t->args[0].min_depth = min_depth;
        t->args[0].max_depth = max_depth;
        return *static_cast<Base*>(this);
    }

    Base& object(entity_t entity, const char *name = nullptr, 
        uint8_t set_mask = 0, entity_t relation = 0, int32_t min_depth = 0, 
        int32_t max_depth = 0)
    {
        ecs_assert(m_term > 0, ECS_INVALID_PARAMETER, NULL);
        ecs_term_t *t = &m_desc->terms[m_term - 1];
        t->args[1].entity = entity;
        t->args[1].name = const_cast<char*>(name);
        t->args[1].set = set_mask,
        t->args[1].relation = relation;
        t->args[1].min_depth = min_depth;
        t->args[1].max_depth = max_depth;
        return *static_cast<Base*>(this);
    }

    void populate_filter_from_pack() {
        flecs::array<flecs::id_t, sizeof...(Components)> ids ({
            (_::cpp_type<Components>::id(m_world))...
        });

        flecs::array<flecs::inout_kind_t, sizeof...(Components)> inout_kinds ({
            (type_to_inout<Components>())...
        });

        flecs::array<flecs::oper_kind_t, sizeof...(Components)> oper_kinds ({
            (type_to_oper<Components>())...
        });

        for (size_t i = 0; i < sizeof...(Components); i ++) {
            term(ids[i]).inout(inout_kinds[i]).oper(oper_kinds[i]);
        }
    }

protected:
    flecs::world_t *m_world;

private:
    template <typename T,
        typename std::enable_if< std::is_const<T>::value == true, void>::type* = nullptr>
    constexpr flecs::inout_kind_t type_to_inout() {
        return flecs::In;
    }

    template <typename T,
        typename std::enable_if< std::is_reference<T>::value == true, void>::type* = nullptr>
    constexpr flecs::inout_kind_t type_to_inout() {
        return flecs::Out;
    }

    template <typename T,
        typename std::enable_if<std::is_const<T>::value == false && std::is_reference<T>::value == false, void>::type* = nullptr>
    constexpr flecs::inout_kind_t type_to_inout() {
        return flecs::InOutDefault;
    }

    template <typename T,
        typename std::enable_if< std::is_pointer<T>::value == true, void>::type* = nullptr>
    constexpr flecs::oper_kind_t type_to_oper() {
        return flecs::Optional;
    }

    template <typename T,
        typename std::enable_if< std::is_pointer<T>::value == false, void>::type* = nullptr>
    constexpr flecs::oper_kind_t type_to_oper() {
        return flecs::And;
    }

    ecs_filter_desc_t *m_desc;
    int32_t m_term;
};

// Query builder interface
template<typename Base, typename ... Components>
class query_builder_i : public filter_builder_i<Base, Components ...> {
public:
    query_builder_i()
        : filter_builder_i<Base, Components ...>(nullptr, nullptr)
        , m_desc(nullptr) { }

    query_builder_i(flecs::world_t *world, ecs_query_desc_t *desc) 
        : filter_builder_i<Base, Components ...>(world, &desc->filter)
        , m_desc(desc) { }

    /** Sort the output of a query.
     * This enables sorting of entities across matched tables. As a result of this
     * operation, the order of entities in the matched tables may be changed. 
     * Resorting happens when a query iterator is obtained, and only if the table
     * data has changed.
     *
     * If multiple queries that match the same (sub)set of tables specify different 
     * sorting functions, resorting is likely to happen every time an iterator is
     * obtained, which can significantly slow down iterations.
     *
     * The sorting function will be applied to the specified component. Resorting
     * only happens if that component has changed, or when the entity order in the
     * table has changed. If no component is provided, resorting only happens when
     * the entity order changes.
     *
     * @tparam T The component used to sort.
     * @param compare The compare function used to sort the components.
     */      
    template <typename T>
    Base& order_by(int(*compare)(flecs::entity_t, const T*, flecs::entity_t, const T*)) {
        ecs_compare_action_t cmp = reinterpret_cast<ecs_compare_action_t>(compare);
        return this->order_by(
            flecs::entity(this->m_world, _::cpp_type<T>::id(this->m_world)), cmp);
    }

    /** Sort the output of a query.
     * Same as order_by<T>, but with component identifier.
     *
     * @param component The component used to sort.
     * @param compare The compare function used to sort the components.
     */    
    Base& order_by(flecs::entity component, int(*compare)(flecs::entity_t, const void*, flecs::entity_t, const void*)) {
        m_desc->order_by = reinterpret_cast<ecs_compare_action_t>(compare);
        m_desc->order_by_id = component;
        return *static_cast<Base*>(this);
    }

    /** Group and sort matched tables.
     * Similar yo ecs_query_order_by, but instead of sorting individual entities, this
     * operation only sorts matched tables. This can be useful of a query needs to
     * enforce a certain iteration order upon the tables it is iterating, for 
     * example by giving a certain component or tag a higher priority.
     *
     * The sorting function assigns a "rank" to each type, which is then used to
     * sort the tables. Tables with higher ranks will appear later in the iteration.
     * 
     * Resorting happens when a query iterator is obtained, and only if the set of
     * matched tables for a query has changed. If table sorting is enabled together
     * with entity sorting, table sorting takes precedence, and entities will be
     * sorted within each set of tables that are assigned the same rank.
     *
     * @tparam T The component used to determine the group rank.
     * @param rank The rank action.
     */
    template <typename T>
    Base& group_by(int(*rank)(flecs::world_t*, flecs::entity_t, flecs::type_t type)) {
        ecs_rank_type_action_t rnk = reinterpret_cast<ecs_rank_type_action_t>(rank);
        return this->group_by(
            flecs::entity(this->m_world, _::cpp_type<T>::id(this->m_world)), rnk);
    }

    /** Group and sort matched tables.
     * Same as group_by<T>, but with component identifier.
     *
     * @param component The component used to determine the group rank.
     * @param rank The rank action.
     */
    Base& group_by(flecs::entity component, int(*rank)(flecs::world_t*, flecs::entity_t, flecs::type_t type)) {
        m_desc->group_by = reinterpret_cast<ecs_rank_type_action_t>(rank);
        m_desc->group_by_id = component;
        return *static_cast<Base*>(this);
    } 

    /** Specify parent query (creates subquery) */
    Base& parent(const query_base& parent);

private:
    ecs_query_desc_t *m_desc;
};

// System builder interface
template<typename Base, typename ... Components>
class system_builder_i : public query_builder_i<Base, Components ...> {
public:
    system_builder_i()
        : query_builder_i<Base, Components ...>(nullptr, nullptr)
        , m_desc(nullptr)
        , m_add_count(0) { }

    system_builder_i(flecs::world_t *world, ecs_system_desc_t *desc) 
        : query_builder_i<Base, Components ...>(world, &desc->query)
        , m_desc(desc)
        , m_add_count(0) { }

    /** Specify string-based signature. */
    Base& signature(const char *signature) {
        m_desc->query.filter.expr = signature;
        return *static_cast<Base*>(this);
    }

    /** Specify when the system should be ran.
     * Use this function to set in which phase the system should run or whether
     * the system is reactive. Valid values for reactive systems are:
     *
     * flecs::OnAdd
     * flecs::OnRemove
     * flecs::OnSet
     * flecs::UnSet
     *
     * @param kind The kind that specifies when the system should be ran.
     */
    Base& kind(entity_t kind) {
        m_desc->entity.add[0] = kind;
        return *static_cast<Base*>(this);
    }

    /** Set system interval.
     * This operation will cause the system to be ran at the specified interval.
     *
     * The timer is synchronous, and is incremented each frame by delta_time.
     *
     * @param interval The interval value.
     */
    Base& interval(FLECS_FLOAT interval) {
        m_desc->interval = interval;
        return *static_cast<Base*>(this);
    }

    /** Set system rate.
     * This operation will cause the system to be ran at a multiple of the 
     * provided tick source. The tick source may be any entity, including
     * another system.
     *
     * @param tick_source The tick source.
     * @param rate The multiple at which to run the system.
     */
    Base& rate(const entity_t tick_source, int32_t rate) {
        m_desc->rate = rate;
        m_desc->tick_source = tick_source;
        return *static_cast<Base*>(this);
    }

    /** Set system rate.
     * This operation will cause the system to be ran at a multiple of the 
     * frame tick frequency. If a tick source was provided, this just updates
     * the rate of the system.
     *
     * @param rate The multiple at which to run the system.
     */
    Base& rate(int32_t rate) {
        m_desc->rate = rate;
        return *static_cast<Base*>(this);
    }

    /** System is an on demand system */
    Base& on_demand() {
        m_desc->entity.add[m_add_count ++] = flecs::OnDemand;
        return *static_cast<Base*>(this);
    }

    /** System is a hidden system */
    Base& hidden() {
        m_desc->entity.add[m_add_count ++] = flecs::Hidden;
        return *static_cast<Base*>(this);
    }

    ECS_DEPRECATED("use interval")
    Base& period(FLECS_FLOAT period) {
        return this->interval(period);
    }   

    void set_context(void *ctx) {
        m_desc->ctx = ctx;
    }     

private:
    ecs_system_desc_t *m_desc;
    int32_t m_add_count;
};

// Query builder
template<typename ... Components>
class query_builder : public query_builder_i<query_builder<Components...>, Components ...> {
    friend class query_base;
public:
    query_builder()
        : m_world(nullptr)
        , m_desc({}) { }

    query_builder(const world& world) 
        : query_builder_i<query_builder<Components...>, Components ...>(world.c_ptr(), &m_desc)
        , m_world(world.c_ptr())
        , m_desc({}) 
    { 
        this->populate_filter_from_pack();
    }

    query_builder(const query_builder& obj) 
        : query_builder_i<query_builder<Components...>, Components ...>(obj.m_world, &m_desc)
    {
        m_world = obj.m_world;
        m_desc = obj.m_desc;
    }

    query_builder(query_builder&& obj) 
        : query_builder_i<query_builder<Components...>, Components ...>(obj.m_world, &m_desc)
    {
        m_world = obj.m_world;
        m_desc = obj.m_desc;
    }

    operator ecs_query_t*() const {
        return ecs_query_init(m_world, &m_desc);
    }

    operator query<Components ...>() const;

    query<Components ...> build() const;

private:
    flecs::world_t *m_world;
    ecs_query_desc_t m_desc;
};

}
