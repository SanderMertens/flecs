/**
 * @file addons/script/edit.c
 * @brief Source preserving script edits.
 */

#include "flecs.h"

#ifdef FLECS_SCRIPT

#include "script.h"

typedef enum flecs_script_edit_kind_t {
    FlecsScriptEditValue,
    FlecsScriptEditRemove,
    FlecsScriptEditDelete
} flecs_script_edit_kind_t;

typedef struct flecs_script_edit_t {
    ecs_entity_t entity;
    ecs_id_t component;
    flecs_script_edit_kind_t kind;
    int32_t offset;
    int32_t length;
    int32_t seq;
    char *text;
} flecs_script_edit_t;

struct ecs_script_edits_t {
    ecs_script_t *script;
    ecs_vec_t edits;
    int32_t seq;
};

typedef enum flecs_script_value_style_t {
    FlecsScriptValueOther,
    FlecsScriptValueNamed,
    FlecsScriptValuePositional,
    FlecsScriptValueEmpty
} flecs_script_value_style_t;

static bool flecs_script_edit_is_ident(
    char c)
{
    return isalpha((unsigned char)c) || isdigit((unsigned char)c) ||
        c == '_' || c == '.';
}

static const char* flecs_script_edit_skip_ws(
    const char *pos,
    const char *end)
{
    while (pos < end) {
        if (isspace((unsigned char)pos[0])) {
            pos ++;
        } else if (pos[0] == '/' && (pos + 1) < end && pos[1] == '/') {
            while (pos < end && pos[0] != '\n') {
                pos ++;
            }
        } else if (pos[0] == '/' && (pos + 1) < end && pos[1] == '*') {
            pos += 2;
            while (pos < end && !(pos[0] == '*' && (pos + 1) < end &&
                pos[1] == '/'))
            {
                pos ++;
            }
            if (pos < end) {
                pos += 2;
            }
        } else {
            break;
        }
    }

    return pos;
}

typedef struct flecs_script_edit_walk_t {
    int (*action)(ecs_script_entity_t *node, void *ctx);
    void *ctx;
} flecs_script_edit_walk_t;

static int flecs_script_edit_walk(
    ecs_script_scope_t *scope,
    void *ctx)
{
    flecs_script_edit_walk_t *walk = ctx;
    ecs_script_node_t **stmts = ecs_vec_first(&scope->stmts);
    for (int32_t i = 0; i < ecs_vec_count(&scope->stmts); i ++) {
        ecs_script_node_t *node = stmts[i];
        if (node->kind == EcsAstTemplate || node->kind == EcsAstFor ||
            node->kind == EcsAstFunction)
        {
            continue;
        }
        if (node->kind == EcsAstEntity) {
            int result = walk->action((ecs_script_entity_t*)node, walk->ctx);
            if (result) {
                return result;
            }
        }
        int result = flecs_script_visit_scopes(node, flecs_script_edit_walk, ctx);
        if (result) {
            return result;
        }
    }
    return 0;
}

static int flecs_script_edit_index_entity(
    ecs_script_entity_t *entity,
    void *ctx)
{
    ecs_script_impl_t *impl = ctx;
    if (entity->symbol >= 0 &&
        entity->symbol < ecs_vec_count(&impl->state.symbol_slots))
    {
        ecs_script_symbol_slot_t *slot = ecs_vec_get_t(
            &impl->state.symbol_slots, ecs_script_symbol_slot_t, entity->symbol);
        if (slot->entity && !ecs_map_get(&impl->entity_index, slot->entity)) {
            ecs_map_insert_ptr(&impl->entity_index, slot->entity, entity);
        }
    }
    return 0;
}

static bool flecs_script_edit_is_template_entity(
    const ecs_world_t *world,
    ecs_entity_t entity)
{
    if (!world || !ecs_is_alive(world, entity)) {
        return false;
    }

    return ecs_get_target(world, entity, EcsScriptTemplate, 0) != 0;
}

static ecs_script_entity_t* flecs_script_edit_find(
    ecs_script_impl_t *impl,
    ecs_entity_t entity)
{
    if (!entity) {
        return NULL;
    }

    if (flecs_script_edit_is_template_entity(impl->pub.world, entity)) {
        return NULL;
    }

    if (!impl->entity_index_valid || impl->entity_index_visit != impl->state.visit) {
        ecs_map_fini(&impl->entity_index);
        ecs_map_init(&impl->entity_index, NULL);
        flecs_script_edit_walk_t walk = {flecs_script_edit_index_entity, impl};
        flecs_script_edit_walk(impl->root, &walk);
        impl->entity_index_valid = true;
        impl->entity_index_visit = impl->state.visit;
    }

    return ecs_map_get_ptr(&impl->entity_index, entity);
}

void flecs_script_entity_index_fini(
    ecs_script_impl_t *impl)
{
    if (impl->entity_index_valid) {
        ecs_map_fini(&impl->entity_index);
        impl->entity_index_valid = false;
    }
}

typedef struct flecs_script_edit_symbol_t {
    int32_t symbol;
    ecs_script_entity_t *node;
} flecs_script_edit_symbol_t;

static int flecs_script_edit_find_symbol(
    ecs_script_entity_t *node,
    void *ctx)
{
    flecs_script_edit_symbol_t *symbol = ctx;
    if (node->symbol != symbol->symbol) {
        return 0;
    }
    symbol->node = node;
    return 1;
}

static ecs_script_entity_t* flecs_script_edit_find_in_instance(
    ecs_script_impl_t *impl,
    ecs_entity_t entity,
    ecs_entity_t instance,
    ecs_entity_t template_entity)
{
    ecs_world_t *world = impl->pub.world;

    const EcsScript *sc = ecs_get(world, template_entity, EcsScript);
    if (!sc || !sc->script || !sc->template_) {
        return NULL;
    }

    if (sc->script != &impl->pub) {
        return NULL;
    }

    const EcsScriptTemplateRoot *root = ecs_get_pair(
        world, instance, EcsScriptTemplateRoot, template_entity);
    if (!root) {
        return NULL;
    }

    ecs_script_template_t *template = sc->template_;
    const ecs_script_symbol_slot_t *slots = ecs_vec_first(&root->state.symbol_slots);
    int32_t i, count = ecs_vec_count(&root->state.symbol_slots);
    int32_t slot = -1;

    for (i = 0; i < count; i ++) {
        if (slots[i].entity == entity) {
            slot = i;
            break;
        }
    }

    if (slot == -1) {
        return NULL;
    }

    int32_t symbol = template->symbol_offset + slot;
    if (symbol == template->root_symbol) {
        return NULL;
    }

    if (!template->node || !template->node->scope) {
        return NULL;
    }

    flecs_script_edit_symbol_t find = {symbol, NULL};
    flecs_script_edit_walk_t walk = {flecs_script_edit_find_symbol, &find};
    flecs_script_edit_walk(template->node->scope, &walk);
    ecs_script_entity_t *node = find.node;
    if (!node) {
        return NULL;
    }

    if (node->name_expr) {
        return NULL;
    }

    if (!node->node.pos || !node->node.end) {
        return NULL;
    }

    return node;
}

static ecs_script_entity_t* flecs_script_edit_find_in_template(
    ecs_script_impl_t *impl,
    ecs_entity_t entity,
    ecs_entity_t *template_out)
{
    ecs_world_t *world = impl->pub.world;
    if (!world || !entity || !ecs_is_alive(world, entity)) {
        return NULL;
    }

    ecs_entity_t instance = ecs_get_target(world, entity, EcsChildOf, 0);

    while (instance) {
        int32_t i = 0;
        ecs_entity_t template_entity;

        while ((template_entity = ecs_get_target(
            world, instance, ecs_id(EcsScriptTemplateRoot), i ++)))
        {
            ecs_script_entity_t *node = flecs_script_edit_find_in_instance(
                impl, entity, instance, template_entity);
            if (node) {
                if (template_out) {
                    template_out[0] = template_entity;
                }
                return node;
            }
        }

        instance = ecs_get_target(world, instance, EcsChildOf, 0);
    }

    return NULL;
}

static ecs_script_entity_t* flecs_script_edit_find_w_template(
    ecs_script_impl_t *impl,
    ecs_entity_t entity,
    ecs_entity_t *template_out)
{
    if (template_out) {
        template_out[0] = 0;
    }

    ecs_script_entity_t *node = flecs_script_edit_find(impl, entity);
    if (node) {
        return node;
    }

    return flecs_script_edit_find_in_template(impl, entity, template_out);
}

bool ecs_script_entity_source(
    const ecs_script_t *script,
    ecs_entity_t entity,
    ecs_script_source_t *source)
{
    ecs_check(script != NULL, ECS_INVALID_PARAMETER, NULL);

    ecs_script_impl_t *impl = flecs_script_impl(
        ECS_CONST_CAST(ecs_script_t*, script));

    ecs_entity_t template_entity = 0;
    ecs_script_entity_t *node = flecs_script_edit_find_w_template(
        impl, entity, &template_entity);
    if (!node) {
        return false;
    }

    if (!node->node.pos || !node->node.end) {
        return false;
    }

    if (source) {
        source->offset = flecs_ito(int32_t, node->node.pos - script->code);
        source->length = flecs_ito(int32_t, node->node.end - node->node.pos);
        source->has_scope = node->scope != NULL && node->scope->open != NULL;
        source->template_ = template_entity;
        flecs_script_pos_to_line_col(script->code, node->node.pos,
            &source->line, &source->column);
    }

    return true;
error:
    return false;
}

ecs_entity_t ecs_script_entity_owner(
    const ecs_world_t *stage,
    ecs_entity_t entity)
{
    ecs_check(stage != NULL, ECS_INVALID_PARAMETER, NULL);

    const ecs_world_t *world = ecs_get_world(stage);
    if (!entity || !ecs_is_alive(world, entity)) {
        return 0;
    }

    ecs_entity_t template_entity;
    bool in_template_body = false;
    int32_t i = 0;

    while ((template_entity = ecs_get_target(
        world, entity, EcsScriptTemplate, i ++)))
    {
        in_template_body = true;

        const EcsScript *sc = ecs_get(world, template_entity, EcsScript);
        if (!sc || !sc->script) {
            continue;
        }

        ecs_entity_t owner = flecs_script_impl(sc->script)->entity;
        if (!owner) {
            continue;
        }

        if (ecs_script_entity_source(sc->script, entity, NULL)) {
            return owner;
        }
    }

    if (in_template_body) {
        return 0;
    }

    ecs_entity_t script_entity = ecs_get_target(
        world, entity, ecs_id(EcsScript), 0);
    if (script_entity) {
        const EcsScript *sc = ecs_get(world, script_entity, EcsScript);
        if (sc && sc->script && !sc->template_ &&
            ecs_script_entity_source(sc->script, entity, NULL))
        {
            return script_entity;
        }
    }

    ecs_entity_t instance = ecs_get_target(world, entity, EcsChildOf, 0);

    while (instance) {
        i = 0;

        while ((template_entity = ecs_get_target(
            world, instance, ecs_id(EcsScriptTemplateRoot), i ++)))
        {
            const EcsScript *sc = ecs_get(world, template_entity, EcsScript);
            if (!sc || !sc->script) {
                continue;
            }

            ecs_entity_t owner = flecs_script_impl(sc->script)->entity;
            if (!owner) {
                continue;
            }

            if (ecs_script_entity_source(sc->script, entity, NULL)) {
                return owner;
            }
        }

        instance = ecs_get_target(world, instance, EcsChildOf, 0);
    }

    return 0;
error:
    return 0;
}

static void flecs_script_edit_using_scopes(
    ecs_script_impl_t *impl,
    ecs_vec_t *scopes)
{
    ecs_world_t *world = impl->pub.world;
    ecs_script_node_t **stmts = ecs_vec_first(&impl->root->stmts);
    int32_t i, count = ecs_vec_count(&impl->root->stmts);

    for (i = 0; i < count; i ++) {
        if (stmts[i]->kind == EcsAstModule) {
            ecs_script_module_t *m = (ecs_script_module_t*)stmts[i];
            if (m->eval) {
                ecs_vec_append_t(NULL, scopes, ecs_entity_t)[0] = m->eval;
            }
        } else if (stmts[i]->kind == EcsAstUsing) {
            ecs_script_using_t *u = (ecs_script_using_t*)stmts[i];
            if (!u->eval) {
                continue;
            }

            int32_t len = ecs_os_strlen(u->name);
            bool wildcard = len > 2 && !ecs_os_strcmp(&u->name[len - 2], ".*");

            ecs_vec_append_t(NULL, scopes, ecs_entity_t)[0] = u->eval;

            if (wildcard) {
                ecs_iter_t it = ecs_children(world, u->eval);
                while (ecs_children_next(&it)) {
                    int32_t e;
                    for (e = 0; e < it.count; e ++) {
                        ecs_vec_append_t(NULL, scopes, ecs_entity_t)[0] =
                            it.entities[e];
                    }
                }
            }
        }
    }
}

static ecs_entity_t flecs_script_edit_lookup(
    ecs_world_t *world,
    const ecs_vec_t *scopes,
    const char *name)
{
    ecs_entity_t result = ecs_lookup_path_w_sep(
        world, 0, name, ".", NULL, false);
    if (result) {
        return result;
    }

    ecs_entity_t *elems = ecs_vec_first(scopes);
    int32_t i, count = ecs_vec_count(scopes);
    for (i = 0; i < count; i ++) {
        result = ecs_lookup_path_w_sep(
            world, elems[i], name, ".", NULL, false);
        if (result) {
            return result;
        }
    }

    return 0;
}

static char* flecs_script_edit_entity_name(
    ecs_script_impl_t *impl,
    const ecs_vec_t *scopes,
    ecs_entity_t entity)
{
    ecs_world_t *world = impl->pub.world;

    char *best = ecs_get_path_w_sep(world, 0, entity, ".", NULL);
    if (!best) {
        return NULL;
    }

    ecs_size_t best_len = ecs_os_strlen(best);

    ecs_entity_t *elems = ecs_vec_first(scopes);
    int32_t i, count = ecs_vec_count(scopes);
    for (i = 0; i < count; i ++) {
        if (!elems[i] || elems[i] == entity) {
            continue;
        }

        char *rel = ecs_get_path_w_sep(world, elems[i], entity, ".", NULL);
        if (!rel) {
            continue;
        }

        ecs_size_t rel_len = ecs_os_strlen(rel);
        if (!rel_len || rel_len >= best_len ||
            flecs_script_edit_lookup(world, scopes, rel) != entity)
        {
            ecs_os_free(rel);
            continue;
        }

        ecs_os_free(best);
        best = rel;
        best_len = rel_len;
    }

    return best;
}

static char* flecs_script_edit_id_str(
    ecs_script_impl_t *impl,
    const ecs_vec_t *scopes,
    ecs_id_t id)
{
    ecs_world_t *world = impl->pub.world;

    if (ECS_IS_PAIR(id)) {
        char *first = flecs_script_edit_entity_name(
            impl, scopes, ecs_pair_first(world, id));
        char *second = flecs_script_edit_entity_name(
            impl, scopes, ecs_pair_second(world, id));
        if (!first || !second) {
            ecs_os_free(first);
            ecs_os_free(second);
            return NULL;
        }

        ecs_strbuf_t buf = ECS_STRBUF_INIT;
        ecs_strbuf_appendlit(&buf, "(");
        ecs_strbuf_appendstr(&buf, first);
        ecs_strbuf_appendlit(&buf, ", ");
        ecs_strbuf_appendstr(&buf, second);
        ecs_strbuf_appendlit(&buf, ")");
        ecs_os_free(first);
        ecs_os_free(second);
        return ecs_strbuf_get(&buf);
    }

    if (id & ECS_ID_FLAGS_MASK) {
        return NULL;
    }

    return flecs_script_edit_entity_name(impl, scopes, id);
}

static flecs_script_value_style_t flecs_script_edit_value_style(
    const char *pos,
    const char *end)
{
    if (pos >= end) {
        return FlecsScriptValueOther;
    }

    char open = pos[0];
    if (open != '{' && open != '(') {
        return FlecsScriptValueOther;
    }

    char close = (open == '{') ? '}' : ')';

    const char *cur = flecs_script_edit_skip_ws(pos + 1, end);
    if (cur >= end || cur[0] == close) {
        return FlecsScriptValueEmpty;
    }

    if (!isalpha((unsigned char)cur[0]) && cur[0] != '_') {
        return FlecsScriptValuePositional;
    }

    while (cur < end && flecs_script_edit_is_ident(cur[0])) {
        cur ++;
    }

    cur = flecs_script_edit_skip_ws(cur, end);

    if (cur < end && cur[0] == ':' && ((cur + 1) >= end || cur[1] != ':')) {
        return FlecsScriptValueNamed;
    }

    return FlecsScriptValuePositional;
}

static char* flecs_script_edit_value_str(
    ecs_world_t *world,
    ecs_id_t component,
    const void *value,
    flecs_script_value_style_t style,
    bool parens)
{
    ecs_entity_t type = ecs_get_typeid(world, component);
    if (!type) {
        char *idstr = ecs_id_str(world, component);
        ecs_err("cannot serialize value for '%s': not a component", idstr);
        ecs_os_free(idstr);
        return NULL;
    }

    bool positional = style == FlecsScriptValuePositional ||
        style == FlecsScriptValueEmpty;
    char *expr = flecs_script_ptr_to_expr_precise(world, type, value, positional);
    if (!expr) {
        return NULL;
    }

    if (parens) {
        ecs_size_t len = ecs_os_strlen(expr);
        if (len >= 2 && expr[0] == '{' && expr[len - 1] == '}') {
            expr[0] = '(';
            expr[len - 1] = ')';
        }
    }

    return expr;
}

static ecs_script_node_t* flecs_script_edit_find_stmt(
    ecs_script_entity_t *entity,
    ecs_id_t component)
{
    ecs_script_scope_t *scope = entity->scope;
    if (!scope) {
        return NULL;
    }

    ecs_script_node_t **stmts = ecs_vec_first(&scope->stmts);
    int32_t i, count = ecs_vec_count(&scope->stmts);
    ecs_script_node_t *result = NULL;

    for (i = 0; i < count; i ++) {
        ecs_script_node_t *node = stmts[i];
        if (node->kind == EcsAstComponent) {
            if (((ecs_script_component_t*)node)->id.eval == component) {
                result = node;
            }
        } else if (node->kind == EcsAstTag) {
            if (((ecs_script_tag_t*)node)->id.eval == component) {
                result = node;
            }
        }
    }

    return result;
}

static const char* flecs_script_edit_line_start(
    const char *code,
    const char *pos)
{
    while (pos > code && pos[-1] != '\n') {
        pos --;
    }
    return pos;
}

static void flecs_script_edit_indent(
    const char *code,
    const char *pos,
    ecs_strbuf_t *buf)
{
    const char *start = flecs_script_edit_line_start(code, pos);
    const char *cur = start;
    while (cur < pos && (cur[0] == ' ' || cur[0] == '\t')) {
        cur ++;
    }
    ecs_strbuf_appendstrn(buf, start, flecs_ito(int32_t, cur - start));
}

static char* flecs_script_edit_indent_str(
    const char *code,
    const char *pos)
{
    ecs_strbuf_t buf = ECS_STRBUF_INIT;
    flecs_script_edit_indent(code, pos, &buf);
    char *result = ecs_strbuf_get(&buf);
    if (!result) {
        result = ecs_os_strdup("");
    }
    return result;
}

static char* flecs_script_edit_child_indent(
    const char *indent)
{
    ecs_strbuf_t buf = ECS_STRBUF_INIT;
    ecs_strbuf_appendstr(&buf, indent);
    if (strchr(indent, 0x09)) {
        ecs_strbuf_appendlit(&buf, "\t");
    } else {
        ecs_strbuf_appendlit(&buf, "    ");
    }
    return ecs_strbuf_get(&buf);
}

static void flecs_script_edit_line_span(
    const char *code,
    const char *start,
    const char *end,
    int32_t *offset_out,
    int32_t *length_out)
{
    const char *begin = flecs_script_edit_line_start(code, start);
    const char *cur = begin;
    while (cur < start && (cur[0] == ' ' || cur[0] == '\t')) {
        cur ++;
    }

    if (cur != start) {
        begin = start;
    }

    const char *finish = end;
    while (finish[0] == ' ' || finish[0] == '\t' || finish[0] == '\r') {
        finish ++;
    }

    if (finish[0] == ';') {
        finish ++;
        while (finish[0] == ' ' || finish[0] == '\t' || finish[0] == '\r') {
            finish ++;
        }
    }

    if (finish[0] == '/' && finish[1] == '/') {
        while (finish[0] && finish[0] != '\n') {
            finish ++;
        }
    }

    if (finish[0] == '\n') {
        finish ++;
    } else if (finish[0]) {
        finish = end;
    }

    *offset_out = flecs_ito(int32_t, begin - code);
    *length_out = flecs_ito(int32_t, finish - begin);
}

static void flecs_script_edit_collapse_blank(
    const char *code,
    int32_t *offset_out,
    int32_t *length_out)
{
    const char *begin = &code[*offset_out];
    const char *finish = begin + *length_out;

    bool blank_before = false;
    {
        const char *cur = begin;
        int32_t newlines = 0;

        while (cur > code) {
            char c = cur[-1];
            if (c == '\n') {
                newlines ++;
                if (newlines >= 2) {
                    break;
                }
            } else if (c != ' ' && c != '\t' && c != '\r') {
                break;
            }
            cur --;
        }

        if (cur == code || newlines >= 2 || cur[-1] == '{') {
            blank_before = true;
        }
    }

    if (!blank_before) {
        return;
    }

    bool blank_after = false;
    int32_t newlines_after = 0;
    {
        const char *cur = finish;

        while (cur[0]) {
            char c = cur[0];
            if (c == '\n') {
                newlines_after ++;
            } else if (c != ' ' && c != '\t' && c != '\r') {
                break;
            }
            cur ++;
        }

        if (!cur[0] || newlines_after >= 1 || cur[0] == '}') {
            blank_after = true;
        }
    }

    if (!blank_after) {
        return;
    }

    if (newlines_after >= 1) {
        const char *cur = finish;
        while (cur[0] == ' ' || cur[0] == '\t' || cur[0] == '\r') {
            cur ++;
        }
        if (cur[0] == '\n') {
            *length_out += flecs_ito(int32_t, (cur + 1) - finish);
        }
        return;
    }

    if (begin > code && begin[-1] == '\n') {
        const char *cur = begin - 1;
        while (cur > code &&
            (cur[-1] == ' ' || cur[-1] == '\t' || cur[-1] == '\r'))
        {
            cur --;
        }

        if (cur == code || cur[-1] == '\n') {
            int32_t delta = flecs_ito(int32_t, begin - cur);
            *offset_out -= delta;
            *length_out += delta;
        }
    }
}

ecs_script_edits_t* ecs_script_edits_new(
    ecs_script_t *script)
{
    ecs_check(script != NULL, ECS_INVALID_PARAMETER, NULL);

    ecs_script_edits_t *result = ecs_os_calloc_t(ecs_script_edits_t);
    result->script = script;
    ecs_vec_init_t(NULL, &result->edits, flecs_script_edit_t, 0);
    return result;
error:
    return NULL;
}

void ecs_script_edits_free(
    ecs_script_edits_t *edits)
{
    if (!edits) {
        return;
    }

    flecs_script_edit_t *elems = ecs_vec_first(&edits->edits);
    int32_t i, count = ecs_vec_count(&edits->edits);
    for (i = 0; i < count; i ++) {
        ecs_os_free(elems[i].text);
    }

    ecs_vec_fini_t(NULL, &edits->edits, flecs_script_edit_t);
    ecs_os_free(edits);
}

static void flecs_script_edit_add(
    ecs_script_edits_t *edits,
    ecs_entity_t entity,
    ecs_id_t component,
    flecs_script_edit_kind_t kind,
    int32_t offset,
    int32_t length,
    char *text)
{
    flecs_script_edit_t *elems = ecs_vec_first(&edits->edits);
    int32_t i, count = ecs_vec_count(&edits->edits);

    for (i = 0; i < count; i ++) {
        if (elems[i].entity == entity && elems[i].component == component) {
            ecs_os_free(elems[i].text);
            elems[i].kind = kind;
            elems[i].offset = offset;
            elems[i].length = length;
            elems[i].text = text;
            return;
        }
    }

    flecs_script_edit_t *elem = ecs_vec_append_t(
        NULL, &edits->edits, flecs_script_edit_t);
    elem->entity = entity;
    elem->component = component;
    elem->kind = kind;
    elem->offset = offset;
    elem->length = length;
    elem->seq = edits->seq ++;
    elem->text = text;
}

static ecs_script_entity_t* flecs_script_edit_entity(
    ecs_script_edits_t *edits,
    ecs_entity_t entity)
{
    ecs_script_impl_t *impl = flecs_script_impl(edits->script);
    ecs_script_entity_t *node = flecs_script_edit_find_w_template(
        impl, entity, NULL);
    if (!node || !node->node.pos || !node->node.end) {
        char *path = ecs_get_path(edits->script->world, entity);
        ecs_err("entity '%s' is not declared by an editable statement in "
            "script '%s'", path ? path : "?", edits->script->name);
        ecs_os_free(path);
        return NULL;
    }

    return node;
}

static int flecs_script_edit_set_text(
    ecs_script_edits_t *edits,
    ecs_entity_t entity,
    ecs_id_t component,
    const void *value,
    const char *expr)
{
    ecs_check(edits != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(component != 0, ECS_INVALID_PARAMETER, NULL);

    ecs_script_t *script = edits->script;
    ecs_script_impl_t *impl = flecs_script_impl(script);
    const char *code = script->code;

    ecs_script_entity_t *node = flecs_script_edit_entity(edits, entity);
    if (!node) {
        return -1;
    }

    ecs_script_node_t *stmt = flecs_script_edit_find_stmt(node, component);

    if (stmt && stmt->kind == EcsAstComponent) {
        ecs_script_component_t *comp = (ecs_script_component_t*)stmt;
        if (comp->value_pos && comp->value_end &&
            comp->value_pos >= code && comp->value_end > comp->value_pos)
        {
            const char *value_pos = comp->value_pos;
            const char *value_end = comp->value_end;

            while (value_pos < value_end &&
                isspace((unsigned char)value_pos[0]))
            {
                value_pos ++;
            }

            char *text;
            if (expr) {
                text = ecs_os_strdup(expr);
            } else {
                flecs_script_value_style_t style =
                    flecs_script_edit_value_style(value_pos, value_end);
                text = flecs_script_edit_value_str(script->world, component,
                    value, style, value_pos[0] == '(');
            }

            if (!text) {
                return -1;
            }

            flecs_script_edit_add(edits, entity, component,
                FlecsScriptEditValue,
                flecs_ito(int32_t, value_pos - code),
                flecs_ito(int32_t, value_end - value_pos),
                text);

            return 0;
        }
    }

    ecs_vec_t scopes;
    ecs_vec_init_t(NULL, &scopes, ecs_entity_t, 0);
    flecs_script_edit_using_scopes(impl, &scopes);
    char *id_str = flecs_script_edit_id_str(impl, &scopes, component);
    ecs_vec_fini_t(NULL, &scopes, ecs_entity_t);

    if (!id_str) {
        char *idstr = ecs_id_str(script->world, component);
        ecs_err("cannot write component '%s' to script '%s'",
            idstr ? idstr : "?", script->name);
        ecs_os_free(idstr);
        return -1;
    }

    char *value_str;
    if (expr) {
        value_str = ecs_os_strdup(expr);
    } else {
        value_str = flecs_script_edit_value_str(script->world, component,
            value, FlecsScriptValueNamed, false);
    }

    if (!value_str) {
        ecs_os_free(id_str);
        return -1;
    }

    if (stmt && stmt->kind == EcsAstTag && stmt->pos && stmt->end) {
        ecs_strbuf_t tag_buf = ECS_STRBUF_INIT;
        ecs_strbuf_appendstr(&tag_buf, id_str);
        ecs_strbuf_appendlit(&tag_buf, ": ");
        ecs_strbuf_appendstr(&tag_buf, value_str);

        flecs_script_edit_add(edits, entity, component, FlecsScriptEditValue,
            flecs_ito(int32_t, stmt->pos - code),
            flecs_ito(int32_t, stmt->end - stmt->pos),
            ecs_strbuf_get(&tag_buf));

        ecs_os_free(id_str);
        ecs_os_free(value_str);

        return 0;
    }

    char *indent = flecs_script_edit_indent_str(code, node->node.pos);
    char *child_indent = flecs_script_edit_child_indent(indent);

    ecs_script_scope_t *scope = node->scope;
    ecs_strbuf_t buf = ECS_STRBUF_INIT;
    int32_t offset;

    if (scope && scope->open && scope->node.end) {
        const char *close = scope->node.end - 1;
        const char *anchor = scope->open + 1;
        const char *last_indent = child_indent;
        char *stmt_indent = NULL;

        {
            ecs_script_node_t **stmts = ecs_vec_first(&scope->stmts);
            int32_t i, count = ecs_vec_count(&scope->stmts);
            for (i = 0; i < count; i ++) {
                if (stmts[i]->end && stmts[i]->end > anchor &&
                    stmts[i]->end <= close)
                {
                    anchor = stmts[i]->end;
                    stmt_indent = flecs_script_edit_indent_str(
                        code, stmts[i]->pos);
                }
            }
        }

        if (stmt_indent) {
            last_indent = stmt_indent;
        }

        const char *newline = NULL;
        const char *cur;
        for (cur = anchor; cur < close; cur ++) {
            if (cur[0] == '\n') {
                newline = cur;
            }
        }

        if (newline) {
            ecs_strbuf_appendstr(&buf, last_indent);
            ecs_strbuf_appendstr(&buf, id_str);
            ecs_strbuf_appendlit(&buf, ": ");
            ecs_strbuf_appendstr(&buf, value_str);
            ecs_strbuf_appendlit(&buf, "\n");
            offset = flecs_ito(int32_t, (newline + 1) - code);
        } else {
            ecs_strbuf_appendlit(&buf, "\n");
            ecs_strbuf_appendstr(&buf, last_indent);
            ecs_strbuf_appendstr(&buf, id_str);
            ecs_strbuf_appendlit(&buf, ": ");
            ecs_strbuf_appendstr(&buf, value_str);
            ecs_strbuf_appendlit(&buf, "\n");
            ecs_strbuf_appendstr(&buf, indent);
            offset = flecs_ito(int32_t, close - code);
        }

        ecs_os_free(stmt_indent);
    } else {
        ecs_strbuf_appendlit(&buf, " {\n");
        ecs_strbuf_appendstr(&buf, child_indent);
        ecs_strbuf_appendstr(&buf, id_str);
        ecs_strbuf_appendlit(&buf, ": ");
        ecs_strbuf_appendstr(&buf, value_str);
        ecs_strbuf_appendlit(&buf, "\n");
        ecs_strbuf_appendstr(&buf, indent);
        ecs_strbuf_appendlit(&buf, "}");
        offset = flecs_ito(int32_t, node->node.end - code);
    }

    ecs_os_free(id_str);
    ecs_os_free(value_str);
    ecs_os_free(indent);
    ecs_os_free(child_indent);

    flecs_script_edit_add(edits, entity, component, FlecsScriptEditValue,
        offset, 0, ecs_strbuf_get(&buf));

    return 0;
error:
    return -1;
}

int ecs_script_edits_set(
    ecs_script_edits_t *edits,
    ecs_entity_t entity,
    ecs_id_t component,
    const void *value)
{
    ecs_check(value != NULL, ECS_INVALID_PARAMETER, NULL);
    return flecs_script_edit_set_text(edits, entity, component, value, NULL);
error:
    return -1;
}

int ecs_script_edits_set_expr(
    ecs_script_edits_t *edits,
    ecs_entity_t entity,
    ecs_id_t component,
    const char *expr)
{
    ecs_check(expr != NULL, ECS_INVALID_PARAMETER, NULL);
    return flecs_script_edit_set_text(edits, entity, component, NULL, expr);
error:
    return -1;
}

int ecs_script_edits_remove(
    ecs_script_edits_t *edits,
    ecs_entity_t entity,
    ecs_id_t component)
{
    ecs_check(edits != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(component != 0, ECS_INVALID_PARAMETER, NULL);

    ecs_script_entity_t *node = flecs_script_edit_entity(edits, entity);
    if (!node) {
        return -1;
    }

    ecs_script_node_t *stmt = flecs_script_edit_find_stmt(node, component);
    if (!stmt) {
        return 0;
    }

    if (!stmt->pos || !stmt->end) {
        char *idstr = ecs_id_str(edits->script->world, component);
        ecs_err("cannot remove '%s': component is part of the entity statement",
            idstr ? idstr : "?");
        ecs_os_free(idstr);
        return -1;
    }

    int32_t offset, length;
    flecs_script_edit_line_span(edits->script->code, stmt->pos, stmt->end,
        &offset, &length);

    flecs_script_edit_add(edits, entity, component, FlecsScriptEditRemove,
        offset, length, ecs_os_strdup(""));

    return 0;
error:
    return -1;
}

int ecs_script_edits_delete(
    ecs_script_edits_t *edits,
    ecs_entity_t entity)
{
    ecs_check(edits != NULL, ECS_INVALID_PARAMETER, NULL);

    ecs_script_entity_t *node = flecs_script_edit_entity(edits, entity);
    if (!node) {
        return -1;
    }

    int32_t offset, length;
    flecs_script_edit_line_span(edits->script->code, node->node.pos,
        node->node.end, &offset, &length);
    flecs_script_edit_collapse_blank(edits->script->code, &offset, &length);

    flecs_script_edit_add(edits, entity, 0, FlecsScriptEditDelete,
        offset, length, ecs_os_strdup(""));

    return 0;
error:
    return -1;
}

int ecs_script_edits_clear(
    ecs_script_edits_t *edits,
    ecs_entity_t entity,
    ecs_id_t component)
{
    ecs_check(edits != NULL, ECS_INVALID_PARAMETER, NULL);

    flecs_script_edit_t *elems = ecs_vec_first(&edits->edits);
    int32_t i, count = ecs_vec_count(&edits->edits);

    for (i = 0; i < count; i ++) {
        if (elems[i].entity == entity && elems[i].component == component) {
            ecs_os_free(elems[i].text);
            ecs_vec_remove_t(&edits->edits, flecs_script_edit_t, i);
            return 0;
        }
    }

    return -1;
error:
    return -1;
}

int32_t ecs_script_edits_count(
    const ecs_script_edits_t *edits)
{
    ecs_check(edits != NULL, ECS_INVALID_PARAMETER, NULL);

    return ecs_vec_count(&edits->edits);
error:
    return 0;
}

static int flecs_script_edit_compare(
    const void *a_ptr,
    const void *b_ptr)
{
    const flecs_script_edit_t *a = a_ptr;
    const flecs_script_edit_t *b = b_ptr;

    if (a->offset != b->offset) {
        return a->offset < b->offset ? -1 : 1;
    }

    if (a->kind != b->kind) {
        if (a->kind == FlecsScriptEditDelete) {
            return -1;
        }
        if (b->kind == FlecsScriptEditDelete) {
            return 1;
        }
    }

    if (a->length != b->length) {
        return a->length > b->length ? -1 : 1;
    }

    return a->seq < b->seq ? -1 : (a->seq > b->seq);
}

char* ecs_script_edits_apply(
    const ecs_script_edits_t *edits)
{
    ecs_check(edits != NULL, ECS_INVALID_PARAMETER, NULL);

    const char *code = edits->script->code;
    if (!code) {
        code = "";
    }

    int32_t count = ecs_vec_count(&edits->edits);
    if (!count) {
        return ecs_os_strdup(code);
    }

    flecs_script_edit_t *sorted = ecs_os_malloc_n(flecs_script_edit_t, count);
    ecs_os_memcpy_n(sorted, ecs_vec_first(&edits->edits),
        flecs_script_edit_t, count);
    qsort(sorted, flecs_itosize(count),
        sizeof(flecs_script_edit_t), flecs_script_edit_compare);

    ecs_strbuf_t buf = ECS_STRBUF_INIT;
    int32_t i, cursor = 0, delete_start = 0, delete_end = 0;

    for (i = 0; i < count; i ++) {
        flecs_script_edit_t *edit = &sorted[i];

        if (edit->kind == FlecsScriptEditDelete && edit->offset >= cursor) {
            delete_start = edit->offset;
            delete_end = edit->offset + edit->length;
        }

        if (edit->offset < cursor) {
            if (edit->offset >= delete_start &&
                (edit->offset + edit->length) <= delete_end)
            {
                continue;
            }

            ecs_err("conflicting edits for script '%s'", edits->script->name);
            ecs_os_free(sorted);
            ecs_strbuf_reset(&buf);
            return NULL;
        }

        ecs_strbuf_appendstrn(&buf, &code[cursor], edit->offset - cursor);
        if (edit->text && edit->text[0]) {
            ecs_strbuf_appendstr(&buf, edit->text);
        }

        cursor = edit->offset + edit->length;
    }

    ecs_strbuf_appendstr(&buf, &code[cursor]);

    ecs_os_free(sorted);

    char *result = ecs_strbuf_get(&buf);
    if (!result) {
        result = ecs_os_strdup("");
    }

    return result;
error:
    return NULL;
}

#endif
