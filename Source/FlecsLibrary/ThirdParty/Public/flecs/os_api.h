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

/**
 * @defgroup c_os_api OS API
 * @ingroup c
 * Interface for providing OS specific functionality.
 *
 * @{
 */

#include <stdarg.h>
#include <errno.h>
#include <stdio.h>

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

/** Time type. */
typedef struct ecs_time_t {
    uint32_t sec;                                 /**< Second part. */
    uint32_t nanosec;                             /**< Nanosecond part. */
} ecs_time_t;

/* Allocation counters */
extern int64_t ecs_os_api_malloc_count;            /**< malloc count. */
extern int64_t ecs_os_api_realloc_count;           /**< realloc count. */
extern int64_t ecs_os_api_calloc_count;            /**< calloc count. */
extern int64_t ecs_os_api_free_count;              /**< free count. */

/* Use handle types that _at least_ can store pointers */
typedef uintptr_t ecs_os_thread_t;                 /**< OS thread. */
typedef uintptr_t ecs_os_cond_t;                   /**< OS cond. */
typedef uintptr_t ecs_os_mutex_t;                  /**< OS mutex. */
typedef uintptr_t ecs_os_dl_t;                     /**< OS dynamic library. */
typedef uintptr_t ecs_os_sock_t;                   /**< OS socket. */

/** 64 bit thread id. */
typedef uint64_t ecs_os_thread_id_t;

/** Generic function pointer type. */
typedef void (*ecs_os_proc_t)(void);

/** OS API init. */
typedef
void (*ecs_os_api_init_t)(void);

/** OS API deinit. */
typedef
void (*ecs_os_api_fini_t)(void);

/** OS API malloc function type. */
typedef
void* (*ecs_os_api_malloc_t)(
    ecs_size_t size);

/** OS API free function type. */
typedef
void (*ecs_os_api_free_t)(
    void *ptr);

/** OS API realloc function type. */
typedef
void* (*ecs_os_api_realloc_t)(
    void *ptr,
    ecs_size_t size);

/** OS API calloc function type. */
typedef
void* (*ecs_os_api_calloc_t)(
    ecs_size_t size);

/** OS API strdup function type. */
typedef
char* (*ecs_os_api_strdup_t)(
    const char *str);

/** OS API thread_callback function type. */
typedef
void* (*ecs_os_thread_callback_t)(
    void*);

/** OS API thread_new function type. */
typedef
ecs_os_thread_t (*ecs_os_api_thread_new_t)(
    ecs_os_thread_callback_t callback,
    void *param);

/** OS API thread_join function type. */
typedef
void* (*ecs_os_api_thread_join_t)(
    ecs_os_thread_t thread);

/** OS API thread_self function type. */
typedef
ecs_os_thread_id_t (*ecs_os_api_thread_self_t)(void);

/** OS API task_new function type. */
typedef
ecs_os_thread_t (*ecs_os_api_task_new_t)(
    ecs_os_thread_callback_t callback,
    void *param);

/** OS API task_join function type. */
typedef
void* (*ecs_os_api_task_join_t)(
    ecs_os_thread_t thread);

/* Atomic increment / decrement */
/** OS API ainc function type. */
typedef
int32_t (*ecs_os_api_ainc_t)(
    int32_t *value);

/** OS API lainc function type. */
typedef
int64_t (*ecs_os_api_lainc_t)(
    int64_t *value);

/* Mutex */
/** OS API mutex_new function type. */
typedef
ecs_os_mutex_t (*ecs_os_api_mutex_new_t)(
    void);

/** OS API mutex_lock function type. */
typedef
void (*ecs_os_api_mutex_lock_t)(
    ecs_os_mutex_t mutex);

/** OS API mutex_unlock function type. */
typedef
void (*ecs_os_api_mutex_unlock_t)(
    ecs_os_mutex_t mutex);

/** OS API mutex_free function type. */
typedef
void (*ecs_os_api_mutex_free_t)(
    ecs_os_mutex_t mutex);

/* Condition variable */
/** OS API cond_new function type. */
typedef
ecs_os_cond_t (*ecs_os_api_cond_new_t)(
    void);

/** OS API cond_free function type. */
typedef
void (*ecs_os_api_cond_free_t)(
    ecs_os_cond_t cond);

/** OS API cond_signal function type. */
typedef
void (*ecs_os_api_cond_signal_t)(
    ecs_os_cond_t cond);

/** OS API cond_broadcast function type. */
typedef
void (*ecs_os_api_cond_broadcast_t)(
    ecs_os_cond_t cond);

/** OS API cond_wait function type. */
typedef
void (*ecs_os_api_cond_wait_t)(
    ecs_os_cond_t cond,
    ecs_os_mutex_t mutex);

/** OS API sleep function type. */
typedef
void (*ecs_os_api_sleep_t)(
    int32_t sec,
    int32_t nanosec);

/** OS API enable_high_timer_resolution function type. */
typedef
void (*ecs_os_api_enable_high_timer_resolution_t)(
    bool enable);

/** OS API get_time function type. */
typedef
void (*ecs_os_api_get_time_t)(
    ecs_time_t *time_out);

/** OS API now function type. */
typedef
uint64_t (*ecs_os_api_now_t)(void);

/** OS API log function type. */
typedef
void (*ecs_os_api_log_t)(
    int32_t level,     /* Logging level */
    const char *file,  /* File where message was logged */
    int32_t line,      /* Line it was logged */
    const char *msg);

/** OS API abort function type. */
typedef
void (*ecs_os_api_abort_t)(
    void);

/** OS API dlopen function type. */
typedef
ecs_os_dl_t (*ecs_os_api_dlopen_t)(
    const char *libname);

/** OS API dlproc function type. */
typedef
ecs_os_proc_t (*ecs_os_api_dlproc_t)(
    ecs_os_dl_t lib,
    const char *procname);

/** OS API dlclose function type. */
typedef
void (*ecs_os_api_dlclose_t)(
    ecs_os_dl_t lib);

/** OS API module_to_path function type. */
typedef
char* (*ecs_os_api_module_to_path_t)(
    const char *module_id);

/* Prefix members of struct with 'ecs_' as some system headers may define
 * macros for functions like "strdup", "log" or "_free" */

/** OS API interface. */
typedef struct ecs_os_api_t {
    /* API init / deinit */
    ecs_os_api_init_t init_;                       /**< init callback. */
    ecs_os_api_fini_t fini_;                       /**< fini callback. */

    /* Memory management */
    ecs_os_api_malloc_t malloc_;                   /**< malloc callback. */
    ecs_os_api_realloc_t realloc_;                 /**< realloc callback. */
    ecs_os_api_calloc_t calloc_;                   /**< calloc callback. */
    ecs_os_api_free_t free_;                       /**< free callback. */

    /* Strings */
    ecs_os_api_strdup_t strdup_;                   /**< strdup callback. */

    /* Threads */
    ecs_os_api_thread_new_t thread_new_;           /**< thread_new callback. */
    ecs_os_api_thread_join_t thread_join_;         /**< thread_join callback. */
    ecs_os_api_thread_self_t thread_self_;         /**< thread_self callback. */

    /* Tasks */
    ecs_os_api_thread_new_t task_new_;             /**< task_new callback. */
    ecs_os_api_thread_join_t task_join_;           /**< task_join callback. */

    /* Atomic increment / decrement */
    ecs_os_api_ainc_t ainc_;                       /**< ainc callback. */
    ecs_os_api_ainc_t adec_;                       /**< adec callback. */
    ecs_os_api_lainc_t lainc_;                     /**< lainc callback. */
    ecs_os_api_lainc_t ladec_;                     /**< ladec callback. */

    /* Mutex */
    ecs_os_api_mutex_new_t mutex_new_;             /**< mutex_new callback. */
    ecs_os_api_mutex_free_t mutex_free_;           /**< mutex_free callback. */
    ecs_os_api_mutex_lock_t mutex_lock_;           /**< mutex_lock callback. */
    ecs_os_api_mutex_lock_t mutex_unlock_;         /**< mutex_unlock callback. */

    /* Condition variable */
    ecs_os_api_cond_new_t cond_new_;               /**< cond_new callback. */
    ecs_os_api_cond_free_t cond_free_;             /**< cond_free callback. */
    ecs_os_api_cond_signal_t cond_signal_;         /**< cond_signal callback. */
    ecs_os_api_cond_broadcast_t cond_broadcast_;   /**< cond_broadcast callback. */
    ecs_os_api_cond_wait_t cond_wait_;             /**< cond_wait callback. */

    /* Time */
    ecs_os_api_sleep_t sleep_;                     /**< sleep callback. */
    ecs_os_api_now_t now_;                         /**< now callback. */
    ecs_os_api_get_time_t get_time_;               /**< get_time callback. */

    /* Logging */
    ecs_os_api_log_t log_; /**< log callback.
                            * The level should be interpreted as:
                            * >0: Debug tracing. Only enabled in debug builds.
                            *  0: Tracing. Enabled in debug/release builds.
                            * -2: Warning. An issue occurred, but operation was successful.
                            * -3: Error. An issue occurred, and operation was unsuccessful.
                            * -4: Fatal. An issue occurred, and application must quit. */

    /* Application termination */
    ecs_os_api_abort_t abort_;                     /**< abort callback. */

    /* Dynamic library loading */
    ecs_os_api_dlopen_t dlopen_;                   /**< dlopen callback. */
    ecs_os_api_dlproc_t dlproc_;                   /**< dlproc callback. */
    ecs_os_api_dlclose_t dlclose_;                 /**< dlclose callback. */

    /* Overridable function that translates from a logical module id to a
     * shared library filename */
    ecs_os_api_module_to_path_t module_to_dl_;     /**< module_to_dl callback. */

    /* Overridable function that translates from a logical module id to a
     * path that contains module-specif resources or assets */
    ecs_os_api_module_to_path_t module_to_etc_;    /**< module_to_etc callback. */

    int32_t log_level_;                            /**< Tracing level. */
    int32_t log_indent_;                           /**< Tracing indentation level. */
    int32_t log_last_error_;                       /**< Last logged error code. */
    int64_t log_last_timestamp_;                   /**< Last logged timestamp. */

    ecs_flags32_t flags_;                          /**< OS API flags */

    FILE *log_out_;                                /**< File used for logging output 
                                                    * (hint, log_ decides where to write) */
} ecs_os_api_t;

/** Static OS API variable with configured callbacks. */
FLECS_API
extern ecs_os_api_t ecs_os_api;

/** Initialize OS API. 
 * This operation is not usually called by an application. To override callbacks
 * of the OS API, use the following pattern:
 * 
 * @code
 * ecs_os_set_api_defaults();
 * ecs_os_api_t os_api = ecs_os_get_api();
 * os_api.abort_ = my_abort;
 * ecs_os_set_api(&os_api);
 * @endcode
 */
FLECS_API
void ecs_os_init(void);

/** Deinitialize OS API. 
 * This operation is not usually called by an application.
 */
FLECS_API
void ecs_os_fini(void);

/** Override OS API.
 * This overrides the OS API struct with new values for callbacks. See 
 * ecs_os_init() on how to use the function.
 * 
 * @param os_api Pointer to struct with values to set.
 */
FLECS_API
void ecs_os_set_api(
    ecs_os_api_t *os_api);

/** Get OS API. 
 * 
 * @return A value with the current OS API callbacks 
 * @see ecs_os_init()
 */
FLECS_API
ecs_os_api_t ecs_os_get_api(void);

/** Set default values for OS API.
 * This initializes the OS API struct with default values for callbacks like
 * malloc and free.
 * 
 * @see ecs_os_init()
 */
FLECS_API
void ecs_os_set_api_defaults(void);

/** Macro utilities 
 * \cond
 */

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

#define ecs_os_realloc_t(ptr, T) ECS_CAST(T*, ecs_os_realloc(ptr, ECS_SIZEOF(T)))
#define ecs_os_realloc_n(ptr, T, count) ECS_CAST(T*, ecs_os_realloc(ptr, ECS_SIZEOF(T) * (count)))
#define ecs_os_alloca_t(T) ECS_CAST(T*, ecs_os_alloca(ECS_SIZEOF(T)))
#define ecs_os_alloca_n(T, count) ECS_CAST(T*, ecs_os_alloca(ECS_SIZEOF(T) * (count)))

/* Strings */
#ifndef ecs_os_strdup
#define ecs_os_strdup(str) ecs_os_api.strdup_(str)
#endif

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
#define ecs_os_memcpy_n(ptr1, ptr2, T, count) ecs_os_memcpy(ptr1, ptr2, ECS_SIZEOF(T) * (size_t)count)
#define ecs_os_memcmp_t(ptr1, ptr2, T) ecs_os_memcmp(ptr1, ptr2, ECS_SIZEOF(T))

#define ecs_os_memmove_t(ptr1, ptr2, T) ecs_os_memmove(ptr1, ptr2, ECS_SIZEOF(T))
#define ecs_os_memmove_n(ptr1, ptr2, T, count) ecs_os_memmove(ptr1, ptr2, ECS_SIZEOF(T) * (size_t)count)
#define ecs_os_memmove_t(ptr1, ptr2, T) ecs_os_memmove(ptr1, ptr2, ECS_SIZEOF(T))

#define ecs_os_strcmp(str1, str2) strcmp(str1, str2)
#define ecs_os_memset_t(ptr, value, T) ecs_os_memset(ptr, value, ECS_SIZEOF(T))
#define ecs_os_memset_n(ptr, value, T, count) ecs_os_memset(ptr, value, ECS_SIZEOF(T) * (size_t)count)
#define ecs_os_zeromem(ptr) ecs_os_memset(ptr, 0, ECS_SIZEOF(*ptr))

#define ecs_os_memdup_t(ptr, T) ecs_os_memdup(ptr, ECS_SIZEOF(T))
#define ecs_os_memdup_n(ptr, T, count) ecs_os_memdup(ptr, ECS_SIZEOF(T) * count)

#define ecs_offset(ptr, T, index)\
    ECS_CAST(T*, ECS_OFFSET(ptr, ECS_SIZEOF(T) * index))

#if !defined(ECS_TARGET_POSIX) && !defined(ECS_TARGET_MINGW)
#define ecs_os_strcat(str1, str2) strcat_s(str1, INT_MAX, str2)
#define ecs_os_snprintf(ptr, len, ...) sprintf_s(ptr, ECS_CAST(size_t, len), __VA_ARGS__)
#define ecs_os_vsnprintf(ptr, len, fmt, args) vsnprintf(ptr, ECS_CAST(size_t, len), fmt, args)
#define ecs_os_strcpy(str1, str2) strcpy_s(str1, INT_MAX, str2)
#define ecs_os_strncpy(str1, str2, len) strncpy_s(str1, INT_MAX, str2, ECS_CAST(size_t, len))
#else
#define ecs_os_strcat(str1, str2) strcat(str1, str2)
#define ecs_os_snprintf(ptr, len, ...) snprintf(ptr, ECS_CAST(size_t, len), __VA_ARGS__)
#define ecs_os_vsnprintf(ptr, len, fmt, args) vsnprintf(ptr, ECS_CAST(size_t, len), fmt, args)
#define ecs_os_strcpy(str1, str2) strcpy(str1, str2)
#define ecs_os_strncpy(str1, str2, len) strncpy(str1, str2, ECS_CAST(size_t, len))
#endif

/* Files */
#ifndef ECS_TARGET_POSIX
#define ecs_os_fopen(result, file, mode) fopen_s(result, file, mode)
#else
#define ecs_os_fopen(result, file, mode) (*(result)) = fopen(file, mode)
#endif

/* Threads */
#define ecs_os_thread_new(callback, param) ecs_os_api.thread_new_(callback, param)
#define ecs_os_thread_join(thread) ecs_os_api.thread_join_(thread)
#define ecs_os_thread_self() ecs_os_api.thread_self_()

/* Tasks */
#define ecs_os_task_new(callback, param) ecs_os_api.task_new_(callback, param)
#define ecs_os_task_join(thread) ecs_os_api.task_join_(thread)

/* Atomic increment / decrement */
#define ecs_os_ainc(value) ecs_os_api.ainc_(value)
#define ecs_os_adec(value) ecs_os_api.adec_(value)
#define ecs_os_lainc(value) ecs_os_api.lainc_(value)
#define ecs_os_ladec(value) ecs_os_api.ladec_(value)

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

#ifdef FLECS_ACCURATE_COUNTERS
#define ecs_os_inc(v)  (ecs_os_ainc(v))
#define ecs_os_linc(v) (ecs_os_lainc(v))
#define ecs_os_dec(v)  (ecs_os_adec(v))
#define ecs_os_ldec(v) (ecs_os_ladec(v))
#else
#define ecs_os_inc(v)  (++(*v))
#define ecs_os_linc(v) (++(*v))
#define ecs_os_dec(v)  (--(*v))
#define ecs_os_ldec(v) (--(*v))
#endif

#ifdef ECS_TARGET_MINGW
/* mingw bug: without this a conversion error is thrown, but isnan/isinf should
 * accept float, double and long double. */
#define ecs_os_isnan(val) (isnan((float)val))
#define ecs_os_isinf(val) (isinf((float)val))
#else
#define ecs_os_isnan(val) (isnan(val))
#define ecs_os_isinf(val) (isinf(val))
#endif

/* Application termination */
#define ecs_os_abort() ecs_os_api.abort_()

/* Dynamic libraries */
#define ecs_os_dlopen(libname) ecs_os_api.dlopen_(libname)
#define ecs_os_dlproc(lib, procname) ecs_os_api.dlproc_(lib, procname)
#define ecs_os_dlclose(lib) ecs_os_api.dlclose_(lib)

/* Module id translation */
#define ecs_os_module_to_dl(lib) ecs_os_api.module_to_dl_(lib)
#define ecs_os_module_to_etc(lib) ecs_os_api.module_to_etc_(lib)

/** Macro utilities 
 * \endcond
 */


/* Logging */

/** Log at debug level.
 * 
 * @param file The file to log.
 * @param line The line to log.
 * @param msg The message to log.
*/
FLECS_API
void ecs_os_dbg(
    const char *file, 
    int32_t line, 
    const char *msg);

/** Log at trace level.
 * 
 * @param file The file to log.
 * @param line The line to log.
 * @param msg The message to log.
*/
FLECS_API
void ecs_os_trace(
    const char *file, 
    int32_t line, 
    const char *msg);

/** Log at warning level.
 * 
 * @param file The file to log.
 * @param line The line to log.
 * @param msg The message to log.
*/
FLECS_API
void ecs_os_warn(
    const char *file, 
    int32_t line, 
    const char *msg);

/** Log at error level.
 * 
 * @param file The file to log.
 * @param line The line to log.
 * @param msg The message to log.
*/
FLECS_API
void ecs_os_err(
    const char *file, 
    int32_t line, 
    const char *msg);

/** Log at fatal level.
 * 
 * @param file The file to log.
 * @param line The line to log.
 * @param msg The message to log.
*/
FLECS_API
void ecs_os_fatal(
    const char *file, 
    int32_t line, 
    const char *msg);

/** Convert errno to string.
 * 
 * @param err The error number.
 * @return A string describing the error.
 */
FLECS_API
const char* ecs_os_strerror(
    int err);

/** Utility for assigning strings. 
 * This operation frees an existing string and duplicates the input string.
 * 
 * @param str Pointer to a string value.
 * @param value The string value to assign.
 */
FLECS_API
void ecs_os_strset(
    char **str, 
    const char *value);

/** Sleep with floating point time. 
 * 
 * @param t The time in seconds.
 */
FLECS_API
void ecs_sleepf(
    double t);

/** Measure time since provided timestamp. 
 * Use with a time value initialized to 0 to obtain the number of seconds since
 * the epoch. The operation will write the current timestamp in start.
 * 
 * Usage:
 * @code
 * ecs_time_t t = {};
 * ecs_time_measure(&t);
 * // code
 * double elapsed = ecs_time_measure(&t);
 * @endcode
 * 
 * @param start The starting timestamp.
 * @return The time elapsed since start.
 */
FLECS_API
double ecs_time_measure(
    ecs_time_t *start);

/** Calculate difference between two timestamps. 
 * 
 * @param t1 The first timestamp.
 * @param t2 The first timestamp.
 * @return The difference between timestamps.
 */
FLECS_API
ecs_time_t ecs_time_sub(
    ecs_time_t t1,
    ecs_time_t t2);

/** Convert time value to a double. 
 * 
 * @param t The timestamp.
 * @return The timestamp converted to a double.
 */
FLECS_API
double ecs_time_to_double(
    ecs_time_t t);

/** Return newly allocated memory that contains a copy of src. 
 * 
 * @param src The source pointer.
 * @param size The number of bytes to copy.
 * @return The duplicated memory.
 */
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

/** Are task functions available? */
FLECS_API
bool ecs_os_has_task_support(void);

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

/** @} */

#endif
