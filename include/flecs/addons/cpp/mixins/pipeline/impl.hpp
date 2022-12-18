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
        m_id = ecs_pipeline_init(world, desc);

        if (!m_id) {
            ecs_abort(ECS_INVALID_PARAMETER, NULL);
        }

        if (desc->query.filter.terms_buffer) {
            ecs_os_free(desc->query.filter.terms_buffer);
        }
    }
};

inline flecs::pipeline_builder<> world::pipeline() const {
    return flecs::pipeline_builder<>(m_world);
}

template <typename Pipeline, if_not_t< is_enum<Pipeline>::value >>
inline flecs::pipeline_builder<> world::pipeline() const {
    return flecs::pipeline_builder<>(m_world, _::cpp_type<Pipeline>::id(m_world));
}

inline void world::set_pipeline(const flecs::entity pip) const {
    return ecs_set_pipeline(m_world, pip);
}

template <typename Pipeline>
inline void world::set_pipeline() const {
    return ecs_set_pipeline(m_world, _::cpp_type<Pipeline>::id(m_world));
}

inline flecs::entity world::get_pipeline() const {
    return flecs::entity(m_world, ecs_get_pipeline(m_world));
}

inline bool world::progress(ecs_ftime_t delta_time) const {
    return ecs_progress(m_world, delta_time);
}

inline void world::run_pipeline(const flecs::entity_t pip, ecs_ftime_t delta_time) const {
    return ecs_run_pipeline(m_world, pip, delta_time);
}

inline void world::set_time_scale(ecs_ftime_t mul) const {
    ecs_set_time_scale(m_world, mul);
}  

inline ecs_ftime_t world::get_time_scale() const {
    const ecs_world_info_t *stats = ecs_get_world_info(m_world);
    return stats->time_scale;
}

inline int64_t world::get_tick() const {
    const ecs_world_info_t *stats = ecs_get_world_info(m_world);
    return stats->frame_count_total;
}

inline ecs_ftime_t world::get_target_fps() const {
    const ecs_world_info_t *stats = ecs_get_world_info(m_world);
    return stats->target_fps;
} 

inline void world::set_target_fps(ecs_ftime_t target_fps) const {
    ecs_set_target_fps(m_world, target_fps);
}

inline void world::reset_clock() const {
    ecs_reset_clock(m_world);
}

inline void world::set_threads(int32_t threads) const {
    ecs_set_threads(m_world, threads);
}

inline int32_t world::get_threads() const {
    return ecs_get_stage_count(m_world);
}

}
