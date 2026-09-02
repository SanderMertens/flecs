/**
 * @file addons/script/eval/visit_eval.c
 * @brief Script evaluation visitor.
 */

#include "flecs.h"

#ifdef FLECS_SCRIPT
#include "../script.h"
#include "../../meta/meta.h"

static ecs_script_symbol_slot_t* flecs_script_runtime_symbol_slot(
    ecs_script_eval_visitor_t *v,
    int32_t slot);

static ecs_entity_t flecs_script_component_slot_src(
    ecs_script_eval_visitor_t *v,
    const ecs_script_component_slot_t *slot);

static bool flecs_script_valid_lookup_path(
    const char *path)
{
    int32_t template_nesting = 0;
    char ch;
    for (; (ch = path[0]); path ++) {
        if (ch == '<') {
            template_nesting ++;
        } else if (ch == '>') {
            template_nesting --;
            if (template_nesting < 0) {
                return false;
            }
        }
    }

    return template_nesting == 0;
}

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

    if (!node && v->base.depth > 0) {
        node = v->base.nodes[v->base.depth - 1];
    }

    const char *name = v->base.script->pub.name;
    const char *code = v->base.script->pub.code;
    const char *pos = node ? node->pos : NULL;

    if (code && pos && (pos >= code) && (pos <= &code[ecs_os_strlen(code)])) {
        ecs_parser_error(name, code, pos - code, "%s", msg);
    } else {
        ecs_parser_error(name, NULL, 0, "%s", msg);
    }

    ecs_os_free(msg);
}

static ecs_value_t* flecs_script_with_append(
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

static void flecs_script_with_set_count(
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
                flecs_type_info_dtor(val->ptr, 1, ti);
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

static int32_t flecs_script_with_count(
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

static int32_t flecs_script_symbol_runtime_slot(
    const ecs_script_eval_visitor_t *v,
    int32_t slot)
{
    if (v->symbol_slots && slot >= v->symbol_offset) {
        return slot - v->symbol_offset;
    }
    return slot;
}

static ecs_script_symbol_slot_t* flecs_script_symbol_slot(
    const ecs_script_eval_visitor_t *v,
    int32_t slot)
{
    ecs_assert(v != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(slot >= 0, ECS_INTERNAL_ERROR, NULL);

    if (v->symbol_slots && slot >= v->symbol_offset) {
        int32_t index = slot - v->symbol_offset;
        ecs_assert(index < ecs_vec_count(v->symbol_slots),
            ECS_INTERNAL_ERROR, NULL);
        return ecs_vec_get_t(
            v->symbol_slots, ecs_script_symbol_slot_t, index);
    }

    ecs_vec_t *slots = &v->base.script->symbol_slots;
    ecs_assert(slot < ecs_vec_count(slots), ECS_INTERNAL_ERROR, NULL);
    return ecs_vec_get_t(slots, ecs_script_symbol_slot_t, slot);
}

ecs_entity_t flecs_script_symbol_entity(
    const ecs_script_eval_visitor_t *v,
    int32_t slot)
{
    return flecs_script_symbol_slot(v, slot)->entity;
}

void flecs_script_symbol_set(
    ecs_script_eval_visitor_t *v,
    int32_t slot,
    ecs_entity_t entity)
{
    ecs_assert(v != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(slot >= 0, ECS_INTERNAL_ERROR, NULL);

    ecs_script_symbol_slot_t *symbol = flecs_script_symbol_slot(v, slot);
    symbol->entity = entity;
    if (entity && v->scope_slot >= 0) {
        symbol->scope_slot = v->scope_slot;
    }
}

int flecs_script_symbol_lookup(
    const ecs_script_t *script,
    const ecs_expr_eval_desc_t *desc,
    ecs_entity_t from,
    const char *name,
    flecs_script_lookup_kind_t lookup_kind,
    flecs_script_symbol_t *symbol)
{
    ecs_assert(script != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(name != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(symbol != NULL, ECS_INTERNAL_ERROR, NULL);

    *symbol = (flecs_script_symbol_t){ .slot = -1, .sp = -1 };
    ecs_script_eval_visitor_t *v = desc ? desc->script_visitor : NULL;
    const ecs_script_vars_t *vars = desc ? desc->vars : NULL;
    bool explicit_var = name[0] == '$';
    const char *var_name = explicit_var ? &name[1] : name;

    if ((lookup_kind & FlecsScriptLookupVariable) && vars) {
        ecs_script_var_t *var = ecs_script_vars_lookup(vars, var_name);
        if (var) {
            symbol->kind = FlecsScriptSymbolVariable;
            symbol->sp = var->sp;
            return 0;
        }
    }

    if (explicit_var || !(lookup_kind & FlecsScriptLookupEntity)) {
        return -1;
    }

    if (v && v->type_visitor && !from &&
        !(lookup_kind & FlecsScriptLookupDynamic))
    {
        int result = flecs_script_type_symbol_lookup(
            v->type_visitor, name, symbol);
        if (!result) {
            return 0;
        }
    }

    ecs_world_t *world = script->world;
    ecs_entity_t result = 0;
    bool valid_path = flecs_script_valid_lookup_path(name);
    if (from && valid_path) {
        result = ecs_lookup_path_w_sep(
            world, from, name, NULL, NULL, false);
    } else if (valid_path) {
        result = ecs_lookup_path_w_sep(
            world, FlecsMeta, name, NULL, NULL, false);

        if (!result && v) {
            result = ecs_lookup_path_w_sep(
                world, v->parent, name, NULL, NULL, true);
        }

        if (!result && v) {
            int32_t i, using_count = ecs_vec_count(&v->r->using);
            ecs_entity_t *using = ecs_vec_first(&v->r->using);
            for (i = using_count - 1; i >= 0; i --) {
                result = ecs_lookup_path_w_sep(
                    world, using[i], name, NULL, NULL, false);
                if (result) {
                    break;
                }
            }
        }

        if (!result && desc && desc->lookup_action) {
            result = desc->lookup_action(world, name, desc->lookup_ctx);
        }
    }

    if (!result) {
        return -1;
    }

    symbol->entity = result;
    if (flecs_script_global_var_get(world, result, NULL).ptr) {
        symbol->kind = FlecsScriptSymbolGlobalVariable;
    } else {
        symbol->kind = FlecsScriptSymbolEntity;
    }
    return 0;
}

int flecs_script_id_elem_lookup(
    const ecs_script_t *script,
    const ecs_expr_eval_desc_t *desc,
    ecs_entity_t first,
    const char *name,
    flecs_script_lookup_kind_t lookup_kind,
    ecs_entity_t *from_out,
    flecs_script_symbol_t *symbol)
{
    ecs_entity_t from = 0;
    if (first) {
        from = flecs_get_oneof(script->world, first);
    }
    if (from_out) {
        from_out[0] = from;
    }
    return flecs_script_symbol_lookup(
        script, desc, from, name, lookup_kind, symbol);
}

int flecs_script_id_lookup(
    const ecs_script_t *script,
    const ecs_expr_eval_desc_t *desc,
    const char *first_name,
    const char *second_name,
    flecs_script_lookup_kind_t lookup_kind,
    ecs_entity_t *first_out,
    ecs_id_t *id_out,
    const char **unresolved)
{
    flecs_script_symbol_t symbol;
    if (flecs_script_id_elem_lookup(script, desc, 0, first_name,
        lookup_kind, NULL, &symbol) || !symbol.entity)
    {
        if (unresolved) {
            unresolved[0] = first_name;
        }
        return -1;
    }

    ecs_entity_t first = symbol.entity;
    if (first_out) {
        first_out[0] = first;
    }

    if (!second_name) {
        id_out[0] = first;
        return 0;
    }

    if (flecs_script_id_elem_lookup(script, desc, first, second_name,
        lookup_kind, NULL, &symbol) || !symbol.entity)
    {
        if (unresolved) {
            unresolved[0] = second_name;
        }
        return -1;
    }

    id_out[0] = ecs_pair(first, symbol.entity);
    return 0;
}

static ecs_entity_t flecs_script_eval_name_expr(
    ecs_script_eval_visitor_t *v,
    ecs_script_entity_t *node,
    ecs_expr_node_t **expr_ptr,
    bool find)
{
    ecs_script_t *script = &v->base.script->pub;
    ecs_expr_eval_desc_t desc = {
        .name = script->name,
        .vars = v->vars,
        .type = ecs_id(ecs_string_t),
        .runtime = v->r,
        .disable_dynamic_variable_binding = true,
        .script_visitor = v
    };

    ecs_expr_node_t *expr = *expr_ptr;
    ecs_assert(expr->type_info != NULL, ECS_INTERNAL_ERROR, NULL);

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
        flecs_script_symbol_t symbol;
        if (flecs_script_symbol_lookup(script, &desc, 0, name,
            FlecsScriptLookupEntity | FlecsScriptLookupDynamic, &symbol))
        {
            ecs_ptr_free(script->world, value.type, value.ptr);
            return 0;
        }
        result = symbol.entity;
    }

    ecs_ptr_free(script->world, value.type, value.ptr);

    return result;
}

bool flecs_script_is_builtin(
    const ecs_world_t *world,
    ecs_entity_t e)
{
    return ecs_has_pair(world, e, EcsChildOf, EcsFlecsCore);
}

bool flecs_script_is_script_scope(
    const ecs_world_t *world,
    ecs_entity_t script,
    ecs_entity_t e)
{
    while (script) {
        if (script == e) {
            return true;
        }
        script = ecs_get_target(world, script, EcsChildOf, 0);
    }
    return false;
}

bool flecs_script_can_own_entity(
    const ecs_world_t *world,
    ecs_entity_t script,
    ecs_entity_t e)
{
    if (flecs_script_is_builtin(world, e)) {
        return false;
    }

    if (flecs_script_is_script_scope(world, script, e)) {
        return false;
    }

    if (!script) {
        return true;
    }

    if (ecs_has_id(world, e, EcsModule)) {
        return false;
    }

    ecs_entity_t owner = ecs_get_target(world, e, ecs_id(EcsScript), 0);
    if (owner && (owner != script)) {
        return false;
    }

    return true;
}

static void flecs_script_apply_with(
    ecs_script_eval_visitor_t *v,
    ecs_entity_t entity)
{
    ecs_value_t *with = NULL;
    if (flecs_script_with_count(v)) {
        with = ecs_vec_first_t(&v->r->with, ecs_value_t);
    }
    if (!with) {
        return;
    }

    int32_t i;
    for (i = 0; with[i].type; i ++) {
        if (with[i].ptr) {
            const ecs_type_info_t *ti = ecs_get_type_info(
                v->world, with[i].type);
            ecs_assert(ti != NULL, ECS_INTERNAL_ERROR, NULL);
            ecs_set_id(v->world, entity, with[i].type,
                flecs_ito(size_t, ti->size), with[i].ptr);
        } else {
            ecs_add_id(v->world, entity, with[i].type);
        }
    }
}

ecs_entity_t flecs_script_create_entity(
    ecs_script_eval_visitor_t *v,
    const char *name)
{
    ecs_entity_desc_t desc = {0};

    if (v->entity && v->entity->node && v->entity->node->non_fragmenting_parent) {
        desc.id = ecs_new_w_parent(v->world, v->parent, name);
    } else {
        desc.parent = v->parent;
        desc.name = name;
    }

    ecs_entity_t result = ecs_entity_init(v->world, &desc);
    if (result && v->script_tag && flecs_script_can_own_entity(
        v->world, v->script_entity, result))
    {
        ecs_add_id(v->world, result, v->script_tag);
    }
    if (result) {
        flecs_script_apply_with(v, result);
    }

    return result;
}

int flecs_script_eval_id_elem(
    ecs_script_eval_visitor_t *v,
    void *node,
    ecs_expr_node_t **name_expr,
    ecs_entity_t eval,
    int32_t slot,
    int32_t sp,
    ecs_entity_t *elem)
{
    ecs_entity_t result = eval;
    if (name_expr && *name_expr) {
        result = flecs_script_eval_name_expr(v, NULL, name_expr, true);
    } else if (slot != -1) {
        result = flecs_script_symbol_entity(v, slot);
    } else if (sp != -1) {
        ecs_script_var_t *var = ecs_script_vars_from_sp(v->vars, sp);
        result = 0;
        if (var && var->value.type == ecs_id(ecs_entity_t) &&
            var->value.ptr)
        {
            result = *(ecs_entity_t*)var->value.ptr;
        }
    }

    if (result == EcsWildcard || result == EcsAny) {
        flecs_script_eval_error(v, node,
            "cannot use wildcard entity as id element");
        return -1;
    }

    elem[0] = result;
    return 0;
}

static int flecs_script_eval_id(
    ecs_script_eval_visitor_t *v,
    void *node,
    ecs_script_id_t *id)
{
    ecs_entity_t first;
    if (flecs_script_eval_id_elem(v, node, &id->first_expr,
        id->first_eval, id->first_symbol, id->first_sp, &first))
    {
        return -1;
    }
    if (!first) {
        flecs_script_eval_error(v, node,
            "unresolved identifier '%s'", id->first);
        return -1;
    }

    if (id->second) {
        ecs_entity_t second;
        if (flecs_script_eval_id_elem(v, node, &id->second_expr,
            id->second_eval, id->second_symbol, id->second_sp, &second))
        {
            return -1;
        }
        if (!second) {
            flecs_script_eval_error(v, node,
                "unresolved identifier '%s'", id->second);
            return -1;
        }
        id->eval = id->flag | ecs_pair(first, second);
    } else {
        id->eval = id->flag | first;
    }

    return 0;
}

int flecs_script_eval_expr(
    ecs_script_eval_visitor_t *v,
    ecs_expr_node_t **expr_ptr,
    ecs_value_t *value)
{
    ecs_assert(*expr_ptr != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert((*expr_ptr)->type_info != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_script_t *script = &v->base.script->pub;
    ecs_expr_eval_desc_t desc = {
        .name = script->name,
        .vars = v->vars,
        .type = value->type,
        .runtime = v->r,
        .disable_dynamic_variable_binding = true,
        .script_visitor = v
    };

    return flecs_expr_visit_eval(script, *expr_ptr, &desc, value);
}

void flecs_script_eval_scope_enter(
    ecs_script_eval_visitor_t *v,
    ecs_script_scope_t *node,
    flecs_script_scope_state_t *state)
{
    state->parent = v->parent;
    state->scope_slot = v->scope_slot;
    v->scope_slot = node->scope_slot;
    if (v->scope_slots && node->scope_slot >= 0 &&
        node->scope_slot < ecs_vec_count(v->scope_slots))
    {
        ecs_vec_get_t(v->scope_slots,
            int32_t, node->scope_slot)[0] = v->visit;
    }

    for (int32_t i = v->base.depth - 2; i >= 0; i --) {
        if (v->base.nodes[i]->kind == EcsAstScope) {
            node->parent = (ecs_script_scope_t*)v->base.nodes[i];
            break;
        }
    }

    v->vars = flecs_script_vars_push(v->vars, &v->r->stack, &v->r->allocator);
}

void flecs_script_eval_scope_leave(
    ecs_script_eval_visitor_t *v,
    const flecs_script_scope_state_t *state)
{
    v->vars = ecs_script_vars_pop(v->vars);
    v->parent = state->parent;
    v->scope_slot = state->scope_slot;
}

static void flecs_script_apply_non_fragmenting_childof(
    ecs_world_t *world,
    ecs_script_entity_t *node,
    bool enabled);

static void flecs_script_apply_non_fragmenting_childof_to_scope(
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
        if (flecs_script_node_is_hoisted(stmt)) {
            continue;
        }
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
                world, ((ecs_script_for_t*)stmt)->scope, enabled);
            break;
        case EcsAstTry: {
            ecs_script_try_t *try_stmt = (ecs_script_try_t*)stmt;
            flecs_script_apply_non_fragmenting_childof_to_scope(
                world, try_stmt->try_scope, enabled);
            int32_t c, catch_count = ecs_vec_count(&try_stmt->catches);
            ecs_script_catch_t *catches = ecs_vec_first(&try_stmt->catches);
            for (c = 0; c < catch_count; c ++) {
                flecs_script_apply_non_fragmenting_childof_to_scope(
                    world, catches[c].scope, enabled);
            }
            break;
        }
        case EcsAstWith:
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
        case EcsAstUsing:
        case EcsAstModule:
        case EcsAstAnnotation:
        case EcsAstTemplate:
        case EcsAstProp:
        case EcsAstMut:
        case EcsAstConst:
        case EcsAstExportConst:
        case EcsAstExportMut:
        case EcsAstInclude:
        case EcsAstFunction:
        case EcsAstAwait:
        case EcsAstContinue:
            break;
        }
    }
}

static void flecs_script_apply_non_fragmenting_childof(
    ecs_world_t *world,
    ecs_script_entity_t *node,
    bool enabled)
{
    node->non_fragmenting_parent = enabled;

    flecs_script_apply_non_fragmenting_childof_to_scope(
        world, node->scope, enabled);
}

void flecs_script_apply_tree_annot(
    ecs_script_annot_t *annot,
    ecs_script_entity_t *node)
{
    if (ecs_os_strcmp(annot->name, "tree")) {
        return;
    }

    if (!ecs_os_strcmp(annot->expr, "Parent")) {
        flecs_script_apply_non_fragmenting_childof(NULL, node, true);
    } else if (!ecs_os_strcmp(annot->expr, "ChildOf")) {
        flecs_script_apply_non_fragmenting_childof(NULL, node, false);
    }
}

int flecs_script_apply_annot(
    ecs_script_eval_visitor_t *v,
    ecs_script_entity_t *node,
    ecs_entity_t entity,
    ecs_script_annot_t *annot)
{
    ecs_entity_t e = entity;

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
    if (!ecs_os_strcmp(annot->name, "uuid")) {
        ecs_doc_set_uuid(v->world, e, annot->expr);
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

int flecs_script_eval_entity_enter(
    ecs_script_eval_visitor_t *v,
    ecs_script_entity_t *node,
    flecs_script_entity_state_t *state)
{
    state->node = node;
    state->eval = 0;
    state->eval_kind = 0;
    state->prev_entity = v->entity;
    state->prev_is_with_scope = v->is_with_scope;
    state->prev_template_entity = v->template_entity;
    state->prev_force = v->force;
    state->symbol = -1;
    state->for_slot = -1;

    if (v->entity && v->entity->eval_kind && !node->kind &&
        !ecs_vec_count(&node->scope->stmts) && ecs_has(
            v->world, v->entity->eval_kind, EcsScriptVisitor))
    {
        const char *parent_kind = v->entity->node->kind;
        flecs_script_eval_error(v, node,
            "invalid declaration for '%s', did you mean '%s %s(%s ...)'?",
                node->name ? node->name : "",
                parent_kind ? parent_kind : "struct",
                v->entity->node->name,
                node->name ? node->name : "");
        return -1;
    }

    if (node->kind) {
        state->eval_kind = node->eval_kind;
        if (node->kind_symbol != -1) {
            state->eval_kind = flecs_script_symbol_entity(
                v, node->kind_symbol);
        } else if (node->kind_sp != -1) {
            ecs_script_var_t *var = ecs_script_vars_from_sp(
                v->vars, node->kind_sp);
            if (var && var->value.ptr &&
                var->value.type == ecs_id(ecs_entity_t))
            {
                state->eval_kind = *(ecs_entity_t*)var->value.ptr;
            }
        }
        if (!state->eval_kind) {
            flecs_script_eval_error(v, node,
                "unresolved identifier '%s'", node->kind);
            return -1;
        }
    }

    if (node->symbol != -1) {
        state->symbol = flecs_script_symbol_runtime_slot(v, node->symbol);
        state->eval = flecs_script_symbol_entity(v, node->symbol);
        if (state->eval && (!ecs_is_alive(v->world, state->eval) ||
            ecs_get_target(v->world, state->eval, EcsChildOf, 0) != v->parent))
        {
            state->eval = 0;
        }
    } else if (node->eval && ecs_is_alive(v->world, node->eval)) {
        state->eval = node->eval;
    }

    bool apply_with = state->eval != 0;
    if (node->name_expr) {
        ecs_entity_t eval = flecs_script_eval_name_expr(
            v, node, &node->name_expr, false);
        if (!eval) {
            return -1;
        }
        if (state->eval && state->eval != eval &&
            ecs_is_alive(v->world, state->eval))
        {
            ecs_delete(v->world, state->eval);
        }
        state->eval = eval;
    } else if (!state->eval) {
        state->eval = flecs_script_create_entity(v, node->name);
    }
    if (apply_with && !node->name_expr) {
        flecs_script_apply_with(v, state->eval);
    }
    if (node->symbol != -1) {
        flecs_script_symbol_set(v, node->symbol, state->eval);
    }

    if (!flecs_script_is_builtin(v->world, state->eval) &&
        v->for_slots && v->for_slot >= 0 &&
        v->for_slot < ecs_vec_count(v->for_slots))
    {
        bool named = false;
        flecs_script_for_slot_track(v->world, ecs_vec_get_t(
            v->for_slots, ecs_script_for_slot_t, v->for_slot),
            state->eval, v->visit, &named);
        if (named) {
            state->for_slot = v->for_slot;
        }
    }

    if (v->template_entity) {
        ecs_add_pair(
            v->world, state->eval, EcsScriptTemplate, v->template_entity);
    }

    v->entity = state;

    if (state->eval_kind) {
        ecs_add_id(v->world, state->eval, state->eval_kind);
    }

    int32_t i, count = ecs_vec_count(&v->r->annot);
    if (count) {
        ecs_script_annot_t **annots = ecs_vec_first(&v->r->annot);
        for (i = 0; i < count ; i ++) {
            if (flecs_script_apply_annot(v, node, state->eval, annots[i])) {
                goto error;
            }
        }
        ecs_vec_clear(&v->r->annot);
    }

    v->is_with_scope = false;
    v->template_entity = 0;
    v->force = state->prev_force ||
        ((node->node.direct_input & v->input) != 0);

    return 0;
error:
    v->entity = state->prev_entity;
    v->force = state->prev_force;
    return -1;
}

void flecs_script_eval_entity_leave(
    ecs_script_eval_visitor_t *v,
    flecs_script_entity_state_t *state)
{
    ecs_script_entity_t *node = state->node;

    v->template_entity = state->prev_template_entity;
    v->is_with_scope = state->prev_is_with_scope;
    v->force = state->prev_force;

    if (state->eval_kind) {
        if (!node->kind_w_expr) {
            if (ecs_get_type_info(v->world, state->eval_kind) != NULL) {
                ecs_modified_id(v->world, state->eval, state->eval_kind);
            }
        }
    }

    v->entity = state->prev_entity;
}

static ecs_entity_t flecs_script_get_src(
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

static bool flecs_script_can_default_ctor(
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

static void flecs_script_track_component(
    ecs_script_eval_visitor_t *v,
    int32_t component_slot,
    ecs_id_t component)
{
    if (v->entity && v->entity->for_slot >= 0 && v->for_slots &&
        v->entity->for_slot < ecs_vec_count(v->for_slots))
    {
        flecs_script_for_slot_track_component(v->world, ecs_vec_get_t(
            v->for_slots, ecs_script_for_slot_t, v->entity->for_slot),
            v->entity->eval, component, v->visit);
        return;
    }

    if (!v->component_slots || !v->entity || v->entity->symbol < 0 ||
        component_slot < 0 ||
        component_slot >= ecs_vec_count(v->component_slots))
    {
        return;
    }
    int32_t entity_slot = v->entity->symbol;
    ecs_script_component_slot_t *slot = ecs_vec_get_t(
        v->component_slots, ecs_script_component_slot_t, component_slot);
    if (slot->component &&
        (slot->component != component ||
         slot->entity_slot != entity_slot))
    {
        ecs_entity_t src = flecs_script_component_slot_src(v, slot);
        if (src && ecs_is_alive(v->world, src)) {
            ecs_remove_id(v->world, src, slot->component);
        }
    }
    slot->entity_slot = entity_slot;
    slot->component = component;
    slot->scope_slot = v->scope_slot;
}

static const ecs_script_var_t* flecs_script_template_prop_var(
    ecs_script_eval_visitor_t *v,
    void *node,
    int32_t sp,
    ecs_id_t id)
{
    const ecs_script_var_t *var = ecs_script_vars_from_sp(v->vars, sp);
    if (!var || !var->value.ptr || var->value.type != id) {
        flecs_script_eval_error(v, node,
            "template prop used as component has no value");
        return NULL;
    }
    return var;
}

static int flecs_script_eval_tag(
    ecs_script_eval_visitor_t *v,
    ecs_script_tag_t *node)
{
    bool resolved = node->id.eval != 0;

    if (flecs_script_eval_id(v, node, &node->id)) {
        return -1;
    }

    if (node->id.eval == ecs_id(EcsParent)) {
        flecs_script_eval_error(v, node, 
            "Parent component cannot be added as tag (set to valid parent)",
            node->id.first, node->id.second);
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

    ecs_assert(v->entity != NULL, ECS_INTERNAL_ERROR, NULL);

    if (v->entity->eval_kind == ecs_id(EcsStruct) &&
        ECS_IS_PAIR(node->id.eval) &&
        ECS_PAIR_FIRST(node->id.eval) == EcsIsA)
    {
        ecs_entity_t base = ecs_pair_second(v->world, node->id.eval);
        if (!ecs_has(v->world, base, EcsStruct)) {
            flecs_script_eval_error(v, node,
                "base '%s' of struct '%s' is not a struct type",
                node->id.second, v->entity->node->name);
            return -1;
        }

        if (base == v->entity->eval) {
            flecs_script_eval_error(v, node,
                "struct '%s' cannot inherit from itself",
                v->entity->node->name);
            return -1;
        }

        if (flecs_struct_is_derived_from(v->world, base, v->entity->eval)) {
            flecs_script_eval_error(v, node,
                "inheritance cycle between struct '%s' and base '%s'",
                v->entity->node->name, node->id.second);
            return -1;
        }

        int32_t i = 0;
        ecs_entity_t existing;
        while ((existing = ecs_get_target(
            v->world, v->entity->eval, EcsIsA, i ++)))
        {
            if (existing != base && ecs_has(v->world, existing, EcsStruct)) {
                flecs_script_eval_error(v, node,
                    "struct '%s' cannot have multiple base types",
                    v->entity->node->name);
                return -1;
            }
        }
    }

    ecs_entity_t src = flecs_script_get_src(
        v, v->entity->eval, node->id.eval);

    if (node->id.value_sp != -1) {
        const ecs_script_var_t *var = flecs_script_template_prop_var(
            v, node, node->id.value_sp, node->id.eval);
        if (!var) {
            return -1;
        }
        const ecs_type_info_t *ti = flecs_script_get_type_info(
            v, node, node->id.eval);
        if (!ti) {
            return -1;
        }
        ecs_set_id(v->world, src, node->id.eval,
            flecs_itosize(ti->size), var->value.ptr);
    } else {
        ecs_add_id(v->world, src, node->id.eval);
    }

    flecs_script_track_component(
        v, node->component_slot, node->id.eval);

    return 0;
}

static int flecs_script_eval_interface_id(
    ecs_script_eval_visitor_t *v,
    ecs_script_component_t *node)
{
    ecs_entity_t tmpl;
    if (flecs_script_eval_id_elem(v, node, &node->id.first_expr,
        node->id.first_eval, node->id.first_symbol, node->id.first_sp, &tmpl))
    {
        return -1;
    }

    char *interface_str = ecs_get_path(v->world, node->id.interface);
    if (!tmpl) {
        flecs_script_eval_error(v, node,
            "template prop '%s' has no value: expected a template derived "
            "from '%s'", node->id.first, interface_str);
        ecs_os_free(interface_str);
        return -1;
    }

    const EcsScript *script = ecs_get(v->world, tmpl, EcsScript);
    if (!script || !script->template_ ||
        !flecs_struct_is_derived_from(v->world, tmpl, node->id.interface))
    {
        char *tmpl_str = ecs_get_path(v->world, tmpl);
        flecs_script_eval_error(v, node,
            "'%s' passed to template prop '%s' is not a template derived "
            "from '%s'", tmpl_str, node->id.first, interface_str);
        ecs_os_free(tmpl_str);
        ecs_os_free(interface_str);
        return -1;
    }

    ecs_os_free(interface_str);
    return 0;
}

static int flecs_script_eval_component(
    ecs_script_eval_visitor_t *v,
    ecs_script_component_t *node)
{
    bool resolved = node->id.eval != 0;

    if (node->id.interface && flecs_script_eval_interface_id(v, node)) {
        return -1;
    }

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

    ecs_assert(v->entity != NULL, ECS_INTERNAL_ERROR, NULL);

    if (!node->id.second) {
        const EcsScriptVisitor *visitor = ecs_get(
            v->world, node->id.eval, EcsScriptVisitor);
        if (visitor) {
            if (v->entity->eval == EcsVariable) {
                flecs_script_eval_error(v, node,
                    "cannot use '%s' in singleton scope", node->id.first);
                return -1;
            }

            if (node->expr && node->expr->kind != EcsExprInitializer &&
                node->expr->kind != EcsExprEmptyInitializer)
            {
                flecs_script_eval_error(v, node,
                    "expected initializer for '%s'", node->id.first);
                return -1;
            }

            ecs_script_visitor_ctx_t ctx = {
                .world = v->world,
                .entity = v->entity->eval,
                .kind = node->id.eval,
                .initializer = (ecs_expr_initializer_t*)node->expr,
                .eval = v,
                .ctx = visitor->ctx
            };

            ecs_assert(visitor->visit != NULL, ECS_INVALID_OPERATION,
                "EcsScriptVisitor component without visit callback");

            return visitor->visit(&ctx);
        }
    }

    ecs_entity_t src = flecs_script_get_src(v, v->entity->eval, node->id.eval);

    if (src == node->id.eval) {
        const EcsScript *tmpl = ecs_get(v->world, node->id.eval, EcsScript);
        if (tmpl && tmpl->template_) {
            flecs_script_eval_error(v, node,
                "cannot instantiate template '%s' on itself", node->id.first);
            return -1;
        }
    }

    if (node->expr) {
        const ecs_type_info_t *ti = flecs_script_get_type_info(
            v, node, node->id.eval);
        if (!ti) {
            return -1;
        }

        bool needs_set = ti->hooks.on_replace != NULL;

        bool partial = false;
        if (node->expr->kind == EcsExprInitializer) {
            partial = ((ecs_expr_initializer_t*)node->expr)->is_partial;
        }

        const void *existing = NULL;
        if (partial) {
            existing = ecs_get_id(v->world, src, node->id.eval);
        }

        /* Evaluate into temporary storage. Evaluating the expression can move
         * the entity, which would invalidate a pointer into the component
         * storage while members are still being assigned. */
        ecs_value_t value = {
            .ptr = ecs_os_alloca(ti->size),
            .type = ti->component
        };

        /* For full assignments, assign entire value including members not set
         * by expression. This prevents uninitialized or unexpected values. For
         * partial assignments, members not set by the expression keep the
         * existing component value. */
        if (!ti->hooks.ctor) {
            ecs_os_memset(value.ptr, 0, ti->size);
        } else {
            flecs_type_info_ctor(value.ptr, 1, ti);
        }

        if (node->id.value_sp != -1) {
            const ecs_script_var_t *var = flecs_script_template_prop_var(
                v, node, node->id.value_sp, node->id.eval);
            if (!var) {
                flecs_type_info_dtor(value.ptr, 1, ti);
                return -1;
            }
            ecs_ptr_copy_w_type_info(v->world, ti, value.ptr, var->value.ptr);
        } else if (existing) {
            ecs_ptr_copy_w_type_info(v->world, ti, value.ptr, existing);
        }

        if (flecs_script_eval_expr(v, &node->expr, &value)) {
            flecs_type_info_dtor(value.ptr, 1, ti);
            return -1;
        }

        if (needs_set) {
            ecs_set_id(v->world, src, node->id.eval,
                flecs_itosize(ti->size), value.ptr);
            flecs_type_info_dtor(value.ptr, 1, ti);
        } else {
            void *dst = ecs_ensure_id(v->world, src, node->id.eval,
                flecs_ito(size_t, ti->size));
            flecs_type_info_move_dtor(dst, value.ptr, 1, ti);
            ecs_modified_id(v->world, src, node->id.eval);
        }
    } else if (node->id.interface) {
        const ecs_type_info_t *ti = flecs_script_get_type_info(
            v, node, node->id.eval);
        if (!ti) {
            return -1;
        }
        void *value = ecs_os_alloca(ti->size);
        if (!ti->hooks.ctor) {
            ecs_os_memset(value, 0, ti->size);
        } else {
            flecs_type_info_ctor(value, 1, ti);
        }
        ecs_set_id(v->world, src, node->id.eval,
            flecs_itosize(ti->size), value);
    } else {
        ecs_add_id(v->world, src, node->id.eval);
    }

    flecs_script_track_component(
        v, node->component_slot, node->id.eval);

    return 0;
}

static int flecs_script_eval_with_tag(
    ecs_script_eval_visitor_t *v,
    ecs_script_tag_t *node)
{
    if (flecs_script_eval_id(v, node, &node->id)) {
        return -1;
    }

    ecs_allocator_t *a = &v->r->allocator;

    if (node->id.value_sp != -1) {
        const ecs_script_var_t *var = flecs_script_template_prop_var(
            v, node, node->id.value_sp, node->id.eval);
        if (!var) {
            return -1;
        }
        const ecs_type_info_t *ti = flecs_script_get_type_info(
            v, node, node->id.eval);
        if (!ti) {
            return -1;
        }
        ecs_value_t *value = flecs_script_with_append(a, v, ti);
        value->type = node->id.eval;
        value->ptr = flecs_stack_alloc(&v->r->stack, ti->size, ti->alignment);
        flecs_type_info_ctor(value->ptr, 1, ti);
        ecs_ptr_copy_w_type_info(v->world, ti, value->ptr, var->value.ptr);
        return 0;
    }

    ecs_value_t *value = flecs_script_with_append(a, v, NULL);
    value->type = node->id.eval;
    value->ptr = NULL;

    return 0;
}

static int flecs_script_eval_with_component(
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

        flecs_type_info_ctor(value->ptr, 1, ti);

        if (node->id.value_sp != -1) {
            const ecs_script_var_t *var = flecs_script_template_prop_var(
                v, node, node->id.value_sp, node->id.eval);
            if (!var) {
                return -1;
            }
            ecs_ptr_copy_w_type_info(v->world, ti, value->ptr, var->value.ptr);
        }

        if (flecs_script_eval_expr(v, &node->expr, value)) {
            return -1;
        }

        value->type = node->id.eval; // Restore so we're adding actual id
    }

    return 0;
}

int flecs_script_eval_with_enter(
    ecs_script_eval_visitor_t *v,
    ecs_script_with_t *node,
    flecs_script_with_state_t *state)
{
    state->with_count = flecs_script_with_count(v);
    state->cursor = flecs_stack_get_cursor(&v->r->stack);
    state->is_with_scope = v->is_with_scope;
    state->force = v->force;

    if (ecs_script_visit_scope(v, node->expressions)) {
        flecs_script_eval_with_leave(v, state);
        return -1;
    }

    v->is_with_scope = true;
    v->force = state->force ||
        ((node->node.direct_input & v->input) != 0);

    return 0;
}

void flecs_script_eval_with_leave(
    ecs_script_eval_visitor_t *v,
    const flecs_script_with_state_t *state)
{
    ecs_allocator_t *a = &v->r->allocator;
    v->is_with_scope = state->is_with_scope;
    v->force = state->force;
    flecs_script_with_set_count(a, v, state->with_count);
    flecs_stack_restore_cursor(&v->r->stack, state->cursor);
}

static int flecs_script_eval_using(
    ecs_script_eval_visitor_t *v,
    ecs_script_using_t *node)
{
    ecs_allocator_t *a = &v->r->allocator;
    int32_t len = ecs_os_strlen(node->name);
    bool wildcard = len > 2 && !ecs_os_strcmp(&node->name[len - 2], ".*");
    ecs_entity_t from = node->eval;
    ecs_assert(from != 0, ECS_INTERNAL_ERROR, NULL);

    if (wildcard) {
        ecs_iter_t it = ecs_children(v->world, from);
        while (ecs_children_next(&it)) {
            int32_t i, count = it.count;
            for (i = 0; i < count; i ++) {
                ecs_vec_append_t(
                    a, &v->r->using, ecs_entity_t)[0] = it.entities[i];
            }
        }
    } else {
        ecs_vec_append_t(a, &v->r->using, ecs_entity_t)[0] = from;
    }

    return 0;
}

static int flecs_script_eval_module(
    ecs_script_eval_visitor_t *v,
    ecs_script_module_t *node)
{
    ecs_assert(node->eval != 0, ECS_INTERNAL_ERROR, NULL);
    v->module = node->eval;
    v->parent = node->eval;
    return 0;
}

int flecs_script_eval_const(
    ecs_script_eval_visitor_t *v,
    ecs_script_var_node_t *node,
    bool export)
{
    const char *kind_str =
        node->node.kind == EcsAstExportMut ? "mut" : "const";

    /* Declare variable. If this variable is declared while instantiating a
     * template, the variable sp has already been resolved in all expressions
     * that used it, so we don't need to create the variable with a name. */
    ecs_script_var_t *var = NULL;
    if (!export) {
        var = ecs_script_vars_declare(v->vars, 
            v->template_entity ? NULL : node->name);
        if (!var) {
            flecs_script_eval_error(v, node, 
                "variable '%s' redeclared", node->name);
            return -1;
        }
    }

    ecs_entity_t type = node->eval_type;
    ecs_assert(type != 0, ECS_INTERNAL_ERROR, NULL);
    const ecs_type_info_t *ti = ecs_get_type_info(v->world, type);
    if (!ti) {
        flecs_script_eval_error(v, node,
            "invalid type %s for variable '%s'",
            flecs_errstr(ecs_get_path(v->world, type)), node->name);
        return -1;
    }

    ecs_value_t result = {
        .type = type
    };

    result.ptr = flecs_stack_calloc(&v->r->stack, ti->size, ti->alignment);
    flecs_type_info_ctor(result.ptr, 1, ti);

    if (flecs_script_eval_expr(v, &node->expr, &result)) {
        flecs_script_eval_error(v, node,
            "failed to evaluate expression for %s variable '%s'",
                kind_str, node->name);
        if (ti->hooks.dtor) {
            flecs_type_info_dtor(result.ptr, 1, ti);
        }
        flecs_stack_free(result.ptr, ti->size);
        return -1;
    }

    if (!export) {
        var->is_const = true;
        var->type_info = ti;
        var->value = result;
        var->owned = true;
    } else {
        ecs_entity_t const_var;
        if (node->node.kind == EcsAstExportMut) {
            const_var = ecs_mut_var(v->world, {
                .parent = v->parent,
                .name = node->name,
                .type = result.type,
                .value = result.ptr
            });
        } else {
            const_var = ecs_const_var(v->world, {
                .parent = v->parent,
                .name = node->name,
                .type = result.type,
                .value = result.ptr
            });
        }

        /* Clean up value since it'll have been copied into the const var. */
        if (ti->hooks.dtor) {
            flecs_type_info_dtor(result.ptr, 1, ti);
        }

        flecs_stack_free(result.ptr, ti->size);

        if (!const_var) {
            flecs_script_eval_error(v, node,
                "failed to create exported %s variable '%s'",
                    kind_str, node->name);
            return -1;
        }

        if (v->script_tag) {
            ecs_add_id(v->world, const_var, v->script_tag);
        }
        if (node->symbol != -1) {
            flecs_script_symbol_set(v, node->symbol, const_var);
        }
    }

    return 0;
}

int flecs_script_eval_pair_scope_enter(
    ecs_script_eval_visitor_t *v,
    ecs_script_pair_scope_t *node,
    flecs_script_pair_scope_state_t *state)
{
    ecs_entity_t first;
    if (flecs_script_eval_id_elem(v, node, &node->id.first_expr,
        node->id.first_eval, node->id.first_symbol, node->id.first_sp,
        &first))
    {
        return -1;
    }
    if (!first && node->id.first_symbol != -1) {
        first = flecs_script_create_entity(v, node->id.first);
        if (!first) {
            return -1;
        }
        flecs_script_symbol_set(v, node->id.first_symbol, first);
    } else if (!first) {
        return -1;
    }

    ecs_entity_t second;
    if (flecs_script_eval_id_elem(v, node, &node->id.second_expr,
        node->id.second_eval, node->id.second_symbol, node->id.second_sp,
        &second))
    {
        return -1;
    }
    if (!second && node->id.second_symbol != -1) {
        second = flecs_script_create_entity(v, node->id.second);
        if (second) {
            flecs_script_symbol_set(v, node->id.second_symbol, second);
        }
    }
    if (!second) {
        return -1;
    }

    ecs_allocator_t *a = &v->r->allocator;
    state->with_relationship = v->with_relationship;
    state->second = 0;
    state->with_relationship_sp = v->with_relationship_sp;
    state->force = v->force;

    v->with_relationship = first;

    if (state->with_relationship != first) {
        /* Append new element to with stack */
        ecs_value_t *value = flecs_script_with_append(a, v, NULL);
        value->type = ecs_pair(first, second);
        value->ptr = NULL;
        v->with_relationship_sp = flecs_script_with_count(v) - 1;
    } else {
        /* Get existing with element for current relationship stack */
        ecs_value_t *value = ecs_vec_get_t(
            &v->r->with, ecs_value_t, v->with_relationship_sp);
        ecs_assert(ECS_PAIR_FIRST(value->type) == (uint32_t)first, 
            ECS_INTERNAL_ERROR, NULL);
        state->second = ECS_PAIR_SECOND(value->type);
        value->type = ecs_pair(first, second);
        value->ptr = NULL;
    }

    v->force = state->force ||
        ((node->node.direct_input & v->input) != 0);
    return 0;
}

void flecs_script_eval_pair_scope_leave(
    ecs_script_eval_visitor_t *v,
    const flecs_script_pair_scope_state_t *state)
{
    ecs_allocator_t *a = &v->r->allocator;

    if (state->second) {
        ecs_value_t *value = ecs_vec_get_t(
            &v->r->with, ecs_value_t, v->with_relationship_sp);
        value->type = ecs_pair(v->with_relationship, state->second);
    } else {
        flecs_script_with_set_count(a, v, v->with_relationship_sp);
    }

    v->with_relationship = state->with_relationship;
    v->with_relationship_sp = state->with_relationship_sp;
    v->force = state->force;
}

static int flecs_script_eval_annot(
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

void flecs_script_user_function_callback(
    const ecs_function_ctx_t *ctx,
    int32_t argc,
    const ecs_value_t *argv,
    ecs_value_t *result)
{
    ecs_world_t *world = ctx->world;
    ecs_world_t *real_world = world;
    flecs_stage_from_world(&real_world);
    bool failed = false;

    const EcsScriptFunction *fcomp = ecs_get(
        real_world, ctx->function, EcsScriptFunction);
    if (!fcomp || !fcomp->binding_ctx) {
        ecs_err("script function entity is missing binding context");
        flecs_script_runtime_get(world)->error = true;
        return;
    }

    ecs_script_user_function_t *uf = fcomp->binding_ctx;
    ecs_script_function_node_t *node = uf->node;
    ecs_script_impl_t *impl = flecs_script_impl(uf->script);

    ecs_script_eval_visitor_t v;
    ecs_script_eval_desc_t desc = {0};
    flecs_script_eval_visit_init(impl, &v, &desc);

    ecs_allocator_t *a = &v.r->allocator;
    int32_t using_count = ecs_vec_count(&uf->using);
    ecs_entity_t *using = ecs_vec_first(&uf->using);
    for (int32_t u = 0; u < using_count; u ++) {
        ecs_vec_append_t(a, &v.r->using, ecs_entity_t)[0] = using[u];
    }

    v.vars = flecs_script_vars_push(v.vars, &v.r->stack, a);

    int32_t i, param_count = ecs_vec_count(&node->params);
    ecs_script_fn_param_t *params = ecs_vec_first(&node->params);

    for (i = 0; i < argc && i < param_count; i ++) {
        const ecs_type_info_t *ti = ecs_get_type_info(
            real_world, argv[i].type);
        ecs_script_var_t *var = ecs_script_vars_declare(
            v.vars, params[i].name);
        var->value.type = argv[i].type;
        var->type_info = ti;

        if (ti) {
            var->value.ptr = flecs_stack_calloc(
                &v.r->stack, ti->size, ti->alignment);
            flecs_type_info_ctor(var->value.ptr, 1, ti);
            ecs_ptr_copy_w_type_info(
                real_world, ti, var->value.ptr, argv[i].ptr);
            var->owned = true;
        } else {
            var->value.ptr = argv[i].ptr;
            var->owned = false;
        }
    }

    int32_t stmt_count = ecs_vec_count(&node->body->stmts);
    ecs_script_node_t **stmts = ecs_vec_first(&node->body->stmts);
    for (i = 0; i < stmt_count; i ++) {
        if (flecs_script_eval_node((ecs_script_visit_t*)&v, stmts[i])) {
            failed = true;
            goto done;
        }
    }

    if (node->return_expr) {
        if (flecs_script_eval_expr(&v, &node->return_expr, result)) {
            failed = true;
        }
    }

done:
    v.vars = ecs_script_vars_pop(v.vars);
    flecs_script_eval_visit_fini(&v, &desc);

    if (failed) {
        flecs_script_runtime_get(world)->error = true;
    }
}

void flecs_script_user_function_ctx_free(
    void *ctx)
{
    ecs_script_user_function_t *uf = ctx;
    if (uf->script) {
        ecs_script_free(uf->script);
    }
    ecs_vec_fini_t(NULL, &uf->refs, ecs_script_ref_t);
    ecs_vec_fini_t(NULL, &uf->using, ecs_entity_t);
    ecs_os_free(uf);
}

int flecs_script_eval_function(
    ecs_script_eval_visitor_t *v,
    ecs_script_function_node_t *node)
{
    ecs_world_t *world = v->world;

    int32_t param_count = ecs_vec_count(&node->params);
    if (param_count > FLECS_SCRIPT_FUNCTION_ARGS_MAX) {
        flecs_script_eval_error(v, node,
            "too many parameters for function '%s' (max %d)",
            node->name, FLECS_SCRIPT_FUNCTION_ARGS_MAX);
        return -1;
    }

    ecs_assert(node->eval_return_type != 0, ECS_INTERNAL_ERROR, NULL);

    int32_t i;
    ecs_script_fn_param_t *params = ecs_vec_first(&node->params);
    for (i = 0; i < param_count; i ++) {
        ecs_assert(params[i].eval_type != 0, ECS_INTERNAL_ERROR, NULL);
    }

    ecs_entity_t fn_entity = node->symbol != -1
        ? flecs_script_symbol_entity(v, node->symbol)
        : 0;
    ecs_assert(fn_entity != 0, ECS_INTERNAL_ERROR, NULL);

    ecs_vec_t fn_refs;
    ecs_vec_init_t(NULL, &fn_refs, ecs_script_ref_t, 0);

    int32_t stmt_count = ecs_vec_count(&node->body->stmts);
    ecs_script_node_t **stmts = ecs_vec_first(&node->body->stmts);
    for (i = 0; i < stmt_count; i ++) {
        ecs_script_var_node_t *var = (ecs_script_var_node_t*)stmts[i];
        ecs_assert(var->expr != NULL && var->expr->type_info != NULL,
            ECS_INTERNAL_ERROR, NULL);
        if (flecs_expr_visit_refs(&v->base.script->pub,
            var->expr, &fn_refs, NULL, NULL, &fn_refs))
        {
            ecs_vec_fini_t(NULL, &fn_refs, ecs_script_ref_t);
            return -1;
        }
    }
    if (node->return_expr && flecs_expr_visit_refs(
        &v->base.script->pub, node->return_expr,
        &fn_refs, NULL, NULL, &fn_refs))
    {
        ecs_vec_fini_t(NULL, &fn_refs, ecs_script_ref_t);
        return -1;
    }

    ecs_script_user_function_t *uf = ecs_os_calloc_t(
        ecs_script_user_function_t);
    uf->script = &v->base.script->pub;
    v->base.script->refcount ++;
    uf->node = node;
    uf->refs = fn_refs;
    uf->using = ecs_vec_copy_t(NULL, &v->r->using, ecs_entity_t);

    EcsScriptFunction *fcomp = ecs_ensure(world, fn_entity, EcsScriptFunction);
    if (fcomp->binding_ctx && fcomp->binding_ctx_free) {
        fcomp->binding_ctx_free(fcomp->binding_ctx);
    }
    fcomp->binding_ctx = uf;
    fcomp->binding_ctx_free = flecs_script_user_function_ctx_free;
    ecs_modified(world, fn_entity, EcsScriptFunction);

    return 0;
}

int flecs_script_eval_node(
    ecs_script_visit_t *_v,
    ecs_script_node_t *node)
{
    ecs_script_eval_visitor_t *v = (ecs_script_eval_visitor_t*)_v;
    ecs_assert(v != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(v->template == NULL, ECS_INTERNAL_ERROR, NULL);

    if (node->skip) {
        return 0;
    }

    switch(node->kind) {
    case EcsAstTag:
        return flecs_script_eval_tag(
            v, (ecs_script_tag_t*)node);
    case EcsAstComponent:
        return flecs_script_eval_component(
            v, (ecs_script_component_t*)node);
    case EcsAstWithTag:
        return flecs_script_eval_with_tag(
            v, (ecs_script_tag_t*)node);
    case EcsAstWithComponent:
        return flecs_script_eval_with_component(
            v, (ecs_script_component_t*)node);
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
        ecs_vec_clear(&v->r->annot);
        return flecs_script_template_update_vars(
            v, (ecs_script_template_node_t*)node);
    case EcsAstProp:
        return 0;
    case EcsAstMut:
        if (!v->template_entity) {
            flecs_script_eval_error(v, node,
                "mut variables are only allowed in templates");
            return -1;
        }
        return 0;
    case EcsAstConst:
        return flecs_script_eval_const(
            v, (ecs_script_var_node_t*)node, false);
    case EcsAstExportConst:
    case EcsAstExportMut:
        return 0;
    case EcsAstInclude:
        return flecs_script_eval_include(
            v, (ecs_script_include_t*)node);
    case EcsAstFunction:
        return 0;
    case EcsAstScope:
    case EcsAstEntity:
    case EcsAstWith:
    case EcsAstPairScope:
    case EcsAstIf:
    case EcsAstFor:
    case EcsAstTry:
    case EcsAstContinue:
        /* Compound statements are evaluated by the script runner */
        flecs_script_eval_error(v, node,
            "invalid context for compound statement");
        return -1;
    case EcsAstAwait:
        flecs_script_eval_error(v, node,
            "await requires resumable script execution");
        return -1;
    }

    ecs_abort(ECS_INTERNAL_ERROR, "corrupt AST node kind");
}

/* Script runner. Evaluates statements iteratively with an explicit frame
 * stack, so execution can suspend at await statements and resume without
 * having to rebuild a C call stack. Each frame owns the evaluation state of
 * the statement it executes; the AST is never written to during evaluation. */

static flecs_script_frame_t* flecs_script_frame_push(
    ecs_script_runner_t *r,
    ecs_script_node_t *node)
{
    ecs_script_visit_t *bv = &r->v.base;

    /* The parser enforces ECS_PARSER_MAX_RECURSION_DEPTH, which bounds the
     * frame and node stacks of any parsed script. */
    ecs_assert(bv->depth < ECS_SCRIPT_VISIT_MAX_DEPTH,
        ECS_INTERNAL_ERROR, NULL);
    ecs_assert(r->frame_count < ECS_SCRIPT_VISIT_MAX_DEPTH,
        ECS_INTERNAL_ERROR, NULL);

    bv->nodes[bv->depth ++] = node;

    flecs_script_frame_t *frame = &r->frames[r->frame_count ++];
    frame->node = node;
    frame->pc = 0;
    return frame;
}

void flecs_script_frame_pop(
    ecs_script_runner_t *r)
{
    ecs_script_visit_t *bv = &r->v.base;
    bv->depth --;
    ecs_assert(bv->nodes[bv->depth] ==
        r->frames[r->frame_count - 1].node, ECS_INTERNAL_ERROR, NULL);
    r->frame_count --;
}

void flecs_script_scope_push(
    ecs_script_runner_t *r,
    ecs_script_scope_t *scope)
{
    flecs_script_frame_t *frame = flecs_script_frame_push(
        r, (ecs_script_node_t*)scope);

    ecs_script_eval_visitor_t *v = &r->v;
    flecs_script_eval_scope_enter(v, scope, &frame->state.scope);

    if (v->entity) {
        ecs_entity_t src = v->entity->eval;
        int32_t count = ecs_vec_count(&scope->components);
        if (src != EcsVariable && count) {
            flecs_add_ids(
                v->world, src, ecs_vec_first(&scope->components), count);
        }
    }
}

static void flecs_script_mark_scope(
    ecs_script_eval_visitor_t *v,
    ecs_script_scope_t *scope);

static void flecs_script_mark_node(
    ecs_script_eval_visitor_t *v,
    ecs_script_node_t *node)
{
    switch(node->kind) {
    case EcsAstScope:
        flecs_script_mark_scope(v, (ecs_script_scope_t*)node);
        break;
    case EcsAstEntity:
        flecs_script_mark_scope(v, ((ecs_script_entity_t*)node)->scope);
        break;
    case EcsAstWith: {
        ecs_script_with_t *n = (ecs_script_with_t*)node;
        flecs_script_mark_scope(v, n->expressions);
        flecs_script_mark_scope(v, n->scope);
        break;
    }
    case EcsAstPairScope:
        flecs_script_mark_scope(v, ((ecs_script_pair_scope_t*)node)->scope);
        break;
    case EcsAstIf: {
        ecs_script_if_t *n = (ecs_script_if_t*)node;
        flecs_script_mark_scope(v, n->if_true);
        flecs_script_mark_scope(v, n->if_false);
        break;
    }
    case EcsAstFor: {
        ecs_script_for_t *n = (ecs_script_for_t*)node;
        if (v->for_slots && n->for_slot >= 0 &&
            n->for_slot < ecs_vec_count(v->for_slots))
        {
            flecs_script_for_slot_mark(ecs_vec_get_t(
                v->for_slots, ecs_script_for_slot_t, n->for_slot), v->visit);
        }
        flecs_script_mark_scope(v, n->scope);
        break;
    }
    case EcsAstTry: {
        ecs_script_try_t *n = (ecs_script_try_t*)node;
        flecs_script_mark_scope(v, n->try_scope);
        ecs_script_catch_t *catches = ecs_vec_first(&n->catches);
        int32_t i, count = ecs_vec_count(&n->catches);
        for (i = 0; i < count; i ++) {
            flecs_script_mark_scope(v, catches[i].scope);
        }
        break;
    }
    case EcsAstTag:
    case EcsAstComponent:
    case EcsAstWithTag:
    case EcsAstWithComponent:
    case EcsAstUsing:
    case EcsAstModule:
    case EcsAstAnnotation:
    case EcsAstTemplate:
    case EcsAstProp:
    case EcsAstMut:
    case EcsAstConst:
    case EcsAstExportConst:
    case EcsAstExportMut:
    case EcsAstInclude:
    case EcsAstFunction:
    case EcsAstAwait:
    case EcsAstContinue:
        break;
    }
}

static void flecs_script_mark_scope(
    ecs_script_eval_visitor_t *v,
    ecs_script_scope_t *scope)
{
    if (v->scope_slots && scope->scope_slot >= 0 &&
        scope->scope_slot < ecs_vec_count(v->scope_slots))
    {
        ecs_vec_get_t(v->scope_slots,
            int32_t, scope->scope_slot)[0] = v->visit;
    }
    ecs_script_node_t **stmts = ecs_vec_first(&scope->stmts);
    int32_t i, count = ecs_vec_count(&scope->stmts);
    for (i = 0; i < count; i ++) {
        flecs_script_mark_node(v, stmts[i]);
    }
}

static bool flecs_script_stmt_support(
    ecs_script_node_t *node)
{
    return node->kind == EcsAstConst ||
        node->kind == EcsAstUsing ||
        node->kind == EcsAstModule;
}

static bool flecs_script_stmt_run(
    ecs_script_eval_visitor_t *v,
    ecs_script_node_t *node)
{
    if (node->skip) {
        return false;
    }
    return v->force || flecs_script_stmt_support(node) ||
        ((node->input & v->input) != 0);
}

static int flecs_script_step_scope(
    ecs_script_runner_t *r,
    flecs_script_frame_t *frame)
{
    ecs_script_eval_visitor_t *v = &r->v;
    ecs_script_scope_t *scope = (ecs_script_scope_t*)frame->node;

    ecs_script_node_t **nodes = ecs_vec_first(&scope->stmts);
    int32_t count = ecs_vec_count(&scope->stmts);

    while (frame->pc < count) {
        ecs_script_node_t *stmt = nodes[frame->pc];
        v->base.prev = frame->pc ? nodes[frame->pc - 1] : NULL;
        v->base.next = (frame->pc + 1) < count ? nodes[frame->pc + 1] : NULL;

        /* Entities hoisted into the scope by "new" expressions are evaluated by
         * the statement that owns the expression. When that statement is
         * skipped the expression isn't evaluated, which means the scope of the
         * hoisted entity has to be marked as visited so cleanup doesn't reclaim
         * it. When the statement does run, the expression creates the entity
         * again and cleanup can reclaim what the statement no longer uses. */
        if (flecs_script_node_is_hoisted(stmt)) {
            if (!flecs_script_stmt_run(
                v, ((ecs_script_entity_t*)stmt)->hoisted_by))
            {
                flecs_script_mark_node(v, stmt);
            }
            frame->pc ++;
            continue;
        }

        if (!flecs_script_stmt_run(v, stmt)) {
            flecs_script_mark_node(v, stmt);
            frame->pc ++;
            continue;
        }

        if (stmt->kind == EcsAstContinue) {
            v->base.nodes[v->base.depth] = stmt;
            return 2;
        }

        if (stmt->kind == EcsAstAwait ||
            ((stmt->kind == EcsAstConst || stmt->kind == EcsAstExportConst ||
              stmt->kind == EcsAstExportMut) &&
                ((ecs_script_var_node_t*)stmt)->is_await))
        {
            ecs_assert(v->base.depth < ECS_SCRIPT_VISIT_MAX_DEPTH,
                ECS_INTERNAL_ERROR, NULL);
            v->base.nodes[v->base.depth ++] = stmt;
#ifdef FLECS_SCRIPT_ASYNC
            int res = flecs_script_step_await(r, stmt);
#else
            flecs_script_eval_error(v, stmt,
                "await requires resumable script execution");
            int res = -1;
#endif
            v->base.depth --;
            if (res) {
                return res;
            }
            frame->pc ++;
        } else if (stmt->kind == EcsAstScope) {
            frame->pc ++;
            flecs_script_scope_push(r, (ecs_script_scope_t*)stmt);
            return 0;
        } else if (stmt->kind == EcsAstEntity &&
            ((ecs_script_entity_t*)stmt)->is_type)
        {
            ecs_vec_clear(&v->r->annot);
            flecs_script_mark_node(v, stmt);
            frame->pc ++;
        } else if (stmt->kind == EcsAstEntity || stmt->kind == EcsAstIf ||
            stmt->kind == EcsAstFor || stmt->kind == EcsAstWith ||
            stmt->kind == EcsAstPairScope)
        {
            frame->pc ++;
            flecs_script_frame_push(r, stmt);
            return 0;
        } else if (stmt->kind == EcsAstTry) {
#ifdef FLECS_SCRIPT_ASYNC
            if (r->can_suspend) {
                frame->pc ++;
                flecs_script_frame_t *try_frame = flecs_script_frame_push(
                    r, stmt);
                try_frame->state.try_.catch_index = -1;
                return 0;
            }
#endif
            flecs_script_eval_error(v, stmt,
                "try requires resumable script execution");
            return -1;
        } else {
            ecs_assert(v->base.depth < ECS_SCRIPT_VISIT_MAX_DEPTH,
                ECS_INTERNAL_ERROR, NULL);
            v->base.nodes[v->base.depth ++] = stmt;
            int res = flecs_script_eval_node(&v->base, stmt);
            v->base.depth --;
            if (res) {
                return -1;
            }
            frame->pc ++;
        }
    }

    flecs_script_eval_scope_leave(v, &frame->state.scope);
    flecs_script_frame_pop(r);
    return 0;
}

static int flecs_script_step_entity(
    ecs_script_runner_t *r,
    flecs_script_frame_t *frame)
{
    ecs_script_entity_t *node = (ecs_script_entity_t*)frame->node;
    if (frame->pc == 0) {
        if (flecs_script_eval_entity_enter(
            &r->v, node, &frame->state.entity))
        {
            return -1;
        }
        frame->pc = 1;
        flecs_script_scope_push(r, node->scope);
        if (!r->v.template) {
            r->v.parent = frame->state.entity.eval;
        }
        return 0;
    }

    r->last_entity = frame->state.entity.eval;
    flecs_script_eval_entity_leave(&r->v, &frame->state.entity);
    flecs_script_frame_pop(r);
    return 0;
}

static int flecs_script_step_if(
    ecs_script_runner_t *r,
    flecs_script_frame_t *frame)
{
    ecs_script_eval_visitor_t *v = &r->v;
    ecs_script_if_t *node = (ecs_script_if_t*)frame->node;
    if (frame->pc == 0) {
        frame->state.if_.force = v->force;
        ecs_value_t condval = { .type = 0, .ptr = NULL };
        if (flecs_script_eval_expr(v, &node->expr, &condval)) {
            return -1;
        }

        bool cond;
        if (condval.type == ecs_id(ecs_bool_t)) {
            cond = *(bool*)(condval.ptr);
        } else {
            const EcsType *type = ecs_get(v->world, condval.type, EcsType);
            if (!type || (type->kind != EcsPrimitiveType &&
                type->kind != EcsEnumType && type->kind != EcsBitmaskType))
            {
                char *type_str = ecs_get_path(v->world, condval.type);
                flecs_script_eval_error(v, node,
                    "value of type '%s' cannot be used as condition", type_str);
                ecs_os_free(type_str);
                ecs_ptr_free(v->world, condval.type, condval.ptr);
                return -1;
            }

            ecs_meta_cursor_t cur = ecs_meta_cursor(
                v->world, condval.type, condval.ptr);
            cond = ecs_meta_get_bool(&cur);
        }

        ecs_ptr_free(v->world, condval.type, condval.ptr);

        frame->pc = 1;
        v->force = frame->state.if_.force ||
            ((node->node.direct_input & v->input) != 0);
        flecs_script_scope_push(r, cond ? node->if_true : node->if_false);
        return 0;
    }

    v->force = frame->state.if_.force;
    flecs_script_frame_pop(r);
    return 0;
}

static int flecs_script_step_with(
    ecs_script_runner_t *r,
    flecs_script_frame_t *frame)
{
    ecs_script_eval_visitor_t *v = &r->v;
    ecs_script_with_t *node = (ecs_script_with_t*)frame->node;
    if (frame->pc == 0) {
        if (flecs_script_eval_with_enter(v, node, &frame->state.with)) {
            return -1;
        }
        frame->pc = 1;
        flecs_script_scope_push(r, node->scope);
        return 0;
    }

    flecs_script_eval_with_leave(v, &frame->state.with);
    flecs_script_frame_pop(r);
    return 0;
}

static int flecs_script_step_pair_scope(
    ecs_script_runner_t *r,
    flecs_script_frame_t *frame)
{
    ecs_script_eval_visitor_t *v = &r->v;
    ecs_script_pair_scope_t *node = (ecs_script_pair_scope_t*)frame->node;
    if (frame->pc == 0) {
        if (flecs_script_eval_pair_scope_enter(
            v, node, &frame->state.pair_scope))
        {
            return -1;
        }
        frame->pc = 1;
        flecs_script_scope_push(r, node->scope);
        return 0;
    }

    flecs_script_eval_pair_scope_leave(v, &frame->state.pair_scope);
    flecs_script_frame_pop(r);
    return 0;
}

static void flecs_script_frame_leave(
    ecs_script_runner_t *r,
    flecs_script_frame_t *frame)
{
    ecs_script_eval_visitor_t *v = &r->v;
    switch(frame->node->kind) {
    case EcsAstScope:
        flecs_script_eval_scope_leave(v, &frame->state.scope);
        break;
    case EcsAstEntity:
        if (frame->pc >= 1) {
            flecs_script_eval_entity_leave(v, &frame->state.entity);
        }
        break;
    case EcsAstWith:
        if (frame->pc >= 1) {
            flecs_script_eval_with_leave(v, &frame->state.with);
        }
        break;
    case EcsAstPairScope:
        if (frame->pc >= 1) {
            flecs_script_eval_pair_scope_leave(v, &frame->state.pair_scope);
        }
        break;
    case EcsAstFor:
        if (frame->pc >= 1) {
            flecs_script_eval_for_leave(v, &frame->state.for_);
        }
        break;
    case EcsAstIf:
        if (frame->pc >= 1) {
            v->force = frame->state.if_.force;
        }
        break;
    case EcsAstTry:
        break;
    case EcsAstTag:
    case EcsAstComponent:
    case EcsAstWithTag:
    case EcsAstWithComponent:
    case EcsAstUsing:
    case EcsAstModule:
    case EcsAstAnnotation:
    case EcsAstTemplate:
    case EcsAstProp:
    case EcsAstMut:
    case EcsAstConst:
    case EcsAstExportConst:
    case EcsAstExportMut:
    case EcsAstInclude:
    case EcsAstFunction:
    case EcsAstAwait:
    default:
        ecs_abort(ECS_INTERNAL_ERROR, "corrupt script frame node");
    }
}

void flecs_script_runner_abandon(
    ecs_script_runner_t *r)
{
    while (r->frame_count) {
        flecs_script_frame_leave(r, &r->frames[r->frame_count - 1]);
        r->v.base.depth --;
        r->frame_count --;
    }
}

/* Pop frames until the for loop that contains the continue statement is
 * reached. Returns false if the statement isn't inside a for loop. */
static bool flecs_script_runner_continue(
    ecs_script_runner_t *r)
{
    int32_t frame = r->frame_count;
    while (frame > 0) {
        if (r->frames[frame - 1].node->kind == EcsAstFor) {
            break;
        }
        frame --;
    }

    if (!frame) {
        return false;
    }

    while (r->frame_count > frame) {
        flecs_script_frame_leave(r, &r->frames[r->frame_count - 1]);
        r->v.base.depth --;
        r->frame_count --;
    }

    return true;
}

/* Pop frames after an error until a try frame catches it. Returns true if a
 * catch clause was entered, false when the error propagates out of the
 * script. */
static bool flecs_script_runner_unwind(
    ecs_script_runner_t *r)
{
    while (r->frame_count) {
        flecs_script_frame_t *frame = &r->frames[r->frame_count - 1];

#ifdef FLECS_SCRIPT_ASYNC
        if (flecs_script_try_catch(r, frame)) {
            return true;
        }
#endif

        flecs_script_frame_leave(r, frame);
        r->v.base.depth --;
        r->frame_count --;
    }

#ifdef FLECS_SCRIPT_ASYNC
    if (r->thrown) {
        /* Thrown error wasn't caught by a try block, report it */
        flecs_script_report_throw(r);
    }
#endif
    return false;
}

static flecs_script_run_status_t flecs_script_runner_exec(
    ecs_script_runner_t *r)
{
    while (r->frame_count) {
        flecs_script_frame_t *frame = &r->frames[r->frame_count - 1];
        int res;
        switch(frame->node->kind) {
        case EcsAstScope:
            res = flecs_script_step_scope(r, frame);
            break;
        case EcsAstEntity:
            res = flecs_script_step_entity(r, frame);
            break;
        case EcsAstIf:
            res = flecs_script_step_if(r, frame);
            break;
        case EcsAstFor:
            res = flecs_script_step_for(r, frame);
            break;
        case EcsAstWith:
            res = flecs_script_step_with(r, frame);
            break;
        case EcsAstPairScope:
            res = flecs_script_step_pair_scope(r, frame);
            break;
        case EcsAstTry:
#ifdef FLECS_SCRIPT_ASYNC
            res = flecs_script_step_try(r, frame);
            break;
#endif
        case EcsAstTag:
        case EcsAstComponent:
        case EcsAstWithTag:
        case EcsAstWithComponent:
        case EcsAstUsing:
        case EcsAstModule:
        case EcsAstAnnotation:
        case EcsAstTemplate:
        case EcsAstProp:
        case EcsAstMut:
        case EcsAstConst:
        case EcsAstExportConst:
        case EcsAstExportMut:
        case EcsAstInclude:
        case EcsAstFunction:
        case EcsAstAwait:
        default:
            ecs_abort(ECS_INTERNAL_ERROR, "corrupt script frame node");
        }

        if (res == -1) {
            if (!flecs_script_runner_unwind(r)) {
                return FlecsScriptRunError;
            }
        } else if (res == 1) {
            return FlecsScriptRunSuspended;
        } else if (res == 2) {
            if (!flecs_script_runner_continue(r)) {
                flecs_script_eval_error(&r->v, frame->node,
                    "continue is only allowed inside a for loop");
                if (!flecs_script_runner_unwind(r)) {
                    return FlecsScriptRunError;
                }
            }
        }
    }

    return FlecsScriptRunDone;
}

void flecs_script_runner_init(
    ecs_script_runner_t *r,
    const ecs_script_impl_t *script,
    const ecs_script_eval_desc_t *desc)
{
    flecs_script_eval_visit_init(script, &r->v, desc);
    r->frame_count = 0;
    r->last_entity = 0;
    r->can_suspend = false;
#ifdef FLECS_SCRIPT_ASYNC
    r->future = NULL;
    r->async_entity = 0;
    r->thrown = NULL;
    r->throw_node = NULL;
#endif
}

void flecs_script_runner_fini(
    ecs_script_runner_t *r,
    const ecs_script_eval_desc_t *desc)
{
    flecs_script_runner_abandon(r);
#ifdef FLECS_SCRIPT_ASYNC
    if (r->future) {
        ecs_script_future_release(r->future);
        r->future = NULL;
    }
    flecs_script_throw_clear(r);
#endif
    flecs_script_eval_visit_fini(&r->v, desc);
}

/* Runner that borrows the state of an in-progress evaluation. Owns no
 * resources and needs no finalization; runs always return with an empty frame
 * stack since nested runners can't suspend. */
static void flecs_script_runner_init_nested(
    ecs_script_runner_t *r,
    const ecs_script_eval_visitor_t *v)
{
    r->v = *v;
    r->frame_count = 0;
    r->last_entity = 0;
    r->can_suspend = false;
#ifdef FLECS_SCRIPT_ASYNC
    r->future = NULL;
    r->async_entity = 0;
    r->thrown = NULL;
    r->throw_node = NULL;
#endif
}

flecs_script_run_status_t flecs_script_runner_run_scope(
    ecs_script_runner_t *r,
    ecs_script_scope_t *scope)
{
    if (!r->frame_count) {
        if (!scope) {
            return FlecsScriptRunError;
        }
        flecs_script_scope_push(r, scope);
    }
    return flecs_script_runner_exec(r);
}

int flecs_script_eval_entity(
    ecs_script_eval_visitor_t *v,
    const ecs_script_t *script,
    const ecs_script_vars_t *vars,
    ecs_script_entity_t *node,
    ecs_entity_t *out)
{
    ecs_script_runner_t runner;
    ecs_script_eval_desc_t desc = {0};

    if (v) {
        flecs_script_runner_init_nested(&runner, v);
    } else {
        flecs_script_runner_init(&runner,
            (const ecs_script_impl_t*)script, &desc);
        if (vars) {
            flecs_script_eval_push_vars(&runner.v, vars);
        }
    }

    flecs_script_frame_push(&runner, (ecs_script_node_t*)node);
    flecs_script_run_status_t status = flecs_script_runner_exec(&runner);
    if (status == FlecsScriptRunDone && out) {
        *out = runner.last_entity;
    }

    ecs_assert(runner.frame_count == 0, ECS_INTERNAL_ERROR, NULL);
    if (!v) {
        if (vars) {
            flecs_script_eval_pop_vars(&runner.v);
        }
        flecs_script_runner_fini(&runner, &desc);
    }

    return status == FlecsScriptRunDone ? 0 : -1;
}

/* Source position of the statement the runner is suspended on */
const char* flecs_script_runner_stmt_pos(
    const ecs_script_runner_t *r)
{
    int32_t i;
    for (i = r->frame_count - 1; i >= 0; i --) {
        const flecs_script_frame_t *frame = &r->frames[i];
        if (frame->node->kind != EcsAstScope) {
            continue;
        }

        ecs_script_scope_t *scope = (ecs_script_scope_t*)frame->node;
        if (frame->pc < ecs_vec_count(&scope->stmts)) {
            ecs_script_node_t **nodes = ecs_vec_first(&scope->stmts);
            return nodes[frame->pc]->pos;
        }
    }

    return NULL;
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
        .r = desc ? desc->runtime : NULL,
        .component_slots = ECS_CONST_CAST(
            ecs_vec_t*, &script->component_slots),
        .scope_slots = ECS_CONST_CAST(ecs_vec_t*, &script->scope_slots),
        .for_slots = ECS_CONST_CAST(ecs_vec_t*, &script->for_slots),
        .input = UINT64_MAX,
        .scope_slot = -1,
        .for_slot = -1,
        .force = true
    };

    if (!v->r) {
        v->r = ecs_script_runtime_new();
    }

    ecs_id_t tag = flecs_script_runtime_get(v->world)->current_tag;
    if (tag) {
        v->script_tag = tag;
        if (ECS_HAS_ID_FLAG(tag, PAIR)) {
            if (ECS_PAIR_FIRST(tag) == ecs_id(EcsScript)) {
                v->script_entity = ecs_pair_second(v->world, tag);
            }
        }
    }

}

void flecs_script_eval_visit_fini(
    ecs_script_eval_visitor_t *v,
    const ecs_script_eval_desc_t *desc)
{
    if (!desc || (v->r != desc->runtime)) {
        ecs_script_runtime_free(v->r);
    }
}

void flecs_script_eval_begin(
    ecs_script_eval_visitor_t *v,
    uint64_t input,
    int32_t visit)
{
    v->input = input;
    v->visit = visit;
    v->scope_slot = -1;
    v->for_slot = -1;
    v->force = input == UINT64_MAX;
}

static ecs_script_symbol_slot_t* flecs_script_runtime_symbol_slot(
    ecs_script_eval_visitor_t *v,
    int32_t slot)
{
    ecs_vec_t *slots = v->symbol_slots
        ? v->symbol_slots
        : &v->base.script->symbol_slots;
    if (slot < 0 || slot >= ecs_vec_count(slots)) {
        return NULL;
    }
    return ecs_vec_get_t(slots, ecs_script_symbol_slot_t, slot);
}

static bool flecs_script_scope_visited(
    ecs_script_eval_visitor_t *v,
    int32_t slot)
{
    if (!v->scope_slots || slot < 0 ||
        slot >= ecs_vec_count(v->scope_slots))
    {
        return true;
    }
    return ecs_vec_get_t(v->scope_slots, int32_t, slot)[0] == v->visit;
}

static ecs_entity_t flecs_script_component_slot_src(
    ecs_script_eval_visitor_t *v,
    const ecs_script_component_slot_t *slot)
{
    ecs_script_symbol_slot_t *symbol = flecs_script_runtime_symbol_slot(
        v, slot->entity_slot);
    if (!symbol || !symbol->entity) {
        return 0;
    }
    return flecs_script_get_src(v, symbol->entity, slot->component);
}

static void flecs_script_cleanup_for_scope(
    ecs_script_eval_visitor_t *v,
    ecs_script_scope_t *scope);

static void flecs_script_cleanup_for_node(
    ecs_script_eval_visitor_t *v,
    ecs_script_node_t *node)
{
    switch(node->kind) {
    case EcsAstScope:
        flecs_script_cleanup_for_scope(v, (ecs_script_scope_t*)node);
        break;
    case EcsAstEntity:
        flecs_script_cleanup_for_scope(
            v, ((ecs_script_entity_t*)node)->scope);
        break;
    case EcsAstWith: {
        ecs_script_with_t *n = (ecs_script_with_t*)node;
        flecs_script_cleanup_for_scope(v, n->expressions);
        flecs_script_cleanup_for_scope(v, n->scope);
        break;
    }
    case EcsAstPairScope:
        flecs_script_cleanup_for_scope(
            v, ((ecs_script_pair_scope_t*)node)->scope);
        break;
    case EcsAstIf: {
        ecs_script_if_t *n = (ecs_script_if_t*)node;
        flecs_script_cleanup_for_scope(v, n->if_true);
        flecs_script_cleanup_for_scope(v, n->if_false);
        break;
    }
    case EcsAstFor: {
        ecs_script_for_t *n = (ecs_script_for_t*)node;
        if (v->for_slots && n->for_slot >= 0 &&
            n->for_slot < ecs_vec_count(v->for_slots))
        {
            ecs_script_for_slot_t *slot = ecs_vec_get_t(
                v->for_slots, ecs_script_for_slot_t, n->for_slot);
            if (!flecs_script_scope_visited(v, n->scope->scope_slot)) {
                flecs_script_for_slot_clear(v->world, slot, true);
            } else {
                flecs_script_for_slot_purge(v->world, slot, v->visit);
            }
        }
        flecs_script_cleanup_for_scope(v, n->scope);
        break;
    }
    case EcsAstTry: {
        ecs_script_try_t *n = (ecs_script_try_t*)node;
        flecs_script_cleanup_for_scope(v, n->try_scope);
        ecs_script_catch_t *catches = ecs_vec_first(&n->catches);
        int32_t i, count = ecs_vec_count(&n->catches);
        for (i = 0; i < count; i ++) {
            flecs_script_cleanup_for_scope(v, catches[i].scope);
        }
        break;
    }
    case EcsAstTag:
    case EcsAstComponent:
    case EcsAstWithTag:
    case EcsAstWithComponent:
    case EcsAstUsing:
    case EcsAstModule:
    case EcsAstAnnotation:
    case EcsAstTemplate:
    case EcsAstProp:
    case EcsAstMut:
    case EcsAstConst:
    case EcsAstExportConst:
    case EcsAstExportMut:
    case EcsAstInclude:
    case EcsAstFunction:
    case EcsAstAwait:
    case EcsAstContinue:
        break;
    }
}

static void flecs_script_cleanup_for_scope(
    ecs_script_eval_visitor_t *v,
    ecs_script_scope_t *scope)
{
    ecs_script_node_t **stmts = ecs_vec_first(&scope->stmts);
    int32_t i, count = ecs_vec_count(&scope->stmts);
    for (i = 0; i < count; i ++) {
        flecs_script_cleanup_for_node(v, stmts[i]);
    }
}

void flecs_script_eval_cleanup(
    ecs_script_eval_visitor_t *v)
{
    if (v->component_slots) {
        ecs_script_component_slot_t *slots = ecs_vec_first(
            v->component_slots);
        int32_t i, j, count = ecs_vec_count(v->component_slots);
        for (i = 0; i < count; i ++) {
            ecs_script_component_slot_t *slot = &slots[i];
            if (!slot->component ||
                flecs_script_scope_visited(v, slot->scope_slot))
            {
                continue;
            }
            ecs_entity_t src = flecs_script_component_slot_src(v, slot);
            bool active = false;
            for (j = 0; j < count; j ++) {
                if (i == j || slots[j].component != slot->component) {
                    continue;
                }
                if (!flecs_script_scope_visited(v, slots[j].scope_slot)) {
                    continue;
                }
                if (slots[j].entity_slot == slot->entity_slot) {
                    active = true;
                    break;
                }
                if (src && flecs_script_component_slot_src(
                    v, &slots[j]) == src)
                {
                    active = true;
                    break;
                }
            }
            if (src && ecs_is_alive(v->world, src) && !active) {
                ecs_remove_id(v->world, src, slot->component);
            }
            ecs_os_zeromem(slot);
        }
    }

    ecs_vec_t *symbol_slots = v->symbol_slots
        ? v->symbol_slots
        : &v->base.script->symbol_slots;
    ecs_script_symbol_slot_t *symbols = ecs_vec_first(symbol_slots);
    int32_t i, j, count = ecs_vec_count(symbol_slots);
    for (i = 0; i < count; i ++) {
        ecs_script_symbol_slot_t *slot = &symbols[i];
        if (!slot->entity || flecs_script_scope_visited(v, slot->scope_slot)) {
            continue;
        }
        if (ecs_is_alive(v->world, slot->entity) &&
            flecs_script_is_builtin(v->world, slot->entity))
        {
            continue;
        }
        bool active = false;
        for (j = 0; j < count; j ++) {
            if (i == j || symbols[j].entity != slot->entity) {
                continue;
            }
            if (flecs_script_scope_visited(v, symbols[j].scope_slot)) {
                active = true;
                break;
            }
        }
        if (!active && ecs_is_alive(v->world, slot->entity)) {
            ecs_delete(v->world, slot->entity);
        }
        ecs_os_zeromem(slot);
    }

    ecs_script_scope_t *scope = v->instance_template
        ? v->instance_template->node->scope
        : v->base.script->root;
    flecs_script_cleanup_for_scope(v, scope);
}

void flecs_script_eval_push_vars(
    ecs_script_eval_visitor_t *v,
    const ecs_script_vars_t *vars)
{
    /* Safe const cast, evaluated code only contains an entity statement and
     * won't modify the variables. */
    v->vars = flecs_script_vars_push(
        v->vars, &v->r->stack, &v->r->allocator);
    v->vars->parent = ECS_CONST_CAST(ecs_script_vars_t*, vars);
    v->vars->sp = ecs_vec_count(&vars->vars);
}

void flecs_script_eval_pop_vars(
    ecs_script_eval_visitor_t *v)
{
    v->vars = ecs_script_vars_pop(v->vars);
}

int flecs_script_eval(
    const ecs_script_t *script,
    const ecs_script_eval_desc_t *desc,
    ecs_id_t tag,
    uint64_t input,
    ecs_script_eval_result_t *result)
{
    ecs_script_runner_t runner;
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

    ecs_script_runtime_t *runtime = flecs_script_runtime_get(script->world);
    flecs_script_runtime_error_reset(runtime);

    if (result) {
        flecs_log_capture_push(true);
    }

    ecs_id_t prev_tag = runtime->current_tag;
    runtime->current_tag = tag;

    flecs_script_runner_init(&runner, impl, &priv_desc);
    int r = 0;
    if (!impl->compiled) {
        if (flecs_script_visit_include(&runner.v, impl->root)) {
            r = -1;
        } else if (flecs_script_visit_type(&runner.v, impl->root)) {
            r = -1;
        } else if (flecs_script_analyze_dependencies(
            &runner.v, impl->root))
        {
            r = -1;
        } else {
            impl->compiled = true;
        }
    }
    if (!r && input) {
        if (impl->visit == INT32_MAX) {
            if (ecs_vec_count(&impl->scope_slots)) {
                ecs_os_memset(ecs_vec_first(&impl->scope_slots), 0,
                    ecs_vec_count(&impl->scope_slots) * ECS_SIZEOF(int32_t));
            }
            impl->visit = 1;
        } else {
            impl->visit ++;
        }
        flecs_script_eval_begin(&runner.v, input, impl->visit);
        if (flecs_script_runner_run_scope(&runner, impl->root) !=
            FlecsScriptRunDone)
        {
            r = -1;
        } else {
            flecs_script_eval_cleanup(&runner.v);
        }
    }
    flecs_script_runner_fini(&runner, &priv_desc);

    runtime->current_tag = prev_tag;

    if (runtime->error) {
        runtime->error = false;
        r = -1;
    }

    if (result) {
        char *unresolved = priv_desc.runtime->unresolved_errors;
        priv_desc.runtime->unresolved_errors = NULL;
        result->error = flecs_log_capture_pop();
        flecs_log_get_captured_error_pos(&result->line, &result->column);
        if (unresolved) {
            if (!result->error) {
                result->error = unresolved;
            } else if (!ecs_os_strncmp(unresolved, result->error,
                ecs_os_strlen(result->error)))
            {
                ecs_os_free(result->error);
                result->error = unresolved;
            } else {
                char *error = flecs_asprintf(
                    "%s\n%s", result->error, unresolved);
                ecs_os_free(result->error);
                ecs_os_free(unresolved);
                result->error = error;
            }
        }
        if (!r && result->error) {
            ecs_err("%s", result->error);
            ecs_os_free(result->error);
            result->error = NULL;
        }
    }

    ecs_script_runtime_clear(priv_desc.runtime);

    return r;
}

int ecs_script_eval(
    const ecs_script_t *script,
    const ecs_script_eval_desc_t *desc,
    ecs_script_eval_result_t *result)
{
    ecs_entity_t prev_scope = ecs_set_scope(script->world, 0);
    int r = flecs_script_eval(script, desc, 0, UINT64_MAX, result);
    ecs_set_scope(script->world, prev_scope);
    return r;
}

#endif
