/**
 * @file addons/script/expr/visit_free.c
 * @brief Visitor to free expression AST.
 */

#include "flecs.h"

#ifdef FLECS_SCRIPT
#include "../script.h"

static
void flecs_expr_value_visit_free(
    ecs_script_t *script,
    ecs_expr_val_t *node)
{
    if (node->ptr != &node->storage) {
        ecs_value_free(script->world, node->node.type, node->ptr);
    }
}

static
void flecs_expr_initializer_visit_free(
    ecs_script_t *script,
    ecs_expr_initializer_t *node)
{
    ecs_expr_initializer_element_t *elems = ecs_vec_first(&node->elements);
    int32_t i, count = ecs_vec_count(&node->elements);
    for (i = 0; i < count; i ++) {
        ecs_expr_initializer_element_t *elem = &elems[i];
        flecs_script_expr_visit_free(script, elem->value);
    }

    ecs_vec_fini_t(&script->world->allocator, &node->elements, 
        ecs_expr_initializer_element_t);
}

static
void flecs_expr_unary_visit_free(
    ecs_script_t *script,
    ecs_expr_unary_t *node)
{
    flecs_script_expr_visit_free(script, node->expr);
}

static
void flecs_expr_binary_visit_free(
    ecs_script_t *script,
    ecs_expr_binary_t *node)
{
    flecs_script_expr_visit_free(script, node->left);
    flecs_script_expr_visit_free(script, node->right);
}

static
void flecs_expr_function_visit_free(
    ecs_script_t *script,
    ecs_expr_function_t *node)
{
    flecs_script_expr_visit_free(script, node->left);
}

static
void flecs_expr_member_visit_free(
    ecs_script_t *script,
    ecs_expr_member_t *node)
{
    flecs_script_expr_visit_free(script, node->left);
}

static
void flecs_expr_element_visit_free(
    ecs_script_t *script,
    ecs_expr_element_t *node)
{
    flecs_script_expr_visit_free(script, node->left);
    flecs_script_expr_visit_free(script, node->index);
}

static
void flecs_expr_cast_visit_free(
    ecs_script_t *script,
    ecs_expr_cast_t *node)
{
    flecs_script_expr_visit_free(script, node->expr);
}

void flecs_script_expr_visit_free(
    ecs_script_t *script,
    ecs_expr_node_t *node)
{
    if (!node) {
        return;
    }

    ecs_allocator_t *a = &script->world->allocator;

    switch(node->kind) {
    case EcsExprValue:
        flecs_expr_value_visit_free(
            script, (ecs_expr_val_t*)node);
        flecs_free_t(a, ecs_expr_val_t, node);
        break;
    case EcsExprInitializer:
        flecs_expr_initializer_visit_free(
            script, (ecs_expr_initializer_t*)node);
        flecs_free_t(a, ecs_expr_initializer_t, node);
        break;
    case EcsExprUnary:
        flecs_expr_unary_visit_free(
            script, (ecs_expr_unary_t*)node);
        flecs_free_t(a, ecs_expr_unary_t, node);
        break;
    case EcsExprBinary:
        flecs_expr_binary_visit_free(
            script, (ecs_expr_binary_t*)node);
        flecs_free_t(a, ecs_expr_binary_t, node);
        break;
    case EcsExprIdentifier:
        flecs_free_t(a, ecs_expr_identifier_t, node);
        break;
    case EcsExprVariable:
        flecs_free_t(a, ecs_expr_variable_t, node);
        break;
    case EcsExprFunction:
        flecs_expr_function_visit_free(
            script, (ecs_expr_function_t*)node);
        flecs_free_t(a, ecs_expr_function_t, node);
        break;
    case EcsExprMember:
        flecs_expr_member_visit_free(
            script, (ecs_expr_member_t*)node);
        flecs_free_t(a, ecs_expr_member_t, node);
        break;
    case EcsExprElement:
    case EcsExprComponent:
        flecs_expr_element_visit_free(
            script, (ecs_expr_element_t*)node);
        flecs_free_t(a, ecs_expr_element_t, node);
        break;
    case EcsExprCast:
        flecs_expr_cast_visit_free(
            script, (ecs_expr_cast_t*)node);
        flecs_free_t(a, ecs_expr_cast_t, node);
        break;
    }
}

#endif
