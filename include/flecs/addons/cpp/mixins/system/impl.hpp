#pragma once

#include "builder.hpp"

namespace flecs 
{

class system_runner_fluent {
public:
    system_runner_fluent(
        world_t *world, 
        entity_t id, 
        int32_t stage_current, 
        int32_t stage_count, 
        FLECS_FLOAT delta_time, 
        void *param)
        : m_stage(world)
        , m_id(id)
        , m_delta_time(delta_time)
        , m_param(param)
        , m_offset(0)
        , m_limit(0)
        , m_stage_current(stage_current)
        , m_stage_count(stage_count) { }

    system_runner_fluent& offset(int32_t offset) {
        m_offset = offset;
        return *this;
    }

    system_runner_fluent& limit(int32_t limit) {
        m_limit = limit;
        return *this;
    }

    system_runner_fluent& stage(flecs::world& stage) {
        m_stage = stage.c_ptr();
        return *this;
    }

    ~system_runner_fluent() {
        if (m_stage_count) {
            ecs_run_worker(
                m_stage, m_id, m_stage_current, m_stage_count, m_delta_time,
                m_param);            
        } else {
            ecs_run_w_filter(
                m_stage, m_id, m_delta_time, m_offset, m_limit, m_param);
        }
    }

private:
    world_t *m_stage;
    entity_t m_id;
    FLECS_FLOAT m_delta_time;
    void *m_param;
    int32_t m_offset;
    int32_t m_limit;
    int32_t m_stage_current;
    int32_t m_stage_count;
};

struct system : entity, extendable<system, Mixins>
{
    explicit system() 
        : entity() { }

    explicit system(flecs::world_t *world, flecs::entity_t id)
        : entity(world, id) { }

    void enable() {
        ecs_enable(m_world, m_id, true);
    }

    void disable() {
        ecs_enable(m_world, m_id, false);
    }

    void ctx(void *ctx) {
        if (ecs_has(m_world, m_id, EcsSystem)) {
            ecs_system_desc_t desc = {};
            desc.entity.entity = m_id;
            desc.ctx = ctx;
            ecs_system_init(m_world, &desc);
        } else {
            ecs_trigger_desc_t desc = {};
            desc.entity.entity = m_id;
            desc.ctx = ctx;
            ecs_trigger_init(m_world, &desc);
        }
    }

    void* ctx() const {
        if (ecs_has(m_world, m_id, EcsSystem)) {
            return ecs_get_system_ctx(m_world, m_id);
        } else {
            return ecs_get_trigger_ctx(m_world, m_id);
        }
    }

    query_base query() const {
        return query_base(m_world, ecs_system_get_query(m_world, m_id));
    }

    system_runner_fluent run(FLECS_FLOAT delta_time = 0.0f, void *param = nullptr) const {
        return system_runner_fluent(m_world, m_id, 0, 0, delta_time, param);
    }

    system_runner_fluent run_worker(
        int32_t stage_current, 
        int32_t stage_count, 
        FLECS_FLOAT delta_time = 0.0f, 
        void *param = nullptr) const 
    {
        return system_runner_fluent(
            m_world, m_id, stage_current, stage_count, delta_time, param);
    }
};

// Mixin implementation
inline void system_m_world::init() {
    this->me().template component<TickSource>("flecs::system::TickSource");
}

inline system system_m_world::system(flecs::entity e) const {
    return flecs::system(this->me().m_world, e);
}

template <typename... Comps, typename... Args>
inline system_builder<Comps...> system_m_world::system(Args &&... args) const {
    return flecs::system_builder<Comps...>(this->me(), std::forward<Args>(args)...);
}

// Builder implementation
template <typename ... Components>    
template <typename Func>
inline system system_builder<Components...>::iter(Func&& func) const {
    using Invoker = typename _::iter_invoker<
        typename std::decay<Func>::type, Components...>;
    flecs::entity_t system = build<Invoker>(std::forward<Func>(func), false);
    return flecs::system(m_world, system);
}

template <typename ... Components>    
template <typename Func>
inline system system_builder<Components...>::each(Func&& func) const {
    using Invoker = typename _::each_invoker<
        typename std::decay<Func>::type, Components...>;
    flecs::entity_t system = build<Invoker>(std::forward<Func>(func), true);
    return flecs::system(m_world, system);
}

} // namespace flecs
