/**
 * @file addons/cpp/utils/iterable.hpp
 * @brief Base class for iterable objects, like queries.
 */

namespace flecs {

/** Forward declaration of iter_iterable. */
template <typename ... Components>
struct iter_iterable;

namespace _ {
    template <auto Create, ecs_iter_next_action_t Next, typename... Components>
    struct chained_iterable;
}

template <typename... Components>
using page_iterable = _::chained_iterable<ecs_page_iter, ecs_page_next, Components...>;

template <typename... Components>
using worker_iterable = _::chained_iterable<ecs_worker_iter, ecs_worker_next, Components...>;

/** Base class for iterable query objects. */
template <typename ... Components>
struct iterable {

    /** Each iterator.
     * The "each" iterator accepts a function that is invoked for each matching
     * entity. The following function signatures are valid:
     *  - func(flecs::entity e, Components& ...)
     *  - func(flecs::iter& it, size_t index, Components& ...)
     *  - func(Components& ...)
     *
     * @param func The callback function.
     */
    template <typename Func>
    void each(Func&& func) const {
        ecs_iter_t it = this->get_iter(nullptr);
        ecs_iter_next_action_t next = this->next_action();
        while (next(&it)) {
            _::each_delegate<Func, Components...>(func).invoke(&it);
        }
    }

    /** Run the iterator.
     * The "run" callback accepts a function that is invoked once for a query
     * with a valid iterator. The following signature is valid:
     *  - func(flecs::iter&)
     *
     * @param func The callback function.
     */
    template <typename Func>
    void run(Func&& func) const {
        ecs_iter_t it = this->get_iter(nullptr);
        _::run_delegate<Func>(func).invoke(&it);
    }

    /** Find the first entity matching a condition.
     * Return the first entity for which the provided function returns true.
     *
     * @param func The predicate function.
     * @return The first matching entity, or an empty entity if none found.
     */
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

    /** Create an iterator.
     * Create an iterator object that can be modified before iterating.
     */
    iter_iterable<Components...> iter(flecs::world_t *world = nullptr) const;

    /** Create an iterator.
     * Create an iterator object that can be modified before iterating.
     */
    iter_iterable<Components...> iter(flecs::iter& iter) const;

    /** Create an iterator.
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

    /** Return the number of entities matched by the iterable. */
    int32_t count() const {
        return this->iter().count();
    }

    /** Return whether the iterable has any matches. */
    bool is_true() const {
        return this->iter().is_true();
    }

    /** Return the first entity matched by the iterable. */
    flecs::entity first() const {
        return this->iter().first();
    }

    template <typename Var, typename Value>
    iter_iterable<Components...> set_var(Var var, Value value) const {
        return this->iter().set_var(var, value);
    }

#ifdef FLECS_CACHED_QUERIES
    /** Limit results to tables with the specified group ID (grouped queries only). */
    iter_iterable<Components...> set_group(uint64_t group_id) const {
        return this->iter().set_group(group_id);
    }

    /** Limit results to tables with the specified group type (grouped queries only). */
    template <typename Group>
    iter_iterable<Components...> set_group() const {
        return this->iter().template set_group<Group>();
    }
#endif

    /** Virtual destructor. */
    virtual ~iterable() { }
protected:
    friend iter_iterable<Components...>;
    friend page_iterable<Components...>;
    friend worker_iterable<Components...>;

    virtual ecs_iter_t get_iter(flecs::world_t *stage) const = 0;
    virtual ecs_iter_next_action_t next_action() const = 0;
};

/** Iterable adapter for iterating with iter/each/run. */
template <typename ... Components>
struct iter_iterable final : iterable<Components...> {
    /** Construct iter_iterable from an iterable and a world. */
    template <typename Iterable>
    iter_iterable(Iterable *it, flecs::world_t *world)
    {
        it_ = it->get_iter(world);
        next_ = it->next_action();
        ecs_assert(next_ != nullptr, ECS_INTERNAL_ERROR, nullptr);
    }

    template <typename Var, typename Value>
    iter_iterable& set_var(Var var, Value value) {
        int var_id;
#ifdef FLECS_QUERY_PLANS
        if constexpr (std::is_convertible_v<Var, const char*>) {
            var_id = ecs_query_find_var(it_.query, var);
        } else
#endif
        {
            var_id = var;
        }
        ecs_assert(var_id != -1, ECS_INVALID_PARAMETER, "query variable was not found");
        if constexpr (std::is_convertible_v<Value, flecs::entity_t>) {
            ecs_iter_set_var(&it_, var_id, value);
        } else if constexpr (std::is_convertible_v<Value, flecs::table_t*> &&
            !std::is_base_of_v<flecs::table_range, Value>)
        {
            ecs_iter_set_var_as_table(&it_, var_id, value);
        } else {
            ecs_table_range_t range;
            if constexpr (std::is_same_v<Value, ecs_table_range_t>) {
                range = value;
            } else {
                range = {value.get_table(), value.offset(), value.count()};
            }
            ecs_iter_set_var_as_range(&it_, var_id, &range);
        }
        return *this;
    }

#   ifdef FLECS_JSON
#   include "../mixins/json/iterable.inl"
#   endif

    /** Return the total number of entities in the result. */
    int32_t count() {
        int32_t result = 0;
        while (next_(&it_)) {
            result += it_.count;
        }
        return result;
    }

    /** Return whether the iterator yields at least one result. */
    bool is_true() {
        bool result = next_(&it_);
        if (result) {
            ecs_iter_fini(&it_);
        }
        return result;
    }

    /** Return the first matching entity. */
    flecs::entity first() {
        flecs::entity result;
        if (next_(&it_) && it_.count) {
            result = flecs::entity(it_.world, it_.entities[0]);
            ecs_iter_fini(&it_);
        }
        return result;
    }

#ifdef FLECS_CACHED_QUERIES
    /** Limit results to tables with the specified group ID (grouped queries only). */
    iter_iterable<Components...>& set_group(uint64_t group_id) {
        ecs_iter_set_group(&it_, group_id);
        return *this;
    }

    /** Limit results to tables with the specified group type (grouped queries only). */
    template <typename Group>
    iter_iterable<Components...>& set_group() {
        ecs_iter_set_group(&it_, _::type<Group>().id(it_.real_world));
        return *this;
    }
#endif

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

namespace _ {

template <auto Create, ecs_iter_next_action_t Next, typename... Components>
struct chained_iterable final : iterable<Components...> {
    chained_iterable(int32_t first, int32_t second, const iterable<Components...> *source)
        : chain_it_(source->get_iter(nullptr)), args_{first, second} { }

protected:
    ecs_iter_t get_iter(flecs::world_t*) const override {
        return Create(&chain_it_, args_[0], args_[1]);
    }

    ecs_iter_next_action_t next_action() const override {
        return Next;
    }

private:
    ecs_iter_t chain_it_;
    int32_t args_[2];
};

}

template <typename... Components>
page_iterable<Components...> iterable<Components...>::page(int32_t offset, int32_t limit) {
    return page_iterable<Components...>(offset, limit, this);
}

template <typename... Components>
worker_iterable<Components...> iterable<Components...>::worker(int32_t index, int32_t count) {
    return worker_iterable<Components...>(index, count, this);
}

}
