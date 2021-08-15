
namespace flecs 
{

////////////////////////////////////////////////////////////////////////////////
//// Persistent queries
////////////////////////////////////////////////////////////////////////////////

class query_base {
public:
    query_base()
        : m_world(nullptr)
        , m_query(nullptr) { }    
    
    query_base(world_t *world, query_t *query = nullptr)
        : m_world(world)
        , m_query(query) { }

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
            reinterpret_cast<ecs_order_by_action_t>(compare));
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
    void group_by(ecs_group_by_action_t callback) {
        ecs_query_group_by(m_world, m_query, 
            flecs::_::cpp_type<T>::id(m_world), callback);
    }

    /** Group and sort matched tables.
     * Same as group_by<T>, but with component identifier.
     *
     * @param component The component used to determine the group rank.
     * @param rank The rank action.
     */
    void group_by(flecs::entity component, ecs_group_by_action_t callback) {
        ecs_query_group_by(m_world, m_query, component.id(), callback);
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
            _::iter_invoker<Func>(func).invoke(&it);
        }
    }  

    template <typename Func>
    void each_term(const Func& func) {
        const ecs_filter_t *f = ecs_query_get_filter(m_query);
        ecs_assert(f != NULL, ECS_INVALID_PARAMETER, NULL);

        for (int i = 0; i < f->term_count; i ++) {
            flecs::term t(m_world, f->terms[i]);
            func(t);
        }
    }

    flecs::term term(int32_t index) {
        const ecs_filter_t *f = ecs_query_get_filter(m_query);
        ecs_assert(f != NULL, ECS_INVALID_PARAMETER, NULL);
        return flecs::term(m_world, f->terms[index]);
    }

    int32_t term_count() {
        const ecs_filter_t *f = ecs_query_get_filter(m_query);
        return f->term_count;   
    }

    flecs::string str() {
        const ecs_filter_t *f = ecs_query_get_filter(m_query);
        char *result = ecs_filter_str(m_world, f);
        return flecs::string(result);
    }

protected:
    world_t *m_world;
    query_t *m_query;
};


template<typename ... Components>
class query : public query_base {
    using Terms = typename _::term_ptrs<Components...>::array;

public:
    query() { }

    query(world_t *world, query_t *q)
        : query_base(world, q) { }

    explicit query(const world& world, const char *expr = nullptr) 
        : query_base(world.c_ptr())
    {
        auto qb = world.query_builder<Components ...>()
            .expr(expr);

        if (!expr) {
            qb.substitute_default();
        }

        m_query = qb;
    }

    explicit query(const world& world, query_base& parent, const char *expr = nullptr) 
        : query_base(world.c_ptr())    
    {
        auto qb = world.query_builder<Components ...>()
            .parent(parent)
            .expr(expr);

        if (!expr) {
            qb.substitute_default();
        }

        m_query = qb;
    }

    template <typename Func>
    void each(Func&& func) const {
        iterate<_::each_invoker>(std::forward<Func>(func), ecs_query_next);
    } 

    template <typename Func>
    void each_worker(int32_t stage_current, int32_t stage_count, Func&& func) const {
        iterate<_::each_invoker>(std::forward<Func>(func), 
            ecs_query_next_worker, stage_current, stage_count);
    }

    template <typename Func>
    void iter(Func&& func) const { 
        iterate<_::iter_invoker>(std::forward<Func>(func), ecs_query_next);
    }

    template <typename Func>
    void iter_worker(int32_t stage_current, int32_t stage_count, Func&& func) const {
        iterate<_::iter_invoker>(std::forward<Func>(func), 
            ecs_query_next_worker, stage_current, stage_count);
    }

    template <typename Func>
    ECS_DEPRECATED("use each or iter")
    void action(Func&& func) const {
        iterate<_::action_invoker>(std::forward<Func>(func), ecs_query_next);
    }

private:
    template < template<typename Func, typename ... Comps> class Invoker, typename Func, typename NextFunc, typename ... Args>
    void iterate(Func&& func, NextFunc next, Args &&... args) const {
        ecs_iter_t it = ecs_query_iter(m_query);
        while (next(&it, std::forward<Args>(args)...)) {
            Invoker<Func, Components...>(func).invoke(&it);
        }
    }
};

} // namespace flecs
