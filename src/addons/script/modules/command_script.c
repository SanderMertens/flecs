/**
 * @file addons/script/modules/command_script.c
 * @brief Commands for reading and editing managed scripts.
 */

#include "flecs.h"

#ifdef FLECS_SCRIPT_COMMAND
#include "command.h"

typedef struct {
    char *path;
    ecs_entity_t entity;
} flecs_command_script_elem_t;

static int flecs_command_script_compare(
    const void *a,
    const void *b)
{
    return ecs_os_strcmp(((const flecs_command_script_elem_t*)a)->path,
        ((const flecs_command_script_elem_t*)b)->path);
}

static bool flecs_command_in_scope(
    const ecs_world_t *world,
    ecs_entity_t e,
    ecs_entity_t scope)
{
    ecs_entity_t cur;
    for (cur = ecs_get_parent(world, e); cur; cur = ecs_get_parent(world, cur)) {
        if (cur == scope) {
            return true;
        }

        if (!scope && cur == EcsFlecs) {
            return false;
        }
    }

    return !scope;
}

static void flecs_command_collect_scripts(
    ecs_world_t *world,
    ecs_entity_t scope,
    bool all,
    ecs_vec_t *out)
{
    ecs_vec_t elems;
    ecs_vec_init_t(NULL, &elems, flecs_command_script_elem_t, 0);

    ecs_iter_t it = ecs_each(world, EcsScript);
    while (ecs_each_next(&it)) {
        EcsScript *s = ecs_field(&it, EcsScript, 0);
        int32_t i;
        for (i = 0; i < it.count; i ++) {
            ecs_entity_t e = it.entities[i];
            if (!all && (s[i].template_ || !flecs_command_in_scope(
                world, e, scope)))
            {
                continue;
            }

            flecs_command_script_elem_t *elem = ecs_vec_append_t(
                NULL, &elems, flecs_command_script_elem_t);
            elem->path = ecs_get_path(world, e);
            elem->entity = e;
        }
    }

    flecs_command_script_elem_t *array = ecs_vec_first(&elems);
    int32_t i, count = ecs_vec_count(&elems);
    if (count) {
        qsort(array, (size_t)count, sizeof(flecs_command_script_elem_t),
            flecs_command_script_compare);
    }

    for (i = 0; i < count; i ++) {
        ecs_vec_append_t(NULL, out, ecs_entity_t)[0] = array[i].entity;
        ecs_os_free(array[i].path);
    }

    ecs_vec_fini_t(NULL, &elems, flecs_command_script_elem_t);
}

static const EcsScript* flecs_command_get_script(
    const ecs_function_ctx_t *ctx,
    ecs_entity_t e)
{
    if (!flecs_command_check_alive(ctx, e)) {
        return NULL;
    }

    const EcsScript *s = ecs_get(ctx->world, e, EcsScript);
    if (!s || s->template_) {
        flecs_command_err(ctx, "'%s' is not a script",
            flecs_errstr(ecs_get_path(ctx->world, e)));
        return NULL;
    }

    return s;
}

static const EcsScript* flecs_command_get_owner(
    const ecs_function_ctx_t *ctx,
    ecs_entity_t e,
    ecs_entity_t *owner_out)
{
    if (!flecs_command_check_alive(ctx, e)) {
        return NULL;
    }

    ecs_entity_t owner = ecs_script_entity_owner(ctx->world, e);
    const EcsScript *s = owner ? ecs_get(ctx->world, owner, EcsScript) : NULL;
    if (!s) {
        flecs_command_err(ctx,
            "entity '%s' is not declared by a managed script",
            flecs_errstr(ecs_get_path(ctx->world, e)));
        return NULL;
    }

    if (!s->script) {
        flecs_command_err(ctx, "script '%s' has errors, fix them before "
            "editing entities", flecs_errstr(ecs_get_path(ctx->world, owner)));
        return NULL;
    }

    *owner_out = owner;
    return s;
}

static int32_t flecs_command_line_of(
    const char *code,
    int32_t offset)
{
    int32_t i, line = 1;
    for (i = 0; i < offset && code[i]; i ++) {
        if (code[i] == '\n') {
            line ++;
        }
    }

    return line;
}

static void flecs_command_line_append_unique(
    ecs_vec_t *lines,
    int32_t line)
{
    int32_t count = ecs_vec_count(lines);
    if (count && ecs_vec_get_t(lines, int32_t, count - 1)[0] >= line) {
        return;
    }

    ecs_vec_append_t(NULL, lines, int32_t)[0] = line;
}

static void flecs_command_lines_select(
    ecs_vec_t *out,
    const char *code,
    const ecs_vec_t *lines)
{
    const int32_t *l = ecs_vec_first(lines);
    int32_t i, count = ecs_vec_count(lines);
    for (i = 0; i < count; i ++) {
        flecs_command_lines_append(out, code, l[i], l[i]);
    }
}

static int32_t flecs_command_first_diff_line(
    const char *a,
    const char *b)
{
    int32_t line = 1;
    while (a[0] && a[0] == b[0]) {
        if (a[0] == '\n') {
            line ++;
        }
        a ++;
        b ++;
    }

    if (!a[0] && !b[0]) {
        return 0;
    }

    return line;
}

static void flecs_command_scripts(
    const ecs_function_ctx_t *ctx,
    int32_t argc,
    const ecs_value_t *argv,
    ecs_value_t *result)
{
    ecs_world_t *world = ctx->world;
    ecs_entity_t scope = argc ? flecs_command_arg(argv, 0, ecs_entity_t) :
        flecs_command_scope(world);
    if (argc && scope && !flecs_command_check_alive(ctx, scope)) {
        return;
    }

    ecs_vec_t scripts;
    ecs_vec_init_t(NULL, &scripts, ecs_entity_t, 0);
    flecs_command_collect_scripts(world, scope, false, &scripts);

    const ecs_entity_t *entities = ecs_vec_first(&scripts);
    int32_t i, count = ecs_vec_count(&scripts);
    for (i = 0; i < count; i ++) {
        const EcsScript *s = ecs_get(world, entities[i], EcsScript);
        ecs_command_script_info_t *info = ecs_vec_append_t(
            NULL, result->ptr, ecs_command_script_info_t);
        info->script = entities[i];
        info->filename = ecs_os_strdup(s->filename);
        info->lines = flecs_command_line_count(s->code);
        info->unsaved = ecs_os_strcmp(s->code ? s->code : "",
            s->saved_code ? s->saved_code : "") != 0;
        info->error = ecs_os_strdup(s->error);
    }

    ecs_vec_fini_t(NULL, &scripts, ecs_entity_t);
}

static void flecs_command_script_read(
    const ecs_function_ctx_t *ctx,
    int32_t argc,
    const ecs_value_t *argv,
    ecs_value_t *result)
{
    (void)argc;
    const EcsScript *s = flecs_command_get_script(
        ctx, flecs_command_arg(argv, 0, ecs_entity_t));
    if (s) {
        flecs_command_lines_append(result->ptr, s->code, 1, -1);
    }
}

static void flecs_command_script_grep_one(
    ecs_entity_t script,
    const EcsScript *s,
    const char *text,
    ecs_vec_t *out)
{
    ecs_vec_t lines;
    ecs_vec_init_t(NULL, &lines, ecs_command_script_line_t, 0);
    flecs_command_lines_append(&lines, s->code, 1, -1);

    ecs_command_script_line_t *l = ecs_vec_first(&lines);
    int32_t i, count = ecs_vec_count(&lines);
    for (i = 0; i < count; i ++) {
        if (strstr(l[i].text, text)) {
            ecs_command_script_match_t *m = ecs_vec_append_t(
                NULL, out, ecs_command_script_match_t);
            m->script = script;
            m->line = l[i].line;
            m->text = l[i].text;
        } else {
            ecs_os_free(l[i].text);
        }
    }

    ecs_vec_fini_t(NULL, &lines, ecs_command_script_line_t);
}

static void flecs_command_script_grep(
    const ecs_function_ctx_t *ctx,
    int32_t argc,
    const ecs_value_t *argv,
    ecs_value_t *result)
{
    ecs_world_t *world = ctx->world;
    const char *text = flecs_command_arg(argv, 0, char*);
    if (!text) {
        text = "";
    }

    if (argc > 1) {
        ecs_entity_t script = flecs_command_arg(argv, 1, ecs_entity_t);
        const EcsScript *s = flecs_command_get_script(ctx, script);
        if (s) {
            flecs_command_script_grep_one(script, s, text, result->ptr);
        }
        return;
    }

    ecs_vec_t scripts;
    ecs_vec_init_t(NULL, &scripts, ecs_entity_t, 0);
    flecs_command_collect_scripts(
        world, flecs_command_scope(world), false, &scripts);

    const ecs_entity_t *entities = ecs_vec_first(&scripts);
    int32_t i, count = ecs_vec_count(&scripts);
    for (i = 0; i < count; i ++) {
        flecs_command_script_grep_one(entities[i],
            ecs_get(world, entities[i], EcsScript), text, result->ptr);
    }

    ecs_vec_fini_t(NULL, &scripts, ecs_entity_t);
}

static void flecs_command_script_where(
    const ecs_function_ctx_t *ctx,
    int32_t argc,
    const ecs_value_t *argv,
    ecs_value_t *result)
{
    (void)argc;
    ecs_entity_t e = flecs_command_arg(argv, 0, ecs_entity_t);
    ecs_entity_t owner = 0;
    const EcsScript *s = flecs_command_get_owner(ctx, e, &owner);
    if (!s) {
        return;
    }

    ecs_script_source_t src;
    if (!ecs_script_entity_source(s->script, e, &src)) {
        flecs_command_err(ctx, "statement for entity '%s' not found",
            flecs_errstr(ecs_get_path(ctx->world, e)));
        return;
    }

    const char *code = s->script->code;
    const char *start = &code[src.offset];
    const char *end = start;
    while (end[0] && end[0] != '\n' && end < (start + src.length)) {
        end ++;
    }

    ecs_command_script_location_t *loc = result->ptr;
    loc->script = owner;
    loc->line = src.line;
    loc->column = src.column;
    loc->template_ = src.template_;
    loc->statement = flecs_asprintf("%.*s",
        flecs_ito(int32_t, end - start), start);
}

static void flecs_command_script_errors(
    const ecs_function_ctx_t *ctx,
    int32_t argc,
    const ecs_value_t *argv,
    ecs_value_t *result)
{
    (void)argc;
    (void)argv;
    ecs_world_t *world = ctx->world;
    ecs_vec_t scripts;
    ecs_vec_init_t(NULL, &scripts, ecs_entity_t, 0);
    flecs_command_collect_scripts(world, 0, true, &scripts);

    const ecs_entity_t *entities = ecs_vec_first(&scripts);
    int32_t i, count = ecs_vec_count(&scripts);
    for (i = 0; i < count; i ++) {
        const EcsScript *s = ecs_get(world, entities[i], EcsScript);
        if (!s->error) {
            continue;
        }

        ecs_command_script_error_t *err = ecs_vec_append_t(
            NULL, result->ptr, ecs_command_script_error_t);
        err->script = entities[i];
        err->error = ecs_os_strdup(s->error);
    }

    ecs_vec_fini_t(NULL, &scripts, ecs_entity_t);
}

static void flecs_command_script_replace(
    const ecs_function_ctx_t *ctx,
    int32_t argc,
    const ecs_value_t *argv,
    ecs_value_t *result)
{
    (void)argc;
    bool all = ctx->ctx != NULL;
    ecs_world_t *world = ctx->world;
    ecs_entity_t script = flecs_command_arg(argv, 0, ecs_entity_t);
    const char *old = flecs_command_arg(argv, 1, char*);
    const char *with = flecs_command_arg(argv, 2, char*);
    const EcsScript *s = flecs_command_get_script(ctx, script);
    if (!s) {
        return;
    }

    if (!old || !old[0]) {
        flecs_command_err(ctx, "text to replace cannot be empty");
        return;
    }

    if (!with) {
        with = "";
    }

    ecs_size_t old_len = ecs_os_strlen(old), with_len = ecs_os_strlen(with);
    ecs_vec_t lines;
    ecs_vec_init_t(NULL, &lines, int32_t, 0);
    ecs_strbuf_t buf = ECS_STRBUF_INIT;
    int32_t occurrences = 0, line = 1;
    const char *ptr = s->code ? s->code : "", *match;
    while ((match = strstr(ptr, old))) {
        int32_t l, prefix_len = flecs_ito(int32_t, match - ptr);
        ecs_strbuf_appendstrn(&buf, ptr, prefix_len);
        ecs_strbuf_appendstrn(&buf, with, with_len);
        line += flecs_command_line_of(ptr, prefix_len) - 1;
        int32_t last = line + flecs_command_line_of(with, with_len - 1) - 1;
        for (l = line; l <= last; l ++) {
            flecs_command_line_append_unique(&lines, l);
        }

        line += flecs_command_line_of(with, with_len) - 1;
        ptr = match + old_len;
        occurrences ++;
    }

    ecs_strbuf_appendstr(&buf, ptr);
    char *new_code = ecs_strbuf_get(&buf);

    if (!occurrences) {
        flecs_command_err(ctx, "text '%s' not found in script", old);
    } else if (!all && occurrences > 1) {
        flecs_command_err(ctx, "text '%s' occurs %d times in script, it must "
            "occur exactly once (use script_replace_all to replace all "
            "occurrences)", old, occurrences);
    } else {
        ecs_script_update(world, script, 0, new_code ? new_code : "");
        flecs_command_lines_select(result->ptr, new_code, &lines);
    }

    ecs_vec_fini_t(NULL, &lines, int32_t);
    ecs_os_free(new_code);
}

static void flecs_command_script_write(
    const ecs_function_ctx_t *ctx,
    int32_t argc,
    const ecs_value_t *argv,
    ecs_value_t *result)
{
    (void)argc;
    ecs_world_t *world = ctx->world;
    const char *name = flecs_command_arg(argv, 0, char*);
    const char *code = flecs_command_arg(argv, 1, char*);
    if (!name || !name[0]) {
        flecs_command_err(ctx, "invalid script name");
        return;
    }

    ecs_entity_t script = flecs_command_lookup(world, name);
    if (!script) {
        script = ecs_entity(world, {
            .name = name, .parent = flecs_command_scope(world) });
        if (!script) {
            flecs_command_fail(ctx);
            return;
        }
    } else {
        const EcsScript *s = ecs_get(world, script, EcsScript);
        if (s && s->template_) {
            flecs_command_err(ctx, "'%s' is a template, not a script", name);
            return;
        }
    }

    ecs_script(world, { .entity = script, .code = code ? code : "" });
    *(ecs_entity_t*)result->ptr = script;
}

static void flecs_command_script_patch(
    const ecs_function_ctx_t *ctx,
    int32_t argc,
    const ecs_value_t *argv,
    ecs_value_t *result)
{
    (void)argc;
    ecs_entity_t script = flecs_command_arg(argv, 0, ecs_entity_t);
    const char *patch = flecs_command_arg(argv, 1, char*);
    const EcsScript *s = flecs_command_get_script(ctx, script);
    if (!s) {
        return;
    }

    ecs_vec_t lines;
    ecs_vec_init_t(NULL, &lines, int32_t, 0);
    char *new_code = flecs_command_patch(
        s->code ? s->code : "", patch ? patch : "", &lines);
    if (!new_code) {
        ecs_vec_fini_t(NULL, &lines, int32_t);
        flecs_command_fail(ctx);
        return;
    }

    ecs_script_update(ctx->world, script, 0, new_code);
    flecs_command_lines_select(result->ptr, new_code, &lines);
    ecs_vec_fini_t(NULL, &lines, int32_t);
    ecs_os_free(new_code);
}

static char* flecs_command_edits_apply(
    const ecs_function_ctx_t *ctx,
    ecs_script_edits_t *edits,
    int edit_result)
{
    char *code = NULL;
    if (!edit_result) {
        code = ecs_script_edits_apply(edits);
    }

    ecs_script_edits_free(edits);

    if (!code) {
        flecs_command_err(ctx, "failed to edit script");
    }

    return code;
}

static void flecs_command_script_set(
    const ecs_function_ctx_t *ctx,
    int32_t argc,
    const ecs_value_t *argv,
    ecs_value_t *result)
{
    (void)argc;
    ecs_world_t *world = ctx->world;
    ecs_entity_t e = flecs_command_arg(argv, 0, ecs_entity_t);
    ecs_id_t id = flecs_command_arg(argv, 1, ecs_id_t);
    const char *expr = flecs_command_arg(argv, 2, char*);
    ecs_entity_t owner = 0;
    const EcsScript *s = flecs_command_get_owner(ctx, e, &owner);
    if (!s) {
        return;
    }

    ecs_value_t value;
    if (flecs_command_parse_component(ctx, e, id, expr, &value)) {
        return;
    }

    ecs_script_source_t src = {0};
    ecs_script_entity_source(s->script, e, &src);

    ecs_script_edits_t *edits = ecs_script_edits_new(s->script);
    int r = ecs_script_edits_set(edits, e, id, value.ptr);
    ecs_value_fini(world, &value);
    char *new_code = flecs_command_edits_apply(ctx, edits, r);
    if (!new_code) {
        return;
    }

    int32_t line = flecs_command_first_diff_line(
        s->code ? s->code : "", new_code);
    if (!line) {
        line = src.line;
    }

    ecs_script_update(world, owner, 0, new_code);

    ecs_vec_t lines;
    ecs_vec_init_t(NULL, &lines, ecs_command_script_line_t, 0);
    flecs_command_lines_append(&lines, new_code, line, line);
    if (ecs_vec_count(&lines)) {
        *(ecs_command_script_line_t*)result->ptr =
            ecs_vec_first_t(&lines, ecs_command_script_line_t)[0];
    }

    ecs_vec_fini_t(NULL, &lines, ecs_command_script_line_t);
    ecs_os_free(new_code);
}

static void flecs_command_script_remove(
    const ecs_function_ctx_t *ctx,
    int32_t argc,
    const ecs_value_t *argv,
    ecs_value_t *result)
{
    (void)argc;
    (void)result;
    ecs_entity_t e = flecs_command_arg(argv, 0, ecs_entity_t);
    ecs_entity_t owner = 0;
    const EcsScript *s = flecs_command_get_owner(ctx, e, &owner);
    if (!s) {
        return;
    }

    ecs_script_edits_t *edits = ecs_script_edits_new(s->script);
    int r = ctx->ctx ? ecs_script_edits_delete(edits, e) :
        ecs_script_edits_remove(edits, e, flecs_command_arg(argv, 1, ecs_id_t));
    char *new_code = flecs_command_edits_apply(ctx, edits, r);
    if (new_code) {
        ecs_script_update(ctx->world, owner, 0, new_code);
        ecs_os_free(new_code);
    }
}

static void flecs_command_script_sync(
    const ecs_function_ctx_t *ctx,
    int32_t argc,
    const ecs_value_t *argv,
    ecs_value_t *result)
{
    (void)argc;
    ecs_entity_t script = flecs_command_arg(argv, 0, ecs_entity_t);
    if (!flecs_command_get_script(ctx, script)) {
        return;
    }

    int r = ecs_script_from_scene(ctx->world, script);
    if (r < 0) {
        flecs_command_fail(ctx);
        return;
    }

    *(bool*)result->ptr = r == 1;
}

static void flecs_command_script_diff(
    const ecs_function_ctx_t *ctx,
    int32_t argc,
    const ecs_value_t *argv,
    ecs_value_t *result)
{
    (void)argc;
    const EcsScript *s = flecs_command_get_script(
        ctx, flecs_command_arg(argv, 0, ecs_entity_t));
    if (s) {
        *(char**)result->ptr = flecs_command_diff(
            s->saved_code ? s->saved_code : "", s->code ? s->code : "");
    }
}

static void flecs_command_script_save(
    const ecs_function_ctx_t *ctx,
    int32_t argc,
    const ecs_value_t *argv,
    ecs_value_t *result)
{
    (void)result;
    ecs_world_t *world = ctx->world;
    ecs_entity_t script = flecs_command_arg(argv, 0, ecs_entity_t);
    const EcsScript *s = flecs_command_get_script(ctx, script);
    if (!s) {
        return;
    }

    const char *filename = argc > 1 ?
        flecs_command_arg(argv, 1, char*) : s->filename;
    if (!filename || !filename[0]) {
        flecs_command_err(ctx, "script '%s' has no filename",
            flecs_errstr(ecs_get_path(world, script)));
        return;
    }

    if (s->filename && !ecs_os_strcmp(filename, s->filename)) {
        FILE *f = ecs_os_fopen(filename, "r");
        if (f) {
            ecs_os_fclose(f);
            char *disk = flecs_load_from_file(filename);
            bool changed = disk && ecs_os_strcmp(disk,
                s->saved_code ? s->saved_code : "");
            ecs_os_free(disk);
            if (changed) {
                flecs_command_err(ctx, "file '%s' changed on disk since it "
                    "was loaded or saved, reload the script before saving",
                    filename);
                return;
            }
        }
    } else {
        EcsScript *ms = ecs_get_mut(world, script, EcsScript);
        ecs_os_strset(&ms->filename, filename);
    }

    if (ecs_script_save(world, script)) {
        flecs_command_fail(ctx);
    }
}

static void flecs_command_script_reload(
    const ecs_function_ctx_t *ctx,
    int32_t argc,
    const ecs_value_t *argv,
    ecs_value_t *result)
{
    (void)argc;
    (void)result;
    ecs_entity_t script = flecs_command_arg(argv, 0, ecs_entity_t);
    const EcsScript *s = flecs_command_get_script(ctx, script);
    if (!s) {
        return;
    }

    int r;
    if (s->filename) {
        r = ecs_script_reload(ctx->world, script, NULL);
    } else {
        char *code = ecs_os_strdup(s->code ? s->code : "");
        r = ecs_script_reload(ctx->world, script, code);
        ecs_os_free(code);
    }

    if (r) {
        flecs_command_fail(ctx);
    }
}

static void flecs_command_script_undo(
    const ecs_function_ctx_t *ctx,
    int32_t argc,
    const ecs_value_t *argv,
    ecs_value_t *result)
{
    (void)argc;
    (void)result;
    ecs_entity_t script = flecs_command_arg(argv, 0, ecs_entity_t);
    if (!flecs_command_get_script(ctx, script)) {
        return;
    }

    if (ecs_script_undo(ctx->world, script)) {
        flecs_command_fail(ctx);
    }
}

void flecs_command_script_import(
    ecs_world_t *world)
{
    ecs_entity_t string_t = ecs_id(ecs_string_t);
    ecs_entity_t entity_t = ecs_id(ecs_entity_t);
    ecs_entity_t id_t = ecs_id(ecs_id_t);
    ecs_entity_t line_vec = flecs_script_vector_type(
        world, ecs_id(ecs_command_script_line_t));
    ecs_entity_t script_info_vec = flecs_script_vector_type(
        world, ecs_id(ecs_command_script_info_t));
    ecs_entity_t script_match_vec = flecs_script_vector_type(
        world, ecs_id(ecs_command_script_match_t));
    ecs_entity_t script_error_vec = flecs_script_vector_type(
        world, ecs_id(ecs_command_script_error_t));

    flecs_command_desc_t commands[] = {
        { "scripts", flecs_command_scripts, script_info_vec,
            "Scripts in descendants", {
            { "scope", entity_t, true,
                "Scope to search (default: session scope)" }}},
        { "script_read", flecs_command_script_read, line_vec,
            "Read the code of a script", {
            { "script", entity_t, false, "Script" }}},
        { "script_grep", flecs_command_script_grep, script_match_vec,
            "Find lines in script code", {
            { "text", string_t, false, "Text to find" },
            { "script", entity_t, true,
                "Script (default: scripts in session scope)" }}},
        { "script_where", flecs_command_script_where,
            ecs_id(ecs_command_script_location_t),
            "Script statement that declares an entity", {
            { "entity", entity_t, false, "Entity" }}},
        { "script_errors", flecs_command_script_errors, script_error_vec,
            "Scripts with errors" },
        { "script_replace", flecs_command_script_replace, line_vec,
            "Replace text in script, returns the changed lines", {
            { "script", entity_t, false, "Script" },
            { "old", string_t, false,
                "Text to replace, must occur exactly once" },
            { "new", string_t, false, "Replacement text" }}},
        { "script_replace_all", flecs_command_script_replace, line_vec,
            "Replace all occurrences of text in script, returns the changed "
            "lines", {
            { "script", entity_t, false, "Script" },
            { "old", string_t, false, "Text to replace" },
            { "new", string_t, false, "Replacement text" }}, (void*)1 },
        { "script_write", flecs_command_script_write, entity_t,
            "Replace the code of a script, creates the script if it doesn't "
            "exist", {
            { "script", string_t, false, "Script name" },
            { "code", string_t, false, "Script code" }}},
        { "script_patch", flecs_command_script_patch, line_vec,
            "Apply patch to script, returns the lines of the patched hunks", {
            { "script", entity_t, false, "Script" },
            { "patch", string_t, false,
                "Unified diff, '@@ <anchor>' lines may be used instead of line "
                "numbers" }}},
        { "script_set", flecs_command_script_set,
            ecs_id(ecs_command_script_line_t),
            "Set component value in the script that declares the entity", {
            { "entity", entity_t, false, "Entity, $ for singleton" },
            { "component", id_t, false, "Component" },
            { "value", string_t, false, "Value" }}},
        { "script_remove", flecs_command_script_remove, 0,
            "Remove component from the script that declares the entity", {
            { "entity", entity_t, false, "Entity, $ for singleton" },
            { "component", id_t, false, "Component" }}},
        { "script_delete", flecs_command_script_remove, 0,
            "Delete entity from the script that declares it", {
            { "entity", entity_t, false, "Entity" }}, (void*)1 },
        { "script_sync", flecs_command_script_sync, ecs_id(ecs_bool_t),
            "Update script code with the current component values", {
            { "script", entity_t, false, "Script" }}},
        { "script_diff", flecs_command_script_diff, string_t,
            "Changes since the script was loaded or saved", {
            { "script", entity_t, false, "Script" }}},
        { "script_save", flecs_command_script_save, 0, "Save script to file", {
            { "script", entity_t, false, "Script" },
            { "filename", string_t, true,
                "File to save to (default: script file)" }}},
        { "script_reload", flecs_command_script_reload, 0,
            "Reload script from file", {
            { "script", entity_t, false, "Script" }}},
        { "script_undo", flecs_command_script_undo, 0,
            "Undo the last change to the script code", {
            { "script", entity_t, false, "Script" }}},
    };

    flecs_command_register(world, commands,
        ECS_SIZEOF(commands) / ECS_SIZEOF(flecs_command_desc_t));
}

#endif
