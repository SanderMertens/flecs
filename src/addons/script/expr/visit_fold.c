/**
 * @file addons/script/expr_fold.c
 * @brief Script expression constant folding.
 */

#include "flecs.h"

#ifdef FLECS_SCRIPT
#include "../script.h"

void flecs_visit_fold_replace(
    ecs_script_t *script,
    ecs_expr_node_t **node_ptr,
    ecs_expr_node_t *with)
{
    ecs_assert(*node_ptr != with, ECS_INTERNAL_ERROR, NULL);
    flecs_script_expr_visit_free(script, *node_ptr);
    *node_ptr = with;
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
        /* Only folding literals */
        return 0;
    }

    if (node->expr->type != ecs_id(ecs_bool_t)) {
        char *type_str = ecs_get_path(script->world, node->node.type);
        flecs_expr_visit_error(script, node, 
            "! operator cannot be applied to value of type '%s' (must be bool)");
        ecs_os_free(type_str);
        goto error;
    }

    ecs_expr_val_t *result = flecs_expr_value_from(
        script, (ecs_expr_node_t*)node, ecs_id(ecs_bool_t));
    result->ptr = &result->storage.bool_;

    ecs_value_t dst = { .ptr = result->ptr, .type = ecs_id(ecs_bool_t) };
    ecs_value_t src = { 
        .ptr = ((ecs_expr_val_t*)node->expr)->ptr, .type = ecs_id(ecs_bool_t) };
    if (flecs_value_unary(script, &src, &dst, node->operator)) {
        goto error;
    }

    flecs_visit_fold_replace(script, node_ptr, (ecs_expr_node_t*)result);

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
        /* Only folding literals */
        return 0;
    }

    ecs_expr_val_t *left = (ecs_expr_val_t*)node->left;
    ecs_expr_val_t *right = (ecs_expr_val_t*)node->right;

    ecs_expr_val_t *result = flecs_expr_value_from(
        script, (ecs_expr_node_t*)node, node->node.type);

    /* Handle bitmask separately since it's not done by switch */
    if (ecs_get(script->world, node->node.type, EcsBitmask) != NULL) {
        ecs_assert(node->left->type == node->node.type, 
            ECS_INTERNAL_ERROR, NULL);
        ecs_assert(node->right->type == node->node.type, 
            ECS_INTERNAL_ERROR, NULL);
        result->storage.u32 = *(uint32_t*)left->ptr | *(uint32_t*)right->ptr;
        goto done;
    }

    ecs_value_t lop = { .type = left->node.type, .ptr = left->ptr };
    ecs_value_t rop = { .type = right->node.type, .ptr = right->ptr };
    ecs_value_t res = { .type = result->node.type, .ptr = result->ptr };

    if (flecs_value_binary(script, &lop, &rop, &res, node->operator)) {
        goto error;
    }

done:
    flecs_visit_fold_replace(script, node_ptr, (ecs_expr_node_t*)result);
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

    if (ecs_meta_set_value(&cur, &value)) {
        flecs_expr_visit_error(script, node, "failed to assign value");
        goto error;
    }

    expr->node.type = dst_type;

    node->expr = NULL; /* Prevent cleanup */
    flecs_visit_fold_replace(script, node_ptr, (ecs_expr_node_t*)expr);

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

    if (node->is_dynamic) {
        *can_fold = false;
        return 0;
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

        ecs_expr_val_t *result = flecs_expr_value_from(
            script, (ecs_expr_node_t*)node, node->node.type);
        result->ptr = value;

        flecs_visit_fold_replace(script, node_ptr, (ecs_expr_node_t*)result);
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

    ecs_expr_val_t *result = flecs_expr_value_from(
        script, (ecs_expr_node_t*)node, type);

    if (type == ecs_id(ecs_entity_t)) {
        result->storage.entity = desc->lookup_action(
            script->world, node->value, desc->lookup_ctx);
        if (!result->storage.entity) {
            flecs_script_expr_visit_free(script, (ecs_expr_node_t*)result);
            flecs_expr_visit_error(script, node, 
                "unresolved identifier '%s'", node->value);
            goto error;
        }
        result->ptr = &result->storage.entity;
    } else {
        ecs_meta_cursor_t cur = ecs_meta_cursor(
            script->world, type, &result->storage.u64);
        if (ecs_meta_set_string(&cur, node->value)) {
            flecs_script_expr_visit_free(script, (ecs_expr_node_t*)result);
            goto error;
        }
        result->ptr = &result->storage.u64;
    }

    flecs_visit_fold_replace(script, node_ptr, (ecs_expr_node_t*)result);

    return 0;
error:
    return -1;
}

int flecs_expr_function_visit_fold(
    ecs_script_t *script,
    ecs_expr_node_t **node_ptr,
    const ecs_script_expr_run_desc_t *desc)
{
    ecs_expr_function_t *node = (ecs_expr_function_t*)*node_ptr;

    if (flecs_script_expr_visit_fold(script, &node->left, desc)) {
        goto error;
    }

    return 0;
error:
    return -1;
}

int flecs_expr_member_visit_fold(
    ecs_script_t *script,
    ecs_expr_node_t **node_ptr,
    const ecs_script_expr_run_desc_t *desc)
{
    ecs_expr_member_t *node = (ecs_expr_member_t*)*node_ptr;

    if (flecs_script_expr_visit_fold(script, &node->left, desc)) {
        goto error;
    }

    return 0;
error:
    return -1;
}

int flecs_expr_element_visit_fold(
    ecs_script_t *script,
    ecs_expr_node_t **node_ptr,
    const ecs_script_expr_run_desc_t *desc)
{
    ecs_expr_element_t *node = (ecs_expr_element_t*)*node_ptr;

    if (flecs_script_expr_visit_fold(script, &node->left, desc)) {
        goto error;
    }

    if (flecs_script_expr_visit_fold(script, &node->index, desc)) {
        goto error;
    }

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
        if (flecs_expr_function_visit_fold(script, node_ptr, desc)) {
            goto error;
        }
        break;
    case EcsExprMember:
        if (flecs_expr_member_visit_fold(script, node_ptr, desc)) {
            goto error;
        }
        break;
    case EcsExprElement:
    case EcsExprComponent:
        if (flecs_expr_element_visit_fold(script, node_ptr, desc)) {
            goto error;
        }
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
