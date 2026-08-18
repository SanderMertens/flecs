/**
 * @file addons/script/expr/visit_refs.c
 * @brief Visitor to discover components referenced by an expression.
 */

#include "flecs.h"

#ifdef FLECS_SCRIPT
#include "../script.h"

static ecs_entity_t flecs_expr_ref_entity(
    const ecs_script_t *script,
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
        ecs_expr_identifier_t *identifier = (ecs_expr_identifier_t*)node;
        ecs_entity_t entity = flecs_expr_ref_entity(script, identifier->expr);
        if (!entity && identifier->symbol != -1) {
            ecs_script_impl_t *impl = flecs_script_impl(
                ECS_CONST_CAST(ecs_script_t*, script));
            if (identifier->symbol < ecs_vec_count(&impl->symbol_slots)) {
                entity = ecs_vec_get_t(
                    &impl->symbol_slots, ecs_script_symbol_slot_t,
                    identifier->symbol)->entity;
            }
        }
        return entity;
    }

    return 0;
}

static const char* flecs_expr_ref_var_name(
    ecs_expr_node_t *node)
{
    if (!node) {
        return NULL;
    }

    if (node->kind == EcsExprVariable) {
        ecs_expr_variable_t *var = (ecs_expr_variable_t*)node;
        if (var->node.type == ecs_id(ecs_entity_t)) {
            return var->name;
        }
    }

    return NULL;
}

static void flecs_expr_add_ref(
    ecs_vec_t *refs,
    ecs_entity_t entity,
    const char *name,
    ecs_id_t component,
    bool is_has)
{
    ecs_script_ref_t *elems = ecs_vec_first(refs);
    int32_t i, count = ecs_vec_count(refs);
    for (i = 0; i < count; i ++) {
        if (elems[i].entity == entity && elems[i].component == component &&
            elems[i].is_has == is_has)
        {
            const char *elem_name = elems[i].name;
            if ((!elem_name && !name) || (elem_name && name &&
                !ecs_os_strcmp(elem_name, name)))
            {
                return;
            }
        }
    }

    ecs_script_ref_t *ref = ecs_vec_append_t(NULL, refs, ecs_script_ref_t);
    ref->entity = entity;
    ref->name = name;
    ref->component = component;
    ref->observer = 0;
    ref->input = 0;
    ref->is_has = is_has;
    ref->is_resolve = false;
}

int flecs_expr_visit_refs(
    const ecs_script_t *script,
    ecs_expr_node_t *node,
    ecs_vec_t *refs,
    ecs_vec_t *dynamic_refs,
    ecs_vec_t *fn_refs)
{
    if (!node) {
        return 0;
    }

    switch(node->kind) {
    case EcsExprValue:
    case EcsExprVariable:
        break;
    case EcsExprGlobalVariable: {
        ecs_expr_variable_t *n = (ecs_expr_variable_t*)node;
        if (refs && n->global && n->global_component) {
            flecs_expr_add_ref(refs, n->global, NULL, n->global_component,
                false);
        }
        break;
    }
    case EcsExprInterpolatedString: {
        ecs_expr_interpolated_string_t *n =
            (ecs_expr_interpolated_string_t*)node;
        int32_t i, count = ecs_vec_count(&n->expressions);
        ecs_expr_node_t **expressions = ecs_vec_first(&n->expressions);
        for (i = 0; i < count; i ++) {
            if (flecs_expr_visit_refs(
                script, expressions[i], refs, dynamic_refs, fn_refs))
            {
                goto error;
            }
        }
        ecs_expr_format_t *formats = ecs_vec_first(&n->formats);
        count = ecs_vec_count(&n->formats);
        for (i = 0; i < count; i ++) {
            if (formats[i].width && flecs_expr_visit_refs(script,
                formats[i].width, refs, dynamic_refs, fn_refs))
            {
                goto error;
            }
            if (formats[i].precision && flecs_expr_visit_refs(script,
                formats[i].precision, refs, dynamic_refs, fn_refs))
            {
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
            if (elems[i].key) {
                if (flecs_expr_visit_refs(
                    script, elems[i].key, refs, dynamic_refs, fn_refs))
                {
                    goto error;
                }
            }
            if (flecs_expr_visit_refs(
                script, elems[i].value, refs, dynamic_refs, fn_refs))
            {
                goto error;
            }
        }
        break;
    }
    case EcsExprUnary:
        if (flecs_expr_visit_refs(script,
            ((ecs_expr_unary_t*)node)->expr, refs, dynamic_refs, fn_refs))
        {
            goto error;
        }
        break;
    case EcsExprBinary: {
        ecs_expr_binary_t *n = (ecs_expr_binary_t*)node;
        if (flecs_expr_visit_refs(script, n->left, refs, dynamic_refs, fn_refs)) {
            goto error;
        }
        if (flecs_expr_visit_refs(script, n->right, refs, dynamic_refs, fn_refs)) {
            goto error;
        }
        break;
    }
    case EcsExprIdentifier:
        if (flecs_expr_visit_refs(script,
            ((ecs_expr_identifier_t*)node)->expr, refs, dynamic_refs, fn_refs))
        {
            goto error;
        }
        break;
    case EcsExprFunction:
    case EcsExprMethod: {
        ecs_expr_function_t *n = (ecs_expr_function_t*)node;
        if (flecs_expr_visit_refs(script, n->left, refs, dynamic_refs, fn_refs)) {
            goto error;
        }
        if (flecs_expr_visit_refs(script,
            (ecs_expr_node_t*)n->args, refs, dynamic_refs, fn_refs))
        {
            goto error;
        }
        if (fn_refs &&
            n->calldata.is.callback == flecs_script_user_function_callback)
        {
            const EcsScriptFunction *fn = ecs_get(
                script->world, n->calldata.function, EcsScriptFunction);
            if (fn && fn->binding_ctx) {
                ecs_script_user_function_t *uf = fn->binding_ctx;
                ecs_script_ref_t *uf_refs = ecs_vec_first(&uf->refs);
                int32_t i, count = ecs_vec_count(&uf->refs);
                for (i = 0; i < count; i ++) {
                    if (refs) {
                        flecs_expr_add_ref(refs, uf_refs[i].entity,
                            uf_refs[i].name, uf_refs[i].component,
                            uf_refs[i].is_has);
                    }
                    flecs_expr_add_ref(fn_refs, uf_refs[i].entity,
                        uf_refs[i].name, uf_refs[i].component,
                        uf_refs[i].is_has);
                }
            }
        }
        break;
    }
    case EcsExprMember:
        if (flecs_expr_visit_refs(script,
            ((ecs_expr_member_t*)node)->left, refs, dynamic_refs, fn_refs))
        {
            goto error;
        }
        break;
    case EcsExprSwizzle:
        if (flecs_expr_visit_refs(script,
            ((ecs_expr_swizzle_t*)node)->left, refs, dynamic_refs, fn_refs))
        {
            goto error;
        }
        break;
    case EcsExprElement: {
        ecs_expr_element_t *n = (ecs_expr_element_t*)node;
        if (flecs_expr_visit_refs(script, n->left, refs, dynamic_refs, fn_refs)) {
            goto error;
        }
        if (flecs_expr_visit_refs(script, n->index, refs, dynamic_refs, fn_refs)) {
            goto error;
        }
        break;
    }
    case EcsExprComponent: {
        ecs_expr_element_t *n = (ecs_expr_element_t*)node;
        ecs_entity_t entity = flecs_expr_ref_entity(script, n->left);
        ecs_id_t component = n->node.type;
        if (entity && component) {
            flecs_expr_add_ref(refs, entity, NULL, component, false);
        } else if (component && dynamic_refs) {
            const char *var_name = flecs_expr_ref_var_name(n->left);
            if (var_name) {
                flecs_expr_add_ref(dynamic_refs, 0, var_name, component,
                    false);
            }
        }
        if (flecs_expr_visit_refs(script, n->left, refs, dynamic_refs, fn_refs)) {
            goto error;
        }
        break;
    }
    case EcsExprHas: {
        ecs_expr_has_t *n = (ecs_expr_has_t*)node;
        ecs_id_t component = n->id;
        ecs_entity_t entity = flecs_expr_ref_entity(script, n->left);
        if (entity && component) {
            flecs_expr_add_ref(refs, entity, NULL, component, true);
        } else if (component && dynamic_refs) {
            const char *var_name = flecs_expr_ref_var_name(n->left);
            if (var_name) {
                flecs_expr_add_ref(dynamic_refs, 0, var_name, component,
                    true);
            }
        }
        if (flecs_expr_visit_refs(script, n->left, refs, dynamic_refs, fn_refs)) {
            goto error;
        }
        break;
    }
    case EcsExprMatch: {
        ecs_expr_match_t *n = (ecs_expr_match_t*)node;
        if (flecs_expr_visit_refs(script, n->expr, refs, dynamic_refs, fn_refs)) {
            goto error;
        }
        int32_t i, count = ecs_vec_count(&n->elements);
        ecs_expr_match_element_t *elems = ecs_vec_first(&n->elements);
        for (i = 0; i < count; i ++) {
            if (flecs_expr_visit_refs(
                script, elems[i].compare, refs, dynamic_refs, fn_refs))
            {
                goto error;
            }
            if (flecs_expr_visit_refs(
                script, elems[i].expr, refs, dynamic_refs, fn_refs))
            {
                goto error;
            }
        }
        if (flecs_expr_visit_refs(script, n->any.compare, refs, dynamic_refs, fn_refs)) {
            goto error;
        }
        if (flecs_expr_visit_refs(script, n->any.expr, refs, dynamic_refs, fn_refs)) {
            goto error;
        }
        break;
    }
    case EcsExprRange: {
        ecs_expr_range_t *n = (ecs_expr_range_t*)node;
        if (flecs_expr_visit_refs(script, n->from, refs, dynamic_refs, fn_refs)) {
            goto error;
        }
        if (flecs_expr_visit_refs(script, n->to, refs, dynamic_refs, fn_refs)) {
            goto error;
        }
        break;
    }
    case EcsExprCast:
    case EcsExprCastNumber:
        if (flecs_expr_visit_refs(script,
            ((ecs_expr_cast_t*)node)->expr, refs, dynamic_refs, fn_refs))
        {
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
