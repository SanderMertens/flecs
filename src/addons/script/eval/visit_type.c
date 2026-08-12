/**
 * @file addons/script/eval/visit_type.c
 * @brief Script type visitor.
 */

#include "flecs.h"

#ifdef FLECS_SCRIPT
#include "../script.h"

static const ecs_script_symbol_t* flecs_script_type_lookup_scope(
    const ecs_script_type_visitor_t *v,
    const ecs_script_type_scope_t *scope,
    const char *name);

static bool flecs_script_type_can_type(
    ecs_script_type_visitor_t *v);

static ecs_script_type_scope_t* flecs_script_type_scope_new(
    ecs_script_type_visitor_t *v,
    ecs_script_type_scope_t *parent)
{
    ecs_allocator_t *a = &v->base.script->allocator;
    ecs_script_type_scope_t *result = flecs_calloc_t(
        a, ecs_script_type_scope_t);
    result->parent = parent;
    flecs_name_index_init(&result->symbols, a);
    ecs_vec_append_t(a, &v->scopes, ecs_script_type_scope_t*)[0] = result;
    return result;
}

static bool flecs_script_symbol_is_var(
    ecs_script_symbol_kind_t kind)
{
    return kind == EcsScriptSymbolConst || kind == EcsScriptSymbolProp ||
           kind == EcsScriptSymbolMut || kind == EcsScriptSymbolArgument;
}

static void flecs_script_type_scope_clear_vars(
    ecs_script_type_visitor_t *v,
    ecs_script_type_scope_t *scope,
    int32_t first_symbol)
{
    int32_t i, count = ecs_vec_count(&v->symbols);
    ecs_script_symbol_t *symbols = ecs_vec_first_t(
        &v->symbols, ecs_script_symbol_t);

    for (i = first_symbol; i < count; i ++) {
        ecs_script_symbol_t *symbol = &symbols[i];
        if (symbol->parent != scope) {
            continue;
        }

        if (!flecs_script_symbol_is_var(symbol->kind)) {
            continue;
        }

        ecs_size_t len = ecs_os_strlen(symbol->name);
        flecs_name_index_remove(&scope->symbols,
            flecs_ito(uint64_t, i + 1), flecs_hash(symbol->name, len));
    }

    scope->variable_count = 0;
}

static int32_t flecs_script_type_declare(
    ecs_script_type_visitor_t *v,
    ecs_script_node_t *node,
    ecs_script_type_scope_t *scope,
    const char *name,
    ecs_script_symbol_kind_t kind)
{
    ecs_size_t len = ecs_os_strlen(name);
    uint64_t existing = flecs_name_index_find(&scope->symbols, name, len, 0);
    if (existing) {
        int32_t index = flecs_uto(int32_t, existing) - 1;
        ecs_script_symbol_t *symbol = ecs_vec_get_t(
            &v->symbols, ecs_script_symbol_t, index);
        if ((kind == EcsScriptSymbolEntity) &&
            (symbol->kind == EcsScriptSymbolEntity))
        {
            return index;
        }

        flecs_script_visit_error(v, node,
            "symbol '%s' redeclared", symbol->name);
        return -1;
    }

    ecs_allocator_t *a = &v->base.script->allocator;
    int32_t index = ecs_vec_count(&v->symbols);
    ecs_script_symbol_t *symbol = ecs_vec_append_t(
        a, &v->symbols, ecs_script_symbol_t);
    ecs_os_memset_t(symbol, 0, ecs_script_symbol_t);
    symbol->name = ecs_os_strdup(name);
    symbol->kind = kind;
    symbol->parent = scope;

    switch(kind) {
    case EcsScriptSymbolConst:
    case EcsScriptSymbolProp:
    case EcsScriptSymbolMut:
    case EcsScriptSymbolArgument:
        symbol->is.variable = v->variable_count ++;
        scope->variable_count ++;
        break;
    case EcsScriptSymbolEntity:
    case EcsScriptSymbolGlobalConst:
    case EcsScriptSymbolGlobalMut:
        symbol->is.entity = v->entity_count ++;
        break;
    case EcsScriptSymbolUnresolved:
        break;
    }

    flecs_name_index_ensure(&scope->symbols,
        flecs_ito(uint64_t, index + 1), symbol->name, len, 0);

    return index;
}

static int32_t flecs_script_type_declare_entity(
    ecs_script_type_visitor_t *v,
    ecs_script_node_t *node,
    const char *name)
{
    if (!name[0]) {
        flecs_script_visit_error(v, node, "invalid entity name '%s'", name);
        return -1;
    }

    return flecs_script_type_declare(
        v, node, v->cur, name, EcsScriptSymbolEntity);
}

static int flecs_script_type_declare_var(
    ecs_script_type_visitor_t *v,
    ecs_script_node_t *node,
    const char *name,
    ecs_script_symbol_kind_t kind)
{
    if (flecs_script_type_declare(v, node, v->cur, name, kind) == -1) {
        return -1;
    }

    return 0;
}

static int flecs_script_type_var(
    ecs_script_type_visitor_t *v,
    ecs_script_node_t *node,
    ecs_script_symbol_kind_t kind)
{
    ecs_script_var_node_t *var = (ecs_script_var_node_t*)node;

    return flecs_script_type_declare_var(v, node, var->name, kind);
}

static int flecs_script_type_scope_stmts(
    ecs_script_type_visitor_t *v,
    ecs_script_scope_t *scope,
    ecs_script_type_scope_t *type_scope)
{
    ecs_script_type_scope_t *prev_scope = v->cur;
    int32_t prev_variable_count = v->variable_count;
    int32_t first_symbol = ecs_vec_count(&v->symbols);

    if (!type_scope) {
        type_scope = flecs_script_type_scope_new(v, prev_scope);
    }

    v->cur = type_scope;

    bool mirror_vars = flecs_script_type_can_type(v);
    if (mirror_vars) {
        if (v->eval->vars) {
            v->eval->vars = flecs_script_vars_push(v->eval->vars, NULL, NULL);
        } else {
            v->eval->vars = flecs_script_vars_push(
                NULL, &v->eval->r->stack, &v->eval->r->allocator);
        }
    }

    int result = ecs_script_visit_scope(v, scope);

    flecs_script_type_scope_clear_vars(v, type_scope, first_symbol);

    if (mirror_vars) {
        v->eval->vars = ecs_script_vars_pop(v->eval->vars);
    }

    v->cur = prev_scope;
    v->variable_count = prev_variable_count;

    return result;
}

static int flecs_script_type_body(
    ecs_script_type_visitor_t *v,
    ecs_script_node_t *node,
    ecs_script_scope_t *scope,
    ecs_script_type_scope_t **type_scope_out)
{
    ecs_script_type_scope_t *prev_scope = v->cur;
    int32_t prev_variable_count = v->variable_count;
    int32_t prev_entity_count = v->entity_count;

    ecs_script_type_scope_t *type_scope = flecs_script_type_scope_new(v, NULL);
    v->cur = type_scope;
    v->variable_count = 0;
    v->entity_count = 0;

    if (type_scope_out) {
        type_scope_out[0] = type_scope;
    }

    /* Template instances are evaluated with a variable stack that holds $this,
     * followed by the props and muts of the template. Declare $this here so
     * that the slots assigned by the type visitor line up with that stack. */
    int result = flecs_script_type_declare_var(
        v, node, "this", EcsScriptSymbolConst);
    if (!result) {
        ecs_script_vars_t *prev_vars = NULL;
        bool mirror_vars = flecs_script_type_can_type(v);
        if (mirror_vars) {
            prev_vars = v->eval->vars;
            v->eval->vars = flecs_script_vars_push(
                NULL, &v->eval->r->stack, &v->eval->r->allocator);
            ecs_script_var_t *this_var = ecs_script_vars_declare(
                v->eval->vars, "this");
            this_var->value.type = ecs_id(ecs_entity_t);
            this_var->type_info = ecs_get_type_info(
                v->world, ecs_id(ecs_entity_t));
        }

        v->no_eval ++;
        v->template_body ++;
        result = ecs_script_visit_scope(v, scope);
        v->template_body --;
        v->no_eval --;

        if (mirror_vars) {
            ecs_script_vars_pop(v->eval->vars);
            v->eval->vars = prev_vars;
        }
    }

    v->cur = prev_scope;
    v->variable_count = prev_variable_count;
    v->entity_count = prev_entity_count;

    return result;
}

static bool flecs_script_type_can_eval(
    ecs_script_type_visitor_t *v)
{
    return v->eval != NULL && !v->no_eval;
}

static bool flecs_script_type_can_type(
    ecs_script_type_visitor_t *v)
{
    return v->eval != NULL && !v->no_typing;
}

static int flecs_script_type_expr(
    ecs_script_type_visitor_t *v,
    ecs_expr_node_t **expr_ptr,
    ecs_entity_t type,
    bool track_refs)
{
    if (!flecs_script_type_can_type(v)) {
        return 1;
    }

    if (expr_ptr[0]->type_info) {
        return 0;
    }

    ecs_script_eval_visitor_t *eval = v->eval;
    ecs_script_impl_t *impl = v->base.script;
    ecs_script_t *script = &impl->pub;

    ecs_expr_eval_desc_t desc = {
        .name = script->name,
        .lookup_action = flecs_script_find_entity_action,
        .lookup_ctx = eval,
        .vars = eval->vars,
        .type = type,
        .runtime = eval->r,
        .disable_dynamic_variable_binding = true,
        .script_visitor = eval
    };

    flecs_log_capture_push(true);
    int result = flecs_expr_visit_type(script, expr_ptr, &desc);
    char *err = flecs_log_capture_pop();
    ecs_os_free(err);

    if (result) {
        return 1;
    }

    if (v->template_body) {
        return 0;
    }

    if (track_refs && eval->script_entity) {
        if (flecs_expr_visit_refs(
            script, *expr_ptr, &impl->refs, NULL, &impl->refs))
        {
            return -1;
        }
    }

    if (flecs_expr_visit_fold(script, expr_ptr, &desc)) {
        return -1;
    }

    return 0;
}

static int flecs_script_type_name_expr(
    ecs_script_type_visitor_t *v,
    ecs_expr_node_t **expr_ptr)
{
    return flecs_script_type_expr(v, expr_ptr, ecs_id(ecs_string_t), false);
}

static bool flecs_script_name_is_static(
    const char *name)
{
    if (!name || !name[0] || name[0] == '#') {
        return false;
    }

    if (flecs_script_name_is_var(name) || !ecs_os_strcmp(name, "_")) {
        return false;
    }

    if (strchr(name, '*') || strchr(name, '\\')) {
        return false;
    }

    return true;
}

static bool flecs_script_type_id_elem_resolvable(
    ecs_script_type_visitor_t *v,
    const char *name)
{
    if (flecs_script_lookup_symbol(v, name).kind != EcsScriptSymbolUnresolved) {
        return true;
    }

    if (!strchr(name, '.')) {
        return false;
    }

    char *path = ecs_os_strdup(name);
    char *cur = path, *next = strchr(cur, '.');
    next[0] = '\0';

    bool result = false;
    ecs_script_symbol_t symbol = flecs_script_lookup_symbol(v, cur);
    if (symbol.kind == EcsScriptSymbolEntity) {
        if (symbol.external) {
            result = ecs_lookup_path_w_sep(v->world, symbol.is.external,
                &next[1], NULL, NULL, false) != 0;
        } else {
            const ecs_script_type_scope_t *scope = symbol.scope;
            result = true;
            do {
                cur = &next[1];
                next = strchr(cur, '.');
                if (next) {
                    next[0] = '\0';
                }

                const ecs_script_symbol_t *s = scope ?
                    flecs_script_type_lookup_scope(v, scope, cur) : NULL;
                if (!s || s->kind != EcsScriptSymbolEntity) {
                    result = false;
                    break;
                }

                scope = s->scope;
            } while (next);
        }
    }

    ecs_os_free(path);
    return result;
}

static int flecs_script_type_id_name_exprs(
    ecs_script_type_visitor_t *v,
    ecs_script_id_t *id)
{
    if (v->template_body) {
        return 0;
    }

    if (id->first_expr && !flecs_script_name_is_var(id->first)) {
        if (flecs_script_type_name_expr(v, &id->first_expr) == -1) {
            return -1;
        }
    }

    if (id->second_expr && !flecs_script_name_is_var(id->second)) {
        if (flecs_script_type_name_expr(v, &id->second_expr) == -1) {
            return -1;
        }
    }

    return 0;
}

static int flecs_script_type_component_id(
    ecs_script_type_visitor_t *v,
    ecs_script_node_t *node,
    ecs_script_id_t *id)
{
    if (!flecs_script_type_can_type(v)) {
        return 1;
    }

    if (id->eval && !id->dynamic) {
        return 0;
    }

    if (id->first_expr || id->second_expr) {
        if (flecs_script_type_id_name_exprs(v, id)) {
            return -1;
        }

        return 1;
    }

    flecs_log_capture_push(true);
    int result = flecs_script_eval_id(v->eval, node, id);
    char *err = flecs_log_capture_pop();
    ecs_os_free(err);

    if (!result) {
        return 0;
    }

    if (!flecs_script_type_can_eval(v)) {
        return 1;
    }

    if (!id->first_expr && flecs_script_name_is_static(id->first) &&
        !flecs_script_type_id_elem_resolvable(v, id->first))
    {
        flecs_script_visit_error(v, node,
            "unresolved identifier '%s'", id->first);
        return -1;
    }

    if (id->second && !id->second_expr &&
        flecs_script_name_is_static(id->second) &&
        !flecs_script_type_id_elem_resolvable(v, id->second))
    {
        bool oneof = false;
        ecs_script_symbol_t first = flecs_script_lookup_symbol(v, id->first);
        if (first.kind != EcsScriptSymbolEntity || !first.external ||
            flecs_get_oneof(v->world, first.is.external))
        {
            oneof = true;
        }

        if (!oneof) {
            flecs_script_visit_error(v, node,
                "unresolved identifier '%s'", id->second);
            return -1;
        }
    }

    return 1;
}

static int flecs_script_type_check_default_ctor(
    ecs_script_type_visitor_t *v,
    ecs_script_node_t *node,
    const ecs_script_id_t *id)
{
    if (node->kind != EcsAstTag && node->kind != EcsAstComponent) {
        return 0;
    }

    if (!flecs_script_can_default_ctor(v->world, id->eval)) {
        if (id->second) {
            flecs_script_visit_error(v, node,
                "cannot add (%s, %s), "
                "type is not default constructible",
                id->first, id->second);
        } else {
            flecs_script_visit_error(v, node,
                "cannot add %s, "
                "type is not default constructible",
                id->first);
        }
        return -1;
    }

    return 0;
}

static int flecs_script_type_tag(
    ecs_script_type_visitor_t *v,
    ecs_script_tag_t *node)
{
    int result = flecs_script_type_component_id(v, &node->node, &node->id);
    if (result == -1) {
        return -1;
    }

    if (!result) {
        if (flecs_script_type_check_default_ctor(
            v, &node->node, &node->id))
        {
            return -1;
        }
    }

    return 0;
}

static int flecs_script_type_pair_expr_type(
    ecs_script_type_visitor_t *v,
    ecs_script_node_t *node,
    const ecs_script_id_t *id,
    ecs_entity_t *type_out)
{
    *type_out = 0;

    if (!flecs_script_type_can_type(v)) {
        return 0;
    }

    if (!id->second || id->first_expr) {
        return 0;
    }

    if (!flecs_script_name_is_static(id->first)) {
        return 0;
    }

    flecs_log_capture_push(true);
    ecs_entity_t first = 0;
    int result = flecs_script_find_entity(
        v->eval, 0, id->first, NULL, NULL, &first, NULL);
    char *err = flecs_log_capture_pop();
    ecs_os_free(err);

    if (result || !first) {
        flecs_script_visit_error(v, node,
            "unresolved identifier '%s'", id->first);
        return -1;
    }

    *type_out = flecs_script_pair_expr_type(v->world, first);
    return 0;
}

static int flecs_script_type_component(
    ecs_script_type_visitor_t *v,
    ecs_script_component_t *node)
{
    int result = flecs_script_type_component_id(v, &node->node, &node->id);
    if (result == -1) {
        return -1;
    }

    if (!result) {
        if (flecs_script_type_check_default_ctor(
            v, &node->node, &node->id))
        {
            return -1;
        }
    }

    if (result || !node->expr) {
        if (result == 1 && node->expr && !node->expr->type_info) {
            ecs_entity_t type;
            if (flecs_script_type_pair_expr_type(
                v, &node->node, &node->id, &type))
            {
                return -1;
            }
            if (type) {
                if (flecs_script_type_expr(v, &node->expr, type, true) == -1) {
                    return -1;
                }
            }
        }

        return 0;
    }

    /* If the id has a script visitor the initializer AST is passed to the
     * visitor as-is during evaluation, don't type check it. */
    if (!node->id.second && ecs_get(
        v->world, node->id.eval, EcsScriptVisitor))
    {
        return 0;
    }

    const ecs_type_info_t *ti = flecs_script_get_type_info(
        v->eval, node, node->id.eval);
    if (!ti) {
        return -1;
    }

    if (flecs_script_type_expr(v, &node->expr, ti->component, true) == -1) {
        return -1;
    }

    return 0;
}

static int flecs_script_type_scope_stmts_no_eval(
    ecs_script_type_visitor_t *v,
    ecs_script_scope_t *scope,
    ecs_script_type_scope_t *type_scope)
{
    v->no_eval ++;
    int result = flecs_script_type_scope_stmts(v, scope, type_scope);
    v->no_eval --;
    return result;
}

static int flecs_script_type_scope(
    ecs_script_type_visitor_t *v,
    ecs_script_scope_t *node)
{
    if (node == v->base.script->root) {
        return ecs_script_visit_scope(v, node);
    }

    return flecs_script_type_scope_stmts_no_eval(v, node, NULL);
}

static bool flecs_script_type_has_tree_annot(
    ecs_script_type_visitor_t *v)
{
    int32_t i, count = ecs_vec_count(&v->eval->r->annot);
    ecs_script_annot_t **annots = ecs_vec_first(&v->eval->r->annot);
    for (i = 0; i < count; i ++) {
        if (!ecs_os_strcmp(annots[i]->name, "tree")) {
            return true;
        }
    }
    return false;
}

static bool flecs_script_type_entity_is_type_pass(
    ecs_script_type_visitor_t *v,
    ecs_script_entity_t *node)
{
    if (!flecs_script_name_is_static(node->kind)) {
        return false;
    }

    ecs_script_symbol_t symbol = flecs_script_lookup_symbol(v, node->kind);
    if (symbol.kind != EcsScriptSymbolEntity || !symbol.external) {
        return false;
    }

    const EcsScriptVisitor *visitor = ecs_get(
        v->world, symbol.is.external, EcsScriptVisitor);

    return visitor && visitor->type_pass;
}

static int flecs_script_type_entity(
    ecs_script_type_visitor_t *v,
    ecs_script_entity_t *node)
{
    ecs_script_type_scope_t *scope = NULL;
    bool can_eval = flecs_script_type_can_eval(v);
    bool plain_name = node->name && !node->name_expr && !node->name_is_var;

    if (node->name_expr) {
        if (flecs_script_type_name_expr(v, &node->name_expr) == -1) {
            return -1;
        }
    }
    bool tree = node->non_fragmenting_parent ||
        (can_eval && flecs_script_type_has_tree_annot(v));
    bool eager = can_eval && plain_name && !tree;

    node->type_pass = false;

    bool type_pass = eager && flecs_script_type_entity_is_type_pass(v, node);

    ecs_entity_t kind = 0;
    if (eager && !type_pass && node->kind) {
        if (!flecs_script_name_is_static(node->kind)) {
            eager = false;
        } else if (!ecs_os_strcmp(node->kind, "prefab")) {
#ifdef FLECS_PREFAB
            kind = EcsPrefab;
#else
            eager = false;
#endif
        } else if (flecs_script_find_entity(
            v->eval, 0, node->kind, NULL, NULL, &kind, NULL) || !kind)
        {
            eager = false;
        }
    }

    if (node->name && !node->name_expr) {
        int32_t index = flecs_script_type_declare_entity(
            v, &node->node, node->name);
        if (index == -1) {
            return -1;
        }

        ecs_script_symbol_t *symbol = ecs_vec_get_t(
            &v->symbols, ecs_script_symbol_t, index);
        if (!symbol->scope) {
            symbol->scope = flecs_script_type_scope_new(v, symbol->parent);
        }

        if (node->entity_slot == -1) {
            node->entity_slot = symbol->is.entity;
        }
        scope = symbol->scope;
    } else if (node->entity_slot == -1) {
        node->entity_slot = v->entity_count ++;
    }

    if (type_pass) {
        node->type_pass = true;

        if (flecs_script_type_scope_stmts_no_eval(v, node->scope, scope)) {
            return -1;
        }

        return flecs_script_eval_entity(v->eval, NULL, node, NULL);
    }

    if (can_eval) {
        ecs_vec_clear(&v->eval->r->annot);
    }

    if (!eager) {
        return flecs_script_type_scope_stmts_no_eval(v, node->scope, scope);
    }

    ecs_script_eval_visitor_t *eval = v->eval;
    ecs_entity_t prev_parent = eval->parent;
    ecs_entity_t entity = flecs_script_create_entity(eval, node->name);
    if (!entity) {
        return -1;
    }

    if (eval->entity_slots && node->entity_slot >= 0) {
        flecs_script_set_entity_slot(
            eval->entity_slots, node->entity_slot, entity);
    }

    if (kind) {
        ecs_add_id(v->world, entity, kind);
    }

    eval->parent = entity;

    int result = flecs_script_type_scope_stmts(v, node->scope, scope);

    eval->parent = prev_parent;

    return result;
}

static int flecs_script_type_template(
    ecs_script_type_visitor_t *v,
    ecs_script_template_node_t *node)
{
    bool can_eval = flecs_script_type_can_eval(v);

    if (flecs_script_type_declare_entity(v, &node->node, node->name) == -1) {
        return -1;
    }

    ecs_script_type_scope_t *body_scope = NULL;
    if (flecs_script_type_body(v, &node->node, node->scope, &body_scope)) {
        return -1;
    }

    if (can_eval) {
        ecs_script_type_scope_t *prev_scope = v->cur;
        v->cur = body_scope;
        int result = flecs_script_eval_template(v->eval, node);
        v->cur = prev_scope;
        return result;
    }

    return 0;
}

static int flecs_script_type_function(
    ecs_script_type_visitor_t *v,
    ecs_script_function_node_t *node)
{
    bool can_eval = flecs_script_type_can_eval(v);

    if (flecs_script_type_declare_entity(v, &node->node, node->name) == -1) {
        return -1;
    }

    ecs_script_type_scope_t *prev_scope = v->cur;
    int32_t prev_variable_count = v->variable_count;
    int32_t prev_entity_count = v->entity_count;

    ecs_script_type_scope_t *fn_scope = flecs_script_type_scope_new(v, NULL);
    v->cur = fn_scope;
    v->variable_count = 0;
    v->entity_count = 0;

    int result = 0;
    int32_t i, count = ecs_vec_count(&node->params);
    ecs_script_fn_param_t *params = ecs_vec_first_t(
        &node->params, ecs_script_fn_param_t);
    for (i = 0; i < count; i ++) {
        if (flecs_script_type_declare_var(
            v, &params[i].node, params[i].name, EcsScriptSymbolArgument))
        {
            result = -1;
            goto done;
        }
    }

    v->no_eval ++;
    v->no_typing ++;
    result = ecs_script_visit_scope(v, node->body);
    v->no_typing --;
    v->no_eval --;

done:
    v->cur = prev_scope;
    v->variable_count = prev_variable_count;
    v->entity_count = prev_entity_count;

    if (!result && can_eval) {
        v->cur = fn_scope;
        result = flecs_script_eval_function(v->eval, node);
        v->cur = prev_scope;
    }

    return result;
}

static int flecs_script_type_if(
    ecs_script_type_visitor_t *v,
    ecs_script_if_t *node)
{
    if (flecs_script_type_expr(v, &node->expr, 0, true) == -1) {
        return -1;
    }

    if (flecs_script_type_scope_stmts_no_eval(v, node->if_true, NULL)) {
        return -1;
    }

    if (flecs_script_type_scope_stmts_no_eval(v, node->if_false, NULL)) {
        return -1;
    }

    return 0;
}

static int flecs_script_type_for(
    ecs_script_type_visitor_t *v,
    ecs_script_for_t *node)
{
    bool can_type = flecs_script_type_can_type(v);
    flecs_script_for_kind_t kind = FlecsScriptForRange;
    ecs_entity_t key_type = 0;
    ecs_entity_t elem_type = ecs_id(ecs_i32_t);
    bool var_types_known = can_type;

    if (can_type) {
        if (!node->expr) {
            if (flecs_script_type_expr(
                v, &node->from, ecs_id(ecs_i32_t), true) == -1)
            {
                return -1;
            }

            if (flecs_script_type_expr(v, &node->to, ecs_id(ecs_i32_t), true) == -1) {
                return -1;
            }

            if (flecs_script_for_check_var_count(v->eval, node, kind)) {
                return -1;
            }
        } else {
            int typed = flecs_script_type_expr(v, &node->expr, 0, true);
            if (typed == -1) {
                return -1;
            }

            if (!typed) {
                if (flecs_script_for_collection_kind(v->eval, node,
                    node->expr->type, &kind, &key_type, &elem_type))
                {
                    return -1;
                }

                if (flecs_script_for_check_var_count(v->eval, node, kind)) {
                    return -1;
                }
            } else {
                var_types_known = false;
            }
        }
    }

    ecs_script_type_scope_t *prev_scope = v->cur;
    int32_t prev_variable_count = v->variable_count;

    v->cur = flecs_script_type_scope_new(v, prev_scope);

    if (can_type) {
        if (v->eval->vars) {
            v->eval->vars = flecs_script_vars_push(v->eval->vars, NULL, NULL);
        } else {
            v->eval->vars = flecs_script_vars_push(
                NULL, &v->eval->r->stack, &v->eval->r->allocator);
        }
    }

    int result = 0;
    int32_t i;
    for (i = 0; i < node->loop_var_count; i ++) {
        if (flecs_script_type_declare_var(
            v, &node->node, node->loop_vars[i], EcsScriptSymbolConst))
        {
            result = -1;
            goto done;
        }

        if (can_type) {
            ecs_script_var_t *var = ecs_script_vars_declare(
                v->eval->vars, node->loop_vars[i]);
            if (var && var_types_known) {
                ecs_entity_t var_type = elem_type;
                if (i != node->loop_var_count - 1) {
                    if (kind == FlecsScriptForMap && i == 0) {
                        var_type = key_type;
                    } else {
                        var_type = ecs_id(ecs_i32_t);
                    }
                }

                var->value.type = var_type;
                var->type_info = ecs_get_type_info(v->world, var_type);
            }
        }
    }

    v->no_eval ++;
    result = ecs_script_visit_scope(v, node->scope);
    v->no_eval --;

done:
    if (can_type) {
        v->eval->vars = ecs_script_vars_pop(v->eval->vars);
    }

    v->cur = prev_scope;
    v->variable_count = prev_variable_count;

    return result;
}

static int flecs_script_type_with(
    ecs_script_type_visitor_t *v,
    ecs_script_with_t *node)
{
    if (ecs_script_visit_scope(v, node->expressions)) {
        return -1;
    }

    return flecs_script_type_scope_stmts_no_eval(v, node->scope, NULL);
}

static int flecs_script_type_try(
    ecs_script_type_visitor_t *v,
    ecs_script_try_t *node)
{
    if (flecs_script_type_scope_stmts_no_eval(v, node->try_scope, NULL)) {
        return -1;
    }

    int32_t i, count = ecs_vec_count(&node->catches);
    ecs_script_catch_t *catches = ecs_vec_first_t(
        &node->catches, ecs_script_catch_t);
    for (i = 0; i < count; i ++) {
        if (flecs_script_type_scope_stmts_no_eval(v, catches[i].scope, NULL)) {
            return -1;
        }
    }

    return 0;
}

static void flecs_script_type_using_append(
    ecs_script_type_visitor_t *v,
    ecs_entity_t entity)
{
    ecs_allocator_t *a = &v->base.script->allocator;
    ecs_vec_append_t(a, &v->using, ecs_entity_t)[0] = entity;

    if (v->eval) {
        ecs_vec_append_t(&v->eval->r->allocator,
            &v->eval->r->using, ecs_entity_t)[0] = entity;
    }
}

static int flecs_script_type_using(
    ecs_script_type_visitor_t *v,
    ecs_script_using_t *node)
{
    ecs_size_t len = ecs_os_strlen(node->name);

    if (len > 2 && !ecs_os_strcmp(&node->name[len - 2], ".*")) {
        char *path = ecs_os_strdup(node->name);
        path[len - 2] = '\0';

        ecs_entity_t from = ecs_lookup(v->world, path);
        if (!from) {
            flecs_script_visit_error(v, node,
                "unresolved path '%s' in using statement", path);
            ecs_os_free(path);
            return -1;
        }

        ecs_os_free(path);

        ecs_iter_t it = ecs_children(v->world, from);
        while (ecs_children_next(&it)) {
            int32_t i, count = it.count;
            for (i = 0; i < count; i ++) {
                flecs_script_type_using_append(v, it.entities[i]);
            }
        }
    } else {
        ecs_entity_t from = ecs_lookup_path_w_sep(
            v->world, 0, node->name, NULL, NULL, false);
        if (!from) {
            flecs_script_visit_error(v, node,
                "unresolved path '%s' in using statement", node->name);
            return -1;
        }

        flecs_script_type_using_append(v, from);
    }

    return 0;
}

static int flecs_script_type_module(
    ecs_script_type_visitor_t *v,
    ecs_script_module_t *node)
{
    if (!flecs_script_type_can_eval(v)) {
        return 0;
    }

    if (flecs_script_eval_module(v->eval, node)) {
        return -1;
    }

    v->module = v->eval->module;

    return 0;
}

static ecs_entity_t flecs_script_type_var_type(
    ecs_script_type_visitor_t *v,
    ecs_script_var_node_t *var)
{
    if (!var->type) {
        return 0;
    }

    ecs_entity_t type = 0;
    flecs_log_capture_push(true);
    int result = flecs_script_find_entity(
        v->eval, 0, var->type, NULL, NULL, &type, NULL);
    char *err = flecs_log_capture_pop();
    ecs_os_free(err);

    if (result) {
        return 0;
    }

    return type;
}

static int flecs_script_type_prop(
    ecs_script_type_visitor_t *v,
    ecs_script_node_t *node,
    ecs_script_symbol_kind_t kind)
{
    if (flecs_script_type_var(v, node, kind)) {
        return -1;
    }

    if (!flecs_script_type_can_type(v)) {
        return 0;
    }

    ecs_script_var_node_t *var = (ecs_script_var_node_t*)node;

    ecs_entity_t type = flecs_script_type_var_type(v, var);

    int typed = 1;
    if (var->expr && (!var->type || type)) {
        typed = flecs_script_type_expr(v, &var->expr, type, true);
        if (typed == -1) {
            return -1;
        }
    }

    ecs_entity_t var_type = type;
    if (!var_type && !typed && var->expr) {
        var_type = var->expr->type;
    }

    ecs_script_var_t *mirror = ecs_script_vars_declare(
        v->eval->vars, var->name);
    if (mirror && var_type) {
        mirror->value.type = var_type;
        mirror->type_info = ecs_get_type_info(v->world, var_type);
    }

    return 0;
}

static int flecs_script_type_export(
    ecs_script_type_visitor_t *v,
    ecs_script_node_t *node,
    ecs_script_symbol_kind_t kind)
{
    if (flecs_script_type_var(v, node, kind)) {
        return -1;
    }

    ecs_script_var_node_t *var = (ecs_script_var_node_t*)node;

    if (flecs_script_type_can_type(v) && var->expr) {
        ecs_entity_t type = flecs_script_type_var_type(v, var);
        if (!var->type || type) {
            if (flecs_script_type_expr(v, &var->expr, type, true) == -1) {
                return -1;
            }
        }
    }

    if (!flecs_script_type_can_eval(v) || var->is_await) {
        return 0;
    }

    return flecs_script_eval_const(v->eval, var, true);
}

static int flecs_script_type_const(
    ecs_script_type_visitor_t *v,
    ecs_script_node_t *node)
{
    if (flecs_script_type_var(v, node, EcsScriptSymbolConst)) {
        return -1;
    }

    if (!flecs_script_type_can_type(v)) {
        return 0;
    }

    ecs_script_var_node_t *var = (ecs_script_var_node_t*)node;

    ecs_entity_t type = flecs_script_type_var_type(v, var);

    int typed = 1;
    if (var->expr && (!var->type || type)) {
        typed = flecs_script_type_expr(v, &var->expr, type, true);
        if (typed == -1) {
            return -1;
        }
    }

    ecs_entity_t var_type = type;
    if (!var_type && !typed && var->expr) {
        var_type = var->expr->type;
    }

    if (!flecs_script_type_can_eval(v) || var->is_await || !var->expr ||
        var->expr->kind == EcsExprNew || var->expr->kind == EcsExprScript)
    {
        ecs_script_var_t *mirror = ecs_script_vars_declare(
            v->eval->vars, var->name);
        if (mirror && var_type) {
            mirror->value.type = var_type;
            mirror->type_info = ecs_get_type_info(v->world, var_type);
        }

        return 0;
    }

    flecs_log_capture_push(true);
    int result = flecs_script_eval_const(v->eval, var, false);
    char *err = flecs_log_capture_pop();
    ecs_os_free(err);

    if (result) {
        ecs_script_vars_t *vars = v->eval->vars;
        ecs_script_var_t *mirror = NULL;
        bool declared = false;
        if (ecs_vec_count(&vars->vars) &&
            flecs_name_index_is_init(&vars->var_index))
        {
            declared = flecs_name_index_find(
                &vars->var_index, var->name, 0, 0) != 0;
        }
        if (!declared) {
            mirror = ecs_script_vars_declare(vars, var->name);
        } else {
            mirror = ecs_script_vars_lookup(vars, var->name);
        }
        if (mirror && var_type && !mirror->value.type) {
            mirror->value.type = var_type;
            mirror->type_info = ecs_get_type_info(v->world, var_type);
        }
    }

    return 0;
}

static int flecs_script_type_annot(
    ecs_script_type_visitor_t *v,
    ecs_script_annot_t *node)
{
    if (!v->base.next) {
        flecs_script_visit_error(v, node,
            "missing target for @%s annotation", node->name);
        return -1;
    }

    ecs_script_node_kind_t next_kind = v->base.next->kind;
    if (next_kind != EcsAstEntity && next_kind != EcsAstTemplate &&
        next_kind != EcsAstAnnotation)
    {
        flecs_script_visit_error(v, node,
            "target of @%s annotation must be an entity or template",
                node->name);
        return -1;
    }

    if (flecs_script_type_can_eval(v)) {
        ecs_script_runtime_t *r = v->eval->r;
        ecs_vec_append_t(&r->allocator, &r->annot,
            ecs_script_annot_t*)[0] = node;
    }

    return 0;
}

static int flecs_script_type_node(
    ecs_script_visit_t *_v,
    ecs_script_node_t *node)
{
    ecs_script_type_visitor_t *v = (ecs_script_type_visitor_t*)_v;

    switch(node->kind) {
    case EcsAstScope:
        return flecs_script_type_scope(v, (ecs_script_scope_t*)node);
    case EcsAstEntity:
        return flecs_script_type_entity(v, (ecs_script_entity_t*)node);
    case EcsAstTemplate:
        return flecs_script_type_template(
            v, (ecs_script_template_node_t*)node);
    case EcsAstFunction:
        return flecs_script_type_function(
            v, (ecs_script_function_node_t*)node);
    case EcsAstIf:
        return flecs_script_type_if(v, (ecs_script_if_t*)node);
    case EcsAstFor:
        return flecs_script_type_for(v, (ecs_script_for_t*)node);
    case EcsAstWith:
        return flecs_script_type_with(v, (ecs_script_with_t*)node);
    case EcsAstPairScope: {
        ecs_script_pair_scope_t *pair_scope = (ecs_script_pair_scope_t*)node;
        if (flecs_script_type_can_type(v)) {
            if (flecs_script_type_id_name_exprs(v, &pair_scope->id)) {
                return -1;
            }
        }
        return flecs_script_type_scope_stmts_no_eval(v, pair_scope->scope, NULL);
    }
    case EcsAstTry:
        return flecs_script_type_try(v, (ecs_script_try_t*)node);
    case EcsAstUsing:
        return flecs_script_type_using(v, (ecs_script_using_t*)node);
    case EcsAstProp:
        return flecs_script_type_prop(v, node, EcsScriptSymbolProp);
    case EcsAstMut:
        return flecs_script_type_prop(v, node, EcsScriptSymbolMut);
    case EcsAstConst:
        return flecs_script_type_const(v, node);
    case EcsAstExportConst:
        return flecs_script_type_export(v, node, EcsScriptSymbolGlobalConst);
    case EcsAstExportMut:
        return flecs_script_type_export(v, node, EcsScriptSymbolGlobalMut);
    case EcsAstModule:
        return flecs_script_type_module(v, (ecs_script_module_t*)node);
    case EcsAstAnnotation:
        return flecs_script_type_annot(v, (ecs_script_annot_t*)node);
    case EcsAstTag:
    case EcsAstWithTag:
        return flecs_script_type_tag(v, (ecs_script_tag_t*)node);
    case EcsAstComponent:
    case EcsAstWithComponent:
        return flecs_script_type_component(v, (ecs_script_component_t*)node);
    case EcsAstAwait:
        if (flecs_script_type_expr(
            v, &((ecs_script_await_t*)node)->expr, 0, true) == -1)
        {
            return -1;
        }
        return 0;
    case EcsAstVarComponent:
    case EcsAstWithVar:
    case EcsAstInclude:
        return 0;
    }

    ecs_abort(ECS_INTERNAL_ERROR, "corrupt AST node kind");
}

void flecs_script_type_visit_init(
    ecs_script_type_visitor_t *v,
    ecs_world_t *world,
    ecs_script_impl_t *script)
{
    ecs_os_memset_t(v, 0, ecs_script_type_visitor_t);

    v->base.script = script;
    v->world = world;

    ecs_allocator_t *a = &script->allocator;
    ecs_vec_init_t(a, &v->using, ecs_entity_t, 0);
    ecs_vec_init_t(a, &v->symbols, ecs_script_symbol_t, 0);
    ecs_vec_init_t(a, &v->scopes, ecs_script_type_scope_t*, 0);

    v->cur = flecs_script_type_scope_new(v, NULL);
    v->root = v->cur;

    script->type_visitor = v;
}

void flecs_script_type_visit_fini(
    ecs_script_type_visitor_t *v)
{
    ecs_allocator_t *a = &v->base.script->allocator;

    int32_t i, count = ecs_vec_count(&v->scopes);
    ecs_script_type_scope_t **scopes = ecs_vec_first_t(
        &v->scopes, ecs_script_type_scope_t*);
    for (i = 0; i < count; i ++) {
        flecs_name_index_fini(&scopes[i]->symbols);
        flecs_free_t(a, ecs_script_type_scope_t, scopes[i]);
    }

    count = ecs_vec_count(&v->symbols);
    ecs_script_symbol_t *symbols = ecs_vec_first_t(
        &v->symbols, ecs_script_symbol_t);
    for (i = 0; i < count; i ++) {
        ecs_os_free(symbols[i].name);
    }

    ecs_vec_fini_t(a, &v->scopes, ecs_script_type_scope_t*);
    ecs_vec_fini_t(a, &v->symbols, ecs_script_symbol_t);
    ecs_vec_fini_t(a, &v->using, ecs_entity_t);

    v->cur = NULL;

    v->base.script->type_visitor = NULL;
}

int flecs_script_visit_type(
    ecs_script_type_visitor_t *v,
    ecs_script_node_t *node)
{
    ecs_script_eval_visitor_t *eval = v->eval;
    ecs_script_vars_t *prev_vars = NULL;

    if (eval) {
        prev_vars = eval->vars;
        if (eval->vars) {
            eval->vars = flecs_script_vars_push(eval->vars, NULL, NULL);
        } else {
            eval->vars = flecs_script_vars_push(
                NULL, &eval->r->stack, &eval->r->allocator);
        }
        v->variable_count = eval->vars->sp;
    }

    int result = ecs_script_visit_from(v->base.script, v,
        flecs_script_type_node, node, v->base.depth);

    if (eval) {
        eval->vars = ecs_script_vars_pop(eval->vars);
        ecs_assert(eval->vars == prev_vars, ECS_INTERNAL_ERROR, NULL);
        (void)prev_vars;
        ecs_vec_clear(&eval->r->using);
        ecs_vec_clear(&eval->r->annot);
        eval->parent = 0;
        eval->module = 0;
    }

    return result;
}

int flecs_script_type_visit_new_expr(
    ecs_script_t *script,
    ecs_script_entity_t *entity,
    const ecs_expr_eval_desc_t *desc)
{
    ecs_script_impl_t *impl = flecs_script_impl(script);
    ecs_script_type_visitor_t *v = impl->type_visitor;

    if (entity->entity_slot != -1) {
        return 0;
    }

    if (desc->script_visitor &&
        ((ecs_script_eval_visitor_t*)desc->script_visitor)->template)
    {
        return 0;
    }

    if (v) {
        if (v->template_body || !v->eval || !v->base.visit) {
            return 0;
        }

        ecs_script_vars_t *prev_vars = v->eval->vars;
        ecs_script_runtime_t *prev_r = v->eval->r;
        if (desc->vars) {
            v->eval->vars = ECS_CONST_CAST(ecs_script_vars_t*, desc->vars);
        }
        if (desc->runtime) {
            v->eval->r = desc->runtime;
        }

        v->no_eval ++;
        int result = ecs_script_visit_node(v, (ecs_script_node_t*)entity);
        v->no_eval --;

        v->eval->r = prev_r;
        v->eval->vars = prev_vars;

        return result;
    }

    ecs_script_eval_visitor_t eval_visitor;
    ecs_script_eval_desc_t eval_desc = { .runtime = desc->runtime };
    flecs_script_eval_visit_init(impl, &eval_visitor, &eval_desc);
    eval_visitor.vars = ECS_CONST_CAST(ecs_script_vars_t*, desc->vars);

    ecs_script_type_visitor_t type_visitor;
    flecs_script_type_visit_init(&type_visitor, script->world, impl);
    type_visitor.eval = &eval_visitor;
    type_visitor.no_eval = 1;

    int result = flecs_script_visit_type(
        &type_visitor, (ecs_script_node_t*)entity);

    flecs_script_type_visit_fini(&type_visitor);
    flecs_script_eval_visit_fini(&eval_visitor, &eval_desc);

    return result;
}

static const ecs_script_symbol_t* flecs_script_type_lookup_scope(
    const ecs_script_type_visitor_t *v,
    const ecs_script_type_scope_t *scope,
    const char *name)
{
    uint64_t index = flecs_name_index_find(
        &scope->symbols, name, ecs_os_strlen(name), 0);
    if (!index) {
        return NULL;
    }

    return ecs_vec_get_t(&v->symbols, ecs_script_symbol_t,
        flecs_uto(int32_t, index) - 1);
}

static ecs_entity_t flecs_script_type_lookup_world(
    const ecs_script_type_visitor_t *v,
    const char *name)
{
    ecs_entity_t result = ecs_lookup_path_w_sep(
        v->world, FlecsMeta, name, NULL, NULL, false);
    if (result) {
        return result;
    }

    int32_t i, count = ecs_vec_count(&v->using);
    const ecs_entity_t *using = ecs_vec_first_t(&v->using, ecs_entity_t);
    for (i = count - 1; i >= 0; i --) {
        result = ecs_lookup_path_w_sep(
            v->world, using[i], name, NULL, NULL, false);
        if (result) {
            return result;
        }
    }

    if (v->module) {
        result = ecs_lookup_path_w_sep(
            v->world, v->module, name, NULL, NULL, false);
        if (result) {
            return result;
        }
    }

    return ecs_lookup(v->world, name);
}

ecs_script_symbol_t flecs_script_lookup_symbol(
    const ecs_script_type_visitor_t *v,
    const char *name)
{
    ecs_script_symbol_t result = { .kind = EcsScriptSymbolUnresolved };

    const ecs_script_type_scope_t *scope = v->cur;
    while (scope) {
        const ecs_script_symbol_t *symbol =
            flecs_script_type_lookup_scope(v, scope, name);
        if (symbol) {
            return *symbol;
        }

        scope = scope->parent;
    }

    ecs_entity_t e = flecs_script_type_lookup_world(v, name);
    if (e) {
        result.kind = EcsScriptSymbolEntity;
        result.external = true;
        result.is.external = e;
    }

    return result;
}

#endif
