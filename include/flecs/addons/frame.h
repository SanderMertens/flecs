/**
 * @file addons/frame.h
 * @brief Frame management.
 */

#ifdef FLECS_FRAME

#if !defined(FLECS_OS_API_IMPL) && !defined(FLECS_NO_OS_API_IMPL)
#define FLECS_OS_API_IMPL
#endif

#ifndef FLECS_FRAME_H
#define FLECS_FRAME_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup c_addons_frame Frame
 * @ingroup c_addons
 * Frame management.
 *
 * @{
 */

/** Begin frame.
 * When an application does not use ecs_progress() to control the main loop, it
 * can still use Flecs features such as FPS limiting and time measurements. This
 * operation needs to be invoked whenever a new frame is about to get processed.
 *
 * Calls to ecs_frame_begin() must always be followed by ecs_frame_end().
 *
 * The function accepts a delta_time parameter, which will get passed to
 * systems. This value is also used to compute the amount of time the function
 * needs to sleep to ensure it does not exceed the target_fps, when it is set.
 * When 0 is provided for delta_time, the time will be measured.
 *
 * This function should only be run from the main thread.
 *
 * When the clock does not advance in between two measurements, a minimal
 * nonzero delta time is returned instead of blocking until it does. The time
 * that was not reported is credited to the frame in which the clock next
 * advances. The first such frame logs a warning. Code that derives a rate by
 * dividing by the delta time should treat a delta at or below 1e-9 as a frame
 * in which no time could be measured, rather than as a rate.
 *
 * @param world The world.
 * @param delta_time Time elapsed since the last frame.
 * @return The provided delta_time, or measured time if 0 was provided.
 */
FLECS_API
ecs_ftime_t ecs_frame_begin(
    ecs_world_t *world,
    ecs_ftime_t delta_time);

/** End frame.
 * This operation must be called at the end of the frame, and always after
 * ecs_frame_begin().
 *
 * @param world The world.
 */
FLECS_API
void ecs_frame_end(
    ecs_world_t *world);

/** Register an action to be executed once after the frame.
 * Post frame actions are typically used for calling operations that cannot be
 * invoked during iteration, such as changing the number of threads.
 *
 * @param world The world.
 * @param action The function to execute.
 * @param ctx Userdata to pass to the function.
 */
FLECS_API
void ecs_run_post_frame(
    ecs_world_t *world,
    ecs_fini_action_t action,
    void *ctx);

/** Signal exit.
 * This operation signals that the application should quit. It will cause
 * ecs_progress() to return false.
 *
 * @param world The world to quit.
 */
FLECS_API
void ecs_quit(
    ecs_world_t *world);

/** Return whether a quit has been requested.
 *
 * @param world The world.
 * @return Whether a quit has been requested.
 * @see ecs_quit()
 */
FLECS_API
bool ecs_should_quit(
    const ecs_world_t *world);

/** Measure frame time.
 * Frame time measurements measure the total time passed in a single frame, and
 * how much of that time was spent on systems and on merging.
 *
 * Frame time measurements add a small constant-time overhead to an application.
 * When an application sets a target FPS, frame time measurements are enabled by
 * default.
 *
 * @param world The world.
 * @param enable Whether to enable or disable frame time measuring.
 */
FLECS_API
void ecs_measure_frame_time(
    ecs_world_t *world,
    bool enable);

/** Measure system time.
 * System time measurements measure the time spent in each system.
 *
 * System time measurements add overhead to every system invocation and
 * therefore have a small but measurable impact on application performance.
 * System time measurements must be enabled before obtaining system statistics.
 *
 * @param world The world.
 * @param enable Whether to enable or disable system time measuring.
 */
FLECS_API
void ecs_measure_system_time(
    ecs_world_t *world,
    bool enable);

/** Set target frames per second (FPS) for an application.
 * Setting the target FPS ensures that ecs_progress() is not invoked faster than
 * the specified FPS. When enabled, ecs_progress() tracks the time passed since
 * the last invocation, and sleeps the remaining time of the frame (if any).
 *
 * This feature ensures systems are run at a consistent interval, as well as
 * conserving CPU time by not running systems more often than required.
 *
 * Note that ecs_progress() only sleeps if there is time left in the frame. Both
 * time spent in Flecs and time spent outside of Flecs are taken into account.
 *
 * Frame rate limiting requires the OS API get_time and sleep functions to agree
 * about the passage of time, to within roughly a factor of four. Both may run
 * on a virtualized timeline of any speed, but an application that replaces one
 * of them must replace the other consistently. A clock that observes at least
 * a quarter of each interval slept reaches the target frame time to within a
 * sixteenth of it, blocking for as long as the disagreement costs, which stays
 * under five frame periods. If
 * the two disagree by more than that (for example a clock that is only stepped
 * by a host in between frames, while sleeping does take time) frame rate
 * limiting requests about one frame period worth of sleep and is then skipped
 * for the remainder of that frame. If the clock is too coarse to observe an
 * individual sleep, sleep intervals are enlarged until it can, which makes
 * pacing coarser but keeps it working. A frame sleeps at most 128 times
 * whatever the clock does, so it never waits for one indefinitely. An
 * application that computes its target from untrusted input should range check
 * it before passing it in.
 *
 * @param world The world.
 * @param fps The target FPS.
 */
FLECS_API
void ecs_set_target_fps(
    ecs_world_t *world,
    ecs_ftime_t fps);

/** Set time scale.
 * Increase or decrease simulation speed by the provided multiplier.
 *
 * @param world The world.
 * @param scale The scale to apply (default = 1).
 */
FLECS_API
void ecs_set_time_scale(
    ecs_world_t *world,
    ecs_ftime_t scale);

/** Reset world clock.
 * Reset the clock that keeps track of the total time passed in the simulation.
 *
 * @param world The world.
 */
FLECS_API
void ecs_reset_clock(
    ecs_world_t *world);

/** @} */

#ifdef __cplusplus
}
#endif

#endif

#endif // FLECS_FRAME
