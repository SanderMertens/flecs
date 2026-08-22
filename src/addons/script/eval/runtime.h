/**
 * @file addons/script/eval/runtime.h
 * @brief Script runtime.
 */

#ifndef FLECS_SCRIPT_RUNTIME_H
#define FLECS_SCRIPT_RUNTIME_H

struct ecs_script_runtime_t {
    ecs_allocator_t allocator;
    ecs_expr_stack_t expr_stack;
    ecs_stack_t stack;
    ecs_vec_t using;
    ecs_vec_t with;
    ecs_vec_t with_type_info;
    ecs_vec_t annot;
    ecs_vec_t pending_resolves;

    /* Tag added to entities created by the currently evaluating managed
     * script. Carried on the world runtime so evaluation triggered from hooks
     * (such as template instantiation) inherits it. */
    ecs_id_t current_tag;

    char *error_name;
    char *unresolved_errors;
    int32_t include_depth;
    bool resolving;
    bool error;
};

ecs_script_runtime_t* flecs_script_runtime_get(
    ecs_world_t *world);

void flecs_script_runtime_error_reset(
    ecs_script_runtime_t *r);

void ecs_script_runtime_clear(
    ecs_script_runtime_t *r);

#endif
