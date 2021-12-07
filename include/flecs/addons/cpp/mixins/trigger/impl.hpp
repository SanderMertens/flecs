#pragma once

#include "builder.hpp"

namespace flecs 
{

struct trigger final : public entity_base
{
    using entity_base::entity_base;

    trigger(flecs::world_t *world, ecs_trigger_desc_t *desc) 
        : entity_base(world, ecs_trigger_init(world, desc)) 
    { 
        ecs_term_fini(&desc->term);
    }
    
    void ctx(void *ctx) {
        ecs_trigger_desc_t desc = {};
        desc.entity.entity = m_id;
        desc.ctx = ctx;
        ecs_trigger_init(m_world, &desc);
    }

    void* ctx() const {
        return ecs_get_trigger_ctx(m_world, m_id);
    }
};

// Mixin implementation

inline void trigger_m_world::init() {
    this->me().template component<Trigger>("flecs::core::Trigger");
}

template <typename... Comps, typename... Args>
inline trigger_builder<Comps...> trigger_m_world::trigger(Args &&... args) const {
    return flecs::trigger_builder<Comps...>(this->me(), std::forward<Args>(args)...);
}

} // namespace flecs
