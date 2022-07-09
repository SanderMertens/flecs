/**
 * @file os_api.h
 * @brief Operating system abstraction API.
 *
 * This file contains the operating system abstraction API. The flecs core 
 * library avoids OS/runtime specific API calls as much as possible. Instead it
 * provides an interface that can be implemented by applications.
 *
 * Examples for how to implement this interface can be found in the 
 * examples/os_api folder.
 */

#ifndef FLECS_OS_API_H
#define FLECS_OS_API_H

#include <stdarg.h>
#include <errno.h>

#if defined(ECS_TARGET_WINDOWS)
#include <malloc.h>
#elif defined(ECS_TARGET_FREEBSD)
#include <stdlib.h>
#else
#include <alloca.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ecs_time_t {
    uint32_t sec;
    uint32_t nanosec;
} ecs_time_t;

/* Allocation counters (not thread safe) */
extern int64_t ecs_os_api_malloc_count;
extern int64_t ecs_os_api_realloc_count;
extern int64_t ecs_os_api_calloc_count;
extern int64_t ecs_os_api_free_count;

/* Use handle types that _at least_ can store pointers */
typedef uintptr_t ecs_os_thread_t;
typedef uintptr_t ecs_os_cond_t;
typedef uintptr_t ecs_os_mutex_t;
typedef uintptr_t ecs_os_dl_t;
typedef uintptr_t ecs_os_sock_t;

/* Generic function pointer type */
typedef void (*ecs_os_proc_t)(void);

/* OS API init */
typedef 
void (*ecs_os_api_init_t)(void);

/* OS API deinit */
typedef 
void (*ecs_os_api_fini_t)(void);

/* Memory management */
typedef 
void* (*ecs_os_api_malloc_t)(
    ecs_size_t size);

typedef 
void (*ecs_os_api_free_t)(
    void *ptr);

typedef
void* (*ecs_os_api_realloc_t)(
    void *ptr, 
    ecs_size_t size);

typedef
void* (*ecs_os_api_calloc_t)(
    ecs_size_t size);

typedef
char* (*ecs_os_api_strdup_t)(
    const char *str);

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

/* Atomic increment / decrement */
typedef
int (*ecs_os_api_ainc_t)(
    int32_t *value);


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
    int32_t sec,
    int32_t nanosec);

typedef 
void (*ecs_os_api_enable_high_timer_resolution_t)(
    bool enable);

typedef
void (*ecs_os_api_get_time_t)(
    ecs_time_t *time_out);

typedef
uint64_t (*ecs_os_api_now_t)(void);

/* Logging */
typedef
void (*ecs_os_api_log_t)(
    int32_t level,     /* Logging level */
    const char *file,  /* File where message was logged */
    int32_t line,      /* Line it was logged */
    const char *msg);

/* Application termination */
typedef
void (*ecs_os_api_abort_t)(
    void);

/* Dynamic libraries */
typedef
ecs_os_dl_t (*ecs_os_api_dlopen_t)(
    const char *libname);

typedef
ecs_os_proc_t (*ecs_os_api_dlproc_t)(
    ecs_os_dl_t lib,
    const char *procname);

typedef
void (*ecs_os_api_dlclose_t)(
    ecs_os_dl_t lib);

typedef
char* (*ecs_os_api_module_to_path_t)(
    const char *module_id);

/* Prefix members of struct with 'ecs_' as some system headers may define 
 * macros for functions like "strdup", "log" or "_free" */

typedef struct ecs_os_api_t {
    /* API init / deinit */
    ecs_os_api_init_t init_;
    ecs_os_api_fini_t fini_;

    /* Memory management */
    ecs_os_api_malloc_t malloc_;
    ecs_os_api_realloc_t realloc_;
    ecs_os_api_calloc_t calloc_;
    ecs_os_api_free_t free_;

    /* Strings */
    ecs_os_api_strdup_t strdup_;

    /* Threads */
    ecs_os_api_thread_new_t thread_new_;
    ecs_os_api_thread_join_t thread_join_;

    /* Atomic incremenet / decrement */
    ecs_os_api_ainc_t ainc_;
    ecs_os_api_ainc_t adec_;

    /* Mutex */
    ecs_os_api_mutex_new_t mutex_new_;
    ecs_os_api_mutex_free_t mutex_free_;
    ecs_os_api_mutex_lock_t mutex_lock_;
    ecs_os_api_mutex_lock_t mutex_unlock_;

    /* Condition variable */
    ecs_os_api_cond_new_t cond_new_;
    ecs_os_api_cond_free_t cond_free_;
    ecs_os_api_cond_signal_t cond_signal_;
    ecs_os_api_cond_broadcast_t cond_broadcast_;
    ecs_os_api_cond_wait_t cond_wait_;

    /* Time */
    ecs_os_api_sleep_t sleep_;
    ecs_os_api_now_t now_;
    ecs_os_api_get_time_t get_time_;

    /* Logging */
    ecs_os_api_log_t log_; /* Logging function. The level should be interpreted as: */
                           /* >0: Debug tracing. Only enabled in debug builds. */
                           /*  0: Tracing. Enabled in debug/release builds. */
                           /* -2: Warning. An issue occurred, but operation was successful. */
                           /* -3: Error. An issue occurred, and operation was unsuccessful. */
                           /* -4: Fatal. An issue occurred, and application must quit. */

    /* Application termination */
    ecs_os_api_abort_t abort_;

    /* Dynamic library loading */
    ecs_os_api_dlopen_t dlopen_;
    ecs_os_api_dlproc_t dlproc_;
    ecs_os_api_dlclose_t dlclose_;

    /* Overridable function that translates from a logical module id to a
     * shared library filename */
    ecs_os_api_module_to_path_t module_to_dl_;

    /* Overridable function that translates from a logical module id to a
     * path that contains module-specif resources or assets */
    ecs_os_api_module_to_path_t module_to_etc_;

    /* Trace level */
    int32_t log_level_;

    /* Trace indentation */
    int32_t log_indent_;

    /* Last error code */
    int32_t log_last_error_;

    /* OS API flags */
    ecs_flags32_t flags_;
} ecs_os_api_t;

FLECS_API
extern ecs_os_api_t ecs_os_api;

FLECS_API
void ecs_os_init(void);

FLECS_API
void ecs_os_fini(void);

FLECS_API
void ecs_os_set_api(
    ecs_os_api_t *os_api);

FLECS_API
void ecs_os_set_api_defaults(void);

/* Memory management */
#ifndef ecs_os_malloc
#define ecs_os_malloc(size) ecs_os_api.malloc_(size)
#endif
#ifndef ecs_os_free
#define ecs_os_free(ptr) ecs_os_api.free_(ptr)
#endif
#ifndef ecs_os_realloc
#define ecs_os_realloc(ptr, size) ecs_os_api.realloc_(ptr, size)
#endif
#ifndef ecs_os_calloc
#define ecs_os_calloc(size) ecs_os_api.calloc_(size)
#endif
#if defined(ECS_TARGET_WINDOWS)
#define ecs_os_alloca(size) _alloca((size_t)(size))
#else
#define ecs_os_alloca(size) alloca((size_t)(size))
#endif

#define ecs_os_malloc_t(T) ECS_CAST(T*, ecs_os_malloc(ECS_SIZEOF(T)))
#define ecs_os_malloc_n(T, count) ECS_CAST(T*, ecs_os_malloc(ECS_SIZEOF(T) * (count)))
#define ecs_os_calloc_t(T) ECS_CAST(T*, ecs_os_calloc(ECS_SIZEOF(T)))
#define ecs_os_calloc_n(T, count) ECS_CAST(T*, ecs_os_calloc(ECS_SIZEOF(T) * (count)))

#define ecs_os_realloc_t(ptr, T) ECS_CAST(T*, ecs_os_realloc([ptr, ECS_SIZEOF(T)))
#define ecs_os_realloc_n(ptr, T, count) ECS_CAST(T*, ecs_os_realloc(ptr, ECS_SIZEOF(T) * (count)))
#define ecs_os_alloca_t(T) ECS_CAST(T*, ecs_os_alloca(ECS_SIZEOF(T)))
#define ecs_os_alloca_n(T, count) ECS_CAST(T*, ecs_os_alloca(ECS_SIZEOF(T) * (count)))

/* Strings */
#ifndef ecs_os_strdup
#define ecs_os_strdup(str) ecs_os_api.strdup_(str)
#endif

#define ecs_os_strset(dst, src) ecs_os_free(*dst); *dst = ecs_os_strdup(src)

#ifdef __cplusplus
#define ecs_os_strlen(str) static_cast<ecs_size_t>(strlen(str))
#define ecs_os_strncmp(str1, str2, num) strncmp(str1, str2, static_cast<size_t>(num))
#define ecs_os_memcmp(ptr1, ptr2, num) memcmp(ptr1, ptr2, static_cast<size_t>(num))
#define ecs_os_memcpy(ptr1, ptr2, num) memcpy(ptr1, ptr2, static_cast<size_t>(num))
#define ecs_os_memset(ptr, value, num) memset(ptr, value, static_cast<size_t>(num))
#define ecs_os_memmove(dst, src, size) memmove(dst, src, static_cast<size_t>(size))
#else
#define ecs_os_strlen(str) (ecs_size_t)strlen(str)
#define ecs_os_strncmp(str1, str2, num) strncmp(str1, str2, (size_t)(num))
#define ecs_os_memcmp(ptr1, ptr2, num) memcmp(ptr1, ptr2, (size_t)(num))
#define ecs_os_memcpy(ptr1, ptr2, num) memcpy(ptr1, ptr2, (size_t)(num))
#define ecs_os_memset(ptr, value, num) memset(ptr, value, (size_t)(num))
#define ecs_os_memmove(dst, src, size) memmove(dst, src, (size_t)(size))
#endif

#define ecs_os_memcpy_t(ptr1, ptr2, T) ecs_os_memcpy(ptr1, ptr2, ECS_SIZEOF(T))
#define ecs_os_memcpy_n(ptr1, ptr2, T, count) ecs_os_memcpy(ptr1, ptr2, ECS_SIZEOF(T) * count)
#define ecs_os_memcmp_t(ptr1, ptr2, T) ecs_os_memcmp(ptr1, ptr2, ECS_SIZEOF(T))

#define ecs_os_strcmp(str1, str2) strcmp(str1, str2)
#define ecs_os_memset_t(ptr, value, T) ecs_os_memset(ptr, value, ECS_SIZEOF(T))
#define ecs_os_memset_n(ptr, value, T, count) ecs_os_memset(ptr, value, ECS_SIZEOF(T) * count)
#define ecs_os_zeromem(ptr) ecs_os_memset(ptr, 0, ECS_SIZEOF(*ptr))

#define ecs_os_memdup_t(ptr, T) ecs_os_memdup(ptr, ECS_SIZEOF(T))
#define ecs_os_memdup_n(ptr, T, count) ecs_os_memdup(ptr, ECS_SIZEOF(T) * count)

#define ecs_offset(ptr, T, index)\
    ECS_CAST(T*, ECS_OFFSET(ptr, ECS_SIZEOF(T) * index))

#if defined(ECS_TARGET_MSVC)
#define ecs_os_strcat(str1, str2) strcat_s(str1, INT_MAX, str2)
#define ecs_os_sprintf(ptr, ...) sprintf_s(ptr, INT_MAX, __VA_ARGS__)
#define ecs_os_vsprintf(ptr, fmt, args) vsprintf_s(ptr, INT_MAX, fmt, args)
#define ecs_os_strcpy(str1, str2) strcpy_s(str1, INT_MAX, str2)
#ifdef __cplusplus
#define ecs_os_strncpy(str1, str2, num) strncpy_s(str1, INT_MAX, str2, static_cast<size_t>(num))
#else
#define ecs_os_strncpy(str1, str2, num) strncpy_s(str1, INT_MAX, str2, (size_t)(num))
#endif
#else
#define ecs_os_strcat(str1, str2) strcat(str1, str2)
#define ecs_os_sprintf(ptr, ...) sprintf(ptr, __VA_ARGS__)
#define ecs_os_vsprintf(ptr, fmt, args) vsprintf(ptr, fmt, args)
#define ecs_os_strcpy(str1, str2) strcpy(str1, str2)
#ifdef __cplusplus
#define ecs_os_strncpy(str1, str2, num) strncpy(str1, str2, static_cast<size_t>(num))
#else
#define ecs_os_strncpy(str1, str2, num) strncpy(str1, str2, (size_t)(num))
#endif
#endif

/* Files */
#if defined(ECS_TARGET_MSVC)
#define ecs_os_fopen(result, file, mode) fopen_s(result, file, mode)
#else
#define ecs_os_fopen(result, file, mode) (*(result)) = fopen(file, mode)
#endif

/* Threads */
#define ecs_os_thread_new(callback, param) ecs_os_api.thread_new_(callback, param)
#define ecs_os_thread_join(thread) ecs_os_api.thread_join_(thread)

/* Atomic increment / decrement */
#define ecs_os_ainc(value) ecs_os_api.ainc_(value)
#define ecs_os_adec(value) ecs_os_api.adec_(value)

/* Mutex */
#define ecs_os_mutex_new() ecs_os_api.mutex_new_()
#define ecs_os_mutex_free(mutex) ecs_os_api.mutex_free_(mutex)
#define ecs_os_mutex_lock(mutex) ecs_os_api.mutex_lock_(mutex)
#define ecs_os_mutex_unlock(mutex) ecs_os_api.mutex_unlock_(mutex)

/* Condition variable */
#define ecs_os_cond_new() ecs_os_api.cond_new_()
#define ecs_os_cond_free(cond) ecs_os_api.cond_free_(cond)
#define ecs_os_cond_signal(cond) ecs_os_api.cond_signal_(cond)
#define ecs_os_cond_broadcast(cond) ecs_os_api.cond_broadcast_(cond)
#define ecs_os_cond_wait(cond, mutex) ecs_os_api.cond_wait_(cond, mutex)

/* Time */
#define ecs_os_sleep(sec, nanosec) ecs_os_api.sleep_(sec, nanosec)
#define ecs_os_now() ecs_os_api.now_()
#define ecs_os_get_time(time_out) ecs_os_api.get_time_(time_out)

/* Logging */
FLECS_API
void ecs_os_dbg(const char *file, int32_t line, const char *msg);

FLECS_API
void ecs_os_trace(const char *file, int32_t line, const char *msg);

FLECS_API
void ecs_os_warn(const char *file, int32_t line, const char *msg);

FLECS_API
void ecs_os_err(const char *file, int32_t line, const char *msg);

FLECS_API
void ecs_os_fatal(const char *file, int32_t line, const char *msg);

FLECS_API
const char* ecs_os_strerror(int err);

/* Application termination */
#define ecs_os_abort() ecs_os_api.abort_()

/* Dynamic libraries */
#define ecs_os_dlopen(libname) ecs_os_api.dlopen_(libname)
#define ecs_os_dlproc(lib, procname) ecs_os_api.dlproc_(lib, procname)
#define ecs_os_dlclose(lib) ecs_os_api.dlclose_(lib)

/* Module id translation */
#define ecs_os_module_to_dl(lib) ecs_os_api.module_to_dl_(lib)
#define ecs_os_module_to_etc(lib) ecs_os_api.module_to_etc_(lib)

/* Sleep with floating point time */
FLECS_API
void ecs_sleepf(
    double t);

/* Measure time since provided timestamp */
FLECS_API
double ecs_time_measure(
    ecs_time_t *start);

/* Calculate difference between two timestamps */
FLECS_API
ecs_time_t ecs_time_sub(
    ecs_time_t t1,
    ecs_time_t t2);

/* Convert time value to a double */
FLECS_API
double ecs_time_to_double(
    ecs_time_t t);

FLECS_API
void* ecs_os_memdup(
    const void *src, 
    ecs_size_t size);

/** Are heap functions available? */
FLECS_API
bool ecs_os_has_heap(void);

/** Are threading functions available? */
FLECS_API
bool ecs_os_has_threading(void);

/** Are time functions available? */
FLECS_API
bool ecs_os_has_time(void);

/** Are logging functions available? */
FLECS_API
bool ecs_os_has_logging(void);

/** Are dynamic library functions available? */
FLECS_API
bool ecs_os_has_dl(void);

/** Are module path functions available? */
FLECS_API
bool ecs_os_has_modules(void);

#ifdef __cplusplus
}
#endif

#endif
