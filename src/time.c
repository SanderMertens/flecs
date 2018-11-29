
#include <time.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <reflecs/util/time.h>

#ifdef __MACH__
#include <mach/clock.h>
#include <mach/mach.h>
#endif

void ut_sleep(
    unsigned int sec,
    unsigned int nanosec)
{
    struct timespec sleepTime;

    sleepTime.tv_sec = sec;
    sleepTime.tv_nsec = nanosec;
    if (nanosleep(&sleepTime, NULL)) {
        fprintf(stderr, "nanosleep failed: %s\n", strerror(errno));
    }
}

void ut_sleepf(
    double t)
{
    if (t > 0) {
        int sec = t;
        int nsec = ((t - sec) * 1000000000);
        ut_sleep(sec, nsec);
    }
}

void ut_time_get(
    struct timespec* time)
{
#ifdef __MACH__ // OS X does not have clock_gettime, use clock_get_time
    clock_serv_t cclock;
    mach_timespec_t mts;
    host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock);
    clock_get_time(cclock, &mts);
    mach_port_deallocate(mach_task_self(), cclock);
    time->tv_sec = mts.tv_sec;
    time->tv_nsec = mts.tv_nsec;
#else
    clock_gettime(CLOCK_REALTIME, time);
#endif
}

/** Convert time to double */
double ut_time_to_double(
    struct timespec t)
{
    double result;
    result = t.tv_sec;
    return result + (double)t.tv_nsec / (double)1000000000;;
}

struct timespec ut_time_sub(
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

double ut_time_measure(
    struct timespec *start)
{
    struct timespec stop, temp;
    ut_time_get(&stop);
    temp = stop;
    stop = ut_time_sub(stop, *start);
    *start = temp;
    return ut_time_to_double(stop);
}
