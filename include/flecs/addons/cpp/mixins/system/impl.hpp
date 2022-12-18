/**
 * @file addons/cpp/mixins/system/impl.hpp
 * @brief System module implementation.
 */

#pragma once

#include "builder.hpp"

namespace flecs 
{

struct system_runner_fluent {
    system_runner_fluent(
        world_t *world, 
        entity_t id, 
        int32_t stage_current, 
        int32_t stage_count, 
        ecs_ftime_t delta_time, 
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
    ecs_ftime_t m_delta_time;
    void *m_param;
    int32_t m_offset;
    int32_t m_limit;
    int32_t m_stage_current;
    int32_t m_stage_count;
};

struct system final : entity
{
    using entity::entity;

    explicit system() {
        m_id = 0;
        m_world = nullptr;
    }

    explicit system(flecs::world_t *world, ecs_system_desc_t *desc, bool instanced) 
    {
        if (!desc->query.filter.instanced) {
            desc->query.filter.instanced = instanced;
        }

        m_world = world;
        m_id = ecs_system_init(world, desc);

        if (desc->query.filter.terms_buffer) {
            ecs_os_free(desc->query.filter.terms_buffer);
        }
    }

    void ctx(void *ctx) {
        ecs_system_desc_t desc = {};
        desc.entity = m_id;
        desc.ctx = ctx;
        ecs_system_init(m_world, &desc);
    }

    void* ctx() const {
        return ecs_get_system_ctx(m_world, m_id);
    }

    flecs::query<> query() const {
        return flecs::query<>(m_world, ecs_system_get_query(m_world, m_id));
    }

    system_runner_fluent run(ecs_ftime_t delta_time = 0.0f, void *param = nullptr) const {
        return system_runner_fluent(m_world, m_id, 0, 0, delta_time, param);
    }

    system_runner_fluent run_worker(
        int32_t stage_current, 
        int32_t stage_count, 
        ecs_ftime_t delta_time = 0.0f, 
        void *param = nullptr) const 
    {
        return system_runner_fluent(
            m_world, m_id, stage_current, stage_count, delta_time, param);
    }

#   ifdef FLECS_TIMER
#   include "../timer/system_mixin.inl"
#   endif

};

// Mixin implementation
inline system world::system(flecs::entity e) const {
    return flecs::system(m_world, e);
}

template <typename... Comps, typename... Args>
inline system_builder<Comps...> world::system(Args &&... args) const {
    return flecs::system_builder<Comps...>(m_world, FLECS_FWD(args)...);
}

namespace _ {

inline void system_init(flecs::world& world) {
    world.component<TickSource>("flecs::system::TickSource");
}

} // namespace _
} // namespace flecs
