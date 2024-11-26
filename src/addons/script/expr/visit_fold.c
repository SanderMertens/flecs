/**
 * @file addons/script/expr_fold.c
 * @brief Script expression constant folding.
 */

#include "flecs.h"

#ifdef FLECS_SCRIPT
#include "../script.h"

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
    ecs_expr_node_t **node_ptr,
    const ecs_script_expr_run_desc_t *desc)
{
    ecs_expr_unary_t *node = (ecs_expr_unary_t*)*node_ptr;

    if (node->operator != EcsTokNot) {
        flecs_expr_visit_error(script, node, 
            "operator invalid for unary expression");
        goto error;
    }

    if (flecs_script_expr_visit_fold(script, &node->expr, desc)) {
        goto error;
    }

    if (node->expr->kind != EcsExprValue) {
        /* Only folding literals for now */
        return 0;
    }

    if (node->expr->type != ecs_id(ecs_bool_t)) {
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
    *(bool*)result->ptr = !*(bool*)(((ecs_expr_val_t*)node->expr)->ptr);

    *node_ptr = (ecs_expr_node_t*)result;

    return 0;
error:
    return -1;
}

int flecs_expr_binary_visit_fold(
    ecs_script_t *script,
    ecs_expr_node_t **node_ptr,
    const ecs_script_expr_run_desc_t *desc)
{
    ecs_expr_binary_t *node = (ecs_expr_binary_t*)*node_ptr;

    if (flecs_script_expr_visit_fold(script, &node->left, desc)) {
        goto error;
    }

    if (flecs_script_expr_visit_fold(script, &node->right, desc)) {
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

    /* Handle bitmask separately since it's not done by switch */
    if (ecs_get(script->world, node->node.type, EcsBitmask) != NULL) {
        ecs_assert(node->left->type == node->node.type, 
            ECS_INTERNAL_ERROR, NULL);
        ecs_assert(node->right->type == node->node.type, 
            ECS_INTERNAL_ERROR, NULL);
        ecs_expr_val_t *left = (ecs_expr_val_t*)node->left;
        ecs_expr_val_t *right = (ecs_expr_val_t*)node->right;
        result->storage.u32 = *(uint32_t*)left->ptr | *(uint32_t*)right->ptr;
        goto done;
    }

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
    case EcsTokBitwiseAnd:
        ECS_BINARY_INT_OP(node->left, node->right, result, &);
        break;
    case EcsTokBitwiseOr:
        ECS_BINARY_INT_OP(node->left, node->right, result, |);
        break;
    case EcsTokShiftLeft:
        ECS_BINARY_INT_OP(node->left, node->right, result, <<);
        break;
    case EcsTokShiftRight:
        ECS_BINARY_INT_OP(node->left, node->right, result, >>);
        break;
    default:
        flecs_expr_visit_error(script, node->left, "unsupported operator");
        goto error;
    }

done:
    *node_ptr = (ecs_expr_node_t*)result;
    return 0;
error:  
    return -1;
}

int flecs_expr_cast_visit_fold(
    ecs_script_t *script,
    ecs_expr_node_t **node_ptr,
    const ecs_script_expr_run_desc_t *desc)
{
    ecs_expr_cast_t *node = (ecs_expr_cast_t*)*node_ptr;

    if (flecs_script_expr_visit_fold(script, &node->expr, desc)) {
        goto error;
    }

    if (node->expr->kind != EcsExprValue) {
        /* Only folding literals for now */
        return 0;
    }

    ecs_expr_val_t *expr = (ecs_expr_val_t*)node->expr;

    /* Reuse existing node to hold casted value */
    *node_ptr = (ecs_expr_node_t*)expr;

    ecs_entity_t dst_type = node->node.type;
    ecs_entity_t src_type = expr->node.type;

    if (dst_type == src_type) {
        /* No cast necessary if types are equal */
        return 0;
    }

    ecs_meta_cursor_t cur = ecs_meta_cursor(script->world, dst_type, expr->ptr);
    ecs_value_t value = {
        .type = src_type,
        .ptr = expr->ptr
    };

    ecs_meta_set_value(&cur, &value);

    expr->node.type = dst_type;

    return 0;
error:  
    return -1;  
}

int flecs_expr_initializer_pre_fold(
    ecs_script_t *script,
    ecs_expr_initializer_t *node,
    const ecs_script_expr_run_desc_t *desc,
    bool *can_fold)
{
    ecs_expr_initializer_element_t *elems = ecs_vec_first(&node->elements);
    int32_t i, count = ecs_vec_count(&node->elements);
    for (i = 0; i < count; i ++) {
        ecs_expr_initializer_element_t *elem = &elems[i];

        /* If this is a nested initializer, don't fold it but instead fold its
         * values. Because nested initializers are flattened, this ensures that
         * we'll be using the correct member offsets later. */
        if (elem->value->kind == EcsExprInitializer) {
            if (flecs_expr_initializer_pre_fold(
                script, (ecs_expr_initializer_t*)elem->value, desc, can_fold)) 
            {
                goto error;
            }
            continue;
        }

        if (flecs_script_expr_visit_fold(script, &elem->value, desc)) {
            goto error;
        }

        if (elem->value->kind != EcsExprValue) {
            *can_fold = false;
        }
    }

    return 0;
error:
    return -1;
}

int flecs_expr_initializer_post_fold(
    ecs_script_t *script,
    ecs_expr_initializer_t *node,
    void *value)
{
    ecs_expr_initializer_element_t *elems = ecs_vec_first(&node->elements);
    int32_t i, count = ecs_vec_count(&node->elements);
    for (i = 0; i < count; i ++) {
        ecs_expr_initializer_element_t *elem = &elems[i];

        if (elem->value->kind == EcsExprInitializer) {
            if (flecs_expr_initializer_post_fold(
                script, (ecs_expr_initializer_t*)elem->value, value)) 
            {
                goto error;
            }
            continue;
        }

        ecs_expr_val_t *elem_value = (ecs_expr_val_t*)elem->value;

        /* Type is guaranteed to be correct, since type visitor will insert
         * a cast to the type of the initializer element. */
        ecs_entity_t type = elem_value->node.type;

        if (ecs_value_copy(script->world, type, 
            ECS_OFFSET(value, elem->offset), elem_value->ptr)) 
        {
            goto error;
        }
    }

    return 0;
error:
    return -1;
}

int flecs_expr_initializer_visit_fold(
    ecs_script_t *script,
    ecs_expr_node_t **node_ptr,
    const ecs_script_expr_run_desc_t *desc)
{
    bool can_fold = true;

    ecs_expr_initializer_t *node = (ecs_expr_initializer_t*)*node_ptr;
    if (flecs_expr_initializer_pre_fold(script, node, desc, &can_fold)) {
        goto error;
    }

    /* If all elements of initializer fold to literals, initializer itself can
     * be folded into a literal. */
    if (can_fold) {
        void *value = ecs_value_new(script->world, node->node.type);
        ecs_expr_initializer_t *node = (ecs_expr_initializer_t*)*node_ptr;

        if (flecs_expr_initializer_post_fold(script, node, value)) {
            goto error;
        }

        ecs_expr_val_t *result = flecs_calloc_t(
            &((ecs_script_impl_t*)script)->allocator, ecs_expr_val_t);
        result->node.kind = EcsExprValue;
        result->node.pos = node->node.pos;
        result->node.type = node->node.type;
        result->ptr = value;
        *node_ptr = (ecs_expr_node_t*)result;
    }

    return 0;
error:
    return -1;
}

int flecs_expr_identifier_visit_fold(
    ecs_script_t *script,
    ecs_expr_node_t **node_ptr,
    const ecs_script_expr_run_desc_t *desc)
{
    ecs_expr_identifier_t *node = (ecs_expr_identifier_t*)*node_ptr;
    ecs_entity_t type = node->node.type;

    ecs_expr_val_t *result = flecs_calloc_t(
        &((ecs_script_impl_t*)script)->allocator, ecs_expr_val_t);
    result->node.kind = EcsExprValue;
    result->node.pos = node->node.pos;
    result->node.type = type;

    if (type == ecs_id(ecs_entity_t)) {
        result->storage.entity = desc->lookup_action(
            script->world, node->value, desc->lookup_ctx);
        if (!result->storage.entity) {
            flecs_expr_visit_error(script, node, 
                "unresolved identifier '%s'", node->value);
            goto error;
        }
        result->ptr = &result->storage.entity;
    } else {
        ecs_meta_cursor_t cur = ecs_meta_cursor(
            script->world, type, &result->storage.u64);
        if (ecs_meta_set_string(&cur, node->value)) {
            goto error;
        }
        result->ptr = &result->storage.u64;
    }

    *node_ptr = (ecs_expr_node_t*)result;

    return 0;
error:
    return -1;
}

int flecs_script_expr_visit_fold(
    ecs_script_t *script,
    ecs_expr_node_t **node_ptr,
    const ecs_script_expr_run_desc_t *desc)
{
    ecs_assert(node_ptr != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_expr_node_t *node = *node_ptr;

    switch(node->kind) {
    case EcsExprValue:
        break;
    case EcsExprInitializer:
        if (flecs_expr_initializer_visit_fold(script, node_ptr, desc)) {
            goto error;
        }
        break;
    case EcsExprUnary:
        if (flecs_expr_unary_visit_fold(script, node_ptr, desc)) {
            goto error;
        }
        break;
    case EcsExprBinary:
        if (flecs_expr_binary_visit_fold(script, node_ptr, desc)) {
            goto error;
        }
        break;
    case EcsExprIdentifier:
        if (flecs_expr_identifier_visit_fold(script, node_ptr, desc)) {
            goto error;
        }
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
        if (flecs_expr_cast_visit_fold(script, node_ptr, desc)) {
            goto error;
        }
        break;
    }

    return 0;
error:
    return -1;
}

#endif
