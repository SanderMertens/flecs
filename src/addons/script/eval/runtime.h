/**
 * @file addons/script/eval/runtime.h
 * @brief Script runtime.
 */

#ifndef FLECS_SCRIPT_RUNTIME_H
#define FLECS_SCRIPT_RUNTIME_H

typedef struct ecs_script_with_value_t {
    ecs_value_t value;
    const ecs_type_info_t *ti;
} ecs_script_with_value_t;

struct ecs_script_runtime_t {
    ecs_script_runtime_t *parent;
    ecs_allocator_t *allocator;
    ecs_expr_stack_t *expr_stack;
    ecs_stack_t stack;
    ecs_vec_t using;
    ecs_vec_t with;
    ecs_vec_t annot;
    ecs_vec_t pending_resolves;
    ecs_vec_t ir_vms;
    ecs_vec_t call_runtimes;

    /* Template instances with changed props whose re-evaluation is deferred
     * until the command queue is flushed. One marker event is enqueued per
     * flush instead of one event per instance. */
    ecs_vec_t template_pending;
    bool template_pending_marker;
    bool template_pending_active;

    /* Tasks created by async blocks. Only used by the world runtime, and
     * advanced by ecs_script_tasks_progress(). */
    ecs_vec_t async_tasks;
    bool async_progressing;

    /* Tag added to entities created by the currently evaluating managed
     * script. Carried on the world runtime so evaluation triggered from hooks
     * (such as template instantiation) inherits it. */
    ecs_id_t current_tag;

    char *error_name;
    int32_t include_depth;

    /* Nesting level of include statements. Guards against scripts that
     * (indirectly) include themselves. */
    int32_t include_nesting;

    /* Nesting level of template instantiations. Guards against templates that
     * (indirectly) instantiate themselves. */
    int32_t template_depth;

    bool resolving;
    bool error;
};

ecs_script_runtime_t* flecs_script_runtime_get(
    ecs_world_t *world);

ecs_script_runtime_t* flecs_script_runtime_new_shared(
    ecs_script_runtime_t *parent);

ecs_expr_stack_t* flecs_script_runtime_expr_stack(
    ecs_script_runtime_t *r);

ecs_script_runtime_t* flecs_script_runtime_acquire_call(
    ecs_script_runtime_t *r);

void flecs_script_runtime_release_call(
    ecs_script_runtime_t *r,
    ecs_script_runtime_t *call);

void flecs_script_runtime_error_reset(
    ecs_script_runtime_t *r);

void ecs_script_runtime_clear(
    ecs_script_runtime_t *r);

static inline FLECS_ALWAYS_INLINE
int flecs_function_calldata_invoke(
    ecs_world_t *world,
    const ecs_function_calldata_t *calldata,
    int32_t argc,
    const ecs_value_t *argv,
    ecs_value_t *result)
{
    ecs_function_ctx_t ctx = {
        .world = world,
        .function = calldata->function,
        .ctx = calldata->ctx
    };

    if (calldata->vector_elem_count) {
        calldata->is.vector_callback(&ctx, argc, argv, result,
            calldata->vector_elem_count);
    } else {
        calldata->is.callback(&ctx, argc, argv, result);
    }

    ecs_script_runtime_t *r = flecs_script_runtime_get(world);
    if (r->error) {
        r->error = false;
        return -1;
    }

    return 0;
}

static inline FLECS_ALWAYS_INLINE
int flecs_expr_function_call(
    const ecs_script_t *script,
    const ecs_expr_function_t *node,
    int32_t argc,
    const ecs_value_t *argv,
    ecs_value_t *out)
{
    const char *kind = node->node.kind == EcsExprMethod
        ? "method" : "function";

#ifdef FLECS_SCRIPT_ASYNC
    if (node->calldata.async_callback) {
        flecs_expr_visit_error(script, node,
            "async %s '%s' requires await", kind, node->function_name);
        return -1;
    }
#endif

    if (flecs_function_calldata_invoke(
        script->world, &node->calldata, argc, argv, out))
    {
        flecs_expr_visit_error(script, node,
            "error in script %s '%s'", kind, node->function_name);
        return -1;
    }

    return 0;
}

#endif
