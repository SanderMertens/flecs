/**
 * @file addons/script/expr/visit_fold.c
 * @brief Script expression constant folding.
 */

#include "flecs.h"

#ifdef FLECS_SCRIPT
#include "../script.h"

typedef struct flecs_expr_fold_ctx_t {
    ecs_script_t *script;
    const ecs_expr_eval_desc_t *desc;
    bool can_fold;
} flecs_expr_fold_ctx_t;

static int flecs_expr_fold_child(
    ecs_expr_node_t **node,
    void *ptr)
{
    flecs_expr_fold_ctx_t *ctx = ptr;
    if (!*node || !(*node)->type) {
        return 0;
    }
    if (flecs_expr_visit_fold(ctx->script, node, ctx->desc)) {
        return -1;
    }
    ctx->can_fold &= (*node)->kind == EcsExprValue;
    return 0;
}

static int flecs_expr_fold_children(
    ecs_script_t *script,
    ecs_expr_node_t *node,
    const ecs_expr_eval_desc_t *desc)
{
    flecs_expr_fold_ctx_t ctx = {script, desc, true};
    return flecs_expr_visit_children(node, flecs_expr_fold_child, &ctx);
}

static void flecs_visit_fold_replace(
    ecs_script_t *script,
    ecs_expr_node_t **node_ptr,
    ecs_expr_node_t *with)
{
    ecs_assert(*node_ptr != with, ECS_INTERNAL_ERROR, NULL);
    flecs_expr_visit_free(script, *node_ptr);
    *node_ptr = with;
}

static int flecs_expr_fold_eval(
    ecs_script_t *script,
    ecs_expr_node_t **node_ptr,
    const ecs_expr_eval_desc_t *desc)
{
    ecs_expr_node_t *node = *node_ptr;
    if (node->kind == EcsExprUnary) {
        ecs_expr_unary_t *unary = (ecs_expr_unary_t*)node;
        ecs_expr_value_node_t *result = (ecs_expr_value_node_t*)unary->expr;
        ecs_value_t value = { .type = result->node.type, .ptr = result->ptr };
        if (flecs_value_unary(script, &value, &value, unary->operator)) {
            return -1;
        }
        unary->expr = NULL;
        result->node.pos = node->pos;
        result->node.end = node->end;
        flecs_visit_fold_replace(script, node_ptr, (ecs_expr_node_t*)result);
        return 0;
    }
    ecs_expr_value_node_t *result = flecs_expr_value_from(script, node, node->type);
    const ecs_type_info_t *ti = result->node.type_info;
    if (ti->size > ECS_SIZEOF(result->storage) || ti->hooks.dtor) {
        result->ptr = ecs_ptr_new_w_type_info(script->world, ti);
        flecs_type_info_claim(ti);
    } else {
        flecs_type_info_ctor(result->ptr, 1, ti);
    }
    ecs_value_t value = { .type = node->type, .ptr = result->ptr };
    int ret;
    if (node->kind == EcsExprCastNumber) {
        ecs_expr_value_node_t *src = (ecs_expr_value_node_t*)
            ((ecs_expr_cast_t*)node)->expr;
        ecs_meta_cursor_t cur = ecs_meta_cursor(
            script->world, node->type, value.ptr);
        ret = ecs_meta_set_value(&cur,
            &(ecs_value_t){ .type = src->node.type, .ptr = src->ptr });
        if (ret) {
            flecs_expr_visit_error(script, node, "failed to assign value");
        }
    } else {
        ret = flecs_expr_visit_eval(script, node, desc, &value);
    }
    if (ret) {
        flecs_expr_visit_free(script, (ecs_expr_node_t*)result);
        return -1;
    }
    flecs_visit_fold_replace(script, node_ptr, (ecs_expr_node_t*)result);
    return 0;
}

static int flecs_expr_binary_visit_fold(
    ecs_script_t *script,
    ecs_expr_node_t **node_ptr,
    const ecs_expr_eval_desc_t *desc)
{
    ecs_expr_binary_t *node = (ecs_expr_binary_t*)*node_ptr;

    if (flecs_expr_visit_fold(script, &node->left, desc)) {
        goto error;
    }

    if (!node->vector_count &&
        (node->operator == EcsTokAnd || node->operator == EcsTokOr))
    {
        if (node->left->kind == EcsExprValue &&
            node->left->type == ecs_id(ecs_bool_t))
        {
            ecs_expr_value_node_t *lnode =
                (ecs_expr_value_node_t*)node->left;
            bool lval = *(bool*)lnode->ptr;
            if ((node->operator == EcsTokAnd) != lval) {
                ecs_expr_value_node_t *result = flecs_expr_value_from(
                    script, (ecs_expr_node_t*)node, node->node.type);
                *(bool*)result->ptr = lval;
                flecs_visit_fold_replace(
                    script, node_ptr, (ecs_expr_node_t*)result);
                return 0;
            }
        }
    }

    if (flecs_expr_visit_fold(script, &node->right, desc)) {
        goto error;
    }

    if (node->left->kind != EcsExprValue || node->right->kind != EcsExprValue) {
        /* Only folding literals */
        return 0;
    }

    if (node->vector_count) {
        return 0;
    }

    return flecs_expr_fold_eval(script, node_ptr, desc);
error:
    return -1;
}

static int flecs_expr_pure_visit_fold(
    ecs_script_t *script,
    ecs_expr_node_t **node_ptr,
    const ecs_expr_eval_desc_t *desc)
{
    flecs_expr_fold_ctx_t ctx = {script, desc, true};
    if (flecs_expr_visit_children(*node_ptr, flecs_expr_fold_child, &ctx)) {
        return -1;
    }
    return ctx.can_fold ? flecs_expr_fold_eval(script, node_ptr, desc) : 0;
}

static int flecs_expr_initializer_pre_fold(
    ecs_script_t *script,
    ecs_expr_initializer_t *node,
    const ecs_expr_eval_desc_t *desc,
    bool *can_fold)
{
    ecs_expr_initializer_element_t *elems = ecs_vec_first(&node->elements);
    int32_t i, count = ecs_vec_count(&node->elements);
    for (i = 0; i < count; i ++) {
        ecs_expr_initializer_element_t *elem = &elems[i];
        ecs_assert(elem->value != NULL, ECS_INTERNAL_ERROR, NULL);

        if (elem->key) {
            if (flecs_expr_visit_fold(script, &elem->key, desc)) {
                goto error;
            }
        }

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

        if (flecs_expr_visit_fold(script, &elem->value, desc)) {
            goto error;
        }

        if (elem->value->kind != EcsExprValue) {
            *can_fold = false;
        }

        if (elem->operator) {
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

static int flecs_expr_initializer_visit_fold(
    ecs_script_t *script,
    ecs_expr_node_t **node_ptr,
    const ecs_expr_eval_desc_t *desc)
{
    ecs_expr_initializer_t *node = (ecs_expr_initializer_t*)*node_ptr;
    bool can_fold = !node->is_partial;
    if (flecs_expr_initializer_pre_fold(script, node, desc, &can_fold)) {
        return -1;
    }
    return can_fold ? flecs_expr_fold_eval(script, node_ptr, desc) : 0;
}

static int flecs_expr_identifier_visit_fold(
    ecs_script_t *script,
    ecs_expr_node_t **node_ptr,
    const ecs_expr_eval_desc_t *desc)
{
    (void)desc;

    ecs_expr_identifier_t *node = (ecs_expr_identifier_t*)*node_ptr;

    ecs_expr_node_t *expr = node->expr;
    if (expr) {
        node->expr = NULL;
        if (flecs_expr_visit_fold(script, &expr, desc)) {
            flecs_expr_visit_free(script, expr);
            goto error;
        }
        flecs_visit_fold_replace(script, node_ptr, expr);
    }

    return 0;
error:
    return -1;
}

static int flecs_expr_variable_visit_fold(
    ecs_script_t *script,
    ecs_expr_node_t **node_ptr,
    const ecs_expr_eval_desc_t *desc)
{
    (void)desc;

    ecs_expr_variable_t *node = (ecs_expr_variable_t*)*node_ptr;

    ecs_script_var_t *var = ecs_script_vars_from_sp(
        desc->vars, node->sp);
    /* Should've been caught by type visitor */
    ecs_assert(var != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(var->value.type == node->node.type, ECS_INTERNAL_ERROR, NULL);
    ecs_entity_t type = node->node.type;

    if (var->is_const && var->value.ptr) {
        ecs_expr_value_node_t *result = flecs_expr_value_from(
            script, (ecs_expr_node_t*)node, type);
        void *value = ecs_ptr_new(script->world, type);
        ecs_ptr_copy(script->world, type, value, var->value.ptr);
        result->ptr = value;
        flecs_type_info_claim(result->node.type_info);
        flecs_visit_fold_replace(script, node_ptr, (ecs_expr_node_t*)result);
    }

    return 0;
}

static int flecs_expr_global_variable_visit_fold(
    ecs_script_t *script,
    ecs_expr_node_t **node_ptr,
    const ecs_expr_eval_desc_t *desc)
{
    ecs_expr_variable_t *node = (ecs_expr_variable_t*)*node_ptr;
    ecs_entity_t type = node->node.type;

    /* A mut variable can change after the expression is compiled, so its value
     * is always read live. */
    if (node->global_component == ecs_id(EcsScriptMutVar)) {
        return 0;
    }

    /* In a template body the node is kept unfolded so its value is read live
     * from the const variable on every (re)instantiation. */
    ecs_script_eval_visitor_t *v = desc->script_visitor;
    if (v && v->template) {
        return 0;
    }

    /* Global const variables are always const, so we can always fold */

    ecs_value_t global_value = flecs_script_global_var_get(
        script->world, node->global, NULL);
    if (!global_value.ptr) {
        return 0;
    }

    ecs_expr_value_node_t *result = flecs_expr_value_from(
        script, (ecs_expr_node_t*)node, type);
    void *value = ecs_ptr_new(script->world, type);
    ecs_ptr_copy(script->world, type, value, global_value.ptr);
    result->ptr = value;
    flecs_type_info_claim(result->node.type_info);
    flecs_visit_fold_replace(script, node_ptr, (ecs_expr_node_t*)result);

    return 0;
}

static int flecs_expr_match_visit_fold(
    ecs_script_t *script,
    ecs_expr_node_t **node_ptr,
    const ecs_expr_eval_desc_t *desc)
{
    ecs_expr_match_t *node = (ecs_expr_match_t*)*node_ptr;

    if (flecs_expr_fold_children(script, &node->node, desc)) {
        goto error;
    }
    int32_t i, count = ecs_vec_count(&node->elements);
    ecs_expr_match_element_t *elems = ecs_vec_first(&node->elements);

    if (node->expr->kind != EcsExprValue) {
        return 0;
    }

    ecs_expr_value_node_t *input = (ecs_expr_value_node_t*)node->expr;
    ecs_value_t input_value = {
        .type = input->node.type,
        .ptr = input->ptr
    };

    ecs_expr_node_t **selected = NULL;

    for (i = 0; i < count; i ++) {
        ecs_expr_match_element_t *elem = &elems[i];
        if (elem->compare->kind != EcsExprValue) {
            return 0;
        }

        ecs_expr_value_node_t *compare = 
            (ecs_expr_value_node_t*)elem->compare;
        ecs_value_t compare_value = {
            .type = compare->node.type,
            .ptr = compare->ptr
        };

        bool equal = false;
        ecs_value_t result = { .type = ecs_id(ecs_bool_t), .ptr = &equal };
        if (flecs_value_binary(script, &node->node, &input_value, 
            &compare_value, &result, EcsTokEq))
        {
            goto error;
        }

        if (equal) {
            selected = &elem->expr;
            break;
        }
    }

    if (!selected) {
        if (!node->any.expr) {
            return 0;
        }
        selected = &node->any.expr;
    }

    if ((*selected)->kind != EcsExprValue) {
        return 0;
    }

    ecs_expr_node_t *value = *selected;
    *selected = NULL;
    flecs_visit_fold_replace(script, node_ptr, value);

    return 0;
error:
    return -1;
}

int flecs_expr_visit_fold(
    ecs_script_t *script,
    ecs_expr_node_t **node_ptr,
    const ecs_expr_eval_desc_t *desc)
{
    ecs_assert(node_ptr != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_expr_node_t *node = *node_ptr;

    switch(node->kind) {
    case EcsExprValue:
        break;
    case EcsExprInterpolatedString:
    case EcsExprUnary:
    case EcsExprCast:
    case EcsExprCastNumber:
        return flecs_expr_pure_visit_fold(script, node_ptr, desc);
    case EcsExprInitializer:
    case EcsExprEmptyInitializer:
        return flecs_expr_initializer_visit_fold(script, node_ptr, desc);
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
        if (flecs_expr_variable_visit_fold(script, node_ptr, desc)) {
            goto error;
        }
        break;
    case EcsExprGlobalVariable:
        if (flecs_expr_global_variable_visit_fold(script, node_ptr, desc)) {
            goto error;
        }
        break;
    case EcsExprFunction:
    case EcsExprMethod: {
        ecs_expr_function_t *function = (ecs_expr_function_t*)node;
        if (flecs_expr_visit_fold(script, &function->left, desc)) {
            goto error;
        }
        return function->args ? flecs_expr_fold_children(
            script, (ecs_expr_node_t*)function->args, desc) : 0;
    }
    case EcsExprMember:
    case EcsExprSwizzle:
    case EcsExprElement:
    case EcsExprComponent:
    case EcsExprRange:
        return flecs_expr_fold_children(script, node, desc);
    case EcsExprHas:
        if (flecs_expr_visit_fold(
            script, &((ecs_expr_has_t*)node)->left, desc))
        {
            goto error;
        }
        break;
    case EcsExprMatch:
        if (flecs_expr_match_visit_fold(script, node_ptr, desc)) {
            goto error;
        }
        break;
    case EcsExprNew:
    case EcsExprScript:
        break;
    }

    return 0;
error:
    return -1;
}

#endif
