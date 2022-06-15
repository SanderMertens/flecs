#pragma once

namespace flecs {

// App builder interface
struct app_builder {
    app_builder(flecs::world_t *world)
        : m_world(world)
        , m_desc{}
    {
        const ecs_world_info_t *stats = ecs_get_world_info(world);
        m_desc.target_fps = stats->target_fps;
        if (m_desc.target_fps == static_cast<ecs_ftime_t>(0.0)) {
            m_desc.target_fps = 60;
        }
    }

    app_builder& target_fps(ecs_ftime_t value) {
        m_desc.target_fps = value;
        return *this;
    }

    app_builder& delta_time(ecs_ftime_t value) {
        m_desc.delta_time = value;
        return *this;
    }

    app_builder& threads(int32_t value) {
        m_desc.threads = value;
        return *this;
    }

    app_builder& enable_rest(bool value = true) {
        m_desc.enable_rest = value;
        return *this;
    }

    app_builder& enable_monitor(bool value = true) {
        m_desc.enable_monitor = value;
        return *this;
    }

    app_builder& init(ecs_app_init_action_t value) {
        m_desc.init = value;
        return *this;
    }

    app_builder& ctx(void *value) {
        m_desc.ctx = value;
        return *this;
    }

    int run() {
        return ecs_app_run(m_world, &m_desc);
    }

private:
    flecs::world_t *m_world;
    ecs_app_desc_t m_desc;
};

}
