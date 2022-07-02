#include "pthread.h"

#if defined(__APPLE__) && defined(__MACH__)
#include <mach/mach_time.h>
#elif defined(__EMSCRIPTEN__)
#include <emscripten.h>
#else
#include <time.h>
#endif

static
ecs_os_thread_t posix_thread_new(
    ecs_os_thread_callback_t callback, 
    void *arg)
{
    pthread_t *thread = ecs_os_malloc(sizeof(pthread_t));

    if (pthread_create (thread, NULL, callback, arg) != 0) {
        ecs_os_abort();
    }

    return (ecs_os_thread_t)(uintptr_t)thread;
}

static
void* posix_thread_join(
    ecs_os_thread_t thread)
{
    void *arg;
    pthread_t *thr = (pthread_t*)(uintptr_t)thread;
    pthread_join(*thr, &arg);
    ecs_os_free(thr);
    return arg;
}

static
int32_t posix_ainc(
    int32_t *count)
{
    int value;
#ifdef __GNUC__
    value = __sync_add_and_fetch (count, 1);
    return value;
#else
    /* Unsupported */
    abort();
#endif
}

static
int32_t posix_adec(
    int32_t *count) 
{
    int value;
#ifdef __GNUC__
    value = __sync_sub_and_fetch (count, 1);
    return value;
#else
    /* Unsupported */
    abort();
#endif
}

static
ecs_os_mutex_t posix_mutex_new(void) {
    pthread_mutex_t *mutex = ecs_os_malloc(sizeof(pthread_mutex_t));
    if (pthread_mutex_init(mutex, NULL)) {
        abort();
    }
    return (ecs_os_mutex_t)(uintptr_t)mutex;
}

static
void posix_mutex_free(
    ecs_os_mutex_t m) 
{
    pthread_mutex_t *mutex = (pthread_mutex_t*)(intptr_t)m;
    pthread_mutex_destroy(mutex);
    ecs_os_free(mutex);
}

static
void posix_mutex_lock(
    ecs_os_mutex_t m) 
{
    pthread_mutex_t *mutex = (pthread_mutex_t*)(intptr_t)m;
    if (pthread_mutex_lock(mutex)) {
        abort();
    }
}

static
void posix_mutex_unlock(
    ecs_os_mutex_t m) 
{
    pthread_mutex_t *mutex = (pthread_mutex_t*)(intptr_t)m;
    if (pthread_mutex_unlock(mutex)) {
        abort();
    }
}

static
ecs_os_cond_t posix_cond_new(void) {
    pthread_cond_t *cond = ecs_os_malloc(sizeof(pthread_cond_t));
    if (pthread_cond_init(cond, NULL)) {
        abort();
    }
    return (ecs_os_cond_t)(uintptr_t)cond;
}

static 
void posix_cond_free(
    ecs_os_cond_t c) 
{
    pthread_cond_t *cond = (pthread_cond_t*)(intptr_t)c;
    if (pthread_cond_destroy(cond)) {
        abort();
    }
    ecs_os_free(cond);
}

static 
void posix_cond_signal(
    ecs_os_cond_t c) 
{
    pthread_cond_t *cond = (pthread_cond_t*)(intptr_t)c;
    if (pthread_cond_signal(cond)) {
        abort();
    }
}

static 
void posix_cond_broadcast(
    ecs_os_cond_t c) 
{
    pthread_cond_t *cond = (pthread_cond_t*)(intptr_t)c;
    if (pthread_cond_broadcast(cond)) {
        abort();
    }
}

static 
void posix_cond_wait(
    ecs_os_cond_t c, 
    ecs_os_mutex_t m) 
{
    pthread_cond_t *cond = (pthread_cond_t*)(intptr_t)c;
    pthread_mutex_t *mutex = (pthread_mutex_t*)(intptr_t)m;
    if (pthread_cond_wait(cond, mutex)) {
        abort();
    }
}

static bool posix_time_initialized;

#if defined(__APPLE__) && defined(__MACH__)
static mach_timebase_info_data_t posix_osx_timebase;
static uint64_t posix_time_start;
#else
static uint64_t posix_time_start;
#endif

static
void posix_time_setup(void) {
    if (posix_time_initialized) {
        return;
    }
    
    posix_time_initialized = true;

    #if defined(__APPLE__) && defined(__MACH__)
        mach_timebase_info(&posix_osx_timebase);
        posix_time_start = mach_absolute_time();
    #else
        struct timespec ts;
        clock_gettime(CLOCK_MONOTONIC, &ts);
        posix_time_start = (uint64_t)ts.tv_sec*1000000000 + (uint64_t)ts.tv_nsec; 
    #endif
}

static
void posix_sleep(
    int32_t sec, 
    int32_t nanosec) 
{
    struct timespec sleepTime;
    ecs_assert(sec >= 0, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(nanosec >= 0, ECS_INTERNAL_ERROR, NULL);

    sleepTime.tv_sec = sec;
    sleepTime.tv_nsec = nanosec;
    if (nanosleep(&sleepTime, NULL)) {
        ecs_err("nanosleep failed");
    }
}

/* prevent 64-bit overflow when computing relative timestamp
    see https://gist.github.com/jspohr/3dc4f00033d79ec5bdaf67bc46c813e3
*/
#if defined(ECS_TARGET_DARWIN)
static
int64_t posix_int64_muldiv(int64_t value, int64_t numer, int64_t denom) {
    int64_t q = value / denom;
    int64_t r = value % denom;
    return q * numer + r * numer / denom;
}
#endif

static
uint64_t posix_time_now(void) {
    ecs_assert(posix_time_initialized != 0, ECS_INTERNAL_ERROR, NULL);

    uint64_t now;

    #if defined(ECS_TARGET_DARWIN)
        now = (uint64_t) posix_int64_muldiv(
            (int64_t)mach_absolute_time(), 
            (int64_t)posix_osx_timebase.numer, 
            (int64_t)posix_osx_timebase.denom);
    #elif defined(__EMSCRIPTEN__)
        now = (long long)(emscripten_get_now() * 1000.0 * 1000);
    #else
        struct timespec ts;
        clock_gettime(CLOCK_MONOTONIC, &ts);
        now = ((uint64_t)ts.tv_sec * 1000 * 1000 * 1000 + (uint64_t)ts.tv_nsec);
    #endif

    return now;
}

void ecs_set_os_api_impl(void) {
    ecs_os_set_api_defaults();

    ecs_os_api_t api = ecs_os_api;

    api.thread_new_ = posix_thread_new;
    api.thread_join_ = posix_thread_join;
    api.ainc_ = posix_ainc;
    api.adec_ = posix_adec;
    api.mutex_new_ = posix_mutex_new;
    api.mutex_free_ = posix_mutex_free;
    api.mutex_lock_ = posix_mutex_lock;
    api.mutex_unlock_ = posix_mutex_unlock;
    api.cond_new_ = posix_cond_new;
    api.cond_free_ = posix_cond_free;
    api.cond_signal_ = posix_cond_signal;
    api.cond_broadcast_ = posix_cond_broadcast;
    api.cond_wait_ = posix_cond_wait;
    api.sleep_ = posix_sleep;
    api.now_ = posix_time_now;

    posix_time_setup();

    ecs_os_set_api(&api);
}
