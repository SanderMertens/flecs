#include "flecs.h"

#ifdef FLECS_SCRIPT
#include "../script.h"

typedef struct flecs_script_type_name_t {
    const char *name;
    uint64_t scope;
    ecs_size_t length;
    bool table;
} flecs_script_type_name_t;

static uint64_t flecs_script_type_name_hash(
    const void *ptr)
{
    const flecs_script_type_name_t *key = ptr;
    uint64_t hash[] = {
        flecs_hash(key->name, key->length), key->scope, key->table
    };
    return flecs_hash(hash, ECS_SIZEOF(hash));
}

static int flecs_script_type_name_compare(
    const void *first,
    const void *second)
{
    const flecs_script_type_name_t *a = first, *b = second;
    if (a->scope != b->scope || a->table != b->table ||
        a->length != b->length)
    {
        return 1;
    }

    return ecs_os_memcmp(a->name, b->name, a->length);
}

static ecs_entity_t flecs_script_type_interface_var(
    const ecs_script_type_visitor_t *t,
    int32_t sp);

static ecs_entity_t flecs_script_type_var_interface(
    const ecs_script_type_visitor_t *t,
    int32_t sp)
{
    ecs_entity_t interface = t->v->template
        ? flecs_script_template_member_interface(t->v->template, sp)
        : 0;
    return interface ? interface : flecs_script_type_interface_var(t, sp);
}

static void flecs_script_type_init(
    ecs_script_type_visitor_t *t)
{
    ecs_vec_init_t(NULL, &t->tables, ecs_script_type_table_t, 0);
    ecs_vec_init_t(NULL, &t->entities, ecs_script_type_entity_t, 0);
    ecs_vec_init_t(NULL, &t->skipped_vars, const char*, 0);
    ecs_vec_init_t(NULL, &t->interface_vars,
        ecs_script_type_interface_var_t, 0);
    flecs_hashmap_init(&t->names, flecs_script_type_name_t, int32_t,
        flecs_script_type_name_hash, flecs_script_type_name_compare, NULL);
    flecs_script_type_table_new(t, -1, NULL);
}

static void flecs_script_type_fini(
    ecs_script_type_visitor_t *t)
{
    flecs_hashmap_fini(&t->names);
    ecs_vec_fini_t(NULL, &t->interface_vars,
        ecs_script_type_interface_var_t);
    ecs_vec_fini_t(NULL, &t->skipped_vars, const char*);
    ecs_vec_fini_t(NULL, &t->entities, ecs_script_type_entity_t);
    ecs_vec_fini_t(NULL, &t->tables, ecs_script_type_table_t);
}

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

static ecs_script_entity_t* flecs_script_type_owner(
    ecs_script_type_visitor_t *t)
{
    return ecs_vec_get_t(&t->tables, ecs_script_type_table_t, t->table)->owner;
}

static ecs_size_t flecs_script_type_path_elem(
    const char *name,
    const char **sep)
{
    const char *ptr = name;
    while ((ptr = strchr(ptr, '.'))) {
        if (ptr == name || ptr[-1] != '\\') {
            *sep = ptr;
            return flecs_ito(ecs_size_t, ptr - name);
        }

        ptr ++;
    }

    *sep = NULL;
    return ecs_os_strlen(name);
}

static void flecs_script_type_unresolved_ref(
    ecs_script_type_visitor_t *t,
    void *node,
    const char *name,
    flecs_script_unresolved_kind_t kind)
{
    flecs_script_unresolved_ref_add(t->v->base.script,
        node ? ((ecs_script_node_t*)node)->pos : NULL, name, kind);
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
    bool prev_append = flecs_log_capture_set_append(true);
    for (i = 0; i < count; i ++) {
        ecs_script_unresolved_ref_t *ref = &refs[i];
        if (ref->offset != -1) {
            ecs_parser_error(impl->pub.name, code, ref->offset,
                "unresolved reference '%s'", ref->name);
        } else {
            ecs_parser_error(impl->pub.name, NULL, 0,
                "unresolved reference '%s' (line %d, column %d)",
                ref->name, ref->line, ref->column);
        }
    }

    flecs_log_capture_set_append(prev_append);
    return -1;
}

static ecs_script_type_entity_t* flecs_script_type_find_in(
    ecs_script_type_visitor_t *t,
    uint64_t scope,
    bool table,
    const char *name,
    ecs_size_t length)
{
    flecs_script_type_name_t key = {name, scope, length, table};
    const int32_t *index = flecs_hashmap_get(&t->names, &key, int32_t);
    return index ? ecs_vec_get_t(
        &t->entities, ecs_script_type_entity_t, *index) : NULL;
}

static ecs_script_type_entity_t* flecs_script_type_find(
    ecs_script_type_visitor_t *t,
    const char *name)
{
    const char *sep;
    ecs_size_t length = flecs_script_type_path_elem(name, &sep);
    int32_t table = t->table;
    ecs_script_type_entity_t *entity = NULL;
    while (!entity && table != -1) {
        entity = flecs_script_type_find_in(
            t, (uint64_t)table, true, name, length);
        table = ecs_vec_get_t(
            &t->tables, ecs_script_type_table_t, table)->parent;
    }

    while (entity && sep) {
        name = sep + 1;
        length = flecs_script_type_path_elem(name, &sep);
        ecs_script_type_entity_t *next = NULL;
        if (entity->child_table != -1) {
            next = flecs_script_type_find_in(
                t, (uint64_t)entity->child_table, true, name, length);
        }

        if (!next) {
            ecs_entity_t parent = flecs_script_symbol_entity(
                t->v, entity->slot);
            if (parent) {
                next = flecs_script_type_find_in(
                    t, parent, false, name, length);
            }
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
        ecs_assert(expected < ecs_vec_count(&impl->state.symbol_slots),
            ECS_INTERNAL_ERROR, NULL);
        return expected;
    }

    int32_t result = ecs_vec_count(&impl->state.symbol_slots);
    ecs_script_symbol_slot_t *slot = ecs_vec_append_t(
        NULL, &impl->state.symbol_slots, ecs_script_symbol_slot_t);
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
    ecs_script_type_entity_t *entity = flecs_script_type_find_in(
        t, (uint64_t)t->table, true, name, length);
    if (entity) {
        *slot = entity->slot;
        if (node && !entity->node) {
            entity->node = node;
        }

        if (node && entity->node == node) {
            node->type_index = (int32_t)(entity -
                (ecs_script_type_entity_t*)ecs_vec_first(&t->entities)) + 1;
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

    entity = ecs_vec_append_t(
        NULL, &t->entities, ecs_script_type_entity_t);
    entity->node = node;
    entity->parent_node = flecs_script_type_owner(t);
    entity->parent = t->v->parent;
    entity->child_table = -1;
    entity->slot = flecs_script_type_slot_new(t, *slot);
    entity->in_template = t->template_scope;
    entity->in_for = t->for_depth != 0;
    int32_t index = ecs_vec_count(&t->entities) - 1;
    flecs_script_type_name_t key = {name, (uint64_t)t->table, length, true};
    flecs_hashmap_result_t result = flecs_hashmap_ensure(
        &t->names, &key, int32_t);
    *(int32_t*)result.value = index;
    if (!entity->parent_node) {
        key.scope = entity->parent;
        key.table = false;
        result = flecs_hashmap_ensure(&t->names, &key, int32_t);
        *(int32_t*)result.value = index;
    }

    if (node) {
        node->type_index = index + 1;
    }

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
    return node->type_index ? ecs_vec_get_t(
        &t->entities, ecs_script_type_entity_t, node->type_index - 1) : NULL;
}

static int flecs_script_type_lookup(
    ecs_script_type_visitor_t *t,
    ecs_entity_t first,
    const char *name,
    ecs_flags32_t lookup_kind,
    ecs_entity_t *from_out,
    flecs_script_symbol_t *symbol)
{
    ecs_expr_eval_desc_t desc = flecs_script_expr_desc(t->v, 0);
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

static void flecs_script_skip_unknown_skip_refs(
    ecs_script_type_visitor_t *t,
    int32_t prev_unresolved)
{
    ecs_script_impl_t *impl = t->v->base.script;
    int32_t i, count = ecs_vec_count(&impl->unresolved_refs);
    ecs_script_unresolved_ref_t *refs = ecs_vec_first(&impl->unresolved_refs);
    for (i = prev_unresolved; i < count; i ++) {
        flecs_script_skip_unknown_warn(&impl->pub, refs[i].name,
            "skipped expression with unresolved reference");
    }

    ecs_vec_set_count_t(NULL, &impl->unresolved_refs,
        ecs_script_unresolved_ref_t, prev_unresolved);

    t->skip_count ++;
    ecs_script_visit_t *base = &t->v->base;
    if (base->depth) {
        base->nodes[base->depth - 1]->skip = true;
    }
}

static void flecs_script_type_skip_var(
    ecs_script_type_visitor_t *t,
    const char *name)
{
    t->skip_count ++;
    if (!name) {
        return;
    }

    ecs_vec_append_t(NULL, &t->skipped_vars, const char*)[0] = name;
}

bool flecs_script_type_is_skipped_var(
    ecs_script_type_visitor_t *t,
    const char *name)
{
    int32_t i, count = ecs_vec_count(&t->skipped_vars);
    const char **vars = ecs_vec_first(&t->skipped_vars);
    for (i = 0; i < count; i ++) {
        ecs_size_t len = ecs_os_strlen(vars[i]);
        if (ecs_os_strncmp(name, vars[i], len)) {
            continue;
        }

        if (!name[len] || name[len] == '.') {
            return true;
        }
    }

    return false;
}

static void flecs_script_type_prune_skipped_refs(
    ecs_script_type_visitor_t *t,
    int32_t prev_unresolved)
{
    if (!ecs_vec_count(&t->skipped_vars)) {
        return;
    }

    ecs_script_impl_t *impl = t->v->base.script;
    int32_t i, count = ecs_vec_count(&impl->unresolved_refs);
    ecs_script_unresolved_ref_t *refs = ecs_vec_first(&impl->unresolved_refs);
    int32_t dst = prev_unresolved;
    for (i = prev_unresolved; i < count; i ++) {
        if (refs[i].kind != FlecsScriptUnresolvedComponent &&
            flecs_script_type_is_skipped_var(t, refs[i].name))
        {
            continue;
        }

        refs[dst ++] = refs[i];
    }

    ecs_vec_set_count_t(NULL, &impl->unresolved_refs,
        ecs_script_unresolved_ref_t, dst);
}

static int flecs_script_type_check_expr(
    ecs_script_type_visitor_t *t,
    ecs_expr_node_t **expr_ptr,
    ecs_entity_t *type)
{
    ecs_script_eval_visitor_t *v = t->v;
    ecs_script_t *script = &v->base.script->pub;
    ecs_entity_t expected_type = type ? type[0] : 0;
    ecs_expr_eval_desc_t desc = flecs_script_expr_desc(v, expected_type);

    bool was_typed = expr_ptr[0]->type_info != NULL;
    if (!was_typed) {
        int32_t prev_unresolved = ecs_vec_count(
            &v->base.script->unresolved_refs);
        if (flecs_expr_visit_type(script, expr_ptr, &desc)) {
            if (ecs_vec_count(&v->base.script->unresolved_refs) >
                prev_unresolved)
            {
                flecs_script_type_prune_skipped_refs(t, prev_unresolved);
                if (v->base.script->skip_unknown) {
                    flecs_script_skip_unknown_skip_refs(t, prev_unresolved);
                }

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

    if (type) {
        type[0] = expr_ptr[0]->type;
    }

    return 0;
}

static void flecs_script_skip_unknown_drop_expr(
    ecs_script_type_visitor_t *t,
    ecs_expr_node_t **expr)
{
    flecs_expr_visit_free(&t->v->base.script->pub, *expr);
    *expr = NULL;
}

static bool flecs_script_skip_unknown_drop_var(
    ecs_script_type_visitor_t *t,
    ecs_script_var_node_t *node)
{
    if (!flecs_script_is_skip_unknown(&t->v->base.script->pub)) {
        return false;
    }

    flecs_script_skip_unknown_warn(&t->v->base.script->pub, node->type,
        "skipped variable with unresolved type");

    flecs_script_type_skip_var(t, node->name);

    if (node->expr) {
        flecs_script_skip_unknown_drop_expr(t, &node->expr);
    }

    node->eval_type = 0;
    node->node.skip = true;

    return true;
}

static bool flecs_script_skip_unknown_drop_fn(
    ecs_script_type_visitor_t *t,
    ecs_script_function_node_t *node,
    const char *type)
{
    if (!flecs_script_is_skip_unknown(&t->v->base.script->pub)) {
        return false;
    }

    flecs_script_skip_unknown_warn(&t->v->base.script->pub, type,
        "skipped function with unresolved type");

    node->node.skip = true;

    return true;
}

static bool flecs_script_is_var_name(
    const char *name)
{
    if (!name || name[0] != '$' || !name[1]) {
        return false;
    }

    const char *ptr;
    for (ptr = &name[1]; ptr[0]; ptr ++) {
        if (!isalnum((unsigned char)ptr[0]) && ptr[0] != '_') {
            return false;
        }
    }

    return true;
}

static int flecs_script_type_template_prop_id(
    ecs_script_type_visitor_t *t,
    void *node,
    ecs_script_id_t *id,
    const char *name,
    int32_t var_sp,
    ecs_entity_t interface,
    int32_t *sp)
{
    if (t->v->template && flecs_script_template_member_is_vector(
        t->v->template, var_sp))
    {
        flecs_script_eval_error(t->v, node,
            "prop '%s' is a vector, use '%s[index]' to select an "
            "element", name, name);
        return -1;
    }

    if (id->second) {
        flecs_script_eval_error(t->v, node,
            "template prop '%s' cannot be used as pair element", name);
        return -1;
    }

    id->interface = interface;
    *sp = var_sp;
    id->dynamic = true;
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
        if (t->v->template && (*name_expr)->kind == EcsExprInterpolatedString) {
            const char *value = ((ecs_expr_interpolated_string_t*)
                *name_expr)->value;
            if (flecs_script_is_var_name(value)) {
                ecs_script_var_t *var = ecs_script_vars_lookup(
                    t->v->vars, &value[1]);
                ecs_entity_t interface = var
                    ? flecs_script_type_var_interface(t, var->sp)
                    : 0;
                if (interface) {
                    if (flecs_script_type_template_prop_id(
                        t, node, id, &value[1], var->sp, interface, sp))
                    {
                        return -1;
                    }

                    flecs_expr_visit_free(&t->v->base.script->pub, *name_expr);
                    *name_expr = NULL;
                    return 0;
                }
            }
        }

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

        ecs_script_node_kind_t node_kind = ((ecs_script_node_t*)node)->kind;
        if (flecs_script_is_skip_unknown(&t->v->base.script->pub) &&
            (node_kind == EcsAstTag || node_kind == EcsAstComponent ||
             node_kind == EcsAstWithTag || node_kind == EcsAstWithComponent) &&
            first != EcsIsA && id->first_eval != EcsIsA)
        {
            flecs_script_skip_unknown_warn(&t->v->base.script->pub, name,
                "skipped statement with unresolved component");
            ((ecs_script_node_t*)node)->skip = true;
            return 1;
        }

        flecs_script_unresolved_kind_t ref_kind = FlecsScriptUnresolvedEntity;
        if (!id->second) {
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
        ecs_entity_t interface = var
            ? flecs_script_type_var_interface(t, symbol.sp)
            : 0;
        if (interface) {
            return flecs_script_type_template_prop_id(
                t, node, id, name, symbol.sp, interface, sp);
        }

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

static int flecs_script_type_index_id(
    ecs_script_type_visitor_t *t,
    void *node,
    ecs_script_id_t *id)
{
    if (id->second) {
        flecs_script_eval_error(t->v, node,
            "vector prop '%s' cannot be used as pair element", id->first);
        return -1;
    }

    ecs_script_var_t *var = ecs_script_vars_lookup(t->v->vars, id->first);
    if (!var || !t->v->template || !flecs_script_template_member_is_vector(
        t->v->template, var->sp))
    {
        flecs_script_eval_error(t->v, node,
            "'%s' is not a vector prop, cannot use '[]'", id->first);
        return -1;
    }

    ecs_entity_t index_type = ecs_id(ecs_i32_t);
    int result = flecs_script_type_check_expr(t, &id->index_expr, &index_type);
    if (result) {
        return result;
    }

    id->interface = flecs_script_template_member_interface(
        t->v->template, var->sp);
    id->index_sp = var->sp;
    id->dynamic = true;

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
    id->index_sp = -1;
    id->interface = 0;
    id->dynamic = id->first_expr || id->second_expr;

    if (id->index_expr) {
        return flecs_script_type_index_id(t, node, id);
    }

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

static ecs_entity_t flecs_script_type_node_entity(
    ecs_script_type_visitor_t *t,
    ecs_script_entity_t *node)
{
    return node->symbol != -1
        ? flecs_script_symbol_entity(t->v, node->symbol)
        : node->eval;
}

static ecs_entity_t flecs_script_type_parent(
    ecs_script_type_visitor_t *t,
    ecs_script_entity_t *owner,
    ecs_entity_t parent)
{
    if (!owner) {
        return parent;
    }

    if (flecs_script_type_ensure_node(t, owner)) {
        return 0;
    }

    return flecs_script_type_node_entity(t, owner);
}

static int flecs_script_type_ensure_owner(
    ecs_script_type_visitor_t *t,
    ecs_entity_t *parent)
{
    ecs_script_entity_t *owner = flecs_script_type_owner(t);
    *parent = flecs_script_type_parent(t, owner, t->v->parent);
    return owner && !*parent ? -1 : 0;
}

static int flecs_script_type_ensure_node(
    ecs_script_type_visitor_t *t,
    ecs_script_entity_t *node)
{
    ecs_entity_t current = flecs_script_type_node_entity(t, node);
    if (current && ecs_is_alive(t->v->world, current)) {
        return 0;
    }

    ecs_script_type_entity_t *entry = flecs_script_type_entity_from_node(
        t, node);
    ecs_script_entity_t *parent_node = entry ? entry->parent_node : NULL;
    ecs_entity_t parent = flecs_script_type_parent(
        t, parent_node, entry ? entry->parent : 0);
    if (parent_node && !parent) {
        return -1;
    }

    ecs_value_t value = {0};
    if (node->name_expr) {
        value.type = node->name_expr->type;
        if (flecs_script_eval_expr(t->v, &node->name_expr, &value)) {
            return -1;
        }
    }

    current = value.ptr && value.type == ecs_id(ecs_entity_t)
        ? *(ecs_entity_t*)value.ptr
        : flecs_script_new_entity(t->v, parent,
            value.ptr ? *(char**)value.ptr : node->name,
            parent_node && parent_node->non_fragmenting_parent);
    if (value.ptr) {
        ecs_ptr_free(t->v->world, value.type, value.ptr);
    }

    if (!current) {
        return -1;
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
        if (entries[i].in_template && !t->template_scope) {
            /* Entities declared in a template body are created when the
             * template is instantiated, not in the scope of the script. */
            continue;
        }

        if (entries[i].in_for && !t->for_depth) {
            continue;
        }

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
        return result;
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

    if (node->id.eval &&
        !flecs_script_can_default_ctor(t->v->world, node->id.eval))
    {
        flecs_script_eval_error(t->v, node,
            "cannot add %s, type is not default constructible",
            node->id.first);
        return -1;
    }

    return 0;
}

static ecs_entity_t flecs_script_type_component_type(
    ecs_script_type_visitor_t *t,
    ecs_script_component_t *node)
{
    if (node->id.interface) {
        return node->id.interface;
    }

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
        bool is_init = elems[i].value &&
            (elems[i].value->kind == EcsExprInitializer ||
             elems[i].value->kind == EcsExprEmptyInitializer);
        ecs_entity_t type = is_init
            ? ecs_id(EcsMember)
            : ecs_id(ecs_entity_t);
        if (is_init) {
            ((ecs_expr_initializer_t*)elems[i].value)->is_partial = true;
        }

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

static int flecs_script_type_meta_type_expr(
    ecs_script_type_visitor_t *t,
    ecs_expr_node_t *expr,
    ecs_entity_t kind)
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
        if (!elems[i].value) {
            continue;
        }

        const ecs_member_t *m = flecs_script_meta_type_member(
            t->v->world, kind, elems[i].member, i);
        ecs_entity_t type = m ? m->type : ecs_id(ecs_entity_t);
        int result = flecs_script_type_check_expr(
            t, &elems[i].value, &type);
        if (result) {
            return result;
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

    if (node->id.eval == ecs_id(EcsVector) ||
        node->id.eval == ecs_id(EcsArray) ||
        node->id.eval == ecs_id(EcsMap) ||
        node->id.eval == ecs_id(EcsPrimitive))
    {
        return flecs_script_type_meta_type_expr(
            t, node->expr, node->id.eval);
    }

    return 0;
}

static int flecs_script_type_check_component_expr(
    ecs_script_type_visitor_t *t,
    ecs_script_component_t *node,
    ecs_entity_t *type)
{
    ecs_expr_initializer_t *init = NULL;
    if (node->expr && (node->expr->kind == EcsExprInitializer ||
         node->expr->kind == EcsExprEmptyInitializer))
    {
        init = (ecs_expr_initializer_t*)node->expr;
        if (node->is_partial || init->is_partial) {
            init->is_partial = true;
            init = NULL;
        } else if (node->id.interface) {
            init->is_partial = true;
        } else {
            init = NULL;
        }
    }

    int result = flecs_script_type_check_expr(t, &node->expr, type);

    if (init && node->expr == (ecs_expr_node_t*)init) {
        init->is_partial = false;
    }

    return result;
}

static int flecs_script_type_untyped_template_check(
    ecs_script_type_visitor_t *t,
    ecs_script_component_t *node)
{
    if (node->id.interface != ecs_id(EcsStruct)) {
        return 0;
    }

    flecs_script_eval_error(t->v, node,
        "cannot assign value to untyped template prop '%s'", node->id.first);
    return -1;
}

static int flecs_script_type_with_component(
    ecs_script_type_visitor_t *t,
    ecs_script_component_t *node)
{
    int result = flecs_script_type_id(t, node, &node->id);
    if (result || !node->expr) {
        return result;
    }

    if (flecs_script_type_untyped_template_check(t, node)) {
        return -1;
    }

    ecs_entity_t type = flecs_script_type_component_type(t, node);
    if (!type) {
        flecs_script_eval_error(t->v, node,
            "unresolved component type '%s'", node->id.first);
        return -1;
    }

    return flecs_script_type_check_component_expr(t, node, &type);
}

static void flecs_script_type_check_overwrite(
    ecs_script_type_visitor_t *t,
    ecs_script_component_t *node)
{
    ecs_script_id_t *id = &node->id;
    if (id->first_expr || id->second_expr || id->first_sp != -1 ||
        id->second_sp != -1)
    {
        return;
    }

    const char *name = t->v->entity->node->name;
    if (t->for_entity) {
        flecs_script_eval_warn(t->v, node,
            "value of '%s' for entity '%s' is overwritten in each loop "
            "iteration: a named entity in a loop body refers to the same "
            "entity in every iteration", id->first, name);
        return;
    }

    if (!t->reopened) {
        return;
    }

    int32_t i, count = ecs_vec_count(&t->reopened->scope->stmts);
    ecs_script_node_t **stmts = ecs_vec_first(&t->reopened->scope->stmts);
    for (i = 0; i < count; i ++) {
        if (stmts[i]->kind != EcsAstComponent) {
            continue;
        }

        ecs_script_id_t *prev = &((ecs_script_component_t*)stmts[i])->id;
        if (!ecs_os_strcmp(prev->first, id->first) &&
            (prev->second == id->second || (prev->second && id->second &&
                !ecs_os_strcmp(prev->second, id->second))))
        {
            flecs_script_eval_warn(t->v, node,
                "value of '%s' for entity '%s' overwrites the value set by a "
                "previous scope: opening a scope for an existing entity does "
                "not create a new entity", id->first, name);
            return;
        }
    }
}

static int flecs_script_type_component(
    ecs_script_type_visitor_t *t,
    ecs_script_component_t *node)
{
    int result = flecs_script_type_id(t, node, &node->id);
    if (result) {
        return result;
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

    if (flecs_script_type_untyped_template_check(t, node)) {
        return -1;
    }

    if (!node->is_partial) {
        flecs_script_type_check_overwrite(t, node);
    }

    if (!node->id.second && node->id.eval && ecs_has(
        t->v->world, node->id.eval, EcsScriptVisitor))
    {
        if (node->is_partial) {
            flecs_script_eval_error(t->v, node,
                "cannot partially assign '%s'", node->id.first);
            return -1;
        }

        return flecs_script_type_visitor_expr(t, node);
    }

    ecs_entity_t component_type = flecs_script_type_component_type(t, node);
    if (!component_type || !ecs_get_type_info(t->v->world, component_type)) {
        if (t->v->base.script->skip_unknown) {
            flecs_script_skip_unknown_warn(&t->v->base.script->pub, node->id.first,
                "skipped value for component without reflection data");
            flecs_script_skip_unknown_drop_expr(t, &node->expr);
            return 0;
        }

        flecs_script_eval_error(t->v, node, component_type
            ? "cannot set value of '%s': not a component"
            : "unresolved component type '%s'", node->id.first);
        return -1;
    }

    return flecs_script_type_check_component_expr(t, node, &component_type);
}

static int flecs_script_type_control_scope(
    ecs_script_type_visitor_t *t,
    ecs_script_scope_t *scope)
{
    int32_t table = flecs_script_type_table_new(
        t, t->table, flecs_script_type_owner(t));
    t->control_depth ++;
    int result = flecs_script_type_scope(t, scope, table, true, false);
    t->control_depth --;
    return result;
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
    t->control_depth ++;
    int result = flecs_script_type_scope(
        t, node->scope, t->table, true, false);
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
    if (!v->base.prev || v->base.prev->kind != EcsAstAnnotation) {
        t->annot_start = ecs_vec_count(&v->r->annot);
    }

    /* Recording an annotation is safe while a template is being preprocessed,
     * as it does not evaluate anything in the world. Clear the template so the
     * evaluation entrypoint doesn't reject the node. */
    ecs_script_template_t *template = v->template;
    v->template = NULL;
    int result = flecs_script_eval_node(&v->base, (ecs_script_node_t*)node);
    v->template = template;

    /* Apply the tree annotations to the ast while typing. Entities can be
     * created before the annotations are evaluated (see
     * flecs_script_type_ensure_node) which means the hierarchy storage has to
     * be known before evaluation starts. */
    if (!result && kind == EcsAstEntity) {
        int32_t i, count = ecs_vec_count(&v->r->annot);
        ecs_script_annot_t **annots = ecs_vec_first(&v->r->annot);
        for (i = t->annot_start; i < count; i ++) {
            flecs_script_apply_tree_annot(
                annots[i], (ecs_script_entity_t*)v->base.next);
        }
    }

    return result;
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
        if (flecs_script_skip_unknown_drop_var(t, node)) {
            return 0;
        }

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
        if (expr_result == 1) {
            flecs_script_type_skip_var(t, node->name);
            return 0;
        }

        return -1;
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

        ecs_entity_t old_parent = t->v->parent, parent;
        if (flecs_script_type_ensure_owner(t, &parent)) {
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

    ecs_script_entity_t *owner = flecs_script_type_owner(t);
    ecs_entity_t parent = owner
        ? flecs_script_type_node_entity(t, owner)
        : t->v->parent;
    if (parent || !owner) {
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

    ecs_expr_eval_desc_t desc = flecs_script_expr_desc(t->v, type);
    if (flecs_expr_visit_fold_const(
        &t->v->base.script->pub, &node->expr, &desc))
    {
        return -1;
    }

    if (node->expr->kind == EcsExprValue) {
        ecs_expr_value_node_t *value = (ecs_expr_value_node_t*)node->expr;
        const ecs_type_info_t *ti = var->type_info;
        var->value.ptr = flecs_stack_calloc(
            &t->v->r->stack, ti->size, ti->alignment);
        flecs_type_info_ctor(var->value.ptr, 1, ti);
        ecs_ptr_copy_w_type_info(
            t->v->world, ti, var->value.ptr, value->ptr);
        var->owned = true;
    }

    return 0;
}

static int flecs_script_type_template_var(
    ecs_script_type_visitor_t *t,
    ecs_script_var_node_t *node,
    bool mut)
{
    if (!node->expr && (mut || (!node->type && !node->type_is_template))) {
        flecs_script_eval_error(t->v, node,
            "%s variable '%s' is missing initializer",
            mut ? "mut" : "prop", node->name);
        return -1;
    }

    ecs_entity_t type = 0;
    if (node->type && flecs_script_type_resolve_type(
        t, node->type, &type))
    {
        if (flecs_script_skip_unknown_drop_var(t, node)) {
            return 0;
        }

        flecs_script_type_unresolved_ref(t, node, node->type,
            FlecsScriptUnresolvedComponent);
        if (!node->expr || node->expr->kind == EcsExprInitializer ||
            node->expr->kind == EcsExprEmptyInitializer)
        {
            return -1;
        }
    }

    if (type && t->v->template && type == t->v->template->props.type) {
        flecs_script_eval_error(t->v, node,
            "%s '%s' cannot have the type of the template that declares it",
            mut ? "mut" : "prop", node->name);
        return -1;
    }

    if (node->type_is_template && !node->type) {
        node->eval_interface = ecs_id(EcsStruct);
        type = ecs_id(ecs_script_template_ref_t);
    } else if (node->type_is_template) {
        if (type && ecs_has(t->v->world, type, EcsStruct)) {
            node->eval_interface = type;
            type = ecs_id(ecs_script_template_ref_t);
        } else {
            flecs_script_eval_error(t->v, node,
                "type '%s' of prop '%s' is not a template or struct",
                node->type, node->name);
            return -1;
        }
    }

    if (node->type_is_vector) {
        type = flecs_script_vector_type(
            t->v->world, ecs_id(ecs_script_template_ref_t));
    }

    if (node->expr) {
        int result = flecs_script_type_check_expr(t, &node->expr, &type);
        if (result) {
            if (result == 1) {
                flecs_script_type_skip_var(t, node->name);
                return 0;
            }

            return -1;
        }
    }

    node->eval_type = type;
    return flecs_script_template_eval_var(t->v, node, mut);
}

static ecs_entity_t flecs_script_type_interface_var(
    const ecs_script_type_visitor_t *t,
    int32_t sp)
{
    const ecs_script_type_interface_var_t *vars = ecs_vec_first(
        &t->interface_vars);
    int32_t i, count = ecs_vec_count(&t->interface_vars);
    for (i = count - 1; i >= 0; i --) {
        if (vars[i].sp == sp) {
            return vars[i].interface;
        }
    }

    return 0;
}

static ecs_entity_t flecs_script_type_for_interface(
    ecs_script_type_visitor_t *t,
    ecs_script_for_t *node,
    ecs_entity_t elem_type)
{
    if (!node->expr || elem_type != ecs_id(ecs_script_template_ref_t) ||
        !t->v->template)
    {
        return 0;
    }

    if (node->expr->kind != EcsExprVariable) {
        return 0;
    }

    int32_t sp = ((ecs_expr_variable_t*)node->expr)->sp;
    if (sp < 0) {
        return 0;
    }

    if (!flecs_script_template_member_is_vector(t->v->template, sp)) {
        return 0;
    }

    return flecs_script_template_member_interface(t->v->template, sp);
}

static int flecs_script_type_for(
    ecs_script_type_visitor_t *t,
    ecs_script_for_t *node)
{
    flecs_script_for_state_t state = {0};
    ecs_value_t collection = {0};
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
        expr_result = flecs_script_type_check_expr(
            t, &node->expr, &collection.type);
        if (expr_result) {
            return expr_result == 1 ? 0 : -1;
        }
    }

    if (flecs_script_for_init(t->v, node, &state,
            node->expr ? &collection : NULL) ||
        flecs_script_for_declare_vars(t->v, node, &state, true))
    {
        return -1;
    }

    int32_t i;
    for (i = 0; i < node->loop_var_count; i ++) {
        node->loop_var_sp[i] = t->v->vars->sp + i;
    }

    int32_t interface_var_count = ecs_vec_count(&t->interface_vars);
    ecs_entity_t elem_interface = flecs_script_type_for_interface(
        t, node, state.var_types[state.elem_var]);
    if (elem_interface) {
        ecs_script_type_interface_var_t *iv = ecs_vec_append_t(NULL,
            &t->interface_vars, ecs_script_type_interface_var_t);
        iv->sp = node->loop_var_sp[state.elem_var];
        iv->interface = elem_interface;
    }

    bool old_for_static = t->for_static;
    bool old_for_entity = t->for_entity;
    t->for_static = true;
    t->for_entity = false;
    t->for_depth ++;
    int result = flecs_script_type_control_scope(t, node->scope);
    t->for_depth --;
    t->for_static = old_for_static;
    t->for_entity = old_for_entity;
    ecs_vec_set_count_t(NULL, &t->interface_vars,
        ecs_script_type_interface_var_t, interface_var_count);
    t->v->vars = ecs_script_vars_pop(t->v->vars);
    return result;
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

    int result = flecs_script_type_control_scope(t, node->scope);
    return result;
}

static int flecs_script_type_if(
    ecs_script_type_visitor_t *t,
    ecs_script_if_t *node)
{
    if (flecs_script_type_check_expr(t, &node->expr, NULL) == -1) {
        return -1;
    }

    if (flecs_script_type_control_scope(t, node->if_true)) {
        return -1;
    }

    return flecs_script_type_control_scope(t, node->if_false);
}

static int flecs_script_type_try(
    ecs_script_type_visitor_t *t,
    ecs_script_try_t *node)
{
    if (flecs_script_type_control_scope(t, node->try_scope)) {
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

        if (flecs_script_type_control_scope(t, catch_->scope)) {
            return -1;
        }
    }

    return 0;
}

static int flecs_script_type_function(
    ecs_script_type_visitor_t *t,
    ecs_script_function_node_t *node)
{
    if (t->template_scope) {
        flecs_script_eval_error(t->v, node,
            "fn is not allowed in template");
        return -1;
    }

    if (ecs_vec_count(&node->params) > FLECS_SCRIPT_FUNCTION_ARGS_MAX) {
        flecs_script_eval_error(t->v, node,
            "too many parameters for function '%s' (max %d)",
            node->name, FLECS_SCRIPT_FUNCTION_ARGS_MAX);
        return -1;
    }

    flecs_script_type_declare(
        t, node->name, NULL, &node->symbol, false);
    if (flecs_script_type_resolve_type(
        t, node->return_type, &node->eval_return_type))
    {
        if (!flecs_script_skip_unknown_drop_fn(t, node, node->return_type)) {
            flecs_script_type_unresolved_ref(
                t, &node->return_type_node, node->return_type,
                FlecsScriptUnresolvedComponent);
        }

        return 0;
    }

    ecs_entity_t parent;
    if (flecs_script_type_ensure_owner(t, &parent)) {
        return -1;
    }

    ecs_entity_t fn_entity = 0;
    bool keep = false;
    int result = -1;
    ecs_script_vars_t *outer_vars = t->v->vars;
    t->v->vars = flecs_script_vars_push(
        NULL, &t->v->r->stack, t->v->r->allocator);
    int32_t i, param_count = ecs_vec_count(&node->params);
    ecs_script_fn_param_t *params = ecs_vec_first(&node->params);
    for (i = 0; i < param_count; i ++) {
        if (flecs_script_type_resolve_type(
            t, params[i].type, &params[i].eval_type))
        {
            if (!flecs_script_skip_unknown_drop_fn(t, node, params[i].type)) {
                flecs_script_type_unresolved_ref(
                    t, &params[i].node, params[i].type,
                    FlecsScriptUnresolvedComponent);
            }

            result = 0;
            goto done;
        }

        ecs_script_var_t *var = ecs_script_vars_declare(
            t->v->vars, params[i].name);
        if (!var) {
            flecs_script_eval_error(t->v, &params[i].node,
                "duplicate parameter '%s' in function '%s'",
                params[i].name, node->name);
            goto done;
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
        goto done;
    }

    flecs_script_symbol_set(t->v, node->symbol, fn_entity);
    if (flecs_script_type_ensure_entities(t)) {
        goto done;
    }

    int32_t table = flecs_script_type_table_new(
        t, t->table, flecs_script_type_owner(t));
    int32_t prev_unresolved = ecs_vec_count(
        &t->v->base.script->unresolved_refs);
    int32_t prev_skip_count = t->skip_count;
    bool old_function_scope = t->function_scope;
    t->function_scope = true;
    result = flecs_script_type_scope(t, node->body, table, false, false);
    if (!result && node->return_expr) {
        ecs_entity_t return_type = node->eval_return_type;
        result = flecs_script_type_check_expr(
            t, &node->return_expr, &return_type);
    }

    t->function_scope = old_function_scope;
    if (!result && t->skip_count == prev_skip_count &&
        ecs_vec_count(&t->v->base.script->unresolved_refs) == prev_unresolved)
    {
        result = flecs_script_eval_function(t->v, node, fn_entity);
        keep = !result;
    } else if (result != -1) {
        if (flecs_script_is_skip_unknown(&t->v->base.script->pub)) {
            flecs_script_skip_unknown_warn(&t->v->base.script->pub, node->name,
                "skipped function with unresolved reference");
            node->node.skip = true;
        }

        result = 0;
    }

done:
    ecs_script_vars_pop(t->v->vars);
    t->v->vars = outer_vars;
    if (fn_entity && !keep) {
        ecs_delete(t->v->world, fn_entity);
        flecs_script_symbol_set(t->v, node->symbol, 0);
    }

    return result;
}

static bool flecs_script_type_entity_name_ref(
    ecs_script_type_visitor_t *t,
    ecs_script_entity_t *node)
{
    if (!flecs_script_is_var_name(node->name)) {
        return false;
    }

    ecs_script_var_t *var = ecs_script_vars_lookup(t->v->vars, &node->name[1]);
    if (!var || var->value.type != ecs_id(ecs_entity_t) ||
        flecs_script_type_var_interface(t, var->sp))
    {
        return false;
    }

    ecs_expr_node_t *expr = node->name_expr;
    if (expr->kind == EcsExprInterpolatedString) {
        ecs_script_t *script = &t->v->base.script->pub;
        node->name_expr = (ecs_expr_node_t*)flecs_expr_variable_from(
            script, expr, &node->name[1]);
        flecs_expr_visit_free(script, expr);
    }

    return true;
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
    node->type_index = 0;
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
        ecs_entity_t type = flecs_script_type_entity_name_ref(t, node)
            ? ecs_id(ecs_entity_t)
            : ecs_id(ecs_string_t);
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

#ifdef FLECS_PREFAB
    if (node->eval_kind == EcsPrefab && !node->tree_annot) {
        flecs_script_apply_tree(node, true, false);
    }
#endif

    flecs_script_entity_state_t state = {
        .node = node,
        .eval_kind = node->eval_kind
    };
    flecs_script_entity_state_t *old_entity = t->v->entity;
    bool old_with = t->v->is_with_scope;
    t->v->entity = &state;
    t->v->is_with_scope = false;

    bool old_for_static = t->for_static;
    bool old_for_entity = t->for_entity;
    ecs_script_entity_t *old_reopened = t->reopened;
    t->for_static = t->for_entity = t->for_static && entry;
    t->reopened = entry && entry->node != node ? entry->node : NULL;

    if (!node->is_type) {
        ecs_vec_clear(&t->v->r->annot);
    }

    int32_t prev_unresolved = ecs_vec_count(
        &t->v->base.script->unresolved_refs);
    int result = flecs_script_type_scope(
        t, node->scope, child_table, true, true);
    t->v->entity = old_entity;
    t->v->is_with_scope = old_with;
    t->for_static = old_for_static;
    t->for_entity = old_for_entity;
    t->reopened = old_reopened;
    if (result) {
        return -1;
    }

    if (node->is_type) {
        if (ecs_vec_count(&t->v->base.script->unresolved_refs) >
            prev_unresolved)
        {
            /* The definition references a symbol that isn't resolved yet. Skip
             * evaluating it, so the definition is either retried once a later
             * include resolves the symbol, or reported as an unresolved
             * reference when the type visitor is done. */
            return 0;
        }

        if (flecs_script_type_ensure_node(t, node)) {
            return -1;
        }

        if (entry) {
            entry = flecs_script_type_entity_from_node(t, node);
        }

        ecs_entity_t old_parent = t->v->parent;
        ecs_entity_t parent = entry
            ? flecs_script_type_parent(t, entry->parent_node, entry->parent)
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

    ecs_entity_t old_parent = t->v->parent, parent;
    if (flecs_script_type_ensure_owner(t, &parent)) {
        return -1;
    }

    t->v->parent = parent;

    node->eval_base = 0;
    if (node->base) {
        if (flecs_script_type_resolve_type(t, node->base, &node->eval_base)) {
            flecs_script_eval_error(t->v, node,
                "unresolved base '%s' for template '%s'",
                node->base, node->name);
            t->v->parent = old_parent;
            return -1;
        }
    }

    node->eval_parent = 0;
    if (node->parent && flecs_script_type_resolve_type(
        t, node->parent, &node->eval_parent))
    {
        flecs_script_eval_error(t->v, node,
            "unresolved parent template '%s'", node->parent);
        t->v->parent = old_parent;
        return -1;
    }

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
    ecs_id_t old_script_tag = t->v->script_tag;
    t->v->parent = 0;
    t->v->script_tag = 0;
    node->eval = flecs_script_create_entity(t->v, node->name);
    t->v->script_tag = old_script_tag;
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
    ecs_allocator_t *a = t->v->r->allocator;
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
        flecs_free_n(a, char, len + 1, path);
    }

    if (result || !symbol.entity) {
        if (flecs_script_is_skip_unknown(&t->v->base.script->pub)) {
            flecs_script_skip_unknown_warn(&t->v->base.script->pub, node->name,
                "skipped using with unresolved identifier");
            node->node.skip = true;
            return 0;
        }

        flecs_script_type_unresolved_ref(t, node, node->name,
            FlecsScriptUnresolvedEntity);
        return -1;
    }

    node->eval = symbol.entity;
    return flecs_script_eval_node(&t->v->base, (ecs_script_node_t*)node);
}

static int flecs_script_type_async(
    ecs_script_type_visitor_t *t,
    ecs_script_async_t *node)
{
#ifndef FLECS_SCRIPT_ASYNC
    flecs_script_eval_error(t->v, node,
        "async blocks require FLECS_SCRIPT_ASYNC");
    return -1;
#else
    ecs_script_eval_visitor_t *v = t->v;
    if (t->async_scope) {
        flecs_script_eval_error(v, node,
            "async blocks cannot be nested");
        return -1;
    }

    ecs_script_scope_t *parent = v->base.depth >= 2
        ? (ecs_script_scope_t*)v->base.nodes[v->base.depth - 2]
        : NULL;
    bool in_root = parent == v->base.script->root;
    bool in_template = v->template && !t->function_scope &&
        (parent == v->template->node->scope ||
            (v->entity && parent == v->entity->node->scope));
    if (!in_root && !in_template) {
        flecs_script_eval_error(v, node,
            "async blocks are only allowed in the root scope of a script or "
            "in template entity scopes");
        return -1;
    }

    if (in_template) {
        v->vars = flecs_script_vars_push(
            v->vars, &v->r->stack, v->r->allocator);
        ecs_script_var_t *this_var = ecs_script_vars_declare(v->vars, "this");
        this_var->value.type = ecs_id(ecs_entity_t);
    }

    int32_t old_for_depth = t->for_depth;
    t->for_depth = 0;
    t->async_scope = true;
    int result = flecs_script_type_control_scope(t, node->scope);
    t->async_scope = false;
    t->for_depth = old_for_depth;
    if (in_template) {
        v->vars = ecs_script_vars_pop(v->vars);
    }

    return result;
#endif
}

static int flecs_script_type_while(
    ecs_script_type_visitor_t *t,
    ecs_script_while_t *node)
{
    if (!t->async_scope) {
        flecs_script_eval_error(t->v, node,
            "while is only allowed in async blocks");
        return -1;
    }

    ecs_entity_t type = 0;
    int expr_result = flecs_script_type_check_expr(t, &node->expr, &type);
    if (expr_result == -1) {
        return -1;
    }

    if (!expr_result && type) {
        const EcsType *type_ptr = ecs_get(t->v->world, type, EcsType);
        if (!type_ptr || (type_ptr->kind != EcsPrimitiveType &&
            type_ptr->kind != EcsEnumType && type_ptr->kind != EcsBitmaskType))
        {
            flecs_script_eval_error(t->v, node,
                "value of type %s cannot be used as while condition",
                flecs_errstr(ecs_get_path(t->v->world, type)));
            return -1;
        }
    }

    t->for_depth ++;
    int result = flecs_script_type_control_scope(t, node->scope);
    t->for_depth --;
    return result;
}

static bool flecs_script_type_is_number(
    const ecs_world_t *world,
    ecs_entity_t type)
{
    const EcsPrimitive *p = ecs_get(world, type, EcsPrimitive);
    return p && p->kind != EcsString && p->kind != EcsEntity &&
        p->kind != EcsId;
}

static int flecs_script_type_assign(
    ecs_script_type_visitor_t *t,
    ecs_script_assign_t *node)
{
    ecs_script_eval_visitor_t *v = t->v;
    if (!t->async_scope) {
        flecs_script_eval_error(v, node,
            "assignment is only allowed in async blocks");
        return -1;
    }

    ecs_script_template_t *template = v->template;
    ecs_script_var_t *var = ecs_script_vars_lookup(v->vars, node->name);
    bool qualified = !ecs_os_strncmp(node->name, "parent.", 7);
    ecs_entity_t component = 0;
    const ecs_member_t *m = NULL;
    node->parent = false;
    node->global = 0;
    if ((!var || qualified) && template && template->parent_type) {
        const char *name = qualified ? node->name + 7 : node->name;
        if (strchr(name, '.')) {
            flecs_script_eval_error(v, node,
                "assignment requires a mut variable name");
            return -1;
        }

        m = flecs_script_template_parent_member(
            v->world, template, name, &component);
        if (m && component != template->parent_type) {
            node->parent = true;
        } else {
            m = NULL;
        }
    }

    if (!node->parent && var && template) {
        ecs_script_template_member_t *members = ecs_vec_first(&template->members);
        int32_t count = ecs_vec_count(&template->members);
        for (int32_t i = 0; i < count; i ++) {
            if (members[i].sp == var->sp && members[i].is_mut) {
                component = template->muts.type;
                const EcsStruct *st = ecs_get(v->world, component, EcsStruct);
                m = ecs_vec_get_t(&st->members, ecs_member_t, members[i].index);
                break;
            }
        }
    }

    ecs_value_t global = {0};
    if (!m && !var) {
        ecs_expr_eval_desc_t desc = flecs_script_expr_desc(v, 0);
        flecs_script_symbol_t symbol;
        ecs_id_t global_component = 0;
        if (!flecs_script_symbol_lookup(&v->base.script->pub, &desc, 0,
            node->name, FlecsScriptLookupEntity, &symbol) &&
            symbol.kind == FlecsScriptSymbolGlobalVariable)
        {
            global = flecs_script_global_var_get(
                v->world, symbol.entity, &global_component);
        }

        if (global_component == ecs_id(EcsScriptMutVar)) {
            node->global = symbol.entity;
        }
    }

    if (!m && !node->global) {
        flecs_script_eval_error(v, node,
            "cannot assign to '%s': only mut variables of a template or "
            "exported mut variables can be assigned", node->name);
        return -1;
    }

    ecs_entity_t var_type = node->global ? global.type
        : node->parent ? m->type : var->value.type;
    int32_t sp = node->global ? -1
        : node->parent ? template->parent_sp : var->sp;
    ecs_entity_t type = var_type;
    int result = flecs_script_type_check_expr(t, &node->expr, &type);
    if (result) {
        if (result == 1) {
            node->node.skip = true;
            return 0;
        }

        return -1;
    }

    if (type != var_type && (!flecs_script_type_is_number(v->world, var_type) ||
        !flecs_script_type_is_number(v->world, type)))
    {
        flecs_script_eval_error(v, node,
            "expression of type %s is incompatible with type %s of "
            "variable '%s'",
            flecs_errstr(ecs_get_path(v->world, type)),
            flecs_errstr_1(ecs_get_path(v->world, var_type)), node->name);
        return -1;
    }

    type = var_type;

    node->eval_type = type;
    node->sp = sp;
    node->component = node->global ? ecs_id(EcsScriptMutVar) : component;
    node->offset = m ? m->offset : 0;
    return 0;
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

    if (t->async_scope) {
        switch(node->kind) {
        case EcsAstScope:
        case EcsAstConst:
        case EcsAstAwait:
        case EcsAstTry:
        case EcsAstIf:
        case EcsAstFor:
        case EcsAstWhile:
        case EcsAstContinue:
        case EcsAstAssign:
            break;
        default:
            flecs_script_eval_error(t->v, node,
                "%s statement is not allowed in async block",
                flecs_script_node_kind_str(node));
            return -1;
        }
    }

    switch (node->kind) {
    case EcsAstScope:
        return flecs_script_type_control_scope(
            t, (ecs_script_scope_t*)node);
    case EcsAstTag:
        return flecs_script_type_tag(t, (ecs_script_tag_t*)node);
    case EcsAstWithTag:
        return flecs_script_type_id(t, node, &((ecs_script_tag_t*)node)->id);
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
    case EcsAstAwait:
        return flecs_script_type_check_expr(t,
            &((ecs_script_await_t*)node)->expr, NULL);
    case EcsAstTry:
        return flecs_script_type_try(t, (ecs_script_try_t*)node);
    case EcsAstContinue:
        if (!t->for_depth) {
            flecs_script_eval_error(t->v, node,
                "continue is only allowed inside a loop");
            return -1;
        }
        return 0;
    case EcsAstAsync:
        return flecs_script_type_async(t, (ecs_script_async_t*)node);
    case EcsAstWhile:
        return flecs_script_type_while(t, (ecs_script_while_t*)node);
    case EcsAstAssign:
        return flecs_script_type_assign(t, (ecs_script_assign_t*)node);
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
    int32_t old_skipped_vars = ecs_vec_count(&t->skipped_vars);
    t->table = table;
    if (push_vars) {
        v->vars = flecs_script_vars_push(
            v->vars, &v->r->stack, v->r->allocator);
    }

    ecs_vec_clear(&scope->components);
    ecs_vec_clear(&scope->set_components);
    ecs_assert(v->base.depth < ECS_SCRIPT_VISIT_MAX_DEPTH,
        ECS_INTERNAL_ERROR, NULL);
    v->base.nodes[v->base.depth ++] = (ecs_script_node_t*)scope;

    int32_t i, count = ecs_vec_count(&scope->stmts);
    int result = 0;
    for (i = 0; i < count; i ++) {
        ecs_script_node_t **stmts = ecs_vec_first(&scope->stmts);
        ecs_script_node_t *node = stmts[i];
        v->base.prev = i ? stmts[i - 1] : NULL;
        v->base.next = i + 1 < count ? stmts[i + 1] : NULL;
        ecs_assert(v->base.depth < ECS_SCRIPT_VISIT_MAX_DEPTH,
            ECS_INTERNAL_ERROR, NULL);
        v->base.nodes[v->base.depth ++] = node;
        node->skip = false;
        result = flecs_script_type_node(t, node, allow_type) == -1 ? -1 : 0;
        v->base.depth --;
        if (result) {
            ecs_vec_clear(&scope->components);
            ecs_vec_clear(&scope->set_components);
            break;
        }

        if (t->template_scope) {
            ecs_script_id_t *id = NULL;
            bool w_expr = false;
            if (node->kind == EcsAstComponent) {
                ecs_script_component_t *component =
                    (ecs_script_component_t*)node;
                id = &component->id;
                w_expr = component->expr != NULL;
            } else if (node->kind == EcsAstTag) {
                id = &((ecs_script_tag_t*)node)->id;
            }

            if (id && id->eval && !id->interface && !id->dynamic) {
                ecs_vec_append_t(&v->base.script->allocator,
                    &scope->components, ecs_id_t)[0] = id->eval;
                if (w_expr) {
                    ecs_vec_append_t(&v->base.script->allocator,
                        &scope->set_components, ecs_id_t)[0] = id->eval;
                }
            }
        }
    }

    v->base.depth = old_depth;
    if (push_vars) {
        v->vars = ecs_script_vars_pop(v->vars);
    }

    ecs_vec_set_count_t(NULL, &t->skipped_vars, const char*, old_skipped_vars);
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
    flecs_script_type_init(&t);
    visitor.type_visitor = &t;

    int result = flecs_script_type_entity(&t, entity, false);
    if (flecs_script_type_report_unresolved(&visitor)) {
        result = -1;
    }

    visitor.type_visitor = NULL;
    flecs_script_type_fini(&t);
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
        v->r->allocator, &outer_using, ecs_entity_t);
    v->r->annot = ecs_vec_copy_t(
        v->r->allocator, &outer_annot, ecs_script_annot_t*);

    ecs_script_type_visitor_t t = {
        .v = v,
        .table = 0
    };
    flecs_script_type_init(&t);

    v->type_visitor = &t;
    int result = flecs_script_type_scope(
        &t, scope, 0, true, true);
    v->type_visitor = NULL;
    if (flecs_script_type_report_unresolved(v)) {
        result = -1;
    }

    flecs_script_type_fini(&t);
    ecs_vec_fini_t(v->r->allocator, &v->r->using, ecs_entity_t);
    ecs_vec_fini_t(v->r->allocator, &v->r->annot, ecs_script_annot_t*);
    v->r->using = outer_using;
    v->r->annot = outer_annot;
    v->parent = outer_parent;
    v->module = outer_module;
    return result;
}

#endif
