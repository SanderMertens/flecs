#include <script.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#ifdef _WIN32
#include <direct.h>
#define test_mkdir(p) _mkdir(p)
#define test_rmdir(p) _rmdir(p)
#else
#include <unistd.h>
#define test_mkdir(p) mkdir(p, 0755)
#define test_rmdir(p) rmdir(p)
#endif

static
void test_write_file(const char *path, const char *content) {
    FILE *f = fopen(path, "w");
    test_assert(f != NULL);
    if (content) {
        fwrite(content, 1, strlen(content), f);
    }
    fclose(f);
}

static
void test_unlink(const char *path) {
    remove(path);
}

static const char *g_fopen_remap_from;
static const char *g_fopen_remap_to;

#define DIAG(...) do { \
    fprintf(stderr, "[diag] " __VA_ARGS__); \
    fputc('\n', stderr); \
    fflush(stderr); \
} while (0)

static int s_hook_call_count = 0;

static
FILE* test_fopen_remap(const char *file, const char *mode) {
    s_hook_call_count ++;
    DIAG("hook called: file=%s, mode=%s", file ? file : "(null)", mode ? mode : "(null)");
    const char *actual = file;
    int matched = g_fopen_remap_from && file && !strcmp(file, g_fopen_remap_from);
    DIAG("hook: match=%d, from=%s, to=%s", matched,
        g_fopen_remap_from ? g_fopen_remap_from : "(null)",
        g_fopen_remap_to ? g_fopen_remap_to : "(null)");
    if (matched) {
        actual = g_fopen_remap_to;
    }
#ifndef ECS_TARGET_POSIX
    FILE *result = NULL;
    errno_t err = fopen_s(&result, actual, mode);
    DIAG("hook: fopen_s('%s','%s') => err=%d, result=%p", actual, mode, (int)err, (void*)result);
    return result;
#else
    FILE *r = fopen(actual, mode);
    DIAG("hook: fopen('%s','%s') => %p", actual, mode, (void*)r);
    return r;
#endif
}

static
const char* test_tmp_dir(const char *name) {
    static char buf[1024];
    snprintf(buf, sizeof(buf), "flecs_include_test_%s", name);
    test_rmdir(buf);
    test_mkdir(buf);
    return buf;
}

void Include_include_simple(void) {
    ecs_world_t *world = ecs_init();

    const char *dir = test_tmp_dir("simple");
    char parent_path[256], child_path[256];
    snprintf(parent_path, sizeof(parent_path), "%s/parent.flecs", dir);
    snprintf(child_path, sizeof(child_path), "%s/child.flecs", dir);

    test_write_file(child_path, "Foo{}\n");
    test_write_file(parent_path, "include child.flecs\n");

    test_assert(ecs_script_run_file(world, parent_path) == 0);

    test_assert(ecs_lookup(world, "Foo") != 0);

    test_unlink(child_path);
    test_unlink(parent_path);
    test_rmdir(dir);

    ecs_fini(world);
}

void Include_include_subdir(void) {
    ecs_world_t *world = ecs_init();

    const char *dir = test_tmp_dir("subdir");
    char sub[256], parent_path[256], child_path[256];
    snprintf(sub, sizeof(sub), "%s/sub", dir);
    test_mkdir(sub);
    snprintf(parent_path, sizeof(parent_path), "%s/parent.flecs", dir);
    snprintf(child_path, sizeof(child_path), "%s/child.flecs", sub);

    test_write_file(child_path, "Bar{}\n");
    test_write_file(parent_path, "include sub/child.flecs\n");

    test_assert(ecs_script_run_file(world, parent_path) == 0);
    test_assert(ecs_lookup(world, "Bar") != 0);

    test_unlink(child_path);
    test_unlink(parent_path);
    test_rmdir(sub);
    test_rmdir(dir);

    ecs_fini(world);
}

void Include_include_missing_file(void) {
    install_test_abort();

    ecs_world_t *world = ecs_init();

    const char *dir = test_tmp_dir("missing");
    char parent_path[256];
    snprintf(parent_path, sizeof(parent_path), "%s/parent.flecs", dir);

    test_write_file(parent_path, "include does_not_exist.flecs\n");

    test_assert(ecs_script_run_file(world, parent_path) != 0);

    test_unlink(parent_path);
    test_rmdir(dir);

    ecs_fini(world);
}

void Include_include_parent_dir_not_allowed(void) {
    install_test_abort();

    ecs_world_t *world = ecs_init();

    const char *dir = test_tmp_dir("parent_dir");
    char parent_path[256];
    snprintf(parent_path, sizeof(parent_path), "%s/parent.flecs", dir);

    test_write_file(parent_path, "include ../other.flecs\n");

    test_assert(ecs_script_run_file(world, parent_path) != 0);

    test_unlink(parent_path);
    test_rmdir(dir);

    ecs_fini(world);
}

void Include_include_relative_to_current_script(void) {
    ecs_world_t *world = ecs_init();

    const char *dir = test_tmp_dir("relative");
    char sub[256], parent_path[256], a_path[256], b_path[256];
    snprintf(sub, sizeof(sub), "%s/sub", dir);
    test_mkdir(sub);
    snprintf(parent_path, sizeof(parent_path), "%s/parent.flecs", dir);
    snprintf(a_path, sizeof(a_path), "%s/a.flecs", sub);
    snprintf(b_path, sizeof(b_path), "%s/b.flecs", sub);

    test_write_file(b_path, "B_entity{}\n");
    test_write_file(a_path, "include b.flecs\n");
    test_write_file(parent_path, "include sub/a.flecs\n");

    test_assert(ecs_script_run_file(world, parent_path) == 0);
    test_assert(ecs_lookup(world, "B_entity") != 0);

    test_unlink(a_path);
    test_unlink(b_path);
    test_unlink(parent_path);
    test_rmdir(sub);
    test_rmdir(dir);

    ecs_fini(world);
}

void Include_include_nested(void) {
    ecs_world_t *world = ecs_init();

    const char *dir = test_tmp_dir("nested");
    char a_path[256], b_path[256], c_path[256];
    snprintf(a_path, sizeof(a_path), "%s/a.flecs", dir);
    snprintf(b_path, sizeof(b_path), "%s/b.flecs", dir);
    snprintf(c_path, sizeof(c_path), "%s/c.flecs", dir);

    test_write_file(c_path, "C_entity{}\n");
    test_write_file(b_path, "include c.flecs\nB_entity{}\n");
    test_write_file(a_path, "include b.flecs\nA_entity{}\n");

    test_assert(ecs_script_run_file(world, a_path) == 0);
    test_assert(ecs_lookup(world, "A_entity") != 0);
    test_assert(ecs_lookup(world, "B_entity") != 0);
    test_assert(ecs_lookup(world, "C_entity") != 0);

    test_unlink(a_path);
    test_unlink(b_path);
    test_unlink(c_path);
    test_rmdir(dir);

    ecs_fini(world);
}

void Include_include_managed_creates_script_entity(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScript);

    const char *dir = test_tmp_dir("managed_creates");
    char parent_path[256], child_path[256];
    snprintf(parent_path, sizeof(parent_path), "%s/parent.flecs", dir);
    snprintf(child_path, sizeof(child_path), "%s/child.flecs", dir);

    test_write_file(child_path, "ChildEntity{}\n");
    test_write_file(parent_path, "include child.flecs\n");

    ecs_entity_t script = ecs_script(world, {
        .filename = parent_path
    });
    test_assert(script != 0);

    test_assert(ecs_lookup(world, "ChildEntity") != 0);

    char child_lookup[256];
    snprintf(child_lookup, sizeof(child_lookup), "%s/child.flecs", dir);
    ecs_entity_t child_script = ecs_lookup_path_w_sep(
        world, 0, child_lookup, "/", NULL, false);
    test_assert(child_script != 0);
    test_assert(ecs_has(world, child_script, EcsScript));

    test_unlink(child_path);
    test_unlink(parent_path);
    test_rmdir(dir);

    ecs_fini(world);
}

void Include_include_managed_skips_existing(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScript);

    const char *dir = test_tmp_dir("managed_skips");
    char parent_path[256], child_path[256];
    snprintf(parent_path, sizeof(parent_path), "%s/parent.flecs", dir);
    snprintf(child_path, sizeof(child_path), "%s/child.flecs", dir);

    test_write_file(child_path, "ChildEntity{}\n");
    test_write_file(parent_path, "include child.flecs\n");

    ecs_entity_t script = ecs_script(world, {
        .filename = parent_path
    });
    test_assert(script != 0);

    ecs_entity_t child_entity1 = ecs_lookup(world, "ChildEntity");
    test_assert(child_entity1 != 0);

    test_assert(ecs_script_update(world, script, 0, "include child.flecs\n") == 0);

    ecs_entity_t child_entity2 = ecs_lookup(world, "ChildEntity");
    test_assert(child_entity2 != 0);
    test_assert(child_entity1 == child_entity2);

    test_unlink(child_path);
    test_unlink(parent_path);
    test_rmdir(dir);

    ecs_fini(world);
}

void Include_include_managed_nested(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScript);

    const char *dir = test_tmp_dir("managed_nested");
    char a_path[256], b_path[256];
    snprintf(a_path, sizeof(a_path), "%s/a.flecs", dir);
    snprintf(b_path, sizeof(b_path), "%s/b.flecs", dir);

    test_write_file(b_path, "BEntity{}\n");
    test_write_file(a_path, "include b.flecs\n");

    ecs_entity_t script = ecs_script(world, {
        .filename = a_path
    });
    test_assert(script != 0);

    ecs_entity_t b_script = ecs_lookup_path_w_sep(
        world, 0, b_path, "/", NULL, false);
    test_assert(b_script != 0);
    test_assert(ecs_has(world, b_script, EcsScript));

    test_unlink(a_path);
    test_unlink(b_path);
    test_rmdir(dir);

    ecs_fini(world);
}

void Include_include_inline_does_not_create_script_entity(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScript);

    const char *dir = test_tmp_dir("inline");
    char parent_path[256], child_path[256];
    snprintf(parent_path, sizeof(parent_path), "%s/parent.flecs", dir);
    snprintf(child_path, sizeof(child_path), "%s/child.flecs", dir);

    test_write_file(child_path, "ChildInline{}\n");
    test_write_file(parent_path, "include child.flecs\n");

    test_assert(ecs_script_run_file(world, parent_path) == 0);

    test_assert(ecs_lookup(world, "ChildInline") != 0);

    ecs_entity_t child_script = ecs_lookup_path_w_sep(
        world, 0, child_path, "/", NULL, false);
    if (child_script) {
        test_assert(!ecs_has(world, child_script, EcsScript));
    }

    test_unlink(child_path);
    test_unlink(parent_path);
    test_rmdir(dir);

    ecs_fini(world);
}

void Include_include_rejects_absolute_unix_path(void) {
    install_test_abort();

    ecs_world_t *world = ecs_init();

    const char *dir = test_tmp_dir("abs_unix");
    char parent_path[256];
    snprintf(parent_path, sizeof(parent_path), "%s/parent.flecs", dir);

    test_write_file(parent_path, "include /etc/passwd\n");

    test_assert(ecs_script_run_file(world, parent_path) != 0);

    test_unlink(parent_path);
    test_rmdir(dir);

    ecs_fini(world);
}

void Include_include_rejects_windows_drive_letter(void) {
    install_test_abort();

    ecs_world_t *world = ecs_init();

    const char *dir = test_tmp_dir("abs_win");
    char parent_path[256];
    snprintf(parent_path, sizeof(parent_path), "%s/parent.flecs", dir);

    test_write_file(parent_path, "include C:/foo.flecs\n");

    test_assert(ecs_script_run_file(world, parent_path) != 0);

    test_unlink(parent_path);
    test_rmdir(dir);

    ecs_fini(world);
}

void Include_include_with_line_comment(void) {
    ecs_world_t *world = ecs_init();

    const char *dir = test_tmp_dir("line_comment");
    char parent_path[256], child_path[256];
    snprintf(parent_path, sizeof(parent_path), "%s/parent.flecs", dir);
    snprintf(child_path, sizeof(child_path), "%s/child.flecs", dir);

    test_write_file(child_path, "LineCommentEntity{}\n");
    test_write_file(parent_path, "include child.flecs // load the child\n");

    test_assert(ecs_script_run_file(world, parent_path) == 0);
    test_assert(ecs_lookup(world, "LineCommentEntity") != 0);

    test_unlink(child_path);
    test_unlink(parent_path);
    test_rmdir(dir);

    ecs_fini(world);
}

void Include_include_with_block_comment(void) {
    ecs_world_t *world = ecs_init();

    const char *dir = test_tmp_dir("block_comment");
    char parent_path[256], child_path[256];
    snprintf(parent_path, sizeof(parent_path), "%s/parent.flecs", dir);
    snprintf(child_path, sizeof(child_path), "%s/child.flecs", dir);

    test_write_file(child_path, "BlockCommentEntity{}\n");
    test_write_file(parent_path, "include child.flecs /* load child */\n");

    test_assert(ecs_script_run_file(world, parent_path) == 0);
    test_assert(ecs_lookup(world, "BlockCommentEntity") != 0);

    test_unlink(child_path);
    test_unlink(parent_path);
    test_rmdir(dir);

    ecs_fini(world);
}

void Include_include_not_allowed_in_template(void) {
    install_test_abort();

    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScript);

    const char *dir = test_tmp_dir("in_template");
    char parent_path[256], child_path[256];
    snprintf(parent_path, sizeof(parent_path), "%s/parent.flecs", dir);
    snprintf(child_path, sizeof(child_path), "%s/child.flecs", dir);

    test_write_file(child_path, "TemplateChild{}\n");
    test_write_file(parent_path,
        "template MyTemplate {\n"
        "  include child.flecs\n"
        "}\n");

    test_assert(ecs_script_run_file(world, parent_path) != 0);

    test_unlink(child_path);
    test_unlink(parent_path);
    test_rmdir(dir);

    ecs_fini(world);
}

void Include_include_not_allowed_in_entity_scope(void) {
    install_test_abort();

    ecs_world_t *world = ecs_init();

    const char *dir = test_tmp_dir("in_entity");
    char parent_path[256], child_path[256];
    snprintf(parent_path, sizeof(parent_path), "%s/parent.flecs", dir);
    snprintf(child_path, sizeof(child_path), "%s/child.flecs", dir);

    test_write_file(child_path, "Child{}\n");
    test_write_file(parent_path,
        "Parent {\n"
        "  include child.flecs\n"
        "}\n");

    test_assert(ecs_script_run_file(world, parent_path) != 0);

    test_unlink(child_path);
    test_unlink(parent_path);
    test_rmdir(dir);

    ecs_fini(world);
}

void Include_include_not_allowed_in_with_scope(void) {
    install_test_abort();

    ecs_world_t *world = ecs_init();

    const char *dir = test_tmp_dir("in_with");
    char parent_path[256], child_path[256];
    snprintf(parent_path, sizeof(parent_path), "%s/parent.flecs", dir);
    snprintf(child_path, sizeof(child_path), "%s/child.flecs", dir);

    test_write_file(child_path, "Child{}\n");
    test_write_file(parent_path,
        "Tag {}\n"
        "with Tag {\n"
        "  include child.flecs\n"
        "}\n");

    test_assert(ecs_script_run_file(world, parent_path) != 0);

    test_unlink(child_path);
    test_unlink(parent_path);
    test_rmdir(dir);

    ecs_fini(world);
}

void Include_include_not_allowed_in_if_scope(void) {
    install_test_abort();

    ecs_world_t *world = ecs_init();

    const char *dir = test_tmp_dir("in_if");
    char parent_path[256], child_path[256];
    snprintf(parent_path, sizeof(parent_path), "%s/parent.flecs", dir);
    snprintf(child_path, sizeof(child_path), "%s/child.flecs", dir);

    test_write_file(child_path, "Child{}\n");
    test_write_file(parent_path,
        "if true {\n"
        "  include child.flecs\n"
        "}\n");

    test_assert(ecs_script_run_file(world, parent_path) != 0);

    test_unlink(child_path);
    test_unlink(parent_path);
    test_rmdir(dir);

    ecs_fini(world);
}

void Include_include_not_allowed_in_for_scope(void) {
    install_test_abort();

    ecs_world_t *world = ecs_init();

    const char *dir = test_tmp_dir("in_for");
    char parent_path[256], child_path[256];
    snprintf(parent_path, sizeof(parent_path), "%s/parent.flecs", dir);
    snprintf(child_path, sizeof(child_path), "%s/child.flecs", dir);

    test_write_file(child_path, "Child{}\n");
    test_write_file(parent_path,
        "for i in 0 .. 1 {\n"
        "  include child.flecs\n"
        "}\n");

    test_assert(ecs_script_run_file(world, parent_path) != 0);

    test_unlink(child_path);
    test_unlink(parent_path);
    test_rmdir(dir);

    ecs_fini(world);
}

void Include_include_auto_appends_extension(void) {
    ecs_world_t *world = ecs_init();

    const char *dir = test_tmp_dir("auto_ext");
    char parent_path[256], child_path[256];
    snprintf(parent_path, sizeof(parent_path), "%s/parent.flecs", dir);
    snprintf(child_path, sizeof(child_path), "%s/child.flecs", dir);

    test_write_file(child_path, "AutoExtEntity{}\n");
    test_write_file(parent_path, "include child\n");

    test_assert(ecs_script_run_file(world, parent_path) == 0);
    test_assert(ecs_lookup(world, "AutoExtEntity") != 0);

    test_unlink(child_path);
    test_unlink(parent_path);
    test_rmdir(dir);

    ecs_fini(world);
}

void Include_include_auto_appends_extension_subdir(void) {
    ecs_world_t *world = ecs_init();

    const char *dir = test_tmp_dir("auto_ext_subdir");
    char sub[256], parent_path[256], child_path[256];
    snprintf(sub, sizeof(sub), "%s/sub", dir);
    test_mkdir(sub);
    snprintf(parent_path, sizeof(parent_path), "%s/parent.flecs", dir);
    snprintf(child_path, sizeof(child_path), "%s/child.flecs", sub);

    test_write_file(child_path, "SubAutoExt{}\n");
    test_write_file(parent_path, "include sub/child\n");

    test_assert(ecs_script_run_file(world, parent_path) == 0);
    test_assert(ecs_lookup(world, "SubAutoExt") != 0);

    test_unlink(child_path);
    test_unlink(parent_path);
    test_rmdir(sub);
    test_rmdir(dir);

    ecs_fini(world);
}

void Include_include_keeps_explicit_extension(void) {
    ecs_world_t *world = ecs_init();

    const char *dir = test_tmp_dir("explicit_ext");
    char parent_path[256], child_path[256];
    snprintf(parent_path, sizeof(parent_path), "%s/parent.flecs", dir);
    snprintf(child_path, sizeof(child_path), "%s/child.flecs", dir);

    test_write_file(child_path, "ExplicitExtEntity{}\n");
    test_write_file(parent_path, "include child.flecs\n");

    test_assert(ecs_script_run_file(world, parent_path) == 0);
    test_assert(ecs_lookup(world, "ExplicitExtEntity") != 0);

    test_unlink(child_path);
    test_unlink(parent_path);
    test_rmdir(dir);

    ecs_fini(world);
}

void Include_include_auto_appends_extension_managed(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScript);

    const char *dir = test_tmp_dir("auto_ext_managed");
    char parent_path[256], child_path[256];
    snprintf(parent_path, sizeof(parent_path), "%s/parent.flecs", dir);
    snprintf(child_path, sizeof(child_path), "%s/child.flecs", dir);

    test_write_file(child_path, "ManagedAutoExt{}\n");
    test_write_file(parent_path, "include child\n");

    ecs_entity_t script = ecs_script(world, {
        .filename = parent_path
    });
    test_assert(script != 0);

    test_assert(ecs_lookup(world, "ManagedAutoExt") != 0);

    ecs_entity_t child_script = ecs_lookup_path_w_sep(
        world, 0, child_path, "/", NULL, false);
    test_assert(child_script != 0);
    test_assert(ecs_has(world, child_script, EcsScript));

    test_unlink(child_path);
    test_unlink(parent_path);
    test_rmdir(dir);

    ecs_fini(world);
}

void Include_fopen_override_remaps_filename(void) {
    setvbuf(stderr, NULL, _IONBF, 0);
    DIAG("test start");

    s_hook_call_count = 0;
    ecs_os_set_api_defaults();
    DIAG("after set_api_defaults, default fopen_=%p", (void*)ecs_os_api.fopen_);
    ecs_os_api_t api = ecs_os_api;
    api.fopen_ = test_fopen_remap;
    ecs_os_set_api(&api);
    DIAG("after set_api, fopen_=%p, test_fopen_remap=%p",
        (void*)ecs_os_api.fopen_, (void*)test_fopen_remap);

    ecs_world_t *world = ecs_init();
    DIAG("after ecs_init, fopen_=%p", (void*)ecs_os_api.fopen_);

    const char *dir = test_tmp_dir("fopen_remap");
    char requested[256], actual[256];
    snprintf(requested, sizeof(requested), "%s/requested.flecs", dir);
    snprintf(actual, sizeof(actual), "%s/actual.flecs", dir);
    DIAG("paths: req=%s act=%s", requested, actual);

    test_write_file(actual, "RemappedEntity{}\n");
    DIAG("wrote actual");

    g_fopen_remap_from = requested;
    g_fopen_remap_to = actual;
    DIAG("globals set, calling run_file");

    int run_rc = ecs_script_run_file(world, requested);
    DIAG("run_file returned %d, hook_calls=%d", run_rc, s_hook_call_count);

    ecs_entity_t found = ecs_lookup(world, "RemappedEntity");
    DIAG("lookup=%u", (unsigned)found);

    test_assert(run_rc == 0);
    test_assert(found != 0);

    g_fopen_remap_from = NULL;
    g_fopen_remap_to = NULL;

    test_unlink(actual);
    test_rmdir(dir);

    ecs_fini(world);
    DIAG("test end");
}
