/**
 * @file addons/script/exor_visit.h
 * @brief Script AST visitor utilities.
 */

#ifndef FLECS_EXPR_SCRIPT_VISIT_H
#define FLECS_EXPR_SCRIPT_VISIT_H

#define flecs_expr_visit_error(script, node, ...) \
    ecs_parser_error( \
        script->name, script->code, ((ecs_expr_node_t*)node)->pos - script->code, \
        __VA_ARGS__);

int flecs_script_expr_visit_type(
    ecs_script_t *script,
    ecs_expr_node_t *node);

int flecs_script_expr_visit_fold(
    ecs_script_t *script,
    ecs_expr_node_t **node);

#endif
