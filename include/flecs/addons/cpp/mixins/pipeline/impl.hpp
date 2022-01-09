#pragma once

namespace flecs {

struct pipeline : type_base<pipeline> {
    explicit pipeline(world_t *world, entity_t e) : type_base(world, e)
    { 
        this->entity().add(flecs::Pipeline);
    }

    explicit pipeline(world_t *world, const char *name) : type_base(world, name)
    { 
        this->entity().add(flecs::Pipeline);
    }
};

template <typename... Args>
inline flecs::pipeline world::pipeline(Args &&... args) const {
    return flecs::pipeline(m_world, FLECS_FWD(args)...);
}

inline void world::set_pipeline(const flecs::pipeline& pip) const {
    return ecs_set_pipeline(m_world, pip.id());
}

inline flecs::pipeline world::get_pipeline() const {
    return flecs::pipeline(m_world, ecs_get_pipeline(m_world));
}

inline bool world::progress(FLECS_FLOAT delta_time) const {
    return ecs_progress(m_world, delta_time);
}

inline void world::run_pipeline(const flecs::pipeline& pip, FLECS_FLOAT delta_time) const {
    return ecs_run_pipeline(m_world, pip.id(), delta_time);
}

inline void world::set_time_scale(FLECS_FLOAT mul) const {
    ecs_set_time_scale(m_world, mul);
}  

inline FLECS_FLOAT world::get_time_scale() const {
    const ecs_world_info_t *stats = ecs_get_world_info(m_world);
    return stats->time_scale;
}

inline int32_t world::get_tick() const {
    const ecs_world_info_t *stats = ecs_get_world_info(m_world);
    return stats->frame_count_total;
}

inline FLECS_FLOAT world::get_target_fps() const {
    const ecs_world_info_t *stats = ecs_get_world_info(m_world);
    return stats->target_fps;
} 

inline void world::set_target_fps(FLECS_FLOAT target_fps) const {
    ecs_set_target_fps(m_world, target_fps);
}

inline void world::reset_clock() const {
    ecs_reset_clock(m_world);
}

inline void world::deactivate_systems() const {
    ecs_deactivate_systems(m_world);
}

inline void world::set_threads(int32_t threads) const {
    ecs_set_threads(m_world, threads);
}

inline int32_t world::get_threads() const {
    return ecs_get_threads(m_world);
}

}
