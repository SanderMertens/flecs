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
        if (!desc->filter.instanced) {
            desc->filter.instanced = instanced;
        }

        m_world = world;
        m_id = ecs_observer_init(world, desc);

        if (desc->filter.terms_buffer) {
            ecs_os_free(desc->filter.terms_buffer);
        }
    }

    void ctx(void *ctx) {
        ecs_observer_desc_t desc = {};
        desc.entity = m_id;
        desc.ctx = ctx;
        ecs_observer_init(m_world, &desc);
    }

    void* ctx() const {
        return ecs_get_observer_ctx(m_world, m_id);
    }

    flecs::filter<> query() const {
        const flecs::Poly *poly = this->get<flecs::Poly>(flecs::Observer);
        const ecs_observer_t *ob = static_cast<const flecs::observer_t*>(poly->poly);
        return flecs::filter<>(m_world, &ob->filter);
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
