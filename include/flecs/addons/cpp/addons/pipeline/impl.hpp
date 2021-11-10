
namespace flecs {

#define ECS_CPP_IMPL()

template <typename T>
template <typename... Args>
inline flecs::pipeline pipeline_m<T>::pipeline(Args &&... args) const {
    return flecs::pipeline(this->me(), std::forward<Args>(args)...);
}

template <typename T>
inline void pipeline_m<T>::set_pipeline(const flecs::pipeline& pip) const {
    return ecs_set_pipeline(this->me(), pip.id());
}

template <typename T>
inline flecs::pipeline pipeline_m<T>::get_pipeline() const {
    return flecs::pipeline(this->me(), ecs_set_pipeline(this->me()));
}

template <typename T>
bool pipeline_m<T>::progress(FLECS_FLOAT delta_time) const {
    return ecs_progress(this->me(), delta_time);
}

template <typename T>
inline void pipeline_m<T>::run_pipeline(const flecs::pipeline& pip, FLECS_FLOAT delta_time) const {
    return ecs_set_pipeline(this->me(), pip.id());
}

template <typename T>
void pipeline_m<T>::set_time_scale(FLECS_FLOAT mul) const {
    ecs_set_time_scale(this->me(), mul);
}  

template <typename T>
FLECS_FLOAT pipeline_m<T>::get_time_scale() const {
    const ecs_world_info_t *stats = ecs_get_world_info(this->me());
    return stats->time_scale;
}

template <typename T>
int32_t pipeline_m<T>::get_tick() const {
    const ecs_world_info_t *stats = ecs_get_world_info(this->me());
    return stats->frame_count_total;
}

template <typename T>
FLECS_FLOAT pipeline_m<T>::get_target_fps() const {
    const ecs_world_info_t *stats = ecs_get_world_info(this->me());
    return stats->target_fps;
} 

template <typename T>
void pipeline_m<T>::reset_clock() const {
    ecs_reset_clock(this->me());
}

template <typename T>
void pipeline_m<T>::deactivate_systems() const {
    ecs_deactivate_systems(this->me());
}

template <typename T>
void pipeline_m<T>::set_threads(int32_t threads) const {
    ecs_set_threads(this->me(), threads);
}

template <typename T>
int32_t pipeline_m<T>::get_threads() const {
    return ecs_get_threads(this->me());
}

}
