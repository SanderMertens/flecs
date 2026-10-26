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

typedef EcsScriptTask EcsScriptAsyncTasks;

static ECS_COMPONENT_DECLARE(EcsScriptAsyncTasks);

struct ecs_script_task_t {
    ecs_script_t *script;
    ecs_script_runner_t *runner;
    ecs_script_ir_vm_t *vm;
    ecs_script_eval_visitor_t *v;
    flecs_script_async_state_t *async;
    ecs_script_eval_desc_t eval_desc;
    ecs_script_vars_t *initial_vars;
    ecs_entity_t entity;
    void *ctx;
    ecs_ctx_free_t ctx_free;
    int32_t iterations;
    int32_t completed_iterations;
    bool running;
    ecs_script_task_loop_t loop;
    ecs_script_task_status_t status;

    /* Scope (runner) or entry (IR) that the task runs. Defaults to the script
     * root, or the body of an async block. */
    ecs_script_scope_t *scope;
    const ecs_script_ir_entry_t *entry;

    /* Async block tasks */
    ecs_script_state_t state;
    ecs_entity_t owner_instance;
    ecs_entity_t owner_template;
    int32_t sched_index;
    bool bound_registered;
    bool is_async_block;
    bool retains_script;
    bool free_pending;
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

ecs_expr_function_t* flecs_script_await_call(
    ecs_script_eval_visitor_t *v,
    const ecs_script_node_t *stmt,
    ecs_expr_node_t *expr)
{
    ecs_expr_function_t *call = (ecs_expr_function_t*)expr;
    if (!expr || (expr->kind != EcsExprFunction &&
        expr->kind != EcsExprMethod) || !call->calldata.async_callback)
    {
        flecs_script_eval_error(v, stmt,
            "await expression must be an async function call");
        return NULL;
    }

    return call;
}

void flecs_script_await_launch(
    ecs_world_t *world,
    flecs_script_async_state_t *async,
    const ecs_expr_function_t *call,
    ecs_value_t *argv)
{
    ecs_script_future_t *future = ecs_os_calloc_t(ecs_script_future_t);
    future->type = call->node.type;
    future->cancel = call->calldata.async_cancel;
    future->function_ctx = (ecs_function_ctx_t){
        .world = world,
        .function = call->calldata.function,
        .entity = async->entity,
        .ctx = call->calldata.ctx
    };
    future->refs = 2;
    if (ecs_os_has_threading()) {
        future->mutex = ecs_os_mutex_new();
    }

    call->calldata.async_callback(&future->function_ctx,
        ecs_vec_count(&call->args->elements), argv, future);
    async->future = future;
}

flecs_script_future_state_t flecs_script_future_poll(
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
    flecs_script_async_state_t *state)
{
    if (state->thrown) {
        ecs_script_future_release(state->thrown);
        state->thrown = NULL;
    }

    state->throw_node = NULL;
}

void flecs_script_async_fini(
    flecs_script_async_state_t *state)
{
    ecs_script_future_release(state->future);
    state->future = NULL;
    flecs_script_throw_clear(state);
}

void flecs_script_report_throw(
    ecs_script_eval_visitor_t *v,
    flecs_script_async_state_t *state)
{
    ecs_entity_t error_id = state->thrown->error_id;
    const char *msg = state->thrown->error;
    if (error_id) {
        char *path = ecs_get_path(v->world, error_id);
        flecs_script_eval_error(v, state->throw_node, "%s: %s", path, msg);
        ecs_os_free(path);
    } else {
        flecs_script_eval_error(v, state->throw_node, "%s", msg);
    }

    flecs_script_throw_clear(state);
}

int flecs_script_await_poll(
    ecs_script_eval_visitor_t *v,
    flecs_script_async_state_t *async,
    const ecs_script_node_t *stmt,
    ecs_script_future_t **ready)
{
    ecs_assert(async->future != NULL, ECS_INTERNAL_ERROR, NULL);
    flecs_script_future_state_t state = flecs_script_future_poll(async->future);
    if (state == FlecsScriptFuturePending) {
        return 1;
    }

    ecs_script_future_t *future = async->future;
    async->future = NULL;
    if (state == FlecsScriptFutureRejected) {
        flecs_script_throw_clear(async);
        async->thrown = future;
        async->throw_node = stmt;
        return -1;
    }

    if (state == FlecsScriptFutureCancelled) {
        flecs_script_eval_error(v, stmt, "awaited operation was cancelled");
        ecs_script_future_release(future);
        return -1;
    }

    *ready = future;
    return 0;
}

static int flecs_script_await_start(
    ecs_script_runner_t *r,
    ecs_script_node_t *stmt,
    ecs_expr_node_t **expr_ptr)
{
    ecs_script_eval_visitor_t *v = &r->v;
    ecs_expr_function_t *call = flecs_script_await_call(v, stmt, *expr_ptr);
    if (!call) {
        return -1;
    }

    int32_t arg_offset = call->node.kind == EcsExprMethod;
    int32_t i, count = ecs_vec_count(&call->args->elements) + arg_offset;
    ecs_value_t *argv = count ? ecs_os_alloca_n(ecs_value_t, count) : NULL;
    ecs_expr_initializer_element_t *elems = ecs_vec_first(
        &call->args->elements);
    int result = 0;
    for (i = 0; !result && i < count; i ++) {
        ecs_expr_node_t **arg = arg_offset && !i
            ? &call->left : &elems[i - arg_offset].value;
        argv[i] = ecs_value_new(v->world, (*arg)->type);
        result = flecs_script_eval_expr(v, arg, &argv[i]);
    }

    if (!result) {
        flecs_script_await_launch(v->world, &r->async, call, argv);
    }

    while (i --) {
        ecs_value_fini(v->world, &argv[i]);
    }

    return result;
}

static int flecs_script_await_assign_const(
    ecs_script_eval_visitor_t *v,
    ecs_script_var_node_t *node,
    const ecs_value_t *value)
{
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

int32_t flecs_script_catch(
    ecs_script_eval_visitor_t *v,
    flecs_script_async_state_t *state,
    const ecs_script_try_t *node)
{
    if (!state->thrown) {
        return -1;
    }

    ecs_entity_t throw_id = state->thrown->error_id;
    int32_t i, count = ecs_vec_count(&node->catches);
    ecs_script_catch_t *catches = ecs_vec_first(&node->catches);
    for (i = 0; i < count; i ++) {
        if (!catches[i].error) {
            break;
        }

        if (!throw_id) {
            continue;
        }

        ecs_entity_t e = catches[i].eval_error;
        if (catches[i].error_symbol != -1) {
            e = flecs_script_symbol_entity(v, catches[i].error_symbol);
        }

        if (e == throw_id) {
            break;
        }
    }

    if (i == count) {
        return -1;
    }

    flecs_script_throw_clear(state);
    return i;
}

/* Returns 0 to continue, -1 on error, 1 when execution suspends */
int flecs_script_step_await(
    ecs_script_runner_t *r,
    ecs_script_node_t *stmt)
{
    ecs_script_eval_visitor_t *v = &r->v;
    ecs_expr_node_t **expr;
    ecs_script_var_node_t *var = NULL;

    if (stmt->kind == EcsAstAwait) {
        expr = &((ecs_script_await_t*)stmt)->expr;
    } else {
        var = (ecs_script_var_node_t*)stmt;
        expr = &var->expr;
    }

    if (!r->can_suspend) {
        flecs_script_eval_error(v, stmt,
            "await requires resumable script execution");
        return -1;
    }

    if (!r->async.future && flecs_script_await_start(r, stmt, expr)) {
        return -1;
    }

    ecs_script_future_t *future = NULL;
    int result = flecs_script_await_poll(v, &r->async, stmt, &future);
    if (result) {
        return result;
    }

    if (var) {
        result = flecs_script_await_assign_const(v, var, &future->value);
    }

    ecs_script_future_release(future);
    return result;
}

bool flecs_script_try_catch(
    ecs_script_runner_t *r,
    flecs_script_frame_t *frame)
{
    /* Runtime errors are not catchable, and an error thrown from a catch
     * handler can only be caught by an enclosing try block. */
    if (frame->node->kind != EcsAstTry || frame->pc != 1) {
        return false;
    }

    ecs_script_try_t *node = (ecs_script_try_t*)frame->node;
    int32_t i = flecs_script_catch(&r->v, &r->async, node);
    if (i == -1) {
        return false;
    }

    frame->pc = 2;
    flecs_script_scope_push(r,
        ecs_vec_get_t(&node->catches, ecs_script_catch_t, i)->scope);
    return true;
}

static void flecs_script_task_push_this(
    ecs_script_eval_visitor_t *v,
    ecs_script_runtime_t *runtime,
    ecs_entity_t entity)
{
    v->vars = flecs_script_vars_push(
        v->vars, &runtime->stack, runtime->allocator);
    ecs_script_var_t *var = ecs_script_vars_declare(v->vars, "this");
    var->value.type = ecs_id(ecs_entity_t);
    var->value.ptr = flecs_stack_alloc(&runtime->stack,
        ECS_SIZEOF(ecs_entity_t), ECS_ALIGNOF(ecs_entity_t));
    *(ecs_entity_t*)var->value.ptr = entity;
    var->type_info = ecs_get_type_info(v->world, ecs_id(ecs_entity_t));
    var->owned = true;
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

static void flecs_script_task_vec_remove(
    ecs_vec_t *tasks,
    const ecs_script_task_t *task)
{
    int32_t i, count = ecs_vec_count(tasks);
    ecs_script_task_t **array = ecs_vec_first(tasks);
    for (i = 0; i < count; i ++) {
        if (array[i] == task) {
            ecs_vec_remove_t(tasks, ecs_script_task_t*, i);
            return;
        }
    }
}

static void flecs_script_task_unbind(
    ecs_script_task_t *task)
{
    if (!task->bound_registered) {
        return;
    }

    task->bound_registered = false;
    EcsScriptAsyncTasks *bound = ecs_get_mut(
        task->script->world, task->entity, EcsScriptAsyncTasks);
    if (bound) {
        flecs_script_task_vec_remove(&bound->tasks, task);
    }
}

static void flecs_script_task_unregister(
    ecs_script_task_t *task)
{
    ecs_entity_t entity = task->entity;
    if (!entity || task->is_async_block) {
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
        if (!ecs_is_stage(world)) {
            return;
        }

        t = ecs_ensure(world, entity, EcsScriptTask);
        if (!t) {
            return;
        }
    }

    flecs_script_task_vec_remove(&t->tasks, task);
    if (!ecs_vec_count(&t->tasks)) {
        ecs_remove(world, entity, EcsScriptTask);
    }
}

static ecs_script_task_t* flecs_script_task_alloc(
    ecs_script_impl_t *impl)
{
    ecs_size_t engine_size = impl->ir_enabled
        ? ECS_SIZEOF(ecs_script_ir_vm_t) : ECS_SIZEOF(ecs_script_runner_t);
    ecs_size_t engine_align = impl->ir_enabled
        ? ECS_ALIGNOF(ecs_script_ir_vm_t) : ECS_ALIGNOF(ecs_script_runner_t);
    ecs_size_t offset = ECS_ALIGN(ECS_SIZEOF(ecs_script_task_t), engine_align);
    ecs_script_task_t *result = ecs_os_malloc(offset + engine_size);
    ecs_os_zeromem(result);
    if (impl->ir_enabled) {
        result->vm = ECS_OFFSET(result, offset);
        result->v = &result->vm->v;
        result->async = &result->vm->async;
    } else {
        result->runner = ECS_OFFSET(result, offset);
        result->v = &result->runner->v;
        result->async = &result->runner->async;
    }

    result->script = &impl->pub;
    result->sched_index = -1;
    impl->task_refcount ++;
    result->eval_desc.runtime = flecs_script_runtime_new_shared(
        flecs_script_runtime_get(impl->pub.world));
    flecs_script_eval_visit_init(impl, result->v, &result->eval_desc);
    return result;
}

static void flecs_script_task_dealloc(
    ecs_script_task_t *task)
{
    ecs_script_eval_visitor_t *v = task->v;
    while (v->vars) {
        v->vars = ecs_script_vars_pop(v->vars);
    }

    if (task->is_async_block) {
        flecs_script_state_fini(&task->state);
    }

    ecs_script_runtime_free(task->eval_desc.runtime);
    if (task->ctx_free) {
        task->ctx_free(task->ctx);
    }

    ecs_script_impl_t *impl = flecs_script_impl(task->script);
    ecs_assert(impl->task_refcount > 0, ECS_INTERNAL_ERROR, NULL);
    impl->task_refcount --;
    bool release = task->retains_script;
    ecs_os_free(task);
    if (release) {
        ecs_script_free(&impl->pub);
    }
}

static int flecs_script_task_engine_init(
    ecs_script_task_t *task,
    const void *node)
{
    if (task->vm) {
        flecs_script_ir_vm_init(task->vm);
        if (task->vm->ir) {
            task->entry = flecs_script_ir_entry(task->vm->ir, node);
        }

        if (!task->entry) {
            flecs_script_ir_vm_fini(task->vm, &task->eval_desc);
            return -1;
        }

        task->vm->can_suspend = true;
    } else {
        flecs_script_runner_init(task->runner, task->v);
        task->runner->can_suspend = true;
    }

    task->async->entity = task->entity;
    return 0;
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

    ecs_script_impl_t *impl = ECS_CONST_CAST(ecs_script_impl_t*, script);
    ecs_script_task_t *result = flecs_script_task_alloc(impl);
    if (desc) {
        result->entity = desc->entity;
        result->ctx = desc->ctx;
        result->ctx_free = desc->ctx_free;
        result->loop = desc->loop;
        result->iterations = desc->iterations;
    }

    if (result->entity) {
        flecs_script_task_push_this(
            result->v, result->eval_desc.runtime, result->entity);
    }

    result->scope = impl->root;
    if (flecs_script_compile(result->v) ||
        flecs_script_task_engine_init(result, impl->root))
    {
        flecs_script_task_dealloc(result);
        return NULL;
    }

    if (result->entity) {
        flecs_script_task_register(result);
    }

    result->initial_vars = result->v->vars;
    return result;
error:
    return NULL;
}

static void flecs_script_task_cancel_future(
    ecs_script_task_t *task)
{
    ecs_script_future_t *future = task->async->future;
    task->async->future = NULL;
    if (future) {
        flecs_script_future_cancel(future);
        ecs_script_future_release(future);
    }
}

ecs_script_task_status_t ecs_script_task_resume(
    ecs_script_task_t *task,
    ecs_script_eval_result_t *result)
{
    ecs_check(task != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(!task->running, ECS_INVALID_OPERATION,
        "cannot resume a task that is already running");

    if (task->status != EcsScriptTaskPending) {
        return task->status;
    }

    task->running = true;
    if (result) {
        flecs_log_capture_push(true);
    }

    ecs_script_runtime_t *world_rt = flecs_script_runtime_get(task->v->world);
    bool prev_error = world_rt->error;
    world_rt->error = false;
    world_rt->eval_depth ++;
    flecs_script_run_status_t status;
    if (task->vm) {
        status = flecs_script_ir_vm_run(task->vm, task->entry);
    } else {
        status = flecs_script_runner_run_scope(task->runner, task->scope);
    }

    world_rt->eval_depth --;
    bool nested_error = world_rt->error;
    world_rt->error = prev_error;
    if (task->status == EcsScriptTaskCancelled) {
        flecs_script_task_cancel_future(task);
    } else if (status == FlecsScriptRunError || nested_error) {
        task->status = EcsScriptTaskError;
    } else if (status == FlecsScriptRunDone) {
        task->status = EcsScriptTaskDone;
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
            ecs_assert(task->vm
                ? task->vm->frame_count == 0
                : task->runner->frame_count == 0,
                ECS_INTERNAL_ERROR, NULL);
            ecs_assert(task->v->vars == task->initial_vars,
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
    ecs_script_future_t *future = task->async->future;
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
    if (task->status != EcsScriptTaskPending) {
        return;
    }

    task->status = EcsScriptTaskCancelled;
    flecs_script_task_cancel_future(task);
error:
    return;
}

static void flecs_script_task_sched_remove(
    ecs_script_task_t *task);

void ecs_script_task_free(
    ecs_script_task_t *task)
{
    if (!task) {
        return;
    }

    flecs_script_task_unbind(task);
    if (task->running) {
        /* Async block task freed by its owner while it is being resumed. The
         * scheduler frees the task after it yields. */
        ecs_check(task->is_async_block, ECS_INVALID_OPERATION,
            "cannot free a task that is already running");
        ecs_script_task_cancel(task);
        task->owner_instance = 0;
        task->owner_template = 0;
        task->free_pending = true;
        return;
    }

    ecs_script_task_cancel(task);
    flecs_script_task_unregister(task);
    flecs_script_task_sched_remove(task);
    if (task->vm) {
        flecs_script_ir_vm_fini(task->vm, &task->eval_desc);
    } else {
        flecs_script_runner_fini(task->runner, &task->eval_desc);
    }

    ecs_assert(task->v->vars == task->initial_vars, ECS_INTERNAL_ERROR, NULL);
    flecs_script_task_dealloc(task);
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

        ecs_script_future_t *future = task->async->future;
        if (future) {
            state.awaiting = future->function_ctx.function;
        }

        const char *pos = task->vm
            ? flecs_script_ir_vm_stmt_pos(task->vm)
            : flecs_script_runner_stmt_pos(task->runner);
        flecs_script_pos_to_line_col(task->script->code, pos,
            &state.line, &state.column);

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

/* Async blocks */

static void flecs_script_task_sched_add(
    ecs_script_task_t *task)
{
    ecs_script_runtime_t *rt = flecs_script_runtime_get(task->script->world);
    task->sched_index = ecs_vec_count(&rt->async_tasks);
    ecs_vec_append_t(NULL, &rt->async_tasks, ecs_script_task_t*)[0] = task;
}

static void flecs_script_task_sched_remove(
    ecs_script_task_t *task)
{
    int32_t index = task->sched_index;
    if (index < 0) {
        return;
    }

    task->sched_index = -1;

    ecs_script_runtime_t *rt = flecs_script_runtime_get(task->script->world);
    ecs_script_task_t **tasks = ecs_vec_first(&rt->async_tasks);
    int32_t count = ecs_vec_count(&rt->async_tasks);
    ecs_assert(index < count && tasks[index] == task,
        ECS_INTERNAL_ERROR, NULL);
    if (rt->async_progressing) {
        tasks[index] = NULL;
        return;
    }

    ecs_script_task_t *last = tasks[count - 1];
    tasks[index] = last;
    if (last) {
        last->sched_index = index;
    }

    ecs_vec_remove_last(&rt->async_tasks);
}

static ecs_script_vars_t* flecs_script_vars_snapshot(
    ecs_world_t *world,
    const ecs_script_vars_t *src,
    const ecs_script_template_t *template,
    ecs_stack_t *stack,
    ecs_allocator_t *allocator)
{
    ecs_script_vars_t *parent = NULL;
    if (src->parent) {
        parent = flecs_script_vars_snapshot(
            world, src->parent, template, stack, allocator);
    }

    ecs_script_vars_t *dst = flecs_script_vars_push(parent, stack, allocator);
    dst->world = world;
    dst->sp = src->sp;

    int32_t i, count = ecs_vec_count(&src->vars);
    ecs_script_var_t *vars = ecs_vec_first(&src->vars);
    for (i = 0; i < count; i ++) {
        ecs_script_var_t *src_var = &vars[i];
        ecs_script_var_t *var = ecs_script_vars_declare(dst, src_var->name);
        if (!var) {
            var = ecs_script_vars_declare(dst, NULL);
        }

        ecs_assert(var != NULL, ECS_INTERNAL_ERROR, NULL);
        ecs_assert(var->sp == src_var->sp, ECS_INTERNAL_ERROR, NULL);

        const ecs_type_info_t *ti = src_var->type_info;
        if (!ti && src_var->value.type) {
            ti = ecs_get_type_info(world, src_var->value.type);
        }

        var->value.type = src_var->value.type;
        var->type_info = ti;
        var->is_const = src_var->is_const;
        if (template) {
            const ecs_script_template_member_t *members = ecs_vec_first(
                &template->members);
            int32_t member_count = ecs_vec_count(&template->members);
            for (int32_t m = 0; m < member_count; m ++) {
                if (members[m].sp == src_var->sp && members[m].is_mut) {
                    var->value.ptr = src_var->value.ptr;
                    break;
                }
            }

            if (var->value.ptr) {
                continue;
            }
        }

        if (src_var->value.ptr && ti) {
            var->value.ptr = flecs_stack_alloc(stack, ti->size, ti->alignment);
            flecs_type_info_ctor(var->value.ptr, 1, ti);
            ecs_ptr_copy_w_type_info(world, ti, var->value.ptr,
                src_var->value.ptr);
            var->owned = true;
        }
    }

    return dst;
}

static void flecs_script_task_state_init(
    ecs_script_task_t *task,
    const ecs_script_state_t *src)
{
    ecs_script_state_t *state = &task->state;
    flecs_script_state_init(state);
    flecs_script_state_resize(state,
        ecs_vec_count(&src->scope_slots),
        ecs_vec_count(&src->component_slots),
        ecs_vec_count(&src->for_slots),
        ecs_vec_count(&src->computed));
    ecs_vec_fini_t(NULL, &state->symbol_slots, ecs_script_symbol_slot_t);
    state->symbol_slots = ecs_vec_copy_t(
        NULL, &src->symbol_slots, ecs_script_symbol_slot_t);
}

int flecs_script_async_spawn(
    ecs_script_eval_visitor_t *v,
    ecs_script_async_t *node)
{
    ecs_script_impl_t *impl = v->base.script;
    ecs_world_t *world = v->world;
    ecs_entity_t entity = 0;
    ecs_entity_t owner_instance = 0, owner_template = 0;

    if (v->instance_template && v->body_template) {
        entity = v->entity ? v->entity->eval : 0;
        if (!entity || !ecs_is_alive(world, entity)) {
            flecs_script_eval_error(v, node,
                "async block has no template instance");
            return -1;
        }

        owner_instance = v->template_instance;
        owner_template = v->body_template;
        EcsScriptTemplateRoot *root = ecs_get_mut_pair(
            world, owner_instance, EcsScriptTemplateRoot, owner_template);
        ecs_assert(root != NULL, ECS_INTERNAL_ERROR, NULL);
        for (int32_t i = 0; i < ecs_vec_count(&root->async_blocks); ) {
            ecs_script_async_block_t *block = ecs_vec_get_t(
                &root->async_blocks, ecs_script_async_block_t, i);
            if (!ecs_is_alive(world, block->entity)) {
                ecs_vec_remove_t(&root->async_blocks, ecs_script_async_block_t, i);
                continue;
            }

            if (block->entity == entity && block->scope == node->scope) {
                return 0;
            }

            i ++;
        }
    } else {
        entity = v->script_entity;
    }

    ecs_script_task_t *task = flecs_script_task_alloc(impl);
    task->entity = entity;
    task->loop = EcsScriptTaskLoopOnce;
    task->is_async_block = true;
    task->owner_instance = owner_instance;
    task->owner_template = owner_template;

    ecs_script_runtime_t *rt = task->eval_desc.runtime;
    ecs_script_eval_visitor_t *tv = task->v;
    tv->script_entity = v->script_entity;
    tv->script_tag = v->script_tag;
    tv->module = v->module;
    tv->symbol_offset = v->symbol_offset;
    tv->template_instance = owner_instance;

    int32_t i, using_count = ecs_vec_count(&v->r->using);
    ecs_entity_t *using = ecs_vec_first(&v->r->using);
    for (i = 0; i < using_count; i ++) {
        ecs_vec_append_t(rt->allocator, &rt->using, ecs_entity_t)[0] =
            using[i];
    }

    if (v->vars) {
        tv->vars = flecs_script_vars_snapshot(
            world, v->vars, v->instance_template, &rt->stack, rt->allocator);
    }

    if (owner_instance) {
        flecs_script_task_push_this(tv, rt, entity);
    }

    flecs_script_task_state_init(task, v->state);
    tv->state = &task->state;

    task->scope = node->scope;
    if (flecs_script_task_engine_init(task, node)) {
        flecs_script_eval_error(v, node,
            "failed to create task for async block");
        flecs_script_task_dealloc(task);
        return -1;
    }

    task->initial_vars = tv->vars;

    if (owner_template) {
        EcsScriptTemplateRoot *root = ecs_ensure_pair(
            world, owner_instance, EcsScriptTemplateRoot, owner_template);
        ecs_vec_append_t(NULL, &root->tasks, ecs_script_task_t*)[0] = task;
        impl->refcount ++;
        task->retains_script = true;
        ecs_script_async_block_t *block = ecs_vec_append_t(
            NULL, &root->async_blocks, ecs_script_async_block_t);
        block->entity = entity;
        block->scope = node->scope;
        block->scope_slot = v->scope_slot;
        if (entity != owner_instance) {
            EcsScriptAsyncTasks *bound = ecs_ensure(
                world, entity, EcsScriptAsyncTasks);
            ecs_vec_init_if_t(&bound->tasks, ecs_script_task_t*);
            ecs_vec_append_t(NULL, &bound->tasks, ecs_script_task_t*)[0] = task;
            task->bound_registered = true;
        }
    } else {
        ecs_vec_append_t(NULL, &impl->async_tasks, ecs_script_task_t*)[0] =
            task;
    }

    flecs_script_task_sched_add(task);
    return 0;
}

void flecs_script_async_tasks_free(
    ecs_vec_t *tasks)
{
    int32_t i, count = ecs_vec_count(tasks);
    ecs_script_task_t **array = ecs_vec_first(tasks);
    for (i = 0; i < count; i ++) {
        ecs_script_task_t *task = array[i];
        task->owner_instance = 0;
        task->owner_template = 0;
        ecs_script_task_free(task);
    }

    ecs_vec_clear(tasks);
}

static void flecs_script_task_detach(
    ecs_script_task_t *task)
{
    ecs_vec_t *owner = NULL;
    ecs_world_t *world = task->script->world;
    if (task->owner_template) {
        if (ecs_is_alive(world, task->owner_instance) &&
            ecs_is_alive(world, task->owner_template))
        {
            EcsScriptTemplateRoot *root = ecs_get_mut_pair(world,
                task->owner_instance, EcsScriptTemplateRoot,
                task->owner_template);
            if (root) {
                owner = &root->tasks;
            }
        }
    } else {
        owner = &flecs_script_impl(task->script)->async_tasks;
    }

    task->owner_instance = 0;
    task->owner_template = 0;
    if (owner) {
        flecs_script_task_vec_remove(owner, task);
    }
}

void flecs_script_async_cleanup(
    ecs_script_eval_visitor_t *v)
{
    EcsScriptTemplateRoot *root = ecs_get_mut_pair(v->world,
        v->template_instance, EcsScriptTemplateRoot, v->body_template);
    ecs_assert(root != NULL, ECS_INTERNAL_ERROR, NULL);
    for (int32_t i = 0; i < ecs_vec_count(&root->async_blocks); ) {
        ecs_script_async_block_t block = ecs_vec_get_t(
            &root->async_blocks, ecs_script_async_block_t, i)[0];
        if (ecs_is_alive(v->world, block.entity) &&
            flecs_script_scope_visited(v, block.scope_slot))
        {
            i ++;
            continue;
        }

        ecs_vec_remove_t(&root->async_blocks, ecs_script_async_block_t, i);
        for (int32_t t = 0; t < ecs_vec_count(&root->tasks); t ++) {
            ecs_script_task_t *task = ecs_vec_get_t(
                &root->tasks, ecs_script_task_t*, t)[0];
            if (task->entity == block.entity && task->scope == block.scope) {
                flecs_script_task_detach(task);
                ecs_script_task_free(task);
                break;
            }
        }
    }
}

static ECS_DTOR(EcsScriptAsyncTasks, ptr, {
    ecs_vec_fini_t(NULL, &ptr->tasks, ecs_script_task_t*);
})

static void flecs_script_async_tasks_on_remove(
    ecs_iter_t *it)
{
    EcsScriptAsyncTasks *bound = ecs_field(it, EcsScriptAsyncTasks, 0);
    for (int32_t i = 0; i < it->count; i ++) {
        ecs_vec_t tasks = bound[i].tasks;
        ecs_os_zeromem(&bound[i].tasks);
        int32_t count = ecs_vec_count(&tasks);
        ecs_script_task_t **array = ecs_vec_first(&tasks);
        for (int32_t t = 0; t < count; t ++) {
            array[t]->bound_registered = false;
            flecs_script_task_detach(array[t]);
        }

        for (int32_t t = 0; t < count; t ++) {
            ecs_script_task_free(array[t]);
        }

        ecs_vec_fini_t(NULL, &tasks, ecs_script_task_t*);
    }
}

int32_t ecs_script_tasks_progress(
    ecs_world_t *world)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    world = ECS_CONST_CAST(ecs_world_t*, ecs_get_world(world));

    ecs_script_runtime_t *rt = flecs_script_runtime_get(world);
    if (rt->async_progressing) {
        return 0;
    }

    bool is_readonly = ecs_stage_is_readonly(world);
    ecs_suspend_readonly_state_t srs;
    ecs_world_t *real_world = NULL;
    if (is_readonly) {
        real_world = flecs_suspend_readonly(world, &srs);
        ecs_assert(real_world != NULL, ECS_INTERNAL_ERROR, NULL);
    }

    rt->async_progressing = true;

    int32_t i, resumed = 0;
    for (i = 0; i < ecs_vec_count(&rt->async_tasks); i ++) {
        ecs_script_task_t *task = ecs_vec_get_t(
            &rt->async_tasks, ecs_script_task_t*, i)[0];
        if (!task) {
            continue;
        }

        if (task->status != EcsScriptTaskPending ||
            !ecs_script_task_is_ready(task))
        {
            continue;
        }

        /* Keep the script alive while the task runs, in case the task frees
         * its own script (e.g. by deleting or updating the script entity). */
        ecs_script_impl_t *impl = flecs_script_impl(task->script);
        impl->refcount ++;

        ecs_script_task_status_t status = ecs_script_task_resume(task, NULL);
        resumed ++;

        if (task->free_pending) {
            ecs_script_task_free(task);
        } else if (status != EcsScriptTaskPending) {
            flecs_script_task_detach(task);
            ecs_script_task_free(task);
        }

        ecs_script_free(&impl->pub);
    }

    ecs_script_task_t **tasks = ecs_vec_first(&rt->async_tasks);
    int32_t count = ecs_vec_count(&rt->async_tasks), dst = 0;
    for (i = 0; i < count; i ++) {
        if (tasks[i]) {
            tasks[i]->sched_index = dst;
            tasks[dst ++] = tasks[i];
        }
    }

    ecs_vec_set_count_t(NULL, &rt->async_tasks, ecs_script_task_t*, dst);

    rt->async_progressing = false;

    if (is_readonly) {
        flecs_resume_readonly(real_world, &srs);
    }

    return resumed;
error:
    return 0;
}

#ifdef FLECS_PIPELINE
static void flecs_script_progress_tasks_system(
    ecs_iter_t *it)
{
    ecs_world_t *world = ECS_CONST_CAST(ecs_world_t*, ecs_get_world(it->world));
    ecs_script_runtime_t *rt = world->stages[0]->runtime;
    if (!rt || !ecs_vec_count(&rt->async_tasks)) {
        return;
    }

    ecs_script_tasks_progress(world);
}
#endif

void flecs_script_async_import(
    ecs_world_t *world)
{
    ECS_COMPONENT_DEFINE(world, EcsScriptTask);
    ECS_COMPONENT_DEFINE(world, EcsScriptAsyncTasks);
    ecs_set_hooks(world, EcsScriptAsyncTasks, {
        .ctor = flecs_default_ctor,
        .move = ecs_move(EcsScriptTask),
        .dtor = ecs_dtor(EcsScriptAsyncTasks),
        .on_remove = flecs_script_async_tasks_on_remove,
        .flags = ECS_TYPE_HOOK_COPY_ILLEGAL
    });
    ecs_add_pair(world, ecs_id(EcsScriptAsyncTasks),
        EcsOnInstantiate, EcsDontInherit);

#ifdef FLECS_PIPELINE
    ECS_IMPORT(world, FlecsPipeline);

    ecs_system(world, {
        .entity = ecs_entity(world, { .name = "ProgressTasks" }),
        .phase = EcsPreUpdate,
        .callback = flecs_script_progress_tasks_system,
        .immediate = true
    });
#endif

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
