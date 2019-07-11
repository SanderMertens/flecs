#include "flecs_private.h"

static bool ecs_os_api_initialized = false;
static bool ecs_os_api_debug_enabled = false;

const ecs_os_api_t ecs_os_api;

/* Allow access to API only in local translation unit */
static
ecs_os_api_t *_ecs_os_api = (ecs_os_api_t*)&ecs_os_api;

void ecs_os_set_api(
    ecs_os_api_t *os_api)
{
    if (!ecs_os_api_initialized) {
        *_ecs_os_api = *os_api;
        ecs_os_api_initialized = true;
    }
}

/* When flecs is built with bake, use threading functions from bake.util */
#ifdef __BAKE__

static
ecs_os_thread_t bake_thread_new(
    ecs_os_thread_callback_t callback, 
    void *param)
{
    return (ecs_os_thread_t)ut_thread_new(callback, param);
}

static
void* bake_thread_join(
    ecs_os_thread_t thread)
{
    void *arg;
    ut_thread_join((ut_thread)thread, &arg);
    return arg;
}

static
ecs_os_mutex_t bake_mutex_new(void) {
    struct ut_mutex_s *m = ecs_os_api.malloc(sizeof(struct ut_mutex_s));
    ut_mutex_new(m);
    return (ecs_os_mutex_t)(uintptr_t)m;
}

static
void bake_mutex_free(ecs_os_mutex_t mutex) {
    ut_mutex_free((struct ut_mutex_s*)mutex);
    ecs_os_api.free((struct ut_mutex_s*)mutex);
}

static
void bake_mutex_lock(ecs_os_mutex_t mutex) {
    ut_mutex_lock((struct ut_mutex_s*)mutex);
}

static
void bake_mutex_unlock(ecs_os_mutex_t mutex) {
    ut_mutex_unlock((struct ut_mutex_s*)mutex);
}

static
ecs_os_cond_t bake_cond_new(void) {
    struct ut_cond_s *c = ecs_os_api.malloc(sizeof(struct ut_cond_s));
    ut_cond_new(c);
    return (ecs_os_cond_t)(uintptr_t)c;
}

static 
void bake_cond_free(ecs_os_cond_t cond) {
    ut_cond_free((struct ut_cond_s *)cond);
}

static 
void bake_cond_signal(ecs_os_cond_t cond) {
    ut_cond_signal((struct ut_cond_s *)cond);
}

static 
void bake_cond_broadcast(ecs_os_cond_t cond) {
    ut_cond_broadcast((struct ut_cond_s *)cond);
}

static 
void bake_cond_wait(ecs_os_cond_t cond, ecs_os_mutex_t mutex) {
    ut_cond_wait((struct ut_cond_s *)cond, (struct ut_mutex_s *)mutex);
}

static 
void bake_log(
    ut_log_verbosity level,
    const char *msg,
    void *ctx)
{
    (void)ctx;
    if (level >= UT_ERROR) {
        ecs_os_err("%s", msg);
    } else if (level <= UT_TRACE) {
        ecs_os_dbg("%s", msg);
    } else {
        ecs_os_log("%s", msg);
    }
}
#endif

static
void ecs_log(const char *fmt, va_list args) {
    fprintf(stdout, "[log] ");
    vfprintf(stdout, fmt, args);
    fprintf(stdout, "\n");
}

static
void ecs_log_error(const char *fmt, va_list args) {
    fprintf(stderr, "[err] ");
    vfprintf(stderr, fmt, args);
    fprintf(stderr, "\n");
}

static
void ecs_log_debug(const char *fmt, va_list args) {
    if (ecs_os_api_debug_enabled) {
        fprintf(stderr, "[dbg] ");
        vfprintf(stderr, fmt, args);
        fprintf(stderr, "\n");
    }
}

static
void ecs_log_warning(const char *fmt, va_list args) {
    fprintf(stderr, "[warn] ");
    vfprintf(stderr, fmt, args);
    fprintf(stderr, "\n");
}

void ecs_os_dbg(const char *fmt, ...) {
#ifndef NDEBUG
    va_list args;
    va_start(args, fmt);
    if (ecs_os_api.log_debug) {
        ecs_os_api.log_debug(fmt, args);
    }
    va_end(args);
#else
    (void)fmt;
#endif
}

void ecs_os_warn(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    if (ecs_os_api.log_warning) {
        ecs_os_api.log_warning(fmt, args);
    }
    va_end(args);
}

void ecs_os_log(const char *fmt, ...) {
    va_list args;;
    va_start(args, fmt);
    if (ecs_os_api.log) {
        ecs_os_api.log(fmt, args);
    }
    va_end(args);
}

void ecs_os_err(const char *fmt, ...) {
    va_list args;;
    va_start(args, fmt);
    if (ecs_os_api.log_error) {
        ecs_os_api.log_error(fmt, args);
    }
    va_end(args);
}

void ecs_os_enable_dbg(bool enable) {
    ecs_os_api_debug_enabled = enable;
}

bool ecs_os_dbg_enabled(void) {
    return ecs_os_api_debug_enabled;
}

void ecs_os_gettime(ecs_time_t *timeOut)
{
    uint64_t now = ecs_os_time_now();
    timeOut->sec = now / 1000000000;
    timeOut->nanosec = now - timeOut->sec * 1000000000;
}

void ecs_os_set_api_defaults(void)
{
    /* Don't overwrite if already initialized */
    if (ecs_os_api_initialized) {
        return;
    }

    ecs_os_time_setup();
    
    _ecs_os_api->malloc = malloc;
    _ecs_os_api->free = free;
    _ecs_os_api->realloc = realloc;
    _ecs_os_api->calloc = calloc;

#ifdef __BAKE__
    _ecs_os_api->thread_new = bake_thread_new;
    _ecs_os_api->thread_join = bake_thread_join;

    _ecs_os_api->mutex_new = bake_mutex_new;
    _ecs_os_api->mutex_free = bake_mutex_free;
    _ecs_os_api->mutex_lock = bake_mutex_lock;
    _ecs_os_api->mutex_unlock = bake_mutex_unlock;

    _ecs_os_api->cond_new = bake_cond_new;
    _ecs_os_api->cond_free = bake_cond_free;
    _ecs_os_api->cond_signal = bake_cond_signal;
    _ecs_os_api->cond_broadcast = bake_cond_broadcast;
    _ecs_os_api->cond_wait = bake_cond_wait;

    ut_log_handlerRegister(bake_log, NULL);

#endif
    _ecs_os_api->sleep = ecs_os_time_sleep;
    _ecs_os_api->get_time = ecs_os_gettime;

    _ecs_os_api->log = ecs_log;
    _ecs_os_api->log_error = ecs_log_error;
    _ecs_os_api->log_debug = ecs_log_debug;
    _ecs_os_api->log_warning = ecs_log_warning;

    _ecs_os_api->abort = abort;
}
