/**
 * @file addons/cpp/mixins/pipeline/mixin.inl
 * @brief Pipeline world mixin.
 */

/**
 * @memberof flecs::world
 * @ingroup cpp_pipelines
 *
 * @{
 */

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
void run_pipeline(const flecs::entity_t pip, ecs_ftime_t delta_time = 0.0) const;

/** Run pipeline.
 * @tparam Pipeline Type associated with pipeline.
 * @see ecs_run_pipeline
 */
template <typename Pipeline, if_not_t< is_enum<Pipeline>::value > = 0>
void run_pipeline(ecs_ftime_t delta_time = 0.0) const;

/** Set timescale.
 * @see ecs_set_time_scale
 */
void set_time_scale(ecs_ftime_t mul) const;

/** Set target FPS.
 * @see ecs_set_target_fps
 */
void set_target_fps(ecs_ftime_t target_fps) const;

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

/** Set number of task threads.
 * @see ecs_set_task_threads
 */
void set_task_threads(int32_t task_threads) const;

/** Returns true if task thread use has been requested.
 * @see ecs_using_task_threads
 */
bool using_task_threads() const;

/** @} */
