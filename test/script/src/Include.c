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
