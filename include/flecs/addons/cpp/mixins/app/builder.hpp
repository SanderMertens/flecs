#pragma once

namespace flecs {

// App builder interface
struct app_builder {
    app_builder(flecs::world_t *world)
        : m_world(world)
        , m_desc{}
    {
        m_desc.target_fps = 60;
    }

    app_builder& target_fps(FLECS_FLOAT value) {
        m_desc.target_fps = value;
        return *this;
    }

    app_builder& delta_time(FLECS_FLOAT value) {
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
