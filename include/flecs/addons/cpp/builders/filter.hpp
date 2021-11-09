#pragma once

#include "filter_i.hpp"

namespace flecs 
{

template<typename ... Components>
class filter_builder_base
    : public filter_builder_i<filter_builder_base<Components...>, Components ...>
{
public:
    filter_builder_base(flecs::world_t *world) 
        : filter_builder_i<filter_builder_base<Components...>, Components ...>(&m_desc)
        , m_desc({})
        , m_world(world)
    { 
        this->populate_filter_from_pack();
    }

    filter_builder_base(const filter_builder_base& f) 
        : filter_builder_i<filter_builder_base<Components...>, Components ...>(&m_desc, f.m_term_index)
    {
        m_world = f.m_world;
        m_desc = f.m_desc;
    }

    filter_builder_base(filter_builder_base&& f) 
        : filter_builder_i<filter_builder_base<Components...>, Components ...>(&m_desc, f.m_term_index)
    {
        m_world = f.m_world;
        m_desc = f.m_desc;
    }

    operator filter<Components ...>() const;

    operator ecs_filter_t() const {
        ecs_filter_t f;

        int res = ecs_filter_init(this->m_world, &f, &this->m_desc);
        if (res != 0) {
            ecs_abort(ECS_INVALID_PARAMETER, NULL);
        }

        if (this->m_desc.terms_buffer) {
            ecs_os_free(this->m_desc.terms_buffer);
        }

        return f;
    }

    filter<Components ...> build() const;

    ecs_filter_desc_t m_desc;

    flecs::world_t* world_v() override { return m_world; }

protected:
    flecs::world_t *m_world;
};

template<typename ... Components>
class filter_builder final : public filter_builder_base<Components...> {
public:
    filter_builder(flecs::world_t *world)
        : filter_builder_base<Components ...>(world) { }

    operator filter<>() const;
};

}
