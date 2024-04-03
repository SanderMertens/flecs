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
        : m_term_index(term_index)
        , m_expr_count(0)
        , m_desc(desc) { }

    Base& instanced() {
        m_desc->flags |= EcsQueryIsInstanced;
        return *this;
    }

    Base& flags(ecs_flags32_t flags) {
        m_desc->flags |= flags;
        return *this;
    }

    Base& cache_kind(query_cache_kind_t kind) {
        m_desc->cache_kind = static_cast<ecs_query_cache_kind_t>(kind);
        return *this;
    }

    Base& cached() {
        return cache_kind(flecs::QueryCacheAuto);
    }

    Base& expr(const char *expr) {
        ecs_check(m_expr_count == 0, ECS_INVALID_OPERATION,
            "query_builder::expr() called more than once");
        m_desc->expr = expr;
        m_expr_count ++;

    error:
        return *this;
    }

    /* With/without shorthand notation. */

    template <typename ... Args>
    Base& with(Args&&... args) {
        return this->term(FLECS_FWD(args)...).inout_none();
    }

    template <typename T, typename ... Args>
    Base& with(Args&&... args) {
        return this->term<T>(FLECS_FWD(args)...).inout_none();
    }

    template <typename First, typename Second>
    Base& with() {
        return this->term<First, Second>().inout_none();
    }

    template <typename ... Args>
    Base& without(Args&&... args) {
        return this->term(FLECS_FWD(args)...).not_();
    }

    template <typename T, typename ... Args>
    Base& without(Args&&... args) {
        return this->term<T>(FLECS_FWD(args)...).not_();
    }

    template <typename First, typename Second>
    Base& without() {
        return this->term<First, Second>().not_();
    }

    /* Write/read shorthand notation */

    Base& write() {
        term_builder_i<Base>::write();
        return *this;
    }

    template <typename ... Args>
    Base& write(Args&&... args) {
        return this->term(FLECS_FWD(args)...).write();
    }

    template <typename T, typename ... Args>
    Base& write(Args&&... args) {
        return this->term<T>(FLECS_FWD(args)...).write();
    }

    template <typename First, typename Second>
    Base& write() {
        return this->term<First, Second>().write();
    }

    Base& read() {
        term_builder_i<Base>::read();
        return *this;
    }

    template <typename ... Args>
    Base& read(Args&&... args) {
        return this->term(FLECS_FWD(args)...).read();
    }

    template <typename T, typename ... Args>
    Base& read(Args&&... args) {
        return this->term<T>(FLECS_FWD(args)...).read();
    }

    template <typename First, typename Second>
    Base& read() {
        return this->term<First, Second>().read();
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
        if (this->m_term) {
            ecs_check(ecs_term_is_initialized(this->m_term), 
                ECS_INVALID_OPERATION, 
                    "query_builder::term() called without initializing term");
        }

        ecs_check(m_term_index < FLECS_TERM_COUNT_MAX, 
            ECS_INVALID_PARAMETER, "maximum number of terms exceeded");

        this->set_term(&m_desc->terms[m_term_index]);

        m_term_index ++;
    
    error:
        return *this;
    }

    Base& term_at(int32_t term_index) {
        ecs_assert(term_index > 0, ECS_INVALID_PARAMETER, NULL);
        int32_t prev_index = m_term_index;
        m_term_index = term_index - 1;
        this->term();
        m_term_index = prev_index;
        ecs_assert(ecs_term_is_initialized(this->m_term), 
            ECS_INVALID_PARAMETER, NULL);
        return *this;
    }

    template<typename T>
    Base& term() {
        this->term();
        *this->m_term = flecs::term(_::type<T>::id(this->world_v()));
        this->m_term->inout = static_cast<ecs_inout_kind_t>(
            _::type_to_inout<T>());
        return *this;
    }

    Base& term(id_t id) {
        this->term();
        *this->m_term = flecs::term(id);
        return *this;
    }

    Base& term(const char *name) {
        this->term();
        *this->m_term = flecs::term().first(name);
        return *this;
    }

    Base& term(const char *first, const char *second) {
        this->term();
        *this->m_term = flecs::term().first(first).second(second);
        return *this;
    }

    Base& term(entity_t r, entity_t o) {
        this->term();
        *this->m_term = flecs::term(r, o);
        return *this;
    }

    Base& term(entity_t r, const char *o) {
        this->term();
        *this->m_term = flecs::term(r).second(o);
        return *this;
    }

    template<typename First>
    Base& term(id_t o) {
        return this->term(_::type<First>::id(this->world_v()), o);
    }

    template<typename First>
    Base& term(const char *second) {
        return this->term(_::type<First>::id(this->world_v())).second(second);
    }

    template<typename First, typename Second>
    Base& term() {
        return this->term<First>(_::type<Second>::id(this->world_v()));
    }

    template <typename E, if_t< is_enum<E>::value > = 0>
    Base& term(E value) {
        flecs::entity_t r = _::type<E>::id(this->world_v());
        auto o = enum_type<E>(this->world_v()).entity(value);
        return this->term(r, o);
    }

    Base& term(flecs::term& term) {
        this->term();
        *this->m_term = term;
        return *this;
    }

    Base& term(flecs::term&& term) {
        this->term();
        *this->m_term = term;
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
        m_desc->order_by_callback = reinterpret_cast<ecs_order_by_action_t>(compare);
        m_desc->order_by = component;
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
        m_desc->group_by_callback = reinterpret_cast<ecs_group_by_action_t>(group_by_action);
        m_desc->group_by = component;
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
        m_desc->group_by_ctx = ctx;
        m_desc->group_by_ctx_free = ctx_free;
        return *this;
    }

    /** Specify on_group_create action.
     */
    Base& on_group_create(ecs_group_create_action_t action) {
        m_desc->on_group_create = action;
        return *this;
    }

    /** Specify on_group_delete action.
     */
    Base& on_group_delete(ecs_group_delete_action_t action) {
        m_desc->on_group_delete = action;
        return *this;
    }

protected:
    virtual flecs::world_t* world_v() = 0;
    int32_t m_term_index;
    int32_t m_expr_count;

private:
    operator Base&() {
        return *static_cast<Base*>(this);
    }

    ecs_query_desc_t *m_desc;
};

}
