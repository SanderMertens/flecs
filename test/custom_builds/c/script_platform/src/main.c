#include <script_platform.h>
#include <stdio.h>

static void check_bool(
    ecs_world_t *world,
    const char *expr,
    bool expected)
{
    bool v = !expected;
    assert(ecs_expr_run(world, expr, &ecs_value_ptr(ecs_bool_t, &v), NULL) != NULL);
    assert(v == expected);
    (void)v;
}

static void check_string(
    ecs_world_t *world,
    const char *expr,
    const char *expected)
{
    char *v = NULL;
    assert(ecs_expr_run(world, expr, &ecs_value_ptr(ecs_string_t, &v), NULL) != NULL);
    assert(v != NULL);
    assert(!ecs_os_strcmp(v, expected));
    ecs_os_free(v);
}

int main(int argc, char *argv[]) {
    ecs_world_t *world = ecs_init_w_args(argc, argv);

    ECS_IMPORT(world, FlecsScriptPlatform);

    check_bool(world, "flecs.script.platform.WINDOWS",
#ifdef ECS_TARGET_WINDOWS
        true
#else
        false
#endif
    );

    check_bool(world, "flecs.script.platform.POSIX",
#ifdef ECS_TARGET_POSIX
        true
#else
        false
#endif
    );

    check_bool(world, "flecs.script.platform.ANDROID",
#ifdef ECS_TARGET_ANDROID
        true
#else
        false
#endif
    );

    check_bool(world, "flecs.script.platform.LINUX",
#ifdef ECS_TARGET_LINUX
        true
#else
        false
#endif
    );

    check_bool(world, "flecs.script.platform.FREEBSD",
#ifdef ECS_TARGET_FREEBSD
        true
#else
        false
#endif
    );

    check_bool(world, "flecs.script.platform.DARWIN",
#ifdef ECS_TARGET_DARWIN
        true
#else
        false
#endif
    );

    check_bool(world, "flecs.script.platform.EMSCRIPTEN",
#ifdef ECS_TARGET_EM
        true
#else
        false
#endif
    );

    check_bool(world, "flecs.script.platform.MINGW",
#ifdef ECS_TARGET_MINGW
        true
#else
        false
#endif
    );

    check_bool(world, "flecs.script.platform.GNU",
#ifdef ECS_TARGET_GNU
        true
#else
        false
#endif
    );

    check_string(world, "flecs.script.platform.os",
#if defined(ECS_TARGET_WINDOWS)
        "windows"
#elif defined(ECS_TARGET_ANDROID)
        "android"
#elif defined(ECS_TARGET_LINUX)
        "linux"
#elif defined(ECS_TARGET_FREEBSD)
        "freebsd"
#elif defined(ECS_TARGET_DARWIN)
        "darwin"
#elif defined(ECS_TARGET_EM)
        "emscripten"
#else
        "unknown"
#endif
    );

    check_string(world, "flecs.script.platform.compiler",
#if defined(ECS_TARGET_MSVC)
        "msvc"
#elif defined(ECS_TARGET_CLANG)
        "clang"
#elif defined(ECS_TARGET_MINGW)
        "mingw"
#elif defined(ECS_TARGET_GNU)
        "gcc"
#else
        "unknown"
#endif
    );

    return ecs_fini(world);
}
