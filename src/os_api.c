
#include "include/private/flecs.h"

static bool ecs_os_api_initialized = false;

const ecs_os_api_t ecs_os_api;

/* Allow access to API only in local translation unit */
static
ecs_os_api_t *_ecs_os_api = (ecs_os_api_t*)&ecs_os_api;

void ecs_set_os_api(
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
    return ut_thread_new(callback, param);
}

static
void* bake_thread_join(
    ecs_os_thread_t thread)
{
    void *arg;
    ut_thread_join(thread, &arg);
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

#endif

void ecs_set_os_api_defaults(void)
{
    /* Don't overwrite if already initialized */
    if (ecs_os_api_initialized) {
        return;
    }
    
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
#endif

    _ecs_os_api->abort = abort;
}