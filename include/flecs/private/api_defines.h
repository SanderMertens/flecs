/**
 * @file api_defines.h
 * @brief Supporting defines for the public API.
 *
 * This file contains constants / macro's that are typically not used by an
 * application but support the public API, and therefore must be exposed. This
 * header should not be included by itself.
 */

#ifndef FLECS_API_DEFINES_H
#define FLECS_API_DEFINES_H

#include "api_flags.h"

#if defined(_WIN32) || defined(_MSC_VER) || defined(__MING32__)
#define ECS_TARGET_WINDOWS
#elif defined(__ANDROID__)
#define ECS_TARGET_ANDROID
#define ECS_TARGET_POSIX
#elif defined(__linux__)
#define ECS_TARGET_LINUX
#define ECS_TARGET_POSIX
#elif defined(__FreeBSD__)
#define ECS_TARGET_FREEBSD
#define ECS_TARGET_POSIX
#elif defined(__APPLE__) && defined(__MACH__)
#define ECS_TARGET_DARWIN
#define ECS_TARGET_POSIX
#elif defined(__EMSCRIPTEN__)
#define ECS_TARGET_EM
#define ECS_TARGET_POSIX
#endif

#if defined(__MING32__)
#define ECS_TARGET_POSIX
#endif

#if defined(_MSC_VER)
#define ECS_TARGET_MSVC
#endif

#if defined(__GNUC__)
#define ECS_TARGET_GNU
#endif

/* Standard library dependencies */
#include <assert.h>
#include <stdarg.h>
#include <string.h>

/* Non-standard but required. If not provided by platform, add manually. */
#include <stdint.h>

/* Contains macro's for importing / exporting symbols */
#include "../bake_config.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __BAKE_LEGACY__
#define FLECS_LEGACY
#endif

/* Some symbols are only exported when building in debug build, to enable
 * whitebox testing of internal datastructures */
#ifndef FLECS_NDEBUG
#define FLECS_DBG_API FLECS_API
#else
#define FLECS_DBG_API
#endif


////////////////////////////////////////////////////////////////////////////////
//// Language support defines
////////////////////////////////////////////////////////////////////////////////

#ifndef FLECS_LEGACY
#include <stdbool.h>
#endif

#ifndef NULL
#define NULL ((void*)0)
#endif

/* The API uses the native bool type in C++, or a custom one in C */
#if !defined(__cplusplus) && !defined(__bool_true_false_are_defined)
#undef bool
#undef true
#undef false
typedef char bool;
#define false 0
#define true !false
#endif

/* Utility types to indicate usage as bitmask */
typedef uint8_t ecs_flags8_t;
typedef uint16_t ecs_flags16_t;
typedef uint32_t ecs_flags32_t;
typedef uint64_t ecs_flags64_t;

/* Keep unsigned integers out of the codebase as they do more harm than good */
typedef int32_t ecs_size_t;

#define ECS_SIZEOF(T) ECS_CAST(ecs_size_t, sizeof(T))

/* Use alignof in C++, or a trick in C. */
#ifdef __cplusplus
#define ECS_ALIGNOF(T) static_cast<int64_t>(alignof(T))
#elif defined(ECS_TARGET_MSVC)
#define ECS_ALIGNOF(T) (int64_t)__alignof(T)
#elif defined(ECS_TARGET_GNU)
#define ECS_ALIGNOF(T) (int64_t)__alignof__(T)
#else
#define ECS_ALIGNOF(T) ((int64_t)&((struct { char c; T d; } *)0)->d)
#endif

#if defined(ECS_TARGET_GNU)
#define ECS_UNUSED __attribute__((unused))
#else
#define ECS_UNUSED
#endif

#ifndef FLECS_NO_DEPRECATED_WARNINGS
#if defined(ECS_TARGET_GNU)
#define ECS_DEPRECATED(msg) __attribute__((deprecated(msg)))
#elif defined(ECS_TARGET_MSVC)
#define ECS_DEPRECATED(msg) __declspec(deprecated(msg))
#else
#define ECS_DEPRECATED(msg)
#endif
#else
#define ECS_DEPRECATED(msg)
#endif

#define ECS_ALIGN(size, alignment) (ecs_size_t)((((((size_t)size) - 1) / ((size_t)alignment)) + 1) * ((size_t)alignment))

/* Simple utility for determining the max of two values */
#define ECS_MAX(a, b) (((a) > (b)) ? a : b)
#define ECS_MIN(a, b) (((a) < (b)) ? a : b)

/* Abstraction on top of C-style casts so that C functions can be used in C++
 * code without producing warnings */
#ifndef __cplusplus
#define ECS_CAST(T, V) ((T)(V))
#else
#define ECS_CAST(T, V) (static_cast<T>(V))
#endif


////////////////////////////////////////////////////////////////////////////////
//// Entity id macro's
////////////////////////////////////////////////////////////////////////////////

#define ECS_ROW_MASK                  (0x0FFFFFFFu)
#define ECS_ROW_FLAGS_MASK            (~ECS_ROW_MASK)
#define ECS_RECORD_TO_ROW(v)          (ECS_CAST(int32_t, (ECS_CAST(uint32_t, v) & ECS_ROW_MASK)))
#define ECS_RECORD_TO_ROW_FLAGS(v)    (ECS_CAST(uint32_t, v) & ECS_ROW_FLAGS_MASK)
#define ECS_ROW_TO_RECORD(row, flags) (ECS_CAST(uint32_t, (ECS_CAST(uint32_t, row) | (flags))))

#define ECS_ROLE_MASK                 (0xFFull << 56)
#define ECS_ENTITY_MASK               (0xFFFFFFFFull)
#define ECS_GENERATION_MASK           (0xFFFFull << 32)
#define ECS_GENERATION(e)             ((e & ECS_GENERATION_MASK) >> 32)
#define ECS_GENERATION_INC(e)         ((e & ~ECS_GENERATION_MASK) | ((0xFFFF & (ECS_GENERATION(e) + 1)) << 32))
#define ECS_COMPONENT_MASK            (~ECS_ROLE_MASK)
#define ECS_HAS_ROLE(e, role)         ((e & ECS_ROLE_MASK) == ECS_##role)
#define ECS_PAIR_FIRST(e)             (ecs_entity_t_hi(e & ECS_COMPONENT_MASK))
#define ECS_PAIR_SECOND(e)            (ecs_entity_t_lo(e))
#define ECS_PAIR_RELATION             ECS_PAIR_FIRST
#define ECS_PAIR_OBJECT               ECS_PAIR_SECOND
#define ECS_HAS_RELATION(e, rel)      (ECS_HAS_ROLE(e, PAIR) && (ECS_PAIR_FIRST(e) == rel))

#define ECS_HAS_PAIR_OBJECT(e, rel, obj)\
    (ECS_HAS_RELATION(e, rel) && ECS_PAIR_SECOND(e) == obj)

#define ECS_HAS(id, has_id)(\
    (id == has_id) ||\
    (ECS_HAS_PAIR_OBJECT(id, ECS_PAIR_FIRST(has_id), ECS_PAIR_SECOND(has_id))))


////////////////////////////////////////////////////////////////////////////////
//// Convert between C typenames and variables
////////////////////////////////////////////////////////////////////////////////

/** Translate C type to id. */
#define ecs_id(T) FLECS__E##T

/** Translate C type to system function. */
#define ecs_iter_action(T) FLECS__F##T


////////////////////////////////////////////////////////////////////////////////
//// Utilities for working with pair identifiers
////////////////////////////////////////////////////////////////////////////////

#define ecs_entity_t_lo(value) ECS_CAST(uint32_t, value)
#define ecs_entity_t_hi(value) ECS_CAST(uint32_t, (value) >> 32)
#define ecs_entity_t_comb(lo, hi) ((ECS_CAST(uint64_t, hi) << 32) + ECS_CAST(uint32_t, lo))

#define ecs_pair(pred, obj) (ECS_PAIR | ecs_entity_t_comb(obj, pred))

/* Get object from pair with the correct (current) generation count */
#define ecs_pair_first(world, pair) ecs_get_alive(world, ECS_PAIR_FIRST(pair))
#define ecs_pair_second(world, pair) ecs_get_alive(world, ECS_PAIR_SECOND(pair))
#define ecs_pair_relation ecs_pair_first
#define ecs_pair_object ecs_pair_second

////////////////////////////////////////////////////////////////////////////////
//// Debug macro's
////////////////////////////////////////////////////////////////////////////////

#ifndef FLECS_NDEBUG
#define ECS_TABLE_LOCK(world, table) ecs_table_lock(world, table)
#define ECS_TABLE_UNLOCK(world, table) ecs_table_unlock(world, table)
#else
#define ECS_TABLE_LOCK(world, table)
#define ECS_TABLE_UNLOCK(world, table)
#endif


////////////////////////////////////////////////////////////////////////////////
//// Convenience macro's for ctor, dtor, move and copy
////////////////////////////////////////////////////////////////////////////////

#ifndef FLECS_LEGACY

/* Constructor/Destructor convenience macro */
#define ECS_XTOR_IMPL(type, postfix, var, ...)\
    void type##_##postfix(\
        void *_ptr,\
        int32_t _count,\
        const ecs_type_info_t *type_info)\
    {\
        (void)_ptr;\
        (void)_count;\
        (void)type_info;\
        for (int32_t i = 0; i < _count; i ++) {\
            type *var = &((type*)_ptr)[i];\
            (void)var;\
            __VA_ARGS__\
        }\
    }

/* Copy convenience macro */
#define ECS_COPY_IMPL(type, dst_var, src_var, ...)\
    void type##_##copy(\
        void *_dst_ptr,\
        const void *_src_ptr,\
        int32_t _count,\
        const ecs_type_info_t *type_info)\
    {\
        (void)_dst_ptr;\
        (void)_src_ptr;\
        (void)_count;\
        (void)type_info;\
        for (int32_t i = 0; i < _count; i ++) {\
            type *dst_var = &((type*)_dst_ptr)[i];\
            type *src_var = &((type*)_src_ptr)[i];\
            (void)dst_var;\
            (void)src_var;\
            __VA_ARGS__\
        }\
    }

/* Move convenience macro */
#define ECS_MOVE_IMPL(type, dst_var, src_var, ...)\
    void type##_##move(\
        void *_dst_ptr,\
        void *_src_ptr,\
        int32_t _count,\
        const ecs_type_info_t *type_info)\
    {\
        (void)_dst_ptr;\
        (void)_src_ptr;\
        (void)_count;\
        (void)type_info;\
        for (int32_t i = 0; i < _count; i ++) {\
            type *dst_var = &((type*)_dst_ptr)[i];\
            type *src_var = &((type*)_src_ptr)[i];\
            (void)dst_var;\
            (void)src_var;\
            __VA_ARGS__\
        }\
    }


#define ECS_ON_SET_IMPL(type, var, ...)\
    void type##_##on_set(ecs_iter_t *_it)\
    {\
        for (int32_t i = 0; i < _it->count; i ++) {\
            ecs_entity_t entity = _it->entities[i];\
            type *var = &((type*)_it->ptrs[0])[i];\
            (void)entity;\
            (void)var;\
            __VA_ARGS__\
        }\
    }

#endif


////////////////////////////////////////////////////////////////////////////////
//// Deprecated constants
////////////////////////////////////////////////////////////////////////////////

/* These constants should no longer be used, but are required by the core to
 * guarantee backwards compatibility */
#define ECS_AND (ECS_ROLE | (0x79ull << 56))
#define ECS_OR (ECS_ROLE | (0x78ull << 56))
#define ECS_XOR (ECS_ROLE | (0x77ull << 56))
#define ECS_NOT (ECS_ROLE | (0x76ull << 56))

#ifdef __cplusplus
}
#endif

#endif
