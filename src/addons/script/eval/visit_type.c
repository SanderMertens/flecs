/**
 * @file addons/script/eval/visit_type.c
 * @brief Script type visitor.
 */

#include "flecs.h"

#ifdef FLECS_SCRIPT
#include "../script.h"

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

    int result = ecs_script_visit_scope(v, scope);

    flecs_script_type_scope_clear_vars(v, type_scope, first_symbol);

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
        v->no_eval ++;
        result = ecs_script_visit_scope(v, scope);
        v->no_eval --;
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
    if (!node->kind || !node->kind[0] || node->kind[0] == '$') {
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
    bool tree = node->non_fragmenting_parent ||
        (can_eval && flecs_script_type_has_tree_annot(v));
    bool eager = can_eval && plain_name && !tree;

    node->type_pass = false;

    bool type_pass = eager && flecs_script_type_entity_is_type_pass(v, node);

    ecs_entity_t kind = 0;
    if (eager && !type_pass && node->kind) {
        if (!node->kind[0] || node->kind[0] == '$' ||
            !ecs_os_strcmp(node->kind, "this"))
        {
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

        node->entity_slot = symbol->is.entity;
        scope = symbol->scope;
    } else {
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
    eval->vars = flecs_script_vars_push(
        eval->vars, &eval->r->stack, &eval->r->allocator);

    int result = flecs_script_type_scope_stmts(v, node->scope, scope);

    eval->vars = ecs_script_vars_pop(eval->vars);
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
    result = ecs_script_visit_scope(v, node->body);
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
    ecs_script_type_scope_t *prev_scope = v->cur;
    int32_t prev_variable_count = v->variable_count;

    v->cur = flecs_script_type_scope_new(v, prev_scope);

    int result = 0;
    int32_t i;
    for (i = 0; i < node->loop_var_count; i ++) {
        if (flecs_script_type_declare_var(
            v, &node->node, node->loop_vars[i], EcsScriptSymbolConst))
        {
            result = -1;
            goto done;
        }
    }

    v->no_eval ++;
    result = ecs_script_visit_scope(v, node->scope);
    v->no_eval --;

done:
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

static int flecs_script_type_export(
    ecs_script_type_visitor_t *v,
    ecs_script_node_t *node,
    ecs_script_symbol_kind_t kind)
{
    if (flecs_script_type_var(v, node, kind)) {
        return -1;
    }

    ecs_script_var_node_t *var = (ecs_script_var_node_t*)node;
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

    if (!flecs_script_type_can_eval(v)) {
        return 0;
    }

    ecs_script_var_node_t *var = (ecs_script_var_node_t*)node;
    if (var->is_await || !var->expr || var->expr->kind == EcsExprNew ||
        var->expr->kind == EcsExprScript)
    {
        ecs_script_vars_declare(v->eval->vars, var->name);
        return 0;
    }

    flecs_log_capture_push(true);
    int result = flecs_script_eval_const(v->eval, var, false);
    char *err = flecs_log_capture_pop();

    if (result) {
        ecs_os_free(err);
        ecs_script_vars_t *vars = v->eval->vars;
        bool declared = false;
        if (ecs_vec_count(&vars->vars) &&
            flecs_name_index_is_init(&vars->var_index))
        {
            declared = flecs_name_index_find(
                &vars->var_index, var->name, 0, 0) != 0;
        }
        if (!declared) {
            ecs_script_vars_declare(vars, var->name);
        }
    }

    return 0;
}

static int flecs_script_type_annot(
    ecs_script_type_visitor_t *v,
    ecs_script_annot_t *node)
{
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
        return flecs_script_type_scope_stmts_no_eval(v, pair_scope->scope, NULL);
    }
    case EcsAstTry:
        return flecs_script_type_try(v, (ecs_script_try_t*)node);
    case EcsAstUsing:
        return flecs_script_type_using(v, (ecs_script_using_t*)node);
    case EcsAstProp:
        return flecs_script_type_var(v, node, EcsScriptSymbolProp);
    case EcsAstMut:
        return flecs_script_type_var(v, node, EcsScriptSymbolMut);
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
    case EcsAstComponent:
    case EcsAstWithComponent:
    case EcsAstAwait:
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
        eval->vars = flecs_script_vars_push(
            eval->vars, &eval->r->stack, &eval->r->allocator);
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
