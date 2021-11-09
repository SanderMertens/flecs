#pragma once

#include "query_i.hpp"

namespace flecs 
{

template<typename ... Components>
class query_builder_base
    : public query_builder_i<query_builder_base<Components...>, Components ...>
{
public:
    query_builder_base(flecs::world_t *world) 
        : query_builder_i<query_builder_base<Components...>, Components ...>(&m_desc)
        , m_desc({})
        , m_world(world)
    { 
        this->populate_filter_from_pack();
    }

    query_builder_base(const query_builder_base& f) 
        : query_builder_i<query_builder_base<Components...>, Components ...>(&m_desc, f.m_term_index)
    {
        m_world = f.m_world;
        m_desc = f.m_desc;
    }

    query_builder_base(query_builder_base&& f) 
        : query_builder_i<query_builder_base<Components...>, Components ...>(&m_desc, f.m_term_index)
    {
        m_world = f.m_world;
        m_desc = f.m_desc;
    }

    operator query<Components ...>() const;

    operator ecs_query_t*() const {
        ecs_query_t *result = ecs_query_init(this->m_world, &this->m_desc);

        if (!result) {
            ecs_abort(ECS_INVALID_PARAMETER, NULL);
        }

        if (this->m_desc.filter.terms_buffer) {
            ecs_os_free(m_desc.filter.terms_buffer);
        }
        
        return result;
    }    

    query<Components ...> build() const;

    ecs_query_desc_t m_desc;

    flecs::world_t* world_v() override { return m_world; }

protected:
    flecs::world_t *m_world;
};

template<typename ... Components>
class query_builder final : public query_builder_base<Components...> {
public:
    query_builder(flecs::world_t *world)
        : query_builder_base<Components ...>(world) { }

    operator query<>() const;
};

}
