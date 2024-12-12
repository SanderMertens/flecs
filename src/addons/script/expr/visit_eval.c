/**
 * @file addons/script/expr_ast.c
 * @brief Script expression AST implementation.
 */

#include "flecs.h"

#ifdef FLECS_SCRIPT
#include "../script.h"

typedef struct ecs_script_eval_ctx_t {
    const ecs_script_t *script;
    ecs_world_t *world;
    const ecs_expr_eval_desc_t *desc;
    ecs_expr_stack_t *stack;
} ecs_script_eval_ctx_t;

static
int flecs_expr_visit_eval_priv(
    ecs_script_eval_ctx_t *ctx,
    ecs_expr_node_t *node,
    ecs_expr_value_t *out);

static
int flecs_expr_value_visit_eval(
    ecs_script_eval_ctx_t *ctx,
    ecs_expr_value_node_t *node,
    ecs_expr_value_t *out)
{
    (void)ctx;
    out->value.type = node->node.type;
    out->value.ptr = node->ptr;
    out->owned = false;
    return 0;
}

static
int flecs_expr_initializer_eval(
    ecs_script_eval_ctx_t *ctx,
    ecs_expr_initializer_t *node,
    void *value);

static
int flecs_expr_initializer_eval_static(
    ecs_script_eval_ctx_t *ctx,
    ecs_expr_initializer_t *node,
    void *value)
{
    flecs_expr_stack_push(ctx->stack);

    ecs_expr_initializer_element_t *elems = ecs_vec_first(&node->elements);
    int32_t i, count = ecs_vec_count(&node->elements);
    for (i = 0; i < count; i ++) {
        ecs_expr_initializer_element_t *elem = &elems[i];

        if (elem->value->kind == EcsExprInitializer) {
            if (flecs_expr_initializer_eval(ctx, 
                (ecs_expr_initializer_t*)elem->value, value)) 
            {
                goto error;
            }
            continue;
        }

        ecs_expr_value_t *expr = flecs_expr_stack_result(ctx->stack, elem->value);
        if (flecs_expr_visit_eval_priv(ctx, elem->value, expr)) {
            goto error;
        }

        /* Type is guaranteed to be correct, since type visitor will insert
         * a cast to the type of the initializer element. */
        ecs_entity_t type = elem->value->type;

        if (expr->owned) {
            if (ecs_value_move(ctx->world, type, 
                ECS_OFFSET(value, elem->offset), expr->value.ptr))
            {
                goto error;
            }
        } else {
            if (ecs_value_copy(ctx->world, type, 
                ECS_OFFSET(value, elem->offset), expr->value.ptr))
            {
                goto error;
            }
        }
    }

    flecs_expr_stack_pop(ctx->stack);
    return 0;
error:
    flecs_expr_stack_pop(ctx->stack);
    return -1;
}

static
int flecs_expr_initializer_eval_dynamic(
    ecs_script_eval_ctx_t *ctx,
    ecs_expr_initializer_t *node,
    void *value)
{
    flecs_expr_stack_push(ctx->stack);

    ecs_meta_cursor_t cur = ecs_meta_cursor(
        ctx->world, node->node.type, value);
    
    if (ecs_meta_push(&cur)) {
        goto error;
    }

    int32_t i, count = ecs_vec_count(&node->elements);
    ecs_expr_initializer_element_t *elems = ecs_vec_first(&node->elements);
    for (i = 0; i < count; i ++) {
        ecs_expr_initializer_element_t *elem = &elems[i];

        if (i) {
            ecs_meta_next(&cur);
        }

        if (elem->value->kind == EcsExprInitializer) {
            if (flecs_expr_initializer_eval(ctx, 
                (ecs_expr_initializer_t*)elem->value, value)) 
            {
                goto error;
            }
            continue;
        }

        ecs_expr_value_t *expr = flecs_expr_stack_result(ctx->stack, elem->value);
        if (flecs_expr_visit_eval_priv(ctx, elem->value, expr)) {
            goto error;
        }

        if (elem->member) {
            ecs_meta_member(&cur, elem->member);
        }

        ecs_value_t v_elem_value = {
            .ptr = expr->value.ptr,
            .type = expr->value.type
        };

        if (ecs_meta_set_value(&cur, &v_elem_value)) {
            goto error;
        }
    }

    if (ecs_meta_pop(&cur)) {
        goto error;
    }

    flecs_expr_stack_pop(ctx->stack);
    return 0;
error:
    flecs_expr_stack_pop(ctx->stack);
    return -1;
}

static
int flecs_expr_initializer_eval(
    ecs_script_eval_ctx_t *ctx,
    ecs_expr_initializer_t *node,
    void *value)
{
    if (node->is_dynamic) {
        return flecs_expr_initializer_eval_dynamic(ctx, node, value);
    } else {
        return flecs_expr_initializer_eval_static(ctx, node, value);
    }
}

static
int flecs_expr_initializer_visit_eval(
    ecs_script_eval_ctx_t *ctx,
    ecs_expr_initializer_t *node,
    ecs_expr_value_t *out)
{
    out->owned = true;
    return flecs_expr_initializer_eval(ctx, node, out->value.ptr);
}

static
int flecs_expr_unary_visit_eval(
    ecs_script_eval_ctx_t *ctx,
    ecs_expr_unary_t *node,
    ecs_expr_value_t *out)
{
    flecs_expr_stack_push(ctx->stack);

    ecs_expr_value_t *expr = flecs_expr_stack_result(ctx->stack, node->expr);
    if (flecs_expr_visit_eval_priv(ctx, node->expr, expr)) {
        goto error;
    }

    if (flecs_value_unary(
        ctx->script, &expr->value, &out->value, node->operator)) 
    {
        goto error;
    }

    flecs_expr_stack_pop(ctx->stack);
    return 0;
error:
    flecs_expr_stack_pop(ctx->stack);
    return -1;
}

static
int flecs_expr_binary_visit_eval(
    ecs_script_eval_ctx_t *ctx,
    ecs_expr_binary_t *node,
    ecs_expr_value_t *out)
{
    flecs_expr_stack_push(ctx->stack);

    /* Evaluate left & right expressions */
    ecs_expr_value_t *left = flecs_expr_stack_result(ctx->stack, node->left);
    if (flecs_expr_visit_eval_priv(ctx, node->left, left)) {
        goto error;
    }

    ecs_expr_value_t *right = flecs_expr_stack_result(ctx->stack, node->right);
    if (flecs_expr_visit_eval_priv(ctx, node->right, right)) {
        goto error;
    }

    if (flecs_value_binary(
        ctx->script, &left->value, &right->value, &out->value, node->operator)) 
    {
        goto error;
    }

    flecs_expr_stack_pop(ctx->stack);
    return 0;
error:
    flecs_expr_stack_pop(ctx->stack);
    return -1;
}

static
int flecs_expr_identifier_visit_eval(
    ecs_script_eval_ctx_t *ctx,
    ecs_expr_identifier_t *node,
    ecs_expr_value_t *out)
{
    return flecs_expr_visit_eval_priv(ctx, node->expr, out);
}

static
int flecs_expr_variable_visit_eval(
    ecs_script_eval_ctx_t *ctx,
    ecs_expr_variable_t *node,
    ecs_expr_value_t *out)
{
    ecs_assert(ctx->desc != NULL, ECS_INVALID_OPERATION,
        "variables available at parse time are not provided");
    ecs_assert(ctx->desc->vars != NULL, ECS_INVALID_OPERATION,
        "variables available at parse time are not provided");

    const ecs_script_var_t *var = ecs_script_vars_lookup(
        ctx->desc->vars, node->name);
    if (!var) {
        flecs_expr_visit_error(ctx->script, node, "unresolved variable '%s'",
            node->name);
        goto error;
    }

    /* Should've been populated by type visitor */
    ecs_assert(var != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(var->value.type == node->node.type, ECS_INTERNAL_ERROR, NULL);
    out->value = var->value;
    out->owned = false;
    return 0;
error:
    return -1;
}

static
int flecs_expr_global_variable_visit_eval(
    ecs_script_eval_ctx_t *ctx,
    ecs_expr_variable_t *node,
    ecs_expr_value_t *out)
{
    ecs_assert(ctx->desc != NULL, ECS_INVALID_OPERATION,
        "variables available at parse time are not provided");

    ecs_assert(node->global_value.type == node->node.type, 
        ECS_INTERNAL_ERROR, NULL);
    out->value = node->global_value;
    out->owned = false;

    return 0;
}

static
int flecs_expr_cast_visit_eval(
    ecs_script_eval_ctx_t *ctx,
    ecs_expr_cast_t *node,
    ecs_expr_value_t *out)
{
    flecs_expr_stack_push(ctx->stack);

    /* Evaluate expression to cast */
    ecs_expr_value_t *expr = flecs_expr_stack_result(ctx->stack, node->expr);
    if (flecs_expr_visit_eval_priv(ctx, node->expr, expr)) {
        goto error;
    }

    /* Copy expression result to storage of casted-to type */
    if (flecs_value_copy_to(ctx->world, &out->value, &expr->value)) {
        flecs_expr_visit_error(ctx->script, node, "failed to cast value");
        goto error;
    }

    flecs_expr_stack_pop(ctx->stack);
    return 0;
error:
    flecs_expr_stack_pop(ctx->stack);
    return -1;
}

static
int flecs_expr_function_args_visit_eval(
    ecs_script_eval_ctx_t *ctx,
    ecs_expr_initializer_t *node,
    ecs_value_t *args)
{
    ecs_expr_initializer_element_t *elems = ecs_vec_first(&node->elements);
    int32_t i, count = ecs_vec_count(&node->elements);
    for (i = 0; i < count; i ++) {
        ecs_expr_initializer_element_t *elem = &elems[i];
        ecs_expr_value_t *expr = flecs_expr_stack_result(
            ctx->stack, elem->value);

        if (flecs_expr_visit_eval_priv(ctx, elem->value, expr)) {
            goto error;
        }

        args[i] = expr->value;
    }

    return 0;
error:  
    return -1;
}

static
int flecs_expr_function_visit_eval(
    ecs_script_eval_ctx_t *ctx,
    ecs_expr_function_t *node,
    ecs_expr_value_t *out)
{
    flecs_expr_stack_push(ctx->stack);

    ecs_function_ctx_t call_ctx = {
        .world = ctx->world,
        .function = node->calldata.function,
        .ctx = node->calldata.ctx
    };

    ecs_assert(out->value.ptr != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_value_t *argv = NULL;
    int32_t argc = ecs_vec_count(&node->args->elements);
    if (argc) {
        argv = ecs_os_alloca_n(ecs_value_t, argc);
        if (flecs_expr_function_args_visit_eval(ctx, node->args, argv)) {
            goto error;
        }
    }

    node->calldata.callback(&call_ctx, argc, argv, &out->value);
    out->owned = true;

    flecs_expr_stack_pop(ctx->stack);
    return 0;
error:
    flecs_expr_stack_pop(ctx->stack);
    return -1;
}

static
int flecs_expr_method_visit_eval(
    ecs_script_eval_ctx_t *ctx,
    ecs_expr_function_t *node,
    ecs_expr_value_t *out)
{
    flecs_expr_stack_push(ctx->stack);

    if (node->left) {
        ecs_expr_value_t *expr = flecs_expr_stack_result(ctx->stack, node->left);
        if (flecs_expr_visit_eval_priv(ctx, node->left, expr)) {
            goto error;
        }

        ecs_function_ctx_t call_ctx = {
            .world = ctx->world,
            .function = node->calldata.function,
            .ctx = node->calldata.ctx
        };

        ecs_assert(expr->value.ptr != NULL, ECS_INTERNAL_ERROR, NULL);
        ecs_assert(out->value.ptr != NULL, ECS_INTERNAL_ERROR, NULL);

        int32_t argc = ecs_vec_count(&node->args->elements);
        ecs_value_t *argv = ecs_os_alloca_n(ecs_value_t, argc + 1);
        argv[0] = expr->value;

        if (argc) {
            if (flecs_expr_function_args_visit_eval(
                ctx, node->args, &argv[1])) 
            {
                goto error;
            }
        }

        node->calldata.callback(&call_ctx, argc, argv, &out->value);
        out->owned = true;
    }

    flecs_expr_stack_pop(ctx->stack);
    return 0;
error:
    flecs_expr_stack_pop(ctx->stack);
    return -1;
}

static
int flecs_expr_member_visit_eval(
    ecs_script_eval_ctx_t *ctx,
    ecs_expr_member_t *node,
    ecs_expr_value_t *out)
{
    flecs_expr_stack_push(ctx->stack);

    ecs_expr_value_t *expr = flecs_expr_stack_result(ctx->stack, node->left);
    if (flecs_expr_visit_eval_priv(ctx, node->left, expr)) {
        goto error;
    }

    out->value.ptr = ECS_OFFSET(expr->value.ptr, node->offset);
    out->value.type = node->node.type;
    out->owned = false;

    flecs_expr_stack_pop(ctx->stack);
    return 0;
error:
    flecs_expr_stack_pop(ctx->stack);
    return -1;
}

static
int flecs_expr_element_visit_eval(
    ecs_script_eval_ctx_t *ctx,
    ecs_expr_element_t *node,
    ecs_expr_value_t *out)
{
    ecs_expr_value_t *expr = flecs_expr_stack_result(ctx->stack, node->left);
    if (flecs_expr_visit_eval_priv(ctx, node->left, expr)) {
        goto error;
    }

    ecs_expr_value_t *index = flecs_expr_stack_result(ctx->stack, node->index);
    if (flecs_expr_visit_eval_priv(ctx, node->index, index)) {
        goto error;
    }

    int64_t index_value = *(int64_t*)index->value.ptr;

    out->value.ptr = ECS_OFFSET(expr->value.ptr, node->elem_size * index_value);
    out->value.type = node->node.type;
    out->owned = false;

    return 0;
error:
    return -1;
}

static
int flecs_expr_component_visit_eval(
    ecs_script_eval_ctx_t *ctx,
    ecs_expr_element_t *node,
    ecs_expr_value_t *out)
{
    ecs_expr_value_t *left = flecs_expr_stack_result(ctx->stack, node->left);
    if (flecs_expr_visit_eval_priv(ctx, node->left, left)) {
        goto error;
    }

    /* Left side of expression must be of entity type */
    ecs_assert(left->value.type == ecs_id(ecs_entity_t), 
        ECS_INTERNAL_ERROR, NULL);

    /* Component must be resolvable at parse time */
    ecs_expr_node_t *index = node->index;
    if (index->kind == EcsExprIdentifier) {
        index = ((ecs_expr_identifier_t*)index)->expr;
    }

    ecs_assert(index->kind == EcsExprValue, ECS_INTERNAL_ERROR, NULL);

    ecs_entity_t entity = *(ecs_entity_t*)left->value.ptr;
    ecs_entity_t component = ((ecs_expr_value_node_t*)index)->storage.entity;

    ecs_assert(out->value.type == node->node.type, ECS_INTERNAL_ERROR, NULL);
    out->value.ptr = ECS_CONST_CAST(void*, 
        ecs_get_id(ctx->world, entity, component));
    out->owned = false;

    if (!out->value.ptr) {
        char *estr = ecs_get_path(ctx->world, entity);
        char *cstr = ecs_get_path(ctx->world, component);
        flecs_expr_visit_error(ctx->script, node, 
            "entity '%s' does not have component '%s'", estr, cstr);
        ecs_os_free(estr);
        ecs_os_free(cstr);
        goto error;
    }

    return 0;
error:
    return -1;
}

static
int flecs_expr_visit_eval_priv(
    ecs_script_eval_ctx_t *ctx,
    ecs_expr_node_t *node,
    ecs_expr_value_t *out)
{
    ecs_assert(node != NULL, ECS_INVALID_PARAMETER, NULL);

    switch(node->kind) {
    case EcsExprValue:
        if (flecs_expr_value_visit_eval(
            ctx, (ecs_expr_value_node_t*)node, out)) 
        {
            goto error;
        }
        break;
    case EcsExprEmptyInitializer:
        break;
    case EcsExprInitializer:
        if (flecs_expr_initializer_visit_eval(
            ctx, (ecs_expr_initializer_t*)node, out)) 
        {
            goto error;
        }
        break;
    case EcsExprUnary:
        if (flecs_expr_unary_visit_eval(
            ctx, (ecs_expr_unary_t*)node, out))
        {
            goto error;
        }
        break;
    case EcsExprBinary:
        if (flecs_expr_binary_visit_eval(
            ctx, (ecs_expr_binary_t*)node, out)) 
        {
            goto error;
        }
        break;
    case EcsExprIdentifier:
        if (flecs_expr_identifier_visit_eval(
            ctx, (ecs_expr_identifier_t*)node, out)) 
        {
            goto error;
        }
        break;
    case EcsExprVariable:
        if (flecs_expr_variable_visit_eval(
            ctx, (ecs_expr_variable_t*)node, out)) 
        {
            goto error;
        }
        break;
    case EcsExprGlobalVariable:
        if (flecs_expr_global_variable_visit_eval(
            ctx, (ecs_expr_variable_t*)node, out)) 
        {
            goto error;
        }
        break;
    case EcsExprFunction:
        if (flecs_expr_function_visit_eval(
            ctx, (ecs_expr_function_t*)node, out)) 
        {
            goto error;
        }
        break;
    case EcsExprMethod:
        if (flecs_expr_method_visit_eval(
            ctx, (ecs_expr_function_t*)node, out)) 
        {
            goto error;
        }
        break;
    case EcsExprMember:
        if (flecs_expr_member_visit_eval(
            ctx, (ecs_expr_member_t*)node, out)) 
        {
            goto error;
        }
        break;
    case EcsExprElement:
        if (flecs_expr_element_visit_eval(
            ctx, (ecs_expr_element_t*)node, out)) 
        {
            goto error;
        }
        break;
    case EcsExprComponent:
        if (flecs_expr_component_visit_eval(
            ctx, (ecs_expr_element_t*)node, out)) 
        {
            goto error;
        }
        break;
    case EcsExprCast:
        if (flecs_expr_cast_visit_eval(
            ctx, (ecs_expr_cast_t*)node, out)) 
        {
            goto error;
        }
        break;
    }

    return 0;
error:
    return -1;
}

int flecs_expr_visit_eval(
    const ecs_script_t *script,
    ecs_expr_node_t *node,
    const ecs_expr_eval_desc_t *desc,
    ecs_value_t *out)
{
    ecs_expr_stack_t *stack = NULL, stack_local;
    if (desc && desc->runtime) {
        stack = &desc->runtime->expr_stack;
        ecs_assert(stack->frame == 0, ECS_INTERNAL_ERROR, NULL);
    }
    if (!stack) {
        stack = &stack_local;
        flecs_expr_stack_init(stack);
    }

    flecs_expr_stack_push(stack);

    ecs_expr_value_t *val = flecs_expr_stack_result(stack, node);

    ecs_script_eval_ctx_t ctx = {
        .script = script,
        .world = script->world,
        .stack = stack,
        .desc = desc
    };

    if (flecs_expr_visit_eval_priv(&ctx, node, val)) {
        goto error;
    }

    if (desc && !out->type) {
        out->type = desc->type;
    }

    if (!out->type) {
        out->type = node->type;
    }

    if (out->type && !out->ptr) {
        out->ptr = ecs_value_new(ctx.world, out->type);
    }

    if (val->owned) {
        /* Values owned by the runtime can be moved to output */
        if (flecs_value_move_to(ctx.world, out, &val->value)) {
            flecs_expr_visit_error(script, node, "failed to write to output");
            goto error;
        }
    } else {
        /* Values not owned by runtime should be copied */
        if (flecs_value_copy_to(ctx.world, out, &val->value)) {
            flecs_expr_visit_error(script, node, "failed to write to output");
            goto error;
        }
    }

    flecs_expr_stack_pop(stack);
    if (stack == &stack_local) {
        flecs_expr_stack_fini(stack);
    }
    return 0;
error:
    flecs_expr_stack_pop(stack);
    if (stack == &stack_local) {
        flecs_expr_stack_fini(stack);
    }
    return -1;
}

#endif
