/**
 * @file addons/cpp/mixins/timer/system_mixin.inl
 * @brief Timer module system mixin.
 */

/**
 * \memberof flecs::system
 * \ingroup cpp_addons_timer
 */

/** Set interval.
 * @see ecs_set_interval
 */
void interval(ecs_ftime_t interval);

/** Get interval.
 * @see ecs_get_interval.
 */
ecs_ftime_t interval();

/** Set timeout.
 * @see ecs_set_timeout
 */
void timeout(ecs_ftime_t timeout);

/** Get timeout.
 * @see ecs_get_timeout
 */
ecs_ftime_t timeout();

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

/** @} */
