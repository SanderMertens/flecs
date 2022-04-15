#pragma once

#include "builder.hpp"

namespace flecs 
{

struct observer final : entity
{
    using entity::entity;

    explicit observer() : entity() { }

    observer(flecs::world_t *world, ecs_observer_desc_t *desc) 
        : entity(world, ecs_observer_init(world, desc)) 
    { 
        if (desc->filter.terms_buffer) {
            ecs_os_free(desc->filter.terms_buffer);
        }
    }

    void ctx(void *ctx) {
        ecs_observer_desc_t desc = {};
        desc.entity.entity = m_id;
        desc.ctx = ctx;
        ecs_observer_init(m_world, &desc);
    }

    void* ctx() const {
        return ecs_get_observer_ctx(m_world, m_id);
    }
};

// Mixin implementation
inline observer world::observer(flecs::entity e) const {
    return flecs::observer(m_world, e);
}

template <typename... Comps, typename... Args>
inline observer_builder<Comps...> world::observer(Args &&... args) const {
    return flecs::observer_builder<Comps...>(m_world, FLECS_FWD(args)...);
}

} // namespace flecs
