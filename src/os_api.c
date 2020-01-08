#include "flecs_private.h"

static bool ecs_os_api_initialized = false;
static bool ecs_os_api_debug_enabled = false;

ecs_os_api_t ecs_os_api;

uint64_t ecs_os_api_malloc_count = 0;
uint64_t ecs_os_api_realloc_count = 0;
uint64_t ecs_os_api_calloc_count = 0;
uint64_t ecs_os_api_free_count = 0;

void ecs_os_set_api(
    ecs_os_api_t *os_api)
{
    if (!ecs_os_api_initialized) {
        ecs_os_api = *os_api;
        ecs_os_api_initialized = true;
    }
}

/* When flecs is built with bake, use functions from bake.util */
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

static
ecs_os_dl_t bake_dlopen(
    const char *dlname)
{
    return (ecs_os_dl_t)ut_dl_open(dlname);
}

static
void bake_dlclose(
    ecs_os_dl_t dl)
{
    ut_dl_close((ut_dl)dl);
}

static
ecs_os_proc_t bake_dlproc(
    ecs_os_dl_t dl,
    const char *procname)
{
    ecs_os_proc_t result = (ecs_os_proc_t)ut_dl_proc((ut_dl)dl, procname);
    if (!result) {
        ut_raise();
    }
    return result;
}

static
char* bake_module_to_dl(
    const char *module_id)
{
    const char *result = ut_locate(module_id, NULL, UT_LOCATE_LIB);
    if (result) {
        return ut_strdup(result);
    } else {
        return NULL;
    }
}


/* __BAKE__ */
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

static
void* ecs_os_api_malloc(size_t size) {
    ecs_os_api_malloc_count ++;
    ecs_assert(size != 0, ECS_INVALID_PARAMETER, NULL);
    return malloc(size);
}

static
void* ecs_os_api_calloc(size_t num, size_t size) {
    ecs_os_api_calloc_count ++;
    ecs_assert(size != 0, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(num != 0, ECS_INVALID_PARAMETER, NULL);
    return calloc(num, size);
}

static
void* ecs_os_api_realloc(void *ptr, size_t size) {
    ecs_assert(size != 0, ECS_INVALID_PARAMETER, NULL);

    if (ptr) {
        ecs_os_api_realloc_count ++;
    } else {
        /* If not actually reallocing, treat as malloc */
        ecs_os_api_malloc_count ++; 
    }
    
    return realloc(ptr, size);
}

static
void ecs_os_api_free(void *ptr) {
    if (ptr) {
        ecs_os_api_free_count ++;
    }
    free(ptr);
}

static
char* ecs_os_api_strdup(const char *str) {
    int len = strlen(str);
    char *result = ecs_os_api_malloc(len + 1);
    ecs_assert(result != NULL, ECS_OUT_OF_MEMORY, NULL);
    strcpy(result, str);
    return result;
}

void ecs_os_set_api_defaults(void)
{
    /* Don't overwrite if already initialized */
    if (ecs_os_api_initialized) {
        return;
    }

    ecs_os_time_setup();
    
    ecs_os_api.malloc = ecs_os_api_malloc;
    ecs_os_api.free = ecs_os_api_free;
    ecs_os_api.realloc = ecs_os_api_realloc;
    ecs_os_api.calloc = ecs_os_api_calloc;
    ecs_os_api.strdup = ecs_os_api_strdup;

#ifdef __BAKE__
    ecs_os_api.thread_new = bake_thread_new;
    ecs_os_api.thread_join = bake_thread_join;

    ecs_os_api.mutex_new = bake_mutex_new;
    ecs_os_api.mutex_free = bake_mutex_free;
    ecs_os_api.mutex_lock = bake_mutex_lock;
    ecs_os_api.mutex_unlock = bake_mutex_unlock;

    ecs_os_api.cond_new = bake_cond_new;
    ecs_os_api.cond_free = bake_cond_free;
    ecs_os_api.cond_signal = bake_cond_signal;
    ecs_os_api.cond_broadcast = bake_cond_broadcast;
    ecs_os_api.cond_wait = bake_cond_wait;

    ecs_os_api.dlopen = bake_dlopen;
    ecs_os_api.dlproc = bake_dlproc;
    ecs_os_api.dlclose = bake_dlclose;

    ecs_os_api.module_to_dl = bake_module_to_dl;

    ut_log_handlerRegister(bake_log, NULL);
/* __BAKE__ */
#endif

    ecs_os_api.sleep = ecs_os_time_sleep;
    ecs_os_api.get_time = ecs_os_gettime;

    ecs_os_api.log = ecs_log;
    ecs_os_api.log_error = ecs_log_error;
    ecs_os_api.log_debug = ecs_log_debug;
    ecs_os_api.log_warning = ecs_log_warning;

    ecs_os_api.abort = abort;
}
