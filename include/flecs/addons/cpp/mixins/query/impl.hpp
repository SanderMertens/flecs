#pragma once

#include "builder.hpp"

namespace flecs {

#define flecs_me_ this->me()

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

    query_base(world_t *world, ecs_query_desc_t *desc) 
        : m_world(world)
    {
        m_query = ecs_query_init(world, desc);

        if (!m_query) {
            ecs_abort(ECS_INVALID_PARAMETER, NULL);
        }

        if (desc->filter.terms_buffer) {
            ecs_os_free(desc->filter.terms_buffer);
        }
    }

    operator query_t*() const {
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

    operator query<>() const;

protected:
    world_t *m_world;
    query_t *m_query;
};

template<typename ... Components>
struct query final : query_base {
private:
    using Terms = typename _::term_ptrs<Components...>::array;

public:
    using query_base::query_base;

    template <typename Func>
    void each(Func&& func) const {
        iterate<_::each_invoker>(std::forward<Func>(func), 
            ecs_query_next_instanced);
    }

    template <typename Func>
    void iter(Func&& func) const { 
        iterate<_::iter_invoker>(std::forward<Func>(func), ecs_query_next);
    }

private:
    template < template<typename Func, typename ... Comps> class Invoker, typename Func, typename NextFunc, typename ... Args>
    void iterate(Func&& func, NextFunc next, Args &&... args) const {
        ecs_iter_t it = ecs_query_iter(m_world, m_query);
        it.is_instanced |= Invoker<Func, Components...>::instanced();

        while (next(&it, std::forward<Args>(args)...)) {
            Invoker<Func, Components...>(func).invoke(&it);
        }
    }
};

// Mixin implementation
template <typename... Comps, typename... Args>
inline flecs::query<Comps...> query_m_world::query(Args &&... args) const {
    return flecs::query_builder<Comps...>(flecs_me_, std::forward<Args>(args)...)
        .build();
}

template <typename... Comps, typename... Args>
inline flecs::query_builder<Comps...> query_m_world::query_builder(Args &&... args) const {
    return flecs::query_builder<Comps...>(flecs_me_, std::forward<Args>(args)...);
}

// Builder implementation
template <typename Base, typename ... Components>
inline Base& query_builder_i<Base, Components ...>::parent(const query_base& parent) {
    m_desc->parent = parent;
    return *static_cast<Base*>(this);
}

// query_base implementation
inline query_base::operator query<>() const {
    return flecs::query<>(m_world, m_query);
}

#undef flecs_me_

} // namespace flecs
