#include "flecs.h"

#ifdef FLECS_SCRIPT
#include "../script.h"

typedef struct flecs_script_component_owner_t {
    ecs_script_entity_t *entity;
    ecs_script_id_t *id;
    ecs_script_scope_t *scope;
    int32_t entity_symbol;
} flecs_script_component_owner_t;

typedef struct flecs_script_entity_parent_t {
    ecs_script_entity_t *entity;
    ecs_script_entity_t *parent;
} flecs_script_entity_parent_t;

typedef struct flecs_script_dep_ctx_t {
    ecs_script_eval_visitor_t *v;
    ecs_script_template_t *template;
    ecs_vec_t *refs;
    ecs_vec_t *dynamic_refs;
    ecs_vec_t vars;
    ecs_vec_t component_owners;
    ecs_vec_t entity_parents;
    int32_t *input_count;
    int32_t scope_count;
    int32_t component_count;
    int32_t for_count;
    int32_t member;
    int32_t entity_symbol;
    bool no_deps;
    ecs_script_entity_t *entity;
    ecs_script_scope_t *scope;
} flecs_script_dep_ctx_t;

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
    ecs_script_scope_t *scope = first;
    while (scope) {
        ecs_script_node_t **stmts = ecs_vec_first(&scope->stmts);
        int32_t i, count = ecs_vec_count(&scope->stmts);
        for (i = 0; i < count; i ++) {
            if (stmts[i]->kind != EcsAstIf) {
                continue;
            }
            ecs_script_if_t *if_ = (ecs_script_if_t*)stmts[i];
            bool first_true = flecs_script_dep_scope_contains(
                first, if_->if_true);
            bool first_false = flecs_script_dep_scope_contains(
                first, if_->if_false);
            bool second_true = flecs_script_dep_scope_contains(
                second, if_->if_true);
            bool second_false = flecs_script_dep_scope_contains(
                second, if_->if_false);
            if ((first_true && second_false) ||
                (first_false && second_true))
            {
                return true;
            }
        }
        scope = scope->parent;
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

static ecs_script_entity_t* flecs_script_dep_entity_parent(
    const flecs_script_dep_ctx_t *ctx,
    ecs_script_entity_t *entity)
{
    flecs_script_entity_parent_t *parents = ecs_vec_first(
        &ctx->entity_parents);
    int32_t i, count = ecs_vec_count(&ctx->entity_parents);
    for (i = count - 1; i >= 0; i --) {
        if (parents[i].entity == entity) {
            return parents[i].parent;
        }
    }
    return NULL;
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
    const flecs_script_dep_ctx_t *ctx,
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
    return flecs_script_dep_entity_may_match(ctx,
        flecs_script_dep_entity_parent(ctx, first),
        flecs_script_dep_entity_parent(ctx, second));
}

static bool flecs_script_dep_entity_matches(
    const flecs_script_dep_ctx_t *ctx,
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
    return flecs_script_dep_entity_matches(ctx,
        flecs_script_dep_entity_parent(ctx, first),
        flecs_script_dep_entity_parent(ctx, second));
}

static bool flecs_script_dep_same_entity(
    const flecs_script_component_owner_t *owner,
    const flecs_script_dep_ctx_t *ctx)
{
    if (owner->entity || ctx->entity) {
        return flecs_script_dep_entity_may_match(
            ctx, owner->entity, ctx->entity);
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
        ctx, first_entity, second_entity))
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
        first_entity = flecs_script_dep_entity_parent(ctx, first_entity);
        second_entity = flecs_script_dep_entity_parent(ctx, second_entity);
    }
    return first_scope == second_scope;
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

static void flecs_script_dep_vars_ensure(
    flecs_script_dep_ctx_t *ctx,
    int32_t count)
{
    int32_t old_count = ecs_vec_count(&ctx->vars);
    if (old_count >= count) {
        return;
    }
    ecs_vec_set_count_t(NULL, &ctx->vars, uint64_t, count);
    uint64_t *vars = ecs_vec_first(&ctx->vars);
    ecs_os_memset(&vars[old_count], 0,
        (count - old_count) * ECS_SIZEOF(uint64_t));
}

static uint64_t flecs_script_dep_var_get(
    flecs_script_dep_ctx_t *ctx,
    int32_t sp)
{
    if (sp < 0 || sp >= ecs_vec_count(&ctx->vars)) {
        return 0;
    }
    return ecs_vec_get_t(&ctx->vars, uint64_t, sp)[0];
}

static void flecs_script_dep_var_set(
    flecs_script_dep_ctx_t *ctx,
    int32_t sp,
    uint64_t input)
{
    if (sp < 0) {
        return;
    }
    flecs_script_dep_vars_ensure(ctx, sp + 1);
    ecs_vec_get_t(&ctx->vars, uint64_t, sp)[0] = input;
}

static int flecs_script_dep_expr_vars(
    flecs_script_dep_ctx_t *ctx,
    ecs_expr_node_t *node,
    uint64_t *input)
{
    if (!node) {
        return 0;
    }

    switch(node->kind) {
    case EcsExprValue:
    case EcsExprGlobalVariable:
        break;
    case EcsExprVariable:
        *input |= flecs_script_dep_var_get(
            ctx, ((ecs_expr_variable_t*)node)->sp);
        break;
    case EcsExprInterpolatedString: {
        ecs_expr_interpolated_string_t *n =
            (ecs_expr_interpolated_string_t*)node;
        ecs_expr_node_t **expressions = ecs_vec_first(&n->expressions);
        int32_t i, count = ecs_vec_count(&n->expressions);
        for (i = 0; i < count; i ++) {
            if (flecs_script_dep_expr_vars(ctx, expressions[i], input)) {
                return -1;
            }
        }
        ecs_expr_format_t *formats = ecs_vec_first(&n->formats);
        count = ecs_vec_count(&n->formats);
        for (i = 0; i < count; i ++) {
            if (flecs_script_dep_expr_vars(
                ctx, formats[i].width, input) ||
                flecs_script_dep_expr_vars(
                    ctx, formats[i].precision, input))
            {
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
            if (flecs_script_dep_expr_vars(ctx, elems[i].key, input) ||
                flecs_script_dep_expr_vars(ctx, elems[i].value, input))
            {
                return -1;
            }
        }
        break;
    }
    case EcsExprUnary:
        return flecs_script_dep_expr_vars(
            ctx, ((ecs_expr_unary_t*)node)->expr, input);
    case EcsExprBinary: {
        ecs_expr_binary_t *n = (ecs_expr_binary_t*)node;
        if (flecs_script_dep_expr_vars(ctx, n->left, input) ||
            flecs_script_dep_expr_vars(ctx, n->right, input))
        {
            return -1;
        }
        break;
    }
    case EcsExprIdentifier:
        return flecs_script_dep_expr_vars(
            ctx, ((ecs_expr_identifier_t*)node)->expr, input);
    case EcsExprFunction:
    case EcsExprMethod: {
        ecs_expr_function_t *n = (ecs_expr_function_t*)node;
        if (flecs_script_dep_expr_vars(ctx, n->left, input) ||
            flecs_script_dep_expr_vars(
                ctx, (ecs_expr_node_t*)n->args, input))
        {
            return -1;
        }
        break;
    }
    case EcsExprMember:
        return flecs_script_dep_expr_vars(
            ctx, ((ecs_expr_member_t*)node)->left, input);
    case EcsExprSwizzle:
        return flecs_script_dep_expr_vars(
            ctx, ((ecs_expr_swizzle_t*)node)->left, input);
    case EcsExprElement: {
        ecs_expr_element_t *n = (ecs_expr_element_t*)node;
        if (flecs_script_dep_expr_vars(ctx, n->left, input) ||
            flecs_script_dep_expr_vars(ctx, n->index, input))
        {
            return -1;
        }
        break;
    }
    case EcsExprComponent: {
        ecs_expr_component_t *n = (ecs_expr_component_t*)node;
        return flecs_script_dep_expr_vars(ctx, n->expr, input);
    }
    case EcsExprHas: {
        ecs_expr_has_t *n = (ecs_expr_has_t*)node;
        if (flecs_script_dep_expr_vars(ctx, n->left, input) ||
            flecs_script_dep_expr_vars(ctx, n->first, input) ||
            flecs_script_dep_expr_vars(ctx, n->second, input))
        {
            return -1;
        }
        break;
    }
    case EcsExprCast:
    case EcsExprCastNumber:
        return flecs_script_dep_expr_vars(
            ctx, ((ecs_expr_cast_t*)node)->expr, input);
    case EcsExprMatch: {
        ecs_expr_match_t *n = (ecs_expr_match_t*)node;
        if (flecs_script_dep_expr_vars(ctx, n->expr, input)) {
            return -1;
        }
        ecs_expr_match_element_t *elems = ecs_vec_first(&n->elements);
        int32_t i, count = ecs_vec_count(&n->elements);
        for (i = 0; i < count; i ++) {
            if (flecs_script_dep_expr_vars(ctx, elems[i].compare, input) ||
                flecs_script_dep_expr_vars(ctx, elems[i].expr, input))
            {
                return -1;
            }
        }
        if (flecs_script_dep_expr_vars(ctx, n->any.compare, input) ||
            flecs_script_dep_expr_vars(ctx, n->any.expr, input))
        {
            return -1;
        }
        break;
    }
    case EcsExprRange: {
        ecs_expr_range_t *n = (ecs_expr_range_t*)node;
        if (flecs_script_dep_expr_vars(ctx, n->from, input) ||
            flecs_script_dep_expr_vars(ctx, n->to, input))
        {
            return -1;
        }
        break;
    }
    case EcsExprNew: {
        ecs_expr_new_t *n = (ecs_expr_new_t*)node;
        if (flecs_script_dep_node(
            ctx, (ecs_script_node_t*)n->entity))
        {
            return -1;
        }
        *input |= n->entity->node.input;
        break;
    }
    case EcsExprScript:
        break;
    }
    return 0;
}

static int flecs_script_dep_expr(
    flecs_script_dep_ctx_t *ctx,
    ecs_expr_node_t *node,
    uint64_t *input)
{
    if (!node) {
        return 0;
    }

    /* Statements that must never be reevaluated by a reactive event don't
     * register refs, so nothing can trigger them. */
    if (ctx->no_deps) {
        uint64_t discard = 0;
        return flecs_script_dep_expr_vars(ctx, node, &discard);
    }

    bool track_dyn_nodes = ctx->v->script_entity && !ctx->template;

    ecs_vec_t refs = {0};
    ecs_vec_t dynamic_refs = {0};
    ecs_vec_t dyn_nodes = {0};
    ecs_vec_init_t(NULL, &refs, ecs_script_ref_t, 0);
    if (ctx->dynamic_refs) {
        ecs_vec_init_t(NULL, &dynamic_refs, ecs_script_ref_t, 0);
    }
    if (track_dyn_nodes) {
        ecs_vec_init_t(NULL, &dyn_nodes, ecs_expr_node_t*, 0);
    }

    if (flecs_expr_visit_refs(&ctx->v->base.script->pub, node,
        &refs, ctx->dynamic_refs ? &dynamic_refs : NULL,
        track_dyn_nodes ? &dyn_nodes : NULL, &refs))
    {
        ecs_vec_fini_t(NULL, &refs, ecs_script_ref_t);
        if (ctx->dynamic_refs) {
            ecs_vec_fini_t(NULL, &dynamic_refs, ecs_script_ref_t);
        }
        if (track_dyn_nodes) {
            ecs_vec_fini_t(NULL, &dyn_nodes, ecs_expr_node_t*);
        }
        return -1;
    }

    ecs_script_ref_t *array = ecs_vec_first(&refs);
    int32_t i, count = ecs_vec_count(&refs);
    for (i = 0; i < count; i ++) {
        if (flecs_script_dep_ref_input(
            ctx, ctx->refs, &array[i], input))
        {
            goto error;
        }
    }

    array = ecs_vec_first(&dynamic_refs);
    count = ecs_vec_count(&dynamic_refs);
    for (i = 0; i < count; i ++) {
        if (flecs_script_dep_ref_input(
            ctx, ctx->dynamic_refs, &array[i], input))
        {
            goto error;
        }
    }

    ecs_expr_node_t **nodes = ecs_vec_first(&dyn_nodes);
    count = ecs_vec_count(&dyn_nodes);
    for (i = 0; i < count; i ++) {
        uint64_t dyn_input = 0;
        if (flecs_script_dep_input_new(ctx, &dyn_input)) {
            goto error;
        }
        if (nodes[i]->kind == EcsExprHas) {
            ((ecs_expr_has_t*)nodes[i])->dyn_input = dyn_input;
        } else {
            ((ecs_expr_element_t*)nodes[i])->dyn_input = dyn_input;
        }
        *input |= dyn_input;
    }

    ecs_vec_fini_t(NULL, &refs, ecs_script_ref_t);
    if (ctx->dynamic_refs) {
        ecs_vec_fini_t(NULL, &dynamic_refs, ecs_script_ref_t);
    }
    if (track_dyn_nodes) {
        ecs_vec_fini_t(NULL, &dyn_nodes, ecs_expr_node_t*);
    }
    return flecs_script_dep_expr_vars(ctx, node, input);
error:
    ecs_vec_fini_t(NULL, &refs, ecs_script_ref_t);
    if (ctx->dynamic_refs) {
        ecs_vec_fini_t(NULL, &dynamic_refs, ecs_script_ref_t);
    }
    if (track_dyn_nodes) {
        ecs_vec_fini_t(NULL, &dyn_nodes, ecs_expr_node_t*);
    }
    return -1;
}

static int flecs_script_dep_id(
    flecs_script_dep_ctx_t *ctx,
    ecs_script_id_t *id,
    uint64_t *input)
{
    if (flecs_script_dep_expr(ctx, id->first_expr, input) ||
        flecs_script_dep_expr(ctx, id->second_expr, input))
    {
        return -1;
    }
    *input |= flecs_script_dep_var_get(ctx, id->first_sp);
    *input |= flecs_script_dep_var_get(ctx, id->second_sp);
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
        if (flecs_script_dep_id(ctx, &n->id, &node->direct_input)) {
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
        break;
    }
    case EcsAstComponent:
    case EcsAstWithComponent: {
        ecs_script_component_t *n = (ecs_script_component_t*)node;
        if (flecs_script_dep_id(ctx, &n->id, &node->direct_input) ||
            flecs_script_dep_expr(ctx, n->expr, &node->direct_input))
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
        break;
    }
    case EcsAstWith: {
        ecs_script_with_t *n = (ecs_script_with_t*)node;
        if (flecs_script_dep_scope(ctx, n->expressions)) {
            return -1;
        }
        node->direct_input = n->expressions->node.input;
        if (flecs_script_dep_scope(ctx, n->scope)) {
            return -1;
        }
        node->input = node->direct_input | n->scope->node.input;
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
        flecs_script_dep_var_set(ctx, n->sp, member->input);
        if (flecs_script_dep_expr(ctx, n->expr, &node->direct_input)) {
            return -1;
        }
        node->input = node->direct_input | member->input;
        break;
    }
    case EcsAstConst:
    case EcsAstExportConst:
    case EcsAstExportMut: {
        ecs_script_var_node_t *n = (ecs_script_var_node_t*)node;
        if (node->skip) {
            break;
        }
        bool no_deps = ctx->no_deps;
        ctx->no_deps = no_deps || node->kind == EcsAstExportMut;
        int dep_result = flecs_script_dep_expr(
            ctx, n->expr, &node->direct_input);
        ctx->no_deps = no_deps;
        if (dep_result) {
            return -1;
        }
        if (node->kind == EcsAstConst) {
            flecs_script_dep_var_set(ctx, n->sp, node->direct_input);
        } else {
            flecs_script_dep_symbol_scope(ctx, n->symbol, scope_slot);
        }
        node->input = node->direct_input;
        break;
    }
    case EcsAstEntity: {
        ecs_script_entity_t *n = (ecs_script_entity_t*)node;
        flecs_script_dep_symbol_scope(ctx, n->symbol, scope_slot);
        if (flecs_script_dep_expr(ctx, n->name_expr, &node->direct_input)) {
            return -1;
        }
        node->direct_input |= flecs_script_dep_var_get(ctx, n->kind_sp);
        int32_t entity_symbol = ctx->entity_symbol;
        ecs_script_entity_t *entity = ctx->entity;
        flecs_script_entity_parent_t *parent = ecs_vec_append_t(
            NULL, &ctx->entity_parents, flecs_script_entity_parent_t);
        parent->entity = n;
        parent->parent = entity;
        ctx->entity_symbol = n->symbol;
        ctx->entity = n;
        int result = flecs_script_dep_scope(ctx, n->scope);
        ctx->entity_symbol = entity_symbol;
        ctx->entity = entity;
        if (result) {
            return -1;
        }
        node->input = node->direct_input | n->scope->node.input;
        break;
    }
    case EcsAstPairScope: {
        ecs_script_pair_scope_t *n = (ecs_script_pair_scope_t*)node;
        flecs_script_dep_symbol_scope(
            ctx, n->id.first_symbol, scope_slot);
        flecs_script_dep_symbol_scope(
            ctx, n->id.second_symbol, scope_slot);
        if (flecs_script_dep_id(ctx, &n->id, &node->direct_input) ||
            flecs_script_dep_scope(ctx, n->scope))
        {
            return -1;
        }
        node->input = node->direct_input | n->scope->node.input;
        break;
    }
    case EcsAstIf: {
        ecs_script_if_t *n = (ecs_script_if_t*)node;
        if (flecs_script_dep_expr(ctx, n->expr, &node->direct_input) ||
            flecs_script_dep_scope(ctx, n->if_true) ||
            flecs_script_dep_scope(ctx, n->if_false))
        {
            return -1;
        }
        node->input = node->direct_input |
            n->if_true->node.input | n->if_false->node.input;
        break;
    }
    case EcsAstFor: {
        ecs_script_for_t *n = (ecs_script_for_t*)node;
        if (flecs_script_dep_expr(ctx, n->from, &node->direct_input) ||
            flecs_script_dep_expr(ctx, n->to, &node->direct_input) ||
            flecs_script_dep_expr(ctx, n->expr, &node->direct_input))
        {
            return -1;
        }
        n->for_slot = ctx->for_count ++;
        int32_t i;
        for (i = 0; i < n->loop_var_count; i ++) {
            flecs_script_dep_var_set(
                ctx, n->loop_var_sp[i], node->direct_input);
        }
        if (flecs_script_dep_scope(ctx, n->scope)) {
            return -1;
        }
        node->input = node->direct_input | n->scope->node.input;
        break;
    }
    case EcsAstFunction: {
        ecs_script_function_node_t *n = (ecs_script_function_node_t*)node;
        flecs_script_dep_symbol_scope(ctx, n->symbol, scope_slot);
        break;
    }
    case EcsAstAwait: {
        ecs_script_await_t *n = (ecs_script_await_t*)node;
        if (flecs_script_dep_expr(ctx, n->expr, &node->direct_input)) {
            return -1;
        }
        node->input = node->direct_input;
        break;
    }
    case EcsAstTry: {
        ecs_script_try_t *n = (ecs_script_try_t*)node;
        if (flecs_script_dep_scope(ctx, n->try_scope)) {
            return -1;
        }
        node->input = n->try_scope->node.input;
        ecs_script_catch_t *catches = ecs_vec_first(&n->catches);
        int32_t i, count = ecs_vec_count(&n->catches);
        for (i = 0; i < count; i ++) {
            if (flecs_script_dep_scope(ctx, catches[i].scope)) {
                return -1;
            }
            node->input |= catches[i].scope->node.input;
        }
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
        flecs_script_dep_var_set(ctx, i, input);
    }
    flecs_script_dep_var_set(ctx, count, 0);
    return 0;
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
    ecs_vec_init_t(NULL, &ctx.vars, uint64_t, 0);
    ecs_vec_init_t(NULL, &ctx.component_owners,
        flecs_script_component_owner_t, 0);
    ecs_vec_init_t(NULL, &ctx.entity_parents,
        flecs_script_entity_parent_t, 0);
    if (flecs_script_dep_template_init(&ctx, template, outer)) {
        ecs_vec_fini_t(NULL, &ctx.vars, uint64_t);
        ecs_vec_fini_t(NULL, &ctx.component_owners,
            flecs_script_component_owner_t);
        ecs_vec_fini_t(NULL, &ctx.entity_parents,
            flecs_script_entity_parent_t);
        return -1;
    }
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
    ecs_vec_fini_t(NULL, &ctx.vars, uint64_t);
    ecs_vec_fini_t(NULL, &ctx.component_owners,
        flecs_script_component_owner_t);
    ecs_vec_fini_t(NULL, &ctx.entity_parents,
        flecs_script_entity_parent_t);
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
    ecs_vec_init_t(NULL, &ctx.vars, uint64_t, 0);
    ecs_vec_init_t(NULL, &ctx.component_owners,
        flecs_script_component_owner_t, 0);
    ecs_vec_init_t(NULL, &ctx.entity_parents,
        flecs_script_entity_parent_t, 0);
    if (flecs_script_dep_assign_refs(&ctx, &impl->refs)) {
        ecs_vec_fini_t(NULL, &ctx.vars, uint64_t);
        ecs_vec_fini_t(NULL, &ctx.component_owners,
            flecs_script_component_owner_t);
        ecs_vec_fini_t(NULL, &ctx.entity_parents,
            flecs_script_entity_parent_t);
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
    ecs_vec_fini_t(NULL, &ctx.vars, uint64_t);
    ecs_vec_fini_t(NULL, &ctx.component_owners,
        flecs_script_component_owner_t);
    ecs_vec_fini_t(NULL, &ctx.entity_parents,
        flecs_script_entity_parent_t);
    return result;
}

#endif
