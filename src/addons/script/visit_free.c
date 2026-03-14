/**
 * @file addons/script/visit_free.c
 * @brief Script free visitor (frees AST resources).
 */

#include "flecs.h"

#ifdef FLECS_SCRIPT
#include "script.h"

/* Free a scope node and its children recursively. */
static
void flecs_script_scope_free(
    ecs_script_visit_t *v,
    ecs_script_scope_t *node)
{
    ecs_script_visit_scope(v, node);
    ecs_vec_fini_t(&v->script->allocator, &node->stmts, ecs_script_node_t*);
    ecs_vec_fini_t(&v->script->allocator, &node->components, ecs_id_t);
    flecs_free_t(&v->script->allocator, ecs_script_scope_t, node);
}

/* Free expression nodes owned by a script identifier. */
static
void flecs_script_id_free(
    ecs_script_visit_t *v,
    ecs_script_id_t *id)
{
    if (id->first_expr) {
        flecs_expr_visit_free(&v->script->pub, id->first_expr);
    }
    if (id->second_expr) {
        flecs_expr_visit_free(&v->script->pub, id->second_expr);
    }
}

/* Free a with node's expression and scope children. */
static
void flecs_script_with_free(
    ecs_script_visit_t *v,
    ecs_script_with_t *node)
{
    flecs_script_scope_free(v, node->expressions);
    flecs_script_scope_free(v, node->scope);
}

/* Free a template node's scope. */
static
void flecs_script_template_free(
    ecs_script_visit_t *v,
    ecs_script_template_node_t *node)
{
    flecs_script_scope_free(v, node->scope);
}

/* Free an entity node's scope and name expression. */
static
void flecs_script_entity_free(
    ecs_script_visit_t *v,
    ecs_script_entity_t *node)
{
    flecs_script_scope_free(v, node->scope);
    if (node->name_expr) {
        flecs_expr_visit_free(&v->script->pub, node->name_expr);
    }
}

/* Free a pair scope node's scope and id expressions. */
static
void flecs_script_pair_scope_free(
    ecs_script_visit_t *v,
    ecs_script_pair_scope_t *node)
{
    flecs_script_scope_free(v, node->scope);
    flecs_script_id_free(v, &node->id);
}

/* Free an if node's branches and condition expression. */
static
void flecs_script_if_free(
    ecs_script_visit_t *v,
    ecs_script_if_t *node)
{
    flecs_script_scope_free(v, node->if_true);
    flecs_script_scope_free(v, node->if_false);
    flecs_expr_visit_free(&v->script->pub, node->expr);
}

/* Free a for-range node's bound expressions and scope. */
static
void flecs_script_for_range_free(
    ecs_script_visit_t *v,
    ecs_script_for_range_t *node)
{
    flecs_expr_visit_free(&v->script->pub, node->from);
    flecs_expr_visit_free(&v->script->pub, node->to);
    flecs_script_scope_free(v, node->scope);
}

/* Free a tag node's id expressions. */
static
void flecs_script_tag_free(
    ecs_script_visit_t *v,
    ecs_script_tag_t *node)
{
    flecs_script_id_free(v, &node->id);
}

/* Free a component node's expression and id. */
static
void flecs_script_component_free(
    ecs_script_visit_t *v,
    ecs_script_component_t *node)
{
    flecs_expr_visit_free(&v->script->pub, node->expr);
    flecs_script_id_free(v, &node->id);
}

/* Free a default component node's expression. */
static
void flecs_script_default_component_free(
    ecs_script_visit_t *v,
    ecs_script_default_component_t *node)
{
    flecs_expr_visit_free(&v->script->pub, node->expr);
}

/* Free a variable node's expression. */
static
void flecs_script_var_node_free(
    ecs_script_visit_t *v,
    ecs_script_var_node_t *node)
{
    flecs_expr_visit_free(&v->script->pub, node->expr);
}

/* Free a statement node and its resources based on its kind. */
static
int flecs_script_stmt_free(
    ecs_script_visit_t *v,
    ecs_script_node_t *node)
{
    ecs_allocator_t *a = &v->script->allocator;
    switch(node->kind) {
    case EcsAstScope:
        flecs_script_scope_free(v, (ecs_script_scope_t*)node);
        break;
    case EcsAstWith:
        flecs_script_with_free(v, (ecs_script_with_t*)node);
        flecs_free_t(a, ecs_script_with_t, node);
        break;
    case EcsAstTemplate:
        flecs_script_template_free(v, (ecs_script_template_node_t*)node);
        flecs_free_t(a, ecs_script_template_node_t, node);
        break;
    case EcsAstEntity:
        flecs_script_entity_free(v, (ecs_script_entity_t*)node);
        flecs_free_t(a, ecs_script_entity_t, node);
        break;
    case EcsAstPairScope:
        flecs_script_pair_scope_free(v, (ecs_script_pair_scope_t*)node);
        flecs_free_t(a, ecs_script_pair_scope_t, node);
        break;
    case EcsAstIf:
        flecs_script_if_free(v, (ecs_script_if_t*)node);
        flecs_free_t(a, ecs_script_if_t, node);
        break;
    case EcsAstFor:
        flecs_script_for_range_free(v, (ecs_script_for_range_t*)node);
        flecs_free_t(a, ecs_script_for_range_t, node);
        break;
    case EcsAstTag:
        flecs_script_tag_free(v, (ecs_script_tag_t*)node);
        flecs_free_t(a, ecs_script_tag_t, node);
        break;
    case EcsAstComponent:
    case EcsAstWithComponent:
        flecs_script_component_free(v, (ecs_script_component_t*)node);
        flecs_free_t(a, ecs_script_component_t, node);
        break;
    case EcsAstDefaultComponent:
        flecs_script_default_component_free(v, 
            (ecs_script_default_component_t*)node);
        flecs_free_t(a, ecs_script_default_component_t, node);
        break;
    case EcsAstVarComponent:
        flecs_free_t(a, ecs_script_var_component_t, node);
        break;
    case EcsAstWithVar:
        flecs_free_t(a, ecs_script_var_component_t, node);
        break;
    case EcsAstWithTag:
        flecs_free_t(a, ecs_script_tag_t, node);
        break;
    case EcsAstUsing:
        flecs_free_t(a, ecs_script_using_t, node);
        break;
    case EcsAstModule:
        flecs_free_t(a, ecs_script_module_t, node);
        break;
    case EcsAstAnnotation:
        flecs_free_t(a, ecs_script_annot_t, node);
        break;
    case EcsAstProp:
    case EcsAstConst:
    case EcsAstExportConst:
        flecs_script_var_node_free(v, (ecs_script_var_node_t*)node);
        flecs_free_t(a, ecs_script_var_node_t, node);
        break;
    }

    return 0;
}

/* Free a single AST node and its descendants. */
int flecs_script_visit_free_node(
    ecs_script_t *script,
    ecs_script_node_t *node)
{
    ecs_check(script != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_script_impl_t *impl = flecs_script_impl(script);

    ecs_script_visit_t v = {
        .script = impl
    };

    if (ecs_script_visit_from(
        flecs_script_impl(script), &v, flecs_script_stmt_free, node, 0))
    {
        goto error;
    }

    return 0;
error:
    return -1;
}

/* Free the entire script AST. */
int flecs_script_visit_free(
    ecs_script_t *script)
{
    ecs_check(script != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_script_impl_t *impl = flecs_script_impl(script);
    if (!impl->expr && !impl->root) {
        return 0;
    }

    ecs_script_visit_t v = {
        .script = impl
    };

    if (ecs_script_visit(
        flecs_script_impl(script), &v, flecs_script_stmt_free)) 
    {
        goto error;
    }

    return 0;
error:
    return - 1;
}

#endif
