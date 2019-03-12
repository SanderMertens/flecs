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
void ut_sleep(
  unsigned int sec,
  unsigned int nanosec);

FLECS_EXPORT
void ut_sleepf(
    double t);

FLECS_EXPORT
void ut_time_get(
    struct timespec* time);

FLECS_EXPORT
double ut_time_to_double(
    struct timespec t);

FLECS_EXPORT
struct timespec ut_time_sub(
    struct timespec t1,
    struct timespec t2);

FLECS_EXPORT
double ut_time_measure(
    struct timespec *start);

#ifdef __cplusplus
}
#endif

#endif
