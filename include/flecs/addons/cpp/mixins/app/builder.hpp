/**
 * @file addons/cpp/mixins/app/builder.hpp
 * @brief App builder.
 */

#pragma once

namespace flecs {

/**
 * @defgroup cpp_addons_app App
 * @ingroup cpp_addons
 * Optional addon for running the main application loop.
 *
 * @{
 */

/** App builder interface */
struct app_builder {
    app_builder(flecs::world_t *world)
        : world_(world)
        , desc_{}
    {
        const ecs_world_info_t *stats = ecs_get_world_info(world);
        desc_.target_fps = stats->target_fps;
        ecs_ftime_t t_zero = 0.0;
        if (ECS_EQ(desc_.target_fps, t_zero)) {
            desc_.target_fps = 60;
        }
    }

    app_builder& target_fps(ecs_ftime_t value) {
        desc_.target_fps = value;
        return *this;
    }

    app_builder& delta_time(ecs_ftime_t value) {
        desc_.delta_time = value;
        return *this;
    }

    app_builder& threads(int32_t value) {
        desc_.threads = value;
        return *this;
    }

    app_builder& frames(int32_t value) {
        desc_.frames = value;
        return *this;
    }

    app_builder& enable_rest(uint16_t port = 0) {
        desc_.enable_rest = true;
        desc_.port = port;
        return *this;
    }

    app_builder& enable_stats(bool value = true) {
        desc_.enable_stats = value;
        return *this;
    }

    app_builder& init(ecs_app_init_action_t value) {
        desc_.init = value;
        return *this;
    }

    app_builder& ctx(void *value) {
        desc_.ctx = value;
        return *this;
    }

    int run() {
        int result = ecs_app_run(world_, &desc_);
        if (ecs_should_quit(world_)) {
            // Only free world if quit flag is set. This ensures that we won't
            // try to cleanup the world if the app is used in an environment
            // that takes over the main loop, like with emscripten.
            if (!flecs_poly_release(world_)) {
                ecs_fini(world_);
            }
        }
        return result;
    }

private:
    flecs::world_t *world_;
    ecs_app_desc_t desc_;
};

/** @} */

}
