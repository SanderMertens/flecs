#include "builder.hpp"

namespace flecs 
{

class trigger : public entity
{
public:
    explicit trigger() 
        : entity() { }

    explicit trigger(flecs::world_t *world, flecs::entity_t id)
        : entity(world, id) { }

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

// Builder implementation

template <typename ... Components>    
template <typename Func>
inline trigger trigger_builder<Components...>::iter(Func&& func) {
    using Invoker = typename _::iter_invoker<
        typename std::decay<Func>::type, Components...>;
    flecs::entity_t trigger = build<Invoker>(std::forward<Func>(func));
    return flecs::trigger(m_world, trigger);
}

template <typename ... Components>    
template <typename Func>
inline trigger trigger_builder<Components...>::each(Func&& func) {
    using Invoker = typename _::each_invoker<
        typename std::decay<Func>::type, Components...>;
    flecs::entity_t trigger = build<Invoker>(std::forward<Func>(func));
    return flecs::trigger(m_world, trigger);
}

} // namespace flecs
