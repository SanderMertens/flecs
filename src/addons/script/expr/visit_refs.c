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

static void flecs_expr_add_dyn_node(
    ecs_vec_t *dyn_nodes,
    ecs_expr_node_t *node)
{
    ecs_expr_node_t **elems = ecs_vec_first(dyn_nodes);
    int32_t i, count = ecs_vec_count(dyn_nodes);
    for (i = 0; i < count; i ++) {
        if (elems[i] == node) {
            return;
        }
    }

    ecs_vec_append_t(NULL, dyn_nodes, ecs_expr_node_t*)[0] = node;
}

typedef struct flecs_expr_ref_ctx_t {
    const ecs_script_t *script;
    ecs_vec_t *refs;
    ecs_vec_t *dynamic_refs;
    ecs_vec_t *dyn_nodes;
} flecs_expr_ref_ctx_t;

static int flecs_expr_ref_visit(
    ecs_expr_node_t *node,
    void *ptr)
{
    if (!node) {
        return 0;
    }
    flecs_expr_ref_ctx_t *ctx = ptr;
    const ecs_script_t *script = ctx->script;
    ecs_vec_t *refs = ctx->refs;
    ecs_vec_t *dynamic_refs = ctx->dynamic_refs;
    ecs_vec_t *dyn_nodes = ctx->dyn_nodes;
    switch(node->kind) {
    case EcsExprGlobalVariable: {
        ecs_expr_variable_t *n = (ecs_expr_variable_t*)node;
        if (refs && n->global && n->global_component) {
            flecs_expr_add_ref(refs, n->global, NULL, n->global_component,
                false);
        }
        break;
    }
    case EcsExprFunction:
    case EcsExprMethod: {
        ecs_expr_function_t *n = (ecs_expr_function_t*)node;
        if (flecs_expr_visit_children(node, flecs_expr_ref_visit, ctx)) {
            return -1;
        }
        if (refs &&
            n->calldata.is.callback == flecs_script_user_function_callback)
        {
            const EcsScriptFunction *fn = ecs_get(
                script->world, n->calldata.function, EcsScriptFunction);
            if (fn && fn->binding_ctx) {
                ecs_script_user_function_t *uf = fn->binding_ctx;
                ecs_script_ref_t *uf_refs = ecs_vec_first(&uf->refs);
                int32_t i, count = ecs_vec_count(&uf->refs);
                for (i = 0; i < count; i ++) {
                    flecs_expr_add_ref(refs, uf_refs[i].entity,
                        uf_refs[i].name, uf_refs[i].component,
                        uf_refs[i].is_has);
                }
            }
        }
        return 0;
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
        if (!entity && component && dyn_nodes) {
            flecs_expr_add_dyn_node(dyn_nodes, node);
        }
        return flecs_expr_ref_visit(n->left, ctx);
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
        if (!entity && component && dyn_nodes) {
            flecs_expr_add_dyn_node(dyn_nodes, node);
        }
        return flecs_expr_ref_visit(n->left, ctx);
    }
    default:
        break;
    }
    return flecs_expr_visit_children(node, flecs_expr_ref_visit, ctx);
}

int flecs_expr_visit_refs(
    const ecs_script_t *script,
    ecs_expr_node_t *node,
    ecs_vec_t *refs,
    ecs_vec_t *dynamic_refs,
    ecs_vec_t *dyn_nodes)
{
    flecs_expr_ref_ctx_t ctx = {script, refs, dynamic_refs, dyn_nodes};
    return flecs_expr_ref_visit(node, &ctx);
}

#endif
