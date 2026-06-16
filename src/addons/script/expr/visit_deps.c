/**
 * @file addons/script/expr/visit_deps.c
 * @brief Script expression dependency visitor.
 */

#include "flecs.h"

#ifdef FLECS_SCRIPT
#include "../../meta/meta.h"
#include "../script.h"

static
uint64_t flecs_expr_interpolated_string_visit_deps(
    const ecs_script_t *script,
    ecs_expr_interpolated_string_t *node,
    const ecs_script_vars_t *vars)
{
    uint64_t result = 0;
    int32_t i, count = ecs_vec_count(&node->expressions);
    ecs_expr_node_t **expressions = ecs_vec_first(&node->expressions);
    for (i = 0; i < count; i ++) {
        result |= flecs_expr_visit_deps(script, expressions[i], vars);
    }
    return result;
}

static
uint64_t flecs_expr_initializer_visit_deps(
    const ecs_script_t *script,
    ecs_expr_initializer_t *node,
    const ecs_script_vars_t *vars)
{
    uint64_t result = 0;
    ecs_expr_initializer_element_t *elems = ecs_vec_first(&node->elements);
    int32_t i, count = ecs_vec_count(&node->elements);
    for (i = 0; i < count; i ++) {
        result |= flecs_expr_visit_deps(script, elems[i].value, vars);
    }
    return result;
}

static
uint64_t flecs_expr_variable_visit_deps(
    const ecs_script_t *script,
    ecs_expr_variable_t *node,
    const ecs_script_vars_t *vars)
{
    (void)script;

    const ecs_script_var_t *var = flecs_script_find_var(
        vars, node->name, NULL);
    if (!var) {
        return 0;
    }

    return var->changed_mask;
}

static
uint64_t flecs_expr_unary_visit_deps(
    const ecs_script_t *script,
    ecs_expr_unary_t *node,
    const ecs_script_vars_t *vars)
{
    return flecs_expr_visit_deps(script, node->expr, vars);
}

static
uint64_t flecs_expr_binary_visit_deps(
    const ecs_script_t *script,
    ecs_expr_binary_t *node,
    const ecs_script_vars_t *vars)
{
    return flecs_expr_visit_deps(script, node->left, vars) |
           flecs_expr_visit_deps(script, node->right, vars);
}

static
uint64_t flecs_expr_identifier_visit_deps(
    const ecs_script_t *script,
    ecs_expr_identifier_t *node,
    const ecs_script_vars_t *vars)
{
    return flecs_expr_visit_deps(script, node->expr, vars);
}

static
uint64_t flecs_expr_function_visit_deps(
    const ecs_script_t *script,
    ecs_expr_function_t *node,
    const ecs_script_vars_t *vars)
{
    return flecs_expr_visit_deps(script, node->left, vars) |
           flecs_expr_visit_deps(script, (ecs_expr_node_t*)node->args, vars);
}

static
uint64_t flecs_expr_member_visit_deps(
    const ecs_script_t *script,
    ecs_expr_member_t *node,
    const ecs_script_vars_t *vars)
{
    return flecs_expr_visit_deps(script, node->left, vars);
}

static
uint64_t flecs_expr_element_visit_deps(
    const ecs_script_t *script,
    ecs_expr_element_t *node,
    const ecs_script_vars_t *vars)
{
    return flecs_expr_visit_deps(script, node->left, vars) |
           flecs_expr_visit_deps(script, node->index, vars);
}

static
uint64_t flecs_expr_match_visit_deps(
    const ecs_script_t *script,
    ecs_expr_match_t *node,
    const ecs_script_vars_t *vars)
{
    uint64_t result = flecs_expr_visit_deps(script, node->expr, vars);

    int32_t i, count = ecs_vec_count(&node->elements);
    ecs_expr_match_element_t *elems = ecs_vec_first(&node->elements);
    for (i = 0; i < count; i ++) {
        result |= flecs_expr_visit_deps(script, elems[i].compare, vars);
        result |= flecs_expr_visit_deps(script, elems[i].expr, vars);
    }

    if (node->any.compare) {
        result |= flecs_expr_visit_deps(script, node->any.compare, vars);
    }
    if (node->any.expr) {
        result |= flecs_expr_visit_deps(script, node->any.expr, vars);
    }

    return result;
}

static
uint64_t flecs_expr_cast_visit_deps(
    const ecs_script_t *script,
    ecs_expr_cast_t *node,
    const ecs_script_vars_t *vars)
{
    return flecs_expr_visit_deps(script, node->expr, vars);
}

static
uint64_t flecs_expr_new_visit_deps(
    const ecs_script_t *script,
    ecs_expr_new_t *node,
    const ecs_script_vars_t *vars)
{
    if (!node->entity) {
        return 0;
    }
    return flecs_script_visit_deps(
        script, (ecs_script_node_t*)node->entity, vars);
}

uint64_t flecs_expr_visit_deps(
    const ecs_script_t *script,
    ecs_expr_node_t *node,
    const ecs_script_vars_t *vars)
{
    if (!node) {
        return 0;
    }

    switch(node->kind) {
    case EcsExprValue:
        return 0;
    case EcsExprInterpolatedString:
        return flecs_expr_interpolated_string_visit_deps(
            script, (ecs_expr_interpolated_string_t*)node, vars);
    case EcsExprInitializer:
    case EcsExprEmptyInitializer:
        return flecs_expr_initializer_visit_deps(
            script, (ecs_expr_initializer_t*)node, vars);
    case EcsExprUnary:
        return flecs_expr_unary_visit_deps(
            script, (ecs_expr_unary_t*)node, vars);
    case EcsExprBinary:
        return flecs_expr_binary_visit_deps(
            script, (ecs_expr_binary_t*)node, vars);
    case EcsExprIdentifier:
        return flecs_expr_identifier_visit_deps(
            script, (ecs_expr_identifier_t*)node, vars);
    case EcsExprVariable:
        return flecs_expr_variable_visit_deps(
            script, (ecs_expr_variable_t*)node, vars);
    case EcsExprGlobalVariable:
        return 0;
    case EcsExprFunction:
    case EcsExprMethod:
        return flecs_expr_function_visit_deps(
            script, (ecs_expr_function_t*)node, vars);
    case EcsExprMember:
        return flecs_expr_member_visit_deps(
            script, (ecs_expr_member_t*)node, vars);
    case EcsExprElement:
    case EcsExprComponent:
        return flecs_expr_element_visit_deps(
            script, (ecs_expr_element_t*)node, vars);
    case EcsExprMatch:
        return flecs_expr_match_visit_deps(
            script, (ecs_expr_match_t*)node, vars);
    case EcsExprCast:
    case EcsExprCastNumber:
        return flecs_expr_cast_visit_deps(
            script, (ecs_expr_cast_t*)node, vars);
    case EcsExprNew:
        return flecs_expr_new_visit_deps(
            script, (ecs_expr_new_t*)node, vars);
    }

    return 0;
}

#endif
