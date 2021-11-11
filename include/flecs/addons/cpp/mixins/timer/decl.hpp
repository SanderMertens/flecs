#pragma once

namespace flecs {

using Timer = EcsTimer;
using RateFilter = EcsRateFilter;

struct timer;

template<typename T>
struct timer_m : mixin<T> { };

/** Timer mixin for flecs::world. */
template<>
struct timer_m<flecs::world> : mixin<flecs::world> {
  void init();
};

/** Shared functions between flecs::system and flecs::timer mixins. */
template<typename T>
struct timer_m_base : mixin<T> {
  void init() { }

  /** Set interval.
   * @see ecs_set_interval
   */
  void interval(FLECS_FLOAT interval);

  /** Get interval.
   * @see ecs_get_interval.
   */
  FLECS_FLOAT interval();

  /** Set timeout.
   * @see ecs_set_timeout
   */
  void timeout(FLECS_FLOAT timeout);

  /** Get timeout.
   * @see ecs_get_timeout
   */
  FLECS_FLOAT timeout();

  /** Set system rate (system is its own tick source).
   * @see ecs_set_rate
   */
  void rate(int32_t rate);

  /** Start timer.
   * @see ecs_start_timer
   */
  void start();

  /** Stop timer.
   * @see ecs_start_timer
   */
  void stop();

  /** Set external tick source.
   * @see ecs_set_tick_source
   */
  void set_tick_source(flecs::entity e);
};

/** Timer mixin for flecs::timer. */
template<>
struct timer_m<flecs::timer> : timer_m_base<flecs::timer> { };

/** Timer mixin for flecs::system. */
template<>
struct timer_m<flecs::system> : timer_m_base<flecs::system> { };

using timer_m_world = timer_m<flecs::world>;
using timer_m_system = timer_m<flecs::system>;

}
