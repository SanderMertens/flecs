/**
 * @file addons/script/expr/visit_free.c
 * @brief Visitor to free expression AST.
 */

#include "flecs.h"

#ifdef FLECS_SCRIPT
#include "../script.h"

static int flecs_expr_free_child(
    ecs_expr_node_t **node,
    void *ctx)
{
    flecs_expr_visit_free(ctx, *node);
    return 0;
}

void flecs_expr_visit_free(
    ecs_script_t *script,
    ecs_expr_node_t *node)
{
    if (!node) {
        return;
    }

    flecs_expr_visit_children(node, flecs_expr_free_child, script);
    ecs_allocator_t *a = &flecs_script_impl(script)->allocator;

    switch(node->kind) {
    case EcsExprValue: {
        ecs_expr_value_node_t *n = (ecs_expr_value_node_t*)node;
        if (n->ptr != &n->storage) {
            ecs_ptr_free_w_type_info(script->world, node->type_info, n->ptr);
            flecs_type_info_release(node->type_info);
        }
        flecs_free_t(a, ecs_expr_value_node_t, node);
        break;
    }
    case EcsExprInterpolatedString: {
        ecs_expr_interpolated_string_t *n =
            (ecs_expr_interpolated_string_t*)node;
        ecs_vec_fini_t(a, &n->fragments, char*);
        ecs_vec_fini_t(a, &n->expressions, ecs_expr_node_t*);
        ecs_vec_fini_t(a, &n->formats, ecs_expr_format_t);
        flecs_free_n(a, char, n->buffer_size, n->buffer);
        flecs_free_t(a, ecs_expr_interpolated_string_t, node);
        break;
    }
    case EcsExprInitializer:
    case EcsExprEmptyInitializer:
        ecs_vec_fini_t(a, &((ecs_expr_initializer_t*)node)->elements,
            ecs_expr_initializer_element_t);
        flecs_free_t(a, ecs_expr_initializer_t, node);
        break;
    case EcsExprUnary:
        flecs_free_t(a, ecs_expr_unary_t, node);
        break;
    case EcsExprBinary:
        flecs_free_t(a, ecs_expr_binary_t, node);
        break;
    case EcsExprIdentifier:
        flecs_free_t(a, ecs_expr_identifier_t, node);
        break;
    case EcsExprVariable:
    case EcsExprGlobalVariable: {
        ecs_expr_variable_t *var = (ecs_expr_variable_t*)node;
        if (var->owns_name) {
            flecs_strfree(a, ECS_CONST_CAST(char*, var->name));
        }
        flecs_free_t(a, ecs_expr_variable_t, node);
        break;
    }
    case EcsExprFunction:
    case EcsExprMethod:
        flecs_free_t(a, ecs_expr_function_t, node);
        break;
    case EcsExprMember:
        flecs_free_t(a, ecs_expr_member_t, node);
        break;
    case EcsExprSwizzle:
        flecs_free_t(a, ecs_expr_swizzle_t, node);
        break;
    case EcsExprElement:
    case EcsExprComponent:
        flecs_free_t(a, ecs_expr_element_t, node);
        break;
    case EcsExprHas:
        flecs_free_t(a, ecs_expr_has_t, node);
        break;
    case EcsExprMatch:
        ecs_vec_fini_t(a, &((ecs_expr_match_t*)node)->elements,
            ecs_expr_match_element_t);
        flecs_free_t(a, ecs_expr_match_t, node);
        break;
    case EcsExprRange:
        flecs_free_t(a, ecs_expr_range_t, node);
        break;
    case EcsExprNew: {
        ecs_script_entity_t *entity = ((ecs_expr_new_t*)node)->entity;
        if (entity) {
            flecs_script_visit_free_node(script, (ecs_script_node_t*)entity);
        }
        flecs_free_t(a, ecs_expr_new_t, node);
        break;
    }
    case EcsExprScript: {
        ecs_script_t *nested = ((ecs_expr_script_t*)node)->script;
        if (nested) {
            ecs_script_free(nested);
        }
        flecs_free_t(a, ecs_expr_script_t, node);
        break;
    }
    case EcsExprCast:
    case EcsExprCastNumber:
        flecs_free_t(a, ecs_expr_cast_t, node);
        break;
    }
}

#endif
