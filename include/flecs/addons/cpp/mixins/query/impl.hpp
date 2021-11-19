#pragma once

#include "builder.hpp"

namespace flecs {

#define me_ this->me()

////////////////////////////////////////////////////////////////////////////////
//// Persistent queries
////////////////////////////////////////////////////////////////////////////////

struct query_base {
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
        ecs_query_fini(m_query);
        m_world = nullptr;
        m_query = nullptr;
    }

    template <typename Func>
    void iter(Func&& func) const {
        ecs_iter_t it = ecs_query_iter(m_world, m_query);
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

    filter_base filter() {
        return filter_base(m_world, ecs_query_get_filter(m_query));
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
struct query : query_base {
private:
    using Terms = typename _::term_ptrs<Components...>::array;

public:
    query() { }

    query(world_t *world, query_t *q)
        : query_base(world, q) { }

    template <typename Func>
    void each(Func&& func) const {
        iterate<_::each_invoker>(true, std::forward<Func>(func), 
            ecs_query_next_instanced);
    } 

    template <typename Func>
    void each_worker(int32_t stage_current, int32_t stage_count, Func&& func) const {
        iterate<_::each_invoker>(true, std::forward<Func>(func), 
            ecs_query_next_worker, stage_current, stage_count);
    }

    template <typename Func>
    void iter(Func&& func) const { 
        iterate<_::iter_invoker>(false, std::forward<Func>(func), ecs_query_next);
    }

    template <typename Func>
    void iter_worker(int32_t stage_current, int32_t stage_count, Func&& func) const {
        iterate<_::iter_invoker>(false, std::forward<Func>(func), 
            ecs_query_next_worker, stage_current, stage_count);
    }

private:
    template < template<typename Func, typename ... Comps> class Invoker, typename Func, typename NextFunc, typename ... Args>
    void iterate(bool force_instanced, Func&& func, NextFunc next, Args &&... args) const {
        ecs_iter_t it = ecs_query_iter(m_world, m_query);
        if (force_instanced) {
            it.is_instanced = true;
        }
        while (next(&it, std::forward<Args>(args)...)) {
            Invoker<Func, Components...>(func).invoke(&it);
        }
    }
};

// Mixin implementation
template <typename... Comps, typename... Args>
inline flecs::query<Comps...> query_m_world::query(Args &&... args) const {
    return flecs::query_builder<Comps...>(me_, std::forward<Args>(args)...)
        .build();
}

template <typename... Comps, typename... Args>
inline flecs::query_builder<Comps...> query_m_world::query_builder(Args &&... args) const {
    return flecs::query_builder<Comps...>(me_, std::forward<Args>(args)...);
}

// Builder implementation
template <typename ... Components>
inline query_builder_base<Components...>::operator query<Components ...>() const {
    ecs_query_t *query = *this;
    return flecs::query<Components...>(m_world, query);
}

template <typename ... Components>
inline query_builder<Components ...>::operator query<>() const {
    ecs_query_t *query = *this;
    return flecs::query<>(this->m_world, query);
}

template <typename ... Components>
inline query<Components ...> query_builder_base<Components...>::build() const {
    ecs_query_t *query = *this;
    return flecs::query<Components...>(m_world, query);
}

template <typename Base, typename ... Components>
inline Base& query_builder_i<Base, Components ...>::parent(const query_base& parent) {
    m_desc->parent = parent.c_ptr();
    return *static_cast<Base*>(this);
}

#undef me_

} // namespace flecs
