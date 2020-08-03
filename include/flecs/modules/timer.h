#ifdef FLECS_STATS
#define FLECS_MODULE
#define FLECS_PIPELINE

#ifndef FLECS_TIMER_H
#define FLECS_TIMER_H

#include "flecs.h"
#include "flecs/modules/pipeline.h"

#ifdef __cplusplus
extern "C" {
#endif


////////////////////////////////////////////////////////////////////////////////
//// Components
////////////////////////////////////////////////////////////////////////////////

FLECS_EXPORT
extern ecs_type_t 
    ecs_type(EcsTimer),
    ecs_type(EcsRateFilter);

/** Component used for timer functionality */
typedef struct EcsTimer {
    float timeout;         /* Timer timeout period */
    float time;            /* Incrementing time value */
    int32_t fired_count;   /* Number of times ticked */
    bool active;           /* Is the timer active or not */
    bool single_shot;      /* Is this a single shot timer */
} EcsTimer;

/* Apply a rate filter to a tick source */
typedef struct EcsRateFilter {
    ecs_entity_t src;
    int32_t rate;
    int32_t tick_count;
    float time_elapsed;   /* Time elapsed since last tick */
} EcsRateFilter;


////////////////////////////////////////////////////////////////////////////////
//// Timer API
////////////////////////////////////////////////////////////////////////////////

/** Set timer timeout.
 * This operation executes any systems associated with the timer after the
 * specified timeout value. If the entity contains an existing timer, the 
 * timeout value will be reset.
 *
 * Any entity can be used as a timer (including systems). If a timeout value is
 * set on a system entity, it will be automatically applied to that system.
 *
 * The timer is synchronous, and is incremented each frame by delta_time.
 *
 * @param world The world.
 * @param timer The timer for which to set the timeout (0 to create one).
 * @param timeout The timeout value.
 * @return The timer entity.
 */
FLECS_EXPORT
ecs_entity_t ecs_set_timeout(
    ecs_world_t *world,
    ecs_entity_t timer,
    float timeout);

/** Get current timeout value for the specified timer.
 * This operation returns the value set by ecs_set_timeout. If no timer is
 * active for this entity, the operation returns 0.
 *
 * After the timeout expires the timer component is removed from the entity.
 * This means that if ecs_get_timeout is invoked after the timer is expired, the
 * operation will return 0.
 *
 * @param world The world.
 * @param timer The timer.
 * @return The current timeout value, or 0 if no timer is active.
 */
FLECS_EXPORT
float ecs_get_timeout(
    ecs_world_t *world,
    ecs_entity_t timer);

/** Set timer interval.
 * This operation will continously invoke systems associated with the timer 
 * after the interval period expires. If the entity contains an existing timer,
 * the interval value will be reset.
 *
 * Any entity can be used as a timer (including systems). If an interval value
 * is set on a system entity, it will be automatically applied to that system.
 *
 * The timer is synchronous, and is incremented each frame by delta_time.
 *
 * @param world The world.
 * @param timer The timer for which to set the interval (0 to create one).
 * @param interval The interval value.
 * @return The timer entity.
 */
FLECS_EXPORT
ecs_entity_t ecs_set_interval(
    ecs_world_t *world,
    ecs_entity_t timer,
    float interval);   

/** Get current interval value for the specified timer.
 * This operation returns the value set by ecs_set_interval. If no timer is
 * active for this entity, the operation returns 0.
 *
 * @param world The world.
 * @param timer The timer for which to set the interval. If 0, an entity will be created.
 * @return The current interval value, or 0 if no timer is active.
 */
FLECS_EXPORT
float ecs_get_interval(
    ecs_world_t *world,
    ecs_entity_t timer);

/** Start timer.
 * This operation resets the timer and starts it with the specified timeout. The
 * entity must have the EcsTimer component (added by ecs_set_timeout and 
 * ecs_set_interval). If the entity does not have the EcsTimer component this
 * operation will assert.
 *
 * @param world The world.
 * @param timer The timer to start.
 */
FLECS_EXPORT
void ecs_start_timer(
    ecs_world_t *world,
    ecs_entity_t timer);

/** Stop timer
 * This operation stops a timer from triggering. The entity must have the 
 * EcsTimer component or this operation will assert.
 *
 * @param world The world.
 * @param timer The timer to stop.
 */
FLECS_EXPORT
void ecs_stop_timer(
    ecs_world_t *world,
    ecs_entity_t timer);

/** Set rate filter.
 * This operation sets the source and rate for a rate filter. A rate filter
 * samples another tick source (or frames, if none provided) and ticks when the
 * number of sampled ticks equals the rate.
 *
 * @param world The world.
 * @param filter The filter entity (0 to create one).
 * @param rate The rate to apply.
 * @param source The tick source (0 to use frames)
 * @return The filter entity.
 */
FLECS_EXPORT
ecs_entity_t ecs_set_rate_filter(
    ecs_world_t *world,
    ecs_entity_t filter,
    int32_t rate,
    ecs_entity_t source);

/** Assign tick source to system.
 * This operation associates a system with a tick source. If the system is both 
 * active and enabled at the moment the tick source fires, it will be executed.
 * If no tick source is associated with a system, it will be invoked every 
 * frame.
 *
 * To disassociate a tick source from a system, use 0 for the tick_source 
 * parameter.
 *
 * Timer and rate filter entities are valid tick sources. An application can
 * also create its own tick source by setting the EcsTickSource component on an
 * entity.
 *
 * If an entity without the EcsTickSource component is provided as tick source,
 * the system will not be executed.
 *
 * @param world The world.
 * @param system The system to associate with the timer.
 * @param timer The timer to associate with the system.
 */ 
FLECS_EXPORT
void ecs_set_tick_source(
    ecs_world_t *world,
    ecs_entity_t system,
    ecs_entity_t tick_source);


////////////////////////////////////////////////////////////////////////////////
//// Module
////////////////////////////////////////////////////////////////////////////////

/* Timers module component */
typedef struct FlecsTimer {
    int32_t dummy;
} FlecsTimer;

FLECS_EXPORT
void FlecsTimerImport(
    ecs_world_t *world);

#define FlecsTimerImportHandles(handles)

#ifdef __cplusplus
}
#endif

#endif

#endif
