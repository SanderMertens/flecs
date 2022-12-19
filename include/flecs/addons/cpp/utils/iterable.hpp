/**
 * @file addons/cpp/utils/iterable.hpp
 * @brief Base class for iterable objects, like queries.
 */

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
        each(nullptr, FLECS_FWD(func));
    }

    template <typename Func>
    void each(flecs::world_t *world, Func&& func) const {
        iterate<_::each_invoker>(world, FLECS_FWD(func), 
            this->next_each_action());
    }

    template <typename Func>
    void each(flecs::iter& it, Func&& func) const {
        iterate<_::each_invoker>(it.world(), FLECS_FWD(func),
            this->next_each_action());
    }

    template <typename Func>
    void each(flecs::entity e, Func&& func) const {
        iterate<_::each_invoker>(e.world(), FLECS_FWD(func), 
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
        iterate<_::iter_invoker>(nullptr, FLECS_FWD(func), 
            this->next_action());
    }

    template <typename Func>
    void iter(flecs::world_t *world, Func&& func) const {
        iterate<_::iter_invoker>(world, FLECS_FWD(func), 
            this->next_action());
    }

    template <typename Func>
    void iter(flecs::iter& it, Func&& func) const {
        iterate<_::iter_invoker>(it.world(), FLECS_FWD(func),
            this->next_action());
    }

    template <typename Func>
    void iter(flecs::entity e, Func&& func) const {
        iterate<_::iter_invoker>(e.world(), FLECS_FWD(func), 
            this->next_action());
    }

    /** Create iterator.
     * Create an iterator object that can be modified before iterating.
     */
    iter_iterable<Components...> iter(flecs::world_t *world = nullptr) const;

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

    /** Return number of entities matched by iterable. */
    int32_t count() const {
        return this->iter().count();
    }

    /** Return whether iterable has any matches. */
    bool is_true() const {
        return this->iter().is_true();
    }

    /** Return first entity matched by iterable. */
    flecs::entity first() const {
        return this->iter().first();
    }

    virtual ~iterable() { }
protected:
    friend iter_iterable<Components...>;
    friend page_iterable<Components...>;
    friend worker_iterable<Components...>;

    virtual ecs_iter_t get_iter(flecs::world_t *stage) const = 0;
    virtual ecs_iter_next_action_t next_action() const = 0;
    virtual ecs_iter_next_action_t next_each_action() const = 0;

    template < template<typename Func, typename ... Comps> class Invoker, typename Func, typename NextFunc, typename ... Args>
    void iterate(flecs::world_t *stage, Func&& func, NextFunc next, Args &&... args) const {
        ecs_iter_t it = this->get_iter(stage);
        if (Invoker<Func, Components...>::instanced()) {
            ECS_BIT_SET(it.flags, EcsIterIsInstanced);
        }

        while (next(&it, FLECS_FWD(args)...)) {
            Invoker<Func, Components...>(func).invoke(&it);
        }
    }
};

template <typename ... Components>
struct iter_iterable final : iterable<Components...> {
    template <typename Iterable>
    iter_iterable(Iterable *it, flecs::world_t *world) 
    {
        m_it = it->get_iter(world);
        m_next = it->next_action();
        m_next_each = it->next_action();
    }

#   ifdef FLECS_RULES
#   include "../mixins/rule/iterable.inl"
#   endif
#   ifdef FLECS_JSON
#   include "../mixins/json/iterable.inl"
#   endif

    // Return total number of entities in result.
    int32_t count() {
        int32_t result = 0;
        while (m_next_each(&m_it)) {
            result += m_it.count;
        }
        return result;
    }

    // Returns true if iterator yields at least once result.
    bool is_true() {
        bool result = m_next_each(&m_it);
        if (result) {
            ecs_iter_fini(&m_it);
        }
        return result;
    }

    // Return first matching entity.
    flecs::entity first() {
        flecs::entity result;
        if (m_next_each(&m_it) && m_it.count) {
            result = flecs::entity(m_it.world, m_it.entities[0]);
            ecs_iter_fini(&m_it);
        }
        return result;
    }

    // Limit results to tables with specified group id (grouped queries only)
    iter_iterable<Components...>& set_group(uint64_t group_id) {
        ecs_query_set_group(&m_it, group_id);
        return *this;
    }

    // Limit results to tables with specified group id (grouped queries only)
    template <typename Group>
    iter_iterable<Components...>& set_group() {
        ecs_query_set_group(&m_it, _::cpp_type<Group>().id(m_it.real_world));
        return *this;
    }

protected:
    ecs_iter_t get_iter(flecs::world_t *world) const {
        if (world) {
            ecs_iter_t result = m_it;
            result.world = world;
            return result;
        }
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
iter_iterable<Components...> iterable<Components...>::iter(flecs::world_t *world) const
{
    return iter_iterable<Components...>(this, world);
}

template <typename ... Components>
struct page_iterable final : iterable<Components...> {
    template <typename Iterable>
    page_iterable(int32_t offset, int32_t limit, Iterable *it) 
        : m_offset(offset)
        , m_limit(limit)
    {
        m_chain_it = it->get_iter(nullptr);
    }

protected:
    ecs_iter_t get_iter(flecs::world_t*) const {
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
        m_chain_it = it->get_iter(nullptr);
    }

protected:
    ecs_iter_t get_iter(flecs::world_t*) const {
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
