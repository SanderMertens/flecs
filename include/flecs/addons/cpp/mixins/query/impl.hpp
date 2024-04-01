/**
 * @file addons/cpp/mixins/query/impl.hpp
 * @brief Query implementation.
 */

#pragma once

#include "builder.hpp"

namespace flecs 
{

struct query_base {
    query_base() { }

    query_base(world_t *world, query_t *q)
        : m_query(q) { 
            ecs_poly_claim(q);
        }

    query_base(world_t *world, const query_t *q)
        : m_query(ECS_CONST_CAST(query_t*, q)) { 
            ecs_poly_claim(q);
        }

    query_base(world_t *world, ecs_query_desc_t *desc) {
        m_query = ecs_query_init(world, desc);
    }

    query_base(const query_base& obj) {
        this->m_query = obj.m_query;
        ecs_poly_claim(this->m_query);
    }

    query_base& operator=(const query_base& obj) {
        this->m_query = obj.m_query;
        ecs_poly_claim(this->m_query);
        return *this; 
    }

    query_base(query_base&& obj) noexcept {
        this->m_query = obj.m_query;
        obj.m_query = nullptr;
    }

    query_base& operator=(query_base&& obj) noexcept {
        this->m_query = obj.m_query;
        obj.m_query = nullptr;
        return *this; 
    }

    flecs::entity entity() {
        return flecs::entity(m_query->world, m_query->entity);
    }

    operator const flecs::query_t*() const {
        return m_query;
    }

    operator bool() const {
        return m_query != nullptr;
    }

    /** Free the query.
     */
    ~query_base() {
        if (m_query) {
            if (!ecs_poly_release(m_query)) {
                ecs_query_fini(m_query);
            }
        }
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
    bool changed() const {
        return ecs_query_changed(m_query);
    }

    /** Get info for group. 
     * 
     * @param group_id The group id for which to retrieve the info.
     * @return The group info.
     */
    const flecs::query_group_info_t* group_info(uint64_t group_id) const {
        return ecs_query_get_group_info(m_query, group_id);
    }

    /** Get context for group. 
     * 
     * @param group_id The group id for which to retrieve the context.
     * @return The group context.
     */
    void* group_ctx(uint64_t group_id) const {
        const flecs::query_group_info_t *gi = group_info(group_id);
        if (gi) {
            return gi->ctx;
        } else {
            return NULL;
        }
    }

    template <typename Func>
    void each_term(const Func& func) {
        for (int i = 0; i < m_query->term_count; i ++) {
            flecs::term t(m_query->world, m_query->terms[i]);
            func(t);
            t.reset(); // prevent freeing resources
        }
    }

    flecs::term term(int32_t index) {
        return flecs::term(m_query->world, m_query->terms[index]);
    }

    int32_t term_count() {
        return m_query->term_count;
    }

    int32_t field_count() {
        return m_query->field_count;
    }

    int32_t find_var(const char *name) {
        return ecs_query_find_var(m_query, name);
    }

    flecs::string str() {
        char *result = ecs_query_str(m_query);
        return flecs::string(result);
    }

    /** Returns a string representing the query plan.
     * This can be used to analyze the behavior & performance of the query.
     * @see ecs_query_plan
     */
    flecs::string plan() const {
        char *result = ecs_query_plan(m_query);
        return flecs::string(result);
    }

    operator query<>() const;

protected:
    query_t *m_query = nullptr;
};

template<typename ... Components>
struct query : query_base, iterable<Components...> {
private:
    using Terms = typename _::term_ptrs<Components...>::array;

public:
    using query_base::query_base;

    query() : query_base() { } // necessary not to confuse msvc

    query(const query& obj) : query_base(obj) { }

    query& operator=(const query& obj) {
        query_base::operator=(obj);
        return *this;
    }

    query(query&& obj) noexcept : query_base(FLECS_MOV(obj)) { }

    query& operator=(query&& obj) noexcept {
        query_base::operator=(FLECS_FWD(obj));
        return *this;
    }

private:
    ecs_iter_t get_iter(flecs::world_t *world) const override {
        if (!world) {
            world = m_query->world;
        }
        return ecs_query_iter(world, m_query);
    }

    ecs_iter_next_action_t next_action() const override {
        return ecs_query_next;
    }

    ecs_iter_next_action_t next_each_action() const override {
        return ecs_query_next_instanced;
    }
};

// World mixin implementation
template <typename... Comps, typename... Args>
inline flecs::query<Comps...> world::query(Args &&... args) const {
    return flecs::query_builder<Comps...>(m_world, FLECS_FWD(args)...)
        .build();
}

template <typename... Comps, typename... Args>
inline flecs::query_builder<Comps...> world::query_builder(Args &&... args) const {
    return flecs::query_builder<Comps...>(m_world, FLECS_FWD(args)...);
}

// world::each
namespace _ {

// Each with entity parameter
template<typename Func, typename ... Args>
struct query_delegate_w_ent;

template<typename Func, typename E, typename ... Args>
struct query_delegate_w_ent<Func, arg_list<E, Args ...> >
{
    query_delegate_w_ent(const flecs::world& world, Func&& func) {
        auto f = world.query<Args ...>();
        f.each(FLECS_MOV(func));
    }
};

// Each without entity parameter
template<typename Func, typename ... Args>
struct query_delegate_no_ent;

template<typename Func, typename ... Args>
struct query_delegate_no_ent<Func, arg_list<Args ...> >
{
    query_delegate_no_ent(const flecs::world& world, Func&& func) {
        auto f = world.query<Args ...>();
        f.each(FLECS_MOV(func));
    }
};

// Switch between function with & without entity parameter
template<typename Func, typename T = int>
struct query_delegate;

template <typename Func>
struct query_delegate<Func, if_t<is_same<first_arg_t<Func>, flecs::entity>::value> > {
    query_delegate(const flecs::world& world, Func&& func) {
        query_delegate_w_ent<Func, arg_list_t<Func>>(world, FLECS_MOV(func));
    }
};

template <typename Func>
struct query_delegate<Func, if_not_t<is_same<first_arg_t<Func>, flecs::entity>::value> > {
    query_delegate(const flecs::world& world, Func&& func) {
        query_delegate_no_ent<Func, arg_list_t<Func>>(world, FLECS_MOV(func));
    }
};

}

template <typename Func>
inline void world::each(Func&& func) const {
    _::query_delegate<Func> f_delegate(*this, FLECS_MOV(func));
}

template <typename T, typename Func>
inline void world::each(Func&& func) const {
    ecs_iter_t it = ecs_each_id(m_world, _::cpp_type<T>::id());

    while (ecs_each_next(&it)) {
        _::each_delegate<Func, T>(func).invoke(&it);
    }
}

template <typename Func>
inline void world::each(flecs::id_t each_id, Func&& func) const {
    ecs_iter_t it = ecs_each_id(m_world, each_id);

    while (ecs_each_next(&it)) {
        _::each_delegate<Func>(func).invoke(&it);
    }
}

// query_base implementation
inline query_base::operator flecs::query<> () const {
    return flecs::query<>(m_query->world, m_query);
}

}
