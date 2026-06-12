/**
 * @file addons/script/expr/visit.h
 * @brief Script expression AST visitor utilities.
 */

#ifndef FLECS_EXPR_SCRIPT_VISIT_H
#define FLECS_EXPR_SCRIPT_VISIT_H

void flecs_expr_visit_error_(
    const ecs_script_t *script,
    const void *node,
    const char *fmt,
    ...);

#define flecs_expr_visit_error(script, node, ...) \
    flecs_expr_visit_error_(script, node, __VA_ARGS__)

int flecs_expr_visit_type(
    ecs_script_t *script,
    ecs_expr_node_t *node,
    const ecs_expr_eval_desc_t *desc);

int flecs_expr_visit_fold(
    ecs_script_t *script,
    ecs_expr_node_t **node,
    const ecs_expr_eval_desc_t *desc);

int flecs_expr_visit_eval(
    const ecs_script_t *script,
    ecs_expr_node_t *node,
    const ecs_expr_eval_desc_t *desc,
    ecs_value_t *out);

void flecs_expr_visit_free(
    ecs_script_t *script,
    ecs_expr_node_t *node);

#endif
