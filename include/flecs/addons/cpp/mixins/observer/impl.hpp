/**
 * @file addons/cpp/mixins/observer/impl.hpp
 * @brief Observer implementation.
 */

#pragma once

#include "builder.hpp"

namespace flecs 
{

struct observer final : entity
{
    using entity::entity;

    explicit observer() : entity() { }

    observer(flecs::world_t *world, ecs_observer_desc_t *desc, bool instanced) 
    {
        if (!(desc->query.flags & EcsQueryIsInstanced)) {
            ECS_BIT_COND(desc->query.flags, EcsQueryIsInstanced, instanced);
        }

        m_world = world;
        m_id = ecs_observer_init(world, desc);
    }

    void ctx(void *ctx) {
        ecs_observer_desc_t desc = {};
        desc.entity = m_id;
        desc.ctx = ctx;
        ecs_observer_init(m_world, &desc);
    }

    void* ctx() const {
        return ecs_observer_get_ctx(m_world, m_id);
    }

    flecs::query<> query() const {
        return flecs::query<>(ecs_observer_get_query(m_world, m_id));
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
