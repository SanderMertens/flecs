#pragma once

#include "builder.hpp"

namespace flecs 
{

struct observer final : public entity_base
{
    explicit observer() 
        : entity_base() { }

    explicit observer(flecs::world_t *world, flecs::entity_t id)
        : entity_base(world, id) { }

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

// Builder implementation

template <typename ... Components>    
template <typename Func>
inline observer observer_builder<Components...>::iter(Func&& func) const {
    using Invoker = typename _::iter_invoker<
        typename std::decay<Func>::type, Components...>;
    flecs::entity_t observer = build<Invoker>(std::forward<Func>(func), false);
    return flecs::observer(m_world, observer);
}

template <typename ... Components>    
template <typename Func>
inline observer observer_builder<Components...>::each(Func&& func) const {
    using Invoker = typename _::each_invoker<
        typename std::decay<Func>::type, Components...>;
    flecs::entity_t observer = build<Invoker>(std::forward<Func>(func), true);
    return flecs::observer(m_world, observer);
}

} // namespace flecs
