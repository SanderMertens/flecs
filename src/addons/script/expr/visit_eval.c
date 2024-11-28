/**
 * @file addons/script/expr_ast.c
 * @brief Script expression AST implementation.
 */

#include "flecs.h"

#ifdef FLECS_SCRIPT
#include "../script.h"

typedef struct ecs_eval_value_t {
    ecs_value_t value;
    ecs_expr_small_val_t storage;
} ecs_eval_value_t;

static
int flecs_script_expr_visit_eval_priv(
    ecs_script_t *script,
    ecs_expr_node_t *node,
    const ecs_script_expr_run_desc_t *desc,
    ecs_eval_value_t *out);

static
void flecs_expr_value_alloc(
    ecs_script_t *script,
    ecs_eval_value_t *val,
    const ecs_type_info_t *ti)
{
    ecs_assert(ti != NULL, ECS_INTERNAL_ERROR, NULL);

    val->value.type = ti->component;

    if (ti->size <= FLECS_EXPR_SMALL_DATA_SIZE) {
        if (!(ti->hooks.flags & ECS_TYPE_HOOK_DTOR)) {
            val->value.ptr = val->storage.small_data;
        }
    } else {
        ecs_abort(ECS_UNSUPPORTED, 
            "non-trivial temporary values not yet supported");
    }
}

static
int flecs_expr_value_visit_eval(
    ecs_script_t *script,
    ecs_expr_val_t *node,
    const ecs_script_expr_run_desc_t *desc,
    ecs_eval_value_t *out)
{
    out->value.type = node->node.type;
    out->value.ptr = node->ptr;
    return 0;
}

static
int flecs_expr_initializer_eval(
    ecs_script_t *script,
    ecs_expr_initializer_t *node,
    const ecs_script_expr_run_desc_t *desc,
    void *value)
{
    ecs_expr_initializer_element_t *elems = ecs_vec_first(&node->elements);
    int32_t i, count = ecs_vec_count(&node->elements);
    for (i = 0; i < count; i ++) {
        ecs_expr_initializer_element_t *elem = &elems[i];

        if (elem->value->kind == EcsExprInitializer) {
            if (flecs_expr_initializer_eval(
                script, (ecs_expr_initializer_t*)elem->value, desc, value)) 
            {
                goto error;
            }
            continue;
        }

        ecs_eval_value_t expr = {{0}};
        if (flecs_script_expr_visit_eval_priv(
            script, elem->value, desc, &expr)) 
        {
            goto error;
        }

        ecs_expr_val_t *elem_value = (ecs_expr_val_t*)elem->value;

        /* Type is guaranteed to be correct, since type visitor will insert
         * a cast to the type of the initializer element. */
        ecs_entity_t type = elem_value->node.type;

        if (ecs_value_copy(script->world, type, 
            ECS_OFFSET(value, elem->offset), expr.value.ptr))
        {
            goto error;
        }
    }

    return 0;
error:
    return -1;
}

static
int flecs_expr_initializer_visit_eval(
    ecs_script_t *script,
    ecs_expr_initializer_t *node,
    const ecs_script_expr_run_desc_t *desc,
    ecs_eval_value_t *out)
{
    /* Initialize storage for initializer's type */
    flecs_expr_value_alloc(script, out, node->node.type_info);
    return flecs_expr_initializer_eval(script, node, desc, out->value.ptr);
}

static
int flecs_expr_unary_visit_eval(
    ecs_script_t *script,
    ecs_expr_unary_t *node,
    const ecs_script_expr_run_desc_t *desc,
    ecs_eval_value_t *out)
{
    ecs_eval_value_t expr = {{0}};

    if (flecs_script_expr_visit_eval_priv(script, node->expr, desc, &expr)) {
        goto error;
    }

    /* Initialize storage of casted-to type */
    flecs_expr_value_alloc(script, out, node->node.type_info);

    if (flecs_value_unary(
        script, &expr.value, &out->value, node->operator)) 
    {
        goto error;
    }

    return 0;
error:
    return -1;
}

static
int flecs_expr_binary_visit_eval(
    ecs_script_t *script,
    ecs_expr_binary_t *node,
    const ecs_script_expr_run_desc_t *desc,
    ecs_eval_value_t *out)
{
    ecs_eval_value_t left = {{0}};
    ecs_eval_value_t right = {{0}};

    /* Evaluate left & right expressions */
    if (flecs_script_expr_visit_eval_priv(script, node->left, desc, &left)) {
        goto error;
    }
    if (flecs_script_expr_visit_eval_priv(script, node->right, desc, &right)) {
        goto error;
    }

    /* Initialize storage of casted-to type */
    flecs_expr_value_alloc(script, out, node->node.type_info);

    if (flecs_value_binary(
        script, &left.value, &right.value, &out->value, node->operator)) 
    {
        goto error;
    }

    return 0;
error:
    return -1;
}

static
int flecs_expr_variable_visit_eval(
    ecs_script_t *script,
    ecs_expr_variable_t *node,
    const ecs_script_expr_run_desc_t *desc,
    ecs_eval_value_t *out)
{
    const ecs_script_var_t *var = node->var;
    /* Should've been populated by type visitor */
    ecs_assert(var != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(var->value.type == node->node.type, ECS_INTERNAL_ERROR, NULL);
    out->value = var->value;
    return 0;
}

static
int flecs_expr_cast_visit_eval(
    ecs_script_t *script,
    ecs_expr_cast_t *node,
    const ecs_script_expr_run_desc_t *desc,
    ecs_eval_value_t *out)
{
    ecs_eval_value_t expr = {{0}};

    /* Evaluate expression to cast */
    if (flecs_script_expr_visit_eval_priv(script, node->expr, desc, &expr)) {
        goto error;
    }

    /* Initialize storage of casted-to type */
    flecs_expr_value_alloc(script, out, node->node.type_info);

    /* Copy expression result to storage of casted-to type */
    if (flecs_value_copy_to(script->world, &out->value, &expr.value)) {
        flecs_expr_visit_error(script, node, "failed to cast value");
        goto error;
    }

    return 0;
error:
    return -1;
}

static
int flecs_expr_member_visit_eval(
    ecs_script_t *script,
    ecs_expr_member_t *node,
    const ecs_script_expr_run_desc_t *desc,
    ecs_eval_value_t *out)
{
    ecs_eval_value_t expr = {{0}};

    if (flecs_script_expr_visit_eval_priv(script, node->left, desc, &expr)) {
        goto error;
    }

    out->value.ptr = ECS_OFFSET(expr.value.ptr, node->offset);
    out->value.type = node->node.type;

    return 0;
error:
    return -1;
}

static
int flecs_expr_element_visit_eval(
    ecs_script_t *script,
    ecs_expr_element_t *node,
    const ecs_script_expr_run_desc_t *desc,
    ecs_eval_value_t *out)
{
    ecs_eval_value_t expr = {{0}};
    if (flecs_script_expr_visit_eval_priv(script, node->left, desc, &expr)) {
        goto error;
    }

    ecs_eval_value_t index = {{0}};
    if (flecs_script_expr_visit_eval_priv(script, node->index, desc, &index)) {
        goto error;
    }

    int32_t index_value = *(int64_t*)index.value.ptr;

    out->value.ptr = ECS_OFFSET(expr.value.ptr, node->elem_size * index_value);
    out->value.type = node->node.type;

    return 0;
error:
    return -1;
}

static
int flecs_script_expr_visit_eval_priv(
    ecs_script_t *script,
    ecs_expr_node_t *node,
    const ecs_script_expr_run_desc_t *desc,
    ecs_eval_value_t *out)
{
    ecs_assert(node != NULL, ECS_INVALID_PARAMETER, NULL);

    switch(node->kind) {
    case EcsExprValue:
        if (flecs_expr_value_visit_eval(
            script, (ecs_expr_val_t*)node, desc, out)) 
        {
            goto error;
        }
        break;
    case EcsExprInitializer:
        if (flecs_expr_initializer_visit_eval(
            script, (ecs_expr_initializer_t*)node, desc, out)) 
        {
            goto error;
        }
        break;
    case EcsExprUnary:
        if (flecs_expr_unary_visit_eval(
            script, (ecs_expr_unary_t*)node, desc, out))
        {
            goto error;
        }
        break;
    case EcsExprBinary:
        if (flecs_expr_binary_visit_eval(
            script, (ecs_expr_binary_t*)node, desc, out)) 
        {
            goto error;
        }
        break;
    case EcsExprIdentifier:
        break;
    case EcsExprVariable:
        if (flecs_expr_variable_visit_eval(
            script, (ecs_expr_variable_t*)node, desc, out)) 
        {
            goto error;
        }
        break;
    case EcsExprFunction:
        break;
    case EcsExprMember:
        if (flecs_expr_member_visit_eval(
            script, (ecs_expr_member_t*)node, desc, out)) 
        {
            goto error;
        }
        break;
    case EcsExprElement:
        if (flecs_expr_element_visit_eval(
            script, (ecs_expr_element_t*)node, desc, out)) 
        {
            goto error;
        }
        break;
    case EcsExprCast:
        if (flecs_expr_cast_visit_eval(
            script, (ecs_expr_cast_t*)node, desc, out)) 
        {
            goto error;
        }
        break;
    }

    return 0;
error:
    return -1;
}

int flecs_script_expr_visit_eval(
    ecs_script_t *script,
    ecs_expr_node_t *node,
    const ecs_script_expr_run_desc_t *desc,
    ecs_value_t *out)
{
    ecs_eval_value_t val = {{0}};

    if (flecs_script_expr_visit_eval_priv(script, node, desc, &val)) {
        goto error;
    }

    if (desc && !out->type) {
        out->type = desc->type;
    }

    if (!out->type) {
        out->type = node->type;
    }

    if (out->type && !out->ptr) {
        out->ptr = ecs_value_new(script->world, out->type);
    }

    if (flecs_value_copy_to(script->world, out, &val.value)) {
        flecs_expr_visit_error(script, node, "failed to write to output");
        goto error;
    }

    return 0;
error:  
    return -1;
}

#endif
