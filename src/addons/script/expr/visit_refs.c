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
            if (identifier->symbol < ecs_vec_count(&impl->state.symbol_slots)) {
                entity = ecs_vec_get_t(
                    &impl->state.symbol_slots, ecs_script_symbol_slot_t,
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

typedef struct flecs_expr_ref_ctx_t {
    const ecs_script_t *script;
    flecs_expr_ref_action_t action;
    void *ctx;
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
    switch(node->kind) {
    case EcsExprGlobalVariable: {
        ecs_expr_variable_t *n = (ecs_expr_variable_t*)node;
        if (n->global && n->global_component) {
            ecs_script_ref_t ref = {
                .entity = n->global,
                .component = n->global_component
            };
            return ctx->action(&ref, NULL, ctx->ctx);
        }
        break;
    }
    case EcsExprFunction:
    case EcsExprMethod: {
        ecs_expr_function_t *n = (ecs_expr_function_t*)node;
        if (flecs_expr_visit_children(node, flecs_expr_ref_visit, ctx)) {
            return -1;
        }
        if (n->calldata.is.callback == flecs_script_user_function_callback)
        {
            const EcsScriptFunction *fn = ecs_get(
                script->world, n->calldata.function, EcsScriptFunction);
            if (fn && fn->binding_ctx) {
                ecs_script_user_function_t *uf = fn->binding_ctx;
                ecs_script_ref_t *uf_refs = ecs_vec_first(&uf->refs);
                int32_t i, count = ecs_vec_count(&uf->refs);
                for (i = 0; i < count; i ++) {
                    if (ctx->action(&uf_refs[i], NULL, ctx->ctx)) {
                        return -1;
                    }
                }
            }
        }
        return 0;
    }
    case EcsExprComponent:
    case EcsExprHas: {
        bool is_has = node->kind == EcsExprHas;
        ecs_expr_node_t *left = is_has
            ? ((ecs_expr_has_t*)node)->left
            : ((ecs_expr_element_t*)node)->left;
        ecs_script_ref_t ref = {
            .entity = flecs_expr_ref_entity(script, left),
            .component = is_has ? ((ecs_expr_has_t*)node)->id : node->type,
            .is_has = is_has
        };
        if (ref.component) {
            if (!ref.entity) {
                ref.name = flecs_expr_ref_var_name(left);
            }
            if (ctx->action(&ref, ref.entity ? NULL : node, ctx->ctx)) {
                return -1;
            }
        }
        return flecs_expr_ref_visit(left, ctx);
    }
    default:
        break;
    }
    return flecs_expr_visit_children(node, flecs_expr_ref_visit, ctx);
}

int flecs_expr_visit_refs(
    const ecs_script_t *script,
    ecs_expr_node_t *node,
    flecs_expr_ref_action_t action,
    void *ctx)
{
    flecs_expr_ref_ctx_t visitor = {script, action, ctx};
    return flecs_expr_ref_visit(node, &visitor);
}

#endif
