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
    ecs_allocator_t allocator;
    ecs_expr_stack_t expr_stack;
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

    /* Tag added to entities created by the currently evaluating managed
     * script. Carried on the world runtime so evaluation triggered from hooks
     * (such as template instantiation) inherits it. */
    ecs_id_t current_tag;

    char *error_name;
    char *unresolved_errors;
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

ecs_script_runtime_t* flecs_script_runtime_acquire_call(
    ecs_script_runtime_t *r);

void flecs_script_runtime_release_call(
    ecs_script_runtime_t *r,
    ecs_script_runtime_t *call);

void flecs_script_runtime_error_reset(
    ecs_script_runtime_t *r);

void ecs_script_runtime_clear(
    ecs_script_runtime_t *r);

#endif
