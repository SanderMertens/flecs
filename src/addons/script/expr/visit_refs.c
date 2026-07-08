/**
 * @file addons/script/expr/visit_refs.c
 * @brief Visitor to discover components referenced by an expression.
 */

#include "flecs.h"

#ifdef FLECS_SCRIPT
#include "../script.h"

static
ecs_entity_t flecs_expr_ref_entity(
    ecs_expr_node_t *node)
{
    if (!node) {
        return 0;
    }

    if (node->kind == EcsExprValue) {
        ecs_expr_value_node_t *value = (ecs_expr_value_node_t*)node;
        if (value->ptr) {
            return *(ecs_entity_t*)value->ptr;
        }
    } else if (node->kind == EcsExprIdentifier) {
        return flecs_expr_ref_entity(((ecs_expr_identifier_t*)node)->expr);
    }

    return 0;
}

static
void flecs_expr_add_ref(
    ecs_vec_t *refs,
    ecs_entity_t entity,
    ecs_id_t component)
{
    ecs_script_ref_t *elems = ecs_vec_first(refs);
    int32_t i, count = ecs_vec_count(refs);
    for (i = 0; i < count; i ++) {
        if (elems[i].entity == entity && elems[i].component == component) {
            return;
        }
    }

    ecs_script_ref_t *ref = ecs_vec_append_t(NULL, refs, ecs_script_ref_t);
    ref->entity = entity;
    ref->component = component;
    ref->observer = 0;
}

int flecs_expr_visit_refs(
    const ecs_script_t *script,
    ecs_expr_node_t *node,
    ecs_vec_t *refs)
{
    if (!node) {
        return 0;
    }

    switch(node->kind) {
    case EcsExprValue:
    case EcsExprVariable:
        break;
    case EcsExprGlobalVariable: {
        ecs_entity_t global = ((ecs_expr_variable_t*)node)->global;
        if (global) {
            flecs_expr_add_ref(refs, global, ecs_id(EcsScriptConstVar));
        }
        break;
    }
    case EcsExprInterpolatedString: {
        ecs_expr_interpolated_string_t *n =
            (ecs_expr_interpolated_string_t*)node;
        int32_t i, count = ecs_vec_count(&n->expressions);
        ecs_expr_node_t **expressions = ecs_vec_first(&n->expressions);
        for (i = 0; i < count; i ++) {
            if (flecs_expr_visit_refs(script, expressions[i], refs)) {
                goto error;
            }
        }
        break;
    }
    case EcsExprInitializer:
    case EcsExprEmptyInitializer: {
        ecs_expr_initializer_t *n = (ecs_expr_initializer_t*)node;
        ecs_expr_initializer_element_t *elems = ecs_vec_first(&n->elements);
        int32_t i, count = ecs_vec_count(&n->elements);
        for (i = 0; i < count; i ++) {
            if (flecs_expr_visit_refs(script, elems[i].value, refs)) {
                goto error;
            }
        }
        break;
    }
    case EcsExprUnary:
        if (flecs_expr_visit_refs(script,
            ((ecs_expr_unary_t*)node)->expr, refs))
        {
            goto error;
        }
        break;
    case EcsExprBinary: {
        ecs_expr_binary_t *n = (ecs_expr_binary_t*)node;
        if (flecs_expr_visit_refs(script, n->left, refs)) {
            goto error;
        }
        if (flecs_expr_visit_refs(script, n->right, refs)) {
            goto error;
        }
        break;
    }
    case EcsExprIdentifier:
        if (flecs_expr_visit_refs(script,
            ((ecs_expr_identifier_t*)node)->expr, refs))
        {
            goto error;
        }
        break;
    case EcsExprFunction:
    case EcsExprMethod: {
        ecs_expr_function_t *n = (ecs_expr_function_t*)node;
        if (flecs_expr_visit_refs(script, n->left, refs)) {
            goto error;
        }
        if (flecs_expr_visit_refs(script, (ecs_expr_node_t*)n->args, refs)) {
            goto error;
        }
        break;
    }
    case EcsExprMember:
        if (flecs_expr_visit_refs(script,
            ((ecs_expr_member_t*)node)->left, refs))
        {
            goto error;
        }
        break;
    case EcsExprElement: {
        ecs_expr_element_t *n = (ecs_expr_element_t*)node;
        if (flecs_expr_visit_refs(script, n->left, refs)) {
            goto error;
        }
        if (flecs_expr_visit_refs(script, n->index, refs)) {
            goto error;
        }
        break;
    }
    case EcsExprComponent: {
        ecs_expr_element_t *n = (ecs_expr_element_t*)node;
        ecs_entity_t entity = flecs_expr_ref_entity(n->left);
        ecs_id_t component = n->node.type;
        if (entity && component) {
            flecs_expr_add_ref(refs, entity, component);
        }
        if (flecs_expr_visit_refs(script, n->left, refs)) {
            goto error;
        }
        break;
    }
    case EcsExprMatch: {
        ecs_expr_match_t *n = (ecs_expr_match_t*)node;
        if (flecs_expr_visit_refs(script, n->expr, refs)) {
            goto error;
        }
        int32_t i, count = ecs_vec_count(&n->elements);
        ecs_expr_match_element_t *elems = ecs_vec_first(&n->elements);
        for (i = 0; i < count; i ++) {
            if (flecs_expr_visit_refs(script, elems[i].compare, refs)) {
                goto error;
            }
            if (flecs_expr_visit_refs(script, elems[i].expr, refs)) {
                goto error;
            }
        }
        if (flecs_expr_visit_refs(script, n->any.compare, refs)) {
            goto error;
        }
        if (flecs_expr_visit_refs(script, n->any.expr, refs)) {
            goto error;
        }
        break;
    }
    case EcsExprCast:
    case EcsExprCastNumber:
        if (flecs_expr_visit_refs(script,
            ((ecs_expr_cast_t*)node)->expr, refs))
        {
            goto error;
        }
        break;
    case EcsExprNew:
        break;
    }

    return 0;
error:
    return -1;
}

#endif
