
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
