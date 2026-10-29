#include <script.h>

#define COMMANDRESOURCE_FS_MAX (16)

typedef struct CommandResource_fs_file_t {
    char path[128];
    char *data;
    size_t size;
    size_t pos;
    bool used;
} CommandResource_fs_file_t;

typedef struct CommandResource_fs_dir_t {
    char path[128];
    bool used;
} CommandResource_fs_dir_t;

static CommandResource_fs_file_t CommandResource_fs_files[COMMANDRESOURCE_FS_MAX];
static CommandResource_fs_dir_t CommandResource_fs_dirs[COMMANDRESOURCE_FS_MAX];

static CommandResource_fs_file_t* CommandResource_fs_file_find(const char *path) {
    int32_t i;
    for (i = 0; i < COMMANDRESOURCE_FS_MAX; i ++) {
        if (CommandResource_fs_files[i].used && !strcmp(CommandResource_fs_files[i].path, path)) {
            return &CommandResource_fs_files[i];
        }
    }
    return NULL;
}

static CommandResource_fs_dir_t* CommandResource_fs_dir_find(const char *path, size_t len) {
    int32_t i;
    for (i = 0; i < COMMANDRESOURCE_FS_MAX; i ++) {
        if (CommandResource_fs_dirs[i].used && strlen(CommandResource_fs_dirs[i].path) == len &&
            !strncmp(CommandResource_fs_dirs[i].path, path, len))
        {
            return &CommandResource_fs_dirs[i];
        }
    }
    return NULL;
}

static bool CommandResource_fs_parent_exists(const char *path) {
    const char *sep = strrchr(path, '/');
    return !sep || CommandResource_fs_dir_find(path, (size_t)(sep - path)) != NULL;
}

static FILE* CommandResource_fs_fopen(const char *path, const char *mode) {
    CommandResource_fs_file_t *f = CommandResource_fs_file_find(path);
    if (!strchr(mode, 'w')) {
        if (f) {
            f->pos = 0;
        }
        return (FILE*)f;
    }

    if (!CommandResource_fs_parent_exists(path)) {
        return NULL;
    }

    if (!f) {
        int32_t i;
        for (i = 0; i < COMMANDRESOURCE_FS_MAX; i ++) {
            if (!CommandResource_fs_files[i].used) {
                f = &CommandResource_fs_files[i];
                break;
            }
        }

        test_assert(f != NULL);
        test_assert(strlen(path) < sizeof(f->path));
        strcpy(f->path, path);
        f->used = true;
    }

    free(f->data);
    f->data = NULL;
    f->size = 0;
    f->pos = 0;
    return (FILE*)f;
}

static size_t CommandResource_fs_fread(void *ptr, size_t size, size_t count, FILE *file) {
    CommandResource_fs_file_t *f = (CommandResource_fs_file_t*)file;
    size_t requested = size * count;
    if (requested > (f->size - f->pos)) {
        requested = f->size - f->pos;
    }

    if (requested) {
        memcpy(ptr, &f->data[f->pos], requested);
    }
    f->pos += requested;
    return size ? requested / size : 0;
}

static size_t CommandResource_fs_fwrite(
    const void *ptr,
    size_t size,
    size_t count,
    FILE *file)
{
    CommandResource_fs_file_t *f = (CommandResource_fs_file_t*)file;
    size_t len = size * count;
    if (!len) {
        return count;
    }
    f->data = realloc(f->data, f->pos + len);
    memcpy(&f->data[f->pos], ptr, len);
    f->pos += len;
    if (f->pos > f->size) {
        f->size = f->pos;
    }
    return count;
}

static void CommandResource_fs_fclose(FILE *file) {
    test_assert(file != NULL);
}

static int CommandResource_fs_mkdir(const char *path) {
    if (CommandResource_fs_dir_find(path, strlen(path))) {
        return 0;
    }

    if (!CommandResource_fs_parent_exists(path)) {
        return -1;
    }

    int32_t i;
    for (i = 0; i < COMMANDRESOURCE_FS_MAX; i ++) {
        if (!CommandResource_fs_dirs[i].used) {
            test_assert(strlen(path) < sizeof(CommandResource_fs_dirs[i].path));
            strcpy(CommandResource_fs_dirs[i].path, path);
            CommandResource_fs_dirs[i].used = true;
            return 0;
        }
    }

    test_assert(false);
    return -1;
}

static int CommandResource_fs_rmdir(const char *path) {
    CommandResource_fs_dir_t *d = CommandResource_fs_dir_find(path, strlen(path));
    if (!d) {
        return -1;
    }

    size_t len = strlen(path);
    int32_t i;
    for (i = 0; i < COMMANDRESOURCE_FS_MAX; i ++) {
        if (CommandResource_fs_files[i].used && !strncmp(CommandResource_fs_files[i].path, path, len) &&
            CommandResource_fs_files[i].path[len] == '/')
        {
            return -1;
        }
        if (CommandResource_fs_dirs[i].used && !strncmp(CommandResource_fs_dirs[i].path, path, len) &&
            CommandResource_fs_dirs[i].path[len] == '/')
        {
            return -1;
        }
    }

    d->used = false;
    return 0;
}

static int CommandResource_fs_remove(const char *path) {
    CommandResource_fs_file_t *f = CommandResource_fs_file_find(path);
    if (!f) {
        return -1;
    }

    free(f->data);
    memset(f, 0, sizeof(*f));
    return 0;
}

static int32_t CommandResource_fs_file_count(void) {
    int32_t i, count = 0;
    for (i = 0; i < COMMANDRESOURCE_FS_MAX; i ++) {
        count += CommandResource_fs_files[i].used;
    }
    return count;
}

static int32_t CommandResource_fs_dir_count(void) {
    int32_t i, count = 0;
    for (i = 0; i < COMMANDRESOURCE_FS_MAX; i ++) {
        count += CommandResource_fs_dirs[i].used;
    }
    return count;
}

void CommandResource_setup(void) {
    memset(CommandResource_fs_files, 0, sizeof(CommandResource_fs_files));
    memset(CommandResource_fs_dirs, 0, sizeof(CommandResource_fs_dirs));

    ecs_os_set_api_defaults();
    ecs_os_api_t os_api = ecs_os_api;
    os_api.fopen_ = CommandResource_fs_fopen;
    os_api.fread_ = CommandResource_fs_fread;
    os_api.fwrite_ = CommandResource_fs_fwrite;
    os_api.fclose_ = CommandResource_fs_fclose;
    os_api.mkdir_ = CommandResource_fs_mkdir;
    os_api.rmdir_ = CommandResource_fs_rmdir;
    os_api.remove_ = CommandResource_fs_remove;
    ecs_os_set_api(&os_api);
}


static void CommandResource_callback(
    const ecs_function_ctx_t *ctx,
    int32_t argc,
    const ecs_value_t *argv,
    ecs_value_t *result)
{
    (void)argc;
    (void)argv;
    ecs_command_resource_create(ctx->world,
        ecs_command_get_session(ctx->world), "image/png", "PNG", 3,
        result->ptr);
}

void CommandResource_lookup_type(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptShell);

    test_assert(ecs_id(ecs_command_resource_t) != 0);
    test_uint(ecs_lookup(world, "flecs.script.shell.resource"),
        ecs_id(ecs_command_resource_t));

    ecs_fini(world);
}

void CommandResource_create(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptShell);

    ecs_entity_t session = ecs_command_session_init(world, NULL);

    ecs_command_resource_t r = {0};
    test_int(0, ecs_command_resource_create(
        world, session, "text/plain", "Hello", 5, &r));
    test_str(r.name, "1");
    test_str(r.content_type, "text/plain");

    const EcsCommandSession *s = ecs_get(world, session, EcsCommandSession);
    test_int(ecs_vec_count(&s->resources), 1);
    ecs_command_resource_t *sr = ecs_vec_first(&s->resources);
    test_str(sr[0].name, "1");
    test_str(sr[0].content_type, "text/plain");

    char path[128];
    ecs_os_snprintf(path, 128, ".flecs/shell/%llu",
        (unsigned long long)session);
    test_str(s->resource_path, path);

    char file[128];
    ecs_os_snprintf(file, 128, ".flecs/shell/%llu/1",
        (unsigned long long)session);
    FILE *f = ecs_os_fopen(file, "rb");
    test_assert(f != NULL);
    char buf[16] = {0};
    test_int(ecs_os_fread(buf, 1, 15, f), 5);
    ecs_os_fclose(f);
    test_str(buf, "Hello");
    test_int(CommandResource_fs_file_count(), 1);
    test_int(CommandResource_fs_dir_count(), 3);

    ecs_os_free(r.name);
    ecs_os_free(r.content_type);

    ecs_fini(world);
}

void CommandResource_create_multiple(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptShell);

    ecs_entity_t session = ecs_command_session_init(world, NULL);

    ecs_command_resource_t r1 = {0}, r2 = {0}, r3 = {0};
    test_int(0, ecs_command_resource_create(
        world, session, "text/plain", "a", 1, &r1));
    test_int(0, ecs_command_resource_create(
        world, session, "text/plain", "b", 1, &r2));
    test_int(0, ecs_command_resource_create(
        world, session, "text/plain", "c", 1, &r3));
    test_str(r1.name, "1");
    test_str(r2.name, "2");
    test_str(r3.name, "3");

    ecs_size_t size = 0;
    char *data = ecs_command_resource_read(world, session, "2", &size, NULL);
    test_assert(data != NULL);
    test_int(size, 1);
    test_assert(data[0] == 'b');
    ecs_os_free(data);

    ecs_os_free(r1.name);
    ecs_os_free(r1.content_type);
    ecs_os_free(r2.name);
    ecs_os_free(r2.content_type);
    ecs_os_free(r3.name);
    ecs_os_free(r3.content_type);

    ecs_fini(world);
}

void CommandResource_create_default_content_type(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptShell);

    ecs_entity_t session = ecs_command_session_init(world, NULL);

    ecs_command_resource_t r = {0};
    test_int(0, ecs_command_resource_create(
        world, session, NULL, "Hello", 5, &r));
    test_str(r.name, "1");
    test_str(r.content_type, "application/octet-stream");

    ecs_os_free(r.name);
    ecs_os_free(r.content_type);

    ecs_fini(world);
}

void CommandResource_create_empty(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptShell);

    ecs_entity_t session = ecs_command_session_init(world, NULL);

    test_int(0, ecs_command_resource_create(
        world, session, "text/plain", NULL, 0, NULL));

    ecs_size_t size = -1;
    const char *content_type = NULL;
    void *data = ecs_command_resource_read(
        world, session, "1", &size, &content_type);
    test_assert(data != NULL);
    test_int(size, 0);
    test_str(content_type, "text/plain");
    ecs_os_free(data);

    ecs_fini(world);
}

void CommandResource_create_not_a_session(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptShell);

    ecs_entity_t e = ecs_new(world);

    ecs_log_set_level(-4);
    test_assert(0 != ecs_command_resource_create(
        world, e, "text/plain", "Hello", 5, NULL));

    test_int(CommandResource_fs_file_count(), 0);
    test_int(CommandResource_fs_dir_count(), 0);

    ecs_fini(world);
}

void CommandResource_no_dir_without_resource(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptShell);

    ecs_entity_t session = ecs_command_session_init(world, NULL);
    ecs_command_result_t r = ecs_command_run(world, session, "pwd");
    test_int(ecs_vec_count(&r.errors), 0);
    ecs_command_result_fini(world, &r);

    const EcsCommandSession *s = ecs_get(world, session, EcsCommandSession);
    test_assert(s->resource_path == NULL);
    test_int(ecs_vec_count(&s->resources), 0);
    test_int(CommandResource_fs_file_count(), 0);
    test_int(CommandResource_fs_dir_count(), 0);

    ecs_fini(world);
}

void CommandResource_names_per_session(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptShell);

    ecs_entity_t session_a = ecs_command_session_init(world, NULL);
    ecs_entity_t session_b = ecs_command_session_init(world, NULL);

    test_int(0, ecs_command_resource_create(
        world, session_a, "text/plain", "a", 1, NULL));
    test_int(0, ecs_command_resource_create(
        world, session_b, "text/plain", "b", 1, NULL));

    ecs_size_t size = 0;
    char *data = ecs_command_resource_read(world, session_a, "1", &size, NULL);
    test_assert(data != NULL);
    test_int(size, 1);
    test_assert(data[0] == 'a');
    ecs_os_free(data);

    data = ecs_command_resource_read(world, session_b, "1", &size, NULL);
    test_assert(data != NULL);
    test_int(size, 1);
    test_assert(data[0] == 'b');
    ecs_os_free(data);

    ecs_fini(world);
}

void CommandResource_read(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptShell);

    ecs_entity_t session = ecs_command_session_init(world, NULL);

    test_int(0, ecs_command_resource_create(
        world, session, "text/plain", "Hello World", 11, NULL));

    ecs_size_t size = 0;
    const char *content_type = NULL;
    char *data = ecs_command_resource_read(
        world, session, "1", &size, &content_type);
    test_assert(data != NULL);
    test_int(size, 11);
    test_assert(!memcmp(data, "Hello World", 11));
    test_str(content_type, "text/plain");
    ecs_os_free(data);

    ecs_fini(world);
}

void CommandResource_read_binary(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptShell);

    ecs_entity_t session = ecs_command_session_init(world, NULL);

    unsigned char bytes[10000];
    int i;
    for (i = 0; i < 10000; i ++) {
        bytes[i] = (unsigned char)(i * 7);
    }

    test_int(0, ecs_command_resource_create(
        world, session, "image/png", bytes, 10000, NULL));

    ecs_size_t size = 0;
    void *data = ecs_command_resource_read(world, session, "1", &size, NULL);
    test_assert(data != NULL);
    test_int(size, 10000);
    test_assert(!memcmp(data, bytes, 10000));
    ecs_os_free(data);

    ecs_fini(world);
}

void CommandResource_read_unknown(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptShell);

    ecs_entity_t session = ecs_command_session_init(world, NULL);

    ecs_size_t size = 0;
    test_assert(NULL == ecs_command_resource_read(
        world, session, "1", &size, NULL));

    test_int(0, ecs_command_resource_create(
        world, session, "text/plain", "Hello", 5, NULL));

    test_assert(NULL == ecs_command_resource_read(
        world, session, "2", &size, NULL));
    test_assert(NULL == ecs_command_resource_read(
        world, session, "", &size, NULL));
    test_assert(NULL == ecs_command_resource_read(
        world, session, "./1", &size, NULL));
    test_assert(NULL == ecs_command_resource_read(
        world, session, "../../../flecs.h", &size, NULL));
    test_int(size, 0);

    ecs_fini(world);
}

void CommandResource_read_other_session(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptShell);

    ecs_entity_t session_a = ecs_command_session_init(world, NULL);
    ecs_entity_t session_b = ecs_command_session_init(world, NULL);

    test_int(0, ecs_command_resource_create(
        world, session_a, "text/plain", "Hello", 5, NULL));

    ecs_size_t size = 0;
    test_assert(NULL == ecs_command_resource_read(
        world, session_b, "1", &size, NULL));

    ecs_fini(world);
}

void CommandResource_read_not_a_session(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptShell);

    ecs_entity_t e = ecs_new(world);

    ecs_size_t size = 0;
    test_assert(NULL == ecs_command_resource_read(
        world, e, "1", &size, NULL));
    test_assert(NULL == ecs_command_resource_read(
        world, 0, "1", &size, NULL));

    ecs_fini(world);
}

void CommandResource_delete_session(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptShell);

    ecs_entity_t session = ecs_command_session_init(world, NULL);

    test_int(0, ecs_command_resource_create(
        world, session, "text/plain", "a", 1, NULL));
    test_int(0, ecs_command_resource_create(
        world, session, "text/plain", "b", 1, NULL));

    char file[128];
    ecs_os_snprintf(file, 128, ".flecs/shell/%llu/1",
        (unsigned long long)session);
    FILE *f = ecs_os_fopen(file, "rb");
    test_assert(f != NULL);
    ecs_os_fclose(f);

    ecs_delete(world, session);

    test_assert(ecs_os_fopen(file, "rb") == NULL);
    ecs_os_snprintf(file, 128, ".flecs/shell/%llu/2",
        (unsigned long long)session);
    test_assert(ecs_os_fopen(file, "rb") == NULL);

    test_int(CommandResource_fs_file_count(), 0);
    test_int(CommandResource_fs_dir_count(), 2);

    ecs_fini(world);
}

void CommandResource_delete_session_keeps_other_session(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptShell);

    ecs_entity_t session_a = ecs_command_session_init(world, NULL);
    ecs_entity_t session_b = ecs_command_session_init(world, NULL);

    test_int(0, ecs_command_resource_create(
        world, session_a, "text/plain", "a", 1, NULL));
    test_int(0, ecs_command_resource_create(
        world, session_b, "text/plain", "b", 1, NULL));

    ecs_delete(world, session_a);
    test_int(CommandResource_fs_file_count(), 1);
    test_int(CommandResource_fs_dir_count(), 3);

    ecs_size_t size = 0;
    char *data = ecs_command_resource_read(world, session_b, "1", &size, NULL);
    test_assert(data != NULL);
    test_int(size, 1);
    test_assert(data[0] == 'b');
    ecs_os_free(data);

    ecs_fini(world);
}

void CommandResource_world_fini(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptShell);

    ecs_entity_t session = ecs_command_session_init(world, NULL);

    test_int(0, ecs_command_resource_create(
        world, session, "text/plain", "a", 1, NULL));

    ecs_fini(world);

    char file[128];
    ecs_os_snprintf(file, 128, ".flecs/shell/%llu/1",
        (unsigned long long)session);
    test_assert(ecs_os_fopen(file, "rb") == NULL);

    test_int(CommandResource_fs_file_count(), 0);
    test_int(CommandResource_fs_dir_count(), 2);
}

void CommandResource_command_returns_resource(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptShell);

    ecs_entity_t commands = ecs_entity(world, { .name = "commands" });
    ecs_function(world, {
        .name = "capture",
        .parent = commands,
        .return_type = ecs_id(ecs_command_resource_t),
        .callback = CommandResource_callback
    });

    EcsScriptShell *shell = ecs_singleton_ensure(world, EcsScriptShell);
    ecs_vec_append_t(NULL, &shell->command_paths, ecs_entity_t)[0] = commands;
    ecs_singleton_modified(world, EcsScriptShell);

    ecs_entity_t session = ecs_command_session_init(world, NULL);

    ecs_command_result_t r = ecs_command_run(world, session, "capture");
    test_int(ecs_vec_count(&r.errors), 0);
    test_uint(r.result.type, ecs_id(ecs_command_resource_t));
    ecs_command_resource_t *res = r.result.ptr;
    test_str(res->name, "1");
    test_str(res->content_type, "image/png");
    ecs_command_result_fini(world, &r);

    r = ecs_command_run(world, session, "capture");
    test_int(ecs_vec_count(&r.errors), 0);
    res = r.result.ptr;
    test_str(res->name, "2");
    ecs_command_result_fini(world, &r);

    ecs_size_t size = 0;
    char *data = ecs_command_resource_read(world, session, "2", &size, NULL);
    test_assert(data != NULL);
    test_int(size, 3);
    test_assert(!memcmp(data, "PNG", 3));
    ecs_os_free(data);

    ecs_fini(world);
}
