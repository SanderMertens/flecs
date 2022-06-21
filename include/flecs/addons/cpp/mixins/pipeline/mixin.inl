
/** Create a new pipeline.
 *
 * @return A pipeline builder.
 */
flecs::pipeline_builder<> pipeline() const;

/** Create a new pipeline.
 *
 * @tparam Pipeline Type associated with pipeline.
 * @return A pipeline builder.
 */
template <typename Pipeline, if_not_t< is_enum<Pipeline>::value > = 0>
flecs::pipeline_builder<> pipeline() const;

/** Set pipeline.
 * @see ecs_set_pipeline
 */
void set_pipeline(const flecs::entity pip) const;

/** Set pipeline.
 * @see ecs_set_pipeline
 */
template <typename Pipeline>
void set_pipeline() const;

/** Get pipeline.
 * @see ecs_get_pipeline
 */
flecs::entity get_pipeline() const;

/** Progress world one tick.
 * @see ecs_progress
 */
bool progress(ecs_ftime_t delta_time = 0.0) const;

/** Run pipeline.
 * @see ecs_run_pipeline
 */
void run_pipeline(const flecs::entity pip, ecs_ftime_t delta_time = 0.0) const;

/** Set timescale
 * @see ecs_set_time_scale
 */
void set_time_scale(ecs_ftime_t mul) const;

/** Get timescale
 * @see ecs_get_time_scale
 */
ecs_ftime_t get_time_scale() const;

/** Get tick
 * @return Monotonically increasing frame count.
 */
int32_t get_tick() const;

/** Set target FPS
 * @see ecs_set_target_fps
 */
void set_target_fps(ecs_ftime_t target_fps) const;

/** Get target FPS
 * @return Configured frames per second.
 */
ecs_ftime_t get_target_fps() const;

/** Reset simulation clock.
 * @see ecs_reset_clock
 */
void reset_clock() const;

/** Set number of threads.
 * @see ecs_set_threads
 */
void set_threads(int32_t threads) const;

/** Set number of threads.
 * @see ecs_get_stage_count
 */
int32_t get_threads() const;
