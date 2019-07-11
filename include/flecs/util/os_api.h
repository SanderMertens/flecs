#ifndef FLECS_OS_API_H
#define FLECS_OS_API_H

#include <stdarg.h>

#if defined(_MSC_VER) || defined(__MINGW32__)
#include <malloc.h>
#else
#include <alloca.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Use handle types that _at least_ can store pointers */
typedef uintptr_t ecs_os_thread_t;
typedef uintptr_t ecs_os_cond_t;
typedef uintptr_t ecs_os_mutex_t;

/* Memory management */
typedef 
void* (*ecs_os_api_malloc_t)(
    size_t size);

typedef 
void (*ecs_os_api_free_t)(
    void *ptr);

typedef
void* (*ecs_os_api_realloc_t)(
    void *ptr, 
    size_t size);

typedef
void* (*ecs_os_api_calloc_t)(
    size_t num,
    size_t size);


/* Threads */
typedef
void* (*ecs_os_thread_callback_t)(
    void*);

typedef
ecs_os_thread_t (*ecs_os_api_thread_new_t)(
    ecs_os_thread_callback_t callback,
    void *param);

typedef
void* (*ecs_os_api_thread_join_t)(
    ecs_os_thread_t thread);


/* Mutex */
typedef
ecs_os_mutex_t (*ecs_os_api_mutex_new_t)(
    void);

typedef
void (*ecs_os_api_mutex_lock_t)(
    ecs_os_mutex_t mutex);

typedef
void (*ecs_os_api_mutex_unlock_t)(
    ecs_os_mutex_t mutex);

typedef
void (*ecs_os_api_mutex_free_t)(
    ecs_os_mutex_t mutex);

/* Condition variable */
typedef
ecs_os_cond_t (*ecs_os_api_cond_new_t)(
    void);

typedef
void (*ecs_os_api_cond_free_t)(
    ecs_os_cond_t cond);

typedef
void (*ecs_os_api_cond_signal_t)(
    ecs_os_cond_t cond);

typedef
void (*ecs_os_api_cond_broadcast_t)(
    ecs_os_cond_t cond);

typedef
void (*ecs_os_api_cond_wait_t)(
    ecs_os_cond_t cond,
    ecs_os_mutex_t mutex);


typedef 
void (*ecs_os_api_sleep_t)(
    uint32_t sec,
    uint32_t nanosec);

typedef
void (*ecs_os_api_get_time_t)(
    ecs_time_t *time_out);

/* Logging */
typedef
void (*ecs_os_api_log_t)(
    const char *fmt,
    va_list args);

/* Application termination */
typedef
void (*ecs_os_api_abort_t)(
    void);

typedef struct ecs_os_api_t {
    /* Memory management */
    ecs_os_api_malloc_t malloc;
    ecs_os_api_realloc_t realloc;
    ecs_os_api_calloc_t calloc;
    ecs_os_api_free_t free;

    /* Threads */
    ecs_os_api_thread_new_t thread_new;
    ecs_os_api_thread_join_t thread_join;

    /* Mutex */
    ecs_os_api_mutex_new_t mutex_new;
    ecs_os_api_mutex_free_t mutex_free;
    ecs_os_api_mutex_lock_t mutex_lock;
    ecs_os_api_mutex_lock_t mutex_unlock;

    /* Condition variable */
    ecs_os_api_cond_new_t cond_new;
    ecs_os_api_cond_free_t cond_free;
    ecs_os_api_cond_signal_t cond_signal;
    ecs_os_api_cond_broadcast_t cond_broadcast;
    ecs_os_api_cond_wait_t cond_wait;

    /* Time */
    ecs_os_api_sleep_t sleep;
    ecs_os_api_get_time_t get_time;

    /* Logging */
    ecs_os_api_log_t log;
    ecs_os_api_log_t log_error;
    ecs_os_api_log_t log_debug;
    ecs_os_api_log_t log_warning;

    /* Application termination */
    ecs_os_api_abort_t abort;
} ecs_os_api_t;

FLECS_EXPORT
extern const ecs_os_api_t ecs_os_api;

FLECS_EXPORT
void ecs_os_set_api(
    ecs_os_api_t *os_api);

FLECS_EXPORT
void ecs_os_set_api_defaults(void);

/* Memory management */
#define ecs_os_malloc(size) ecs_os_api.malloc(size);
#define ecs_os_free(ptr) ecs_os_api.free(ptr);
#define ecs_os_realloc(ptr, size) ecs_os_api.realloc(ptr, size)
#define ecs_os_calloc(num, size) ecs_os_api.calloc(num, size)

#if defined(_MSC_VER) || defined(__MINGW32__)
#define ecs_os_alloca(type, count) _alloca(sizeof(type) * (count))
#else
#define ecs_os_alloca(type, count) alloca(sizeof(type) * (count))
#endif

/* Threads */
#define ecs_os_thread_new(callback, param) ecs_os_api.thread_new(callback, param)
#define ecs_os_thread_join(thread) ecs_os_api.thread_join(thread)

/* Mutex */
#define ecs_os_mutex_new() ecs_os_api.mutex_new()
#define ecs_os_mutex_free(mutex) ecs_os_api.mutex_free(mutex)
#define ecs_os_mutex_lock(mutex) ecs_os_api.mutex_lock(mutex)
#define ecs_os_mutex_unlock(mutex) ecs_os_api.mutex_unlock(mutex)

/* Condition variable */
#define ecs_os_cond_new() ecs_os_api.cond_new()
#define ecs_os_cond_free(cond) ecs_os_api.cond_free(cond)
#define ecs_os_cond_signal(cond) ecs_os_api.cond_signal(cond)
#define ecs_os_cond_broadcast(cond) ecs_os_api.cond_broadcast(cond)
#define ecs_os_cond_wait(cond, mutex) ecs_os_api.cond_wait(cond, mutex)

/* Time */
#define ecs_os_sleep(sec, nanosec) ecs_os_api.sleep(sec, nanosec)
#define ecs_os_get_time(time_out) ecs_os_api.get_time(time_out)

/* Logging (use functions to avoid using variadic macro arguments) */
FLECS_EXPORT
void ecs_os_log(const char *fmt, ...);

FLECS_EXPORT
void ecs_os_warn(const char *fmt, ...);

FLECS_EXPORT
void ecs_os_err(const char *fmt, ...);

FLECS_EXPORT
void ecs_os_dbg(const char *fmt, ...);

FLECS_EXPORT
void ecs_os_enable_dbg(bool enable);

FLECS_EXPORT
bool ecs_os_dbg_enabled(void);

/* Application termination */
#define ecs_os_abort() ecs_os_api.abort()

/* Sleep with floating point time */
FLECS_EXPORT
void ecs_sleepf(
    double t);

/* Measure time since provided timestamp */
FLECS_EXPORT
double ecs_time_measure(
    ecs_time_t *start);

#ifdef __cplusplus
}
#endif

#endif
