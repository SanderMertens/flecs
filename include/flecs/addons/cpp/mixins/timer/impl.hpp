#pragma once

namespace flecs {

#define me_ this->me()
#define world_ me_.world()

// Timer class
struct timer : extendable<timer, Mixins>, entity {
    template <typename ... Args>
    timer(Args&&... args) : entity(std::forward<Args>(args)...) { }
};

// Mixin functions for flecs::system and flecs::timer
inline void timer_m_world::init() {
    me_.template component<RateFilter>("flecs::timer::RateFilter");
    me_.template component<Timer>("flecs::timer::Timer");
}

template <typename T>
inline void timer_m_base<T>::interval(FLECS_FLOAT interval) {
    ecs_set_interval(world_, me_, interval);
}

template <typename T>
inline FLECS_FLOAT timer_m_base<T>::interval() {
    return ecs_get_interval(world_, me_);
}

template <typename T>
inline void timer_m_base<T>::timeout(FLECS_FLOAT timeout) {
    ecs_set_timeout(world_, me_, timeout);
}

template <typename T>
inline FLECS_FLOAT timer_m_base<T>::timeout() {
    return ecs_get_timeout(world_, me_);
}

template <typename T>
inline void timer_m_base<T>::rate(int32_t rate) {
    ecs_set_rate(world_, me_, rate, 0);
}

template <typename T>
inline void timer_m_base<T>::start() {
    ecs_start_timer(world_, me_);
}

template <typename T>
inline void timer_m_base<T>::stop() {
    ecs_stop_timer(world_, me_);
}

template <typename T>
inline void timer_m_base<T>::set_tick_source(flecs::entity e) {
    ecs_set_tick_source(world_, me_, e);
}

#undef world_
#undef me_

}
