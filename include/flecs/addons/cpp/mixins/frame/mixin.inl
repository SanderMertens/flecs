/**
 * @file addons/cpp/mixins/frame/mixin.inl
 * @brief Frame world mixin.
 */

/**
 * @defgroup cpp_addons_frame Frame
 * @ingroup cpp_addons
 * Frame management.
 *
 * @{
 * @}
 */

/**
 * @memberof flecs::world
 * @ingroup cpp_addons_frame
 *
 * @{
 */

/** Begin frame.
 * Calls to frame_begin() must always be followed by frame_end().
 *
 * @param delta_time Time elapsed since the last frame.
 * @return The provided delta_time, or measured time if 0 was provided.
 * @see ecs_frame_begin()
 */
ecs_ftime_t frame_begin(ecs_ftime_t delta_time = 0) const;

/** End frame.
 * @see ecs_frame_end()
 */
void frame_end() const;

/** Run callback after completing the frame.
 * @see ecs_run_post_frame()
 */
void run_post_frame(ecs_fini_action_t action, void *ctx) const;

/** Signal application should quit.
 * @see ecs_quit()
 */
void quit() const;

/** Test if quit() has been called.
 * @see ecs_should_quit()
 */
bool should_quit() const;

/** Set the time scale.
 * @see ecs_set_time_scale()
 */
void set_time_scale(ecs_ftime_t mul) const;

/** Set the target FPS.
 * @see ecs_set_target_fps()
 */
void set_target_fps(ecs_ftime_t target_fps) const;

/** Reset the simulation clock.
 * @see ecs_reset_clock()
 */
void reset_clock() const;

/** @} */
