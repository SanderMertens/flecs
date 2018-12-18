/** @file
 * @section time Time API
 * @brief Utility API for doing time measurements.
 */

#ifndef REFLECS_TIME_H_
#define REFLECS_TIME_H_

#include <reflecs>

#ifdef __cplusplus
extern "C" {
#endif

REFLECS_EXPORT
void ut_sleep(
  unsigned int sec,
  unsigned int nanosec);

REFLECS_EXPORT
void ut_sleepf(
    double t);

REFLECS_EXPORT
void ut_time_get(
    struct timespec* time);

REFLECS_EXPORT
double ut_time_to_double(
    struct timespec t);

REFLECS_EXPORT
struct timespec ut_time_sub(
    struct timespec t1,
    struct timespec t2);

REFLECS_EXPORT
double ut_time_measure(
    struct timespec *start);

#ifdef __cplusplus
}
#endif

#endif
