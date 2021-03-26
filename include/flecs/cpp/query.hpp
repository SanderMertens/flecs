
namespace flecs 
{

////////////////////////////////////////////////////////////////////////////////
//// Persistent queries
////////////////////////////////////////////////////////////////////////////////

class query_base {
public:
    query_base()
        : m_world(nullptr), m_query(nullptr) { }    
    
    query_base(world_t *world, query_t *query)
        : m_world(world), m_query(query) { }

    /** Get pointer to C query object.
     */
    query_t* c_ptr() const {
        return m_query;
    }

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
    void order_by(int(*compare)(flecs::entity_t, const T*, flecs::entity_t, const T*)) {
        ecs_query_order_by(m_world, m_query, 
            flecs::_::cpp_type<T>::id(m_world),
            reinterpret_cast<ecs_compare_action_t>(compare));
    }

    /** Sort the output of a query.
     * Same as order_by<T>, but with component identifier.
     *
     * @param component The component used to sort.
     * @param compare The compare function used to sort the components.
     */
    void order_by(flecs::entity component, int(*compare)(flecs::entity_t, const void*, flecs::entity_t, const void*)) {
        ecs_query_order_by(m_world, m_query, component.id(), compare);
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
    void group_by(int(*rank)(flecs::world_t*, flecs::entity_t, flecs::type_t type)) {
        ecs_query_group_by(m_world, m_query, 
            flecs::_::cpp_type<T>::id(m_world), rank);
    }

    /** Group and sort matched tables.
     * Same as group_by<T>, but with component identifier.
     *
     * @param component The component used to determine the group rank.
     * @param rank The rank action.
     */
    void group_by(flecs::entity component, int(*rank)(flecs::world_t*, flecs::entity_t, flecs::type_t type)) {
        ecs_query_group_by(m_world, m_query, component.id(), rank);
    }

    /** Returns whether the query data changed since the last iteration.
     * This operation must be invoked before obtaining the iterator, as this will
     * reset the changed state. The operation will return true after:
     * - new entities have been matched with
     * - matched entities were deleted
     * - matched components were changed
     * 
     * @return true if entities changed, otherwise false.
     */
    bool changed() {
        return ecs_query_changed(m_query);
    }

    /** Returns whether query is orphaned.
     * When the parent query of a subquery is deleted, it is left in an orphaned
     * state. The only valid operation on an orphaned query is deleting it. Only
     * subqueries can be orphaned.
     *
     * @return true if query is orphaned, otherwise false.
     */
    bool orphaned() {
        return ecs_query_orphaned(m_query);
    }

    /** Free the query.
     */
    void destruct() {
        ecs_query_free(m_query);
        m_world = nullptr;
        m_query = nullptr;
    }

    template <typename Func>
    void iter(Func&& func) const {
        ecs_iter_t it = ecs_query_iter(m_query);

        while (ecs_query_next(&it)) {
            _::column_args<> columns(&it);
            _::iter_invoker<Func>::call_system(&it, func, 0, columns.m_columns);
        }
    }  

protected:
    world_t *m_world;
    query_t *m_query;
};

template<typename ... Components>
class query : public query_base {
    using Columns = typename _::column_args<Components...>::Columns;

public:
    query() : query_base(nullptr, nullptr) { }

    explicit query(const world& world) {
        flecs::stringstream str;
        if (!_::pack_args_to_string<Components...>(world.c_ptr(), str, true)) {
            ecs_abort(ECS_INVALID_PARAMETER, NULL);
        }

        m_world = world.c_ptr();
        m_query = ecs_query_new(world.c_ptr(), str.str().c_str());
    }

    explicit query(const world& world, query_base& parent) {
        flecs::stringstream str;
        if (!_::pack_args_to_string<Components...>(world.c_ptr(), str, true)) {
            ecs_abort(ECS_INVALID_PARAMETER, NULL);
        }

        m_world = world.c_ptr();
        m_query = ecs_subquery_new(world.c_ptr(), parent.c_ptr(), str.str().c_str());
    }

    explicit query(const world& world, const char *expr) {
        flecs::stringstream str;
        m_world = world.c_ptr();
        if (!_::pack_args_to_string<Components...>(world.c_ptr(), str, true)) {
            m_query = ecs_query_new(world.c_ptr(), expr);
        } else {
            str << "," << expr;
            m_query = ecs_query_new(world.c_ptr(), str.str().c_str());
        }
    }

    explicit query(const world& world, query_base& parent, const char *expr) {
        flecs::stringstream str;
        m_world = world.c_ptr();
        if (!_::pack_args_to_string<Components...>(world.c_ptr(), str, true)) {
            m_query = ecs_subquery_new(world.c_ptr(), parent.c_ptr(), expr);
        } else {
            str << "," << expr;
            m_query = ecs_subquery_new(world.c_ptr(), parent.c_ptr(), str.str().c_str());
        }
    }

    query_iterator<Components...> begin() const;

    query_iterator<Components...> end() const;

    template <typename Func>
    void each(Func&& func) const {
        ecs_iter_t it = ecs_query_iter(m_query);

        while (ecs_query_next(&it)) {
            _::column_args<Components...> columns(&it);
            _::each_invoker<Func, Components...>::call_system(
                &it, func, 0, columns.m_columns);
        }
    } 

    template <typename Func>
    void each(const flecs::filter& filter, Func&& func) const {
        ecs_iter_t it = ecs_query_iter(m_query);
        const filter_t* filter_ptr = filter.c_ptr();

        while (ecs_query_next_w_filter(&it, filter_ptr)) {
            _::column_args<Components...> columns(&it);
            _::each_invoker<Func, Components...>::call_system(
                &it, func, 0, columns.m_columns);
        }
    }

    template <typename Func>
    void each_worker(int32_t stage_current, int32_t stage_count, Func&& func) const {
        ecs_iter_t it = ecs_query_iter(m_query);

        while (ecs_query_next_worker(&it, stage_current, stage_count)) {
            _::column_args<Components...> columns(&it);
            _::each_invoker<Func, Components...>::call_system(
                &it, func, 0, columns.m_columns);
        }
    }

    template <typename Func>
    ECS_DEPRECATED("use each or iter")
    void action(Func&& func) const {
        ecs_iter_t it = ecs_query_iter(m_query);

        while (ecs_query_next(&it)) {
            _::column_args<Components...> columns(&it);
            _::action_invoker<Func, Components...>::call_system(&it, func, 0, columns.m_columns);
        }
    }

    template <typename Func>
    void iter(Func&& func) const {
        ecs_iter_t it = ecs_query_iter(m_query);

        while (ecs_query_next(&it)) {
            _::column_args<Components...> columns(&it);
            _::iter_invoker<Func, Components...>::call_system(&it, func, 0, columns.m_columns);
        }
    }  

    template <typename Func>
    void iter(const flecs::filter& filter, Func&& func) const {
        ecs_iter_t it = ecs_query_iter(m_query);        
        const filter_t* filter_ptr = filter.c_ptr();

        while (ecs_query_next_w_filter(&it, filter_ptr)) {
            _::column_args<Components...> columns(&it);
            _::iter_invoker<Func, Components...>::call_system(&it, func, 0, columns.m_columns);
        }
    }

    template <typename Func>
    void iter_worker(int32_t stage_current, int32_t stage_count, Func&& func) const {
        ecs_iter_t it = ecs_query_iter(m_query);

        while (ecs_query_next_worker(&it, stage_current, stage_count)) {
            _::column_args<Components...> columns(&it);
            _::iter_invoker<Func, Components...>::call_system(&it, func, 0, columns.m_columns);
        }
    }      
};

////////////////////////////////////////////////////////////////////////////////
//// Persistent queries
////////////////////////////////////////////////////////////////////////////////

template<typename ... Components>
class query_iterator
{
public:
    query_iterator()
        : m_has_next(false)
        , m_iter{ } { }

    query_iterator(const query<Components...>& query) 
        : m_iter( ecs_query_iter(query.c_ptr()) )
    {
        m_has_next = ecs_query_next(&m_iter);
    }

    bool operator!=(query_iterator const& other) const {
        return m_has_next != other.m_has_next;
    }

    flecs::iter const operator*() const {
        return flecs::iter(&m_iter);
    }

    query_iterator& operator++() {
        m_has_next = ecs_query_next(&m_iter);
        return *this;
    }

private:
    bool m_has_next;
    ecs_iter_t m_iter;
};

template<typename ... Components>
inline query_iterator<Components...> query<Components...>::begin() const {
    return query_iterator<Components...>(*this);
}

template<typename ... Components>
inline query_iterator<Components...> query<Components...>::end() const {
    return query_iterator<Components...>();
}

} // namespace flecs
