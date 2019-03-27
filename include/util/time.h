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
    struct timespec t);

FLECS_EXPORT
struct timespec ecs_time_sub(
    struct timespec t1,
    struct timespec t2);

FLECS_EXPORT
double ecs_time_measure(
    struct timespec *start);

#ifdef __cplusplus
}
#endif

#endif
