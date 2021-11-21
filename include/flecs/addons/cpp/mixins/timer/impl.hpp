#pragma once

namespace flecs {

#define flecs_me_ this->me()
#define flecs_world_ flecs_me_.world()

// Timer class
struct timer final : extendable<timer, Mixins>, entity_base {
    template <typename ... Args>
    timer(Args&&... args) : entity_base(std::forward<Args>(args)...) { }
};

// Mixin functions for flecs::system and flecs::timer
inline void timer_m_world::init() {
    flecs_me_.template component<RateFilter>("flecs::timer::RateFilter");
    flecs_me_.template component<Timer>("flecs::timer::Timer");
}

template <typename T>
inline void timer_m_base<T>::interval(FLECS_FLOAT interval) {
    ecs_set_interval(flecs_world_, flecs_me_, interval);
}

template <typename T>
inline FLECS_FLOAT timer_m_base<T>::interval() {
    return ecs_get_interval(flecs_world_, flecs_me_);
}

template <typename T>
inline void timer_m_base<T>::timeout(FLECS_FLOAT timeout) {
    ecs_set_timeout(flecs_world_, flecs_me_, timeout);
}

template <typename T>
inline FLECS_FLOAT timer_m_base<T>::timeout() {
    return ecs_get_timeout(flecs_world_, flecs_me_);
}

template <typename T>
inline void timer_m_base<T>::rate(int32_t rate) {
    ecs_set_rate(flecs_world_, flecs_me_, rate, 0);
}

template <typename T>
inline void timer_m_base<T>::start() {
    ecs_start_timer(flecs_world_, flecs_me_);
}

template <typename T>
inline void timer_m_base<T>::stop() {
    ecs_stop_timer(flecs_world_, flecs_me_);
}

template <typename T>
inline void timer_m_base<T>::set_tick_source(flecs::entity e) {
    ecs_set_tick_source(flecs_world_, flecs_me_, e);
}

#undef flecs_world_
#undef flecs_me_

}
