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
     */
    template <typename Func>
    void each(Func&& func) const {
        ecs_iter_t it = this->get_iter(nullptr);
        ecs_iter_next_action_t next = this->next_action();
        while (next(&it)) {
            _::each_delegate<Func, Components...>(func).invoke(&it);
        }
    }

    /** Run iterator.
     * The "each" iterator accepts a function that is invoked once for a query
     * with a valid iterator. The following signature is valid:
     *  - func(flecs::iter&)
     */
    template <typename Func>
    void run(Func&& func) const {
        ecs_iter_t it = this->get_iter(nullptr);
        _::run_delegate<Func>(func).invoke(&it);
    }

    template <typename Func>
    flecs::entity find(Func&& func) const {
        ecs_iter_t it = this->get_iter(nullptr);
        ecs_iter_next_action_t next = this->next_action();

        flecs::entity result;
        while (!result && next(&it)) {
            result = _::find_delegate<Func, Components...>(func).invoke(&it);
        }

        if (result) {
            ecs_iter_fini(&it);
        }

        return result;
    }

    /** Create iterator.
     * Create an iterator object that can be modified before iterating.
     */
    iter_iterable<Components...> iter(flecs::world_t *world = nullptr) const;

    /** Create iterator.
     * Create an iterator object that can be modified before iterating.
     */
    iter_iterable<Components...> iter(flecs::iter& iter) const;

    /** Create iterator.
     * Create an iterator object that can be modified before iterating.
     */
    iter_iterable<Components...> iter(flecs::entity e) const;

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

    iter_iterable<Components...> set_var(int var_id, flecs::entity_t value) {
        return this->iter().set_var(var_id, value);
    }

    iter_iterable<Components...> set_var(const char *name, flecs::entity_t value) {
        return this->iter().set_var(name, value);
    }

    iter_iterable<Components...> set_var(const char *name, flecs::table_t *value) {
        return this->iter().set_var(name, value);
    }

    iter_iterable<Components...> set_var(const char *name, ecs_table_range_t value) {
        return this->iter().set_var(name, value);
    }

    iter_iterable<Components...> set_var(const char *name, flecs::table_range value) {
        return this->iter().set_var(name, value);
    }

    // Limit results to tables with specified group id (grouped queries only)
    iter_iterable<Components...> set_group(uint64_t group_id) {
        return this->iter().set_group(group_id);
    }

    // Limit results to tables with specified group id (grouped queries only)
    template <typename Group>
    iter_iterable<Components...> set_group() {
        return this->iter().template set_group<Group>();
    }

    virtual ~iterable() { }
protected:
    friend iter_iterable<Components...>;
    friend page_iterable<Components...>;
    friend worker_iterable<Components...>;

    virtual ecs_iter_t get_iter(flecs::world_t *stage) const = 0;
    virtual ecs_iter_next_action_t next_action() const = 0;
};

template <typename ... Components>
struct iter_iterable final : iterable<Components...> {
    template <typename Iterable>
    iter_iterable(Iterable *it, flecs::world_t *world) 
    {
        it_ = it->get_iter(world);
        next_ = it->next_action();
        next_each_ = it->next_action();
        ecs_assert(next_ != nullptr, ECS_INTERNAL_ERROR, NULL);
        ecs_assert(next_each_ != nullptr, ECS_INTERNAL_ERROR, NULL);
    }

    iter_iterable<Components...>& set_var(int var_id, flecs::entity_t value) {
        ecs_assert(var_id != -1, ECS_INVALID_PARAMETER, 0);
        ecs_iter_set_var(&it_, var_id, value);
        return *this;
    }

    iter_iterable<Components...>& set_var(const char *name, flecs::entity_t value) {
        ecs_query_iter_t *qit = &it_.priv_.iter.query;
        int var_id = ecs_query_find_var(qit->query, name);
        ecs_assert(var_id != -1, ECS_INVALID_PARAMETER, name);
        ecs_iter_set_var(&it_, var_id, value);
        return *this;
    }

    iter_iterable<Components...>& set_var(const char *name, flecs::table_t *value) {
        ecs_query_iter_t *qit = &it_.priv_.iter.query;
        int var_id = ecs_query_find_var(qit->query, name);
        ecs_assert(var_id != -1, ECS_INVALID_PARAMETER, name);
        ecs_iter_set_var_as_table(&it_, var_id, value);
        return *this;
    }

    iter_iterable<Components...>& set_var(const char *name, ecs_table_range_t value) {
        ecs_query_iter_t *qit = &it_.priv_.iter.query;
        int var_id = ecs_query_find_var(qit->query, name);
        ecs_assert(var_id != -1, ECS_INVALID_PARAMETER, name);
        ecs_iter_set_var_as_range(&it_, var_id, &value);
        return *this;
    }

    iter_iterable<Components...>& set_var(const char *name, flecs::table_range value) {
        ecs_table_range_t range;
        range.table = value.get_table();
        range.offset = value.offset();
        range.count = value.count();
        return set_var(name, range);
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
    ecs_iter_t get_iter(flecs::world_t *world) const override {
        if (world) {
            ecs_iter_t result = it_;
            result.world = world;
            return result;
        }
        return it_;
    }

    ecs_iter_next_action_t next_action() const override {
        return next_;
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
iter_iterable<Components...> iterable<Components...>::iter(flecs::iter& it) const
{
    return iter_iterable<Components...>(this, it.world());
}

template <typename ... Components>
iter_iterable<Components...> iterable<Components...>::iter(flecs::entity e) const
{
    return iter_iterable<Components...>(this, e.world());
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
