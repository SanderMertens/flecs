#include "flecs.h"

#ifdef FLECS_SCRIPT
#include "../script.h"

typedef struct flecs_script_component_owner_t {
    ecs_script_entity_t *entity;
    ecs_script_id_t *id;
    ecs_script_scope_t *scope;
    int32_t entity_symbol;
} flecs_script_component_owner_t;

typedef struct flecs_script_dep_var_t {
    uint64_t input;
    uint64_t internal;
} flecs_script_dep_var_t;

typedef struct flecs_script_dep_ctx_t {
    ecs_script_eval_visitor_t *v;
    ecs_script_template_t *template;
    ecs_vec_t *refs;
    ecs_vec_t *dynamic_refs;
    ecs_vec_t vars;
    ecs_vec_t expr_refs;
    ecs_vec_t expr_dynamic_refs;
    ecs_vec_t expr_dyn_nodes;
    ecs_vec_t component_owners;
    int32_t *input_count;
    int32_t scope_count;
    int32_t component_count;
    int32_t for_count;
    int32_t member;
    int32_t entity_symbol;
    bool no_deps;
    bool no_computed;
    int32_t conditional;
    ecs_script_entity_t *entity;
    ecs_script_scope_t *scope;
} flecs_script_dep_ctx_t;

static void flecs_script_dep_fini(
    flecs_script_dep_ctx_t *ctx)
{
    ecs_vec_fini_t(NULL, &ctx->vars, flecs_script_dep_var_t);
    ecs_vec_fini_t(NULL, &ctx->expr_refs, ecs_script_ref_t);
    ecs_vec_fini_t(NULL, &ctx->expr_dynamic_refs, ecs_script_ref_t);
    ecs_vec_fini_t(NULL, &ctx->expr_dyn_nodes, ecs_expr_node_t*);
    ecs_vec_fini_t(NULL, &ctx->component_owners, flecs_script_component_owner_t);
}

static int flecs_script_dep_node(
    flecs_script_dep_ctx_t *ctx,
    ecs_script_node_t *node);

static int flecs_script_dep_scope(
    flecs_script_dep_ctx_t *ctx,
    ecs_script_scope_t *scope);

static bool flecs_script_dep_scope_contains(
    ecs_script_scope_t *scope,
    ecs_script_scope_t *contains)
{
    while (scope) {
        if (scope == contains) {
            return true;
        }
        scope = scope->parent;
    }
    return false;
}

static bool flecs_script_dep_scopes_exclusive(
    ecs_script_scope_t *first,
    ecs_script_scope_t *second)
{
    while (first) {
        if (first->alternative && flecs_script_dep_scope_contains(
            second, first->alternative))
        {
            return true;
        }
        first = first->parent;
    }
    return false;
}

static bool flecs_script_dep_id_elem_static(
    ecs_expr_node_t *expr,
    int32_t symbol,
    int32_t sp)
{
    return !expr && symbol == -1 && sp == -1;
}

static bool flecs_script_dep_ids_may_match(
    const ecs_script_id_t *first,
    const ecs_script_id_t *second)
{
    if (first->flag != second->flag ||
        (first->second != NULL) != (second->second != NULL))
    {
        return false;
    }
    if (!first->dynamic && !second->dynamic &&
        first->eval && second->eval)
    {
        return first->eval == second->eval;
    }
    if (flecs_script_dep_id_elem_static(
            first->first_expr, first->first_symbol, first->first_sp) &&
        flecs_script_dep_id_elem_static(
            second->first_expr, second->first_symbol, second->first_sp) &&
        first->first_eval && second->first_eval &&
        first->first_eval != second->first_eval)
    {
        return false;
    }
    if (first->second && flecs_script_dep_id_elem_static(
            first->second_expr, first->second_symbol, first->second_sp) &&
        flecs_script_dep_id_elem_static(
            second->second_expr, second->second_symbol, second->second_sp) &&
        first->second_eval && second->second_eval &&
        first->second_eval != second->second_eval)
    {
        return false;
    }
    return true;
}

static bool flecs_script_dep_names_may_match(
    const char *first,
    const char *second)
{
    while (*first && *second && *first == *second &&
        *first != '{' && *second != '{')
    {
        first ++;
        second ++;
    }
    return *first == '{' || *second == '{';
}

static bool flecs_script_dep_entity_may_match(
    ecs_script_entity_t *first,
    ecs_script_entity_t *second)
{
    if (first == second) {
        return true;
    }
    if (!first || !second) {
        return false;
    }
    if (first->symbol != -1 && first->symbol == second->symbol) {
        return true;
    }
    if (!first->name || !second->name) {
        return false;
    }
    if (ecs_os_strcmp(first->name, second->name) &&
        !flecs_script_dep_names_may_match(first->name, second->name))
    {
        return false;
    }
    return flecs_script_dep_entity_may_match(first->parent, second->parent);
}

static bool flecs_script_dep_entity_matches(
    ecs_script_entity_t *first,
    ecs_script_entity_t *second)
{
    if (first == second) {
        return true;
    }
    if (!first || !second) {
        return false;
    }
    if (first->symbol != -1 && first->symbol == second->symbol) {
        return true;
    }
    if (!first->name || !second->name ||
        first->name_expr || second->name_expr ||
        ecs_os_strcmp(first->name, second->name))
    {
        return false;
    }
    return flecs_script_dep_entity_matches(first->parent, second->parent);
}

static bool flecs_script_dep_same_entity(
    const flecs_script_component_owner_t *owner,
    const flecs_script_dep_ctx_t *ctx)
{
    if (owner->entity || ctx->entity) {
        return flecs_script_dep_entity_may_match(
            owner->entity, ctx->entity);
    }
    return owner->entity_symbol == ctx->entity_symbol;
}

static bool flecs_script_dep_component_already_owned(
    const flecs_script_component_owner_t *owner,
    const flecs_script_dep_ctx_t *ctx)
{
    ecs_script_entity_t *first_entity = owner->entity;
    ecs_script_entity_t *second_entity = ctx->entity;
    if (!flecs_script_dep_entity_matches(
        first_entity, second_entity))
    {
        return false;
    }

    ecs_script_scope_t *first_scope = owner->scope;
    ecs_script_scope_t *second_scope = ctx->scope;
    while (first_entity && second_entity &&
        first_scope == first_entity->scope &&
        second_scope == second_entity->scope)
    {
        first_scope = first_scope->parent;
        second_scope = second_scope->parent;
        first_entity = first_entity->parent;
        second_entity = second_entity->parent;
    }
    return first_scope == second_scope;
}

static bool flecs_script_dep_initializer_is_complete(
    const ecs_world_t *world,
    ecs_entity_t component,
    const ecs_expr_initializer_t *initializer)
{
    if (!component) {
        return false;
    }

    ecs_entity_t type = ecs_get_typeid(world, component);
    if (!type) {
        return false;
    }

    const EcsStruct *st = ecs_get(world, type, EcsStruct);
    if (!st) {
        return false;
    }

    const ecs_expr_initializer_element_t *elements = ecs_vec_first(
        &initializer->elements);
    int32_t i, count = ecs_vec_count(&initializer->elements);
    if (count != ecs_vec_count(&st->members)) {
        return false;
    }

    for (i = 0; i < count; i ++) {
        if (!elements[i].member) {
            return false;
        }
        const ecs_expr_node_t *value = elements[i].value;
        if (value && value->kind == EcsExprInitializer &&
            ((const ecs_expr_initializer_t*)value)->is_partial)
        {
            return false;
        }
    }

    return true;
}

static int flecs_script_dep_component_owner(
    flecs_script_dep_ctx_t *ctx,
    ecs_script_node_t *node,
    ecs_script_id_t *id,
    bool *already_owned)
{
    *already_owned = false;
    if (!ctx->v->base.script->entity) {
        return 0;
    }
    flecs_script_component_owner_t *owners = ecs_vec_first(
        &ctx->component_owners);
    int32_t i, count = ecs_vec_count(&ctx->component_owners);
    for (i = 0; i < count; i ++) {
        flecs_script_component_owner_t *owner = &owners[i];
        if (!flecs_script_dep_same_entity(owner, ctx) ||
            !flecs_script_dep_ids_may_match(owner->id, id))
        {
            continue;
        }
        if (owner->scope == ctx->scope ||
            flecs_script_dep_component_already_owned(owner, ctx))
        {
            *already_owned = true;
            return 0;
        }
        if (flecs_script_dep_scopes_exclusive(owner->scope, ctx->scope)) {
            continue;
        }
        flecs_script_eval_error(ctx->v, node,
            "component can only be created in one scope or "
            "mutually exclusive scopes (use if/else branches, or assign "
            "individual members)");
        return -1;
    }
    flecs_script_component_owner_t *owner = ecs_vec_append_t(
        NULL, &ctx->component_owners, flecs_script_component_owner_t);
    owner->entity = ctx->entity;
    owner->entity_symbol = ctx->entity_symbol;
    owner->id = id;
    owner->scope = ctx->scope;
    return 0;
}

static int flecs_script_dep_input_new(
    flecs_script_dep_ctx_t *ctx,
    uint64_t *input)
{
    if (*ctx->input_count >= 64) {
        flecs_script_eval_error(ctx->v, ctx->v->base.script->root,
            "script cannot have more than 64 reactive inputs");
        return -1;
    }
    *input = (uint64_t)1 << (*ctx->input_count) ++;
    return 0;
}

static ecs_script_ref_t* flecs_script_dep_ref_find(
    ecs_vec_t *refs,
    const ecs_script_ref_t *value)
{
    ecs_script_ref_t *array = ecs_vec_first(refs);
    int32_t i, count = ecs_vec_count(refs);
    for (i = 0; i < count; i ++) {
        if (array[i].entity != value->entity ||
            array[i].component != value->component ||
            array[i].is_has != value->is_has)
        {
            continue;
        }
        if ((!array[i].name && !value->name) ||
            (array[i].name && value->name &&
                !ecs_os_strcmp(array[i].name, value->name)))
        {
            return &array[i];
        }
    }
    return NULL;
}

static int flecs_script_dep_ref_input(
    flecs_script_dep_ctx_t *ctx,
    ecs_vec_t *refs,
    const ecs_script_ref_t *value,
    uint64_t *input)
{
    ecs_script_ref_t *ref = flecs_script_dep_ref_find(refs, value);
    if (!ref) {
        ref = ecs_vec_append_t(NULL, refs, ecs_script_ref_t);
        *ref = *value;
        ref->observer = 0;
        ref->input = 0;
        ref->is_resolve = false;
    }
    if (!ref->input && flecs_script_dep_input_new(ctx, &ref->input)) {
        return -1;
    }
    *input |= ref->input;
    return 0;
}

static uint64_t flecs_script_dep_var_get(
    flecs_script_dep_ctx_t *ctx,
    int32_t sp)
{
    if (sp < 0 || sp >= ecs_vec_count(&ctx->vars)) {
        return 0;
    }
    return ecs_vec_get_t(&ctx->vars, flecs_script_dep_var_t, sp)->input;
}

static uint64_t flecs_script_dep_var_get_internal(
    flecs_script_dep_ctx_t *ctx,
    int32_t sp)
{
    if (sp < 0 || sp >= ecs_vec_count(&ctx->vars)) {
        return 0;
    }
    return ecs_vec_get_t(&ctx->vars, flecs_script_dep_var_t, sp)->internal;
}

static void flecs_script_dep_var_set(
    flecs_script_dep_ctx_t *ctx,
    int32_t sp,
    uint64_t input,
    uint64_t internal)
{
    if (sp < 0) {
        return;
    }
    ecs_vec_set_min_count_zeromem_t(NULL, &ctx->vars,
        flecs_script_dep_var_t, sp + 1);
    ecs_vec_get_t(&ctx->vars, flecs_script_dep_var_t, sp)[0] =
        (flecs_script_dep_var_t){input, internal};
}

typedef struct flecs_script_dep_expr_ctx_t {
    flecs_script_dep_ctx_t *deps;
    uint64_t *input;
    uint64_t *internal;
} flecs_script_dep_expr_ctx_t;

static int flecs_script_dep_expr_var(
    ecs_expr_node_t *node,
    void *ptr)
{
    if (!node) {
        return 0;
    }
    flecs_script_dep_expr_ctx_t *ctx = ptr;
    if (node->kind == EcsExprVariable) {
        int32_t sp = ((ecs_expr_variable_t*)node)->sp;
        *ctx->input |= flecs_script_dep_var_get(ctx->deps, sp);
        *ctx->internal |= flecs_script_dep_var_get_internal(ctx->deps, sp);
    } else if (node->kind == EcsExprNew) {
        ecs_script_entity_t *entity = ((ecs_expr_new_t*)node)->entity;
        if (flecs_script_dep_node(ctx->deps, (ecs_script_node_t*)entity)) {
            return -1;
        }
        *ctx->input |= entity->node.input;
        *ctx->internal |= entity->node.internal;
    }
    return flecs_expr_visit_children(node, flecs_script_dep_expr_var, ctx);
}

static int flecs_script_dep_expr_vars(
    flecs_script_dep_ctx_t *ctx,
    ecs_expr_node_t *node,
    uint64_t *input,
    uint64_t *internal)
{
    flecs_script_dep_expr_ctx_t expr_ctx = {ctx, input, internal};
    return flecs_script_dep_expr_var(node, &expr_ctx);
}

static int flecs_script_dep_expr(
    flecs_script_dep_ctx_t *ctx,
    ecs_expr_node_t *node,
    uint64_t *input,
    uint64_t *internal)
{
    if (!node) {
        return 0;
    }

    /* Statements that must never be reevaluated by a reactive event don't
     * register refs, so nothing can trigger them. */
    if (ctx->no_deps) {
        uint64_t discard = 0, discard_internal = 0;
        return flecs_script_dep_expr_vars(
            ctx, node, &discard, &discard_internal);
    }

    bool track_dyn_nodes = ctx->v->script_entity && !ctx->template;

    ecs_vec_t *refs = ecs_vec_reset_t(
        NULL, &ctx->expr_refs, ecs_script_ref_t);
    ecs_vec_t *dynamic_refs = ecs_vec_reset_t(
        NULL, &ctx->expr_dynamic_refs, ecs_script_ref_t);
    ecs_vec_t *dyn_nodes = ecs_vec_reset_t(
        NULL, &ctx->expr_dyn_nodes, ecs_expr_node_t*);

    if (flecs_expr_visit_refs(&ctx->v->base.script->pub, node,
        refs, ctx->dynamic_refs ? dynamic_refs : NULL,
        track_dyn_nodes ? dyn_nodes : NULL, refs))
    {
        return -1;
    }

    ecs_script_ref_t *array = ecs_vec_first(refs);
    int32_t i, count = ecs_vec_count(refs);
    for (i = 0; i < count; i ++) {
        if (flecs_script_dep_ref_input(
            ctx, ctx->refs, &array[i], input))
        {
            return -1;
        }
    }

    array = ecs_vec_first(dynamic_refs);
    count = ecs_vec_count(dynamic_refs);
    for (i = 0; i < count; i ++) {
        if (flecs_script_dep_ref_input(
            ctx, ctx->dynamic_refs, &array[i], input))
        {
            return -1;
        }
    }

    ecs_expr_node_t **nodes = ecs_vec_first(dyn_nodes);
    count = ecs_vec_count(dyn_nodes);
    for (i = 0; i < count; i ++) {
        uint64_t dyn_input = 0;
        if (flecs_script_dep_input_new(ctx, &dyn_input)) {
            return -1;
        }
        if (nodes[i]->kind == EcsExprHas) {
            ((ecs_expr_has_t*)nodes[i])->dyn_input = dyn_input;
        } else {
            ((ecs_expr_element_t*)nodes[i])->dyn_input = dyn_input;
        }
        *input |= dyn_input;
    }

    return flecs_script_dep_expr_vars(ctx, node, input, internal);
}

static int flecs_script_dep_id(
    flecs_script_dep_ctx_t *ctx,
    ecs_script_id_t *id,
    uint64_t *input,
    uint64_t *internal)
{
    if (flecs_script_dep_expr(ctx, id->first_expr, input, internal) ||
        flecs_script_dep_expr(ctx, id->second_expr, input, internal))
    {
        return -1;
    }
    *input |= flecs_script_dep_var_get(ctx, id->first_sp);
    *input |= flecs_script_dep_var_get(ctx, id->second_sp);
    *input |= flecs_script_dep_var_get(ctx, id->value_sp);
    *internal |= flecs_script_dep_var_get_internal(ctx, id->first_sp);
    *internal |= flecs_script_dep_var_get_internal(ctx, id->second_sp);
    *internal |= flecs_script_dep_var_get_internal(ctx, id->value_sp);
    return 0;
}

static void flecs_script_dep_symbol_scope(
    flecs_script_dep_ctx_t *ctx,
    int32_t symbol,
    int32_t scope_slot)
{
    if (symbol < 0) {
        return;
    }
    ecs_vec_t *slots = &ctx->v->base.script->symbol_slots;
    if (symbol >= ecs_vec_count(slots)) {
        return;
    }
    ecs_vec_get_t(slots,
        ecs_script_symbol_slot_t, symbol)->scope_slot = scope_slot;
}

static int flecs_script_dep_template(
    flecs_script_dep_ctx_t *ctx,
    ecs_script_template_node_t *node,
    uint64_t *input);

static int flecs_script_dep_node(
    flecs_script_dep_ctx_t *ctx,
    ecs_script_node_t *node)
{
    node->input = 0;
    node->direct_input = 0;
    node->internal = 0;
    node->direct_internal = 0;
    int32_t scope_slot = -1;
    if (ctx->v->base.depth) {
        ecs_script_node_t *parent = ctx->v->base.nodes[ctx->v->base.depth - 1];
        if (parent->kind == EcsAstScope) {
            scope_slot = ((ecs_script_scope_t*)parent)->scope_slot;
        }
    }

    switch(node->kind) {
    case EcsAstScope:
        return flecs_script_dep_scope(ctx, (ecs_script_scope_t*)node);
    case EcsAstTag:
    case EcsAstWithTag: {
        ecs_script_tag_t *n = (ecs_script_tag_t*)node;
        if (flecs_script_dep_id(ctx, &n->id, &node->direct_input, &node->direct_internal)) {
            return -1;
        }
        if (node->kind == EcsAstTag) {
            bool already_owned;
            if (flecs_script_dep_component_owner(
                ctx, node, &n->id, &already_owned))
            {
                return -1;
            }
            n->component_slot = already_owned
                ? -1
                : ctx->component_count ++;
        }
        node->input = node->direct_input;
        node->internal = node->direct_internal;
        break;
    }
    case EcsAstComponent:
    case EcsAstWithComponent: {
        ecs_script_component_t *n = (ecs_script_component_t*)node;
        if (flecs_script_dep_id(ctx, &n->id, &node->direct_input, &node->direct_internal) ||
            flecs_script_dep_expr(ctx, n->expr, &node->direct_input, &node->direct_internal))
        {
            return -1;
        }
        if (node->kind == EcsAstComponent) {
            bool partial = false;
            if (n->expr && n->expr->kind == EcsExprInitializer) {
                ecs_expr_initializer_t *initializer =
                    (ecs_expr_initializer_t*)n->expr;
                partial = initializer->is_partial;
                if (!partial) {
                    ecs_expr_initializer_element_t *elements =
                        ecs_vec_first(&initializer->elements);
                    int32_t i, count = ecs_vec_count(&initializer->elements);
                    for (i = 0; i < count; i ++) {
                        if (elements[i].member) {
                            partial = true;
                            break;
                        }
                    }
                }
                if (partial && flecs_script_dep_initializer_is_complete(
                    ctx->v->world, n->id.eval, initializer))
                {
                    partial = false;
                }
            }
            bool already_owned = false;
            if (!partial) {
                if (flecs_script_dep_component_owner(
                    ctx, node, &n->id, &already_owned))
                {
                    return -1;
                }
            }
            n->component_slot = partial || already_owned
                ? -1
                : ctx->component_count ++;
        }
        node->input = node->direct_input;
        node->internal = node->direct_internal;
        break;
    }
    case EcsAstWith: {
        ecs_script_with_t *n = (ecs_script_with_t*)node;
        if (flecs_script_dep_scope(ctx, n->expressions)) {
            return -1;
        }
        node->direct_input = n->expressions->node.input;
        node->direct_internal = n->expressions->node.internal;
        if (flecs_script_dep_scope(ctx, n->scope)) {
            return -1;
        }
        node->input = node->direct_input | n->scope->node.input;
        node->internal = node->direct_internal | n->scope->node.internal;
        break;
    }
    case EcsAstUsing:
    case EcsAstAnnotation:
    case EcsAstInclude:
    case EcsAstContinue:
        break;
    case EcsAstModule: {
        ecs_script_module_t *n = (ecs_script_module_t*)node;
        flecs_script_dep_symbol_scope(ctx, n->symbol, scope_slot);
        break;
    }
    case EcsAstTemplate: {
        ecs_script_template_node_t *n = (ecs_script_template_node_t*)node;
        flecs_script_dep_symbol_scope(ctx, n->symbol, scope_slot);
        if (flecs_script_dep_template(ctx, n, &node->direct_input)) {
            return -1;
        }
        node->input = node->direct_input;
        node->internal = node->direct_internal;
        break;
    }
    case EcsAstProp:
    case EcsAstMut: {
        ecs_script_var_node_t *n = (ecs_script_var_node_t*)node;
        if (node->skip) {
            break;
        }
        if (!ctx->template ||
            ctx->member >= ecs_vec_count(&ctx->template->members))
        {
            flecs_script_eval_error(ctx->v, node,
                "variable '%s' is not a template member", n->name);
            return -1;
        }
        ecs_script_template_member_t *member = ecs_vec_get_t(
            &ctx->template->members, ecs_script_template_member_t,
            ctx->member ++);
        flecs_script_dep_var_set(ctx, n->sp, member->input, 0);
        if (flecs_script_dep_expr(ctx, n->expr, &node->direct_input, &node->direct_internal)) {
            return -1;
        }
        node->input = node->direct_input | member->input;
        break;
    }
    case EcsAstConst:
    case EcsAstExportConst:
    case EcsAstExportMut: {
        ecs_script_var_node_t *n = (ecs_script_var_node_t*)node;
        n->computed = 0;
        if (node->skip) {
            break;
        }
        bool no_deps = ctx->no_deps;
        ctx->no_deps = no_deps || node->kind == EcsAstExportMut;
        int dep_result = flecs_script_dep_expr(
            ctx, n->expr, &node->direct_input, &node->direct_internal);
        ctx->no_deps = no_deps;
        if (dep_result) {
            return -1;
        }
        if (node->kind == EcsAstConst) {
            bool computed = ctx->template && !ctx->conditional &&
                !ctx->no_computed && !n->is_await &&
                ctx->template->computed_count < 64;
            if (computed) {
                int32_t slot = ctx->template->computed_count ++;
                n->computed = slot + 1;
                flecs_script_dep_var_set(ctx, n->sp, 0, (uint64_t)1 << slot);
            } else {
                flecs_script_dep_var_set(ctx, n->sp,
                    node->direct_input, node->direct_internal);
            }
        } else {
            flecs_script_dep_symbol_scope(ctx, n->symbol, scope_slot);
        }
        node->input = node->direct_input;
        node->internal = node->direct_internal;
        break;
    }
    case EcsAstEntity: {
        ecs_script_entity_t *n = (ecs_script_entity_t*)node;
        flecs_script_dep_symbol_scope(ctx, n->symbol, scope_slot);
        if (flecs_script_dep_expr(ctx, n->name_expr, &node->direct_input, &node->direct_internal)) {
            return -1;
        }
        node->direct_input |= flecs_script_dep_var_get(ctx, n->kind_sp);
        int32_t entity_symbol = ctx->entity_symbol;
        ecs_script_entity_t *entity = ctx->entity;
        n->parent = entity;
        ctx->entity_symbol = n->symbol;
        ctx->entity = n;
        int result = flecs_script_dep_scope(ctx, n->scope);
        ctx->entity_symbol = entity_symbol;
        ctx->entity = entity;
        if (result) {
            return -1;
        }
        node->input = node->direct_input | n->scope->node.input;
        node->internal = node->direct_internal | n->scope->node.internal;
        break;
    }
    case EcsAstPairScope: {
        ecs_script_pair_scope_t *n = (ecs_script_pair_scope_t*)node;
        flecs_script_dep_symbol_scope(
            ctx, n->id.first_symbol, scope_slot);
        flecs_script_dep_symbol_scope(
            ctx, n->id.second_symbol, scope_slot);
        if (flecs_script_dep_id(ctx, &n->id, &node->direct_input, &node->direct_internal) ||
            flecs_script_dep_scope(ctx, n->scope))
        {
            return -1;
        }
        node->input = node->direct_input | n->scope->node.input;
        node->internal = node->direct_internal | n->scope->node.internal;
        break;
    }
    case EcsAstIf: {
        ecs_script_if_t *n = (ecs_script_if_t*)node;
        n->if_true->alternative = n->if_false;
        n->if_false->alternative = n->if_true;
        if (flecs_script_dep_expr(ctx, n->expr, &node->direct_input,
            &node->direct_internal))
        {
            return -1;
        }
        ctx->conditional ++;
        if (flecs_script_dep_scope(ctx, n->if_true) ||
            flecs_script_dep_scope(ctx, n->if_false))
        {
            ctx->conditional --;
            return -1;
        }
        ctx->conditional --;
        node->input = node->direct_input |
            n->if_true->node.input | n->if_false->node.input;
        node->internal = node->direct_internal |
            n->if_true->node.internal | n->if_false->node.internal;
        break;
    }
    case EcsAstFor: {
        ecs_script_for_t *n = (ecs_script_for_t*)node;
        if (flecs_script_dep_expr(ctx, n->from, &node->direct_input, &node->direct_internal) ||
            flecs_script_dep_expr(ctx, n->to, &node->direct_input, &node->direct_internal) ||
            flecs_script_dep_expr(ctx, n->expr, &node->direct_input, &node->direct_internal))
        {
            return -1;
        }
        n->for_slot = ctx->for_count ++;
        int32_t i;
        for (i = 0; i < n->loop_var_count; i ++) {
            flecs_script_dep_var_set(ctx, n->loop_var_sp[i],
                node->direct_input, node->direct_internal);
        }
        ctx->conditional ++;
        if (flecs_script_dep_scope(ctx, n->scope)) {
            ctx->conditional --;
            return -1;
        }
        ctx->conditional --;
        node->input = node->direct_input | n->scope->node.input;
        node->internal = node->direct_internal | n->scope->node.internal;
        break;
    }
    case EcsAstFunction: {
        ecs_script_function_node_t *n = (ecs_script_function_node_t*)node;
        flecs_script_dep_symbol_scope(ctx, n->symbol, scope_slot);
        break;
    }
    case EcsAstAwait: {
        ecs_script_await_t *n = (ecs_script_await_t*)node;
        if (flecs_script_dep_expr(ctx, n->expr, &node->direct_input, &node->direct_internal)) {
            return -1;
        }
        node->input = node->direct_input;
        node->internal = node->direct_internal;
        break;
    }
    case EcsAstTry: {
        ecs_script_try_t *n = (ecs_script_try_t*)node;
        ctx->conditional ++;
        if (flecs_script_dep_scope(ctx, n->try_scope)) {
            ctx->conditional --;
            return -1;
        }
        node->input = n->try_scope->node.input;
        node->internal = n->try_scope->node.internal;
        ecs_script_catch_t *catches = ecs_vec_first(&n->catches);
        int32_t i, count = ecs_vec_count(&n->catches);
        for (i = 0; i < count; i ++) {
            if (flecs_script_dep_scope(ctx, catches[i].scope)) {
                ctx->conditional --;
                return -1;
            }
            node->input |= catches[i].scope->node.input;
            node->internal |= catches[i].scope->node.internal;
        }
        ctx->conditional --;
        break;
    }
    }
    return 0;
}

static int flecs_script_dep_scope(
    flecs_script_dep_ctx_t *ctx,
    ecs_script_scope_t *scope)
{
    ecs_script_scope_t *prev_scope = ctx->scope;
    ctx->scope = scope;
    scope->scope_slot = ctx->scope_count ++;
    scope->node.input = 0;
    scope->node.direct_input = 0;
    scope->node.internal = 0;
    scope->node.direct_internal = 0;
    ecs_assert(ctx->v->base.depth < ECS_SCRIPT_VISIT_MAX_DEPTH,
        ECS_INTERNAL_ERROR, NULL);
    ctx->v->base.nodes[ctx->v->base.depth ++] = (ecs_script_node_t*)scope;
    ecs_script_node_t **stmts = ecs_vec_first(&scope->stmts);
    int32_t i, count = ecs_vec_count(&scope->stmts);
    for (i = 0; i < count; i ++) {
        /* Entities hoisted into the scope by "new" expressions are analyzed by
         * the expression that created them, which adds the inputs of the entity
         * to the statement that owns the expression. */
        if (flecs_script_node_is_hoisted(stmts[i])) {
            continue;
        }
        if (flecs_script_dep_node(ctx, stmts[i])) {
            ctx->v->base.depth --;
            ctx->scope = prev_scope;
            return -1;
        }
        scope->node.input |= stmts[i]->input;
        scope->node.internal |= stmts[i]->internal;
    }
    uint64_t next_input = 0;
    for (i = count - 1; i >= 0; i --) {
        if (flecs_script_node_is_hoisted(stmts[i])) {
            continue;
        }
        if (stmts[i]->kind == EcsAstAnnotation) {
            stmts[i]->input = next_input;
            scope->node.input |= next_input;
        } else {
            next_input = stmts[i]->input;
        }
    }
    ctx->v->base.depth --;
    ctx->scope = prev_scope;
    return 0;
}

static int flecs_script_dep_assign_refs(
    flecs_script_dep_ctx_t *ctx,
    ecs_vec_t *refs)
{
    ecs_script_ref_t *array = ecs_vec_first(refs);
    int32_t i, count = ecs_vec_count(refs);
    for (i = 0; i < count; i ++) {
        array[i].input = 0;
        if (flecs_script_dep_input_new(ctx, &array[i].input)) {
            return -1;
        }
    }
    return 0;
}

static int flecs_script_dep_template_init(
    flecs_script_dep_ctx_t *ctx,
    ecs_script_template_t *template,
    flecs_script_dep_ctx_t *outer)
{
    template->input_count = 0;
    ctx->input_count = &template->input_count;
    ecs_script_template_member_t *members = ecs_vec_first(&template->members);
    int32_t i, count = ecs_vec_count(&template->members);
    for (i = 0; i < count; i ++) {
        members[i].input = 0;
        if (flecs_script_dep_input_new(ctx, &members[i].input)) {
            return -1;
        }
    }
    if (flecs_script_dep_assign_refs(ctx, &template->refs) ||
        flecs_script_dep_assign_refs(ctx, &template->dynamic_refs))
    {
        return -1;
    }
    int32_t *capture_sp = ecs_vec_first(&template->capture_sp);
    count = ecs_vec_count(&template->capture_sp);
    ecs_vec_set_count_t(NULL, &template->capture_input,
        ecs_script_template_capture_t, count);
    ecs_script_template_capture_t *captures = ecs_vec_first(
        &template->capture_input);
    for (i = 0; i < count; i ++) {
        uint64_t outer_input = outer
            ? flecs_script_dep_var_get(outer, capture_sp[i])
            : 0;
        uint64_t input = 0;
        if (outer_input && flecs_script_dep_input_new(ctx, &input)) {
            return -1;
        }
        captures[i].outer_input = outer_input;
        captures[i].input = input;
        flecs_script_dep_var_set(ctx, i, input, 0);
    }
    flecs_script_dep_var_set(ctx, count, 0, 0);

    members = ecs_vec_first(&template->members);
    for (i = 0; i < template->inherited_count; i ++) {
        flecs_script_dep_var_set(ctx, count + 1 + i, members[i].input, 0);
    }
    ctx->member = template->inherited_count;
    return 0;
}

static bool flecs_script_dep_scope_has_template(
    ecs_script_scope_t *scope)
{
    ecs_script_node_t **stmts = ecs_vec_first(&scope->stmts);
    int32_t i, count = ecs_vec_count(&scope->stmts);
    for (i = 0; i < count; i ++) {
        ecs_script_node_t *node = stmts[i];
        switch(node->kind) {
        case EcsAstTemplate:
            return true;
        case EcsAstScope:
            if (flecs_script_dep_scope_has_template(
                (ecs_script_scope_t*)node))
            {
                return true;
            }
            break;
        case EcsAstEntity:
            if (flecs_script_dep_scope_has_template(
                ((ecs_script_entity_t*)node)->scope))
            {
                return true;
            }
            break;
        case EcsAstWith:
            if (flecs_script_dep_scope_has_template(
                ((ecs_script_with_t*)node)->scope))
            {
                return true;
            }
            break;
        case EcsAstPairScope:
            if (flecs_script_dep_scope_has_template(
                ((ecs_script_pair_scope_t*)node)->scope))
            {
                return true;
            }
            break;
        case EcsAstIf: {
            ecs_script_if_t *n = (ecs_script_if_t*)node;
            if (flecs_script_dep_scope_has_template(n->if_true) ||
                flecs_script_dep_scope_has_template(n->if_false))
            {
                return true;
            }
            break;
        }
        case EcsAstFor:
            if (flecs_script_dep_scope_has_template(
                ((ecs_script_for_t*)node)->scope))
            {
                return true;
            }
            break;
        case EcsAstTry: {
            ecs_script_try_t *n = (ecs_script_try_t*)node;
            if (flecs_script_dep_scope_has_template(n->try_scope)) {
                return true;
            }
            ecs_script_catch_t *catches = ecs_vec_first(&n->catches);
            int32_t j, catch_count = ecs_vec_count(&n->catches);
            for (j = 0; j < catch_count; j ++) {
                if (flecs_script_dep_scope_has_template(catches[j].scope)) {
                    return true;
                }
            }
            break;
        }
        default:
            break;
        }
    }
    return false;
}

static int flecs_script_dep_template_analyze(
    ecs_script_eval_visitor_t *v,
    ecs_script_template_t *template,
    flecs_script_dep_ctx_t *outer)
{
    flecs_script_dep_ctx_t ctx = {
        .v = v,
        .template = template,
        .refs = &template->refs,
        .dynamic_refs = &template->dynamic_refs,
        .entity_symbol = template->root_symbol
    };
    ecs_vec_init_t(NULL, &ctx.vars, flecs_script_dep_var_t, 0);
    ecs_vec_init_t(NULL, &ctx.component_owners,
        flecs_script_component_owner_t, 0);
    if (flecs_script_dep_template_init(&ctx, template, outer)) {
        flecs_script_dep_fini(&ctx);
        return -1;
    }
    template->computed_count = 0;
    ctx.no_computed = flecs_script_dep_scope_has_template(
        template->node->scope);
    int32_t old_depth = v->base.depth;
    v->base.depth = 0;
    int result = flecs_script_dep_scope(&ctx, template->node->scope);
    v->base.depth = old_depth;
    if (!result) {
        template->scope_count = ctx.scope_count;
        template->component_count = ctx.component_count;
        template->for_count = ctx.for_count;
        if (template->root_symbol >= 0) {
            flecs_script_dep_symbol_scope(
                &ctx, template->root_symbol,
                template->node->scope->scope_slot);
        }
        template->input_count = *ctx.input_count;
    }
    flecs_script_dep_fini(&ctx);
    return result;
}

static int flecs_script_dep_template(
    flecs_script_dep_ctx_t *ctx,
    ecs_script_template_node_t *node,
    uint64_t *input)
{
    ecs_entity_t entity = flecs_script_symbol_entity(ctx->v, node->symbol);
    const EcsScript *script = ecs_get(ctx->v->world, entity, EcsScript);
    if (!script || !script->template_) {
        flecs_script_eval_error(ctx->v, node,
            "'%s' is not a template", node->name);
        return -1;
    }
    ecs_script_template_t *template = script->template_;
    int32_t *capture_sp = ecs_vec_first(&template->capture_sp);
    int32_t i, count = ecs_vec_count(&template->capture_sp);
    for (i = 0; i < count; i ++) {
        *input |= flecs_script_dep_var_get(ctx, capture_sp[i]);
    }
    return flecs_script_dep_template_analyze(ctx->v, template, ctx);
}

int flecs_script_analyze_dependencies(
    ecs_script_eval_visitor_t *v,
    ecs_script_scope_t *scope)
{
    ecs_script_impl_t *impl = v->base.script;
    impl->input_count = 0;
    flecs_script_dep_ctx_t ctx = {
        .v = v,
        .refs = &impl->refs,
        .input_count = &impl->input_count,
        .entity_symbol = -1
    };
    ecs_vec_init_t(NULL, &ctx.vars, flecs_script_dep_var_t, 0);
    ecs_vec_init_t(NULL, &ctx.component_owners,
        flecs_script_component_owner_t, 0);
    if (flecs_script_dep_assign_refs(&ctx, &impl->refs)) {
        flecs_script_dep_fini(&ctx);
        return -1;
    }
    int32_t old_depth = v->base.depth;
    v->base.depth = 0;
    int result = flecs_script_dep_scope(&ctx, scope);
    v->base.depth = old_depth;
    if (!result) {
        ecs_vec_set_count_t(NULL, &impl->scope_slots,
            int32_t, ctx.scope_count);
        if (ctx.scope_count) {
            ecs_os_memset(ecs_vec_first(&impl->scope_slots), 0,
                ctx.scope_count * ECS_SIZEOF(int32_t));
        }
        ecs_vec_set_count_t(NULL, &impl->component_slots,
            ecs_script_component_slot_t, ctx.component_count);
        if (ctx.component_count) {
            ecs_os_memset(ecs_vec_first(&impl->component_slots), 0,
                ctx.component_count *
                    ECS_SIZEOF(ecs_script_component_slot_t));
        }
        flecs_script_for_slots_init(&impl->for_slots, ctx.for_count);
    }
    flecs_script_dep_fini(&ctx);
    return result;
}

#endif
