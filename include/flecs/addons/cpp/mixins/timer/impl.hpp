#pragma once

namespace flecs {

// Timer class
struct timer final : entity {
    template <typename ... Args>
    timer(Args&&... args) : entity(FLECS_FWD(args)...) { }
};

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
