/**
 * @file addons/cpp/mixins/timer/mixin.inl
 * @brief Timer module mixin.
 */

/**
 * @memberof flecs::world
 * @ingroup cpp_addons_timer
 */

/** Find or register a singleton timer. */
template <typename T>
flecs::timer timer() const;

/** Find or register a timer. */
template <typename... Args>
flecs::timer timer(Args &&... args) const;

/** Enable randomization of initial time values for timers.
 * @see ecs_randomize_timers
 */
void randomize_timers() const;
