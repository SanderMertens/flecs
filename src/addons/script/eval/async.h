/**
 * @file addons/script/eval/async.h
 * @brief Async/await support for Flecs script.
 */

#ifndef FLECS_SCRIPT_ASYNC_PRIVATE_H
#define FLECS_SCRIPT_ASYNC_PRIVATE_H

#ifdef FLECS_SCRIPT_ASYNC

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
    ecs_script_future_status_t status;
};

ecs_expr_function_t* flecs_script_await_call(
    ecs_script_eval_visitor_t *v,
    const ecs_script_node_t *stmt,
    ecs_expr_node_t *expr);

/* Create a future and invoke the async function callback. The future is
 * returned with two references: one for the caller and one for the callback. */
ecs_script_future_t* flecs_script_future_launch(
    ecs_world_t *world,
    const ecs_function_calldata_t *calldata,
    ecs_entity_t type,
    ecs_entity_t entity,
    int32_t argc,
    const ecs_value_t *argv);

void flecs_script_await_launch(
    ecs_world_t *world,
    flecs_script_async_state_t *async,
    const ecs_expr_function_t *call,
    ecs_value_t *argv);

/* Async statement evaluation, invoked by the script runner */

int flecs_script_step_await(
    ecs_script_runner_t *r,
    ecs_script_node_t *stmt);

/* Enter a catch clause if the frame is a try block that catches the pending
 * thrown error. Returns false if the error should continue to unwind. */
bool flecs_script_try_catch(
    ecs_script_runner_t *r,
    flecs_script_frame_t *frame);

int32_t flecs_script_catch(
    ecs_script_eval_visitor_t *v,
    flecs_script_async_state_t *state,
    const ecs_script_try_t *node);

int flecs_script_await_poll(
    ecs_script_eval_visitor_t *v,
    flecs_script_async_state_t *async,
    const ecs_script_node_t *stmt,
    ecs_script_future_t **ready);

void flecs_script_async_fini(
    flecs_script_async_state_t *state);

void flecs_script_throw_clear(
    flecs_script_async_state_t *state);

void flecs_script_report_throw(
    ecs_script_eval_visitor_t *v,
    flecs_script_async_state_t *state);

void flecs_script_async_import(
    ecs_world_t *world);

/* Create a task for an async block. Invoked when the async statement is
 * evaluated. The task captures the variables visible at the statement and is
 * owned by the template instance (for templates) or the script. */
int flecs_script_async_spawn(
    ecs_script_eval_visitor_t *v,
    ecs_script_async_t *node);

/* Free all tasks in a vector of async block tasks. */
void flecs_script_async_tasks_free(
    ecs_vec_t *tasks);

void flecs_script_async_cleanup(
    ecs_script_eval_visitor_t *v);

#endif // FLECS_SCRIPT_ASYNC
#endif // FLECS_SCRIPT_ASYNC_PRIVATE_H
