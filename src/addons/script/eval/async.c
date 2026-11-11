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

typedef struct flecs_script_event_value_t {
    void *ptr;
    uint64_t pass;
} flecs_script_event_value_t;

typedef struct flecs_script_event_sub_t {
    ecs_entity_t event;
    ecs_entity_t type;
    ecs_script_future_t *future;
    ecs_vec_t queue; /* vec<flecs_script_event_value_t> */
} flecs_script_event_sub_t;

struct ecs_script_task_t {
    ecs_vec_t event_subs; /* vec<flecs_script_event_sub_t> */
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

static void flecs_script_sched_lock(
    ecs_script_runtime_t *rt)
{
    if (rt->async_lock) {
        ecs_os_mutex_lock(rt->async_lock);
    }
}

static void flecs_script_sched_unlock(
    ecs_script_runtime_t *rt)
{
    if (rt->async_lock) {
        ecs_os_mutex_unlock(rt->async_lock);
    }
}

static bool flecs_script_sched_is_ready(
    const ecs_script_runtime_t *rt,
    int32_t index)
{
    int32_t word = index >> 6;
    if (word >= ecs_vec_count(&rt->async_ready)) {
        return false;
    }

    uint64_t bits = ecs_vec_first_t(&rt->async_ready, uint64_t)[word];
    return (bits & (1ull << (index & 63))) != 0;
}

static void flecs_script_sched_set_ready(
    ecs_script_runtime_t *rt,
    int32_t index,
    bool ready)
{
    int32_t word = index >> 6;
    uint64_t bit = 1ull << (index & 63);
    if (ready) {
        ecs_vec_set_min_count_zeromem_t(
            NULL, &rt->async_ready, uint64_t, word + 1);
        ecs_vec_first_t(&rt->async_ready, uint64_t)[word] |= bit;
    } else if (word < ecs_vec_count(&rt->async_ready)) {
        ecs_vec_first_t(&rt->async_ready, uint64_t)[word] &= ~bit;
    }
}

static void flecs_script_task_wake(
    ecs_script_task_t *task)
{
    ecs_script_runtime_t *rt = flecs_script_runtime_get(task->script->world);
    flecs_script_sched_lock(rt);
    if (task->sched_index >= 0) {
        flecs_script_sched_set_ready(rt, task->sched_index, true);
    }
    flecs_script_sched_unlock(rt);
}

static void flecs_script_future_wake(
    ecs_script_future_t *future)
{
    ecs_script_task_t *task = future->waiter;
    if (task) {
        future->waiter = NULL;
        flecs_script_task_wake(task);
    }
}

static void flecs_script_future_unwait(
    ecs_script_future_t *future)
{
    if (future) {
        flecs_script_future_lock(future);
        future->waiter = NULL;
        flecs_script_future_unlock(future);
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

ecs_script_future_t* flecs_script_future_launch(
    ecs_world_t *world,
    const ecs_function_calldata_t *calldata,
    ecs_entity_t type,
    ecs_entity_t entity,
    int32_t argc,
    const ecs_value_t *argv)
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

void flecs_script_await_launch(
    ecs_world_t *world,
    flecs_script_async_state_t *async,
    const ecs_expr_function_t *call,
    ecs_value_t *argv)
{
    async->future = flecs_script_future_launch(world, &call->calldata,
        call->node.type, async->entity,
        ecs_vec_count(&call->args->elements), argv);
}

ecs_script_future_status_t ecs_script_future_status(
    const ecs_script_future_t *future)
{
    ecs_check(future != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_script_future_t *ptr = ECS_CONST_CAST(ecs_script_future_t*, future);
    flecs_script_future_lock(ptr);
    ecs_script_future_status_t status = ptr->status;
    flecs_script_future_unlock(ptr);
    return status;
error:
    return EcsScriptFutureCancelled;
}

const ecs_value_t* ecs_script_future_get_value(
    const ecs_script_future_t *future)
{
    if (ecs_script_future_status(future) != EcsScriptFutureResolved) {
        return NULL;
    }

    return &future->value;
}

const char* ecs_script_future_get_error(
    const ecs_script_future_t *future)
{
    if (ecs_script_future_status(future) != EcsScriptFutureRejected) {
        return NULL;
    }

    return future->error;
}

ecs_entity_t ecs_script_future_get_error_id(
    const ecs_script_future_t *future)
{
    if (ecs_script_future_status(future) != EcsScriptFutureRejected) {
        return 0;
    }

    return future->error_id;
}

void ecs_script_future_cancel(
    ecs_script_future_t *future)
{
    ecs_check(future != NULL, ECS_INVALID_PARAMETER, NULL);
    flecs_script_future_lock(future);
    bool cancel = future->status == EcsScriptFuturePending;
    if (cancel) {
        future->status = EcsScriptFutureCancelled;
        flecs_script_future_wake(future);
    }

    flecs_script_future_unlock(future);
    if (cancel && future->cancel) {
        future->cancel(&future->function_ctx, future);
    }
error:
    return;
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
    if (future->status != EcsScriptFuturePending) {
        flecs_script_future_unlock(future);
        goto error;
    }

    future->value = ecs_value_new(future->function_ctx.world, future->type);
    ecs_ptr_copy(future->function_ctx.world, future->type,
        future->value.ptr, value->ptr);
    future->status = EcsScriptFutureResolved;
    flecs_script_future_wake(future);
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
    if (future->status != EcsScriptFuturePending) {
        flecs_script_future_unlock(future);
        goto error;
    }

    future->error = ecs_os_strdup(msg ? msg : "async function failed");
    future->error_id = error;
    future->status = EcsScriptFutureRejected;
    flecs_script_future_wake(future);
    flecs_script_future_unlock(future);
    return 0;
error:
    return -1;
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
    flecs_script_future_unwait(state->future);
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
    ecs_script_future_status_t status = ecs_script_future_status(async->future);
    if (status == EcsScriptFuturePending) {
        return 1;
    }

    ecs_script_future_t *future = async->future;
    async->future = NULL;
    if (status == EcsScriptFutureRejected) {
        flecs_script_throw_clear(async);
        async->thrown = future;
        async->throw_node = stmt;
        return -1;
    }

    if (status == EcsScriptFutureCancelled) {
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
    ecs_vec_init_t(NULL, &result->event_subs, flecs_script_event_sub_t, 0);
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

static flecs_script_event_sub_t* flecs_script_task_event_subscribe(
    ecs_script_task_t *task,
    ecs_entity_t event);

static void flecs_script_task_event_subs_fini(
    ecs_script_task_t *task)
{
    ecs_world_t *world = task->v->world;
    ecs_script_runtime_t *rt = flecs_script_runtime_get(world);
    flecs_script_event_sub_t *subs = ecs_vec_first(&task->event_subs);
    int32_t s, sub_count = ecs_vec_count(&task->event_subs);
    for (s = 0; s < sub_count; s ++) {
        flecs_script_event_sub_t *sub = &subs[s];
        if (sub->future) {
            ecs_script_future_release(sub->future);
        }

        const ecs_type_info_t *ti = ecs_get_type_info(world, sub->type);
        flecs_script_event_value_t *values = ecs_vec_first(&sub->queue);
        int32_t v, value_count = ecs_vec_count(&sub->queue);
        for (v = 0; v < value_count; v ++) {
            ecs_ptr_free_w_type_info(world, ti, values[v].ptr);
        }

        rt->event_queued -= value_count;
        ecs_vec_fini_t(NULL, &sub->queue, flecs_script_event_value_t);
    }

    ecs_vec_fini_t(NULL, &task->event_subs, flecs_script_event_sub_t);
}

static void flecs_script_task_dealloc(
    ecs_script_task_t *task)
{
    ecs_script_eval_visitor_t *v = task->v;
    flecs_script_task_event_subs_fini(task);

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
        flecs_script_future_unwait(future);
        ecs_script_future_cancel(future);
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
    ecs_script_task_t *prev_task = world_rt->current_task;
    world_rt->current_task = task;
    flecs_script_run_status_t status;
    if (task->vm) {
        status = flecs_script_ir_vm_run(task->vm, task->entry);
    } else {
        status = flecs_script_runner_run_scope(task->runner, task->scope);
    }

    world_rt->current_task = prev_task;
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

    return ecs_script_future_status(future) != EcsScriptFuturePending;
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
    if (!rt->async_lock && ecs_os_has_threading()) {
        rt->async_lock = ecs_os_mutex_new();
    }

    flecs_script_sched_lock(rt);
    task->sched_index = ecs_vec_count(&rt->async_tasks);
    ecs_vec_append_t(NULL, &rt->async_tasks, ecs_script_task_t*)[0] = task;
    flecs_script_sched_set_ready(rt, task->sched_index, true);
    flecs_script_sched_unlock(rt);
}

static void flecs_script_task_sched_remove(
    ecs_script_task_t *task)
{
    int32_t index = task->sched_index;
    if (index < 0) {
        return;
    }

    ecs_script_runtime_t *rt = flecs_script_runtime_get(task->script->world);
    flecs_script_sched_lock(rt);
    task->sched_index = -1;

    ecs_script_task_t **tasks = ecs_vec_first(&rt->async_tasks);
    int32_t last = ecs_vec_count(&rt->async_tasks) - 1;
    ecs_assert(index <= last && tasks[index] == task,
        ECS_INTERNAL_ERROR, NULL);
    flecs_script_sched_set_ready(rt, index, false);
    if (rt->async_progressing) {
        tasks[index] = NULL;
        rt->async_holes = true;
    } else {
        if (index != last) {
            tasks[index] = tasks[last];
            tasks[index]->sched_index = index;
            if (flecs_script_sched_is_ready(rt, last)) {
                flecs_script_sched_set_ready(rt, last, false);
                flecs_script_sched_set_ready(rt, index, true);
            }
        }

        ecs_vec_remove_last(&rt->async_tasks);
    }

    flecs_script_sched_unlock(rt);
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
        ecs_vec_count(&src->query_slots),
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
        block->restart_input = node->restart_input;
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

    ecs_entity_t *events = ecs_vec_first(&node->events);
    int32_t e, event_count = ecs_vec_count(&node->events);
    for (e = 0; e < event_count; e ++) {
        flecs_script_task_event_subscribe(task, events[e]);
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

static void flecs_script_async_block_stop(
    EcsScriptTemplateRoot *root,
    int32_t index)
{
    ecs_script_async_block_t block = ecs_vec_get_t(
        &root->async_blocks, ecs_script_async_block_t, index)[0];
    ecs_vec_remove_t(&root->async_blocks, ecs_script_async_block_t, index);
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

void flecs_script_async_blocks_restart(
    EcsScriptTemplateRoot *root,
    uint64_t input)
{
    for (int32_t i = 0; i < ecs_vec_count(&root->async_blocks); ) {
        ecs_script_async_block_t *block = ecs_vec_get_t(
            &root->async_blocks, ecs_script_async_block_t, i);
        if (block->restart_input & input) {
            flecs_script_async_block_stop(root, i);
        } else {
            i ++;
        }
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

        flecs_script_async_block_stop(root, i);
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

static flecs_script_event_sub_t* flecs_script_task_event_sub(
    ecs_script_task_t *task,
    ecs_entity_t event)
{
    flecs_script_event_sub_t *subs = ecs_vec_first(&task->event_subs);
    int32_t i, count = ecs_vec_count(&task->event_subs);
    for (i = 0; i < count; i ++) {
        if (subs[i].event == event) {
            return &subs[i];
        }
    }

    return NULL;
}

static flecs_script_event_sub_t* flecs_script_task_event_subscribe(
    ecs_script_task_t *task,
    ecs_entity_t event)
{
    flecs_script_event_sub_t *sub = flecs_script_task_event_sub(task, event);
    if (sub) {
        return sub;
    }

    const EcsScriptFunction *f = ecs_get(
        task->v->world, event, EcsScriptFunction);
    ecs_assert(f != NULL, ECS_INTERNAL_ERROR, NULL);

    sub = ecs_vec_append_t(
        NULL, &task->event_subs, flecs_script_event_sub_t);
    ecs_os_zeromem(sub);
    sub->event = event;
    sub->type = f->return_type;
    ecs_vec_init_t(NULL, &sub->queue, flecs_script_event_value_t, 0);
    return sub;
}

static void flecs_script_event_sub_resolve_next(
    ecs_script_runtime_t *rt,
    flecs_script_event_sub_t *sub,
    ecs_script_future_t *future)
{
    ecs_world_t *world = future->function_ctx.world;
    flecs_script_event_value_t value = ecs_vec_first_t(
        &sub->queue, flecs_script_event_value_t)[0];
    ecs_vec_remove_ordered_t(&sub->queue, flecs_script_event_value_t, 0);
    rt->event_queued --;

    ecs_value_t result = { .type = sub->type, .ptr = value.ptr };
    ecs_script_future_resolve(future, &result);
    ecs_script_future_release(future);
    ecs_ptr_free_w_type_info(world,
        ecs_get_type_info(world, sub->type), value.ptr);
}

static void flecs_script_event_wait(
    const ecs_function_ctx_t *ctx,
    int32_t argc,
    const ecs_value_t *argv,
    ecs_script_future_t *future)
{
    (void)argc;
    (void)argv;
    ecs_script_runtime_t *rt = flecs_script_runtime_get(ctx->world);
    ecs_script_task_t *task = rt->current_task;
    if (!task) {
        ecs_script_future_reject(future, "events can only be awaited by tasks");
        ecs_script_future_release(future);
        return;
    }

    flecs_script_event_sub_t *sub = flecs_script_task_event_subscribe(
        task, ctx->function);
    ecs_assert(sub->future == NULL, ECS_INTERNAL_ERROR, NULL);

    /* Events emitted during this pass are delivered in the next one, which
     * prevents a block that emits the event it awaits from never yielding. */
    if (ecs_vec_count(&sub->queue) && ecs_vec_first_t(&sub->queue,
        flecs_script_event_value_t)->pass != rt->event_pass)
    {
        flecs_script_event_sub_resolve_next(rt, sub, future);
    } else {
        sub->future = future;
    }
}

static void flecs_script_event_cancel(
    const ecs_function_ctx_t *ctx,
    ecs_script_future_t *future)
{
    ecs_script_runtime_t *rt = flecs_script_runtime_get(ctx->world);
    ecs_script_task_t **tasks = ecs_vec_first(&rt->async_tasks);
    int32_t t, task_count = ecs_vec_count(&rt->async_tasks);
    for (t = 0; t < task_count; t ++) {
        if (!tasks[t]) {
            continue;
        }

        flecs_script_event_sub_t *sub = flecs_script_task_event_sub(
            tasks[t], ctx->function);
        if (sub && sub->future == future) {
            sub->future = NULL;
            ecs_script_future_release(future);
            return;
        }
    }
}

static void flecs_script_event_deliver(
    ecs_world_t *world,
    ecs_entity_t event,
    ecs_entity_t type,
    const void *value)
{
    ecs_script_runtime_t *rt = flecs_script_runtime_get(world);
    const ecs_type_info_t *ti = ecs_get_type_info(world, type);
    ecs_assert(ti != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_script_task_t **tasks = ecs_vec_first(&rt->async_tasks);
    int32_t t, task_count = ecs_vec_count(&rt->async_tasks);
    for (t = 0; t < task_count; t ++) {
        ecs_script_task_t *task = tasks[t];
        if (!task || task->status != EcsScriptTaskPending) {
            continue;
        }

        flecs_script_event_sub_t *sub = flecs_script_task_event_sub(task, event);
        if (!sub) {
            continue;
        }

        if (sub->future && !ecs_vec_count(&sub->queue)) {
            ecs_value_t result = { .type = type, .ptr = ECS_CONST_CAST(void*, value) };
            ecs_script_future_resolve(sub->future, &result);
            ecs_script_future_release(sub->future);
            sub->future = NULL;
            continue;
        }

        if (ecs_vec_count(&sub->queue) >= FLECS_SCRIPT_EVENT_QUEUE_MAX) {
            ecs_err("event '%s' dropped: queue of async block is full (%d events)",
                ecs_get_name(world, event), FLECS_SCRIPT_EVENT_QUEUE_MAX);
            continue;
        }

        void *copy = ecs_ptr_new_w_type_info(world, ti);
        ecs_ptr_copy_w_type_info(world, ti, copy, value);
        ecs_vec_append_t(NULL, &sub->queue, flecs_script_event_value_t)[0] =
            (flecs_script_event_value_t){ .ptr = copy, .pass = rt->event_pass };
        rt->event_queued ++;
    }
}

static void flecs_script_event_observer(
    ecs_iter_t *it)
{
    ecs_world_t *world = ECS_CONST_CAST(ecs_world_t*, ecs_get_world(it->world));
    const EcsScriptFunction *f = ecs_get(world, it->event, EcsScriptFunction);
    if (!f || !f->is_event) {
        return;
    }

    ecs_entity_t type = f->return_type;
    if (type != it->event) {
        bool value = true;
        flecs_script_event_deliver(world, it->event, type, &value);
        return;
    }

    if (it->param) {
        flecs_script_event_deliver(world, it->event, type, it->param);
        return;
    }

    const ecs_type_info_t *ti = ecs_get_type_info(world, type);
    ecs_assert(ti != NULL, ECS_INTERNAL_ERROR, NULL);
    void *value = ecs_ptr_new_w_type_info(world, ti);
    flecs_script_event_deliver(world, it->event, type, value);
    ecs_ptr_free_w_type_info(world, ti, value);
}

int flecs_script_event_register(
    ecs_world_t *world,
    ecs_entity_t event)
{
    const char *name = ecs_get_name(world, event);
    if (!name) {
        return -1;
    }

    const EcsScriptFunction *prev = ecs_get(world, event, EcsScriptFunction);
    bool registered = prev && prev->is_event;

    ecs_entity_t return_type = event;
    if (!ecs_get_type_info(world, event)) {
        if (!ecs_struct_init(world, &(ecs_struct_desc_t){ .entity = event })) {
            return -1;
        }
    }

    const EcsStruct *st = ecs_get(world, event, EcsStruct);
    if (!ecs_get_type_info(world, event) ||
        (st && !ecs_vec_count(&st->members)))
    {
        return_type = ecs_id(ecs_bool_t);
    }

    ecs_entity_t prev_scope = ecs_set_scope(world, 0);
    ecs_entity_t function = ecs_async_function(world, {
        .name = name,
        .parent = ecs_get_parent(world, event),
        .return_type = return_type,
        .callback = flecs_script_event_wait,
        .cancel = flecs_script_event_cancel
    });
    ecs_set_scope(world, prev_scope);
    if (function != event) {
        return -1;
    }

    ecs_get_mut(world, event, EcsScriptFunction)->is_event = true;

    if (!registered) {
        ecs_entity_t observer = ecs_observer(world, {
            .events = { event },
            .query.terms = {{ .id = EcsAny, .src.id = event }},
            .callback = flecs_script_event_observer
        });
        if (!observer) {
            return -1;
        }

        ecs_add_pair(world, observer, EcsChildOf, event);
    }

    return 0;
}

void flecs_script_event_emit(
    ecs_world_t *world,
    ecs_entity_t event,
    ecs_entity_t type,
    const void *value)
{
    ecs_emit(world, &(ecs_event_desc_t){
        .event = event,
        .entity = event,
        .const_param = type == event ? value : NULL
    });
}

static void flecs_script_events_flush(
    ecs_script_runtime_t *rt)
{
    ecs_script_task_t **tasks = ecs_vec_first(&rt->async_tasks);
    int32_t t, task_count = ecs_vec_count(&rt->async_tasks);
    for (t = 0; t < task_count; t ++) {
        if (!tasks[t]) {
            continue;
        }

        flecs_script_event_sub_t *subs = ecs_vec_first(&tasks[t]->event_subs);
        int32_t s, sub_count = ecs_vec_count(&tasks[t]->event_subs);
        for (s = 0; s < sub_count; s ++) {
            flecs_script_event_sub_t *sub = &subs[s];
            if (sub->future && ecs_vec_count(&sub->queue)) {
                ecs_script_future_t *future = sub->future;
                sub->future = NULL;
                flecs_script_event_sub_resolve_next(rt, sub, future);
            }
        }
    }
}

static ecs_script_task_t* flecs_script_sched_next(
    ecs_script_runtime_t *rt,
    int32_t *cursor)
{
    ecs_script_task_t *result = NULL;
    flecs_script_sched_lock(rt);

    uint64_t *words = ecs_vec_first_t(&rt->async_ready, uint64_t);
    int32_t w, word_count = ecs_vec_count(&rt->async_ready);
    uint64_t mask = ~0ull << (*cursor & 63);
    for (w = *cursor >> 6; w < word_count; w ++, mask = ~0ull) {
        uint64_t bits = words[w] & mask;
        if (bits) {
            int32_t index = (w << 6) + flecs_ctz64(bits);
            words[w] &= ~(1ull << (index & 63));
            *cursor = index + 1;
            result = ecs_vec_get_t(
                &rt->async_tasks, ecs_script_task_t*, index)[0];
            ecs_assert(result != NULL, ECS_INTERNAL_ERROR, NULL);
            break;
        }
    }

    flecs_script_sched_unlock(rt);
    return result;
}

static void flecs_script_task_sched_wait(
    ecs_script_task_t *task)
{
    ecs_script_future_t *future = task->async->future;
    if (future) {
        flecs_script_future_lock(future);
        bool pending = future->status == EcsScriptFuturePending;
        if (pending) {
            future->waiter = task;
        }

        flecs_script_future_unlock(future);
        if (pending) {
            return;
        }
    }

    flecs_script_task_wake(task);
}

static void flecs_script_sched_compact(
    ecs_script_runtime_t *rt)
{
    flecs_script_sched_lock(rt);
    ecs_script_task_t **tasks = ecs_vec_first(&rt->async_tasks);
    int32_t i, count = ecs_vec_count(&rt->async_tasks), dst = 0;
    for (i = 0; i < count; i ++) {
        ecs_script_task_t *task = tasks[i];
        if (!task) {
            continue;
        }

        if (dst != i) {
            bool ready = flecs_script_sched_is_ready(rt, i);
            flecs_script_sched_set_ready(rt, i, false);
            flecs_script_sched_set_ready(rt, dst, ready);
            task->sched_index = dst;
            tasks[dst] = task;
        }

        dst ++;
    }

    ecs_vec_set_count_t(NULL, &rt->async_tasks, ecs_script_task_t*, dst);
    rt->async_holes = false;
    flecs_script_sched_unlock(rt);
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
    rt->event_pass ++;
    if (rt->event_queued) {
        flecs_script_events_flush(rt);
    }

    int32_t cursor = 0, resumed = 0;
    ecs_script_task_t *task;
    while ((task = flecs_script_sched_next(rt, &cursor))) {
        if (task->status != EcsScriptTaskPending) {
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
        } else {
            flecs_script_task_sched_wait(task);
        }

        ecs_script_free(&impl->pub);
    }

    if (rt->async_holes) {
        flecs_script_sched_compact(rt);
    }

    rt->async_progressing = false;

    if (is_readonly) {
        flecs_resume_readonly(real_world, &srs);
    }

    return resumed;
error:
    return 0;
}

#ifdef FLECS_PIPELINE
typedef struct flecs_script_time_waiter_t {
    ecs_script_future_t *future;
    double wake_time;
    uint64_t wake_frame;
} flecs_script_time_waiter_t;

typedef struct EcsScriptTimeState {
    ecs_vec_t waiters; /* vec<flecs_script_time_waiter_t> */
    double time;
    uint64_t frame;
} EcsScriptTimeState;

static ECS_COMPONENT_DECLARE(EcsScriptTimeState);

static ECS_CTOR(EcsScriptTimeState, state, {
    ecs_os_zeromem(state);
    ecs_vec_init_t(NULL, &state->waiters, flecs_script_time_waiter_t, 0);
})

static ECS_MOVE(EcsScriptTimeState, dst, src, {
    ecs_vec_fini_t(NULL, &dst->waiters, flecs_script_time_waiter_t);
    *dst = *src;
    ecs_os_zeromem(src);
})

static ECS_DTOR(EcsScriptTimeState, state, {
    flecs_script_time_waiter_t *waiters = ecs_vec_first(&state->waiters);
    for (int32_t w = 0; w < ecs_vec_count(&state->waiters); w ++) {
        ecs_script_future_release(waiters[w].future);
    }

    ecs_vec_fini_t(NULL, &state->waiters, flecs_script_time_waiter_t);
})

static EcsScriptTimeState* flecs_script_time_state(
    ecs_world_t *world)
{
    if (!ecs_id(EcsScriptTimeState) || ecs_is_fini(world)) {
        return NULL;
    }

    return ecs_singleton_get_mut(world, EcsScriptTimeState);
}

static void flecs_script_time_wait(
    const ecs_function_ctx_t *ctx,
    int32_t argc,
    const ecs_value_t *argv,
    ecs_script_future_t *future)
{
    EcsScriptTimeState *state = flecs_script_time_state(ctx->world);
    if (!state) {
        ecs_script_future_reject(future, "world is shutting down");
        ecs_script_future_release(future);
        return;
    }

    double seconds = argc ? *(double*)argv[0].ptr : 0;
    ecs_vec_append_t(NULL, &state->waiters, flecs_script_time_waiter_t)[0] =
        (flecs_script_time_waiter_t){
            .future = future,
            .wake_time = state->time + (seconds > 0 ? seconds : 0),
            .wake_frame = state->frame + 1
        };
}

static void flecs_script_time_cancel(
    const ecs_function_ctx_t *ctx,
    ecs_script_future_t *future)
{
    EcsScriptTimeState *state = flecs_script_time_state(ctx->world);
    if (!state) {
        return;
    }

    flecs_script_time_waiter_t *waiters = ecs_vec_first(&state->waiters);
    for (int32_t i = 0; i < ecs_vec_count(&state->waiters); i ++) {
        if (waiters[i].future == future) {
            ecs_script_future_release(future);
            ecs_vec_remove_ordered_t(
                &state->waiters, flecs_script_time_waiter_t, i);
            return;
        }
    }
}

static void flecs_script_time_update(
    ecs_world_t *world,
    ecs_ftime_t delta_time)
{
    EcsScriptTimeState *state = flecs_script_time_state(world);
    if (!state) {
        return;
    }

    state->time += (double)delta_time;
    state->frame ++;

    for (int32_t i = 0; i < ecs_vec_count(&state->waiters); ) {
        flecs_script_time_waiter_t *waiter = ecs_vec_get_t(
            &state->waiters, flecs_script_time_waiter_t, i);
        if (state->frame < waiter->wake_frame ||
            state->time < waiter->wake_time)
        {
            i ++;
            continue;
        }

        ecs_script_future_t *future = waiter->future;
        ecs_vec_remove_ordered_t(
            &state->waiters, flecs_script_time_waiter_t, i);

        ecs_value_t result = ecs_value(ecs_bool_t, {true});
        ecs_script_future_resolve(future, &result);
        ecs_script_future_release(future);
    }
}

static void flecs_script_progress_tasks_system(
    ecs_iter_t *it)
{
    ecs_world_t *world = ECS_CONST_CAST(ecs_world_t*, ecs_get_world(it->world));
    flecs_script_time_update(world, it->delta_time);

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

    ECS_COMPONENT_DEFINE(world, EcsScriptTimeState);
    ecs_set_hooks(world, EcsScriptTimeState, {
        .ctor = ecs_ctor(EcsScriptTimeState),
        .move = ecs_move(EcsScriptTimeState),
        .dtor = ecs_dtor(EcsScriptTimeState),
        .flags = ECS_TYPE_HOOK_COPY_ILLEGAL
    });
    ecs_singleton_add(world, EcsScriptTimeState);

    ecs_async_function(world, {
        .name = "next_frame",
        .return_type = ecs_id(ecs_bool_t),
        .callback = flecs_script_time_wait,
        .cancel = flecs_script_time_cancel
    });

    ecs_async_function(world, {
        .name = "delay",
        .return_type = ecs_id(ecs_bool_t),
        .params = {{ .name = "seconds", .type = ecs_id(ecs_f64_t) }},
        .callback = flecs_script_time_wait,
        .cancel = flecs_script_time_cancel
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
