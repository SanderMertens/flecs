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
static ECS_TAG_DECLARE(EcsScriptTasksPending);

struct ecs_script_task_t {
    ecs_script_t *script;
    ecs_script_runner_t *runner;
    ecs_script_ir_vm_t *vm;
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

    /* Scope (runner) or entry (IR) that the task runs. Defaults to the script
     * root, or the body of an async block. */
    ecs_script_scope_t *scope;
    const ecs_script_ir_entry_t *entry;

    /* Async block tasks */
    ecs_script_state_t state;
    ecs_entity_t owner_instance;
    ecs_entity_t owner_template;
    int32_t sched_index;
    bool is_async_block;
    bool has_state;
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

ecs_script_future_t* flecs_script_future_start(
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

    int32_t arg_offset = expr->kind == EcsExprMethod;
    int32_t argc = ecs_vec_count(&call->args->elements);
    int32_t value_count = argc + arg_offset;
    ecs_value_t *argv = value_count
        ? ecs_os_alloca_n(ecs_value_t, value_count) : NULL;
    ecs_expr_initializer_element_t *elems = ecs_vec_first(
        &call->args->elements);
    int32_t evaluated = 0;
    int result = 0;
    for (int32_t i = 0; i < value_count; i ++) {
        ecs_expr_node_t **arg = arg_offset && !i
            ? &call->left : &elems[i - arg_offset].value;
        argv[i] = ecs_value_new(v->world, (*arg)->type);
        evaluated ++;
        if (flecs_script_eval_expr(v, arg, &argv[i])) {
            result = -1;
            goto done;
        }
    }

    r->async.future = flecs_script_future_start(v->world, r->async.entity,
        call->node.type, &call->calldata, argc, argv);

done:
    for (int32_t i = result ? arg_offset : 0; i < evaluated; i ++) {
        ecs_value_fini(v->world, &argv[i]);
    }
    if (result && arg_offset) {
        ecs_value_fini(v->world, &argv[0]);
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

int32_t flecs_script_find_catch(
    ecs_script_eval_visitor_t *v,
    const flecs_script_async_state_t *state,
    const ecs_script_try_t *node)
{
    ecs_entity_t throw_id = state->thrown->error_id;
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

    if (!r->async.future) {
        if (flecs_script_await_start(r, stmt, expr)) {
            return -1;
        }
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
    if (frame->node->kind != EcsAstTry || !r->async.thrown ||
        frame->pc != 1)
    {
        return false;
    }

    ecs_script_try_t *node = (ecs_script_try_t*)frame->node;
    int32_t catch_index = flecs_script_find_catch(&r->v, &r->async, node);
    if (catch_index == -1) {
        return false;
    }

    flecs_script_throw_clear(&r->async);
    frame->pc = 2;
    ecs_script_catch_t *catches = ecs_vec_first(&node->catches);
    flecs_script_scope_push(r, catches[catch_index].scope);
    return true;
}

static ecs_script_eval_visitor_t* flecs_script_task_visitor(
    ecs_script_task_t *task)
{
    if (task->vm) {
        return &task->vm->v;
    }
    return &task->runner->v;
}

static void flecs_script_task_push_this(
    ecs_script_eval_visitor_t *v,
    ecs_script_runtime_t *runtime,
    ecs_entity_t entity)
{
    v->vars = flecs_script_vars_push(
        v->vars, &runtime->stack, &runtime->allocator);
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
    } else {
        result->runner = ECS_OFFSET(result, offset);
    }
    result->script = &impl->pub;
    result->sched_index = -1;
    impl->task_refcount ++;
    ecs_script_runtime_t *runtime = ecs_script_runtime_new();
    result->eval_desc.runtime = runtime;
    ecs_script_eval_visitor_t *v = flecs_script_task_visitor(result);
    flecs_script_eval_visit_init(impl, v, &result->eval_desc);
    return result;
}

static int flecs_script_task_compile(
    ecs_script_task_t *task)
{
    ecs_script_impl_t *impl = flecs_script_impl(task->script);
    if (impl->compiled) {
        return 0;
    }
    ecs_script_eval_visitor_t *v = flecs_script_task_visitor(task);
    if (flecs_script_visit_include(v, impl->root) ||
        flecs_script_visit_type(v, impl->root))
    {
        return -1;
    }
    impl->compiled = true;
    return 0;
}

static int flecs_script_task_engine_init(
    ecs_script_task_t *task)
{
    ecs_script_eval_visitor_t *v = flecs_script_task_visitor(task);
    if (task->vm) {
        flecs_script_ir_vm_init(task->vm);
        if (!task->vm->ir) {
            flecs_script_ir_vm_fini(task->vm, &task->eval_desc);
            return -1;
        }
        task->vm->can_suspend = true;
        task->vm->async.entity = task->entity;
    } else {
        flecs_script_runner_init(task->runner, v);
        task->runner->can_suspend = true;
        task->runner->async.entity = task->entity;
    }
    return 0;
}

static void flecs_script_task_dealloc(
    ecs_script_task_t *task)
{
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
    ecs_script_runtime_t *runtime = result->eval_desc.runtime;
    if (desc) {
        result->entity = desc->entity;
        result->ctx = desc->ctx;
        result->ctx_free = desc->ctx_free;
        result->loop = desc->loop;
        result->iterations = desc->iterations;
    }
    ecs_script_eval_visitor_t *v = flecs_script_task_visitor(result);
    if (result->entity) {
        flecs_script_task_push_this(v, runtime, result->entity);
        result->has_owner_vars = true;
    }
    if (flecs_script_task_compile(result)) {
        goto task_error;
    }
    result->scope = impl->root;
    if (flecs_script_task_engine_init(result)) {
        goto task_error;
    }
    if (result->vm) {
        result->entry = ecs_vec_get_t(&result->vm->ir->entries,
            ecs_script_ir_entry_t, result->vm->ir->root_entry);
    }
    if (result->entity) {
        flecs_script_task_register(result);
    }
    result->initial_vars = flecs_script_task_visitor(result)->vars;
    return result;
task_error:
    if (result->has_owner_vars) {
        v->vars = ecs_script_vars_pop(v->vars);
    }
    flecs_script_eval_visit_fini(v, &result->eval_desc);
    flecs_script_task_dealloc(result);
error:
    return NULL;
}

static ecs_script_future_t** flecs_script_task_future(
    ecs_script_task_t *task)
{
    if (task->vm) {
        return &task->vm->async.future;
    }
    return &task->runner->async.future;
}

static void flecs_script_task_cancel_future(
    ecs_script_task_t *task)
{
    ecs_script_future_t **future_ptr = flecs_script_task_future(task);
    ecs_script_future_t *future = *future_ptr;
    *future_ptr = NULL;
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
    ecs_script_runtime_t *world_rt = flecs_script_runtime_get(
        flecs_script_task_visitor(task)->world);
    bool prev_error = world_rt->error;
    world_rt->error = false;
    flecs_script_run_status_t status;
    if (task->vm) {
        status = flecs_script_ir_vm_run(task->vm, task->entry);
    } else {
        status = flecs_script_runner_run_scope(task->runner, task->scope);
    }
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
            ecs_assert(flecs_script_task_visitor(task)->vars ==
                task->initial_vars, ECS_INTERNAL_ERROR, NULL);
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
    ecs_script_future_t *future = *flecs_script_task_future(
        ECS_CONST_CAST(ecs_script_task_t*, task));
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

static void flecs_script_task_sched_remove(
    ecs_script_task_t *task);

void ecs_script_task_free(
    ecs_script_task_t *task)
{
    if (!task) {
        return;
    }
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
        flecs_script_ir_vm_abandon(task->vm);
    } else {
        flecs_script_runner_abandon(task->runner);
    }
    ecs_script_eval_visitor_t *v = flecs_script_task_visitor(task);
    ecs_assert(v->vars == task->initial_vars, ECS_INTERNAL_ERROR, NULL);
    if (task->has_owner_vars) {
        v->vars = ecs_script_vars_pop(v->vars);
    }
    if (task->is_async_block) {
        while (v->vars) {
            v->vars = ecs_script_vars_pop(v->vars);
        }
    }
    if (task->vm) {
        flecs_script_ir_vm_fini(task->vm, &task->eval_desc);
    } else {
        flecs_script_runner_fini(task->runner, &task->eval_desc);
    }
    if (task->has_state) {
        flecs_script_state_fini(&task->state);
    }
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

        ecs_script_future_t *future = task->vm
            ? task->vm->async.future
            : task->runner->async.future;
        if (future) {
            state.awaiting = future->function_ctx.function;
        }

        const char *pos = task->vm
            ? flecs_script_ir_vm_stmt_pos(task->vm)
            : flecs_script_runner_stmt_pos(task->runner);
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

/* Async blocks */

/* The ProgressTasks system matches this tag, which is added to the tag entity
 * itself while async block tasks exist. This keeps the system inactive (and
 * free) for worlds that don't use async blocks. */
static void flecs_script_tasks_pending_set(
    ecs_world_t *world,
    bool pending)
{
    if (!EcsScriptTasksPending || ecs_is_fini(world)) {
        return;
    }
    if (pending) {
        ecs_add_id(world, EcsScriptTasksPending, EcsScriptTasksPending);
    } else {
        ecs_remove_id(world, EcsScriptTasksPending, EcsScriptTasksPending);
    }
}

static void flecs_script_task_sched_add(
    ecs_script_task_t *task)
{
    ecs_world_t *world = task->script->world;
    ecs_script_runtime_t *rt = flecs_script_runtime_get(world);
    task->sched_index = ecs_vec_count(&rt->async_tasks);
    ecs_vec_append_t(NULL, &rt->async_tasks, ecs_script_task_t*)[0] = task;
    if (task->sched_index == 0) {
        flecs_script_tasks_pending_set(world, true);
    }
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
    if (!ecs_vec_count(&rt->async_tasks)) {
        flecs_script_tasks_pending_set(task->script->world, false);
    }
}

static ecs_script_vars_t* flecs_script_vars_snapshot(
    ecs_world_t *world,
    const ecs_script_vars_t *src,
    ecs_stack_t *stack,
    ecs_allocator_t *allocator)
{
    ecs_script_vars_t *parent = NULL;
    if (src->parent) {
        parent = flecs_script_vars_snapshot(
            world, src->parent, stack, allocator);
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
        ecs_vec_count(&src->for_slots));
    flecs_script_state_resize_computed(state, ecs_vec_count(&src->computed));
    ecs_vec_fini_t(NULL, &state->symbol_slots, ecs_script_symbol_slot_t);
    state->symbol_slots = ecs_vec_copy_t(
        NULL, &src->symbol_slots, ecs_script_symbol_slot_t);
    task->has_state = true;
}

int flecs_script_async_spawn(
    ecs_script_eval_visitor_t *v,
    ecs_script_async_t *node)
{
    ecs_script_impl_t *impl = v->base.script;
    ecs_world_t *world = v->world;
    ecs_entity_t entity = 0;
    ecs_entity_t owner_instance = 0, owner_template = 0;

    if (v->instance_template && v->template_entity) {
        entity = v->entity ? v->entity->eval : 0;
        if (!entity || !ecs_is_alive(world, entity)) {
            flecs_script_eval_error(v, node,
                "async block has no template instance");
            return -1;
        }
        owner_instance = entity;
        owner_template = v->template_entity;
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
    ecs_script_eval_visitor_t *tv = flecs_script_task_visitor(task);
    tv->script_entity = v->script_entity;
    tv->script_tag = v->script_tag;
    tv->module = v->module;
    tv->symbol_offset = v->symbol_offset;

    int32_t i, using_count = ecs_vec_count(&v->r->using);
    ecs_entity_t *using = ecs_vec_first(&v->r->using);
    for (i = 0; i < using_count; i ++) {
        ecs_vec_append_t(&rt->allocator, &rt->using, ecs_entity_t)[0] =
            using[i];
    }

    if (v->vars) {
        tv->vars = flecs_script_vars_snapshot(
            world, v->vars, &rt->stack, &rt->allocator);
    }

    flecs_script_task_state_init(task, v->state);
    tv->state = &task->state;

    task->scope = node->scope;
    if (flecs_script_task_engine_init(task)) {
        goto error;
    }
    if (task->vm) {
        task->entry = flecs_script_ir_entry(task->vm->ir, node);
        if (!task->entry) {
            flecs_script_ir_vm_fini(task->vm, &task->eval_desc);
            goto error;
        }
    }
    task->initial_vars = tv->vars;

    if (owner_template) {
        EcsScriptTemplateRoot *root = ecs_ensure_pair(
            world, owner_instance, EcsScriptTemplateRoot, owner_template);
        ecs_vec_append_t(NULL, &root->tasks, ecs_script_task_t*)[0] = task;
        impl->refcount ++;
        task->retains_script = true;
    } else {
        ecs_vec_append_t(NULL, &impl->async_tasks, ecs_script_task_t*)[0] =
            task;
    }

    flecs_script_task_sched_add(task);
    return 0;
error:
    flecs_script_eval_error(v, node, "failed to create task for async block");
    while (tv->vars) {
        tv->vars = ecs_script_vars_pop(tv->vars);
    }
    flecs_script_state_fini(&task->state);
    flecs_script_eval_visit_fini(tv, &task->eval_desc);
    flecs_script_task_dealloc(task);
    return -1;
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

    if (!owner) {
        return;
    }

    int32_t i, count = ecs_vec_count(owner);
    ecs_script_task_t **tasks = ecs_vec_first(owner);
    for (i = 0; i < count; i ++) {
        if (tasks[i] == task) {
            ecs_vec_remove_t(owner, ecs_script_task_t*, i);
            break;
        }
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

    bool is_defer = ecs_is_deferred(world);
    ecs_suspend_readonly_state_t srs;
    ecs_world_t *real_world = NULL;
    if (is_defer) {
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

        ecs_script_task_status_t status = ecs_script_task_resume(task, NULL);
        resumed ++;

        if (task->free_pending) {
            ecs_script_task_free(task);
        } else if (status != EcsScriptTaskPending) {
            flecs_script_task_detach(task);
            ecs_script_task_free(task);
        }
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
    if (!dst) {
        flecs_script_tasks_pending_set(world, false);
    }

    rt->async_progressing = false;

    if (is_defer) {
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
    ecs_script_tasks_progress(it->world);
}
#endif

void flecs_script_async_import(
    ecs_world_t *world)
{
    ECS_COMPONENT_DEFINE(world, EcsScriptTask);
    ECS_TAG_DEFINE(world, EcsScriptTasksPending);

#ifdef FLECS_PIPELINE
    ECS_IMPORT(world, FlecsPipeline);

    ecs_system(world, {
        .entity = ecs_entity(world, { .name = "ProgressTasks" }),
        .phase = EcsPreUpdate,
        .query.terms = {{
            .id = EcsScriptTasksPending,
            .inout = EcsInOutNone
        }},
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
