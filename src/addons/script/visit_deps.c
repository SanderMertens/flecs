/**
 * @file addons/script/visit_deps.c
 * @brief Visitor that computes which variables a statement depends on.
 */

#include "flecs.h"

#ifdef FLECS_SCRIPT
#include "script.h"

static
uint64_t flecs_script_node_visit_deps(
    const ecs_script_t *script,
    ecs_script_node_t *node,
    const ecs_script_vars_t *vars);

static
uint64_t flecs_script_var_deps(
    const ecs_script_vars_t *vars,
    const char *name)
{
    const ecs_script_var_t *var = flecs_script_find_var(vars, name, NULL);
    if (!var) {
        return 0;
    }
    return var->changed_mask;
}

static
uint64_t flecs_script_id_visit_deps(
    const ecs_script_t *script,
    ecs_script_id_t *id,
    const ecs_script_vars_t *vars)
{
    uint64_t result = 0;
    if (id->first_expr) {
        result |= flecs_expr_visit_deps(script, id->first_expr, vars);
    }
    if (id->second_expr) {
        result |= flecs_expr_visit_deps(script, id->second_expr, vars);
    }
    if (id->first && id->first[0] == '$') {
        result |= flecs_script_var_deps(vars, &id->first[1]);
    }
    if (id->second && id->second[0] == '$') {
        result |= flecs_script_var_deps(vars, &id->second[1]);
    }
    return result;
}

static
uint64_t flecs_script_scope_visit_deps(
    const ecs_script_t *script,
    ecs_script_scope_t *scope,
    const ecs_script_vars_t *vars)
{
    uint64_t result = 0;
    if (!scope) {
        return 0;
    }

    ecs_script_node_t **nodes = ecs_vec_first(&scope->stmts);
    int32_t i, count = ecs_vec_count(&scope->stmts);
    for (i = 0; i < count; i ++) {
        result |= flecs_script_node_visit_deps(script, nodes[i], vars);
    }

    scope->node.changed_mask = result;
    return result;
}

static
uint64_t flecs_script_node_visit_deps(
    const ecs_script_t *script,
    ecs_script_node_t *node,
    const ecs_script_vars_t *vars)
{
    uint64_t result = 0;

    switch(node->kind) {
    case EcsAstScope:
        result = flecs_script_scope_visit_deps(
            script, (ecs_script_scope_t*)node, vars);
        return result;
    case EcsAstTag:
    case EcsAstWithTag:
        result |= flecs_script_id_visit_deps(
            script, &((ecs_script_tag_t*)node)->id, vars);
        break;
    case EcsAstComponent:
    case EcsAstWithComponent: {
        ecs_script_component_t *comp = (ecs_script_component_t*)node;
        result |= flecs_script_id_visit_deps(script, &comp->id, vars);
        result |= flecs_expr_visit_deps(script, comp->expr, vars);
        break;
    }
    case EcsAstDefaultComponent: {
        ecs_script_default_component_t *comp =
            (ecs_script_default_component_t*)node;
        result |= flecs_expr_visit_deps(script, comp->expr, vars);
        break;
    }
    case EcsAstVarComponent:
    case EcsAstWithVar:
        result |= flecs_script_var_deps(
            vars, ((ecs_script_var_component_t*)node)->name);
        break;
    case EcsAstWith: {
        ecs_script_with_t *with = (ecs_script_with_t*)node;
        result |= flecs_script_scope_visit_deps(
            script, with->expressions, vars);
        result |= flecs_script_scope_visit_deps(script, with->scope, vars);
        break;
    }
    case EcsAstTemplate:
        flecs_script_scope_visit_deps(
            script, ((ecs_script_template_node_t*)node)->scope, vars);
        break;
    case EcsAstProp:
    case EcsAstConst:
    case EcsAstExportConst:
        result |= flecs_expr_visit_deps(
            script, ((ecs_script_var_node_t*)node)->expr, vars);
        break;
    case EcsAstEntity: {
        ecs_script_entity_t *entity = (ecs_script_entity_t*)node;
        result |= flecs_expr_visit_deps(script, entity->name_expr, vars);
        result |= flecs_script_scope_visit_deps(script, entity->scope, vars);
        break;
    }
    case EcsAstPairScope: {
        ecs_script_pair_scope_t *pair = (ecs_script_pair_scope_t*)node;
        result |= flecs_script_id_visit_deps(script, &pair->id, vars);
        result |= flecs_script_scope_visit_deps(script, pair->scope, vars);
        break;
    }
    case EcsAstIf: {
        ecs_script_if_t *if_node = (ecs_script_if_t*)node;
        result |= flecs_expr_visit_deps(script, if_node->expr, vars);
        result |= flecs_script_scope_visit_deps(
            script, if_node->if_true, vars);
        result |= flecs_script_scope_visit_deps(
            script, if_node->if_false, vars);
        break;
    }
    case EcsAstFor: {
        ecs_script_for_range_t *for_node = (ecs_script_for_range_t*)node;
        result |= flecs_expr_visit_deps(script, for_node->from, vars);
        result |= flecs_expr_visit_deps(script, for_node->to, vars);
        result |= flecs_script_scope_visit_deps(script, for_node->scope, vars);
        break;
    }
    case EcsAstFunction: {
        ecs_script_function_node_t *fn = (ecs_script_function_node_t*)node;
        flecs_script_scope_visit_deps(script, fn->body, vars);
        flecs_expr_visit_deps(script, fn->return_expr, vars);
        break;
    }
    case EcsAstUsing:
    case EcsAstModule:
    case EcsAstAnnotation:
    case EcsAstInclude:
        break;
    }

    node->changed_mask = result;
    return result;
}

uint64_t flecs_script_visit_deps(
    const ecs_script_t *script,
    ecs_script_node_t *node,
    const ecs_script_vars_t *vars)
{
    if (!node) {
        return 0;
    }

    return flecs_script_node_visit_deps(script, node, vars);
}

#endif
