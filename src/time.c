
#include <time.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <include/util/time.h>

void ecs_sleepf(
    double t)
{
    if (t > 0) {
        int sec = t;
        int nsec = ((t - sec) * 1000000000);
        ecs_os_sleep(sec, nsec);
    }
}

/** Convert time to double */
double ecs_time_to_double(
    struct timespec t)
{
    double result;
    result = t.tv_sec;
    return result + (double)t.tv_nsec / (double)1000000000;;
}

struct timespec ecs_time_sub(
    struct timespec t1,
    struct timespec t2)
{
    struct timespec result;

    if (t1.tv_nsec >= t2.tv_nsec) {
        result.tv_nsec = t1.tv_nsec - t2.tv_nsec;
        result.tv_sec = t1.tv_sec - t2.tv_sec;
    } else {
        result.tv_nsec = t1.tv_nsec - t2.tv_nsec + 1000000000;
        result.tv_sec = t1.tv_sec - t2.tv_sec - 1;
    }

    return result;
}

double ecs_time_measure(
    struct timespec *start)
{
    struct timespec stop, temp;
    ecs_os_get_time(&stop);
    temp = stop;
    stop = ecs_time_sub(stop, *start);
    *start = temp;
    return ecs_time_to_double(stop);
}
