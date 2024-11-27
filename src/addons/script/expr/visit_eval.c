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
    ecs_entity_t type)
{
    const EcsPrimitive *p = ecs_get(script->world, type, EcsPrimitive);
    if (!p) {
        ecs_abort(ECS_UNSUPPORTED, 
            "non-primitive temporary values not yet supported");
    }

    val->value.type = type;

    switch (p->kind) {
    case EcsBool:    val->value.ptr = &val->storage.bool_; break;
    case EcsChar:    val->value.ptr = &val->storage.char_; break;
    case EcsByte:    val->value.ptr = &val->storage.byte_; break;
    case EcsU8:      val->value.ptr = &val->storage.u8; break;
    case EcsU16:     val->value.ptr = &val->storage.u16; break;
    case EcsU32:     val->value.ptr = &val->storage.u32; break;
    case EcsU64:     val->value.ptr = &val->storage.u64; break;
    case EcsI8:      val->value.ptr = &val->storage.i8; break;
    case EcsI16:     val->value.ptr = &val->storage.i16; break;
    case EcsI32:     val->value.ptr = &val->storage.i32; break;
    case EcsI64:     val->value.ptr = &val->storage.i64; break;
    case EcsF32:     val->value.ptr = &val->storage.f32; break;
    case EcsF64:     val->value.ptr = &val->storage.f64; break;
    case EcsUPtr:    val->value.ptr = &val->storage.uptr; break;
    case EcsIPtr:    val->value.ptr = &val->storage.iptr; break;
    case EcsString:  val->value.ptr = &val->storage.string; break;
    case EcsEntity:  val->value.ptr = &val->storage.entity; break;
    case EcsId:      val->value.ptr = &val->storage.id; break;
    default:
        ecs_abort(ECS_INTERNAL_ERROR, "invalid primitive kind");
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
    flecs_expr_value_alloc(script, out, node->node.type);

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
    flecs_expr_value_alloc(script, out, node->node.type);

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
    flecs_expr_value_alloc(script, out, node->node.type);

    /* Copy expression result to storage of casted-to type */
    if (flecs_value_copy_to(script->world, &out->value, &expr.value)) {
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
        break;
    case EcsExprUnary:
        if (flecs_expr_unary_visit_eval(
            script, (ecs_expr_binary_t*)node, desc, out)) 
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

    flecs_value_copy_to(script->world, out, &val.value);

    return 0;
error:  
    return -1;
}

#endif
