#include "flecs.h"

#ifdef FLECS_SCRIPT
#include "script.h"

typedef enum flecs_script_future_state_t {
    FlecsScriptFuturePending,
    FlecsScriptFutureResolved,
    FlecsScriptFutureRejected,
    FlecsScriptFutureCancelled
} flecs_script_future_state_t;

struct ecs_script_future_t {
    ecs_world_t *world;
    ecs_entity_t type;
    ecs_value_t value;
    char *error;
    void *ctx;
    ecs_ctx_free_t ctx_free;
    ecs_async_function_cancel_t cancel;
    ecs_function_ctx_t function_ctx;
    ecs_os_mutex_t mutex;
    int32_t refs;
    flecs_script_future_state_t state;
};

typedef enum flecs_script_frame_kind_t {
    FlecsScriptFrameScope,
    FlecsScriptFrameEntity,
    FlecsScriptFrameIf,
    FlecsScriptFrameFor,
    FlecsScriptFrameWith,
    FlecsScriptFramePairScope
} flecs_script_frame_kind_t;

typedef struct flecs_script_frame_t {
    flecs_script_frame_kind_t kind;
    ecs_script_node_t *node;
    int32_t pc;
    union {
        struct {
            ecs_entity_t parent;
            ecs_entity_t default_component_eval;
            int32_t using_count;
        } scope;
        struct {
            flecs_script_entity_state_t visitor;
            ecs_entity_t eval;
            ecs_entity_t eval_kind;
            ecs_entity_t default_component_eval;
        } entity;
        struct {
            ecs_script_scope_t *scope;
        } if_;
        struct {
            ecs_script_var_t *var;
            int32_t current;
            int32_t to;
        } for_;
        flecs_script_with_state_t with;
        flecs_script_pair_scope_state_t pair_scope;
    } state;
} flecs_script_frame_t;

struct ecs_script_task_t {
    ecs_script_t *script;
    ecs_script_runtime_t *runtime;
    ecs_script_eval_visitor_t visitor;
    ecs_script_eval_desc_t eval_desc;
    ecs_script_vars_t *initial_vars;
    ecs_script_future_t *future;
    ecs_entity_t entity;
    void *ctx;
    ecs_ctx_free_t ctx_free;
    flecs_script_frame_t frames[ECS_SCRIPT_VISIT_MAX_DEPTH];
    int32_t frame_count;
    int32_t frame_depth;
    int32_t iterations;
    int32_t completed_iterations;
    bool has_owner_vars;
    ecs_script_task_loop_t loop;
    ecs_script_task_status_t status;
};

static
void flecs_script_future_lock(
    ecs_script_future_t *future)
{
    if (future->mutex) {
        ecs_os_mutex_lock(future->mutex);
    }
}

static
void flecs_script_future_unlock(
    ecs_script_future_t *future)
{
    if (future->mutex) {
        ecs_os_mutex_unlock(future->mutex);
    }
}

static
ecs_script_future_t* flecs_script_future_new(
    ecs_script_task_t *task,
    ecs_expr_function_t *call)
{
    ecs_script_future_t *future = ecs_os_calloc_t(ecs_script_future_t);
    future->world = task->script->world;
    future->type = call->node.type;
    future->cancel = call->calldata.async_cancel;
    future->function_ctx = (ecs_function_ctx_t){
        .world = task->script->world,
        .function = call->calldata.function,
        .entity = task->entity,
        .ctx = call->calldata.ctx
    };
    future->refs = 2;
    if (ecs_os_has_threading()) {
        future->mutex = ecs_os_mutex_new();
    }
    return future;
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
        ecs_value_fini(future->world, &future->value);
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

    future->value = ecs_value_new(future->world, future->type);
    ecs_ptr_copy(future->world, future->type,
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
    ecs_check(future != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(error != NULL, ECS_INVALID_PARAMETER, NULL);

    flecs_script_future_lock(future);
    if (future->state != FlecsScriptFuturePending) {
        flecs_script_future_unlock(future);
        goto error;
    }
    future->error = ecs_os_strdup(error);
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
    ecs_script_future_t *ptr = ECS_CONST_CAST(ecs_script_future_t*, future);
    flecs_script_future_lock(ptr);
    bool result = ptr->state == FlecsScriptFutureCancelled;
    flecs_script_future_unlock(ptr);
    return result;
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

static
int flecs_script_task_eval_args(
    ecs_script_task_t *task,
    ecs_expr_function_t *call,
    ecs_value_t *argv)
{
    ecs_expr_initializer_element_t *elems = ecs_vec_first(
        &call->args->elements);
    int32_t i, count = ecs_vec_count(&call->args->elements);
    for (i = 0; i < count; i ++) {
        if (flecs_script_prepare_expr(
            &task->visitor, &elems[i].value, 0))
        {
            goto error;
        }
        argv[i] = ecs_value_new(
            task->script->world, elems[i].value->type);
        if (flecs_script_eval_expr(
            &task->visitor, &elems[i].value, &argv[i]))
        {
            goto error;
        }
    }
    return 0;
error:
    for (int32_t j = 0; j <= i; j ++) {
        ecs_value_fini(task->script->world, &argv[j]);
    }
    return -1;
}

static
int flecs_script_task_start_await(
    ecs_script_task_t *task,
    ecs_expr_node_t **expr_ptr)
{
    if (flecs_script_prepare_expr(&task->visitor, expr_ptr, 0)) {
        return -1;
    }

    ecs_expr_node_t *expr = *expr_ptr;
    if (expr->kind != EcsExprFunction && expr->kind != EcsExprMethod) {
        flecs_script_eval_error(&task->visitor,
            task->visitor.base.nodes[task->visitor.base.depth - 1],
            "await expression must be an async function call");
        return -1;
    }

    ecs_expr_function_t *call = (ecs_expr_function_t*)expr;
    if (!call->calldata.async_callback) {
        flecs_script_eval_error(&task->visitor,
            task->visitor.base.nodes[task->visitor.base.depth - 1],
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
            argv[0] = ecs_value_new(
                task->script->world, call->left->type);
            if (flecs_script_eval_expr(
                &task->visitor, &call->left, &argv[0]))
            {
                ecs_value_fini(task->script->world, &argv[0]);
                ecs_os_free(argv);
                return -1;
            }
        }
        if (argc && flecs_script_task_eval_args(
            task, call, &argv[arg_offset]))
        {
            if (is_method) {
                ecs_value_fini(task->script->world, &argv[0]);
            }
            ecs_os_free(argv);
            return -1;
        }
    }

    ecs_script_future_t *future = flecs_script_future_new(task, call);
    task->future = future;
    call->calldata.async_callback(
        &future->function_ctx, argc, argv, future);

    for (int32_t i = 0; i < value_count; i ++) {
        ecs_value_fini(task->script->world, &argv[i]);
    }
    ecs_os_free(argv);
    return 0;
}

static
int flecs_script_task_assign_const(
    ecs_script_task_t *task,
    ecs_script_var_node_t *node,
    const ecs_value_t *value,
    bool export)
{
    if (export) {
        if (ecs_script_vars_lookup(task->visitor.vars, node->name)) {
            flecs_script_eval_error(&task->visitor, node,
                "exported variable '%s' shadows a local variable",
                node->name);
            return -1;
        }

        ecs_entity_t const_var = ecs_const_var(task->script->world, {
            .parent = task->visitor.parent,
            .name = node->name,
            .type = value->type,
            .value = value->ptr
        });
        if (!const_var) {
            flecs_script_eval_error(&task->visitor, node,
                "failed to create exported const variable '%s'",
                node->name);
            return -1;
        }
        return 0;
    }

    ecs_script_var_t *var = ecs_script_vars_declare(
        task->visitor.vars, node->name);
    if (!var) {
        flecs_script_eval_error(&task->visitor, node,
            "variable '%s' redeclared", node->name);
        return -1;
    }

    const ecs_type_info_t *ti = ecs_get_type_info(
        task->script->world, value->type);
    var->value.type = value->type;
    var->value.ptr = flecs_stack_alloc(&task->runtime->stack,
        ti->size, ti->alignment);
    var->type_info = ti;
    flecs_type_info_ctor(var->value.ptr, 1, ti);
    ecs_ptr_copy(task->script->world, value->type,
        var->value.ptr, value->ptr);
    return 0;
}

static
ecs_script_task_status_t flecs_script_task_await(
    ecs_script_task_t *task,
    ecs_expr_node_t **expr,
    ecs_script_var_node_t *var,
    bool export,
    ecs_script_node_t *node)
{
    if (!task->future) {
        if (flecs_script_task_start_await(task, expr)) {
            return EcsScriptTaskError;
        }
    }

    ecs_script_future_t *future = task->future;
    flecs_script_future_lock(future);
    flecs_script_future_state_t state = future->state;
    flecs_script_future_unlock(future);

    if (state == FlecsScriptFuturePending) {
        return EcsScriptTaskPending;
    }

    ecs_script_task_status_t status = EcsScriptTaskDone;
    if (state == FlecsScriptFutureRejected) {
        flecs_script_eval_error(&task->visitor, node,
            "%s", future->error);
        status = EcsScriptTaskError;
    } else if (state == FlecsScriptFutureCancelled) {
        flecs_script_eval_error(&task->visitor, node,
            "awaited operation was cancelled");
        status = EcsScriptTaskError;
    } else if (var && flecs_script_task_assign_const(
        task, var, &future->value, export))
    {
        status = EcsScriptTaskError;
    }

    task->future = NULL;
    ecs_script_future_release(future);
    return status;
}

static
flecs_script_frame_t* flecs_script_task_frame_enter(
    ecs_script_task_t *task,
    flecs_script_frame_kind_t kind,
    ecs_script_node_t *node,
    bool *is_new)
{
    int32_t depth = task->frame_depth ++;
    ecs_assert(depth < ECS_SCRIPT_VISIT_MAX_DEPTH,
        ECS_OUT_OF_RANGE, NULL);
    flecs_script_frame_t *frame = &task->frames[depth];
    if (depth == task->frame_count) {
        task->frame_count ++;
        ecs_os_zeromem(frame);
        frame->kind = kind;
        frame->node = node;
        *is_new = true;
    } else {
        ecs_assert(frame->kind == kind, ECS_INTERNAL_ERROR, NULL);
        ecs_assert(frame->node == node, ECS_INTERNAL_ERROR, NULL);
        *is_new = false;
    }
    return frame;
}

static
void flecs_script_task_frame_leave(
    ecs_script_task_t *task,
    bool complete)
{
    task->frame_depth --;
    if (complete) {
        ecs_assert(task->frame_count == task->frame_depth + 1,
            ECS_INTERNAL_ERROR, NULL);
        task->frame_count --;
    }
}

static
int flecs_script_task_node_push(
    ecs_script_task_t *task,
    ecs_script_node_t *node)
{
    ecs_script_visit_t *v = &task->visitor.base;
    if (v->depth >= ECS_SCRIPT_VISIT_MAX_DEPTH) {
        return -1;
    }
    v->nodes[v->depth ++] = node;
    return 0;
}

static
void flecs_script_task_node_pop(
    ecs_script_task_t *task)
{
    task->visitor.base.depth --;
}

static
ecs_script_task_status_t flecs_script_task_run_scope(
    ecs_script_task_t *task,
    ecs_script_scope_t *scope);

static
ecs_script_task_status_t flecs_script_task_run_if(
    ecs_script_task_t *task,
    ecs_script_if_t *node)
{
    bool is_new;
    flecs_script_frame_t *frame = flecs_script_task_frame_enter(
        task, FlecsScriptFrameIf, &node->node, &is_new);
    if (is_new) {
        ecs_value_t value = {0};
        if (flecs_script_eval_expr(
            &task->visitor, &node->expr, &value))
        {
            flecs_script_task_frame_leave(task, true);
            return EcsScriptTaskError;
        }

        bool cond;
        if (value.type == ecs_id(ecs_bool_t)) {
            cond = *(bool*)value.ptr;
        } else {
            ecs_meta_cursor_t cur = ecs_meta_cursor(
                task->script->world, value.type, value.ptr);
            cond = ecs_meta_get_bool(&cur);
        }
        ecs_ptr_free(task->script->world, value.type, value.ptr);
        frame->state.if_.scope = cond ? node->if_true : node->if_false;
    }

    ecs_script_task_status_t status = flecs_script_task_run_scope(
        task, frame->state.if_.scope);
    flecs_script_task_frame_leave(
        task, status == EcsScriptTaskDone);
    return status;
}

static
ecs_script_task_status_t flecs_script_task_run_for(
    ecs_script_task_t *task,
    ecs_script_for_range_t *node)
{
    bool is_new;
    flecs_script_frame_t *frame = flecs_script_task_frame_enter(
        task, FlecsScriptFrameFor, &node->node, &is_new);
    ecs_script_eval_visitor_t *v = &task->visitor;
    if (is_new) {
        int32_t from, to;
        ecs_value_t from_value = {
            ecs_id(ecs_i32_t), &from
        };
        ecs_value_t to_value = {
            ecs_id(ecs_i32_t), &to
        };
        if (flecs_script_eval_expr(v, &node->from, &from_value) ||
            flecs_script_eval_expr(v, &node->to, &to_value))
        {
            flecs_script_task_frame_leave(task, true);
            return EcsScriptTaskError;
        }

        v->vars = flecs_script_vars_push(
            v->vars, &task->runtime->stack,
            &task->runtime->allocator);
        frame->state.for_.var = ecs_script_vars_declare(
            v->vars, node->loop_var);
        frame->state.for_.var->value.ptr = flecs_stack_calloc(
            &task->runtime->stack, 4, 4);
        frame->state.for_.var->value.type = ecs_id(ecs_i32_t);
        frame->state.for_.var->type_info = ecs_get_type_info(
            task->script->world, ecs_id(ecs_i32_t));
        frame->state.for_.current = from;
        frame->state.for_.to = to;
    }

    while (frame->state.for_.current < frame->state.for_.to) {
        *(int32_t*)frame->state.for_.var->value.ptr =
            frame->state.for_.current;
        ecs_script_task_status_t status =
            flecs_script_task_run_scope(task, node->scope);
        if (status != EcsScriptTaskDone) {
            flecs_script_task_frame_leave(task, false);
            return status;
        }
        frame->state.for_.current ++;
    }

    v->vars = ecs_script_vars_pop(v->vars);
    flecs_script_task_frame_leave(task, true);
    return EcsScriptTaskDone;
}

static
void flecs_script_task_entity_restore(
    flecs_script_frame_t *frame)
{
    ecs_script_entity_t *node = (ecs_script_entity_t*)frame->node;
    node->eval = frame->state.entity.eval;
    node->eval_kind = frame->state.entity.eval_kind;
    node->scope->default_component_eval =
        frame->state.entity.default_component_eval;
}

static
ecs_script_task_status_t flecs_script_task_run_entity(
    ecs_script_task_t *task,
    ecs_script_entity_t *node)
{
    bool is_new;
    flecs_script_frame_t *frame = flecs_script_task_frame_enter(
        task, FlecsScriptFrameEntity, &node->node, &is_new);
    if (is_new) {
        if (flecs_script_eval_entity_enter(
            &task->visitor, node, &frame->state.entity.visitor))
        {
            flecs_script_task_frame_leave(task, true);
            return EcsScriptTaskError;
        }
        frame->state.entity.eval = node->eval;
        frame->state.entity.eval_kind = node->eval_kind;
        frame->state.entity.default_component_eval =
            node->scope->default_component_eval;
    } else {
        flecs_script_task_entity_restore(frame);
    }

    ecs_script_task_status_t status = flecs_script_task_run_scope(
        task, node->scope);
    if (status == EcsScriptTaskDone) {
        flecs_script_eval_entity_leave(
            &task->visitor, node, &frame->state.entity.visitor);
    }
    flecs_script_task_frame_leave(
        task, status == EcsScriptTaskDone);
    return status;
}

static
ecs_script_task_status_t flecs_script_task_run_with(
    ecs_script_task_t *task,
    ecs_script_with_t *node)
{
    bool is_new;
    flecs_script_frame_t *frame = flecs_script_task_frame_enter(
        task, FlecsScriptFrameWith, &node->node, &is_new);
    if (is_new && flecs_script_eval_with_enter(
        &task->visitor, node, &frame->state.with))
    {
        flecs_script_task_frame_leave(task, true);
        return EcsScriptTaskError;
    }

    ecs_script_task_status_t status = flecs_script_task_run_scope(
        task, node->scope);
    if (status == EcsScriptTaskDone) {
        flecs_script_eval_with_leave(
            &task->visitor, &frame->state.with);
    }
    flecs_script_task_frame_leave(
        task, status == EcsScriptTaskDone);
    return status;
}

static
ecs_script_task_status_t flecs_script_task_run_pair_scope(
    ecs_script_task_t *task,
    ecs_script_pair_scope_t *node)
{
    bool is_new;
    flecs_script_frame_t *frame = flecs_script_task_frame_enter(
        task, FlecsScriptFramePairScope, &node->node, &is_new);
    if (is_new && flecs_script_eval_pair_scope_enter(
        &task->visitor, node, &frame->state.pair_scope))
    {
        flecs_script_task_frame_leave(task, true);
        return EcsScriptTaskError;
    }

    ecs_script_task_status_t status = flecs_script_task_run_scope(
        task, node->scope);
    if (status == EcsScriptTaskDone) {
        flecs_script_eval_pair_scope_leave(
            &task->visitor, &frame->state.pair_scope);
    }
    flecs_script_task_frame_leave(
        task, status == EcsScriptTaskDone);
    return status;
}

static
ecs_script_task_status_t flecs_script_task_run_node(
    ecs_script_task_t *task,
    ecs_script_node_t *node)
{
    if (flecs_script_task_node_push(task, node)) {
        return EcsScriptTaskError;
    }

    ecs_script_task_status_t status;
    if (node->kind == EcsAstAwait) {
        ecs_script_await_t *await = (ecs_script_await_t*)node;
        status = flecs_script_task_await(
            task, &await->expr, NULL, false, node);
    } else if ((node->kind == EcsAstConst ||
        node->kind == EcsAstExportConst) &&
        ((ecs_script_var_node_t*)node)->is_await)
    {
        ecs_script_var_node_t *var = (ecs_script_var_node_t*)node;
        status = flecs_script_task_await(
            task, &var->expr, var,
            node->kind == EcsAstExportConst, node);
    } else if (node->kind == EcsAstIf) {
        status = flecs_script_task_run_if(
            task, (ecs_script_if_t*)node);
    } else if (node->kind == EcsAstFor) {
        status = flecs_script_task_run_for(
            task, (ecs_script_for_range_t*)node);
    } else if (node->kind == EcsAstEntity) {
        status = flecs_script_task_run_entity(
            task, (ecs_script_entity_t*)node);
    } else if (node->kind == EcsAstWith) {
        status = flecs_script_task_run_with(
            task, (ecs_script_with_t*)node);
    } else if (node->kind == EcsAstPairScope) {
        status = flecs_script_task_run_pair_scope(
            task, (ecs_script_pair_scope_t*)node);
    } else {
        status = flecs_script_eval_node(
            (ecs_script_visit_t*)&task->visitor, node)
                ? EcsScriptTaskError
                : EcsScriptTaskDone;
    }

    flecs_script_task_node_pop(task);
    return status;
}

static
ecs_script_task_status_t flecs_script_task_run_scope(
    ecs_script_task_t *task,
    ecs_script_scope_t *scope)
{
    if (flecs_script_task_node_push(
        task, (ecs_script_node_t*)scope))
    {
        return EcsScriptTaskError;
    }

    bool is_new;
    flecs_script_frame_t *frame = flecs_script_task_frame_enter(
        task, FlecsScriptFrameScope,
        (ecs_script_node_t*)scope, &is_new);
    ecs_script_eval_visitor_t *v = &task->visitor;
    if (is_new) {
        frame->state.scope.parent = v->parent;
        frame->state.scope.using_count = ecs_vec_count(&v->r->using);

        for (int32_t i = v->base.depth - 2; i >= 0; i --) {
            if (v->base.nodes[i]->kind == EcsAstScope) {
                scope->parent = (ecs_script_scope_t*)v->base.nodes[i];
                break;
            }
        }

        ecs_script_node_t *scope_parent = ecs_script_parent_node(v);
        if (scope_parent &&
            (scope_parent->kind == EcsAstIf ||
                scope_parent->kind == EcsAstFor) &&
            scope->parent)
        {
            scope->default_component_eval =
                scope->parent->default_component_eval;
        }
        if (scope_parent && scope_parent->kind == EcsAstEntity &&
            !v->template)
        {
            v->parent = ((ecs_script_entity_t*)scope_parent)->eval;
        }

        v->vars = flecs_script_vars_push(
            v->vars, &task->runtime->stack,
            &task->runtime->allocator);

        if (v->entity) {
            ecs_entity_t src = v->entity->eval;
            int32_t component_count = ecs_vec_count(&scope->components);
            if (src != EcsVariable && component_count) {
                flecs_add_ids(v->world, src,
                    ecs_vec_first(&scope->components), component_count);
            }
        }
        frame->state.scope.default_component_eval =
            scope->default_component_eval;
    } else {
        scope->default_component_eval =
            frame->state.scope.default_component_eval;
    }

    ecs_script_node_t **nodes = ecs_vec_first(&scope->stmts);
    int32_t count = ecs_vec_count(&scope->stmts);
    while (frame->pc < count) {
        v->base.prev = frame->pc ? nodes[frame->pc - 1] : NULL;
        v->base.next = frame->pc + 1 < count
            ? nodes[frame->pc + 1] : NULL;
        ecs_script_task_status_t status =
            flecs_script_task_run_node(task, nodes[frame->pc]);
        if (status != EcsScriptTaskDone) {
            flecs_script_task_frame_leave(task, false);
            flecs_script_task_node_pop(task);
            return status;
        }
        frame->pc ++;
    }

    ecs_vec_set_count_t(&v->r->allocator, &v->r->using,
        ecs_entity_t, frame->state.scope.using_count);
    v->vars = ecs_script_vars_pop(v->vars);
    v->parent = frame->state.scope.parent;
    flecs_script_task_frame_leave(task, true);
    flecs_script_task_node_pop(task);
    return EcsScriptTaskDone;
}

static
ecs_script_task_status_t flecs_script_task_run(
    ecs_script_task_t *task)
{
    task->frame_depth = 0;
    task->visitor.base.depth = 0;
    return flecs_script_task_run_scope(
        task, flecs_script_impl(task->script)->root);
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

    ecs_script_task_t *result = ecs_os_calloc_t(
        ecs_script_task_t);
    result->script = ECS_CONST_CAST(ecs_script_t*, script);
    flecs_script_impl(result->script)->refcount ++;
    result->runtime = ecs_script_runtime_new();
    result->eval_desc.runtime = result->runtime;
    if (desc) {
        result->entity = desc->entity;
        result->ctx = desc->ctx;
        result->ctx_free = desc->ctx_free;
        result->loop = desc->loop;
        result->iterations = desc->iterations;
        result->eval_desc.vars = ECS_CONST_CAST(
            ecs_script_vars_t*, desc->vars);
    }
    flecs_script_eval_visit_init(flecs_script_impl(result->script),
        &result->visitor, &result->eval_desc);
    if (result->entity) {
        result->visitor.vars = flecs_script_vars_push(
            result->visitor.vars, &result->runtime->stack,
            &result->runtime->allocator);
        ecs_script_var_t *var = ecs_script_vars_declare(
            result->visitor.vars, "this");
        var->value.type = ecs_id(ecs_entity_t);
        var->value.ptr = flecs_stack_alloc(
            &result->runtime->stack,
            ECS_SIZEOF(ecs_entity_t), ECS_ALIGNOF(ecs_entity_t));
        *(ecs_entity_t*)var->value.ptr = result->entity;
        var->type_info = ecs_get_type_info(
            result->script->world, ecs_id(ecs_entity_t));
        result->has_owner_vars = true;
    }
    result->initial_vars = result->visitor.vars;
    return result;
error:
    return NULL;
}

ecs_script_task_status_t ecs_script_task_resume(
    ecs_script_task_t *task,
    ecs_script_eval_result_t *result)
{
    ecs_check(task != NULL, ECS_INVALID_PARAMETER, NULL);
    if (task->status == EcsScriptTaskDone ||
        task->status == EcsScriptTaskError ||
        task->status == EcsScriptTaskCancelled)
    {
        return task->status;
    }

    if (result) {
        flecs_log_capture_push(true);
    }
    task->status = flecs_script_task_run(task);
    if (result) {
        result->error = flecs_log_capture_pop();
        flecs_log_get_captured_error_pos(&result->line, &result->column);
    }
    if (task->status == EcsScriptTaskDone) {
        bool repeat = task->loop == EcsScriptTaskLoopForever;
        if (task->loop == EcsScriptTaskLoopCount) {
            task->completed_iterations ++;
            repeat = task->completed_iterations < task->iterations;
        }
        if (repeat) {
            ecs_assert(task->frame_count == 0, ECS_INTERNAL_ERROR, NULL);
            ecs_assert(task->visitor.vars == task->initial_vars,
                ECS_INTERNAL_ERROR, NULL);
            task->status = EcsScriptTaskPending;
        }
    }
    return task->status;
error:
    return EcsScriptTaskError;
}

bool ecs_script_task_is_ready(
    const ecs_script_task_t *task)
{
    ecs_check(task != NULL, ECS_INVALID_PARAMETER, NULL);
    if (!task->future) {
        return true;
    }
    ecs_script_future_t *future = task->future;
    flecs_script_future_lock(future);
    bool ready = future->state != FlecsScriptFuturePending;
    flecs_script_future_unlock(future);
    return ready;
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
    ecs_script_future_t *future = task->future;
    if (!future) {
        return;
    }

    flecs_script_future_lock(future);
    bool cancel = future->state == FlecsScriptFuturePending;
    if (cancel) {
        future->state = FlecsScriptFutureCancelled;
    }
    flecs_script_future_unlock(future);
    if (cancel && future->cancel) {
        future->cancel(&future->function_ctx, future);
    }
    task->future = NULL;
    ecs_script_future_release(future);
error:
    return;
}

void ecs_script_task_free(
    ecs_script_task_t *task)
{
    if (!task) {
        return;
    }
    ecs_script_task_cancel(task);
    for (int32_t i = task->frame_count - 1; i >= 0; i --) {
        flecs_script_frame_t *frame = &task->frames[i];
        if (frame->kind == FlecsScriptFrameScope ||
            frame->kind == FlecsScriptFrameFor)
        {
            task->visitor.vars = ecs_script_vars_pop(
                task->visitor.vars);
        } else if (frame->kind == FlecsScriptFrameEntity) {
            flecs_script_task_entity_restore(frame);
            flecs_script_eval_entity_leave(&task->visitor,
                (ecs_script_entity_t*)frame->node,
                &frame->state.entity.visitor);
        } else if (frame->kind == FlecsScriptFrameWith) {
            flecs_script_eval_with_leave(
                &task->visitor, &frame->state.with);
        } else if (frame->kind == FlecsScriptFramePairScope) {
            flecs_script_eval_pair_scope_leave(
                &task->visitor, &frame->state.pair_scope);
        }
    }
    ecs_assert(task->visitor.vars == task->initial_vars,
        ECS_INTERNAL_ERROR, NULL);
    if (task->has_owner_vars) {
        task->visitor.vars = ecs_script_vars_pop(
            task->visitor.vars);
    }
    flecs_script_eval_visit_fini(
        &task->visitor, &task->eval_desc);
    ecs_script_runtime_free(task->runtime);
    if (task->ctx_free) {
        task->ctx_free(task->ctx);
    }
    ecs_script_free(task->script);
    ecs_os_free(task);
}

#endif
