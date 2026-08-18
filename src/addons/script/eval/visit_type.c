#include "flecs.h"

#ifdef FLECS_SCRIPT
#include "../script.h"

int32_t flecs_script_type_table_new(
    ecs_script_type_visitor_t *t,
    int32_t parent,
    ecs_script_entity_t *owner)
{
    ecs_script_type_table_t *table = ecs_vec_append_t(
        NULL, &t->tables, ecs_script_type_table_t);
    table->parent = parent;
    table->owner = owner;
    return ecs_vec_count(&t->tables) - 1;
}

static const char* flecs_script_type_path_sep(
    const char *name)
{
    const char *ptr = name;
    while ((ptr = strchr(ptr, '.'))) {
        if (ptr == name || ptr[-1] != '\\') {
            return ptr;
        }
        ptr ++;
    }
    return NULL;
}

static bool flecs_script_type_name_eq(
    const char *name,
    const char *value,
    ecs_size_t length)
{
    return !ecs_os_strncmp(name, value, length) && name[length] == '\0';
}

static void flecs_script_type_unresolved_ref(
    ecs_script_type_visitor_t *t,
    void *node,
    const char *name,
    flecs_script_unresolved_kind_t kind)
{
    ecs_script_impl_t *impl = t->v->base.script;
    int32_t line, column;
    flecs_script_pos_to_line_col(impl->pub.code,
        node ? ((ecs_script_node_t*)node)->pos : NULL, &line, &column);

    ecs_script_unresolved_ref_t *ref = ecs_vec_append_t(
        NULL, &impl->unresolved_refs, ecs_script_unresolved_ref_t);
    ref->name = name;
    ref->kind = kind;
    ref->line = line;
    ref->column = column;
}

static int flecs_script_type_report_unresolved(
    ecs_script_eval_visitor_t *v)
{
    ecs_script_impl_t *impl = v->base.script;
    int32_t i, count = ecs_vec_count(&impl->unresolved_refs);
    if (!count) {
        return 0;
    }

    const char *code = impl->pub.code;
    ecs_script_unresolved_ref_t *refs = ecs_vec_first(
        &impl->unresolved_refs);
    ecs_strbuf_t buf = ECS_STRBUF_INIT;
    for (i = 0; i < count; i ++) {
        ecs_script_unresolved_ref_t *ref = &refs[i];
        const char *line_start = NULL;
        if (code && ref->line) {
            line_start = code;
            int32_t l;
            for (l = 1; l < ref->line && line_start; l ++) {
                line_start = strchr(line_start, '\n');
                if (line_start) {
                    line_start ++;
                }
            }
        }
        if (i) {
            ecs_strbuf_appendch(&buf, '\n');
        }
        if (line_start) {
            int32_t line_len = 0;
            while (line_start[line_len] && line_start[line_len] != '\n') {
                line_len ++;
            }
            int32_t col = ref->column - 1;
            if (col > line_len) {
                col = line_len;
            }
            ecs_parser_error(impl->pub.name, code,
                (line_start - code) + col,
                "unresolved reference '%s'", ref->name);
            ecs_strbuf_append(&buf, "%d: unresolved reference '%s'\n",
                ref->line, ref->name);
            ecs_strbuf_appendstrn(&buf, line_start, line_len);
            ecs_strbuf_appendch(&buf, '\n');
            int32_t c;
            for (c = 0; c < col; c ++) {
                ecs_strbuf_appendch(&buf, ' ');
            }
            ecs_strbuf_appendch(&buf, '^');
        } else {
            ecs_parser_error(impl->pub.name, NULL, 0,
                "unresolved reference '%s' (line %d, column %d)",
                ref->name, ref->line, ref->column);
            ecs_strbuf_append(&buf,
                "unresolved reference '%s' (line %d, column %d)",
                ref->name, ref->line, ref->column);
        }
    }

    ecs_os_free(v->r->unresolved_errors);
    v->r->unresolved_errors = ecs_strbuf_get(&buf);
    return -1;
}

static ecs_script_type_entity_t* flecs_script_type_find_in_table(
    ecs_script_type_visitor_t *t,
    int32_t table,
    const char *name,
    ecs_size_t length)
{
    int32_t i, count = ecs_vec_count(&t->entities);
    ecs_script_type_entity_t *entities = ecs_vec_first(&t->entities);
    for (i = count - 1; i >= 0; i --) {
        ecs_script_type_entity_t *entity = &entities[i];
        if (entity->table == table &&
            flecs_script_type_name_eq(entity->name, name, length))
        {
            return entity;
        }
    }
    return NULL;
}

static ecs_script_type_entity_t* flecs_script_type_find_in_parent(
    ecs_script_type_visitor_t *t,
    ecs_entity_t parent,
    const char *name,
    ecs_size_t length)
{
    int32_t i, count = ecs_vec_count(&t->entities);
    ecs_script_type_entity_t *entities = ecs_vec_first(&t->entities);
    for (i = count - 1; i >= 0; i --) {
        ecs_script_type_entity_t *entity = &entities[i];
        if (!entity->parent_node && entity->parent == parent &&
            flecs_script_type_name_eq(entity->name, name, length))
        {
            return entity;
        }
    }
    return NULL;
}

static ecs_script_type_entity_t* flecs_script_type_find(
    ecs_script_type_visitor_t *t,
    const char *name)
{
    const char *sep = flecs_script_type_path_sep(name);
    ecs_size_t length = sep
        ? flecs_ito(ecs_size_t, sep - name)
        : ecs_os_strlen(name);
    int32_t table = t->table;
    ecs_script_type_entity_t *entity = NULL;
    while (table != -1) {
        entity = flecs_script_type_find_in_table(t, table, name, length);
        if (entity) {
            break;
        }
        table = ecs_vec_get_t(
            &t->tables, ecs_script_type_table_t, table)->parent;
    }

    if (!entity) {
        return NULL;
    }

    while (sep) {
        name = sep + 1;
        sep = flecs_script_type_path_sep(name);
        length = sep
            ? flecs_ito(ecs_size_t, sep - name)
            : ecs_os_strlen(name);
        ecs_script_type_entity_t *next = NULL;
        if (entity->child_table != -1) {
            next = flecs_script_type_find_in_table(
                t, entity->child_table, name, length);
        }
        if (!next) {
            ecs_entity_t parent = flecs_script_symbol_entity(
                t->v, entity->slot);
            if (parent) {
                next = flecs_script_type_find_in_parent(
                    t, parent, name, length);
            }
        }
        if (!next) {
            return NULL;
        }
        entity = next;
    }
    return entity;
}

int32_t flecs_script_type_slot_new(
    ecs_script_type_visitor_t *t,
    int32_t expected)
{
    ecs_script_impl_t *impl = t->v->base.script;
    if (expected != -1) {
        ecs_assert(expected < ecs_vec_count(&impl->symbol_slots),
            ECS_INTERNAL_ERROR, NULL);
        return expected;
    }
    int32_t result = ecs_vec_count(&impl->symbol_slots);
    ecs_script_symbol_slot_t *slot = ecs_vec_append_t(
        NULL, &impl->symbol_slots, ecs_script_symbol_slot_t);
    slot->entity = 0;
    slot->scope_slot = -1;
    return result;
}

static ecs_script_type_entity_t* flecs_script_type_declare(
    ecs_script_type_visitor_t *t,
    const char *name,
    ecs_script_entity_t *node,
    int32_t *slot,
    bool has_scope)
{
    ecs_size_t length = ecs_os_strlen(name);
    ecs_script_type_entity_t *entity = flecs_script_type_find_in_table(
        t, t->table, name, length);
    if (entity) {
        *slot = entity->slot;
        if (node && !entity->node) {
            entity->node = node;
        }
        if (has_scope && entity->child_table == -1) {
            entity->child_table = flecs_script_type_table_new(
                t, t->table, node);
        } else if (node && entity->child_table != -1) {
            ecs_script_type_table_t *child_table = ecs_vec_get_t(
                &t->tables, ecs_script_type_table_t, entity->child_table);
            if (!child_table->owner) {
                child_table->owner = node;
            }
        }
        return entity;
    }

    ecs_script_type_table_t *table = ecs_vec_get_t(
        &t->tables, ecs_script_type_table_t, t->table);
    entity = ecs_vec_append_t(
        NULL, &t->entities, ecs_script_type_entity_t);
    entity->name = name;
    entity->node = node;
    entity->parent_node = table->owner;
    entity->parent = t->v->parent;
    entity->table = t->table;
    entity->child_table = -1;
    entity->slot = flecs_script_type_slot_new(t, *slot);
    *slot = entity->slot;
    if (has_scope) {
        entity->child_table = flecs_script_type_table_new(
            t, t->table, node);
    }
    return entity;
}

int flecs_script_type_symbol_lookup(
    void *ctx,
    const char *name,
    flecs_script_symbol_t *symbol)
{
    ecs_script_type_visitor_t *t = ctx;
    ecs_script_type_entity_t *entity = flecs_script_type_find(t, name);
    if (!entity) {
        return -1;
    }

    symbol->kind = FlecsScriptSymbolEntitySlot;
    symbol->slot = entity->slot;
    symbol->sp = -1;
    symbol->entity = flecs_script_symbol_entity(t->v, entity->slot);
    if (symbol->entity && flecs_script_global_var_get(
        t->v->world, symbol->entity, NULL).ptr)
    {
        symbol->kind = FlecsScriptSymbolGlobalVariable;
    }
    return 0;
}

static ecs_script_type_entity_t* flecs_script_type_entity_from_node(
    ecs_script_type_visitor_t *t,
    ecs_script_entity_t *node)
{
    int32_t i, count = ecs_vec_count(&t->entities);
    ecs_script_type_entity_t *entities = ecs_vec_first(&t->entities);
    for (i = count - 1; i >= 0; i --) {
        if (entities[i].node == node) {
            return &entities[i];
        }
    }
    return NULL;
}

static int flecs_script_type_lookup(
    ecs_script_type_visitor_t *t,
    ecs_entity_t first,
    const char *name,
    flecs_script_lookup_kind_t lookup_kind,
    ecs_entity_t *from_out,
    flecs_script_symbol_t *symbol)
{
    ecs_expr_eval_desc_t desc = {
        .name = t->v->base.script->pub.name,
        .vars = t->v->vars,
        .runtime = t->v->r,
        .script_visitor = t->v
    };
    return flecs_script_id_elem_lookup(&t->v->base.script->pub, &desc,
        first, name, lookup_kind, from_out, symbol);
}

static int flecs_script_type_resolve_type(
    ecs_script_type_visitor_t *t,
    const char *name,
    ecs_entity_t *type)
{
    flecs_script_symbol_t symbol;
    if (flecs_script_type_lookup(
        t, 0, name, FlecsScriptLookupEntity, NULL, &symbol) || !symbol.entity)
    {
        return -1;
    }
    type[0] = symbol.entity;
    return 0;
}

static int flecs_script_type_check_expr(
    ecs_script_type_visitor_t *t,
    ecs_expr_node_t **expr_ptr,
    ecs_entity_t *type)
{
    ecs_script_eval_visitor_t *v = t->v;
    ecs_script_t *script = &v->base.script->pub;
    ecs_entity_t expected_type = type ? type[0] : 0;
    ecs_expr_eval_desc_t desc = {
        .name = script->name,
        .vars = v->vars,
        .type = type ? type[0] : 0,
        .runtime = v->r,
        .disable_dynamic_variable_binding = true,
        .script_visitor = v
    };

    bool was_typed = expr_ptr[0]->type_info != NULL;
    if (!was_typed) {
        int32_t prev_unresolved = ecs_vec_count(
            &v->base.script->unresolved_refs);
        if (flecs_expr_visit_type(script, expr_ptr, &desc)) {
            if (ecs_vec_count(&v->base.script->unresolved_refs) >
                prev_unresolved)
            {
                return 1;
            }
            return -1;
        }
    }

    if (was_typed) {
        if (expected_type && expr_ptr[0]->type != expected_type &&
            (!ecs_get(v->world, expected_type, EcsPrimitive) ||
             !ecs_get(v->world, expr_ptr[0]->type, EcsPrimitive)))
        {
            flecs_expr_visit_error(script, expr_ptr[0],
                "expression of type %s is incompatible with expected type %s",
                flecs_errstr(ecs_get_path(v->world, expr_ptr[0]->type)),
                flecs_errstr_1(ecs_get_path(v->world, expected_type)));
            return -1;
        }
    } else {
        if (flecs_expr_visit_fold(script, expr_ptr, &desc)) {
            return -1;
        }
    }

    ecs_vec_t *refs = NULL;
    ecs_vec_t *dynamic_refs = NULL;
    ecs_vec_t *function_refs = NULL;
    if (v->script_entity && !t->function_scope) {
        refs = &v->base.script->refs;
    }
    if (v->template) {
        refs = &v->template->refs;
        dynamic_refs = &v->template->dynamic_refs;
        function_refs = &v->base.script->refs;
    }
    if (refs && flecs_expr_visit_refs(
        script, *expr_ptr, refs, dynamic_refs,
        function_refs ? function_refs : refs))
    {
        return -1;
    }

    if (type) {
        type[0] = expr_ptr[0]->type;
    }
    return 0;
}

static int flecs_script_type_id_elem(
    ecs_script_type_visitor_t *t,
    ecs_script_id_t *id,
    void *node,
    const char *name,
    ecs_expr_node_t **name_expr,
    ecs_entity_t first,
    ecs_entity_t *eval,
    int32_t *slot,
    int32_t *sp)
{
    if (name_expr && *name_expr) {
        ecs_entity_t type = ecs_id(ecs_string_t);
        id->dynamic = true;
        return flecs_script_type_check_expr(t, name_expr, &type);
    }

    ecs_entity_t from;
    flecs_script_symbol_t symbol;
    if (flecs_script_type_lookup(
        t, first, name, FlecsScriptLookupAll, &from, &symbol))
    {
        if (from) {
            char *parent_str = ecs_id_str(t->v->world, from);
            flecs_script_eval_error(t->v, node,
                "target '%s' not found in parent '%s'", name, parent_str);
            ecs_os_free(parent_str);
            return -1;
        }
        flecs_script_unresolved_kind_t ref_kind = FlecsScriptUnresolvedEntity;
        if (!id->second) {
            ecs_script_node_kind_t node_kind = ((ecs_script_node_t*)node)->kind;
            if (node_kind == EcsAstComponent ||
                node_kind == EcsAstWithComponent)
            {
                if (((ecs_script_component_t*)node)->expr) {
                    ref_kind = FlecsScriptUnresolvedComponent;
                }
            }
        }
        flecs_script_type_unresolved_ref(t, node, name, ref_kind);
        return 1;
    }

    if (symbol.kind == FlecsScriptSymbolVariable) {
        ecs_script_var_t *var = ecs_script_vars_from_sp(
            t->v->vars, symbol.sp);
        if (!var || var->value.type != ecs_id(ecs_entity_t)) {
            flecs_script_eval_error(t->v, node,
                "variable '%s' must be of type entity", name);
            return -1;
        }
        *sp = symbol.sp;
        id->dynamic = true;
    } else if (symbol.kind == FlecsScriptSymbolEntitySlot) {
        *slot = symbol.slot;
        id->dynamic = true;
    } else {
        *eval = symbol.entity;
    }
    return 0;
}

static int flecs_script_type_id(
    ecs_script_type_visitor_t *t,
    void *node,
    ecs_script_id_t *id)
{
    if (!id->first || (!id->first[0] && !id->first_expr) ||
        (id->second && !id->second[0] && !id->second_expr))
    {
        flecs_script_eval_error(t->v, node, "invalid component identifier");
        return -1;
    }

    id->eval = 0;
    id->first_eval = 0;
    id->second_eval = 0;
    id->first_symbol = -1;
    id->second_symbol = -1;
    id->first_sp = -1;
    id->second_sp = -1;
    id->dynamic = id->first_expr || id->second_expr;

    int result = flecs_script_type_id_elem(t, id, node, id->first,
        &id->first_expr, 0, &id->first_eval, &id->first_symbol,
        &id->first_sp);
    if (result) {
        return result;
    }

    ecs_entity_t first;
    if (flecs_script_eval_id_elem(t->v, node, NULL,
        id->first_eval, id->first_symbol, id->first_sp, &first))
    {
        return -1;
    }
    if (id->second) {
        result = flecs_script_type_id_elem(t, id, node, id->second,
            &id->second_expr, first, &id->second_eval, &id->second_symbol,
            &id->second_sp);
        if (result) {
            return result;
        }
        ecs_entity_t second;
        if (flecs_script_eval_id_elem(t->v, node, NULL,
            id->second_eval, id->second_symbol, id->second_sp, &second))
        {
            return -1;
        }
        if (first && second) {
            id->eval = id->flag | ecs_pair(first, second);
        }
    } else if (first) {
        id->eval = id->flag | first;
    }
    return 0;
}

static int flecs_script_type_ensure_node(
    ecs_script_type_visitor_t *t,
    ecs_script_entity_t *node);

static ecs_entity_t flecs_script_type_ensure_owner(
    ecs_script_type_visitor_t *t);

static ecs_entity_t flecs_script_type_parent(
    ecs_script_type_visitor_t *t,
    ecs_script_type_entity_t *entry)
{
    if (entry->parent_node) {
        if (flecs_script_type_ensure_node(t, entry->parent_node)) {
            return 0;
        }
        if (entry->parent_node->symbol != -1) {
            return flecs_script_symbol_entity(
                t->v, entry->parent_node->symbol);
        }
        return entry->parent_node->eval;
    }
    return entry->parent;
}

static int flecs_script_type_ensure_node(
    ecs_script_type_visitor_t *t,
    ecs_script_entity_t *node)
{
    ecs_entity_t current = node->symbol != -1
        ? flecs_script_symbol_entity(t->v, node->symbol)
        : node->eval;
    if (current && ecs_is_alive(t->v->world, current)) {
        return 0;
    }

    ecs_script_type_entity_t *entry = flecs_script_type_entity_from_node(
        t, node);
    ecs_entity_t parent = entry ? flecs_script_type_parent(t, entry) : 0;
    if (entry && (entry->parent_node || entry->parent) && !parent) {
        return -1;
    }

    const char *name = node->name;
    ecs_value_t value = {0};
    if (node->name_expr) {
        value.type = ecs_id(ecs_string_t);
        if (flecs_script_eval_expr(t->v, &node->name_expr, &value)) {
            return -1;
        }
        name = *(char**)value.ptr;
    }

    ecs_entity_desc_t desc = {
        .parent = parent,
        .name = name
    };
    current = ecs_entity_init(t->v->world, &desc);
    if (value.ptr) {
        ecs_ptr_free(t->v->world, value.type, value.ptr);
    }
    if (!current) {
        return -1;
    }
    if (t->v->script_tag) {
        ecs_add_id(t->v->world, current, t->v->script_tag);
    }
    node->eval = current;
    if (node->symbol != -1) {
        flecs_script_symbol_set(t->v, node->symbol, current);
    }
    return 0;
}

static int flecs_script_type_ensure_entities(
    ecs_script_type_visitor_t *t)
{
    int32_t i, count = ecs_vec_count(&t->entities);
    for (i = 0; i < count; i ++) {
        ecs_script_type_entity_t *entries = ecs_vec_first(&t->entities);
        ecs_script_entity_t *node = entries[i].node;
        if (node && flecs_script_type_ensure_node(t, node)) {
            return -1;
        }
    }
    return 0;
}

static int flecs_script_type_tag(
    ecs_script_type_visitor_t *t,
    ecs_script_tag_t *node)
{
    int result = flecs_script_type_id(t, node, &node->id);
    if (result) {
        return result == 1 ? 0 : -1;
    }
    if (t->v->is_with_scope) {
        flecs_script_eval_error(t->v, node, "invalid tag in with scope");
        return -1;
    }
    if (!t->v->entity) {
        flecs_script_eval_error(t->v, node, "missing entity for tag %s",
            node->id.first);
        return -1;
    }
    if (node->id.eval) {
        ecs_entity_t type = ecs_get_typeid(t->v->world, node->id.eval);
        if (type) {
            const ecs_type_info_t *ti = ecs_get_type_info(
                t->v->world, type);
            if (ti && (ti->hooks.flags & ECS_TYPE_HOOK_CTOR_ILLEGAL)) {
                flecs_script_eval_error(t->v, node,
                    "cannot add %s, type is not default constructible",
                    node->id.first);
                return -1;
            }
        }
    }
    return 0;
}

static ecs_entity_t flecs_script_type_component_type(
    ecs_script_type_visitor_t *t,
    ecs_script_component_t *node)
{
    if (node->id.eval) {
        return ecs_get_typeid(t->v->world, node->id.eval);
    }

    ecs_entity_t candidates[2] = {0, 0};
    if (flecs_script_eval_id_elem(t->v, node, NULL,
        node->id.first_eval, node->id.first_symbol,
        node->id.first_sp, &candidates[0]) ||
        flecs_script_eval_id_elem(t->v, node, NULL,
            node->id.second_eval, node->id.second_symbol,
            node->id.second_sp, &candidates[1]))
    {
        return 0;
    }
    if (candidates[0] && node->id.second &&
        ecs_has_id(t->v->world, candidates[0], EcsPairIsTag))
    {
        return 0;
    }
    for (int32_t i = 0; i < 2; i ++) {
        if (!candidates[i]) {
            continue;
        }
        const ecs_type_info_t *ti = ecs_get_type_info(
            t->v->world, candidates[i]);
        if (ti) {
            return ti->component;
        }
    }
    return 0;
}

static int flecs_script_type_struct_expr(
    ecs_script_type_visitor_t *t,
    ecs_expr_node_t *expr)
{
    if (!expr || (expr->kind != EcsExprInitializer &&
        expr->kind != EcsExprEmptyInitializer))
    {
        return 0;
    }

    ecs_expr_initializer_t *initializer = (ecs_expr_initializer_t*)expr;
    int32_t i, count = ecs_vec_count(&initializer->elements);
    ecs_expr_initializer_element_t *elems = ecs_vec_first(
        &initializer->elements);
    for (i = 0; i < count; i ++) {
        ecs_entity_t type = elems[i].value &&
            (elems[i].value->kind == EcsExprInitializer ||
             elems[i].value->kind == EcsExprEmptyInitializer)
                ? ecs_id(EcsMember)
                : ecs_id(ecs_entity_t);
        if (elems[i].value) {
            int result = flecs_script_type_check_expr(
                t, &elems[i].value, &type);
            if (result) {
                return result;
            }
        }
    }
    return 0;
}

static int flecs_script_type_constants_expr(
    ecs_script_type_visitor_t *t,
    ecs_expr_node_t *expr,
    bool is_bitmask)
{
    if (!expr || (expr->kind != EcsExprInitializer &&
        expr->kind != EcsExprEmptyInitializer))
    {
        return 0;
    }

    ecs_expr_initializer_t *initializer = (ecs_expr_initializer_t*)expr;
    int32_t i, count = ecs_vec_count(&initializer->elements);
    ecs_expr_initializer_element_t *elems = ecs_vec_first(
        &initializer->elements);
    ecs_entity_t underlying = is_bitmask ? ecs_id(ecs_u32_t) : 0;

    for (i = 0; i < count; i ++) {
        ecs_expr_initializer_element_t *elem = &elems[i];
        if (elem->member || !elem->value ||
            (elem->value->kind != EcsExprInitializer &&
             elem->value->kind != EcsExprEmptyInitializer))
        {
            continue;
        }

        ecs_expr_initializer_t *cfg = (ecs_expr_initializer_t*)elem->value;
        int32_t c, cfg_count = ecs_vec_count(&cfg->elements);
        ecs_expr_initializer_element_t *cfg_elems = ecs_vec_first(
            &cfg->elements);
        for (c = 0; c < cfg_count; c ++) {
            ecs_entity_t type = ecs_id(ecs_entity_t);
            if (cfg_elems[c].value) {
                int result = flecs_script_type_check_expr(
                    t, &cfg_elems[c].value, &type);
                if (result) {
                    return result;
                }
            }
            if (!is_bitmask && cfg_elems[c].member &&
                !ecs_os_strcmp(cfg_elems[c].member, "underlying_type"))
            {
                ecs_value_t value = {
                    .type = ecs_id(ecs_entity_t)
                };
                if (flecs_script_eval_expr(
                    t->v, &cfg_elems[c].value, &value))
                {
                    return -1;
                }
                underlying = *(ecs_entity_t*)value.ptr;
                ecs_ptr_free(t->v->world, value.type, value.ptr);
            }
        }
    }

    if (!underlying) {
        underlying = ecs_id(ecs_i32_t);
    }
    for (i = 0; i < count; i ++) {
        ecs_expr_initializer_element_t *elem = &elems[i];
        if (elem->member && elem->value) {
            int result = flecs_script_type_check_expr(
                t, &elem->value, &underlying);
            if (result) {
                return result;
            }
        }
    }
    return 0;
}

static int flecs_script_type_visitor_expr(
    ecs_script_type_visitor_t *t,
    ecs_script_component_t *node)
{
    if (node->id.eval == ecs_id(EcsStruct)) {
        return flecs_script_type_struct_expr(t, node->expr);
    }
    if (node->id.eval == ecs_id(EcsEnum)) {
        return flecs_script_type_constants_expr(t, node->expr, false);
    }
    if (node->id.eval == ecs_id(EcsBitmask)) {
        return flecs_script_type_constants_expr(t, node->expr, true);
    }
    return 0;
}

static int flecs_script_type_with_tag(
    ecs_script_type_visitor_t *t,
    ecs_script_tag_t *node)
{
    int result = flecs_script_type_id(t, node, &node->id);
    return result == 1 ? 0 : result;
}

static int flecs_script_type_with_component(
    ecs_script_type_visitor_t *t,
    ecs_script_component_t *node)
{
    int result = flecs_script_type_id(t, node, &node->id);
    if (result) {
        return result == 1 ? 0 : -1;
    }
    if (node->expr) {
        ecs_entity_t type = flecs_script_type_component_type(t, node);
        if (!type) {
            flecs_script_eval_error(t->v, node,
                "unresolved component type '%s'", node->id.first);
            return -1;
        }
        result = flecs_script_type_check_expr(t, &node->expr, &type);
        return result == 1 ? 0 : result;
    }
    return 0;
}

static int flecs_script_type_component(
    ecs_script_type_visitor_t *t,
    ecs_script_component_t *node)
{
    int id_result = flecs_script_type_id(t, node, &node->id);
    if (id_result) {
        return id_result == 1 ? 0 : -1;
    }
    if (!t->v->entity) {
        flecs_script_eval_error(t->v, node,
            "missing entity for component %s", node->id.first);
        return -1;
    }
    if (t->v->is_with_scope) {
        flecs_script_eval_error(t->v, node,
            "invalid component in with scope");
        return -1;
    }

    if (!node->expr) {
        return 0;
    }
    if (!node->id.second && node->id.eval && ecs_has(
        t->v->world, node->id.eval, EcsScriptVisitor))
    {
        int result = flecs_script_type_visitor_expr(t, node);
        return result == 1 ? 0 : result;
    }

    ecs_entity_t component_type = flecs_script_type_component_type(t, node);
    if (!component_type) {
        flecs_script_eval_error(t->v, node,
            "unresolved component type '%s'", node->id.first);
        return -1;
    }
    const ecs_type_info_t *ti = ecs_get_type_info(
        t->v->world, component_type);
    if (!ti) {
        flecs_script_eval_error(t->v, node,
            "cannot set value of '%s': not a component", node->id.first);
        return -1;
    }

    ecs_entity_t expr_type = component_type;
    int result = flecs_script_type_check_expr(t, &node->expr, &expr_type);
    return result == 1 ? 0 : result;
}

static int flecs_script_type_with(
    ecs_script_type_visitor_t *t,
    ecs_script_with_t *node)
{
    if (flecs_script_type_scope(
        t, node->expressions, t->table, false, false))
    {
        return -1;
    }

    bool old_with = t->v->is_with_scope;
    t->v->is_with_scope = true;
    int32_t table = flecs_script_type_table_new(t, t->table,
        ecs_vec_get_t(&t->tables, ecs_script_type_table_t,
            t->table)->owner);
    t->control_depth ++;
    int result = flecs_script_type_scope(
        t, node->scope, table, true, false);
    t->control_depth --;
    t->v->is_with_scope = old_with;
    return result;
}

static int flecs_script_type_annot(
    ecs_script_type_visitor_t *t,
    ecs_script_annot_t *node)
{
    ecs_script_eval_visitor_t *v = t->v;
    if (!v->base.next) {
        flecs_script_eval_error(v, node,
            "annotation '%s' is not applied to anything", node->name);
        return -1;
    }
    ecs_script_node_kind_t kind = v->base.next->kind;
    if (kind != EcsAstEntity && kind != EcsAstTemplate &&
        kind != EcsAstAnnotation)
    {
        flecs_script_eval_error(v, node,
            "annotation must be applied to an entity or template");
        return -1;
    }
    return flecs_script_eval_node(&v->base, (ecs_script_node_t*)node);
}

static int flecs_script_type_const(
    ecs_script_type_visitor_t *t,
    ecs_script_var_node_t *node)
{
    bool export = node->node.kind == EcsAstExportConst ||
        node->node.kind == EcsAstExportMut;
    if (!node->expr) {
        flecs_script_eval_error(t->v, node,
            "const variable '%s' is missing initializer", node->name);
        return -1;
    }

    ecs_entity_t expected_type = 0;
    if (node->type && flecs_script_type_resolve_type(
        t, node->type, &expected_type))
    {
        flecs_script_type_unresolved_ref(t, node, node->type,
            FlecsScriptUnresolvedComponent);
        if (node->expr->kind == EcsExprInitializer ||
            node->expr->kind == EcsExprEmptyInitializer)
        {
            return -1;
        }
    }
    ecs_entity_t type = expected_type;
    int expr_result = flecs_script_type_check_expr(t, &node->expr, &type);
    if (expr_result) {
        return expr_result == 1 ? 0 : -1;
    }
    node->eval_type = type;

    if (node->is_await && export) {
        flecs_script_eval_error(t->v, node,
            "await is not allowed for exported variables");
        return -1;
    }

    if (export) {
        if (ecs_script_vars_lookup(t->v->vars, node->name)) {
            flecs_script_eval_error(t->v, node,
                "exported variable '%s' shadows a local variable", node->name);
            return -1;
        }
        flecs_script_type_declare(
            t, node->name, NULL, &node->symbol, false);
        if (flecs_script_type_ensure_entities(t)) {
            return -1;
        }
        ecs_entity_t old_parent = t->v->parent;
        ecs_entity_t parent = flecs_script_type_ensure_owner(t);
        if (!parent && ecs_vec_get_t(
            &t->tables, ecs_script_type_table_t, t->table)->owner)
        {
            return -1;
        }
        t->v->parent = parent;
        int result = flecs_script_eval_const(t->v, node, true);
        t->v->parent = old_parent;
        if (result || !flecs_script_symbol_entity(t->v, node->symbol)) {
            return -1;
        }
        return 0;
    }

    {
        ecs_script_type_table_t *table = ecs_vec_get_t(
            &t->tables, ecs_script_type_table_t, t->table);
        ecs_entity_t parent = t->v->parent;
        bool parent_known = true;
        if (table->owner) {
            parent = table->owner->symbol != -1
                ? flecs_script_symbol_entity(t->v, table->owner->symbol)
                : table->owner->eval;
            parent_known = parent != 0;
        }
        if (parent_known) {
            ecs_entity_t existing = ecs_lookup_child(
                t->v->world, parent, node->name);
            if (existing && flecs_script_global_var_get(
                t->v->world, existing, NULL).ptr)
            {
                flecs_script_eval_error(t->v, node,
                    "local variable '%s' shadows an exported variable",
                    node->name);
                return -1;
            }
        }
    }

    ecs_script_var_t *var = ecs_script_vars_declare(
        t->v->vars, node->name);
    if (!var) {
        flecs_script_eval_error(t->v, node,
            "variable '%s' redeclared", node->name);
        return -1;
    }
    var->value.type = type;
    var->type_info = ecs_get_type_info(t->v->world, type);
    var->is_const = true;
    node->sp = var->sp;

    if (node->is_await) {
        return 0;
    }

    if (node->expr->kind == EcsExprValue) {
        ecs_expr_value_node_t *value = (ecs_expr_value_node_t*)node->expr;
        const ecs_type_info_t *ti = var->type_info;
        var->value.ptr = flecs_stack_calloc(
            &t->v->r->stack, ti->size, ti->alignment);
        flecs_type_info_ctor(var->value.ptr, 1, ti);
        ecs_ptr_copy_w_type_info(
            t->v->world, ti, var->value.ptr, value->ptr);
        var->is_const = true;
    }
    return 0;
}

static int flecs_script_type_template_var(
    ecs_script_type_visitor_t *t,
    ecs_script_var_node_t *node,
    bool mut)
{
    if (!node->expr) {
        flecs_script_eval_error(t->v, node,
            "%s variable '%s' is missing initializer",
            mut ? "mut" : "prop", node->name);
        return -1;
    }

    ecs_entity_t type = 0;
    if (node->type && flecs_script_type_resolve_type(
        t, node->type, &type))
    {
        flecs_script_type_unresolved_ref(t, node, node->type,
            FlecsScriptUnresolvedComponent);
        if (node->expr->kind == EcsExprInitializer ||
            node->expr->kind == EcsExprEmptyInitializer)
        {
            return -1;
        }
    }
    int result = flecs_script_type_check_expr(t, &node->expr, &type);
    if (result) {
        return result == 1 ? 0 : -1;
    }
    node->eval_type = type;
    return flecs_script_template_eval_var(t->v, node, mut);
}

static int flecs_script_type_for(
    ecs_script_type_visitor_t *t,
    ecs_script_for_t *node)
{
    flecs_script_for_kind_t kind = FlecsScriptForRange;
    ecs_entity_t key_type = 0;
    ecs_entity_t elem_type = ecs_id(ecs_i32_t);
    int expr_result;
    if (!node->expr) {
        ecs_entity_t type = ecs_id(ecs_i32_t);
        expr_result = flecs_script_type_check_expr(t, &node->from, &type);
        if (expr_result) {
            return expr_result == 1 ? 0 : -1;
        }
        type = ecs_id(ecs_i32_t);
        expr_result = flecs_script_type_check_expr(t, &node->to, &type);
        if (expr_result) {
            return expr_result == 1 ? 0 : -1;
        }
    } else {
        ecs_entity_t type = 0;
        expr_result = flecs_script_type_check_expr(t, &node->expr, &type);
        if (expr_result) {
            return expr_result == 1 ? 0 : -1;
        }
        if (flecs_script_for_collection_kind(
            t->v, node, type, &kind, &key_type, &elem_type))
        {
            return -1;
        }
    }
    if (flecs_script_for_check_var_count(t->v, node, kind)) {
        return -1;
    }

    t->v->vars = flecs_script_vars_push(t->v->vars,
        &t->v->r->stack, &t->v->r->allocator);
    int32_t var_i = 0;
    if (kind == FlecsScriptForMap && node->loop_var_count >= 2) {
        ecs_script_var_t *var = flecs_script_for_declare_var(
            t->v, node, node->loop_vars[var_i], key_type, true);
        if (!var) {
            goto error;
        }
        node->loop_var_sp[var_i ++] = var->sp;
    }
    if (flecs_script_for_has_index_var(kind, node->loop_var_count)) {
        ecs_script_var_t *var = flecs_script_for_declare_var(
            t->v, node, node->loop_vars[var_i], ecs_id(ecs_i32_t), true);
        if (!var) {
            goto error;
        }
        node->loop_var_sp[var_i ++] = var->sp;
    }
    ecs_script_var_t *var = flecs_script_for_declare_var(
        t->v, node, node->loop_vars[var_i], elem_type, true);
    if (!var) {
        goto error;
    }
    node->loop_var_sp[var_i] = var->sp;

    {
        int32_t table = flecs_script_type_table_new(t, t->table,
            ecs_vec_get_t(&t->tables, ecs_script_type_table_t,
                t->table)->owner);
        t->control_depth ++;
        t->for_depth ++;
        int result = flecs_script_type_scope(
            t, node->scope, table, true, false);
        t->for_depth --;
        t->control_depth --;
        t->v->vars = ecs_script_vars_pop(t->v->vars);
        return result;
    }
error:
    t->v->vars = ecs_script_vars_pop(t->v->vars);
    return -1;
}

static ecs_entity_t flecs_script_type_ensure_owner(
    ecs_script_type_visitor_t *t)
{
    ecs_script_type_table_t *table = ecs_vec_get_t(
        &t->tables, ecs_script_type_table_t, t->table);
    if (!table->owner) {
        return t->v->parent;
    }
    if (flecs_script_type_ensure_node(t, table->owner)) {
        return 0;
    }
    if (table->owner->symbol != -1) {
        return flecs_script_symbol_entity(t->v, table->owner->symbol);
    }
    return table->owner->eval;
}

static int flecs_script_type_pair_scope(
    ecs_script_type_visitor_t *t,
    ecs_script_pair_scope_t *node)
{
    const char *names[2] = { node->id.first, node->id.second };
    ecs_expr_node_t *exprs[2] = {
        node->id.first_expr, node->id.second_expr
    };
    int32_t *slots[2] = {
        &node->id.first_symbol, &node->id.second_symbol
    };
    for (int32_t i = 0; i < 2; i ++) {
        const char *name = names[i];
        if (!name || exprs[i] || name[0] == '$' ||
            !ecs_os_strcmp(name, "this"))
        {
            continue;
        }
        if (i == 1) {
            flecs_script_type_declare(
                t, name, NULL, slots[i], true);
            continue;
        }
        flecs_script_symbol_t symbol;
        if (flecs_script_type_lookup(
            t, 0, name, FlecsScriptLookupEntity, NULL, &symbol))
        {
            flecs_script_type_declare(
                t, name, NULL, slots[i], true);
        }
    }

    if (flecs_script_type_id(t, node, &node->id) == -1) {
        return -1;
    }

    int32_t table = flecs_script_type_table_new(t, t->table,
        ecs_vec_get_t(&t->tables, ecs_script_type_table_t,
            t->table)->owner);
    t->control_depth ++;
    int result = flecs_script_type_scope(
        t, node->scope, table, true, false);
    t->control_depth --;
    return result;
}

static int flecs_script_type_if(
    ecs_script_type_visitor_t *t,
    ecs_script_if_t *node)
{
    if (flecs_script_type_check_expr(t, &node->expr, NULL) == -1) {
        return -1;
    }
    ecs_script_entity_t *owner = ecs_vec_get_t(
        &t->tables, ecs_script_type_table_t, t->table)->owner;
    int32_t true_table = flecs_script_type_table_new(
        t, t->table, owner);
    int32_t false_table = flecs_script_type_table_new(
        t, t->table, owner);
    t->control_depth ++;
    int result = flecs_script_type_scope(
        t, node->if_true, true_table, true, false);
    if (!result) {
        result = flecs_script_type_scope(
            t, node->if_false, false_table, true, false);
    }
    t->control_depth --;
    return result;
}

static int flecs_script_type_try(
    ecs_script_type_visitor_t *t,
    ecs_script_try_t *node)
{
    ecs_script_entity_t *owner = ecs_vec_get_t(
        &t->tables, ecs_script_type_table_t, t->table)->owner;
    int32_t try_table = flecs_script_type_table_new(
        t, t->table, owner);
    t->control_depth ++;
    int result = flecs_script_type_scope(
        t, node->try_scope, try_table, true, false);
    if (result) {
        t->control_depth --;
        return -1;
    }

    int32_t i, count = ecs_vec_count(&node->catches);
    ecs_script_catch_t *catches = ecs_vec_first(&node->catches);
    for (i = 0; i < count; i ++) {
        ecs_script_catch_t *catch_ = &catches[i];
        catch_->eval_error = 0;
        catch_->error_symbol = -1;
        if (catch_->error) {
            flecs_script_symbol_t symbol;
            if (flecs_script_type_lookup(
                t, 0, catch_->error, FlecsScriptLookupEntity, NULL, &symbol))
            {
                flecs_script_type_unresolved_ref(t, node, catch_->error,
                    FlecsScriptUnresolvedEntity);
            } else if (symbol.kind == FlecsScriptSymbolEntitySlot) {
                catch_->error_symbol = symbol.slot;
            } else {
                catch_->eval_error = symbol.entity;
            }
        }
        int32_t catch_table = flecs_script_type_table_new(
            t, t->table, owner);
        if (flecs_script_type_scope(
            t, catch_->scope, catch_table, true, false))
        {
            t->control_depth --;
            return -1;
        }
    }
    t->control_depth --;
    return 0;
}

static int flecs_script_type_function(
    ecs_script_type_visitor_t *t,
    ecs_script_function_node_t *node)
{
    ecs_entity_t fn_entity = 0;
    if (t->template_scope) {
        flecs_script_eval_error(t->v, node,
            "fn is not allowed in template");
        return -1;
    }

    flecs_script_type_declare(
        t, node->name, NULL, &node->symbol, false);
    if (flecs_script_type_resolve_type(
        t, node->return_type, &node->eval_return_type))
    {
        flecs_script_type_unresolved_ref(
            t, &node->return_type_node, node->return_type,
            FlecsScriptUnresolvedComponent);
        return 0;
    }

    ecs_entity_t parent = flecs_script_type_ensure_owner(t);
    if (!parent && ecs_vec_get_t(
        &t->tables, ecs_script_type_table_t, t->table)->owner)
    {
        return -1;
    }

    ecs_script_vars_t *outer_vars = t->v->vars;
    t->v->vars = flecs_script_vars_push(
        NULL, &t->v->r->stack, &t->v->r->allocator);
    int32_t i, param_count = ecs_vec_count(&node->params);
    ecs_script_fn_param_t *params = ecs_vec_first(&node->params);
    for (i = 0; i < param_count; i ++) {
        if (flecs_script_type_resolve_type(
            t, params[i].type, &params[i].eval_type))
        {
            flecs_script_type_unresolved_ref(
                t, &params[i].node, params[i].type,
                FlecsScriptUnresolvedComponent);
            t->v->vars = ecs_script_vars_pop(t->v->vars);
            t->v->vars = outer_vars;
            return 0;
        }
        ecs_script_var_t *var = ecs_script_vars_declare(
            t->v->vars, params[i].name);
        if (!var) {
            flecs_script_eval_error(t->v, &params[i].node,
                "duplicate parameter '%s' in function '%s'",
                params[i].name, node->name);
            goto error;
        }
        params[i].sp = var->sp;
        var->value.type = params[i].eval_type;
        var->type_info = ecs_get_type_info(
            t->v->world, params[i].eval_type);
    }

    ecs_function_desc_t fn_desc = {
        .name = node->name,
        .parent = parent,
        .return_type = node->eval_return_type,
        .callback = flecs_script_user_function_callback
    };
    for (i = 0; i < param_count; i ++) {
        fn_desc.params[i].name = params[i].name;
        fn_desc.params[i].type = params[i].eval_type;
    }
    fn_entity = ecs_function_init(t->v->world, &fn_desc);
    if (!fn_entity) {
        flecs_script_eval_error(t->v, node,
            "failed to register function '%s'", node->name);
        goto error;
    }
    flecs_script_symbol_set(t->v, node->symbol, fn_entity);
    if (flecs_script_type_ensure_entities(t)) {
        goto error;
    }

    int32_t table = flecs_script_type_table_new(t, t->table,
        ecs_vec_get_t(&t->tables, ecs_script_type_table_t,
            t->table)->owner);
    bool old_function_scope = t->function_scope;
    t->function_scope = true;
    int result = flecs_script_type_scope(
        t, node->body, table, false, false);
    if (result) {
        t->function_scope = old_function_scope;
        goto error;
    }
    if (node->return_expr) {
        ecs_entity_t return_type = node->eval_return_type;
        int expr_result = flecs_script_type_check_expr(
            t, &node->return_expr, &return_type);
        if (expr_result) {
            t->function_scope = old_function_scope;
            if (expr_result == 1) {
                t->v->vars = ecs_script_vars_pop(t->v->vars);
                t->v->vars = outer_vars;
                return 0;
            }
            goto error;
        }
    }
    t->function_scope = old_function_scope;

    t->v->vars = ecs_script_vars_pop(t->v->vars);
    t->v->vars = outer_vars;
    ecs_entity_t old_parent = t->v->parent;
    t->v->parent = parent;
    result = flecs_script_eval_function(t->v, node);
    t->v->parent = old_parent;
    if (result) {
        ecs_delete(t->v->world, fn_entity);
        flecs_script_symbol_set(t->v, node->symbol, 0);
    }
    return result;
error:
    t->v->vars = ecs_script_vars_pop(t->v->vars);
    t->v->vars = outer_vars;
    if (fn_entity) {
        ecs_delete(t->v->world, fn_entity);
        flecs_script_symbol_set(t->v, node->symbol, 0);
    }
    return -1;
}

static int flecs_script_type_entity(
    ecs_script_type_visitor_t *t,
    ecs_script_entity_t *node,
    bool allow_type)
{
    node->eval = 0;
    node->eval_kind = 0;
    node->kind_symbol = -1;
    node->kind_sp = -1;
    ecs_script_type_entity_t *entry = NULL;
    int32_t child_table;
    if (node->name && !node->name_expr) {
        entry = flecs_script_type_declare(
            t, node->name, node, &node->symbol, true);
        child_table = entry->child_table;
    } else {
        if (node->symbol == -1 && !t->for_depth && !t->function_scope) {
            node->symbol = flecs_script_type_slot_new(t, -1);
        }
        child_table = flecs_script_type_table_new(t, t->table, node);
    }

    if (node->name_expr) {
        ecs_entity_t type = ecs_id(ecs_string_t);
        int result = flecs_script_type_check_expr(t, &node->name_expr, &type);
        if (result) {
            return result == 1 ? 0 : -1;
        }
    }

    if (node->kind) {
        if (!node->kind[0]) {
            flecs_script_eval_error(t->v, node,
                "invalid empty entity kind");
            return -1;
        }
        if (!ecs_os_strcmp(node->kind, "prefab")) {
#ifdef FLECS_PREFAB
            node->eval_kind = EcsPrefab;
#else
            flecs_script_eval_error(t->v, node,
                "prefabs are unsupported in this flecs build, enable FLECS_PREFAB addon");
            return -1;
#endif
        } else {
            flecs_script_symbol_t symbol = {0};
            if (flecs_script_type_lookup(
                t, 0, node->kind, FlecsScriptLookupAll, NULL, &symbol))
            {
                flecs_script_type_unresolved_ref(t, node, node->kind,
                    FlecsScriptUnresolvedEntity);
            } else if (symbol.kind == FlecsScriptSymbolVariable) {
                ecs_script_var_t *var = ecs_script_vars_from_sp(
                    t->v->vars, symbol.sp);
                if (!var || var->value.type != ecs_id(ecs_entity_t)) {
                    flecs_script_eval_error(t->v, node,
                        "variable '%s' must be of type entity", node->kind);
                    return -1;
                }
                node->kind_sp = symbol.sp;
            } else if (symbol.kind == FlecsScriptSymbolEntitySlot) {
                node->kind_symbol = symbol.slot;
                node->eval_kind = symbol.entity;
            } else {
                node->eval_kind = symbol.entity;
            }
        }
    }

    node->is_type = node->eval_kind && ecs_has(
        t->v->world, node->eval_kind, EcsScriptVisitor);
    if (node->is_type) {
        const EcsScript *script = ecs_get(
            t->v->world, node->eval_kind, EcsScript);
        if (script && script->template_) {
            node->is_type = false;
        }
    }
    if (node->is_type && (!allow_type || t->control_depth ||
        t->template_scope || t->function_scope))
    {
        flecs_script_eval_error(t->v, node,
            "type definitions are only allowed in entity scopes");
        return -1;
    }

    flecs_script_entity_state_t state = {
        .node = node,
        .eval_kind = node->eval_kind
    };
    flecs_script_entity_state_t *old_entity = t->v->entity;
    bool old_with = t->v->is_with_scope;
    t->v->entity = &state;
    t->v->is_with_scope = false;

    if (!node->is_type) {
        ecs_vec_clear(&t->v->r->annot);
    }
    int result = flecs_script_type_scope(
        t, node->scope, child_table, true, true);
    t->v->entity = old_entity;
    t->v->is_with_scope = old_with;
    if (result) {
        return -1;
    }

    if (node->is_type) {
        if (flecs_script_type_ensure_node(t, node)) {
            return -1;
        }
        if (entry) {
            entry = flecs_script_type_entity_from_node(t, node);
        }
        ecs_entity_t old_parent = t->v->parent;
        ecs_entity_t parent = entry
            ? flecs_script_type_parent(t, entry)
            : old_parent;
        t->v->parent = parent;
        ecs_entity_t out = 0;
        result = flecs_script_eval_entity(t->v,
            &t->v->base.script->pub, t->v->vars, node, &out);
        t->v->parent = old_parent;
        if (result) {
            return -1;
        }
        if (node->symbol != -1) {
            flecs_script_symbol_set(t->v, node->symbol, out);
        }
    }
    return 0;
}

static int flecs_script_type_template(
    ecs_script_type_visitor_t *t,
    ecs_script_template_node_t *node)
{
    if (t->template_scope) {
        flecs_script_eval_error(t->v, node,
            "nested templates are not allowed");
        return -1;
    }
    flecs_script_type_declare(
        t, node->name, NULL, &node->symbol, true);
    if (flecs_script_type_ensure_entities(t)) {
        return -1;
    }
    ecs_entity_t old_parent = t->v->parent;
    ecs_entity_t parent = flecs_script_type_ensure_owner(t);
    if (!parent && ecs_vec_get_t(
        &t->tables, ecs_script_type_table_t, t->table)->owner)
    {
        return -1;
    }
    t->v->parent = parent;
    int result = flecs_script_eval_template(t->v, node);
    t->v->parent = old_parent;
    return result;
}

static int flecs_script_type_module(
    ecs_script_type_visitor_t *t,
    ecs_script_module_t *node)
{
    flecs_script_type_declare(
        t, node->name, NULL, &node->symbol, false);
    ecs_entity_t old_parent = t->v->parent;
    ecs_entity_t old_scope = ecs_set_scope(t->v->world, 0);
    t->v->parent = 0;
    node->eval = flecs_script_create_entity(t->v, node->name);
    ecs_set_scope(t->v->world, old_scope);
    if (!node->eval) {
        t->v->parent = old_parent;
        return -1;
    }
    ecs_add_id(t->v->world, node->eval, EcsModule);
    t->v->module = node->eval;
    t->v->parent = node->eval;
    flecs_script_symbol_set(t->v, node->symbol, node->eval);
    return 0;
}

static int flecs_script_type_using(
    ecs_script_type_visitor_t *t,
    ecs_script_using_t *node)
{
    ecs_allocator_t *a = &t->v->r->allocator;
    const char *name = node->name;
    int32_t len = ecs_os_strlen(name);
    char *path = NULL;
    if (len > 2 && !ecs_os_strcmp(&name[len - 2], ".*")) {
        path = flecs_strdup(a, name);
        path[len - 2] = '\0';
        name = path;
    }

    flecs_script_symbol_t symbol;
    int result = flecs_script_type_lookup(
        t, 0, name, FlecsScriptLookupEntity, NULL, &symbol);
    if (path) {
        flecs_strfree(a, path);
    }
    if (result || !symbol.entity) {
        flecs_script_type_unresolved_ref(t, node, node->name,
            FlecsScriptUnresolvedEntity);
        return -1;
    }
    node->eval = symbol.entity;
    return flecs_script_eval_node(&t->v->base, (ecs_script_node_t*)node);
}

static int flecs_script_type_node(
    ecs_script_type_visitor_t *t,
    ecs_script_node_t *node,
    bool allow_type)
{
    if (t->function_scope && node->kind != EcsAstConst) {
        flecs_script_eval_error(t->v, node,
            "only const declarations are allowed in fn body");
        return -1;
    }

    switch (node->kind) {
    case EcsAstScope: {
        ecs_script_type_table_t *cur = ecs_vec_get_t(
            &t->tables, ecs_script_type_table_t, t->table);
        int32_t table = flecs_script_type_table_new(
            t, t->table, cur->owner);
        t->control_depth ++;
        int result = flecs_script_type_scope(t,
            (ecs_script_scope_t*)node, table, true, false);
        t->control_depth --;
        return result;
    }
    case EcsAstTag:
        return flecs_script_type_tag(t, (ecs_script_tag_t*)node);
    case EcsAstWithTag:
        return flecs_script_type_with_tag(t, (ecs_script_tag_t*)node);
    case EcsAstComponent:
        return flecs_script_type_component(
            t, (ecs_script_component_t*)node);
    case EcsAstWithComponent:
        return flecs_script_type_with_component(
            t, (ecs_script_component_t*)node);
    case EcsAstWith:
        return flecs_script_type_with(t, (ecs_script_with_t*)node);
    case EcsAstUsing:
        if (t->template_scope) {
            flecs_script_eval_error(t->v, node,
                "using in template is not allowed");
            return -1;
        }
        return flecs_script_type_using(
            t, (ecs_script_using_t*)node);
    case EcsAstModule:
        return flecs_script_type_module(
            t, (ecs_script_module_t*)node);
    case EcsAstAnnotation:
        return flecs_script_type_annot(t, (ecs_script_annot_t*)node);
    case EcsAstTemplate:
        return flecs_script_type_template(
            t, (ecs_script_template_node_t*)node);
    case EcsAstProp:
        if (!t->template_scope) {
            flecs_script_eval_error(t->v, node,
                "prop variables are only allowed in templates");
            return -1;
        }
        return flecs_script_type_template_var(
            t, (ecs_script_var_node_t*)node, false);
    case EcsAstMut:
        if (!t->template_scope) {
            flecs_script_eval_error(t->v, node,
                "mut variables are only allowed in templates");
            return -1;
        }
        return flecs_script_type_template_var(
            t, (ecs_script_var_node_t*)node, true);
    case EcsAstConst:
    case EcsAstExportConst:
    case EcsAstExportMut:
        return flecs_script_type_const(
            t, (ecs_script_var_node_t*)node);
    case EcsAstEntity:
        return flecs_script_type_entity(
            t, (ecs_script_entity_t*)node, allow_type);
    case EcsAstPairScope:
        return flecs_script_type_pair_scope(
            t, (ecs_script_pair_scope_t*)node);
    case EcsAstIf:
        return flecs_script_type_if(t, (ecs_script_if_t*)node);
    case EcsAstFor:
        return flecs_script_type_for(t, (ecs_script_for_t*)node);
    case EcsAstInclude:
        if (t->template_scope) {
            flecs_script_eval_error(t->v, node,
                "include is not allowed in template");
            return -1;
        }
        return flecs_script_eval_include(
            t->v, (ecs_script_include_t*)node);
    case EcsAstFunction:
        return flecs_script_type_function(
            t, (ecs_script_function_node_t*)node);
    case EcsAstAwait: {
        int result = flecs_script_type_check_expr(t,
            &((ecs_script_await_t*)node)->expr, NULL);
        return result == 1 ? 0 : result;
    }
    case EcsAstTry:
        return flecs_script_type_try(t, (ecs_script_try_t*)node);
    }
    ecs_abort(ECS_INTERNAL_ERROR, "corrupt AST node kind");
}

int flecs_script_type_scope(
    ecs_script_type_visitor_t *t,
    ecs_script_scope_t *scope,
    int32_t table,
    bool push_vars,
    bool allow_type)
{
    ecs_script_eval_visitor_t *v = t->v;
    int32_t old_table = t->table;
    int32_t old_depth = v->base.depth;
    t->table = table;
    if (push_vars) {
        v->vars = flecs_script_vars_push(
            v->vars, &v->r->stack, &v->r->allocator);
    }

    ecs_vec_clear(&scope->components);
    ecs_assert(v->base.depth < ECS_SCRIPT_VISIT_MAX_DEPTH,
        ECS_INTERNAL_ERROR, NULL);
    v->base.nodes[v->base.depth ++] = (ecs_script_node_t*)scope;

    int32_t i, count = ecs_vec_count(&scope->stmts);
    ecs_script_node_t **stmts = ecs_vec_first(&scope->stmts);
    int result = 0;
    for (i = 0; i < count; i ++) {
        v->base.prev = i ? stmts[i - 1] : NULL;
        v->base.next = i + 1 < count ? stmts[i + 1] : NULL;
        ecs_assert(v->base.depth < ECS_SCRIPT_VISIT_MAX_DEPTH,
            ECS_INTERNAL_ERROR, NULL);
        v->base.nodes[v->base.depth ++] = stmts[i];
        result = flecs_script_type_node(t, stmts[i], allow_type);
        v->base.depth --;
        if (result) {
            break;
        }
    }

    if (!result && t->template_scope) {
        ecs_allocator_t *a = &v->base.script->allocator;
        for (i = 0; i < count; i ++) {
            ecs_id_t id = 0;
            if (stmts[i]->kind == EcsAstComponent) {
                id = ((ecs_script_component_t*)stmts[i])->id.eval;
            } else if (stmts[i]->kind == EcsAstTag) {
                id = ((ecs_script_tag_t*)stmts[i])->id.eval;
            }
            if (id) {
                ecs_vec_append_t(a, &scope->components, ecs_id_t)[0] = id;
            }
        }
    }

    v->base.depth = old_depth;
    if (push_vars) {
        v->vars = ecs_script_vars_pop(v->vars);
    }
    t->table = old_table;
    return result;
}

int flecs_script_visit_type_entity_expr(
    ecs_script_t *script,
    const ecs_expr_eval_desc_t *desc,
    ecs_script_eval_visitor_t *v,
    ecs_script_entity_t *entity)
{
    if (v && v->type_visitor) {
        ecs_script_type_visitor_t *t = v->type_visitor;
        bool old_function_scope = t->function_scope;
        t->function_scope = false;
        int result = flecs_script_type_entity(t, entity, false);
        if (old_function_scope) {
            entity->symbol = -1;
        }
        t->function_scope = old_function_scope;
        return result;
    }

    ecs_script_eval_desc_t eval_desc = {
        .runtime = desc ? desc->runtime : NULL
    };
    ecs_vec_clear(&flecs_script_impl(script)->unresolved_refs);
    ecs_vec_clear(&flecs_script_impl(script)->unresolved_component_refs);
    ecs_script_eval_visitor_t visitor;
    flecs_script_eval_visit_init(
        flecs_script_impl(script), &visitor, &eval_desc);
    if (desc && desc->vars) {
        flecs_script_eval_push_vars(&visitor, desc->vars);
    }

    ecs_script_type_visitor_t t = {
        .v = &visitor,
        .table = 0
    };
    ecs_vec_init_t(NULL, &t.tables, ecs_script_type_table_t, 0);
    ecs_vec_init_t(NULL, &t.entities, ecs_script_type_entity_t, 0);
    flecs_script_type_table_new(&t, -1, NULL);
    visitor.type_visitor = &t;

    int result = flecs_script_type_entity(&t, entity, false);
    if (flecs_script_type_report_unresolved(&visitor)) {
        result = -1;
    }

    visitor.type_visitor = NULL;
    ecs_vec_fini_t(NULL, &t.entities, ecs_script_type_entity_t);
    ecs_vec_fini_t(NULL, &t.tables, ecs_script_type_table_t);
    if (desc && desc->vars) {
        flecs_script_eval_pop_vars(&visitor);
    }
    flecs_script_eval_visit_fini(&visitor, &eval_desc);
    return result;
}

int flecs_script_visit_type(
    ecs_script_eval_visitor_t *v,
    ecs_script_scope_t *scope)
{
    ecs_vec_clear(&v->base.script->unresolved_refs);
    ecs_vec_clear(&v->base.script->unresolved_component_refs);
    ecs_entity_t outer_parent = v->parent;
    ecs_entity_t outer_module = v->module;
    ecs_vec_t outer_using = v->r->using;
    ecs_vec_t outer_annot = v->r->annot;
    v->r->using = ecs_vec_copy_t(
        &v->r->allocator, &outer_using, ecs_entity_t);
    v->r->annot = ecs_vec_copy_t(
        &v->r->allocator, &outer_annot, ecs_script_annot_t*);

    ecs_script_type_visitor_t t = {
        .v = v,
        .table = 0
    };
    ecs_vec_init_t(NULL, &t.tables, ecs_script_type_table_t, 0);
    ecs_vec_init_t(NULL, &t.entities, ecs_script_type_entity_t, 0);
    flecs_script_type_table_new(&t, -1, NULL);

    v->type_visitor = &t;
    int result = flecs_script_type_scope(
        &t, scope, 0, true, true);
    v->type_visitor = NULL;
    if (flecs_script_type_report_unresolved(v)) {
        result = -1;
    }

    ecs_vec_fini_t(NULL, &t.entities, ecs_script_type_entity_t);
    ecs_vec_fini_t(NULL, &t.tables, ecs_script_type_table_t);
    ecs_vec_fini_t(&v->r->allocator, &v->r->using, ecs_entity_t);
    ecs_vec_fini_t(&v->r->allocator, &v->r->annot, ecs_script_annot_t*);
    v->r->using = outer_using;
    v->r->annot = outer_annot;
    v->parent = outer_parent;
    v->module = outer_module;
    return result;
}

#endif
