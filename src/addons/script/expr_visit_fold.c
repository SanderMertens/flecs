/**
 * @file addons/script/expr_fold.c
 * @brief Script expression constant folding.
 */

#include "flecs.h"

#ifdef FLECS_SCRIPT
#include "script.h"

#define ECS_VALUE_GET(value, T) (*(T*)((ecs_expr_val_t*)value)->ptr)

#define ECS_BINARY_OP_T(left, right, result, op, R, T)\
    ECS_VALUE_GET(result, R) = ECS_VALUE_GET(left, T) op ECS_VALUE_GET(right, T)

#define ECS_BINARY_INT_OP(left, right, result, op)\
    if (left->type == ecs_id(ecs_u64_t)) { \
        ECS_BINARY_OP_T(left, right, result, op, ecs_u64_t, ecs_u64_t);\
    } else if (left->type == ecs_id(ecs_i64_t)) { \
        ECS_BINARY_OP_T(left, right, result, op, ecs_i64_t, ecs_i64_t);\
    } else {\
        ecs_abort(ECS_INTERNAL_ERROR, "unexpected type in binary expression");\
    }

#define ECS_BINARY_OP(left, right, result, op)\
    if (left->type == ecs_id(ecs_u64_t)) { \
        ECS_BINARY_OP_T(left, right, result, op, ecs_u64_t, ecs_u64_t);\
    } else if (left->type == ecs_id(ecs_i64_t)) { \
        ECS_BINARY_OP_T(left, right, result, op, ecs_i64_t, ecs_i64_t);\
    } else if (left->type == ecs_id(ecs_f64_t)) { \
        ECS_BINARY_OP_T(left, right, result, op, ecs_f64_t, ecs_f64_t);\
    } else {\
        ecs_abort(ECS_INTERNAL_ERROR, "unexpected type in binary expression");\
    }

#define ECS_BINARY_COND_EQ_OP(left, right, result, op)\
    if (left->type == ecs_id(ecs_u64_t)) { \
        ECS_BINARY_OP_T(left, right, result, op, ecs_bool_t, ecs_u64_t);\
    } else if (left->type == ecs_id(ecs_i64_t)) { \
        ECS_BINARY_OP_T(left, right, result, op, ecs_bool_t, ecs_i64_t);\
    } else if (left->type == ecs_id(ecs_f64_t)) { \
        flecs_expr_visit_error(script, left, "unsupported operator for floating point");\
        return -1;\
    } else if (left->type == ecs_id(ecs_u8_t)) { \
        ECS_BINARY_OP_T(left, right, result, op, ecs_bool_t, ecs_u8_t);\
    } else if (left->type == ecs_id(ecs_char_t)) { \
        ECS_BINARY_OP_T(left, right, result, op, ecs_bool_t, ecs_char_t);\
    } else if (left->type == ecs_id(ecs_bool_t)) { \
        ECS_BINARY_OP_T(left, right, result, op, ecs_bool_t, ecs_bool_t);\
    } else {\
        ecs_abort(ECS_INTERNAL_ERROR, "unexpected type in binary expression");\
    }

#define ECS_BINARY_COND_OP(left, right, result, op)\
    if (left->type == ecs_id(ecs_u64_t)) { \
        ECS_BINARY_OP_T(left, right, result, op, ecs_bool_t, ecs_u64_t);\
    } else if (left->type == ecs_id(ecs_i64_t)) { \
        ECS_BINARY_OP_T(left, right, result, op, ecs_bool_t, ecs_i64_t);\
    } else if (left->type == ecs_id(ecs_f64_t)) { \
        ECS_BINARY_OP_T(left, right, result, op, ecs_bool_t, ecs_f64_t);\
    } else if (left->type == ecs_id(ecs_u8_t)) { \
        ECS_BINARY_OP_T(left, right, result, op, ecs_bool_t, ecs_u8_t);\
    } else if (left->type == ecs_id(ecs_char_t)) { \
        ECS_BINARY_OP_T(left, right, result, op, ecs_bool_t, ecs_char_t);\
    } else if (left->type == ecs_id(ecs_bool_t)) { \
        ECS_BINARY_OP_T(left, right, result, op, ecs_bool_t, ecs_bool_t);\
    } else {\
        ecs_abort(ECS_INTERNAL_ERROR, "unexpected type in binary expression");\
    }

#define ECS_BINARY_BOOL_OP(left, right, result, op)\
    if (left->type == ecs_id(ecs_bool_t)) { \
        ECS_BINARY_OP_T(left, right, result, op, ecs_bool_t, ecs_bool_t);\
    } else {\
        ecs_abort(ECS_INTERNAL_ERROR, "unexpected type in binary expression");\
    }

#define ECS_BINARY_UINT_OP(left, right, result, op)\
    if (left->type == ecs_id(ecs_u64_t)) { \
        ECS_BINARY_OP_T(left, right, result, op, ecs_u64_t, ecs_u64_t);\
    } else {\
        ecs_abort(ECS_INTERNAL_ERROR, "unexpected type in binary expression");\
    }

int flecs_expr_unary_visit_fold(
    ecs_script_t *script,
    ecs_expr_node_t **node_ptr)
{
    ecs_expr_unary_t *node = (ecs_expr_unary_t*)*node_ptr;

    if (node->operator != EcsTokNot) {
        flecs_expr_visit_error(script, node, 
            "operator invalid for unary expression");
        goto error;
    }

    if (flecs_script_expr_visit_fold(script, &node->expr)) {
        goto error;
    }

    if (node->expr->kind != EcsExprValue) {
        /* Only folding literals for now */
        return 0;
    }

    if (node->node.type != ecs_id(ecs_bool_t)) {
        char *type_str = ecs_get_path(script->world, node->node.type);
        flecs_expr_visit_error(script, node, 
            "! operator cannot be applied to value of type '%s' (must be bool)");
        ecs_os_free(type_str);
        goto error;
    }

    ecs_expr_val_t *result = flecs_calloc_t(
        &((ecs_script_impl_t*)script)->allocator, ecs_expr_val_t);
    result->node.kind = EcsExprValue;
    result->node.pos = node->node.pos;
    result->node.type = ecs_id(ecs_bool_t);
    result->ptr = &result->storage.bool_;
    *(bool*)result->ptr = !(*(bool*)((ecs_expr_val_t*)node)->ptr);

    return 0;
error:
    return -1;
}

int flecs_expr_binary_visit_fold(
    ecs_script_t *script,
    ecs_expr_node_t **node_ptr)
{
    ecs_expr_binary_t *node = (ecs_expr_binary_t*)*node_ptr;

    if (flecs_script_expr_visit_fold(script, &node->left)) {
        goto error;
    }

    if (flecs_script_expr_visit_fold(script, &node->right)) {
        goto error;
    }

    if (node->left->kind != EcsExprValue || node->right->kind != EcsExprValue) {
        /* Only folding literals for now */
        return 0;
    }

    ecs_expr_val_t *result = flecs_calloc_t(
        &((ecs_script_impl_t*)script)->allocator, ecs_expr_val_t);
    result->ptr = &result->storage.u64;
    result->node.kind = EcsExprValue;
    result->node.pos = node->node.pos;
    result->node.type = node->node.type;

    switch(node->operator) {
    case EcsTokAdd:
        ECS_BINARY_OP(node->left, node->right, result, +);
        break;
    case EcsTokSub:
        ECS_BINARY_OP(node->left, node->right, result, -);
        break;
    case EcsTokMul:
        ECS_BINARY_OP(node->left, node->right, result, *);
        break;
    case EcsTokDiv:
        ECS_BINARY_OP(node->left, node->right, result, /);
        break;
    case EcsTokMod:
        ECS_BINARY_INT_OP(node->left, node->right, result, %);
        break;
    case EcsTokEq:
        ECS_BINARY_COND_EQ_OP(node->left, node->right, result, ==);
        break;
    case EcsTokNeq:
        ECS_BINARY_COND_EQ_OP(node->left, node->right, result, !=);
        break;
    case EcsTokGt:
        ECS_BINARY_COND_OP(node->left, node->right, result, >);
        break;
    case EcsTokGtEq:
        ECS_BINARY_COND_OP(node->left, node->right, result, >=);
        break;
    case EcsTokLt:
        ECS_BINARY_COND_OP(node->left, node->right, result, <);
        break;
    case EcsTokLtEq:
        ECS_BINARY_COND_OP(node->left, node->right, result, <=);
        break;
    case EcsTokAnd:
        ECS_BINARY_BOOL_OP(node->left, node->right, result, &&);
        break;
    case EcsTokOr:
        ECS_BINARY_BOOL_OP(node->left, node->right, result, ||);
        break;
    case EcsTokShiftLeft:
        ECS_BINARY_UINT_OP(node->left, node->right, result, <<);
        break;
    case EcsTokShiftRight:
        ECS_BINARY_UINT_OP(node->left, node->right, result, >>);
        break;
    default:
        flecs_expr_visit_error(script, node->left, "unsupported operator");
        goto error;
    }

    *node_ptr = (ecs_expr_node_t*)result;

    return 0;
error:  
    return -1;
}

int flecs_script_expr_visit_fold(
    ecs_script_t *script,
    ecs_expr_node_t **node_ptr)
{
    ecs_assert(node_ptr != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_expr_node_t *node = *node_ptr;

    switch(node->kind) {
    case EcsExprValue:
        break;
    case EcsExprUnary:
        if (flecs_expr_unary_visit_fold(script, node_ptr)) {
            goto error;
        }
        break;
    case EcsExprBinary:
        if (flecs_expr_binary_visit_fold(script, node_ptr)) {
            goto error;
        }
        break;
    case EcsExprIdentifier:
        break;
    case EcsExprVariable:
        break;
    case EcsExprFunction:
        break;
    case EcsExprMember:
        break;
    case EcsExprElement:
        break;
    case EcsExprCast:
        break;
    }

    return 0;
error:
    return -1;
}

#endif
