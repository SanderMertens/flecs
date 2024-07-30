/**
 * @file addons/cpp/mixins/query/builder_i.hpp
 * @brief Query builder interface.
 */

#pragma once

#include "../term/builder_i.hpp"

namespace flecs 
{

/** Query builder interface.
 * 
 * @ingroup cpp_core_queries
 */
template<typename Base, typename ... Components>
struct query_builder_i : term_builder_i<Base> {
    query_builder_i(ecs_query_desc_t *desc, int32_t term_index = 0) 
        : term_index_(term_index)
        , expr_count_(0)
        , desc_(desc) { }

    Base& query_flags(ecs_flags32_t flags) {
        desc_->flags |= flags;
        return *this;
    }

    Base& cache_kind(query_cache_kind_t kind) {
        desc_->cache_kind = static_cast<ecs_query_cache_kind_t>(kind);
        return *this;
    }

    Base& cached() {
        return cache_kind(flecs::QueryCacheAuto);
    }

    Base& expr(const char *expr) {
        ecs_check(expr_count_ == 0, ECS_INVALID_OPERATION,
            "query_builder::expr() called more than once");
        desc_->expr = expr;
        expr_count_ ++;

    error:
        return *this;
    }

    /* With methods */

    template<typename T>
    Base& with() {
        this->term();
        *this->term_ = flecs::term(_::type<T>::id(this->world_v()));
        this->term_->inout = static_cast<ecs_inout_kind_t>(
            _::type_to_inout<T>());
            if (this->term_->inout == EcsInOutDefault) {
            this->inout_none();
        }
        return *this;
    }

    Base& with(id_t id) {
        this->term();
        *this->term_ = flecs::term(id);
        if (this->term_->inout == EcsInOutDefault) {
            this->inout_none();
        }
        return *this;
    }

    Base& with(const char *name) {
        this->term();
        *this->term_ = flecs::term().first(name);
        if (this->term_->inout == EcsInOutDefault) {
            this->inout_none();
        }
        return *this;
    }

    Base& with(const char *first, const char *second) {
        this->term();
        *this->term_ = flecs::term().first(first).second(second);
        if (this->term_->inout == EcsInOutDefault) {
            this->inout_none();
        }
        return *this;
    }

    Base& with(entity_t r, entity_t o) {
        this->term();
        *this->term_ = flecs::term(r, o);
        if (this->term_->inout == EcsInOutDefault) {
            this->inout_none();
        }
        return *this;
    }

    Base& with(entity_t r, const char *o) {
        this->term();
        *this->term_ = flecs::term(r).second(o);
        if (this->term_->inout == EcsInOutDefault) {
            this->inout_none();
        }
        return *this;
    }

    template<typename First>
    Base& with(id_t o) {
        return this->with(_::type<First>::id(this->world_v()), o);
    }

    template<typename First>
    Base& with(const char *second) {
        return this->with(_::type<First>::id(this->world_v())).second(second);
    }

    template<typename First, typename Second>
    Base& with() {
        return this->with<First>(_::type<Second>::id(this->world_v()));
    }

    template <typename E, if_t< is_enum<E>::value > = 0>
    Base& with(E value) {
        flecs::entity_t r = _::type<E>::id(this->world_v());
        auto o = enum_type<E>(this->world_v()).entity(value);
        return this->with(r, o);
    }

    Base& with(flecs::term& term) {
        this->term();
        *this->term_ = term;
        return *this;
    }

    Base& with(flecs::term&& term) {
        this->term();
        *this->term_ = term;
        return *this;
    }

    /* Without methods, shorthand for .with(...).not_(). */

    template <typename ... Args>
    Base& without(Args&&... args) {
        return this->with(FLECS_FWD(args)...).not_();
    }

    template <typename T, typename ... Args>
    Base& without(Args&&... args) {
        return this->with<T>(FLECS_FWD(args)...).not_();
    }

    template <typename First, typename Second>
    Base& without() {
        return this->with<First, Second>().not_();
    }

    /* Write/read methods */

    Base& write() {
        term_builder_i<Base>::write();
        return *this;
    }

    template <typename ... Args>
    Base& write(Args&&... args) {
        return this->with(FLECS_FWD(args)...).write();
    }

    template <typename T, typename ... Args>
    Base& write(Args&&... args) {
        return this->with<T>(FLECS_FWD(args)...).write();
    }

    template <typename First, typename Second>
    Base& write() {
        return this->with<First, Second>().write();
    }

    Base& read() {
        term_builder_i<Base>::read();
        return *this;
    }

    template <typename ... Args>
    Base& read(Args&&... args) {
        return this->with(FLECS_FWD(args)...).read();
    }

    template <typename T, typename ... Args>
    Base& read(Args&&... args) {
        return this->with<T>(FLECS_FWD(args)...).read();
    }

    template <typename First, typename Second>
    Base& read() {
        return this->with<First, Second>().read();
    }

    /* Scope_open/scope_close shorthand notation. */
    Base& scope_open() {
        return this->with(flecs::ScopeOpen).entity(0);
    }

    Base& scope_close() {
        return this->with(flecs::ScopeClose).entity(0);
    }

    /* Term notation for more complex query features */

    Base& term() {
        if (this->term_) {
            ecs_check(ecs_term_is_initialized(this->term_), 
                ECS_INVALID_OPERATION, 
                    "query_builder::term() called without initializing term");
        }

        ecs_check(term_index_ < FLECS_TERM_COUNT_MAX, 
            ECS_INVALID_PARAMETER, "maximum number of terms exceeded");

        this->set_term(&desc_->terms[term_index_]);

        term_index_ ++;
    
    error:
        return *this;
    }

    Base& term_at(int32_t term_index) {
        ecs_assert(term_index >= 0, ECS_INVALID_PARAMETER, NULL);
        int32_t prev_index = term_index_;
        term_index_ = term_index;
        this->term();
        term_index_ = prev_index;
        ecs_assert(ecs_term_is_initialized(this->term_), 
            ECS_INVALID_PARAMETER, NULL);
        return *this;
    }

    /** Sort the output of a query.
     * This enables sorting of entities across matched tables. As a result of this
     * operation, the order of entities in the matched tables may be changed. 
     * Resorting happens when a query iterator is obtained, and only if the table
     * data has changed.
     *
     * If multiple queries that match the same (down)set of tables specify different 
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
        return this->order_by(_::type<T>::id(this->world_v()), cmp);
    }

    /** Sort the output of a query.
     * Same as order_by<T>, but with component identifier.
     *
     * @param component The component used to sort.
     * @param compare The compare function used to sort the components.
     */    
    Base& order_by(flecs::entity_t component, int(*compare)(flecs::entity_t, const void*, flecs::entity_t, const void*)) {
        desc_->order_by_callback = reinterpret_cast<ecs_order_by_action_t>(compare);
        desc_->order_by = component;
        return *this;
    }

    /** Group and sort matched tables.
     * Similar to ecs_query_order_by(), but instead of sorting individual entities, this
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
     * @param group_by_action Callback that determines group id for table.
     */
    template <typename T>
    Base& group_by(uint64_t(*group_by_action)(flecs::world_t*, flecs::table_t *table, flecs::id_t id, void* ctx)) {
        ecs_group_by_action_t action = reinterpret_cast<ecs_group_by_action_t>(group_by_action);
        return this->group_by(_::type<T>::id(this->world_v()), action);
    }

    /** Group and sort matched tables.
     * Same as group_by<T>, but with component identifier.
     *
     * @param component The component used to determine the group rank.
     * @param group_by_action Callback that determines group id for table.
     */
    Base& group_by(flecs::entity_t component, uint64_t(*group_by_action)(flecs::world_t*, flecs::table_t *table, flecs::id_t id, void* ctx)) {
        desc_->group_by_callback = reinterpret_cast<ecs_group_by_action_t>(group_by_action);
        desc_->group_by = component;
        return *this;
    }

    /** Group and sort matched tables.
     * Same as group_by<T>, but with default group_by action.
     *
     * @tparam T The component used to determine the group rank.
     */
    template <typename T>
    Base& group_by() {
        return this->group_by(_::type<T>::id(this->world_v()), nullptr);
    }

    /** Group and sort matched tables.
     * Same as group_by, but with default group_by action.
     *
     * @param component The component used to determine the group rank.
     */
    Base& group_by(flecs::entity_t component) {
        return this->group_by(component, nullptr);
    }

    /** Specify context to be passed to group_by function.
     *
     * @param ctx Context to pass to group_by function.
     * @param ctx_free Function to cleanup context (called when query is deleted).
     */
    Base& group_by_ctx(void *ctx, ecs_ctx_free_t ctx_free = nullptr) {
        desc_->group_by_ctx = ctx;
        desc_->group_by_ctx_free = ctx_free;
        return *this;
    }

    /** Specify on_group_create action.
     */
    Base& on_group_create(ecs_group_create_action_t action) {
        desc_->on_group_create = action;
        return *this;
    }

    /** Specify on_group_delete action.
     */
    Base& on_group_delete(ecs_group_delete_action_t action) {
        desc_->on_group_delete = action;
        return *this;
    }

protected:
    virtual flecs::world_t* world_v() override = 0;
    int32_t term_index_;
    int32_t expr_count_;

private:
    operator Base&() {
        return *static_cast<Base*>(this);
    }

    ecs_query_desc_t *desc_;
};

}
