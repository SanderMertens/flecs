/**
 * @file addons/script/eval/eval.h
 * @brief Script evaluation.
 */

#ifndef FLECS_SCRIPT_EVAL_H
#define FLECS_SCRIPT_EVAL_H

#include "runtime.h"
#include "visit_eval.h"
#include "async.h"

int flecs_script_apply_annot(
    ecs_script_eval_visitor_t *v,
    ecs_script_entity_t *node,
    ecs_entity_t entity,
    ecs_script_annot_t *annot);

/* Type visitors (implement struct/enum/bitmask initializer syntax) */

int flecs_script_struct_visit(
    const ecs_script_visitor_ctx_t *ctx);

int flecs_script_enum_visit(
    const ecs_script_visitor_ctx_t *ctx);

int flecs_script_bitmask_visit(
    const ecs_script_visitor_ctx_t *ctx);

/* User functions (functions defined in scripts) */

typedef struct ecs_script_user_function_t {
    ecs_script_t *script;
    ecs_script_function_node_t *node;
    ecs_vec_t refs;
    ecs_vec_t using;
} ecs_script_user_function_t;

void flecs_script_user_function_callback(
    const ecs_function_ctx_t *ctx,
    int32_t argc,
    const ecs_value_t *argv,
    ecs_value_t *result);

void flecs_script_user_function_ctx_free(
    void *ctx);

int flecs_script_eval_function(
    ecs_script_eval_visitor_t *v,
    ecs_script_function_node_t *node);

#endif
