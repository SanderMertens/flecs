/**
 * @file addons/script/modules/command.c
 * @brief Command sessions, parsing and running commands.
 */

#include "flecs.h"

#ifdef FLECS_SCRIPT_SHELL
#include "command.h"

ECS_COMPONENT_DECLARE(EcsScriptShell);
ECS_COMPONENT_DECLARE(EcsCommandSession);
ECS_COMPONENT_DECLARE(ecs_command_param_info_t);
ECS_COMPONENT_DECLARE(ecs_command_info_t);
ECS_COMPONENT_DECLARE(ecs_command_tree_node_t);
ECS_COMPONENT_DECLARE(ecs_command_component_value_t);
ECS_COMPONENT_DECLARE(ecs_command_entity_info_t);
ECS_COMPONENT_DECLARE(ecs_command_doc_info_t);
ECS_COMPONENT_DECLARE(ecs_command_query_row_t);
ECS_COMPONENT_DECLARE(ecs_command_grep_match_t);
ECS_COMPONENT_DECLARE(ecs_command_script_info_t);
ECS_COMPONENT_DECLARE(ecs_command_script_line_t);
ECS_COMPONENT_DECLARE(ecs_command_script_match_t);
ECS_COMPONENT_DECLARE(ecs_command_script_location_t);
ECS_COMPONENT_DECLARE(ecs_command_script_error_t);
#ifdef FLECS_SCRIPT_INPUT
ECS_COMPONENT_DECLARE(ecs_command_pointer_state_t);
ECS_COMPONENT_DECLARE(ecs_script_event_result_t);
#endif

typedef struct EcsCommandState {
    ecs_entity_t session;
} EcsCommandState;

static ECS_COMPONENT_DECLARE(EcsCommandState);

static void flecs_command_strings_fini(
    ecs_vec_t *strings)
{
    char **array = ecs_vec_first(strings);
    int32_t i, count = ecs_vec_count(strings);
    for (i = 0; i < count; i ++) {
        ecs_os_free(array[i]);
    }

    ecs_vec_fini_t(NULL, strings, char*);
}

static ECS_COPY(EcsScriptShell, dst, src, {
    ecs_vec_fini_t(NULL, &dst->command_paths, ecs_entity_t);
    dst->command_paths = ecs_vec_copy_t(
        NULL, &src->command_paths, ecs_entity_t);
})

static ECS_MOVE(EcsScriptShell, dst, src, {
    ecs_vec_fini_t(NULL, &dst->command_paths, ecs_entity_t);
    *dst = *src;
    ecs_os_zeromem(src);
})

static ECS_DTOR(EcsScriptShell, ptr, {
    ecs_vec_fini_t(NULL, &ptr->command_paths, ecs_entity_t);
})

static ECS_MOVE(EcsCommandSession, dst, src, {
    flecs_command_strings_fini(&dst->history);
    flecs_command_strings_fini(&dst->results);
    *dst = *src;
    ecs_os_zeromem(src);
})

static ECS_DTOR(EcsCommandSession, ptr, {
    flecs_command_strings_fini(&ptr->history);
    flecs_command_strings_fini(&ptr->results);
})

ecs_entity_t ecs_command_get_session(
    const ecs_world_t *world)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    world = ecs_get_world(world);
    if (!ecs_id(EcsCommandState)) {
        return 0;
    }

    const EcsCommandState *state = ecs_singleton_get(world, EcsCommandState);
    if (!state) {
        return 0;
    }

    return state->session;
error:
    return 0;
}

ecs_entity_t flecs_command_scope(
    const ecs_world_t *world)
{
    ecs_entity_t session = ecs_command_get_session(world);
    if (!session) {
        return 0;
    }

    const EcsCommandSession *s = ecs_get(world, session, EcsCommandSession);
    if (!s || !s->scope || !ecs_is_alive(world, s->scope)) {
        return 0;
    }

    return s->scope;
}

ecs_entity_t flecs_command_resolve(
    const ecs_world_t *world,
    const char *name)
{
    const EcsScriptShell *shell = ecs_singleton_get(world, EcsScriptShell);
    if (!shell) {
        return 0;
    }

    const ecs_entity_t *paths = ecs_vec_first(&shell->command_paths);
    int32_t i, count = ecs_vec_count(&shell->command_paths);
    for (i = 0; i < count; i ++) {
        if (!paths[i] || !ecs_is_alive(world, paths[i])) {
            continue;
        }

        ecs_entity_t function = ecs_lookup_child(world, paths[i], name);
        if (function && ecs_has(world, function, EcsScriptFunction)) {
            return function;
        }
    }

    return 0;
}

ecs_entity_t flecs_command_lookup(
    const ecs_world_t *world,
    const char *name)
{
    ecs_entity_t scope = flecs_command_scope(world);
    ecs_entity_t result = 0;
    if (scope) {
        result = ecs_lookup_path_w_sep(world, scope, name, ".", NULL, true);
    }

    if (!result) {
        result = ecs_lookup(world, name);
    }

    return result;
}

static ecs_entity_t flecs_command_lookup_action(
    const ecs_world_t *world,
    const char *name,
    void *ctx)
{
    (void)ctx;
    return flecs_command_lookup(world, name);
}

int flecs_command_parse_value(
    ecs_world_t *world,
    const char *expr,
    ecs_value_t *value,
    bool partial)
{
    ecs_expr_eval_desc_t desc = {
        .name = "command",
        .type = value->type,
        .lookup_action = flecs_command_lookup_action,
        .partial = partial
    };

    const char *ptr = ecs_expr_run(world, expr, value, &desc);
    if (!ptr) {
        return -1;
    }

    while (isspace(ptr[0])) {
        ptr ++;
    }

    if (ptr[0]) {
        ecs_err("unexpected '%s' after value '%s'", ptr, expr);
        return -1;
    }

    return 0;
}

void flecs_command_fail(
    const ecs_function_ctx_t *ctx)
{
    flecs_script_runtime_get(ctx->world)->error = true;
}

bool flecs_command_check_alive(
    const ecs_function_ctx_t *ctx,
    ecs_entity_t entity)
{
    if (!entity || !ecs_is_alive(ctx->world, entity)) {
        flecs_command_err(ctx, "entity #%u is not alive", (uint32_t)entity);
        return false;
    }

    return true;
}

ecs_entity_t flecs_command_value_type(
    const ecs_world_t *world,
    ecs_id_t id)
{
    ecs_entity_t type = ecs_get_typeid(world, id);
    const ecs_type_info_t *ti = type ? ecs_get_type_info(world, type) : NULL;
    if (!ti || (ti->hooks.flags & ECS_TYPE_HOOK_COPY_ILLEGAL) ||
        !ecs_has(world, type, EcsTypeSerializer))
    {
        return 0;
    }

    return type;
}

int flecs_command_parse_component(
    const ecs_function_ctx_t *ctx,
    ecs_entity_t e,
    ecs_id_t id,
    const char *expr,
    ecs_value_t *value)
{
    ecs_world_t *world = ctx->world;
    ecs_entity_t type = flecs_command_value_type(world, id);
    if (!type) {
        flecs_command_err(ctx, "cannot assign a value to '%s'",
            flecs_errstr(ecs_id_str(world, id)));
        return -1;
    }

    *value = ecs_value_new(world, type);
    const void *cur = ecs_get_id(world, e, id);
    if (cur) {
        ecs_ptr_copy(world, type, value->ptr, cur);
    }

    if (flecs_command_parse_value(world, expr ? expr : "", value, true)) {
        ecs_value_fini(world, value);
        flecs_command_fail(ctx);
        return -1;
    }

    return 0;
}

void flecs_command_children(
    ecs_world_t *world,
    ecs_entity_t parent,
    ecs_vec_t *children)
{
    ecs_iter_t it = ecs_children(world, parent);
    while (ecs_children_next(&it)) {
        int32_t i;
        for (i = 0; i < it.count; i ++) {
            ecs_entity_t e = it.entities[i];
            if (!parent && ecs_get_parent(world, e)) {
                continue;
            }

            ecs_vec_append_t(NULL, children, ecs_entity_t)[0] = e;
        }
    }
}

void flecs_command_descendants(
    ecs_world_t *world,
    ecs_entity_t scope,
    int32_t depth,
    flecs_command_visit_t visit,
    void *ctx)
{
    ecs_vec_t children;
    ecs_vec_init_t(NULL, &children, ecs_entity_t, 0);
    flecs_command_children(world, scope, &children);

    ecs_entity_t *entities = ecs_vec_first(&children);
    int32_t i, count = ecs_vec_count(&children);
    for (i = 0; i < count; i ++) {
        ecs_entity_t e = entities[i];
        if (!ecs_is_alive(world, e)) {
            continue;
        }

        visit(world, e, depth, ctx);

        if (!scope && e == EcsFlecs) {
            continue;
        }

        flecs_command_descendants(world, e, depth + 1, visit, ctx);
    }

    ecs_vec_fini_t(NULL, &children, ecs_entity_t);
}

int32_t flecs_command_line_count(
    const char *code)
{
    if (!code || !code[0]) {
        return 0;
    }

    int32_t count = 1;
    const char *ptr;
    for (ptr = code; ptr[0]; ptr ++) {
        if (ptr[0] == '\n' && ptr[1]) {
            count ++;
        }
    }

    return count;
}

void flecs_command_lines_append(
    ecs_vec_t *lines,
    const char *code,
    int32_t first,
    int32_t last)
{
    if (!code) {
        return;
    }

    const char *ptr = code;
    int32_t line = 1;
    while (ptr[0] && (last < 0 || line <= last)) {
        const char *end = strchr(ptr, '\n');
        if (!end) {
            end = ptr + ecs_os_strlen(ptr);
        }

        if (line >= first) {
            ecs_command_script_line_t *l = ecs_vec_append_t(
                NULL, lines, ecs_command_script_line_t);
            l->line = line;
            l->text = flecs_asprintf("%.*s",
                flecs_ito(int32_t, end - ptr), ptr);
        }

        if (!end[0]) {
            break;
        }

        ptr = end + 1;
        line ++;
    }
}

static const char* flecs_command_skip_string(
    const char *ptr)
{
    ecs_assert(ptr[0] == '"', ECS_INTERNAL_ERROR, NULL);
    ptr ++;
    while (ptr[0] && ptr[0] != '"') {
        if (ptr[0] == '\\' && ptr[1]) {
            ptr ++;
        }
        ptr ++;
    }

    return ptr[0] ? ptr + 1 : NULL;
}

static char* flecs_command_unescape(
    const char *start,
    const char *end)
{
    char *result = ecs_os_malloc(flecs_ito(int32_t, end - start) + 1);
    char *out = result;
    const char *ptr = start;
    while (ptr < end) {
        if (ptr[0] == '\\') {
            const char *next = flecs_chrparse(ptr, out);
            if (!next) {
                ecs_err("invalid escape sequence '\\%c'", ptr[1]);
                ecs_os_free(result);
                return NULL;
            }

            ptr = next;
        } else {
            out[0] = ptr[0];
            ptr ++;
        }

        out ++;
    }

    out[0] = '\0';
    return result;
}

static const char* flecs_command_parse_group(
    const char *ptr)
{
    char closers[64];
    int32_t depth = 0;
    while (ptr[0]) {
        char ch = ptr[0];
        if (!depth && isspace(ch)) {
            break;
        }

        if (ch == '"') {
            const char *end = flecs_command_skip_string(ptr);
            if (!end) {
                ecs_err("missing '\"' in argument");
                return NULL;
            }

            ptr = end;
            continue;
        }

        if (ch == '(' || ch == '{' || ch == '[') {
            if (depth == 64) {
                ecs_err("argument is nested too deeply");
                return NULL;
            }

            closers[depth ++] = ch == '(' ? ')' : ch == '{' ? '}' : ']';
        } else if (ch == ')' || ch == '}' || ch == ']') {
            if (!depth || closers[depth - 1] != ch) {
                ecs_err("unexpected '%c' in argument", ch);
                return NULL;
            }

            depth --;
        }

        ptr ++;
    }

    if (depth) {
        ecs_err("missing '%c' in argument", closers[depth - 1]);
        return NULL;
    }

    return ptr;
}

static int flecs_command_tokenize(
    const char *cmd,
    ecs_vec_t *tokens)
{
    const char *ptr = cmd;
    while (true) {
        while (isspace(ptr[0])) {
            ptr ++;
        }

        if (!ptr[0]) {
            break;
        }

        char *text = NULL;
        if (!ecs_os_strncmp(ptr, "```", 3)) {
            const char *start = ptr + 3;
            const char *end = strstr(start, "```");
            if (!end) {
                ecs_err("missing closing ``` in command");
                return -1;
            }

            text = flecs_asprintf("%.*s",
                flecs_ito(int32_t, end - start), start);
            ptr = end + 3;
        } else if (ptr[0] == '"') {
            const char *end = flecs_command_skip_string(ptr);
            if (!end) {
                ecs_err("missing closing '\"' in command");
                return -1;
            }

            text = flecs_command_unescape(ptr + 1, end - 1);
            if (!text) {
                return -1;
            }

            ptr = end;
        } else {
            const char *end = flecs_command_parse_group(ptr);
            if (!end) {
                return -1;
            }

            text = flecs_asprintf("%.*s", flecs_ito(int32_t, end - ptr), ptr);
            ptr = end;
        }

        ecs_vec_append_t(NULL, tokens, char*)[0] = text;
    }

    return 0;
}

static int flecs_command_parse_pair(
    ecs_world_t *world,
    const char *text,
    ecs_id_t *out)
{
    ecs_size_t len = ecs_os_strlen(text);
    const char *comma = NULL, *ptr;
    int32_t depth = 0;
    for (ptr = text + 1; ptr < (text + len - 1); ptr ++) {
        if (ptr[0] == '(' || ptr[0] == '{' || ptr[0] == '[') {
            depth ++;
        } else if (ptr[0] == ')' || ptr[0] == '}' || ptr[0] == ']') {
            depth --;
        } else if (ptr[0] == ',' && !depth) {
            comma = ptr;
            break;
        }
    }

    if (!comma) {
        return 1;
    }

    char *elems[2] = {
        flecs_asprintf("%.*s", flecs_ito(int32_t, comma - text - 1), text + 1),
        flecs_asprintf("%.*s", len - flecs_ito(int32_t, comma - text) - 2,
            comma + 1)
    };

    ecs_entity_t ids[2] = {0};
    int32_t i, result = 0;
    for (i = 0; i < 2; i ++) {
        ecs_value_t v = { .type = ecs_id(ecs_entity_t), .ptr = &ids[i] };
        if (flecs_command_parse_value(world, elems[i], &v, false)) {
            result = -1;
        }
        ecs_os_free(elems[i]);
    }

    if (!result) {
        *out = ecs_pair(ids[0], ids[1]);
    }

    return result;
}

static bool flecs_command_is_relative(
    const char *text)
{
    if (text[0] != '.') {
        return false;
    }

    if (!text[1] || text[1] == '/') {
        return true;
    }

    return text[1] == '.' && (!text[2] || text[2] == '/');
}

static int flecs_command_parse_relative(
    ecs_world_t *world,
    const char *text,
    ecs_entity_t *out)
{
    ecs_entity_t e = flecs_command_scope(world);
    const char *ptr = text;
    while (ptr[0]) {
        const char *end = strchr(ptr, '/');
        int32_t len = end ? flecs_ito(int32_t, end - ptr) :
            ecs_os_strlen(ptr);

        if (len == 2 && !ecs_os_strncmp(ptr, "..", 2)) {
            if (e) {
                e = ecs_get_parent(world, e);
            }
        } else if (len && (len != 1 || ptr[0] != '.')) {
            char *name = flecs_asprintf("%.*s", len, ptr);
            e = ecs_lookup_child(world, e, name);
            ecs_os_free(name);
            if (!e) {
                return -1;
            }
        }

        ptr += len;
        if (ptr[0] == '/') {
            ptr ++;
        }
    }

    *out = e;
    return 0;
}

static char* flecs_command_usage(
    const char *name,
    const EcsScriptFunction *f)
{
    ecs_strbuf_t buf = ECS_STRBUF_INIT;
    ecs_strbuf_appendstr(&buf, name);
    const ecs_script_parameter_t *params = ecs_vec_first(&f->params);
    int32_t i, count = ecs_vec_count(&f->params);
    for (i = 0; i < count; i ++) {
        ecs_strbuf_append(&buf, params[i].optional ? " [%s]" : " <%s>",
            params[i].name);
    }

    return ecs_strbuf_get(&buf);
}

static int flecs_command_parse_args(
    ecs_world_t *world,
    const char *name,
    const EcsScriptFunction *f,
    char **tokens,
    int32_t argc,
    ecs_value_t *argv)
{
    const ecs_script_parameter_t *params = ecs_vec_first(&f->params);
    int32_t i, component = -1;
    for (i = 0; i < argc; i ++) {
        if (params[i].type == ecs_id(ecs_id_t)) {
            component = i;
            break;
        }
    }

    for (i = 0; i < argc; i ++) {
        ecs_entity_t type = params[i].type;
        const char *text = tokens[i];
        argv[i] = ecs_value_new(world, type);
        if (!argv[i].ptr) {
            ecs_err("invalid type for parameter '%s' of command '%s'",
                params[i].name, name);
            return -1;
        }

        if (type == ecs_id(ecs_string_t)) {
            *(char**)argv[i].ptr = ecs_os_strdup(text);
            continue;
        }

        if (type == ecs_id(ecs_entity_t) && !ecs_os_strcmp(text, "$")) {
            continue;
        }

        if (type == ecs_id(ecs_entity_t) && flecs_command_is_relative(text)) {
            if (flecs_command_parse_relative(world, text, argv[i].ptr)) {
                ecs_err("invalid argument '%s' for parameter '%s' of "
                    "command '%s'", text, params[i].name, name);
                return -1;
            }
            continue;
        }

        if (type == ecs_id(ecs_id_t) && text[0] == '(' &&
            text[ecs_os_strlen(text) - 1] == ')')
        {
            int r = flecs_command_parse_pair(world, text, argv[i].ptr);
            if (r == 0) {
                continue;
            }

            if (r == -1) {
                ecs_err("invalid argument '%s' for parameter '%s' of "
                    "command '%s'", text, params[i].name, name);
                return -1;
            }
        }

        if (flecs_command_parse_value(world, text, &argv[i], false)) {
            ecs_err("invalid argument '%s' for parameter '%s' of command '%s'",
                text, params[i].name, name);
            return -1;
        }
    }

    for (i = 0; i < argc; i ++) {
        if (params[i].type != ecs_id(ecs_entity_t) ||
            ecs_os_strcmp(tokens[i], "$"))
        {
            continue;
        }

        if (component == -1) {
            ecs_err("'$' requires a component argument for command '%s'",
                name);
            return -1;
        }

        ecs_id_t id = *(ecs_id_t*)argv[component].ptr;
        if (ECS_IS_PAIR(id)) {
            id = ecs_pair_first(world, id);
        }

        *(ecs_entity_t*)argv[i].ptr = id;
    }

    return 0;
}

static char* flecs_command_history_push(
    EcsCommandSession *s,
    const char *cmd)
{
    int32_t size = s->history_size;
    if (size <= 0) {
        return NULL;
    }

    ecs_vec_init_if_t(&s->history, char*);
    ecs_vec_init_if_t(&s->results, char*);

    while (ecs_vec_count(&s->history) >= size) {
        ecs_os_free(ecs_vec_first_t(&s->history, char*)[0]);
        ecs_vec_remove_ordered_t(&s->history, char*, 0);
        ecs_os_free(ecs_vec_first_t(&s->results, char*)[0]);
        ecs_vec_remove_ordered_t(&s->results, char*, 0);
    }

    char *result = ecs_os_strdup(cmd);
    ecs_vec_append_t(NULL, &s->history, char*)[0] = result;
    ecs_vec_append_t(NULL, &s->results, char*)[0] = NULL;
    return result;
}

static void flecs_command_history_set_result(
    ecs_world_t *world,
    ecs_entity_t session,
    const char *cmd,
    const char *scope,
    const ecs_command_result_t *result)
{
    EcsCommandSession *s = ecs_get_mut(world, session, EcsCommandSession);
    if (!s) {
        return;
    }

    int32_t count = ecs_vec_count(&s->history);
    if (!count || ecs_vec_last_t(&s->history, char*)[0] != cmd) {
        return;
    }

    ecs_strbuf_t buf = ECS_STRBUF_INIT;
    ecs_strbuf_appendlit(&buf, "{\"scope\":");
    ecs_strbuf_appendstr(&buf, scope);

    if (result->result.type) {
        ecs_strbuf_appendlit(&buf, ", \"type\":");
        char *type = ecs_get_path(world, result->result.type);
        ecs_ptr_to_json_buf(world, ecs_id(ecs_string_t), &type, &buf);
        ecs_os_free(type);
        ecs_strbuf_appendlit(&buf, ", \"result\":");
        char *json = ecs_ptr_to_json(
            world, result->result.type, result->result.ptr);
        if (json) {
            ecs_strbuf_appendstr(&buf, json);
            ecs_os_free(json);
        } else {
            ecs_strbuf_appendlit(&buf, "null");
        }
    }

    ecs_strbuf_appendlit(&buf, ", \"errors\":[");
    char **errors = ecs_vec_first(&result->errors);
    int32_t i, error_count = ecs_vec_count(&result->errors);
    for (i = 0; i < error_count; i ++) {
        if (i) {
            ecs_strbuf_appendlit(&buf, ", ");
        }
        ecs_ptr_to_json_buf(world, ecs_id(ecs_string_t), &errors[i], &buf);
    }
    ecs_strbuf_appendlit(&buf, "]}");

    char **ptr = ecs_vec_last_t(&s->results, char*);
    ecs_os_free(ptr[0]);
    ptr[0] = ecs_strbuf_get(&buf);
}

void flecs_command_history_clear(
    EcsCommandSession *s)
{
    char **array = ecs_vec_first(&s->history);
    int32_t i, count = ecs_vec_count(&s->history);
    for (i = 0; i < count; i ++) {
        ecs_os_free(array[i]);
    }

    array = ecs_vec_first(&s->results);
    count = ecs_vec_count(&s->results);
    for (i = 0; i < count; i ++) {
        ecs_os_free(array[i]);
    }

    ecs_vec_clear(&s->history);
    ecs_vec_clear(&s->results);
}

static int flecs_command_invoke(
    ecs_world_t *world,
    const char *cmd,
    ecs_value_t *result)
{
    int r = -1;
    ecs_vec_t tokens;
    ecs_vec_init_t(NULL, &tokens, char*, 0);
    ecs_value_t argv[FLECS_SCRIPT_FUNCTION_ARGS_MAX] = {{0}};
    int32_t i, argc = 0;
    char *usage = NULL;

    if (flecs_command_tokenize(cmd, &tokens)) {
        goto done;
    }

    if (!ecs_vec_count(&tokens)) {
        ecs_err("empty command");
        goto done;
    }

    char **t = ecs_vec_first(&tokens);
    const char *name = t[0];
    ecs_entity_t function = flecs_command_resolve(world, name);
    const EcsScriptFunction *f = function ?
        ecs_get(world, function, EcsScriptFunction) : NULL;
    if (!f || !f->callback) {
        ecs_err("unknown command '%s'", name);
        goto done;
    }

    usage = flecs_command_usage(name, f);
    argc = ecs_vec_count(&tokens) - 1;
    if (argc > ecs_vec_count(&f->params)) {
        ecs_err("too many arguments for command '%s' (usage: %s)",
            name, usage);
        argc = 0;
        goto done;
    }

    int32_t required = flecs_script_function_required_params(f);
    if (argc < required) {
        ecs_err("missing argument '%s' for command '%s' (usage: %s)",
            ecs_vec_get_t(&f->params, ecs_script_parameter_t, argc)->name,
            name, usage);
        argc = 0;
        goto done;
    }

    if (flecs_command_parse_args(world, name, f, &t[1], argc, argv)) {
        goto done;
    }

    if (ecs_function_call(world, function, argc, argv, result)) {
        goto done;
    }

    r = 0;
done:
    for (i = 0; i < argc; i ++) {
        ecs_value_fini(world, &argv[i]);
    }

    ecs_os_free(usage);
    flecs_command_strings_fini(&tokens);
    return r;
}

ecs_command_result_t ecs_command_run(
    ecs_world_t *world,
    ecs_entity_t session,
    const char *cmd)
{
    ecs_command_result_t result = { .result = { 0 } };
    ecs_vec_init_t(NULL, &result.errors, char*, 0);

    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(cmd != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(ecs_id(EcsCommandState) != 0, ECS_INVALID_OPERATION,
        "script command module is not imported");

    ecs_log_start_collect_errors(&result.errors);

    EcsCommandSession *s = NULL;
    if (session && ecs_is_alive(world, session)) {
        s = ecs_get_mut(world, session, EcsCommandSession);
    }

    if (!s) {
        ecs_err("entity #%u is not a command session", (uint32_t)session);
        goto done;
    }

    char *entry = flecs_command_history_push(s, cmd);
    ecs_entity_t cur_scope = s->scope;
    if (cur_scope && !ecs_is_alive(world, cur_scope)) {
        cur_scope = 0;
    }

    char *scope = entry ? ecs_ptr_to_json(
        world, ecs_id(ecs_entity_t), &cur_scope) : NULL;

    EcsCommandState *state = ecs_singleton_ensure(world, EcsCommandState);
    ecs_entity_t prev_session = state->session;
    state->session = session;

    int32_t error_count = ecs_vec_count(&result.errors);
    if (flecs_command_invoke(world, cmd, &result.result)) {
        if (ecs_vec_count(&result.errors) == error_count) {
            ecs_err("command failed");
        }
    }

    ecs_singleton_ensure(world, EcsCommandState)->session = prev_session;
    ecs_log_stop_collect_errors();

    if (scope && ecs_is_alive(world, session)) {
        flecs_command_history_set_result(world, session, entry, scope, &result);
    }

    ecs_os_free(scope);
    return result;
done:
    ecs_log_stop_collect_errors();
error:
    return result;
}

void ecs_command_result_fini(
    ecs_world_t *world,
    ecs_command_result_t *result)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(result != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_value_fini(world, &result->result);

    flecs_command_strings_fini(&result->errors);
error:
    return;
}

ecs_entity_t ecs_command_session_init(
    ecs_world_t *world,
    const ecs_command_session_desc_t *desc)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(ecs_id(EcsCommandSession) != 0, ECS_INVALID_OPERATION,
        "script command module is not imported");

    ecs_command_session_desc_t empty = {0};
    if (!desc) {
        desc = &empty;
    }

    ecs_entity_t session = desc->entity;
    if (!session) {
        ecs_entity_t parent = ecs_lookup(world, "script.shell.sessions");
        session = ecs_new_w_pair(world, EcsChildOf, parent);
    }

    EcsCommandSession *s = ecs_ensure(world, session, EcsCommandSession);
    s->scope = desc->scope;
    s->history_size = desc->history_size ?
        desc->history_size : FLECS_SCRIPT_SHELL_HISTORY_SIZE;
    ecs_modified(world, session, EcsCommandSession);
    return session;
error:
    return 0;
}

void flecs_command_register(
    ecs_world_t *world,
    ecs_entity_t scope,
    const flecs_command_desc_t *commands,
    int32_t count)
{
    int32_t i;
    for (i = 0; i < count; i ++) {
        const flecs_command_desc_t *cmd = &commands[i];
        ecs_function_desc_t desc = {
            .name = cmd->name,
            .parent = scope,
            .return_type = cmd->return_type,
            .callback = cmd->callback,
            .ctx = cmd->ctx
        };

        ecs_os_memcpy_n(desc.params, cmd->params, ecs_script_parameter_t, 4);
        ecs_doc_set_brief(world, ecs_function_init(world, &desc), cmd->brief);
    }
}

#define flecs_command_struct(world, T, type_name, ...)\
    ecs_id(T) = ecs_struct(world, {\
        .entity = ecs_entity(world, { .name = type_name }),\
        .members = __VA_ARGS__\
    });\
    ecs_assert(ecs_get_type_info(world, ecs_id(T))->size == ECS_SIZEOF(T),\
        ECS_INTERNAL_ERROR, NULL);\
    ecs_assert(ecs_get_type_info(world, ecs_id(T))->alignment ==\
        ECS_ALIGNOF(T), ECS_INTERNAL_ERROR, NULL)

static void flecs_command_register_types(
    ecs_world_t *world)
{
    ecs_entity_t string_t = ecs_id(ecs_string_t);
    ecs_entity_t entity_t = ecs_id(ecs_entity_t);
    ecs_entity_t id_t = ecs_id(ecs_id_t);
    ecs_entity_t i32_t = ecs_id(ecs_i32_t);
    ecs_entity_t bool_t = ecs_id(ecs_bool_t);

    flecs_command_struct(world, ecs_command_param_info_t, "ParamInfo", {
        { .name = "name", .type = string_t },
        { .name = "type", .type = entity_t },
        { .name = "optional", .type = bool_t },
        { .name = "brief", .type = string_t }
    });

    flecs_command_struct(world, ecs_command_info_t, "CommandInfo", {
        { .name = "name", .type = string_t },
        { .name = "brief", .type = string_t },
        { .name = "params", .type = flecs_script_vector_type(
            world, ecs_id(ecs_command_param_info_t)) },
        { .name = "return_type", .type = entity_t }
    });

    flecs_command_struct(world, ecs_command_tree_node_t, "TreeNode", {
        { .name = "entity", .type = entity_t },
        { .name = "depth", .type = i32_t }
    });

    flecs_command_struct(world, ecs_command_component_value_t,
        "ComponentValue", {
        { .name = "id", .type = id_t },
        { .name = "value", .type = ecs_id(ecs_value_t) }
    });

    flecs_command_struct(world, ecs_command_entity_info_t, "EntityInfo", {
        { .name = "entity", .type = entity_t },
        { .name = "id", .type = ecs_id(ecs_u64_t) },
        { .name = "components", .type = flecs_script_vector_type(
            world, ecs_id(ecs_command_component_value_t)) },
        { .name = "ids", .type = flecs_script_vector_type(world, id_t) }
    });

    flecs_command_struct(world, ecs_command_doc_info_t, "DocInfo", {
        { .name = "entity", .type = entity_t },
        { .name = "name", .type = string_t },
        { .name = "brief", .type = string_t },
        { .name = "detail", .type = string_t },
        { .name = "link", .type = string_t },
        { .name = "color", .type = string_t }
    });

    flecs_command_struct(world, ecs_command_query_row_t, "QueryRow", {
        { .name = "entity", .type = entity_t },
        { .name = "fields", .type = flecs_script_vector_type(
            world, ecs_id(ecs_command_component_value_t)) }
    });

    flecs_command_struct(world, ecs_command_grep_match_t, "GrepMatch", {
        { .name = "entity", .type = entity_t },
        { .name = "id", .type = id_t },
        { .name = "text", .type = string_t }
    });

    flecs_command_struct(world, ecs_command_script_info_t, "ScriptInfo", {
        { .name = "script", .type = entity_t },
        { .name = "filename", .type = string_t },
        { .name = "lines", .type = i32_t },
        { .name = "unsaved", .type = bool_t },
        { .name = "error", .type = string_t }
    });

    flecs_command_struct(world, ecs_command_script_line_t, "ScriptLine", {
        { .name = "line", .type = i32_t },
        { .name = "text", .type = string_t }
    });

    flecs_command_struct(world, ecs_command_script_match_t, "ScriptMatch", {
        { .name = "script", .type = entity_t },
        { .name = "line", .type = i32_t },
        { .name = "text", .type = string_t }
    });

    flecs_command_struct(world, ecs_command_script_location_t,
        "ScriptLocation", {
        { .name = "script", .type = entity_t },
        { .name = "line", .type = i32_t },
        { .name = "column", .type = i32_t },
        { .name = "template", .type = entity_t },
        { .name = "statement", .type = string_t }
    });

    flecs_command_struct(world, ecs_command_script_error_t, "ScriptError", {
        { .name = "script", .type = entity_t },
        { .name = "error", .type = string_t }
    });

#ifdef FLECS_SCRIPT_INPUT
    flecs_command_struct(world, ecs_command_pointer_state_t, "PointerState", {
        { .name = "event", .type = ecs_id(EcsScriptMouseEvent) },
        { .name = "pressed", .type = entity_t }
    });

    flecs_command_struct(world, ecs_script_event_result_t, "EventResult", {
        { .name = "resolved", .type = i32_t },
        { .name = "stopped", .type = bool_t }
    });
#endif
}

static ecs_entity_t flecs_command_path_init(
    ecs_world_t *world,
    const char *name)
{
    ecs_entity_t path = ecs_entity(world, { .name = name });
    EcsScriptShell *shell = ecs_singleton_ensure(world, EcsScriptShell);
    ecs_vec_append_t(NULL, &shell->command_paths, ecs_entity_t)[0] = path;
    ecs_singleton_modified(world, EcsScriptShell);
    return path;
}

void FlecsScriptShellImport(
    ecs_world_t *world)
{
    ECS_MODULE(world, FlecsScriptShell);
    ECS_IMPORT(world, FlecsScript);
    ECS_IMPORT(world, FlecsDoc);
#ifdef FLECS_SCRIPT_INPUT
    ECS_IMPORT(world, FlecsScriptInput);
#endif

    ecs_set_name_prefix(world, "Ecs");
    ECS_COMPONENT_DEFINE(world, EcsCommandSession);
    ECS_COMPONENT_DEFINE(world, EcsCommandState);

    ecs_set_name_prefix(world, "EcsScript");
    ECS_COMPONENT_DEFINE(world, EcsScriptShell);

    ecs_set_hooks(world, EcsScriptShell, {
        .ctor = flecs_default_ctor,
        .copy = ecs_copy(EcsScriptShell),
        .move = ecs_move(EcsScriptShell),
        .dtor = ecs_dtor(EcsScriptShell)
    });

    ecs_struct(world, {
        .entity = ecs_id(EcsScriptShell),
        .members = {
            { .name = "command_paths", .type = flecs_script_vector_type(
                world, ecs_id(ecs_entity_t)) }
        }
    });

    ecs_set_hooks(world, EcsCommandState, {
        .ctor = flecs_default_ctor
    });

    ecs_set_hooks(world, EcsCommandSession, {
        .ctor = flecs_default_ctor,
        .move = ecs_move(EcsCommandSession),
        .dtor = ecs_dtor(EcsCommandSession),
        .flags = ECS_TYPE_HOOK_COPY_ILLEGAL
    });

    ecs_struct(world, {
        .entity = ecs_id(EcsCommandSession),
        .members = {
            { .name = "scope", .type = ecs_id(ecs_entity_t) },
            { .name = "history", .type = flecs_script_vector_type(
                world, ecs_id(ecs_string_t)) },
            { .name = "history_size", .type = ecs_id(ecs_i32_t) }
        }
    });

    ecs_add_pair(world, ecs_id(EcsCommandSession), EcsOnInstantiate,
        EcsDontInherit);

    flecs_command_register_types(world);
    flecs_command_core_import(world,
        flecs_command_path_init(world, "commands.core"));
#ifdef FLECS_SCRIPT_INPUT
    flecs_command_input_import(world,
        flecs_command_path_init(world, "commands.input"));
#endif
    flecs_command_script_import(world,
        flecs_command_path_init(world, "commands.script"));

    ecs_entity_t prev_scope = ecs_set_scope(world, 0);
    ecs_entity(world, { .name = "script.shell.sessions" });
    ecs_set_scope(world, prev_scope);

    ecs_singleton_add(world, EcsCommandState);
}

#endif
