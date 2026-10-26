#include "flecs.h"

#ifdef FLECS_SCRIPT
#include "../script.h"

typedef struct flecs_script_component_owner_t {
    ecs_script_entity_t *entity;
    ecs_script_id_t *id;
    ecs_script_scope_t *scope;
    int32_t entity_symbol;
    int32_t next;
} flecs_script_component_owner_t;

typedef struct flecs_script_dep_var_t {
    uint64_t input;
    uint64_t internal;
} flecs_script_dep_var_t;

typedef struct flecs_script_dep_ctx_t {
    ecs_script_eval_visitor_t *v;
    ecs_script_template_t *template;
    ecs_vec_t *refs;
    ecs_vec_t dyn_refs;
    ecs_vec_t vars;
    ecs_vec_t component_owners;
    ecs_map_t owner_index;
    int32_t owner_fuzzy_first;
    int32_t owner_fuzzy_last;
    int32_t *input_count;
    int32_t scope_count;
    int32_t component_count;
    int32_t for_count;
    int32_t computed_count;
    int32_t member;
    int32_t entity_symbol;
    ecs_entity_t parent_muts;
    bool reactive;
    bool no_deps;
    int32_t conditional;
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
    int32_t sp)
{
    return !expr && sp == -1;
}

static bool flecs_script_dep_static_elems_differ(
    const char *first_name,
    ecs_entity_t first_eval,
    int32_t first_symbol,
    const char *second_name,
    ecs_entity_t second_eval,
    int32_t second_symbol)
{
    if (first_symbol != -1 && second_symbol != -1) {
        return first_symbol != second_symbol;
    }

    if (first_eval && second_eval) {
        return first_eval != second_eval;
    }

    if (!first_name || !second_name) {
        return false;
    }

    return ecs_os_strcmp(
        flecs_script_name_leaf(first_name),
        flecs_script_name_leaf(second_name)) != 0;
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
            first->first_expr, first->first_sp) &&
        flecs_script_dep_id_elem_static(
            second->first_expr, second->first_sp) &&
        flecs_script_dep_static_elems_differ(
            first->first, first->first_eval, first->first_symbol,
            second->first, second->first_eval, second->first_symbol))
    {
        return false;
    }

    if (first->second && flecs_script_dep_id_elem_static(
            first->second_expr, first->second_sp) &&
        flecs_script_dep_id_elem_static(
            second->second_expr, second->second_sp) &&
        flecs_script_dep_static_elems_differ(
            first->second, first->second_eval, first->second_symbol,
            second->second, second->second_eval, second->second_symbol))
    {
        return false;
    }

    return true;
}

enum {
    FlecsScriptDepNameAny = -1,
    FlecsScriptDepNameInteger = -2
};

typedef struct flecs_script_dep_name_char_t {
    int16_t value;
    char fill;
} flecs_script_dep_name_char_t;

static void flecs_script_dep_name_text(
    ecs_vec_t *pattern,
    const char *text)
{
    if (text) {
        while (text[0]) {
            ecs_vec_append_t(NULL, pattern, flecs_script_dep_name_char_t)[0] =
                (flecs_script_dep_name_char_t){
                    .value = (unsigned char)*text ++ };
        }
    }
}

static void flecs_script_dep_name_expr(
    ecs_vec_t *pattern,
    const ecs_expr_node_t *expr,
    const ecs_expr_format_t *format)
{
    bool formatted = format && format->is_present;
    if (!formatted && expr->kind == EcsExprValue &&
        expr->type == ecs_id(ecs_string_t))
    {
        flecs_script_dep_name_text(pattern,
            *(char**)((const ecs_expr_value_node_t*)expr)->ptr);
        return;
    }

    if (expr->kind == EcsExprCast) {
        expr = ((const ecs_expr_cast_t*)expr)->expr;
    }

    ecs_vec_append_t(NULL, pattern, flecs_script_dep_name_char_t)[0] =
        (flecs_script_dep_name_char_t){
            .value = flecs_expr_is_type_integer(expr->type)
                ? FlecsScriptDepNameInteger : FlecsScriptDepNameAny,
            .fill = formatted ? format->fill : 0
        };
}

static void flecs_script_dep_name_pattern(
    ecs_vec_t *pattern,
    const ecs_script_entity_t *entity)
{
    ecs_vec_init_t(NULL, pattern, flecs_script_dep_name_char_t, 0);
    const ecs_expr_node_t *expr = entity->name_expr;
    if (!expr) {
        flecs_script_dep_name_text(pattern, entity->name);
    } else if (expr->kind == EcsExprInterpolatedString) {
        const ecs_expr_interpolated_string_t *str =
            (const ecs_expr_interpolated_string_t*)expr;
        const ecs_expr_fragment_t *fragments = ecs_vec_first(&str->fragments);
        int32_t i, count = ecs_vec_count(&str->fragments);
        for (i = 0; i < count; i ++) {
            flecs_script_dep_name_text(pattern, fragments[i].text);
            if (fragments[i].expr) {
                flecs_script_dep_name_expr(pattern, fragments[i].expr,
                    &fragments[i].format);
            }
        }
    } else {
        flecs_script_dep_name_expr(pattern, expr, NULL);
    }
}

static bool flecs_script_dep_name_accepts(
    const flecs_script_dep_name_char_t *pattern,
    int16_t ch)
{
    return pattern->value == FlecsScriptDepNameAny ||
        ch == (unsigned char)pattern->fill || ch == '-' || ch == '+' ||
        (ch >= '0' && ch <= '9');
}

static bool flecs_script_dep_names_may_match(
    const ecs_script_entity_t *first,
    const ecs_script_entity_t *second)
{
    if (!first->name_expr && !second->name_expr) {
        return false;
    }

    ecs_vec_t first_pattern, second_pattern;
    flecs_script_dep_name_pattern(&first_pattern, first);
    flecs_script_dep_name_pattern(&second_pattern, second);
    const flecs_script_dep_name_char_t *a = ecs_vec_first(&first_pattern);
    const flecs_script_dep_name_char_t *b = ecs_vec_first(&second_pattern);
    int32_t a_count = ecs_vec_count(&first_pattern);
    int32_t b_count = ecs_vec_count(&second_pattern);
    bool *matches = ecs_os_calloc_n(bool, b_count + 1);

    int32_t i, j;
    for (i = a_count; i >= 0; i --) {
        bool diagonal = false;
        for (j = b_count; j >= 0; j --) {
            bool below = matches[j];
            int16_t a_ch = i < a_count ? a[i].value : 0;
            int16_t b_ch = j < b_count ? b[j].value : 0;
            bool match = (i == a_count && j == b_count) ||
                (a_ch < 0 && below) ||
                (b_ch < 0 && matches[j + 1]);
            if (a_ch && b_ch) {
                if (a_ch < 0 && b_ch > 0) {
                    match |= flecs_script_dep_name_accepts(&a[i], b_ch) &&
                        matches[j + 1];
                } else if (b_ch < 0 && a_ch > 0) {
                    match |= flecs_script_dep_name_accepts(&b[j], a_ch) && below;
                } else if (a_ch > 0 && b_ch > 0) {
                    match |= a_ch == b_ch && diagonal;
                }
            }

            matches[j] = match;
            diagonal = below;
        }
    }

    bool result = matches[0];
    ecs_os_free(matches);
    ecs_vec_fini_t(NULL, &first_pattern, flecs_script_dep_name_char_t);
    ecs_vec_fini_t(NULL, &second_pattern, flecs_script_dep_name_char_t);
    return result;
}

static bool flecs_script_dep_entity_matches(
    ecs_script_entity_t *first,
    ecs_script_entity_t *second,
    bool fuzzy)
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
        (!fuzzy && (first->name_expr || second->name_expr)))
    {
        return false;
    }

    if (ecs_os_strcmp(first->name, second->name) &&
        (!fuzzy || !flecs_script_dep_names_may_match(first, second)))
    {
        return false;
    }

    return flecs_script_dep_entity_matches(
        first->parent, second->parent, fuzzy);
}

static bool flecs_script_dep_same_entity(
    const flecs_script_component_owner_t *owner,
    const flecs_script_dep_ctx_t *ctx)
{
    if (owner->entity || ctx->entity) {
        return flecs_script_dep_entity_matches(
            owner->entity, ctx->entity, true);
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
        first_entity, second_entity, false))
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

static bool flecs_script_dep_owner_key(
    const ecs_script_entity_t *entity,
    int32_t entity_symbol,
    uint64_t *key_out)
{
    uint64_t key = entity ? 14695981039346656037ull : (uint32_t)entity_symbol;
    while (entity) {
        if (!entity->name || entity->name_expr) {
            return false;
        }

        const char *name = entity->name;
        do {
            key = (key ^ (unsigned char)name[0]) * 1099511628211ull;
        } while (*(++ name));

        entity = entity->parent;
    }

    *key_out = key;
    return true;
}

static void flecs_script_dep_owner_append(
    flecs_script_component_owner_t *owners,
    int32_t *first,
    int32_t *last,
    int32_t index)
{
    if (*first == -1) {
        *first = index;
    } else {
        owners[*last].next = index;
    }

    *last = index;
}

static int32_t flecs_script_dep_owner_next(
    const flecs_script_component_owner_t *owners,
    int32_t count,
    int32_t cur[2],
    bool scan_all)
{
    if (scan_all) {
        return cur[0] < count ? cur[0] ++ : -1;
    }

    int32_t i = cur[1] != -1 && (cur[0] == -1 || cur[1] < cur[0]);
    int32_t result = cur[i];
    if (result != -1) {
        cur[i] = owners[result].next;
    }

    return result;
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
    int32_t count = ecs_vec_count(&ctx->component_owners);
    uint64_t key = 0;
    bool scan_all = !flecs_script_dep_owner_key(
        ctx->entity, ctx->entity_symbol, &key);
    int32_t cur[2] = {0, -1};
    if (!scan_all) {
        ecs_map_val_t *val = ecs_map_get(&ctx->owner_index, key);
        cur[0] = val ? (int32_t)(*val >> 32) - 1 : -1;
        cur[1] = ctx->entity ? ctx->owner_fuzzy_first : -1;
    }

    int32_t i;
    while ((i = flecs_script_dep_owner_next(
        owners, count, cur, scan_all)) != -1)
    {
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
    owner->next = -1;
    owners = ecs_vec_first(&ctx->component_owners);
    if (scan_all) {
        flecs_script_dep_owner_append(owners, &ctx->owner_fuzzy_first,
            &ctx->owner_fuzzy_last, count);
    } else {
        ecs_map_val_t *val = ecs_map_ensure(&ctx->owner_index, key);
        int32_t first = (int32_t)(*val >> 32) - 1;
        int32_t last = (int32_t)(*val & 0xffffffff) - 1;
        flecs_script_dep_owner_append(owners, &first, &last, count);
        *val = ((uint64_t)(uint32_t)(first + 1) << 32) | (uint32_t)(last + 1);
    }

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

static void flecs_script_dep_var_get(
    flecs_script_dep_ctx_t *ctx,
    int32_t sp,
    uint64_t *input,
    uint64_t *internal)
{
    if (sp < 0 || sp >= ecs_vec_count(&ctx->vars)) {
        return;
    }

    flecs_script_dep_var_t *var = ecs_vec_get_t(
        &ctx->vars, flecs_script_dep_var_t, sp);
    *input |= var->input;
    *internal |= var->internal;
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
    ecs_expr_node_t **node_ptr,
    void *ptr)
{
    ecs_expr_node_t *node = *node_ptr;
    if (!node) {
        return 0;
    }

    flecs_script_dep_expr_ctx_t *ctx = ptr;
    if (node->kind == EcsExprVariable) {
        flecs_script_dep_var_get(ctx->deps, ((ecs_expr_variable_t*)node)->sp,
            ctx->input, ctx->internal);
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

static int flecs_script_dep_expr_ref(
    const ecs_script_ref_t *value,
    ecs_expr_node_t *dynamic,
    void *ptr)
{
    flecs_script_dep_expr_ctx_t *expr = ptr;
    flecs_script_dep_ctx_t *ctx = expr->deps;
    if (value->entity) {
        ecs_script_ref_t *ref = flecs_script_ref_ensure(ctx->refs, value);
        if (!ref->input && flecs_script_dep_input_new(ctx, &ref->input)) {
            return -1;
        }

        *expr->input |= ref->input;
    }

    if (dynamic && ctx->reactive) {
        ecs_script_ref_t *ref = value->name
            ? flecs_script_ref_ensure(&ctx->dyn_refs, value)
            : &(ecs_script_ref_t){0};
        if (!ref->input && flecs_script_dep_input_new(ctx, &ref->input)) {
            return -1;
        }

        uint64_t input = ref->input;
        if (value->name && value->component == ctx->parent_muts &&
            !ecs_os_strcmp(value->name, "#parent"))
        {
            ctx->template->async_input &= ~input;
        }

        if (dynamic->kind == EcsExprHas) {
            ((ecs_expr_has_t*)dynamic)->dyn_input = input;
        } else {
            ((ecs_expr_element_t*)dynamic)->dyn_input = input;
        }

        *expr->input |= input;
    }

    return 0;
}

static int flecs_script_dep_expr(
    flecs_script_dep_ctx_t *ctx,
    ecs_expr_node_t *node,
    ecs_script_node_t *stmt)
{
    if (!node) {
        return 0;
    }

    uint64_t discard = 0, discard_internal = 0;
    flecs_script_dep_expr_ctx_t expr = {ctx,
        &stmt->direct_input, &stmt->direct_internal};

    /* Statements that must never be reevaluated by a reactive event don't
     * register refs, so nothing can trigger them. */
    if (ctx->no_deps) {
        expr.input = &discard;
        expr.internal = &discard_internal;
    } else if (flecs_expr_visit_refs(&ctx->v->base.script->pub, node,
        flecs_script_dep_expr_ref, &expr))
    {
        return -1;
    }

    return flecs_script_dep_expr_var(&node, &expr);
}

static int flecs_script_dep_id(
    flecs_script_dep_ctx_t *ctx,
    ecs_script_id_t *id,
    ecs_script_node_t *stmt)
{
    if (flecs_script_dep_expr(ctx, id->first_expr, stmt) ||
        flecs_script_dep_expr(ctx, id->second_expr, stmt) ||
        flecs_script_dep_expr(ctx, id->index_expr, stmt))
    {
        return -1;
    }

    flecs_script_dep_var_get(ctx, id->first_sp,
        &stmt->direct_input, &stmt->direct_internal);
    flecs_script_dep_var_get(ctx, id->second_sp,
        &stmt->direct_input, &stmt->direct_internal);
    flecs_script_dep_var_get(ctx, id->index_sp,
        &stmt->direct_input, &stmt->direct_internal);
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

    ecs_vec_t *slots = &ctx->v->base.script->state.symbol_slots;
    if (symbol >= ecs_vec_count(slots)) {
        return;
    }

    ecs_vec_get_t(slots,
        ecs_script_symbol_slot_t, symbol)->scope_slot = scope_slot;
}

static int flecs_script_dep_template(
    flecs_script_dep_ctx_t *ctx,
    ecs_script_template_node_t *node);

static void flecs_script_dep_region(
    flecs_script_dep_ctx_t *ctx,
    ecs_script_node_t *node,
    int32_t scope_first,
    int32_t for_first)
{
    node->region = 0;
    ecs_script_region_t region = {
        scope_first, ctx->scope_count - scope_first,
        for_first, ctx->for_count - for_first
    };
    if (!region.scope_count && !region.for_count) {
        return;
    }

    ecs_vec_t *regions = &ctx->v->base.script->regions;
    int32_t count = ecs_vec_count(regions);
    if (!count || ecs_os_memcmp(ecs_vec_last_t(regions, ecs_script_region_t),
        &region, ECS_SIZEOF(ecs_script_region_t)))
    {
        ecs_vec_append_t(NULL, regions, ecs_script_region_t)[0] = region;
        count ++;
    }

    node->region = count;
}

static int flecs_script_dep_child(
    flecs_script_dep_ctx_t *ctx,
    ecs_script_node_t *node,
    ecs_script_scope_t *scope,
    bool conditional)
{
    ctx->conditional += conditional;
    int result = flecs_script_dep_scope(ctx, scope);
    ctx->conditional -= conditional;
    node->input |= scope->node.input;
    node->internal |= scope->node.internal;
    return result;
}

static bool flecs_script_dep_component_partial(
    flecs_script_dep_ctx_t *ctx,
    ecs_script_component_t *node)
{
    if (!node->expr || node->expr->kind != EcsExprInitializer) {
        return false;
    }

    ecs_expr_initializer_t *initializer = (ecs_expr_initializer_t*)node->expr;
    bool partial = initializer->is_partial;
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

    return partial && !flecs_script_dep_initializer_is_complete(
        ctx->v->world, node->id.eval, initializer);
}

static int flecs_script_dep_node_impl(
    flecs_script_dep_ctx_t *ctx,
    ecs_script_node_t *node)
{
    node->input = 0;
    node->direct_input = 0;
    node->internal = 0;
    node->direct_internal = 0;
    int32_t scope_slot = ctx->scope ? ctx->scope->scope_slot : -1;

    switch(node->kind) {
    case EcsAstScope:
        return flecs_script_dep_scope(ctx, (ecs_script_scope_t*)node);
    case EcsAstTag:
    case EcsAstWithTag: {
        ecs_script_tag_t *n = (ecs_script_tag_t*)node;
        if (flecs_script_dep_id(ctx, &n->id, node)) {
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
        break;
    }
    case EcsAstComponent:
    case EcsAstWithComponent: {
        ecs_script_component_t *n = (ecs_script_component_t*)node;
        if (flecs_script_dep_id(ctx, &n->id, node) ||
            flecs_script_dep_expr(ctx, n->expr, node))
        {
            return -1;
        }

        if (node->kind == EcsAstComponent) {
            bool partial = flecs_script_dep_component_partial(ctx, n);
            bool already_owned = false;
            if (!partial && flecs_script_dep_component_owner(
                ctx, node, &n->id, &already_owned))
            {
                return -1;
            }

            n->component_slot = partial || already_owned
                ? -1
                : ctx->component_count ++;
        }
        break;
    }
    case EcsAstWith: {
        ecs_script_with_t *n = (ecs_script_with_t*)node;
        if (flecs_script_dep_scope(ctx, n->expressions)) {
            return -1;
        }

        node->direct_input = n->expressions->node.input;
        node->direct_internal = n->expressions->node.internal;
        if (flecs_script_dep_child(ctx, node, n->scope, false)) {
            return -1;
        }
        break;
    }
    case EcsAstUsing:
    case EcsAstAnnotation:
    case EcsAstInclude:
    case EcsAstContinue:
        break;
    case EcsAstModule:
        flecs_script_dep_symbol_scope(ctx,
            ((ecs_script_module_t*)node)->symbol, scope_slot);
        break;
    case EcsAstFunction:
        flecs_script_dep_symbol_scope(ctx,
            ((ecs_script_function_node_t*)node)->symbol, scope_slot);
        break;
    case EcsAstTemplate: {
        ecs_script_template_node_t *n = (ecs_script_template_node_t*)node;
        flecs_script_dep_symbol_scope(ctx, n->symbol, scope_slot);
        if (flecs_script_dep_template(ctx, n)) {
            return -1;
        }
        break;
    }
    case EcsAstProp:
    case EcsAstMut: {
        ecs_script_var_node_t *n = (ecs_script_var_node_t*)node;
        if (node->skip) {
            break;
        }

        ecs_script_template_member_t *member;
        if (n->base_member) {
            ecs_assert(ctx->template != NULL, ECS_INTERNAL_ERROR, NULL);
            member = ecs_vec_get_t(&ctx->template->members,
                ecs_script_template_member_t, n->base_member - 1);
        } else {
            if (!ctx->template ||
                ctx->member >= ecs_vec_count(&ctx->template->members))
            {
                flecs_script_eval_error(ctx->v, node,
                    "variable '%s' is not a template member", n->name);
                return -1;
            }

            member = ecs_vec_get_t(
                &ctx->template->members, ecs_script_template_member_t,
                ctx->member ++);
            flecs_script_dep_var_set(ctx, n->sp, member->input, 0);
        }

        if (flecs_script_dep_expr(ctx, n->expr, node)) {
            return -1;
        }

        node->input = node->direct_input | member->input;
        return 0;
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
        int dep_result = flecs_script_dep_expr(ctx, n->expr, node);
        ctx->no_deps = no_deps;
        if (dep_result) {
            return -1;
        }

        if (node->kind == EcsAstConst) {
            bool computed = ctx->reactive && !ctx->conditional &&
                !n->is_await && ctx->computed_count < 64;
            if (computed) {
                int32_t slot = ctx->computed_count ++;
                n->computed = slot + 1;
                flecs_script_dep_var_set(ctx, n->sp, 0, (uint64_t)1 << slot);
            } else {
                flecs_script_dep_var_set(ctx, n->sp,
                    node->direct_input, node->direct_internal);
            }
        } else {
            flecs_script_dep_symbol_scope(ctx, n->symbol, scope_slot);
        }
        break;
    }
    case EcsAstEntity: {
        ecs_script_entity_t *n = (ecs_script_entity_t*)node;
        flecs_script_dep_symbol_scope(ctx, n->symbol, scope_slot);
        if (flecs_script_dep_expr(ctx, n->name_expr, node)) {
            return -1;
        }

        flecs_script_dep_var_get(ctx, n->kind_sp,
            &node->direct_input, &node->direct_internal);
        int32_t entity_symbol = ctx->entity_symbol;
        ecs_script_entity_t *entity = ctx->entity;
        n->parent = entity;
        ctx->entity_symbol = n->symbol;
        ctx->entity = n;
        int result = flecs_script_dep_child(ctx, node, n->scope, false);
        ctx->entity_symbol = entity_symbol;
        ctx->entity = entity;
        if (result) {
            return -1;
        }
        break;
    }
    case EcsAstPairScope: {
        ecs_script_pair_scope_t *n = (ecs_script_pair_scope_t*)node;
        flecs_script_dep_symbol_scope(
            ctx, n->id.first_symbol, scope_slot);
        flecs_script_dep_symbol_scope(
            ctx, n->id.second_symbol, scope_slot);
        if (flecs_script_dep_id(ctx, &n->id, node) ||
            flecs_script_dep_child(ctx, node, n->scope, false))
        {
            return -1;
        }
        break;
    }
    case EcsAstIf: {
        ecs_script_if_t *n = (ecs_script_if_t*)node;
        n->if_true->alternative = n->if_false;
        n->if_false->alternative = n->if_true;
        if (flecs_script_dep_expr(ctx, n->expr, node) ||
            flecs_script_dep_child(ctx, node, n->if_true, true) ||
            flecs_script_dep_child(ctx, node, n->if_false, true))
        {
            return -1;
        }
        break;
    }
    case EcsAstFor: {
        ecs_script_for_t *n = (ecs_script_for_t*)node;
        if (flecs_script_dep_expr(ctx, n->from, node) ||
            flecs_script_dep_expr(ctx, n->to, node) ||
            flecs_script_dep_expr(ctx, n->expr, node))
        {
            return -1;
        }

        n->for_slot = ctx->for_count ++;
        int32_t i;
        for (i = 0; i < n->loop_var_count; i ++) {
            flecs_script_dep_var_set(ctx, n->loop_var_sp[i],
                node->direct_input, node->direct_internal);
        }

        if (flecs_script_dep_child(ctx, node, n->scope, true)) {
            return -1;
        }
        break;
    }
    case EcsAstAwait:
        if (flecs_script_dep_expr(ctx,
            ((ecs_script_await_t*)node)->expr, node))
        {
            return -1;
        }
        break;
    case EcsAstAsync:
        if (flecs_script_dep_child(ctx, node,
            ((ecs_script_async_t*)node)->scope, true))
        {
            return -1;
        }

        node->input = ctx->template ? ctx->template->async_input : 0;
        node->internal = 0;
        break;
    case EcsAstWhile: {
        ecs_script_while_t *n = (ecs_script_while_t*)node;
        if (flecs_script_dep_expr(ctx, n->expr, node) ||
            flecs_script_dep_child(ctx, node, n->scope, true))
        {
            return -1;
        }
        break;
    }
    case EcsAstAssign:
        if (flecs_script_dep_expr(ctx,
            ((ecs_script_assign_t*)node)->expr, node))
        {
            return -1;
        }
        break;
    case EcsAstTry: {
        ecs_script_try_t *n = (ecs_script_try_t*)node;
        if (flecs_script_dep_child(ctx, node, n->try_scope, true)) {
            return -1;
        }

        ecs_script_catch_t *catches = ecs_vec_first(&n->catches);
        int32_t i, count = ecs_vec_count(&n->catches);
        for (i = 0; i < count; i ++) {
            if (flecs_script_dep_child(ctx, node, catches[i].scope, true)) {
                return -1;
            }
        }
        break;
    }
    }

    node->input |= node->direct_input;
    node->internal |= node->direct_internal;
    return 0;
}

static int flecs_script_dep_node(
    flecs_script_dep_ctx_t *ctx,
    ecs_script_node_t *node)
{
    int32_t scope_first = ctx->scope_count, for_first = ctx->for_count;
    int result = flecs_script_dep_node_impl(ctx, node);
    if (!result) {
        flecs_script_dep_region(ctx, node, scope_first, for_first);
    }

    return result;
}

static int flecs_script_dep_scope(
    flecs_script_dep_ctx_t *ctx,
    ecs_script_scope_t *scope)
{
    int32_t for_first = ctx->for_count;
    ecs_script_scope_t *prev_scope = ctx->scope;
    ctx->scope = scope;
    scope->scope_slot = ctx->scope_count ++;
    scope->node.input = 0;
    scope->node.direct_input = 0;
    scope->node.internal = 0;
    scope->node.direct_internal = 0;
    ecs_script_node_t **stmts = ecs_vec_first(&scope->stmts);
    int32_t i, count = ecs_vec_count(&scope->stmts);
    for (i = 0; i < count; i ++) {
        if (flecs_script_dep_node(ctx, stmts[i])) {
            ctx->scope = prev_scope;
            return -1;
        }

        scope->node.input |= stmts[i]->input;
        scope->node.internal |= stmts[i]->internal;
    }

    uint64_t next_input = 0, next_internal = 0;
    for (i = count - 1; i >= 0; i --) {
        if (stmts[i]->kind == EcsAstAnnotation) {
            stmts[i]->input = next_input;
            stmts[i]->internal = next_internal;
            scope->node.input |= next_input;
            scope->node.internal |= next_internal;
        } else {
            next_input = stmts[i]->input;
            next_internal = stmts[i]->internal;
        }
    }

    ctx->scope = prev_scope;
    flecs_script_dep_region(ctx, &scope->node, scope->scope_slot, for_first);
    return 0;
}

static int flecs_script_dep_assign_refs(
    flecs_script_dep_ctx_t *ctx,
    ecs_vec_t *refs)
{
    ecs_script_ref_t *array = ecs_vec_first(refs);
    int32_t i, count = ecs_vec_count(refs);
    for (i = 0; i < count; i ++) {
        if (flecs_script_dep_input_new(ctx, &array[i].input)) {
            return -1;
        }
    }

    return 0;
}

static int flecs_script_dep_template_init(
    flecs_script_dep_ctx_t *ctx,
    flecs_script_dep_ctx_t *outer)
{
    ecs_script_template_t *template = ctx->template;
    template->input_count = 0;
    template->async_input = UINT64_MAX;
    ecs_script_template_member_t *members = ecs_vec_first(&template->members);
    int32_t i, count = ecs_vec_count(&template->members);
    for (i = 0; i < count; i ++) {
        if (flecs_script_dep_input_new(ctx, &members[i].input)) {
            return -1;
        }

        if (members[i].is_mut) {
            template->async_input &= ~members[i].input;
        }
    }

    if (flecs_script_dep_assign_refs(ctx, &template->refs)) {
        return -1;
    }

    if (template->parent_type) {
        const EcsScript *parent = ecs_get(ctx->v->world,
            template->parent_type, EcsScript);
        ecs_assert(parent != NULL && parent->template_ != NULL,
            ECS_INTERNAL_ERROR, NULL);
        ctx->parent_muts = parent->template_->muts.type;
    }

    int32_t *capture_sp = ecs_vec_first(&template->capture_sp);
    count = ecs_vec_count(&template->capture_sp);
    ecs_vec_set_count_t(NULL, &template->capture_input,
        ecs_script_template_capture_t, count);
    ecs_script_template_capture_t *captures = ecs_vec_first(
        &template->capture_input);
    for (i = 0; i < count; i ++) {
        uint64_t outer_input = 0, outer_internal = 0, input = 0;
        flecs_script_dep_var_get(outer, capture_sp[i],
            &outer_input, &outer_internal);
        if ((outer_input || outer_internal) &&
            flecs_script_dep_input_new(ctx, &input))
        {
            return -1;
        }

        captures[i].outer_input = outer_input;
        captures[i].outer_internal = outer_internal;
        captures[i].input = input;
        flecs_script_dep_var_set(ctx, i, input, 0);
    }

    for (i = 0; i < template->inherited_count; i ++) {
        flecs_script_dep_var_set(ctx, members[i].sp, members[i].input, 0);
    }

    ctx->member = template->inherited_count;
    return 0;
}

static int flecs_script_dep_analyze(
    flecs_script_dep_ctx_t *ctx,
    ecs_script_scope_t *scope,
    flecs_script_dep_ctx_t *outer)
{
    ecs_vec_init_t(NULL, &ctx->vars, flecs_script_dep_var_t, 0);
    ecs_vec_init_t(NULL, &ctx->dyn_refs, ecs_script_ref_t, 0);
    ecs_vec_init_t(NULL, &ctx->component_owners,
        flecs_script_component_owner_t, 0);
    ecs_map_init(&ctx->owner_index, NULL);
    ctx->owner_fuzzy_first = -1;
    ctx->owner_fuzzy_last = -1;

    int result = ctx->template
        ? flecs_script_dep_template_init(ctx, outer)
        : flecs_script_dep_assign_refs(ctx, ctx->refs);
    if (!result) {
        result = flecs_script_dep_scope(ctx, scope);
    }

    ecs_vec_fini_t(NULL, &ctx->vars, flecs_script_dep_var_t);
    ecs_vec_fini_t(NULL, &ctx->dyn_refs, ecs_script_ref_t);
    ecs_vec_fini_t(NULL, &ctx->component_owners,
        flecs_script_component_owner_t);
    ecs_map_fini(&ctx->owner_index);
    return result;
}

static int flecs_script_dep_template(
    flecs_script_dep_ctx_t *outer,
    ecs_script_template_node_t *node)
{
    ecs_entity_t entity = flecs_script_symbol_entity(outer->v, node->symbol);
    const EcsScript *script = ecs_get(outer->v->world, entity, EcsScript);
    if (!script || !script->template_) {
        flecs_script_eval_error(outer->v, node,
            "'%s' is not a template", node->name);
        return -1;
    }

    ecs_script_template_t *template = script->template_;
    int32_t *capture_sp = ecs_vec_first(&template->capture_sp);
    int32_t i, count = ecs_vec_count(&template->capture_sp);
    for (i = 0; i < count; i ++) {
        flecs_script_dep_var_get(outer, capture_sp[i],
            &node->node.direct_input, &node->node.direct_internal);
    }

    flecs_script_dep_ctx_t ctx = {
        .v = outer->v,
        .template = template,
        .refs = &template->refs,
        .input_count = &template->input_count,
        .entity_symbol = template->root_symbol,
        .reactive = true
    };

    if (flecs_script_dep_analyze(&ctx, template->node->scope, outer)) {
        return -1;
    }

    template->scope_count = ctx.scope_count;
    template->component_count = ctx.component_count;
    template->for_count = ctx.for_count;
    template->computed_count = ctx.computed_count;
    flecs_script_dep_symbol_scope(&ctx, template->root_symbol,
        template->node->scope->scope_slot);
    return 0;
}

int flecs_script_analyze_dependencies(
    ecs_script_eval_visitor_t *v,
    ecs_script_scope_t *scope)
{
    ecs_script_impl_t *impl = v->base.script;
    impl->input_count = 0;
    ecs_vec_clear(&impl->regions);
    flecs_script_dep_ctx_t ctx = {
        .v = v,
        .refs = &impl->refs,
        .input_count = &impl->input_count,
        .entity_symbol = -1,
        .reactive = impl->entity != 0
    };

    if (flecs_script_dep_analyze(&ctx, scope, NULL)) {
        return -1;
    }

    flecs_script_state_resize(&impl->state, ctx.scope_count,
        ctx.component_count, ctx.for_count, ctx.computed_count);
    return 0;
}

#endif
