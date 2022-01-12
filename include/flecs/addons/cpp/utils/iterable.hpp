
namespace flecs {

template <typename ... Components>
struct iter_iterable;

template <typename ... Components>
struct page_iterable;

template <typename ... Components>
struct worker_iterable;

template <typename ... Components>
struct iterable {
    /** Each iterator.
     * The "each" iterator accepts a function that is invoked for each matching
     * entity. The following function signatures are valid:
     *  - func(flecs::entity e, Components& ...)
     *  - func(flecs::iter& it, int32_t index, Components& ....)
     *  - func(Components& ...)
     * 
     * Each iterators are automatically instanced.
     */
    template <typename Func>
    void each(Func&& func) const {
        iterate<_::each_invoker>(FLECS_FWD(func), 
            this->next_each_action());
    }

    /** Iter iterator.
     * The "iter" iterator accepts a function that is invoked for each matching
     * table. The following function signatures are valid:
     *  - func(flecs::iter& it, Components* ...)
     *  - func(Components& ...)
     * 
     * Iter iterators are not automatically instanced. When a result contains
     * shared components, entities of the result will be iterated one by one.
     * This ensures that applications can't accidentally read out of bounds by
     * accessing a shared component as an array.
     */
    template <typename Func>
    void iter(Func&& func) const { 
        iterate<_::iter_invoker>(FLECS_FWD(func), this->next_action());
    }

    /** Create iterator.
     * Create an iterator object that can be modified before iterating.
     */
    iter_iterable<Components...> iter();

    /** Page iterator.
     * Create an iterator that limits the returned entities with offset/limit.
     * 
     * @param offset How many entities to skip.
     * @param limit The maximum number of entities to return.
     * @return Iterable that can be iterated with each/iter.
     */
    page_iterable<Components...> page(int32_t offset, int32_t limit);

    /** Worker iterator.
     * Create an iterator that divides the number of matched entities across
     * a number of resources.
     * 
     * @param index The index of the current resource.
     * @param count The total number of resources to divide entities between.
     * @return Iterable that can be iterated with each/iter.
     */
    worker_iterable<Components...> worker(int32_t index, int32_t count);

    virtual ~iterable() { }
protected:
    friend iter_iterable<Components...>;
    friend page_iterable<Components...>;
    friend worker_iterable<Components...>;

    virtual ecs_iter_t get_iter() const = 0;
    virtual ecs_iter_next_action_t next_action() const = 0;
    virtual ecs_iter_next_action_t next_each_action() const = 0;

    template < template<typename Func, typename ... Comps> class Invoker, typename Func, typename NextFunc, typename ... Args>
    void iterate(Func&& func, NextFunc next, Args &&... args) const {
        ecs_iter_t it = this->get_iter();
        it.is_instanced |= Invoker<Func, Components...>::instanced();

        while (next(&it, FLECS_FWD(args)...)) {
            Invoker<Func, Components...>(func).invoke(&it);
        }
    }
};

template <typename ... Components>
struct iter_iterable final : iterable<Components...> {
    template <typename Iterable>
    iter_iterable(Iterable *it) 
    {
        m_it = it->get_iter();
        m_next = it->next_action();
        m_next_each = it->next_action();
    }

#ifdef FLECS_RULES
    iter_iterable<Components...>& set_var(int var_id, flecs::entity_t value) {
        ecs_assert(m_it.next == ecs_rule_next, ECS_INVALID_OPERATION, NULL);
        ecs_assert(var_id != -1, ECS_INVALID_PARAMETER, 0);
        ecs_rule_set_var(&m_it, var_id, value);
        return *this;
    }

    iter_iterable<Components...>& set_var(const char *name, flecs::entity_t value) {
        ecs_assert(m_it.next == ecs_rule_next, ECS_INVALID_OPERATION, NULL);
        ecs_rule_iter_t *rit = &m_it.priv.iter.rule;
        int var_id = ecs_rule_find_var(rit->rule, name);
        ecs_assert(var_id != -1, ECS_INVALID_PARAMETER, name);
        ecs_rule_set_var(&m_it, var_id, value);
        return *this;
    }
#endif

protected:
    ecs_iter_t get_iter() const {
        return m_it;
    }

    ecs_iter_next_action_t next_action() const {
        return m_next;
    }

    ecs_iter_next_action_t next_each_action() const {
        return m_next_each;
    }

private:
    ecs_iter_t m_it;
    ecs_iter_next_action_t m_next;
    ecs_iter_next_action_t m_next_each;
};

template <typename ... Components>
iter_iterable<Components...> iterable<Components...>::iter() 
{
    return iter_iterable<Components...>(this);
}

template <typename ... Components>
struct page_iterable final : iterable<Components...> {
    template <typename Iterable>
    page_iterable(int32_t offset, int32_t limit, Iterable *it) 
        : m_offset(offset)
        , m_limit(limit)
    {
        m_chain_it = it->get_iter();
    }

protected:
    ecs_iter_t get_iter() const {
        return ecs_page_iter(&m_chain_it, m_offset, m_limit);
    }

    ecs_iter_next_action_t next_action() const {
        return ecs_page_next;
    }

    ecs_iter_next_action_t next_each_action() const {
        return ecs_page_next;
    }

private:
    ecs_iter_t m_chain_it;
    int32_t m_offset;
    int32_t m_limit;
};

template <typename ... Components>
page_iterable<Components...> iterable<Components...>::page(
    int32_t offset, 
    int32_t limit) 
{
    return page_iterable<Components...>(offset, limit, this);
}

template <typename ... Components>
struct worker_iterable final : iterable<Components...> {
    worker_iterable(int32_t offset, int32_t limit, iterable<Components...> *it) 
        : m_offset(offset)
        , m_limit(limit)
    {
        m_chain_it = it->get_iter();
    }

protected:
    ecs_iter_t get_iter() const {
        return ecs_worker_iter(&m_chain_it, m_offset, m_limit);
    }

    ecs_iter_next_action_t next_action() const {
        return ecs_worker_next;
    }

    ecs_iter_next_action_t next_each_action() const {
        return ecs_worker_next;
    }

private:
    ecs_iter_t m_chain_it;
    int32_t m_offset;
    int32_t m_limit;
};

template <typename ... Components>
worker_iterable<Components...> iterable<Components...>::worker(
    int32_t index, 
    int32_t count) 
{
    return worker_iterable<Components...>(index, count, this);
}

}
