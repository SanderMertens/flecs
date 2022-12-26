/**
 * @file addons/cpp/mixins/timer/impl.hpp
 * @brief Timer module implementation.
 */

#pragma once

namespace flecs {

// Timer class
struct timer final : entity {
    using entity::entity;

    timer& interval(ecs_ftime_t interval) {
        ecs_set_interval(m_world, m_id, interval);
        return *this;
    }

    ecs_ftime_t interval() {
        return ecs_get_interval(m_world, m_id);
    }

    timer& timeout(ecs_ftime_t timeout) {
        ecs_set_timeout(m_world, m_id, timeout);
        return *this;
    }

    ecs_ftime_t timeout() {
        return ecs_get_timeout(m_world, m_id);
    }

    timer& rate(int32_t rate, flecs::entity_t tick_source = 0) {
        ecs_set_rate(m_world, m_id, rate, tick_source);
        return *this;
    }

    void start() {
        ecs_start_timer(m_world, m_id);
    }

    void stop() {
        ecs_stop_timer(m_world, m_id);
    }
};

template <typename... Args>
inline flecs::timer world::timer(Args &&... args) const {
    return flecs::timer(m_world, FLECS_FWD(args)...);
}

inline void system::interval(ecs_ftime_t interval) {
    ecs_set_interval(m_world, m_id, interval);
}

inline ecs_ftime_t system::interval() {
    return ecs_get_interval(m_world, m_id);
}

inline void system::timeout(ecs_ftime_t timeout) {
    ecs_set_timeout(m_world, m_id, timeout);
}

inline ecs_ftime_t system::timeout() {
    return ecs_get_timeout(m_world, m_id);
}

inline void system::rate(int32_t rate) {
    ecs_set_rate(m_world, m_id, rate, 0);
}

inline void system::start() {
    ecs_start_timer(m_world, m_id);
}

inline void system::stop() {
    ecs_stop_timer(m_world, m_id);
}

inline void system::set_tick_source(flecs::entity e) {
    ecs_set_tick_source(m_world, m_id, e);
}

namespace _ {

inline void timer_init(flecs::world& world) {
    world.component<RateFilter>("flecs::timer::RateFilter");
    world.component<Timer>("flecs::timer::Timer");
}

}
}
