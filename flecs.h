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
 * @defgroup c C API
 * 
 * @{
 * @}
 */

/**
 * @defgroup core Core
 * @brief Core ECS functionality (entities, storage, queries).
 * 
 * \ingroup c
 * @{
 */

/**
 * @defgroup options API defines
 * @brief Defines for customizing compile time features.
 * @{
 */

/** \def ecs_float_t 
 * Customizable precision for floating point operations */
#ifndef ecs_float_t
#define ecs_float_t float
#endif

/** \def ecs_ftime_t
 * Customizable precision for scalar time values. Change to double precision for 
 * processes that can run for a long time (e.g. longer than a day). */
#ifndef ecs_ftime_t
#define ecs_ftime_t ecs_float_t
#endif

/** \def FLECS_LEGACY
 * Define when building for C89 
 */
// #define FLECS_LEGACY

/** \def FLECS_NO_DEPRECATED_WARNINGS
 * disables deprecated warnings 
 */
#define FLECS_NO_DEPRECATED_WARNINGS

/** \def FLECS_ACCURATE_COUNTERS
 * Define to ensure that global counters used for statistics (such as the 
 * allocation counters in the OS API) are accurate in multithreaded
 * applications, at the cost of increased overhead. 
 */
// #define FLECS_ACCURATE_COUNTERS

/* Make sure provided configuration is valid */
#if defined(FLECS_DEBUG) && defined(FLECS_NDEBUG)
#error "invalid configuration: cannot both define FLECS_DEBUG and FLECS_NDEBUG"
#endif
#if defined(FLECS_DEBUG) && defined(NDEBUG)
#error "invalid configuration: cannot both define FLECS_DEBUG and NDEBUG"
#endif

/** \def FLECS_DEBUG
 * Used for input parameter checking and cheap sanity checks. There are lots of 
 * asserts in every part of the code, so this will slow down applications. 
 */
#if !defined(FLECS_DEBUG) && !defined(FLECS_NDEBUG) 
#if defined(NDEBUG)
#define FLECS_NDEBUG
#else
#define FLECS_DEBUG
#endif
#endif

/** \def FLECS_SANITIZE
 * Enables expensive checks that can detect issues early. Recommended for
 * running tests or when debugging issues. This will severely slow down code.
 */
#ifdef FLECS_SANITIZE
#define FLECS_DEBUG /* If sanitized mode is enabled, so is debug mode */
#endif

/* Tip: if you see weird behavior that you think might be a bug, make sure to
 * test with the FLECS_DEBUG or FLECS_SANITIZE flags enabled. There's a good
 * chance that this gives you more information about the issue! */

/** \def FLECS_SOFT_ASSERT 
 * Define to not abort for recoverable errors, like invalid parameters. An error
 * is still thrown to the console. This is recommended for when running inside a
 * third party runtime, such as the Unreal editor.
 * 
 * Note that internal sanity checks (ECS_INTERNAL_ERROR) will still abort a
 * process, as this gives more information than a (likely) subsequent crash.
 * 
 * When a soft assert occurs, the code will attempt to minimize the number of 
 * side effects of the failed operation, but this may not always be possible.
 * Even though an application may still be able to continue running after a soft 
 * assert, it should be treated as if in an undefined state. 
 */
// #define FLECS_SOFT_ASSERT

/** \def FLECS_KEEP_ASSERT
 * By default asserts are disabled in release mode, when either FLECS_NDEBUG or
 * NDEBUG is defined. Defining FLECS_KEEP_ASSERT ensures that asserts are not
 * disabled. This define can be combined with FLECS_SOFT_ASSERT. 
 */
// #define FLECS_KEEP_ASSERT

/** \def FLECS_CUSTOM_BUILD
 * This macro lets you customize which addons to build flecs with.
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
// #define FLECS_CUSTOM_BUILD

#ifndef FLECS_CUSTOM_BUILD
// #define FLECS_C          /**< C API convenience macros, always enabled */
#define FLECS_CPP           /**< C++ API */
#define FLECS_MODULE        /**< Module support */
#define FLECS_PARSER        /**< String parser for queries */
#define FLECS_PLECS         /**< ECS data definition format */
#define FLECS_RULES         /**< Constraint solver for advanced queries */
#define FLECS_SNAPSHOT      /**< Snapshot & restore ECS data */
#define FLECS_STATS         /**< Access runtime statistics */
#define FLECS_MONITOR       /**< Track runtime statistics periodically */
#define FLECS_METRICS       /**< Expose component data as statistics */
#define FLECS_ALERTS        /**< Monitor conditions for errors */
#define FLECS_SYSTEM        /**< System support */
#define FLECS_PIPELINE      /**< Pipeline support */
#define FLECS_TIMER         /**< Timer support */
#define FLECS_META          /**< Reflection support */
#define FLECS_META_C        /**< Utilities for populating reflection data */
#define FLECS_UNITS         /**< Builtin standard units */
#define FLECS_EXPR          /**< Parsing strings to/from component values */
#define FLECS_JSON          /**< Parsing JSON to/from component values */
#define FLECS_DOC           /**< Document entities & components */
#define FLECS_COREDOC       /**< Documentation for core entities & components */
#define FLECS_LOG           /**< When enabled ECS provides more detailed logs */
#define FLECS_APP           /**< Application addon */
#define FLECS_OS_API_IMPL   /**< Default implementation for OS API */
#define FLECS_HTTP          /**< Tiny HTTP server for connecting to remote UI */
#define FLECS_REST          /**< REST API for querying application data */
// #define FLECS_JOURNAL    /**< Journaling addon (disabled by default) */
#endif // ifndef FLECS_CUSTOM_BUILD

/** \def FLECS_LOW_FOOTPRINT
 * Set a number of constants to values that decrease memory footprint, at the
 * cost of decreased performance. */
// #define FLECS_LOW_FOOTPRINT
#ifdef FLECS_LOW_FOOTPRINT
#define FLECS_HI_COMPONENT_ID (16)
#define FLECS_HI_ID_RECORD_ID (16)
#define FLECS_SPARSE_PAGE_BITS (6)
#define FLECS_ENTITY_PAGE_BITS (6)
#define FLECS_USE_OS_ALLOC
#endif

/** \def FLECS_HI_COMPONENT_ID
 * This constant can be used to balance between performance and memory 
 * utilization. The constant is used in two ways:
 * - Entity ids 0..FLECS_HI_COMPONENT_ID are reserved for component ids.
 * - Used as lookup array size in table edges.
 * 
 * Increasing this value increases the size of the lookup array, which allows
 * fast table traversal, which improves performance of ECS add/remove 
 * operations. Component ids that fall outside of this range use a regular map
 * lookup, which is slower but more memory efficient. */
#ifndef FLECS_HI_COMPONENT_ID
#define FLECS_HI_COMPONENT_ID (256)
#endif

/** \def FLECS_HI_ID_RECORD_ID
 * This constant can be used to balance between performance and memory 
 * utilization. The constant is used to determine the size of the id record
 * lookup array. Id values that fall outside of this range use a regular map
 * lookup, which is slower but more memory efficient.
 */
#ifndef FLECS_HI_ID_RECORD_ID
#define FLECS_HI_ID_RECORD_ID (1024)
#endif

/** \def FLECS_SPARSE_PAGE_BITS
 * This constant is used to determine the number of bits of an id that is used
 * to determine the page index when used with a sparse set. The number of bits
 * determines the page size, which is (1 << bits).
 * Lower values decrease memory utilization, at the cost of more allocations. */
#ifndef FLECS_SPARSE_PAGE_BITS
#define FLECS_SPARSE_PAGE_BITS (12)
#endif

/** \def FLECS_ENTITY_PAGE_BITS
 * Same as FLECS_SPARSE_PAGE_BITS, but for the entity index. */
#ifndef FLECS_ENTITY_PAGE_BITS
#define FLECS_ENTITY_PAGE_BITS (12)
#endif

/** \def FLECS_USE_OS_ALLOC 
 * When enabled, Flecs will use the OS allocator provided in the OS API directly
 * instead of the builtin block allocator. This can decrease memory utilization
 * as memory will be freed more often, at the cost of decreased performance. */
// #define FLECS_USE_OS_ALLOC

/** \def FLECS_ID_DESC_MAX
 * Maximum number of ids to add ecs_entity_desc_t / ecs_bulk_desc_t */
#ifndef FLECS_ID_DESC_MAX
#define FLECS_ID_DESC_MAX (32)
#endif

/** \def FLECS_TERM_DESC_MAX 
 * Maximum number of terms in ecs_filter_desc_t */
#define FLECS_TERM_DESC_MAX (16)

/** \def FLECS_EVENT_DESC_MAX
 * Maximum number of events in ecs_observer_desc_t */
#define FLECS_EVENT_DESC_MAX (8)

/** \def FLECS_VARIABLE_COUNT_MAX
 * Maximum number of query variables per query */
#define FLECS_VARIABLE_COUNT_MAX (64)

/** \def FLECS_QUERY_SCOPE_NESTING_MAX 
 * Maximum nesting depth of query scopes */
#define FLECS_QUERY_SCOPE_NESTING_MAX (8)

/** @} */

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
//// World flags
////////////////////////////////////////////////////////////////////////////////

#define EcsWorldQuitWorkers           (1u << 0)
#define EcsWorldReadonly              (1u << 1)
#define EcsWorldInit                  (1u << 2)
#define EcsWorldQuit                  (1u << 3)
#define EcsWorldFini                  (1u << 4)
#define EcsWorldMeasureFrameTime      (1u << 5)
#define EcsWorldMeasureSystemTime     (1u << 6)
#define EcsWorldMultiThreaded         (1u << 7)


////////////////////////////////////////////////////////////////////////////////
//// OS API flags
////////////////////////////////////////////////////////////////////////////////

#define EcsOsApiHighResolutionTimer   (1u << 0)
#define EcsOsApiLogWithColors         (1u << 1)
#define EcsOsApiLogWithTimeStamp      (1u << 2)
#define EcsOsApiLogWithTimeDelta      (1u << 3)


////////////////////////////////////////////////////////////////////////////////
//// Entity flags (set in upper bits of ecs_record_t::row)
////////////////////////////////////////////////////////////////////////////////

#define EcsEntityIsId                 (1u << 31)
#define EcsEntityIsTarget             (1u << 30)
#define EcsEntityIsTraversable        (1u << 29)


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
#define EcsIdTraversable               (1u << 8)
#define EcsIdTag                       (1u << 9)
#define EcsIdWith                      (1u << 10)
#define EcsIdUnion                     (1u << 11)
#define EcsIdAlwaysOverride            (1u << 12)

#define EcsIdHasOnAdd                  (1u << 16) /* Same values as table flags */
#define EcsIdHasOnRemove               (1u << 17) 
#define EcsIdHasOnSet                  (1u << 18)
#define EcsIdHasUnSet                  (1u << 19)
#define EcsIdHasOnTableFill            (1u << 20)
#define EcsIdHasOnTableEmpty           (1u << 21)
#define EcsIdHasOnTableCreate          (1u << 22)
#define EcsIdHasOnTableDelete          (1u << 23)
#define EcsIdEventMask\
    (EcsIdHasOnAdd|EcsIdHasOnRemove|EcsIdHasOnSet|EcsIdHasUnSet|\
        EcsIdHasOnTableFill|EcsIdHasOnTableEmpty|EcsIdHasOnTableCreate|\
            EcsIdHasOnTableDelete)

#define EcsIdMarkedForDelete           (1u << 30)

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
#define EcsIterNoData                  (1u << 1u)  /* Does iterator provide (component) data */
#define EcsIterIsInstanced             (1u << 2u)  /* Is iterator instanced */
#define EcsIterHasShared               (1u << 3u)  /* Does result have shared terms */
#define EcsIterTableOnly               (1u << 4u)  /* Result only populates table */
#define EcsIterEntityOptional          (1u << 5u)  /* Treat terms with entity subject as optional */
#define EcsIterNoResults               (1u << 6u)  /* Iterator has no results */
#define EcsIterIgnoreThis              (1u << 7u)  /* Only evaluate non-this terms */
#define EcsIterMatchVar                (1u << 8u)  
#define EcsIterHasCondSet              (1u << 10u) /* Does iterator have conditionally set fields */
#define EcsIterProfile                 (1u << 11u) /* Profile iterator performance */

////////////////////////////////////////////////////////////////////////////////
//// Event flags (used by ecs_event_decs_t::flags)
////////////////////////////////////////////////////////////////////////////////

#define EcsEventTableOnly              (1u << 4u)   /* Table event (no data, same as iter flags) */
#define EcsEventNoOnSet                (1u << 16u)  /* Don't emit OnSet/UnSet for inherited ids */

////////////////////////////////////////////////////////////////////////////////
//// Filter flags (used by ecs_filter_t::flags)
////////////////////////////////////////////////////////////////////////////////

#define EcsFilterMatchThis             (1u << 1u)  /* Has terms that match This */
#define EcsFilterMatchOnlyThis         (1u << 2u)  /* Has only terms that match This */
#define EcsFilterMatchPrefab           (1u << 3u)  /* Does filter match prefabs */
#define EcsFilterMatchDisabled         (1u << 4u)  /* Does filter match disabled entities */
#define EcsFilterMatchEmptyTables      (1u << 5u)  /* Does filter return empty tables */
#define EcsFilterMatchAnything         (1u << 6u)  /* False if filter has no/only Not terms */
#define EcsFilterNoData                (1u << 7u)  /* When true, data fields won't be populated */
#define EcsFilterIsInstanced           (1u << 8u)  /* Is filter instanced (see ecs_filter_desc_t) */
#define EcsFilterPopulate              (1u << 9u)  /* Populate data, ignore non-matching fields */
#define EcsFilterHasCondSet            (1u << 10u) /* Does filter have conditionally set fields */
#define EcsFilterUnresolvedByName      (1u << 11u) /* Use by-name matching for unresolved entity identifiers */
#define EcsFilterHasPred               (1u << 12u) /* Filter has equality predicates */
#define EcsFilterHasScopes             (1u << 13u) /* Filter has query scopes */

////////////////////////////////////////////////////////////////////////////////
//// Table flags (used by ecs_table_t::flags)
////////////////////////////////////////////////////////////////////////////////

#define EcsTableHasBuiltins            (1u << 1u)  /* Does table have builtin components */
#define EcsTableIsPrefab               (1u << 2u)  /* Does the table store prefabs */
#define EcsTableHasIsA                 (1u << 3u)  /* Does the table have IsA relationship */
#define EcsTableHasChildOf             (1u << 4u)  /* Does the table type ChildOf relationship */
#define EcsTableHasName                (1u << 5u)  /* Does the table type have (Identifier, Name) */
#define EcsTableHasPairs               (1u << 6u)  /* Does the table type have pairs */
#define EcsTableHasModule              (1u << 7u)  /* Does the table have module data */
#define EcsTableIsDisabled             (1u << 8u)  /* Does the table type has EcsDisabled */
#define EcsTableHasCtors               (1u << 9u)
#define EcsTableHasDtors               (1u << 10u)
#define EcsTableHasCopy                (1u << 11u)
#define EcsTableHasMove                (1u << 12u)
#define EcsTableHasUnion               (1u << 13u)
#define EcsTableHasToggle              (1u << 14u)
#define EcsTableHasOverrides           (1u << 15u)

#define EcsTableHasOnAdd               (1u << 16u) /* Same values as id flags */
#define EcsTableHasOnRemove            (1u << 17u)
#define EcsTableHasOnSet               (1u << 18u)
#define EcsTableHasUnSet               (1u << 19u)
#define EcsTableHasOnTableFill         (1u << 20u)
#define EcsTableHasOnTableEmpty        (1u << 21u)
#define EcsTableHasOnTableCreate       (1u << 22u)
#define EcsTableHasOnTableDelete       (1u << 23u)

#define EcsTableHasTraversable         (1u << 25u)
#define EcsTableHasTarget              (1u << 26u)

#define EcsTableMarkedForDelete        (1u << 30u)

/* Composite table flags */
#define EcsTableHasLifecycle        (EcsTableHasCtors | EcsTableHasDtors)
#define EcsTableIsComplex           (EcsTableHasLifecycle | EcsTableHasUnion | EcsTableHasToggle)
#define EcsTableHasAddActions       (EcsTableHasIsA | EcsTableHasUnion | EcsTableHasCtors | EcsTableHasOnAdd | EcsTableHasOnSet)
#define EcsTableHasRemoveActions    (EcsTableHasIsA | EcsTableHasDtors | EcsTableHasOnRemove | EcsTableHasUnSet)


////////////////////////////////////////////////////////////////////////////////
//// Query flags (used by ecs_query_t::flags)
////////////////////////////////////////////////////////////////////////////////

#define EcsQueryHasRefs                (1u << 1u)  /* Does query have references */
#define EcsQueryIsSubquery             (1u << 2u)  /* Is query a subquery */
#define EcsQueryIsOrphaned             (1u << 3u)  /* Is subquery orphaned */
#define EcsQueryHasOutTerms            (1u << 4u)  /* Does query have out terms */
#define EcsQueryHasNonThisOutTerms     (1u << 5u)  /* Does query have non-this out terms */
#define EcsQueryHasMonitor             (1u << 6u)  /* Does query track changes */
#define EcsQueryTrivialIter            (1u << 7u)  /* Does the query require special features to iterate */


////////////////////////////////////////////////////////////////////////////////
//// Aperiodic action flags (used by ecs_run_aperiodic)
////////////////////////////////////////////////////////////////////////////////

#define EcsAperiodicEmptyTables        (1u << 1u)  /* Process pending empty table events */
#define EcsAperiodicComponentMonitors  (1u << 2u)  /* Process component monitors */
#define EcsAperiodicEmptyQueries       (1u << 4u)  /* Process empty queries */

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
#elif defined(ECS_TARGET_GNU)
#ifndef __cplusplus
#pragma GCC diagnostic ignored "-Wdeclaration-after-statement"
#pragma GCC diagnostic ignored "-Wbad-function-cast"
#endif
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
#pragma GCC diagnostic ignored "-Wunused-macros"
#endif

/* Standard library dependencies */
#include <assert.h>
#include <stdarg.h>
#include <string.h>

/* Non-standard but required. If not provided by platform, add manually. */
#include <stdint.h>

/* Contains macros for importing / exporting symbols */
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
#if defined(flecs_EXPORTS) && (defined(_MSC_VER) || defined(__MINGW32__))
  #define FLECS_API __declspec(dllexport)
#elif defined(flecs_EXPORTS)
  #define FLECS_API __attribute__((__visibility__("default")))
#elif defined(_MSC_VER)
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

/* Utility macro's to do bitwise comparisons between floats without warnings */
#define ECS_EQ(a, b) (ecs_os_memcmp(&(a), &(b), sizeof(a)) == 0)
#define ECS_NEQ(a, b) (!ECS_EQ(a, b))
#define ECS_EQZERO(a) ECS_EQ(a, (uint64_t){0})
#define ECS_NEQZERO(a) ECS_NEQ(a, (uint64_t){0})

#define ECS_CONCAT(a, b) a ## b

////////////////////////////////////////////////////////////////////////////////
//// Magic numbers for sanity checking
////////////////////////////////////////////////////////////////////////////////

/* Magic number to identify the type of the object */
#define ecs_world_t_magic     (0x65637377)
#define ecs_stage_t_magic     (0x65637373)
#define ecs_query_t_magic     (0x65637371)
#define ecs_rule_t_magic      (0x65637375)
#define ecs_table_t_magic     (0x65637374)
#define ecs_filter_t_magic    (0x65637366)
#define ecs_trigger_t_magic   (0x65637372)
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
#define ecs_pair_object ecs_pair_second

#define ecs_poly_id(tag) ecs_pair(ecs_id(EcsPoly), tag)


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

/**
 * @file vec.h
 * @brief Vector with allocator support.
 */

#ifndef FLECS_VEC_H
#define FLECS_VEC_H

#ifdef __cplusplus
extern "C" {
#endif

/** A component column. */
typedef struct ecs_vec_t {
    void *array;
    int32_t count;
    int32_t size;
#ifdef FLECS_DEBUG
    ecs_size_t elem_size;
#endif
} ecs_vec_t;

FLECS_API
ecs_vec_t* ecs_vec_init(
    struct ecs_allocator_t *allocator,
    ecs_vec_t *vec,
    ecs_size_t size,
    int32_t elem_count);

#define ecs_vec_init_t(allocator, vec, T, elem_count) \
    ecs_vec_init(allocator, vec, ECS_SIZEOF(T), elem_count)

FLECS_API
void ecs_vec_init_if(
    ecs_vec_t *vec,
    ecs_size_t size);

#define ecs_vec_init_if_t(vec, T) \
    ecs_vec_init_if(vec, ECS_SIZEOF(T))

FLECS_API
void ecs_vec_fini(
    struct ecs_allocator_t *allocator,
    ecs_vec_t *vec,
    ecs_size_t size);

#define ecs_vec_fini_t(allocator, vec, T) \
    ecs_vec_fini(allocator, vec, ECS_SIZEOF(T))

FLECS_API
ecs_vec_t* ecs_vec_reset(
    struct ecs_allocator_t *allocator,
    ecs_vec_t *vec,
    ecs_size_t size);

#define ecs_vec_reset_t(allocator, vec, T) \
    ecs_vec_reset(allocator, vec, ECS_SIZEOF(T))

FLECS_API
void ecs_vec_clear(
    ecs_vec_t *vec);

FLECS_API
void* ecs_vec_append(
    struct ecs_allocator_t *allocator,
    ecs_vec_t *vec,
    ecs_size_t size);

#define ecs_vec_append_t(allocator, vec, T) \
    ECS_CAST(T*, ecs_vec_append(allocator, vec, ECS_SIZEOF(T)))

FLECS_API
void ecs_vec_remove(
    ecs_vec_t *vec,
    ecs_size_t size,
    int32_t elem);

#define ecs_vec_remove_t(vec, T, elem) \
    ecs_vec_remove(vec, ECS_SIZEOF(T), elem)

FLECS_API
void ecs_vec_remove_last(
    ecs_vec_t *vec);

FLECS_API
ecs_vec_t ecs_vec_copy(
    struct ecs_allocator_t *allocator,
    const ecs_vec_t *vec,
    ecs_size_t size);

#define ecs_vec_copy_t(allocator, vec, T) \
    ecs_vec_copy(allocator, vec, ECS_SIZEOF(T))

FLECS_API
void ecs_vec_reclaim(
    struct ecs_allocator_t *allocator,
    ecs_vec_t *vec,
    ecs_size_t size);

#define ecs_vec_reclaim_t(allocator, vec, T) \
    ecs_vec_reclaim(allocator, vec, ECS_SIZEOF(T))

FLECS_API
void ecs_vec_set_size(
    struct ecs_allocator_t *allocator,
    ecs_vec_t *vec,
    ecs_size_t size,
    int32_t elem_count);

#define ecs_vec_set_size_t(allocator, vec, T, elem_count) \
    ecs_vec_set_size(allocator, vec, ECS_SIZEOF(T), elem_count)

FLECS_API
void ecs_vec_set_min_size(
    struct ecs_allocator_t *allocator,
    ecs_vec_t *vec,
    ecs_size_t size,
    int32_t elem_count);

#define ecs_vec_set_min_size_t(allocator, vec, T, elem_count) \
    ecs_vec_set_min_size(allocator, vec, ECS_SIZEOF(T), elem_count)

FLECS_API
void ecs_vec_set_min_count(
    struct ecs_allocator_t *allocator,
    ecs_vec_t *vec,
    ecs_size_t size,
    int32_t elem_count);

#define ecs_vec_set_min_count_t(allocator, vec, T, elem_count) \
    ecs_vec_set_min_count(allocator, vec, ECS_SIZEOF(T), elem_count)

FLECS_API
void ecs_vec_set_min_count_zeromem(
    struct ecs_allocator_t *allocator,
    ecs_vec_t *vec,
    ecs_size_t size,
    int32_t elem_count);

#define ecs_vec_set_min_count_zeromem_t(allocator, vec, T, elem_count) \
    ecs_vec_set_min_count_zeromem(allocator, vec, ECS_SIZEOF(T), elem_count)

FLECS_API
void ecs_vec_set_count(
    struct ecs_allocator_t *allocator,
    ecs_vec_t *vec,
    ecs_size_t size,
    int32_t elem_count);

#define ecs_vec_set_count_t(allocator, vec, T, elem_count) \
    ecs_vec_set_count(allocator, vec, ECS_SIZEOF(T), elem_count)

FLECS_API
void* ecs_vec_grow(
    struct ecs_allocator_t *allocator,
    ecs_vec_t *vec,
    ecs_size_t size,
    int32_t elem_count);

#define ecs_vec_grow_t(allocator, vec, T, elem_count) \
    ecs_vec_grow(allocator, vec, ECS_SIZEOF(T), elem_count)

FLECS_API
int32_t ecs_vec_count(
    const ecs_vec_t *vec);

FLECS_API
int32_t ecs_vec_size(
    const ecs_vec_t *vec);

FLECS_API
void* ecs_vec_get(
    const ecs_vec_t *vec,
    ecs_size_t size,
    int32_t index);

#define ecs_vec_get_t(vec, T, index) \
    ECS_CAST(T*, ecs_vec_get(vec, ECS_SIZEOF(T), index))

FLECS_API
void* ecs_vec_first(
    const ecs_vec_t *vec);

#define ecs_vec_first_t(vec, T) \
    ECS_CAST(T*, ecs_vec_first(vec))

FLECS_API
void* ecs_vec_last(
    const ecs_vec_t *vec,
    ecs_size_t size);

#define ecs_vec_last_t(vec, T) \
    ECS_CAST(T*, ecs_vec_last(vec, ECS_SIZEOF(T)))

#ifdef __cplusplus
}
#endif

#endif 

/**
 * @file sparse.h
 * @brief Sparse set data structure.
 */

#ifndef FLECS_SPARSE_H
#define FLECS_SPARSE_H


#ifdef __cplusplus
extern "C" {
#endif

/** The number of elements in a single page */
#define FLECS_SPARSE_PAGE_SIZE (1 << FLECS_SPARSE_PAGE_BITS)

typedef struct ecs_sparse_t {
    ecs_vec_t dense;         /* Dense array with indices to sparse array. The
                              * dense array stores both alive and not alive
                              * sparse indices. The 'count' member keeps
                              * track of which indices are alive. */

    ecs_vec_t pages;         /* Chunks with sparse arrays & data */
    ecs_size_t size;         /* Element size */
    int32_t count;           /* Number of alive entries */
    uint64_t max_id;         /* Local max index (if no global is set) */
    struct ecs_allocator_t *allocator;
    struct ecs_block_allocator_t *page_allocator;
} ecs_sparse_t;

/** Initialize sparse set */
FLECS_DBG_API
void flecs_sparse_init(
    ecs_sparse_t *sparse,
    struct ecs_allocator_t *allocator,
    struct ecs_block_allocator_t *page_allocator,
    ecs_size_t elem_size);

#define flecs_sparse_init_t(sparse, allocator, page_allocator, T)\
    flecs_sparse_init(sparse, allocator, page_allocator, ECS_SIZEOF(T))

FLECS_DBG_API
void flecs_sparse_fini(
    ecs_sparse_t *sparse);

/** Remove all elements from sparse set */
FLECS_DBG_API
void flecs_sparse_clear(
    ecs_sparse_t *sparse);

/** Add element to sparse set, this generates or recycles an id */
FLECS_DBG_API
void* flecs_sparse_add(
    ecs_sparse_t *sparse,
    ecs_size_t elem_size);

#define flecs_sparse_add_t(sparse, T)\
    ECS_CAST(T*, flecs_sparse_add(sparse, ECS_SIZEOF(T)))

/** Get last issued id. */
FLECS_DBG_API
uint64_t flecs_sparse_last_id(
    const ecs_sparse_t *sparse);

/** Generate or recycle a new id. */
FLECS_DBG_API
uint64_t flecs_sparse_new_id(
    ecs_sparse_t *sparse);

/** Remove an element */
FLECS_DBG_API
void flecs_sparse_remove(
    ecs_sparse_t *sparse,
    ecs_size_t elem_size,
    uint64_t id);

#define flecs_sparse_remove_t(sparse, T, id)\
    flecs_sparse_remove(sparse, ECS_SIZEOF(T), id)

/** Test if id is alive, which requires the generation count to match. */
FLECS_DBG_API
bool flecs_sparse_is_alive(
    const ecs_sparse_t *sparse,
    uint64_t id);

/** Get value from sparse set by dense id. This function is useful in 
 * combination with flecs_sparse_count for iterating all values in the set. */
FLECS_DBG_API
void* flecs_sparse_get_dense(
    const ecs_sparse_t *sparse,
    ecs_size_t elem_size,
    int32_t index);

#define flecs_sparse_get_dense_t(sparse, T, index)\
    ECS_CAST(T*, flecs_sparse_get_dense(sparse, ECS_SIZEOF(T), index))

/** Get the number of alive elements in the sparse set. */
FLECS_DBG_API
int32_t flecs_sparse_count(
    const ecs_sparse_t *sparse);

/** Get element by (sparse) id. The returned pointer is stable for the duration
 * of the sparse set, as it is stored in the sparse array. */
FLECS_DBG_API
void* flecs_sparse_get(
    const ecs_sparse_t *sparse,
    ecs_size_t elem_size,
    uint64_t id);

#define flecs_sparse_get_t(sparse, T, index)\
    ECS_CAST(T*, flecs_sparse_get(sparse, ECS_SIZEOF(T), index))

/** Same as flecs_sparse_get, but doesn't assert if id is not alive. */
FLECS_DBG_API
void* flecs_sparse_try(
    const ecs_sparse_t *sparse,
    ecs_size_t elem_size,
    uint64_t id);

#define flecs_sparse_try_t(sparse, T, index)\
    ECS_CAST(T*, flecs_sparse_try(sparse, ECS_SIZEOF(T), index))

/** Like get_sparse, but don't care whether element is alive or not. */
FLECS_DBG_API
void* flecs_sparse_get_any(
    const ecs_sparse_t *sparse,
    ecs_size_t elem_size,
    uint64_t id);

#define flecs_sparse_get_any_t(sparse, T, index)\
    ECS_CAST(T*, flecs_sparse_get_any(sparse, ECS_SIZEOF(T), index))

/** Get or create element by (sparse) id. */
FLECS_DBG_API
void* flecs_sparse_ensure(
    ecs_sparse_t *sparse,
    ecs_size_t elem_size,
    uint64_t id);

#define flecs_sparse_ensure_t(sparse, T, index)\
    ECS_CAST(T*, flecs_sparse_ensure(sparse, ECS_SIZEOF(T), index))

/** Fast version of ensure, no liveliness checking */
FLECS_DBG_API
void* flecs_sparse_ensure_fast(
    ecs_sparse_t *sparse,
    ecs_size_t elem_size,
    uint64_t id);

#define flecs_sparse_ensure_fast_t(sparse, T, index)\
    ECS_CAST(T*, flecs_sparse_ensure_fast(sparse, ECS_SIZEOF(T), index))

/** Get pointer to ids (alive and not alive). Use with count() or size(). */
FLECS_DBG_API
const uint64_t* flecs_sparse_ids(
    const ecs_sparse_t *sparse);

/* Publicly exposed APIs 
 * The flecs_ functions aren't exposed directly as this can cause some
 * optimizers to not consider them for link time optimization. */

FLECS_API
void ecs_sparse_init(
    ecs_sparse_t *sparse,
    ecs_size_t elem_size);

#define ecs_sparse_init_t(sparse, T)\
    ecs_sparse_init(sparse, ECS_SIZEOF(T))

FLECS_API
void* ecs_sparse_add(
    ecs_sparse_t *sparse,
    ecs_size_t elem_size);

#define ecs_sparse_add_t(sparse, T)\
    ECS_CAST(T*, ecs_sparse_add(sparse, ECS_SIZEOF(T)))

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
void* ecs_sparse_get_dense(
    const ecs_sparse_t *sparse,
    ecs_size_t elem_size,
    int32_t index);

#define ecs_sparse_get_dense_t(sparse, T, index)\
    ECS_CAST(T*, ecs_sparse_get_dense(sparse, ECS_SIZEOF(T), index))

FLECS_API
void* ecs_sparse_get(
    const ecs_sparse_t *sparse,
    ecs_size_t elem_size,
    uint64_t id);

#define ecs_sparse_get_t(sparse, T, index)\
    ECS_CAST(T*, ecs_sparse_get(sparse, ECS_SIZEOF(T), index))

#ifdef __cplusplus
}
#endif

#endif

/**
 * @file block_allocator.h
 * @brief Block allocator.
 */

#ifndef FLECS_BLOCK_ALLOCATOR_H
#define FLECS_BLOCK_ALLOCATOR_H


typedef struct ecs_block_allocator_block_t {
    void *memory;
    struct ecs_block_allocator_block_t *next;
} ecs_block_allocator_block_t;

typedef struct ecs_block_allocator_chunk_header_t {
    struct ecs_block_allocator_chunk_header_t *next;
} ecs_block_allocator_chunk_header_t;

typedef struct ecs_block_allocator_t {
    ecs_block_allocator_chunk_header_t *head;
    ecs_block_allocator_block_t *block_head;
    ecs_block_allocator_block_t *block_tail;
    int32_t chunk_size;
    int32_t data_size;
    int32_t chunks_per_block;
    int32_t block_size;
    int32_t alloc_count;
} ecs_block_allocator_t;

FLECS_API
void flecs_ballocator_init(
    ecs_block_allocator_t *ba,
    ecs_size_t size);

#define flecs_ballocator_init_t(ba, T)\
    flecs_ballocator_init(ba, ECS_SIZEOF(T))
#define flecs_ballocator_init_n(ba, T, count)\
    flecs_ballocator_init(ba, ECS_SIZEOF(T) * count)

FLECS_API
ecs_block_allocator_t* flecs_ballocator_new(
    ecs_size_t size);

#define flecs_ballocator_new_t(T)\
    flecs_ballocator_new(ECS_SIZEOF(T))
#define flecs_ballocator_new_n(T, count)\
    flecs_ballocator_new(ECS_SIZEOF(T) * count)

FLECS_API
void flecs_ballocator_fini(
    ecs_block_allocator_t *ba);

FLECS_API
void flecs_ballocator_free(
    ecs_block_allocator_t *ba);

FLECS_API
void* flecs_balloc(
    ecs_block_allocator_t *allocator);

FLECS_API
void* flecs_bcalloc(
    ecs_block_allocator_t *allocator);

FLECS_API
void flecs_bfree(
    ecs_block_allocator_t *allocator, 
    void *memory);

FLECS_API
void* flecs_brealloc(
    ecs_block_allocator_t *dst, 
    ecs_block_allocator_t *src, 
    void *memory);

FLECS_API
void* flecs_bdup(
    ecs_block_allocator_t *ba, 
    void *memory);

#endif

/**
 * @file map.h
 * @brief Map data structure.
 */

#ifndef FLECS_MAP_H
#define FLECS_MAP_H


#ifdef __cplusplus
extern "C" {
#endif

typedef uint64_t ecs_map_data_t;
typedef ecs_map_data_t ecs_map_key_t;
typedef ecs_map_data_t ecs_map_val_t;

/* Map type */
typedef struct ecs_bucket_entry_t {
    ecs_map_key_t key;
    ecs_map_val_t value;
    struct ecs_bucket_entry_t *next;
} ecs_bucket_entry_t;

typedef struct ecs_bucket_t {
    ecs_bucket_entry_t *first;
} ecs_bucket_t;

typedef struct ecs_map_t {
    uint8_t bucket_shift;
    bool shared_allocator;
    ecs_bucket_t *buckets;
    int32_t bucket_count;
    int32_t count;
    struct ecs_block_allocator_t *entry_allocator;
    struct ecs_allocator_t *allocator;
} ecs_map_t;

typedef struct ecs_map_iter_t {
    const ecs_map_t *map;
    ecs_bucket_t *bucket;
    ecs_bucket_entry_t *entry;
    ecs_map_data_t *res;
} ecs_map_iter_t;

typedef struct ecs_map_params_t {
    struct ecs_allocator_t *allocator;
    struct ecs_block_allocator_t entry_allocator;
} ecs_map_params_t;

/* Function/macro postfixes meaning:
 *   _ptr:    access ecs_map_val_t as void*
 *   _ref:    access ecs_map_val_t* as T**
 *   _deref:  dereferences a _ref
 *   _alloc:  if _ptr is NULL, alloc
 *   _free:   if _ptr is not NULL, free
 */

FLECS_API
void ecs_map_params_init(
    ecs_map_params_t *params,
    struct ecs_allocator_t *allocator);

FLECS_API
void ecs_map_params_fini(
    ecs_map_params_t *params);

/** Initialize new map. */
FLECS_API
void ecs_map_init(
    ecs_map_t *map,
    struct ecs_allocator_t *allocator);

/** Initialize new map. */
FLECS_API
void ecs_map_init_w_params(
    ecs_map_t *map,
    ecs_map_params_t *params);

/** Initialize new map if uninitialized, leave as is otherwise */
FLECS_API
void ecs_map_init_if(
    ecs_map_t *map,
    struct ecs_allocator_t *allocator);

FLECS_API
void ecs_map_init_w_params_if(
    ecs_map_t *result,
    ecs_map_params_t *params);

/** Deinitialize map. */
FLECS_API
void ecs_map_fini(
    ecs_map_t *map);

/** Get element for key, returns NULL if they key doesn't exist. */
FLECS_API
ecs_map_val_t* ecs_map_get(
    const ecs_map_t *map,
    ecs_map_key_t key);

/* Get element as pointer (auto-dereferences _ptr) */
FLECS_API
void* ecs_map_get_deref_(
    const ecs_map_t *map,
    ecs_map_key_t key);

/** Get or insert element for key. */
FLECS_API
ecs_map_val_t* ecs_map_ensure(
    ecs_map_t *map,
    ecs_map_key_t key);

/** Get or insert pointer element for key, allocate if the pointer is NULL */
FLECS_API
void* ecs_map_ensure_alloc(
    ecs_map_t *map,
    ecs_size_t elem_size,
    ecs_map_key_t key);

/** Insert element for key. */
FLECS_API
void ecs_map_insert(
    ecs_map_t *map,
    ecs_map_key_t key,
    ecs_map_val_t value);

/** Insert pointer element for key, populate with new allocation. */
FLECS_API
void* ecs_map_insert_alloc(
    ecs_map_t *map,
    ecs_size_t elem_size,
    ecs_map_key_t key);

/** Remove key from map. */
FLECS_API
ecs_map_val_t ecs_map_remove(
    ecs_map_t *map,
    ecs_map_key_t key);

/* Remove pointer element, free if not NULL */
FLECS_API
void ecs_map_remove_free(
    ecs_map_t *map,
    ecs_map_key_t key);

/** Remove all elements from map. */
FLECS_API
void ecs_map_clear(
    ecs_map_t *map);

/** Return number of elements in map. */
#define ecs_map_count(map) ((map) ? (map)->count : 0)

/** Is map initialized */
#define ecs_map_is_init(map) ((map) ? (map)->bucket_shift != 0 : false)

/** Return iterator to map contents. */
FLECS_API
ecs_map_iter_t ecs_map_iter(
    const ecs_map_t *map);

/** Obtain next element in map from iterator. */
FLECS_API
bool ecs_map_next(
    ecs_map_iter_t *iter);

/** Copy map. */
FLECS_API
void ecs_map_copy(
    ecs_map_t *dst,
    const ecs_map_t *src);

#define ecs_map_get_ref(m, T, k) ECS_CAST(T**, ecs_map_get(m, k))
#define ecs_map_get_deref(m, T, k) ECS_CAST(T*, ecs_map_get_deref_(m, k))
#define ecs_map_ensure_ref(m, T, k) ECS_CAST(T**, ecs_map_ensure(m, k))

#define ecs_map_insert_ptr(m, k, v) ecs_map_insert(m, k, ECS_CAST(ecs_map_val_t, v))
#define ecs_map_insert_alloc_t(m, T, k) ECS_CAST(T*, ecs_map_insert_alloc(m, ECS_SIZEOF(T), k))
#define ecs_map_ensure_alloc_t(m, T, k) ECS_CAST(T*, ecs_map_ensure_alloc(m, ECS_SIZEOF(T), k))
#define ecs_map_remove_ptr(m, k) ((void*)(ecs_map_remove(m, k)))

#define ecs_map_key(it) ((it)->res[0])
#define ecs_map_value(it) ((it)->res[1])
#define ecs_map_ptr(it) ECS_CAST(void*, ecs_map_value(it))
#define ecs_map_ref(it, T) (ECS_CAST(T**, &((it)->res[1])))

#ifdef __cplusplus
}
#endif

#endif

/**
 * @file allocator.h
 * @brief Allocator that returns memory objects of any size. 
 */

#ifndef FLECS_ALLOCATOR_H
#define FLECS_ALLOCATOR_H


FLECS_DBG_API extern int64_t ecs_block_allocator_alloc_count;
FLECS_DBG_API extern int64_t ecs_block_allocator_free_count;
FLECS_DBG_API extern int64_t ecs_stack_allocator_alloc_count;
FLECS_DBG_API extern int64_t ecs_stack_allocator_free_count;

struct ecs_allocator_t {
    ecs_block_allocator_t chunks;
    struct ecs_sparse_t sizes; /* <size, block_allocator_t> */
};

FLECS_API
void flecs_allocator_init(
    ecs_allocator_t *a);

FLECS_API
void flecs_allocator_fini(
    ecs_allocator_t *a);

FLECS_API
ecs_block_allocator_t* flecs_allocator_get(
    ecs_allocator_t *a, 
    ecs_size_t size);

FLECS_API
char* flecs_strdup(
    ecs_allocator_t *a, 
    const char* str);

FLECS_API
void flecs_strfree(
    ecs_allocator_t *a, 
    char* str);

FLECS_API
void* flecs_dup(
    ecs_allocator_t *a,
    ecs_size_t size,
    const void *src);

#define flecs_allocator(obj) (&obj->allocators.dyn)

#define flecs_alloc(a, size) flecs_balloc(flecs_allocator_get(a, size))
#define flecs_alloc_t(a, T) flecs_alloc(a, ECS_SIZEOF(T))
#define flecs_alloc_n(a, T, count) flecs_alloc(a, ECS_SIZEOF(T) * (count))

#define flecs_calloc(a, size) flecs_bcalloc(flecs_allocator_get(a, size))
#define flecs_calloc_t(a, T) flecs_calloc(a, ECS_SIZEOF(T))
#define flecs_calloc_n(a, T, count) flecs_calloc(a, ECS_SIZEOF(T) * (count))

#define flecs_free(a, size, ptr) flecs_bfree(flecs_allocator_get(a, size), ptr)
#define flecs_free_t(a, T, ptr) flecs_free(a, ECS_SIZEOF(T), ptr)
#define flecs_free_n(a, T, count, ptr) flecs_free(a, ECS_SIZEOF(T) * (count), ptr)

#define flecs_realloc(a, size_dst, size_src, ptr)\
    flecs_brealloc(flecs_allocator_get(a, size_dst),\
    flecs_allocator_get(a, size_src),\
    ptr)
#define flecs_realloc_n(a, T, count_dst, count_src, ptr)\
    flecs_realloc(a, ECS_SIZEOF(T) * (count_dst), ECS_SIZEOF(T) * (count_src), ptr)

#define flecs_dup_n(a, T, count, ptr) flecs_dup(a, ECS_SIZEOF(T) * (count), ptr)

#endif

/**
 * @file strbuf.h
 * @brief Utility for constructing strings.
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

/* Append int to buffer.
 * Returns false when max is reached, true when there is still space */
FLECS_API
bool ecs_strbuf_appendint(
    ecs_strbuf_t *buffer,
    int64_t v);

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

/* Append string to buffer, transfer ownership to buffer.
 * Returns false when max is reached, true when there is still space */
FLECS_API
bool ecs_strbuf_appendstr_zerocpyn(
    ecs_strbuf_t *buffer,
    char *str,
    int32_t n);

/* Append string to buffer, do not free/modify string.
 * Returns false when max is reached, true when there is still space */
FLECS_API
bool ecs_strbuf_appendstr_zerocpy_const(
    ecs_strbuf_t *buffer,
    const char *str);

/* Append string to buffer, transfer ownership to buffer.
 * Returns false when max is reached, true when there is still space */
FLECS_API
bool ecs_strbuf_appendstr_zerocpyn_const(
    ecs_strbuf_t *buffer,
    const char *str,
    int32_t n);

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

/* Append character to as new element in list. */
FLECS_API
bool ecs_strbuf_list_appendch(
    ecs_strbuf_t *buffer,
    char ch);

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

/* Append string as a new element in list */
FLECS_API
bool ecs_strbuf_list_appendstrn(
    ecs_strbuf_t *buffer,
    const char *str,
    int32_t n);

FLECS_API
int32_t ecs_strbuf_written(
    const ecs_strbuf_t *buffer);

#define ecs_strbuf_appendlit(buf, str)\
    ecs_strbuf_appendstrn(buf, str, (int32_t)(sizeof(str) - 1))

#define ecs_strbuf_list_appendlit(buf, str)\
    ecs_strbuf_list_appendstrn(buf, str, (int32_t)(sizeof(str) - 1))

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

/**
 * @defgroup c_os_api OS API
 * @brief Interface for providing OS specific functionality.
 * 
 * \ingroup c
 * @{
 */

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

/* Allocation counters */
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

/* 64 bit thread id */
typedef uint64_t ecs_os_thread_id_t;

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

typedef
ecs_os_thread_id_t (*ecs_os_api_thread_self_t)(void);

/* Tasks */
typedef
ecs_os_thread_t (*ecs_os_api_task_new_t)(
    ecs_os_thread_callback_t callback,
    void *param);

typedef
void* (*ecs_os_api_task_join_t)(
    ecs_os_thread_t thread);

/* Atomic increment / decrement */
typedef
int32_t (*ecs_os_api_ainc_t)(
    int32_t *value);

typedef
int64_t (*ecs_os_api_lainc_t)(
    int64_t *value);

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
 * macros for functions like "strdup", "log" or "_free" */

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
    ecs_os_api_thread_self_t thread_self_;

    /* Tasks */
    ecs_os_api_thread_new_t task_new_;
    ecs_os_api_thread_join_t task_join_;

    /* Atomic incremenet / decrement */
    ecs_os_api_ainc_t ainc_;
    ecs_os_api_ainc_t adec_;
    ecs_os_api_lainc_t lainc_;
    ecs_os_api_lainc_t ladec_;

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

    /* Last recorded timestamp */
    int64_t log_last_timestamp_;

    /* OS API flags */
    ecs_flags32_t flags_;
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
ecs_os_api_t ecs_os_get_api(void);

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

#ifndef ECS_TARGET_POSIX
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
#ifndef ECS_TARGET_POSIX
#define ecs_os_fopen(result, file, mode) fopen_s(result, file, mode)
#else
#define ecs_os_fopen(result, file, mode) (*(result)) = fopen(file, mode)
#endif

/* Threads */
#define ecs_os_thread_new(callback, param) ecs_os_api.thread_new_(callback, param)
#define ecs_os_thread_join(thread) ecs_os_api.thread_join_(thread)
#define ecs_os_thread_self() ecs_os_api.thread_self_()

/* Tasks */
#define ecs_os_task_new(callback, param) ecs_os_api.task_new_(callback, param)
#define ecs_os_task_join(thread) ecs_os_api.task_join_(thread)

/* Atomic increment / decrement */
#define ecs_os_ainc(value) ecs_os_api.ainc_(value)
#define ecs_os_adec(value) ecs_os_api.adec_(value)
#define ecs_os_lainc(value) ecs_os_api.lainc_(value)
#define ecs_os_ladec(value) ecs_os_api.ladec_(value)

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

FLECS_API
void ecs_os_strset(char **str, const char *value);

#ifdef FLECS_ACCURATE_COUNTERS
#define ecs_os_inc(v)  (ecs_os_ainc(v))
#define ecs_os_linc(v) (ecs_os_lainc(v))
#define ecs_os_dec(v)  (ecs_os_adec(v))
#define ecs_os_ldec(v) (ecs_os_ladec(v))
#else
#define ecs_os_inc(v)  (++(*v))
#define ecs_os_linc(v) (++(*v))
#define ecs_os_dec(v)  (--(*v))
#define ecs_os_ldec(v) (--(*v))
#endif

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

/** Are task functions available? */
FLECS_API
bool ecs_os_has_task_support(void);

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

/** @} */

#endif


#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup api_types API types
 * @brief Public API types.
 * @{
 */

/**
 * @defgroup core_types Core API Types
 * @brief Types for core API objects.
 * @{
 */

/** An id. Ids are the things that can be added to an entity. An id can be an
 * entity or pair, and can have optional id flags. */
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

/** An observer reacts to events matching multiple filter terms */
typedef struct ecs_observer_t ecs_observer_t;

/** An observable contains lists of triggers for specific events/components */
typedef struct ecs_observable_t ecs_observable_t;

/* An iterator lets an application iterate entities across tables. */
typedef struct ecs_iter_t ecs_iter_t;

/** Refs cache data that lets them access components faster than ecs_get. */
typedef struct ecs_ref_t ecs_ref_t;

/** Type hooks (callbacks) */
typedef struct ecs_type_hooks_t ecs_type_hooks_t;

/** Type information */
typedef struct ecs_type_info_t ecs_type_info_t;

/* Internal index that stores tables tables for a (component) id */
typedef struct ecs_id_record_t ecs_id_record_t;

/* Internal table storage record */
typedef struct ecs_table_record_t ecs_table_record_t;

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

/** Type that stores poly mixins */
typedef struct ecs_mixins_t ecs_mixins_t;

/** Header for ecs_poly_t objects. */
typedef struct ecs_header_t {
    int32_t magic; /* Magic number verifying it's a flecs object */
    int32_t type;  /* Magic number indicating which type of flecs object */
    ecs_mixins_t *mixins; /* Table with offsets to (optional) mixins */
} ecs_header_t;

/** @} */

/**
 * @defgroup function_types Function types.
 * @brief Function callback types.
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

/** Callback used for comparing components */
typedef int (*ecs_order_by_action_t)(
    ecs_entity_t e1,
    const void *ptr1,
    ecs_entity_t e2,
    const void *ptr2);

/** Callback used for sorting the entire table of components */
typedef void (*ecs_sort_table_action_t)(
    ecs_world_t* world,
    ecs_table_t* table,
    ecs_entity_t* entities,
    void* ptr,
    int32_t size,
    int32_t lo,
    int32_t hi,
    ecs_order_by_action_t order_by);

/** Callback used for grouping tables in a query */
typedef uint64_t (*ecs_group_by_action_t)(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_id_t group_id,
    void *ctx);

/* Callback invoked when a query creates a new group. */
typedef void* (*ecs_group_create_action_t)(
    ecs_world_t *world,
    uint64_t group_id,
    void *group_by_ctx); /* from ecs_query_desc_t */

/* Callback invoked when a query deletes an existing group. */
typedef void (*ecs_group_delete_action_t)(
    ecs_world_t *world,
    uint64_t group_id,
    void *group_ctx,     /* return value from ecs_group_create_action_t */
    void *group_by_ctx); /* from ecs_query_desc_t */

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

/* Destructor function for poly objects */
typedef void (*ecs_poly_dtor_t)(
    ecs_poly_t *poly);

/** @} */

/**
 * @defgroup mixins Poly mixin types.
 * @brief Mixin types for poly mechanism.
 * @{
 */

/** Iterable mixin.
 * Allows its container to be iterated. */
typedef struct ecs_iterable_t {
    ecs_iter_init_action_t init; /**< Callback that creates iterator. */
} ecs_iterable_t;

/** @} */

/**
 * @defgroup query_types Query descriptor types.
 * @brief Types used to describe queries.
 * @{
 */

/** Specify read/write access for term */
typedef enum ecs_inout_kind_t {
    EcsInOutDefault,  /**< InOut for regular terms, In for shared terms */
    EcsInOutNone,     /**< Term is neither read nor written */
    EcsInOut,         /**< Term is both read and written */
    EcsIn,            /**< Term is only read */
    EcsOut,           /**< Term is only written */
} ecs_inout_kind_t;

/** Specify operator for term */
typedef enum ecs_oper_kind_t {
    EcsAnd,           /**< The term must match */
    EcsOr,            /**< One of the terms in an or chain must match */
    EcsNot,           /**< The term must not match */
    EcsOptional,      /**< The term may match */
    EcsAndFrom,       /**< Term must match all components from term id */
    EcsOrFrom,        /**< Term must match at least one component from term id */
    EcsNotFrom,       /**< Term must match none of the components from term id */
} ecs_oper_kind_t;

/* Term id flags  */
#define EcsSelf                       (1u << 1)  /**< Match on self */
#define EcsUp                         (1u << 2)  /**< Match by traversing upwards */
#define EcsDown                       (1u << 3)  /**< Match by traversing downwards (derived, cannot be set) */
#define EcsTraverseAll                (1u << 4)  /**< Match all entities encountered through traversal */
#define EcsCascade                    (1u << 5)  /**< Sort results breadth first */
#define EcsParent                     (1u << 6)  /**< Short for up(ChildOf) */
#define EcsIsVariable                 (1u << 7)  /**< Term id is a variable */
#define EcsIsEntity                   (1u << 8)  /**< Term id is an entity */
#define EcsIsName                     (1u << 9)  /**< Term id is a name (don't attempt to lookup as entity) */
#define EcsFilter                     (1u << 10) /**< Prevent observer from triggering on term */
#define EcsTraverseFlags              (EcsUp|EcsDown|EcsTraverseAll|EcsSelf|EcsCascade|EcsParent)

/* Term flags discovered & set during filter creation. */
#define EcsTermMatchAny               (1u << 0)
#define EcsTermMatchAnySrc            (1u << 1)
#define EcsTermSrcFirstEq             (1u << 2)
#define EcsTermSrcSecondEq            (1u << 3)
#define EcsTermTransitive             (1u << 4)
#define EcsTermReflexive              (1u << 5)
#define EcsTermIdInherited            (1u << 6)

/* Term flags used for term iteration */
#define EcsTermMatchDisabled          (1u << 7)
#define EcsTermMatchPrefab            (1u << 8)

/** Type that describes a single identifier in a term */
typedef struct ecs_term_id_t {
    ecs_entity_t id;            /**< Entity id. If left to 0 and flags does not 
                                 * specify whether id is an entity or a variable
                                 * the id will be initialized to EcsThis. 
                                 * To explicitly set the id to 0, leave the id
                                 * member to 0 and set EcsIsEntity in flags. */

    const char *name;           /**< Name. This can be either the variable name
                                 * (when the EcsIsVariable flag is set) or an
                                 * entity name. When ecs_term_t::move is true,
                                 * the API assumes ownership over the string and
                                 * will free it when the term is destroyed. */

    ecs_entity_t trav;          /**< Relationship to traverse when looking for the
                                 * component. The relationship must have
                                 * the Traversable property. Default is IsA. */

    ecs_flags32_t flags;        /**< Term flags */
} ecs_term_id_t;

/** Type that describes a term (single element in a query) */
struct ecs_term_t {
    ecs_id_t id;                /**< Component id to be matched by term. Can be
                                 * set directly, or will be populated from the
                                 * first/second members, which provide more
                                 * flexibility. */

    ecs_term_id_t src;          /**< Source of term */
    ecs_term_id_t first;        /**< Component or first element of pair */
    ecs_term_id_t second;       /**< Second element of pair */
    
    ecs_inout_kind_t inout;     /**< Access to contents matched by term */
    ecs_oper_kind_t oper;       /**< Operator of term */

    ecs_id_t id_flags;          /**< Id flags of term id */
    char *name;                 /**< Name of term */

    int32_t field_index;        /**< Index of field for term in iterator */
    ecs_id_record_t *idr;       /**< Cached pointer to internal index */

    ecs_flags16_t flags;        /**< Flags that help eval, set by ecs_filter_init */

    bool move;                  /**< Used by internals */
};

/** Use this variable to initialize user-allocated filter object */
FLECS_API extern ecs_filter_t ECS_FILTER_INIT;

/** Filters alllow for ad-hoc quick filtering of entity tables. */
struct ecs_filter_t {
    ecs_header_t hdr;
    
    ecs_term_t *terms;         /**< Array containing terms for filter */
    int32_t term_count;        /**< Number of elements in terms array */
    int32_t field_count;       /**< Number of fields in iterator for filter */
    
    bool owned;                /**< Is filter object owned by filter */
    bool terms_owned;          /**< Is terms array owned by filter */

    ecs_flags32_t flags;       /**< Filter flags */
    
    char *variable_names[1];   /**< Placeholder variable names array */
    int32_t *sizes;            /**< Field size (same for each result) */

    /* Mixins */
    ecs_entity_t entity;       /**< Entity associated with filter (optional) */
    ecs_iterable_t iterable;   /**< Iterable mixin */
    ecs_poly_dtor_t dtor;      /**< Dtor mixin */
    ecs_world_t *world;        /**< World mixin */
};

/* An observer reacts to events matching a filter */
struct ecs_observer_t {
    ecs_header_t hdr;
    
    ecs_filter_t filter;        /**< Query for observer */

    /* Observer events */
    ecs_entity_t events[FLECS_EVENT_DESC_MAX];
    int32_t event_count;   
    
    ecs_iter_action_t callback; /**< See ecs_observer_desc_t::callback */
    ecs_run_action_t run;       /**< See ecs_observer_desc_t::run */

    void *ctx;                  /**< Callback context */
    void *binding_ctx;          /**< Binding context (for language bindings) */

    ecs_ctx_free_t ctx_free;    /**< Callback to free ctx */
    ecs_ctx_free_t binding_ctx_free; /**< Callback to free binding_ctx */

    ecs_observable_t *observable; /**< Observable for observer */

    int32_t *last_event_id;     /**< Last handled event id */
    int32_t last_event_id_storage;

    ecs_id_t register_id;       /**< Id observer is registered with (single term observers only) */
    int32_t term_index;         /**< Index of the term in parent observer (single term observers only) */

    bool is_monitor;            /**< If true, the observer only triggers when the
                                 * filter did not match with the entity before
                                 * the event happened. */

    bool is_multi;              /**< If true, the observer triggers on more than one term */

    /* Mixins */
    ecs_poly_dtor_t dtor;
};

/** @} */

/** Type that contains component lifecycle callbacks. 
 * 
 * \ingroup components
 */
struct ecs_type_hooks_t {
    ecs_xtor_t ctor;            /**< ctor */
    ecs_xtor_t dtor;            /**< dtor */
    ecs_copy_t copy;            /**< copy assignment */
    ecs_move_t move;            /**< move assignment */

    /** Ctor + copy */
    ecs_copy_t copy_ctor;

    /** Ctor + move */
    ecs_move_t move_ctor;

    /** Ctor + move + dtor (or move_ctor + dtor).
     * This combination is typically used when a component is moved from one
     * location to a new location, like when it is moved to a new table. If
     * not set explicitly it will be derived from other callbacks. */
    ecs_move_t ctor_move_dtor;

    /** Move + dtor.
     * This combination is typically used when a component is moved from one
     * location to an existing location, like what happens during a remove. If
     * not set explicitly it will be derived from other callbacks. */
    ecs_move_t move_dtor;

    /** Callback that is invoked when an instance of a component is added. This
     * callback is invoked before triggers are invoked. */
    ecs_iter_action_t on_add;

    /** Callback that is invoked when an instance of the component is set. This
     * callback is invoked before triggers are invoked, and enable the component
     * to respond to changes on itself before others can. */
    ecs_iter_action_t on_set;

    /** Callback that is invoked when an instance of the component is removed. 
     * This callback is invoked after the triggers are invoked, and before the
     * destructor is invoked. */
    ecs_iter_action_t on_remove;

    void *ctx;                       /**< User defined context */
    void *binding_ctx;               /**< Language binding context */

    ecs_ctx_free_t ctx_free;         /**< Callback to free ctx */
    ecs_ctx_free_t binding_ctx_free; /**< Callback to free binding_ctx */
};

/** Type that contains component information (passed to ctors/dtors/...) 
 * 
 * \ingroup components
 */
struct ecs_type_info_t {
    ecs_size_t size;         /**< Size of type */
    ecs_size_t alignment;    /**< Alignment of type */
    ecs_type_hooks_t hooks;  /**< Type hooks */
    ecs_entity_t component;  /**< Handle to component (do not set) */
    const char *name;        /**< Type name. */
};

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

/** Table data */
typedef struct ecs_data_t ecs_data_t;

/* Switch list */
typedef struct ecs_switch_t ecs_switch_t;

/* Cached query table data */
typedef struct ecs_query_table_match_t ecs_query_table_match_t;

////////////////////////////////////////////////////////////////////////////////
//// Non-opaque types
////////////////////////////////////////////////////////////////////////////////

/** Mixin for emitting events to triggers/observers */
/** All observers for a specific event */
typedef struct ecs_event_record_t {
    struct ecs_event_id_record_t *any;
    struct ecs_event_id_record_t *wildcard;
    struct ecs_event_id_record_t *wildcard_pair;
    ecs_map_t event_ids; /* map<id, ecs_event_id_record_t> */
    ecs_entity_t event;
} ecs_event_record_t;

struct ecs_observable_t {
    ecs_event_record_t on_add;
    ecs_event_record_t on_remove;
    ecs_event_record_t on_set;
    ecs_event_record_t un_set;
    ecs_event_record_t on_wildcard;
    ecs_sparse_t events;  /* sparse<event, ecs_event_record_t> */
};

/** Record for entity index */
struct ecs_record_t {
    ecs_id_record_t *idr; /* Id record to (*, entity) for target entities */
    ecs_table_t *table;   /* Identifies a type (and table) in world */
    uint32_t row;         /* Table row of the entity */
    int32_t dense;        /* Index in dense array */    
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

/* Cursor to stack allocator (used internally) */
struct ecs_stack_page_t;

typedef struct ecs_stack_cursor_t {
    struct ecs_stack_page_t *cur;
    int16_t sp;
} ecs_stack_cursor_t;

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
    struct ecs_table_cache_hdr_t *next_list;
} ecs_table_cache_iter_t;

/** Term-iterator specific data */
typedef struct ecs_term_iter_t {
    ecs_term_t term;
    ecs_id_record_t *self_index;
    ecs_id_record_t *set_index;

    ecs_id_record_t *cur;
    ecs_table_cache_iter_t it;
    int32_t index;
    int32_t observed_table_count;
    
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
    const ecs_filter_t *filter;
    ecs_iter_kind_t kind; 
    ecs_term_iter_t term_iter;
    int32_t matches_left;
    int32_t pivot_term;
} ecs_filter_iter_t;

/** Query-iterator specific data */
typedef struct ecs_query_iter_t {
    ecs_query_t *query;
    ecs_query_table_match_t *node, *prev, *last;
    int32_t sparse_smallest;
    int32_t sparse_first;
    int32_t bitset_first;
    int32_t skip_count;
} ecs_query_iter_t;

/** Snapshot-iterator specific data */
typedef struct ecs_snapshot_iter_t {
    ecs_filter_t filter;
    ecs_vec_t tables; /* ecs_table_leaf_t */
    int32_t index;
} ecs_snapshot_iter_t;

typedef struct ecs_rule_op_profile_t {
    int32_t count[2]; /* 0 = enter, 1 = redo */
} ecs_rule_op_profile_t;

/** Rule-iterator specific data */
typedef struct ecs_rule_iter_t {
    const ecs_rule_t *rule;
    struct ecs_var_t *vars;              /* Variable storage */
    const struct ecs_rule_var_t *rule_vars;
    const struct ecs_rule_op_t *ops;
    struct ecs_rule_op_ctx_t *op_ctx;    /* Operation-specific state */
    uint64_t *written;

#ifdef FLECS_DEBUG
    ecs_rule_op_profile_t *profile;
#endif

    bool redo;
    int16_t op;
    int16_t sp;
} ecs_rule_iter_t;

/* Bits for tracking whether a cache was used/whether the array was allocated.
 * Used by flecs_iter_init, flecs_iter_validate and ecs_iter_fini. 
 * Constants are named to enable easy macro substitution. */
#define flecs_iter_cache_ids           (1u << 0u)
#define flecs_iter_cache_columns       (1u << 1u)
#define flecs_iter_cache_sources       (1u << 2u)
#define flecs_iter_cache_ptrs          (1u << 3u)
#define flecs_iter_cache_match_indices (1u << 4u)
#define flecs_iter_cache_variables     (1u << 5u)
#define flecs_iter_cache_all           (255)

/* Inline iterator arrays to prevent allocations for small array sizes */
typedef struct ecs_iter_cache_t {
    ecs_stack_cursor_t stack_cursor; /* Stack cursor to restore to */
    ecs_flags8_t used;       /* For which fields is the cache used */
    ecs_flags8_t allocated;  /* Which fields are allocated */
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

    void *entity_iter;            /* Filter applied after matching a table */
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
    ecs_var_t *variables;         /* Values of variables (if any) */
    int32_t *columns;             /* Query term to table column mapping */
    ecs_entity_t *sources;        /* Entity on which the id was matched (0 if same as entities) */
    int32_t *match_indices;       /* Indices of current match for term. Allows an iterator to iterate
                                   * all permutations of wildcards in query. */
    ecs_ref_t *references;        /* Cached refs to components (if iterating a cache) */
    ecs_flags64_t constrained_vars; /* Bitset that marks constrained variables */
    uint64_t group_id;            /* Group id for table, if group_by is used */
    int32_t field_count;          /* Number of fields in iterator */

    /* Input information */
    ecs_entity_t system;          /* The system (if applicable) */
    ecs_entity_t event;           /* The event (if applicable) */
    ecs_id_t event_id;            /* The (component) id for the event */

    /* Query information */
    ecs_term_t *terms;            /* Terms of query being evaluated */
    int32_t table_count;          /* Active table count for query */
    int32_t term_index;           /* Index of term that emitted an event.
                                   * This field will be set to the 'index' field
                                   * of an observer term. */
    int32_t variable_count;       /* Number of variables for query */
    char **variable_names;        /* Names of variables (if any) */

    /* Context */
    void *param;                  /* Param passed to ecs_run */
    void *ctx;                    /* System context */
    void *binding_ctx;            /* Binding context */

    /* Time */
    ecs_ftime_t delta_time;       /* Time elapsed since last frame */
    ecs_ftime_t delta_system_time;/* Time elapsed since last system invocation */

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
    ecs_iter_action_t callback;   /* Callback of system or observer */
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

/** This is the largest possible component id. Components for the most part
 * occupy the same id range as entities, however they are not allowed to overlap
 * with (8) bits reserved for id flags. */
#define ECS_MAX_COMPONENT_ID (~((uint32_t)(ECS_ID_FLAGS_MASK >> 32)))

/** The maximum number of nested function calls before the core will throw a
 * cycle detected error */
#define ECS_MAX_RECURSION (512)

/** Maximum length of a parser token (used by parser-related addons) */
#define ECS_MAX_TOKEN_SIZE (256)

////////////////////////////////////////////////////////////////////////////////
//// Global type handles
////////////////////////////////////////////////////////////////////////////////

/** This allows passing 0 as type to functions that accept ids */
#define FLECS_ID0ID_ 0

FLECS_API
char* ecs_module_path_from_c(
    const char *c_name);

bool ecs_identifier_is_0(
    const char *id);

/* Constructor that zeromem's a component value */
FLECS_API
void ecs_default_ctor(
    void *ptr, 
    int32_t count, 
    const ecs_type_info_t *ctx);

/* Create allocated string from format */
FLECS_DBG_API
char* ecs_vasprintf(
    const char *fmt,
    va_list args);

/* Create allocated string from format */
FLECS_API
char* ecs_asprintf(
    const char *fmt,
    ...);

/* Convert identifier to snake case */
FLECS_API
char* flecs_to_snake_case(
    const char *str);

FLECS_DBG_API
int32_t flecs_table_observed_count(
    const ecs_table_t *table);

/** Calculate offset from address */
#ifdef __cplusplus
#define ECS_OFFSET(o, offset) reinterpret_cast<void*>((reinterpret_cast<uintptr_t>(o)) + (static_cast<uintptr_t>(offset)))
#else
#define ECS_OFFSET(o, offset) (void*)(((uintptr_t)(o)) + ((uintptr_t)(offset)))
#endif
#define ECS_OFFSET_T(o, T) ECS_OFFSET(o, ECS_SIZEOF(T))

#define ECS_ELEM(ptr, size, index) ECS_OFFSET(ptr, (size) * (index))
#define ECS_ELEM_T(o, T, index) ECS_ELEM(o, ECS_SIZEOF(T), index)

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
 * @file hashmap.h
 * @brief Hashmap data structure.
 */

#ifndef FLECS_HASHMAP_H
#define FLECS_HASHMAP_H


#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    ecs_vec_t keys;
    ecs_vec_t values;
} ecs_hm_bucket_t;

typedef struct {
    ecs_hash_value_action_t hash;
    ecs_compare_action_t compare;
    ecs_size_t key_size;
    ecs_size_t value_size;
    ecs_block_allocator_t *hashmap_allocator;
    ecs_block_allocator_t bucket_allocator;
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
void flecs_hashmap_init_(
    ecs_hashmap_t *hm,
    ecs_size_t key_size,
    ecs_size_t value_size,
    ecs_hash_value_action_t hash,
    ecs_compare_action_t compare,
    ecs_allocator_t *allocator);

#define flecs_hashmap_init(hm, K, V, hash, compare, allocator)\
    flecs_hashmap_init_(hm, ECS_SIZEOF(K), ECS_SIZEOF(V), hash, compare, allocator)

FLECS_DBG_API
void flecs_hashmap_fini(
    ecs_hashmap_t *map);

FLECS_DBG_API
void* flecs_hashmap_get_(
    const ecs_hashmap_t *map,
    ecs_size_t key_size,
    const void *key,
    ecs_size_t value_size);

#define flecs_hashmap_get(map, key, V)\
    (V*)flecs_hashmap_get_(map, ECS_SIZEOF(*key), key, ECS_SIZEOF(V))

FLECS_DBG_API
flecs_hashmap_result_t flecs_hashmap_ensure_(
    ecs_hashmap_t *map,
    ecs_size_t key_size,
    const void *key,
    ecs_size_t value_size);

#define flecs_hashmap_ensure(map, key, V)\
    flecs_hashmap_ensure_(map, ECS_SIZEOF(*key), key, ECS_SIZEOF(V))

FLECS_DBG_API
void flecs_hashmap_set_(
    ecs_hashmap_t *map,
    ecs_size_t key_size,
    void *key,
    ecs_size_t value_size,
    const void *value);

#define flecs_hashmap_set(map, key, value)\
    flecs_hashmap_set_(map, ECS_SIZEOF(*key), key, ECS_SIZEOF(*value), value)

FLECS_DBG_API
void flecs_hashmap_remove_(
    ecs_hashmap_t *map,
    ecs_size_t key_size,
    const void *key,
    ecs_size_t value_size);

#define flecs_hashmap_remove(map, key, V)\
    flecs_hashmap_remove_(map, ECS_SIZEOF(*key), key, ECS_SIZEOF(V))

FLECS_DBG_API
void flecs_hashmap_remove_w_hash_(
    ecs_hashmap_t *map,
    ecs_size_t key_size,
    const void *key,
    ecs_size_t value_size,
    uint64_t hash);

#define flecs_hashmap_remove_w_hash(map, key, V, hash)\
    flecs_hashmap_remove_w_hash_(map, ECS_SIZEOF(*key), key, ECS_SIZEOF(V), hash)

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
    ecs_hashmap_t *dst,
    const ecs_hashmap_t *src);

FLECS_DBG_API
flecs_hashmap_iter_t flecs_hashmap_iter(
    ecs_hashmap_t *map);

FLECS_DBG_API
void* flecs_hashmap_next_(
    flecs_hashmap_iter_t *it,
    ecs_size_t key_size,
    void *key_out,
    ecs_size_t value_size);

#define flecs_hashmap_next(map, V)\
    (V*)flecs_hashmap_next_(map, 0, NULL, ECS_SIZEOF(V))

#define flecs_hashmap_next_w_key(map, K, key, V)\
    (V*)flecs_hashmap_next_(map, ECS_SIZEOF(K), key, ECS_SIZEOF(V))

#ifdef __cplusplus
}
#endif

#endif


/** Used with ecs_entity_init 
 * 
 * \ingroup entities
 */ 
typedef struct ecs_entity_desc_t {
    int32_t _canary;

    ecs_entity_t id;      /**< Set to modify existing entity (optional) */

    const char *name;     /**< Name of the entity. If no entity is provided, an
                           * entity with this name will be looked up first. When
                           * an entity is provided, the name will be verified
                           * with the existing entity. */

    const char *sep;      /**< Optional custom separator for hierarchical names.
                           * Leave to NULL for default ('.') separator. Set to
                           * an empty string to prevent tokenization of name. */

    const char *root_sep; /**< Optional, used for identifiers relative to root */

    const char *symbol;   /**< Optional entity symbol. A symbol is an unscoped
                           * identifier that can be used to lookup an entity. The
                           * primary use case for this is to associate the entity
                           * with a language identifier, such as a type or
                           * function name, where these identifiers differ from
                           * the name they are registered with in flecs. For
                           * example, C type "EcsPosition" might be registered
                           * as "flecs.components.transform.Position", with the
                           * symbol set to "EcsPosition". */

    bool use_low_id;      /**< When set to true, a low id (typically reserved for
                           * components) will be used to create the entity, if
                           * no id is specified. */

    /** Array of ids to add to the new or existing entity. */
    ecs_id_t add[FLECS_ID_DESC_MAX];

    /** String expression with components to add */
    const char *add_expr;
} ecs_entity_desc_t;

/** Used with ecs_bulk_init 
 * 
 * \ingroup entities
 */
typedef struct ecs_bulk_desc_t { 
    int32_t _canary;

    ecs_entity_t *entities; /**< Entities to bulk insert. Entity ids provided by
                             * the application application must be empty (cannot
                             * have components). If no entity ids are provided, the
                             * operation will create 'count' new entities. */

    int32_t count;     /**< Number of entities to create/populate */

    ecs_id_t ids[FLECS_ID_DESC_MAX]; /**< Ids to create the entities with */

    void **data;       /**< Array with component data to insert. Each element in 
                        * the array must correspond with an element in the ids
                        * array. If an element in the ids array is a tag, the
                        * data array must contain a NULL. An element may be
                        * set to NULL for a component, in which case the
                        * component will not be set by the operation. */

    ecs_table_t *table; /**< Table to insert the entities into. Should not be set
                         * at the same time as ids. When 'table' is set at the
                         * same time as 'data', the elements in the data array
                         * must correspond with the ids in the table's type. */

} ecs_bulk_desc_t;

/** Used with ecs_component_init. 
 * 
 * \ingroup components
 */
typedef struct ecs_component_desc_t {
    int32_t _canary;
    
    /** Existing entity to associate with observer (optional) */
    ecs_entity_t entity;

    /** Parameters for type (size, hooks, ...) */
    ecs_type_info_t type;
} ecs_component_desc_t;

/** Used with ecs_filter_init. 
 * 
 * \ingroup filters
 */
typedef struct ecs_filter_desc_t {
    int32_t _canary;

    /** Terms of the filter. If a filter has more terms than 
     * FLECS_TERM_DESC_MAX use terms_buffer */
    ecs_term_t terms[FLECS_TERM_DESC_MAX];

    /** For filters with lots of terms an outside array can be provided. */
    ecs_term_t *terms_buffer;

    /** Number of terms in array provided in terms_buffer. */
    int32_t terms_buffer_count;

    /** External storage to prevent allocation of the filter object */
    ecs_filter_t *storage;

    /** When true, terms returned by an iterator may either contain 1 or N 
     * elements, where terms with N elements are owned, and terms with 1 element 
     * are shared, for example from a parent or base entity. When false, the 
     * iterator will at most return 1 element when the result contains both 
     * owned and shared terms. */ 
    bool instanced;

    /** Flags for advanced usage */
    ecs_flags32_t flags;

    /** Filter expression. Should not be set at the same time as terms array */
    const char *expr;

    /** Entity associated with query (optional) */
    ecs_entity_t entity;
} ecs_filter_desc_t;

/** Used with ecs_query_init. 
 * 
 * \ingroup queries
 */
typedef struct ecs_query_desc_t {
    int32_t _canary;

    /** Filter for the query */
    ecs_filter_desc_t filter;

    /** Component to be used by order_by */
    ecs_entity_t order_by_component;

    /** Callback used for ordering query results. If order_by_id is 0, the 
     * pointer provided to the callback will be NULL. If the callback is not
     * set, results will not be ordered. */
    ecs_order_by_action_t order_by;

    /** Callback used for ordering query results. Same as order_by,
     * but more efficient. */
    ecs_sort_table_action_t sort_table;

    /** Id to be used by group_by. This id is passed to the group_by function and
     * can be used identify the part of an entity type that should be used for
     * grouping. */
    ecs_id_t group_by_id;

    /** Callback used for grouping results. If the callback is not set, results
     * will not be grouped. When set, this callback will be used to calculate a
     * "rank" for each entity (table) based on its components. This rank is then
     * used to sort entities (tables), so that entities (tables) of the same
     * rank are "grouped" together when iterated. */
    ecs_group_by_action_t group_by;

    /** Callback that is invoked when a new group is created. The return value of
     * the callback is stored as context for a group. */
    ecs_group_create_action_t on_group_create;

    /** Callback that is invoked when an existing group is deleted. The return 
     * value of the on_group_create callback is passed as context parameter. */
    ecs_group_delete_action_t on_group_delete;

    /** Context to pass to group_by */
    void *group_by_ctx;

    /** Function to free group_by_ctx */
    ecs_ctx_free_t group_by_ctx_free;

    /** If set, the query will be created as a subquery. A subquery matches at
     * most a subset of its parent query. Subqueries do not directly receive
     * (table) notifications from the world. Instead parent queries forward
     * results to subqueries. This can improve matching performance, as fewer
     * queries need to be matched with new tables.
     * Subqueries can be nested. */
    ecs_query_t *parent;
} ecs_query_desc_t;

/** Used with ecs_observer_init. 
 * 
 * \ingroup observers
 */
typedef struct ecs_observer_desc_t {
    int32_t _canary;

    /** Existing entity to associate with observer (optional) */
    ecs_entity_t entity;

    /** Filter for observer */
    ecs_filter_desc_t filter;

    /** Events to observe (OnAdd, OnRemove, OnSet, UnSet) */
    ecs_entity_t events[FLECS_EVENT_DESC_MAX];

    /** When observer is created, generate events from existing data. For example,
     * EcsOnAdd Position would match all existing instances of Position.
     * This is only supported for events that are iterable (see EcsIterable) */
    bool yield_existing;

    /** Callback to invoke on an event, invoked when the observer matches. */
    ecs_iter_action_t callback;

    /** Callback invoked on an event. When left to NULL the default runner
     * is used which matches the event with the observer's filter, and calls
     * 'callback' when it matches. 
     * A reason to override the run function is to improve performance, if there
     * are more efficient way to test whether an event matches the observer than
     * the general purpose query matcher. */
    ecs_run_action_t run;

    /** User context to pass to callback */
    void *ctx;

    /** Context to be used for language bindings */
    void *binding_ctx;
    
    /** Callback to free ctx */
    ecs_ctx_free_t ctx_free;

    /** Callback to free binding_ctx */     
    ecs_ctx_free_t binding_ctx_free;

    /** Observable with which to register the observer */
    ecs_poly_t *observable;

    /** Optional shared last event id for multiple observers. Ensures only one
     * of the observers with the shared id gets triggered for an event */
    int32_t *last_event_id;

    /** Used for internal purposes */
    int32_t term_index;
} ecs_observer_desc_t;

/**
 * @defgroup misc_types Miscellaneous types
 * @brief Types used to create entities, observers, queries and more.
 * @{
 */

/* Utility to hold a value of a dynamic type */
typedef struct ecs_value_t {
    ecs_entity_t type;
    void *ptr;
} ecs_value_t;

/** Type that contains information about the world. */
typedef struct ecs_world_info_t {
    ecs_entity_t last_component_id;   /**< Last issued component entity id */
    ecs_entity_t min_id;              /**< First allowed entity id */
    ecs_entity_t max_id;              /**< Last allowed entity id */

    ecs_ftime_t delta_time_raw;       /**< Raw delta time (no time scaling) */
    ecs_ftime_t delta_time;           /**< Time passed to or computed by ecs_progress */
    ecs_ftime_t time_scale;           /**< Time scale applied to delta_time */
    ecs_ftime_t target_fps;           /**< Target fps */
    ecs_ftime_t frame_time_total;     /**< Total time spent processing a frame */
    ecs_ftime_t system_time_total;    /**< Total time spent in systems */
    ecs_ftime_t emit_time_total;      /**< Total time spent notifying observers */
    ecs_ftime_t merge_time_total;     /**< Total time spent in merges */
    ecs_ftime_t world_time_total;     /**< Time elapsed in simulation */
    ecs_ftime_t world_time_total_raw; /**< Time elapsed in simulation (no scaling) */
    ecs_ftime_t rematch_time_total;   /**< Time spent on query rematching */
    
    int64_t frame_count_total;        /**< Total number of frames */
    int64_t merge_count_total;        /**< Total number of merges */
    int64_t rematch_count_total;      /**< Total number of rematches */

    int64_t id_create_total;          /**< Total number of times a new id was created */
    int64_t id_delete_total;          /**< Total number of times an id was deleted */
    int64_t table_create_total;       /**< Total number of times a table was created */
    int64_t table_delete_total;       /**< Total number of times a table was deleted */
    int64_t pipeline_build_count_total; /**< Total number of pipeline builds */
    int64_t systems_ran_frame;        /**< Total number of systems ran in last frame */
    int64_t observers_ran_frame;      /**< Total number of times observer was invoked */

    int32_t id_count;                 /**< Number of ids in the world (excluding wildcards) */
    int32_t tag_id_count;             /**< Number of tag (no data) ids in the world */
    int32_t component_id_count;       /**< Number of component (data) ids in the world */
    int32_t pair_id_count;            /**< Number of pair ids in the world */
    int32_t wildcard_id_count;        /**< Number of wildcard ids */

    int32_t table_count;              /**< Number of tables */
    int32_t tag_table_count;          /**< Number of tag-only tables */
    int32_t trivial_table_count;      /**< Number of tables with trivial components (no lifecycle callbacks) */
    int32_t empty_table_count;        /**< Number of tables without entities */
    int32_t table_record_count;       /**< Total number of table records (entries in table caches) */
    int32_t table_storage_count;      /**< Total number of table storages */

    /* -- Command counts -- */
    struct {
        int64_t add_count;             /**< add commands processed */
        int64_t remove_count;          /**< remove commands processed */
        int64_t delete_count;          /**< delete commands processed */
        int64_t clear_count;           /**< clear commands processed */
        int64_t set_count;             /**< set commands processed */
        int64_t get_mut_count;         /**< get_mut/emplace commands processed */
        int64_t modified_count;        /**< modified commands processed */
        int64_t other_count;           /**< other commands processed */
        int64_t discard_count;         /**< commands discarded, happens when entity is no longer alive when running the command */
        int64_t batched_entity_count;  /**< entities for which commands were batched */
        int64_t batched_command_count; /**< commands batched */
    } cmd;

    const char *name_prefix;          /**< Value set by ecs_set_name_prefix. Used
                                       * to remove library prefixes of symbol
                                       * names (such as Ecs, ecs_) when 
                                       * registering them as names. */
} ecs_world_info_t;

/** Type that contains information about a query group. */
typedef struct ecs_query_group_info_t {
    int32_t match_count;  /**< How often tables have been matched/unmatched */
    int32_t table_count;  /**< Number of tables in group */
    void *ctx;            /**< Group context, returned by on_group_create */
} ecs_query_group_info_t;

/** @} */

/**
 * @defgroup builtin_components Builtin component types.
 * @brief Types that represent builtin components.
 * @{
 */

/** A (string) identifier. Used as pair with EcsName and EcsSymbol tags */
typedef struct EcsIdentifier {
    char *value;          /**< Identifier string */
    ecs_size_t length;    /**< Length of identifier */
    uint64_t hash;        /**< Hash of current value */
    uint64_t index_hash;  /**< Hash of existing record in current index */
    ecs_hashmap_t *index; /**< Current index */
} EcsIdentifier;

/** Component information. */
typedef struct EcsComponent {
    ecs_size_t size;           /**< Component size */
    ecs_size_t alignment;      /**< Component alignment */
} EcsComponent;

/** Component for storing a poly object */
typedef struct EcsPoly {
    ecs_poly_t *poly;          /**< Pointer to poly object */
} EcsPoly;

/** Target data for flattened relationships. */
typedef struct EcsTarget {
    int32_t count;
    ecs_record_t *target;
} EcsTarget;

/** Component for iterable entities */
typedef ecs_iterable_t EcsIterable;

/** @} */
/** @} */

/* Only include deprecated definitions if deprecated addon is required */
#ifdef FLECS_DEPRECATED
/**
 * @file addons/deprecated.h
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
 * @defgroup api_constants API Constants
 * @brief Public API constants.
 * @{
 */

/**
 * @defgroup id_flags Component id flags.
 * @brief Id flags are bits that can be set on an id (ecs_id_t).
 * @{
 */

/** Bit added to flags to differentiate between id flags and generation */
#define ECS_ID_FLAG_BIT (1ull << 63)

/** Indicates that the id is a pair. */
FLECS_API extern const ecs_id_t ECS_PAIR;

/** Automatically override component when it is inherited */
FLECS_API extern const ecs_id_t ECS_OVERRIDE;

/** Adds bitset to storage which allows component to be enabled/disabled */
FLECS_API extern const ecs_id_t ECS_TOGGLE;

/** Include all components from entity to which AND is applied */
FLECS_API extern const ecs_id_t ECS_AND;

/** @} */

/**
 * @defgroup builtin_tags Builtin component ids.
 * @{
 */

/* Builtin component ids */
FLECS_API extern const ecs_entity_t ecs_id(EcsComponent);
FLECS_API extern const ecs_entity_t ecs_id(EcsIdentifier);
FLECS_API extern const ecs_entity_t ecs_id(EcsIterable);
FLECS_API extern const ecs_entity_t ecs_id(EcsPoly);

FLECS_API extern const ecs_entity_t EcsQuery;
FLECS_API extern const ecs_entity_t EcsObserver;

/* System module component ids */
FLECS_API extern const ecs_entity_t EcsSystem;
FLECS_API extern const ecs_entity_t ecs_id(EcsTickSource);

/* Pipeline module component ids */
FLECS_API extern const ecs_entity_t ecs_id(EcsPipelineQuery);

/* Timer module component ids */
FLECS_API extern const ecs_entity_t ecs_id(EcsTimer);
FLECS_API extern const ecs_entity_t ecs_id(EcsRateFilter);

/** Root scope for builtin flecs entities */
FLECS_API extern const ecs_entity_t EcsFlecs;

/** Core module scope */
FLECS_API extern const ecs_entity_t EcsFlecsCore;

/** Entity associated with world (used for "attaching" components to world) */
FLECS_API extern const ecs_entity_t EcsWorld;

/** Wildcard entity ("*"). Matches any id, returns all matches. */
FLECS_API extern const ecs_entity_t EcsWildcard;

/** Any entity ("_"). Matches any id, returns only the first. */
FLECS_API extern const ecs_entity_t EcsAny;

/** This entity. Default source for queries. */
FLECS_API extern const ecs_entity_t EcsThis;

/** Variable entity ("$"). Used in expressions to prefix variable names */
FLECS_API extern const ecs_entity_t EcsVariable;

/** Marks a relationship as transitive. 
 * Behavior: 
 *   if R(X, Y) and R(Y, Z) then R(X, Z)
 */
FLECS_API extern const ecs_entity_t EcsTransitive;

/** Marks a relatoinship as reflexive.
 * Behavior: 
 *   R(X, X) == true
 */
FLECS_API extern const ecs_entity_t EcsReflexive;

/** Ensures that entity/component cannot be used as target in IsA relationship.
 * Final can improve the performance of rule-based queries, as they will not 
 * attempt to substitute a final component with its subsets.
 * 
 * Behavior: 
 *   if IsA(X, Y) and Final(Y) throw error
 */
FLECS_API extern const ecs_entity_t EcsFinal;

/** Ensures that component is never inherited from an IsA target.
 * 
 * Behavior:
 *   if DontInherit(X) and X(B) and IsA(A, B) then X(A) is false.
 */
FLECS_API extern const ecs_entity_t EcsDontInherit;

/** Ensures a component is always overridden.
 * 
 * Behavior:
 *   As if the component is added together with OVERRIDE | T
 */
FLECS_API extern const ecs_entity_t EcsAlwaysOverride;

/** Marks relationship as commutative.
 * Behavior:
 *   if R(X, Y) then R(Y, X)
 */
FLECS_API extern const ecs_entity_t EcsSymmetric;

/** Can be added to relationship to indicate that the relationship can only occur
 * once on an entity. Adding a 2nd instance will replace the 1st. 
 *
 * Behavior:
 *   R(X, Y) + R(X, Z) = R(X, Z)
 */
FLECS_API extern const ecs_entity_t EcsExclusive;

/** Marks a relationship as acyclic. Acyclic relationships may not form cycles. */
FLECS_API extern const ecs_entity_t EcsAcyclic;

/** Marks a relationship as traversable. Traversable relationships may be 
 * traversed with "up" queries. Traversable relatinoships are acyclic. */
FLECS_API extern const ecs_entity_t EcsTraversable;

/** Ensure that a component always is added together with another component.
 * 
 * Behavior:
 *   If With(R, O) and R(X) then O(X)
 *   If With(R, O) and R(X, Y) then O(X, Y)
 */
FLECS_API extern const ecs_entity_t EcsWith;

/** Ensure that relationship target is child of specified entity.
 * 
 * Behavior:
 *   If OneOf(R, O) and R(X, Y), Y must be a child of O
 *   If OneOf(R) and R(X, Y), Y must be a child of R
 */
FLECS_API extern const ecs_entity_t EcsOneOf;

/** Can be added to relationship to indicate that it should never hold data, even
 * when it or the relationship target is a component. */
FLECS_API extern const ecs_entity_t EcsTag;

/** Tag to indicate that relationship is stored as union. Union relationships enable
 * changing the target of a union without switching tables. Union relationships
 * are also marked as exclusive. */
FLECS_API extern const ecs_entity_t EcsUnion;

/** Tag to indicate name identifier */
FLECS_API extern const ecs_entity_t EcsName;

/** Tag to indicate symbol identifier */
FLECS_API extern const ecs_entity_t EcsSymbol;

/** Tag to indicate alias identifier */
FLECS_API extern const ecs_entity_t EcsAlias;

/** Used to express parent-child relationships. */
FLECS_API extern const ecs_entity_t EcsChildOf;

/** Used to express inheritance relationships. */
FLECS_API extern const ecs_entity_t EcsIsA;

/** Used to express dependency relationships */
FLECS_API extern const ecs_entity_t EcsDependsOn;

/** Used to express a slot (used with prefab inheritance) */
FLECS_API extern const ecs_entity_t EcsSlotOf;

/** Tag added to module entities */
FLECS_API extern const ecs_entity_t EcsModule;

/** Tag to indicate an entity/component/system is private to a module */
FLECS_API extern const ecs_entity_t EcsPrivate;

/** Tag added to prefab entities. Any entity with this tag is automatically
 * ignored by filters/queries, unless EcsPrefab is explicitly added. */
FLECS_API extern const ecs_entity_t EcsPrefab;

/** When this tag is added to an entity it is skipped by all queries/filters */
FLECS_API extern const ecs_entity_t EcsDisabled;

/** Event. Triggers when an id (component, tag, pair) is added to an entity */
FLECS_API extern const ecs_entity_t EcsOnAdd;

/** Event. Triggers when an id (component, tag, pair) is removed from an entity */
FLECS_API extern const ecs_entity_t EcsOnRemove;

/** Event. Triggers when a component is set for an entity */
FLECS_API extern const ecs_entity_t EcsOnSet;

/** Event. Triggers when a component is unset for an entity */
FLECS_API extern const ecs_entity_t EcsUnSet;

/** Event. Exactly-once observer for when an entity matches/unmatches a filter */
FLECS_API extern const ecs_entity_t EcsMonitor;

/** Event. Triggers when an entity is deleted.
 * Also used as relationship for defining cleanup behavior, see: 
 * https://github.com/SanderMertens/flecs/blob/master/docs/Relationships.md#cleanup-properties
 */
FLECS_API extern const ecs_entity_t EcsOnDelete;

/** Event. Triggers when a table is created. */
FLECS_API extern const ecs_entity_t EcsOnTableCreate;

/** Event. Triggers when a table is deleted. */
FLECS_API extern const ecs_entity_t EcsOnTableDelete;

/** Event. Triggers when a table becomes empty (doesn't emit on creation). */
FLECS_API extern const ecs_entity_t EcsOnTableEmpty;

/** Event. Triggers when a table becomes non-empty. */
FLECS_API extern const ecs_entity_t EcsOnTableFill;

/** Relationship used to define what should happen when a target entity (second
 * element of a pair) is deleted. For details see: 
 * https://github.com/SanderMertens/flecs/blob/master/docs/Relationships.md#cleanup-properties
 */
FLECS_API extern const ecs_entity_t EcsOnDeleteTarget;

/** Remove cleanup policy. Must be used as target in pair with EcsOnDelete or
 * EcsOnDeleteTarget. */
FLECS_API extern const ecs_entity_t EcsRemove;

/** Delete cleanup policy. Must be used as target in pair with EcsOnDelete or
 * EcsOnDeleteTarget. */
FLECS_API extern const ecs_entity_t EcsDelete;

/** Panic cleanup policy. Must be used as target in pair with EcsOnDelete or
 * EcsOnDeleteTarget. */
FLECS_API extern const ecs_entity_t EcsPanic;

FLECS_API extern const ecs_entity_t ecs_id(EcsTarget);
FLECS_API extern const ecs_entity_t EcsFlatten;

/** Used like (EcsDefaultChildComponent, Component). When added to an entity,
 * this informs serialization formats which component to use when a value is
 * assigned to an entity without specifying the component. This is intended as
 * a hint, serialization formats are not required to use it. Adding this 
 * component does not change the behavior of core ECS operations. */
FLECS_API extern const ecs_entity_t EcsDefaultChildComponent;

/* Builtin predicates for comparing entity ids in queries. Only supported by rules */
FLECS_API extern const ecs_entity_t EcsPredEq;
FLECS_API extern const ecs_entity_t EcsPredMatch;
FLECS_API extern const ecs_entity_t EcsPredLookup;

/* Builtin marker entities for opening/closing query scopes */
FLECS_API extern const ecs_entity_t EcsScopeOpen;
FLECS_API extern const ecs_entity_t EcsScopeClose;

/** Tag used to indicate query is empty */
FLECS_API extern const ecs_entity_t EcsEmpty;

/* Pipeline module tags */
FLECS_API extern const ecs_entity_t ecs_id(EcsPipeline);
FLECS_API extern const ecs_entity_t EcsOnStart;
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
FLECS_API extern const ecs_entity_t EcsPhase;

/** Value used to quickly check if component is builtin. This is used to quickly
 * filter out tables with builtin components (for example for ecs_delete) */
#define EcsLastInternalComponentId (ecs_id(EcsPoly))

/** The first user-defined component starts from this id. Ids up to this number
 * are reserved for builtin components */
#define EcsFirstUserComponentId (8)

/** The first user-defined entity starts from this id. Ids up to this number
 * are reserved for builtin components */
#define EcsFirstUserEntityId (FLECS_HI_COMPONENT_ID + 128)

/** @} */
/** @} */

/**
 * @defgroup world_api World
 * @brief Functions for working with `ecs_world_t`.
 * @{
 */

/**
 * @defgroup world_creation_deletion Creation & Deletion
 * @{
 */

/** Create a new world.
 * A world manages all the ECS data and supporting infrastructure. Applications 
 * must have at least one world. Entities, component and system handles are 
 * local to a world and should not be shared between worlds.
 * 
 * This operation creates a world with all builtin modules loaded. 
 *
 * @return A new world
 */
FLECS_API
ecs_world_t* ecs_init(void);

/** Same as ecs_init, but with minimal set of modules loaded.
 *
 * @return A new tiny world
 */
FLECS_API
ecs_world_t* ecs_mini(void);

/** Create a new world with arguments.
 * Same as ecs_init, but allows passing in command line arguments. These can be
 * used to dynamically enable flecs features to an application. Currently these
 * arguments are not used.
 *
 * @return A new world
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

/** @} */

/**
 * @defgroup world_frame Frame functions
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
ecs_ftime_t ecs_frame_begin(
    ecs_world_t *world,
    ecs_ftime_t delta_time);

/** End frame. 
 * This operation must be called at the end of the frame, and always after
 * ecs_frame_begin.
 *
 * @param world The world.
 */
FLECS_API
void ecs_frame_end(
    ecs_world_t *world);

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
    ecs_ftime_t fps);

/** @} */

/**
 * @defgroup commands Commands
 * @{
 */

/** Begin readonly mode.
 * Readonly mode guarantees that no mutations will occur on the world, which
 * makes the world safe to access from multiple threads. While the world is in
 * readonly mode, operations are deferred.
 * 
 * Note that while similar to ecs_defer_begin, deferring only does not guarantee
 * the world is not mutated. Operations that are not deferred (like creating a
 * query) update data structures on the world and are allowed when deferring is
 * enabled, but not when the world is in readonly mode.
 * 
 * A call to ecs_readonly_begin must be followed up with ecs_readonly_end.
 * 
 * The ecs_progress() function automatically enables readonly mode while systems
 * are executed.
 * 
 * When a world has more than one stage, the specific stage must be provided to
 * mutating ECS operations. Failing to do so will throw a readonly assert. A
 * world typically has more than one stage when using threads. An example:
 * 
 * ecs_set_stage_count(world, 2);
 * ecs_stage_t *stage = ecs_get_stage(world, 1);
 * 
 * ecs_readonly_begin(world);
 * ecs_add(world, e, Tag); // readonly assert
 * ecs_add(stage, e, Tag); // OK
 *
 * @param world The world
 * @return Whether world is in readonly mode.
 */
FLECS_API
bool ecs_readonly_begin(
    ecs_world_t *world);

/** End readonly mode.
 * This operation ends readonly mode, and must be called after 
 * ecs_readonly_begin. Operations that were deferred while the world was in
 * readonly mode will be flushed.
 *
 * @param world The world
 */
FLECS_API
void ecs_readonly_end(
    ecs_world_t *world);

/** Merge world or stage.
 * When automatic merging is disabled, an application can call this
 * operation on either an individual stage, or on the world which will merge
 * all stages. This operation may only be called when staging is not enabled
 * (either after progress() or after readonly_end()).
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

/** Suspend deferring but do not flush queue.
 * This operation can be used to do an undeferred operation while not flushing
 * the operations in the queue.
 * 
 * An application should invoke ecs_defer_resume before ecs_defer_end is called.
 * The operation may only be called when deferring is enabled.
 * 
 * @param world The world.
 */
FLECS_API
void ecs_defer_suspend(
    ecs_world_t *world);

/** Resume deferring.
 * See ecs_defer_suspend.
 * 
 * @param world The world.
 */
FLECS_API
void ecs_defer_resume(
    ecs_world_t *world);

/** Enable/disable automerging for world or stage.
 * When automerging is enabled, staged data will automatically be merged with
 * the world when staging ends. This happens at the end of progress(), at a
 * sync point or when readonly_end() is called.
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
 * number of stages. The set_stage_count() operation is useful for applications that
 * want to manage their own stages and/or threads.
 * 
 * @param world The world.
 * @param stages The number of stages.
 */
FLECS_API
void ecs_set_stage_count(
    ecs_world_t *world,
    int32_t stages);

/** Get number of configured stages.
 * Return number of stages set by ecs_set_stage_count.
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

/** Test whether the current world is readonly.
 * This function allows the code to test whether the currently used world
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
 * @defgroup world_misc Misc
 * @{
 */

/** Set a world context.
 * This operation allows an application to register custom data with a world
 * that can be accessed anywhere where the application has the world.
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

/** Get the largest issued entity id (not counting generation).
 * 
 * @param world The world.
 */
FLECS_API
ecs_entity_t ecs_get_max_id(
    const ecs_world_t *world);

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

/** Get world from poly.
 *
 * @param poly A pointer to a poly object.
 * @return The world.
 */
FLECS_API
const ecs_world_t* ecs_get_world(
    const ecs_poly_t *poly);

/** Get entity from poly.
 *
 * @param poly A pointer to a poly object.
 * @return Entity associated with the poly object.
 */
FLECS_API
ecs_entity_t ecs_get_entity(
    const ecs_poly_t *poly);

/** Test if pointer is of specified type.
 * Usage:
 *   ecs_poly_is(ptr, ecs_world_t)
 * 
 * This operation only works for poly types.
 * 
 * @param object The object to test.
 * @param type The id of the type.
 * @return True if the pointer is of the specified type.
 */
FLECS_API
bool ecs_poly_is_(
    const ecs_poly_t *object,
    int32_t type);

#define ecs_poly_is(object, type)\
    ecs_poly_is_(object, type##_magic)

/** Make a pair id.
 * This function is equivalent to using the ecs_pair macro, and is added for
 * convenience to make it easier for non C/C++ bindings to work with pairs.
 *
 * @param first The first element of the pair of the pair.
 * @param second The target of the pair.
 */
FLECS_API
ecs_id_t ecs_make_pair(
    ecs_entity_t first,
    ecs_entity_t second);

/** @} */

/** @} */

/**
 * @defgroup entities Entities
 * @brief Functions for working with `ecs_entity_t`.
 * @{
 */

/**
 * @defgroup creating_entities Creating & Deleting
 * @brief Functions for creating and deleting entities.
 * @{
 */

/** Create new entity id.
 * This operation returns an unused entity id. This operation is guaranteed to
 * return an empty entity as it does not use values set by ecs_set_scope or
 * ecs_set_with.
 *
 * @param world The world.
 * @return The new entity id.
 */
FLECS_API
ecs_entity_t ecs_new_id(
    ecs_world_t *world);

/** Create new low id.
 * This operation returns a new low id. Entity ids start after the
 * FLECS_HI_COMPONENT_ID constant. This reserves a range of low ids for things 
 * like components, and allows parts of the code to optimize operations.
 *
 * Note that FLECS_HI_COMPONENT_ID does not represent the maximum number of 
 * components that can be created, only the maximum number of components that
 * can take advantage of these optimizations.
 * 
 * This operation is guaranteed to return an empty entity as it does not use 
 * values set by ecs_set_scope or ecs_set_with.
 * 
 * This operation does not recycle ids.
 *
 * @param world The world.
 * @return The new component id.
 */
FLECS_API
ecs_entity_t ecs_new_low_id(
    ecs_world_t *world);

/** Create new entity with (component) id.
 * This operation creates a new entity with an optional (component) id. When 0
 * is passed to the id paramter, no component is added to the new entity.
 * 
 * @param world The world.
 * @param id The component id to initialize the new entity with.
 * @return The new entity.
 */
FLECS_API
ecs_entity_t ecs_new_w_id(
    ecs_world_t *world,
    ecs_id_t id);

/** Create new entity in table.
 * This operation creates a new entity in the specified table.
 * 
 * @param world The world.
 * @param table The table to which to add the new entity.
 * @return The new entity.
 */
FLECS_API
ecs_entity_t ecs_new_w_table(
    ecs_world_t *world,
    ecs_table_t *table);

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

/** Create N new entities.
 * This operation is the same as ecs_new_w_id, but creates N entities
 * instead of one.
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

/** @} */

/**
 * @defgroup adding_removing Adding & Removing
 * @brief Functions for adding and removing components.
 * @{
 */

/** Add a (component) id to an entity.
 * This operation adds a single (component) id to an entity. If the entity 
 * already has the id, this operation has no side effects.
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

/** Remove a (component) id from an entity.
 * This operation removes a single (component) id to an entity. If the entity 
 * does not have the id, this operation has no side effects.
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

/** Add override for (component) id.
 * Adding an override to an entity ensures that when the entity is instantiated
 * (by adding an IsA relationship to it) the component with the override is
 * copied to a component that is private to the instance. By default components
 * reachable through an IsA relationship are shared.
 * 
 * Adding an override does not add the component. If an override is added to an
 * entity that does not have the component, it will still be added to the
 * instance, but with an uninitialized value (unless the component has a ctor).
 * When the entity does have the entity, the component of the instance will be
 * initialized with the value of the component on the entity.
 * 
 * This is the same as what happens when calling ecs_add_id for an id that is
 * inherited (reachable through an IsA relationship).
 * 
 * This operation is equivalent to doing:
 *   ecs_add_id(world, entity, ECS_OVERRIDE | id);
 * 
 * @param world The world.
 * @param entity The entity.
 * @param id The id to override.
 */
FLECS_API
void ecs_override_id(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_id_t id);

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

/** @} */

/**
 * @defgroup enabling_disabling Enabling & Disabling
 * @brief Functions for enabling/disabling entities and components.
 * @{
 */

/** Enable or disable entity.
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
void ecs_enable_id(
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
bool ecs_is_enabled_id(
    const ecs_world_t *world,
    ecs_entity_t entity,
    ecs_id_t id);

/** @} */

/**
 * @defgroup getting Getting & Setting
 * @brief Functions for getting/setting components.
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

/** Create a component ref.
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

/** Update ref.
 * Ensures contents of ref are up to date. Same as ecs_ref_get_id, but does not
 * return pointer to component id. 
 * 
 * @param world The world.
 * @param ref The ref.
 */
FLECS_API
void ecs_ref_update(
    const ecs_world_t *world,
    ecs_ref_t *ref);

/** Get a mutable pointer to a component.
 * This operation returns a mutable pointer to a component. If the component did
 * not yet exist, it will be added.
 * 
 * If get_mut is called when the world is in deferred/readonly mode, the 
 * function will:
 * - return a pointer to a temp storage if the component does not yet exist, or
 * - return a pointer to the existing component if it exists
 *
 * @param world The world.
 * @param entity The entity.
 * @param id The entity id of the component to obtain.
 * @return The component pointer.
 */
FLECS_API
void* ecs_get_mut_id(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_id_t id); 

/** Begin exclusive write access to entity.
 * This operation provides safe exclusive access to the components of an entity
 * without the overhead of deferring operations.
 * 
 * When this operation is called simultaneously for the same entity more than
 * once it will throw an assert. Note that for this to happen, asserts must be
 * enabled. It is up to the application to ensure that access is exclusive, for
 * example by using a read-write mutex.
 * 
 * Exclusive access is enforced at the table level, so only one entity can be
 * exclusively accessed per table. The exclusive access check is thread safe.
 * 
 * This operation must be followed up with ecs_write_end.
 * 
 * @param world The world.
 * @param entity The entity.
 * @return A record to the entity.
 */
FLECS_API
ecs_record_t* ecs_write_begin(
    ecs_world_t *world,
    ecs_entity_t entity);

/** End exclusive write access to entity.
 * This operation ends exclusive access, and must be called after 
 * ecs_write_begin.
 * 
 * @param record Record to the entity.
 */
FLECS_API
void ecs_write_end(
    ecs_record_t *record);

/** Begin read access to entity.
 * This operation provides safe read access to the components of an entity.
 * Multiple simultaneous reads are allowed per entity.
 * 
 * This operation ensures that code attempting to mutate the entity's table will
 * throw an assert. Note that for this to happen, asserts must be enabled. It is
 * up to the application to ensure that this does not happen, for example by
 * using a read-write mutex.
 * 
 * This operation does *not* provide the same guarantees as a read-write mutex,
 * as it is possible to call ecs_read_begin after calling ecs_write_begin. It is
 * up to application has to ensure that this does not happen.
 * 
 * This operation must be followed up with ecs_read_end.
 *
 * @param world The world.
 * @param entity The entity.
 * @return A record to the entity.
 */
FLECS_API
const ecs_record_t* ecs_read_begin(
    ecs_world_t *world,
    ecs_entity_t entity);

/** End read access to entity.
 * This operation ends read access, and must be called after ecs_read_begin.
 *
 * @param record Record to the entity.
 */
FLECS_API
void ecs_read_end(
    const ecs_record_t *record);

/** Get entity corresponding with record.
 * This operation only works for entities that are not empty.
 * 
 * @param record The record for which to obtain the entity id.
 */
FLECS_API
ecs_entity_t ecs_record_get_entity(
    const ecs_record_t *record);

/** Get component from entity record.
 * This operation returns a pointer to a component for the entity 
 * associated with the provided record. For safe access to the component, obtain
 * the record with ecs_read_begin or ecs_write_begin.
 * 
 * Obtaining a component from a record is faster than obtaining it from the
 * entity handle, as it reduces the number of lookups required.
 * 
 * @param world The world.
 * @param record Record to the entity.
 * @param id The (component) id.
 * @return Pointer to component, or NULL if entity does not have the component.
 */
FLECS_API
const void* ecs_record_get_id(
    ecs_world_t *world,
    const ecs_record_t *record,
    ecs_id_t id);

/** Same as ecs_record_get_id, but returns a mutable pointer.
 * For safe access to the component, obtain the record with ecs_write_begin.
 * 
 * @param world The world.
 * @param record Record to the entity.
 * @param id The (component) id.
 * @return Pointer to component, or NULL if entity does not have the component.
 */
FLECS_API
void* ecs_record_get_mut_id(
    ecs_world_t *world,
    ecs_record_t *record,
    ecs_id_t id);

/** Test if entity for record has component. 
 * 
 * @param world The world.
 * @param record Record to the entity.
 * @param id The (component) id.
 */
FLECS_API
bool ecs_record_has_id(
    ecs_world_t *world,
    const ecs_record_t *record,
    ecs_id_t id);

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
 * @defgroup liveliness Entity Liveliness
 * @brief Functions for testing and modifying entity liveliness.
 * @{
 */

/** Test whether an entity is valid.
 * Entities that are valid can be used with API functions.
 *
 * An entity is valid if it is not 0 and if it is alive. If the provided id is
 * a pair, the contents of the pair will be checked for validity.
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

/** Override the generation of an entity.
 * The generation count of an entity is increased each time an entity is deleted
 * and is used to test whether an entity id is alive.
 * 
 * This operation overrides the current generation of an entity with the
 * specified generation, which can be useful if an entity is externally managed,
 * like for external pools, savefiles or netcode.
 *
 * @param world The world.
 * @param entity Entity for which to set the generation with the new generation.
 */
FLECS_API
void ecs_set_entity_generation(
    ecs_world_t *world,
    ecs_entity_t entity);

/** Get alive identifier.
 * In some cases an application may need to work with identifiers from which
 * the generation has been stripped. A typical scenario in which this happens is
 * when iterating relationships in an entity type.
 *
 * For example, when obtaining the parent id from a ChildOf relationship, the parent
 * (second element of the pair) will have been stored in a 32 bit value, which 
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
 * An id can be an entity or pair, and can contain id flags. This operation
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

/** @} */

/**
 * @defgroup entity_info Entity Information.
 * @brief Get information from entity.
 * @{
 */

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
    const ecs_table_t *table);

/** Convert entity to string.
 * Same as combining:
 * - ecs_get_fullpath(world, entity)
 * - ecs_type_str(world, ecs_get_type(world, entity))
 * 
 * The result of this operation must be freed with ecs_os_free.
 *
 * @param world The world.
 * @param entity The entity.
 * @return The entity path with stringified type.
 */
FLECS_API
char* ecs_entity_str(
    const ecs_world_t *world,
    ecs_entity_t entity);

/** Test if an entity has an id.
 * This operation returns true if the entity has or inherits the specified id.
 *
 * @param world The world.
 * @param entity The entity.
 * @param id The id to test for.
 * @return True if the entity has the id, false if not.
 */
FLECS_API
bool ecs_has_id(
    const ecs_world_t *world,
    ecs_entity_t entity,
    ecs_id_t id);

/** Test if an entity owns an id.
 * This operation returns true if the entity has the specified id. Other than
 * ecs_has_id this operation will not return true if the id is inherited.
 *
 * @param world The world.
 * @param entity The entity.
 * @param id The id to test for.
 * @return True if the entity has the id, false if not.
 */
FLECS_API
bool ecs_owns_id(
    const ecs_world_t *world,
    ecs_entity_t entity,
    ecs_id_t id);

/** Get the target of a relationship.
 * This will return a target (second element of a pair) of the entity for the 
 * specified relationship. The index allows for iterating through the targets, if a 
 * single entity has multiple targets for the same relationship.
 *
 * If the index is larger than the total number of instances the entity has for
 * the relationship, the operation will return 0.
 *
 * @param world The world.
 * @param entity The entity.
 * @param rel The relationship between the entity and the target.
 * @param index The index of the relationship instance.
 * @return The target for the relationship at the specified index.
 */
FLECS_API
ecs_entity_t ecs_get_target(
    const ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_t rel,
    int32_t index);

/** Get parent (target of ChildOf relationship) for entity.
 * This operation is the same as calling:
 *   ecs_get_target(world, entity, EcsChildOf, 0);
 * 
 * @param world The world.
 * @param entity The entity.
 * @return The parent of the entity, 0 if the entity has no parent.
 */
FLECS_API
ecs_entity_t ecs_get_parent(
    const ecs_world_t *world,
    ecs_entity_t entity);

/** Get the target of a relationship for a given id.
 * This operation returns the first entity that has the provided id by following
 * the specified relationship. If the entity itself has the id then entity will
 * be returned. If the id cannot be found on the entity or by following the
 * relationship, the operation will return 0.
 * 
 * This operation can be used to lookup, for example, which prefab is providing
 * a component by specifying the IsA relationship:
 * 
 *   // Is Position provided by the entity or one of its base entities?
 *   ecs_get_target_for_id(world, entity, EcsIsA, ecs_id(Position))
 * 
 * @param world The world.
 * @param entity The entity.
 * @param rel The relationship to follow.
 * @param id The id to lookup.
 * @return The entity for which the target has been found.
 */
FLECS_API
ecs_entity_t ecs_get_target_for_id(
    const ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_t rel,
    ecs_id_t id);

/** Return depth for entity in tree for relationship rel.
 * Depth is determined by counting the number of targets encountered while 
 * traversing up the relationship tree for rel. Only acyclic relationships are
 * supported.
 * 
 * @param world The world.
 * @param entity The entity.
 * @param rel The relationship.
 * @return The depth of the entity in the tree.
 */
FLECS_API
int32_t ecs_get_depth(
    const ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_t rel);

typedef struct ecs_flatten_desc_t {
    /* When true, the flatten operation will not remove names from entities in
     * the flattened tree. This may fail if entities from different subtrees
     * have the same name. */
    bool keep_names;

    /* When true, the flattened tree won't contain information about the 
     * original depth of the entities. This can reduce fragmentation, but may
     * cause existing code, such as cascade queries, to no longer work. */
    bool lose_depth;
} ecs_flatten_desc_t;

/** Recursively flatten relationship for target entity (experimental).
 * This operation combines entities in the subtree of the specified pair from
 * different parents in the same table. This can reduce memory fragmentation
 * and reduces the number of tables in the storage, which improves RAM 
 * utilization and various other operations, such as entity cleanup.
 * 
 * The lifecycle of entities in a fixed subtree are bound to the specified
 * parent. Entities in a fixed subtree cannot be deleted individually. Entities
 * can also not change the target of the fixed relationship, which includes
 * removing the relationship.
 * 
 * Entities in a fixed subtree are still fragmented on subtree depth. This 
 * ensures that entities can still be iterated in breadth-first order with the
 * cascade query modifier.
 * 
 * The current implementation is limited to exclusive acyclic relationships, and
 * does not allow for adding/removing to entities in flattened tables. An entity
 * may only be flattened for a single relationship. Future iterations of the
 * feature may remove these limitations.
 * 
 * @param world The world.
 * @param pair The relationship pair from which to start flattening.
 * @param desc Options for flattening the tree.
 */
FLECS_API
void ecs_flatten(
    ecs_world_t *world,
    ecs_id_t pair,
    const ecs_flatten_desc_t *desc);

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
 * @defgroup paths Entity Names
 * @brief Functions for working with entity names and paths.
 * @{
 */

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
 * 
 * @param world The world.
 * @param symbol The symbol.
 * @param lookup_as_path If not found as a symbol, lookup as path.
 * @param recursive If looking up as path, recursively traverse up the tree.
 * @return The entity if found, else 0.
 */
FLECS_API
ecs_entity_t ecs_lookup_symbol(
    const ecs_world_t *world,
    const char *symbol,
    bool lookup_as_path,
    bool recursive);

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

/** @} */

/**
 * @defgroup components Components
 * @brief Functions for registering and working with components.
 * @{
 */

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

/** Register hooks for component.
 * Hooks allow for the execution of user code when components are constructed,
 * copied, moved, destructed, added, removed or set. Hooks can be assigned as
 * as long as a component has not yet been used (added to an entity).
 * 
 * The hooks that are currently set can be accessed with ecs_get_type_info.
 *
 * @param world The world.
 * @param id The component id for which to register the actions
 * @param hooks Type that contains the component actions.
 */
FLECS_API
void ecs_set_hooks_id(
    ecs_world_t *world,
    ecs_entity_t id,
    const ecs_type_hooks_t *hooks);

/** Get hooks for component.
 * 
 * @param world The world.
 * @param id The component id for which to retrieve the hooks.
 * @return The hooks for the component, or NULL if not registered.
 */
FLECS_API
const ecs_type_hooks_t* ecs_get_hooks_id(
    ecs_world_t *world,
    ecs_entity_t id);

/** @} */

/**
 * @defgroup ids Ids
 * @brief Functions for working with `ecs_id_t`.
 * @{
 */

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
bool ecs_id_is_tag(
    const ecs_world_t *world,
    ecs_id_t id);

/** Return whether represents a union.
 * This operation returns whether the specified type represents a union. Only
 * pair ids can be unions.
 * 
 * An id represents a union when:
 * - The first element of the pair is EcsUnion/flecs::Union
 * - The first element of the pair has EcsUnion/flecs::Union
 *
 * @param world The world.
 * @param id The id.
 * @return Whether the provided id represents a union.
 */
FLECS_API
bool ecs_id_is_union(
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
    const ecs_world_t *world,
    ecs_id_t id);

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
 * - The first pair element is returned if it is a component
 * - 0 is returned if the relationship entity has the Tag property
 * - The second pair element is returned if it is a component
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

/** Get flags associated with id.
 * This operation returns the internal flags (see api_flags.h) that are 
 * associated with the provided id.
 * 
 * @param world The world.
 * @param id The id.
 * @return Flags associated with the id, or 0 if the id is not in use.
 */
FLECS_API
ecs_flags32_t ecs_id_get_flags(
    const ecs_world_t *world,
    ecs_id_t id);

/** Convert id flag to string.
 * This operation converts a id flag to a string.
 * 
 * @param id_flags The id flag.
 * @return The id flag string, or NULL if no valid id is provided.
 */
FLECS_API
const char* ecs_id_flag_str(
    ecs_id_t id_flags);

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

/** @} */

/**
 * @defgroup filters Filters
 * @brief Functions for working with `ecs_term_t` and `ecs_filter_t`.
 * @{
 */

/** Iterator for a single (component) id.
 * A term iterator returns all entities (tables) that match a single (component)
 * id. The search for the matching set of entities (tables) is performed in 
 * constant time.
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

/** Progress a term iterator.
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

/** Iterator for a parent's children.
 * This operation is equivalent to a term iterator for (ChildOf, parent). 
 * Iterate the result with ecs_children_next.
 * 
 * @param world The world.
 * @param parent The parent for which to iterate the children.
 * @return The iterator.
 */
FLECS_API
ecs_iter_t ecs_children(
    const ecs_world_t *world,
    ecs_entity_t parent);

/** Progress a children iterator.
 * Equivalent to ecs_term_next.
 * 
 * @param it The iterator.
 * @returns True if more data is available, false if not.
 */
FLECS_API
bool ecs_children_next(
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

FLECS_API
bool ecs_term_match_this(
    const ecs_term_t *term);

FLECS_API
bool ecs_term_match_0(
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
 * @param term The term to finalize.
 * @return Zero if success, nonzero if an error occurred.
 */
FLECS_API 
int ecs_term_finalize(
    const ecs_world_t *world,
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
 * itself is not freed.
 *
 * @param term The term to free.
 */
FLECS_API
void ecs_term_fini(
    ecs_term_t *term);

/** Initialize filter 
 * A filter is a lightweight object that can be used to query for entities in
 * a world. Filters, as opposed to queries, do not cache results. They are 
 * therefore slower to iterate, but are faster to create.
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
 * It is possible to create a filter without allocating any memory, by setting
 * the .storage member in ecs_filter_desc_t. See the documentation for the 
 * member for more details.
 *
 * @param world The world.
 * @param desc Properties for the filter to create.
 * @return The filter if successful, NULL if not successful.
 */
FLECS_API
ecs_filter_t * ecs_filter_init(
    ecs_world_t *world,
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
 * specified filter.
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
 * @brief Functions for working with `ecs_query_t`.
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

/** Get filter from a query.
 * This operation obtains a pointer to the internally constructed filter
 * of the query and can be used to introspect the query terms.
 *
 * @param query The query.
 */
FLECS_API
const ecs_filter_t* ecs_query_get_filter(
    const ecs_query_t *query);

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

/** Fast alternative to ecs_query_next that only returns matched tables.
 * This operation only populates the ecs_iter_t::table field. To access the
 * matched components, call ecs_query_populate.
 * 
 * If this operation is used with a query that has inout/out terms, those terms
 * will not be marked dirty unless ecs_query_populate is called. 
 * 
 * @param iter The iterator.
 * @returns True if more data is available, false if not.
 */
FLECS_API
bool ecs_query_next_table(
    ecs_iter_t *iter);

/** Populate iterator fields.
 * This operation can be combined with ecs_query_next_table to populate the
 * iterator fields for the current table.
 * 
 * Populating fields conditionally can save time when a query uses change 
 * detection, and only needs iterator data when the table has changed. When this
 * operation is called, inout/out terms will be marked dirty.
 * 
 * In cases where inout/out terms are conditionally written and no changes
 * were made after calling ecs_query_populate, the ecs_query_skip function can
 * be called to prevent the matched table components from being marked dirty.
 * 
 * This operation does should not be used with queries that match disabled 
 * components, union relationships, or with queries that use order_by.
 * 
 * When the when_changed argument is set to true, the iterator data will only
 * populate when the data has changed, using query change detection.
 * 
 * @param iter The iterator.
 * @param when_changed Only populate data when result has changed.
 */
FLECS_API
int ecs_query_populate(
    ecs_iter_t *iter,
    bool when_changed);

/** Returns whether the query data changed since the last iteration.
 * The operation will return true after:
 * - new entities have been matched with
 * - new tables have been matched/unmatched with
 * - matched entities were deleted
 * - matched components were changed
 * 
 * The operation will not return true after a write-only (EcsOut) or filter
 * (EcsInOutNone) term has changed, when a term is not matched with the
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

/** Set group to iterate for query iterator.
 * This operation limits the results returned by the query to only the selected
 * group id. The query must have a group_by function, and the iterator must
 * be a query iterator.
 * 
 * Groups are sets of tables that are stored together in the query cache based
 * on a group id, which is calculated per table by the group_by function. To 
 * iterate a group, an iterator only needs to know the first and last cache node
 * for that group, which can both be found in a fast O(1) operation.
 * 
 * As a result, group iteration is one of the most efficient mechanisms to 
 * filter out large numbers of entities, even if those entities are distributed
 * across many tables. This makes it a good fit for things like dividing up
 * a world into cells, and only iterating cells close to a player.
 * 
 * The group to iterate must be set before the first call to ecs_query_next. No
 * operations that can add/remove components should be invoked between calling 
 * ecs_query_set_group and ecs_query_next.
 * 
 * @param it The query iterator.
 * @param group_id The group to iterate.
 */
FLECS_API
void ecs_query_set_group(
    ecs_iter_t *it,
    uint64_t group_id);

/** Get context of query group.
 * This operation returns the context of a query group as returned by the 
 * on_group_create callback.
 * 
 * @param query The query.
 * @param group_id The group for which to obtain the context.
 * @return The group context, NULL if the group doesn't exist.
 */
FLECS_API
void* ecs_query_get_group_ctx(
    const ecs_query_t *query,
    uint64_t group_id);

/** Get information about query group.
 * This operation returns information about a query group, including the group
 * context returned by the on_group_create callback.
 * 
 * @param query The query.
 * @param group_id The group for which to obtain the group info.
 * @return The group info, NULL if the group doesn't exist.
 */
FLECS_API
const ecs_query_group_info_t* ecs_query_get_group_info(
    const ecs_query_t *query,
    uint64_t group_id);

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
    const ecs_query_t *query);

/** Convert query to string.
 *
 * @param query The query.
 * @return The query string.
 */
FLECS_API
char* ecs_query_str(
    const ecs_query_t *query);

/** Returns number of tables query matched with.
 *
 * @param query The query.
 * @return The number of matched tables.
 */
FLECS_API
int32_t ecs_query_table_count(
    const ecs_query_t *query);

/** Returns number of empty tables query matched with.
 *
 * @param query The query.
 * @return The number of matched empty tables.
 */
FLECS_API
int32_t ecs_query_empty_table_count(
    const ecs_query_t *query);

/** Returns number of entities query matched with.
 * This operation iterates all non-empty tables in the query cache to find the
 * total number of entities.
 *
 * @param query The query.
 * @return The number of matched entities.
 */
FLECS_API
int32_t ecs_query_entity_count(
    const ecs_query_t *query);

/** @} */

/**
 * @defgroup observer Observers
 * @brief Functions for working with events and observers.
 * @{
 */

typedef struct ecs_event_desc_t {
    /** The event id. Only triggers for the specified event will be notified */
    ecs_entity_t event;

    /** Component ids. Only triggers with a matching component id will be
     * notified. Observers are guaranteed to get notified once, even if they
     * match more than one id. */
    const ecs_type_t *ids;

    /** The table for which to notify. */
    ecs_table_t *table;

    /** Optional 2nd table to notify. This can be used to communicate the
     * previous or next table, in case an entity is moved between tables. */
    ecs_table_t *other_table;

    /** Limit notified entities to ones starting from offset (row) in table */
    int32_t offset;

    /** Limit number of notified entities to count. offset+count must be less
     * than the total number of entities in the table. If left to 0, it will be
     * automatically determined by doing ecs_table_count(table) - offset. */
    int32_t count;

    /** Single-entity alternative to setting table / offset / count */
    ecs_entity_t entity;

    /** Optional context. Assigned to iter param member */
    const void *param;

    /** Observable (usually the world) */
    ecs_poly_t *observable;

    /** Event flags */
    ecs_flags32_t flags;
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
 * @brief Functions for working with `ecs_iter_t`.
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
 * sources) the operation will return 0.
 * 
 * To determine the number of matched entities, the operation iterates the
 * iterator until it yields no more results.
 * 
 * @param it The iterator.
 * @return True if iterator has more results, false if not.
 */
FLECS_API
int32_t ecs_iter_count(
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

/** Get first matching entity from iterator.
 * After this operation the application should treat the iterator as if it has
 * been iterated until completion.
 * 
 * @param it The iterator.
 * @return The first matching entity, or 0 if no entities were matched.
 */
FLECS_API
ecs_entity_t ecs_iter_first(
    ecs_iter_t *it);

/** Set value for iterator variable.
 * This constrains the iterator to return only results for which the variable
 * equals the specified value. The default value for all variables is 
 * EcsWildcard, which means the variable can assume any value.
 * 
 * Example:
 * 
 * // Rule that matches (Eats, *)
 * ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
 *   .terms = {
 *     { .first.id = Eats, .second.name = "_Food" }
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

/** Obtain data for a query field.
 * This operation retrieves a pointer to an array of data that belongs to the
 * term in the query. The index refers to the location of the term in the query,
 * and starts counting from one.
 *
 * For example, the query "Position, Velocity" will return the Position array
 * for index 1, and the Velocity array for index 2.
 *
 * When the specified field is not owned by the entity this function returns a
 * pointer instead of an array. This happens when the source of a field is not
 * the entity being iterated, such as a shared component (from a prefab), a
 * component from a parent, or another entity. The ecs_field_is_self operation
 * can be used to test dynamically if a field is owned.
 *
 * The provided size must be either 0 or must match the size of the datatype
 * of the returned array. If the size does not match, the operation may assert.
 * The size can be dynamically obtained with ecs_field_size.
 *
 * @param it The iterator.
 * @param size The type size of the requested data.
 * @param index The index of the field in the iterator.
 * @return A pointer to the data of the field.
 */
FLECS_API
void* ecs_field_w_size(
    const ecs_iter_t *it,
    size_t size,
    int32_t index);

/** Test whether the field is readonly.
 * This operation returns whether the field is readonly. Readonly fields are
 * annotated with [in], or are added as a const type in the C++ API.
 *
 * @param it The iterator.
 * @param index The index of the field in the iterator.
 * @return Whether the field is readonly.
 */
FLECS_API
bool ecs_field_is_readonly(
    const ecs_iter_t *it,
    int32_t index);    

/** Test whether the field is writeonly.
 * This operation returns whether this is a writeonly field. Writeonly terms are
 * annotated with [out].
 * 
 * Serializers are not required to serialize the values of a writeonly field.
 *
 * @param it The iterator.
 * @param index The index of the field in the iterator.
 * @return Whether the field is writeonly.
 */
FLECS_API
bool ecs_field_is_writeonly(
    const ecs_iter_t *it,
    int32_t index);

/** Test whether field is set.
 * 
 * @param it The iterator.
 * @param index The index of the field in the iterator.
 * @return Whether the field is set.
 */
FLECS_API
bool ecs_field_is_set(
    const ecs_iter_t *it,
    int32_t index); 

/** Return id matched for field.
 * 
 * @param it The iterator.
 * @param index The index of the field in the iterator.
 * @return The id matched for the field.
 */
FLECS_API
ecs_id_t ecs_field_id(
    const ecs_iter_t *it,
    int32_t index);

/** Return index of matched table column.
 * This function only returns column indices for fields that have been matched
 * on the the $this variable. Fields matched on other tables will return -1.
 * 
 * @param it The iterator.
 * @param index The index of the field in the iterator.
 * @return The index of the matched column, -1 if not matched.
 */
FLECS_API
int32_t ecs_field_column_index(
    const ecs_iter_t *it,
    int32_t index);

/** Return field source.
 * The field source is the entity on which the field was matched.
 * 
 * @param it The iterator.
 * @param index The index of the field in the iterator.
 * @return The source for the field.
 */
FLECS_API
ecs_entity_t ecs_field_src(
    const ecs_iter_t *it,
    int32_t index);

/** Return field type size.
 * Return type size of the data returned by field. Returns 0 if field has no 
 * data.
 * 
 * @param it The iterator.
 * @param index The index of the field in the iterator.
 * @return The type size for the field.
 */
FLECS_API
size_t ecs_field_size(
    const ecs_iter_t *it,
    int32_t index);

/** Test whether the field is matched on self.
 * This operation returns whether the field is matched on the currently iterated
 * entity. This function will return false when the field is owned by another
 * entity, such as a parent or a prefab.
 * 
 * When this operation returns false, the field must be accessed as a single 
 * value instead of an array. Fields for which this operation returns true
 * return arrays with it->count values.
 *
 * @param it The iterator.
 * @param index The index of the field in the iterator.
 * @return Whether the field is matched on self.
 */
FLECS_API
bool ecs_field_is_self(
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


/** @} */

/**
 * @defgroup tables Tables
 * @brief Functions for working with `ecs_table_t`.
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

/** Get column from table.
 * This operation returns the component array for the provided index.
 * 
 * @param table The table.
 * @param index The index of the column (corresponds with element in type).
 * @param offset The index of the first row to return (0 for entire column).
 * @return The component array, or NULL if the index is not a component.
 */
FLECS_API
void* ecs_table_get_column(
    const ecs_table_t *table,
    int32_t index,
    int32_t offset);

/** Get column size from table.
 * This operation returns the component size for the provided index.
 * 
 * @param table The table.
 * @param index The index of the column (corresponds with element in type).
 * @return The component size, or 0 if the index is not a component.
 */
FLECS_API
size_t ecs_table_get_column_size(
    const ecs_table_t *table,
    int32_t index);

/** Get column index for id.
 * This operation returns the index for an id in the table's type.
 * 
 * @param world The world.
 * @param table The table.
 * @param id The id.
 * @return The index of the id in the table type, or -1 if not found.
 */
FLECS_API
int32_t ecs_table_get_index(
    const ecs_world_t *world,
    const ecs_table_t *table,
    ecs_id_t id);

/** Test if table has id.
 * Same as ecs_table_get_index(world, table, id) != -1.
 * 
 * @param world The world.
 * @param table The table.
 * @param id The id.
 * @return True if the table has the id, false if the table doesn't.
 */
FLECS_API
bool ecs_table_has_id(
    const ecs_world_t *world,
    const ecs_table_t *table,
    ecs_id_t id);

/** Get column from table by component id.
 * This operation returns the component array for the provided component  id.
 * 
 * @param table The table.
 * @param id The component id for the column.
 * @param offset The index of the first row to return (0 for entire column).
 * @return The component array, or NULL if the index is not a component.
 */
FLECS_API
void* ecs_table_get_id(
    const ecs_world_t *world,
    const ecs_table_t *table,
    ecs_id_t id,
    int32_t offset);

/** Return depth for table in tree for relationship rel.
 * Depth is determined by counting the number of targets encountered while 
 * traversing up the relationship tree for rel. Only acyclic relationships are
 * supported.
 * 
 * @param world The world.
 * @param table The table.
 * @param rel The relationship.
 * @return The depth of the table in the tree.
 */
FLECS_API
int32_t ecs_table_get_depth(
    const ecs_world_t *world,
    const ecs_table_t *table,
    ecs_entity_t rel);

/** Get storage type for table.
 *
 * @param table The table.
 * @return The storage type of the table (components only).
 */
FLECS_API
ecs_table_t* ecs_table_get_storage_table(
    const ecs_table_t *table);

/** Convert index in table type to index in table storage type. */
FLECS_API
int32_t ecs_table_type_to_storage_index(
    const ecs_table_t *table,
    int32_t index);

/** Convert index in table storage type to index in table type. */
FLECS_API
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

/** Find table from id array. 
 * This operation finds or creates a table with the specified array of 
 * (component) ids. The ids in the array must be sorted, and it may not contain
 * duplicate elements.
 * 
 * @param world The world.
 * @param ids The id array.
 * @param id_count The number of elements in the id array.
 * @return The table with the specified (component) ids.
 */
FLECS_API
ecs_table_t* ecs_table_find(
    ecs_world_t *world,
    const ecs_id_t *ids,
    int32_t id_count);

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
 * When a table is locked, modifications to it will throw an assert. When the 
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

/** Swaps two elements inside the table. This is useful for implementing custom
 * table sorting algorithms.
 * @param world The world
 * @param table The table to swap elements in
 * @param row_1 Table element to swap with row_2
 * @param row_2 Table element to swap with row_1
*/
FLECS_API
void ecs_table_swap_rows(
    ecs_world_t* world,
    ecs_table_t* table,
    int32_t row_1,
    int32_t row_2);

/** Commit (move) entity to a table.
 * This operation moves an entity from its current table to the specified
 * table. This may cause the following actions:
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
    const ecs_record_t *r,
    int32_t column,
    size_t c_size);

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
 * If the provided id has the form (*, tgt) the operation takes linear time. The
 * reason for this is that ids for an target are not packed together, as they
 * are sorted relationship first.
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

/** Search for component/relationship id in table type starting from an offset.
 * This operation is the same as ecs_search_offset, but has the additional
 * capability of traversing relationships to find a component. For example, if
 * an application wants to find a component for either the provided table or a
 * prefab (using the IsA relationship) of that table, it could use the operation 
 * like this:
 * 
 * int32_t index = ecs_search_relation(
 *   world,            // the world
 *   table,            // the table
 *   0,                // offset 0
 *   ecs_id(Position), // the component id
 *   EcsIsA,           // the relationship to traverse
 *   0,                // start at depth 0 (the table itself)
 *   0,                // no depth limit
 *   NULL,             // (optional) entity on which component was found
 *   NULL,             // see above
 *   NULL);            // internal type with information about matched id
 * 
 * The operation searches depth first. If a table type has 2 IsA relationships, the
 * operation will first search the IsA tree of the first relationship.
 * 
 * When choosing betwen ecs_search, ecs_search_offset and ecs_search_relation,
 * the simpler the function the better its performance.
 * 
 * @param world The world.
 * @param table The table.
 * @param offset Offset from where to start searching.
 * @param id The id to search for.
 * @param rel The relationship to traverse (optional).
 * @param flags Whether to search EcsSelf and/or EcsUp.
 * @param subject_out If provided, it will be set to the matched entity.
 * @param id_out If provided, it will be set to the found id (optional).
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
    ecs_flags32_t flags, /* EcsSelf and/or EcsUp */
    ecs_entity_t *subject_out,
    ecs_id_t *id_out,
    struct ecs_table_record_t **tr_out);

/** @} */

/**
 * @defgroup values Values
 * @brief Construct, destruct, copy and move dynamically created values.
 * @{
 */

/** Construct a value in existing storage 
 *
 * @param world The world.
 * @param type The type of the value to create.
 * @param ptr Pointer to a value of type 'type'
 * @return Zero if success, nonzero if failed.
 */
FLECS_API
int ecs_value_init(
    const ecs_world_t *world,
    ecs_entity_t type,
    void *ptr);

/** Construct a value in existing storage 
 *
 * @param world The world.
 * @param ti The type info of the type to create.
 * @param ptr Pointer to a value of type 'type'
 * @return Zero if success, nonzero if failed.
 */
FLECS_API
int ecs_value_init_w_type_info(
    const ecs_world_t *world,
    const ecs_type_info_t *ti,
    void *ptr);

/** Construct a value in new storage 
 * 
 * @param world The world.
 * @param type The type of the value to create.
 * @return Pointer to type if success, NULL if failed.
 */
FLECS_API
void* ecs_value_new(
    ecs_world_t *world,
    ecs_entity_t type);

/** Construct a value in new storage 
 * 
 * @param world The world.
 * @param ti The type info of the type to create.
 * @return Pointer to type if success, NULL if failed.
 */
void* ecs_value_new_w_type_info(
    ecs_world_t *world,
    const ecs_type_info_t *ti);

/** Destruct a value 
 * 
 * @param world The world.
 * @param ti Type info of the value to destruct.
 * @param ptr Pointer to constructed value of type 'type'.
 * @return Zero if success, nonzero if failed. 
 */
int ecs_value_fini_w_type_info(
    const ecs_world_t *world,
    const ecs_type_info_t *ti,
    void *ptr);

/** Destruct a value 
 * 
 * @param world The world.
 * @param type The type of the value to destruct.
 * @param ptr Pointer to constructed value of type 'type'.
 * @return Zero if success, nonzero if failed. 
 */
FLECS_API
int ecs_value_fini(
    const ecs_world_t *world,
    ecs_entity_t type,
    void* ptr);

/** Destruct a value, free storage
 * 
 * @param world The world.
 * @param type The type of the value to destruct.
 * @return Zero if success, nonzero if failed. 
 */
FLECS_API
int ecs_value_free(
    ecs_world_t *world,
    ecs_entity_t type,
    void* ptr);

/** Copy value.
 * 
 * @param world The world.
 * @param ti Type info of the value to copy.
 * @param dst Pointer to the storage to copy to.
 * @param src Pointer to the value to copy.
 * @return Zero if success, nonzero if failed. 
 */
FLECS_API
int ecs_value_copy_w_type_info(
    const ecs_world_t *world,
    const ecs_type_info_t *ti,
    void* dst,
    const void *src);

/** Copy value.
 * 
 * @param world The world.
 * @param type The type of the value to copy.
 * @param dst Pointer to the storage to copy to.
 * @param src Pointer to the value to copy.
 * @return Zero if success, nonzero if failed. 
 */
FLECS_API
int ecs_value_copy(
    const ecs_world_t *world,
    ecs_entity_t type,
    void* dst,
    const void *src);

/** Move value.
 * 
 * @param world The world.
 * @param ti Type info of the value to move.
 * @param dst Pointer to the storage to move to.
 * @param src Pointer to the value to move.
 * @return Zero if success, nonzero if failed. 
 */
int ecs_value_move_w_type_info(
    const ecs_world_t *world,
    const ecs_type_info_t *ti,
    void* dst,
    void *src);

/** Move value.
 * 
 * @param world The world.
 * @param type The type of the value to move.
 * @param dst Pointer to the storage to move to.
 * @param src Pointer to the value to move.
 * @return Zero if success, nonzero if failed. 
 */
int ecs_value_move(
    const ecs_world_t *world,
    ecs_entity_t type,
    void* dst,
    void *src);

/** Move construct value.
 * 
 * @param world The world.
 * @param ti Type info of the value to move.
 * @param dst Pointer to the storage to move to.
 * @param src Pointer to the value to move.
 * @return Zero if success, nonzero if failed. 
 */
int ecs_value_move_ctor_w_type_info(
    const ecs_world_t *world,
    const ecs_type_info_t *ti,
    void* dst,
    void *src);

/** Move construct value.
 * 
 * @param world The world.
 * @param type The type of the value to move.
 * @param dst Pointer to the storage to move to.
 * @param src Pointer to the value to move.
 * @return Zero if success, nonzero if failed. 
 */
int ecs_value_move_ctor(
    const ecs_world_t *world,
    ecs_entity_t type,
    void* dst,
    void *src);

/** @} */

/** @} */

/**
 * @defgroup c_addons Addons
 * @brief C APIs for addons.
 * 
 * \ingroup c
 * 
 * @{
 * @}
 */

/**
 * @file addons/flecs_c.h
 * @brief Extends the core API with convenience macros for C applications.
 */

#ifndef FLECS_C_
#define FLECS_C_

/**
 * @defgroup flecs_c Macro API
 * @brief Convenience macro's for C API
 * 
 * \ingroup c
 * @{
 */

/**
 * @defgroup flecs_c_creation Creation macro's
 * @brief Convenience macro's for creating entities, components and observers
 * @{
 */

/* Use for declaring entity, tag, prefab / any other entity identifier */
#define ECS_DECLARE(id)\
    ecs_entity_t id, ecs_id(id)

/** Forward declare an entity. */
#define ECS_ENTITY_DECLARE ECS_DECLARE

/** Define a forward declared entity.
 * 
 * Example:
 *   ECS_ENTITY_DEFINE(world, MyEntity, Position, Velocity);
 */
#define ECS_ENTITY_DEFINE(world, id_, ...) \
    { \
        ecs_entity_desc_t desc = {0}; \
        desc.id = id_; \
        desc.name = #id_; \
        desc.add_expr = #__VA_ARGS__; \
        id_ = ecs_entity_init(world, &desc); \
        ecs_id(id_) = id_; \
        ecs_assert(id_ != 0, ECS_INVALID_PARAMETER, NULL); \
    } \
    (void)id_; \
    (void)ecs_id(id_)

/** Declare & define an entity.
 *
 * Example:
 *   ECS_ENTITY(world, MyEntity, Position, Velocity);
 */
#define ECS_ENTITY(world, id, ...) \
    ecs_entity_t ecs_id(id); \
    ecs_entity_t id = 0; \
    ECS_ENTITY_DEFINE(world, id, __VA_ARGS__)

/** Forward declare a tag. */
#define ECS_TAG_DECLARE ECS_DECLARE

/** Define a forward declared tag.
 * 
 * Example:
 *   ECS_TAG_DEFINE(world, MyTag);
 */
#define ECS_TAG_DEFINE(world, id) ECS_ENTITY_DEFINE(world, id, 0)

/** Declare & define a tag.
 *
 * Example:
 *   ECS_TAG(world, MyTag);
 */
#define ECS_TAG(world, id) ECS_ENTITY(world, id, 0)

/** Forward declare a prefab. */
#define ECS_PREFAB_DECLARE ECS_DECLARE

/** Define a forward declared prefab.
 * 
 * Example:
 *   ECS_PREFAB_DEFINE(world, MyPrefab, Position, Velocity);
 */
#define ECS_PREFAB_DEFINE(world, id, ...) ECS_ENTITY_DEFINE(world, id, Prefab, __VA_ARGS__)

/** Declare & define a prefab.
 *
 * Example:
 *   ECS_PREFAB(world, MyPrefab, Position, Velocity);
 */
#define ECS_PREFAB(world, id, ...) ECS_ENTITY(world, id, Prefab, __VA_ARGS__)

/** Forward declare a component. */
#define ECS_COMPONENT_DECLARE(id)         ecs_entity_t ecs_id(id)

/** Define a forward declared component.
 * 
 * Example:
 *   ECS_COMPONENT_DEFINE(world, Position);
 */
#define ECS_COMPONENT_DEFINE(world, id_) \
    {\
        ecs_component_desc_t desc = {0}; \
        ecs_entity_desc_t edesc = {0}; \
        edesc.id = ecs_id(id_); \
        edesc.use_low_id = true; \
        edesc.name = #id_; \
        edesc.symbol = #id_; \
        desc.entity = ecs_entity_init(world, &edesc); \
        desc.type.size = ECS_SIZEOF(id_); \
        desc.type.alignment = ECS_ALIGNOF(id_); \
        ecs_id(id_) = ecs_component_init(world, &desc);\
    }\
    ecs_assert(ecs_id(id_) != 0, ECS_INVALID_PARAMETER, NULL)

/** Declare & define a component.
 *
 * Example:
 *   ECS_COMPONENT(world, Position);
 */
#define ECS_COMPONENT(world, id)\
    ecs_entity_t ecs_id(id) = 0;\
    ECS_COMPONENT_DEFINE(world, id);\
    (void)ecs_id(id)

/* Forward declare an observer. */
#define ECS_OBSERVER_DECLARE(id)         ecs_entity_t ecs_id(id)

/** Define a forward declared observer.
 * 
 * Example:
 *   ECS_OBSERVER_DEFINE(world, AddPosition, EcsOnAdd, Position);
 */
#define ECS_OBSERVER_DEFINE(world, id_, kind, ...)\
    {\
        ecs_observer_desc_t desc = {0};\
        ecs_entity_desc_t edesc = {0}; \
        edesc.id = ecs_id(id_); \
        edesc.name = #id_; \
        desc.entity = ecs_entity_init(world, &edesc); \
        desc.callback = id_;\
        desc.filter.expr = #__VA_ARGS__;\
        desc.events[0] = kind;\
        ecs_id(id_) = ecs_observer_init(world, &desc);\
        ecs_assert(ecs_id(id_) != 0, ECS_INVALID_PARAMETER, NULL);\
    }

/** Declare & define an observer.
 *
 * Example:
 *   ECS_OBSERVER(world, AddPosition, EcsOnAdd, Position);
 */
#define ECS_OBSERVER(world, id, kind, ...)\
    ecs_entity_t ecs_id(id) = 0; \
    ECS_OBSERVER_DEFINE(world, id, kind, __VA_ARGS__);\
    ecs_entity_t id = ecs_id(id);\
    (void)ecs_id(id);\
    (void)id

/** Shorthand for creating an entity with ecs_entity_init.
 *
 * Example:
 *   ecs_entity(world, {
 *     .name = "MyEntity"
 *   });
 */
#define ecs_entity(world, ...)\
    ecs_entity_init(world, &(ecs_entity_desc_t) __VA_ARGS__ )

/** Shorthand for creating a component with ecs_component_init.
 *
 * Example:
 *   ecs_component(world, {
 *     .type.size = 4,
 *     .type.alignment = 4
 *   });
 */
#define ecs_component(world, ...)\
    ecs_component_init(world, &(ecs_component_desc_t) __VA_ARGS__ )

/** Shorthand for creating a component from a type.
 *
 * Example:
 *   ecs_component_t(world, Position);
 */
#define ecs_component_t(world, T)\
    ecs_component_init(world, &(ecs_component_desc_t) { \
        .entity = ecs_entity(world, { \
            .name = #T, \
            .symbol = #T, \
            .use_low_id = true \
        }), \
        .type.size = ECS_SIZEOF(T), \
        .type.alignment = ECS_ALIGNOF(T) \
    })

/** Shorthand for creating a filter with ecs_filter_init.
 *
 * Example:
 *   ecs_filter(world, {
 *     .terms = {{ ecs_id(Position) }}
 *   });
 */
#define ecs_filter(world, ...)\
    ecs_filter_init(world, &(ecs_filter_desc_t) __VA_ARGS__ )

/** Shorthand for creating a query with ecs_query_init.
 *
 * Example:
 *   ecs_query(world, {
 *     .filter.terms = {{ ecs_id(Position) }}
 *   });
 */
#define ecs_query(world, ...)\
    ecs_query_init(world, &(ecs_query_desc_t) __VA_ARGS__ )

/** Shorthand for creating an observer with ecs_observer_init.
 *
 * Example:
 *   ecs_observer(world, {
 *     .filter.terms = {{ ecs_id(Position) }},
 *     .events = { EcsOnAdd },
 *     .callback = AddPosition
 *   });
 */
#define ecs_observer(world, ...)\
    ecs_observer_init(world, &(ecs_observer_desc_t) __VA_ARGS__ )

/** @} */

/**
 * @defgroup flecs_c_type_safe Type Safe API
 * @brief Macro's that wrap around core functions to provide a "type safe" API in C
 * @{
 */

/**
 * @defgroup flecs_c_entities Entity API
 * @{
 */

/**
 * @defgroup flecs_c_creation_deletion Creation & Deletion
 * @{
 */

#define ecs_new(world, T) ecs_new_w_id(world, ecs_id(T))

#define ecs_new_w_pair(world, first, second)\
    ecs_new_w_id(world, ecs_pair(first, second))

#define ecs_bulk_new(world, component, count)\
    ecs_bulk_new_w_id(world, ecs_id(component), count)

#define ecs_new_entity(world, n)\
    ecs_entity_init(world, &(ecs_entity_desc_t){\
        .name = n,\
    })

#define ecs_new_prefab(world, n)\
    ecs_entity_init(world, &(ecs_entity_desc_t){\
        .name = n,\
        .add = {EcsPrefab}\
    })

#define ecs_delete_children(world, parent)\
    ecs_delete_with(world, ecs_pair(EcsChildOf, parent))

/** @} */

/**
 * @defgroup flecs_c_adding_removing Adding & Removing
 * @{
 */

#define ecs_add(world, entity, T)\
    ecs_add_id(world, entity, ecs_id(T))

#define ecs_add_pair(world, subject, first, second)\
    ecs_add_id(world, subject, ecs_pair(first, second))


#define ecs_remove(world, entity, T)\
    ecs_remove_id(world, entity, ecs_id(T))

#define ecs_remove_pair(world, subject, first, second)\
    ecs_remove_id(world, subject, ecs_pair(first, second))


#define ecs_override(world, entity, T)\
    ecs_override_id(world, entity, ecs_id(T))

#define ecs_override_pair(world, subject, first, second)\
    ecs_override_id(world, subject, ecs_pair(first, second))

/** @} */

/**
 * @defgroup flecs_c_getting_setting Getting & Setting
 * @{
 */

#define ecs_set_ptr(world, entity, component, ptr)\
    ecs_set_id(world, entity, ecs_id(component), sizeof(component), ptr)

#define ecs_set(world, entity, component, ...)\
    ecs_set_id(world, entity, ecs_id(component), sizeof(component), &(component)__VA_ARGS__)

#define ecs_set_pair(world, subject, First, second, ...)\
    ecs_set_id(world, subject,\
        ecs_pair(ecs_id(First), second),\
        sizeof(First), &(First)__VA_ARGS__)

#define ecs_set_pair_second(world, subject, first, Second, ...)\
    ecs_set_id(world, subject,\
        ecs_pair(first, ecs_id(Second)),\
        sizeof(Second), &(Second)__VA_ARGS__)

#define ecs_set_pair_object ecs_set_pair_second

#define ecs_set_override(world, entity, T, ...)\
    ecs_add_id(world, entity, ECS_OVERRIDE | ecs_id(T));\
    ecs_set(world, entity, T, __VA_ARGS__)

#define ecs_emplace(world, entity, T)\
    (ECS_CAST(T*, ecs_emplace_id(world, entity, ecs_id(T))))

#define ecs_emplace_pair(world, entity, First, second)\
    (ECS_CAST(First*, ecs_emplace_id(world, entity, ecs_pair_t(First, second))))

#define ecs_get(world, entity, T)\
    (ECS_CAST(const T*, ecs_get_id(world, entity, ecs_id(T))))

#define ecs_get_pair(world, subject, First, second)\
    (ECS_CAST(const First*, ecs_get_id(world, subject,\
        ecs_pair(ecs_id(First), second))))

#define ecs_get_pair_second(world, subject, first, Second)\
    (ECS_CAST(const Second*, ecs_get_id(world, subject,\
        ecs_pair(first, ecs_id(Second)))))

#define ecs_get_pair_object ecs_get_pair_second

#define ecs_record_get(world, record, T)\
    (ECS_CAST(const T*, ecs_record_get_id(world, record, ecs_id(T))))

#define ecs_record_has(world, record, T)\
    (ecs_record_has_id(world, record, ecs_id(T)))

#define ecs_record_get_pair(world, record, First, second)\
    (ECS_CAST(const First*, ecs_record_get_id(world, record, \
        ecs_pair(ecs_id(First), second))))

#define ecs_record_get_pair_second(world, record, first, Second)\
    (ECS_CAST(const Second*, ecs_record_get_id(world, record,\
        ecs_pair(first, ecs_id(Second)))))

#define ecs_record_get_mut(world, record, T)\
    (ECS_CAST(T*, ecs_record_get_mut_id(world, record, ecs_id(T))))

#define ecs_record_get_mut_pair(world, record, First, second)\
    (ECS_CAST(First*, ecs_record_get_mut_id(world, record, \
        ecs_pair(ecs_id(First), second))))

#define ecs_record_get_mut_pair_second(world, record, first, Second)\
    (ECS_CAST(Second*, ecs_record_get_mut_id(world, record,\
        ecs_pair(first, ecs_id(Second)))))

#define ecs_record_get_mut_pair_object ecs_record_get_mut_pair_second

#define ecs_ref_init(world, entity, T)\
    ecs_ref_init_id(world, entity, ecs_id(T))

#define ecs_ref_get(world, ref, T)\
    (ECS_CAST(const T*, ecs_ref_get_id(world, ref, ecs_id(T))))

#define ecs_get_mut(world, entity, T)\
    (ECS_CAST(T*, ecs_get_mut_id(world, entity, ecs_id(T))))

#define ecs_get_mut_pair(world, subject, First, second)\
    (ECS_CAST(First*, ecs_get_mut_id(world, subject,\
        ecs_pair(ecs_id(First), second))))

#define ecs_get_mut_pair_second(world, subject, first, Second)\
    (ECS_CAST(Second*, ecs_get_mut_id(world, subject,\
        ecs_pair(first, ecs_id(Second)))))

#define ecs_get_mut_pair_object ecs_get_mut_pair_second

#define ecs_modified(world, entity, component)\
    ecs_modified_id(world, entity, ecs_id(component))

#define ecs_modified_pair(world, subject, first, second)\
    ecs_modified_id(world, subject, ecs_pair(first, second))

/** @} */

/**
 * @defgroup flecs_c_singletons Singletons
 * @{
 */

#define ecs_singleton_add(world, comp)\
    ecs_add(world, ecs_id(comp), comp)

#define ecs_singleton_remove(world, comp)\
    ecs_remove(world, ecs_id(comp), comp)

#define ecs_singleton_get(world, comp)\
    ecs_get(world, ecs_id(comp), comp)

#define ecs_singleton_set_ptr(world, comp, ptr)\
    ecs_set_ptr(world, ecs_id(comp), comp, ptr)

#define ecs_singleton_set(world, comp, ...)\
    ecs_set(world, ecs_id(comp), comp, __VA_ARGS__)

#define ecs_singleton_get_mut(world, comp)\
    ecs_get_mut(world, ecs_id(comp), comp)

#define ecs_singleton_modified(world, comp)\
    ecs_modified(world, ecs_id(comp), comp)

/** @} */

/**
 * @defgroup flecs_c_has Has, Owns, Shares
 * @{
 */

#define ecs_has(world, entity, T)\
    ecs_has_id(world, entity, ecs_id(T))

#define ecs_has_pair(world, entity, first, second)\
    ecs_has_id(world, entity, ecs_pair(first, second))

#define ecs_owns_pair(world, entity, first, second)\
    ecs_owns_id(world, entity, ecs_pair(first, second))

#define ecs_owns(world, entity, T)\
    ecs_owns_id(world, entity, ecs_id(T))

#define ecs_shares_id(world, entity, id)\
    (ecs_search_relation(world, ecs_get_table(world, entity), 0, ecs_id(id), \
        EcsIsA, 1, 0, 0, 0, 0) != -1)

#define ecs_shares_pair(world, entity, first, second)\
    (ecs_shares_id(world, entity, ecs_pair(first, second)))

#define ecs_shares(world, entity, T)\
    (ecs_shares_id(world, entity, ecs_id(T)))

/** @} */

/**
 * @defgroup flecs_c_enable_disable Enabling & Disabling
 * @{
 */

#define ecs_enable_component(world, entity, T, enable)\
    ecs_enable_id(world, entity, ecs_id(T), enable)

#define ecs_is_enabled_component(world, entity, T)\
    ecs_is_enabled_id(world, entity, ecs_id(T))

#define ecs_enable_pair(world, entity, First, second, enable)\
    ecs_enable_id(world, entity, ecs_pair(ecs_id(First), second), enable)

#define ecs_is_enabled_pair(world, entity, First, second)\
    ecs_is_enabled_id(world, entity, ecs_pair(ecs_id(First), second))

/** @} */

/**
 * @defgroup flecs_c_entity_names Entity Names
 * @{
 */

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

/** @} */

/** @} */

/**
 * @defgroup flecs_c_components Component API
 * @{
 */

#define ecs_set_hooks(world, T, ...)\
    ecs_set_hooks_id(world, ecs_id(T), &(ecs_type_hooks_t)__VA_ARGS__)

#define ecs_get_hooks(world, T)\
    ecs_get_hooks_id(world, ecs_id(T));

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

/** Declare component hooks.
 * Example:
 *   ECS_ON_SET(MyType, ptr, { printf("%d\n", ptr->value); });
 */
#define ECS_ON_ADD(type, ptr, ...)\
    ECS_HOOK_IMPL(type, ecs_on_add(type), ptr, __VA_ARGS__)
#define ECS_ON_REMOVE(type, ptr, ...)\
    ECS_HOOK_IMPL(type, ecs_on_remove(type), ptr, __VA_ARGS__)
#define ECS_ON_SET(type, ptr, ...)\
    ECS_HOOK_IMPL(type, ecs_on_set(type), ptr, __VA_ARGS__)

/* Map from typename to function name of component lifecycle action */
#define ecs_ctor(type) type##_ctor
#define ecs_dtor(type) type##_dtor
#define ecs_copy(type) type##_copy
#define ecs_move(type) type##_move
#define ecs_on_set(type) type##_on_set
#define ecs_on_add(type) type##_on_add
#define ecs_on_remove(type) type##_on_remove

/** @} */

/**
 * @defgroup flecs_c_ids Id API
 * @{
 */

#define ecs_count(world, type)\
    ecs_count_id(world, ecs_id(type))

/** @} */

/**
 * @defgroup flecs_c_iterators Iterator API
 * @{
 */

#define ecs_field(it, T, index)\
    (ECS_CAST(T*, ecs_field_w_size(it, sizeof(T), index)))

/** @} */

/**
 * @defgroup flecs_c_tables Table API
 * @{
 */

#define ecs_table_get(world, table, T, offset)\
    (ECS_CAST(T*, ecs_table_get_id(world, table, ecs_id(T), offset)))

#define ecs_table_get_pair(world, table, First, second, offset)\
    (ECS_CAST(First*, ecs_table_get_id(world, table, ecs_pair(ecs_id(First), second), offset)))

#define ecs_table_get_pair_second(world, table, first, Second, offset)\
    (ECS_CAST(Second*, ecs_table_get_id(world, table, ecs_pair(first, ecs_id(Second)), offset)))

/** @} */

/**
 * @defgroup flecs_c_values Value API
 * @{
 */

#define ecs_value(T, ptr) ((ecs_value_t){ecs_id(T), ptr})
#define ecs_value_new_t(world, T) ecs_value_new(world, ecs_id(T))

/** @} */

/** @} */

/**
 * @defgroup flecs_c_table_sorting Table sorting
 * @brief Convenience macro's for sorting tables.
 * 
 * @{
 */
#define ecs_sort_table(id) ecs_id(id##_sort_table)

#define ecs_compare(id) ecs_id(id##_compare_fn)

/* Declare efficient table sorting operation that uses provided compare function.
 * For best results use LTO or make the function body visible in the same compilation unit.
 * Variadic arguments are prepended before generated functions, use it to declare static
 *   or exported functions.
 * Parameters of the comparison function:
 *   ecs_entity_t e1, const void* ptr1,
 *   ecs_entity_t e2, const void* ptr2
 * Parameters of the sort functions:
 *   ecs_world_t *world
 *   ecs_table_t *table
 *   ecs_entity_t *entities
 *   void *ptr
 *   int32_t elem_size
 *   int32_t lo
 *   int32_t hi
 *   ecs_order_by_action_t order_by - Pointer to the original comparison function. You are not supposed to use it.
 * Example:
 *   int CompareMyType(ecs_entity_t e1, const void* ptr1, ecs_entity_t e2, const void* ptr2) { const MyType* p1 = ptr1; const MyType* p2 = ptr2; return p1->value - p2->value; }
 *   ECS_SORT_TABLE_WITH_COMPARE(MyType, MyCustomCompare, CompareMyType)
 */
#define ECS_SORT_TABLE_WITH_COMPARE(id, op_name, compare_fn, ...) \
    static int32_t ECS_CONCAT(op_name, _partition)( \
        ecs_world_t *world, \
        ecs_table_t *table, \
        ecs_entity_t *entities, \
        void *ptr, \
        int32_t elem_size, \
        int32_t lo, \
        int32_t hi, \
        ecs_order_by_action_t order_by) \
    { \
        (void)(order_by); \
        int32_t p = (hi + lo) / 2; \
        void *pivot = ECS_ELEM(ptr, elem_size, p); \
        ecs_entity_t pivot_e = entities[p]; \
        int32_t i = lo - 1, j = hi + 1; \
        void *el; \
    repeat: \
        { \
            do { \
                i ++; \
                el = ECS_ELEM(ptr, elem_size, i); \
            } while ( compare_fn(entities[i], el, pivot_e, pivot) < 0); \
            do { \
                j --; \
                el = ECS_ELEM(ptr, elem_size, j); \
            } while ( compare_fn(entities[j], el, pivot_e, pivot) > 0); \
            if (i >= j) { \
                return j; \
            } \
            ecs_table_swap_rows(world, table, i, j); \
            if (p == i) { \
                pivot = ECS_ELEM(ptr, elem_size, j); \
                pivot_e = entities[j]; \
            } else if (p == j) { \
                pivot = ECS_ELEM(ptr, elem_size, i); \
                pivot_e = entities[i]; \
            } \
            goto repeat; \
        } \
    } \
    __VA_ARGS__ void op_name( \
        ecs_world_t *world, \
        ecs_table_t *table, \
        ecs_entity_t *entities, \
        void *ptr, \
        int32_t size, \
        int32_t lo, \
        int32_t hi, \
        ecs_order_by_action_t order_by) \
    { \
        if ((hi - lo) < 1)  { \
            return; \
        } \
        int32_t p = ECS_CONCAT(op_name, _partition)(world, table, entities, ptr, size, lo, hi, order_by); \
        op_name(world, table, entities, ptr, size, lo, p, order_by); \
        op_name(world, table, entities, ptr, size, p + 1, hi, order_by); \
    }

/* Declare efficient table sorting operation that uses default component comparison operator.
 * For best results use LTO or make the comparison operator visible in the same compilation unit.
 * Variadic arguments are prepended before generated functions, use it to declare static
 *   or exported functions.
 * Example:
 *   ECS_COMPARE(MyType, { const MyType* p1 = ptr1; const MyType* p2 = ptr2; return p1->value - p2->value; });
 *   ECS_SORT_TABLE(MyType)
 */
#define ECS_SORT_TABLE(id, ...) \
    ECS_SORT_TABLE_WITH_COMPARE(id, ecs_sort_table(id), ecs_compare(id), __VA_ARGS__)

/* Declare component comparison operations.
 * Parameters:
 *   ecs_entity_t e1, const void* ptr1,
 *   ecs_entity_t e2, const void* ptr2
 * Example:
 *   ECS_COMPARE(MyType, { const MyType* p1 = ptr1; const MyType* p2 = ptr2; return p1->value - p2->value; });
 */
#define ECS_COMPARE(id, ...) \
    int ecs_compare(id)(ecs_entity_t e1, const void* ptr1, ecs_entity_t e2, const void* ptr2) { \
        __VA_ARGS__ \
    }

/** @} */

/**
 * @defgroup flecs_c_misc Misc
 * @brief Misc convenience macro's.
 * 
 * @{
 */

#define ecs_isa(e)       ecs_pair(EcsIsA, e)
#define ecs_childof(e)   ecs_pair(EcsChildOf, e)
#define ecs_dependson(e) ecs_pair(EcsDependsOn, e)

#define ecs_query_new(world, q_expr)\
    ecs_query_init(world, &(ecs_query_desc_t){\
        .filter.expr = q_expr\
    })

#define ecs_rule_new(world, q_expr)\
    ecs_rule_init(world, &(ecs_filter_desc_t){\
        .expr = q_expr\
    })

/** @} */

/** @} */

#endif // FLECS_C_


#ifdef __cplusplus
}
#endif

/**
 * @file addons.h
 * @brief Include enabled addons.
 *
 * This file should only be included by the main flecs.h header.
 */

#ifndef FLECS_ADDONS_H
#define FLECS_ADDONS_H

/* Blacklist macros */
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
#ifdef FLECS_NO_MONITOR
#undef FLECS_MONITOR
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
#ifdef FLECS_NO_JOURNAL
#undef FLECS_JOURNAL
#endif

/* Always included, if disabled functions are replaced with dummy macros */
/**
 * @file addons/journal.h
 * @brief Journaling addon that logs API functions.
 *
 * The journaling addon traces API calls. The trace is formatted as runnable
 * C code, which allows for (partially) reproducing the behavior of an app
 * with the journaling trace.
 * 
 * The journaling addon is disabled by default. Enabling it can have a 
 * significant impact on performance.
 */

#ifdef FLECS_JOURNAL

#ifndef FLECS_LOG
#define FLECS_LOG
#endif

#ifndef FLECS_JOURNAL_H
#define FLECS_JOURNAL_H

/**
 * @defgroup c_addons_journal Journal
 * @brief Journaling addon (disabled by default).
 * 
 * \ingroup c_addons
 * @{
 */

/* Trace when log level is at or higher than level */
#define FLECS_JOURNAL_LOG_LEVEL (0)

#ifdef __cplusplus
extern "C" {
#endif

/* Journaling API, meant to be used by internals. */

typedef enum ecs_journal_kind_t {
    EcsJournalNew,
    EcsJournalMove,
    EcsJournalClear,
    EcsJournalDelete,
    EcsJournalDeleteWith,
    EcsJournalRemoveAll,
    EcsJournalTableEvents
} ecs_journal_kind_t;

FLECS_DBG_API
void flecs_journal_begin(
    ecs_world_t *world,
    ecs_journal_kind_t kind,
    ecs_entity_t entity,
    ecs_type_t *add,
    ecs_type_t *remove);

FLECS_DBG_API
void flecs_journal_end(void);

#define flecs_journal(...)\
    flecs_journal_begin(__VA_ARGS__);\
    flecs_journal_end();

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // FLECS_JOURNAL_H
#else
#define flecs_journal_begin(...)
#define flecs_journal_end(...)
#define flecs_journal(...)

/** @} */

#endif // FLECS_JOURNAL

/**
 * @file addons/log.h
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

/**
 * @defgroup c_addons_log Log
 * @brief Logging functions.
 * 
 * \ingroup c_addons
 * @{
 */

////////////////////////////////////////////////////////////////////////////////
//// Tracing
////////////////////////////////////////////////////////////////////////////////

FLECS_API
void ecs_deprecated_(
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
void ecs_log_push_(int32_t level);

/** Decrease log stack.
 * This operation decreases the indent_ value of the OS API and can be useful to
 * make nested behavior more visible.
 * 
 * @param level The log level.
 */
FLECS_API
void ecs_log_pop_(int32_t level);

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
//// Dummy macros for when logging is disabled
////////////////////////////////////////////////////////////////////////////////

#define ecs_deprecated_(file, line, msg)\
    (void)file;\
    (void)line;\
    (void)msg

#define ecs_log_push_(level)
#define ecs_log_pop_(level)
#define ecs_should_log(level) false

#define ecs_strerror(error_code)\
    (void)error_code

#endif // FLECS_LOG


////////////////////////////////////////////////////////////////////////////////
//// Logging functions (do nothing when logging is enabled)
////////////////////////////////////////////////////////////////////////////////

FLECS_API
void ecs_print_(
    int32_t level,
    const char *file,
    int32_t line,
    const char *fmt,
    ...);

FLECS_API
void ecs_printv_(
    int level,
    const char *file,
    int32_t line,
    const char *fmt,
    va_list args);

FLECS_API
void ecs_log_(
    int32_t level,
    const char *file,
    int32_t line,
    const char *fmt,
    ...);

FLECS_API
void ecs_logv_(
    int level,
    const char *file,
    int32_t line,
    const char *fmt,
    va_list args);

FLECS_API
void ecs_abort_(
    int32_t error_code,
    const char *file,
    int32_t line,
    const char *fmt,
    ...);

FLECS_API
bool ecs_assert_(
    bool condition,
    int32_t error_code,
    const char *condition_str,
    const char *file,
    int32_t line,
    const char *fmt,
    ...);

FLECS_API
void ecs_parser_error_(
    const char *name,
    const char *expr, 
    int64_t column,
    const char *fmt,
    ...);

FLECS_API
void ecs_parser_errorv_(
    const char *name,
    const char *expr, 
    int64_t column,
    const char *fmt,
    va_list args);


////////////////////////////////////////////////////////////////////////////////
//// Logging macros
////////////////////////////////////////////////////////////////////////////////

#ifndef FLECS_LEGACY /* C89 doesn't support variadic macros */

/* Base logging function. Accepts a custom level */
#define ecs_print(level, ...)\
    ecs_print_(level, __FILE__, __LINE__, __VA_ARGS__)

#define ecs_printv(level, fmt, args)\
    ecs_printv_(level, __FILE__, __LINE__, fmt, args)

#define ecs_log(level, ...)\
    ecs_log_(level, __FILE__, __LINE__, __VA_ARGS__)

#define ecs_logv(level, fmt, args)\
    ecs_logv_(level, __FILE__, __LINE__, fmt, args)

/* Tracing. Used for logging of infrequent events  */
#define ecs_trace_(file, line, ...) ecs_log_(0, file, line, __VA_ARGS__)
#define ecs_trace(...) ecs_trace_(__FILE__, __LINE__, __VA_ARGS__)

/* Warning. Used when an issue occurs, but operation is successful */
#define ecs_warn_(file, line, ...) ecs_log_(-2, file, line, __VA_ARGS__)
#define ecs_warn(...) ecs_warn_(__FILE__, __LINE__, __VA_ARGS__)

/* Error. Used when an issue occurs, and operation failed. */
#define ecs_err_(file, line, ...) ecs_log_(-3, file, line, __VA_ARGS__)
#define ecs_err(...) ecs_err_(__FILE__, __LINE__, __VA_ARGS__)

/* Fatal. Used when an issue occurs, and the application cannot continue. */
#define ecs_fatal_(file, line, ...) ecs_log_(-4, file, line, __VA_ARGS__)
#define ecs_fatal(...) ecs_fatal_(__FILE__, __LINE__, __VA_ARGS__)

/* Optionally include warnings about using deprecated features */
#ifndef FLECS_NO_DEPRECATED_WARNINGS
#define ecs_deprecated(...)\
    ecs_deprecated_(__FILE__, __LINE__, __VA_ARGS__)
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


/* Define/undefine macros based on compiled-in tracing level. This can optimize
 * out tracing statements from a build, which improves performance. */

#if defined(FLECS_LOG_3) /* All debug tracing enabled */
#define ecs_dbg_1(...) ecs_log(1, __VA_ARGS__);
#define ecs_dbg_2(...) ecs_log(2, __VA_ARGS__);
#define ecs_dbg_3(...) ecs_log(3, __VA_ARGS__);

#define ecs_log_push_1() ecs_log_push_(1);
#define ecs_log_push_2() ecs_log_push_(2);
#define ecs_log_push_3() ecs_log_push_(3);

#define ecs_log_pop_1() ecs_log_pop_(1);
#define ecs_log_pop_2() ecs_log_pop_(2);
#define ecs_log_pop_3() ecs_log_pop_(3);

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

#define ecs_log_push_1() ecs_log_push_(1);
#define ecs_log_push_2() ecs_log_push_(2);
#define ecs_log_push_3()

#define ecs_log_pop_1() ecs_log_pop_(1);
#define ecs_log_pop_2() ecs_log_pop_(2);
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

#define ecs_log_push_1() ecs_log_push_(1);
#define ecs_log_push_2()
#define ecs_log_push_3()

#define ecs_log_pop_1() ecs_log_pop_(1);
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
#define ecs_log_push() ecs_log_push_(0)
#define ecs_log_pop() ecs_log_pop_(0)

/** Abort.
 * Unconditionally aborts process. */
#define ecs_abort(error_code, ...)\
    ecs_abort_(error_code, __FILE__, __LINE__, __VA_ARGS__);\
    ecs_os_abort(); abort(); /* satisfy compiler/static analyzers */

/** Assert. 
 * Aborts if condition is false, disabled in debug mode. */
#if defined(FLECS_NDEBUG) && !defined(FLECS_KEEP_ASSERT)
#define ecs_assert(condition, error_code, ...)
#else
#define ecs_assert(condition, error_code, ...)\
    if (!ecs_assert_(condition, error_code, #condition, __FILE__, __LINE__, __VA_ARGS__)) {\
        ecs_os_abort();\
    }\
    assert(condition) /* satisfy compiler/static analyzers */
#endif // FLECS_NDEBUG

#define ecs_assert_var(var, error_code, ...)\
    ecs_assert(var, error_code, __VA_ARGS__);\
    (void)var

/** Debug assert.
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

/** Check.
 * goto error if condition is false. */
#if defined(FLECS_NDEBUG) && !defined(FLECS_KEEP_ASSERT)
#define ecs_check(condition, error_code, ...) ecs_dummy_check
#else
#ifdef FLECS_SOFT_ASSERT
#define ecs_check(condition, error_code, ...)\
    if (!ecs_assert_(condition, error_code, #condition, __FILE__, __LINE__, __VA_ARGS__)) {\
        goto error;\
    }
#else // FLECS_SOFT_ASSERT
#define ecs_check(condition, error_code, ...)\
    ecs_assert(condition, error_code, __VA_ARGS__);\
    ecs_dummy_check
#endif
#endif // FLECS_NDEBUG

/** Panic.
 * goto error when FLECS_SOFT_ASSERT is defined, otherwise abort */
#if defined(FLECS_NDEBUG) && !defined(FLECS_KEEP_ASSERT)
#define ecs_throw(error_code, ...) ecs_dummy_check
#else
#ifdef FLECS_SOFT_ASSERT
#define ecs_throw(error_code, ...)\
    ecs_abort_(error_code, __FILE__, __LINE__, __VA_ARGS__);\
    goto error;
#else
#define ecs_throw(error_code, ...)\
    ecs_abort(error_code, __VA_ARGS__);\
    ecs_dummy_check
#endif
#endif // FLECS_NDEBUG

/** Parser error */
#define ecs_parser_error(name, expr, column, ...)\
    ecs_parser_error_(name, expr, column, __VA_ARGS__)

#define ecs_parser_errorv(name, expr, column, fmt, args)\
    ecs_parser_errorv_(name, expr, column, fmt, args)

#endif // FLECS_LEGACY


////////////////////////////////////////////////////////////////////////////////
//// Functions that are always available
////////////////////////////////////////////////////////////////////////////////

/** Enable or disable log.
 * This will enable builtin log. For log to work, it will have to be
 * compiled in which requires defining one of the following macros:
 *
 * FLECS_LOG_0 - All log is disabled
 * FLECS_LOG_1 - Enable log level 1
 * FLECS_LOG_2 - Enable log level 2 and below
 * FLECS_LOG_3 - Enable log level 3 and below
 *
 * If no log level is defined and this is a debug build, FLECS_LOG_3 will
 * have been automatically defined.
 *
 * The provided level corresponds with the log level. If -1 is provided as
 * value, warnings are disabled. If -2 is provided, errors are disabled as well.
 *
 * @param level Desired tracing level.
 * @return Previous log level.
 */
FLECS_API
int ecs_log_set_level(
    int level);

/** Get current log level. 
 * 
 * @return Previous log level.
 */
FLECS_API
int ecs_log_get_level(void);

/** Enable/disable tracing with colors.
 * By default colors are enabled.
 *
 * @param enabled Whether to enable tracing with colors.
 * @return Previous color setting.
 */
FLECS_API
bool ecs_log_enable_colors(
    bool enabled);

/** Enable/disable logging timestamp.
 * By default timestamps are disabled. Note that enabling timestamps introduces
 * overhead as the logging code will need to obtain the current time.
 *
 * @param enabled Whether to enable tracing with timestamps.
 * @return Previous timestamp setting.
 */
FLECS_API
bool ecs_log_enable_timestamp(
    bool enabled);

/** Enable/disable logging time since last log.
 * By default deltatime is disabled. Note that enabling timestamps introduces
 * overhead as the logging code will need to obtain the current time.
 * 
 * When enabled, this logs the amount of time in seconds passed since the last
 * log, when this amount is non-zero. The format is a '+' character followed by
 * the number of seconds:
 * 
 *   +1 trace: log message
 *
 * @param enabled Whether to enable tracing with timestamps.
 * @return Previous timestamp setting.
 */
FLECS_API
bool ecs_log_enable_timedelta(
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
#define ECS_LEAK_DETECTED (14)

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

#define ECS_ACCESS_VIOLATION (40)
#define ECS_COLUMN_INDEX_OUT_OF_RANGE (41)
#define ECS_COLUMN_IS_NOT_SHARED (42)
#define ECS_COLUMN_IS_SHARED (43)
#define ECS_COLUMN_TYPE_MISMATCH (45)

#define ECS_INVALID_WHILE_READONLY (70)
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

/** @} */

#endif // FLECS_LOG_H


/* Handle addon dependencies that need declarations to be visible in header */
#ifdef FLECS_MONITOR
#ifndef FLECS_STATS
#define FLECS_STATS
#endif
#ifndef FLECS_SYSTEM
#define FLECS_SYSTEM
#endif
#ifndef FLECS_TIMER
#define FLECS_TIMER
#endif
#endif

#ifdef FLECS_REST
#define FLECS_HTTP
#endif

#ifdef FLECS_PLECS
#define FLECS_EXPR
#endif

#ifdef FLECS_APP
#ifdef FLECS_NO_APP
#error "FLECS_NO_APP failed: APP is required by other addons"
#endif
/**
 * @file addons/app.h
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

/**
 * @defgroup c_addons_app App
 * @brief Optional addon for running the main application loop.
 * 
 * \ingroup c_addons
 * @{
 */

/** Callback type for init action. */
typedef int(*ecs_app_init_action_t)(
    ecs_world_t *world);

/** Used with ecs_app_run. */
typedef struct ecs_app_desc_t {
    ecs_ftime_t target_fps;   /**< Target FPS. */
    ecs_ftime_t delta_time;   /**< Frame time increment (0 for measured values) */
    int32_t threads;          /**< Number of threads. */
    int32_t frames;           /**< Number of frames to run (0 for infinite) */
    bool enable_rest;         /**< Enables ECS access over HTTP, necessary for explorer */
    bool enable_monitor;      /**< Periodically collect statistics */
    uint16_t port;            /**< HTTP port used by REST API */

    ecs_app_init_action_t init; /**< If set, function is ran before starting the
                                 * main loop. */

    void *ctx;                /**< Reserved for custom run/frame actions */
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

/** @} */

#ifdef __cplusplus
}
#endif

#endif

#endif // FLECS_APP

#endif

#ifdef FLECS_HTTP
#ifdef FLECS_NO_HTTP
#error "FLECS_NO_HTTP failed: HTTP is required by other addons"
#endif
/**
 * @file addons/http.h
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

/**
 * @defgroup c_addons_http Http
 * @brief Simple HTTP server used for serving up REST API.
 * 
 * \ingroup c_addons
 * @{
 */

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
    int code;                   /**< default = 200 */
    ecs_strbuf_t body;          /**< default = "" */
    const char* status;         /**< default = OK */
    const char* content_type;   /**< default = application/json */
    ecs_strbuf_t headers;       /**< default = "" */
} ecs_http_reply_t;

#define ECS_HTTP_REPLY_INIT \
    (ecs_http_reply_t){200, ECS_STRBUF_INIT, "OK", "application/json", ECS_STRBUF_INIT}

/* Global statistics. */
extern int64_t ecs_http_request_received_count;
extern int64_t ecs_http_request_invalid_count;
extern int64_t ecs_http_request_handled_ok_count;
extern int64_t ecs_http_request_handled_error_count;
extern int64_t ecs_http_request_not_handled_count;
extern int64_t ecs_http_request_preflight_count;
extern int64_t ecs_http_send_ok_count;
extern int64_t ecs_http_send_error_count;
extern int64_t ecs_http_busy_count;

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
    ecs_http_reply_action_t callback; /**< Function called for each request  */
    void *ctx;                        /**< Passed to callback (optional) */
    uint16_t port;                    /**< HTTP port */
    const char *ipaddr;               /**< Interface to listen on (optional) */
    int32_t send_queue_wait_ms;       /**< Send queue wait time when empty */
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
    ecs_ftime_t delta_time);

/** Stop server. 
 * After this operation no new requests can be received.
 * 
 * @param server The server.
 */
FLECS_API
void ecs_http_server_stop(
    ecs_http_server_t* server);

/** Emulate a request.
 * The request string must be a valid HTTP request. A minimal example:
 *   GET /entity/flecs/core/World?label=true HTTP/1.1
 *
 * @param srv The server.
 * @param req The request.
 * @param len The length of the request (optional).
 * @return The reply.
 */
FLECS_API
int ecs_http_server_http_request(
    ecs_http_server_t* srv,
    const char *req,
    ecs_size_t len,
    ecs_http_reply_t *reply_out);

/** Convenience wrapper around ecs_http_server_request. */
FLECS_API
int ecs_http_server_request(
    ecs_http_server_t* srv,
    const char *method,
    const char *req,
    ecs_http_reply_t *reply_out);

/** Get context provided in ecs_http_server_desc_t */
FLECS_API
void* ecs_http_server_ctx(
    ecs_http_server_t* srv);

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

/** @} */

#endif // FLECS_HTTP_H

#endif // FLECS_HTTP

#endif

#ifdef FLECS_REST
#ifdef FLECS_NO_REST
#error "FLECS_NO_REST failed: REST is required by other addons"
#endif
/**
 * @file addons/rest.h
 * @brief REST API addon.
 *
 * A small REST API that uses the HTTP server and JSON serializer to provide
 * access to application data for remote applications.
 * 
 * A description of the API can be found in docs/RestApi.md
 */

#ifdef FLECS_REST

/**
 * @defgroup c_addons_rest Rest
 * @brief REST API for querying and mutating entities.
 * 
 * \ingroup c_addons
 * @{
 */

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

/** Component that instantiates the REST API */
FLECS_API extern const ecs_entity_t ecs_id(EcsRest);

typedef struct {
    uint16_t port;      /**< Port of server (optional, default = 27750) */
    char *ipaddr;       /**< Interface address (optional, default = 0.0.0.0) */
    void *impl;
} EcsRest;

/* Global statistics */
extern int64_t ecs_rest_request_count;
extern int64_t ecs_rest_entity_count;
extern int64_t ecs_rest_entity_error_count;
extern int64_t ecs_rest_query_count;
extern int64_t ecs_rest_query_error_count;
extern int64_t ecs_rest_query_name_count;
extern int64_t ecs_rest_query_name_error_count;
extern int64_t ecs_rest_query_name_from_cache_count;
extern int64_t ecs_rest_enable_count;
extern int64_t ecs_rest_enable_error_count;
extern int64_t ecs_rest_delete_count;
extern int64_t ecs_rest_delete_error_count;
extern int64_t ecs_rest_world_stats_count;
extern int64_t ecs_rest_pipeline_stats_count;
extern int64_t ecs_rest_stats_error_count;

/** Create HTTP server for REST API. 
 * This allows for the creation of a REST server that can be managed by the
 * application without using Flecs systems.
 * 
 * @param world The world.
 * @param desc The HTTP server descriptor.
 * @return The HTTP server, or NULL if failed.
 */
FLECS_API
ecs_http_server_t* ecs_rest_server_init(
    ecs_world_t *world,
    const ecs_http_server_desc_t *desc);

/** Cleanup REST HTTP server. 
 * The server must have been created with ecs_rest_server_init.
 */
FLECS_API
void ecs_rest_server_fini(
    ecs_http_server_t *srv);

/* Module import */
FLECS_API
void FlecsRestImport(
    ecs_world_t *world);

#ifdef __cplusplus
}
#endif

#endif

/** @} */

#endif

#endif

#ifdef FLECS_TIMER
#ifdef FLECS_NO_TIMER
#error "FLECS_NO_TIMER failed: TIMER is required by other addons"
#endif
/**
 * @file addons/timer.h
 * @brief Timer module.
 *
 * Timers can be used to trigger actions at periodic or one-shot intervals. They
 * are typically used together with systems and pipelines.
 */

#ifdef FLECS_TIMER

/**
 * @defgroup c_addons_timer Timer
 * @brief Run systems at a time interval.
 * 
 * \ingroup c_addons
 * @{
 */

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

/** Component used for one shot/interval timer functionality */
typedef struct EcsTimer {
    ecs_ftime_t timeout;         /**< Timer timeout period */
    ecs_ftime_t time;            /**< Incrementing time value */
    ecs_ftime_t overshoot;       /**< Used to correct returned interval time */
    int32_t fired_count;         /**< Number of times ticked */
    bool active;                 /**< Is the timer active or not */
    bool single_shot;            /**< Is this a single shot timer */
} EcsTimer;

/** Apply a rate filter to a tick source */
typedef struct EcsRateFilter {
    ecs_entity_t src;            /**< Source of the rate filter */
    int32_t rate;                /**< Rate of the rate filter */
    int32_t tick_count;          /**< Number of times the rate filter ticked */
    ecs_ftime_t time_elapsed;    /**< Time elapsed since last tick */
} EcsRateFilter;


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
    ecs_ftime_t timeout);

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
ecs_ftime_t ecs_get_timeout(
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
    ecs_ftime_t interval);   

/** Get current interval value for the specified timer.
 * This operation returns the value set by ecs_set_interval. If the entity is
 * not a timer, the operation will return 0.
 *
 * @param world The world.
 * @param tick_source The timer for which to set the interval.
 * @return The current interval value, or 0 if no timer is active.
 */
FLECS_API
ecs_ftime_t ecs_get_interval(
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

/** @} */

#endif

#endif

#ifdef FLECS_PIPELINE
#ifdef FLECS_NO_PIPELINE
#error "FLECS_NO_PIPELINE failed: PIPELINE is required by other addons"
#endif
/**
 * @file addons/pipeline.h
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

/**
 * @defgroup c_addons_pipeline Pipeline
 * @brief Pipelines order and schedule systems for execution.
 * 
 * \ingroup c_addons
 * @{
 */

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

#define ECS_PIPELINE_DEFINE(world, id_, ...) \
    { \
        ecs_pipeline_desc_t desc = {0}; \
        ecs_entity_desc_t edesc = {0}; \
        edesc.id = id_;\
        edesc.name = #id_;\
        desc.entity = ecs_entity_init(world, &edesc);\
        desc.query.filter.expr = #__VA_ARGS__; \
        id_ = ecs_pipeline_init(world, &desc); \
        ecs_id(id_) = id_;\
    } \
    ecs_assert(id_ != 0, ECS_INVALID_PARAMETER, NULL);

#define ECS_PIPELINE(world, id, ...) \
    ecs_entity_t id = 0, ecs_id(id) = 0; ECS_PIPELINE_DEFINE(world, id, __VA_ARGS__);\
    (void)id;\
    (void)ecs_id(id);

#define ecs_pipeline(world, ...)\
    ecs_pipeline_init(world, &(ecs_pipeline_desc_t) __VA_ARGS__ )

#endif

/* Pipeline descriptor (used with ecs_pipeline_init) */
typedef struct ecs_pipeline_desc_t {
    /* Existing entity to associate with pipeline (optional) */
    ecs_entity_t entity;
    
    /* Query descriptor. The first term of the query must match the EcsSystem
     * component. */
    ecs_query_desc_t query;
} ecs_pipeline_desc_t;

/** Create a custom pipeline.
 */
FLECS_API
ecs_entity_t ecs_pipeline_init(
    ecs_world_t *world,
    const ecs_pipeline_desc_t *desc);

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
    ecs_ftime_t delta_time);   

/** Set time scale.
 * Increase or decrease simulation speed by the provided multiplier.
 *
 * @param world The world.
 * @param scale The scale to apply (default = 1).
 */
FLECS_API 
void ecs_set_time_scale(
    ecs_world_t *world,
    ecs_ftime_t scale);

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
 * @param world The world.
 * @param pipeline The pipeline to run.
 */
FLECS_API 
void ecs_run_pipeline(
    ecs_world_t *world,
    ecs_entity_t pipeline,
    ecs_ftime_t delta_time);


////////////////////////////////////////////////////////////////////////////////
//// Threading
////////////////////////////////////////////////////////////////////////////////

/** Set number of worker threads.
 * Setting this value to a value higher than 1 will start as many threads and
 * will cause systems to evenly distribute matched entities across threads. The
 * operation may be called multiple times to reconfigure the number of threads
 * used, but never while running a system / pipeline. 
 * Calling ecs_set_threads will also end the use of task threads setup with 
 * ecs_set_task_threads and vice-versa */
FLECS_API
void ecs_set_threads(
    ecs_world_t *world,
    int32_t threads);

/** Set number of worker task threads.
 * ecs_set_task_threads is similar to ecs_set_threads, except threads are treated
 * as short-lived tasks and will be created and joined around each update of the world. 
 * Creation and joining of these tasks will use the os_api_t tasks APIs rather than the
 * the standard thread API functions, although they may be the same if desired.
 * This function is useful for multithreading world updates using an external
 * asynchronous job system rather than long running threads by providing the APIs
 * to create tasks for your job system and then wait on their conclusion. 
 * The operation may be called multiple times to reconfigure the number of task threads
 * used, but never while running a system / pipeline. 
 * Calling ecs_set_task_threads will also end the use of threads setup with 
 * ecs_set_threads and vice-versa */

FLECS_API
void ecs_set_task_threads(
    ecs_world_t *world,
    int32_t task_threads);

/** Returns true if task thread use have been requested. */
FLECS_API
bool ecs_using_task_threads(
    ecs_world_t *world);

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

/** @} */

#endif

#endif

#ifdef FLECS_SYSTEM
#ifdef FLECS_NO_SYSTEM
#error "FLECS_NO_SYSTEM failed: SYSTEM is required by other addons"
#endif
/**
 * @file addons/system.h
 * @brief System module.
 *
 * The system module allows for creating and running systems. A system is a
 * query in combination with a callback function. In addition systems have
 * support for time management and can be monitored by the stats addon.
 */

#ifdef FLECS_SYSTEM

/**
 * @defgroup c_addons_system System
 * @brief Systems are a query + function that can be ran manually or by a pipeline.
 * 
 * \ingroup c_addons
 * @{
 */

#ifndef FLECS_MODULE
#define FLECS_MODULE
#endif

#ifndef FLECS_SYSTEM_H
#define FLECS_SYSTEM_H

#ifdef __cplusplus
extern "C" {
#endif

/** Component used to provide a tick source to systems */
typedef struct EcsTickSource {
    bool tick;                 /**< True if providing tick */
    ecs_ftime_t time_elapsed;  /**< Time elapsed since last tick */
} EcsTickSource;

/** Use with ecs_system_init */
typedef struct ecs_system_desc_t {
    int32_t _canary;

    /** Existing entity to associate with system (optional) */
    ecs_entity_t entity;

    /** System query parameters */
    ecs_query_desc_t query;

    /** Callback that is invoked when a system is ran. 
     * When left to NULL, the default system runner is used, which calls the 
     * "callback" action for each result returned from the system's query. 
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

    /** Callback that is ran for each result returned by the system's query. This
     * means that this callback can be invoked multiple times per system per
     * frame, typically once for each matching table. */
    ecs_iter_action_t callback;

    /** Context to be passed to callback (as ecs_iter_t::param) */
    void *ctx;

    /** Binding context, for when system is implemented in other language */
    void *binding_ctx;

    /** Functions that are invoked during system cleanup to free context data.
     * When set, functions are called unconditionally, even when the ctx
     * pointers are NULL. */
    ecs_ctx_free_t ctx_free;
    ecs_ctx_free_t binding_ctx_free;

    /** Interval in seconds at which the system should run */
    ecs_ftime_t interval;

    /** Rate at which the system should run */
    int32_t rate;

    /** External tick soutce that determines when system ticks */
    ecs_entity_t tick_source;

    /** If true, system will be ran on multiple threads */
    bool multi_threaded;

    /** If true, system will have access to actuall world. Cannot be true at the
     * same time as multi_threaded. */
    bool no_readonly;
} ecs_system_desc_t;

/** Create a system */
FLECS_API
ecs_entity_t ecs_system_init(
    ecs_world_t *world,
    const ecs_system_desc_t *desc);

#ifndef FLECS_LEGACY

/** Forward declare a system. */
#define ECS_SYSTEM_DECLARE(id) ecs_entity_t ecs_id(id)

/** Define a forward declared system.
 * 
 * Example:
 *   ECS_SYSTEM_DEFINE(world, Move, EcsOnUpdate, Position, Velocity);
 */
#define ECS_SYSTEM_DEFINE(world, id_, phase, ...) \
    { \
        ecs_system_desc_t desc = {0}; \
        ecs_entity_desc_t edesc = {0}; \
        edesc.id = ecs_id(id_);\
        edesc.name = #id_;\
        edesc.add[0] = ((phase) ? ecs_pair(EcsDependsOn, (phase)) : 0); \
        edesc.add[1] = (phase); \
        desc.entity = ecs_entity_init(world, &edesc);\
        desc.query.filter.expr = #__VA_ARGS__; \
        desc.callback = id_; \
        ecs_id(id_) = ecs_system_init(world, &desc); \
    } \
    ecs_assert(ecs_id(id_) != 0, ECS_INVALID_PARAMETER, NULL)

/** Declare & define a system.
 * 
 * Example:
 *   ECS_SYSTEM(world, Move, EcsOnUpdate, Position, Velocity);
 */
#define ECS_SYSTEM(world, id, phase, ...) \
    ecs_entity_t ecs_id(id) = 0; ECS_SYSTEM_DEFINE(world, id, phase, __VA_ARGS__);\
    ecs_entity_t id = ecs_id(id);\
    (void)ecs_id(id);\
    (void)id

/** Shorthand for creating a system with ecs_system_init.
 *
 * Example:
 *   ecs_system(world, {
 *     .entity = ecs_entity(world, {
 *       .name = "MyEntity",
 *       .add = { ecs_dependson(EcsOnUpdate) }
 *     },
 *     .query.filter.terms = {
 *       { ecs_id(Position) },
 *       { ecs_id(Velocity) }
 *     },
 *     .callback = Move
 *   });
 */
#define ecs_system(world, ...)\
    ecs_system_init(world, &(ecs_system_desc_t) __VA_ARGS__ )

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
    ecs_ftime_t delta_time,
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
    ecs_ftime_t delta_time,
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
    ecs_ftime_t delta_time,
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

FLECS_API
void FlecsSystemImport(
    ecs_world_t *world);

#ifdef __cplusplus
}
#endif

#endif

/** @} */

#endif

#endif

#ifdef FLECS_STATS
#ifdef FLECS_NO_STATS
#error "FLECS_NO_STATS failed: STATS is required by other addons"
#endif
/**
 * @file addons/stats.h
 * @brief Statistics addon.
 *
 * The statistics addon enables an application to obtain detailed metrics about
 * the storage, systems and operations of a world.
 */

#ifdef FLECS_STATS

/**
 * @defgroup c_addons_stats Stats
 * @brief Collection of statistics for world, queries, systems and pipelines.
 * 
 * \ingroup c_addons
 * @{
 */

#ifndef FLECS_STATS_H
#define FLECS_STATS_H

#ifdef __cplusplus
extern "C" {
#endif

#define ECS_STAT_WINDOW (60)

/** Simple value that indicates current state */
typedef struct ecs_gauge_t {
    ecs_float_t avg[ECS_STAT_WINDOW];
    ecs_float_t min[ECS_STAT_WINDOW];
    ecs_float_t max[ECS_STAT_WINDOW];
} ecs_gauge_t;

/** Monotonically increasing counter */
typedef struct ecs_counter_t {
    ecs_gauge_t rate;                     /**< Keep track of deltas too */
    double value[ECS_STAT_WINDOW];
} ecs_counter_t;

/** Make all metrics the same size, so we can iterate over fields */
typedef union ecs_metric_t {
    ecs_gauge_t gauge;
    ecs_counter_t counter;
} ecs_metric_t;

typedef struct ecs_world_stats_t {
    int64_t first_;

    /* Entities */
    struct {
        ecs_metric_t count;               /**< Number of entities */
        ecs_metric_t not_alive_count;     /**< Number of not alive (recyclable) entity ids */
    } entities;

    /* Components and ids */
    struct {
        ecs_metric_t count;               /**< Number of ids (excluding wildcards) */
        ecs_metric_t tag_count;           /**< Number of tag ids (ids without data) */
        ecs_metric_t component_count;     /**< Number of components ids (ids with data) */
        ecs_metric_t pair_count;          /**< Number of pair ids */
        ecs_metric_t wildcard_count;      /**< Number of wildcard ids */
        ecs_metric_t type_count;          /**< Number of registered types */
        ecs_metric_t create_count;        /**< Number of times id has been created */
        ecs_metric_t delete_count;        /**< Number of times id has been deleted */
    } ids;

    /* Tables */
    struct {
        ecs_metric_t count;                /**< Number of tables */
        ecs_metric_t empty_count;          /**< Number of empty tables */
        ecs_metric_t tag_only_count;       /**< Number of tables with only tags */
        ecs_metric_t trivial_only_count;   /**< Number of tables with only trivial components */
        ecs_metric_t record_count;         /**< Number of table cache records */
        ecs_metric_t storage_count;        /**< Number of table storages */
        ecs_metric_t create_count;         /**< Number of times table has been created */
        ecs_metric_t delete_count;         /**< Number of times table has been deleted */
    } tables;

    /* Queries & events */
    struct {
        ecs_metric_t query_count;          /**< Number of queries */
        ecs_metric_t observer_count;       /**< Number of observers */
        ecs_metric_t system_count;         /**< Number of systems */
    } queries;

    /* Commands */
    struct {
        ecs_metric_t add_count;
        ecs_metric_t remove_count;
        ecs_metric_t delete_count;
        ecs_metric_t clear_count;
        ecs_metric_t set_count;
        ecs_metric_t get_mut_count;
        ecs_metric_t modified_count;
        ecs_metric_t other_count;
        ecs_metric_t discard_count;
        ecs_metric_t batched_entity_count;
        ecs_metric_t batched_count;
    } commands;

    /* Frame data */
    struct {
        ecs_metric_t frame_count;          /**< Number of frames processed. */
        ecs_metric_t merge_count;          /**< Number of merges executed. */
        ecs_metric_t rematch_count;        /**< Number of query rematches */
        ecs_metric_t pipeline_build_count; /**< Number of system pipeline rebuilds (occurs when an inactive system becomes active). */
        ecs_metric_t systems_ran;          /**< Number of systems ran. */
        ecs_metric_t observers_ran;        /**< Number of times an observer was invoked. */
        ecs_metric_t event_emit_count;     /**< Number of events emitted */
    } frame;

    /* Timing */
    struct {
        ecs_metric_t world_time_raw;       /**< Actual time passed since simulation start (first time progress() is called) */
        ecs_metric_t world_time;           /**< Simulation time passed since simulation start. Takes into account time scaling */
        ecs_metric_t frame_time;           /**< Time spent processing a frame. Smaller than world_time_total when load is not 100% */
        ecs_metric_t system_time;          /**< Time spent on running systems. */
        ecs_metric_t emit_time;            /**< Time spent on notifying observers. */
        ecs_metric_t merge_time;           /**< Time spent on merging commands. */
        ecs_metric_t rematch_time;         /**< Time spent on rematching. */
        ecs_metric_t fps;                  /**< Frames per second. */
        ecs_metric_t delta_time;           /**< Delta_time. */
    } performance;

    struct {
        /* Memory allocation data */
        ecs_metric_t alloc_count;          /**< Allocs per frame */
        ecs_metric_t realloc_count;        /**< Reallocs per frame */
        ecs_metric_t free_count;           /**< Frees per frame */
        ecs_metric_t outstanding_alloc_count; /**< Difference between allocs & frees */

        /* Memory allocator data */
        ecs_metric_t block_alloc_count;    /**< Block allocations per frame */
        ecs_metric_t block_free_count;     /**< Block frees per frame */
        ecs_metric_t block_outstanding_alloc_count; /**< Difference between allocs & frees */
        ecs_metric_t stack_alloc_count;    /**< Page allocations per frame */
        ecs_metric_t stack_free_count;     /**< Page frees per frame */
        ecs_metric_t stack_outstanding_alloc_count; /**< Difference between allocs & frees */
    } memory;

    /* REST statistics */
    struct {
        ecs_metric_t request_count;
        ecs_metric_t entity_count;
        ecs_metric_t entity_error_count;
        ecs_metric_t query_count;
        ecs_metric_t query_error_count;
        ecs_metric_t query_name_count;
        ecs_metric_t query_name_error_count;
        ecs_metric_t query_name_from_cache_count;
        ecs_metric_t enable_count;
        ecs_metric_t enable_error_count;
        ecs_metric_t world_stats_count;
        ecs_metric_t pipeline_stats_count;
        ecs_metric_t stats_error_count;
    } rest;

    /* HTTP statistics */
    struct {
        ecs_metric_t request_received_count;
        ecs_metric_t request_invalid_count;
        ecs_metric_t request_handled_ok_count;
        ecs_metric_t request_handled_error_count;
        ecs_metric_t request_not_handled_count;
        ecs_metric_t request_preflight_count;
        ecs_metric_t send_ok_count;
        ecs_metric_t send_error_count;
        ecs_metric_t busy_count;
    } http;

    int64_t last_;

    /** Current position in ringbuffer */
    int32_t t;
} ecs_world_stats_t;

/** Statistics for a single query (use ecs_query_stats_get) */
typedef struct ecs_query_stats_t {
    int64_t first_;
    ecs_metric_t matched_table_count;       /**< Matched non-empty tables */    
    ecs_metric_t matched_empty_table_count; /**< Matched empty tables */
    ecs_metric_t matched_entity_count;      /**< Number of matched entities */
    int64_t last_;

    /** Current position in ringbuffer */
    int32_t t; 
} ecs_query_stats_t;

/** Statistics for a single system (use ecs_system_stats_get) */
typedef struct ecs_system_stats_t {
    int64_t first_;
    ecs_metric_t time_spent;       /**< Time spent processing a system */
    ecs_metric_t invoke_count;     /**< Number of times system is invoked */
    ecs_metric_t active;           /**< Whether system is active (is matched with >0 entities) */
    ecs_metric_t enabled;          /**< Whether system is enabled */
    int64_t last_;

    bool task;                     /**< Is system a task */

    ecs_query_stats_t query;
} ecs_system_stats_t;

/** Statistics for all systems in a pipeline. */
typedef struct ecs_pipeline_stats_t {
    /* Allow for initializing struct with {0} */
    int8_t canary_;

    /** Vector with system ids of all systems in the pipeline. The systems are
     * stored in the order they are executed. Merges are represented by a 0. */
    ecs_vec_t systems;

    /** Map with system statistics. For each system in the systems vector, an
     * entry in the map exists of type ecs_system_stats_t. */
    ecs_map_t system_stats;

    /** Current position in ringbuffer */
    int32_t t;

    int32_t system_count;        /**< Number of systems in pipeline */
    int32_t active_system_count; /**< Number of active systems in pipeline */
    int32_t rebuild_count;       /**< Number of times pipeline has rebuilt */
} ecs_pipeline_stats_t;

/** Get world statistics.
 *
 * @param world The world.
 * @param stats Out parameter for statistics.
 */
FLECS_API 
void ecs_world_stats_get(
    const ecs_world_t *world,
    ecs_world_stats_t *stats);

/** Reduce source measurement window into single destination measurement. */
FLECS_API 
void ecs_world_stats_reduce(
    ecs_world_stats_t *dst,
    const ecs_world_stats_t *src);

/** Reduce last measurement into previous measurement, restore old value. */
FLECS_API
void ecs_world_stats_reduce_last(
    ecs_world_stats_t *stats,
    const ecs_world_stats_t *old,
    int32_t count);

/** Repeat last measurement. */
FLECS_API
void ecs_world_stats_repeat_last(
    ecs_world_stats_t *stats);

/** Copy last measurement from source to destination. */
FLECS_API
void ecs_world_stats_copy_last(
    ecs_world_stats_t *dst,
    const ecs_world_stats_t *src);

FLECS_API 
void ecs_world_stats_log(
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
void ecs_query_stats_get(
    const ecs_world_t *world,
    const ecs_query_t *query,
    ecs_query_stats_t *stats);

/** Reduce source measurement window into single destination measurement. */
FLECS_API 
void ecs_query_stats_reduce(
    ecs_query_stats_t *dst,
    const ecs_query_stats_t *src);

/** Reduce last measurement into previous measurement, restore old value. */
FLECS_API
void ecs_query_stats_reduce_last(
    ecs_query_stats_t *stats,
    const ecs_query_stats_t *old,
    int32_t count);

/** Repeat last measurement. */
FLECS_API
void ecs_query_stats_repeat_last(
    ecs_query_stats_t *stats);

/** Copy last measurement from source to destination. */
FLECS_API
void ecs_query_stats_copy_last(
    ecs_query_stats_t *dst,
    const ecs_query_stats_t *src);

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
bool ecs_system_stats_get(
    const ecs_world_t *world,
    ecs_entity_t system,
    ecs_system_stats_t *stats);

/** Reduce source measurement window into single destination measurement */
FLECS_API 
void ecs_system_stats_reduce(
    ecs_system_stats_t *dst,
    const ecs_system_stats_t *src);

/** Reduce last measurement into previous measurement, restore old value. */
FLECS_API
void ecs_system_stats_reduce_last(
    ecs_system_stats_t *stats,
    const ecs_system_stats_t *old,
    int32_t count);

/** Repeat last measurement. */
FLECS_API
void ecs_system_stats_repeat_last(
    ecs_system_stats_t *stats);

/** Copy last measurement from source to destination. */
FLECS_API
void ecs_system_stats_copy_last(
    ecs_system_stats_t *dst,
    const ecs_system_stats_t *src);
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
bool ecs_pipeline_stats_get(
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

/** Reduce source measurement window into single destination measurement */
FLECS_API 
void ecs_pipeline_stats_reduce(
    ecs_pipeline_stats_t *dst,
    const ecs_pipeline_stats_t *src);

/** Reduce last measurement into previous measurement, restore old value. */
FLECS_API
void ecs_pipeline_stats_reduce_last(
    ecs_pipeline_stats_t *stats,
    const ecs_pipeline_stats_t *old,
    int32_t count);

/** Repeat last measurement. */
FLECS_API
void ecs_pipeline_stats_repeat_last(
    ecs_pipeline_stats_t *stats);

/** Copy last measurement to destination.
 * This operation copies the last measurement into the destination. It does not
 * modify the cursor.
 * 
 * @param dst The metrics.
 * @param src The metrics to copy.
 */
FLECS_API
void ecs_pipeline_stats_copy_last(
    ecs_pipeline_stats_t *dst,
    const ecs_pipeline_stats_t *src);

#endif

/** Reduce all measurements from a window into a single measurement. */
FLECS_API 
void ecs_metric_reduce(
    ecs_metric_t *dst,
    const ecs_metric_t *src,
    int32_t t_dst,
    int32_t t_src);

/** Reduce last measurement into previous measurement */
FLECS_API
void ecs_metric_reduce_last(
    ecs_metric_t *m,
    int32_t t,
    int32_t count);

/** Copy measurement */
FLECS_API
void ecs_metric_copy(
    ecs_metric_t *m,
    int32_t dst,
    int32_t src);

#ifdef __cplusplus
}
#endif

#endif

/** @} */

#endif

#endif

#ifdef FLECS_METRICS
#ifdef FLECS_NO_METRICS
#error "FLECS_NO_METRICS failed: METRICS is required by other addons"
#endif
/**
 * @file addons/metrics.h
 * @brief Metrics module.
 *
 * The metrics module extracts metrics from components and makes them available
 * through a unified component interface.
 */

#ifdef FLECS_METRICS

/**
 * @defgroup c_addons_metrics Metrics
 * @brief Collect user-defined metrics from ECS data.
 * 
 * \ingroup c_addons
 * @{
 */

#ifndef FLECS_METRICS_H
#define FLECS_METRICS_H

#ifndef FLECS_META
#define FLECS_META
#endif

#ifndef FLECS_UNITS
#define FLECS_UNITS
#endif

#ifndef FLECS_PIPELINE
#define FLECS_PIPELINE
#endif

#ifdef __cplusplus
extern "C" {
#endif

FLECS_API extern ECS_COMPONENT_DECLARE(FlecsMetrics);

/** Tag added to metrics, and used as first element of metric kind pair */
FLECS_API extern ECS_TAG_DECLARE(EcsMetric);

/** Metric that has monotonically increasing value */
FLECS_API extern ECS_TAG_DECLARE(EcsCounter);

/** Counter metric that is auto-incremented by source value */
FLECS_API extern ECS_TAG_DECLARE(EcsCounterIncrement);

/** Counter metric that counts the number of entities with an id */
FLECS_API extern ECS_TAG_DECLARE(EcsCounterId);

/** Metric that represents current value */
FLECS_API extern ECS_TAG_DECLARE(EcsGauge);

/** Tag added to metric instances */
FLECS_API extern ECS_TAG_DECLARE(EcsMetricInstance);

/** Component with metric instance value */
FLECS_API extern ECS_COMPONENT_DECLARE(EcsMetricValue);

/** Component with entity source of metric instance */
FLECS_API extern ECS_COMPONENT_DECLARE(EcsMetricSource);

typedef struct EcsMetricValue {
    double value;
} EcsMetricValue;

typedef struct EcsMetricSource {
    ecs_entity_t entity;
} EcsMetricSource;

typedef struct ecs_metric_desc_t {
    int32_t _canary;

    /** Entity associated with metric */
    ecs_entity_t entity;
    
    /** Entity associated with member that stores metric value. Must not be set
     * at the same time as id. Cannot be combined with EcsCounterId. */
    ecs_entity_t member;

    /** Tracks whether entities have the specified component id. Must not be set
     * at the same time as member. */
    ecs_id_t id;

    /** If id is a (R, *) wildcard and relationship R has the OneOf property,
     * setting this value to true will track individual targets. 
     * If the kind is EcsCountId and the id is a (R, *) wildcard, this value
     * will create a metric per target. */
    bool targets;

    /** Must be EcsGauge, EcsCounter, EcsCounterIncrement or EcsCounterId */
    ecs_entity_t kind;

    /** Description of metric. Will only be set if FLECS_DOC addon is enabled */
    const char *brief;
} ecs_metric_desc_t;

/** Create a new metric.
 * Metrics are entities that store values measured from a range of different
 * properties in the ECS storage. Metrics provide a single unified interface to
 * discovering and reading these values, which can be useful for monitoring
 * utilities, or for debugging.
 * 
 * Examples of properties that can be measured by metrics are:
 *  - Component member values
 *  - How long an entity has had a specific component
 *  - How long an entity has had a specific target for a relationship
 *  - How many entities have a specific component
 * 
 * Metrics can either be created as a "gauge" or "counter". A gauge is a metric
 * that represents the value of something at a specific point in time, for
 * example "velocity". A counter metric represents a value that is monotonically
 * increasing, for example "miles driven".
 * 
 * There are three different kinds of counter metric kinds:
 * - EcsCounter
 *   When combined with a member, this will store the actual value of the member
 *   in the metric. This is useful for values that are already counters, such as
 *   a MilesDriven component.
 *   This kind creates a metric per entity that has the member/id.
 * 
 * - EcsCounterIncrement
 *   When combined with a member, this will increment the value of the metric by
 *   the value of the member * delta_time. This is useful for values that are
 *   not counters, such as a Velocity component.
 *   This kind creates a metric per entity that has the member.
 * 
 * - EcsCounterId
 *   This metric kind will count the number of entities with a specific 
 *   (component) id. This kind creates a single metric instance for regular ids,
 *   and a metric instance per target for wildcard ids when targets is set.
 * 
 * @param world The world.
 * @param desc Metric description.
 * @return The metric entity.
 */
FLECS_API
ecs_entity_t ecs_metric_init(
    ecs_world_t *world,
    const ecs_metric_desc_t *desc);

/** Shorthand for creating a metric with ecs_metric_init.
 *
 * Example:
 *   ecs_metric(world, {
 *     .member = ecs_lookup_fullpath(world, "Position.x")
 *     .kind = EcsGauge
 *   });
 */
#define ecs_metric(world, ...)\
    ecs_metric_init(world, &(ecs_metric_desc_t) __VA_ARGS__ )

/* Module import */
FLECS_API
void FlecsMetricsImport(
    ecs_world_t *world);

#ifdef __cplusplus
}
#endif

#endif

/** @} */

#endif

#endif

#ifdef FLECS_ALERTS
#ifdef FLECS_NO_ALERTS
#error "FLECS_NO_ALERTS failed: ALERTS is required by other addons"
#endif
/**
 * @file addons/alerts.h
 * @brief Alerts module.
 *
 * The alerts module enables applications to register alerts for when certain
 * conditions are met. Alerts are registered as queries, and automatically
 * become active when entities match the alert query.
 */

#ifdef FLECS_ALERTS

/**
 * @defgroup c_addons_alerts Alerts
 * @brief Create alerts from monitoring queries.
 * 
 * \ingroup c_addons
 * @{
 */

#ifndef FLECS_ALERTS_H
#define FLECS_ALERTS_H

#ifndef FLECS_RULES
#define FLECS_RULES
#endif

#ifndef FLECS_PIPELINE
#define FLECS_PIPELINE
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define ECS_ALERT_MAX_SEVERITY_FILTERS (4)

/* Module id */
FLECS_API extern ECS_COMPONENT_DECLARE(FlecsAlerts);

/* Module components */

/** Tag added to alert, and used as first element of alert severity pair */
FLECS_API extern ECS_COMPONENT_DECLARE(EcsAlert);
FLECS_API extern ECS_COMPONENT_DECLARE(EcsAlertInstance);
FLECS_API extern ECS_COMPONENT_DECLARE(EcsAlertsActive);
FLECS_API extern ECS_COMPONENT_DECLARE(EcsAlertTimeout);

/* Alert severity tags */
FLECS_API extern ECS_TAG_DECLARE(EcsAlertInfo);
FLECS_API extern ECS_TAG_DECLARE(EcsAlertWarning);
FLECS_API extern ECS_TAG_DECLARE(EcsAlertError);
FLECS_API extern ECS_TAG_DECLARE(EcsAlertCritical);

/** Alert information. Added to each alert instance */
typedef struct EcsAlertInstance {
    char *message;
} EcsAlertInstance;

/** Map with active alerts for entity. */
typedef struct EcsAlertsActive {
    ecs_map_t alerts;
} EcsAlertsActive;

typedef struct ecs_alert_severity_filter_t {
    ecs_entity_t severity; /* Severity kind */
    ecs_id_t with;         /* Component to match */
    const char *var;       /* Variable to match component on. Do not include the
                            * '$' character. Leave to NULL for $this. */
    int32_t _var_index;    /* Index of variable in filter (do not set) */
} ecs_alert_severity_filter_t;

typedef struct ecs_alert_desc_t { 
    int32_t _canary;

    /** Entity associated with alert */
    ecs_entity_t entity;

    /** Alert query. An alert will be created for each entity that matches the
     * specified query. The query must have at least one term that uses the
     * $this variable (default). */
    ecs_filter_desc_t filter;

    /** Template for alert message. This string is used to generate the alert
     * message and may refer to variables in the query result. The format for
     * the template expressions is as specified by ecs_interpolate_string.
     * 
     * Examples:
     *   "$this has Position but not Velocity"
     *   "$this has a parent entity $parent without Position"
     */
    const char *message;

    /** User friendly name. Will only be set if FLECS_DOC addon is enabled. */
    const char *doc_name;

    /** Description of alert. Will only be set if FLECS_DOC addon is enabled */
    const char *brief;

    /** Metric kind. Must be EcsAlertInfo, EcsAlertWarning, EcsAlertError or 
     * EcsAlertCritical. Defaults to EcsAlertError. */
    ecs_entity_t severity;

    /** Severity filters can be used to assign different severities to the same
     * alert. This prevents having to create multiple alerts, and allows 
     * entities to transition between severities without resetting the 
     * alert duration (optional). */
    ecs_alert_severity_filter_t severity_filters[ECS_ALERT_MAX_SEVERITY_FILTERS];

    /** The retain period specifies how long an alert must be inactive before it
     * is cleared. This makes it easier to track noisy alerts. While an alert is
     * inactive its duration won't increase. 
     * When the retain period is 0, the alert will clear immediately after it no
     * longer matches the alert query. */
    ecs_ftime_t retain_period;

    /** Alert when member value is out of range. Uses the warning/error ranges
     * assigned to the member in the MemberRanges component (optional). */
    ecs_entity_t member;

    /** (Component) id of member to monitor. If left to 0 this will be set to
     * the parent entity of the member (optional). */
    ecs_id_t id;

    /** Variable from which to fetch the member (optional). When left to NULL
     * 'id' will be obtained from $this. */
    const char *var;
} ecs_alert_desc_t;

/** Create a new alert.
 * An alert is a query that is evaluated periodically and creates alert 
 * instances for each entity that matches the query. Alerts can be used to 
 * automate detection of errors in an application.
 * 
 * Alerts are automatically cleared when a query is no longer true for an alert
 * instance. At most one alert instance will be created per matched entity.
 * 
 * Alert instances have three components:
 * - AlertInstance: contains the alert message for the instance
 * - MetricSource: contains the entity that triggered the alert
 * - MetricValue: contains how long the alert has been active
 * 
 * Alerts reuse components from the metrics addon so that alert instances can be
 * tracked and discovered as metrics. Just like metrics, alert instances are
 * created as children of the alert.
 * 
 * When an entity has active alerts, it will have the EcsAlertsActive component
 * which contains a map with active alerts for the entity. This component
 * will be automatically removed once all alerts are cleared for the entity.
 * 
 * @param world The world.
 * @param desc Alert description.
 * @return The alert entity.
 */
FLECS_API
ecs_entity_t ecs_alert_init(
    ecs_world_t *world,
    const ecs_alert_desc_t *desc);

#define ecs_alert(world, ...)\
    ecs_alert_init(world, &(ecs_alert_desc_t)__VA_ARGS__)

/** Return number of active alerts for entity.
 * When a valid alert entity is specified for the alert parameter, the operation
 * will return whether the specified alert is active for the entity. When no
 * alert is specified, the operation will return the total number of active
 * alerts for the entity.
 * 
 * @param world The world.
 * @param entity The entity.
 * @param alert The alert to test for (optional).
 * @return The number of active alerts for the entity.
 */
FLECS_API
int32_t ecs_get_alert_count(
    const ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_t alert);

/** Return alert instance for specified alert.
 * This operation returns the alert instance for the specified alert. If the
 * alert is not active for the entity, the operation will return 0.
 * 
 * @param world The world.
 * @param entity The entity.
 * @param alert The alert to test for.
 * @return The alert instance for the specified alert.
 */
FLECS_API
ecs_entity_t ecs_get_alert(
    const ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_t alert);

/* Module import */
FLECS_API
void FlecsAlertsImport(
    ecs_world_t *world);

#ifdef __cplusplus
}
#endif

#endif

/** @} */

#endif

#endif

#ifdef FLECS_MONITOR
#ifdef FLECS_NO_MONITOR
#error "FLECS_NO_MONITOR failed: MONITOR is required by other addons"
#endif
/**
 * @file addons/monitor.h
 * @brief Doc module.
 *
 * The monitor module automatically tracks statistics from the stats addon and
 * stores them in components.
 */

#ifdef FLECS_MONITOR

/**
 * @defgroup c_addons_monitor Monitor
 * @brief The monitor addon periodically tracks statistics for the world and systems.
 * 
 * \ingroup c_addons
 * @{
 */

#ifndef FLECS_MONITOR_H
#define FLECS_MONITOR_H

#ifndef FLECS_MODULE
#define FLECS_MODULE
#endif

#ifndef FLECS_STATS
#define FLECS_STATS
#endif

#ifdef __cplusplus
extern "C" {
#endif

FLECS_API extern ECS_COMPONENT_DECLARE(FlecsMonitor);
FLECS_API extern ECS_COMPONENT_DECLARE(EcsWorldStats);
FLECS_API extern ECS_COMPONENT_DECLARE(EcsWorldSummary);
FLECS_API extern ECS_COMPONENT_DECLARE(EcsPipelineStats);

FLECS_API extern ecs_entity_t EcsPeriod1s;
FLECS_API extern ecs_entity_t EcsPeriod1m;
FLECS_API extern ecs_entity_t EcsPeriod1h;
FLECS_API extern ecs_entity_t EcsPeriod1d;
FLECS_API extern ecs_entity_t EcsPeriod1w;

typedef struct {
    ecs_ftime_t elapsed;
    int32_t reduce_count;
} EcsStatsHeader;

typedef struct {
    EcsStatsHeader hdr; 
    ecs_world_stats_t stats;
} EcsWorldStats;

typedef struct {
    EcsStatsHeader hdr;
    ecs_pipeline_stats_t stats;
} EcsPipelineStats;

typedef struct {
    /* Target FPS */
    double target_fps;          /**< Target FPS */

    /* Total time */
    double frame_time_total;    /**< Total time spent processing a frame */
    double system_time_total;   /**< Total time spent in systems */
    double merge_time_total;    /**< Total time spent in merges */

    /* Last frame time */
    double frame_time_last;     /**< Time spent processing a frame */
    double system_time_last;    /**< Time spent in systems */
    double merge_time_last;     /**< Time spent in merges */
} EcsWorldSummary;

/* Module import */
FLECS_API
void FlecsMonitorImport(
    ecs_world_t *world);

#ifdef __cplusplus
}
#endif

#endif

/** @} */

#endif

#endif

#ifdef FLECS_COREDOC
#ifdef FLECS_NO_COREDOC
#error "FLECS_NO_COREDOC failed: COREDOC is required by other addons"
#endif
/**
 * @file addons/coredoc.h
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

/**
 * @defgroup c_addons_coredoc Coredoc
 * @brief Module that adds documentation and reflection to core entities.
 * 
 * \ingroup c_addons
 * @{
 */

/* Module import */

FLECS_API
void FlecsCoreDocImport(
    ecs_world_t *world);

/* @} */

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
 * @file addons/doc.h
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

/**
 * @defgroup c_addons_doc Doc
 * @brief Utilities for documenting entities, components and systems.
 * 
 * \ingroup c_addons
 * @{
 */

FLECS_API extern const ecs_entity_t ecs_id(EcsDocDescription);
FLECS_API extern const ecs_entity_t EcsDocBrief;
FLECS_API extern const ecs_entity_t EcsDocDetail;
FLECS_API extern const ecs_entity_t EcsDocLink;
FLECS_API extern const ecs_entity_t EcsDocColor;

typedef struct EcsDocDescription {
    char *value;
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

/** Add color to entity.
 * UIs can use color as hint to improve visualizing entities.
 * 
 * @param world The world.
 * @param entity The entity to which to add the link.
 * @param color The color to add.
 */
FLECS_API
void ecs_doc_set_color(
    ecs_world_t *world,
    ecs_entity_t entity,
    const char *color);

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

/** Get color from entity.
 * 
 * @param world The world.
 * @param entity The entity from which to get the link.
 * @return The color.
 */
FLECS_API
const char* ecs_doc_get_color(
    const ecs_world_t *world,
    ecs_entity_t entity);

/* Module import */
FLECS_API
void FlecsDocImport(
    ecs_world_t *world);

/* @} */

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
 * @file addons/json.h
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

/**
 * @defgroup c_addons_json Json
 * @brief Functions for serializing to/from JSON.
 * 
 * \ingroup c_addons
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/** Used with ecs_ptr_from_json, ecs_entity_from_json. */
typedef struct ecs_from_json_desc_t {
    const char *name; /**< Name of expression (used for logging) */
    const char *expr; /**< Full expression (used for logging) */

    /** Callback that allows for specifying a custom lookup function. The 
     * default behavior uses ecs_lookup_fullpath */
    ecs_entity_t (*lookup_action)(
        const ecs_world_t*, 
        const char *value, 
        void *ctx);
    void *lookup_ctx;
} ecs_from_json_desc_t;

/** Parse JSON string into value.
 * This operation parses a JSON expression into the provided pointer. The
 * memory pointed to must be large enough to contain a value of the used type.
 * 
 * @param world The world.
 * @param type The type of the expression to parse.
 * @param ptr Pointer to the memory to write to.
 * @param json The JSON expression to parse.
 * @param desc Configuration parameters for deserializer.
 * @return Pointer to the character after the last one read, or NULL if failed.
 */
FLECS_API
const char* ecs_ptr_from_json(
    const ecs_world_t *world,
    ecs_entity_t type,
    void *ptr,
    const char *json,
    const ecs_from_json_desc_t *desc);

/** Parse JSON object with multiple component values into entity. The format
 * is the same as the one outputted by ecs_entity_to_json, but at the moment
 * only supports the "ids" and "values" member. 
 * 
 * @param world The world.
 * @param entity The entity to serialize to.
 * @param json The JSON expression to parse (see entity in JSON format manual).
 * @param desc Configuration parameters for deserializer.
 * @return Pointer to the character after the last one read, or NULL if failed.
 */
FLECS_API
const char* ecs_entity_from_json(
    ecs_world_t *world,
    ecs_entity_t entity,
    const char *json,
    const ecs_from_json_desc_t *desc);

/** Parse JSON object with multiple entities into the world. The format is the
 * same as the one outputted by ecs_world_to_json. 
 * 
 * @param world The world.
 * @param json The JSON expression to parse (see iterator in JSON format manual).
 */
FLECS_API
const char* ecs_world_from_json(
    ecs_world_t *world,
    const char *json,
    const ecs_from_json_desc_t *desc);

/** Serialize array into JSON string.
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

/** Serialize array into JSON string buffer.
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
    bool serialize_path;       /**< Serialize full pathname */
    bool serialize_meta_ids;   /**< Serialize 'meta' ids (Name, ChildOf, etc) */
    bool serialize_label;      /**< Serialize doc name */
    bool serialize_brief;      /**< Serialize brief doc description */
    bool serialize_link;       /**< Serialize doc link (URL) */
    bool serialize_color;      /**< Serialize doc color */
    bool serialize_id_labels;  /**< Serialize labels of (component) ids */
    bool serialize_base;       /**< Serialize base components */
    bool serialize_private;    /**< Serialize private components */
    bool serialize_hidden;     /**< Serialize ids hidden by override */
    bool serialize_values;     /**< Serialize component values */
    bool serialize_type_info;  /**< Serialize type info (requires serialize_values) */
    bool serialize_alerts;     /**< Serialize active alerts for entity */
} ecs_entity_to_json_desc_t;

#define ECS_ENTITY_TO_JSON_INIT (ecs_entity_to_json_desc_t){true, false,\
    false, false, false, false, false, true, false, false, false, false, false }

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
    bool serialize_term_ids;        /**< Serialize term (query) component ids */
    bool serialize_ids;             /**< Serialize actual (matched) component ids */
    bool serialize_sources;         /**< Serialize sources */
    bool serialize_variables;       /**< Serialize variables */
    bool serialize_is_set;          /**< Serialize is_set (for optional terms) */
    bool serialize_values;          /**< Serialize component values */
    bool serialize_entities;        /**< Serialize entities (for This terms) */
    bool serialize_entity_labels;   /**< Serialize doc name for entities */
    bool serialize_entity_ids;      /**< Serialize numerical ids for entities */
    bool serialize_entity_names;    /**< Serialize names (not paths) for entities */
    bool serialize_variable_labels; /**< Serialize doc name for variables */
    bool serialize_variable_ids;    /**< Serialize numerical ids for variables */
    bool serialize_colors;          /**< Serialize doc color for entities */
    bool measure_eval_duration;     /**< Serialize evaluation duration */
    bool serialize_type_info;       /**< Serialize type information */
    bool serialize_table;           /**< Serialize entire table vs. matched components */
} ecs_iter_to_json_desc_t;

#define ECS_ITER_TO_JSON_INIT (ecs_iter_to_json_desc_t){\
    .serialize_term_ids =        true,  \
    .serialize_ids =             true,  \
    .serialize_sources =         true,  \
    .serialize_variables =       true,  \
    .serialize_is_set =          true,  \
    .serialize_values =          true,  \
    .serialize_entities =        true,  \
    .serialize_entity_labels =   false, \
    .serialize_entity_ids =      false, \
    .serialize_entity_names =    false, \
    .serialize_variable_labels = false, \
    .serialize_variable_ids =    false, \
    .serialize_colors =          false, \
    .measure_eval_duration =     false, \
    .serialize_type_info =       false, \
    .serialize_table =           false  \
}

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

/** Used with ecs_iter_to_json. */
typedef struct ecs_world_to_json_desc_t {
    bool serialize_builtin;    /**< Exclude flecs modules & contents */
    bool serialize_modules;    /**< Exclude modules & contents */
} ecs_world_to_json_desc_t;

/** Serialize world into JSON string.
 * This operation iterates the contents of the world to JSON. The operation is
 * equivalent to the following code:
 * 
 * ecs_filter_t *f = ecs_filter(world, {
 *   .terms = {{ .id = EcsAny }}
 * });
 * 
 * ecs_iter_t it = ecs_filter_init(world, &f);
 * ecs_iter_to_json_desc_t desc = { .serialize_table = true };
 * ecs_iter_to_json(world, iter, &desc);
 * 
 * @param world The world to serialize.
 * @return A JSON string with the serialized iterator data, or NULL if failed.
 */
FLECS_API
char* ecs_world_to_json(
    ecs_world_t *world,
    const ecs_world_to_json_desc_t *desc);

/** Serialize world into JSON string buffer.
 * Same as ecs_world_to_json, but serializes to an ecs_strbuf_t instance.
 * 
 * @param world The world to serialize.
 * @param buf_out The strbuf to append the string to.
 * @return Zero if success, non-zero if failed.
 */
FLECS_API
int ecs_world_to_json_buf(
    ecs_world_t *world,
    ecs_strbuf_t *buf_out,
    const ecs_world_to_json_desc_t *desc);

#ifdef __cplusplus
}
#endif

#endif

/** @} */

#endif

#endif

#if defined(FLECS_EXPR) || defined(FLECS_META_C)
#ifndef FLECS_META
#define FLECS_META
#endif
#endif
#ifdef FLECS_UNITS
#ifdef FLECS_NO_UNITS
#error "FLECS_NO_UNITS failed: UNITS is required by other addons"
#endif
/**
 * @file addons/units.h
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

/**
 * @defgroup c_addons_units Units.
 * @brief Common unit annotations for reflection framework.
 * 
 * \ingroup c_addons
 * @{
 */

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

/**
 * @defgroup c_addons_units_prefixes Prefixes
 * @brief Prefixes to indicate unit count (e.g. Kilo, Mega)
 * 
 * \ingroup c_addons_units
 * @{
 */

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

/** @} */

/**
 * @defgroup c_addons_units_duration Duration
 * 
 * \ingroup c_addons_units
 * @{
 */

FLECS_API extern ECS_DECLARE(EcsDuration);
FLECS_API extern     ECS_DECLARE(EcsPicoSeconds);
FLECS_API extern     ECS_DECLARE(EcsNanoSeconds);
FLECS_API extern     ECS_DECLARE(EcsMicroSeconds);
FLECS_API extern     ECS_DECLARE(EcsMilliSeconds);
FLECS_API extern     ECS_DECLARE(EcsSeconds);
FLECS_API extern     ECS_DECLARE(EcsMinutes);
FLECS_API extern     ECS_DECLARE(EcsHours);
FLECS_API extern     ECS_DECLARE(EcsDays);

/** @} */

/**
 * @defgroup c_addons_units_time Time
 * 
 * \ingroup c_addons_units
 * @{
 */

FLECS_API extern ECS_DECLARE(EcsTime);
FLECS_API extern     ECS_DECLARE(EcsDate);

/** @} */

/**
 * @defgroup c_addons_units_mass Mass
 * 
 * \ingroup c_addons_units
 * @{
 */

FLECS_API extern ECS_DECLARE(EcsMass);
FLECS_API extern     ECS_DECLARE(EcsGrams);
FLECS_API extern     ECS_DECLARE(EcsKiloGrams);

/** @} */

/**
 * @defgroup c_addons_units_electric_Current Electric Current
 * 
 * \ingroup c_addons_units
 * @{
 */

FLECS_API extern ECS_DECLARE(EcsElectricCurrent);
FLECS_API extern     ECS_DECLARE(EcsAmpere);

/** @} */

/**
 * @defgroup c_addons_units_amount Amount
 * 
 * \ingroup c_addons_units
 * @{
 */

FLECS_API extern ECS_DECLARE(EcsAmount);
FLECS_API extern     ECS_DECLARE(EcsMole);

/** @} */

/**
 * @defgroup c_addons_units_luminous_intensity Luminous Intensity
 * 
 * \ingroup c_addons_units
 * @{
 */

FLECS_API extern ECS_DECLARE(EcsLuminousIntensity);
FLECS_API extern     ECS_DECLARE(EcsCandela);

/** @} */

/**
 * @defgroup c_addons_units_force Force
 * 
 * \ingroup c_addons_units
 * @{
 */

FLECS_API extern ECS_DECLARE(EcsForce);
FLECS_API extern     ECS_DECLARE(EcsNewton);

/** @} */

/**
 * @defgroup c_addons_units_length Length
 * 
 * \ingroup c_addons_units
 * @{
 */

FLECS_API extern ECS_DECLARE(EcsLength);
FLECS_API extern     ECS_DECLARE(EcsMeters);
FLECS_API extern         ECS_DECLARE(EcsPicoMeters);
FLECS_API extern         ECS_DECLARE(EcsNanoMeters);
FLECS_API extern         ECS_DECLARE(EcsMicroMeters);
FLECS_API extern         ECS_DECLARE(EcsMilliMeters);
FLECS_API extern         ECS_DECLARE(EcsCentiMeters);
FLECS_API extern         ECS_DECLARE(EcsKiloMeters);
FLECS_API extern     ECS_DECLARE(EcsMiles);
FLECS_API extern     ECS_DECLARE(EcsPixels);

/** @} */

/**
 * @defgroup c_addons_units_pressure Pressure
 * 
 * \ingroup c_addons_units
 * @{
 */

FLECS_API extern ECS_DECLARE(EcsPressure);
FLECS_API extern     ECS_DECLARE(EcsPascal);
FLECS_API extern     ECS_DECLARE(EcsBar);

/** @} */

/**
 * @defgroup c_addons_units_speed Speed
 * 
 * \ingroup c_addons_units
 * @{
 */

FLECS_API extern ECS_DECLARE(EcsSpeed);
FLECS_API extern     ECS_DECLARE(EcsMetersPerSecond);
FLECS_API extern     ECS_DECLARE(EcsKiloMetersPerSecond);
FLECS_API extern     ECS_DECLARE(EcsKiloMetersPerHour);
FLECS_API extern     ECS_DECLARE(EcsMilesPerHour);

/** @} */

/**
 * @defgroup c_addons_units_temperature Temperature
 * 
 * \ingroup c_addons_units
 * @{
 */

FLECS_API extern ECS_DECLARE(EcsTemperature);
FLECS_API extern     ECS_DECLARE(EcsKelvin);
FLECS_API extern     ECS_DECLARE(EcsCelsius);
FLECS_API extern     ECS_DECLARE(EcsFahrenheit);

/** @} */

/**
 * @defgroup c_addons_units_data Data
 * 
 * \ingroup c_addons_units
 * @{
 */

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

/** @} */

/**
 * @defgroup c_addons_units_datarate Data Rate
 * 
 * \ingroup c_addons_units
 * @{
 */

FLECS_API extern ECS_DECLARE(EcsDataRate);
FLECS_API extern     ECS_DECLARE(EcsBitsPerSecond);
FLECS_API extern     ECS_DECLARE(EcsKiloBitsPerSecond);
FLECS_API extern     ECS_DECLARE(EcsMegaBitsPerSecond);
FLECS_API extern     ECS_DECLARE(EcsGigaBitsPerSecond);
FLECS_API extern     ECS_DECLARE(EcsBytesPerSecond);
FLECS_API extern     ECS_DECLARE(EcsKiloBytesPerSecond);
FLECS_API extern     ECS_DECLARE(EcsMegaBytesPerSecond);
FLECS_API extern     ECS_DECLARE(EcsGigaBytesPerSecond);

/** @} */

/**
 * @defgroup c_addons_units_duration Duration
 * 
 * \ingroup c_addons_units
 * @{
 */

FLECS_API extern ECS_DECLARE(EcsAngle);
FLECS_API extern     ECS_DECLARE(EcsRadians);
FLECS_API extern     ECS_DECLARE(EcsDegrees);

/** @} */

/**
 * @defgroup c_addons_units_angle Angle
 * 
 * \ingroup c_addons_units
 * @{
 */

FLECS_API extern ECS_DECLARE(EcsFrequency);
FLECS_API extern     ECS_DECLARE(EcsHertz);
FLECS_API extern     ECS_DECLARE(EcsKiloHertz);
FLECS_API extern     ECS_DECLARE(EcsMegaHertz);
FLECS_API extern     ECS_DECLARE(EcsGigaHertz);

/** @} */

/**
 * @defgroup c_addons_units_uri Uri
 * 
 * \ingroup c_addons_units
 * @{
 */

FLECS_API extern ECS_DECLARE(EcsUri);
FLECS_API extern     ECS_DECLARE(EcsUriHyperlink);
FLECS_API extern     ECS_DECLARE(EcsUriImage);
FLECS_API extern     ECS_DECLARE(EcsUriFile);

/** @} */

FLECS_API extern ECS_DECLARE(EcsAcceleration);
FLECS_API extern ECS_DECLARE(EcsPercentage);
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

/** @} */

#endif

#endif

#ifdef FLECS_META
#ifdef FLECS_NO_META
#error "FLECS_NO_META failed: META is required by other addons"
#endif
/**
 * @file addons/meta.h
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
 * ecs_entity_t pos = ecs_struct_init(world, &(ecs_struct_desc_t){
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

/**
 * @defgroup c_addons_meta Meta
 * @brief Flecs reflection framework.
 * 
 * \ingroup c_addons
 * @{
 */

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

/* Meta module component ids */
FLECS_API extern const ecs_entity_t ecs_id(EcsMetaType);
FLECS_API extern const ecs_entity_t ecs_id(EcsMetaTypeSerialized);
FLECS_API extern const ecs_entity_t ecs_id(EcsPrimitive);
FLECS_API extern const ecs_entity_t ecs_id(EcsEnum);
FLECS_API extern const ecs_entity_t ecs_id(EcsBitmask);
FLECS_API extern const ecs_entity_t ecs_id(EcsMember);
FLECS_API extern const ecs_entity_t ecs_id(EcsMemberRanges);
FLECS_API extern const ecs_entity_t ecs_id(EcsStruct);
FLECS_API extern const ecs_entity_t ecs_id(EcsArray);
FLECS_API extern const ecs_entity_t ecs_id(EcsVector);
FLECS_API extern const ecs_entity_t ecs_id(EcsOpaque);
FLECS_API extern const ecs_entity_t ecs_id(EcsUnit);
FLECS_API extern const ecs_entity_t ecs_id(EcsUnitPrefix);
FLECS_API extern const ecs_entity_t EcsConstant;
FLECS_API extern const ecs_entity_t EcsQuantity;

/* Primitive type component ids */
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

/** Type kinds supported by meta addon */
typedef enum ecs_type_kind_t {
    EcsPrimitiveType,
    EcsBitmaskType,
    EcsEnumType,
    EcsStructType,
    EcsArrayType,
    EcsVectorType,
    EcsOpaqueType,
    EcsTypeKindLast = EcsOpaqueType
} ecs_type_kind_t;

/** Component that is automatically added to every type with the right kind. */
typedef struct EcsMetaType {
    ecs_type_kind_t kind;
    bool existing;         /**< Did the type exist or is it populated from reflection */
    bool partial;          /**< Is the reflection data a partial type description */
    ecs_size_t size;       /**< Computed size */
    ecs_size_t alignment;  /**< Computed alignment */
} EcsMetaType;

/** Primitive type kinds supported by meta addon */
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

/** Component added to primitive types */
typedef struct EcsPrimitive {
    ecs_primitive_kind_t kind;
} EcsPrimitive;

/** Component added to member entities */
typedef struct EcsMember {
    ecs_entity_t type;
    int32_t count;
    ecs_entity_t unit;
    int32_t offset;
} EcsMember;

/** Type expressing a range for a member value */
typedef struct ecs_member_value_range_t {
    double min;
    double max;
} ecs_member_value_range_t;

/** Component added to member entities to express valid value ranges */
typedef struct EcsMemberRanges {
    ecs_member_value_range_t value;
    ecs_member_value_range_t warning;
    ecs_member_value_range_t error;
} EcsMemberRanges;

/** Element type of members vector in EcsStruct */
typedef struct ecs_member_t {
    /** Must be set when used with ecs_struct_desc_t */
    const char *name;
    ecs_entity_t type;

    /** May be set when used with ecs_struct_desc_t */
    int32_t count;
    int32_t offset;

    /** May be set when used with ecs_struct_desc_t, will be auto-populated if
     * type entity is also a unit */
    ecs_entity_t unit;

    /** Numerical range that specifies which values member can assume. This 
     * range may be used by UI elements such as a progress bar or slider. The
     * value of a member should not exceed this range. */
    ecs_member_value_range_t range;

    /** Numerical range outside of which the value represents an error. This 
     * range may be used by UI elements to style a value. */
    ecs_member_value_range_t error_range;

    /** Numerical range outside of which the value represents an warning. This 
     * range may be used by UI elements to style a value. */
    ecs_member_value_range_t warning_range;

    /** Should not be set by ecs_struct_desc_t */
    ecs_size_t size;
    ecs_entity_t member;
} ecs_member_t;

/** Component added to struct type entities */
typedef struct EcsStruct {
    /** Populated from child entities with Member component */
    ecs_vec_t members; /* vector<ecs_member_t> */
} EcsStruct;

typedef struct ecs_enum_constant_t {
    /** Must be set when used with ecs_enum_desc_t */
    const char *name;

    /** May be set when used with ecs_enum_desc_t */
    int32_t value;

    /** Should not be set by ecs_enum_desc_t */
    ecs_entity_t constant;
} ecs_enum_constant_t;

/** Component added to enum type entities */
typedef struct EcsEnum {
    /** Populated from child entities with Constant component */
    ecs_map_t constants; /* map<i32_t, ecs_enum_constant_t> */
} EcsEnum;

typedef struct ecs_bitmask_constant_t {
    /** Must be set when used with ecs_bitmask_desc_t */
    const char *name;

    /** May be set when used with ecs_bitmask_desc_t */
    ecs_flags32_t value;

    /** Should not be set by ecs_bitmask_desc_t */
    ecs_entity_t constant;
} ecs_bitmask_constant_t;

/** Component added to bitmask type entities */
typedef struct EcsBitmask {
    /* Populated from child entities with Constant component */
    ecs_map_t constants; /* map<u32_t, ecs_bitmask_constant_t> */
} EcsBitmask;

/** Component added to array type entities */
typedef struct EcsArray {
    ecs_entity_t type; /**< Element type */
    int32_t count;     /**< Number of elements */
} EcsArray;

/** Component added to vector type entities */
typedef struct EcsVector {
    ecs_entity_t type; /**< Element type */
} EcsVector;


/* Opaque type support */

#if !defined(__cplusplus) || !defined(FLECS_CPP)

/** Serializer interface */
typedef struct ecs_serializer_t {
    /* Serialize value */
    int (*value)(
        const struct ecs_serializer_t *ser, /**< Serializer */
        ecs_entity_t type,                  /**< Type of the value to serialize */
        const void *value);                 /**< Pointer to the value to serialize */

    /* Serialize member */
    int (*member)(
        const struct ecs_serializer_t *ser, /**< Serializer */
        const char *member);                /**< Member name */

    const ecs_world_t *world;
    void *ctx;
} ecs_serializer_t;

#elif defined(__cplusplus)

} /* extern "C" { */

/** Serializer interface (same layout as C, but with convenience methods) */
typedef struct ecs_serializer_t {
    /* Serialize value */
    int (*value_)(
        const struct ecs_serializer_t *ser,
        ecs_entity_t type,
        const void *value);

    /* Serialize member */
    int (*member_)(
        const struct ecs_serializer_t *ser,
        const char *name);

    /* Serialize value */
    int value(ecs_entity_t type, const void *value) const;
    
    /* Serialize value */
    template <typename T>
    int value(const T& value) const;

    /* Serialize member */
    int member(const char *name) const;

    const ecs_world_t *world;
    void *ctx;
} ecs_serializer_t;

extern "C" {
#endif

/** Callback invoked serializing an opaque type. */
typedef int (*ecs_meta_serialize_t)(
    const ecs_serializer_t *ser,
    const void *src);                  /**< Pointer to value to serialize */

typedef struct EcsOpaque {
    ecs_entity_t as_type;              /**< Type that describes the serialized output */
    ecs_meta_serialize_t serialize;    /**< Serialize action */

    /* Deserializer interface 
     * Only override the callbacks that are valid for the opaque type. If a
     * deserializer attempts to assign a value type that is not supported by the
     * interface, a conversion error is thrown.
     */

    /** Assign bool value */
    void (*assign_bool)(
        void *dst, 
        bool value);

    /** Assign char value */
    void (*assign_char)(
        void *dst, 
        char value);

    /** Assign int value */
    void (*assign_int)(
        void *dst, 
        int64_t value);

    /** Assign unsigned int value */
    void (*assign_uint)(
        void *dst, 
        uint64_t value);

    /** Assign float value */
    void (*assign_float)(
        void *dst, 
        double value);

    /** Assign string value */
    void (*assign_string)(
        void *dst, 
        const char *value);

    /** Assign entity value */
    void (*assign_entity)(
        void *dst,
        ecs_world_t *world,
        ecs_entity_t entity);

    /** Assign null value */
    void (*assign_null)(
        void *dst);

    /** Clear collection elements */
    void (*clear)(
        void *dst);

    /** Ensure & get collection element */
    void* (*ensure_element)(
        void *dst, 
        size_t elem);

    /** Ensure & get element */
    void* (*ensure_member)(
        void *dst, 
        const char *member);

    /** Return number of elements */
    size_t (*count)(
        const void *dst);
    
    /** Resize to number of elements */
    void (*resize)(
        void *dst, 
        size_t count);
} EcsOpaque;


/* Units */

/* Helper type to describe translation between two units. Note that this
 * is not intended as a generic approach to unit conversions (e.g. from celsius
 * to fahrenheit) but to translate between units that derive from the same base 
 * (e.g. meters to kilometers). 
 * 
 * Note that power is applied to the factor. When describing a translation of
 * 1000, either use {factor = 1000, power = 1} or {factor = 1, power = 3}. */
typedef struct ecs_unit_translation_t {
    int32_t factor; /**< Factor to apply (e.g. "1000", "1000000", "1024") */
    int32_t power;  /**< Power to apply to factor (e.g. "1", "3", "-9") */
} ecs_unit_translation_t;

typedef struct EcsUnit {
    char *symbol;
    ecs_entity_t prefix; /**< Order of magnitude prefix relative to derived */
    ecs_entity_t base;   /**< Base unit (e.g. "meters") */
    ecs_entity_t over;   /**< Over unit (e.g. "per second") */
    ecs_unit_translation_t translation; /**< Translation for derived unit */
} EcsUnit;

typedef struct EcsUnitPrefix {
    char *symbol;        /**< Symbol of prefix (e.g. "K", "M", "Ki") */
    ecs_unit_translation_t translation; /**< Translation of prefix */
} EcsUnitPrefix;


/* Serializer utilities */

typedef enum ecs_meta_type_op_kind_t {
    EcsOpArray,
    EcsOpVector,
    EcsOpOpaque,
    EcsOpPush,
    EcsOpPop,

    EcsOpScope, /**< Marks last constant that can open/close a scope */

    EcsOpEnum,
    EcsOpBitmask,

    EcsOpPrimitive, /**< Marks first constant that's a primitive */

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
    EcsOpEntity,
    EcsMetaTypeOpKindLast = EcsOpEntity
} ecs_meta_type_op_kind_t;

typedef struct ecs_meta_type_op_t {
    ecs_meta_type_op_kind_t kind;
    ecs_size_t offset;      /**< Offset of current field */
    int32_t count;
    const char *name;       /**< Name of value (only used for struct members) */
    int32_t op_count;       /**< Number of operations until next field or end */
    ecs_size_t size;        /**< Size of type of operation */
    ecs_entity_t type;      /**< Type entity */
    int32_t member_index;   /**< Index of member in struct */
    ecs_hashmap_t *members; /**< string -> member index (structs only) */
} ecs_meta_type_op_t;

typedef struct EcsMetaTypeSerialized {
    ecs_vec_t ops;      /**< vector<ecs_meta_type_op_t> */
} EcsMetaTypeSerialized;


/* Deserializer utilities */

#define ECS_META_MAX_SCOPE_DEPTH (32) /* >32 levels of nesting is not sane */

typedef struct ecs_meta_scope_t {
    ecs_entity_t type;        /**< The type being iterated */
    ecs_meta_type_op_t *ops;  /**< The type operations (see ecs_meta_type_op_t) */
    int32_t op_count;         /**< Number of operations in ops array to process */
    int32_t op_cur;           /**< Current operation */
    int32_t elem_cur;         /**< Current element (for collections) */
    int32_t prev_depth;       /**< Depth to restore, in case dotmember was used */
    void *ptr;                /**< Pointer to the value being iterated */

    const EcsComponent *comp; /**< Pointer to component, in case size/alignment is needed */
    const EcsOpaque *opaque;  /**< Opaque type interface */ 
    ecs_vec_t *vector;        /**< Current vector, in case a vector is iterated */
    ecs_hashmap_t *members;   /**< string -> member index */
    bool is_collection;       /**< Is the scope iterating elements? */
    bool is_inline_array;     /**< Is the scope iterating an inline array? */
    bool is_empty_scope;      /**< Was scope populated (for collections) */
} ecs_meta_scope_t;

/** Type that enables iterating/populating a value using reflection data */
typedef struct ecs_meta_cursor_t {
    const ecs_world_t *world;
    ecs_meta_scope_t scope[ECS_META_MAX_SCOPE_DEPTH];
    int32_t depth;
    bool valid;
    bool is_primitive_scope;  /**< If in root scope, this allows for a push for primitive types */

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

/** Move cursor to a element */
FLECS_API
int ecs_meta_elem(
    ecs_meta_cursor_t *cursor,
    int32_t elem);

/** Move cursor to member */
FLECS_API
int ecs_meta_member(
    ecs_meta_cursor_t *cursor,
    const char *name);

/** Move cursor to member, supports dot-separated nested members */
FLECS_API
int ecs_meta_dotmember(
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

/* The set functions assign the field with the specified value. If the value
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

/** Set field with dynamic value */
FLECS_API
int ecs_meta_set_value(
    ecs_meta_cursor_t *cursor,
    const ecs_value_t *value);

/* Functions for getting members. */

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

/** Convert pointer of primitive kind to float. */
FLECS_API
double ecs_meta_ptr_to_float(
    ecs_primitive_kind_t type_kind,
    const void *ptr);

/* API functions for creating meta types */

/** Used with ecs_primitive_init. */
typedef struct ecs_primitive_desc_t {
    ecs_entity_t entity; /**< Existing entity to use for type (optional) */
    ecs_primitive_kind_t kind;
} ecs_primitive_desc_t;

/** Create a new primitive type */
FLECS_API
ecs_entity_t ecs_primitive_init(
    ecs_world_t *world,
    const ecs_primitive_desc_t *desc);

/** Used with ecs_enum_init. */
typedef struct ecs_enum_desc_t {
    ecs_entity_t entity; /**< Existing entity to use for type (optional) */
    ecs_enum_constant_t constants[ECS_MEMBER_DESC_CACHE_SIZE];
} ecs_enum_desc_t;

/** Create a new enum type */
FLECS_API
ecs_entity_t ecs_enum_init(
    ecs_world_t *world,
    const ecs_enum_desc_t *desc);


/** Used with ecs_bitmask_init. */
typedef struct ecs_bitmask_desc_t {
    ecs_entity_t entity; /**< Existing entity to use for type (optional) */
    ecs_bitmask_constant_t constants[ECS_MEMBER_DESC_CACHE_SIZE];
} ecs_bitmask_desc_t;

/** Create a new bitmask type */
FLECS_API
ecs_entity_t ecs_bitmask_init(
    ecs_world_t *world,
    const ecs_bitmask_desc_t *desc);


/** Used with ecs_array_init. */
typedef struct ecs_array_desc_t {
    ecs_entity_t entity; /**< Existing entity to use for type (optional) */
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
    ecs_entity_t entity; /**< Existing entity to use for type (optional) */
    ecs_entity_t type;
} ecs_vector_desc_t;

/** Create a new vector type */
FLECS_API
ecs_entity_t ecs_vector_init(
    ecs_world_t *world,
    const ecs_vector_desc_t *desc);


/** Used with ecs_struct_init. */
typedef struct ecs_struct_desc_t {
    ecs_entity_t entity; /**< Existing entity to use for type (optional) */
    ecs_member_t members[ECS_MEMBER_DESC_CACHE_SIZE];
} ecs_struct_desc_t;

/** Create a new struct type */
FLECS_API
ecs_entity_t ecs_struct_init(
    ecs_world_t *world,
    const ecs_struct_desc_t *desc);

/** Used with ecs_opaque_init. */
typedef struct ecs_opaque_desc_t {
    ecs_entity_t entity;
    EcsOpaque type;
} ecs_opaque_desc_t;

/** Create a new opaque type.
 * Opaque types are types of which the layout doesn't match what can be modelled
 * with the primitives of the meta framework, but which have a structure
 * that can be described with meta primitives. Typical examples are STL types
 * such as std::string or std::vector, types with a nontrivial layout, and types
 * that only expose getter/setter methods.
 * 
 * An opaque type is a combination of a serialization function, and a handle to
 * a meta type which describes the structure of the serialized output. For
 * example, an opaque type for std::string would have a serializer function that
 * accesses .c_str(), and with type ecs_string_t.
 * 
 * The serializer callback accepts a serializer object and a pointer to the 
 * value of the opaque type to be serialized. The serializer has two methods:
 * 
 * - value, which serializes a value (such as .c_str())
 * - member, which specifies a member to be serialized (in the case of a struct)
 */
FLECS_API
ecs_entity_t ecs_opaque_init(
    ecs_world_t *world,
    const ecs_opaque_desc_t *desc);

/** Used with ecs_unit_init. */
typedef struct ecs_unit_desc_t {
    /** Existing entity to associate with unit (optional) */
    ecs_entity_t entity;

    /** Unit symbol, e.g. "m", "%", "g". (optional) */
    const char *symbol;

    /** Unit quantity, e.g. distance, percentage, weight. (optional) */
    ecs_entity_t quantity;

    /** Base unit, e.g. "meters" (optional) */
    ecs_entity_t base;

    /** Over unit, e.g. "per second" (optional) */
    ecs_entity_t over;

    /** Translation to apply to derived unit (optional) */
    ecs_unit_translation_t translation;

    /** Prefix indicating order of magnitude relative to the derived unit. If set
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
    /** Existing entity to associate with unit prefix (optional) */
    ecs_entity_t entity;

    /** Unit symbol, e.g. "m", "%", "g". (optional) */
    const char *symbol;

    /** Translation to apply to derived unit (optional) */
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

/* Convenience macros */

#define ecs_primitive(world, ...)\
    ecs_primitive_init(world, &(ecs_primitive_desc_t) __VA_ARGS__ )

#define ecs_enum(world, ...)\
    ecs_enum_init(world, &(ecs_enum_desc_t) __VA_ARGS__ )

#define ecs_bitmask(world, ...)\
    ecs_bitmask_init(world, &(ecs_bitmask_desc_t) __VA_ARGS__ )

#define ecs_array(world, ...)\
    ecs_array_init(world, &(ecs_array_desc_t) __VA_ARGS__ )

#define ecs_vector(world, ...)\
    ecs_vector_init(world, &(ecs_vector_desc_t) __VA_ARGS__ )

#define ecs_opaque(world, ...)\
    ecs_opaque_init(world, &(ecs_opaque_desc_t) __VA_ARGS__ )

#define ecs_struct(world, ...)\
    ecs_struct_init(world, &(ecs_struct_desc_t) __VA_ARGS__ )

#define ecs_unit(world, ...)\
    ecs_unit_init(world, &(ecs_unit_desc_t) __VA_ARGS__ )

#define ecs_unit_prefix(world, ...)\
    ecs_unit_prefix_init(world, &(ecs_unit_prefix_desc_t) __VA_ARGS__ )

#define ecs_quantity(world, ...)\
    ecs_quantity_init(world, &(ecs_entity_desc_t) __VA_ARGS__ )

/* Module import */
FLECS_API
void FlecsMetaImport(
    ecs_world_t *world);

#ifdef __cplusplus
}
#endif

#endif

/** @} */

#endif

#endif

#ifdef FLECS_EXPR
#ifdef FLECS_NO_EXPR
#error "FLECS_NO_EXPR failed: EXPR is required by other addons"
#endif
/**
 * @file addons/expr.h
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

/**
 * @defgroup c_addons_expr Expr
 * @brief Serialize/deserialize values to string.
 * 
 * \ingroup c_addons
 * @{
 */

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

/** Storage for parser variables. Variables make it possible to parameterize
 * expression strings, and are referenced with the $ operator (e.g. $var). */
typedef struct ecs_expr_var_t {
    char *name;
    ecs_value_t value;
    bool owned; /* Set to false if ecs_vars_t should not take ownership of var */
} ecs_expr_var_t;

typedef struct ecs_expr_var_scope_t {
    ecs_hashmap_t var_index;
    ecs_vec_t vars;
    struct ecs_expr_var_scope_t *parent;
} ecs_expr_var_scope_t;

typedef struct ecs_vars_t {
    ecs_world_t *world;
    ecs_expr_var_scope_t root;
    ecs_expr_var_scope_t *cur;
} ecs_vars_t;

/** Init variable storage */
FLECS_API
void ecs_vars_init(
    ecs_world_t *world,
    ecs_vars_t *vars);

/** Cleanup variable storage */
FLECS_API
void ecs_vars_fini(
    ecs_vars_t *vars);

/** Push variable scope */
FLECS_API
void ecs_vars_push(
    ecs_vars_t *vars);

/** Pop variable scope */
FLECS_API
int ecs_vars_pop(
    ecs_vars_t *vars);

/** Declare variable in current scope */
FLECS_API
ecs_expr_var_t* ecs_vars_declare(
    ecs_vars_t *vars,
    const char *name,
    ecs_entity_t type);

/** Declare variable in current scope from value.
 * This operation takes ownership of the value. The value pointer must be 
 * allocated with ecs_value_new.
 */
FLECS_API
ecs_expr_var_t* ecs_vars_declare_w_value(
    ecs_vars_t *vars,
    const char *name,
    ecs_value_t *value);

/** Lookup variable in scope and parent scopes */
FLECS_API
ecs_expr_var_t* ecs_vars_lookup(
    const ecs_vars_t *vars,
    const char *name);

/** Used with ecs_parse_expr. */
typedef struct ecs_parse_expr_desc_t {
    const char *name;
    const char *expr;
    ecs_entity_t (*lookup_action)(
        const ecs_world_t*, 
        const char *value, 
        void *ctx);
    void *lookup_ctx;
    ecs_vars_t *vars;
} ecs_parse_expr_desc_t;

/** Parse expression into value.
 * This operation parses a flecs expression into the provided pointer. The
 * memory pointed to must be large enough to contain a value of the used type.
 * 
 * If no type and pointer are provided for the value argument, the operation 
 * will discover the type from the expression and allocate storage for the 
 * value. The allocated value must be freed with ecs_value_free.
 * 
 * @param world The world.
 * @param ptr The pointer to the expression to parse.
 * @param value The value containing type & pointer to write to.
 * @param desc Configuration parameters for deserializer.
 * @return Pointer to the character after the last one read, or NULL if failed.
 */
FLECS_API
const char* ecs_parse_expr(
    ecs_world_t *world,
    const char *ptr,
    ecs_value_t *value,
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

/** Serialize value into expression buffer.
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

/** Similar as ecs_ptr_to_expr, but serializes values to string. 
 * Whereas the output of ecs_ptr_to_expr is a valid expression, the output of
 * ecs_ptr_to_str is a string representation of the value. In most cases the
 * output of the two operations is the same, but there are some differences:
 * - Strings are not quoted
 * 
 * @param world The world.
 * @param type The type of the value to serialize.
 * @param data The value to serialize.
 * @return String with result, or NULL if failed.
 */
FLECS_API
char* ecs_ptr_to_str(
    const ecs_world_t *world,
    ecs_entity_t type,
    const void *data);

/** Serialize value into string buffer.
 * Same as ecs_ptr_to_str, but serializes to an ecs_strbuf_t instance.
 * 
 * @param world The world.
 * @param type The type of the value to serialize.
 * @param data The value to serialize.
 * @param buf The strbuf to append the string to.
 * @return Zero if success, non-zero if failed.
 */
FLECS_API
int ecs_ptr_to_str_buf(
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

/** Evaluate interpolated expressions in string.
 * This operation evaluates expressions in a string, and replaces them with
 * their evaluated result. Supported expression formats are:
 *  - $variable_name
 *  - {expression}
 * 
 * The $, { and } characters can be escaped with a backslash (\).
 * 
 * @param world The world.
 * @param str The string to evaluate.
 * @param vars The variables to use for evaluation.
 */
FLECS_API
char* ecs_interpolate_string(
    ecs_world_t *world,
    const char *str,
    const ecs_vars_t *vars);

/** Convert iterator to vars 
 * This operation converts an iterator to a variable array. This allows for
 * using iterator results in expressions. The operation only converts a 
 * single result at a time, and does not progress the iterator.
 * 
 * Iterator fields with data will be made available as variables with as name
 * the field index (e.g. "$1"). The operation does not check if reflection data
 * is registered for a field type. If no reflection data is registered for the
 * type, using the field variable in expressions will fail.
 * 
 * Field variables will only contain single elements, even if the iterator 
 * returns component arrays. The offset parameter can be used to specify which
 * element in the component arrays to return. The offset parameter must be
 * smaller than it->count.
 * 
 * The operation will create a variable for query variables that contain a
 * single entity.
 * 
 * The operation will attempt to use existing variables. If a variable does not
 * yet exist, the operation will create it. If an existing variable exists with
 * a mismatching type, the operation will fail.
 * 
 * Accessing variables after progressing the iterator or after the iterator is
 * destroyed will result in undefined behavior.
 * 
 * If vars contains a variable that is not present in the iterator, the variable
 * will not be modified.
 * 
 * @param it The iterator to convert to variables.
 * @param vars The variables to write to.
 * @param offset The offset to the current element.
 */
FLECS_API
void ecs_iter_to_vars(
    const ecs_iter_t *it,
    ecs_vars_t *vars,
    int offset);

/** @} */

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
 * @file addons/meta_c.h
 * @brief Utility macros for populating reflection data in C.
 */

#ifdef FLECS_META_C

/**
 * @defgroup c_addons_meta_c Meta Utilities
 * @brief Macro utilities to automatically insert reflection data.
 * 
 * \ingroup c_addons
 * @{
 */

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
    ECS_META_IMPL_CALL(ECS_STRUCT_, ECS_META_IMPL, name, #__VA_ARGS__);\
    ECS_STRUCT_TYPE(name, __VA_ARGS__)

/** ECS_ENUM(name, body) */
#define ECS_ENUM(name, ...)\
    ECS_META_IMPL_CALL(ECS_ENUM_, ECS_META_IMPL, name, #__VA_ARGS__);\
    ECS_ENUM_TYPE(name, __VA_ARGS__)

/** ECS_BITMASK(name, body) */
#define ECS_BITMASK(name, ...)\
    ECS_META_IMPL_CALL(ECS_BITMASK_, ECS_META_IMPL, name, #__VA_ARGS__);\
    ECS_ENUM_TYPE(name, __VA_ARGS__)

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
    extern ECS_COMPONENT_DECLARE(name);\
    static const char *FLECS__##name##_desc = type_desc;\
    static ecs_type_kind_t FLECS__##name##_kind = EcsStructType;\
    ECS_COMPONENT_DECLARE(name) = 0

#define ECS_STRUCT_DECLARE(name, type_desc)\
    extern ECS_COMPONENT_DECLARE(name);\
    ECS_COMPONENT_DECLARE(name) = 0

#define ECS_STRUCT_EXTERN(name, type_desc)\
    extern ECS_COMPONENT_DECLARE(name)


/* ECS_ENUM implementation */
#define ECS_ENUM_TYPE(name, ...)\
    typedef enum __VA_ARGS__ name

#define ECS_ENUM_ECS_META_IMPL ECS_ENUM_IMPL

#define ECS_ENUM_IMPL(name, type_desc)\
    extern ECS_COMPONENT_DECLARE(name);\
    static const char *FLECS__##name##_desc = type_desc;\
    static ecs_type_kind_t FLECS__##name##_kind = EcsEnumType;\
    ECS_COMPONENT_DECLARE(name) = 0

#define ECS_ENUM_DECLARE(name, type_desc)\
    extern ECS_COMPONENT_DECLARE(name);\
    ECS_COMPONENT_DECLARE(name) = 0

#define ECS_ENUM_EXTERN(name, type_desc)\
    extern ECS_COMPONENT_DECLARE(name)


/* ECS_BITMASK implementation */
#define ECS_BITMASK_TYPE(name, ...)\
    typedef enum __VA_ARGS__ name

#define ECS_BITMASK_ECS_META_IMPL ECS_BITMASK_IMPL

#define ECS_BITMASK_IMPL(name, type_desc)\
    extern ECS_COMPONENT_DECLARE(name);\
    static const char *FLECS__##name##_desc = type_desc;\
    static ecs_type_kind_t FLECS__##name##_kind = EcsBitmaskType;\
    ECS_COMPONENT_DECLARE(name) = 0

#define ECS_BITMASK_DECLARE(name, type_desc)\
    extern ECS_COMPONENT_DECLARE(name);\
    ECS_COMPONENT_DECLARE(name) = 0

#define ECS_BITMASK_EXTERN(name, type_desc)\
    extern ECS_COMPONENT_DECLARE(name)

#ifdef __cplusplus
}
#endif

#endif // FLECS_META_C_H

/** @} */

#endif // FLECS_META_C

#endif

#ifdef FLECS_PLECS
#ifdef FLECS_NO_PLECS
#error "FLECS_NO_PLECS failed: PLECS is required by other addons"
#endif
/**
 * @file addons/plecs.h
 * @brief Flecs script module.
 * 
 * For script, see examples/plecs.
 */

#ifdef FLECS_PLECS

/**
 * @defgroup c_addons_plecs Flecs script
 * @brief Data definition format for loading entity data.
 * 
 * \ingroup c_addons
 * @{
 */

#ifndef FLECS_MODULE
#define FLECS_MODULE
#endif

#ifndef FLECS_PARSER
#define FLECS_PARSER
#endif

#ifndef FLECS_EXPR
#define FLECS_EXPR
#endif

#ifndef FLECS_PLECS_H
#define FLECS_PLECS_H

#ifdef __cplusplus
extern "C" {
#endif

FLECS_API
extern ECS_COMPONENT_DECLARE(EcsScript);

/* Script component */
typedef struct EcsScript {
    ecs_vec_t using_;
    char *script;
    ecs_vec_t prop_defaults;
    ecs_world_t *world;
} EcsScript;

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

/** Used with ecs_script_init */
typedef struct ecs_script_desc_t {
    ecs_entity_t entity;      /* Set to customize entity handle associated with script */
    const char *filename;     /* Set to load script from file */
    const char *str;          /* Set to parse script from string */
} ecs_script_desc_t;

/** Load managed script.
 * A managed script tracks which entities it creates, and keeps those entities
 * synchronized when the contents of the script are updated. When the script is
 * updated, entities that are no longer in the new version will be deleted.
 * 
 * This feature is experimental.
 * 
 * @param world The world.
 * @param desc Script descriptor.
 */
FLECS_API
ecs_entity_t ecs_script_init(
    ecs_world_t *world,
    const ecs_script_desc_t *desc);

#define ecs_script(world, ...)\
    ecs_script_init(world, &(ecs_script_desc_t) __VA_ARGS__)

/** Update script with new code. 
 * 
 * @param world The world.
 * @param script The script entity.
 * @param instance An assembly instance (optional).
 * @param str The script code.
 * @param vars Optional preset variables for script parameterization.
 */
FLECS_API
int ecs_script_update(
    ecs_world_t *world,
    ecs_entity_t script,
    ecs_entity_t instance,
    const char *str,
    ecs_vars_t *vars);

/** Clear all entities associated with script.
 *
 * @param world The world.
 * @param script The script entity.
 * @param instance The script instance.
 */
FLECS_API
void ecs_script_clear(
    ecs_world_t *world,
    ecs_entity_t script,
    ecs_entity_t instance);

/* Module import */
FLECS_API
void FlecsScriptImport(
    ecs_world_t *world);

#ifdef __cplusplus
}
#endif

#endif

/** @} */

#endif

#endif

#ifdef FLECS_RULES
#ifdef FLECS_NO_RULES
#error "FLECS_NO_RULES failed: RULES is required by other addons"
#endif
/**
 * @file addons/rules.h
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

/**
 * @defgroup c_addons_rules Rules
 * @brief Rules are an advanced query engine for matching against entity graphs.
 * 
 * \ingroup c_addons
 * @{
 */

#ifndef FLECS_RULES_H
#define FLECS_RULES_H

#ifdef __cplusplus
extern "C" {
#endif

/** Convenience macro for rule creation */
#define ecs_rule(world, ...)\
    ecs_rule_init(world, &(ecs_filter_desc_t) __VA_ARGS__ )

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
 *   desc.terms[0].second = { .name = "X", .var = EcsVarIsVariable }
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
    const ecs_rule_t *rule);

/** Convert rule to string with profile.
 * To use this you must set the EcsIterProfile flag on an iterator before 
 * starting uteration:
 *   it.flags |= EcsIterProfile 
 *
 * @param rule The rule.
 * @return The string
 */
FLECS_API
char* ecs_rule_str_w_profile(
    const ecs_rule_t *rule,
    const ecs_iter_t *it);

/** Populate variables from key-value string.
 * Convenience function to set rule variables from a key-value string separated
 * by comma's. The string must have the followig format:
 *   var_a: value, var_b: value
 * 
 * The key-value list may optionally be enclosed in parenthesis.
 * 
 * @param rule The rule.
 * @param it The iterator for which to set the variables.
 * @param expr The key-value expression.
 */
FLECS_API
const char* ecs_rule_parse_vars(
    ecs_rule_t *rule,
    ecs_iter_t *it,
    const char *expr);

#ifdef __cplusplus
}
#endif

#endif // FLECS_RULES_H

/** @} */

#endif // FLECS_RULES

#endif

#ifdef FLECS_SNAPSHOT
#ifdef FLECS_NO_SNAPSHOT
#error "FLECS_NO_SNAPSHOT failed: SNAPSHOT is required by other addons"
#endif
/**
 * @file addons/snapshot.h
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

/**
 * @defgroup c_addons_snapshot Snapshot
 * @brief Save & restore world.
 * 
 * \ingroup c_addons
 * @{
 */

#ifndef FLECS_SNAPSHOT_H
#define FLECS_SNAPSHOT_H

#ifdef __cplusplus
extern "C" {
#endif

/** A snapshot stores the state of a world in a particular point in time. */
typedef struct ecs_snapshot_t ecs_snapshot_t;

/** Create a snapshot.
 * This operation makes a copy of the current state of the world.
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

/** @} */

#endif

#endif

#ifdef FLECS_PARSER
#ifdef FLECS_NO_PARSER
#error "FLECS_NO_PARSER failed: PARSER is required by other addons"
#endif
/**
 * @file addons/parser.h
 * @brief Parser addon.
 *
 * The parser addon parses string expressions into lists of terms, and can be
 * used to construct filters, queries and types.
 */

#ifdef FLECS_PARSER

/**
 * @defgroup c_addons_parser Parser
 * @brief Query DSL parser and parsing utilities.
 * 
 * \ingroup c_addons
 * @{
 */

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
const char* ecs_parse_ws(
    const char *ptr);

/** Skip whitespace and newline characters.
 * This function skips whitespace characters.
 * 
 * @param ptr Pointer to (potential) whitespaces to skip.
 * @return Pointer to the next non-whitespace character.
 */
FLECS_API
const char* ecs_parse_ws_eol(
    const char *ptr);

/** Utility function to parse an identifier */
const char* ecs_parse_identifier(
    const char *name,
    const char *expr,
    const char *ptr,
    char *token_out);

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
    char *token_out,
    char delim);

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

/** @} */

#endif // FLECS_PARSER

#endif

#ifdef FLECS_OS_API_IMPL
#ifdef FLECS_NO_OS_API_IMPL
#error "FLECS_NO_OS_API_IMPL failed: OS_API_IMPL is required by other addons"
#endif
/**
 * @file addons/os_api_impl.h
 * @brief Default OS API implementation.
 */

#ifdef FLECS_OS_API_IMPL

/**
 * @defgroup c_addons_os_api_impl OS API Implementation
 * @brief Default implementation for OS API interface.
 * 
 * \ingroup c_addons
 * @{
 */

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

/** @} */

#endif // FLECS_OS_API_IMPL

#endif

#ifdef FLECS_MODULE
#ifdef FLECS_NO_MODULE
#error "FLECS_NO_MODULE failed: MODULE is required by other addons"
#endif
/**
 * @file addons/module.h
 * @brief Module addon.
 *
 * The module addon allows for creating and importing modules. Flecs modules 
 * enable applications to organize components and systems into reusable units of
 * code that can easily be across projects.
 */

#ifdef FLECS_MODULE

/**
 * @defgroup c_addons_module Module
 * @brief Modules organize components, systems and more in reusable units of code.
 * 
 * \ingroup c_addons
 * @{
 */

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

/** Same as ecs_import, but with name to scope conversion.
 * PascalCase names are automatically converted to scoped names.
 *
 * @param world The world.
 * @param module The module import function.
 * @param module_name_c The name of the module.
 * @return The module entity.
 */
FLECS_API
ecs_entity_t ecs_import_c(
    ecs_world_t *world,
    ecs_module_action_t module,
    const char *module_name_c);

/** Import a module from a library.
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

/** Register a new module. */
FLECS_API
ecs_entity_t ecs_module_init(
    ecs_world_t *world,
    const char *c_name,
    const ecs_component_desc_t *desc);

/** Define module. */
#define ECS_MODULE_DEFINE(world, id)\
    {\
        ecs_component_desc_t desc = {0};\
        desc.entity = ecs_id(id);\
        ecs_id(id) = ecs_module_init(world, #id, &desc);\
        ecs_set_scope(world, ecs_id(id));\
    }

#define ECS_MODULE(world, id)\
    ecs_entity_t ecs_id(id) = 0; ECS_MODULE_DEFINE(world, id)\
    (void)ecs_id(id)

/** Wrapper around ecs_import.
 * This macro provides a convenient way to load a module with the world. It can
 * be used like this:
 *
 * ECS_IMPORT(world, FlecsSystemsPhysics);
 */
#define ECS_IMPORT(world, id) ecs_import_c(world, id##Import, #id)

#ifdef __cplusplus
}
#endif

#endif

/** @} */

#endif

#endif

#ifdef FLECS_CPP
#ifdef FLECS_NO_CPP
#error "FLECS_NO_CPP failed: CPP is required by other addons"
#endif
/**
 * @file addons/flecs_cpp.h
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

// The functions in this file can be used from C or C++, but these macros are only relevant to C++.
#ifdef __cplusplus

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

#endif

FLECS_API
char* ecs_cpp_get_type_name(
    char *type_name, 
    const char *func_name,
    size_t len,
    size_t front_len);

FLECS_API
char* ecs_cpp_get_symbol_name(
    char *symbol_name,
    const char *type_name,
    size_t len);

FLECS_API
char* ecs_cpp_get_constant_name(
    char *constant_name,
    const char *func_name,
    size_t len,
    size_t back_len);

FLECS_API
const char* ecs_cpp_trim_module(
    ecs_world_t *world,
    const char *type_name);

FLECS_API
void ecs_cpp_component_validate(
    ecs_world_t *world,
    ecs_entity_t id,
    const char *name,
    const char *symbol,
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
    bool implicit_name,
    bool *existing_out);

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
    bool is_component,
    bool *existing_out);

FLECS_API
void ecs_cpp_enum_init(
    ecs_world_t *world,
    ecs_entity_t id);

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

#ifdef FLECS_META
FLECS_API
const ecs_member_t* ecs_cpp_last_member(
    const ecs_world_t *world, 
    ecs_entity_t type);
#endif

#ifdef __cplusplus
}
#endif

#endif // FLECS_CPP_H

#endif // FLECS_CPP


#ifdef __cplusplus
/**
 * @file addons/cpp/flecs.hpp
 * @brief Flecs C++11 API.
 */

#pragma once

// STL includes
#include <type_traits>

/**
 * @defgroup cpp C++ API
 * @{
 */

namespace flecs 
{

struct world;
struct world_async_stage;
struct iter;
struct entity_view;
struct entity;
struct type;
struct table;
struct table_range;
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
/**
 * @file addons/cpp/c_types.hpp
 * @brief Aliases for types/constants from C API
 */

#pragma once

namespace flecs {

/**
 * @defgroup cpp_globals API Types & Globals
 * @brief Types & constants bridged from C API.
 * 
 * \ingroup cpp_core
 * @{
 */

using world_t = ecs_world_t;
using world_info_t = ecs_world_info_t;
using query_group_info_t = ecs_query_group_info_t;
using id_t = ecs_id_t;
using entity_t = ecs_entity_t;
using type_t = ecs_type_t;
using table_t = ecs_table_t;
using filter_t = ecs_filter_t;
using observer_t = ecs_observer_t;
using query_t = ecs_query_t;
using rule_t = ecs_rule_t;
using ref_t = ecs_ref_t;
using iter_t = ecs_iter_t;
using type_info_t = ecs_type_info_t;
using type_hooks_t = ecs_type_hooks_t;
using flags32_t = ecs_flags32_t;

enum inout_kind_t {
    InOutDefault = EcsInOutDefault,
    InOutNone = EcsInOutNone,
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

/** Id flags */
static const flecs::entity_t Pair = ECS_PAIR;
static const flecs::entity_t Override = ECS_OVERRIDE;
static const flecs::entity_t Toggle = ECS_TOGGLE;

////////////////////////////////////////////////////////////////////////////////
//// Builtin components and tags 
////////////////////////////////////////////////////////////////////////////////

/* Builtin components */
using Component = EcsComponent;
using Identifier = EcsIdentifier;
using Poly = EcsPoly;
using Target = EcsTarget;

/* Builtin tags */
static const flecs::entity_t Query = EcsQuery;
static const flecs::entity_t Observer = EcsObserver;
static const flecs::entity_t Private = EcsPrivate;
static const flecs::entity_t Module = EcsModule;
static const flecs::entity_t Prefab = EcsPrefab;
static const flecs::entity_t Disabled = EcsDisabled;
static const flecs::entity_t Empty = EcsEmpty;
static const flecs::entity_t Monitor = EcsMonitor;
static const flecs::entity_t System = EcsSystem;
static const flecs::entity_t Pipeline = ecs_id(EcsPipeline);
static const flecs::entity_t Phase = EcsPhase;

/* Builtin event tags */
static const flecs::entity_t OnAdd = EcsOnAdd;
static const flecs::entity_t OnRemove = EcsOnRemove;
static const flecs::entity_t OnSet = EcsOnSet;
static const flecs::entity_t UnSet = EcsUnSet;
static const flecs::entity_t OnTableCreate = EcsOnTableCreate;
static const flecs::entity_t OnTableDelete = EcsOnTableDelete;

/* Builtin term flags */
static const uint32_t Self = EcsSelf;
static const uint32_t Up = EcsUp;
static const uint32_t Down = EcsDown;
static const uint32_t Cascade = EcsCascade;
static const uint32_t Parent = EcsParent;
static const uint32_t IsVariable = EcsIsVariable;
static const uint32_t IsEntity = EcsIsEntity;
static const uint32_t Filter = EcsFilter;
static const uint32_t TraverseFlags = EcsTraverseFlags;

/* Builtin entity ids */
static const flecs::entity_t Flecs = EcsFlecs;
static const flecs::entity_t FlecsCore = EcsFlecsCore;
static const flecs::entity_t World = EcsWorld;

/* Relationship properties */
static const flecs::entity_t Wildcard = EcsWildcard;
static const flecs::entity_t Any = EcsAny;
static const flecs::entity_t This = EcsThis;
static const flecs::entity_t Transitive = EcsTransitive;
static const flecs::entity_t Reflexive = EcsReflexive;
static const flecs::entity_t Final = EcsFinal;
static const flecs::entity_t DontInherit = EcsDontInherit;
static const flecs::entity_t Tag = EcsTag;
static const flecs::entity_t Union = EcsUnion;
static const flecs::entity_t Exclusive = EcsExclusive;
static const flecs::entity_t Acyclic = EcsAcyclic;
static const flecs::entity_t Traversable = EcsTraversable;
static const flecs::entity_t Symmetric = EcsSymmetric;
static const flecs::entity_t With = EcsWith;
static const flecs::entity_t OneOf = EcsOneOf;

/* Builtin relationships */
static const flecs::entity_t IsA = EcsIsA;
static const flecs::entity_t ChildOf = EcsChildOf;
static const flecs::entity_t DependsOn = EcsDependsOn;
static const flecs::entity_t SlotOf = EcsSlotOf;

/* Builtin identifiers */
static const flecs::entity_t Name = EcsName;
static const flecs::entity_t Symbol = EcsSymbol;

/* Cleanup policies */
static const flecs::entity_t OnDelete = EcsOnDelete;
static const flecs::entity_t OnDeleteTarget = EcsOnDeleteTarget;
static const flecs::entity_t Remove = EcsRemove;
static const flecs::entity_t Delete = EcsDelete;
static const flecs::entity_t Panic = EcsPanic;

/* Misc */
static const flecs::entity_t Flatten = EcsFlatten;
static const flecs::entity_t DefaultChildComponent = EcsDefaultChildComponent;

/* Builtin predicates for comparing entity ids in queries. Only supported by rules */
static const flecs::entity_t PredEq = EcsPredEq;
static const flecs::entity_t PredMatch = EcsPredMatch;
static const flecs::entity_t PredLookup = EcsPredLookup;

/* Builtin marker entities for query scopes */
static const flecs::entity_t ScopeOpen = EcsScopeOpen;
static const flecs::entity_t ScopeClose = EcsScopeClose;

/** @} */

}


// C++ utilities
/**
 * @file addons/cpp/utils/utils.hpp
 * @brief Flecs STL (FTL?)
 * 
 * Flecs STL (FTL?)
 * Minimalistic utilities that allow for STL like functionality without having
 * to depend on the actual STL.
 */

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
/**
 * @file addons/cpp/utils/array.hpp
 * @brief Array class.
 * 
 * Array class. Simple std::array like utility that is mostly there to aid
 * template code where template expansion would lead to an array with size 0.
 */

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

/**
 * @file addons/cpp/utils/string.hpp
 * @brief String utility that doesn't implicitly allocate memory.
 */

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

/**
 * @file addons/cpp/utils/enum.hpp
 * @brief Compile time enum reflection utilities.
 * 
 * Discover at compile time valid enumeration constants for an enumeration type
 * and their names. This is used to automatically register enum constants.
 */

#include <string.h>

#define FLECS_ENUM_MAX(T) _::to_constant<T, 128>::value
#define FLECS_ENUM_MAX_COUNT (FLECS_ENUM_MAX(int) + 1)

#ifndef FLECS_CPP_ENUM_REFLECTION_SUPPORT
#if !defined(__clang__) && defined(__GNUC__)
#if __GNUC__ > 7 || (__GNUC__ == 7 && __GNUC_MINOR__ >= 5)
#define FLECS_CPP_ENUM_REFLECTION_SUPPORT 1
#else
#define FLECS_CPP_ENUM_REFLECTION_SUPPORT 0
#endif
#else
#define FLECS_CPP_ENUM_REFLECTION_SUPPORT 1
#endif
#endif

namespace flecs {

/** Int to enum */
namespace _ {
template <typename E, int Value>
struct to_constant {
#if defined(__clang__) && __clang_major__ >= 16
    // https://reviews.llvm.org/D130058, https://reviews.llvm.org/D131307
    static constexpr E value = __builtin_bit_cast(E, Value);
#else
    static constexpr E value = static_cast<E>(Value);
#endif
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
#ifdef ECS_TARGET_WINDOWS
#define ECS_SIZE_T_STR "constexpr size_t; size_t = long long unsigned int"
#else
#define ECS_SIZE_T_STR "constexpr size_t; size_t = long unsigned int"
#endif
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
#if defined(ECS_TARGET_CLANG)
#if ECS_CLANG_VERSION < 13
template <typename E, E C>
constexpr bool enum_constant_is_valid() {
    return !(
        (ECS_FUNC_NAME[ECS_FUNC_NAME_FRONT(bool, enum_constant_is_valid) +
            enum_type_len<E>() + 6 /* ', C = ' */] >= '0') &&
        (ECS_FUNC_NAME[ECS_FUNC_NAME_FRONT(bool, enum_constant_is_valid) +
            enum_type_len<E>() + 6 /* ', C = ' */] <= '9'));
}
#else
template <typename E, E C>
constexpr bool enum_constant_is_valid() {
    return (ECS_FUNC_NAME[ECS_FUNC_NAME_FRONT(bool, enum_constant_is_valid) +
        enum_type_len<E>() + 6 /* ', E C = ' */] != '(');
}
#endif
#elif defined(ECS_TARGET_GNU)
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
        result, ECS_FUNC_NAME, string::length(ECS_FUNC_NAME),
            ECS_FUNC_NAME_BACK);
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
#if !FLECS_CPP_ENUM_REFLECTION_SUPPORT
        ecs_abort(ECS_UNSUPPORTED, "enum reflection requires gcc 7.5 or higher")
#endif

        ecs_log_push();
        ecs_cpp_enum_init(world, id);
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

/**
 * @file addons/cpp/utils/stringstream.hpp
 * @brief Wrapper around ecs_strbuf_t that provides a simple stringstream like API.
 */

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

/**
 * @file addons/cpp/utils/function_traits.hpp
 * @brief Compile time utilities to inspect properties of functions.
 *
 * Code from: https://stackoverflow.com/questions/27024238/c-template-mechanism-to-get-the-number-of-function-arguments-which-would-work
 */

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
/**
 * @file addons/cpp/mixins/id/decl.hpp
 * @brief Id class.
 */

#pragma once

namespace flecs {

struct id;
struct entity;

/**
 * @defgroup cpp_ids Ids
 * @brief Class for working with entity, component, tag and pair ids.
 * 
 * \ingroup cpp_core
 * @{
 */

/** Class that wraps around a flecs::id_t.
 * A flecs id is an identifier that can be added to entities. Ids can be:
 * - entities (including components, tags)
 * - pair ids
 * - entities with id flags set (like flecs::Override, flecs::Toggle)
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

    explicit id(flecs::world_t *world, flecs::id_t first, flecs::id_t second)
        : m_world(world)
        , m_id(ecs_pair(first, second)) { }

    explicit id(flecs::id_t first, flecs::id_t second)
        : m_world(nullptr)
        , m_id(ecs_pair(first, second)) { }

    explicit id(const flecs::id& first, const flecs::id& second)
        : m_world(first.m_world)
        , m_id(ecs_pair(first.m_id, second.m_id)) { }

    /** Test if id is pair (has first, second) */
    bool is_pair() const {
        return (m_id & ECS_ID_FLAGS_MASK) == flecs::Pair;
    }

    /** Test if id is a wildcard */
    bool is_wildcard() const {
        return ecs_id_is_wildcard(m_id);
    }

    /** Test if id is entity */
    bool is_entity() const {
        return !(m_id & ECS_ID_FLAGS_MASK);
    }

    /** Return id as entity (only allowed when id is valid entity) */
    flecs::entity entity() const;

    /** Return id with role added */
    flecs::entity add_flags(flecs::id_t flags) const;

    /** Return id with role removed */
    flecs::entity remove_flags(flecs::id_t flags) const;

    /** Return id without role */
    flecs::entity remove_flags() const;

    /** Return id without role */
    flecs::entity remove_generation() const;    

    /** Return component type of id */
    flecs::entity type_id() const;

    /** Test if id has specified role */
    bool has_flags(flecs::id_t flags) const {
        return ((m_id & flags) == flags);
    }

    /** Test if id has any role */
    bool has_flags() const {
        return (m_id & ECS_ID_FLAGS_MASK) != 0;
    }

    /** Return id flags set on id */
    flecs::entity flags() const;

    /** Test if id has specified first */
    bool has_relation(flecs::id_t first) const {
        if (!is_pair()) {
            return false;
        }
        return ECS_PAIR_FIRST(m_id) == first;
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

    /* Convert id to string */
    flecs::string str() const {
        return flecs::string(ecs_id_str(m_world, m_id));
    }

    /** Convert role of id to string. */
    flecs::string flags_str() const {
        return flecs::string_view( ecs_id_flag_str(m_id & ECS_ID_FLAGS_MASK));
    }

    /** Return flecs::id_t value */
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

/** @} */

}

/**
 * @file addons/cpp/mixins/term/decl.hpp
 * @brief Term declarations.
 */

#pragma once

namespace flecs {

/**
 * \ingroup cpp_core_filters
 */

struct term;
struct term_builder;

/** @} */

}

/**
 * @file addons/cpp/mixins/filter/decl.hpp
 * @brief Filter declarations.
 */

#pragma once

namespace flecs {

/**
 * @defgroup cpp_core_filters Filters
 * @brief Filters are are cheaper to create, but slower to iterate than flecs::query.
 * 
 * \ingroup cpp_core
 * @{
 */

struct filter_base;

template<typename ... Components>
struct filter;

template<typename ... Components>
struct filter_builder;

/** @} */

}

/**
 * @file addons/cpp/mixins/event/decl.hpp
 * @brief Event declarations.
 */

#pragma once

/**
 * @file addons/cpp/mixins/event/builder.hpp
 * @brief Event builder.
 */

#pragma once

#define ECS_EVENT_DESC_ID_COUNT_MAX (8)

namespace flecs {

/**
 * \ingroup cpp_addons_event
 * @{
 */

/** Event builder interface */
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

    /** Add component to emit for */
    template <typename T>
    Base& id() {
        m_ids.array = m_ids_array;
        m_ids.array[m_ids.count] = _::cpp_type<T>().id(m_world);
        m_ids.count ++;
        return *this;
    }
    
    /** 
     * Add pair to emit for
     * @tparam First The first element of the pair.
     * @tparam Second the second element of a pair.
     */
    template <typename First, typename Second>
    Base& id() {
        return id(
            ecs_pair(_::cpp_type<First>::id(this->m_world), 
                _::cpp_type<Second>::id(this->m_world)));
    }

    /** 
     * Add pair to emit for
     * @tparam First The first element of the pair.
     * @param second The second element of the pair id.
     */
    template <typename First>
    Base& id(entity_t second) {
        return id(ecs_pair(_::cpp_type<First>::id(this->m_world), second));
    }

    /** 
     * Add pair to emit for
     * @param first The first element of the pair type.
     * @param second The second element of the pair id.
     */
    Base& id(entity_t first, entity_t second) {
        return id(ecs_pair(first, second));
    }

    /** Add (component) id to emit for */
    Base& id(flecs::id_t id) {
        m_ids.array = m_ids_array;
        m_ids.array[m_ids.count] = id;
        m_ids.count ++;
        return *this;
    }

    /** Set entity for which to emit event */
    Base& entity(flecs::entity_t e) {
        ecs_record_t *r = ecs_record_find(m_world, e);
        
        /* Can't emit for empty entity */
        ecs_assert(r != nullptr, ECS_INVALID_PARAMETER, nullptr);
        ecs_assert(r->table != nullptr, ECS_INVALID_PARAMETER, nullptr);

        m_desc.table = r->table;
        m_desc.offset = ECS_RECORD_TO_ROW(r->row);
        m_desc.count = 1;
        return *this;
    }

    /* Set table for which to emit event */
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

/** @} */

}


/**
 * @file addons/cpp/mixins/query/decl.hpp
 * @brief Query declarations.
 */

#pragma once

namespace flecs {

/**
 * @defgroup cpp_core_queries Queries
 * @brief Cached query implementation. Fast to iterate, but slower to create than flecs::filter.
 * 
 * \ingroup cpp_core
 * @{
 */

struct query_base;

template<typename ... Components>
struct query;

template<typename ... Components>
struct query_builder;

/** @} */

}

/**
 * @file addons/cpp/mixins/observer/decl.hpp
 * @brief Observer declarations.
 */

#pragma once

namespace flecs {

/**
 * @defgroup cpp_observers Observers
 * @brief Observers let applications register callbacks for ECS events.
 * 
 * \ingroup cpp_core
 * @{
 */

struct observer;

template<typename ... Components>
struct observer_builder;

/** @} */

}

#ifdef FLECS_SYSTEM
/**
 * @file addons/cpp/mixins/system/decl.hpp
 * @brief System module declarations.
 */

#pragma once

namespace flecs {

/**
 * @defgroup cpp_addons_systems Systems
 * @brief Systems are a query + function that can be ran manually or by a pipeline.
 * 
 * \ingroup cpp_addons
 * @{
 */

using TickSource = EcsTickSource;

struct system;

template<typename ... Components>
struct system_builder;

namespace _ {

void system_init(flecs::world& world);

/** @} */

} // namespace _
} // namespace flecs

#endif
#ifdef FLECS_PIPELINE
/**
 * @file addons/cpp/mixins/pipeline/decl.hpp
 * @brief Pipeline module declarations.
 */

#pragma once

namespace flecs {

/**
 * @defgroup cpp_pipelines Pipelines
 * @brief Pipelines order and schedule systems for execution.
 * 
 * \ingroup cpp_addons
 * @{
 */

template <typename ... Components>
struct pipeline;

template <typename ... Components>
struct pipeline_builder;

/* Builtin pipeline tags */
static const flecs::entity_t OnStart = EcsOnStart;
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

/** @} */

}

#endif
#ifdef FLECS_TIMER
/**
 * @file addons/cpp/mixins/timer/decl.hpp
 * @brief Timer module declarations.
 */

#pragma once

namespace flecs {

/**
 * @defgroup cpp_addons_timer Timer
 * @brief Run systems at a time interval.
 * 
 * \ingroup cpp_addons
 * @{
 */

using Timer = EcsTimer;
using RateFilter = EcsRateFilter;

struct timer;

/** @} */

namespace _ {

void timer_init(flecs::world& world);

} // namespace _
} // namespace flecs

#endif
#ifdef FLECS_SNAPSHOT
/**
 * @file addons/cpp/mixins/snapshot/decl.hpp
 * @brief Snapshot module declarations.
 */

#pragma once

namespace flecs {

/**
 * @defgroup cpp_addons_snapshots Snapshots
 * @brief Save & restore world.
 * 
 * \ingroup cpp_addons
 * @{
 */

using snapshot_t = ecs_snapshot_t;

struct snapshot;

/** @} */

}

#endif
#ifdef FLECS_DOC
/**
 * @file addons/cpp/mixins/doc/decl.hpp
 * @brief Doc mixin declarations.
 */

#pragma once

namespace flecs {
namespace doc {

/**
 * @defgroup cpp_addons_doc Doc
 * @brief Utilities for documenting entities, components and systems.
 * 
 * \ingroup cpp_addons
 * @{
 */

/** flecs.doc.Description component */
using Description = EcsDocDescription;

/** flecs.doc.Brief component */
static const flecs::entity_t Brief = EcsDocBrief;

/** flecs.doc.Detail component */
static const flecs::entity_t Detail = EcsDocDetail;

/** flecs.doc.Link component */
static const flecs::entity_t Link = EcsDocLink;

/** flecs.doc.Color component */
static const flecs::entity_t Color = EcsDocColor;

namespace _ {
void init(flecs::world& world);
}

/** @} */

}
}

#endif
#ifdef FLECS_REST
/**
 * @file addons/cpp/mixins/rest/decl.hpp
 * @brief Rest module declarations.
 */

#pragma once

namespace flecs {

/**
 * @defgroup cpp_addons_rest Rest
 * @brief REST API for querying and mutating entities.
 * 
 * \ingroup cpp_addons
 * @{
 */

using Rest = EcsRest;

namespace rest {

namespace _ {

void init(flecs::world& world);

}
}

/** @} */

}

#endif
#ifdef FLECS_RULES
/**
 * @file addons/cpp/mixins/rule/decl.hpp
 * @brief Rule declarations.
 */

#pragma once

namespace flecs {

/**
 * @defgroup cpp_addons_rules Rules
 * @brief Rules are an advanced query engine for matching against entity graphs.
 * 
 * \ingroup cpp_addons
 * @{
 */

struct rule_base;

template<typename ... Components>
struct rule;

template<typename ... Components>
struct rule_builder;

/** @} */

}

#endif
#ifdef FLECS_META
/**
 * @file addons/cpp/mixins/meta/decl.hpp
 * @brief Meta declarations.
 */

#pragma once

namespace flecs {

/**
 * @defgroup cpp_addons_meta Meta
 * @brief Flecs reflection framework.
 * 
 * \ingroup cpp_addons
 * @{
 */

/* Primitive type aliases */
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

/* Embedded type aliases */
using member_t = ecs_member_t;
using enum_constant_t = ecs_enum_constant_t;
using bitmask_constant_t = ecs_bitmask_constant_t;

/* Components */
using MetaType = EcsMetaType;
using MetaTypeSerialized = EcsMetaTypeSerialized;
using Primitive = EcsPrimitive;
using Enum = EcsEnum;
using Bitmask = EcsBitmask;
using Member = EcsMember;
using MemberRanges = EcsMemberRanges;
using Struct = EcsStruct;
using Array = EcsArray;
using Vector = EcsVector;
using Unit = EcsUnit;

/** Base type for bitmasks */
struct bitmask {
    uint32_t value;
};

/* Handles to builtin reflection types */
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

/* Type kinds supported by reflection system */
using type_kind_t = ecs_type_kind_t;
static const type_kind_t PrimitiveType = EcsPrimitiveType;
static const type_kind_t BitmaskType = EcsBitmaskType;
static const type_kind_t EnumType = EcsEnumType;
static const type_kind_t StructType = EcsStructType;
static const type_kind_t ArrayType = EcsArrayType;
static const type_kind_t VectorType = EcsVectorType;
static const type_kind_t CustomType = EcsOpaqueType;
static const type_kind_t TypeKindLast = EcsTypeKindLast;

/* Primitive type kinds supported by reflection system */
using primitive_kind_t = ecs_primitive_kind_t;
static const primitive_kind_t Bool = EcsBool;
static const primitive_kind_t Char = EcsChar;
static const primitive_kind_t Byte = EcsByte;
static const primitive_kind_t U8 = EcsU8;
static const primitive_kind_t U16 = EcsU16;
static const primitive_kind_t U32 = EcsU32;
static const primitive_kind_t U64 = EcsU64;
static const primitive_kind_t I8 = EcsI8;
static const primitive_kind_t I16 = EcsI16;
static const primitive_kind_t I32 = EcsI32;
static const primitive_kind_t I64 = EcsI64;
static const primitive_kind_t F32 = EcsF32;
static const primitive_kind_t F64 = EcsF64;
static const primitive_kind_t UPtr = EcsUPtr;
static const primitive_kind_t IPtr = EcsIPtr;
static const primitive_kind_t String = EcsString;
static const primitive_kind_t Entity = EcsEntity;
static const primitive_kind_t PrimitiveKindLast = EcsPrimitiveKindLast;

/** @} */

namespace _ {

void init(flecs::world& world);

} // namespace _
} // namespace meta
} // namespace flecs

/**
 * @file addons/cpp/mixins/meta/opaque.hpp
 * @brief Helpers for opaque type registration.
 */

#pragma once

namespace flecs {

/**
 * @defgroup cpp_addons_meta Meta
 * @brief Flecs reflection framework.
 * 
 * \ingroup cpp_addons
 * @{
 */

/** Class for reading/writing dynamic values.
 * 
 * \ingroup cpp_addons_meta
 */
struct cursor {
    cursor(flecs::world_t *world, flecs::entity_t type_id, void *ptr) {
        m_cursor = ecs_meta_cursor(world, type_id, ptr);
    }

    /** Push value scope (such as a nested struct) */
    int push() {
        return ecs_meta_push(&m_cursor);
    }

    /** Pop value scope */
    int pop() {
        return ecs_meta_pop(&m_cursor);
    }

    /** Move to next member/element */
    int next() {
        return ecs_meta_next(&m_cursor);
    }

    /** Move to member by name */
    int member(const char *name) {
        return ecs_meta_member(&m_cursor, name);
    }

    /** Move to element by index */
    int elem(int32_t elem) {
        return ecs_meta_elem(&m_cursor, elem);
    }

    /** Test if current scope is a collection type */
    bool is_collection() {
        return ecs_meta_is_collection(&m_cursor);
    }

    /** Get member name */
    flecs::string_view get_member() const {
        return flecs::string_view(ecs_meta_get_member(&m_cursor));
    }

    /** Get type of value */
    flecs::entity get_type() const;

    /** Get unit of value */
    flecs::entity get_unit() const;

    /** Get untyped pointer to value */
    void* get_ptr() {
        return ecs_meta_get_ptr(&m_cursor);
    }

    /** Set boolean value */
    int set_bool(bool value) {
        return ecs_meta_set_bool(&m_cursor, value);
    }

    /** Set char value */
    int set_char(char value) {
        return ecs_meta_set_char(&m_cursor, value);
    }

    /** Set signed int value */
    int set_int(int64_t value) {
        return ecs_meta_set_int(&m_cursor, value);
    }

    /** Set unsigned int value */
    int set_uint(uint64_t value) {
        return ecs_meta_set_uint(&m_cursor, value);
    }

    /** Set float value */
    int set_float(double value) {
        return ecs_meta_set_float(&m_cursor, value);
    }

    /** Set string value */
    int set_string(const char *value) {
        return ecs_meta_set_string(&m_cursor, value);
    }

    /** Set string literal value */
    int set_string_literal(const char *value) {
        return ecs_meta_set_string_literal(&m_cursor, value);
    }

    /** Set entity value */
    int set_entity(flecs::entity_t value) {
        return ecs_meta_set_entity(&m_cursor, value);
    }

    /** Set null value */
    int set_null() {
        return ecs_meta_set_null(&m_cursor);
    }

    /** Get boolean value */
    bool get_bool() const {
        return ecs_meta_get_bool(&m_cursor);
    }

    /** Get char value */
    char get_char() const {
        return ecs_meta_get_char(&m_cursor);
    }

    /** Get signed int value */
    int64_t get_int() const {
        return ecs_meta_get_int(&m_cursor);
    }

    /** Get unsigned int value */
    uint64_t get_uint() const {
        return ecs_meta_get_uint(&m_cursor);
    }

    /** Get float value */
    double get_float() const {
        return ecs_meta_get_float(&m_cursor);
    }

    /** Get string value */
    const char *get_string() const {
        return ecs_meta_get_string(&m_cursor);
    }

    /** Get entity value */
    flecs::entity get_entity() const;

    /** Cursor object */
    ecs_meta_cursor_t m_cursor;
};

/** @} */

}

/**
 * @file addons/cpp/mixins/meta/opaque.hpp
 * @brief Helpers for opaque type registration.
 */

#pragma once

#include <stdio.h>

namespace flecs {

/**
 * @defgroup cpp_addons_meta Meta
 * @brief Flecs reflection framework.
 * 
 * \ingroup cpp_addons
 * @{
 */

/** Serializer object, used for serializing opaque types */
using serializer = ecs_serializer_t;

/** Serializer function, used to serialize opaque types */
using serialize_t = ecs_meta_serialize_t;

/** Type safe variant of serializer function */
template <typename T>
using serialize = int(*)(const serializer *, const T*);

/** Type safe interface for opaque types */
template <typename T, typename ElemType = void>
struct opaque {
    opaque(flecs::world_t *w = nullptr) : world(w) {
        if (world) {
            desc.entity = _::cpp_type<T>::id(world);
        }
    }

    /** Type that describes the type kind/structure of the opaque type */
    opaque& as_type(flecs::id_t func) {
        this->desc.type.as_type = func;
        return *this;
    }

    /** Serialize function */
    opaque& serialize(flecs::serialize<T> func) {
        this->desc.type.serialize = 
            reinterpret_cast<decltype(
                this->desc.type.serialize)>(func);
        return *this;
    }

    /** Assign bool value */
    opaque& assign_bool(void (*func)(T *dst, bool value)) {
        this->desc.type.assign_bool = 
            reinterpret_cast<decltype(
                this->desc.type.assign_bool)>(func);
        return *this;
    }

    /** Assign char value */
    opaque& assign_char(void (*func)(T *dst, char value)) {
        this->desc.type.assign_char = 
            reinterpret_cast<decltype(
                this->desc.type.assign_char)>(func);
        return *this;
    }

    /** Assign int value */
    opaque& assign_int(void (*func)(T *dst, int64_t value)) {
        this->desc.type.assign_int = 
            reinterpret_cast<decltype(
                this->desc.type.assign_int)>(func);
        return *this;
    }

    /** Assign unsigned int value */
    opaque& assign_uint(void (*func)(T *dst, uint64_t value)) {
        this->desc.type.assign_uint = 
            reinterpret_cast<decltype(
                this->desc.type.assign_uint)>(func);
        return *this;
    }

    /** Assign float value */
    opaque& assign_float(void (*func)(T *dst, double value)) {
        this->desc.type.assign_float = 
            reinterpret_cast<decltype(
                this->desc.type.assign_float)>(func);
        return *this;
    }

    /** Assign string value */
    opaque& assign_string(void (*func)(T *dst, const char *value)) {
        this->desc.type.assign_string = 
            reinterpret_cast<decltype(
                this->desc.type.assign_string)>(func);
        return *this;
    }

    /** Assign entity value */
    opaque& assign_entity(
        void (*func)(T *dst, ecs_world_t *world, ecs_entity_t entity)) 
    {
        this->desc.type.assign_entity = 
            reinterpret_cast<decltype(
                this->desc.type.assign_entity)>(func);
        return *this;
    }

    /** Assign null value */
    opaque& assign_null(void (*func)(T *dst)) {
        this->desc.type.assign_null = 
            reinterpret_cast<decltype(
                this->desc.type.assign_null)>(func);
        return *this;
    }

    /** Clear collection elements */
    opaque& clear(void (*func)(T *dst)) {
        this->desc.type.clear = 
            reinterpret_cast<decltype(
                this->desc.type.clear)>(func);
        return *this;
    }

    /** Ensure & get collection element */
    opaque& ensure_element(ElemType* (*func)(T *dst, size_t elem)) {
        this->desc.type.ensure_element = 
            reinterpret_cast<decltype(
                this->desc.type.ensure_element)>(func);
        return *this;
    }

    /** Ensure & get element */
    opaque& ensure_member(void* (*func)(T *dst, const char *member)) {
        this->desc.type.ensure_member = 
            reinterpret_cast<decltype(
                this->desc.type.ensure_member)>(func);
        return *this;
    }

    /** Return number of elements */
    opaque& count(size_t (*func)(const T *dst)) {
        this->desc.type.count = 
            reinterpret_cast<decltype(
                this->desc.type.count)>(func);
        return *this;
    }
    
    /** Resize to number of elements */
    opaque& resize(void (*func)(T *dst, size_t count)) {
        this->desc.type.resize = 
            reinterpret_cast<decltype(
                this->desc.type.resize)>(func);
        return *this;
    }

    ~opaque() {
        if (world) {
            ecs_opaque_init(world, &desc);
        }
    }

    /** Opaque type descriptor */
    flecs::world_t *world = nullptr;
    ecs_opaque_desc_t desc = {};
};

/** @} */

}


#endif
#ifdef FLECS_UNITS
/**
 * @file addons/cpp/mixins/units/decl.hpp
 * @brief Units module declarations.
 */

#pragma once

namespace flecs {
struct units {

/**
 * @defgroup cpp_addons_units Units
 * @brief Common unit annotations for reflection framework.
 * 
 * \ingroup cpp_addons
 * @{
 */

struct Prefixes { };

/**
 * @defgroup cpp_addons_units_prefixes Prefixes
 * @brief Prefixes to indicate unit count (e.g. Kilo, Mega)
 * 
 * \ingroup cpp_addons_units
 * @{
 */

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

/** @} */

/**
 * @defgroup cpp_addons_units_quantities Quantities
 * @brief Quantities that group units (e.g. Length)
 * 
 * \ingroup cpp_addons_units
 * @{
 */

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
struct Frequency { };
struct Uri { };

/** @} */

struct duration {
/**
 * @defgroup cpp_addons_units_duration Duration
 * 
 * \ingroup cpp_addons_units
 * @{
 */

struct PicoSeconds { };
struct NanoSeconds { };
struct MicroSeconds { };
struct MilliSeconds { };
struct Seconds { };
struct Minutes { };
struct Hours { };
struct Days { };

/** @} */
};

struct angle {
/**
 * @defgroup cpp_addons_units_angle Angle
 * 
 * \ingroup cpp_addons_units
 * @{
 */

struct Radians { };
struct Degrees { };

/** @} */
};


struct time {
/**
 * @defgroup cpp_addons_units_time Time
 * 
 * \ingroup cpp_addons_units
 * @{
 */

struct Date { };

/** @} */
};


struct mass {
/**
 * @defgroup cpp_addons_units_mass Mass
 * 
 * \ingroup cpp_addons_units
 * @{
 */

struct Grams { };
struct KiloGrams { };

/** @} */
};


struct electric_current {
/**
 * @defgroup cpp_addons_units_electric_current Electric Current
 * 
 * \ingroup cpp_addons_units
 * @{
 */

struct Ampere { };

/** @} */
};


struct amount {
/**
 * @defgroup cpp_addons_units_amount Amount
 * 
 * \ingroup cpp_addons_units
 * @{
 */

struct Mole { };

/** @} */
};


struct luminous_intensity {
/**
 * @defgroup cpp_addons_units_luminous_intensity Luminous Intensity
 * 
 * \ingroup cpp_addons_units
 * @{
 */

struct Candela { };

/** @} */
};


struct force {
/**
 * @defgroup cpp_addons_units_force Force
 * 
 * \ingroup cpp_addons_units
 * @{
 */

struct Newton { };

/** @} */
};


struct length {
/**
 * @defgroup cpp_addons_units_length Length
 * 
 * \ingroup cpp_addons_units
 * @{
 */

struct Meters { };
struct PicoMeters { };
struct NanoMeters { };
struct MicroMeters { };
struct MilliMeters { };
struct CentiMeters { };
struct KiloMeters { };
struct Miles { };
struct Pixels { };

/** @} */
};


struct pressure {
/**
 * @defgroup cpp_addons_units_pressure Pressure
 * 
 * \ingroup cpp_addons_units
 * @{
 */

struct Pascal { };
struct Bar { };

/** @} */
};


struct speed {
/**
 * @defgroup cpp_addons_units_speed Speed
 * 
 * \ingroup cpp_addons_units
 * @{
 */

struct MetersPerSecond { };
struct KiloMetersPerSecond { };
struct KiloMetersPerHour { };
struct MilesPerHour { };

/** @} */
};


struct temperature {
/**
 * @defgroup cpp_addons_units_temperature Temperature
 * 
 * \ingroup cpp_addons_units
 * @{
 */

struct Kelvin { };
struct Celsius { };
struct Fahrenheit { };

/** @} */
};


struct data {
/**
 * @defgroup cpp_addons_units_data Data
 * 
 * \ingroup cpp_addons_units
 * @{
 */

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

/** @} */
};

struct datarate {
/**
 * @defgroup cpp_addons_units_datarate Data Rate
 * 
 * \ingroup cpp_addons_units
 * @{
 */

struct BitsPerSecond { };
struct KiloBitsPerSecond { };
struct MegaBitsPerSecond { };
struct GigaBitsPerSecond { };
struct BytesPerSecond { };
struct KiloBytesPerSecond { };
struct MegaBytesPerSecond { };
struct GigaBytesPerSecond { };

/** @} */
};


struct frequency {
/**
 * @defgroup cpp_addons_units_frequency Frequency
 * 
 * \ingroup cpp_addons_units
 * @{
 */

struct Hertz { };
struct KiloHertz { };
struct MegaHertz { };
struct GigaHertz { };

/** @} */
};


struct uri {
/**
 * @defgroup cpp_addons_units_uri Uri
 * 
 * \ingroup cpp_addons_units
 * @{
 */

struct Hyperlink { };
struct Image { };
struct File { };

/** @} */
};


struct Percentage { };
struct Bel { };
struct DeciBel { };

units(flecs::world& world);

/** @} */

};
}

#endif
#ifdef FLECS_MONITOR
/**
 * @file addons/cpp/mixins/monitor/decl.hpp
 * @brief Monitor module declarations.
 */

#pragma once

namespace flecs {

/**
 * @defgroup cpp_addons_monitor Monitor
 * @brief The monitor addon periodically tracks statistics for the world and systems.
 * 
 * \ingroup cpp_addons
 * @{
 */

/** Component that stores world statistics */
using WorldStats = EcsWorldStats;

/** Component that stores system/pipeline statistics */
using PipelineStats = EcsPipelineStats;
    
struct monitor {
    monitor(flecs::world& world);
};

/** @} */

}
#endif
#ifdef FLECS_METRICS
/**
 * @file addons/cpp/mixins/metrics/decl.hpp
 * @brief Metrics declarations.
 */

#pragma once

/**
 * @file addons/cpp/mixins/metrics/builder.hpp
 * @brief Metric builder.
 */

#pragma once

#define ECS_EVENT_DESC_ID_COUNT_MAX (8)

namespace flecs {

/**
 * \ingroup cpp_addon_metrics
 * @{
 */

/** Event builder interface */
struct metric_builder {
    metric_builder(flecs::world_t *world, flecs::entity_t entity) 
        : m_world(world) 
    {
        m_desc.entity = entity;
    }

    ~metric_builder();

    metric_builder& member(flecs::entity_t e) {
        m_desc.member = e;
        return *this;
    }

    metric_builder& member(const char *name);

    template <typename T>
    metric_builder& member(const char *name);

    metric_builder& id(flecs::id_t the_id) {
        m_desc.id = the_id;
        return *this;
    }

    metric_builder& id(flecs::entity_t first, flecs::entity_t second) {
        m_desc.id = ecs_pair(first, second);
        return *this;
    }

    template <typename T>
    metric_builder& id() {
        return id(_::cpp_type<T>::id(m_world));
    }

    template <typename First>
    metric_builder& id(flecs::entity_t second) {
        return id(_::cpp_type<First>::id(m_world), second);
    }

    template <typename First, typename Second>
    metric_builder& id() {
        return id<First>(_::cpp_type<Second>::id(m_world));
    }

    metric_builder& targets(bool value = true) {
        m_desc.targets = value;
        return *this;
    }

    metric_builder& kind(flecs::entity_t the_kind) {
        m_desc.kind = the_kind;
        return *this;
    }

    template <typename Kind>
    metric_builder& kind() {
        return kind(_::cpp_type<Kind>::id(m_world));
    }

    metric_builder& brief(const char *b) {
        m_desc.brief = b;
        return *this;
    }

    operator flecs::entity();

protected:
    flecs::world_t *m_world;
    ecs_metric_desc_t m_desc = {};
    bool m_created = false;
};

/**
 * @}
 */

}


namespace flecs {

struct metrics {
    using Value = EcsMetricValue;
    using Source = EcsMetricSource;

    struct Instance { };
    struct Metric { };
    struct Counter { };
    struct CounterIncrement { };
    struct CounterId { };
    struct Gauge { };

    metrics(flecs::world& world);
};

}

#endif
#ifdef FLECS_ALERTS
/**
 * @file addons/cpp/mixins/alerts/decl.hpp
 * @brief Alert declarations.
 */

#pragma once

namespace flecs {

/**
 * @defgroup cpp_addons_alerts Alerts
 * @brief Alert implementation.
 * 
 * \ingroup cpp_addons
 * @{
 */

/** Module */
struct alerts {
    using AlertsActive = EcsAlertsActive;
    using Instance = EcsAlertInstance;

    struct Alert { };
    struct Info { };
    struct Warning { };
    struct Error { };

    alerts(flecs::world& world);
};

template <typename ... Components>
struct alert;

template <typename ... Components>
struct alert_builder;

/** @} */

}

#endif
#ifdef FLECS_JSON
/**
 * @file addons/cpp/mixins/json/decl.hpp
 * @brief JSON addon declarations.
 */

#pragma once

namespace flecs {

/**
 * @defgroup cpp_addons_json Json
 * @brief Functions for serializing to/from JSON.
 * 
 * \ingroup cpp_addons
 * @{
 */

using from_json_desc_t = ecs_from_json_desc_t;
using entity_to_json_desc_t = ecs_entity_to_json_desc_t;
using iter_to_json_desc_t = ecs_iter_to_json_desc_t;

/** @} */

}

#endif
#ifdef FLECS_APP
/**
 * @file addons/cpp/mixins/app/decl.hpp
 * @brief App addon declarations.
 */

#pragma once

/**
 * @file addons/cpp/mixins/app/builder.hpp
 * @brief App builder.
 */

#pragma once

namespace flecs {

/**
 * @defgroup cpp_addons_app App
 * @brief Optional addon for running the main application loop.
 * \ingroup cpp_addons
 * @{
 */

/** App builder interface */
struct app_builder {
    app_builder(flecs::world_t *world)
        : m_world(world)
        , m_desc{}
    {
        const ecs_world_info_t *stats = ecs_get_world_info(world);
        m_desc.target_fps = stats->target_fps;
        ecs_ftime_t t_zero = 0.0;
        if (ECS_EQ(m_desc.target_fps, t_zero)) {
            m_desc.target_fps = 60;
        }
    }

    app_builder& target_fps(ecs_ftime_t value) {
        m_desc.target_fps = value;
        return *this;
    }

    app_builder& delta_time(ecs_ftime_t value) {
        m_desc.delta_time = value;
        return *this;
    }

    app_builder& threads(int32_t value) {
        m_desc.threads = value;
        return *this;
    }

    app_builder& frames(int32_t value) {
        m_desc.frames = value;
        return *this;
    }

    app_builder& enable_rest(uint16_t port = 0) {
        m_desc.enable_rest = true;
        m_desc.port = port;
        return *this;
    }

    app_builder& enable_monitor(bool value = true) {
        m_desc.enable_monitor = value;
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
        int result = ecs_app_run(m_world, &m_desc);
        if (ecs_should_quit(m_world)) {
            // Only free world if quit flag is set. This ensures that we won't
            // try to cleanup the world if the app is used in an environment 
            // that takes over the main loop, like with emscripten.
            ecs_fini(m_world);
        }
        return result;
    }

private:
    flecs::world_t *m_world;
    ecs_app_desc_t m_desc;
};

/** @} */

}


#endif

/**
 * @file addons/cpp/log.hpp
 * @brief Logging functions.
 */

#pragma once

namespace flecs {
namespace log {

/**
 * @defgroup cpp_log Logging
 * @brief Logging functions.
 * 
 * \ingroup cpp_addons
 * @{
 */

/** Set log level */
inline void set_level(int level) {
    ecs_log_set_level(level);
}

inline int get_level(void) {
    return ecs_log_get_level();
}

/** Enable colors in logging */
inline void enable_colors(bool enabled = true) {
    ecs_log_enable_colors(enabled);
}

/** Enable timestamps in logging */
inline void enable_timestamp(bool enabled = true) {
    ecs_log_enable_timestamp(enabled);
}

/** Enable time delta in logging */
inline void enable_timedelta(bool enabled = true) {
    ecs_log_enable_timedelta(enabled);
}

/** Debug trace (level 1) */
inline void dbg(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    ecs_logv(1, fmt, args);
    va_end(args);
}

/** Trace (level 0) */
inline void trace(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    ecs_logv(0, fmt, args);
    va_end(args);
}

/** Trace (level -2) */
inline void warn(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    ecs_logv(-2, fmt, args);
    va_end(args);
}

/** Trace (level -3) */
inline void err(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    ecs_logv(-3, fmt, args);
    va_end(args);
}

/** Increase log indentation */
inline void push(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    ecs_logv(0, fmt, args);
    va_end(args);
    ecs_log_push();
}

/** Increase log indentation */
inline void push() {
    ecs_log_push();
}

/** Increase log indentation */
inline void pop() {
    ecs_log_pop();
}

/** @} */

}
}

/**
 * @file addons/cpp/pair.hpp
 * @brief Utilities for working with compile time pairs.
 */

#pragma once

namespace flecs {

namespace _ {
    struct pair_base { };   
} // _


/**
 * @defgroup cpp_pair_type Pair type
 * @brief Compile time utilities for working with relationship pairs.
 * 
 * \ingroup cpp_core
 * @{
 */

/** Type that represents a pair.
 * The pair type can be used to represent a pair at compile time, and is able
 * to automatically derive the storage type associated with the pair, accessible
 * through pair::type.
 * 
 * The storage type is derived using the following rules:
 * - if pair::first is non-empty, the storage type is pair::first
 * - if pair::first is empty and pair::second is non-empty, the storage type is pair::second
 * 
 * The pair type can hold a temporary value so that it can be used in the
 * signatures of queries
 */
template <typename First, typename Second>
struct pair : _::pair_base { 
    using type = conditional_t<!is_empty<First>::value || is_empty<Second>::value, First, Second>;
    using first = First;
    using second = Second;

    pair(type& v) : ref_(v) { }

    // This allows the class to be used as a temporary object
    pair(const type& v) : ref_(const_cast<type&>(v)) { }

    operator type&() { 
        return ref_;
    }

    operator const type&() const { 
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

template <typename First, typename Second, if_t<is_empty<First>::value> = 0>
using pair_object = pair<First, Second>;


/** Test if type is a pair. */
template <typename T>
struct is_pair {
    static constexpr bool value = is_base_of<_::pair_base, remove_reference_t<T> >::value;
};


/** Get pair::first from pair while preserving cv qualifiers. */
template <typename P>
using pair_first_t = transcribe_cv_t<remove_reference_t<P>, typename remove_reference_t<P>::first>;

/** Get pair::second from pair while preserving cv qualifiers. */
template <typename P>
using pair_second_t = transcribe_cv_t<remove_reference_t<P>, typename remove_reference_t<P>::second>;

/** Get pair::type type from pair while preserving cv qualifiers. */
template <typename P>
using pair_type_t = transcribe_cv_t<remove_reference_t<P>, typename remove_reference_t<P>::type>;

/** Get actual type from a regular type or pair. */
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

/**
 * @file addons/cpp/lifecycle_traits.hpp
 * @brief Utilities for discovering and registering component lifecycle hooks.
 */

#pragma once

namespace flecs 
{

namespace _ 
{

inline void ecs_ctor_illegal(void *, int32_t, const ecs_type_info_t *ti) {
    ecs_abort(ECS_INVALID_OPERATION, "invalid constructor for %s", ti->name);
}

inline void ecs_dtor_illegal(void *, int32_t, const ecs_type_info_t *ti) {
    ecs_abort(ECS_INVALID_OPERATION, "invalid destructor for %s", ti->name);
}

inline void ecs_copy_illegal(
    void *, const void *, int32_t, const ecs_type_info_t *ti)
{
    ecs_abort(ECS_INVALID_OPERATION, "invalid copy assignment for %s", ti->name);
}

inline void ecs_move_illegal(void *, void *, int32_t, const ecs_type_info_t *ti) {
    ecs_abort(ECS_INVALID_OPERATION, "invalid move assignment for %s", ti->name);
}

inline void ecs_copy_ctor_illegal(
    void *, const void *, int32_t, const ecs_type_info_t *ti)
{
    ecs_abort(ECS_INVALID_OPERATION, "invalid copy construct for %s", ti->name);
}

inline void ecs_move_ctor_illegal(
    void *, void *, int32_t, const ecs_type_info_t *ti)
{
    ecs_abort(ECS_INVALID_OPERATION, "invalid move construct for %s", ti->name);
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

/**
 * @file addons/cpp/ref.hpp
 * @brief Class that caches data to speedup get operations.
 */

#pragma once

namespace flecs
{

/**
 * @defgroup cpp_ref Refs
 * @brief Refs are a fast mechanism for referring to a specific entity/component.
 * 
 * \ingroup cpp_core
 * @{
 */

/** Component reference.
 * Reference to a component from a specific entity.
 */
template <typename T>
struct ref {
    ref(world_t *world, entity_t entity, flecs::id_t id = 0)
        : m_ref()
    {
        // the world we were called with may be a stage; convert it to a world
        // here if that is the case
        m_world = world ? const_cast<flecs::world_t *>(ecs_get_world(world))
            : nullptr;
        if (!id) {
            id = _::cpp_type<T>::id(world);
        }

        ecs_assert(_::cpp_type<T>::size() != 0, ECS_INVALID_PARAMETER, NULL);

        m_ref = ecs_ref_init_id(m_world, entity, id);
    }

    T* operator->() {
        T* result = static_cast<T*>(ecs_ref_get_id(
            m_world, &m_ref, this->m_ref.id));

        ecs_assert(result != NULL, ECS_INVALID_PARAMETER, NULL);

        return result;
    }

    T* get() {
        return static_cast<T*>(ecs_ref_get_id(
            m_world, &m_ref, this->m_ref.id));
    }

    flecs::entity entity() const;

private:
    world_t *m_world;
    flecs::ref_t m_ref;
};

/** @} */

}

/**
 * @file addons/cpp/world.hpp
 * @brief World class.
 */

#pragma once

namespace flecs
{

/* Static helper functions to assign a component value */

// set(T&&), T = constructible
template <typename T, if_t< is_flecs_constructible<T>::value > = 0>
inline void set(world_t *world, flecs::entity_t entity, T&& value, flecs::id_t id) {
    ecs_assert(_::cpp_type<T>::size() != 0, ECS_INVALID_PARAMETER, NULL);

    T& dst = *static_cast<T*>(ecs_get_mut_id(world, entity, id));
    dst = FLECS_MOV(value);

    ecs_modified_id(world, entity, id);
}

// set(const T&), T = constructible
template <typename T, if_t< is_flecs_constructible<T>::value > = 0>
inline void set(world_t *world, flecs::entity_t entity, const T& value, flecs::id_t id) {
    ecs_assert(_::cpp_type<T>::size() != 0, ECS_INVALID_PARAMETER, NULL);

    T& dst = *static_cast<T*>(ecs_get_mut_id(world, entity, id));
    dst = value;

    ecs_modified_id(world, entity, id);
}

// set(T&&), T = not constructible
template <typename T, if_not_t< is_flecs_constructible<T>::value > = 0>
inline void set(world_t *world, flecs::entity_t entity, T&& value, flecs::id_t id) {
    ecs_assert(_::cpp_type<T>::size() != 0, ECS_INVALID_PARAMETER, NULL);

    T& dst = *static_cast<remove_reference_t<T>*>(ecs_get_mut_id(world, entity, id));

    dst = FLECS_MOV(value);

    ecs_modified_id(world, entity, id);
}

// set(const T&), T = not constructible
template <typename T, if_not_t< is_flecs_constructible<T>::value > = 0>
inline void set(world_t *world, flecs::entity_t entity, const T& value, flecs::id_t id) {
    ecs_assert(_::cpp_type<T>::size() != 0, ECS_INVALID_PARAMETER, NULL);

    T& dst = *static_cast<T*>(ecs_get_mut_id(world, entity, id));
    dst = value;

    ecs_modified_id(world, entity, id);
}

// emplace for T(Args...)
template <typename T, typename ... Args, if_t< 
    std::is_constructible<actual_type_t<T>, Args...>::value ||
    std::is_default_constructible<actual_type_t<T>>::value > = 0>
inline void emplace(world_t *world, flecs::entity_t entity, flecs::id_t id, Args&&... args) {
    ecs_assert(_::cpp_type<T>::size() != 0, ECS_INVALID_PARAMETER, NULL);
    T& dst = *static_cast<T*>(ecs_emplace_id(world, entity, id));
    
    FLECS_PLACEMENT_NEW(&dst, T{FLECS_FWD(args)...});

    ecs_modified_id(world, entity, id);    
}

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

struct scoped_world;

/**
 * @defgroup cpp_world World
 * @brief World operations.
 * 
 * \ingroup cpp_core
 * @{
 */

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

    /** Deletes and recreates the world. */
    void reset() {
        // Can only reset the world if we own the world object.
        ecs_assert(this->m_owned, ECS_INVALID_OPERATION, NULL);
        ecs_fini(m_world);
        m_world = ecs_init();
    }

    /** Obtain pointer to C world object.
     */
    world_t* c_ptr() const {
        return m_world;
    }

    /** Get last delta_time.
     */
    ecs_ftime_t delta_time() const {
        const ecs_world_info_t *stats = ecs_get_world_info(m_world);
        return stats->delta_time;
    }

    /** Get current tick.
     */
    int64_t tick() const {
        const ecs_world_info_t *stats = ecs_get_world_info(m_world);
        return stats->frame_count_total;
    }

    /** Get current simulation time.
     */
    ecs_ftime_t time() const {
        const ecs_world_info_t *stats = ecs_get_world_info(m_world);
        return stats->world_time_total;
    }

    /** Signal application should quit.
     * After calling this operation, the next call to progress() returns false.
     */
    void quit() const {
        ecs_quit(m_world);
    }

    /** Register action to be executed when world is destroyed.
     */
    void atfini(ecs_fini_action_t action, void *ctx) const {
        ecs_atfini(m_world, action, ctx);
    }

    /** Test if quit() has been called.
     */
    bool should_quit() const {
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
    ecs_ftime_t frame_begin(float delta_time = 0) const {
        return ecs_frame_begin(m_world, delta_time);
    }

    /** End frame. 
     * This operation must be called at the end of the frame, and always after
     * ecs_frame_begin.
     *
     * This function should only be ran from the main thread.
     */
    void frame_end() const {
        ecs_frame_end(m_world);
    }

    /** Begin staging.
     * When an application does not use ecs_progress to control the main loop, it
     * can still use Flecs features such as the defer queue. When an application
     * needs to stage changes, it needs to call this function after ecs_frame_begin.
     * A call to ecs_readonly_begin must be followed by a call to ecs_readonly_end.
     * 
     * When staging is enabled, modifications to entities are stored to a stage.
     * This ensures that arrays are not modified while iterating. Modifications are
     * merged back to the "main stage" when ecs_readonly_end is invoked.
     *
     * While the world is in staging mode, no structural changes (add/remove/...)
     * can be made to the world itself. Operations must be executed on a stage
     * instead (see ecs_get_stage).
     *
     * This function should only be ran from the main thread.
     *
     * @return Whether world is currently staged.
     */
    bool readonly_begin() const {
        return ecs_readonly_begin(m_world);
    }

    /** End staging.
     * Leaves staging mode. After this operation the world may be directly mutated
     * again. By default this operation also merges data back into the world, unless
     * automerging was disabled explicitly.
     *
     * This function should only be ran from the main thread.
     */
    void readonly_end() const {
        ecs_readonly_end(m_world);
    }

    /** Defer operations until end of frame. 
     * When this operation is invoked while iterating, operations inbetween the
     * defer_begin and defer_end operations are executed at the end of the frame.
     *
     * This operation is thread safe.
     */
    bool defer_begin() const {
        return ecs_defer_begin(m_world);
    }

    /** End block of operations to defer. 
     * See defer_begin.
     *
     * This operation is thread safe.
     */
    bool defer_end() const {
        return ecs_defer_end(m_world);
    }

    /** Test whether deferring is enabled.
     */
    bool is_deferred() const {
        return ecs_is_deferred(m_world);
    }

    /** Configure world to have N stages.
     * This initializes N stages, which allows applications to defer operations to
     * multiple isolated defer queues. This is typically used for applications with
     * multiple threads, where each thread gets its own queue, and commands are
     * merged when threads are synchronized.
     *
     * Note that set_threads() already creates the appropriate number of stages. 
     * The set_stage_count() operation is useful for applications that want to manage 
     * their own stages and/or threads.
     * 
     * @param stages The number of stages.
     */
    void set_stage_count(int32_t stages) const {
        ecs_set_stage_count(m_world, stages);
    }

    /** Get number of configured stages.
     * Return number of stages set by set_stage_count.
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

    /** Test if is a stage.
     * If this function returns false, it is guaranteed that this is a valid
     * world object.
     * 
     * @return True if the world is a stage, false if not.
     */
    bool is_stage() const {
        ecs_assert(
            ecs_poly_is(m_world, ecs_world_t) || 
            ecs_poly_is(m_world, ecs_stage_t),
                ECS_INVALID_PARAMETER, NULL);
        return ecs_poly_is(m_world, ecs_stage_t);
    }

    /** Enable/disable automerging for world or stage.
     * When automerging is enabled, staged data will automatically be merged 
     * with the world when staging ends. This happens at the end of progress(), 
     * at a sync point or when readonly_end() is called.
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
    void set_automerge(bool automerge) const {
        ecs_set_automerge(m_world, automerge);
    }

    /** Merge world or stage.
     * When automatic merging is disabled, an application can call this
     * operation on either an individual stage, or on the world which will merge
     * all stages. This operation may only be called when staging is not enabled
     * (either after progress() or after readonly_end()).
     *
     * This operation may be called on an already merged stage or world.
     */
    void merge() const {
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
    flecs::entity_t* set_lookup_path(const flecs::entity_t *search_path) const {
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

    /** Set singleton component.
     */
    template <typename T, if_t< !is_callable<T>::value > = 0>
    void set(T&& value) const {
        flecs::set<T>(m_world, _::cpp_type<T>::id(m_world), 
            FLECS_FWD(value));
    }

    /** Set singleton pair.
     */
    template <typename First, typename Second, typename P = flecs::pair<First, Second>, 
        typename A = actual_type_t<P>, if_not_t< flecs::is_pair<First>::value> = 0>
    void set(const A& value) const {
        flecs::set<P>(m_world, _::cpp_type<First>::id(m_world), value);
    }

    /** Set singleton pair.
     */
    template <typename First, typename Second, typename P = flecs::pair<First, Second>, 
        typename A = actual_type_t<P>, if_not_t< flecs::is_pair<First>::value> = 0>
    void set(A&& value) const {
        flecs::set<P>(m_world, _::cpp_type<First>::id(m_world), FLECS_FWD(value));
    }

    /** Set singleton pair.
     */
    template <typename First, typename Second>
    void set(Second second, const First& value) const;

    /** Set singleton pair.
     */
    template <typename First, typename Second>
    void set(Second second, First&& value) const;

    /** Set singleton component inside a callback.
     */
    template <typename Func, if_t< is_callable<Func>::value > = 0 >
    void set(const Func& func) const;

    template <typename T, typename ... Args>
    void emplace(Args&&... args) const {
        flecs::id_t component_id = _::cpp_type<T>::id(m_world);
        flecs::emplace<T>(m_world, component_id, component_id,
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

    /** Get ref singleton component.
     */
    template <typename T>
    ref<T> get_ref() const;

    /** Get singleton component.
     */
    template <typename T>
    const T* get() const;

    /** Get singleton pair.
     */
    template <typename First, typename Second, typename P = flecs::pair<First, Second>, 
        typename A = actual_type_t<P>>
    const A* get() const;

    /** Get singleton pair.
     */
    template <typename First, typename Second>
    const First* get(Second second) const;
    
    /** Get singleton component inside a callback.
     */
    template <typename Func, if_t< is_callable<Func>::value > = 0 >
    void get(const Func& func) const;

    /** Test if world has singleton component.
     */
    template <typename T>
    bool has() const;

    /** Test if world has the provided pair.
     * 
     * @tparam First The first element of the pair
     * @tparam Second The second element of the pair
     */
    template <typename First, typename Second>
    bool has() const;

    /** Test if world has the provided pair.
     * 
     * @tparam First The first element of the pair
     * @param second The second element of the pair.
     */
    template <typename First>
    bool has(flecs::id_t second) const;

    /** Test if world has the provided pair.
     * 
     * @param first The first element of the pair
     * @param second The second element of the pair
     */
    bool has(flecs::id_t first, flecs::id_t second) const;

    /** Add singleton component.
     */
    template <typename T>
    void add() const;

    /** Adds a pair to the singleton component.
     * 
     * @tparam First The first element of the pair
     * @tparam Second The second element of the pair
     */
    template <typename First, typename Second>
    void add() const;

    /** Adds a pair to the singleton component.
     * 
     * @tparam First The first element of the pair
     * @param second The second element of the pair.
     */
    template <typename First>
    void add(flecs::entity_t second) const;

    /** Adds a pair to the singleton entity.
     * 
     * @param first The first element of the pair
     * @param second The second element of the pair
     */
    void add(flecs::entity_t first, flecs::entity_t second) const;

    /** Remove singleton component.
     */
    template <typename T>
    void remove() const;

    /** Adds a pair to the singleton component.
     * 
     * @tparam First The first element of the pair
     * @tparam Second The second element of the pair
     */
    template <typename First, typename Second>
    void remove() const;

    /** Adds a pair to the singleton component.
     * 
     * @tparam First The first element of the pair
     * @param second The second element of the pair.
     */
    template <typename First>
    void remove(flecs::entity_t second) const;

    /** Adds a pair to the singleton entity.
     * 
     * @param first The first element of the pair
     * @param second The second element of the pair
     */
    void remove(flecs::entity_t first, flecs::entity_t second) const;

    /** Iterate entities in root of world 
     * Accepts a callback with the following signature:
     *  void(*)(flecs::entity e);
     */
    template <typename Func>
    void children(Func&& f) const;

    /** Get singleton entity for type.
     */
    template <typename T>
    flecs::entity singleton() const;

    /** Get target for a given pair from a singleton entity.
     * This operation returns the target for a given pair. The optional
     * index can be used to iterate through targets, in case the entity has
     * multiple instances for the same relationship.
     *
     * @tparam First The first element of the pair.
     * @param index The index (0 for the first instance of the relationship).
     */
    template<typename First>
    flecs::entity target(int32_t index = 0) const;

    /** Get target for a given pair from a singleton entity.
     * This operation returns the target for a given pair. The optional
     * index can be used to iterate through targets, in case the entity has
     * multiple instances for the same relationship.
     *
     * @param first The first element of the pair for which to retrieve the target.
     * @param index The index (0 for the first instance of the relationship).
     */
    template<typename T>
    flecs::entity target(flecs::entity_t first, int32_t index = 0) const;

    /** Get target for a given pair from a singleton entity.
     * This operation returns the target for a given pair. The optional
     * index can be used to iterate through targets, in case the entity has
     * multiple instances for the same relationship.
     *
     * @param first The first element of the pair for which to retrieve the target.
     * @param index The index (0 for the first instance of the relationship).
     */
    flecs::entity target(flecs::entity_t first, int32_t index = 0) const;

    /** Create alias for component.
     *
     * @tparam T to create an alias for.
     * @param alias Alias for the component.
     * @return Entity representing the component.
     */
    template <typename T>
    flecs::entity use(const char *alias = nullptr) const;

    /** Create alias for entity.
     *
     * @param name Name of the entity.
     * @param alias Alias for the entity.
     */
    flecs::entity use(const char *name, const char *alias = nullptr) const;    

    /** Create alias for entity.
     *
     * @param entity Entity for which to create the alias.
     * @param alias Alias for the entity.
     */
    void use(flecs::entity entity, const char *alias = nullptr) const;   

    /** Count entities matching a component.
     *
     * @param component_id The component id.
     */
    int count(flecs::id_t component_id) const {
        return ecs_count_id(m_world, component_id);
    }

    /** Count entities matching a pair.
     *
     * @param first The first element of the pair.
     * @param second The second element of the pair.
     */
    int count(flecs::entity_t first, flecs::entity_t second) const {
        return ecs_count_id(m_world, ecs_pair(first, second));
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
     * @tparam First The first element of the pair.
     * @param second The second element of the pair.
     */
    template <typename First>
    int count(flecs::entity_t second) const {
        return count(_::cpp_type<First>::id(m_world), second);
    }

    /** Count entities matching a pair.
     *
     * @tparam First The first element of the pair.
     * @tparam Second The second element of the pair.
     */
    template <typename First, typename Second>
    int count() const {
        return count( 
            _::cpp_type<First>::id(m_world),
            _::cpp_type<Second>::id(m_world));
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

    /** All entities created in function are created with pair.
     */
    template <typename First, typename Second, typename Func>
    void with(const Func& func) const {
        with(ecs_pair(this->id<First>(), this->id<Second>()), func);
    }

    /** All entities created in function are created with pair.
     */
    template <typename First, typename Func>
    void with(id_t second, const Func& func) const {
        with(ecs_pair(this->id<First>(), second), func);
    } 

    /** All entities created in function are created with pair.
     */
    template <typename Func>
    void with(id_t first, id_t second, const Func& func) const {
        with(ecs_pair(first, second), func);
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
    flecs::scoped_world scope(id_t parent) const;

    template <typename T>
    flecs::scoped_world scope() const;

    /** Delete all entities with specified id. */
    void delete_with(id_t the_id) const {
        ecs_delete_with(m_world, the_id);
    }

    /** Delete all entities with specified pair. */
    void delete_with(entity_t first, entity_t second) const {
        delete_with(ecs_pair(first, second));
    }

    /** Delete all entities with specified component. */
    template <typename T>
    void delete_with() const {
        delete_with(_::cpp_type<T>::id(m_world));
    }

    /** Delete all entities with specified pair. */
    template <typename First, typename Second>
    void delete_with() const {
        delete_with(_::cpp_type<First>::id(m_world), _::cpp_type<Second>::id(m_world));
    }

    /** Remove all instances of specified id. */
    void remove_all(id_t the_id) const {
        ecs_remove_all(m_world, the_id);
    }

    /** Remove all instances of specified pair. */
    void remove_all(entity_t first, entity_t second) const {
        remove_all(ecs_pair(first, second));
    }

    /** Remove all instances of specified component. */
    template <typename T>
    void remove_all() const {
        remove_all(_::cpp_type<T>::id(m_world));
    }

    /** Remove all instances of specified pair. */
    template <typename First, typename Second>
    void remove_all() const {
        remove_all(_::cpp_type<First>::id(m_world), _::cpp_type<Second>::id(m_world));
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

    /** Suspend deferring operations.
     * 
     * @see ecs_defer_suspend
     */
    void defer_suspend() const {
        ecs_defer_suspend(m_world);
    }

    /** Resume deferring operations.
     * 
     * @see ecs_defer_suspend
     */
    void defer_resume() const {
        ecs_defer_resume(m_world);
    }

    /** Check if entity id exists in the world.
     * 
     * @see ecs_exists
     */
    bool exists(flecs::entity_t e) const {
        return ecs_exists(m_world, e);
    }

    /** Check if entity id exists in the world.
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
    void run_post_frame(ecs_fini_action_t action, void *ctx) const {
        ecs_run_post_frame(m_world, action, ctx);
    }

/**
 * @file addons/cpp/mixins/id/mixin.inl
 * @brief Id world mixin.
 */

/** Get id from a type.
 * 
 * \memberof flecs::world
 */
template <typename T>
flecs::id id() const;

/** Id factory.
 * 
 * \memberof flecs::world
 */
template <typename ... Args>
flecs::id id(Args&&... args) const;

/** Get pair id from relationship, object.
 * 
 * \memberof flecs::world
 */
template <typename First, typename Second>
flecs::id pair() const;

/** Get pair id from relationship, object.
 * 
 * \memberof flecs::world
 */
template <typename First>
flecs::id pair(entity_t o) const;

/** Get pair id from relationship, object.
 * 
 * \memberof flecs::world
 */
flecs::id pair(entity_t r, entity_t o) const;

/**
 * @file addons/cpp/mixins/component/mixin.inl
 * @brief Component mixin.
 */

/** Find or register component.
 * 
 * \ingroup cpp_components
 * \memberof flecs::world
 */
template <typename T, typename... Args>
flecs::component<T> component(Args &&... args) const;

/** Find or register untyped component.
 * Method available on flecs::world class.
 * 
 * \ingroup cpp_components
 * \memberof flecs::world
 */
template <typename... Args>
flecs::untyped_component component(Args &&... args) const;

/**
 * @file addons/cpp/mixins/entity/mixin.inl
 * @brief Entity world mixin.
 */

/** Create an entity.
 * 
 * \memberof flecs::world
 * \ingroup cpp_entities
 */
template <typename... Args>
flecs::entity entity(Args &&... args) const;

/** Convert enum constant to entity.
 * 
 * \memberof flecs::world
 * \ingroup cpp_entities
 */
template <typename E, if_t< is_enum<E>::value > = 0>
flecs::entity id(E value) const;

/** Convert enum constant to entity.
 * 
 * \memberof flecs::world
 * \ingroup cpp_entities
 */
template <typename E, if_t< is_enum<E>::value > = 0>
flecs::entity entity(E value) const;

/** Create a prefab.
 * 
 * \memberof flecs::world
 * \ingroup cpp_entities
 */
template <typename... Args>
flecs::entity prefab(Args &&... args) const;

/** Create an entity that's associated with a type.
 * 
 * \memberof flecs::world
 * \ingroup cpp_entities
 */
template <typename T>
flecs::entity entity(const char *name = nullptr) const;

/** Create a prefab that's associated with a type.
 * 
 * \memberof flecs::world
 * \ingroup cpp_entities
 */
template <typename T>
flecs::entity prefab(const char *name = nullptr) const;

/**
 * @file addons/cpp/mixins/event/mixin.inl
 * @brief Event world mixin.
 */

/**
 * @defgroup cpp_addons_event Events
 * @brief API for emitting events.
 * 
 * \ingroup cpp_addons
 * @{
 */

/** Create a new event.
 * 
 * \memberof flecs::world
 * 
 * @param evt The event id.
 * @return Event builder.
 */
flecs::event_builder event(flecs::entity_t evt) const;

/** Create a new event.
 * 
 * \memberof flecs::world
 * 
 * @tparam E The event type.
 * @return Event builder.
 */
template <typename E>
flecs::event_builder_typed<E> event() const;

/** @} */

/**
 * @file addons/cpp/mixins/term/mixin.inl
 * @brief Term world mixin.
 */

/**
 * \memberof flecs::world
 * \ingroup cpp_core_filters
 */

/** Create a term.
 * 
 */
template<typename... Args>
flecs::term term(Args &&... args) const;

/** Create a term for a (component) type.
 */
template<typename T>
flecs::term term() const;  

/** Create a term for a pair.
 */
template<typename First, typename Second>
flecs::term term() const;

/** @} */

/**
 * @file addons/cpp/mixins/filter/mixin.inl
 * @brief Filter world mixin.
 */

/**
 * \memberof flecs::world
 * \ingroup cpp_core_filters
 */

/** Create a filter.
 * 
 * @see ecs_filter_init
 */
template <typename... Comps, typename... Args>
flecs::filter<Comps...> filter(Args &&... args) const;

/** Create a filter builder.
 * 
 * @see ecs_filter_init
 */
template <typename... Comps, typename... Args>
flecs::filter_builder<Comps...> filter_builder(Args &&... args) const;

/** Iterate over all entities with components in argument list of function.
 * The function parameter must match the following signature:
 *   void(*)(T&, U&, ...) or
 *   void(*)(flecs::entity, T&, U&, ...)
 * 
 */
template <typename Func>
void each(Func&& func) const;

/** Iterate over all entities with provided component.
 * The function parameter must match the following signature:
 *   void(*)(T&) or
 *   void(*)(flecs::entity, T&)
 * 
 */
template <typename T, typename Func>
void each(Func&& func) const;

/** Iterate over all entities with provided (component) id. */
template <typename Func>
void each(flecs::id_t term_id, Func&& func) const;

/** @} */

/**
 * @file addons/cpp/mixins/observer/mixin.inl
 * @brief Observer world mixin.
 */

/** Observer builder.
 * 
 * \memberof flecs::world
 * \ingroup cpp_observers
 */

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

/** @} */

/**
 * @file addons/cpp/mixins/query/mixin.inl
 * @brief Query world mixin.
 */

/**
 * \memberof flecs::world
 * \ingroup cpp_core_queries
 */

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

/** @} */

/**
 * @file addons/cpp/mixins/enum/mixin.inl
 * @brief Enum world mixin.
 */

/** Convert enum constant to entity.
 * 
 * \memberof flecs::world
 * \ingroup cpp_entities
 */
template <typename E, if_t< is_enum<E>::value > = 0>
flecs::entity to_entity(E constant) const;


#   ifdef FLECS_MODULE
/**
 * @file addons/cpp/mixins/module/mixin.inl
 * @brief Module world mixin.
 */

/** 
 * \memberof flecs::world
 * \ingroup cpp_addons_modules
 * 
 * @{
 */

/** Define a module.
 * This operation is not mandatory, but can be called inside the module ctor to
 * obtain the entity associated with the module, or override the module name.
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

/** @} */

#   endif
#   ifdef FLECS_PIPELINE
/**
 * @file addons/cpp/mixins/pipeline/mixin.inl
 * @brief Pipeline world mixin.
 */

/**
 * \memberof flecs::world
 * \ingroup cpp_pipelines
 */

/** Create a new pipeline.
 *
 * @return A pipeline builder.
 */
flecs::pipeline_builder<> pipeline() const;

/** Create a new pipeline.
 *
 * @tparam Pipeline Type associated with pipeline.
 * @return A pipeline builder.
 */
template <typename Pipeline, if_not_t< is_enum<Pipeline>::value > = 0>
flecs::pipeline_builder<> pipeline() const;

/** Set pipeline.
 * @see ecs_set_pipeline
 */
void set_pipeline(const flecs::entity pip) const;

/** Set pipeline.
 * @see ecs_set_pipeline
 */
template <typename Pipeline>
void set_pipeline() const;

/** Get pipeline.
 * @see ecs_get_pipeline
 */
flecs::entity get_pipeline() const;

/** Progress world one tick.
 * @see ecs_progress
 */
bool progress(ecs_ftime_t delta_time = 0.0) const;

/** Run pipeline.
 * @see ecs_run_pipeline
 */
void run_pipeline(const flecs::entity_t pip, ecs_ftime_t delta_time = 0.0) const;

/** Run pipeline.
 * @tparam Pipeline Type associated with pipeline.
 * @see ecs_run_pipeline
 */
template <typename Pipeline, if_not_t< is_enum<Pipeline>::value > = 0>
void run_pipeline(ecs_ftime_t delta_time = 0.0) const;

/** Set timescale.
 * @see ecs_set_time_scale
 */
void set_time_scale(ecs_ftime_t mul) const;

/** Get timescale.
 * @see ecs_get_time_scale
 */
ecs_ftime_t get_time_scale() const;

/** Get tick.
 * @return Monotonically increasing frame count.
 */
int64_t get_tick() const;

/** Set target FPS.
 * @see ecs_set_target_fps
 */
void set_target_fps(ecs_ftime_t target_fps) const;

/** Get target FPS.
 * @return Configured frames per second.
 */
ecs_ftime_t get_target_fps() const;

/** Reset simulation clock.
 * @see ecs_reset_clock
 */
void reset_clock() const;

/** Set number of threads.
 * @see ecs_set_threads
 */
void set_threads(int32_t threads) const;

/** Set number of threads.
 * @see ecs_get_stage_count
 */
int32_t get_threads() const;

/** Set number of task threads.
 * @see ecs_set_task_threads
 */
void set_task_threads(int32_t task_threads) const;

/** Returns true if task thread use has been requested.
 * @see ecs_using_task_threads
 */
bool using_task_threads() const;

/** @} */

#   endif
#   ifdef FLECS_SNAPSHOT
/**
 * @file addons/cpp/mixins/snapshot/mixin.inl
 * @brief Snapshot world mixin.
 */

/**
 * \memberof flecs::world
 * \ingroup cpp_addons_snapshot
 */

/** Create a snapshot.
 */
template <typename... Args>
flecs::snapshot snapshot(Args &&... args) const;

/** @} */

#   endif
#   ifdef FLECS_SYSTEM
/**
 * @file addons/cpp/mixins/system/mixin.inl
 * @brief System module world mixin.
 */

/** 
 * \memberof flecs::world
 * \ingroup cpp_addons_system
*/

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

/** @} */

#   endif
#   ifdef FLECS_TIMER
/**
 * @file addons/cpp/mixins/timer/mixin.inl
 * @brief Timer module mixin.
 */

/**
 * \memberof flecs::world
 * \ingroup cpp_addons_timer
 */

/** Find or register a timer. */
template <typename... Args>
flecs::timer timer(Args &&... args) const;

#   endif
#   ifdef FLECS_RULES
/**
 * @file addons/cpp/mixins/rule/mixin.inl
 * @brief Rule world mixin.
 */

/**
 * \memberof flecs::world
 * \ingroup cpp_addons_rules
 */

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

/** @} */

#   endif
#   ifdef FLECS_PLECS
/**
 * @file addons/cpp/mixins/plecs/mixin.inl
 * @brief Plecs world mixin.
 */

/**
 * @defgroup cpp_addons_plecs Plecs
 * @brief Data definition format for loading entity data.
 * 
 * \ingroup cpp_addons
 * @{
 */

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

/** @} */

#   endif
#   ifdef FLECS_META
/**
 * @file addons/cpp/mixins/meta/world.inl
 * @brief Meta world mixin.
 */

/**
 * \memberof flecs::world
 * \ingroup cpp_addons_meta
 * 
 * @{
 */

/** Convert value to string */
flecs::string to_expr(flecs::entity_t tid, const void* value) {
    char *expr = ecs_ptr_to_expr(m_world, tid, value);
    return flecs::string(expr);
}

/** Convert value to string */
template <typename T>
flecs::string to_expr(const T* value) {
    flecs::entity_t tid = _::cpp_type<T>::id(m_world);
    return to_expr(tid, value);
}

/** Return meta cursor to value */
flecs::cursor cursor(flecs::entity_t tid, void *ptr) {
    return flecs::cursor(m_world, tid, ptr);
}

/** Return meta cursor to value */
template <typename T>
flecs::cursor cursor(void *ptr) {
    flecs::entity_t tid = _::cpp_type<T>::id(m_world);
    return cursor(tid, ptr);
}

/** Create primitive type */
flecs::entity primitive(flecs::meta::primitive_kind_t kind);

/** Create array type. */
flecs::entity array(flecs::entity_t elem_id, int32_t array_count);

/** Create array type. */
template <typename T>
flecs::entity array(int32_t array_count);

/** Create vector type. */
flecs::entity vector(flecs::entity_t elem_id);

/** Create vector type. */
template <typename T>
flecs::entity vector();

/** @} */

#   endif
#   ifdef FLECS_JSON
/**
 * @file addons/cpp/mixins/json/world.inl
 * @brief JSON world mixin.
 */

/** Serialize untyped value to JSON.
 * 
 * \memberof flecs::world
 * \ingroup cpp_addons_json
 */
flecs::string to_json(flecs::entity_t tid, const void* value) {
    char *json = ecs_ptr_to_json(m_world, tid, value);
    return flecs::string(json);
}

/** Serialize value to JSON.
 * 
 * \memberof flecs::world
 * \ingroup cpp_addons_json
 */
template <typename T>
flecs::string to_json(const T* value) {
    flecs::entity_t tid = _::cpp_type<T>::id(m_world);
    return to_json(tid, value);
}

/** Serialize world to JSON.
 * 
 * \memberof flecs::world
 * \ingroup cpp_addons_json
 */
flecs::string to_json() {
    return flecs::string( ecs_world_to_json(m_world, nullptr) );
}

/** Deserialize value from JSON.
 * 
 * \memberof flecs::world
 * \ingroup cpp_addons_json
 */
template <typename T>
const char* from_json(flecs::entity_t tid, void* value, const char *json, flecs::from_json_desc_t *desc = nullptr) {
    return ecs_ptr_from_json(m_world, tid, value, json, desc);
}

/** Deserialize value from JSON.
 * 
 * \memberof flecs::world
 * \ingroup cpp_addons_json
 */
template <typename T>
const char* from_json(T* value, const char *json, flecs::from_json_desc_t *desc = nullptr) {
    return ecs_ptr_from_json(m_world, _::cpp_type<T>::id(m_world),
        value, json, desc);
}

/** Deserialize JSON into world.
 * 
 * \memberof flecs::world
 * \ingroup cpp_addons_json
 */
const char* from_json(const char *json, flecs::from_json_desc_t *desc = nullptr) {
    return ecs_world_from_json(m_world, json, desc);
}

#   endif
#   ifdef FLECS_APP
/**
 * @file addons/cpp/mixins/app/mixin.inl
 * @brief App world addon mixin.
 */

/** Return app builder.
 * The app builder is a convenience wrapper around a loop that runs 
 * world::progress. An app allows for writing platform agnostic code,
 * as it provides hooks to modules for overtaking the main loop which is 
 * required for frameworks like emscripten.
 * 
 * \ingroup cpp_addons_app
 * \memberof flecs::world
 */
flecs::app_builder app() {
    m_owned = false; // App takes ownership of world
    return flecs::app_builder(m_world);
}

/** @} */

#   endif
#   ifdef FLECS_METRICS

/** Create metric.
 * 
 * \ingroup cpp_addons_metrics
 * \memberof flecs::world
 */
template <typename... Args>
flecs::metric_builder metric(Args &&... args) const;

#   endif
#   ifdef FLECS_ALERTS

/** Create alert.
 * 
 * \ingroup cpp_addons_alerts
 * \memberof flecs::world
 */
template <typename... Comps, typename... Args>
flecs::alert_builder<Comps...> alert(Args &&... args) const;

#   endif

public:
    void init_builtin_components();

    world_t *m_world;
    bool m_owned;
};

/** Scoped world.
 * Utility class used by the world::scope method to create entities in a scope.
 */
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

/** @} */

} // namespace flecs

/**
 * @file addons/cpp/iter.hpp
 * @brief Wrapper classes for ecs_iter_t and component arrays.
 */

#pragma once

/**
 * @defgroup cpp_iterator Iterators
 * @brief Iterator operations.
 * 
 * \ingroup cpp_core
 * @{
 */

namespace flecs 
{

/** Unsafe wrapper class around a column.
 * This class can be used when a system does not know the type of a column at
 * compile time.
 * 
 * \ingroup cpp_iterator
 */
struct untyped_column {
    untyped_column(void* array, size_t size, size_t count, bool is_shared = false)
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
    void* operator[](size_t index) const {
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
 * 
 * \ingroup cpp_iterator
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
    T& operator[](size_t index) const {
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
    T& operator*() const {
      ecs_assert(m_array != nullptr, ECS_COLUMN_INDEX_OUT_OF_RANGE, NULL);
      return *m_array;
    }

    /** Return first element of component array.
     * This operator is typically used when the column is shared.
     * 
     * @return Pointer to the first element.
     */
    T* operator->() const {
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

/** Class for iterating over query results.
 * 
 * \ingroup cpp_iterator
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

    ecs_ftime_t delta_time() const {
        return m_iter->delta_time;
    }

    ecs_ftime_t delta_system_time() const {
        return m_iter->delta_system_time;
    }

    flecs::type type() const;

    flecs::table table() const;

    flecs::table_range range() const;

    /** Is current type a module or does it contain module contents? */
    bool has_module() const {
        return ecs_table_has_module(m_iter->table);
    }

    /** Access ctx. 
     * ctx contains the context pointer assigned to a system.
     */
    void* ctx() {
        return m_iter->ctx;
    }

    /** Access ctx. 
     * ctx contains the context pointer assigned to a system.
     */
    template <typename T>
    T* ctx() {
        return static_cast<T*>(m_iter->ctx);
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

    /** Returns whether field is matched on self.
     * 
     * @param index The field index.
     */
    bool is_self(int32_t index) const {
        return ecs_field_is_self(m_iter, index);
    }

    /** Returns whether field is set.
     * 
     * @param index The field index.
     */
    bool is_set(int32_t index) const {
        return ecs_field_is_set(m_iter, index);
    }

    /** Returns whether field is readonly.
     *
     * @param index The field index.
     */
    bool is_readonly(int32_t index) const {
        return ecs_field_is_readonly(m_iter, index);
    }

    /** Number of fields in iteator.
     */
    int32_t field_count() const {
        return m_iter->field_count;
    }

    /** Size of field data type.
     *
     * @param index The field id.
     */
    size_t size(int32_t index) const {
        return ecs_field_size(m_iter, index);
    }

    /** Obtain field source (0 if This).
     *
     * @param index The field index.
     */    
    flecs::entity src(int32_t index) const;

    /** Obtain id matched for field.
     *
     * @param index The field index.
     */
    flecs::entity id(int32_t index) const;

    /** Obtain pair id matched for field.
     * This operation will fail if the id is not a pair.
     * 
     * @param index The field index.
     */
    flecs::id pair(int32_t index) const;

    /** Obtain column index for field.
     *
     * @param index The field index.
     */    
    int32_t column_index(int32_t index) const {
        return ecs_field_column_index(m_iter, index);
    }

    /** Convert current iterator result to string.
     */
    flecs::string str() const {
        char *s = ecs_iter_str(m_iter);
        return flecs::string(s);
    }

    /** Get readonly access to field data.
     * If the specified field index does not match with the provided type, the
     * function will assert.
     *
     * @tparam T Type of the field.
     * @param index The field index.
     * @return The field data.
     */
    template <typename T, typename A = actual_type_t<T>,
        typename std::enable_if<std::is_const<T>::value, void>::type* = nullptr>
    flecs::column<A> field(int32_t index) const {
        return get_field<A>(index);
    }

    /** Get read/write access to field data.
     * If the matched id for the specified field does not match with the provided 
     * type or if the field is readonly, the function will assert.
     *
     * @tparam T Type of the field.
     * @param index The field index.
     * @return The field data.
     */
    template <typename T, typename A = actual_type_t<T>,
        typename std::enable_if<
            std::is_const<T>::value == false, void>::type* = nullptr>
    flecs::column<A> field(int32_t index) const {
        ecs_assert(!ecs_field_is_readonly(m_iter, index), 
            ECS_ACCESS_VIOLATION, NULL);
        return get_field<A>(index);
    }

    /** Get unchecked access to field data.
     * Unchecked access is required when a system does not know the type of a
     * field at compile time.
     *
     * @param index The field index. 
     */
    flecs::untyped_column field(int32_t index) const {
        return get_unchecked_field(index);
    }

    /** Get readonly access to entity ids.
     *
     * @return The entity ids.
     */
    flecs::column<const flecs::entity_t> entities() const {
        return flecs::column<const flecs::entity_t>(m_iter->entities, static_cast<size_t>(m_iter->count), false);
    }

    /** Obtain the total number of tables the iterator will iterate over. */
    int32_t table_count() const {
        return m_iter->table_count;
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

    /* Return group id for current table (grouped queries only) */
    uint64_t group_id() const {
        return m_iter->group_id;
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
    /* Get field, check if correct type is used */
    template <typename T, typename A = actual_type_t<T>>
    flecs::column<T> get_field(int32_t index) const {

#ifndef FLECS_NDEBUG
        ecs_entity_t term_id = ecs_field_id(m_iter, index);
        ecs_assert(ECS_HAS_ID_FLAG(term_id, PAIR) ||
            term_id == _::cpp_type<T>::id(m_iter->world), 
            ECS_COLUMN_TYPE_MISMATCH, NULL);
#endif

        size_t count;
        bool is_shared = !ecs_field_is_self(m_iter, index);

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
            static_cast<T*>(ecs_field_w_size(m_iter, sizeof(A), index)), 
            count, is_shared);
    }

    flecs::untyped_column get_unchecked_field(int32_t index) const {
        size_t count;
        size_t size = ecs_field_size(m_iter, index);
        bool is_shared = !ecs_field_is_self(m_iter, index);

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

        return flecs::untyped_column(
            ecs_field_w_size(m_iter, 0, index), size, count, is_shared);
    }     

    flecs::iter_t *m_iter;
    std::size_t m_begin;
    std::size_t m_end;
};

} // namespace flecs

/** @} */

/**
 * @file addons/cpp/entity.hpp
 * @brief Entity class.
 * 
 * This class provides read/write access to entities.
 */

#pragma once

/**
 * @file addons/cpp/entity_view.hpp
 * @brief Entity class with only readonly operations.
 * 
 * This class provides readonly access to entities. Using this class to store 
 * entities in components ensures valid handles, as this class will always store
 * the actual world vs. a stage. The constructors of this class will never 
 * create a new entity.
 *
 * To obtain a mutable handle to the entity, use the "mut" function.
 */

#pragma once

/**
 * \ingroup cpp_entities
 * @{
 */

namespace flecs
{

/** Entity view.
 * Class with read operations for entities. Base for flecs::entity.
 * 
 * \ingroup cpp_entities
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
        return path_from(0, sep, init_sep);
    }   

    /** Return the entity path relative to a parent.
     *
     * @return The relative hierarchical entity path.
     */
    flecs::string path_from(flecs::entity_t parent, const char *sep = "::", const char *init_sep = "::") const {
        char *path = ecs_get_path_w_sep(m_world, parent, m_id, sep, init_sep);
        return flecs::string(path);
    }

    /** Return the entity path relative to a parent.
     *
     * @return The relative hierarchical entity path.
     */
    template <typename Parent>
    flecs::string path_from(const char *sep = "::", const char *init_sep = "::") const {
        return path_from(_::cpp_type<Parent>::id(m_world), sep, init_sep);
    }

    bool enabled() const {
        return !ecs_has_id(m_world, m_id, flecs::Disabled);
    }

    /** Get the entity's type.
     *
     * @return The entity's type.
     */
    flecs::type type() const;

    /** Get the entity's table.
     *
     * @return Returns the entity's table.
     */
    flecs::table table() const;

    /** Get table range for the entity.
     * Returns a range with the entity's row as offset and count set to 1. If
     * the entity is not stored in a table, the function returns a range with
     * count 0.
     *
     * @return Returns the entity's table range.
     */
    flecs::table_range range() const;

    /** Iterate (component) ids of an entity.
     * The function parameter must match the following signature:
     *   void(*)(flecs::id id)
     *
     * @param func The function invoked for each id.
     */
    template <typename Func>
    void each(const Func& func) const;

    /** Iterate matching pair ids of an entity.
     * The function parameter must match the following signature:
     *   void(*)(flecs::id id)
     *
     * @param func The function invoked for each id.
     */
    template <typename Func>
    void each(flecs::id_t first, flecs::id_t second, const Func& func) const;

    /** Iterate targets for a given relationship.
     * The function parameter must match the following signature:
     *   void(*)(flecs::entity target)
     *
     * @param rel The relationship for which to iterate the targets.
     * @param func The function invoked for each target.
     */
    template <typename Func>
    void each(const flecs::entity_view& rel, const Func& func) const;

    /** Iterate targets for a given relationship.
     * The function parameter must match the following signature:
     *   void(*)(flecs::entity target)
     *
     * @tparam First The relationship for which to iterate the targets.
     * @param func The function invoked for each target.     
     */
    template <typename First, typename Func>
    void each(const Func& func) const { 
        return each(_::cpp_type<First>::id(m_world), func);
    }

    /** Iterate children for entity.
     * The function parameter must match the following signature:
     *   void(*)(flecs::entity target)
     *
     * @param rel The relationship to follow.
     * @param func The function invoked for each child.     
     */
    template <typename Func>
    void children(flecs::entity_t rel, Func&& func) const {
        /* When the entity is a wildcard, this would attempt to query for all
         * entities with (ChildOf, *) or (ChildOf, _) instead of querying for
         * the children of the wildcard entity. */
        if (m_id == flecs::Wildcard || m_id == flecs::Any) {
            /* This is correct, wildcard entities don't have children */
            return;
        }

        flecs::world world(m_world);

        ecs_term_t terms[2];
        ecs_filter_t f = ECS_FILTER_INIT;
        f.terms = terms;
        f.term_count = 2;

        ecs_filter_desc_t desc = {};
        desc.terms[0].first.id = rel;
        desc.terms[0].second.id = m_id;
        desc.terms[0].second.flags = EcsIsEntity;
        desc.terms[1].id = flecs::Prefab;
        desc.terms[1].oper = EcsOptional;
        desc.storage = &f;
        if (ecs_filter_init(m_world, &desc) != nullptr) {
            ecs_iter_t it = ecs_filter_iter(m_world, &f);
            while (ecs_filter_next(&it)) {
                _::each_invoker<Func>(FLECS_MOV(func)).invoke(&it);
            }

            ecs_filter_fini(&f);
        }
    }

    /** Iterate children for entity.
     * The function parameter must match the following signature:
     *   void(*)(flecs::entity target)
     *
     * @tparam Rel The relationship to follow.
     * @param func The function invoked for each child.     
     */
    template <typename Rel, typename Func>
    void children(Func&& func) const {
        children(_::cpp_type<Rel>::id(m_world), FLECS_MOV(func));
    }

    /** Iterate children for entity.
     * The function parameter must match the following signature:
     *   void(*)(flecs::entity target)
     * 
     * This operation follows the ChildOf relationship.
     *
     * @param func The function invoked for each child.     
     */
    template <typename Func>
    void children(Func&& func) const {
        children(flecs::ChildOf, FLECS_MOV(func));
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
     * @tparam First The first element of the pair.
     * @tparam Second the second element of a pair.
     */
    template <typename First, typename Second, typename P = pair<First, Second>, 
        typename A = actual_type_t<P>, if_not_t< flecs::is_pair<First>::value > = 0>
    const A* get() const {
        return this->get<P>();
    }

    /** Get a pair.
     * This operation gets the value for a pair from the entity. 
     *
     * @tparam First The first element of the pair.
     * @param second The second element of the pair.
     */
    template<typename First, typename Second, if_not_t< is_enum<Second>::value> = 0>
    const First* get(Second second) const {
        auto comp_id = _::cpp_type<First>::id(m_world);
        ecs_assert(_::cpp_type<First>::size() != 0, ECS_INVALID_PARAMETER, NULL);
        return static_cast<const First*>(
            ecs_get_id(m_world, m_id, ecs_pair(comp_id, second)));
    }

    /** Get a pair.
     * This operation gets the value for a pair from the entity. 
     *
     * @tparam First The first element of the pair.
     * @param constant the enum constant.
     */
    template<typename First, typename Second, if_t<is_enum<Second>::value> = 0>
    const First* get(Second constant) const {
        const auto& et = enum_type<Second>(this->m_world);
        flecs::entity_t target = et.entity(constant);
        return get<First>(target);
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
     * first nor the second part of the pair are components, the operation 
     * will fail.
     *
     * @param first The first element of the pair.
     * @param second The second element of the pair.
     */
    const void* get(flecs::entity_t first, flecs::entity_t second) const {
        return ecs_get_id(m_world, m_id, ecs_pair(first, second));
    }

    /** Get 1..N components.
     * This operation accepts a callback with as arguments the components to
     * retrieve. The callback will only be invoked when the entity has all
     * the components.
     *
     * This operation is faster than individually calling get for each component
     * as it only obtains entity metadata once.
     * 
     * While the callback is invoked the table in which the components are
     * stored is locked, which prevents mutations that could cause invalidation
     * of the component references. Note that this is not an actual lock: 
     * invalid access causes a runtime panic and so it is still up to the 
     * application to ensure access is protected.
     * 
     * The component arguments must be references and can be either const or
     * non-const. When all arguments are const, the function will read-lock the
     * table (see ecs_read_begin). If one or more arguments are non-const the
     * function will write-lock the table (see ecs_write_begin).
     * 
     * Example:
     *   e.get([](Position& p, Velocity& v) { // write lock
     *     p.x += v.x;
     *   });
     * 
     *   e.get([](const Position& p) {        // read lock
     *     std::cout << p.x << std::endl;
     *   });
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

    /** Get the second part for a pair.
     * This operation gets the value for a pair from the entity. The first
     * part of the pair should not be a component.
     *
     * @tparam Second the second element of a pair.
     * @param first The first part of the pair.
     */
    template<typename Second>
    const Second* get_second(flecs::entity_t first) const {
        auto second = _::cpp_type<Second>::id(m_world);
        ecs_assert(_::cpp_type<Second>::size() != 0, ECS_INVALID_PARAMETER, NULL);
        return static_cast<const Second*>(
            ecs_get_id(m_world, m_id, ecs_pair(first, second)));
    }

    /** Get the second part for a pair.
     * This operation gets the value for a pair from the entity. The first
     * part of the pair should not be a component.
     *
     * @tparam First The first element of the pair.
     * @tparam Second the second element of a pair.
     */
    template<typename First, typename Second>
    const Second* get_second() const {
        return get<pair_object<First, Second>>();
    }

    /** Get target for a given pair.
     * This operation returns the target for a given pair. The optional
     * index can be used to iterate through targets, in case the entity has
     * multiple instances for the same relationship.
     *
     * @tparam First The first element of the pair.
     * @param index The index (0 for the first instance of the relationship).
     */
    template<typename First>
    flecs::entity target(int32_t index = 0) const;

    /** Get target for a given pair.
     * This operation returns the target for a given pair. The optional
     * index can be used to iterate through targets, in case the entity has
     * multiple instances for the same relationship.
     *
     * @param first The first element of the pair for which to retrieve the target.
     * @param index The index (0 for the first instance of the relationship).
     */
    flecs::entity target(flecs::entity_t first, int32_t index = 0) const;

    /** Get the target of a pair for a given relationship id.
     * This operation returns the first entity that has the provided id by following
     * the specified relationship. If the entity itself has the id then entity will
     * be returned. If the id cannot be found on the entity or by following the
     * relationship, the operation will return 0.
     * 
     * This operation can be used to lookup, for example, which prefab is providing
     * a component by specifying the IsA pair:
     * 
     *   // Is Position provided by the entity or one of its base entities?
     *   ecs_get_target_for_id(world, entity, EcsIsA, ecs_id(Position))
     * 
     * @param relationship The relationship to follow.
     * @param id The id to lookup.
     * @return The entity for which the target has been found.
     */
    flecs::entity target_for(flecs::entity_t relationship, flecs::id_t id) const;

    template <typename T>
    flecs::entity target_for(flecs::entity_t relationship) const;

    template <typename First, typename Second>
    flecs::entity target_for(flecs::entity_t relationship) const;

    /** Get depth for given relationship.
     *
     * @param rel The relationship.
     * @return The depth.
     */
    int32_t depth(flecs::entity_t rel) const {
        return ecs_get_depth(m_world, m_id, rel);
    }

    /** Get depth for given relationship.
     *
     * @tparam Rel The relationship.
     * @return The depth.
     */
    template<typename Rel>
    int32_t depth() const {
        return this->depth(_::cpp_type<Rel>::id(m_world));
    }

    /** Get parent of entity.
     * Short for target(flecs::ChildOf).
     * 
     * @return The parent of the entity.
     */
    flecs::entity parent() const;
    
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
     * @tparam First The first element of the pair.
     * @tparam Second The second element of the pair.
     * @return True if the entity has the provided component, false otherwise.
     */
    template <typename First, typename Second>
    bool has() const {
        return this->has<First>(_::cpp_type<Second>::id(m_world));
    }

    /** Check if entity has the provided pair.
     *
     * @tparam First The first element of the pair.
     * @param second The second element of the pair.
     * @return True if the entity has the provided component, false otherwise.
     */
    template<typename First, typename Second, if_not_t< is_enum<Second>::value > = 0>
    bool has(Second second) const {
        auto comp_id = _::cpp_type<First>::id(m_world);
        return ecs_has_id(m_world, m_id, ecs_pair(comp_id, second));
    }

    /** Check if entity has the provided pair.
     *
     * @tparam Second The second element of the pair.
     * @param first The first element of the pair.
     * @return True if the entity has the provided component, false otherwise.
     */
    template <typename Second>
    bool has_second(flecs::entity_t first) const {
        return this->has(first, _::cpp_type<Second>::id(m_world));
    }

    /** Check if entity has the provided pair.
     *
     * @tparam First The first element of the pair.
     * @param value The enum constant.
     * @return True if the entity has the provided component, false otherwise.
     */
    template<typename First, typename E, if_t< is_enum<E>::value > = 0>
    bool has(E value) const {
        const auto& et = enum_type<E>(this->m_world);
        flecs::entity_t second = et.entity(value);
        return has<First>(second);
    }

    /** Check if entity has the provided pair.
     *
     * @param first The first element of the pair.
     * @param second The second element of the pair.
     * @return True if the entity has the provided component, false otherwise.
     */
    bool has(flecs::id_t first, flecs::id_t second) const {
        return ecs_has_id(m_world, m_id, ecs_pair(first, second));
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
     * @tparam First The first element of the pair.
     * @param second The second element of the pair.
     * @return True if the entity owns the provided component, false otherwise.
     */
    template <typename First>
    bool owns(flecs::id_t second) const {
        auto comp_id = _::cpp_type<First>::id(m_world);
        return owns(ecs_pair(comp_id, second));
    }

    /** Check if entity owns the provided pair.
     *
     * @param first The first element of the pair.
     * @param second The second element of the pair.
     * @return True if the entity owns the provided component, false otherwise.
     */
    bool owns(flecs::id_t first, flecs::id_t second) const {
        return owns(ecs_pair(first, second));
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

    /** Check if entity owns the provided pair.
     * An pair is owned if it is not shared from a base entity.
     *
     * @tparam First The first element of the pair.
     * @tparam Second The second element of the pair.
     * @return True if the entity owns the provided pair, false otherwise.
     */
    template <typename First, typename Second>
    bool owns() const {
        return owns(
            _::cpp_type<First>::id(m_world),
            _::cpp_type<Second>::id(m_world));
    }

    /** Test if id is enabled.
     *
     * @param id The id to test.
     * @return True if enabled, false if not.
     */
    bool enabled(flecs::id_t id) const {
        return ecs_is_enabled_id(m_world, m_id, id);
    }

    /** Test if component is enabled.
     *
     * @tparam T The component to test.
     * @return True if enabled, false if not.
     */
    template<typename T>
    bool enabled() const {
        return this->enabled(_::cpp_type<T>::id(m_world));
    }

    /** Test if pair is enabled.
     *
     * @param first The first element of the pair.
     * @param second The second element of the pair.
     * @return True if enabled, false if not.
     */
    bool enabled(flecs::id_t first, flecs::id_t second) const {
        return this->enabled(ecs_pair(first, second));
    }

    /** Test if pair is enabled.
     *
     * @tparam First The first element of the pair.
     * @param second The second element of the pair.
     * @return True if enabled, false if not.
     */
    template <typename First>
    bool enabled(flecs::id_t second) const {
        return this->enabled(_::cpp_type<First>::id(m_world), second);
    }

    /** Test if pair is enabled.
     *
     * @tparam First The first element of the pair.
     * @tparam Second The second element of the pair.
     * @return True if enabled, false if not.
     */
    template <typename First, typename Second>
    bool enabled() const {
        return this->enabled<First>(_::cpp_type<Second>::id(m_world));
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
/**
 * @file addons/cpp/mixins/json/entity_view.inl
 * @brief JSON entity mixin.
 */

/** Serialize entity to JSON.
 * 
 * \memberof flecs::entity_view
 * \ingroup cpp_addons_json
 */
flecs::string to_json(const flecs::entity_to_json_desc_t *desc = nullptr) {
    char *json = ecs_entity_to_json(m_world, m_id, desc);
    return flecs::string(json);
}

#   endif
#   ifdef FLECS_DOC
/**
 * @file addons/cpp/mixins/doc/entity_view.inl
 * @brief Doc entity view mixin.
 */

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

const char* doc_color() {
    return ecs_doc_get_color(m_world, m_id);
}

#   endif
#   ifdef FLECS_ALERTS
/**
 * @file addons/cpp/mixins/alerts/entity_view.inl
 * @brief Alerts entity mixin.
 */

/** Return number of alerts for entity.
 * 
 * \memberof flecs::entity_view
 * \ingroup cpp_addons_alerts
 */
int32_t alert_count(flecs::entity_t alert = 0) const {
    return ecs_get_alert_count(m_world, m_id, alert);
}

#   endif

/**
 * @file addons/cpp/mixins/enum/entity_view.inl
 * @brief Enum entity view mixin.
 */

/** Convert entity to enum constant.
 * 
 * \memberof flecs::entity_view
 * \ingroup cpp_entities
 */
template <typename E>
E to_constant() const;



private:
    flecs::entity set_stage(world_t *stage);
};

}

/** @} */

/**
 * @file addons/cpp/mixins/entity/builder.hpp
 * @brief Entity builder.
 */

#pragma once

namespace flecs
{

/** Entity builder. 
 * \ingroup cpp_entities
 */
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
     * @param first The first element of the pair.
     * @param second The second element of the pair.
     */
    Self& add(entity_t first, entity_t second) {
        ecs_add_pair(this->m_world, this->m_id, first, second);
        return to_base();
    }

    /** Add a pair.
     * This operation adds a pair to the entity.
     *
     * @tparam First The first element of the pair
     * @tparam Second The second element of the pair
     */
    template<typename First, typename Second>
    Self& add() {
        return this->add<First>(_::cpp_type<Second>::id(this->m_world));
    }

    /** Add a pair.
     * This operation adds a pair to the entity.
     *
     * @tparam First The first element of the pair
     * @param second The second element of the pair.
     */
    template<typename First, typename Second, if_not_t< is_enum<Second>::value > = 0>
    Self& add(Second second) {
        flecs_static_assert(is_flecs_constructible<First>::value,
            "cannot default construct type: add T::T() or use emplace<T>()");
        return this->add(_::cpp_type<First>::id(this->m_world), second);
    }

    /** Add a pair.
     * This operation adds a pair to the entity that consists out of a tag
     * combined with an enum constant.
     *
     * @tparam First The first element of the pair
     * @param constant the enum constant.
     */
    template<typename First, typename Second, if_t< is_enum<Second>::value > = 0>
    Self& add(Second constant) {
        flecs_static_assert(is_flecs_constructible<First>::value,
            "cannot default construct type: add T::T() or use emplace<T>()");
        const auto& et = enum_type<Second>(this->m_world);
        return this->add<First>(et.entity(constant));
    }

    /** Add a pair.
     * This operation adds a pair to the entity.
     *
     * @param first The first element of the pair
     * @tparam Second The second element of the pair
     */
    template<typename Second>
    Self& add_second(flecs::entity_t first) {
        return this->add(first, _::cpp_type<Second>::id(this->m_world));
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
     * @param first The first element of the pair.
     * @param second The second element of the pair.
     */
    Self& add_if(bool cond, flecs::entity_t first, flecs::entity_t second) {
        if (cond) {
            return this->add(first, second);
        } else {
            /* If second is 0 or if relationship is exclusive, use wildcard for
             * second which will remove all instances of the relationship.
             * Replacing 0 with Wildcard will make it possible to use the second
             * as the condition. */
            if (!second || ecs_has_id(this->m_world, first, flecs::Exclusive)) {
                second = flecs::Wildcard;
            }
            return this->remove(first, second);
        }
    }

    /** Conditional add.
     * This operation adds if condition is true, removes if condition is false.
     * 
     * @tparam First The first element of the pair
     * @param cond The condition to evaluate.
     * @param second The second element of the pair.
     */
    template <typename First>
    Self& add_if(bool cond, flecs::entity_t second) {
        return this->add_if(cond, _::cpp_type<First>::id(this->m_world), second);
    }

    /** Conditional add.
     * This operation adds if condition is true, removes if condition is false.
     * 
     * @tparam First The first element of the pair
     * @tparam Second The second element of the pair
     * @param cond The condition to evaluate.
     */
    template <typename First, typename Second>
    Self& add_if(bool cond) {
        return this->add_if<First>(cond, _::cpp_type<Second>::id(this->m_world));
    }

    /** Conditional add.
     * This operation adds if condition is true, removes if condition is false.
     * 
     * @param cond The condition to evaluate.
     * @param constant The enumeration constant.
     */
    template <typename E, if_t< is_enum<E>::value > = 0>
    Self& add_if(bool cond, E constant) {
        const auto& et = enum_type<E>(this->m_world);
        return this->add_if<E>(cond, et.entity(constant));
    }

    /** Shortcut for add(IsA, entity).
     *
     * @param second The second element of the pair.
     */
    Self& is_a(entity_t second) {
        return this->add(flecs::IsA, second);
    }

    /** Shortcut for add(IsA, entity).
     *
     * @tparam T the type associated with the entity.
     */
    template <typename T>
    Self& is_a() {
        return this->add(flecs::IsA, _::cpp_type<T>::id(this->m_world));
    }

    /** Shortcut for add(ChildOf, entity).
     *
     * @param second The second element of the pair.
     */
    Self& child_of(entity_t second) {
        return this->add(flecs::ChildOf, second);
    }

    /** Shortcut for add(DependsOn, entity).
     *
     * @param second The second element of the pair.
     */
    Self& depends_on(entity_t second) {
        return this->add(flecs::DependsOn, second);
    }

    /** Shortcut for add(SlotOf, entity).
     *
     * @param second The second element of the pair.
     */
    Self& slot_of(entity_t second) {
        return this->add(flecs::SlotOf, second);
    }

    /** Shortcut for add(SlotOf, target(ChildOf)).
     */
    Self& slot() {
        ecs_check(ecs_get_target(m_world, m_id, flecs::ChildOf, 0), 
            ECS_INVALID_PARAMETER, "add ChildOf pair before using slot()");
        return this->slot_of(this->target(flecs::ChildOf));
    error:
        return to_base();
    }

    /** Shortcut for add(ChildOf, entity).
     *
     * @tparam T the type associated with the entity.
     */
    template <typename T>
    Self& child_of() {
        return this->child_of(_::cpp_type<T>::id(this->m_world));
    }
 
    /** Shortcut for add(DependsOn, entity).
     *
     * @tparam T the type associated with the entity.
     */
    template <typename T>
    Self& depends_on() {
        return this->depends_on(_::cpp_type<T>::id(this->m_world));
    }

    /** Shortcut for add(SlotOf, entity).
     *
     * @tparam T the type associated with the entity.
     */
    template <typename T>
    Self& slot_of() {
        return this->slot_of(_::cpp_type<T>::id(this->m_world));
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
     * @param first The first element of the pair.
     * @param second The second element of the pair.
     */
    Self& remove(entity_t first, entity_t second) {
        ecs_remove_pair(this->m_world, this->m_id, first, second);
        return to_base();
    }

    /** Removes a pair.
     * This operation removes a pair from the entity.
     *
     * @tparam First The first element of the pair
     * @tparam Second The second element of the pair
     */
    template<typename First, typename Second>
    Self& remove() {
        return this->remove<First>(_::cpp_type<Second>::id(this->m_world));
    }

    /** Remove a pair.
     * This operation adds a pair to the entity.
     *
     * @tparam First The first element of the pair
     * @param second The second element of the pair.
     */
    template<typename First, typename Second, if_not_t< is_enum<Second>::value > = 0>
    Self& remove(Second second) {
        return this->remove(_::cpp_type<First>::id(this->m_world), second);
    }

    /** Removes a pair.
     * This operation removes a pair from the entity.
     *
     * @tparam Second The second element of the pair
     * @param first The first element of the pair
     */
    template<typename Second>
    Self& remove_second(flecs::entity_t first) {
        return this->remove(first, _::cpp_type<Second>::id(this->m_world));
    }

    /** Remove a pair.
     * This operation adds a pair to the entity.
     *
     * @tparam First The first element of the pair
     * @param constant the enum constant.
     */
    template<typename First, typename Second, if_t< is_enum<Second>::value > = 0>
    Self& remove(Second constant) {
        const auto& et = enum_type<Second>(this->m_world);
        flecs::entity_t second = et.entity(constant);
        return this->remove<First>(second);
    }  

    /** Mark id for auto-overriding.
     * When an entity inherits from a base entity (using the IsA relationship)
     * any ids marked for auto-overriding on the base will be overridden
     * automatically by the entity.
     *
     * @param id The id to mark for overriding.
     */    
    Self& override(flecs::id_t id) {
        return this->add(ECS_OVERRIDE | id);
    }

    /** Mark pair for auto-overriding.
     * @see override(flecs::id_t id)
     *
     * @param first The first element of the pair.
     * @param second The second element of the pair.
     */     
    Self& override(flecs::entity_t first, flecs::entity_t second) {
        return this->override(ecs_pair(first, second));
    }

    /** Mark component for auto-overriding.
     * @see override(flecs::id_t id)
     *
     * @tparam T The component to mark for overriding.
     */     
    template <typename T>
    Self& override() {
        return this->override(_::cpp_type<T>::id(this->m_world));
    }

    /** Mark pair for auto-overriding.
     * @see override(flecs::id_t id)
     *
     * @tparam First The first element of the pair.
     * @param second The second element of the pair.
     */     
    template <typename First>
    Self& override(flecs::entity_t second) {
        return this->override(_::cpp_type<First>::id(this->m_world), second);
    }

    /** Mark pair for auto-overriding.
     * @see override(flecs::id_t id)
     *
     * @tparam First The first element of the pair.
     * @tparam Second The second element of the pair.
     */     
    template <typename First, typename Second>
    Self& override() {
        return this->override<First>(_::cpp_type<Second>::id(this->m_world));
    }

    /** Set component, mark component for auto-overriding.
     * @see override(flecs::id_t id)
     *
     * @tparam T The component to set and for which to add the OVERRIDE flag
     */    
    template <typename T>
    Self& set_override(const T& val) {
        this->override<T>();
        return this->set<T>(val);
    }

    /** Set component, mark component for auto-overriding.
     * @see override(flecs::id_t id)
     *
     * @tparam T The component to set and for which to add the OVERRIDE flag
     */    
    template <typename T>
    Self& set_override(T&& val) {
        this->override<T>();
        return this->set<T>(FLECS_FWD(val));
    }

    /** Set pair, mark component for auto-overriding.
     * @see override(flecs::id_t id)
     *
     * @tparam First The first element of the pair.
     * @param second The second element of the pair.
     */    
    template <typename First>
    Self& set_override(flecs::entity_t second, const First& val) {
        this->override<First>(second);
        return this->set<First>(second, val);
    }

    /** Set pair, mark component for auto-overriding.
     * @see override(flecs::id_t id)
     *
     * @tparam First The first element of the pair.
     * @param second The second element of the pair.
     */    
    template <typename First>
    Self& set_override(flecs::entity_t second, First&& val) {
        this->override<First>(second);
        return this->set<First>(second, FLECS_FWD(val));
    }

    /** Set component, mark component for auto-overriding.
     * @see override(flecs::id_t id)
     *
     * @tparam First The first element of the pair.
     * @tparam Second The second element of the pair.
     */    
    template <typename First, typename Second, typename P = pair<First, Second>, 
        typename A = actual_type_t<P>, if_not_t< flecs::is_pair<First>::value> = 0>    
    Self& set_override(const A& val) {
        this->override<First, Second>();
        return this->set<First, Second>(val);
    }

    /** Set component, mark component for auto-overriding.
     * @see override(flecs::id_t id)
     *
     * @tparam First The first element of the pair.
     * @tparam Second The second element of the pair.
     */    
    template <typename First, typename Second, typename P = pair<First, Second>, 
        typename A = actual_type_t<P>, if_not_t< flecs::is_pair<First>::value> = 0>    
    Self& set_override(A&& val) {
        this->override<First, Second>();
        return this->set<First, Second>(FLECS_FWD(val));
    }

    /** Emplace component, mark component for auto-overriding.
     * @see override(flecs::id_t id)
     *
     * @tparam T The component to emplace and override.
     */    
    template <typename T, typename ... Args>
    Self& emplace_override(Args&&... args) {
        this->override<T>();

        flecs::emplace<T>(this->m_world, this->m_id, 
            _::cpp_type<T>::id(this->m_world), FLECS_FWD(args)...);

        return to_base();  
    }

    /** Emplace pair, mark pair for auto-overriding.
     * @see override(flecs::id_t id)
     *
     * @tparam First The first element of the pair to emplace and override.
     * @tparam Second The second element of the pair to emplace and override.
     */    
    template <typename First, typename Second, typename P = pair<First, Second>, 
        typename A = actual_type_t<P>, if_not_t< flecs::is_pair<First>::value> = 0,
            typename ... Args>
    Self& emplace_override(Args&&... args) {
        this->override<First, Second>();

        flecs::emplace<A>(this->m_world, this->m_id, 
            ecs_pair(_::cpp_type<First>::id(this->m_world),
                _::cpp_type<Second>::id(this->m_world)),
                    FLECS_FWD(args)...);

        return to_base();  
    }

    /** Add pair for enum constant.
     * This operation will add a pair to the entity where the first element is
     * the enumeration type, and the second element the enumeration constant.
     * 
     * The operation may be used with regular (C style) enumerations as well as
     * enum classes.
     * 
     * @param value The enumeration value.
     */
    template <typename E, if_t< is_enum<E>::value > = 0>
    Self& add(E value) {
        flecs::entity_t first = _::cpp_type<E>::id(this->m_world);
        const auto& et = enum_type<E>(this->m_world);
        flecs::entity_t second = et.entity(value);
        return this->add(first, second);
    }

    /** Remove pair for enum.
     * This operation will remove any (Enum, *) pair from the entity.
     * 
     * @tparam E The enumeration type.
     */
    template <typename E, if_t< is_enum<E>::value > = 0>
    Self& remove() {
        flecs::entity_t first = _::cpp_type<E>::id(this->m_world);
        return this->remove(first, flecs::Wildcard);
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

    /** Enable an id.
     * This sets the enabled bit for this component. If this is the first time
     * the component is enabled or disabled, the bitset is added.
     * 
     * @param id The id to enable.
     * @param toggle True to enable, false to disable (default = true).
     */   
    Self& enable(flecs::id_t id, bool toggle = true) {
        ecs_enable_id(this->m_world, this->m_id, id, toggle);
        return to_base();       
    }

    /** Enable a component.
     * @see enable(flecs::id_t id)
     *
     * @tparam T The component to enable.
     */   
    template<typename T>
    Self& enable() {
        return this->enable(_::cpp_type<T>::id());
    }

    /** Enable a pair.
     * @see enable(flecs::id_t id)
     *
     * @param first The first element of the pair.
     * @param second The second element of the pair.
     */   
    Self& enable(flecs::id_t first, flecs::id_t second) {
        return this->enable(ecs_pair(first, second));
    }

    /** Enable a pair.
     * @see enable(flecs::id_t id)
     *
     * @tparam First The first element of the pair.
     * @param second The second element of the pair.
     */   
    template<typename First>
    Self& enable(flecs::id_t second) {
        return this->enable(_::cpp_type<First>::id(), second);
    }

    /** Enable a pair.
     * @see enable(flecs::id_t id)
     *
     * @tparam First The first element of the pair.
     * @tparam Second The second element of the pair.
     */   
    template<typename First, typename Second>
    Self& enable() {
        return this->enable<First>(_::cpp_type<Second>::id());
    }

    /** Disable an id.
     * This sets the enabled bit for this id. If this is the first time
     * the id is enabled or disabled, the bitset is added.
     *
     * @param id The id to disable.
     */   
    Self& disable(flecs::id_t id) {
        return this->enable(id, false);
    }

    /** Disable a component.
     * @see disable(flecs::id_t id)
     *
     * @tparam T The component to enable.
     */   
    template<typename T>
    Self& disable() {
        return this->disable(_::cpp_type<T>::id());
    }

    /** Disable a pair.
     * @see disable(flecs::id_t id)
     *
     * @param first The first element of the pair.
     * @param second The second element of the pair.
     */   
    Self& disable(flecs::id_t first, flecs::id_t second) {
        return this->disable(ecs_pair(first, second));
    }

    /** Disable a pair.
     * @see disable(flecs::id_t id)
     *
     * @tparam First The first element of the pair.
     * @param second The second element of the pair.
     */   
    template<typename First>
    Self& disable(flecs::id_t second) {
        return this->disable(_::cpp_type<First>::id(), second);
    }

    /** Disable a pair.
     * @see disable(flecs::id_t id)
     *
     * @tparam First The first element of the pair.
     * @tparam Second The second element of the pair.
     */   
    template<typename First, typename Second>
    Self& disable() {
        return this->disable<First>(_::cpp_type<Second>::id());
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
     * This operation sets the pair value, and uses First as type. If the
     * entity did not yet have the pair, it will be added.
     *
     * @tparam First The first element of the pair.
     * @tparam Second The second element of the pair
     * @param value The value to set.
     */
    template <typename First, typename Second, typename P = pair<First, Second>, 
        typename A = actual_type_t<P>, if_not_t< flecs::is_pair<First>::value> = 0>
    Self& set(A&& value) {
        flecs::set<P>(this->m_world, this->m_id, FLECS_FWD(value));
        return to_base();
    }

    /** Set a pair for an entity.
     * This operation sets the pair value, and uses First as type. If the
     * entity did not yet have the pair, it will be added.
     *
     * @tparam First The first element of the pair.
     * @tparam Second The second element of the pair
     * @param value The value to set.
     */
    template <typename First, typename Second, typename P = pair<First, Second>, 
        typename A = actual_type_t<P>, if_not_t< flecs::is_pair<First>::value> = 0>
    Self& set(const A& value) {
        flecs::set<P>(this->m_world, this->m_id, value);
        return to_base();
    }

    /** Set a pair for an entity.
     * This operation sets the pair value, and uses First as type. If the
     * entity did not yet have the pair, it will be added.
     *
     * @tparam First The first element of the pair.
     * @param second The second element of the pair.
     * @param value The value to set.
     */
    template <typename First, typename Second, if_not_t< is_enum<Second>::value > = 0>
    Self& set(Second second, const First& value) {
        auto first = _::cpp_type<First>::id(this->m_world);
        flecs::set(this->m_world, this->m_id, value, 
            ecs_pair(first, second));
        return to_base();
    }

    /** Set a pair for an entity.
     * This operation sets the pair value, and uses First as type. If the
     * entity did not yet have the pair, it will be added.
     *
     * @tparam First The first element of the pair.
     * @param second The second element of the pair.
     * @param value The value to set.
     */
    template <typename First, typename Second, if_not_t< is_enum<Second>::value > = 0>
    Self& set(Second second, First&& value) {
        auto first = _::cpp_type<First>::id(this->m_world);
        flecs::set(this->m_world, this->m_id, FLECS_FWD(value), 
            ecs_pair(first, second));
        return to_base();
    }

    /** Set a pair for an entity.
     * This operation sets the pair value, and uses First as type. If the
     * entity did not yet have the pair, it will be added.
     *
     * @tparam First The first element of the pair.
     * @param constant The enum constant.
     * @param value The value to set.
     */
    template <typename First, typename Second, if_t< is_enum<Second>::value > = 0>
    Self& set(Second constant, const First& value) {
        const auto& et = enum_type<Second>(this->m_world);
        flecs::entity_t second = et.entity(constant);
        return set<First>(second, value);
    }

    /** Set a pair for an entity.
     * This operation sets the pair value, and uses Second as type. If the
     * entity did not yet have the pair, it will be added.
     *
     * @tparam Second The second element of the pair
     * @param first The first element of the pair.
     * @param value The value to set.
     */
    template <typename Second>
    Self& set_second(entity_t first, const Second& value) {
        auto second = _::cpp_type<Second>::id(this->m_world);
        flecs::set(this->m_world, this->m_id, value, 
            ecs_pair(first, second));
        return to_base();
    }

    /** Set a pair for an entity.
     * This operation sets the pair value, and uses Second as type. If the
     * entity did not yet have the pair, it will be added.
     *
     * @tparam Second The second element of the pair
     * @param first The first element of the pair.
     * @param value The value to set.
     */
    template <typename Second>
    Self& set_second(entity_t first, Second&& value) {
        auto second = _::cpp_type<Second>::id(this->m_world);
        flecs::set(this->m_world, this->m_id, FLECS_FWD(value), 
            ecs_pair(first, second));
        return to_base();
    }

    template <typename First, typename Second>
    Self& set_second(const Second& value) {
        flecs::set<pair_object<First, Second>>(this->m_world, this->m_id, value);
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
     * destructor on the value passed into the function.
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
    template<typename T, typename ... Args, typename A = actual_type_t<T>>
    Self& emplace(Args&&... args) {
        flecs::emplace<A>(this->m_world, this->m_id, 
            _::cpp_type<T>::id(this->m_world), FLECS_FWD(args)...);
        return to_base();
    }

    template <typename First, typename Second, typename ... Args, typename P = pair<First, Second>, 
        typename A = actual_type_t<P>, if_not_t< flecs::is_pair<First>::value> = 0>
    Self& emplace(Args&&... args) {
        flecs::emplace<A>(this->m_world, this->m_id, 
            ecs_pair(_::cpp_type<First>::id(this->m_world),
                _::cpp_type<Second>::id(this->m_world)),
            FLECS_FWD(args)...);
        return to_base();
    }

    template <typename First, typename ... Args>
    Self& emplace_first(flecs::entity_t second, Args&&... args) {
        flecs::emplace<First>(this->m_world, this->m_id, 
            ecs_pair(_::cpp_type<First>::id(this->m_world), second),
            FLECS_FWD(args)...);
        return to_base();
    }

    template <typename Second, typename ... Args>
    Self& emplace_second(flecs::entity_t first, Args&&... args) {
        flecs::emplace<Second>(this->m_world, this->m_id, 
            ecs_pair(first, _::cpp_type<Second>::id(this->m_world)),
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

    /** Entities created in function will have (First, this).
     * This operation is thread safe.
     *
     * @tparam First The first element of the pair
     * @param func The function to call.
     */
    template <typename First, typename Func>
    Self& with(const Func& func) {
        with(_::cpp_type<First>::id(this->m_world), func);
        return to_base();
    }

    /** Entities created in function will have (first, this).
     *
     * @param first The first element of the pair.
     * @param func The function to call.
     */
    template <typename Func>
    Self& with(entity_t first, const Func& func) {
        ecs_id_t prev = ecs_set_with(this->m_world, 
            ecs_pair(first, this->m_id));
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

    /** Return world scoped to entity */
    scoped_world scope() const {
        return scoped_world(m_world, m_id);
    }

    /* Set the entity name.
     */
    Self& set_name(const char *name) {
        ecs_set_name(this->m_world, this->m_id, name);
        return to_base();
    }

    /* Set entity alias.
     */
    Self& set_alias(const char *name) {
        ecs_set_alias(this->m_world, this->m_id, name);
        return to_base();
    }

#   ifdef FLECS_DOC
/**
 * @file addons/cpp/mixins/doc/entity_builder.inl
 * @brief Doc entity builder mixin.
 */

/** Set doc name.
 * This adds (flecs.doc.Description, flecs.Name) to the entity.
 * 
 * \memberof flecs::entity_builder
 * \ingroup cpp_addons_doc
 */
Self& set_doc_name(const char *name) {
    ecs_doc_set_name(m_world, m_id, name);
    return to_base();
}

/** Set doc brief.
 * This adds (flecs.doc.Description, flecs.doc.Brief) to the entity.
 * 
 * \memberof flecs::entity_builder
 * \ingroup cpp_addons_doc
 */
Self& set_doc_brief(const char *brief) {
    ecs_doc_set_brief(m_world, m_id, brief);
    return to_base();
}

/** Set doc detailed description.
 * This adds (flecs.doc.Description, flecs.doc.Detail) to the entity.
 * 
 * \memberof flecs::entity_builder
 * \ingroup cpp_addons_doc
 */
Self& set_doc_detail(const char *detail) {
    ecs_doc_set_detail(m_world, m_id, detail);
    return to_base();
}

/** Set doc link.
 * This adds (flecs.doc.Description, flecs.doc.Link) to the entity.
 * 
 * \memberof flecs::entity_builder
 * \ingroup cpp_addons_doc
 */
Self& set_doc_link(const char *link) {
    ecs_doc_set_link(m_world, m_id, link);
    return to_base();
}

/** Set doc color.
 * This adds (flecs.doc.Description, flecs.doc.Color) to the entity.
 * 
 * \memberof flecs::entity_builder
 * \ingroup cpp_addons_doc
 */
Self& set_doc_color(const char *link) {
    ecs_doc_set_color(m_world, m_id, link);
    return to_base();
}

#   endif

#   ifdef FLECS_META
/**
 * @file addons/cpp/mixins/meta/entity_builder.inl
 * @brief Meta entity builder mixin.
 */

/**
 * \memberof flecs::entity_view
 * \ingroup cpp_addons_meta
 * 
 * @{
 */

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
    desc.entity = this->m_id;
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
    desc.entity = this->m_id;
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
    desc.entity = this->m_id;
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

/** @} */

#   endif

protected:
    Self& to_base() {
        return *static_cast<Self*>(this);
    }
};

}


/**
 * @defgroup cpp_entities Entities
 * @brief Entity operations.
 * 
 * \ingroup cpp_core
 * @{
 */

namespace flecs
{

/** Entity.
 * Class with read/write operations for entities.
 * 
 * \ingroup cpp_entities
*/
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
     * @return Pointer to the component value.
     */
    template <typename T>
    T* get_mut() const {
        auto comp_id = _::cpp_type<T>::id(m_world);
        ecs_assert(_::cpp_type<T>::size() != 0, ECS_INVALID_PARAMETER, NULL);
        return static_cast<T*>(ecs_get_mut_id(m_world, m_id, comp_id));
    }

    /** Get mutable component value (untyped).
     * This operation returns a mutable pointer to the component. If the entity
     * did not yet have the component, it will be added. If a base entity had
     * the component, it will be overridden, and the value of the base component
     * will be copied to the entity before this function returns.
     *
     * @param comp The component to get.
     * @return Pointer to the component value.
     */
    void* get_mut(entity_t comp) const {
        return ecs_get_mut_id(m_world, m_id, comp);
    }

    /** Get mutable pointer for a pair.
     * This operation gets the value for a pair from the entity.
     *
     * @tparam First The first part of the pair.
     * @tparam Second the second part of the pair.
     */
    template <typename First, typename Second, typename P = pair<First, Second>, 
        typename A = actual_type_t<P>, if_not_t< flecs::is_pair<First>::value> = 0>
    A* get_mut() const {
        return static_cast<A*>(ecs_get_mut_id(m_world, m_id, ecs_pair(
            _::cpp_type<First>::id(m_world),
            _::cpp_type<Second>::id(m_world))));
    }

    /** Get mutable pointer for a pair.
     * This operation gets the value for a pair from the entity.
     *
     * @tparam First The first part of the pair.
     * @param second The second element of the pair.
     */
    template <typename First>
    First* get_mut(entity_t second) const {
        auto comp_id = _::cpp_type<First>::id(m_world);
        ecs_assert(_::cpp_type<First>::size() != 0, ECS_INVALID_PARAMETER, NULL);
        return static_cast<First*>(
            ecs_get_mut_id(m_world, m_id, ecs_pair(comp_id, second)));
    }

    /** Get mutable pointer for a pair (untyped).
     * This operation gets the value for a pair from the entity. If neither the
     * first nor second element of the pair is a component, the operation will 
     * fail.
     *
     * @param first The first element of the pair.
     * @param second The second element of the pair.
     */
    void* get_mut(entity_t first, entity_t second) const {
        return ecs_get_mut_id(m_world, m_id, ecs_pair(first, second));
    }

    /** Get mutable pointer for the second element of a pair.
     * This operation gets the value for a pair from the entity.
     *
     * @tparam Second The second element of the pair.
     * @param first The first element of the pair.
     */
    template <typename Second>
    Second* get_mut_second(entity_t first) const {
        auto second = _::cpp_type<Second>::id(m_world);
        ecs_assert(_::cpp_type<Second>::size() != 0, ECS_INVALID_PARAMETER, NULL);
        return static_cast<Second*>(
            ecs_get_mut_id(m_world, m_id, ecs_pair(first, second)));
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

    /** Signal that the first element of a pair was modified.
     *
     * @tparam First The first part of the pair.
     * @tparam Second the second part of the pair.
     */
    template <typename First, typename Second>
    void modified() const {
        this->modified<First>(_::cpp_type<Second>::id(m_world));
    }

    /** Signal that the first part of a pair was modified.
     *
     * @tparam First The first part of the pair.
     * @param second The second element of the pair.
     */
    template <typename First>
    void modified(entity_t second) const {
        auto first = _::cpp_type<First>::id(m_world);
        ecs_assert(_::cpp_type<First>::size() != 0, ECS_INVALID_PARAMETER, NULL);
        this->modified(first, second);
    }

    /** Signal that a pair has modified (untyped).
     * If neither the first or second element of the pair are a component, the
     * operation will fail.
     *
     * @param first The first element of the pair.
     * @param second The second element of the pair.
     */
    void modified(entity_t first, entity_t second) const {
        this->modified(ecs_pair(first, second));
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
        return ref<T>(m_world, m_id);
    }

    template <typename First, typename Second, typename P = flecs::pair<First, Second>, 
        typename A = actual_type_t<P>>
    ref<A> get_ref() const {
        // Ensure component is registered
        _::cpp_type<A>::id(m_world);
        return ref<A>(m_world, m_id, 
            ecs_pair(_::cpp_type<First>::id(m_world),
                _::cpp_type<Second>::id(m_world)));
    }

    template <typename First>
    ref<First> get_ref(flecs::entity_t second) const {
        // Ensure component is registered
        _::cpp_type<First>::id(m_world);
        return ref<First>(m_world, m_id, 
            ecs_pair(_::cpp_type<First>::id(m_world), second));
    }

    template <typename Second>
    ref<Second> get_ref_second(flecs::entity_t first) const {
        // Ensure component is registered
        _::cpp_type<Second>::id(m_world);
        return ref<Second>(m_world, m_id, 
            ecs_pair(first, _::cpp_type<Second>::id(m_world)));
    }

    /** Recursively flatten relationship.
     * @see ecs_flatten
     */
    void flatten(flecs::entity_t r, const ecs_flatten_desc_t *desc = nullptr) {
        ecs_flatten(m_world, ecs_pair(r, m_id), desc);
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
     * entity object goes out of scope.
     */
    void destruct() const {
        ecs_delete(m_world, m_id);
    }

    /** Return entity as entity_view.
     * This returns an entity_view instance for the entity which is a readonly
     * version of the entity class.
     * 
     * This is similar to a regular upcast, except that this method ensures that
     * the entity_view instance is instantiated with a world vs. a stage, which
     * a regular upcast does not guarantee.
     */
    flecs::entity_view view() const {
        return flecs::entity_view(
            const_cast<flecs::world_t*>(ecs_get_world(m_world)), m_id);
    }

    /** Entity id 0.
     * This function is useful when the API must provide an entity that
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

#   ifdef FLECS_JSON

/** Deserialize entity to JSON.
 * 
 * \memberof flecs::entity
 * \ingroup cpp_addons_json
 */
const char* from_json(const char *json) {
    return ecs_entity_from_json(m_world, m_id, json, nullptr);
}


#   endif
};

} // namespace flecs

/** @} */

/**
 * @file addons/cpp/invoker.hpp
 * @brief Utilities for invoking each/iter callbacks.
 */

#pragma once

namespace flecs
{

namespace _ 
{

// Binding ctx for component hooks
struct component_binding_ctx {
    void *on_add = nullptr;
    void *on_remove = nullptr;
    void *on_set = nullptr;
    ecs_ctx_free_t free_on_add = nullptr;
    ecs_ctx_free_t free_on_remove = nullptr;
    ecs_ctx_free_t free_on_set = nullptr;

    ~component_binding_ctx() {
        if (on_add && free_on_add) {
            free_on_add(on_add);
        }
        if (on_remove && free_on_remove) {
            free_on_remove(on_remove);
        }
        if (on_set && free_on_set) {
            free_on_set(on_set);
        }
    }
};

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
        bool is_ref = iter->sources && iter->sources[index] != 0;
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

    template < if_not_t< is_same< decay_t<Func>, decay_t<Func>& >::value > = 0>
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

    // Static function to call for component on_add hook
    static void run_add(ecs_iter_t *iter) {
        component_binding_ctx *ctx = reinterpret_cast<component_binding_ctx*>(
            iter->binding_ctx);
        iter->binding_ctx = ctx->on_add;
        run(iter);
    }

    // Static function to call for component on_remove hook
    static void run_remove(ecs_iter_t *iter) {
        component_binding_ctx *ctx = reinterpret_cast<component_binding_ctx*>(
            iter->binding_ctx);
        iter->binding_ctx = ctx->on_remove;
        run(iter);
    }

    // Static function to call for component on_set hook
    static void run_set(ecs_iter_t *iter) {
        component_binding_ctx *ctx = reinterpret_cast<component_binding_ctx*>(
            iter->binding_ctx);
        iter->binding_ctx = ctx->on_set;
        run(iter);
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

        ecs_assert(count > 0, ECS_INVALID_OPERATION,
            "no entities returned, use each() without flecs::entity argument");

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
        size_t count = static_cast<size_t>(iter->count);
        if (count == 0) {
            // If query has no This terms, count can be 0. Since each does not
            // have an entity parameter, just pass through components
            count = 1;
        }

        flecs::iter it(iter);

        ECS_TABLE_LOCK(iter->world, iter->table);

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
        size_t count = static_cast<size_t>(iter->count);
        if (count == 0) {
            // If query has no This terms, count can be 0. Since each does not
            // have an entity parameter, just pass through components
            count = 1;
        }

        flecs::iter it(iter);

        ECS_TABLE_LOCK(iter->world, iter->table);

        for (size_t i = 0; i < count; i ++) {
            func( (ColumnType< remove_reference_t<Components> >(comps, i)
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
    template < if_not_t< is_same< decay_t<Func>, decay_t<Func>& >::value > = 0>
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
    using ArrayType = flecs::array<void*, sizeof...(Args)>;
    using DummyArray = flecs::array<int, sizeof...(Args)>;
    using IdArray = flecs::array<id_t, sizeof...(Args)>;

    static bool const_args() {
        static flecs::array<bool, sizeof...(Args)> is_const_args ({
            flecs::is_const<flecs::remove_reference_t<Args>>::value...
        });

        for (auto is_const : is_const_args) {
            if (!is_const) {
                return false;
            }
        }
        return true;
    }

    static bool get_ptrs(world_t *world, const ecs_record_t *r, ecs_table_t *table,
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

    static bool get_mut_ptrs(world_t *world, ecs_entity_t e, ArrayType& ptrs) {
        /* Get pointers w/get_mut */
        size_t i = 0;
        DummyArray dummy ({
            (ptrs[i ++] = ecs_get_mut_id(world, e, 
                _::cpp_type<Args>().id(world)), 0)...
        });

        return true;
    }    

    template <typename Func>
    static bool invoke_read(world_t *world, entity_t e, const Func& func) {
        const ecs_record_t *r = ecs_read_begin(world, e);
        if (!r) {
            return false;
        }

        ecs_table_t *table = r->table;
        if (!table) {
            return false;
        }

        ArrayType ptrs;
        bool has_components;
        if ((has_components = get_ptrs(world, r, table, ptrs))) {
            invoke_callback(func, 0, ptrs);
        }

        ecs_read_end(r);

        return has_components;
    }

    template <typename Func>
    static bool invoke_write(world_t *world, entity_t e, const Func& func) {
        ecs_record_t *r = ecs_write_begin(world, e);
        if (!r) {
            return false;
        }

        ecs_table_t *table = r->table;
        if (!table) {
            return false;
        }

        ArrayType ptrs;
        bool has_components;
        if ((has_components = get_ptrs(world, r, table, ptrs))) {
            invoke_callback(func, 0, ptrs);
        }

        ecs_write_end(r);

        return has_components;
    }

    template <typename Func>
    static bool invoke_get(world_t *world, entity_t e, const Func& func) {
        if (const_args()) {
            return invoke_read(world, e, func);
        } else {
            return invoke_write(world, e, func);
        }
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

        ArrayType ptrs;
        ecs_table_t *table = NULL;

        // When not deferred take the fast path.
        if (!w.is_deferred()) {
            // Bit of low level code so we only do at most one table move & one
            // entity lookup for the entire operation.

            // Make sure the object is not a stage. Operations on a stage are
            // only allowed when the stage is in deferred mode, which is when
            // the world is in readonly mode.
            ecs_assert(!w.is_stage(), ECS_INVALID_PARAMETER, NULL);

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
    template <typename Func, typename ... TArgs, 
        if_t<sizeof...(TArgs) == sizeof...(Args)> = 0>
    static void invoke_callback(
        const Func& f, size_t, ArrayType&, TArgs&& ... comps) 
    {
        f(*static_cast<typename base_arg_type<Args>::type*>(comps)...);
    }

    template <typename Func, typename ... TArgs, 
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

/**
 * @file addons/cpp/utils/iterable.hpp
 * @brief Base class for iterable objects, like queries.
 */

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
     *  - func(flecs::iter& it, size_t index, Components& ....)
     *  - func(Components& ...)
     * 
     * Each iterators are automatically instanced.
     */
    template <typename Func>
    void each(Func&& func) const {
        each(nullptr, FLECS_FWD(func));
    }

    template <typename Func>
    void each(flecs::world_t *world, Func&& func) const {
        iterate<_::each_invoker>(world, FLECS_FWD(func), 
            this->next_each_action());
    }

    template <typename Func>
    void each(flecs::iter& it, Func&& func) const {
        iterate<_::each_invoker>(it.world(), FLECS_FWD(func),
            this->next_each_action());
    }

    template <typename Func>
    void each(flecs::entity e, Func&& func) const {
        iterate<_::each_invoker>(e.world(), FLECS_FWD(func), 
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
        iterate<_::iter_invoker>(nullptr, FLECS_FWD(func), 
            this->next_action());
    }

    template <typename Func>
    void iter(flecs::world_t *world, Func&& func) const {
        iterate<_::iter_invoker>(world, FLECS_FWD(func), 
            this->next_action());
    }

    template <typename Func>
    void iter(flecs::iter& it, Func&& func) const {
        iterate<_::iter_invoker>(it.world(), FLECS_FWD(func),
            this->next_action());
    }

    template <typename Func>
    void iter(flecs::entity e, Func&& func) const {
        iterate<_::iter_invoker>(e.world(), FLECS_FWD(func), 
            this->next_action());
    }

    /** Create iterator.
     * Create an iterator object that can be modified before iterating.
     */
    iter_iterable<Components...> iter(flecs::world_t *world = nullptr) const;

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

    /** Return number of entities matched by iterable. */
    int32_t count() const {
        return this->iter().count();
    }

    /** Return whether iterable has any matches. */
    bool is_true() const {
        return this->iter().is_true();
    }

    /** Return first entity matched by iterable. */
    flecs::entity first() const {
        return this->iter().first();
    }

    virtual ~iterable() { }
protected:
    friend iter_iterable<Components...>;
    friend page_iterable<Components...>;
    friend worker_iterable<Components...>;

    virtual ecs_iter_t get_iter(flecs::world_t *stage) const = 0;
    virtual ecs_iter_next_action_t next_action() const = 0;
    virtual ecs_iter_next_action_t next_each_action() const = 0;

    template < template<typename Func, typename ... Comps> class Invoker, typename Func, typename NextFunc, typename ... Args>
    void iterate(flecs::world_t *stage, Func&& func, NextFunc next, Args &&... args) const {
        ecs_iter_t it = this->get_iter(stage);
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
    iter_iterable(Iterable *it, flecs::world_t *world) 
    {
        m_it = it->get_iter(world);
        m_next = it->next_action();
        m_next_each = it->next_action();
    }

    iter_iterable<Components...>& set_var(int var_id, flecs::entity_t value) {
        ecs_assert(var_id != -1, ECS_INVALID_PARAMETER, 0);
        ecs_iter_set_var(&m_it, var_id, value);
        return *this;
    }

#   ifdef FLECS_RULES
/**
 * @file addons/cpp/mixins/rule/iterable.inl
 * @brief Rule iterable mixin.
 */

/**
 * \memberof flecs::iter
 * \ingroup cpp_addons_rules
 */

iter_iterable<Components...>& set_var(const char *name, flecs::entity_t value) {
    ecs_assert(m_it.next == ecs_rule_next, ECS_INVALID_OPERATION, NULL);
    ecs_rule_iter_t *rit = &m_it.priv.iter.rule;
    int var_id = ecs_rule_find_var(rit->rule, name);
    ecs_assert(var_id != -1, ECS_INVALID_PARAMETER, name);
    ecs_iter_set_var(&m_it, var_id, value);
    return *this;
}

/** @} */

#   endif
#   ifdef FLECS_JSON
/**
 * @file addons/cpp/mixins/json/iterable.inl
 * @brief JSON iterable mixin.
 */

/** Serialize iterator result to JSON.
 * 
 * \memberof flecs::iter
 * \ingroup cpp_addons_json
 */
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

    // Return first matching entity.
    flecs::entity first() {
        flecs::entity result;
        if (m_next_each(&m_it) && m_it.count) {
            result = flecs::entity(m_it.world, m_it.entities[0]);
            ecs_iter_fini(&m_it);
        }
        return result;
    }

    // Limit results to tables with specified group id (grouped queries only)
    iter_iterable<Components...>& set_group(uint64_t group_id) {
        ecs_query_set_group(&m_it, group_id);
        return *this;
    }

    // Limit results to tables with specified group id (grouped queries only)
    template <typename Group>
    iter_iterable<Components...>& set_group() {
        ecs_query_set_group(&m_it, _::cpp_type<Group>().id(m_it.real_world));
        return *this;
    }

protected:
    ecs_iter_t get_iter(flecs::world_t *world) const {
        if (world) {
            ecs_iter_t result = m_it;
            result.world = world;
            return result;
        }
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
iter_iterable<Components...> iterable<Components...>::iter(flecs::world_t *world) const
{
    return iter_iterable<Components...>(this, world);
}

template <typename ... Components>
struct page_iterable final : iterable<Components...> {
    template <typename Iterable>
    page_iterable(int32_t offset, int32_t limit, Iterable *it) 
        : m_offset(offset)
        , m_limit(limit)
    {
        m_chain_it = it->get_iter(nullptr);
    }

protected:
    ecs_iter_t get_iter(flecs::world_t*) const {
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
        m_chain_it = it->get_iter(nullptr);
    }

protected:
    ecs_iter_t get_iter(flecs::world_t*) const {
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

/**
 * @file addons/cpp/component.hpp
 * @brief Registering/obtaining info from components.
 */

#pragma once

#include <ctype.h>
#include <stdio.h>

/**
 * @defgroup cpp_components Components
 * @brief Registering and working with components.
 * 
 * \ingroup cpp_core
 * @{
 */

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
    static const size_t front_len = ECS_FUNC_NAME_FRONT(const char*, type_name);
    return ecs_cpp_get_type_name(result, ECS_FUNC_NAME, len, front_len);
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
    ecs_type_hooks_t cl{};
    cl.ctor = ctor<T>();
    cl.dtor = dtor<T>();

    cl.copy = copy<T>();
    cl.copy_ctor = copy_ctor<T>();
    cl.move = move<T>();
    cl.move_ctor = move_ctor<T>();

    cl.ctor_move_dtor = ctor_move_dtor<T>();
    cl.move_dtor = move_dtor<T>();

    ecs_set_hooks_id( world, component, &cl);
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
        entity_t entity, 
        bool allow_tag = true) 
    {
        if (s_reset_count != ecs_cpp_reset_count_get()) {
            reset();
        }

        // If an identifier was already set, check for consistency
        if (s_id) {
            ecs_assert(s_id == entity, ECS_INCONSISTENT_COMPONENT_ID, 
                type_name<T>());
            ecs_assert(allow_tag == s_allow_tag, ECS_INVALID_PARAMETER, NULL);

            // Component was already registered and data is consistent with new
            // identifier, so nothing else to be done.
            return;
        }

        // Component wasn't registered yet, set the values. Register component
        // name as the fully qualified flecs path.
        s_id = entity;
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
        bool is_component = true, bool *existing = nullptr)
    {
        if (!s_id) {
            // If no world was provided the component cannot be registered
            ecs_assert(world != nullptr, ECS_COMPONENT_NOT_REGISTERED, name);
        } else {
            ecs_assert(!id || s_id == id, ECS_INCONSISTENT_COMPONENT_ID, NULL);
        }

        // If no id has been registered yet for the component (indicating the 
        // component has not yet been registered, or the component is used
        // across more than one binary), or if the id does not exists in the 
        // world (indicating a multi-world application), register it. */
        if (!s_id || (world && !ecs_exists(world, s_id))) {
            init(s_id ? s_id : id, allow_tag);

            ecs_assert(!id || s_id == id, ECS_INTERNAL_ERROR, NULL);

            const char *symbol = nullptr;
            if (id) {
                symbol = ecs_get_symbol(world, id);
            }
            if (!symbol) {
                symbol = symbol_name<T>();
            }

            entity_t entity = ecs_cpp_component_register_explicit(
                    world, s_id, id, name, type_name<T>(), symbol, 
                        s_size, s_alignment, is_component, existing);

            s_id = entity;

            // If component is enum type, register constants
            #if FLECS_CPP_ENUM_REFLECTION_SUPPORT            
            _::init_enum<T>(world, entity);
            #endif
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
        if (!registered(world)) {
            ecs_entity_t prev_scope = 0;
            ecs_id_t prev_with = 0;

            if (world) {
                prev_scope = ecs_set_scope(world, 0);
                prev_with = ecs_set_with(world, 0);
            }

            // This will register a component id, but will not register 
            // lifecycle callbacks.
            bool existing;
            id_explicit(world, name, allow_tag, 0, true, &existing);

            // Register lifecycle callbacks, but only if the component has a
            // size. Components that don't have a size are tags, and tags don't
            // require construction/destruction/copy/move's. */
            if (size() && !existing) {
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
    static bool registered(flecs::world_t *world) {
        if (s_reset_count != ecs_cpp_reset_count_get()) {
            reset();
        }
        if (s_id == 0) {
            return false;
        }
        if (world && !ecs_exists(world, s_id)) {
            return false;
        }
        return true;
    }

    // This function is only used to test cross-translation unit features. No
    // code other than test cases should invoke this function.
    static void reset() {
        s_id = 0;
        s_size = 0;
        s_alignment = 0;
        s_allow_tag = true;
    }

    static entity_t s_id;
    static size_t s_size;
    static size_t s_alignment;
    static bool s_allow_tag;
    static int32_t s_reset_count;
};

// Global templated variables that hold component identifier and other info
template <typename T> entity_t      cpp_type_impl<T>::s_id;
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
            cpp_type< pair_first_t<T> >::id(world),
            cpp_type< pair_second_t<T> >::id(world));
    }
};

} // namespace _

/** Untyped component class.
 * Generic base class for flecs::component.
 * 
 * \ingroup cpp_components
 */
struct untyped_component : entity {
    using entity::entity;
    
#   ifdef FLECS_META
/**
 * @file addons/cpp/mixins/meta/untyped_component.inl
 * @brief Meta component mixin.
 */

/**
 * \memberof flecs::component
 * \ingroup cpp_addons_meta
 * 
 * @{
 */

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

/** Add member value range */
untyped_component& range(double min, double max) {
    const flecs::member_t *m = ecs_cpp_last_member(m_world, m_id);
    if (!m) {
        return *this;
    }

    flecs::world w(m_world);
    flecs::entity me = w.entity(m->member);
    flecs::MemberRanges *mr = me.get_mut<flecs::MemberRanges>();
    mr->value.min = min;
    mr->value.max = max;
    me.modified<flecs::MemberRanges>();
    return *this;
}

/** Add member warning range */
untyped_component& warning_range(double min, double max) {
    const flecs::member_t *m = ecs_cpp_last_member(m_world, m_id);
    if (!m) {
        return *this;
    }

    flecs::world w(m_world);
    flecs::entity me = w.entity(m->member);
    flecs::MemberRanges *mr = me.get_mut<flecs::MemberRanges>();
    mr->warning.min = min;
    mr->warning.max = max;
    me.modified<flecs::MemberRanges>();
    return *this;
}

/** Add member error range */
untyped_component& error_range(double min, double max) {
    const flecs::member_t *m = ecs_cpp_last_member(m_world, m_id);
    if (!m) {
        return *this;
    }

    flecs::world w(m_world);
    flecs::entity me = w.entity(m->member);
    flecs::MemberRanges *mr = me.get_mut<flecs::MemberRanges>();
    mr->error.min = min;
    mr->error.max = max;
    me.modified<flecs::MemberRanges>();
    return *this;
}


/** @} */

#   endif
#   ifdef FLECS_METRICS
/**
 * @file addons/cpp/mixins/meta/untyped_component.inl
 * @brief Metrics component mixin.
 */

/**
 * \memberof flecs::component
 * \ingroup cpp_addons_metrics
 * 
 * @{
 */

/** Register member as metric.
 * When no explicit name is provided, this operation will derive the metric name
 * from the member name. When the member name is "value", the operation will use
 * the name of the component.
 * 
 * When the brief parameter is provided, it is set on the metric as if 
 * set_doc_brief is used. The brief description can be obtained with 
 * get_doc_brief.
 * 
 * @tparam Kind Metric kind (Counter, CounterIncrement or Gauge).
 * @param parent Parent entity of the metric (optional).
 * @param brief Description for metric (optional).
 * @param name Name of metric (optional).
 * 
 * \ingroup cpp_addons_metrics
 * \memberof flecs::world
 */
template <typename Kind>
untyped_component& metric(
    flecs::entity_t parent = 0, 
    const char *brief = nullptr, 
    const char *name = nullptr);

/** @} */

#   endif
};

/** Component class.
 * Class used to register components and component metadata.
 * 
 * \ingroup cpp_components
 */
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

        if (_::cpp_type<T>::registered(world)) {
            /* Obtain component id. Because the component is already registered,
             * this operation does nothing besides returning the existing id */
            id = _::cpp_type<T>::id_explicit(world, name, allow_tag, id);

            ecs_cpp_component_validate(world, id, n, _::symbol_name<T>(),
                _::cpp_type<T>::size(),
                _::cpp_type<T>::alignment(),
                implicit_name);
        } else {
            /* If component is registered from an existing scope, ignore the
             * namespace in the name of the component. */
            if (implicit_name && (ecs_get_scope(world) != 0)) {
                /* If the type is a template type, make sure to ignore ':'
                 * inside the template parameter list. */
                const char *start = strchr(n, '<'), *last_elem = NULL;
                if (start) {
                    const char *ptr = start;
                    while (ptr[0] && (ptr[0] != ':') && (ptr > n)) {
                        ptr --;
                    }
                    if (ptr[0] == ':') {
                        last_elem = ptr;
                    }
                } else {
                    last_elem = strrchr(n, ':');
                }
                if (last_elem) {
                    name = last_elem + 1;
                }
            }

            /* Find or register component */
            bool existing;
            id = ecs_cpp_component_register(world, id, n, _::symbol_name<T>(),
                ECS_SIZEOF(T), ECS_ALIGNOF(T), implicit_name, &existing);

            /* Initialize static component data */
            id = _::cpp_type<T>::id_explicit(world, name, allow_tag, id);

            /* Initialize lifecycle actions (ctor, dtor, copy, move) */
            if (_::cpp_type<T>::size() && !existing) {
                _::register_lifecycle_actions<T>(world, id);
            }
        }

        m_world = world;
        m_id = id;
    }

    /** Register on_add hook. */
    template <typename Func>
    component<T>& on_add(Func&& func) {
        using Invoker = typename _::each_invoker<
            typename std::decay<Func>::type, T>;
        flecs::type_hooks_t h = get_hooks();
        ecs_assert(h.on_add == nullptr, ECS_INVALID_OPERATION, 
            "on_add hook is already set");
        BindingCtx *ctx = get_binding_ctx(h);
        h.on_add = Invoker::run_add;
        ctx->on_add = FLECS_NEW(Invoker)(FLECS_FWD(func));
        ctx->free_on_add = reinterpret_cast<ecs_ctx_free_t>(
            _::free_obj<Invoker>);
        ecs_set_hooks_id(m_world, m_id, &h);
        return *this;
    }

    /** Register on_remove hook. */
    template <typename Func>
    component<T>& on_remove(Func&& func) {
        using Invoker = typename _::each_invoker<
            typename std::decay<Func>::type, T>;
        flecs::type_hooks_t h = get_hooks();
        ecs_assert(h.on_remove == nullptr, ECS_INVALID_OPERATION, 
            "on_remove hook is already set");
        BindingCtx *ctx = get_binding_ctx(h);
        h.on_remove = Invoker::run_remove;
        ctx->on_remove = FLECS_NEW(Invoker)(FLECS_FWD(func));
        ctx->free_on_remove = reinterpret_cast<ecs_ctx_free_t>(
            _::free_obj<Invoker>);
        ecs_set_hooks_id(m_world, m_id, &h);
        return *this;
    }

    /** Register on_set hook. */
    template <typename Func>
    component<T>& on_set(Func&& func) {
        using Invoker = typename _::each_invoker<
            typename std::decay<Func>::type, T>;
        flecs::type_hooks_t h = get_hooks();
        ecs_assert(h.on_set == nullptr, ECS_INVALID_OPERATION, 
            "on_set hook is already set");
        BindingCtx *ctx = get_binding_ctx(h);
        h.on_set = Invoker::run_set;
        ctx->on_set = FLECS_NEW(Invoker)(FLECS_FWD(func));
        ctx->free_on_set = reinterpret_cast<ecs_ctx_free_t>(
            _::free_obj<Invoker>);
        ecs_set_hooks_id(m_world, m_id, &h);
        return *this;
    }

#   ifdef FLECS_META

/** Register opaque type interface */
template <typename Func>
component& opaque(const Func& type_support) {
    flecs::world world(m_world);
    auto ts = type_support(world);
    ts.desc.entity = _::cpp_type<T>::id(m_world);
    ecs_opaque_init(m_world, &ts.desc);
    return *this;
}

flecs::opaque<T> opaque(flecs::entity_t as_type) {
    return flecs::opaque<T>(m_world).as_type(as_type);
}

flecs::opaque<T> opaque(flecs::entity as_type) {
    return this->opaque(as_type.id());
}

flecs::opaque<T> opaque(flecs::untyped_component as_type) {
    return this->opaque(as_type.id());
}

/** Return opaque type builder for collection type */
template <typename ElemType>
flecs::opaque<T, ElemType> opaque(flecs::id_t as_type) {
    return flecs::opaque<T, ElemType>(m_world).as_type(as_type);
}

/** Add constant. */
component<T>& constant(const char *name, T value) {
    int32_t v = static_cast<int32_t>(value);
    untyped_component::constant(name, v);
    return *this;
}

#   endif

private:
    using BindingCtx = _::component_binding_ctx;

    BindingCtx* get_binding_ctx(flecs::type_hooks_t& h){        
        BindingCtx *result = static_cast<BindingCtx*>(h.binding_ctx);
        if (!result) {
            result = FLECS_NEW(BindingCtx);
            h.binding_ctx = result;
            h.binding_ctx_free = reinterpret_cast<ecs_ctx_free_t>(
                _::free_obj<BindingCtx>);
        }
        return result;
    }

    flecs::type_hooks_t get_hooks() {
        const flecs::type_hooks_t* h = ecs_get_hooks_id(m_world, m_id);
        if (h) {
            return *h;
        } else {
            return {};
        }
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

/** Reset static component ids.
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
 * 
 * \ingroup cpp_components
 */
inline void reset() {
    ecs_cpp_reset_count_inc();
}

}

/** @} */

/**
 * @file addons/cpp/type.hpp
 * @brief Utility functions for id vector.
 */

#pragma once

namespace flecs {

/**
 * @defgroup cpp_types Types
 * @brief Type operations.
 * 
 * \ingroup cpp_core
 * @{
 */

/** Type class.
 * A type is a vector of component ids which can be requested from entities or tables.
 */
struct type {
    type() : m_world(nullptr), m_type(nullptr) { }

    type(world_t *world, const type_t *t)
        : m_world(world)
        , m_type(t) { }

    /** Convert type to comma-separated string */
    flecs::string str() const {
        return flecs::string(ecs_type_str(m_world, m_type));
    }

    /** Return number of ids in type */
    int32_t count() const {
        if (!m_type) {
            return 0;
        }
        return m_type->count;
    }

    /** Return pointer to array. */
    flecs::id_t* array() const {
        if (!m_type) {
            return nullptr;
        }
        return m_type->array;
    }

    /** Get id at specified index in type */
    flecs::id get(int32_t index) const {
        ecs_assert(m_type != NULL, ECS_INVALID_PARAMETER, NULL);
        ecs_assert(m_type->count > index, ECS_OUT_OF_RANGE, NULL);
        if (!m_type) {
            return flecs::id();
        }
        return flecs::id(m_world, m_type->array[index]);
    }
    
    flecs::id_t* begin() const {
        return m_type->array;
    }

    flecs::id_t* end() const {
        return &m_type->array[m_type->count];
    }

    /** Implicit conversion to type_t */
    operator const type_t*() const {
        return m_type;
    }
private:
    world_t *m_world;
    const type_t *m_type;
};

/** #} */

}

/**
 * @file addons/cpp/table.hpp
 * @brief Direct access to table data.
 */

#pragma once

namespace flecs {

/**
 * @defgroup cpp_tables Tables
 * @brief Table operations.
 * 
 * \ingroup cpp_core
 * @{
 */

struct table {
    table() : m_world(nullptr), m_table(nullptr) { }

    table(world_t *world, table_t *t)
        : m_world(world)
        , m_table(t) { }

    virtual ~table() { }

    /** Convert table type to string. */
    flecs::string str() const {
        return flecs::string(ecs_table_str(m_world, m_table));
    }

    /** Get table type. */
    flecs::type type() const {
        return flecs::type(m_world, ecs_table_get_type(m_table));
    }

    /** Get table count. */
    int32_t count() const {
        return ecs_table_count(m_table);
    }

    /** Find index for (component) id. 
     * 
     * @param id The (component) id.
     * @return The index of the id in the table type, -1 if not found/
     */
    int32_t search(flecs::id_t id) const {
        return ecs_search(m_world, m_table, id, 0);
    }

    /** Find index for type. 
     * 
     * @tparam T The type.
     * @return True if the table has the type, false if not.
     */
    template <typename T>
    int32_t search() const {
        return search(_::cpp_type<T>::id(m_world));
    }

    /** Find index for pair. 
     * @param first First element of pair.
     * @param second Second element of pair.
     * @return True if the table has the pair, false if not.
     */
    int32_t search(flecs::entity_t first, flecs::entity_t second) const {
        return search(ecs_pair(first, second));
    }

    /** Find index for pair. 
     * @tparam First First element of pair.
     * @param second Second element of pair.
     * @return True if the table has the pair, false if not.
     */
    template <typename First>
    int32_t search(flecs::entity_t second) const {
        return search(_::cpp_type<First>::id(m_world), second);
    }

    /** Find index for pair. 
     * @tparam First First element of pair.
     * @tparam Second Second element of pair.
     * @return True if the table has the pair, false if not.
     */
    template <typename First, typename Second>
    int32_t search() const {
        return search<First>(_::cpp_type<Second>::id(m_world));
    }

    /** Test if table has (component) id. 
     * 
     * @param id The (component) id.
     * @return True if the table has the id, false if not.
     */
    bool has(flecs::id_t id) const {
        return search(id) != -1;
    }

    /** Test if table has the type. 
     * 
     * @tparam T The type.
     * @return True if the table has the type, false if not.
     */
    template <typename T>
    bool has() const {
        return search<T>() != -1;
    }

    /** Test if table has the pair.
     * 
     * @param first First element of pair.
     * @param second Second element of pair.
     * @return True if the table has the pair, false if not.
     */
    bool has(flecs::entity_t first, flecs::entity_t second) const {
        return search(first, second) != -1;
    }

    /** Test if table has the pair.
     * 
     * @tparam First First element of pair.
     * @param second Second element of pair.
     * @return True if the table has the pair, false if not.
     */
    template <typename First>
    bool has(flecs::entity_t second) const {
        return search<First>(second) != -1;
    }

    /** Test if table has the pair.
     * 
     * @tparam First First element of pair.
     * @tparam Second Second element of pair.
     * @return True if the table has the pair, false if not.
     */
    template <typename First, typename Second>
    bool has() const {
        return search<First, Second>() != -1;
    }

    /** Get pointer to component array by column index. 
     * 
     * @param index The column index.
     * @return Pointer to the column, NULL if not a component.
     */
    virtual void* get_by_index(int32_t index) const {
        return ecs_table_get_column(m_table, index, 0);
    }

    /** Get pointer to component array by component.
     * 
     * @param id The component id.
     * @return Pointer to the column, NULL if not found.
     */
    void* get(flecs::id_t id) const {
        int32_t index = search(id);
        if (index == -1) {
            return NULL;
        }
        return get_by_index(index);
    }

    /** Get pointer to component array by pair.
     * 
     * @param first The first element of the pair.
     * @param second The second element of the pair.
     * @return Pointer to the column, NULL if not found.
     */
    void* get(flecs::entity_t first, flecs::entity_t second) const {
        return get(ecs_pair(first, second));
    }

    /** Get pointer to component array by component.
     * 
     * @tparam T The component.
     * @return Pointer to the column, NULL if not found.
     */
    template <typename T, if_t< is_actual<T>::value > = 0>
    T* get() const {
        return static_cast<T*>(get(_::cpp_type<T>::id(m_world)));
    }

    /** Get pointer to component array by component.
     * 
     * @tparam T The component.
     * @return Pointer to the column, NULL if not found.
     */
    template <typename T, typename A = actual_type_t<T>,
        if_t< flecs::is_pair<T>::value > = 0>
    A* get() const {
        return static_cast<A*>(get(_::cpp_type<T>::id(m_world)));
    }

    /** Get pointer to component array by pair.
     * 
     * @tparam First The first element of the pair.
     * @param second The second element of the pair.
     * @return Pointer to the column, NULL if not found.
     */
    template <typename First>
    First* get(flecs::entity_t second) const {
        return static_cast<First*>(get(_::cpp_type<First>::id(m_world), second));
    }

    /** Get pointer to component array by pair.
     * 
     * @tparam First The first element of the pair.
     * @tparam Second The second element of the pair.
     * @return Pointer to the column, NULL if not found.
     */
    template <typename First, typename Second, typename P = flecs::pair<First, Second>, 
        typename A = actual_type_t<P>, if_not_t< flecs::is_pair<First>::value> = 0>
    A* get() const {
        return static_cast<A*>(get<First>(_::cpp_type<Second>::id(m_world)));
    }

    /** Get column size */
    size_t column_size(int32_t column_index) {
        return ecs_table_get_column_size(m_table, column_index);
    }

    /** Get depth for given relationship.
     *
     * @param rel The relationship.
     * @return The depth.
     */    
    int32_t depth(flecs::entity_t rel) {
        return ecs_table_get_depth(m_world, m_table, rel);
    }

    /** Get depth for given relationship.
     *
     * @tparam Rel The relationship.
     * @return The depth.
     */
    template <typename Rel>
    int32_t depth() {
        return depth(_::cpp_type<Rel>::id(m_world));
    }

    /* Implicit conversion to table_t */
    operator table_t*() const {
        return m_table;
    }

protected:
    world_t *m_world;
    table_t *m_table;
};

struct table_range : table {
    table_range() 
        : table()
        , m_offset(0)
        , m_count(0) { }

    table_range(world_t *world, table_t *t, int32_t offset, int32_t count)
        : table(world, t)
        , m_offset(offset)
        , m_count(count) { }

    int32_t offset() const {
        return m_offset;
    }

    int32_t count() const {
        return m_count;
    }

    /** Get pointer to component array by column index. 
     * 
     * @param index The column index.
     * @return Pointer to the column, NULL if not a component.
     */
    void* get_by_index(int32_t index) const override {
        return ecs_table_get_column(m_table, index, m_offset);
    }

private:
    int32_t m_offset = 0;
    int32_t m_count = 0;
};

/** @} */

}


// Mixin implementations
/**
 * @file addons/cpp/mixins/id/impl.hpp
 * @brief Id class implementation.
 */

#pragma once

namespace flecs {

inline flecs::entity id::entity() const {
    ecs_assert(!is_pair(), ECS_INVALID_OPERATION, NULL);
    ecs_assert(!flags(), ECS_INVALID_OPERATION, NULL);
    return flecs::entity(m_world, m_id);
}

inline flecs::entity id::flags() const {
    return flecs::entity(m_world, m_id & ECS_ID_FLAGS_MASK);
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
        return flecs::entity(e);
    }
}

inline flecs::entity id::add_flags(flecs::id_t flags) const {
    return flecs::entity(m_world, m_id | flags);
}

inline flecs::entity id::remove_flags(flecs::id_t flags) const {
    (void)flags;
    ecs_assert((m_id & ECS_ID_FLAGS_MASK) == flags, ECS_INVALID_PARAMETER, NULL);
    return flecs::entity(m_world, m_id & ECS_COMPONENT_MASK);
}

inline flecs::entity id::remove_flags() const {
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

template <typename First, typename Second>
inline flecs::id world::pair() const {
    return flecs::id(
        m_world, 
        ecs_pair(
            _::cpp_type<First>::id(m_world), 
            _::cpp_type<Second>::id(m_world)));
}

template <typename First>
inline flecs::id world::pair(entity_t o) const {
    return flecs::id(
        m_world,
        ecs_pair(
            _::cpp_type<First>::id(m_world), 
            o));
}

inline flecs::id world::pair(entity_t r, entity_t o) const {
    return flecs::id(
        m_world,
        ecs_pair(r, o));
}

}

/**
 * @file addons/cpp/mixins/entity/impl.hpp
 * @brief Entity implementation.
 */

#pragma once

namespace flecs {

template <typename T>
flecs::entity ref<T>::entity() const {
    return flecs::entity(m_world, m_ref.entity);
}

template <typename Self>
template <typename Func, if_t< is_callable<Func>::value > >
inline Self& entity_builder<Self>::set(const Func& func) {
    _::entity_with_invoker<Func>::invoke_get_mut(
        this->m_world, this->m_id, func);
    return to_base();
}

template <typename T, if_t< is_enum<T>::value > >
const T* entity_view::get() const {
    entity_t r = _::cpp_type<T>::id(m_world);
    entity_t c = ecs_get_target(m_world, m_id, r, 0);

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

template<typename First>
inline flecs::entity entity_view::target(int32_t index) const 
{
    return flecs::entity(m_world, 
        ecs_get_target(m_world, m_id, _::cpp_type<First>::id(m_world), index));
}

inline flecs::entity entity_view::target(
    flecs::entity_t relationship, 
    int32_t index) const 
{
    return flecs::entity(m_world, 
        ecs_get_target(m_world, m_id, relationship, index));
}

inline flecs::entity entity_view::target_for(
    flecs::entity_t relationship, 
    flecs::id_t id) const 
{
    return flecs::entity(m_world, 
        ecs_get_target_for_id(m_world, m_id, relationship, id));
}

template <typename T>
inline flecs::entity entity_view::target_for(flecs::entity_t relationship) const {
    return target_for(relationship, _::cpp_type<T>::id(m_world));
}

template <typename First, typename Second>
inline flecs::entity entity_view::target_for(flecs::entity_t relationship) const {
    return target_for(relationship, _::cpp_type<First, Second>::id(m_world));
}

inline flecs::entity entity_view::parent() const {
    return target(flecs::ChildOf);
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
    return flecs::type(m_world, ecs_get_type(m_world, m_id));
}

inline flecs::table entity_view::table() const {
    return flecs::table(m_world, ecs_get_table(m_world, m_id));
}

inline flecs::table_range entity_view::range() const {
    ecs_record_t *r = ecs_record_find(m_world, m_id);
    if (r) {
        return flecs::table_range(m_world, r->table, 
            ECS_RECORD_TO_ROW(r->row), 1);
    }
    return flecs::table_range();
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

        // Union object is not stored in type, so handle separately
        if (ECS_PAIR_FIRST(id) == EcsUnion) {
            ent = flecs::id(m_world, ECS_PAIR_SECOND(id),
                ecs_get_target(m_world, m_id, ECS_PAIR_SECOND(id), 0));
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
    ecs_assert(m_id != 0, ECS_INVALID_PARAMETER, "invalid lookup from null handle");
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

template <typename E, if_t< is_enum<E>::value >>
inline flecs::entity world::entity(E value) const {
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

/**
 * @file addons/cpp/mixins/component/impl.hpp
 * @brief Component mixin implementation
 */

#pragma once

namespace flecs {

template <typename T, typename... Args>
inline flecs::component<T> world::component(Args &&... args) const {
    return flecs::component<T>(m_world, FLECS_FWD(args)...);
}

template <typename... Args>
inline flecs::untyped_component world::component(Args &&... args) const {
    return flecs::untyped_component(m_world, FLECS_FWD(args)...);
}

} // namespace flecs

/**
 * @file addons/cpp/mixins/term/impl.hpp
 * @brief Term implementation.
 */

#pragma once

/**
 * @file addons/cpp/mixins/term/builder_i.hpp
 * @brief Term builder interface.
 */

#pragma once

/**
 * @file addons/cpp/utils/signature.hpp
 * @brief Compile time utilities for deriving query attributes from param pack.
 */

#pragma once

#include <stdio.h>

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
            : m_world(world)
            , ids({ (_::cpp_type<Components>::id(world))... })
            , inout ({ (type_to_inout<Components>())... })
            , oper ({ (type_to_oper<Components>())... }) 
        { }

        flecs::world_t *m_world;
        flecs::array<flecs::id_t, sizeof...(Components)> ids;
        flecs::array<flecs::inout_kind_t, sizeof...(Components)> inout;
        flecs::array<flecs::oper_kind_t, sizeof...(Components)> oper;

        template <typename Builder>
        void populate(const Builder& b) {
            size_t i = 0;
            for (auto id : ids) {
                if (!(id & ECS_ID_FLAGS_MASK)) {
                    const flecs::type_info_t *ti = ecs_get_type_info(m_world, id);
                    if (ti) {
                        // Union relationships always return a value of type
                        // flecs::entity_t which holds the target id of the 
                        // union relationship.
                        // If a union component with a non-zero size (like an 
                        // enum) is added to the query signature, the each/iter
                        // functions would accept a parameter of the component
                        // type instead of flecs::entity_t, which would cause
                        // an assert.
                        ecs_assert(!ti->size || !ecs_has_id(m_world, id, flecs::Union),
                            ECS_INVALID_PARAMETER,
                            "use term() method to add union relationship");
                    }
                }
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
 * A term identifier describes a single identifier in a term. Identifier
 * descriptions can reference entities by id, name or by variable, which means
 * the entity will be resolved when the term is evaluated.
 * 
 * \ingroup cpp_core_filters
 */
template<typename Base>
struct term_id_builder_i {
    term_id_builder_i() : m_term_id(nullptr) { }

    virtual ~term_id_builder_i() { }

    /* The self flag indicates the term identifier itself is used */
    Base& self() {
        this->assert_term_id();
        m_term_id->flags |= flecs::Self;
        return *this;
    }

    /* The up flag indicates that the term identifier may be substituted by
     * traversing a relationship upwards. For example: substitute the identifier
     * with its parent by traversing the ChildOf relationship. */
    Base& up(flecs::entity_t trav = 0) {
        this->assert_term_id();
        m_term_id->flags |= flecs::Up;
        if (trav) {
            m_term_id->trav = trav;
        }
        return *this;
    }

    template <typename Trav>
    Base& up() {
        return this->up(_::cpp_type<Trav>::id(this->world_v()));
    }

    /* The cascade flag is like up, but returns results in breadth-first order.
     * Only supported for flecs::query */
    Base& cascade(flecs::entity_t trav = 0) {
        this->assert_term_id();
        m_term_id->flags |= flecs::Cascade;
        if (trav) {
            m_term_id->trav = trav;
        }
        return *this;
    }

    template <typename Trav>
    Base& cascade() {
        return this->cascade(_::cpp_type<Trav>::id(this->world_v()));
    }

    /* The parent flag is short for up(flecs::ChildOf) */
    Base& parent() {
        this->assert_term_id();
        m_term_id->flags |= flecs::Parent;
        return *this;
    }

    /* Specify relationship to traverse, and flags to indicate direction */
    Base& trav(flecs::entity_t trav, flecs::flags32_t flags = 0) {
        this->assert_term_id();
        m_term_id->trav = trav;
        m_term_id->flags |= flags;
        return *this;
    }

    /* Specify value of identifier by id */
    Base& id(flecs::entity_t id) {
        this->assert_term_id();
        m_term_id->id = id;
        return *this;
    }

    /* Specify value of identifier by id. Amost the same as id(entity), but this
     * operation explicitly sets the flecs::IsEntity flag. This forces the id to 
     * be interpreted as entity, whereas not setting the flag would implicitly
     * convert ids for builtin variables such as flecs::This to a variable.
     * 
     * This function can also be used to disambiguate id(0), which would match
     * both id(entity_t) and id(const char*).
     */
    Base& entity(flecs::entity_t entity) {
        this->assert_term_id();
        m_term_id->flags = flecs::IsEntity;
        m_term_id->id = entity;
        return *this;
    }

    /* Specify value of identifier by name */
    Base& name(const char *name) {
        this->assert_term_id();
        m_term_id->flags |= flecs::IsEntity;
        m_term_id->name = const_cast<char*>(name);
        return *this;
    }

    /* Specify identifier is a variable (resolved at query evaluation time) */
    Base& var(const char *var_name) {
        this->assert_term_id();
        m_term_id->flags |= flecs::IsVariable;
        m_term_id->name = const_cast<char*>(var_name);
        return *this;
    }

    /* Override term id flags */
    Base& flags(flecs::flags32_t flags) {
        this->assert_term_id();
        m_term_id->flags = flags;
        return *this;
    }

    ecs_term_id_t *m_term_id;
    
protected:
    virtual flecs::world_t* world_v() = 0;

private:
    void assert_term_id() {
        ecs_assert(m_term_id != NULL, ECS_INVALID_PARAMETER, 
            "no active term (call .term() first)");
    }

    operator Base&() {
        return *static_cast<Base*>(this);
    }
};

/** Term builder interface. 
 * A term is a single element of a query expression. 
 * 
 * \ingroup cpp_addons_filter
 */
template<typename Base>
struct term_builder_i : term_id_builder_i<Base> {
    term_builder_i() : m_term(nullptr) { }

    term_builder_i(ecs_term_t *term_ptr) { 
        set_term(term_ptr);
    }

    Base& term(id_t id) {
        return this->id(id);
    }

    /* Call prior to setting values for src identifier */
    Base& src() {
        this->assert_term();
        this->m_term_id = &m_term->src;
        return *this;
    }

    /* Call prior to setting values for first identifier. This is either the
     * component identifier, or first element of a pair (in case second is
     * populated as well). */
    Base& first() {
        this->assert_term();
        this->m_term_id = &m_term->first;
        return *this;
    }

    /* Call prior to setting values for second identifier. This is the second 
     * element of a pair. Requires that first() is populated as well. */
    Base& second() {
        this->assert_term();
        this->m_term_id = &m_term->second;
        return *this;
    }

    /* Select src identifier, initialize it with entity id */
    Base& src(flecs::entity_t id) {
        this->src();
        this->id(id);
        return *this;
    }

    /* Select src identifier, initialize it with id associated with type */
    template<typename T>
    Base& src() {
        this->src(_::cpp_type<T>::id(this->world_v()));
        return *this;
    }

    /* Select src identifier, initialize it with name. If name starts with a $
     * the name is interpreted as a variable. */
    Base& src(const char *name) {
        ecs_assert(name != NULL, ECS_INVALID_PARAMETER, NULL);
        this->src();
        if (name[0] == '$') {
            this->var(&name[1]);
        } else {
            this->name(name);
        }
        return *this;
    }

    /* Select first identifier, initialize it with entity id */
    Base& first(flecs::entity_t id) {
        this->first();
        this->id(id);
        return *this;
    }

    /* Select first identifier, initialize it with id associated with type */
    template<typename T>
    Base& first() {
        this->first(_::cpp_type<T>::id(this->world_v()));
        return *this;
    }

    /* Select first identifier, initialize it with name. If name starts with a $
     * the name is interpreted as a variable. */
    Base& first(const char *name) {
        ecs_assert(name != NULL, ECS_INVALID_PARAMETER, NULL);
        this->first();
        if (name[0] == '$') {
            this->var(&name[1]);
        } else {
            this->name(name);
        }
        return *this;
    }

    /* Select second identifier, initialize it with entity id */
    Base& second(flecs::entity_t id) {
        this->second();
        this->id(id);
        return *this;
    }

    /* Select second identifier, initialize it with id associated with type */
    template<typename T>
    Base& second() {
        this->second(_::cpp_type<T>::id(this->world_v()));
        return *this;
    }

    /* Select second identifier, initialize it with name. If name starts with a $
     * the name is interpreted as a variable. */
    Base& second(const char *name) {
        ecs_assert(name != NULL, ECS_INVALID_PARAMETER, NULL);
        this->second();
        if (name[0] == '$') {
            this->var(&name[1]);
        } else {
            this->name(name);
        }
        return *this;
    }

    /** Set role of term. */
    Base& role(id_t role) {
        this->assert_term();
        m_term->id_flags = role;
        return *this;
    }

    /** Set read/write access of term. */
    Base& inout(flecs::inout_kind_t inout) {
        this->assert_term();
        m_term->inout = static_cast<ecs_inout_kind_t>(inout);
        return *this;
    }

    /** Set read/write access for stage. Use this when a system reads or writes
     * components other than the ones provided by the query. This information 
     * can be used by schedulers to insert sync/merge points between systems
     * where deferred operations are flushed.
     * 
     * Setting this is optional. If not set, the value of the accessed component
     * may be out of sync for at most one frame.
     */
    Base& inout_stage(flecs::inout_kind_t inout) {
        this->assert_term();
        m_term->inout = static_cast<ecs_inout_kind_t>(inout);
        if (m_term->oper != EcsNot) {
            this->src().entity(0);
        }
        return *this;
    }

    /** Short for inout_stage(flecs::Out). 
     *   Use when system uses add, remove or set. 
     */
    Base& write() {
        return this->inout_stage(flecs::Out);
    }

    /** Short for inout_stage(flecs::In).
     *   Use when system uses get.
     */
    Base& read() {
        return this->inout_stage(flecs::In);
    }

    /** Short for inout_stage(flecs::InOut).
     *   Use when system uses get_mut.
     */
    Base& read_write() {
        return this->inout_stage(flecs::InOut);
    }

    /** Short for inout(flecs::In) */
    Base& in() {
        return this->inout(flecs::In);
    }

    /** Short for inout(flecs::Out) */
    Base& out() {
        return this->inout(flecs::Out);
    }

    /** Short for inout(flecs::InOut) */
    Base& inout() {
        return this->inout(flecs::InOut);
    }

    /** Short for inout(flecs::In) */
    Base& inout_none() {
        return this->inout(flecs::InOutNone);
    }

    /** Set operator of term. */
    Base& oper(flecs::oper_kind_t oper) {
        this->assert_term();
        m_term->oper = static_cast<ecs_oper_kind_t>(oper);
        return *this;
    }

    /* Short for oper(flecs::And) */
    Base& and_() {
        return this->oper(flecs::And);
    }

    /* Short for oper(flecs::Or) */
    Base& or_() {
        return this->oper(flecs::Or);
    }

    /* Short for oper(flecs::Or) */
    Base& not_() {
        return this->oper(flecs::Not);
    }

    /* Short for oper(flecs::Or) */
    Base& optional() {
        return this->oper(flecs::Optional);
    }

    /* Short for oper(flecs::AndFrom) */
    Base& and_from() {
        return this->oper(flecs::AndFrom);
    }

    /* Short for oper(flecs::OrFrom) */
    Base& or_from() {
        return this->oper(flecs::OrFrom);
    }

    /* Short for oper(flecs::NotFrom) */
    Base& not_from() {
        return this->oper(flecs::NotFrom);
    }

    /** Match singleton. */
    Base& singleton() {
        this->assert_term();
        ecs_assert(m_term->id || m_term->first.id, ECS_INVALID_PARAMETER, 
                "no component specified for singleton");
        
        flecs::id_t sid = m_term->id;
        if (!sid) {
            sid = m_term->first.id;
        }

        ecs_assert(sid != 0, ECS_INVALID_PARAMETER, NULL);
        m_term->src.id = sid;
        return *this;
    }

    /* Filter terms are not triggered on by observers */
    Base& filter() {
        m_term->src.flags |= flecs::Filter;
        return *this;
    }

    ecs_term_t *m_term;

protected:
    virtual flecs::world_t* world_v() = 0;

    void set_term(ecs_term_t *term) {
        m_term = term;
        if (term) {
            this->m_term_id = &m_term->src; // default to subject
        } else {
            this->m_term_id = nullptr;
        }
    }

private:
    void assert_term() {
        ecs_assert(m_term != NULL, ECS_INVALID_PARAMETER, 
            "no active term (call .term() first)");
    }

    operator Base&() {
        return *static_cast<Base*>(this);
    }   
};

}


namespace flecs {

/** Class that describes a term.
 * 
 * \ingroup cpp_core_filters
 */
struct term final : term_builder_i<term> {
    term()
        : term_builder_i<term>(&value)
        , value({})
        , m_world(nullptr) { value.move = true; }

    term(flecs::world_t *world_ptr) 
        : term_builder_i<term>(&value)
        , value({})
        , m_world(world_ptr) { value.move = true; }

    term(flecs::world_t *world_ptr, ecs_term_t t)
        : term_builder_i<term>(&value)
        , value({})
        , m_world(world_ptr) {
            value = t;
            value.move = false;
            this->set_term(&value);
        }

    term(flecs::world_t *world_ptr, id_t id)
        : term_builder_i<term>(&value)
        , value({})
        , m_world(world_ptr) {
            if (id & ECS_ID_FLAGS_MASK) {
                value.id = id;
            } else {
                value.first.id = id;
            }
            value.move = false;
            this->set_term(&value);
        }

    term(flecs::world_t *world_ptr, entity_t r, entity_t o)
        : term_builder_i<term>(&value)
        , value({})
        , m_world(world_ptr) {
            value.id = ecs_pair(r, o);
            value.move = false;
            this->set_term(&value);
        }

    term(id_t id) 
        : term_builder_i<term>(&value)
        , value({})
        , m_world(nullptr) { 
            if (id & ECS_ID_FLAGS_MASK) {
                value.id = id;
            } else {
                value.first.id = id;
            }
            value.move = true; 
        }

    term(id_t r, id_t o) 
        : term_builder_i<term>(&value)
        , value({})
        , m_world(nullptr) { 
            value.id = ecs_pair(r, o);
            value.move = true; 
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
        return ecs_term_finalize(m_world, &value);
    }

    bool is_set() {
        return ecs_term_is_initialized(&value);
    }

    flecs::id id() {
        return flecs::id(m_world, value.id);
    }

    flecs::inout_kind_t inout() {
        return static_cast<flecs::inout_kind_t>(value.inout);
    }

    flecs::oper_kind_t oper() {
        return static_cast<flecs::oper_kind_t>(value.oper);
    }

    flecs::entity get_src() {
        return flecs::entity(m_world, value.src.id);
    }

    flecs::entity get_first() {
        return flecs::entity(m_world, value.first.id);
    }

    flecs::entity get_second() {
        return flecs::entity(m_world, value.second.id);
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

template <typename T>
inline flecs::term world::term() const {
    return flecs::term(m_world, _::cpp_type<T>::id(m_world));
}

template <typename First, typename Second>
inline flecs::term world::term() const {
    return flecs::term(m_world, ecs_pair(
        _::cpp_type<First>::id(m_world),
        _::cpp_type<Second>::id(m_world)));
}

}

/**
 * @file addons/cpp/mixins/filter/impl.hpp
 * @brief Filter implementation.
 */

#pragma once

/**
 * @file addons/cpp/mixins/filter/builder.hpp
 * @brief Filter builder.
 */

#pragma once

/**
 * @file addons/cpp/utils/builder.hpp
 * @brief Builder base class.
 * 
 * Generic functionality for builder classes.
 */

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

/**
 * @file addons/cpp/mixins/filter/builder_i.hpp
 * @brief Filter builder interface.
 */

#pragma once


namespace flecs 
{

/** Filter builder interface.
 * 
 * \ingroup cpp_filters
 */
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

    Base& filter_flags(ecs_flags32_t flags) {
        m_desc->flags |= flags;
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

    /* With/without shorthand notation. */

    template <typename ... Args>
    Base& with(Args&&... args) {
        return this->term(FLECS_FWD(args)...);
    }

    template <typename T, typename ... Args>
    Base& with(Args&&... args) {
        return this->term<T>(FLECS_FWD(args)...);
    }

    template <typename First, typename Second>
    Base& with() {
        return this->term<First, Second>();
    }

    template <typename ... Args>
    Base& without(Args&&... args) {
        return this->term(FLECS_FWD(args)...).not_();
    }

    template <typename T, typename ... Args>
    Base& without(Args&&... args) {
        return this->term<T>(FLECS_FWD(args)...).not_();
    }

    template <typename First, typename Second>
    Base& without() {
        return this->term<First, Second>().not_();
    }

    /* Write/read shorthand notation */

    Base& write() {
        term_builder_i<Base>::write();
        return *this;
    }

    template <typename ... Args>
    Base& write(Args&&... args) {
        return this->term(FLECS_FWD(args)...).write();
    }

    template <typename T, typename ... Args>
    Base& write(Args&&... args) {
        return this->term<T>(FLECS_FWD(args)...).write();
    }

    template <typename First, typename Second>
    Base& write() {
        return this->term<First, Second>().write();
    }

    Base& read() {
        term_builder_i<Base>::read();
        return *this;
    }

    template <typename ... Args>
    Base& read(Args&&... args) {
        return this->term(FLECS_FWD(args)...).read();
    }

    template <typename T, typename ... Args>
    Base& read(Args&&... args) {
        return this->term<T>(FLECS_FWD(args)...).read();
    }

    template <typename First, typename Second>
    Base& read() {
        return this->term<First, Second>().read();
    }

    /* Scope_open/scope_close shorthand notation. */
    Base& scope_open() {
        return this->with(flecs::ScopeOpen).entity(0);
    }

    Base& scope_close() {
        return this->with(flecs::ScopeClose).entity(0);
    }

    /* Term notation for more complex query features */

    Base& term() {
        if (this->m_term) {
            ecs_check(ecs_term_is_initialized(this->m_term), 
                ECS_INVALID_OPERATION, 
                    "filter_builder::term() called without initializing term");
        }

        if (m_term_index >= FLECS_TERM_DESC_MAX) {
            if (m_term_index == FLECS_TERM_DESC_MAX) {
                m_desc->terms_buffer = ecs_os_calloc_n(
                    ecs_term_t, m_term_index + 1);
                ecs_os_memcpy_n(m_desc->terms_buffer, m_desc->terms, 
                    ecs_term_t, m_term_index);
                ecs_os_memset_n(m_desc->terms, 0, 
                    ecs_term_t, FLECS_TERM_DESC_MAX);
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

    Base& term_at(int32_t term_index) {
        ecs_assert(term_index > 0, ECS_INVALID_PARAMETER, NULL);
        int32_t prev_index = m_term_index;
        m_term_index = term_index - 1;
        this->term();
        m_term_index = prev_index;
        ecs_assert(ecs_term_is_initialized(this->m_term), 
            ECS_INVALID_PARAMETER, NULL);
        return *this;
    }

    Base& arg(int32_t term_index) {
        return this->term_at(term_index);
    }

    template<typename T>
    Base& term() {
        this->term();
        *this->m_term = flecs::term(_::cpp_type<T>::id(this->world_v())).move();
        this->m_term->inout = static_cast<ecs_inout_kind_t>(
            _::type_to_inout<T>());
        return *this;
    }

    Base& term(id_t id) {
        this->term();
        *this->m_term = flecs::term(id).move();
        return *this;
    }

    Base& term(const char *name) {
        this->term();
        *this->m_term = flecs::term().first(name).move();
        return *this;
    }

    Base& term(const char *first, const char *second) {
        this->term();
        *this->m_term = flecs::term().first(first).second(second).move();
        return *this;
    }

    Base& term(entity_t r, entity_t o) {
        this->term();
        *this->m_term = flecs::term(r, o).move();
        return *this;
    }

    Base& term(entity_t r, const char *o) {
        this->term();
        *this->m_term = flecs::term(r).second(o).move();
        return *this;
    }

    template<typename First>
    Base& term(id_t o) {
        return this->term(_::cpp_type<First>::id(this->world_v()), o);
    }

    template<typename First>
    Base& term(const char *second) {
        return this->term(_::cpp_type<First>::id(this->world_v())).second(second);
    }

    template<typename First, typename Second>
    Base& term() {
        return this->term<First>(_::cpp_type<Second>::id(this->world_v()));
    }

    template <typename E, if_t< is_enum<E>::value > = 0>
    Base& term(E value) {
        flecs::entity_t r = _::cpp_type<E>::id(this->world_v());
        auto o = enum_type<E>(this->world_v()).entity(value);
        return this->term(r, o);
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

/** Filter builder.
 * 
 * \ingroup cpp_filters
 */
template <typename ... Components>
struct filter_builder final : _::filter_builder_base<Components...> {
    filter_builder(flecs::world_t* world, const char *name = nullptr)
        : _::filter_builder_base<Components...>(world)
    {
        _::sig<Components...>(world).populate(this);
        if (name != nullptr) {
            ecs_entity_desc_t entity_desc = {};
            entity_desc.name = name;
            entity_desc.sep = "::";
            entity_desc.root_sep = "::";
            this->m_desc.entity = ecs_entity_init(world, &entity_desc);
        }
    }

    template <typename Func>
    void each(Func&& func) {
        this->build().each(FLECS_FWD(func));
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
        desc->storage = &m_filter;

        if (ecs_filter_init(world, desc) == NULL) {
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

    flecs::entity entity() {
        return flecs::entity(m_world, ecs_get_entity(m_filter_ptr));
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

    int32_t field_count() {
        return m_filter_ptr->term_count;
    }

    flecs::string str() {
        char *result = ecs_filter_str(m_world, m_filter_ptr);
        return flecs::string(result);
    }

    operator filter<>() const;

protected:
    world_t *m_world = nullptr;
    filter_t m_filter = ECS_FILTER_INIT;
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
        filter_base::operator=(FLECS_FWD(obj));
        return *this;
    }

private:
    ecs_iter_t get_iter(flecs::world_t *world) const override {
        if (!world) {
            world = m_world;
        }
        return ecs_filter_iter(world, m_filter_ptr);
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

/**
 * @file addons/cpp/mixins/event/impl.hpp
 * @brief Event implementation.
 */

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

/**
 * @file addons/cpp/mixins/query/impl.hpp
 * @brief Query implementation.
 */

#pragma once

/**
 * @file addons/cpp/mixins/query/builder.hpp
 * @brief Query builder.
 */

#pragma once

/**
 * @file addons/cpp/mixins/query/builder_i.hpp
 * @brief Query builder interface.
 */

#pragma once


namespace flecs {

/** Query builder interface.
 * 
 * \ingroup cpp_core_queries
 */
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
     * If multiple queries that match the same (down)set of tables specify different 
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
     * @param group_by_action Callback that determines group id for table.
     */
    template <typename T>
    Base& group_by(uint64_t(*group_by_action)(flecs::world_t*, flecs::table_t *table, flecs::id_t id, void* ctx)) {
        ecs_group_by_action_t action = reinterpret_cast<ecs_group_by_action_t>(group_by_action);
        return this->group_by(_::cpp_type<T>::id(this->world_v()), action);
    }

    /** Group and sort matched tables.
     * Same as group_by<T>, but with component identifier.
     *
     * @param component The component used to determine the group rank.
     * @param group_by_action Callback that determines group id for table.
     */
    Base& group_by(flecs::entity_t component, uint64_t(*group_by_action)(flecs::world_t*, flecs::table_t *table, flecs::id_t id, void* ctx)) {
        m_desc->group_by = reinterpret_cast<ecs_group_by_action_t>(group_by_action);
        m_desc->group_by_id = component;
        return *this;
    }

    /** Group and sort matched tables.
     * Same as group_by<T>, but with default group_by action.
     *
     * @tparam T The component used to determine the group rank.
     */
    template <typename T>
    Base& group_by() {
        return this->group_by(_::cpp_type<T>::id(this->world_v()), nullptr);
    }

    /** Group and sort matched tables.
     * Same as group_by, but with default group_by action.
     *
     * @param component The component used to determine the group rank.
     */
    Base& group_by(flecs::entity_t component) {
        return this->group_by(component, nullptr);
    }

    /** Specify context to be passed to group_by function.
     *
     * @param ctx Context to pass to group_by function.
     * @param ctx_free Function to cleanup context (called when query is deleted).
     */
    Base& group_by_ctx(void *ctx, ecs_ctx_free_t ctx_free = nullptr) {
        m_desc->group_by_ctx = ctx;
        m_desc->group_by_ctx_free = ctx_free;
        return *this;
    }

    /** Specify on_group_create action.
     */
    Base& on_group_create(ecs_group_create_action_t action) {
        m_desc->on_group_create = action;
        return *this;
    }

    /** Specify on_group_delete action.
     */
    Base& on_group_delete(ecs_group_delete_action_t action) {
        m_desc->on_group_delete = action;
        return *this;
    }

    /** Specify parent query (creates subquery) */
    Base& observable(const query_base& parent);
    
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

/** Query builder.
 * 
 * \ingroup cpp_core_queries
 */
template <typename ... Components>
struct query_builder final : _::query_builder_base<Components...> {
    query_builder(flecs::world_t* world, const char *name = nullptr)
        : _::query_builder_base<Components...>(world)
    {
        _::sig<Components...>(world).populate(this);
        if (name != nullptr) {
            ecs_entity_desc_t entity_desc = {};
            entity_desc.name = name;
            entity_desc.sep = "::";
            entity_desc.root_sep = "::";
            this->m_desc.filter.entity = ecs_entity_init(world, &entity_desc);
        }
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
    bool changed() const {
        return ecs_query_changed(m_query, 0);
    }

    /** Returns whether query is orphaned.
     * When the parent query of a subquery is deleted, it is left in an orphaned
     * state. The only valid operation on an orphaned query is deleting it. Only
     * subqueries can be orphaned.
     *
     * @return true if query is orphaned, otherwise false.
     */
    bool orphaned() const {
        return ecs_query_orphaned(m_query);
    }

    /** Get info for group. 
     * 
     * @param group_id The group id for which to retrieve the info.
     * @return The group info.
     */
    const flecs::query_group_info_t* group_info(uint64_t group_id) const {
        return ecs_query_get_group_info(m_query, group_id);
    }

    /** Get context for group. 
     * 
     * @param group_id The group id for which to retrieve the context.
     * @return The group context.
     */
    void* group_ctx(uint64_t group_id) const {
        const flecs::query_group_info_t *gi = group_info(group_id);
        if (gi) {
            return gi->ctx;
        } else {
            return NULL;
        }
    }

    /** Free the query.
     */
    void destruct() {
        ecs_query_fini(m_query);
        m_world = nullptr;
        m_query = nullptr;
    }

    template <typename Func>
    void each_term(const Func& func) const {
        const ecs_filter_t *f = ecs_query_get_filter(m_query);
        ecs_assert(f != NULL, ECS_INVALID_PARAMETER, NULL);

        for (int i = 0; i < f->term_count; i ++) {
            flecs::term t(m_world, f->terms[i]);
            func(t);
        }
    }

    filter_base filter() const {
        return filter_base(m_world, ecs_query_get_filter(m_query));
    }

    flecs::term term(int32_t index) const {
        const ecs_filter_t *f = ecs_query_get_filter(m_query);
        ecs_assert(f != NULL, ECS_INVALID_PARAMETER, NULL);
        return flecs::term(m_world, f->terms[index]);
    }

    int32_t field_count() const {
        const ecs_filter_t *f = ecs_query_get_filter(m_query);
        return f->term_count;   
    }

    flecs::string str() const {
        const ecs_filter_t *f = ecs_query_get_filter(m_query);
        char *result = ecs_filter_str(m_world, f);
        return flecs::string(result);
    }

    flecs::entity entity() const {
        return flecs::entity(m_world, ecs_get_entity(m_query));
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

    ecs_iter_t get_iter(flecs::world_t *world) const override {
        if (!world) {
            world = m_world;
        }
        return ecs_query_iter(world, m_query);
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
inline Base& query_builder_i<Base, Components ...>::observable(const query_base& parent) {
    m_desc->parent = parent;
    return *static_cast<Base*>(this);
}

// query_base implementation
inline query_base::operator query<>() const {
    return flecs::query<>(m_world, m_query);
}

} // namespace flecs

/**
 * @file addons/cpp/mixins/observer/impl.hpp
 * @brief Observer implementation.
 */

#pragma once

/**
 * @file addons/cpp/mixins/observer/builder.hpp
 * @brief Observer builder.
 */

#pragma once

/**
 * @file addons/cpp/utils/node_builder.hpp
 * @brief Base builder class for node objects, like systems, observers.
 */

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
        ecs_entity_desc_t entity_desc = {};
        entity_desc.name = name;
        entity_desc.sep = "::";
        m_desc.entity = ecs_entity_init(m_world, &entity_desc);
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
        
        return T(m_world, &m_desc, m_instanced);
    }
};

#undef FLECS_IBUILDER

} // namespace _
} // namespace flecs

/**
 * @file addons/cpp/mixins/observer/builder_i.hpp
 * @brief Observer builder interface.
 */

#pragma once


namespace flecs {

/** Observer builder interface.
 * 
 * \ingroup cpp_observers
 */
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

    /** Set observer context */
    Base& ctx(void *ptr) {
        m_desc->ctx = ptr;
        return *this;
    }

    /** Set observer run callback */
    Base& run(ecs_iter_action_t action) {
        m_desc->run = action;
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

/** Observer builder.
 * 
 * \ingroup cpp_observers
 */
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

    observer(flecs::world_t *world, ecs_observer_desc_t *desc, bool instanced) 
    {
        if (!desc->filter.instanced) {
            desc->filter.instanced = instanced;
        }

        m_world = world;
        m_id = ecs_observer_init(world, desc);

        if (desc->filter.terms_buffer) {
            ecs_os_free(desc->filter.terms_buffer);
        }
    }

    void ctx(void *ctx) {
        ecs_observer_desc_t desc = {};
        desc.entity = m_id;
        desc.ctx = ctx;
        ecs_observer_init(m_world, &desc);
    }

    void* ctx() const {
        return ecs_get_observer_ctx(m_world, m_id);
    }

    flecs::filter<> query() const {
        const flecs::Poly *poly = this->get<flecs::Poly>(flecs::Observer);
        const ecs_observer_t *ob = static_cast<const flecs::observer_t*>(poly->poly);
        return flecs::filter<>(m_world, &ob->filter);
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

/**
 * @file addons/cpp/mixins/enum/impl.hpp
 * @brief Enum implementation.
 */

#pragma once

namespace flecs {

template <typename E>
inline E entity_view::to_constant() const {
    const E* ptr = this->get<E>();
    ecs_assert(ptr != NULL, ECS_INVALID_PARAMETER, "entity is not a constant");
    return ptr[0];
}

template <typename E, if_t< is_enum<E>::value >>
inline flecs::entity world::to_entity(E constant) const {
    const auto& et = enum_type<E>(m_world);
    return flecs::entity(m_world, et.entity(constant));
}

}
#ifdef FLECS_MODULE
/**
 * @file addons/cpp/mixins/module/impl.hpp
 * @brief Module implementation.
 */

#pragma once

namespace flecs {

namespace _ {

template <typename T>
ecs_entity_t do_import(world& world, const char *symbol) {
    ecs_trace("#[magenta]import#[reset] %s", _::type_name<T>());
    ecs_log_push();

    ecs_entity_t scope = ecs_set_scope(world, 0);

    // Initialize module component type & don't allow it to be registered as a
    // tag, as this would prevent calling emplace()
    auto m_c = component<T>(world, nullptr, false);
    ecs_add_id(world, m_c, EcsModule);

    ecs_set_scope(world, m_c);
    world.emplace<T>(world);
    ecs_set_scope(world, scope);

    // It should now be possible to lookup the module
    ecs_entity_t m = ecs_lookup_symbol(world, symbol, true, false);
    ecs_assert(m != 0, ECS_MODULE_UNDEFINED, symbol);
    ecs_assert(m == m_c, ECS_INTERNAL_ERROR, NULL);

    ecs_log_pop();     

    return m;
}

template <typename T>
flecs::entity import(world& world) {
    const char *symbol = _::symbol_name<T>();

    ecs_entity_t m = ecs_lookup_symbol(world, symbol, true, false);
    
    if (!_::cpp_type<T>::registered(world)) {

        /* Module is registered with world, initialize static data */
        if (m) {
            _::cpp_type<T>::init(m, false);
        
        /* Module is not yet registered, register it now */
        } else {
            m = _::do_import<T>(world, symbol);
        }

    /* Module has been registered, but could have been for another world. Import
     * if module hasn't been registered for this world. */
    } else if (!m) {
        m = _::do_import<T>(world, symbol);
    }

    return flecs::entity(world, m);
}

}

/**
 * @defgroup cpp_addons_modules Modules
 * @brief Modules organize components, systems and more in reusable units of code.
 * 
 * \ingroup cpp_addons
 * @{
 */

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
    return flecs::_::import<Module>(*this);
}

/** @} */

}

#endif
#ifdef FLECS_SYSTEM
/**
 * @file addons/cpp/mixins/system/impl.hpp
 * @brief System module implementation.
 */

#pragma once

/**
 * @file addons/cpp/mixins/system/builder.hpp
 * @brief System builder.
 */

#pragma once

/**
 * @file addons/cpp/mixins/system/builder_i.hpp
 * @brief System builder interface.
 */

#pragma once


namespace flecs 
{

/** System builder interface.
 * 
 * \ingroup cpp_addons_systems
 */
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
        flecs::entity_t cur_phase = ecs_get_target(
            world_v(), m_desc->entity, EcsDependsOn, 0);
        if (cur_phase) {
            ecs_remove_id(world_v(), m_desc->entity, ecs_dependson(cur_phase));
            ecs_remove_id(world_v(), m_desc->entity, cur_phase);
        }
        if (phase) {
            ecs_add_id(world_v(), m_desc->entity, ecs_dependson(phase));
            ecs_add_id(world_v(), m_desc->entity, phase);
        }
        return *this;
    }

    /** Specify in which phase the system should run.
     *
     * @tparam Phase The phase.
     */
    template <typename Phase>
    Base& kind() {
        return this->kind(_::cpp_type<Phase>::id(world_v()));
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
    Base& no_readonly(bool value = true) {
        m_desc->no_readonly = value;
        return *this;
    }

    /** Set system interval.
     * This operation will cause the system to be ran at the specified interval.
     *
     * The timer is synchronous, and is incremented each frame by delta_time.
     *
     * @param interval The interval value.
     */
    Base& interval(ecs_ftime_t interval) {
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

    /** Set tick source.
     * This operation sets a shared tick source for the system.
     *
     * @param tick_source The tick source to use for the system.
     */
    Base& tick_source(flecs::entity_t tick_source) {
        m_desc->tick_source = tick_source;
        return *this;
    }

    /** Set system context */
    Base& ctx(void *ptr) {
        m_desc->ctx = ptr;
        return *this;
    }

    /** Set system run callback */
    Base& run(ecs_iter_action_t action) {
        m_desc->run = action;
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

/** @} */

}


namespace flecs {
namespace _ {
    template <typename ... Components>
    using system_builder_base = node_builder<
        system, ecs_system_desc_t, system_builder<Components...>, 
        system_builder_i, Components ...>;
}

/** System builder.
 * 
 * \ingroup cpp_addons_systems
 */
template <typename ... Components>
struct system_builder final : _::system_builder_base<Components...> {
    system_builder(flecs::world_t* world, const char *name = nullptr)
        : _::system_builder_base<Components...>(world, name)
    {
        _::sig<Components...>(world).populate(this);

#ifdef FLECS_PIPELINE
        ecs_add_id(world, this->m_desc.entity, ecs_dependson(flecs::OnUpdate));
        ecs_add_id(world, this->m_desc.entity, flecs::OnUpdate);
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
        ecs_ftime_t delta_time, 
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
    ecs_ftime_t m_delta_time;
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

    explicit system(flecs::world_t *world, ecs_system_desc_t *desc, bool instanced) 
    {
        if (!desc->query.filter.instanced) {
            desc->query.filter.instanced = instanced;
        }

        m_world = world;
        m_id = ecs_system_init(world, desc);

        if (desc->query.filter.terms_buffer) {
            ecs_os_free(desc->query.filter.terms_buffer);
        }
    }

    void ctx(void *ctx) {
        ecs_system_desc_t desc = {};
        desc.entity = m_id;
        desc.ctx = ctx;
        ecs_system_init(m_world, &desc);
    }

    void* ctx() const {
        return ecs_get_system_ctx(m_world, m_id);
    }

    flecs::query<> query() const {
        return flecs::query<>(m_world, ecs_system_get_query(m_world, m_id));
    }

    system_runner_fluent run(ecs_ftime_t delta_time = 0.0f, void *param = nullptr) const {
        return system_runner_fluent(m_world, m_id, 0, 0, delta_time, param);
    }

    system_runner_fluent run_worker(
        int32_t stage_current, 
        int32_t stage_count, 
        ecs_ftime_t delta_time = 0.0f, 
        void *param = nullptr) const 
    {
        return system_runner_fluent(
            m_world, m_id, stage_current, stage_count, delta_time, param);
    }

#   ifdef FLECS_TIMER
/**
 * @file addons/cpp/mixins/timer/system_mixin.inl
 * @brief Timer module system mixin.
 */

/**
 * \memberof flecs::system
 * \ingroup cpp_addons_timer
 */

/** Set interval.
 * @see ecs_set_interval
 */
void interval(ecs_ftime_t interval);

/** Get interval.
 * @see ecs_get_interval.
 */
ecs_ftime_t interval();

/** Set timeout.
 * @see ecs_set_timeout
 */
void timeout(ecs_ftime_t timeout);

/** Get timeout.
 * @see ecs_get_timeout
 */
ecs_ftime_t timeout();

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

/** @} */

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
/**
 * @file addons/cpp/mixins/pipeline/impl.hpp
 * @brief Pipeline module implementation.
 */

#pragma once

/**
 * @file addons/cpp/mixins/pipeline/builder.hpp
 * @brief Pipeline builder.
 */

#pragma once

/**
 * @file addons/cpp/mixins/pipeline/builder_i.hpp
 * @brief Pipeline builder interface.
 */

#pragma once


namespace flecs {

/** Pipeline builder interface.
 * 
 * \ingroup cpp_pipelines
 */
template<typename Base>
struct pipeline_builder_i : query_builder_i<Base> {
    pipeline_builder_i(ecs_pipeline_desc_t *desc, int32_t term_index = 0) 
        : query_builder_i<Base>(&desc->query, term_index)
        , m_desc(desc) { }

private:
    ecs_pipeline_desc_t *m_desc;
};

}


namespace flecs {
namespace _ {
    template <typename ... Components>
    using pipeline_builder_base = builder<
        pipeline, ecs_pipeline_desc_t, pipeline_builder<Components...>, 
        pipeline_builder_i, Components ...>;
}

/** Pipeline builder.
 * 
 * \ingroup cpp_pipelines
 */
template <typename ... Components>
struct pipeline_builder final : _::pipeline_builder_base<Components...> {
    pipeline_builder(flecs::world_t* world, flecs::entity_t id = 0)
        : _::pipeline_builder_base<Components...>(world)
    {
        _::sig<Components...>(world).populate(this);
        this->m_desc.entity = id;
    }
};

}


namespace flecs {

template <typename ... Components>
struct pipeline : entity {
    pipeline(world_t *world, ecs_pipeline_desc_t *desc) 
        : entity(world)
    {
        m_id = ecs_pipeline_init(world, desc);

        if (!m_id) {
            ecs_abort(ECS_INVALID_PARAMETER, NULL);
        }

        if (desc->query.filter.terms_buffer) {
            ecs_os_free(desc->query.filter.terms_buffer);
        }
    }
};

inline flecs::pipeline_builder<> world::pipeline() const {
    return flecs::pipeline_builder<>(m_world);
}

template <typename Pipeline, if_not_t< is_enum<Pipeline>::value >>
inline flecs::pipeline_builder<> world::pipeline() const {
    return flecs::pipeline_builder<>(m_world, _::cpp_type<Pipeline>::id(m_world));
}

inline void world::set_pipeline(const flecs::entity pip) const {
    return ecs_set_pipeline(m_world, pip);
}

template <typename Pipeline>
inline void world::set_pipeline() const {
    return ecs_set_pipeline(m_world, _::cpp_type<Pipeline>::id(m_world));
}

inline flecs::entity world::get_pipeline() const {
    return flecs::entity(m_world, ecs_get_pipeline(m_world));
}

inline bool world::progress(ecs_ftime_t delta_time) const {
    return ecs_progress(m_world, delta_time);
}

inline void world::run_pipeline(const flecs::entity_t pip, ecs_ftime_t delta_time) const {
    return ecs_run_pipeline(m_world, pip, delta_time);
}

template <typename Pipeline, if_not_t< is_enum<Pipeline>::value >>
inline void world::run_pipeline(ecs_ftime_t delta_time) const {
    return ecs_run_pipeline(m_world, _::cpp_type<Pipeline>::id(m_world), delta_time);
}

inline void world::set_time_scale(ecs_ftime_t mul) const {
    ecs_set_time_scale(m_world, mul);
}  

inline ecs_ftime_t world::get_time_scale() const {
    const ecs_world_info_t *stats = ecs_get_world_info(m_world);
    return stats->time_scale;
}

inline int64_t world::get_tick() const {
    const ecs_world_info_t *stats = ecs_get_world_info(m_world);
    return stats->frame_count_total;
}

inline ecs_ftime_t world::get_target_fps() const {
    const ecs_world_info_t *stats = ecs_get_world_info(m_world);
    return stats->target_fps;
} 

inline void world::set_target_fps(ecs_ftime_t target_fps) const {
    ecs_set_target_fps(m_world, target_fps);
}

inline void world::reset_clock() const {
    ecs_reset_clock(m_world);
}

inline void world::set_threads(int32_t threads) const {
    ecs_set_threads(m_world, threads);
}

inline int32_t world::get_threads() const {
    return ecs_get_stage_count(m_world);
}

inline void world::set_task_threads(int32_t task_threads) const {
    ecs_set_task_threads(m_world, task_threads);
}

inline bool world::using_task_threads() const {
    return ecs_using_task_threads(m_world);
}

}

#endif
#ifdef FLECS_TIMER
/**
 * @file addons/cpp/mixins/timer/impl.hpp
 * @brief Timer module implementation.
 */

#pragma once

namespace flecs {

// Timer class
struct timer final : entity {
    using entity::entity;

    timer& interval(ecs_ftime_t interval) {
        ecs_set_interval(m_world, m_id, interval);
        return *this;
    }

    ecs_ftime_t interval() {
        return ecs_get_interval(m_world, m_id);
    }

    timer& timeout(ecs_ftime_t timeout) {
        ecs_set_timeout(m_world, m_id, timeout);
        return *this;
    }

    ecs_ftime_t timeout() {
        return ecs_get_timeout(m_world, m_id);
    }

    timer& rate(int32_t rate, flecs::entity_t tick_source = 0) {
        ecs_set_rate(m_world, m_id, rate, tick_source);
        return *this;
    }

    void start() {
        ecs_start_timer(m_world, m_id);
    }

    void stop() {
        ecs_stop_timer(m_world, m_id);
    }
};

template <typename... Args>
inline flecs::timer world::timer(Args &&... args) const {
    return flecs::timer(m_world, FLECS_FWD(args)...);
}

inline void system::interval(ecs_ftime_t interval) {
    ecs_set_interval(m_world, m_id, interval);
}

inline ecs_ftime_t system::interval() {
    return ecs_get_interval(m_world, m_id);
}

inline void system::timeout(ecs_ftime_t timeout) {
    ecs_set_timeout(m_world, m_id, timeout);
}

inline ecs_ftime_t system::timeout() {
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
/**
 * @file addons/cpp/mixins/snapshot/impl.hpp
 * @brief Snapshot module implementation.
 */

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
/**
 * @file addons/cpp/mixins/doc/impl.hpp
 * @brief Doc mixin implementation.
 */

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
/**
 * @file addons/cpp/mixins/rest/impl.hpp
 * @brief Rest module implementation.
 */

#pragma once

namespace flecs {
namespace rest {
namespace _ {

inline void init(flecs::world& world) {
    world.component<Rest>("flecs::rest::Rest");
}
 
} // namespace _
} // namespace rest
} // namespace flecs

#endif
#ifdef FLECS_RULES
/**
 * @file addons/cpp/mixins/rule/impl.hpp
 * @brief Rule implementation.
 */

#pragma once

/**
 * @file addons/cpp/mixins/rule/builder.hpp
 * @brief Rule builder.
 */

#pragma once


namespace flecs {
namespace _ {
    template <typename ... Components>
    using rule_builder_base = builder<
        rule, ecs_filter_desc_t, rule_builder<Components...>, 
        filter_builder_i, Components ...>;
}

/** Rule builder.
 * 
 * \ingroup cpp_addons_rules
 */
template <typename ... Components>
struct rule_builder final : _::rule_builder_base<Components...> {
    rule_builder(flecs::world_t* world, const char *name = nullptr)
        : _::rule_builder_base<Components...>(world)
    {
        _::sig<Components...>(world).populate(this);
        if (name != nullptr) {
            ecs_entity_desc_t entity_desc = {};
            entity_desc.name = name;
            entity_desc.sep = "::";
            entity_desc.root_sep = "::";
            this->m_desc.entity = ecs_entity_init(world, &entity_desc);
        }
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
        if (desc->terms_buffer) {
            ecs_os_free(desc->terms_buffer);
        }
    }

    bool is_valid() const {
        return m_rule != nullptr;
    }

    operator rule_t*() const {
        return m_rule;
    }

    flecs::entity entity() {
        return flecs::entity(m_world, ecs_get_entity(m_rule));
    }

    /** Free the rule. */
    void destruct() {
        if (m_rule) {
            ecs_rule_fini(m_rule);
            m_world = nullptr;
            m_rule = nullptr;
        }
    }

    /** Move the rule. */
    void move(flecs::rule_base&& obj) {
        this->destruct();
        this->m_world = obj.m_world;
        this->m_rule = obj.m_rule;
        obj.m_world = nullptr;
        obj.m_rule = nullptr;
    }

    flecs::filter_base filter() const {
        return filter_base(m_world, ecs_rule_get_filter(m_rule));
    }

    /** Converts this rule to a string expression
     * @see ecs_filter_str
     */
    flecs::string str() const {
        const ecs_filter_t *f = ecs_rule_get_filter(m_rule);
        char *result = ecs_filter_str(m_world, f);
        return flecs::string(result);
    }


    /** Converts this rule to a string that can be used to aid debugging
     * the behavior of the rule.
     * @see ecs_rule_str
     */
    flecs::string rule_str() const {
        char *result = ecs_rule_str(m_rule);
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

    ecs_iter_t get_iter(flecs::world_t *world) const override {
        if (!world) {
            world = m_world;
        }
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
/**
 * @file addons/cpp/mixins/meta/impl.hpp
 * @brief Meta implementation.
 */

#pragma once

FLECS_ENUM_LAST(flecs::meta::type_kind_t, flecs::meta::TypeKindLast)
FLECS_ENUM_LAST(flecs::meta::primitive_kind_t, flecs::meta::PrimitiveKindLast)

namespace flecs {
namespace meta {
namespace _ {

/* Type support for entity wrappers */
template <typename EntityType>
inline flecs::opaque<EntityType> flecs_entity_support(flecs::world&) {
    return flecs::opaque<EntityType>()
        .as_type(flecs::Entity)
        .serialize([](const flecs::serializer *ser, const EntityType *data) {
            flecs::entity_t id = data->id();
            return ser->value(flecs::Entity, &id);
        })
        .assign_entity(
            [](EntityType *dst, flecs::world_t *world, flecs::entity_t e) {
                *dst = EntityType(world, e);
            });
}

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
        flecs::_::cpp_type<iptr_t>::init(flecs::Iptr, true);
        ecs_assert(flecs::type_id<iptr_t>() == flecs::Iptr, 
            ECS_INTERNAL_ERROR, NULL);
        // Remove symbol to prevent validation errors, as it doesn't match with 
        // the typename
        ecs_remove_pair(world, flecs::Iptr, ecs_id(EcsIdentifier), EcsSymbol);
    }

    if (!flecs::is_same<u32_t, uptr_t>() && !flecs::is_same<u64_t, uptr_t>()) {
        flecs::_::cpp_type<uptr_t>::init(flecs::Uptr, true);
        ecs_assert(flecs::type_id<uptr_t>() == flecs::Uptr, 
            ECS_INTERNAL_ERROR, NULL);
        // Remove symbol to prevent validation errors, as it doesn't match with 
        // the typename
        ecs_remove_pair(world, flecs::Uptr, ecs_id(EcsIdentifier), EcsSymbol);
    }

    // Register opaque type support for C++ entity wrappers
    world.component<flecs::entity_view>()
        .opaque(flecs_entity_support<flecs::entity_view>);

    world.component<flecs::entity>()
        .opaque(flecs_entity_support<flecs::entity>);
}

} // namespace _

} // namespace meta


inline flecs::entity cursor::get_type() const {
    return flecs::entity(m_cursor.world, ecs_meta_get_type(&m_cursor));
}

inline flecs::entity cursor::get_unit() const {
    return flecs::entity(m_cursor.world, ecs_meta_get_unit(&m_cursor));
}

inline flecs::entity cursor::get_entity() const {
    return flecs::entity(m_cursor.world, ecs_meta_get_entity(&m_cursor));
}

/** Create primitive type */
inline flecs::entity world::primitive(flecs::meta::primitive_kind_t kind) {
    ecs_primitive_desc_t desc = {};
    desc.kind = kind;
    flecs::entity_t eid = ecs_primitive_init(m_world, &desc);
    ecs_assert(eid != 0, ECS_INVALID_OPERATION, NULL);
    return flecs::entity(m_world, eid);
}

/** Create array type. */
inline flecs::entity world::array(flecs::entity_t elem_id, int32_t array_count) {
    ecs_array_desc_t desc = {};
    desc.type = elem_id;
    desc.count = array_count;
    flecs::entity_t eid = ecs_array_init(m_world, &desc);
    ecs_assert(eid != 0, ECS_INVALID_OPERATION, NULL);
    return flecs::entity(m_world, eid);
}

/** Create array type. */
template <typename T>
inline flecs::entity world::array(int32_t array_count) {
    return this->array(_::cpp_type<T>::id(m_world), array_count);
}

inline flecs::entity world::vector(flecs::entity_t elem_id) {
    ecs_vector_desc_t desc = {};
    desc.type = elem_id;
    flecs::entity_t eid = ecs_vector_init(m_world, &desc);
    ecs_assert(eid != 0, ECS_INVALID_OPERATION, NULL);
    return flecs::entity(m_world, eid);
}

template <typename T>
inline flecs::entity world::vector() {
    return this->vector(_::cpp_type<T>::id(m_world));
}

} // namespace flecs

inline int ecs_serializer_t::value(ecs_entity_t type, const void *v) const {
    return this->value_(this, type, v);
}

template <typename T>
inline int ecs_serializer_t::value(const T& v) const {
    return this->value(flecs::_::cpp_type<T>::id(
        const_cast<flecs::world_t*>(this->world)), &v);
}

inline int ecs_serializer_t::member(const char *name) const {
    return this->member_(this, name);
}

#endif
#ifdef FLECS_UNITS
/**
 * @file addons/cpp/mixins/units/impl.hpp
 * @brief Units module implementation.
 */

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
    world.entity<Frequency>("::flecs::units::Frequency");
    world.entity<Uri>("::flecs::units::Uri");

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
    world.entity<length::Pixels>("::flecs::units::Length::Pixels");

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

    // Initialize hertz units
    world.entity<frequency::Hertz>(
        "::flecs::units::Frequency::Hertz");
    world.entity<frequency::KiloHertz>(
        "::flecs::units::Frequency::KiloHertz");
    world.entity<frequency::MegaHertz>(
        "::flecs::units::Frequency::MegaHertz");
    world.entity<frequency::GigaHertz>(
        "::flecs::units::Frequency::GigaHertz");

    // Initialize uri units
    world.entity<uri::Hyperlink>(
        "::flecs::units::Uri::Hyperlink");
    world.entity<uri::Image>(
        "::flecs::units::Uri::Image");
    world.entity<uri::File>(
        "::flecs::units::Uri::File");

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
#ifdef FLECS_MONITOR
/**
 * @file addons/cpp/mixins/monitor/impl.hpp
 * @brief Monitor module implementation.
 */

#pragma once

namespace flecs {

inline monitor::monitor(flecs::world& world) {
    /* Import C module  */
    FlecsMonitorImport(world);

}

}

#endif
#ifdef FLECS_METRICS
/**
 * @file addons/cpp/mixins/metrics/impl.hpp
 * @brief Metrics module implementation.
 */

#pragma once

namespace flecs {

inline metrics::metrics(flecs::world& world) {
    world.import<flecs::units>();

    /* Import C module  */
    FlecsMetricsImport(world);

    world.entity<metrics::Instance>("::flecs::metrics::Instance");
    world.entity<metrics::Metric>("::flecs::metrics::Metric");
    world.entity<metrics::Counter>("::flecs::metrics::Metric::Counter");
    world.entity<metrics::CounterId>("::flecs::metrics::Metric::CounterId");
    world.entity<metrics::CounterIncrement>("::flecs::metrics::Metric::CounterIncrement");
    world.entity<metrics::Gauge>("::flecs::metrics::Metric::Gauge");
}

inline metric_builder::~metric_builder() {
    if (!m_created) {
        ecs_metric_init(m_world, &m_desc);
    }
}

inline metric_builder& metric_builder::member(const char *name) {
    return member(flecs::world(m_world).lookup(name));
}

template <typename T>
inline metric_builder& metric_builder::member(const char *name) {
    flecs::entity e (m_world, _::cpp_type<T>::id(m_world));
    flecs::entity_t m = e.lookup(name);
    if (!m) {
        flecs::log::err("member '%s' not found in type '%s'", 
            name, e.path().c_str());
        return *this;
    }
    return member(m);
}

inline metric_builder::operator flecs::entity() {
    if (!m_created) {
        m_created = true;
        flecs::entity result(m_world, ecs_metric_init(m_world, &m_desc));
        m_desc.entity = result;
        return result;
    } else {
        return flecs::entity(m_world, m_desc.entity);
    }
}

template <typename... Args>
inline flecs::metric_builder world::metric(Args &&... args) const {
    flecs::entity result(m_world, FLECS_FWD(args)...);
    return flecs::metric_builder(m_world, result);
}

template <typename Kind>
inline untyped_component& untyped_component::metric(
    flecs::entity_t parent, 
    const char *brief, 
    const char *metric_name) 
{
    flecs::world w(m_world);
    flecs::entity e(m_world, m_id);

    const flecs::member_t *m = ecs_cpp_last_member(w, e);
    if (!m) {
        return *this;
    }

    flecs::entity me = w.entity(m->member);
    flecs::entity metric_entity = me;
    if (parent) {
        const char *component_name = e.name();
        if (!metric_name) {
            if (ecs_os_strcmp(m->name, "value") || !component_name) {
                metric_entity = w.scope(parent).entity(m->name);
            } else {
                // If name of member is "value", use name of type.
                char *snake_name = flecs_to_snake_case(component_name);
                metric_entity = w.scope(parent).entity(snake_name);
                ecs_os_free(snake_name);
            }
        } else {
            metric_entity = w.scope(parent).entity(metric_name);
        }
    }

    w.metric(metric_entity).member(me).kind<Kind>().brief(brief);

    return *this;
}

}

#endif
#ifdef FLECS_ALERTS
/**
 * @file addons/cpp/mixins/alerts/impl.hpp
 * @brief Alerts module implementation.
 */

#pragma once

/**
 * @file addons/cpp/mixins/alerts/builder.hpp
 * @brief Alert builder.
 */

#pragma once

/**
 * @file addons/cpp/mixins/alerts/builder_i.hpp
 * @brief Alert builder interface.
 */

#pragma once


namespace flecs {

/** Alert builder interface.
 * 
 * \ingroup cpp_addons_alerts
 */
template<typename Base, typename ... Components>
struct alert_builder_i : filter_builder_i<Base, Components ...> {
private:
    using BaseClass = filter_builder_i<Base, Components ...>;
    
public:
    alert_builder_i()
        : BaseClass(nullptr)
        , m_desc(nullptr) { }

    alert_builder_i(ecs_alert_desc_t *desc, int32_t term_index = 0) 
        : BaseClass(&desc->filter, term_index)
        , m_desc(desc) { }

    /** Alert message.
     *
     * @see ecs_alert_desc_t::message
     */      
    Base& message(const char *message) {
        m_desc->message = message;
        return *this;
    }

    /** Set brief description for alert.
     * 
     * @see ecs_alert_desc_t::brief
     */
    Base& brief(const char *brief) {
        m_desc->brief = brief;
        return *this;
    }

    /** Set doc name for alert.
     * 
     * @see ecs_alert_desc_t::doc_name
     */
    Base& doc_name(const char *doc_name) {
        m_desc->doc_name = doc_name;
        return *this;
    }

    /** Set severity of alert (default is Error) 
     * 
     * @see ecs_alert_desc_t::severity
     */
    Base& severity(flecs::entity_t kind) {
        m_desc->severity = kind;
        return *this;
    }

    /* Set retain period of alert. 
     * 
     * @see ecs_alert_desc_t::retain_period
     */
    Base& retain_period(ecs_ftime_t period) {
        m_desc->retain_period = period;
        return *this;
    }

    /** Set severity of alert (default is Error) 
     * 
     * @see ecs_alert_desc_t::severity
     */
    template <typename Severity>
    Base& severity() {
        return severity(_::cpp_type<Severity>::id(world_v()));
    }

    /** Add severity filter */
    Base& severity_filter(flecs::entity_t kind, flecs::id_t with, const char *var = nullptr) {
        ecs_assert(severity_filter_count < ECS_ALERT_MAX_SEVERITY_FILTERS, 
            ECS_INVALID_PARAMETER, "Maxium number of severity filters reached");

        ecs_alert_severity_filter_t *filter = 
            &m_desc->severity_filters[severity_filter_count ++];

        filter->severity = kind;
        filter->with = with;
        filter->var = var;
        return *this;
    }

    /** Add severity filter */
    template <typename Severity>
    Base& severity_filter(flecs::id_t with, const char *var = nullptr) {
        return severity_filter(_::cpp_type<Severity>::id(world_v()), with, var);
    }

    /** Add severity filter */
    template <typename Severity, typename T, if_not_t< is_enum<T>::value > = 0>
    Base& severity_filter(const char *var = nullptr) {
        return severity_filter(_::cpp_type<Severity>::id(world_v()), 
            _::cpp_type<T>::id(world_v()), var);
    }

    /** Add severity filter */
    template <typename Severity, typename T, if_t< is_enum<T>::value > = 0 >
    Base& severity_filter(T with, const char *var = nullptr) {
        flecs::world w(world_v());
        flecs::entity constant = w.to_entity<T>(with);
        return severity_filter(_::cpp_type<Severity>::id(world_v()), 
            w.pair<T>(constant), var);
    }

    /** Set member to create an alert for out of range values */
    Base& member(flecs::entity_t m) {
        m_desc->member = m;
        return *this;
    }

    /** Set (component) id for member (optional). If .member() is set and id
     * is not set, the id will default to the member parent. */
    Base& id(flecs::id_t id) {
        m_desc->id = id;
        return *this;
    }

    /** Set member to create an alert for out of range values */
    template <typename T>
    Base& member(const char *m, const char *v = nullptr) {
        flecs::entity_t id = _::cpp_type<T>::id(world_v());
        flecs::entity_t mid = ecs_lookup_path_w_sep(
            world_v(), id, m, "::", "::", false);
        ecs_assert(m != 0, ECS_INVALID_PARAMETER, NULL);
        m_desc->var = v;
        return this->member(mid);
    }

    /** Set source variable for member (optional, defaults to $this) */
    Base& var(const char *v) {
        m_desc->var = v;
        return *this;
    }

protected:
    virtual flecs::world_t* world_v() = 0;

private:
    operator Base&() {
        return *static_cast<Base*>(this);
    }

    ecs_alert_desc_t *m_desc;
    int32_t severity_filter_count = 0;
};

}


namespace flecs {
namespace _ {
    template <typename ... Components>
    using alert_builder_base = builder<
        alert, ecs_alert_desc_t, alert_builder<Components...>, 
        alert_builder_i, Components ...>;
}

/** Alert builder.
 * 
 * \ingroup cpp_addons_alerts
 */
template <typename ... Components>
struct alert_builder final : _::alert_builder_base<Components...> {
    alert_builder(flecs::world_t* world, const char *name = nullptr)
        : _::alert_builder_base<Components...>(world)
    {
        _::sig<Components...>(world).populate(this);
        if (name != nullptr) {
            ecs_entity_desc_t entity_desc = {};
            entity_desc.name = name;
            entity_desc.sep = "::";
            entity_desc.root_sep = "::";
            this->m_desc.entity = ecs_entity_init(world, &entity_desc);
        }
    }
};

}


namespace flecs {

template <typename ... Components>
struct alert final : entity
{
    using entity::entity;

    explicit alert() {
        m_id = 0;
        m_world = nullptr;
    }

    explicit alert(flecs::world_t *world, ecs_alert_desc_t *desc) 
    {
        m_world = world;
        m_id = ecs_alert_init(world, desc);

        if (desc->filter.terms_buffer) {
            ecs_os_free(desc->filter.terms_buffer);
        }
    }
};

inline alerts::alerts(flecs::world& world) {
    /* Import C module  */
    FlecsAlertsImport(world);

    world.entity<alerts::Alert>("::flecs::alerts::Alert");
    world.entity<alerts::Info>("::flecs::alerts::Info");
    world.entity<alerts::Warning>("::flecs::alerts::Warning");
    world.entity<alerts::Error>("::flecs::alerts::Error");
}

template <typename... Comps, typename... Args>
inline flecs::alert_builder<Comps...> world::alert(Args &&... args) const {
    return flecs::alert_builder<Comps...>(m_world, FLECS_FWD(args)...);
}

}

#endif

/**
 * @file addons/cpp/impl/iter.hpp
 * @brief Iterator implementation.
 */

#pragma once

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

inline flecs::world iter::world() const {
    return flecs::world(m_iter->world);
}

inline flecs::entity iter::entity(size_t row) const {
    ecs_assert(row < static_cast<size_t>(m_iter->count), 
        ECS_COLUMN_INDEX_OUT_OF_RANGE, NULL);
    return flecs::entity(m_iter->world, m_iter->entities[row]);
}

template <typename T>
inline column<T>::column(iter &iter, int32_t index) {
    *this = iter.field<T>(index);
}

inline flecs::entity iter::src(int32_t index) const {
    return flecs::entity(m_iter->world, ecs_field_src(m_iter, index));
}

inline flecs::entity iter::id(int32_t index) const {
    return flecs::entity(m_iter->world, ecs_field_id(m_iter, index));
}

inline flecs::id iter::pair(int32_t index) const {
    flecs::id_t id = ecs_field_id(m_iter, index);
    ecs_check(ECS_HAS_ID_FLAG(id, PAIR), ECS_INVALID_PARAMETER, NULL);
    return flecs::id(m_iter->world, id);
error:
    return flecs::id();
}

inline flecs::type iter::type() const {
    return flecs::type(m_iter->world, ecs_table_get_type(m_iter->table));
}

inline flecs::table iter::table() const {
    return flecs::table(m_iter->real_world, m_iter->table);
}

inline flecs::table_range iter::range() const {
    return flecs::table_range(m_iter->real_world, m_iter->table, 
        m_iter->offset, m_iter->count);
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

/**
 * @file addons/cpp/impl/world.hpp
 * @brief World implementation.
 */

#pragma once

namespace flecs 
{

inline void world::init_builtin_components() {
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
inline flecs::entity world::use(const char *alias) const {
    entity_t e = _::cpp_type<T>::id(m_world);
    const char *name = alias;
    if (!name) {
        // If no name is defined, use the entity name without the scope
        name = ecs_get_name(m_world, e);
    }
    ecs_set_alias(m_world, e, name);
    return flecs::entity(m_world, e);
}

inline flecs::entity world::use(const char *name, const char *alias) const {
    entity_t e = ecs_lookup_path_w_sep(m_world, 0, name, "::", "::", true);
    ecs_assert(e != 0, ECS_INVALID_PARAMETER, NULL);

    ecs_set_alias(m_world, e, alias);
    return flecs::entity(m_world, e);
}

inline void world::use(flecs::entity e, const char *alias) const {
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
inline T* world::get_mut() const {
    flecs::entity e(m_world, _::cpp_type<T>::id(m_world));
    return e.get_mut<T>();
}

template <typename T>
inline void world::modified() const {
    flecs::entity e(m_world, _::cpp_type<T>::id(m_world));
    return e.modified<T>();
}

template <typename First, typename Second>
inline void world::set(Second second, const First& value) const {
    flecs::entity e(m_world, _::cpp_type<First>::id(m_world));
    e.set<First>(second, value);
}

template <typename First, typename Second>
inline void world::set(Second second, First&& value) const {
    flecs::entity e(m_world, _::cpp_type<First>::id(m_world));
    e.set<First>(second, value);
}

template <typename T>
inline ref<T> world::get_ref() const {
    flecs::entity e(m_world, _::cpp_type<T>::id(m_world));
    return e.get_ref<T>();
}

template <typename T>
inline const T* world::get() const {
    flecs::entity e(m_world, _::cpp_type<T>::id(m_world));
    return e.get<T>();
}

template <typename First, typename Second, typename P, typename A>
const A* world::get() const {
    flecs::entity e(m_world, _::cpp_type<First>::id(m_world));
    return e.get<First, Second>();
}

template <typename First, typename Second>
const First* world::get(Second second) const {
    flecs::entity e(m_world, _::cpp_type<First>::id(m_world));
    return e.get<First>(second);
}

template <typename T>
inline bool world::has() const {
    flecs::entity e(m_world, _::cpp_type<T>::id(m_world));
    return e.has<T>();
}

template <typename First, typename Second>
inline bool world::has() const {
    flecs::entity e(m_world, _::cpp_type<First>::id(m_world));
    return e.has<First, Second>();
}

template <typename First>
inline bool world::has(flecs::id_t second) const {
    flecs::entity e(m_world, _::cpp_type<First>::id(m_world));
    return e.has<First>(second);
}

inline bool world::has(flecs::id_t first, flecs::id_t second) const {
    flecs::entity e(m_world, first);
    return e.has(first, second);
}

template <typename T>
inline void world::add() const {
    flecs::entity e(m_world, _::cpp_type<T>::id(m_world));
    e.add<T>();
}

template <typename First, typename Second>
inline void world::add() const {
    flecs::entity e(m_world, _::cpp_type<First>::id(m_world));
    e.add<First, Second>();
}

template <typename First>
inline void world::add(flecs::entity_t second) const {
    flecs::entity e(m_world, _::cpp_type<First>::id(m_world));
    e.add<First>(second);
}

inline void world::add(flecs::entity_t first, flecs::entity_t second) const {
    flecs::entity e(m_world, first);
    e.add(first, second);
}

template <typename T>
inline void world::remove() const {
    flecs::entity e(m_world, _::cpp_type<T>::id(m_world));
    e.remove<T>();
}

template <typename First, typename Second>
inline void world::remove() const {
    flecs::entity e(m_world, _::cpp_type<First>::id(m_world));
    e.remove<First, Second>();
}

template <typename First>
inline void world::remove(flecs::entity_t second) const {
    flecs::entity e(m_world, _::cpp_type<First>::id(m_world));
    e.remove<First>(second);
}

inline void world::remove(flecs::entity_t first, flecs::entity_t second) const {
    flecs::entity e(m_world, first);
    e.remove(first, second);
}

template <typename Func>
inline void world::children(Func&& f) const {
    this->entity(0).children(FLECS_FWD(f));
}

template <typename T>
inline flecs::entity world::singleton() const {
    return flecs::entity(m_world, _::cpp_type<T>::id(m_world));
}

template <typename First>
inline flecs::entity world::target(int32_t index) const
{
    return flecs::entity(m_world,
        ecs_get_target(m_world, _::cpp_type<First>::id(m_world), _::cpp_type<First>::id(m_world), index));
}

template <typename T>
inline flecs::entity world::target(
    flecs::entity_t relationship,
    int32_t index) const
{
    return flecs::entity(m_world,
        ecs_get_target(m_world, _::cpp_type<T>::id(m_world), relationship, index));
}

inline flecs::entity world::target(
    flecs::entity_t relationship,
    int32_t index) const
{
    return flecs::entity(m_world,
        ecs_get_target(m_world, relationship, relationship, index));
}

template <typename Func, if_t< is_callable<Func>::value > >
inline void world::get(const Func& func) const {
    static_assert(arity<Func>::value == 1, "singleton component must be the only argument");
    _::entity_with_invoker<Func>::invoke_get(
        this->m_world, this->singleton<first_arg_t<Func>>(), func);
}

template <typename Func, if_t< is_callable<Func>::value > >
inline void world::set(const Func& func) const {
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
inline flecs::scoped_world world::scope(id_t parent) const {
    return scoped_world(m_world, parent);
}

template <typename T>
inline flecs::scoped_world world::scope() const {
    flecs::id_t parent = _::cpp_type<T>::id(m_world);
    return scoped_world(m_world, parent);
}

} // namespace flecs


/**
 * @defgroup cpp_core Core
 * @brief Core ECS functionality (entities, storage, queries)
 * 
 * @{
 * @}
 */

/**
 * @defgroup cpp_addons Addons
 * @brief C++ APIs for addons.
 * 
 * @{
 * @}
 */

/** @} */

#endif // __cplusplus

#endif // FLECS_CPP

#endif


#endif

