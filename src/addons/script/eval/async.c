/**
 * @file addons/script/eval/async.c
 * @brief Async/await support for Flecs script.
 *
 * This file implements futures and tasks. A task runs a script through the
 * script runner (see visit_eval.c), which evaluates statements and suspends
 * when a script awaits a future. Statement evaluation details live entirely
 * in the runner; this file only starts, resumes and cancels an opaque
 * runner and manages the futures that complete asynchronously.
 */

#include "flecs.h"

#ifdef FLECS_SCRIPT_ASYNC
#include "../script.h"

ECS_COMPONENT_DECLARE(EcsScriptTask);

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

struct ecs_script_task_t {
    ecs_script_t *script;
    ecs_script_runner_t runner;
    ecs_script_eval_desc_t eval_desc;
    ecs_script_vars_t *initial_vars;
    ecs_entity_t entity;
    void *ctx;
    ecs_ctx_free_t ctx_free;
    int32_t iterations;
    int32_t completed_iterations;
    bool has_owner_vars;
    bool running;
    ecs_script_task_loop_t loop;
    ecs_script_task_status_t status;
};

static void flecs_script_future_lock(
    ecs_script_future_t *future)
{
    if (future->mutex) {
        ecs_os_mutex_lock(future->mutex);
    }
}

static void flecs_script_future_unlock(
    ecs_script_future_t *future)
{
    if (future->mutex) {
        ecs_os_mutex_unlock(future->mutex);
    }
}

static ecs_script_future_t* flecs_script_future_start(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_t type,
    ecs_function_calldata_t *calldata,
    int32_t argc,
    ecs_value_t *argv)
{
    ecs_script_future_t *future = ecs_os_calloc_t(ecs_script_future_t);
    future->type = type;
    future->cancel = calldata->async_cancel;
    future->function_ctx = (ecs_function_ctx_t){
        .world = world,
        .function = calldata->function,
        .entity = entity,
        .ctx = calldata->ctx
    };
    future->refs = 2;
    if (ecs_os_has_threading()) {
        future->mutex = ecs_os_mutex_new();
    }

    calldata->async_callback(&future->function_ctx, argc, argv, future);

    return future;
}

static flecs_script_future_state_t flecs_script_future_poll(
    const ecs_script_future_t *future)
{
    ecs_script_future_t *ptr = ECS_CONST_CAST(ecs_script_future_t*, future);
    flecs_script_future_lock(ptr);
    flecs_script_future_state_t state = ptr->state;
    flecs_script_future_unlock(ptr);
    return state;
}

static void flecs_script_future_cancel(
    ecs_script_future_t *future)
{
    flecs_script_future_lock(future);
    bool cancel = future->state == FlecsScriptFuturePending;
    if (cancel) {
        future->state = FlecsScriptFutureCancelled;
    }
    flecs_script_future_unlock(future);
    if (cancel && future->cancel) {
        future->cancel(&future->function_ctx, future);
    }
}

ecs_script_future_t* ecs_script_future_retain(
    ecs_script_future_t *future)
{
    ecs_check(future != NULL, ECS_INVALID_PARAMETER, NULL);
    flecs_script_future_lock(future);
    future->refs ++;
    flecs_script_future_unlock(future);
    return future;
error:
    return NULL;
}

void ecs_script_future_release(
    ecs_script_future_t *future)
{
    if (!future) {
        return;
    }

    flecs_script_future_lock(future);
    int32_t refs = -- future->refs;
    flecs_script_future_unlock(future);
    if (refs) {
        return;
    }

    if (future->value.ptr) {
        ecs_value_fini(future->function_ctx.world, &future->value);
    }
    ecs_os_free(future->error);
    if (future->ctx_free) {
        future->ctx_free(future->ctx);
    }
    if (future->mutex) {
        ecs_os_mutex_free(future->mutex);
    }
    ecs_os_free(future);
}

int ecs_script_future_resolve(
    ecs_script_future_t *future,
    const ecs_value_t *value)
{
    ecs_check(future != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(value != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(value->type == future->type, ECS_INVALID_PARAMETER, NULL);
    ecs_check(value->ptr != NULL, ECS_INVALID_PARAMETER, NULL);

    flecs_script_future_lock(future);
    if (future->state != FlecsScriptFuturePending) {
        flecs_script_future_unlock(future);
        goto error;
    }

    future->value = ecs_value_new(future->function_ctx.world, future->type);
    ecs_ptr_copy(future->function_ctx.world, future->type,
        future->value.ptr, value->ptr);
    future->state = FlecsScriptFutureResolved;
    flecs_script_future_unlock(future);
    return 0;
error:
    return -1;
}

int ecs_script_future_reject(
    ecs_script_future_t *future,
    const char *error)
{
    ecs_check(error != NULL, ECS_INVALID_PARAMETER, NULL);
    return ecs_script_future_reject_id(future, 0, error);
error:
    return -1;
}

int ecs_script_future_reject_id(
    ecs_script_future_t *future,
    ecs_entity_t error,
    const char *msg)
{
    ecs_check(future != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(error || msg, ECS_INVALID_PARAMETER, NULL);

    flecs_script_future_lock(future);
    if (future->state != FlecsScriptFuturePending) {
        flecs_script_future_unlock(future);
        goto error;
    }
    future->error = ecs_os_strdup(msg ? msg : "async function failed");
    future->error_id = error;
    future->state = FlecsScriptFutureRejected;
    flecs_script_future_unlock(future);
    return 0;
error:
    return -1;
}

bool ecs_script_future_is_cancelled(
    const ecs_script_future_t *future)
{
    ecs_check(future != NULL, ECS_INVALID_PARAMETER, NULL);
    return flecs_script_future_poll(future) == FlecsScriptFutureCancelled;
error:
    return false;
}

void ecs_script_future_set_ctx(
    ecs_script_future_t *future,
    void *ctx,
    ecs_ctx_free_t ctx_free)
{
    ecs_check(future != NULL, ECS_INVALID_PARAMETER, NULL);
    flecs_script_future_lock(future);
    ecs_assert(future->ctx == NULL, ECS_INVALID_OPERATION, NULL);
    future->ctx = ctx;
    future->ctx_free = ctx_free;
    flecs_script_future_unlock(future);
error:
    return;
}

void* ecs_script_future_get_ctx(
    const ecs_script_future_t *future)
{
    ecs_check(future != NULL, ECS_INVALID_PARAMETER, NULL);
    return future->ctx;
error:
    return NULL;
}

/* Async statement evaluation, invoked by the script runner */

void flecs_script_throw_clear(
    ecs_script_runner_t *r)
{
    if (r->thrown) {
        ecs_script_future_release(r->thrown);
        r->thrown = NULL;
    }
    r->throw_node = NULL;
}

void flecs_script_report_throw(
    ecs_script_runner_t *r)
{
    ecs_script_eval_visitor_t *v = &r->v;
    ecs_entity_t error_id = r->thrown->error_id;
    const char *msg = r->thrown->error;
    if (error_id) {
        char *path = ecs_get_path(v->world, error_id);
        flecs_script_eval_error(v, r->throw_node, "%s: %s", path, msg);
        ecs_os_free(path);
    } else {
        flecs_script_eval_error(v, r->throw_node, "%s", msg);
    }
    flecs_script_throw_clear(r);
}

static int flecs_script_await_args(
    ecs_script_eval_visitor_t *v,
    ecs_expr_function_t *call,
    ecs_value_t *argv)
{
    ecs_expr_initializer_element_t *elems = ecs_vec_first(
        &call->args->elements);
    int32_t i, count = ecs_vec_count(&call->args->elements);
    for (i = 0; i < count; i ++) {
        argv[i] = ecs_value_new(v->world, elems[i].value->type);
        if (flecs_script_eval_expr(v, &elems[i].value, &argv[i])) {
            goto error;
        }
    }
    return 0;
error:
    for (int32_t j = 0; j <= i; j ++) {
        ecs_value_fini(v->world, &argv[j]);
    }
    return -1;
}

static int flecs_script_await_start(
    ecs_script_runner_t *r,
    ecs_script_node_t *stmt,
    ecs_expr_node_t **expr_ptr)
{
    ecs_script_eval_visitor_t *v = &r->v;
    ecs_expr_node_t *expr = *expr_ptr;
    if (expr->kind != EcsExprFunction && expr->kind != EcsExprMethod) {
        flecs_script_eval_error(v, stmt,
            "await expression must be an async function call");
        return -1;
    }

    ecs_expr_function_t *call = (ecs_expr_function_t*)expr;
    if (!call->calldata.async_callback) {
        flecs_script_eval_error(v, stmt,
            "await expression must be an async function call");
        return -1;
    }

    bool is_method = expr->kind == EcsExprMethod;
    int32_t argc = ecs_vec_count(&call->args->elements);
    int32_t arg_offset = is_method;
    int32_t value_count = argc + arg_offset;
    ecs_value_t *argv = NULL;
    if (value_count) {
        argv = ecs_os_calloc_n(ecs_value_t, value_count);
        if (is_method) {
            argv[0] = ecs_value_new(v->world, call->left->type);
            if (flecs_script_eval_expr(v, &call->left, &argv[0])) {
                ecs_value_fini(v->world, &argv[0]);
                ecs_os_free(argv);
                return -1;
            }
        }
        if (argc && flecs_script_await_args(v, call, &argv[arg_offset])) {
            if (is_method) {
                ecs_value_fini(v->world, &argv[0]);
            }
            ecs_os_free(argv);
            return -1;
        }
    }

    r->future = flecs_script_future_start(v->world, r->async_entity,
        call->node.type, &call->calldata, argc, argv);

    for (int32_t i = 0; i < value_count; i ++) {
        ecs_value_fini(v->world, &argv[i]);
    }
    ecs_os_free(argv);
    return 0;
}

static int flecs_script_await_assign_const(
    ecs_script_eval_visitor_t *v,
    ecs_script_var_node_t *node,
    const ecs_value_t *value,
    bool export)
{
    if (export) {
        if (ecs_script_vars_lookup(v->vars, node->name)) {
            flecs_script_eval_error(v, node,
                "exported variable '%s' shadows a local variable",
                node->name);
            return -1;
        }

        bool is_mut = node->node.kind == EcsAstExportMut;
        ecs_entity_t global_var;
        if (is_mut) {
            global_var = ecs_mut_var(v->world, {
                .parent = v->parent,
                .name = node->name,
                .type = value->type,
                .value = value->ptr
            });
        } else {
            global_var = ecs_const_var(v->world, {
                .parent = v->parent,
                .name = node->name,
                .type = value->type,
                .value = value->ptr
            });
        }
        if (!global_var) {
            flecs_script_eval_error(v, node,
                "failed to create exported %s variable '%s'",
                is_mut ? "mut" : "const", node->name);
            return -1;
        }
        return 0;
    }

    ecs_script_var_t *var = ecs_script_vars_declare(v->vars, node->name);
    if (!var) {
        flecs_script_eval_error(v, node,
            "variable '%s' redeclared", node->name);
        return -1;
    }

    const ecs_type_info_t *ti = ecs_get_type_info(v->world, value->type);
    var->value.type = value->type;
    var->value.ptr = flecs_stack_alloc(&v->r->stack,
        ti->size, ti->alignment);
    var->type_info = ti;
    var->owned = true;
    flecs_type_info_ctor(var->value.ptr, 1, ti);
    ecs_ptr_copy(v->world, value->type, var->value.ptr, value->ptr);
    return 0;
}

static int32_t flecs_script_find_catch(
    ecs_script_runner_t *r,
    ecs_script_try_t *node)
{
    ecs_script_eval_visitor_t *v = &r->v;
    ecs_entity_t throw_id = r->thrown->error_id;
    int32_t i, count = ecs_vec_count(&node->catches);
    ecs_script_catch_t *catches = ecs_vec_first(&node->catches);
    for (i = 0; i < count; i ++) {
        if (!catches[i].error) {
            return i;
        }
        if (!throw_id) {
            continue;
        }
        ecs_entity_t e = catches[i].eval_error;
        if (catches[i].error_symbol != -1) {
            e = flecs_script_symbol_entity(v, catches[i].error_symbol);
        }
        if (e == throw_id) {
            return i;
        }
    }
    return -1;
}

/* Returns 0 to continue, -1 on error, 1 when execution suspends */
int flecs_script_step_await(
    ecs_script_runner_t *r,
    ecs_script_node_t *stmt)
{
    ecs_script_eval_visitor_t *v = &r->v;
    ecs_expr_node_t **expr;
    ecs_script_var_node_t *var = NULL;
    bool export = false;

    if (stmt->kind == EcsAstAwait) {
        expr = &((ecs_script_await_t*)stmt)->expr;
    } else {
        var = (ecs_script_var_node_t*)stmt;
        expr = &var->expr;
        export = stmt->kind == EcsAstExportConst ||
            stmt->kind == EcsAstExportMut;
    }

    if (!r->can_suspend) {
        flecs_script_eval_error(v, stmt,
            "await requires resumable script execution");
        return -1;
    }

    if (!r->future) {
        if (flecs_script_await_start(r, stmt, expr)) {
            return -1;
        }
    }

    flecs_script_future_state_t state = flecs_script_future_poll(r->future);
    if (state == FlecsScriptFuturePending) {
        return 1;
    }

    ecs_script_future_t *future = r->future;
    r->future = NULL;

    if (state == FlecsScriptFutureRejected) {
        /* Record the error instead of reporting it immediately, so that an
         * enclosing try block can catch it. If it remains uncaught it is
         * reported when it propagates out of the script. The reference to the
         * rejected future is transferred to the runner. */
        flecs_script_throw_clear(r);
        r->thrown = future;
        r->throw_node = stmt;
        return -1;
    }

    int result = 0;
    if (state == FlecsScriptFutureCancelled) {
        flecs_script_eval_error(v, stmt,
            "awaited operation was cancelled");
        result = -1;
    } else if (var && flecs_script_await_assign_const(
        v, var, &future->value, export))
    {
        result = -1;
    }

    ecs_script_future_release(future);
    return result;
}

int flecs_script_step_try(
    ecs_script_runner_t *r,
    flecs_script_frame_t *frame)
{
    ecs_script_try_t *node = (ecs_script_try_t*)frame->node;
    if (frame->pc == 0) {
        frame->pc = 1;
        flecs_script_scope_push(r, node->try_scope);
        return 0;
    }

    flecs_script_frame_pop(r);
    return 0;
}

bool flecs_script_try_catch(
    ecs_script_runner_t *r,
    flecs_script_frame_t *frame)
{
    /* Runtime errors are not catchable, and an error thrown from a catch
     * handler can only be caught by an enclosing try block. */
    if (frame->node->kind != EcsAstTry || !r->thrown ||
        frame->pc != 1)
    {
        return false;
    }

    ecs_script_try_t *node = (ecs_script_try_t*)frame->node;
    int32_t catch_index = flecs_script_find_catch(r, node);
    if (catch_index == -1) {
        return false;
    }

    flecs_script_throw_clear(r);
    frame->state.try_.catch_index = catch_index;
    frame->pc = 2;
    ecs_script_catch_t *catches = ecs_vec_first(&node->catches);
    flecs_script_scope_push(r, catches[catch_index].scope);
    return true;
}

static void flecs_script_task_register(
    ecs_script_task_t *task)
{
    ecs_world_t *world = task->script->world;
    if (!ecs_id(EcsScriptTask)) {
        /* Script module wasn't imported for world */
        return;
    }

    EcsScriptTask *t = ecs_ensure(world, task->entity, EcsScriptTask);
    ecs_vec_init_if_t(&t->tasks, ecs_script_task_t*);
    ecs_script_task_t **elem = ecs_vec_append_t(
        NULL, &t->tasks, ecs_script_task_t*);
    *elem = task;
    ecs_modified(world, task->entity, EcsScriptTask);
}

static void flecs_script_task_unregister(
    ecs_script_task_t *task)
{
    ecs_entity_t entity = task->entity;
    if (!entity) {
        return;
    }

    task->entity = 0;

    ecs_world_t *world = task->script->world;
    if (!ecs_id(EcsScriptTask) || ecs_is_fini(world) ||
        !ecs_is_alive(world, entity))
    {
        return;
    }

    EcsScriptTask *t = ecs_get_mut(world, entity, EcsScriptTask);
    if (!t) {
        if (!ecs_is_deferred(world)) {
            return;
        }

        t = ecs_ensure(world, entity, EcsScriptTask);
        if (!t) {
            return;
        }
    }

    int32_t i, count = ecs_vec_count(&t->tasks);
    ecs_script_task_t **tasks = ecs_vec_first(&t->tasks);
    for (i = 0; i < count; i ++) {
        if (tasks[i] == task) {
            ecs_vec_remove_t(&t->tasks, ecs_script_task_t*, i);
            break;
        }
    }

    if (!ecs_vec_count(&t->tasks)) {
        ecs_remove(world, entity, EcsScriptTask);
    }
}

ecs_script_task_t* ecs_script_task_new(
    const ecs_script_t *script,
    const ecs_script_task_desc_t *desc)
{
    ecs_check(script != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(!desc || (desc->loop >= EcsScriptTaskLoopOnce &&
        desc->loop <= EcsScriptTaskLoopForever),
        ECS_INVALID_PARAMETER, NULL);
    ecs_check(!desc || desc->loop != EcsScriptTaskLoopCount ||
        desc->iterations > 0,
        ECS_INVALID_PARAMETER, NULL);
    ecs_check(!desc || !desc->entity ||
        ecs_is_alive(script->world, desc->entity),
        ECS_INVALID_PARAMETER, "task entity is not alive");

    ecs_script_task_t *result = ecs_os_calloc_t(
        ecs_script_task_t);
    result->script = ECS_CONST_CAST(ecs_script_t*, script);
    flecs_script_impl(result->script)->task_refcount ++;
    ecs_script_runtime_t *runtime = ecs_script_runtime_new();
    result->eval_desc.runtime = runtime;
    if (desc) {
        result->entity = desc->entity;
        result->ctx = desc->ctx;
        result->ctx_free = desc->ctx_free;
        result->loop = desc->loop;
        result->iterations = desc->iterations;
    }
    flecs_script_runner_init(&result->runner,
        flecs_script_impl(result->script), &result->eval_desc);
    result->runner.can_suspend = true;
    result->runner.async_entity = result->entity;
    if (result->entity) {
        ecs_script_eval_visitor_t *v = &result->runner.v;
        v->vars = flecs_script_vars_push(
            v->vars, &runtime->stack, &runtime->allocator);
        ecs_script_var_t *var = ecs_script_vars_declare(
            v->vars, "this");
        var->value.type = ecs_id(ecs_entity_t);
        var->value.ptr = flecs_stack_alloc(&runtime->stack,
            ECS_SIZEOF(ecs_entity_t), ECS_ALIGNOF(ecs_entity_t));
        *(ecs_entity_t*)var->value.ptr = result->entity;
        var->type_info = ecs_get_type_info(
            result->script->world, ecs_id(ecs_entity_t));
        var->owned = true;
        result->has_owner_vars = true;
    }
    ecs_script_impl_t *impl = flecs_script_impl(result->script);
    if (!impl->compiled) {
        if (flecs_script_visit_include(&result->runner.v, impl->root) ||
            flecs_script_visit_type(&result->runner.v, impl->root))
        {
            goto task_error;
        }
        impl->compiled = true;
    }
    if (result->entity) {
        flecs_script_task_register(result);
    }
    result->initial_vars = result->runner.v.vars;
    return result;
task_error:
    if (result->has_owner_vars) {
        result->runner.v.vars = ecs_script_vars_pop(result->runner.v.vars);
    }
    flecs_script_runner_fini(&result->runner, &result->eval_desc);
    ecs_script_runtime_free(runtime);
    if (result->ctx_free) {
        result->ctx_free(result->ctx);
    }
    flecs_script_impl(result->script)->task_refcount --;
    ecs_os_free(result);
error:
    return NULL;
}

static void flecs_script_task_cancel_future(
    ecs_script_task_t *task)
{
    ecs_script_future_t *future = task->runner.future;
    task->runner.future = NULL;
    if (!future) {
        return;
    }

    flecs_script_future_cancel(future);
    ecs_script_future_release(future);
}

ecs_script_task_status_t ecs_script_task_resume(
    ecs_script_task_t *task,
    ecs_script_eval_result_t *result)
{
    ecs_check(task != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(!task->running, ECS_INVALID_OPERATION,
        "cannot resume a task that is already running");

    if (task->status == EcsScriptTaskDone ||
        task->status == EcsScriptTaskError ||
        task->status == EcsScriptTaskCancelled)
    {
        return task->status;
    }

    task->running = true;
    if (result) {
        flecs_log_capture_push(true);
    }
    flecs_script_run_status_t status = flecs_script_runner_run_scope(
        &task->runner, flecs_script_impl(task->script)->root);
    if (task->status == EcsScriptTaskCancelled) {
        flecs_script_task_cancel_future(task);
    } else if (status == FlecsScriptRunDone) {
        task->status = EcsScriptTaskDone;
    } else if (status == FlecsScriptRunError) {
        task->status = EcsScriptTaskError;
    } else {
        task->status = EcsScriptTaskPending;
    }
    if (result) {
        result->error = flecs_log_capture_pop();
        flecs_log_get_captured_error_pos(&result->line, &result->column);
    }
    if (task->status == EcsScriptTaskDone) {
        bool repeat = task->loop == EcsScriptTaskLoopForever;
        if (task->loop != EcsScriptTaskLoopOnce) {
            task->completed_iterations ++;
        }
        if (task->loop == EcsScriptTaskLoopCount) {
            repeat = task->completed_iterations < task->iterations;
        }
        if (repeat) {
            ecs_assert(task->runner.frame_count == 0,
                ECS_INTERNAL_ERROR, NULL);
            ecs_assert(task->runner.v.vars == task->initial_vars,
                ECS_INTERNAL_ERROR, NULL);
            task->status = EcsScriptTaskPending;
        }
    }
    task->running = false;
    return task->status;
error:
    return EcsScriptTaskError;
}

bool ecs_script_task_is_ready(
    const ecs_script_task_t *task)
{
    ecs_check(task != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_script_future_t *future = task->runner.future;
    if (!future) {
        return true;
    }
    return flecs_script_future_poll(future) != FlecsScriptFuturePending;
error:
    return false;
}

void ecs_script_task_cancel(
    ecs_script_task_t *task)
{
    ecs_check(task != NULL, ECS_INVALID_PARAMETER, NULL);
    if (task->status == EcsScriptTaskDone ||
        task->status == EcsScriptTaskError ||
        task->status == EcsScriptTaskCancelled)
    {
        return;
    }

    task->status = EcsScriptTaskCancelled;
    flecs_script_task_cancel_future(task);
error:
    return;
}

void ecs_script_task_free(
    ecs_script_task_t *task)
{
    if (!task) {
        return;
    }
    ecs_check(!task->running, ECS_INVALID_OPERATION,
        "cannot free a task that is already running");
    ecs_script_task_cancel(task);
    flecs_script_task_unregister(task);
    flecs_script_runner_abandon(&task->runner);
    ecs_assert(task->runner.v.vars == task->initial_vars,
        ECS_INTERNAL_ERROR, NULL);
    if (task->has_owner_vars) {
        task->runner.v.vars = ecs_script_vars_pop(
            task->runner.v.vars);
    }
    flecs_script_runner_fini(&task->runner, &task->eval_desc);
    ecs_script_runtime_free(task->eval_desc.runtime);
    if (task->ctx_free) {
        task->ctx_free(task->ctx);
    }
    ecs_assert(flecs_script_impl(task->script)->task_refcount > 0,
        ECS_INTERNAL_ERROR, NULL);
    flecs_script_impl(task->script)->task_refcount --;
    ecs_os_free(task);
error:
    return;
}

static ECS_MOVE(EcsScriptTask, dst, src, {
    ecs_vec_fini_t(NULL, &dst->tasks, ecs_script_task_t*);
    *dst = *src;
    ecs_os_zeromem(src);
})

static ECS_DTOR(EcsScriptTask, ptr, {
    int32_t t, task_count = ecs_vec_count(&ptr->tasks);
    ecs_script_task_t **tasks = ecs_vec_first(&ptr->tasks);
    if (task_count) {
        ecs_assert(!(ecs_get_world(tasks[0]->script->world)->flags &
            (EcsWorldFini|EcsWorldQuit)),
            ECS_INVALID_OPERATION,
            "script still has alive tasks, free tasks before ecs_fini");
    }
    for (t = 0; t < task_count; t ++) {
        /* Component doesn't own tasks; cancel so the application observes a
         * cancelled status on the next resume and frees them. */
        tasks[t]->entity = 0;
        ecs_script_task_cancel(tasks[t]);
    }
    ecs_vec_fini_t(NULL, &ptr->tasks, ecs_script_task_t*);
})

static void flecs_script_task_stmt_line(
    const char *code,
    const char *pos,
    int32_t *line_out,
    int32_t *column_out)
{
    if (!code || !pos || (pos < code)) {
        return;
    }

    int32_t line = 1, column = 1;
    const char *ptr;
    for (ptr = code; (ptr < pos) && ptr[0]; ptr ++) {
        if (ptr[0] == '\n') {
            line ++;
            column = 1;
        } else {
            column ++;
        }
    }

    *line_out = line;
    *column_out = column;
}

/* Must match the task_state_t struct registered in the import function */
typedef struct flecs_script_task_state_t {
    ecs_entity_t script;
    int32_t status;
    int32_t line;
    int32_t column;
    ecs_entity_t awaiting;
    int32_t iteration;
} flecs_script_task_state_t;

static int flecs_script_task_component_serialize(
    const ecs_serializer_t *ser,
    const void *ptr)
{
    const EcsScriptTask *data = ptr;
    ecs_entity_t state_type = ecs_lookup(
        ser->world, "flecs.script.task_state_t");
    ecs_assert(state_type != 0, ECS_INTERNAL_ERROR, NULL);

    int32_t i, count = ecs_vec_count(&data->tasks);
    ecs_script_task_t **tasks = ecs_vec_first(&data->tasks);
    for (i = 0; i < count; i ++) {
        const ecs_script_task_t *task = tasks[i];
        flecs_script_task_state_t state = {0};
        state.script = flecs_script_impl(task->script)->entity;
        state.status = task->status;
        state.iteration = task->completed_iterations;

        ecs_script_future_t *future = task->runner.future;
        if (future) {
            state.awaiting = future->function_ctx.function;
        }

        const char *pos = flecs_script_runner_stmt_pos(&task->runner);
        if (pos) {
            flecs_script_task_stmt_line(task->script->code, pos,
                &state.line, &state.column);
        }

        ser->value(ser, state_type, &state);
    }

    return 0;
}

static size_t flecs_script_task_component_count(
    const void *ptr)
{
    const EcsScriptTask *data = ptr;
    return flecs_ito(size_t, ecs_vec_count(&data->tasks));
}

void flecs_script_async_import(
    ecs_world_t *world)
{
    ECS_COMPONENT_DEFINE(world, EcsScriptTask);

    ecs_set_hooks(world, EcsScriptTask, {
        .ctor = flecs_default_ctor,
        .move = ecs_move(EcsScriptTask),
        .dtor = ecs_dtor(EcsScriptTask),
        .flags = ECS_TYPE_HOOK_COPY_ILLEGAL
    });

    ecs_add_pair(world, ecs_id(EcsScriptTask),
        EcsOnInstantiate, EcsDontInherit);

    ecs_entity_t status_type = ecs_enum(world, {
        .entity = ecs_entity(world, { .name = "task_status_t" }),
        .constants = {
            { .name = "Pending" },
            { .name = "Done" },
            { .name = "Error" },
            { .name = "Cancelled" }
        }
    });

    ecs_entity_t state_type = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "task_state_t" }),
        .members = {
            { .name = "script", .type = ecs_id(ecs_entity_t) },
            { .name = "status", .type = status_type },
            { .name = "line", .type = ecs_id(ecs_i32_t) },
            { .name = "column", .type = ecs_id(ecs_i32_t) },
            { .name = "awaiting", .type = ecs_id(ecs_entity_t) },
            { .name = "iteration", .type = ecs_id(ecs_i32_t) }
        }
    });

    ecs_entity_t state_vec = ecs_vector(world, {
        .entity = ecs_entity(world, { .name = "task_state_vec_t" }),
        .type = state_type
    });

    ecs_opaque(world, {
        .entity = ecs_id(EcsScriptTask),
        .type = {
            .as_type = state_vec,
            .serialize = flecs_script_task_component_serialize,
            .count = flecs_script_task_component_count
        }
    });
}

#endif
