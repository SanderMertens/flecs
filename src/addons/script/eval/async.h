/**
 * @file addons/script/eval/async.h
 * @brief Async/await support for Flecs script.
 */

#ifndef FLECS_SCRIPT_ASYNC_PRIVATE_H
#define FLECS_SCRIPT_ASYNC_PRIVATE_H

#ifdef FLECS_SCRIPT_ASYNC

typedef enum flecs_script_future_state_t {
    FlecsScriptFuturePending,
    FlecsScriptFutureResolved,
    FlecsScriptFutureRejected,
    FlecsScriptFutureCancelled
} flecs_script_future_state_t;

struct ecs_script_future_t {
    ecs_entity_t type;
    ecs_value_t value;
    char *error;
    ecs_entity_t error_id;
    void *ctx;
    ecs_ctx_free_t ctx_free;
    ecs_async_function_cancel_t cancel;
    ecs_function_ctx_t function_ctx;
    ecs_os_mutex_t mutex;
    int32_t refs;
    flecs_script_future_state_t state;
};

ecs_script_future_t* flecs_script_future_start(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_t type,
    ecs_function_calldata_t *calldata,
    int32_t argc,
    ecs_value_t *argv);

flecs_script_future_state_t flecs_script_future_poll(
    const ecs_script_future_t *future);

/* Async statement evaluation, invoked by the script runner */

int flecs_script_step_await(
    ecs_script_runner_t *r,
    ecs_script_node_t *stmt);

int flecs_script_step_try(
    ecs_script_runner_t *r,
    flecs_script_frame_t *frame);

/* Enter a catch clause if the frame is a try block that catches the pending
 * thrown error. Returns false if the error should continue to unwind. */
bool flecs_script_try_catch(
    ecs_script_runner_t *r,
    flecs_script_frame_t *frame);

int32_t flecs_script_find_catch(
    ecs_script_eval_visitor_t *v,
    const flecs_script_async_state_t *state,
    const ecs_script_try_t *node);

int flecs_script_await_poll(
    ecs_script_eval_visitor_t *v,
    flecs_script_async_state_t *async,
    const ecs_script_node_t *stmt,
    ecs_script_future_t **ready);

int flecs_script_await_export(
    ecs_script_eval_visitor_t *v,
    const ecs_script_var_node_t *node,
    const ecs_value_t *value);

void flecs_script_throw_clear(
    flecs_script_async_state_t *state);

void flecs_script_report_throw(
    ecs_script_eval_visitor_t *v,
    flecs_script_async_state_t *state);

void flecs_script_async_import(
    ecs_world_t *world);

#endif // FLECS_SCRIPT_ASYNC
#endif // FLECS_SCRIPT_ASYNC_PRIVATE_H
