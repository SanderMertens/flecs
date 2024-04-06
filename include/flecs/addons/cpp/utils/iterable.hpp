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
     *  - func(flecs::iter& it, size_t index, Components& ....)
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
        iterate<_::each_delegate>(world, FLECS_FWD(func), 
            this->next_each_action());
    }

    template <typename Func>
    void each(flecs::iter& it, Func&& func) const {
        iterate<_::each_delegate>(it.world(), FLECS_FWD(func),
            this->next_each_action());
    }

    template <typename Func>
    void each(flecs::entity e, Func&& func) const {
        iterate<_::each_delegate>(e.world(), FLECS_FWD(func), 
            this->next_each_action());
    }

    template <typename Func>
    flecs::entity find(Func&& func) const {
        return iterate_find<_::find_delegate>(nullptr, FLECS_FWD(func), 
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
        iterate<_::iter_delegate>(nullptr, FLECS_FWD(func), 
            this->next_action());
    }

    template <typename Func>
    void iter(flecs::world_t *world, Func&& func) const {
        iterate<_::iter_delegate>(world, FLECS_FWD(func), 
            this->next_action());
    }

    template <typename Func>
    void iter(flecs::iter& it, Func&& func) const {
        iterate<_::iter_delegate>(it.world(), FLECS_FWD(func),
            this->next_action());
    }

    template <typename Func>
    void iter(flecs::entity e, Func&& func) const {
        iterate<_::iter_delegate>(e.world(), FLECS_FWD(func), 
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

    template < template<typename Func, typename ... Comps> class Delegate, typename Func, typename NextFunc, typename ... Args>
    void iterate(flecs::world_t *stage, Func&& func, NextFunc next, Args &&... args) const {
        ecs_iter_t it = this->get_iter(stage);
        if (Delegate<Func, Components...>::instanced()) {
            ECS_BIT_SET(it.flags, EcsIterIsInstanced);
        }

        while (next(&it, FLECS_FWD(args)...)) {
            Delegate<Func, Components...>(func).invoke(&it);
        }
    }

    template < template<typename Func, typename ... Comps> class Delegate, typename Func, typename NextFunc, typename ... Args>
    flecs::entity iterate_find(flecs::world_t *stage, Func&& func, NextFunc next, Args &&... args) const {
        ecs_iter_t it = this->get_iter(stage);
        if (Delegate<Func, Components...>::instanced()) {
            ECS_BIT_SET(it.flags, EcsIterIsInstanced);
        }

        flecs::entity result;
        while (!result && next(&it, FLECS_FWD(args)...)) {
            result = Delegate<Func, Components...>(func).invoke(&it);
        }
        if (result) {
            ecs_iter_fini(&it);
        }
        return result;
    }
};

template <typename ... Components>
struct iter_iterable final : iterable<Components...> {
    template <typename Iterable>
    iter_iterable(Iterable *it, flecs::world_t *world) 
    {
        it_ = it->get_iter(world);
        next_ = it->next_action();
        next_each_ = it->next_action();
    }

    iter_iterable<Components...>& set_var(int var_id, flecs::entity_t value) {
        ecs_assert(var_id != -1, ECS_INVALID_PARAMETER, 0);
        ecs_iter_set_var(&it_, var_id, value);
        return *this;
    }

    iter_iterable<Components...>& set_var(const char *name, flecs::entity_t value) {
        ecs_query_iter_t *qit = &it_.priv.iter.query;
        int var_id = ecs_query_find_var(qit->query, name);
        ecs_assert(var_id != -1, ECS_INVALID_PARAMETER, name);
        ecs_iter_set_var(&it_, var_id, value);
        return *this;
    }

#   ifdef FLECS_JSON
#   include "../mixins/json/iterable.inl"
#   endif

    // Return total number of entities in result.
    int32_t count() {
        int32_t result = 0;
        while (next_each_(&it_)) {
            result += it_.count;
        }
        return result;
    }

    // Returns true if iterator yields at least once result.
    bool is_true() {
        bool result = next_each_(&it_);
        if (result) {
            ecs_iter_fini(&it_);
        }
        return result;
    }

    // Return first matching entity.
    flecs::entity first() {
        flecs::entity result;
        if (next_each_(&it_) && it_.count) {
            result = flecs::entity(it_.world, it_.entities[0]);
            ecs_iter_fini(&it_);
        }
        return result;
    }

    // Limit results to tables with specified group id (grouped queries only)
    iter_iterable<Components...>& set_group(uint64_t group_id) {
        ecs_iter_set_group(&it_, group_id);
        return *this;
    }

    // Limit results to tables with specified group id (grouped queries only)
    template <typename Group>
    iter_iterable<Components...>& set_group() {
        ecs_iter_set_group(&it_, _::type<Group>().id(it_.real_world));
        return *this;
    }

protected:
    ecs_iter_t get_iter(flecs::world_t *world) const {
        if (world) {
            ecs_iter_t result = it_;
            result.world = world;
            return result;
        }
        return it_;
    }

    ecs_iter_next_action_t next_action() const {
        return next_;
    }

    ecs_iter_next_action_t next_each_action() const {
        return next_each_;
    }

private:
    ecs_iter_t it_;
    ecs_iter_next_action_t next_;
    ecs_iter_next_action_t next_each_;
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
        : offset_(offset)
        , limit_(limit)
    {
        chain_it_ = it->get_iter(nullptr);
    }

protected:
    ecs_iter_t get_iter(flecs::world_t*) const {
        return ecs_page_iter(&chain_it_, offset_, limit_);
    }

    ecs_iter_next_action_t next_action() const {
        return ecs_page_next;
    }

    ecs_iter_next_action_t next_each_action() const {
        return ecs_page_next;
    }

private:
    ecs_iter_t chain_it_;
    int32_t offset_;
    int32_t limit_;
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
        : offset_(offset)
        , limit_(limit)
    {
        chain_it_ = it->get_iter(nullptr);
    }

protected:
    ecs_iter_t get_iter(flecs::world_t*) const {
        return ecs_worker_iter(&chain_it_, offset_, limit_);
    }

    ecs_iter_next_action_t next_action() const {
        return ecs_worker_next;
    }

    ecs_iter_next_action_t next_each_action() const {
        return ecs_worker_next;
    }

private:
    ecs_iter_t chain_it_;
    int32_t offset_;
    int32_t limit_;
};

template <typename ... Components>
worker_iterable<Components...> iterable<Components...>::worker(
    int32_t index, 
    int32_t count) 
{
    return worker_iterable<Components...>(index, count, this);
}

}
