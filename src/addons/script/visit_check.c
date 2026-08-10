/**
 * @file addons/script/visit_check.c
 * @brief Script AST validation.
 */

#include "flecs.h"

#ifdef FLECS_SCRIPT
#include "script.h"

static bool flecs_script_scope_has_entity(
    ecs_script_scope_t *scope,
    const char *name)
{
    int32_t i, count = ecs_vec_count(&scope->stmts);
    ecs_script_node_t **stmts = ecs_vec_first(&scope->stmts);
    for (i = 0; i < count; i ++) {
        ecs_script_node_t *node = stmts[i];
        if (node->kind == EcsAstEntity) {
            ecs_script_entity_t *entity = (ecs_script_entity_t*)node;
            if (entity->name && !ecs_os_strcmp(entity->name, name)) {
                return true;
            }
            if (entity->scope &&
                flecs_script_scope_has_entity(entity->scope, name))
            {
                return true;
            }
        }
    }
    return false;
}

static bool flecs_script_check_unresolved_identifier(
    const ecs_world_t *world,
    const char *value,
    void *ctx)
{
    (void)world;
    ecs_script_eval_visitor_t *v = ctx;
    if (!v->template) {
        return true;
    }
    return flecs_script_scope_has_entity(v->template->node->scope, value);
}

static int flecs_script_check_expr(
    ecs_script_eval_visitor_t *v,
    ecs_expr_node_t **expr_ptr,
    ecs_entity_t *type)
{
    ecs_expr_node_t *expr = *expr_ptr;
    ecs_script_impl_t *impl = v->base.script;
    ecs_script_t *script = &impl->pub;

    ecs_expr_eval_desc_t desc = {
        .name = script->name,
        .lookup_action = flecs_script_find_entity_action,
        .lookup_ctx = v,
        .vars = v->vars,
        .type = type ? type[0] : 0,
        .runtime = v->r,
        .allow_unresolved_identifiers = true,
        .unresolved_identifier_action = flecs_script_check_unresolved_identifier,
        .script_visitor = v
    };

    ecs_assert(expr->type_info == NULL, ECS_INTERNAL_ERROR, NULL);

    if (flecs_expr_visit_type(script, expr_ptr, &desc)) {
        goto error;
    }

    if (flecs_expr_visit_fold(script, expr_ptr, &desc)) {
        goto error;
    }

    /* Collect statically known component references used in a template body so
     * a single observer per reference can trigger reevaluation of instances. */
    if (v->template) {
        if (flecs_expr_visit_refs(script, *expr_ptr, &v->template->refs,
            &v->template->dynamic_refs, &impl->refs))
        {
            goto error;
        }
    }

    if (type) {
        type[0] = expr_ptr[0]->type;
    }

    return 0;
error:
    return -1;
}

int flecs_script_check_scope(
    ecs_script_eval_visitor_t *v,
    ecs_script_scope_t *node)
{
    int ret = flecs_script_eval_scope(v, node);
    if (ret) {
        return -1;
    }

    /* Gather all resolved components in scope so we can add them in one bulk
     * operation to entities. */
    ecs_allocator_t *a = &v->base.script->allocator;
    int32_t i, count = ecs_vec_count(&node->stmts);
    ecs_script_node_t **stmts = ecs_vec_first(&node->stmts);
    for (i = 0; i < count; i ++) {
        ecs_script_node_t *stmt = stmts[i];
        ecs_id_t id = 0;
        if (stmt->kind == EcsAstComponent) {
            ecs_script_component_t *cmp = (ecs_script_component_t*)stmt;
            id = cmp->id.eval;
        } else if (stmt->kind == EcsAstTag) {
            ecs_script_tag_t *cmp = (ecs_script_tag_t*)stmt;
            id = cmp->id.eval;
        }

        if (id) {
            ecs_vec_append_t(a, &node->components, ecs_id_t)[0] = id;
        }
    }

    return 0;
}

static int flecs_script_check_entity(
    ecs_script_eval_visitor_t *v,
    ecs_script_entity_t *node)
{
    ecs_entity_t eval_kind = 0;

    if (node->kind) {
        ecs_script_id_t id = {
            .first = node->kind
        };

        if (!ecs_os_strcmp(node->kind, "prefab")) {
#ifdef FLECS_PREFAB
            id.eval = EcsPrefab;
#else
            flecs_script_eval_error(v, node, "prefabs are unsupported in this "
                "flecs build, enable FLECS_PREFAB addon");
#endif
        } else if (flecs_script_eval_id(v, node, &id)) {
            return -1;
        }

        eval_kind = id.eval;
    }

    if (node->name_expr && !node->name_expr->type_info) {
        ecs_entity_t type = ecs_id(ecs_string_t);
        if (flecs_script_check_expr(v, &node->name_expr, &type)) {
            return -1;
        }
    }

    flecs_script_entity_state_t state = {
        .node = node,
        .eval_kind = eval_kind
    };

    flecs_script_entity_state_t *old_entity = v->entity;
    v->entity = &state;

    bool old_is_with_scope = v->is_with_scope;
    v->is_with_scope = false;

    int ret = flecs_script_check_scope(v, node->scope);
    if (ret) {
        v->entity = old_entity;
        return -1;
    }

    v->is_with_scope = old_is_with_scope;
    v->entity = old_entity;

    return 0;
}

static int flecs_script_check_tag(
    ecs_script_eval_visitor_t *v,
    ecs_script_tag_t *node)
{
    if (flecs_script_eval_id(v, node, &node->id)) {
        return -1;
    }

    if (v->is_with_scope) {
        flecs_script_eval_error(v, node, "invalid tag in with scope");
        return -1;
    }

    return 0;
}

static int flecs_script_check_component(
    ecs_script_eval_visitor_t *v,
    ecs_script_component_t *node)
{
    if (flecs_script_eval_id(v, node, &node->id)) {
        return -1;
    }

    if (v->is_with_scope) {
        flecs_script_eval_error(v, node, "invalid component in with scope");
        return -1;
    }

    /* If the id has a script visitor the initializer AST is passed to the
     * visitor as-is during evaluation, don't type check it. */
    if (!node->id.second && node->id.eval &&
        ecs_has(v->world, node->id.eval, EcsScriptVisitor))
    {
        return 0;
    }

    if (node->expr) {
        if (!node->id.eval) {
            return 0;
        }

        const ecs_type_info_t *ti = ecs_get_type_info(v->world, node->id.eval);
        if (!ti) {
            return 0;
        }

        const EcsType *type = ecs_get(v->world, ti->component, EcsType);
        if (type) {
            bool is_collection = false;

            switch(type->kind) {
            case EcsPrimitiveType:
            case EcsBitmaskType:
            case EcsEnumType:
            case EcsStructType:
            case EcsOpaqueType:
            case EcsValueType:
                break;
            case EcsArrayType:
            case EcsVectorType:
            case EcsMapType:
                is_collection = true;
                break;
            }

            if (node->is_collection != is_collection) {
                char *id_str = ecs_id_str(v->world, ti->component);
                if (node->is_collection && !is_collection) {
                    flecs_script_eval_error(v, node, 
                        "type %s is not a collection (use '%s: {...}')", 
                            id_str, id_str);
                } else {
                    flecs_script_eval_error(v, node, 
                        "type %s is a collection (use '%s: [...]')", 
                            id_str, id_str);
                }
                ecs_os_free(id_str);
                return -1;
            }
        }

        ecs_entity_t expr_type = ti->component;
        if (flecs_script_check_expr(v, &node->expr, &expr_type)) {
            return -1;
        }
    }

    return 0;
}

static int flecs_script_check_var_component(
    ecs_script_eval_visitor_t *v,
    ecs_script_var_component_t *node)
{
    ecs_script_var_t *var = ecs_script_vars_lookup(v->vars, node->name);
    if (!var) {
        flecs_script_eval_error(v, node, 
            "unresolved variable '%s'", node->name);
        return -1;
    }

    node->sp = var->sp;

    return 0;
}

static int flecs_script_check_with_var(
    ecs_script_eval_visitor_t *v,
    ecs_script_var_component_t *node)
{
    ecs_script_var_t *var = ecs_script_vars_lookup(v->vars, node->name);
    if (!var) {
        flecs_script_eval_error(v, node, 
            "unresolved variable '%s'", node->name);
        return -1;
    }

    node->sp = var->sp;

    return 0;
}

static int flecs_script_check_with_tag(
    ecs_script_eval_visitor_t *v,
    ecs_script_tag_t *node)
{
    if (flecs_script_eval_id(v, node, &node->id)) {
        return -1;
    }

    return 0;
}

static int flecs_script_check_with_component(
    ecs_script_eval_visitor_t *v,
    ecs_script_component_t *node)
{
    if (flecs_script_eval_id(v, node, &node->id)) {
        return -1;
    }

    if (node->expr) {
        ecs_entity_t type = node->id.eval;

        if (flecs_script_check_expr(v, &node->expr, &type)) {
            return -1;
        }
    }

    return 0;
}

static int flecs_script_check_with(
    ecs_script_eval_visitor_t *v,
    ecs_script_with_t *node)
{
    if (ecs_script_visit_scope(v, ((ecs_script_with_t*)node)->expressions)) {
        return -1;
    }

    bool old_is_with_scope = v->is_with_scope;
    v->is_with_scope = true;

    if (ecs_script_visit_scope(v, ((ecs_script_with_t*)node)->scope)) {
        return -1;
    }

    v->is_with_scope = old_is_with_scope;

    return 0;
}

static int flecs_script_check_using(
    ecs_script_eval_visitor_t *v,
    ecs_script_using_t *node)
{
    flecs_script_eval_error(v, node, "using in template is not allowed");
    return -1;
}

static int flecs_script_check_const(
    ecs_script_eval_visitor_t *v,
    ecs_script_var_node_t *node)
{
    return flecs_script_eval_const(v, node, false);
}

static int flecs_script_check_pair_scope(
    ecs_script_eval_visitor_t *v,
    ecs_script_pair_scope_t *node)
{
    ecs_entity_t dummy;

    if (flecs_script_find_entity(
        v, 0, node->id.first, NULL, &node->id.first_sp, &dummy, NULL))
    {
        return -1;
    }

    if (flecs_script_find_entity(
        v, 0, node->id.second, NULL, &node->id.second_sp, &dummy, NULL))
    {
        return -1;
    }

    if (ecs_script_visit_scope(v, node->scope)) {
        return -1;
    }

    return 0;
}

static int flecs_script_check_if(
    ecs_script_eval_visitor_t *v,
    ecs_script_if_t *node)
{
    if (flecs_script_check_expr(v, &node->expr, NULL)) {
        return -1;
    }

    if (flecs_script_check_scope(v, node->if_true)) {
        return -1;
    }

    if (flecs_script_check_scope(v, node->if_false)) {
        return -1;
    }

    return 0;
}

static int flecs_script_check_for(
    ecs_script_eval_visitor_t *v,
    ecs_script_for_t *node)
{
    flecs_script_for_kind_t kind = FlecsScriptForRange;
    ecs_entity_t key_type = 0;
    ecs_entity_t elem_type = ecs_id(ecs_i32_t);

    if (!node->expr) {
        ecs_entity_t type = ecs_id(ecs_i32_t);
        if (flecs_script_check_expr(v, &node->from, &type)) {
            return -1;
        }

        type = ecs_id(ecs_i32_t);
        if (flecs_script_check_expr(v, &node->to, &type)) {
            return -1;
        }
    } else {
        ecs_entity_t type = 0;
        if (flecs_script_check_expr(v, &node->expr, &type)) {
            return -1;
        }

        if (flecs_script_for_collection_kind(
            v, node, type, &kind, &key_type, &elem_type))
        {
            return -1;
        }
    }

    if (flecs_script_for_check_var_count(v, node, kind)) {
        return -1;
    }

    v->vars = flecs_script_vars_push(v->vars, &v->r->stack, &v->r->allocator);

    int result = -1;
    int32_t var_i = 0;

    if (kind == FlecsScriptForMap && node->loop_var_count >= 2) {
        if (!flecs_script_for_declare_var(
            v, node, node->loop_vars[var_i ++], key_type, true))
        {
            goto done;
        }
    }

    if (flecs_script_for_has_index_var(kind, node->loop_var_count)) {
        if (!flecs_script_for_declare_var(
            v, node, node->loop_vars[var_i ++], ecs_id(ecs_i32_t), true))
        {
            goto done;
        }
    }

    if (!flecs_script_for_declare_var(
        v, node, node->loop_vars[var_i], elem_type, true))
    {
        goto done;
    }

    result = flecs_script_eval_scope(v, node->scope);
done:
    v->vars = ecs_script_vars_pop(v->vars);
    return result;
}

static int flecs_script_check_annot(
    ecs_script_eval_visitor_t *v,
    ecs_script_annot_t *node)
{
    if (!v->base.next) {
        flecs_script_eval_error(v, node,
            "annotation '%s' is not applied to anything", node->name);
        return -1;
    }

    ecs_script_node_kind_t kind = v->base.next->kind;
    if (kind != EcsAstEntity && kind != EcsAstAnnotation) {
        flecs_script_eval_error(v, node,
            "annotation must be applied to an entity");
        return -1;
    }

    return 0;
}

int flecs_script_check_node(
    ecs_script_visit_t *_v,
    ecs_script_node_t *node)
{
    ecs_script_eval_visitor_t *v = (ecs_script_eval_visitor_t*)_v;
    switch(node->kind) {
    case EcsAstAwait:
    case EcsAstTry:
        return 0;
    case EcsAstScope:
        return flecs_script_check_scope(v, (ecs_script_scope_t*)node);
    case EcsAstTag:
        return flecs_script_check_tag(
            v, (ecs_script_tag_t*)node);
    case EcsAstComponent:
        return flecs_script_check_component(
            v, (ecs_script_component_t*)node);
    case EcsAstVarComponent:
        return flecs_script_check_var_component(
            v, (ecs_script_var_component_t*)node);
    case EcsAstWithVar:
        return flecs_script_check_with_var(
            v, (ecs_script_var_component_t*)node);
    case EcsAstWithTag:
        return flecs_script_check_with_tag(
            v, (ecs_script_tag_t*)node);
    case EcsAstWithComponent:
        return flecs_script_check_with_component(
            v, (ecs_script_component_t*)node);
    case EcsAstWith:
        return flecs_script_check_with(
            v, (ecs_script_with_t*)node);
    case EcsAstUsing:
        return flecs_script_check_using(
            v, (ecs_script_using_t*)node);
    case EcsAstModule:
        return 0;
    case EcsAstAnnotation:
        return flecs_script_check_annot(
            v, (ecs_script_annot_t*)node);
    case EcsAstTemplate:
        return 0;
    case EcsAstProp:
    case EcsAstMut:
        return 0;
    case EcsAstConst:
        return flecs_script_check_const(
            v, (ecs_script_var_node_t*)node);
    case EcsAstExportConst:
    case EcsAstExportMut:
        return 0;
    case EcsAstEntity:
        return flecs_script_check_entity(
            v, (ecs_script_entity_t*)node);
    case EcsAstPairScope:
        return flecs_script_check_pair_scope(
            v, (ecs_script_pair_scope_t*)node);
    case EcsAstIf:
        return flecs_script_check_if(
            v, (ecs_script_if_t*)node);
    case EcsAstFor:
        return flecs_script_check_for(
            v, (ecs_script_for_t*)node);
    case EcsAstInclude:
        flecs_script_eval_error(v, node,
            "include is not allowed in template");
        return -1;
    case EcsAstFunction:
        flecs_script_eval_error(v, node,
            "fn is not allowed in template");
        return -1;
    }

    ecs_abort(ECS_INTERNAL_ERROR, "corrupt AST node kind");
}

#endif
