#include "flecs_os_api_stdcpp.h"
#include <thread>
#include <mutex>
#include <condition_variable>

#ifdef _MSC_VER
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <WinSock2.h>
#include "windows.h"
#endif

static
ecs_os_thread_t stdcpp_thread_new(
    ecs_os_thread_callback_t callback, 
    void *arg)
{
	std::thread *thread = new std::thread{callback,arg};
    return reinterpret_cast<ecs_os_thread_t>(thread);
}

static
void* stdcpp_thread_join(
    ecs_os_thread_t thread)
{
    void *arg = nullptr;
    std::thread *thr = reinterpret_cast<std::thread*>(thread);
    thr->join();
    delete thr;
    return arg;
}

static
int32_t stdcpp_ainc(int32_t *count) {
#ifdef __GNUC__
    int value = __sync_add_and_fetch (count, 1);
    return value;
#else
    return InterlockedIncrement((uint32_t*)count);
#endif
}

static
int32_t stdcpp_adec(int32_t *count) {
#ifdef __GNUC__
    int value = __sync_sub_and_fetch (count, 1);
    return value;
#else
    return InterlockedDecrement((uint32_t*)count);
#endif
}

static
ecs_os_mutex_t stdcpp_mutex_new(void) {
    std::mutex *mutex = new std::mutex;
    return reinterpret_cast<ecs_os_mutex_t>(mutex);
}

static
void stdcpp_mutex_free(ecs_os_mutex_t m) {
    std::mutex*mutex = reinterpret_cast<std::mutex*>(m);
    delete mutex;
}

static
void stdcpp_mutex_lock(ecs_os_mutex_t m) {
    std::mutex*mutex = reinterpret_cast<std::mutex*>(m);
    mutex->lock();
}

static
void stdcpp_mutex_unlock(ecs_os_mutex_t m) {
    std::mutex *mutex = reinterpret_cast<std::mutex*>(m);
    mutex->unlock();
}

static
ecs_os_cond_t stdcpp_cond_new(void) {
    std::condition_variable_any* cond = new std::condition_variable_any{};
   return reinterpret_cast<ecs_os_cond_t>(cond);
}

static 
void stdcpp_cond_free(ecs_os_cond_t c) {
    std::condition_variable_any *cond = reinterpret_cast<std::condition_variable_any*>(c);
    delete cond;
}

static 
void stdcpp_cond_signal(ecs_os_cond_t c) {
    std::condition_variable_any *cond = reinterpret_cast<std::condition_variable_any*>(c);
    cond->notify_one();
}

static 
void stdcpp_cond_broadcast(ecs_os_cond_t c) {
    std::condition_variable_any*cond = reinterpret_cast<std::condition_variable_any*>(c);
    cond->notify_all();
}

static 
void stdcpp_cond_wait(ecs_os_cond_t c, ecs_os_mutex_t m) {
    std::condition_variable_any* cond = reinterpret_cast<std::condition_variable_any*>(c);
    std::mutex* mutex = reinterpret_cast<std::mutex*>(m);
    cond->wait(*mutex);
}

void stdcpp_set_os_api(void) {
    ecs_os_set_api_defaults();

    ecs_os_api_t api = ecs_os_api;

    api.thread_new_ = stdcpp_thread_new;
    api.thread_join_ = stdcpp_thread_join;
    api.task_new_ = stdcpp_thread_new;
    api.task_join_ = stdcpp_thread_join;
    api.ainc_ = stdcpp_ainc;
    api.adec_ = stdcpp_adec;
    api.mutex_new_ = stdcpp_mutex_new;
    api.mutex_free_ = stdcpp_mutex_free;
    api.mutex_lock_ = stdcpp_mutex_lock;
    api.mutex_unlock_ = stdcpp_mutex_unlock;
    api.cond_new_ = stdcpp_cond_new;
    api.cond_free_ = stdcpp_cond_free;
    api.cond_signal_ = stdcpp_cond_signal;
    api.cond_broadcast_ = stdcpp_cond_broadcast;
    api.cond_wait_ = stdcpp_cond_wait;

    ecs_os_set_api(&api);
}
