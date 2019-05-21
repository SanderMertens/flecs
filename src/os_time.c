
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

#include "include/private/flecs.h"

#ifndef SOKOL_API_DECL
    #define SOKOL_API_DECL extern
#endif


#ifndef SOKOL_API_IMPL
    #define SOKOL_API_IMPL
#endif
#ifndef SOKOL_ASSERT
    #include <assert.h>
    #define SOKOL_ASSERT(c) assert(c)
#endif
#ifndef _SOKOL_PRIVATE
    #if defined(__GNUC__)
        #define _SOKOL_PRIVATE __attribute__((unused)) static
    #else
        #define _SOKOL_PRIVATE static
    #endif
#endif

static int _ecs_os_time_initialized;
#if defined(_WIN32)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
static LARGE_INTEGER _ecs_os_time_win_freq;
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
_SOKOL_PRIVATE int64_t int64_muldiv(int64_t value, int64_t numer, int64_t denom) {
    int64_t q = value / denom;
    int64_t r = value % denom;
    return q * numer + r * numer / denom;
}
#endif


SOKOL_API_IMPL void ecs_os_time_setup(void) {
    SOKOL_ASSERT(0 == _ecs_os_time_initialized);
    _ecs_os_time_initialized = 1;
    #if defined(_WIN32)
        QueryPerformanceFrequency(&_ecs_os_time_win_freq);
        QueryPerformanceCounter(&_ecs_os_time_win_start);
    #elif defined(__APPLE__) && defined(__MACH__)
        mach_timebase_info(&_ecs_os_time_osx_timebase);
        _ecs_os_time_osx_start = mach_absolute_time();
    #else
        struct timespec ts;
        clock_gettime(CLOCK_MONOTONIC, &ts);
        _ecs_os_time_posix_start = (uint64_t)ts.tv_sec*1000000000 + (uint64_t)ts.tv_nsec; 
    #endif
}

SOKOL_API_IMPL uint64_t ecs_os_time_now(void) {
    SOKOL_ASSERT(_ecs_os_time_initialized);
    uint64_t now;
    #if defined(_WIN32)
        LARGE_INTEGER qpc_t;
        QueryPerformanceCounter(&qpc_t);
        now = int64_muldiv(qpc_t.QuadPart - _ecs_os_time_win_start.QuadPart, 1000000000, _ecs_os_time_win_freq.QuadPart);
    #elif defined(__APPLE__) && defined(__MACH__)
        const uint64_t mach_now = mach_absolute_time() - _ecs_os_time_osx_start;
        now = int64_muldiv(mach_now, _ecs_os_time_osx_timebase.numer, _ecs_os_time_osx_timebase.denom);
    #else
        struct timespec ts;
        clock_gettime(CLOCK_MONOTONIC, &ts);
        now = ((uint64_t)ts.tv_sec*1000000000 + (uint64_t)ts.tv_nsec) - _ecs_os_time_posix_start;
    #endif
    return now;
}

SOKOL_API_IMPL uint64_t ecs_os_time_diff(uint64_t new_ticks, uint64_t old_ticks) {
    if (new_ticks > old_ticks) {
        return new_ticks - old_ticks;
    }
    else {
        /* FIXME: this should be a value that converts to a non-null double */
        return 1;
    }
}

SOKOL_API_IMPL uint64_t ecs_os_time_since(uint64_t start_ticks) {
    return ecs_os_time_diff(ecs_os_time_now(), start_ticks);
}

SOKOL_API_IMPL uint64_t ecs_os_time_laptime(uint64_t* last_time) {
    SOKOL_ASSERT(last_time);
    uint64_t dt = 0;
    uint64_t now = ecs_os_time_now();
    if (0 != *last_time) {
        dt = ecs_os_time_diff(now, *last_time);
    }
    *last_time = now;
    return dt;
}

SOKOL_API_IMPL double ecs_os_time_sec(uint64_t ticks) {
    return (double)ticks / 1000000000.0;
}

SOKOL_API_IMPL double ecs_os_time_ms(uint64_t ticks) {
    return (double)ticks / 1000000.0;
}

SOKOL_API_IMPL double ecs_os_time_us(uint64_t ticks) {
    return (double)ticks / 1000.0;
}

SOKOL_API_IMPL double ecs_os_time_ns(uint64_t ticks) {
    return (double)ticks;
}