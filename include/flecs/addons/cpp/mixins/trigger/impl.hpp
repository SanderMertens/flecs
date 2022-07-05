#pragma once

#include "builder.hpp"

namespace flecs 
{

struct trigger final : entity
{
    using entity::entity;

    explicit trigger() : entity() { }

    trigger(flecs::world_t *world, ecs_observer_desc_t *desc) 
        : entity(world, ecs_observer_init(world, desc)) 
    { 
        ecs_term_fini(&desc->term);
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
inline trigger world::trigger(flecs::entity e) const {
    return flecs::trigger(m_world, e);
}

template <typename... Comps, typename... Args>
inline trigger_builder<Comps...> world::trigger(Args &&... args) const {
    return flecs::trigger_builder<Comps...>(m_world, FLECS_FWD(args)...);
}

} // namespace flecs
