/**
 * @file addons/script/visit_refs.c
 * @brief Visitor to statically collect references made by a script.
 */

#include "flecs.h"

#ifdef FLECS_SCRIPT
#include "script.h"

typedef struct ecs_script_refs_var_t {
    const char *name;
    const char *entity;
} ecs_script_refs_var_t;

typedef struct ecs_script_refs_visitor_t {
    ecs_script_impl_t *script;
    ecs_vec_t *refs;
    ecs_vec_t *fn_refs;
    ecs_vec_t *usings;
    ecs_vec_t vars;        /* vec<ecs_script_refs_var_t> */
    ecs_vec_t entities;    /* vec<const char*> */
    ecs_vec_t scope_names; /* vec<const char*> */
    ecs_vec_t fn_nodes;    /* vec<ecs_script_function_node_t*> */
    int32_t dynamic_scope;
    bool template_scope;
} ecs_script_refs_visitor_t;

static
int flecs_script_refs_visit_node(
    ecs_script_refs_visitor_t *v,
    ecs_script_node_t *node);

static
int flecs_script_refs_visit_expr(
    ecs_script_refs_visitor_t *v,
    ecs_expr_node_t *node);

static
int flecs_script_refs_visit_scope(
    ecs_script_refs_visitor_t *v,
    ecs_script_scope_t *scope);

static
bool flecs_script_refs_str_eq(
    const char *a,
    const char *b)
{
    if (a == b) {
        return true;
    }
    if (!a || !b) {
        return false;
    }
    return !ecs_os_strcmp(a, b);
}

static
bool flecs_script_refs_is_declared(
    ecs_script_refs_visitor_t *v,
    const char *name)
{
    const char *sep = strchr(name, '.');
    ecs_size_t len = sep
        ? flecs_ito(ecs_size_t, sep - name)
        : ecs_os_strlen(name);

    int32_t i, count = ecs_vec_count(&v->entities);
    const char **entities = ecs_vec_first(&v->entities);
    for (i = 0; i < count; i ++) {
        if (!ecs_os_strncmp(entities[i], name, len) && !entities[i][len]) {
            return true;
        }
    }

    return false;
}

static
ecs_script_refs_var_t* flecs_script_refs_find_var(
    ecs_script_refs_visitor_t *v,
    const char *name)
{
    int32_t i, count = ecs_vec_count(&v->vars);
    ecs_script_refs_var_t *vars = ecs_vec_first(&v->vars);
    for (i = count - 1; i >= 0; i --) {
        if (!ecs_os_strcmp(vars[i].name, name)) {
            return &vars[i];
        }
    }
    return NULL;
}

static
void flecs_script_refs_declare_var(
    ecs_script_refs_visitor_t *v,
    const char *name,
    const char *entity)
{
    ecs_script_refs_var_t *var = ecs_vec_append_t(
        NULL, &v->vars, ecs_script_refs_var_t);
    var->name = name;
    var->entity = entity;
}

static
char* flecs_script_refs_scope_path(
    ecs_script_refs_visitor_t *v)
{
    int32_t i, count = ecs_vec_count(&v->scope_names);
    if (!count) {
        return NULL;
    }

    ecs_strbuf_t buf = ECS_STRBUF_INIT;
    const char **names = ecs_vec_first(&v->scope_names);
    for (i = 0; i < count; i ++) {
        if (i) {
            ecs_strbuf_appendch(&buf, '.');
        }
        ecs_strbuf_appendstr(&buf, names[i]);
    }

    return ecs_strbuf_get(&buf);
}

static
void flecs_script_refs_add(
    ecs_script_refs_visitor_t *v,
    const char *entity,
    const char *var,
    const char *component,
    ecs_id_t component_id)
{
    if (entity && flecs_script_refs_is_declared(v, entity)) {
        return;
    }

    bool dynamic = v->template_scope || (v->dynamic_scope != 0);
    if (component_id == ecs_id(EcsScriptConstVar)) {
        /* Global const variable names resolve the same way regardless of the
         * scope the reference was made from. */
        dynamic = false;
    }

    char *scope = dynamic ? NULL : flecs_script_refs_scope_path(v);

    int32_t i, count = ecs_vec_count(v->refs);
    ecs_script_requirement_t *refs = ecs_vec_first(v->refs);
    for (i = 0; i < count; i ++) {
        ecs_script_requirement_t *ref = &refs[i];
        if (ref->component_id == component_id &&
            flecs_script_refs_str_eq(ref->entity, entity) &&
            flecs_script_refs_str_eq(ref->var, var) &&
            flecs_script_refs_str_eq(ref->scope, scope) &&
            flecs_script_refs_str_eq(ref->component, component))
        {
            ecs_os_free(scope);
            return;
        }
    }

    ecs_script_requirement_t *ref = ecs_vec_append_t(
        NULL, v->refs, ecs_script_requirement_t);
    ref->entity = entity ? ecs_os_strdup(entity) : NULL;
    ref->var = var ? ecs_os_strdup(var) : NULL;
    ref->scope = scope;
    ref->component = component ? ecs_os_strdup(component) : NULL;
    ref->component_id = component_id;
    ref->dynamic_scope = dynamic;
}

static
void flecs_script_refs_visit_var(
    ecs_script_refs_visitor_t *v,
    const char *name)
{
    if (!name || !name[0]) {
        /* Member shorthand notation ({x: $}), resolves to a local variable */
        return;
    }

    if (!ecs_os_strcmp(name, "this")) {
        return;
    }

    if (flecs_script_refs_find_var(v, name)) {
        return;
    }

    /* Variable is not declared in the script, so it refers to a global
     * const variable. */
    flecs_script_refs_add(v, name, NULL, NULL, ecs_id(EcsScriptConstVar));
}

static
void flecs_script_refs_visit_element(
    ecs_script_refs_visitor_t *v,
    ecs_expr_element_t *node)
{
    if (!node->index || node->index->kind != EcsExprIdentifier) {
        return;
    }

    const char *component = ((ecs_expr_identifier_t*)node->index)->value;
    ecs_expr_node_t *left = node->left;
    if (!left) {
        return;
    }

    if (left->kind == EcsExprIdentifier) {
        const char *entity = ((ecs_expr_identifier_t*)left)->value;
        if (!ecs_os_strcmp(entity, "this")) {
            if (v->template_scope) {
                flecs_script_refs_add(v, NULL, "this", component, 0);
            }
        } else {
            flecs_script_refs_add(v, entity, NULL, component, 0);
        }
    } else if (left->kind == EcsExprVariable) {
        const char *var_name = ((ecs_expr_variable_t*)left)->name;
        if (!ecs_os_strcmp(var_name, "this")) {
            if (v->template_scope) {
                flecs_script_refs_add(v, NULL, "this", component, 0);
            }
            return;
        }

        ecs_script_refs_var_t *var = flecs_script_refs_find_var(v, var_name);
        if (var && var->entity) {
            flecs_script_refs_add(v, var->entity, NULL, component, 0);
        } else {
            flecs_script_refs_add(v, NULL, var_name, component, 0);
        }
    } else {
        flecs_script_refs_add(v, NULL, NULL, component, 0);
    }
}

static
int flecs_script_refs_visit_fn_body(
    ecs_script_refs_visitor_t *v,
    ecs_script_function_node_t *node)
{
    int32_t var_count = ecs_vec_count(&v->vars);
    int32_t i, count = ecs_vec_count(&node->params);
    ecs_script_fn_param_t *params = ecs_vec_first(&node->params);
    for (i = 0; i < count; i ++) {
        flecs_script_refs_declare_var(v, params[i].name, NULL);
    }
    int result = flecs_script_refs_visit_scope(v, node->body);
    if (!result) {
        result = flecs_script_refs_visit_expr(v, node->return_expr);
    }
    ecs_vec_set_count_t(NULL, &v->vars, ecs_script_refs_var_t, var_count);
    return result;
}

static
void flecs_script_refs_visit_fn_call(
    ecs_script_refs_visitor_t *v,
    ecs_expr_function_t *node)
{
    const char *name = node->function_name;
    if (!name && node->left && node->left->kind == EcsExprIdentifier) {
        name = ((ecs_expr_identifier_t*)node->left)->value;
    }
    if (!name) {
        return;
    }

    int32_t i, count = ecs_vec_count(&v->fn_nodes);
    ecs_script_function_node_t **fn_nodes = ecs_vec_first(&v->fn_nodes);
    for (i = count - 1; i >= 0; i --) {
        ecs_script_function_node_t *fn_node = fn_nodes[i];
        if (fn_node && fn_node->name &&
            !ecs_os_strcmp(fn_node->name, name))
        {
            fn_nodes[i] = NULL;
            flecs_script_refs_visit_fn_body(v, fn_node);
            ecs_vec_get_t(&v->fn_nodes,
                ecs_script_function_node_t*, i)[0] = fn_node;
            return;
        }
    }

    ecs_world_t *world = v->script->pub.world;
    ecs_entity_t fn = ecs_lookup(world, name);
    if (!fn) {
        return;
    }

    const EcsScriptFunction *f = ecs_get(world, fn, EcsScriptFunction);
    if (!f || f->callback != flecs_script_user_function_callback ||
        !f->binding_ctx)
    {
        return;
    }

    ecs_script_user_function_t *uf = f->binding_ctx;
    ecs_vec_t *prev_refs = v->refs;
    v->refs = v->fn_refs;
    count = ecs_vec_count(&uf->refs);
    ecs_script_requirement_t *refs = ecs_vec_first(&uf->refs);
    for (i = 0; i < count; i ++) {
        ecs_script_requirement_t *ref = &refs[i];
        if (ref->component_id == ecs_id(EcsScriptConstVar) && ref->entity &&
            flecs_script_refs_find_var(v, ref->entity))
        {
            continue;
        }
        flecs_script_refs_add(v, ref->entity, ref->var, ref->component,
            ref->component_id);
    }
    v->refs = prev_refs;
}

static
int flecs_script_refs_visit_expr(
    ecs_script_refs_visitor_t *v,
    ecs_expr_node_t *node)
{
    if (!node) {
        return 0;
    }

    switch(node->kind) {
    case EcsExprValue:
    case EcsExprIdentifier:
        break;
    case EcsExprVariable:
    case EcsExprGlobalVariable:
        flecs_script_refs_visit_var(v, ((ecs_expr_variable_t*)node)->name);
        break;
    case EcsExprInterpolatedString: {
        ecs_expr_interpolated_string_t *n =
            (ecs_expr_interpolated_string_t*)node;
        int32_t i, count = ecs_vec_count(&n->expressions);
        ecs_expr_node_t **expressions = ecs_vec_first(&n->expressions);
        for (i = 0; i < count; i ++) {
            if (flecs_script_refs_visit_expr(v, expressions[i])) {
                return -1;
            }
        }
        break;
    }
    case EcsExprInitializer:
    case EcsExprEmptyInitializer: {
        ecs_expr_initializer_t *n = (ecs_expr_initializer_t*)node;
        ecs_expr_initializer_element_t *elems = ecs_vec_first(&n->elements);
        int32_t i, count = ecs_vec_count(&n->elements);
        for (i = 0; i < count; i ++) {
            if (flecs_script_refs_visit_expr(v, elems[i].key)) {
                return -1;
            }
            if (flecs_script_refs_visit_expr(v, elems[i].value)) {
                return -1;
            }
        }
        break;
    }
    case EcsExprUnary:
        return flecs_script_refs_visit_expr(
            v, ((ecs_expr_unary_t*)node)->expr);
    case EcsExprBinary: {
        ecs_expr_binary_t *n = (ecs_expr_binary_t*)node;
        if (flecs_script_refs_visit_expr(v, n->left)) {
            return -1;
        }
        return flecs_script_refs_visit_expr(v, n->right);
    }
    case EcsExprFunction:
    case EcsExprMethod: {
        ecs_expr_function_t *n = (ecs_expr_function_t*)node;
        if (flecs_script_refs_visit_expr(v, n->left)) {
            return -1;
        }
        if (flecs_script_refs_visit_expr(v, (ecs_expr_node_t*)n->args)) {
            return -1;
        }
        if (node->kind == EcsExprFunction) {
            flecs_script_refs_visit_fn_call(v, n);
        }
        break;
    }
    case EcsExprMember:
        return flecs_script_refs_visit_expr(
            v, ((ecs_expr_member_t*)node)->left);
    case EcsExprElement: {
        ecs_expr_element_t *n = (ecs_expr_element_t*)node;
        if (flecs_script_refs_visit_expr(v, n->left)) {
            return -1;
        }
        if (flecs_script_refs_visit_expr(v, n->index)) {
            return -1;
        }
        flecs_script_refs_visit_element(v, n);
        break;
    }
    case EcsExprComponent:
        return flecs_script_refs_visit_expr(
            v, ((ecs_expr_component_t*)node)->expr);
    case EcsExprCast:
    case EcsExprCastNumber:
        return flecs_script_refs_visit_expr(
            v, ((ecs_expr_cast_t*)node)->expr);
    case EcsExprMatch: {
        ecs_expr_match_t *n = (ecs_expr_match_t*)node;
        if (flecs_script_refs_visit_expr(v, n->expr)) {
            return -1;
        }
        int32_t i, count = ecs_vec_count(&n->elements);
        ecs_expr_match_element_t *elems = ecs_vec_first(&n->elements);
        for (i = 0; i < count; i ++) {
            if (flecs_script_refs_visit_expr(v, elems[i].compare)) {
                return -1;
            }
            if (flecs_script_refs_visit_expr(v, elems[i].expr)) {
                return -1;
            }
        }
        if (flecs_script_refs_visit_expr(v, n->any.compare)) {
            return -1;
        }
        return flecs_script_refs_visit_expr(v, n->any.expr);
    }
    case EcsExprNew: {
        ecs_expr_new_t *n = (ecs_expr_new_t*)node;
        if (n->entity) {
            v->dynamic_scope ++;
            int result = flecs_script_refs_visit_node(
                v, (ecs_script_node_t*)n->entity);
            v->dynamic_scope --;
            return result;
        }
        break;
    }
    }

    return 0;
}

static
int flecs_script_refs_visit_scope(
    ecs_script_refs_visitor_t *v,
    ecs_script_scope_t *scope)
{
    int32_t var_count = ecs_vec_count(&v->vars);

    int32_t i, count = ecs_vec_count(&scope->stmts);
    ecs_script_node_t **stmts = ecs_vec_first(&scope->stmts);
    for (i = 0; i < count; i ++) {
        if (flecs_script_refs_visit_node(v, stmts[i])) {
            return -1;
        }
    }

    ecs_vec_set_count_t(NULL, &v->vars, ecs_script_refs_var_t, var_count);

    return 0;
}

static
int flecs_script_refs_visit_entity(
    ecs_script_refs_visitor_t *v,
    ecs_script_entity_t *node)
{
    bool static_name = node->name && !node->name_is_var && !node->name_expr;

    if (static_name) {
        ecs_vec_append_t(NULL, &v->scope_names, const char*)[0] = node->name;
    } else {
        v->dynamic_scope ++;
    }

    int result = 0;
    if (node->scope) {
        result = flecs_script_refs_visit_scope(v, node->scope);
    }

    if (static_name) {
        ecs_vec_remove_last(&v->scope_names);
    } else {
        v->dynamic_scope --;
    }

    return result;
}

static
int flecs_script_refs_visit_node(
    ecs_script_refs_visitor_t *v,
    ecs_script_node_t *node)
{
    switch(node->kind) {
    case EcsAstScope:
        return flecs_script_refs_visit_scope(v, (ecs_script_scope_t*)node);
    case EcsAstTag:
    case EcsAstAnnotation:
    case EcsAstInclude:
    case EcsAstWithTag:
        break;
    case EcsAstComponent:
    case EcsAstWithComponent:
        return flecs_script_refs_visit_expr(
            v, ((ecs_script_component_t*)node)->expr);
    case EcsAstDefaultComponent:
        return flecs_script_refs_visit_expr(
            v, ((ecs_script_default_component_t*)node)->expr);
    case EcsAstVarComponent:
    case EcsAstWithVar:
        flecs_script_refs_visit_var(
            v, ((ecs_script_var_component_t*)node)->name);
        break;
    case EcsAstWith: {
        ecs_script_with_t *n = (ecs_script_with_t*)node;
        if (flecs_script_refs_visit_scope(v, n->expressions)) {
            return -1;
        }
        return flecs_script_refs_visit_scope(v, n->scope);
    }
    case EcsAstUsing:
        if (v->usings) {
            ecs_vec_append_t(NULL, v->usings, char*)[0] =
                ecs_os_strdup(((ecs_script_using_t*)node)->name);
        }
        break;
    case EcsAstModule:
        ecs_vec_append_t(NULL, &v->scope_names, const char*)[0] =
            ((ecs_script_module_t*)node)->name;
        break;
    case EcsAstTemplate:
        /* References made in template bodies don't gate script evaluation.
         * They are collected separately when the template entity is created
         * (see flecs_script_eval_template). */
        break;
    case EcsAstProp: {
        ecs_script_var_node_t *n = (ecs_script_var_node_t*)node;
        if (flecs_script_refs_visit_expr(v, n->expr)) {
            return -1;
        }
        flecs_script_refs_declare_var(v, n->name, NULL);
        break;
    }
    case EcsAstConst:
    case EcsAstExportConst: {
        ecs_script_var_node_t *n = (ecs_script_var_node_t*)node;
        if (flecs_script_refs_visit_expr(v, n->expr)) {
            return -1;
        }
        const char *entity = NULL;
        if (n->expr && n->expr->kind == EcsExprIdentifier) {
            entity = ((ecs_expr_identifier_t*)n->expr)->value;
        }
        flecs_script_refs_declare_var(v, n->name, entity);
        break;
    }
    case EcsAstEntity:
        return flecs_script_refs_visit_entity(v, (ecs_script_entity_t*)node);
    case EcsAstPairScope:
        return flecs_script_refs_visit_scope(
            v, ((ecs_script_pair_scope_t*)node)->scope);
    case EcsAstIf: {
        ecs_script_if_t *n = (ecs_script_if_t*)node;
        if (flecs_script_refs_visit_expr(v, n->expr)) {
            return -1;
        }
        if (n->if_true && flecs_script_refs_visit_scope(v, n->if_true)) {
            return -1;
        }
        if (n->if_false && flecs_script_refs_visit_scope(v, n->if_false)) {
            return -1;
        }
        break;
    }
    case EcsAstFor: {
        ecs_script_for_range_t *n = (ecs_script_for_range_t*)node;
        if (flecs_script_refs_visit_expr(v, n->from)) {
            return -1;
        }
        if (flecs_script_refs_visit_expr(v, n->to)) {
            return -1;
        }
        int32_t var_count = ecs_vec_count(&v->vars);
        flecs_script_refs_declare_var(v, n->loop_var, NULL);
        int result = flecs_script_refs_visit_scope(v, n->scope);
        ecs_vec_set_count_t(NULL, &v->vars, ecs_script_refs_var_t, var_count);
        return result;
    }
    case EcsAstFunction:
        ecs_vec_append_t(NULL, &v->fn_nodes, ecs_script_function_node_t*)[0] =
            (ecs_script_function_node_t*)node;
        break;
    }

    return 0;
}

static
void flecs_script_refs_collect_entities(
    ecs_script_refs_visitor_t *v,
    ecs_script_scope_t *scope)
{
    int32_t i, count = ecs_vec_count(&scope->stmts);
    ecs_script_node_t **stmts = ecs_vec_first(&scope->stmts);
    for (i = 0; i < count; i ++) {
        ecs_script_node_t *node = stmts[i];
        switch(node->kind) {
        case EcsAstEntity: {
            ecs_script_entity_t *entity = (ecs_script_entity_t*)node;
            if (entity->name && !entity->name_is_var && !entity->name_expr) {
                ecs_vec_append_t(NULL, &v->entities, const char*)[0] =
                    entity->name;
            }
            if (entity->scope) {
                flecs_script_refs_collect_entities(v, entity->scope);
            }
            break;
        }
        case EcsAstTemplate: {
            ecs_script_template_node_t *template =
                (ecs_script_template_node_t*)node;
            if (template->name) {
                ecs_vec_append_t(NULL, &v->entities, const char*)[0] =
                    template->name;
            }
            break;
        }
        case EcsAstWith:
            flecs_script_refs_collect_entities(
                v, ((ecs_script_with_t*)node)->scope);
            break;
        case EcsAstPairScope:
            flecs_script_refs_collect_entities(
                v, ((ecs_script_pair_scope_t*)node)->scope);
            break;
        case EcsAstIf: {
            ecs_script_if_t *n = (ecs_script_if_t*)node;
            if (n->if_true) {
                flecs_script_refs_collect_entities(v, n->if_true);
            }
            if (n->if_false) {
                flecs_script_refs_collect_entities(v, n->if_false);
            }
            break;
        }
        case EcsAstFor:
            flecs_script_refs_collect_entities(
                v, ((ecs_script_for_range_t*)node)->scope);
            break;
        case EcsAstScope:
            flecs_script_refs_collect_entities(v, (ecs_script_scope_t*)node);
            break;
        case EcsAstTag:
        case EcsAstComponent:
        case EcsAstDefaultComponent:
        case EcsAstVarComponent:
        case EcsAstWithVar:
        case EcsAstWithTag:
        case EcsAstWithComponent:
        case EcsAstUsing:
        case EcsAstModule:
        case EcsAstAnnotation:
        case EcsAstProp:
        case EcsAstConst:
        case EcsAstExportConst:
        case EcsAstInclude:
        case EcsAstFunction:
            break;
        }
    }
}

static
void flecs_script_refs_declare_outer_vars(
    ecs_script_refs_visitor_t *v,
    const ecs_script_vars_t *vars)
{
    if (!vars) {
        return;
    }

    flecs_script_refs_declare_outer_vars(v, vars->parent);

    int32_t i, count = ecs_vec_count(&vars->vars);
    const ecs_script_var_t *elems = ecs_vec_first(&vars->vars);
    for (i = 0; i < count; i ++) {
        if (elems[i].name) {
            flecs_script_refs_declare_var(v, elems[i].name, NULL);
        }
    }
}

int flecs_script_visit_refs(
    ecs_script_impl_t *script,
    ecs_script_scope_t *scope,
    bool template_scope,
    const ecs_script_vars_t *vars,
    ecs_vec_t *refs,
    ecs_vec_t *usings,
    ecs_vec_t *fn_refs)
{
    ecs_script_refs_visitor_t v = {
        .script = script,
        .refs = refs,
        .fn_refs = fn_refs ? fn_refs : refs,
        .usings = usings,
        .template_scope = template_scope
    };

    ecs_vec_init_t(NULL, &v.vars, ecs_script_refs_var_t, 0);
    ecs_vec_init_t(NULL, &v.entities, const char*, 0);
    ecs_vec_init_t(NULL, &v.scope_names, const char*, 0);
    ecs_vec_init_t(NULL, &v.fn_nodes, ecs_script_function_node_t*, 0);

    flecs_script_refs_collect_entities(&v, scope);

    flecs_script_refs_declare_outer_vars(&v, vars);

    if (template_scope) {
        flecs_script_refs_declare_var(&v, "this", NULL);
    }

    int result = flecs_script_refs_visit_scope(&v, scope);

    ecs_vec_fini_t(NULL, &v.vars, ecs_script_refs_var_t);
    ecs_vec_fini_t(NULL, &v.entities, const char*);
    ecs_vec_fini_t(NULL, &v.scope_names, const char*);
    ecs_vec_fini_t(NULL, &v.fn_nodes, ecs_script_function_node_t*);

    return result;
}

int flecs_script_visit_fn_refs(
    ecs_script_impl_t *script,
    ecs_script_function_node_t *node,
    ecs_vec_t *refs)
{
    ecs_script_refs_visitor_t v = {
        .script = script,
        .refs = refs,
        .fn_refs = refs
    };

    ecs_vec_init_t(NULL, &v.vars, ecs_script_refs_var_t, 0);
    ecs_vec_init_t(NULL, &v.entities, const char*, 0);
    ecs_vec_init_t(NULL, &v.scope_names, const char*, 0);
    ecs_vec_init_t(NULL, &v.fn_nodes, ecs_script_function_node_t*, 0);

    int result = flecs_script_refs_visit_fn_body(&v, node);

    ecs_vec_fini_t(NULL, &v.vars, ecs_script_refs_var_t);
    ecs_vec_fini_t(NULL, &v.entities, const char*);
    ecs_vec_fini_t(NULL, &v.scope_names, const char*);
    ecs_vec_fini_t(NULL, &v.fn_nodes, ecs_script_function_node_t*);

    return result;
}

#endif
