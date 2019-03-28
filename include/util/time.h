/** @file
 * @section time Time API
 * @brief Utility API for doing time measurements.
 */

#ifndef FLECS_TIME_H_
#define FLECS_TIME_H_

#include "../flecs.h"

#ifdef __cplusplus
extern "C" {
#endif

FLECS_EXPORT
void ecs_sleepf(
    double t);

FLECS_EXPORT
double ecs_time_to_double(
    ecs_time_t t);

FLECS_EXPORT
ecs_time_t ecs_time_sub(
    ecs_time_t t1,
    ecs_time_t t2);

FLECS_EXPORT
double ecs_time_measure(
    ecs_time_t *start);

#ifdef __cplusplus
}
#endif

#endif
