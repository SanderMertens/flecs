/**
 * @file addons/cpp/mixins/pipeline/impl.hpp
 * @brief Pipeline module implementation.
 */

#pragma once

#include "builder.hpp"

namespace flecs {

template <typename ... Components>
struct pipeline : entity {
    pipeline(world_t *world, ecs_pipeline_desc_t *desc) 
        : entity(world)
    {
        id_ = ecs_pipeline_init(world, desc);

        if (!id_) {
            ecs_abort(ECS_INVALID_PARAMETER, NULL);
        }
    }
};

inline flecs::pipeline_builder<> world::pipeline() const {
    return flecs::pipeline_builder<>(world_);
}

template <typename Pipeline, if_not_t< is_enum<Pipeline>::value >>
inline flecs::pipeline_builder<> world::pipeline() const {
    return flecs::pipeline_builder<>(world_, _::type<Pipeline>::id(world_));
}

inline void world::set_pipeline(const flecs::entity pip) const {
    return ecs_set_pipeline(world_, pip);
}

template <typename Pipeline>
inline void world::set_pipeline() const {
    return ecs_set_pipeline(world_, _::type<Pipeline>::id(world_));
}

inline flecs::entity world::get_pipeline() const {
    return flecs::entity(world_, ecs_get_pipeline(world_));
}

inline bool world::progress(ecs_ftime_t delta_time) const {
    return ecs_progress(world_, delta_time);
}

inline void world::run_pipeline(const flecs::entity_t pip, ecs_ftime_t delta_time) const {
    return ecs_run_pipeline(world_, pip, delta_time);
}

template <typename Pipeline, if_not_t< is_enum<Pipeline>::value >>
inline void world::run_pipeline(ecs_ftime_t delta_time) const {
    return ecs_run_pipeline(world_, _::type<Pipeline>::id(world_), delta_time);
}

inline void world::set_time_scale(ecs_ftime_t mul) const {
    ecs_set_time_scale(world_, mul);
}

inline void world::set_target_fps(ecs_ftime_t target_fps) const {
    ecs_set_target_fps(world_, target_fps);
}

inline void world::reset_clock() const {
    ecs_reset_clock(world_);
}

inline void world::set_threads(int32_t threads) const {
    ecs_set_threads(world_, threads);
}

inline int32_t world::get_threads() const {
    return ecs_get_stage_count(world_);
}

inline void world::set_task_threads(int32_t task_threads) const {
    ecs_set_task_threads(world_, task_threads);
}

inline bool world::using_task_threads() const {
    return ecs_using_task_threads(world_);
}

}
