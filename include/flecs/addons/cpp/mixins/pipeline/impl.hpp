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
inline flecs::pipeline pipeline_m_world::pipeline(Args &&... args) const {
    return flecs::pipeline(this->me(), std::forward<Args>(args)...);
}

inline void pipeline_m_world::set_pipeline(const flecs::pipeline& pip) const {
    return ecs_set_pipeline(this->me(), pip.id());
}

inline flecs::pipeline pipeline_m_world::get_pipeline() const {
    return flecs::pipeline(this->me(), ecs_get_pipeline(this->me()));
}

inline bool pipeline_m_world::progress(FLECS_FLOAT delta_time) const {
    return ecs_progress(this->me(), delta_time);
}

inline void pipeline_m_world::run_pipeline(const flecs::pipeline& pip, FLECS_FLOAT delta_time) const {
    return ecs_run_pipeline(this->me(), pip.id(), delta_time);
}

inline void pipeline_m_world::set_time_scale(FLECS_FLOAT mul) const {
    ecs_set_time_scale(this->me(), mul);
}  

inline FLECS_FLOAT pipeline_m_world::get_time_scale() const {
    const ecs_world_info_t *stats = ecs_get_world_info(this->me());
    return stats->time_scale;
}

inline int32_t pipeline_m_world::get_tick() const {
    const ecs_world_info_t *stats = ecs_get_world_info(this->me());
    return stats->frame_count_total;
}

inline FLECS_FLOAT pipeline_m_world::get_target_fps() const {
    const ecs_world_info_t *stats = ecs_get_world_info(this->me());
    return stats->target_fps;
} 

inline void pipeline_m_world::set_target_fps(FLECS_FLOAT target_fps) const {
    ecs_set_target_fps(this->me(), target_fps);
}

inline void pipeline_m_world::reset_clock() const {
    ecs_reset_clock(this->me());
}

inline void pipeline_m_world::deactivate_systems() const {
    ecs_deactivate_systems(this->me());
}

inline void pipeline_m_world::set_threads(int32_t threads) const {
    ecs_set_threads(this->me(), threads);
}

inline int32_t pipeline_m_world::get_threads() const {
    return ecs_get_threads(this->me());
}

}
