
namespace flecs {

template<typename Base>
class term_id_builder_i {
public:
    term_id_builder_i() : m_term_id(nullptr) { }

    virtual ~term_id_builder_i() { }

    template<typename T>
    Base& entity() {
        ecs_assert(m_term_id != NULL, ECS_INVALID_PARAMETER, NULL);
        m_term_id->entity = _::cpp_type<T>::id(world());
        return *this;
    }

    Base& entity(flecs::id_t id) {
        ecs_assert(m_term_id != NULL, ECS_INVALID_PARAMETER, NULL);
        m_term_id->entity = id;
        return *this;
    }

    Base& name(const char *name) {
        ecs_assert(m_term_id != NULL, ECS_INVALID_PARAMETER, NULL);
        // Const cast is safe, when the value is actually used to construct a
        // query, it will be duplicated.
        m_term_id->name = const_cast<char*>(name);
        return *this;
    }

    Base& var(flecs::var_kind_t var = flecs::VarIsVariable) {
        m_term_id->var = static_cast<ecs_var_kind_t>(var);
        return *this;
    }

    Base& var(const char *name) {
        ecs_assert(m_term_id != NULL, ECS_INVALID_PARAMETER, NULL);
        // Const cast is safe, when the value is actually used to construct a
        // query, it will be duplicated.
        m_term_id->name = const_cast<char*>(name);
        return var(); // Default to VarIsVariable
    }

    Base& set(uint8_t mask, const flecs::id_t relation = flecs::IsA)
    {
        ecs_assert(m_term_id != NULL, ECS_INVALID_PARAMETER, NULL);
        m_term_id->set.mask = mask;
        m_term_id->set.relation = relation;
        return *this;
    }

    Base& super(const flecs::id_t relation = flecs::IsA, uint8_t mask = 0)
    {
        ecs_assert(!(mask & flecs::SubSet), ECS_INVALID_PARAMETER, NULL);
        return set(flecs::SuperSet | mask, relation);
    }

    Base& sub(const flecs::id_t relation = flecs::IsA, uint8_t mask = 0)
    {
        ecs_assert(!(mask & flecs::SuperSet), ECS_INVALID_PARAMETER, NULL);
        return set(flecs::SubSet | mask, relation);
    }

    Base& min_depth(int32_t min_depth) {
        m_term_id->set.min_depth = min_depth;
        return *this;
    }

    Base& max_depth(int32_t max_depth) {
        m_term_id->set.max_depth = max_depth;
        return *this;
    }    

    ecs_term_id_t *m_term_id;
    
protected:
    virtual flecs::world_t* world() = 0;

private:
    operator Base&() {
        return *static_cast<Base*>(this);
    }
};

template<typename Base>
class term_builder_i : public term_id_builder_i<Base> {
public:
    term_builder_i() : m_term(nullptr) { }

    term_builder_i(ecs_term_t *term_ptr) { 
        set_term(term_ptr);
    }

    template<typename T>
    Base& id() {
        ecs_assert(m_term != nullptr, ECS_INVALID_PARAMETER, NULL);
        m_term->pred.entity = _::cpp_type<T>::id(world());
        return *this;
    }

    template<typename R, typename O>
    Base& id() {
        ecs_assert(m_term != nullptr, ECS_INVALID_PARAMETER, NULL);
        m_term->pred.entity = _::cpp_type<R>::id(world());
        m_term->args[1].entity = _::cpp_type<O>::id(world());
        return *this;
    }

    template<typename R>
    Base& id(id_t o) {
        ecs_assert(m_term != nullptr, ECS_INVALID_PARAMETER, NULL);
        m_term->pred.entity = _::cpp_type<R>::id(world());
        m_term->args[1].entity = o;
        return *this;
    }    

    Base& id(id_t id) {
        ecs_assert(m_term != nullptr, ECS_INVALID_PARAMETER, NULL);
        m_term->pred.entity = id;
        return *this;
    }

    Base& id(const flecs::type& type);

    Base& id(id_t r, id_t o) {
        ecs_assert(m_term != nullptr, ECS_INVALID_PARAMETER, NULL);
        m_term->pred.entity = r;
        m_term->args[1].entity = o;
        return *this;
    }

    Base& expr(const char *expr) {
        ecs_assert(m_term != nullptr, ECS_INVALID_PARAMETER, NULL);
        const char *ptr;
        if ((ptr = ecs_parse_term(world(), nullptr, expr, expr, m_term)) == nullptr) {
            ecs_abort(ECS_INVALID_PARAMETER, NULL);
        }

        // Should not have more than one term
        ecs_assert(ptr[0] == 0, ECS_INVALID_PARAMETER, NULL);
        return *this;
    }

    Base& predicate() {
        ecs_assert(m_term != nullptr, ECS_INVALID_PARAMETER, NULL);
        this->m_term_id = &m_term->pred;
        return *this;
    }

    Base& subject() {
        ecs_assert(m_term != nullptr, ECS_INVALID_PARAMETER, NULL);
        this->m_term_id = &m_term->args[0];
        return *this;
    }

    Base& object() {
        ecs_assert(m_term != nullptr, ECS_INVALID_PARAMETER, NULL);
        this->m_term_id = &m_term->args[1];
        return *this;
    }

    Base& subject(entity_t entity) {
        this->subject();
        this->m_term_id->entity = entity;
        return *this;
    }

    Base& object(entity_t entity) {
        this->object();
        this->m_term_id->entity = entity;
        return *this;
    }
    
    template<typename T>
    Base& subject() {
        this->subject();
        this->m_term_id->entity = _::cpp_type<T>::id(world());
        return *this;
    }

    template<typename T>
    Base& object() {
        this->object();
        this->m_term_id->entity = _::cpp_type<T>::id(world());
        return *this;
    }        

    Base& role(id_t role) {
        ecs_assert(m_term != nullptr, ECS_INVALID_PARAMETER, NULL);
        m_term->role = role;
        return *this;
    }

    Base& inout(flecs::inout_kind_t inout) {
        ecs_assert(m_term != nullptr, ECS_INVALID_PARAMETER, NULL);
        m_term->inout = static_cast<ecs_inout_kind_t>(inout);
        return *this;
    }

    Base& oper(flecs::oper_kind_t oper) {
        ecs_assert(m_term != nullptr, ECS_INVALID_PARAMETER, NULL);
        m_term->oper = static_cast<ecs_oper_kind_t>(oper);
        return *this;
    }

    Base& singleton() {
        ecs_assert(m_term != nullptr, ECS_INVALID_PARAMETER, NULL);
        ecs_assert(m_term->id || m_term->pred.entity, ECS_INVALID_PARAMETER, NULL);
        
        flecs::id_t pred = m_term->id;
        if (!pred) {
            pred = m_term->pred.entity;
        }

        ecs_assert(pred != 0, ECS_INVALID_PARAMETER, NULL);

        m_term->args[0].entity = pred;

        return *this;
    }

    flecs::id id() {
        return flecs::id(world(), m_term->id);
    }

    flecs::entity get_subject() {
        return flecs::entity(world(), m_term->args[0].entity);
    }

    flecs::entity get_object() {
        return flecs::entity(world(), m_term->args[1].entity);
    }

    flecs::inout_kind_t inout() {
        return static_cast<flecs::inout_kind_t>(m_term->inout);
    }

    flecs::oper_kind_t oper() {
        return static_cast<flecs::oper_kind_t>(m_term->oper);
    }

    ecs_term_t *m_term;

protected:
    virtual flecs::world_t* world() = 0;

    void set_term(ecs_term_t *term) {
        m_term = term;
        if (term) {
            this->m_term_id = &m_term->args[0]; // default to subject
        } else {
            this->m_term_id = nullptr;
        }
    }

    // A term can contain at most one component, but the parameter pack makes
    // the template parameter optional, which makes it easier to reuse the same
    // code for templated vs. non-templated terms.
    template <typename ... Components>
    void populate_term_from_pack() {
        flecs::array<flecs::id_t, sizeof...(Components)> ids ({
            (_::cpp_type<Components>::id(world()))...
        });

        flecs::array<flecs::inout_kind_t, sizeof...(Components)> inout_kinds ({
            (type_to_inout<Components>())...
        });

        flecs::array<flecs::oper_kind_t, sizeof...(Components)> oper_kinds ({
            (type_to_oper<Components>())...
        });

        size_t i = 0;
        for (auto the_id : ids) {
            this->id(the_id).inout(inout_kinds[i]).oper(oper_kinds[i]);
            i ++;
        }
    }

    template <typename T, if_t< is_const<T>::value > = 0>
    constexpr flecs::inout_kind_t type_to_inout() const {
        return flecs::In;
    }

    template <typename T, if_t< is_reference<T>::value > = 0>
    constexpr flecs::inout_kind_t type_to_inout() const {
        return flecs::Out;
    }

    template <typename T, if_not_t< 
        is_const<T>::value || is_reference<T>::value > = 0>
    constexpr flecs::inout_kind_t type_to_inout() const {
        return flecs::InOutDefault;
    }

    template <typename T, if_t< is_pointer<T>::value > = 0>
    constexpr flecs::oper_kind_t type_to_oper() const {
        return flecs::Optional;
    }

    template <typename T, if_not_t< is_pointer<T>::value > = 0>
    constexpr flecs::oper_kind_t type_to_oper() const {
        return flecs::And;
    } 

private:
    operator Base&() {
        return *static_cast<Base*>(this);
    }   
};

// Class that describes a term
class term final : public term_builder_i<term> {
public:
    term(flecs::world_t *world_ptr) 
        : term_builder_i<term>(&value)
        , value({})
        , m_world(world_ptr) { value.move = true; }

    term(flecs::world_t *world_ptr, id_t id) 
        : term_builder_i<term>(&value)
        , value({})
        , m_world(world_ptr) { 
            value.move = true; 
            this->id(id);
        }

    term(flecs::world_t *world_ptr, ecs_term_t t)
        : term_builder_i<term>(&value)
        , value({})
        , m_world(world_ptr) {
            value = t;
            value.move = false;
            this->set_term(&value);
        }        

    term(flecs::world_t *world_ptr, id_t r, id_t o) 
        : term_builder_i<term>(&value)
        , value({})
        , m_world(world_ptr) { 
            value.move = true; 
            this->id(r, o);
        }

    term(const term& obj) : term_builder_i<term>(&value) {
        m_world = obj.m_world;
        value = ecs_term_copy(&obj.value);
        this->set_term(&value);
    }

    term(term&& obj) : term_builder_i<term>(&value) {
        m_world = obj.m_world;
        value = ecs_term_move(&obj.value);
        obj.reset();
        this->set_term(&value);
    }

    term& operator=(const term& obj) {
        ecs_assert(m_world == obj.m_world, ECS_INVALID_PARAMETER, NULL);
        ecs_term_fini(&value);
        value = ecs_term_copy(&obj.value);
        this->set_term(&value);
        return *this;
    }

    term& operator=(term&& obj) {
        ecs_assert(m_world == obj.m_world, ECS_INVALID_PARAMETER, NULL);
        ecs_term_fini(&value);
        value = obj.value;
        this->set_term(&value);
        obj.reset();
        return *this;
    }   

    ~term() {
        ecs_term_fini(&value);
    }

    void reset() {
        value = {};
        this->set_term(nullptr);
    }

    int finalize() {
        return ecs_term_finalize(m_world, nullptr, &value);
    }

    bool is_set() {
        return ecs_term_is_initialized(&value);
    }

    bool is_trivial() {
        return ecs_term_is_trivial(&value);
    }

    ecs_term_t move() { /* explicit move to ecs_term_t */
        return ecs_term_move(&value);
    }

    ecs_term_t value;

protected:
    flecs::world_t* world() override { return m_world; }

private:
    flecs::world_t *m_world;
};

// Filter builder interface
template<typename Base, typename ... Components>
class filter_builder_i : public term_builder_i<Base> {
public:
    filter_builder_i(ecs_filter_desc_t *desc, int32_t term_index = 0) 
        : m_term_index(term_index)
        , m_desc(desc) { }

    Base& expr(const char *expr) {
        m_desc->expr = expr;
        return *this;
    }

    Base& substitute_default(bool value = true) {
        m_desc->substitute_default = value;
        return *this;
    }

    Base& term() {
        if (m_term_index >= ECS_TERM_DESC_CACHE_SIZE) {
            if (m_term_index == ECS_TERM_DESC_CACHE_SIZE) {
                m_desc->terms_buffer = ecs_os_calloc_n(
                    ecs_term_t, m_term_index + 1);
                ecs_os_memcpy_n(m_desc->terms_buffer, m_desc->terms, 
                    ecs_term_t, m_term_index);
                ecs_os_memset_n(m_desc->terms, 0, 
                    ecs_term_t, ECS_TERM_DESC_CACHE_SIZE);
            } else {
                m_desc->terms_buffer = ecs_os_realloc_n(m_desc->terms_buffer, 
                    ecs_term_t, m_term_index + 1);
            }

            m_desc->terms_buffer_count = m_term_index + 1;

            this->set_term(&m_desc->terms_buffer[m_term_index]);
        } else {
            this->set_term(&m_desc->terms[m_term_index]);
        }

        m_term_index ++;
        return *this;
    }
    
    Base& arg(int32_t term_index) {
        ecs_assert(term_index > 0, ECS_INVALID_PARAMETER, NULL);
        int32_t prev_index = m_term_index;
        m_term_index = term_index - 1;
        this->term();
        m_term_index = prev_index;
        ecs_assert(ecs_term_is_initialized(this->m_term), ECS_INVALID_PARAMETER, NULL);
        return *this;
    }    

    template<typename T>
    Base& term() {
        this->term();
        *this->m_term = flecs::term(world()).id<T>().move();
        return *this;
    }

    Base& term(id_t id) {
        this->term();
        *this->m_term = flecs::term(world()).id(id).move();
        return *this;
    }   

    template<typename R, typename O>
    Base& term() {
        this->term();
        *this->m_term = flecs::term(world()).id<R, O>().move();
        return *this;
    }

    template<typename R>
    Base& term(id_t o) {
        this->term();
        *this->m_term = flecs::term(world()).id<R>(o).move();
        return *this;
    }     

    Base& term(id_t r, id_t o) {
        this->term();
        *this->m_term = flecs::term(world()).id(r, o).move();
        return *this;
    }

    Base& term(const flecs::type& type) {
        this->term();
        *this->m_term = flecs::term(world()).id(type).move();
        return *this;
    }

    Base& term(const char *expr) {
        this->term();
        *this->m_term = flecs::term(world()).expr(expr).move();
        return *this;
    }

    Base& term(flecs::term& term) {
        this->term();
        *this->m_term = term.move();
        return *this;
    }

    Base& term(flecs::term&& term) {
        this->term();
        *this->m_term = term.move();
        return *this;
    }    

    void populate_filter_from_pack() {
        flecs::array<flecs::id_t, sizeof...(Components)> ids ({
            (_::cpp_type<Components>::id(world()))...
        });

        flecs::array<flecs::inout_kind_t, sizeof...(Components)> inout_kinds ({
            (this->template type_to_inout<Components>())...
        });

        flecs::array<flecs::oper_kind_t, sizeof...(Components)> oper_kinds ({
            (this->template type_to_oper<Components>())...
        });

        size_t i = 0;
        for (auto id : ids) {
            this->term(id).inout(inout_kinds[i]).oper(oper_kinds[i]);
            i ++;
        }
    }

protected:
    virtual flecs::world_t* world() = 0;
    int32_t m_term_index;

private:
    operator Base&() {
        return *static_cast<Base*>(this);
    }

    ecs_filter_desc_t *m_desc;
};

// Query builder interface
template<typename Base, typename ... Components>
class query_builder_i : public filter_builder_i<Base, Components ...> {
    using BaseClass = filter_builder_i<Base, Components ...>;
public:
    query_builder_i()
        : BaseClass(nullptr)
        , m_desc(nullptr) { }

    query_builder_i(ecs_query_desc_t *desc, int32_t term_index = 0) 
        : BaseClass(&desc->filter, term_index)
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
        ecs_order_by_action_t cmp = reinterpret_cast<ecs_order_by_action_t>(compare);
        return this->order_by(_::cpp_type<T>::id(world()), cmp);
    }

    /** Sort the output of a query.
     * Same as order_by<T>, but with component identifier.
     *
     * @param component The component used to sort.
     * @param compare The compare function used to sort the components.
     */    
    Base& order_by(flecs::entity_t component, int(*compare)(flecs::entity_t, const void*, flecs::entity_t, const void*)) {
        m_desc->order_by = reinterpret_cast<ecs_order_by_action_t>(compare);
        m_desc->order_by_component = component;
        return *this;
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
    Base& group_by(uint64_t(*rank)(flecs::world_t*, flecs::type_t type, flecs::id_t id, void* ctx)) {
        ecs_group_by_action_t rnk = reinterpret_cast<ecs_group_by_action_t>(rank);
        return this->group_by(_::cpp_type<T>::id(world()), rnk);
    }

    /** Group and sort matched tables.
     * Same as group_by<T>, but with component identifier.
     *
     * @param component The component used to determine the group rank.
     * @param rank The rank action.
     */
    Base& group_by(flecs::entity_t component, uint64_t(*rank)(flecs::world_t*, flecs::type_t type, flecs::id_t id, void* ctx)) {
        m_desc->group_by = reinterpret_cast<ecs_group_by_action_t>(rank);
        m_desc->group_by_id = component;
        return *this;
    } 

    /** Specify parent query (creates subquery) */
    Base& parent(const query_base& parent);
    
protected:
    virtual flecs::world_t* world() = 0;

private:
    operator Base&() {
        return *static_cast<Base*>(this);
    }

    ecs_query_desc_t *m_desc;
};

// System builder interface
template<typename Base, typename ... Components>
class system_builder_i : public query_builder_i<Base, Components ...> {
    using BaseClass = query_builder_i<Base, Components ...>;
public:
    system_builder_i()
        : BaseClass(nullptr)
        , m_desc(nullptr)
        , m_add_count(0) { }

    system_builder_i(ecs_system_desc_t *desc) 
        : BaseClass(&desc->query)
        , m_desc(desc)
        , m_add_count(0) { }

    /** Specify string-based signature. */
    Base& signature(const char *signature) {
        m_desc->query.filter.expr = signature;
        return *this;
    }

    /** Specify when the system should be ran.
     *
     * @param kind The kind that specifies when the system should be ran.
     */
    Base& kind(entity_t kind) {
        m_desc->entity.add[0] = kind;
        return *this;
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
        return *this;
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
        return *this;
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
        return *this;
    }
    
    /** Associate system with entity */
    Base& self(flecs::entity self) {
        m_desc->self = self;
        return *this;
    }

    /** Set system context */
    Base& ctx(void *ptr) {
        m_desc->ctx = ptr;
        return *this;
    }

protected:
    virtual flecs::world_t* world() = 0;

private:
    operator Base&() {
        return *static_cast<Base*>(this);
    }

    ecs_system_desc_t *m_desc;
    int32_t m_add_count;
};

// Trigger builder interface
template<typename Base, typename ... Components>
class trigger_builder_i : public term_builder_i<Base> {
    using BaseClass = term_builder_i<Base>;
public:
    trigger_builder_i()
        : BaseClass(nullptr)
        , m_desc(nullptr)
        , m_event_count(0) { }

    trigger_builder_i(ecs_trigger_desc_t *desc) 
        : BaseClass(&desc->term)
        , m_desc(desc)
        , m_event_count(0) { }

    /** Specify when the event(s) for which the trigger run.
     * @param kind The kind that specifies when the system should be ran.
     */
    Base& event(entity_t kind) {
        m_desc->events[m_event_count ++] = kind;
        return *this;
    }

    /** Associate trigger with entity */
    Base& self(flecs::entity self) {
        m_desc->self = self;
        return *this;
    }

    /** Set system context */
    Base& ctx(void *ptr) {
        m_desc->ctx = ptr;
        return *this;
    }    

protected:
    virtual flecs::world_t* world() = 0;

private:
    operator Base&() {
        return *static_cast<Base*>(this);
    }

    ecs_trigger_desc_t *m_desc;
    int32_t m_event_count;
};


// Observer builder interface
template<typename Base, typename ... Components>
class observer_builder_i : public filter_builder_i<Base, Components ...> {
    using BaseClass = filter_builder_i<Base, Components ...>;
public:
    observer_builder_i()
        : BaseClass(nullptr)
        , m_desc(nullptr)
        , m_event_count(0) { }

    observer_builder_i(ecs_observer_desc_t *desc) 
        : BaseClass(&desc->filter)
        , m_desc(desc)
        , m_event_count(0) { }

    /** Specify when the event(s) for which the trigger run.
     * @param kind The kind that specifies when the system should be ran.
     */
    Base& event(entity_t kind) {
        m_desc->events[m_event_count ++] = kind;
        return *this;
    }

    /** Associate observer with entity */
    Base& self(flecs::entity self) {
        m_desc->self = self;
        return *this;
    }

    /** Set system context */
    Base& ctx(void *ptr) {
        m_desc->ctx = ptr;
        return *this;
    }    

protected:
    virtual flecs::world_t* world() = 0;

private:
    operator Base&() {
        return *static_cast<Base*>(this);
    }

    ecs_observer_desc_t *m_desc;
    int32_t m_event_count;
};

// Filter builder
template<typename ... Components>
class filter_builder_base
    : public filter_builder_i<filter_builder_base<Components...>, Components ...>
{
public:
    filter_builder_base(flecs::world_t *world) 
        : filter_builder_i<filter_builder_base<Components...>, Components ...>(&m_desc)
        , m_desc({})
        , m_world(world)
    { 
        this->populate_filter_from_pack();
    }

    filter_builder_base(const filter_builder_base& obj) 
        : filter_builder_i<filter_builder_base<Components...>, Components ...>(&m_desc, obj.m_term_index)
    {
        m_world = obj.m_world;
        m_desc = obj.m_desc;
    }

    filter_builder_base(filter_builder_base&& obj) 
        : filter_builder_i<filter_builder_base<Components...>, Components ...>(&m_desc, obj.m_term_index)
    {
        m_world = obj.m_world;
        m_desc = obj.m_desc;
    }

    operator filter<Components ...>() const;

    operator ecs_filter_t() const {
        ecs_filter_t f;

        int res = ecs_filter_init(this->m_world, &f, &this->m_desc);
        if (res != 0) {
            ecs_abort(ECS_INVALID_PARAMETER, NULL);
        }

        if (this->m_desc.terms_buffer) {
            ecs_os_free(this->m_desc.terms_buffer);
        }

        return f;
    }

    filter<Components ...> build() const;

    ecs_filter_desc_t m_desc;

    flecs::world_t* world() override { return m_world; }

protected:
    flecs::world_t *m_world;
};

template<typename ... Components>
class filter_builder final : public filter_builder_base<Components...> {
public:
    filter_builder(flecs::world_t *world)
        : filter_builder_base<Components ...>(world) { }

    operator filter<>() const;
};

// Query builder
template<typename ... Components>
class query_builder_base
    : public query_builder_i<query_builder_base<Components...>, Components ...>
{
public:
    query_builder_base(flecs::world_t *world) 
        : query_builder_i<query_builder_base<Components...>, Components ...>(&m_desc)
        , m_desc({})
        , m_world(world)
    { 
        this->populate_filter_from_pack();
    }

    query_builder_base(const query_builder_base& obj) 
        : query_builder_i<query_builder_base<Components...>, Components ...>(&m_desc, obj.m_term_index)
    {
        m_world = obj.m_world;
        m_desc = obj.m_desc;
    }

    query_builder_base(query_builder_base&& obj) 
        : query_builder_i<query_builder_base<Components...>, Components ...>(&m_desc, obj.m_term_index)
    {
        m_world = obj.m_world;
        m_desc = obj.m_desc;
    }

    operator query<Components ...>() const;

    operator ecs_query_t*() const {
        ecs_query_t *result = ecs_query_init(this->m_world, &this->m_desc);

        if (!result) {
            ecs_abort(ECS_INVALID_PARAMETER, NULL);
        }

        if (this->m_desc.filter.terms_buffer) {
            ecs_os_free(m_desc.filter.terms_buffer);
        }
        
        return result;
    }    

    query<Components ...> build() const;

    ecs_query_desc_t m_desc;

    flecs::world_t* world() override { return m_world; }

protected:
    flecs::world_t *m_world;
};

template<typename ... Components>
class query_builder final : public query_builder_base<Components...> {
public:
    query_builder(flecs::world_t *world)
        : query_builder_base<Components ...>(world) { }

    operator query<>() const;
};

template<typename ... Components>
class system_builder final
    : public system_builder_i<system_builder<Components ...>, Components ...>
{
    using Class = system_builder<Components ...>;
public:
    explicit system_builder(flecs::world_t* world, const char *name = nullptr, const char *expr = nullptr) 
        : system_builder_i<Class, Components ...>(&m_desc)
        , m_desc({})
        , m_world(world)
        { 
            m_desc.entity.name = name;
            m_desc.entity.sep = "::";
            m_desc.entity.add[0] = flecs::OnUpdate;
            m_desc.query.filter.expr = expr;
            this->populate_filter_from_pack();
        }

    /* Iter (or each) is mandatory and always the last thing that 
     * is added in the fluent method chain. Create system signature from both 
     * template parameters and anything provided by the signature method. */
    template <typename Func>
    system<Components...> iter(Func&& func) const;

    /* Each is similar to action, but accepts a function that operates on a
     * single entity */
    template <typename Func>
    system<Components...> each(Func&& func) const;

    ecs_system_desc_t m_desc;

protected:
    flecs::world_t* world() override { return m_world; }
    flecs::world_t *m_world;

private:
    template <typename Invoker, typename Func>
    entity_t build(Func&& func, bool is_each) const {
        auto ctx = FLECS_NEW(Invoker)(std::forward<Func>(func));

        ecs_system_desc_t desc = m_desc;
        desc.callback = Invoker::run;
        desc.self = m_desc.self;
        desc.query.filter.substitute_default = is_each;
        desc.binding_ctx = ctx;
        desc.binding_ctx_free = reinterpret_cast<
            ecs_ctx_free_t>(_::free_obj<Invoker>);

        entity_t e = ecs_system_init(m_world, &desc);

        if (this->m_desc.query.filter.terms_buffer) {
            ecs_os_free(m_desc.query.filter.terms_buffer);
        }

        return e;
    }
};

template<typename ... Components>
class trigger_builder final
    : public trigger_builder_i<trigger_builder<Components...>, Components...>
{
    using Class = trigger_builder<Components...>;
public:
    explicit trigger_builder(flecs::world_t* world, const char *name = nullptr, flecs::id_t the_id = 0)
        : trigger_builder_i<Class, Components...>(&m_desc)
        , m_desc({})
        , m_world(world)
        { 
            m_desc.entity.name = name;
            m_desc.entity.sep = "::";
            this->template populate_term_from_pack<Components...>();
            
            if (the_id) {
                /* Id should not be set if term is populated from template */
                ecs_assert(!m_desc.term.id, ECS_INVALID_PARAMETER, NULL);
                this->id(the_id);
            }
        }

    /* Iter (or each) is mandatory and always the last thing that 
     * is added in the fluent method chain. Create system signature from both 
     * template parameters and anything provided by the signature method. */
    template <typename Func>
    trigger<Components...> iter(Func&& func);

    /* Each is similar to action, but accepts a function that operates on a
     * single entity */
    template <typename Func>
    trigger<Components...> each(Func&& func);

    ecs_trigger_desc_t m_desc;

protected:
    flecs::world_t* world() override { return m_world; }
    flecs::world_t *m_world;

private:
    template <typename Invoker, typename Func>
    entity_t build(Func&& func) {
        auto ctx = FLECS_NEW(Invoker)(std::forward<Func>(func));

        ecs_trigger_desc_t desc = m_desc;
        desc.callback = Invoker::run;
        desc.binding_ctx = ctx;
        desc.binding_ctx_free = reinterpret_cast<
            ecs_ctx_free_t>(_::free_obj<Invoker>);

        entity_t result = ecs_trigger_init(m_world, &desc);

        ecs_term_fini(&m_desc.term);
        
        return result;
    }
};

template<typename ... Components>
class observer_builder final
    : public observer_builder_i<observer_builder<Components ...>, Components ...>
{
    using Class = observer_builder<Components ...>;
public:
    explicit observer_builder(flecs::world_t* world, const char *name = nullptr, const char *expr = nullptr) 
        : observer_builder_i<Class, Components ...>(&m_desc)
        , m_desc({})
        , m_world(world)
        { 
            m_desc.entity.name = name;
            m_desc.entity.sep = "::";
            m_desc.filter.expr = expr;
            this->populate_filter_from_pack();
        }

    /* Iter (or each) is mandatory and always the last thing that 
     * is added in the fluent method chain. Create system signature from both 
     * template parameters and anything provided by the signature method. */
    template <typename Func>
    observer<Components...> iter(Func&& func) const;

    /* Each is similar to action, but accepts a function that operates on a
     * single entity */
    template <typename Func>
    observer<Components...> each(Func&& func) const;

    ecs_observer_desc_t m_desc;

protected:
    flecs::world_t* world() override { return m_world; }
    flecs::world_t *m_world;

private:
    template <typename Invoker, typename Func>
    entity_t build(Func&& func, bool is_each) const {
        auto ctx = FLECS_NEW(Invoker)(std::forward<Func>(func));

        ecs_observer_desc_t desc = m_desc;
        desc.callback = Invoker::run;
        desc.filter.substitute_default = is_each;
        desc.binding_ctx = ctx;
        desc.binding_ctx_free = reinterpret_cast<
            ecs_ctx_free_t>(_::free_obj<Invoker>);

        ecs_entity_t result = ecs_observer_init(m_world, &desc);

        if (this->m_desc.filter.terms_buffer) {
            ecs_os_free(m_desc.filter.terms_buffer);
        }

        return result;
    }
};

}
