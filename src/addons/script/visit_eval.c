/**
 * @file addons/script/visit_eval.c
 * @brief Script evaluation visitor.
 */

#include "flecs.h"

#ifdef FLECS_SCRIPT
#include "script.h"

void flecs_script_eval_error_(
    ecs_script_eval_visitor_t *v,
    ecs_script_node_t *node,
    const char *fmt,
    ...)
{
    va_list args;
    va_start(args, fmt);
    char *msg = flecs_vasprintf(fmt, args);
    va_end(args);

    if (node) {
        int32_t line = ecs_script_node_line_number(v->base.script, node);
        ecs_parser_error(v->base.script->pub.name, NULL, 0, "%d: %s", line, msg);
    } else {
        ecs_parser_error(v->base.script->pub.name, NULL, 0, "%s", msg);
    }

    ecs_os_free(msg);
}

static
ecs_value_t* flecs_script_with_append(
    ecs_allocator_t *a,
    ecs_script_eval_visitor_t *v,
    const ecs_type_info_t *ti)
{
    if (ecs_vec_count(&v->r->with)) {
        ecs_assert(ecs_vec_last_t(&v->r->with, ecs_value_t)->type == 0, 
            ECS_INTERNAL_ERROR, NULL);
        ecs_assert(ecs_vec_last_t(&v->r->with, ecs_value_t)->ptr == NULL, 
            ECS_INTERNAL_ERROR, NULL);
        ecs_vec_remove_last(&v->r->with);
    }

    ecs_vec_append_t(a, &v->r->with_type_info, const ecs_type_info_t*)[0] = ti;

    ecs_vec_append_t(a, &v->r->with, ecs_value_t);
    ecs_value_t *last = ecs_vec_append_t(a, &v->r->with, ecs_value_t);
    ecs_os_memset_t(last, 0, ecs_value_t);
    return ecs_vec_get_t(&v->r->with, ecs_value_t, ecs_vec_count(&v->r->with) - 2);
}

static
void flecs_script_with_set_count(
    ecs_allocator_t *a,
    ecs_script_eval_visitor_t *v,
    int32_t count)
{
    int32_t i = count, until = ecs_vec_count(&v->r->with) - 1;
    for (; i < until; i ++) {
        ecs_value_t *val = ecs_vec_get_t(&v->r->with, ecs_value_t, i);
        ecs_type_info_t *ti = ecs_vec_get_t(
            &v->r->with_type_info, ecs_type_info_t*, i)[0];
        if (ti) {
            if (ti->hooks.dtor) {
                ti->hooks.dtor(val->ptr, 1, ti);
            }
            flecs_stack_free(val->ptr, ti->size);
        }
    }

    if (count) {
        ecs_value_t *last = ecs_vec_get_t(&v->r->with, ecs_value_t, count);
        ecs_os_memset_t(last, 0, ecs_value_t);
        ecs_vec_set_count_t(a, &v->r->with, ecs_value_t, count + 1);
    } else {
        ecs_vec_set_count_t(a, &v->r->with, ecs_value_t, 0);
    }

    ecs_vec_set_count_t(a, &v->r->with_type_info, ecs_type_info_t*, count);
}

static
ecs_value_t* flecs_script_with_last(
    ecs_script_eval_visitor_t *v)
{
    int32_t count = ecs_vec_count(&v->r->with);
    if (count) {
        return ecs_vec_get_t(&v->r->with, ecs_value_t, count - 2);
    }
    return NULL;
}

static
int32_t flecs_script_with_count(
    ecs_script_eval_visitor_t *v)
{
    if (ecs_vec_count(&v->r->with)) {
        ecs_assert(ecs_vec_last_t(&v->r->with, ecs_value_t)->type == 0, 
            ECS_INTERNAL_ERROR, NULL);
        ecs_assert(ecs_vec_last_t(&v->r->with, ecs_value_t)->ptr == NULL, 
            ECS_INTERNAL_ERROR, NULL);
        return ecs_vec_count(&v->r->with) - 1;
    }
    return 0;
}

const ecs_type_info_t* flecs_script_get_type_info(
    ecs_script_eval_visitor_t *v,
    void *node,
    ecs_id_t id)
{
    ecs_component_record_t *cr = flecs_components_ensure(v->world, id);
    if (!cr) {
        goto error;
    }

    if (!cr->type_info) {
        goto error;
    }

    return cr->type_info;
error:
    {
        char *idstr = ecs_id_str(v->world, id);
        flecs_script_eval_error(v, node, 
            "cannot set value of '%s': not a component", idstr);
        ecs_os_free(idstr);
    }
    return NULL;
}

ecs_script_var_t* flecs_script_find_var(
    const ecs_script_vars_t *vars,
    const char *name,
    int32_t *sp)
{
    if (sp && sp[0] != -1) {
        return ecs_script_vars_from_sp(vars, sp[0]);
    } else {
        ecs_script_var_t *var = ecs_script_vars_lookup(vars, name);
        if (var && sp) {
            sp[0] = var->sp;
        }
        return var;
    }
}

static
ecs_entity_t flecs_script_eval_name_expr(
    ecs_script_eval_visitor_t *v,
    ecs_script_entity_t *node,
    ecs_expr_node_t **expr_ptr,
    bool find)
{
    ecs_script_t *script = &v->base.script->pub;
    ecs_expr_eval_desc_t desc = {
        .name = script->name,
        .lookup_action = flecs_script_find_entity_action,
        .lookup_ctx = v,
        .vars = v->vars,
        .type = ecs_id(ecs_string_t),
        .runtime = v->r
    };

    ecs_expr_node_t *expr = *expr_ptr;

    if (!expr->type_info) {
        if (flecs_expr_visit_type(script, expr, &desc)) {
            return 0;
        }

        if (flecs_expr_visit_fold(script, expr_ptr, &desc)) {
            return 0;
        }

        expr = *expr_ptr;
    }

    ecs_value_t value = { .type = ecs_id(ecs_string_t) };
    if (flecs_expr_visit_eval(script, expr, &desc, &value)) {
        return 0;
    }

    char *name = *(char**)value.ptr;
    if (!name) {
        flecs_script_eval_error(v, node, "failed to evaluate entity name");
        return 0;
    }

    ecs_entity_t result;
    if (!find) {
        result = flecs_script_create_entity(v, name);
    } else {
        if (flecs_script_find_entity(v, 0, name, NULL, NULL, &result, NULL)) {
            return 0;
        }
    }

    ecs_value_free(script->world, value.type, value.ptr);

    return result;
}

int flecs_script_find_entity(
    ecs_script_eval_visitor_t *v,
    ecs_entity_t from,
    const char *path,
    ecs_expr_node_t **name_expr,
    int32_t *sp,
    ecs_entity_t *out,
    bool *is_var)
{
    if (!path) {
        goto error;
    }

    ecs_entity_t result = 0;

    if (path[0] != '$') {
        if (name_expr && *name_expr) {
            result = flecs_script_eval_name_expr(v, NULL, name_expr, true);
            if (!result) {
                return -1;
            }
        } else if (from) {
            result = ecs_lookup_path_w_sep(
                v->world, from, path, NULL, NULL, false);
        } else {
            int32_t i, using_count = ecs_vec_count(&v->r->using);
            if (using_count) {
                ecs_entity_t *using = ecs_vec_first(&v->r->using);
                for (i = using_count - 1; i >= 0; i --) {
                    ecs_entity_t e = ecs_lookup_path_w_sep(
                        v->world, using[i], path, NULL, NULL, false);
                    if (e) {
                        result = e;
                    }
                }
            }

            if (!result) {
                result = ecs_lookup_path_w_sep(
                    v->world, v->parent, path, NULL, NULL, true);
            }
        }

        if (result) {
            *out = result;
            return 0;
        }

        if (!sp) {
            return -1;
        }
    } else {
        if (!sp) {
            flecs_script_eval_error(v, NULL, 
                "variable identifier '%s' not allowed here", path);
            goto error;
        }

        path ++;
    }

    const ecs_script_var_t *var = flecs_script_find_var(
        v->vars, path, v->dynamic_variable_binding ? NULL : sp);
    if (!var) {
        goto error;
    }

    if (var->value.type != ecs_id(ecs_entity_t)) {
        char *type_str = ecs_id_str(v->world, var->value.type);
        flecs_script_eval_error(v, NULL, 
            "variable '%s' must be of type entity, got '%s'", 
                path, type_str);
        ecs_os_free(type_str);
        goto error;
    }

    if (v->template) {
        return 0;
    }

    if (var->value.ptr == NULL) {            
        flecs_script_eval_error(v, NULL, 
            "variable '%s' is not initialized", path);
        goto error;
    }

    result = *(ecs_entity_t*)var->value.ptr;
    if (!result) {
        flecs_script_eval_error(v, NULL, 
            "variable '%s' contains invalid entity id (0)", path);
        goto error;
    }

    *out = result;
    if (is_var) *is_var = true;

    return 0;
error:
    return -1;
}

ecs_entity_t flecs_script_create_entity(
    ecs_script_eval_visitor_t *v,
    const char *name)
{
    ecs_value_t *with = NULL;
    if (flecs_script_with_count(v)) {
        with = ecs_vec_first_t(&v->r->with, ecs_value_t);
    }

    ecs_entity_desc_t desc = {0};

    if (v->entity && v->entity->non_fragmenting_parent) {
        desc.id = ecs_new_w_parent(v->world, v->parent, name);
    } else {
        desc.parent = v->parent;
        desc.name = name;
    }

    desc.set = with;
    return ecs_entity_init(v->world, &desc);
}

ecs_entity_t flecs_script_find_entity_action(
    const ecs_world_t *world,
    const char *path,
    void *ctx)
{
    (void)world;
    ecs_script_eval_visitor_t *v = ctx;
    ecs_entity_t result;
    if (!flecs_script_find_entity(v, 0, path, NULL, NULL, &result, NULL)) {
        return result;
    }
    return 0;
}

static
int flecs_script_find_template_entity(
    ecs_script_eval_visitor_t *v,
    void *node,
    const char *name)
{
    ecs_assert(name != NULL, ECS_INTERNAL_ERROR, NULL);

    /* Loop template scope to see if it declares an entity with requested name */
    ecs_script_template_t *t = v->template;
    ecs_assert(t != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(t->node != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_script_scope_t *scope = t->node->scope;
    ecs_script_node_t **nodes = ecs_vec_first_t(
        &scope->stmts, ecs_script_node_t*);

    int32_t i, count = ecs_vec_count(&scope->stmts);
    for (i = 0; i < count; i ++) {
        ecs_script_node_t *elem = nodes[i];
        if (elem->kind == EcsAstEntity) {
            ecs_script_entity_t *entity_node = (ecs_script_entity_t*)elem;
            if (!entity_node->name) {
                continue;
            }

            if (!ecs_os_strcmp(entity_node->name, name)) {
                return 0;
            }
        }
    }

    flecs_script_eval_error(v, node, "unresolved reference to '%s'", name);

    return -1;
}

int flecs_script_eval_id(
    ecs_script_eval_visitor_t *v,
    void *node,
    ecs_script_id_t *id)
{
    ecs_entity_t second_from = 0;

    if (id->eval && !id->dynamic) {
        /* Already resolved */
        return 0;
    }

    if (!id->first) {
        flecs_script_eval_error(v, node, 
            "invalid component/tag identifier");
        return -1;
    }

    if (v->template) {
        /* Can't resolve variables while preprocessing template scope */
        if (id->first[0] == '$') {
            if (flecs_script_find_var(v->vars, &id->first[1], 
                v->dynamic_variable_binding ? NULL : &id->first_sp)) 
            {
                return 0;
            } else {
                flecs_script_eval_error(v, node, 
                    "unresolved variable '%s'", &id->first[1]);
                return -1;
            }
        }
        if (id->second && id->second[0] == '$') {
            if (flecs_script_find_var(v->vars, &id->second[1], 
                v->dynamic_variable_binding ? NULL : &id->second_sp))
            {
                return 0;
            } else {
                flecs_script_eval_error(v, node, 
                    "unresolved variable '%s'", &id->second[1]);
                return -1;
            }
        }
    }

    ecs_entity_t first = 0;
    if (flecs_script_find_entity(
        v, 0, id->first, &id->first_expr, &id->first_sp, &first, NULL) || !first)
    {
        if (id->first[0] == '$') {
            flecs_script_eval_error(v, node, 
                "unresolved variable '%s'", id->first);
            return -1;
        }

        flecs_script_eval_error(v, node,
            "unresolved identifier '%s'", id->first);

        return -1;
    } else if (id->second) {
        second_from = flecs_get_oneof(v->world, first);
    }

    if (id->second) {
        ecs_entity_t second = 0;
        if (flecs_script_find_entity(
            v, second_from, id->second, &id->second_expr, &id->second_sp, 
            &second, &id->dynamic) || !second) 
        {
            if (id->second[0] == '$') {
                flecs_script_eval_error(v, node, 
                    "unresolved variable '%s'", id->second);
                return -1;
            }

            /* Targets may be defined by the template */
            if (v->template) {
                if (!flecs_script_find_template_entity(v, node, id->second)) {
                    id->dynamic = true;
                    return 0;
                } else {
                    return -1;
                }
            }

            if (second_from) {
                char *parent_str = ecs_id_str(v->world, second_from);
                flecs_script_eval_error(v, node, "target '%s' not found in "
                    "parent '%s'", id->second, parent_str);
                ecs_os_free(parent_str);
                return -1;
            }

            flecs_script_eval_error(v, node,
                "unresolved identifier '%s'", id->second);

            return -1;
        }

        if (first == EcsAny || second == EcsAny) {
            flecs_script_eval_error(v, node,
                "cannot use anonymous entity as element of pair");
            return -1;
        }

        id->eval = id->flag | ecs_pair(first, second);
    } else {
        if (first == EcsAny) {
            flecs_script_eval_error(v, node,
                "cannot use anonymous entity as component or tag");
            return -1;
        }

        id->eval = id->flag | first;
    }

    return 0;
}

int flecs_script_eval_expr(
    ecs_script_eval_visitor_t *v,
    ecs_expr_node_t **expr_ptr,
    ecs_value_t *value)
{
    ecs_expr_node_t *expr = *expr_ptr;
    ecs_assert(expr != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_script_impl_t *impl = v->base.script;
    ecs_script_t *script = &impl->pub;

    ecs_expr_eval_desc_t desc = {
        .name = script->name,
        .lookup_action = flecs_script_find_entity_action,
        .lookup_ctx = v,
        .vars = v->vars,
        .type = value->type,
        .runtime = v->r,
        .disable_dynamic_variable_binding = !v->dynamic_variable_binding,
        .script_visitor = v
    };

    if (expr->type_info == NULL) {
        if (flecs_expr_visit_type(script, expr, &desc)) {
            goto error;
        }
        if (flecs_expr_visit_fold(script, expr_ptr, &desc)) {
            goto error;
        }
    }

    if (flecs_expr_visit_eval(script, *expr_ptr, &desc, value)) {
        goto error;
    }

    return 0;
error:
    return -1;
}

int flecs_script_eval_scope(
    ecs_script_eval_visitor_t *v,
    ecs_script_scope_t *node)
{
    ecs_script_node_t *scope_parent = ecs_script_parent_node(v);
    ecs_entity_t prev_eval_parent = v->parent;
    int32_t prev_using_count = ecs_vec_count(&v->r->using);

    for (int i = v->base.depth - 2; i >= 0; i --) {
        if (v->base.nodes[i]->kind == EcsAstScope) {
            node->parent = (ecs_script_scope_t*)v->base.nodes[i];
            break;
        }
    }

    ecs_allocator_t *a = &v->r->allocator;
    v->vars = flecs_script_vars_push(v->vars, &v->r->stack, a);

    if (scope_parent && (scope_parent->kind == EcsAstEntity)) {
        if (!v->template) {
            v->parent = ecs_script_node(entity, scope_parent)->eval;
        }
    }

    if (v->entity) {
        ecs_entity_t src = v->entity->eval;
        int32_t count = ecs_vec_count(&node->components);
        if (src != EcsVariable && count) {
            flecs_add_ids(
                v->world, src, ecs_vec_first(&node->components), count);
        }
    }

    int result = ecs_script_visit_scope(v, node);

    ecs_vec_set_count_t(a, &v->r->using, ecs_entity_t, prev_using_count);
    v->vars = ecs_script_vars_pop(v->vars);
    v->parent = prev_eval_parent;

    return result;
}

static
void flecs_script_apply_non_fragmenting_childof(
    ecs_world_t *world,
    ecs_script_entity_t *node,
    bool enabled);

static
void flecs_script_apply_non_fragmenting_childof_to_scope(
    ecs_world_t *world,
    ecs_script_scope_t *scope,
    bool enabled)
{
    if (!scope) {
        return;
    }

    int32_t i, count = ecs_vec_count(&scope->stmts);
    ecs_script_node_t **stmts = ecs_vec_first(&scope->stmts);
    for (i = 0; i < count; i ++) {
        ecs_script_node_t *stmt = stmts[i];
        switch(stmt->kind) {
        case EcsAstScope:
            flecs_script_apply_non_fragmenting_childof_to_scope(
                world, (ecs_script_scope_t*)stmt, enabled);
            break;
        case EcsAstEntity:
            flecs_script_apply_non_fragmenting_childof(
                world, (ecs_script_entity_t*)stmt, enabled);
            break;
        case EcsAstIf:
            flecs_script_apply_non_fragmenting_childof_to_scope(
                world, ((ecs_script_if_t*)stmt)->if_false, enabled);
            flecs_script_apply_non_fragmenting_childof_to_scope(
                world, ((ecs_script_if_t*)stmt)->if_true, enabled);
            break;
        case EcsAstFor:
            flecs_script_apply_non_fragmenting_childof_to_scope(
                world, ((ecs_script_for_range_t*)stmt)->scope, enabled);
            break;
        case EcsAstWith:
        case EcsAstWithVar:
        case EcsAstWithTag:
        case EcsAstWithComponent:
            flecs_script_apply_non_fragmenting_childof_to_scope(
                world, ((ecs_script_with_t*)stmt)->scope, enabled);
            break;
        case EcsAstPairScope:
            flecs_script_apply_non_fragmenting_childof_to_scope(
                world, ((ecs_script_pair_scope_t*)stmt)->scope, enabled);
            break;
        case EcsAstTag:
        case EcsAstComponent:
        case EcsAstDefaultComponent:
        case EcsAstVarComponent:        
        case EcsAstUsing:
        case EcsAstModule:
        case EcsAstAnnotation:
        case EcsAstTemplate:
        case EcsAstProp:
        case EcsAstConst:
        case EcsAstExportConst:
            break;
        }
    }
}

static
void flecs_script_apply_non_fragmenting_childof(
    ecs_world_t *world,
    ecs_script_entity_t *node,
    bool enabled)
{
    node->non_fragmenting_parent = enabled;

    flecs_script_apply_non_fragmenting_childof_to_scope(
        world, node->scope, enabled);
}

int flecs_script_apply_annot(
    ecs_script_eval_visitor_t *v,
    ecs_script_entity_t *node,
    ecs_script_annot_t *annot)
{
    ecs_entity_t e = node->eval;

    if (!ecs_os_strcmp(annot->name, "name")) {
        ecs_doc_set_name(v->world, e, annot->expr);
    } else
    if (!ecs_os_strcmp(annot->name, "brief")) {
        ecs_doc_set_brief(v->world, e, annot->expr);
    } else 
    if (!ecs_os_strcmp(annot->name, "detail")) {
        ecs_doc_set_detail(v->world, e, annot->expr);
    } else
    if (!ecs_os_strcmp(annot->name, "link")) {
        ecs_doc_set_link(v->world, e, annot->expr);
    } else
    if (!ecs_os_strcmp(annot->name, "color")) {
        ecs_doc_set_color(v->world, e, annot->expr);
    } else
    if (!ecs_os_strcmp(annot->name, "tree")) {
        if (!ecs_os_strcmp(annot->expr, "Parent")) {
            flecs_script_apply_non_fragmenting_childof(v->world, node, true);
        } else if (!ecs_os_strcmp(annot->expr, "ChildOf")) {
            flecs_script_apply_non_fragmenting_childof(v->world, node, false);
        } else {
            flecs_script_eval_error(v, annot, 
                "invalid value for tree annotation: '%s' (expected 'Parent' or 'ChildOf')",
                annot->expr);
            return -1;            
        }
    } else {
        flecs_script_eval_error(v, annot, "unknown annotation '%s'",
            annot->name);
        return -1;
    }
    
    return 0;
}

static
int flecs_script_eval_entity(
    ecs_script_eval_visitor_t *v,
    ecs_script_entity_t *node)
{
    bool is_slot = false;
    if (node->kind) {
        ecs_script_id_t id = {
            .first = node->kind,
            .first_sp = -1
        };

        if (!ecs_os_strcmp(node->kind, "prefab")) {
            id.eval = EcsPrefab;
        } else if (!ecs_os_strcmp(node->kind, "slot")) {
            is_slot = true;
        } else if (flecs_script_eval_id(v, node, &id)) {
            return -1;
        }

        node->eval_kind = id.eval;
    } else {
        /* Inherit kind from parent kind's DefaultChildComponent, if it exists */
        ecs_script_scope_t *scope = ecs_script_current_scope(v);
        if (scope && scope->default_component_eval) {
            node->eval_kind = scope->default_component_eval;
        }
    }

    ecs_expr_node_t *name_expr = node->name_expr;
    if (name_expr) {
        node->eval = flecs_script_eval_name_expr(
            v, node, &node->name_expr, false);
        if (!node->eval) {
            return -1;
        }
    } else {
        node->eval = flecs_script_create_entity(v, node->name);
    }

    node->parent = v->entity;

    if (v->template_entity) {
        ecs_add_pair(
            v->world, node->eval, EcsScriptTemplate, v->template_entity);
    }

    if (is_slot) {
        ecs_entity_t parent = ecs_get_target(
            v->world, node->eval, EcsChildOf, 0);
        if (!parent) {
            flecs_script_eval_error(v, node, 
                "slot entity must have a parent");
            return -1;
        }

        ecs_add_pair(v->world, node->eval, EcsSlotOf, parent);
    }

    const EcsDefaultChildComponent *default_comp = NULL;
    ecs_script_entity_t *old_entity = v->entity;
    v->entity = node;

    if (node->eval_kind) {
        ecs_add_id(v->world, node->eval, node->eval_kind);

        default_comp = 
            ecs_get(v->world, node->eval_kind, EcsDefaultChildComponent);
        if (default_comp) {
            if (!default_comp->component) {
                flecs_script_eval_error(v, node, "entity '%s' has kind '%s' "
                    "with uninitialized DefaultChildComponent",
                        node->name, node->kind);
                return -1;
            }

            node->scope->default_component_eval = default_comp->component;
        }
    }

    int32_t i, count = ecs_vec_count(&v->r->annot);
    if (count) {
        ecs_script_annot_t **annots = ecs_vec_first(&v->r->annot);
        for (i = 0; i < count ; i ++) {
            flecs_script_apply_annot(v, node, annots[i]);
        }
        ecs_vec_clear(&v->r->annot);
    }

    bool old_is_with_scope = v->is_with_scope;
    ecs_entity_t old_template_entity = v->template_entity;
    v->is_with_scope = false;
    v->template_entity = 0;
    if (ecs_script_visit_node(v, node->scope)) {
        return -1;
    }
    v->template_entity = old_template_entity;
    v->is_with_scope = old_is_with_scope;

    if (node->eval_kind) {
        if (!node->kind_w_expr) {
            if (ecs_get_type_info(v->world, node->eval_kind) != NULL) {
                ecs_modified_id(v->world, node->eval, node->eval_kind);
            }
        }
    }

    v->entity = old_entity;

    return 0;
}

static
ecs_entity_t flecs_script_get_src(
    ecs_script_eval_visitor_t *v,
    ecs_entity_t entity,
    ecs_id_t id)
{
    if (entity == EcsVariable) {
        if (ECS_IS_PAIR(id)) {
            return ecs_pair_first(v->world, id);
        } else {
            return id & ECS_COMPONENT_MASK;
        }
    }
    return entity;
}

static
bool flecs_script_can_default_ctor(
    ecs_world_t *world,
    ecs_id_t component)
{
    /* Check if tag is a component, and if so, if it can be default 
     * constructed. */
    ecs_entity_t type = ecs_get_typeid(world, component);
    if (type) {
        const ecs_type_info_t *ti = ecs_get_type_info(world, type);
        ecs_assert(ti != NULL, ECS_INTERNAL_ERROR, NULL);
        if (ti->hooks.flags & ECS_TYPE_HOOK_CTOR_ILLEGAL) {
            return false;
        }
    }
    return true;
}

static
int flecs_script_eval_tag(
    ecs_script_eval_visitor_t *v,
    ecs_script_tag_t *node)
{
    bool resolved = node->id.eval != 0;

    if (flecs_script_eval_id(v, node, &node->id)) {
        return -1;
    }

    if (!resolved) {
        if (!flecs_script_can_default_ctor(v->world, node->id.eval)) {
            if (node->id.second) {
                flecs_script_eval_error(v, node, 
                    "cannot add (%s, %s), "
                    "type is not default constructible",
                    node->id.first, node->id.second);
            } else {
                flecs_script_eval_error(v, node, 
                    "cannot add %s, "
                    "type is not default constructible",
                    node->id.first);
            }
            return -1;
        }
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

    ecs_entity_t src = flecs_script_get_src(
        v, v->entity->eval, node->id.eval);
    ecs_add_id(v->world, src, node->id.eval);

    return 0;
}

static
int flecs_script_eval_component(
    ecs_script_eval_visitor_t *v,
    ecs_script_component_t *node)
{
    bool resolved = node->id.eval != 0;

    if (flecs_script_eval_id(v, node, &node->id)) {
        return -1;
    }

    if (!resolved) {
        if (!flecs_script_can_default_ctor(v->world, node->id.eval)) {
            if (node->id.second) {
                flecs_script_eval_error(v, node, 
                    "cannot add (%s, %s), "
                    "type is not default constructible",
                    node->id.first, node->id.second);
            } else {
                flecs_script_eval_error(v, node, 
                    "cannot add %s, "
                    "type is not default constructible",
                    node->id.first);
            }
            return -1;
        }
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

    ecs_entity_t src = flecs_script_get_src(v, v->entity->eval, node->id.eval);

    if (node->expr) {
        const ecs_type_info_t *ti = flecs_script_get_type_info(
            v, node, node->id.eval);
        if (!ti) {
            return -1;
        }

        ecs_record_t *r = flecs_entities_get(v->world, src);
        ecs_assert(r != NULL, ECS_INTERNAL_ERROR, NULL);
        ecs_table_t *table = r->table;
 
        ecs_value_t value = {
            .ptr = ecs_ensure_id(v->world, src, node->id.eval, 
                flecs_ito(size_t, ti->size)),
            .type = ti->component
        };

        /* Assign entire value, including members not set by expression. This 
         * prevents uninitialized or unexpected values. */
        if (r->table != table) {
            if (!ti->hooks.ctor) {
                ecs_os_memset(value.ptr, 0, ti->size);
            } else if (ti->hooks.ctor) {
                if (ti->hooks.dtor) {
                    ti->hooks.dtor(value.ptr, 1, ti);
                }
                ti->hooks.ctor(value.ptr, 1, ti);
            }
        }

        if (flecs_script_eval_expr(v, &node->expr, &value)) {
            return -1;
        }

        ecs_modified_id(v->world, src, node->id.eval);
    } else {
        ecs_add_id(v->world, src, node->id.eval);
    }

    return 0;
}

static
int flecs_script_eval_var_component(
    ecs_script_eval_visitor_t *v,
    ecs_script_var_component_t *node)
{
    ecs_script_var_t *var = flecs_script_find_var(
        v->vars, node->name, v->dynamic_variable_binding ? NULL : &node->sp);
    ecs_value_t var_value = {0};
    if (!var) {
        /* If we cannot find local variable, try find as const var */
        ecs_entity_t var_entity = 0;
        if (flecs_script_find_entity(
            v, 0, node->name, NULL, NULL, &var_entity, NULL)) 
        {
            return -1;
        }

        if (var_entity) {
            var_value = ecs_const_var_get(v->world, var_entity);
        }

        if (!var_value.ptr) {
            flecs_script_eval_error(v, node, 
                "unresolved variable '%s'", node->name);
            return -1;
        }
    } else {
        var_value = var->value;
    }

    if (v->is_with_scope) {
        flecs_script_eval_error(v, node, "invalid component in with scope"); 
        return -1;
    }

    ecs_id_t var_id = var_value.type;

    if (var_value.ptr) {
        const ecs_type_info_t *ti = flecs_script_get_type_info(
            v, node, var_id);
        if (!ti) {
            return -1;
        }

        ecs_value_t value = {
            .ptr = ecs_ensure_id(v->world, v->entity->eval, var_id, 
                flecs_ito(size_t, ti->size)),
            .type = var_id
        };

        ecs_value_copy_w_type_info(v->world, ti, value.ptr, var_value.ptr);

        ecs_modified_id(v->world, v->entity->eval, var_id);
    } else {
        ecs_add_id(v->world, v->entity->eval, var_id);
    }

    return 0;
}

static
int flecs_script_eval_default_component(
    ecs_script_eval_visitor_t *v,
    ecs_script_default_component_t *node)
{
    if (!v->entity) {
        flecs_script_eval_error(v, node, 
            "missing entity for default component");
        return -1;
    }

    ecs_script_scope_t *scope = ecs_script_current_scope(v);
    ecs_assert(scope != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(scope->node.kind == EcsAstScope, ECS_INTERNAL_ERROR, NULL);
    scope = scope->parent;

    if (!scope) {
        flecs_script_eval_error(v, node,
            "entity '%s' is in root scope which cannot have a default type",
                v->entity->name); 
        return -1;
    }

    ecs_id_t default_type = scope->default_component_eval;
    if (!default_type) {
        flecs_script_eval_error(v, node,
            "scope for entity '%s' does not have a default type",
                v->entity->name);
        return -1;
    }

    const ecs_type_info_t *ti = ecs_get_type_info(v->world, default_type);
    if (ti == NULL) {
        char *id_str = ecs_id_str(v->world, default_type);
        flecs_script_eval_error(v, node,
            "cannot use tag '%s' as default type in assignment",
                id_str);
        ecs_os_free(id_str);
        return -1;
    }

    ecs_value_t value = {
        .ptr = ecs_ensure_id(v->world, v->entity->eval, default_type, 
            flecs_ito(size_t, ti->size)),
        .type = default_type
    };

    if (flecs_script_eval_expr(v, &node->expr, &value)) {
        return -1;
    }

    ecs_modified_id(v->world, v->entity->eval, default_type);

    return 0;
}

static
int flecs_script_eval_with_var(
    ecs_script_eval_visitor_t *v,
    ecs_script_var_component_t *node)
{
    ecs_script_var_t *var = flecs_script_find_var(
        v->vars, node->name, v->dynamic_variable_binding ? NULL : &node->sp);
    if (!var) {
        flecs_script_eval_error(v, node, 
            "unresolved variable '%s'", node->name);
        return -1;
    }

    ecs_allocator_t *a = &v->r->allocator;
    ecs_value_t *value = flecs_script_with_append(a, v, NULL); // TODO: vars of non trivial types
    *value = var->value;

    return 0;
}

static
int flecs_script_eval_with_tag(
    ecs_script_eval_visitor_t *v,
    ecs_script_tag_t *node)
{
    if (flecs_script_eval_id(v, node, &node->id)) {
        return -1;
    }

    ecs_allocator_t *a = &v->r->allocator;
    ecs_value_t *value = flecs_script_with_append(a, v, NULL);
    value->type = node->id.eval;
    value->ptr = NULL;

    return 0;
}

static
int flecs_script_eval_with_component(
    ecs_script_eval_visitor_t *v,
    ecs_script_component_t *node)
{
    if (flecs_script_eval_id(v, node, &node->id)) {
        return -1;
    }

    ecs_allocator_t *a = &v->r->allocator;
    const ecs_type_info_t *ti = flecs_script_get_type_info(
        v, node, node->id.eval);

    ecs_value_t *value = flecs_script_with_append(a, v, ti);
    value->type = node->id.eval;
    value->ptr = NULL;

    if (node->expr) {
        if (!ti) {
            return -1;
        }

        value->ptr = flecs_stack_alloc(&v->r->stack, ti->size, ti->alignment);
        value->type = ti->component; // Expression parser needs actual type

        if (ti->hooks.ctor) {
            ti->hooks.ctor(value->ptr, 1, ti);
        }

        if (flecs_script_eval_expr(v, &node->expr, value)) {
            return -1;
        }

        value->type = node->id.eval; // Restore so we're adding actual id
    }

    return 0;
}

static
int flecs_script_eval_with(
    ecs_script_eval_visitor_t *v,
    ecs_script_with_t *node)
{
    ecs_allocator_t *a = &v->r->allocator;
    int32_t prev_with_count = flecs_script_with_count(v);
    ecs_stack_cursor_t *prev_stack_cursor = flecs_stack_get_cursor(&v->r->stack);
    int result = 0;

    if (ecs_script_visit_scope(v, node->expressions)) {
        result = -1;
        goto error;
    }

    ecs_value_t *value = flecs_script_with_last(v);
    if (!value->ptr) {
        if (ecs_is_valid(v->world, value->type)) {
            node->scope->default_component_eval = value->type;
        }
    }

    bool old_is_with_scope = v->is_with_scope;
    v->is_with_scope = true;

    if (ecs_script_visit_scope(v, node->scope)) {
        result = -1;
        goto error;
    }

    v->is_with_scope = old_is_with_scope;

error:
    flecs_script_with_set_count(a, v, prev_with_count);
    flecs_stack_restore_cursor(&v->r->stack, prev_stack_cursor);
    return result;
}

int flecs_script_eval_using(
    ecs_script_eval_visitor_t *v,
    ecs_script_using_t *node)
{
    ecs_allocator_t *a = &v->r->allocator;
    int32_t len = ecs_os_strlen(node->name);

    if (len > 2 && !ecs_os_strcmp(&node->name[len - 2], ".*")) {
        char *path = flecs_strdup(a, node->name);
        path[len - 2] = '\0';

        ecs_entity_t from = ecs_lookup(v->world, path);
        if (!from) {
            flecs_script_eval_error(v, node, 
                "unresolved path '%s' in using statement", path);
            flecs_strfree(a, path);
            return -1;
        }

        /* Add each child of the scope to using stack */
        ecs_iter_t it = ecs_children(v->world, from);
        while (ecs_children_next(&it)) {
            int32_t i, count = it.count;
            for (i = 0; i < count; i ++) {
                ecs_vec_append_t(
                    a, &v->r->using, ecs_entity_t)[0] = it.entities[i];
            }
        }

        flecs_strfree(a, path);
    } else {
        ecs_entity_t from = ecs_lookup_path_w_sep(
            v->world, 0, node->name, NULL, NULL, false);
        if (!from) {
            from = ecs_entity(v->world, {
                .name = node->name,
                .root_sep = ""
            });

            if (!from) {
                return -1;
            }
        }

        ecs_vec_append_t(a, &v->r->using, ecs_entity_t)[0] = from;
    }

    return 0;
}

static
int flecs_script_eval_module(
    ecs_script_eval_visitor_t *v,
    ecs_script_module_t *node)
{
    /* Always create modules in root */
    ecs_entity_t old_scope = ecs_set_scope(v->world, 0);
    v->parent = 0;


    ecs_entity_t m = flecs_script_create_entity(v, node->name);
    if (!m) {
        return -1;
    }

    ecs_set_scope(v->world, old_scope);

    ecs_add_id(v->world, m, EcsModule);

    v->module = m;
    v->parent = m;

    return 0;
}

int flecs_script_eval_const(
    ecs_script_eval_visitor_t *v,
    ecs_script_var_node_t *node,
    bool export)
{
    /* Declare variable. If this variable is declared while instantiating a
     * template, the variable sp has already been resolved in all expressions
     * that used it, so we don't need to create the variable with a name. */
    ecs_script_var_t *var = NULL;
    if (!export) {
        ecs_entity_t e = ecs_lookup_child(v->world, v->parent, node->name);
        if (e) {
            ecs_value_t existing = ecs_const_var_get(v->world, e);
            if (existing.ptr != NULL) {
                flecs_script_eval_error(v, node, 
                    "local variable '%s' shadows an exported variable", 
                        node->name);
                return -1;
            }
        }

        var = ecs_script_vars_declare(v->vars, 
            v->template_entity ? NULL : node->name);
        if (!var) {
            flecs_script_eval_error(v, node, 
                "variable '%s' redeclared", node->name);
            return -1;
        }
    } else {
        var = ecs_script_vars_lookup(v->vars, 
            v->template_entity ? NULL : node->name);
        if (var) {
            flecs_script_eval_error(v, node, 
                "exported variable '%s' shadows a local variable", node->name);
            return -1;
        }
    }

    ecs_entity_t type = 0;
    const ecs_type_info_t *ti = NULL;
    if (node->expr) {
        type = node->expr->type;
        ti = node->expr->type_info;
    }

    if (!type && node->type) {
        if (flecs_script_find_entity(v, 0, node->type, NULL, NULL, &type, NULL) || !type) {
            flecs_script_eval_error(v, node,
                "unresolved type '%s' for const variable '%s'", 
                    node->type, node->name);
            return -1;
        }

        ti = flecs_script_get_type_info(v, node, type);
        if (!ti) {
            flecs_script_eval_error(v, node,
                "failed to retrieve type info for '%s' for const variable '%s'", 
                    node->type, node->name);
            return -1;
        }
    }

    ecs_value_t result = {
        .type = type
    };

    if (type && ti) {
        ecs_assert(type != 0, ECS_INTERNAL_ERROR, NULL);
        ecs_assert(ti != NULL, ECS_INTERNAL_ERROR, NULL);

        result.ptr = flecs_stack_calloc(&v->r->stack, ti->size, ti->alignment);

        if (ti->hooks.ctor) {
            ti->hooks.ctor(result.ptr, 1, ti);
        }

        if (flecs_script_eval_expr(v, &node->expr, &result)) {
            flecs_script_eval_error(v, node,
                "failed to evaluate expression for const variable '%s'", 
                    node->name);
            return -1;
        }
    } else {
        /* We don't know the type yet, so we can't create a storage for it yet.
         * Run the expression first to deduce the type. */
        ecs_value_t value = {0};
        if (flecs_script_eval_expr(v, &node->expr, &value)) {
            flecs_script_eval_error(v, node,
                "failed to evaluate expression for const variable '%s'", 
                    node->name);
            return -1;
        }

        ecs_assert(value.type != 0, ECS_INTERNAL_ERROR, NULL);
        ti = ecs_get_type_info(v->world, value.type);
        ecs_assert(ti != NULL, ECS_INTERNAL_ERROR, NULL);

        result.ptr = flecs_stack_calloc(
            &v->r->stack, ti->size, ti->alignment);
        result.type = value.type;

        if (ti->hooks.ctor) {
            ti->hooks.ctor(result.ptr, 1, ti);
        }

        ecs_value_copy_w_type_info(v->world, ti, result.ptr, value.ptr);
        ecs_value_fini_w_type_info(v->world, ti, value.ptr);
        flecs_free(&v->world->allocator, ti->size, value.ptr);
    }

    /* If variable resolves to a constant expression, mark it as const so that
     * its value can be folded. */
    if (!export) {
        if (node->expr->kind == EcsExprValue) {
            var->is_const = true;
        }
        var->type_info = ti;
        var->value = result;
    } else {
        ecs_entity_t const_var = ecs_const_var(v->world, {
            .parent = v->parent,
            .name = node->name,
            .type = result.type,
            .value = result.ptr
        });

        /* Clean up value since it'll have been copied into the const var. */
        if (ti->hooks.dtor) {
            ti->hooks.dtor(result.ptr, 1, ti);
        }

        flecs_stack_free(result.ptr, ti->size);

        if (!const_var) {
            flecs_script_eval_error(v, node,
                "failed to create exported const variable '%s'",
                    node->name);
            return -1;
        }
    }

    return 0;
}

static
int flecs_script_eval_pair_scope(
    ecs_script_eval_visitor_t *v,
    ecs_script_pair_scope_t *node)
{
    ecs_entity_t first;
    if (flecs_script_find_entity(
        v, 0, node->id.first, &node->id.first_expr, &node->id.first_sp, 
        &first, NULL) || !first)
    {
        first = flecs_script_create_entity(v, node->id.first);
        if (!first) {
            return -1;
        }
    }

    ecs_entity_t second = 0;
    if (node->id.second) {
        if (node->id.second[0] == '$') {
            if (flecs_script_find_entity(v, 0, node->id.second, 
                &node->id.second_expr, &node->id.second_sp, &second, NULL)) 
            {
                return -1;
            }
        } else {
            second = flecs_script_create_entity(v, node->id.second);
        }
        
    }
     
    if (!second) {
        return -1;
    }

    ecs_allocator_t *a = &v->r->allocator;
    ecs_entity_t prev_first = v->with_relationship;
    ecs_entity_t prev_second = 0;
    int32_t prev_with_relationship_sp = v->with_relationship_sp;

    v->with_relationship = first;

    if (prev_first != first) {
        /* Append new element to with stack */
        ecs_value_t *value = flecs_script_with_append(a, v, NULL);
        value->type = ecs_pair(first, second);
        value->ptr = NULL;
        v->with_relationship_sp = flecs_script_with_count(v) - 1;
    } else {
        /* Get existing with element for current relationhip stack */
        ecs_value_t *value = ecs_vec_get_t(
            &v->r->with, ecs_value_t, v->with_relationship_sp);
        ecs_assert(ECS_PAIR_FIRST(value->type) == (uint32_t)first, 
            ECS_INTERNAL_ERROR, NULL);
        prev_second = ECS_PAIR_SECOND(value->type);
        value->type = ecs_pair(first, second);
        value->ptr = NULL;
    }

    if (ecs_script_visit_scope(v, node->scope)) {
        return -1;
    }

    if (prev_second) {
        ecs_value_t *value = ecs_vec_get_t(
            &v->r->with, ecs_value_t, v->with_relationship_sp);
        value->type = ecs_pair(first, prev_second);
    } else {
        flecs_script_with_set_count(a, v, v->with_relationship_sp);
    }

    v->with_relationship = prev_first;
    v->with_relationship_sp = prev_with_relationship_sp;

    return 0;
}

static
int flecs_script_eval_if(
    ecs_script_eval_visitor_t *v,
    ecs_script_if_t *node)
{
    ecs_value_t condval = { .type = 0, .ptr = NULL };
    if (flecs_script_eval_expr(v, &node->expr, &condval)) {
        return -1;
    }

    bool cond;
    if (condval.type == ecs_id(ecs_bool_t)) {
        cond = *(bool*)(condval.ptr);
    } else {
        ecs_meta_cursor_t cur = ecs_meta_cursor(
            v->world, condval.type, condval.ptr);
        cond = ecs_meta_get_bool(&cur);
    }

    ecs_value_free(v->world, condval.type, condval.ptr);

    if (flecs_script_eval_scope(v, cond ? node->if_true : node->if_false)) {
        return -1;
    }

    return 0;
}

static
int flecs_script_eval_for_range(
    ecs_script_eval_visitor_t *v,
    ecs_script_for_range_t *node)
{
    int32_t from;
    int32_t to;
    ecs_value_t from_val = { .type = ecs_id(ecs_i32_t), .ptr = &from };
    ecs_value_t to_val = { .type = ecs_id(ecs_i32_t), .ptr = &to };

    if (flecs_script_eval_expr(v, &node->from, &from_val)) {
        return -1;
    }

    if (flecs_script_eval_expr(v, &node->to, &to_val)) {
        return -1;
    }

    v->vars = flecs_script_vars_push(v->vars, &v->r->stack, &v->r->allocator);

    ecs_script_var_t *var = ecs_script_vars_declare(v->vars, node->loop_var);
    var->value.ptr = flecs_stack_calloc(&v->r->stack, 4, 4);
    var->value.type = ecs_id(ecs_i32_t);
    var->type_info = ecs_get_type_info(v->world, ecs_id(ecs_i32_t));
    
    int32_t i;
    for (i = from; i < to; i ++) {
        *(int32_t*)var->value.ptr = i;
        if (flecs_script_eval_scope(v, node->scope)) {
            v->vars = ecs_script_vars_pop(v->vars);
            return -1;
        }
    }

    v->vars = ecs_script_vars_pop(v->vars);

    return 0;
}

static
int flecs_script_eval_annot(
    ecs_script_eval_visitor_t *v,
    ecs_script_annot_t *node)
{
    if (!v->base.next) {
        flecs_script_eval_error(v, node,
            "missing target for @%s annotation", node->name);
        return -1;
    }
    
    ecs_script_node_kind_t next_kind = v->base.next->kind;
    if (next_kind != EcsAstEntity && next_kind != EcsAstTemplate) {
        if (next_kind != EcsAstAnnotation) {
            flecs_script_eval_error(v, node,
                "target of @%s annotation must be an entity or template", 
                    node->name);
            return -1;
        }
    }

    ecs_allocator_t *a = &v->r->allocator;
    ecs_vec_append_t(a, &v->r->annot, ecs_script_annot_t*)[0] = node;

    return 0;
}

int flecs_script_eval_node(
    ecs_script_visit_t *_v,
    ecs_script_node_t *node)
{
    ecs_script_eval_visitor_t *v = (ecs_script_eval_visitor_t*)_v;
    ecs_assert(v != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(v->template == NULL, ECS_INTERNAL_ERROR, NULL);

    switch(node->kind) {
    case EcsAstScope:
        return flecs_script_eval_scope(
            v, (ecs_script_scope_t*)node);
    case EcsAstTag:
        return flecs_script_eval_tag(
            v, (ecs_script_tag_t*)node);
    case EcsAstComponent:
        return flecs_script_eval_component(
            v, (ecs_script_component_t*)node);
    case EcsAstVarComponent:
        return flecs_script_eval_var_component(
            v, (ecs_script_var_component_t*)node);
    case EcsAstDefaultComponent:
        return flecs_script_eval_default_component(
            v, (ecs_script_default_component_t*)node);
    case EcsAstWithVar:
        return flecs_script_eval_with_var(
            v, (ecs_script_var_component_t*)node);
    case EcsAstWithTag:
        return flecs_script_eval_with_tag(
            v, (ecs_script_tag_t*)node);
    case EcsAstWithComponent:
        return flecs_script_eval_with_component(
            v, (ecs_script_component_t*)node);
    case EcsAstWith:
        return flecs_script_eval_with(
            v, (ecs_script_with_t*)node);
    case EcsAstUsing:
        return flecs_script_eval_using(
            v, (ecs_script_using_t*)node);
    case EcsAstModule:
        return flecs_script_eval_module(
            v, (ecs_script_module_t*)node);
    case EcsAstAnnotation:
        return flecs_script_eval_annot(
            v, (ecs_script_annot_t*)node);
    case EcsAstTemplate:
        return flecs_script_eval_template(
            v, (ecs_script_template_node_t*)node);
    case EcsAstProp:
        return 0;
    case EcsAstConst:
        return flecs_script_eval_const(
            v, (ecs_script_var_node_t*)node, false);
    case EcsAstExportConst:
        return flecs_script_eval_const(
            v, (ecs_script_var_node_t*)node, true);
    case EcsAstEntity:
        return flecs_script_eval_entity(
            v, (ecs_script_entity_t*)node);
    case EcsAstPairScope:
        return flecs_script_eval_pair_scope(
            v, (ecs_script_pair_scope_t*)node);
    case EcsAstIf:
        return flecs_script_eval_if(
            v, (ecs_script_if_t*)node);
    case EcsAstFor:
        return flecs_script_eval_for_range(
            v, (ecs_script_for_range_t*)node);
    }

    ecs_abort(ECS_INTERNAL_ERROR, "corrupt AST node kind");
}

void flecs_script_eval_visit_init(
    const ecs_script_impl_t *script,
    ecs_script_eval_visitor_t *v,
    const ecs_script_eval_desc_t *desc)
{
    *v = (ecs_script_eval_visitor_t){
        .base = {
            .visit = (ecs_visit_action_t)flecs_script_eval_node,
            .script = ECS_CONST_CAST(ecs_script_impl_t*, script)
        },
        .world = script->pub.world,
        .r = desc ? desc->runtime : NULL
    };

    if (!v->r) {
        v->r = ecs_script_runtime_new();
    }

    if (desc && desc->vars) {
        ecs_allocator_t *a = &v->r->allocator;
        v->vars = flecs_script_vars_push(v->vars, &v->r->stack, a);
        v->vars->parent = desc->vars;
        v->vars->sp = ecs_vec_count(&desc->vars->vars);

        /* When variables are provided to script, don't use cached variable
         * stack pointers, as the order in which the application provides 
         * variables may not be the same across evaluations. */
        v->dynamic_variable_binding = true;
    }

    /* Always include flecs.meta */
    ecs_vec_append_t(&v->r->allocator, &v->r->using, ecs_entity_t)[0] = 
        ecs_lookup(v->world, "flecs.meta");
}

void flecs_script_eval_visit_fini(
    ecs_script_eval_visitor_t *v,
    const ecs_script_eval_desc_t *desc)
{
    if (desc && desc->vars) {
        v->vars = ecs_script_vars_pop(v->vars);
    }

    if (!desc || (v->r != desc->runtime)) {
        ecs_script_runtime_free(v->r);
    }
}

int ecs_script_eval(
    const ecs_script_t *script,
    const ecs_script_eval_desc_t *desc,
    ecs_script_eval_result_t *result)
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

    if (result) {
        ecs_log_start_capture(true);
    }

    flecs_script_eval_visit_init(impl, &v, &priv_desc);
    int r = ecs_script_visit(impl, &v, flecs_script_eval_node);
    flecs_script_eval_visit_fini(&v, &priv_desc);

    // printf("%s\n", ecs_script_ast_to_str(script, true));

    if (result) {
        result->error = ecs_log_stop_capture();
    }

    if (r) {
        ecs_script_runtime_clear(priv_desc.runtime);
    }

    return r;
}

#endif
