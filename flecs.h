// Comment out this line when using as DLL
#define flecs_STATIC
/**
 * @file flecs.h
 * @brief Flecs public API.
 *
 * This file contains the public API for Flecs.
 */

#ifndef FLECS_H
#define FLECS_H

/**
 * @defgroup options API toggles & constants
 * @{
 */

/* Customizable precision for floating point operations (such as time ops) */
#ifndef FLECS_FLOAT
#define FLECS_FLOAT float
#endif

/* FLECS_LEGACY should be defined when building for C89 */
// #define FLECS_LEGACY

/* FLECS_NO_DEPRECATED_WARNINGS disables deprecated warnings */
#define FLECS_NO_DEPRECATED_WARNINGS

/* Make sure provided configuration is valid */
#if defined(FLECS_DEBUG) && defined(FLECS_NDEBUG)
#error "invalid configuration: cannot both define FLECS_DEBUG and FLECS_NDEBUG"
#endif
#if defined(FLECS_DEBUG) && defined(NDEBUG)
#error "invalid configuration: cannot both define FLECS_DEBUG and NDEBUG"
#endif

/* Flecs debugging enables asserts, which are used for input parameter checking
 * and cheap (constant time) sanity checks. There are lots of asserts in every
 * part of the code, so this will slow down code. */
#if !defined(FLECS_DEBUG) && !defined(FLECS_NDEBUG) 
#if defined(NDEBUG)
#define FLECS_NDEBUG
#else
#define FLECS_DEBUG
#endif
#endif

/* FLECS_SANITIZE enables expensive checks that can detect issues early. This
 * will severely slow down code. */
// #define FLECS_SANITIZE
#ifdef FLECS_SANITIZE
#define FLECS_DEBUG /* If sanitized mode is enabled, so is debug mode */
#endif

/* Tip: if you see weird behavior that you think might be a bug, make sure to
 * test with the FLECS_DEBUG or FLECS_SANITIZE flags enabled. There's a good
 * chance that this gives you more information about the issue! */

/* FLECS_SOFT_ASSERT disables aborting for recoverable errors */
// #define FLECS_SOFT_ASSERT

/* FLECS_KEEP_ASSERT keeps asserts in release mode. */
// #define FLECS_KEEP_ASSERT

/* The following macro's let you customize with which addons Flecs is built.
 * Without any addons Flecs is just a minimal ECS storage, but addons add 
 * features such as systems, scheduling and reflection. If an addon is disabled,
 * it is excluded from the build, so that it consumes no resources. By default
 * all addons are enabled.
 * 
 * You can customize a build by either whitelisting or blacklisting addons. To
 * whitelist addons, first define the FLECS_CUSTOM_BUILD macro, which disables
 * all addons. You can then manually select the addons you need by defining
 * their macro, like "FLECS_SYSTEM".
 * 
 * To blacklist an addon, make sure to *not* define FLECS_CUSTOM_BUILD, and
 * instead define the addons you don't need by defining FLECS_NO_<addon>, for
 * example "FLECS_NO_SYSTEM". If there are any addons that depend on the 
 * blacklisted addon, an error will be thrown during the build.
 * 
 * Note that addons can have dependencies on each other. Addons will 
 * automatically enable their dependencies. To see the list of addons that was
 * compiled in a build, enable tracing before creating the world by doing:
 *   ecs_log_set_level(0);
 * which outputs the full list of addons Flecs was compiled with.
 */

/* Define if you want to create a custom build by whitelisting addons */
// #define FLECS_CUSTOM_BUILD

#ifndef FLECS_CUSTOM_BUILD
// #define FLECS_C          /* C API convenience macro's, always enabled */
#define FLECS_CPP           /* C++ API */
#define FLECS_MODULE        /* Module support */
#define FLECS_PARSER        /* String parser for queries */
#define FLECS_PLECS         /* ECS data definition format */
#define FLECS_RULES         /* Constraint solver for advanced queries */
#define FLECS_SNAPSHOT      /* Snapshot & restore ECS data */
#define FLECS_STATS         /* Keep track of runtime statistics */
#define FLECS_SYSTEM        /* System support */
#define FLECS_PIPELINE      /* Pipeline support */
#define FLECS_TIMER         /* Timer support */
#define FLECS_META          /* Reflection support */
#define FLECS_META_C        /* Utilities for populating reflection data */
#define FLECS_UNITS         /* Builtin standard units */
#define FLECS_EXPR          /* Parsing strings to/from component values */
#define FLECS_JSON          /* Parsing JSON to/from component values */
#define FLECS_DOC           /* Document entities & components */
#define FLECS_COREDOC       /* Documentation for core entities & components */
#define FLECS_LOG           /* When enabled ECS provides more detailed logs */
#define FLECS_APP           /* Application addon */
#define FLECS_OS_API_IMPL   /* Default implementation for OS API */
#define FLECS_HTTP          /* Tiny HTTP server for connecting to remote UI */
#define FLECS_REST          /* REST API for querying application data */
#endif // ifndef FLECS_CUSTOM_BUILD

/** @} */

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

/**
 * @file api_flags.h
 * @brief Bitset flags used by internals.
 */

#ifndef FLECS_API_FLAGS_H
#define FLECS_API_FLAGS_H

#ifdef __cplusplus
extern "C" {
#endif

////////////////////////////////////////////////////////////////////////////////
//// Entity flags (set in upper bits of ecs_record_t::row)
////////////////////////////////////////////////////////////////////////////////

#define EcsEntityObserved             (1u << 31)
#define EcsEntityObservedId           (1u << 30)
#define EcsEntityObservedObject       (1u << 29)
#define EcsEntityObservedAcyclic      (1u << 28)


////////////////////////////////////////////////////////////////////////////////
//// Id flags (used by ecs_id_record_t::flags)
////////////////////////////////////////////////////////////////////////////////

#define EcsIdOnDeleteRemove            (1u << 0)
#define EcsIdOnDeleteDelete            (1u << 1)
#define EcsIdOnDeletePanic             (1u << 2)
#define EcsIdOnDeleteMask\
    (EcsIdOnDeletePanic|EcsIdOnDeleteRemove|EcsIdOnDeleteDelete)

#define EcsIdOnDeleteObjectRemove      (1u << 3)
#define EcsIdOnDeleteObjectDelete      (1u << 4)
#define EcsIdOnDeleteObjectPanic       (1u << 5)
#define EcsIdOnDeleteObjectMask\
    (EcsIdOnDeleteObjectPanic|EcsIdOnDeleteObjectRemove|\
        EcsIdOnDeleteObjectDelete)

#define EcsIdExclusive                 (1u << 6)
#define EcsIdDontInherit               (1u << 7)
#define EcsIdAcyclic                   (1u << 8)
#define EcsIdTag                       (1u << 9)
#define EcsIdWith                      (1u << 10)

#define EcsIdHasOnAdd                  (1u << 15) /* Same values as table flags */
#define EcsIdHasOnRemove               (1u << 16) 
#define EcsIdHasOnSet                  (1u << 17)
#define EcsIdHasUnSet                  (1u << 18)
#define EcsIdEventMask\
    (EcsIdHasOnAdd|EcsIdHasOnRemove|EcsIdHasOnSet|EcsIdHasUnSet)

/* Utilities for converting from flags to delete policies and vice versa */
#define ECS_ID_ON_DELETE(flags) \
    ((ecs_entity_t[]){0, EcsRemove, EcsDelete, 0, EcsPanic}\
        [((flags) & EcsIdOnDeleteMask)])
#define ECS_ID_ON_DELETE_OBJECT(flags) ECS_ID_ON_DELETE(flags >> 3)
#define ECS_ID_ON_DELETE_FLAG(id) (1u << ((id) - EcsRemove))
#define ECS_ID_ON_DELETE_OBJECT_FLAG(id) (1u << (3 + ((id) - EcsRemove)))


////////////////////////////////////////////////////////////////////////////////
//// Iterator flags (used by ecs_iter_t::flags)
////////////////////////////////////////////////////////////////////////////////

#define EcsIterIsValid                 (1u << 0u)  /* Does iterator contain valid result */
#define EcsIterIsFilter                (1u << 1u)  /* Is iterator filter (metadata only) */
#define EcsIterIsInstanced             (1u << 2u)  /* Is iterator instanced */
#define EcsIterHasShared               (1u << 3u)  /* Does result have shared terms */
#define EcsIterTableOnly               (1u << 4u)  /* Result only populates table */
#define EcsIterEntityOptional          (1u << 5u)  /* Treat terms with entity subject as optional */
#define EcsIterNoResults               (1u << 6u)  /* Iterator has no results */
#define EcsIterIgnoreThis              (1u << 7u)  /* Only evaluate non-this terms */


////////////////////////////////////////////////////////////////////////////////
//// Filter flags (used by ecs_filter_t::flags)
////////////////////////////////////////////////////////////////////////////////

#define EcsFilterMatchThis             (1u << 1u)  /* Has terms that match This */
#define EcsFilterMatchOnlyThis         (1u << 2u)  /* Has only terms that match This */
#define EcsFilterMatchPrefab           (1u << 3u)  /* Does filter match prefabs */
#define EcsFilterMatchDisabled         (1u << 4u)  /* Does filter match disabled entities */
#define EcsFilterMatchEmptyTables      (1u << 5u)  /* Does filter return empty tables */
#define EcsFilterMatchAnything         (1u << 6u)  /* False if filter has no/only Not terms */
#define EcsFilterIsFilter              (1u << 7u)  /* When true, data fields won't be populated */
#define EcsFilterIsInstanced           (1u << 8u)  /* Is filter instanced (see ecs_filter_desc_t) */


////////////////////////////////////////////////////////////////////////////////
//// Table flags (used by ecs_table_t::flags)
////////////////////////////////////////////////////////////////////////////////

#define EcsTableHasBuiltins            (1u << 1u)  /* Does table have builtin components */
#define EcsTableIsPrefab               (1u << 2u)  /* Does the table store prefabs */
#define EcsTableHasIsA                 (1u << 3u)  /* Does the table have IsA relation */
#define EcsTableHasChildOf             (1u << 4u)  /* Does the table type ChildOf relation */
#define EcsTableHasPairs               (1u << 5u)  /* Does the table type have pairs */
#define EcsTableHasModule              (1u << 6u)  /* Does the table have module data */
#define EcsTableIsDisabled             (1u << 7u)  /* Does the table type has EcsDisabled */
#define EcsTableHasCtors               (1u << 8u)
#define EcsTableHasDtors               (1u << 9u)
#define EcsTableHasCopy                (1u << 10u)
#define EcsTableHasMove                (1u << 11u)
#define EcsTableHasSwitch              (1u << 12u)
#define EcsTableHasDisabled            (1u << 13u)
#define EcsTableHasOverrides           (1u << 14u)

#define EcsTableHasOnAdd               (1u << 15u) /* Same values as id flags */
#define EcsTableHasOnRemove            (1u << 16u)
#define EcsTableHasOnSet               (1u << 17u)
#define EcsTableHasUnSet               (1u << 18u)

/* Composite table flags */
#define EcsTableHasLifecycle        (EcsTableHasCtors | EcsTableHasDtors)
#define EcsTableIsComplex           (EcsTableHasLifecycle | EcsTableHasSwitch | EcsTableHasDisabled)
#define EcsTableHasAddActions       (EcsTableHasIsA | EcsTableHasSwitch | EcsTableHasCtors | EcsTableHasOnAdd | EcsTableHasOnSet)
#define EcsTableHasRemoveActions    (EcsTableHasIsA | EcsTableHasDtors | EcsTableHasOnRemove | EcsTableHasUnSet)


////////////////////////////////////////////////////////////////////////////////
//// Query flags (used by ecs_query_t::flags)
////////////////////////////////////////////////////////////////////////////////

#define EcsQueryHasRefs                (1u << 1u)  /* Does query have references */
#define EcsQueryIsSubquery             (1u << 2u)  /* Is query a subquery */
#define EcsQueryIsOrphaned             (1u << 3u)  /* Is subquery orphaned */
#define EcsQueryHasOutColumns          (1u << 4u)  /* Does query have out columns */
#define EcsQueryHasMonitor             (1u << 5u)  /* Does query track changes */


////////////////////////////////////////////////////////////////////////////////
//// Aperiodic action flags (used by ecs_run_aperiodic)
////////////////////////////////////////////////////////////////////////////////

#define EcsAperiodicEmptyTableEvents   (1u << 1u)  /* Process pending empty table events */
#define EcsAperiodicComponentMonitors  (1u << 2u)  /* Process component monitors */

#ifdef __cplusplus
}
#endif

#endif


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
/*
                                   )
                                  (.)
                                  .|.
                                  | |
                              _.--| |--._
                           .-';  ;`-'& ; `&.
                          \   &  ;    &   &_/
                           |"""---...---"""|
                           \ | | | | | | | /
                            `---.|.|.|.---'

 * This file is generated by bake.lang.c for your convenience. Headers of
 * dependencies will automatically show up in this file. Include bake_config.h
 * in your main project file. Do not edit! */

#ifndef FLECS_BAKE_CONFIG_H
#define FLECS_BAKE_CONFIG_H

/* Headers of public dependencies */
/* No dependencies */

/* Convenience macro for exporting symbols */
#ifndef flecs_STATIC
#if flecs_EXPORTS && (defined(_MSC_VER) || defined(__MINGW32__))
  #define FLECS_API __declspec(dllexport)
#elif flecs_EXPORTS
  #define FLECS_API __attribute__((__visibility__("default")))
#elif defined _MSC_VER
  #define FLECS_API __declspec(dllimport)
#else
  #define FLECS_API
#endif
#else
  #define FLECS_API
#endif

#endif



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
#define ecs_case(pred, obj) (ECS_CASE | ecs_entity_t_comb(obj, pred))

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


/**
 * @file vector.h
 * @brief Vector datastructure.
 *
 * This is an implementation of a simple vector type. The vector is allocated in
 * a single block of memory, with the element count, and allocated number of
 * elements encoded in the block. As this vector is used for user-types it has
 * been designed to support alignments higher than 8 bytes. This makes the size
 * of the vector header variable in size. To reduce the overhead associated with
 * retrieving or computing this size, the functions are wrapped in macro calls
 * that compute the header size at compile time.
 *
 * The API provides a number of _t macro's, which accept a size and alignment.
 * These macro's are used when no compile-time type is available.
 *
 * The vector guarantees contiguous access to its elements. When an element is
 * removed from the vector, the last element is copied to the removed element.
 *
 * The API requires passing in the type of the vector. This type is used to test
 * whether the size of the provided type equals the size of the type with which
 * the vector was created. In release mode this check is not performed.
 *
 * When elements are added to the vector, it will automatically resize to the
 * next power of two. This can change the pointer of the vector, which is why
 * operations that can increase the vector size, accept a double pointer to the
 * vector.
 */

#ifndef FLECS_VECTOR_H
#define FLECS_VECTOR_H


#ifdef __cplusplus
extern "C" {
#endif

#ifdef FLECS_NDEBUG
#define ECS_VECTOR_T_SIZE\
    (ECS_SIZEOF(int32_t) + ECS_SIZEOF(int32_t))
#else
#define ECS_VECTOR_T_SIZE\
    (ECS_SIZEOF(int32_t) + ECS_SIZEOF(int32_t) + ECS_SIZEOF(int64_t))
#endif

/* Compute the header size of the vector from size & alignment */
#define ECS_VECTOR_U(size, alignment) size, ECS_CAST(int16_t, ECS_MAX(ECS_VECTOR_T_SIZE, alignment))

/* Compute the header size of the vector from a provided compile-time type */
#define ECS_VECTOR_T(T) ECS_VECTOR_U(ECS_SIZEOF(T), ECS_ALIGNOF(T))

typedef struct ecs_vector_t ecs_vector_t;

typedef int (*ecs_comparator_t)(
    const void* p1,
    const void *p2);

/** Create new vector. */
FLECS_API
ecs_vector_t* _ecs_vector_new(
    ecs_size_t elem_size,
    int16_t offset,
    int32_t elem_count);

#define ecs_vector_new(T, elem_count) \
    _ecs_vector_new(ECS_VECTOR_T(T), elem_count)

#define ecs_vector_new_t(size, alignment, elem_count) \
    _ecs_vector_new(ECS_VECTOR_U(size, alignment), elem_count)    

/* Create new vector, initialize it with provided array */
FLECS_API
ecs_vector_t* _ecs_vector_from_array(
    ecs_size_t elem_size,
    int16_t offset,
    int32_t elem_count,
    void *array);

#define ecs_vector_from_array(T, elem_count, array)\
    _ecs_vector_from_array(ECS_VECTOR_T(T), elem_count, array)

/* Initialize vector with zero's */
FLECS_API
void _ecs_vector_zero(
    ecs_vector_t *vector,
    ecs_size_t elem_size,
    int16_t offset);

#define ecs_vector_zero(vector, T) \
    _ecs_vector_zero(vector, ECS_VECTOR_T(T))

/** Free vector */
FLECS_API
void ecs_vector_free(
    ecs_vector_t *vector);

/** Clear values in vector */
FLECS_API
void ecs_vector_clear(
    ecs_vector_t *vector);

/** Assert when the provided size does not match the vector type. */
FLECS_API
void ecs_vector_assert_size(
    ecs_vector_t* vector_inout,
    ecs_size_t elem_size);

/** Add element to vector. */
FLECS_API
void* _ecs_vector_add(
    ecs_vector_t **array_inout,
    ecs_size_t elem_size,
    int16_t offset);

#define ecs_vector_add(vector, T) \
    ((T*)_ecs_vector_add(vector, ECS_VECTOR_T(T)))

#define ecs_vector_add_t(vector, size, alignment) \
    _ecs_vector_add(vector, ECS_VECTOR_U(size, alignment))

/** Insert element to vector. */
FLECS_API
void* _ecs_vector_insert_at(
    ecs_vector_t **array_inout,
    ecs_size_t elem_size,
    int16_t offset,
    int32_t index);

#define ecs_vector_insert_at(vector, T, index) \
    ((T*)_ecs_vector_insert_at(vector, ECS_VECTOR_T(T), index))

#define ecs_vector_insert_at_t(vector, size, alignment, index) \
    _ecs_vector_insert_at(vector, ECS_VECTOR_U(size, alignment), index)

/** Add n elements to the vector. */
FLECS_API
void* _ecs_vector_addn(
    ecs_vector_t **array_inout,
    ecs_size_t elem_size,
    int16_t offset,
    int32_t elem_count);

#define ecs_vector_addn(vector, T, elem_count) \
    ((T*)_ecs_vector_addn(vector, ECS_VECTOR_T(T), elem_count))

#define ecs_vector_addn_t(vector, size, alignment, elem_count) \
    _ecs_vector_addn(vector, ECS_VECTOR_U(size, alignment), elem_count)

/** Get element from vector. */
FLECS_API
void* _ecs_vector_get(
    const ecs_vector_t *vector,
    ecs_size_t elem_size,
    int16_t offset,
    int32_t index);

#define ecs_vector_get(vector, T, index) \
    ((T*)_ecs_vector_get(vector, ECS_VECTOR_T(T), index))

#define ecs_vector_get_t(vector, size, alignment, index) \
    _ecs_vector_get(vector, ECS_VECTOR_U(size, alignment), index)

/** Get last element from vector. */
FLECS_API
void* _ecs_vector_last(
    const ecs_vector_t *vector,
    ecs_size_t elem_size,
    int16_t offset);

#define ecs_vector_last(vector, T) \
    (T*)_ecs_vector_last(vector, ECS_VECTOR_T(T))

#define ecs_vector_last_t(vector, size, alignment) \
    _ecs_vector_last(vector, ECS_VECTOR_U(size, alignment))

/** Set minimum size for vector. If the current size of the vector is larger, 
 * the function will have no side effects. */
FLECS_API
int32_t _ecs_vector_set_min_size(
    ecs_vector_t **array_inout,
    ecs_size_t elem_size,
    int16_t offset,
    int32_t elem_count);

#define ecs_vector_set_min_size(vector, T, size) \
    _ecs_vector_set_min_size(vector, ECS_VECTOR_T(T), size)

/** Set minimum count for vector. If the current count of the vector is larger, 
 * the function will have no side effects. */
FLECS_API
int32_t _ecs_vector_set_min_count(
    ecs_vector_t **vector_inout,
    ecs_size_t elem_size,
    int16_t offset,
    int32_t elem_count);

#define ecs_vector_set_min_count(vector, T, elem_count) \
    _ecs_vector_set_min_count(vector, ECS_VECTOR_T(T), elem_count)

#define ecs_vector_set_min_count_t(vector, size, alignment, elem_count) \
    _ecs_vector_set_min_count(vector, ECS_VECTOR_U(size, alignment), elem_count)

/** Remove last element. This operation requires no swapping of values. */
FLECS_API
void ecs_vector_remove_last(
    ecs_vector_t *vector);

/** Remove last value, store last element in provided value. */
FLECS_API
bool _ecs_vector_pop(
    ecs_vector_t *vector,
    ecs_size_t elem_size,
    int16_t offset,
    void *value);

#define ecs_vector_pop(vector, T, value) \
    _ecs_vector_pop(vector, ECS_VECTOR_T(T), value)

/** Append element at specified index to another vector. */
FLECS_API
int32_t _ecs_vector_move_index(
    ecs_vector_t **dst,
    ecs_vector_t *src,
    ecs_size_t elem_size,
    int16_t offset,
    int32_t index);

#define ecs_vector_move_index(dst, src, T, index) \
    _ecs_vector_move_index(dst, src, ECS_VECTOR_T(T), index)

#define ecs_vector_move_index_t(dst, src, size, alignment, index) \
    _ecs_vector_move_index(dst, src, ECS_VECTOR_U(size, alignment), index)

/** Remove element at specified index. Moves the last value to the index. */
FLECS_API
int32_t _ecs_vector_remove(
    ecs_vector_t *vector,
    ecs_size_t elem_size,
    int16_t offset,
    int32_t index);

#define ecs_vector_remove(vector, T, index) \
    _ecs_vector_remove(vector, ECS_VECTOR_T(T), index)

#define ecs_vector_remove_t(vector, size, alignment, index) \
    _ecs_vector_remove(vector, ECS_VECTOR_U(size, alignment), index)

/** Shrink vector to make the size match the count. */
FLECS_API
void _ecs_vector_reclaim(
    ecs_vector_t **vector,
    ecs_size_t elem_size,
    int16_t offset);

#define ecs_vector_reclaim(vector, T)\
    _ecs_vector_reclaim(vector, ECS_VECTOR_T(T))

#define ecs_vector_reclaim_t(vector, size, alignment)\
    _ecs_vector_reclaim(vector, ECS_VECTOR_U(size, alignment))

/** Grow size of vector with provided number of elements. */
FLECS_API
int32_t _ecs_vector_grow(
    ecs_vector_t **vector,
    ecs_size_t elem_size,
    int16_t offset,
    int32_t elem_count);

#define ecs_vector_grow(vector, T, size) \
    _ecs_vector_grow(vector, ECS_VECTOR_T(T), size)

/** Set allocation size of vector. */
FLECS_API
int32_t _ecs_vector_set_size(
    ecs_vector_t **vector,
    ecs_size_t elem_size,
    int16_t offset,
    int32_t elem_count);

#define ecs_vector_set_size(vector, T, elem_count) \
    _ecs_vector_set_size(vector, ECS_VECTOR_T(T), elem_count)

#define ecs_vector_set_size_t(vector, size, alignment, elem_count) \
    _ecs_vector_set_size(vector, ECS_VECTOR_U(size, alignment), elem_count)

/** Set count of vector. If the size of the vector is smaller than the provided
 * count, the vector is resized. */
FLECS_API
int32_t _ecs_vector_set_count(
    ecs_vector_t **vector,
    ecs_size_t elem_size,
    int16_t offset,
    int32_t elem_count);

#define ecs_vector_set_count(vector, T, elem_count) \
    _ecs_vector_set_count(vector, ECS_VECTOR_T(T), elem_count)

#define ecs_vector_set_count_t(vector, size, alignment, elem_count) \
    _ecs_vector_set_count(vector, ECS_VECTOR_U(size, alignment), elem_count)

/** Return number of elements in vector. */
FLECS_API
int32_t ecs_vector_count(
    const ecs_vector_t *vector);

/** Return size of vector. */
FLECS_API
int32_t ecs_vector_size(
    const ecs_vector_t *vector);

/** Return first element of vector. */
FLECS_API
void* _ecs_vector_first(
    const ecs_vector_t *vector,
    ecs_size_t elem_size,
    int16_t offset);

#define ecs_vector_first(vector, T) \
    ((T*)_ecs_vector_first(vector, ECS_VECTOR_T(T)))

#define ecs_vector_first_t(vector, size, alignment) \
    _ecs_vector_first(vector, ECS_VECTOR_U(size, alignment))

/** Sort elements in vector. */
FLECS_API
void _ecs_vector_sort(
    ecs_vector_t *vector,
    ecs_size_t elem_size,
    int16_t offset,
    ecs_comparator_t compare_action);

#define ecs_vector_sort(vector, T, compare_action) \
    _ecs_vector_sort(vector, ECS_VECTOR_T(T), compare_action)

/** Return memory occupied by vector. */
FLECS_API
void _ecs_vector_memory(
    const ecs_vector_t *vector,
    ecs_size_t elem_size,
    int16_t offset,
    int32_t *allocd,
    int32_t *used);

#define ecs_vector_memory(vector, T, allocd, used) \
    _ecs_vector_memory(vector, ECS_VECTOR_T(T), allocd, used)

#define ecs_vector_memory_t(vector, size, alignment, allocd, used) \
    _ecs_vector_memory(vector, ECS_VECTOR_U(size, alignment), allocd, used)

/** Copy vectors */
FLECS_API
ecs_vector_t* _ecs_vector_copy(
    const ecs_vector_t *src,
    ecs_size_t elem_size,
    int16_t offset);

#define ecs_vector_copy(src, T) \
    _ecs_vector_copy(src, ECS_VECTOR_T(T))

#define ecs_vector_copy_t(src, size, alignment) \
    _ecs_vector_copy(src, ECS_VECTOR_U(size, alignment))

#ifndef FLECS_LEGACY
#define ecs_vector_each(vector, T, var, ...)\
    {\
        int var##_i, var##_count = ecs_vector_count(vector);\
        T* var##_array = ecs_vector_first(vector, T);\
        for (var##_i = 0; var##_i < var##_count; var##_i ++) {\
            T* var = &var##_array[var##_i];\
            __VA_ARGS__\
        }\
    }
#endif
#ifdef __cplusplus
}
#endif


/** C++ wrapper for vector class. */
#ifdef __cplusplus
#ifndef FLECS_NO_CPP

namespace flecs {

template <typename T>
class vector_iterator
{
public:
    explicit vector_iterator(T* value, int index) {
        m_value = value;
        m_index = index;
    }

    bool operator!=(vector_iterator const& other) const
    {
        return m_index != other.m_index;
    }

    T const& operator*() const
    {
        return m_value[m_index];
    }

    vector_iterator& operator++()
    {
        ++m_index;
        return *this;
    }

private:
    T* m_value;
    int m_index;
};

/* C++ class mainly used as wrapper around internal ecs_vector_t. Do not use
 * this class as a replacement for STL datastructures! */
template <typename T>
class vector {
public:
    explicit vector(ecs_vector_t *v) : m_vector( v ) { }

    vector(size_t count = 0) : m_vector( nullptr ) { 
        if (count) {
            init(count);
        }
    }

    T& operator[](size_t index) {
        return *static_cast<T*>(_ecs_vector_get(m_vector, ECS_VECTOR_T(T), index));
    }

    vector_iterator<T> begin() {
        return vector_iterator<T>(
            static_cast<T*>(_ecs_vector_first(m_vector, ECS_VECTOR_T(T))), 0);
    }

    vector_iterator<T> end() {
        return vector_iterator<T>(
            static_cast<T*>(_ecs_vector_last(m_vector, ECS_VECTOR_T(T))),
                ecs_vector_count(m_vector));
    }    

    void clear() {
        ecs_vector_clear(m_vector);
    }

    void destruct() {
        ecs_vector_free(m_vector);
    }

    void add(T& value) {
        T* elem = static_cast<T*>(_ecs_vector_add(&m_vector, ECS_VECTOR_T(T)));
        *elem = value;
    }

    void add(T&& value) {
        T* elem = static_cast<T*>(_ecs_vector_add(&m_vector, ECS_VECTOR_T(T)))
        *elem = value;
    }

    T& get(int32_t index) {
        return *static_cast<T*>(_ecs_vector_get(m_vector, ECS_VECTOR_T(T), index));
    }

    T& first() {
        return *static_cast<T*>(_ecs_vector_first(m_vector, ECS_VECTOR_T(T)));
    }

    T& last() {
        return *static_cast<T*>(_ecs_vector_last(m_vector, ECS_VECTOR_T(T)));
    }

    int32_t count() {
        return ecs_vector_count(m_vector);
    }

    int32_t size() {
        return ecs_vector_size(m_vector);
    }

    ecs_vector_t *ptr() {
        return m_vector;
    }

    void ptr(ecs_vector_t *ptr) {
        m_vector = ptr;
    }

private:
    void init(size_t count) {
        m_vector = ecs_vector_new(T, static_cast<ecs_size_t>(count));
    }

    ecs_vector_t *m_vector;
};

}

#endif
#endif

#endif

/**
 * @file map.h
 * @brief Map datastructure.
 *
 * Key-value datastructure. The map allows for fast retrieval of a payload for
 * a 64-bit key. While it is not as fast as the sparse set, it is better at
 * handling randomly distributed values.
 *
 * Payload is stored in bucket arrays. A bucket is computed from an id by
 * using the (bucket_count - 1) as an AND-mask. The number of buckets is always
 * a power of 2. Multiple keys will be stored in the same bucket. As a result
 * the worst case retrieval performance of the map is O(n), though this is rare.
 * On average lookup performance should equal O(1).
 *
 * The datastructure will automatically grow the number of buckets when the
 * ratio between elements and buckets exceeds a certain threshold (LOAD_FACTOR).
 *
 * Note that while the implementation is a hashmap, it can only compute hashes
 * for the provided 64 bit keys. This means that the provided keys must always
 * be unique. If the provided keys are hashes themselves, it is the
 * responsibility of the user to ensure that collisions are handled.
 *
 * In debug mode the map verifies that the type provided to the map functions
 * matches the one used at creation time.
 */

#ifndef FLECS_MAP_H
#define FLECS_MAP_H


#ifdef __cplusplus
extern "C" {
#endif

typedef uint64_t ecs_map_key_t;

/* Map type */
typedef struct ecs_bucket_t {
    ecs_map_key_t *keys;    /* Array with keys */
    void *payload;          /* Payload array */
    int32_t count;          /* Number of elements in bucket */
} ecs_bucket_t;

typedef struct ecs_map_t {
    ecs_bucket_t *buckets;
    int16_t elem_size;
    uint8_t bucket_shift;
    int32_t bucket_count;
    int32_t count;
} ecs_map_t;

typedef struct ecs_map_iter_t {
    const ecs_map_t *map;
    struct ecs_bucket_t *bucket;
    int32_t bucket_index;
    int32_t element_index;
    void *payload;
} ecs_map_iter_t;

#define ECS_MAP_INIT(T) { .elem_size = ECS_SIZEOF(T) }

/** Initialize new map. */
FLECS_API
void _ecs_map_init(
    ecs_map_t *map,
    ecs_size_t elem_size,
    int32_t elem_count);

#define ecs_map_init(map, T, elem_count)\
    _ecs_map_init(map, sizeof(T), elem_count)

/** Deinitialize map. */
FLECS_API
void ecs_map_fini(
    ecs_map_t *map);

/** Create new map. */
FLECS_API
ecs_map_t* _ecs_map_new(
    ecs_size_t elem_size,
    int32_t elem_count);

#define ecs_map_new(T, elem_count)\
    _ecs_map_new(sizeof(T), elem_count)

/** Is map initialized */
bool ecs_map_is_initialized(
    const ecs_map_t *result);

/** Get element for key, returns NULL if they key doesn't exist. */
FLECS_API
void* _ecs_map_get(
    const ecs_map_t *map,
    ecs_size_t elem_size,
    ecs_map_key_t key);

#define ecs_map_get(map, T, key)\
    (T*)_ecs_map_get(map, sizeof(T), (ecs_map_key_t)key)

/** Get pointer element. This dereferences the map element as a pointer. This
 * operation returns NULL when either the element does not exist or whether the
 * pointer is NULL, and should therefore only be used when the application knows
 * for sure that a pointer should never be NULL. */
FLECS_API
void* _ecs_map_get_ptr(
    const ecs_map_t *map,
    ecs_map_key_t key);

#define ecs_map_get_ptr(map, T, key)\
    (T)_ecs_map_get_ptr(map, key)

/** Test if map has key */
FLECS_API
bool ecs_map_has(
    const ecs_map_t *map,
    ecs_map_key_t key);

/** Get or create element for key. */
FLECS_API
void* _ecs_map_ensure(
    ecs_map_t *map,
    ecs_size_t elem_size,
    ecs_map_key_t key);

#define ecs_map_ensure(map, T, key)\
    ((T*)_ecs_map_ensure(map, sizeof(T), (ecs_map_key_t)key))

/** Set element. */
FLECS_API
void* _ecs_map_set(
    ecs_map_t *map,
    ecs_size_t elem_size,
    ecs_map_key_t key,
    const void *payload);

#define ecs_map_set(map, key, payload)\
    _ecs_map_set(map, sizeof(*payload), (ecs_map_key_t)key, payload)

#define ecs_map_set_ptr(map, key, payload)\
    _ecs_map_set(map, sizeof(payload), (ecs_map_key_t)key, &payload)

/** Free map. */
FLECS_API
void ecs_map_free(
    ecs_map_t *map);

/** Remove key from map.
 * Returns number of remaining elements.
 */
FLECS_API
int32_t ecs_map_remove(
    ecs_map_t *map,
    ecs_map_key_t key);

/** Remove all elements from map. */
FLECS_API
void ecs_map_clear(
    ecs_map_t *map);

/** Return number of elements in map. */
FLECS_API
int32_t ecs_map_count(
    const ecs_map_t *map);

/** Return number of buckets in map. */
FLECS_API
int32_t ecs_map_bucket_count(
    const ecs_map_t *map);

/** Return iterator to map contents. */
FLECS_API
ecs_map_iter_t ecs_map_iter(
    const ecs_map_t *map);

/** Obtain next element in map from iterator. */
FLECS_API
void* _ecs_map_next(
    ecs_map_iter_t* iter,
    ecs_size_t elem_size,
    ecs_map_key_t *key);

#define ecs_map_next(iter, T, key) \
    (T*)_ecs_map_next(iter, sizeof(T), key)

/** Obtain next pointer element from iterator. See ecs_map_get_ptr. */
FLECS_API
void* _ecs_map_next_ptr(
    ecs_map_iter_t* iter,
    ecs_map_key_t *key);

#define ecs_map_next_ptr(iter, T, key) \
    (T)_ecs_map_next_ptr(iter, key)

/** Grow number of buckets in the map for specified number of elements. */
FLECS_API
void ecs_map_grow(
    ecs_map_t *map,
    int32_t elem_count);

/** Set number of buckets in the map for specified number of elements. */
FLECS_API
void ecs_map_set_size(
    ecs_map_t *map,
    int32_t elem_count);

/** Copy map. */
FLECS_API
ecs_map_t* ecs_map_copy(
    ecs_map_t *map);

/** Return memory occupied by map. */
FLECS_API
void ecs_map_memory(
    ecs_map_t *map,
    int32_t *allocd,
    int32_t *used);

#ifndef FLECS_LEGACY
#define ecs_map_each(map, T, key, var, ...)\
    {\
        ecs_map_iter_t it = ecs_map_iter(map);\
        ecs_map_key_t key;\
        T* var;\
        (void)key;\
        (void)var;\
        while ((var = ecs_map_next(&it, T, &key))) {\
            __VA_ARGS__\
        }\
    }
#endif

#ifdef __cplusplus
}
#endif

#endif

/**
 * @file strbuf.h
 * @brief Utility for constructing strings.
 *
 * A buffer builds up a list of elements which individually can be up to N bytes
 * large. While appending, data is added to these elements. More elements are
 * added on the fly when needed. When an application calls ecs_strbuf_get, all
 * elements are combined in one string and the element administration is freed.
 *
 * This approach prevents reallocs of large blocks of memory, and therefore
 * copying large blocks of memory when appending to a large buffer. A buffer
 * preallocates some memory for the element overhead so that for small strings
 * there is hardly any overhead, while for large strings the overhead is offset
 * by the reduced time spent on copying memory.
 */

#ifndef FLECS_STRBUF_H_
#define FLECS_STRBUF_H_


#ifdef __cplusplus
extern "C" {
#endif

#define ECS_STRBUF_INIT (ecs_strbuf_t){0}
#define ECS_STRBUF_ELEMENT_SIZE (511)
#define ECS_STRBUF_MAX_LIST_DEPTH (32)

typedef struct ecs_strbuf_element {
    bool buffer_embedded;
    int32_t pos;
    char *buf;
    struct ecs_strbuf_element *next;
} ecs_strbuf_element;

typedef struct ecs_strbuf_element_embedded {
    ecs_strbuf_element super;
    char buf[ECS_STRBUF_ELEMENT_SIZE + 1];
} ecs_strbuf_element_embedded;

typedef struct ecs_strbuf_element_str {
    ecs_strbuf_element super;
    char *alloc_str;
} ecs_strbuf_element_str;

typedef struct ecs_strbuf_list_elem {
    int32_t count;
    const char *separator;
} ecs_strbuf_list_elem;

typedef struct ecs_strbuf_t {
    /* When set by an application, append will write to this buffer */
    char *buf;

    /* The maximum number of characters that may be printed */
    int32_t max;

    /* Size of elements minus current element */
    int32_t size;

    /* The number of elements in use */
    int32_t elementCount;

    /* Always allocate at least one element */
    ecs_strbuf_element_embedded firstElement;

    /* The current element being appended to */
    ecs_strbuf_element *current;

    /* Stack that keeps track of number of list elements, used for conditionally
     * inserting a separator */
    ecs_strbuf_list_elem list_stack[ECS_STRBUF_MAX_LIST_DEPTH];
    int32_t list_sp;

    /* This is set to the output string after calling ecs_strbuf_get */
    char *content;

    /* This is set to the output string length after calling ecs_strbuf_get */
    int32_t length;
} ecs_strbuf_t;

/* Append format string to a buffer.
 * Returns false when max is reached, true when there is still space */
FLECS_API
bool ecs_strbuf_append(
    ecs_strbuf_t *buffer,
    const char *fmt,
    ...);

/* Append format string with argument list to a buffer.
 * Returns false when max is reached, true when there is still space */
FLECS_API
bool ecs_strbuf_vappend(
    ecs_strbuf_t *buffer,
    const char *fmt,
    va_list args);

/* Append string to buffer.
 * Returns false when max is reached, true when there is still space */
FLECS_API
bool ecs_strbuf_appendstr(
    ecs_strbuf_t *buffer,
    const char *str);

/* Append character to buffer.
 * Returns false when max is reached, true when there is still space */
FLECS_API
bool ecs_strbuf_appendch(
    ecs_strbuf_t *buffer,
    char ch);

/* Append float to buffer.
 * Returns false when max is reached, true when there is still space */
FLECS_API
bool ecs_strbuf_appendflt(
    ecs_strbuf_t *buffer,
    double v,
    char nan_delim);

/* Append source buffer to destination buffer.
 * Returns false when max is reached, true when there is still space */
FLECS_API
bool ecs_strbuf_mergebuff(
    ecs_strbuf_t *dst_buffer,
    ecs_strbuf_t *src_buffer);

/* Append string to buffer, transfer ownership to buffer.
 * Returns false when max is reached, true when there is still space */
FLECS_API
bool ecs_strbuf_appendstr_zerocpy(
    ecs_strbuf_t *buffer,
    char *str);

/* Append string to buffer, do not free/modify string.
 * Returns false when max is reached, true when there is still space */
FLECS_API
bool ecs_strbuf_appendstr_zerocpy_const(
    ecs_strbuf_t *buffer,
    const char *str);

/* Append n characters to buffer.
 * Returns false when max is reached, true when there is still space */
FLECS_API
bool ecs_strbuf_appendstrn(
    ecs_strbuf_t *buffer,
    const char *str,
    int32_t n);

/* Return result string */
FLECS_API
char *ecs_strbuf_get(
    ecs_strbuf_t *buffer);

/* Return small string from first element (appends \0) */
FLECS_API
char *ecs_strbuf_get_small(
    ecs_strbuf_t *buffer);

/* Reset buffer without returning a string */
FLECS_API
void ecs_strbuf_reset(
    ecs_strbuf_t *buffer);

/* Push a list */
FLECS_API
void ecs_strbuf_list_push(
    ecs_strbuf_t *buffer,
    const char *list_open,
    const char *separator);

/* Pop a new list */
FLECS_API
void ecs_strbuf_list_pop(
    ecs_strbuf_t *buffer,
    const char *list_close);

/* Insert a new element in list */
FLECS_API
void ecs_strbuf_list_next(
    ecs_strbuf_t *buffer);

/* Append formatted string as a new element in list */
FLECS_API
bool ecs_strbuf_list_append(
    ecs_strbuf_t *buffer,
    const char *fmt,
    ...);

/* Append string as a new element in list */
FLECS_API
bool ecs_strbuf_list_appendstr(
    ecs_strbuf_t *buffer,
    const char *str);

FLECS_API
int32_t ecs_strbuf_written(
    const ecs_strbuf_t *buffer);

#ifdef __cplusplus
}
#endif

#endif

/**
 * @file os_api.h
 * @brief Operating system abstraction API.
 *
 * This file contains the operating system abstraction API. The flecs core 
 * library avoids OS/runtime specific API calls as much as possible. Instead it
 * provides an interface that can be implemented by applications.
 *
 * Examples for how to implement this interface can be found in the 
 * examples/os_api folder.
 */

#ifndef FLECS_OS_API_H
#define FLECS_OS_API_H

#include <stdarg.h>
#include <errno.h>

#if defined(ECS_TARGET_WINDOWS)
#include <malloc.h>
#elif defined(ECS_TARGET_FREEBSD)
#include <stdlib.h>
#else
#include <alloca.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ecs_time_t {
    uint32_t sec;
    uint32_t nanosec;
} ecs_time_t;

/* Allocation counters (not thread safe) */
extern int64_t ecs_os_api_malloc_count;
extern int64_t ecs_os_api_realloc_count;
extern int64_t ecs_os_api_calloc_count;
extern int64_t ecs_os_api_free_count;

/* Use handle types that _at least_ can store pointers */
typedef uintptr_t ecs_os_thread_t;
typedef uintptr_t ecs_os_cond_t;
typedef uintptr_t ecs_os_mutex_t;
typedef uintptr_t ecs_os_dl_t;
typedef uintptr_t ecs_os_sock_t;

/* Generic function pointer type */
typedef void (*ecs_os_proc_t)(void);

/* OS API init */
typedef 
void (*ecs_os_api_init_t)(void);

/* OS API deinit */
typedef 
void (*ecs_os_api_fini_t)(void);

/* Memory management */
typedef 
void* (*ecs_os_api_malloc_t)(
    ecs_size_t size);

typedef 
void (*ecs_os_api_free_t)(
    void *ptr);

typedef
void* (*ecs_os_api_realloc_t)(
    void *ptr, 
    ecs_size_t size);

typedef
void* (*ecs_os_api_calloc_t)(
    ecs_size_t size);

typedef
char* (*ecs_os_api_strdup_t)(
    const char *str);

/* Threads */
typedef
void* (*ecs_os_thread_callback_t)(
    void*);

typedef
ecs_os_thread_t (*ecs_os_api_thread_new_t)(
    ecs_os_thread_callback_t callback,
    void *param);

typedef
void* (*ecs_os_api_thread_join_t)(
    ecs_os_thread_t thread);

/* Atomic increment / decrement */
typedef
int (*ecs_os_api_ainc_t)(
    int32_t *value);


/* Mutex */
typedef
ecs_os_mutex_t (*ecs_os_api_mutex_new_t)(
    void);

typedef
void (*ecs_os_api_mutex_lock_t)(
    ecs_os_mutex_t mutex);

typedef
void (*ecs_os_api_mutex_unlock_t)(
    ecs_os_mutex_t mutex);

typedef
void (*ecs_os_api_mutex_free_t)(
    ecs_os_mutex_t mutex);

/* Condition variable */
typedef
ecs_os_cond_t (*ecs_os_api_cond_new_t)(
    void);

typedef
void (*ecs_os_api_cond_free_t)(
    ecs_os_cond_t cond);

typedef
void (*ecs_os_api_cond_signal_t)(
    ecs_os_cond_t cond);

typedef
void (*ecs_os_api_cond_broadcast_t)(
    ecs_os_cond_t cond);

typedef
void (*ecs_os_api_cond_wait_t)(
    ecs_os_cond_t cond,
    ecs_os_mutex_t mutex);

typedef 
void (*ecs_os_api_sleep_t)(
    int32_t sec,
    int32_t nanosec);

typedef 
void (*ecs_os_api_enable_high_timer_resolution_t)(
    bool enable);

typedef
void (*ecs_os_api_get_time_t)(
    ecs_time_t *time_out);

typedef
uint64_t (*ecs_os_api_now_t)(void);

/* Logging */
typedef
void (*ecs_os_api_log_t)(
    int32_t level,     /* Logging level */
    const char *file,  /* File where message was logged */
    int32_t line,      /* Line it was logged */
    const char *msg);

/* Application termination */
typedef
void (*ecs_os_api_abort_t)(
    void);

/* Dynamic libraries */
typedef
ecs_os_dl_t (*ecs_os_api_dlopen_t)(
    const char *libname);

typedef
ecs_os_proc_t (*ecs_os_api_dlproc_t)(
    ecs_os_dl_t lib,
    const char *procname);

typedef
void (*ecs_os_api_dlclose_t)(
    ecs_os_dl_t lib);

typedef
char* (*ecs_os_api_module_to_path_t)(
    const char *module_id);

/* Prefix members of struct with 'ecs_' as some system headers may define 
 * macro's for functions like "strdup", "log" or "_free" */

typedef struct ecs_os_api_t {
    /* API init / deinit */
    ecs_os_api_init_t init_;
    ecs_os_api_fini_t fini_;

    /* Memory management */
    ecs_os_api_malloc_t malloc_;
    ecs_os_api_realloc_t realloc_;
    ecs_os_api_calloc_t calloc_;
    ecs_os_api_free_t free_;

    /* Strings */
    ecs_os_api_strdup_t strdup_;

    /* Threads */
    ecs_os_api_thread_new_t thread_new_;
    ecs_os_api_thread_join_t thread_join_;

    /* Atomic incremenet / decrement */
    ecs_os_api_ainc_t ainc_;
    ecs_os_api_ainc_t adec_;

    /* Mutex */
    ecs_os_api_mutex_new_t mutex_new_;
    ecs_os_api_mutex_free_t mutex_free_;
    ecs_os_api_mutex_lock_t mutex_lock_;
    ecs_os_api_mutex_lock_t mutex_unlock_;

    /* Condition variable */
    ecs_os_api_cond_new_t cond_new_;
    ecs_os_api_cond_free_t cond_free_;
    ecs_os_api_cond_signal_t cond_signal_;
    ecs_os_api_cond_broadcast_t cond_broadcast_;
    ecs_os_api_cond_wait_t cond_wait_;

    /* Time */
    ecs_os_api_sleep_t sleep_;
    ecs_os_api_now_t now_;
    ecs_os_api_get_time_t get_time_;
    ecs_os_api_enable_high_timer_resolution_t enable_high_timer_resolution_;

    /* Logging */
    ecs_os_api_log_t log_; /* Logging function. The level should be interpreted as: */
                           /* >0: Debug tracing. Only enabled in debug builds. */
                           /*  0: Tracing. Enabled in debug/release builds. */
                           /* -2: Warning. An issue occurred, but operation was successful. */
                           /* -3: Error. An issue occurred, and operation was unsuccessful. */
                           /* -4: Fatal. An issue occurred, and application must quit. */

    /* Application termination */
    ecs_os_api_abort_t abort_;

    /* Dynamic library loading */
    ecs_os_api_dlopen_t dlopen_;
    ecs_os_api_dlproc_t dlproc_;
    ecs_os_api_dlclose_t dlclose_;

    /* Overridable function that translates from a logical module id to a
     * shared library filename */
    ecs_os_api_module_to_path_t module_to_dl_;

    /* Overridable function that translates from a logical module id to a
     * path that contains module-specif resources or assets */
    ecs_os_api_module_to_path_t module_to_etc_;

    /* Trace level */
    int32_t log_level_;

    /* Trace indentation */
    int32_t log_indent_;

    /* Last error code */
    int32_t log_last_error_;

    /* Enable tracing with color */
    bool log_with_color_;
} ecs_os_api_t;

FLECS_API
extern ecs_os_api_t ecs_os_api;

FLECS_API
void ecs_os_init(void);

FLECS_API
void ecs_os_fini(void);

FLECS_API
void ecs_os_set_api(
    ecs_os_api_t *os_api);

FLECS_API
void ecs_os_set_api_defaults(void);

/* Memory management */
#ifndef ecs_os_malloc
#define ecs_os_malloc(size) ecs_os_api.malloc_(size)
#endif
#ifndef ecs_os_free
#define ecs_os_free(ptr) ecs_os_api.free_(ptr)
#endif
#ifndef ecs_os_realloc
#define ecs_os_realloc(ptr, size) ecs_os_api.realloc_(ptr, size)
#endif
#ifndef ecs_os_calloc
#define ecs_os_calloc(size) ecs_os_api.calloc_(size)
#endif
#if defined(ECS_TARGET_WINDOWS)
#define ecs_os_alloca(size) _alloca((size_t)(size))
#else
#define ecs_os_alloca(size) alloca((size_t)(size))
#endif

#define ecs_os_malloc_t(T) ECS_CAST(T*, ecs_os_malloc(ECS_SIZEOF(T)))
#define ecs_os_malloc_n(T, count) ECS_CAST(T*, ecs_os_malloc(ECS_SIZEOF(T) * (count)))
#define ecs_os_calloc_t(T) ECS_CAST(T*, ecs_os_calloc(ECS_SIZEOF(T)))
#define ecs_os_calloc_n(T, count) ECS_CAST(T*, ecs_os_calloc(ECS_SIZEOF(T) * (count)))

#define ecs_os_realloc_t(ptr, T) ECS_CAST(T*, ecs_os_realloc([ptr, ECS_SIZEOF(T)))
#define ecs_os_realloc_n(ptr, T, count) ECS_CAST(T*, ecs_os_realloc(ptr, ECS_SIZEOF(T) * (count)))
#define ecs_os_alloca_t(T) ECS_CAST(T*, ecs_os_alloca(ECS_SIZEOF(T)))
#define ecs_os_alloca_n(T, count) ECS_CAST(T*, ecs_os_alloca(ECS_SIZEOF(T) * (count)))

/* Strings */
#ifndef ecs_os_strdup
#define ecs_os_strdup(str) ecs_os_api.strdup_(str)
#endif

#define ecs_os_strset(dst, src) ecs_os_free(*dst); *dst = ecs_os_strdup(src)

#ifdef __cplusplus
#define ecs_os_strlen(str) static_cast<ecs_size_t>(strlen(str))
#define ecs_os_strncmp(str1, str2, num) strncmp(str1, str2, static_cast<size_t>(num))
#define ecs_os_memcmp(ptr1, ptr2, num) memcmp(ptr1, ptr2, static_cast<size_t>(num))
#define ecs_os_memcpy(ptr1, ptr2, num) memcpy(ptr1, ptr2, static_cast<size_t>(num))
#define ecs_os_memset(ptr, value, num) memset(ptr, value, static_cast<size_t>(num))
#define ecs_os_memmove(dst, src, size) memmove(dst, src, static_cast<size_t>(size))
#else
#define ecs_os_strlen(str) (ecs_size_t)strlen(str)
#define ecs_os_strncmp(str1, str2, num) strncmp(str1, str2, (size_t)(num))
#define ecs_os_memcmp(ptr1, ptr2, num) memcmp(ptr1, ptr2, (size_t)(num))
#define ecs_os_memcpy(ptr1, ptr2, num) memcpy(ptr1, ptr2, (size_t)(num))
#define ecs_os_memset(ptr, value, num) memset(ptr, value, (size_t)(num))
#define ecs_os_memmove(dst, src, size) memmove(dst, src, (size_t)(size))
#endif

#define ecs_os_memcpy_t(ptr1, ptr2, T) ecs_os_memcpy(ptr1, ptr2, ECS_SIZEOF(T))
#define ecs_os_memcpy_n(ptr1, ptr2, T, count) ecs_os_memcpy(ptr1, ptr2, ECS_SIZEOF(T) * count)
#define ecs_os_memcmp_t(ptr1, ptr2, T) ecs_os_memcmp(ptr1, ptr2, ECS_SIZEOF(T))

#define ecs_os_strcmp(str1, str2) strcmp(str1, str2)
#define ecs_os_memset_t(ptr, value, T) ecs_os_memset(ptr, value, ECS_SIZEOF(T))
#define ecs_os_memset_n(ptr, value, T, count) ecs_os_memset(ptr, value, ECS_SIZEOF(T) * count)
#define ecs_os_zeromem(ptr) ecs_os_memset(ptr, 0, ECS_SIZEOF(*ptr))

#define ecs_os_memdup_t(ptr, T) ecs_os_memdup(ptr, ECS_SIZEOF(T))
#define ecs_os_memdup_n(ptr, T, count) ecs_os_memdup(ptr, ECS_SIZEOF(T) * count)

#define ecs_offset(ptr, T, index)\
    ECS_CAST(T*, ECS_OFFSET(ptr, ECS_SIZEOF(T) * index))

#if defined(ECS_TARGET_MSVC)
#define ecs_os_strcat(str1, str2) strcat_s(str1, INT_MAX, str2)
#define ecs_os_sprintf(ptr, ...) sprintf_s(ptr, INT_MAX, __VA_ARGS__)
#define ecs_os_vsprintf(ptr, fmt, args) vsprintf_s(ptr, INT_MAX, fmt, args)
#define ecs_os_strcpy(str1, str2) strcpy_s(str1, INT_MAX, str2)
#ifdef __cplusplus
#define ecs_os_strncpy(str1, str2, num) strncpy_s(str1, INT_MAX, str2, static_cast<size_t>(num))
#else
#define ecs_os_strncpy(str1, str2, num) strncpy_s(str1, INT_MAX, str2, (size_t)(num))
#endif
#else
#define ecs_os_strcat(str1, str2) strcat(str1, str2)
#define ecs_os_sprintf(ptr, ...) sprintf(ptr, __VA_ARGS__)
#define ecs_os_vsprintf(ptr, fmt, args) vsprintf(ptr, fmt, args)
#define ecs_os_strcpy(str1, str2) strcpy(str1, str2)
#ifdef __cplusplus
#define ecs_os_strncpy(str1, str2, num) strncpy(str1, str2, static_cast<size_t>(num))
#else
#define ecs_os_strncpy(str1, str2, num) strncpy(str1, str2, (size_t)(num))
#endif
#endif

/* Files */
#if defined(ECS_TARGET_MSVC)
#define ecs_os_fopen(result, file, mode) fopen_s(result, file, mode)
#else
#define ecs_os_fopen(result, file, mode) (*(result)) = fopen(file, mode)
#endif

/* Threads */
#define ecs_os_thread_new(callback, param) ecs_os_api.thread_new_(callback, param)
#define ecs_os_thread_join(thread) ecs_os_api.thread_join_(thread)

/* Atomic increment / decrement */
#define ecs_os_ainc(value) ecs_os_api.ainc_(value)
#define ecs_os_adec(value) ecs_os_api.adec_(value)

/* Mutex */
#define ecs_os_mutex_new() ecs_os_api.mutex_new_()
#define ecs_os_mutex_free(mutex) ecs_os_api.mutex_free_(mutex)
#define ecs_os_mutex_lock(mutex) ecs_os_api.mutex_lock_(mutex)
#define ecs_os_mutex_unlock(mutex) ecs_os_api.mutex_unlock_(mutex)

/* Condition variable */
#define ecs_os_cond_new() ecs_os_api.cond_new_()
#define ecs_os_cond_free(cond) ecs_os_api.cond_free_(cond)
#define ecs_os_cond_signal(cond) ecs_os_api.cond_signal_(cond)
#define ecs_os_cond_broadcast(cond) ecs_os_api.cond_broadcast_(cond)
#define ecs_os_cond_wait(cond, mutex) ecs_os_api.cond_wait_(cond, mutex)

/* Time */
#define ecs_os_sleep(sec, nanosec) ecs_os_api.sleep_(sec, nanosec)
#define ecs_os_now() ecs_os_api.now_()
#define ecs_os_get_time(time_out) ecs_os_api.get_time_(time_out)

FLECS_API
void ecs_os_enable_high_timer_resolution(bool enable);

/* Logging */
FLECS_API
void ecs_os_dbg(const char *file, int32_t line, const char *msg);

FLECS_API
void ecs_os_trace(const char *file, int32_t line, const char *msg);

FLECS_API
void ecs_os_warn(const char *file, int32_t line, const char *msg);

FLECS_API
void ecs_os_err(const char *file, int32_t line, const char *msg);

FLECS_API
void ecs_os_fatal(const char *file, int32_t line, const char *msg);

FLECS_API
const char* ecs_os_strerror(int err);

/* Application termination */
#define ecs_os_abort() ecs_os_api.abort_()

/* Dynamic libraries */
#define ecs_os_dlopen(libname) ecs_os_api.dlopen_(libname)
#define ecs_os_dlproc(lib, procname) ecs_os_api.dlproc_(lib, procname)
#define ecs_os_dlclose(lib) ecs_os_api.dlclose_(lib)

/* Module id translation */
#define ecs_os_module_to_dl(lib) ecs_os_api.module_to_dl_(lib)
#define ecs_os_module_to_etc(lib) ecs_os_api.module_to_etc_(lib)

/* Sleep with floating point time */
FLECS_API
void ecs_sleepf(
    double t);

/* Measure time since provided timestamp */
FLECS_API
double ecs_time_measure(
    ecs_time_t *start);

/* Calculate difference between two timestamps */
FLECS_API
ecs_time_t ecs_time_sub(
    ecs_time_t t1,
    ecs_time_t t2);

/* Convert time value to a double */
FLECS_API
double ecs_time_to_double(
    ecs_time_t t);

FLECS_API
void* ecs_os_memdup(
    const void *src, 
    ecs_size_t size);

/** Are heap functions available? */
FLECS_API
bool ecs_os_has_heap(void);

/** Are threading functions available? */
FLECS_API
bool ecs_os_has_threading(void);

/** Are time functions available? */
FLECS_API
bool ecs_os_has_time(void);

/** Are logging functions available? */
FLECS_API
bool ecs_os_has_logging(void);

/** Are dynamic library functions available? */
FLECS_API
bool ecs_os_has_dl(void);

/** Are module path functions available? */
FLECS_API
bool ecs_os_has_modules(void);

#ifdef __cplusplus
}
#endif

#endif


#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup api_types Basic API types
 * @{
 */

/** A poly object.
 * A poly (short for polymorph) object is an object that has a variable list of
 * capabilities, determined by a mixin table. This is the current list of types
 * in the flecs API that can be used as an ecs_poly_t:
 * 
 * - ecs_world_t
 * - ecs_stage_t
 * - ecs_query_t
 * - ecs_filter_t
 * - ecs_rule_t
 * - (more to come)
 * 
 * Functions that accept an ecs_poly_t argument can accept objects of these
 * types. If the object does not have the requested mixin the API will throw an
 * assert.
 * 
 * The poly/mixin framework enables partially overlapping features to be
 * implemented once, and enables objects of different types to interact with
 * each other depending on what mixins they have, rather than their type
 * (in some ways it's like a mini-ECS). Additionally, each poly object has a
 * header that enables the API to do sanity checking on the input arguments.
 */
typedef void ecs_poly_t;

/** An id. Ids are the things that can be added to an entity. An id can be an
 * entity or pair, and can have an optional role. */
typedef uint64_t ecs_id_t;

/** An entity identifier. */
typedef ecs_id_t ecs_entity_t;

/** An array with (component) ids */
typedef struct {
    ecs_id_t *array;
    int32_t count;
} ecs_type_t;

/** A world is the container for all ECS data and supporting features. */
typedef struct ecs_world_t ecs_world_t;

/** A table is where entities and components are stored */
typedef struct ecs_table_t ecs_table_t;

/** A term is a single element in a query */
typedef struct ecs_term_t ecs_term_t;

/** A query allows for cached iteration over ECS data */
typedef struct ecs_query_t ecs_query_t;

/** A filter allows for uncached, ad hoc iteration over ECS data */
typedef struct ecs_filter_t ecs_filter_t;

/** A rule implements a non-trivial filter */
typedef struct ecs_rule_t ecs_rule_t;

/** A trigger reacts to events matching a single filter term */
typedef struct ecs_trigger_t ecs_trigger_t;

/** An observer reacts to events matching multiple filter terms */
typedef struct ecs_observer_t ecs_observer_t;

/** An observable contains lists of triggers for specific events/components */
typedef struct ecs_observable_t ecs_observable_t;

/* An iterator lets an application iterate entities across tables. */
typedef struct ecs_iter_t ecs_iter_t;

/** Refs cache data that lets them access components faster than ecs_get. */
typedef struct ecs_ref_t ecs_ref_t;

/** Type information */
typedef struct ecs_type_info_t ecs_type_info_t;

/* Mixins */
typedef struct ecs_mixins_t ecs_mixins_t;

/** @} */


/**
 * @defgroup constants API constants
 * @{
 */

/* Maximum number of components to add/remove in a single operation */
#define ECS_ID_CACHE_SIZE (32)

/* Maximum number of terms cached in static arrays */
#define ECS_TERM_CACHE_SIZE (4)

/* Maximum number of terms in desc (larger, as these are temp objects) */
#define ECS_TERM_DESC_CACHE_SIZE (16)

/* Maximum number of events to set in static array of trigger descriptor */
#define ECS_TRIGGER_DESC_EVENT_COUNT_MAX (8)

/* Maximum number of query variables per query */
#define ECS_VARIABLE_COUNT_MAX (64)

/* Number of query variables in iterator cache */
#define ECS_VARIABLE_CACHE_SIZE (4)

/** @} */


/**
 * @defgroup function_types Function Prototypes
 * @{
 */

/** Function prototype for runnables (systems, observers).
 * The run callback overrides the default behavior for iterating through the
 * results of a runnable object.
 * 
 * The default runnable iterates the iterator, and calls an iter_action (see
 * below) for each returned result.
 * 
 * @param it The iterator to be iterated by the runnable.
 */
typedef void (*ecs_run_action_t)(
    ecs_iter_t *it);

/** Function prototype for iterables.
 * A system may invoke a callback multiple times, typically once for each
 * matched table.
 * 
 * @param it The iterator containing the data for the current match.
 */
typedef void (*ecs_iter_action_t)(
    ecs_iter_t *it);

/** Function prototype for creating an iterator from a poly.
 * Used to create iterators from poly objects with the iterable mixin. When a
 * filter is provided, an array of two iterators must be passed to the function.
 * This allows the mixin implementation to create a chained iterator when
 * necessary, which requires two iterator objects.
 * 
 * @param world The world or stage for which to create the iterator.
 * @param iterable An iterable poly object.
 * @param it The iterator to create (out parameter)
 * @param filter Optional term to filter results.
 */
typedef void (*ecs_iter_init_action_t)(
    const ecs_world_t *world,
    const ecs_poly_t *iterable,
    ecs_iter_t *it,
    ecs_term_t *filter);

/** Function prototype for iterating an iterator.
 * Stored inside initialized iterators. This allows an application to * iterate 
 * an iterator without needing to know what created it.
 * 
 * @param it The iterator to iterate.
 * @return True if iterator has no more results, false if it does.
 */
typedef bool (*ecs_iter_next_action_t)(
    ecs_iter_t *it);  

/** Function prototype for freeing an iterator.
 * Free iterator resources.
 * 
 * @param it The iterator to free.
 */
typedef void (*ecs_iter_fini_action_t)(
    ecs_iter_t *it); 

/** Callback used for sorting components */
typedef int (*ecs_order_by_action_t)(
    ecs_entity_t e1,
    const void *ptr1,
    ecs_entity_t e2,
    const void *ptr2);

/** Callback used for ranking types */
typedef uint64_t (*ecs_group_by_action_t)(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_id_t id,
    void *ctx);

/** Initialization action for modules */
typedef void (*ecs_module_action_t)(
    ecs_world_t *world);    

/** Action callback on world exit */
typedef void (*ecs_fini_action_t)(
    ecs_world_t *world,
    void *ctx);

/** Function to cleanup context data */
typedef void (*ecs_ctx_free_t)(
    void *ctx);

/** Callback used for sorting values */
typedef int (*ecs_compare_action_t)(
    const void *ptr1,
    const void *ptr2);

/** Callback used for hashing values */
typedef uint64_t (*ecs_hash_value_action_t)(
    const void *ptr); 

/** Constructor/destructor callback */
typedef void (*ecs_xtor_t)(
    void *ptr,
    int32_t count,
    const ecs_type_info_t *type_info);

/** Copy is invoked when a component is copied into another component. */
typedef void (*ecs_copy_t)(
    void *dst_ptr,
    const void *src_ptr,
    int32_t count,
    const ecs_type_info_t *type_info);

/** Move is invoked when a component is moved to another component. */
typedef void (*ecs_move_t)(
    void *dst_ptr,
    void *src_ptr,
    int32_t count,
    const ecs_type_info_t *type_info);

/** @} */

/**
 * @defgroup mixin Public mixin types.
 * @{
 */

/** Header for ecs_poly_t objects. */
typedef struct ecs_header_t {
    int32_t magic; /* Magic number verifying it's a flecs object */
    int32_t type;  /* Magic number indicating which type of flecs object */
    ecs_mixins_t *mixins; /* Table with offsets to (optional) mixins */
} ecs_header_t;

/** Iterable mixin.
 * Allows its container to be iterated. */
typedef struct ecs_iterable_t {
    ecs_iter_init_action_t init; /* Callback that creates iterator. */
} ecs_iterable_t;

/** @} */

/**
 * @defgroup filter_types Types used to describe filters, terms and triggers
 * @{
 */

/** Set flags describe if & how a matched entity should be substituted */
#define EcsDefaultSet   (0)  /* Default set, SuperSet|Self for This subject */
#define EcsSelf         (1)  /* Select self (inclusive) */
#define EcsSuperSet     (2)  /* Select superset until predicate match */
#define EcsSubSet       (4)  /* Select subset until predicate match */
#define EcsCascade      (8)  /* Use breadth-first ordering of relations */
#define EcsAll          (16) /* Walk full super/subset, regardless of match */
#define EcsParent       (32) /* Shortcut for SuperSet(ChildOf) */
#define EcsNothing      (64) /* Select from nothing */

/** Specify read/write access for term */
typedef enum ecs_inout_kind_t {
    EcsInOutDefault,  /* InOut for regular terms, In for shared terms */
    EcsInOutFilter,   /* Term is neither read nor written */
    EcsInOut,         /* Term is both read and written */
    EcsIn,            /* Term is only read */
    EcsOut,           /* Term is only written */
} ecs_inout_kind_t;

/** Specifies whether term identifier is a variable or entity */
typedef enum ecs_var_kind_t {
    EcsVarDefault,      /* Variable if name is all caps, otherwise an entity */
    EcsVarIsEntity,     /* Term is an entity */
    EcsVarIsVariable    /* Term is a variable */
} ecs_var_kind_t;

/** Type describing an operator used in an signature of a system signature */
typedef enum ecs_oper_kind_t {
    EcsAnd,         /* The term must match */
    EcsOr,          /* One of the terms in an or chain must match */
    EcsNot,         /* The term must not match */
    EcsOptional,    /* The term may match */
    EcsAndFrom,     /* Term must match all components from term id */
    EcsOrFrom,      /* Term must match at least one component from term id */
    EcsNotFrom      /* Term must match none of the components from term id */
} ecs_oper_kind_t;

/** Substitution with set parameters.
 * These parameters allow for substituting a term id with its super- or subsets
 * for a specified relationship. This enables functionality such as selecting
 * components from a base (IsA) or a parent (ChildOf) in a single term */
typedef struct ecs_term_set_t {
    ecs_entity_t relation;      /* Relationship to substitute (default = IsA) */
    uint8_t mask;               /* Substitute as self, subset, superset */
    int32_t min_depth;          /* Min depth of subset/superset substitution */
    int32_t max_depth;          /* Max depth of subset/superset substitution */
} ecs_term_set_t;

/** Type that describes a single identifier in a term */
typedef struct ecs_term_id_t {
    ecs_entity_t entity;        /* Entity (default = This) */
    char *name;                 /* Name (default = ".") */
    ecs_var_kind_t var;         /* Is id a variable (default yes if name is 
                                 * all caps & entity is 0) */
    ecs_term_set_t set;         /* Set substitution parameters */
} ecs_term_id_t;

/** Type that describes a single column in the system signature */
struct ecs_term_t {
    ecs_id_t id;                /* Can be used instead of pred, args and role to
                                 * set component/pair id. If not set, it will be 
                                 * computed from predicate, object. If set, the
                                 * subject cannot be set, or be set to This. */
    
    ecs_inout_kind_t inout;     /* Access to contents matched with term */
    ecs_term_id_t pred;         /* Predicate of term */
    ecs_term_id_t subj;         /* Subject of term */
    ecs_term_id_t obj;          /* Object of term */
    ecs_oper_kind_t oper;       /* Operator of term */
    ecs_id_t role;              /* Role of term */
    char *name;                 /* Name of term */

    int32_t index;              /* Computed term index in filter which takes 
                                 * into account folded OR terms */

    bool move;                  /* When true, this signals to ecs_term_copy that
                                 * the resources held by this term may be moved
                                 * into the destination term. */
};

/** Filters alllow for ad-hoc quick filtering of entity tables. */
struct ecs_filter_t {
    ecs_header_t hdr;
    
    ecs_term_t *terms;         /* Array containing terms for filter */
    int32_t term_count;        /* Number of elements in terms array */
    int32_t term_count_actual; /* Processed count, which folds OR terms */

    ecs_term_t term_cache[ECS_TERM_CACHE_SIZE]; /* Cache for small filters */
    bool term_cache_used;

    ecs_flags32_t flags;       /* Filter flags */
    
    char *name;                /* Name of filter (optional) */
    char *variable_names[1];   /* Array with variable names */

    ecs_iterable_t iterable;   /* Iterable mixin */
};

/** A trigger reacts to events matching a single term */
struct ecs_trigger_t {
    ecs_term_t term;            /* Term describing the trigger condition id */

    /* Trigger events */
    ecs_entity_t events[ECS_TRIGGER_DESC_EVENT_COUNT_MAX];
    int32_t event_count;

    ecs_iter_action_t callback; /* Callback */

    void *ctx;                  /* Callback context */
    void *binding_ctx;          /* Binding context (for language bindings) */

    ecs_ctx_free_t ctx_free;    /* Callback to free ctx */
    ecs_ctx_free_t binding_ctx_free; /* Callback to free binding_ctx */
    
    ecs_entity_t entity;        /* Trigger entity */
    ecs_entity_t self;          /* Entity associated with observer */

    ecs_observable_t *observable;  /* Observable for trigger */

    bool match_prefab;          /* Should trigger ignore prefabs */
    bool match_disabled;        /* Should trigger ignore disabled entities */
    bool instanced;             /* See ecs_filter_desc_t */

    uint64_t id;                /* Internal id */
    int32_t *last_event_id;     /* Optional pointer to observer last_event_id */
};

/* An observer reacts to events matching a filter */
struct ecs_observer_t {
    ecs_filter_t filter;

    /* Triggers created by observer */
    ecs_vector_t *triggers;

    /* Observer events */
    ecs_entity_t events[ECS_TRIGGER_DESC_EVENT_COUNT_MAX];
    int32_t event_count;   
    
    ecs_iter_action_t callback; /* See ecs_observer_desc_t::callback */
    ecs_run_action_t run;       /* See ecs_observer_desc_t::run */

    void *ctx;                  /* Callback context */
    void *binding_ctx;          /* Binding context (for language bindings) */

    ecs_ctx_free_t ctx_free;    /* Callback to free ctx */
    ecs_ctx_free_t binding_ctx_free; /* Callback to free binding_ctx */
    
    ecs_entity_t entity;        /* Observer entity */
    ecs_entity_t self;          /* Entity associated with observer */

    ecs_observable_t *observable;  /* Observable for observer */

    uint64_t id;                /* Internal id */  
    int32_t last_event_id;      /* Last handled event id */

    bool is_monitor;            /* If true, the observer only triggers when the
                                 * filter did not match with the entity before
                                 * the event happened. */
};

/** @} */


/**
 * @file api_types.h
 * @brief Supporting types for the public API.
 *
 * This file contains types that are typically not used by an application but 
 * support the public API, and therefore must be exposed. This header should not
 * be included by itself.
 */

#ifndef FLECS_API_TYPES_H
#define FLECS_API_TYPES_H


#ifdef __cplusplus
extern "C" {
#endif


////////////////////////////////////////////////////////////////////////////////
//// Opaque types
////////////////////////////////////////////////////////////////////////////////

/** A stage enables modification while iterating and from multiple threads */
typedef struct ecs_stage_t ecs_stage_t;

/** A record stores data to map an entity id to a location in a table */
typedef struct ecs_record_t ecs_record_t;

/** Table column */
typedef struct ecs_column_t ecs_column_t;

/** Table data */
typedef struct ecs_data_t ecs_data_t;

/* Sparse set */
typedef struct ecs_sparse_t ecs_sparse_t;

/* Switch list */
typedef struct ecs_switch_t ecs_switch_t;

/* Internal structure to lookup tables for a (component) id */
typedef struct ecs_id_record_t ecs_id_record_t;

/* Cached query table data */
typedef struct ecs_query_table_node_t ecs_query_table_node_t;

/* Internal table storage record */
struct ecs_table_record_t;

////////////////////////////////////////////////////////////////////////////////
//// Non-opaque types
////////////////////////////////////////////////////////////////////////////////

/** Mixin for emitting events to triggers/observers */
struct ecs_observable_t {
    ecs_sparse_t *events;  /* sparse<event, ecs_event_record_t> */
};

/** Record for entity index */
struct ecs_record_t {
    ecs_table_t *table;  /* Identifies a type (and table) in world */
    uint32_t row;        /* Table row of the entity */
};

/** Range in table */
typedef struct ecs_table_range_t {
    ecs_table_t *table;
    int32_t offset;       /* Leave both members to 0 to cover entire table */
    int32_t count;       
} ecs_table_range_t;

/** Value of query variable */
typedef struct ecs_var_t {
    ecs_table_range_t range; /* Set when variable stores a range of entities */
    ecs_entity_t entity;     /* Set when variable stores single entity */

    /* Most entities can be stored as a range by setting range.count to 1, 
     * however in order to also be able to store empty entities in variables, 
     * a separate entity member is needed. Both range and entity may be set at
     * the same time, as long as they are consistent. */
} ecs_var_t;

/** Cached reference. */
struct ecs_ref_t {
    ecs_entity_t entity;    /* Entity */
    ecs_entity_t id;        /* Component id */
    struct ecs_table_record_t *tr; /* Table record for component */
    ecs_record_t *record;   /* Entity index record */
};

/* Page-iterator specific data */
typedef struct ecs_page_iter_t {
    int32_t offset;
    int32_t limit;
    int32_t remaining;
} ecs_page_iter_t;

/* Worker-iterator specific data */
typedef struct ecs_worker_iter_t {
    int32_t index;
    int32_t count;
} ecs_worker_iter_t;

/* Convenience struct to iterate table array for id */
typedef struct ecs_table_cache_iter_t {
    struct ecs_table_cache_hdr_t *cur, *next;
} ecs_table_cache_iter_t;

/** Term-iterator specific data */
typedef struct ecs_term_iter_t {
    ecs_term_t term;
    ecs_id_record_t *self_index;
    ecs_id_record_t *set_index;

    ecs_id_record_t *cur;
    ecs_table_cache_iter_t it;
    int32_t index;
    
    ecs_table_t *table;
    int32_t cur_match;
    int32_t match_count;
    int32_t last_column;

    bool empty_tables;

    /* Storage */
    ecs_id_t id;
    int32_t column;
    ecs_entity_t subject;
    ecs_size_t size;
    void *ptr;
} ecs_term_iter_t;

typedef enum ecs_iter_kind_t {
    EcsIterEvalCondition,
    EcsIterEvalTables,
    EcsIterEvalChain,
    EcsIterEvalNone
} ecs_iter_kind_t;

/** Filter-iterator specific data */
typedef struct ecs_filter_iter_t {
    ecs_filter_t filter;
    ecs_iter_kind_t kind; 
    ecs_term_iter_t term_iter;
    int32_t matches_left;
    int32_t pivot_term;
} ecs_filter_iter_t;

/** Query-iterator specific data */
typedef struct ecs_query_iter_t {
    ecs_query_t *query;
    ecs_query_table_node_t *node, *prev;
    int32_t sparse_smallest;
    int32_t sparse_first;
    int32_t bitset_first;
    int32_t skip_count;
} ecs_query_iter_t;

/** Snapshot-iterator specific data */
typedef struct ecs_snapshot_iter_t {
    ecs_filter_t filter;
    ecs_vector_t *tables; /* ecs_table_leaf_t */
    int32_t index;
} ecs_snapshot_iter_t;  

/** Type used for iterating ecs_sparse_t */
typedef struct ecs_sparse_iter_t {
    ecs_sparse_t *sparse;
    const uint64_t *ids;
    ecs_size_t size;
    int32_t i;
    int32_t count;
} ecs_sparse_iter_t;

/** Rule-iterator specific data */
typedef struct ecs_rule_iter_t {
    const ecs_rule_t *rule;
    struct ecs_var_t *registers;         /* Variable storage (tables, entities) */
    struct ecs_rule_op_ctx_t *op_ctx;    /* Operation-specific state */
    
    int32_t *columns;                    /* Column indices */
    
    ecs_entity_t entity;                 /* Result in case of 1 entity */

    bool redo;
    int32_t op;
    int32_t sp;
} ecs_rule_iter_t;

/* Bits for tracking whether a cache was used/whether the array was allocated.
 * Used by flecs_iter_init, flecs_iter_validate and ecs_iter_fini. 
 * Constants are named to enable easy macro substitution. */
#define flecs_iter_cache_ids           (1u << 0u)
#define flecs_iter_cache_columns       (1u << 1u)
#define flecs_iter_cache_subjects      (1u << 2u)
#define flecs_iter_cache_sizes         (1u << 3u)
#define flecs_iter_cache_ptrs          (1u << 4u)
#define flecs_iter_cache_match_indices (1u << 5u)
#define flecs_iter_cache_variables     (1u << 6u)
#define flecs_iter_cache_all           (255)

/* Inline iterator arrays to prevent allocations for small array sizes */
typedef struct ecs_iter_cache_t {
    ecs_id_t ids[ECS_TERM_CACHE_SIZE];
    int32_t columns[ECS_TERM_CACHE_SIZE];
    ecs_entity_t subjects[ECS_TERM_CACHE_SIZE];
    ecs_size_t sizes[ECS_TERM_CACHE_SIZE];
    void *ptrs[ECS_TERM_CACHE_SIZE];
    int32_t match_indices[ECS_TERM_CACHE_SIZE];
    ecs_var_t variables[ECS_VARIABLE_CACHE_SIZE];

    ecs_flags8_t used;       /* For which fields is the cache used */
    ecs_flags8_t allocated; /* Which fields are allocated */
} ecs_iter_cache_t;

/* Private iterator data. Used by iterator implementations to keep track of
 * progress & to provide builtin storage. */
typedef struct ecs_iter_private_t {
    union {
        ecs_term_iter_t term;
        ecs_filter_iter_t filter;
        ecs_query_iter_t query;
        ecs_rule_iter_t rule;
        ecs_snapshot_iter_t snapshot;
        ecs_page_iter_t page;
        ecs_worker_iter_t worker;
    } iter;                       /* Iterator specific data */

    ecs_iter_cache_t cache;       /* Inline arrays to reduce allocations */
} ecs_iter_private_t;

/** Iterator */
struct ecs_iter_t {
    /* World */
    ecs_world_t *world;           /* The world */
    ecs_world_t *real_world;      /* Actual world. This differs from world when in readonly mode */

    /* Matched data */
    ecs_entity_t *entities;       /* Entity identifiers */
    void **ptrs;                  /* Pointers to components. Array if from this, pointer if not. */
    ecs_size_t *sizes;            /* Component sizes */
    ecs_table_t *table;           /* Current table */
    ecs_table_t *other_table;     /* Prev or next table when adding/removing */
    ecs_id_t *ids;                /* (Component) ids */
    ecs_var_t *variables;      /* Values of variables (if any) */
    int32_t *columns;             /* Query term to table column mapping */
    ecs_entity_t *subjects;       /* Subject (source) entities */
    int32_t *match_indices;       /* Indices of current match for term. Allows an iterator to iterate
                                   * all permutations of wildcards in query. */
    ecs_ref_t *references;        /* Cached refs to components (if iterating a cache) */
    ecs_flags64_t constrained_vars; /* Bitset that marks constrained variables */

    /* Source information */
    ecs_entity_t system;          /* The system (if applicable) */
    ecs_entity_t event;           /* The event (if applicable) */
    ecs_id_t event_id;            /* The (component) id for the event */
    ecs_entity_t self;            /* Self entity (if set for system) */

    /* Query information */
    ecs_term_t *terms;            /* Terms of query being evaluated */
    int32_t table_count;          /* Active table count for query */
    int32_t term_count;           /* Number of terms in query */
    int32_t term_index;           /* Index of term that triggered an event.
                                   * This field will be set to the 'index' field
                                   * of a trigger/observer term. */
    int32_t variable_count;       /* Number of variables for query */
    char **variable_names;        /* Names of variables (if any) */

    /* Context */
    void *param;                  /* Param passed to ecs_run */
    void *ctx;                    /* System context */
    void *binding_ctx;            /* Binding context */

    /* Time */
    FLECS_FLOAT delta_time;       /* Time elapsed since last frame */
    FLECS_FLOAT delta_system_time;/* Time elapsed since last system invocation */

    /* Iterator counters */
    int32_t frame_offset;         /* Offset relative to start of iteration */
    int32_t offset;               /* Offset relative to current table */
    int32_t count;                /* Number of entities to iterate */
    int32_t instance_count;       /* Number of entities to iterate before next table */

    /* Iterator flags */
    ecs_flags32_t flags;

    ecs_entity_t interrupted_by;  /* When set, system execution is interrupted */

    ecs_iter_private_t priv;      /* Private data */

    /* Chained iterators */
    ecs_iter_next_action_t next;  /* Function to progress iterator */
    ecs_iter_action_t callback;   /* Callback of system, trigger, observer */
    ecs_iter_fini_action_t fini;  /* Function to cleanup iterator resources */
    ecs_iter_t *chain_it;         /* Optional, allows for creating iterator chains */
};

#ifdef __cplusplus
}
#endif

#endif


/**
 * @file api_support.h
 * @brief Support functions and constants.
 *
 * Supporting types and functions that need to be exposed either in support of 
 * the public API or for unit tests, but that may change between minor / patch 
 * releases. 
 */

#ifndef FLECS_API_SUPPORT_H
#define FLECS_API_SUPPORT_H


#ifdef __cplusplus
extern "C" {
#endif

/** This reserves entity ids for components. Regular entity ids will start after
 * this constant. This affects performance of table traversal, as edges with ids 
 * lower than this constant are looked up in an array, whereas constants higher
 * than this id are looked up in a map. Increasing this value can improve
 * performance at the cost of (significantly) higher memory usage. */
#define ECS_HI_COMPONENT_ID (256) /* Maximum number of components */

/** The maximum number of nested function calls before the core will throw a
 * cycle detected error */
#define ECS_MAX_RECURSION (512)

/** Maximum length of a parser token (used by parser-related addons) */
#define ECS_MAX_TOKEN_SIZE (256)

////////////////////////////////////////////////////////////////////////////////
//// Global type handles
////////////////////////////////////////////////////////////////////////////////

/** This allows passing 0 as type to functions that accept ids */
#define FLECS__E0 0

////////////////////////////////////////////////////////////////////////////////
//// Functions used in declarative (macro) API
////////////////////////////////////////////////////////////////////////////////

FLECS_API
char* ecs_module_path_from_c(
    const char *c_name);

FLECS_API
bool ecs_component_has_actions(
    const ecs_world_t *world,
    ecs_entity_t component);

////////////////////////////////////////////////////////////////////////////////
//// Signature API
////////////////////////////////////////////////////////////////////////////////

bool ecs_identifier_is_0(
    const char *id);

const char* ecs_identifier_is_var(
    const char *id);

////////////////////////////////////////////////////////////////////////////////
//// Ctor that initializes component to 0
////////////////////////////////////////////////////////////////////////////////

FLECS_API
void ecs_default_ctor(
    void *ptr, 
    int32_t count, 
    const ecs_type_info_t *ctx);

/** Calculate offset from address */
#ifdef __cplusplus
#define ECS_OFFSET(o, offset) reinterpret_cast<void*>((reinterpret_cast<uintptr_t>(o)) + (static_cast<uintptr_t>(offset)))
#else
#define ECS_OFFSET(o, offset) (void*)(((uintptr_t)(o)) + ((uintptr_t)(offset)))
#endif

#define ECS_ELEM(ptr, size, index) ECS_OFFSET(ptr, (size) * (index))

/** Enable/disable bitsets */
#define ECS_BIT_SET(flags, bit) (flags) |= (bit)
#define ECS_BIT_CLEAR(flags, bit) (flags) &= ~(bit) 
#define ECS_BIT_COND(flags, bit, cond) ((cond) \
    ? (ECS_BIT_SET(flags, bit)) \
    : (ECS_BIT_CLEAR(flags, bit)))
#define ECS_BIT_IS_SET(flags, bit) ((flags) & (bit))

#ifdef __cplusplus
}
#endif

#endif

/**
 * @file sparse.h
 * @brief Sparse set datastructure.
 *
 * This is an implementation of a paged sparse set that stores the payload in
 * the sparse array.
 *
 * A sparse set has a dense and a sparse array. The sparse array is directly
 * indexed by a 64 bit identifier. The sparse element is linked with a dense
 * element, which allows for liveliness checking. The liveliness check itself
 * can be performed by doing (psuedo code):
 *  dense[sparse[sparse_id].dense] == sparse_id
 *
 * To ensure that the sparse array doesn't have to grow to a large size when
 * using large sparse_id's, the sparse set uses paging. This cuts up the array
 * into several pages of 4096 elements. When an element is set, the sparse set
 * ensures that the corresponding page is created. The page associated with an
 * id is determined by shifting a bit 12 bits to the right.
 *
 * The sparse set keeps track of a generation count per id, which is increased
 * each time an id is deleted. The generation is encoded in the returned id.
 *
 * This sparse set implementation stores payload in the sparse array, which is
 * not typical. The reason for this is to guarantee that (in combination with
 * paging) the returned payload pointers are stable. This allows for various
 * optimizations in the parts of the framework that uses the sparse set.
 *
 * The sparse set has been designed so that new ids can be generated in bulk, in
 * an O(1) operation. The way this works is that once a dense-sparse pair is
 * created, it is never unpaired. Instead it is moved to the end of the dense
 * array, and the sparse set stores an additional count to keep track of the
 * last alive id in the sparse set. To generate new ids in bulk, the sparse set
 * only needs to increase this count by the number of requested ids.
 */

#ifndef FLECS_SPARSE_H
#define FLECS_SPARSE_H


#ifdef __cplusplus
extern "C" {
#endif

struct ecs_sparse_t {
    ecs_vector_t *dense;        /* Dense array with indices to sparse array. The
                                 * dense array stores both alive and not alive
                                 * sparse indices. The 'count' member keeps
                                 * track of which indices are alive. */

    ecs_vector_t *chunks;       /* Chunks with sparse arrays & data */
    ecs_size_t size;            /* Element size */
    int32_t count;              /* Number of alive entries */
    uint64_t max_id_local;      /* Local max index (if no global is set) */
    uint64_t *max_id;           /* Maximum issued sparse index */
};

/** Initialize sparse set */
FLECS_DBG_API
void _flecs_sparse_init(
    ecs_sparse_t *sparse,
    ecs_size_t elem_size);

#define flecs_sparse_init(sparse, T)\
    _flecs_sparse_init(sparse, ECS_SIZEOF(T))

/** Create new sparse set */
FLECS_DBG_API
ecs_sparse_t* _flecs_sparse_new(
    ecs_size_t elem_size);

#define flecs_sparse_new(T)\
    _flecs_sparse_new(ECS_SIZEOF(T))

FLECS_DBG_API
void _flecs_sparse_fini(
    ecs_sparse_t *sparse);

#define flecs_sparse_fini(sparse)\
    _flecs_sparse_fini(sparse)

/** Free sparse set */
FLECS_DBG_API
void flecs_sparse_free(
    ecs_sparse_t *sparse);

/** Remove all elements from sparse set */
FLECS_DBG_API
void flecs_sparse_clear(
    ecs_sparse_t *sparse);

/** Set id source. This allows the sparse set to use an external variable for
 * issuing and increasing new ids. */
FLECS_DBG_API
void flecs_sparse_set_id_source(
    ecs_sparse_t *sparse,
    uint64_t *id_source);

/** Add element to sparse set, this generates or recycles an id */
FLECS_DBG_API
void* _flecs_sparse_add(
    ecs_sparse_t *sparse,
    ecs_size_t elem_size);

#define flecs_sparse_add(sparse, T)\
    ((T*)_flecs_sparse_add(sparse, ECS_SIZEOF(T)))

/** Get last issued id. */
FLECS_DBG_API
uint64_t flecs_sparse_last_id(
    const ecs_sparse_t *sparse);

/** Generate or recycle a new id. */
FLECS_DBG_API
uint64_t flecs_sparse_new_id(
    ecs_sparse_t *sparse);

/** Generate or recycle new ids in bulk. The returned pointer points directly to
 * the internal dense array vector with sparse ids. Operations on the sparse set
 * can (and likely will) modify the contents of the buffer. */
FLECS_DBG_API
const uint64_t* flecs_sparse_new_ids(
    ecs_sparse_t *sparse,
    int32_t count);

/** Remove an element */
FLECS_DBG_API
void flecs_sparse_remove(
    ecs_sparse_t *sparse,
    uint64_t id);

/** Remove an element, return pointer to the value in the sparse array */
FLECS_DBG_API
void* _flecs_sparse_remove_get(
    ecs_sparse_t *sparse,
    ecs_size_t elem_size,
    uint64_t id);    

#define flecs_sparse_remove_get(sparse, T, index)\
    ((T*)_flecs_sparse_remove_get(sparse, ECS_SIZEOF(T), index))

/** Check whether an id has ever been issued. */
FLECS_DBG_API
bool flecs_sparse_exists(
    const ecs_sparse_t *sparse,
    uint64_t id);

/** Test if id is alive, which requires the generation count to match. */
FLECS_DBG_API
bool flecs_sparse_is_alive(
    const ecs_sparse_t *sparse,
    uint64_t id);

/** Return identifier with current generation set. */
FLECS_DBG_API
uint64_t flecs_sparse_get_alive(
    const ecs_sparse_t *sparse,
    uint64_t id);

/** Get value from sparse set by dense id. This function is useful in 
 * combination with flecs_sparse_count for iterating all values in the set. */
FLECS_DBG_API
void* _flecs_sparse_get_dense(
    const ecs_sparse_t *sparse,
    ecs_size_t elem_size,
    int32_t index);

#define flecs_sparse_get_dense(sparse, T, index)\
    ((T*)_flecs_sparse_get_dense(sparse, ECS_SIZEOF(T), index))

/** Get the number of alive elements in the sparse set. */
FLECS_DBG_API
int32_t flecs_sparse_count(
    const ecs_sparse_t *sparse);

/** Get the number of not alive alive elements in the sparse set. */
FLECS_DBG_API
int32_t flecs_sparse_not_alive_count(
    const ecs_sparse_t *sparse);

/** Return total number of allocated elements in the dense array */
FLECS_DBG_API
int32_t flecs_sparse_size(
    const ecs_sparse_t *sparse);

/** Get element by (sparse) id. The returned pointer is stable for the duration
 * of the sparse set, as it is stored in the sparse array. */
FLECS_DBG_API
void* _flecs_sparse_get(
    const ecs_sparse_t *sparse,
    ecs_size_t elem_size,
    uint64_t id);

#define flecs_sparse_get(sparse, T, index)\
    ((T*)_flecs_sparse_get(sparse, ECS_SIZEOF(T), index))

/** Like get_sparse, but don't care whether element is alive or not. */
FLECS_DBG_API
void* _flecs_sparse_get_any(
    const ecs_sparse_t *sparse,
    ecs_size_t elem_size,
    uint64_t id);

#define flecs_sparse_get_any(sparse, T, index)\
    ((T*)_flecs_sparse_get_any(sparse, ECS_SIZEOF(T), index))

/** Get or create element by (sparse) id. */
FLECS_DBG_API
void* _flecs_sparse_ensure(
    ecs_sparse_t *sparse,
    ecs_size_t elem_size,
    uint64_t id);

#define flecs_sparse_ensure(sparse, T, index)\
    ((T*)_flecs_sparse_ensure(sparse, ECS_SIZEOF(T), index))

/** Set value. */
FLECS_DBG_API
void* _flecs_sparse_set(
    ecs_sparse_t *sparse,
    ecs_size_t elem_size,
    uint64_t id,
    void *value);

#define flecs_sparse_set(sparse, T, index, value)\
    ((T*)_flecs_sparse_set(sparse, ECS_SIZEOF(T), index, value))

/** Get pointer to ids (alive and not alive). Use with count() or size(). */
FLECS_DBG_API
const uint64_t* flecs_sparse_ids(
    const ecs_sparse_t *sparse);

/** Set size of the dense array. */
FLECS_DBG_API
void flecs_sparse_set_size(
    ecs_sparse_t *sparse,
    int32_t elem_count);

/** Copy sparse set into a new sparse set. */
FLECS_DBG_API
ecs_sparse_t* flecs_sparse_copy(
    const ecs_sparse_t *src);    

/** Restore sparse set into destination sparse set. */
FLECS_DBG_API
void flecs_sparse_restore(
    ecs_sparse_t *dst,
    const ecs_sparse_t *src);

/** Get memory usage of sparse set. */
FLECS_DBG_API
void flecs_sparse_memory(
    ecs_sparse_t *sparse,
    int32_t *allocd,
    int32_t *used);

FLECS_DBG_API
ecs_sparse_iter_t _flecs_sparse_iter(
    ecs_sparse_t *sparse,
    ecs_size_t elem_size);

#define flecs_sparse_iter(sparse, T)\
    _flecs_sparse_iter(sparse, ECS_SIZEOF(T))

#ifndef FLECS_LEGACY
#define flecs_sparse_each(sparse, T, var, ...)\
    {\
        int var##_i, var##_count = ecs_sparse_count(sparse);\
        for (var##_i = 0; var##_i < var##_count; var##_i ++) {\
            T* var = ecs_sparse_get_dense(sparse, T, var##_i);\
            __VA_ARGS__\
        }\
    }
#endif

/* Publicly exposed APIs 
 * The flecs_ functions aren't exposed directly as this can cause some
 * optimizers to not consider them for link time optimization. */

FLECS_API
ecs_sparse_t* _ecs_sparse_new(
    ecs_size_t elem_size);

#define ecs_sparse_new(T)\
    _ecs_sparse_new(ECS_SIZEOF(T))

FLECS_API
void* _ecs_sparse_add(
    ecs_sparse_t *sparse,
    ecs_size_t elem_size);

#define ecs_sparse_add(sparse, T)\
    ((T*)_ecs_sparse_add(sparse, ECS_SIZEOF(T)))

FLECS_API
uint64_t ecs_sparse_last_id(
    const ecs_sparse_t *sparse);

FLECS_API
int32_t ecs_sparse_count(
    const ecs_sparse_t *sparse);

/** Override the generation count for a specific id */
FLECS_API
void flecs_sparse_set_generation(
    ecs_sparse_t *sparse,
    uint64_t id);

FLECS_API
void* _ecs_sparse_get_dense(
    const ecs_sparse_t *sparse,
    ecs_size_t elem_size,
    int32_t index);

#define ecs_sparse_get_dense(sparse, T, index)\
    ((T*)_ecs_sparse_get_dense(sparse, ECS_SIZEOF(T), index))

FLECS_API
void* _ecs_sparse_get(
    const ecs_sparse_t *sparse,
    ecs_size_t elem_size,
    uint64_t id);

#define ecs_sparse_get(sparse, T, index)\
    ((T*)_ecs_sparse_get(sparse, ECS_SIZEOF(T), index))

#ifdef __cplusplus
}
#endif

#endif

/**
 * @file hashmap.h
 * @brief Hashmap datastructure.
 *
 * Datastructure that computes a hash to store & retrieve values. Similar to
 * ecs_map_t, but allows for arbitrary keytypes.
 */

#ifndef FLECS_HASHMAP_H
#define FLECS_HASHMAP_H


#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    ecs_vector_t *keys;
    ecs_vector_t *values;
} ecs_hm_bucket_t;

typedef struct {
    ecs_hash_value_action_t hash;
    ecs_compare_action_t compare;
    ecs_size_t key_size;
    ecs_size_t value_size;
    ecs_map_t impl;
} ecs_hashmap_t;

typedef struct {
    ecs_map_iter_t it;
    ecs_hm_bucket_t *bucket;
    int32_t index;
} flecs_hashmap_iter_t;

typedef struct {
    void *key;
    void *value;
    uint64_t hash;
} flecs_hashmap_result_t;

FLECS_DBG_API
void _flecs_hashmap_init(
    ecs_hashmap_t *hm,
    ecs_size_t key_size,
    ecs_size_t value_size,
    ecs_hash_value_action_t hash,
    ecs_compare_action_t compare);

#define flecs_hashmap_init(hm, K, V, compare, hash)\
    _flecs_hashmap_init(hm, ECS_SIZEOF(K), ECS_SIZEOF(V), compare, hash)

FLECS_DBG_API
void flecs_hashmap_fini(
    ecs_hashmap_t *map);

FLECS_DBG_API
void* _flecs_hashmap_get(
    const ecs_hashmap_t *map,
    ecs_size_t key_size,
    const void *key,
    ecs_size_t value_size);

#define flecs_hashmap_get(map, key, V)\
    (V*)_flecs_hashmap_get(map, ECS_SIZEOF(*key), key, ECS_SIZEOF(V))

FLECS_DBG_API
flecs_hashmap_result_t _flecs_hashmap_ensure(
    ecs_hashmap_t *map,
    ecs_size_t key_size,
    const void *key,
    ecs_size_t value_size);

#define flecs_hashmap_ensure(map, key, V)\
    _flecs_hashmap_ensure(map, ECS_SIZEOF(*key), key, ECS_SIZEOF(V))

FLECS_DBG_API
void _flecs_hashmap_set(
    ecs_hashmap_t *map,
    ecs_size_t key_size,
    void *key,
    ecs_size_t value_size,
    const void *value);

#define flecs_hashmap_set(map, key, value)\
    _flecs_hashmap_set(map, ECS_SIZEOF(*key), key, ECS_SIZEOF(*value), value)

FLECS_DBG_API
void _flecs_hashmap_remove(
    ecs_hashmap_t *map,
    ecs_size_t key_size,
    const void *key,
    ecs_size_t value_size);

#define flecs_hashmap_remove(map, key, V)\
    _flecs_hashmap_remove(map, ECS_SIZEOF(*key), key, ECS_SIZEOF(V))

FLECS_DBG_API
void _flecs_hashmap_remove_w_hash(
    ecs_hashmap_t *map,
    ecs_size_t key_size,
    const void *key,
    ecs_size_t value_size,
    uint64_t hash);

#define flecs_hashmap_remove_w_hash(map, key, V, hash)\
    _flecs_hashmap_remove_w_hash(map, ECS_SIZEOF(*key), key, ECS_SIZEOF(V), hash)

FLECS_DBG_API
ecs_hm_bucket_t* flecs_hashmap_get_bucket(
    const ecs_hashmap_t *map,
    uint64_t hash);

FLECS_DBG_API
void flecs_hm_bucket_remove(
    ecs_hashmap_t *map,
    ecs_hm_bucket_t *bucket,
    uint64_t hash,
    int32_t index);

FLECS_DBG_API
void flecs_hashmap_copy(
    const ecs_hashmap_t *src,
    ecs_hashmap_t *dst);

FLECS_DBG_API
flecs_hashmap_iter_t flecs_hashmap_iter(
    ecs_hashmap_t *map);

FLECS_DBG_API
void* _flecs_hashmap_next(
    flecs_hashmap_iter_t *it,
    ecs_size_t key_size,
    void *key_out,
    ecs_size_t value_size);

#define flecs_hashmap_next(map, V)\
    (V*)_flecs_hashmap_next(map, 0, NULL, ECS_SIZEOF(V))

#define flecs_hashmap_next_w_key(map, K, key, V)\
    (V*)_flecs_hashmap_next(map, ECS_SIZEOF(K), key, ECS_SIZEOF(V))

#ifdef __cplusplus
}
#endif

#endif


/**
 * @defgroup desc_types Types used for creating API constructs
 * @{
 */

/** Used with ecs_entity_init */
typedef struct ecs_entity_desc_t {
    int32_t _canary;

    ecs_entity_t entity; /* Optional existing entity handle. */

    const char *name;    /* Name of the entity. If no entity is provided, an 
                          * entity with this name will be looked up first. When
                          * an entity is provided, the name will be verified
                          * with the existing entity. */

    const char *sep;     /* Optional custom separator for hierarchical names */
    const char *root_sep; /* Optional, used for identifiers relative to root */

    const char *symbol;  /* Optional entity symbol. A symbol is an unscoped
                          * identifier that can be used to lookup an entity. The
                          * primary use case for this is to associate the entity
                          * with a language identifier, such as a type or 
                          * function name, where these identifiers differ from
                          * the name they are registered with in flecs. For 
                          * example, C type "EcsPosition" might be registered
                          * as "flecs.components.transform.Position", with the
                          * symbol set to "EcsPosition". */

    bool use_low_id;     /* When set to true, a low id (typically reserved for
                          * components) will be used to create the entity, if
                          * no id is specified. */

    /* Array of ids to add to the new or existing entity. */
    ecs_id_t add[ECS_ID_CACHE_SIZE];

    /* String expression with components to add */
    const char *add_expr;
} ecs_entity_desc_t;

/** Used with ecs_bulk_init */
typedef struct ecs_bulk_desc_t { 
    int32_t _canary;

    ecs_entity_t *entities;  /* Entities to bulk insert. Entity ids provided by 
                         * the application application must be empty (cannot
                         * have components). If no entity ids are provided, the
                         * operation will create 'count' new entities. */

    int32_t count;      /* Number of entities to create/populate */

    ecs_id_t ids[ECS_ID_CACHE_SIZE]; /* Ids to create the entities with */

    void **data;       /* Array with component data to insert. Each element in 
                        * the array must correspond with an element in the ids
                        * array. If an element in the ids array is a tag, the
                        * data array must contain a NULL. An element may be
                        * set to NULL for a component, in which case the
                        * component will not be set by the operation. */

    ecs_table_t *table; /* Table to insert the entities into. Should not be set
                         * at the same time as ids. When 'table' is set at the
                         * same time as 'data', the elements in the data array
                         * must correspond with the ids in the table's type. */

} ecs_bulk_desc_t;


/** Used with ecs_component_init. */
typedef struct ecs_component_desc_t {
    int32_t _canary;

    ecs_entity_desc_t entity;           /* Parameters for component entity */
    size_t size;                        /* Component size */
    size_t alignment;                   /* Component alignment */
} ecs_component_desc_t;


/** Used with ecs_type_init. */
typedef struct ecs_type_desc_t {
    int32_t _canary;

    ecs_entity_desc_t entity;           /* Parameters for type entity */
    ecs_id_t ids[ECS_ID_CACHE_SIZE];   /* Ids to include in type */
    const char *ids_expr;               /* Id expression to include in type */
} ecs_type_desc_t;


/** Used with ecs_filter_init. */
typedef struct ecs_filter_desc_t {
    int32_t _canary;

    /* Terms of the filter. If a filter has more terms than 
     * ECS_TERM_CACHE_SIZE use terms_buffer */
    ecs_term_t terms[ECS_TERM_DESC_CACHE_SIZE];

    /* For filters with lots of terms an outside array can be provided. */
    ecs_term_t *terms_buffer;
    int32_t terms_buffer_count;

    /* When true, don't populate data fields of iterator. This is useful for
     * filters that are only interested in finding the set of matching tables or
     * entities, and not in the component data. */
    bool filter;

    /* When true, terms returned by an iterator may either contain 1 or N 
     * elements, where terms with N elements are owned, and terms with 1 element 
     * are shared, for example from a parent or base entity. When false, the 
     * iterator will at most return 1 element when the result contains both 
     * owned and shared terms. */ 
    bool instanced;

    /* Match empty tables. By default empty tables are not returned. */ 
    bool match_empty_tables;

    /* Filter expression. Should not be set at the same time as terms array */
    const char *expr;

    /* Optional name of filter, used for debugging. If a filter is created for
     * a system, the provided name should match the system name. */
    const char *name;
} ecs_filter_desc_t;


/** Used with ecs_query_init. */
typedef struct ecs_query_desc_t {
    int32_t _canary;

    /* Filter for the query */
    ecs_filter_desc_t filter;

    /* Component to be used by order_by */
    ecs_entity_t order_by_component;

    /* Callback used for ordering query results. If order_by_id is 0, the 
     * pointer provided to the callback will be NULL. If the callback is not
     * set, results will not be ordered. */
    ecs_order_by_action_t order_by;

    /* Id to be used by group_by. This id is passed to the group_by function and
     * can be used identify the part of an entity type that should be used for
     * grouping. */
    ecs_id_t group_by_id;

    /* Callback used for grouping results. If the callback is not set, results
     * will not be grouped. When set, this callback will be used to calculate a
     * "rank" for each entity (table) based on its components. This rank is then
     * used to sort entities (tables), so that entities (tables) of the same
     * rank are "grouped" together when iterated. */
    ecs_group_by_action_t group_by;

    /* Context to pass to group_by */
    void *group_by_ctx;

    /* Function to free group_by_ctx */
    ecs_ctx_free_t group_by_ctx_free;

    /* If set, the query will be created as a subquery. A subquery matches at
     * most a subset of its parent query. Subqueries do not directly receive
     * (table) notifications from the world. Instead parent queries forward
     * results to subqueries. This can improve matching performance, as fewer
     * queries need to be matched with new tables.
     * Subqueries can be nested. */
    ecs_query_t *parent;

    /* INTERNAL PROPERTY - system to be associated with query. Do not set, as 
     * this will change in future versions. */
    ecs_entity_t system;
} ecs_query_desc_t;


/** Used with ecs_trigger_init. */
typedef struct ecs_trigger_desc_t {
    int32_t _canary;

    /* Entity to associate with trigger */
    ecs_entity_desc_t entity;

    /* Term specifying the id to subscribe for */
    ecs_term_t term;

    /* Filter expression. May only contain a single term. If this field is set,
     * the term field is ignored. */
    const char *expr;

    /* Events to trigger on (OnAdd, OnRemove, OnSet, UnSet) */
    ecs_entity_t events[ECS_TRIGGER_DESC_EVENT_COUNT_MAX];

    /* Should trigger match prefabs & disabled entities */
    bool match_prefab;
    bool match_disabled;

    /* See ecs_filter_desc_t::instanced */
    bool instanced;

    /* When trigger is created, generate events from existing data. For example,
     * EcsOnAdd Position would trigger for all existing instances of Position.
     * This is only supported for events that are iterable (see EcsIterable) */
    bool yield_existing;

    /* Callback to invoke on an event */
    ecs_iter_action_t callback;

    /* Associate with entity */
    ecs_entity_t self;

    /* User context to pass to callback */
    void *ctx;

    /* Context to be used for language bindings */
    void *binding_ctx;
    
    /* Callback to free ctx */
    ecs_ctx_free_t ctx_free;

    /* Callback to free binding_ctx */     
    ecs_ctx_free_t binding_ctx_free;

    /* Observable with which to register the trigger */
    ecs_poly_t *observable;

    /* This field is usually only set if a trigger is part of an observer, and
     * points to the observer's last_event_id. This enables skipping triggers if
     * a previous trigger for the same observer already notified it. */
    int32_t *last_event_id;
} ecs_trigger_desc_t;


/** Used with ecs_observer_init. */
typedef struct ecs_observer_desc_t {
    int32_t _canary;

    /* Entity to associate with observer */
    ecs_entity_desc_t entity;

    /* Filter for observer */
    ecs_filter_desc_t filter;

    /* Events to observe (OnAdd, OnRemove, OnSet, UnSet) */
    ecs_entity_t events[ECS_TRIGGER_DESC_EVENT_COUNT_MAX];

    /* See ecs_trigger_desc_t */
    bool yield_existing;

    /* Callback to invoke on an event, invoked when the observer matches. */
    ecs_iter_action_t callback;

    /* Callback invoked on an event. When left to NULL the default runner
     * is used which matches the event with the observer's filter, and calls
     * 'callback' when it matches. 
     * A reason to override the run function is to improve performance, if there
     * are more efficient way to test whether an event matches the observer than
     * the general purpose query matcher. */
    ecs_run_action_t run;

    /* Associate with entity */
    ecs_entity_t self;

    /* User context to pass to callback */
    void *ctx;

    /* Context to be used for language bindings */
    void *binding_ctx;
    
    /* Callback to free ctx */
    ecs_ctx_free_t ctx_free;

    /* Callback to free binding_ctx */     
    ecs_ctx_free_t binding_ctx_free;

    /* Observable with which to register the trigger */
    ecs_poly_t *observable;  
} ecs_observer_desc_t;

/** @} */


/**
 * @defgroup builtin_components Builtin components
 * @{
 */

/** A (string) identifier. Used as pair with EcsName and EcsSymbol tags */
typedef struct EcsIdentifier {
    char *value;
    ecs_size_t length;
    uint64_t hash;
    uint64_t index_hash; /* Hash of existing record in current index */
    ecs_hashmap_t *index; /* Current index */
} EcsIdentifier;

/** Component information. */
typedef struct EcsComponent {
    ecs_size_t size;           /* Component size */
    ecs_size_t alignment;      /* Component alignment */
} EcsComponent;

/** Component that stores an ecs_type_t. 
 * This component allows for the creation of entities that represent a type, and
 * therefore the creation of named types. */
typedef struct EcsType {
    const ecs_type_t *type;          /* Preserved nested types */
    ecs_table_t *normalized;  /* Table with union of type + nested AND types */
} EcsType;

/** Component that contains lifecycle callbacks for a component. */
typedef struct EcsComponentLifecycle {
    ecs_xtor_t ctor;            /* ctor */
    ecs_xtor_t dtor;            /* dtor */
    ecs_copy_t copy;            /* copy assignment */
    ecs_move_t move;            /* move assignment */

    /* Ctor + copy */
    ecs_copy_t copy_ctor;

    /* Ctor + move */  
    ecs_move_t move_ctor;

    /* Ctor + move + dtor (or move_ctor + dtor).
     * This combination is typically used when a component is moved from one
     * location to a new location, like when it is moved to a new table. If
     * not set explicitly it will be derived from other callbacks. */
    ecs_move_t ctor_move_dtor;

    /* Move + dtor.
     * This combination is typically used when a component is moved from one
     * location to an existing location, like what happens during a remove. If
     * not set explicitly it will be derived from other callbacks. */
    ecs_move_t move_dtor;

    /* Callback that is invoked when an instance of a component is added. This
     * callback is invoked before triggers are invoked. */
    ecs_iter_action_t on_add;

    /* Callback that is invoked when an instance of the component is set. This
     * callback is invoked before triggers are invoked, and enable the component
     * to respond to changes on itself before others can. */
    ecs_iter_action_t on_set;

    /* Callback that is invoked when an instance of the component is removed. 
     * This callback is invoked after the triggers are invoked, and before the
     * destructor is invoked. */
    ecs_iter_action_t on_remove;

    /* User defined context */
    void *ctx;                  
} EcsComponentLifecycle;

/** Type that contains component information (passed to ctors/dtors/...) */
struct ecs_type_info_t {
    ecs_size_t size;
    ecs_size_t alignment;
    EcsComponentLifecycle lifecycle;
    ecs_entity_t component;
    bool lifecycle_set;
};

/** Component that stores reference to trigger */
typedef struct EcsTrigger {
    const ecs_trigger_t *trigger;
} EcsTrigger;

/** Component that stores reference to observer */
typedef struct EcsObserver {
    const ecs_observer_t *observer;
} EcsObserver;

/** Component for storing a query */
typedef struct EcsQuery {
    ecs_query_t *query;
} EcsQuery;

/** Component for iterable entities */
typedef ecs_iterable_t EcsIterable;

/** @} */


/**
 * @defgroup misc_types Miscalleneous types
 * @{
 */

/** Type that contains information about the world. */
typedef struct ecs_world_info_t {
    ecs_entity_t last_component_id;   /* Last issued component entity id */
    ecs_entity_t last_id;             /* Last issued entity id */
    ecs_entity_t min_id;              /* First allowed entity id */
    ecs_entity_t max_id;              /* Last allowed entity id */

    FLECS_FLOAT delta_time_raw;       /* Raw delta time (no time scaling) */
    FLECS_FLOAT delta_time;           /* Time passed to or computed by ecs_progress */
    FLECS_FLOAT time_scale;           /* Time scale applied to delta_time */
    FLECS_FLOAT target_fps;           /* Target fps */
    FLECS_FLOAT frame_time_total;     /* Total time spent processing a frame */
    float system_time_total;          /* Total time spent in systems */
    float merge_time_total;           /* Total time spent in merges */
    FLECS_FLOAT world_time_total;     /* Time elapsed in simulation */
    FLECS_FLOAT world_time_total_raw; /* Time elapsed in simulation (no scaling) */
    
    int32_t frame_count_total;        /* Total number of frames */
    int32_t merge_count_total;        /* Total number of merges */

    int32_t id_create_total;          /* Total number of times a new id was created */
    int32_t id_delete_total;          /* Total number of times an id was deleted */
    int32_t table_create_total;       /* Total number of times a table was created */
    int32_t table_delete_total;       /* Total number of times a table was deleted */
    int32_t pipeline_build_count_total; /* Total number of pipeline builds */
    int32_t systems_ran_frame;  /* Total number of systems ran in last frame */

    int32_t id_count;                 /* Number of ids in the world (excluding wildcards) */
    int32_t tag_id_count;             /* Number of tag (no data) ids in the world */
    int32_t component_id_count;       /* Number of component (data) ids in the world */
    int32_t pair_id_count;            /* Number of pair ids in the world */
    int32_t wildcard_id_count;        /* Number of wildcard ids */

    int32_t table_count;              /* Number of tables */
    int32_t tag_table_count;          /* Number of tag-only tables */
    int32_t trivial_table_count;      /* Number of tables with trivial components (no lifecycle callbacks) */
    int32_t empty_table_count;        /* Number of tables without entities */
    int32_t table_record_count;       /* Total number of table records (entries in table caches) */
    int32_t table_storage_count;      /* Total number of table storages */
} ecs_world_info_t;

/** @} */

/* Only include deprecated definitions if deprecated addon is required */
#ifdef FLECS_DEPRECATED
/**
 * @file deprecated.h
 * @brief The deprecated addon contains deprecated operations.
 */

#ifdef FLECS_DEPRECATED

#ifndef FLECS_DEPRECATED_H
#define FLECS_DEPRECATED_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif

#endif

#endif


/**
 * @defgroup type_roles Type Roles
 * @{
 */

/* Type roles are used to indicate the role of an entity in a type. If no flag
 * is specified, the entity is interpreted as a regular component or tag. Flags
 * are added to an entity by using a bitwise OR (|). */

/** Role bit added to roles to differentiate between roles and generations */
#define ECS_ROLE (1ull << 63)

/** Cases are used to switch between mutually exclusive components */
FLECS_API extern const ecs_id_t ECS_CASE;

/** Switches allow for fast switching between mutually exclusive components */
FLECS_API extern const ecs_id_t ECS_SWITCH;

/** The PAIR role indicates that the entity is a pair identifier. */
FLECS_API extern const ecs_id_t ECS_PAIR;

/** Enforce ownership of a component */
FLECS_API extern const ecs_id_t ECS_OVERRIDE;

/** Track whether component is enabled or not */
FLECS_API extern const ecs_id_t ECS_DISABLED;

/** @} */


/**
 * @defgroup builtin_tags Builtin components & tags
 * @{
 */

/** Builtin component ids */
FLECS_API extern const ecs_entity_t ecs_id(EcsComponent);
FLECS_API extern const ecs_entity_t ecs_id(EcsComponentLifecycle);
FLECS_API extern const ecs_entity_t ecs_id(EcsType);
FLECS_API extern const ecs_entity_t ecs_id(EcsIdentifier);
FLECS_API extern const ecs_entity_t ecs_id(EcsTrigger);
FLECS_API extern const ecs_entity_t ecs_id(EcsQuery);
FLECS_API extern const ecs_entity_t ecs_id(EcsObserver);
FLECS_API extern const ecs_entity_t ecs_id(EcsIterable);

/* System module component ids */
FLECS_API extern const ecs_entity_t ecs_id(EcsSystem);
FLECS_API extern const ecs_entity_t ecs_id(EcsTickSource);

/** Pipeline module component ids */
FLECS_API extern const ecs_entity_t ecs_id(EcsPipelineQuery);

/** Timer module component ids */
FLECS_API extern const ecs_entity_t ecs_id(EcsTimer);
FLECS_API extern const ecs_entity_t ecs_id(EcsRateFilter);

/** Root scope for builtin flecs entities */
FLECS_API extern const ecs_entity_t EcsFlecs;

/* Core module scope */
FLECS_API extern const ecs_entity_t EcsFlecsCore;

/* Hidden module scope. This is used as the default location for (anonymous)
 * entities associated with triggers & observers to keeps other scopes clean. */
FLECS_API extern const ecs_entity_t EcsFlecsHidden;

/* Entity associated with world (used for "attaching" components to world) */
FLECS_API extern const ecs_entity_t EcsWorld;

/* Wildcard entity ("*"). Matches any id, returns all matches. */
FLECS_API extern const ecs_entity_t EcsWildcard;

/* Any entity ("_"). Matches any id, returns only the first. */
FLECS_API extern const ecs_entity_t EcsAny;

/* This entity ("."). Used in expressions to indicate This variable */
FLECS_API extern const ecs_entity_t EcsThis;

/* Variable entity ("$"). Used in expressions to prefix variable names */
FLECS_API extern const ecs_entity_t EcsVariable;

/* Marks a relationship as transitive. 
 * Behavior: 
 *   if R(X, Y) and R(Y, Z) then R(X, Z)
 */
FLECS_API extern const ecs_entity_t EcsTransitive;

/* Marks a relatoinship as reflexive.
 * Behavior: 
 *   R(X, X) == true
 */
FLECS_API extern const ecs_entity_t EcsReflexive;

/** Ensures that entity/component cannot be used as object in IsA relation.
 * Final can improve the performance of rule-based queries, as they will not 
 * attempt to substitute a final component with its subsets.
 * 
 * Behavior: 
 *   if IsA(X, Y) and Final(Y) throw error
 */
FLECS_API extern const ecs_entity_t EcsFinal;

/** Ensures that component is never inherited from an IsA object.
 * 
 * Behavior:
 *   if DontInherit(X) and X(B) and IsA(A, B) then X(A) is false.
 */
FLECS_API extern const ecs_entity_t EcsDontInherit;

/* Marks relationship as commutative.
 * Behavior:
 *   if R(X, Y) then R(Y, X)
 */
FLECS_API extern const ecs_entity_t EcsSymmetric;

/* Can be added to relation to indicate that the relationship can only occur
 * once on an entity. Adding a 2nd instance will replace the 1st. 
 *
 * Behavior:
 *   R(X, Y) + R(X, Z) = R(X, Z)
 */
FLECS_API extern const ecs_entity_t EcsExclusive;

/* Marks a relation as acyclic. Acyclic relations may not form cycles. */
FLECS_API extern const ecs_entity_t EcsAcyclic;

/* Ensure that a component always is added together with another component.
 * 
 * Behavior:
 *   If With(R, O) and R(X) then O(X)
 *   If With(R, O) and R(X, Y) then O(X, Y)
 */
FLECS_API extern const ecs_entity_t EcsWith;

/* Ensure that relationship target is child of specified entity.
 * 
 * Behavior:
 *   If OneOf(R, O) and R(X, Y), Y must be a child of O
 *   If OneOf(R) and R(X, Y), Y must be a child of R
 */
FLECS_API extern const ecs_entity_t EcsOneOf;

/* Can be added to relation to indicate that it should never hold data, even
 * when it or the relation object is a component. */
FLECS_API extern const ecs_entity_t EcsTag;

/* Tag to indicate name identifier */
FLECS_API extern const ecs_entity_t EcsName;

/* Tag to indicate symbol identifier */
FLECS_API extern const ecs_entity_t EcsSymbol;

/* Tag to indicate alias identifier */
FLECS_API extern const ecs_entity_t EcsAlias;

/* Used to express parent-child relations. */
FLECS_API extern const ecs_entity_t EcsChildOf;

/* Used to express is-a relations. An IsA relation indicates that the subject is
 * a subset of the relation object. For example:
 *   ecs_add_pair(world, Freighter, EcsIsA, SpaceShip);
 *
 * Here the Freighter is considered a subset of SpaceShip, meaning that every
 * entity that has Freighter also implicitly has SpaceShip.
 *
 * The subject of the relation (Freighter) inherits all components from any IsA
 * object (SpaceShip). If SpaceShip has a component "MaxSpeed", this component
 * will also appear on Freighter after adding (IsA, SpaceShip) to Freighter.
 *
 * The IsA relation is transitive. This means that if SpaceShip IsA Machine, 
 * then Freigther is also a Machine. As a result, Freighter also inherits all
 * components from Machine, just as it does from SpaceShip.
 *
 * Queries/filters may implicitly substitute predicates, subjects and objects 
 * with their IsA super/subsets. This behavior can be controlled by the "set" 
 * member of a query term.
 */
FLECS_API extern const ecs_entity_t EcsIsA;

/* Tag added to module entities */
FLECS_API extern const ecs_entity_t EcsModule;

/* Tag to indicate an entity/component/system is private to a module */
FLECS_API extern const ecs_entity_t EcsPrivate;

/* Tag added to prefab entities. Any entity with this tag is automatically
 * ignored by filters/queries, unless EcsPrefab is explicitly added. */
FLECS_API extern const ecs_entity_t EcsPrefab;

/* When this tag is added to an entity it is skipped by all queries/filters */
FLECS_API extern const ecs_entity_t EcsDisabled;

/* Event. Triggers when an id (component, tag, pair) is added to an entity */
FLECS_API extern const ecs_entity_t EcsOnAdd;

/* Event. Triggers when an id (component, tag, pair) is removed from an entity */
FLECS_API extern const ecs_entity_t EcsOnRemove;

/* Event. Triggers when a component is set for an entity */
FLECS_API extern const ecs_entity_t EcsOnSet;

/* Event. Triggers when a component is unset for an entity */
FLECS_API extern const ecs_entity_t EcsUnSet;

/* Event. Exactly-once trigger for when an entity matches/unmatches a filter */
FLECS_API extern const ecs_entity_t EcsMonitor;

/* Event. Triggers when an entity is deleted.
 * Also used as relation for defining cleanup behavior, see: 
 * https://github.com/SanderMertens/flecs/blob/master/docs/Relations.md#relation-cleanup-properties
 */
FLECS_API extern const ecs_entity_t EcsOnDelete;

/* Event. Triggers when a table is created. */
// FLECS_API extern const ecs_entity_t EcsOnCreateTable;

/* Event. Triggers when a table is deleted. */
// FLECS_API extern const ecs_entity_t EcsOnDeleteTable;

/* Event. Triggers when a table becomes empty (doesn't trigger on creation). */
FLECS_API extern const ecs_entity_t EcsOnTableEmpty;

/* Event. Triggers when a table becomes non-empty. */
FLECS_API extern const ecs_entity_t EcsOnTableFill;

/* Event. Triggers when a trigger is created. */
// FLECS_API extern const ecs_entity_t EcsOnCreateTrigger;

/* Event. Triggers when a trigger is deleted. */
// FLECS_API extern const ecs_entity_t EcsOnDeleteTrigger;

/* Event. Triggers when observable is deleted. */
// FLECS_API extern const ecs_entity_t EcsOnDeleteObservable;

/* Event. Triggers when lifecycle methods for a component are registered */
// FLECS_API extern const ecs_entity_t EcsOnComponentLifecycle;

/* Relationship used to define what should happen when an entity is deleted that
 * is added to other entities. For details see: 
 * https://github.com/SanderMertens/flecs/blob/master/docs/Relations.md#relation-cleanup-properties
 */
FLECS_API extern const ecs_entity_t EcsOnDeleteObject;

/* Specifies that a component/relation/object of relation should be removed when
 * it is deleted. Must be combined with EcsOnDelete or EcsOnDeleteObject. */
FLECS_API extern const ecs_entity_t EcsRemove;

/* Specifies that entities with a component/relation/object of relation should 
 * be deleted when the component/relation/object of relation is deleted. Must be 
 * combined with EcsOnDelete or EcsOnDeleteObject. */
FLECS_API extern const ecs_entity_t EcsDelete;

/* Specifies that whenever a component/relation/object of relation is deleted an
 * error should be thrown. Must be combined with EcsOnDelete or 
 * EcsOnDeleteObject. */
FLECS_API extern const ecs_entity_t EcsPanic;

/* Used like (EcsDefaultChildComponent, Component). When added to an entity,
 * this informs serialization formats which component to use when a value is
 * assigned to an entity without specifying the component. This is intended as
 * a hint, serialization formats are not required to use it. Adding this 
 * component does not change the behavior of core ECS operations. */
FLECS_API extern const ecs_entity_t EcsDefaultChildComponent;

/* System module tags */
FLECS_API extern const ecs_entity_t EcsInactive;

/* Pipeline module tags */
FLECS_API extern const ecs_entity_t EcsPipeline;
FLECS_API extern const ecs_entity_t EcsPreFrame;
FLECS_API extern const ecs_entity_t EcsOnLoad;
FLECS_API extern const ecs_entity_t EcsPostLoad;
FLECS_API extern const ecs_entity_t EcsPreUpdate;
FLECS_API extern const ecs_entity_t EcsOnUpdate;
FLECS_API extern const ecs_entity_t EcsOnValidate;
FLECS_API extern const ecs_entity_t EcsPostUpdate;
FLECS_API extern const ecs_entity_t EcsPreStore;
FLECS_API extern const ecs_entity_t EcsOnStore;
FLECS_API extern const ecs_entity_t EcsPostFrame;

/* Value used to quickly check if component is builtin. This is used to quickly
 * filter out tables with builtin components (for example for ecs_delete) */
#define EcsLastInternalComponentId (ecs_id(EcsSystem))

/* The first user-defined component starts from this id. Ids up to this number
 * are reserved for builtin components */
#define EcsFirstUserComponentId (32)

/* The first user-defined entity starts from this id. Ids up to this number
 * are reserved for builtin components */
#define EcsFirstUserEntityId (ECS_HI_COMPONENT_ID + 128)

/** @} */


/**
 * @defgroup world_api World API
 * @{
 */

/** Create a new world.
 * A world manages all the ECS data and supporting infrastructure. Applications 
 * must have at least one world. Entities, component and system handles are 
 * local to a world and should not be shared between worlds.
 * 
 * This operation creates a world with all builtin modules loaded. 
 *
 * @return A new world object
 */
FLECS_API
ecs_world_t* ecs_init(void);

/** Same as ecs_init, but with minimal set of modules loaded.
 *
 * @return A new world object
 */
FLECS_API
ecs_world_t* ecs_mini(void);

/** Create a new world with arguments.
 * Same as ecs_init, but allows passing in command line arguments. These can be
 * used to dynamically enable flecs features to an application. Currently these
 * arguments are not used.
 *
 * @return A new world object
 */
FLECS_API
ecs_world_t* ecs_init_w_args(
    int argc,
    char *argv[]);

/** Delete a world.
 * This operation deletes the world, and everything it contains.
 *
 * @param world The world to delete.
 * @return Zero if successful, non-zero if failed.
 */
FLECS_API
int ecs_fini(
    ecs_world_t *world);

/** Returns whether the world is being deleted.
 *
 * @param world The world.
 * @return True if being deleted, false if not.
 */
FLECS_API
bool ecs_is_fini(
    const ecs_world_t *world);

/** Register action to be executed when world is destroyed.
 * Fini actions are typically used when a module needs to clean up before a
 * world shuts down.
 * 
 * @param world The world.
 * @param action The function to execute.
 * @param ctx Userdata to pass to the function */
FLECS_API
void ecs_atfini(
    ecs_world_t *world,
    ecs_fini_action_t action,
    void *ctx);

/** Register action to be executed once after frame.
 * Post frame actions are typically used for calling operations that cannot be
 * invoked during iteration, such as changing the number of threads.
 * 
 * @param world The world.
 * @param action The function to execute.
 * @param ctx Userdata to pass to the function */
FLECS_API
void ecs_run_post_frame(
    ecs_world_t *world,
    ecs_fini_action_t action,
    void *ctx);    

/** Signal exit
 * This operation signals that the application should quit. It will cause
 * ecs_progress to return false.
 *
 * @param world The world to quit.
 */
FLECS_API
void ecs_quit(
    ecs_world_t *world);

/** Return whether a quit has been signaled.
 *
 * @param world The world.
 */
FLECS_API 
bool ecs_should_quit(
    const ecs_world_t *world);

/** Register ctor, dtor, copy & move actions for component.
 *
 * @param world The world.
 * @param id The component id for which to register the actions
 * @param actions Type that contains the component actions.
 */
FLECS_API
void ecs_set_component_actions_w_id(
    ecs_world_t *world,
    ecs_id_t id,
    const EcsComponentLifecycle *actions);

/** Set a world context.
 * This operation allows an application to register custom data with a world
 * that can be accessed anywhere where the application has the world object.
 *
 * @param world The world.
 * @param ctx A pointer to a user defined structure.
 */
FLECS_API
void ecs_set_context(
    ecs_world_t *world,
    void *ctx);

/** Get the world context.
 * This operation retrieves a previously set world context.
 *
 * @param world The world.
 * @return The context set with ecs_set_context. If no context was set, the
 *         function returns NULL.
 */
FLECS_API
void* ecs_get_context(
    const ecs_world_t *world);

/** Get world info.
 *
 * @param world The world.
 * @return Pointer to the world info. This pointer will remain valid for as long
 *         as the world is valid.
 */
FLECS_API
const ecs_world_info_t* ecs_get_world_info(
    const ecs_world_t *world);

/** Dimension the world for a specified number of entities.
 * This operation will preallocate memory in the world for the specified number
 * of entities. Specifying a number lower than the current number of entities in
 * the world will have no effect.
 *
 * @param world The world.
 * @param entity_count The number of entities to preallocate.
 */
FLECS_API
void ecs_dim(
    ecs_world_t *world,
    int32_t entity_count);

/** Set a range for issueing new entity ids.
 * This function constrains the entity identifiers returned by ecs_new to the 
 * specified range. This operation can be used to ensure that multiple processes
 * can run in the same simulation without requiring a central service that
 * coordinates issueing identifiers.
 * 
 * If id_end is set to 0, the range is infinite. If id_end is set to a non-zero
 * value, it has to be larger than id_start. If id_end is set and ecs_new is
 * invoked after an id is issued that is equal to id_end, the application will
 * abort.
 * 
 * @param world The world.
 * @param id_start The start of the range.
 * @param id_end The end of the range.
 */
FLECS_API
void ecs_set_entity_range(
    ecs_world_t *world,
    ecs_entity_t id_start,
    ecs_entity_t id_end);

/** Sets the entity's generation in the world's sparse set.
 * Used for managing manual id pools.
 *
 * @param world The world.
 * @param entity_with_generation Entity for which to set the generation with the new generation to set.
 */
FLECS_API
void ecs_set_entity_generation(
    ecs_world_t *world,
    ecs_entity_t entity_with_generation);

/** Enable/disable range limits.
 * When an application is both a receiver of range-limited entities and a
 * producer of range-limited entities, range checking needs to be temporarily
 * disabled when inserting received entities. Range checking is disabled on a 
 * stage, so setting this value is thread safe.
 *
 * @param world The world.
 * @param enable True if range checking should be enabled, false to disable.
 * @return The previous value.
 */
FLECS_API
bool ecs_enable_range_check(
    ecs_world_t *world,
    bool enable);

/** Enable world locking while in progress.
 * When locking is enabled, Flecs will lock the world while in progress. This
 * allows applications to interact with the world from other threads without
 * running into race conditions.
 *
 * This is a better alternative to applications putting a lock around calls to
 * ecs_progress, since ecs_progress can sleep when FPS control is enabled,
 * which is time during which other threads could perform work.
 *
 * Locking must be enabled before applications can use the ecs_lock and
 * ecs_unlock functions. Locking is turned off by default.
 *
 * @param world The world.
 * @param enable True if locking is to be enabled.
 * @result The previous value of the setting.
 */
FLECS_API
bool ecs_enable_locking(
    ecs_world_t *world,
    bool enable);

/** Locks the world.
 * See ecs_enable_locking for details.
 *
 * @param world The world.
 */
FLECS_API
void ecs_lock(
    ecs_world_t *world);

/** Unlocks the world.
 * See ecs_enable_locking for details.
 * 
 * @param world The world.
 */
FLECS_API
void ecs_unlock(
    ecs_world_t *world);

/** Wait until world becomes available.
 * When a non-flecs thread needs to interact with the world, it should invoke
 * this function to wait until the world becomes available (as in, it is not
 * progressing the frame). Invoking this function guarantees that the thread
 * will not starve. (as opposed to simply taking the world lock).
 *
 * An application will have to invoke ecs_end_wait after this function returns.
 * 
 * @param world The world.
 */
FLECS_API 
void ecs_begin_wait(
    ecs_world_t *world);

/** Release world after calling ecs_begin_wait.
 * This operation should be invoked after invoking ecs_begin_wait, and will
 * release the world back to the thread running the main loop.
 *
 * @param world The world.
 */
FLECS_API 
void ecs_end_wait(
    ecs_world_t *world);

/** Measure frame time. 
 * Frame time measurements measure the total time passed in a single frame, and 
 * how much of that time was spent on systems and on merging.
 *
 * Frame time measurements add a small constant-time overhead to an application.
 * When an application sets a target FPS, frame time measurements are enabled by
 * default.
 *
 * @param world The world.
 * @param enable Whether to enable or disable frame time measuring.
 */
FLECS_API void ecs_measure_frame_time(
    ecs_world_t *world,
    bool enable);

/** Measure system time. 
 * System time measurements measure the time spent in each system.
 *
 * System time measurements add overhead to every system invocation and 
 * therefore have a small but measurable impact on application performance.
 * System time measurements must be enabled before obtaining system statistics.
 *
 * @param world The world.
 * @param enable Whether to enable or disable system time measuring.
 */
FLECS_API void ecs_measure_system_time(
    ecs_world_t *world,
    bool enable);   

/** Set target frames per second (FPS) for application.
 * Setting the target FPS ensures that ecs_progress is not invoked faster than
 * the specified FPS. When enabled, ecs_progress tracks the time passed since
 * the last invocation, and sleeps the remaining time of the frame (if any).
 *
 * This feature ensures systems are ran at a consistent interval, as well as
 * conserving CPU time by not running systems more often than required.
 *
 * Note that ecs_progress only sleeps if there is time left in the frame. Both
 * time spent in flecs as time spent outside of flecs are taken into
 * account.
 *
 * @param world The world.
 * @param fps The target FPS.
 */
FLECS_API
void ecs_set_target_fps(
    ecs_world_t *world,
    FLECS_FLOAT fps);     

/** Get current number of threads. */
FLECS_API
int32_t ecs_get_threads(
    ecs_world_t *world);

/** Force aperiodic actions.
 * The world may delay certain operations until they are necessary for the
 * application to function correctly. This may cause observable side effects
 * such as delayed triggering of events, which can be inconvenient when for
 * example running a test suite.
 * 
 * The flags parameter specifies which aperiodic actions to run. Specify 0 to
 * run all actions. Supported flags start with 'EcsAperiodic'. Flags identify
 * internal mechanisms and may change unannounced.
 * 
 * @param world The world.
 * @param flags The flags specifying which actions to run.
 */
FLECS_API
void ecs_run_aperiodic(
    ecs_world_t *world,
    ecs_flags32_t flags);

/** Cleanup empty tables.
 * This operation cleans up empty tables that meet certain conditions. Having
 * large amounts of empty tables does not negatively impact performance of the
 * ECS, but can take up considerable amounts of memory, especially in 
 * applications with many components, and many components per entity.
 * 
 * The generation specifies the minimum number of times this operation has
 * to be called before an empty table is cleaned up. If a table becomes non
 * empty, the generation is reset.
 * 
 * The operation allows for both a "clear" generation and a "delete"
 * generation. When the clear generation is reached, the table's 
 * resources are freed (like component arrays) but the table itself is not
 * deleted. When the delete generation is reached, the empty table is deleted.
 * 
 * By specifying a non-zero id the cleanup logic can be limited to tables with
 * a specific (component) id. The operation will only increase the generation
 * count of matching tables.
 * 
 * The min_id_count specifies a lower bound for the number of components a table
 * should have. Often the more components a table has, the more specific it is 
 * and therefore less likely to be reused.
 * 
 * The time budget specifies how long the operation should take at most.
 * 
 * @param world The world.
 * @param id Optional component filter for the tables to evaluate.
 * @param clear_generation Free table data when generation > clear_generation.
 * @param delete_generation Delete table when generation > delete_generation.
 * @param min_id_count Minimum number of component ids the table should have.
 * @param time_budget_seconds Amount of time operation is allowed to spend.
 * @return Number of deleted tables.
 */
FLECS_API
int32_t ecs_delete_empty_tables(
    ecs_world_t *world,
    ecs_id_t id,
    uint16_t clear_generation,
    uint16_t delete_generation,
    int32_t min_id_count,
    double time_budget_seconds);

/** @} */

/**
 * @defgroup creating_entities Creating Entities
 * @{
 */

/** Create new entity id.
 * This operation returns an unused entity id.
 *
 * @param world The world.
 * @return The new entity id.
 */
FLECS_API
ecs_entity_t ecs_new_id(
    ecs_world_t *world);

/** Create new low id.
 * This operation returns a new low id. Entity ids start after the
 * ECS_HI_COMPONENT_ID constant. This reserves a range of low ids for things 
 * like components, and allows parts of the code to optimize operations.
 *
 * Note that ECS_HI_COMPONENT_ID does not represent the maximum number of 
 * components that can be created, only the maximum number of components that
 * can take advantage of these optimizations.
 * 
 * This operation does not recycle ids.
 *
 * @param world The world.
 * @return The new component id.
 */
FLECS_API
ecs_entity_t ecs_new_low_id(
    ecs_world_t *world);

/** Create new entity.
 * This operation creates a new entity with a single entity in its type. The
 * entity may contain type roles. This operation recycles ids.
 *
 * @param world The world.
 * @param id The component id to initialize the new entity with.
 * @return The new entity.
 */
FLECS_API
ecs_entity_t ecs_new_w_id(
    ecs_world_t *world,
    ecs_id_t id);

/** Find or create an entity. 
 * This operation creates a new entity, or modifies an existing one. When a name
 * is set in the ecs_entity_desc_t::name field and ecs_entity_desc_t::entity is
 * not set, the operation will first attempt to find an existing entity by that
 * name. If no entity with that name can be found, it will be created.
 *
 * If both a name and entity handle are provided, the operation will check if
 * the entity name matches with the provided name. If the names do not match,
 * the function will fail and return 0.
 *
 * If an id to a non-existing entity is provided, that entity id become alive.
 * 
 * See the documentation of ecs_entity_desc_t for more details. 
 *
 * @param world The world.
 * @param desc Entity init parameters.
 * @return A handle to the new or existing entity, or 0 if failed.
 */
FLECS_API
ecs_entity_t ecs_entity_init(
    ecs_world_t *world,
    const ecs_entity_desc_t *desc);

/** Bulk create/populate new entities.
 * This operation bulk inserts a list of new or predefined entities into a
 * single table.
 * 
 * The operation does not take ownership of component arrays provided by the
 * application. Components that are non-trivially copyable will be moved into
 * the storage.
 * 
 * The operation will emit OnAdd events for each added id, and OnSet events for
 * each component that has been set.
 * 
 * If no entity ids are provided by the application, the returned array of ids
 * points to an internal datastructure which changes when new entities are
 * created/deleted.
 * 
 * If as a result of the operation triggers are invoked that deletes
 * entities and no entity ids were provided by the application, the returned
 * array of identifiers may be incorrect. To avoid this problem, an application
 * can first call ecs_bulk_init to create empty entities, copy the array to one
 * that is owned by the application, and then use this array to populate the
 * entities.
 * 
 * @param world The world.
 * @param desc Bulk creation parameters.
 * @return Array with the list of entity ids created/populated.
 */
FLECS_API
const ecs_entity_t* ecs_bulk_init(
    ecs_world_t *world,
    const ecs_bulk_desc_t *desc);

/** Find or create a component. 
 * This operation creates a new component, or finds an existing one. The find or
 * create behavior is the same as ecs_entity_init.
 *
 * When an existing component is found, the size and alignment are verified with
 * the provided values. If the values do not match, the operation will fail.
 *
 * See the documentation of ecs_component_desc_t for more details. 
 *
 * @param world The world.
 * @param desc Component init parameters.
 * @return A handle to the new or existing component, or 0 if failed.
 */
FLECS_API
ecs_entity_t ecs_component_init(
    ecs_world_t *world,
    const ecs_component_desc_t *desc);

/** Create a new type entity. 
 * This operation creates a new type entity, or finds an existing one. The find 
 * or create behavior is the same as ecs_entity_init.
 *
 * A type entity is an entity with the EcsType component. This component
 * a pointer to an ecs_type_t, which allows for the creation of named types.
 * Named types are used in a few places, such as for pipelines and filter terms
 * with the EcsAndFrom or EcsOrFrom operators.
 *
 * When an existing type entity is found, its types are verified with the
 * provided values. If the values do not match, the operation will fail.
 *
 * See the documentation of ecs_type_desc_t for more details.
 *
 * @param world The world.
 * @param desc Type entity init parameters.
 * @return A handle to the new or existing type, or 0 if failed.
*/
FLECS_API
ecs_entity_t ecs_type_init(
    ecs_world_t *world,
    const ecs_type_desc_t *desc);    

/** Create N new entities.
 * This operation is the same as ecs_new_w_id, but creates N entities
 * instead of one and does not recycle ids.
 * 
 * @param world The world.
 * @param id The component id to create the entities with.
 * @param count The number of entities to create.
 * @return The first entity id of the newly created entities.
 */
FLECS_API
const ecs_entity_t* ecs_bulk_new_w_id(
    ecs_world_t *world,
    ecs_id_t id,
    int32_t count);

/** Clone an entity
 * This operation clones the components of one entity into another entity. If
 * no destination entity is provided, a new entity will be created. Component
 * values are not copied unless copy_value is true.
 *
 * @param world The world.
 * @param dst The entity to copy the components to.
 * @param src The entity to copy the components from.
 * @param copy_value If true, the value of components will be copied to dst.
 * @return The destination entity.
 */
FLECS_API
ecs_entity_t ecs_clone(
    ecs_world_t *world,
    ecs_entity_t dst,
    ecs_entity_t src,
    bool copy_value);

/** @} */

/**
 * @defgroup adding_removing Adding & Removing
 * @{
 */

/** Add an entity to an entity.
 * This operation adds a single entity to the type of an entity. Type roles may
 * be used in combination with the added entity. If the entity already has the
 * entity, this operation will have no side effects.
 *
 * @param world The world.
 * @param entity The entity.
 * @param id The id to add.
 */
FLECS_API
void ecs_add_id(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_id_t id);

/** Remove an entity from an entity.
 * This operation removes a single entity from the type of an entity. Type roles
 * may be used in combination with the added entity. If the entity does not have
 * the entity, this operation will have no side effects.
 *
 * @param world The world.
 * @param entity The entity.
 * @param id The id to remove.
 */
FLECS_API
void ecs_remove_id(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_id_t id);

/** @} */


/**
 * @defgroup enabling_disabling Enabling & Disabling components.
 * @{
 */

/** Enable or disable component.
 * Enabling or disabling a component does not add or remove a component from an
 * entity, but prevents it from being matched with queries. This operation can
 * be useful when a component must be temporarily disabled without destroying
 * its value. It is also a more performant operation for when an application
 * needs to add/remove components at high frequency, as enabling/disabling is
 * cheaper than a regular add or remove.
 *
 * @param world The world.
 * @param entity The entity.
 * @param id The component.
 * @param enable True to enable the component, false to disable.
 */
FLECS_API 
void ecs_enable_component_w_id(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_id_t id,
    bool enable);

/** Test if component is enabled.
 * Test whether a component is currently enabled or disabled. This operation
 * will return true when the entity has the component and if it has not been
 * disabled by ecs_enable_component.
 *
 * @param world The world.
 * @param entity The entity.
 * @param id The component.
 * @return True if the component is enabled, otherwise false.
 */
FLECS_API 
bool ecs_is_component_enabled_w_id(
    const ecs_world_t *world,
    ecs_entity_t entity,
    ecs_id_t id);

/** @} */


/**
 * @defgroup pairs Pairs
 * @{
 */
 
/** Make a pair identifier.
 * This function is equivalent to using the ecs_pair macro, and is added for
 * convenience to make it easier for non C/C++ bindings to work with pairs.
 *
 * @param relation The relation of the pair.
 * @param object The object of the pair.
 */
FLECS_API
ecs_id_t ecs_make_pair(
    ecs_entity_t relation,
    ecs_entity_t object);

/** @} */


/**
 * @defgroup deleting Deleting Entities and components
 * @{
 */

/** Clear all components.
 * This operation will clear all components from an entity but will not delete
 * the entity itself. This effectively prevents the entity id from being 
 * recycled.
 *
 * @param world The world.
 * @param entity The entity.
 */
FLECS_API
void ecs_clear(
    ecs_world_t *world,
    ecs_entity_t entity);

/** Delete an entity.
 * This operation will delete an entity and all of its components. The entity id
 * will be recycled. Repeatedly calling ecs_delete without ecs_new or
 * ecs_new_w_id will cause a memory leak as it will cause
 * the list with ids that can be recycled to grow unbounded.
 *
 * @param world The world.
 * @param entity The entity.
 */
FLECS_API
void ecs_delete(
    ecs_world_t *world,
    ecs_entity_t entity);

/** Delete all entities with the specified id.
 * This will delete all entities (tables) that have the specified id. The id 
 * may be a wildcard and/or a pair.
 * 
 * @param world The world.
 * @param id The id.
 */
FLECS_API
void ecs_delete_with(
    ecs_world_t *world,
    ecs_id_t id);

/** Remove all instances of the specified id.
 * This will remove the specified id from all entities (tables). Teh id may be
 * a wildcard and/or a pair.
 * 
 * @param world The world.
 * @param id The id.
 */
FLECS_API
void ecs_remove_all(
    ecs_world_t *world,
    ecs_id_t id);

/** @} */


/**
 * @defgroup getting Getting Components
 * @{
 */

/** Get an immutable pointer to a component.
 * This operation obtains a const pointer to the requested component. The
 * operation accepts the component entity id.
 *
 * @param world The world.
 * @param entity The entity.
 * @param id The id of the component to get.
 * @return The component pointer, NULL if the entity does not have the component.
 */
FLECS_API
const void* ecs_get_id(
    const ecs_world_t *world,
    ecs_entity_t entity,
    ecs_id_t id);

/** Create a ref.
 * A ref is a handle to an entity + component which caches a small amount of
 * data to reduce overhead of repeatedly accessing the component. Use 
 * ecs_ref_get to get the component data.
 *
 * @param world The world.
 * @param entity The entity.
 * @param id The id of the component.
 * @return The reference.
 */
FLECS_API
ecs_ref_t ecs_ref_init_id(
    const ecs_world_t *world,
    ecs_entity_t entity,
    ecs_id_t id);

/** Get component from ref.
 * Get component pointer from ref. The ref must be created with ecs_ref_init.
 * 
 * @param world The world.
 * @param ref The ref.
 * @param id The component id.
 * @return The component pointer, NULL if the entity does not have the component.
 */
FLECS_API
void* ecs_ref_get_id(
    const ecs_world_t *world,
    ecs_ref_t *ref,
    ecs_id_t id);

/** Get case for switch.
 * This operation gets the current case for the specified switch. If the current
 * switch is not set for the entity, the operation will return 0.
 *
 * @param world The world.
 * @param e The entity.
 * @param sw The switch for which to obtain the case.
 * @return The current case for the specified switch. 
 */
FLECS_API
ecs_entity_t ecs_get_case(
    const ecs_world_t *world,
    ecs_entity_t e,
    ecs_entity_t sw);

/** @} */


/**
 * @defgroup setting Setting Components
 * @{
 */

/** Get a mutable pointer to a component.
 * This operation is similar to ecs_get_id but it returns a mutable 
 * pointer. If this operation is invoked from inside a system, the entity will
 * be staged and a pointer to the staged component will be returned.
 *
 * If the entity did not yet have the component, the component will be added by
 * this operation. In this case the is_added out parameter will be set to true.
 *
 * @param world The world.
 * @param entity The entity.
 * @param id The entity id of the component to obtain.
 * @param is_added Out parameter that returns true if the component was added.
 * @return The component pointer.
 */
FLECS_API
void* ecs_get_mut_id(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_id_t id,
    bool *is_added); 

/** Emplace a component.
 * Emplace is similar to get_mut except that the component constructor is not
 * invoked for the returned pointer, allowing the component to be "constructed"
 * directly in the storage.
 *
 * Emplace can only be used if the entity does not yet have the component. If
 * the entity has the component, the operation will fail.
 *
 * @param world The world.
 * @param entity The entity.
 * @param id The component to obtain.
 * @return The (uninitialized) component pointer.
 */
FLECS_API
void* ecs_emplace_id(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_id_t id); 

/** Signal that a component has been modified.
 * This operation allows an application to signal to Flecs that a component has
 * been modified. As a result, OnSet systems will be invoked.
 *
 * This operation is commonly used together with ecs_get_mut.
 *
 * @param world The world.
 * @param entity The entity.
 * @param id The id of the component that was modified.
 */
FLECS_API 
void ecs_modified_id(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_id_t id);

/** Set the value of a component.
 * This operation allows an application to set the value of a component. The
 * operation is equivalent to calling ecs_get_mut and ecs_modified.
 *
 * If the provided entity is 0, a new entity will be created.
 *
 * @param world The world.
 * @param entity The entity.
 * @param id The id of the component to set.
 * @param size The size of the pointer to the value.
 * @param ptr The pointer to the value.
 * @return The entity. A new entity if no entity was provided.
 */
FLECS_API
ecs_entity_t ecs_set_id(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_id_t id,
    size_t size,
    const void *ptr);

/** @} */


/**
 * @defgroup metadata Entity Metadata
 * @{
 */

/** Test whether an entity is valid.
 * Entities that are valid can be used with API functions.
 *
 * An entity is valid if it is not 0 and if it is alive. If the provided id has
 * a role or a pair, the contents of the role or the pair will be checked for
 * validity.
 *
 * is_valid will return true for ids that don't exist (alive or not alive). This
 * allows for using ids that have never been created by ecs_new or similar. In
 * this the function differs from ecs_is_alive, which will return false for
 * entities that do not yet exist.
 *
 * The operation will return false for an id that exists and is not alive, as
 * using this id with an API operation would cause it to assert.
 *
 * @param world The world.
 * @param e The entity.
 * @return True if the entity is valid, false if the entity is not valid.
 */
FLECS_API
bool ecs_is_valid(
    const ecs_world_t *world,
    ecs_entity_t e);

/** Test whether an entity is alive.
 * An entity is alive when it has been returned by ecs_new (or similar) or if
 * it is not empty (componentts have been explicitly added to the id).
 *
 * @param world The world.
 * @param e The entity.
 * @return True if the entity is alive, false if the entity is not alive.
 */
FLECS_API
bool ecs_is_alive(
    const ecs_world_t *world,
    ecs_entity_t e);

/** Remove generation from entity id.
 *
 * @param e The entity id.
 * @return The entity id without the generation count.
 */
FLECS_API
ecs_id_t ecs_strip_generation(
    ecs_entity_t e);

/** Get alive identifier.
 * In some cases an application may need to work with identifiers from which
 * the generation has been stripped. A typical scenario in which this happens is
 * when iterating relationships in an entity type.
 *
 * For example, when obtaining the parent id from a ChildOf relation, the parent
 * (object part of the pair) will have been stored in a 32 bit value, which 
 * cannot store the entity generation. This function can retrieve the identifier
 * with the current generation for that id.
 *
 * If the provided identifier is not alive, the function will return 0.
 *
 * @param world The world.
 * @param e The for which to obtain the current alive entity id.
 * @return The alive entity id if there is one, or 0 if the id is not alive.
 */
FLECS_API
ecs_entity_t ecs_get_alive(
    const ecs_world_t *world,
    ecs_entity_t e);

/** Ensure id is alive.
 * This operation ensures that the provided id is alive. This is useful in
 * scenarios where an application has an existing id that has not been created
 * with ecs_new (such as a global constant or an id from a remote application).
 * 
 * When this operation is successful it guarantees that the provided id exists, 
 * is valid and is alive.
 *
 * Before this operation the id must either not be alive or have a generation
 * that is equal to the passed in entity.
 *
 * If the provided id has a non-zero generation count and the id does not exist
 * in the world, the id will be created with the specified generation.
 * 
 * If the provided id is alive and has a generation count that does not match
 * the provided id, the operation will fail.
 *
 * @param world The world.
 * @param entity The entity id to make alive.
 */
FLECS_API
void ecs_ensure(
    ecs_world_t *world,
    ecs_entity_t entity);

/** Same as ecs_ensure, but for (component) ids.
 * An id can be an entity or pair, and can contain type flags. This operation
 * ensures that the entity (or entities, for a pair) are alive.
 * 
 * When this operation is successful it guarantees that the provided id can be
 * used in operations that accept an id.
 * 
 * Since entities in a pair do not encode their generation ids, this operation
 * will not fail when an entity with non-zero generation count already exists in
 * the world. 
 * 
 * This is different from ecs_ensure, which will fail if attempted with an id
 * that has generation 0 and an entity with a non-zero generation is currently 
 * alive.
 * 
 * @param world The world.
 * @param id The id to make alive.
 */
FLECS_API
void ecs_ensure_id(
    ecs_world_t *world,
    ecs_id_t id);

/** Test whether an entity exists.
 * Similar as ecs_is_alive, but ignores entity generation count.
 *
 * @param world The world.
 * @param entity The entity.
 * @return True if the entity exists, false if the entity does not exist.
 */
FLECS_API
bool ecs_exists(
    const ecs_world_t *world,
    ecs_entity_t entity);

/** Get the type of an entity.
 *
 * @param world The world.
 * @param entity The entity.
 * @return The type of the entity, NULL if the entity has no components.
 */
FLECS_API
const ecs_type_t* ecs_get_type(
    const ecs_world_t *world,
    ecs_entity_t entity);

/** Get the table of an entity.
 *
 * @param world The world.
 * @param entity The entity.
 * @return The table of the entity, NULL if the entity has no components/tags.
 */
FLECS_API
ecs_table_t* ecs_get_table(
    const ecs_world_t *world,
    ecs_entity_t entity);

/** Get the storage table of an entity.
 * The storage table of an entity has a type that only contains components, and
 * excludes tags/entities/pairs that have no data.
 *
 * @param world The world.
 * @param entity The entity.
 * @return The storage table of the entity, NULL if the entity has no components.
 */
FLECS_API
ecs_table_t* ecs_get_storage_table(
    const ecs_world_t *world,
    ecs_entity_t entity);

/** Get the type for an id.
 * This function returnsthe type information for an id. The specified id can be
 * any valid id. For the rules on how type information is determined based on
 * id, see ecs_get_typeid.
 * 
 * @param world The world.
 * @param id The id.
 * @return The type information of the id.
 */
FLECS_API
const ecs_type_info_t* ecs_get_type_info(
    const ecs_world_t *world,
    ecs_id_t id);

/** Get the type for an id.
 * This operation returns the component id for an id, if the id is associated
 * with a type. For a regular component with a non-zero size (an entity with the
 * EcsComponent component) the operation will return the entity itself.
 * 
 * For an entity that does not have the EcsComponent component, or with an
 * EcsComponent value with size 0, the operation will return 0.
 * 
 * For a pair id the operation will return the type associated with the pair, by
 * applying the following rules in order:
 * - The relation entity is returned if it is a component
 * - 0 is returned if the relation entity has the Tag property
 * - The object entity is returned if it is a component
 * - 0 is returned.
 *
 * @param world The world.
 * @param id The id.
 * @return The type id of the id.
 */
FLECS_API
ecs_entity_t ecs_get_typeid(
    const ecs_world_t *world,
    ecs_id_t id);

/** Returns whether specified id a tag.
 * This operation returns whether the specified type is a tag (a component 
 * without data/size).
 * 
 * An id is a tag when:
 * - it is an entity without the EcsComponent component
 * - it has an EcsComponent with size member set to 0
 * - it is a pair where both elements are a tag
 * - it is a pair where the first element has the EcsTag tag
 * 
 * @param world The world.
 * @param id The id.
 * @return Whether the provided id is a tag.
 */
FLECS_API
ecs_entity_t ecs_id_is_tag(
    const ecs_world_t *world,
    ecs_id_t id);

/** Returns whether specified id is in use.
 * This operation returns whether an id is in use in the world. An id is in use
 * if it has been added to one or more tables.
 * 
 * @param world The world.
 * @param id The id.
 * @return Whether the id is in use.
 */
FLECS_API
bool ecs_id_in_use(
    ecs_world_t *world,
    ecs_id_t id);

/** Get the name of an entity.
 * This will return the name stored in (EcsIdentifier, EcsName).
 *
 * @param world The world.
 * @param entity The entity.
 * @return The type of the entity, NULL if the entity has no name.
 */
FLECS_API
const char* ecs_get_name(
    const ecs_world_t *world,
    ecs_entity_t entity);

/** Get the symbol of an entity.
 * This will return the symbol stored in (EcsIdentifier, EcsSymbol).
 *
 * @param world The world.
 * @param entity The entity.
 * @return The type of the entity, NULL if the entity has no name.
 */
FLECS_API
const char* ecs_get_symbol(
    const ecs_world_t *world,
    ecs_entity_t entity);

/** Set the name of an entity.
 * This will set or overwrite the name of an entity. If no entity is provided,
 * a new entity will be created.
 *
 * The name is stored in (EcsIdentifier, EcsName).
 *
 * @param world The world.
 * @param entity The entity.
 * @param name The name.
 * @return The provided entity, or a new entity if 0 was provided.
 */
FLECS_API
ecs_entity_t ecs_set_name(
    ecs_world_t *world,
    ecs_entity_t entity,
    const char *name);

/** Set the symbol of an entity.
 * This will set or overwrite the symbol of an entity. If no entity is provided,
 * a new entity will be created.
 *
 * The symbol is stored in (EcsIdentifier, EcsSymbol).
 *
 * @param world The world.
 * @param entity The entity.
 * @param symbol The symbol.
 * @return The provided entity, or a new entity if 0 was provided.
 */
FLECS_API
ecs_entity_t ecs_set_symbol(
    ecs_world_t *world,
    ecs_entity_t entity,
    const char *symbol);

/** Set alias for entity. 
 * An entity can be looked up using its alias from the root scope without 
 * providing the fully qualified name if its parent. An entity can only have
 * a single alias.
 * 
 * The symbol is stored in (EcsIdentifier, EcsAlias).
 * 
 * @param world The world.
 * @param entity The entity.
 * @param alias The alias.
 */
FLECS_API
void ecs_set_alias(
    ecs_world_t *world,
    ecs_entity_t entity,
    const char *alias);

/** Convert role to string.
 * This operation converts a role to a string.
 * 
 * @param role The role id.
 * @return The role string, or NULL if no valid role is provided.
 */
FLECS_API
const char* ecs_role_str(
    ecs_id_t role);

/** Convert id to string.
 * This operation interprets the structure of an id and converts it to a string.
 *
 * @param world The world.
 * @param id The id to convert to a string.
 * @return The id converted to a string.
 */
FLECS_API
char* ecs_id_str(
    const ecs_world_t *world,
    ecs_id_t id);

/** Write id string to buffer.
 * Same as ecs_id_str but writes result to ecs_strbuf_t.
 *
 * @param world The world.
 * @param id The id to convert to a string.
 * @param buf The buffer to write to.
 */
FLECS_API
void ecs_id_str_buf(
    const ecs_world_t *world,
    ecs_id_t id,
    ecs_strbuf_t *buf);

/** Convert type to string.
 * The result of this operation must be freed with ecs_os_free.
 * 
 * @param world The world.
 * @param type The type.
 * @return The stringified type.
 */
FLECS_API
char* ecs_type_str(
    const ecs_world_t *world,
    const ecs_type_t* type);

/** Convert table to string.
 * Same as ecs_type_str(world, ecs_table_get_type(table)). The result of this
 * operation must be freed with ecs_os_free.
 *
 * @param world The world.
 * @param table The table.
 * @return The stringified table type.
 */
FLECS_API
char* ecs_table_str(
    const ecs_world_t *world,
    ecs_table_t *table);

/** Test if an entity has an entity.
 * This operation returns true if the entity has the provided entity in its 
 * type.
 *
 * @param world The world.
 * @param entity The entity.
 * @param id The id to test for.
 * @return True if the entity has the entity, false if not.
 */
FLECS_API
bool ecs_has_id(
    const ecs_world_t *world,
    ecs_entity_t entity,
    ecs_id_t id);

/** Get the object of a relation.
 * This will return a object of the entity for the specified relation. The index
 * allows for iterating through the objects, if a single entity has multiple
 * objects for the same relation.
 *
 * If the index is larger than the total number of instances the entity has for
 * the relation, the operation will return 0.
 *
 * @param world The world.
 * @param entity The entity.
 * @param rel The relation between the entity and the object.
 * @param index The index of the relation instance.
 * @return The object for the relation at the specified index.
 */
FLECS_API
ecs_entity_t ecs_get_object(
    const ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_t rel,
    int32_t index);

/** Get the object of a relation for a given id.
 * This operation returns the first entity that has the provided id by following
 * the specified relationship. If the entity itself has the id then entity will
 * be returned. If the id cannot be found on the entity or by following the
 * relation, the operation will return 0.
 * 
 * This operation can be used to lookup, for example, which prefab is providing
 * a component by specifying the IsA relation:
 * 
 *   // Is Position provided by the entity or one of its base entities?
 *   ecs_get_object_for_id(world, entity, EcsIsA, ecs_id(Position))
 * 
 * @param world The world.
 * @param entity The entity.
 * @param rel The relationship to follow.
 * @param id The id to lookup.
 * @return The entity for which the object has been found.
 */
FLECS_API
ecs_entity_t ecs_get_object_for_id(
    const ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_t rel,
    ecs_id_t id);

/** Enable or disable an entity.
 * This operation enables or disables an entity by adding or removing the
 * EcsDisabled tag. A disabled entity will not be matched with any systems,
 * unless the system explicitly specifies the EcsDisabled tag.
 *
 * @param world The world.
 * @param entity The entity to enable or disable.
 * @param enabled true to enable the entity, false to disable.
 */
FLECS_API
void ecs_enable(
    ecs_world_t *world,
    ecs_entity_t entity,
    bool enabled);

/** Count entities that have the specified id.
 * Returns the number of entities that have the specified id.
 *
 * @param world The world.
 * @param entity The id to search for.
 * @return The number of entities that have the id.
 */
FLECS_API
int32_t ecs_count_id(
    const ecs_world_t *world,
    ecs_id_t entity);

/** @} */


/**
 * @defgroup lookup Lookups
 * @{
 */

/** Lookup an entity by name.
 * Returns an entity that matches the specified name. Only looks for entities in
 * the current scope (root if no scope is provided).
 *
 * @param world The world.
 * @param name The entity name.
 * @return The entity with the specified name, or 0 if no entity was found.
 */
FLECS_API
ecs_entity_t ecs_lookup(
    const ecs_world_t *world,
    const char *name);

/** Lookup a child entity by name.
 * Returns an entity that matches the specified name. Only looks for entities in
 * the provided parent. If no parent is provided, look in the current scope (
 * root if no scope is provided).
 *
 * @param world The world.
 * @param name The entity name.
 * @return The entity with the specified name, or 0 if no entity was found.
 */
FLECS_API
ecs_entity_t ecs_lookup_child(
    const ecs_world_t *world,
    ecs_entity_t parent,
    const char *name);

/** Lookup an entity from a path.
 * Lookup an entity from a provided path, relative to the provided parent. The
 * operation will use the provided separator to tokenize the path expression. If
 * the provided path contains the prefix, the search will start from the root.
 *
 * If the entity is not found in the provided parent, the operation will 
 * continue to search in the parent of the parent, until the root is reached. If
 * the entity is still not found, the lookup will search in the flecs.core
 * scope. If the entity is not found there either, the function returns 0.
 *
 * @param world The world.
 * @param parent The entity from which to resolve the path.
 * @param path The path to resolve.
 * @param sep The path separator.
 * @param prefix The path prefix.
 * @param recursive Recursively traverse up the tree until entity is found.
 * @return The entity if found, else 0.
 */
FLECS_API
ecs_entity_t ecs_lookup_path_w_sep(
    const ecs_world_t *world,
    ecs_entity_t parent,
    const char *path,
    const char *sep,
    const char *prefix,
    bool recursive);

/** Lookup an entity by its symbol name.
 * This looks up an entity by symbol stored in (EcsIdentifier, EcsSymbol). The
 * operation does not take into account hierarchies.
 *
 * This operation can be useful to resolve, for example, a type by its C 
 * identifier, which does not include the Flecs namespacing.
 */
FLECS_API
ecs_entity_t ecs_lookup_symbol(
    const ecs_world_t *world,
    const char *symbol,
    bool lookup_as_path);

/** @} */


/**
 * @defgroup paths Paths
 * @{
 */

/** Get a path identifier for an entity.
 * This operation creates a path that contains the names of the entities from
 * the specified parent to the provided entity, separated by the provided 
 * separator. If no parent is provided the path will be relative to the root. If
 * a prefix is provided, the path will be prefixed by the prefix.
 *
 * If the parent is equal to the provided child, the operation will return an
 * empty string. If a nonzero component is provided, the path will be created by 
 * looking for parents with that component.
 *
 * The returned path should be freed by the application.
 *
 * @param world The world.
 * @param parent The entity from which to create the path.
 * @param child The entity to which to create the path.
 * @param sep The separator to use between path elements.
 * @param prefix The initial character to use for root elements.
 * @return The relative entity path.
 */
FLECS_API
char* ecs_get_path_w_sep(
    const ecs_world_t *world,
    ecs_entity_t parent,
    ecs_entity_t child,
    const char *sep,
    const char *prefix);

/** Write path identifier to buffer.
 * Same as ecs_get_path_w_sep, but writes result to an ecs_strbuf_t.
 * 
 * @param world The world.
 * @param parent The entity from which to create the path.
 * @param child The entity to which to create the path.
 * @param sep The separator to use between path elements.
 * @param prefix The initial character to use for root elements.
 * @param buf The buffer to write to.
 */
void ecs_get_path_w_sep_buf(
    const ecs_world_t *world,
    ecs_entity_t parent,
    ecs_entity_t child,
    const char *sep,
    const char *prefix,
    ecs_strbuf_t *buf);

/** Find or create entity from path.
 * This operation will find or create an entity from a path, and will create any
 * intermediate entities if required. If the entity already exists, no entities
 * will be created.
 *
 * If the path starts with the prefix, then the entity will be created from the
 * root scope.
 *
 * @param world The world.
 * @param parent The entity relative to which the entity should be created.
 * @param path The path to create the entity for.
 * @param sep The separator used in the path.
 * @param prefix The prefix used in the path.
 * @return The entity.
 */
FLECS_API
ecs_entity_t ecs_new_from_path_w_sep(
    ecs_world_t *world,
    ecs_entity_t parent,
    const char *path,
    const char *sep,
    const char *prefix);

/** Add specified path to entity.
 * This operation is similar to ecs_new_from_path, but will instead add the path
 * to an existing entity.
 *
 * If an entity already exists for the path, it will be returned instead.
 *
 * @param world The world.
 * @param entity The entity to which to add the path.
 * @param parent The entity relative to which the entity should be created.
 * @param path The path to create the entity for.
 * @param sep The separator used in the path.
 * @param prefix The prefix used in the path.
 * @return The entity.
 */ 
FLECS_API
ecs_entity_t ecs_add_path_w_sep(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_t parent,
    const char *path,
    const char *sep,
    const char *prefix);

/** @} */


/**
 * @defgroup scopes Scopes
 * @{
 */

/** Set the current scope.
 * This operation sets the scope of the current stage to the provided entity.
 * As a result new entities will be created in this scope, and lookups will be
 * relative to the provided scope.
 *
 * It is considered good practice to restore the scope to the old value.
 *
 * @param world The world.
 * @param scope The entity to use as scope.
 * @return The previous scope.
 */
FLECS_API
ecs_entity_t ecs_set_scope(
    ecs_world_t *world,
    ecs_entity_t scope);

/** Get the current scope.
 * Get the scope set by ecs_set_scope. If no scope is set, this operation will
 * return 0.
 *
 * @param world The world.
 * @return The current scope.
 */
FLECS_API
ecs_entity_t ecs_get_scope(
    const ecs_world_t *world);

/** Set current with id.
 * New entities are automatically created with the specified id.
 *
 * @param world The world.
 * @param id The id.
 * @return The previous id.
 */
FLECS_API
ecs_entity_t ecs_set_with(
    ecs_world_t *world,
    ecs_id_t id);

/** Get current with id.
 * Get the id set with ecs_set_with.
 *
 * @param world The world.
 * @return The last id provided to ecs_set_with.
 */
FLECS_API
ecs_id_t ecs_get_with(
    const ecs_world_t *world);

/** Set a name prefix for newly created entities.
 * This is a utility that lets C modules use prefixed names for C types and
 * C functions, while using names for the entity names that do not have the 
 * prefix. The name prefix is currently only used by ECS_COMPONENT.
 *
 * @param world The world.
 * @param prefix The name prefix to use.
 * @return The previous prefix.
 */
FLECS_API
const char* ecs_set_name_prefix(
    ecs_world_t *world,
    const char *prefix);    

/** Set search path for lookup operations.
 * This operation accepts an array of entity ids that will be used as search
 * scopes by lookup operations. The operation returns the current search path.
 * It is good practice to restore the old search path.
 * 
 * The search path will be evaluated starting from the last element.
 * 
 * The default search path includes flecs.core. When a custom search path is
 * provided it overwrites the existing search path. Operations that rely on
 * looking up names from flecs.core without providing the namespace may fail if
 * the custom search path does not include flecs.core (EcsFlecsCore).
 * 
 * The search path array is not copied into managed memory. The application must
 * ensure that the provided array is valid for as long as it is used as the
 * search path.
 * 
 * The provided array must be terminated with a 0 element. This enables an
 * application to push/pop elements to an existing array without invoking the
 * ecs_set_lookup_path operation again.
 * 
 * @param world The world.
 * @param lookup_path 0-terminated array with entity ids for the lookup path.
 * @return Current lookup path array.
 */
FLECS_API
ecs_entity_t* ecs_set_lookup_path(
    ecs_world_t *world,
    const ecs_entity_t *lookup_path);

/** Get current lookup path.
 * Returns value set by ecs_set_lookup_path.
 * 
 * @param world The world.
 * @return The current lookup path.
 */
FLECS_API
ecs_entity_t* ecs_get_lookup_path(
    const ecs_world_t *world);

/** @} */


/**
 * @defgroup terms Terms
 * @{
 */

/** Iterator for a single (component) id.
 * A term iterator returns all entities (tables) that match a single (component)
 * id. The search for the matching set of entities (tables) is performed in 
 * constant time.
 *
 * Currently only trivial terms are supported (see ecs_term_is_trivial). Only
 * the id field of the term needs to be initialized.
 *
 * @param world The world.
 * @param term The term.
 * @return The iterator.
 */
FLECS_API
ecs_iter_t ecs_term_iter(
    const ecs_world_t *world,
    ecs_term_t *term);

/** Return a chained term iterator.
 * A chained iterator applies a filter to the results of the input iterator. The
 * resulting iterator must be iterated with ecs_term_next.
 * 
 * @param it The input iterator
 * @param term The term filter to apply to the iterator.
 * @return The chained iterator. 
 */
FLECS_API
ecs_iter_t ecs_term_chain_iter(
    const ecs_iter_t *it,
    ecs_term_t *term);

/** Progress the term iterator.
 * This operation progresses the term iterator to the next table. The 
 * iterator must have been initialized with `ecs_term_iter`. This operation 
 * must be invoked at least once before interpreting the contents of the 
 * iterator.
 *
 * @param it The iterator.
 * @returns True if more data is available, false if not.
 */
FLECS_API
bool ecs_term_next(
    ecs_iter_t *it);

/** Test whether term id is set.
 *
 * @param id The term id.
 * @return True when set, false when not set.
 */
FLECS_API 
bool ecs_term_id_is_set(
    const ecs_term_id_t *id);

/** Test whether a term is set.
 * This operation can be used to test whether a term has been initialized with
 * values or whether it is empty.
 *
 * An application generally does not need to invoke this operation. It is useful
 * when initializing a 0-initialized array of terms (like in ecs_term_desc_t) as
 * this operation can be used to find the last initialized element.
 *
 * @param term The term.
 * @return True when set, false when not set.
 */
FLECS_API
bool ecs_term_is_initialized(
    const ecs_term_t *term);

/** Test whether a term is a trivial term.
 * A trivial term is a term that only contains a type id. Trivial terms must not
 * have read/write annotations, relation substitutions and subjects other than
 * 'This'. Examples of trivial terms are:
 * - 'Position'
 * - 'Position(This)'
 * - '(Likes, IceCream)'
 * - 'Likes(This, IceCream)'
 * 
 * Examples of non-trivial terms are:
 * - '[in] Position'
 * - 'Position(MyEntity)'
 * - 'Position(self|superset)'
 *
 * Trivial terms are useful in expressions that should just represent a list of
 * components, such as when parsing the list of components to add to an entity.
 *
 * The term passed to this operation must be finalized. Terms returned by the
 * parser are guaranteed to be finalized.
 *
 * @param term The term.
 * @return True if term is trivial, false if it is not.
 */
FLECS_API
bool ecs_term_is_trivial(
    const ecs_term_t *term);

/** Finalize term.
 * Ensure that all fields of a term are consistent and filled out. This 
 * operation should be invoked before using and after assigning members to, or 
 * parsing a term. When a term contains unresolved identifiers, this operation
 * will resolve and assign the identifiers. If the term contains any identifiers
 * that cannot be resolved, the operation will fail.
 *
 * An application generally does not need to invoke this operation as the APIs
 * that use terms (such as filters, queries and triggers) will finalize terms
 * when they are created.
 *
 * The name and expr parameters are optional, and only used for giving more 
 * descriptive error messages.
 *
 * @param world The world.
 * @param name The name of the entity that uses the term (such as a system).
 * @param term The term to finalize.
 * @return Zero if success, nonzero if an error occurred.
 */
FLECS_API 
int ecs_term_finalize(
    const ecs_world_t *world,
    const char *name,
    ecs_term_t *term);

/** Copy resources of a term to another term.
 * This operation copies one term to another term. If the source term contains
 * allocated resources (such as identifiers), they will be duplicated so that
 * no memory is shared between the terms.
 *
 * @param src The term to copy from.
 * @return The destination term.
 */
FLECS_API 
ecs_term_t ecs_term_copy(
    const ecs_term_t *src);

/** Move resources of a term to another term.
 * Same as copy, but moves resources from src, if src->move is set to true. If
 * src->move is not set to true, this operation will do a copy.
 *
 * The conditional move reduces redundant allocations in scenarios where a list 
 * of terms is partially created with allocated resources.
 *
 * @param src The term to move from.
 * @return The destination term.
 */
FLECS_API 
ecs_term_t ecs_term_move(
    ecs_term_t *src);    

/** Free resources of term.
 * This operation frees all resources (such as identifiers) of a term. The term
 * object itself is not freed.
 *
 * @param term The term to free.
 */
FLECS_API
void ecs_term_fini(
    ecs_term_t *term);

/** Utility to match an id with a pattern.
 * This operation returns true if the provided pattern matches the provided
 * id. The pattern may contain a wildcard (or wildcards, when a pair).
 *
 * @param id The id.
 * @param pattern The pattern to compare with.
 */
FLECS_API
bool ecs_id_match(
    ecs_id_t id,
    ecs_id_t pattern);

/** Utility to check if id is a pair.
 *
 * @param id The id.
 * @return True if id is a pair.
 */
FLECS_API
bool ecs_id_is_pair(
    ecs_id_t id);

/** Utility to check if id is a wildcard.
 *
 * @param id The id.
 * @return True if id is a wildcard or a pair containing a wildcard.
 */
FLECS_API
bool ecs_id_is_wildcard(
    ecs_id_t id);

/** Utility to check if id is valid.
 * A valid id is an id that can be added to an entity. Invalid ids are:
 * - ids that contain wildcards
 * - ids that contain invalid entities
 * - ids that are 0 or contain 0 entities
 *
 * Note that the same rules apply to removing from an entity, with the exception
 * of wildcards.
 *
 * @param world The world.
 * @param id The id.
 * @return True if the id is valid.
 */
FLECS_API
bool ecs_id_is_valid(
    const ecs_world_t *world,
    ecs_id_t id);

/** @} */


/**
 * @defgroup filters Filters
 * @{
 */

/** Initialize filter 
 * A filter is a lightweight object that can be used to query for entities in
 * a world. Filters, as opposed to queries, do not cache results. They are 
 * therefore slower to iterate, but are faster to create.
 *
 * This operation will allocate an array to hold filter terms if the number of
 * terms in the filter exceed ECS_TERM_DESC_CACHE_SIZE. If the number of terms
 * is below that constant, the "terms" pointer is set to an inline array.
 * 
 * When a filter is copied by value, make sure to use "ecs_filter_move" to 
 * ensure that the terms pointer still points to the inline array:
 * 
 *   ecs_filter_move(&dst_filter, &src_filter)
 * 
 * Alternatively, the ecs_filter_move function can be called with both arguments
 * set to the same filter, to ensure the pointer is valid:
 * 
 *   ecs_filter_move(&f, &f)
 * 
 * When a filter contains entity or variable names memory is allocated to store
 * those. To cleanup memory associated with a filter, call ecs_filter_fini.
 *
 * It is possible to create a filter without allocating any memory, by setting
 * the "terms" and "term_count" members directly. When doing so an application
 * should not call ecs_filter_init but ecs_filter_finalize. This will ensure
 * that all fields are consistent and properly filled out.
 *
 * @param world The world.
 * @param desc Properties for the filter to create.
 * @param filter_out The filter.
 * @return Zero if successful, non-zero if failed.
 */
FLECS_API
int ecs_filter_init(
    const ecs_world_t *world,
    ecs_filter_t *filter_out,
    const ecs_filter_desc_t *desc);

/** Deinitialize filter.
 * Free resources associated with filter.
 *
 * @param filter The filter to deinitialize.
 */
FLECS_API
void ecs_filter_fini(
    ecs_filter_t *filter); 

/** Finalize filter.
 * When manually assigning an array of terms to the filter struct (so not when
 * using ecs_filter_init), this operation should be used to ensure that all 
 * terms are assigned properly and all (derived) fields have been set.
 *
 * When ecs_filter_init is used to create the filter, this function should not
 * be called. The purpose of this operation is to support creation of filters
 * without allocating memory.
 *
 * @param filter The filter to finalize.
 * @return Zero if filter is valid, non-zero if it contains errors.
 * @
 */
FLECS_API 
int ecs_filter_finalize(
    const ecs_world_t *world,
    ecs_filter_t *filter); 

/** Find index for This variable.
 * This operation looks up the index of the This variable. This index can
 * be used in operations like ecs_iter_set_var and ecs_iter_get_var.
 * 
 * The operation will return -1 if the variable was not found. This happens when
 * a filter only has terms that are not matched on the This variable, like a
 * filter that exclusively matches singleton components.
 * 
 * @param filter The rule.
 * @return The index of the This variable.
 */
FLECS_API
int32_t ecs_filter_find_this_var(
    const ecs_filter_t *filter);

/** Convert ter, to string expression.
 * Convert term to a string expression. The resulting expression is equivalent
 * to the same term, with the exception of And & Or operators.
 */
FLECS_API
char* ecs_term_str(
    const ecs_world_t *world,
    const ecs_term_t *term);

/** Convert filter to string expression.
 * Convert filter terms to a string expression. The resulting expression can be
 * parsed to create the same filter.
 */
FLECS_API 
char* ecs_filter_str(
    const ecs_world_t *world,
    const ecs_filter_t *filter); 

/** Return a filter iterator.
 * A filter iterator lets an application iterate over entities that match the
 * specified filter. If NULL is provided for the filter, the iterator will
 * iterate all tables in the world.
 * 
 * @param world The world.
 * @param filter The filter.
 * @return An iterator that can be used with ecs_filter_next.
 */
FLECS_API
ecs_iter_t ecs_filter_iter(
    const ecs_world_t *world,
    const ecs_filter_t *filter);  

/** Return a chained filter iterator.
 * A chained iterator applies a filter to the results of the input iterator. The
 * resulting iterator must be iterated with ecs_filter_next.
 * 
 * @param it The input iterator
 * @param filter The filter to apply to the iterator.
 * @return The chained iterator. 
 */
FLECS_API
ecs_iter_t ecs_filter_chain_iter(
    const ecs_iter_t *it,
    const ecs_filter_t *filter);

/** Get pivot term for filter.
 * The pivot term is the term that matches the smallest set of tables, and is
 * a good default starting point for a search.
 * 
 * The following conditions must be met for a term to be considered as pivot:
 * - It must have a This subject
 * - It must have the And operator
 * 
 * When a filter does not have any terms that match those conditions, it will
 * return -1.
 * 
 * If one or more terms in the filter have no matching tables the filter won't
 * yield any results. In this case the operation will return -2 which gives a
 * search function the option to early out.
 * 
 * @param world The world.
 * @param filter The filter.
 * @return Index of the pivot term (use with filter->terms)
 */
FLECS_API
int32_t ecs_filter_pivot_term(
    const ecs_world_t *world,
    const ecs_filter_t *filter);

/** Iterate tables matched by filter.
 * This operation progresses the filter iterator to the next table. The 
 * iterator must have been initialized with `ecs_filter_iter`. This operation 
 * must be invoked at least once before interpreting the contents of the 
 * iterator.
 *
 * @param it The iterator
 * @return True if more data is available, false if not.
 */
FLECS_API
bool ecs_filter_next(
    ecs_iter_t *it);

/** Same as ecs_filter_next, but always instanced.
 * See instanced property of ecs_filter_desc_t.
 */
FLECS_API
bool ecs_filter_next_instanced(
    ecs_iter_t *it);

/** Move resources of one filter to another. */
FLECS_API
void ecs_filter_move(
    ecs_filter_t *dst,
    ecs_filter_t *src);

/** Copy resources of one filter to another. */
FLECS_API
void ecs_filter_copy(
    ecs_filter_t *dst,
    const ecs_filter_t *src);

/** @} */

/**
 * @defgroup queries Queries
 * @{
 */

/** Create a query.
 * This operation creates a query. Queries are used to iterate over entities
 * that match a filter and are the fastest way to find and iterate over entities
 * and their components.
 * 
 * Queries should be created once, and reused multiple times. While iterating a
 * query is a cheap operation, creating and deleting a query is expensive. The
 * reason for this is that queries are "prematched", which means that a query
 * stores state about which entities (or rather, tables) match with the query.
 * Building up this state happens during query creation.
 *
 * Once a query is created, matching only happens when new tables are created.
 * In most applications this is an infrequent process, since it only occurs when
 * a new combination of components is introduced. While matching is expensive,
 * it is importent to note that matching does not happen on a per-entity basis,
 * but on a per-table basis. This means that the average time spent on matching
 * per frame should rapidly approach zero over the lifetime of an application.
 *
 * A query provides direct access to the component arrays. When an application
 * creates/deletes entities or adds/removes components, these arrays can shift
 * component values around, or may grow in size. This can cause unexpected or
 * undefined behavior to occur if these operations are performed while 
 * iterating. To prevent this from happening an application should either not
 * perform these operations while iterating, or use deferred operations (see
 * ecs_defer_begin and ecs_defer_end).
 *
 * Queries can be created and deleted dynamically. If a query was not deleted
 * (using ecs_query_fini) before the world is deleted, it will be deleted 
 * automatically.
 *
 * @param world The world.
 * @param desc A structure describing the query properties.
 * @return The new query.
 */
FLECS_API
ecs_query_t* ecs_query_init(
    ecs_world_t *world, 
    const ecs_query_desc_t *desc);

/** Destroy a query.
 * This operation destroys a query and its resources. If the query is used as
 * the parent of subqueries, those subqueries will be orphaned and must be
 * deinitialized as well.
 *
 * @param query The query.
 */
FLECS_API
void ecs_query_fini(
    ecs_query_t *query);

/** Get filter object of query.
 * This operation obtains a pointer to the internally constructed filter object
 * of the query and can be used to introspect the query terms.
 *
 * @param query The query.
 */
FLECS_API
const ecs_filter_t* ecs_query_get_filter(
    ecs_query_t *query);    

/** Return a query iterator.
 * A query iterator lets an application iterate over entities that match the
 * specified query. If a sorting function is specified, the query will check
 * whether a resort is required upon creating the iterator.
 *
 * Creating a query iterator is a cheap operation that does not allocate any
 * resources. An application does not need to deinitialize or free a query 
 * iterator before it goes out of scope.
 *
 * To iterate the iterator, an application should use ecs_query_next to progress
 * the iterator and test if it has data.
 *
 * Query iteration requires an outer and an inner loop. The outer loop uses
 * ecs_query_next to test if new tables are available. The inner loop iterates
 * the entities in the table, and is usually a for loop that uses iter.count to
 * loop through the entities and component arrays.
 *
 * The two loops are necessary because of how data is stored internally. 
 * Entities are grouped by the components they have, in tables. A single query 
 * can (and often does) match with multiple tables. Because each table has its
 * own set of arrays, an application has to reobtain pointers to those arrays
 * for each matching table.
 *
 * @param world The world or stage, when iterating in readonly mode.
 * @param query The query to iterate.
 * @return The query iterator.
 */
FLECS_API
ecs_iter_t ecs_query_iter(
    const ecs_world_t *world,
    ecs_query_t *query);  

/** Progress the query iterator.
 * This operation progresses the query iterator to the next table. The 
 * iterator must have been initialized with `ecs_query_iter`. This operation 
 * must be invoked at least once before interpreting the contents of the 
 * iterator.
 *
 * @param iter The iterator.
 * @returns True if more data is available, false if not.
 */
FLECS_API
bool ecs_query_next(
    ecs_iter_t *iter);

/** Same as ecs_query_next, but always instanced.
 * See "instanced" property of ecs_filter_desc_t.
 */
FLECS_API
bool ecs_query_next_instanced(
    ecs_iter_t *iter);

/** Returns whether the query data changed since the last iteration.
 * The operation will return true after:
 * - new entities have been matched with
 * - new tables have been matched/unmatched with
 * - matched entities were deleted
 * - matched components were changed
 * 
 * The operation will not return true after a write-only (EcsOut) or filter
 * (EcsInOutFilter) term has changed, when a term is not matched with the
 * current table (This subject) or for tag terms.
 * 
 * The changed state of a table is reset after it is iterated. If a iterator was
 * not iterated until completion, tables may still be marked as changed.
 * 
 * If no iterator is provided the operation will return the changed state of the
 * all matched tables of the query. 
 * 
 * If an iterator is provided, the operation will return the changed state of 
 * the currently returned iterator result. The following preconditions must be
 * met before using an iterator with change detection:
 * 
 * - The iterator is a query iterator (created with ecs_query_iter)
 * - The iterator must be valid (ecs_query_next must have returned true)
 * - The iterator must be instanced
 * 
 * @param query The query (optional if 'it' is provided).
 * @param it The iterator result to test (optional if 'query' is provided).
 * @return true if entities changed, otherwise false.
 */
FLECS_API
bool ecs_query_changed(
    ecs_query_t *query,
    const ecs_iter_t *it);

/** Skip a table while iterating.
 * This operation lets the query iterator know that a table was skipped while
 * iterating. A skipped table will not reset its changed state, and the query
 * will not update the dirty flags of the table for its out columns.
 * 
 * Only valid iterators must be provided (next has to be called at least once &
 * return true) and the iterator must be a query iterator.
 * 
 * @param it The iterator result to skip.
 */
FLECS_API
void ecs_query_skip(
    ecs_iter_t *it);

/** Returns whether query is orphaned.
 * When the parent query of a subquery is deleted, it is left in an orphaned
 * state. The only valid operation on an orphaned query is deleting it. Only
 * subqueries can be orphaned.
 *
 * @param query The query.
 * @return true if query is orphaned, otherwise false.
 */
FLECS_API
bool ecs_query_orphaned(
    ecs_query_t *query);

/** @} */


/**
 * @defgroup trigger Triggers
 */

/** Create trigger.
 * Triggers notify the application when certain events happen such as adding or
 * removing components.
 * 
 * An application can change the trigger callback or context pointer by calling
 * ecs_trigger_init for an existing trigger entity, by setting the
 * ecs_trigger_desc_t::entity.entity field in combination with callback and/or
 * ctx.
 *
 * See the documentation for ecs_trigger_desc_t for more details.
 *
 * @param world The world.
 * @param desc The trigger creation parameters.
 */
FLECS_API
ecs_entity_t ecs_trigger_init(
    ecs_world_t *world,
    const ecs_trigger_desc_t *desc);

/** Get trigger context.
 * This operation returns the context pointer set for the trigger. If
 * the provided entity is not a trigger, the function will return NULL.
 *
 * @param world The world.
 * @param trigger The trigger from which to obtain the context.
 * @return The context.
 */
FLECS_API
void* ecs_get_trigger_ctx(
    const ecs_world_t *world,
    ecs_entity_t trigger);

/** Same as ecs_get_trigger_ctx, but for binding ctx. 
 * The binding context is a context typically used to attach any language 
 * binding specific data that is needed when invoking a callback that is 
 * implemented in another language.
 * 
 * @param world The world.
 * @param trigger The trigger from which to obtain the context.
 * @return The context.
 */
FLECS_API
void* ecs_get_trigger_binding_ctx(
    const ecs_world_t *world,
    ecs_entity_t trigger);

typedef struct ecs_event_desc_t {
    /* The event id. Only triggers for the specified event will be notified */
    ecs_entity_t event;

    /* Component ids. Only triggers with a matching component id will be 
     * notified. Observers are guaranteed to get notified once, even if they
     * match more than one id. */
    const ecs_type_t *ids;

    /* The table for which to notify. */
    ecs_table_t *table;

    /* Optional 2nd table to notify. This can be used to communicate the
     * previous or next table, in case an entity is moved between tables. */
    ecs_table_t *other_table;

    /* Limit notified entities to ones starting from offset (row) in table */
    int32_t offset;

    /* Limit number of notified entities to count. offset+count must be less
     * than the total number of entities in the table. If left to 0, it will be
     * automatically determined by doing ecs_table_count(table) - offset. */
    int32_t count;

    /* Optional context. Assigned to iter param member */
    const void *param;

    /* Observable (usually the world) */
    ecs_poly_t *observable;

    /* Table events apply to tables, not the entities in the table. When
     * enabled, (super)set triggers are not notified. */
    bool table_event;

    /* When set, events will only be propagated by traversing the relation */
    ecs_entity_t relation;
} ecs_event_desc_t;

/** Send event.
 * This sends an event to matching triggers & is the mechanism used by flecs
 * itself to send OnAdd, OnRemove, etc events.
 * 
 * Applications can use this function to send custom events, where a custom
 * event can be any regular entity.
 * 
 * Applications should not send builtin flecs events, as this may violate
 * assumptions the code makes about the conditions under which those events are
 * sent.
 * 
 * Triggers are invoked synchronously. It is therefore safe to use stack-based
 * data as event context, which can be set in the "param" member.
 * 
 * To send a notification for a single entity, an application should set the
 * following members in the event descriptor:
 * 
 * - table: set to the table of the entity
 * - offset: set to the row of the entity in the table
 * - count: set to 1
 * 
 * The table & row of the entity can be obtained like this:
 *   ecs_record_t *r = ecs_record_find(world, e);
 *   desc.table = r->table;
 *   desc.offset = ECS_RECORD_TO_ROW(r->row);
 * 
 * @param world The world.
 * @param desc Event parameters.
 */
FLECS_API
void ecs_emit( 
    ecs_world_t *world,
    ecs_event_desc_t *desc);

/** @} */


/**
 * @defgroup observer Observers
 */

/** Create observer.
 * Observers are like triggers, but can subscribe for multiple terms. An 
 * observer only triggers when the source of the event meets all terms.
 *
 * See the documentation for ecs_observer_desc_t for more details.
 *
 * @param world The world.
 * @param desc The observer creation parameters.
 */
FLECS_API
ecs_entity_t ecs_observer_init(
    ecs_world_t *world,
    const ecs_observer_desc_t *desc);

/** Default run action for observer.
 * This function can be called from a custom observer run action (see 
 * ecs_observer_desc_t::run for more details). This function ensures that the 
 * observer's filter is applied to the iterator's table, filters out duplicate 
 * events and implements EcsMonitor logic.
 * 
 * @param it The iterator.
 * @return True if the observer was invoked.
 */
FLECS_API
bool ecs_observer_default_run_action(
    ecs_iter_t *it);

FLECS_API
void* ecs_get_observer_ctx(
    const ecs_world_t *world,
    ecs_entity_t observer);

FLECS_API
void* ecs_get_observer_binding_ctx(
    const ecs_world_t *world,
    ecs_entity_t observer);

/** @} */


/**
 * @defgroup iterator Iterators
 * @{
 */

/** Create iterator from poly object.
 * The provided poly object must have the iterable mixin. If an object is 
 * provided that does not have the mixin, the function will assert. 
 * 
 * When a filter is provided, an array of two iterators must be passed to the 
 * function. This allows the mixin implementation to create a chained iterator 
 * when necessary, which requires two iterator objects.
 * 
 * If a filter is provided, the first element in the array of two iterators is
 * the one that should be iterated. The mixin implementation may or may not set
 * the second element, depending on whether an iterator chain is required.
 * 
 * Additionally, when a filter is provided the returned iterator will be for a
 * single term with the provided filter id. If the iterator is chained, the
 * previous iterator in the chain can be accessed through it->chain_it.
 * 
 * @param world The world or stage for which to create the iterator.
 * @param poly The poly object from which to create the iterator.
 * @param iter The iterator (out, ecs_iter_t[2] when filter is set).
 * @param filter Optional term used for filtering the results.
 */
FLECS_API
void ecs_iter_poly(
    const ecs_world_t *world,
    const ecs_poly_t *poly,
    ecs_iter_t *iter,
    ecs_term_t *filter);

/** Progress any iterator.
 * This operation is useful in combination with iterators for which it is not
 * known what created them. Example use cases are functions that should accept
 * any kind of iterator (such as serializers) or iterators created from poly
 * objects.
 * 
 * This operation is slightly slower than using a type-specific iterator (e.g.
 * ecs_filter_next, ecs_query_next) as it has to call a function pointer which
 * introduces a level of indirection.
 * 
 * @param it The iterator.
 * @return True if iterator has more results, false if not.
 */
FLECS_API
bool ecs_iter_next(
    ecs_iter_t *it);

/** Cleanup iterator resources.
 * This operation cleans up any resources associated with the iterator. 
 * Iterators may contain allocated resources when the number of matched terms
 * exceeds ECS_TERM_CACHE_SIZE and/or when the source for the iterator requires
 * to keep state while iterating.
 * 
 * This operation should only be used when an iterator is not iterated until
 * completion (next has not yet returned false). When an iterator is iterated
 * until completion, resources are automatically freed.
 * 
 * @param it The iterator.
 */
FLECS_API
void ecs_iter_fini(
    ecs_iter_t *it);

/** Count number of matched entities in query.
 * This operation returns the number of matched entities. If a query contains no
 * matched entities but still yields results (e.g. it has no terms with This
 * subjects) the operation will return 0.
 * 
 * To determine the number of matched entities, the operation iterates the
 * iterator until it yields no more results.
 * 
 * @param it The iterator.
 * @return True if iterator has more results, false if not.
 */
FLECS_API
bool ecs_iter_count(
    ecs_iter_t *it);

/** Test if iterator is true.
 * This operation will return true if the iterator returns at least one result.
 * This is especially useful in combination with fact-checking rules (see the
 * rules addon).
 * 
 * The operation requires a valid iterator. After the operation is invoked, the
 * application should no longer invoke next on the iterator and should treat it
 * as if the iterator is iterated until completion.
 * 
 * @param it The iterator.
 * @return true if the iterator returns at least one result.
 */
FLECS_API
bool ecs_iter_is_true(
    ecs_iter_t *it);

/** Set value for iterator variable.
 * This constrains the iterator to return only results for which the variable
 * equals the specified value. The default value for all variables is 
 * EcsWildcard, which means the variable can assume any value.
 * 
 * Example:
 * 
 * // Rule that matches (Eats, *)
 * ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t) {
 *   .terms = {
 *     { .pred.entity = Eats, .obj.name = "_Food" }
 *   }
 * });
 * 
 * int food_var = ecs_rule_find_var(r, "Food");
 * 
 * // Set Food to Apples, so we're only matching (Eats, Apples)
 * ecs_iter_t it = ecs_rule_iter(world, r);
 * ecs_iter_set_var(&it, food_var, Apples);
 * 
 * while (ecs_rule_next(&it)) {
 *   for (int i = 0; i < it.count; i ++) {
 *     // iterate as usual
 *   }
 * }
 * 
 * The variable must be initialized after creating the iterator and before the
 * first call to next.
 * 
 * @param it The iterator.
 * @param var_id The variable index.
 * @param entity The entity variable value.
 */
FLECS_API
void ecs_iter_set_var(
    ecs_iter_t *it,
    int32_t var_id,
    ecs_entity_t entity);

/** Same as ecs_iter_set_var, but for a table.
 * This constrains the variable to all entities in a table.
 * 
 * @param it The iterator.
 * @param var_id The variable index.
 * @param table The table variable value.
 */
FLECS_API
void ecs_iter_set_var_as_table(
    ecs_iter_t *it,
    int32_t var_id,
    const ecs_table_t *table);

/** Same as ecs_iter_set_var, but for a range of entities
 * This constrains the variable to a range of entities in a table.
 * 
 * @param it The iterator.
 * @param var_id The variable index.
 * @param range The range variable value.
 */
FLECS_API
void ecs_iter_set_var_as_range(
    ecs_iter_t *it,
    int32_t var_id,
    const ecs_table_range_t *range);

/** Get value of iterator variable as entity.
 * A variable can be interpreted as entity if it is set to an entity, or if it
 * is set to a table range with count 1.
 * 
 * This operation can only be invoked on valid iterators. The variable index
 * must be smaller than the total number of variables provided by the iterator
 * (as set in ecs_iter_t::variable_count).
 * 
 * @param it The iterator.
 * @param var_id The variable index.
 */
FLECS_API
ecs_entity_t ecs_iter_get_var(
    ecs_iter_t *it,
    int32_t var_id);

/** Get value of iterator variable as table.
 * A variable can be interpreted as table if it is set as table range with
 * both offset and count set to 0, or if offset is 0 and count matches the
 * number of elements in the table.
 * 
 * This operation can only be invoked on valid iterators. The variable index
 * must be smaller than the total number of variables provided by the iterator
 * (as set in ecs_iter_t::variable_count).
 * 
 * @param it The iterator.
 * @param var_id The variable index.
 */
FLECS_API
ecs_table_t* ecs_iter_get_var_as_table(
    ecs_iter_t *it,
    int32_t var_id);

/** Get value of iterator variable as table range.
 * A value can be interpreted as table range if it is set as table range, or if
 * it is set to an entity with a non-empty type (the entity must have at least
 * one component, tag or relationship in its type).
 * 
 * This operation can only be invoked on valid iterators. The variable index
 * must be smaller than the total number of variables provided by the iterator
 * (as set in ecs_iter_t::variable_count).
 * 
 * @param it The iterator.
 * @param var_id The variable index.
 */
FLECS_API
ecs_table_range_t ecs_iter_get_var_as_range(
    ecs_iter_t *it,
    int32_t var_id);


/** Returns whether variable is constrained.
 * This operation returns true for variables set by one of the ecs_iter_set_var*
 * operations.
 * 
 * A constrained variable is guaranteed not to change values while results are
 * being iterated.
 * 
 * @param it The iterator.
 * @param var_id The variable index.
 * @return Whether the variable is constrained to a specified value.
 */
FLECS_API
bool ecs_iter_var_is_constrained(
    ecs_iter_t *it,
    int32_t var_id);

/** Create a paged iterator.
 * Paged iterators limit the results to those starting from 'offset', and will
 * return at most 'limit' results.
 * 
 * The iterator must be iterated with ecs_page_next.
 * 
 * A paged iterator acts as a passthrough for data exposed by the parent
 * iterator, so that any data provided by the parent will also be provided by
 * the paged iterator.
 * 
 * @param it The source iterator.
 * @param offset The number of entities to skip.
 * @param limit The maximum number of entities to iterate.
 * @return A page iterator.
 */
FLECS_API
ecs_iter_t ecs_page_iter(
    const ecs_iter_t *it,
    int32_t offset,
    int32_t limit);

/** Progress a paged iterator.
 * Progresses an iterator created by ecs_page_iter.
 * 
 * @param it The iterator.
 * @return true if iterator has more results, false if not.
 */
FLECS_API
bool ecs_page_next(
    ecs_iter_t *it);

/** Create a worker iterator.
 * Worker iterators can be used to equally divide the number of matched entities 
 * across N resources (usually threads). Each resource will process the total
 * number of matched entities divided by 'count'.
 * 
 * Entities are distributed across resources such that the distribution is
 * stable between queries. Two queries that match the same table are guaranteed
 * to match the same entities in that table.
 * 
 * The iterator must be iterated with ecs_worker_next.
 * 
 * A worker iterator acts as a passthrough for data exposed by the parent
 * iterator, so that any data provided by the parent will also be provided by
 * the worker iterator.
 * 
 * @param it The source iterator.
 * @param index The index of the current resource.
 * @param count The total number of resources to divide entities between.
 * @return A worker iterator.
 */
FLECS_API
ecs_iter_t ecs_worker_iter(
    const ecs_iter_t *it,
    int32_t index,
    int32_t count);

/** Progress a worker iterator.
 * Progresses an iterator created by ecs_worker_iter.
 * 
 * @param it The iterator.
 * @return true if iterator has more results, false if not.
 */
FLECS_API
bool ecs_worker_next(
    ecs_iter_t *it);

/** Obtain data for a query term.
 * This operation retrieves a pointer to an array of data that belongs to the
 * term in the query. The index refers to the location of the term in the query,
 * and starts counting from one.
 *
 * For example, the query "Position, Velocity" will return the Position array
 * for index 1, and the Velocity array for index 2.
 *
 * When the specified term is not owned by the entity this function returns a
 * pointer instead of an array. This happens when the source of a term is not
 * the entity being iterated, such as a shared component (from a prefab), a
 * component from a parent, or another entity. The ecs_term_is_owned operation
 * can be used to test dynamically if a term is owned.
 *
 * The provided size must be either 0 or must match the size of the datatype
 * of the returned array. If the size does not match, the operation may assert.
 * The size can be dynamically obtained with ecs_term_size.
 *
 * @param it The iterator.
 * @param size The size of the returned array.
 * @param index The index of the term in the query.
 * @return A pointer to the data associated with the term.
 */
FLECS_API
void* ecs_term_w_size(
    const ecs_iter_t *it,
    size_t size,
    int32_t index);

/** Test whether the term is readonly.
 * This operation returns whether this is a readonly term. Readonly terms are
 * annotated with [in], or are added as a const type in the C++ API.
 *
 * @param it The iterator.
 * @param index The index of the term in the query.
 * @return Whether the term is readonly.
 */
FLECS_API
bool ecs_term_is_readonly(
    const ecs_iter_t *it,
    int32_t index);    

/** Test whether the term is writeonly.
 * This operation returns whether this is a writeonly term. Writeonly terms are
 * annotated with [out].
 * 
 * Serializers are not required to serialize the values of a writeonly term.
 *
 * @param it The iterator.
 * @param index The index of the term in the query.
 * @return Whether the term is writeonly.
 */
FLECS_API
bool ecs_term_is_writeonly(
    const ecs_iter_t *it,
    int32_t index);

/** Test whether term is set.
 * 
 * @param it The iterator.
 * @param index The index of the term in the query.
 * @return Whether the term is set.
 */
FLECS_API
bool ecs_term_is_set(
    const ecs_iter_t *it,
    int32_t index); 

/** Test whether the term is owned
 * This operation returns whether the term is owned by the currently iterated
 * entity. This function will return false when the term is owned by another
 * entity, such as a parent or a prefab.
 *
 * @param it The iterator.
 * @param index The index of the term in the query.
 * @return Whether the term is owned by the iterated over entity/entities.
 */
FLECS_API
bool ecs_term_is_owned(
    const ecs_iter_t *it,
    int32_t index);

/** Convert iterator to string.
 * Prints the contents of an iterator to a string. Useful for debugging and/or
 * testing the output of an iterator.
 * 
 * The function only converts the currently iterated data to a string. To 
 * convert all data, the application has to manually call the next function and
 * call ecs_iter_str on each result.
 * 
 * @param it The iterator.
 * @return A string representing the contents of the iterator.
 */
FLECS_API
char* ecs_iter_str(
    const ecs_iter_t *it);

/** Find the column index for a given id.
 * This operation finds the index of a column in the current type for the 
 * specified id. For example, if an entity has type Position, Velocity, and the
 * application requests the id for the Velocity component, this function will
 * return 1.
 *
 * Note that the column index returned by this function starts from 0, as
 * opposed to 1 for the terms. The reason for this is that the returned index
 * is equivalent to using the ecs_type_get_index function.
 *
 * This operation can be used to request columns that are not requested by a
 * query. For example, a query may request Position, Velocity, but an entity
 * may also have Mass. With this function the iterator can request the data for
 * Mass as well, when used in combination with ecs_iter_column.
 *
 * @param it The iterator.
 * @return The type of the currently iterated entity/entities.
 */
FLECS_API
int32_t ecs_iter_find_column(
    const ecs_iter_t *it,
    ecs_id_t id);

/** Obtain data for a column index.
 * This operation can be used with the id obtained from ecs_iter_find_column to
 * request data from the currently iterated over entity/entities that is not
 * requested by the query.
 *
 * The data in the returned pointer can be accessed using the same index as
 * the one used to access the arrays returned by the ecs_term function.
 *
 * The provided size must be either 0 or must match the size of the datatype
 * of the returned array. If the size does not match, the operation may assert.
 * The size can be dynamically obtained with ecs_iter_column_size.
 *
 * Note that this function can be used together with iter::type to 
 * dynamically iterate all data that the matched entities have. An application
 * can use the ecs_vector_count function to obtain the number of elements in a
 * type. All indices from 0..type->count are valid column indices.
 *
 * Additionally, note that this provides unprotected access to the column data.
 * An iterator cannot know or prevent accessing columns that are not queried for
 * and thus applications should only use this when it can be guaranteed that
 * there are no other threads reading/writing the same column data.
 *
 * @param it The iterator.
 * @param size The size of the column.
 * @param index The index of the column.
 * @return The data belonging to the column.
 */
FLECS_API
void* ecs_iter_column_w_size(
    const ecs_iter_t *it,
    size_t size,
    int32_t index);

/** Obtain size for a column index.
 * This operation obtains the size for a column. The size is equal to the size
 * of the datatype associated with the column.
 *
 * @param it The iterator.
 * @param index The index of the column.
 * @return The size belonging to the column.
 */
FLECS_API
size_t ecs_iter_column_size(
    const ecs_iter_t *it,
    int32_t index);    

/** @} */


/**
 * @defgroup staging Staging
 * @{
 */

/** Begin frame. 
 * When an application does not use ecs_progress to control the main loop, it
 * can still use Flecs features such as FPS limiting and time measurements. This
 * operation needs to be invoked whenever a new frame is about to get processed.
 *
 * Calls to ecs_frame_begin must always be followed by ecs_frame_end.
 *
 * The function accepts a delta_time parameter, which will get passed to 
 * systems. This value is also used to compute the amount of time the function
 * needs to sleep to ensure it does not exceed the target_fps, when it is set.
 * When 0 is provided for delta_time, the time will be measured.
 *
 * This function should only be ran from the main thread.
 *
 * @param world The world.
 * @param delta_time Time elapsed since the last frame.
 * @return The provided delta_time, or measured time if 0 was provided.
 */
FLECS_API
FLECS_FLOAT ecs_frame_begin(
    ecs_world_t *world,
    FLECS_FLOAT delta_time);

/** End frame. 
 * This operation must be called at the end of the frame, and always after
 * ecs_frame_begin.
 *
 * @param world The world.
 */
FLECS_API
void ecs_frame_end(
    ecs_world_t *world);

/** Begin staging.
 * When an application does not use ecs_progress to control the main loop, it
 * can still use Flecs features such as the defer queue. When an application
 * needs to stage changes, it needs to call this function after ecs_frame_begin.
 * A call to ecs_staging_begin must be followed by a call to ecs_staging_end.
 * 
 * When staging is enabled, modifications to entities are stored to a stage.
 * This ensures that arrays are not modified while iterating. Modifications are
 * merged back to the "main stage" when ecs_staging_end is invoked.
 *
 * While the world is in staging mode, no structural changes (add/remove/...)
 * can be made to the world itself. Operations must be executed on a stage
 * instead (see ecs_get_stage).
 *
 * This function should only be ran from the main thread.
 *
 * @param world The world
 * @return Whether world is currently staged.
 */
FLECS_API
bool ecs_staging_begin(
    ecs_world_t *world);

/** End staging.
 * Leaves staging mode. After this operation the world may be directly mutated
 * again. By default this operation also merges data back into the world, unless
 * automerging was disabled explicitly.
 *
 * This function should only be ran from the main thread.
 *
 * @param world The world
 */
FLECS_API
void ecs_staging_end(
    ecs_world_t *world);

/** Merge world or stage.
 * When automatic merging is disabled, an application can call this
 * operation on either an individual stage, or on the world which will merge
 * all stages. This operation may only be called when staging is not enabled
 * (either after progress() or after staging_end()).
 *
 * This operation may be called on an already merged stage or world.
 *
 * @param world The world.
 */
FLECS_API
void ecs_merge(
    ecs_world_t *world);

/** Defer operations until end of frame. 
 * When this operation is invoked while iterating, operations inbetween the
 * defer_begin and defer_end operations are executed at the end of the frame.
 *
 * This operation is thread safe.
 * 
 * @param world The world.
 * @return true if world changed from non-deferred mode to deferred mode.
 */
FLECS_API
bool ecs_defer_begin(
    ecs_world_t *world);

/** Test if deferring is enabled for current stage.
 * 
 * @param world The world.
 * @return True if deferred, false if not.
 */
FLECS_API
bool ecs_is_deferred(
    const ecs_world_t *world);

/** End block of operations to defer. 
 * See defer_begin.
 *
 * This operation is thread safe.
 *
 * @param world The world.
 * @return true if world changed from deferred mode to non-deferred mode.
 */
FLECS_API
bool ecs_defer_end(
    ecs_world_t *world);

/** Enable/disable automerging for world or stage.
 * When automerging is enabled, staged data will automatically be merged with
 * the world when staging ends. This happens at the end of progress(), at a
 * sync point or when staging_end() is called.
 *
 * Applications can exercise more control over when data from a stage is merged
 * by disabling automerging. This requires an application to explicitly call
 * merge() on the stage.
 *
 * When this function is invoked on the world, it sets all current stages to
 * the provided value and sets the default for new stages. When this function is
 * invoked on a stage, automerging is only set for that specific stage.
 *
 * @param world The world.
 * @param automerge Whether to enable or disable automerging.
 */
FLECS_API
void ecs_set_automerge(
    ecs_world_t *world,
    bool automerge);

/** Configure world to have N stages.
 * This initializes N stages, which allows applications to defer operations to
 * multiple isolated defer queues. This is typically used for applications with
 * multiple threads, where each thread gets its own queue, and commands are
 * merged when threads are synchronized.
 *
 * Note that the ecs_set_threads function already creates the appropriate
 * number of stages. The set_stages() operation is useful for applications that
 * want to manage their own stages and/or threads.
 * 
 * @param world The world.
 * @param stages The number of stages.
 */
FLECS_API
void ecs_set_stages(
    ecs_world_t *world,
    int32_t stages);

/** Get number of configured stages.
 * Return number of stages set by ecs_set_stages.
 *
 * @param world The world.
 * @return The number of stages used for threading.
 */
FLECS_API
int32_t ecs_get_stage_count(
    const ecs_world_t *world);

/** Get current stage id.
 * The stage id can be used by an application to learn about which stage it is
 * using, which typically corresponds with the worker thread id.
 *
 * @param world The world.
 * @return The stage id.
 */
FLECS_API
int32_t ecs_get_stage_id(
    const ecs_world_t *world);

/** Get stage-specific world pointer.
 * Flecs threads can safely invoke the API as long as they have a private 
 * context to write to, also referred to as the stage. This function returns a
 * pointer to a stage, disguised as a world pointer.
 *
 * Note that this function does not(!) create a new world. It simply wraps the
 * existing world in a thread-specific context, which the API knows how to
 * unwrap. The reason the stage is returned as an ecs_world_t is so that it
 * can be passed transparently to the existing API functions, vs. having to 
 * create a dediated API for threading.
 *
 * @param world The world.
 * @param stage_id The index of the stage to retrieve.
 * @return A thread-specific pointer to the world. 
 */
FLECS_API
ecs_world_t* ecs_get_stage(
    const ecs_world_t *world,
    int32_t stage_id);

/** Get actual world from world.
 *
 * @param world A pointer to a stage or the world.
 * @return The world.
 */
FLECS_API
const ecs_world_t* ecs_get_world(
    const ecs_poly_t *world);

/** Test whether the current world object is readonly.
 * This function allows the code to test whether the currently used world object
 * is readonly or whether it allows for writing.  
 *
 * @param world A pointer to a stage or the world.
 * @return True if the world or stage is readonly.
 */
FLECS_API
bool ecs_stage_is_readonly(
    const ecs_world_t *world);

/** Create asynchronous stage.
 * An asynchronous stage can be used to asynchronously queue operations for
 * later merging with the world. An asynchronous stage is similar to a regular
 * stage, except that it does not allow reading from the world.
 *
 * Asynchronous stages are never merged automatically, and must therefore be
 * manually merged with the ecs_merge function. It is not necessary to call
 * defer_begin or defer_end before and after enqueuing commands, as an 
 * asynchronous stage unconditionally defers operations.
 *
 * The application must ensure that no commands are added to the stage while the
 * stage is being merged.
 *
 * An asynchronous stage must be cleaned up by ecs_async_stage_free. 
 *
 * @param world The world.
 * @return The stage.
 */
FLECS_API
ecs_world_t* ecs_async_stage_new(
    ecs_world_t *world);

/** Free asynchronous stage.
 * The provided stage must be an asynchronous stage. If a non-asynchronous stage
 * is provided, the operation will fail.
 *
 * @param stage The stage to free.
 */
FLECS_API
void ecs_async_stage_free(
    ecs_world_t *stage);

/** Test whether provided stage is asynchronous.
 *
 * @param stage The stage.
 * @return True when the stage is asynchronous, false for a regular stage or 
 *         world.
 */
FLECS_API
bool ecs_stage_is_async(
    ecs_world_t *stage);

/** @} */

/**
 * @defgroup id_search_functions Search functions for component ids.
 * @brief Low-level functions to search for component ids in table types.
 * @{
 */

/** Search for component id in table type.
 * This operation returns the index of first occurrance of the id in the table
 * type. The id may be a wildcard.
 * 
 * When id_out is provided, the function will assign it with the found id. The
 * found id may be different from the provided id if it is a wildcard.
 * 
 * This is a constant time operation.
 * 
 * @param world The world.
 * @param table The table.
 * @param id The id to search for.
 * @param id_out If provided, it will be set to the found id (optional).
 * @return The index of the id in the table type.
 */
FLECS_API
int32_t ecs_search(
    const ecs_world_t *world,
    const ecs_table_t *table,
    ecs_id_t id,
    ecs_id_t *id_out);

/** Search for component id in table type starting from an offset.
 * This operation is the same as ecs_search, but starts searching from an offset
 * in the table type.
 * 
 * This operation is typically called in a loop where the resulting index is
 * used in the next iteration as offset:
 * 
 * int32_t index = -1;
 * while ((index = ecs_search_offset(world, table, offset, id, NULL))) {
 *   // do stuff
 * }
 * 
 * Depending on how the operation is used it is either linear or constant time.
 * When the id has the form (id) or (rel, *) and the operation is invoked as 
 * in the above example, it is guaranteed to be constant time.
 * 
 * If the provided id has the form (*, obj) the operation takes linear time. The
 * reason for this is that ids for an object are not packed together, as they
 * are sorted relation first.
 * 
 * If the id at the offset does not match the provided id, the operation will do
 * a linear search to find a matching id.
 * 
 * @param world The world.
 * @param table The table.
 * @param offset Offset from where to start searching.
 * @param id The id to search for.
 * @param id_out If provided, it will be set to the found id (optional).
 * @return The index of the id in the table type.
 */
FLECS_API
int32_t ecs_search_offset(
    const ecs_world_t *world,
    const ecs_table_t *table,
    int32_t offset,
    ecs_id_t id,
    ecs_id_t *id_out);

/** Search for component/relation id in table type starting from an offset.
 * This operation is the same as ecs_search_offset, but has the additional
 * capability of traversing relationships to find a component. For example, if
 * an application wants to find a component for either the provided table or a
 * prefab (using the IsA relation) of that table, it could use the operation 
 * like this:
 * 
 * int32_t index = ecs_search_relation(
 *   world,            // the world
 *   table,            // the table
 *   0,                // offset 0
 *   ecs_id(Position), // the component id
 *   EcsIsA,           // the relation to traverse
 *   0,                // start at depth 0 (the table itself)
 *   0,                // no depth limit
 *   NULL,             // (optional) entity on which component was found
 *   NULL,             // see above
 *   NULL);            // internal type with information about matched id
 * 
 * The operation searches depth first. If a table type has 2 IsA relations, the
 * operation will first search the IsA tree of the first relation.
 * 
 * When choosing betwen ecs_search, ecs_search_offset and ecs_search_relation,
 * the simpler the function the better its performance.
 * 
 * @param world The world.
 * @param table The table.
 * @param offset Offset from where to start searching.
 * @param id The id to search for.
 * @param rel The relation to traverse (optional).
 * @param min_depth The minimum search depth. Use 1 for only shared components.
 * @param max_depth The maximum search depth. Zero means no maximum.
 * @param subject_out If provided, it will be set to the matched entity.
 * @param id_out If provided, it will be set to the found id (optional).
 * @param depth_out If provided, it will be set to the traversal depth.
 * @param tr_out Internal datatype.
 * @return The index of the id in the table type.
 */
FLECS_API
int32_t ecs_search_relation(
    const ecs_world_t *world,
    const ecs_table_t *table,
    int32_t offset,
    ecs_id_t id,
    ecs_entity_t rel,
    int32_t min_depth,
    int32_t max_depth,
    ecs_entity_t *subject_out,
    ecs_id_t *id_out,
    int32_t *depth_out,
    struct ecs_table_record_t **tr_out);

/** Similar to ecs_search_relation, but find component at maximum depth.
 * Instead of searching for the first occurrence of a component following a
 * relationship, this operation returns the last (deepest) occurrence of the
 * component. This operation can be used to determine the depth of a tree.
 * 
 * @param world The world.
 * @param table The table.
 * @param offset Offset from where to start searching.
 * @param id The id to search for.
 * @param rel The relation to traverse (optional).
 * @param min_depth The minimum search depth. Use 1 for only shared components.
 * @param max_depth The maximum search depth. Zero means no maximum.
 * @param subject_out If provided, it will be set to the matched entity.
 * @param id_out If provided, it will be set to the found id (optional).
 * @param depth_out If provided, it will be set to the traversal depth.
 * @param tr_out Internal datatype.
 */
FLECS_API
int32_t ecs_search_relation_last(
    const ecs_world_t *world,
    const ecs_table_t *table,
    int32_t offset,
    ecs_id_t id,
    ecs_entity_t rel,
    int32_t min_depth,
    int32_t max_depth,
    ecs_entity_t *subject_out,
    ecs_id_t *id_out,
    int32_t *depth_out,
    struct ecs_table_record_t **tr_out);

/** @} */

/**
 * @defgroup table_functions Public table operations
 * @brief Low-level table functions. These functions are intended to enable the
 *        creation of higher-level operations. It is not recommended to use
 *        these operations directly in application code as they do not provide
 *        the same safety guarantees as the other APIs.
 * @{
 */

/** Get type for table.
 *
 * @param table The table.
 * @return The type of the table.
 */
FLECS_API
const ecs_type_t* ecs_table_get_type(
    const ecs_table_t *table);

/** Get storage type for table.
 *
 * @param table The table.
 * @return The storage type of the table (components only).
 */
FLECS_API
ecs_table_t* ecs_table_get_storage_table(
    const ecs_table_t *table);

/** Convert index in table type to index in table storage type. */
int32_t ecs_table_type_to_storage_index(
    const ecs_table_t *table,
    int32_t index);

/** Convert index in table storage type to index in table type. */
int32_t ecs_table_storage_to_type_index(
    const ecs_table_t *table,
    int32_t index);

/** Returns the number of records in the table. 
 * This operation returns the number of records that have been populated through
 * the regular (entity) API as well as the number of records that have been
 * inserted using the direct access API.
 *
 * @param table The table.
 * @return The number of records in a table.
 */
FLECS_API
int32_t ecs_table_count(
    const ecs_table_t *table);

/** Get table that has all components of current table plus the specified id.
 * If the provided table already has the provided id, the operation will return
 * the provided table.
 *
 * @param world The world.
 * @param table The table.
 * @param id The id to add.
 * @result The resulting table.
 */
FLECS_API
ecs_table_t* ecs_table_add_id(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_id_t id);

/** Get table that has all components of current table minus the specified id.
 * If the provided table doesn't have the provided id, the operation will return
 * the provided table.
 *
 * @param world The world.
 * @param table The table.
 * @param id The id to remove.
 * @result The resulting table.
 */
FLECS_API
ecs_table_t* ecs_table_remove_id(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_id_t id);

/** Lock or unlock table.
 * When a table is locked, modifications to it will trigger an assert. When the 
 * table is locked recursively, it will take an equal amount of unlock
 * operations to actually unlock the table.
 *
 * Table locks can be used to build safe iterators where it is guaranteed that
 * the contents of a table are not modified while it is being iterated.
 *
 * The operation only works when called on the world, and has no side effects 
 * when called on a stage. The assumption is that when called on a stage,
 * operations are deferred already.
 *
 * @param world The world.
 * @param table The table to lock.
 */
FLECS_API
void ecs_table_lock(
    ecs_world_t *world,
    ecs_table_t *table);

/** Unlock a table.
 * Must be called after calling ecs_table_lock.
 *
 * @param world The world.
 * @param table The table to unlock.
 */
FLECS_API
void ecs_table_unlock(
    ecs_world_t *world,
    ecs_table_t *table);    

/** Returns whether table is a module or contains module contents
 * Returns true for tables that have module contents. Can be used to filter out
 * tables that do not contain application data.
 *
 * @param table The table.
 * @return true if table contains module contents, false if not.
 */
FLECS_API
bool ecs_table_has_module(
    ecs_table_t *table);

/** Commit (move) entity to a table.
 * This operation moves an entity from its current table to the specified
 * table. This may trigger the following actions:
 * - Ctor for each component in the target table
 * - Move for each overlapping component
 * - Dtor for each component in the source table.
 * - OnAdd triggers for non-overlapping components in the target table
 * - OnRemove triggers for non-overlapping components in the source table.
 *
 * This operation is a faster than adding/removing components individually.
 *
 * The application must explicitly provide the difference in components between
 * tables as the added/removed parameters. This can usually be derived directly
 * from the result of ecs_table_add_id and esc_table_remove_id. These arrays are
 * required to properly execute OnAdd/OnRemove triggers.
 *
 * @param world The world.
 * @param entity The entity to commit.
 * @param record The entity's record (optional, providing it saves a lookup).
 * @param table The table to commit the entity to.
 * @return True if the entity got moved, false otherwise.
 */
FLECS_API
bool ecs_commit(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_record_t *record,
    ecs_table_t *table,
    const ecs_type_t *added,
    const ecs_type_t *removed);

/** Find record for entity. */
FLECS_API
ecs_record_t* ecs_record_find(
    const ecs_world_t *world,
    ecs_entity_t entity);

/** Get component pointer from column/record. */
FLECS_API
void* ecs_record_get_column(
    ecs_record_t *r,
    int32_t column,
    size_t c_size);

/** @} */

/**
 * @file flecs_c.h
 * @brief Extends the core API with convenience functions/macro's for C applications.
 */

#ifndef FLECS_C_
#define FLECS_C_

/**
 * @defgroup create_macros Macro's that help with creation of ECS objects.
 * @{
 */

/* Use for declaring entity, tag, prefab / any other entity identifier */
#define ECS_DECLARE(id)\
    ecs_entity_t id, ecs_id(id)

#define ECS_ENTITY_DEFINE(world, id, ...) \
    { \
        ecs_entity_desc_t desc = {0}; \
        desc.entity = id; \
        desc.name = #id; \
        desc.add_expr = #__VA_ARGS__; \
        id = ecs_entity_init(world, &desc); \
        ecs_id(id) = id; \
    } \
    (void)id; \
    (void)ecs_id(id);

#define ECS_ENTITY(world, id, ...) \
    ecs_entity_t ecs_id(id); \
    ecs_entity_t id = 0; \
    ECS_ENTITY_DEFINE(world, id, __VA_ARGS__);

#define ECS_TAG_DEFINE(world, id)         ECS_ENTITY_DEFINE(world, id, 0)
#define ECS_TAG(world, id)                ECS_ENTITY(world, id, 0)

#define ECS_PREFAB_DEFINE(world, id, ...) ECS_ENTITY_DEFINE(world, id, Prefab, __VA_ARGS__)
#define ECS_PREFAB(world, id, ...)        ECS_ENTITY(world, id, Prefab, __VA_ARGS__)

/* Use for declaring component identifiers */
#define ECS_COMPONENT_DECLARE(id)         ecs_entity_t ecs_id(id)
#define ECS_COMPONENT_DEFINE(world, id) \
    {\
        ecs_component_desc_t desc = {0}; \
        desc.entity.entity = ecs_id(id); \
        desc.entity.name = #id; \
        desc.entity.symbol = #id; \
        desc.size = sizeof(id); \
        desc.alignment = ECS_ALIGNOF(id); \
        ecs_id(id) = ecs_component_init(world, &desc);\
        ecs_assert(ecs_id(id) != 0, ECS_INVALID_PARAMETER, NULL);\
    }

#define ECS_COMPONENT(world, id)\
    ecs_entity_t ecs_id(id) = 0;\
    ECS_COMPONENT_DEFINE(world, id);\
    (void)ecs_id(id)

/* Use for declaring trigger, observer and system identifiers */
#define ECS_SYSTEM_DECLARE(id)         ecs_entity_t ecs_id(id)

/* Triggers */
#define ECS_TRIGGER_DEFINE(world, id, kind, component) \
    {\
        ecs_trigger_desc_t desc = {0}; \
        desc.entity.entity = ecs_id(id); \
        desc.entity.name = #id;\
        desc.callback = id;\
        desc.expr = #component;\
        desc.events[0] = kind;\
        ecs_id(id) = ecs_trigger_init(world, &desc);\
        ecs_assert(ecs_id(id) != 0, ECS_INVALID_PARAMETER, NULL);\
    }

#define ECS_TRIGGER(world, id, kind, component) \
    ecs_entity_t ecs_id(id) = 0; \
    ECS_TRIGGER_DEFINE(world, id, kind, component);\
    ecs_entity_t id = ecs_id(id);\
    (void)ecs_id(id);\
    (void)id;

/* Observers */
#define ECS_OBSERVER_DEFINE(world, id, kind, ...)\
    {\
        ecs_observer_desc_t desc = {0};\
        desc.entity.entity = ecs_id(id); \
        desc.entity.name = #id;\
        desc.callback = id;\
        desc.filter.expr = #__VA_ARGS__;\
        desc.events[0] = kind;\
        ecs_id(id) = ecs_observer_init(world, &desc);\
        ecs_assert(ecs_id(id) != 0, ECS_INVALID_PARAMETER, NULL);\
    }

#define ECS_OBSERVER(world, id, kind, ...)\
    ecs_entity_t ecs_id(id) = 0; \
    ECS_OBSERVER_DEFINE(world, id, kind, __VA_ARGS__);\
    ecs_entity_t id = ecs_id(id);\
    (void)ecs_id(id);\
    (void)id;

/** @} */


/**
 * @defgroup function_macros Convenience macro's that wrap ECS operations
 * @{
 */


/* -- World API -- */

#define ecs_set_component_actions(world, T, ...)\
    ecs_set_component_actions_w_id(world, ecs_id(T), &(EcsComponentLifecycle)__VA_ARGS__)

/* -- New -- */

#define ecs_new(world, T) ecs_new_w_id(world, ecs_id(T))

#define ecs_new_w_pair(world, relation, object)\
    ecs_new_w_id(world, ecs_pair(relation, object))

#define ecs_bulk_new(world, component, count)\
    ecs_bulk_new_w_id(world, ecs_id(component), count)

#define ecs_new_entity(world, n)\
    ecs_entity_init(world, &(ecs_entity_desc_t) {\
        .name = n,\
    })

#define ecs_new_prefab(world, n)\
    ecs_entity_init(world, &(ecs_entity_desc_t) {\
        .name = n,\
        .add = {EcsPrefab}\
    })

/* -- Add -- */

#define ecs_add(world, entity, T)\
    ecs_add_id(world, entity, ecs_id(T))

#define ecs_add_pair(world, subject, relation, object)\
    ecs_add_id(world, subject, ecs_pair(relation, object))


/* -- Remove -- */

#define ecs_remove(world, entity, T)\
    ecs_remove_id(world, entity, ecs_id(T))

#define ecs_remove_pair(world, subject, relation, object)\
    ecs_remove_id(world, subject, ecs_pair(relation, object))


/* -- Bulk remove/delete -- */

#define ecs_delete_children(world, parent)\
    ecs_delete_with(world, ecs_pair(EcsChildOf, parent))


/* -- Set -- */

#define ecs_set_ptr(world, entity, component, ptr)\
    ecs_set_id(world, entity, ecs_id(component), sizeof(component), ptr)

#define ecs_set(world, entity, component, ...)\
    ecs_set_id(world, entity, ecs_id(component), sizeof(component), &(component)__VA_ARGS__)

#define ecs_set_pair(world, subject, relation, object, ...)\
    ecs_set_id(world, subject,\
        ecs_pair(ecs_id(relation), object),\
        sizeof(relation), &(relation)__VA_ARGS__)

#define ecs_set_pair_second(world, subject, relation, object, ...)\
    ecs_set_id(world, subject,\
        ecs_pair(relation, ecs_id(object)),\
        sizeof(object), &(object)__VA_ARGS__)

#define ecs_set_pair_object ecs_set_pair_second

#define ecs_set_override(world, entity, T, ...)\
    ecs_add_id(world, entity, ECS_OVERRIDE | ecs_id(T));\
    ecs_set(world, entity, T, __VA_ARGS__)

/* -- Emplace -- */

#define ecs_emplace(world, entity, T)\
    (ECS_CAST(T*, ecs_emplace_id(world, entity, ecs_id(T))))

/* -- Get -- */

#define ecs_get(world, entity, T)\
    (ECS_CAST(const T*, ecs_get_id(world, entity, ecs_id(T))))

#define ecs_get_pair(world, subject, relation, object)\
    (ECS_CAST(relation*, ecs_get_id(world, subject,\
        ecs_pair(ecs_id(relation), object))))

#define ecs_get_pair_second(world, subject, relation, object)\
    (ECS_CAST(object*, ecs_get_id(world, subject,\
        ecs_pair(relation, ecs_id(object)))))

#define ecs_get_pair_object ecs_get_pair_second

/* -- Ref -- */

#define ecs_ref_init(world, entity, T)\
    ecs_ref_init_id(world, entity, ecs_id(T))

#define ecs_ref_get(world, ref, T)\
    (ECS_CAST(const T*, ecs_ref_get_id(world, ref, ecs_id(T))))

/* -- Get mut & Modified -- */

#define ecs_get_mut(world, entity, T, is_added)\
    (ECS_CAST(T*, ecs_get_mut_id(world, entity, ecs_id(T), is_added)))

#define ecs_get_mut_pair(world, subject, relation, object, is_added)\
    (ECS_CAST(relation*, ecs_get_mut_id(world, subject,\
        ecs_pair(ecs_id(relation), object), is_added)))

#define ecs_get_mut_pair_second(world, subject, relation, object, is_added)\
    (ECS_CAST(object*, ecs_get_mut_id(world, subject,\
        ecs_pair(relation, ecs_id(object)), is_added)))

#define ecs_get_mut_pair_object ecs_get_mut_pair_second

#define ecs_modified(world, entity, component)\
    ecs_modified_id(world, entity, ecs_id(component))

#define ecs_modified_pair(world, subject, relation, object)\
    ecs_modified_id(world, subject, ecs_pair(relation, object))


/* -- Singletons -- */

#define ecs_singleton_add(world, comp)\
    ecs_add(world, ecs_id(comp), comp)

#define ecs_singleton_remove(world, comp)\
    ecs_remove(world, ecs_id(comp), comp)

#define ecs_singleton_get(world, comp)\
    ecs_get(world, ecs_id(comp), comp)

#define ecs_singleton_set(world, comp, ...)\
    ecs_set(world, ecs_id(comp), comp, __VA_ARGS__)

#define ecs_singleton_get_mut(world, comp)\
    ecs_get_mut(world, ecs_id(comp), comp, NULL)

#define ecs_singleton_modified(world, comp)\
    ecs_modified(world, ecs_id(comp), comp)


/* -- Has, Owns & Shares -- */

#define ecs_has(world, entity, T)\
    ecs_has_id(world, entity, ecs_id(T))

#define ecs_has_pair(world, entity, relation, object)\
    ecs_has_id(world, entity, ecs_pair(relation, object))

#define ecs_owns_id(world, entity, id)\
    (ecs_search(world, ecs_get_table(world, entity), id, 0) != -1)

#define ecs_owns_pair(world, entity, relation, object)\
    ecs_owns_id(world, entity, ecs_pair(relation, object))

#define ecs_owns(world, entity, T)\
    ecs_owns_id(world, entity, ecs_id(T))

#define ecs_shares_id(world, entity, id)\
    (ecs_search_relation(world, ecs_get_table(world, entity), 0, ecs_id(id), \
        EcsIsA, 1, 0, 0, 0, 0) != -1)

#define ecs_shares_pair(world, entity, relation, object)\
    (ecs_shares_id(world, entity, ecs_pair(relation, object)))

#define ecs_shares(world, entity, T)\
    (ecs_shares_id(world, entity, ecs_id(T)))


/* -- Enable / Disable component -- */

#define ecs_enable_component(world, entity, T, enable)\
    ecs_enable_component_w_id(world, entity, ecs_id(T), enable)

#define ecs_is_component_enabled(world, entity, T)\
    ecs_is_component_enabled_w_id(world, entity, ecs_id(T))


/* -- Count -- */

#define ecs_count(world, type)\
    ecs_count_id(world, ecs_id(type))


/* -- Lookups & Paths -- */

#define ecs_lookup_path(world, parent, path)\
    ecs_lookup_path_w_sep(world, parent, path, ".", NULL, true)

#define ecs_lookup_fullpath(world, path)\
    ecs_lookup_path_w_sep(world, 0, path, ".", NULL, true)

#define ecs_get_path(world, parent, child)\
    ecs_get_path_w_sep(world, parent, child, ".", NULL)

#define ecs_get_fullpath(world, child)\
    ecs_get_path_w_sep(world, 0, child, ".", NULL)

#define ecs_get_fullpath_buf(world, child, buf)\
    ecs_get_path_w_sep_buf(world, 0, child, ".", NULL, buf)

#define ecs_new_from_path(world, parent, path)\
    ecs_new_from_path_w_sep(world, parent, path, ".", NULL)

#define ecs_new_from_fullpath(world, path)\
    ecs_new_from_path_w_sep(world, 0, path, ".", NULL)

#define ecs_add_path(world, entity, parent, path)\
    ecs_add_path_w_sep(world, entity, parent, path, ".", NULL)

#define ecs_add_fullpath(world, entity, path)\
    ecs_add_path_w_sep(world, entity, 0, path, ".", NULL)


/* -- Queries -- */

#define ecs_query_table_count(query) query->cache.tables.count
#define ecs_query_empty_table_count(query) query->cache.empty_tables.count

/* -- Iterators -- */

#define ecs_term_id(it, index)\
    ((it)->ids[(index) - 1])

#define ecs_term_source(it, index)\
    ((it)->subjects ? (it)->subjects[(index) - 1] : 0)

#define ecs_term_size(it, index)\
    ((index) == 0 ? sizeof(ecs_entity_t) : ECS_CAST(size_t, (it)->sizes[(index) - 1]))

#define ecs_term_is_owned(it, index)\
    ((it)->subjects == NULL || (it)->subjects[(index) - 1] == 0)

#define ecs_term(it, T, index)\
    (ECS_CAST(T*, ecs_term_w_size(it, sizeof(T), index)))

#define ecs_iter_column(it, T, index)\
    (ECS_CAST(T*, ecs_iter_column_w_size(it, sizeof(T), index)))

/** @} */

/**
 * @defgroup utilities Utility macro's for commonly used operations
 * @{
 */

#define ecs_childof(parent) ecs_pair(EcsChildOf, parent)

#define ecs_isa(base) ecs_pair(EcsIsA, base)

/** @} */

/**
 * @defgroup temporary_macros Temp macro's for easing the transition to v3
 * @{
 */

/** Declare a constructor.
 * Example:
 *   ECS_CTOR(MyType, ptr, { ptr->value = NULL; });
 */
#define ECS_CTOR(type, var, ...)\
    ECS_XTOR_IMPL(type, ctor, var, __VA_ARGS__)

/** Declare a destructor.
 * Example:
 *   ECS_DTOR(MyType, ptr, { free(ptr->value); });
 */
#define ECS_DTOR(type, var, ...)\
    ECS_XTOR_IMPL(type, dtor, var, __VA_ARGS__)

/** Declare a copy action.
 * Example:
 *   ECS_COPY(MyType, dst, src, { dst->value = strdup(src->value); });
 */
#define ECS_COPY(type, dst_var, src_var, ...)\
    ECS_COPY_IMPL(type, dst_var, src_var, __VA_ARGS__)

/** Declare a move action.
 * Example:
 *   ECS_MOVE(MyType, dst, src, { dst->value = src->value; src->value = 0; });
 */
#define ECS_MOVE(type, dst_var, src_var, ...)\
    ECS_MOVE_IMPL(type, dst_var, src_var, __VA_ARGS__)

/** Declare an on_set action.
 * Example:
 *   ECS_ON_SET(MyType, ptr, { printf("%d\n", ptr->value); });
 */
#define ECS_ON_SET(type, ptr, ...)\
    ECS_ON_SET_IMPL(type, ptr, __VA_ARGS__)

/* Map from typename to function name of component lifecycle action */
#define ecs_ctor(type) type##_ctor
#define ecs_dtor(type) type##_dtor
#define ecs_copy(type) type##_copy
#define ecs_move(type) type##_move
#define ecs_on_set(type) type##_on_set
#define ecs_on_add(type) type##_on_add
#define ecs_on_remove(type) type##_on_remove

#define ecs_query_new(world, q_expr)\
    ecs_query_init(world, &(ecs_query_desc_t){\
        .filter.expr = q_expr\
    })

#define ecs_rule_new(world, q_expr)\
    ecs_rule_init(world, &(ecs_filter_desc_t){\
        .expr = q_expr\
    })

#define ECS_TYPE(world, id, ...) \
    ecs_entity_t id = ecs_type_init(world, &(ecs_type_desc_t){\
        .entity.name = #id,\
        .ids_expr = #__VA_ARGS__\
    });\
    ecs_assert(id != 0, ECS_INVALID_PARAMETER, NULL);\
    (void)id;

/** @} */


#endif // FLECS_C_

/**
 * @file addons.h
 * @brief Include enabled addons.
 *
 * This file should only be included by the main flecs.h header.
 */

#ifndef FLECS_ADDONS_H
#define FLECS_ADDONS_H

/* Don't enable web addons if we're running as a webasm app */
#ifdef ECS_TARGET_EM
#ifndef FLECS_NO_HTTP
#define FLECS_NO_HTTP
#endif // FLECS_NO_HTTP

#ifndef FLECS_NO_REST
#define FLECS_NO_REST
#endif // FLECS_NO_REST
#endif // ECS_TARGET_EM

/* Blacklist macro's */
#ifdef FLECS_NO_CPP
#undef FLECS_CPP
#endif
#ifdef FLECS_NO_MODULE
#undef FLECS_MODULE
#endif
#ifdef FLECS_NO_PARSER
#undef FLECS_PARSER
#endif
#ifdef FLECS_NO_PLECS
#undef FLECS_PLECS
#endif
#ifdef FLECS_NO_RULES
#undef FLECS_RULES
#endif
#ifdef FLECS_NO_SNAPSHOT
#undef FLECS_SNAPSHOT
#endif
#ifdef FLECS_NO_STATS
#undef FLECS_STATS
#endif
#ifdef FLECS_NO_SYSTEM
#undef FLECS_SYSTEM
#endif
#ifdef FLECS_NO_PIPELINE
#undef FLECS_PIPELINE
#endif
#ifdef FLECS_NO_TIMER
#undef FLECS_TIMER
#endif
#ifdef FLECS_NO_META
#undef FLECS_META
#endif
#ifdef FLECS_NO_META_C
#undef FLECS_META_C
#endif
#ifdef FLECS_NO_UNITS
#undef FLECS_UNITS
#endif
#ifdef FLECS_NO_EXPR
#undef FLECS_EXPR
#endif
#ifdef FLECS_NO_JSON
#undef FLECS_JSON
#endif
#ifdef FLECS_NO_DOC
#undef FLECS_DOC
#endif
#ifdef FLECS_NO_COREDOC
#undef FLECS_COREDOC
#endif
#ifdef FLECS_NO_LOG
#undef FLECS_LOG
#endif
#ifdef FLECS_NO_APP
#undef FLECS_APP
#endif
#ifdef FLECS_NO_OS_API_IMPL
#undef FLECS_OS_API_IMPL
#endif
#ifdef FLECS_NO_HTTP
#undef FLECS_HTTP
#endif
#ifdef FLECS_NO_REST
#undef FLECS_REST
#endif

/* Always included, if disabled log functions are replaced with dummy macro's */
/**
 * @file log.h
 * @brief Logging addon.
 * 
 * The logging addon provides an API for (debug) tracing and reporting errors
 * at various levels. When enabled, the logging addon can provide more detailed
 * information about the state of the ECS and any errors that may occur.
 * 
 * The logging addon can be disabled to reduce footprint of the library, but
 * limits information logged to only file, line and error code.
 * 
 * When enabled the logging addon can be configured to exclude levels of tracing
 * from the build to reduce the impact on performance. By default all debug 
 * tracing is enabled for debug builds, tracing is enabled at release builds.
 * 
 * Applications can change the logging level at runtime with ecs_log_set_level,
 * but what is actually logged depends on what is compiled (when compiled 
 * without debug tracing, setting the runtime level to debug won't have an 
 * effect).
 * 
 * The logging addon uses the OS API log_ function for all tracing.
 * 
 * Note that even when the logging addon is not enabled, its header/source must
 * be included in a build. To prevent unused variable warnings in the code, some
 * API functions are included when the addon is disabled, but have empty bodies.
 */

#ifndef FLECS_LOG_H
#define FLECS_LOG_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef FLECS_LOG

////////////////////////////////////////////////////////////////////////////////
//// Tracing
////////////////////////////////////////////////////////////////////////////////

FLECS_API
void _ecs_deprecated(
    const char *file, 
    int32_t line, 
    const char *msg);

/** Increase log stack.
 * This operation increases the indent_ value of the OS API and can be useful to
 * make nested behavior more visible.
 * 
 * @param level The log level.
 */
FLECS_API
void _ecs_log_push(int32_t level);

/** Decrease log stack.
 * This operation decreases the indent_ value of the OS API and can be useful to
 * make nested behavior more visible.
 * 
 * @param level The log level.
 */
FLECS_API
void _ecs_log_pop(int32_t level);

/** Should current level be logged.
 * This operation returns true when the specified log level should be logged 
 * with the current log level.
 *
 * @param level The log level to check for.
 * @return Whether logging is enabled for the current level.
 */
FLECS_API
bool ecs_should_log(int32_t level);

////////////////////////////////////////////////////////////////////////////////
//// Error reporting
////////////////////////////////////////////////////////////////////////////////

/** Get description for error code */
FLECS_API
const char* ecs_strerror(
    int32_t error_code);

#else // FLECS_LOG

////////////////////////////////////////////////////////////////////////////////
//// Dummy macro's for when logging is disabled
////////////////////////////////////////////////////////////////////////////////

#define _ecs_deprecated(file, line, msg)\
    (void)file;\
    (void)line;\
    (void)msg

#define _ecs_log_push(level)
#define _ecs_log_pop(level)
#define ecs_should_log(level) false

#define ecs_strerror(error_code)\
    (void)error_code

#endif // FLECS_LOG


////////////////////////////////////////////////////////////////////////////////
//// Logging functions (do nothing when logging is enabled)
////////////////////////////////////////////////////////////////////////////////

FLECS_API
void _ecs_log(
    int32_t level,
    const char *file,
    int32_t line,
    const char *fmt,
    ...);

FLECS_API
void _ecs_logv(
    int level,
    const char *file,
    int32_t line,
    const char *fmt,
    va_list args);

FLECS_API
void _ecs_abort(
    int32_t error_code,
    const char *file,
    int32_t line,
    const char *fmt,
    ...);

FLECS_API
bool _ecs_assert(
    bool condition,
    int32_t error_code,
    const char *condition_str,
    const char *file,
    int32_t line,
    const char *fmt,
    ...);

FLECS_API
void _ecs_parser_error(
    const char *name,
    const char *expr, 
    int64_t column,
    const char *fmt,
    ...);

FLECS_API
void _ecs_parser_errorv(
    const char *name,
    const char *expr, 
    int64_t column,
    const char *fmt,
    va_list args);


////////////////////////////////////////////////////////////////////////////////
//// Logging Macro's
////////////////////////////////////////////////////////////////////////////////

#ifndef FLECS_LEGACY /* C89 doesn't support variadic macro's */

/* Base logging function. Accepts a custom level */
#define ecs_log(level, ...)\
    _ecs_log(level, __FILE__, __LINE__, __VA_ARGS__)

#define ecs_logv(level, fmt, args)\
    _ecs_logv(level, __FILE__, __LINE__, fmt, args)

/* Tracing. Used for logging of infrequent events  */
#define _ecs_trace(file, line, ...) _ecs_log(0, file, line, __VA_ARGS__)
#define ecs_trace(...) _ecs_trace(__FILE__, __LINE__, __VA_ARGS__)

/* Warning. Used when an issue occurs, but operation is successful */
#define _ecs_warn(file, line, ...) _ecs_log(-2, file, line, __VA_ARGS__)
#define ecs_warn(...) _ecs_warn(__FILE__, __LINE__, __VA_ARGS__)

/* Error. Used when an issue occurs, and operation failed. */
#define _ecs_err(file, line, ...) _ecs_log(-3, file, line, __VA_ARGS__)
#define ecs_err(...) _ecs_err(__FILE__, __LINE__, __VA_ARGS__)

/* Fatal. Used when an issue occurs, and the application cannot continue. */
#define _ecs_fatal(file, line, ...) _ecs_log(-4, file, line, __VA_ARGS__)
#define ecs_fatal(...) _ecs_fatal(__FILE__, __LINE__, __VA_ARGS__)

/* Optionally include warnings about using deprecated features */
#ifndef FLECS_NO_DEPRECATED_WARNINGS
#define ecs_deprecated(...)\
    _ecs_deprecated(__FILE__, __LINE__, __VA_ARGS__)
#else
#define ecs_deprecated(...)
#endif // FLECS_NO_DEPRECATED_WARNINGS

/* If no tracing verbosity is defined, pick default based on build config */
#if !(defined(FLECS_LOG_0) || defined(FLECS_LOG_1) || defined(FLECS_LOG_2) || defined(FLECS_LOG_3))
#if !defined(FLECS_NDEBUG)
#define FLECS_LOG_3 /* Enable all tracing in debug mode. May slow things down */
#else
#define FLECS_LOG_0 /* Only enable infrequent tracing in release mode */
#endif // !defined(FLECS_NDEBUG)
#endif // !(defined(FLECS_LOG_0) || defined(FLECS_LOG_1) || defined(FLECS_LOG_2) || defined(FLECS_LOG_3))


/* Define/undefine macro's based on compiled-in tracing level. This can optimize
 * out tracing statements from a build, which improves performance. */

#if defined(FLECS_LOG_3) /* All debug tracing enabled */
#define ecs_dbg_1(...) ecs_log(1, __VA_ARGS__);
#define ecs_dbg_2(...) ecs_log(2, __VA_ARGS__);
#define ecs_dbg_3(...) ecs_log(3, __VA_ARGS__);

#define ecs_log_push_1() _ecs_log_push(1);
#define ecs_log_push_2() _ecs_log_push(2);
#define ecs_log_push_3() _ecs_log_push(3);

#define ecs_log_pop_1() _ecs_log_pop(1);
#define ecs_log_pop_2() _ecs_log_pop(2);
#define ecs_log_pop_3() _ecs_log_pop(3);

#define ecs_should_log_1() ecs_should_log(1)
#define ecs_should_log_2() ecs_should_log(2)
#define ecs_should_log_3() ecs_should_log(3)

#define FLECS_LOG_2
#define FLECS_LOG_1
#define FLECS_LOG_0

#elif defined(FLECS_LOG_2) /* Level 2 and below debug tracing enabled */
#define ecs_dbg_1(...) ecs_log(1, __VA_ARGS__);
#define ecs_dbg_2(...) ecs_log(2, __VA_ARGS__);
#define ecs_dbg_3(...)

#define ecs_log_push_1() _ecs_log_push(1);
#define ecs_log_push_2() _ecs_log_push(2);
#define ecs_log_push_3()

#define ecs_log_pop_1() _ecs_log_pop(1);
#define ecs_log_pop_2() _ecs_log_pop(2);
#define ecs_log_pop_3()

#define ecs_should_log_1() ecs_should_log(1)
#define ecs_should_log_2() ecs_should_log(2)
#define ecs_should_log_3() false

#define FLECS_LOG_1
#define FLECS_LOG_0

#elif defined(FLECS_LOG_1) /* Level 1 debug tracing enabled */
#define ecs_dbg_1(...) ecs_log(1, __VA_ARGS__);
#define ecs_dbg_2(...)
#define ecs_dbg_3(...)

#define ecs_log_push_1() _ecs_log_push(1);
#define ecs_log_push_2()
#define ecs_log_push_3()

#define ecs_log_pop_1() _ecs_log_pop(1);
#define ecs_log_pop_2()
#define ecs_log_pop_3()

#define ecs_should_log_1() ecs_should_log(1)
#define ecs_should_log_2() false
#define ecs_should_log_3() false

#define FLECS_LOG_0

#elif defined(FLECS_LOG_0) /* No debug tracing enabled */
#define ecs_dbg_1(...)
#define ecs_dbg_2(...)
#define ecs_dbg_3(...)

#define ecs_log_push_1()
#define ecs_log_push_2()
#define ecs_log_push_3()

#define ecs_log_pop_1()
#define ecs_log_pop_2()
#define ecs_log_pop_3()

#define ecs_should_log_1() false
#define ecs_should_log_2() false
#define ecs_should_log_3() false

#else /* No tracing enabled */
#undef ecs_trace
#define ecs_trace(...)
#define ecs_dbg_1(...)
#define ecs_dbg_2(...)
#define ecs_dbg_3(...)

#define ecs_log_push_1()
#define ecs_log_push_2()
#define ecs_log_push_3()

#define ecs_log_pop_1()
#define ecs_log_pop_2()
#define ecs_log_pop_3()

#endif // defined(FLECS_LOG_3)

/* Default debug tracing is at level 1 */
#define ecs_dbg ecs_dbg_1

/* Default level for push/pop is 0 */
#define ecs_log_push() _ecs_log_push(0)
#define ecs_log_pop() _ecs_log_pop(0)

/** Abort 
 * Unconditionally aborts process. */
#define ecs_abort(error_code, ...)\
    _ecs_abort(error_code, __FILE__, __LINE__, __VA_ARGS__);\
    ecs_os_abort(); abort(); /* satisfy compiler/static analyzers */

/** Assert 
 * Aborts if condition is false, disabled in debug mode. */
#if defined(FLECS_NDEBUG) && !defined(FLECS_KEEP_ASSERT)
#define ecs_assert(condition, error_code, ...)
#else
#define ecs_assert(condition, error_code, ...)\
    if (!_ecs_assert(condition, error_code, #condition, __FILE__, __LINE__, __VA_ARGS__)) {\
        ecs_os_abort();\
    }\
    assert(condition) /* satisfy compiler/static analyzers */
#endif // FLECS_NDEBUG

/** Debug assert 
 * Assert that is only valid in debug mode (ignores FLECS_KEEP_ASSERT) */
#ifndef FLECS_NDEBUG
#define ecs_dbg_assert(condition, error_code, ...) ecs_assert(condition, error_code, __VA_ARGS__)
#else
#define ecs_dbg_assert(condition, error_code, ...)
#endif

/* Silence dead code/unused label warnings when compiling without checks. */
#define ecs_dummy_check\
    if ((false)) {\
        goto error;\
    }

/** Check
 * goto error if condition is false. */
#if defined(FLECS_NDEBUG) && !defined(FLECS_KEEP_ASSERT)
#define ecs_check(condition, error_code, ...) ecs_dummy_check
#else
#ifdef FLECS_SOFT_ASSERT
#define ecs_check(condition, error_code, ...)\
    if (!_ecs_assert(condition, error_code, #condition, __FILE__, __LINE__, __VA_ARGS__)) {\
        goto error;\
    }
#else // FLECS_SOFT_ASSERT
#define ecs_check(condition, error_code, ...)\
    ecs_assert(condition, error_code, __VA_ARGS__);\
    ecs_dummy_check
#endif
#endif // FLECS_NDEBUG

/** Panic
 * goto error when FLECS_SOFT_ASSERT is defined, otherwise abort */
#if defined(FLECS_NDEBUG) && !defined(FLECS_KEEP_ASSERT)
#define ecs_throw(error_code, ...) ecs_dummy_check
#else
#ifdef FLECS_SOFT_ASSERT
#define ecs_throw(error_code, ...)\
    _ecs_abort(error_code, __FILE__, __LINE__, __VA_ARGS__);\
    goto error;
#else
#define ecs_throw(error_code, ...)\
    ecs_abort(error_code, __VA_ARGS__);\
    ecs_dummy_check
#endif
#endif // FLECS_NDEBUG

/** Parser error */
#define ecs_parser_error(name, expr, column, ...)\
    _ecs_parser_error(name, expr, column, __VA_ARGS__)

#define ecs_parser_errorv(name, expr, column, fmt, args)\
    _ecs_parser_errorv(name, expr, column, fmt, args)

#endif // FLECS_LEGACY


////////////////////////////////////////////////////////////////////////////////
//// Functions that are always available
////////////////////////////////////////////////////////////////////////////////

/** Enable or disable tracing.
 * This will enable builtin tracing. For tracing to work, it will have to be
 * compiled in which requires defining one of the following macro's:
 *
 * FLECS_LOG_0 - All tracing is disabled
 * FLECS_LOG_1 - Enable tracing level 1
 * FLECS_LOG_2 - Enable tracing level 2 and below
 * FLECS_LOG_3 - Enable tracing level 3 and below
 *
 * If no tracing level is defined and this is a debug build, FLECS_LOG_3 will
 * have been automatically defined.
 *
 * The provided level corresponds with the tracing level. If -1 is provided as
 * value, warnings are disabled. If -2 is provided, errors are disabled as well.
 *
 * @param level Desired tracing level.
 * @return Previous tracing level.
 */
FLECS_API
int ecs_log_set_level(
    int level);

/** Enable/disable tracing with colors.
 * By default colors are enabled.
 *
 * @param enabled Whether to enable tracing with colors.
 * @return Previous color setting.
 */
FLECS_API
bool ecs_log_enable_colors(
    bool enabled);

/** Get last logged error code.
 * Calling this operation resets the error code.
 *
 * @return Last error, 0 if none was logged since last call to last_error.
 */
FLECS_API
int ecs_log_last_error(void);


////////////////////////////////////////////////////////////////////////////////
//// Error codes
////////////////////////////////////////////////////////////////////////////////

#define ECS_INVALID_OPERATION (1)
#define ECS_INVALID_PARAMETER (2)
#define ECS_CONSTRAINT_VIOLATED (3)
#define ECS_OUT_OF_MEMORY (4)
#define ECS_OUT_OF_RANGE (5)
#define ECS_UNSUPPORTED (6)
#define ECS_INTERNAL_ERROR (7)
#define ECS_ALREADY_DEFINED (8)
#define ECS_MISSING_OS_API (9)
#define ECS_OPERATION_FAILED (10)
#define ECS_INVALID_CONVERSION (11)
#define ECS_ID_IN_USE (12)
#define ECS_CYCLE_DETECTED (13)

#define ECS_INCONSISTENT_NAME (20)
#define ECS_NAME_IN_USE (21)
#define ECS_NOT_A_COMPONENT (22)
#define ECS_INVALID_COMPONENT_SIZE (23)
#define ECS_INVALID_COMPONENT_ALIGNMENT (24)
#define ECS_COMPONENT_NOT_REGISTERED (25)
#define ECS_INCONSISTENT_COMPONENT_ID (26)
#define ECS_INCONSISTENT_COMPONENT_ACTION (27)
#define ECS_MODULE_UNDEFINED (28)
#define ECS_MISSING_SYMBOL (29)
#define ECS_ALREADY_IN_USE (30)

#define ECS_COLUMN_ACCESS_VIOLATION (40)
#define ECS_COLUMN_INDEX_OUT_OF_RANGE (41)
#define ECS_COLUMN_IS_NOT_SHARED (42)
#define ECS_COLUMN_IS_SHARED (43)
#define ECS_COLUMN_TYPE_MISMATCH (45)

#define ECS_TYPE_INVALID_CASE (62)

#define ECS_INVALID_WHILE_ITERATING (70)
#define ECS_LOCKED_STORAGE (71)
#define ECS_INVALID_FROM_WORKER (72)


////////////////////////////////////////////////////////////////////////////////
//// Used when logging with colors is enabled
////////////////////////////////////////////////////////////////////////////////

#define ECS_BLACK   "\033[1;30m"
#define ECS_RED     "\033[0;31m"
#define ECS_GREEN   "\033[0;32m"
#define ECS_YELLOW  "\033[0;33m"
#define ECS_BLUE    "\033[0;34m"
#define ECS_MAGENTA "\033[0;35m"
#define ECS_CYAN    "\033[0;36m"
#define ECS_WHITE   "\033[1;37m"
#define ECS_GREY    "\033[0;37m"
#define ECS_NORMAL  "\033[0;49m"
#define ECS_BOLD    "\033[1;49m"

#ifdef __cplusplus
}
#endif

#endif // FLECS_LOG_H


#ifdef FLECS_APP
#ifdef FLECS_NO_APP
#error "FLECS_NO_APP failed: APP is required by other addons"
#endif
/**
 * @file app.h
 * @brief App addon.
 *
 * The app addon is a wrapper around the application's main loop. Its main
 * purpose is to provide a hook to modules that need to take control of the
 * main loop, as is for example the case with native applications that use
 * emscripten with webGL.
 */

#ifdef FLECS_APP

#ifndef FLECS_PIPELINE
#define FLECS_PIPELINE
#endif

#ifndef FLECS_APP_H
#define FLECS_APP_H

#ifdef __cplusplus
extern "C" {
#endif

/** Callback type for init action. */
typedef int(*ecs_app_init_action_t)(
    ecs_world_t *world);

/** Used with ecs_app_run. */
typedef struct ecs_app_desc_t {
    FLECS_FLOAT target_fps;   /* Target FPS. */
    FLECS_FLOAT delta_time;   /* Frame time increment (0 for measured values) */
    int32_t threads;          /* Number of threads. */
    bool enable_rest;         /* Allows HTTP clients to access ECS data */

    ecs_app_init_action_t init; /* If set, function is ran before starting the
                                 * main loop. */

    void *ctx;                /* Reserved for custom run/frame actions */
} ecs_app_desc_t;

/** Callback type for run action. */
typedef int(*ecs_app_run_action_t)(
    ecs_world_t *world, 
    ecs_app_desc_t *desc);

/** Callback type for frame action. */
typedef int(*ecs_app_frame_action_t)(
    ecs_world_t *world, 
    const ecs_app_desc_t *desc);

/** Run application.
 * This will run the application with the parameters specified in desc. After
 * the application quits (ecs_quit is called) the world will be cleaned up.
 * 
 * If a custom run action is set, it will be invoked by this operation. The
 * default run action calls the frame action in a loop until it returns a
 * non-zero value.
 * 
 * @param world The world.
 * @param desc Application parameters.
 */
FLECS_API
int ecs_app_run(
    ecs_world_t *world,
    ecs_app_desc_t *desc);

/** Default frame callback.
 * This operation will run a single frame. By default this operation will invoke
 * ecs_progress directly, unless a custom frame action is set.
 * 
 * @param world The world.
 * @param desc The desc struct passed to ecs_app_run.
 * @return value returned by ecs_progress
 */
FLECS_API
int ecs_app_run_frame(
    ecs_world_t *world,
    const ecs_app_desc_t *desc);

/** Set custom run action.
 * See ecs_app_run.
 * 
 * @param callback The run action.
 */
FLECS_API
int ecs_app_set_run_action(
    ecs_app_run_action_t callback);

/** Set custom frame action.
 * See ecs_app_run_frame.
 * 
 * @param callback The frame action.
 */
FLECS_API
int ecs_app_set_frame_action(
    ecs_app_frame_action_t callback);

#ifdef __cplusplus
}
#endif

#endif

#endif // FLECS_APP

#endif
#ifdef FLECS_REST
#ifdef FLECS_NO_REST
#error "FLECS_NO_REST failed: REST is required by other addons"
#endif
/**
 * @file rest.h
 * @brief REST API addon.
 *
 * A small REST API that uses the HTTP server and JSON serializer to provide
 * access to application data for remote applications.
 * 
 * A description of the API can be found in docs/RestApi.md
 */

#ifdef FLECS_REST

/* Used for the HTTP server */
#ifndef FLECS_HTTP
#define FLECS_HTTP
#endif

/* Used for building the JSON replies */
#ifndef FLECS_JSON
#define FLECS_JSON
#endif

/* Query engine used */
#ifndef FLECS_RULES
#define FLECS_RULES
#endif

/* For the REST system */
#ifndef FLECS_PIPELINE
#define FLECS_PIPELINE
#endif

#ifndef FLECS_REST_H
#define FLECS_REST_H

#ifdef __cplusplus
extern "C" {
#endif

#define ECS_REST_DEFAULT_PORT (27750)

/* Component that instantiates the REST API */
FLECS_API extern const ecs_entity_t ecs_id(EcsRest);

typedef struct {
    uint16_t port;        /* Port of server (optional, default = 27750) */
    char *ipaddr;         /* Interface address (optional, default = 0.0.0.0) */
    void *impl;
} EcsRest;

/* Module import */
FLECS_API
void FlecsRestImport(
    ecs_world_t *world);

#ifdef __cplusplus
}
#endif

#endif

#endif

#endif
#ifdef FLECS_TIMER
#ifdef FLECS_NO_TIMER
#error "FLECS_NO_TIMER failed: TIMER is required by other addons"
#endif
/**
 * @file timer.h
 * @brief Timer module.
 *
 * Timers can be used to trigger actions at periodic or one-shot intervals. They
 * are typically used together with systems and pipelines.
 */

#ifdef FLECS_TIMER

#ifndef FLECS_MODULE
#define FLECS_MODULE
#endif

#ifndef FLECS_PIPELINE
#define FLECS_PIPELINE
#endif

#ifndef FLECS_TIMER_H
#define FLECS_TIMER_H

#ifdef __cplusplus
extern "C" {
#endif


////////////////////////////////////////////////////////////////////////////////
//// Components
////////////////////////////////////////////////////////////////////////////////

/** Component used for one shot/interval timer functionality */
typedef struct EcsTimer {
    FLECS_FLOAT timeout;         /* Timer timeout period */
    FLECS_FLOAT time;            /* Incrementing time value */
    int32_t fired_count;         /* Number of times ticked */
    bool active;                 /* Is the timer active or not */
    bool single_shot;            /* Is this a single shot timer */
} EcsTimer;

/* Apply a rate filter to a tick source */
typedef struct EcsRateFilter {
    ecs_entity_t src;            /* Source of the rate filter */
    int32_t rate;                /* Rate of the rate filter */
    int32_t tick_count;          /* Number of times the rate filter ticked */
    FLECS_FLOAT time_elapsed;    /* Time elapsed since last tick */
} EcsRateFilter;


////////////////////////////////////////////////////////////////////////////////
//// Timer API
////////////////////////////////////////////////////////////////////////////////

/** Set timer timeout.
 * This operation executes any systems associated with the timer after the
 * specified timeout value. If the entity contains an existing timer, the 
 * timeout value will be reset. The timer can be started and stopped with 
 * ecs_start_timer and ecs_stop_timer.
 *
 * The timer is synchronous, and is incremented each frame by delta_time.
 *
 * The tick_source entity will be be a tick source after this operation. Tick
 * sources can be read by getting the EcsTickSource component. If the tick
 * source ticked this frame, the 'tick' member will be true. When the tick 
 * source is a system, the system will tick when the timer ticks.
 *
 * @param world The world.
 * @param tick_source The timer for which to set the timeout (0 to create one).
 * @param timeout The timeout value.
 * @return The timer entity.
 */
FLECS_API
ecs_entity_t ecs_set_timeout(
    ecs_world_t *world,
    ecs_entity_t tick_source,
    FLECS_FLOAT timeout);

/** Get current timeout value for the specified timer.
 * This operation returns the value set by ecs_set_timeout. If no timer is
 * active for this entity, the operation returns 0.
 *
 * After the timeout expires the EcsTimer component is removed from the entity.
 * This means that if ecs_get_timeout is invoked after the timer is expired, the
 * operation will return 0.
 *
 * The timer is synchronous, and is incremented each frame by delta_time.
 *
 * The tick_source entity will be be a tick source after this operation. Tick
 * sources can be read by getting the EcsTickSource component. If the tick
 * source ticked this frame, the 'tick' member will be true. When the tick 
 * source is a system, the system will tick when the timer ticks.
 *
 * @param world The world.
 * @param tick_source The timer.
 * @return The current timeout value, or 0 if no timer is active.
 */
FLECS_API
FLECS_FLOAT ecs_get_timeout(
    const ecs_world_t *world,
    ecs_entity_t tick_source);

/** Set timer interval.
 * This operation will continously invoke systems associated with the timer 
 * after the interval period expires. If the entity contains an existing timer,
 * the interval value will be reset.
 *
 * The timer is synchronous, and is incremented each frame by delta_time.
 *
 * The tick_source entity will be be a tick source after this operation. Tick
 * sources can be read by getting the EcsTickSource component. If the tick
 * source ticked this frame, the 'tick' member will be true. When the tick 
 * source is a system, the system will tick when the timer ticks. 
 *
 * @param world The world.
 * @param tick_source The timer for which to set the interval (0 to create one).
 * @param interval The interval value.
 * @return The timer entity.
 */
FLECS_API
ecs_entity_t ecs_set_interval(
    ecs_world_t *world,
    ecs_entity_t tick_source,
    FLECS_FLOAT interval);   

/** Get current interval value for the specified timer.
 * This operation returns the value set by ecs_set_interval. If the entity is
 * not a timer, the operation will return 0.
 *
 * @param world The world.
 * @param tick_source The timer for which to set the interval.
 * @return The current interval value, or 0 if no timer is active.
 */
FLECS_API
FLECS_FLOAT ecs_get_interval(
    const ecs_world_t *world,
    ecs_entity_t tick_source);

/** Start timer.
 * This operation resets the timer and starts it with the specified timeout. The
 * entity must have the EcsTimer component (added by ecs_set_timeout and 
 * ecs_set_interval). If the entity does not have the EcsTimer component this
 * operation will assert.
 *
 * @param world The world.
 * @param tick_source The timer to start.
 */
FLECS_API
void ecs_start_timer(
    ecs_world_t *world,
    ecs_entity_t tick_source);

/** Stop timer
 * This operation stops a timer from triggering. The entity must have the 
 * EcsTimer component or this operation will assert.
 *
 * @param world The world.
 * @param tick_source The timer to stop.
 */
FLECS_API
void ecs_stop_timer(
    ecs_world_t *world,
    ecs_entity_t tick_source);

/** Set rate filter.
 * This operation initializes a rate filter. Rate filters sample tick sources
 * and tick at a configurable multiple. A rate filter is a tick source itself,
 * which means that rate filters can be chained.
 *
 * Rate filters enable deterministic system execution which cannot be achieved
 * with interval timers alone. For example, if timer A has interval 2.0 and
 * timer B has interval 4.0, it is not guaranteed that B will tick at exactly
 * twice the multiple of A. This is partly due to the indeterministic nature of
 * timers, and partly due to floating point rounding errors. 
 *
 * Rate filters can be combined with timers (or other rate filters) to ensure 
 * that a system ticks at an exact multiple of a tick source (which can be
 * another system). If a rate filter is created with a rate of 1 it will tick
 * at the exact same time as its source.
 *
 * If no tick source is provided, the rate filter will use the frame tick as
 * source, which corresponds with the number of times ecs_progress is called.
 *
 * The tick_source entity will be be a tick source after this operation. Tick
 * sources can be read by getting the EcsTickSource component. If the tick
 * source ticked this frame, the 'tick' member will be true. When the tick 
 * source is a system, the system will tick when the timer ticks.  
 *
 * @param world The world.
 * @param tick_source The rate filter entity (0 to create one).
 * @param rate The rate to apply.
 * @param source The tick source (0 to use frames)
 * @return The filter entity.
 */
FLECS_API
ecs_entity_t ecs_set_rate(
    ecs_world_t *world,
    ecs_entity_t tick_source,
    int32_t rate,
    ecs_entity_t source);

/** Assign tick source to system.
 * Systems can be their own tick source, which can be any of the tick sources
 * (one shot timers, interval times and rate filters). However, in some cases it
 * is must be guaranteed that different systems tick on the exact same frame.
 * 
 * This cannot be guaranteed by giving two systems the same interval/rate filter
 * as it is possible that one system is (for example) disabled, which would
 * cause the systems to go out of sync. To provide these guarantees, systems
 * must use the same tick source, which is what this operation enables.
 *
 * When two systems share the same tick source, it is guaranteed that they tick
 * in the same frame. The provided tick source can be any entity that is a tick
 * source, including another system. If the provided entity is not a tick source
 * the system will not be ran.
 *
 * To disassociate a tick source from a system, use 0 for the tick_source 
 * parameter.
 *
 * @param world The world.
 * @param system The system to associate with the timer.
 * @param tick_source The tick source to associate with the system.
 */ 
FLECS_API
void ecs_set_tick_source(
    ecs_world_t *world,
    ecs_entity_t system,
    ecs_entity_t tick_source);


////////////////////////////////////////////////////////////////////////////////
//// Module
////////////////////////////////////////////////////////////////////////////////

FLECS_API
void FlecsTimerImport(
    ecs_world_t *world);

#ifdef __cplusplus
}
#endif

#endif

#endif

#endif
#ifdef FLECS_PIPELINE
#ifdef FLECS_NO_PIPELINE
#error "FLECS_NO_PIPELINE failed: PIPELINE is required by other addons"
#endif
/**
 * @file pipeline.h
 * @brief Pipeline module.
 *
 * The pipeline module provides support for running systems automatically and
 * on multiple threads. A pipeline is a collection of tags that can be added to
 * systems. When ran, a pipeline will query for all systems that have the tags
 * that belong to a pipeline, and run them.
 *
 * The module defines a number of builtin tags (EcsPreUpdate, EcsOnUpdate, 
 * EcsPostUpdate etc.) that are registered with the builtin pipeline. The 
 * builtin pipeline is ran by default when calling ecs_progress(). An 
 * application can set a custom pipeline with the ecs_set_pipeline function.
 */

#ifdef FLECS_PIPELINE

#ifndef FLECS_MODULE
#define FLECS_MODULE
#endif

#ifndef FLECS_SYSTEM
#define FLECS_SYSTEM
#endif

#if !defined(FLECS_OS_API_IMPL) && !defined(FLECS_NO_OS_API_IMPL)
#define FLECS_OS_API_IMPL
#endif

#ifndef FLECS_PIPELINE_H
#define FLECS_PIPELINE_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef FLECS_LEGACY
#define ECS_PIPELINE_DEFINE(world, id, ...)\
    id = ecs_type_init(world, &(ecs_type_desc_t){\
        .entity = {\
            .name = #id,\
            .add = {EcsPipeline}\
        },\
        .ids_expr = #__VA_ARGS__\
    });\
    ecs_id(id) = id;

#define ECS_PIPELINE(world, id, ...) \
    ecs_entity_t ecs_id(id), ECS_PIPELINE_DEFINE(world, id, __VA_ARGS__);\
    (void)id;\
    (void)ecs_id(id)
    
#endif

/** Set a custom pipeline.
 * This operation sets the pipeline to run when ecs_progress is invoked.
 *
 * @param world The world.
 * @param pipeline The pipeline to set.
 */
FLECS_API
void ecs_set_pipeline(
    ecs_world_t *world,
    ecs_entity_t pipeline);       

/** Get the current pipeline.
 * This operation gets the current pipeline.
 *
 * @param world The world.
 * @return The current pipeline.
 */
FLECS_API
ecs_entity_t ecs_get_pipeline(
    const ecs_world_t *world);  

/** Progress a world.
 * This operation progresses the world by running all systems that are both
 * enabled and periodic on their matching entities.
 *
 * An application can pass a delta_time into the function, which is the time
 * passed since the last frame. This value is passed to systems so they can
 * update entity values proportional to the elapsed time since their last
 * invocation.
 *
 * When an application passes 0 to delta_time, ecs_progress will automatically
 * measure the time passed since the last frame. If an application does not uses
 * time management, it should pass a non-zero value for delta_time (1.0 is
 * recommended). That way, no time will be wasted measuring the time.
 *
 * @param world The world to progress.
 * @param delta_time The time passed since the last frame.
 * @return false if ecs_quit has been called, true otherwise.
 */
FLECS_API
bool ecs_progress(
    ecs_world_t *world,
    FLECS_FLOAT delta_time);   

/** Set time scale.
 * Increase or decrease simulation speed by the provided multiplier.
 *
 * @param world The world.
 * @param scale The scale to apply (default = 1).
 */
FLECS_API 
void ecs_set_time_scale(
    ecs_world_t *world,
    FLECS_FLOAT scale);

/** Reset world clock.
 * Reset the clock that keeps track of the total time passed in the simulation.
 *
 * @param world The world.
 */
FLECS_API
void ecs_reset_clock(
    ecs_world_t *world);

/** Run pipeline.
 * This will run all systems in the provided pipeline. This operation may be
 * invoked from multiple threads, and only when staging is disabled, as the
 * pipeline manages staging and, if necessary, synchronization between threads.
 *
 * If 0 is provided for the pipeline id, the default pipeline will be ran (this
 * is either the builtin pipeline or the pipeline set with set_pipeline()). 
 *
 * When using progress() this operation will be invoked automatically for the
 * default pipeline (either the builtin pipeline or the pipeline set with 
 * set_pipeline()). An application may run additional pipelines.
 *
 * Note: calling this function from an application currently only works in
 * single threaded applications with a single stage.
 *
 * @param world The world.
 * @param pipeline The pipeline to run.
 */
FLECS_API 
void ecs_run_pipeline(
    ecs_world_t *world,
    ecs_entity_t pipeline,
    FLECS_FLOAT delta_time);    
    
/** Deactivate systems that are not matched with tables.
 * By default Flecs deactivates systems that are not matched with any tables.
 * However, once a system has been matched with a table it remains activated, to
 * prevent systems from continuously becoming active and inactive.
 *
 * To re-deactivate systems, an application can invoke this function, which will
 * deactivate all systems that are not matched with any tables.
 *
 * @param world The world.
 */
FLECS_API
void ecs_deactivate_systems(
    ecs_world_t *world);


////////////////////////////////////////////////////////////////////////////////
//// Threading
////////////////////////////////////////////////////////////////////////////////

/** Set number of worker threads.
 * Setting this value to a value higher than 1 will start as many threads and
 * will cause systems to evenly distribute matched entities across threads. The
 * operation may be called multiple times to reconfigure the number of threads
 * used, but never while running a system / pipeline. */
FLECS_API
void ecs_set_threads(
    ecs_world_t *world,
    int32_t threads);

////////////////////////////////////////////////////////////////////////////////
//// Module
////////////////////////////////////////////////////////////////////////////////

FLECS_API
void FlecsPipelineImport(
    ecs_world_t *world);

#ifdef __cplusplus
}
#endif

#endif

#endif

#endif
#ifdef FLECS_SYSTEM
#ifdef FLECS_NO_SYSTEM
#error "FLECS_NO_SYSTEM failed: SYSTEM is required by other addons"
#endif
/**
 * @file system.h
 * @brief System module.
 *
 * The system module allows for creating and running systems. A system is a
 * query in combination with a callback function. In addition systems have
 * support for time management and can be monitored by the stats addon.
 */

#ifdef FLECS_SYSTEM

#ifndef FLECS_MODULE
#define FLECS_MODULE
#endif

#ifndef FLECS_SYSTEM_H
#define FLECS_SYSTEM_H

#ifdef __cplusplus
extern "C" {
#endif


////////////////////////////////////////////////////////////////////////////////
//// Components
////////////////////////////////////////////////////////////////////////////////

/* Component used to provide a tick source to systems */
typedef struct EcsTickSource {
    bool tick;                 /* True if providing tick */
    FLECS_FLOAT time_elapsed;  /* Time elapsed since last tick */
} EcsTickSource;

////////////////////////////////////////////////////////////////////////////////
//// Systems API
////////////////////////////////////////////////////////////////////////////////

/** System status change callback */
typedef enum ecs_system_status_t {
    EcsSystemStatusNone = 0,
    EcsSystemEnabled,
    EcsSystemDisabled,
    EcsSystemActivated,
    EcsSystemDeactivated
} ecs_system_status_t;

/** System status action.
 * The status action is invoked whenever a system is enabled or disabled. Note
 * that a system may be enabled but may not actually match any entities. In this
 * case the system is enabled but not _active_.
 *
 * In addition to communicating the enabled / disabled status, the action also
 * communicates changes in the activation status of the system. A system becomes
 * active when it has one or more matching entities, and becomes inactive when
 * it no longer matches any entities.
 * 
 * A system switches between enabled and disabled when an application invokes the
 * ecs_enable operation with a state different from the state of the system, for
 * example the system is disabled, and ecs_enable is invoked with enabled: true.
 *
 * @param world The world.
 * @param system The system for which the action has changed.
 * @param status The status that triggered the callback.
 * @param ctx Context passed to ecs_system_desc_t as status_ctx.
 */
typedef void (*ecs_system_status_action_t)(
    ecs_world_t *world,
    ecs_entity_t system,
    ecs_system_status_t status,
    void *ctx);

/* Use with ecs_system_init */
typedef struct ecs_system_desc_t {
    int32_t _canary;

    /* System entity creation parameters */
    ecs_entity_desc_t entity;

    /* System query parameters */
    ecs_query_desc_t query;

    /* Callback that is invoked when a system is ran. When left to NULL, the
     * default system runner is used, which calls the "callback" action for each
     * result returned from the system's query. 
     * 
     * It should not be assumed that the input iterator can always be iterated
     * with ecs_query_next. When a system is multithreaded and/or paged, the
     * iterator can be either a worker or paged iterator. Future use cases may
     * introduce additional inputs for a system, such as rules and filters. The
     * correct function to use for iteration is ecs_iter_next.
     * 
     * An implementation can test whether the iterator is a query iterator by
     * testing whether the it->next value is equal to ecs_query_next. */
    ecs_run_action_t run;

    /* Callback that is ran for each result returned by the system's query. This
     * means that this callback can be invoked multiple times per system per
     * frame, typically once for each matching table. */
    ecs_iter_action_t callback;

    /* System status callback, invoked when system status changes */
    ecs_system_status_action_t status_callback;

    /* Associate with entity. */
    ecs_entity_t self;    

    /* Context to be passed to callback (as ecs_iter_t::param) */
    void *ctx;

    /* Context to be passed to system status callback */
    void *status_ctx;

    /* Binding context, for when system is implemented in other language */
    void *binding_ctx;

    /* Functions that are invoked during system cleanup to free context data.
     * When set, functions are called unconditionally, even when the ctx
     * pointers are NULL. */
    ecs_ctx_free_t ctx_free;
    ecs_ctx_free_t status_ctx_free;
    ecs_ctx_free_t binding_ctx_free;   

    /* Interval in seconds at which the system should run */
    FLECS_FLOAT interval;

    /* Rate at which the system should run */
    int32_t rate;

    /* External tick soutce that determines when system ticks */
    ecs_entity_t tick_source;

    /* If true, system will be ran on multiple threads */
    bool multi_threaded;

    /* If true, system will have access to actuall world. Cannot be true at the
     * same time as multi_threaded. */
    bool no_staging;
} ecs_system_desc_t;

/* Create a system */
FLECS_API
ecs_entity_t ecs_system_init(
    ecs_world_t *world,
    const ecs_system_desc_t *desc);

#ifndef FLECS_LEGACY
#define ECS_SYSTEM_DEFINE(world, id, kind, ...) \
    { \
        ecs_system_desc_t desc = {0}; \
        desc.entity.name = #id; \
        desc.entity.add[0] = kind; \
        desc.query.filter.expr = #__VA_ARGS__; \
        desc.callback = id; \
        ecs_id(id) = ecs_system_init(world, &desc); \
    } \
    ecs_assert(ecs_id(id) != 0, ECS_INVALID_PARAMETER, NULL);

#define ECS_SYSTEM(world, id, kind, ...) \
    ecs_entity_t ecs_id(id); ECS_SYSTEM_DEFINE(world, id, kind, __VA_ARGS__);\
    ecs_entity_t id = ecs_id(id);\
    (void)ecs_id(id);\
    (void)id;
#endif

/** Run a specific system manually.
 * This operation runs a single system manually. It is an efficient way to
 * invoke logic on a set of entities, as manual systems are only matched to
 * tables at creation time or after creation time, when a new table is created.
 *
 * Manual systems are useful to evaluate lists of prematched entities at
 * application defined times. Because none of the matching logic is evaluated
 * before the system is invoked, manual systems are much more efficient than
 * manually obtaining a list of entities and retrieving their components.
 *
 * An application may pass custom data to a system through the param parameter.
 * This data can be accessed by the system through the param member in the
 * ecs_iter_t value that is passed to the system callback.
 *
 * Any system may interrupt execution by setting the interrupted_by member in
 * the ecs_iter_t value. This is particularly useful for manual systems, where
 * the value of interrupted_by is returned by this operation. This, in
 * cominbation with the param argument lets applications use manual systems
 * to lookup entities: once the entity has been found its handle is passed to
 * interrupted_by, which is then subsequently returned.
 *
 * @param world The world.
 * @param system The system to run.
 * @param delta_time The time passed since the last system invocation.
 * @param param A user-defined parameter to pass to the system.
 * @return handle to last evaluated entity if system was interrupted.
 */
FLECS_API
ecs_entity_t ecs_run(
    ecs_world_t *world,
    ecs_entity_t system,
    FLECS_FLOAT delta_time,
    void *param);

/** Same as ecs_run, but subdivides entities across number of provided stages.
 * 
 * @param world The world.
 * @param system The system to run.
 * @param stage_current The id of the current stage.
 * @param stage_count The total number of stages.
 * @param delta_time The time passed since the last system invocation.
 * @param param A user-defined parameter to pass to the system.
 * @return handle to last evaluated entity if system was interrupted.
 */
FLECS_API
ecs_entity_t ecs_run_worker(
    ecs_world_t *world,
    ecs_entity_t system,
    int32_t stage_current,
    int32_t stage_count,
    FLECS_FLOAT delta_time,
    void *param);

/** Run system with offset/limit and type filter.
 * This operation is the same as ecs_run, but filters the entities that will be
 * iterated by the system.
 * 
 * Entities can be filtered in two ways. Offset and limit control the range of
 * entities that is iterated over. The range is applied to all entities matched
 * with the system, thus may cover multiple archetypes.
 * 
 * The type filter controls which entity types the system will evaluate. Only
 * types that contain all components in the type filter will be iterated over. A
 * type filter is only evaluated once per table, which makes filtering cheap if
 * the number of entities is large and the number of tables is small, but not as
 * cheap as filtering in the system signature.
 * 
 * @param world The world.
 * @param system The system to invoke.
 * @param delta_time The time passed since the last system invocation.
 * @param param A user-defined parameter to pass to the system.
 * @return handle to last evaluated entity if system was interrupted.
 */
FLECS_API
ecs_entity_t ecs_run_w_filter(
    ecs_world_t *world,
    ecs_entity_t system,
    FLECS_FLOAT delta_time,
    int32_t offset,
    int32_t limit,
    void *param);

/** Get the query object for a system.
 * Systems use queries under the hood. This enables an application to get access
 * to the underlying query object of a system. This can be useful when, for 
 * example, an application needs to enable sorting for a system.
 *
 * @param world The world.
 * @param system The system from which to obtain the query.
 * @return The query.
 */
FLECS_API
ecs_query_t* ecs_system_get_query(
    const ecs_world_t *world,
    ecs_entity_t system);

/** Get system context.
 * This operation returns the context pointer set for the system. If
 * the provided entity is not a system, the function will return NULL.
 *
 * @param world The world.
 * @param system The system from which to obtain the context.
 * @return The context.
 */
FLECS_API
void* ecs_get_system_ctx(
    const ecs_world_t *world,
    ecs_entity_t system);

/** Get system binding context.
 * The binding context is a context typically used to attach any language 
 * binding specific data that is needed when invoking a callback that is 
 * implemented in another language.
 *
 * @param world The world.
 * @param system The system from which to obtain the context.
 * @return The context.
 */
FLECS_API
void* ecs_get_system_binding_ctx(
    const ecs_world_t *world,
    ecs_entity_t system);    


////////////////////////////////////////////////////////////////////////////////
//// Module
////////////////////////////////////////////////////////////////////////////////

FLECS_API
void FlecsSystemImport(
    ecs_world_t *world);

#ifdef __cplusplus
}
#endif

#endif

#endif

#endif
#ifdef FLECS_COREDOC
#ifdef FLECS_NO_COREDOC
#error "FLECS_NO_COREDOC failed: COREDOC is required by other addons"
#endif
/**
 * @file coredoc.h
 * @brief Core doc module.
 *
 * The core doc module imports documentation and reflection data for core
 * components, tags and systems.
 */

#ifdef FLECS_COREDOC

#ifndef FLECS_DOC
#define FLECS_DOC
#endif

#ifndef FLECS_META
#define FLECS_META
#endif

#ifndef FLECS_COREDOC_H
#define FLECS_COREDOC_H

#ifdef __cplusplus
extern "C" {
#endif

/* Module import */

FLECS_API
void FlecsCoreDocImport(
    ecs_world_t *world);

#ifdef __cplusplus
}
#endif

#endif

#endif

#endif
#ifdef FLECS_DOC
#ifdef FLECS_NO_DOC
#error "FLECS_NO_DOC failed: DOC is required by other addons"
#endif
/**
 * @file doc.h
 * @brief Doc module.
 *
 * The doc module allows for documenting entities (and thus components, systems)
 * by adding brief and/or detailed descriptions as components. Documentation
 * added with the doc module can be retrieved at runtime, and can be used by
 * tooling such as UIs or documentation frameworks.
 */

#ifdef FLECS_DOC

#ifndef FLECS_DOC_H
#define FLECS_DOC_H

#ifndef FLECS_MODULE
#define FLECS_MODULE
#endif

#ifdef __cplusplus
extern "C" {
#endif

FLECS_API extern const ecs_entity_t ecs_id(EcsDocDescription);
FLECS_API extern const ecs_entity_t EcsDocBrief;
FLECS_API extern const ecs_entity_t EcsDocDetail;
FLECS_API extern const ecs_entity_t EcsDocLink;

typedef struct EcsDocDescription {
    const char *value;
} EcsDocDescription;

/** Add human-readable name to entity.
 * Contrary to entity names, human readable names do not have to be unique and
 * can contain special characters used in the query language like '*'.
 * 
 * @param world The world.
 * @param entity The entity to which to add the name.
 * @param name The name to add.
 */
FLECS_API
void ecs_doc_set_name(
    ecs_world_t *world,
    ecs_entity_t entity,
    const char *name);

/** Add brief description to entity.
 * 
 * @param world The world.
 * @param entity The entity to which to add the description.
 * @param description The description to add.
 */
FLECS_API
void ecs_doc_set_brief(
    ecs_world_t *world,
    ecs_entity_t entity,
    const char *description);

/** Add detailed description to entity.
 * 
 * @param world The world.
 * @param entity The entity to which to add the description.
 * @param description The description to add.
 */
FLECS_API
void ecs_doc_set_detail(
    ecs_world_t *world,
    ecs_entity_t entity,
    const char *description);

/** Add link to external documentation to entity.
 * 
 * @param world The world.
 * @param entity The entity to which to add the link.
 * @param link The link to add.
 */
FLECS_API
void ecs_doc_set_link(
    ecs_world_t *world,
    ecs_entity_t entity,
    const char *link);

/** Get human readable name from entity.
 * If entity does not have an explicit human readable name, this operation will
 * return the entity name.
 * 
 * To test if an entity has a human readable name, use:
 *   ecs_has_pair(world, e, ecs_id(EcsDescription), EcsName);
 * Or in C++:
 *   e.has<flecs::Description>(flecs::Name);
 * 
 * @param world The world.
 * @param entity The entity from which to get the name.
 * @return The name.
 */
FLECS_API
const char* ecs_doc_get_name(
    const ecs_world_t *world,
    ecs_entity_t entity);

/** Get brief description from entity.
 * 
 * @param world The world.
 * @param entity The entity from which to get the description.
 * @return The description.
 */
FLECS_API
const char* ecs_doc_get_brief(
    const ecs_world_t *world,
    ecs_entity_t entity);

/** Get detailed description from entity.
 * 
 * @param world The world.
 * @param entity The entity from which to get the description.
 * @return The description.
 */
FLECS_API
const char* ecs_doc_get_detail(
    const ecs_world_t *world,
    ecs_entity_t entity);

/** Get link to external documentation from entity.
 * 
 * @param world The world.
 * @param entity The entity from which to get the link.
 * @return The link.
 */
FLECS_API
const char* ecs_doc_get_link(
    const ecs_world_t *world,
    ecs_entity_t entity);

/* Module import */
FLECS_API
void FlecsDocImport(
    ecs_world_t *world);

#ifdef __cplusplus
}
#endif

#endif

#endif

#endif
#ifdef FLECS_JSON
#ifdef FLECS_NO_JSON
#error "FLECS_NO_JSON failed: JSON is required by other addons"
#endif
/**
 * @file json.h
 * @brief JSON parser addon.
 *
 * Parse expression strings into component values. Entity identifiers, 
 * enumerations and bitmasks are encoded as strings.
 * 
 * See docs/JsonFormat.md for a description of the JSON format.
 */

#ifdef FLECS_JSON

#ifndef FLECS_EXPR
#define FLECS_EXPR
#endif

#ifndef FLECS_JSON_H
#define FLECS_JSON_H

#ifdef __cplusplus
extern "C" {
#endif

/** Used with ecs_parse_json. */
typedef struct ecs_parse_json_desc_t {
    const char *name; /* Name of expression (used for logging) */
    const char *expr; /* Full expression (used for logging) */
} ecs_parse_json_desc_t;

/** Parse JSON string into value.
 * This operation parses a JSON expression into the provided pointer. The
 * memory pointed to must be large enough to contain a value of the used type.
 * 
 * @param world The world.
 * @param ptr The pointer to the expression to parse.
 * @param type The type of the expression to parse.
 * @param data_out Pointer to the memory to write to.
 * @param desc Configuration parameters for deserializer.
 * @return Pointer to the character after the last one read, or NULL if failed.
 */
FLECS_API
const char* ecs_parse_json(
    const ecs_world_t *world,
    const char *ptr,
    ecs_entity_t type,
    void *data_out,
    const ecs_parse_json_desc_t *desc);

/** Serialize value into JSON string.
 * This operation serializes a value of the provided type to a JSON string. The 
 * memory pointed to must be large enough to contain a value of the used type.
 * 
 * If count is 0, the function will serialize a single value, not wrapped in
 * array brackets. If count is >= 1, the operation will serialize values to a
 * a comma-separated list inside of array brackets.
 * 
 * @param world The world.
 * @param type The type of the value to serialize.
 * @param data The value to serialize.
 * @param count The number of elements to serialize.
 * @return String with JSON expression, or NULL if failed.
 */
FLECS_API
char* ecs_array_to_json(
    const ecs_world_t *world,
    ecs_entity_t type,
    const void *data,
    int32_t count);

/** Serialize value into JSON string buffer.
 * Same as ecs_array_to_json_buf, but serializes to an ecs_strbuf_t instance.
 * 
 * @param world The world.
 * @param type The type of the value to serialize.
 * @param data The value to serialize.
 * @param count The number of elements to serialize.
 * @param buf_out The strbuf to append the string to.
 * @return Zero if success, non-zero if failed.
 */
FLECS_API
int ecs_array_to_json_buf(
    const ecs_world_t *world,
    ecs_entity_t type,
    const void *data,
    int32_t count,
    ecs_strbuf_t *buf_out);

/** Serialize value into JSON string.
 * Same as ecs_array_to_json, with count = 0.
 * 
 * @param world The world.
 * @param type The type of the value to serialize.
 * @param data The value to serialize.
 * @return String with JSON expression, or NULL if failed.
 */
FLECS_API
char* ecs_ptr_to_json(
    const ecs_world_t *world,
    ecs_entity_t type,
    const void *data);

/** Serialize value into JSON string buffer.
 * Same as ecs_ptr_to_json, but serializes to an ecs_strbuf_t instance.
 * 
 * @param world The world.
 * @param type The type of the value to serialize.
 * @param data The value to serialize.
 * @param buf_out The strbuf to append the string to.
 * @return Zero if success, non-zero if failed.
 */
FLECS_API
int ecs_ptr_to_json_buf(
    const ecs_world_t *world,
    ecs_entity_t type,
    const void *data,
    ecs_strbuf_t *buf_out);

/** Serialize type info to JSON.
 * This serializes type information to JSON, and can be used to store/transmit
 * the structure of a (component) value.
 * 
 * If the provided type does not have reflection data, "0" will be returned.
 * 
 * @param world The world.
 * @param type The type to serialize to JSON.
 * @return A JSON string with the serialized type info, or NULL if failed.
 */
FLECS_API
char* ecs_type_info_to_json(
    const ecs_world_t *world,
    ecs_entity_t type);

/** Serialize type info into JSON string buffer.
 * Same as ecs_type_info_to_json, but serializes to an ecs_strbuf_t instance.
 * 
 * @param world The world.
 * @param type The type to serialize.
 * @param buf_out The strbuf to append the string to.
 * @return Zero if success, non-zero if failed.
 */
FLECS_API
int ecs_type_info_to_json_buf(
    const ecs_world_t *world,
    ecs_entity_t type,
    ecs_strbuf_t *buf_out);

/** Used with ecs_iter_to_json. */
typedef struct ecs_entity_to_json_desc_t {
    bool serialize_path;       /* Serialize full pathname */
    bool serialize_meta_ids;   /* Serialize 'meta' ids (Name, ChildOf, etc) */
    bool serialize_label;      /* Serialize doc name */
    bool serialize_brief;      /* Serialize brief doc description */
    bool serialize_link;       /* Serialize doc link (URL) */
    bool serialize_id_labels;  /* Serialize labels of (component) ids */
    bool serialize_base;       /* Serialize base components */
    bool serialize_private;    /* Serialize private components */
    bool serialize_hidden;     /* Serialize ids hidden by override */
    bool serialize_values;     /* Serialize component values */
    bool serialize_type_info;  /* Serialize type info (requires serialize_values) */
} ecs_entity_to_json_desc_t;

#define ECS_ENTITY_TO_JSON_INIT (ecs_entity_to_json_desc_t) {\
    true, false, false, false, false, false, true, false, false, false, false }

/** Serialize entity into JSON string.
 * This creates a JSON object with the entity's (path) name, which components
 * and tags the entity has, and the component values.
 * 
 * The operation may fail if the entity contains components with invalid values.
 * 
 * @param world The world.
 * @param entity The entity to serialize to JSON.
 * @return A JSON string with the serialized entity data, or NULL if failed.
 */
FLECS_API
char* ecs_entity_to_json(
    const ecs_world_t *world,
    ecs_entity_t entity,
    const ecs_entity_to_json_desc_t *desc);

/** Serialize entity into JSON string buffer.
 * Same as ecs_entity_to_json, but serializes to an ecs_strbuf_t instance.
 * 
 * @param world The world.
 * @param entity The entity to serialize.
 * @param buf_out The strbuf to append the string to.
 * @return Zero if success, non-zero if failed.
 */
FLECS_API
int ecs_entity_to_json_buf(
    const ecs_world_t *world,
    ecs_entity_t entity,
    ecs_strbuf_t *buf_out,
    const ecs_entity_to_json_desc_t *desc);

/** Used with ecs_iter_to_json. */
typedef struct ecs_iter_to_json_desc_t {
    bool serialize_term_ids;    /* Include term (query) component ids */
    bool serialize_ids;         /* Include actual (matched) component ids */
    bool serialize_subjects;    /* Include subjects */
    bool serialize_variables;   /* Include variables */
    bool serialize_is_set;      /* Include is_set (for optional terms) */
    bool serialize_values;      /* Include component values */
    bool serialize_entities;    /* Include entities (for This terms) */
    bool serialize_entity_labels; /* Include doc name for entities */
    bool serialize_variable_labels; /* Include doc name for variables */
    bool measure_eval_duration; /* Include evaluation duration */
    bool serialize_type_info;   /* Include type information */
} ecs_iter_to_json_desc_t;

#define ECS_ITER_TO_JSON_INIT (ecs_iter_to_json_desc_t) {\
    true, true, true, true, true, true, true, false, false, false, false }

/** Serialize iterator into JSON string.
 * This operation will iterate the contents of the iterator and serialize them
 * to JSON. The function acccepts iterators from any source.
 * 
 * @param world The world.
 * @param iter The iterator to serialize to JSON.
 * @return A JSON string with the serialized iterator data, or NULL if failed.
 */
FLECS_API
char* ecs_iter_to_json(
    const ecs_world_t *world,
    ecs_iter_t *iter,
    const ecs_iter_to_json_desc_t *desc);

/** Serialize iterator into JSON string buffer.
 * Same as ecs_iter_to_json, but serializes to an ecs_strbuf_t instance.
 * 
 * @param world The world.
 * @param iter The iterator to serialize.
 * @param buf_out The strbuf to append the string to.
 * @return Zero if success, non-zero if failed.
 */
FLECS_API
int ecs_iter_to_json_buf(
    const ecs_world_t *world,
    ecs_iter_t *iter,
    ecs_strbuf_t *buf_out,
    const ecs_iter_to_json_desc_t *desc);

#ifdef __cplusplus
}
#endif

#endif

#endif

#endif
#if defined(FLECS_EXPR) || defined(FLECS_META_C)
#define FLECS_META
#endif
#ifdef FLECS_UNITS
#ifdef FLECS_NO_UNITS
#error "FLECS_NO_UNITS failed: UNITS is required by other addons"
#endif
/**
 * @file units.h
 * @brief Units module.
 *
 * Builtin standard units. The units addon is not imported by default, even if
 * the addon is included in the build. To import the module, do:
 *
 * In C:
 *   ECS_IMPORT(world, FlecsUnits);
 * 
 * In C++:
 *   world.import<flecs::units>();
 *
 * As a result this module behaves just like an application-defined module, 
 * which means that the ids generated for the entities inside the module are not
 * fixed, and depend on the order in which the module is imported.
 */

#ifdef FLECS_UNITS

#ifndef FLECS_MODULE
#define FLECS_MODULE
#endif

#ifndef FLECS_META
#define FLECS_META
#endif

#ifndef FLECS_UNITS_H
#define FLECS_UNITS_H

#ifdef __cplusplus
extern "C" {
#endif

////////////////////////////////////////////////////////////////////////////////
//// Unit prefixes
////////////////////////////////////////////////////////////////////////////////

FLECS_API extern ECS_DECLARE(EcsUnitPrefixes); /* Parent scope for prefixes */

FLECS_API extern ECS_DECLARE(EcsYocto);
FLECS_API extern ECS_DECLARE(EcsZepto);
FLECS_API extern ECS_DECLARE(EcsAtto);
FLECS_API extern ECS_DECLARE(EcsFemto);
FLECS_API extern ECS_DECLARE(EcsPico);
FLECS_API extern ECS_DECLARE(EcsNano);
FLECS_API extern ECS_DECLARE(EcsMicro);
FLECS_API extern ECS_DECLARE(EcsMilli);
FLECS_API extern ECS_DECLARE(EcsCenti);
FLECS_API extern ECS_DECLARE(EcsDeci);
FLECS_API extern ECS_DECLARE(EcsDeca);
FLECS_API extern ECS_DECLARE(EcsHecto);
FLECS_API extern ECS_DECLARE(EcsKilo);
FLECS_API extern ECS_DECLARE(EcsMega);
FLECS_API extern ECS_DECLARE(EcsGiga);
FLECS_API extern ECS_DECLARE(EcsTera);
FLECS_API extern ECS_DECLARE(EcsPeta);
FLECS_API extern ECS_DECLARE(EcsExa);
FLECS_API extern ECS_DECLARE(EcsZetta);
FLECS_API extern ECS_DECLARE(EcsYotta);

FLECS_API extern ECS_DECLARE(EcsKibi);
FLECS_API extern ECS_DECLARE(EcsMebi);
FLECS_API extern ECS_DECLARE(EcsGibi);
FLECS_API extern ECS_DECLARE(EcsTebi);
FLECS_API extern ECS_DECLARE(EcsPebi);
FLECS_API extern ECS_DECLARE(EcsExbi);
FLECS_API extern ECS_DECLARE(EcsZebi);
FLECS_API extern ECS_DECLARE(EcsYobi);


////////////////////////////////////////////////////////////////////////////////
//// Units & quantities
////////////////////////////////////////////////////////////////////////////////

FLECS_API extern ECS_DECLARE(EcsDuration);
FLECS_API extern     ECS_DECLARE(EcsPicoSeconds);
FLECS_API extern     ECS_DECLARE(EcsNanoSeconds);
FLECS_API extern     ECS_DECLARE(EcsMicroSeconds);
FLECS_API extern     ECS_DECLARE(EcsMilliSeconds);
FLECS_API extern     ECS_DECLARE(EcsSeconds);
FLECS_API extern     ECS_DECLARE(EcsMinutes);
FLECS_API extern     ECS_DECLARE(EcsHours);
FLECS_API extern     ECS_DECLARE(EcsDays);

FLECS_API extern ECS_DECLARE(EcsTime);
FLECS_API extern     ECS_DECLARE(EcsDate);

FLECS_API extern ECS_DECLARE(EcsMass);
FLECS_API extern     ECS_DECLARE(EcsGrams);
FLECS_API extern     ECS_DECLARE(EcsKiloGrams);

FLECS_API extern ECS_DECLARE(EcsElectricCurrent);
FLECS_API extern     ECS_DECLARE(EcsAmpere);

FLECS_API extern ECS_DECLARE(EcsAmount);
FLECS_API extern     ECS_DECLARE(EcsMole);

FLECS_API extern ECS_DECLARE(EcsLuminousIntensity);
FLECS_API extern     ECS_DECLARE(EcsCandela);

FLECS_API extern ECS_DECLARE(EcsForce);
FLECS_API extern     ECS_DECLARE(EcsNewton);

FLECS_API extern ECS_DECLARE(EcsLength);
FLECS_API extern     ECS_DECLARE(EcsMeters);
FLECS_API extern         ECS_DECLARE(EcsPicoMeters);
FLECS_API extern         ECS_DECLARE(EcsNanoMeters);
FLECS_API extern         ECS_DECLARE(EcsMicroMeters);
FLECS_API extern         ECS_DECLARE(EcsMilliMeters);
FLECS_API extern         ECS_DECLARE(EcsCentiMeters);
FLECS_API extern         ECS_DECLARE(EcsKiloMeters);
FLECS_API extern     ECS_DECLARE(EcsMiles);

FLECS_API extern ECS_DECLARE(EcsPressure);
FLECS_API extern     ECS_DECLARE(EcsPascal);
FLECS_API extern     ECS_DECLARE(EcsBar);

FLECS_API extern ECS_DECLARE(EcsSpeed);
FLECS_API extern     ECS_DECLARE(EcsMetersPerSecond);
FLECS_API extern     ECS_DECLARE(EcsKiloMetersPerSecond);
FLECS_API extern     ECS_DECLARE(EcsKiloMetersPerHour);
FLECS_API extern     ECS_DECLARE(EcsMilesPerHour);

FLECS_API extern ECS_DECLARE(EcsTemperature);
FLECS_API extern     ECS_DECLARE(EcsKelvin);
FLECS_API extern     ECS_DECLARE(EcsCelsius);
FLECS_API extern     ECS_DECLARE(EcsFahrenheit);

FLECS_API extern ECS_DECLARE(EcsAcceleration);

FLECS_API extern ECS_DECLARE(EcsData);
FLECS_API extern     ECS_DECLARE(EcsBits);
FLECS_API extern         ECS_DECLARE(EcsKiloBits);
FLECS_API extern         ECS_DECLARE(EcsMegaBits);
FLECS_API extern         ECS_DECLARE(EcsGigaBits);
FLECS_API extern     ECS_DECLARE(EcsBytes);
FLECS_API extern         ECS_DECLARE(EcsKiloBytes);
FLECS_API extern         ECS_DECLARE(EcsMegaBytes);
FLECS_API extern         ECS_DECLARE(EcsGigaBytes);
FLECS_API extern         ECS_DECLARE(EcsKibiBytes);
FLECS_API extern         ECS_DECLARE(EcsMebiBytes);
FLECS_API extern         ECS_DECLARE(EcsGibiBytes);

FLECS_API extern ECS_DECLARE(EcsDataRate);
FLECS_API extern     ECS_DECLARE(EcsBitsPerSecond);
FLECS_API extern     ECS_DECLARE(EcsKiloBitsPerSecond);
FLECS_API extern     ECS_DECLARE(EcsMegaBitsPerSecond);
FLECS_API extern     ECS_DECLARE(EcsGigaBitsPerSecond);
FLECS_API extern     ECS_DECLARE(EcsBytesPerSecond);
FLECS_API extern     ECS_DECLARE(EcsKiloBytesPerSecond);
FLECS_API extern     ECS_DECLARE(EcsMegaBytesPerSecond);
FLECS_API extern     ECS_DECLARE(EcsGigaBytesPerSecond);

FLECS_API extern ECS_DECLARE(EcsPercentage);

FLECS_API extern ECS_DECLARE(EcsAngle);
FLECS_API extern     ECS_DECLARE(EcsRadians);
FLECS_API extern     ECS_DECLARE(EcsDegrees);

FLECS_API extern ECS_DECLARE(EcsBel);
FLECS_API extern ECS_DECLARE(EcsDeciBel);

////////////////////////////////////////////////////////////////////////////////
//// Module
////////////////////////////////////////////////////////////////////////////////

FLECS_API
void FlecsUnitsImport(
    ecs_world_t *world);

#ifdef __cplusplus
}
#endif

#endif

#endif

#endif
#ifdef FLECS_META
#ifdef FLECS_NO_META
#error "FLECS_NO_META failed: META is required by other addons"
#endif
/**
 * @file meta.h
 * @brief Meta addon.
 *
 * The meta addon enables reflecting on component data. Types are stored as
 * entities, with components that store the reflection data. A type has at least
 * two components:
 * 
 * - EcsComponent: core component, contains size & alignment
 * - EcsMetaType:  component that indicates what kind of type the entity is
 *
 * Additionally the type may have an additional component that contains the
 * reflection data for the type. For example, structs have these components:
 * 
 * - EcsComponent
 * - EcsMetaType
 * - EcsStruct
 * 
 * Structs can be populated by adding child entities with the EcsMember 
 * component. Adding a child with a Member component to an entity will 
 * automatically add the EcsStruct component to the parent.
 * 
 * Enums/bitmasks can be populated by adding child entities with the Constant
 * tag. By default constants are automatically assigned values when they are
 * added to the enum/bitmask. The parent entity must have the EcsEnum or
 * EcsBitmask component before adding the constants.
 * 
 * To create enum constants with a manual value, set (Constant, i32) to the
 * desired value. To create bitmask constants with a manual value, set
 * (Constant, u32) to the desired value. Constants with manual values should not
 * conflict with other constants.
 * 
 * The _init APIs are convenience wrappers around creating the entities and
 * components for the types.
 * 
 * When a type is created it automatically receives the EcsComponent and 
 * EcsMetaType components. The former means that the resulting type can be
 * used as a regular component:
 * 
 * // Create Position type
 * ecs_entity_t pos = ecs_struct_init(world, &(ecs_struct_desc_t) {
 *  .entity.name = "Position",
 *  .members = {
 *       {"x", ecs_id(ecs_f32_t)},
 *       {"y", ecs_id(ecs_f32_t)}
 *   }
 * });
 *
 * // Create entity with Position component
 * ecs_entity_t e = ecs_new_w_id(world, pos);
 * 
 * Type entities do not have to be named.
 */

#ifdef FLECS_META

#include <stddef.h>

#ifndef FLECS_MODULE
#define FLECS_MODULE
#endif

#ifndef FLECS_META_H
#define FLECS_META_H

#ifdef __cplusplus
extern "C" {
#endif

#define ECS_MEMBER_DESC_CACHE_SIZE (32)

/** Primitive type definitions.
 * These typedefs allow the builtin primitives to be used as regular components:
 *   ecs_set(world, e, ecs_i32_t, {10});
 * 
 * Or a more useful example (create an enum constant with a manual value):
 *   ecs_set_pair_object(world, e, EcsConstant, ecs_i32_t, {10});
 */
typedef bool ecs_bool_t;
typedef char ecs_char_t;
typedef unsigned char ecs_byte_t;
typedef uint8_t ecs_u8_t;
typedef uint16_t ecs_u16_t;
typedef uint32_t ecs_u32_t;
typedef uint64_t ecs_u64_t;
typedef uintptr_t ecs_uptr_t;
typedef int8_t ecs_i8_t;
typedef int16_t ecs_i16_t;
typedef int32_t ecs_i32_t;
typedef int64_t ecs_i64_t;
typedef intptr_t ecs_iptr_t;
typedef float ecs_f32_t;
typedef double ecs_f64_t;
typedef char* ecs_string_t;

/** Meta module component ids */
FLECS_API extern const ecs_entity_t ecs_id(EcsMetaType);
FLECS_API extern const ecs_entity_t ecs_id(EcsMetaTypeSerialized);
FLECS_API extern const ecs_entity_t ecs_id(EcsPrimitive);
FLECS_API extern const ecs_entity_t ecs_id(EcsEnum);
FLECS_API extern const ecs_entity_t ecs_id(EcsBitmask);
FLECS_API extern const ecs_entity_t ecs_id(EcsMember);
FLECS_API extern const ecs_entity_t ecs_id(EcsStruct);
FLECS_API extern const ecs_entity_t ecs_id(EcsArray);
FLECS_API extern const ecs_entity_t ecs_id(EcsVector);
FLECS_API extern const ecs_entity_t ecs_id(EcsUnit);
FLECS_API extern const ecs_entity_t ecs_id(EcsUnitPrefix);
FLECS_API extern const ecs_entity_t EcsConstant;
FLECS_API extern const ecs_entity_t EcsQuantity;

/** Primitive type component ids */
FLECS_API extern const ecs_entity_t ecs_id(ecs_bool_t);
FLECS_API extern const ecs_entity_t ecs_id(ecs_char_t);
FLECS_API extern const ecs_entity_t ecs_id(ecs_byte_t);
FLECS_API extern const ecs_entity_t ecs_id(ecs_u8_t);
FLECS_API extern const ecs_entity_t ecs_id(ecs_u16_t);
FLECS_API extern const ecs_entity_t ecs_id(ecs_u32_t);
FLECS_API extern const ecs_entity_t ecs_id(ecs_u64_t);
FLECS_API extern const ecs_entity_t ecs_id(ecs_uptr_t);
FLECS_API extern const ecs_entity_t ecs_id(ecs_i8_t);
FLECS_API extern const ecs_entity_t ecs_id(ecs_i16_t);
FLECS_API extern const ecs_entity_t ecs_id(ecs_i32_t);
FLECS_API extern const ecs_entity_t ecs_id(ecs_i64_t);
FLECS_API extern const ecs_entity_t ecs_id(ecs_iptr_t);
FLECS_API extern const ecs_entity_t ecs_id(ecs_f32_t);
FLECS_API extern const ecs_entity_t ecs_id(ecs_f64_t);
FLECS_API extern const ecs_entity_t ecs_id(ecs_string_t);
FLECS_API extern const ecs_entity_t ecs_id(ecs_entity_t);

/** Type kinds supported by reflection type system */
typedef enum ecs_type_kind_t {
    EcsPrimitiveType,
    EcsBitmaskType,
    EcsEnumType,
    EcsStructType,
    EcsArrayType,
    EcsVectorType,
    EcsTypeKindLast = EcsVectorType
} ecs_type_kind_t;

/** Component that is automatically added to every type with the right kind. */
typedef struct EcsMetaType {
    ecs_type_kind_t kind;
    bool existing;   /* Did the type exist or is it populated from reflection */
    bool partial;    /* Is the reflection data a partial type description */
    ecs_size_t size;       /* Computed size */
    ecs_size_t alignment;  /* Computed alignment */
} EcsMetaType;

typedef enum ecs_primitive_kind_t {
    EcsBool = 1,
    EcsChar,
    EcsByte,
    EcsU8,
    EcsU16,
    EcsU32,
    EcsU64,
    EcsI8,
    EcsI16,
    EcsI32,
    EcsI64,
    EcsF32,
    EcsF64,
    EcsUPtr,
    EcsIPtr,
    EcsString,
    EcsEntity,
    EcsPrimitiveKindLast = EcsEntity
} ecs_primitive_kind_t;

typedef struct EcsPrimitive {
    ecs_primitive_kind_t kind;
} EcsPrimitive;

typedef struct EcsMember {
    ecs_entity_t type;
    int32_t count;
    ecs_entity_t unit;
    int32_t offset;
} EcsMember;

/* Element type of members vector in EcsStruct */
typedef struct ecs_member_t {
    /* Must be set when used with ecs_struct_desc_t */
    const char *name;
    ecs_entity_t type;

    /* May be set when used with ecs_struct_desc_t */
    int32_t count;
    int32_t offset;

    /* May be set when used with ecs_struct_desc_t, will be auto-populated if
     * type entity is also a unit */
    ecs_entity_t unit;

    /* Should not be set by ecs_struct_desc_t */
    ecs_size_t size;
    ecs_entity_t member;
} ecs_member_t;

typedef struct EcsStruct {
    /* Populated from child entities with Member component */
    ecs_vector_t *members; /* vector<ecs_member_t> */
} EcsStruct;

typedef struct ecs_enum_constant_t {
    /* Must be set when used with ecs_enum_desc_t */
    const char *name;

    /* May be set when used with ecs_enum_desc_t */
    int32_t value;

    /* Should not be set by ecs_enum_desc_t */
    ecs_entity_t constant;
} ecs_enum_constant_t;

typedef struct EcsEnum {
    /* Populated from child entities with Constant component */
    ecs_map_t *constants; /* map<i32_t, ecs_enum_constant_t> */
} EcsEnum;

typedef struct ecs_bitmask_constant_t {
    /* Must be set when used with ecs_bitmask_desc_t */
    const char *name;

    /* May be set when used with ecs_bitmask_desc_t */
    ecs_flags32_t value;

    /* Should not be set by ecs_bitmask_desc_t */
    ecs_entity_t constant;
} ecs_bitmask_constant_t;

typedef struct EcsBitmask {
    /* Populated from child entities with Constant component */
    ecs_map_t *constants; /* map<u32_t, ecs_bitmask_constant_t> */
} EcsBitmask;

typedef struct EcsArray {
    ecs_entity_t type;
    int32_t count;
} EcsArray;

typedef struct EcsVector {
    ecs_entity_t type;
} EcsVector;


/** Units */

/* Helper type to describe translation between two units. Note that this
 * is not intended as a generic approach to unit conversions (e.g. from celsius
 * to fahrenheit) but to translate between units that derive from the same base 
 * (e.g. meters to kilometers). 
 * 
 * Note that power is applied to the factor. When describing a translation of
 * 1000, either use {factor = 1000, power = 1} or {factor = 1, power = 3}. */
typedef struct ecs_unit_translation_t {
    int32_t factor; /* Factor to apply (e.g. "1000", "1000000", "1024") */
    int32_t power; /* Power to apply to factor (e.g. "1", "3", "-9") */
} ecs_unit_translation_t;

typedef struct EcsUnit {
    char *symbol;
    ecs_entity_t prefix; /* Order of magnitude prefix relative to derived */
    ecs_entity_t base; /* Base unit (e.g. "meters") */
    ecs_entity_t over; /* Over unit (e.g. "per second") */
    ecs_unit_translation_t translation; /* Translation for derived unit */
} EcsUnit;

typedef struct EcsUnitPrefix {
    char *symbol;   /* Symbol of prefix (e.g. "K", "M", "Ki") */
    ecs_unit_translation_t translation; /* Translation of prefix */
} EcsUnitPrefix;


/** Serializer utilities */

typedef enum ecs_meta_type_op_kind_t {
    EcsOpArray,
    EcsOpVector,
    EcsOpPush,
    EcsOpPop,

    EcsOpScope, /* Marks last constant that can open/close a scope */

    EcsOpEnum,
    EcsOpBitmask,

    EcsOpPrimitive, /* Marks first constant that's a primitive */

    EcsOpBool,
    EcsOpChar,
    EcsOpByte,
    EcsOpU8,
    EcsOpU16,
    EcsOpU32,
    EcsOpU64,
    EcsOpI8,
    EcsOpI16,
    EcsOpI32,
    EcsOpI64,
    EcsOpF32,
    EcsOpF64,
    EcsOpUPtr,
    EcsOpIPtr,
    EcsOpString,
    EcsOpEntity
} ecs_meta_type_op_kind_t;

typedef struct ecs_meta_type_op_t {
    ecs_meta_type_op_kind_t kind;
    ecs_size_t offset;      /* Offset of current field */
    int32_t count;        
    const char *name;       /* Name of value (only used for struct members) */
    int32_t op_count;       /* Number of operations until next field or end */
    ecs_size_t size;        /* Size of type of operation */
    ecs_entity_t type;
    ecs_entity_t unit;
    ecs_hashmap_t *members; /* string -> member index (structs only) */
} ecs_meta_type_op_t;

typedef struct EcsMetaTypeSerialized {
    ecs_vector_t* ops;     /* vector<ecs_meta_type_op_t> */
} EcsMetaTypeSerialized;


/** Deserializer utilities */

#define ECS_META_MAX_SCOPE_DEPTH (32) /* >32 levels of nesting is not sane */

typedef struct ecs_meta_scope_t {
    ecs_entity_t type;        /* The type being iterated */
    ecs_meta_type_op_t *ops;  /* The type operations (see ecs_meta_type_op_t) */
    int32_t op_count;         /* Number of operations in ops array to process */
    int32_t op_cur;           /* Current operation */
    int32_t elem_cur;         /* Current element (for collections) */
    void *ptr;                /* Pointer to the value being iterated */

    const EcsComponent *comp; /* Pointer to component, in case size/alignment is needed */
    ecs_vector_t **vector;    /* Current vector, in case a vector is iterated */
    bool is_collection;       /* Is the scope iterating elements? */
    bool is_inline_array;     /* Is the scope iterating an inline array? */
} ecs_meta_scope_t;

/** Type that enables iterating/populating a value using reflection data */
typedef struct ecs_meta_cursor_t {
    const ecs_world_t *world;
    ecs_meta_scope_t scope[ECS_META_MAX_SCOPE_DEPTH];
    int32_t depth;
    bool valid;
    bool is_primitive_scope;  /* If in root scope, this allows for a push for primitive types */

    /* Custom entity lookup action for overriding default ecs_lookup_fullpath */
    ecs_entity_t (*lookup_action)(const ecs_world_t*, const char*, void*);
    void *lookup_ctx;
} ecs_meta_cursor_t;

FLECS_API
ecs_meta_cursor_t ecs_meta_cursor(
    const ecs_world_t *world,
    ecs_entity_t type,
    void *ptr);

/** Get pointer to current field */
FLECS_API
void* ecs_meta_get_ptr(
    ecs_meta_cursor_t *cursor);

/** Move cursor to next field */
FLECS_API
int ecs_meta_next(
    ecs_meta_cursor_t *cursor);

/** Move cursor to a field */
FLECS_API
int ecs_meta_move(
    ecs_meta_cursor_t *cursor,
    int32_t pos);

/** Move cursor to member */
FLECS_API
int ecs_meta_member(
    ecs_meta_cursor_t *cursor,
    const char *name);

/** Push a scope (required/only valid for structs & collections) */
FLECS_API
int ecs_meta_push(
    ecs_meta_cursor_t *cursor);

/** Pop a struct or collection scope (must follow a push) */
FLECS_API
int ecs_meta_pop(
    ecs_meta_cursor_t *cursor);

/** Is the current scope a collection? */
FLECS_API
bool ecs_meta_is_collection(
    const ecs_meta_cursor_t *cursor);

/** Get type of current element. */
FLECS_API
ecs_entity_t ecs_meta_get_type(
    const ecs_meta_cursor_t *cursor);

/** Get unit of current element. */
FLECS_API
ecs_entity_t ecs_meta_get_unit(
    const ecs_meta_cursor_t *cursor);

/** Get member name of current member */
FLECS_API
const char* ecs_meta_get_member(
    const ecs_meta_cursor_t *cursor);

/** The set functions assign the field with the specified value. If the value
 * does not have the same type as the field, it will be cased to the field type.
 * If no valid conversion is available, the operation will fail. */

/** Set field with boolean value */
FLECS_API
int ecs_meta_set_bool(
    ecs_meta_cursor_t *cursor,
    bool value);

/** Set field with char value */
FLECS_API
int ecs_meta_set_char(
    ecs_meta_cursor_t *cursor,
    char value);

/** Set field with int value */
FLECS_API
int ecs_meta_set_int(
    ecs_meta_cursor_t *cursor,
    int64_t value);

/** Set field with uint value */
FLECS_API
int ecs_meta_set_uint(
    ecs_meta_cursor_t *cursor,
    uint64_t value);

/** Set field with float value */
FLECS_API
int ecs_meta_set_float(
    ecs_meta_cursor_t *cursor,
    double value);

/** Set field with string value */
FLECS_API
int ecs_meta_set_string(
    ecs_meta_cursor_t *cursor,
    const char *value);

/** Set field with string literal value (has enclosing "") */
FLECS_API
int ecs_meta_set_string_literal(
    ecs_meta_cursor_t *cursor,
    const char *value);

/** Set field with entity value */
FLECS_API
int ecs_meta_set_entity(
    ecs_meta_cursor_t *cursor,
    ecs_entity_t value);

/** Set field with null value */
FLECS_API
int ecs_meta_set_null(
    ecs_meta_cursor_t *cursor);


/** Functions for getting members. */

/** Get field value as boolean. */
FLECS_API
bool ecs_meta_get_bool(
    const ecs_meta_cursor_t *cursor);

/** Get field value as char. */
FLECS_API
char ecs_meta_get_char(
    const ecs_meta_cursor_t *cursor);

/** Get field value as signed integer. */
FLECS_API
int64_t ecs_meta_get_int(
    const ecs_meta_cursor_t *cursor);

/** Get field value as unsigned integer. */
FLECS_API
uint64_t ecs_meta_get_uint(
    const ecs_meta_cursor_t *cursor);

/** Get field value as float. */
FLECS_API
double ecs_meta_get_float(
    const ecs_meta_cursor_t *cursor);

/** Get field value as string. 
 * This operation does not perform conversions. If the field is not a string,
 * this operation will fail.
 */
FLECS_API
const char* ecs_meta_get_string(
    const ecs_meta_cursor_t *cursor);

/** Get field value as entity. 
 * This operation does not perform conversions. */
FLECS_API
ecs_entity_t ecs_meta_get_entity(
    const ecs_meta_cursor_t *cursor);


/** API functions for creating meta types */

/** Used with ecs_primitive_init. */
typedef struct ecs_primitive_desc_t {
    ecs_entity_desc_t entity;
    ecs_primitive_kind_t kind;
} ecs_primitive_desc_t;

/** Create a new primitive type */
FLECS_API
ecs_entity_t ecs_primitive_init(
    ecs_world_t *world,
    const ecs_primitive_desc_t *desc);

/** Used with ecs_enum_init. */
typedef struct ecs_enum_desc_t {
    ecs_entity_desc_t entity;
    ecs_enum_constant_t constants[ECS_MEMBER_DESC_CACHE_SIZE];
} ecs_enum_desc_t;

/** Create a new enum type */
FLECS_API
ecs_entity_t ecs_enum_init(
    ecs_world_t *world,
    const ecs_enum_desc_t *desc);


/** Used with ecs_bitmask_init. */
typedef struct ecs_bitmask_desc_t {
    ecs_entity_desc_t entity;
    ecs_bitmask_constant_t constants[ECS_MEMBER_DESC_CACHE_SIZE];
} ecs_bitmask_desc_t;

/** Create a new bitmask type */
FLECS_API
ecs_entity_t ecs_bitmask_init(
    ecs_world_t *world,
    const ecs_bitmask_desc_t *desc);


/** Used with ecs_array_init. */
typedef struct ecs_array_desc_t {
    ecs_entity_desc_t entity;
    ecs_entity_t type;
    int32_t count;
} ecs_array_desc_t;

/** Create a new array type */
FLECS_API
ecs_entity_t ecs_array_init(
    ecs_world_t *world,
    const ecs_array_desc_t *desc);


/** Used with ecs_vector_init. */
typedef struct ecs_vector_desc_t {
    ecs_entity_desc_t entity;
    ecs_entity_t type;
} ecs_vector_desc_t;

/** Create a new vector type */
FLECS_API
ecs_entity_t ecs_vector_init(
    ecs_world_t *world,
    const ecs_vector_desc_t *desc);


/** Used with ecs_struct_init. */
typedef struct ecs_struct_desc_t {
    ecs_entity_desc_t entity;
    ecs_member_t members[ECS_MEMBER_DESC_CACHE_SIZE];
} ecs_struct_desc_t;

/** Create a new struct type */
FLECS_API
ecs_entity_t ecs_struct_init(
    ecs_world_t *world,
    const ecs_struct_desc_t *desc);

/** Used with ecs_unit_init. */
typedef struct ecs_unit_desc_t {
    ecs_entity_desc_t entity;
    
    /* Unit symbol, e.g. "m", "%", "g". (optional) */
    const char *symbol;

    /* Unit quantity, e.g. distance, percentage, weight. (optional) */
    ecs_entity_t quantity;

    /* Base unit, e.g. "meters" (optional) */
    ecs_entity_t base;

    /* Over unit, e.g. "per second" (optional) */
    ecs_entity_t over;

    /* Translation to apply to derived unit (optional) */
    ecs_unit_translation_t translation;

    /* Prefix indicating order of magnitude relative to the derived unit. If set
     * together with "translation", the values must match. If translation is not 
     * set, setting prefix will autopopulate it. 
     * Additionally, setting the prefix will enforce that the symbol (if set)
     * is consistent with the prefix symbol + symbol of the derived unit. If the
     * symbol is not set, it will be auto populated. */
    ecs_entity_t prefix;
} ecs_unit_desc_t;

/** Create a new unit */
FLECS_API
ecs_entity_t ecs_unit_init(
    ecs_world_t *world,
    const ecs_unit_desc_t *desc);

/** Used with ecs_unit_prefix_init. */
typedef struct ecs_unit_prefix_desc_t {
    ecs_entity_desc_t entity;
    
    /* Unit symbol, e.g. "m", "%", "g". (optional) */
    const char *symbol;

    /* Translation to apply to derived unit (optional) */
    ecs_unit_translation_t translation;
} ecs_unit_prefix_desc_t;

/** Create a new unit prefix */
FLECS_API
ecs_entity_t ecs_unit_prefix_init(
    ecs_world_t *world,
    const ecs_unit_prefix_desc_t *desc);

/** Create a new quantity */
FLECS_API
ecs_entity_t ecs_quantity_init(
    ecs_world_t *world,
    const ecs_entity_desc_t *desc);

/* Module import */
FLECS_API
void FlecsMetaImport(
    ecs_world_t *world);

#ifdef __cplusplus
}
#endif

#endif

#endif

#endif
#ifdef FLECS_EXPR
#ifdef FLECS_NO_EXPR
#error "FLECS_NO_EXPR failed: EXPR is required by other addons"
#endif
/**
 * @file expr.h
 * @brief Flecs expression parser addon.
 *
 * Parse expression strings into component values. The notation is similar to
 * JSON but with a smaller footprint, native support for (large) integer types,
 * character types, enumerations, bitmasks and entity identifiers.
 * 
 * Examples:
 * 
 * Member names:
 *   {x: 10, y: 20}
 * 
 * No member names (uses member ordering):
 *   {10, 20}
 * 
 * Enum values:
 *   {color: Red}
 * 
 * Bitmask values:
 *   {toppings: Lettuce|Tomato}
 * 
 * Collections:
 *   {points: [10, 20, 30]}
 * 
 * Nested objects:
 *   {start: {x: 10, y: 20}, stop: {x: 30, y: 40}}
 * 
 */

#ifdef FLECS_EXPR

#ifndef FLECS_META
#define FLECS_META
#endif

#ifndef FLECS_PARSER
#define FLECS_PARSER
#endif

#ifndef FLECS_EXPR_H
#define FLECS_EXPR_H

#ifdef __cplusplus
extern "C" {
#endif

/** Write an escaped character.
 * Write a character to an output string, insert escape character if necessary.
 * 
 * @param out The string to write the character to.
 * @param in The input character.
 * @param delimiter The delimiiter used (for example '"')
 * @return Pointer to the character after the last one written.
 */
FLECS_API
char* ecs_chresc(
    char *out, 
    char in, 
    char delimiter);

/** Parse an escaped character.
 * Parse a character with a potential escape sequence.
 * 
 * @param in Pointer to character in input string.
 * @param out Output string.
 * @return Pointer to the character after the last one read.
 */ 
const char* ecs_chrparse(
    const char *in, 
    char *out);

/** Write an escaped string.
 * Write an input string to an output string, escape characters where necessary.
 * To determine the size of the output string, call the operation with a NULL
 * argument for 'out', and use the returned size to allocate a string that is
 * large enough.
 * 
 * @param out Pointer to output string (msut be).
 * @param size Maximum number of characters written to output.
 * @param delimiter The delimiter used (for example '"').
 * @param in The input string.
 * @return The number of characters that (would) have been written.
 */
FLECS_API
ecs_size_t ecs_stresc(
    char *out, 
    ecs_size_t size, 
    char delimiter, 
    const char *in);

/** Return escaped string.
 * Return escaped version of input string. Same as ecs_stresc, but returns an
 * allocated string of the right size.
 * 
 * @param delimiter The delimiter used (for example '"').
 * @param in The input string.
 * @return Escaped string.
 */
FLECS_API
char* ecs_astresc(
    char delimiter, 
    const char *in);

/** Used with ecs_parse_expr. */
typedef struct ecs_parse_expr_desc_t {
    const char *name;
    const char *expr;
    ecs_entity_t (*lookup_action)(
        const ecs_world_t*, 
        const char *value, 
        void *ctx);
    void *lookup_ctx;
} ecs_parse_expr_desc_t;

/** Parse expression into value.
 * This operation parses a flecs expression into the provided pointer. The
 * memory pointed to must be large enough to contain a value of the used type.
 * 
 * @param world The world.
 * @param ptr The pointer to the expression to parse.
 * @param type The type of the expression to parse.
 * @param data_out Pointer to the memory to write to.
 * @param desc Configuration parameters for deserializer.
 * @return Pointer to the character after the last one read, or NULL if failed.
 */
FLECS_API
const char* ecs_parse_expr(
    const ecs_world_t *world,
    const char *ptr,
    ecs_entity_t type,
    void *data_out,
    const ecs_parse_expr_desc_t *desc);

/** Serialize value into expression string.
 * This operation serializes a value of the provided type to a string. The 
 * memory pointed to must be large enough to contain a value of the used type.
 * 
 * @param world The world.
 * @param type The type of the value to serialize.
 * @param data The value to serialize.
 * @return String with expression, or NULL if failed.
 */
FLECS_API
char* ecs_ptr_to_expr(
    const ecs_world_t *world,
    ecs_entity_t type,
    const void *data);

/** Serialize value into string buffer.
 * Same as ecs_ptr_to_expr, but serializes to an ecs_strbuf_t instance.
 * 
 * @param world The world.
 * @param type The type of the value to serialize.
 * @param data The value to serialize.
 * @param buf The strbuf to append the string to.
 * @return Zero if success, non-zero if failed.
 */
FLECS_API
int ecs_ptr_to_expr_buf(
    const ecs_world_t *world,
    ecs_entity_t type,
    const void *data,
    ecs_strbuf_t *buf);

/** Serialize primitive value into string buffer.
 * Serializes a primitive value to an ecs_strbuf_t instance. This operation can
 * be reused by other serializers to avoid having to write boilerplate code that
 * serializes primitive values to a string.
 * 
 * @param world The world.
 * @param kind The kind of primitive value.
 * @param data The value ot serialize
 * @param buf The strbuf to append the string to.
 * @return Zero if success, non-zero if failed.
 */
FLECS_API
int ecs_primitive_to_expr_buf(
    const ecs_world_t *world,
    ecs_primitive_kind_t kind,
    const void *data, 
    ecs_strbuf_t *buf);

/** Parse expression token.
 * Expression tokens can contain more characters (such as '|') than tokens
 * parsed by the query (term) parser.
 * 
 * @param name The name of the expression (used for debug logs).
 * @param expr The full expression (used for debug logs).
 * @param ptr The pointer to the expression to parse.
 * @param token The buffer to write to (must have size ECS_MAX_TOKEN_SIZE)
 * @return Pointer to the character after the last one read, or NULL if failed.
 */
FLECS_API
const char *ecs_parse_expr_token(
    const char *name,
    const char *expr,
    const char *ptr,
    char *token);

#ifdef __cplusplus
}
#endif

#endif

#endif

#endif
#ifdef FLECS_META_C
#ifdef FLECS_NO_META_C
#error "FLECS_NO_META_C failed: META_C is required by other addons"
#endif
/**
 * @file meta_c.h
 * @brief Utility macro's for populating reflection data in C.
 */

#ifdef FLECS_META_C

#ifndef FLECS_META
#define FLECS_META
#endif

#ifndef FLECS_PARSER
#define FLECS_PARSER
#endif

#ifndef FLECS_META_C_H
#define FLECS_META_C_H

#ifdef __cplusplus
extern "C" {
#endif

/* Public API */

/* Macro that controls behavior of API. Usually set in module header. When the
 * macro is not defined, it defaults to IMPL. */

/* Define variables used by reflection utilities. This should only be defined
 * by the module itself, not by the code importing the module */
/* #define ECS_META_IMPL IMPL */

/* Don't define variables used by reflection utilities but still declare the
 * variable for the component id. This enables the reflection utilities to be
 * used for global component variables, even if no reflection is used. */
/* #define ECS_META_IMPL DECLARE */

/* Don't define variables used by reflection utilities. This generates an extern
 * variable for the component identifier. */
/* #define ECS_META_IMPL EXTERN */

/** Declare component with descriptor */
#define ECS_META_COMPONENT(world, name)\
    ECS_COMPONENT_DEFINE(world, name);\
    ecs_meta_from_desc(world, ecs_id(name),\
        FLECS__##name##_kind, FLECS__##name##_desc)

/** ECS_STRUCT(name, body) */
#define ECS_STRUCT(name, ...)\
    ECS_STRUCT_TYPE(name, __VA_ARGS__);\
    ECS_META_IMPL_CALL(ECS_STRUCT_, ECS_META_IMPL, name, #__VA_ARGS__)

/** ECS_ENUM(name, body) */
#define ECS_ENUM(name, ...)\
    ECS_ENUM_TYPE(name, __VA_ARGS__);\
    ECS_META_IMPL_CALL(ECS_ENUM_, ECS_META_IMPL, name, #__VA_ARGS__)

/** ECS_BITMASK(name, body) */
#define ECS_BITMASK(name, ...)\
    ECS_ENUM_TYPE(name, __VA_ARGS__);\
    ECS_META_IMPL_CALL(ECS_BITMASK_, ECS_META_IMPL, name, #__VA_ARGS__)

/** Macro used to mark part of type for which no reflection data is created */
#define ECS_PRIVATE

/** Populate meta information from type descriptor. */
FLECS_API
int ecs_meta_from_desc(
    ecs_world_t *world,
    ecs_entity_t component,
    ecs_type_kind_t kind,
    const char *desc);


/* Private API */

/* Utilities to switch beteen IMPL, DECLARE and EXTERN variants */
#define ECS_META_IMPL_CALL_INNER(base, impl, name, type_desc)\
    base ## impl(name, type_desc)

#define ECS_META_IMPL_CALL(base, impl, name, type_desc)\
    ECS_META_IMPL_CALL_INNER(base, impl, name, type_desc)

/* ECS_STRUCT implementation */
#define ECS_STRUCT_TYPE(name, ...)\
    typedef struct __VA_ARGS__ name

#define ECS_STRUCT_ECS_META_IMPL ECS_STRUCT_IMPL

#define ECS_STRUCT_IMPL(name, type_desc)\
    FLECS_META_C_EXPORT extern ECS_COMPONENT_DECLARE(name);\
    static const char *FLECS__##name##_desc = type_desc;\
    static ecs_type_kind_t FLECS__##name##_kind = EcsStructType;\
    FLECS_META_C_EXPORT ECS_COMPONENT_DECLARE(name) = 0

#define ECS_STRUCT_DECLARE(name, type_desc)\
    FLECS_META_C_EXPORT extern ECS_COMPONENT_DECLARE(name);\
    FLECS_META_C_EXPORT ECS_COMPONENT_DECLARE(name) = 0

#define ECS_STRUCT_EXTERN(name, type_desc)\
    FLECS_META_C_IMPORT extern ECS_COMPONENT_DECLARE(name)


/* ECS_ENUM implementation */
#define ECS_ENUM_TYPE(name, ...)\
    typedef enum __VA_ARGS__ name

#define ECS_ENUM_ECS_META_IMPL ECS_ENUM_IMPL

#define ECS_ENUM_IMPL(name, type_desc)\
    FLECS_META_C_EXPORT extern ECS_COMPONENT_DECLARE(name);\
    static const char *FLECS__##name##_desc = type_desc;\
    static ecs_type_kind_t FLECS__##name##_kind = EcsEnumType;\
    FLECS_META_C_EXPORT ECS_COMPONENT_DECLARE(name) = 0

#define ECS_ENUM_DECLARE(name, type_desc)\
    FLECS_META_C_EXPORT extern ECS_COMPONENT_DECLARE(name);\
    FLECS_META_C_EXPORT ECS_COMPONENT_DECLARE(name) = 0

#define ECS_ENUM_EXTERN(name, type_desc)\
    FLECS_META_C_IMPORT extern ECS_COMPONENT_DECLARE(name)


/* ECS_BITMASK implementation */
#define ECS_BITMASK_TYPE(name, ...)\
    typedef enum __VA_ARGS__ name

#define ECS_BITMASK_ECS_META_IMPL ECS_BITMASK_IMPL

#define ECS_BITMASK_IMPL(name, type_desc)\
    FLECS_META_C_EXPORT extern ECS_COMPONENT_DECLARE(name);\
    static const char *FLECS__##name##_desc = type_desc;\
    static ecs_type_kind_t FLECS__##name##_kind = EcsBitmaskType;\
    FLECS_META_C_EXPORT ECS_COMPONENT_DECLARE(name) = 0

#define ECS_BITMASK_DECLARE(name, type_desc)\
    FLECS_META_C_EXPORT extern ECS_COMPONENT_DECLARE(name);\
    FLECS_META_C_EXPORT ECS_COMPONENT_DECLARE(name) = 0

#define ECS_BITMASK_EXTERN(name, type_desc)\
    FLECS_META_C_IMPORT extern ECS_COMPONENT_DECLARE(name)


/* Symbol export utility macro's */
#if defined(ECS_TARGET_WINDOWS)
#define FLECS_META_C_EXPORT __declspec(dllexport)
#define FLECS_META_C_IMPORT __declspec(dllimport)
#else
#define FLECS_META_C_EXPORT __attribute__((__visibility__("default")))
#define FLECS_META_C_IMPORT
#endif

#ifdef __cplusplus
}
#endif

#endif // FLECS_META_C_H

#endif // FLECS_META_C

#endif
#ifdef FLECS_PLECS
#ifdef FLECS_NO_PLECS
#error "FLECS_NO_PLECS failed: PLECS is required by other addons"
#endif
/**
 * @file pecs.h
 * @brief Plecs addon.
 *
 * Plecs is a small data definition language for instantiating entities that
 * reuses the existing flecs query parser. The following examples illustrate
 * how a plecs snippet translates to regular flecs operations:
 *
 * Plecs:
 *   Entity
 * C code:
 *   ecs_entity_t Entity = ecs_set_name(world, 0, "Entity");
 *
 * Plecs:
 *   Position(Entity)
 * C code:
 *   ecs_entity_t Position = ecs_set_name(world, 0, "Position"); 
 *   ecs_entity_t Entity = ecs_set_name(world, 0, "Entity");
 *   ecs_add_id(world, Entity, Position);
 *
 * Plecs:
 *   Likes(Entity, Apples)
 * C code:
 *   ecs_entity_t Likes = ecs_set_name(world, 0, "Likes"); 
 *   ecs_entity_t Apples = ecs_set_name(world, 0, "Apples"); 
 *   ecs_entity_t Entity = ecs_set_name(world, 0, "Entity");
 *   ecs_add_pair(world, Entity, Likes, Apples);
 *
 * A plecs string may contain multiple statements, separated by a newline:
 *   Likes(Entity, Apples)
 *   Likes(Entity, Pears)
 *   Likes(Entity, Bananas)
 */

#ifdef FLECS_PLECS

#ifndef FLECS_PARSER
#define FLECS_PARSER
#endif

#ifndef FLECS_PLECS_H
#define FLECS_PLECS_H

#ifdef __cplusplus
extern "C" {
#endif

/** Parse plecs string.
 * This parses a plecs string and instantiates the entities in the world.
 *
 * @param world The world.
 * @param name The script name (typically the file).
 * @param str The plecs string.
 * @return Zero if success, non-zero otherwise.
 */
FLECS_API
int ecs_plecs_from_str(
    ecs_world_t *world,
    const char *name,
    const char *str);

/** Parse plecs file.
 * This parses a plecs file and instantiates the entities in the world. This
 * operation is equivalent to loading the file contents and passing it to
 * ecs_plecs_from_str.
 *
 * @param world The world.
 * @param filename The plecs file name.
 * @return Zero if success, non-zero otherwise.
 */
FLECS_API
int ecs_plecs_from_file(
    ecs_world_t *world,
    const char *filename);

#ifdef __cplusplus
}
#endif

#endif

#endif

#endif
#ifdef FLECS_RULES
#ifdef FLECS_NO_RULES
#error "FLECS_NO_RULES failed: RULES is required by other addons"
#endif

/**
 * @file rules.h
 * @brief Rule query engine addon.
 * 
 * Rules are advanced queries that in addition to the capabilities of regular
 * queries and filters have the folllowing features:
 * 
 * - query for all components of an entity (vs. all entities for a component)
 * - query for all relationship pairs of an entity
 * - support for query variables that are resolved at evaluation time
 * - automatic traversal of transitive relationships
 */

#ifdef FLECS_RULES

#ifndef FLECS_RULES_H
#define FLECS_RULES_H

#ifdef __cplusplus
extern "C" {
#endif

/** Create a rule.
 * A rule accepts the same descriptor as a filter, but has the additional
 * ability to use query variables.
 * 
 * Query variables can be used to constrain wildcards across multiple terms to
 * the same entity. Regular ECS queries do this in a limited form, as querying
 * for Position, Velocity only returns entities that have both components.
 * 
 * Query variables expand this to constrain entities that are resolved while the
 * query is being matched. Consider a query for all entities and the mission
 * they are on:
 *   (Mission, *)
 * 
 * If an entity is on multiple missions, the wildcard will match it multiple
 * times. Now say we want to only list combat missions. Naively we could try:
 *   (Mission, *), CombatMission(*)
 * 
 * But this doesn't work, as term 1 returns entities with missions, and term 2
 * returns all combat missions for all entities. Query variables make it 
 * possible to apply CombatMission to the found mission:
 *   (Mission, $M), CombatMission($M)
 * 
 * By using the same variable ('M') we ensure that CombatMission is applied to
 * the mission found in the current result.
 * 
 * Variables can be used in each part of the term (predicate, subject, object).
 * This is a valid query:
 *   Likes($X, $Y), Likes($Y, $X)
 * 
 * This is also a valid query:
 *   _Component, Serializable(_Component)
 * 
 * In the query expression syntax, variables are prefixed with a $. When using
 * the descriptor, specify the variable kind:
 *   desc.terms[0].obj = { .name = "X", .var = EcsVarIsVariable }
 * 
 * Different terms with the same variable name are automatically correlated by
 * the query engine.
 * 
 * A rule needs to be explicitly deleted with ecs_rule_fini.
 * 
 * @param world The world.
 * @param desc The descriptor (see ecs_filter_desc_t)
 * @return The rule.
 */
FLECS_API
ecs_rule_t* ecs_rule_init(
    ecs_world_t *world,
    const ecs_filter_desc_t *desc);

/** Delete a rule. 
 * 
 * @param rule The rule.
 */
FLECS_API
void ecs_rule_fini(
    ecs_rule_t *rule);

/** Obtain filter from rule.
 * This operation returns the filter with which the rule was created.
 * 
 * @param rule The rule.
 * @return The filter.
 */
FLECS_API
const ecs_filter_t* ecs_rule_get_filter(
    const ecs_rule_t *rule);

/** Return number of variables in rule.
 * 
 * @param rule The rule.
 * @return The number of variables/
 */
FLECS_API
int32_t ecs_rule_var_count(
    const ecs_rule_t *rule);

/** Find variable index.
 * This operation looks up the index of a variable in the rule. This index can
 * be used in operations like ecs_iter_set_var and ecs_iter_get_var.
 * 
 * @param rule The rule.
 * @param name The variable name.
 * @return The variable index.
 */
FLECS_API
int32_t ecs_rule_find_var(
    const ecs_rule_t *rule,
    const char *name);    

/** Get variable name.
 * This operation returns the variable name for an index.
 * 
 * @param rule The rule.
 * @param var_id The variable index.
 */
FLECS_API
const char* ecs_rule_var_name(
    const ecs_rule_t *rule,
    int32_t var_id);

/** Test if variable is an entity.
 * Internally the rule engine has entity variables and table variables. When
 * iterating through rule variables (by using ecs_rule_variable_count) only
 * the values for entity variables are accessible. This operation enables an
 * appliction to check if a variable is an entity variable.
 * 
 * @param rule The rule.
 * @param var_id The variable id.
 */
FLECS_API
bool ecs_rule_var_is_entity(
    const ecs_rule_t *rule,
    int32_t var_id);  

/** Iterate a rule.
 * Note that rule iterators may allocate memory, and that unless the iterator
 * is iterated until completion, it may still hold resources. When stopping
 * iteration before ecs_rule_next has returned false, use ecs_iter_fini to
 * cleanup any remaining resources.
 * 
 * @param world The world.
 * @param rule The rule.
 * @return An iterator.
 */
FLECS_API
ecs_iter_t ecs_rule_iter(
    const ecs_world_t *world,
    const ecs_rule_t *rule);

/** Progress rule iterator.
 * 
 * @param it The iterator.
 */
FLECS_API
bool ecs_rule_next(
    ecs_iter_t *it);

/** Progress instanced iterator.
 * Should not be called unless you know what you're doing :-)
 * 
 * @param it The iterator.
 */
FLECS_API
bool ecs_rule_next_instanced(
    ecs_iter_t *it);

/** Convert rule to a string.
 * This will convert the rule program to a string which can aid in debugging
 * the behavior of a rule.
 * 
 * The returned string must be freed with ecs_os_free.
 * 
 * @param rule The rule.
 * @return The string
 */
FLECS_API
char* ecs_rule_str(
    ecs_rule_t *rule);


#ifdef __cplusplus
}
#endif

#endif // FLECS_RULES_H

#endif // FLECS_RULES

#endif
#ifdef FLECS_SNAPSHOT
#ifdef FLECS_NO_SNAPSHOT
#error "FLECS_NO_SNAPSHOT failed: SNAPSHOT is required by other addons"
#endif
/**
 * @file snapshot.h
 * @brief Snapshot addon.
 *
 * A snapshot records the state of a world in a way so that it can be restored
 * later. Snapshots work with POD components and non-POD components, provided
 * that the appropriate lifecycle actions are registered for non-POD components.
 *
 * A snapshot is tightly coupled to a world. It is not possible to restore a
 * snapshot from world A into world B.
 */

#ifdef FLECS_SNAPSHOT

#ifndef FLECS_SNAPSHOT_H
#define FLECS_SNAPSHOT_H

#ifdef __cplusplus
extern "C" {
#endif

/** A snapshot stores the state of a world in a particular point in time. */
typedef struct ecs_snapshot_t ecs_snapshot_t;

/** Create a snapshot.
 * This operation makes a copy of all component in the world that matches the 
 * specified filter.
 *
 * @param world The world to snapshot.
 * @return The snapshot.
 */
FLECS_API
ecs_snapshot_t* ecs_snapshot_take(
    ecs_world_t *world);

/** Create a filtered snapshot.
 * This operation is the same as ecs_snapshot_take, but accepts an iterator so
 * an application can control what is stored by the snapshot. 
 *
 * @param iter An iterator to the data to be stored by the snapshot.
 * @return The snapshot.
 */
FLECS_API
ecs_snapshot_t* ecs_snapshot_take_w_iter(
    ecs_iter_t *iter);

/** Restore a snapshot.
 * This operation restores the world to the state it was in when the specified
 * snapshot was taken. A snapshot can only be used once for restoring, as its
 * data replaces the data that is currently in the world.
 * This operation also resets the last issued entity handle, so any calls to
 * ecs_new may return entity ids that have been issued before restoring the 
 * snapshot.
 *
 * The world in which the snapshot is restored must be the same as the world in
 * which the snapshot is taken.
 *
 * @param world The world to restore the snapshot to.
 * @param snapshot The snapshot to restore. 
 */
FLECS_API
void ecs_snapshot_restore(
    ecs_world_t *world,
    ecs_snapshot_t *snapshot);

/** Obtain iterator to snapshot data.
 *
 * @param snapshot The snapshot to iterate over.
 * @return Iterator to snapshot data. */
FLECS_API
ecs_iter_t ecs_snapshot_iter(
    ecs_snapshot_t *snapshot);

/** Progress snapshot iterator.
 * 
 * @param iter The snapshot iterator.
 * @return True if more data is available, otherwise false.
 */
FLECS_API
bool ecs_snapshot_next(
    ecs_iter_t *iter);

/** Free snapshot resources.
 * This frees resources associated with a snapshot without restoring it.
 *
 * @param snapshot The snapshot to free. 
 */
FLECS_API
void ecs_snapshot_free(
    ecs_snapshot_t *snapshot);
    
#ifdef __cplusplus
}
#endif

#endif

#endif

#endif
#ifdef FLECS_STATS
#ifdef FLECS_NO_STATS
#error "FLECS_NO_STATS failed: STATS is required by other addons"
#endif
/**
 * @file stats.h
 * @brief Statistics addon.
 *
 * The statistics addon enables an application to obtain detailed metrics about
 * the storage, systems and operations of a world.
 */

#ifdef FLECS_STATS

#ifndef FLECS_STATS_H
#define FLECS_STATS_H

#ifdef __cplusplus
extern "C" {
#endif

#define ECS_STAT_WINDOW (60)

/** Simple value that indicates current state */
typedef struct ecs_gauge_t {
    float avg[ECS_STAT_WINDOW];
    float min[ECS_STAT_WINDOW];
    float max[ECS_STAT_WINDOW];
} ecs_gauge_t;

/* Monotonically increasing counter */
typedef struct ecs_counter_t {
    ecs_gauge_t rate;                          /* Keep track of deltas too */
    float value[ECS_STAT_WINDOW];
} ecs_counter_t;

typedef struct ecs_world_stats_t {
    /* Allows struct to be initialized with {0} */
    int32_t dummy_;

    ecs_gauge_t entity_count;                 /* Number of entities */
    ecs_gauge_t entity_not_alive_count;       /* Number of not alive (recyclable) entity ids */

    /* Components and ids */
    ecs_gauge_t id_count;                     /* Number of ids (excluding wildcards) */
    ecs_gauge_t tag_id_count;                 /* Number of tag ids (ids without data) */
    ecs_gauge_t component_id_count;           /* Number of components ids (ids with data) */
    ecs_gauge_t pair_id_count;                /* Number of pair ids */
    ecs_gauge_t wildcard_id_count;            /* Number of wildcard ids */
    ecs_gauge_t component_count;              /* Number of components  (non-zero sized types) */
    ecs_counter_t id_create_count;            /* Number of times id has been created */
    ecs_counter_t id_delete_count;            /* Number of times id has been deleted */

    /* Tables */
    ecs_gauge_t table_count;                  /* Number of tables */
    ecs_gauge_t empty_table_count;            /* Number of empty tables */
    ecs_gauge_t singleton_table_count;        /* Number of singleton tables. Singleton tables are tables with just a single entity that contains itself */
    ecs_gauge_t tag_table_count;              /* Number of tables with only tags */
    ecs_gauge_t trivial_table_count;          /* Number of tables with only trivial components */
    ecs_gauge_t table_record_count;           /* Number of table cache records */
    ecs_gauge_t table_storage_count;          /* Number of table storages */
    ecs_counter_t table_create_count;         /* Number of times table has been created */
    ecs_counter_t table_delete_count;         /* Number of times table has been deleted */

    /* Queries & events */
    ecs_gauge_t query_count;                  /* Number of queries */
    ecs_gauge_t trigger_count;                /* Number of triggers */
    ecs_gauge_t observer_count;               /* Number of observers */
    ecs_gauge_t system_count;                 /* Number of systems */

    /* Deferred operations */
    ecs_counter_t new_count;
    ecs_counter_t bulk_new_count;
    ecs_counter_t delete_count;
    ecs_counter_t clear_count;
    ecs_counter_t add_count;
    ecs_counter_t remove_count;
    ecs_counter_t set_count;
    ecs_counter_t discard_count;

    /* Timing */
    ecs_counter_t world_time_total_raw;       /* Actual time passed since simulation start (first time progress() is called) */
    ecs_counter_t world_time_total;           /* Simulation time passed since simulation start. Takes into account time scaling */
    ecs_counter_t frame_time_total;           /* Time spent processing a frame. Smaller than world_time_total when load is not 100% */
    ecs_counter_t system_time_total;          /* Time spent on processing systems. */
    ecs_counter_t merge_time_total;           /* Time spent on merging deferred actions. */
    ecs_gauge_t fps;                          /* Frames per second. */
    ecs_gauge_t delta_time;                   /* Delta_time. */
    
    /* Frame data */
    ecs_counter_t frame_count_total;          /* Number of frames processed. */
    ecs_counter_t merge_count_total;          /* Number of merges executed. */
    ecs_counter_t pipeline_build_count_total; /* Number of system pipeline rebuilds (occurs when an inactive system becomes active). */
    ecs_counter_t systems_ran_frame;          /* Number of systems ran in the last frame. */

    /** Current position in ringbuffer */
    int32_t t;
} ecs_world_stats_t;

/* Statistics for a single query (use ecs_get_query_stats) */
typedef struct ecs_query_stats_t {
    ecs_gauge_t matched_table_count;       /* Number of matched non-empty tables. This is the number of tables 
                                            * iterated over when evaluating a query. */    

    ecs_gauge_t matched_empty_table_count; /* Number of matched empty tables. Empty tables are not iterated over when
                                            * evaluating a query. */
    
    ecs_gauge_t matched_entity_count;      /* Number of matched entities across all tables */

    /** Current position in ringbuffer */
    int32_t t; 
} ecs_query_stats_t;

/** Statistics for a single system (use ecs_get_system_stats) */
typedef struct ecs_system_stats_t {
    ecs_query_stats_t query_stats;
    ecs_counter_t time_spent;       /* Time spent processing a system */
    ecs_counter_t invoke_count;     /* Number of times system is invoked */
    ecs_gauge_t active;             /* Whether system is active (is matched with >0 entities) */
    ecs_gauge_t enabled;            /* Whether system is enabled */
} ecs_system_stats_t;

/** Statistics for all systems in a pipeline. */
typedef struct ecs_pipeline_stats_t {
    /** Vector with system ids of all systems in the pipeline. The systems are
     * stored in the order they are executed. Merges are represented by a 0. */
    ecs_vector_t *systems;

    /** Map with system statistics. For each system in the systems vector, an
     * entry in the map exists of type ecs_system_stats_t. */
    ecs_map_t *system_stats;

    int32_t system_count; /* Number of systems in pipeline */
    int32_t active_system_count; /* Number of active systems in pipeline */
    int32_t rebuild_count; /* Number of times pipeline has rebuilt */
} ecs_pipeline_stats_t;

/** Get world statistics.
 * Obtain statistics for the provided world. This operation loops several times
 * over the tables in the world, and can impact application performance.
 *
 * @param world The world.
 * @param stats Out parameter for statistics.
 */
FLECS_API 
void ecs_get_world_stats(
    const ecs_world_t *world,
    ecs_world_stats_t *stats);

/** Print world statistics.
 * Print statistics obtained by ecs_get_world_statistics and in the
 * ecs_world_info_t struct.
 * 
 * @param world The world.
 * @param stats The statistics to print.
 */
FLECS_API 
void ecs_dump_world_stats(
    const ecs_world_t *world,
    const ecs_world_stats_t *stats);

/** Get query statistics.
 * Obtain statistics for the provided query.
 *
 * @param world The world.
 * @param query The query.
 * @param stats Out parameter for statistics.
 */
FLECS_API 
void ecs_get_query_stats(
    const ecs_world_t *world,
    const ecs_query_t *query,
    ecs_query_stats_t *stats);

#ifdef FLECS_SYSTEM
/** Get system statistics.
 * Obtain statistics for the provided system.
 *
 * @param world The world.
 * @param system The system.
 * @param stats Out parameter for statistics.
 * @return true if success, false if not a system.
 */
FLECS_API 
bool ecs_get_system_stats(
    const ecs_world_t *world,
    ecs_entity_t system,
    ecs_system_stats_t *stats);
#endif

#ifdef FLECS_PIPELINE
/** Get pipeline statistics.
 * Obtain statistics for the provided pipeline.
 *
 * @param world The world.
 * @param pipeline The pipeline.
 * @param stats Out parameter for statistics.
 * @return true if success, false if not a pipeline.
 */
FLECS_API 
bool ecs_get_pipeline_stats(
    ecs_world_t *world,
    ecs_entity_t pipeline,
    ecs_pipeline_stats_t *stats);

/** Free pipeline stats.
 * 
 * @param stats The stats to free.
 */
FLECS_API
void ecs_pipeline_stats_fini(
    ecs_pipeline_stats_t *stats);

#endif

FLECS_API 
void ecs_gauge_reduce(
    ecs_gauge_t *dst,
    int32_t t_dst,
    ecs_gauge_t *src,
    int32_t t_src);

#ifdef __cplusplus
}
#endif

#endif

#endif

#endif
#ifdef FLECS_PARSER
#ifdef FLECS_NO_PARSER
#error "FLECS_NO_PARSER failed: PARSER is required by other addons"
#endif
/**
 * @file parser.h
 * @brief Parser addon.
 *
 * The parser addon parses string expressions into lists of terms, and can be
 * used to construct filters, queries and types.
 */

#ifdef FLECS_PARSER

#ifndef FLECS_PARSER_H
#define FLECS_PARSER_H

#ifdef __cplusplus
extern "C" {
#endif

/** Skip whitespace characters.
 * This function skips whitespace characters. Does not skip newlines.
 * 
 * @param ptr Pointer to (potential) whitespaces to skip.
 * @return Pointer to the next non-whitespace character.
 */
FLECS_API
const char* ecs_parse_whitespace(
    const char *ptr);

/** Skip whitespace and newline characters.
 * This function skips whitespace characters.
 * 
 * @param ptr Pointer to (potential) whitespaces to skip.
 * @return Pointer to the next non-whitespace character.
 */
FLECS_API
const char* ecs_parse_eol_and_whitespace(
    const char *ptr);

/** Parse digit.
 * This function will parse until the first non-digit character is found. The
 * provided expression must contain at least one digit character.
 * 
 * @param ptr The expression to parse.
 * @param token The output buffer.
 * @return Pointer to the first non-digit character.
 */
FLECS_API
const char* ecs_parse_digit(
    const char *ptr,
    char *token);

/** Skip whitespaces and comments.
 * This function skips whitespace characters and comments (single line, //).
 * 
 * @param ptr pointer to (potential) whitespaces/comments to skip.
 * @return pointer to the next non-whitespace character.
 */
FLECS_API
const char* ecs_parse_fluff(
    const char *ptr,
    char **last_comment);

/** Parse a single token.
 * This function can be used as simple tokenizer by other parsers.
 * 
 * @param name of program (used for logging).
 * @param expr pointer to token to parse.
 * @param ptr pointer to first character to parse.
 * @param token_out Parsed token (buffer should be ECS_MAX_TOKEN_SIZE large)
 * @return Pointer to the next token, or NULL if error occurred.
 */
FLECS_API
const char* ecs_parse_token(
    const char *name,
    const char *expr,
    const char *ptr,
    char *token_out);

/** Parse term in expression.
 * This operation parses a single term in an expression and returns a pointer
 * to the next term expression.
 *
 * If the returned pointer points to the 0-terminator, the expression is fully 
 * parsed. The function would typically be called in a while loop:
 *
 * const char *ptr = expr;
 * while (ptr[0] && (ptr = ecs_parse_term(world, name, expr, ptr, &term))) { }
 *
 * The operation does not attempt to find entity ids from the names in the
 * expression. Use the ecs_term_resolve_ids function to resolve the identifiers
 * in the parsed term.
 *
 * The returned term will in most cases contain allocated resources, which
 * should freed (or used) by the application. To free the resources for a term,
 * use the ecs_term_free function.
 *
 * The parser accepts expressions in the legacy string format.
 *
 * @param world The world.
 * @param name The name of the expression (optional, improves error logs)
 * @param expr The expression to parse (optional, improves error logs)
 * @param ptr The pointer to the current term (must be in expr).
 * @param term_out Out parameter for the term.
 * @return pointer to next term if successful, NULL if failed.
 */
FLECS_API
char* ecs_parse_term(
    const ecs_world_t *world,
    const char *name,
    const char *expr,
    const char *ptr,
    ecs_term_t *term_out);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // FLECS_PARSER_H

#endif // FLECS_PARSER

#endif
#ifdef FLECS_HTTP
#ifdef FLECS_NO_HTTP
#error "FLECS_NO_HTTP failed: HTTP is required by other addons"
#endif
/**
 * @file http.h
 * @brief HTTP addon.
 * 
 * Minimalistic HTTP server that can receive and reply to simple HTTP requests.
 * The main goal of this addon is to enable remotely connecting to a running
 * Flecs application (for example, with a web-based UI) and request/visualize
 * data from the ECS world.
 * 
 * Each server instance creates a single thread used for receiving requests.
 * Receiving requests are enqueued and handled when the application calls
 * ecs_http_server_dequeue. This increases latency of request handling vs.
 * responding directly in the receive thread, but is better suited for 
 * retrieving data from ECS applications, as requests can be processed by an ECS
 * system without having to lock the world.
 * 
 * This server is intended to be used in a development environment.
 */

#ifdef FLECS_HTTP

#if !defined(FLECS_OS_API_IMPL) && !defined(FLECS_NO_OS_API_IMPL)
#define FLECS_OS_API_IMPL
#endif

#ifndef FLECS_HTTP_H
#define FLECS_HTTP_H

/* Maximum number of headers in request */
#define ECS_HTTP_HEADER_COUNT_MAX (32)

/* Maximum number of query parameters in request */
#define ECS_HTTP_QUERY_PARAM_COUNT_MAX (32)

#ifdef __cplusplus
extern "C" {
#endif

/** HTTP server */
typedef struct ecs_http_server_t ecs_http_server_t;

/** A connection manages communication with the remote host */
typedef struct {
    uint64_t id;
    ecs_http_server_t *server;

    char host[128];
    char port[16];
} ecs_http_connection_t;

/** Helper type used for headers & URL query parameters */
typedef struct {
    const char *key;
    const char *value;
} ecs_http_key_value_t;

/** Supported request methods */
typedef enum {
    EcsHttpGet,
    EcsHttpPost,
    EcsHttpPut,
    EcsHttpDelete,
    EcsHttpOptions,
    EcsHttpMethodUnsupported
} ecs_http_method_t;

/** A request */
typedef struct {
    uint64_t id;

    ecs_http_method_t method;
    char *path;
    char *body;
    ecs_http_key_value_t headers[ECS_HTTP_HEADER_COUNT_MAX];
    ecs_http_key_value_t params[ECS_HTTP_HEADER_COUNT_MAX];
    int32_t header_count;
    int32_t param_count;

    ecs_http_connection_t *conn;
} ecs_http_request_t;

/** A reply */
typedef struct {
    int code;                   /* default = 200 */
    ecs_strbuf_t body;          /* default = "" */
    const char* status;         /* default = OK */
    const char* content_type;   /* default = application/json */
    ecs_strbuf_t headers;       /* default = "" */
} ecs_http_reply_t;

#define ECS_HTTP_REPLY_INIT \
    (ecs_http_reply_t){200, ECS_STRBUF_INIT, "OK", "application/json", ECS_STRBUF_INIT}

/** Request callback.
 * Invoked for each valid request. The function should populate the reply and
 * return true. When the function returns false, the server will reply with a 
 * 404 (Not found) code. */
typedef bool (*ecs_http_reply_action_t)(
    const ecs_http_request_t* request, 
    ecs_http_reply_t *reply,
    void *ctx);

/** Used with ecs_http_server_init. */
typedef struct {
    ecs_http_reply_action_t callback; /* Function called for each request  */
    void *ctx;                        /* Passed to callback (optional) */
    uint16_t port;                    /* HTTP port */
    const char *ipaddr;               /* Interface to listen on (optional) */
} ecs_http_server_desc_t;

/** Create server. 
 * Use ecs_http_server_start to start receiving requests.
 * 
 * @param desc Server configuration parameters.
 * @return The new server, or NULL if creation failed.
 */
FLECS_API
ecs_http_server_t* ecs_http_server_init(
    const ecs_http_server_desc_t *desc);

/** Destroy server. 
 * This operation will stop the server if it was still running.
 * 
 * @param server The server to destroy.
 */
FLECS_API
void ecs_http_server_fini(
    ecs_http_server_t* server);

/** Start server. 
 * After this operation the server will be able to accept requests.
 * 
 * @param server The server to start.
 * @return Zero if successful, non-zero if failed.
 */
FLECS_API
int ecs_http_server_start(
    ecs_http_server_t* server);

/** Process server requests. 
 * This operation invokes the reply callback for each received request. No new
 * requests will be enqueued while processing requests.
 * 
 * @param server The server for which to process requests.
 */
FLECS_API
void ecs_http_server_dequeue(
    ecs_http_server_t* server,
    float delta_time);

/** Stop server. 
 * After this operation no new requests can be received.
 * 
 * @param server The server.
 */
FLECS_API
void ecs_http_server_stop(
    ecs_http_server_t* server);

/** Find header in request. 
 * 
 * @param req The request.
 * @param name name of the header to find
 * @return The header value, or NULL if not found.
*/
FLECS_API
const char* ecs_http_get_header(
    const ecs_http_request_t* req,
    const char* name);

/** Find query parameter in request. 
 * 
 * @param req The request.
 * @param name The parameter name.
 * @return The decoded parameter value, or NULL if not found.
 */
FLECS_API
const char* ecs_http_get_param(
    const ecs_http_request_t* req,
    const char* name);

#ifdef __cplusplus
}
#endif

#endif // FLECS_HTTP_H

#endif // FLECS_HTTP

#endif
#ifdef FLECS_OS_API_IMPL
#ifdef FLECS_NO_OS_API_IMPL
#error "FLECS_NO_OS_API_IMPL failed: OS_API_IMPL is required by other addons"
#endif
/**
 * @file os_api_impl.h
 * @brief Default OS API implementation.
 */

#ifdef FLECS_OS_API_IMPL

#ifndef FLECS_OS_API_IMPL_H
#define FLECS_OS_API_IMPL_H

#ifdef __cplusplus
extern "C" {
#endif

FLECS_API
void ecs_set_os_api_impl(void);

#ifdef __cplusplus
}
#endif

#endif // FLECS_OS_API_IMPL_H

#endif // FLECS_OS_API_IMPL

#endif
#ifdef FLECS_MODULE
#ifdef FLECS_NO_MODULE
#error "FLECS_NO_MODULE failed: MODULE is required by other addons"
#endif
/**
 * @file module.h
 * @brief Module addon.
 *
 * The module addon allows for creating and importing modules. Flecs modules 
 * enable applications to organize components and systems into reusable units of
 * code that can easily be across projects.
 */

#ifdef FLECS_MODULE

#ifndef FLECS_MODULE_H
#define FLECS_MODULE_H

#ifdef __cplusplus
extern "C" {
#endif

/** Import a module.
 * This operation will load a modules and store the public module handles in the
 * handles_out out parameter. The module name will be used to verify if the
 * module was already loaded, in which case it won't be reimported. The name
 * will be translated from PascalCase to an entity path (pascal.case) before the
 * lookup occurs.
 *
 * Module contents will be stored as children of the module entity. This 
 * prevents modules from accidentally defining conflicting identifiers. This is
 * enforced by setting the scope before and after loading the module to the 
 * module entity id.
 *
 * A more convenient way to import a module is by using the ECS_IMPORT macro.
 *
 * @param world The world.
 * @param module The module import function.
 * @param module_name The name of the module.
 * @return The module entity.
 */
FLECS_API
ecs_entity_t ecs_import(
    ecs_world_t *world,
    ecs_module_action_t module,
    const char *module_name);

/* Import a module from a library.
 * Similar to ecs_import, except that this operation will attempt to load the 
 * module from a dynamic library.
 *
 * A library may contain multiple modules, which is why both a library name and
 * a module name need to be provided. If only a library name is provided, the
 * library name will be reused for the module name.
 *
 * The library will be looked up using a canonical name, which is in the same
 * form as a module, like `flecs.components.transform`. To transform this
 * identifier to a platform specific library name, the operation relies on the
 * module_to_dl callback of the os_api which the application has to override if
 * the default does not yield the correct library name.
 *
 * @param world The world.
 * @param library_name The name of the library to load.
 * @param module_name The name of the module to load.
 */
FLECS_API
ecs_entity_t ecs_import_from_library(
    ecs_world_t *world,
    const char *library_name,
    const char *module_name);

/** Register a new module.
 */
FLECS_API
ecs_entity_t ecs_module_init(
    ecs_world_t *world,
    const ecs_component_desc_t *desc);

/** Define module
 */
#define ECS_MODULE(world, id)\
    ecs_entity_t ecs_id(id) = ecs_module_init(world, &(ecs_component_desc_t){\
        .entity = {\
            .name = #id,\
            .add = {EcsModule}\
        }\
    });\
    ecs_set_scope(world, ecs_id(id));\
    (void)ecs_id(id);

/** Wrapper around ecs_import.
 * This macro provides a convenient way to load a module with the world. It can
 * be used like this:
 *
 * ECS_IMPORT(world, FlecsSystemsPhysics, 0);
 * 
 * This macro will define entity and type handles for the component associated
 * with the module. The module component will be created as a singleton. 
 * 
 * The contents of a module component are module specific, although they
 * typically contain handles to the content of the module.
 */
#define ECS_IMPORT(world, id) \
    char *FLECS__##id##_name = ecs_module_path_from_c(#id);\
    ecs_id_t ecs_id(id) = ecs_import(world, id##Import, FLECS__##id##_name);\
    ecs_os_free(FLECS__##id##_name);\
    (void)ecs_id(id)

#ifdef __cplusplus
}
#endif

#endif

#endif

#endif

#ifdef FLECS_CPP
#ifdef FLECS_NO_CPP
#error "FLECS_NO_CPP failed: CPP is required by other addons"
#endif
/**
 * @file flecs_cpp.h
 * @brief C++ utility functions
 *
 * This header contains utility functions that are accessible from both C and
 * C++ code. These functions are not part of the public API and are not meant
 * to be used directly by applications.
 */

#ifdef FLECS_CPP

#ifndef FLECS_CPP_H
#define FLECS_CPP_H

#ifdef __cplusplus
extern "C" {
#endif

#if defined(__clang__)
#define ECS_FUNC_NAME_FRONT(type, name) ((sizeof(#type) + sizeof(" flecs::_::() [T = ") + sizeof(#name)) - 3u)
#define ECS_FUNC_NAME_BACK (sizeof("]") - 1u)
#define ECS_FUNC_NAME __PRETTY_FUNCTION__
#elif defined(__GNUC__)
#define ECS_FUNC_NAME_FRONT(type, name) ((sizeof(#type) + sizeof(" flecs::_::() [with T = ") + sizeof(#name)) - 3u)
#define ECS_FUNC_NAME_BACK (sizeof("]") - 1u)
#define ECS_FUNC_NAME __PRETTY_FUNCTION__
#elif defined(_WIN32)
#define ECS_FUNC_NAME_FRONT(type, name) ((sizeof(#type) + sizeof(" __cdecl flecs::_::<") + sizeof(#name)) - 3u)
#define ECS_FUNC_NAME_BACK (sizeof(">(void)") - 1u)
#define ECS_FUNC_NAME __FUNCSIG__
#else
#error "implicit component registration not supported"
#endif

#define ECS_FUNC_TYPE_LEN(type, name, str)\
    (flecs::string::length(str) - (ECS_FUNC_NAME_FRONT(type, name) + ECS_FUNC_NAME_BACK))

FLECS_API
char* ecs_cpp_get_type_name(
    char *type_name, 
    const char *func_name,
    size_t len);

FLECS_API
char* ecs_cpp_get_symbol_name(
    char *symbol_name,
    const char *type_name,
    size_t len);

FLECS_API
char* ecs_cpp_get_constant_name(
    char *constant_name,
    const char *func_name,
    size_t len);

FLECS_API
const char* ecs_cpp_trim_module(
    ecs_world_t *world,
    const char *type_name);

FLECS_API
void ecs_cpp_component_validate(
    ecs_world_t *world,
    ecs_entity_t id,
    const char *name,
    size_t size,
    size_t alignment,
    bool implicit_name);

FLECS_API
ecs_entity_t ecs_cpp_component_register(
    ecs_world_t *world,
    ecs_entity_t id,
    const char *name,
    const char *symbol,
    ecs_size_t size,
    ecs_size_t alignment,
    bool implicit_name);

FLECS_API
ecs_entity_t ecs_cpp_component_register_explicit(
    ecs_world_t *world,
    ecs_entity_t s_id,
    ecs_entity_t id,
    const char *name,
    const char *type_name,
    const char *symbol,
    size_t size,
    size_t alignment,
    bool is_component);

FLECS_API
ecs_entity_t ecs_cpp_enum_constant_register(
    ecs_world_t *world,
    ecs_entity_t parent,
    ecs_entity_t id,
    const char *name,
    int value);

FLECS_API 
int32_t ecs_cpp_reset_count_get(void);

FLECS_API
int32_t ecs_cpp_reset_count_inc(void);

#ifdef __cplusplus
}
#endif

#endif // FLECS_CPP_H

#endif // FLECS_CPP


#ifdef __cplusplus
}

/**
 * @file flecs.hpp
 * @brief Flecs C++ API.
 *
 * Modern C++11 API
 */

#pragma once

// STL includes
#include <type_traits>

// Forward declarations
namespace flecs 
{

struct world;
struct world_async_stage;
struct iter;
struct entity_view;
struct entity;
struct untyped_component;

template <typename T>
struct component;

namespace _ 
{
template <typename T, typename U = int>
struct cpp_type;

template <typename Func, typename ... Components>
struct each_invoker;

} // namespace _
} // namespace flecs

// Types imported from C API
////////////////////////////////////////////////////////////////////////////////
//// Aliases for types/constants from C API
////////////////////////////////////////////////////////////////////////////////

namespace flecs {

using world_t = ecs_world_t;
using id_t = ecs_id_t;
using entity_t = ecs_entity_t;
using type_t = ecs_type_t;
using table_t = ecs_table_t;
using filter_t = ecs_filter_t;
using query_t = ecs_query_t;
using rule_t = ecs_rule_t;
using ref_t = ecs_ref_t;
using iter_t = ecs_iter_t;
using ComponentLifecycle = EcsComponentLifecycle;

enum inout_kind_t {
    InOutDefault = EcsInOutDefault,
    InOutFilter = EcsInOutFilter,
    InOut = EcsInOut,
    In = EcsIn,
    Out = EcsOut
};

enum oper_kind_t {
    And = EcsAnd,
    Or = EcsOr,
    Not = EcsNot,
    Optional = EcsOptional,
    AndFrom = EcsAndFrom,
    OrFrom = EcsOrFrom,
    NotFrom = EcsNotFrom
};

enum var_kind_t {
    VarDefault = EcsVarDefault,
    VarIsEntity = EcsVarIsEntity,
    VarIsVariable = EcsVarIsVariable
};

////////////////////////////////////////////////////////////////////////////////
//// Builtin components and tags 
////////////////////////////////////////////////////////////////////////////////

/* Builtin components */
using Component = EcsComponent;
using Type = EcsType;
using Identifier = EcsIdentifier;
using Query = EcsQuery;
using Trigger = EcsTrigger;
using Observer = EcsObserver;

/* Builtin opaque components */
static const flecs::entity_t System = ecs_id(EcsSystem);

/* Builtin set constants */
static const uint8_t DefaultSet = EcsDefaultSet;
static const uint8_t Self = EcsSelf;
static const uint8_t SuperSet = EcsSuperSet;
static const uint8_t SubSet = EcsSubSet;
static const uint8_t Cascade = EcsCascade;
static const uint8_t All = EcsAll;
static const uint8_t Nothing = EcsNothing;
static const uint8_t Parent = EcsParent;

/* Builtin tag ids */
static const flecs::entity_t Private = EcsPrivate;
static const flecs::entity_t Module = EcsModule;
static const flecs::entity_t Prefab = EcsPrefab;
static const flecs::entity_t Disabled = EcsDisabled;
static const flecs::entity_t Inactive = EcsInactive;
static const flecs::entity_t Monitor = EcsMonitor;
static const flecs::entity_t Pipeline = EcsPipeline;

/* Trigger tags */
static const flecs::entity_t OnAdd = EcsOnAdd;
static const flecs::entity_t OnRemove = EcsOnRemove;
static const flecs::entity_t OnSet = EcsOnSet;
static const flecs::entity_t UnSet = EcsUnSet;

/** Builtin roles */
static const flecs::entity_t Pair = ECS_PAIR;
static const flecs::entity_t Switch = ECS_SWITCH;
static const flecs::entity_t Case = ECS_CASE;
static const flecs::entity_t Override = ECS_OVERRIDE;

/* Builtin entity ids */
static const flecs::entity_t Flecs = EcsFlecs;
static const flecs::entity_t FlecsCore = EcsFlecsCore;
static const flecs::entity_t World = EcsWorld;

/* Relation properties */
static const flecs::entity_t Wildcard = EcsWildcard;
static const flecs::entity_t This = EcsThis;
static const flecs::entity_t Transitive = EcsTransitive;
static const flecs::entity_t Reflexive = EcsReflexive;
static const flecs::entity_t Final = EcsFinal;
static const flecs::entity_t DontInherit = EcsDontInherit;
static const flecs::entity_t Tag = EcsTag;
static const flecs::entity_t Exclusive = EcsExclusive;
static const flecs::entity_t Acyclic = EcsAcyclic;
static const flecs::entity_t Symmetric = EcsSymmetric;
static const flecs::entity_t With = EcsWith;
static const flecs::entity_t OneOf = EcsOneOf;

/* Builtin relationships */
static const flecs::entity_t IsA = EcsIsA;
static const flecs::entity_t ChildOf = EcsChildOf;

/* Builtin identifiers */
static const flecs::entity_t Name = EcsName;
static const flecs::entity_t Symbol = EcsSymbol;

/* Cleanup policies */
static const flecs::entity_t OnDelete = EcsOnDelete;
static const flecs::entity_t OnDeleteObject = EcsOnDeleteObject;
static const flecs::entity_t Remove = EcsRemove;
static const flecs::entity_t Delete = EcsDelete;
static const flecs::entity_t Panic = EcsPanic;

}


// C++ utilities
////////////////////////////////////////////////////////////////////////////////
//// Flecs STL (FTL?)
//// Minimalistic utilities that allow for STL like functionality without having
//// to depend on the actual STL.
////////////////////////////////////////////////////////////////////////////////

// Macros so that C++ new calls can allocate using ecs_os_api memory allocation functions
// Rationale:
//  - Using macros here instead of a templated function bc clients might override ecs_os_malloc
//    to contain extra debug info like source tracking location. Using a template function
//    in that scenario would collapse all source location into said function vs. the
//    actual call site
//  - FLECS_PLACEMENT_NEW(): exists to remove any naked new calls/make it easy to identify any regressions
//    by grepping for new/delete

#define FLECS_PLACEMENT_NEW(_ptr, _type)  ::new(flecs::_::placement_new_tag, _ptr) _type
#define FLECS_NEW(_type)                  FLECS_PLACEMENT_NEW(ecs_os_malloc(sizeof(_type)), _type)
#define FLECS_DELETE(_ptr)          \
  do {                              \
    if (_ptr) {                     \
      flecs::_::destruct_obj(_ptr); \
      ecs_os_free(_ptr);            \
    }                               \
  } while (false)

/* Faster (compile time) alternatives to std::move / std::forward. From:
 *   https://www.foonathan.net/2020/09/move-forward/
 */

#define FLECS_MOV(...) \
  static_cast<flecs::remove_reference_t<decltype(__VA_ARGS__)>&&>(__VA_ARGS__)

#define FLECS_FWD(...) \
  static_cast<decltype(__VA_ARGS__)&&>(__VA_ARGS__)

namespace flecs 
{

namespace _
{

// Dummy Placement new tag to disambiguate from any other operator new overrides
struct placement_new_tag_t{};
constexpr placement_new_tag_t placement_new_tag{};
template<class Ty> inline void destruct_obj(Ty* _ptr) { _ptr->~Ty(); }
template<class Ty> inline void free_obj(Ty* _ptr) { 
    if (_ptr) {
        destruct_obj(_ptr); 
        ecs_os_free(_ptr); 
    }
}

} // namespace _

} // namespace flecs

// Allows overriding flecs_static_assert, which is useful when testing
#ifndef flecs_static_assert
#define flecs_static_assert(cond, str) static_assert(cond, str)
#endif

inline void* operator new(size_t,   flecs::_::placement_new_tag_t, void* _ptr) noexcept { return _ptr; }
inline void  operator delete(void*, flecs::_::placement_new_tag_t, void*)      noexcept {              }

namespace flecs
{

// C++11/C++14 convenience template replacements

template <bool V, typename T, typename F>
using conditional_t = typename std::conditional<V, T, F>::type;

template <typename T>
using decay_t = typename std::decay<T>::type;

template <bool V, typename T = void>
using enable_if_t = typename std::enable_if<V, T>::type;

template <typename T>
using remove_pointer_t = typename std::remove_pointer<T>::type;

template <typename T>
using remove_reference_t = typename std::remove_reference<T>::type;

using std::is_base_of;
using std::is_empty;
using std::is_const;
using std::is_pointer;
using std::is_reference;
using std::is_volatile;
using std::is_same;
using std::is_enum;

// Determine constness even if T is a pointer type
template <typename T>
using is_const_p = is_const< remove_pointer_t<T> >;

// Apply cv modifiers from source type to destination type
// (from: https://stackoverflow.com/questions/52559336/add-const-to-type-if-template-arg-is-const)
template<class Src, class Dst>
using transcribe_const_t = conditional_t<is_const<Src>::value, Dst const, Dst>;

template<class Src, class Dst>
using transcribe_volatile_t = conditional_t<is_volatile<Src>::value, Dst volatile, Dst>;

template<class Src, class Dst>
using transcribe_cv_t = transcribe_const_t< Src, transcribe_volatile_t< Src, Dst> >;


// More convenience templates. The if_*_t templates use int as default type
// instead of void. This enables writing code that's a bit less cluttered when
// the templates are used in a template declaration:
//
//     enable_if_t<true>* = nullptr
// vs:
//     if_t<true> = 0

template <bool V>
using if_t = enable_if_t<V, int>;

template <bool V>
using if_not_t = enable_if_t<false == V, int>;

namespace _
{

// Utility to prevent static assert from immediately triggering
template <class... T>
struct always_false {
    static const bool value = false;
};

} // namespace _

} // namespace flecs

#include <stdlib.h>
// Array class. Simple std::array like utility that is mostly there to aid
// template code where template expansion would lead to an array with size 0.

namespace flecs {

template <typename T>
struct array_iterator
{
    explicit array_iterator(T* value, int index) {
        m_value = value;
        m_index = index;
    }

    bool operator!=(array_iterator const& other) const
    {
        return m_index != other.m_index;
    }

    T & operator*() const
    {
        return m_value[m_index];
    }

    array_iterator& operator++()
    {
        ++m_index;
        return *this;
    }

private:
    T* m_value;
    int m_index;
};

template <typename T, size_t Size, class Enable = void> 
struct array final { };

template <typename T, size_t Size>
struct array<T, Size, enable_if_t<Size != 0> > final {
    array() {};

    array(const T (&elems)[Size]) {
        int i = 0;
        for (auto it = this->begin(); it != this->end(); ++ it) {
            *it = elems[i ++];
        }
    }

    T& operator[](int index) {
        return m_array[index];
    }

    T& operator[](size_t index) {
        return m_array[index];
    }

    array_iterator<T> begin() {
        return array_iterator<T>(m_array, 0);
    }

    array_iterator<T> end() {
        return array_iterator<T>(m_array, Size);
    }

    size_t size() {
        return Size;
    }

    T* ptr() {
        return m_array;
    }

    template <typename Func>
    void each(const Func& func) {
        for (auto& elem : *this) {
            func(elem);
        }
    }

private:
    T m_array[Size];
};

template<typename T, size_t Size>
array<T, Size> to_array(const T (&elems)[Size]) {
    return array<T, Size>(elems);
}

// Specialized class for zero-sized array
template <typename T, size_t Size>
struct array<T, Size, enable_if_t<Size == 0>> final {
    array() {};
    array(const T* (&elems)) { (void)elems; }
    T operator[](size_t index) { ecs_os_abort(); (void)index; return T(); }
    array_iterator<T> begin() { return array_iterator<T>(nullptr, 0); }
    array_iterator<T> end() { return array_iterator<T>(nullptr, 0); }

    size_t size() {
        return 0;
    }

    T* ptr() {
        return NULL;
    }
};

}

// String utility that doesn't implicitly allocate memory.

namespace flecs {

struct string_view;

// This removes dependencies on std::string (and therefore STL) and allows the 
// API to return allocated strings without incurring additional allocations when
// wrapping in an std::string.
struct string {
    explicit string() 
        : m_str(nullptr)
        , m_const_str("")
        , m_length(0) { }

    explicit string(char *str) 
        : m_str(str)
        , m_const_str(str ? str : "")
        , m_length(str ? ecs_os_strlen(str) : 0) { }

    ~string() {
        // If flecs is included in a binary but is not used, it is possible that
        // the OS API is not initialized. Calling ecs_os_free in that case could
        // crash the application during exit. However, if a string has been set
        // flecs has been used, and OS API should have been initialized.
        if (m_str) {
            ecs_os_free(m_str);
        }
    }

    string(string&& str) {
        ecs_os_free(m_str);
        m_str = str.m_str;
        m_const_str = str.m_const_str;
        m_length = str.m_length;
        str.m_str = nullptr;
    }

    operator const char*() const {
        return m_const_str;
    }

    string& operator=(string&& str) {
        ecs_os_free(m_str);
        m_str = str.m_str;
        m_const_str = str.m_const_str;
        m_length = str.m_length;
        str.m_str = nullptr;
        return *this;
    }

    // Ban implicit copies/allocations
    string& operator=(const string& str) = delete;
    string(const string& str) = delete;

    bool operator==(const flecs::string& str) const {
        if (str.m_const_str == m_const_str) {
            return true;
        }

        if (!m_const_str || !str.m_const_str) {
            return false;
        }

        if (str.m_length != m_length) {
            return false;
        }

        return ecs_os_strcmp(str, m_const_str) == 0;
    }

    bool operator!=(const flecs::string& str) const {
        return !(*this == str);
    }    

    bool operator==(const char *str) const {
        if (m_const_str == str) {
            return true;
        }

        if (!m_const_str || !str) {
            return false;
        }

        return ecs_os_strcmp(str, m_const_str) == 0;
    }

    bool operator!=(const char *str) const {
        return !(*this == str);
    }    

    const char* c_str() const {
        return m_const_str;
    }

    std::size_t length() {
        return static_cast<std::size_t>(m_length);
    }

    template <size_t N>
    static constexpr size_t length( char const (&)[N] ) {
        return N - 1;
    }

    std::size_t size() {
        return length();
    }

    void clear() {
        ecs_os_free(m_str);
        m_str = nullptr;
        m_const_str = nullptr;
    }

protected:
    // Must be constructed through string_view. This allows for using the string
    // class for both owned and non-owned strings, which can reduce allocations
    // when code conditionally should store a literal or an owned string.
    // Making this constructor private forces the code to explicitly create a
    // string_view which emphasizes that the string won't be freed by the class.
    string(const char *str)
        : m_str(nullptr)
        , m_const_str(str ? str : "")
        , m_length(str ? ecs_os_strlen(str) : 0) { }

    char *m_str = nullptr;
    const char *m_const_str;
    ecs_size_t m_length;
};

// For consistency, the API returns a string_view where it could have returned
// a const char*, so an application won't have to think about whether to call
// c_str() or not. The string_view is a thin wrapper around a string that forces
// the API to indicate explicitly when a string is owned or not.
struct string_view : string {
    explicit string_view(const char *str)
        : string(str) { }
};

}

#include <string.h>

#define FLECS_ENUM_MAX(T) _::to_constant<T, 128>::value
#define FLECS_ENUM_MAX_COUNT (FLECS_ENUM_MAX(int) + 1)

namespace flecs {

/** Int to enum */
namespace _ {
template <typename E, int Value>
struct to_constant {
    static constexpr E value = static_cast<E>(Value);
};

template <typename E, int Value>
constexpr E to_constant<E, Value>::value;
}

/** Convenience type with enum reflection data */
template <typename E>
struct enum_data;

template <typename E>
static enum_data<E> enum_type(flecs::world_t *world);

template <typename E>
struct enum_last {
    static constexpr E value = FLECS_ENUM_MAX(E);
};

/* Utility macro to override enum_last trait */
#define FLECS_ENUM_LAST(T, Last)\
    namespace flecs {\
    template<>\
    struct enum_last<T> {\
        static constexpr T value = Last;\
    };\
    }

namespace _ {

#ifdef ECS_TARGET_MSVC
#define ECS_SIZE_T_STR "unsigned __int64"
#elif defined(__clang__)
#define ECS_SIZE_T_STR "size_t"
#else
#define ECS_SIZE_T_STR "constexpr size_t; size_t = long unsigned int"
#endif

template <typename E>
constexpr size_t enum_type_len() {
    return ECS_FUNC_TYPE_LEN(, enum_type_len, ECS_FUNC_NAME) 
        - (sizeof(ECS_SIZE_T_STR) - 1u);
}

/** Test if value is valid for enumeration.
 * This function leverages that when a valid value is provided, 
 * __PRETTY_FUNCTION__ contains the enumeration name, whereas if a value is
 * invalid, the string contains a number. */
#if defined(__clang__)
template <typename E, E C>
constexpr bool enum_constant_is_valid() {
    return !(
        (ECS_FUNC_NAME[ECS_FUNC_NAME_FRONT(bool, enum_constant_is_valid) +
            enum_type_len<E>() + 6 /* ', C = ' */] >= '0') &&
        (ECS_FUNC_NAME[ECS_FUNC_NAME_FRONT(bool, enum_constant_is_valid) +
            enum_type_len<E>() + 6 /* ', C = ' */] <= '9'));
}
#elif defined(__GNUC__)
template <typename E, E C>
constexpr bool enum_constant_is_valid() {
    return (ECS_FUNC_NAME[ECS_FUNC_NAME_FRONT(constepxr bool, enum_constant_is_valid) +
        enum_type_len<E>() + 8 /* ', E C = ' */] != '(');
}
#else
/* Use different trick on MSVC, since it uses hexadecimal representation for
 * invalid enum constants. We can leverage that msvc inserts a C-style cast
 * into the name, and the location of its first character ('(') is known. */
template <typename E, E C>
constexpr bool enum_constant_is_valid() {
    return ECS_FUNC_NAME[ECS_FUNC_NAME_FRONT(bool, enum_constant_is_valid) +
        enum_type_len<E>() + 1] != '(';
}
#endif

template <typename E, E C>
struct enum_is_valid {
    static constexpr bool value = enum_constant_is_valid<E, C>();
};

/** Extract name of constant from string */
template <typename E, E C>
static const char* enum_constant_to_name() {
    static const size_t len = ECS_FUNC_TYPE_LEN(const char*, enum_constant_to_name, ECS_FUNC_NAME);
    static char result[len + 1] = {};
    return ecs_cpp_get_constant_name(
        result, ECS_FUNC_NAME, string::length(ECS_FUNC_NAME));
}

/** Enumeration constant data */
struct enum_constant_data {
    flecs::entity_t id;
    int next;
};

/** Enumeration type data */
struct enum_data_impl {
    flecs::entity_t id;
    int min;
    int max;
    enum_constant_data constants[FLECS_ENUM_MAX_COUNT];
};

/** Class that scans an enum for constants, extracts names & creates entities */
template <typename E>
struct enum_type {
    static enum_data_impl data;

    static enum_type<E>& get() {
        static _::enum_type<E> instance;
        return instance;
    }

    flecs::entity_t entity(E value) const {
        return data.constants[static_cast<int>(value)].id;
    }

    void init(flecs::world_t *world, flecs::entity_t id) {
#if !defined(__clang__) && defined(__GNUC__)
        ecs_assert(__GNUC__ > 7 || (__GNUC__ == 7 && __GNUC_MINOR__ >= 5), 
            ECS_UNSUPPORTED, "enum component types require gcc 7.5 or higher");
#endif

        ecs_log_push();
        ecs_add_id(world, id, flecs::Exclusive);
        ecs_add_id(world, id, flecs::OneOf);
        ecs_add_id(world, id, flecs::Tag);
        data.id = id;
        data.min = FLECS_ENUM_MAX(int);
        init< enum_last<E>::value >(world);
        ecs_log_pop();
    }

private:
    template <E Value>
    static constexpr int to_int() {
        return static_cast<int>(Value);
    }

    template <int Value>
    static constexpr E from_int() {
        return to_constant<E, Value>::value;
    }

    template <E Value>
    static constexpr int is_not_0() {
        return static_cast<int>(Value != from_int<0>());
    }

    template <E Value, flecs::if_not_t< enum_constant_is_valid<E, Value>() > = 0>
    static void init_constant(flecs::world_t*) { }

    template <E Value, flecs::if_t< enum_constant_is_valid<E, Value>() > = 0>
    static void init_constant(flecs::world_t *world) {
        int v = to_int<Value>();
        const char *name = enum_constant_to_name<E, Value>();
        data.constants[v].next = data.min;
        data.min = v;
        if (!data.max) {
            data.max = v;
        }

        data.constants[v].id = ecs_cpp_enum_constant_register(
            world, data.id, data.constants[v].id, name, v);
    }

    template <E Value = FLECS_ENUM_MAX(E) >
    static void init(flecs::world_t *world) {
        init_constant<Value>(world);
        if (is_not_0<Value>()) {
            init<from_int<to_int<Value>() - is_not_0<Value>()>()>(world);
        }
    }
};

template <typename E>
enum_data_impl enum_type<E>::data;

template <typename E, if_t< is_enum<E>::value > = 0>
inline static void init_enum(flecs::world_t *world, flecs::entity_t id) {
    _::enum_type<E>::get().init(world, id);
}

template <typename E, if_not_t< is_enum<E>::value > = 0>
inline static void init_enum(flecs::world_t*, flecs::entity_t) { }

} // namespace _

/** Enumeration type data wrapper with world pointer */
template <typename E>
struct enum_data {
    enum_data(flecs::world_t *world, _::enum_data_impl& impl) 
        : world_(world)
        , impl_(impl) { }

    bool is_valid(int value) {
        return impl_.constants[value].id != 0;
    }

    int first() const {
        return impl_.min;
    }

    int last() const {
        return impl_.max;
    }

    int next(int cur) const {
        return impl_.constants[cur].next;
    }

    flecs::entity entity() const;
    flecs::entity entity(int value) const;
    flecs::entity entity(E value) const;

    flecs::world_t *world_;
    _::enum_data_impl& impl_;
};

/** Convenience function for getting enum reflection data */
template <typename E>
enum_data<E> enum_type(flecs::world_t *world) {
    _::cpp_type<E>::id(world); // Ensure enum is registered
    auto& ref = _::enum_type<E>::get();
    return enum_data<E>(world, ref.data);
}

} // namespace flecs

// Wrapper around ecs_strbuf_t that provides a simple stringstream like API.

namespace flecs {

struct stringstream {
    explicit stringstream() 
        : m_buf({}) { }

    ~stringstream() {
        ecs_strbuf_reset(&m_buf);
    }

    stringstream(stringstream&& str) {
        ecs_strbuf_reset(&m_buf);
        m_buf = str.m_buf;
        str.m_buf = {};
    }

    stringstream& operator=(stringstream&& str) {
        ecs_strbuf_reset(&m_buf);
        m_buf = str.m_buf;
        str.m_buf = {};
        return *this;
    }

    // Ban implicit copies/allocations
    stringstream& operator=(const stringstream& str) = delete;
    stringstream(const stringstream& str) = delete;    

    stringstream& operator<<(const char* str) {
        ecs_strbuf_appendstr(&m_buf, str);
        return *this;
    }

    flecs::string str() {
        return flecs::string(ecs_strbuf_get(&m_buf));
    }

private:
    ecs_strbuf_t m_buf;
};

}


// Neat utility to inspect arguments & returntype of a function type
// Code from: https://stackoverflow.com/questions/27024238/c-template-mechanism-to-get-the-number-of-function-arguments-which-would-work

namespace flecs {
namespace _ {

template <typename ... Args>
struct arg_list { };

// Base type that contains the traits
template <typename ReturnType, typename... Args>
struct function_traits_defs
{
    static constexpr bool is_callable = true;
    static constexpr size_t arity = sizeof...(Args);
    using return_type = ReturnType;
    using args = arg_list<Args ...>;
};

// Primary template for function_traits_impl
template <typename T>
struct function_traits_impl {
    static constexpr bool is_callable = false;
};

// Template specializations for the different kinds of function types (whew)
template <typename ReturnType, typename... Args>
struct function_traits_impl<ReturnType(Args...)>
    : function_traits_defs<ReturnType, Args...> {};

template <typename ReturnType, typename... Args>
struct function_traits_impl<ReturnType(*)(Args...)>
    : function_traits_defs<ReturnType, Args...> {};

template <typename ClassType, typename ReturnType, typename... Args>
struct function_traits_impl<ReturnType(ClassType::*)(Args...)>
    : function_traits_defs<ReturnType, Args...> {};

template <typename ClassType, typename ReturnType, typename... Args>
struct function_traits_impl<ReturnType(ClassType::*)(Args...) const>
    : function_traits_defs<ReturnType, Args...> {};    

template <typename ClassType, typename ReturnType, typename... Args>
struct function_traits_impl<ReturnType(ClassType::*)(Args...) const&>
    : function_traits_defs<ReturnType, Args...> {};
    
template <typename ClassType, typename ReturnType, typename... Args>
struct function_traits_impl<ReturnType(ClassType::*)(Args...) const&&>
    : function_traits_defs<ReturnType, Args...> {};
    
template <typename ClassType, typename ReturnType, typename... Args>
struct function_traits_impl<ReturnType(ClassType::*)(Args...) volatile>
    : function_traits_defs<ReturnType, Args...> {};

template <typename ClassType, typename ReturnType, typename... Args>
struct function_traits_impl<ReturnType(ClassType::*)(Args...) volatile&>
    : function_traits_defs<ReturnType, Args...> {};
    
template <typename ClassType, typename ReturnType, typename... Args>
struct function_traits_impl<ReturnType(ClassType::*)(Args...) volatile&&>
    : function_traits_defs<ReturnType, Args...> {};

template <typename ClassType, typename ReturnType, typename... Args>
struct function_traits_impl<ReturnType(ClassType::*)(Args...) const volatile>
    : function_traits_defs<ReturnType, Args...> {};

template <typename ClassType, typename ReturnType, typename... Args>
struct function_traits_impl<ReturnType(ClassType::*)(Args...) const volatile&>
    : function_traits_defs<ReturnType, Args...> {};
    
template <typename ClassType, typename ReturnType, typename... Args>
struct function_traits_impl<ReturnType(ClassType::*)(Args...) const volatile&&>
    : function_traits_defs<ReturnType, Args...> {};

// Primary template for function_traits_no_cv. If T is not a function, the
// compiler will attempt to instantiate this template and fail, because its base
// is undefined.
template <typename T, typename V = void>
struct function_traits_no_cv
    : function_traits_impl<T> {};

// Specialized template for function types
template <typename T>
struct function_traits_no_cv<T, decltype((void)&T::operator())>
    : function_traits_impl<decltype(&T::operator())> {};
 
// Front facing template that decays T before ripping it apart.
template <typename T>
struct function_traits
    : function_traits_no_cv< decay_t<T> > {};

} // _


template <typename T>
struct is_callable {
    static constexpr bool value = _::function_traits<T>::is_callable;
};

template <typename T>
struct arity {
    static constexpr int value = _::function_traits<T>::arity;
};

template <typename T>
using return_type_t = typename _::function_traits<T>::return_type;

template <typename T>
using arg_list_t = typename _::function_traits<T>::args;


template<typename Func, typename ... Args>
struct first_arg_impl;

template<typename Func, typename T, typename ... Args>
struct first_arg_impl<Func, _::arg_list<T, Args ...> > {
    using type = T;
};

template<typename Func>
struct first_arg {
    using type = typename first_arg_impl<Func, arg_list_t<Func>>::type;
};

template <typename Func>
using first_arg_t = typename first_arg<Func>::type;

} // flecs



// Mixin forward declarations
#pragma once

namespace flecs {

struct id;
struct entity;

/** Class that stores a flecs id.
 * A flecs id is an identifier that can store an entity id, an relation-object 
 * pair, or role annotated id (such as SWITCH | Movement).
 */
struct id {
    id()
        : m_world(nullptr)
        , m_id(0) { }
    
    explicit id(flecs::id_t value) 
        : m_world(nullptr)
        , m_id(value) { }

    explicit id(flecs::world_t *world, flecs::id_t value = 0)
        : m_world(world)
        , m_id(value) { }

    explicit id(flecs::world_t *world, flecs::id_t relation, flecs::id_t object)
        : m_world(world)
        , m_id(ecs_pair(relation, object)) { }

    explicit id(flecs::id_t relation, flecs::id_t object)
        : m_world(nullptr)
        , m_id(ecs_pair(relation, object)) { }

    explicit id(const flecs::id& relation, const flecs::id& object)
        : m_world(relation.m_world)
        , m_id(ecs_pair(relation.m_id, object.m_id)) { }

    /** Test if id is pair (has relation, object) */
    bool is_pair() const {
        return (m_id & ECS_ROLE_MASK) == flecs::Pair;
    }

    /* Test if id is a wildcard */
    bool is_wildcard() const {
        return ecs_id_is_wildcard(m_id);
    }

    /* Test if id has the Switch role */
    bool is_switch() const {
        return (m_id & ECS_ROLE_MASK) == flecs::Switch;
    }

    /* Test if id has the Case role */
    bool is_case() const {
        return (m_id & ECS_ROLE_MASK) == flecs::Case;
    }

    /* Test if id is entity */
    bool is_entity() const {
        return !(m_id & ECS_ROLE_MASK);
    }

    /* Return id as entity (only allowed when id is valid entity) */
    flecs::entity entity() const;

    /* Return id with role added */
    flecs::entity add_role(flecs::id_t role) const;

    /* Return id with role removed */
    flecs::entity remove_role(flecs::id_t role) const;

    /* Return id without role */
    flecs::entity remove_role() const;

    /* Return id without role */
    flecs::entity remove_generation() const;    

    /* Return component type of id */
    flecs::entity type_id() const;

    /* Test if id has specified role */
    bool has_role(flecs::id_t role) const {
        return ((m_id & ECS_ROLE_MASK) == role);
    }

    /* Test if id has any role */
    bool has_role() const {
        return (m_id & ECS_ROLE_MASK) != 0;
    }

    flecs::entity role() const;

    /* Test if id has specified relation */
    bool has_relation(flecs::id_t relation) const {
        if (!is_pair()) {
            return false;
        }
        return ECS_PAIR_FIRST(m_id) == relation;
    }

    /** Get first element from a pair.
     * If the id is not a pair, this operation will fail. When the id has a
     * world, the operation will ensure that the returned id has the correct
     * generation count. */
    flecs::entity first() const;

    /** Get second element from a pair.
     * If the id is not a pair, this operation will fail. When the id has a
     * world, the operation will ensure that the returned id has the correct
     * generation count. */
    flecs::entity second() const;

    /** Same as first() */
    flecs::entity relation() const;

    /** Same as second() */
    flecs::entity object() const;

    /* Convert id to string */
    flecs::string str() const {
        return flecs::string(ecs_id_str(m_world, m_id));
    }

    /** Convert role of id to string. */
    flecs::string role_str() const {
        return flecs::string_view( ecs_role_str(m_id & ECS_ROLE_MASK));
    }

    flecs::id_t raw_id() const {
        return m_id;
    }

    operator flecs::id_t() const {
        return m_id;
    }

    flecs::world world() const;
    
protected:
    /* World is optional, but guarantees that entity identifiers extracted from
     * the id are valid */
    flecs::world_t *m_world;
    flecs::id_t m_id;
};

}

#pragma once

namespace flecs {

struct type;

}

#pragma once

namespace flecs {

struct term;
struct term_builder;

}

#pragma once

namespace flecs {

struct filter_base;

template<typename ... Components>
struct filter;

template<typename ... Components>
struct filter_builder;

}

#pragma once

#pragma once

#define ECS_EVENT_DESC_ID_COUNT_MAX (8)

namespace flecs {

// Event builder interface
template <typename Base, typename E>
struct event_builder_base {
    event_builder_base(flecs::world_t *world, flecs::entity_t event)
        : m_world(world)
        , m_desc{}
        , m_ids{}
        , m_ids_array{}
    {
        m_desc.event = event;
    }

    /** Add component to trigger on */
    template <typename T>
    Base& id() {
        m_ids.array = m_ids_array;
        m_ids.array[m_ids.count] = _::cpp_type<T>().id(m_world);
        m_ids.count ++;
        return *this;
    }
    
    /** 
     * Add pair to trigger on
     * @tparam R the relation type.
     * @tparam O the object type.
     */
    template <typename R, typename O>
    Base& id() {
        return id(
            ecs_pair(_::cpp_type<R>::id(this->m_world), 
                _::cpp_type<O>::id(this->m_world)));
    }

    /** 
     * Add pair to trigger on
     * @tparam R the relation type.
     * @param object The object id.
     */
    template <typename R>
    Base& id(entity_t object) {
        return id(ecs_pair(_::cpp_type<R>::id(this->m_world), object));
    }

    /** 
     * Add pair to trigger on
     * @param relation The relation type.
     * @param object The object id.
     */
    Base& id(entity_t relation, entity_t object) {
        return id(ecs_pair(relation, object));
    }

    /** Add (component) id to trigger on */
    Base& id(flecs::id_t id) {
        m_ids.array = m_ids_array;
        m_ids.array[m_ids.count] = id;
        m_ids.count ++;
        return *this;
    }

    /** Set entity for which to trigger */
    Base& entity(flecs::entity_t e) {
        ecs_record_t *r = ecs_record_find(m_world, e);
        
        /* can't trigger for empty entity */
        ecs_assert(r != nullptr, ECS_INVALID_PARAMETER, nullptr);
        ecs_assert(r->table != nullptr, ECS_INVALID_PARAMETER, nullptr);

        m_desc.table = r->table;
        m_desc.offset = ECS_RECORD_TO_ROW(r->row);
        m_desc.count = 1;
        return *this;
    }

    /* Set table for which to trigger */
    Base& table(flecs::table_t *t, int32_t offset = 0, int32_t count = 0) {
        m_desc.table = t;
        m_desc.offset = offset;
        m_desc.count = count;
        return *this;
    }

    /* Set event data */
    Base& ctx(const E* ptr) {
        m_desc.param = ptr;
        return *this;
    }

    void emit() {
        ecs_assert(m_ids.count != 0, ECS_INVALID_PARAMETER, NULL);
        ecs_assert(m_desc.table != nullptr, ECS_INVALID_PARAMETER, NULL);
        m_ids.array = m_ids_array;
        m_desc.ids = &m_ids;
        m_desc.observable = const_cast<flecs::world_t*>(ecs_get_world(m_world));
        ecs_emit(m_world, &m_desc);
    }

protected:
    flecs::world_t *m_world;
    ecs_event_desc_t m_desc;
    flecs::type_t m_ids;
    flecs::id_t m_ids_array[ECS_EVENT_DESC_ID_COUNT_MAX];

private:
    operator Base&() {
        return *static_cast<Base*>(this);
    }
};

struct event_builder : event_builder_base<event_builder, void> {
    using event_builder_base::event_builder_base;
};

template <typename E>
struct event_builder_typed : event_builder_base<event_builder_typed<E>, E> {
private:
    using Class = event_builder_typed<E>;

public:
    using event_builder_base<Class, E>::event_builder_base;

    /* Set event data */
    Class& ctx(const E& ptr) {
        this->m_desc.param = &ptr;
        return *this;
    }
};

}


#pragma once

namespace flecs {

struct query_base;

template<typename ... Components>
struct query;

template<typename ... Components>
struct query_builder;

}

#pragma once

namespace flecs {

struct trigger;

template<typename ... Components>
struct trigger_builder;

}

#pragma once

namespace flecs {

struct observer;

template<typename ... Components>
struct observer_builder;

}

#ifdef FLECS_SYSTEM
#pragma once

namespace flecs {

using TickSource = EcsTickSource;

struct system;

template<typename ... Components>
struct system_builder;

namespace _ {

void system_init(flecs::world& world);

} // namespace _
} // namespace flecs

#endif
#ifdef FLECS_PIPELINE
#pragma once

namespace flecs {

struct pipeline;

/* Builtin pipeline tags */
static const flecs::entity_t PreFrame = EcsPreFrame;
static const flecs::entity_t OnLoad = EcsOnLoad;
static const flecs::entity_t PostLoad = EcsPostLoad;
static const flecs::entity_t PreUpdate = EcsPreUpdate;
static const flecs::entity_t OnUpdate = EcsOnUpdate;
static const flecs::entity_t OnValidate = EcsOnValidate;
static const flecs::entity_t PostUpdate = EcsPostUpdate;
static const flecs::entity_t PreStore = EcsPreStore;
static const flecs::entity_t OnStore = EcsOnStore;
static const flecs::entity_t PostFrame = EcsPostFrame;

}

#endif
#ifdef FLECS_TIMER
#pragma once

namespace flecs {

using Timer = EcsTimer;
using RateFilter = EcsRateFilter;

struct timer;

namespace _ {

void timer_init(flecs::world& world);

} // namespace _
} // namespace flecs

#endif
#ifdef FLECS_SNAPSHOT
#pragma once

namespace flecs {

using snapshot_t = ecs_snapshot_t;

struct snapshot;

}

#endif
#ifdef FLECS_DOC
#pragma once

namespace flecs {
namespace doc {

using Description = EcsDocDescription;
static const flecs::entity_t Brief = EcsDocBrief;
static const flecs::entity_t Detail = EcsDocDetail;
static const flecs::entity_t Link = EcsDocLink;

namespace _ {
void init(flecs::world& world);
}

}
}

#endif
#ifdef FLECS_REST
#pragma once

namespace flecs {
namespace rest {

using Rest = EcsRest;

namespace _ {

void init(flecs::world& world);

}
}
}

#endif
#ifdef FLECS_RULES
#pragma once

namespace flecs {

struct rule_base;

template<typename ... Components>
struct rule;

template<typename ... Components>
struct rule_builder;

}

#endif
#ifdef FLECS_META
#pragma once

namespace flecs {

using bool_t = ecs_bool_t;
using char_t = ecs_char_t;
using u8_t = ecs_u8_t;
using u16_t = ecs_u16_t;
using u32_t = ecs_u32_t;
using u64_t = ecs_u64_t;
using uptr_t = ecs_uptr_t;
using i8_t = ecs_i8_t;
using i16_t = ecs_i16_t;
using i32_t = ecs_i32_t;
using i64_t = ecs_i64_t;
using iptr_t = ecs_iptr_t;
using f32_t = ecs_f32_t;
using f64_t = ecs_f64_t;

using type_kind_t = ecs_type_kind_t;
using primitive_kind_t = ecs_primitive_kind_t;
using member_t = ecs_member_t;
using enum_constant_t = ecs_enum_constant_t;
using bitmask_constant_t = ecs_bitmask_constant_t;

using MetaType = EcsMetaType;
using MetaTypeSerialized = EcsMetaTypeSerialized;
using Primitive = EcsPrimitive;
using Enum = EcsEnum;
using Bitmask = EcsBitmask;
using Member = EcsMember;
using Struct = EcsStruct;
using Array = EcsArray;
using Vector = EcsVector;
using Unit = EcsUnit;

static const flecs::entity_t Bool = ecs_id(ecs_bool_t);
static const flecs::entity_t Char = ecs_id(ecs_char_t);
static const flecs::entity_t Byte = ecs_id(ecs_byte_t);
static const flecs::entity_t U8 = ecs_id(ecs_u8_t);
static const flecs::entity_t U16 = ecs_id(ecs_u16_t);
static const flecs::entity_t U32 = ecs_id(ecs_u32_t);
static const flecs::entity_t U64 = ecs_id(ecs_u64_t);
static const flecs::entity_t Uptr = ecs_id(ecs_uptr_t);
static const flecs::entity_t I8 = ecs_id(ecs_i8_t);
static const flecs::entity_t I16 = ecs_id(ecs_i16_t);
static const flecs::entity_t I32 = ecs_id(ecs_i32_t);
static const flecs::entity_t I64 = ecs_id(ecs_i64_t);
static const flecs::entity_t Iptr = ecs_id(ecs_iptr_t);
static const flecs::entity_t F32 = ecs_id(ecs_f32_t);
static const flecs::entity_t F64 = ecs_id(ecs_f64_t);
static const flecs::entity_t String = ecs_id(ecs_string_t);
static const flecs::entity_t Entity = ecs_id(ecs_entity_t);

static const flecs::entity_t Constant = EcsConstant;
static const flecs::entity_t Quantity = EcsQuantity;

namespace meta {

struct cursor {
    cursor(flecs::world_t *world, flecs::entity_t type_id, void *ptr) {
        m_cursor = ecs_meta_cursor(world, type_id, ptr);
    }

    int push() {
        return ecs_meta_push(&m_cursor);
    }

    int pop() {
        return ecs_meta_pop(&m_cursor);
    }

    int next() {
        return ecs_meta_next(&m_cursor);
    }

    int member(const char *name) {
        return ecs_meta_member(&m_cursor, name);
    }

    bool is_collection() {
        return ecs_meta_is_collection(&m_cursor);
    }

    flecs::string_view get_member() const {
        return flecs::string_view(ecs_meta_get_member(&m_cursor));
    }

    flecs::entity get_type() const;

    flecs::entity get_unit() const;

    void* get_ptr() {
        return ecs_meta_get_ptr(&m_cursor);
    }

    int set_bool(bool value) {
        return ecs_meta_set_bool(&m_cursor, value);
    }

    int set_char(char value) {
        return ecs_meta_set_char(&m_cursor, value);
    }

    int set_int(int64_t value) {
        return ecs_meta_set_int(&m_cursor, value);
    }

    int set_uint(uint64_t value) {
        return ecs_meta_set_uint(&m_cursor, value);
    }

    int set_float(double value) {
        return ecs_meta_set_float(&m_cursor, value);
    }

    int set_string(const char *value) {
        return ecs_meta_set_string(&m_cursor, value);
    }

    int set_string_literal(const char *value) {
        return ecs_meta_set_string_literal(&m_cursor, value);
    }

    int set_entity(flecs::entity_t value) {
        return ecs_meta_set_entity(&m_cursor, value);
    }

    int set_null() {
        return ecs_meta_set_null(&m_cursor);
    }


    bool get_bool() const {
        return ecs_meta_get_bool(&m_cursor);
    }

    char get_char() const {
        return ecs_meta_get_char(&m_cursor);
    }

    int64_t get_int() const {
        return ecs_meta_get_int(&m_cursor);
    }

    uint64_t get_uint() const {
        return ecs_meta_get_uint(&m_cursor);
    }

    double get_float() const {
        return ecs_meta_get_float(&m_cursor);
    }

    const char *get_string() const {
        return ecs_meta_get_string(&m_cursor);
    }

    flecs::entity get_entity() const;

    ecs_meta_cursor_t m_cursor;
};

namespace _ {

void init(flecs::world& world);

} // namespace _
} // namespace meta
} // namespace flecs

#endif
#ifdef FLECS_UNITS
#pragma once

namespace flecs {
struct units {

////////////////////////////////////////////////////////////////////////////////
//// Unit prefixes
////////////////////////////////////////////////////////////////////////////////

struct Prefixes { };

struct Yocto { };
struct Zepto { };
struct Atto { };
struct Femto { };
struct Pico { };
struct Nano { };
struct Micro { };
struct Milli { };
struct Centi { };
struct Deci { };
struct Deca { };
struct Hecto { };
struct Kilo { };
struct Mega { };
struct Giga { };
struct Tera { };
struct Peta { };
struct Exa { };
struct Zetta { };
struct Yotta { };
struct Kibi { };
struct Mebi { };
struct Gibi { };
struct Tebi { };
struct Pebi { };
struct Exbi { };
struct Zebi { };
struct Yobi { };


////////////////////////////////////////////////////////////////////////////////
//// Quantities
////////////////////////////////////////////////////////////////////////////////

struct Duration { };
struct Time { };
struct Mass { };
struct ElectricCurrent { };
struct LuminousIntensity { };
struct Force { };
struct Amount { };
struct Length { };
struct Pressure { };
struct Speed { };
struct Temperature { };
struct Data { };
struct DataRate { };
struct Angle { };


////////////////////////////////////////////////////////////////////////////////
//// Units
////////////////////////////////////////////////////////////////////////////////

struct duration {
struct PicoSeconds { };
struct NanoSeconds { };
struct MicroSeconds { };
struct MilliSeconds { };
struct Seconds { };
struct Minutes { };
struct Hours { };
struct Days { };
};

struct angle {
struct Radians { };
struct Degrees { };
};

struct time {
struct Date { };
};

struct mass {
struct Grams { };
struct KiloGrams { };
};

struct electric_current {
struct Ampere { };
};

struct amount {
struct Mole { };
};

struct luminous_intensity {
struct Candela { };
};

struct force {
struct Newton { };
};

struct length {
struct Meters { };
struct PicoMeters { };
struct NanoMeters { };
struct MicroMeters { };
struct MilliMeters { };
struct CentiMeters { };
struct KiloMeters { };
struct Miles { };
};

struct pressure {
struct Pascal { };
struct Bar { };
};

struct speed {
struct MetersPerSecond { };
struct KiloMetersPerSecond { };
struct KiloMetersPerHour { };
struct MilesPerHour { };
};

struct temperature {
struct Kelvin { };
struct Celsius { };
struct Fahrenheit { };
};

struct data {
struct Bits { };
struct KiloBits { };
struct MegaBits { };
struct GigaBits { };
struct Bytes { };
struct KiloBytes { };
struct MegaBytes { };
struct GigaBytes { };
struct KibiBytes { };
struct MebiBytes { };
struct GibiBytes { };
};

struct datarate {
struct BitsPerSecond { };
struct KiloBitsPerSecond { };
struct MegaBitsPerSecond { };
struct GigaBitsPerSecond { };
struct BytesPerSecond { };
struct KiloBytesPerSecond { };
struct MegaBytesPerSecond { };
struct GigaBytesPerSecond { };
};

struct Percentage { };
struct Bel { };
struct DeciBel { };

units(flecs::world& world);

};
}

#endif
#ifdef FLECS_JSON
#pragma once

namespace flecs {

using entity_to_json_desc_t = ecs_entity_to_json_desc_t;
using iter_to_json_desc_t = ecs_iter_to_json_desc_t;

}

#endif
#ifdef FLECS_APP
#pragma once

#pragma once

namespace flecs {

// App builder interface
struct app_builder {
    app_builder(flecs::world_t *world)
        : m_world(world)
        , m_desc{}
    {
        const ecs_world_info_t *stats = ecs_get_world_info(world);
        m_desc.target_fps = stats->target_fps;
        if (m_desc.target_fps == static_cast<FLECS_FLOAT>(0.0)) {
            m_desc.target_fps = 60;
        }
    }

    app_builder& target_fps(FLECS_FLOAT value) {
        m_desc.target_fps = value;
        return *this;
    }

    app_builder& delta_time(FLECS_FLOAT value) {
        m_desc.delta_time = value;
        return *this;
    }

    app_builder& threads(int32_t value) {
        m_desc.threads = value;
        return *this;
    }

    app_builder& enable_rest(bool value = true) {
        m_desc.enable_rest = value;
        return *this;
    }

    app_builder& init(ecs_app_init_action_t value) {
        m_desc.init = value;
        return *this;
    }

    app_builder& ctx(void *value) {
        m_desc.ctx = value;
        return *this;
    }

    int run() {
        return ecs_app_run(m_world, &m_desc);
    }

private:
    flecs::world_t *m_world;
    ecs_app_desc_t m_desc;
};

}



#endif


namespace flecs {
namespace log {

inline void set_level(int level) {
    ecs_log_set_level(level);
}

inline void enable_colors(bool enabled) {
    ecs_log_enable_colors(enabled);
}

inline void dbg(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    ecs_logv(1, fmt, args);
    va_end(args);
}

inline void trace(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    ecs_logv(0, fmt, args);
    va_end(args);
}

inline void warn(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    ecs_logv(-2, fmt, args);
    va_end(args);
}

inline void err(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    ecs_logv(-3, fmt, args);
    va_end(args);
}

inline void push() {
    ecs_log_push();
}

inline void pop() {
    ecs_log_pop();
}

}
}


namespace flecs {

namespace _ {
    struct pair_base { };   
} // _


// Type that represents a pair and can encapsulate a temporary value
template <typename R, typename O>
struct pair : _::pair_base { 
    // Traits used to deconstruct the pair

    // The actual type of the pair is determined by which type of the pair is
    // empty. If both types are empty or not empty, the pair assumes the type
    // of the relation.
    using type = conditional_t<!is_empty<R>::value || is_empty<O>::value, R, O>;
    using relation = R;
    using object = O;

    pair(type& v) : ref_(v) { }

    // This allows the class to be used as a temporary object
    pair(const type& v) : ref_(const_cast<type&>(v)) { }

    operator type&() { 
        return ref_;
    }

    operator const Type&() const { 
        return ref_;
    }    

    type* operator->() {
        return &ref_;
    }

    const type* operator->() const {
        return &ref_;
    }

    type& operator*() {
        return ref_;
    }

    const type& operator*() const {
        return ref_;
    }
    
private:
    type& ref_;
};

template <typename R, typename O, if_t<is_empty<R>::value> = 0>
using pair_object = pair<R, O>;


// Utilities to test if type is a pair
template <typename T>
struct is_pair {
    static constexpr bool value = is_base_of<_::pair_base, remove_reference_t<T> >::value;
};


// Get actual type, relation or object from pair while preserving cv qualifiers.
template <typename P>
using pair_relation_t = transcribe_cv_t<remove_reference_t<P>, typename remove_reference_t<P>::relation>;

template <typename P>
using pair_object_t = transcribe_cv_t<remove_reference_t<P>, typename remove_reference_t<P>::object>;

template <typename P>
using pair_type_t = transcribe_cv_t<remove_reference_t<P>, typename remove_reference_t<P>::type>;


// Get actual type from a regular type or pair
template <typename T, typename U = int>
struct actual_type;

template <typename T>
struct actual_type<T, if_not_t< is_pair<T>::value >> {
    using type = T;
};

template <typename T>
struct actual_type<T, if_t< is_pair<T>::value >> {
    using type = pair_type_t<T>;
};

template <typename T>
using actual_type_t = typename actual_type<T>::type;


// Get type without const, *, &
template<typename T>
struct base_type {
    using type = decay_t< remove_pointer_t< actual_type_t<T> > >;
};

template <typename T>
using base_type_t = typename base_type<T>::type;


// Get type without *, & (retains const which is useful for function args)
template<typename T>
struct base_arg_type {
    using type = remove_pointer_t< remove_reference_t< actual_type_t<T> > >;
};

template <typename T>
using base_arg_type_t = typename base_arg_type<T>::type;


// Test if type is the same as its actual type
template <typename T>
struct is_actual {
    static constexpr bool value = 
        std::is_same<T, actual_type_t<T> >::value && !is_enum<T>::value;
};

} // flecs

namespace flecs 
{

namespace _ 
{

inline void ecs_ctor_illegal(void *, int32_t, const ecs_type_info_t*) {
    ecs_abort(ECS_INVALID_OPERATION, "invalid constructor");
}

inline void ecs_dtor_illegal(void *, int32_t, const ecs_type_info_t*) {
    ecs_abort(ECS_INVALID_OPERATION, "invalid destructor");
}

inline void ecs_copy_illegal(
    void *, const void *, int32_t, const ecs_type_info_t *)
{
    ecs_abort(ECS_INVALID_OPERATION, "invalid copy assignment");
}

inline void ecs_move_illegal(void *, void *, int32_t, const ecs_type_info_t *) {
    ecs_abort(ECS_INVALID_OPERATION, "invalid move assignment");
}

inline void ecs_copy_ctor_illegal(
    void *, const void *, int32_t, const ecs_type_info_t *)
{
    ecs_abort(ECS_INVALID_OPERATION, "invalid copy construct");
}

inline void ecs_move_ctor_illegal(
    void *, void *, int32_t, const ecs_type_info_t *)
{
    ecs_abort(ECS_INVALID_OPERATION, "invalid move construct");
}


// T()
// Can't coexist with T(flecs::entity) or T(flecs::world, flecs::entity)
template <typename T>
void ctor_impl(void *ptr, int32_t count, const ecs_type_info_t *info) {
    (void)info; ecs_assert(info->size == ECS_SIZEOF(T),
        ECS_INTERNAL_ERROR, NULL);
    T *arr = static_cast<T*>(ptr);
    for (int i = 0; i < count; i ++) {
        FLECS_PLACEMENT_NEW(&arr[i], T);
    }
}

// ~T()
template <typename T>
void dtor_impl(void *ptr, int32_t count, const ecs_type_info_t *info) {
    (void)info; ecs_assert(info->size == ECS_SIZEOF(T), 
        ECS_INTERNAL_ERROR, NULL);
    T *arr = static_cast<T*>(ptr);
    for (int i = 0; i < count; i ++) {
        arr[i].~T();
    }
}

// T& operator=(const T&)
template <typename T>
void copy_impl(void *dst_ptr, const void *src_ptr, int32_t count, 
    const ecs_type_info_t *info)
{
    (void)info; ecs_assert(info->size == ECS_SIZEOF(T), 
        ECS_INTERNAL_ERROR, NULL);
    T *dst_arr = static_cast<T*>(dst_ptr);
    const T *src_arr = static_cast<const T*>(src_ptr);
    for (int i = 0; i < count; i ++) {
        dst_arr[i] = src_arr[i];
    }
}

// T& operator=(T&&)
template <typename T>
void move_impl(void *dst_ptr, void *src_ptr, int32_t count, 
    const ecs_type_info_t *info)
{
    (void)info; ecs_assert(info->size == ECS_SIZEOF(T), 
        ECS_INTERNAL_ERROR, NULL);
    T *dst_arr = static_cast<T*>(dst_ptr);
    T *src_arr = static_cast<T*>(src_ptr);
    for (int i = 0; i < count; i ++) {
        dst_arr[i] = FLECS_MOV(src_arr[i]);
    }
}

// T(T&)
template <typename T>
void copy_ctor_impl(void *dst_ptr, const void *src_ptr, int32_t count, 
    const ecs_type_info_t *info)
{
    (void)info; ecs_assert(info->size == ECS_SIZEOF(T), 
        ECS_INTERNAL_ERROR, NULL);
    T *dst_arr = static_cast<T*>(dst_ptr);
    const T *src_arr = static_cast<const T*>(src_ptr);
    for (int i = 0; i < count; i ++) {
        FLECS_PLACEMENT_NEW(&dst_arr[i], T(src_arr[i]));
    }
}

// T(T&&)
template <typename T>
void move_ctor_impl(void *dst_ptr, void *src_ptr, int32_t count, 
    const ecs_type_info_t *info)
{
    (void)info; ecs_assert(info->size == ECS_SIZEOF(T), 
        ECS_INTERNAL_ERROR, NULL);
    T *dst_arr = static_cast<T*>(dst_ptr);
    T *src_arr = static_cast<T*>(src_ptr);
    for (int i = 0; i < count; i ++) {
        FLECS_PLACEMENT_NEW(&dst_arr[i], T(FLECS_MOV(src_arr[i])));
    }
}

// T(T&&), ~T()
// Typically used when moving to a new table, and removing from the old table
template <typename T>
void ctor_move_dtor_impl(void *dst_ptr, void *src_ptr, int32_t count, 
    const ecs_type_info_t *info)
{
    (void)info; ecs_assert(info->size == ECS_SIZEOF(T), 
        ECS_INTERNAL_ERROR, NULL);
    T *dst_arr = static_cast<T*>(dst_ptr);
    T *src_arr = static_cast<T*>(src_ptr);
    for (int i = 0; i < count; i ++) {
        FLECS_PLACEMENT_NEW(&dst_arr[i], T(FLECS_MOV(src_arr[i])));
        src_arr[i].~T();
    }
}

// Move assign + dtor (non-trivial move assigmnment)
// Typically used when moving a component to a deleted component
template <typename T, if_not_t<
    std::is_trivially_move_assignable<T>::value > = 0>
void move_dtor_impl(void *dst_ptr, void *src_ptr, int32_t count, 
    const ecs_type_info_t *info)
{
    (void)info; ecs_assert(info->size == ECS_SIZEOF(T), 
        ECS_INTERNAL_ERROR, NULL);
    T *dst_arr = static_cast<T*>(dst_ptr);
    T *src_arr = static_cast<T*>(src_ptr);
    for (int i = 0; i < count; i ++) {
        // Move assignment should free dst & assign dst to src
        dst_arr[i] = FLECS_MOV(src_arr[i]);
        // Destruct src. Move should have left object in a state where it no
        // longer holds resources, but it still needs to be destructed.
        src_arr[i].~T();
    }
}

// Move assign + dtor (trivial move assigmnment)
// Typically used when moving a component to a deleted component
template <typename T, if_t<
    std::is_trivially_move_assignable<T>::value > = 0>
void move_dtor_impl(void *dst_ptr, void *src_ptr, int32_t count, 
    const ecs_type_info_t *info)
{
    (void)info; ecs_assert(info->size == ECS_SIZEOF(T), 
        ECS_INTERNAL_ERROR, NULL);
    T *dst_arr = static_cast<T*>(dst_ptr);
    T *src_arr = static_cast<T*>(src_ptr);
    for (int i = 0; i < count; i ++) {
        // Cleanup resources of dst
        dst_arr[i].~T();
        // Copy src to dst
        dst_arr[i] = FLECS_MOV(src_arr[i]);
        // No need to destruct src. Since this is a trivial move the code
        // should be agnostic to the address of the component which means we
        // can pretend nothing got destructed.
    }
}

} // _

// Trait to test if type is constructible by flecs
template <typename T>
struct is_flecs_constructible {
    static constexpr bool value = 
        std::is_default_constructible<actual_type_t<T>>::value;
};

namespace _
{

// Trivially constructible
template <typename T, if_t< std::is_trivially_constructible<T>::value > = 0>
ecs_xtor_t ctor() {
    return nullptr;
}

// Not constructible by flecs
template <typename T, if_t< 
    ! std::is_default_constructible<T>::value > = 0>
ecs_xtor_t ctor() {
    return ecs_ctor_illegal;
}

// Default constructible
template <typename T, if_t<
    ! std::is_trivially_constructible<T>::value &&
    std::is_default_constructible<T>::value > = 0>
ecs_xtor_t ctor() {
    return ctor_impl<T>;
}

// No dtor
template <typename T, if_t< std::is_trivially_destructible<T>::value > = 0>
ecs_xtor_t dtor() {
    return nullptr;
}

// Dtor
template <typename T, if_t<
    std::is_destructible<T>::value &&
    ! std::is_trivially_destructible<T>::value > = 0>
ecs_xtor_t dtor() {
    return dtor_impl<T>;
}

// Assert when the type cannot be destructed
template <typename T, if_not_t< std::is_destructible<T>::value > = 0>
ecs_xtor_t dtor() {
    flecs_static_assert(always_false<T>::value, 
        "component type must be destructible");
    return ecs_dtor_illegal;
}

// Trivially copyable
template <typename T, if_t< std::is_trivially_copyable<T>::value > = 0>
ecs_copy_t copy() {
    return nullptr;
}

// Not copyable
template <typename T, if_t<
    ! std::is_trivially_copyable<T>::value &&
    ! std::is_copy_assignable<T>::value > = 0>
ecs_copy_t copy() {
    return ecs_copy_illegal;
}

// Copy assignment
template <typename T, if_t<
    std::is_copy_assignable<T>::value &&
    ! std::is_trivially_copyable<T>::value > = 0>
ecs_copy_t copy() {
    return copy_impl<T>;
}

// Trivially move assignable
template <typename T, if_t< std::is_trivially_move_assignable<T>::value > = 0>
ecs_move_t move() {
    return nullptr;
}

// Component types must be move assignable
template <typename T, if_not_t< std::is_move_assignable<T>::value > = 0>
ecs_move_t move() {
    flecs_static_assert(always_false<T>::value,
        "component type must be move assignable");
    return ecs_move_illegal;
}

// Move assignment
template <typename T, if_t<
    std::is_move_assignable<T>::value &&
    ! std::is_trivially_move_assignable<T>::value > = 0>
ecs_move_t move() {
    return move_impl<T>;
}

// Trivially copy constructible
template <typename T, if_t<
    std::is_trivially_copy_constructible<T>::value > = 0>
ecs_copy_t copy_ctor() {
    return nullptr;
}

// No copy ctor
template <typename T, if_t< ! std::is_copy_constructible<T>::value > = 0>
ecs_copy_t copy_ctor() {
    return ecs_copy_ctor_illegal;
}

// Copy ctor
template <typename T, if_t<
    std::is_copy_constructible<T>::value &&
    ! std::is_trivially_copy_constructible<T>::value > = 0>
ecs_copy_t copy_ctor() {
    return copy_ctor_impl<T>;
}

// Trivially move constructible
template <typename T, if_t<
    std::is_trivially_move_constructible<T>::value > = 0>
ecs_move_t move_ctor() {
    return nullptr;
}

// Component types must be move constructible
template <typename T, if_not_t< std::is_move_constructible<T>::value > = 0>
ecs_move_t move_ctor() {
    flecs_static_assert(always_false<T>::value,
        "component type must be move constructible");    
    return ecs_move_ctor_illegal;
}

// Move ctor
template <typename T, if_t<
    std::is_move_constructible<T>::value &&
    ! std::is_trivially_move_constructible<T>::value > = 0>
ecs_move_t move_ctor() {
    return move_ctor_impl<T>;
}

// Trivial merge (move assign + dtor)
template <typename T, if_t<
    std::is_trivially_move_constructible<T>::value  &&
    std::is_trivially_destructible<T>::value > = 0>
ecs_move_t ctor_move_dtor() {
    return nullptr;
}

// Component types must be move constructible and destructible
template <typename T, if_t<
    ! std::is_move_constructible<T>::value ||
    ! std::is_destructible<T>::value > = 0>
ecs_move_t ctor_move_dtor() {
    flecs_static_assert(always_false<T>::value,
        "component type must be move constructible and destructible");
    return ecs_move_ctor_illegal;
}

// Merge ctor + dtor
template <typename T, if_t<
    !(std::is_trivially_move_constructible<T>::value &&
      std::is_trivially_destructible<T>::value) &&
    std::is_move_constructible<T>::value &&
    std::is_destructible<T>::value > = 0>
ecs_move_t ctor_move_dtor() {
    return ctor_move_dtor_impl<T>;
}

// Trivial merge (move assign + dtor)
template <typename T, if_t<
    std::is_trivially_move_assignable<T>::value  &&
    std::is_trivially_destructible<T>::value > = 0>
ecs_move_t move_dtor() {
    return nullptr;
}

// Component types must be move constructible and destructible
template <typename T, if_t<
    ! std::is_move_assignable<T>::value ||
    ! std::is_destructible<T>::value > = 0>
ecs_move_t move_dtor() {
    flecs_static_assert(always_false<T>::value,
        "component type must be move constructible and destructible");
    return ecs_move_ctor_illegal;
}

// Merge assign + dtor
template <typename T, if_t<
    !(std::is_trivially_move_assignable<T>::value &&
      std::is_trivially_destructible<T>::value) &&
    std::is_move_assignable<T>::value &&
    std::is_destructible<T>::value > = 0>
ecs_move_t move_dtor() {
    return move_dtor_impl<T>;
}

} // _
} // flecs


namespace flecs
{

/** Static helper functions to assign a component value */

// set(T&&), T = constructible
template <typename T, if_t< is_flecs_constructible<T>::value > = 0>
inline void set(world_t *world, entity_t entity, T&& value, ecs_id_t id) {
    ecs_assert(_::cpp_type<T>::size() != 0, ECS_INVALID_PARAMETER, NULL);

    T& dst = *static_cast<T*>(ecs_get_mut_id(world, entity, id, NULL));
    dst = FLECS_MOV(value);

    ecs_modified_id(world, entity, id);
}

// set(const T&), T = constructible
template <typename T, if_t< is_flecs_constructible<T>::value > = 0>
inline void set(world_t *world, entity_t entity, const T& value, ecs_id_t id) {
    ecs_assert(_::cpp_type<T>::size() != 0, ECS_INVALID_PARAMETER, NULL);

    T& dst = *static_cast<T*>(ecs_get_mut_id(world, entity, id, NULL));
    dst = value;

    ecs_modified_id(world, entity, id);
}

// set(T&&), T = not constructible
template <typename T, if_not_t< is_flecs_constructible<T>::value > = 0>
inline void set(world_t *world, entity_t entity, T&& value, ecs_id_t id) {
    ecs_assert(_::cpp_type<T>::size() != 0, ECS_INVALID_PARAMETER, NULL);

    bool is_new = false;
    T& dst = *static_cast<T*>(ecs_get_mut_id(world, entity, id, &is_new));

    /* If type is not constructible get_mut should assert on new values */
    ecs_assert(!is_new, ECS_INTERNAL_ERROR, NULL);

    dst = FLECS_MOV(value);

    ecs_modified_id(world, entity, id);
}

// set(const T&), T = not constructible
template <typename T, if_not_t< is_flecs_constructible<T>::value > = 0>
inline void set(world_t *world, id_t entity, const T& value, id_t id) {
    ecs_assert(_::cpp_type<T>::size() != 0, ECS_INVALID_PARAMETER, NULL);

    bool is_new = false;
    T& dst = *static_cast<T*>(ecs_get_mut_id(world, entity, id, &is_new));

    /* If type is not constructible get_mut should assert on new values */
    ecs_assert(!is_new, ECS_INTERNAL_ERROR, NULL);
    dst = value;

    ecs_modified_id(world, entity, id);
}

// emplace for T(Args...)
template <typename T, typename ... Args, if_t< 
    std::is_constructible<actual_type_t<T>, Args...>::value ||
    std::is_default_constructible<actual_type_t<T>>::value > = 0>
inline void emplace(world_t *world, id_t entity, Args&&... args) {
    id_t id = _::cpp_type<T>::id(world);

    ecs_assert(_::cpp_type<T>::size() != 0, ECS_INVALID_PARAMETER, NULL);
    T& dst = *static_cast<T*>(ecs_emplace_id(world, entity, id));
    
    FLECS_PLACEMENT_NEW(&dst, T{FLECS_FWD(args)...});

    ecs_modified_id(world, entity, id);    
}

// emplace for T(flecs::entity, Args...)
template <typename T, typename ... Args, if_t<
    std::is_constructible<actual_type_t<T>, flecs::entity, Args...>::value > = 0>
inline void emplace(world_t *world, id_t entity, Args&&... args);

// set(T&&)
template <typename T, typename A>
inline void set(world_t *world, entity_t entity, A&& value) {
    id_t id = _::cpp_type<T>::id(world);
    flecs::set(world, entity, FLECS_FWD(value), id);
}

// set(const T&)
template <typename T, typename A>
inline void set(world_t *world, entity_t entity, const A& value) {
    id_t id = _::cpp_type<T>::id(world);
    flecs::set(world, entity, value, id);
}

struct scoped_world;

/** The world.
 * The world is the container of all ECS data and systems. If the world is
 * deleted, all data in the world will be deleted as well.
 */
struct world {
    /** Create world.
     */
    explicit world() 
        : m_world( ecs_init() )
        , m_owned( true ) { init_builtin_components(); }

    /** Create world with command line arguments.
     * Currently command line arguments are not interpreted, but they may be
     * used in the future to configure Flecs parameters.
     */
    explicit world(int argc, char *argv[])
        : m_world( ecs_init_w_args(argc, argv) )
        , m_owned( true ) { init_builtin_components(); }

    /** Create world from C world.
     */
    explicit world(world_t *w)
        : m_world( w ) 
        , m_owned( false ) { }

    /** Not allowed to copy a world. May only take a reference. 
     */
    world(const world& obj) = delete;

    world(world&& obj) {
        m_world = obj.m_world;
        m_owned = obj.m_owned;
        obj.m_world = nullptr;
        obj.m_owned = false;
    }

    /* Implicit conversion to world_t* */
    operator world_t*() const { return m_world; }

    /** Not allowed to copy a world. May only take a reference.
     */
    world& operator=(const world& obj) = delete;

    world& operator=(world&& obj) {
        this->~world();

        m_world = obj.m_world;
        m_owned = obj.m_owned;
        obj.m_world = nullptr;
        obj.m_owned = false;
        return *this;
    }
    
    ~world() { 
        if (m_owned && ecs_stage_is_async(m_world)) {
            ecs_async_stage_free(m_world);
        } else
        if (m_owned && m_world) {
            ecs_fini(m_world); 
        }
    }

    /** Obtain pointer to C world object.
     */
    world_t* c_ptr() const {
        return m_world;
    }

    /** Get last delta_time.
     */
    FLECS_FLOAT delta_time() const {
        const ecs_world_info_t *stats = ecs_get_world_info(m_world);
        return stats->delta_time;
    }

    /** Get current tick.
     */
    int32_t tick() const {
        const ecs_world_info_t *stats = ecs_get_world_info(m_world);
        return stats->frame_count_total;
    }

    /** Get current simulation time.
     */
    FLECS_FLOAT time() const {
        const ecs_world_info_t *stats = ecs_get_world_info(m_world);
        return stats->world_time_total;
    }

    /** Signal application should quit.
     * After calling this operation, the next call to progress() returns false.
     */
    void quit() {
        ecs_quit(m_world);
    }

    /** Register action to be executed when world is destroyed.
     */
    void atfini(ecs_fini_action_t action, void *ctx) const {
        ecs_atfini(m_world, action, ctx);
    }

    /** Test if quit() has been called.
     */
    bool should_quit() {
        return ecs_should_quit(m_world);
    }

    /** Begin frame.
     * When an application does not use progress() to control the main loop, it
     * can still use Flecs features such as FPS limiting and time measurements.
     * This operation needs to be invoked whenever a new frame is about to get 
     * processed.
     *
     * Calls to frame_begin must always be followed by frame_end.
     *
     * The function accepts a delta_time parameter, which will get passed to 
     * systems. This value is also used to compute the amount of time the 
     * function needs to sleep to ensure it does not exceed the target_fps, when 
     * it is set. When 0 is provided for delta_time, the time will be measured.
     *
     * This function should only be ran from the main thread.
     *
     * @param delta_time Time elapsed since the last frame.
     * @return The provided delta_time, or measured time if 0 was provided.
     */
    FLECS_FLOAT frame_begin(float delta_time = 0) {
        return ecs_frame_begin(m_world, delta_time);
    }

    /** End frame. 
     * This operation must be called at the end of the frame, and always after
     * ecs_frame_begin.
     *
     * This function should only be ran from the main thread.
     */
    void frame_end() {
        ecs_frame_end(m_world);
    }

    /** Begin staging.
     * When an application does not use ecs_progress to control the main loop, it
     * can still use Flecs features such as the defer queue. When an application
     * needs to stage changes, it needs to call this function after ecs_frame_begin.
     * A call to ecs_staging_begin must be followed by a call to ecs_staging_end.
     * 
     * When staging is enabled, modifications to entities are stored to a stage.
     * This ensures that arrays are not modified while iterating. Modifications are
     * merged back to the "main stage" when ecs_staging_end is invoked.
     *
     * While the world is in staging mode, no structural changes (add/remove/...)
     * can be made to the world itself. Operations must be executed on a stage
     * instead (see ecs_get_stage).
     *
     * This function should only be ran from the main thread.
     *
     * @return Whether world is currently staged.
     */
    bool staging_begin() {
        return ecs_staging_begin(m_world);
    }

    /** End staging.
     * Leaves staging mode. After this operation the world may be directly mutated
     * again. By default this operation also merges data back into the world, unless
     * automerging was disabled explicitly.
     *
     * This function should only be ran from the main thread.
     */
    void staging_end() {
        ecs_staging_end(m_world);
    }

    /** Defer operations until end of frame. 
     * When this operation is invoked while iterating, operations inbetween the
     * defer_begin and defer_end operations are executed at the end of the frame.
     *
     * This operation is thread safe.
     */
    bool defer_begin() {
        return ecs_defer_begin(m_world);
    }

    /** End block of operations to defer. 
     * See defer_begin.
     *
     * This operation is thread safe.
     */
    bool defer_end() {
        return ecs_defer_end(m_world);
    }

    /** Test whether deferring is enabled.
     */
    bool is_deferred() {
        return ecs_is_deferred(m_world);
    }

    /** Configure world to have N stages.
     * This initializes N stages, which allows applications to defer operations to
     * multiple isolated defer queues. This is typically used for applications with
     * multiple threads, where each thread gets its own queue, and commands are
     * merged when threads are synchronized.
     *
     * Note that set_threads() already creates the appropriate number of stages. 
     * The set_stages() operation is useful for applications that want to manage 
     * their own stages and/or threads.
     * 
     * @param stages The number of stages.
     */
    void set_stages(int32_t stages) const {
        ecs_set_stages(m_world, stages);
    }

    /** Get number of configured stages.
     * Return number of stages set by set_stages.
     *
     * @return The number of stages used for threading.
     */
    int32_t get_stage_count() const {
        return ecs_get_stage_count(m_world);
    }

    /** Get current stage id.
     * The stage id can be used by an application to learn about which stage it
     * is using, which typically corresponds with the worker thread id.
     *
     * @return The stage id.
     */
    int32_t get_stage_id() const {
        return ecs_get_stage_id(m_world);
    }

    /** Enable/disable automerging for world or stage.
     * When automerging is enabled, staged data will automatically be merged 
     * with the world when staging ends. This happens at the end of progress(), 
     * at a sync point or when staging_end() is called.
     *
     * Applications can exercise more control over when data from a stage is 
     * merged by disabling automerging. This requires an application to 
     * explicitly call merge() on the stage.
     *
     * When this function is invoked on the world, it sets all current stages to
     * the provided value and sets the default for new stages. When this 
     * function is invoked on a stage, automerging is only set for that specific 
     * stage. 
     *
     * @param automerge Whether to enable or disable automerging.
     */
    void set_automerge(bool automerge) {
        ecs_set_automerge(m_world, automerge);
    }

    /** Merge world or stage.
     * When automatic merging is disabled, an application can call this
     * operation on either an individual stage, or on the world which will merge
     * all stages. This operation may only be called when staging is not enabled
     * (either after progress() or after staging_end()).
     *
     * This operation may be called on an already merged stage or world.
     */
    void merge() {
        ecs_merge(m_world);
    }

    /** Get stage-specific world pointer.
     * Flecs threads can safely invoke the API as long as they have a private 
     * context to write to, also referred to as the stage. This function returns a
     * pointer to a stage, disguised as a world pointer.
     *
     * Note that this function does not(!) create a new world. It simply wraps the
     * existing world in a thread-specific context, which the API knows how to
     * unwrap. The reason the stage is returned as an ecs_world_t is so that it
     * can be passed transparently to the existing API functions, vs. having to 
     * create a dediated API for threading.
     *
     * @param stage_id The index of the stage to retrieve.
     * @return A thread-specific pointer to the world. 
     */
    flecs::world get_stage(int32_t stage_id) const {
        return flecs::world(ecs_get_stage(m_world, stage_id));
    }

    /** Create asynchronous stage.
     * An asynchronous stage can be used to asynchronously queue operations for
     * later merging with the world. An asynchronous stage is similar to a regular
     * stage, except that it does not allow reading from the world.
     *
     * Asynchronous stages are never merged automatically, and must therefore be
     * manually merged with the ecs_merge function. It is not necessary to call
     * defer_begin or defer_end before and after enqueuing commands, as an 
     * asynchronous stage unconditionally defers operations.
     *
     * The application must ensure that no commands are added to the stage while the
     * stage is being merged.
     *
     * An asynchronous stage must be cleaned up by ecs_async_stage_free. 
     *
     * @return The stage.
     */
    flecs::world async_stage() const {
        auto result = flecs::world(ecs_async_stage_new(m_world));
        result.m_owned = true;
        return result;
    }

    /** Get actual world.
     * If the current object points to a stage, this operation will return the
     * actual world.
     *
     * @return The actual world.
     */
    flecs::world get_world() const {
        /* Safe cast, mutability is checked */
        return flecs::world(
            m_world ? const_cast<flecs::world_t*>(ecs_get_world(m_world)) : nullptr);
    }

    /** Test whether the current world object is readonly.
     * This function allows the code to test whether the currently used world
     * object is readonly or whether it allows for writing.
     *
     * @return True if the world or stage is readonly.
     */
    bool is_readonly() const {
        return ecs_stage_is_readonly(m_world);
    }

    /** Set world context.
     * Set a context value that can be accessed by anyone that has a reference
     * to the world.
     *
     * @param ctx The world context.
     */
    void set_context(void* ctx) const {
        ecs_set_context(m_world, ctx);
    }

    /** Get world context.
     *
     * @return The configured world context.
     */
    void* get_context() const {
        return ecs_get_context(m_world);
    }

    /** Preallocate memory for number of entities.
     * This function preallocates memory for the entity index.
     *
     * @param entity_count Number of entities to preallocate memory for.
     */
    void dim(int32_t entity_count) const {
        ecs_dim(m_world, entity_count);
    }

    /** Set entity range.
     * This function limits the range of issued entity ids between min and max.
     *
     * @param min Minimum entity id issued.
     * @param max Maximum entity id issued.
     */
    void set_entity_range(entity_t min, entity_t max) const {
        ecs_set_entity_range(m_world, min, max);
    }

    /** Enforce that operations cannot modify entities outside of range.
     * This function ensures that only entities within the specified range can
     * be modified. Use this function if specific parts of the code only are
     * allowed to modify a certain set of entities, as could be the case for
     * networked applications.
     *
     * @param enabled True if range check should be enabled, false if not.
     */
    void enable_range_check(bool enabled) const {
        ecs_enable_range_check(m_world, enabled);
    }

    /** Set current scope.
     *
     * @param scope The scope to set.
     * @return The current scope;
     * @see ecs_set_scope
     */
    flecs::entity set_scope(const flecs::entity_t scope) const;

    /** Get current scope.
     *
     * @return The current scope.
     * * @see ecs_get_scope
     */
    flecs::entity get_scope() const;

    /** Same as set_scope but with type.
     * * @see ecs_set_scope
     */
    template <typename T>
    flecs::entity set_scope() const;

    /** Set search path.
     */
    flecs::entity_t* set_lookup_path(const flecs::entity_t *search_path) {
        return ecs_set_lookup_path(m_world, search_path);
    }

    /** Lookup entity by name.
     * 
     * @param name Entity name.
     */
    flecs::entity lookup(const char *name) const;

    /** Set singleton component.
     */
    template <typename T, if_t< !is_callable<T>::value > = 0>
    void set(const T& value) const {
        flecs::set<T>(m_world, _::cpp_type<T>::id(m_world), value);
    }

    template <typename T, if_t< !is_callable<T>::value > = 0>
    void set(T&& value) const {
        flecs::set<T>(m_world, _::cpp_type<T>::id(m_world), 
            FLECS_FWD(value));
    }
    
    /** Set singleton component inside a callback.
     */
    template <typename Func, if_t< is_callable<Func>::value > = 0 >
    void set(const Func& func);

    template <typename T, typename ... Args>
    void emplace(Args&&... args) const {
        flecs::emplace<T>(m_world, _::cpp_type<T>::id(m_world), 
            FLECS_FWD(args)...);
    }        

    /** Get mut singleton component.
     */
    template <typename T>
    T* get_mut() const;

    /** Mark singleton component as modified.
     */
    template <typename T>
    void modified() const;

    /** Get singleton component.
     */
    template <typename T>
    const T* get() const;
    
    /** Get singleton component inside a callback.
     */
    template <typename Func, if_t< is_callable<Func>::value > = 0 >
    void get(const Func& func);

    /** Test if world has singleton component.
     */
    template <typename T>
    bool has() const;

    /** Add singleton component.
     */
    template <typename T>
    void add() const;

    /** Remove singleton component.
     */
    template <typename T>
    void remove() const;

    /** Get singleton entity for type.
     */
    template <typename T>
    flecs::entity singleton();

    /** Create alias for component.
     *
     * @tparam T to create an alias for.
     * @param alias Alias for the component.
     * @return Entity representing the component.
     */
    template <typename T>
    flecs::entity use(const char *alias = nullptr);

    /** Create alias for entity.
     *
     * @param name Name of the entity.
     * @param alias Alias for the entity.
     */
    flecs::entity use(const char *name, const char *alias = nullptr);    

    /** Create alias for entity.
     *
     * @param entity Entity for which to create the alias.
     * @param alias Alias for the entity.
     */
    void use(flecs::entity entity, const char *alias = nullptr);   

    /** Count entities matching a component.
     *
     * @param component_id The component id.
     */
    int count(flecs::id_t component_id) const {
        return ecs_count_id(m_world, component_id);
    }

    /** Count entities matching a pair.
     *
     * @param rel The relation id.
     * @param obj The object id.
     */
    int count(flecs::entity_t rel, flecs::entity_t obj) const {
        return ecs_count_id(m_world, ecs_pair(rel, obj));
    }

    /** Count entities matching a component.
     *
     * @tparam T The component type.
     */
    template <typename T>
    int count() const {
        return count(_::cpp_type<T>::id(m_world));
    }

    /** Count entities matching a pair.
     *
     * @tparam Rel The relation type.
     * @param obj The object id.
     */
    template <typename Rel>
    int count(flecs::entity_t obj) const {
        return count(_::cpp_type<Rel>::id(m_world), obj);
    }

    /** Count entities matching a pair.
     *
     * @tparam Rel The relation type.
     * @tparam Obj The object type.
     */
    template <typename Rel, typename Obj>
    int count() const {
        return count( 
            _::cpp_type<Rel>::id(m_world),
            _::cpp_type<Obj>::id(m_world));
    }

    /** Enable locking.
     * 
     * @param enabled True if locking should be enabled, false if not.
     */
    bool enable_locking(bool enabled) {
        return ecs_enable_locking(m_world, enabled);
    }

    /** Lock world.
     */
    void lock() {
        ecs_lock(m_world);
    }

    /** Unlock world.
     */
    void unlock() {
        ecs_unlock(m_world);
    }

    /** All entities created in function are created with id.
     */
    template <typename Func>
    void with(id_t with_id, const Func& func) const {
        ecs_id_t prev = ecs_set_with(m_world, with_id);
        func();
        ecs_set_with(m_world, prev);    
    }

    /** All entities created in function are created with type.
     */
    template <typename T, typename Func>
    void with(const Func& func) const {
        with(this->id<T>(), func);
    }

    /** All entities created in function are created with relation.
     */
    template <typename Relation, typename Object, typename Func>
    void with(const Func& func) const {
        with(ecs_pair(this->id<Relation>(), this->id<Object>()), func);
    }

    /** All entities created in function are created with relation.
     */
    template <typename Relation, typename Func>
    void with(id_t object, const Func& func) const {
        with(ecs_pair(this->id<Relation>(), object), func);
    } 

    /** All entities created in function are created with relation.
     */
    template <typename Func>
    void with(id_t relation, id_t object, const Func& func) const {
        with(ecs_pair(relation, object), func);
    }

    /** All entities created in function are created in scope. All operations
     * called in function (such as lookup) are relative to scope.
     */
    template <typename Func>
    void scope(id_t parent, const Func& func) const {
        ecs_entity_t prev = ecs_set_scope(m_world, parent);
        func();
        ecs_set_scope(m_world, prev);
    }
    
    /** Same as scope(parent, func), but with T as parent.
     */
    template <typename T, typename Func>
    void scope(const Func& func) const {
        flecs::id_t parent = _::cpp_type<T>::id(m_world);
        scope(parent, func);
    }

    /** Use provided scope for operations ran on returned world.
     * Operations need to be ran in a single statement.
     */
    flecs::scoped_world scope(id_t parent);

    template <typename T>
    flecs::scoped_world scope();

    /** Delete all entities with specified id. */
    void delete_with(id_t the_id) const {
        ecs_delete_with(m_world, the_id);
    }

    /** Delete all entities with specified relation. */
    void delete_with(entity_t rel, entity_t obj) const {
        delete_with(ecs_pair(rel, obj));
    }

    /** Delete all entities with specified component. */
    template <typename T>
    void delete_with() const {
        delete_with(_::cpp_type<T>::id());
    }

    /** Delete all entities with specified relation. */
    template <typename R, typename O>
    void delete_with() const {
        delete_with(_::cpp_type<R>::id(), _::cpp_type<O>::id());
    }

    /** Remove all instances of specified id. */
    void remove_all(id_t the_id) const {
        ecs_remove_all(m_world, the_id);
    }

    /** Remove all instances of specified relation. */
    void remove_all(entity_t rel, entity_t obj) const {
        remove_all(ecs_pair(rel, obj));
    }

    /** Remove all instances of specified component. */
    template <typename T>
    void remove_all() const {
        remove_all(_::cpp_type<T>::id());
    }

    /** Remove all instances of specified relation. */
    template <typename R, typename O>
    void remove_all() const {
        remove_all(_::cpp_type<R>::id(), _::cpp_type<O>::id());
    }

    /** Defer all operations called in function. If the world is already in
     * deferred mode, do nothing.
     */
    template <typename Func>
    void defer(const Func& func) const {
        ecs_defer_begin(m_world);
        func();
        ecs_defer_end(m_world);
    }

    /** Check if entity id exists in the world.
     * Ignores entity relation.
     * 
     * @see ecs_exists
     */
    bool exists(flecs::entity_t e) const {
        return ecs_exists(m_world, e);
    }

    /** Check if entity id exists in the world.
     * Ignores entity relation.
     *
     * @see ecs_is_alive
     */
    bool is_alive(flecs::entity_t e) const {
        return ecs_is_alive(m_world, e);
    }

    /** Check if entity id is valid.
     * Invalid entities cannot be used with API functions.
     * 
     * @see ecs_is_valid
     */
    bool is_valid(flecs::entity_t e) const {
        return ecs_is_valid(m_world, e);
    }

    /** Get alive entity for id.
     * Returns the entity with the current generation.
     * 
     * @see ecs_get_alive
     */
    flecs::entity get_alive(flecs::entity_t e) const;

/* Prevent clashing with Unreal define. Unreal applications will have to use
 *  ecs_ensure. */
#ifndef ensure
    /** Ensures that entity with provided generation is alive.
     * Ths operation will fail if an entity exists with the same id and a 
     * different, non-zero generation.
     * 
     * @see ecs_ensure
     */
    flecs::entity ensure(flecs::entity_t e) const;
#endif

    /* Run callback after completing frame */
    void run_post_frame(ecs_fini_action_t action, void *ctx) {
        ecs_run_post_frame(m_world, action, ctx);
    }


/** Get id from a type.
 */
template <typename T>
flecs::id id() const;

/** Id factory.
 */
template <typename ... Args>
flecs::id id(Args&&... args) const;

/** Get pair id from relation, object
 */
template <typename R, typename O>
flecs::id pair() const;

/** Get pair id from relation, object
 */
template <typename R>
flecs::id pair(entity_t o) const;

/** Get pair id from relation, object
 */
flecs::id pair(entity_t r, entity_t o) const;


/** Register a component.
 */
template <typename T, typename... Args>
flecs::component<T> component(Args &&... args) const;

/** Register a component.
 */
template <typename... Args>
flecs::untyped_component component(Args &&... args) const;


/** Create an entity.
 */
template <typename... Args>
flecs::entity entity(Args &&... args) const;

/** Get id from an enum constant.
 */
template <typename E, if_t< is_enum<E>::value > = 0>
flecs::entity id(E value) const;

/** Get id from an enum constant.
 */
template <typename E, if_t< is_enum<E>::value > = 0>
flecs::entity entity(E value) const;

/** Create a prefab.
 */
template <typename... Args>
flecs::entity prefab(Args &&... args) const;

/** Create an entity that's associated with a type.
 */
template <typename T>
flecs::entity entity(const char *name = nullptr) const;

/** Create a prefab that's associated with a type.
 */
template <typename T>
flecs::entity prefab(const char *name = nullptr) const;


/** Create a new event.
 * 
 * @param evt The event id.
 * @return Event builder.
 */
flecs::event_builder event(flecs::entity_t evt) const;

/** Create a new event.
 * 
 * @tparam E The event type.
 * @return Event builder.
 */
template <typename E>
flecs::event_builder_typed<E> event() const;

/** Create a term.
 * 
 */
template<typename... Args>
flecs::term term(Args &&... args) const;

/** Create a term for a (component) type.
 */
template<typename T, typename... Args>
flecs::term term(Args &&... args) const;  

/** Create a term for a pair.
 */
template<typename R, typename O, typename... Args>
flecs::term term(Args &&... args) const;


/** Create a filter.
 * @see ecs_filter_init
 */
template <typename... Comps, typename... Args>
flecs::filter<Comps...> filter(Args &&... args) const;

/** Create a filter builder.
 * @see ecs_filter_init
 */
template <typename... Comps, typename... Args>
flecs::filter_builder<Comps...> filter_builder(Args &&... args) const;

/** Iterate over all entities with components in argument list of function.
 * The function parameter must match the following signature:
 *   void(*)(T&, U&, ...) or
 *   void(*)(flecs::entity, T&, U&, ...)
 */
template <typename Func>
void each(Func&& func) const;

/** Iterate over all entities with provided component.
 * The function parameter must match the following signature:
 *   void(*)(T&) or
 *   void(*)(flecs::entity, T&)
 */
template <typename T, typename Func>
void each(Func&& func) const;

/** Iterate over all entities with provided (component) id.
 */
template <typename Func>
void each(flecs::id_t term_id, Func&& func) const;


/** Upcast entity to a trigger.
 * The provided entity must be a trigger.
 * 
 * @param e The entity.
 * @return A trigger object.
 */
flecs::trigger trigger(flecs::entity e) const;

/** Create a new trigger.
 * 
 * @tparam Components The components to match on.
 * @tparam Args Arguments passed to the constructor of flecs::trigger_builder.
 * @return Trigger builder.
 */
template <typename... Components, typename... Args>
flecs::trigger_builder<Components...> trigger(Args &&... args) const;


/** Upcast entity to an observer.
 * The provided entity must be an observer.
 * 
 * @param e The entity.
 * @return An observer object.
 */
flecs::observer observer(flecs::entity e) const;

/** Create a new observer.
 * 
 * @tparam Components The components to match on.
 * @tparam Args Arguments passed to the constructor of flecs::observer_builder.
 * @return Observer builder.
 */
template <typename... Components, typename... Args>
flecs::observer_builder<Components...> observer(Args &&... args) const;


/** Create a query.
 * @see ecs_query_init
 */
template <typename... Comps, typename... Args>
flecs::query<Comps...> query(Args &&... args) const;

/** Create a subquery.
 * @see ecs_query_init
 */
template <typename... Comps, typename... Args>
flecs::query<Comps...> query(flecs::query_base& parent, Args &&... args) const;

/** Create a query builder.
 * @see ecs_query_init
 */
template <typename... Comps, typename... Args>
flecs::query_builder<Comps...> query_builder(Args &&... args) const;


/** Create a type.
 */
template <typename... Args>
flecs::type type(Args &&... args) const;  

/** Create a type associated with a component.
 */
template <typename T>
flecs::type type(const char *name = nullptr) const;


#   ifdef FLECS_MODULE

/** Create a module.
 * 
 * @tparam Module module class.
 * @return Module entity.
 */
template <typename Module>
flecs::entity module(const char *name = nullptr) const;

/** Import a module.
 * 
 * @tparam Module module class.
 * @return Module entity.
 */
template <typename Module>
flecs::entity import();

#   endif
#   ifdef FLECS_PIPELINE

/** Create a new pipeline.
 *
 * @tparam Args Arguments to pass into the constructor of flecs::system.
 * @return The pipeline.
 */
template <typename... Args>
flecs::pipeline pipeline(Args &&... args) const;

/** Create a new pipeline.
 *
 * @tparam Pipeline Type associated with pipeline.
 * @return The pipeline.
 */
template <typename Pipeline, if_not_t< is_enum<Pipeline>::value > = 0>
flecs::pipeline pipeline() const;

/** Set pipeline.
 * @see ecs_set_pipeline
 */
void set_pipeline(const flecs::pipeline& pip) const;

/** Set pipeline.
 * @see ecs_set_pipeline
 */
template <typename Pipeline>
void set_pipeline() const;

/** Get pipeline.
 * @see ecs_get_pipeline
 */
flecs::pipeline get_pipeline() const;

/** Progress world one tick.
 * @see ecs_progress
 */
bool progress(FLECS_FLOAT delta_time = 0.0) const;

/** Run pipeline.
 * @see ecs_run_pipeline
 */
void run_pipeline(const flecs::pipeline& pip, FLECS_FLOAT delta_time = 0.0) const;

/** Set timescale
 * @see ecs_set_time_scale
 */
void set_time_scale(FLECS_FLOAT mul) const;

/** Get timescale
 * @see ecs_get_time_scale
 */
FLECS_FLOAT get_time_scale() const;

/** Get tick
 * @return Monotonically increasing frame count.
 */
int32_t get_tick() const;

/** Set target FPS
 * @see ecs_set_target_fps
 */
void set_target_fps(FLECS_FLOAT target_fps) const;

/** Get target FPS
 * @return Configured frames per second.
 */
FLECS_FLOAT get_target_fps() const;

/** Reset simulation clock.
 * @see ecs_reset_clock
 */
void reset_clock() const;

/** Deactivate systems.
 * @see ecs_deactivate_systems.
 */
void deactivate_systems() const;

/** Set number of threads.
 * @see ecs_set_threads
 */
void set_threads(int32_t threads) const;

/** Set number of threads.
 * @see ecs_get_threads
 */
int32_t get_threads() const;

#   endif
#   ifdef FLECS_SNAPSHOT

/** Create a snapshot.
 */
template <typename... Args>
flecs::snapshot snapshot(Args &&... args) const;

#   endif
#   ifdef FLECS_SYSTEM

/** Upcast entity to a system.
 * The provided entity must be a system.
 * 
 * @param e The entity.
 * @return A system object.
 */
flecs::system system(flecs::entity e) const;

/** Create a new system.
 * 
 * @tparam Components The components to match on.
 * @tparam Args Arguments passed to the constructor of flecs::system_builder.
 * @return System builder.
 */
template <typename... Components, typename... Args>
flecs::system_builder<Components...> system(Args &&... args) const;

#   endif
#   ifdef FLECS_RULES

/** Create a rule.
 * @see ecs_rule_init
 */
template <typename... Comps, typename... Args>
flecs::rule<Comps...> rule(Args &&... args) const;

/** Create a subrule.
 * @see ecs_rule_init
 */
template <typename... Comps, typename... Args>
flecs::rule<Comps...> rule(flecs::rule_base& parent, Args &&... args) const;

/** Create a rule builder.
 * @see ecs_rule_init
 */
template <typename... Comps, typename... Args>
flecs::rule_builder<Comps...> rule_builder(Args &&... args) const;

#   endif
#   ifdef FLECS_PLECS

/** Load plecs string.
 * @see ecs_plecs_from_str
 */
int plecs_from_str(const char *name, const char *str) const {
    return ecs_plecs_from_str(m_world, name, str);
}

/** Load plecs from file.
 * @see ecs_plecs_from_file
 */
int plecs_from_file(const char *filename) const {
    return ecs_plecs_from_file(m_world, filename);
}

#   endif
#   ifdef FLECS_META

flecs::string to_expr(flecs::entity_t tid, const void* value) {
    char *expr = ecs_ptr_to_expr(m_world, tid, value);
    return flecs::string(expr);
}

template <typename T>
flecs::string to_expr(const T* value) {
    flecs::entity_t tid = _::cpp_type<T>::id(m_world);
    return to_expr(tid, value);
}

flecs::meta::cursor cursor(flecs::entity_t tid, void *ptr) {
    return flecs::meta::cursor(m_world, tid, ptr);
}

template <typename T>
flecs::meta::cursor cursor(void *ptr) {
    flecs::entity_t tid = _::cpp_type<T>::id(m_world);
    return cursor(tid, ptr);
}

#   endif
#   ifdef FLECS_JSON

flecs::string to_json(flecs::entity_t tid, const void* value) {
    char *json = ecs_ptr_to_json(m_world, tid, value);
    return flecs::string(json);
}

template <typename T>
flecs::string to_json(const T* value) {
    flecs::entity_t tid = _::cpp_type<T>::id(m_world);
    return to_json(tid, value);
}

#   endif
#   ifdef FLECS_APP

flecs::app_builder app() const {
    return flecs::app_builder(m_world);
}

#   endif

public:
    void init_builtin_components();

    world_t *m_world;
    bool m_owned;
};

struct scoped_world : world {
    scoped_world(
        flecs::world_t *w, 
        flecs::entity_t s) : world(nullptr)
    {
        m_prev_scope = ecs_set_scope(w, s);
        m_world = w;
        m_owned = false;
    }

    ~scoped_world() {
        ecs_set_scope(m_world, m_prev_scope);
    }

    scoped_world(const scoped_world& obj) : world(nullptr) {
        m_prev_scope = obj.m_prev_scope;
        m_world = obj.m_world;
        m_owned = obj.m_owned;
    }

    flecs::entity_t m_prev_scope;
};

/** Return id without generation.
 * 
 * @see ecs_strip_generation
 */
inline flecs::id_t strip_generation(flecs::entity_t e) {
    return ecs_strip_generation(e);
}

/** Return entity generation.
 */
inline uint32_t get_generation(flecs::entity_t e) {
    return ECS_GENERATION(e);
}

} // namespace flecs


namespace flecs 
{

/** Unsafe wrapper class around a column.
 * This class can be used when a system does not know the type of a column at
 * compile time.
 */
struct unsafe_column {
    unsafe_column(void* array, size_t size, size_t count, bool is_shared = false)
        : m_array(array)
        , m_size(size)
        , m_count(count) 
        , m_is_shared(is_shared) {}

    /** Return element in component array.
     * This operator may only be used if the column is not shared.
     * 
     * @param index Index of element.
     * @return Reference to element.
     */
    void* operator[](size_t index) {
        ecs_assert(index < m_count, ECS_COLUMN_INDEX_OUT_OF_RANGE, NULL);
        ecs_assert(!m_is_shared, ECS_INVALID_PARAMETER, NULL);
        return ECS_OFFSET(m_array, m_size * index);
    }

protected:
    void* m_array;
    size_t m_size;
    size_t m_count;
    bool m_is_shared;        
};

/** Wrapper class around a column.
 * 
 * @tparam T component type of the column.
 */
template <typename T>
struct column {
    static_assert(std::is_empty<T>::value == false, 
        "invalid type for column, cannot iterate empty type");

    /** Create column from component array.
     *
     * @param array Pointer to the component array.
     * @param count Number of elements in component array.
     * @param is_shared Is the component shared or not.
     */
    column(T* array, size_t count, bool is_shared = false)
        : m_array(array)
        , m_count(count) 
        , m_is_shared(is_shared) {}

    /** Create column from iterator.
     *
     * @param iter Iterator object.
     * @param column Index of the signature of the query being iterated over.
     */
    column(iter &iter, int column);

    /** Return element in component array.
     * This operator may only be used if the column is not shared.
     * 
     * @param index Index of element.
     * @return Reference to element.
     */
    T& operator[](size_t index) {
        ecs_assert(index < m_count, ECS_COLUMN_INDEX_OUT_OF_RANGE, NULL);
        ecs_assert(!index || !m_is_shared, ECS_INVALID_PARAMETER, NULL);
        ecs_assert(m_array != nullptr, ECS_COLUMN_INDEX_OUT_OF_RANGE, NULL);
        return m_array[index];
    }
  
    /** Return first element of component array.
     * This operator is typically used when the column is shared.
     *
     * @return Reference to the first element.
     */
    T& operator*() {
      ecs_assert(m_array != nullptr, ECS_COLUMN_INDEX_OUT_OF_RANGE, NULL);
      return *m_array;
    }

    /** Return first element of component array.
     * This operator is typically used when the column is shared.
     * 
     * @return Pointer to the first element.
     */
    T* operator->() {
        ecs_assert(m_array != nullptr, ECS_COLUMN_INDEX_OUT_OF_RANGE, NULL);
        return m_array;
    }

protected:
    T* m_array;
    size_t m_count;
    bool m_is_shared;
};


////////////////////////////////////////////////////////////////////////////////

namespace _ {

////////////////////////////////////////////////////////////////////////////////

/** Iterate over an integer range (used to iterate over entity range).
 *
 * @tparam T of the iterator
 */
template <typename T>
struct range_iterator
{
    explicit range_iterator(T value)
        : m_value(value){}

    bool operator!=(range_iterator const& other) const
    {
        return m_value != other.m_value;
    }

    T const& operator*() const
    {
        return m_value;
    }

    range_iterator& operator++()
    {
        ++m_value;
        return *this;
    }

private:
    T m_value;
};

} // namespace _

} // namespace flecs

namespace flecs
{

////////////////////////////////////////////////////////////////////////////////

/** Class that enables iterating over table columns.
 */
struct iter {
private:
    using row_iterator = _::range_iterator<size_t>;
    
public:
    /** Construct iterator from C iterator object.
     * This operation is typically not invoked directly by the user.
     *
     * @param it Pointer to C iterator.
     */
    iter(ecs_iter_t *it) : m_iter(it) { 
        m_begin = 0;
        m_end = static_cast<std::size_t>(it->count);
    }

    row_iterator begin() const {
        return row_iterator(m_begin);
    }

    row_iterator end() const {
        return row_iterator(m_end);
    }

    flecs::entity system() const;

    flecs::entity event() const;

    flecs::id event_id() const;

    flecs::world world() const;

    const flecs::iter_t* c_ptr() const {
        return m_iter;
    }

    size_t count() const {
        return static_cast<size_t>(m_iter->count);
    }

    FLECS_FLOAT delta_time() const {
        return m_iter->delta_time;
    }

    FLECS_FLOAT delta_system_time() const {
        return m_iter->delta_system_time;
    }

    flecs::type type() const;

    /** Is current type a module or does it contain module contents? */
    bool has_module() const {
        return ecs_table_has_module(m_iter->table);
    }

    /** Access self.
     * 'self' is an entity that can be associated with a trigger, observer or
     * system when they are created. */
    flecs::entity self() const;

    /** Access ctx. 
     * ctx contains the context pointer assigned to a system.
     */
    void* ctx() {
        return m_iter->ctx;
    }

    /** Access param. 
     * param contains the pointer passed to the param argument of system::run
     */
    void* param() {
        return m_iter->param;
    }

    /** Access param. 
     * param contains the pointer passed to the param argument of system::run
     */
    template <typename T>
    T* param() {
        /* TODO: type check */
        return static_cast<T*>(m_iter->param);
    }

    /** Obtain mutable handle to entity being iterated over.
     *
     * @param row Row being iterated over.
     */
    flecs::entity entity(size_t row) const;

    /** Returns whether term is owned.
     * 
     * @param index The term index.
     */
    bool is_owned(int32_t index) const {
        return ecs_term_is_owned(m_iter, index);
    }

    /** Returns whether term is set.
     * 
     * @param index The term index.
     */
    bool is_set(int32_t index) const {
        return ecs_term_is_set(m_iter, index);
    }

    /** Returns whether term is readonly.
     *
     * @param index The term index.
     */
    bool is_readonly(int32_t index) const {
        return ecs_term_is_readonly(m_iter, index);
    }

    /** Number of terms in iteator.
     */
    int32_t term_count() const {
        return m_iter->term_count;
    }

    /** Size of term data type.
     *
     * @param index The term id.
     */
    size_t size(int32_t index) const {
        return ecs_term_size(m_iter, index);
    }

    /** Obtain term source (0 if self)
     *
     * @param index The term index.
     */    
    flecs::entity source(int32_t index) const;

    /** Obtain component id of term.
     *
     * @param index The term index.
     */
    flecs::entity id(int32_t index) const;

    /** Obtain pair id of term.
     * This operation will fail if the term is not a pair.
     * 
     * @param index The term index.
     */
    flecs::id pair(int32_t index) const;

    /** Convert current iterator result to string.
     */
    flecs::string str() const {
        char *s = ecs_iter_str(m_iter);
        return flecs::string(s);
    }

    /** Obtain term with const type.
     * If the specified term index does not match with the provided type, the
     * function will assert.
     *
     * @tparam T Type of the term.
     * @param index The term index.
     * @return The term data.
     */
    template <typename T, typename A = actual_type_t<T>,
        typename std::enable_if<std::is_const<T>::value, void>::type* = nullptr>
        
    flecs::column<A> term(int32_t index) const {
        return get_term<A>(index);
    }

    /** Obtain term with non-const type.
     * If the specified term id does not match with the provided type or if
     * the term is readonly, the function will assert.
     *
     * @tparam T Type of the term.
     * @param index The term index.
     * @return The term data.
     */
    template <typename T, typename A = actual_type_t<T>,
        typename std::enable_if<
            std::is_const<T>::value == false, void>::type* = nullptr>

    flecs::column<A> term(int32_t index) const {
        ecs_assert(!ecs_term_is_readonly(m_iter, index), 
            ECS_COLUMN_ACCESS_VIOLATION, NULL);
        return get_term<A>(index);
    }

    /** Obtain unsafe term.
     * Unsafe terms are required when a system does not know at compile time
     * which component will be passed to it. 
     *
     * @param index The term index. 
     */
    flecs::unsafe_column term(int32_t index) const {
        return get_unsafe_term(index);
    }

    /** Obtain owned term.
     * Same as iter::term, but ensures that term is owned.
     *
     * @tparam T of the term.
     * @param index The term index.
     * @return The term data.
     */
    template <typename T, typename A = actual_type_t<T>>
    flecs::column<A> term_owned(int32_t index) const {
        ecs_assert(!!ecs_term_is_owned(m_iter, index), ECS_COLUMN_IS_SHARED, NULL);
        return this->term<A>(index);
    }

    /** Obtain shared term.
     * Same as iter::term, but ensures that term is shared.
     *
     * @tparam T of the term.
     * @param index The term index.
     * @return The component term.
     */
    template <typename T, typename A = actual_type_t<T>>
    const T& term_shared(int32_t index) const {
        ecs_assert(
            ecs_term_id(m_iter, index) == 
                _::cpp_type<T>::id(m_iter->world), 
                    ECS_COLUMN_TYPE_MISMATCH, NULL);

        ecs_assert(!ecs_term_is_owned(m_iter, index), 
            ECS_COLUMN_IS_NOT_SHARED, NULL);

        return *static_cast<A*>(ecs_term_w_size(m_iter, sizeof(A), index));
    }

    /** Obtain the total number of tables the iterator will iterate over.
     */
    int32_t table_count() const {
        return m_iter->table_count;
    }

    /** Obtain untyped pointer to table column.
     *
     * @param column Id of table column (corresponds with location in table type).
     * @return Pointer to table column.
     */
    void* table_column(int32_t column) const {
        return ecs_iter_column_w_size(m_iter, 0, column);
    }

    /** Obtain typed pointer to table column.
     * If the table does not contain a column with the specified type, the
     * function will assert.
     *
     * @tparam T Type of the table column.
     */
    template <typename T, typename A = actual_type_t<T>>
    flecs::column<T> table_column() const {
        auto col = ecs_iter_find_column(m_iter, _::cpp_type<T>::id());
        ecs_assert(col != -1, ECS_INVALID_PARAMETER, NULL);

        return flecs::column<A>(static_cast<A*>(ecs_iter_column_w_size(m_iter,
            sizeof(A), col)), static_cast<std::size_t>(m_iter->count), false);
    }

    template <typename T>
    flecs::column<T> table_column(flecs::id_t obj) const {
        auto col = ecs_iter_find_column(m_iter, 
            ecs_pair(_::cpp_type<T>::id(), obj));
        ecs_assert(col != -1, ECS_INVALID_PARAMETER, NULL);

        return flecs::column<T>(static_cast<T*>(ecs_iter_column_w_size(m_iter,
            sizeof(T), col)), static_cast<std::size_t>(m_iter->count), false);
    }

    /** Check if the current table has changed since the last iteration.
     * Can only be used when iterating queries and/or systems. */
    bool changed() {
        return ecs_query_changed(nullptr, m_iter);
    }

    /** Skip current table.
     * This indicates to the query that the data in the current table is not
     * modified. By default, iterating a table with a query will mark the 
     * iterated components as dirty if they are annotated with InOut or Out.
     * 
     * When this operation is invoked, the components of the current table will
     * not be marked dirty. */
    void skip() {
        ecs_query_skip(m_iter);
    }

#ifdef FLECS_RULES
    /** Get value of variable by id.
     * Get value of a query variable for current result.
     */
    flecs::entity get_var(int var_id) const;

    /** Get value of variable by name.
     * Get value of a query variable for current result.
     */
    flecs::entity get_var(const char *name) const;
#endif

private:
    /* Get term, check if correct type is used */
    template <typename T, typename A = actual_type_t<T>>
    flecs::column<T> get_term(int32_t index) const {

#ifndef FLECS_NDEBUG
        ecs_entity_t term_id = ecs_term_id(m_iter, index);
        ecs_assert(term_id & ECS_PAIR || term_id & ECS_SWITCH || 
            term_id & ECS_CASE ||
            term_id == _::cpp_type<T>::id(m_iter->world), 
            ECS_COLUMN_TYPE_MISMATCH, NULL);
#endif

        size_t count;
        bool is_shared = !ecs_term_is_owned(m_iter, index);

        /* If a shared column is retrieved with 'column', there will only be a
         * single value. Ensure that the application does not accidentally read
         * out of bounds. */
        if (is_shared) {
            count = 1;
        } else {
            /* If column is owned, there will be as many values as there are
             * entities. */
            count = static_cast<size_t>(m_iter->count);
        }
        
        return flecs::column<A>(
            static_cast<T*>(ecs_term_w_size(m_iter, sizeof(A), index)), 
            count, is_shared);
    }

    flecs::unsafe_column get_unsafe_term(int32_t index) const {
        size_t count;
        size_t size = ecs_term_size(m_iter, index);
        bool is_shared = !ecs_term_is_owned(m_iter, index);

        /* If a shared column is retrieved with 'column', there will only be a
         * single value. Ensure that the application does not accidentally read
         * out of bounds. */
        if (is_shared) {
            count = 1;
        } else {
            /* If column is owned, there will be as many values as there are
             * entities. */
            count = static_cast<size_t>(m_iter->count);
        }

        return flecs::unsafe_column(
            ecs_term_w_size(m_iter, 0, index), size, count, is_shared);
    }     

    flecs::iter_t *m_iter;
    std::size_t m_begin;
    std::size_t m_end;
};

} // namespace flecs

namespace flecs
{

template <typename T>
struct ref {
    ref()
        : m_world( nullptr )
        , m_ref() { }

    ref(world_t *world, entity_t entity) 
        : m_world( world )
        , m_ref() 
    {
        auto comp_id = _::cpp_type<T>::id(world);

        ecs_assert(_::cpp_type<T>::size() != 0, 
                ECS_INVALID_PARAMETER, NULL);

        m_ref = ecs_ref_init_id(m_world, entity, comp_id);
    }

    T* operator->() {
        T* result = static_cast<T*>(ecs_ref_get_id(
            m_world, &m_ref, _::cpp_type<T>::id(m_world)));

        ecs_assert(result != NULL, ECS_INVALID_PARAMETER, NULL);

        return result;
    }

    T* get() {
        return static_cast<T*>(ecs_ref_get_id(
            m_world, &m_ref, _::cpp_type<T>::id(m_world)));
    }

    flecs::entity entity() const;

private:
    world_t *m_world;
    flecs::ref_t m_ref;
};

}

#pragma once

#pragma once

namespace flecs
{

/** Entity view class
 * This class provides readonly access to entities. Using this class to store 
 * entities in components ensures valid handles, as this class will always store
 * the actual world vs. a stage. The constructors of this class will never 
 * create a new entity.
 *
 * To obtain a mutable handle to the entity, use the "mut" function.
 */
struct entity_view : public id {

    entity_view() : flecs::id() { }

    /** Wrap an existing entity id.
     *
     * @param world The world in which the entity is created.
     * @param id The entity id.
     */
    explicit entity_view(flecs::world_t *world, flecs::id_t id)
        : flecs::id(world 
            ? const_cast<flecs::world_t*>(ecs_get_world(world))
            : nullptr
        , id ) { }

    /** Implicit conversion from flecs::entity_t to flecs::entity_view. */
    entity_view(entity_t id) 
        : flecs::id( nullptr, id ) { }

    /** Get entity id.
     * @return The integer entity id.
     */
    entity_t id() const {
        return m_id;
    }

    /** Check is entity is valid.
     *
     * @return True if the entity is alive, false otherwise.
     */
    bool is_valid() const {
        return m_world && ecs_is_valid(m_world, m_id);
    }
  
    explicit operator bool() const {
        return is_valid();
    }

    /** Check is entity is alive.
     *
     * @return True if the entity is alive, false otherwise.
     */
    bool is_alive() const {
        return m_world && ecs_is_alive(m_world, m_id);
    }

    /** Return the entity name.
     *
     * @return The entity name.
     */
    flecs::string_view name() const {
        return flecs::string_view(ecs_get_name(m_world, m_id));
    }

    /** Return the entity symbol.
     *
     * @return The entity symbol.
     */
    flecs::string_view symbol() const {
        return flecs::string_view(ecs_get_symbol(m_world, m_id));
    }

    /** Return the entity path.
     *
     * @return The hierarchical entity path.
     */
    flecs::string path(const char *sep = "::", const char *init_sep = "::") const {
        char *path = ecs_get_path_w_sep(m_world, 0, m_id, sep, init_sep);
        return flecs::string(path);
    }   

    bool enabled() {
        return !ecs_has_id(m_world, m_id, flecs::Disabled);
    }

    /** Return the type.
     *
     * @return Returns the entity type.
     */
    flecs::type type() const;

    /** Return the table.
     *
     * @return Returns the entity type.
     */
    flecs::table_t* table() const {
        return ecs_get_table(m_world, m_id);
    }

    /** Iterate (component) ids of an entity.
     * The function parameter must match the following signature:
     *   void(*)(flecs::id id)
     *
     * @param func The function invoked for each id.
     */
    template <typename Func>
    void each(const Func& func) const;

    /** Iterate matching relation ids of an entity.
     * The function parameter must match the following signature:
     *   void(*)(flecs::id id)
     *
     * @param func The function invoked for each id.
     */
    template <typename Func>
    void each(flecs::id_t rel, flecs::id_t obj, const Func& func) const;

    /** Iterate objects for a given relationship.
     * The function parameter must match the following signature:
     *   void(*)(flecs::entity object)
     *
     * @param rel The relationship for which to iterate the objects.
     * @param func The function invoked for each object.
     */
    template <typename Func>
    void each(const flecs::entity_view& rel, const Func& func) const;

    /** Iterate objects for a given relationship.
     * The function parameter must match the following signature:
     *   void(*)(flecs::entity object)
     *
     * @tparam Rel The relationship for which to iterate the objects.
     * @param func The function invoked for each object.     
     */
    template <typename Rel, typename Func>
    void each(const Func& func) const { 
        return each(_::cpp_type<Rel>::id(m_world), func);
    }

    /** Iterate children for entity.
     * The function parameter must match the following signature:
     *   void(*)(flecs::entity object)
     *
     * @param func The function invoked for each child.     
     */
    template <typename Func>
    void children(Func&& func) const {
        flecs::world world(m_world);

        ecs_filter_t f;
        ecs_filter_desc_t desc = {};
        desc.terms[0].id = ecs_pair(flecs::ChildOf, m_id);
        desc.terms[1].id = flecs::Prefab;
        desc.terms[1].oper = EcsOptional;
        ecs_filter_init(m_world, &f, &desc);

        ecs_iter_t it = ecs_filter_iter(m_world, &f);
        while (ecs_filter_next(&it)) {
            _::each_invoker<Func>(FLECS_MOV(func)).invoke(&it);
        }
    }

    /** Get component value.
     * 
     * @tparam T The component to get.
     * @return Pointer to the component value, nullptr if the entity does not
     *         have the component.
     */
    template <typename T, if_t< is_actual<T>::value > = 0>
    const T* get() const {
        auto comp_id = _::cpp_type<T>::id(m_world);
        ecs_assert(_::cpp_type<T>::size() != 0, ECS_INVALID_PARAMETER, NULL);
        return static_cast<const T*>(ecs_get_id(m_world, m_id, comp_id));
    }

    /** Get component value.
     * Overload for when T is not the same as the actual type, which happens
     * when using pair types.
     * 
     * @tparam T The component to get.
     * @return Pointer to the component value, nullptr if the entity does not
     *         have the component.
     */
    template <typename T, typename A = actual_type_t<T>, 
        if_t< flecs::is_pair<T>::value > = 0>
    const A* get() const {
        auto comp_id = _::cpp_type<T>::id(m_world);
        ecs_assert(_::cpp_type<A>::size() != 0, ECS_INVALID_PARAMETER, NULL);
        return static_cast<const A*>(ecs_get_id(m_world, m_id, comp_id));
    }

    /** Get a pair.
     * This operation gets the value for a pair from the entity.
     *
     * @tparam R the relation type.
     * @tparam O the object type.
     */
    template <typename R, typename O, typename P = pair<R, O>, 
        typename A = actual_type_t<P>, if_not_t< flecs::is_pair<R>::value > = 0>
    const A* get() const {
        return this->get<P>();
    }

    /** Get a pair.
     * This operation gets the value for a pair from the entity. 
     *
     * @tparam R the relation type.
     * @param object the object.
     */
    template<typename R, typename O, if_not_t< is_enum<O>::value> = 0>
    const R* get(O object) const {
        auto comp_id = _::cpp_type<R>::id(m_world);
        ecs_assert(_::cpp_type<R>::size() != 0, ECS_INVALID_PARAMETER, NULL);
        return static_cast<const R*>(
            ecs_get_id(m_world, m_id, ecs_pair(comp_id, object)));
    }

    /** Get a pair.
     * This operation gets the value for a pair from the entity. 
     *
     * @tparam R the relation type.
     * @param constant the enum constant.
     */
    template<typename R, typename O, if_t< is_enum<O>::value> = 0>
    const R* get(O constant) const {
        const auto& et = enum_type<O>(this->m_world);
        flecs::entity_t object = et.entity(constant);
        return get<R>(object);
    }

    /** Get component value (untyped).
     * 
     * @param comp The component to get.
     * @return Pointer to the component value, nullptr if the entity does not
     *         have the component.
     */
    const void* get(flecs::id_t comp) const {
        return ecs_get_id(m_world, m_id, comp);
    }

    /** Get a pair (untyped).
     * This operation gets the value for a pair from the entity. If neither the
     * relation nor the object part of the pair are components, the operation 
     * will fail.
     *
     * @param relation the relation.
     * @param object the object.
     */
    const void* get(flecs::entity_t relation, flecs::entity_t object) const {
        return ecs_get_id(m_world, m_id, ecs_pair(relation, object));
    }

    /** Get 1..N components.
     * This operation accepts a callback with as arguments the components to
     * retrieve. The callback will only be invoked when the entity has all
     * the components.
     *
     * This operation is faster than individually calling get for each component
     * as it only obtains entity metadata once.
     *
     * @param func The callback to invoke.
     * @return True if the entity has all components, false if not.
     */
    template <typename Func, if_t< is_callable<Func>::value > = 0>
    bool get(const Func& func) const;

    /** Get enum constant.
     * 
     * @tparam T The enum type for which to get the constant
     * @return Constant entity if found, 0 entity if not.
     */
    template <typename T, if_t< is_enum<T>::value > = 0>
    const T* get() const;

    /** Get the object part from a pair.
     * This operation gets the value for a pair from the entity. The relation
     * part of the pair should not be a component.
     *
     * @tparam O the object type.
     * @param relation the relation.
     */
    template<typename O>
    const O* get_w_object(flecs::entity_t relation) const {
        auto comp_id = _::cpp_type<O>::id(m_world);
        ecs_assert(_::cpp_type<O>::size() != 0, ECS_INVALID_PARAMETER, NULL);
        return static_cast<const O*>(
            ecs_get_id(m_world, m_id, ecs_pair(relation, comp_id)));
    }

    /** Get the object part from a pair.
     * This operation gets the value for a pair from the entity. The relation
     * part of the pair should not be a component.
     *
     * @tparam R the relation type.
     * @tparam O the object type.
     */
    template<typename R, typename O>
    const O* get_w_object() const {
        return get<pair_object<R, O>>();
    }

    /** Get object for a given relation.
     * This operation returns the object for a given relation. The optional
     * index can be used to iterate through objects, in case the entity has
     * multiple instances for the same relation.
     *
     * @tparam R The relation for which to retrieve the object.
     * @param index The index (0 for the first instance of the relation).
     */
    template<typename R>
    flecs::entity get_object(int32_t index = 0) const;

    /** Get object for a given relation.
     * This operation returns the object for a given relation. The optional
     * index can be used to iterate through objects, in case the entity has
     * multiple instances for the same relation.
     *
     * @param relation The relation for which to retrieve the object.
     * @param index The index (0 for the first instance of the relation).
     */
    flecs::entity get_object(flecs::entity_t relation, int32_t index = 0) const;

    /** Get the object of a relation for a given id.
     * This operation returns the first entity that has the provided id by following
     * the specified relationship. If the entity itself has the id then entity will
     * be returned. If the id cannot be found on the entity or by following the
     * relation, the operation will return 0.
     * 
     * This operation can be used to lookup, for example, which prefab is providing
     * a component by specifying the IsA relation:
     * 
     *   // Is Position provided by the entity or one of its base entities?
     *   ecs_get_object_for_id(world, entity, EcsIsA, ecs_id(Position))
     * 
     * @param relation The relationship to follow.
     * @param id The id to lookup.
     * @return The entity for which the object has been found.
     */
    flecs::entity get_object_for(flecs::entity_t relation, flecs::id_t id) const;

    template <typename T>
    flecs::entity get_object_for(flecs::entity_t relation) const;

    template <typename R, typename O>
    flecs::entity get_object_for(flecs::entity_t relation) const;
    
    /** Lookup an entity by name.
     * Lookup an entity in the scope of this entity. The provided path may
     * contain double colons as scope separators, for example: "Foo::Bar".
     *
     * @param path The name of the entity to lookup.
     * @return The found entity, or entity::null if no entity matched.
     */
    flecs::entity lookup(const char *path) const;

    /** Check if entity has the provided entity.
     *
     * @param e The entity to check.
     * @return True if the entity has the provided entity, false otherwise.
     */
    bool has(flecs::id_t e) const {
        return ecs_has_id(m_world, m_id, e);
    }     

    /** Check if entity has the provided component.
     *
     * @tparam T The component to check.
     * @return True if the entity has the provided component, false otherwise.
     */
    template <typename T>
    bool has() const {
        flecs::id_t cid = _::cpp_type<T>::id(m_world);
        bool result = ecs_has_id(m_world, m_id, cid);
        if (result) {
            return result;
        }

        if (is_enum<T>::value) {
            return ecs_has_pair(m_world, m_id, cid, flecs::Wildcard);
        }

        return false;
    }

    /** Check if entity has the provided enum constant.
     *
     * @tparam E The enum type (can be deduced).
     * @param value The enum constant to check. 
     * @return True if the entity has the provided constant, false otherwise.
     */
    template <typename E, if_t< is_enum<E>::value > = 0>
    bool has(E value) const {
        auto r = _::cpp_type<E>::id(m_world);
        auto o = enum_type<E>(m_world).entity(value);
        return ecs_has_pair(m_world, m_id, r, o);
    }

    /** Check if entity has the provided pair.
     *
     * @tparam Relation The relation type.
     * @tparam Object The object type.
     * @return True if the entity has the provided component, false otherwise.
     */
    template <typename Relation, typename Object>
    bool has() const {
        return this->has<Relation>(_::cpp_type<Object>::id(m_world));
    }

    /** Check if entity has the provided pair.
     *
     * @tparam R The relation type.
     * @param object The object.
     * @return True if the entity has the provided component, false otherwise.
     */
    template<typename R, typename O, if_not_t< is_enum<O>::value > = 0>
    bool has(O object) const {
        auto comp_id = _::cpp_type<R>::id(m_world);
        return ecs_has_id(m_world, m_id, ecs_pair(comp_id, object));
    }

    /** Check if entity has the provided pair.
     *
     * @tparam R The relation type.
     * @param value The enum constant.
     * @return True if the entity has the provided component, false otherwise.
     */
    template<typename R, typename E, if_t< is_enum<E>::value > = 0>
    bool has(E value) const {
        const auto& et = enum_type<E>(this->m_world);
        flecs::entity_t object = et.entity(value);
        return has<R>(object);
    }

    /** Check if entity has the provided pair.
     *
     * @param relation The relation.
     * @param object The object.
     * @return True if the entity has the provided component, false otherwise.
     */
    bool has(flecs::id_t relation, flecs::id_t object) const {
        return ecs_has_id(m_world, m_id, ecs_pair(relation, object));
    }

    /** Check if entity has the provided pair.
     *
     * @tparam Object The object type.
     * @param relation The relation.
     * @return True if the entity has the provided component, false otherwise.
     */
    template <typename Object>
    bool has_w_object(flecs::id_t relation) const {
        auto comp_id = _::cpp_type<Object>::id(m_world);
        return ecs_has_id(m_world, m_id, ecs_pair(relation, comp_id));
    }

    /** Check if entity owns the provided entity.
     * An entity is owned if it is not shared from a base entity.
     *
     * @param e The entity to check.
     * @return True if the entity owns the provided entity, false otherwise.
     */
    bool owns(flecs::id_t e) const {
        return ecs_owns_id(m_world, m_id, e);
    }

    /** Check if entity owns the provided pair.
     *
     * @tparam Relation The relation type.
     * @param object The object.
     * @return True if the entity owns the provided component, false otherwise.
     */
    template <typename Relation>
    bool owns(flecs::id_t object) const {
        auto comp_id = _::cpp_type<Relation>::id(m_world);
        return owns(ecs_pair(comp_id, object));
    }

    /** Check if entity owns the provided pair.
     *
     * @param relation The relation.
     * @param object The object.
     * @return True if the entity owns the provided component, false otherwise.
     */
    bool owns(flecs::id_t relation, flecs::id_t object) const {
        return owns(ecs_pair(relation, object));
    }

    /** Check if entity owns the provided component.
     * An component is owned if it is not shared from a base entity.
     *
     * @tparam T The component to check.
     * @return True if the entity owns the provided component, false otherwise.
     */
    template <typename T>
    bool owns() const {
        return owns(_::cpp_type<T>::id(m_world));
    }

    /** Check if entity has the provided switch.
     *
     * @param sw The switch to check.
     * @return True if the entity has the provided switch, false otherwise.
     */
    bool has_switch(const flecs::type& sw) const;

    template <typename T>
    bool has_switch() const {
        return ecs_has_id(m_world, m_id, 
            flecs::Switch | _::cpp_type<T>::id(m_world));
    }

    /** Check if entity has the provided case.
     *
     * @param sw_case The case to check.
     * @return True if the entity has the provided case, false otherwise.
     */
    bool has_case(flecs::id_t sw_case) const {
        return ecs_has_id(m_world, m_id, flecs::Case | sw_case);
    }

    template<typename T>
    bool has_case() const {
        return this->has_case(_::cpp_type<T>::id(m_world));
    }

    /** Get case for switch.
     *
     * @param sw The switch for which to obtain the case.
     * @return The entity representing the case.
     */
    flecs::entity get_case(flecs::id_t sw) const;

    /** Get case for switch.
     *
     * @tparam T The switch type for which to obtain the case.
     * @return The entity representing the case.
     */
    template<typename T> 
    flecs::entity get_case() const;

    /** Get case for switch.
     *
     * @param sw The switch for which to obtain the case.
     * @return The entity representing the case.
     */
    flecs::entity get_case(const flecs::type& sw) const;

    /** Test if component is enabled.
     *
     * @tparam T The component to test.
     * @return True if the component is enabled, false if it has been disabled.
     */
    template<typename T>
    bool is_enabled() {
        return ecs_is_component_enabled_w_id(
            m_world, m_id, _::cpp_type<T>::id(m_world));
    }

    /** Test if component is enabled.
     *
     * @param e The component to test.
     * @return True if the component is enabled, false if it has been disabled.
     */
    bool is_enabled(const flecs::entity_view& e) {
        return ecs_is_component_enabled_w_id(
            m_world, m_id, e);
    }

    /** Get current delta time.
     * Convenience function so system implementations can get delta_time, even
     * if they are using the .each() function.
     *
     * @return Current delta_time.
     */
    FLECS_FLOAT delta_time() const {
        const ecs_world_info_t *stats = ecs_get_world_info(m_world);
        return stats->delta_time;
    }

    flecs::entity clone(bool clone_value = true, flecs::entity_t dst_id = 0) const;

    /** Return mutable entity handle for current stage 
     * When an entity handle created from the world is used while the world is
     * in staged mode, it will only allow for readonly operations since 
     * structural changes are not allowed on the world while in staged mode.
     * 
     * To do mutations on the entity, this operation provides a handle to the
     * entity that uses the stage instead of the actual world.
     *
     * Note that staged entity handles should never be stored persistently, in
     * components or elsewhere. An entity handle should always point to the
     * main world.
     *
     * Also note that this operation is not necessary when doing mutations on an
     * entity outside of a system. It is allowed to do entity operations 
     * directly on the world, as long as the world is not in staged mode.
     *
     * @param stage The current stage.
     * @return An entity handle that allows for mutations in the current stage.
     */
    flecs::entity mut(const flecs::world& stage) const;

    /** Same as mut(world), but for iterator.
     * This operation allows for the construction of a mutable entity handle
     * from an iterator.
     *
     * @param it An iterator that contains a reference to the world or stage.
     * @return An entity handle that allows for mutations in the current stage.
     */
    flecs::entity mut(const flecs::iter& it) const;

    /** Same as mut(world), but for entity.
     * This operation allows for the construction of a mutable entity handle
     * from another entity. This is useful in each() functions, which only 
     * provide a handle to the entity being iterated over.
     *
     * @param e Another mutable entity.
     * @return An entity handle that allows for mutations in the current stage.
     */
    flecs::entity mut(const flecs::entity_view& e) const;

#   ifdef FLECS_JSON

flecs::string to_json(const flecs::entity_to_json_desc_t *desc = nullptr) {
    char *json = ecs_entity_to_json(m_world, m_id, desc);
    return flecs::string(json);
}

#   endif
#   ifdef FLECS_DOC

const char* doc_name() {
    return ecs_doc_get_name(m_world, m_id);
}

const char* doc_brief() {
    return ecs_doc_get_brief(m_world, m_id);
}

const char* doc_detail() {
    return ecs_doc_get_detail(m_world, m_id);
}

const char* doc_link() {
    return ecs_doc_get_link(m_world, m_id);
}

#   endif

private:
    flecs::entity set_stage(world_t *stage);
};

}

#pragma once

namespace flecs
{

template <typename Self>
struct entity_builder : entity_view {

    using entity_view::entity_view;

    /** Add a component to an entity.
     * To ensure the component is initialized, it should have a constructor.
     * 
     * @tparam T the component type to add.
     */
    template <typename T>
    Self& add() {
        flecs_static_assert(is_flecs_constructible<T>::value,
            "cannot default construct type: add T::T() or use emplace<T>()");
        ecs_add_id(this->m_world, this->m_id, _::cpp_type<T>::id(this->m_world));
        return to_base();
    }

    /** Add an entity to an entity.
     * Add an entity to the entity. This is typically used for tagging.
     *
     * @param component The component to add.
     */
    Self& add(id_t component) {
        ecs_add_id(this->m_world, this->m_id, component);
        return to_base();
    }

    /** Add a pair.
     * This operation adds a pair to the entity.
     *
     * @param relation The relation.
     * @param object The object.
     */
    Self& add(entity_t relation, entity_t object) {
        ecs_add_pair(this->m_world, this->m_id, relation, object);
        return to_base();
    }

    /** Add a pair.
     * This operation adds a pair to the entity.
     *
     * @tparam R The relation type
     * @tparam O The object type.
     */
    template<typename R, typename O>
    Self& add() {
        return this->add<R>(_::cpp_type<O>::id(this->m_world));
    }

    /** Add a pair.
     * This operation adds a pair to the entity.
     *
     * @tparam R The relation type
     * @param object The object.
     */
    template<typename R, typename O, if_not_t< is_enum<O>::value > = 0>
    Self& add(O object) {
        flecs_static_assert(is_flecs_constructible<R>::value,
            "cannot default construct type: add T::T() or use emplace<T>()");
        return this->add(_::cpp_type<R>::id(this->m_world), object);
    }

    /** Add a pair.
     * This operation adds a pair to the entity that consists out of a tag
     * combined with an enum constant.
     *
     * @tparam R The relation type
     * @param constant the enum constant.
     */
    template<typename R, typename O, if_t< is_enum<O>::value > = 0>
    Self& add(O constant) {
        flecs_static_assert(is_flecs_constructible<R>::value,
            "cannot default construct type: add T::T() or use emplace<T>()");
        const auto& et = enum_type<O>(this->m_world);
        flecs::entity_t object = et.entity(constant);
        return this->add<R>(object);
    }

    /** Conditional add.
     * This operation adds if condition is true, removes if condition is false.
     * 
     * @tparam T The component to add.
     * @param cond The condition to evaluate.
     */
    template <typename T>
    Self& add_if(bool cond) {
        if (cond) {
            return this->add<T>();
        } else {
            return this->remove<T>();
        }
    }

    /** Conditional add.
     * This operation adds if condition is true, removes if condition is false.
     * 
     * @param cond The condition to evaluate.
     * @param component The component to add.
     */
    Self& add_if(bool cond, flecs::id_t component) {
        if (cond) {
            return this->add(component);
        } else {
            return this->remove(component);
        }
    }

    /** Conditional add.
     * This operation adds if condition is true, removes if condition is false.
     * 
     * @tparam R The relation type
     * @tparam O The object type.
     * @param cond The condition to evaluate.
     */
    template <typename R, typename O>
    Self& add_if(bool cond) {
        if (cond) {
            return this->add<R, O>();
        } else {
            return this->remove<R, O>();
        }
    }

    /** Conditional add.
     * This operation adds if condition is true, removes if condition is false.
     * 
     * @tparam R The relation type
     * @param cond The condition to evaluate.
     * @param object The relation object.
     */
    template <typename R>
    Self& add_if(bool cond, flecs::entity_t object) {
        if (cond) {
            return this->add<R>(object);
        } else {
            return this->remove<R>(object);
        }
    }

    /** Conditional add.
     * This operation adds if condition is true, removes if condition is false.
     * 
     * @param cond The condition to evaluate.
     * @param relation The relation.
     * @param object The relation object.
     */
    Self& add_if(bool cond, flecs::entity_t relation, flecs::entity_t object) {
        if (cond) {
            return this->add(relation, object);
        } else {
            return this->remove(relation, object);
        }
    }

    /** Shortcut for add(IsA, obj).
     *
     * @param object The object.
     */
    Self& is_a(entity_t object) {
        return this->add(flecs::IsA, object);
    }

    /** Shortcut for add(IsA, obj).
     *
     * @tparam T the type associated with the O.
     */
    template <typename T>
    Self& is_a() {
        return this->add(flecs::IsA, _::cpp_type<T>::id(this->m_world));
    }

    /** Shortcut for add(ChildOf, obj).
     *
     * @param object The object.
     */
    Self& child_of(entity_t object) {
        return this->add(flecs::ChildOf, object);
    }

    /** Shortcut for add(ChildOf, obj).
     *
     * @tparam T the type associated with the O.
     */
    template <typename T>
    Self& child_of() {
        return this->add(flecs::ChildOf, _::cpp_type<T>::id(this->m_world));
    }
 
    /** Add a pair with O type.
     * This operation adds a pair to the entity. The R part of the pair
     * should not be a component.
     *
     * @param relation The relation.
     * @tparam O The object type.
     */
    template<typename O>
    Self& add_w_object(entity_t relation) {
        flecs_static_assert(is_flecs_constructible<O>::value,
            "cannot default construct type: add T::T() or use emplace<T>()");      
        return this->add(relation,  _::cpp_type<O>::id(this->m_world));
    }

    /** Remove a component from an entity.
     *
     * @tparam T the type of the component to remove.
     */
    template <typename T, if_not_t< is_enum<T>::value > = 0>
    Self& remove() {
        ecs_remove_id(this->m_world, this->m_id, _::cpp_type<T>::id(this->m_world));
        return to_base();
    }

    /** Remove an entity from an entity.
     *
     * @param entity The entity to remove.
     */
    Self& remove(entity_t entity) {
        ecs_remove_id(this->m_world, this->m_id, entity);
        return to_base();
    }

    /** Remove a pair.
     * This operation removes a pair from the entity.
     *
     * @param relation The relation.
     * @param object The object.
     */
    Self& remove(entity_t relation, entity_t object) {
        ecs_remove_pair(this->m_world, this->m_id, relation, object);
        return to_base();
    }

    /** Removes a pair.
     * This operation removes a pair from the entity.
     *
     * @tparam R The relation type
     * @tparam O The object type.
     */
    template<typename R, typename O>
    Self& remove() {
        return this->remove<R>(_::cpp_type<O>::id(this->m_world));
    }

    /** Remove a pair.
     * This operation adds a pair to the entity.
     *
     * @tparam R The relation type
     * @param object The object.
     */
    template<typename R, typename O, if_not_t< is_enum<O>::value > = 0>
    Self& remove(O object) {
        return this->remove(_::cpp_type<R>::id(this->m_world), object);
    }

    /** Remove a pair.
     * This operation adds a pair to the entity.
     *
     * @tparam R The relation type
     * @param constant the enum constant.
     */
    template<typename R, typename O, if_t< is_enum<O>::value > = 0>
    Self& remove(O constant) {
        const auto& et = enum_type<O>(this->m_world);
        flecs::entity_t object = et.entity(constant);
        return this->remove<R>(object);
    }  

    /** Removes a pair with O type.
     * This operation removes a pair from the entity.
     *
     * @param relation The relation.
     * @tparam O The object type.
     */
    template<typename O>
    Self& remove_w_object(entity_t relation) {
        return this->remove(relation, _::cpp_type<O>::id(this->m_world));
    }    

    /** Add owned flag for component (forces ownership when instantiating)
     *
     * @param entity The entity for which to add the OVERRIDE flag
     */    
    Self& override(entity_t entity) {
        ecs_add_id(this->m_world, this->m_id, ECS_OVERRIDE | entity);
        return to_base();  
    }

    /** Add owned flag for component (forces ownership when instantiating)
     *
     * @tparam T The component for which to add the OVERRIDE flag
     */    
    template <typename T>
    Self& override() {
        ecs_add_id(this->m_world, this->m_id, ECS_OVERRIDE | _::cpp_type<T>::id(this->m_world));
        return to_base();  
    }

    /** Set value, add owned flag.
     *
     * @tparam T The component to set and for which to add the OVERRIDE flag
     */    
    template <typename T>
    Self& set_override(T&& val) {
        this->override<T>();
        this->set<T>(FLECS_FWD(val));
        return to_base();  
    }

    /** Set value, add owned flag.
     *
     * @tparam T The component to set and for which to add the OVERRIDE flag
     */    
    template <typename T>
    Self& set_override(const T& val) {
        this->override<T>();
        this->set<T>(val);
        return to_base();  
    }

    /** Emplace value, add owned flag.
     *
     * @tparam T The component to set and for which to add the OVERRIDE flag
     */    
    template <typename T, typename ... Args>
    Self& emplace_override(Args&&... args) {
        this->override<T>();

        flecs::emplace<T>(this->m_world, this->m_id, 
            FLECS_FWD(args)...);

        return to_base();  
    }

    /** Add a switch to an entity by id.
     * The switch entity must be a type, that is it must have the EcsType
     * component. Entities created with flecs::type are valid here.
     *
     * @param sw The switch entity id to add.
     */    
    Self& add_switch(entity_t sw) {
        ecs_add_id(this->m_world, this->m_id, ECS_SWITCH | sw);
        return to_base();  
    }

    /** Add a switch to an entity by C++ type.
     * The C++ type must be associated with a switch type.
     *
     * @tparam T The switch to add.
     */ 
    template <typename T>
    Self& add_switch() {
        ecs_add_id(this->m_world, this->m_id, 
            ECS_SWITCH | _::cpp_type<T>::id());
        return to_base();  
    }

    /** Add a switch to an entity.
     * Any instance of flecs::type can be used as a switch.
     *
     * @param sw The switch to add.
     */     
    Self& add_switch(const flecs::type& sw);

    /** Remove a switch from an entity by id.
     *
     * @param sw The switch to remove.
     */    
    Self& remove_switch(entity_t sw) {
        ecs_remove_id(this->m_world, this->m_id, ECS_SWITCH | sw);
        return to_base();  
    }
    
    /** Add a switch to an entity by C++ type.
     * The C++ type must be associated with a switch type.
     *
     * @tparam T The switch to remove.
     */ 
    template <typename T>
    Self& remove_switch() {
        ecs_remove_id(this->m_world, this->m_id, 
            ECS_SWITCH | _::cpp_type<T>::id());
        return to_base();  
    }

    /** Remove a switch from an entity.
     * Any instance of flecs::type can be used as a switch.
     *
     * @param sw The switch to remove.
     */      
    Self& remove_switch(const flecs::type& sw);

    /** Add a switch to an entity by id.
     * The case must belong to a switch that is already added to the entity.
     *
     * @param sw_case The case entity id to add.
     */    
    Self& add_case(entity_t sw_case) {
        ecs_add_id(this->m_world, this->m_id, ECS_CASE | sw_case);
        return to_base();
    }

    /** Add a switch to an entity by id.
     * The case must belong to a switch that is already added to the entity.
     *
     * @tparam T The case to add.
     */   
    template<typename T>
    Self& add_case() {
        return this->add_case(_::cpp_type<T>::id());
    }

    /** Remove a case from an entity by id.
     * The case must belong to a switch that is already added to the entity.
     *
     * @param sw_case The case entity id to remove.
     */    
    Self& remove_case(entity_t sw_case) {
        ecs_remove_id(this->m_world, this->m_id, ECS_CASE | sw_case);
        return to_base();  
    }

    /** Remove a switch from an entity by id.
     * The case must belong to a switch that is already added to the entity.
     *
     * @tparam T The case to remove.
     */   
    template<typename T>
    Self& remove_case() {
        return this->remove_case(_::cpp_type<T>::id());
    }

    /** Add pair for enum constant.
     * This operation will add a pair to the entity where R is the enumeration
     * type, and O is the entity representing the enum constant.
     * 
     * The operation may be used with regular (C style) enumerations as well as
     * enum classes.
     * 
     * @param value The enumeration value.
     */
    template <typename E, if_t< is_enum<E>::value > = 0>
    Self& add(E value) {
        flecs::entity_t relation = _::cpp_type<E>::id(this->m_world);
        const auto& et = enum_type<E>(this->m_world);
        flecs::entity_t object = et.entity(value);
        return this->add(relation, object);
    }

    /** Remove pair for enum.
     * This operation will remove any (Enum, *) pair from the entity.
     * 
     * @tparam E The enumeration type.
     */
    template <typename E, if_t< is_enum<E>::value > = 0>
    Self& remove() {
        flecs::entity_t relation = _::cpp_type<E>::id(this->m_world);
        return this->remove(relation, flecs::Wildcard);
    }

    /** Enable an entity.
     * Enabled entities are matched with systems and can be searched with
     * queries.
     */
    Self& enable() {
        ecs_enable(this->m_world, this->m_id, true);
        return to_base();
    }

    /** Disable an entity.
     * Disabled entities are not matched with systems and cannot be searched 
     * with queries, unless explicitly specified in the query expression.
     */
    Self& disable() {
        ecs_enable(this->m_world, this->m_id, false);
        return to_base();
    }

    /** Enable a component.
     * This sets the enabled bit for this component. If this is the first time
     * the component is enabled or disabled, the bitset is added.
     *
     * @tparam T The component to enable.
     */   
    template<typename T>
    Self& enable() {
        ecs_enable_component_w_id(this->m_world, this->m_id, _::cpp_type<T>::id(), true);
        return to_base();
    }  

    /** Disable a component.
     * This sets the enabled bit for this component. If this is the first time
     * the component is enabled or disabled, the bitset is added.
     *
     * @tparam T The component to enable.
     */   
    template<typename T>
    Self& disable() {
        ecs_enable_component_w_id(this->m_world, this->m_id, _::cpp_type<T>::id(), false);
        return to_base();
    }  

    /** Enable a component.
     * See enable<T>.
     *
     * @param comp The component to enable.
     */   
    Self& enable(entity_t comp) {
        ecs_enable_component_w_id(this->m_world, this->m_id, comp, true);
        return to_base();       
    }

    /** Disable a component.
     * See disable<T>.
     *
     * @param comp The component to disable.
     */   
    Self& disable(entity_t comp) {
        ecs_enable_component_w_id(this->m_world, this->m_id, comp, false);
        return to_base();       
    }

    Self& set_ptr(entity_t comp, size_t size, const void *ptr) {
        ecs_set_id(this->m_world, this->m_id, comp, size, ptr);
        return to_base();
    }

    Self& set_ptr(entity_t comp, const void *ptr) {
        const flecs::Component *cptr = ecs_get(
            this->m_world, comp, EcsComponent);

        /* Can't set if it's not a component */
        ecs_assert(cptr != NULL, ECS_INVALID_PARAMETER, NULL);

        return set_ptr(comp, cptr->size, ptr);
    }

    template<typename T, if_t< 
        !is_callable<T>::value && is_actual<T>::value> = 0 >
    Self& set(T&& value) {
        flecs::set<T>(this->m_world, this->m_id, FLECS_FWD(value));
        return to_base();
    }

    template<typename T, if_t< 
        !is_callable<T>::value && is_actual<T>::value > = 0>
    Self& set(const T& value) {
        flecs::set<T>(this->m_world, this->m_id, value);
        return to_base();
    }

    template<typename T, typename A = actual_type_t<T>, if_not_t< 
        is_callable<T>::value || is_actual<T>::value > = 0>
    Self& set(A&& value) {
        flecs::set<T>(this->m_world, this->m_id, FLECS_FWD(value));
        return to_base();
    }

    template<typename T, typename A = actual_type_t<T>, if_not_t<
        is_callable<T>::value || is_actual<T>::value > = 0>
    Self& set(const A& value) {
        flecs::set<T>(this->m_world, this->m_id, value);
        return to_base();
    }

    /** Set a pair for an entity.
     * This operation sets the pair value, and uses the R as type. If the
     * entity did not yet have the pair, it will be added.
     *
     * @tparam R The relation type.
     * @tparam O The object type.
     * @param value The value to set.
     */
    template <typename R, typename O, typename P = pair<R, O>, 
        typename A = actual_type_t<P>, if_not_t< flecs::is_pair<R>::value> = 0>
    Self& set(const A& value) {
        flecs::set<P>(this->m_world, this->m_id, value);
        return to_base();
    }

    /** Set a pair for an entity.
     * This operation sets the pair value, and uses the R as type. If the
     * entity did not yet have the pair, it will be added.
     *
     * @tparam R The relation type.
     * @param object The object.
     * @param value The value to set.
     */
    template <typename R, typename O, if_not_t< is_enum<O>::value > = 0>
    Self& set(O object, const R& value) {
        auto relation = _::cpp_type<R>::id(this->m_world);
        flecs::set(this->m_world, this->m_id, value, 
            ecs_pair(relation, object));
        return to_base();
    }

    /** Set a pair for an entity.
     * This operation sets the pair value, and uses the R as type. If the
     * entity did not yet have the pair, it will be added.
     *
     * @tparam R The relation type.
     * @param constant The enum constant.
     * @param value The value to set.
     */
    template <typename R, typename O, if_t< is_enum<O>::value > = 0>
    Self& set(O constant, const R& value) {
        const auto& et = enum_type<O>(this->m_world);
        flecs::entity_t object = et.entity(constant);
        return set<R>(object, value);
    }

    /** Set a pair for an entity.
     * This operation sets the pair value, and uses the R as type. If the
     * entity did not yet have the pair, it will be added.
     *
     * @tparam O The object type.
     * @param relation The relation.
     * @param value The value to set.
     */
    template <typename O>
    Self& set_w_object(entity_t relation, const O& value) {
        auto object = _::cpp_type<O>::id(this->m_world);
        flecs::set(this->m_world, this->m_id, value, 
            ecs_pair(relation, object));
        return to_base();
    }

    template <typename R, typename O>
    Self& set_w_object(const O& value) {
        flecs::set<pair_object<R, O>>(this->m_world, this->m_id, value);
        return to_base();
    }    

    /** Set 1..N components.
     * This operation accepts a callback with as arguments the components to
     * set. If the entity does not have all of the provided components, they
     * will be added.
     *
     * This operation is faster than individually calling get for each component
     * as it only obtains entity metadata once. When this operation is called
     * while deferred, its performance is equivalent to that of calling get_mut
     * for each component separately.
     *
     * The operation will invoke modified for each component after the callback
     * has been invoked.
     *
     * @param func The callback to invoke.
     */
    template <typename Func, if_t< is_callable<Func>::value > = 0>
    Self& set(const Func& func);

    /** Emplace component.
     * Emplace constructs a component in the storage, which prevents calling the
     * destructor on the O passed into the function.
     *
     * Emplace attempts the following signatures to construct the component:
     *  T{Args...}
     *  T{flecs::entity, Args...}
     *
     * If the second signature matches, emplace will pass in the current entity 
     * as argument to the constructor, which is useful if the component needs
     * to be aware of the entity to which it has been added.
     *
     * Emplace may only be called for components that have not yet been added
     * to the entity.
     *
     * @tparam T the component to emplace
     * @param args The arguments to pass to the constructor of T
     */
    template <typename T, typename ... Args>
    Self& emplace(Args&&... args) {
        flecs::emplace<T>(this->m_world, this->m_id, 
            FLECS_FWD(args)...);
        return to_base();
    }

    /** Entities created in function will have the current entity.
     *
     * @param func The function to call.
     */
    template <typename Func>
    Self& with(const Func& func) {
        ecs_id_t prev = ecs_set_with(this->m_world, this->m_id);
        func();
        ecs_set_with(this->m_world, prev);
        return to_base();
    }

    /** Entities created in function will have (relation, this) 
     * This operation is thread safe.
     *
     * @tparam R The R to use.
     * @param func The function to call.
     */
    template <typename R, typename Func>
    Self& with(const Func& func) {
        with(_::cpp_type<R>::id(this->m_world), func);
        return to_base();
    }

    /** Entities created in function will have (relation, this) 
     *
     * @param relation The relation.
     * @param func The function to call.
     */
    template <typename Func>
    Self& with(entity_t relation, const Func& func) {
        ecs_id_t prev = ecs_set_with(this->m_world, 
            ecs_pair(relation, this->m_id));
        func();
        ecs_set_with(this->m_world, prev);
        return to_base();
    }

    /** The function will be ran with the scope set to the current entity. */
    template <typename Func>
    Self& scope(const Func& func) {
        ecs_entity_t prev = ecs_set_scope(this->m_world, this->m_id);
        func();
        ecs_set_scope(this->m_world, prev);
        return to_base();
    }

    /* Set the entity name.
     */
    Self& set_name(const char *name) {
        ecs_set_name(this->m_world, this->m_id, name);
        return to_base();
    }

#   ifdef FLECS_DOC

Self& set_doc_name(const char *name) {
    ecs_doc_set_name(m_world, m_id, name);
    return to_base();
}

Self& set_doc_brief(const char *brief) {
    ecs_doc_set_brief(m_world, m_id, brief);
    return to_base();
}

Self& set_doc_detail(const char *detail) {
    ecs_doc_set_detail(m_world, m_id, detail);
    return to_base();
}

Self& set_doc_link(const char *link) {
    ecs_doc_set_link(m_world, m_id, link);
    return to_base();
}

#   endif

#   ifdef FLECS_META

/** Make entity a unit */
Self& unit(
    const char *symbol, 
    flecs::entity_t prefix = 0,
    flecs::entity_t base = 0,
    flecs::entity_t over = 0,
    int32_t factor = 0,
    int32_t power = 0) 
{
    ecs_unit_desc_t desc = {};
    desc.entity.entity = this->m_id;
    desc.symbol = const_cast<char*>(symbol); /* safe, will be copied in */
    desc.base = base;
    desc.over = over;
    desc.prefix = prefix;
    desc.translation.factor = factor;
    desc.translation.power = power;
    ecs_unit_init(this->world(), &desc);

    return to_base();
}

/** Make entity a derived unit */
Self& unit( 
    flecs::entity_t prefix = 0,
    flecs::entity_t base = 0,
    flecs::entity_t over = 0,
    int32_t factor = 0,
    int32_t power = 0) 
{
    ecs_unit_desc_t desc = {};
    desc.entity.entity = this->m_id;
    desc.base = base;
    desc.over = over;
    desc.prefix = prefix;
    desc.translation.factor = factor;
    desc.translation.power = power;
    ecs_unit_init(this->world(), &desc);

    return to_base();
}

/** Make entity a derived unit */
Self& unit_prefix( 
    const char *symbol,
    int32_t factor = 0,
    int32_t power = 0) 
{
    ecs_unit_prefix_desc_t desc = {};
    desc.entity.entity = this->m_id;
    desc.symbol = const_cast<char*>(symbol); /* safe, will be copied in */
    desc.translation.factor = factor;
    desc.translation.power = power;
    ecs_unit_prefix_init(this->world(), &desc);

    return to_base();
}

/** Add quantity to unit */
Self& quantity(flecs::entity_t quantity) {
    ecs_add_pair(this->world(), this->id(), flecs::Quantity, quantity);
    return to_base();
}

/** Make entity a unity prefix */
template <typename Quantity>
Self& quantity() {
    return this->quantity(_::cpp_type<Quantity>::id(this->world()));
}

/** Make entity a quantity */
Self& quantity() {
    ecs_add_id(this->world(), this->id(), flecs::Quantity);
    return to_base();
}

#   endif

protected:
    Self& to_base() {
        return *static_cast<Self*>(this);
    }
};

}


namespace flecs
{

/** Entity class
 * This class provides access to entities. */
struct entity : entity_builder<entity>
{
    entity() : entity_builder<entity>() { }

    /** Create entity.
     *
     * @param world The world in which to create the entity.
     */
    explicit entity(world_t *world) 
        : entity_builder() 
    {
        m_world = world;
        m_id = ecs_new(world, 0);
    }

    /** Wrap an existing entity id.
     *
     * @param world The world in which the entity is created.
     * @param id The entity id.
     */
    explicit entity(const flecs::world_t *world, flecs::id_t id) {
        m_world = const_cast<flecs::world_t*>(world);
        m_id = id;
    }

    /** Create a named entity.
     * Named entities can be looked up with the lookup functions. Entity names
     * may be scoped, where each element in the name is separated by "::".
     * For example: "Foo::Bar". If parts of the hierarchy in the scoped name do
     * not yet exist, they will be automatically created.
     *
     * @param world The world in which to create the entity.
     * @param name The entity name.
     */
    explicit entity(world_t *world, const char *name) 
        : entity_builder()
    { 
        m_world = world;

        ecs_entity_desc_t desc = {};
        desc.name = name;
        desc.sep = "::";
        desc.root_sep = "::";
        m_id = ecs_entity_init(world, &desc);
    }

    /** Conversion from flecs::entity_t to flecs::entity. 
     * 
     * @param id The entity_t value to convert.
     */
    explicit entity(entity_t id) 
        : entity_builder( nullptr, id ) { }

    /** Get mutable component value.
     * This operation returns a mutable pointer to the component. If the entity
     * did not yet have the component, it will be added. If a base entity had
     * the component, it will be overridden, and the value of the base component
     * will be copied to the entity before this function returns.
     *
     * @tparam T The component to get.
     * @param is_added If provided, this parameter will be set to true if the component was added.
     * @return Pointer to the component value.
     */
    template <typename T>
    T* get_mut(bool *is_added = nullptr) const {
        auto comp_id = _::cpp_type<T>::id(m_world);
        ecs_assert(_::cpp_type<T>::size() != 0, ECS_INVALID_PARAMETER, NULL);
        return static_cast<T*>(
            ecs_get_mut_id(m_world, m_id, comp_id, is_added));
    }

    /** Get mutable component value (untyped).
     * This operation returns a mutable pointer to the component. If the entity
     * did not yet have the component, it will be added. If a base entity had
     * the component, it will be overridden, and the value of the base component
     * will be copied to the entity before this function returns.
     *
     * @param comp The component to get.
     * @param is_added If provided, this parameter will be set to true if the component was added.
     * @return Pointer to the component value.
     */
    void* get_mut(entity_t comp, bool *is_added = nullptr) const {
        return ecs_get_mut_id(m_world, m_id, comp, is_added);
    }

    /** Get mutable pointer for a pair.
     * This operation gets the value for a pair from the entity.
     *
     * @tparam Relation the relation type.
     * @tparam Object the object type.
     */
    template <typename Relation, typename Object>
    Relation* get_mut(bool *is_added = nullptr) const {
        return this->get_mut<Relation>(
            _::cpp_type<Object>::id(m_world), is_added);
    }

    /** Get mutable pointer for a pair.
     * This operation gets the value for a pair from the entity.
     *
     * @tparam Relation the relation type.
     * @param object the object.
     */
    template <typename Relation>
    Relation* get_mut(entity_t object, bool *is_added = nullptr) const {
        auto comp_id = _::cpp_type<Relation>::id(m_world);
        ecs_assert(_::cpp_type<Relation>::size() != 0, ECS_INVALID_PARAMETER, NULL);
        return static_cast<Relation*>(
            ecs_get_mut_id(m_world, m_id, 
                ecs_pair(comp_id, object), is_added));
    }

    /** Get mutable pointer for a pair (untyped).
     * This operation gets the value for a pair from the entity. If neither the
     * relation or object are a component, the operation will fail.
     *
     * @param relation the relation.
     * @param object the object.
     */
    void* get_mut(entity_t relation, entity_t object, bool *is_added = nullptr) const {
        return ecs_get_mut_id(m_world, m_id, 
                ecs_pair(relation, object), is_added);
    }

    /** Get mutable pointer for the object from a pair.
     * This operation gets the value for a pair from the entity.
     *
     * @tparam Object the object type.
     * @param relation the relation.
     */
    template <typename Object>
    Object* get_mut_w_object(entity_t relation, bool *is_added = nullptr) const {
        auto comp_id = _::cpp_type<Object>::id(m_world);
        ecs_assert(_::cpp_type<Object>::size() != 0, ECS_INVALID_PARAMETER, NULL);
        return static_cast<Object*>(
            ecs_get_mut_id(m_world, m_id, 
                ecs_pair(relation, comp_id), is_added));
    }           

    /** Signal that component was modified.
     *
     * @tparam T component that was modified.
     */
    template <typename T>
    void modified() const {
        auto comp_id = _::cpp_type<T>::id(m_world);
        ecs_assert(_::cpp_type<T>::size() != 0, ECS_INVALID_PARAMETER, NULL);
        this->modified(comp_id);
    } 

    /** Signal that the relation part of a pair was modified.
     *
     * @tparam Relation the relation type.
     * @tparam Object the object type.
     */
    template <typename Relation, typename Object>
    void modified() const {
        this->modified<Relation>(_::cpp_type<Object>::id(m_world));
    }

    /** Signal that the relation part of a pair was modified.
     *
     * @tparam Relation the relation type.
     * @param object the object.
     */
    template <typename Relation>
    void modified(entity_t object) const {
        auto comp_id = _::cpp_type<Relation>::id(m_world);
        ecs_assert(_::cpp_type<Relation>::size() != 0, ECS_INVALID_PARAMETER, NULL);
        this->modified(comp_id, object);
    }

    /** Signal that a pair has modified (untyped).
     * If neither the relation or object part of the pair are a component, the
     * operation will fail.
     *
     * @param relation the relation.
     * @param object the object.
     */
    void modified(entity_t relation, entity_t object) const {
        this->modified(ecs_pair(relation, object));
    }

    /** Signal that component was modified.
     *
     * @param comp component that was modified.
     */
    void modified(entity_t comp) const {
        ecs_modified_id(m_world, m_id, comp);
    }

    /** Get reference to component.
     * A reference allows for quick and safe access to a component value, and is
     * a faster alternative to repeatedly calling 'get' for the same component.
     *
     * @tparam T component for which to get a reference.
     * @return The reference.
     */
    template <typename T>
    ref<T> get_ref() const {
        // Ensure component is registered
        _::cpp_type<T>::id(m_world);
        ecs_assert(_::cpp_type<T>::size() != 0, ECS_INVALID_PARAMETER, NULL);
        return ref<T>(m_world, m_id);
    }

    /** Clear an entity.
     * This operation removes all components from an entity without recycling
     * the entity id.
     */
    void clear() const {
        ecs_clear(m_world, m_id);
    }

    /** Delete an entity.
     * Entities have to be deleted explicitly, and are not deleted when the
     * flecs::entity object goes out of scope.
     */
    void destruct() const {
        ecs_delete(m_world, m_id);
    }

    /** Entity id 0.
     * This function is useful when the API must provide an entity object that
     * belongs to a world, but the entity id is 0.
     *
     * @param world The world.
     */
    static
    flecs::entity null(const flecs::world_t *world) {
        flecs::entity result;
        result.m_world = const_cast<flecs::world_t*>(world);
        return result;
    }

    static
    flecs::entity null() {
        return flecs::entity();
    }
};

} // namespace flecs


////////////////////////////////////////////////////////////////////////////////
//// Utility class to invoke a system each
////////////////////////////////////////////////////////////////////////////////

namespace flecs
{

namespace _ 
{

// Utility to convert template argument pack to array of term ptrs
struct term_ptr {
    void *ptr;
    bool is_ref;
};

template <typename ... Components>
struct term_ptrs {
    using array = flecs::array<_::term_ptr, sizeof...(Components)>;

    bool populate(const ecs_iter_t *iter) {
        return populate(iter, 0, static_cast<
            remove_reference_t<
                remove_pointer_t<Components>>
                    *>(nullptr)...);
    }

    array m_terms;

private:
    /* Populate terms array without checking for references */
    bool populate(const ecs_iter_t*, size_t) { return false; }

    template <typename T, typename... Targs>
    bool populate(const ecs_iter_t *iter, size_t index, T, Targs... comps) {
        m_terms[index].ptr = iter->ptrs[index];
        bool is_ref = iter->subjects && iter->subjects[index] != 0;
        m_terms[index].is_ref = is_ref;
        is_ref |= populate(iter, index + 1, comps ...);
        return is_ref;
    }  
};    

struct invoker { };

// Template that figures out from the template parameters of a query/system
// how to pass the value to the each callback
template <typename T, typename = int>
struct each_column { };

// Base class
struct each_column_base {
    each_column_base(const _::term_ptr& term, size_t row) 
        : m_term(term), m_row(row) { }

protected:
    const _::term_ptr& m_term;
    size_t m_row;    
};

// If type is not a pointer, return a reference to the type (default case)
template <typename T>
struct each_column<T, if_t< !is_pointer<T>::value && 
        !is_empty<actual_type_t<T>>::value && is_actual<T>::value > > 
    : each_column_base 
{
    each_column(const _::term_ptr& term, size_t row) 
        : each_column_base(term, row) { }

    T& get_row() {
        return static_cast<T*>(this->m_term.ptr)[this->m_row];
    }  
};

// If argument type is not the same as actual component type, return by value.
// This requires that the actual type can be converted to the type.
// A typical scenario where this happens is when using flecs::pair types.
template <typename T>
struct each_column<T, if_t< !is_pointer<T>::value &&
        !is_empty<actual_type_t<T>>::value && !is_actual<T>::value> > 
    : each_column_base 
{
    each_column(const _::term_ptr& term, size_t row) 
        : each_column_base(term, row) { }

    T get_row() {
        return static_cast<actual_type_t<T>*>(this->m_term.ptr)[this->m_row];
    }  
};


// If type is empty (indicating a tag) the query will pass a nullptr. To avoid
// returning nullptr to reference arguments, return a temporary value.
template <typename T>
struct each_column<T, if_t< is_empty<actual_type_t<T>>::value && 
        !is_pointer<T>::value > > 
    : each_column_base 
{
    each_column(const _::term_ptr& term, size_t row) 
        : each_column_base(term, row) { }

    T get_row() {
        return actual_type_t<T>();
    }
};


// If type is a pointer (indicating an optional value) return the type as is
template <typename T>
struct each_column<T, if_t< is_pointer<T>::value && 
        !is_empty<actual_type_t<T>>::value > > 
    : each_column_base 
{
    each_column(const _::term_ptr& term, size_t row) 
        : each_column_base(term, row) { }

    T get_row() {
        if (this->m_term.ptr) {
            return &static_cast<actual_type_t<T>>(this->m_term.ptr)[this->m_row];
        } else {
            // optional argument doesn't hava a value
            return nullptr;
        }
    }
};

// If the query contains component references to other entities, check if the
// current argument is one.
template <typename T, typename = int>
struct each_ref_column : public each_column<T> {
    each_ref_column(const _::term_ptr& term, size_t row) 
        : each_column<T>(term, row) {

        if (term.is_ref) {
            // If this is a reference, set the row to 0 as a ref always is a
            // single value, not an array. This prevents the application from
            // having to do an if-check on whether the column is owned.
            //
            // This check only happens when the current table being iterated
            // over caused the query to match a reference. The check is
            // performed once per iterated table.
            this->m_row = 0;
        }
    }
};

template <typename Func, typename ... Components>
struct each_invoker : public invoker {
    // If the number of arguments in the function signature is one more than the
    // number of components in the query, an extra entity arg is required.
    static constexpr bool PassEntity = 
        (sizeof...(Components) + 1) == (arity<Func>::value);

    // If the number of arguments in the function is two more than the number of
    // components in the query, extra iter + index arguments are required.
    static constexpr bool PassIter = 
        (sizeof...(Components) + 2) == (arity<Func>::value);

    static_assert(arity<Func>::value > 0, 
        "each() must have at least one argument");

    using Terms = typename term_ptrs<Components ...>::array;

    template < if_not_t< is_same< void(Func), void(Func)& >::value > = 0>
    explicit each_invoker(Func&& func) noexcept 
        : m_func(FLECS_MOV(func)) { }

    explicit each_invoker(const Func& func) noexcept 
        : m_func(func) { }

    // Invoke object directly. This operation is useful when the calling
    // function has just constructed the invoker, such as what happens when
    // iterating a query.
    void invoke(ecs_iter_t *iter) const {
        term_ptrs<Components...> terms;

        if (terms.populate(iter)) {
            invoke_callback< each_ref_column >(iter, m_func, 0, terms.m_terms);
        } else {
            invoke_callback< each_column >(iter, m_func, 0, terms.m_terms);
        }   
    }

    // Static function that can be used as callback for systems/triggers
    static void run(ecs_iter_t *iter) {
        auto self = static_cast<const each_invoker*>(iter->binding_ctx);
        ecs_assert(self != nullptr, ECS_INTERNAL_ERROR, NULL);
        self->invoke(iter);
    }

    // Each invokers always use instanced iterators
    static bool instanced() {
        return true;
    }

private:
    // Number of function arguments is one more than number of components, pass
    // entity as argument.
    template <template<typename X, typename = int> class ColumnType, 
        typename... Args, if_t< 
            sizeof...(Components) == sizeof...(Args) && PassEntity> = 0>
    static void invoke_callback(
        ecs_iter_t *iter, const Func& func, size_t, Terms&, Args... comps) 
    {
        ECS_TABLE_LOCK(iter->world, iter->table);

        ecs_world_t *world = iter->world;
        size_t count = static_cast<size_t>(iter->count);

        for (size_t i = 0; i < count; i ++) {
            func(flecs::entity(world, iter->entities[i]),
                (ColumnType< remove_reference_t<Components> >(comps, i)
                    .get_row())...);
        }

        ECS_TABLE_UNLOCK(iter->world, iter->table);
    }


    // Number of function arguments is two more than number of components, pass
    // iter + index as argument.
    template <template<typename X, typename = int> class ColumnType, 
        typename... Args, int Enabled = PassIter, if_t< 
            sizeof...(Components) == sizeof...(Args) && Enabled> = 0>
    static void invoke_callback(
        ecs_iter_t *iter, const Func& func, size_t, Terms&, Args... comps) 
    {
        ECS_TABLE_LOCK(iter->world, iter->table);

        size_t count = static_cast<size_t>(iter->count);
        flecs::iter it(iter);

        for (size_t i = 0; i < count; i ++) {
            func(it, i, (ColumnType< remove_reference_t<Components> >(comps, i)
                .get_row())...);
        }

        ECS_TABLE_UNLOCK(iter->world, iter->table);
    }


    // Number of function arguments is equal to number of components, no entity
    template <template<typename X, typename = int> class ColumnType, 
        typename... Args, if_t< 
            sizeof...(Components) == sizeof...(Args) && !PassEntity && !PassIter> = 0>
    static void invoke_callback(
        ecs_iter_t *iter, const Func& func, size_t, Terms&, Args... comps) 
    {
        flecs::iter it(iter);

        ECS_TABLE_LOCK(iter->world, iter->table);

        for (auto row : it) {
            func( (ColumnType< remove_reference_t<Components> >(comps, row)
                .get_row())...);
        }

        ECS_TABLE_UNLOCK(iter->world, iter->table);
    }

    template <template<typename X, typename = int> class ColumnType, 
        typename... Args, if_t< sizeof...(Components) != sizeof...(Args) > = 0>
    static void invoke_callback(ecs_iter_t *iter, const Func& func, 
        size_t index, Terms& columns, Args... comps) 
    {
        invoke_callback<ColumnType>(
            iter, func, index + 1, columns, comps..., columns[index]);
    }    

    Func m_func;
};


////////////////////////////////////////////////////////////////////////////////
//// Utility class to invoke a system iterate action
////////////////////////////////////////////////////////////////////////////////

template <typename Func, typename ... Components>
struct iter_invoker : invoker {
private:
    static constexpr bool IterOnly = arity<Func>::value == 1;

    using Terms = typename term_ptrs<Components ...>::array;

public:
    template < if_not_t< is_same< void(Func), void(Func)& >::value > = 0>
    explicit iter_invoker(Func&& func) noexcept 
        : m_func(FLECS_MOV(func)) { }

    explicit iter_invoker(const Func& func) noexcept 
        : m_func(func) { }

    // Invoke object directly. This operation is useful when the calling
    // function has just constructed the invoker, such as what happens when
    // iterating a query.
    void invoke(ecs_iter_t *iter) const {
        term_ptrs<Components...> terms;
        terms.populate(iter);
        invoke_callback(iter, m_func, 0, terms.m_terms);
    }

    // Static function that can be used as callback for systems/triggers
    static void run(ecs_iter_t *iter) {
        auto self = static_cast<const iter_invoker*>(iter->binding_ctx);
        ecs_assert(self != nullptr, ECS_INTERNAL_ERROR, NULL);
        self->invoke(iter);
    }

    // Instancing needs to be enabled explicitly for iter invokers
    static bool instanced() {
        return false;
    }

private:
    template <typename... Args, if_t<!sizeof...(Args) && IterOnly> = 0>
    static void invoke_callback(ecs_iter_t *iter, const Func& func, 
        size_t, Terms&, Args...) 
    {
        flecs::iter it(iter);

        ECS_TABLE_LOCK(iter->world, iter->table);

        func(it);

        ECS_TABLE_UNLOCK(iter->world, iter->table);
    }

    template <typename... Targs, if_t<!IterOnly &&
        (sizeof...(Targs) == sizeof...(Components))> = 0>
    static void invoke_callback(ecs_iter_t *iter, const Func& func, size_t, 
        Terms&, Targs... comps) 
    {
        flecs::iter it(iter);

        ECS_TABLE_LOCK(iter->world, iter->table);

        func(it, ( static_cast< 
            remove_reference_t< 
                remove_pointer_t< 
                    actual_type_t<Components> > >* >
                        (comps.ptr))...);

        ECS_TABLE_UNLOCK(iter->world, iter->table);
    }

    template <typename... Targs, if_t<!IterOnly &&
        (sizeof...(Targs) != sizeof...(Components)) > = 0>
    static void invoke_callback(ecs_iter_t *iter, const Func& func, 
        size_t index, Terms& columns, Targs... comps) 
    {
        invoke_callback(iter, func, index + 1, columns, comps..., 
            columns[index]);
    }

    Func m_func;
};


////////////////////////////////////////////////////////////////////////////////
//// Utility to invoke callback on entity if it has components in signature
////////////////////////////////////////////////////////////////////////////////

template<typename ... Args>
struct entity_with_invoker_impl;

template<typename ... Args>
struct entity_with_invoker_impl<arg_list<Args ...>> {
    using ColumnArray = flecs::array<int32_t, sizeof...(Args)>;
    using ConstPtrArray = flecs::array<const void*, sizeof...(Args)>;
    using PtrArray = flecs::array<void*, sizeof...(Args)>;
    using DummyArray = flecs::array<int, sizeof...(Args)>;
    using IdArray = flecs::array<id_t, sizeof...(Args)>;

    template <typename ArrayType>
    static bool get_ptrs(world_t *world, ecs_record_t *r, ecs_table_t *table,
        ArrayType& ptrs) 
    {
        ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);

        ecs_table_t *storage_table = ecs_table_get_storage_table(table);
        if (!storage_table) {
            return false;
        }

        /* table_index_of needs real world */
        const flecs::world_t *real_world = ecs_get_world(world);

        /* Get column indices for components */
        ColumnArray columns ({
            ecs_search_offset(real_world, storage_table, 0, 
                _::cpp_type<Args>().id(world), 0)...
        });

        /* Get pointers for columns for entity */
        size_t i = 0;
        for (int32_t column : columns) {
            if (column == -1) {
                return false;
            }

            ptrs[i ++] = ecs_record_get_column(r, column, 0);
        }

        return true;
    }

    template <typename ArrayType>
    static bool get_mut_ptrs(world_t *world, ecs_entity_t e, ArrayType& ptrs) {
        /* Get pointers w/get_mut */
        size_t i = 0;
        DummyArray dummy ({
            (ptrs[i ++] = ecs_get_mut_id(world, e, 
                _::cpp_type<Args>().id(world), NULL), 0)...
        });

        return true;
    }    

    template <typename Func>
    static bool invoke_get(world_t *world, entity_t id, const Func& func) {
        ecs_record_t *r = ecs_record_find(world, id);
        if (!r) {
            return false;
        }

        ecs_table_t *table = r->table;
        if (!table) {
            return false;
        }

        ConstPtrArray ptrs;
        if (!get_ptrs(world, r, table, ptrs)) {
            return false;
        }

        ECS_TABLE_LOCK(world, table);

        invoke_callback(func, 0, ptrs);

        ECS_TABLE_UNLOCK(world, table);

        return true;
    }

    // Utility for storing id in array in pack expansion
    static size_t store_added(IdArray& added, size_t elem, ecs_table_t *prev, 
        ecs_table_t *next, id_t id) 
    {
        // Array should only contain ids for components that are actually added,
        // so check if the prev and next tables are different.
        if (prev != next) {
            added[elem] = id;
            elem ++;
        }
        return elem;
    }

    template <typename Func>
    static bool invoke_get_mut(world_t *world, entity_t id, const Func& func) {
        flecs::world w(world);

        PtrArray ptrs;
        ecs_table_t *table = NULL;

        // When not deferred take the fast path.
        if (!w.is_deferred()) {
            // Bit of low level code so we only do at most one table move & one
            // entity lookup for the entire operation.

            // Find table for entity
            ecs_record_t *r = ecs_record_find(world, id);
            if (r) {
                table = r->table;
            }

            // Find destination table that has all components
            ecs_table_t *prev = table, *next;
            size_t elem = 0;
            IdArray added;

            // Iterate components, only store added component ids in added array
            DummyArray dummy_before ({ (
                next = ecs_table_add_id(world, prev, w.id<Args>()),
                elem = store_added(added, elem, prev, next, w.id<Args>()),
                prev = next, 0
            )... });
            (void)dummy_before;

            // If table is different, move entity straight to it
            if (table != next) {
                ecs_type_t ids;
                ids.array = added.ptr();
                ids.count = static_cast<ecs_size_t>(elem);
                ecs_commit(world, id, r, next, &ids, NULL);
                table = next;
            }

            if (!get_ptrs(w, r, table, ptrs)) {
                ecs_abort(ECS_INTERNAL_ERROR, NULL);
            }

            ECS_TABLE_LOCK(world, table);

        // When deferred, obtain pointers with regular get_mut
        } else {
            get_mut_ptrs(world, id, ptrs);
        }

        invoke_callback(func, 0, ptrs);

        if (!w.is_deferred()) {
            ECS_TABLE_UNLOCK(world, table);
        }

        // Call modified on each component
        DummyArray dummy_after ({
            ( ecs_modified_id(world, id, w.id<Args>()), 0)...
        });
        (void)dummy_after;

        return true;
    }    

private:
    template <typename Func, typename ArrayType, typename ... TArgs, 
        if_t<sizeof...(TArgs) == sizeof...(Args)> = 0>
    static void invoke_callback(
        const Func& f, size_t, ArrayType&, TArgs&& ... comps) 
    {
        f(*static_cast<typename base_arg_type<Args>::type*>(comps)...);
    }

    template <typename Func, typename ArrayType, typename ... TArgs, 
        if_t<sizeof...(TArgs) != sizeof...(Args)> = 0>
    static void invoke_callback(const Func& f, size_t arg, ArrayType& ptrs, 
        TArgs&& ... comps) 
    {
        invoke_callback(f, arg + 1, ptrs, comps..., ptrs[arg]);
    }
};

template <typename Func, typename U = int>
struct entity_with_invoker {
    static_assert(function_traits<Func>::value, "type is not callable");
};

template <typename Func>
struct entity_with_invoker<Func, if_t< is_callable<Func>::value > >
    : entity_with_invoker_impl< arg_list_t<Func> >
{
    static_assert(function_traits<Func>::arity > 0,
        "function must have at least one argument");
};

} // namespace _

} // namespace flecs


namespace flecs {

template <typename ... Components>
struct iter_iterable;

template <typename ... Components>
struct page_iterable;

template <typename ... Components>
struct worker_iterable;

template <typename ... Components>
struct iterable {
    /** Each iterator.
     * The "each" iterator accepts a function that is invoked for each matching
     * entity. The following function signatures are valid:
     *  - func(flecs::entity e, Components& ...)
     *  - func(flecs::iter& it, int32_t index, Components& ....)
     *  - func(Components& ...)
     * 
     * Each iterators are automatically instanced.
     */
    template <typename Func>
    void each(Func&& func) const {
        iterate<_::each_invoker>(FLECS_FWD(func), 
            this->next_each_action());
    }

    /** Iter iterator.
     * The "iter" iterator accepts a function that is invoked for each matching
     * table. The following function signatures are valid:
     *  - func(flecs::iter& it, Components* ...)
     *  - func(Components& ...)
     * 
     * Iter iterators are not automatically instanced. When a result contains
     * shared components, entities of the result will be iterated one by one.
     * This ensures that applications can't accidentally read out of bounds by
     * accessing a shared component as an array.
     */
    template <typename Func>
    void iter(Func&& func) const { 
        iterate<_::iter_invoker>(FLECS_FWD(func), this->next_action());
    }

    /** Create iterator.
     * Create an iterator object that can be modified before iterating.
     */
    iter_iterable<Components...> iter() const;

    /** Page iterator.
     * Create an iterator that limits the returned entities with offset/limit.
     * 
     * @param offset How many entities to skip.
     * @param limit The maximum number of entities to return.
     * @return Iterable that can be iterated with each/iter.
     */
    page_iterable<Components...> page(int32_t offset, int32_t limit);

    /** Worker iterator.
     * Create an iterator that divides the number of matched entities across
     * a number of resources.
     * 
     * @param index The index of the current resource.
     * @param count The total number of resources to divide entities between.
     * @return Iterable that can be iterated with each/iter.
     */
    worker_iterable<Components...> worker(int32_t index, int32_t count);

    virtual ~iterable() { }
protected:
    friend iter_iterable<Components...>;
    friend page_iterable<Components...>;
    friend worker_iterable<Components...>;

    virtual ecs_iter_t get_iter() const = 0;
    virtual ecs_iter_next_action_t next_action() const = 0;
    virtual ecs_iter_next_action_t next_each_action() const = 0;

    template < template<typename Func, typename ... Comps> class Invoker, typename Func, typename NextFunc, typename ... Args>
    void iterate(Func&& func, NextFunc next, Args &&... args) const {
        ecs_iter_t it = this->get_iter();
        if (Invoker<Func, Components...>::instanced()) {
            ECS_BIT_SET(it.flags, EcsIterIsInstanced);
        }

        while (next(&it, FLECS_FWD(args)...)) {
            Invoker<Func, Components...>(func).invoke(&it);
        }
    }
};

template <typename ... Components>
struct iter_iterable final : iterable<Components...> {
    template <typename Iterable>
    iter_iterable(Iterable *it) 
    {
        m_it = it->get_iter();
        m_next = it->next_action();
        m_next_each = it->next_action();
    }

#   ifdef FLECS_RULES

iter_iterable<Components...>& set_var(int var_id, flecs::entity_t value) {
    ecs_assert(m_it.next == ecs_rule_next, ECS_INVALID_OPERATION, NULL);
    ecs_assert(var_id != -1, ECS_INVALID_PARAMETER, 0);
    ecs_iter_set_var(&m_it, var_id, value);
    return *this;
}

iter_iterable<Components...>& set_var(const char *name, flecs::entity_t value) {
    ecs_assert(m_it.next == ecs_rule_next, ECS_INVALID_OPERATION, NULL);
    ecs_rule_iter_t *rit = &m_it.priv.iter.rule;
    int var_id = ecs_rule_find_var(rit->rule, name);
    ecs_assert(var_id != -1, ECS_INVALID_PARAMETER, name);
    ecs_iter_set_var(&m_it, var_id, value);
    return *this;
}

#   endif
#   ifdef FLECS_JSON

flecs::string to_json(flecs::iter_to_json_desc_t *desc = nullptr) {
    char *json = ecs_iter_to_json(m_it.real_world, &m_it, desc);
    return flecs::string(json);
}

#   endif

    // Return total number of entities in result.
    int32_t count() {
        int32_t result = 0;
        while (m_next_each(&m_it)) {
            result += m_it.count;
        }
        return result;
    }

    // Returns true if iterator yields at least once result.
    bool is_true() {
        bool result = m_next_each(&m_it);
        if (result) {
            ecs_iter_fini(&m_it);
        }
        return result;
    }

protected:
    ecs_iter_t get_iter() const {
        return m_it;
    }

    ecs_iter_next_action_t next_action() const {
        return m_next;
    }

    ecs_iter_next_action_t next_each_action() const {
        return m_next_each;
    }

private:
    ecs_iter_t m_it;
    ecs_iter_next_action_t m_next;
    ecs_iter_next_action_t m_next_each;
};

template <typename ... Components>
iter_iterable<Components...> iterable<Components...>::iter() const
{
    return iter_iterable<Components...>(this);
}

template <typename ... Components>
struct page_iterable final : iterable<Components...> {
    template <typename Iterable>
    page_iterable(int32_t offset, int32_t limit, Iterable *it) 
        : m_offset(offset)
        , m_limit(limit)
    {
        m_chain_it = it->get_iter();
    }

protected:
    ecs_iter_t get_iter() const {
        return ecs_page_iter(&m_chain_it, m_offset, m_limit);
    }

    ecs_iter_next_action_t next_action() const {
        return ecs_page_next;
    }

    ecs_iter_next_action_t next_each_action() const {
        return ecs_page_next;
    }

private:
    ecs_iter_t m_chain_it;
    int32_t m_offset;
    int32_t m_limit;
};

template <typename ... Components>
page_iterable<Components...> iterable<Components...>::page(
    int32_t offset, 
    int32_t limit) 
{
    return page_iterable<Components...>(offset, limit, this);
}

template <typename ... Components>
struct worker_iterable final : iterable<Components...> {
    worker_iterable(int32_t offset, int32_t limit, iterable<Components...> *it) 
        : m_offset(offset)
        , m_limit(limit)
    {
        m_chain_it = it->get_iter();
    }

protected:
    ecs_iter_t get_iter() const {
        return ecs_worker_iter(&m_chain_it, m_offset, m_limit);
    }

    ecs_iter_next_action_t next_action() const {
        return ecs_worker_next;
    }

    ecs_iter_next_action_t next_each_action() const {
        return ecs_worker_next;
    }

private:
    ecs_iter_t m_chain_it;
    int32_t m_offset;
    int32_t m_limit;
};

template <typename ... Components>
worker_iterable<Components...> iterable<Components...>::worker(
    int32_t index, 
    int32_t count) 
{
    return worker_iterable<Components...>(index, count, this);
}

}

#pragma once
#include <stdio.h>
#include <ctype.h>

namespace flecs {

namespace _ {

// Trick to obtain typename from type, as described here
// https://blog.molecular-matters.com/2015/12/11/getting-the-type-of-a-template-argument-as-string-without-rtti/
//
// The code from the link has been modified to work with more types, and across
// multiple compilers. The resulting string should be the same on all platforms
// for all compilers.
//

#if defined(__GNUC__) || defined(_WIN32)
template <typename T>
inline static const char* type_name() {
    static const size_t len = ECS_FUNC_TYPE_LEN(const char*, type_name, ECS_FUNC_NAME);
    static char result[len + 1] = {};
    return ecs_cpp_get_type_name(result, ECS_FUNC_NAME, len);
} 
#else
#error "implicit component registration not supported"
#endif

// Translate a typename into a language-agnostic identifier. This allows for
// registration of components/modules across language boundaries.
template <typename T>
inline static const char* symbol_name() {
    static const size_t len = ECS_FUNC_TYPE_LEN(const char*, symbol_name, ECS_FUNC_NAME);
    static char result[len + 1] = {};  
    return ecs_cpp_get_symbol_name(result, type_name<T>(), len);
}

template <> inline const char* symbol_name<uint8_t>() {
    return "u8";
}
template <> inline const char* symbol_name<uint16_t>() {
    return "u16";
}
template <> inline const char* symbol_name<uint32_t>() {
    return "u32";
}
template <> inline const char* symbol_name<uint64_t>() {
    return "u64";
}
template <> inline const char* symbol_name<int8_t>() {
    return "i8";
}
template <> inline const char* symbol_name<int16_t>() {
    return "i16";
}
template <> inline const char* symbol_name<int32_t>() {
    return "i32";
}
template <> inline const char* symbol_name<int64_t>() {
    return "i64";
}
template <> inline const char* symbol_name<float>() {
    return "f32";
}
template <> inline const char* symbol_name<double>() {
    return "f64";
}

// If type is trivial, don't register lifecycle actions. While the functions
// that obtain the lifecycle callback do detect whether the callback is required
// adding a special case for trivial types eases the burden a bit on the
// compiler as it reduces the number of templates to evaluate.
template<typename T, enable_if_t<
    std::is_trivial<T>::value == true
        >* = nullptr>
void register_lifecycle_actions(ecs_world_t*, ecs_entity_t) { }

// If the component is non-trivial, register component lifecycle actions. 
// Depending on the type not all callbacks may be available.
template<typename T, enable_if_t<
    std::is_trivial<T>::value == false
        >* = nullptr>
void register_lifecycle_actions(
    ecs_world_t *world,
    ecs_entity_t component)
{
    if (!ecs_component_has_actions(world, component)) {
        EcsComponentLifecycle cl{};
        cl.ctor = ctor<T>();
        cl.dtor = dtor<T>();

        cl.copy = copy<T>();
        cl.copy_ctor = copy_ctor<T>();
        cl.move = move<T>();
        cl.move_ctor = move_ctor<T>();

        cl.ctor_move_dtor = ctor_move_dtor<T>();
        cl.move_dtor = move_dtor<T>();

        ecs_set_component_actions_w_id( world, component, &cl);
    }
}

// Class that manages component ids across worlds & binaries.
// The cpp_type class stores the component id for a C++ type in a static global
// variable that is shared between worlds. Whenever a component is used this
// class will check if it already has been registered (has the global id been
// set), and if not, register the component with the world.
//
// If the id has been set, the class will ensure it is known by the world. If it
// is not known the component has been registered by another world and will be
// registered with the world using the same id. If the id does exist, the class
// will register it as a component, and verify whether the input is consistent.
template <typename T>
struct cpp_type_impl {
    // Initialize component identifier
    static void init(
        world_t* world, 
        entity_t entity, 
        bool allow_tag = true) 
    {
        if (s_reset_count != ecs_cpp_reset_count_get()) {
            reset();
        }

        // If an identifier was already set, check for consistency
        if (s_id) {
            ecs_assert(s_name.c_str() != nullptr, ECS_INTERNAL_ERROR, NULL);
            ecs_assert(s_id == entity, ECS_INCONSISTENT_COMPONENT_ID, 
                type_name<T>());
            ecs_assert(allow_tag == s_allow_tag, ECS_INVALID_PARAMETER, NULL);

            // Component was already registered and data is consistent with new
            // identifier, so nothing else to be done.
            return;
        }

        // Component wasn't registered yet, set the values. Register component
        // name as the fully qualified flecs path.
        char *path = ecs_get_fullpath(world, entity);
        s_id = entity;
        s_name = flecs::string(path);
        s_allow_tag = allow_tag;
        s_size = sizeof(T);
        s_alignment = alignof(T);
        if (is_empty<T>::value && allow_tag) {
            s_size = 0;
            s_alignment = 0;
        }

        s_reset_count = ecs_cpp_reset_count_get();
    }

    // Obtain a component identifier for explicit component registration.
    static entity_t id_explicit(world_t *world = nullptr, 
        const char *name = nullptr, bool allow_tag = true, flecs::id_t id = 0,
        bool is_component = true)
    {
        if (!s_id) {
            // If no world was provided the component cannot be registered
            ecs_assert(world != nullptr, ECS_COMPONENT_NOT_REGISTERED, name);
        } else {
            ecs_assert(!id || s_id == id, ECS_INCONSISTENT_COMPONENT_ID, NULL);
            ecs_assert(s_allow_tag == allow_tag, ECS_INVALID_PARAMETER, NULL);
        }

        // If no id has been registered yet for the component (indicating the 
        // component has not yet been registered, or the component is used
        // across more than one binary), or if the id does not exists in the 
        // world (indicating a multi-world application), register it. */
        if (!s_id || (world && !ecs_exists(world, s_id))) {
            if (!s_id) {
                s_id = id;
            }

            ecs_assert(!id || s_id == id, ECS_INTERNAL_ERROR, NULL);

            init(world, s_id, allow_tag);

            entity_t entity = ecs_cpp_component_register_explicit(
                    world, s_id, id, name, type_name<T>(), symbol_name<T>(), 
                        s_size, s_alignment, is_component);

            s_id = entity;

            // If component is enum type, register constants
            _::init_enum<T>(world, entity);
        }

        // By now the identifier must be valid and known with the world.
        ecs_assert(s_id != 0 && ecs_exists(world, s_id), 
            ECS_INTERNAL_ERROR, NULL);

        return s_id;
    }

    // Obtain a component identifier for implicit component registration. This
    // is almost the same as id_explicit, except that this operation 
    // automatically registers lifecycle callbacks.
    // Additionally, implicit registration temporarily resets the scope & with
    // state of the world, so that the component is not implicitly created with
    // the scope/with of the code it happens to be first used by.
    static id_t id(world_t *world = nullptr, const char *name = nullptr, 
        bool allow_tag = true)
    {
        // If no id has been registered yet, do it now.
        if (!registered() || (world && !ecs_exists(world, s_id))) {
            ecs_entity_t prev_scope = 0;
            ecs_id_t prev_with = 0;

            if (world) {
                prev_scope = ecs_set_scope(world, 0);
                prev_with = ecs_set_with(world, 0);
            }
            
            // This will register a component id, but will not register 
            // lifecycle callbacks.
            id_explicit(world, name, allow_tag);

            // Register lifecycle callbacks, but only if the component has a
            // size. Components that don't have a size are tags, and tags don't
            // require construction/destruction/copy/move's. */
            if (size()) {
                register_lifecycle_actions<T>(world, s_id);
            }
            
            if (prev_with) {
                ecs_set_with(world, prev_with);
            }
            if (prev_scope) {
                ecs_set_scope(world, prev_scope);
            }
        }

        // By now we should have a valid identifier
        ecs_assert(s_id != 0, ECS_INTERNAL_ERROR, NULL);

        return s_id;
    }

    // Obtain a component name
    static const char* name(world_t *world = nullptr) {
        // If no id has been registered yet, do it now.
        if (!s_id) {
            id(world);
        }

        // By now we should have a valid identifier
        ecs_assert(s_id != 0, ECS_INTERNAL_ERROR, NULL);

        // If the id is set, the name should also have been set
        return s_name.c_str();
    }

    // Return the size of a component.
    static size_t size() {
        ecs_assert(s_id != 0, ECS_INTERNAL_ERROR, NULL);
        return s_size;
    }

    // Return the alignment of a component.
    static size_t alignment() {
        ecs_assert(s_id != 0, ECS_INTERNAL_ERROR, NULL);
        return s_alignment;
    }

    // Was the component already registered.
    static bool registered() {
        if (s_reset_count != ecs_cpp_reset_count_get()) {
            reset();
        }
        return s_id != 0;
    }

    // This function is only used to test cross-translation unit features. No
    // code other than test cases should invoke this function.
    static void reset() {
        s_id = 0;
        s_size = 0;
        s_alignment = 0;
        s_allow_tag = true;
        s_name.clear();
    }

    static entity_t s_id;
    static flecs::string s_name;
    static size_t s_size;
    static size_t s_alignment;
    static bool s_allow_tag;
    static int32_t s_reset_count;
};

// Global templated variables that hold component identifier and other info
template <typename T> entity_t      cpp_type_impl<T>::s_id;
template <typename T> flecs::string cpp_type_impl<T>::s_name;
template <typename T> size_t        cpp_type_impl<T>::s_size;
template <typename T> size_t        cpp_type_impl<T>::s_alignment;
template <typename T> bool          cpp_type_impl<T>::s_allow_tag( true );
template <typename T> int32_t       cpp_type_impl<T>::s_reset_count;

// Front facing class for implicitly registering a component & obtaining 
// static component data

// Regular type
template <typename T>
struct cpp_type<T, if_not_t< is_pair<T>::value >> 
    : cpp_type_impl<base_type_t<T>> { };

// Pair type
template <typename T>
struct cpp_type<T, if_t< is_pair<T>::value >>
{
    // Override id method to return id of pair
    static id_t id(world_t *world = nullptr) {
        return ecs_pair(
            cpp_type< pair_relation_t<T> >::id(world),
            cpp_type< pair_object_t<T> >::id(world));
    }
};

} // namespace _

struct untyped_component : entity {
    using entity::entity;
    
#   ifdef FLECS_META

/** Add member. */
untyped_component& member(flecs::entity_t type_id, const char *name, int32_t count = 0, size_t offset = 0) {
    ecs_entity_desc_t desc = {};
    desc.name = name;
    desc.add[0] = ecs_pair(flecs::ChildOf, m_id);
    ecs_entity_t eid = ecs_entity_init(m_world, &desc);
    ecs_assert(eid != 0, ECS_INTERNAL_ERROR, NULL);

    flecs::entity e(m_world, eid);

    Member m = {};
    m.type = type_id;
    m.count = count;
    m.offset = static_cast<int32_t>(offset);
    e.set<Member>(m);

    return *this;
}

/** Add member with unit. */
untyped_component& member(flecs::entity_t type_id, flecs::entity_t unit, const char *name, int32_t count = 0, size_t offset = 0) {
    ecs_entity_desc_t desc = {};
    desc.name = name;
    desc.add[0] = ecs_pair(flecs::ChildOf, m_id);
    ecs_entity_t eid = ecs_entity_init(m_world, &desc);
    ecs_assert(eid != 0, ECS_INTERNAL_ERROR, NULL);

    flecs::entity e(m_world, eid);

    Member m = {};
    m.type = type_id;
    m.unit = unit;
    m.count = count;
    m.offset = static_cast<int32_t>(offset);
    e.set<Member>(m);

    return *this;
}

/** Add member. */
template <typename MemberType>
untyped_component& member(const char *name, int32_t count = 0, size_t offset = 0) {
    flecs::entity_t type_id = _::cpp_type<MemberType>::id(m_world);
    return member(type_id, name, count, offset);
}

/** Add member with unit. */
template <typename MemberType>
untyped_component& member(flecs::entity_t unit, const char *name, int32_t count = 0, size_t offset = 0) {
    flecs::entity_t type_id = _::cpp_type<MemberType>::id(m_world);
    return member(type_id, unit, name, count, offset);
}

/** Add member with unit. */
template <typename MemberType, typename UnitType>
untyped_component& member(const char *name, int32_t count = 0, size_t offset = 0) {
    flecs::entity_t type_id = _::cpp_type<MemberType>::id(m_world);
    flecs::entity_t unit_id = _::cpp_type<UnitType>::id(m_world);
    return member(type_id, unit_id, name, count, offset);
}

/** Add constant. */
untyped_component& constant(const char *name, int32_t value) {
    ecs_add_id(m_world, m_id, _::cpp_type<flecs::Enum>::id(m_world));

    ecs_entity_desc_t desc = {};
    desc.name = name;
    desc.add[0] = ecs_pair(flecs::ChildOf, m_id);
    ecs_entity_t eid = ecs_entity_init(m_world, &desc);
    ecs_assert(eid != 0, ECS_INTERNAL_ERROR, NULL);

    ecs_set_id(m_world, eid, 
        ecs_pair(flecs::Constant, flecs::I32), sizeof(int32_t),
        &value);

    return *this;
}

/** Add bitmask constant. */
untyped_component& bit(const char *name, uint32_t value) {
    ecs_add_id(m_world, m_id, _::cpp_type<flecs::Bitmask>::id(m_world));

    ecs_entity_desc_t desc = {};
    desc.name = name;
    desc.add[0] = ecs_pair(flecs::ChildOf, m_id);
    ecs_entity_t eid = ecs_entity_init(m_world, &desc);
    ecs_assert(eid != 0, ECS_INTERNAL_ERROR, NULL);

    ecs_set_id(m_world, eid, 
        ecs_pair(flecs::Constant, flecs::U32), sizeof(uint32_t),
        &value);

    return *this;
}

#   endif
};

template <typename T>
struct component : untyped_component {
    /** Register a component.
     * If the component was already registered, this operation will return a handle
     * to the existing component.
     * 
     * @param world The world for which to register the component.
     * @param name Optional name (overrides typename).
     * @param allow_tag If true, empty types will be registered with size 0.
     * @param id Optional id to register component with.
     */
    component(
        flecs::world_t *world, 
        const char *name = nullptr, 
        bool allow_tag = true, 
        flecs::id_t id = 0) 
    {
        const char *n = name;
        bool implicit_name = false;
        if (!n) {
            n = _::type_name<T>();

            /* Keep track of whether name was explicitly set. If not, and the 
            * component was already registered, just use the registered name.
            *
            * The registered name may differ from the typename as the registered
            * name includes the flecs scope. This can in theory be different from
            * the C++ namespace though it is good practice to keep them the same */
            implicit_name = true;
        }

        if (_::cpp_type<T>::registered()) {
            /* Obtain component id. Because the component is already registered,
             * this operation does nothing besides returning the existing id */
            id = _::cpp_type<T>::id_explicit(world, name, allow_tag, id);

            ecs_cpp_component_validate(world, id, n,
                _::cpp_type<T>::size(),
                _::cpp_type<T>::alignment(),
                implicit_name);
        } else {
            /* If component is registered from an existing scope, ignore the
             * namespace in the name of the component. */
            if (implicit_name && (ecs_get_scope(world) != 0)) {
                const char *last_elem = strrchr(n, ':');
                if (last_elem) {
                    name = last_elem + 1;
                }
            }

            /* Find or register component */
            id = ecs_cpp_component_register(world, id, n, _::symbol_name<T>(),
                ECS_SIZEOF(T), ECS_ALIGNOF(T), implicit_name);

            /* Initialize static component data */
            id = _::cpp_type<T>::id_explicit(world, name, allow_tag, id);
        }

        /* Initialize lifecycle actions (ctor, dtor, copy, move) */
        if (_::cpp_type<T>::size()) {
            _::register_lifecycle_actions<T>(world, id);
        }

        m_world = world;
        m_id = id;
    }
};

/** Get id currently assigned to component. If no world has registered the
 * component yet, this operation will return 0. */
template <typename T>
flecs::entity_t type_id() {
    if (_::cpp_type<T>::s_reset_count == ecs_cpp_reset_count_get()) {
        return _::cpp_type<T>::s_id;
    } else {
        return 0;
    }
}

/** Reset static component variables.
 * When components are registered their component ids are stored in a static
 * type specific variable. This stored id is passed into component registration
 * functions to ensure consistent ids across worlds.
 * 
 * In some cases this can be undesirable, like when a process repeatedly creates
 * worlds with different components. A typical example where this can happen is
 * when running multiple tests in a single process, where each test registers
 * its own set of components.
 * 
 * This operation can be used to prevent reusing of component ids and force 
 * generating a new ids upon registration.
 * 
 * Note that this operation should *never* be called while there are still
 * alive worlds in a process. Doing so results in undefined behavior.
 * 
 * Also note that this operation does not actually change the static component
 * variables. It only ensures that the next time a component id is requested, a
 * new id will be generated.
 */
inline void reset() {
    ecs_cpp_reset_count_inc();
}

}

#pragma once

namespace flecs {

template <typename Base>
struct type_base {
    explicit type_base(
        world_t *world, const char *name = nullptr, const char *expr = nullptr)
    { 
        ecs_type_desc_t desc = {};
        desc.entity.name = name;
        desc.ids_expr = expr;
        m_entity = flecs::entity(world, ecs_type_init(world, &desc));
        sync_from_flecs();
    }

    explicit type_base(
        world_t *world, entity_t e)
    { 
        ecs_type_desc_t desc = {};
        desc.entity.entity = e;
        m_entity = flecs::entity(world, ecs_type_init(world, &desc));
        ecs_assert(!e || e == m_entity, ECS_INTERNAL_ERROR, nullptr);
        sync_from_flecs();
    }

    explicit type_base(world_t *world, table_t *t)
        : m_entity( flecs::entity::null(world) )
        , m_table( t )
    { }

    type_base(table_t *t)
        : m_table( t ) { }

    Base& add(id_t id) {
        flecs::world_t *world = this->world();

        if (!m_table) {
            const ecs_type_t *type = ecs_table_get_type(m_table);
            if (type) {
                for (int i = 0; i < type->count; i ++) {
                    m_table = ecs_table_add_id(world, m_table, type->array[i]);
                }
            }
        }

        m_table = ecs_table_add_id(world, m_table, id);
        sync_from_me();
        return *this;
    }

    template <typename T>
    Base& add() {
        return this->add(_::cpp_type<T>::id(world()));
    }

    Base& add(entity_t relation, entity_t object) {
        return this->add(ecs_pair(relation, object));
    }

    template <typename Relation, typename Object>
    Base& add() {
        return this->add<Relation>(_::cpp_type<Object>::id(world()));
    }

    Base& is_a(entity_t object) {
        return this->add(flecs::IsA, object);
    }

    Base& child_of(entity_t object) {
        return this->add(flecs::ChildOf, object);
    }    

    template <typename Relation>
    Base& add(entity_t object) {
        return this->add(_::cpp_type<Relation>::id(world()), object);
    }     

    template <typename Object>
    Base& add_w_object(entity_t relation) {
        return this->add(relation, _::cpp_type<Object>::id(world()));
    }

    bool has(id_t id) {
        const flecs::world_t *w = ecs_get_world(world());
        return ecs_search(w, m_table, id, 0) != -1;
    }

    bool has(id_t relation, id_t object) {
        return this->has(ecs_pair(relation, object));
    }    

    template <typename T>
    bool has() {
        return this->has(_::cpp_type<T>::id(world()));
    }

    template <typename Relation, typename Object>
    bool has() {
        return this->has(_::cpp_type<flecs::pair<Relation, Object>>::id(world()));
    }

    flecs::string str() const {
        const flecs::world_t *w = ecs_get_world(world());
        char *str = ecs_type_str(w, ecs_table_get_type(m_table));
        return flecs::string(str);
    }

    const type_t* c_ptr() const {
        return ecs_table_get_type(m_table);
    }

    flecs::id_t id() const { 
        return m_entity.id(); 
    }

    flecs::entity entity() const {
        return m_entity;
    }

    flecs::world world() const { 
        return m_entity.world();
    } 

    void enable() const {
        ecs_enable(world(), id(), true);
    }

    void disable() const {
        ecs_enable(world(), id(), false);
    }

    flecs::id get(int32_t index) {
        const ecs_type_t *type = ecs_table_get_type(m_table);
        if (!type || index >= type->count) {
            ecs_abort(ECS_OUT_OF_RANGE, 0);
            return flecs::id(world(), 0);
        }
        const flecs::world_t *w = ecs_get_world(world());
        return flecs::id(const_cast<flecs::world_t*>(w), type->array[index]);
    }

    size_t count() {
        const ecs_type_t *type = ecs_table_get_type(m_table);
        if (!type) {
            return 0;
        }
        return static_cast<size_t>(type->count);
    }

    /* Implicit conversion to type_t */
    operator const type_t*() const { return ecs_table_get_type(m_table); }

    operator Base&() { return *static_cast<Base*>(this); }

private:
    void sync_from_me() {
        if (!id()) {
            return;
        }

        EcsType *tc = ecs_get_mut(world(), id(), EcsType, NULL);
        ecs_assert(tc != NULL, ECS_INTERNAL_ERROR, NULL);
        tc->type = ecs_table_get_type(m_table);
        tc->normalized = m_table;
        ecs_modified(world(), id(), EcsType);
    }

    void sync_from_flecs() {
        if (!id()) {
            return;
        }

        const EcsType *tc = ecs_get(world(), id(), EcsType);
        if (!tc) {
            m_table = nullptr;
        } else {
            m_table = tc->normalized;
        }
    }

    flecs::entity m_entity;
    table_t *m_table = nullptr;
};

struct type : type_base<type> { 
    using type_base<type>::type_base;
};

}


// Mixin implementations

namespace flecs {

inline flecs::entity id::entity() const {
    ecs_assert(!is_pair(), ECS_INVALID_OPERATION, NULL);
    ecs_assert(!role(), ECS_INVALID_OPERATION, NULL);
    return flecs::entity(m_world, m_id);
}

inline flecs::entity id::role() const {
    return flecs::entity(m_world, m_id & ECS_ROLE_MASK);
}

inline flecs::entity id::first() const {
    ecs_assert(is_pair(), ECS_INVALID_OPERATION, NULL);

    flecs::entity_t e = ECS_PAIR_FIRST(m_id);
    if (m_world) {
        return flecs::entity(m_world, ecs_get_alive(m_world, e));
    } else {
        return flecs::entity(e);
    }
}

inline flecs::entity id::second() const {
    flecs::entity_t e = ECS_PAIR_SECOND(m_id);
    if (m_world) {
        return flecs::entity(m_world, ecs_get_alive(m_world, e));
    } else {
        return flecs::entity(m_world, e);
    }
}

inline flecs::entity id::relation() const {
    return first();
}

inline flecs::entity id::object() const {
    return second();
}

inline flecs::entity id::add_role(flecs::id_t role) const {
    return flecs::entity(m_world, m_id | role);
}

inline flecs::entity id::remove_role(flecs::id_t role) const {
    (void)role;
    ecs_assert((m_id & ECS_ROLE_MASK) == role, ECS_INVALID_PARAMETER, NULL);
    return flecs::entity(m_world, m_id & ECS_COMPONENT_MASK);
}

inline flecs::entity id::remove_role() const {
    return flecs::entity(m_world, m_id & ECS_COMPONENT_MASK);
}

inline flecs::entity id::remove_generation() const {
    return flecs::entity(m_world, static_cast<uint32_t>(m_id));
}

inline flecs::world id::world() const {
    return flecs::world(m_world);
}

inline flecs::entity id::type_id() const {
    return flecs::entity(m_world, ecs_get_typeid(m_world, m_id));
}


// Id mixin implementation

template <typename T>
inline flecs::id world::id() const {
    return flecs::id(m_world, _::cpp_type<T>::id(m_world));
}

template <typename ... Args>
inline flecs::id world::id(Args&&... args) const {
    return flecs::id(m_world, FLECS_FWD(args)...);
}

template <typename R, typename O>
inline flecs::id world::pair() const {
    return flecs::id(
        m_world, 
        ecs_pair(
            _::cpp_type<R>::id(m_world), 
            _::cpp_type<O>::id(m_world)));
}

template <typename R>
inline flecs::id world::pair(entity_t o) const {
    return flecs::id(
        m_world,
        ecs_pair(
            _::cpp_type<R>::id(m_world), 
            o));
}

inline flecs::id world::pair(entity_t r, entity_t o) const {
    return flecs::id(
        m_world,
        ecs_pair(r, o));
}

}

#pragma once

namespace flecs {

template <typename T>
flecs::entity ref<T>::entity() const {
    return flecs::entity(m_world, m_ref.entity);
}

template <typename Self>
inline Self& entity_builder<Self>::add_switch(const flecs::type& sw) {
    return add_switch(sw.id());
}

template <typename Self>
inline Self& entity_builder<Self>::remove_switch(const flecs::type& sw) {
    return remove_switch(sw.id());
}

template <typename Self>
template <typename Func, if_t< is_callable<Func>::value > >
inline Self& entity_builder<Self>::set(const Func& func) {
    _::entity_with_invoker<Func>::invoke_get_mut(
        this->m_world, this->m_id, func);
    return to_base();
}

inline bool entity_view::has_switch(const flecs::type& type) const {
    return ecs_has_id(m_world, m_id, flecs::Switch | type.id());
}

inline flecs::entity entity_view::get_case(const flecs::type& sw) const {
    return flecs::entity(m_world, ecs_get_case(m_world, m_id, sw.id()));
}

inline flecs::entity entity_view::get_case(flecs::id_t sw) const {
    return flecs::entity(m_world, ecs_get_case(m_world, m_id, sw));
}

template <typename T>
inline flecs::entity entity_view::get_case() const {
    return get_case(_::cpp_type<T>::id(m_world));
}

template <typename T, if_t< is_enum<T>::value > >
const T* entity_view::get() const {
    entity_t r = _::cpp_type<T>::id(m_world);
    entity_t c = ecs_get_object(m_world, m_id, r, 0);

    if (c) {
        // Get constant value from constant entity
        const T* v = static_cast<const T*>(ecs_get_id(m_world, c, r));
        ecs_assert(v != NULL, ECS_INTERNAL_ERROR, 
            "missing enum constant value");
        return v;
    } else {
        // If there is no matching pair for (r, *), try just r
        return static_cast<const T*>(ecs_get_id(m_world, m_id, r));
    }
}

template<typename R>
inline flecs::entity entity_view::get_object(int32_t index) const 
{
    return flecs::entity(m_world, 
        ecs_get_object(m_world, m_id, _::cpp_type<R>::id(m_world), index));
}

inline flecs::entity entity_view::get_object(
    flecs::entity_t relation, 
    int32_t index) const 
{
    return flecs::entity(m_world, 
        ecs_get_object(m_world, m_id, relation, index));
}

inline flecs::entity entity_view::get_object_for(
    flecs::entity_t relation, 
    flecs::id_t id) const 
{
    return flecs::entity(m_world, 
        ecs_get_object_for_id(m_world, m_id, relation, id));
}

template <typename T>
inline flecs::entity entity_view::get_object_for(flecs::entity_t relation) const {
    return get_object_for(relation, _::cpp_type<T>::id(m_world));
}

template <typename R, typename O>
inline flecs::entity entity_view::get_object_for(flecs::entity_t relation) const {
    return get_object_for(relation, _::cpp_type<R, O>::id(m_world));
}

inline flecs::entity entity_view::mut(const flecs::world& stage) const {
    ecs_assert(!stage.is_readonly(), ECS_INVALID_PARAMETER, 
        "cannot use readonly world/stage to create mutable handle");
    return flecs::entity(m_id).set_stage(stage.c_ptr());
}

inline flecs::entity entity_view::mut(const flecs::iter& it) const {
    ecs_assert(!it.world().is_readonly(), ECS_INVALID_PARAMETER, 
        "cannot use iterator created for readonly world/stage to create mutable handle");
    return flecs::entity(m_id).set_stage(it.world().c_ptr());
}

inline flecs::entity entity_view::mut(const flecs::entity_view& e) const {
    ecs_assert(!e.world().is_readonly(), ECS_INVALID_PARAMETER, 
        "cannot use entity created for readonly world/stage to create mutable handle");
    return flecs::entity(m_id).set_stage(e.m_world);
}

inline flecs::entity entity_view::set_stage(world_t *stage) {
    return flecs::entity(stage, m_id);
}   

inline flecs::type entity_view::type() const {
    return flecs::type(m_world, ecs_get_table(m_world, m_id));
}

template <typename Func>
inline void entity_view::each(const Func& func) const {
    const ecs_type_t *type = ecs_get_type(m_world, m_id);
    if (!type) {
        return;
    }

    const ecs_id_t *ids = type->array;
    int32_t count = type->count;

    for (int i = 0; i < count; i ++) {
        ecs_id_t id = ids[i];
        flecs::id ent(m_world, id);
        func(ent); 

        // Case is not stored in type, so handle separately
        if ((id & ECS_ROLE_MASK) == flecs::Switch) {
            ent = flecs::id(
                m_world, flecs::Case | ecs_get_case(
                        m_world, m_id, ent.second().id()));
            func(ent);
        }
    }
}

template <typename Func>
inline void entity_view::each(flecs::id_t pred, flecs::id_t obj, const Func& func) const {
    flecs::world_t *real_world = const_cast<flecs::world_t*>(
        ecs_get_world(m_world));

    const ecs_table_t *table = ecs_get_table(m_world, m_id);
    if (!table) {
        return;
    }

    const ecs_type_t *type = ecs_table_get_type(table);
    if (!type) {
        return;
    }

    flecs::id_t pattern = pred;
    if (obj) {
        pattern = ecs_pair(pred, obj);
    }

    int32_t cur = 0;
    id_t *ids = type->array;
    
    while (-1 != (cur = ecs_search_offset(real_world, table, cur, pattern, 0)))
    {
        flecs::id ent(m_world, ids[cur]);
        func(ent);
        cur ++;
    }
}

template <typename Func>
inline void entity_view::each(const flecs::entity_view& rel, const Func& func) const {
    return this->each(rel, flecs::Wildcard, [&](flecs::id id) {
        flecs::entity obj = id.second();
        func(obj);
    });
}

template <typename Func, if_t< is_callable<Func>::value > >
inline bool entity_view::get(const Func& func) const {
    return _::entity_with_invoker<Func>::invoke_get(m_world, m_id, func);
} 

inline flecs::entity entity_view::lookup(const char *path) const {
    auto id = ecs_lookup_path_w_sep(m_world, m_id, path, "::", "::", false);
    return flecs::entity(m_world, id);
}

inline flecs::entity entity_view::clone(bool copy_value, flecs::entity_t dst_id) const {
    if (!dst_id) {
        dst_id = ecs_new_id(m_world);
    }

    flecs::entity dst = flecs::entity(m_world, dst_id);
    ecs_clone(m_world, dst_id, m_id, copy_value);
    return dst;
}

// Entity mixin implementation
template <typename... Args>
inline flecs::entity world::entity(Args &&... args) const {
    return flecs::entity(m_world, FLECS_FWD(args)...);
}

template <typename E, if_t< is_enum<E>::value >>
inline flecs::entity world::id(E value) const {
    flecs::entity_t constant = enum_type<E>(m_world).entity(value);
    return flecs::entity(m_world, constant);
}

template <typename T>
inline flecs::entity world::entity(const char *name) const {
    return flecs::entity(m_world, 
        _::cpp_type<T>::id_explicit(m_world, name, true, 0, false) );
}

template <typename... Args>
inline flecs::entity world::prefab(Args &&... args) const {
    flecs::entity result = flecs::entity(m_world, FLECS_FWD(args)...);
    result.add(flecs::Prefab);
    return result;
}

template <typename T>
inline flecs::entity world::prefab(const char *name) const {
    flecs::entity result = flecs::component<T>(m_world, name, true);
    result.add(flecs::Prefab);
    return result;
}

}

#pragma once

namespace flecs {

// Component mixin implementation
template <typename T, typename... Args>
inline flecs::component<T> world::component(Args &&... args) const {
    return flecs::component<T>(m_world, FLECS_FWD(args)...);
}

template <typename... Args>
inline flecs::untyped_component world::component(Args &&... args) const {
    return flecs::untyped_component(m_world, FLECS_FWD(args)...);
}

} // namespace flecs

#pragma once

namespace flecs {

// Type mixin implementation
template <typename... Args>
inline flecs::type world::type(Args &&... args) const {
    return flecs::type(m_world, FLECS_FWD(args)...);
}

template <typename T>
inline flecs::type world::type(const char *name) const {
    flecs::entity result = flecs::component<T>(m_world, name, true);
    return flecs::type(m_world, result);
}

}

#pragma once

#pragma once

#pragma once

namespace flecs {
namespace _ {

    template <typename T, if_t< is_const_p<T>::value > = 0>
    static constexpr flecs::inout_kind_t type_to_inout() {
        return flecs::In;
    }

    template <typename T, if_t< is_reference<T>::value > = 0>
    static constexpr flecs::inout_kind_t type_to_inout() {
        return flecs::Out;
    }

    template <typename T, if_not_t< 
        is_const_p<T>::value || is_reference<T>::value > = 0>
    static constexpr flecs::inout_kind_t type_to_inout() {
        return flecs::InOutDefault;
    }

    template <typename T, if_t< is_pointer<T>::value > = 0>
    static constexpr flecs::oper_kind_t type_to_oper() {
        return flecs::Optional;
    }

    template <typename T, if_not_t< is_pointer<T>::value > = 0>
    static constexpr flecs::oper_kind_t type_to_oper() {
        return flecs::And;
    }

    template <typename ... Components>
    struct sig {
        sig(flecs::world_t *world) 
            : ids({ (_::cpp_type<Components>::id(world))... })
            , inout ({ (type_to_inout<Components>())... })
            , oper ({ (type_to_oper<Components>())... }) 
        { (void)world; }

        flecs::array<flecs::id_t, sizeof...(Components)> ids;
        flecs::array<flecs::inout_kind_t, sizeof...(Components)> inout;
        flecs::array<flecs::oper_kind_t, sizeof...(Components)> oper;

        template <typename Builder>
        void populate(const Builder& b) {
            size_t i = 0;
            for (auto id : ids) {
                b->term(id).inout(inout[i]).oper(oper[i]);
                i ++;
            }
        }
    };

} // namespace _
} // namespace flecs


namespace flecs 
{

/** Term identifier builder.
 * A term identifier is either the predicate (pred), subject (subj) or object
 * (obj) of a term. Use the term builder to select the term identifier. */
template<typename Base>
struct term_id_builder_i {
    term_id_builder_i() : m_term_id(nullptr) { }

    virtual ~term_id_builder_i() { }

    /** Assign entity from type to currently selected term identifier. */
    template<typename T>
    Base& entity() {
        ecs_assert(m_term_id != NULL, ECS_INVALID_PARAMETER, NULL);
        m_term_id->entity = _::cpp_type<T>::id(this->world_v());
        return *this;
    }

    /** Assign entity to currently selected term identifier. */
    Base& entity(flecs::id_t id) {
        ecs_assert(m_term_id != NULL, ECS_INVALID_PARAMETER, NULL);
        m_term_id->entity = id;
        return *this;
    }

    /** Assign name to currently selected term identifier. 
     * The name will be resolved to either an entity or a variable name,
     * depending on the following rules (which are the same as the query DSL):
     * 
     * If the variable kind is flecs::VarIsDefault:
     * - If the name is a single uppercase value it is parsed as a variable
     * - If the name starts with an underscore it is parsed as a variable
     * - In any other case the name is parsed as an entity identifier
     * 
     * If the variable kind is flecs::VarIsEntity:
     * - The name is parsed as an entity identifier
     * 
     * If the variable kind is flecs::VarIsVariable:
     * - The name is interpreted as a variable
     */
    Base& name(const char *name) {
        ecs_assert(m_term_id != NULL, ECS_INVALID_PARAMETER, NULL);
        m_term_id->name = const_cast<char*>(name);
        return *this;
    }

    /** Set whether the currently selected term id is a variable or entity. */
    Base& var(flecs::var_kind_t var = flecs::VarIsVariable) {
        m_term_id->var = static_cast<ecs_var_kind_t>(var);
        return *this;
    }

    /** Set the current term id to be a variable. */
    Base& var(const char *name) {
        ecs_assert(m_term_id != NULL, ECS_INVALID_PARAMETER, NULL);
        // Const cast is safe, when the value is actually used to construct a
        // query, it will be duplicated.
        m_term_id->name = const_cast<char*>(name);
        return var(); // Default to VarIsVariable
    }

    /** Assign set mask and relation. */
    Base& set(uint8_t mask, const flecs::id_t relation = flecs::IsA)
    {
        ecs_assert(m_term_id != NULL, ECS_INVALID_PARAMETER, NULL);
        m_term_id->set.mask = mask;
        m_term_id->set.relation = relation;
        return *this;
    }

    /** Shorthand for .set(flecs::SuperSet | mask, relation). */
    Base& super(const flecs::id_t relation = flecs::IsA, uint8_t mask = 0)
    {
        ecs_assert(!(mask & flecs::SubSet), ECS_INVALID_PARAMETER, NULL);
        return set(flecs::SuperSet | mask, relation);
    }

    /** Shorthand for .set(flecs::SubSet | mask, relation). */
    Base& sub(const flecs::id_t relation = flecs::IsA, uint8_t mask = 0)
    {
        ecs_assert(!(mask & flecs::SuperSet), ECS_INVALID_PARAMETER, NULL);
        return set(flecs::SubSet | mask, relation);
    }

    /** Set min_depth for set substitution. */
    Base& min_depth(int32_t min_depth) {
        m_term_id->set.min_depth = min_depth;
        return *this;
    }

    /** Set min_depth for set substitution. */
    Base& max_depth(int32_t max_depth) {
        m_term_id->set.max_depth = max_depth;
        return *this;
    }

    Base& owned(bool value = true) {
        if (value) {
            m_term_id->set.mask = flecs::Self;
        } else {
            m_term_id->set.mask = flecs::SuperSet;
            m_term_id->set.relation = flecs::IsA;
        }
        return *this;
    }

    ecs_term_id_t *m_term_id;
    
protected:
    virtual flecs::world_t* world_v() = 0;

private:
    operator Base&() {
        return *static_cast<Base*>(this);
    }
};

/** Term builder. A term is a single element of a query expression. */
template<typename Base>
struct term_builder_i : term_id_builder_i<Base> {
    term_builder_i() : m_term(nullptr) { }

    term_builder_i(ecs_term_t *term_ptr) { 
        set_term(term_ptr);
    }

    /** Set (component) id to type id. */
    template<typename T>
    Base& id() {
        ecs_assert(m_term != nullptr, ECS_INVALID_PARAMETER, NULL);
        m_term->pred.entity = _::cpp_type<T>::id(this->world_v());
        return *this;
    }

    /** Set (component) id to id. */
    Base& id(id_t id) {
        ecs_assert(m_term != nullptr, ECS_INVALID_PARAMETER, NULL);
        m_term->pred.entity = id;
        return *this;
    }

    /** Alias for id, for compatibility with populate_filter_from_pack */
    Base& term(id_t id) {
        return this->id(id);
    }

    /** Set (component) id to type.
     * Type must be associated with an entity (e.g. created by world::type) and
     * not an entity type (e.g. returned from entity::type). */
    Base& id(const flecs::type& type);

    Base& id(const flecs::type& type, id_t id);

    /** Set (component) id to pair derived from relation id / object id */
    Base& id(id_t r, id_t o) {
        ecs_assert(m_term != nullptr, ECS_INVALID_PARAMETER, NULL);
        m_term->pred.entity = r;
        m_term->obj.entity = o;
        return *this;
    }

    /** Set (component) id to pair derived from two types. */
    template<typename R, typename O>
    Base& id() {
        ecs_assert(m_term != nullptr, ECS_INVALID_PARAMETER, NULL);
        m_term->pred.entity = _::cpp_type<R>::id(this->world_v());
        m_term->obj.entity = _::cpp_type<O>::id(this->world_v());
        return *this;
    }

    /** Set (component) id to pair derived from relation type / object id. */
    template<typename R>
    Base& id(id_t o) {
        ecs_assert(m_term != nullptr, ECS_INVALID_PARAMETER, NULL);
        m_term->pred.entity = _::cpp_type<R>::id(this->world_v());
        m_term->obj.entity = o;
        return *this;
    }    

    /** Set term from expression.
     * The syntax for expr is the same as that of the query DSL. The expression
     * must only contain a single term, for example:
     *   Position // correct
     *   Position, Velocity // incorrect
     */
    Base& expr(const char *expr) {
#       ifdef FLECS_PARSER
        ecs_assert(m_term != nullptr, ECS_INVALID_PARAMETER, NULL);
        const char *ptr;
        if ((ptr = ecs_parse_term(this->world_v(), nullptr, expr, expr, m_term)) == nullptr) {
            ecs_abort(ECS_INVALID_PARAMETER, NULL);
        }

        m_term->move = true;

        // Should not have more than one term
        ecs_assert(ptr[0] == 0, ECS_INVALID_PARAMETER, NULL);
#       else
        (void)expr;
        ecs_abort(ECS_UNSUPPORTED, "parser addon required for expr()");
#       endif
        return *this;
    }

    /** Select predicate of term. 
     * Use methods from term_builder to configure properties of predicate. */
    Base& pred() {
        ecs_assert(m_term != nullptr, ECS_INVALID_PARAMETER, NULL);
        this->m_term_id = &m_term->pred;
        return *this;
    }

    /** Select subject of term. 
     * Use methods from term_builder to configure properties of subject. */
    Base& subj() {
        ecs_assert(m_term != nullptr, ECS_INVALID_PARAMETER, NULL);
        this->m_term_id = &m_term->subj;
        return *this;
    }

    /** Select object of term. 
     * Use methods from term_builder to configure properties of object. Setting
     * the object of a term will turn the term into a pair, and requires the
     * predicate to also be set. */
    Base& obj() {
        ecs_assert(m_term != nullptr, ECS_INVALID_PARAMETER, NULL);
        this->m_term_id = &m_term->obj;
        return *this;
    }

    /** Select subject of term, initialize it with specified entity. */
    Base& subj(entity_t entity) {
        this->subj();
        this->m_term_id->entity = entity;
        return *this;
    }

    /** Select object of term, initialize it with specified entity. */
    Base& obj(entity_t entity) {
        this->obj();
        this->m_term_id->entity = entity;
        return *this;
    }

    /** Select predicate of term, initialize it with specified name. */
    Base& pred(const char *n) {
        this->pred();
        this->m_term_id->name = const_cast<char*>(n);
        return *this;
    }

    /** Select subject of term, initialize it with specified name. */
    Base& subj(const char *n) {
        this->subj();
        this->m_term_id->name = const_cast<char*>(n);
        return *this;
    }

    /** Select object of term, initialize it with specified name. */
    Base& obj(const char *n) {
        this->obj();
        this->m_term_id->name = const_cast<char*>(n);
        return *this;
    }

    /** Select subject of term, initialize it with id from specified type. */
    template<typename T>
    Base& subj() {
        this->subj();
        this->m_term_id->entity = _::cpp_type<T>::id(this->world_v());
        return *this;
    }

    /** Select object of term, initialize it with id from specified type. */
    template<typename T>
    Base& obj() {
        this->obj();
        this->m_term_id->entity = _::cpp_type<T>::id(this->world_v());
        return *this;
    }

    /** Set role of term. */
    Base& role(id_t role) {
        ecs_assert(m_term != nullptr, ECS_INVALID_PARAMETER, NULL);
        m_term->role = role;
        return *this;
    }

    /** Set read/write access of term. */
    Base& inout(flecs::inout_kind_t inout) {
        ecs_assert(m_term != nullptr, ECS_INVALID_PARAMETER, NULL);
        m_term->inout = static_cast<ecs_inout_kind_t>(inout);
        return *this;
    }

    /** Scheduler annotation to indicate system uses add<T> */
    Base& add() {
        ecs_assert(m_term != nullptr, ECS_INVALID_PARAMETER, NULL);
        m_term->inout = static_cast<ecs_inout_kind_t>(flecs::Out);
        m_term->subj.set.mask = flecs::Nothing;
        return *this;
    }

    /** Scheduler annotation to indicate sytem uses add<T>, remove<T> or set<T> */
    Base& write() {
        ecs_assert(m_term != nullptr, ECS_INVALID_PARAMETER, NULL);
        m_term->inout = static_cast<ecs_inout_kind_t>(flecs::Out);
        m_term->subj.set.mask = flecs::Nothing;
        return *this;
    }

    /** Scheduler annotation to indicate sytem uses get<T> */
    Base& read() {
        ecs_assert(m_term != nullptr, ECS_INVALID_PARAMETER, NULL);
        m_term->inout = static_cast<ecs_inout_kind_t>(flecs::In);
        m_term->subj.set.mask = flecs::Nothing;
        return *this;
    }

    /** Scheduler annotation to indicate sytem uses get_mut<T> */
    Base& read_write() {
        ecs_assert(m_term != nullptr, ECS_INVALID_PARAMETER, NULL);
        m_term->inout = static_cast<ecs_inout_kind_t>(flecs::InOut);
        m_term->subj.set.mask = flecs::Nothing;
        return *this;
    }

    /** Set operator of term. */
    Base& oper(flecs::oper_kind_t oper) {
        ecs_assert(m_term != nullptr, ECS_INVALID_PARAMETER, NULL);
        m_term->oper = static_cast<ecs_oper_kind_t>(oper);
        return *this;
    }

    /** Make term a singleton. */
    Base& singleton() {
        ecs_assert(m_term != nullptr, ECS_INVALID_PARAMETER, NULL);
        ecs_assert(m_term->id || m_term->pred.entity, ECS_INVALID_PARAMETER, NULL);
        
        flecs::id_t pred = m_term->id;
        if (!pred) {
            pred = m_term->pred.entity;
        }

        ecs_assert(pred != 0, ECS_INVALID_PARAMETER, NULL);

        m_term->subj.entity = pred;

        return *this;
    }

    flecs::id id() {
        return flecs::id(this->world_v(), m_term->id);
    }

    ecs_term_t *m_term;

protected:
    virtual flecs::world_t* world_v() = 0;

    void set_term(ecs_term_t *term) {
        m_term = term;
        if (term) {
            this->m_term_id = &m_term->subj; // default to subject
        } else {
            this->m_term_id = nullptr;
        }
    }

private:
    operator Base&() {
        return *static_cast<Base*>(this);
    }   
};

}


namespace flecs {

// Class that describes a term
struct term final : term_builder_i<term> {
    term(flecs::world_t *world_ptr) 
        : term_builder_i<term>(&value)
        , value({})
        , m_world(world_ptr) { value.move = true; }

    term(id_t id) 
        : term_builder_i<term>(&value)
        , value({})
        , m_world(nullptr) { 
            value.move = true; 
            this->id(id);
        }

    term(flecs::world_t *world_ptr, id_t id) 
        : term_builder_i<term>(&value)
        , value({})
        , m_world(world_ptr) { 
            value.move = true; 
            this->id(id);
        }

    term(flecs::world_t *world_ptr, ecs_term_t t)
        : term_builder_i<term>(&value)
        , value({})
        , m_world(world_ptr) {
            value = t;
            value.move = false;
            this->set_term(&value);
        }

    term(flecs::world_t *world_ptr, id_t r, id_t o) 
        : term_builder_i<term>(&value)
        , value({})
        , m_world(world_ptr) { 
            value.move = true; 
            this->id(r, o);
        }

    term(flecs::world_t *world_ptr, const char *expr) 
        : term_builder_i<term>(&value)
        , value({})
        , m_world(world_ptr)
    {
        this->expr(expr);
    }

    term(const term& t) : term_builder_i<term>(&value) {
        m_world = t.m_world;
        value = ecs_term_copy(&t.value);
        this->set_term(&value);
    }

    term(term&& t) : term_builder_i<term>(&value) {
        m_world = t.m_world;
        value = ecs_term_move(&t.value);
        t.reset();
        this->set_term(&value);
    }

    term& operator=(const term& t) {
        ecs_assert(m_world == t.m_world, ECS_INVALID_PARAMETER, NULL);
        ecs_term_fini(&value);
        value = ecs_term_copy(&t.value);
        this->set_term(&value);
        return *this;
    }

    term& operator=(term&& t) {
        ecs_assert(m_world == t.m_world, ECS_INVALID_PARAMETER, NULL);
        ecs_term_fini(&value);
        value = t.value;
        this->set_term(&value);
        t.reset();
        return *this;
    }   

    ~term() {
        ecs_term_fini(&value);
    }

    void reset() {
        value = {};
        this->set_term(nullptr);
    }

    int finalize() {
        return ecs_term_finalize(m_world, nullptr, &value);
    }

    bool is_set() {
        return ecs_term_is_initialized(&value);
    }

    bool is_trivial() {
        return ecs_term_is_trivial(&value);
    }

    flecs::inout_kind_t inout() {
        return static_cast<flecs::inout_kind_t>(value.inout);
    }

    flecs::oper_kind_t oper() {
        return static_cast<flecs::oper_kind_t>(value.oper);
    }

    flecs::entity get_subject() {
        return flecs::entity(world(), value.subj.entity);
    }

    flecs::entity get_object() {
        return flecs::entity(world(), value.obj.entity);
    }

    ecs_term_t move() { /* explicit move to ecs_term_t */
        return ecs_term_move(&value);
    }

    ecs_term_t value;

protected:
    flecs::world_t* world_v() override { return m_world; }

private:
    flecs::world_t *m_world;
};

// Term mixin implementation
template <typename... Args>
inline flecs::term world::term(Args &&... args) const {
    return flecs::term(m_world, FLECS_FWD(args)...);
}

template <typename T, typename... Args>
inline flecs::term world::term(Args &&... args) const {
    return flecs::term(m_world, FLECS_FWD(args)...).id<T>();
}

template <typename R, typename O, typename... Args>
inline flecs::term world::term(Args &&... args) const {
    return flecs::term(m_world, FLECS_FWD(args)...).id<R, O>();
}

// Builder implementation
template<typename Base>
inline Base& term_builder_i<Base>::id(const flecs::type& type) {
    ecs_assert(m_term != nullptr, ECS_INVALID_PARAMETER, NULL);
    m_term->pred.entity = type.id();
    return *this;
}

template<typename Base>
inline Base& term_builder_i<Base>::id(const flecs::type& type, id_t o) {
    ecs_assert(m_term != nullptr, ECS_INVALID_PARAMETER, NULL);
    m_term->pred.entity = type.id();
    m_term->obj.entity = o;
    m_term->role = ECS_PAIR;
    return *this;
}

}

#pragma once

#pragma once

#pragma once

namespace flecs {
namespace _ {

// Macros for template types so we don't go cross-eyed
#define FLECS_TBUILDER template<typename ... Components> class
#define FLECS_IBUILDER template<typename IBase, typename ... Components> class

template<FLECS_TBUILDER T, typename TDesc, typename Base, FLECS_IBUILDER IBuilder, typename ... Components>
struct builder : IBuilder<Base, Components ...>
{
    using IBase = IBuilder<Base, Components ...>;

public:
    builder(flecs::world_t *world)
        : IBase(&m_desc)
        , m_desc{}
        , m_world(world) { }

    builder(const builder& f) 
        : IBase(&m_desc, f.m_term_index)
    {
        m_world = f.m_world;
        m_desc = f.m_desc;
    }

    builder(builder&& f) 
        : builder<T, TDesc, Base, IBuilder, Components...>(f) { }

    operator TDesc*() {
        return &m_desc;
    }

    T<Components ...> build() {
        return T<Components...>(m_world, *static_cast<Base*>(this));
    }

protected:
    flecs::world_t* world_v() override { return m_world; }
    TDesc m_desc;
    flecs::world_t *m_world;
};

#undef FLECS_TBUILDER
#undef FLECS_IBUILDER

} // namespace _
} // namespace flecs

#pragma once


namespace flecs 
{

// Filter builder interface
template<typename Base, typename ... Components>
struct filter_builder_i : term_builder_i<Base> {
    filter_builder_i(ecs_filter_desc_t *desc, int32_t term_index = 0) 
        : m_term_index(term_index)
        , m_expr_count(0)
        , m_desc(desc) { }

    Base& instanced() {
        m_desc->instanced = true;
        return *this;
    }

    Base& expr(const char *expr) {
        ecs_check(m_expr_count == 0, ECS_INVALID_OPERATION,
            "filter_builder::expr() called more than once");
        m_desc->expr = expr;
        m_expr_count ++;

    error:
        return *this;
    }

    Base& term() {
        if (this->m_term) {
            ecs_check(ecs_term_is_initialized(this->m_term), 
                ECS_INVALID_OPERATION, 
                    "filter_builder::term() called without initializing term");
        }

        if (m_term_index >= ECS_TERM_DESC_CACHE_SIZE) {
            if (m_term_index == ECS_TERM_DESC_CACHE_SIZE) {
                m_desc->terms_buffer = ecs_os_calloc_n(
                    ecs_term_t, m_term_index + 1);
                ecs_os_memcpy_n(m_desc->terms_buffer, m_desc->terms, 
                    ecs_term_t, m_term_index);
                ecs_os_memset_n(m_desc->terms, 0, 
                    ecs_term_t, ECS_TERM_DESC_CACHE_SIZE);
            } else {
                m_desc->terms_buffer = ecs_os_realloc_n(m_desc->terms_buffer, 
                    ecs_term_t, m_term_index + 1);
            }

            m_desc->terms_buffer_count = m_term_index + 1;

            this->set_term(&m_desc->terms_buffer[m_term_index]);
        } else {
            this->set_term(&m_desc->terms[m_term_index]);
        }

        m_term_index ++;
    
    error:
        return *this;
    }
    
    Base& arg(int32_t term_index) {
        ecs_assert(term_index > 0, ECS_INVALID_PARAMETER, NULL);
        int32_t prev_index = m_term_index;
        m_term_index = term_index - 1;
        this->term();
        m_term_index = prev_index;
        ecs_assert(ecs_term_is_initialized(this->m_term), 
            ECS_INVALID_PARAMETER, NULL);
        return *this;
    }    

    template<typename T>
    Base& term() {
        this->term();
        *this->m_term = flecs::term(this->world_v()).id<T>().move();
        this->m_term->inout = static_cast<ecs_inout_kind_t>(_::type_to_inout<T>());
        return *this;
    }

    Base& term(id_t id) {
        this->term();
        *this->m_term = flecs::term(id).move();
        return *this;
    }

    template <typename E, if_t< is_enum<E>::value > = 0>
    Base& term(E value) {
        flecs::entity_t r = _::cpp_type<E>::id(this->world_v());
        auto o = enum_type<E>(this->world_v()).entity(value);
        return term(r, o);
    }

    template<typename R, typename O>
    Base& term() {
        this->term();
        *this->m_term = flecs::term(this->world_v()).id<R, O>().move();
        return *this;
    }

    template<typename R>
    Base& term(id_t o) {
        this->term();
        *this->m_term = flecs::term(this->world_v()).id<R>(o).move();
        return *this;
    }     

    Base& term(id_t r, id_t o) {
        this->term();
        *this->m_term = flecs::term(this->world_v()).id(r, o).move();
        return *this;
    }

    Base& term(const flecs::type& type) {
        this->term();
        *this->m_term = flecs::term(this->world_v()).id(type).move();
        return *this;
    }

    Base& term(const flecs::type& type, flecs::id_t o) {
        this->term();
        *this->m_term = flecs::term(this->world_v()).id(type, o).move();
        return *this;
    }

    Base& term(const char *expr) {
        this->term();
        *this->m_term = flecs::term(this->world_v(), expr).move();
        this->m_term->move = true;
        return *this;
    }

    Base& term(flecs::term& term) {
        this->term();
        *this->m_term = term.move();
        return *this;
    }

    Base& term(flecs::term&& term) {
        this->term();
        *this->m_term = term.move();
        return *this;
    }

protected:
    virtual flecs::world_t* world_v() = 0;
    int32_t m_term_index;
    int32_t m_expr_count;

private:
    operator Base&() {
        return *static_cast<Base*>(this);
    }

    ecs_filter_desc_t *m_desc;
};

}


namespace flecs {
namespace _ {
    template <typename ... Components>
    using filter_builder_base = builder<
        filter, ecs_filter_desc_t, filter_builder<Components...>, 
        filter_builder_i, Components ...>;
}

template <typename ... Components>
struct filter_builder final : _::filter_builder_base<Components...> {
    filter_builder(flecs::world_t* world)
        : _::filter_builder_base<Components...>(world)
    {
        _::sig<Components...>(world).populate(this);
    }
};

}


namespace flecs 
{

struct filter_base {
    filter_base()
        : m_world(nullptr)
        , m_filter({})
        , m_filter_ptr(nullptr) { }

    filter_base(world_t *world, const ecs_filter_t *filter)
        : m_world(world)
        , m_filter({})
        , m_filter_ptr(filter) { }

    filter_base(world_t *world, ecs_filter_t *filter)
        : m_world(world)
        , m_filter_ptr(&m_filter) {
            ecs_filter_move(&m_filter, filter);
        }

    filter_base(world_t *world, ecs_filter_desc_t *desc) 
        : m_world(world)
    {
        int res = ecs_filter_init(world, &m_filter, desc);
        if (res != 0) {
            ecs_abort(ECS_INVALID_PARAMETER, NULL);
        }

        if (desc->terms_buffer) {
            ecs_os_free(desc->terms_buffer);
        }

        m_filter_ptr = &m_filter;
    }

    filter_base(const filter_base& obj) {
        this->m_world = obj.m_world;
        if (obj.m_filter_ptr) {
            this->m_filter_ptr = &this->m_filter;
        } else {
            this->m_filter_ptr = nullptr;
        }
        ecs_filter_copy(&m_filter, &obj.m_filter);
    }

    filter_base& operator=(const filter_base& obj) {
        this->m_world = obj.m_world;
        if (obj.m_filter_ptr) {
            this->m_filter_ptr = &this->m_filter;
        } else {
            this->m_filter_ptr = nullptr;
        }
        ecs_filter_copy(&m_filter, &obj.m_filter);
        return *this; 
    }

    filter_base(filter_base&& obj) {
        this->m_world = obj.m_world;
        if (obj.m_filter_ptr) {
            this->m_filter_ptr = &this->m_filter;
        } else {
            this->m_filter_ptr = nullptr;
        }
        ecs_filter_move(&m_filter, &obj.m_filter);
    }

    filter_base& operator=(filter_base&& obj) {
        this->m_world = obj.m_world;
        if (obj.m_filter_ptr) {
            this->m_filter_ptr = &this->m_filter;
        } else {
            this->m_filter_ptr = nullptr;
        }
        ecs_filter_move(&m_filter, &obj.m_filter);
        return *this; 
    }

    operator const flecs::filter_t*() const {
        return m_filter_ptr;
    }

    /** Free the filter.
     */
    ~filter_base() {
        if ((&m_filter == m_filter_ptr) && m_filter_ptr) {
            ecs_filter_fini(&m_filter);
        }
    }

    template <typename Func>
    void each_term(const Func& func) {
        for (int i = 0; i < m_filter_ptr->term_count; i ++) {
            flecs::term t(m_world, m_filter_ptr->terms[i]);
            func(t);
        }
    }

    flecs::term term(int32_t index) {
        return flecs::term(m_world, m_filter_ptr->terms[index]);
    }

    int32_t term_count() {
        return m_filter_ptr->term_count;
    }

    flecs::string str() {
        char *result = ecs_filter_str(m_world, m_filter_ptr);
        return flecs::string(result);
    }

    operator filter<>() const;

protected:
    world_t *m_world;
    filter_t m_filter;
    const filter_t *m_filter_ptr;
};

template<typename ... Components>
struct filter : filter_base, iterable<Components...> {
private:
    using Terms = typename _::term_ptrs<Components...>::array;

public:
    using filter_base::filter_base;

    filter() : filter_base() { } // necessary not not confuse msvc

    filter(const filter& obj) : filter_base(obj) { }

    filter& operator=(const filter& obj) {
        filter_base::operator=(obj);
        return *this;
    }

    filter(filter&& obj) : filter_base(FLECS_MOV(obj)) { }

    filter& operator=(filter&& obj) {
        filter_base(FLECS_MOV(obj));
        return *this;
    }

private:
    ecs_iter_t get_iter() const override {
        return ecs_filter_iter(m_world, m_filter_ptr);
    }

    ecs_iter_next_action_t next_action() const override {
        return ecs_filter_next;
    }

    ecs_iter_next_action_t next_each_action() const override {
        return ecs_filter_next_instanced;
    }
};

// World mixin implementation
template <typename... Comps, typename... Args>
inline flecs::filter<Comps...> world::filter(Args &&... args) const {
    return flecs::filter_builder<Comps...>(m_world, FLECS_FWD(args)...)
        .build();
}

template <typename... Comps, typename... Args>
inline flecs::filter_builder<Comps...> world::filter_builder(Args &&... args) const {
    return flecs::filter_builder<Comps...>(m_world, FLECS_FWD(args)...);
}

// world::each
namespace _ {

// Each with entity parameter
template<typename Func, typename ... Args>
struct filter_invoker_w_ent;

template<typename Func, typename E, typename ... Args>
struct filter_invoker_w_ent<Func, arg_list<E, Args ...> >
{
    filter_invoker_w_ent(const flecs::world& world, Func&& func) {
        auto f = world.filter<Args ...>();
        f.each(FLECS_MOV(func));
    }
};

// Each without entity parameter
template<typename Func, typename ... Args>
struct filter_invoker_no_ent;

template<typename Func, typename ... Args>
struct filter_invoker_no_ent<Func, arg_list<Args ...> >
{
    filter_invoker_no_ent(const flecs::world& world, Func&& func) {
        auto f = world.filter<Args ...>();
        f.each(FLECS_MOV(func));
    }
};

// Switch between function with & without entity parameter
template<typename Func, typename T = int>
struct filter_invoker;

template <typename Func>
struct filter_invoker<Func, if_t<is_same<first_arg_t<Func>, flecs::entity>::value> > {
    filter_invoker(const flecs::world& world, Func&& func) {
        filter_invoker_w_ent<Func, arg_list_t<Func>>(world, FLECS_MOV(func));
    }
};

template <typename Func>
struct filter_invoker<Func, if_not_t<is_same<first_arg_t<Func>, flecs::entity>::value> > {
    filter_invoker(const flecs::world& world, Func&& func) {
        filter_invoker_no_ent<Func, arg_list_t<Func>>(world, FLECS_MOV(func));
    }
};

}

template <typename Func>
inline void world::each(Func&& func) const {
    _::filter_invoker<Func> f_invoker(*this, FLECS_MOV(func));
}

template <typename T, typename Func>
inline void world::each(Func&& func) const {
    ecs_term_t t = {};
    t.id = _::cpp_type<T>::id();
    ecs_iter_t it = ecs_term_iter(m_world, &t);

    while (ecs_term_next(&it)) {
        _::each_invoker<Func, T>(func).invoke(&it);
    }
}

template <typename Func>
inline void world::each(flecs::id_t term_id, Func&& func) const {
    ecs_term_t t = {};
    t.id = term_id;
    ecs_iter_t it = ecs_term_iter(m_world, &t);

    while (ecs_term_next(&it)) {
        _::each_invoker<Func>(func).invoke(&it);
    }
}

// filter_base implementation
inline filter_base::operator flecs::filter<> () const {
    flecs::filter<> f;
    ecs_filter_copy(&f.m_filter, &this->m_filter);
    f.m_filter_ptr = &f.m_filter;
    f.m_world = this->m_world;
    return f;
}

}

#pragma once


namespace flecs 
{

// Mixin implementation

inline flecs::event_builder world::event(flecs::entity_t evt) const {
    return flecs::event_builder(m_world, evt);
}

template <typename E>
inline flecs::event_builder_typed<E> world::event() const {
    return flecs::event_builder_typed<E>(m_world, _::cpp_type<E>().id(m_world));
}

} // namespace flecs

#pragma once

#pragma once

#pragma once


namespace flecs {

// Query builder interface
template<typename Base, typename ... Components>
struct query_builder_i : filter_builder_i<Base, Components ...> {
private:
    using BaseClass = filter_builder_i<Base, Components ...>;
    
public:
    query_builder_i()
        : BaseClass(nullptr)
        , m_desc(nullptr) { }

    query_builder_i(ecs_query_desc_t *desc, int32_t term_index = 0) 
        : BaseClass(&desc->filter, term_index)
        , m_desc(desc) { }

    /** Sort the output of a query.
     * This enables sorting of entities across matched tables. As a result of this
     * operation, the order of entities in the matched tables may be changed. 
     * Resorting happens when a query iterator is obtained, and only if the table
     * data has changed.
     *
     * If multiple queries that match the same (sub)set of tables specify different 
     * sorting functions, resorting is likely to happen every time an iterator is
     * obtained, which can significantly slow down iterations.
     *
     * The sorting function will be applied to the specified component. Resorting
     * only happens if that component has changed, or when the entity order in the
     * table has changed. If no component is provided, resorting only happens when
     * the entity order changes.
     *
     * @tparam T The component used to sort.
     * @param compare The compare function used to sort the components.
     */      
    template <typename T>
    Base& order_by(int(*compare)(flecs::entity_t, const T*, flecs::entity_t, const T*)) {
        ecs_order_by_action_t cmp = reinterpret_cast<ecs_order_by_action_t>(compare);
        return this->order_by(_::cpp_type<T>::id(this->world_v()), cmp);
    }

    /** Sort the output of a query.
     * Same as order_by<T>, but with component identifier.
     *
     * @param component The component used to sort.
     * @param compare The compare function used to sort the components.
     */    
    Base& order_by(flecs::entity_t component, int(*compare)(flecs::entity_t, const void*, flecs::entity_t, const void*)) {
        m_desc->order_by = reinterpret_cast<ecs_order_by_action_t>(compare);
        m_desc->order_by_component = component;
        return *this;
    }

    /** Group and sort matched tables.
     * Similar yo ecs_query_order_by, but instead of sorting individual entities, this
     * operation only sorts matched tables. This can be useful of a query needs to
     * enforce a certain iteration order upon the tables it is iterating, for 
     * example by giving a certain component or tag a higher priority.
     *
     * The sorting function assigns a "rank" to each type, which is then used to
     * sort the tables. Tables with higher ranks will appear later in the iteration.
     * 
     * Resorting happens when a query iterator is obtained, and only if the set of
     * matched tables for a query has changed. If table sorting is enabled together
     * with entity sorting, table sorting takes precedence, and entities will be
     * sorted within each set of tables that are assigned the same rank.
     *
     * @tparam T The component used to determine the group rank.
     * @param rank The rank action.
     */
    template <typename T>
    Base& group_by(uint64_t(*rank)(flecs::world_t*, flecs::table_t *table, flecs::id_t id, void* ctx)) {
        ecs_group_by_action_t rnk = reinterpret_cast<ecs_group_by_action_t>(rank);
        return this->group_by(_::cpp_type<T>::id(this->world_v()), rnk);
    }

    /** Group and sort matched tables.
     * Same as group_by<T>, but with component identifier.
     *
     * @param component The component used to determine the group rank.
     * @param rank The rank action.
     */
    Base& group_by(flecs::entity_t component, uint64_t(*rank)(flecs::world_t*, flecs::table_t *table, flecs::id_t id, void* ctx)) {
        m_desc->group_by = reinterpret_cast<ecs_group_by_action_t>(rank);
        m_desc->group_by_id = component;
        return *this;
    } 

    /** Specify parent query (creates subquery) */
    Base& parent(const query_base& parent);
    
protected:
    virtual flecs::world_t* world_v() = 0;

private:
    operator Base&() {
        return *static_cast<Base*>(this);
    }

    ecs_query_desc_t *m_desc;
};

}


namespace flecs {
namespace _ {
    template <typename ... Components>
    using query_builder_base = builder<
        query, ecs_query_desc_t, query_builder<Components...>, 
        query_builder_i, Components ...>;
}

template <typename ... Components>
struct query_builder final : _::query_builder_base<Components...> {
    query_builder(flecs::world_t* world)
        : _::query_builder_base<Components...>(world)
    {
        _::sig<Components...>(world).populate(this);
    }
};

}


namespace flecs {

////////////////////////////////////////////////////////////////////////////////
//// Persistent queries
////////////////////////////////////////////////////////////////////////////////

struct query_base {
    query_base()
        : m_world(nullptr)
        , m_query(nullptr) { }    
    
    query_base(world_t *world, query_t *query = nullptr)
        : m_world(world)
        , m_query(query) { }

    query_base(world_t *world, ecs_query_desc_t *desc) 
        : m_world(world)
    {
        m_query = ecs_query_init(world, desc);

        if (!m_query) {
            ecs_abort(ECS_INVALID_PARAMETER, NULL);
        }

        if (desc->filter.terms_buffer) {
            ecs_os_free(desc->filter.terms_buffer);
        }
    }

    operator query_t*() const {
        return m_query;
    }

    /** Returns whether the query data changed since the last iteration.
     * This operation must be invoked before obtaining the iterator, as this will
     * reset the changed state. The operation will return true after:
     * - new entities have been matched with
     * - matched entities were deleted
     * - matched components were changed
     * 
     * @return true if entities changed, otherwise false.
     */
    bool changed() {
        return ecs_query_changed(m_query, 0);
    }

    /** Returns whether query is orphaned.
     * When the parent query of a subquery is deleted, it is left in an orphaned
     * state. The only valid operation on an orphaned query is deleting it. Only
     * subqueries can be orphaned.
     *
     * @return true if query is orphaned, otherwise false.
     */
    bool orphaned() {
        return ecs_query_orphaned(m_query);
    }

    /** Free the query.
     */
    void destruct() {
        ecs_query_fini(m_query);
        m_world = nullptr;
        m_query = nullptr;
    }

    template <typename Func>
    void each_term(const Func& func) {
        const ecs_filter_t *f = ecs_query_get_filter(m_query);
        ecs_assert(f != NULL, ECS_INVALID_PARAMETER, NULL);

        for (int i = 0; i < f->term_count; i ++) {
            flecs::term t(m_world, f->terms[i]);
            func(t);
        }
    }

    filter_base filter() {
        return filter_base(m_world, ecs_query_get_filter(m_query));
    }

    flecs::term term(int32_t index) {
        const ecs_filter_t *f = ecs_query_get_filter(m_query);
        ecs_assert(f != NULL, ECS_INVALID_PARAMETER, NULL);
        return flecs::term(m_world, f->terms[index]);
    }

    int32_t term_count() {
        const ecs_filter_t *f = ecs_query_get_filter(m_query);
        return f->term_count;   
    }

    flecs::string str() {
        const ecs_filter_t *f = ecs_query_get_filter(m_query);
        char *result = ecs_filter_str(m_world, f);
        return flecs::string(result);
    }
    
    operator query<>() const;

protected:
    world_t *m_world;
    query_t *m_query;
};

template<typename ... Components>
struct query final : query_base, iterable<Components...> {
public:
    flecs::world world() const {
        return flecs::world(m_world);
    }
    
private:
    using Terms = typename _::term_ptrs<Components...>::array;

    ecs_iter_t get_iter() const override {
        return ecs_query_iter(m_world, m_query);
    }

    ecs_iter_next_action_t next_action() const override {
        return ecs_query_next;
    }

    ecs_iter_next_action_t next_each_action() const override {
        return ecs_query_next_instanced;
    }

public:
    using query_base::query_base;
};

// Mixin implementation
template <typename... Comps, typename... Args>
inline flecs::query<Comps...> world::query(Args &&... args) const {
    return flecs::query_builder<Comps...>(m_world, FLECS_FWD(args)...)
        .build();
}

template <typename... Comps, typename... Args>
inline flecs::query_builder<Comps...> world::query_builder(Args &&... args) const {
    return flecs::query_builder<Comps...>(m_world, FLECS_FWD(args)...);
}

// Builder implementation
template <typename Base, typename ... Components>
inline Base& query_builder_i<Base, Components ...>::parent(const query_base& parent) {
    m_desc->parent = parent;
    return *static_cast<Base*>(this);
}

// query_base implementation
inline query_base::operator query<>() const {
    return flecs::query<>(m_world, m_query);
}

} // namespace flecs

#pragma once

#pragma once

#pragma once

namespace flecs {
namespace _ {

// Macros for template types so we don't go cross-eyed
#define FLECS_IBUILDER template<typename IBase, typename ... Components> class

template<typename T, typename TDesc, typename Base, FLECS_IBUILDER IBuilder, typename ... Components>
struct node_builder : IBuilder<Base, Components ...>
{
    using IBase = IBuilder<Base, Components ...>;

public:
    explicit node_builder(flecs::world_t* world, const char *name = nullptr)
        : IBase(&m_desc)
        , m_desc{}
        , m_world(world)
        , m_instanced(false)
    {
        m_desc.entity.name = name;
        m_desc.entity.sep = "::";
    }

    /* Iter (or each) is mandatory and always the last thing that 
     * is added in the fluent method chain. Create system signature from both 
     * template parameters and anything provided by the signature method. */
    template <typename Func>
    T iter(Func&& func) {
        using Invoker = typename _::iter_invoker<
            typename std::decay<Func>::type, Components...>;
        return build<Invoker>(FLECS_FWD(func));
    }

    /* Each is similar to action, but accepts a function that operates on a
     * single entity */
    template <typename Func>
    T each(Func&& func) {
        using Invoker = typename _::each_invoker<
            typename std::decay<Func>::type, Components...>;
        m_instanced = true;
        return build<Invoker>(FLECS_FWD(func));
    }

protected:
    flecs::world_t* world_v() override { return m_world; }
    TDesc m_desc;
    flecs::world_t *m_world;
    bool m_instanced;

private:
    template <typename Invoker, typename Func>
    T build(Func&& func) {
        auto ctx = FLECS_NEW(Invoker)(FLECS_FWD(func));

        m_desc.callback = Invoker::run;
        m_desc.binding_ctx = ctx;
        m_desc.binding_ctx_free = reinterpret_cast<
            ecs_ctx_free_t>(_::free_obj<Invoker>);
        
        return T(m_world, &m_desc);
    }
};

#undef FLECS_IBUILDER

} // namespace _
} // namespace flecs

#pragma once


namespace flecs 
{

// Trigger builder interface
template<typename Base, typename ... Components>
struct trigger_builder_i : term_builder_i<Base> {
    using Class = trigger_builder_i<Base, Components...>;
    using BaseClass = term_builder_i<Base>;

    trigger_builder_i(ecs_trigger_desc_t *desc) 
        : BaseClass(&desc->term)
        , m_desc(desc)
        , m_event_count(0) { }

    /** Specify the event(s) for when the trigger should run.
     * @param evt The event.
     */
    Base& event(entity_t evt) {
        m_desc->events[m_event_count ++] = evt;
        return *this;
    }

    /** Specify the event(s) for when the trigger should run.
     * @tparam E The event.
     */
    template <typename E>
    Base& event() {
        m_desc->events[m_event_count ++] = _::cpp_type<E>().id(world_v());
        return *this;
    }

    /** Invoke observer for anything that matches its filter on creation */
    Base& yield_existing(bool value = true) {
        m_desc->yield_existing = value;
        return *this;
    }

    /** Associate trigger with entity */
    Base& self(flecs::entity self) {
        m_desc->self = self;
        return *this;
    }

    /** Set system context */
    Base& ctx(void *ptr) {
        m_desc->ctx = ptr;
        return *this;
    }

protected:
    virtual flecs::world_t* world_v() = 0;

private:
    operator Base&() {
        return *static_cast<Base*>(this);
    }

    ecs_trigger_desc_t *m_desc;
    int32_t m_event_count;
};

}


namespace flecs {
namespace _ {
    template <typename ... Components>
    using trigger_builder_base = node_builder<
        trigger, 
        ecs_trigger_desc_t, 
        trigger_builder<Components...>, 
        trigger_builder_i, 
        Components ...>;
}

template <typename ... Components>
struct trigger_builder final : _::trigger_builder_base<Components...> {
    trigger_builder(flecs::world_t* world, const char *name = nullptr)
        : _::trigger_builder_base<Components...>(world, name)
    {
        _::sig<Components...>(world).populate(this);
    }
};

}


namespace flecs 
{

struct trigger final : entity
{
    using entity::entity;

    explicit trigger() : entity() { }

    trigger(flecs::world_t *world, ecs_trigger_desc_t *desc) 
        : entity(world, ecs_trigger_init(world, desc)) 
    { 
        ecs_term_fini(&desc->term);
    }
    
    void ctx(void *ctx) {
        ecs_trigger_desc_t desc = {};
        desc.entity.entity = m_id;
        desc.ctx = ctx;
        ecs_trigger_init(m_world, &desc);
    }

    void* ctx() const {
        return ecs_get_trigger_ctx(m_world, m_id);
    }
};

// Mixin implementation
inline trigger world::trigger(flecs::entity e) const {
    return flecs::trigger(m_world, e);
}

template <typename... Comps, typename... Args>
inline trigger_builder<Comps...> world::trigger(Args &&... args) const {
    return flecs::trigger_builder<Comps...>(m_world, FLECS_FWD(args)...);
}

} // namespace flecs

#pragma once

#pragma once

#pragma once


namespace flecs {

// Observer builder interface
template<typename Base, typename ... Components>
struct observer_builder_i : filter_builder_i<Base, Components ...> {
    using BaseClass = filter_builder_i<Base, Components ...>;
    observer_builder_i()
        : BaseClass(nullptr)
        , m_desc(nullptr)
        , m_event_count(0) { }

    observer_builder_i(ecs_observer_desc_t *desc) 
        : BaseClass(&desc->filter)
        , m_desc(desc)
        , m_event_count(0) { }

    /** Specify the event(s) for when the observer should run.
     * @param evt The event.
     */
    Base& event(entity_t evt) {
        m_desc->events[m_event_count ++] = evt;
        return *this;
    }

    /** Specify the event(s) for when the observer should run.
     * @tparam E The event.
     */
    template <typename E>
    Base& event() {
        m_desc->events[m_event_count ++] = _::cpp_type<E>().id(world_v());
        return *this;
    }

    /** Invoke observer for anything that matches its filter on creation */
    Base& yield_existing(bool value = true) {
        m_desc->yield_existing = value;
        return *this;
    }

    /** Associate observer with entity */
    Base& self(flecs::entity self) {
        m_desc->self = self;
        return *this;
    }

    /** Set system context */
    Base& ctx(void *ptr) {
        m_desc->ctx = ptr;
        return *this;
    }    

protected:
    virtual flecs::world_t* world_v() = 0;

private:
    operator Base&() {
        return *static_cast<Base*>(this);
    }

    ecs_observer_desc_t *m_desc;
    int32_t m_event_count;
};

}


namespace flecs {
namespace _ {
    template <typename ... Components>
    using observer_builder_base = node_builder<
        observer, ecs_observer_desc_t, observer_builder<Components...>, 
        observer_builder_i, Components ...>;
}

template <typename ... Components>
struct observer_builder final : _::observer_builder_base<Components...> {
    observer_builder(flecs::world_t* world, const char *name = nullptr)
        : _::observer_builder_base<Components...>(world, name)
    {
        _::sig<Components...>(world).populate(this);
    }
};

}


namespace flecs 
{

struct observer final : entity
{
    using entity::entity;

    explicit observer() : entity() { }

    observer(flecs::world_t *world, ecs_observer_desc_t *desc) 
        : entity(world, ecs_observer_init(world, desc)) 
    { 
        if (desc->filter.terms_buffer) {
            ecs_os_free(desc->filter.terms_buffer);
        }
    }

    void ctx(void *ctx) {
        ecs_observer_desc_t desc = {};
        desc.entity.entity = m_id;
        desc.ctx = ctx;
        ecs_observer_init(m_world, &desc);
    }

    void* ctx() const {
        return ecs_get_observer_ctx(m_world, m_id);
    }
};

// Mixin implementation
inline observer world::observer(flecs::entity e) const {
    return flecs::observer(m_world, e);
}

template <typename... Comps, typename... Args>
inline observer_builder<Comps...> world::observer(Args &&... args) const {
    return flecs::observer_builder<Comps...>(m_world, FLECS_FWD(args)...);
}

} // namespace flecs

#ifdef FLECS_MODULE
#pragma once

namespace flecs {

template <typename T>
ecs_entity_t do_import(world& world, const char *symbol) {
    ecs_trace("import %s", _::type_name<T>());
    ecs_log_push();

    ecs_entity_t scope = ecs_get_scope(world);
    ecs_set_scope(world, 0);

    // Initialize module component type & don't allow it to be registered as a
    // tag, as this would prevent calling emplace()
    auto m_c = component<T>(world, nullptr, false);
    ecs_add_id(world, m_c, EcsModule);

    world.emplace<T>(world);

    ecs_set_scope(world, scope);

    // It should now be possible to lookup the module
    ecs_entity_t m = ecs_lookup_symbol(world, symbol, true);
    ecs_assert(m != 0, ECS_MODULE_UNDEFINED, symbol);
    ecs_assert(m == m_c, ECS_INTERNAL_ERROR, NULL);

    ecs_log_pop();     

    return m;
}

template <typename T>
flecs::entity import(world& world) {
    const char *symbol = _::symbol_name<T>();

    ecs_entity_t m = ecs_lookup_symbol(world, symbol, true);
    
    if (!_::cpp_type<T>::registered()) {

        /* Module is registered with world, initialize static data */
        if (m) {
            _::cpp_type<T>::init(world, m, false);
        
        /* Module is not yet registered, register it now */
        } else {
            m = do_import<T>(world, symbol);
        }

    /* Module has been registered, but could have been for another world. Import
     * if module hasn't been registered for this world. */
    } else if (!m) {
        m = do_import<T>(world, symbol);
    }

    return flecs::entity(world, m);
}

template <typename Module>
inline flecs::entity world::module(const char *name) const {
    flecs::id_t result = _::cpp_type<Module>::id(m_world);
    if (name) {
        ecs_add_path_w_sep(m_world, result, 0, name, "::", "::");
    }
    ecs_set_scope(m_world, result);
    return flecs::entity(m_world, result);
}

template <typename Module>
inline flecs::entity world::import() {
    return flecs::import<Module>(*this);
}

}

#endif
#ifdef FLECS_SYSTEM
#pragma once

#pragma once

#pragma once


namespace flecs 
{

// System builder interface
template<typename Base, typename ... Components>
struct system_builder_i : query_builder_i<Base, Components ...> {
private:
    using BaseClass = query_builder_i<Base, Components ...>;

public:
    system_builder_i(ecs_system_desc_t *desc) 
        : BaseClass(&desc->query)
        , m_desc(desc) { }

    /** Specify in which phase the system should run.
     *
     * @param phase The phase.
     */
    Base& kind(entity_t phase) {
        m_desc->entity.add[0] = phase;
        return *this;
    }

    /** Specify in which phase the system should run.
     *
     * @tparam Phase The phase.
     */
    template <typename Phase>
    Base& kind() {
        m_desc->entity.add[0] = _::cpp_type<Phase>::id(world_v());
        return *this;
    }

    /** Specify whether system can run on multiple threads.
     *
     * @param value If false system will always run on a single thread.
     */
    Base& multi_threaded(bool value = true) {
        m_desc->multi_threaded = value;
        return *this;
    }

    /** Specify whether system should be ran in staged context.
     *
     * @param value If false system will always run staged.
     */
    Base& no_staging(bool value = true) {
        m_desc->no_staging = value;
        return *this;
    }

    /** Set system interval.
     * This operation will cause the system to be ran at the specified interval.
     *
     * The timer is synchronous, and is incremented each frame by delta_time.
     *
     * @param interval The interval value.
     */
    Base& interval(FLECS_FLOAT interval) {
        m_desc->interval = interval;
        return *this;
    }

    /** Set system rate.
     * This operation will cause the system to be ran at a multiple of the 
     * provided tick source. The tick source may be any entity, including
     * another system.
     *
     * @param tick_source The tick source.
     * @param rate The multiple at which to run the system.
     */
    Base& rate(const entity_t tick_source, int32_t rate) {
        m_desc->rate = rate;
        m_desc->tick_source = tick_source;
        return *this;
    }

    /** Set system rate.
     * This operation will cause the system to be ran at a multiple of the 
     * frame tick frequency. If a tick source was provided, this just updates
     * the rate of the system.
     *
     * @param rate The multiple at which to run the system.
     */
    Base& rate(int32_t rate) {
        m_desc->rate = rate;
        return *this;
    }

    /** Associate system with entity */
    Base& self(flecs::entity self) {
        m_desc->self = self;
        return *this;
    }

    /** Set system context */
    Base& ctx(void *ptr) {
        m_desc->ctx = ptr;
        return *this;
    }

protected:
    virtual flecs::world_t* world_v() = 0;

private:
    operator Base&() {
        return *static_cast<Base*>(this);
    }

    ecs_system_desc_t *m_desc;
};

}


namespace flecs {
namespace _ {
    template <typename ... Components>
    using system_builder_base = node_builder<
        system, ecs_system_desc_t, system_builder<Components...>, 
        system_builder_i, Components ...>;
}

template <typename ... Components>
struct system_builder final : _::system_builder_base<Components...> {
    system_builder(flecs::world_t* world, const char *name = nullptr)
        : _::system_builder_base<Components...>(world, name)
    {
        _::sig<Components...>(world).populate(this);
        
        this->m_desc.query.filter.instanced = this->m_instanced;

#ifdef FLECS_PIPELINE
        this->m_desc.entity.add[0] = flecs::OnUpdate;
#endif
    }
};

}


namespace flecs 
{

struct system_runner_fluent {
    system_runner_fluent(
        world_t *world, 
        entity_t id, 
        int32_t stage_current, 
        int32_t stage_count, 
        FLECS_FLOAT delta_time, 
        void *param)
        : m_stage(world)
        , m_id(id)
        , m_delta_time(delta_time)
        , m_param(param)
        , m_offset(0)
        , m_limit(0)
        , m_stage_current(stage_current)
        , m_stage_count(stage_count) { }

    system_runner_fluent& offset(int32_t offset) {
        m_offset = offset;
        return *this;
    }

    system_runner_fluent& limit(int32_t limit) {
        m_limit = limit;
        return *this;
    }

    system_runner_fluent& stage(flecs::world& stage) {
        m_stage = stage.c_ptr();
        return *this;
    }

    ~system_runner_fluent() {
        if (m_stage_count) {
            ecs_run_worker(
                m_stage, m_id, m_stage_current, m_stage_count, m_delta_time,
                m_param);            
        } else {
            ecs_run_w_filter(
                m_stage, m_id, m_delta_time, m_offset, m_limit, m_param);
        }
    }

private:
    world_t *m_stage;
    entity_t m_id;
    FLECS_FLOAT m_delta_time;
    void *m_param;
    int32_t m_offset;
    int32_t m_limit;
    int32_t m_stage_current;
    int32_t m_stage_count;
};

struct system final : entity
{
    using entity::entity;

    explicit system() {
        m_id = 0;
        m_world = nullptr;
    }

    explicit system(flecs::world_t *world, ecs_system_desc_t *desc) 
        : entity(world, ecs_system_init(world, desc)) 
    {
        if (desc->query.filter.terms_buffer) {
            ecs_os_free(desc->query.filter.terms_buffer);
        }
    }

    void ctx(void *ctx) {
        ecs_system_desc_t desc = {};
        desc.entity.entity = m_id;
        desc.ctx = ctx;
        ecs_system_init(m_world, &desc);
    }

    void* ctx() const {
        return ecs_get_system_ctx(m_world, m_id);
    }

    flecs::query<> query() const {
        return flecs::query<>(m_world, ecs_system_get_query(m_world, m_id));
    }

    system_runner_fluent run(FLECS_FLOAT delta_time = 0.0f, void *param = nullptr) const {
        return system_runner_fluent(m_world, m_id, 0, 0, delta_time, param);
    }

    system_runner_fluent run_worker(
        int32_t stage_current, 
        int32_t stage_count, 
        FLECS_FLOAT delta_time = 0.0f, 
        void *param = nullptr) const 
    {
        return system_runner_fluent(
            m_world, m_id, stage_current, stage_count, delta_time, param);
    }

#   ifdef FLECS_TIMER

/** Set interval.
 * @see ecs_set_interval
 */
void interval(FLECS_FLOAT interval);

/** Get interval.
 * @see ecs_get_interval.
 */
FLECS_FLOAT interval();

/** Set timeout.
 * @see ecs_set_timeout
 */
void timeout(FLECS_FLOAT timeout);

/** Get timeout.
 * @see ecs_get_timeout
 */
FLECS_FLOAT timeout();

/** Set system rate (system is its own tick source).
 * @see ecs_set_rate
 */
void rate(int32_t rate);

/** Start timer.
 * @see ecs_start_timer
 */
void start();

/** Stop timer.
 * @see ecs_start_timer
 */
void stop();

/** Set external tick source.
 * @see ecs_set_tick_source
 */
void set_tick_source(flecs::entity e);

#   endif

};

// Mixin implementation
inline system world::system(flecs::entity e) const {
    return flecs::system(m_world, e);
}

template <typename... Comps, typename... Args>
inline system_builder<Comps...> world::system(Args &&... args) const {
    return flecs::system_builder<Comps...>(m_world, FLECS_FWD(args)...);
}

namespace _ {

inline void system_init(flecs::world& world) {
    world.component<TickSource>("flecs::system::TickSource");
}

} // namespace _
} // namespace flecs

#endif
#ifdef FLECS_PIPELINE
#pragma once

namespace flecs {

struct pipeline : type_base<pipeline> {
    explicit pipeline(world_t *world, entity_t e) : type_base(world, e)
    { 
        this->entity().add(flecs::Pipeline);
    }

    explicit pipeline(world_t *world, const char *name) : type_base(world, name)
    { 
        this->entity().add(flecs::Pipeline);
    }
};

template <typename... Args>
inline flecs::pipeline world::pipeline(Args &&... args) const {
    return flecs::pipeline(m_world, FLECS_FWD(args)...);
}

template <typename Pipeline, if_not_t< is_enum<Pipeline>::value >>
inline flecs::pipeline world::pipeline() const {
    return flecs::pipeline(m_world, _::cpp_type<Pipeline>::id(m_world));
}

inline void world::set_pipeline(const flecs::pipeline& pip) const {
    return ecs_set_pipeline(m_world, pip.id());
}

template <typename Pipeline>
inline void world::set_pipeline() const {
    return ecs_set_pipeline(m_world, _::cpp_type<Pipeline>::id(m_world));
}

inline flecs::pipeline world::get_pipeline() const {
    return flecs::pipeline(m_world, ecs_get_pipeline(m_world));
}

inline bool world::progress(FLECS_FLOAT delta_time) const {
    return ecs_progress(m_world, delta_time);
}

inline void world::run_pipeline(const flecs::pipeline& pip, FLECS_FLOAT delta_time) const {
    return ecs_run_pipeline(m_world, pip.id(), delta_time);
}

inline void world::set_time_scale(FLECS_FLOAT mul) const {
    ecs_set_time_scale(m_world, mul);
}  

inline FLECS_FLOAT world::get_time_scale() const {
    const ecs_world_info_t *stats = ecs_get_world_info(m_world);
    return stats->time_scale;
}

inline int32_t world::get_tick() const {
    const ecs_world_info_t *stats = ecs_get_world_info(m_world);
    return stats->frame_count_total;
}

inline FLECS_FLOAT world::get_target_fps() const {
    const ecs_world_info_t *stats = ecs_get_world_info(m_world);
    return stats->target_fps;
} 

inline void world::set_target_fps(FLECS_FLOAT target_fps) const {
    ecs_set_target_fps(m_world, target_fps);
}

inline void world::reset_clock() const {
    ecs_reset_clock(m_world);
}

inline void world::deactivate_systems() const {
    ecs_deactivate_systems(m_world);
}

inline void world::set_threads(int32_t threads) const {
    ecs_set_threads(m_world, threads);
}

inline int32_t world::get_threads() const {
    return ecs_get_threads(m_world);
}

}

#endif
#ifdef FLECS_TIMER
#pragma once

namespace flecs {

// Timer class
struct timer final : entity {
    template <typename ... Args>
    timer(Args&&... args) : entity(FLECS_FWD(args)...) { }
};

inline void system::interval(FLECS_FLOAT interval) {
    ecs_set_interval(m_world, m_id, interval);
}

inline FLECS_FLOAT system::interval() {
    return ecs_get_interval(m_world, m_id);
}

inline void system::timeout(FLECS_FLOAT timeout) {
    ecs_set_timeout(m_world, m_id, timeout);
}

inline FLECS_FLOAT system::timeout() {
    return ecs_get_timeout(m_world, m_id);
}

inline void system::rate(int32_t rate) {
    ecs_set_rate(m_world, m_id, rate, 0);
}

inline void system::start() {
    ecs_start_timer(m_world, m_id);
}

inline void system::stop() {
    ecs_stop_timer(m_world, m_id);
}

inline void system::set_tick_source(flecs::entity e) {
    ecs_set_tick_source(m_world, m_id, e);
}

namespace _ {

inline void timer_init(flecs::world& world) {
    world.component<RateFilter>("flecs::timer::RateFilter");
    world.component<Timer>("flecs::timer::Timer");
}

}
}

#endif
#ifdef FLECS_SNAPSHOT
#pragma once

namespace flecs {

struct snapshot final {
    explicit snapshot(const world& world)
        : m_world( world )
        , m_snapshot( nullptr ) { }

    snapshot(const snapshot& obj) 
        : m_world( obj.m_world )
    { 
        ecs_iter_t it = ecs_snapshot_iter(obj.m_snapshot);
        m_snapshot = ecs_snapshot_take_w_iter(&it);
    }

    snapshot(snapshot&& obj) 
        : m_world(obj.m_world)
        , m_snapshot(obj.m_snapshot)
    {
        obj.m_snapshot = nullptr;
    }

    snapshot& operator=(const snapshot& obj) {
        ecs_assert(m_world.c_ptr() == obj.m_world.c_ptr(), ECS_INVALID_PARAMETER, NULL);
        ecs_iter_t it = ecs_snapshot_iter(obj.m_snapshot);
        m_snapshot = ecs_snapshot_take_w_iter(&it);        
        return *this;
    }

    snapshot& operator=(snapshot&& obj) {
        ecs_assert(m_world.c_ptr() == obj.m_world.c_ptr(), ECS_INVALID_PARAMETER, NULL);
        m_snapshot = obj.m_snapshot;
        obj.m_snapshot = nullptr;
        return *this;
    }

    void take() {
        if (m_snapshot) {
            ecs_snapshot_free(m_snapshot);
        }

        m_snapshot = ecs_snapshot_take(m_world.c_ptr());
    }

    template <typename F>
    void take(const F& f) {
        if (m_snapshot) {
            ecs_snapshot_free(m_snapshot);
        }

        ecs_iter_t it = ecs_filter_iter(m_world, f.c_ptr());

        m_snapshot = ecs_snapshot_take_w_iter(&it);
    }    

    void restore() {
        if (m_snapshot) {
            ecs_snapshot_restore(m_world.c_ptr(), m_snapshot);
            m_snapshot = nullptr;
        }
    }

    ~snapshot() {
        if (m_snapshot) {
            ecs_snapshot_free(m_snapshot);
        }
    }

    snapshot_t* c_ptr() const {
        return m_snapshot;
    }

private:
    const world& m_world;
    snapshot_t *m_snapshot;
};

// Snapshot mixin implementation
template <typename... Args>
inline flecs::snapshot world::snapshot(Args &&... args) const {
    return flecs::snapshot(*this, FLECS_FWD(args)...);
}

}

#endif
#ifdef FLECS_DOC
#pragma once

namespace flecs {
namespace doc {

inline const char* get_name(const flecs::entity_view& e) {
    return ecs_doc_get_name(e.world(), e);
}

inline const char* get_brief(const flecs::entity_view& e) {
    return ecs_doc_get_brief(e.world(), e);
}

inline const char* get_detail(const flecs::entity_view& e) {
    return ecs_doc_get_detail(e.world(), e);
}

inline const char* get_link(const flecs::entity_view& e) {
    return ecs_doc_get_link(e.world(), e);
}

inline void set_name(flecs::entity& e, const char *name) {
    ecs_doc_set_name(e.world(), e, name);
}

inline void set_brief(flecs::entity& e, const char *description) {
    ecs_doc_set_brief(e.world(), e, description);
}

inline void set_detail(flecs::entity& e, const char *description) {
    ecs_doc_set_detail(e.world(), e, description);
}

inline void set_link(flecs::entity& e, const char *description) {
    ecs_doc_set_link(e.world(), e, description);
}

namespace _ {

inline void init(flecs::world& world) {
    world.component<doc::Description>("flecs::doc::Description");
}

} // namespace _
} // namespace doc
} // namespace flecs

#endif
#ifdef FLECS_DOC
#endif
#ifdef FLECS_REST
#pragma once

namespace flecs {
namespace rest {
namespace _ {

inline void init(flecs::world& world) {
    world.component<rest::Rest>("flecs::rest::Rest");
}
 
} // namespace _
} // namespace rest
} // namespace flecs

#endif
#ifdef FLECS_RULES
#pragma once

#pragma once


namespace flecs {
namespace _ {
    template <typename ... Components>
    using rule_builder_base = builder<
        rule, ecs_filter_desc_t, rule_builder<Components...>, 
        filter_builder_i, Components ...>;
}

template <typename ... Components>
struct rule_builder final : _::rule_builder_base<Components...> {
    rule_builder(flecs::world_t* world)
        : _::rule_builder_base<Components...>(world)
    {
        _::sig<Components...>(world).populate(this);
    }
};

}


namespace flecs {

////////////////////////////////////////////////////////////////////////////////
//// Persistent queries
////////////////////////////////////////////////////////////////////////////////

struct rule_base {
    rule_base()
        : m_world(nullptr)
        , m_rule(nullptr) { }    
    
    rule_base(world_t *world, rule_t *rule = nullptr)
        : m_world(world)
        , m_rule(rule) { }

    rule_base(world_t *world, ecs_filter_desc_t *desc) 
        : m_world(world)
    {
        m_rule = ecs_rule_init(world, desc);

        if (!m_rule) {
            ecs_abort(ECS_INVALID_PARAMETER, NULL);
        }

        if (desc->terms_buffer) {
            ecs_os_free(desc->terms_buffer);
        }
    }

    operator rule_t*() const {
        return m_rule;
    }

    /** Free the rule.
     */
    void destruct() {
        ecs_rule_fini(m_rule);
        m_world = nullptr;
        m_rule = nullptr;
    }

    flecs::string str() {
        const ecs_filter_t *f = ecs_rule_get_filter(m_rule);
        char *result = ecs_filter_str(m_world, f);
        return flecs::string(result);
    }

    operator rule<>() const;

protected:
    world_t *m_world;
    rule_t *m_rule;
};

template<typename ... Components>
struct rule final : rule_base, iterable<Components...> {
private:
    using Terms = typename _::term_ptrs<Components...>::array;

    ecs_iter_t get_iter() const override {
        return ecs_rule_iter(m_world, m_rule);
    }

    ecs_iter_next_action_t next_action() const override {
        return ecs_rule_next;
    }

    ecs_iter_next_action_t next_each_action() const override {
        return ecs_rule_next_instanced;
    }

public:
    using rule_base::rule_base;

    int32_t find_var(const char *name) {
        return ecs_rule_find_var(m_rule, name);
    }
};

// Mixin implementation
template <typename... Comps, typename... Args>
inline flecs::rule<Comps...> world::rule(Args &&... args) const {
    return flecs::rule_builder<Comps...>(m_world, FLECS_FWD(args)...)
        .build();
}

template <typename... Comps, typename... Args>
inline flecs::rule_builder<Comps...> world::rule_builder(Args &&... args) const {
    return flecs::rule_builder<Comps...>(m_world, FLECS_FWD(args)...);
}

// rule_base implementation
inline rule_base::operator rule<>() const {
    return flecs::rule<>(m_world, m_rule);
}

} // namespace flecs

#endif
#ifdef FLECS_META
#pragma once

FLECS_ENUM_LAST(flecs::type_kind_t, EcsTypeKindLast)
FLECS_ENUM_LAST(flecs::primitive_kind_t, EcsPrimitiveKindLast)

namespace flecs {
namespace meta {
namespace _ {

inline void init(flecs::world& world) {
    world.component<bool_t>("flecs::meta::bool");
    world.component<char_t>("flecs::meta::char");
    world.component<u8_t>("flecs::meta::u8");
    world.component<u16_t>("flecs::meta::u16");
    world.component<u32_t>("flecs::meta::u32");
    world.component<u64_t>("flecs::meta::u64");
    world.component<i8_t>("flecs::meta::i8");
    world.component<i16_t>("flecs::meta::i16");
    world.component<i32_t>("flecs::meta::i32");
    world.component<i64_t>("flecs::meta::i64");
    world.component<f32_t>("flecs::meta::f32");
    world.component<f64_t>("flecs::meta::f64");

    world.component<type_kind_t>("flecs::meta::type_kind");
    world.component<primitive_kind_t>("flecs::meta::primitive_kind");
    world.component<member_t>("flecs::meta::member");
    world.component<enum_constant_t>("flecs::meta::enum_constant");
    world.component<bitmask_constant_t>("flecs::meta::bitmask_constant");

    world.component<MetaType>("flecs::meta::MetaType");
    world.component<MetaTypeSerialized>("flecs::meta::MetaTypeSerialized");
    world.component<Primitive>("flecs::meta::Primitive");
    world.component<Enum>("flecs::meta::Enum");
    world.component<Bitmask>("flecs::meta::Bitmask");
    world.component<Member>("flecs::meta::Member");
    world.component<Struct>("flecs::meta::Struct");
    world.component<Array>("flecs::meta::Array");
    world.component<Vector>("flecs::meta::Vector");

    world.component<Unit>("flecs::meta::Unit");

    // To support member<uintptr_t> and member<intptr_t> register components
    // (that do not have conflicting symbols with builtin ones) for platform
    // specific types.

    if (!flecs::is_same<i32_t, iptr_t>() && !flecs::is_same<i64_t, iptr_t>()) {
        flecs::_::cpp_type<iptr_t>::init(world, flecs::Iptr, true);
        ecs_assert(flecs::type_id<iptr_t>() == flecs::Iptr, 
            ECS_INTERNAL_ERROR, NULL);
    }

    if (!flecs::is_same<u32_t, uptr_t>() && !flecs::is_same<u64_t, uptr_t>()) {
        flecs::_::cpp_type<uptr_t>::init(world, flecs::Uptr, true);
        ecs_assert(flecs::type_id<uptr_t>() == flecs::Uptr, 
            ECS_INTERNAL_ERROR, NULL);
    }
}

} // namespace _

inline flecs::entity cursor::get_type() const {
    return flecs::entity(m_cursor.world, ecs_meta_get_type(&m_cursor));
}

inline flecs::entity cursor::get_unit() const {
    return flecs::entity(m_cursor.world, ecs_meta_get_unit(&m_cursor));
}

inline flecs::entity cursor::get_entity() const {
    return flecs::entity(m_cursor.world, ecs_meta_get_entity(&m_cursor));
}

} // namespace meta
} // namespace flecs

#endif
#ifdef FLECS_UNITS
#pragma once

namespace flecs {

inline units::units(flecs::world& world) {
    /* Import C module  */
    FlecsUnitsImport(world);

    /* Bridge between C++ types and flecs.units entities */
    world.module<units>();

    // Initialize world.entity(prefixes) scope
    world.entity<Prefixes>("::flecs::units::prefixes");

    // Initialize prefixes
    world.entity<Yocto>("::flecs::units::prefixes::Yocto");
    world.entity<Zepto>("::flecs::units::prefixes::Zepto");
    world.entity<Atto>("::flecs::units::prefixes::Atto");
    world.entity<Femto>("::flecs::units::prefixes::Femto");
    world.entity<Pico>("::flecs::units::prefixes::Pico");
    world.entity<Nano>("::flecs::units::prefixes::Nano");
    world.entity<Micro>("::flecs::units::prefixes::Micro");
    world.entity<Milli>("::flecs::units::prefixes::Milli");
    world.entity<Centi>("::flecs::units::prefixes::Centi");
    world.entity<Deci>("::flecs::units::prefixes::Deci");
    world.entity<Deca>("::flecs::units::prefixes::Deca");
    world.entity<Hecto>("::flecs::units::prefixes::Hecto");
    world.entity<Kilo>("::flecs::units::prefixes::Kilo");
    world.entity<Mega>("::flecs::units::prefixes::Mega");
    world.entity<Giga>("::flecs::units::prefixes::Giga");
    world.entity<Tera>("::flecs::units::prefixes::Tera");
    world.entity<Peta>("::flecs::units::prefixes::Peta");
    world.entity<Exa>("::flecs::units::prefixes::Exa");
    world.entity<Zetta>("::flecs::units::prefixes::Zetta");
    world.entity<Yotta>("::flecs::units::prefixes::Yotta");
    world.entity<Kibi>("::flecs::units::prefixes::Kibi");
    world.entity<Mebi>("::flecs::units::prefixes::Mebi");
    world.entity<Gibi>("::flecs::units::prefixes::Gibi");
    world.entity<Tebi>("::flecs::units::prefixes::Tebi");
    world.entity<Pebi>("::flecs::units::prefixes::Pebi");
    world.entity<Exbi>("::flecs::units::prefixes::Exbi");
    world.entity<Zebi>("::flecs::units::prefixes::Zebi");
    world.entity<Yobi>("::flecs::units::prefixes::Yobi");

    // Initialize quantities
    world.entity<Duration>("::flecs::units::Duration");
    world.entity<Time>("::flecs::units::Time");
    world.entity<Mass>("::flecs::units::Mass");
    world.entity<Force>("::flecs::units::Force");
    world.entity<ElectricCurrent>("::flecs::units::ElectricCurrent");
    world.entity<Amount>("::flecs::units::Amount");
    world.entity<LuminousIntensity>("::flecs::units::LuminousIntensity");
    world.entity<Length>("::flecs::units::Length");
    world.entity<Pressure>("::flecs::units::Pressure");
    world.entity<Speed>("::flecs::units::Speed");
    world.entity<Temperature>("::flecs::units::Temperature");
    world.entity<Data>("::flecs::units::Data");
    world.entity<DataRate>("::flecs::units::DataRate");
    world.entity<Angle>("::flecs::units::Angle");

    // Initialize duration units
    world.entity<duration::PicoSeconds>(
        "::flecs::units::Duration::PicoSeconds");
    world.entity<duration::NanoSeconds>(
        "::flecs::units::Duration::NanoSeconds");
    world.entity<duration::MicroSeconds>(
        "::flecs::units::Duration::MicroSeconds");
    world.entity<duration::MilliSeconds>(
        "::flecs::units::Duration::MilliSeconds");
    world.entity<duration::Seconds>(
        "::flecs::units::Duration::Seconds");
    world.entity<duration::Minutes>(
        "::flecs::units::Duration::Minutes");
    world.entity<duration::Hours>(
        "::flecs::units::Duration::Hours");
    world.entity<duration::Days>(
        "::flecs::units::Duration::Days");

    // Initialize time units
    world.entity<time::Date>("::flecs::units::Time::Date");

    // Initialize mass units
    world.entity<mass::Grams>("::flecs::units::Mass::Grams");
    world.entity<mass::KiloGrams>("::flecs::units::Mass::KiloGrams");

    // Initialize current units
    world.entity<electric_current::Ampere>
    ("::flecs::units::ElectricCurrent::Ampere");  

    // Initialize amount units
    world.entity<amount::Mole>("::flecs::units::Amount::Mole");

    // Initialize luminous intensity units
    world.entity<luminous_intensity::Candela>(
        "::flecs::units::LuminousIntensity::Candela");

    // Initialize force units
    world.entity<force::Newton>("::flecs::units::Force::Newton");

    // Initialize length units
    world.entity<length::Meters>("::flecs::units::Length::Meters");
    world.entity<length::PicoMeters>("::flecs::units::Length::PicoMeters");
    world.entity<length::NanoMeters>("::flecs::units::Length::NanoMeters");
    world.entity<length::MicroMeters>("::flecs::units::Length::MicroMeters");
    world.entity<length::MilliMeters>("::flecs::units::Length::MilliMeters");
    world.entity<length::CentiMeters>("::flecs::units::Length::CentiMeters");
    world.entity<length::KiloMeters>("::flecs::units::Length::KiloMeters");
    world.entity<length::Miles>("::flecs::units::Length::Miles");

    // Initialize pressure units
    world.entity<pressure::Pascal>("::flecs::units::Pressure::Pascal");
    world.entity<pressure::Bar>("::flecs::units::Pressure::Bar");

    // Initialize speed units
    world.entity<speed::MetersPerSecond>(
        "::flecs::units::Speed::MetersPerSecond");
    world.entity<speed::KiloMetersPerSecond>(
        "::flecs::units::Speed::KiloMetersPerSecond");
    world.entity<speed::KiloMetersPerHour>(
        "::flecs::units::Speed::KiloMetersPerHour");
    world.entity<speed::MilesPerHour>(
        "::flecs::units::Speed::MilesPerHour");

    // Initialize temperature units
    world.entity<temperature::Kelvin>(
        "::flecs::units::Temperature::Kelvin");
    world.entity<temperature::Celsius>(
        "::flecs::units::Temperature::Celsius");
    world.entity<temperature::Fahrenheit>(
        "::flecs::units::Temperature::Fahrenheit");

    // Initialize data units
    world.entity<data::Bits>(
        "::flecs::units::Data::Bits");
    world.entity<data::KiloBits>(
        "::flecs::units::Data::KiloBits");
    world.entity<data::MegaBits>(
        "::flecs::units::Data::MegaBits");
    world.entity<data::GigaBits>(
        "::flecs::units::Data::GigaBits");
    world.entity<data::Bytes>(
        "::flecs::units::Data::Bytes");
    world.entity<data::KiloBytes>(
        "::flecs::units::Data::KiloBytes");
    world.entity<data::MegaBytes>(
        "::flecs::units::Data::MegaBytes");
    world.entity<data::GigaBytes>(
        "::flecs::units::Data::GigaBytes");
    world.entity<data::KibiBytes>(
        "::flecs::units::Data::KibiBytes");
    world.entity<data::MebiBytes>(
        "::flecs::units::Data::MebiBytes");
    world.entity<data::GibiBytes>(
        "::flecs::units::Data::GibiBytes");

    // Initialize datarate units
    world.entity<datarate::BitsPerSecond>(
        "::flecs::units::DataRate::BitsPerSecond");
    world.entity<datarate::KiloBitsPerSecond>(
        "::flecs::units::DataRate::KiloBitsPerSecond");
    world.entity<datarate::MegaBitsPerSecond>(
        "::flecs::units::DataRate::MegaBitsPerSecond");
    world.entity<datarate::GigaBitsPerSecond>(
        "::flecs::units::DataRate::GigaBitsPerSecond");
    world.entity<datarate::BytesPerSecond>(
        "::flecs::units::DataRate::BytesPerSecond");
    world.entity<datarate::KiloBytesPerSecond>(
        "::flecs::units::DataRate::KiloBytesPerSecond");
    world.entity<datarate::MegaBytesPerSecond>(
        "::flecs::units::DataRate::MegaBytesPerSecond");
    world.entity<datarate::GigaBytesPerSecond>(
        "::flecs::units::DataRate::GigaBytesPerSecond");

    // Initialize angles
    world.entity<angle::Radians>(
        "::flecs::units::Angle::Radians");
    world.entity<angle::Degrees>(
        "::flecs::units::Angle::Degrees");

    // Initialize percentage
    world.entity<Percentage>("::flecs::units::Percentage");

    // Initialize Bel
    world.entity<Bel>("::flecs::units::Bel");
    world.entity<DeciBel>("::flecs::units::DeciBel");
}

}

#endif



namespace flecs
{

inline flecs::entity iter::system() const {
    return flecs::entity(m_iter->world, m_iter->system);
}

inline flecs::entity iter::event() const {
    return flecs::entity(m_iter->world, m_iter->event);
}

inline flecs::id iter::event_id() const {
    return flecs::id(m_iter->world, m_iter->event_id);
}

 inline flecs::entity iter::self() const {
    return flecs::entity(m_iter->world, m_iter->self);
}

inline flecs::world iter::world() const {
    return flecs::world(m_iter->world);
}

inline flecs::entity iter::entity(size_t row) const {
    ecs_assert(row < static_cast<size_t>(m_iter->count), ECS_COLUMN_INDEX_OUT_OF_RANGE, NULL);
    return flecs::entity(m_iter->world, m_iter->entities[row]);
}

template <typename T>
inline column<T>::column(iter &iter, int32_t index) {
    *this = iter.term<T>(index);
}

inline flecs::entity iter::source(int32_t index) const {
    return flecs::entity(m_iter->world, ecs_term_source(m_iter, index));
}

inline flecs::entity iter::id(int32_t index) const {
    return flecs::entity(m_iter->world, ecs_term_id(m_iter, index));
}

inline flecs::id iter::pair(int32_t index) const {
    flecs::id_t id = ecs_term_id(m_iter, index);
    ecs_check(ECS_HAS_ROLE(id, PAIR), ECS_INVALID_PARAMETER, NULL);
    return flecs::id(m_iter->world, id);
error:
    return flecs::id();
}

/* Obtain type of iter */
inline flecs::type iter::type() const {
    return flecs::type(m_iter->world, m_iter->table);
}

#ifdef FLECS_RULES
inline flecs::entity iter::get_var(int var_id) const {
    ecs_assert(m_iter->next == ecs_rule_next, ECS_INVALID_OPERATION, NULL);
    ecs_assert(var_id != -1, ECS_INVALID_PARAMETER, 0);
    return flecs::entity(m_iter->world, ecs_iter_get_var(m_iter, var_id));
}

/** Get value of variable by name.
 * Get value of a query variable for current result.
 */
inline flecs::entity iter::get_var(const char *name) const {
    ecs_assert(m_iter->next == ecs_rule_next, ECS_INVALID_OPERATION, NULL);
    ecs_rule_iter_t *rit = &m_iter->priv.iter.rule;
    const flecs::rule_t *r = rit->rule;
    int var_id = ecs_rule_find_var(r, name);
    ecs_assert(var_id != -1, ECS_INVALID_PARAMETER, name);
    return flecs::entity(m_iter->world, ecs_iter_get_var(m_iter, var_id));
}
#endif

} // namespace flecs

#pragma once

namespace flecs 
{

// emplace for T(flecs::entity, Args...)
template <typename T, typename ... Args, if_t<
    std::is_constructible<actual_type_t<T>, flecs::entity, Args...>::value >>
inline void emplace(world_t *world, id_t entity, Args&&... args) {
    flecs::entity self(world, entity);
    emplace<T>(world, entity, self, FLECS_FWD(args)...);
}

inline void world::init_builtin_components() {
    component<Component>("flecs::core::Component");
    component<Type>("flecs::core::Type");
    component<Identifier>("flecs::core::Identifier");
    component<Trigger>("flecs::core::Trigger");
    component<Observer>("flecs::core::Observer");
    component<Query>("flecs::core::Query");

#   ifdef FLECS_SYSTEM
    _::system_init(*this);
#   endif
#   ifdef FLECS_TIMER
    _::timer_init(*this);
#   endif
#   ifdef FLECS_DOC
    doc::_::init(*this);
#   endif
#   ifdef FLECS_REST
    rest::_::init(*this);
#   endif
#   ifdef FLECS_META
    meta::_::init(*this);
#   endif
}

template <typename T>
inline flecs::entity world::use(const char *alias) {
    entity_t e = _::cpp_type<T>::id(m_world);
    const char *name = alias;
    if (!name) {
        // If no name is defined, use the entity name without the scope
        name = ecs_get_name(m_world, e);
    }
    ecs_set_alias(m_world, e, name);
    return flecs::entity(m_world, e);
}

inline flecs::entity world::use(const char *name, const char *alias) {
    entity_t e = ecs_lookup_path_w_sep(m_world, 0, name, "::", "::", true);
    ecs_assert(e != 0, ECS_INVALID_PARAMETER, NULL);

    ecs_set_alias(m_world, e, alias);
    return flecs::entity(m_world, e);
}

inline void world::use(flecs::entity e, const char *alias) {
    entity_t eid = e.id();
    const char *name = alias;
    if (!name) {
        // If no name is defined, use the entity name without the scope
        ecs_get_name(m_world, eid);
    }
    ecs_set_alias(m_world, eid, alias);
}

inline flecs::entity world::set_scope(const flecs::entity_t s) const {
    return flecs::entity(ecs_set_scope(m_world, s));
}

inline flecs::entity world::get_scope() const {
    return flecs::entity(m_world, ecs_get_scope(m_world));
}

template <typename T>
inline flecs::entity world::set_scope() const {
    return set_scope( _::cpp_type<T>::id(m_world) ); 
}

inline entity world::lookup(const char *name) const {
    auto e = ecs_lookup_path_w_sep(m_world, 0, name, "::", "::", true);
    return flecs::entity(*this, e);
}

template <typename T>
T* world::get_mut() const {
    flecs::entity e(m_world, _::cpp_type<T>::id(m_world));
    return e.get_mut<T>();
}

template <typename T>
void world::modified() const {
    flecs::entity e(m_world, _::cpp_type<T>::id(m_world));
    return e.modified<T>();
}

template <typename T>
const T* world::get() const {
    flecs::entity e(m_world, _::cpp_type<T>::id(m_world));
    return e.get<T>();
}

template <typename T>
bool world::has() const {
    flecs::entity e(m_world, _::cpp_type<T>::id(m_world));
    return e.has<T>();
}

template <typename T>
void world::add() const {
    flecs::entity e(m_world, _::cpp_type<T>::id(m_world));
    e.add<T>();
}

template <typename T>
void world::remove() const {
    flecs::entity e(m_world, _::cpp_type<T>::id(m_world));
    e.remove<T>();
}

template <typename T>
inline flecs::entity world::singleton() {
    return flecs::entity(m_world, _::cpp_type<T>::id(m_world));
}

template <typename Func, if_t< is_callable<Func>::value > >
void world::get(const Func& func) {
    static_assert(arity<Func>::value == 1, "singleton component must be the only argument");
    _::entity_with_invoker<Func>::invoke_get(
        this->m_world, this->singleton<first_arg_t<Func>>(), func);
}

template <typename Func, if_t< is_callable<Func>::value > >
void world::set(const Func& func) {
    static_assert(arity<Func>::value == 1, "singleton component must be the only argument");
    _::entity_with_invoker<Func>::invoke_get_mut(
        this->m_world, this->singleton<first_arg_t<Func>>(), func);
}

inline flecs::entity world::get_alive(flecs::entity_t e) const {
    e = ecs_get_alive(m_world, e);
    return flecs::entity(m_world, e);
}
/* Prevent clashing with Unreal define. Unreal applications will have to use
 *  ecs_ensure. */
#ifndef ensure
inline flecs::entity world::ensure(flecs::entity_t e) const {
    ecs_ensure(m_world, e);
    return flecs::entity(m_world, e);
}
#endif

template <typename E>
inline flecs::entity enum_data<E>::entity() const {
    return flecs::entity(world_, impl_.id);
}

template <typename E>
inline flecs::entity enum_data<E>::entity(int value) const {
    return flecs::entity(world_, impl_.constants[value].id);
}

template <typename E>
inline flecs::entity enum_data<E>::entity(E value) const {
    return flecs::entity(world_, impl_.constants[static_cast<int>(value)].id);
}

/** Use provided scope for operations ran on returned world.
 * Operations need to be ran in a single statement.
 */
inline flecs::scoped_world world::scope(id_t parent) {
    return scoped_world(m_world, parent);
}

template <typename T>
inline flecs::scoped_world world::scope() {
    flecs::id_t parent = _::cpp_type<T>::id(m_world);
    return scoped_world(m_world, parent);
}

} // namespace flecs




extern "C" {
#endif // __cplusplus

#endif // FLECS_CPP

#endif


#ifdef __cplusplus
}
#endif

#endif

