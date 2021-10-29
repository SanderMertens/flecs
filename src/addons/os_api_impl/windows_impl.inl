#include <windows.h>

static
ecs_os_thread_t win_thread_new(
    ecs_os_thread_callback_t callback, 
    void *arg)
{
    HANDLE *thread = ecs_os_malloc_t(HANDLE);
    *thread = CreateThread(
        NULL, 0, (LPTHREAD_START_ROUTINE)callback, arg, 0, NULL);
    return (ecs_os_thread_t)(uintptr_t)thread;
}

static
void* win_thread_join(
    ecs_os_thread_t thr)
{
    HANDLE *thread = (HANDLE*)(uintptr_t)thr;
    WaitForSingleObject(thread, INFINITE);
    return NULL;
}

static
int32_t win_ainc(
    int32_t *count) 
{
    return InterlockedIncrement(count);
}

static
int32_t win_adec(
    int32_t *count) 
{
    return InterlockedDecrement(count);
}

static
ecs_os_mutex_t win_mutex_new(void) {
    CRITICAL_SECTION *mutex = ecs_os_malloc_t(CRITICAL_SECTION);
    InitializeCriticalSection(mutex);
    return (ecs_os_mutex_t)(uintptr_t)mutex;
}

static
void win_mutex_free(
    ecs_os_mutex_t m) 
{
    CRITICAL_SECTION *mutex = (CRITICAL_SECTION*)(intptr_t)m;
    DeleteCriticalSection(mutex);
    ecs_os_free(mutex);
}

static
void win_mutex_lock(
    ecs_os_mutex_t m) 
{
    CRITICAL_SECTION *mutex = (CRITICAL_SECTION*)(intptr_t)m;
    EnterCriticalSection(mutex);
}

static
void win_mutex_unlock(
    ecs_os_mutex_t m) 
{
    CRITICAL_SECTION *mutex = (CRITICAL_SECTION*)(intptr_t)m;
    LeaveCriticalSection(mutex);
}

static
ecs_os_cond_t win_cond_new(void) {
    CONDITION_VARIABLE *cond = ecs_os_malloc_t(CONDITION_VARIABLE);
    InitializeConditionVariable(cond);
    return (ecs_os_cond_t)(uintptr_t)cond;
}

static 
void win_cond_free(
    ecs_os_cond_t c) 
{
    (void)c;
}

static 
void win_cond_signal(
    ecs_os_cond_t c) 
{
    CONDITION_VARIABLE *cond = (CONDITION_VARIABLE*)(intptr_t)c;
    WakeConditionVariable(cond);
}

static 
void win_cond_broadcast(
    ecs_os_cond_t c) 
{
    CONDITION_VARIABLE *cond = (CONDITION_VARIABLE*)(intptr_t)c;
    WakeAllConditionVariable(cond);
}

static 
void win_cond_wait(
    ecs_os_cond_t c, 
    ecs_os_mutex_t m) 
{
    CRITICAL_SECTION *mutex = (CRITICAL_SECTION*)(intptr_t)m;
    CONDITION_VARIABLE *cond = (CONDITION_VARIABLE*)(intptr_t)c;
    SleepConditionVariableCS(cond, mutex, INFINITE);
}

void ecs_set_os_api_impl(void) {
    ecs_os_set_api_defaults();

    ecs_os_api_t api = ecs_os_api;

    api.thread_new_ = win_thread_new;
    api.thread_join_ = win_thread_join;
    api.ainc_ = win_ainc;
    api.adec_ = win_adec;
    api.mutex_new_ = win_mutex_new;
    api.mutex_free_ = win_mutex_free;
    api.mutex_lock_ = win_mutex_lock;
    api.mutex_unlock_ = win_mutex_unlock;
    api.cond_new_ = win_cond_new;
    api.cond_free_ = win_cond_free;
    api.cond_signal_ = win_cond_signal;
    api.cond_broadcast_ = win_cond_broadcast;
    api.cond_wait_ = win_cond_wait;

    ecs_os_set_api(&api);
}
