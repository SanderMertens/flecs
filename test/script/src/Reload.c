#include <script.h>

typedef struct reload_file_t {
    const char *name;
    const char *content;
    size_t pos;
} reload_file_t;

#define RELOAD_FILE_MAX (4)

static reload_file_t reload_files[RELOAD_FILE_MAX];

static ecs_os_api_fopen_t reload_default_fopen = NULL;
static ecs_os_api_fread_t reload_default_fread = NULL;
static ecs_os_api_fclose_t reload_default_fclose = NULL;

static reload_file_t* reload_file_get(FILE *file) {
    int32_t i;
    for (i = 0; i < RELOAD_FILE_MAX; i ++) {
        if (file == (FILE*)&reload_files[i]) {
            return &reload_files[i];
        }
    }
    return NULL;
}

static FILE* reload_fopen(const char *file, const char *mode) {
    int32_t i;
    for (i = 0; i < RELOAD_FILE_MAX; i ++) {
        if (reload_files[i].name && !strcmp(file, reload_files[i].name)) {
            reload_files[i].pos = 0;
            return (FILE*)&reload_files[i];
        }
    }
    return reload_default_fopen(file, mode);
}

static size_t reload_fread(void *ptr, size_t size, size_t count, FILE *file) {
    reload_file_t *f = reload_file_get(file);
    size_t remaining, requested;
    if (!f) {
        return reload_default_fread(ptr, size, count, file);
    }

    remaining = strlen(f->content) - f->pos;
    requested = size * count;
    if (requested > remaining) {
        requested = remaining;
    }

    memcpy(ptr, &f->content[f->pos], requested);
    f->pos += requested;
    return requested;
}

static void reload_fclose(FILE *file) {
    if (!reload_file_get(file)) {
        reload_default_fclose(file);
    }
}

void Reload_setup(void) {
    memset(reload_files, 0, sizeof(reload_files));

    ecs_os_set_api_defaults();
    reload_default_fopen = ecs_os_api.fopen_;
    reload_default_fread = ecs_os_api.fread_;
    reload_default_fclose = ecs_os_api.fclose_;

    ecs_os_api_t os_api = ecs_os_api;
    os_api.fopen_ = reload_fopen;
    os_api.fread_ = reload_fread;
    os_api.fclose_ = reload_fclose;
    ecs_os_set_api(&os_api);
}

void Reload_added_entity(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t s = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code = "foo {}\n"
    });
    test_assert(s != 0);
    test_assert(ecs_lookup(world, "foo") != 0);
    test_assert(ecs_lookup(world, "bar") == 0);

    test_int(0, ecs_script_reload(world, s, "foo {}\nbar {}\n"));

    test_assert(ecs_lookup(world, "foo") != 0);
    test_assert(ecs_lookup(world, "bar") != 0);

    ecs_fini(world);
}

void Reload_changed_entity(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_struct(world, {
        .entity = ecs_id(Position),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    ecs_entity_t s = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code = "foo { Position: {x: 10, y: 20} }\n"
    });
    test_assert(s != 0);

    ecs_entity_t foo = ecs_lookup(world, "foo");
    test_assert(foo != 0);
    {
        const Position *p = ecs_get(world, foo, Position);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 20);
    }

    test_int(0, ecs_script_reload(
        world, s, "foo { Position: {x: 30, y: 40} }\n"));

    foo = ecs_lookup(world, "foo");
    test_assert(foo != 0);
    {
        const Position *p = ecs_get(world, foo, Position);
        test_assert(p != NULL);
        test_int(p->x, 30);
        test_int(p->y, 40);
    }

    ecs_fini(world);
}

void Reload_removed_entity(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t s = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code = "foo {}\nbar {}\n"
    });
    test_assert(s != 0);
    test_assert(ecs_lookup(world, "foo") != 0);
    test_assert(ecs_lookup(world, "bar") != 0);

    test_int(0, ecs_script_reload(world, s, "foo {}\n"));

    test_assert(ecs_lookup(world, "foo") != 0);
    test_assert(ecs_lookup(world, "bar") == 0);

    ecs_fini(world);
}

void Reload_removed_component(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);

    ecs_entity_t s = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code = "foo { Tag }\n"
    });
    test_assert(s != 0);

    ecs_entity_t foo = ecs_lookup(world, "foo");
    test_assert(foo != 0);
    test_assert(ecs_has(world, foo, Tag));

    test_int(0, ecs_script_reload(world, s, "foo {}\n"));

    foo = ecs_lookup(world, "foo");
    test_assert(foo != 0);
    test_assert(!ecs_has(world, foo, Tag));

    ecs_fini(world);
}

void Reload_parse_error_keeps_world(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t s = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code = "foo {}\nbar {}\n"
    });
    test_assert(s != 0);
    test_assert(ecs_lookup(world, "foo") != 0);
    test_assert(ecs_lookup(world, "bar") != 0);

    ecs_log_set_level(-4);
    test_int(-1, ecs_script_reload(world, s, "foo {\n"));
    ecs_log_set_level(-1);

    test_assert(ecs_lookup(world, "foo") != 0);
    test_assert(ecs_lookup(world, "bar") != 0);

    const EcsScript *script = ecs_get(world, s, EcsScript);
    test_assert(script != NULL);
    test_assert(script->error != NULL);
    test_assert(script->script != NULL);
    test_str(script->code, "foo {}\nbar {}\n");

    ecs_fini(world);
}

void Reload_parse_error_then_reload(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t s = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code = "foo {}\n"
    });
    test_assert(s != 0);

    ecs_log_set_level(-4);
    test_int(-1, ecs_script_reload(world, s, "foo {\n"));
    ecs_log_set_level(-1);

    test_int(0, ecs_script_reload(world, s, "bar {}\n"));

    test_assert(ecs_lookup(world, "foo") == 0);
    test_assert(ecs_lookup(world, "bar") != 0);

    const EcsScript *script = ecs_get(world, s, EcsScript);
    test_assert(script != NULL);
    test_assert(script->error == NULL);

    ecs_fini(world);
}

void Reload_from_file(void) {
    reload_files[0] = (reload_file_t){ "scene.flecs", "foo {}\n", 0 };

    ecs_world_t *world = ecs_init();

    ecs_entity_t s = ecs_script(world, {
        .filename = "scene.flecs"
    });
    test_assert(s != 0);
    test_assert(ecs_lookup(world, "foo") != 0);
    test_assert(ecs_lookup(world, "bar") == 0);

    reload_files[0].content = "foo {}\nbar {}\n";

    test_int(0, ecs_script_reload(world, s, NULL));

    test_assert(ecs_lookup(world, "foo") != 0);
    test_assert(ecs_lookup(world, "bar") != 0);

    ecs_fini(world);
}

void Reload_from_file_removed_entity(void) {
    reload_files[0] = (reload_file_t){ "scene.flecs", "foo {}\nbar {}\n", 0 };

    ecs_world_t *world = ecs_init();

    ecs_entity_t s = ecs_script(world, {
        .filename = "scene.flecs"
    });
    test_assert(s != 0);
    test_assert(ecs_lookup(world, "foo") != 0);
    test_assert(ecs_lookup(world, "bar") != 0);

    reload_files[0].content = "bar {}\n";

    test_int(0, ecs_script_reload(world, s, NULL));

    test_assert(ecs_lookup(world, "foo") == 0);
    test_assert(ecs_lookup(world, "bar") != 0);

    ecs_fini(world);
}

void Reload_from_file_parse_error_keeps_world(void) {
    reload_files[0] = (reload_file_t){ "scene.flecs", "foo {}\n", 0 };

    ecs_world_t *world = ecs_init();

    ecs_entity_t s = ecs_script(world, {
        .filename = "scene.flecs"
    });
    test_assert(s != 0);
    test_assert(ecs_lookup(world, "foo") != 0);

    reload_files[0].content = "foo {\n";

    ecs_log_set_level(-4);
    test_int(-1, ecs_script_reload(world, s, NULL));
    ecs_log_set_level(-1);

    test_assert(ecs_lookup(world, "foo") != 0);

    const EcsScript *script = ecs_get(world, s, EcsScript);
    test_assert(script != NULL);
    test_assert(script->error != NULL);
    test_str(script->code, "foo {}\n");

    ecs_fini(world);
}

void Reload_without_filename(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t s = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code = "foo {}\n"
    });
    test_assert(s != 0);

    ecs_log_set_level(-4);
    test_int(-1, ecs_script_reload(world, s, NULL));
    ecs_log_set_level(-1);

    test_assert(ecs_lookup(world, "foo") != 0);

    ecs_fini(world);
}

void Reload_not_a_script(void) {
    ecs_world_t *world = ecs_init();

    ECS_IMPORT(world, FlecsScript);

    ecs_entity_t e = ecs_new(world);

    ecs_log_set_level(-4);
    test_int(-1, ecs_script_reload(world, e, "foo {}\n"));
    ecs_log_set_level(-1);

    test_assert(ecs_lookup(world, "foo") == 0);

    ecs_fini(world);
}

void Reload_keeps_entities_outside_script(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t manual = ecs_entity(world, { .name = "manual" });

    ecs_entity_t s = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code = "foo {}\n"
    });
    test_assert(s != 0);

    test_int(0, ecs_script_reload(world, s, "bar {}\n"));

    test_assert(ecs_is_alive(world, manual));
    test_assert(ecs_lookup(world, "manual") == manual);
    test_assert(ecs_lookup(world, "foo") == 0);
    test_assert(ecs_lookup(world, "bar") != 0);

    ecs_fini(world);
}
