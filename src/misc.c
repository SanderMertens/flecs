#include "flecs_private.h"

/** Convert time to double */
double ecs_time_to_double(
    ecs_time_t t)
{
    double result;
    result = t.sec;
    return result + (double)t.nanosec / (double)1000000000;;
}

ecs_time_t ecs_time_sub(
    ecs_time_t t1,
    ecs_time_t t2)
{
    ecs_time_t result;

    if (t1.nanosec >= t2.nanosec) {
        result.nanosec = t1.nanosec - t2.nanosec;
        result.sec = t1.sec - t2.sec;
    } else {
        result.nanosec = t1.nanosec - t2.nanosec + 1000000000;
        result.sec = t1.sec - t2.sec - 1;
    }

    return result;
}

void ecs_sleepf(
    double t)
{
    if (t > 0) {
        int sec = t;
        int nsec = ((t - sec) * 1000000000);
        ecs_os_sleep(sec, nsec);
    }
}

double ecs_time_measure(
    ecs_time_t *start)
{
    ecs_time_t stop, temp;
    ecs_os_get_time(&stop);
    temp = stop;
    stop = ecs_time_sub(stop, *start);
    *start = temp;
    return ecs_time_to_double(stop);
}

void* ecs_os_memdup(const void *src, size_t size) {
    if (!src) {
        return NULL;
    }
    
    void *dst = ecs_os_malloc(size);
    ecs_assert(dst != NULL, ECS_OUT_OF_MEMORY, NULL);
    memcpy(dst, src, size);  
    return dst;  
}

/*
    This code was taken from sokol_time.h 
    
    zlib/libpng license
    Copyright (c) 2018 Andre Weissflog
    This software is provided 'as-is', without any express or implied warranty.
    In no event will the authors be held liable for any damages arising from the
    use of this software.
    Permission is granted to anyone to use this software for any purpose,
    including commercial applications, and to alter it and redistribute it
    freely, subject to the following restrictions:
        1. The origin of this software must not be misrepresented; you must not
        claim that you wrote the original software. If you use this software in a
        product, an acknowledgment in the product documentation would be
        appreciated but is not required.
        2. Altered source versions must be plainly marked as such, and must not
        be misrepresented as being the original software.
        3. This notice may not be removed or altered from any source
        distribution.
*/

#include "flecs_private.h"

static int ecs_os_time_initialized;

#if defined(_WIN32)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
static double _ecs_os_time_win_freq;
static LARGE_INTEGER _ecs_os_time_win_start;
#elif defined(__APPLE__) && defined(__MACH__)
#include <mach/mach_time.h>
static mach_timebase_info_data_t _ecs_os_time_osx_timebase;
static uint64_t _ecs_os_time_osx_start;
#else /* anything else, this will need more care for non-Linux platforms */
#include <time.h>
static uint64_t _ecs_os_time_posix_start;
#endif

/* prevent 64-bit overflow when computing relative timestamp
    see https://gist.github.com/jspohr/3dc4f00033d79ec5bdaf67bc46c813e3
*/
#if defined(_WIN32) || (defined(__APPLE__) && defined(__MACH__))
int64_t int64_muldiv(int64_t value, int64_t numer, int64_t denom) {
    int64_t q = value / denom;
    int64_t r = value % denom;
    return q * numer + r * numer / denom;
}
#endif

void ecs_os_time_setup(void) {
    if ( ecs_os_time_initialized) {
        return;
    }
    
    ecs_os_time_initialized = 1;
    #if defined(_WIN32)
        LARGE_INTEGER freq;
        QueryPerformanceFrequency(&freq);
        QueryPerformanceCounter(&_ecs_os_time_win_start);
        _ecs_os_time_win_freq = (double)freq.QuadPart / 1000000000.0;
    #elif defined(__APPLE__) && defined(__MACH__)
        mach_timebase_info(&_ecs_os_time_osx_timebase);
        _ecs_os_time_osx_start = mach_absolute_time();
    #else
        struct timespec ts;
        clock_gettime(CLOCK_MONOTONIC, &ts);
        _ecs_os_time_posix_start = (uint64_t)ts.tv_sec*1000000000 + (uint64_t)ts.tv_nsec; 
    #endif
}

uint64_t ecs_os_time_now(void) {
    ecs_assert(ecs_os_time_initialized != 0, ECS_INTERNAL_ERROR, NULL);

    uint64_t now;

    #if defined(_WIN32)
        LARGE_INTEGER qpc_t;
        QueryPerformanceCounter(&qpc_t);
        now = qpc_t.QuadPart / _ecs_os_time_win_freq;
    #elif defined(__APPLE__) && defined(__MACH__)
        now = mach_absolute_time();
    #else
        struct timespec ts;
        clock_gettime(CLOCK_MONOTONIC, &ts);
        now = ((uint64_t)ts.tv_sec * 1000000000 + (uint64_t)ts.tv_nsec);
    #endif

    return now;
}

void ecs_os_time_sleep(
    int32_t sec, 
    int32_t nanosec) 
{
#ifndef _WIN32
    struct timespec sleepTime;

    sleepTime.tv_sec = sec;
    sleepTime.tv_nsec = nanosec;
    if (nanosleep(&sleepTime, NULL)) {
        ecs_os_err("nanosleep failed");
    }
#else
    HANDLE timer;
    LARGE_INTEGER ft;

    ft.QuadPart = -((int64_t)sec * 10000000 + (int64_t)nanosec / 100);

    timer = CreateWaitableTimer(NULL, TRUE, NULL);
    SetWaitableTimer(timer, &ft, 0, NULL, NULL, 0);
    WaitForSingleObject(timer, INFINITE);
    CloseHandle(timer);
#endif
}
