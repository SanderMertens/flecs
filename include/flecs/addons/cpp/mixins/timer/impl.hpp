/**
 * @file addons/cpp/mixins/timer/impl.hpp
 * @brief Timer module implementation.
 */

#pragma once

namespace flecs {

/** Timer class.
 *
 * @ingroup cpp_addons_timer
 */
struct timer final : entity {
    using entity::entity;

    /** Set the timer interval. */
    timer& interval(ecs_ftime_t interval) {
        ecs_set_interval(world_, id_, interval);
        return *this;
    }

    /** Get the timer interval. */
    ecs_ftime_t interval() {
        return ecs_get_interval(world_, id_);
    }

    /** Set the timer timeout. */
    timer& timeout(ecs_ftime_t timeout) {
        ecs_set_timeout(world_, id_, timeout);
        return *this;
    }

    /** Get the timer timeout. */
    ecs_ftime_t timeout() {
        return ecs_get_timeout(world_, id_);
    }

    /** Set the timer rate. */
    timer& rate(int32_t rate, flecs::entity_t tick_source = 0) {
        ecs_set_rate(world_, id_, rate, tick_source);
        return *this;
    }

    /** Start the timer. */
    void start() {
        ecs_start_timer(world_, id_);
    }

    /** Stop the timer. */
    void stop() {
        ecs_stop_timer(world_, id_);
    }
};

template <typename T>
inline flecs::timer world::timer() const {
    return flecs::timer(world_, _::type<T>::id(world_));
}

template <typename... Args>
inline flecs::timer world::timer(Args &&... args) const {
    return flecs::timer(world_, FLECS_FWD(args)...);
}

/** Randomize timers for all systems. */
inline void world::randomize_timers() const {
    ecs_randomize_timers(world_);
}

/** Set the interval for a system. */
inline void system::interval(ecs_ftime_t interval) {
    ecs_set_interval(world_, id_, interval);
}

/** Get the interval for a system. */
inline ecs_ftime_t system::interval() {
    return ecs_get_interval(world_, id_);
}

/** Set the timeout for a system. */
inline void system::timeout(ecs_ftime_t timeout) {
    ecs_set_timeout(world_, id_, timeout);
}

/** Get the timeout for a system. */
inline ecs_ftime_t system::timeout() {
    return ecs_get_timeout(world_, id_);
}

/** Set the rate for a system. */
inline void system::rate(int32_t rate) {
    ecs_set_rate(world_, id_, rate, 0);
}

/** Start the system timer. */
inline void system::start() {
    ecs_start_timer(world_, id_);
}

/** Stop the system timer. */
inline void system::stop() {
    ecs_stop_timer(world_, id_);
}

/** Set the tick source for a system by type. */
template<typename T>
inline void system::set_tick_source() {
    ecs_set_tick_source(world_, id_, _::type<T>::id(world_));
}

/** Set the tick source for a system by entity. */
inline void system::set_tick_source(flecs::entity e) {
    ecs_set_tick_source(world_, id_, e);
}

namespace _ {

inline void timer_init(flecs::world& world) {
    world.component<RateFilter>("flecs::timer::RateFilter");
    world.component<Timer>("flecs::timer::Timer");
}

}
}
