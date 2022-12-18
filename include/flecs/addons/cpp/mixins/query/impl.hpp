/**
 * @file addons/cpp/mixins/query/impl.hpp
 * @brief Query implementation.
 */

#pragma once

#include "builder.hpp"

namespace flecs {

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
    bool changed() const {
        return ecs_query_changed(m_query, 0);
    }

    /** Returns whether query is orphaned.
     * When the parent query of a subquery is deleted, it is left in an orphaned
     * state. The only valid operation on an orphaned query is deleting it. Only
     * subqueries can be orphaned.
     *
     * @return true if query is orphaned, otherwise false.
     */
    bool orphaned() const {
        return ecs_query_orphaned(m_query);
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

    /** Free the query.
     */
    void destruct() {
        ecs_query_fini(m_query);
        m_world = nullptr;
        m_query = nullptr;
    }

    template <typename Func>
    void each_term(const Func& func) const {
        const ecs_filter_t *f = ecs_query_get_filter(m_query);
        ecs_assert(f != NULL, ECS_INVALID_PARAMETER, NULL);

        for (int i = 0; i < f->term_count; i ++) {
            flecs::term t(m_world, f->terms[i]);
            func(t);
        }
    }

    filter_base filter() const {
        return filter_base(m_world, ecs_query_get_filter(m_query));
    }

    flecs::term term(int32_t index) const {
        const ecs_filter_t *f = ecs_query_get_filter(m_query);
        ecs_assert(f != NULL, ECS_INVALID_PARAMETER, NULL);
        return flecs::term(m_world, f->terms[index]);
    }

    int32_t field_count() const {
        const ecs_filter_t *f = ecs_query_get_filter(m_query);
        return f->term_count;   
    }

    flecs::string str() const {
        const ecs_filter_t *f = ecs_query_get_filter(m_query);
        char *result = ecs_filter_str(m_world, f);
        return flecs::string(result);
    }

    flecs::entity entity() const {
        return flecs::entity(m_world, ecs_get_entity(m_query));
    }
    
    operator query<>() const;

protected:
    world_t *m_world;
    query_t *m_query;
};

template<typename ... Components>
struct query final : query_base, iterable<Components...> {
public:
    flecs::world world() const {
        return flecs::world(m_world);
    }
    
private:
    using Terms = typename _::term_ptrs<Components...>::array;

    ecs_iter_t get_iter(flecs::world_t *world) const override {
        if (!world) {
            world = m_world;
        }
        return ecs_query_iter(world, m_query);
    }

    ecs_iter_next_action_t next_action() const override {
        return ecs_query_next;
    }

    ecs_iter_next_action_t next_each_action() const override {
        return ecs_query_next_instanced;
    }

public:
    using query_base::query_base;
};

// Mixin implementation
template <typename... Comps, typename... Args>
inline flecs::query<Comps...> world::query(Args &&... args) const {
    return flecs::query_builder<Comps...>(m_world, FLECS_FWD(args)...)
        .build();
}

template <typename... Comps, typename... Args>
inline flecs::query_builder<Comps...> world::query_builder(Args &&... args) const {
    return flecs::query_builder<Comps...>(m_world, FLECS_FWD(args)...);
}

// Builder implementation
template <typename Base, typename ... Components>
inline Base& query_builder_i<Base, Components ...>::observable(const query_base& parent) {
    m_desc->parent = parent;
    return *static_cast<Base*>(this);
}

// query_base implementation
inline query_base::operator query<>() const {
    return flecs::query<>(m_world, m_query);
}

} // namespace flecs
