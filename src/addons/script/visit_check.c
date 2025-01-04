/**
 * @file addons/script/visit_validate.c
 * @brief Script AST validation.
 */

#include "flecs.h"

#ifdef FLECS_SCRIPT
#include "script.h"

static
int flecs_script_check_expr(
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
        .allow_unresolved_identifiers = true
    };

    ecs_assert(expr->type_info == NULL, ECS_INTERNAL_ERROR, NULL);

    if (flecs_expr_visit_type(script, expr, &desc)) {
        goto error;
    }

    if (flecs_expr_visit_fold(script, expr_ptr, &desc)) {
        goto error;
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
    int ret =  flecs_script_eval_scope(v, node);
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

static
int flecs_script_check_entity(
    ecs_script_eval_visitor_t *v,
    ecs_script_entity_t *node)
{
    if (node->kind) {
        ecs_script_id_t id = {
            .first = node->kind
        };

        if (!ecs_os_strcmp(node->kind, "prefab")) {
            id.eval = EcsPrefab;
        } else if (!ecs_os_strcmp(node->kind, "slot")) {
        } else if (flecs_script_eval_id(v, node, &id)) {
            return -1;
        }

        node->eval_kind = id.eval;
    } else {
        /* Inherit kind from parent kind's DefaultChildComponent, if it existst */
        ecs_script_scope_t *scope = ecs_script_current_scope(v);
        if (scope && scope->default_component_eval) {
            node->eval_kind = scope->default_component_eval;
        }
    }

    ecs_script_entity_t *old_entity = v->entity;
    v->entity = node;

    bool old_is_with_scope = v->is_with_scope;
    v->is_with_scope = false;

    if (ecs_script_visit_node(v, node->scope)) {
        return -1;
    }

    v->is_with_scope = old_is_with_scope;
    v->entity = old_entity;

    return 0;
}

static
int flecs_script_check_tag(
    ecs_script_eval_visitor_t *v,
    ecs_script_tag_t *node)
{
    if (flecs_script_eval_id(v, node, &node->id)) {
        return -1;
    }

    if (v->is_with_scope) {
        flecs_script_eval_error(v, node, "invalid component in with scope"); 
        return -1;
    }

    if (!v->entity) {
        if (node->id.second) {
            flecs_script_eval_error(
                v, node, "missing entity for pair (%s, %s)",
                node->id.first, node->id.second);
        } else {
            flecs_script_eval_error(v, node, "missing entity for tag %s", 
                node->id.first);
        }
        return -1;
    }

    return 0;
}

static
int flecs_script_check_component(
    ecs_script_eval_visitor_t *v,
    ecs_script_component_t *node)
{
    if (flecs_script_eval_id(v, node, &node->id)) {
        return -1;
    }

    if (!v->entity) {
        if (node->id.second) {
            flecs_script_eval_error(v, node, "missing entity for pair (%s, %s)",
                node->id.first, node->id.second);
        } else {
            flecs_script_eval_error(v, node, "missing entity for component %s", 
                node->id.first);
        }
        return -1;
    }

    if (v->is_with_scope) {
        flecs_script_eval_error(v, node, "invalid component in with scope"); 
        return -1;
    }

    if (node->expr) {
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
                break;
            case EcsArrayType:
            case EcsVectorType:
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

static
int flecs_script_check_var_component(
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

static
int flecs_script_check_default_component(
    ecs_script_eval_visitor_t *v,
    ecs_script_default_component_t *node)
{
    if (!v->entity) {
        flecs_script_eval_error(v, node, 
            "missing entity for default component");
        return -1;
    }

    return 0;
}

static
int flecs_script_check_with_var(
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

static
int flecs_script_check_with_tag(
    ecs_script_eval_visitor_t *v,
    ecs_script_tag_t *node)
{
    if (flecs_script_eval_id(v, node, &node->id)) {
        return -1;
    }

    return 0;
}

static
int flecs_script_check_with_component(
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

static
int flecs_script_check_with(
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

static
int flecs_script_check_using(
    ecs_script_eval_visitor_t *v,
    ecs_script_using_t *node)
{
    return flecs_script_eval_using(v, node);
}

static
int flecs_script_check_const(
    ecs_script_eval_visitor_t *v,
    ecs_script_var_node_t *node)
{
    return flecs_script_eval_const(v, node);
}

static
int flecs_script_check_pair_scope(
    ecs_script_eval_visitor_t *v,
    ecs_script_pair_scope_t *node)
{
    ecs_entity_t dummy;

    if (flecs_script_find_entity(
        v, 0, node->id.first, &node->id.first_sp, &dummy))
    {
        return -1;
    }

    if (flecs_script_find_entity(
        v, 0, node->id.second, &node->id.second_sp, &dummy))
    {
        return -1;
    }

    if (ecs_script_visit_scope(v, node->scope)) {
        return -1;
    }

    return 0;
}

static
int flecs_script_check_if(
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

static
int flecs_script_check_for_range(
    ecs_script_eval_visitor_t *v,
    ecs_script_for_range_t *node)
{
    ecs_entity_t type = ecs_id(ecs_i32_t);
    if (flecs_script_check_expr(v, &node->from, &type)) {
        return -1;
    }

    type = ecs_id(ecs_i32_t);
    if (flecs_script_check_expr(v, &node->to, &type)) {
        return -1;
    }

    v->vars = flecs_script_vars_push(v->vars, &v->r->stack, &v->r->allocator);

    ecs_script_var_t *var = ecs_script_vars_declare(v->vars, node->loop_var);
    const ecs_type_info_t *ti = ecs_get_type_info(v->world, ecs_id(ecs_i32_t));
    int32_t dummy = 0;
    var->value.ptr = &dummy;
    var->value.type = ecs_id(ecs_i32_t);
    var->type_info = ti;

    if (flecs_script_eval_scope(v, node->scope)) {
        return -1;
    }

    var->value.ptr = NULL;

    v->vars = ecs_script_vars_pop(v->vars);

    return 0;
}

static
int flecs_script_check_annot(
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
    ecs_script_eval_visitor_t *v,
    ecs_script_node_t *node)
{
    switch(node->kind) {
    case EcsAstScope:
        return flecs_script_check_scope(
            v, (ecs_script_scope_t*)node);
    case EcsAstTag:
        return flecs_script_check_tag(
            v, (ecs_script_tag_t*)node);
    case EcsAstComponent:
        return flecs_script_check_component(
            v, (ecs_script_component_t*)node);
    case EcsAstVarComponent:
        return flecs_script_check_var_component(
            v, (ecs_script_var_component_t*)node);
    case EcsAstDefaultComponent:
        return flecs_script_check_default_component(
            v, (ecs_script_default_component_t*)node);
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
        return 0;
    case EcsAstConst:
        return flecs_script_check_const(
            v, (ecs_script_var_node_t*)node);
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
        return flecs_script_check_for_range(
            v, (ecs_script_for_range_t*)node);
    }

    ecs_abort(ECS_INTERNAL_ERROR, "corrupt AST node kind");
}

int flecs_script_check(
    const ecs_script_t *script,
    const ecs_script_eval_desc_t *desc)
{
    ecs_script_eval_visitor_t v;
    ecs_script_impl_t *impl = flecs_script_impl(
        /* Safe, script will only be used for reading by visitor */
        ECS_CONST_CAST(ecs_script_t*, script));

    ecs_script_eval_desc_t priv_desc = {0};
    if (desc) {
        priv_desc = *desc;
    }

    if (!priv_desc.runtime) {
        priv_desc.runtime = flecs_script_runtime_get(script->world);
    }

    flecs_script_eval_visit_init(impl, &v, &priv_desc);
    int result = ecs_script_visit(impl, &v, flecs_script_check_node);
    flecs_script_eval_visit_fini(&v, &priv_desc);

    return result;
}

#endif
