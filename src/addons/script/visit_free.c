/**
 * @file addons/script/visit_free.c
 * @brief Script free visitor (frees AST resources).
 */

#include "flecs.h"

#ifdef FLECS_SCRIPT
#include "script.h"

static int flecs_script_scope_free(
    ecs_script_scope_t *scope,
    void *ctx)
{
    return flecs_script_visit_free_node(ctx, (ecs_script_node_t*)scope);
}

static void flecs_script_id_free(
    ecs_script_t *script,
    ecs_script_id_t *id)
{
    flecs_expr_visit_free(script, id->first_expr);
    flecs_expr_visit_free(script, id->second_expr);
}

int flecs_script_visit_free_node(
    ecs_script_t *script,
    ecs_script_node_t *node)
{
    ecs_check(script != NULL, ECS_INVALID_PARAMETER, NULL);
    if (!node) {
        return 0;
    }
    ecs_allocator_t *a = &flecs_script_impl(script)->allocator;
    flecs_script_visit_scopes(node, flecs_script_scope_free, script);
    switch (node->kind) {
    case EcsAstScope: {
        ecs_script_scope_t *scope = (ecs_script_scope_t*)node;
        ecs_script_node_t **stmts = ecs_vec_first(&scope->stmts);
        for (int32_t i = 0; i < ecs_vec_count(&scope->stmts); i ++) {
            flecs_script_visit_free_node(script, stmts[i]);
        }
        ecs_vec_fini_t(a, &scope->stmts, ecs_script_node_t*);
        ecs_vec_fini_t(a, &scope->components, ecs_id_t);
        break;
    }
    case EcsAstEntity:
        flecs_expr_visit_free(script, ((ecs_script_entity_t*)node)->name_expr);
        break;
    case EcsAstPairScope:
        flecs_script_id_free(script, &((ecs_script_pair_scope_t*)node)->id);
        break;
    case EcsAstIf:
        flecs_expr_visit_free(script, ((ecs_script_if_t*)node)->expr);
        break;
    case EcsAstFor: {
        ecs_script_for_t *stmt = (ecs_script_for_t*)node;
        flecs_expr_visit_free(script, stmt->from);
        flecs_expr_visit_free(script, stmt->to);
        flecs_expr_visit_free(script, stmt->expr);
        break;
    }
    case EcsAstTag:
    case EcsAstWithTag:
        flecs_script_id_free(script, &((ecs_script_tag_t*)node)->id);
        break;
    case EcsAstComponent:
    case EcsAstWithComponent: {
        ecs_script_component_t *stmt = (ecs_script_component_t*)node;
        flecs_expr_visit_free(script, stmt->expr);
        flecs_script_id_free(script, &stmt->id);
        break;
    }
    case EcsAstProp:
    case EcsAstMut:
    case EcsAstConst:
    case EcsAstExportConst:
    case EcsAstExportMut:
        flecs_expr_visit_free(script, ((ecs_script_var_node_t*)node)->expr);
        break;
    case EcsAstAwait:
        flecs_expr_visit_free(script, ((ecs_script_await_t*)node)->expr);
        break;
    case EcsAstTry:
        ecs_vec_fini_t(a, &((ecs_script_try_t*)node)->catches, ecs_script_catch_t);
        break;
    case EcsAstFunction: {
        ecs_script_function_node_t *fn = (ecs_script_function_node_t*)node;
        flecs_expr_visit_free(script, fn->return_expr);
        ecs_vec_fini_t(a, &fn->params, ecs_script_fn_param_t);
        break;
    }
    default:
        break;
    }
    flecs_free(a, node->alloc_size, node);
    return 0;
error:
    return -1;
}

int flecs_script_visit_free(
    ecs_script_t *script)
{
    ecs_check(script != NULL, ECS_INVALID_PARAMETER, NULL);
    return flecs_script_visit_free_node(script,
        (ecs_script_node_t*)flecs_script_impl(script)->root);
error:
    return -1;
}

#endif
