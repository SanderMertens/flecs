#include <script.h>

static ecs_entity_t command_session_invoked = 0;
static int32_t command_argc = -1;

typedef struct command_file_t {
    const char *name;
    char content[1024];
    size_t pos;
} command_file_t;

#define COMMAND_FILE_MAX (4)

static command_file_t command_files[COMMAND_FILE_MAX];

static command_file_t* command_file_get(FILE *file) {
    int32_t i;
    for (i = 0; i < COMMAND_FILE_MAX; i ++) {
        if (file == (FILE*)&command_files[i]) {
            return &command_files[i];
        }
    }
    return NULL;
}

static FILE* command_fopen(const char *file, const char *mode) {
    int32_t i;
    for (i = 0; i < COMMAND_FILE_MAX; i ++) {
        if (command_files[i].name && !strcmp(file, command_files[i].name)) {
            break;
        }
    }

    if (i == COMMAND_FILE_MAX) {
        if (!strchr(mode, 'w')) {
            return NULL;
        }

        for (i = 0; i < COMMAND_FILE_MAX; i ++) {
            if (!command_files[i].name) {
                command_files[i].name = file;
                break;
            }
        }

        test_assert(i != COMMAND_FILE_MAX);
    }

    if (strchr(mode, 'w')) {
        command_files[i].content[0] = '\0';
    }

    command_files[i].pos = 0;
    return (FILE*)&command_files[i];
}

static size_t command_fread(void *ptr, size_t size, size_t count, FILE *file) {
    command_file_t *f = command_file_get(file);
    test_assert(f != NULL);
    size_t remaining = strlen(f->content) - f->pos;
    size_t requested = size * count;
    if (requested > remaining) {
        requested = remaining;
    }

    memcpy(ptr, &f->content[f->pos], requested);
    f->pos += requested;
    return requested;
}

static size_t command_fwrite(
    const void *ptr,
    size_t size,
    size_t count,
    FILE *file)
{
    command_file_t *f = command_file_get(file);
    test_assert(f != NULL);
    size_t len = size * count;
    test_assert((f->pos + len) < sizeof(f->content));
    memcpy(&f->content[f->pos], ptr, len);
    f->pos += len;
    f->content[f->pos] = '\0';
    return count;
}

static void command_fclose(FILE *file) {
    test_assert(command_file_get(file) != NULL);
}

void Command_setup(void) {
    memset(command_files, 0, sizeof(command_files));

    ecs_os_set_api_defaults();
    ecs_os_api_t os_api = ecs_os_api;
    os_api.fopen_ = command_fopen;
    os_api.fread_ = command_fread;
    os_api.fwrite_ = command_fwrite;
    os_api.fclose_ = command_fclose;
    ecs_os_set_api(&os_api);
}

static void Command_session_callback(
    const ecs_function_ctx_t *ctx,
    int32_t argc,
    const ecs_value_t *argv,
    ecs_value_t *result)
{
    (void)argv;
    command_argc = argc;
    command_session_invoked = ecs_shell_get_session(ctx->world);
    *(ecs_entity_t*)result->ptr = command_session_invoked;
}

static void Command_fail_callback(
    const ecs_function_ctx_t *ctx,
    int32_t argc,
    const ecs_value_t *argv,
    ecs_value_t *result)
{
    (void)ctx;
    (void)argc;
    (void)argv;
    (void)result;
    ecs_err("custom error");
}

static void Command_function_callback(
    const ecs_function_ctx_t *ctx,
    int32_t argc,
    const ecs_value_t *argv,
    ecs_value_t *result)
{
    (void)argc;
    (void)argv;
    *(ecs_entity_t*)result->ptr = ctx->function;
}

static void Command_to_screen(
    ecs_world_t *world,
    ecs_entity_t entity,
    float local_x,
    float local_y,
    float *screen_x,
    float *screen_y,
    void *ctx)
{
    (void)world;
    (void)entity;
    (void)ctx;
    *screen_x = local_x + 100;
    *screen_y = local_y + 200;
}

void Command_session_init(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptShell);

    ecs_entity_t session = ecs_shell_session_init(world, NULL);
    test_assert(session != 0);

    ecs_entity_t sessions = ecs_lookup(world, "script.shell.sessions");
    test_assert(sessions != 0);
    test_uint(ecs_get_parent(world, ecs_get_parent(world,
        ecs_get_parent(world, sessions))), 0);
    test_assert(!ecs_has_id(world, sessions, EcsModule));
    test_assert(ecs_lookup(world, "flecs.script.shell.sessions") == 0);
    test_uint(ecs_get_parent(world, session), sessions);

    const EcsCommandSession *s = ecs_get(world, session, EcsCommandSession);
    test_assert(s != NULL);
    test_uint(s->scope, 0);
    test_int(s->history_size, FLECS_SCRIPT_SHELL_HISTORY_SIZE);
    test_int(ecs_vec_count(&s->history), 0);

    ecs_fini(world);
}

void Command_session_init_w_desc(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptShell);

    ecs_entity_t e = ecs_new(world);
    ecs_entity_t scope = ecs_entity(world, { .name = "scope" });

    ecs_entity_t session = ecs_shell_session_init(world,
        &(ecs_shell_session_desc_t){
            .entity = e, .scope = scope, .history_size = 10 });
    test_uint(session, e);

    const EcsCommandSession *s = ecs_get(world, session, EcsCommandSession);
    test_assert(s != NULL);
    test_uint(s->scope, scope);
    test_int(s->history_size, 10);

    ecs_fini(world);
}

void Command_get_session_outside_command(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptShell);

    ecs_shell_session_init(world, NULL);
    test_uint(ecs_shell_get_session(world), 0);

    ecs_fini(world);
}

void Command_get_session_in_command(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptShell);

    ecs_function(world, {
        .name = "session",
        .parent = ecs_lookup(world, "flecs.script.shell.commands.core"),
        .return_type = ecs_id(ecs_entity_t),
        .callback = Command_session_callback
    });

    ecs_entity_t session = ecs_shell_session_init(world, NULL);
    ecs_shell_result_t r = ecs_shell_run(world, session, "session");
    test_int(ecs_vec_count(&r.errors), 0);
    test_uint(r.result.type, ecs_id(ecs_entity_t));
    test_uint(*(ecs_entity_t*)r.result.ptr, session);
    test_uint(command_session_invoked, session);
    ecs_shell_result_fini(world, &r);

    test_uint(ecs_shell_get_session(world), 0);

    ecs_fini(world);
}

void Command_custom_command_optional_args(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptShell);

    ecs_function(world, {
        .name = "custom",
        .parent = ecs_lookup(world, "flecs.script.shell.commands.core"),
        .params = {
            { "a", ecs_id(ecs_i32_t) },
            { "b", ecs_id(ecs_i32_t), true },
            { "c", ecs_id(ecs_string_t), true }
        },
        .return_type = ecs_id(ecs_entity_t),
        .callback = Command_session_callback
    });

    ecs_entity_t session = ecs_shell_session_init(world, NULL);

    ecs_shell_result_t r = ecs_shell_run(world, session, "custom 1");
    test_int(ecs_vec_count(&r.errors), 0);
    test_int(command_argc, 1);
    ecs_shell_result_fini(world, &r);

    r = ecs_shell_run(world, session, "custom 1 2");
    test_int(ecs_vec_count(&r.errors), 0);
    test_int(command_argc, 2);
    ecs_shell_result_fini(world, &r);

    r = ecs_shell_run(world, session, "custom 1 2 foo");
    test_int(ecs_vec_count(&r.errors), 0);
    test_int(command_argc, 3);
    ecs_shell_result_fini(world, &r);

    ecs_fini(world);
}

void Command_custom_command_error(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptShell);

    ecs_function(world, {
        .name = "custom",
        .parent = ecs_lookup(world, "flecs.script.shell.commands.core"),
        .callback = Command_fail_callback
    });

    ecs_entity_t session = ecs_shell_session_init(world, NULL);

    ecs_shell_result_t r = ecs_shell_run(world, session, "custom");
    test_uint(r.result.type, 0);
    test_int(ecs_vec_count(&r.errors), 1);
    test_str(ecs_vec_first_t(&r.errors, char*)[0], "custom error");
    ecs_shell_result_fini(world, &r);

    ecs_fini(world);
}

void Command_history(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptShell);

    ecs_entity_t session = ecs_shell_session_init(world, NULL);

    ecs_shell_result_t r = ecs_shell_run(world, session, "pwd");
    ecs_shell_result_fini(world, &r);
    r = ecs_shell_run(world, session, "ls");
    ecs_shell_result_fini(world, &r);
    r = ecs_shell_run(world, session, "unknown");
    ecs_shell_result_fini(world, &r);

    const EcsCommandSession *s = ecs_get(world, session, EcsCommandSession);
    test_int(ecs_vec_count(&s->history), 3);
    test_str(ecs_vec_get_t(&s->history, char*, 0)[0], "pwd");
    test_str(ecs_vec_get_t(&s->history, char*, 1)[0], "ls");
    test_str(ecs_vec_get_t(&s->history, char*, 2)[0], "unknown");

    ecs_fini(world);
}

void Command_history_bounded(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptShell);

    ecs_entity_t session = ecs_shell_session_init(world,
        &(ecs_shell_session_desc_t){ .history_size = 2 });

    ecs_shell_result_t r = ecs_shell_run(world, session, "pwd");
    ecs_shell_result_fini(world, &r);
    r = ecs_shell_run(world, session, "ls");
    ecs_shell_result_fini(world, &r);
    r = ecs_shell_run(world, session, "tree");
    ecs_shell_result_fini(world, &r);

    const EcsCommandSession *s = ecs_get(world, session, EcsCommandSession);
    test_int(ecs_vec_count(&s->history), 2);
    test_str(ecs_vec_get_t(&s->history, char*, 0)[0], "ls");
    test_str(ecs_vec_get_t(&s->history, char*, 1)[0], "tree");

    ecs_fini(world);
}

void Command_history_results(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptShell);

    ecs_entity(world, { .name = "Parent" });

    ecs_entity_t session = ecs_shell_session_init(world, NULL);

    ecs_shell_result_t r = ecs_shell_run(world, session, "cd Parent");
    ecs_shell_result_fini(world, &r);
    r = ecs_shell_run(world, session, "pwd");
    ecs_shell_result_fini(world, &r);
    r = ecs_shell_run(world, session, "unknown");
    ecs_shell_result_fini(world, &r);

    const EcsCommandSession *s = ecs_get(world, session, EcsCommandSession);
    test_int(ecs_vec_count(&s->history), 3);
    test_int(ecs_vec_count(&s->results), 3);
    test_str(ecs_vec_get_t(&s->results, char*, 0)[0],
        "{\"scope\":\"#0\", \"type\":\"flecs.meta.entity\", "
        "\"result\":\"Parent\", \"errors\":[]}");
    test_str(ecs_vec_get_t(&s->results, char*, 1)[0],
        "{\"scope\":\"Parent\", \"type\":\"flecs.meta.entity\", "
        "\"result\":\"Parent\", \"errors\":[]}");
    test_str(ecs_vec_get_t(&s->results, char*, 2)[0],
        "{\"scope\":\"Parent\", \"errors\":[\"unknown command 'unknown'\"]}");

    ecs_fini(world);
}

void Command_history_results_bounded(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptShell);

    ecs_entity_t session = ecs_shell_session_init(world,
        &(ecs_shell_session_desc_t){ .history_size = 2 });

    ecs_shell_result_t r = ecs_shell_run(world, session, "unknown_a");
    ecs_shell_result_fini(world, &r);
    r = ecs_shell_run(world, session, "unknown_b");
    ecs_shell_result_fini(world, &r);
    r = ecs_shell_run(world, session, "unknown_c");
    ecs_shell_result_fini(world, &r);

    const EcsCommandSession *s = ecs_get(world, session, EcsCommandSession);
    test_int(ecs_vec_count(&s->history), 2);
    test_int(ecs_vec_count(&s->results), 2);
    test_str(ecs_vec_get_t(&s->results, char*, 0)[0],
        "{\"scope\":\"#0\", \"errors\":[\"unknown command 'unknown_b'\"]}");
    test_str(ecs_vec_get_t(&s->results, char*, 1)[0],
        "{\"scope\":\"#0\", \"errors\":[\"unknown command 'unknown_c'\"]}");

    ecs_fini(world);
}

void Command_history_results_deleted_scope(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptShell);

    ecs_entity_t parent = ecs_entity(world, { .name = "Parent" });

    ecs_entity_t session = ecs_shell_session_init(world, NULL);

    ecs_shell_result_t r = ecs_shell_run(world, session, "cd Parent");
    ecs_shell_result_fini(world, &r);

    ecs_delete(world, parent);

    r = ecs_shell_run(world, session, "unknown");
    ecs_shell_result_fini(world, &r);

    const EcsCommandSession *s = ecs_get(world, session, EcsCommandSession);
    test_int(ecs_vec_count(&s->results), 2);
    test_str(ecs_vec_get_t(&s->results, char*, 1)[0],
        "{\"scope\":\"#0\", \"errors\":[\"unknown command 'unknown'\"]}");

    ecs_fini(world);
}

void Command_clear(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptShell);

    ecs_entity_t session = ecs_shell_session_init(world, NULL);

    ecs_shell_result_t r = ecs_shell_run(world, session, "pwd");
    ecs_shell_result_fini(world, &r);
    r = ecs_shell_run(world, session, "ls");
    ecs_shell_result_fini(world, &r);

    const EcsCommandSession *s = ecs_get(world, session, EcsCommandSession);
    test_int(ecs_vec_count(&s->history), 2);

    r = ecs_shell_run(world, session, "clear");
    test_int(ecs_vec_count(&r.errors), 0);
    test_uint(r.result.type, 0);
    ecs_shell_result_fini(world, &r);

    s = ecs_get(world, session, EcsCommandSession);
    test_int(ecs_vec_count(&s->history), 0);

    r = ecs_shell_run(world, session, "pwd");
    ecs_shell_result_fini(world, &r);

    s = ecs_get(world, session, EcsCommandSession);
    test_int(ecs_vec_count(&s->history), 1);
    test_str(ecs_vec_get_t(&s->history, char*, 0)[0], "pwd");

    ecs_fini(world);
}

void Command_clear_results(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptShell);

    ecs_entity_t session = ecs_shell_session_init(world, NULL);

    ecs_shell_result_t r = ecs_shell_run(world, session, "pwd");
    ecs_shell_result_fini(world, &r);
    r = ecs_shell_run(world, session, "clear");
    ecs_shell_result_fini(world, &r);

    const EcsCommandSession *s = ecs_get(world, session, EcsCommandSession);
    test_int(ecs_vec_count(&s->history), 0);
    test_int(ecs_vec_count(&s->results), 0);

    r = ecs_shell_run(world, session, "unknown");
    ecs_shell_result_fini(world, &r);

    s = ecs_get(world, session, EcsCommandSession);
    test_int(ecs_vec_count(&s->history), 1);
    test_int(ecs_vec_count(&s->results), 1);
    test_str(ecs_vec_get_t(&s->results, char*, 0)[0],
        "{\"scope\":\"#0\", \"errors\":[\"unknown command 'unknown'\"]}");

    ecs_fini(world);
}

void Command_invalid_session(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptShell);

    ecs_entity_t e = ecs_new(world);
    ecs_shell_result_t r = ecs_shell_run(world, e, "pwd");
    test_uint(r.result.type, 0);
    test_int(ecs_vec_count(&r.errors), 1);
    ecs_shell_result_fini(world, &r);

    ecs_fini(world);
}

void Command_unknown_command(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptShell);

    ecs_entity_t session = ecs_shell_session_init(world, NULL);
    ecs_shell_result_t r = ecs_shell_run(world, session, "foo 10");
    test_uint(r.result.type, 0);
    test_int(ecs_vec_count(&r.errors), 1);
    test_str(ecs_vec_first_t(&r.errors, char*)[0], "unknown command 'foo'");
    ecs_shell_result_fini(world, &r);

    ecs_fini(world);
}

void Command_empty_command(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptShell);

    ecs_entity_t session = ecs_shell_session_init(world, NULL);
    ecs_shell_result_t r = ecs_shell_run(world, session, "   ");
    test_uint(r.result.type, 0);
    test_int(ecs_vec_count(&r.errors), 1);
    test_str(ecs_vec_first_t(&r.errors, char*)[0], "empty command");
    ecs_shell_result_fini(world, &r);

    ecs_fini(world);
}

void Command_too_many_args(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptShell);

    ecs_entity_t session = ecs_shell_session_init(world, NULL);
    ecs_shell_result_t r = ecs_shell_run(world, session, "pwd foo");
    test_uint(r.result.type, 0);
    test_int(ecs_vec_count(&r.errors), 1);
    test_str(ecs_vec_first_t(&r.errors, char*)[0],
        "too many arguments for command 'pwd' (usage: pwd)");
    ecs_shell_result_fini(world, &r);

    ecs_fini(world);
}

void Command_missing_arg(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptShell);

    ecs_entity_t session = ecs_shell_session_init(world, NULL);
    ecs_shell_result_t r = ecs_shell_run(world, session, "get foo");
    test_uint(r.result.type, 0);
    test_int(ecs_vec_count(&r.errors), 1);
    test_str(ecs_vec_first_t(&r.errors, char*)[0],
        "missing argument 'component' for command 'get' "
        "(usage: get <entity> <component>)");
    ecs_shell_result_fini(world, &r);

    ecs_fini(world);
}

void Command_invalid_arg(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptShell);

    ecs_entity_t session = ecs_shell_session_init(world, NULL);
    ecs_shell_result_t r = ecs_shell_run(world, session, "ls foo");
    test_uint(r.result.type, 0);
    test_int(ecs_vec_count(&r.errors), 2);
    test_str(ecs_vec_get_t(&r.errors, char*, 1)[0],
        "invalid argument 'foo' for parameter 'scope' of command 'ls'");
    ecs_shell_result_fini(world, &r);

    ecs_fini(world);
}

void Command_invalid_arg_backtick_string(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptShell);

    ecs_entity_t session = ecs_shell_session_init(world, NULL);
    ecs_shell_result_t r = ecs_shell_run(world, session, "ls `a\\.b`");
    test_uint(r.result.type, 0);
    test_assert(ecs_vec_count(&r.errors) != 0);
    test_str(ecs_vec_last_t(&r.errors, char*)[0],
        "invalid argument '`a\\.b`' for parameter 'scope' of command 'ls'");
    ecs_shell_result_fini(world, &r);

    ecs_fini(world);
}

void Command_quoted_arg(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptShell);

    ecs_entity_t session = ecs_shell_session_init(world, NULL);
    ecs_shell_result_t r = ecs_shell_run(world, session,
        "new \"foo bar\"");
    test_int(ecs_vec_count(&r.errors), 0);
    test_uint(r.result.type, ecs_id(ecs_entity_t));
    ecs_entity_t e = *(ecs_entity_t*)r.result.ptr;
    test_str(ecs_get_name(world, e), "foo bar");
    ecs_shell_result_fini(world, &r);

    ecs_fini(world);
}

void Command_quoted_arg_w_escape(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptShell);

    ecs_entity_t session = ecs_shell_session_init(world, NULL);
    ecs_shell_result_t r = ecs_shell_run(world, session,
        "script_write s \"a {}\\nb { \\\"c\\\" {} }\"");
    test_int(ecs_vec_count(&r.errors), 0);
    ecs_entity_t s = *(ecs_entity_t*)r.result.ptr;
    ecs_shell_result_fini(world, &r);

    const EcsScript *script = ecs_get(world, s, EcsScript);
    test_assert(script != NULL);
    test_str(script->code, "a {}\nb { \"c\" {} }");
    test_assert(ecs_lookup(world, "a") != 0);
    test_assert(ecs_lookup(world, "b.c") != 0);

    ecs_fini(world);
}

void Command_backtick_arg(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptShell);

    ecs_entity_t session = ecs_shell_session_init(world, NULL);
    ecs_shell_result_t r = ecs_shell_run(world, session,
        "script_write s ```a {}\n\"b\" {} // \\n```");
    test_int(ecs_vec_count(&r.errors), 0);
    ecs_entity_t s = *(ecs_entity_t*)r.result.ptr;
    ecs_shell_result_fini(world, &r);

    const EcsScript *script = ecs_get(world, s, EcsScript);
    test_assert(script != NULL);
    test_str(script->code, "a {}\n\"b\" {} // \\n");
    test_assert(ecs_lookup(world, "b") != 0);

    ecs_fini(world);
}

void Command_unterminated_quote(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptShell);

    ecs_entity_t session = ecs_shell_session_init(world, NULL);
    ecs_shell_result_t r = ecs_shell_run(world, session, "new \"foo");
    test_uint(r.result.type, 0);
    test_int(ecs_vec_count(&r.errors), 1);
    test_str(ecs_vec_first_t(&r.errors, char*)[0],
        "missing closing '\"' in command");
    ecs_shell_result_fini(world, &r);

    r = ecs_shell_run(world, session, "new ```foo");
    test_uint(r.result.type, 0);
    test_int(ecs_vec_count(&r.errors), 1);
    test_str(ecs_vec_first_t(&r.errors, char*)[0],
        "missing closing ``` in command");
    ecs_shell_result_fini(world, &r);

    ecs_fini(world);
}

void Command_grouped_arg(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptShell);

    ecs_entity_t session = ecs_shell_session_init(world, NULL);

    ecs_shell_result_t r = ecs_shell_run(world, session, "eval (1 + 2)");
    test_int(ecs_vec_count(&r.errors), 0);
    char *str = ecs_ptr_to_expr(world, r.result.type, r.result.ptr);
    test_str(str, "{i64: 3}");
    ecs_os_free(str);
    ecs_shell_result_fini(world, &r);

    r = ecs_shell_run(world, session, "eval [1, (2 + 3), 4]");
    test_int(ecs_vec_count(&r.errors), 0);
    str = ecs_ptr_to_expr(world, r.result.type, r.result.ptr);
    test_str(str, "{flecs.script.typecache.vector<i64>: [1, 5, 4]}");
    ecs_os_free(str);
    ecs_shell_result_fini(world, &r);

    r = ecs_shell_run(world, session, "eval {x: \"a } b\", y: {1, 2}}.x");
    test_int(ecs_vec_count(&r.errors), 1);
    ecs_shell_result_fini(world, &r);

    ecs_fini(world);
}

void Command_unbalanced_group(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptShell);

    ecs_entity_t session = ecs_shell_session_init(world, NULL);

    ecs_shell_result_t r = ecs_shell_run(world, session, "eval (1 + 2");
    test_uint(r.result.type, 0);
    test_int(ecs_vec_count(&r.errors), 1);
    test_str(ecs_vec_first_t(&r.errors, char*)[0],
        "missing ')' in argument");
    ecs_shell_result_fini(world, &r);

    r = ecs_shell_run(world, session, "eval (1 + 2]");
    test_uint(r.result.type, 0);
    test_int(ecs_vec_count(&r.errors), 1);
    test_str(ecs_vec_first_t(&r.errors, char*)[0],
        "unexpected ']' in argument");
    ecs_shell_result_fini(world, &r);

    ecs_fini(world);
}

void Command_lookup_relative_to_scope(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptShell);

    ecs_entity_t parent = ecs_entity(world, { .name = "parent" });
    ecs_entity_t child = ecs_entity(world, { .name = "parent.child" });
    ecs_entity_t root = ecs_entity(world, { .name = "root" });

    ecs_entity_t session = ecs_shell_session_init(world,
        &(ecs_shell_session_desc_t){ .scope = parent });

    ecs_shell_result_t r = ecs_shell_run(world, session, "man child");
    test_int(ecs_vec_count(&r.errors), 0);
    test_uint(((ecs_shell_doc_info_t*)r.result.ptr)->entity, child);
    ecs_shell_result_fini(world, &r);

    r = ecs_shell_run(world, session, "man root");
    test_int(ecs_vec_count(&r.errors), 0);
    test_uint(((ecs_shell_doc_info_t*)r.result.ptr)->entity, root);
    ecs_shell_result_fini(world, &r);

    r = ecs_shell_run(world, session, "man parent.child");
    test_int(ecs_vec_count(&r.errors), 0);
    test_uint(((ecs_shell_doc_info_t*)r.result.ptr)->entity, child);
    ecs_shell_result_fini(world, &r);

    ecs_fini(world);
}

void Command_command_does_not_set_scope(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptShell);

    ecs_entity_t parent = ecs_entity(world, { .name = "parent" });
    ecs_entity_t session = ecs_shell_session_init(world, NULL);

    ecs_shell_result_t r = ecs_shell_run(world, session, "cd parent");
    test_int(ecs_vec_count(&r.errors), 0);
    test_uint(*(ecs_entity_t*)r.result.ptr, parent);
    ecs_shell_result_fini(world, &r);
    test_uint(ecs_get_scope(world), 0);

    r = ecs_shell_run(world, session, "new child");
    test_int(ecs_vec_count(&r.errors), 0);
    ecs_shell_result_fini(world, &r);
    test_uint(ecs_get_scope(world), 0);

    test_assert(ecs_lookup(world, "parent.child") != 0);
    test_assert(ecs_lookup(world, "child") == 0);

    ecs_fini(world);
}

void Command_singleton_arg(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptShell);

    ecs_script_run(world, NULL, "struct Position(x: f32, y: f32)", NULL);
    ecs_entity_t position = ecs_lookup(world, "Position");
    test_assert(position != 0);

    ecs_entity_t session = ecs_shell_session_init(world, NULL);

    ecs_shell_result_t r = ecs_shell_run(world, session,
        "set $ Position {10, 20}");
    test_int(ecs_vec_count(&r.errors), 0);
    ecs_shell_result_fini(world, &r);

    const Position *p = ecs_get_id(world, position, position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    r = ecs_shell_run(world, session, "get $ Position");
    test_int(ecs_vec_count(&r.errors), 0);
    char *str = ecs_ptr_to_expr(world, r.result.type, r.result.ptr);
    test_str(str, "{Position: {x: 10, y: 20}}");
    ecs_os_free(str);
    ecs_shell_result_fini(world, &r);

    ecs_fini(world);
}

void Command_singleton_arg_no_component(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptShell);

    ecs_entity_t session = ecs_shell_session_init(world, NULL);

    ecs_shell_result_t r = ecs_shell_run(world, session, "cat $");
    test_uint(r.result.type, 0);
    test_int(ecs_vec_count(&r.errors), 1);
    test_str(ecs_vec_first_t(&r.errors, char*)[0],
        "'$' requires a component argument for command 'cat'");
    ecs_shell_result_fini(world, &r);

    ecs_fini(world);
}

void Command_pair_arg(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptShell);

    ecs_entity_t e = ecs_entity(world, { .name = "e" });
    ecs_entity_t rel = ecs_entity(world, { .name = "Rel" });
    ecs_entity_t tgt = ecs_entity(world, { .name = "parent.Tgt" });

    ecs_entity_t session = ecs_shell_session_init(world, NULL);

    ecs_shell_result_t r = ecs_shell_run(world, session,
        "add e (Rel, parent.Tgt)");
    test_int(ecs_vec_count(&r.errors), 0);
    test_uint(r.result.type, ecs_id(ecs_id_t));
    test_uint(*(ecs_id_t*)r.result.ptr, ecs_pair(rel, tgt));
    ecs_shell_result_fini(world, &r);
    test_assert(ecs_has_pair(world, e, rel, tgt));

    r = ecs_shell_run(world, session, "rm e (Rel, parent.Tgt)");
    test_int(ecs_vec_count(&r.errors), 0);
    ecs_shell_result_fini(world, &r);
    test_assert(!ecs_has_pair(world, e, rel, tgt));

    r = ecs_shell_run(world, session, "add e (Rel, Foo)");
    test_uint(r.result.type, 0);
    test_assert(ecs_vec_count(&r.errors) > 0);
    ecs_shell_result_fini(world, &r);

    ecs_fini(world);
}

void Command_help(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptShell);

    ecs_entity_t session = ecs_shell_session_init(world, NULL);

    ecs_shell_result_t r = ecs_shell_run(world, session, "help");
    test_int(ecs_vec_count(&r.errors), 0);
    const EcsVector *v = ecs_get(world, r.result.type, EcsVector);
    test_assert(v != NULL);
    test_uint(v->type, ecs_lookup(world, "flecs.script.shell.CommandInfo"));

    ecs_vec_t *commands = r.result.ptr;
    test_assert(ecs_vec_count(commands) > 50);
    ecs_shell_info_t *info = ecs_vec_first(commands);
    test_str(info[0].name, "add");

    int32_t i, count = ecs_vec_count(commands);
    for (i = 1; i < count; i ++) {
        test_assert(strcmp(info[i - 1].name, info[i].name) < 0);
        test_assert(info[i].brief != NULL);
    }
    ecs_shell_result_fini(world, &r);

    ecs_fini(world);
}

void Command_help_command(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptShell);

    ecs_entity_t session = ecs_shell_session_init(world, NULL);

    ecs_shell_result_t r = ecs_shell_run(world, session, "help grep");
    test_int(ecs_vec_count(&r.errors), 0);
    char *str = ecs_ptr_to_expr(world, r.result.type, r.result.ptr);
    test_str(str, "[{name: \"grep\", brief: \"Search names, component names "
        "and values of descendants\", params: [{name: \"text\", "
        "type: flecs.meta.string, optional: false, brief: \"Text to find\"}, "
        "{name: \"scope\", type: flecs.meta.entity, optional: true, "
        "brief: \"Scope to search (default: session scope)\"}], "
        "return_type: flecs.script.typecache.vector<GrepMatch>}]");
    ecs_os_free(str);
    ecs_shell_result_fini(world, &r);

    r = ecs_shell_run(world, session, "help rm");
    test_int(ecs_vec_count(&r.errors), 0);
    ecs_shell_info_t *info = ecs_vec_first(r.result.ptr);
    test_uint(info->return_type, 0);
    ecs_shell_result_fini(world, &r);

    ecs_fini(world);
}

void Command_help_unknown(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptShell);

    ecs_entity_t session = ecs_shell_session_init(world, NULL);

    ecs_shell_result_t r = ecs_shell_run(world, session, "help foo");
    test_uint(r.result.type, 0);
    test_int(ecs_vec_count(&r.errors), 1);
    test_str(ecs_vec_first_t(&r.errors, char*)[0], "unknown command 'foo'");
    ecs_shell_result_fini(world, &r);

    ecs_fini(world);
}

void Command_ls(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptShell);

    ecs_entity(world, { .name = "parent.a" });
    ecs_entity(world, { .name = "parent.b" });
    ecs_entity(world, { .name = "parent.b.c" });

    ecs_entity_t session = ecs_shell_session_init(world, NULL);

    ecs_shell_result_t r = ecs_shell_run(world, session, "ls parent");
    test_int(ecs_vec_count(&r.errors), 0);
    char *str = ecs_ptr_to_expr(world, r.result.type, r.result.ptr);
    test_str(str, "[parent.a, parent.b]");
    ecs_os_free(str);
    ecs_shell_result_fini(world, &r);

    ecs_fini(world);
}

void Command_ls_root(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptShell);

    ecs_entity_t parent = ecs_entity(world, { .name = "parent" });
    ecs_entity_t child = ecs_entity(world, { .name = "parent.child" });

    ecs_entity_t session = ecs_shell_session_init(world, NULL);

    ecs_shell_result_t r = ecs_shell_run(world, session, "ls");
    test_int(ecs_vec_count(&r.errors), 0);
    ecs_entity_t *entities = ecs_vec_first(r.result.ptr);
    int32_t i, count = ecs_vec_count(r.result.ptr);
    bool has_flecs = false, has_parent = false, has_child = false;
    for (i = 0; i < count; i ++) {
        has_flecs |= entities[i] == EcsFlecs;
        has_parent |= entities[i] == parent;
        has_child |= entities[i] == child;
    }
    test_bool(has_flecs, true);
    test_bool(has_parent, true);
    test_bool(has_child, false);
    ecs_shell_result_fini(world, &r);

    ecs_fini(world);
}

void Command_ls_session_scope(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptShell);

    ecs_entity_t parent = ecs_entity(world, { .name = "parent" });
    ecs_entity(world, { .name = "parent.a" });

    ecs_entity_t session = ecs_shell_session_init(world,
        &(ecs_shell_session_desc_t){ .scope = parent });

    ecs_shell_result_t r = ecs_shell_run(world, session, "ls");
    test_int(ecs_vec_count(&r.errors), 0);
    char *str = ecs_ptr_to_expr(world, r.result.type, r.result.ptr);
    test_str(str, "[parent.a]");
    ecs_os_free(str);
    ecs_shell_result_fini(world, &r);

    ecs_fini(world);
}

void Command_invalid_entity(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptShell);

    ecs_entity_t session = ecs_shell_session_init(world, NULL);

    ecs_shell_result_t r = ecs_shell_run(world, session, "cat 0");
    test_uint(r.result.type, 0);
    test_int(ecs_vec_count(&r.errors), 1);
    test_str(ecs_vec_first_t(&r.errors, char*)[0], "entity #0 is not alive");
    ecs_shell_result_fini(world, &r);

    ecs_fini(world);
}

void Command_tree(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptShell);

    ecs_entity(world, { .name = "parent.a" });
    ecs_entity(world, { .name = "parent.a.b" });
    ecs_entity(world, { .name = "parent.c" });

    ecs_entity_t session = ecs_shell_session_init(world, NULL);

    ecs_shell_result_t r = ecs_shell_run(world, session, "tree parent");
    test_int(ecs_vec_count(&r.errors), 0);
    char *str = ecs_ptr_to_expr(world, r.result.type, r.result.ptr);
    test_str(str, "[{entity: parent.a, depth: 0}, "
        "{entity: parent.a.b, depth: 1}, {entity: parent.c, depth: 0}]");
    ecs_os_free(str);
    ecs_shell_result_fini(world, &r);

    ecs_fini(world);
}

void Command_tree_root_skips_flecs(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptShell);

    ecs_entity_t parent = ecs_entity(world, { .name = "parent" });
    ecs_entity_t child = ecs_entity(world, { .name = "parent.child" });

    ecs_entity_t session = ecs_shell_session_init(world, NULL);

    ecs_shell_result_t r = ecs_shell_run(world, session, "tree");
    test_int(ecs_vec_count(&r.errors), 0);
    ecs_shell_tree_node_t *nodes = ecs_vec_first(r.result.ptr);
    int32_t i, count = ecs_vec_count(r.result.ptr);
    bool has_flecs = false, has_core = false, has_child = false;
    for (i = 0; i < count; i ++) {
        has_flecs |= nodes[i].entity == EcsFlecs;
        has_core |= nodes[i].entity == EcsFlecsCore;
        if (nodes[i].entity == child) {
            has_child = true;
            test_int(nodes[i].depth, 1);
            test_uint(nodes[i - 1].entity, parent);
        }
    }
    test_bool(has_flecs, true);
    test_bool(has_core, false);
    test_bool(has_child, true);
    ecs_shell_result_fini(world, &r);

    ecs_fini(world);
}

void Command_cat(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptShell);

    ecs_script_run(world, NULL,
        HEAD "struct Position(x: f32, y: f32)"
        LINE "Tag {}"
        LINE "e { Position: {1, 2}; Tag }", NULL);
    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t tag = ecs_lookup(world, "Tag");

    ecs_entity_t session = ecs_shell_session_init(world, NULL);

    ecs_shell_result_t r = ecs_shell_run(world, session, "cat e");
    test_int(ecs_vec_count(&r.errors), 0);
    test_uint(r.result.type, ecs_lookup(world, "flecs.script.shell.EntityInfo"));
    ecs_shell_entity_info_t *info = r.result.ptr;
    test_uint(info->entity, e);
    test_uint(info->id, e);

    bool has_position = false, has_tag = false;
    ecs_shell_component_value_t *cv = ecs_vec_first(&info->components);
    int32_t i, count = ecs_vec_count(&info->components);
    for (i = 0; i < count; i ++) {
        if (cv[i].id == ecs_lookup(world, "Position")) {
            char *str = ecs_ptr_to_expr(
                world, cv[i].value.type, cv[i].value.ptr);
            test_str(str, "{x: 1, y: 2}");
            ecs_os_free(str);
            has_position = true;
        }
    }

    ecs_id_t *ids = ecs_vec_first(&info->ids);
    count = ecs_vec_count(&info->ids);
    for (i = 0; i < count; i ++) {
        has_tag |= ids[i] == tag;
    }

    test_bool(has_position, true);
    test_bool(has_tag, true);
    ecs_shell_result_fini(world, &r);

    ecs_fini(world);
}

void Command_schema(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptShell);

    ecs_script_run(world, NULL,
        HEAD "struct Position(x: f32, y: f32)"
        LINE "struct Line(start: Position, stop: Position)", NULL);

    ecs_entity_t session = ecs_shell_session_init(world, NULL);

    ecs_shell_result_t r = ecs_shell_run(world, session, "schema Line");
    test_int(ecs_vec_count(&r.errors), 0);
    test_uint(r.result.type, ecs_id(ecs_string_t));
    test_str(*(char**)r.result.ptr,
        HEAD "struct Position(x: f32, y: f32)"
        LINE "struct Line(start: Position, stop: Position)"
        LINE);
    ecs_shell_result_fini(world, &r);

    ecs_fini(world);
}

void Command_schema_not_a_type(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptShell);

    ecs_entity(world, { .name = "Tag" });
    ecs_entity_t session = ecs_shell_session_init(world, NULL);

    ecs_shell_result_t r = ecs_shell_run(world, session, "schema Tag");
    test_uint(r.result.type, 0);
    test_int(ecs_vec_count(&r.errors), 1);
    test_str(ecs_vec_first_t(&r.errors, char*)[0], "'Tag' is not a type");
    ecs_shell_result_fini(world, &r);

    ecs_fini(world);
}

void Command_schema_value(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptShell);

    ecs_script_run(world, NULL, "struct Position(x: f32, y: f32)", NULL);

    ecs_entity_t session = ecs_shell_session_init(world, NULL);

    ecs_shell_result_t r = ecs_shell_run(world, session,
        "schema_value Position");
    test_int(ecs_vec_count(&r.errors), 0);
    test_uint(r.result.type, ecs_id(ecs_value_t));
    char *str = ecs_ptr_to_expr(world, r.result.type, r.result.ptr);
    test_str(str, "{Position: {x: 0, y: 0}}");
    ecs_os_free(str);
    ecs_shell_result_fini(world, &r);

    ecs_fini(world);
}

void Command_cd_pwd(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptShell);

    ecs_entity_t parent = ecs_entity(world, { .name = "parent" });
    ecs_entity_t child = ecs_entity(world, { .name = "parent.child" });

    ecs_entity_t session = ecs_shell_session_init(world, NULL);

    ecs_shell_result_t r = ecs_shell_run(world, session, "pwd");
    test_int(ecs_vec_count(&r.errors), 0);
    test_uint(*(ecs_entity_t*)r.result.ptr, 0);
    ecs_shell_result_fini(world, &r);

    r = ecs_shell_run(world, session, "cd parent");
    test_int(ecs_vec_count(&r.errors), 0);
    test_uint(*(ecs_entity_t*)r.result.ptr, parent);
    ecs_shell_result_fini(world, &r);

    r = ecs_shell_run(world, session, "cd child");
    test_int(ecs_vec_count(&r.errors), 0);
    test_uint(*(ecs_entity_t*)r.result.ptr, child);
    ecs_shell_result_fini(world, &r);

    r = ecs_shell_run(world, session, "pwd");
    test_int(ecs_vec_count(&r.errors), 0);
    test_uint(*(ecs_entity_t*)r.result.ptr, child);
    ecs_shell_result_fini(world, &r);

    test_uint(ecs_get(world, session, EcsCommandSession)->scope, child);

    r = ecs_shell_run(world, session, "cd");
    test_int(ecs_vec_count(&r.errors), 0);
    test_uint(*(ecs_entity_t*)r.result.ptr, 0);
    ecs_shell_result_fini(world, &r);

    test_uint(ecs_get(world, session, EcsCommandSession)->scope, 0);

    ecs_fini(world);
}

void Command_cd_parent(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptShell);

    ecs_entity_t parent = ecs_entity(world, { .name = "parent" });
    ecs_entity_t child = ecs_entity(world, { .name = "parent.child" });

    ecs_shell_session_desc_t desc = { .scope = child };
    ecs_entity_t session = ecs_shell_session_init(world, &desc);

    ecs_shell_result_t r = ecs_shell_run(world, session, "cd ..");
    test_int(ecs_vec_count(&r.errors), 0);
    test_uint(*(ecs_entity_t*)r.result.ptr, parent);
    ecs_shell_result_fini(world, &r);

    test_uint(ecs_get(world, session, EcsCommandSession)->scope, parent);

    r = ecs_shell_run(world, session, "cd ..");
    test_int(ecs_vec_count(&r.errors), 0);
    test_uint(*(ecs_entity_t*)r.result.ptr, 0);
    ecs_shell_result_fini(world, &r);

    test_uint(ecs_get(world, session, EcsCommandSession)->scope, 0);

    ecs_fini(world);
}

void Command_cd_parent_parent(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptShell);

    ecs_entity_t parent = ecs_entity(world, { .name = "parent" });
    ecs_entity_t grandchild = ecs_entity(world, {
        .name = "parent.child.grandchild" });

    ecs_shell_session_desc_t desc = { .scope = grandchild };
    ecs_entity_t session = ecs_shell_session_init(world, &desc);

    ecs_shell_result_t r = ecs_shell_run(world, session, "cd ../../");
    test_int(ecs_vec_count(&r.errors), 0);
    test_uint(*(ecs_entity_t*)r.result.ptr, parent);
    ecs_shell_result_fini(world, &r);

    test_uint(ecs_get(world, session, EcsCommandSession)->scope, parent);

    r = ecs_shell_run(world, session, "cd ./child/grandchild");
    test_int(ecs_vec_count(&r.errors), 0);
    test_uint(*(ecs_entity_t*)r.result.ptr, grandchild);
    ecs_shell_result_fini(world, &r);

    ecs_fini(world);
}

void Command_cd_parent_sibling(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptShell);

    ecs_entity_t a = ecs_entity(world, { .name = "parent.a" });
    ecs_entity_t b = ecs_entity(world, { .name = "parent.b" });

    ecs_shell_session_desc_t desc = { .scope = a };
    ecs_entity_t session = ecs_shell_session_init(world, &desc);

    ecs_shell_result_t r = ecs_shell_run(world, session, "cd ../b");
    test_int(ecs_vec_count(&r.errors), 0);
    test_uint(*(ecs_entity_t*)r.result.ptr, b);
    ecs_shell_result_fini(world, &r);

    test_uint(ecs_get(world, session, EcsCommandSession)->scope, b);

    ecs_fini(world);
}

void Command_cd_parent_sibling_child(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptShell);

    ecs_entity_t a = ecs_entity(world, { .name = "parent.a" });
    ecs_entity_t c = ecs_entity(world, { .name = "parent.b.c" });

    ecs_shell_session_desc_t desc = { .scope = a };
    ecs_entity_t session = ecs_shell_session_init(world, &desc);

    ecs_shell_result_t r = ecs_shell_run(world, session, "cd ../b/c");
    test_int(ecs_vec_count(&r.errors), 0);
    test_uint(*(ecs_entity_t*)r.result.ptr, c);
    ecs_shell_result_fini(world, &r);

    test_uint(ecs_get(world, session, EcsCommandSession)->scope, c);

    ecs_fini(world);
}

void Command_cd_parent_at_root(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptShell);

    ecs_entity_t parent = ecs_entity(world, { .name = "parent" });

    ecs_entity_t session = ecs_shell_session_init(world, NULL);

    ecs_shell_result_t r = ecs_shell_run(world, session, "cd ../..");
    test_int(ecs_vec_count(&r.errors), 0);
    test_uint(*(ecs_entity_t*)r.result.ptr, 0);
    ecs_shell_result_fini(world, &r);

    r = ecs_shell_run(world, session, "cd ../parent");
    test_int(ecs_vec_count(&r.errors), 0);
    test_uint(*(ecs_entity_t*)r.result.ptr, parent);
    ecs_shell_result_fini(world, &r);

    ecs_fini(world);
}

void Command_cd_current(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptShell);

    ecs_entity_t parent = ecs_entity(world, { .name = "parent" });
    ecs_entity_t child = ecs_entity(world, { .name = "parent.child" });

    ecs_shell_session_desc_t desc = { .scope = parent };
    ecs_entity_t session = ecs_shell_session_init(world, &desc);

    ecs_shell_result_t r = ecs_shell_run(world, session, "cd .");
    test_int(ecs_vec_count(&r.errors), 0);
    test_uint(*(ecs_entity_t*)r.result.ptr, parent);
    ecs_shell_result_fini(world, &r);

    r = ecs_shell_run(world, session, "cd ./child");
    test_int(ecs_vec_count(&r.errors), 0);
    test_uint(*(ecs_entity_t*)r.result.ptr, child);
    ecs_shell_result_fini(world, &r);

    ecs_fini(world);
}

void Command_cd_relative_not_found(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptShell);

    ecs_entity_t a = ecs_entity(world, { .name = "parent.a" });

    ecs_shell_session_desc_t desc = { .scope = a };
    ecs_entity_t session = ecs_shell_session_init(world, &desc);

    ecs_shell_result_t r = ecs_shell_run(world, session, "cd ../b");
    test_uint(r.result.type, 0);
    test_int(ecs_vec_count(&r.errors), 1);
    test_str(ecs_vec_first_t(&r.errors, char*)[0],
        "invalid argument '../b' for parameter 'scope' of command 'cd'");
    ecs_shell_result_fini(world, &r);

    test_uint(ecs_get(world, session, EcsCommandSession)->scope, a);

    ecs_fini(world);
}

void Command_ls_parent(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptShell);

    ecs_entity_t a = ecs_entity(world, { .name = "parent.a" });
    ecs_entity(world, { .name = "parent.b" });

    ecs_shell_session_desc_t desc = { .scope = a };
    ecs_entity_t session = ecs_shell_session_init(world, &desc);

    ecs_shell_result_t r = ecs_shell_run(world, session, "ls ..");
    test_int(ecs_vec_count(&r.errors), 0);
    char *str = ecs_ptr_to_expr(world, r.result.type, r.result.ptr);
    test_str(str, "[parent.a, parent.b]");
    ecs_os_free(str);
    ecs_shell_result_fini(world, &r);

    ecs_fini(world);
}

void Command_get(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptShell);

    ecs_script_run(world, NULL,
        HEAD "struct Position(x: f32, y: f32)"
        LINE "e { Position: {1, 2} }", NULL);

    ecs_entity_t session = ecs_shell_session_init(world, NULL);

    ecs_shell_result_t r = ecs_shell_run(world, session, "get e Position");
    test_int(ecs_vec_count(&r.errors), 0);
    test_uint(r.result.type, ecs_id(ecs_value_t));
    ecs_value_t *v = r.result.ptr;
    test_uint(v->type, ecs_lookup(world, "Position"));
    test_int(((Position*)v->ptr)->x, 1);
    test_int(((Position*)v->ptr)->y, 2);
    ecs_shell_result_fini(world, &r);

    ecs_fini(world);
}

void Command_get_not_present(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptShell);

    ecs_script_run(world, NULL,
        HEAD "struct Position(x: f32, y: f32)"
        LINE "Tag {}"
        LINE "e { Tag }", NULL);

    ecs_entity_t session = ecs_shell_session_init(world, NULL);

    ecs_shell_result_t r = ecs_shell_run(world, session, "get e Position");
    test_uint(r.result.type, 0);
    test_int(ecs_vec_count(&r.errors), 1);
    test_str(ecs_vec_first_t(&r.errors, char*)[0],
        "entity 'e' does not have component 'Position'");
    ecs_shell_result_fini(world, &r);

    r = ecs_shell_run(world, session, "get e Tag");
    test_uint(r.result.type, 0);
    test_int(ecs_vec_count(&r.errors), 1);
    test_str(ecs_vec_first_t(&r.errors, char*)[0],
        "'Tag' is not a component with a value");
    ecs_shell_result_fini(world, &r);

    ecs_fini(world);
}

void Command_set(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptShell);

    ecs_script_run(world, NULL, "struct Position(x: f32, y: f32)", NULL);
    ecs_entity_t position = ecs_lookup(world, "Position");
    ecs_entity_t e = ecs_entity(world, { .name = "e" });

    ecs_entity_t session = ecs_shell_session_init(world, NULL);

    ecs_shell_result_t r = ecs_shell_run(world, session,
        "set e Position {10, 20}");
    test_int(ecs_vec_count(&r.errors), 0);
    char *str = ecs_ptr_to_expr(world, r.result.type, r.result.ptr);
    test_str(str, "{Position: {x: 10, y: 20}}");
    ecs_os_free(str);
    ecs_shell_result_fini(world, &r);

    const Position *p = ecs_get_id(world, e, position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void Command_set_partial(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptShell);

    ecs_script_run(world, NULL,
        HEAD "struct Position(x: f32, y: f32)"
        LINE "e { Position: {1, 2} }", NULL);
    ecs_entity_t position = ecs_lookup(world, "Position");
    ecs_entity_t e = ecs_lookup(world, "e");

    ecs_entity_t session = ecs_shell_session_init(world, NULL);

    ecs_shell_result_t r = ecs_shell_run(world, session,
        "set e Position {y: 20}");
    test_int(ecs_vec_count(&r.errors), 0);
    ecs_shell_result_fini(world, &r);

    const Position *p = ecs_get_id(world, e, position);
    test_int(p->x, 1);
    test_int(p->y, 20);

    r = ecs_shell_run(world, session, "set e Position {x: 10}");
    test_int(ecs_vec_count(&r.errors), 0);
    ecs_shell_result_fini(world, &r);

    p = ecs_get_id(world, e, position);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void Command_set_nested_partial(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptShell);

    ecs_script_run(world, NULL,
        HEAD "struct Position(x: f32, y: f32)"
        LINE "struct Line(start: Position, stop: Position)"
        LINE "e { Line: {{1, 2}, {3, 4}} }", NULL);

    ecs_entity_t session = ecs_shell_session_init(world, NULL);

    ecs_shell_result_t r = ecs_shell_run(world, session,
        "set e Line {stop: {x: 30, y: 40}}");
    test_int(ecs_vec_count(&r.errors), 0);
    char *str = ecs_ptr_to_expr(world, r.result.type, r.result.ptr);
    test_str(str, "{Line: {start: {x: 1, y: 2}, stop: {x: 30, y: 40}}}");
    ecs_os_free(str);
    ecs_shell_result_fini(world, &r);

    ecs_fini(world);
}

void Command_set_invalid_value(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptShell);

    ecs_script_run(world, NULL,
        HEAD "struct Position(x: f32, y: f32)"
        LINE "e { Position: {1, 2} }", NULL);
    ecs_entity_t position = ecs_lookup(world, "Position");
    ecs_entity_t e = ecs_lookup(world, "e");

    ecs_entity_t session = ecs_shell_session_init(world, NULL);

    ecs_shell_result_t r = ecs_shell_run(world, session,
        "set e Position {z: 10}");
    test_uint(r.result.type, 0);
    test_assert(ecs_vec_count(&r.errors) > 0);
    ecs_shell_result_fini(world, &r);

    const Position *p = ecs_get_id(world, e, position);
    test_int(p->x, 1);
    test_int(p->y, 2);

    ecs_fini(world);
}

void Command_add_remove(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptShell);

    ecs_entity_t e = ecs_entity(world, { .name = "e" });
    ecs_entity_t tag = ecs_entity(world, { .name = "Tag" });

    ecs_entity_t session = ecs_shell_session_init(world, NULL);

    ecs_shell_result_t r = ecs_shell_run(world, session, "add e Tag");
    test_int(ecs_vec_count(&r.errors), 0);
    test_uint(*(ecs_id_t*)r.result.ptr, tag);
    ecs_shell_result_fini(world, &r);
    test_assert(ecs_has_id(world, e, tag));

    r = ecs_shell_run(world, session, "rm e Tag");
    test_int(ecs_vec_count(&r.errors), 0);
    test_uint(r.result.type, 0);
    ecs_shell_result_fini(world, &r);
    test_assert(!ecs_has_id(world, e, tag));

    ecs_fini(world);
}

void Command_new(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptShell);

    ecs_entity_t session = ecs_shell_session_init(world, NULL);

    ecs_shell_result_t r = ecs_shell_run(world, session, "new");
    test_int(ecs_vec_count(&r.errors), 0);
    ecs_entity_t e = *(ecs_entity_t*)r.result.ptr;
    test_assert(e != 0);
    test_assert(ecs_is_alive(world, e));
    test_uint(ecs_get_parent(world, e), 0);
    ecs_shell_result_fini(world, &r);

    r = ecs_shell_run(world, session, "new foo");
    test_int(ecs_vec_count(&r.errors), 0);
    test_uint(*(ecs_entity_t*)r.result.ptr, ecs_lookup(world, "foo"));
    ecs_shell_result_fini(world, &r);

    ecs_fini(world);
}

void Command_new_in_scope(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptShell);

    ecs_entity_t parent = ecs_entity(world, { .name = "parent" });
    ecs_entity_t session = ecs_shell_session_init(world,
        &(ecs_shell_session_desc_t){ .scope = parent });

    ecs_shell_result_t r = ecs_shell_run(world, session, "new");
    test_int(ecs_vec_count(&r.errors), 0);
    test_uint(ecs_get_parent(world, *(ecs_entity_t*)r.result.ptr), parent);
    ecs_shell_result_fini(world, &r);

    r = ecs_shell_run(world, session, "new foo");
    test_int(ecs_vec_count(&r.errors), 0);
    test_uint(*(ecs_entity_t*)r.result.ptr, ecs_lookup(world, "parent.foo"));
    ecs_shell_result_fini(world, &r);

    ecs_fini(world);
}

void Command_new_existing(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptShell);

    ecs_entity(world, { .name = "foo" });
    ecs_entity_t session = ecs_shell_session_init(world, NULL);

    ecs_shell_result_t r = ecs_shell_run(world, session, "new foo");
    test_uint(r.result.type, 0);
    test_int(ecs_vec_count(&r.errors), 1);
    test_str(ecs_vec_first_t(&r.errors, char*)[0],
        "entity 'foo' already exists");
    ecs_shell_result_fini(world, &r);

    ecs_fini(world);
}

void Command_delete(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptShell);

    ecs_entity_t e = ecs_entity(world, { .name = "e" });
    ecs_entity_t session = ecs_shell_session_init(world, NULL);

    ecs_shell_result_t r = ecs_shell_run(world, session, "rm e");
    test_int(ecs_vec_count(&r.errors), 0);
    test_uint(r.result.type, 0);
    ecs_shell_result_fini(world, &r);
    test_assert(!ecs_is_alive(world, e));

    ecs_fini(world);
}

void Command_mv(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptShell);

    ecs_entity_t e = ecs_entity(world, { .name = "e" });
    ecs_entity_t parent = ecs_entity(world, { .name = "parent" });
    ecs_entity_t session = ecs_shell_session_init(world, NULL);

    ecs_shell_result_t r = ecs_shell_run(world, session, "mv e f");
    test_int(ecs_vec_count(&r.errors), 0);
    test_uint(*(ecs_entity_t*)r.result.ptr, e);
    ecs_shell_result_fini(world, &r);
    test_str(ecs_get_name(world, e), "f");
    test_uint(ecs_get_parent(world, e), 0);

    r = ecs_shell_run(world, session, "mv f parent.g");
    test_int(ecs_vec_count(&r.errors), 0);
    ecs_shell_result_fini(world, &r);
    test_str(ecs_get_name(world, e), "g");
    test_uint(ecs_get_parent(world, e), parent);
    test_uint(ecs_lookup(world, "parent.g"), e);

    r = ecs_shell_run(world, session, "mv parent.g h");
    test_int(ecs_vec_count(&r.errors), 0);
    ecs_shell_result_fini(world, &r);
    test_uint(ecs_get_parent(world, e), 0);
    test_uint(ecs_lookup(world, "h"), e);

    ecs_fini(world);
}

void Command_mv_errors(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptShell);

    ecs_entity_t e = ecs_entity(world, { .name = "e" });
    ecs_entity(world, { .name = "e.child" });
    ecs_entity(world, { .name = "f" });
    ecs_entity_t session = ecs_shell_session_init(world, NULL);

    ecs_shell_result_t r = ecs_shell_run(world, session, "mv e foo.e");
    test_uint(r.result.type, 0);
    test_int(ecs_vec_count(&r.errors), 1);
    test_str(ecs_vec_first_t(&r.errors, char*)[0],
        "parent 'foo' does not exist");
    ecs_shell_result_fini(world, &r);

    r = ecs_shell_run(world, session, "mv e e.child.e");
    test_uint(r.result.type, 0);
    test_int(ecs_vec_count(&r.errors), 1);
    test_str(ecs_vec_first_t(&r.errors, char*)[0],
        "cannot move entity into itself");
    ecs_shell_result_fini(world, &r);

    r = ecs_shell_run(world, session, "mv e f");
    test_uint(r.result.type, 0);
    test_int(ecs_vec_count(&r.errors), 1);
    test_str(ecs_vec_first_t(&r.errors, char*)[0],
        "entity 'f' already exists");
    ecs_shell_result_fini(world, &r);

    test_str(ecs_get_name(world, e), "e");

    ecs_fini(world);
}

void Command_man(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptShell);

    ecs_entity_t e = ecs_entity(world, { .name = "e" });
    ecs_doc_set_name(world, e, "Entity");
    ecs_doc_set_brief(world, e, "Brief");
    ecs_doc_set_detail(world, e, "Detail");
    ecs_doc_set_link(world, e, "https://flecs.dev");
    ecs_doc_set_color(world, e, "#ff0000");

    ecs_entity_t session = ecs_shell_session_init(world, NULL);

    ecs_shell_result_t r = ecs_shell_run(world, session, "man e");
    test_int(ecs_vec_count(&r.errors), 0);
    char *str = ecs_ptr_to_expr(world, r.result.type, r.result.ptr);
    test_str(str, "{entity: e, name: \"Entity\", brief: \"Brief\", "
        "detail: \"Detail\", link: \"https://flecs.dev\", color: \"#ff0000\"}");
    ecs_os_free(str);
    ecs_shell_result_fini(world, &r);

    ecs_fini(world);
}

void Command_eval(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptShell);

    ecs_entity_t session = ecs_shell_session_init(world, NULL);

    ecs_shell_result_t r = ecs_shell_run(world, session,
        "eval ```10 * 2 + 1```");
    test_int(ecs_vec_count(&r.errors), 0);
    test_uint(r.result.type, ecs_id(ecs_value_t));
    ecs_value_t *v = r.result.ptr;
    test_uint(v->type, ecs_id(ecs_i64_t));
    test_int(*(int64_t*)v->ptr, 21);
    ecs_shell_result_fini(world, &r);

    r = ecs_shell_run(world, session, "eval ```\"Hello\"```");
    test_int(ecs_vec_count(&r.errors), 0);
    v = r.result.ptr;
    test_uint(v->type, ecs_id(ecs_string_t));
    test_str(*(char**)v->ptr, "Hello");
    ecs_shell_result_fini(world, &r);

    ecs_fini(world);
}

void Command_eval_error(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptShell);

    ecs_entity_t session = ecs_shell_session_init(world, NULL);

    ecs_shell_result_t r = ecs_shell_run(world, session, "eval \"1 +\"");
    test_uint(r.result.type, 0);
    test_assert(ecs_vec_count(&r.errors) > 0);
    ecs_shell_result_fini(world, &r);

    ecs_fini(world);
}

void Command_query(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptShell);

    ecs_script_run(world, NULL,
        HEAD "struct Position(x: f32, y: f32)"
        LINE "Tag {}"
        LINE "a { Position: {1, 2} }"
        LINE "b { Position: {3, 4}; Tag }", NULL);

    ecs_entity_t session = ecs_shell_session_init(world, NULL);

    ecs_shell_result_t r = ecs_shell_run(world, session,
        "query \"Position, Tag\"");
    test_int(ecs_vec_count(&r.errors), 0);
    char *str = ecs_ptr_to_expr(world, r.result.type, r.result.ptr);
    test_str(str, "[{entity: b, fields: [{id: Position, value: "
        "{Position: {x: 3, y: 4}}}]}]");
    ecs_os_free(str);
    ecs_shell_result_fini(world, &r);

    r = ecs_shell_run(world, session, "query Position");
    test_int(ecs_vec_count(&r.errors), 0);
    test_int(ecs_vec_count(r.result.ptr), 2);
    ecs_shell_result_fini(world, &r);

    ecs_fini(world);
}

void Command_query_error(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptShell);

    ecs_entity_t session = ecs_shell_session_init(world, NULL);

    ecs_shell_result_t r = ecs_shell_run(world, session, "query Foo");
    test_uint(r.result.type, 0);
    test_assert(ecs_vec_count(&r.errors) > 0);
    ecs_shell_result_fini(world, &r);

    ecs_fini(world);
}

void Command_quit(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptShell);

    ecs_entity_t session = ecs_shell_session_init(world, NULL);
    test_bool(ecs_should_quit(world), false);

    ecs_shell_result_t r = ecs_shell_run(world, session, "quit");
    test_int(ecs_vec_count(&r.errors), 0);
    test_uint(r.result.type, 0);
    ecs_shell_result_fini(world, &r);

    test_bool(ecs_should_quit(world), true);

    ecs_fini(world);
}

void Command_find(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptShell);

    ecs_entity(world, { .name = "parent.player_one" });
    ecs_entity(world, { .name = "parent.enemy" });
    ecs_entity(world, { .name = "other.player_two" });

    ecs_entity_t session = ecs_shell_session_init(world, NULL);

    ecs_shell_result_t r = ecs_shell_run(world, session, "find player");
    test_int(ecs_vec_count(&r.errors), 0);
    char *str = ecs_ptr_to_expr(world, r.result.type, r.result.ptr);
    test_str(str, "[parent.player_one, other.player_two]");
    ecs_os_free(str);
    ecs_shell_result_fini(world, &r);

    r = ecs_shell_run(world, session, "cd parent");
    ecs_shell_result_fini(world, &r);

    r = ecs_shell_run(world, session, "find player");
    test_int(ecs_vec_count(&r.errors), 0);
    str = ecs_ptr_to_expr(world, r.result.type, r.result.ptr);
    test_str(str, "[parent.player_one]");
    ecs_os_free(str);
    ecs_shell_result_fini(world, &r);

    ecs_fini(world);
}

void Command_find_root_skips_flecs(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptShell);

    ecs_entity_t session = ecs_shell_session_init(world, NULL);

    ecs_shell_result_t r = ecs_shell_run(world, session, "find Identifier");
    test_int(ecs_vec_count(&r.errors), 0);
    test_int(ecs_vec_count(r.result.ptr), 0);
    ecs_shell_result_fini(world, &r);

    ecs_fini(world);
}

void Command_grep(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptShell);

    ecs_script_run(world, NULL,
        HEAD "struct Position(x: f32, y: f32)"
        LINE "TagFoo {}"
        LINE "scene {"
        LINE "  foo_entity {}"
        LINE "  a { TagFoo }"
        LINE "  b { Position: {42, 0} }"
        LINE "}", NULL);

    ecs_entity_t session = ecs_shell_session_init(world, NULL);

    ecs_shell_result_t r = ecs_shell_run(world, session, "grep foo scene");
    test_int(ecs_vec_count(&r.errors), 0);
    char *str = ecs_ptr_to_expr(world, r.result.type, r.result.ptr);
    test_str(str, "[{entity: scene.foo_entity, id: #0, text: \"foo_entity\"}]");
    ecs_os_free(str);
    ecs_shell_result_fini(world, &r);

    r = ecs_shell_run(world, session, "grep Foo scene");
    test_int(ecs_vec_count(&r.errors), 0);
    str = ecs_ptr_to_expr(world, r.result.type, r.result.ptr);
    test_str(str, "[{entity: scene.a, id: TagFoo, text: \"TagFoo\"}]");
    ecs_os_free(str);
    ecs_shell_result_fini(world, &r);

    r = ecs_shell_run(world, session, "grep 42 scene");
    test_int(ecs_vec_count(&r.errors), 0);
    str = ecs_ptr_to_expr(world, r.result.type, r.result.ptr);
    test_str(str, "[{entity: scene.b, id: Position, text: \"{x: 42, y: 0}\"}]");
    ecs_os_free(str);
    ecs_shell_result_fini(world, &r);

    ecs_fini(world);
}

void Command_grep_i(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptShell);

    ecs_script_run(world, NULL,
        HEAD "TagFoo {}"
        LINE "scene {"
        LINE "  foo_entity {}"
        LINE "  a { TagFoo }"
        LINE "}", NULL);

    ecs_entity_t session = ecs_shell_session_init(world, NULL);

    ecs_shell_result_t r = ecs_shell_run(world, session,
        "grep_i FOO scene");
    test_int(ecs_vec_count(&r.errors), 0);
    char *str = ecs_ptr_to_expr(world, r.result.type, r.result.ptr);
    test_str(str, "[{entity: scene.foo_entity, id: #0, text: \"foo_entity\"}, "
        "{entity: scene.a, id: TagFoo, text: \"TagFoo\"}]");
    ecs_os_free(str);
    ecs_shell_result_fini(world, &r);

    ecs_fini(world);
}

void Command_scripts(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptShell);

    ecs_log_set_level(-4);

    ecs_entity_t a = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "a" }),
        .code = "e {}\nf {}\n"
    });

    ecs_entity_t b = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "b" }),
        .code = "g { Foo }"
    });

    ecs_entity_t session = ecs_shell_session_init(world, NULL);

    ecs_shell_result_t r = ecs_shell_run(world, session, "scripts");
    test_int(ecs_vec_count(&r.errors), 0);
    test_int(ecs_vec_count(r.result.ptr), 2);
    ecs_shell_script_info_t *info = ecs_vec_first(r.result.ptr);
    test_uint(info[0].script, a);
    test_int(info[0].lines, 2);
    test_bool(info[0].unsaved, false);
    test_str(info[0].error, NULL);
    test_str(info[0].filename, NULL);
    test_uint(info[1].script, b);
    test_int(info[1].lines, 1);
    test_assert(info[1].error != NULL);
    ecs_shell_result_fini(world, &r);

    ecs_fini(world);
}

void Command_scripts_unsaved(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptShell);

    ecs_entity_t s = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "s" }),
        .code = "e {}"
    });

    ecs_script_update(world, s, 0, "e {}\nf {}");

    ecs_entity_t session = ecs_shell_session_init(world, NULL);

    ecs_shell_result_t r = ecs_shell_run(world, session, "scripts");
    test_int(ecs_vec_count(&r.errors), 0);
    test_int(ecs_vec_count(r.result.ptr), 1);
    ecs_shell_script_info_t *info = ecs_vec_first(r.result.ptr);
    test_bool(info[0].unsaved, true);
    ecs_shell_result_fini(world, &r);

    ecs_fini(world);
}

void Command_script_read(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptShell);

    ecs_script(world, {
        .entity = ecs_entity(world, { .name = "s" }),
        .code = "e {}\n\nf {}\n"
    });

    ecs_entity_t session = ecs_shell_session_init(world, NULL);

    ecs_shell_result_t r = ecs_shell_run(world, session, "script_read s");
    test_int(ecs_vec_count(&r.errors), 0);
    char *str = ecs_ptr_to_expr(world, r.result.type, r.result.ptr);
    test_str(str, "[{line: 1, text: \"e {}\"}, {line: 2, text: \"\"}, "
        "{line: 3, text: \"f {}\"}]");
    ecs_os_free(str);
    ecs_shell_result_fini(world, &r);

    ecs_fini(world);
}

void Command_script_read_not_a_script(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptShell);

    ecs_entity(world, { .name = "e" });
    ecs_entity_t session = ecs_shell_session_init(world, NULL);

    ecs_shell_result_t r = ecs_shell_run(world, session, "script_read e");
    test_uint(r.result.type, 0);
    test_int(ecs_vec_count(&r.errors), 1);
    test_str(ecs_vec_first_t(&r.errors, char*)[0], "'e' is not a script");
    ecs_shell_result_fini(world, &r);

    ecs_fini(world);
}

void Command_script_grep(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptShell);

    ecs_script(world, {
        .entity = ecs_entity(world, { .name = "a" }),
        .code = "foo {}\nbar {}\nfoo_bar {}"
    });

    ecs_script(world, {
        .entity = ecs_entity(world, { .name = "b" }),
        .code = "my_foo {}"
    });

    ecs_entity_t session = ecs_shell_session_init(world, NULL);

    ecs_shell_result_t r = ecs_shell_run(world, session, "script_grep foo a");
    test_int(ecs_vec_count(&r.errors), 0);
    char *str = ecs_ptr_to_expr(world, r.result.type, r.result.ptr);
    test_str(str, "[{script: a, line: 1, text: \"foo {}\"}, "
        "{script: a, line: 3, text: \"foo_bar {}\"}]");
    ecs_os_free(str);
    ecs_shell_result_fini(world, &r);

    r = ecs_shell_run(world, session, "script_grep foo");
    test_int(ecs_vec_count(&r.errors), 0);
    str = ecs_ptr_to_expr(world, r.result.type, r.result.ptr);
    test_str(str, "[{script: a, line: 1, text: \"foo {}\"}, "
        "{script: a, line: 3, text: \"foo_bar {}\"}, "
        "{script: b, line: 1, text: \"my_foo {}\"}]");
    ecs_os_free(str);
    ecs_shell_result_fini(world, &r);

    ecs_fini(world);
}

void Command_script_where(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptShell);

    ecs_entity_t s = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "s" }),
        .code =
        HEAD "parent {"
        LINE "  child {"
        LINE "    Foo {}"
        LINE "  }"
        LINE "}"
    });

    ecs_entity_t session = ecs_shell_session_init(world, NULL);

    ecs_shell_result_t r = ecs_shell_run(world, session,
        "script_where parent.child");
    test_int(ecs_vec_count(&r.errors), 0);
    ecs_shell_script_location_t *loc = r.result.ptr;
    test_uint(loc->script, s);
    test_int(loc->line, 2);
    test_int(loc->column, 3);
    test_uint(loc->template_, 0);
    test_str(loc->statement, "child {");
    ecs_shell_result_fini(world, &r);

    ecs_fini(world);
}

void Command_script_where_not_declared(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptShell);

    ecs_entity(world, { .name = "e" });
    ecs_entity_t session = ecs_shell_session_init(world, NULL);

    ecs_shell_result_t r = ecs_shell_run(world, session, "script_where e");
    test_uint(r.result.type, 0);
    test_int(ecs_vec_count(&r.errors), 1);
    test_str(ecs_vec_first_t(&r.errors, char*)[0],
        "entity 'e' is not declared by a managed script");
    ecs_shell_result_fini(world, &r);

    ecs_fini(world);
}

void Command_script_errors(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptShell);

    ecs_log_set_level(-4);

    ecs_script(world, {
        .entity = ecs_entity(world, { .name = "a" }),
        .code = "e {}"
    });

    ecs_entity_t b = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "b" }),
        .code = "e { Foo }"
    });

    ecs_entity_t session = ecs_shell_session_init(world, NULL);

    ecs_shell_result_t r = ecs_shell_run(world, session, "script_errors");
    test_int(ecs_vec_count(&r.errors), 0);
    test_int(ecs_vec_count(r.result.ptr), 1);
    ecs_shell_script_error_t *err = ecs_vec_first(r.result.ptr);
    test_uint(err->script, b);
    test_str(err->error, ecs_get(world, b, EcsScript)->error);
    ecs_shell_result_fini(world, &r);

    ecs_fini(world);
}

void Command_script_replace(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptShell);

    ecs_entity_t s = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "s" }),
        .code = "a {}\nb {}\nc {}"
    });

    ecs_entity_t session = ecs_shell_session_init(world, NULL);

    ecs_shell_result_t r = ecs_shell_run(world, session,
        "script_replace s \"b {}\" \"d {}\\ne {}\"");
    test_int(ecs_vec_count(&r.errors), 0);
    char *str = ecs_ptr_to_expr(world, r.result.type, r.result.ptr);
    test_str(str, "[{line: 2, text: \"d {}\"}, {line: 3, text: \"e {}\"}]");
    ecs_os_free(str);
    ecs_shell_result_fini(world, &r);

    test_str(ecs_get(world, s, EcsScript)->code, "a {}\nd {}\ne {}\nc {}");
    test_assert(ecs_lookup(world, "b") == 0);
    test_assert(ecs_lookup(world, "d") != 0);
    test_assert(ecs_lookup(world, "e") != 0);

    ecs_fini(world);
}

void Command_script_replace_not_found(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptShell);

    ecs_entity_t s = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "s" }),
        .code = "a {}"
    });

    ecs_entity_t session = ecs_shell_session_init(world, NULL);

    ecs_shell_result_t r = ecs_shell_run(world, session,
        "script_replace s foo bar");
    test_uint(r.result.type, 0);
    test_int(ecs_vec_count(&r.errors), 1);
    test_str(ecs_vec_first_t(&r.errors, char*)[0],
        "text 'foo' not found in script");
    ecs_shell_result_fini(world, &r);

    test_str(ecs_get(world, s, EcsScript)->code, "a {}");

    ecs_fini(world);
}

void Command_script_replace_multiple(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptShell);

    ecs_entity_t s = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "s" }),
        .code = "a {}\nab {}"
    });

    ecs_entity_t session = ecs_shell_session_init(world, NULL);

    ecs_shell_result_t r = ecs_shell_run(world, session,
        "script_replace s a c");
    test_uint(r.result.type, 0);
    test_int(ecs_vec_count(&r.errors), 1);
    test_str(ecs_vec_first_t(&r.errors, char*)[0],
        "text 'a' occurs 2 times in script, it must occur exactly once "
        "(use script_replace_all to replace all occurrences)");
    ecs_shell_result_fini(world, &r);

    test_str(ecs_get(world, s, EcsScript)->code, "a {}\nab {}");

    ecs_fini(world);
}

void Command_script_replace_all(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptShell);

    ecs_entity_t s = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "s" }),
        .code = "a {}\nb {}\nab {}"
    });

    ecs_entity_t session = ecs_shell_session_init(world, NULL);

    ecs_shell_result_t r = ecs_shell_run(world, session,
        "script_replace_all s a x");
    test_int(ecs_vec_count(&r.errors), 0);
    char *str = ecs_ptr_to_expr(world, r.result.type, r.result.ptr);
    test_str(str, "[{line: 1, text: \"x {}\"}, {line: 3, text: \"xb {}\"}]");
    ecs_os_free(str);
    ecs_shell_result_fini(world, &r);

    test_str(ecs_get(world, s, EcsScript)->code, "x {}\nb {}\nxb {}");
    test_assert(ecs_lookup(world, "xb") != 0);

    ecs_fini(world);
}

void Command_script_write(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptShell);

    ecs_entity_t session = ecs_shell_session_init(world, NULL);

    ecs_shell_result_t r = ecs_shell_run(world, session,
        "script_write s \"a {}\"");
    test_int(ecs_vec_count(&r.errors), 0);
    ecs_entity_t s = *(ecs_entity_t*)r.result.ptr;
    test_uint(s, ecs_lookup(world, "s"));
    ecs_shell_result_fini(world, &r);
    test_assert(ecs_lookup(world, "a") != 0);

    r = ecs_shell_run(world, session, "script_write s \"b {}\"");
    test_int(ecs_vec_count(&r.errors), 0);
    test_uint(*(ecs_entity_t*)r.result.ptr, s);
    ecs_shell_result_fini(world, &r);
    test_assert(ecs_lookup(world, "a") == 0);
    test_assert(ecs_lookup(world, "b") != 0);
    test_str(ecs_get(world, s, EcsScript)->code, "b {}");

    ecs_fini(world);
}

void Command_script_write_in_scope(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptShell);

    ecs_entity_t parent = ecs_entity(world, { .name = "parent" });
    ecs_entity_t session = ecs_shell_session_init(world,
        &(ecs_shell_session_desc_t){ .scope = parent });

    ecs_shell_result_t r = ecs_shell_run(world, session,
        "script_write s \"a {}\"");
    test_int(ecs_vec_count(&r.errors), 0);
    test_uint(*(ecs_entity_t*)r.result.ptr, ecs_lookup(world, "parent.s"));
    ecs_shell_result_fini(world, &r);

    ecs_fini(world);
}

void Command_script_patch(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptShell);

    ecs_entity_t s = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "s" }),
        .code =
        HEAD "a {}"
        LINE "b {}"
        LINE "c {}"
        LINE "d {}"
        LINE
    });

    ecs_entity_t session = ecs_shell_session_init(world, NULL);

    ecs_shell_result_t r = ecs_shell_run(world, session,
        "script_patch s ```"
        HEAD "@@ -1,3 +1,3 @@"
        LINE " a {}"
        LINE "-b {}"
        LINE "+e {}"
        LINE " c {}"
        LINE "```");
    test_int(ecs_vec_count(&r.errors), 0);
    char *str = ecs_ptr_to_expr(world, r.result.type, r.result.ptr);
    test_str(str, "[{line: 1, text: \"a {}\"}, {line: 2, text: \"e {}\"}, "
        "{line: 3, text: \"c {}\"}]");
    ecs_os_free(str);
    ecs_shell_result_fini(world, &r);

    test_str(ecs_get(world, s, EcsScript)->code,
        HEAD "a {}"
        LINE "e {}"
        LINE "c {}"
        LINE "d {}"
        LINE);
    test_assert(ecs_lookup(world, "b") == 0);
    test_assert(ecs_lookup(world, "e") != 0);

    ecs_fini(world);
}

void Command_script_patch_anchor(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptShell);

    ecs_entity_t s = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "s" }),
        .code =
        HEAD "a {"
        LINE "  x {}"
        LINE "}"
        LINE "b {"
        LINE "  x {}"
        LINE "}"
        LINE
    });

    ecs_entity_t session = ecs_shell_session_init(world, NULL);

    ecs_shell_result_t r = ecs_shell_run(world, session,
        "script_patch s ```"
        HEAD "@@ b {"
        LINE "-  x {}"
        LINE "+  y {}"
        LINE "```");
    test_int(ecs_vec_count(&r.errors), 0);
    char *str = ecs_ptr_to_expr(world, r.result.type, r.result.ptr);
    test_str(str, "[{line: 5, text: \"  y {}\"}]");
    ecs_os_free(str);
    ecs_shell_result_fini(world, &r);

    test_str(ecs_get(world, s, EcsScript)->code,
        HEAD "a {"
        LINE "  x {}"
        LINE "}"
        LINE "b {"
        LINE "  y {}"
        LINE "}"
        LINE);
    test_assert(ecs_lookup(world, "a.x") != 0);
    test_assert(ecs_lookup(world, "b.y") != 0);
    test_assert(ecs_lookup(world, "b.x") == 0);

    ecs_fini(world);
}

void Command_script_patch_insert_after_anchor(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptShell);

    ecs_entity_t s = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "s" }),
        .code =
        HEAD "a {"
        LINE "}"
        LINE
    });

    ecs_entity_t session = ecs_shell_session_init(world, NULL);

    ecs_shell_result_t r = ecs_shell_run(world, session,
        "script_patch s ```"
        HEAD "@@ a {"
        LINE "+  b {}"
        LINE "```");
    test_int(ecs_vec_count(&r.errors), 0);
    ecs_shell_result_fini(world, &r);

    test_str(ecs_get(world, s, EcsScript)->code,
        HEAD "a {"
        LINE "  b {}"
        LINE "}"
        LINE);
    test_assert(ecs_lookup(world, "a.b") != 0);

    ecs_fini(world);
}

void Command_script_patch_multiple_hunks(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptShell);

    ecs_entity_t s = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "s" }),
        .code =
        HEAD "a {}"
        LINE "b {}"
        LINE "c {}"
        LINE "d {}"
        LINE
    });

    ecs_entity_t session = ecs_shell_session_init(world, NULL);

    ecs_shell_result_t r = ecs_shell_run(world, session,
        "script_patch s ```"
        HEAD "@@"
        LINE "-a {}"
        LINE "+x {}"
        LINE "@@"
        LINE "-d {}"
        LINE "+y {}"
        LINE "```");
    test_int(ecs_vec_count(&r.errors), 0);
    char *str = ecs_ptr_to_expr(world, r.result.type, r.result.ptr);
    test_str(str, "[{line: 1, text: \"x {}\"}, {line: 4, text: \"y {}\"}]");
    ecs_os_free(str);
    ecs_shell_result_fini(world, &r);

    test_str(ecs_get(world, s, EcsScript)->code,
        HEAD "x {}"
        LINE "b {}"
        LINE "c {}"
        LINE "y {}"
        LINE);

    ecs_fini(world);
}

void Command_script_patch_no_match(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptShell);

    ecs_entity_t s = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "s" }),
        .code =
        HEAD "a {}"
        LINE "b {}"
        LINE "c {}"
        LINE
    });

    ecs_entity_t session = ecs_shell_session_init(world, NULL);

    ecs_shell_result_t r = ecs_shell_run(world, session,
        "script_patch s ```"
        HEAD "@@"
        LINE " b {}"
        LINE "-c  {}"
        LINE "+d {}"
        LINE "```");
    test_uint(r.result.type, 0);
    test_int(ecs_vec_count(&r.errors), 1);
    test_str(ecs_vec_first_t(&r.errors, char*)[0],
        "hunk 1 does not match, closest match at line 2:"
        LINE "  line 3: expected 'c  {}', found 'c {}'");
    ecs_shell_result_fini(world, &r);

    test_str(ecs_get(world, s, EcsScript)->code,
        HEAD "a {}"
        LINE "b {}"
        LINE "c {}"
        LINE);

    ecs_fini(world);
}

void Command_script_patch_ambiguous(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptShell);

    ecs_script(world, {
        .entity = ecs_entity(world, { .name = "s" }),
        .code =
        HEAD "a {"
        LINE "  x {}"
        LINE "}"
        LINE "b {"
        LINE "  x {}"
        LINE "}"
        LINE
    });

    ecs_entity_t session = ecs_shell_session_init(world, NULL);

    ecs_shell_result_t r = ecs_shell_run(world, session,
        "script_patch s ```"
        HEAD "@@"
        LINE "-  x {}"
        LINE "+  y {}"
        LINE "```");
    test_uint(r.result.type, 0);
    test_int(ecs_vec_count(&r.errors), 1);
    test_str(ecs_vec_first_t(&r.errors, char*)[0],
        "hunk 1 matches 2 times, add context lines or an anchor to make it "
        "unique");
    ecs_shell_result_fini(world, &r);

    ecs_fini(world);
}

void Command_script_patch_all_or_nothing(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptShell);

    ecs_entity_t s = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "s" }),
        .code =
        HEAD "a {}"
        LINE "b {}"
        LINE
    });

    ecs_entity_t session = ecs_shell_session_init(world, NULL);

    ecs_shell_result_t r = ecs_shell_run(world, session,
        "script_patch s ```"
        HEAD "@@"
        LINE "-a {}"
        LINE "+x {}"
        LINE "@@"
        LINE "-c {}"
        LINE "+y {}"
        LINE "```");
    test_uint(r.result.type, 0);
    test_int(ecs_vec_count(&r.errors), 1);
    ecs_shell_result_fini(world, &r);

    test_str(ecs_get(world, s, EcsScript)->code, "a {}\nb {}\n");
    test_assert(ecs_lookup(world, "a") != 0);
    test_assert(ecs_lookup(world, "x") == 0);

    ecs_fini(world);
}

void Command_script_patch_anchor_not_found(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptShell);

    ecs_script(world, {
        .entity = ecs_entity(world, { .name = "s" }),
        .code = "a {}\n"
    });

    ecs_entity_t session = ecs_shell_session_init(world, NULL);

    ecs_shell_result_t r = ecs_shell_run(world, session,
        "script_patch s ```"
        HEAD "@@ foo"
        LINE "+b {}"
        LINE "```");
    test_uint(r.result.type, 0);
    test_int(ecs_vec_count(&r.errors), 1);
    test_str(ecs_vec_first_t(&r.errors, char*)[0],
        "anchor 'foo' of hunk 1 not found");
    ecs_shell_result_fini(world, &r);

    ecs_fini(world);
}

void Command_script_edit_reload_error(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptShell);

    ecs_entity_t s = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "s" }),
        .code = "a {}\n"
    });

    ecs_entity_t session = ecs_shell_session_init(world, NULL);

    ecs_shell_result_t r = ecs_shell_run(world, session,
        "script_replace s \"a {}\" \"a { Foo }\"");
    test_assert(r.result.type != 0);
    char *str = ecs_ptr_to_expr(world, r.result.type, r.result.ptr);
    test_str(str, "[{line: 1, text: \"a { Foo }\"}]");
    ecs_os_free(str);
    test_int(ecs_vec_count(&r.errors), 1);
    ecs_shell_result_fini(world, &r);

    const EcsScript *script = ecs_get(world, s, EcsScript);
    test_str(script->code, "a { Foo }\n");
    test_assert(script->error != NULL);

    ecs_fini(world);
}

void Command_script_set(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptShell);

    ecs_entity_t s = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "s" }),
        .code =
        HEAD "struct Position(x: f32, y: f32)"
        LINE "e {"
        LINE "  Position: {1, 2}"
        LINE "}"
        LINE
    });

    ecs_entity_t session = ecs_shell_session_init(world, NULL);

    ecs_shell_result_t r = ecs_shell_run(world, session,
        "script_set e Position {y: 20}");
    test_int(ecs_vec_count(&r.errors), 0);
    char *str = ecs_ptr_to_expr(world, r.result.type, r.result.ptr);
    test_str(str, "{line: 3, text: \"  Position: {1, 20}\"}");
    ecs_os_free(str);
    ecs_shell_result_fini(world, &r);

    test_str(ecs_get(world, s, EcsScript)->code,
        HEAD "struct Position(x: f32, y: f32)"
        LINE "e {"
        LINE "  Position: {1, 20}"
        LINE "}"
        LINE);

    const Position *p = ecs_get_id(world,
        ecs_lookup(world, "e"), ecs_lookup(world, "Position"));
    test_assert(p != NULL);
    test_int(p->x, 1);
    test_int(p->y, 20);

    ecs_fini(world);
}

void Command_script_set_not_declared(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptShell);

    ecs_script_run(world, NULL, "struct Position(x: f32, y: f32)", NULL);
    ecs_entity(world, { .name = "e" });
    ecs_entity_t session = ecs_shell_session_init(world, NULL);

    ecs_shell_result_t r = ecs_shell_run(world, session,
        "script_set e Position {1, 2}");
    test_uint(r.result.type, 0);
    test_int(ecs_vec_count(&r.errors), 1);
    test_str(ecs_vec_first_t(&r.errors, char*)[0],
        "entity 'e' is not declared by a managed script");
    ecs_shell_result_fini(world, &r);

    ecs_fini(world);
}

void Command_script_remove(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptShell);

    ecs_entity_t s = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "s" }),
        .code =
        HEAD "Tag {}"
        LINE "e {"
        LINE "  Tag"
        LINE "}"
        LINE
    });

    ecs_entity_t session = ecs_shell_session_init(world, NULL);

    ecs_shell_result_t r = ecs_shell_run(world, session,
        "script_remove e Tag");
    test_int(ecs_vec_count(&r.errors), 0);
    test_uint(r.result.type, 0);
    ecs_shell_result_fini(world, &r);

    test_str(ecs_get(world, s, EcsScript)->code,
        HEAD "Tag {}"
        LINE "e {"
        LINE "}"
        LINE);
    test_assert(!ecs_has_id(world,
        ecs_lookup(world, "e"), ecs_lookup(world, "Tag")));

    ecs_fini(world);
}

void Command_script_delete(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptShell);

    ecs_entity_t s = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "s" }),
        .code =
        HEAD "a {}"
        LINE "b {}"
        LINE
    });

    ecs_entity_t session = ecs_shell_session_init(world, NULL);

    ecs_shell_result_t r = ecs_shell_run(world, session,
        "script_delete a");
    test_int(ecs_vec_count(&r.errors), 0);
    test_uint(r.result.type, 0);
    ecs_shell_result_fini(world, &r);

    test_str(ecs_get(world, s, EcsScript)->code, "b {}\n");
    test_assert(ecs_lookup(world, "a") == 0);
    test_assert(ecs_lookup(world, "b") != 0);

    ecs_fini(world);
}

void Command_script_sync(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptShell);

    ecs_entity_t s = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "s" }),
        .code =
        HEAD "struct Position(x: f32, y: f32)"
        LINE "e {"
        LINE "  Position: {1, 2}"
        LINE "}"
        LINE
    });

    ecs_entity_t session = ecs_shell_session_init(world, NULL);

    ecs_shell_result_t r = ecs_shell_run(world, session,
        "script_sync s");
    test_int(ecs_vec_count(&r.errors), 0);
    test_bool(*(bool*)r.result.ptr, false);
    ecs_shell_result_fini(world, &r);

    r = ecs_shell_run(world, session, "set e Position {x: 5}");
    ecs_shell_result_fini(world, &r);

    r = ecs_shell_run(world, session, "script_sync s");
    test_int(ecs_vec_count(&r.errors), 0);
    test_bool(*(bool*)r.result.ptr, true);
    ecs_shell_result_fini(world, &r);

    test_str(ecs_get(world, s, EcsScript)->code,
        HEAD "struct Position(x: f32, y: f32)"
        LINE "e {"
        LINE "  Position: {5, 2}"
        LINE "}"
        LINE);

    ecs_fini(world);
}

void Command_script_diff(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptShell);

    ecs_entity_t s = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "s" }),
        .code =
        HEAD "a {}"
        LINE "b {}"
        LINE "c {}"
        LINE "d {}"
        LINE "e {}"
        LINE "f {}"
        LINE "g {}"
        LINE "h {}"
        LINE "i {}"
        LINE "j {}"
        LINE
    });

    ecs_entity_t session = ecs_shell_session_init(world, NULL);

    ecs_shell_result_t r = ecs_shell_run(world, session, "script_diff s");
    test_int(ecs_vec_count(&r.errors), 0);
    test_str(*(char**)r.result.ptr, "");
    ecs_shell_result_fini(world, &r);

    ecs_script_update(world, s, 0,
        HEAD "a {}"
        LINE "x {}"
        LINE "c {}"
        LINE "d {}"
        LINE "e {}"
        LINE "f {}"
        LINE "g {}"
        LINE "h {}"
        LINE "i {}"
        LINE "j {}"
        LINE "k {}"
        LINE);

    r = ecs_shell_run(world, session, "script_diff s");
    test_int(ecs_vec_count(&r.errors), 0);
    test_str(*(char**)r.result.ptr,
        HEAD "@@ -1,5 +1,5 @@"
        LINE " a {}"
        LINE "-b {}"
        LINE "+x {}"
        LINE " c {}"
        LINE " d {}"
        LINE " e {}"
        LINE "@@ -8,3 +8,4 @@"
        LINE " h {}"
        LINE " i {}"
        LINE " j {}"
        LINE "+k {}"
        LINE);
    ecs_shell_result_fini(world, &r);

    ecs_fini(world);
}

void Command_script_diff_patch_roundtrip(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptShell);

    const char *original =
        HEAD "a {}"
        LINE "b {}"
        LINE "c {}"
        LINE "d {}"
        LINE;

    const char *changed =
        HEAD "b {}"
        LINE "c {}"
        LINE "x {}"
        LINE "d {}"
        LINE "e {}"
        LINE;

    ecs_entity_t a = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "a" }),
        .code = original
    });
    ecs_script_update(world, a, 0, changed);

    ecs_entity_t b = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "b" }),
        .code = original
    });

    ecs_entity_t session = ecs_shell_session_init(world, NULL);

    ecs_shell_result_t r = ecs_shell_run(world, session, "script_diff a");
    test_int(ecs_vec_count(&r.errors), 0);
    char *cmd = flecs_asprintf("script_patch b ```%s```",
        *(char**)r.result.ptr);
    ecs_shell_result_fini(world, &r);

    r = ecs_shell_run(world, session, cmd);
    test_int(ecs_vec_count(&r.errors), 0);
    ecs_shell_result_fini(world, &r);
    ecs_os_free(cmd);

    test_str(ecs_get(world, b, EcsScript)->code, changed);

    ecs_fini(world);
}

void Command_script_save(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptShell);

    command_files[0].name = "s.flecs";
    strcpy(command_files[0].content, "a {}\n");

    ecs_entity_t s = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "s" }),
        .filename = "s.flecs"
    });
    test_assert(s != 0);
    test_str(ecs_get(world, s, EcsScript)->saved_code, "a {}\n");

    ecs_entity_t session = ecs_shell_session_init(world, NULL);

    ecs_shell_result_t r = ecs_shell_run(world, session,
        "script_replace s a b");
    test_int(ecs_vec_count(&r.errors), 0);
    ecs_shell_result_fini(world, &r);

    r = ecs_shell_run(world, session, "scripts");
    test_bool(((ecs_shell_script_info_t*)
        ecs_vec_first(r.result.ptr))->unsaved, true);
    ecs_shell_result_fini(world, &r);

    r = ecs_shell_run(world, session, "script_save s");
    test_int(ecs_vec_count(&r.errors), 0);
    ecs_shell_result_fini(world, &r);

    test_str(command_files[0].content, "b {}\n");
    test_str(ecs_get(world, s, EcsScript)->saved_code, "b {}\n");

    r = ecs_shell_run(world, session, "scripts");
    test_bool(((ecs_shell_script_info_t*)
        ecs_vec_first(r.result.ptr))->unsaved, false);
    ecs_shell_result_fini(world, &r);

    ecs_fini(world);
}

void Command_script_save_conflict(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptShell);

    command_files[0].name = "s.flecs";
    strcpy(command_files[0].content, "a {}\n");

    ecs_entity_t s = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "s" }),
        .filename = "s.flecs"
    });
    test_assert(s != 0);

    strcpy(command_files[0].content, "c {}\n");

    ecs_entity_t session = ecs_shell_session_init(world, NULL);

    ecs_shell_result_t r = ecs_shell_run(world, session,
        "script_replace s a b");
    test_int(ecs_vec_count(&r.errors), 0);
    ecs_shell_result_fini(world, &r);

    r = ecs_shell_run(world, session, "script_save s");
    test_int(ecs_vec_count(&r.errors), 1);
    test_str(ecs_vec_first_t(&r.errors, char*)[0],
        "file 's.flecs' changed on disk since it was loaded or saved, "
        "reload the script before saving");
    ecs_shell_result_fini(world, &r);

    test_str(command_files[0].content, "c {}\n");

    ecs_fini(world);
}

void Command_script_save_as(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptShell);

    ecs_entity_t s = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "s" }),
        .code = "a {}\n"
    });

    ecs_entity_t session = ecs_shell_session_init(world, NULL);

    ecs_shell_result_t r = ecs_shell_run(world, session, "script_save s");
    test_int(ecs_vec_count(&r.errors), 1);
    test_str(ecs_vec_first_t(&r.errors, char*)[0],
        "script 's' has no filename");
    ecs_shell_result_fini(world, &r);

    r = ecs_shell_run(world, session, "script_save s out.flecs");
    test_int(ecs_vec_count(&r.errors), 0);
    ecs_shell_result_fini(world, &r);

    test_str(ecs_get(world, s, EcsScript)->filename, "out.flecs");
    test_str(command_files[0].name, "out.flecs");
    test_str(command_files[0].content, "a {}\n");

    ecs_fini(world);
}

void Command_script_reload(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptShell);

    command_files[0].name = "s.flecs";
    strcpy(command_files[0].content, "a {}\n");

    ecs_entity_t s = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "s" }),
        .filename = "s.flecs"
    });
    test_assert(s != 0);
    test_assert(ecs_lookup(world, "a") != 0);

    strcpy(command_files[0].content, "b {}\n");

    ecs_entity_t session = ecs_shell_session_init(world, NULL);

    ecs_shell_result_t r = ecs_shell_run(world, session, "script_reload s");
    test_int(ecs_vec_count(&r.errors), 0);
    ecs_shell_result_fini(world, &r);

    test_assert(ecs_lookup(world, "a") == 0);
    test_assert(ecs_lookup(world, "b") != 0);
    test_str(ecs_get(world, s, EcsScript)->saved_code, "b {}\n");

    ecs_fini(world);
}

void Command_script_undo(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptShell);

    ecs_entity_t s = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "s" }),
        .code = "a {}"
    });

    ecs_entity_t session = ecs_shell_session_init(world, NULL);

    ecs_shell_result_t r = ecs_shell_run(world, session,
        "script_replace s a b");
    ecs_shell_result_fini(world, &r);
    r = ecs_shell_run(world, session, "script_replace s b c");
    ecs_shell_result_fini(world, &r);
    test_str(ecs_get(world, s, EcsScript)->code, "c {}");

    r = ecs_shell_run(world, session, "script_undo s");
    test_int(ecs_vec_count(&r.errors), 0);
    ecs_shell_result_fini(world, &r);
    test_str(ecs_get(world, s, EcsScript)->code, "b {}");
    test_assert(ecs_lookup(world, "b") != 0);
    test_assert(ecs_lookup(world, "c") == 0);

    r = ecs_shell_run(world, session, "script_undo s");
    test_int(ecs_vec_count(&r.errors), 0);
    ecs_shell_result_fini(world, &r);
    test_str(ecs_get(world, s, EcsScript)->code, "a {}");

    r = ecs_shell_run(world, session, "script_undo s");
    test_int(ecs_vec_count(&r.errors), 1);
    test_str(ecs_vec_first_t(&r.errors, char*)[0],
        "undo history of script is empty");
    ecs_shell_result_fini(world, &r);
    test_str(ecs_get(world, s, EcsScript)->code, "a {}");

    ecs_fini(world);
}

void Command_focus_blur(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptShell);

    ecs_entity_t e = ecs_entity(world, { .name = "e" });
    ecs_entity_t session = ecs_shell_session_init(world, NULL);

    ecs_shell_result_t r = ecs_shell_run(world, session, "focus");
    test_int(ecs_vec_count(&r.errors), 0);
    test_uint(*(ecs_entity_t*)r.result.ptr, 0);
    ecs_shell_result_fini(world, &r);

    r = ecs_shell_run(world, session, "focus e");
    test_int(ecs_vec_count(&r.errors), 0);
    test_uint(*(ecs_entity_t*)r.result.ptr, e);
    ecs_shell_result_fini(world, &r);
    test_uint(ecs_script_get_focus(world), e);

    r = ecs_shell_run(world, session, "blur");
    test_int(ecs_vec_count(&r.errors), 0);
    test_uint(r.result.type, 0);
    ecs_shell_result_fini(world, &r);
    test_uint(ecs_script_get_focus(world), 0);

    ecs_fini(world);
}

void Command_click(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptShell);

    ecs_script(world, {
        .code =
        HEAD "using flecs.script"
        LINE "template Widget {"
        LINE "  mut clicks = 0"
        LINE "  mut button = -1"
        LINE "  async {"
        LINE "    while true {"
        LINE "      const evt = await on.click(this)"
        LINE "      clicks = clicks + 1"
        LINE "      button = evt.button"
        LINE "    }"
        LINE "  }"
        LINE "}"
        LINE "Widget w"
    });
    ecs_script_tasks_progress(world);

    ecs_entity_t w = ecs_lookup(world, "w");
    ecs_entity_t mut = ecs_lookup(world, "Widget.mut");
    test_assert(w != 0);
    test_assert(mut != 0);

    ecs_entity_t session = ecs_shell_session_init(world, NULL);

    ecs_shell_result_t r = ecs_shell_run(world, session, "click w");
    test_int(ecs_vec_count(&r.errors), 0);
    char *str = ecs_ptr_to_expr(world, r.result.type, r.result.ptr);
    test_str(str, "{resolved: 1, stopped: false}");
    ecs_os_free(str);
    ecs_shell_result_fini(world, &r);

    const int64_t *v = ecs_get_id(world, w, mut);
    test_int(v[0], 1);
    test_int(v[1], 0);

    r = ecs_shell_run(world, session, "click w ctrl+right");
    test_int(ecs_vec_count(&r.errors), 0);
    ecs_shell_result_fini(world, &r);

    v = ecs_get_id(world, w, mut);
    test_int(v[0], 2);
    test_int(v[1], 2);

    ecs_fini(world);
}

void Command_press_release(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptShell);

    ecs_script(world, {
        .code =
        HEAD "using flecs.script"
        LINE "template Widget {"
        LINE "  mut presses = 0"
        LINE "  mut releases = 0"
        LINE "  async { while true { await on.press(this)"
        LINE "    presses = presses + 1 } }"
        LINE "  async { while true { await on.release(this)"
        LINE "    releases = releases + 1 } }"
        LINE "}"
        LINE "Widget w"
    });
    ecs_script_tasks_progress(world);

    ecs_entity_t w = ecs_lookup(world, "w");
    ecs_entity_t mut = ecs_lookup(world, "Widget.mut");

    ecs_entity_t session = ecs_shell_session_init(world, NULL);

    ecs_shell_result_t r = ecs_shell_run(world, session, "press w");
    test_int(ecs_vec_count(&r.errors), 0);
    test_int(((ecs_script_event_result_t*)r.result.ptr)->resolved, 1);
    ecs_shell_result_fini(world, &r);

    const int64_t *v = ecs_get_id(world, w, mut);
    test_int(v[0], 1);
    test_int(v[1], 0);

    r = ecs_shell_run(world, session, "pointer");
    test_int(ecs_vec_count(&r.errors), 0);
    ecs_shell_pointer_state_t *ps = r.result.ptr;
    test_uint(ps->pressed, w);
    test_uint(ps->event.target, w);
    test_uint(ps->event.buttons, 1);
    ecs_shell_result_fini(world, &r);

    r = ecs_shell_run(world, session, "release");
    test_int(ecs_vec_count(&r.errors), 0);
    ecs_shell_result_fini(world, &r);

    v = ecs_get_id(world, w, mut);
    test_int(v[0], 1);
    test_int(v[1], 1);

    r = ecs_shell_run(world, session, "pointer");
    test_int(ecs_vec_count(&r.errors), 0);
    ps = r.result.ptr;
    test_uint(ps->pressed, 0);
    test_uint(ps->event.buttons, 0);
    ecs_shell_result_fini(world, &r);

    ecs_fini(world);
}

void Command_move_drag(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptShell);

    ecs_script(world, {
        .code =
        HEAD "using flecs.script"
        LINE "template Widget {"
        LINE "  mut moves = 0"
        LINE "  mut drags = 0"
        LINE "  mut dx: f32 = 0"
        LINE "  async { while true { await on.move(this)"
        LINE "    moves = moves + 1 } }"
        LINE "  async { while true { const e = await on.drag(this)"
        LINE "    drags = drags + 1"
        LINE "    dx = dx + e.delta_x } }"
        LINE "}"
        LINE "Widget w"
    });
    ecs_script_tasks_progress(world);

    ecs_entity_t w = ecs_lookup(world, "w");
    ecs_entity_t mut = ecs_lookup(world, "Widget.mut");

    typedef struct { int64_t moves; int64_t drags; float dx; } widget_t;

    ecs_entity_t session = ecs_shell_session_init(world, NULL);

    ecs_shell_result_t r = ecs_shell_run(world, session, "move w 1 2");
    test_int(ecs_vec_count(&r.errors), 0);
    ecs_shell_result_fini(world, &r);

    const widget_t *v = ecs_get_id(world, w, mut);
    test_int(v->moves, 1);
    test_int(v->drags, 0);

    r = ecs_shell_run(world, session, "move_by 3 4");
    test_int(ecs_vec_count(&r.errors), 0);
    ecs_shell_result_fini(world, &r);

    v = ecs_get_id(world, w, mut);
    test_int(v->moves, 2);
    test_int(v->drags, 0);

    r = ecs_shell_run(world, session, "pointer");
    ecs_shell_pointer_state_t *ps = r.result.ptr;
    test_flt(ps->event.local_x, 4);
    test_flt(ps->event.local_y, 6);
    ecs_shell_result_fini(world, &r);

    r = ecs_shell_run(world, session, "press w");
    ecs_shell_result_fini(world, &r);
    v = ecs_get_id(world, w, mut);
    test_int(v->drags, 1);

    r = ecs_shell_run(world, session, "move_by 5 0");
    test_int(ecs_vec_count(&r.errors), 0);
    ecs_shell_result_fini(world, &r);

    v = ecs_get_id(world, w, mut);
    test_int(v->moves, 2);
    test_int(v->drags, 2);
    test_flt(v->dx, 5);

    r = ecs_shell_run(world, session, "release");
    ecs_shell_result_fini(world, &r);

    r = ecs_shell_run(world, session, "drag w 10 0");
    test_int(ecs_vec_count(&r.errors), 0);
    ecs_shell_result_fini(world, &r);

    v = ecs_get_id(world, w, mut);
    test_int(v->drags, 4);
    test_flt(v->dx, 15);

    r = ecs_shell_run(world, session, "pointer");
    ps = r.result.ptr;
    test_uint(ps->event.buttons, 0);
    ecs_shell_result_fini(world, &r);

    ecs_fini(world);
}

void Command_move_to_screen(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptShell);

    ecs_entity_t e = ecs_entity(world, { .name = "e" });
    ecs_script_set_event_hooks(world, &(ecs_script_event_hooks_t){
        .to_screen = Command_to_screen
    });

    ecs_entity_t session = ecs_shell_session_init(world, NULL);

    ecs_shell_result_t r = ecs_shell_run(world, session, "move e 1 2");
    test_int(ecs_vec_count(&r.errors), 0);
    ecs_shell_result_fini(world, &r);

    EcsScriptMouseEvent last;
    test_bool(ecs_script_get_mouse_state(world, &last, NULL), true);
    test_uint(last.target, e);
    test_flt(last.local_x, 1);
    test_flt(last.local_y, 2);
    test_flt(last.screen_x, 101);
    test_flt(last.screen_y, 202);

    ecs_fini(world);
}

void Command_key(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptShell);

    ecs_script(world, {
        .code =
        HEAD "using flecs.script"
        LINE "template Widget {"
        LINE "  mut downs = 0"
        LINE "  mut ups = 0"
        LINE "  mut ctrl = false"
        LINE "  async { while true { const e = await on.key_down(this)"
        LINE "    downs = downs + 1"
        LINE "    ctrl = e.ctrl } }"
        LINE "  async { while true { await on.key_up(this)"
        LINE "    ups = ups + 1 } }"
        LINE "}"
        LINE "Widget w"
    });
    ecs_script_tasks_progress(world);

    ecs_entity_t w = ecs_lookup(world, "w");
    ecs_entity_t mut = ecs_lookup(world, "Widget.mut");

    typedef struct { int64_t downs; int64_t ups; bool ctrl; } widget_t;

    ecs_entity_t session = ecs_shell_session_init(world, NULL);

    ecs_shell_result_t r = ecs_shell_run(world, session, "focus w");
    ecs_shell_result_fini(world, &r);

    r = ecs_shell_run(world, session, "key a");
    test_int(ecs_vec_count(&r.errors), 0);
    test_int(((ecs_script_event_result_t*)r.result.ptr)->resolved, 2);
    ecs_shell_result_fini(world, &r);

    const widget_t *v = ecs_get_id(world, w, mut);
    test_int(v->downs, 1);
    test_int(v->ups, 1);
    test_bool(v->ctrl, false);

    r = ecs_shell_run(world, session, "key_down ctrl+a");
    test_int(ecs_vec_count(&r.errors), 0);
    ecs_shell_result_fini(world, &r);

    v = ecs_get_id(world, w, mut);
    test_int(v->downs, 2);
    test_int(v->ups, 1);
    test_bool(v->ctrl, true);

    r = ecs_shell_run(world, session, "key_up ctrl+a");
    test_int(ecs_vec_count(&r.errors), 0);
    ecs_shell_result_fini(world, &r);

    v = ecs_get_id(world, w, mut);
    test_int(v->downs, 2);
    test_int(v->ups, 2);

    r = ecs_shell_run(world, session, "type \"abc\"");
    test_int(ecs_vec_count(&r.errors), 0);
    test_int(((ecs_script_event_result_t*)r.result.ptr)->resolved, 6);
    ecs_shell_result_fini(world, &r);

    v = ecs_get_id(world, w, mut);
    test_int(v->downs, 5);
    test_int(v->ups, 5);

    ecs_fini(world);
}

void Command_key_plus(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptShell);

    ecs_script(world, {
        .code =
        HEAD "using flecs.script"
        LINE "template Widget {"
        LINE "  mut key = \"\""
        LINE "  mut shift = false"
        LINE "  async { while true { const e = await on.key_down(this)"
        LINE "    key = e.key"
        LINE "    shift = e.shift } }"
        LINE "}"
        LINE "Widget w"
    });
    ecs_script_tasks_progress(world);

    ecs_entity_t w = ecs_lookup(world, "w");
    ecs_entity_t mut = ecs_lookup(world, "Widget.mut");

    typedef struct { char *key; bool shift; } widget_t;

    ecs_entity_t session = ecs_shell_session_init(world, NULL);

    ecs_shell_result_t r = ecs_shell_run(world, session, "focus w");
    ecs_shell_result_fini(world, &r);

    r = ecs_shell_run(world, session, "key_down shift++");
    test_int(ecs_vec_count(&r.errors), 0);
    ecs_shell_result_fini(world, &r);

    const widget_t *v = ecs_get_id(world, w, mut);
    test_str(v->key, "+");
    test_bool(v->shift, true);

    ecs_fini(world);
}

void Command_invalid_button(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptShell);

    ecs_entity(world, { .name = "e" });
    ecs_entity_t session = ecs_shell_session_init(world, NULL);

    ecs_shell_result_t r = ecs_shell_run(world, session, "click e up");
    test_uint(r.result.type, 0);
    test_int(ecs_vec_count(&r.errors), 1);
    test_str(ecs_vec_first_t(&r.errors, char*)[0],
        "invalid button 'up', expected left, middle or right");
    ecs_shell_result_fini(world, &r);

    r = ecs_shell_run(world, session, "click e foo+left");
    test_uint(r.result.type, 0);
    test_int(ecs_vec_count(&r.errors), 1);
    test_str(ecs_vec_first_t(&r.errors, char*)[0],
        "invalid modifier 'foo' in 'foo+left', expected ctrl, shift, alt "
        "or meta");
    ecs_shell_result_fini(world, &r);

    ecs_fini(world);
}

void Command_mouse_keyboard_event(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptShell);

    ecs_script(world, {
        .code =
        HEAD "using flecs.script"
        LINE "template Widget {"
        LINE "  mut presses = 0"
        LINE "  mut keys = 0"
        LINE "  async { while true { await on.press(this)"
        LINE "    presses = presses + 1 } }"
        LINE "  async { while true { await on.key_down(this)"
        LINE "    keys = keys + 1 } }"
        LINE "}"
        LINE "Widget w"
    });
    ecs_script_tasks_progress(world);

    ecs_entity_t w = ecs_lookup(world, "w");
    ecs_entity_t mut = ecs_lookup(world, "Widget.mut");

    ecs_entity_t session = ecs_shell_session_init(world, NULL);

    ecs_shell_result_t r = ecs_shell_run(world, session,
        "mouse_event {target: w, buttons: 1, local_x: 5}");
    test_int(ecs_vec_count(&r.errors), 0);
    test_int(((ecs_script_event_result_t*)r.result.ptr)->resolved, 1);
    ecs_shell_result_fini(world, &r);

    const int64_t *v = ecs_get_id(world, w, mut);
    test_int(v[0], 1);
    test_uint(ecs_script_get_focus(world), w);

    r = ecs_shell_run(world, session,
        "keyboard_event {key: \"x\", down: true}");
    test_int(ecs_vec_count(&r.errors), 0);
    test_int(((ecs_script_event_result_t*)r.result.ptr)->resolved, 1);
    ecs_shell_result_fini(world, &r);

    v = ecs_get_id(world, w, mut);
    test_int(v[1], 1);

    ecs_fini(world);
}

void Command_listeners(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptShell);

    ecs_script(world, {
        .code =
        HEAD "using flecs.script"
        LINE "template Widget {"
        LINE "  async { while true { await on.click(this) } }"
        LINE "  async { while true { await on.key_down(this) } }"
        LINE "  async { while true { await on.click(this) } }"
        LINE "}"
        LINE "Widget w"
        LINE "other {}"
    });
    ecs_script_tasks_progress(world);

    ecs_entity_t session = ecs_shell_session_init(world, NULL);

    ecs_shell_result_t r = ecs_shell_run(world, session, "listeners w");
    test_int(ecs_vec_count(&r.errors), 0);
    char *str = ecs_ptr_to_expr(world, r.result.type, r.result.ptr);
    test_str(str, "[\"click\", \"key_down\"]");
    ecs_os_free(str);
    ecs_shell_result_fini(world, &r);

    r = ecs_shell_run(world, session, "listeners other");
    test_int(ecs_vec_count(&r.errors), 0);
    test_int(ecs_vec_count(r.result.ptr), 0);
    ecs_shell_result_fini(world, &r);

    ecs_fini(world);
}

void Command_call_from_script(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptShell);

    ecs_entity_t parent = ecs_entity(world, { .name = "parent" });
    ecs_entity(world, { .name = "parent.a" });

    ecs_entity_t session = ecs_shell_session_init(world, NULL);
    (void)session;

    int32_t count = 0;
    ecs_expr_eval_desc_t desc = {0};
    test_assert(ecs_expr_run(world,
        "flecs.script.shell.commands.core.ls(parent).count()",
        &ecs_value_ptr(ecs_i32_t, &count), &desc) != NULL);
    test_int(count, 1);
    (void)parent;

    ecs_fini(world);
}

void Command_shell_singleton(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptShell);

    test_uint(ecs_lookup(world, "flecs.script.shell.Shell"),
        ecs_id(EcsScriptShell));

    const EcsScriptShell *shell = ecs_singleton_get(world, EcsScriptShell);
    test_assert(shell != NULL);
    test_int(ecs_vec_count(&shell->command_paths), 3);

    ecs_entity_t *paths = ecs_vec_first(&shell->command_paths);
    test_uint(paths[0], ecs_lookup(world, "flecs.script.shell.commands.core"));
    test_uint(paths[1], ecs_lookup(world, "flecs.script.shell.commands.input"));
    test_uint(paths[2], ecs_lookup(world, "flecs.script.shell.commands.script"));
    test_assert(paths[0] != 0);
    test_assert(paths[1] != 0);
    test_assert(paths[2] != 0);

    test_assert(ecs_lookup(world, "flecs.script.shell.commands.core.ls") != 0);
    test_assert(ecs_lookup(world, "flecs.script.shell.commands.input.click") != 0);
    test_assert(ecs_lookup(world, "flecs.script.shell.commands.script.scripts") != 0);
    test_assert(ecs_lookup(world, "flecs.script.shell.commands.core.click") == 0);
    test_assert(ecs_lookup(world, "flecs.script.shell.commands.core.scripts") == 0);

    ecs_fini(world);
}

void Command_custom_command_path(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptShell);

    ecs_entity_t commands = ecs_entity(world, { .name = "commands" });
    ecs_entity_t custom = ecs_function(world, {
        .name = "custom",
        .parent = commands,
        .return_type = ecs_id(ecs_entity_t),
        .callback = Command_function_callback
    });

    ecs_entity_t session = ecs_shell_session_init(world, NULL);

    ecs_shell_result_t r = ecs_shell_run(world, session, "custom");
    test_int(ecs_vec_count(&r.errors), 1);
    test_str(ecs_vec_first_t(&r.errors, char*)[0], "unknown command 'custom'");
    ecs_shell_result_fini(world, &r);

    EcsScriptShell *shell = ecs_singleton_ensure(world, EcsScriptShell);
    ecs_vec_append_t(NULL, &shell->command_paths, ecs_entity_t)[0] = commands;
    ecs_singleton_modified(world, EcsScriptShell);

    r = ecs_shell_run(world, session, "custom");
    test_int(ecs_vec_count(&r.errors), 0);
    test_uint(r.result.type, ecs_id(ecs_entity_t));
    test_uint(*(ecs_entity_t*)r.result.ptr, custom);
    ecs_shell_result_fini(world, &r);

    r = ecs_shell_run(world, session, "ls");
    test_int(ecs_vec_count(&r.errors), 0);
    ecs_shell_result_fini(world, &r);

    ecs_fini(world);
}

void Command_command_path_order(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptShell);

    ecs_entity_t a = ecs_entity(world, { .name = "a" });
    ecs_entity_t b = ecs_entity(world, { .name = "b" });

    ecs_entity_t a_custom = ecs_function(world, {
        .name = "custom",
        .parent = a,
        .return_type = ecs_id(ecs_entity_t),
        .callback = Command_function_callback
    });

    ecs_entity_t b_custom = ecs_function(world, {
        .name = "custom",
        .parent = b,
        .return_type = ecs_id(ecs_entity_t),
        .callback = Command_function_callback
    });

    ecs_entity_t b_ls = ecs_function(world, {
        .name = "ls",
        .parent = b,
        .return_type = ecs_id(ecs_entity_t),
        .callback = Command_function_callback
    });

    EcsScriptShell *shell = ecs_singleton_ensure(world, EcsScriptShell);
    ecs_vec_append_t(NULL, &shell->command_paths, ecs_entity_t)[0] = a;
    ecs_vec_append_t(NULL, &shell->command_paths, ecs_entity_t)[0] = b;
    ecs_singleton_modified(world, EcsScriptShell);

    ecs_entity_t session = ecs_shell_session_init(world, NULL);

    ecs_shell_result_t r = ecs_shell_run(world, session, "custom");
    test_int(ecs_vec_count(&r.errors), 0);
    test_uint(*(ecs_entity_t*)r.result.ptr, a_custom);
    ecs_shell_result_fini(world, &r);

    r = ecs_shell_run(world, session, "ls");
    test_int(ecs_vec_count(&r.errors), 0);
    test_assert(r.result.type != ecs_id(ecs_entity_t));
    ecs_shell_result_fini(world, &r);

    shell = ecs_singleton_ensure(world, EcsScriptShell);
    ecs_vec_clear(&shell->command_paths);
    ecs_vec_append_t(NULL, &shell->command_paths, ecs_entity_t)[0] = b;
    ecs_vec_append_t(NULL, &shell->command_paths, ecs_entity_t)[0] = a;
    ecs_singleton_modified(world, EcsScriptShell);

    r = ecs_shell_run(world, session, "custom");
    test_int(ecs_vec_count(&r.errors), 0);
    test_uint(*(ecs_entity_t*)r.result.ptr, b_custom);
    ecs_shell_result_fini(world, &r);

    r = ecs_shell_run(world, session, "ls");
    test_int(ecs_vec_count(&r.errors), 0);
    test_uint(*(ecs_entity_t*)r.result.ptr, b_ls);
    ecs_shell_result_fini(world, &r);

    ecs_fini(world);
}

void Command_help_custom_command_path(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptShell);

    ecs_entity_t commands = ecs_entity(world, { .name = "commands" });
    ecs_function(world, {
        .name = "custom",
        .parent = commands,
        .return_type = ecs_id(ecs_entity_t),
        .callback = Command_function_callback
    });

    ecs_function(world, {
        .name = "ls",
        .parent = commands,
        .return_type = ecs_id(ecs_entity_t),
        .callback = Command_function_callback
    });

    EcsScriptShell *shell = ecs_singleton_ensure(world, EcsScriptShell);
    ecs_vec_append_t(NULL, &shell->command_paths, ecs_entity_t)[0] = commands;
    ecs_singleton_modified(world, EcsScriptShell);

    ecs_entity_t session = ecs_shell_session_init(world, NULL);

    ecs_shell_result_t r = ecs_shell_run(world, session, "help");
    test_int(ecs_vec_count(&r.errors), 0);

    ecs_vec_t *infos = r.result.ptr;
    ecs_shell_info_t *info = ecs_vec_first(infos);
    int32_t i, count = ecs_vec_count(infos);
    int32_t custom_count = 0, ls_count = 0, click_count = 0, scripts_count = 0;
    for (i = 0; i < count; i ++) {
        if (i) {
            test_assert(strcmp(info[i - 1].name, info[i].name) < 0);
        }
        if (!strcmp(info[i].name, "custom")) {
            custom_count ++;
        } else if (!strcmp(info[i].name, "ls")) {
            ls_count ++;
            test_assert(info[i].return_type != ecs_id(ecs_entity_t));
        } else if (!strcmp(info[i].name, "click")) {
            click_count ++;
        } else if (!strcmp(info[i].name, "scripts")) {
            scripts_count ++;
        }
    }

    test_int(custom_count, 1);
    test_int(ls_count, 1);
    test_int(click_count, 1);
    test_int(scripts_count, 1);
    ecs_shell_result_fini(world, &r);

    r = ecs_shell_run(world, session, "help custom");
    test_int(ecs_vec_count(&r.errors), 0);
    test_int(ecs_vec_count(r.result.ptr), 1);
    ecs_shell_result_fini(world, &r);

    ecs_fini(world);
}

void Command_no_command_paths(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptShell);

    ecs_entity_t session = ecs_shell_session_init(world, NULL);

    EcsScriptShell *shell = ecs_singleton_ensure(world, EcsScriptShell);
    ecs_vec_clear(&shell->command_paths);
    ecs_singleton_modified(world, EcsScriptShell);

    ecs_shell_result_t r = ecs_shell_run(world, session, "ls");
    test_int(ecs_vec_count(&r.errors), 1);
    test_str(ecs_vec_first_t(&r.errors, char*)[0], "unknown command 'ls'");
    ecs_shell_result_fini(world, &r);

    ecs_fini(world);
}

void Command_command_path_from_script(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptShell);

    ecs_entity_t commands = ecs_entity(world, { .name = "commands" });
    ecs_entity_t custom = ecs_function(world, {
        .name = "custom",
        .parent = commands,
        .return_type = ecs_id(ecs_entity_t),
        .callback = Command_function_callback
    });

    test_assert(ecs_script_run(world, "test",
        "$ { flecs.script.shell.Shell: {command_paths: ["
            "flecs.script.shell.commands.core, commands]} }", NULL) == 0);

    const EcsScriptShell *shell = ecs_singleton_get(world, EcsScriptShell);
    test_int(ecs_vec_count(&shell->command_paths), 2);

    ecs_entity_t session = ecs_shell_session_init(world, NULL);

    ecs_shell_result_t r = ecs_shell_run(world, session, "custom");
    test_int(ecs_vec_count(&r.errors), 0);
    test_uint(*(ecs_entity_t*)r.result.ptr, custom);
    ecs_shell_result_fini(world, &r);

    r = ecs_shell_run(world, session, "scripts");
    test_int(ecs_vec_count(&r.errors), 1);
    ecs_shell_result_fini(world, &r);

    ecs_fini(world);
}
