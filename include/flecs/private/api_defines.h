/**
 * @file api_defines.h
 * @brief Supporting defines for the public API.
 *
 * This file contains constants / macros that are typically not used by an
 * application but support the public API, and therefore must be exposed. This
 * header should not be included by itself.
 */

#ifndef FLECS_API_DEFINES_H
#define FLECS_API_DEFINES_H

#include "api_flags.h"

#if defined(_WIN32) || defined(_MSC_VER)
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

#if defined(__MINGW32__) || defined(__MINGW64__)
#define ECS_TARGET_MINGW
#endif

#if defined(_MSC_VER)
#ifndef __clang__
#define ECS_TARGET_MSVC
#endif
#endif

#if defined(__clang__)
#define ECS_TARGET_CLANG
#endif

#if defined(__GNUC__)
#define ECS_TARGET_GNU
#endif

/* Map between clang and apple clang versions, as version 13 has a difference in
 * the format of __PRETTY_FUNCTION__ which enum reflection depends on. */
#if defined(__clang__)
    #if defined(__APPLE__)
        #if __clang_major__ == 13
            #if __clang_minor__ < 1
                #define ECS_CLANG_VERSION 12
            #else
                #define ECS_CLANG_VERSION 13
            #endif
        #else
            #define ECS_CLANG_VERSION __clang_major__
        #endif
    #else
        #define ECS_CLANG_VERSION __clang_major__
    #endif
#endif

/* Ignored warnings */
#if defined(ECS_TARGET_CLANG)
/* Ignore unknown options so we don't have to care about the compiler version */
#pragma clang diagnostic ignored "-Wunknown-warning-option"
/* Warns for double or redundant semicolons. There are legitimate cases where a
 * semicolon after an empty statement is useful, for example after a macro that
 * is replaced with a code block. With this warning enabled, semicolons would 
 * only have to be added after macro's that are not code blocks, which in some
 * cases isn't possible as the implementation of a macro can be different in
 * debug/release mode. */
#pragma clang diagnostic ignored "-Wextra-semi-stmt"
/* This is valid in C99, and Flecs must be compiled as C99. */
#pragma clang diagnostic ignored "-Wdeclaration-after-statement"
/* Clang attribute to detect fallthrough isn't supported on older versions. 
 * Implicit fallthrough is still detected by gcc and ignored with "fall through"
 * comments */
#pragma clang diagnostic ignored "-Wimplicit-fallthrough"
/* This warning prevents adding a default case when all enum constants are part
 * of the switch. In C however an enum type can assume any value in the range of
 * the type, and this warning makes it harder to catch invalid enum values. */
#pragma clang diagnostic ignored "-Wcovered-switch-default"
/* This warning prevents some casts of function results to a different kind of
 * type, e.g. casting an int result to double. Not very useful in practice, as
 * it just forces the code to assign to a variable first, then cast. */
#pragma clang diagnostic ignored "-Wbad-function-cast"
/* Format strings can be passed down from other functions. */
#pragma clang diagnostic ignored "-Wformat-nonliteral"
/* Useful, but not reliable enough. It can incorrectly flag macro's as unused
 * in standalone builds. */
#pragma clang diagnostic ignored "-Wunused-macros"
#if __clang_major__ == 13
/* clang 13 can throw this warning for a define in ctype.h */
#pragma clang diagnostic ignored "-Wreserved-identifier"
#endif
/* Filenames aren't consistent across targets as they can use different casing 
 * (e.g. WinSock2 vs winsock2). */
#pragma clang diagnostic ignored "-Wnonportable-system-include-path"
/* Enum reflection relies on testing constant values that may not be valid for
 * the enumeration. */
#pragma clang diagnostic ignored "-Wenum-constexpr-conversion"
/* Very difficult to workaround this warning in C, especially for an ECS. */
#pragma clang diagnostic ignored "-Wunsafe-buffer-usage"
/* This warning gets thrown when trying to cast pointer returned from dlproc */
#pragma clang diagnostic ignored "-Wcast-function-type-strict"
/* This warning can get thrown for expressions that evaluate to constants
 * in debug/release mode. */
#pragma clang diagnostic ignored "-Wconstant-logical-operand"
/* With soft asserts enabled the code won't abort, which in some cases means
 * code paths are reached where values are uninitialized. */
#ifdef FLECS_SOFT_ASSERT
#pragma clang diagnostic ignored "-Wsometimes-uninitialized"
#endif

#elif defined(ECS_TARGET_GNU)
#ifndef __cplusplus
#pragma GCC diagnostic ignored "-Wdeclaration-after-statement"
#pragma GCC diagnostic ignored "-Wbad-function-cast"
#endif
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
#pragma GCC diagnostic ignored "-Wunused-macros"
/* This warning gets thrown *sometimes* when not all members for a struct are
 * provided in an initializer. Flecs heavily relies on descriptor structs that
 * only require partly initialization, so this warning isn't useful.
 * It doesn't introduce any safety issues (fields are guaranteed to be 0 
 * initialized), and later versions of gcc (>=11) seem to no longer throw this 
 * warning. */
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#endif

/* Standard library dependencies */
#include <assert.h>
#include <stdarg.h>
#include <string.h>

/* Non-standard but required. If not provided by platform, add manually. */
#include <stdint.h>

/* Contains macros for importing / exporting symbols */
#include "../bake_config.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __BAKE_LEGACY__
#define FLECS_LEGACY
#endif

/* Some symbols are only exported when building in debug build, to enable
 * white-box testing of internal data structures */
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

/* Bitmask type with compile-time defined size */
#define ecs_flagsn_t_(bits) ecs_flags##bits##_t
#define ecs_flagsn_t(bits) ecs_flagsn_t_(bits)

/* Bitset type that can store exactly as many bits as there are terms */
#define ecs_termset_t ecs_flagsn_t(FLECS_TERM_COUNT_MAX)

/* Utility macro's for setting/clearing termset bits */
#define ECS_TERMSET_SET(set, flag) ((set) |= (ecs_termset_t)(flag))
#define ECS_TERMSET_CLEAR(set, flag) ((set) &= (ecs_termset_t)~(flag))
#define ECS_TERMSET_COND(set, flag, cond) ((cond) \
    ? (ECS_TERMSET_SET(set, flag)) \
    : (ECS_TERMSET_CLEAR(set, flag)))

/* Keep unsigned integers out of the codebase as they do more harm than good */
typedef int32_t ecs_size_t;

/* Allocator type */
typedef struct ecs_allocator_t ecs_allocator_t;

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

/* Utility macro for doing const casts without warnings */
#ifndef __cplusplus
#define ECS_CONST_CAST(type, value) ((type)(uintptr_t)(value))
#else
#define ECS_CONST_CAST(type, value) (const_cast<type>(value))
#endif

/* Utility macro for doing pointer casts without warnings */
#ifndef __cplusplus
#define ECS_PTR_CAST(type, value) ((type)(uintptr_t)(value))
#else
#define ECS_PTR_CAST(type, value) (reinterpret_cast<type>(value))
#endif

/* Utility macro's to do bitwise comparisons between floats without warnings */
#define ECS_EQ(a, b) (ecs_os_memcmp(&(a), &(b), sizeof(a)) == 0)
#define ECS_NEQ(a, b) (!ECS_EQ(a, b))
#define ECS_EQZERO(a) ECS_EQ(a, (uint64_t){0})
#define ECS_NEQZERO(a) ECS_NEQ(a, (uint64_t){0})

/* Utilities to convert flecs version to string */
#define FLECS_VERSION_IMPLSTR(major, minor, patch) #major "." #minor "." #patch
#define FLECS_VERSION_IMPL(major, minor, patch) \
    FLECS_VERSION_IMPLSTR(major, minor, patch)

#define ECS_CONCAT(a, b) a ## b

////////////////////////////////////////////////////////////////////////////////
//// Magic numbers for sanity checking
////////////////////////////////////////////////////////////////////////////////

/* Magic number to identify the type of the object */
#define ecs_world_t_magic     (0x65637377)
#define ecs_stage_t_magic     (0x65637373)
#define ecs_query_t_magic     (0x65637375)
#define ecs_observer_t_magic  (0x65637362)


////////////////////////////////////////////////////////////////////////////////
//// Entity id macros
////////////////////////////////////////////////////////////////////////////////

#define ECS_ROW_MASK                  (0x0FFFFFFFu)
#define ECS_ROW_FLAGS_MASK            (~ECS_ROW_MASK)
#define ECS_RECORD_TO_ROW(v)          (ECS_CAST(int32_t, (ECS_CAST(uint32_t, v) & ECS_ROW_MASK)))
#define ECS_RECORD_TO_ROW_FLAGS(v)    (ECS_CAST(uint32_t, v) & ECS_ROW_FLAGS_MASK)
#define ECS_ROW_TO_RECORD(row, flags) (ECS_CAST(uint32_t, (ECS_CAST(uint32_t, row) | (flags))))

#define ECS_ID_FLAGS_MASK             (0xFFull << 60)
#define ECS_ENTITY_MASK               (0xFFFFFFFFull)
#define ECS_GENERATION_MASK           (0xFFFFull << 32)
#define ECS_GENERATION(e)             ((e & ECS_GENERATION_MASK) >> 32)
#define ECS_GENERATION_INC(e)         ((e & ~ECS_GENERATION_MASK) | ((0xFFFF & (ECS_GENERATION(e) + 1)) << 32))
#define ECS_COMPONENT_MASK            (~ECS_ID_FLAGS_MASK)
#define ECS_HAS_ID_FLAG(e, flag)      ((e) & ECS_##flag)
#define ECS_IS_PAIR(id)               (((id) & ECS_ID_FLAGS_MASK) == ECS_PAIR)
#define ECS_PAIR_FIRST(e)             (ecs_entity_t_hi(e & ECS_COMPONENT_MASK))
#define ECS_PAIR_SECOND(e)            (ecs_entity_t_lo(e))
#define ECS_HAS_RELATION(e, rel)      (ECS_HAS_ID_FLAG(e, PAIR) && (ECS_PAIR_FIRST(e) == rel))

#define ECS_TERM_REF_FLAGS(ref)       ((ref)->id & EcsTermRefFlags)
#define ECS_TERM_REF_ID(ref)          ((ref)->id & ~EcsTermRefFlags)

////////////////////////////////////////////////////////////////////////////////
//// Convert between C typenames and variables
////////////////////////////////////////////////////////////////////////////////

/** Translate C type to id. */
#define ecs_id(T) FLECS_ID##T##ID_


////////////////////////////////////////////////////////////////////////////////
//// Utilities for working with pair identifiers
////////////////////////////////////////////////////////////////////////////////

#define ecs_entity_t_lo(value) ECS_CAST(uint32_t, value)
#define ecs_entity_t_hi(value) ECS_CAST(uint32_t, (value) >> 32)
#define ecs_entity_t_comb(lo, hi) ((ECS_CAST(uint64_t, hi) << 32) + ECS_CAST(uint32_t, lo))

#define ecs_pair(pred, obj) (ECS_PAIR | ecs_entity_t_comb(obj, pred))
#define ecs_pair_t(pred, obj) (ECS_PAIR | ecs_entity_t_comb(obj, ecs_id(pred)))
#define ecs_pair_first(world, pair) ecs_get_alive(world, ECS_PAIR_FIRST(pair))
#define ecs_pair_second(world, pair) ecs_get_alive(world, ECS_PAIR_SECOND(pair))
#define ecs_pair_relation ecs_pair_first
#define ecs_pair_target ecs_pair_second

#define flecs_poly_id(tag) ecs_pair(ecs_id(EcsPoly), tag)


////////////////////////////////////////////////////////////////////////////////
//// Debug macros
////////////////////////////////////////////////////////////////////////////////

#ifndef FLECS_NDEBUG
#define ECS_TABLE_LOCK(world, table) ecs_table_lock(world, table)
#define ECS_TABLE_UNLOCK(world, table) ecs_table_unlock(world, table)
#else
#define ECS_TABLE_LOCK(world, table)
#define ECS_TABLE_UNLOCK(world, table)
#endif


////////////////////////////////////////////////////////////////////////////////
//// Actions that drive iteration
////////////////////////////////////////////////////////////////////////////////

#define EcsIterNextYield  (0)   /* Move to next table, yield current */
#define EcsIterYield      (-1)  /* Stay on current table, yield */
#define EcsIterNext  (1)   /* Move to next table, don't yield */

////////////////////////////////////////////////////////////////////////////////
//// Convenience macros for ctor, dtor, move and copy
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
            const type *src_var = &((const type*)_src_ptr)[i];\
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

#define ECS_HOOK_IMPL(type, func, var, ...)\
    void func(ecs_iter_t *_it)\
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

#ifdef __cplusplus
}
#endif

#endif
