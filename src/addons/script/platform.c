/**
 * @file addons/script/platform.c
 * @brief Platform constants for flecs script.
 */

#include "flecs.h"

#ifdef FLECS_SCRIPT_PLATFORM
#include "script.h"

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

#define FLECS_SCRIPT_PLATFORM_BOOL(const_name)\
    {\
        bool const_name = FLECS_SCRIPT_PLATFORM_HAS_##const_name;\
        ecs_const_var(world, {\
            .name = #const_name,\
            .parent = ecs_id(FlecsScriptPlatform),\
            .type = ecs_id(ecs_bool_t),\
            .value = &const_name\
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

    FLECS_SCRIPT_PLATFORM_BOOL(WINDOWS)
    FLECS_SCRIPT_PLATFORM_BOOL(POSIX)
    FLECS_SCRIPT_PLATFORM_BOOL(ANDROID)
    FLECS_SCRIPT_PLATFORM_BOOL(LINUX)
    FLECS_SCRIPT_PLATFORM_BOOL(FREEBSD)
    FLECS_SCRIPT_PLATFORM_BOOL(DARWIN)
    FLECS_SCRIPT_PLATFORM_BOOL(EMSCRIPTEN)
    FLECS_SCRIPT_PLATFORM_BOOL(MINGW)
    FLECS_SCRIPT_PLATFORM_BOOL(GNU)

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
}

#endif
