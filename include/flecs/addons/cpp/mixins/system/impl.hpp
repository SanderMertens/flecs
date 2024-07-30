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
        : stage_(world)
        , id_(id)
        , delta_time_(delta_time)
        , param_(param)
        , stage_current_(stage_current)
        , stage_count_(stage_count) { }

    system_runner_fluent& offset(int32_t offset) {
        offset_ = offset;
        return *this;
    }

    system_runner_fluent& limit(int32_t limit) {
        limit_ = limit;
        return *this;
    }

    system_runner_fluent& stage(flecs::world& stage) {
        stage_ = stage.c_ptr();
        return *this;
    }

    ~system_runner_fluent() {
        if (stage_count_) {
            ecs_run_worker(
                stage_, id_, stage_current_, stage_count_, delta_time_,
                param_);            
        } else {
            ecs_run(stage_, id_, delta_time_, param_);
        }
    }

private:
    world_t *stage_;
    entity_t id_;
    ecs_ftime_t delta_time_;
    void *param_;
    int32_t offset_;
    int32_t limit_;
    int32_t stage_current_;
    int32_t stage_count_;
};

struct system final : entity
{
    using entity::entity;

    explicit system() {
        id_ = 0;
        world_ = nullptr;
    }

    explicit system(flecs::world_t *world, ecs_system_desc_t *desc) {
        world_ = world;
        id_ = ecs_system_init(world, desc);
    }

    void ctx(void *ctx) {
        ecs_system_desc_t desc = {};
        desc.entity = id_;
        desc.ctx = ctx;
        ecs_system_init(world_, &desc);
    }

    void* ctx() const {
        return ecs_system_get(world_, id_)->ctx;
    }

    flecs::query<> query() const {
        return flecs::query<>(ecs_system_get(world_, id_)->query);
    }

    system_runner_fluent run(ecs_ftime_t delta_time = 0.0f, void *param = nullptr) const {
        return system_runner_fluent(world_, id_, 0, 0, delta_time, param);
    }

    system_runner_fluent run_worker(
        int32_t stage_current, 
        int32_t stage_count, 
        ecs_ftime_t delta_time = 0.0f, 
        void *param = nullptr) const 
    {
        return system_runner_fluent(
            world_, id_, stage_current, stage_count, delta_time, param);
    }

#   ifdef FLECS_TIMER
#   include "../timer/system_mixin.inl"
#   endif

};

// Mixin implementation
inline system world::system(flecs::entity e) const {
    return flecs::system(world_, e);
}

template <typename... Comps, typename... Args>
inline system_builder<Comps...> world::system(Args &&... args) const {
    return flecs::system_builder<Comps...>(world_, FLECS_FWD(args)...);
}

namespace _ {

inline void system_init(flecs::world& world) {
    world.component<TickSource>("flecs::system::TickSource");
}

} // namespace _
} // namespace flecs
