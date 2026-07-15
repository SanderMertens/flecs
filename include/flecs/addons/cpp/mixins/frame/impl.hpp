/**
 * @file addons/cpp/mixins/frame/impl.hpp
 * @brief Frame implementation.
 */

#pragma once

namespace flecs {

inline ecs_ftime_t world::frame_begin(ecs_ftime_t delta_time) const {
    return ecs_frame_begin(world_, delta_time);
}

inline void world::frame_end() const {
    ecs_frame_end(world_);
}

inline void world::run_post_frame(
    ecs_fini_action_t action,
    void *ctx) const
{
    ecs_run_post_frame(world_, action, ctx);
}

inline void world::quit() const {
    ecs_quit(world_);
}

inline bool world::should_quit() const {
    return ecs_should_quit(world_);
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

}
