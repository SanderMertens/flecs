#pragma once

#include "builder.hpp"

namespace flecs 
{

struct observer final : public entity_base
{
    using entity_base::entity_base;

    observer(flecs::world_t *world, ecs_observer_desc_t *desc) 
        : entity_base(world, ecs_observer_init(world, desc)) 
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

inline void observer_m_world::init() {
    this->me().template component<Observer>("flecs::core::Observer");
}

template <typename... Comps, typename... Args>
inline observer_builder<Comps...> observer_m_world::observer(Args &&... args) const {
    return flecs::observer_builder<Comps...>(this->me(), std::forward<Args>(args)...);
}

} // namespace flecs
