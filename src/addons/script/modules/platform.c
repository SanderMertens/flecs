/**
 * @file addons/script/modules/platform.c
 * @brief Platform constants for flecs script.
 */

#include "flecs.h"

#ifdef FLECS_SCRIPT_PLATFORM
#include "../script.h"

#ifdef ECS_TARGET_EM
#include <emscripten.h>

EM_JS(int, flecs_script_platform_mobile, (void), {
    if (typeof navigator === "undefined") {
        return 0;
    }
    if (navigator.userAgentData && navigator.userAgentData.mobile) {
        return 1;
    }
    var ua = navigator.userAgent || "";
    if (/Android|iPhone|iPad|iPod|Mobile|Silk|Tablet/i.test(ua)) {
        return 1;
    }
    if (/Mac/.test(ua) && navigator.maxTouchPoints > 1) {
        return 1;
    }
    if (typeof window !== "undefined" && window.matchMedia &&
        window.matchMedia("(pointer: coarse)").matches &&
        navigator.maxTouchPoints > 0)
    {
        return 1;
    }
    return 0;
});

static bool flecs_script_platform_is_mobile(void) {
    return flecs_script_platform_mobile() != 0;
}
#else
static bool flecs_script_platform_is_mobile(void) {
    return false;
}
#endif

#if defined(ECS_TARGET_WINDOWS)
#define FLECS_SCRIPT_PLATFORM_OS "windows"
#elif defined(ECS_TARGET_ANDROID)
#define FLECS_SCRIPT_PLATFORM_OS "android"
#elif defined(ECS_TARGET_LINUX)
#define FLECS_SCRIPT_PLATFORM_OS "linux"
#elif defined(ECS_TARGET_FREEBSD)
#define FLECS_SCRIPT_PLATFORM_OS "freebsd"
#elif defined(ECS_TARGET_DARWIN)
#define FLECS_SCRIPT_PLATFORM_OS "darwin"
#elif defined(ECS_TARGET_EM)
#define FLECS_SCRIPT_PLATFORM_OS "emscripten"
#else
#define FLECS_SCRIPT_PLATFORM_OS "unknown"
#endif

#if defined(ECS_TARGET_MSVC)
#define FLECS_SCRIPT_PLATFORM_COMPILER "msvc"
#elif defined(ECS_TARGET_CLANG)
#define FLECS_SCRIPT_PLATFORM_COMPILER "clang"
#elif defined(ECS_TARGET_MINGW)
#define FLECS_SCRIPT_PLATFORM_COMPILER "mingw"
#elif defined(ECS_TARGET_GNU)
#define FLECS_SCRIPT_PLATFORM_COMPILER "gcc"
#else
#define FLECS_SCRIPT_PLATFORM_COMPILER "unknown"
#endif

#define FLECS_SCRIPT_PLATFORM_BOOL(const_name, has_value)\
    {\
        bool value = has_value;\
        ecs_const_var(world, {\
            .name = const_name,\
            .parent = ecs_id(FlecsScriptPlatform),\
            .type = ecs_id(ecs_bool_t),\
            .value = &value\
        });\
    }

#ifdef ECS_TARGET_WINDOWS
#define FLECS_SCRIPT_PLATFORM_HAS_WINDOWS true
#else
#define FLECS_SCRIPT_PLATFORM_HAS_WINDOWS false
#endif
#ifdef ECS_TARGET_POSIX
#define FLECS_SCRIPT_PLATFORM_HAS_POSIX true
#else
#define FLECS_SCRIPT_PLATFORM_HAS_POSIX false
#endif
#ifdef ECS_TARGET_ANDROID
#define FLECS_SCRIPT_PLATFORM_HAS_ANDROID true
#else
#define FLECS_SCRIPT_PLATFORM_HAS_ANDROID false
#endif
#ifdef ECS_TARGET_LINUX
#define FLECS_SCRIPT_PLATFORM_HAS_LINUX true
#else
#define FLECS_SCRIPT_PLATFORM_HAS_LINUX false
#endif
#ifdef ECS_TARGET_FREEBSD
#define FLECS_SCRIPT_PLATFORM_HAS_FREEBSD true
#else
#define FLECS_SCRIPT_PLATFORM_HAS_FREEBSD false
#endif
#ifdef ECS_TARGET_DARWIN
#define FLECS_SCRIPT_PLATFORM_HAS_DARWIN true
#else
#define FLECS_SCRIPT_PLATFORM_HAS_DARWIN false
#endif
#ifdef ECS_TARGET_EM
#define FLECS_SCRIPT_PLATFORM_HAS_EMSCRIPTEN true
#else
#define FLECS_SCRIPT_PLATFORM_HAS_EMSCRIPTEN false
#endif
#ifdef ECS_TARGET_MINGW
#define FLECS_SCRIPT_PLATFORM_HAS_MINGW true
#else
#define FLECS_SCRIPT_PLATFORM_HAS_MINGW false
#endif
#ifdef ECS_TARGET_GNU
#define FLECS_SCRIPT_PLATFORM_HAS_GNU true
#else
#define FLECS_SCRIPT_PLATFORM_HAS_GNU false
#endif

void FlecsScriptPlatformImport(
    ecs_world_t *world)
{
    ECS_MODULE(world, FlecsScriptPlatform);

    ECS_IMPORT(world, FlecsScript);

    FLECS_SCRIPT_PLATFORM_BOOL("WINDOWS", FLECS_SCRIPT_PLATFORM_HAS_WINDOWS)
    FLECS_SCRIPT_PLATFORM_BOOL("POSIX", FLECS_SCRIPT_PLATFORM_HAS_POSIX)
    FLECS_SCRIPT_PLATFORM_BOOL("ANDROID", FLECS_SCRIPT_PLATFORM_HAS_ANDROID)
    FLECS_SCRIPT_PLATFORM_BOOL("LINUX", FLECS_SCRIPT_PLATFORM_HAS_LINUX)
    FLECS_SCRIPT_PLATFORM_BOOL("FREEBSD", FLECS_SCRIPT_PLATFORM_HAS_FREEBSD)
    FLECS_SCRIPT_PLATFORM_BOOL("DARWIN", FLECS_SCRIPT_PLATFORM_HAS_DARWIN)
    FLECS_SCRIPT_PLATFORM_BOOL("EMSCRIPTEN", 
        FLECS_SCRIPT_PLATFORM_HAS_EMSCRIPTEN)
    FLECS_SCRIPT_PLATFORM_BOOL("MINGW", FLECS_SCRIPT_PLATFORM_HAS_MINGW)
    FLECS_SCRIPT_PLATFORM_BOOL("GNU", FLECS_SCRIPT_PLATFORM_HAS_GNU)

    char *compiler = ECS_CONST_CAST(char*, FLECS_SCRIPT_PLATFORM_COMPILER);
    ecs_const_var(world, {
        .name = "compiler",
        .parent = ecs_id(FlecsScriptPlatform),
        .type = ecs_id(ecs_string_t),
        .value = &compiler
    });

    char *os = ECS_CONST_CAST(char*, FLECS_SCRIPT_PLATFORM_OS);
    ecs_const_var(world, {
        .name = "os",
        .parent = ecs_id(FlecsScriptPlatform),
        .type = ecs_id(ecs_string_t),
        .value = &os
    });

    bool mobile = flecs_script_platform_is_mobile();
    ecs_const_var(world, {
        .name = "mobile",
        .parent = ecs_id(FlecsScriptPlatform),
        .type = ecs_id(ecs_bool_t),
        .value = &mobile
    });
}

#endif
